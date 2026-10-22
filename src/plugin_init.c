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

typedef цяло (*FUNC_auth_plugin_version)(void);
typedef цяло (*FUNC_plugin_version)(цяло, const цяло *);


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


static цяло plugin__load_single(mosquitto_plugin_id_t *plugin)
{
	void *lib;
	цяло (*plugin_version)(цяло, const цяло *) = NULL;
	цяло (*plugin_auth_version)(void) = NULL;
	цяло version;
	цяло rc;
	const цяло plugin_versions[] = {5, 4, 3, 2};
	цяло plugin_version_count = sizeof(plugin_versions)/sizeof(цяло);

	ако(plugin->config.security_option_count == 0){
		върни MOSQ_ERR_SUCCESS;
	}

	memset(&plugin->lib, 0, sizeof(struct mosquitto__plugin_lib));

	log__printf(NULL, MOSQ_LOG_INFO, "Loading plugin: %s", plugin->config.path);

	lib = LIB_LOAD(plugin->config.path);
	ако(!lib){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load plugin \"%s\".", plugin->config.path);
		LIB_ERROR();
		върни MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = NULL;
	ако((plugin_version = (FUNC_plugin_version)LIB_SYM(lib, "mosquitto_plugin_version"))){
		version = plugin_version(plugin_version_count, plugin_versions);
	}иначе ако((plugin_auth_version = (FUNC_auth_plugin_version)LIB_SYM(lib, "mosquitto_auth_plugin_version"))){
		version = plugin_auth_version();
	}иначе{
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_version() or mosquitto_plugin_version().");
		LIB_ERROR();
		LIB_CLOSE(lib);
		върни MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.version = version;
	ако(version == 5){
		rc = plugin__load_v5(plugin, lib);
		ако(rc){
			върни rc;
		}
	}иначе ако(version == 4){
		rc = plugin__load_v4(plugin, lib);
		ако(rc){
			върни rc;
		}
	}иначе ако(version == 3){
		rc = plugin__load_v3(plugin, lib);
		ако(rc){
			върни rc;
		}
	}иначе ако(version == 2){
		rc = plugin__load_v2(plugin, lib);
		ако(rc){
			върни rc;
		}
	}иначе{
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unsupported auth plugin version (got %d, expected %d).",
				version, MOSQ_PLUGIN_VERSION);
		LIB_ERROR();

		LIB_CLOSE(lib);
		върни MOSQ_ERR_UNKNOWN;
	}
	върни MOSQ_ERR_SUCCESS;
}


цяло plugin__load_all(void)
{
	цяло rc = MOSQ_ERR_SUCCESS;

	за(цяло i=0; i<db.plugin_count; i++){
		rc = plugin__load_single(db.plugins[i]);
		ако(rc){
			върни rc;
		}
	}
	върни MOSQ_ERR_SUCCESS;
}


static цяло plugin__security_init_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	цяло rc;

	ако(plugin->lib.version == 5){
		rc = MOSQ_ERR_SUCCESS;
	}иначе ако(plugin->lib.version == 4){
		rc = plugin->lib.security_init_v4(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_count,
				reload);

	}иначе ако(plugin->lib.version == 3){
		rc = plugin->lib.security_init_v3(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_count,
				reload);

	}иначе ако(plugin->lib.version == 2){
		rc = plugin->lib.security_init_v2(
				plugin->lib.user_data,
				(struct mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_count,
				reload);
	}иначе{
		rc = MOSQ_ERR_INVAL;
	}

	върни rc;
}


цяло mosquitto_security_init(bool reload)
{
	цяло rc;

	за(цяло i=0; i<db.plugin_count; i++){
		rc = plugin__security_init_single(db.plugins[i], reload);
		ако(rc != MOSQ_ERR_SUCCESS){
			върни rc;
		}
	}
	rc = mosquitto_security_init_default();
	върни rc;
}
