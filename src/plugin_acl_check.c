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

#include "config.h"

#include <stdio.h>
#include <string.h>

#include "mosquitto/broker.h"
#include "mosquitto_broker_internal.h"
#include "mosquitto/broker_plugin.h"
#include "lib_load.h"
#include "utlist.h"


целое acl__pre_check(mosquitto_plugin_id_t *plugin, struct mosquitto *context, целое access)
{
	const char *username;

	username = mosquitto_client_username(context);
	если(plugin->config.deny_special_chars == true){
		/* Check whether the client id or username contains a +, # or / and if
		* so deny access.
		*
		* Do this check for every message regardless, we have to protect the
		* plugins against possible pattern based attacks.
		*/
		если(username && strpbrk(username, "+#/")){
			log__printf(NULL, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous username \"%s\"", username);
			вернуть MOSQ_ERR_ACL_DENIED;
		}
		если(context->id && strpbrk(context->id, "+#/")){
			log__printf(NULL, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous client id \"%s\"", context->id);
			вернуть MOSQ_ERR_ACL_DENIED;
		}
	}

	если(plugin->lib.version == 4){
		если(access == MOSQ_ACL_UNSUBSCRIBE){
			вернуть MOSQ_ERR_SUCCESS;
		}
	}иначе если(plugin->lib.version == 3){
		если(access == MOSQ_ACL_UNSUBSCRIBE){
			вернуть MOSQ_ERR_SUCCESS;
		}
	}иначе если(plugin->lib.version == 2){
		если(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			вернуть MOSQ_ERR_SUCCESS;
		}
	}
	вернуть MOSQ_ERR_PLUGIN_DEFER;
}


static целое acl__check_dollar(const char *topic, целое access)
{
	целое rc;
	bool match = false;

	если(topic[0] != '$'){
		вернуть MOSQ_ERR_SUCCESS;
	}

	если(!strncmp(topic, "$SYS", 4)){
		если(access == MOSQ_ACL_WRITE){
			/* Potentially allow write access for bridge status, otherwise explicitly deny. */
			rc = mosquitto_topic_matches_sub("$SYS/broker/connection/+/state", topic, &match);
			если(rc == MOSQ_ERR_SUCCESS && match == true){
				вернуть MOSQ_ERR_SUCCESS;
			}иначе{
				вернуть MOSQ_ERR_ACL_DENIED;
			}
		}иначе{
			вернуть MOSQ_ERR_SUCCESS;
		}
	}иначе если(!strncmp(topic, "$share", 6)){
		/* Only allow sub/unsub to shared subscriptions */
		если(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			вернуть MOSQ_ERR_SUCCESS;
		}иначе{
			вернуть MOSQ_ERR_ACL_DENIED;
		}
	}иначе{
		/* This is an unknown $ topic, for the moment just defer to actual tests. */
		вернуть MOSQ_ERR_SUCCESS;
	}
}


static целое plugin__acl_check(struct mosquitto__security_options *opts, struct mosquitto *context, const char *topic, uint32_t payloadlen, void *payload, uint8_t qos, bool retain, mosquitto_property *properties, целое access)
{
	целое rc = MOSQ_ERR_PLUGIN_DEFER;
	struct mosquitto_acl_msg msg;
	struct mosquitto__callback *cb_base, *cb_next;
	struct mosquitto_evt_acl_check event_data;

	memset(&msg, 0, sizeof(msg));
	msg.topic = topic;
	msg.payloadlen = payloadlen;
	msg.payload = payload;
	msg.qos = qos;
	msg.retain = retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.acl_check, cb_base, cb_next){
		/* FIXME - username deny special chars */

		memset(&event_data, 0, sizeof(event_data));
		event_data.client = context;
		event_data.access = access;
		event_data.topic = topic;
		event_data.payloadlen = payloadlen;
		event_data.payload = payload;
		event_data.qos = qos;
		event_data.retain = retain;
		event_data.properties = properties;
		rc = cb_base->cb(MOSQ_EVT_ACL_CHECK, &event_data, cb_base->userdata);
		если(rc != MOSQ_ERR_PLUGIN_DEFER && rc != MOSQ_ERR_PLUGIN_IGNORE){
			вернуть rc;
		}
	}

	вернуть rc;
}


целое mosquitto_acl_check(struct mosquitto *context, const char *topic, uint32_t payloadlen, void *payload, uint8_t qos, bool retain, mosquitto_property *properties, целое access)
{
	целое rc;
	целое rc_final;

	если(!context->id){
		вернуть MOSQ_ERR_ACL_DENIED;
	}
	если(context->bridge){
		вернуть MOSQ_ERR_SUCCESS;
	}

	rc = acl__check_dollar(topic, access);
	если(rc){
		вернуть rc;
	}

	/*
	 * If no plugins exist we should accept at this point so set rc to success.
	 */
	rc_final = MOSQ_ERR_SUCCESS;

	если(db.config->security_options.plugin_callbacks.acl_check){
		rc = plugin__acl_check(&db.config->security_options, context, topic, payloadlen,
				payload, qos, retain, properties, access);

		если(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}иначе если(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}иначе{
			вернуть rc;
		}
	}

	если(context->listener){
		если(context->listener->security_options->plugin_callbacks.acl_check){
			rc = plugin__acl_check(context->listener->security_options, context, topic, payloadlen,
					payload, qos, retain, properties, access);

			если(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing, this is as if the plugin doesn't exist */
			}иначе если(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}иначе{
				вернуть rc;
			}
		}
	}иначе{
		если(db.config->per_listener_settings){
			вернуть MOSQ_ERR_ACL_DENIED;
		}
	}

	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured, or all plugins ignored. */
	если(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_ACL_DENIED;
	}
	вернуть rc_final;
}
