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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/broker.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/broker_plugin.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "lib_load.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utlist.h"


ਸਥਿਰ ਪੂਰਨ plugin_psk_ਕੁੰਜੀ_get(ਬਣਤਰ mosquitto__security_options *opts, ਬਣਤਰ mosquitto *context, ਅਚਲ ਅੱਖਰ *hint, ਅਚਲ ਅੱਖਰ *ਪਛਾਣ_ਤੱਤ, ਅੱਖਰ *ਕੁੰਜੀ, ਪੂਰਨ max_ਕੁੰਜੀ_len)
{
	ਬਣਤਰ mosquitto_evt_psk_ਕੁੰਜੀ event_ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto__callback *cb_ਅਧਾਰ, *cb_ਅਗਲਾ;
	ਪੂਰਨ rc;
	ਪੂਰਨ rc_final = MOSQ_ERR_SUCCESS;

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(opts->plugin_callbacks.psk_ਕੁੰਜੀ, cb_ਅਧਾਰ, cb_ਅਗਲਾ){
		memset(&event_ਅੰਕੜੇ, 0, ਆਕਾਰ(event_ਅੰਕੜੇ));
		event_ਅੰਕੜੇ.client = context;
		event_ਅੰਕੜੇ.hint = hint;
		event_ਅੰਕੜੇ.ਪਛਾਣ_ਤੱਤ = ਪਛਾਣ_ਤੱਤ;
		event_ਅੰਕੜੇ.ਕੁੰਜੀ = ਕੁੰਜੀ;
		event_ਅੰਕੜੇ.max_ਕੁੰਜੀ_len = max_ਕੁੰਜੀ_len;
		rc = cb_ਅਧਾਰ->cb(MOSQ_EVT_PSK_ਕੁੰਜੀ, &event_ਅੰਕੜੇ, cb_ਅਧਾਰ->userdata);
		ਜੇ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}ਨਹੀਂਤਾਂ{
			ਵਾਪਸ rc;
		}
	}
	ਵਾਪਸ rc_final;
}


ਪੂਰਨ mosquitto_psk_ਕੁੰਜੀ_get(ਬਣਤਰ mosquitto *context, ਅਚਲ ਅੱਖਰ *hint, ਅਚਲ ਅੱਖਰ *ਪਛਾਣ_ਤੱਤ, ਅੱਖਰ *ਕੁੰਜੀ, ਪੂਰਨ max_ਕੁੰਜੀ_len)
{
	ਪੂਰਨ rc;
	ਪੂਰਨ rc_final = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	ਜੇ(db.config->security_options.plugin_callbacks.psk_ਕੁੰਜੀ){
		rc = plugin_psk_ਕੁੰਜੀ_get(&db.config->security_options, context,
				hint, ਪਛਾਣ_ਤੱਤ, ਕੁੰਜੀ, max_ਕੁੰਜੀ_len);

		ਜੇ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}ਨਹੀਂਤਾਂ{
			ਵਾਪਸ rc;
		}
	}

	/* Per listener plugins */
	ਜੇ(context->listener){
		ਜੇ(context->listener->security_options->plugin_callbacks.psk_ਕੁੰਜੀ){
			rc = plugin_psk_ਕੁੰਜੀ_get(context->listener->security_options, context,
					hint, ਪਛਾਣ_ਤੱਤ, ਕੁੰਜੀ, max_ਕੁੰਜੀ_len);

			ਜੇ(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing */
			}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}ਨਹੀਂਤਾਂ{
				ਵਾਪਸ rc;
			}
		}
	}ਨਹੀਂਤਾਂ{
		ਜੇ(db.config->ਪ੍ਰਤੀ_listener_settings){
			ਵਾਪਸ MOSQ_ERR_AUTH;
		}
	}

	rc = mosquitto_psk_ਕੁੰਜੀ_get_default(context, hint, ਪਛਾਣ_ਤੱਤ, ਕੁੰਜੀ, max_ਕੁੰਜੀ_len);
	ਜੇ(rc == MOSQ_ERR_PLUGIN_IGNORE){
		/* Do nothing */
	}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_PLUGIN_DEFER;
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ rc;
	}


	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured. */
	ਜੇ(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_AUTH;
	}
	ਵਾਪਸ rc_final;
}
