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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "mosquitto/broker.h"
#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto/broker_plugin.h"
#অন্তর্ভাব "lib_load.h"
#অন্তর্ভাব "utlist.h"


স্থির পূর্ণসংখ্যা plugin_psk_চাবি_get(কাঠামো mosquitto__security_options *opts, কাঠামো mosquitto *context, ধ্রুবক অক্ষর *hint, ধ্রুবক অক্ষর *identity, অক্ষর *চাবি, পূর্ণসংখ্যা max_চাবি_len)
{
	কাঠামো mosquitto_evt_psk_চাবি event_তথ্য;
	কাঠামো mosquitto__callback *cb_base, *cb_পরবর্তী;
	পূর্ণসংখ্যা rc;
	পূর্ণসংখ্যা rc_final = MOSQ_ERR_SUCCESS;

	DL_FOREACH_SAFE(opts->plugin_callbacks.psk_চাবি, cb_base, cb_পরবর্তী){
		memset(&event_তথ্য, 0, আকার(event_তথ্য));
		event_তথ্য.client = context;
		event_তথ্য.hint = hint;
		event_তথ্য.identity = identity;
		event_তথ্য.চাবি = চাবি;
		event_তথ্য.max_চাবি_len = max_চাবি_len;
		rc = cb_base->cb(MOSQ_EVT_PSK_চাবি, &event_তথ্য, cb_base->userdata);
		যদি(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}অন্যথা যদি(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}অন্যথা{
			ফেরত rc;
		}
	}
	ফেরত rc_final;
}


পূর্ণসংখ্যা mosquitto_psk_চাবি_get(কাঠামো mosquitto *context, ধ্রুবক অক্ষর *hint, ধ্রুবক অক্ষর *identity, অক্ষর *চাবি, পূর্ণসংখ্যা max_চাবি_len)
{
	পূর্ণসংখ্যা rc;
	পূর্ণসংখ্যা rc_final = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	যদি(db.config->security_options.plugin_callbacks.psk_চাবি){
		rc = plugin_psk_চাবি_get(&db.config->security_options, context,
				hint, identity, চাবি, max_চাবি_len);

		যদি(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}অন্যথা যদি(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}অন্যথা{
			ফেরত rc;
		}
	}

	/* Per listener plugins */
	যদি(context->listener){
		যদি(context->listener->security_options->plugin_callbacks.psk_চাবি){
			rc = plugin_psk_চাবি_get(context->listener->security_options, context,
					hint, identity, চাবি, max_চাবি_len);

			যদি(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing */
			}অন্যথা যদি(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}অন্যথা{
				ফেরত rc;
			}
		}
	}অন্যথা{
		যদি(db.config->per_listener_settings){
			ফেরত MOSQ_ERR_AUTH;
		}
	}

	rc = mosquitto_psk_চাবি_get_default(context, hint, identity, চাবি, max_চাবি_len);
	যদি(rc == MOSQ_ERR_PLUGIN_IGNORE){
		/* Do nothing */
	}অন্যথা যদি(rc == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_PLUGIN_DEFER;
	}অন্যথা{
		ফেরত rc;
	}


	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured. */
	যদি(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_AUTH;
	}
	ফেরত rc_final;
}
