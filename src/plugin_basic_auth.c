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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto/broker.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/broker_plugin.h"
#ଅନ୍ତର୍ଭାଵ "lib_load.h"
#ଅନ୍ତର୍ଭାଵ "utlist.h"


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin__basic_auth(ସଂରଚନା mosquitto__security_options *opts, ସଂରଚନା mosquitto *context)
{
	ସଂରଚନା mosquitto_evt_basic_auth event_data;
	ସଂରଚନା mosquitto__callback *cb_base, *cb_next;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc_final = MOSQ_ERR_PLUGIN_IGNORE;

	DL_FOREACH_SAFE(opts->plugin_callbacks.basic_auth, cb_base, cb_next){
		memset(&event_data, 0, ଆକାର(event_data));
		event_data.client = context;
		event_data.username = context->username;
		event_data.password = context->password;
		event_data.password_len = context->password_len;
		rc = cb_base->cb(MOSQ_EVT_BASIC_AUTH, &event_data, cb_base->userdata);
		ଯଦି(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}ଅନ୍ୟଥା ଯଦି(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}ଅନ୍ୟଥା{
			ଫେରନ୍ତୁ rc;
		}
	}
	ଫେରନ୍ତୁ rc_final;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_basic_auth(ସଂରଚନା mosquitto *context)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	bool plugin_used = false;

	/* Global plugins */
	ଯଦି(db.config->security_options.plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(&db.config->security_options, context);

		ଯଦି(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}ଅନ୍ୟଥା ଯଦି(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_used = true;
		}ଅନ୍ୟଥା{
			ଫେରନ୍ତୁ rc;
		}
	}

	ଯଦି(context->listener && context->listener->security_options->plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(context->listener->security_options, context);

		ଯଦି(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}ଅନ୍ୟଥା ଯଦି(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_used = true;
		}ଅନ୍ୟଥା{
			ଫେରନ୍ତୁ rc;
		}
	}

	/* If all plugins deferred, this is a denial. plugin_used == false
	 * here, then no plugins were configured.
	 * anonymous logins are allowed. */
	ଯଦି(plugin_used == false){
		ଯଦି((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){

			ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
		}ଅନ୍ୟଥା{
			ଫେରନ୍ତୁ MOSQ_ERR_AUTH;
		}
	}ଅନ୍ୟଥା{
		/* Can't have got here without at least one plugin returning MOSQ_ERR_PLUGIN_DEFER.
		 * This will now be a denial, unless it is anon and allow anon is true. */
		ଯଦି(context->username == ଖାଲି &&
				((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false))){

			ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
		}ଅନ୍ୟଥା{
			ଫେରନ୍ତୁ MOSQ_ERR_AUTH;
		}
	}

	ଫେରନ୍ତୁ rc;
}
