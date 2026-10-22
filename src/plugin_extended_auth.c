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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto/broker.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/broker_plugin.h"
#ଅନ୍ତର୍ଭାଵ "lib_load.h"
#ଅନ୍ତର୍ଭାଵ "utlist.h"


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin__ext_auth_start(ସଂରଚନା mosquitto__security_options *opts, ସଂରଚନା mosquitto *context, bool reauth, ଅଚଳ ଶୂନ୍ୟ *data_in, uint16_t data_in_len, ଶୂନ୍ୟ **data_out, uint16_t *data_out_len)
{
	ସଂରଚନା mosquitto_evt_extended_auth event_data;
	ସଂରଚନା mosquitto__callback *cb_base, *cb_next;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc_final = MOSQ_ERR_PLUGIN_DEFER;

	UNUSED(reauth);

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_start, cb_base, cb_next){
		memset(&event_data, 0, ଆକାର(event_data));
		event_data.client = context;
		event_data.auth_method = context->auth_method;
		event_data.data_in = data_in;
		event_data.data_out = ଖାଲି;
		event_data.data_in_len = data_in_len;
		event_data.data_out_len = 0;
		rc = cb_base->cb(MOSQ_EVT_EXT_AUTH_START, &event_data, cb_base->userdata);
		ଯଦି(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}ଅନ୍ୟଥା ଯଦି(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}ଅନ୍ୟଥା{
			*data_out = event_data.data_out;
			*data_out_len = event_data.data_out_len;
			ଫେରନ୍ତୁ rc;
		}
	}
	ଫେରନ୍ତୁ rc_final;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_security_auth_start(ସଂରଚନା mosquitto *context, bool reauth, ଅଚଳ ଶୂନ୍ୟ *data_in, uint16_t data_in_len, ଶୂନ୍ୟ **data_out, uint16_t *data_out_len)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି(!context || !context->listener || !context->auth_method){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}
	ଯଦି(!data_out || !data_out_len){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	ଯଦି(db.config->security_options.plugin_callbacks.ext_auth_start){
		rc = plugin__ext_auth_start(&db.config->security_options, context,
				reauth, data_in, data_in_len, data_out, data_out_len);

		ଯଦି(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}ଅନ୍ୟଥା{
			ଫେରନ୍ତୁ rc;
		}
	}

	/* Per listener plugins */
	ଯଦି(context->listener){
		ଯଦି(context->listener->security_options->plugin_callbacks.ext_auth_start){
			rc = plugin__ext_auth_start(context->listener->security_options, context,
					reauth, data_in, data_in_len, data_out, data_out_len);

			ଯଦି(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}ଅନ୍ୟଥା{
				ଫେରନ୍ତୁ rc;
			}
		}
	}ଅନ୍ୟଥା{
		ଯଦି(db.config->per_listener_settings){
			ଫେରନ୍ତୁ MOSQ_ERR_AUTH;
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_NOT_SUPPORTED;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin__ext_auth_continue(ସଂରଚନା mosquitto__security_options *opts, ସଂରଚନା mosquitto *context, ଅଚଳ ଶୂନ୍ୟ *data_in, uint16_t data_in_len, ଶୂନ୍ୟ **data_out, uint16_t *data_out_len)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା mosquitto_evt_extended_auth event_data;
	ସଂରଚନା mosquitto__callback *cb_base, *cb_next;

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_continue, cb_base, cb_next){
		memset(&event_data, 0, ଆକାର(event_data));
		event_data.client = context;
		event_data.auth_method = context->auth_method;
		event_data.data_in = data_in;
		event_data.data_out = ଖାଲି;
		event_data.data_in_len = data_in_len;
		event_data.data_out_len = 0;
		rc = cb_base->cb(MOSQ_EVT_EXT_AUTH_CONTINUE, &event_data, cb_base->userdata);
		ଯଦି(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}ଅନ୍ୟଥା{
			*data_out = event_data.data_out;
			*data_out_len = event_data.data_out_len;
			ଫେରନ୍ତୁ rc;
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_PLUGIN_DEFER;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_security_auth_continue(ସଂରଚନା mosquitto *context, ଅଚଳ ଶୂନ୍ୟ *data_in, uint16_t data_in_len, ଶୂନ୍ୟ **data_out, uint16_t *data_out_len)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି(!context || !context->listener || !context->auth_method){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}
	ଯଦି(!data_out || !data_out_len){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	ଯଦି(db.config->security_options.plugin_callbacks.ext_auth_continue){
		rc = plugin__ext_auth_continue(&db.config->security_options, context,
				data_in, data_in_len, data_out, data_out_len);

		ଯଦି(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}ଅନ୍ୟଥା{
			ଫେରନ୍ତୁ rc;
		}
	}

	/* Per listener plugins */
	ଯଦି(context->listener){
		ଯଦି(context->listener->security_options->plugin_callbacks.ext_auth_continue){
			rc = plugin__ext_auth_continue(context->listener->security_options, context,
					data_in, data_in_len, data_out, data_out_len);

			ଯଦି(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}ଅନ୍ୟଥା{
				ଫେରନ୍ତୁ rc;
			}
		}
	}ଅନ୍ୟଥା{
		ଯଦି(db.config->per_listener_settings){
			ଫେରନ୍ତୁ MOSQ_ERR_AUTH;
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_NOT_SUPPORTED;
}
