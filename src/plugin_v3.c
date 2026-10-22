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

/* This loads v3 plugins in a v5 wrapper to make the core code cleaner */

#अन्तर्भाव "config.h"

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "mosquitto/broker.h"
#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/broker_plugin.h"
#अन्तर्भाव "lib_load.h"
#अन्तर्भाव "utlist.h"

प्रकारव्याख्या पूर्णांक (*FUNC_auth_plugin_version)(शून्य);
प्रकारव्याख्या पूर्णांक (*FUNC_plugin_version)(पूर्णांक, स्थिरांक पूर्णांक *);


स्थिर पूर्णांक plugin_v3_basic_auth(पूर्णांक event, शून्य *event_data, शून्य *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	संरचना mosquitto_evt_basic_auth *ed = event_data;

	UNUSED(event);

	जर(plugin->lib.unpwd_check_v3 == रिक्त){
		परत MOSQ_ERR_INVAL;
	}

	परत plugin->lib.unpwd_check_v3(
			plugin->lib.user_data,
			ed->client,
			ed->username,
			ed->password);
}


स्थिर पूर्णांक plugin_v3_acl_check(पूर्णांक event, शून्य *event_data, शून्य *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	संरचना mosquitto_evt_acl_check *ed = event_data;
	संरचना mosquitto_acl_संदेश संदेश;
	पूर्णांक rc;

	UNUSED(event);

	जर(plugin->lib.acl_check_v3 == रिक्त){
		परत MOSQ_ERR_INVAL;
	}

	स_रखो(&संदेश, 0, आकार(संदेश));
	संदेश.topic = ed->topic;
	संदेश.payloadlen = ed->payloadlen;
	संदेश.payload = ed->payload;
	संदेश.qos = ed->qos;
	संदेश.retain = ed->retain;

	rc = acl__pre_check(plugin, ed->client, ed->access);
	जर(rc == MOSQ_ERR_PLUGIN_DEFER){
		परत plugin->lib.acl_check_v3(
				plugin->lib.user_data,
				ed->access,
				ed->client,
				&संदेश);
	}अन्यथा{
		परत rc;
	}
}


स्थिर पूर्णांक plugin_v3_psk_किल्ली_get(पूर्णांक event, शून्य *event_data, शून्य *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	संरचना mosquitto_evt_psk_किल्ली *ed = event_data;

	UNUSED(event);

	जर(plugin->lib.psk_किल्ली_get_v3 == रिक्त){
		परत MOSQ_ERR_INVAL;
	}

	परत plugin->lib.psk_किल्ली_get_v3(
			plugin->lib.user_data,
			ed->client,
			ed->hint,
			ed->identity,
			ed->किल्ली,
			ed->max_किल्ली_len);
}


स्थिर पूर्णांक plugin_v3_reload(पूर्णांक event, शून्य *event_data, शून्य *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	पूर्णांक rc;

	UNUSED(event);
	UNUSED(event_data);

	rc = plugin->lib.security_cleanup_v3(
			plugin->lib.user_data,
			plugin->config.options,
			plugin->config.option_मोजणी,
			true);
	जर(rc){
		परत rc;
	}

	rc = plugin->lib.security_init_v3(
			plugin->lib.user_data,
			plugin->config.options,
			plugin->config.option_मोजणी,
			true);
	परत rc;
}


पूर्णांक plugin__load_v3(mosquitto_plugin_id_t *plugin, शून्य *lib)
{
	पूर्णांक rc;

	जर(!(plugin->lib.plugin_init_v3 = (FUNC_auth_plugin_init_v3)LIB_SYM(lib, "mosquitto_auth_plugin_init"))){
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_init().");
		LIB_ERROR();
		परत MOSQ_ERR_UNKNOWN;
	}
	जर(!(plugin->lib.plugin_cleanup_v3 = (FUNC_auth_plugin_cleanup_v3)LIB_SYM(lib, "mosquitto_auth_plugin_cleanup"))){
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_cleanup().");
		LIB_ERROR();
		परत MOSQ_ERR_UNKNOWN;
	}

	जर(!(plugin->lib.security_init_v3 = (FUNC_auth_plugin_security_init_v3)LIB_SYM(lib, "mosquitto_auth_security_init"))){
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_init().");
		LIB_ERROR();
		परत MOSQ_ERR_UNKNOWN;
	}

	जर(!(plugin->lib.security_cleanup_v3 = (FUNC_auth_plugin_security_cleanup_v3)LIB_SYM(lib, "mosquitto_auth_security_cleanup"))){
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_cleanup().");
		LIB_ERROR();
		परत MOSQ_ERR_UNKNOWN;
	}

	जर(!(plugin->lib.acl_check_v3 = (FUNC_auth_plugin_acl_check_v3)LIB_SYM(lib, "mosquitto_auth_acl_check"))){
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_acl_check().");
		LIB_ERROR();
		परत MOSQ_ERR_UNKNOWN;
	}

	जर(!(plugin->lib.unpwd_check_v3 = (FUNC_auth_plugin_unpwd_check_v3)LIB_SYM(lib, "mosquitto_auth_unpwd_check"))){
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_unpwd_check().");
		LIB_ERROR();
		परत MOSQ_ERR_UNKNOWN;
	}

	जर(!(plugin->lib.psk_किल्ली_get_v3 = (FUNC_auth_plugin_psk_किल्ली_get_v3)LIB_SYM(lib, "mosquitto_auth_psk_key_get"))){
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_psk_key_get().");
		LIB_ERROR();
		परत MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = lib;
	plugin->lib.user_data = रिक्त;
	plugin->lib.identifier = plugin;
	जर(plugin->lib.plugin_init_v3){
		rc = plugin->lib.plugin_init_v3(&plugin->lib.user_data, plugin->config.options, plugin->config.option_मोजणी);
		जर(rc){
			log__printf(रिक्त, MOSQ_LOG_ERR,
					"Error: Authentication plugin returned %d when initialising.", rc);
			परत rc;
		}
	}

	mosquitto_callback_register(plugin, MOSQ_EVT_RELOAD, plugin_v3_reload, रिक्त, plugin);

	जर(plugin->lib.unpwd_check_v3){
		mosquitto_callback_register(plugin, MOSQ_EVT_BASIC_AUTH, plugin_v3_basic_auth, रिक्त, plugin);
	}
	जर(plugin->lib.acl_check_v3){
		mosquitto_callback_register(plugin, MOSQ_EVT_ACL_CHECK, plugin_v3_acl_check, रिक्त, plugin);
	}
	जर(plugin->lib.psk_किल्ली_get_v3){
		mosquitto_callback_register(plugin, MOSQ_EVT_PSK_किल्ली, plugin_v3_psk_किल्ली_get, रिक्त, plugin);
	}

	परत 0;
}


