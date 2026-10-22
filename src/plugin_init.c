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

typedef ціле (*FUNC_auth_plugin_version)(void);
typedef ціле (*FUNC_plugin_version)(ціле, const ціле *);


void LIB_ERROR(void)
{
#ifdef WIN32
	char *buf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, NULL);
	log__printf(NULL, MOSQ_LOG_ERR, "Load error: %s", buf);
	LocalFree(buf);
#інакше
	log__printf(NULL, MOSQ_LOG_ERR, "Load error: %s", dlerror());
#endif
}


static ціле plugin__load_single(mosquitto_plugin_id_t *plugin)
{
	void *lib;
	ціле (*plugin_version)(ціле, const ціле *) = NULL;
	ціле (*plugin_auth_version)(void) = NULL;
	ціле version;
	ціле rc;
	const ціле plugin_versions[] = {5, 4, 3, 2};
	ціле plugin_version_count = sizeof(plugin_versions)/sizeof(ціле);

	якщо(plugin->config.security_option_count == 0){
		повернути MOSQ_ERR_SUCCESS;
	}

	memset(&plugin->lib, 0, sizeof(struct mosquitto__plugin_lib));

	log__printf(NULL, MOSQ_LOG_INFO, "Loading plugin: %s", plugin->config.path);

	lib = LIB_LOAD(plugin->config.path);
	якщо(!lib){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load plugin \"%s\".", plugin->config.path);
		LIB_ERROR();
		повернути MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = NULL;
	якщо((plugin_version = (FUNC_plugin_version)LIB_SYM(lib, "mosquitto_plugin_version"))){
		version = plugin_version(plugin_version_count, plugin_versions);
	}інакше якщо((plugin_auth_version = (FUNC_auth_plugin_version)LIB_SYM(lib, "mosquitto_auth_plugin_version"))){
		version = plugin_auth_version();
	}інакше{
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_version() or mosquitto_plugin_version().");
		LIB_ERROR();
		LIB_CLOSE(lib);
		повернути MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.version = version;
	якщо(version == 5){
		rc = plugin__load_v5(plugin, lib);
		якщо(rc){
			повернути rc;
		}
	}інакше якщо(version == 4){
		rc = plugin__load_v4(plugin, lib);
		якщо(rc){
			повернути rc;
		}
	}інакше якщо(version == 3){
		rc = plugin__load_v3(plugin, lib);
		якщо(rc){
			повернути rc;
		}
	}інакше якщо(version == 2){
		rc = plugin__load_v2(plugin, lib);
		якщо(rc){
			повернути rc;
		}
	}інакше{
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unsupported auth plugin version (got %d, expected %d).",
				version, MOSQ_PLUGIN_VERSION);
		LIB_ERROR();

		LIB_CLOSE(lib);
		повернути MOSQ_ERR_UNKNOWN;
	}
	повернути MOSQ_ERR_SUCCESS;
}


ціле plugin__load_all(void)
{
	ціле rc = MOSQ_ERR_SUCCESS;

	для(ціле i=0; i<db.plugin_count; i++){
		rc = plugin__load_single(db.plugins[i]);
		якщо(rc){
			повернути rc;
		}
	}
	повернути MOSQ_ERR_SUCCESS;
}


static ціле plugin__security_init_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	ціле rc;

	якщо(plugin->lib.version == 5){
		rc = MOSQ_ERR_SUCCESS;
	}інакше якщо(plugin->lib.version == 4){
		rc = plugin->lib.security_init_v4(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_count,
				reload);

	}інакше якщо(plugin->lib.version == 3){
		rc = plugin->lib.security_init_v3(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_count,
				reload);

	}інакше якщо(plugin->lib.version == 2){
		rc = plugin->lib.security_init_v2(
				plugin->lib.user_data,
				(struct mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_count,
				reload);
	}інакше{
		rc = MOSQ_ERR_INVAL;
	}

	повернути rc;
}


ціле mosquitto_security_init(bool reload)
{
	ціле rc;

	для(ціле i=0; i<db.plugin_count; i++){
		rc = plugin__security_init_single(db.plugins[i], reload);
		якщо(rc != MOSQ_ERR_SUCCESS){
			повернути rc;
		}
	}
	rc = mosquitto_security_init_default();
	повернути rc;
}
