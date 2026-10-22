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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "mosquitto/broker.h"
#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto/broker_plugin.h"
#অন্তর্ভাব "lib_load.h"
#অন্তর্ভাব "utlist.h"


পূর্ণসংখ্যা acl__pre_check(mosquitto_plugin_id_t *plugin, কাঠামো mosquitto *context, পূর্ণসংখ্যা access)
{
	ধ্রুবক অক্ষর *username;

	username = mosquitto_client_username(context);
	যদি(plugin->config.deny_special_chars == true){
		/* Check whether the client id or username contains a +, # or / and if
		* so deny access.
		*
		* Do this check for every message regardless, we have to protect the
		* plugins against possible pattern based attacks.
		*/
		যদি(username && strpbrk(username, "+#/")){
			log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous username \"%s\"", username);
			ফেরত MOSQ_ERR_ACL_DENIED;
		}
		যদি(context->id && strpbrk(context->id, "+#/")){
			log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous client id \"%s\"", context->id);
			ফেরত MOSQ_ERR_ACL_DENIED;
		}
	}

	যদি(plugin->lib.version == 4){
		যদি(access == MOSQ_ACL_UNSUBSCRIBE){
			ফেরত MOSQ_ERR_SUCCESS;
		}
	}অন্যথা যদি(plugin->lib.version == 3){
		যদি(access == MOSQ_ACL_UNSUBSCRIBE){
			ফেরত MOSQ_ERR_SUCCESS;
		}
	}অন্যথা যদি(plugin->lib.version == 2){
		যদি(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			ফেরত MOSQ_ERR_SUCCESS;
		}
	}
	ফেরত MOSQ_ERR_PLUGIN_DEFER;
}


স্থির পূর্ণসংখ্যা acl__check_dollar(ধ্রুবক অক্ষর *topic, পূর্ণসংখ্যা access)
{
	পূর্ণসংখ্যা rc;
	bool match = false;

	যদি(topic[0] != '$'){
		ফেরত MOSQ_ERR_SUCCESS;
	}

	যদি(!strncmp(topic, "$SYS", 4)){
		যদি(access == MOSQ_ACL_WRITE){
			/* Potentially allow write access for bridge status, otherwise explicitly deny. */
			rc = mosquitto_topic_matches_sub("$SYS/broker/connection/+/state", topic, &match);
			যদি(rc == MOSQ_ERR_SUCCESS && match == true){
				ফেরত MOSQ_ERR_SUCCESS;
			}অন্যথা{
				ফেরত MOSQ_ERR_ACL_DENIED;
			}
		}অন্যথা{
			ফেরত MOSQ_ERR_SUCCESS;
		}
	}অন্যথা যদি(!strncmp(topic, "$share", 6)){
		/* Only allow sub/unsub to shared subscriptions */
		যদি(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			ফেরত MOSQ_ERR_SUCCESS;
		}অন্যথা{
			ফেরত MOSQ_ERR_ACL_DENIED;
		}
	}অন্যথা{
		/* This is an unknown $ topic, for the moment just defer to actual tests. */
		ফেরত MOSQ_ERR_SUCCESS;
	}
}


স্থির পূর্ণসংখ্যা plugin__acl_check(কাঠামো mosquitto__security_options *opts, কাঠামো mosquitto *context, ধ্রুবক অক্ষর *topic, uint32_t payloadlen, শূন্য *payload, uint8_t qos, bool retain, mosquitto_property *properties, পূর্ণসংখ্যা access)
{
	পূর্ণসংখ্যা rc = MOSQ_ERR_PLUGIN_DEFER;
	কাঠামো mosquitto_acl_বার্তা বার্তা;
	কাঠামো mosquitto__callback *cb_base, *cb_পরবর্তী;
	কাঠামো mosquitto_evt_acl_check event_তথ্য;

	memset(&বার্তা, 0, আকার(বার্তা));
	বার্তা.topic = topic;
	বার্তা.payloadlen = payloadlen;
	বার্তা.payload = payload;
	বার্তা.qos = qos;
	বার্তা.retain = retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.acl_check, cb_base, cb_পরবর্তী){
		/* FIXME - username deny special chars */

		memset(&event_তথ্য, 0, আকার(event_তথ্য));
		event_তথ্য.client = context;
		event_তথ্য.access = access;
		event_তথ্য.topic = topic;
		event_তথ্য.payloadlen = payloadlen;
		event_তথ্য.payload = payload;
		event_তথ্য.qos = qos;
		event_তথ্য.retain = retain;
		event_তথ্য.properties = properties;
		rc = cb_base->cb(MOSQ_EVT_ACL_CHECK, &event_তথ্য, cb_base->userdata);
		যদি(rc != MOSQ_ERR_PLUGIN_DEFER && rc != MOSQ_ERR_PLUGIN_IGNORE){
			ফেরত rc;
		}
	}

	ফেরত rc;
}


পূর্ণসংখ্যা mosquitto_acl_check(কাঠামো mosquitto *context, ধ্রুবক অক্ষর *topic, uint32_t payloadlen, শূন্য *payload, uint8_t qos, bool retain, mosquitto_property *properties, পূর্ণসংখ্যা access)
{
	পূর্ণসংখ্যা rc;
	পূর্ণসংখ্যা rc_final;

	যদি(!context->id){
		ফেরত MOSQ_ERR_ACL_DENIED;
	}
	যদি(context->bridge){
		ফেরত MOSQ_ERR_SUCCESS;
	}

	rc = acl__check_dollar(topic, access);
	যদি(rc){
		ফেরত rc;
	}

	/*
	 * If no plugins exist we should accept at this point so set rc to success.
	 */
	rc_final = MOSQ_ERR_SUCCESS;

	যদি(db.config->security_options.plugin_callbacks.acl_check){
		rc = plugin__acl_check(&db.config->security_options, context, topic, payloadlen,
				payload, qos, retain, properties, access);

		যদি(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}অন্যথা যদি(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}অন্যথা{
			ফেরত rc;
		}
	}

	যদি(context->listener){
		যদি(context->listener->security_options->plugin_callbacks.acl_check){
			rc = plugin__acl_check(context->listener->security_options, context, topic, payloadlen,
					payload, qos, retain, properties, access);

			যদি(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing, this is as if the plugin doesn't exist */
			}অন্যথা যদি(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}অন্যথা{
				ফেরত rc;
			}
		}
	}অন্যথা{
		যদি(db.config->per_listener_settings){
			ফেরত MOSQ_ERR_ACL_DENIED;
		}
	}

	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured, or all plugins ignored. */
	যদি(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_ACL_DENIED;
	}
	ফেরত rc_final;
}
