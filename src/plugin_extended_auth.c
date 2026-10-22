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

#ضمن "config.h"

#ضمن <stdio.h>
#ضمن <string.h>

#ضمن "mosquitto/broker.h"
#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/broker_plugin.h"
#ضمن "lib_load.h"
#ضمن "utlist.h"


ساكن صحيح plugin__ext_auth_start(بنية mosquitto__security_options *opts, بنية mosquitto *context, bool reauth, ثابت فراغ *data_in, uint16_t data_in_len, فراغ **data_out, uint16_t *data_out_len)
{
	بنية mosquitto_evt_extended_auth event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	صحيح rc;
	صحيح rc_final = MOSQ_ERR_PLUGIN_DEFER;

	UNUSED(reauth);

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_start, cb_base, cb_next){
		memset(&event_data, 0, حجم(event_data));
		event_data.client = context;
		event_data.auth_method = context->auth_method;
		event_data.data_in = data_in;
		event_data.data_out = عدم;
		event_data.data_in_len = data_in_len;
		event_data.data_out_len = 0;
		rc = cb_base->cb(MOSQ_EVT_EXT_AUTH_START, &event_data, cb_base->userdata);
		ܐܢ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}ܐܚܪܢܐ ܐܢ(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}ܐܚܪܢܐ{
			*data_out = event_data.data_out;
			*data_out_len = event_data.data_out_len;
			ܦܢܐ rc;
		}
	}
	ܦܢܐ rc_final;
}


صحيح mosquitto_security_auth_start(بنية mosquitto *context, bool reauth, ثابت فراغ *data_in, uint16_t data_in_len, فراغ **data_out, uint16_t *data_out_len)
{
	صحيح rc;

	ܐܢ(!context || !context->listener || !context->auth_method){
		ܦܢܐ MOSQ_ERR_INVAL;
	}
	ܐܢ(!data_out || !data_out_len){
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	ܐܢ(db.config->security_options.plugin_callbacks.ext_auth_start){
		rc = plugin__ext_auth_start(&db.config->security_options, context,
				reauth, data_in, data_in_len, data_out, data_out_len);

		ܐܢ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}ܐܚܪܢܐ{
			ܦܢܐ rc;
		}
	}

	/* Per listener plugins */
	ܐܢ(context->listener){
		ܐܢ(context->listener->security_options->plugin_callbacks.ext_auth_start){
			rc = plugin__ext_auth_start(context->listener->security_options, context,
					reauth, data_in, data_in_len, data_out, data_out_len);

			ܐܢ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}ܐܚܪܢܐ{
				ܦܢܐ rc;
			}
		}
	}ܐܚܪܢܐ{
		ܐܢ(db.config->per_listener_settings){
			ܦܢܐ MOSQ_ERR_AUTH;
		}
	}

	ܦܢܐ MOSQ_ERR_NOT_SUPPORTED;
}


ساكن صحيح plugin__ext_auth_continue(بنية mosquitto__security_options *opts, بنية mosquitto *context, ثابت فراغ *data_in, uint16_t data_in_len, فراغ **data_out, uint16_t *data_out_len)
{
	صحيح rc;
	بنية mosquitto_evt_extended_auth event_data;
	بنية mosquitto__callback *cb_base, *cb_next;

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_continue, cb_base, cb_next){
		memset(&event_data, 0, حجم(event_data));
		event_data.client = context;
		event_data.auth_method = context->auth_method;
		event_data.data_in = data_in;
		event_data.data_out = عدم;
		event_data.data_in_len = data_in_len;
		event_data.data_out_len = 0;
		rc = cb_base->cb(MOSQ_EVT_EXT_AUTH_CONTINUE, &event_data, cb_base->userdata);
		ܐܢ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}ܐܚܪܢܐ{
			*data_out = event_data.data_out;
			*data_out_len = event_data.data_out_len;
			ܦܢܐ rc;
		}
	}

	ܦܢܐ MOSQ_ERR_PLUGIN_DEFER;
}


صحيح mosquitto_security_auth_continue(بنية mosquitto *context, ثابت فراغ *data_in, uint16_t data_in_len, فراغ **data_out, uint16_t *data_out_len)
{
	صحيح rc;

	ܐܢ(!context || !context->listener || !context->auth_method){
		ܦܢܐ MOSQ_ERR_INVAL;
	}
	ܐܢ(!data_out || !data_out_len){
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	ܐܢ(db.config->security_options.plugin_callbacks.ext_auth_continue){
		rc = plugin__ext_auth_continue(&db.config->security_options, context,
				data_in, data_in_len, data_out, data_out_len);

		ܐܢ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}ܐܚܪܢܐ{
			ܦܢܐ rc;
		}
	}

	/* Per listener plugins */
	ܐܢ(context->listener){
		ܐܢ(context->listener->security_options->plugin_callbacks.ext_auth_continue){
			rc = plugin__ext_auth_continue(context->listener->security_options, context,
					data_in, data_in_len, data_out, data_out_len);

			ܐܢ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}ܐܚܪܢܐ{
				ܦܢܐ rc;
			}
		}
	}ܐܚܪܢܐ{
		ܐܢ(db.config->per_listener_settings){
			ܦܢܐ MOSQ_ERR_AUTH;
		}
	}

	ܦܢܐ MOSQ_ERR_NOT_SUPPORTED;
}
