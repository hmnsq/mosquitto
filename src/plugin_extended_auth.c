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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "mosquitto/broker.h"
#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/broker_plugin.h"
#ಅನ್ತರ್ಭಾವ "lib_load.h"
#ಅನ್ತರ್ಭಾವ "utlist.h"


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ plugin__ext_auth_start(ಸಂರಚನಾ mosquitto__security_options *opts, ಸಂರಚನಾ mosquitto *context, bool reauth, ಧ್ರುವ ಶೂನ್ಯ *data_in, uint16_t data_in_len, ಶೂನ್ಯ **data_out, uint16_t *data_out_len)
{
	ಸಂರಚನಾ mosquitto_evt_extended_auth event_data;
	ಸಂರಚನಾ mosquitto__callback *cb_base, *cb_next;
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಪೂರ್ಣಾಙ್ಕ rc_final = MOSQ_ERR_PLUGIN_DEFER;

	UNUSED(reauth);

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_start, cb_base, cb_next){
		memset(&event_data, 0, ಪರಿಮಾಣ(event_data));
		event_data.client = context;
		event_data.auth_method = context->auth_method;
		event_data.data_in = data_in;
		event_data.data_out = ರಿಕ್ತ;
		event_data.data_in_len = data_in_len;
		event_data.data_out_len = 0;
		rc = cb_base->cb(MOSQ_EVT_EXT_AUTH_START, &event_data, cb_base->userdata);
		ಯದಿ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}ಅನ್ಯಥಾ{
			*data_out = event_data.data_out;
			*data_out_len = event_data.data_out_len;
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}
	ಪ್ರತ್ಯಾಗಮ rc_final;
}


ಪೂರ್ಣಾಙ್ಕ mosquitto_security_auth_start(ಸಂರಚನಾ mosquitto *context, bool reauth, ಧ್ರುವ ಶೂನ್ಯ *data_in, uint16_t data_in_len, ಶೂನ್ಯ **data_out, uint16_t *data_out_len)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ(!context || !context->listener || !context->auth_method){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}
	ಯದಿ(!data_out || !data_out_len){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	ಯದಿ(db.config->security_options.plugin_callbacks.ext_auth_start){
		rc = plugin__ext_auth_start(&db.config->security_options, context,
				reauth, data_in, data_in_len, data_out, data_out_len);

		ಯದಿ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}ಅನ್ಯಥಾ{
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}

	/* Per listener plugins */
	ಯದಿ(context->listener){
		ಯದಿ(context->listener->security_options->plugin_callbacks.ext_auth_start){
			rc = plugin__ext_auth_start(context->listener->security_options, context,
					reauth, data_in, data_in_len, data_out, data_out_len);

			ಯದಿ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}ಅನ್ಯಥಾ{
				ಪ್ರತ್ಯಾಗಮ rc;
			}
		}
	}ಅನ್ಯಥಾ{
		ಯದಿ(db.config->per_listener_settings){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_AUTH;
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_SUPPORTED;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ plugin__ext_auth_continue(ಸಂರಚನಾ mosquitto__security_options *opts, ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ಶೂನ್ಯ *data_in, uint16_t data_in_len, ಶೂನ್ಯ **data_out, uint16_t *data_out_len)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ mosquitto_evt_extended_auth event_data;
	ಸಂರಚನಾ mosquitto__callback *cb_base, *cb_next;

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_continue, cb_base, cb_next){
		memset(&event_data, 0, ಪರಿಮಾಣ(event_data));
		event_data.client = context;
		event_data.auth_method = context->auth_method;
		event_data.data_in = data_in;
		event_data.data_out = ರಿಕ್ತ;
		event_data.data_in_len = data_in_len;
		event_data.data_out_len = 0;
		rc = cb_base->cb(MOSQ_EVT_EXT_AUTH_CONTINUE, &event_data, cb_base->userdata);
		ಯದಿ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}ಅನ್ಯಥಾ{
			*data_out = event_data.data_out;
			*data_out_len = event_data.data_out_len;
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PLUGIN_DEFER;
}


ಪೂರ್ಣಾಙ್ಕ mosquitto_security_auth_continue(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ಶೂನ್ಯ *data_in, uint16_t data_in_len, ಶೂನ್ಯ **data_out, uint16_t *data_out_len)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ(!context || !context->listener || !context->auth_method){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}
	ಯದಿ(!data_out || !data_out_len){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	ಯದಿ(db.config->security_options.plugin_callbacks.ext_auth_continue){
		rc = plugin__ext_auth_continue(&db.config->security_options, context,
				data_in, data_in_len, data_out, data_out_len);

		ಯದಿ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}ಅನ್ಯಥಾ{
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}

	/* Per listener plugins */
	ಯದಿ(context->listener){
		ಯದಿ(context->listener->security_options->plugin_callbacks.ext_auth_continue){
			rc = plugin__ext_auth_continue(context->listener->security_options, context,
					data_in, data_in_len, data_out, data_out_len);

			ಯದಿ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}ಅನ್ಯಥಾ{
				ಪ್ರತ್ಯಾಗಮ rc;
			}
		}
	}ಅನ್ಯಥಾ{
		ಯದಿ(db.config->per_listener_settings){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_AUTH;
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_SUPPORTED;
}
