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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto/broker.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/broker_plugin.h"
#ଅନ୍ତର୍ଭାଵ "lib_load.h"
#ଅନ୍ତର୍ଭାଵ "utlist.h"


ପୂର୍ଣ୍ଣସଂଖ୍ୟା acl__pre_check(mosquitto_plugin_id_t *plugin, ସଂରଚନା mosquitto *context, ପୂର୍ଣ୍ଣସଂଖ୍ୟା access)
{
	ଅଚଳ ଅକ୍ଷର *username;

	username = mosquitto_client_username(context);
	ଯଦି(plugin->config.deny_special_chars == true){
		/* Check whether the client id or username contains a +, # or / and if
		* so deny access.
		*
		* Do this check for every message regardless, we have to protect the
		* plugins against possible pattern based attacks.
		*/
		ଯଦି(username && strpbrk(username, "+#/")){
			log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous username \"%s\"", username);
			ଫେରନ୍ତୁ MOSQ_ERR_ACL_DENIED;
		}
		ଯଦି(context->id && strpbrk(context->id, "+#/")){
			log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous client id \"%s\"", context->id);
			ଫେରନ୍ତୁ MOSQ_ERR_ACL_DENIED;
		}
	}

	ଯଦି(plugin->lib.version == 4){
		ଯଦି(access == MOSQ_ACL_UNSUBSCRIBE){
			ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
		}
	}ଅନ୍ୟଥା ଯଦି(plugin->lib.version == 3){
		ଯଦି(access == MOSQ_ACL_UNSUBSCRIBE){
			ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
		}
	}ଅନ୍ୟଥା ଯଦି(plugin->lib.version == 2){
		ଯଦି(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
		}
	}
	ଫେରନ୍ତୁ MOSQ_ERR_PLUGIN_DEFER;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା acl__check_dollar(ଅଚଳ ଅକ୍ଷର *topic, ପୂର୍ଣ୍ଣସଂଖ୍ୟା access)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	bool match = false;

	ଯଦି(topic[0] != '$'){
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

	ଯଦି(!strncmp(topic, "$SYS", 4)){
		ଯଦି(access == MOSQ_ACL_WRITE){
			/* Potentially allow write access for bridge status, otherwise explicitly deny. */
			rc = mosquitto_topic_matches_sub("$SYS/broker/connection/+/state", topic, &match);
			ଯଦି(rc == MOSQ_ERR_SUCCESS && match == true){
				ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
			}ଅନ୍ୟଥା{
				ଫେରନ୍ତୁ MOSQ_ERR_ACL_DENIED;
			}
		}ଅନ୍ୟଥା{
			ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
		}
	}ଅନ୍ୟଥା ଯଦି(!strncmp(topic, "$share", 6)){
		/* Only allow sub/unsub to shared subscriptions */
		ଯଦି(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
		}ଅନ୍ୟଥା{
			ଫେରନ୍ତୁ MOSQ_ERR_ACL_DENIED;
		}
	}ଅନ୍ୟଥା{
		/* This is an unknown $ topic, for the moment just defer to actual tests. */
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin__acl_check(ସଂରଚନା mosquitto__security_options *opts, ସଂରଚନା mosquitto *context, ଅଚଳ ଅକ୍ଷର *topic, uint32_t payloadlen, ଶୂନ୍ୟ *payload, uint8_t qos, bool retain, mosquitto_property *properties, ପୂର୍ଣ୍ଣସଂଖ୍ୟା access)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = MOSQ_ERR_PLUGIN_DEFER;
	ସଂରଚନା mosquitto_acl_ସନ୍ଦେଶ ସନ୍ଦେଶ;
	ସଂରଚନା mosquitto__callback *cb_base, *cb_next;
	ସଂରଚନା mosquitto_evt_acl_check event_data;

	memset(&ସନ୍ଦେଶ, 0, ଆକାର(ସନ୍ଦେଶ));
	ସନ୍ଦେଶ.topic = topic;
	ସନ୍ଦେଶ.payloadlen = payloadlen;
	ସନ୍ଦେଶ.payload = payload;
	ସନ୍ଦେଶ.qos = qos;
	ସନ୍ଦେଶ.retain = retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.acl_check, cb_base, cb_next){
		/* FIXME - username deny special chars */

		memset(&event_data, 0, ଆକାର(event_data));
		event_data.client = context;
		event_data.access = access;
		event_data.topic = topic;
		event_data.payloadlen = payloadlen;
		event_data.payload = payload;
		event_data.qos = qos;
		event_data.retain = retain;
		event_data.properties = properties;
		rc = cb_base->cb(MOSQ_EVT_ACL_CHECK, &event_data, cb_base->userdata);
		ଯଦି(rc != MOSQ_ERR_PLUGIN_DEFER && rc != MOSQ_ERR_PLUGIN_IGNORE){
			ଫେରନ୍ତୁ rc;
		}
	}

	ଫେରନ୍ତୁ rc;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_acl_check(ସଂରଚନା mosquitto *context, ଅଚଳ ଅକ୍ଷର *topic, uint32_t payloadlen, ଶୂନ୍ୟ *payload, uint8_t qos, bool retain, mosquitto_property *properties, ପୂର୍ଣ୍ଣସଂଖ୍ୟା access)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc_final;

	ଯଦି(!context->id){
		ଫେରନ୍ତୁ MOSQ_ERR_ACL_DENIED;
	}
	ଯଦି(context->bridge){
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

	rc = acl__check_dollar(topic, access);
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}

	/*
	 * If no plugins exist we should accept at this point so set rc to success.
	 */
	rc_final = MOSQ_ERR_SUCCESS;

	ଯଦି(db.config->security_options.plugin_callbacks.acl_check){
		rc = plugin__acl_check(&db.config->security_options, context, topic, payloadlen,
				payload, qos, retain, properties, access);

		ଯଦି(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}ଅନ୍ୟଥା ଯଦି(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}ଅନ୍ୟଥା{
			ଫେରନ୍ତୁ rc;
		}
	}

	ଯଦି(context->listener){
		ଯଦି(context->listener->security_options->plugin_callbacks.acl_check){
			rc = plugin__acl_check(context->listener->security_options, context, topic, payloadlen,
					payload, qos, retain, properties, access);

			ଯଦି(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing, this is as if the plugin doesn't exist */
			}ଅନ୍ୟଥା ଯଦି(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}ଅନ୍ୟଥା{
				ଫେରନ୍ତୁ rc;
			}
		}
	}ଅନ୍ୟଥା{
		ଯଦି(db.config->per_listener_settings){
			ଫେରନ୍ତୁ MOSQ_ERR_ACL_DENIED;
		}
	}

	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured, or all plugins ignored. */
	ଯଦି(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_ACL_DENIED;
	}
	ଫେରନ୍ତୁ rc_final;
}
