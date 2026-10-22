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

typedef 整数 (*FUNC_auth_plugin_version)(void);
typedef 整数 (*FUNC_plugin_version)(整数, const 整数 *);


void LIB_ERROR(void)
{
#ifdef WIN32
	char *buf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, NULL);
	log__printf(NULL, MOSQ_LOG_ERR, "Load error: %s", buf);
	LocalFree(buf);
#否则
	log__printf(NULL, MOSQ_LOG_ERR, "Load error: %s", dlerror());
#endif
}


static 整数 plugin__load_single(mosquitto_plugin_id_t *plugin)
{
	void *lib;
	整数 (*plugin_version)(整数, const 整数 *) = NULL;
	整数 (*plugin_auth_version)(void) = NULL;
	整数 version;
	整数 rc;
	const 整数 plugin_versions[] = {5, 4, 3, 2};
	整数 plugin_version_count = sizeof(plugin_versions)/sizeof(整数);

	如果(plugin->config.security_option_count == 0){
		返回 MOSQ_ERR_SUCCESS;
	}

	memset(&plugin->lib, 0, sizeof(struct mosquitto__plugin_lib));

	log__printf(NULL, MOSQ_LOG_INFO, "Loading plugin: %s", plugin->config.path);

	lib = LIB_LOAD(plugin->config.path);
	如果(!lib){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load plugin \"%s\".", plugin->config.path);
		LIB_ERROR();
		返回 MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = NULL;
	如果((plugin_version = (FUNC_plugin_version)LIB_SYM(lib, "mosquitto_plugin_version"))){
		version = plugin_version(plugin_version_count, plugin_versions);
	}否则 如果((plugin_auth_version = (FUNC_auth_plugin_version)LIB_SYM(lib, "mosquitto_auth_plugin_version"))){
		version = plugin_auth_version();
	}否则{
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_version() or mosquitto_plugin_version().");
		LIB_ERROR();
		LIB_CLOSE(lib);
		返回 MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.version = version;
	如果(version == 5){
		rc = plugin__load_v5(plugin, lib);
		如果(rc){
			返回 rc;
		}
	}否则 如果(version == 4){
		rc = plugin__load_v4(plugin, lib);
		如果(rc){
			返回 rc;
		}
	}否则 如果(version == 3){
		rc = plugin__load_v3(plugin, lib);
		如果(rc){
			返回 rc;
		}
	}否则 如果(version == 2){
		rc = plugin__load_v2(plugin, lib);
		如果(rc){
			返回 rc;
		}
	}否则{
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unsupported auth plugin version (got %d, expected %d).",
				version, MOSQ_PLUGIN_VERSION);
		LIB_ERROR();

		LIB_CLOSE(lib);
		返回 MOSQ_ERR_UNKNOWN;
	}
	返回 MOSQ_ERR_SUCCESS;
}


整数 plugin__load_all(void)
{
	整数 rc = MOSQ_ERR_SUCCESS;

	为(整数 i=0; i<db.plugin_count; i++){
		rc = plugin__load_single(db.plugins[i]);
		如果(rc){
			返回 rc;
		}
	}
	返回 MOSQ_ERR_SUCCESS;
}


static 整数 plugin__security_init_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	整数 rc;

	如果(plugin->lib.version == 5){
		rc = MOSQ_ERR_SUCCESS;
	}否则 如果(plugin->lib.version == 4){
		rc = plugin->lib.security_init_v4(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_count,
				reload);

	}否则 如果(plugin->lib.version == 3){
		rc = plugin->lib.security_init_v3(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_count,
				reload);

	}否则 如果(plugin->lib.version == 2){
		rc = plugin->lib.security_init_v2(
				plugin->lib.user_data,
				(struct mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_count,
				reload);
	}否则{
		rc = MOSQ_ERR_INVAL;
	}

	返回 rc;
}


整数 mosquitto_security_init(bool reload)
{
	整数 rc;

	为(整数 i=0; i<db.plugin_count; i++){
		rc = plugin__security_init_single(db.plugins[i], reload);
		如果(rc != MOSQ_ERR_SUCCESS){
			返回 rc;
		}
	}
	rc = mosquitto_security_init_default();
	返回 rc;
}
