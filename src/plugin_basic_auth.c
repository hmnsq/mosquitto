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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "mosquitto/broker.h"
#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto/broker_plugin.h"
#অন্তর্ভাব "lib_load.h"
#অন্তর্ভাব "utlist.h"


স্থির পূর্ণাঙ্ক plugin__basic_auth(সংরচনা mosquitto__security_options *opts, সংরচনা mosquitto *context)
{
	সংরচনা mosquitto_evt_basic_auth event_data;
	সংরচনা mosquitto__callback *cb_base, *cb_next;
	পূর্ণাঙ্ক rc;
	পূর্ণাঙ্ক rc_final = MOSQ_ERR_PLUGIN_IGNORE;

	DL_FOREACH_SAFE(opts->plugin_callbacks.basic_auth, cb_base, cb_next){
		memset(&event_data, 0, পরিমাণ(event_data));
		event_data.client = context;
		event_data.username = context->username;
		event_data.password = context->password;
		event_data.password_len = context->password_len;
		rc = cb_base->cb(MOSQ_EVT_BASIC_AUTH, &event_data, cb_base->userdata);
		যদি(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}অন্যথা যদি(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}অন্যথা{
			প্রত্যাগম rc;
		}
	}
	প্রত্যাগম rc_final;
}


পূর্ণাঙ্ক mosquitto_basic_auth(সংরচনা mosquitto *context)
{
	পূর্ণাঙ্ক rc;
	bool plugin_used = false;

	/* Global plugins */
	যদি(db.config->security_options.plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(&db.config->security_options, context);

		যদি(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}অন্যথা যদি(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_used = true;
		}অন্যথা{
			প্রত্যাগম rc;
		}
	}

	যদি(context->listener && context->listener->security_options->plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(context->listener->security_options, context);

		যদি(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}অন্যথা যদি(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_used = true;
		}অন্যথা{
			প্রত্যাগম rc;
		}
	}

	/* If all plugins deferred, this is a denial. plugin_used == false
	 * here, then no plugins were configured.
	 * anonymous logins are allowed. */
	যদি(plugin_used == false){
		যদি((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){

			প্রত্যাগম MOSQ_ERR_SUCCESS;
		}অন্যথা{
			প্রত্যাগম MOSQ_ERR_AUTH;
		}
	}অন্যথা{
		/* Can't have got here without at least one plugin returning MOSQ_ERR_PLUGIN_DEFER.
		 * This will now be a denial, unless it is anon and allow anon is true. */
		যদি(context->username == রিক্ত &&
				((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false))){

			প্রত্যাগম MOSQ_ERR_SUCCESS;
		}অন্যথা{
			প্রত্যাগম MOSQ_ERR_AUTH;
		}
	}

	প্রত্যাগম rc;
}
