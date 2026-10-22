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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>

#அந்தர்பாவ "mosquitto/broker.h"
#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto/broker_plugin.h"
#அந்தர்பாவ "lib_load.h"
#அந்தர்பாவ "utlist.h"


முழு acl__pre_check(mosquitto_plugin_id_t *plugin, கட்டமைப்பு mosquitto *context, முழு access)
{
	மாறிலி எழுத்து *username;

	username = mosquitto_client_username(context);
	எனில்(plugin->config.deny_special_chars == true){
		/* Check whether the client id or username contains a +, # or / and if
		* so deny access.
		*
		* Do this check for every message regardless, we have to protect the
		* plugins against possible pattern based attacks.
		*/
		எனில்(username && strpbrk(username, "+#/")){
			log__printf(வெற்று, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous username \"%s\"", username);
			திரும்பு MOSQ_ERR_ACL_DENIED;
		}
		எனில்(context->id && strpbrk(context->id, "+#/")){
			log__printf(வெற்று, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous client id \"%s\"", context->id);
			திரும்பு MOSQ_ERR_ACL_DENIED;
		}
	}

	எனில்(plugin->lib.version == 4){
		எனில்(access == MOSQ_ACL_UNSUBSCRIBE){
			திரும்பு MOSQ_ERR_SUCCESS;
		}
	}இல்லையெனில் எனில்(plugin->lib.version == 3){
		எனில்(access == MOSQ_ACL_UNSUBSCRIBE){
			திரும்பு MOSQ_ERR_SUCCESS;
		}
	}இல்லையெனில் எனில்(plugin->lib.version == 2){
		எனில்(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			திரும்பு MOSQ_ERR_SUCCESS;
		}
	}
	திரும்பு MOSQ_ERR_PLUGIN_DEFER;
}


நிலையான முழு acl__check_dollar(மாறிலி எழுத்து *topic, முழு access)
{
	முழு rc;
	bool match = false;

	எனில்(topic[0] != '$'){
		திரும்பு MOSQ_ERR_SUCCESS;
	}

	எனில்(!strncmp(topic, "$SYS", 4)){
		எனில்(access == MOSQ_ACL_WRITE){
			/* Potentially allow write access for bridge status, otherwise explicitly deny. */
			rc = mosquitto_topic_matches_sub("$SYS/broker/connection/+/state", topic, &match);
			எனில்(rc == MOSQ_ERR_SUCCESS && match == true){
				திரும்பு MOSQ_ERR_SUCCESS;
			}இல்லையெனில்{
				திரும்பு MOSQ_ERR_ACL_DENIED;
			}
		}இல்லையெனில்{
			திரும்பு MOSQ_ERR_SUCCESS;
		}
	}இல்லையெனில் எனில்(!strncmp(topic, "$share", 6)){
		/* Only allow sub/unsub to shared subscriptions */
		எனில்(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			திரும்பு MOSQ_ERR_SUCCESS;
		}இல்லையெனில்{
			திரும்பு MOSQ_ERR_ACL_DENIED;
		}
	}இல்லையெனில்{
		/* This is an unknown $ topic, for the moment just defer to actual tests. */
		திரும்பு MOSQ_ERR_SUCCESS;
	}
}


நிலையான முழு plugin__acl_check(கட்டமைப்பு mosquitto__security_options *opts, கட்டமைப்பு mosquitto *context, மாறிலி எழுத்து *topic, uint32_t payloadlen, வெறுமை *payload, uint8_t qos, bool retain, mosquitto_property *properties, முழு access)
{
	முழு rc = MOSQ_ERR_PLUGIN_DEFER;
	கட்டமைப்பு mosquitto_acl_செய்தி செய்தி;
	கட்டமைப்பு mosquitto__callback *cb_base, *cb_next;
	கட்டமைப்பு mosquitto_evt_acl_check event_data;

	memset(&செய்தி, 0, அளவு(செய்தி));
	செய்தி.topic = topic;
	செய்தி.payloadlen = payloadlen;
	செய்தி.payload = payload;
	செய்தி.qos = qos;
	செய்தி.retain = retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.acl_check, cb_base, cb_next){
		/* FIXME - username deny special chars */

		memset(&event_data, 0, அளவு(event_data));
		event_data.client = context;
		event_data.access = access;
		event_data.topic = topic;
		event_data.payloadlen = payloadlen;
		event_data.payload = payload;
		event_data.qos = qos;
		event_data.retain = retain;
		event_data.properties = properties;
		rc = cb_base->cb(MOSQ_EVT_ACL_CHECK, &event_data, cb_base->userdata);
		எனில்(rc != MOSQ_ERR_PLUGIN_DEFER && rc != MOSQ_ERR_PLUGIN_IGNORE){
			திரும்பு rc;
		}
	}

	திரும்பு rc;
}


முழு mosquitto_acl_check(கட்டமைப்பு mosquitto *context, மாறிலி எழுத்து *topic, uint32_t payloadlen, வெறுமை *payload, uint8_t qos, bool retain, mosquitto_property *properties, முழு access)
{
	முழு rc;
	முழு rc_final;

	எனில்(!context->id){
		திரும்பு MOSQ_ERR_ACL_DENIED;
	}
	எனில்(context->bridge){
		திரும்பு MOSQ_ERR_SUCCESS;
	}

	rc = acl__check_dollar(topic, access);
	எனில்(rc){
		திரும்பு rc;
	}

	/*
	 * If no plugins exist we should accept at this point so set rc to success.
	 */
	rc_final = MOSQ_ERR_SUCCESS;

	எனில்(db.config->security_options.plugin_callbacks.acl_check){
		rc = plugin__acl_check(&db.config->security_options, context, topic, payloadlen,
				payload, qos, retain, properties, access);

		எனில்(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}இல்லையெனில் எனில்(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}இல்லையெனில்{
			திரும்பு rc;
		}
	}

	எனில்(context->listener){
		எனில்(context->listener->security_options->plugin_callbacks.acl_check){
			rc = plugin__acl_check(context->listener->security_options, context, topic, payloadlen,
					payload, qos, retain, properties, access);

			எனில்(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing, this is as if the plugin doesn't exist */
			}இல்லையெனில் எனில்(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}இல்லையெனில்{
				திரும்பு rc;
			}
		}
	}இல்லையெனில்{
		எனில்(db.config->per_listener_settings){
			திரும்பு MOSQ_ERR_ACL_DENIED;
		}
	}

	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured, or all plugins ignored. */
	எனில்(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_ACL_DENIED;
	}
	திரும்பு rc_final;
}
