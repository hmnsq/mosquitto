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


static цяло plugin__ext_auth_start(struct mosquitto__security_options *opts, struct mosquitto *context, bool reauth, const void *data_in, uint16_t data_in_len, void **data_out, uint16_t *data_out_len)
{
	struct mosquitto_evt_extended_auth event_data;
	struct mosquitto__callback *cb_base, *cb_next;
	цяло rc;
	цяло rc_final = MOSQ_ERR_PLUGIN_DEFER;

	UNUSED(reauth);

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_start, cb_base, cb_next){
		memset(&event_data, 0, sizeof(event_data));
		event_data.client = context;
		event_data.auth_method = context->auth_method;
		event_data.data_in = data_in;
		event_data.data_out = NULL;
		event_data.data_in_len = data_in_len;
		event_data.data_out_len = 0;
		rc = cb_base->cb(MOSQ_EVT_EXT_AUTH_START, &event_data, cb_base->userdata);
		ако(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}иначе ако(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}иначе{
			*data_out = event_data.data_out;
			*data_out_len = event_data.data_out_len;
			върни rc;
		}
	}
	върни rc_final;
}


цяло mosquitto_security_auth_start(struct mosquitto *context, bool reauth, const void *data_in, uint16_t data_in_len, void **data_out, uint16_t *data_out_len)
{
	цяло rc;

	ако(!context || !context->listener || !context->auth_method){
		върни MOSQ_ERR_INVAL;
	}
	ако(!data_out || !data_out_len){
		върни MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	ако(db.config->security_options.plugin_callbacks.ext_auth_start){
		rc = plugin__ext_auth_start(&db.config->security_options, context,
				reauth, data_in, data_in_len, data_out, data_out_len);

		ако(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}иначе{
			върни rc;
		}
	}

	/* Per listener plugins */
	ако(context->listener){
		ако(context->listener->security_options->plugin_callbacks.ext_auth_start){
			rc = plugin__ext_auth_start(context->listener->security_options, context,
					reauth, data_in, data_in_len, data_out, data_out_len);

			ако(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}иначе{
				върни rc;
			}
		}
	}иначе{
		ако(db.config->per_listener_settings){
			върни MOSQ_ERR_AUTH;
		}
	}

	върни MOSQ_ERR_NOT_SUPPORTED;
}


static цяло plugin__ext_auth_continue(struct mosquitto__security_options *opts, struct mosquitto *context, const void *data_in, uint16_t data_in_len, void **data_out, uint16_t *data_out_len)
{
	цяло rc;
	struct mosquitto_evt_extended_auth event_data;
	struct mosquitto__callback *cb_base, *cb_next;

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_continue, cb_base, cb_next){
		memset(&event_data, 0, sizeof(event_data));
		event_data.client = context;
		event_data.auth_method = context->auth_method;
		event_data.data_in = data_in;
		event_data.data_out = NULL;
		event_data.data_in_len = data_in_len;
		event_data.data_out_len = 0;
		rc = cb_base->cb(MOSQ_EVT_EXT_AUTH_CONTINUE, &event_data, cb_base->userdata);
		ако(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}иначе{
			*data_out = event_data.data_out;
			*data_out_len = event_data.data_out_len;
			върни rc;
		}
	}

	върни MOSQ_ERR_PLUGIN_DEFER;
}


цяло mosquitto_security_auth_continue(struct mosquitto *context, const void *data_in, uint16_t data_in_len, void **data_out, uint16_t *data_out_len)
{
	цяло rc;

	ако(!context || !context->listener || !context->auth_method){
		върни MOSQ_ERR_INVAL;
	}
	ако(!data_out || !data_out_len){
		върни MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	ако(db.config->security_options.plugin_callbacks.ext_auth_continue){
		rc = plugin__ext_auth_continue(&db.config->security_options, context,
				data_in, data_in_len, data_out, data_out_len);

		ако(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}иначе{
			върни rc;
		}
	}

	/* Per listener plugins */
	ако(context->listener){
		ако(context->listener->security_options->plugin_callbacks.ext_auth_continue){
			rc = plugin__ext_auth_continue(context->listener->security_options, context,
					data_in, data_in_len, data_out, data_out_len);

			ако(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}иначе{
				върни rc;
			}
		}
	}иначе{
		ако(db.config->per_listener_settings){
			върни MOSQ_ERR_AUTH;
		}
	}

	върни MOSQ_ERR_NOT_SUPPORTED;
}
