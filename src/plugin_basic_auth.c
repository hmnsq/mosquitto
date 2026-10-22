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

#అన్తర్భావ "config.h"

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ "mosquitto/broker.h"
#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto/broker_plugin.h"
#అన్తర్భావ "lib_load.h"
#అన్తర్భావ "utlist.h"


స్థిర పూర్ణాంకం plugin__basic_auth(నిర్మాణం mosquitto__security_options *opts, నిర్మాణం mosquitto *context)
{
	నిర్మాణం mosquitto_evt_basic_auth event_దత్తాంశం;
	నిర్మాణం mosquitto__callback *cb_పునాది, *cb_తదుపరి;
	పూర్ణాంకం rc;
	పూర్ణాంకం rc_final = MOSQ_ERR_PLUGIN_IGNORE;

	DL_FOREACH_సురక్షిత(opts->plugin_callbacks.basic_auth, cb_పునాది, cb_తదుపరి){
		memset(&event_దత్తాంశం, 0, పరిమాణం(event_దత్తాంశం));
		event_దత్తాంశం.client = context;
		event_దత్తాంశం.username = context->username;
		event_దత్తాంశం.password = context->password;
		event_దత్తాంశం.password_len = context->password_len;
		rc = cb_పునాది->cb(MOSQ_EVT_BASIC_AUTH, &event_దత్తాంశం, cb_పునాది->userdata);
		ఒకవేళ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing, this is as if the plugin doesn't exist */
		}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}లేకపోతే{
			తిరిగి rc;
		}
	}
	తిరిగి rc_final;
}


పూర్ణాంకం mosquitto_basic_auth(నిర్మాణం mosquitto *context)
{
	పూర్ణాంకం rc;
	bool plugin_వాడిన = false;

	/* Global plugins */
	ఒకవేళ(db.config->security_options.plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(&db.config->security_options, context);

		ఒకవేళ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_వాడిన = true;
		}లేకపోతే{
			తిరిగి rc;
		}
	}

	ఒకవేళ(context->listener && context->listener->security_options->plugin_callbacks.basic_auth){
		rc = plugin__basic_auth(context->listener->security_options, context);

		ఒకవేళ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_PLUGIN_DEFER){
			plugin_వాడిన = true;
		}లేకపోతే{
			తిరిగి rc;
		}
	}

	/* If all plugins deferred, this is a denial. plugin_used == false
	 * here, then no plugins were configured.
	 * anonymous logins are allowed. */
	ఒకవేళ(plugin_వాడిన == false){
		ఒకవేళ((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->చొప్పున_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){

			తిరిగి MOSQ_ERR_SUCCESS;
		}లేకపోతే{
			తిరిగి MOSQ_ERR_AUTH;
		}
	}లేకపోతే{
		/* Can't have got here without at least one plugin returning MOSQ_ERR_PLUGIN_DEFER.
		 * This will now be a denial, unless it is anon and allow anon is true. */
		ఒకవేళ(context->username == ఖాళీ &&
				((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->చొప్పున_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false))){

			తిరిగి MOSQ_ERR_SUCCESS;
		}లేకపోతే{
			తిరిగి MOSQ_ERR_AUTH;
		}
	}

	తిరిగి rc;
}
