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
	अचर वर्ण *username;

	username = mosquitto_client_username(context);
	यदि(plugin->config.deny_special_chars == true){
		/* Check whether the client id or username contains a +, # or / and if
		* so deny access.
		*
		* Do this check for every message regardless, we have to protect the
		* plugins against possible pattern based attacks.
		*/
		यदि(username && strpbrk(username, "+#/")){
			log__printf(रिक्त, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous username \"%s\"", username);
			फर्काऊ MOSQ_ERR_ACL_DENIED;
		}
		यदि(context->id && strpbrk(context->id, "+#/")){
			log__printf(रिक्त, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous client id \"%s\"", context->id);
			फर्काऊ MOSQ_ERR_ACL_DENIED;
		}
	}

	यदि(plugin->lib.version == 4){
		यदि(access == MOSQ_ACL_UNSUBSCRIBE){
			फर्काऊ MOSQ_ERR_SUCCESS;
		}
	}अन्यथा यदि(plugin->lib.version == 3){
		यदि(access == MOSQ_ACL_UNSUBSCRIBE){
			फर्काऊ MOSQ_ERR_SUCCESS;
		}
	}अन्यथा यदि(plugin->lib.version == 2){
		यदि(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			फर्काऊ MOSQ_ERR_SUCCESS;
		}
	}
	फर्काऊ MOSQ_ERR_PLUGIN_DEFER;
}


स्थिर पूर्णाङ्क acl__check_dollar(अचर वर्ण *topic, पूर्णाङ्क access)
{
	पूर्णाङ्क rc;
	bool match = false;

	यदि(topic[0] != '$'){
		फर्काऊ MOSQ_ERR_SUCCESS;
	}

	यदि(!म_भेदन(topic, "$SYS", 4)){
		यदि(access == MOSQ_ACL_WRITE){
			/* Potentially allow write access for bridge status, otherwise explicitly deny. */
			rc = mosquitto_topic_matches_sub("$SYS/broker/connection/+/state", topic, &match);
			यदि(rc == MOSQ_ERR_SUCCESS && match == true){
				फर्काऊ MOSQ_ERR_SUCCESS;
			}अन्यथा{
				फर्काऊ MOSQ_ERR_ACL_DENIED;
			}
		}अन्यथा{
			फर्काऊ MOSQ_ERR_SUCCESS;
		}
	}अन्यथा यदि(!म_भेदन(topic, "$share", 6)){
		/* Only allow sub/unsub to shared subscriptions */
		यदि(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			फर्काऊ MOSQ_ERR_SUCCESS;
		}अन्यथा{
			फर्काऊ MOSQ_ERR_ACL_DENIED;
		}
	}अन्यथा{
		/* This is an unknown $ topic, for the moment just defer to actual tests. */
		फर्काऊ MOSQ_ERR_SUCCESS;
	}
}


स्थिर पूर्णाङ्क plugin__acl_check(संरचना mosquitto__security_options *opts, संरचना mosquitto *context, अचर वर्ण *topic, uint32_t payloadlen, शून्य *payload, uint8_t qos, bool retain, mosquitto_property *properties, पूर्णाङ्क access)
{
	पूर्णाङ्क rc = MOSQ_ERR_PLUGIN_DEFER;
	संरचना mosquitto_acl_सन्देश सन्देश;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto_evt_acl_check event_data;

	स_रखो(&सन्देश, 0, परिमाण(सन्देश));
	सन्देश.topic = topic;
	सन्देश.payloadlen = payloadlen;
	सन्देश.payload = payload;
	सन्देश.qos = qos;
	सन्देश.retain = retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.acl_check, cb_base, cb_next){
		/* FIXME - username deny special chars */

		स_रखो(&event_data, 0, परिमाण(event_data));
		event_data.client = context;
		event_data.access = access;
		event_data.topic = topic;
		event_data.payloadlen = payloadlen;
		event_data.payload = payload;
		event_data.qos = qos;
		event_data.retain = retain;
		event_data.properties = properties;
		rc = cb_base->cb(MOSQ_EVT_ACL_CHECK, &event_data, cb_base->userdata);
		यदि(rc != MOSQ_ERR_PLUGIN_DEFER && rc != MOSQ_ERR_PLUGIN_IGNORE){
			फर्काऊ rc;
		}
	}

	फर्काऊ rc;
}


पूर्णाङ्क mosquitto_acl_check(संरचना mosquitto *context, अचर वर्ण *topic, uint32_t payloadlen, शून्य *payload, uint8_t qos, bool retain, mosquitto_property *properties, पूर्णाङ्क access)
{
	पूर्णाङ्क rc;
	पूर्णाङ्क rc_final;

	यदि(!context->id){
		फर्काऊ MOSQ_ERR_ACL_DENIED;
	}
	यदि(context->bridge){
		फर्काऊ MOSQ_ERR_SUCCESS;
	}

	rc = acl__check_dollar(topic, access);
	यदि(rc){
		फर्काऊ rc;
	}

	/*
	 * If no plugins exist we should accept at this point so set rc to success.
	 */
	rc_final = MOSQ_ERR_SUCCESS;

	यदि(db.config->security_options.plugin_callbacks.acl_check){
		rc = plugin__acl_check(&db.config->security_options, context, topic, payloadlen,
				payload, qos, retain, properties, access);

		यदि(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}अन्यथा यदि(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}अन्यथा{
			फर्काऊ rc;
		}
	}

	यदि(context->listener){
		यदि(context->listener->security_options->plugin_callbacks.acl_check){
			rc = plugin__acl_check(context->listener->security_options, context, topic, payloadlen,
					payload, qos, retain, properties, access);

			यदि(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing, this is as if the plugin doesn't exist */
			}अन्यथा यदि(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}अन्यथा{
				फर्काऊ rc;
			}
		}
	}अन्यथा{
		यदि(db.config->per_listener_settings){
			फर्काऊ MOSQ_ERR_ACL_DENIED;
		}
	}

	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured, or all plugins ignored. */
	यदि(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_ACL_DENIED;
	}
	फर्काऊ rc_final;
}
