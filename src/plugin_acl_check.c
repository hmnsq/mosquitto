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

#समावेश "config.h"

#समावेश <stdio.h>
#समावेश <string.h>

#समावेश "mosquitto/broker.h"
#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/broker_plugin.h"
#समावेश "lib_load.h"
#समावेश "utlist.h"


पूर्णांक acl__pre_check(mosquitto_plugin_id_t *plugin, काष्ठा mosquitto *context, पूर्णांक access)
{
	स्थिर अक्षर *username;

	username = mosquitto_client_username(context);
	अगर(plugin->config.deny_special_chars == true){
		/* Check whether the client id or username contains a +, # or / and if
		* so deny access.
		*
		* Do this check for every message regardless, we have to protect the
		* plugins against possible pattern based attacks.
		*/
		अगर(username && strpbrk(username, "+#/")){
			log__printf(शून्य, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous username \"%s\"", username);
			वापस MOSQ_ERR_ACL_DENIED;
		}
		अगर(context->id && strpbrk(context->id, "+#/")){
			log__printf(शून्य, MOSQ_LOG_NOTICE, "ACL denying access to client with dangerous client id \"%s\"", context->id);
			वापस MOSQ_ERR_ACL_DENIED;
		}
	}

	अगर(plugin->lib.version == 4){
		अगर(access == MOSQ_ACL_UNSUBSCRIBE){
			वापस MOSQ_ERR_SUCCESS;
		}
	}अन्यथा अगर(plugin->lib.version == 3){
		अगर(access == MOSQ_ACL_UNSUBSCRIBE){
			वापस MOSQ_ERR_SUCCESS;
		}
	}अन्यथा अगर(plugin->lib.version == 2){
		अगर(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			वापस MOSQ_ERR_SUCCESS;
		}
	}
	वापस MOSQ_ERR_PLUGIN_DEFER;
}


अटल पूर्णांक acl__check_dollar(स्थिर अक्षर *topic, पूर्णांक access)
{
	पूर्णांक rc;
	bool match = false;

	अगर(topic[0] != '$'){
		वापस MOSQ_ERR_SUCCESS;
	}

	अगर(!म_भेदन(topic, "$SYS", 4)){
		अगर(access == MOSQ_ACL_WRITE){
			/* Potentially allow write access for bridge status, otherwise explicitly deny. */
			rc = mosquitto_topic_matches_sub("$SYS/broker/connection/+/state", topic, &match);
			अगर(rc == MOSQ_ERR_SUCCESS && match == true){
				वापस MOSQ_ERR_SUCCESS;
			}अन्यथा{
				वापस MOSQ_ERR_ACL_DENIED;
			}
		}अन्यथा{
			वापस MOSQ_ERR_SUCCESS;
		}
	}अन्यथा अगर(!म_भेदन(topic, "$share", 6)){
		/* Only allow sub/unsub to shared subscriptions */
		अगर(access == MOSQ_ACL_SUBSCRIBE || access == MOSQ_ACL_UNSUBSCRIBE){
			वापस MOSQ_ERR_SUCCESS;
		}अन्यथा{
			वापस MOSQ_ERR_ACL_DENIED;
		}
	}अन्यथा{
		/* This is an unknown $ topic, for the moment just defer to actual tests. */
		वापस MOSQ_ERR_SUCCESS;
	}
}


अटल पूर्णांक plugin__acl_check(काष्ठा mosquitto__security_options *opts, काष्ठा mosquitto *context, स्थिर अक्षर *topic, uint32_t payloadlen, व्योम *payload, uint8_t qos, bool retain, mosquitto_property *properties, पूर्णांक access)
{
	पूर्णांक rc = MOSQ_ERR_PLUGIN_DEFER;
	काष्ठा mosquitto_acl_संदेश संदेश;
	काष्ठा mosquitto__callback *cb_base, *cb_अग्र;
	काष्ठा mosquitto_evt_acl_check event_तथ्य;

	स_रखो(&संदेश, 0, माप(संदेश));
	संदेश.topic = topic;
	संदेश.payloadlen = payloadlen;
	संदेश.payload = payload;
	संदेश.qos = qos;
	संदेश.retain = retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.acl_check, cb_base, cb_अग्र){
		/* FIXME - username deny special chars */

		स_रखो(&event_तथ्य, 0, माप(event_तथ्य));
		event_तथ्य.client = context;
		event_तथ्य.access = access;
		event_तथ्य.topic = topic;
		event_तथ्य.payloadlen = payloadlen;
		event_तथ्य.payload = payload;
		event_तथ्य.qos = qos;
		event_तथ्य.retain = retain;
		event_तथ्य.properties = properties;
		rc = cb_base->cb(MOSQ_EVT_ACL_CHECK, &event_तथ्य, cb_base->userdata);
		अगर(rc != MOSQ_ERR_PLUGIN_DEFER && rc != MOSQ_ERR_PLUGIN_IGNORE){
			वापस rc;
		}
	}

	वापस rc;
}


पूर्णांक mosquitto_acl_check(काष्ठा mosquitto *context, स्थिर अक्षर *topic, uint32_t payloadlen, व्योम *payload, uint8_t qos, bool retain, mosquitto_property *properties, पूर्णांक access)
{
	पूर्णांक rc;
	पूर्णांक rc_final;

	अगर(!context->id){
		वापस MOSQ_ERR_ACL_DENIED;
	}
	अगर(context->bridge){
		वापस MOSQ_ERR_SUCCESS;
	}

	rc = acl__check_dollar(topic, access);
	अगर(rc){
		वापस rc;
	}

	/*
	 * If no plugins exist we should accept at this point so set rc to success.
	 */
	rc_final = MOSQ_ERR_SUCCESS;

	अगर(db.config->security_options.plugin_callbacks.acl_check){
		rc = plugin__acl_check(&db.config->security_options, context, topic, payloadlen,
				payload, qos, retain, properties, access);

		अगर(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}अन्यथा अगर(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}अन्यथा{
			वापस rc;
		}
	}

	अगर(context->listener){
		अगर(context->listener->security_options->plugin_callbacks.acl_check){
			rc = plugin__acl_check(context->listener->security_options, context, topic, payloadlen,
					payload, qos, retain, properties, access);

			अगर(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing, this is as if the plugin doesn't exist */
			}अन्यथा अगर(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}अन्यथा{
				वापस rc;
			}
		}
	}अन्यथा{
		अगर(db.config->per_listener_settings){
			वापस MOSQ_ERR_ACL_DENIED;
		}
	}

	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured, or all plugins ignored. */
	अगर(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_ACL_DENIED;
	}
	वापस rc_final;
}
