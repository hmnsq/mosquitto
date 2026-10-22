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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "mosquitto/broker.h"
#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/broker_plugin.h"
#അന്തര്ഭാവ "lib_load.h"
#അന്തര്ഭാവ "utlist.h"

തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_auth_plugin_version)(ശൂന്യം);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_plugin_version)(പൂർണ്ണസംഖ്യ, സ്ഥിരം പൂർണ്ണസംഖ്യ *);


ശൂന്യം LIB_ERROR(ശൂന്യം)
{
#ifdef WIN32
	അക്ഷരം *buf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			ശൂന്യ, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, ശൂന്യ);
	log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Load error: %s", buf);
	LocalFree(buf);
#അല്ലെങ്കിൽ
	log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Load error: %s", dlerror());
#endif
}


സ്ഥിതം പൂർണ്ണസംഖ്യ plugin__load_single(mosquitto_plugin_id_t *plugin)
{
	ശൂന്യം *lib;
	പൂർണ്ണസംഖ്യ (*plugin_version)(പൂർണ്ണസംഖ്യ, സ്ഥിരം പൂർണ്ണസംഖ്യ *) = ശൂന്യ;
	പൂർണ്ണസംഖ്യ (*plugin_auth_version)(ശൂന്യം) = ശൂന്യ;
	പൂർണ്ണസംഖ്യ version;
	പൂർണ്ണസംഖ്യ rc;
	സ്ഥിരം പൂർണ്ണസംഖ്യ plugin_versions[] = {5, 4, 3, 2};
	പൂർണ്ണസംഖ്യ plugin_version_എണ്ണം = വലുപ്പം(plugin_versions)/വലുപ്പം(പൂർണ്ണസംഖ്യ);

	എങ്കിൽ(plugin->config.security_option_എണ്ണം == 0){
		തിരികെ MOSQ_ERR_SUCCESS;
	}

	memset(&plugin->lib, 0, വലുപ്പം(ഘടന mosquitto__plugin_lib));

	log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Loading plugin: %s", plugin->config.path);

	lib = LIB_LOAD(plugin->config.path);
	എങ്കിൽ(!lib){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR,
				"Error: Unable to load plugin \"%s\".", plugin->config.path);
		LIB_ERROR();
		തിരികെ MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = ശൂന്യ;
	എങ്കിൽ((plugin_version = (FUNC_plugin_version)LIB_SYM(lib, "mosquitto_plugin_version"))){
		version = plugin_version(plugin_version_എണ്ണം, plugin_versions);
	}അല്ലെങ്കിൽ എങ്കിൽ((plugin_auth_version = (FUNC_auth_plugin_version)LIB_SYM(lib, "mosquitto_auth_plugin_version"))){
		version = plugin_auth_version();
	}അല്ലെങ്കിൽ{
		log__printf(ശൂന്യ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_version() or mosquitto_plugin_version().");
		LIB_ERROR();
		LIB_CLOSE(lib);
		തിരികെ MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.version = version;
	എങ്കിൽ(version == 5){
		rc = plugin__load_v5(plugin, lib);
		എങ്കിൽ(rc){
			തിരികെ rc;
		}
	}അല്ലെങ്കിൽ എങ്കിൽ(version == 4){
		rc = plugin__load_v4(plugin, lib);
		എങ്കിൽ(rc){
			തിരികെ rc;
		}
	}അല്ലെങ്കിൽ എങ്കിൽ(version == 3){
		rc = plugin__load_v3(plugin, lib);
		എങ്കിൽ(rc){
			തിരികെ rc;
		}
	}അല്ലെങ്കിൽ എങ്കിൽ(version == 2){
		rc = plugin__load_v2(plugin, lib);
		എങ്കിൽ(rc){
			തിരികെ rc;
		}
	}അല്ലെങ്കിൽ{
		log__printf(ശൂന്യ, MOSQ_LOG_ERR,
				"Error: Unsupported auth plugin version (got %d, expected %d).",
				version, MOSQ_PLUGIN_VERSION);
		LIB_ERROR();

		LIB_CLOSE(lib);
		തിരികെ MOSQ_ERR_UNKNOWN;
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ plugin__load_all(ശൂന്യം)
{
	പൂർണ്ണസംഖ്യ rc = MOSQ_ERR_SUCCESS;

	വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<db.plugin_എണ്ണം; i++){
		rc = plugin__load_single(db.plugins[i]);
		എങ്കിൽ(rc){
			തിരികെ rc;
		}
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ plugin__security_init_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ(plugin->lib.version == 5){
		rc = MOSQ_ERR_SUCCESS;
	}അല്ലെങ്കിൽ എങ്കിൽ(plugin->lib.version == 4){
		rc = plugin->lib.security_init_v4(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_എണ്ണം,
				reload);

	}അല്ലെങ്കിൽ എങ്കിൽ(plugin->lib.version == 3){
		rc = plugin->lib.security_init_v3(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_എണ്ണം,
				reload);

	}അല്ലെങ്കിൽ എങ്കിൽ(plugin->lib.version == 2){
		rc = plugin->lib.security_init_v2(
				plugin->lib.user_data,
				(ഘടന mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_എണ്ണം,
				reload);
	}അല്ലെങ്കിൽ{
		rc = MOSQ_ERR_INVAL;
	}

	തിരികെ rc;
}


പൂർണ്ണസംഖ്യ mosquitto_security_init(bool reload)
{
	പൂർണ്ണസംഖ്യ rc;

	വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<db.plugin_എണ്ണം; i++){
		rc = plugin__security_init_single(db.plugins[i], reload);
		എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
			തിരികെ rc;
		}
	}
	rc = mosquitto_security_init_default();
	തിരികെ rc;
}
