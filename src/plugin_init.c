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

قسمتعریف صحیح (*FUNC_auth_plugin_نسخہ)(خالی);
قسمتعریف صحیح (*FUNC_plugin_نسخہ)(صحیح, ثابت صحیح *);


خالی LIB_ERROR(خالی)
{
#اگر_تعریف_شدہ WIN32
	حرف *buf;
	Format_پیغام(FORMAT_پیغام_ALLOCATE_بفر | FORMAT_پیغام_FROM_SYSTEM,
			تہی, Get_آخری_Error(), LANG_NEUTRAL, (LPTSTR)&buf, 0, تہی);
	log__printf(تہی, MOSQ_LOG_ERR, "Load error: %s", buf);
	LocalFree(buf);
#ورنہ
	log__printf(تہی, MOSQ_LOG_ERR, "Load error: %s", dlerror());
#اگر_ختم
}


ساکن صحیح plugin_بھرو_single(mosquitto_plugin_id_t *plugin)
{
	خالی *lib;
	صحیح (*plugin_نسخہ)(صحیح, ثابت صحیح *) = تہی;
	صحیح (*plugin_auth_نسخہ)(خالی) = تہی;
	صحیح نسخہ_2;
	صحیح rc;
	ثابت صحیح plugin_versions[] = {5, 4, 3, 2};
	صحیح plugin_نسخہ_گنتی = حجم(plugin_versions)/حجم(صحیح);

	اگر(plugin->config.security_option_گنتی == 0){
		واپس MOSQ_ERR_SUCCESS;
	}

	memset(&plugin->lib, 0, حجم(ساخت mosquitto__plugin_lib));

	log__printf(تہی, MOSQ_LOG_INFO, "Loading plugin: %s", plugin->config.راستہ);

	lib = LIB_بھرو(plugin->config.راستہ);
	اگر(!lib){
		log__printf(تہی, MOSQ_LOG_ERR,
				"Error: Unable to load plugin \"%s\".", plugin->config.راستہ);
		LIB_ERROR();
		واپس MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = تہی;
	اگر((plugin_نسخہ = (FUNC_plugin_نسخہ)LIB_SYM(lib, "mosquitto_plugin_version"))){
		نسخہ_2 = plugin_نسخہ(plugin_نسخہ_گنتی, plugin_versions);
	}ورنہ اگر((plugin_auth_نسخہ = (FUNC_auth_plugin_نسخہ)LIB_SYM(lib, "mosquitto_auth_plugin_version"))){
		نسخہ_2 = plugin_auth_نسخہ();
	}ورنہ{
		log__printf(تہی, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_version() or mosquitto_plugin_version().");
		LIB_ERROR();
		LIB_CLOSE(lib);
		واپس MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.نسخہ_2 = نسخہ_2;
	اگر(نسخہ_2 == 5){
		rc = plugin_بھرو_v5(plugin, lib);
		اگر(rc){
			واپس rc;
		}
	}ورنہ اگر(نسخہ_2 == 4){
		rc = plugin_بھرو_v4(plugin, lib);
		اگر(rc){
			واپس rc;
		}
	}ورنہ اگر(نسخہ_2 == 3){
		rc = plugin_بھرو_v3(plugin, lib);
		اگر(rc){
			واپس rc;
		}
	}ورنہ اگر(نسخہ_2 == 2){
		rc = plugin_بھرو_v2(plugin, lib);
		اگر(rc){
			واپس rc;
		}
	}ورنہ{
		log__printf(تہی, MOSQ_LOG_ERR,
				"Error: Unsupported auth plugin version (got %d, expected %d).",
				نسخہ_2, MOSQ_PLUGIN_نسخہ);
		LIB_ERROR();

		LIB_CLOSE(lib);
		واپس MOSQ_ERR_UNKNOWN;
	}
	واپس MOSQ_ERR_SUCCESS;
}


صحیح plugin_بھرو_all(خالی)
{
	صحیح rc = MOSQ_ERR_SUCCESS;

	برائے(صحیح i=0; i<db.plugin_گنتی; i++){
		rc = plugin_بھرو_single(db.plugins[i]);
		اگر(rc){
			واپس rc;
		}
	}
	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح plugin__security_init_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	صحیح rc;

	اگر(plugin->lib.نسخہ_2 == 5){
		rc = MOSQ_ERR_SUCCESS;
	}ورنہ اگر(plugin->lib.نسخہ_2 == 4){
		rc = plugin->lib.security_init_v4(
				plugin->lib.user_ڈیٹا,
				plugin->config.options,
				plugin->config.option_گنتی,
				reload);

	}ورنہ اگر(plugin->lib.نسخہ_2 == 3){
		rc = plugin->lib.security_init_v3(
				plugin->lib.user_ڈیٹا,
				plugin->config.options,
				plugin->config.option_گنتی,
				reload);

	}ورنہ اگر(plugin->lib.نسخہ_2 == 2){
		rc = plugin->lib.security_init_v2(
				plugin->lib.user_ڈیٹا,
				(ساخت mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_گنتی,
				reload);
	}ورنہ{
		rc = MOSQ_ERR_INVAL;
	}

	واپس rc;
}


صحیح mosquitto_security_init(bool reload)
{
	صحیح rc;

	برائے(صحیح i=0; i<db.plugin_گنتی; i++){
		rc = plugin__security_init_single(db.plugins[i], reload);
		اگر(rc != MOSQ_ERR_SUCCESS){
			واپس rc;
		}
	}
	rc = mosquitto_security_init_default();
	واپس rc;
}
