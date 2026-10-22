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


ساكن صحيح plugin__psk_key_get(بنية mosquitto__security_options *opts, بنية mosquitto *context, ثابت حرف *hint, ثابت حرف *identity, حرف *key, صحيح max_key_len)
{
	بنية mosquitto_evt_psk_key event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	صحيح rc;
	صحيح rc_final = MOSQ_ERR_SUCCESS;

	DL_FOREACH_SAFE(opts->plugin_callbacks.psk_key, cb_base, cb_next){
		memset(&event_data, 0, حجم(event_data));
		event_data.client = context;
		event_data.hint = hint;
		event_data.identity = identity;
		event_data.key = key;
		event_data.max_key_len = max_key_len;
		rc = cb_base->cb(MOSQ_EVT_PSK_KEY, &event_data, cb_base->userdata);
		אם(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}אחרת אם(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}אחרת{
			החזר rc;
		}
	}
	החזר rc_final;
}


صحيح mosquitto_psk_key_get(بنية mosquitto *context, ثابت حرف *hint, ثابت حرف *identity, حرف *key, صحيح max_key_len)
{
	صحيح rc;
	صحيح rc_final = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	אם(db.config->security_options.plugin_callbacks.psk_key){
		rc = plugin__psk_key_get(&db.config->security_options, context,
				hint, identity, key, max_key_len);

		אם(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}אחרת אם(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}אחרת{
			החזר rc;
		}
	}

	/* Per listener plugins */
	אם(context->listener){
		אם(context->listener->security_options->plugin_callbacks.psk_key){
			rc = plugin__psk_key_get(context->listener->security_options, context,
					hint, identity, key, max_key_len);

			אם(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing */
			}אחרת אם(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}אחרת{
				החזר rc;
			}
		}
	}אחרת{
		אם(db.config->per_listener_settings){
			החזר MOSQ_ERR_AUTH;
		}
	}

	rc = mosquitto_psk_key_get_default(context, hint, identity, key, max_key_len);
	אם(rc == MOSQ_ERR_PLUGIN_IGNORE){
		/* Do nothing */
	}אחרת אם(rc == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_PLUGIN_DEFER;
	}אחרת{
		החזר rc;
	}


	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured. */
	אם(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_AUTH;
	}
	החזר rc_final;
}
