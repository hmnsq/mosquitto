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

تعريفنوع صحيح (*FUNC_auth_plugin_نسخہ)(فراغ);
تعريفنوع صحيح (*FUNC_plugin_نسخہ)(صحيح, ثابت صحيح *);


فراغ LIB_ERROR(فراغ)
{
#جے_تعریف_شدہ WIN32
	حرف *buf;
	Format_سنیہا(FORMAT_سنیہا_ALLOCATE_بفر | FORMAT_سنیہا_FROM_SYSTEM,
			عدم, Get_آخری_Error(), LANG_NEUTRAL, (LPTSTR)&buf, 0, عدم);
	log__printf(عدم, MOSQ_LOG_ERR, "Load error: %s", buf);
	LocalFree(buf);
#وإلا
	log__printf(عدم, MOSQ_LOG_ERR, "Load error: %s", dlerror());
#جے_انت
}


ساكن صحيح plugin_لدو_single(mosquitto_plugin_id_t *plugin)
{
	فراغ *lib;
	صحيح (*plugin_نسخہ)(صحيح, ثابت صحيح *) = عدم;
	صحيح (*plugin_auth_نسخہ)(فراغ) = عدم;
	صحيح نسخہ_2;
	صحيح rc;
	ثابت صحيح plugin_versions[] = {5, 4, 3, 2};
	صحيح plugin_نسخہ_گنتی = حجم(plugin_versions)/حجم(صحيح);

	إذا(plugin->config.security_option_گنتی == 0){
		أرجع MOSQ_ERR_SUCCESS;
	}

	memset(&plugin->lib, 0, حجم(بنية mosquitto__plugin_lib));

	log__printf(عدم, MOSQ_LOG_INFO, "Loading plugin: %s", plugin->config.رستہ);

	lib = LIB_لدو(plugin->config.رستہ);
	إذا(!lib){
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unable to load plugin \"%s\".", plugin->config.رستہ);
		LIB_ERROR();
		أرجع MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = عدم;
	إذا((plugin_نسخہ = (FUNC_plugin_نسخہ)LIB_SYM(lib, "mosquitto_plugin_version"))){
		نسخہ_2 = plugin_نسخہ(plugin_نسخہ_گنتی, plugin_versions);
	}وإلا إذا((plugin_auth_نسخہ = (FUNC_auth_plugin_نسخہ)LIB_SYM(lib, "mosquitto_auth_plugin_version"))){
		نسخہ_2 = plugin_auth_نسخہ();
	}وإلا{
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_version() or mosquitto_plugin_version().");
		LIB_ERROR();
		LIB_CLOSE(lib);
		أرجع MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.نسخہ_2 = نسخہ_2;
	إذا(نسخہ_2 == 5){
		rc = plugin_لدو_v5(plugin, lib);
		إذا(rc){
			أرجع rc;
		}
	}وإلا إذا(نسخہ_2 == 4){
		rc = plugin_لدو_v4(plugin, lib);
		إذا(rc){
			أرجع rc;
		}
	}وإلا إذا(نسخہ_2 == 3){
		rc = plugin_لدو_v3(plugin, lib);
		إذا(rc){
			أرجع rc;
		}
	}وإلا إذا(نسخہ_2 == 2){
		rc = plugin_لدو_v2(plugin, lib);
		إذا(rc){
			أرجع rc;
		}
	}وإلا{
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unsupported auth plugin version (got %d, expected %d).",
				نسخہ_2, MOSQ_PLUGIN_نسخہ);
		LIB_ERROR();

		LIB_CLOSE(lib);
		أرجع MOSQ_ERR_UNKNOWN;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح plugin_لدو_all(فراغ)
{
	صحيح rc = MOSQ_ERR_SUCCESS;

	لكل(صحيح i=0; i<db.plugin_گنتی; i++){
		rc = plugin_لدو_single(db.plugins[i]);
		إذا(rc){
			أرجع rc;
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح plugin__security_init_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	صحيح rc;

	إذا(plugin->lib.نسخہ_2 == 5){
		rc = MOSQ_ERR_SUCCESS;
	}وإلا إذا(plugin->lib.نسخہ_2 == 4){
		rc = plugin->lib.security_init_v4(
				plugin->lib.user_انکڑے,
				plugin->config.options,
				plugin->config.option_گنتی,
				reload);

	}وإلا إذا(plugin->lib.نسخہ_2 == 3){
		rc = plugin->lib.security_init_v3(
				plugin->lib.user_انکڑے,
				plugin->config.options,
				plugin->config.option_گنتی,
				reload);

	}وإلا إذا(plugin->lib.نسخہ_2 == 2){
		rc = plugin->lib.security_init_v2(
				plugin->lib.user_انکڑے,
				(بنية mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_گنتی,
				reload);
	}وإلا{
		rc = MOSQ_ERR_INVAL;
	}

	أرجع rc;
}


صحيح mosquitto_security_init(bool reload)
{
	صحيح rc;

	لكل(صحيح i=0; i<db.plugin_گنتی; i++){
		rc = plugin__security_init_single(db.plugins[i], reload);
		إذا(rc != MOSQ_ERR_SUCCESS){
			أرجع rc;
		}
	}
	rc = mosquitto_security_init_default();
	أرجع rc;
}
