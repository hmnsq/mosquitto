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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/broker.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/broker_plugin.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "lib_load.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utlist.h"


ਪੂਰਨ acl__pre_check(mosquitto_plugin_id_t *plugin, ਬਣਤਰ mosquitto *context, ਪੂਰਨ access)
{
	ਅਚਲ ਅੱਖਰ *username;

	username = mosquitto_client_username(context);
	ਜੇ(plugin->config.deny_special_chars == true){
		/* Check whether the client id or username contains a +, # or / and if
		* so deny access.
		*
		* Do this check for every message regardless, we have to protect the
		* plugins against possible pattern based attacks.
		*/
		ਜੇ(username && strpbrk(username, "+#/")){
			log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous username \"%s\"", username);
			ਵਾਪਸ MOSQ_ERR_ACL_DENIED;
		}
		ਜੇ(context->id && strpbrk(context->id, "+#/")){
			log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous client id \"%s\"", context->id);
			ਵਾਪਸ MOSQ_ERR_ACL_DENIED;
		}
	}

	ਜੇ(plugin->lib.ਸੰਸਕਰਣ_2 == 4){
		ਜੇ(access == MOSQ_ACL_UNSUBSCRIBE){
			ਵਾਪਸ MOSQ_ERR_SUCCESS;
		}
	}ਨਹੀਂਤਾਂ ਜੇ(plugin->lib.ਸੰਸਕਰਣ_2 == 3){
		ਜੇ(access == MOSQ_ACL_UNSUBSCRIBE){
			ਵਾਪਸ MOSQ_ERR_SUCCESS;
		}
	}ਨਹੀਂਤਾਂ ਜੇ(plugin->lib.ਸੰਸਕਰਣ_2 == 2){
		ਜੇ(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			ਵਾਪਸ MOSQ_ERR_SUCCESS;
		}
	}
	ਵਾਪਸ MOSQ_ERR_PLUGIN_DEFER;
}


ਸਥਿਰ ਪੂਰਨ acl__check_dollar(ਅਚਲ ਅੱਖਰ *topic, ਪੂਰਨ access)
{
	ਪੂਰਨ rc;
	bool ਮੇਲ = false;

	ਜੇ(topic[0] != '$'){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

	ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(topic, "$SYS", 4)){
		ਜੇ(access == MOSQ_ACL_WRITE){
			/* Potentially allow write access for bridge status, otherwise explicitly deny. */
			rc = mosquitto_topic_matches_sub("$SYS/broker/connection/+/state", topic, &ਮੇਲ);
			ਜੇ(rc == MOSQ_ERR_SUCCESS && ਮੇਲ == true){
				ਵਾਪਸ MOSQ_ERR_SUCCESS;
			}ਨਹੀਂਤਾਂ{
				ਵਾਪਸ MOSQ_ERR_ACL_DENIED;
			}
		}ਨਹੀਂਤਾਂ{
			ਵਾਪਸ MOSQ_ERR_SUCCESS;
		}
	}ਨਹੀਂਤਾਂ ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(topic, "$share", 6)){
		/* Only allow sub/unsub to shared subscriptions */
		ਜੇ(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			ਵਾਪਸ MOSQ_ERR_SUCCESS;
		}ਨਹੀਂਤਾਂ{
			ਵਾਪਸ MOSQ_ERR_ACL_DENIED;
		}
	}ਨਹੀਂਤਾਂ{
		/* This is an unknown $ topic, for the moment just defer to actual tests. */
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}
}


