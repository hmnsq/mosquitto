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

تعريفنوع صحيح (*FUNC_auth_plugin_نسخه)(فراغ);
تعريفنوع صحيح (*FUNC_plugin_نسخه)(صحيح, ثابت صحيح *);


فراغ LIB_ERROR(فراغ)
{
#اگر_تعریف_شده WIN32
	حرف *buf;
	Format_پیام(FORMAT_پیام_ALLOCATE_میانگیر | FORMAT_پیام_FROM_SYSTEM,
			عدم, Get_آخرین_Error(), LANG_NEUTRAL, (LPTSTR)&buf, 0, عدم);
	log__printf(عدم, MOSQ_LOG_ERR, "Load error: %s", buf);
	LocalFree(buf);
#وإلا
	log__printf(عدم, MOSQ_LOG_ERR, "Load error: %s", dlerror());
#پایان_اگر
}


ساكن صحيح plugin_بارگیری_single(mosquitto_plugin_id_t *plugin)
{
	فراغ *lib;
	صحيح (*plugin_نسخه)(صحيح, ثابت صحيح *) = عدم;
	صحيح (*plugin_auth_نسخه)(فراغ) = عدم;
	صحيح نسخه_2;
	صحيح rc;
	ثابت صحيح plugin_versions[] = {5, 4, 3, 2};
	صحيح plugin_نسخه_شمار = حجم(plugin_versions)/حجم(صحيح);

	إذا(plugin->config.security_option_شمار == 0){
		أرجع MOSQ_ERR_SUCCESS;
	}

	memset(&plugin->lib, 0, حجم(بنية mosquitto__plugin_lib));

	log__printf(عدم, MOSQ_LOG_INFO, "Loading plugin: %s", plugin->config.مسیر);

	lib = LIB_بارگیری(plugin->config.مسیر);
	إذا(!lib){
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unable to load plugin \"%s\".", plugin->config.مسیر);
		LIB_ERROR();
		أرجع MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = عدم;
	إذا((plugin_نسخه = (FUNC_plugin_نسخه)LIB_SYM(lib, "mosquitto_plugin_version"))){
		نسخه_2 = plugin_نسخه(plugin_نسخه_شمار, plugin_versions);
	}وإلا إذا((plugin_auth_نسخه = (FUNC_auth_plugin_نسخه)LIB_SYM(lib, "mosquitto_auth_plugin_version"))){
		نسخه_2 = plugin_auth_نسخه();
	}وإلا{
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_version() or mosquitto_plugin_version().");
		LIB_ERROR();
		LIB_CLOSE(lib);
		أرجع MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.نسخه_2 = نسخه_2;
	إذا(نسخه_2 == 5){
		rc = plugin_بارگیری_v5(plugin, lib);
		إذا(rc){
			أرجع rc;
		}
	}وإلا إذا(نسخه_2 == 4){
		rc = plugin_بارگیری_v4(plugin, lib);
		إذا(rc){
			أرجع rc;
		}
	}وإلا إذا(نسخه_2 == 3){
		rc = plugin_بارگیری_v3(plugin, lib);
		إذا(rc){
			أرجع rc;
		}
	}وإلا إذا(نسخه_2 == 2){
		rc = plugin_بارگیری_v2(plugin, lib);
		إذا(rc){
			أرجع rc;
		}
	}وإلا{
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unsupported auth plugin version (got %d, expected %d).",
				نسخه_2, MOSQ_PLUGIN_نسخه);
		LIB_ERROR();

		LIB_CLOSE(lib);
		أرجع MOSQ_ERR_UNKNOWN;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح plugin_بارگیری_all(فراغ)
{
	صحيح rc = MOSQ_ERR_SUCCESS;

	لكل(صحيح i=0; i<db.plugin_شمار; i++){
		rc = plugin_بارگیری_single(db.plugins[i]);
		إذا(rc){
			أرجع rc;
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح plugin__security_init_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	صحيح rc;

	إذا(plugin->lib.نسخه_2 == 5){
		rc = MOSQ_ERR_SUCCESS;
	}وإلا إذا(plugin->lib.نسخه_2 == 4){
		rc = plugin->lib.security_init_v4(
				plugin->lib.user_داده,
				plugin->config.options,
				plugin->config.option_شمار,
				reload);

	}وإلا إذا(plugin->lib.نسخه_2 == 3){
		rc = plugin->lib.security_init_v3(
				plugin->lib.user_داده,
				plugin->config.options,
				plugin->config.option_شمار,
				reload);

	}وإلا إذا(plugin->lib.نسخه_2 == 2){
		rc = plugin->lib.security_init_v2(
				plugin->lib.user_داده,
				(بنية mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_شمار,
				reload);
	}وإلا{
		rc = MOSQ_ERR_INVAL;
	}

	أرجع rc;
}


صحيح mosquitto_security_init(bool reload)
{
	صحيح rc;

	لكل(صحيح i=0; i<db.plugin_شمار; i++){
		rc = plugin__security_init_single(db.plugins[i], reload);
		إذا(rc != MOSQ_ERR_SUCCESS){
			أرجع rc;
		}
	}
	rc = mosquitto_security_init_default();
	أرجع rc;
}
