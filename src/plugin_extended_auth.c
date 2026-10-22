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


ساكن صحيح plugin_ext_auth_پیل(بنية mosquitto__security_options *opts, بنية mosquitto *context, bool reauth, ثابت فراغ *معلومات_دننه, uint16_t معلومات_دننه_len, فراغ **معلومات_بهر, uint16_t *معلومات_بهر_len)
{
	بنية mosquitto_evt_extended_auth event_معلومات;
	بنية mosquitto__callback *cb_بنسټ, *cb_بل;
	صحيح rc;
	صحيح rc_final = MOSQ_ERR_PLUGIN_DEFER;

	ناکارول_شوی(reauth);

	DL_FOREACH_خوندي(opts->plugin_callbacks.ext_auth_پیل, cb_بنسټ, cb_بل){
		memset(&event_معلومات, 0, حجم(event_معلومات));
		event_معلومات.client = context;
		event_معلومات.auth_لاره = context->auth_لاره;
		event_معلومات.معلومات_دننه = معلومات_دننه;
		event_معلومات.معلومات_بهر = عدم;
		event_معلومات.معلومات_دننه_len = معلومات_دننه_len;
		event_معلومات.معلومات_بهر_len = 0;
		rc = cb_بنسټ->cb(MOSQ_EVT_EXT_AUTH_پیل, &event_معلومات, cb_بنسټ->userdata);
		إذا(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}وإلا إذا(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}وإلا{
			*معلومات_بهر = event_معلومات.معلومات_بهر;
			*معلومات_بهر_len = event_معلومات.معلومات_بهر_len;
			أرجع rc;
		}
	}
	أرجع rc_final;
}


صحيح mosquitto_security_auth_پیل(بنية mosquitto *context, bool reauth, ثابت فراغ *معلومات_دننه, uint16_t معلومات_دننه_len, فراغ **معلومات_بهر, uint16_t *معلومات_بهر_len)
{
	صحيح rc;

	إذا(!context || !context->listener || !context->auth_لاره){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(!معلومات_بهر || !معلومات_بهر_len){
		أرجع MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	إذا(db.config->security_options.plugin_callbacks.ext_auth_پیل){
		rc = plugin_ext_auth_پیل(&db.config->security_options, context,
				reauth, معلومات_دننه, معلومات_دننه_len, معلومات_بهر, معلومات_بهر_len);

		إذا(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}وإلا{
			أرجع rc;
		}
	}

	/* Per listener plugins */
	إذا(context->listener){
		إذا(context->listener->security_options->plugin_callbacks.ext_auth_پیل){
			rc = plugin_ext_auth_پیل(context->listener->security_options, context,
					reauth, معلومات_دننه, معلومات_دننه_len, معلومات_بهر, معلومات_بهر_len);

			إذا(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}وإلا{
				أرجع rc;
			}
		}
	}وإلا{
		إذا(db.config->فی_listener_settings){
			أرجع MOSQ_ERR_AUTH;
		}
	}

	أرجع MOSQ_ERR_NOT_SUPPORTED;
}


ساكن صحيح plugin__ext_auth_continue(بنية mosquitto__security_options *opts, بنية mosquitto *context, ثابت فراغ *معلومات_دننه, uint16_t معلومات_دننه_len, فراغ **معلومات_بهر, uint16_t *معلومات_بهر_len)
{
	صحيح rc;
	بنية mosquitto_evt_extended_auth event_معلومات;
	بنية mosquitto__callback *cb_بنسټ, *cb_بل;

	DL_FOREACH_خوندي(opts->plugin_callbacks.ext_auth_continue, cb_بنسټ, cb_بل){
		memset(&event_معلومات, 0, حجم(event_معلومات));
		event_معلومات.client = context;
		event_معلومات.auth_لاره = context->auth_لاره;
		event_معلومات.معلومات_دننه = معلومات_دننه;
		event_معلومات.معلومات_بهر = عدم;
		event_معلومات.معلومات_دننه_len = معلومات_دننه_len;
		event_معلومات.معلومات_بهر_len = 0;
		rc = cb_بنسټ->cb(MOSQ_EVT_EXT_AUTH_CONTINUE, &event_معلومات, cb_بنسټ->userdata);
		إذا(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}وإلا{
			*معلومات_بهر = event_معلومات.معلومات_بهر;
			*معلومات_بهر_len = event_معلومات.معلومات_بهر_len;
			أرجع rc;
		}
	}

	أرجع MOSQ_ERR_PLUGIN_DEFER;
}


صحيح mosquitto_security_auth_continue(بنية mosquitto *context, ثابت فراغ *معلومات_دننه, uint16_t معلومات_دننه_len, فراغ **معلومات_بهر, uint16_t *معلومات_بهر_len)
{
	صحيح rc;

	إذا(!context || !context->listener || !context->auth_لاره){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(!معلومات_بهر || !معلومات_بهر_len){
		أرجع MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	إذا(db.config->security_options.plugin_callbacks.ext_auth_continue){
		rc = plugin__ext_auth_continue(&db.config->security_options, context,
				معلومات_دننه, معلومات_دننه_len, معلومات_بهر, معلومات_بهر_len);

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
					معلومات_دننه, معلومات_دننه_len, معلومات_بهر, معلومات_بهر_len);

			إذا(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}وإلا{
				أرجع rc;
			}
		}
	}وإلا{
		إذا(db.config->فی_listener_settings){
			أرجع MOSQ_ERR_AUTH;
		}
	}

	أرجع MOSQ_ERR_NOT_SUPPORTED;
}
