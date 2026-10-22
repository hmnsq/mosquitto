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

#include "config.h"

#include <stdio.h>
#include <string.h>

#include "mosquitto/broker.h"
#include "mosquitto_broker_internal.h"
#include "mosquitto/broker_plugin.h"
#include "lib_load.h"
#include "utlist.h"


static จำนวนเต็ม plugin__basic_auth(sa_nracanaa mosquitto__security_options *opts, sa_nracanaa mosquitto *context)
{
	sa_nracanaa mosquitto_evt_basic_auth event_data;
	sa_nracanaa mosquitto__callback *cb_base, *cb_next;
	จำนวนเต็ม rc;
	จำนวนเต็ม rc_final = MOSQ_ERR_PLUGIN_IGNORE;

	DL_FOREACH_SAFE(opts->plugin_callbacks.basic_auth, cb_base, cb_next){
		memset(&event_data, 0, parimaa_na(event_data));
		event_data.client = context;
		event_data.username = context->username;
		event_data.password = context->password;
		event_data.password_len = context->password_len;
		rc = cb_base->cb(MOSQ_EVT_BASIC_AUTH, &event_data, cb_base->userdata);
		ถ้า(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}มิฉะนั้น ถ้า(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}มิฉะนั้น{
			คืน rc;
		}
	}
	คืน rc_final;
}


จำนวนเต็ม mosquitto_basic_auth(sa_nracanaa mosquitto *context)
{
	จำนวนเต็ม rc;
	bool plugin_used = false;

	/* Global plugins */
	ถ้า(db.config->security_options.plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(&db.config->security_options, context);

		ถ้า(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}มิฉะนั้น ถ้า(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_used = true;
		}มิฉะนั้น{
			คืน rc;
		}
	}

	ถ้า(context->listener && context->listener->security_options->plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(context->listener->security_options, context);

		ถ้า(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}มิฉะนั้น ถ้า(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_used = true;
		}มิฉะนั้น{
			คืน rc;
		}
	}

	/* If all plugins deferred, this is a denial. plugin_used == false
	 * here, then no plugins were configured.
	 * anonymous logins are allowed. */
	ถ้า(plugin_used == false){
		ถ้า((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){

			คืน MOSQ_ERR_SUCCESS;
		}มิฉะนั้น{
			คืน MOSQ_ERR_AUTH;
		}
	}มิฉะนั้น{
		/* Can't have got here without at least one plugin returning MOSQ_ERR_PLUGIN_DEFER.
		 * This will now be a denial, unless it is anon and allow anon is true. */
		ถ้า(context->username == NULL &&
				((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false))){

			คืน MOSQ_ERR_SUCCESS;
		}มิฉะนั้น{
			คืน MOSQ_ERR_AUTH;
		}
	}

	คืน rc;
}
