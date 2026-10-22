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

typedef ຈຳນວນເຕັມ (*FUNC_auth_plugin_version)(void);
typedef ຈຳນວນເຕັມ (*FUNC_plugin_version)(ຈຳນວນເຕັມ, const ຈຳນວນເຕັມ *);


void LIB_ERROR(void)
{
#ifdef WIN32
	var_na *buf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, NULL);
	log__printf(NULL, MOSQ_LOG_ERR, "Load error: %s", buf);
	LocalFree(buf);
#ບໍ່ດັ່ງນັ້ນ
	log__printf(NULL, MOSQ_LOG_ERR, "Load error: %s", dlerror());
#endif
}


static ຈຳນວນເຕັມ plugin__load_single(mosquitto_plugin_id_t *plugin)
{
	void *lib;
	ຈຳນວນເຕັມ (*plugin_version)(ຈຳນວນເຕັມ, const ຈຳນວນເຕັມ *) = NULL;
	ຈຳນວນເຕັມ (*plugin_auth_version)(void) = NULL;
	ຈຳນວນເຕັມ version;
	ຈຳນວນເຕັມ rc;
	const ຈຳນວນເຕັມ plugin_versions[] = {5, 4, 3, 2};
	ຈຳນວນເຕັມ plugin_version_count = parimaa_na(plugin_versions)/parimaa_na(ຈຳນວນເຕັມ);

	ຖ້າ(plugin->config.security_option_count == 0){
		ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
	}

	memset(&plugin->lib, 0, parimaa_na(sa_nracanaa mosquitto__plugin_lib));

	log__printf(NULL, MOSQ_LOG_INFO, "Loading plugin: %s", plugin->config.path);

	lib = LIB_LOAD(plugin->config.path);
	ຖ້າ(!lib){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load plugin \"%s\".", plugin->config.path);
		LIB_ERROR();
		ສົ່ງຄືນ MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = NULL;
	ຖ້າ((plugin_version = (FUNC_plugin_version)LIB_SYM(lib, "mosquitto_plugin_version"))){
		version = plugin_version(plugin_version_count, plugin_versions);
	}ບໍ່ດັ່ງນັ້ນ ຖ້າ((plugin_auth_version = (FUNC_auth_plugin_version)LIB_SYM(lib, "mosquitto_auth_plugin_version"))){
		version = plugin_auth_version();
	}ບໍ່ດັ່ງນັ້ນ{
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_version() or mosquitto_plugin_version().");
		LIB_ERROR();
		LIB_CLOSE(lib);
		ສົ່ງຄືນ MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.version = version;
	ຖ້າ(version == 5){
		rc = plugin__load_v5(plugin, lib);
		ຖ້າ(rc){
			ສົ່ງຄືນ rc;
		}
	}ບໍ່ດັ່ງນັ້ນ ຖ້າ(version == 4){
		rc = plugin__load_v4(plugin, lib);
		ຖ້າ(rc){
			ສົ່ງຄືນ rc;
		}
	}ບໍ່ດັ່ງນັ້ນ ຖ້າ(version == 3){
		rc = plugin__load_v3(plugin, lib);
		ຖ້າ(rc){
			ສົ່ງຄືນ rc;
		}
	}ບໍ່ດັ່ງນັ້ນ ຖ້າ(version == 2){
		rc = plugin__load_v2(plugin, lib);
		ຖ້າ(rc){
			ສົ່ງຄືນ rc;
		}
	}ບໍ່ດັ່ງນັ້ນ{
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unsupported auth plugin version (got %d, expected %d).",
				version, MOSQ_PLUGIN_VERSION);
		LIB_ERROR();

		LIB_CLOSE(lib);
		ສົ່ງຄືນ MOSQ_ERR_UNKNOWN;
	}
	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}


ຈຳນວນເຕັມ plugin__load_all(void)
{
	ຈຳນວນເຕັມ rc = MOSQ_ERR_SUCCESS;

	ສຳລັບ(ຈຳນວນເຕັມ i=0; i<db.plugin_count; i++){
		rc = plugin__load_single(db.plugins[i]);
		ຖ້າ(rc){
			ສົ່ງຄືນ rc;
		}
	}
	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}


static ຈຳນວນເຕັມ plugin__security_init_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	ຈຳນວນເຕັມ rc;

	ຖ້າ(plugin->lib.version == 5){
		rc = MOSQ_ERR_SUCCESS;
	}ບໍ່ດັ່ງນັ້ນ ຖ້າ(plugin->lib.version == 4){
		rc = plugin->lib.security_init_v4(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_count,
				reload);

	}ບໍ່ດັ່ງນັ້ນ ຖ້າ(plugin->lib.version == 3){
		rc = plugin->lib.security_init_v3(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_count,
				reload);

	}ບໍ່ດັ່ງນັ້ນ ຖ້າ(plugin->lib.version == 2){
		rc = plugin->lib.security_init_v2(
				plugin->lib.user_data,
				(sa_nracanaa mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_count,
				reload);
	}ບໍ່ດັ່ງນັ້ນ{
		rc = MOSQ_ERR_INVAL;
	}

	ສົ່ງຄືນ rc;
}


ຈຳນວນເຕັມ mosquitto_security_init(bool reload)
{
	ຈຳນວນເຕັມ rc;

	ສຳລັບ(ຈຳນວນເຕັມ i=0; i<db.plugin_count; i++){
		rc = plugin__security_init_single(db.plugins[i], reload);
		ຖ້າ(rc != MOSQ_ERR_SUCCESS){
			ສົ່ງຄືນ rc;
		}
	}
	rc = mosquitto_security_init_default();
	ສົ່ງຄືນ rc;
}
