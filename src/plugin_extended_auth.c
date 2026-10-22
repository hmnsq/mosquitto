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

#অন্তর্ভাৱ "config.h"

#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <string.h>

#অন্তর্ভাৱ "mosquitto/broker.h"
#অন্তর্ভাৱ "mosquitto_broker_internal.h"
#অন্তর্ভাৱ "mosquitto/broker_plugin.h"
#অন্তর্ভাৱ "lib_load.h"
#অন্তর্ভাৱ "utlist.h"


স্থির পূর্ণাঙ্ক plugin__ext_auth_start(সংরচনা mosquitto__security_options *opts, সংরচনা mosquitto *context, bool reauth, ধ্রুৱ শূন্য *data_in, uint16_t data_in_len, শূন্য **data_out, uint16_t *data_out_len)
{
	সংরচনা mosquitto_evt_extended_auth event_data;
	সংরচনা mosquitto__callback *cb_base, *cb_next;
	পূর্ণাঙ্ক rc;
	পূর্ণাঙ্ক rc_final = MOSQ_ERR_PLUGIN_DEFER;

	UNUSED(reauth);

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_start, cb_base, cb_next){
		memset(&event_data, 0, পরিমাণ(event_data));
		event_data.client = context;
		event_data.auth_method = context->auth_method;
		event_data.data_in = data_in;
		event_data.data_out = রিক্ত;
		event_data.data_in_len = data_in_len;
		event_data.data_out_len = 0;
		rc = cb_base->cb(MOSQ_EVT_EXT_AUTH_START, &event_data, cb_base->userdata);
		যদি(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}অন্যথা যদি(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}অন্যথা{
			*data_out = event_data.data_out;
			*data_out_len = event_data.data_out_len;
			ঘূৰাই rc;
		}
	}
	ঘূৰাই rc_final;
}


পূর্ণাঙ্ক mosquitto_security_auth_start(সংরচনা mosquitto *context, bool reauth, ধ্রুৱ শূন্য *data_in, uint16_t data_in_len, শূন্য **data_out, uint16_t *data_out_len)
{
	পূর্ণাঙ্ক rc;

	যদি(!context || !context->listener || !context->auth_method){
		ঘূৰাই MOSQ_ERR_INVAL;
	}
	যদি(!data_out || !data_out_len){
		ঘূৰাই MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	যদি(db.config->security_options.plugin_callbacks.ext_auth_start){
		rc = plugin__ext_auth_start(&db.config->security_options, context,
				reauth, data_in, data_in_len, data_out, data_out_len);

		যদি(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}অন্যথা{
			ঘূৰাই rc;
		}
	}

	/* Per listener plugins */
	যদি(context->listener){
		যদি(context->listener->security_options->plugin_callbacks.ext_auth_start){
			rc = plugin__ext_auth_start(context->listener->security_options, context,
					reauth, data_in, data_in_len, data_out, data_out_len);

			যদি(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}অন্যথা{
				ঘূৰাই rc;
			}
		}
	}অন্যথা{
		যদি(db.config->per_listener_settings){
			ঘূৰাই MOSQ_ERR_AUTH;
		}
	}

	ঘূৰাই MOSQ_ERR_NOT_SUPPORTED;
}


স্থির পূর্ণাঙ্ক plugin__ext_auth_continue(সংরচনা mosquitto__security_options *opts, সংরচনা mosquitto *context, ধ্রুৱ শূন্য *data_in, uint16_t data_in_len, শূন্য **data_out, uint16_t *data_out_len)
{
	পূর্ণাঙ্ক rc;
	সংরচনা mosquitto_evt_extended_auth event_data;
	সংরচনা mosquitto__callback *cb_base, *cb_next;

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_continue, cb_base, cb_next){
		memset(&event_data, 0, পরিমাণ(event_data));
		event_data.client = context;
		event_data.auth_method = context->auth_method;
		event_data.data_in = data_in;
		event_data.data_out = রিক্ত;
		event_data.data_in_len = data_in_len;
		event_data.data_out_len = 0;
		rc = cb_base->cb(MOSQ_EVT_EXT_AUTH_CONTINUE, &event_data, cb_base->userdata);
		যদি(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}অন্যথা{
			*data_out = event_data.data_out;
			*data_out_len = event_data.data_out_len;
			ঘূৰাই rc;
		}
	}

	ঘূৰাই MOSQ_ERR_PLUGIN_DEFER;
}


পূর্ণাঙ্ক mosquitto_security_auth_continue(সংরচনা mosquitto *context, ধ্রুৱ শূন্য *data_in, uint16_t data_in_len, শূন্য **data_out, uint16_t *data_out_len)
{
	পূর্ণাঙ্ক rc;

	যদি(!context || !context->listener || !context->auth_method){
		ঘূৰাই MOSQ_ERR_INVAL;
	}
	যদি(!data_out || !data_out_len){
		ঘূৰাই MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	যদি(db.config->security_options.plugin_callbacks.ext_auth_continue){
		rc = plugin__ext_auth_continue(&db.config->security_options, context,
				data_in, data_in_len, data_out, data_out_len);

		যদি(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}অন্যথা{
			ঘূৰাই rc;
		}
	}

	/* Per listener plugins */
	যদি(context->listener){
		যদি(context->listener->security_options->plugin_callbacks.ext_auth_continue){
			rc = plugin__ext_auth_continue(context->listener->security_options, context,
					data_in, data_in_len, data_out, data_out_len);

			যদি(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}অন্যথা{
				ঘূৰাই rc;
			}
		}
	}অন্যথা{
		যদি(db.config->per_listener_settings){
			ঘূৰাই MOSQ_ERR_AUTH;
		}
	}

	ঘূৰাই MOSQ_ERR_NOT_SUPPORTED;
}
