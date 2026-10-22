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


ایستا صحیح plugin__basic_auth(ساختار mosquitto__security_options *opts, ساختار mosquitto *context)
{
	ساختار mosquitto_evt_basic_auth event_data;
	ساختار mosquitto__callback *cb_base, *cb_next;
	صحیح rc;
	صحیح rc_final = MOSQ_ERR_PLUGIN_IGNORE;

	DL_FOREACH_SAFE(opts->plugin_callbacks.basic_auth, cb_base, cb_next){
		memset(&event_data, 0, اندازه(event_data));
		event_data.client = context;
		event_data.username = context->username;
		event_data.password = context->password;
		event_data.password_len = context->password_len;
		rc = cb_base->cb(MOSQ_EVT_BASIC_AUTH, &event_data, cb_base->userdata);
		اگر(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}وگرنه اگر(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}وگرنه{
			بازگشت rc;
		}
	}
	بازگشت rc_final;
}


صحیح mosquitto_basic_auth(ساختار mosquitto *context)
{
	صحیح rc;
	bool plugin_used = false;

	/* Global plugins */
	اگر(db.config->security_options.plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(&db.config->security_options, context);

		اگر(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}وگرنه اگر(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_used = true;
		}وگرنه{
			بازگشت rc;
		}
	}

	اگر(context->listener && context->listener->security_options->plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(context->listener->security_options, context);

		اگر(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}وگرنه اگر(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_used = true;
		}وگرنه{
			بازگشت rc;
		}
	}

	/* If all plugins deferred, this is a denial. plugin_used == false
	 * here, then no plugins were configured.
	 * anonymous logins are allowed. */
	اگر(plugin_used == false){
		اگر((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){

			بازگشت MOSQ_ERR_SUCCESS;
		}وگرنه{
			بازگشت MOSQ_ERR_AUTH;
		}
	}وگرنه{
		/* Can't have got here without at least one plugin returning MOSQ_ERR_PLUGIN_DEFER.
		 * This will now be a denial, unless it is anon and allow anon is true. */
		اگر(context->username == پوچ &&
				((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false))){

			بازگشت MOSQ_ERR_SUCCESS;
		}وگرنه{
			بازگشت MOSQ_ERR_AUTH;
		}
	}

	بازگشت rc;
}
