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


స్థిర పూర్ణాంకం plugin_ext_auth_ప్రారంభం(నిర్మాణం mosquitto__security_options *opts, నిర్మాణం mosquitto *context, bool reauth, స్థిరరాశి శూన్య *దత్తాంశం_లోపల, uint16_t దత్తాంశం_లోపల_len, శూన్య **దత్తాంశం_బహిర్గతం, uint16_t *దత్తాంశం_బహిర్గతం_len)
{
	నిర్మాణం mosquitto_evt_extended_auth event_దత్తాంశం;
	నిర్మాణం mosquitto__callback *cb_పునాది, *cb_తదుపరి;
	పూర్ణాంకం rc;
	పూర్ణాంకం rc_final = MOSQ_ERR_PLUGIN_DEFER;

	వాడని(reauth);

	DL_FOREACH_సురక్షిత(opts->plugin_callbacks.ext_auth_ప్రారంభం, cb_పునాది, cb_తదుపరి){
		memset(&event_దత్తాంశం, 0, పరిమాణం(event_దత్తాంశం));
		event_దత్తాంశం.client = context;
		event_దత్తాంశం.auth_పద్ధతి = context->auth_పద్ధతి;
		event_దత్తాంశం.దత్తాంశం_లోపల = దత్తాంశం_లోపల;
		event_దత్తాంశం.దత్తాంశం_బహిర్గతం = ఖాళీ;
		event_దత్తాంశం.దత్తాంశం_లోపల_len = దత్తాంశం_లోపల_len;
		event_దత్తాంశం.దత్తాంశం_బహిర్గతం_len = 0;
		rc = cb_పునాది->cb(MOSQ_EVT_EXT_AUTH_ప్రారంభం, &event_దత్తాంశం, cb_పునాది->userdata);
		ఒకవేళ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}లేకపోతే{
			*దత్తాంశం_బహిర్గతం = event_దత్తాంశం.దత్తాంశం_బహిర్గతం;
			*దత్తాంశం_బహిర్గతం_len = event_దత్తాంశం.దత్తాంశం_బహిర్గతం_len;
			తిరిగి rc;
		}
	}
	తిరిగి rc_final;
}


పూర్ణాంకం mosquitto_security_auth_ప్రారంభం(నిర్మాణం mosquitto *context, bool reauth, స్థిరరాశి శూన్య *దత్తాంశం_లోపల, uint16_t దత్తాంశం_లోపల_len, శూన్య **దత్తాంశం_బహిర్గతం, uint16_t *దత్తాంశం_బహిర్గతం_len)
{
	పూర్ణాంకం rc;

	ఒకవేళ(!context || !context->listener || !context->auth_పద్ధతి){
		తిరిగి MOSQ_ERR_INVAL;
	}
	ఒకవేళ(!దత్తాంశం_బహిర్గతం || !దత్తాంశం_బహిర్గతం_len){
		తిరిగి MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	ఒకవేళ(db.config->security_options.plugin_callbacks.ext_auth_ప్రారంభం){
		rc = plugin_ext_auth_ప్రారంభం(&db.config->security_options, context,
				reauth, దత్తాంశం_లోపల, దత్తాంశం_లోపల_len, దత్తాంశం_బహిర్గతం, దత్తాంశం_బహిర్గతం_len);

		ఒకవేళ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}లేకపోతే{
			తిరిగి rc;
		}
	}

	/* Per listener plugins */
	ఒకవేళ(context->listener){
		ఒకవేళ(context->listener->security_options->plugin_callbacks.ext_auth_ప్రారంభం){
			rc = plugin_ext_auth_ప్రారంభం(context->listener->security_options, context,
					reauth, దత్తాంశం_లోపల, దత్తాంశం_లోపల_len, దత్తాంశం_బహిర్గతం, దత్తాంశం_బహిర్గతం_len);

			ఒకవేళ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}లేకపోతే{
				తిరిగి rc;
			}
		}
	}లేకపోతే{
		ఒకవేళ(db.config->చొప్పున_listener_settings){
			తిరిగి MOSQ_ERR_AUTH;
		}
	}

	తిరిగి MOSQ_ERR_NOT_SUPPORTED;
}


