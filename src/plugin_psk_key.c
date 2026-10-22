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

#अन्तर्भाव "config.h"

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "mosquitto/broker.h"
#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/broker_plugin.h"
#अन्तर्भाव "lib_load.h"
#अन्तर्भाव "utlist.h"


स्थिर पूर्णाङ्क plugin_psk_साँचो_get(संरचना mosquitto__security_options *opts, संरचना mosquitto *context, अचर वर्ण *hint, अचर वर्ण *identity, वर्ण *साँचो, पूर्णाङ्क max_साँचो_len)
{
	संरचना mosquitto_evt_psk_साँचो event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	पूर्णाङ्क rc;
	पूर्णाङ्क rc_final = MOSQ_ERR_SUCCESS;

	DL_FOREACH_SAFE(opts->plugin_callbacks.psk_साँचो, cb_base, cb_next){
		स_रखो(&event_data, 0, परिमाण(event_data));
		event_data.client = context;
		event_data.hint = hint;
		event_data.identity = identity;
		event_data.साँचो = साँचो;
		event_data.max_साँचो_len = max_साँचो_len;
		rc = cb_base->cb(MOSQ_EVT_PSK_साँचो, &event_data, cb_base->userdata);
		यदि(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}अन्यथा यदि(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}अन्यथा{
			फर्काऊ rc;
		}
	}
	फर्काऊ rc_final;
}


पूर्णाङ्क mosquitto_psk_साँचो_get(संरचना mosquitto *context, अचर वर्ण *hint, अचर वर्ण *identity, वर्ण *साँचो, पूर्णाङ्क max_साँचो_len)
{
	पूर्णाङ्क rc;
	पूर्णाङ्क rc_final = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	यदि(db.config->security_options.plugin_callbacks.psk_साँचो){
		rc = plugin_psk_साँचो_get(&db.config->security_options, context,
				hint, identity, साँचो, max_साँचो_len);

		यदि(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}अन्यथा यदि(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}अन्यथा{
			फर्काऊ rc;
		}
	}

	/* Per listener plugins */
	यदि(context->listener){
		यदि(context->listener->security_options->plugin_callbacks.psk_साँचो){
			rc = plugin_psk_साँचो_get(context->listener->security_options, context,
					hint, identity, साँचो, max_साँचो_len);

			यदि(rc == MOSQ_ERR_PLUGIN_IGNORE){
				/* Do nothing */
			}अन्यथा यदि(rc == MOSQ_ERR_PLUGIN_DEFER){
				rc_final = MOSQ_ERR_PLUGIN_DEFER;
			}अन्यथा{
				फर्काऊ rc;
			}
		}
	}अन्यथा{
		यदि(db.config->per_listener_settings){
			फर्काऊ MOSQ_ERR_AUTH;
		}
	}

	rc = mosquitto_psk_साँचो_get_default(context, hint, identity, साँचो, max_साँचो_len);
	यदि(rc == MOSQ_ERR_PLUGIN_IGNORE){
		/* Do nothing */
	}अन्यथा यदि(rc == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_PLUGIN_DEFER;
	}अन्यथा{
		फर्काऊ rc;
	}


	/* If all plugins deferred, this is a denial. If rc == MOSQ_ERR_SUCCESS
	 * here, then no plugins were configured. */
	यदि(rc_final == MOSQ_ERR_PLUGIN_DEFER){
		rc_final = MOSQ_ERR_AUTH;
	}
	फर्काऊ rc_final;
}
