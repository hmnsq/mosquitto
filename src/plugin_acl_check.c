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

#شامل "config.h"

#شامل <stdio.h>
#شامل <string.h>

#شامل "mosquitto/broker.h"
#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/broker_plugin.h"
#شامل "lib_load.h"
#شامل "utlist.h"


صحیح acl__pre_check(mosquitto_plugin_id_t *plugin, ساخت mosquitto *context, صحیح access)
{
	ثابت حرف *username;

	username = mosquitto_client_username(context);
	اگر(plugin->config.deny_special_chars == true){
		/* Check whether the client id or username contains a +, # or / and if
		* so deny access.
		*
		* Do this check for every message regardless, we have to protect the
		* plugins against possible pattern based attacks.
		*/
		اگر(username && strpbrk(username, "+#/")){
			log__printf(تہی, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous username \"%s\"", username);
			واپس MOSQ_ERR_ACL_DENIED;
		}
		اگر(context->id && strpbrk(context->id, "+#/")){
			log__printf(تہی, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous client id \"%s\"", context->id);
			واپس MOSQ_ERR_ACL_DENIED;
		}
	}

	اگر(plugin->lib.نسخہ_2 == 4){
		اگر(access == MOSQ_ACL_UNSUBSCRIBE){
			واپس MOSQ_ERR_SUCCESS;
		}
	}ورنہ اگر(plugin->lib.نسخہ_2 == 3){
		اگر(access == MOSQ_ACL_UNSUBSCRIBE){
			واپس MOSQ_ERR_SUCCESS;
		}
	}ورنہ اگر(plugin->lib.نسخہ_2 == 2){
		اگر(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			واپس MOSQ_ERR_SUCCESS;
		}
	}
	واپس MOSQ_ERR_PLUGIN_DEFER;
}


ساکن صحیح acl__check_dollar(ثابت حرف *topic, صحیح access)
{
	صحیح rc;
	bool مطابقت = false;

	اگر(topic[0] != '$'){
		واپس MOSQ_ERR_SUCCESS;
	}

	اگر(!لڑی_موازنہ(topic, "$SYS", 4)){
		اگر(access == MOSQ_ACL_WRITE){
			/* Potentially allow write access for bridge status, otherwise explicitly deny. */
			rc = mosquitto_topic_matches_sub("$SYS/broker/connection/+/state", topic, &مطابقت);
			اگر(rc == MOSQ_ERR_SUCCESS && مطابقت == true){
				واپس MOSQ_ERR_SUCCESS;
			}ورنہ{
				واپس MOSQ_ERR_ACL_DENIED;
			}
		}ورنہ{
			واپس MOSQ_ERR_SUCCESS;
		}
	}ورنہ اگر(!لڑی_موازنہ(topic, "$share", 6)){
		/* Only allow sub/unsub to shared subscriptions */
		اگر(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			واپس MOSQ_ERR_SUCCESS;
		}ورنہ{
			واپس MOSQ_ERR_ACL_DENIED;
		}
	}ورنہ{
		/* This is an unknown $ topic, for the moment just defer to actual tests. */
		واپس MOSQ_ERR_SUCCESS;
	}
}


ساکن صحیح plugin__acl_check(ساخت mosquitto__security_options *opts, ساخت mosquitto *context, ثابت حرف *topic, uint32_t payloadlen, خالی *payload, uint8_t qos, bool retain, mosquitto_property *properties, صحیح access)
{
	صحیح rc = MOSQ_ERR_PLUGIN_DEFER;
	ساخت mosquitto_acl_پیغام پیغام;
	ساخت mosquitto__callback *cb_بنیاد, *cb_اگلا;
	ساخت mosquitto_evt_acl_check event_ڈیٹا;

	memset(&پیغام, 0, حجم(پیغام));
	پیغام.topic = topic;
	پیغام.payloadlen = payloadlen;
	پیغام.payload = payload;
	پیغام.qos = qos;
	پیغام.retain = retain;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.acl_check, cb_بنیاد, cb_اگلا){
		/* FIXME - username deny special chars */

		memset(&event_ڈیٹا, 0, حجم(event_ڈیٹا));
		event_ڈیٹا.client = context;
		event_ڈیٹا.access = access;
		event_ڈیٹا.topic = topic;
		event_ڈیٹا.payloadlen = payloadlen;
		event_ڈیٹا.payload = payload;
		event_ڈیٹا.qos = qos;
		event_ڈیٹا.retain = retain;
		event_ڈیٹا.properties = properties;
		rc = cb_بنیاد->cb(MOSQ_EVT_ACL_CHECK, &event_ڈیٹا, cb_بنیاد->userdata);
		اگر(rc != MOSQ_ERR_PLUGIN_DEFER && rc != MOSQ_ERR_PLUGIN_IGNORE){
			واپس rc;
		}
	}

	واپس rc;
}


صحیح mosquitto_acl_check(ساخت mosquitto *context, ثابت حرف *topic, uint32_t payloadlen, خالی *payload, uint8_t qos, bool retain, mosquitto_property *properties, صحیح access)
{
	صحیح rc;
	صحیح rc_final;

	اگر(!context->id){
		واپس MOSQ_ERR_ACL_DENIED;
	}
	اگر(context->bridge){
		واپس MOSQ_ERR_SUCCESS;
	}

	rc = acl__check_dollar(topic, access);
	اگر(rc){
		واپس rc;
	}

	/*
	 * If no plugins exist we should accept at this point so set rc to success.
	 */
	rc_final = MOSQ_ERR_SUCCESS;

	اگر(db.config->security_options.plugin_callbacks.acl_check){
		rc = plugin__acl_check(&db.config->security_options, context, topic, payloadlen,
				payload, qos, retain, properties, access);

		اگر(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}ورنہ اگر(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}ورنہ{
			واپس rc;
		}
	}

	اگر(context->listener){
		اگر(context->listener->security_options->plugin_callbacks.acl_check){
			rc = plugin__acl_check(context->listener->security_options, context, topic, payloadlen,
					payload, qos, retain, properties, access);

			اگر(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing, this is as if the plugin doesn't exist */
			}ورنہ اگر(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}ورنہ{
				واپس rc;
			}
		}
	}ورنہ{
		اگر(db.config->فی_listener_settings){
			واپس MOSQ_ERR_ACL_DENIED;
		}
	}

	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured, or all plugins ignored. */
	اگر(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_ACL_DENIED;
	}
	واپس rc_final;
}
