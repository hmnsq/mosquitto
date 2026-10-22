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


स्थिर पूर्णाङ्क plugin__ext_auth_start(संरचना mosquitto__security_options *opts, संरचना mosquitto *context, bool reauth, ध्रुव शून्य *data_in, uint16_t data_in_len, शून्य **data_out, uint16_t *data_out_len)
{
	संरचना mosquitto_evt_extended_auth event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	पूर्णाङ्क rc;
	पूर्णाङ्क rc_final = MOSQ_ERR_PLUGIN_DEFER;

	UNUSED(reauth);

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_start, cb_base, cb_next){
		स_रखो(&event_data, 0, परिमाण(event_data));
		event_data.client = context;
		event_data.auth_method = context->auth_method;
		event_data.data_in = data_in;
		event_data.data_out = रिक्त;
		event_data.data_in_len = data_in_len;
		event_data.data_out_len = 0;
		rc = cb_base->cb(MOSQ_EVT_EXT_AUTH_START, &event_data, cb_base->userdata);
		यदि(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}अन्यथा यदि(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}अन्यथा{
			*data_out = event_data.data_out;
			*data_out_len = event_data.data_out_len;
			प्रत्यागम rc;
		}
	}
	प्रत्यागम rc_final;
}


पूर्णाङ्क mosquitto_security_auth_start(संरचना mosquitto *context, bool reauth, ध्रुव शून्य *data_in, uint16_t data_in_len, शून्य **data_out, uint16_t *data_out_len)
{
	पूर्णाङ्क rc;

	यदि(!context || !context->listener || !context->auth_method){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि(!data_out || !data_out_len){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	यदि(db.config->security_options.plugin_callbacks.ext_auth_start){
		rc = plugin__ext_auth_start(&db.config->security_options, context,
				reauth, data_in, data_in_len, data_out, data_out_len);

		यदि(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}अन्यथा{
			प्रत्यागम rc;
		}
	}

	/* Per listener plugins */
	यदि(context->listener){
		यदि(context->listener->security_options->plugin_callbacks.ext_auth_start){
			rc = plugin__ext_auth_start(context->listener->security_options, context,
					reauth, data_in, data_in_len, data_out, data_out_len);

			यदि(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}अन्यथा{
				प्रत्यागम rc;
			}
		}
	}अन्यथा{
		यदि(db.config->per_listener_settings){
			प्रत्यागम MOSQ_ERR_AUTH;
		}
	}

	प्रत्यागम MOSQ_ERR_NOT_SUPPORTED;
}


स्थिर पूर्णाङ्क plugin__ext_auth_continue(संरचना mosquitto__security_options *opts, संरचना mosquitto *context, ध्रुव शून्य *data_in, uint16_t data_in_len, शून्य **data_out, uint16_t *data_out_len)
{
	पूर्णाङ्क rc;
	संरचना mosquitto_evt_extended_auth event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_continue, cb_base, cb_next){
		स_रखो(&event_data, 0, परिमाण(event_data));
		event_data.client = context;
		event_data.auth_method = context->auth_method;
		event_data.data_in = data_in;
		event_data.data_out = रिक्त;
		event_data.data_in_len = data_in_len;
		event_data.data_out_len = 0;
		rc = cb_base->cb(MOSQ_EVT_EXT_AUTH_CONTINUE, &event_data, cb_base->userdata);
		यदि(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}अन्यथा{
			*data_out = event_data.data_out;
			*data_out_len = event_data.data_out_len;
			प्रत्यागम rc;
		}
	}

	प्रत्यागम MOSQ_ERR_PLUGIN_DEFER;
}


पूर्णाङ्क mosquitto_security_auth_continue(संरचना mosquitto *context, ध्रुव शून्य *data_in, uint16_t data_in_len, शून्य **data_out, uint16_t *data_out_len)
{
	पूर्णाङ्क rc;

	यदि(!context || !context->listener || !context->auth_method){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि(!data_out || !data_out_len){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	यदि(db.config->security_options.plugin_callbacks.ext_auth_continue){
		rc = plugin__ext_auth_continue(&db.config->security_options, context,
				data_in, data_in_len, data_out, data_out_len);

		यदि(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}अन्यथा{
			प्रत्यागम rc;
		}
	}

	/* Per listener plugins */
	यदि(context->listener){
		यदि(context->listener->security_options->plugin_callbacks.ext_auth_continue){
			rc = plugin__ext_auth_continue(context->listener->security_options, context,
					data_in, data_in_len, data_out, data_out_len);

			यदि(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}अन्यथा{
				प्रत्यागम rc;
			}
		}
	}अन्यथा{
		यदि(db.config->per_listener_settings){
			प्रत्यागम MOSQ_ERR_AUTH;
		}
	}

	प्रत्यागम MOSQ_ERR_NOT_SUPPORTED;
}
