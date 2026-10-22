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

#شامل "config.h"

#شامل <stdio.h>
#شامل <string.h>

#شامل "mosquitto/broker.h"
#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/broker_plugin.h"
#شامل "lib_load.h"
#شامل "utlist.h"

تعریفنوع صحیح (*FUNC_auth_plugin_version)(تهی);
تعریفنوع صحیح (*FUNC_plugin_version)(صحیح, ثابت صحیح *);


تهی LIB_ERROR(تهی)
{
#ifdef WIN32
	نویسه *buf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			پوچ, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, پوچ);
	log__printf(پوچ, MOSQ_LOG_ERR, "Load error: %s", buf);
	LocalFree(buf);
#وگرنه
	log__printf(پوچ, MOSQ_LOG_ERR, "Load error: %s", dlerror());
#endif
}


ایستا صحیح plugin__load_single(mosquitto_plugin_id_t *plugin)
{
	تهی *lib;
	صحیح (*plugin_version)(صحیح, ثابت صحیح *) = پوچ;
	صحیح (*plugin_auth_version)(تهی) = پوچ;
	صحیح version;
	صحیح rc;
	ثابت صحیح plugin_versions[] = {5, 4, 3, 2};
	صحیح plugin_version_شمار = اندازه(plugin_versions)/اندازه(صحیح);

	اگر(plugin->config.security_option_شمار == 0){
		بازگشت MOSQ_ERR_SUCCESS;
	}

	memset(&plugin->lib, 0, اندازه(ساختار mosquitto__plugin_lib));

	log__printf(پوچ, MOSQ_LOG_INFO, "Loading plugin: %s", plugin->config.path);

	lib = LIB_LOAD(plugin->config.path);
	اگر(!lib){
		log__printf(پوچ, MOSQ_LOG_ERR,
				"Error: Unable to load plugin \"%s\".", plugin->config.path);
		LIB_ERROR();
		بازگشت MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = پوچ;
	اگر((plugin_version = (FUNC_plugin_version)LIB_SYM(lib, "mosquitto_plugin_version"))){
		version = plugin_version(plugin_version_شمار, plugin_versions);
	}وگرنه اگر((plugin_auth_version = (FUNC_auth_plugin_version)LIB_SYM(lib, "mosquitto_auth_plugin_version"))){
		version = plugin_auth_version();
	}وگرنه{
		log__printf(پوچ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_version() or mosquitto_plugin_version().");
		LIB_ERROR();
		LIB_CLOSE(lib);
		بازگشت MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.version = version;
	اگر(version == 5){
		rc = plugin__load_v5(plugin, lib);
		اگر(rc){
			بازگشت rc;
		}
	}وگرنه اگر(version == 4){
		rc = plugin__load_v4(plugin, lib);
		اگر(rc){
			بازگشت rc;
		}
	}وگرنه اگر(version == 3){
		rc = plugin__load_v3(plugin, lib);
		اگر(rc){
			بازگشت rc;
		}
	}وگرنه اگر(version == 2){
		rc = plugin__load_v2(plugin, lib);
		اگر(rc){
			بازگشت rc;
		}
	}وگرنه{
		log__printf(پوچ, MOSQ_LOG_ERR,
				"Error: Unsupported auth plugin version (got %d, expected %d).",
				version, MOSQ_PLUGIN_VERSION);
		LIB_ERROR();

		LIB_CLOSE(lib);
		بازگشت MOSQ_ERR_UNKNOWN;
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح plugin__load_all(تهی)
{
	صحیح rc = MOSQ_ERR_SUCCESS;

	برای(صحیح i=0; i<db.plugin_شمار; i++){
		rc = plugin__load_single(db.plugins[i]);
		اگر(rc){
			بازگشت rc;
		}
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح plugin__security_init_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	صحیح rc;

	اگر(plugin->lib.version == 5){
		rc = MOSQ_ERR_SUCCESS;
	}وگرنه اگر(plugin->lib.version == 4){
		rc = plugin->lib.security_init_v4(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_شمار,
				reload);

	}وگرنه اگر(plugin->lib.version == 3){
		rc = plugin->lib.security_init_v3(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_شمار,
				reload);

	}وگرنه اگر(plugin->lib.version == 2){
		rc = plugin->lib.security_init_v2(
				plugin->lib.user_data,
				(ساختار mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_شمار,
				reload);
	}وگرنه{
		rc = MOSQ_ERR_INVAL;
	}

	بازگشت rc;
}


صحیح mosquitto_security_init(bool reload)
{
	صحیح rc;

	برای(صحیح i=0; i<db.plugin_شمار; i++){
		rc = plugin__security_init_single(db.plugins[i], reload);
		اگر(rc != MOSQ_ERR_SUCCESS){
			بازگشت rc;
		}
	}
	rc = mosquitto_security_init_default();
	بازگشت rc;
}
