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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "mosquitto/broker.h"
#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto/broker_plugin.h"
#অন্তর্ভাব "lib_load.h"
#অন্তর্ভাব "utlist.h"


স্থির পূর্ণসংখ্যা plugin__ext_auth_start(কাঠামো mosquitto__security_options *opts, কাঠামো mosquitto *context, bool reauth, ধ্রুবক শূন্য *তথ্য_in, uint16_t তথ্য_in_len, শূন্য **তথ্য_out, uint16_t *তথ্য_out_len)
{
	কাঠামো mosquitto_evt_extended_auth event_তথ্য;
	কাঠামো mosquitto__callback *cb_base, *cb_পরবর্তী;
	পূর্ণসংখ্যা rc;
	পূর্ণসংখ্যা rc_final = MOSQ_ERR_PLUGIN_DEFER;

	UNUSED(reauth);

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_start, cb_base, cb_পরবর্তী){
		memset(&event_তথ্য, 0, আকার(event_তথ্য));
		event_তথ্য.client = context;
		event_তথ্য.auth_method = context->auth_method;
		event_তথ্য.তথ্য_in = তথ্য_in;
		event_তথ্য.তথ্য_out = ফাঁকা;
		event_তথ্য.তথ্য_in_len = তথ্য_in_len;
		event_তথ্য.তথ্য_out_len = 0;
		rc = cb_base->cb(MOSQ_EVT_EXT_AUTH_START, &event_তথ্য, cb_base->userdata);
		যদি(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}অন্যথা যদি(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}অন্যথা{
			*তথ্য_out = event_তথ্য.তথ্য_out;
			*তথ্য_out_len = event_তথ্য.তথ্য_out_len;
			ফেরত rc;
		}
	}
	ফেরত rc_final;
}


পূর্ণসংখ্যা mosquitto_security_auth_start(কাঠামো mosquitto *context, bool reauth, ধ্রুবক শূন্য *তথ্য_in, uint16_t তথ্য_in_len, শূন্য **তথ্য_out, uint16_t *তথ্য_out_len)
{
	পূর্ণসংখ্যা rc;

	যদি(!context || !context->listener || !context->auth_method){
		ফেরত MOSQ_ERR_INVAL;
	}
	যদি(!তথ্য_out || !তথ্য_out_len){
		ফেরত MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	যদি(db.config->security_options.plugin_callbacks.ext_auth_start){
		rc = plugin__ext_auth_start(&db.config->security_options, context,
				reauth, তথ্য_in, তথ্য_in_len, তথ্য_out, তথ্য_out_len);

		যদি(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}অন্যথা{
			ফেরত rc;
		}
	}

	/* Per listener plugins */
	যদি(context->listener){
		যদি(context->listener->security_options->plugin_callbacks.ext_auth_start){
			rc = plugin__ext_auth_start(context->listener->security_options, context,
					reauth, তথ্য_in, তথ্য_in_len, তথ্য_out, তথ্য_out_len);

			যদি(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}অন্যথা{
				ফেরত rc;
			}
		}
	}অন্যথা{
		যদি(db.config->per_listener_settings){
			ফেরত MOSQ_ERR_AUTH;
		}
	}

	ফেরত MOSQ_ERR_NOT_SUPPORTED;
}


স্থির পূর্ণসংখ্যা plugin__ext_auth_continue(কাঠামো mosquitto__security_options *opts, কাঠামো mosquitto *context, ধ্রুবক শূন্য *তথ্য_in, uint16_t তথ্য_in_len, শূন্য **তথ্য_out, uint16_t *তথ্য_out_len)
{
	পূর্ণসংখ্যা rc;
	কাঠামো mosquitto_evt_extended_auth event_তথ্য;
	কাঠামো mosquitto__callback *cb_base, *cb_পরবর্তী;

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_continue, cb_base, cb_পরবর্তী){
		memset(&event_তথ্য, 0, আকার(event_তথ্য));
		event_তথ্য.client = context;
		event_তথ্য.auth_method = context->auth_method;
		event_তথ্য.তথ্য_in = তথ্য_in;
		event_তথ্য.তথ্য_out = ফাঁকা;
		event_তথ্য.তথ্য_in_len = তথ্য_in_len;
		event_তথ্য.তথ্য_out_len = 0;
		rc = cb_base->cb(MOSQ_EVT_EXT_AUTH_CONTINUE, &event_তথ্য, cb_base->userdata);
		যদি(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}অন্যথা{
			*তথ্য_out = event_তথ্য.তথ্য_out;
			*তথ্য_out_len = event_তথ্য.তথ্য_out_len;
			ফেরত rc;
		}
	}

	ফেরত MOSQ_ERR_PLUGIN_DEFER;
}


পূর্ণসংখ্যা mosquitto_security_auth_continue(কাঠামো mosquitto *context, ধ্রুবক শূন্য *তথ্য_in, uint16_t তথ্য_in_len, শূন্য **তথ্য_out, uint16_t *তথ্য_out_len)
{
	পূর্ণসংখ্যা rc;

	যদি(!context || !context->listener || !context->auth_method){
		ফেরত MOSQ_ERR_INVAL;
	}
	যদি(!তথ্য_out || !তথ্য_out_len){
		ফেরত MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	যদি(db.config->security_options.plugin_callbacks.ext_auth_continue){
		rc = plugin__ext_auth_continue(&db.config->security_options, context,
				তথ্য_in, তথ্য_in_len, তথ্য_out, তথ্য_out_len);

		যদি(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}অন্যথা{
			ফেরত rc;
		}
	}

	/* Per listener plugins */
	যদি(context->listener){
		যদি(context->listener->security_options->plugin_callbacks.ext_auth_continue){
			rc = plugin__ext_auth_continue(context->listener->security_options, context,
					তথ্য_in, তথ্য_in_len, তথ্য_out, তথ্য_out_len);

			যদি(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}অন্যথা{
				ফেরত rc;
			}
		}
	}অন্যথা{
		যদি(db.config->per_listener_settings){
			ফেরত MOSQ_ERR_AUTH;
		}
	}

	ফেরত MOSQ_ERR_NOT_SUPPORTED;
}
