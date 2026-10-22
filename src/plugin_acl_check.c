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

#ضمن "config.h"

#ضمن <stdio.h>
#ضمن <string.h>

#ضمن "mosquitto/broker.h"
#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/broker_plugin.h"
#ضمن "lib_load.h"
#ضمن "utlist.h"


صحيح acl__pre_check(mosquitto_plugin_id_t *plugin, بنية mosquitto *context, صحيح access)
{
	ثابت حرف *username;

	username = mosquitto_client_username(context);
	إذا(plugin->config.deny_special_chars == true){
		/* Check whether the client id or username contains a +, # or / and if
		* so deny access.
		*
		* Do this check for every message regardless, we have to protect the
		* plugins against possible pattern based attacks.
		*/
		إذا(username && strpbrk(username, "+#/")){
			log__printf(عدم, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous username \"%s\"", username);
			أرجع MOSQ_ERR_ACL_DENIED;
		}
		إذا(context->id && strpbrk(context->id, "+#/")){
			log__printf(عدم, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous client id \"%s\"", context->id);
			أرجع MOSQ_ERR_ACL_DENIED;
		}
	}

	إذا(plugin->lib.نسخہ_2 == 4){
		إذا(access == MOSQ_ACL_UNSUBSCRIBE){
			أرجع MOSQ_ERR_SUCCESS;
		}
	}وإلا إذا(plugin->lib.نسخہ_2 == 3){
		إذا(access == MOSQ_ACL_UNSUBSCRIBE){
			أرجع MOSQ_ERR_SUCCESS;
		}
	}وإلا إذا(plugin->lib.نسخہ_2 == 2){
		إذا(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			أرجع MOSQ_ERR_SUCCESS;
		}
	}
	أرجع MOSQ_ERR_PLUGIN_DEFER;
}


ساكن صحيح acl__check_dollar(ثابت حرف *topic, صحيح access)
{
	صحيح rc;
	bool میل = false;

	إذا(topic[0] != '$'){
		أرجع MOSQ_ERR_SUCCESS;
	}

	إذا(!لڑی_مقابلہ(topic, "$SYS", 4)){
		إذا(access == MOSQ_ACL_WRITE){
			/* Potentially allow write access for bridge status, otherwise explicitly deny. */
			rc = mosquitto_topic_matches_sub("$SYS/broker/connection/+/state", topic, &میل);
			إذا(rc == MOSQ_ERR_SUCCESS && میل == true){
				أرجع MOSQ_ERR_SUCCESS;
			}وإلا{
				أرجع MOSQ_ERR_ACL_DENIED;
			}
		}وإلا{
			أرجع MOSQ_ERR_SUCCESS;
		}
	}وإلا إذا(!لڑی_مقابلہ(topic, "$share", 6)){
		/* Only allow sub/unsub to shared subscriptions */
		إذا(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			أرجع MOSQ_ERR_SUCCESS;
		}وإلا{
			أرجع MOSQ_ERR_ACL_DENIED;
		}
	}وإلا{
		/* This is an unknown $ topic, for the moment just defer to actual tests. */
		أرجع MOSQ_ERR_SUCCESS;
	}
}


ساكن صحيح plugin__acl_check(بنية mosquitto__security_options *opts, بنية mosquitto *context, ثابت حرف *topic, uint32_t payloadlen, فراغ *payload, uint8_t qos, bool retain, mosquitto_property *properties, صحيح access)
{
	صحيح rc = MOSQ_ERR_PLUGIN_DEFER;
	بنية mosquitto_acl_پیغام پیغام;
	بنية mosquitto__callback *cb_ادھار, *cb_اگلا;
	بنية mosquitto_evt_acl_check event_انکڑے;

	memset(&پیغام, 0, حجم(پیغام));
	پیغام.topic = topic;
	پیغام.payloadlen = payloadlen;
	پیغام.payload = payload;
	پیغام.qos = qos;
	پیغام.retain = retain;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.acl_check, cb_ادھار, cb_اگلا){
		/* FIXME - username deny special chars */

		memset(&event_انکڑے, 0, حجم(event_انکڑے));
		event_انکڑے.client = context;
		event_انکڑے.access = access;
		event_انکڑے.topic = topic;
		event_انکڑے.payloadlen = payloadlen;
		event_انکڑے.payload = payload;
		event_انکڑے.qos = qos;
		event_انکڑے.retain = retain;
		event_انکڑے.properties = properties;
		rc = cb_ادھار->cb(MOSQ_EVT_ACL_CHECK, &event_انکڑے, cb_ادھار->userdata);
		إذا(rc != MOSQ_ERR_PLUGIN_DEFER && rc != MOSQ_ERR_PLUGIN_IGNORE){
			أرجع rc;
		}
	}

	أرجع rc;
}


صحيح mosquitto_acl_check(بنية mosquitto *context, ثابت حرف *topic, uint32_t payloadlen, فراغ *payload, uint8_t qos, bool retain, mosquitto_property *properties, صحيح access)
{
	صحيح rc;
	صحيح rc_final;

	إذا(!context->id){
		أرجع MOSQ_ERR_ACL_DENIED;
	}
	إذا(context->bridge){
		أرجع MOSQ_ERR_SUCCESS;
	}

	rc = acl__check_dollar(topic, access);
	إذا(rc){
		أرجع rc;
	}

	/*
	 * If no plugins exist we should accept at this point so set rc to success.
	 */
	rc_final = MOSQ_ERR_SUCCESS;

	إذا(db.config->security_options.plugin_callbacks.acl_check){
		rc = plugin__acl_check(&db.config->security_options, context, topic, payloadlen,
				payload, qos, retain, properties, access);

		إذا(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}وإلا إذا(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}وإلا{
			أرجع rc;
		}
	}

	إذا(context->listener){
		إذا(context->listener->security_options->plugin_callbacks.acl_check){
			rc = plugin__acl_check(context->listener->security_options, context, topic, payloadlen,
					payload, qos, retain, properties, access);

			إذا(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing, this is as if the plugin doesn't exist */
			}وإلا إذا(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}وإلا{
				أرجع rc;
			}
		}
	}وإلا{
		إذا(db.config->فی_listener_settings){
			أرجع MOSQ_ERR_ACL_DENIED;
		}
	}

	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured, or all plugins ignored. */
	إذا(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_ACL_DENIED;
	}
	أرجع rc_final;
}
