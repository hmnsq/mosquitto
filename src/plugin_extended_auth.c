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

#अन्तर्भाव "config.h"

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "mosquitto/broker.h"
#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/broker_plugin.h"
#अन्तर्भाव "lib_load.h"
#अन्तर्भाव "utlist.h"


स्थिर पूर्णाङ्क plugin__ext_auth_start(संरचना mosquitto__security_options *opts, संरचना mosquitto *context, bool reauth, ध्रुव शून्य *दत्त_अन्तर्, uint16_t दत्त_अन्तर्_len, शून्य **दत्त_बहिर्गमनम्, uint16_t *दत्त_बहिर्गमनम्_len)
{
	संरचना mosquitto_evt_extended_auth event_दत्त;
	संरचना mosquitto__callback *cb_आधारः, *cb_अग्र;
	पूर्णाङ्क rc;
	पूर्णाङ्क rc_final = MOSQ_ERR_PLUGIN_DEFER;

	अप्रयुक्तम्(reauth);

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_start, cb_आधारः, cb_अग्र){
		स्मृतिं_पूरय(&event_दत्त, 0, परिमाण(event_दत्त));
		event_दत्त.client = context;
		event_दत्त.auth_विधानम् = context->auth_विधानम्;
		event_दत्त.दत्त_अन्तर् = दत्त_अन्तर्;
		event_दत्त.दत्त_बहिर्गमनम् = रिक्त;
		event_दत्त.दत्त_अन्तर्_len = दत्त_अन्तर्_len;
		event_दत्त.दत्त_बहिर्गमनम्_len = 0;
		rc = cb_आधारः->cb(MOSQ_EVT_EXT_AUTH_START, &event_दत्त, cb_आधारः->userdata);
		यदि(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}अन्यथा यदि(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}अन्यथा{
			*दत्त_बहिर्गमनम् = event_दत्त.दत्त_बहिर्गमनम्;
			*दत्त_बहिर्गमनम्_len = event_दत्त.दत्त_बहिर्गमनम्_len;
			प्रत्यागम rc;
		}
	}
	प्रत्यागम rc_final;
}


पूर्णाङ्क mosquitto_security_auth_start(संरचना mosquitto *context, bool reauth, ध्रुव शून्य *दत्त_अन्तर्, uint16_t दत्त_अन्तर्_len, शून्य **दत्त_बहिर्गमनम्, uint16_t *दत्त_बहिर्गमनम्_len)
{
	पूर्णाङ्क rc;

	यदि(!context || !context->listener || !context->auth_विधानम्){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि(!दत्त_बहिर्गमनम् || !दत्त_बहिर्गमनम्_len){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	यदि(db.config->security_options.plugin_callbacks.ext_auth_start){
		rc = plugin__ext_auth_start(&db.config->security_options, context,
				reauth, दत्त_अन्तर्, दत्त_अन्तर्_len, दत्त_बहिर्गमनम्, दत्त_बहिर्गमनम्_len);

		यदि(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}अन्यथा{
			प्रत्यागम rc;
		}
	}

	/* Per listener plugins */
	यदि(context->listener){
		यदि(context->listener->security_options->plugin_callbacks.ext_auth_start){
			rc = plugin__ext_auth_start(context->listener->security_options, context,
					reauth, दत्त_अन्तर्, दत्त_अन्तर्_len, दत्त_बहिर्गमनम्, दत्त_बहिर्गमनम्_len);

			यदि(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}अन्यथा{
				प्रत्यागम rc;
			}
		}
	}अन्यथा{
		यदि(db.config->प्रत्येकम्_listener_settings){
			प्रत्यागम MOSQ_ERR_AUTH;
		}
	}

	प्रत्यागम MOSQ_ERR_NOT_SUPPORTED;
}


स्थिर पूर्णाङ्क plugin__ext_auth_continue(संरचना mosquitto__security_options *opts, संरचना mosquitto *context, ध्रुव शून्य *दत्त_अन्तर्, uint16_t दत्त_अन्तर्_len, शून्य **दत्त_बहिर्गमनम्, uint16_t *दत्त_बहिर्गमनम्_len)
{
	पूर्णाङ्क rc;
	संरचना mosquitto_evt_extended_auth event_दत्त;
	संरचना mosquitto__callback *cb_आधारः, *cb_अग्र;

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_continue, cb_आधारः, cb_अग्र){
		स्मृतिं_पूरय(&event_दत्त, 0, परिमाण(event_दत्त));
		event_दत्त.client = context;
		event_दत्त.auth_विधानम् = context->auth_विधानम्;
		event_दत्त.दत्त_अन्तर् = दत्त_अन्तर्;
		event_दत्त.दत्त_बहिर्गमनम् = रिक्त;
		event_दत्त.दत्त_अन्तर्_len = दत्त_अन्तर्_len;
		event_दत्त.दत्त_बहिर्गमनम्_len = 0;
		rc = cb_आधारः->cb(MOSQ_EVT_EXT_AUTH_CONTINUE, &event_दत्त, cb_आधारः->userdata);
		यदि(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}अन्यथा{
			*दत्त_बहिर्गमनम् = event_दत्त.दत्त_बहिर्गमनम्;
			*दत्त_बहिर्गमनम्_len = event_दत्त.दत्त_बहिर्गमनम्_len;
			प्रत्यागम rc;
		}
	}

	प्रत्यागम MOSQ_ERR_PLUGIN_DEFER;
}


पूर्णाङ्क mosquitto_security_auth_continue(संरचना mosquitto *context, ध्रुव शून्य *दत्त_अन्तर्, uint16_t दत्त_अन्तर्_len, शून्य **दत्त_बहिर्गमनम्, uint16_t *दत्त_बहिर्गमनम्_len)
{
	पूर्णाङ्क rc;

	यदि(!context || !context->listener || !context->auth_विधानम्){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि(!दत्त_बहिर्गमनम् || !दत्त_बहिर्गमनम्_len){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	यदि(db.config->security_options.plugin_callbacks.ext_auth_continue){
		rc = plugin__ext_auth_continue(&db.config->security_options, context,
				दत्त_अन्तर्, दत्त_अन्तर्_len, दत्त_बहिर्गमनम्, दत्त_बहिर्गमनम्_len);

		यदि(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}अन्यथा{
			प्रत्यागम rc;
		}
	}

	/* Per listener plugins */
	यदि(context->listener){
		यदि(context->listener->security_options->plugin_callbacks.ext_auth_continue){
			rc = plugin__ext_auth_continue(context->listener->security_options, context,
					दत्त_अन्तर्, दत्त_अन्तर्_len, दत्त_बहिर्गमनम्, दत्त_बहिर्गमनम्_len);

			यदि(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}अन्यथा{
				प्रत्यागम rc;
			}
		}
	}अन्यथा{
		यदि(db.config->प्रत्येकम्_listener_settings){
			प्रत्यागम MOSQ_ERR_AUTH;
		}
	}

	प्रत्यागम MOSQ_ERR_NOT_SUPPORTED;
}
