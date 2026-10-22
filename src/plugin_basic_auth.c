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


ساكن صحيح plugin__basic_auth(بنية mosquitto__security_options *opts, بنية mosquitto *context)
{
	بنية mosquitto_evt_basic_auth event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	صحيح rc;
	صحيح rc_final = MOSQ_ERR_PLUGIN_IGNORE;

	DL_FOREACH_SAFE(opts->plugin_callbacks.basic_auth, cb_base, cb_next){
		memset(&event_data, 0, حجم(event_data));
		event_data.client = context;
		event_data.username = context->username;
		event_data.password = context->password;
		event_data.password_len = context->password_len;
		rc = cb_base->cb(MOSQ_EVT_BASIC_AUTH, &event_data, cb_base->userdata);
		إذا(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}وإلا إذا(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}وإلا{
			أرجع rc;
		}
	}
	أرجع rc_final;
}


صحيح mosquitto_basic_auth(بنية mosquitto *context)
{
	صحيح rc;
	bool plugin_used = false;

	/* Global plugins */
	إذا(db.config->security_options.plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(&db.config->security_options, context);

		إذا(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}وإلا إذا(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_used = true;
		}وإلا{
			أرجع rc;
		}
	}

	إذا(context->listener && context->listener->security_options->plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(context->listener->security_options, context);

		إذا(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}وإلا إذا(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_used = true;
		}وإلا{
			أرجع rc;
		}
	}

	/* If all plugins deferred, this is a denial. plugin_used == false
	 * here, then no plugins were configured.
	 * anonymous logins are allowed. */
	إذا(plugin_used == false){
		إذا((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){

			أرجع MOSQ_ERR_SUCCESS;
		}وإلا{
			أرجع MOSQ_ERR_AUTH;
		}
	}وإلا{
		/* Can't have got here without at least one plugin returning MOSQ_ERR_PLUGIN_DEFER.
		 * This will now be a denial, unless it is anon and allow anon is true. */
		إذا(context->username == عدم &&
				((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false))){

			أرجع MOSQ_ERR_SUCCESS;
		}وإلا{
			أرجع MOSQ_ERR_AUTH;
		}
	}

	أرجع rc;
}
