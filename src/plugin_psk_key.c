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


अटल पूर्णांक plugin_psk_कुंजी_get(काष्ठा mosquitto__security_options *opts, काष्ठा mosquitto *context, स्थिर अक्षर *hint, स्थिर अक्षर *identity, अक्षर *कुंजी, पूर्णांक max_कुंजी_len)
{
	काष्ठा mosquitto_evt_psk_कुंजी event_तथ्य;
	काष्ठा mosquitto__callback *cb_base, *cb_अग्र;
	पूर्णांक rc;
	पूर्णांक rc_final = MOSQ_ERR_SUCCESS;

	DL_FOREACH_SAFE(opts->plugin_callbacks.psk_कुंजी, cb_base, cb_अग्र){
		स_रखो(&event_तथ्य, 0, माप(event_तथ्य));
		event_तथ्य.client = context;
		event_तथ्य.hint = hint;
		event_तथ्य.identity = identity;
		event_तथ्य.कुंजी = कुंजी;
		event_तथ्य.max_कुंजी_len = max_कुंजी_len;
		rc = cb_base->cb(MOSQ_EVT_PSK_कुंजी, &event_तथ्य, cb_base->userdata);
		अगर(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}अन्यथा अगर(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}अन्यथा{
			वापस rc;
		}
	}
	वापस rc_final;
}


पूर्णांक mosquitto_psk_कुंजी_get(काष्ठा mosquitto *context, स्थिर अक्षर *hint, स्थिर अक्षर *identity, अक्षर *कुंजी, पूर्णांक max_कुंजी_len)
{
	पूर्णांक rc;
	पूर्णांक rc_final = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	अगर(db.config->security_options.plugin_callbacks.psk_कुंजी){
		rc = plugin_psk_कुंजी_get(&db.config->security_options, context,
				hint, identity, कुंजी, max_कुंजी_len);

		अगर(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}अन्यथा अगर(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}अन्यथा{
			वापस rc;
		}
	}

	/* Per listener plugins */
	अगर(context->listener){
		अगर(context->listener->security_options->plugin_callbacks.psk_कुंजी){
			rc = plugin_psk_कुंजी_get(context->listener->security_options, context,
					hint, identity, कुंजी, max_कुंजी_len);

			अगर(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing */
			}अन्यथा अगर(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}अन्यथा{
				वापस rc;
			}
		}
	}अन्यथा{
		अगर(db.config->per_listener_settings){
			वापस MOSQ_ERR_AUTH;
		}
	}

	rc = mosquitto_psk_कुंजी_get_default(context, hint, identity, कुंजी, max_कुंजी_len);
	अगर(rc == MOSQ_ERR_PLUGIN_IGNORE){
		/* Do nothing */
	}अन्यथा अगर(rc == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_PLUGIN_DEFER;
	}अन्यथा{
		वापस rc;
	}


	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured. */
	अगर(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_AUTH;
	}
	वापस rc_final;
}
