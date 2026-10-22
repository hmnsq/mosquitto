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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "mosquitto/broker.h"
#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/broker_plugin.h"
#അന്തര്ഭാവ "lib_load.h"
#അന്തര്ഭാവ "utlist.h"


സ്ഥിതം പൂർണ്ണസംഖ്യ plugin__basic_auth(ഘടന mosquitto__security_options *opts, ഘടന mosquitto *context)
{
	ഘടന mosquitto_evt_basic_auth event_data;
	ഘടന mosquitto__callback *cb_base, *cb_next;
	പൂർണ്ണസംഖ്യ rc;
	പൂർണ്ണസംഖ്യ rc_final = MOSQ_ERR_PLUGIN_IGNORE;

	DL_FOREACH_SAFE(opts->plugin_callbacks.basic_auth, cb_base, cb_next){
		memset(&event_data, 0, വലുപ്പം(event_data));
		event_data.client = context;
		event_data.username = context->username;
		event_data.password = context->password;
		event_data.password_len = context->password_len;
		rc = cb_base->cb(MOSQ_EVT_BASIC_AUTH, &event_data, cb_base->userdata);
		എങ്കിൽ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}അല്ലെങ്കിൽ{
			തിരികെ rc;
		}
	}
	തിരികെ rc_final;
}


പൂർണ്ണസംഖ്യ mosquitto_basic_auth(ഘടന mosquitto *context)
{
	പൂർണ്ണസംഖ്യ rc;
	bool plugin_used = false;

	/* Global plugins */
	എങ്കിൽ(db.config->security_options.plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(&db.config->security_options, context);

		എങ്കിൽ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_used = true;
		}അല്ലെങ്കിൽ{
			തിരികെ rc;
		}
	}

	എങ്കിൽ(context->listener && context->listener->security_options->plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(context->listener->security_options, context);

		എങ്കിൽ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_used = true;
		}അല്ലെങ്കിൽ{
			തിരികെ rc;
		}
	}

	/* If all plugins deferred, this is a denial. plugin_used == false
	 * here, then no plugins were configured.
	 * anonymous logins are allowed. */
	എങ്കിൽ(plugin_used == false){
		എങ്കിൽ((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){

			തിരികെ MOSQ_ERR_SUCCESS;
		}അല്ലെങ്കിൽ{
			തിരികെ MOSQ_ERR_AUTH;
		}
	}അല്ലെങ്കിൽ{
		/* Can't have got here without at least one plugin returning MOSQ_ERR_PLUGIN_DEFER.
		 * This will now be a denial, unless it is anon and allow anon is true. */
		എങ്കിൽ(context->username == ശൂന്യ &&
				((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false))){

			തിരികെ MOSQ_ERR_SUCCESS;
		}അല്ലെങ്കിൽ{
			തിരികെ MOSQ_ERR_AUTH;
		}
	}

	തിരികെ rc;
}
