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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "mosquitto/broker.h"
#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/broker_plugin.h"
#അന്തര്ഭാവ "lib_load.h"
#അന്തര്ഭാവ "utlist.h"


പൂർണ്ണസംഖ്യ acl__pre_check(mosquitto_plugin_id_t *plugin, ഘടന mosquitto *context, പൂർണ്ണസംഖ്യ access)
{
	സ്ഥിരം അക്ഷരം *username;

	username = mosquitto_client_username(context);
	എങ്കിൽ(plugin->config.deny_special_chars == true){
		/* Check whether the client id or username contains a +, # or / and if
		* so deny access.
		*
		* Do this check for every message regardless, we have to protect the
		* plugins against possible pattern based attacks.
		*/
		എങ്കിൽ(username && strpbrk(username, "+#/")){
			log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous username \"%s\"", username);
			തിരികെ MOSQ_ERR_ACL_DENIED;
		}
		എങ്കിൽ(context->id && strpbrk(context->id, "+#/")){
			log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous client id \"%s\"", context->id);
			തിരികെ MOSQ_ERR_ACL_DENIED;
		}
	}

	എങ്കിൽ(plugin->lib.version == 4){
		എങ്കിൽ(access == MOSQ_ACL_UNSUBSCRIBE){
			തിരികെ MOSQ_ERR_SUCCESS;
		}
	}അല്ലെങ്കിൽ എങ്കിൽ(plugin->lib.version == 3){
		എങ്കിൽ(access == MOSQ_ACL_UNSUBSCRIBE){
			തിരികെ MOSQ_ERR_SUCCESS;
		}
	}അല്ലെങ്കിൽ എങ്കിൽ(plugin->lib.version == 2){
		എങ്കിൽ(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			തിരികെ MOSQ_ERR_SUCCESS;
		}
	}
	തിരികെ MOSQ_ERR_PLUGIN_DEFER;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ acl__check_dollar(സ്ഥിരം അക്ഷരം *topic, പൂർണ്ണസംഖ്യ access)
{
	പൂർണ്ണസംഖ്യ rc;
	bool match = false;

	എങ്കിൽ(topic[0] != '$'){
		തിരികെ MOSQ_ERR_SUCCESS;
	}

	എങ്കിൽ(!strncmp(topic, "$SYS", 4)){
		എങ്കിൽ(access == MOSQ_ACL_WRITE){
			/* Potentially allow write access for bridge status, otherwise explicitly deny. */
			rc = mosquitto_topic_matches_sub("$SYS/broker/connection/+/state", topic, &match);
			എങ്കിൽ(rc == MOSQ_ERR_SUCCESS && match == true){
				തിരികെ MOSQ_ERR_SUCCESS;
			}അല്ലെങ്കിൽ{
				തിരികെ MOSQ_ERR_ACL_DENIED;
			}
		}അല്ലെങ്കിൽ{
			തിരികെ MOSQ_ERR_SUCCESS;
		}
	}അല്ലെങ്കിൽ എങ്കിൽ(!strncmp(topic, "$share", 6)){
		/* Only allow sub/unsub to shared subscriptions */
		എങ്കിൽ(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			തിരികെ MOSQ_ERR_SUCCESS;
		}അല്ലെങ്കിൽ{
			തിരികെ MOSQ_ERR_ACL_DENIED;
		}
	}അല്ലെങ്കിൽ{
		/* This is an unknown $ topic, for the moment just defer to actual tests. */
		തിരികെ MOSQ_ERR_SUCCESS;
	}
}


സ്ഥിതം പൂർണ്ണസംഖ്യ plugin__acl_check(ഘടന mosquitto__security_options *opts, ഘടന mosquitto *context, സ്ഥിരം അക്ഷരം *topic, uint32_t payloadlen, ശൂന്യം *payload, uint8_t qos, bool retain, mosquitto_property *properties, പൂർണ്ണസംഖ്യ access)
{
	പൂർണ്ണസംഖ്യ rc = MOSQ_ERR_PLUGIN_DEFER;
	ഘടന mosquitto_acl_സന്ദേശം സന്ദേശം;
	ഘടന mosquitto__callback *cb_base, *cb_next;
	ഘടന mosquitto_evt_acl_check event_data;

	memset(&സന്ദേശം, 0, വലുപ്പം(സന്ദേശം));
	സന്ദേശം.topic = topic;
	സന്ദേശം.payloadlen = payloadlen;
	സന്ദേശം.payload = payload;
	സന്ദേശം.qos = qos;
	സന്ദേശം.retain = retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.acl_check, cb_base, cb_next){
		/* FIXME - username deny special chars */

		memset(&event_data, 0, വലുപ്പം(event_data));
		event_data.client = context;
		event_data.access = access;
		event_data.topic = topic;
		event_data.payloadlen = payloadlen;
		event_data.payload = payload;
		event_data.qos = qos;
		event_data.retain = retain;
		event_data.properties = properties;
		rc = cb_base->cb(MOSQ_EVT_ACL_CHECK, &event_data, cb_base->userdata);
		എങ്കിൽ(rc != MOSQ_ERR_PLUGIN_DEFER && rc != MOSQ_ERR_PLUGIN_IGNORE){
			തിരികെ rc;
		}
	}

	തിരികെ rc;
}


പൂർണ്ണസംഖ്യ mosquitto_acl_check(ഘടന mosquitto *context, സ്ഥിരം അക്ഷരം *topic, uint32_t payloadlen, ശൂന്യം *payload, uint8_t qos, bool retain, mosquitto_property *properties, പൂർണ്ണസംഖ്യ access)
{
	പൂർണ്ണസംഖ്യ rc;
	പൂർണ്ണസംഖ്യ rc_final;

	എങ്കിൽ(!context->id){
		തിരികെ MOSQ_ERR_ACL_DENIED;
	}
	എങ്കിൽ(context->bridge){
		തിരികെ MOSQ_ERR_SUCCESS;
	}

	rc = acl__check_dollar(topic, access);
	എങ്കിൽ(rc){
		തിരികെ rc;
	}

	/*
	 * If no plugins exist we should accept at this point so set rc to success.
	 */
	rc_final = MOSQ_ERR_SUCCESS;

	എങ്കിൽ(db.config->security_options.plugin_callbacks.acl_check){
		rc = plugin__acl_check(&db.config->security_options, context, topic, payloadlen,
				payload, qos, retain, properties, access);

		എങ്കിൽ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}അല്ലെങ്കിൽ{
			തിരികെ rc;
		}
	}

	എങ്കിൽ(context->listener){
		എങ്കിൽ(context->listener->security_options->plugin_callbacks.acl_check){
			rc = plugin__acl_check(context->listener->security_options, context, topic, payloadlen,
					payload, qos, retain, properties, access);

			എങ്കിൽ(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing, this is as if the plugin doesn't exist */
			}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}അല്ലെങ്കിൽ{
				തിരികെ rc;
			}
		}
	}അല്ലെങ്കിൽ{
		എങ്കിൽ(db.config->per_listener_settings){
			തിരികെ MOSQ_ERR_ACL_DENIED;
		}
	}

	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured, or all plugins ignored. */
	എങ്കിൽ(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_ACL_DENIED;
	}
	തിരികെ rc_final;
}