స్థిర పూర్ణాంకం plugin__ext_auth_continue(నిర్మాణం mosquitto__security_options *opts, నిర్మాణం mosquitto *context, స్థిరరాశి శూన్య *దత్తాంశం_లోపల, uint16_t దత్తాంశం_లోపల_len, శూన్య **దత్తాంశం_బహిర్గతం, uint16_t *దత్తాంశం_బహిర్గతం_len)
{
	పూర్ణాంకం rc;
	నిర్మాణం mosquitto_evt_extended_auth event_దత్తాంశం;
	నిర్మాణం mosquitto__callback *cb_పునాది, *cb_తదుపరి;

	DL_FOREACH_సురక్షిత(opts->plugin_callbacks.ext_auth_continue, cb_పునాది, cb_తదుపరి){
		memset(&event_దత్తాంశం, 0, పరిమాణం(event_దత్తాంశం));
		event_దత్తాంశం.client = context;
		event_దత్తాంశం.auth_పద్ధతి = context->auth_పద్ధతి;
		event_దత్తాంశం.దత్తాంశం_లోపల = దత్తాంశం_లోపల;
		event_దత్తాంశం.దత్తాంశం_బహిర్గతం = ఖాళీ;
		event_దత్తాంశం.దత్తాంశం_లోపల_len = దత్తాంశం_లోపల_len;
		event_దత్తాంశం.దత్తాంశం_బహిర్గతం_len = 0;
		rc = cb_పునాది->cb(MOSQ_EVT_EXT_AUTH_CONTINUE, &event_దత్తాంశం, cb_పునాది->userdata);
		ఒకవేళ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}లేకపోతే{
			*దత్తాంశం_బహిర్గతం = event_దత్తాంశం.దత్తాంశం_బహిర్గతం;
			*దత్తాంశం_బహిర్గతం_len = event_దత్తాంశం.దత్తాంశం_బహిర్గతం_len;
			తిరిగి rc;
		}
	}

	తిరిగి MOSQ_ERR_PLUGIN_DEFER;
}


పూర్ణాంకం mosquitto_security_auth_continue(నిర్మాణం mosquitto *context, స్థిరరాశి శూన్య *దత్తాంశం_లోపల, uint16_t దత్తాంశం_లోపల_len, శూన్య **దత్తాంశం_బహిర్గతం, uint16_t *దత్తాంశం_బహిర్గతం_len)
{
	పూర్ణాంకం rc;

	ఒకవేళ(!context || !context->listener || !context->auth_పద్ధతి){
		తిరిగి MOSQ_ERR_INVAL;
	}
	ఒకవేళ(!దత్తాంశం_బహిర్గతం || !దత్తాంశం_బహిర్గతం_len){
		తిరిగి MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	ఒకవేళ(db.config->security_options.plugin_callbacks.ext_auth_continue){
		rc = plugin__ext_auth_continue(&db.config->security_options, context,
				దత్తాంశం_లోపల, దత్తాంశం_లోపల_len, దత్తాంశం_బహిర్గతం, దత్తాంశం_బహిర్గతం_len);

		ఒకవేళ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}లేకపోతే{
			తిరిగి rc;
		}
	}

	/* Per listener plugins */
	ఒకవేళ(context->listener){
		ఒకవేళ(context->listener->security_options->plugin_callbacks.ext_auth_continue){
			rc = plugin__ext_auth_continue(context->listener->security_options, context,
					దత్తాంశం_లోపల, దత్తాంశం_లోపల_len, దత్తాంశం_బహిర్గతం, దత్తాంశం_బహిర్గతం_len);

			ఒకవేళ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}లేకపోతే{
				తిరిగి rc;
			}
		}
	}లేకపోతే{
		ఒకవేళ(db.config->చొప్పున_listener_settings){
			తిరిగి MOSQ_ERR_AUTH;
		}
	}

	తిరిగి MOSQ_ERR_NOT_SUPPORTED;
}
