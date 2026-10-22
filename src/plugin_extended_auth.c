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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>

#அந்தர்பாவ "mosquitto/broker.h"
#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto/broker_plugin.h"
#அந்தர்பாவ "lib_load.h"
#அந்தர்பாவ "utlist.h"


நிலையான முழு plugin__ext_auth_start(கட்டமைப்பு mosquitto__security_options *opts, கட்டமைப்பு mosquitto *context, bool reauth, மாறிலி வெறுமை *data_in, uint16_t data_in_len, வெறுமை **data_out, uint16_t *data_out_len)
{
	கட்டமைப்பு mosquitto_evt_extended_auth event_data;
	கட்டமைப்பு mosquitto__callback *cb_base, *cb_next;
	முழு rc;
	முழு rc_final = MOSQ_ERR_PLUGIN_DEFER;

	UNUSED(reauth);

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_start, cb_base, cb_next){
		memset(&event_data, 0, அளவு(event_data));
		event_data.client = context;
		event_data.auth_method = context->auth_method;
		event_data.data_in = data_in;
		event_data.data_out = வெற்று;
		event_data.data_in_len = data_in_len;
		event_data.data_out_len = 0;
		rc = cb_base->cb(MOSQ_EVT_EXT_AUTH_START, &event_data, cb_base->userdata);
		எனில்(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}இல்லையெனில் எனில்(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}இல்லையெனில்{
			*data_out = event_data.data_out;
			*data_out_len = event_data.data_out_len;
			திரும்பு rc;
		}
	}
	திரும்பு rc_final;
}


முழு mosquitto_security_auth_start(கட்டமைப்பு mosquitto *context, bool reauth, மாறிலி வெறுமை *data_in, uint16_t data_in_len, வெறுமை **data_out, uint16_t *data_out_len)
{
	முழு rc;

	எனில்(!context || !context->listener || !context->auth_method){
		திரும்பு MOSQ_ERR_INVAL;
	}
	எனில்(!data_out || !data_out_len){
		திரும்பு MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	எனில்(db.config->security_options.plugin_callbacks.ext_auth_start){
		rc = plugin__ext_auth_start(&db.config->security_options, context,
				reauth, data_in, data_in_len, data_out, data_out_len);

		எனில்(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}இல்லையெனில்{
			திரும்பு rc;
		}
	}

	/* Per listener plugins */
	எனில்(context->listener){
		எனில்(context->listener->security_options->plugin_callbacks.ext_auth_start){
			rc = plugin__ext_auth_start(context->listener->security_options, context,
					reauth, data_in, data_in_len, data_out, data_out_len);

			எனில்(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}இல்லையெனில்{
				திரும்பு rc;
			}
		}
	}இல்லையெனில்{
		எனில்(db.config->per_listener_settings){
			திரும்பு MOSQ_ERR_AUTH;
		}
	}

	திரும்பு MOSQ_ERR_NOT_SUPPORTED;
}


நிலையான முழு plugin__ext_auth_continue(கட்டமைப்பு mosquitto__security_options *opts, கட்டமைப்பு mosquitto *context, மாறிலி வெறுமை *data_in, uint16_t data_in_len, வெறுமை **data_out, uint16_t *data_out_len)
{
	முழு rc;
	கட்டமைப்பு mosquitto_evt_extended_auth event_data;
	கட்டமைப்பு mosquitto__callback *cb_base, *cb_next;

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_continue, cb_base, cb_next){
		memset(&event_data, 0, அளவு(event_data));
		event_data.client = context;
		event_data.auth_method = context->auth_method;
		event_data.data_in = data_in;
		event_data.data_out = வெற்று;
		event_data.data_in_len = data_in_len;
		event_data.data_out_len = 0;
		rc = cb_base->cb(MOSQ_EVT_EXT_AUTH_CONTINUE, &event_data, cb_base->userdata);
		எனில்(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}இல்லையெனில்{
			*data_out = event_data.data_out;
			*data_out_len = event_data.data_out_len;
			திரும்பு rc;
		}
	}

	திரும்பு MOSQ_ERR_PLUGIN_DEFER;
}


முழு mosquitto_security_auth_continue(கட்டமைப்பு mosquitto *context, மாறிலி வெறுமை *data_in, uint16_t data_in_len, வெறுமை **data_out, uint16_t *data_out_len)
{
	முழு rc;

	எனில்(!context || !context->listener || !context->auth_method){
		திரும்பு MOSQ_ERR_INVAL;
	}
	எனில்(!data_out || !data_out_len){
		திரும்பு MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	எனில்(db.config->security_options.plugin_callbacks.ext_auth_continue){
		rc = plugin__ext_auth_continue(&db.config->security_options, context,
				data_in, data_in_len, data_out, data_out_len);

		எனில்(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}இல்லையெனில்{
			திரும்பு rc;
		}
	}

	/* Per listener plugins */
	எனில்(context->listener){
		எனில்(context->listener->security_options->plugin_callbacks.ext_auth_continue){
			rc = plugin__ext_auth_continue(context->listener->security_options, context,
					data_in, data_in_len, data_out, data_out_len);

			எனில்(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}இல்லையெனில்{
				திரும்பு rc;
			}
		}
	}இல்லையெனில்{
		எனில்(db.config->per_listener_settings){
			திரும்பு MOSQ_ERR_AUTH;
		}
	}

	திரும்பு MOSQ_ERR_NOT_SUPPORTED;
}
