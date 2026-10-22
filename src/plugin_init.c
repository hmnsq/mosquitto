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

typedef පූර්ණ_සංඛ්‍යා (*FUNC_auth_plugin_version)(void);
typedef පූර්ණ_සංඛ්‍යා (*FUNC_plugin_version)(පූර්ණ_සංඛ්‍යා, const පූර්ණ_සංඛ්‍යා *);


void LIB_ERROR(void)
{
#ifdef WIN32
	var_na *buf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, NULL);
	log__printf(NULL, MOSQ_LOG_ERR, "Load error: %s", buf);
	LocalFree(buf);
#නැතහොත්
	log__printf(NULL, MOSQ_LOG_ERR, "Load error: %s", dlerror());
#endif
}


static පූර්ණ_සංඛ්‍යා plugin__load_single(mosquitto_plugin_id_t *plugin)
{
	void *lib;
	පූර්ණ_සංඛ්‍යා (*plugin_version)(පූර්ණ_සංඛ්‍යා, const පූර්ණ_සංඛ්‍යා *) = NULL;
	පූර්ණ_සංඛ්‍යා (*plugin_auth_version)(void) = NULL;
	පූර්ණ_සංඛ්‍යා version;
	පූර්ණ_සංඛ්‍යා rc;
	const පූර්ණ_සංඛ්‍යා plugin_versions[] = {5, 4, 3, 2};
	පූර්ණ_සංඛ්‍යා plugin_version_count = parimaa_na(plugin_versions)/parimaa_na(පූර්ණ_සංඛ්‍යා);

	නම්(plugin->config.security_option_count == 0){
		ආපසු MOSQ_ERR_SUCCESS;
	}

	memset(&plugin->lib, 0, parimaa_na(sa_nracanaa mosquitto__plugin_lib));

	log__printf(NULL, MOSQ_LOG_INFO, "Loading plugin: %s", plugin->config.path);

	lib = LIB_LOAD(plugin->config.path);
	නම්(!lib){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load plugin \"%s\".", plugin->config.path);
		LIB_ERROR();
		ආපසු MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = NULL;
	නම්((plugin_version = (FUNC_plugin_version)LIB_SYM(lib, "mosquitto_plugin_version"))){
		version = plugin_version(plugin_version_count, plugin_versions);
	}නැතහොත් නම්((plugin_auth_version = (FUNC_auth_plugin_version)LIB_SYM(lib, "mosquitto_auth_plugin_version"))){
		version = plugin_auth_version();
	}නැතහොත්{
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_version() or mosquitto_plugin_version().");
		LIB_ERROR();
		LIB_CLOSE(lib);
		ආපසු MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.version = version;
	නම්(version == 5){
		rc = plugin__load_v5(plugin, lib);
		නම්(rc){
			ආපසු rc;
		}
	}නැතහොත් නම්(version == 4){
		rc = plugin__load_v4(plugin, lib);
		නම්(rc){
			ආපසු rc;
		}
	}නැතහොත් නම්(version == 3){
		rc = plugin__load_v3(plugin, lib);
		නම්(rc){
			ආපසු rc;
		}
	}නැතහොත් නම්(version == 2){
		rc = plugin__load_v2(plugin, lib);
		නම්(rc){
			ආපසු rc;
		}
	}නැතහොත්{
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unsupported auth plugin version (got %d, expected %d).",
				version, MOSQ_PLUGIN_VERSION);
		LIB_ERROR();

		LIB_CLOSE(lib);
		ආපසු MOSQ_ERR_UNKNOWN;
	}
	ආපසු MOSQ_ERR_SUCCESS;
}


පූර්ණ_සංඛ්‍යා plugin__load_all(void)
{
	පූර්ණ_සංඛ්‍යා rc = MOSQ_ERR_SUCCESS;

	සඳහා(පූර්ණ_සංඛ්‍යා i=0; i<db.plugin_count; i++){
		rc = plugin__load_single(db.plugins[i]);
		නම්(rc){
			ආපසු rc;
		}
	}
	ආපසු MOSQ_ERR_SUCCESS;
}


static පූර්ණ_සංඛ්‍යා plugin__security_init_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	පූර්ණ_සංඛ්‍යා rc;

	නම්(plugin->lib.version == 5){
		rc = MOSQ_ERR_SUCCESS;
	}නැතහොත් නම්(plugin->lib.version == 4){
		rc = plugin->lib.security_init_v4(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_count,
				reload);

	}නැතහොත් නම්(plugin->lib.version == 3){
		rc = plugin->lib.security_init_v3(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_count,
				reload);

	}නැතහොත් නම්(plugin->lib.version == 2){
		rc = plugin->lib.security_init_v2(
				plugin->lib.user_data,
				(sa_nracanaa mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_count,
				reload);
	}නැතහොත්{
		rc = MOSQ_ERR_INVAL;
	}

	ආපසු rc;
}


පූර්ණ_සංඛ්‍යා mosquitto_security_init(bool reload)
{
	පූර්ණ_සංඛ්‍යා rc;

	සඳහා(පූර්ණ_සංඛ්‍යා i=0; i<db.plugin_count; i++){
		rc = plugin__security_init_single(db.plugins[i], reload);
		නම්(rc != MOSQ_ERR_SUCCESS){
			ආපසු rc;
		}
	}
	rc = mosquitto_security_init_default();
	ආපසු rc;
}
