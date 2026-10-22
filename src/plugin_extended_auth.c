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


ساكن صحيح plugin_ext_auth_شروع(بنية mosquitto__security_options *opts, بنية mosquitto *context, bool reauth, ثابت فراغ *داده_درون, uint16_t داده_درون_len, فراغ **داده_بیرون, uint16_t *داده_بیرون_len)
{
	بنية mosquitto_evt_extended_auth event_داده;
	بنية mosquitto__callback *cb_پایه, *cb_بعدی;
	صحيح rc;
	صحيح rc_final = MOSQ_ERR_PLUGIN_DEFER;

	استفادهنشده(reauth);

	DL_FOREACH_امن(opts->plugin_callbacks.ext_auth_شروع, cb_پایه, cb_بعدی){
		memset(&event_داده, 0, حجم(event_داده));
		event_داده.client = context;
		event_داده.auth_روش = context->auth_روش;
		event_داده.داده_درون = داده_درون;
		event_داده.داده_بیرون = عدم;
		event_داده.داده_درون_len = داده_درون_len;
		event_داده.داده_بیرون_len = 0;
		rc = cb_پایه->cb(MOSQ_EVT_EXT_AUTH_شروع, &event_داده, cb_پایه->userdata);
		إذا(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}وإلا إذا(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}وإلا{
			*داده_بیرون = event_داده.داده_بیرون;
			*داده_بیرون_len = event_داده.داده_بیرون_len;
			أرجع rc;
		}
	}
	أرجع rc_final;
}


صحيح mosquitto_security_auth_شروع(بنية mosquitto *context, bool reauth, ثابت فراغ *داده_درون, uint16_t داده_درون_len, فراغ **داده_بیرون, uint16_t *داده_بیرون_len)
{
	صحيح rc;

	إذا(!context || !context->listener || !context->auth_روش){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(!داده_بیرون || !داده_بیرون_len){
		أرجع MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	إذا(db.config->security_options.plugin_callbacks.ext_auth_شروع){
		rc = plugin_ext_auth_شروع(&db.config->security_options, context,
				reauth, داده_درون, داده_درون_len, داده_بیرون, داده_بیرون_len);

		إذا(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}وإلا{
			أرجع rc;
		}
	}

	/* Per listener plugins */
	إذا(context->listener){
		إذا(context->listener->security_options->plugin_callbacks.ext_auth_شروع){
			rc = plugin_ext_auth_شروع(context->listener->security_options, context,
					reauth, داده_درون, داده_درون_len, داده_بیرون, داده_بیرون_len);

			إذا(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}وإلا{
				أرجع rc;
			}
		}
	}وإلا{
		إذا(db.config->در_هر_listener_settings){
			أرجع MOSQ_ERR_AUTH;
		}
	}

	أرجع MOSQ_ERR_NOT_SUPPORTED;
}


ساكن صحيح plugin__ext_auth_continue(بنية mosquitto__security_options *opts, بنية mosquitto *context, ثابت فراغ *داده_درون, uint16_t داده_درون_len, فراغ **داده_بیرون, uint16_t *داده_بیرون_len)
{
	صحيح rc;
	بنية mosquitto_evt_extended_auth event_داده;
	بنية mosquitto__callback *cb_پایه, *cb_بعدی;

	DL_FOREACH_امن(opts->plugin_callbacks.ext_auth_continue, cb_پایه, cb_بعدی){
		memset(&event_داده, 0, حجم(event_داده));
		event_داده.client = context;
		event_داده.auth_روش = context->auth_روش;
		event_داده.داده_درون = داده_درون;
		event_داده.داده_بیرون = عدم;
		event_داده.داده_درون_len = داده_درون_len;
		event_داده.داده_بیرون_len = 0;
		rc = cb_پایه->cb(MOSQ_EVT_EXT_AUTH_CONTINUE, &event_داده, cb_پایه->userdata);
		إذا(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}وإلا{
			*داده_بیرون = event_داده.داده_بیرون;
			*داده_بیرون_len = event_داده.داده_بیرون_len;
			أرجع rc;
		}
	}

	أرجع MOSQ_ERR_PLUGIN_DEFER;
}


صحيح mosquitto_security_auth_continue(بنية mosquitto *context, ثابت فراغ *داده_درون, uint16_t داده_درون_len, فراغ **داده_بیرون, uint16_t *داده_بیرون_len)
{
	صحيح rc;

	إذا(!context || !context->listener || !context->auth_روش){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(!داده_بیرون || !داده_بیرون_len){
		أرجع MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	إذا(db.config->security_options.plugin_callbacks.ext_auth_continue){
		rc = plugin__ext_auth_continue(&db.config->security_options, context,
				داده_درون, داده_درون_len, داده_بیرون, داده_بیرون_len);

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
					داده_درون, داده_درون_len, داده_بیرون, داده_بیرون_len);

			إذا(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}وإلا{
				أرجع rc;
			}
		}
	}وإلا{
		إذا(db.config->در_هر_listener_settings){
			أرجع MOSQ_ERR_AUTH;
		}
	}

	أرجع MOSQ_ERR_NOT_SUPPORTED;
}
