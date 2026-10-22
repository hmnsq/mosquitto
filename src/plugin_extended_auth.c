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


static ចំនួនគត់ plugin__ext_auth_start(sa_nracanaa mosquitto__security_options *opts, sa_nracanaa mosquitto *context, bool reauth, const void *data_in, uint16_t data_in_len, void **data_out, uint16_t *data_out_len)
{
	sa_nracanaa mosquitto_evt_extended_auth event_data;
	sa_nracanaa mosquitto__callback *cb_base, *cb_next;
	ចំនួនគត់ rc;
	ចំនួនគត់ rc_final = MOSQ_ERR_PLUGIN_DEFER;

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
		ប្រសិនបើ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}បើមិនដូច្នេះ ប្រសិនបើ(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}បើមិនដូច្នេះ{
			*data_out = event_data.data_out;
			*data_out_len = event_data.data_out_len;
			ត្រឡប់ rc;
		}
	}
	ត្រឡប់ rc_final;
}


ចំនួនគត់ mosquitto_security_auth_start(sa_nracanaa mosquitto *context, bool reauth, const void *data_in, uint16_t data_in_len, void **data_out, uint16_t *data_out_len)
{
	ចំនួនគត់ rc;

	ប្រសិនបើ(!context || !context->listener || !context->auth_method){
		ត្រឡប់ MOSQ_ERR_INVAL;
	}
	ប្រសិនបើ(!data_out || !data_out_len){
		ត្រឡប់ MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	ប្រសិនបើ(db.config->security_options.plugin_callbacks.ext_auth_start){
		rc = plugin__ext_auth_start(&db.config->security_options, context,
				reauth, data_in, data_in_len, data_out, data_out_len);

		ប្រសិនបើ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}បើមិនដូច្នេះ{
			ត្រឡប់ rc;
		}
	}

	/* Per listener plugins */
	ប្រសិនបើ(context->listener){
		ប្រសិនបើ(context->listener->security_options->plugin_callbacks.ext_auth_start){
			rc = plugin__ext_auth_start(context->listener->security_options, context,
					reauth, data_in, data_in_len, data_out, data_out_len);

			ប្រសិនបើ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}បើមិនដូច្នេះ{
				ត្រឡប់ rc;
			}
		}
	}បើមិនដូច្នេះ{
		ប្រសិនបើ(db.config->per_listener_settings){
			ត្រឡប់ MOSQ_ERR_AUTH;
		}
	}

	ត្រឡប់ MOSQ_ERR_NOT_SUPPORTED;
}


static ចំនួនគត់ plugin__ext_auth_continue(sa_nracanaa mosquitto__security_options *opts, sa_nracanaa mosquitto *context, const void *data_in, uint16_t data_in_len, void **data_out, uint16_t *data_out_len)
{
	ចំនួនគត់ rc;
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
		ប្រសិនបើ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}បើមិនដូច្នេះ{
			*data_out = event_data.data_out;
			*data_out_len = event_data.data_out_len;
			ត្រឡប់ rc;
		}
	}

	ត្រឡប់ MOSQ_ERR_PLUGIN_DEFER;
}


ចំនួនគត់ mosquitto_security_auth_continue(sa_nracanaa mosquitto *context, const void *data_in, uint16_t data_in_len, void **data_out, uint16_t *data_out_len)
{
	ចំនួនគត់ rc;

	ប្រសិនបើ(!context || !context->listener || !context->auth_method){
		ត្រឡប់ MOSQ_ERR_INVAL;
	}
	ប្រសិនបើ(!data_out || !data_out_len){
		ត្រឡប់ MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	ប្រសិនបើ(db.config->security_options.plugin_callbacks.ext_auth_continue){
		rc = plugin__ext_auth_continue(&db.config->security_options, context,
				data_in, data_in_len, data_out, data_out_len);

		ប្រសិនបើ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}បើមិនដូច្នេះ{
			ត្រឡប់ rc;
		}
	}

	/* Per listener plugins */
	ប្រសិនបើ(context->listener){
		ប្រសិនបើ(context->listener->security_options->plugin_callbacks.ext_auth_continue){
			rc = plugin__ext_auth_continue(context->listener->security_options, context,
					data_in, data_in_len, data_out, data_out_len);

			ប្រសិនបើ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}បើមិនដូច្នេះ{
				ត្រឡប់ rc;
			}
		}
	}បើមិនដូច្នេះ{
		ប្រសិនបើ(db.config->per_listener_settings){
			ត្រឡប់ MOSQ_ERR_AUTH;
		}
	}

	ត្រឡប់ MOSQ_ERR_NOT_SUPPORTED;
}
