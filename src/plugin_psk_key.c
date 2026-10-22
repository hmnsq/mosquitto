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

#include "config.h"

#include <stdio.h>
#include <string.h>

#include "mosquitto/broker.h"
#include "mosquitto_broker_internal.h"
#include "mosquitto/broker_plugin.h"
#include "lib_load.h"
#include "utlist.h"


static цео plugin__psk_key_get(struct mosquitto__security_options *opts, struct mosquitto *context, const char *hint, const char *identity, char *key, цео max_key_len)
{
	struct mosquitto_evt_psk_key event_data;
	struct mosquitto__callback *cb_base, *cb_next;
	цео rc;
	цео rc_final = MOSQ_ERR_SUCCESS;

	DL_FOREACH_SAFE(opts->plugin_callbacks.psk_key, cb_base, cb_next){
		memset(&event_data, 0, sizeof(event_data));
		event_data.client = context;
		event_data.hint = hint;
		event_data.identity = identity;
		event_data.key = key;
		event_data.max_key_len = max_key_len;
		rc = cb_base->cb(MOSQ_EVT_PSK_KEY, &event_data, cb_base->userdata);
		ако(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}инако ако(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}инако{
			врати rc;
		}
	}
	врати rc_final;
}


цео mosquitto_psk_key_get(struct mosquitto *context, const char *hint, const char *identity, char *key, цео max_key_len)
{
	цео rc;
	цео rc_final = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	ако(db.config->security_options.plugin_callbacks.psk_key){
		rc = plugin__psk_key_get(&db.config->security_options, context,
				hint, identity, key, max_key_len);

		ако(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}инако ако(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}инако{
			врати rc;
		}
	}

	/* Per listener plugins */
	ако(context->listener){
		ако(context->listener->security_options->plugin_callbacks.psk_key){
			rc = plugin__psk_key_get(context->listener->security_options, context,
					hint, identity, key, max_key_len);

			ако(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing */
			}инако ако(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}инако{
				врати rc;
			}
		}
	}инако{
		ако(db.config->per_listener_settings){
			врати MOSQ_ERR_AUTH;
		}
	}

	rc = mosquitto_psk_key_get_default(context, hint, identity, key, max_key_len);
	ако(rc == MOSQ_ERR_PLUGIN_IGNORE){
		/* Do nothing */
	}инако ако(rc == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_PLUGIN_DEFER;
	}инако{
		врати rc;
	}


	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured. */
	ако(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_AUTH;
	}
	врати rc_final;
}
