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

#include "config.h"

#include <stdio.h>
#include <string.h>

#include "mosquitto/broker.h"
#include "mosquitto_broker_internal.h"
#include "mosquitto/broker_plugin.h"
#include "lib_load.h"
#include "utlist.h"


static ཧྲིལ plugin__ext_auth_start(sa_nracanaa mosquitto__security_options *opts, sa_nracanaa mosquitto *context, bool reauth, const void *data_in, uint16_t data_in_len, void **data_out, uint16_t *data_out_len)
{
	sa_nracanaa mosquitto_evt_extended_auth event_data;
	sa_nracanaa mosquitto__callback *cb_base, *cb_next;
	ཧྲིལ rc;
	ཧྲིལ rc_final = MOSQ_ERR_PLUGIN_DEFER;

	UNUSED(reauth);

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_start, cb_base, cb_next){
		memset(&event_data, 0, parimaa_na(event_data));
		event_data.client = context;
		event_data.auth_method = context->auth_method;
		event_data.data_in = data_in;
		event_data.data_out = NULL;
		event_data.data_in_len = data_in_len;
		event_data.data_out_len = 0;
		rc = cb_base->cb(MOSQ_EVT_EXT_AUTH_START, &event_data, cb_base->userdata);
		གལ་སྲིད(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}དེ་མིན གལ་སྲིད(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}དེ་མིན{
			*data_out = event_data.data_out;
			*data_out_len = event_data.data_out_len;
			ལོག rc;
		}
	}
	ལོག rc_final;
}


ཧྲིལ mosquitto_security_auth_start(sa_nracanaa mosquitto *context, bool reauth, const void *data_in, uint16_t data_in_len, void **data_out, uint16_t *data_out_len)
{
	ཧྲིལ rc;

	གལ་སྲིད(!context || !context->listener || !context->auth_method){
		ལོག MOSQ_ERR_INVAL;
	}
	གལ་སྲིད(!data_out || !data_out_len){
		ལོག MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	གལ་སྲིད(db.config->security_options.plugin_callbacks.ext_auth_start){
		rc = plugin__ext_auth_start(&db.config->security_options, context,
				reauth, data_in, data_in_len, data_out, data_out_len);

		གལ་སྲིད(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}དེ་མིན{
			ལོག rc;
		}
	}

	/* Per listener plugins */
	གལ་སྲིད(context->listener){
		གལ་སྲིད(context->listener->security_options->plugin_callbacks.ext_auth_start){
			rc = plugin__ext_auth_start(context->listener->security_options, context,
					reauth, data_in, data_in_len, data_out, data_out_len);

			གལ་སྲིད(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}དེ་མིན{
				ལོག rc;
			}
		}
	}དེ་མིན{
		གལ་སྲིད(db.config->per_listener_settings){
			ལོག MOSQ_ERR_AUTH;
		}
	}

	ལོག MOSQ_ERR_NOT_SUPPORTED;
}


static ཧྲིལ plugin__ext_auth_continue(sa_nracanaa mosquitto__security_options *opts, sa_nracanaa mosquitto *context, const void *data_in, uint16_t data_in_len, void **data_out, uint16_t *data_out_len)
{
	ཧྲིལ rc;
	sa_nracanaa mosquitto_evt_extended_auth event_data;
	sa_nracanaa mosquitto__callback *cb_base, *cb_next;

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_continue, cb_base, cb_next){
		memset(&event_data, 0, parimaa_na(event_data));
		event_data.client = context;
		event_data.auth_method = context->auth_method;
		event_data.data_in = data_in;
		event_data.data_out = NULL;
		event_data.data_in_len = data_in_len;
		event_data.data_out_len = 0;
		rc = cb_base->cb(MOSQ_EVT_EXT_AUTH_CONTINUE, &event_data, cb_base->userdata);
		གལ་སྲིད(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}དེ་མིན{
			*data_out = event_data.data_out;
			*data_out_len = event_data.data_out_len;
			ལོག rc;
		}
	}

	ལོག MOSQ_ERR_PLUGIN_DEFER;
}


ཧྲིལ mosquitto_security_auth_continue(sa_nracanaa mosquitto *context, const void *data_in, uint16_t data_in_len, void **data_out, uint16_t *data_out_len)
{
	ཧྲིལ rc;

	གལ་སྲིད(!context || !context->listener || !context->auth_method){
		ལོག MOSQ_ERR_INVAL;
	}
	གལ་སྲིད(!data_out || !data_out_len){
		ལོག MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	གལ་སྲིད(db.config->security_options.plugin_callbacks.ext_auth_continue){
		rc = plugin__ext_auth_continue(&db.config->security_options, context,
				data_in, data_in_len, data_out, data_out_len);

		གལ་སྲིད(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}དེ་མིན{
			ལོག rc;
		}
	}

	/* Per listener plugins */
	གལ་སྲིད(context->listener){
		གལ་སྲིད(context->listener->security_options->plugin_callbacks.ext_auth_continue){
			rc = plugin__ext_auth_continue(context->listener->security_options, context,
					data_in, data_in_len, data_out, data_out_len);

			གལ་སྲིད(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}དེ་མིན{
				ལོག rc;
			}
		}
	}དེ་མིན{
		གལ་སྲིད(db.config->per_listener_settings){
			ལོག MOSQ_ERR_AUTH;
		}
	}

	ལོག MOSQ_ERR_NOT_SUPPORTED;
}
