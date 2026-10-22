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


ਸਥਿਰ ਪੂਰਨ plugin_ext_auth_ਸ਼ੁਰੂ(ਬਣਤਰ mosquitto__security_options *opts, ਬਣਤਰ mosquitto *context, bool reauth, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ_ਵਿੱਚ, uint16_t ਅੰਕੜੇ_ਵਿੱਚ_len, ਖਾਲੀ **ਅੰਕੜੇ_ਬਾਹਰਲਾ, uint16_t *ਅੰਕੜੇ_ਬਾਹਰਲਾ_len)
{
	ਬਣਤਰ mosquitto_evt_extended_auth event_ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto__callback *cb_ਅਧਾਰ, *cb_ਅਗਲਾ;
	ਪੂਰਨ rc;
	ਪੂਰਨ rc_final = MOSQ_ERR_PLUGIN_DEFER;

	ਅਣਵਰਤਿਆ(reauth);

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(opts->plugin_callbacks.ext_auth_ਸ਼ੁਰੂ, cb_ਅਧਾਰ, cb_ਅਗਲਾ){
		memset(&event_ਅੰਕੜੇ, 0, ਆਕਾਰ(event_ਅੰਕੜੇ));
		event_ਅੰਕੜੇ.client = context;
		event_ਅੰਕੜੇ.auth_ਵਿਧੀ = context->auth_ਵਿਧੀ;
		event_ਅੰਕੜੇ.ਅੰਕੜੇ_ਵਿੱਚ = ਅੰਕੜੇ_ਵਿੱਚ;
		event_ਅੰਕੜੇ.ਅੰਕੜੇ_ਬਾਹਰਲਾ = ਨਿਰਾ;
		event_ਅੰਕੜੇ.ਅੰਕੜੇ_ਵਿੱਚ_len = ਅੰਕੜੇ_ਵਿੱਚ_len;
		event_ਅੰਕੜੇ.ਅੰਕੜੇ_ਬਾਹਰਲਾ_len = 0;
		rc = cb_ਅਧਾਰ->cb(MOSQ_EVT_EXT_AUTH_ਸ਼ੁਰੂ, &event_ਅੰਕੜੇ, cb_ਅਧਾਰ->userdata);
		ਜੇ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}ਨਹੀਂਤਾਂ{
			*ਅੰਕੜੇ_ਬਾਹਰਲਾ = event_ਅੰਕੜੇ.ਅੰਕੜੇ_ਬਾਹਰਲਾ;
			*ਅੰਕੜੇ_ਬਾਹਰਲਾ_len = event_ਅੰਕੜੇ.ਅੰਕੜੇ_ਬਾਹਰਲਾ_len;
			ਵਾਪਸ rc;
		}
	}
	ਵਾਪਸ rc_final;
}


ਪੂਰਨ mosquitto_security_auth_ਸ਼ੁਰੂ(ਬਣਤਰ mosquitto *context, bool reauth, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ_ਵਿੱਚ, uint16_t ਅੰਕੜੇ_ਵਿੱਚ_len, ਖਾਲੀ **ਅੰਕੜੇ_ਬਾਹਰਲਾ, uint16_t *ਅੰਕੜੇ_ਬਾਹਰਲਾ_len)
{
	ਪੂਰਨ rc;

	ਜੇ(!context || !context->listener || !context->auth_ਵਿਧੀ){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}
	ਜੇ(!ਅੰਕੜੇ_ਬਾਹਰਲਾ || !ਅੰਕੜੇ_ਬਾਹਰਲਾ_len){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	ਜੇ(db.config->security_options.plugin_callbacks.ext_auth_ਸ਼ੁਰੂ){
		rc = plugin_ext_auth_ਸ਼ੁਰੂ(&db.config->security_options, context,
				reauth, ਅੰਕੜੇ_ਵਿੱਚ, ਅੰਕੜੇ_ਵਿੱਚ_len, ਅੰਕੜੇ_ਬਾਹਰਲਾ, ਅੰਕੜੇ_ਬਾਹਰਲਾ_len);

		ਜੇ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}ਨਹੀਂਤਾਂ{
			ਵਾਪਸ rc;
		}
	}

	/* Per listener plugins */
	ਜੇ(context->listener){
		ਜੇ(context->listener->security_options->plugin_callbacks.ext_auth_ਸ਼ੁਰੂ){
			rc = plugin_ext_auth_ਸ਼ੁਰੂ(context->listener->security_options, context,
					reauth, ਅੰਕੜੇ_ਵਿੱਚ, ਅੰਕੜੇ_ਵਿੱਚ_len, ਅੰਕੜੇ_ਬਾਹਰਲਾ, ਅੰਕੜੇ_ਬਾਹਰਲਾ_len);

			ਜੇ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}ਨਹੀਂਤਾਂ{
				ਵਾਪਸ rc;
			}
		}
	}ਨਹੀਂਤਾਂ{
		ਜੇ(db.config->ਪ੍ਰਤੀ_listener_settings){
			ਵਾਪਸ MOSQ_ERR_AUTH;
		}
	}

	ਵਾਪਸ MOSQ_ERR_NOT_SUPPORTED;
}


