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


స్థిర పూర్ణాంకం plugin_psk_కీలకం_get(నిర్మాణం mosquitto__security_options *opts, నిర్మాణం mosquitto *context, స్థిరరాశి అక్షరం *hint, స్థిరరాశి అక్షరం *తటస్థం, అక్షరం *కీలకం, పూర్ణాంకం max_కీలకం_len)
{
	నిర్మాణం mosquitto_evt_psk_కీలకం event_దత్తాంశం;
	నిర్మాణం mosquitto__callback *cb_పునాది, *cb_తదుపరి;
	పూర్ణాంకం rc;
	పూర్ణాంకం rc_final = MOSQ_ERR_SUCCESS;

	DL_FOREACH_సురక్షిత(opts->plugin_callbacks.psk_కీలకం, cb_పునాది, cb_తదుపరి){
		memset(&event_దత్తాంశం, 0, పరిమాణం(event_దత్తాంశం));
		event_దత్తాంశం.client = context;
		event_దత్తాంశం.hint = hint;
		event_దత్తాంశం.తటస్థం = తటస్థం;
		event_దత్తాంశం.కీలకం = కీలకం;
		event_దత్తాంశం.max_కీలకం_len = max_కీలకం_len;
		rc = cb_పునాది->cb(MOSQ_EVT_PSK_కీలకం, &event_దత్తాంశం, cb_పునాది->userdata);
		ఒకవేళ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}లేకపోతే{
			తిరిగి rc;
		}
	}
	తిరిగి rc_final;
}


పూర్ణాంకం mosquitto_psk_కీలకం_get(నిర్మాణం mosquitto *context, స్థిరరాశి అక్షరం *hint, స్థిరరాశి అక్షరం *తటస్థం, అక్షరం *కీలకం, పూర్ణాంకం max_కీలకం_len)
{
	పూర్ణాంకం rc;
	పూర్ణాంకం rc_final = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	ఒకవేళ(db.config->security_options.plugin_callbacks.psk_కీలకం){
		rc = plugin_psk_కీలకం_get(&db.config->security_options, context,
				hint, తటస్థం, కీలకం, max_కీలకం_len);

		ఒకవేళ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}లేకపోతే{
			తిరిగి rc;
		}
	}

	/* Per listener plugins */
	ఒకవేళ(context->listener){
		ఒకవేళ(context->listener->security_options->plugin_callbacks.psk_కీలకం){
			rc = plugin_psk_కీలకం_get(context->listener->security_options, context,
					hint, తటస్థం, కీలకం, max_కీలకం_len);

			ఒకవేళ(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing */
			}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}లేకపోతే{
				తిరిగి rc;
			}
		}
	}లేకపోతే{
		ఒకవేళ(db.config->చొప్పున_listener_settings){
			తిరిగి MOSQ_ERR_AUTH;
		}
	}

	rc = mosquitto_psk_కీలకం_get_default(context, hint, తటస్థం, కీలకం, max_కీలకం_len);
	ఒకవేళ(rc == MOSQ_ERR_PLUGIN_IGNORE){
		/* Do nothing */
	}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_PLUGIN_DEFER;
	}లేకపోతే{
		తిరిగి rc;
	}


	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured. */
	ఒకవేళ(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_AUTH;
	}
	తిరిగి rc_final;
}
