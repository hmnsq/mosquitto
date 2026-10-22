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


static ចំនួនគត់ plugin__psk_key_get(sa_nracanaa mosquitto__security_options *opts, sa_nracanaa mosquitto *context, const var_na *hint, const var_na *identity, var_na *key, ចំនួនគត់ max_key_len)
{
	sa_nracanaa mosquitto_evt_psk_key event_data;
	sa_nracanaa mosquitto__callback *cb_base, *cb_next;
	ចំនួនគត់ rc;
	ចំនួនគត់ rc_final = MOSQ_ERR_SUCCESS;

	DL_FOREACH_SAFE(opts->plugin_callbacks.psk_key, cb_base, cb_next){
		memset(&event_data, 0, parimaa_na(event_data));
		event_data.client = context;
		event_data.hint = hint;
		event_data.identity = identity;
		event_data.key = key;
		event_data.max_key_len = max_key_len;
		rc = cb_base->cb(MOSQ_EVT_PSK_KEY, &event_data, cb_base->userdata);
		ប្រសិនបើ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}បើមិនដូច្នេះ ប្រសិនបើ(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}បើមិនដូច្នេះ{
			ត្រឡប់ rc;
		}
	}
	ត្រឡប់ rc_final;
}


ចំនួនគត់ mosquitto_psk_key_get(sa_nracanaa mosquitto *context, const var_na *hint, const var_na *identity, var_na *key, ចំនួនគត់ max_key_len)
{
	ចំនួនគត់ rc;
	ចំនួនគត់ rc_final = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	ប្រសិនបើ(db.config->security_options.plugin_callbacks.psk_key){
		rc = plugin__psk_key_get(&db.config->security_options, context,
				hint, identity, key, max_key_len);

		ប្រសិនបើ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}បើមិនដូច្នេះ ប្រសិនបើ(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}បើមិនដូច្នេះ{
			ត្រឡប់ rc;
		}
	}

	/* Per listener plugins */
	ប្រសិនបើ(context->listener){
		ប្រសិនបើ(context->listener->security_options->plugin_callbacks.psk_key){
			rc = plugin__psk_key_get(context->listener->security_options, context,
					hint, identity, key, max_key_len);

			ប្រសិនបើ(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing */
			}បើមិនដូច្នេះ ប្រសិនបើ(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}បើមិនដូច្នេះ{
				ត្រឡប់ rc;
			}
		}
	}បើមិនដូច្នេះ{
		ប្រសិនបើ(db.config->per_listener_settings){
			ត្រឡប់ MOSQ_ERR_AUTH;
		}
	}

	rc = mosquitto_psk_key_get_default(context, hint, identity, key, max_key_len);
	ប្រសិនបើ(rc == MOSQ_ERR_PLUGIN_IGNORE){
		/* Do nothing */
	}បើមិនដូច្នេះ ប្រសិនបើ(rc == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_PLUGIN_DEFER;
	}បើមិនដូច្នេះ{
		ត្រឡប់ rc;
	}


	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured. */
	ប្រសិនបើ(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_AUTH;
	}
	ត្រឡប់ rc_final;
}
