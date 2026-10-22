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

/* This loads v4 plugins in a v5 wrapper to make the core code cleaner */

#अन्तर्भाव "config.h"

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "mosquitto/broker.h"
#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/broker_plugin.h"
#अन्तर्भाव "lib_load.h"
#अन्तर्भाव "utlist.h"

प्रकारनिर्देश पूर्णाङ्क (*FUNC_auth_plugin_संस्करणम्)(शून्य);
प्रकारनिर्देश पूर्णाङ्क (*FUNC_plugin_संस्करणम्)(पूर्णाङ्क, ध्रुव पूर्णाङ्क *);


स्थिर पूर्णाङ्क plugin_v4_basic_auth(पूर्णाङ्क event, शून्य *event_दत्त, शून्य *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	संरचना mosquitto_evt_basic_auth *ed = event_दत्त;

	अप्रयुक्तम्(event);

	यदि(plugin->lib.unpwd_check_v4 == रिक्त){
		प्रत्यागम MOSQ_ERR_PLUGIN_DEFER;
	}

	प्रत्यागम plugin->lib.unpwd_check_v4(
			plugin->lib.user_दत्त,
			ed->client,
			ed->username,
			ed->password);
}


स्थिर पूर्णाङ्क plugin_v4_acl_check(पूर्णाङ्क event, शून्य *event_दत्त, शून्य *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	संरचना mosquitto_evt_acl_check *ed = event_दत्त;
	संरचना mosquitto_acl_सन्देशः सन्देशः;
	पूर्णाङ्क rc;

	अप्रयुक्तम्(event);

	यदि(plugin->lib.acl_check_v4 == रिक्त){
		प्रत्यागम MOSQ_ERR_PLUGIN_DEFER;
	}

	स्मृतिं_पूरय(&सन्देशः, 0, परिमाण(सन्देशः));
	सन्देशः.topic = ed->topic;
	सन्देशः.payloadlen = ed->payloadlen;
	सन्देशः.payload = ed->payload;
	सन्देशः.qos = ed->qos;
	सन्देशः.retain = ed->retain;

	rc = acl__pre_check(plugin, ed->client, ed->access);
	यदि(rc == MOSQ_ERR_PLUGIN_DEFER){
		प्रत्यागम plugin->lib.acl_check_v4(
				plugin->lib.user_दत्त,
				ed->access,
				ed->client,
				&सन्देशः);
	}अन्यथा{
		प्रत्यागम rc;
	}
}


स्थिर पूर्णाङ्क plugin_v4_auth_start(पूर्णाङ्क event, शून्य *event_दत्त, शून्य *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	संरचना mosquitto_evt_extended_auth *ed = event_दत्त;

	अप्रयुक्तम्(event);

	यदि(plugin->lib.auth_start_v4 == रिक्त){
		प्रत्यागम MOSQ_ERR_PLUGIN_DEFER;
	}

	प्रत्यागम plugin->lib.auth_start_v4(
			plugin->lib.user_दत्त,
			ed->client,
			ed->client->auth_विधानम्,
			false,
			ed->दत्त_अन्तर्, ed->दत्त_अन्तर्_len,
			&ed->दत्त_बहिर्गमनम्, &ed->दत्त_बहिर्गमनम्_len);
}


स्थिर पूर्णाङ्क plugin_v4_auth_continue(पूर्णाङ्क event, शून्य *event_दत्त, शून्य *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	संरचना mosquitto_evt_extended_auth *ed = event_दत्त;

	अप्रयुक्तम्(event);

	यदि(plugin->lib.auth_continue_v4 == रिक्त){
		प्रत्यागम MOSQ_ERR_PLUGIN_DEFER;
	}

	प्रत्यागम plugin->lib.auth_continue_v4(
			plugin->lib.user_दत्त,
			ed->client,
			ed->client->auth_विधानम्,
			ed->दत्त_अन्तर्, ed->दत्त_अन्तर्_len,
			&ed->दत्त_बहिर्गमनम्, &ed->दत्त_बहिर्गमनम्_len);
}


स्थिर पूर्णाङ्क plugin_v4_psk_कुञ्चिका_get(पूर्णाङ्क event, शून्य *event_दत्त, शून्य *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	संरचना mosquitto_evt_psk_कुञ्चिका *ed = event_दत्त;

	अप्रयुक्तम्(event);

	यदि(plugin->lib.psk_कुञ्चिका_get_v4 == रिक्त){
		प्रत्यागम MOSQ_ERR_PLUGIN_DEFER;
	}

	प्रत्यागम plugin->lib.psk_कुञ्चिका_get_v4(
			plugin->lib.user_दत्त,
			ed->client,
			ed->hint,
			ed->तटस्थः,
			ed->कुञ्चिका,
			ed->max_कुञ्चिका_len);
}


स्थिर पूर्णाङ्क plugin_v4_reload(पूर्णाङ्क event, शून्य *event_दत्त, शून्य *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	पूर्णाङ्क rc;

	अप्रयुक्तम्(event);
	अप्रयुक्तम्(event_दत्त);

	rc = plugin->lib.security_cleanup_v4(
			plugin->lib.user_दत्त,
			plugin->config.options,
			plugin->config.option_गणनम्,
			true);
	यदि(rc){
		प्रत्यागम rc;
	}

	rc = plugin->lib.security_init_v4(
			plugin->lib.user_दत्त,
			plugin->config.options,
			plugin->config.option_गणनम्,
			true);
	प्रत्यागम rc;
}


