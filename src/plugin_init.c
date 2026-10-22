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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "mosquitto/broker.h"
#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/broker_plugin.h"
#અન્તર્ભાવ "lib_load.h"
#અન્તર્ભાવ "utlist.h"

પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_auth_plugin_version)(શૂન્ય);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_plugin_version)(પૂર્ણાંક, અચલ પૂર્ણાંક *);


શૂન્ય LIB_ERROR(શૂન્ય)
{
#ifdef WIN32
	અક્ષર *buf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			ખાલી, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, ખાલી);
	log__printf(ખાલી, MOSQ_LOG_ERR, "Load error: %s", buf);
	LocalFree(buf);
#નહીંતર
	log__printf(ખાલી, MOSQ_LOG_ERR, "Load error: %s", dlerror());
#endif
}


સ્થિર પૂર્ણાંક plugin__load_single(mosquitto_plugin_id_t *plugin)
{
	શૂન્ય *lib;
	પૂર્ણાંક (*plugin_version)(પૂર્ણાંક, અચલ પૂર્ણાંક *) = ખાલી;
	પૂર્ણાંક (*plugin_auth_version)(શૂન્ય) = ખાલી;
	પૂર્ણાંક version;
	પૂર્ણાંક rc;
	અચલ પૂર્ણાંક plugin_versions[] = {5, 4, 3, 2};
	પૂર્ણાંક plugin_version_ગણતરી = કદ(plugin_versions)/કદ(પૂર્ણાંક);

	જો(plugin->config.security_option_ગણતરી == 0){
		પાછા MOSQ_ERR_SUCCESS;
	}

	memset(&plugin->lib, 0, કદ(સંરચના mosquitto__plugin_lib));

	log__printf(ખાલી, MOSQ_LOG_INFO, "Loading plugin: %s", plugin->config.path);

	lib = LIB_LOAD(plugin->config.path);
	જો(!lib){
		log__printf(ખાલી, MOSQ_LOG_ERR,
				"Error: Unable to load plugin \"%s\".", plugin->config.path);
		LIB_ERROR();
		પાછા MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = ખાલી;
	જો((plugin_version = (FUNC_plugin_version)LIB_SYM(lib, "mosquitto_plugin_version"))){
		version = plugin_version(plugin_version_ગણતરી, plugin_versions);
	}નહીંતર જો((plugin_auth_version = (FUNC_auth_plugin_version)LIB_SYM(lib, "mosquitto_auth_plugin_version"))){
		version = plugin_auth_version();
	}નહીંતર{
		log__printf(ખાલી, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_version() or mosquitto_plugin_version().");
		LIB_ERROR();
		LIB_CLOSE(lib);
		પાછા MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.version = version;
	જો(version == 5){
		rc = plugin__load_v5(plugin, lib);
		જો(rc){
			પાછા rc;
		}
	}નહીંતર જો(version == 4){
		rc = plugin__load_v4(plugin, lib);
		જો(rc){
			પાછા rc;
		}
	}નહીંતર જો(version == 3){
		rc = plugin__load_v3(plugin, lib);
		જો(rc){
			પાછા rc;
		}
	}નહીંતર જો(version == 2){
		rc = plugin__load_v2(plugin, lib);
		જો(rc){
			પાછા rc;
		}
	}નહીંતર{
		log__printf(ખાલી, MOSQ_LOG_ERR,
				"Error: Unsupported auth plugin version (got %d, expected %d).",
				version, MOSQ_PLUGIN_VERSION);
		LIB_ERROR();

		LIB_CLOSE(lib);
		પાછા MOSQ_ERR_UNKNOWN;
	}
	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક plugin__load_all(શૂન્ય)
{
	પૂર્ણાંક rc = MOSQ_ERR_SUCCESS;

	માટે(પૂર્ણાંક i=0; i<db.plugin_ગણતરી; i++){
		rc = plugin__load_single(db.plugins[i]);
		જો(rc){
			પાછા rc;
		}
	}
	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક plugin__security_init_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	પૂર્ણાંક rc;

	જો(plugin->lib.version == 5){
		rc = MOSQ_ERR_SUCCESS;
	}નહીંતર જો(plugin->lib.version == 4){
		rc = plugin->lib.security_init_v4(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_ગણતરી,
				reload);

	}નહીંતર જો(plugin->lib.version == 3){
		rc = plugin->lib.security_init_v3(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_ગણતરી,
				reload);

	}નહીંતર જો(plugin->lib.version == 2){
		rc = plugin->lib.security_init_v2(
				plugin->lib.user_data,
				(સંરચના mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_ગણતરી,
				reload);
	}નહીંતર{
		rc = MOSQ_ERR_INVAL;
	}

	પાછા rc;
}


પૂર્ણાંક mosquitto_security_init(bool reload)
{
	પૂર્ણાંક rc;

	માટે(પૂર્ણાંક i=0; i<db.plugin_ગણતરી; i++){
		rc = plugin__security_init_single(db.plugins[i], reload);
		જો(rc != MOSQ_ERR_SUCCESS){
			પાછા rc;
		}
	}
	rc = mosquitto_security_init_default();
	પાછા rc;
}
