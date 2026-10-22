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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/broker.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/broker_plugin.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "lib_load.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utlist.h"


ਸਥਿਰ ਪੂਰਨ plugin__basic_auth(ਬਣਤਰ mosquitto__security_options *opts, ਬਣਤਰ mosquitto *context)
{
	ਬਣਤਰ mosquitto_evt_basic_auth event_ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto__callback *cb_ਅਧਾਰ, *cb_ਅਗਲਾ;
	ਪੂਰਨ rc;
	ਪੂਰਨ rc_final = MOSQ_ERR_PLUGIN_IGNORE;

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(opts->plugin_callbacks.basic_auth, cb_ਅਧਾਰ, cb_ਅਗਲਾ){
		memset(&event_ਅੰਕੜੇ, 0, ਆਕਾਰ(event_ਅੰਕੜੇ));
		event_ਅੰਕੜੇ.client = context;
		event_ਅੰਕੜੇ.username = context->username;
		event_ਅੰਕੜੇ.password = context->password;
		event_ਅੰਕੜੇ.password_len = context->password_len;
		rc = cb_ਅਧਾਰ->cb(MOSQ_EVT_BASIC_AUTH, &event_ਅੰਕੜੇ, cb_ਅਧਾਰ->userdata);
		ਜੇ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}ਨਹੀਂਤਾਂ{
			ਵਾਪਸ rc;
		}
	}
	ਵਾਪਸ rc_final;
}


ਪੂਰਨ mosquitto_basic_auth(ਬਣਤਰ mosquitto *context)
{
	ਪੂਰਨ rc;
	bool plugin_ਵਰਤਿਆ = false;

	/* Global plugins */
	ਜੇ(db.config->security_options.plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(&db.config->security_options, context);

		ਜੇ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_ਵਰਤਿਆ = true;
		}ਨਹੀਂਤਾਂ{
			ਵਾਪਸ rc;
		}
	}

	ਜੇ(context->listener && context->listener->security_options->plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(context->listener->security_options, context);

		ਜੇ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_ਵਰਤਿਆ = true;
		}ਨਹੀਂਤਾਂ{
			ਵਾਪਸ rc;
		}
	}

	/* If all plugins deferred, this is a denial. plugin_used == false
	 * here, then no plugins were configured.
	 * anonymous logins are allowed. */
	ਜੇ(plugin_ਵਰਤਿਆ == false){
		ਜੇ((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->ਪ੍ਰਤੀ_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){

			ਵਾਪਸ MOSQ_ERR_SUCCESS;
		}ਨਹੀਂਤਾਂ{
			ਵਾਪਸ MOSQ_ERR_AUTH;
		}
	}ਨਹੀਂਤਾਂ{
		/* Can't have got here without at least one plugin returning MOSQ_ERR_PLUGIN_DEFER.
		 * This will now be a denial, unless it is anon and allow anon is true. */
		ਜੇ(context->username == ਨਿਰਾ &&
				((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->ਪ੍ਰਤੀ_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false))){

			ਵਾਪਸ MOSQ_ERR_SUCCESS;
		}ਨਹੀਂਤਾਂ{
			ਵਾਪਸ MOSQ_ERR_AUTH;
		}
	}

	ਵਾਪਸ rc;
}