ਸਥਿਰ ਪੂਰਨ plugin__ext_auth_continue(ਬਣਤਰ mosquitto__security_options *opts, ਬਣਤਰ mosquitto *context, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ_ਵਿੱਚ, uint16_t ਅੰਕੜੇ_ਵਿੱਚ_len, ਖਾਲੀ **ਅੰਕੜੇ_ਬਾਹਰਲਾ, uint16_t *ਅੰਕੜੇ_ਬਾਹਰਲਾ_len)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ mosquitto_evt_extended_auth event_ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto__callback *cb_ਅਧਾਰ, *cb_ਅਗਲਾ;

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(opts->plugin_callbacks.ext_auth_continue, cb_ਅਧਾਰ, cb_ਅਗਲਾ){
		memset(&event_ਅੰਕੜੇ, 0, ਆਕਾਰ(event_ਅੰਕੜੇ));
		event_ਅੰਕੜੇ.client = context;
		event_ਅੰਕੜੇ.auth_ਵਿਧੀ = context->auth_ਵਿਧੀ;
		event_ਅੰਕੜੇ.ਅੰਕੜੇ_ਵਿੱਚ = ਅੰਕੜੇ_ਵਿੱਚ;
		event_ਅੰਕੜੇ.ਅੰਕੜੇ_ਬਾਹਰਲਾ = ਨਿਰਾ;
		event_ਅੰਕੜੇ.ਅੰਕੜੇ_ਵਿੱਚ_len = ਅੰਕੜੇ_ਵਿੱਚ_len;
		event_ਅੰਕੜੇ.ਅੰਕੜੇ_ਬਾਹਰਲਾ_len = 0;
		rc = cb_ਅਧਾਰ->cb(MOSQ_EVT_EXT_AUTH_CONTINUE, &event_ਅੰਕੜੇ, cb_ਅਧਾਰ->userdata);
		ਜੇ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}ਨਹੀਂਤਾਂ{
			*ਅੰਕੜੇ_ਬਾਹਰਲਾ = event_ਅੰਕੜੇ.ਅੰਕੜੇ_ਬਾਹਰਲਾ;
			*ਅੰਕੜੇ_ਬਾਹਰਲਾ_len = event_ਅੰਕੜੇ.ਅੰਕੜੇ_ਬਾਹਰਲਾ_len;
			ਵਾਪਸ rc;
		}
	}

	ਵਾਪਸ MOSQ_ERR_PLUGIN_DEFER;
}


ਪੂਰਨ mosquitto_security_auth_continue(ਬਣਤਰ mosquitto *context, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ_ਵਿੱਚ, uint16_t ਅੰਕੜੇ_ਵਿੱਚ_len, ਖਾਲੀ **ਅੰਕੜੇ_ਬਾਹਰਲਾ, uint16_t *ਅੰਕੜੇ_ਬਾਹਰਲਾ_len)
{
	ਪੂਰਨ rc;

	ਜੇ(!context || !context->listener || !context->auth_ਵਿਧੀ){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}
	ਜੇ(!ਅੰਕੜੇ_ਬਾਹਰਲਾ || !ਅੰਕੜੇ_ਬਾਹਰਲਾ_len){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	ਜੇ(db.config->security_options.plugin_callbacks.ext_auth_continue){
		rc = plugin__ext_auth_continue(&db.config->security_options, context,
				ਅੰਕੜੇ_ਵਿੱਚ, ਅੰਕੜੇ_ਵਿੱਚ_len, ਅੰਕੜੇ_ਬਾਹਰਲਾ, ਅੰਕੜੇ_ਬਾਹਰਲਾ_len);

		ਜੇ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}ਨਹੀਂਤਾਂ{
			ਵਾਪਸ rc;
		}
	}

	/* Per listener plugins */
	ਜੇ(context->listener){
		ਜੇ(context->listener->security_options->plugin_callbacks.ext_auth_continue){
			rc = plugin__ext_auth_continue(context->listener->security_options, context,
					ਅੰਕੜੇ_ਵਿੱਚ, ਅੰਕੜੇ_ਵਿੱਚ_len, ਅੰਕੜੇ_ਬਾਹਰਲਾ, ਅੰਕੜੇ_ਬਾਹਰਲਾ_len);

			ਜੇ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}ਨਹੀਂਤਾਂ{
				ਵਾਪਸ rc;
			}
		}
	}ਨਹੀਂਤਾਂ{
		ਜੇ(db.config->ਪ੍ਰਤੀ_listener_settings){
			ਵਾਪਸ MOSQ_ERR_AUTH;
		}
	}

	ਵਾਪਸ MOSQ_ERR_NOT_SUPPORTED;
}
