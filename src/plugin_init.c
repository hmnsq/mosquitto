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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "mosquitto/broker.h"
#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto/broker_plugin.h"
#অন্তর্ভাব "lib_load.h"
#অন্তর্ভাব "utlist.h"

প্রকারলেখ পূর্ণসংখ্যা (*FUNC_auth_plugin_version)(শূন্য);
প্রকারলেখ পূর্ণসংখ্যা (*FUNC_plugin_version)(পূর্ণসংখ্যা, ধ্রুবক পূর্ণসংখ্যা *);


শূন্য LIB_ERROR(শূন্য)
{
#ifdef WIN32
	অক্ষর *buf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_বাফার | FORMAT_MESSAGE_FROM_SYSTEM,
			ফাঁকা, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, ফাঁকা);
	log__printf(ফাঁকা, MOSQ_LOG_ERR, "Load error: %s", buf);
	LocalFree(buf);
#অন্যথা
	log__printf(ফাঁকা, MOSQ_LOG_ERR, "Load error: %s", dlerror());
#endif
}


স্থির পূর্ণসংখ্যা plugin__load_single(mosquitto_plugin_id_t *plugin)
{
	শূন্য *lib;
	পূর্ণসংখ্যা (*plugin_version)(পূর্ণসংখ্যা, ধ্রুবক পূর্ণসংখ্যা *) = ফাঁকা;
	পূর্ণসংখ্যা (*plugin_auth_version)(শূন্য) = ফাঁকা;
	পূর্ণসংখ্যা version;
	পূর্ণসংখ্যা rc;
	ধ্রুবক পূর্ণসংখ্যা plugin_versions[] = {5, 4, 3, 2};
	পূর্ণসংখ্যা plugin_version_গুনতি = আকার(plugin_versions)/আকার(পূর্ণসংখ্যা);

	যদি(plugin->config.security_option_গুনতি == 0){
		ফেরত MOSQ_ERR_SUCCESS;
	}

	memset(&plugin->lib, 0, আকার(কাঠামো mosquitto__plugin_lib));

	log__printf(ফাঁকা, MOSQ_LOG_INFO, "Loading plugin: %s", plugin->config.path);

	lib = LIB_LOAD(plugin->config.path);
	যদি(!lib){
		log__printf(ফাঁকা, MOSQ_LOG_ERR,
				"Error: Unable to load plugin \"%s\".", plugin->config.path);
		LIB_ERROR();
		ফেরত MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = ফাঁকা;
	যদি((plugin_version = (FUNC_plugin_version)LIB_SYM(lib, "mosquitto_plugin_version"))){
		version = plugin_version(plugin_version_গুনতি, plugin_versions);
	}অন্যথা যদি((plugin_auth_version = (FUNC_auth_plugin_version)LIB_SYM(lib, "mosquitto_auth_plugin_version"))){
		version = plugin_auth_version();
	}অন্যথা{
		log__printf(ফাঁকা, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_version() or mosquitto_plugin_version().");
		LIB_ERROR();
		LIB_CLOSE(lib);
		ফেরত MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.version = version;
	যদি(version == 5){
		rc = plugin__load_v5(plugin, lib);
		যদি(rc){
			ফেরত rc;
		}
	}অন্যথা যদি(version == 4){
		rc = plugin__load_v4(plugin, lib);
		যদি(rc){
			ফেরত rc;
		}
	}অন্যথা যদি(version == 3){
		rc = plugin__load_v3(plugin, lib);
		যদি(rc){
			ফেরত rc;
		}
	}অন্যথা যদি(version == 2){
		rc = plugin__load_v2(plugin, lib);
		যদি(rc){
			ফেরত rc;
		}
	}অন্যথা{
		log__printf(ফাঁকা, MOSQ_LOG_ERR,
				"Error: Unsupported auth plugin version (got %d, expected %d).",
				version, MOSQ_PLUGIN_VERSION);
		LIB_ERROR();

		LIB_CLOSE(lib);
		ফেরত MOSQ_ERR_UNKNOWN;
	}
	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা plugin__load_all(শূন্য)
{
	পূর্ণসংখ্যা rc = MOSQ_ERR_SUCCESS;

	জন্য(পূর্ণসংখ্যা i=0; i<db.plugin_গুনতি; i++){
		rc = plugin__load_single(db.plugins[i]);
		যদি(rc){
			ফেরত rc;
		}
	}
	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা plugin__security_init_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	পূর্ণসংখ্যা rc;

	যদি(plugin->lib.version == 5){
		rc = MOSQ_ERR_SUCCESS;
	}অন্যথা যদি(plugin->lib.version == 4){
		rc = plugin->lib.security_init_v4(
				plugin->lib.user_তথ্য,
				plugin->config.options,
				plugin->config.option_গুনতি,
				reload);

	}অন্যথা যদি(plugin->lib.version == 3){
		rc = plugin->lib.security_init_v3(
				plugin->lib.user_তথ্য,
				plugin->config.options,
				plugin->config.option_গুনতি,
				reload);

	}অন্যথা যদি(plugin->lib.version == 2){
		rc = plugin->lib.security_init_v2(
				plugin->lib.user_তথ্য,
				(কাঠামো mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_গুনতি,
				reload);
	}অন্যথা{
		rc = MOSQ_ERR_INVAL;
	}

	ফেরত rc;
}


পূর্ণসংখ্যা mosquitto_security_init(bool reload)
{
	পূর্ণসংখ্যা rc;

	জন্য(পূর্ণসংখ্যা i=0; i<db.plugin_গুনতি; i++){
		rc = plugin__security_init_single(db.plugins[i], reload);
		যদি(rc != MOSQ_ERR_SUCCESS){
			ফেরত rc;
		}
	}
	rc = mosquitto_security_init_default();
	ফেরত rc;
}
