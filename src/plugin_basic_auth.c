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


ساکن صحیح plugin__basic_auth(ساخت mosquitto__security_options *opts, ساخت mosquitto *context)
{
	ساخت mosquitto_evt_basic_auth event_ڈیٹا;
	ساخت mosquitto__callback *cb_بنیاد, *cb_اگلا;
	صحیح rc;
	صحیح rc_final = MOSQ_ERR_PLUGIN_IGNORE;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.basic_auth, cb_بنیاد, cb_اگلا){
		memset(&event_ڈیٹا, 0, حجم(event_ڈیٹا));
		event_ڈیٹا.client = context;
		event_ڈیٹا.username = context->username;
		event_ڈیٹا.password = context->password;
		event_ڈیٹا.password_len = context->password_len;
		rc = cb_بنیاد->cb(MOSQ_EVT_BASIC_AUTH, &event_ڈیٹا, cb_بنیاد->userdata);
		اگر(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}ورنہ اگر(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}ورنہ{
			واپس rc;
		}
	}
	واپس rc_final;
}


صحیح mosquitto_basic_auth(ساخت mosquitto *context)
{
	صحیح rc;
	bool plugin_مستعمل = false;

	/* Global plugins */
	اگر(db.config->security_options.plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(&db.config->security_options, context);

		اگر(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}ورنہ اگر(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_مستعمل = true;
		}ورنہ{
			واپس rc;
		}
	}

	اگر(context->listener && context->listener->security_options->plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(context->listener->security_options, context);

		اگر(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}ورنہ اگر(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_مستعمل = true;
		}ورنہ{
			واپس rc;
		}
	}

	/* If all plugins deferred, this is a denial. plugin_used == false
	 * here, then no plugins were configured.
	 * anonymous logins are allowed. */
	اگر(plugin_مستعمل == false){
		اگر((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->فی_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){

			واپس MOSQ_ERR_SUCCESS;
		}ورنہ{
			واپس MOSQ_ERR_AUTH;
		}
	}ورنہ{
		/* Can't have got here without at least one plugin returning MOSQ_ERR_PLUGIN_DEFER.
		 * This will now be a denial, unless it is anon and allow anon is true. */
		اگر(context->username == تہی &&
				((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->فی_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false))){

			واپس MOSQ_ERR_SUCCESS;
		}ورنہ{
			واپس MOSQ_ERR_AUTH;
		}
	}

	واپس rc;
}
