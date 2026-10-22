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

/* This loads v2 plugins in a v5 wrapper to make the core code cleaner */

#अन्तर्भाव "config.h"

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "mosquitto/broker.h"
#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/broker_plugin.h"
#अन्तर्भाव "lib_load.h"
#अन्तर्भाव "utlist.h"

प्रकारनिर्देश पूर्णाङ्क (*FUNC_auth_plugin_version)(शून्य);
प्रकारनिर्देश पूर्णाङ्क (*FUNC_plugin_version)(पूर्णाङ्क, अचर पूर्णाङ्क *);


स्थिर पूर्णाङ्क plugin_v2_basic_auth(पूर्णाङ्क event, शून्य *event_data, शून्य *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	संरचना mosquitto_evt_basic_auth *ed = event_data;

	UNUSED(event);

	यदि(plugin->lib.unpwd_check_v2 == रिक्त){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	फर्काऊ plugin->lib.unpwd_check_v2(
			plugin->lib.user_data,
			ed->username,
			ed->password);
}


स्थिर पूर्णाङ्क plugin_v2_acl_check(पूर्णाङ्क event, शून्य *event_data, शून्य *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	संरचना mosquitto_evt_acl_check *ed = event_data;
	पूर्णाङ्क rc;

	UNUSED(event);

	यदि(plugin->lib.acl_check_v2 == रिक्त){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	rc = acl__pre_check(plugin, ed->client, ed->access);
	यदि(rc == MOSQ_ERR_PLUGIN_DEFER){
		फर्काऊ plugin->lib.acl_check_v2(
				plugin->lib.user_data,
				ed->client->id,
				ed->client->username,
				ed->topic,
				ed->access);
	}अन्यथा{
		फर्काऊ rc;
	}
}


स्थिर पूर्णाङ्क plugin_v2_psk_साँचो_get(पूर्णाङ्क event, शून्य *event_data, शून्य *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	संरचना mosquitto_evt_psk_साँचो *ed = event_data;

	UNUSED(event);

	यदि(plugin->lib.psk_साँचो_get_v2 == रिक्त){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	फर्काऊ plugin->lib.psk_साँचो_get_v2(
			plugin->lib.user_data,
			ed->hint,
			ed->identity,
			ed->साँचो,
			ed->max_साँचो_len);
}


स्थिर पूर्णाङ्क plugin_v2_reload(पूर्णाङ्क event, शून्य *event_data, शून्य *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	पूर्णाङ्क rc;

	UNUSED(event);
	UNUSED(event_data);

	rc = plugin->lib.security_cleanup_v2(
			plugin->lib.user_data,
			(संरचना mosquitto_auth_opt *)plugin->config.options,
			plugin->config.option_गन्ती,
			true);
	यदि(rc){
		फर्काऊ rc;
	}

	rc = plugin->lib.security_init_v2(
			plugin->lib.user_data,
			(संरचना mosquitto_auth_opt *)plugin->config.options,
			plugin->config.option_गन्ती,
			true);
	फर्काऊ rc;
}


पूर्णाङ्क plugin__load_v2(mosquitto_plugin_id_t *plugin, शून्य *lib)
{
	पूर्णाङ्क rc;

	यदि(!(plugin->lib.plugin_init_v2 = (FUNC_auth_plugin_init_v2)LIB_SYM(lib, "mosquitto_auth_plugin_init"))){
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_init().");
		LIB_ERROR();
		फर्काऊ MOSQ_ERR_UNKNOWN;
	}
	यदि(!(plugin->lib.plugin_cleanup_v2 = (FUNC_auth_plugin_cleanup_v2)LIB_SYM(lib, "mosquitto_auth_plugin_cleanup"))){
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_cleanup().");
		LIB_ERROR();
		फर्काऊ MOSQ_ERR_UNKNOWN;
	}

	यदि(!(plugin->lib.security_init_v2 = (FUNC_auth_plugin_security_init_v2)LIB_SYM(lib, "mosquitto_auth_security_init"))){
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_init().");
		LIB_ERROR();
		फर्काऊ MOSQ_ERR_UNKNOWN;
	}

	यदि(!(plugin->lib.security_cleanup_v2 = (FUNC_auth_plugin_security_cleanup_v2)LIB_SYM(lib, "mosquitto_auth_security_cleanup"))){
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_cleanup().");
		LIB_ERROR();
		फर्काऊ MOSQ_ERR_UNKNOWN;
	}

	यदि(!(plugin->lib.acl_check_v2 = (FUNC_auth_plugin_acl_check_v2)LIB_SYM(lib, "mosquitto_auth_acl_check"))){
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_acl_check().");
		LIB_ERROR();
		फर्काऊ MOSQ_ERR_UNKNOWN;
	}

	यदि(!(plugin->lib.unpwd_check_v2 = (FUNC_auth_plugin_unpwd_check_v2)LIB_SYM(lib, "mosquitto_auth_unpwd_check"))){
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_unpwd_check().");
		LIB_ERROR();
		फर्काऊ MOSQ_ERR_UNKNOWN;
	}

	यदि(!(plugin->lib.psk_साँचो_get_v2 = (FUNC_auth_plugin_psk_साँचो_get_v2)LIB_SYM(lib, "mosquitto_auth_psk_key_get"))){
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_psk_key_get().");
		LIB_ERROR();
		फर्काऊ MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = lib;
	plugin->lib.user_data = रिक्त;
	plugin->lib.identifier = plugin;

	यदि(plugin->lib.plugin_init_v2){
		rc = plugin->lib.plugin_init_v2(
				&plugin->lib.user_data,
				(संरचना mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_गन्ती);
		यदि(rc){
			log__printf(रिक्त, MOSQ_LOG_ERR,
					"Error: Authentication plugin returned %d when initialising.", rc);
			फर्काऊ rc;
		}
	}

	mosquitto_callback_register(plugin, MOSQ_EVT_RELOAD, plugin_v2_reload, रिक्त, plugin);

	यदि(plugin->lib.unpwd_check_v2){
		mosquitto_callback_register(plugin, MOSQ_EVT_BASIC_AUTH, plugin_v2_basic_auth, रिक्त, plugin);
	}
	यदि(plugin->lib.acl_check_v2){
		mosquitto_callback_register(plugin, MOSQ_EVT_ACL_CHECK, plugin_v2_acl_check, रिक्त, plugin);
	}
	यदि(plugin->lib.psk_साँचो_get_v2){
		mosquitto_callback_register(plugin, MOSQ_EVT_PSK_साँचो, plugin_v2_psk_साँचो_get, रिक्त, plugin);
	}

	फर्काऊ 0;
}
