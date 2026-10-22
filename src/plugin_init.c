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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>

#அந்தர்பாவ "mosquitto/broker.h"
#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto/broker_plugin.h"
#அந்தர்பாவ "lib_load.h"
#அந்தர்பாவ "utlist.h"

வகைவரை முழு (*FUNC_auth_plugin_version)(வெறுமை);
வகைவரை முழு (*FUNC_plugin_version)(முழு, மாறிலி முழு *);


வெறுமை LIB_ERROR(வெறுமை)
{
#ifdef WIN32
	எழுத்து *buf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			வெற்று, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, வெற்று);
	log__printf(வெற்று, MOSQ_LOG_ERR, "Load error: %s", buf);
	LocalFree(buf);
#இல்லையெனில்
	log__printf(வெற்று, MOSQ_LOG_ERR, "Load error: %s", dlerror());
#endif
}


நிலையான முழு plugin__load_single(mosquitto_plugin_id_t *plugin)
{
	வெறுமை *lib;
	முழு (*plugin_version)(முழு, மாறிலி முழு *) = வெற்று;
	முழு (*plugin_auth_version)(வெறுமை) = வெற்று;
	முழு version;
	முழு rc;
	மாறிலி முழு plugin_versions[] = {5, 4, 3, 2};
	முழு plugin_version_மொத்தம் = அளவு(plugin_versions)/அளவு(முழு);

	எனில்(plugin->config.security_option_மொத்தம் == 0){
		திரும்பு MOSQ_ERR_SUCCESS;
	}

	memset(&plugin->lib, 0, அளவு(கட்டமைப்பு mosquitto__plugin_lib));

	log__printf(வெற்று, MOSQ_LOG_INFO, "Loading plugin: %s", plugin->config.path);

	lib = LIB_LOAD(plugin->config.path);
	எனில்(!lib){
		log__printf(வெற்று, MOSQ_LOG_ERR,
				"Error: Unable to load plugin \"%s\".", plugin->config.path);
		LIB_ERROR();
		திரும்பு MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = வெற்று;
	எனில்((plugin_version = (FUNC_plugin_version)LIB_SYM(lib, "mosquitto_plugin_version"))){
		version = plugin_version(plugin_version_மொத்தம், plugin_versions);
	}இல்லையெனில் எனில்((plugin_auth_version = (FUNC_auth_plugin_version)LIB_SYM(lib, "mosquitto_auth_plugin_version"))){
		version = plugin_auth_version();
	}இல்லையெனில்{
		log__printf(வெற்று, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_version() or mosquitto_plugin_version().");
		LIB_ERROR();
		LIB_CLOSE(lib);
		திரும்பு MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.version = version;
	எனில்(version == 5){
		rc = plugin__load_v5(plugin, lib);
		எனில்(rc){
			திரும்பு rc;
		}
	}இல்லையெனில் எனில்(version == 4){
		rc = plugin__load_v4(plugin, lib);
		எனில்(rc){
			திரும்பு rc;
		}
	}இல்லையெனில் எனில்(version == 3){
		rc = plugin__load_v3(plugin, lib);
		எனில்(rc){
			திரும்பு rc;
		}
	}இல்லையெனில் எனில்(version == 2){
		rc = plugin__load_v2(plugin, lib);
		எனில்(rc){
			திரும்பு rc;
		}
	}இல்லையெனில்{
		log__printf(வெற்று, MOSQ_LOG_ERR,
				"Error: Unsupported auth plugin version (got %d, expected %d).",
				version, MOSQ_PLUGIN_VERSION);
		LIB_ERROR();

		LIB_CLOSE(lib);
		திரும்பு MOSQ_ERR_UNKNOWN;
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு plugin__load_all(வெறுமை)
{
	முழு rc = MOSQ_ERR_SUCCESS;

	க்கு(முழு i=0; i<db.plugin_மொத்தம்; i++){
		rc = plugin__load_single(db.plugins[i]);
		எனில்(rc){
			திரும்பு rc;
		}
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு plugin__security_init_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	முழு rc;

	எனில்(plugin->lib.version == 5){
		rc = MOSQ_ERR_SUCCESS;
	}இல்லையெனில் எனில்(plugin->lib.version == 4){
		rc = plugin->lib.security_init_v4(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_மொத்தம்,
				reload);

	}இல்லையெனில் எனில்(plugin->lib.version == 3){
		rc = plugin->lib.security_init_v3(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_மொத்தம்,
				reload);

	}இல்லையெனில் எனில்(plugin->lib.version == 2){
		rc = plugin->lib.security_init_v2(
				plugin->lib.user_data,
				(கட்டமைப்பு mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_மொத்தம்,
				reload);
	}இல்லையெனில்{
		rc = MOSQ_ERR_INVAL;
	}

	திரும்பு rc;
}


முழு mosquitto_security_init(bool reload)
{
	முழு rc;

	க்கு(முழு i=0; i<db.plugin_மொத்தம்; i++){
		rc = plugin__security_init_single(db.plugins[i], reload);
		எனில்(rc != MOSQ_ERR_SUCCESS){
			திரும்பு rc;
		}
	}
	rc = mosquitto_security_init_default();
	திரும்பு rc;
}
