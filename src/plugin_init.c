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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "mosquitto/broker.h"
#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/broker_plugin.h"
#ಅನ್ತರ್ಭಾವ "lib_load.h"
#ಅನ್ತರ್ಭಾವ "utlist.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_auth_plugin_version)(ಶೂನ್ಯ);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_plugin_version)(ಪೂರ್ಣಾಙ್ಕ, ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ *);


ಶೂನ್ಯ LIB_ERROR(ಶೂನ್ಯ)
{
#ifdef WIN32
	ವರ್ಣ *buf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			ರಿಕ್ತ, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, ರಿಕ್ತ);
	log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Load error: %s", buf);
	LocalFree(buf);
#ಅನ್ಯಥಾ
	log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Load error: %s", dlerror());
#endif
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ plugin__load_single(mosquitto_plugin_id_t *plugin)
{
	ಶೂನ್ಯ *lib;
	ಪೂರ್ಣಾಙ್ಕ (*plugin_version)(ಪೂರ್ಣಾಙ್ಕ, ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ *) = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ (*plugin_auth_version)(ಶೂನ್ಯ) = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ version;
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ plugin_versions[] = {5, 4, 3, 2};
	ಪೂರ್ಣಾಙ್ಕ plugin_version_ಎಣಿಕೆ = ಪರಿಮಾಣ(plugin_versions)/ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ);

	ಯದಿ(plugin->config.security_option_ಎಣಿಕೆ == 0){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}

	memset(&plugin->lib, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__plugin_lib));

	log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Loading plugin: %s", plugin->config.path);

	lib = LIB_LOAD(plugin->config.path);
	ಯದಿ(!lib){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR,
				"Error: Unable to load plugin \"%s\".", plugin->config.path);
		LIB_ERROR();
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = ರಿಕ್ತ;
	ಯದಿ((plugin_version = (FUNC_plugin_version)LIB_SYM(lib, "mosquitto_plugin_version"))){
		version = plugin_version(plugin_version_ಎಣಿಕೆ, plugin_versions);
	}ಅನ್ಯಥಾ ಯದಿ((plugin_auth_version = (FUNC_auth_plugin_version)LIB_SYM(lib, "mosquitto_auth_plugin_version"))){
		version = plugin_auth_version();
	}ಅನ್ಯಥಾ{
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_version() or mosquitto_plugin_version().");
		LIB_ERROR();
		LIB_CLOSE(lib);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.version = version;
	ಯದಿ(version == 5){
		rc = plugin__load_v5(plugin, lib);
		ಯದಿ(rc){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}ಅನ್ಯಥಾ ಯದಿ(version == 4){
		rc = plugin__load_v4(plugin, lib);
		ಯದಿ(rc){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}ಅನ್ಯಥಾ ಯದಿ(version == 3){
		rc = plugin__load_v3(plugin, lib);
		ಯದಿ(rc){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}ಅನ್ಯಥಾ ಯದಿ(version == 2){
		rc = plugin__load_v2(plugin, lib);
		ಯದಿ(rc){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}ಅನ್ಯಥಾ{
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR,
				"Error: Unsupported auth plugin version (got %d, expected %d).",
				version, MOSQ_PLUGIN_VERSION);
		LIB_ERROR();

		LIB_CLOSE(lib);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ plugin__load_all(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಙ್ಕ rc = MOSQ_ERR_SUCCESS;

	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<db.plugin_ಎಣಿಕೆ; i++){
		rc = plugin__load_single(db.plugins[i]);
		ಯದಿ(rc){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ plugin__security_init_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ(plugin->lib.version == 5){
		rc = MOSQ_ERR_SUCCESS;
	}ಅನ್ಯಥಾ ಯದಿ(plugin->lib.version == 4){
		rc = plugin->lib.security_init_v4(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_ಎಣಿಕೆ,
				reload);

	}ಅನ್ಯಥಾ ಯದಿ(plugin->lib.version == 3){
		rc = plugin->lib.security_init_v3(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_ಎಣಿಕೆ,
				reload);

	}ಅನ್ಯಥಾ ಯದಿ(plugin->lib.version == 2){
		rc = plugin->lib.security_init_v2(
				plugin->lib.user_data,
				(ಸಂರಚನಾ mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_ಎಣಿಕೆ,
				reload);
	}ಅನ್ಯಥಾ{
		rc = MOSQ_ERR_INVAL;
	}

	ಪ್ರತ್ಯಾಗಮ rc;
}


ಪೂರ್ಣಾಙ್ಕ mosquitto_security_init(bool reload)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<db.plugin_ಎಣಿಕೆ; i++){
		rc = plugin__security_init_single(db.plugins[i], reload);
		ಯದಿ(rc != MOSQ_ERR_SUCCESS){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}
	rc = mosquitto_security_init_default();
	ಪ್ರತ್ಯಾಗಮ rc;
}
