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

#شامل "config.h"

#شامل <stdio.h>
#شامل <string.h>

#شامل "mosquitto/broker.h"
#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/broker_plugin.h"
#شامل "lib_load.h"
#شامل "utlist.h"


ایستا صحیح plugin__ext_auth_start(ساختار mosquitto__security_options *opts, ساختار mosquitto *context, bool reauth, ثابت تهی *data_in, uint16_t data_in_len, تهی **data_out, uint16_t *data_out_len)
{
	ساختار mosquitto_evt_extended_auth event_data;
	ساختار mosquitto__callback *cb_base, *cb_next;
	صحیح rc;
	صحیح rc_final = MOSQ_ERR_PLUGIN_DEFER;

	UNUSED(reauth);

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_start, cb_base, cb_next){
		memset(&event_data, 0, اندازه(event_data));
		event_data.client = context;
		event_data.auth_method = context->auth_method;
		event_data.data_in = data_in;
		event_data.data_out = پوچ;
		event_data.data_in_len = data_in_len;
		event_data.data_out_len = 0;
		rc = cb_base->cb(MOSQ_EVT_EXT_AUTH_START, &event_data, cb_base->userdata);
		اگر(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}وگرنه اگر(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}وگرنه{
			*data_out = event_data.data_out;
			*data_out_len = event_data.data_out_len;
			بازگشت rc;
		}
	}
	بازگشت rc_final;
}


صحیح mosquitto_security_auth_start(ساختار mosquitto *context, bool reauth, ثابت تهی *data_in, uint16_t data_in_len, تهی **data_out, uint16_t *data_out_len)
{
	صحیح rc;

	اگر(!context || !context->listener || !context->auth_method){
		بازگشت MOSQ_ERR_INVAL;
	}
	اگر(!data_out || !data_out_len){
		بازگشت MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	اگر(db.config->security_options.plugin_callbacks.ext_auth_start){
		rc = plugin__ext_auth_start(&db.config->security_options, context,
				reauth, data_in, data_in_len, data_out, data_out_len);

		اگر(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}وگرنه{
			بازگشت rc;
		}
	}

	/* Per listener plugins */
	اگر(context->listener){
		اگر(context->listener->security_options->plugin_callbacks.ext_auth_start){
			rc = plugin__ext_auth_start(context->listener->security_options, context,
					reauth, data_in, data_in_len, data_out, data_out_len);

			اگر(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}وگرنه{
				بازگشت rc;
			}
		}
	}وگرنه{
		اگر(db.config->per_listener_settings){
			بازگشت MOSQ_ERR_AUTH;
		}
	}

	بازگشت MOSQ_ERR_NOT_SUPPORTED;
}


ایستا صحیح plugin__ext_auth_continue(ساختار mosquitto__security_options *opts, ساختار mosquitto *context, ثابت تهی *data_in, uint16_t data_in_len, تهی **data_out, uint16_t *data_out_len)
{
	صحیح rc;
	ساختار mosquitto_evt_extended_auth event_data;
	ساختار mosquitto__callback *cb_base, *cb_next;

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_continue, cb_base, cb_next){
		memset(&event_data, 0, اندازه(event_data));
		event_data.client = context;
		event_data.auth_method = context->auth_method;
		event_data.data_in = data_in;
		event_data.data_out = پوچ;
		event_data.data_in_len = data_in_len;
		event_data.data_out_len = 0;
		rc = cb_base->cb(MOSQ_EVT_EXT_AUTH_CONTINUE, &event_data, cb_base->userdata);
		اگر(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}وگرنه{
			*data_out = event_data.data_out;
			*data_out_len = event_data.data_out_len;
			بازگشت rc;
		}
	}

	بازگشت MOSQ_ERR_PLUGIN_DEFER;
}


صحیح mosquitto_security_auth_continue(ساختار mosquitto *context, ثابت تهی *data_in, uint16_t data_in_len, تهی **data_out, uint16_t *data_out_len)
{
	صحیح rc;

	اگر(!context || !context->listener || !context->auth_method){
		بازگشت MOSQ_ERR_INVAL;
	}
	اگر(!data_out || !data_out_len){
		بازگشت MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	اگر(db.config->security_options.plugin_callbacks.ext_auth_continue){
		rc = plugin__ext_auth_continue(&db.config->security_options, context,
				data_in, data_in_len, data_out, data_out_len);

		اگر(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}وگرنه{
			بازگشت rc;
		}
	}

	/* Per listener plugins */
	اگر(context->listener){
		اگر(context->listener->security_options->plugin_callbacks.ext_auth_continue){
			rc = plugin__ext_auth_continue(context->listener->security_options, context,
					data_in, data_in_len, data_out, data_out_len);

			اگر(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}وگرنه{
				بازگشت rc;
			}
		}
	}وگرنه{
		اگر(db.config->per_listener_settings){
			بازگشت MOSQ_ERR_AUTH;
		}
	}

	بازگشت MOSQ_ERR_NOT_SUPPORTED;
}
