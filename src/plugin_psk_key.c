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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "mosquitto/broker.h"
#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/broker_plugin.h"
#અન્તર્ભાવ "lib_load.h"
#અન્તર્ભાવ "utlist.h"


સ્થિર પૂર્ણાંક plugin_psk_ચાવી_get(સંરચના mosquitto__security_options *opts, સંરચના mosquitto *context, અચલ અક્ષર *hint, અચલ અક્ષર *identity, અક્ષર *ચાવી, પૂર્ણાંક max_ચાવી_len)
{
	સંરચના mosquitto_evt_psk_ચાવી event_data;
	સંરચના mosquitto__callback *cb_base, *cb_next;
	પૂર્ણાંક rc;
	પૂર્ણાંક rc_final = MOSQ_ERR_SUCCESS;

	DL_FOREACH_SAFE(opts->plugin_callbacks.psk_ચાવી, cb_base, cb_next){
		memset(&event_data, 0, કદ(event_data));
		event_data.client = context;
		event_data.hint = hint;
		event_data.identity = identity;
		event_data.ચાવી = ચાવી;
		event_data.max_ચાવી_len = max_ચાવી_len;
		rc = cb_base->cb(MOSQ_EVT_PSK_ચાવી, &event_data, cb_base->userdata);
		જો(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}નહીંતર જો(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}નહીંતર{
			પાછા rc;
		}
	}
	પાછા rc_final;
}


પૂર્ણાંક mosquitto_psk_ચાવી_get(સંરચના mosquitto *context, અચલ અક્ષર *hint, અચલ અક્ષર *identity, અક્ષર *ચાવી, પૂર્ણાંક max_ચાવી_len)
{
	પૂર્ણાંક rc;
	પૂર્ણાંક rc_final = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	જો(db.config->security_options.plugin_callbacks.psk_ચાવી){
		rc = plugin_psk_ચાવી_get(&db.config->security_options, context,
				hint, identity, ચાવી, max_ચાવી_len);

		જો(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}નહીંતર જો(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}નહીંતર{
			પાછા rc;
		}
	}

	/* Per listener plugins */
	જો(context->listener){
		જો(context->listener->security_options->plugin_callbacks.psk_ચાવી){
			rc = plugin_psk_ચાવી_get(context->listener->security_options, context,
					hint, identity, ચાવી, max_ચાવી_len);

			જો(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing */
			}નહીંતર જો(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}નહીંતર{
				પાછા rc;
			}
		}
	}નહીંતર{
		જો(db.config->per_listener_settings){
			પાછા MOSQ_ERR_AUTH;
		}
	}

	rc = mosquitto_psk_ચાવી_get_default(context, hint, identity, ચાવી, max_ચાવી_len);
	જો(rc == MOSQ_ERR_PLUGIN_IGNORE){
		/* Do nothing */
	}નહીંતર જો(rc == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_PLUGIN_DEFER;
	}નહીંતર{
		પાછા rc;
	}


	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured. */
	જો(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_AUTH;
	}
	પાછા rc_final;
}
