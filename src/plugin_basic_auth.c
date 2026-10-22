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


স্থির পূর্ণসংখ্যা plugin__basic_auth(কাঠামো mosquitto__security_options *opts, কাঠামো mosquitto *context)
{
	কাঠামো mosquitto_evt_basic_auth event_তথ্য;
	কাঠামো mosquitto__callback *cb_base, *cb_পরবর্তী;
	পূর্ণসংখ্যা rc;
	পূর্ণসংখ্যা rc_final = MOSQ_ERR_PLUGIN_IGNORE;

	DL_FOREACH_SAFE(opts->plugin_callbacks.basic_auth, cb_base, cb_পরবর্তী){
		memset(&event_তথ্য, 0, আকার(event_তথ্য));
		event_তথ্য.client = context;
		event_তথ্য.username = context->username;
		event_তথ্য.password = context->password;
		event_তথ্য.password_len = context->password_len;
		rc = cb_base->cb(MOSQ_EVT_BASIC_AUTH, &event_তথ্য, cb_base->userdata);
		যদি(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}অন্যথা যদি(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}অন্যথা{
			ফেরত rc;
		}
	}
	ফেরত rc_final;
}


পূর্ণসংখ্যা mosquitto_basic_auth(কাঠামো mosquitto *context)
{
	পূর্ণসংখ্যা rc;
	bool plugin_used = false;

	/* Global plugins */
	যদি(db.config->security_options.plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(&db.config->security_options, context);

		যদি(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}অন্যথা যদি(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_used = true;
		}অন্যথা{
			ফেরত rc;
		}
	}

	যদি(context->listener && context->listener->security_options->plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(context->listener->security_options, context);

		যদি(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}অন্যথা যদি(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_used = true;
		}অন্যথা{
			ফেরত rc;
		}
	}

	/* If all plugins deferred, this is a denial. plugin_used == false
	 * here, then no plugins were configured.
	 * anonymous logins are allowed. */
	যদি(plugin_used == false){
		যদি((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){

			ফেরত MOSQ_ERR_SUCCESS;
		}অন্যথা{
			ফেরত MOSQ_ERR_AUTH;
		}
	}অন্যথা{
		/* Can't have got here without at least one plugin returning MOSQ_ERR_PLUGIN_DEFER.
		 * This will now be a denial, unless it is anon and allow anon is true. */
		যদি(context->username == ফাঁকা &&
				((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false))){

			ফেরত MOSQ_ERR_SUCCESS;
		}অন্যথা{
			ফেরত MOSQ_ERR_AUTH;
		}
	}

	ফেরত rc;
}
