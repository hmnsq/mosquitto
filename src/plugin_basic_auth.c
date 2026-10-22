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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>

#அந்தர்பாவ "mosquitto/broker.h"
#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto/broker_plugin.h"
#அந்தர்பாவ "lib_load.h"
#அந்தர்பாவ "utlist.h"


நிலையான முழு plugin__basic_auth(கட்டமைப்பு mosquitto__security_options *opts, கட்டமைப்பு mosquitto *context)
{
	கட்டமைப்பு mosquitto_evt_basic_auth event_data;
	கட்டமைப்பு mosquitto__callback *cb_base, *cb_next;
	முழு rc;
	முழு rc_final = MOSQ_ERR_PLUGIN_IGNORE;

	DL_FOREACH_SAFE(opts->plugin_callbacks.basic_auth, cb_base, cb_next){
		memset(&event_data, 0, அளவு(event_data));
		event_data.client = context;
		event_data.username = context->username;
		event_data.password = context->password;
		event_data.password_len = context->password_len;
		rc = cb_base->cb(MOSQ_EVT_BASIC_AUTH, &event_data, cb_base->userdata);
		எனில்(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}இல்லையெனில் எனில்(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}இல்லையெனில்{
			திரும்பு rc;
		}
	}
	திரும்பு rc_final;
}


முழு mosquitto_basic_auth(கட்டமைப்பு mosquitto *context)
{
	முழு rc;
	bool plugin_used = false;

	/* Global plugins */
	எனில்(db.config->security_options.plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(&db.config->security_options, context);

		எனில்(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}இல்லையெனில் எனில்(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_used = true;
		}இல்லையெனில்{
			திரும்பு rc;
		}
	}

	எனில்(context->listener && context->listener->security_options->plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(context->listener->security_options, context);

		எனில்(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}இல்லையெனில் எனில்(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_used = true;
		}இல்லையெனில்{
			திரும்பு rc;
		}
	}

	/* If all plugins deferred, this is a denial. plugin_used == false
	 * here, then no plugins were configured.
	 * anonymous logins are allowed. */
	எனில்(plugin_used == false){
		எனில்((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){

			திரும்பு MOSQ_ERR_SUCCESS;
		}இல்லையெனில்{
			திரும்பு MOSQ_ERR_AUTH;
		}
	}இல்லையெனில்{
		/* Can't have got here without at least one plugin returning MOSQ_ERR_PLUGIN_DEFER.
		 * This will now be a denial, unless it is anon and allow anon is true. */
		எனில்(context->username == வெற்று &&
				((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false))){

			திரும்பு MOSQ_ERR_SUCCESS;
		}இல்லையெனில்{
			திரும்பு MOSQ_ERR_AUTH;
		}
	}

	திரும்பு rc;
}
