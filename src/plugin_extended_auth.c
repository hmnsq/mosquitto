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

#ضمن "config.h"

#ضمن <stdio.h>
#ضمن <string.h>

#ضمن "mosquitto/broker.h"
#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/broker_plugin.h"
#ضمن "lib_load.h"
#ضمن "utlist.h"


ساكن صحيح plugin_ext_auth_شروعات(بنية mosquitto__security_options *opts, بنية mosquitto *context, bool reauth, ثابت فراغ *معلومات_اندريون, uint16_t معلومات_اندريون_len, فراغ **معلومات_ٻاهريون, uint16_t *معلومات_ٻاهريون_len)
{
	بنية mosquitto_evt_extended_auth event_معلومات;
	بنية mosquitto__callback *cb_بنياد, *cb_ايندڙ;
	صحيح rc;
	صحيح rc_final = MOSQ_ERR_PLUGIN_DEFER;

	اڻ_استعمال_ٿيل(reauth);

	DL_FOREACH_محفوظ(opts->plugin_callbacks.ext_auth_شروعات, cb_بنياد, cb_ايندڙ){
		memset(&event_معلومات, 0, حجم(event_معلومات));
		event_معلومات.client = context;
		event_معلومات.auth_طريقو = context->auth_طريقو;
		event_معلومات.معلومات_اندريون = معلومات_اندريون;
		event_معلومات.معلومات_ٻاهريون = عدم;
		event_معلومات.معلومات_اندريون_len = معلومات_اندريون_len;
		event_معلومات.معلومات_ٻاهريون_len = 0;
		rc = cb_بنياد->cb(MOSQ_EVT_EXT_AUTH_شروعات, &event_معلومات, cb_بنياد->userdata);
		إذا(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}وإلا إذا(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}وإلا{
			*معلومات_ٻاهريون = event_معلومات.معلومات_ٻاهريون;
			*معلومات_ٻاهريون_len = event_معلومات.معلومات_ٻاهريون_len;
			أرجع rc;
		}
	}
	أرجع rc_final;
}


صحيح mosquitto_security_auth_شروعات(بنية mosquitto *context, bool reauth, ثابت فراغ *معلومات_اندريون, uint16_t معلومات_اندريون_len, فراغ **معلومات_ٻاهريون, uint16_t *معلومات_ٻاهريون_len)
{
	صحيح rc;

	إذا(!context || !context->listener || !context->auth_طريقو){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(!معلومات_ٻاهريون || !معلومات_ٻاهريون_len){
		أرجع MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	إذا(db.config->security_options.plugin_callbacks.ext_auth_شروعات){
		rc = plugin_ext_auth_شروعات(&db.config->security_options, context,
				reauth, معلومات_اندريون, معلومات_اندريون_len, معلومات_ٻاهريون, معلومات_ٻاهريون_len);

		إذا(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}وإلا{
			أرجع rc;
		}
	}

	/* Per listener plugins */
	إذا(context->listener){
		إذا(context->listener->security_options->plugin_callbacks.ext_auth_شروعات){
			rc = plugin_ext_auth_شروعات(context->listener->security_options, context,
					reauth, معلومات_اندريون, معلومات_اندريون_len, معلومات_ٻاهريون, معلومات_ٻاهريون_len);

			إذا(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}وإلا{
				أرجع rc;
			}
		}
	}وإلا{
		إذا(db.config->في_listener_settings){
			أرجع MOSQ_ERR_AUTH;
		}
	}

	أرجع MOSQ_ERR_NOT_SUPPORTED;
}


ساكن صحيح plugin__ext_auth_continue(بنية mosquitto__security_options *opts, بنية mosquitto *context, ثابت فراغ *معلومات_اندريون, uint16_t معلومات_اندريون_len, فراغ **معلومات_ٻاهريون, uint16_t *معلومات_ٻاهريون_len)
{
	صحيح rc;
	بنية mosquitto_evt_extended_auth event_معلومات;
	بنية mosquitto__callback *cb_بنياد, *cb_ايندڙ;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.ext_auth_continue, cb_بنياد, cb_ايندڙ){
		memset(&event_معلومات, 0, حجم(event_معلومات));
		event_معلومات.client = context;
		event_معلومات.auth_طريقو = context->auth_طريقو;
		event_معلومات.معلومات_اندريون = معلومات_اندريون;
		event_معلومات.معلومات_ٻاهريون = عدم;
		event_معلومات.معلومات_اندريون_len = معلومات_اندريون_len;
		event_معلومات.معلومات_ٻاهريون_len = 0;
		rc = cb_بنياد->cb(MOSQ_EVT_EXT_AUTH_CONTINUE, &event_معلومات, cb_بنياد->userdata);
		إذا(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}وإلا{
			*معلومات_ٻاهريون = event_معلومات.معلومات_ٻاهريون;
			*معلومات_ٻاهريون_len = event_معلومات.معلومات_ٻاهريون_len;
			أرجع rc;
		}
	}

	أرجع MOSQ_ERR_PLUGIN_DEFER;
}


صحيح mosquitto_security_auth_continue(بنية mosquitto *context, ثابت فراغ *معلومات_اندريون, uint16_t معلومات_اندريون_len, فراغ **معلومات_ٻاهريون, uint16_t *معلومات_ٻاهريون_len)
{
	صحيح rc;

	إذا(!context || !context->listener || !context->auth_طريقو){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(!معلومات_ٻاهريون || !معلومات_ٻاهريون_len){
		أرجع MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	إذا(db.config->security_options.plugin_callbacks.ext_auth_continue){
		rc = plugin__ext_auth_continue(&db.config->security_options, context,
				معلومات_اندريون, معلومات_اندريون_len, معلومات_ٻاهريون, معلومات_ٻاهريون_len);

		إذا(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}وإلا{
			أرجع rc;
		}
	}

	/* Per listener plugins */
	إذا(context->listener){
		إذا(context->listener->security_options->plugin_callbacks.ext_auth_continue){
			rc = plugin__ext_auth_continue(context->listener->security_options, context,
					معلومات_اندريون, معلومات_اندريون_len, معلومات_ٻاهريون, معلومات_ٻاهريون_len);

			إذا(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}وإلا{
				أرجع rc;
			}
		}
	}وإلا{
		إذا(db.config->في_listener_settings){
			أرجع MOSQ_ERR_AUTH;
		}
	}

	أرجع MOSQ_ERR_NOT_SUPPORTED;
}
