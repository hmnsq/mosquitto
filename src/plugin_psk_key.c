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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>

#அந்தர்பாவ "mosquitto/broker.h"
#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto/broker_plugin.h"
#அந்தர்பாவ "lib_load.h"
#அந்தர்பாவ "utlist.h"


நிலையான முழு plugin_psk_சாவி_get(கட்டமைப்பு mosquitto__security_options *opts, கட்டமைப்பு mosquitto *context, மாறிலி எழுத்து *hint, மாறிலி எழுத்து *identity, எழுத்து *சாவி, முழு max_சாவி_len)
{
	கட்டமைப்பு mosquitto_evt_psk_சாவி event_data;
	கட்டமைப்பு mosquitto__callback *cb_base, *cb_next;
	முழு rc;
	முழு rc_final = MOSQ_ERR_SUCCESS;

	DL_FOREACH_SAFE(opts->plugin_callbacks.psk_சாவி, cb_base, cb_next){
		memset(&event_data, 0, அளவு(event_data));
		event_data.client = context;
		event_data.hint = hint;
		event_data.identity = identity;
		event_data.சாவி = சாவி;
		event_data.max_சாவி_len = max_சாவி_len;
		rc = cb_base->cb(MOSQ_EVT_PSK_சாவி, &event_data, cb_base->userdata);
		எனில்(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}இல்லையெனில் எனில்(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}இல்லையெனில்{
			திரும்பு rc;
		}
	}
	திரும்பு rc_final;
}


முழு mosquitto_psk_சாவி_get(கட்டமைப்பு mosquitto *context, மாறிலி எழுத்து *hint, மாறிலி எழுத்து *identity, எழுத்து *சாவி, முழு max_சாவி_len)
{
	முழு rc;
	முழு rc_final = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	எனில்(db.config->security_options.plugin_callbacks.psk_சாவி){
		rc = plugin_psk_சாவி_get(&db.config->security_options, context,
				hint, identity, சாவி, max_சாவி_len);

		எனில்(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}இல்லையெனில் எனில்(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}இல்லையெனில்{
			திரும்பு rc;
		}
	}

	/* Per listener plugins */
	எனில்(context->listener){
		எனில்(context->listener->security_options->plugin_callbacks.psk_சாவி){
			rc = plugin_psk_சாவி_get(context->listener->security_options, context,
					hint, identity, சாவி, max_சாவி_len);

			எனில்(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing */
			}இல்லையெனில் எனில்(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}இல்லையெனில்{
				திரும்பு rc;
			}
		}
	}இல்லையெனில்{
		எனில்(db.config->per_listener_settings){
			திரும்பு MOSQ_ERR_AUTH;
		}
	}

	rc = mosquitto_psk_சாவி_get_default(context, hint, identity, சாவி, max_சாவி_len);
	எனில்(rc == MOSQ_ERR_PLUGIN_IGNORE){
		/* Do nothing */
	}இல்லையெனில் எனில்(rc == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_PLUGIN_DEFER;
	}இல்லையெனில்{
		திரும்பு rc;
	}


	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured. */
	எனில்(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_AUTH;
	}
	திரும்பு rc_final;
}
