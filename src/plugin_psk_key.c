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

#अन्तर्भाव "config.h"

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "mosquitto/broker.h"
#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/broker_plugin.h"
#अन्तर्भाव "lib_load.h"
#अन्तर्भाव "utlist.h"


स्थिर पूर्णांक plugin_psk_किल्ली_get(संरचना mosquitto__security_options *opts, संरचना mosquitto *context, स्थिरांक अक्षर *hint, स्थिरांक अक्षर *identity, अक्षर *किल्ली, पूर्णांक max_किल्ली_len)
{
	संरचना mosquitto_evt_psk_किल्ली event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	पूर्णांक rc;
	पूर्णांक rc_final = MOSQ_ERR_SUCCESS;

	DL_FOREACH_SAFE(opts->plugin_callbacks.psk_किल्ली, cb_base, cb_next){
		स_रखो(&event_data, 0, आकार(event_data));
		event_data.client = context;
		event_data.hint = hint;
		event_data.identity = identity;
		event_data.किल्ली = किल्ली;
		event_data.max_किल्ली_len = max_किल्ली_len;
		rc = cb_base->cb(MOSQ_EVT_PSK_किल्ली, &event_data, cb_base->userdata);
		जर(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}अन्यथा जर(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}अन्यथा{
			परत rc;
		}
	}
	परत rc_final;
}


पूर्णांक mosquitto_psk_किल्ली_get(संरचना mosquitto *context, स्थिरांक अक्षर *hint, स्थिरांक अक्षर *identity, अक्षर *किल्ली, पूर्णांक max_किल्ली_len)
{
	पूर्णांक rc;
	पूर्णांक rc_final = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	जर(db.config->security_options.plugin_callbacks.psk_किल्ली){
		rc = plugin_psk_किल्ली_get(&db.config->security_options, context,
				hint, identity, किल्ली, max_किल्ली_len);

		जर(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}अन्यथा जर(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}अन्यथा{
			परत rc;
		}
	}

	/* Per listener plugins */
	जर(context->listener){
		जर(context->listener->security_options->plugin_callbacks.psk_किल्ली){
			rc = plugin_psk_किल्ली_get(context->listener->security_options, context,
					hint, identity, किल्ली, max_किल्ली_len);

			जर(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing */
			}अन्यथा जर(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}अन्यथा{
				परत rc;
			}
		}
	}अन्यथा{
		जर(db.config->per_listener_settings){
			परत MOSQ_ERR_AUTH;
		}
	}

	rc = mosquitto_psk_किल्ली_get_default(context, hint, identity, किल्ली, max_किल्ली_len);
	जर(rc == MOSQ_ERR_PLUGIN_IGNORE){
		/* Do nothing */
	}अन्यथा जर(rc == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_PLUGIN_DEFER;
	}अन्यथा{
		परत rc;
	}


	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured. */
	जर(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_AUTH;
	}
	परत rc_final;
}
