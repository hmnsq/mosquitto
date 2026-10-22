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

#समावेश "config.h"

#समावेश <stdio.h>
#समावेश <string.h>

#समावेश "mosquitto/broker.h"
#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/broker_plugin.h"
#समावेश "lib_load.h"
#समावेश "utlist.h"

प्रकार पूर्णांक (*FUNC_auth_plugin_version)(व्योम);
प्रकार पूर्णांक (*FUNC_plugin_version)(पूर्णांक, स्थिर पूर्णांक *);


अटल पूर्णांक plugin_v4_basic_auth(पूर्णांक event, व्योम *event_तथ्य, व्योम *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	काष्ठा mosquitto_evt_basic_auth *ed = event_तथ्य;

	UNUSED(event);

	अगर(plugin->lib.unpwd_check_v4 == शून्य){
		वापस MOSQ_ERR_PLUGIN_DEFER;
	}

	वापस plugin->lib.unpwd_check_v4(
			plugin->lib.user_तथ्य,
			ed->client,
			ed->username,
			ed->password);
}


अटल पूर्णांक plugin_v4_acl_check(पूर्णांक event, व्योम *event_तथ्य, व्योम *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	काष्ठा mosquitto_evt_acl_check *ed = event_तथ्य;
	काष्ठा mosquitto_acl_संदेश संदेश;
	पूर्णांक rc;

	UNUSED(event);

	अगर(plugin->lib.acl_check_v4 == शून्य){
		वापस MOSQ_ERR_PLUGIN_DEFER;
	}

	स_रखो(&संदेश, 0, माप(संदेश));
	संदेश.topic = ed->topic;
	संदेश.payloadlen = ed->payloadlen;
	संदेश.payload = ed->payload;
	संदेश.qos = ed->qos;
	संदेश.retain = ed->retain;

	rc = acl__pre_check(plugin, ed->client, ed->access);
	अगर(rc == MOSQ_ERR_PLUGIN_DEFER){
		वापस plugin->lib.acl_check_v4(
				plugin->lib.user_तथ्य,
				ed->access,
				ed->client,
				&संदेश);
	}अन्यथा{
		वापस rc;
	}
}


अटल पूर्णांक plugin_v4_auth_start(पूर्णांक event, व्योम *event_तथ्य, व्योम *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	काष्ठा mosquitto_evt_extended_auth *ed = event_तथ्य;

	UNUSED(event);

	अगर(plugin->lib.auth_start_v4 == शून्य){
		वापस MOSQ_ERR_PLUGIN_DEFER;
	}

	वापस plugin->lib.auth_start_v4(
			plugin->lib.user_तथ्य,
			ed->client,
			ed->client->auth_method,
			false,
			ed->तथ्य_in, ed->तथ्य_in_len,
			&ed->तथ्य_out, &ed->तथ्य_out_len);
}


अटल पूर्णांक plugin_v4_auth_continue(पूर्णांक event, व्योम *event_तथ्य, व्योम *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	काष्ठा mosquitto_evt_extended_auth *ed = event_तथ्य;

	UNUSED(event);

	अगर(plugin->lib.auth_continue_v4 == शून्य){
		वापस MOSQ_ERR_PLUGIN_DEFER;
	}

	वापस plugin->lib.auth_continue_v4(
			plugin->lib.user_तथ्य,
			ed->client,
			ed->client->auth_method,
			ed->तथ्य_in, ed->तथ्य_in_len,
			&ed->तथ्य_out, &ed->तथ्य_out_len);
}


अटल पूर्णांक plugin_v4_psk_कुंजी_get(पूर्णांक event, व्योम *event_तथ्य, व्योम *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	काष्ठा mosquitto_evt_psk_कुंजी *ed = event_तथ्य;

	UNUSED(event);

	अगर(plugin->lib.psk_कुंजी_get_v4 == शून्य){
		वापस MOSQ_ERR_PLUGIN_DEFER;
	}

	वापस plugin->lib.psk_कुंजी_get_v4(
			plugin->lib.user_तथ्य,
			ed->client,
			ed->hint,
			ed->identity,
			ed->कुंजी,
			ed->max_कुंजी_len);
}


अटल पूर्णांक plugin_v4_reload(पूर्णांक event, व्योम *event_तथ्य, व्योम *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	पूर्णांक rc;

	UNUSED(event);
	UNUSED(event_तथ्य);

	rc = plugin->lib.security_cleanup_v4(
			plugin->lib.user_तथ्य,
			plugin->config.options,
			plugin->config.option_गिनती,
			true);
	अगर(rc){
		वापस rc;
	}

	rc = plugin->lib.security_init_v4(
			plugin->lib.user_तथ्य,
			plugin->config.options,
			plugin->config.option_गिनती,
			true);
	वापस rc;
}


