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


स्थिर पूर्णाङ्क plugin_psk_कुञ्चिका_get(संरचना mosquitto__security_options *opts, संरचना mosquitto *context, ध्रुव वर्ण *hint, ध्रुव वर्ण *तटस्थः, वर्ण *कुञ्चिका, पूर्णाङ्क max_कुञ्चिका_len)
{
	संरचना mosquitto_evt_psk_कुञ्चिका event_दत्त;
	संरचना mosquitto__callback *cb_आधारः, *cb_अग्र;
	पूर्णाङ्क rc;
	पूर्णाङ्क rc_final = MOSQ_ERR_SUCCESS;

	DL_FOREACH_SAFE(opts->plugin_callbacks.psk_कुञ्चिका, cb_आधारः, cb_अग्र){
		स्मृतिं_पूरय(&event_दत्त, 0, परिमाण(event_दत्त));
		event_दत्त.client = context;
		event_दत्त.hint = hint;
		event_दत्त.तटस्थः = तटस्थः;
		event_दत्त.कुञ्चिका = कुञ्चिका;
		event_दत्त.max_कुञ्चिका_len = max_कुञ्चिका_len;
		rc = cb_आधारः->cb(MOSQ_EVT_PSK_कुञ्चिका, &event_दत्त, cb_आधारः->userdata);
		यदि(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}अन्यथा यदि(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}अन्यथा{
			प्रत्यागम rc;
		}
	}
	प्रत्यागम rc_final;
}


पूर्णाङ्क mosquitto_psk_कुञ्चिका_get(संरचना mosquitto *context, ध्रुव वर्ण *hint, ध्रुव वर्ण *तटस्थः, वर्ण *कुञ्चिका, पूर्णाङ्क max_कुञ्चिका_len)
{
	पूर्णाङ्क rc;
	पूर्णाङ्क rc_final = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	यदि(db.config->security_options.plugin_callbacks.psk_कुञ्चिका){
		rc = plugin_psk_कुञ्चिका_get(&db.config->security_options, context,
				hint, तटस्थः, कुञ्चिका, max_कुञ्चिका_len);

		यदि(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}अन्यथा यदि(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}अन्यथा{
			प्रत्यागम rc;
		}
	}

	/* Per listener plugins */
	यदि(context->listener){
		यदि(context->listener->security_options->plugin_callbacks.psk_कुञ्चिका){
			rc = plugin_psk_कुञ्चिका_get(context->listener->security_options, context,
					hint, तटस्थः, कुञ्चिका, max_कुञ्चिका_len);

			यदि(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing */
			}अन्यथा यदि(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}अन्यथा{
				प्रत्यागम rc;
			}
		}
	}अन्यथा{
		यदि(db.config->प्रत्येकम्_listener_settings){
			प्रत्यागम MOSQ_ERR_AUTH;
		}
	}

	rc = mosquitto_psk_कुञ्चिका_get_default(context, hint, तटस्थः, कुञ्चिका, max_कुञ्चिका_len);
	यदि(rc == MOSQ_ERR_PLUGIN_IGNORE){
		/* Do nothing */
	}अन्यथा यदि(rc == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_PLUGIN_DEFER;
	}अन्यथा{
		प्रत्यागम rc;
	}


	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured. */
	यदि(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_AUTH;
	}
	प्रत्यागम rc_final;
}
