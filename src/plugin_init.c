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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/broker.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/broker_plugin.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "lib_load.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utlist.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_auth_plugin_ਸੰਸਕਰਣ)(ਖਾਲੀ);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_plugin_ਸੰਸਕਰਣ)(ਪੂਰਨ, ਅਚਲ ਪੂਰਨ *);


ਖਾਲੀ LIB_ERROR(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
	ਅੱਖਰ *buf;
	Format_ਸੁਨੇਹਾ(FORMAT_ਸੁਨੇਹਾ_ALLOCATE_ਬਫਰ | FORMAT_ਸੁਨੇਹਾ_FROM_SYSTEM,
			ਨਿਰਾ, Get_ਆਖਰੀ_Error(), LANG_NEUTRAL, (LPTSTR)&buf, 0, ਨਿਰਾ);
	log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Load error: %s", buf);
	LocalFree(buf);
#ਨਹੀਂਤਾਂ
	log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Load error: %s", dlerror());
#ਜੇ_ਅੰਤ
}


ਸਥਿਰ ਪੂਰਨ plugin_ਲੱਦੋ_single(mosquitto_plugin_id_t *plugin)
{
	ਖਾਲੀ *lib;
	ਪੂਰਨ (*plugin_ਸੰਸਕਰਣ)(ਪੂਰਨ, ਅਚਲ ਪੂਰਨ *) = ਨਿਰਾ;
	ਪੂਰਨ (*plugin_auth_ਸੰਸਕਰਣ)(ਖਾਲੀ) = ਨਿਰਾ;
	ਪੂਰਨ ਸੰਸਕਰਣ_2;
	ਪੂਰਨ rc;
	ਅਚਲ ਪੂਰਨ plugin_versions[] = {5, 4, 3, 2};
	ਪੂਰਨ plugin_ਸੰਸਕਰਣ_ਗਿਣਤੀ = ਆਕਾਰ(plugin_versions)/ਆਕਾਰ(ਪੂਰਨ);

	ਜੇ(plugin->config.security_option_ਗਿਣਤੀ == 0){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

	memset(&plugin->lib, 0, ਆਕਾਰ(ਬਣਤਰ mosquitto__plugin_lib));

	log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Loading plugin: %s", plugin->config.ਰਸਤਾ);

	lib = LIB_ਲੱਦੋ(plugin->config.ਰਸਤਾ);
	ਜੇ(!lib){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR,
				"Error: Unable to load plugin \"%s\".", plugin->config.ਰਸਤਾ);
		LIB_ERROR();
		ਵਾਪਸ MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = ਨਿਰਾ;
	ਜੇ((plugin_ਸੰਸਕਰਣ = (FUNC_plugin_ਸੰਸਕਰਣ)LIB_SYM(lib, "mosquitto_plugin_version"))){
		ਸੰਸਕਰਣ_2 = plugin_ਸੰਸਕਰਣ(plugin_ਸੰਸਕਰਣ_ਗਿਣਤੀ, plugin_versions);
	}ਨਹੀਂਤਾਂ ਜੇ((plugin_auth_ਸੰਸਕਰਣ = (FUNC_auth_plugin_ਸੰਸਕਰਣ)LIB_SYM(lib, "mosquitto_auth_plugin_version"))){
		ਸੰਸਕਰਣ_2 = plugin_auth_ਸੰਸਕਰਣ();
	}ਨਹੀਂਤਾਂ{
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_version() or mosquitto_plugin_version().");
		LIB_ERROR();
		LIB_CLOSE(lib);
		ਵਾਪਸ MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.ਸੰਸਕਰਣ_2 = ਸੰਸਕਰਣ_2;
	ਜੇ(ਸੰਸਕਰਣ_2 == 5){
		rc = plugin_ਲੱਦੋ_v5(plugin, lib);
		ਜੇ(rc){
			ਵਾਪਸ rc;
		}
	}ਨਹੀਂਤਾਂ ਜੇ(ਸੰਸਕਰਣ_2 == 4){
		rc = plugin_ਲੱਦੋ_v4(plugin, lib);
		ਜੇ(rc){
			ਵਾਪਸ rc;
		}
	}ਨਹੀਂਤਾਂ ਜੇ(ਸੰਸਕਰਣ_2 == 3){
		rc = plugin_ਲੱਦੋ_v3(plugin, lib);
		ਜੇ(rc){
			ਵਾਪਸ rc;
		}
	}ਨਹੀਂਤਾਂ ਜੇ(ਸੰਸਕਰਣ_2 == 2){
		rc = plugin_ਲੱਦੋ_v2(plugin, lib);
		ਜੇ(rc){
			ਵਾਪਸ rc;
		}
	}ਨਹੀਂਤਾਂ{
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR,
				"Error: Unsupported auth plugin version (got %d, expected %d).",
				ਸੰਸਕਰਣ_2, MOSQ_PLUGIN_ਸੰਸਕਰਣ);
		LIB_ERROR();

		LIB_CLOSE(lib);
		ਵਾਪਸ MOSQ_ERR_UNKNOWN;
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ plugin_ਲੱਦੋ_all(ਖਾਲੀ)
{
	ਪੂਰਨ rc = MOSQ_ERR_SUCCESS;

	ਲਈ(ਪੂਰਨ i=0; i<db.plugin_ਗਿਣਤੀ; i++){
		rc = plugin_ਲੱਦੋ_single(db.plugins[i]);
		ਜੇ(rc){
			ਵਾਪਸ rc;
		}
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ plugin__security_init_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	ਪੂਰਨ rc;

	ਜੇ(plugin->lib.ਸੰਸਕਰਣ_2 == 5){
		rc = MOSQ_ERR_SUCCESS;
	}ਨਹੀਂਤਾਂ ਜੇ(plugin->lib.ਸੰਸਕਰਣ_2 == 4){
		rc = plugin->lib.security_init_v4(
				plugin->lib.user_ਅੰਕੜੇ,
				plugin->config.options,
				plugin->config.option_ਗਿਣਤੀ,
				reload);

	}ਨਹੀਂਤਾਂ ਜੇ(plugin->lib.ਸੰਸਕਰਣ_2 == 3){
		rc = plugin->lib.security_init_v3(
				plugin->lib.user_ਅੰਕੜੇ,
				plugin->config.options,
				plugin->config.option_ਗਿਣਤੀ,
				reload);

	}ਨਹੀਂਤਾਂ ਜੇ(plugin->lib.ਸੰਸਕਰਣ_2 == 2){
		rc = plugin->lib.security_init_v2(
				plugin->lib.user_ਅੰਕੜੇ,
				(ਬਣਤਰ mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_ਗਿਣਤੀ,
				reload);
	}ਨਹੀਂਤਾਂ{
		rc = MOSQ_ERR_INVAL;
	}

	ਵਾਪਸ rc;
}


ਪੂਰਨ mosquitto_security_init(bool reload)
{
	ਪੂਰਨ rc;

	ਲਈ(ਪੂਰਨ i=0; i<db.plugin_ਗਿਣਤੀ; i++){
		rc = plugin__security_init_single(db.plugins[i], reload);
		ਜੇ(rc != MOSQ_ERR_SUCCESS){
			ਵਾਪਸ rc;
		}
	}
	rc = mosquitto_security_init_default();
	ਵਾਪਸ rc;
}
