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


ساکن صحیح plugin_psk_کلید_get(ساخت mosquitto__security_options *opts, ساخت mosquitto *context, ثابت حرف *hint, ثابت حرف *شناختی_عنصر, حرف *کلید, صحیح max_کلید_len)
{
	ساخت mosquitto_evt_psk_کلید event_ڈیٹا;
	ساخت mosquitto__callback *cb_بنیاد, *cb_اگلا;
	صحیح rc;
	صحیح rc_final = MOSQ_ERR_SUCCESS;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.psk_کلید, cb_بنیاد, cb_اگلا){
		memset(&event_ڈیٹا, 0, حجم(event_ڈیٹا));
		event_ڈیٹا.client = context;
		event_ڈیٹا.hint = hint;
		event_ڈیٹا.شناختی_عنصر = شناختی_عنصر;
		event_ڈیٹا.کلید = کلید;
		event_ڈیٹا.max_کلید_len = max_کلید_len;
		rc = cb_بنیاد->cb(MOSQ_EVT_PSK_کلید, &event_ڈیٹا, cb_بنیاد->userdata);
		اگر(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}ورنہ اگر(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}ورنہ{
			واپس rc;
		}
	}
	واپس rc_final;
}


صحیح mosquitto_psk_کلید_get(ساخت mosquitto *context, ثابت حرف *hint, ثابت حرف *شناختی_عنصر, حرف *کلید, صحیح max_کلید_len)
{
	صحیح rc;
	صحیح rc_final = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	اگر(db.config->security_options.plugin_callbacks.psk_کلید){
		rc = plugin_psk_کلید_get(&db.config->security_options, context,
				hint, شناختی_عنصر, کلید, max_کلید_len);

		اگر(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}ورنہ اگر(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}ورنہ{
			واپس rc;
		}
	}

	/* Per listener plugins */
	اگر(context->listener){
		اگر(context->listener->security_options->plugin_callbacks.psk_کلید){
			rc = plugin_psk_کلید_get(context->listener->security_options, context,
					hint, شناختی_عنصر, کلید, max_کلید_len);

			اگر(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing */
			}ورنہ اگر(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}ورنہ{
				واپس rc;
			}
		}
	}ورنہ{
		اگر(db.config->فی_listener_settings){
			واپس MOSQ_ERR_AUTH;
		}
	}

	rc = mosquitto_psk_کلید_get_default(context, hint, شناختی_عنصر, کلید, max_کلید_len);
	اگر(rc == MOSQ_ERR_PLUGIN_IGNORE){
		/* Do nothing */
	}ورنہ اگر(rc == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_PLUGIN_DEFER;
	}ورنہ{
		واپس rc;
	}


	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured. */
	اگر(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_AUTH;
	}
	واپس rc_final;
}
