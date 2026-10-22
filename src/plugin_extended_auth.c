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

#समावेश "config.h"

#समावेश <stdio.h>
#समावेश <string.h>

#समावेश "mosquitto/broker.h"
#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/broker_plugin.h"
#समावेश "lib_load.h"
#समावेश "utlist.h"


अटल पूर्णांक plugin__ext_auth_start(काष्ठा mosquitto__security_options *opts, काष्ठा mosquitto *context, bool reauth, स्थिर व्योम *तथ्य_in, uint16_t तथ्य_in_len, व्योम **तथ्य_out, uint16_t *तथ्य_out_len)
{
	काष्ठा mosquitto_evt_extended_auth event_तथ्य;
	काष्ठा mosquitto__callback *cb_base, *cb_अग्र;
	पूर्णांक rc;
	पूर्णांक rc_final = MOSQ_ERR_PLUGIN_DEFER;

	UNUSED(reauth);

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_start, cb_base, cb_अग्र){
		स_रखो(&event_तथ्य, 0, माप(event_तथ्य));
		event_तथ्य.client = context;
		event_तथ्य.auth_method = context->auth_method;
		event_तथ्य.तथ्य_in = तथ्य_in;
		event_तथ्य.तथ्य_out = शून्य;
		event_तथ्य.तथ्य_in_len = तथ्य_in_len;
		event_तथ्य.तथ्य_out_len = 0;
		rc = cb_base->cb(MOSQ_EVT_EXT_AUTH_START, &event_तथ्य, cb_base->userdata);
		अगर(rc == MOSQ_ERR_PLUGIN_IGNORE){
			/* Do nothing */
		}अन्यथा अगर(rc == MOSQ_ERR_PLUGIN_DEFER){
			rc_final = MOSQ_ERR_PLUGIN_DEFER;
		}अन्यथा{
			*तथ्य_out = event_तथ्य.तथ्य_out;
			*तथ्य_out_len = event_तथ्य.तथ्य_out_len;
			वापस rc;
		}
	}
	वापस rc_final;
}


पूर्णांक mosquitto_security_auth_start(काष्ठा mosquitto *context, bool reauth, स्थिर व्योम *तथ्य_in, uint16_t तथ्य_in_len, व्योम **तथ्य_out, uint16_t *तथ्य_out_len)
{
	पूर्णांक rc;

	अगर(!context || !context->listener || !context->auth_method){
		वापस MOSQ_ERR_INVAL;
	}
	अगर(!तथ्य_out || !तथ्य_out_len){
		वापस MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	अगर(db.config->security_options.plugin_callbacks.ext_auth_start){
		rc = plugin__ext_auth_start(&db.config->security_options, context,
				reauth, तथ्य_in, तथ्य_in_len, तथ्य_out, तथ्य_out_len);

		अगर(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}अन्यथा{
			वापस rc;
		}
	}

	/* Per listener plugins */
	अगर(context->listener){
		अगर(context->listener->security_options->plugin_callbacks.ext_auth_start){
			rc = plugin__ext_auth_start(context->listener->security_options, context,
					reauth, तथ्य_in, तथ्य_in_len, तथ्य_out, तथ्य_out_len);

			अगर(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}अन्यथा{
				वापस rc;
			}
		}
	}अन्यथा{
		अगर(db.config->per_listener_settings){
			वापस MOSQ_ERR_AUTH;
		}
	}

	वापस MOSQ_ERR_NOT_SUPPORTED;
}


अटल पूर्णांक plugin__ext_auth_continue(काष्ठा mosquitto__security_options *opts, काष्ठा mosquitto *context, स्थिर व्योम *तथ्य_in, uint16_t तथ्य_in_len, व्योम **तथ्य_out, uint16_t *तथ्य_out_len)
{
	पूर्णांक rc;
	काष्ठा mosquitto_evt_extended_auth event_तथ्य;
	काष्ठा mosquitto__callback *cb_base, *cb_अग्र;

	DL_FOREACH_SAFE(opts->plugin_callbacks.ext_auth_continue, cb_base, cb_अग्र){
		स_रखो(&event_तथ्य, 0, माप(event_तथ्य));
		event_तथ्य.client = context;
		event_तथ्य.auth_method = context->auth_method;
		event_तथ्य.तथ्य_in = तथ्य_in;
		event_तथ्य.तथ्य_out = शून्य;
		event_तथ्य.तथ्य_in_len = तथ्य_in_len;
		event_तथ्य.तथ्य_out_len = 0;
		rc = cb_base->cb(MOSQ_EVT_EXT_AUTH_CONTINUE, &event_तथ्य, cb_base->userdata);
		अगर(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}अन्यथा{
			*तथ्य_out = event_तथ्य.तथ्य_out;
			*तथ्य_out_len = event_तथ्य.तथ्य_out_len;
			वापस rc;
		}
	}

	वापस MOSQ_ERR_PLUGIN_DEFER;
}


पूर्णांक mosquitto_security_auth_continue(काष्ठा mosquitto *context, स्थिर व्योम *तथ्य_in, uint16_t तथ्य_in_len, व्योम **तथ्य_out, uint16_t *तथ्य_out_len)
{
	पूर्णांक rc;

	अगर(!context || !context->listener || !context->auth_method){
		वापस MOSQ_ERR_INVAL;
	}
	अगर(!तथ्य_out || !तथ्य_out_len){
		वापस MOSQ_ERR_INVAL;
	}

	/* Global plugins */
	अगर(db.config->security_options.plugin_callbacks.ext_auth_continue){
		rc = plugin__ext_auth_continue(&db.config->security_options, context,
				तथ्य_in, तथ्य_in_len, तथ्य_out, तथ्य_out_len);

		अगर(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
			/* Do nothing */
		}अन्यथा{
			वापस rc;
		}
	}

	/* Per listener plugins */
	अगर(context->listener){
		अगर(context->listener->security_options->plugin_callbacks.ext_auth_continue){
			rc = plugin__ext_auth_continue(context->listener->security_options, context,
					तथ्य_in, तथ्य_in_len, तथ्य_out, तथ्य_out_len);

			अगर(rc == MOSQ_ERR_PLUGIN_IGNORE || rc == MOSQ_ERR_PLUGIN_DEFER){
				/* Do nothing */
			}अन्यथा{
				वापस rc;
			}
		}
	}अन्यथा{
		अगर(db.config->per_listener_settings){
			वापस MOSQ_ERR_AUTH;
		}
	}

	वापस MOSQ_ERR_NOT_SUPPORTED;
}
