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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "mosquitto/broker.h"
#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/broker_plugin.h"
#ಅನ್ತರ್ಭಾವ "lib_load.h"
#ಅನ್ತರ್ಭಾವ "utlist.h"


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ plugin_ext_auth_ಆರಂಭ(ರಚನೆ mosquitto__security_options *opts, ರಚನೆ mosquitto *context, bool reauth, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ_ಒಳ, uint16_t ದತ್ತಾಂಶ_ಒಳ_len, ಶೂನ್ಯ **ದತ್ತಾಂಶ_ಹೊರಹೋಗು, uint16_t *ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len)
{
	ರಚನೆ mosquitto_evt_extended_auth event_ದತ್ತಾಂಶ;
	ರಚನೆ mosquitto__callback *cb_ತಳ, *cb_ಮುಂದಿನ;
	ಪೂರ್ಣಾಂಕ rc;
	ಪೂರ್ಣಾಂಕ rc_final = MOSQ_ERR_PLUGIN_DEFER;

	ಬಳಸದ(reauth);

	DL_FOREACH_ಸುರಕ್ಷಿತ(opts->plugin_callbacks.ext_auth_ಆರಂಭ, cb_ತಳ, cb_ಮುಂದಿನ){
		memset(&event_ದತ್ತಾಂಶ, 0, ಗಾತ್ರ(event_ದತ್ತಾಂಶ));
		event_ದತ್ತಾಂಶ.client = context;
		event_ದತ್ತಾಂಶ.auth_ವಿಧಾನ = context->auth_ವಿಧಾನ;
		event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ_ಒಳ = ದತ್ತಾಂಶ_ಒಳ;
		event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ_ಹೊರಹೋಗು = ಖಾಲಿ;
		event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ_ಒಳ_len = ದತ್ತಾಂಶ_ಒಳ_len;
		event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len = 0;
		rc = cb_ತಳ->cb(MOSQ_EVT_EXT_AUTH_ಆರಂಭ, &event_ದತ್ತಾಂಶ, cb_ತಳ->userdata);
		ಆದರೆ(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}ಇಲ್ಲದಿದ್ದರೆ{
			*ದತ್ತಾಂಶ_ಹೊರಹೋಗು = event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ_ಹೊರಹೋಗು;
			*ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len = event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len;
			ಹಿಂತಿರುಗಿ rc;
		}
	}
	ಹಿಂತಿರುಗಿ rc_final;
}


