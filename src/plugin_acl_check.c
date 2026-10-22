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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "mosquitto/broker.h"
#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/broker_plugin.h"
#ಅನ್ತರ್ಭಾವ "lib_load.h"
#ಅನ್ತರ್ಭಾವ "utlist.h"


ಪೂರ್ಣಾಙ್ಕ acl__pre_check(mosquitto_plugin_id_t *plugin, ಸಂರಚನಾ mosquitto *context, ಪೂರ್ಣಾಙ್ಕ access)
{
	ಧ್ರುವ ವರ್ಣ *username;

	username = mosquitto_client_username(context);
	ಯದಿ(plugin->config.deny_special_chars == true){
		/* Check whether the client id or username contains a +, # or / and if
		* so deny access.
		*
		* Do this check for every message regardless, we have to protect the
		* plugins against possible pattern based attacks.
		*/
		ಯದಿ(username && strpbrk(username, "+#/")){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous username \"%s\"", username);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_ACL_DENIED;
		}
		ಯದಿ(context->id && strpbrk(context->id, "+#/")){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous client id \"%s\"", context->id);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_ACL_DENIED;
		}
	}

	ಯದಿ(plugin->lib.version == 4){
		ಯದಿ(access == MOSQ_ACL_UNSUBSCRIBE){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
		}
	}ಅನ್ಯಥಾ ಯದಿ(plugin->lib.version == 3){
		ಯದಿ(access == MOSQ_ACL_UNSUBSCRIBE){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
		}
	}ಅನ್ಯಥಾ ಯದಿ(plugin->lib.version == 2){
		ಯದಿ(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
		}
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PLUGIN_DEFER;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ acl__check_dollar(ಧ್ರುವ ವರ್ಣ *topic, ಪೂರ್ಣಾಙ್ಕ access)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	bool match = false;

	ಯದಿ(topic[0] != '$'){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}

	ಯದಿ(!strncmp(topic, "$SYS", 4)){
		ಯದಿ(access == MOSQ_ACL_WRITE){
			/* Potentially allow write access for bridge status, otherwise explicitly deny. */
			rc = mosquitto_topic_matches_sub("$SYS/broker/connection/+/state", topic, &match);
			ಯದಿ(rc == MOSQ_ERR_SUCCESS && match == true){
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
			}ಅನ್ಯಥಾ{
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_ACL_DENIED;
			}
		}ಅನ್ಯಥಾ{
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
		}
	}ಅನ್ಯಥಾ ಯದಿ(!strncmp(topic, "$share", 6)){
		/* Only allow sub/unsub to shared subscriptions */
		ಯದಿ(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
		}ಅನ್ಯಥಾ{
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_ACL_DENIED;
		}
	}ಅನ್ಯಥಾ{
		/* This is an unknown $ topic, for the moment just defer to actual tests. */
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ plugin__acl_check(ಸಂರಚನಾ mosquitto__security_options *opts, ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ವರ್ಣ *topic, uint32_t payloadlen, ಶೂನ್ಯ *payload, uint8_t qos, bool retain, mosquitto_property *properties, ಪೂರ್ಣಾಙ್ಕ access)
{
	ಪೂರ್ಣಾಙ್ಕ rc = MOSQ_ERR_PLUGIN_DEFER;
	ಸಂರಚನಾ mosquitto_acl_ಸಂದೇಶ ಸಂದೇಶ;
	ಸಂರಚನಾ mosquitto__callback *cb_base, *cb_next;
	ಸಂರಚನಾ mosquitto_evt_acl_check event_data;

	memset(&ಸಂದೇಶ, 0, ಪರಿಮಾಣ(ಸಂದೇಶ));
	ಸಂದೇಶ.topic = topic;
	ಸಂದೇಶ.payloadlen = payloadlen;
	ಸಂದೇಶ.payload = payload;
	ಸಂದೇಶ.qos = qos;
	ಸಂದೇಶ.retain = retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.acl_check, cb_base, cb_next){
		/* FIXME - username deny special chars */

		memset(&event_data, 0, ಪರಿಮಾಣ(event_data));
		event_data.client = context;
		event_data.access = access;
		event_data.topic = topic;
		event_data.payloadlen = payloadlen;
		event_data.payload = payload;
		event_data.qos = qos;
		event_data.retain = retain;
		event_data.properties = properties;
		rc = cb_base->cb(MOSQ_EVT_ACL_CHECK, &event_data, cb_base->userdata);
		ಯದಿ(rc != MOSQ_ERR_PLUGIN_DEFER && rc != MOSQ_ERR_PLUGIN_IGNORE){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}

	ಪ್ರತ್ಯಾಗಮ rc;
}


ಪೂರ್ಣಾಙ್ಕ mosquitto_acl_check(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ವರ್ಣ *topic, uint32_t payloadlen, ಶೂನ್ಯ *payload, uint8_t qos, bool retain, mosquitto_property *properties, ಪೂರ್ಣಾಙ್ಕ access)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಪೂರ್ಣಾಙ್ಕ rc_final;

	ಯದಿ(!context->id){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_ACL_DENIED;
	}
	ಯದಿ(context->bridge){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}

	rc = acl__check_dollar(topic, access);
	ಯದಿ(rc){
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	/*
	 * If no plugins exist we should accept at this point so set rc to success.
	 */
	rc_final = MOSQ_ERR_SUCCESS;

	ಯದಿ(db.config->security_options.plugin_callbacks.acl_check){
		rc = plugin__acl_check(&db.config->security_options, context, topic, payloadlen,
				payload, qos, retain, properties, access);

		ಯದಿ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}ಅನ್ಯಥಾ{
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}

	ಯದಿ(context->listener){
		ಯದಿ(context->listener->security_options->plugin_callbacks.acl_check){
			rc = plugin__acl_check(context->listener->security_options, context, topic, payloadlen,
					payload, qos, retain, properties, access);

			ಯದಿ(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing, this is as if the plugin doesn't exist */
			}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}ಅನ್ಯಥಾ{
				ಪ್ರತ್ಯಾಗಮ rc;
			}
		}
	}ಅನ್ಯಥಾ{
		ಯದಿ(db.config->per_listener_settings){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_ACL_DENIED;
		}
	}

	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured, or all plugins ignored. */
	ಯದಿ(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_ACL_DENIED;
	}
	ಪ್ರತ್ಯಾಗಮ rc_final;
}
