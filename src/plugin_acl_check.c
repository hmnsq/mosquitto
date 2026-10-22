/*
Copyright (c) 2011-2021 Roger Light <roger@atchoo.org>

All rights reserved. This program and the accompanying materials
are made available under the terms of the Eclipse Public License 2.0
and Eclipse Distribution License v1.0 which accompany this distribution.

The Eclipse Public License is available at
   https://www.eclipse.org/legal/epl-2.0/
and the Eclipse Distribution License is available at
  http://www.eclipse.org/org/documents/edl-v10.php.

SPDX-License-Identifier: EPL-2.0 OR BSD-3-Clause

Contributors:
   Roger Light - initial implementation and documentation.
*/

#अन्तर्भाव "config.h"

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "mosquitto/broker.h"
#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/broker_plugin.h"
#अन्तर्भाव "lib_load.h"
#अन्तर्भाव "utlist.h"


पूर्णांक acl__pre_check(mosquitto_plugin_id_t *plugin, संरचना mosquitto *context, पूर्णांक access)
{
	स्थिरांक अक्षर *username;

	username = mosquitto_client_username(context);
	जर(plugin->config.deny_special_chars == true){
		/* Check whether the client id or username contains a +, # or / and if
		* so deny access.
		*
		* Do this check for every message regardless, we have to protect the
		* plugins against possible pattern based attacks.
		*/
		जर(username && strpbrk(username, "+#/")){
			log__printf(रिक्त, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous username \"%s\"", username);
			परत MOSQ_ERR_ACL_DENIED;
		}
		जर(context->id && strpbrk(context->id, "+#/")){
			log__printf(रिक्त, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous client id \"%s\"", context->id);
			परत MOSQ_ERR_ACL_DENIED;
		}
	}

	जर(plugin->lib.version == 4){
		जर(access == MOSQ_ACL_UNSUBSCRIBE){
			परत MOSQ_ERR_SUCCESS;
		}
	}अन्यथा जर(plugin->lib.version == 3){
		जर(access == MOSQ_ACL_UNSUBSCRIBE){
			परत MOSQ_ERR_SUCCESS;
		}
	}अन्यथा जर(plugin->lib.version == 2){
		जर(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			परत MOSQ_ERR_SUCCESS;
		}
	}
	परत MOSQ_ERR_PLUGIN_DEFER;
}


स्थिर पूर्णांक acl__check_dollar(स्थिरांक अक्षर *topic, पूर्णांक access)
{
	पूर्णांक rc;
	bool match = false;

	जर(topic[0] != '$'){
		परत MOSQ_ERR_SUCCESS;
	}

	जर(!म_भेदन(topic, "$SYS", 4)){
		जर(access == MOSQ_ACL_WRITE){
			/* Potentially allow write access for bridge status, otherwise explicitly deny. */
			rc = mosquitto_topic_matches_sub("$SYS/broker/connection/+/state", topic, &match);
			जर(rc == MOSQ_ERR_SUCCESS && match == true){
				परत MOSQ_ERR_SUCCESS;
			}अन्यथा{
				परत MOSQ_ERR_ACL_DENIED;
			}
		}अन्यथा{
			परत MOSQ_ERR_SUCCESS;
		}
	}अन्यथा जर(!म_भेदन(topic, "$share", 6)){
		/* Only allow sub/unsub to shared subscriptions */
		जर(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			परत MOSQ_ERR_SUCCESS;
		}अन्यथा{
			परत MOSQ_ERR_ACL_DENIED;
		}
	}अन्यथा{
		/* This is an unknown $ topic, for the moment just defer to actual tests. */
		परत MOSQ_ERR_SUCCESS;
	}
}


स्थिर पूर्णांक plugin__acl_check(संरचना mosquitto__security_options *opts, संरचना mosquitto *context, स्थिरांक अक्षर *topic, uint32_t payloadlen, शून्य *payload, uint8_t qos, bool retain, mosquitto_property *properties, पूर्णांक access)
{
	पूर्णांक rc = MOSQ_ERR_PLUGIN_DEFER;
	संरचना mosquitto_acl_संदेश संदेश;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto_evt_acl_check event_data;

	स_रखो(&संदेश, 0, आकार(संदेश));
	संदेश.topic = topic;
	संदेश.payloadlen = payloadlen;
	संदेश.payload = payload;
	संदेश.qos = qos;
	संदेश.retain = retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.acl_check, cb_base, cb_next){
		/* FIXME - username deny special chars */

		स_रखो(&event_data, 0, आकार(event_data));
		event_data.client = context;
		event_data.access = access;
		event_data.topic = topic;
		event_data.payloadlen = payloadlen;
		event_data.payload = payload;
		event_data.qos = qos;
		event_data.retain = retain;
		event_data.properties = properties;
		rc = cb_base->cb(MOSQ_EVT_ACL_CHECK, &event_data, cb_base->userdata);
		जर(rc != MOSQ_ERR_PLUGIN_DEFER && rc != MOSQ_ERR_PLUGIN_IGNORE){
			परत rc;
		}
	}

	परत rc;
}


पूर्णांक mosquitto_acl_check(संरचना mosquitto *context, स्थिरांक अक्षर *topic, uint32_t payloadlen, शून्य *payload, uint8_t qos, bool retain, mosquitto_property *properties, पूर्णांक access)
{
	पूर्णांक rc;
	पूर्णांक rc_final;

	जर(!context->id){
		परत MOSQ_ERR_ACL_DENIED;
	}
	जर(context->bridge){
		परत MOSQ_ERR_SUCCESS;
	}

	rc = acl__check_dollar(topic, access);
	जर(rc){
		परत rc;
	}

	/*
	 * If no plugins exist we should accept at this point so set rc to success.
	 */
	rc_final = MOSQ_ERR_SUCCESS;

	जर(db.config->security_options.plugin_callbacks.acl_check){
		rc = plugin__acl_check(&db.config->security_options, context, topic, payloadlen,
				payload, qos, retain, properties, access);

		जर(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}अन्यथा जर(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}अन्यथा{
			परत rc;
		}
	}

	जर(context->listener){
		जर(context->listener->security_options->plugin_callbacks.acl_check){
			rc = plugin__acl_check(context->listener->security_options, context, topic, payloadlen,
					payload, qos, retain, properties, access);

			जर(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing, this is as if the plugin doesn't exist */
			}अन्यथा जर(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}अन्यथा{
				परत rc;
			}
		}
	}अन्यथा{
		जर(db.config->per_listener_settings){
			परत MOSQ_ERR_ACL_DENIED;
		}
	}

	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured, or all plugins ignored. */
	जर(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_ACL_DENIED;
	}
	परत rc_final;
}
