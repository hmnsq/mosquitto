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


ចំនួនគត់ acl__pre_check(mosquitto_plugin_id_t *plugin, sa_nracanaa mosquitto *context, ចំនួនគត់ access)
{
	const var_na *username;

	username = mosquitto_client_username(context);
	ប្រសិនបើ(plugin->config.deny_special_chars == true){
		/* Check whether the client id or username contains a +, # or / and if
		* so deny access.
		*
		* Do this check for every message regardless, we have to protect the
		* plugins against possible pattern based attacks.
		*/
		ប្រសិនបើ(username && strpbrk(username, "+#/")){
			log__printf(NULL, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous username \"%s\"", username);
			ត្រឡប់ MOSQ_ERR_ACL_DENIED;
		}
		ប្រសិនបើ(context->id && strpbrk(context->id, "+#/")){
			log__printf(NULL, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous client id \"%s\"", context->id);
			ត្រឡប់ MOSQ_ERR_ACL_DENIED;
		}
	}

	ប្រសិនបើ(plugin->lib.version == 4){
		ប្រសិនបើ(access == MOSQ_ACL_UNSUBSCRIBE){
			ត្រឡប់ MOSQ_ERR_SUCCESS;
		}
	}បើមិនដូច្នេះ ប្រសិនបើ(plugin->lib.version == 3){
		ប្រសិនបើ(access == MOSQ_ACL_UNSUBSCRIBE){
			ត្រឡប់ MOSQ_ERR_SUCCESS;
		}
	}បើមិនដូច្នេះ ប្រសិនបើ(plugin->lib.version == 2){
		ប្រសិនបើ(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			ត្រឡប់ MOSQ_ERR_SUCCESS;
		}
	}
	ត្រឡប់ MOSQ_ERR_PLUGIN_DEFER;
}


static ចំនួនគត់ acl__check_dollar(const var_na *topic, ចំនួនគត់ access)
{
	ចំនួនគត់ rc;
	bool match = false;

	ប្រសិនបើ(topic[0] != '$'){
		ត្រឡប់ MOSQ_ERR_SUCCESS;
	}

	ប្រសិនបើ(!strncmp(topic, "$SYS", 4)){
		ប្រសិនបើ(access == MOSQ_ACL_WRITE){
			/* Potentially allow write access for bridge status, otherwise explicitly deny. */
			rc = mosquitto_topic_matches_sub("$SYS/broker/connection/+/state", topic, &match);
			ប្រសិនបើ(rc == MOSQ_ERR_SUCCESS && match == true){
				ត្រឡប់ MOSQ_ERR_SUCCESS;
			}បើមិនដូច្នេះ{
				ត្រឡប់ MOSQ_ERR_ACL_DENIED;
			}
		}បើមិនដូច្នេះ{
			ត្រឡប់ MOSQ_ERR_SUCCESS;
		}
	}បើមិនដូច្នេះ ប្រសិនបើ(!strncmp(topic, "$share", 6)){
		/* Only allow sub/unsub to shared subscriptions */
		ប្រសិនបើ(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			ត្រឡប់ MOSQ_ERR_SUCCESS;
		}បើមិនដូច្នេះ{
			ត្រឡប់ MOSQ_ERR_ACL_DENIED;
		}
	}បើមិនដូច្នេះ{
		/* This is an unknown $ topic, for the moment just defer to actual tests. */
		ត្រឡប់ MOSQ_ERR_SUCCESS;
	}
}


static ចំនួនគត់ plugin__acl_check(sa_nracanaa mosquitto__security_options *opts, sa_nracanaa mosquitto *context, const var_na *topic, uint32_t payloadlen, void *payload, uint8_t qos, bool retain, mosquitto_property *properties, ចំនួនគត់ access)
{
	ចំនួនគត់ rc = MOSQ_ERR_PLUGIN_DEFER;
	sa_nracanaa mosquitto_acl_msg msg;
	sa_nracanaa mosquitto__callback *cb_base, *cb_next;
	sa_nracanaa mosquitto_evt_acl_check event_data;

	memset(&msg, 0, parimaa_na(msg));
	msg.topic = topic;
	msg.payloadlen = payloadlen;
	msg.payload = payload;
	msg.qos = qos;
	msg.retain = retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.acl_check, cb_base, cb_next){
		/* FIXME - username deny special chars */

		memset(&event_data, 0, parimaa_na(event_data));
		event_data.client = context;
		event_data.access = access;
		event_data.topic = topic;
		event_data.payloadlen = payloadlen;
		event_data.payload = payload;
		event_data.qos = qos;
		event_data.retain = retain;
		event_data.properties = properties;
		rc = cb_base->cb(MOSQ_EVT_ACL_CHECK, &event_data, cb_base->userdata);
		ប្រសិនបើ(rc != MOSQ_ERR_PLUGIN_DEFER && rc != MOSQ_ERR_PLUGIN_IGNORE){
			ត្រឡប់ rc;
		}
	}

	ត្រឡប់ rc;
}


ចំនួនគត់ mosquitto_acl_check(sa_nracanaa mosquitto *context, const var_na *topic, uint32_t payloadlen, void *payload, uint8_t qos, bool retain, mosquitto_property *properties, ចំនួនគត់ access)
{
	ចំនួនគត់ rc;
	ចំនួនគត់ rc_final;

	ប្រសិនបើ(!context->id){
		ត្រឡប់ MOSQ_ERR_ACL_DENIED;
	}
	ប្រសិនបើ(context->bridge){
		ត្រឡប់ MOSQ_ERR_SUCCESS;
	}

	rc = acl__check_dollar(topic, access);
	ប្រសិនបើ(rc){
		ត្រឡប់ rc;
	}

	/*
	 * If no plugins exist we should accept at this point so set rc to success.
	 */
	rc_final = MOSQ_ERR_SUCCESS;

	ប្រសិនបើ(db.config->security_options.plugin_callbacks.acl_check){
		rc = plugin__acl_check(&db.config->security_options, context, topic, payloadlen,
				payload, qos, retain, properties, access);

		ប្រសិនបើ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}បើមិនដូច្នេះ ប្រសិនបើ(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}បើមិនដូច្នេះ{
			ត្រឡប់ rc;
		}
	}

	ប្រសិនបើ(context->listener){
		ប្រសិនបើ(context->listener->security_options->plugin_callbacks.acl_check){
			rc = plugin__acl_check(context->listener->security_options, context, topic, payloadlen,
					payload, qos, retain, properties, access);

			ប្រសិនបើ(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing, this is as if the plugin doesn't exist */
			}បើមិនដូច្នេះ ប្រសិនបើ(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}បើមិនដូច្នេះ{
				ត្រឡប់ rc;
			}
		}
	}បើមិនដូច្នេះ{
		ប្រសិនបើ(db.config->per_listener_settings){
			ត្រឡប់ MOSQ_ERR_ACL_DENIED;
		}
	}

	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured, or all plugins ignored. */
	ប្រសិនបើ(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_ACL_DENIED;
	}
	ត្រឡប់ rc_final;
}
