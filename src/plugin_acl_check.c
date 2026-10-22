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


صحیح acl__pre_check(mosquitto_plugin_id_t *plugin, ساختار mosquitto *context, صحیح access)
{
	ثابت نویسه *username;

	username = mosquitto_client_username(context);
	اگر(plugin->config.deny_special_chars == true){
		/* Check whether the client id or username contains a +, # or / and if
		* so deny access.
		*
		* Do this check for every message regardless, we have to protect the
		* plugins against possible pattern based attacks.
		*/
		اگر(username && strpbrk(username, "+#/")){
			log__printf(پوچ, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous username \"%s\"", username);
			بازگشت MOSQ_ERR_ACL_DENIED;
		}
		اگر(context->id && strpbrk(context->id, "+#/")){
			log__printf(پوچ, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous client id \"%s\"", context->id);
			بازگشت MOSQ_ERR_ACL_DENIED;
		}
	}

	اگر(plugin->lib.version == 4){
		اگر(access == MOSQ_ACL_UNSUBSCRIBE){
			بازگشت MOSQ_ERR_SUCCESS;
		}
	}وگرنه اگر(plugin->lib.version == 3){
		اگر(access == MOSQ_ACL_UNSUBSCRIBE){
			بازگشت MOSQ_ERR_SUCCESS;
		}
	}وگرنه اگر(plugin->lib.version == 2){
		اگر(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			بازگشت MOSQ_ERR_SUCCESS;
		}
	}
	بازگشت MOSQ_ERR_PLUGIN_DEFER;
}


ایستا صحیح acl__check_dollar(ثابت نویسه *topic, صحیح access)
{
	صحیح rc;
	bool match = false;

	اگر(topic[0] != '$'){
		بازگشت MOSQ_ERR_SUCCESS;
	}

	اگر(!strncmp(topic, "$SYS", 4)){
		اگر(access == MOSQ_ACL_WRITE){
			/* Potentially allow write access for bridge status, otherwise explicitly deny. */
			rc = mosquitto_topic_matches_sub("$SYS/broker/connection/+/state", topic, &match);
			اگر(rc == MOSQ_ERR_SUCCESS && match == true){
				بازگشت MOSQ_ERR_SUCCESS;
			}وگرنه{
				بازگشت MOSQ_ERR_ACL_DENIED;
			}
		}وگرنه{
			بازگشت MOSQ_ERR_SUCCESS;
		}
	}وگرنه اگر(!strncmp(topic, "$share", 6)){
		/* Only allow sub/unsub to shared subscriptions */
		اگر(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			بازگشت MOSQ_ERR_SUCCESS;
		}وگرنه{
			بازگشت MOSQ_ERR_ACL_DENIED;
		}
	}وگرنه{
		/* This is an unknown $ topic, for the moment just defer to actual tests. */
		بازگشت MOSQ_ERR_SUCCESS;
	}
}


ایستا صحیح plugin__acl_check(ساختار mosquitto__security_options *opts, ساختار mosquitto *context, ثابت نویسه *topic, uint32_t payloadlen, تهی *payload, uint8_t qos, bool retain, mosquitto_property *properties, صحیح access)
{
	صحیح rc = MOSQ_ERR_PLUGIN_DEFER;
	ساختار mosquitto_acl_پیام پیام;
	ساختار mosquitto__callback *cb_base, *cb_next;
	ساختار mosquitto_evt_acl_check event_data;

	memset(&پیام, 0, اندازه(پیام));
	پیام.topic = topic;
	پیام.payloadlen = payloadlen;
	پیام.payload = payload;
	پیام.qos = qos;
	پیام.retain = retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.acl_check, cb_base, cb_next){
		/* FIXME - username deny special chars */

		memset(&event_data, 0, اندازه(event_data));
		event_data.client = context;
		event_data.access = access;
		event_data.topic = topic;
		event_data.payloadlen = payloadlen;
		event_data.payload = payload;
		event_data.qos = qos;
		event_data.retain = retain;
		event_data.properties = properties;
		rc = cb_base->cb(MOSQ_EVT_ACL_CHECK, &event_data, cb_base->userdata);
		اگر(rc != MOSQ_ERR_PLUGIN_DEFER && rc != MOSQ_ERR_PLUGIN_IGNORE){
			بازگشت rc;
		}
	}

	بازگشت rc;
}


صحیح mosquitto_acl_check(ساختار mosquitto *context, ثابت نویسه *topic, uint32_t payloadlen, تهی *payload, uint8_t qos, bool retain, mosquitto_property *properties, صحیح access)
{
	صحیح rc;
	صحیح rc_final;

	اگر(!context->id){
		بازگشت MOSQ_ERR_ACL_DENIED;
	}
	اگر(context->bridge){
		بازگشت MOSQ_ERR_SUCCESS;
	}

	rc = acl__check_dollar(topic, access);
	اگر(rc){
		بازگشت rc;
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
		}وگرنه اگر(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}وگرنه{
			بازگشت rc;
		}
	}

	اگر(context->listener){
		اگر(context->listener->security_options->plugin_callbacks.acl_check){
			rc = plugin__acl_check(context->listener->security_options, context, topic, payloadlen,
					payload, qos, retain, properties, access);

			اگر(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing, this is as if the plugin doesn't exist */
			}وگرنه اگر(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}وگرنه{
				بازگشت rc;
			}
		}
	}وگرنه{
		اگر(db.config->per_listener_settings){
			بازگشت MOSQ_ERR_ACL_DENIED;
		}
	}

	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured, or all plugins ignored. */
	اگر(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_ACL_DENIED;
	}
	بازگشت rc_final;
}
