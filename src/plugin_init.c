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

typedef целое (*FUNC_auth_plugin_version)(void);
typedef целое (*FUNC_plugin_version)(целое, const целое *);


void LIB_ERROR(void)
{
#ifdef WIN32
	char *buf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, NULL);
	log__printf(NULL, MOSQ_LOG_ERR, "Load error: %s", buf);
	LocalFree(buf);
#иначе
	log__printf(NULL, MOSQ_LOG_ERR, "Load error: %s", dlerror());
#endif
}


static целое plugin__load_single(mosquitto_plugin_id_t *plugin)
{
	void *lib;
	целое (*plugin_version)(целое, const целое *) = NULL;
	целое (*plugin_auth_version)(void) = NULL;
	целое version;
	целое rc;
	const целое plugin_versions[] = {5, 4, 3, 2};
	целое plugin_version_count = sizeof(plugin_versions)/sizeof(целое);

	если(plugin->config.security_option_count == 0){
		вернуть MOSQ_ERR_SUCCESS;
	}

	memset(&plugin->lib, 0, sizeof(struct mosquitto__plugin_lib));

	log__printf(NULL, MOSQ_LOG_INFO, "Loading plugin: %s", plugin->config.path);

	lib = LIB_LOAD(plugin->config.path);
	если(!lib){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load plugin \"%s\".", plugin->config.path);
		LIB_ERROR();
		вернуть MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = NULL;
	если((plugin_version = (FUNC_plugin_version)LIB_SYM(lib, "mosquitto_plugin_version"))){
		version = plugin_version(plugin_version_count, plugin_versions);
	}иначе если((plugin_auth_version = (FUNC_auth_plugin_version)LIB_SYM(lib, "mosquitto_auth_plugin_version"))){
		version = plugin_auth_version();
	}иначе{
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_version() or mosquitto_plugin_version().");
		LIB_ERROR();
		LIB_CLOSE(lib);
		вернуть MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.version = version;
	если(version == 5){
		rc = plugin__load_v5(plugin, lib);
		если(rc){
			вернуть rc;
		}
	}иначе если(version == 4){
		rc = plugin__load_v4(plugin, lib);
		если(rc){
			вернуть rc;
		}
	}иначе если(version == 3){
		rc = plugin__load_v3(plugin, lib);
		если(rc){
			вернуть rc;
		}
	}иначе если(version == 2){
		rc = plugin__load_v2(plugin, lib);
		если(rc){
			вернуть rc;
		}
	}иначе{
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unsupported auth plugin version (got %d, expected %d).",
				version, MOSQ_PLUGIN_VERSION);
		LIB_ERROR();

		LIB_CLOSE(lib);
		вернуть MOSQ_ERR_UNKNOWN;
	}
	вернуть MOSQ_ERR_SUCCESS;
}


целое plugin__load_all(void)
{
	целое rc = MOSQ_ERR_SUCCESS;

	для(целое i=0; i<db.plugin_count; i++){
		rc = plugin__load_single(db.plugins[i]);
		если(rc){
			вернуть rc;
		}
	}
	вернуть MOSQ_ERR_SUCCESS;
}


static целое plugin__security_init_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	целое rc;

	если(plugin->lib.version == 5){
		rc = MOSQ_ERR_SUCCESS;
	}иначе если(plugin->lib.version == 4){
		rc = plugin->lib.security_init_v4(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_count,
				reload);

	}иначе если(plugin->lib.version == 3){
		rc = plugin->lib.security_init_v3(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_count,
				reload);

	}иначе если(plugin->lib.version == 2){
		rc = plugin->lib.security_init_v2(
				plugin->lib.user_data,
				(struct mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_count,
				reload);
	}иначе{
		rc = MOSQ_ERR_INVAL;
	}

	вернуть rc;
}


целое mosquitto_security_init(bool reload)
{
	целое rc;

	для(целое i=0; i<db.plugin_count; i++){
		rc = plugin__security_init_single(db.plugins[i], reload);
		если(rc != MOSQ_ERR_SUCCESS){
			вернуть rc;
		}
	}
	rc = mosquitto_security_init_default();
	вернуть rc;
}
