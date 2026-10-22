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
	אם(plugin->config.deny_special_chars == true){
		/* Check whether the client id or username contains a +, # or / and if
		* so deny access.
		*
		* Do this check for every message regardless, we have to protect the
		* plugins against possible pattern based attacks.
		*/
		אם(username && strpbrk(username, "+#/")){
			log__printf(عدم, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous username \"%s\"", username);
			החזר MOSQ_ERR_ACL_DENIED;
		}
		אם(context->id && strpbrk(context->id, "+#/")){
			log__printf(عدم, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous client id \"%s\"", context->id);
			החזר MOSQ_ERR_ACL_DENIED;
		}
	}

	אם(plugin->lib.version == 4){
		אם(access == MOSQ_ACL_UNSUBSCRIBE){
			החזר MOSQ_ERR_SUCCESS;
		}
	}אחרת אם(plugin->lib.version == 3){
		אם(access == MOSQ_ACL_UNSUBSCRIBE){
			החזר MOSQ_ERR_SUCCESS;
		}
	}אחרת אם(plugin->lib.version == 2){
		אם(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			החזר MOSQ_ERR_SUCCESS;
		}
	}
	החזר MOSQ_ERR_PLUGIN_DEFER;
}


ساكن صحيح acl__check_dollar(ثابت حرف *topic, صحيح access)
{
	صحيح rc;
	bool match = false;

	אם(topic[0] != '$'){
		החזר MOSQ_ERR_SUCCESS;
	}

	אם(!strncmp(topic, "$SYS", 4)){
		אם(access == MOSQ_ACL_WRITE){
			/* Potentially allow write access for bridge status, otherwise explicitly deny. */
			rc = mosquitto_topic_matches_sub("$SYS/broker/connection/+/state", topic, &match);
			אם(rc == MOSQ_ERR_SUCCESS && match == true){
				החזר MOSQ_ERR_SUCCESS;
			}אחרת{
				החזר MOSQ_ERR_ACL_DENIED;
			}
		}אחרת{
			החזר MOSQ_ERR_SUCCESS;
		}
	}אחרת אם(!strncmp(topic, "$share", 6)){
		/* Only allow sub/unsub to shared subscriptions */
		אם(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			החזר MOSQ_ERR_SUCCESS;
		}אחרת{
			החזר MOSQ_ERR_ACL_DENIED;
		}
	}אחרת{
		/* This is an unknown $ topic, for the moment just defer to actual tests. */
		החזר MOSQ_ERR_SUCCESS;
	}
}


ساكن صحيح plugin__acl_check(بنية mosquitto__security_options *opts, بنية mosquitto *context, ثابت حرف *topic, uint32_t payloadlen, فراغ *payload, uint8_t qos, bool retain, mosquitto_property *properties, صحيح access)
{
	صحيح rc = MOSQ_ERR_PLUGIN_DEFER;
	بنية mosquitto_acl_msg msg;
	بنية mosquitto__callback *cb_base, *cb_next;
	بنية mosquitto_evt_acl_check event_data;

	memset(&msg, 0, حجم(msg));
	msg.topic = topic;
	msg.payloadlen = payloadlen;
	msg.payload = payload;
	msg.qos = qos;
	msg.retain = retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.acl_check, cb_base, cb_next){
		/* FIXME - username deny special chars */

		memset(&event_data, 0, حجم(event_data));
		event_data.client = context;
		event_data.access = access;
		event_data.topic = topic;
		event_data.payloadlen = payloadlen;
		event_data.payload = payload;
		event_data.qos = qos;
		event_data.retain = retain;
		event_data.properties = properties;
		rc = cb_base->cb(MOSQ_EVT_ACL_CHECK, &event_data, cb_base->userdata);
		אם(rc != MOSQ_ERR_PLUGIN_DEFER && rc != MOSQ_ERR_PLUGIN_IGNORE){
			החזר rc;
		}
	}

	החזר rc;
}


صحيح mosquitto_acl_check(بنية mosquitto *context, ثابت حرف *topic, uint32_t payloadlen, فراغ *payload, uint8_t qos, bool retain, mosquitto_property *properties, صحيح access)
{
	صحيح rc;
	صحيح rc_final;

	אם(!context->id){
		החזר MOSQ_ERR_ACL_DENIED;
	}
	אם(context->bridge){
		החזר MOSQ_ERR_SUCCESS;
	}

	rc = acl__check_dollar(topic, access);
	אם(rc){
		החזר rc;
	}

	/*
	 * If no plugins exist we should accept at this point so set rc to success.
	 */
	rc_final = MOSQ_ERR_SUCCESS;

	אם(db.config->security_options.plugin_callbacks.acl_check){
		rc = plugin__acl_check(&db.config->security_options, context, topic, payloadlen,
				payload, qos, retain, properties, access);

		אם(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}אחרת אם(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}אחרת{
			החזר rc;
		}
	}

	אם(context->listener){
		אם(context->listener->security_options->plugin_callbacks.acl_check){
			rc = plugin__acl_check(context->listener->security_options, context, topic, payloadlen,
					payload, qos, retain, properties, access);

			אם(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing, this is as if the plugin doesn't exist */
			}אחרת אם(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}אחרת{
				החזר rc;
			}
		}
	}אחרת{
		אם(db.config->per_listener_settings){
			החזר MOSQ_ERR_ACL_DENIED;
		}
	}

	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured, or all plugins ignored. */
	אם(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_ACL_DENIED;
	}
	החזר rc_final;
}