ಪೂರ್ಣಾಂಕ mosquitto_security_auth_ಆರಂಭ(ರಚನೆ mosquitto *context, bool reauth, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ_ಒಳ, uint16_t ದತ್ತಾಂಶ_ಒಳ_len, ಶೂನ್ಯ **ದತ್ತಾಂಶ_ಹೊರಹೋಗು, uint16_t *ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len)
{
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ(!context || !context->listener || !context->auth_ವಿಧಾನ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}
	ಆದರೆ(!ದತ್ತಾಂಶ_ಹೊರಹೋಗು || !ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	ಆದರೆ(db.config->security_options.plugin_callbacks.ext_auth_ಆರಂಭ){
		rc = plugin_ext_auth_ಆರಂಭ(&db.config->security_options, context,
				reauth, ದತ್ತಾಂಶ_ಒಳ, ದತ್ತಾಂಶ_ಒಳ_len, ದತ್ತಾಂಶ_ಹೊರಹೋಗು, ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len);

		ಆದರೆ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಹಿಂತಿರುಗಿ rc;
		}
	}

	/* Per listener plugins */
	ಆದರೆ(context->listener){
		ಆದರೆ(context->listener->security_options->plugin_callbacks.ext_auth_ಆರಂಭ){
			rc = plugin_ext_auth_ಆರಂಭ(context->listener->security_options, context,
					reauth, ದತ್ತಾಂಶ_ಒಳ, ದತ್ತಾಂಶ_ಒಳ_len, ದತ್ತಾಂಶ_ಹೊರಹೋಗು, ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len);

			ಆದರೆ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}ಇಲ್ಲದಿದ್ದರೆ{
				ಹಿಂತಿರುಗಿ rc;
			}
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಆದರೆ(db.config->ಪ್ರತಿ_listener_settings){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_AUTH;
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_SUPPORTED;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ plugin__ext_auth_continue(ರಚನೆ mosquitto__security_options *opts, ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ_ಒಳ, uint16_t ದತ್ತಾಂಶ_ಒಳ_len, ಶೂನ್ಯ **ದತ್ತಾಂಶ_ಹೊರಹೋಗು, uint16_t *ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len)
{
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ mosquitto_evt_extended_auth event_ದತ್ತಾಂಶ;
	ರಚನೆ mosquitto__callback *cb_ತಳ, *cb_ಮುಂದಿನ;

	DL_FOREACH_ಸುರಕ್ಷಿತ(opts->plugin_callbacks.ext_auth_continue, cb_ತಳ, cb_ಮುಂದಿನ){
		memset(&event_ದತ್ತಾಂಶ, 0, ಗಾತ್ರ(event_ದತ್ತಾಂಶ));
		event_ದತ್ತಾಂಶ.client = context;
		event_ದತ್ತಾಂಶ.auth_ವಿಧಾನ = context->auth_ವಿಧಾನ;
		event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ_ಒಳ = ದತ್ತಾಂಶ_ಒಳ;
		event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ_ಹೊರಹೋಗು = ಖಾಲಿ;
		event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ_ಒಳ_len = ದತ್ತಾಂಶ_ಒಳ_len;
		event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len = 0;
		rc = cb_ತಳ->cb(MOSQ_EVT_EXT_AUTH_CONTINUE, &event_ದತ್ತಾಂಶ, cb_ತಳ->userdata);
		ಆದರೆ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}ಇಲ್ಲದಿದ್ದರೆ{
			*ದತ್ತಾಂಶ_ಹೊರಹೋಗು = event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ_ಹೊರಹೋಗು;
			*ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len = event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len;
			ಹಿಂತಿರುಗಿ rc;
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_PLUGIN_DEFER;
}


ಪೂರ್ಣಾಂಕ mosquitto_security_auth_continue(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ_ಒಳ, uint16_t ದತ್ತಾಂಶ_ಒಳ_len, ಶೂನ್ಯ **ದತ್ತಾಂಶ_ಹೊರಹೋಗು, uint16_t *ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len)
{
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ(!context || !context->listener || !context->auth_ವಿಧಾನ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}
	ಆದರೆ(!ದತ್ತಾಂಶ_ಹೊರಹೋಗು || !ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	ಆದರೆ(db.config->security_options.plugin_callbacks.ext_auth_continue){
		rc = plugin__ext_auth_continue(&db.config->security_options, context,
				ದತ್ತಾಂಶ_ಒಳ, ದತ್ತಾಂಶ_ಒಳ_len, ದತ್ತಾಂಶ_ಹೊರಹೋಗು, ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len);

		ಆದರೆ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಹಿಂತಿರುಗಿ rc;
		}
	}

	/* Per listener plugins */
	ಆದರೆ(context->listener){
		ಆದರೆ(context->listener->security_options->plugin_callbacks.ext_auth_continue){
			rc = plugin__ext_auth_continue(context->listener->security_options, context,
					ದತ್ತಾಂಶ_ಒಳ, ದತ್ತಾಂಶ_ಒಳ_len, ದತ್ತಾಂಶ_ಹೊರಹೋಗು, ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len);

			ಆದರೆ(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}ಇಲ್ಲದಿದ್ದರೆ{
				ಹಿಂತಿರುಗಿ rc;
			}
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಆದರೆ(db.config->ಪ್ರತಿ_listener_settings){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_AUTH;
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_SUPPORTED;
}
