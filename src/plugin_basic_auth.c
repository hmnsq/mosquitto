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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "mosquitto/broker.h"
#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/broker_plugin.h"
#અન્તર્ભાવ "lib_load.h"
#અન્તર્ભાવ "utlist.h"


સ્થિર પૂર્ણાંક plugin__basic_auth(સંરચના mosquitto__security_options *opts, સંરચના mosquitto *context)
{
	સંરચના mosquitto_evt_basic_auth event_data;
	સંરચના mosquitto__callback *cb_base, *cb_next;
	પૂર્ણાંક rc;
	પૂર્ણાંક rc_final = MOSQ_ERR_PLUGIN_IGNORE;

	DL_FOREACH_SAFE(opts->plugin_callbacks.basic_auth, cb_base, cb_next){
		memset(&event_data, 0, કદ(event_data));
		event_data.client = context;
		event_data.username = context->username;
		event_data.password = context->password;
		event_data.password_len = context->password_len;
		rc = cb_base->cb(MOSQ_EVT_BASIC_AUTH, &event_data, cb_base->userdata);
		જો(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}નહીંતર જો(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}નહીંતર{
			પાછા rc;
		}
	}
	પાછા rc_final;
}


પૂર્ણાંક mosquitto_basic_auth(સંરચના mosquitto *context)
{
	પૂર્ણાંક rc;
	bool plugin_used = false;

	/* Global plugins */
	જો(db.config->security_options.plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(&db.config->security_options, context);

		જો(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}નહીંતર જો(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_used = true;
		}નહીંતર{
			પાછા rc;
		}
	}

	જો(context->listener && context->listener->security_options->plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(context->listener->security_options, context);

		જો(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}નહીંતર જો(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_used = true;
		}નહીંતર{
			પાછા rc;
		}
	}

	/* If all plugins deferred, this is a denial. plugin_used == false
	 * here, then no plugins were configured.
	 * anonymous logins are allowed. */
	જો(plugin_used == false){
		જો((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){

			પાછા MOSQ_ERR_SUCCESS;
		}નહીંતર{
			પાછા MOSQ_ERR_AUTH;
		}
	}નહીંતર{
		/* Can't have got here without at least one plugin returning MOSQ_ERR_PLUGIN_DEFER.
		 * This will now be a denial, unless it is anon and allow anon is true. */
		જો(context->username == ખાલી &&
				((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false))){

			પાછા MOSQ_ERR_SUCCESS;
		}નહીંતર{
			પાછા MOSQ_ERR_AUTH;
		}
	}

	પાછા rc;
}