पूर्णांक plugin__load_v4(mosquitto_plugin_id_t *plugin, व्योम *lib)
{
	पूर्णांक rc;

	अगर(!(plugin->lib.plugin_init_v4 = (FUNC_auth_plugin_init_v4)LIB_SYM(lib, "mosquitto_auth_plugin_init"))){
		log__printf(शून्य, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_init().");
		LIB_ERROR();
		वापस MOSQ_ERR_UNKNOWN;
	}
	अगर(!(plugin->lib.plugin_cleanup_v4 = (FUNC_auth_plugin_cleanup_v4)LIB_SYM(lib, "mosquitto_auth_plugin_cleanup"))){
		log__printf(शून्य, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_cleanup().");
		LIB_ERROR();
		वापस MOSQ_ERR_UNKNOWN;
	}

	अगर(!(plugin->lib.security_init_v4 = (FUNC_auth_plugin_security_init_v4)LIB_SYM(lib, "mosquitto_auth_security_init"))){
		log__printf(शून्य, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_init().");
		LIB_ERROR();
		वापस MOSQ_ERR_UNKNOWN;
	}

	अगर(!(plugin->lib.security_cleanup_v4 = (FUNC_auth_plugin_security_cleanup_v4)LIB_SYM(lib, "mosquitto_auth_security_cleanup"))){
		log__printf(शून्य, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_cleanup().");
		LIB_ERROR();
		वापस MOSQ_ERR_UNKNOWN;
	}

	अगर(!(plugin->lib.acl_check_v4 = (FUNC_auth_plugin_acl_check_v4)LIB_SYM(lib, "mosquitto_auth_acl_check"))){
		log__printf(शून्य, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_acl_check().");
		LIB_ERROR();
		वापस MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.unpwd_check_v4 = (FUNC_auth_plugin_unpwd_check_v4)LIB_SYM(lib, "mosquitto_auth_unpwd_check");
	अगर(plugin->lib.unpwd_check_v4){
		log__printf(शून्य, MOSQ_LOG_INFO,
				" ├── Username/password checking enabled.");
	}अन्यथा{
		log__printf(शून्य, MOSQ_LOG_INFO,
				" ├── Username/password checking not enabled.");
	}

	plugin->lib.psk_कुंजी_get_v4 = (FUNC_auth_plugin_psk_कुंजी_get_v4)LIB_SYM(lib, "mosquitto_auth_psk_key_get");
	अगर(plugin->lib.psk_कुंजी_get_v4){
		log__printf(शून्य, MOSQ_LOG_INFO,
				" ├── TLS-PSK checking enabled.");
	}अन्यथा{
		log__printf(शून्य, MOSQ_LOG_INFO,
				" ├── TLS-PSK checking not enabled.");
	}

	plugin->lib.auth_start_v4 = (FUNC_auth_plugin_auth_start_v4)LIB_SYM(lib, "mosquitto_auth_start");
	plugin->lib.auth_continue_v4 = (FUNC_auth_plugin_auth_continue_v4)LIB_SYM(lib, "mosquitto_auth_continue");

	अगर(plugin->lib.auth_start_v4){
		अगर(plugin->lib.auth_continue_v4){
			log__printf(शून्य, MOSQ_LOG_INFO,
					" └── Extended authentication enabled.");
		}अन्यथा{
			log__printf(शून्य, MOSQ_LOG_ERR,
					"Error: Plugin has missing mosquitto_auth_continue() function.");
			वापस MOSQ_ERR_UNKNOWN;
		}
	}अन्यथा{
		log__printf(शून्य, MOSQ_LOG_INFO,
				" └── Extended authentication not enabled.");
	}

	plugin->lib.lib = lib;
	plugin->lib.user_तथ्य = शून्य;
	plugin->lib.identifier = plugin;

	अगर(plugin->lib.plugin_init_v4){
		rc = plugin->lib.plugin_init_v4(&plugin->lib.user_तथ्य, plugin->config.options, plugin->config.option_गिनती);
		अगर(rc){
			log__printf(शून्य, MOSQ_LOG_ERR,
					"Error: Authentication plugin returned %d when initialising.", rc);
			वापस rc;
		}
	}

	mosquitto_callback_register(plugin, MOSQ_EVT_RELOAD, plugin_v4_reload, शून्य, plugin);

	अगर(plugin->lib.unpwd_check_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_BASIC_AUTH, plugin_v4_basic_auth, शून्य, plugin);
	}
	अगर(plugin->lib.acl_check_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_ACL_CHECK, plugin_v4_acl_check, शून्य, plugin);
	}
	अगर(plugin->lib.auth_start_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_EXT_AUTH_START, plugin_v4_auth_start, शून्य, plugin);
	}
	अगर(plugin->lib.auth_continue_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_EXT_AUTH_CONTINUE, plugin_v4_auth_continue, शून्य, plugin);
	}
	अगर(plugin->lib.psk_कुंजी_get_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_PSK_कुंजी, plugin_v4_psk_कुंजी_get, शून्य, plugin);
	}

	वापस 0;
}
