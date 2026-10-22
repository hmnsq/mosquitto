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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "mosquitto/broker.h"
#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/broker_plugin.h"
#അന്തര്ഭാവ "lib_load.h"
#അന്തര്ഭാവ "utlist.h"


സ്ഥിതം പൂർണ്ണസംഖ്യ plugin_psk_താക്കോൽ_get(ഘടന mosquitto__security_options *opts, ഘടന mosquitto *context, സ്ഥിരം അക്ഷരം *hint, സ്ഥിരം അക്ഷരം *identity, അക്ഷരം *താക്കോൽ, പൂർണ്ണസംഖ്യ max_താക്കോൽ_len)
{
	ഘടന mosquitto_evt_psk_താക്കോൽ event_data;
	ഘടന mosquitto__callback *cb_base, *cb_next;
	പൂർണ്ണസംഖ്യ rc;
	പൂർണ്ണസംഖ്യ rc_final = MOSQ_ERR_SUCCESS;

	DL_FOREACH_SAFE(opts->plugin_callbacks.psk_താക്കോൽ, cb_base, cb_next){
		memset(&event_data, 0, വലുപ്പം(event_data));
		event_data.client = context;
		event_data.hint = hint;
		event_data.identity = identity;
		event_data.താക്കോൽ = താക്കോൽ;
		event_data.max_താക്കോൽ_len = max_താക്കോൽ_len;
		rc = cb_base->cb(MOSQ_EVT_PSK_താക്കോൽ, &event_data, cb_base->userdata);
		എങ്കിൽ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}അല്ലെങ്കിൽ{
			തിരികെ rc;
		}
	}
	തിരികെ rc_final;
}


പൂർണ്ണസംഖ്യ mosquitto_psk_താക്കോൽ_get(ഘടന mosquitto *context, സ്ഥിരം അക്ഷരം *hint, സ്ഥിരം അക്ഷരം *identity, അക്ഷരം *താക്കോൽ, പൂർണ്ണസംഖ്യ max_താക്കോൽ_len)
{
	പൂർണ്ണസംഖ്യ rc;
	പൂർണ്ണസംഖ്യ rc_final = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	എങ്കിൽ(db.config->security_options.plugin_callbacks.psk_താക്കോൽ){
		rc = plugin_psk_താക്കോൽ_get(&db.config->security_options, context,
				hint, identity, താക്കോൽ, max_താക്കോൽ_len);

		എങ്കിൽ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}അല്ലെങ്കിൽ{
			തിരികെ rc;
		}
	}

	/* Per listener plugins */
	എങ്കിൽ(context->listener){
		എങ്കിൽ(context->listener->security_options->plugin_callbacks.psk_താക്കോൽ){
			rc = plugin_psk_താക്കോൽ_get(context->listener->security_options, context,
					hint, identity, താക്കോൽ, max_താക്കോൽ_len);

			എങ്കിൽ(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing */
			}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}അല്ലെങ്കിൽ{
				തിരികെ rc;
			}
		}
	}അല്ലെങ്കിൽ{
		എങ്കിൽ(db.config->per_listener_settings){
			തിരികെ MOSQ_ERR_AUTH;
		}
	}

	rc = mosquitto_psk_താക്കോൽ_get_default(context, hint, identity, താക്കോൽ, max_താക്കോൽ_len);
	എങ്കിൽ(rc == MOSQ_ERR_PLUGIN_IGNORE){
		/* Do nothing */
	}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_PLUGIN_DEFER;
	}അല്ലെങ്കിൽ{
		തിരികെ rc;
	}


	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured. */
	എങ്കിൽ(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_AUTH;
	}
	തിരികെ rc_final;
}
