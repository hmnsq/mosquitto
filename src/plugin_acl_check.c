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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "mosquitto/broker.h"
#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/broker_plugin.h"
#અન્તર્ભાવ "lib_load.h"
#અન્તર્ભાવ "utlist.h"


પૂર્ણાંક acl__pre_check(mosquitto_plugin_id_t *plugin, સંરચના mosquitto *context, પૂર્ણાંક access)
{
	અચલ અક્ષર *username;

	username = mosquitto_client_username(context);
	જો(plugin->config.deny_special_chars == true){
		/* Check whether the client id or username contains a +, # or / and if
		* so deny access.
		*
		* Do this check for every message regardless, we have to protect the
		* plugins against possible pattern based attacks.
		*/
		જો(username && strpbrk(username, "+#/")){
			log__printf(ખાલી, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous username \"%s\"", username);
			પાછા MOSQ_ERR_ACL_DENIED;
		}
		જો(context->id && strpbrk(context->id, "+#/")){
			log__printf(ખાલી, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous client id \"%s\"", context->id);
			પાછા MOSQ_ERR_ACL_DENIED;
		}
	}

	જો(plugin->lib.version == 4){
		જો(access == MOSQ_ACL_UNSUBSCRIBE){
			પાછા MOSQ_ERR_SUCCESS;
		}
	}નહીંતર જો(plugin->lib.version == 3){
		જો(access == MOSQ_ACL_UNSUBSCRIBE){
			પાછા MOSQ_ERR_SUCCESS;
		}
	}નહીંતર જો(plugin->lib.version == 2){
		જો(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			પાછા MOSQ_ERR_SUCCESS;
		}
	}
	પાછા MOSQ_ERR_PLUGIN_DEFER;
}


સ્થિર પૂર્ણાંક acl__check_dollar(અચલ અક્ષર *topic, પૂર્ણાંક access)
{
	પૂર્ણાંક rc;
	bool match = false;

	જો(topic[0] != '$'){
		પાછા MOSQ_ERR_SUCCESS;
	}

	જો(!strncmp(topic, "$SYS", 4)){
		જો(access == MOSQ_ACL_WRITE){
			/* Potentially allow write access for bridge status, otherwise explicitly deny. */
			rc = mosquitto_topic_matches_sub("$SYS/broker/connection/+/state", topic, &match);
			જો(rc == MOSQ_ERR_SUCCESS && match == true){
				પાછા MOSQ_ERR_SUCCESS;
			}નહીંતર{
				પાછા MOSQ_ERR_ACL_DENIED;
			}
		}નહીંતર{
			પાછા MOSQ_ERR_SUCCESS;
		}
	}નહીંતર જો(!strncmp(topic, "$share", 6)){
		/* Only allow sub/unsub to shared subscriptions */
		જો(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			પાછા MOSQ_ERR_SUCCESS;
		}નહીંતર{
			પાછા MOSQ_ERR_ACL_DENIED;
		}
	}નહીંતર{
		/* This is an unknown $ topic, for the moment just defer to actual tests. */
		પાછા MOSQ_ERR_SUCCESS;
	}
}


સ્થિર પૂર્ણાંક plugin__acl_check(સંરચના mosquitto__security_options *opts, સંરચના mosquitto *context, અચલ અક્ષર *topic, uint32_t payloadlen, શૂન્ય *payload, uint8_t qos, bool retain, mosquitto_property *properties, પૂર્ણાંક access)
{
	પૂર્ણાંક rc = MOSQ_ERR_PLUGIN_DEFER;
	સંરચના mosquitto_acl_સંદેશ સંદેશ;
	સંરચના mosquitto__callback *cb_base, *cb_next;
	સંરચના mosquitto_evt_acl_check event_data;

	memset(&સંદેશ, 0, કદ(સંદેશ));
	સંદેશ.topic = topic;
	સંદેશ.payloadlen = payloadlen;
	સંદેશ.payload = payload;
	સંદેશ.qos = qos;
	સંદેશ.retain = retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.acl_check, cb_base, cb_next){
		/* FIXME - username deny special chars */

		memset(&event_data, 0, કદ(event_data));
		event_data.client = context;
		event_data.access = access;
		event_data.topic = topic;
		event_data.payloadlen = payloadlen;
		event_data.payload = payload;
		event_data.qos = qos;
		event_data.retain = retain;
		event_data.properties = properties;
		rc = cb_base->cb(MOSQ_EVT_ACL_CHECK, &event_data, cb_base->userdata);
		જો(rc != MOSQ_ERR_PLUGIN_DEFER && rc != MOSQ_ERR_PLUGIN_IGNORE){
			પાછા rc;
		}
	}

	પાછા rc;
}


પૂર્ણાંક mosquitto_acl_check(સંરચના mosquitto *context, અચલ અક્ષર *topic, uint32_t payloadlen, શૂન્ય *payload, uint8_t qos, bool retain, mosquitto_property *properties, પૂર્ણાંક access)
{
	પૂર્ણાંક rc;
	પૂર્ણાંક rc_final;

	જો(!context->id){
		પાછા MOSQ_ERR_ACL_DENIED;
	}
	જો(context->bridge){
		પાછા MOSQ_ERR_SUCCESS;
	}

	rc = acl__check_dollar(topic, access);
	જો(rc){
		પાછા rc;
	}

	/*
	 * If no plugins exist we should accept at this point so set rc to success.
	 */
	rc_final = MOSQ_ERR_SUCCESS;

	જો(db.config->security_options.plugin_callbacks.acl_check){
		rc = plugin__acl_check(&db.config->security_options, context, topic, payloadlen,
				payload, qos, retain, properties, access);

		જો(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}નહીંતર જો(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}નહીંતર{
			પાછા rc;
		}
	}

	જો(context->listener){
		જો(context->listener->security_options->plugin_callbacks.acl_check){
			rc = plugin__acl_check(context->listener->security_options, context, topic, payloadlen,
					payload, qos, retain, properties, access);

			જો(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing, this is as if the plugin doesn't exist */
			}નહીંતર જો(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}નહીંતર{
				પાછા rc;
			}
		}
	}નહીંતર{
		જો(db.config->per_listener_settings){
			પાછા MOSQ_ERR_ACL_DENIED;
		}
	}

	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured, or all plugins ignored. */
	જો(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_ACL_DENIED;
	}
	પાછા rc_final;
}