पूर्णाङ्क plugin_आरोपय_v4(mosquitto_plugin_id_t *plugin, शून्य *lib)
{
	पूर्णाङ्क rc;

	यदि(!(plugin->lib.plugin_init_v4 = (FUNC_auth_plugin_init_v4)LIB_SYM(lib, "mosquitto_auth_plugin_init"))){
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_init().");
		LIB_ERROR();
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}
	यदि(!(plugin->lib.plugin_cleanup_v4 = (FUNC_auth_plugin_cleanup_v4)LIB_SYM(lib, "mosquitto_auth_plugin_cleanup"))){
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_cleanup().");
		LIB_ERROR();
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}

	यदि(!(plugin->lib.security_init_v4 = (FUNC_auth_plugin_security_init_v4)LIB_SYM(lib, "mosquitto_auth_security_init"))){
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_init().");
		LIB_ERROR();
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}

	यदि(!(plugin->lib.security_cleanup_v4 = (FUNC_auth_plugin_security_cleanup_v4)LIB_SYM(lib, "mosquitto_auth_security_cleanup"))){
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_cleanup().");
		LIB_ERROR();
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}

	यदि(!(plugin->lib.acl_check_v4 = (FUNC_auth_plugin_acl_check_v4)LIB_SYM(lib, "mosquitto_auth_acl_check"))){
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_acl_check().");
		LIB_ERROR();
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.unpwd_check_v4 = (FUNC_auth_plugin_unpwd_check_v4)LIB_SYM(lib, "mosquitto_auth_unpwd_check");
	यदि(plugin->lib.unpwd_check_v4){
		log__printf(रिक्त, MOSQ_LOG_INFO,
				" ├── Username/password checking enabled.");
	}अन्यथा{
		log__printf(रिक्त, MOSQ_LOG_INFO,
				" ├── Username/password checking not enabled.");
	}

	plugin->lib.psk_कुञ्चिका_get_v4 = (FUNC_auth_plugin_psk_कुञ्चिका_get_v4)LIB_SYM(lib, "mosquitto_auth_psk_key_get");
	यदि(plugin->lib.psk_कुञ्चिका_get_v4){
		log__printf(रिक्त, MOSQ_LOG_INFO,
				" ├── TLS-PSK checking enabled.");
	}अन्यथा{
		log__printf(रिक्त, MOSQ_LOG_INFO,
				" ├── TLS-PSK checking not enabled.");
	}

	plugin->lib.auth_start_v4 = (FUNC_auth_plugin_auth_start_v4)LIB_SYM(lib, "mosquitto_auth_start");
	plugin->lib.auth_continue_v4 = (FUNC_auth_plugin_auth_continue_v4)LIB_SYM(lib, "mosquitto_auth_continue");

	यदि(plugin->lib.auth_start_v4){
		यदि(plugin->lib.auth_continue_v4){
			log__printf(रिक्त, MOSQ_LOG_INFO,
					" └── Extended authentication enabled.");
		}अन्यथा{
			log__printf(रिक्त, MOSQ_LOG_ERR,
					"Error: Plugin has missing mosquitto_auth_continue() function.");
			प्रत्यागम MOSQ_ERR_UNKNOWN;
		}
	}अन्यथा{
		log__printf(रिक्त, MOSQ_LOG_INFO,
				" └── Extended authentication not enabled.");
	}

	plugin->lib.lib = lib;
	plugin->lib.user_दत्त = रिक्त;
	plugin->lib.identifier = plugin;

	यदि(plugin->lib.plugin_init_v4){
		rc = plugin->lib.plugin_init_v4(&plugin->lib.user_दत्त, plugin->config.options, plugin->config.option_गणनम्);
		यदि(rc){
			log__printf(रिक्त, MOSQ_LOG_ERR,
					"Error: Authentication plugin returned %d when initialising.", rc);
			प्रत्यागम rc;
		}
	}

	mosquitto_callback_पञ्जिकाकोशः(plugin, MOSQ_EVT_RELOAD, plugin_v4_reload, रिक्त, plugin);

	यदि(plugin->lib.unpwd_check_v4){
		mosquitto_callback_पञ्जिकाकोशः(plugin, MOSQ_EVT_BASIC_AUTH, plugin_v4_basic_auth, रिक्त, plugin);
	}
	यदि(plugin->lib.acl_check_v4){
		mosquitto_callback_पञ्जिकाकोशः(plugin, MOSQ_EVT_ACL_CHECK, plugin_v4_acl_check, रिक्त, plugin);
	}
	यदि(plugin->lib.auth_start_v4){
		mosquitto_callback_पञ्जिकाकोशः(plugin, MOSQ_EVT_EXT_AUTH_START, plugin_v4_auth_start, रिक्त, plugin);
	}
	यदि(plugin->lib.auth_continue_v4){
		mosquitto_callback_पञ्जिकाकोशः(plugin, MOSQ_EVT_EXT_AUTH_CONTINUE, plugin_v4_auth_continue, रिक्त, plugin);
	}
	यदि(plugin->lib.psk_कुञ्चिका_get_v4){
		mosquitto_callback_पञ्जिकाकोशः(plugin, MOSQ_EVT_PSK_कुञ्चिका, plugin_v4_psk_कुञ्चिका_get, रिक्त, plugin);
	}

	प्रत्यागम 0;
}
