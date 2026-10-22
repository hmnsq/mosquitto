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


पूर्णाङ्क acl__pre_check(mosquitto_plugin_id_t *plugin, संरचना mosquitto *context, पूर्णाङ्क access)
{
	ध्रुव वर्ण *username;

	username = mosquitto_client_username(context);
	सचे(plugin->config.deny_special_chars == true){
		/* Check whether the client id or username contains a +, # or / and if
		* so deny access.
		*
		* Do this check for every message regardless, we have to protect the
		* plugins against possible pattern based attacks.
		*/
		सचे(username && strpbrk(username, "+#/")){
			log__printf(रिक्त, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous username \"%s\"", username);
			पटिनिवत्त MOSQ_ERR_ACL_DENIED;
		}
		सचे(context->id && strpbrk(context->id, "+#/")){
			log__printf(रिक्त, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous client id \"%s\"", context->id);
			पटिनिवत्त MOSQ_ERR_ACL_DENIED;
		}
	}

	सचे(plugin->lib.version == 4){
		सचे(access == MOSQ_ACL_UNSUBSCRIBE){
			पटिनिवत्त MOSQ_ERR_SUCCESS;
		}
	}अञ्ञथा सचे(plugin->lib.version == 3){
		सचे(access == MOSQ_ACL_UNSUBSCRIBE){
			पटिनिवत्त MOSQ_ERR_SUCCESS;
		}
	}अञ्ञथा सचे(plugin->lib.version == 2){
		सचे(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			पटिनिवत्त MOSQ_ERR_SUCCESS;
		}
	}
	पटिनिवत्त MOSQ_ERR_PLUGIN_DEFER;
}


स्थिर पूर्णाङ्क acl__check_dollar(ध्रुव वर्ण *topic, पूर्णाङ्क access)
{
	पूर्णाङ्क rc;
	bool match = false;

	सचे(topic[0] != '$'){
		पटिनिवत्त MOSQ_ERR_SUCCESS;
	}

	सचे(!strncmp(topic, "$SYS", 4)){
		सचे(access == MOSQ_ACL_WRITE){
			/* Potentially allow write access for bridge status, otherwise explicitly deny. */
			rc = mosquitto_topic_matches_sub("$SYS/broker/connection/+/state", topic, &match);
			सचे(rc == MOSQ_ERR_SUCCESS && match == true){
				पटिनिवत्त MOSQ_ERR_SUCCESS;
			}अञ्ञथा{
				पटिनिवत्त MOSQ_ERR_ACL_DENIED;
			}
		}अञ्ञथा{
			पटिनिवत्त MOSQ_ERR_SUCCESS;
		}
	}अञ्ञथा सचे(!strncmp(topic, "$share", 6)){
		/* Only allow sub/unsub to shared subscriptions */
		सचे(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			पटिनिवत्त MOSQ_ERR_SUCCESS;
		}अञ्ञथा{
			पटिनिवत्त MOSQ_ERR_ACL_DENIED;
		}
	}अञ्ञथा{
		/* This is an unknown $ topic, for the moment just defer to actual tests. */
		पटिनिवत्त MOSQ_ERR_SUCCESS;
	}
}


स्थिर पूर्णाङ्क plugin__acl_check(संरचना mosquitto__security_options *opts, संरचना mosquitto *context, ध्रुव वर्ण *topic, uint32_t payloadlen, शून्य *payload, uint8_t qos, bool retain, mosquitto_property *properties, पूर्णाङ्क access)
{
	पूर्णाङ्क rc = MOSQ_ERR_PLUGIN_DEFER;
	संरचना mosquitto_acl_सन्देशः सन्देशः;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto_evt_acl_check event_data;

	memset(&सन्देशः, 0, परिमाण(सन्देशः));
	सन्देशः.topic = topic;
	सन्देशः.payloadlen = payloadlen;
	सन्देशः.payload = payload;
	सन्देशः.qos = qos;
	सन्देशः.retain = retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.acl_check, cb_base, cb_next){
		/* FIXME - username deny special chars */

		memset(&event_data, 0, परिमाण(event_data));
		event_data.client = context;
		event_data.access = access;
		event_data.topic = topic;
		event_data.payloadlen = payloadlen;
		event_data.payload = payload;
		event_data.qos = qos;
		event_data.retain = retain;
		event_data.properties = properties;
		rc = cb_base->cb(MOSQ_EVT_ACL_CHECK, &event_data, cb_base->userdata);
		सचे(rc != MOSQ_ERR_PLUGIN_DEFER && rc != MOSQ_ERR_PLUGIN_IGNORE){
			पटिनिवत्त rc;
		}
	}

	पटिनिवत्त rc;
}


पूर्णाङ्क mosquitto_acl_check(संरचना mosquitto *context, ध्रुव वर्ण *topic, uint32_t payloadlen, शून्य *payload, uint8_t qos, bool retain, mosquitto_property *properties, पूर्णाङ्क access)
{
	पूर्णाङ्क rc;
	पूर्णाङ्क rc_final;

	सचे(!context->id){
		पटिनिवत्त MOSQ_ERR_ACL_DENIED;
	}
	सचे(context->bridge){
		पटिनिवत्त MOSQ_ERR_SUCCESS;
	}

	rc = acl__check_dollar(topic, access);
	सचे(rc){
		पटिनिवत्त rc;
	}

	/*
	 * If no plugins exist we should accept at this point so set rc to success.
	 */
	rc_final = MOSQ_ERR_SUCCESS;

	सचे(db.config->security_options.plugin_callbacks.acl_check){
		rc = plugin__acl_check(&db.config->security_options, context, topic, payloadlen,
				payload, qos, retain, properties, access);

		सचे(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}अञ्ञथा सचे(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}अञ्ञथा{
			पटिनिवत्त rc;
		}
	}

	सचे(context->listener){
		सचे(context->listener->security_options->plugin_callbacks.acl_check){
			rc = plugin__acl_check(context->listener->security_options, context, topic, payloadlen,
					payload, qos, retain, properties, access);

			सचे(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing, this is as if the plugin doesn't exist */
			}अञ्ञथा सचे(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}अञ्ञथा{
				पटिनिवत्त rc;
			}
		}
	}अञ्ञथा{
		सचे(db.config->per_listener_settings){
			पटिनिवत्त MOSQ_ERR_ACL_DENIED;
		}
	}

	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured, or all plugins ignored. */
	सचे(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_ACL_DENIED;
	}
	पटिनिवत्त rc_final;
}
