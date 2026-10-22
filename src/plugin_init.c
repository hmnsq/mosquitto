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

#ضمن "config.h"

#ضمن <stdio.h>
#ضمن <string.h>

#ضمن "mosquitto/broker.h"
#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/broker_plugin.h"
#ضمن "lib_load.h"
#ضمن "utlist.h"

تعريفنوع صحيح (*FUNC_auth_plugin_نسخو)(فراغ);
تعريفنوع صحيح (*FUNC_plugin_نسخو)(صحيح, ثابت صحيح *);


فراغ LIB_ERROR(فراغ)
{
#جيڪڏهن_وصف_ٿيل WIN32
	حرف *buf;
	Format_پيغام(FORMAT_پيغام_ALLOCATE_بفر | FORMAT_پيغام_FROM_SYSTEM,
			عدم, Get_آخري_Error(), LANG_NEUTRAL, (LPTSTR)&buf, 0, عدم);
	log__printf(عدم, MOSQ_LOG_ERR, "Load error: %s", buf);
	LocalFree(buf);
#وإلا
	log__printf(عدم, MOSQ_LOG_ERR, "Load error: %s", dlerror());
#جيڪڏهن_پورو
}


ساكن صحيح plugin_ڀريو_single(mosquitto_plugin_id_t *plugin)
{
	فراغ *lib;
	صحيح (*plugin_نسخو)(صحيح, ثابت صحيح *) = عدم;
	صحيح (*plugin_auth_نسخو)(فراغ) = عدم;
	صحيح نسخو_2;
	صحيح rc;
	ثابت صحيح plugin_versions[] = {5, 4, 3, 2};
	صحيح plugin_نسخو_ڳڻپ = حجم(plugin_versions)/حجم(صحيح);

	إذا(plugin->config.security_option_ڳڻپ == 0){
		أرجع MOSQ_ERR_SUCCESS;
	}

	memset(&plugin->lib, 0, حجم(بنية mosquitto__plugin_lib));

	log__printf(عدم, MOSQ_LOG_INFO, "Loading plugin: %s", plugin->config.رستو);

	lib = LIB_ڀريو(plugin->config.رستو);
	إذا(!lib){
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unable to load plugin \"%s\".", plugin->config.رستو);
		LIB_ERROR();
		أرجع MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = عدم;
	إذا((plugin_نسخو = (FUNC_plugin_نسخو)LIB_SYM(lib, "mosquitto_plugin_version"))){
		نسخو_2 = plugin_نسخو(plugin_نسخو_ڳڻپ, plugin_versions);
	}وإلا إذا((plugin_auth_نسخو = (FUNC_auth_plugin_نسخو)LIB_SYM(lib, "mosquitto_auth_plugin_version"))){
		نسخو_2 = plugin_auth_نسخو();
	}وإلا{
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_version() or mosquitto_plugin_version().");
		LIB_ERROR();
		LIB_CLOSE(lib);
		أرجع MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.نسخو_2 = نسخو_2;
	إذا(نسخو_2 == 5){
		rc = plugin_ڀريو_v5(plugin, lib);
		إذا(rc){
			أرجع rc;
		}
	}وإلا إذا(نسخو_2 == 4){
		rc = plugin_ڀريو_v4(plugin, lib);
		إذا(rc){
			أرجع rc;
		}
	}وإلا إذا(نسخو_2 == 3){
		rc = plugin_ڀريو_v3(plugin, lib);
		إذا(rc){
			أرجع rc;
		}
	}وإلا إذا(نسخو_2 == 2){
		rc = plugin_ڀريو_v2(plugin, lib);
		إذا(rc){
			أرجع rc;
		}
	}وإلا{
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unsupported auth plugin version (got %d, expected %d).",
				نسخو_2, MOSQ_PLUGIN_نسخو);
		LIB_ERROR();

		LIB_CLOSE(lib);
		أرجع MOSQ_ERR_UNKNOWN;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح plugin_ڀريو_all(فراغ)
{
	صحيح rc = MOSQ_ERR_SUCCESS;

	لكل(صحيح i=0; i<db.plugin_ڳڻپ; i++){
		rc = plugin_ڀريو_single(db.plugins[i]);
		إذا(rc){
			أرجع rc;
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح plugin__security_init_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	صحيح rc;

	إذا(plugin->lib.نسخو_2 == 5){
		rc = MOSQ_ERR_SUCCESS;
	}وإلا إذا(plugin->lib.نسخو_2 == 4){
		rc = plugin->lib.security_init_v4(
				plugin->lib.user_معلومات,
				plugin->config.options,
				plugin->config.option_ڳڻپ,
				reload);

	}وإلا إذا(plugin->lib.نسخو_2 == 3){
		rc = plugin->lib.security_init_v3(
				plugin->lib.user_معلومات,
				plugin->config.options,
				plugin->config.option_ڳڻپ,
				reload);

	}وإلا إذا(plugin->lib.نسخو_2 == 2){
		rc = plugin->lib.security_init_v2(
				plugin->lib.user_معلومات,
				(بنية mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_ڳڻپ,
				reload);
	}وإلا{
		rc = MOSQ_ERR_INVAL;
	}

	أرجع rc;
}


صحيح mosquitto_security_init(bool reload)
{
	صحيح rc;

	لكل(صحيح i=0; i<db.plugin_ڳڻپ; i++){
		rc = plugin__security_init_single(db.plugins[i], reload);
		إذا(rc != MOSQ_ERR_SUCCESS){
			أرجع rc;
		}
	}
	rc = mosquitto_security_init_default();
	أرجع rc;
}
