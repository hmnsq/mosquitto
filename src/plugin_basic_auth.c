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

#समावेश "config.h"

#समावेश <stdio.h>
#समावेश <string.h>

#समावेश "mosquitto/broker.h"
#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/broker_plugin.h"
#समावेश "lib_load.h"
#समावेश "utlist.h"


अटल पूर्णांक plugin__basic_auth(काष्ठा mosquitto__security_options *opts, काष्ठा mosquitto *context)
{
	काष्ठा mosquitto_evt_basic_auth event_तथ्य;
	काष्ठा mosquitto__callback *cb_base, *cb_अग्र;
	पूर्णांक rc;
	पूर्णांक rc_final = MOSQ_ERR_PLUGIN_IGNORE;

	DL_FOREACH_SAFE(opts->plugin_callbacks.basic_auth, cb_base, cb_अग्र){
		स_रखो(&event_तथ्य, 0, माप(event_तथ्य));
		event_तथ्य.client = context;
		event_तथ्य.username = context->username;
		event_तथ्य.password = context->password;
		event_तथ्य.password_len = context->password_len;
		rc = cb_base->cb(MOSQ_EVT_BASIC_AUTH, &event_तथ्य, cb_base->userdata);
		अगर(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}अन्यथा अगर(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}अन्यथा{
			वापस rc;
		}
	}
	वापस rc_final;
}


पूर्णांक mosquitto_basic_auth(काष्ठा mosquitto *context)
{
	पूर्णांक rc;
	bool plugin_used = false;

	/* Global plugins */
	अगर(db.config->security_options.plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(&db.config->security_options, context);

		अगर(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}अन्यथा अगर(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_used = true;
		}अन्यथा{
			वापस rc;
		}
	}

	अगर(context->listener && context->listener->security_options->plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(context->listener->security_options, context);

		अगर(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}अन्यथा अगर(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_used = true;
		}अन्यथा{
			वापस rc;
		}
	}

	/* If all plugins deferred, this is a denial. plugin_used == false
	 * here, then no plugins were configured.
	 * anonymous logins are allowed. */
	अगर(plugin_used == false){
		अगर((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){

			वापस MOSQ_ERR_SUCCESS;
		}अन्यथा{
			वापस MOSQ_ERR_AUTH;
		}
	}अन्यथा{
		/* Can't have got here without at least one plugin returning MOSQ_ERR_PLUGIN_DEFER.
		 * This will now be a denial, unless it is anon and allow anon is true. */
		अगर(context->username == शून्य &&
				((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false))){

			वापस MOSQ_ERR_SUCCESS;
		}अन्यथा{
			वापस MOSQ_ERR_AUTH;
		}
	}

	वापस rc;
}