ਸਥਿਰ ਪੂਰਨ plugin__acl_check(ਬਣਤਰ mosquitto__security_options *opts, ਬਣਤਰ mosquitto *context, ਅਚਲ ਅੱਖਰ *topic, uint32_t payloadlen, ਖਾਲੀ *payload, uint8_t qos, bool retain, mosquitto_property *properties, ਪੂਰਨ access)
{
	ਪੂਰਨ rc = MOSQ_ERR_PLUGIN_DEFER;
	ਬਣਤਰ mosquitto_acl_ਸੁਨੇਹਾ ਸੁਨੇਹਾ;
	ਬਣਤਰ mosquitto__callback *cb_ਅਧਾਰ, *cb_ਅਗਲਾ;
	ਬਣਤਰ mosquitto_evt_acl_check event_ਅੰਕੜੇ;

	memset(&ਸੁਨੇਹਾ, 0, ਆਕਾਰ(ਸੁਨੇਹਾ));
	ਸੁਨੇਹਾ.topic = topic;
	ਸੁਨੇਹਾ.payloadlen = payloadlen;
	ਸੁਨੇਹਾ.payload = payload;
	ਸੁਨੇਹਾ.qos = qos;
	ਸੁਨੇਹਾ.retain = retain;

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(opts->plugin_callbacks.acl_check, cb_ਅਧਾਰ, cb_ਅਗਲਾ){
		/* FIXME - username deny special chars */

		memset(&event_ਅੰਕੜੇ, 0, ਆਕਾਰ(event_ਅੰਕੜੇ));
		event_ਅੰਕੜੇ.client = context;
		event_ਅੰਕੜੇ.access = access;
		event_ਅੰਕੜੇ.topic = topic;
		event_ਅੰਕੜੇ.payloadlen = payloadlen;
		event_ਅੰਕੜੇ.payload = payload;
		event_ਅੰਕੜੇ.qos = qos;
		event_ਅੰਕੜੇ.retain = retain;
		event_ਅੰਕੜੇ.properties = properties;
		rc = cb_ਅਧਾਰ->cb(MOSQ_EVT_ACL_CHECK, &event_ਅੰਕੜੇ, cb_ਅਧਾਰ->userdata);
		ਜੇ(rc != MOSQ_ERR_PLUGIN_DEFER && rc != MOSQ_ERR_PLUGIN_IGNORE){
			ਵਾਪਸ rc;
		}
	}

	ਵਾਪਸ rc;
}


ਪੂਰਨ mosquitto_acl_check(ਬਣਤਰ mosquitto *context, ਅਚਲ ਅੱਖਰ *topic, uint32_t payloadlen, ਖਾਲੀ *payload, uint8_t qos, bool retain, mosquitto_property *properties, ਪੂਰਨ access)
{
	ਪੂਰਨ rc;
	ਪੂਰਨ rc_final;

	ਜੇ(!context->id){
		ਵਾਪਸ MOSQ_ERR_ACL_DENIED;
	}
	ਜੇ(context->bridge){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

	rc = acl__check_dollar(topic, access);
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}

	/*
	 * If no plugins exist we should accept at this point so set rc to success.
	 */
	rc_final = MOSQ_ERR_SUCCESS;

	ਜੇ(db.config->security_options.plugin_callbacks.acl_check){
		rc = plugin__acl_check(&db.config->security_options, context, topic, payloadlen,
				payload, qos, retain, properties, access);

		ਜੇ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}ਨਹੀਂਤਾਂ{
			ਵਾਪਸ rc;
		}
	}

	ਜੇ(context->listener){
		ਜੇ(context->listener->security_options->plugin_callbacks.acl_check){
			rc = plugin__acl_check(context->listener->security_options, context, topic, payloadlen,
					payload, qos, retain, properties, access);

			ਜੇ(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing, this is as if the plugin doesn't exist */
			}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}ਨਹੀਂਤਾਂ{
				ਵਾਪਸ rc;
			}
		}
	}ਨਹੀਂਤਾਂ{
		ਜੇ(db.config->ਪ੍ਰਤੀ_listener_settings){
			ਵਾਪਸ MOSQ_ERR_ACL_DENIED;
		}
	}

	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured, or all plugins ignored. */
	ਜੇ(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_ACL_DENIED;
	}
	ਵਾਪਸ rc_final;
}
