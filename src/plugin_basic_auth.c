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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "mosquitto/broker.h"
#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/broker_plugin.h"
#ಅನ್ತರ್ಭಾವ "lib_load.h"
#ಅನ್ತರ್ಭಾವ "utlist.h"


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ plugin__basic_auth(ಸಂರಚನಾ mosquitto__security_options *opts, ಸಂರಚನಾ mosquitto *context)
{
	ಸಂರಚನಾ mosquitto_evt_basic_auth event_data;
	ಸಂರಚನಾ mosquitto__callback *cb_base, *cb_next;
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಪೂರ್ಣಾಙ್ಕ rc_final = MOSQ_ERR_PLUGIN_IGNORE;

	DL_FOREACH_SAFE(opts->plugin_callbacks.basic_auth, cb_base, cb_next){
		memset(&event_data, 0, ಪರಿಮಾಣ(event_data));
		event_data.client = context;
		event_data.username = context->username;
		event_data.password = context->password;
		event_data.password_len = context->password_len;
		rc = cb_base->cb(MOSQ_EVT_BASIC_AUTH, &event_data, cb_base->userdata);
		ಯದಿ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}ಅನ್ಯಥಾ{
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}
	ಪ್ರತ್ಯಾಗಮ rc_final;
}


ಪೂರ್ಣಾಙ್ಕ mosquitto_basic_auth(ಸಂರಚನಾ mosquitto *context)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	bool plugin_used = false;

	/* Global plugins */
	ಯದಿ(db.config->security_options.plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(&db.config->security_options, context);

		ಯದಿ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_used = true;
		}ಅನ್ಯಥಾ{
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}

	ಯದಿ(context->listener && context->listener->security_options->plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(context->listener->security_options, context);

		ಯದಿ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_used = true;
		}ಅನ್ಯಥಾ{
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}

	/* If all plugins deferred, this is a denial. plugin_used == false
	 * here, then no plugins were configured.
	 * anonymous logins are allowed. */
	ಯದಿ(plugin_used == false){
		ಯದಿ((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){

			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
		}ಅನ್ಯಥಾ{
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_AUTH;
		}
	}ಅನ್ಯಥಾ{
		/* Can't have got here without at least one plugin returning MOSQ_ERR_PLUGIN_DEFER.
		 * This will now be a denial, unless it is anon and allow anon is true. */
		ಯದಿ(context->username == ರಿಕ್ತ &&
				((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false))){

			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
		}ಅನ್ಯಥಾ{
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_AUTH;
		}
	}

	ಪ್ರತ್ಯಾಗಮ rc;
}
