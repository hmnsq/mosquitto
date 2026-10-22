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


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ plugin__basic_auth(ರಚನೆ mosquitto__security_options *opts, ರಚನೆ mosquitto *context)
{
	ರಚನೆ mosquitto_evt_basic_auth event_ದತ್ತಾಂಶ;
	ರಚನೆ mosquitto__callback *cb_ತಳ, *cb_ಮುಂದಿನ;
	ಪೂರ್ಣಾಂಕ rc;
	ಪೂರ್ಣಾಂಕ rc_final = MOSQ_ERR_PLUGIN_IGNORE;

	DL_FOREACH_ಸುರಕ್ಷಿತ(opts->plugin_callbacks.basic_auth, cb_ತಳ, cb_ಮುಂದಿನ){
		memset(&event_ದತ್ತಾಂಶ, 0, ಗಾತ್ರ(event_ದತ್ತಾಂಶ));
		event_ದತ್ತಾಂಶ.client = context;
		event_ದತ್ತಾಂಶ.username = context->username;
		event_ದತ್ತಾಂಶ.password = context->password;
		event_ದತ್ತಾಂಶ.password_len = context->password_len;
		rc = cb_ತಳ->cb(MOSQ_EVT_BASIC_AUTH, &event_ದತ್ತಾಂಶ, cb_ತಳ->userdata);
		ಆದರೆ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಹಿಂತಿರುಗಿ rc;
		}
	}
	ಹಿಂತಿರುಗಿ rc_final;
}


ಪೂರ್ಣಾಂಕ mosquitto_basic_auth(ರಚನೆ mosquitto *context)
{
	ಪೂರ್ಣಾಂಕ rc;
	bool plugin_ಬಳಸಿದ = false;

	/* Global plugins */
	ಆದರೆ(db.config->security_options.plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(&db.config->security_options, context);

		ಆದರೆ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_ಬಳಸಿದ = true;
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಹಿಂತಿರುಗಿ rc;
		}
	}

	ಆದರೆ(context->listener && context->listener->security_options->plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(context->listener->security_options, context);

		ಆದರೆ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_ಬಳಸಿದ = true;
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಹಿಂತಿರುಗಿ rc;
		}
	}

	/* If all plugins deferred, this is a denial. plugin_used == false
	 * here, then no plugins were configured.
	 * anonymous logins are allowed. */
	ಆದರೆ(plugin_ಬಳಸಿದ == false){
		ಆದರೆ((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->ಪ್ರತಿ_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){

			ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಹಿಂತಿರುಗಿ MOSQ_ERR_AUTH;
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		/* Can't have got here without at least one plugin returning MOSQ_ERR_PLUGIN_DEFER.
		 * This will now be a denial, unless it is anon and allow anon is true. */
		ಆದರೆ(context->username == ಖಾಲಿ &&
				((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->ಪ್ರತಿ_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false))){

			ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಹಿಂತಿರುಗಿ MOSQ_ERR_AUTH;
		}
	}

	ಹಿಂತಿರುಗಿ rc;
}
