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

typedef ကိန်းပြည့် (*FUNC_auth_plugin_version)(void);
typedef ကိန်းပြည့် (*FUNC_plugin_version)(ကိန်းပြည့်, const ကိန်းပြည့် *);


void LIB_ERROR(void)
{
#ifdef WIN32
	var_na *buf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, NULL);
	log__printf(NULL, MOSQ_LOG_ERR, "Load error: %s", buf);
	LocalFree(buf);
#မဟုတ်ပါက
	log__printf(NULL, MOSQ_LOG_ERR, "Load error: %s", dlerror());
#endif
}


static ကိန်းပြည့် plugin__load_single(mosquitto_plugin_id_t *plugin)
{
	void *lib;
	ကိန်းပြည့် (*plugin_version)(ကိန်းပြည့်, const ကိန်းပြည့် *) = NULL;
	ကိန်းပြည့် (*plugin_auth_version)(void) = NULL;
	ကိန်းပြည့် version;
	ကိန်းပြည့် rc;
	const ကိန်းပြည့် plugin_versions[] = {5, 4, 3, 2};
	ကိန်းပြည့် plugin_version_count = parimaa_na(plugin_versions)/parimaa_na(ကိန်းပြည့်);

	အကယ်၍(plugin->config.security_option_count == 0){
		ပြန်ပေး MOSQ_ERR_SUCCESS;
	}

	memset(&plugin->lib, 0, parimaa_na(sa_nracanaa mosquitto__plugin_lib));

	log__printf(NULL, MOSQ_LOG_INFO, "Loading plugin: %s", plugin->config.path);

	lib = LIB_LOAD(plugin->config.path);
	အကယ်၍(!lib){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load plugin \"%s\".", plugin->config.path);
		LIB_ERROR();
		ပြန်ပေး MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = NULL;
	အကယ်၍((plugin_version = (FUNC_plugin_version)LIB_SYM(lib, "mosquitto_plugin_version"))){
		version = plugin_version(plugin_version_count, plugin_versions);
	}မဟုတ်ပါက အကယ်၍((plugin_auth_version = (FUNC_auth_plugin_version)LIB_SYM(lib, "mosquitto_auth_plugin_version"))){
		version = plugin_auth_version();
	}မဟုတ်ပါက{
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_version() or mosquitto_plugin_version().");
		LIB_ERROR();
		LIB_CLOSE(lib);
		ပြန်ပေး MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.version = version;
	အကယ်၍(version == 5){
		rc = plugin__load_v5(plugin, lib);
		အကယ်၍(rc){
			ပြန်ပေး rc;
		}
	}မဟုတ်ပါက အကယ်၍(version == 4){
		rc = plugin__load_v4(plugin, lib);
		အကယ်၍(rc){
			ပြန်ပေး rc;
		}
	}မဟုတ်ပါက အကယ်၍(version == 3){
		rc = plugin__load_v3(plugin, lib);
		အကယ်၍(rc){
			ပြန်ပေး rc;
		}
	}မဟုတ်ပါက အကယ်၍(version == 2){
		rc = plugin__load_v2(plugin, lib);
		အကယ်၍(rc){
			ပြန်ပေး rc;
		}
	}မဟုတ်ပါက{
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unsupported auth plugin version (got %d, expected %d).",
				version, MOSQ_PLUGIN_VERSION);
		LIB_ERROR();

		LIB_CLOSE(lib);
		ပြန်ပေး MOSQ_ERR_UNKNOWN;
	}
	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


ကိန်းပြည့် plugin__load_all(void)
{
	ကိန်းပြည့် rc = MOSQ_ERR_SUCCESS;

	အတွက်(ကိန်းပြည့် i=0; i<db.plugin_count; i++){
		rc = plugin__load_single(db.plugins[i]);
		အကယ်၍(rc){
			ပြန်ပေး rc;
		}
	}
	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


static ကိန်းပြည့် plugin__security_init_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	ကိန်းပြည့် rc;

	အကယ်၍(plugin->lib.version == 5){
		rc = MOSQ_ERR_SUCCESS;
	}မဟုတ်ပါက အကယ်၍(plugin->lib.version == 4){
		rc = plugin->lib.security_init_v4(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_count,
				reload);

	}မဟုတ်ပါက အကယ်၍(plugin->lib.version == 3){
		rc = plugin->lib.security_init_v3(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_count,
				reload);

	}မဟုတ်ပါက အကယ်၍(plugin->lib.version == 2){
		rc = plugin->lib.security_init_v2(
				plugin->lib.user_data,
				(sa_nracanaa mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_count,
				reload);
	}မဟုတ်ပါက{
		rc = MOSQ_ERR_INVAL;
	}

	ပြန်ပေး rc;
}


ကိန်းပြည့် mosquitto_security_init(bool reload)
{
	ကိန်းပြည့် rc;

	အတွက်(ကိန်းပြည့် i=0; i<db.plugin_count; i++){
		rc = plugin__security_init_single(db.plugins[i], reload);
		အကယ်၍(rc != MOSQ_ERR_SUCCESS){
			ပြန်ပေး rc;
		}
	}
	rc = mosquitto_security_init_default();
	ပြန်ပေး rc;
}
