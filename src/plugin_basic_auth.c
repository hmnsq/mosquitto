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
	संरचना mosquitto_evt_basic_auth event_दत्त;
	संरचना mosquitto__callback *cb_आधारः, *cb_अग्र;
	पूर्णाङ्क rc;
	पूर्णाङ्क rc_final = MOSQ_ERR_PLUGIN_IGNORE;

	DL_FOREACH_SAFE(opts->plugin_callbacks.basic_auth, cb_आधारः, cb_अग्र){
		स्मृतिं_पूरय(&event_दत्त, 0, परिमाण(event_दत्त));
		event_दत्त.client = context;
		event_दत्त.username = context->username;
		event_दत्त.password = context->password;
		event_दत्त.password_len = context->password_len;
		rc = cb_आधारः->cb(MOSQ_EVT_BASIC_AUTH, &event_दत्त, cb_आधारः->userdata);
		यदि(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}अन्यथा यदि(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}अन्यथा{
			प्रत्यागम rc;
		}
	}
	प्रत्यागम rc_final;
}


पूर्णाङ्क mosquitto_basic_auth(संरचना mosquitto *context)
{
	पूर्णाङ्क rc;
	bool plugin_प्रयुक्तम् = false;

	/* Global plugins */
	यदि(db.config->security_options.plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(&db.config->security_options, context);

		यदि(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}अन्यथा यदि(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_प्रयुक्तम् = true;
		}अन्यथा{
			प्रत्यागम rc;
		}
	}

	यदि(context->listener && context->listener->security_options->plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(context->listener->security_options, context);

		यदि(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}अन्यथा यदि(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_प्रयुक्तम् = true;
		}अन्यथा{
			प्रत्यागम rc;
		}
	}

	/* If all plugins deferred, this is a denial. plugin_used == false
	 * here, then no plugins were configured.
	 * anonymous logins are allowed. */
	यदि(plugin_प्रयुक्तम् == false){
		यदि((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->प्रत्येकम्_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){

			प्रत्यागम MOSQ_ERR_SUCCESS;
		}अन्यथा{
			प्रत्यागम MOSQ_ERR_AUTH;
		}
	}अन्यथा{
		/* Can't have got here without at least one plugin returning MOSQ_ERR_PLUGIN_DEFER.
		 * This will now be a denial, unless it is anon and allow anon is true. */
		यदि(context->username == रिक्त &&
				((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->प्रत्येकम्_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false))){

			प्रत्यागम MOSQ_ERR_SUCCESS;
		}अन्यथा{
			प्रत्यागम MOSQ_ERR_AUTH;
		}
	}

	प्रत्यागम rc;
}
