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

#شامل "config.h"

#شامل <stdio.h>
#شامل <string.h>

#شامل "mosquitto/broker.h"
#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/broker_plugin.h"
#شامل "lib_load.h"
#شامل "utlist.h"


ساکن صحیح plugin_ext_auth_آغاز(ساخت mosquitto__security_options *opts, ساخت mosquitto *context, bool reauth, ثابت خالی *ڈیٹا_اندر, uint16_t ڈیٹا_اندر_len, خالی **ڈیٹا_برآمد, uint16_t *ڈیٹا_برآمد_len)
{
	ساخت mosquitto_evt_extended_auth event_ڈیٹا;
	ساخت mosquitto__callback *cb_بنیاد, *cb_اگلا;
	صحیح rc;
	صحیح rc_final = MOSQ_ERR_PLUGIN_DEFER;

	غیر_مستعمل(reauth);

	DL_FOREACH_محفوظ(opts->plugin_callbacks.ext_auth_آغاز, cb_بنیاد, cb_اگلا){
		memset(&event_ڈیٹا, 0, حجم(event_ڈیٹا));
		event_ڈیٹا.client = context;
		event_ڈیٹا.auth_طریقہ = context->auth_طریقہ;
		event_ڈیٹا.ڈیٹا_اندر = ڈیٹا_اندر;
		event_ڈیٹا.ڈیٹا_برآمد = تہی;
		event_ڈیٹا.ڈیٹا_اندر_len = ڈیٹا_اندر_len;
		event_ڈیٹا.ڈیٹا_برآمد_len = 0;
		rc = cb_بنیاد->cb(MOSQ_EVT_EXT_AUTH_آغاز, &event_ڈیٹا, cb_بنیاد->userdata);
		اگر(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}ورنہ اگر(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}ورنہ{
			*ڈیٹا_برآمد = event_ڈیٹا.ڈیٹا_برآمد;
			*ڈیٹا_برآمد_len = event_ڈیٹا.ڈیٹا_برآمد_len;
			واپس rc;
		}
	}
	واپس rc_final;
}


صحیح mosquitto_security_auth_آغاز(ساخت mosquitto *context, bool reauth, ثابت خالی *ڈیٹا_اندر, uint16_t ڈیٹا_اندر_len, خالی **ڈیٹا_برآمد, uint16_t *ڈیٹا_برآمد_len)
{
	صحیح rc;

	اگر(!context || !context->listener || !context->auth_طریقہ){
		واپس MOSQ_ERR_INVAL;
	}
	اگر(!ڈیٹا_برآمد || !ڈیٹا_برآمد_len){
		واپس MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	اگر(db.config->security_options.plugin_callbacks.ext_auth_آغاز){
		rc = plugin_ext_auth_آغاز(&db.config->security_options, context,
				reauth, ڈیٹا_اندر, ڈیٹا_اندر_len, ڈیٹا_برآمد, ڈیٹا_برآمد_len);

		اگر(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}ورنہ{
			واپس rc;
		}
	}

	/* Per listener plugins */
	اگر(context->listener){
		اگر(context->listener->security_options->plugin_callbacks.ext_auth_آغاز){
			rc = plugin_ext_auth_آغاز(context->listener->security_options, context,
					reauth, ڈیٹا_اندر, ڈیٹا_اندر_len, ڈیٹا_برآمد, ڈیٹا_برآمد_len);

			اگر(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}ورنہ{
				واپس rc;
			}
		}
	}ورنہ{
		اگر(db.config->فی_listener_settings){
			واپس MOSQ_ERR_AUTH;
		}
	}

	واپس MOSQ_ERR_NOT_SUPPORTED;
}


ساکن صحیح plugin__ext_auth_continue(ساخت mosquitto__security_options *opts, ساخت mosquitto *context, ثابت خالی *ڈیٹا_اندر, uint16_t ڈیٹا_اندر_len, خالی **ڈیٹا_برآمد, uint16_t *ڈیٹا_برآمد_len)
{
	صحیح rc;
	ساخت mosquitto_evt_extended_auth event_ڈیٹا;
	ساخت mosquitto__callback *cb_بنیاد, *cb_اگلا;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.ext_auth_continue, cb_بنیاد, cb_اگلا){
		memset(&event_ڈیٹا, 0, حجم(event_ڈیٹا));
		event_ڈیٹا.client = context;
		event_ڈیٹا.auth_طریقہ = context->auth_طریقہ;
		event_ڈیٹا.ڈیٹا_اندر = ڈیٹا_اندر;
		event_ڈیٹا.ڈیٹا_برآمد = تہی;
		event_ڈیٹا.ڈیٹا_اندر_len = ڈیٹا_اندر_len;
		event_ڈیٹا.ڈیٹا_برآمد_len = 0;
		rc = cb_بنیاد->cb(MOSQ_EVT_EXT_AUTH_CONTINUE, &event_ڈیٹا, cb_بنیاد->userdata);
		اگر(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}ورنہ{
			*ڈیٹا_برآمد = event_ڈیٹا.ڈیٹا_برآمد;
			*ڈیٹا_برآمد_len = event_ڈیٹا.ڈیٹا_برآمد_len;
			واپس rc;
		}
	}

	واپس MOSQ_ERR_PLUGIN_DEFER;
}


صحیح mosquitto_security_auth_continue(ساخت mosquitto *context, ثابت خالی *ڈیٹا_اندر, uint16_t ڈیٹا_اندر_len, خالی **ڈیٹا_برآمد, uint16_t *ڈیٹا_برآمد_len)
{
	صحیح rc;

	اگر(!context || !context->listener || !context->auth_طریقہ){
		واپس MOSQ_ERR_INVAL;
	}
	اگر(!ڈیٹا_برآمد || !ڈیٹا_برآمد_len){
		واپس MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	اگر(db.config->security_options.plugin_callbacks.ext_auth_continue){
		rc = plugin__ext_auth_continue(&db.config->security_options, context,
				ڈیٹا_اندر, ڈیٹا_اندر_len, ڈیٹا_برآمد, ڈیٹا_برآمد_len);

		اگر(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}ورنہ{
			واپس rc;
		}
	}

	/* Per listener plugins */
	اگر(context->listener){
		اگر(context->listener->security_options->plugin_callbacks.ext_auth_continue){
			rc = plugin__ext_auth_continue(context->listener->security_options, context,
					ڈیٹا_اندر, ڈیٹا_اندر_len, ڈیٹا_برآمد, ڈیٹا_برآمد_len);

			اگر(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}ورنہ{
				واپس rc;
			}
		}
	}ورنہ{
		اگر(db.config->فی_listener_settings){
			واپس MOSQ_ERR_AUTH;
		}
	}

	واپس MOSQ_ERR_NOT_SUPPORTED;
}
