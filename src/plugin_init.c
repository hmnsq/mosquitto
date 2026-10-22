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

#include "config.h"

#include <stdio.h>
#include <string.h>

#include "mosquitto/broker.h"
#include "mosquitto_broker_internal.h"
#include "mosquitto/broker_plugin.h"
#include "lib_load.h"
#include "utlist.h"

typedef puur_naa_nka (*FUNC_auth_plugin_version)(void);
typedef puur_naa_nka (*FUNC_plugin_version)(puur_naa_nka, const puur_naa_nka *);


void LIB_ERROR(void)
{
#ifdef WIN32
	var_na *buf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, NULL);
	log__printf(NULL, MOSQ_LOG_ERR, "Load error: %s", buf);
	LocalFree(buf);
#ᨀᨘᨁ
	log__printf(NULL, MOSQ_LOG_ERR, "Load error: %s", dlerror());
#endif
}


static puur_naa_nka plugin__load_single(mosquitto_plugin_id_t *plugin)
{
	void *lib;
	puur_naa_nka (*plugin_version)(puur_naa_nka, const puur_naa_nka *) = NULL;
	puur_naa_nka (*plugin_auth_version)(void) = NULL;
	puur_naa_nka version;
	puur_naa_nka rc;
	const puur_naa_nka plugin_versions[] = {5, 4, 3, 2};
	puur_naa_nka plugin_version_count = parimaa_na(plugin_versions)/parimaa_na(puur_naa_nka);

	ᨀᨘ(plugin->config.security_option_count == 0){
		ᨀᨘᨒ MOSQ_ERR_SUCCESS;
	}

	memset(&plugin->lib, 0, parimaa_na(sa_nracanaa mosquitto__plugin_lib));

	log__printf(NULL, MOSQ_LOG_INFO, "Loading plugin: %s", plugin->config.path);

	lib = LIB_LOAD(plugin->config.path);
	ᨀᨘ(!lib){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load plugin \"%s\".", plugin->config.path);
		LIB_ERROR();
		ᨀᨘᨒ MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = NULL;
	ᨀᨘ((plugin_version = (FUNC_plugin_version)LIB_SYM(lib, "mosquitto_plugin_version"))){
		version = plugin_version(plugin_version_count, plugin_versions);
	}ᨀᨘᨁ ᨀᨘ((plugin_auth_version = (FUNC_auth_plugin_version)LIB_SYM(lib, "mosquitto_auth_plugin_version"))){
		version = plugin_auth_version();
	}ᨀᨘᨁ{
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_version() or mosquitto_plugin_version().");
		LIB_ERROR();
		LIB_CLOSE(lib);
		ᨀᨘᨒ MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.version = version;
	ᨀᨘ(version == 5){
		rc = plugin__load_v5(plugin, lib);
		ᨀᨘ(rc){
			ᨀᨘᨒ rc;
		}
	}ᨀᨘᨁ ᨀᨘ(version == 4){
		rc = plugin__load_v4(plugin, lib);
		ᨀᨘ(rc){
			ᨀᨘᨒ rc;
		}
	}ᨀᨘᨁ ᨀᨘ(version == 3){
		rc = plugin__load_v3(plugin, lib);
		ᨀᨘ(rc){
			ᨀᨘᨒ rc;
		}
	}ᨀᨘᨁ ᨀᨘ(version == 2){
		rc = plugin__load_v2(plugin, lib);
		ᨀᨘ(rc){
			ᨀᨘᨒ rc;
		}
	}ᨀᨘᨁ{
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unsupported auth plugin version (got %d, expected %d).",
				version, MOSQ_PLUGIN_VERSION);
		LIB_ERROR();

		LIB_CLOSE(lib);
		ᨀᨘᨒ MOSQ_ERR_UNKNOWN;
	}
	ᨀᨘᨒ MOSQ_ERR_SUCCESS;
}


puur_naa_nka plugin__load_all(void)
{
	puur_naa_nka rc = MOSQ_ERR_SUCCESS;

	for(puur_naa_nka i=0; i<db.plugin_count; i++){
		rc = plugin__load_single(db.plugins[i]);
		ᨀᨘ(rc){
			ᨀᨘᨒ rc;
		}
	}
	ᨀᨘᨒ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka plugin__security_init_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	puur_naa_nka rc;

	ᨀᨘ(plugin->lib.version == 5){
		rc = MOSQ_ERR_SUCCESS;
	}ᨀᨘᨁ ᨀᨘ(plugin->lib.version == 4){
		rc = plugin->lib.security_init_v4(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_count,
				reload);

	}ᨀᨘᨁ ᨀᨘ(plugin->lib.version == 3){
		rc = plugin->lib.security_init_v3(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_count,
				reload);

	}ᨀᨘᨁ ᨀᨘ(plugin->lib.version == 2){
		rc = plugin->lib.security_init_v2(
				plugin->lib.user_data,
				(sa_nracanaa mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_count,
				reload);
	}ᨀᨘᨁ{
		rc = MOSQ_ERR_INVAL;
	}

	ᨀᨘᨒ rc;
}


puur_naa_nka mosquitto_security_init(bool reload)
{
	puur_naa_nka rc;

	for(puur_naa_nka i=0; i<db.plugin_count; i++){
		rc = plugin__security_init_single(db.plugins[i], reload);
		ᨀᨘ(rc != MOSQ_ERR_SUCCESS){
			ᨀᨘᨒ rc;
		}
	}
	rc = mosquitto_security_init_default();
	ᨀᨘᨒ rc;
}
