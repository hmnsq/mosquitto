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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "mosquitto/broker.h"
#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/broker_plugin.h"
#അന്തര്ഭാവ "lib_load.h"
#അന്തര്ഭാവ "utlist.h"


സ്ഥിതം പൂർണ്ണസംഖ്യ plugin__ext_auth_start(ഘടന mosquitto__security_options *opts, ഘടന mosquitto *context, bool reauth, സ്ഥിരം ശൂന്യം *data_in, uint16_t data_in_len, ശൂന്യം **data_out, uint16_t *data_out_len)
{
	ഘടന mosquitto_evt_extended_auth event_data;
	ഘടന mosquitto__callback *cb_base, *cb_next;
	പൂർണ്ണസംഖ്യ rc;
	പൂർണ്ണസംഖ്യ rc_final = MOSQ_ERR_PLUGIN_DEFER;

	UNUSED(reauth);

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_start, cb_base, cb_next){
		memset(&event_data, 0, വലുപ്പം(event_data));
		event_data.client = context;
		event_data.auth_method = context->auth_method;
		event_data.data_in = data_in;
		event_data.data_out = ശൂന്യ;
		event_data.data_in_len = data_in_len;
		event_data.data_out_len = 0;
		rc = cb_base->cb(MOSQ_EVT_EXT_AUTH_START, &event_data, cb_base->userdata);
		എങ്കിൽ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}അല്ലെങ്കിൽ{
			*data_out = event_data.data_out;
			*data_out_len = event_data.data_out_len;
			തിരികെ rc;
		}
	}
	തിരികെ rc_final;
}


പൂർണ്ണസംഖ്യ mosquitto_security_auth_start(ഘടന mosquitto *context, bool reauth, സ്ഥിരം ശൂന്യം *data_in, uint16_t data_in_len, ശൂന്യം **data_out, uint16_t *data_out_len)
{
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ(!context || !context->listener || !context->auth_method){
		തിരികെ MOSQ_ERR_INVAL;
	}
	എങ്കിൽ(!data_out || !data_out_len){
		തിരികെ MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	എങ്കിൽ(db.config->security_options.plugin_callbacks.ext_auth_start){
		rc = plugin__ext_auth_start(&db.config->security_options, context,
				reauth, data_in, data_in_len, data_out, data_out_len);

		എങ്കിൽ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}അല്ലെങ്കിൽ{
			തിരികെ rc;
		}
	}

	/* Per listener plugins */
	എങ്കിൽ(context->listener){
		എങ്കിൽ(context->listener->security_options->plugin_callbacks.ext_auth_start){
			rc = plugin__ext_auth_start(context->listener->security_options, context,
					reauth, data_in, data_in_len, data_out, data_out_len);

			എങ്കിൽ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}അല്ലെങ്കിൽ{
				തിരികെ rc;
			}
		}
	}അല്ലെങ്കിൽ{
		എങ്കിൽ(db.config->per_listener_settings){
			തിരികെ MOSQ_ERR_AUTH;
		}
	}

	തിരികെ MOSQ_ERR_NOT_SUPPORTED;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ plugin__ext_auth_continue(ഘടന mosquitto__security_options *opts, ഘടന mosquitto *context, സ്ഥിരം ശൂന്യം *data_in, uint16_t data_in_len, ശൂന്യം **data_out, uint16_t *data_out_len)
{
	പൂർണ്ണസംഖ്യ rc;
	ഘടന mosquitto_evt_extended_auth event_data;
	ഘടന mosquitto__callback *cb_base, *cb_next;

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_continue, cb_base, cb_next){
		memset(&event_data, 0, വലുപ്പം(event_data));
		event_data.client = context;
		event_data.auth_method = context->auth_method;
		event_data.data_in = data_in;
		event_data.data_out = ശൂന്യ;
		event_data.data_in_len = data_in_len;
		event_data.data_out_len = 0;
		rc = cb_base->cb(MOSQ_EVT_EXT_AUTH_CONTINUE, &event_data, cb_base->userdata);
		എങ്കിൽ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}അല്ലെങ്കിൽ{
			*data_out = event_data.data_out;
			*data_out_len = event_data.data_out_len;
			തിരികെ rc;
		}
	}

	തിരികെ MOSQ_ERR_PLUGIN_DEFER;
}


പൂർണ്ണസംഖ്യ mosquitto_security_auth_continue(ഘടന mosquitto *context, സ്ഥിരം ശൂന്യം *data_in, uint16_t data_in_len, ശൂന്യം **data_out, uint16_t *data_out_len)
{
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ(!context || !context->listener || !context->auth_method){
		തിരികെ MOSQ_ERR_INVAL;
	}
	എങ്കിൽ(!data_out || !data_out_len){
		തിരികെ MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	എങ്കിൽ(db.config->security_options.plugin_callbacks.ext_auth_continue){
		rc = plugin__ext_auth_continue(&db.config->security_options, context,
				data_in, data_in_len, data_out, data_out_len);

		എങ്കിൽ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}അല്ലെങ്കിൽ{
			തിരികെ rc;
		}
	}

	/* Per listener plugins */
	എങ്കിൽ(context->listener){
		എങ്കിൽ(context->listener->security_options->plugin_callbacks.ext_auth_continue){
			rc = plugin__ext_auth_continue(context->listener->security_options, context,
					data_in, data_in_len, data_out, data_out_len);

			എങ്കിൽ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}അല്ലെങ്കിൽ{
				തിരികെ rc;
			}
		}
	}അല്ലെങ്കിൽ{
		എങ്കിൽ(db.config->per_listener_settings){
			തിരികെ MOSQ_ERR_AUTH;
		}
	}

	തിരികെ MOSQ_ERR_NOT_SUPPORTED;
}
