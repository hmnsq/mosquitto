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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "mosquitto/broker.h"
#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/broker_plugin.h"
#અન્તર્ભાવ "lib_load.h"
#અન્તર્ભાવ "utlist.h"


સ્થિર પૂર્ણાંક plugin__ext_auth_start(સંરચના mosquitto__security_options *opts, સંરચના mosquitto *context, bool reauth, અચલ શૂન્ય *data_in, uint16_t data_in_len, શૂન્ય **data_out, uint16_t *data_out_len)
{
	સંરચના mosquitto_evt_extended_auth event_data;
	સંરચના mosquitto__callback *cb_base, *cb_next;
	પૂર્ણાંક rc;
	પૂર્ણાંક rc_final = MOSQ_ERR_PLUGIN_DEFER;

	UNUSED(reauth);

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_start, cb_base, cb_next){
		memset(&event_data, 0, કદ(event_data));
		event_data.client = context;
		event_data.auth_method = context->auth_method;
		event_data.data_in = data_in;
		event_data.data_out = ખાલી;
		event_data.data_in_len = data_in_len;
		event_data.data_out_len = 0;
		rc = cb_base->cb(MOSQ_EVT_EXT_AUTH_START, &event_data, cb_base->userdata);
		જો(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}નહીંતર જો(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}નહીંતર{
			*data_out = event_data.data_out;
			*data_out_len = event_data.data_out_len;
			પાછા rc;
		}
	}
	પાછા rc_final;
}


પૂર્ણાંક mosquitto_security_auth_start(સંરચના mosquitto *context, bool reauth, અચલ શૂન્ય *data_in, uint16_t data_in_len, શૂન્ય **data_out, uint16_t *data_out_len)
{
	પૂર્ણાંક rc;

	જો(!context || !context->listener || !context->auth_method){
		પાછા MOSQ_ERR_INVAL;
	}
	જો(!data_out || !data_out_len){
		પાછા MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	જો(db.config->security_options.plugin_callbacks.ext_auth_start){
		rc = plugin__ext_auth_start(&db.config->security_options, context,
				reauth, data_in, data_in_len, data_out, data_out_len);

		જો(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}નહીંતર{
			પાછા rc;
		}
	}

	/* Per listener plugins */
	જો(context->listener){
		જો(context->listener->security_options->plugin_callbacks.ext_auth_start){
			rc = plugin__ext_auth_start(context->listener->security_options, context,
					reauth, data_in, data_in_len, data_out, data_out_len);

			જો(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}નહીંતર{
				પાછા rc;
			}
		}
	}નહીંતર{
		જો(db.config->per_listener_settings){
			પાછા MOSQ_ERR_AUTH;
		}
	}

	પાછા MOSQ_ERR_NOT_SUPPORTED;
}


સ્થિર પૂર્ણાંક plugin__ext_auth_continue(સંરચના mosquitto__security_options *opts, સંરચના mosquitto *context, અચલ શૂન્ય *data_in, uint16_t data_in_len, શૂન્ય **data_out, uint16_t *data_out_len)
{
	પૂર્ણાંક rc;
	સંરચના mosquitto_evt_extended_auth event_data;
	સંરચના mosquitto__callback *cb_base, *cb_next;

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_continue, cb_base, cb_next){
		memset(&event_data, 0, કદ(event_data));
		event_data.client = context;
		event_data.auth_method = context->auth_method;
		event_data.data_in = data_in;
		event_data.data_out = ખાલી;
		event_data.data_in_len = data_in_len;
		event_data.data_out_len = 0;
		rc = cb_base->cb(MOSQ_EVT_EXT_AUTH_CONTINUE, &event_data, cb_base->userdata);
		જો(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}નહીંતર{
			*data_out = event_data.data_out;
			*data_out_len = event_data.data_out_len;
			પાછા rc;
		}
	}

	પાછા MOSQ_ERR_PLUGIN_DEFER;
}


પૂર્ણાંક mosquitto_security_auth_continue(સંરચના mosquitto *context, અચલ શૂન્ય *data_in, uint16_t data_in_len, શૂન્ય **data_out, uint16_t *data_out_len)
{
	પૂર્ણાંક rc;

	જો(!context || !context->listener || !context->auth_method){
		પાછા MOSQ_ERR_INVAL;
	}
	જો(!data_out || !data_out_len){
		પાછા MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	જો(db.config->security_options.plugin_callbacks.ext_auth_continue){
		rc = plugin__ext_auth_continue(&db.config->security_options, context,
				data_in, data_in_len, data_out, data_out_len);

		જો(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}નહીંતર{
			પાછા rc;
		}
	}

	/* Per listener plugins */
	જો(context->listener){
		જો(context->listener->security_options->plugin_callbacks.ext_auth_continue){
			rc = plugin__ext_auth_continue(context->listener->security_options, context,
					data_in, data_in_len, data_out, data_out_len);

			જો(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}નહીંતર{
				પાછા rc;
			}
		}
	}નહીંતર{
		જો(db.config->per_listener_settings){
			પાછા MOSQ_ERR_AUTH;
		}
	}

	પાછા MOSQ_ERR_NOT_SUPPORTED;
}
