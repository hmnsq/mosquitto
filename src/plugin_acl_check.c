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

#అన్తర్భావ "config.h"

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ "mosquitto/broker.h"
#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto/broker_plugin.h"
#అన్తర్భావ "lib_load.h"
#అన్తర్భావ "utlist.h"


పూర్ణాంకం acl__pre_check(mosquitto_plugin_id_t *plugin, నిర్మాణం mosquitto *context, పూర్ణాంకం access)
{
	స్థిరరాశి అక్షరం *username;

	username = mosquitto_client_username(context);
	ఒకవేళ(plugin->config.deny_special_chars == true){
		/* Check whether the client id or username contains a +, # or / and if
		* so deny access.
		*
		* Do this check for every message regardless, we have to protect the
		* plugins against possible pattern based attacks.
		*/
		ఒకవేళ(username && strpbrk(username, "+#/")){
			log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous username \"%s\"", username);
			తిరిగి MOSQ_ERR_ACL_DENIED;
		}
		ఒకవేళ(context->id && strpbrk(context->id, "+#/")){
			log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous client id \"%s\"", context->id);
			తిరిగి MOSQ_ERR_ACL_DENIED;
		}
	}

	ఒకవేళ(plugin->lib.సంస్కరణ_2 == 4){
		ఒకవేళ(access == MOSQ_ACL_UNSUBSCRIBE){
			తిరిగి MOSQ_ERR_SUCCESS;
		}
	}లేకపోతే ఒకవేళ(plugin->lib.సంస్కరణ_2 == 3){
		ఒకవేళ(access == MOSQ_ACL_UNSUBSCRIBE){
			తిరిగి MOSQ_ERR_SUCCESS;
		}
	}లేకపోతే ఒకవేళ(plugin->lib.సంస్కరణ_2 == 2){
		ఒకవేళ(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			తిరిగి MOSQ_ERR_SUCCESS;
		}
	}
	తిరిగి MOSQ_ERR_PLUGIN_DEFER;
}


స్థిర పూర్ణాంకం acl__check_dollar(స్థిరరాశి అక్షరం *topic, పూర్ణాంకం access)
{
	పూర్ణాంకం rc;
	bool సరిపోలిక = false;

	ఒకవేళ(topic[0] != '$'){
		తిరిగి MOSQ_ERR_SUCCESS;
	}

	ఒకవేళ(!అక్షరశ్రేణి_పోలిక(topic, "$SYS", 4)){
		ఒకవేళ(access == MOSQ_ACL_WRITE){
			/* Potentially allow write access for bridge status, otherwise explicitly deny. */
			rc = mosquitto_topic_matches_sub("$SYS/broker/connection/+/state", topic, &సరిపోలిక);
			ఒకవేళ(rc == MOSQ_ERR_SUCCESS && సరిపోలిక == true){
				తిరిగి MOSQ_ERR_SUCCESS;
			}లేకపోతే{
				తిరిగి MOSQ_ERR_ACL_DENIED;
			}
		}లేకపోతే{
			తిరిగి MOSQ_ERR_SUCCESS;
		}
	}లేకపోతే ఒకవేళ(!అక్షరశ్రేణి_పోలిక(topic, "$share", 6)){
		/* Only allow sub/unsub to shared subscriptions */
		ఒకవేళ(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			తిరిగి MOSQ_ERR_SUCCESS;
		}లేకపోతే{
			తిరిగి MOSQ_ERR_ACL_DENIED;
		}
	}లేకపోతే{
		/* This is an unknown $ topic, for the moment just defer to actual tests. */
		తిరిగి MOSQ_ERR_SUCCESS;
	}
}


స్థిర పూర్ణాంకం plugin__acl_check(నిర్మాణం mosquitto__security_options *opts, నిర్మాణం mosquitto *context, స్థిరరాశి అక్షరం *topic, uint32_t payloadlen, శూన్య *payload, uint8_t qos, bool retain, mosquitto_property *properties, పూర్ణాంకం access)
{
	పూర్ణాంకం rc = MOSQ_ERR_PLUGIN_DEFER;
	నిర్మాణం mosquitto_acl_సందేశం సందేశం;
	నిర్మాణం mosquitto__callback *cb_పునాది, *cb_తదుపరి;
	నిర్మాణం mosquitto_evt_acl_check event_దత్తాంశం;

	memset(&సందేశం, 0, పరిమాణం(సందేశం));
	సందేశం.topic = topic;
	సందేశం.payloadlen = payloadlen;
	సందేశం.payload = payload;
	సందేశం.qos = qos;
	సందేశం.retain = retain;

	DL_FOREACH_సురక్షిత(opts->plugin_callbacks.acl_check, cb_పునాది, cb_తదుపరి){
		/* FIXME - username deny special chars */

		memset(&event_దత్తాంశం, 0, పరిమాణం(event_దత్తాంశం));
		event_దత్తాంశం.client = context;
		event_దత్తాంశం.access = access;
		event_దత్తాంశం.topic = topic;
		event_దత్తాంశం.payloadlen = payloadlen;
		event_దత్తాంశం.payload = payload;
		event_దత్తాంశం.qos = qos;
		event_దత్తాంశం.retain = retain;
		event_దత్తాంశం.properties = properties;
		rc = cb_పునాది->cb(MOSQ_EVT_ACL_CHECK, &event_దత్తాంశం, cb_పునాది->userdata);
		ఒకవేళ(rc != MOSQ_ERR_PLUGIN_DEFER && rc != MOSQ_ERR_PLUGIN_IGNORE){
			తిరిగి rc;
		}
	}

	తిరిగి rc;
}


పూర్ణాంకం mosquitto_acl_check(నిర్మాణం mosquitto *context, స్థిరరాశి అక్షరం *topic, uint32_t payloadlen, శూన్య *payload, uint8_t qos, bool retain, mosquitto_property *properties, పూర్ణాంకం access)
{
	పూర్ణాంకం rc;
	పూర్ణాంకం rc_final;

	ఒకవేళ(!context->id){
		తిరిగి MOSQ_ERR_ACL_DENIED;
	}
	ఒకవేళ(context->bridge){
		తిరిగి MOSQ_ERR_SUCCESS;
	}

	rc = acl__check_dollar(topic, access);
	ఒకవేళ(rc){
		తిరిగి rc;
	}

	/*
	 * If no plugins exist we should accept at this point so set rc to success.
	 */
	rc_final = MOSQ_ERR_SUCCESS;

	ఒకవేళ(db.config->security_options.plugin_callbacks.acl_check){
		rc = plugin__acl_check(&db.config->security_options, context, topic, payloadlen,
				payload, qos, retain, properties, access);

		ఒకవేళ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}లేకపోతే{
			తిరిగి rc;
		}
	}

	ఒకవేళ(context->listener){
		ఒకవేళ(context->listener->security_options->plugin_callbacks.acl_check){
			rc = plugin__acl_check(context->listener->security_options, context, topic, payloadlen,
					payload, qos, retain, properties, access);

			ఒకవేళ(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing, this is as if the plugin doesn't exist */
			}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}లేకపోతే{
				తిరిగి rc;
			}
		}
	}లేకపోతే{
		ఒకవేళ(db.config->చొప్పున_listener_settings){
			తిరిగి MOSQ_ERR_ACL_DENIED;
		}
	}

	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured, or all plugins ignored. */
	ఒకవేళ(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_ACL_DENIED;
	}
	తిరిగి rc_final;
}
