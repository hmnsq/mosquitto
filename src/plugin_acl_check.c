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


ಪೂರ್ಣಾಂಕ acl__pre_check(mosquitto_plugin_id_t *plugin, ರಚನೆ mosquitto *context, ಪೂರ್ಣಾಂಕ access)
{
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *username;

	username = mosquitto_client_username(context);
	ಆದರೆ(plugin->config.deny_special_chars == true){
		/* Check whether the client id or username contains a +, # or / and if
		* so deny access.
		*
		* Do this check for every message regardless, we have to protect the
		* plugins against possible pattern based attacks.
		*/
		ಆದರೆ(username && strpbrk(username, "+#/")){
			log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous username \"%s\"", username);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_ACL_DENIED;
		}
		ಆದರೆ(context->id && strpbrk(context->id, "+#/")){
			log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous client id \"%s\"", context->id);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_ACL_DENIED;
		}
	}

	ಆದರೆ(plugin->lib.ಆವೃತ್ತಿ_2 == 4){
		ಆದರೆ(access == MOSQ_ACL_UNSUBSCRIBE){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
		}
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(plugin->lib.ಆವೃತ್ತಿ_2 == 3){
		ಆದರೆ(access == MOSQ_ACL_UNSUBSCRIBE){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
		}
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(plugin->lib.ಆವೃತ್ತಿ_2 == 2){
		ಆದರೆ(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
		}
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_PLUGIN_DEFER;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ acl__check_dollar(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic, ಪೂರ್ಣಾಂಕ access)
{
	ಪೂರ್ಣಾಂಕ rc;
	bool ಹೊಂದಿಕೆ = false;

	ಆದರೆ(topic[0] != '$'){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}

	ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(topic, "$SYS", 4)){
		ಆದರೆ(access == MOSQ_ACL_WRITE){
			/* Potentially allow write access for bridge status, otherwise explicitly deny. */
			rc = mosquitto_topic_matches_sub("$SYS/broker/connection/+/state", topic, &ಹೊಂದಿಕೆ);
			ಆದರೆ(rc == MOSQ_ERR_SUCCESS && ಹೊಂದಿಕೆ == true){
				ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
			}ಇಲ್ಲದಿದ್ದರೆ{
				ಹಿಂತಿರುಗಿ MOSQ_ERR_ACL_DENIED;
			}
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
		}
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(topic, "$share", 6)){
		/* Only allow sub/unsub to shared subscriptions */
		ಆದರೆ(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಹಿಂತಿರುಗಿ MOSQ_ERR_ACL_DENIED;
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		/* This is an unknown $ topic, for the moment just defer to actual tests. */
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ plugin__acl_check(ರಚನೆ mosquitto__security_options *opts, ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic, uint32_t payloadlen, ಶೂನ್ಯ *payload, uint8_t qos, bool retain, mosquitto_property *properties, ಪೂರ್ಣಾಂಕ access)
{
	ಪೂರ್ಣಾಂಕ rc = MOSQ_ERR_PLUGIN_DEFER;
	ರಚನೆ mosquitto_acl_ಸಂದೇಶ ಸಂದೇಶ;
	ರಚನೆ mosquitto__callback *cb_ತಳ, *cb_ಮುಂದಿನ;
	ರಚನೆ mosquitto_evt_acl_check event_ದತ್ತಾಂಶ;

	memset(&ಸಂದೇಶ, 0, ಗಾತ್ರ(ಸಂದೇಶ));
	ಸಂದೇಶ.topic = topic;
	ಸಂದೇಶ.payloadlen = payloadlen;
	ಸಂದೇಶ.payload = payload;
	ಸಂದೇಶ.qos = qos;
	ಸಂದೇಶ.retain = retain;

	DL_FOREACH_ಸುರಕ್ಷಿತ(opts->plugin_callbacks.acl_check, cb_ತಳ, cb_ಮುಂದಿನ){
		/* FIXME - username deny special chars */

		memset(&event_ದತ್ತಾಂಶ, 0, ಗಾತ್ರ(event_ದತ್ತಾಂಶ));
		event_ದತ್ತಾಂಶ.client = context;
		event_ದತ್ತಾಂಶ.access = access;
		event_ದತ್ತಾಂಶ.topic = topic;
		event_ದತ್ತಾಂಶ.payloadlen = payloadlen;
		event_ದತ್ತಾಂಶ.payload = payload;
		event_ದತ್ತಾಂಶ.qos = qos;
		event_ದತ್ತಾಂಶ.retain = retain;
		event_ದತ್ತಾಂಶ.properties = properties;
		rc = cb_ತಳ->cb(MOSQ_EVT_ACL_CHECK, &event_ದತ್ತಾಂಶ, cb_ತಳ->userdata);
		ಆದರೆ(rc != MOSQ_ERR_PLUGIN_DEFER && rc != MOSQ_ERR_PLUGIN_IGNORE){
			ಹಿಂತಿರುಗಿ rc;
		}
	}

	ಹಿಂತಿರುಗಿ rc;
}


ಪೂರ್ಣಾಂಕ mosquitto_acl_check(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic, uint32_t payloadlen, ಶೂನ್ಯ *payload, uint8_t qos, bool retain, mosquitto_property *properties, ಪೂರ್ಣಾಂಕ access)
{
	ಪೂರ್ಣಾಂಕ rc;
	ಪೂರ್ಣಾಂಕ rc_final;

	ಆದರೆ(!context->id){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_ACL_DENIED;
	}
	ಆದರೆ(context->bridge){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}

	rc = acl__check_dollar(topic, access);
	ಆದರೆ(rc){
		ಹಿಂತಿರುಗಿ rc;
	}

	/*
	 * If no plugins exist we should accept at this point so set rc to success.
	 */
	rc_final = MOSQ_ERR_SUCCESS;

	ಆದರೆ(db.config->security_options.plugin_callbacks.acl_check){
		rc = plugin__acl_check(&db.config->security_options, context, topic, payloadlen,
				payload, qos, retain, properties, access);

		ಆದರೆ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಹಿಂತಿರುಗಿ rc;
		}
	}

	ಆದರೆ(context->listener){
		ಆದರೆ(context->listener->security_options->plugin_callbacks.acl_check){
			rc = plugin__acl_check(context->listener->security_options, context, topic, payloadlen,
					payload, qos, retain, properties, access);

			ಆದರೆ(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing, this is as if the plugin doesn't exist */
			}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}ಇಲ್ಲದಿದ್ದರೆ{
				ಹಿಂತಿರುಗಿ rc;
			}
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಆದರೆ(db.config->ಪ್ರತಿ_listener_settings){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_ACL_DENIED;
		}
	}

	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured, or all plugins ignored. */
	ಆದರೆ(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_ACL_DENIED;
	}
	ಹಿಂತಿರುಗಿ rc_final;
}
