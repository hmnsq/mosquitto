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

#अन्तर्भाव "config.h"

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "mosquitto/broker.h"
#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/broker_plugin.h"
#अन्तर्भाव "lib_load.h"
#अन्तर्भाव "utlist.h"


स्थिर पूर्णाङ्क plugin__basic_auth(संरचना mosquitto__security_options *opts, संरचना mosquitto *context)
{
	संरचना mosquitto_evt_basic_auth event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	पूर्णाङ्क rc;
	पूर्णाङ्क rc_final = MOSQ_ERR_PLUGIN_IGNORE;

	DL_FOREACH_SAFE(opts->plugin_callbacks.basic_auth, cb_base, cb_next){
		memset(&event_data, 0, परिमाण(event_data));
		event_data.client = context;
		event_data.username = context->username;
		event_data.password = context->password;
		event_data.password_len = context->password_len;
		rc = cb_base->cb(MOSQ_EVT_BASIC_AUTH, &event_data, cb_base->userdata);
		जइ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}अण्णहा जइ(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}अण्णहा{
			प्रत्यागम rc;
		}
	}
	प्रत्यागम rc_final;
}


पूर्णाङ्क mosquitto_basic_auth(संरचना mosquitto *context)
{
	पूर्णाङ्क rc;
	bool plugin_used = false;

	/* Global plugins */
	जइ(db.config->security_options.plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(&db.config->security_options, context);

		जइ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}अण्णहा जइ(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_used = true;
		}अण्णहा{
			प्रत्यागम rc;
		}
	}

	जइ(context->listener && context->listener->security_options->plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(context->listener->security_options, context);

		जइ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}अण्णहा जइ(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_used = true;
		}अण्णहा{
			प्रत्यागम rc;
		}
	}

	/* If all plugins deferred, this is a denial. plugin_used == false
	 * here, then no plugins were configured.
	 * anonymous logins are allowed. */
	जइ(plugin_used == false){
		जइ((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){

			प्रत्यागम MOSQ_ERR_SUCCESS;
		}अण्णहा{
			प्रत्यागम MOSQ_ERR_AUTH;
		}
	}अण्णहा{
		/* Can't have got here without at least one plugin returning MOSQ_ERR_PLUGIN_DEFER.
		 * This will now be a denial, unless it is anon and allow anon is true. */
		जइ(context->username == रिक्त &&
				((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false))){

			प्रत्यागम MOSQ_ERR_SUCCESS;
		}अण्णहा{
			प्रत्यागम MOSQ_ERR_AUTH;
		}
	}

	प्रत्यागम rc;
}
