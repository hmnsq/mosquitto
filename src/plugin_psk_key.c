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

#ضمن "config.h"

#ضمن <stdio.h>
#ضمن <string.h>

#ضمن "mosquitto/broker.h"
#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/broker_plugin.h"
#ضمن "lib_load.h"
#ضمن "utlist.h"


ساكن صحيح plugin_psk_کلی_get(بنية mosquitto__security_options *opts, بنية mosquitto *context, ثابت حرف *hint, ثابت حرف *بې_اغېزه_عنصر, حرف *کلی, صحيح max_کلی_len)
{
	بنية mosquitto_evt_psk_کلی event_معلومات;
	بنية mosquitto__callback *cb_بنسټ, *cb_بل;
	صحيح rc;
	صحيح rc_final = MOSQ_ERR_SUCCESS;

	DL_FOREACH_خوندي(opts->plugin_callbacks.psk_کلی, cb_بنسټ, cb_بل){
		memset(&event_معلومات, 0, حجم(event_معلومات));
		event_معلومات.client = context;
		event_معلومات.hint = hint;
		event_معلومات.بې_اغېزه_عنصر = بې_اغېزه_عنصر;
		event_معلومات.کلی = کلی;
		event_معلومات.max_کلی_len = max_کلی_len;
		rc = cb_بنسټ->cb(MOSQ_EVT_PSK_کلی, &event_معلومات, cb_بنسټ->userdata);
		إذا(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}وإلا إذا(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}وإلا{
			أرجع rc;
		}
	}
	أرجع rc_final;
}


صحيح mosquitto_psk_کلی_get(بنية mosquitto *context, ثابت حرف *hint, ثابت حرف *بې_اغېزه_عنصر, حرف *کلی, صحيح max_کلی_len)
{
	صحيح rc;
	صحيح rc_final = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	إذا(db.config->security_options.plugin_callbacks.psk_کلی){
		rc = plugin_psk_کلی_get(&db.config->security_options, context,
				hint, بې_اغېزه_عنصر, کلی, max_کلی_len);

		إذا(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}وإلا إذا(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}وإلا{
			أرجع rc;
		}
	}

	/* Per listener plugins */
	إذا(context->listener){
		إذا(context->listener->security_options->plugin_callbacks.psk_کلی){
			rc = plugin_psk_کلی_get(context->listener->security_options, context,
					hint, بې_اغېزه_عنصر, کلی, max_کلی_len);

			إذا(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing */
			}وإلا إذا(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}وإلا{
				أرجع rc;
			}
		}
	}وإلا{
		إذا(db.config->فی_listener_settings){
			أرجع MOSQ_ERR_AUTH;
		}
	}

	rc = mosquitto_psk_کلی_get_default(context, hint, بې_اغېزه_عنصر, کلی, max_کلی_len);
	إذا(rc == MOSQ_ERR_PLUGIN_IGNORE){
		/* Do nothing */
	}وإلا إذا(rc == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_PLUGIN_DEFER;
	}وإلا{
		أرجع rc;
	}


	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured. */
	إذا(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_AUTH;
	}
	أرجع rc_final;
}
