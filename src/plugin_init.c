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

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*FUNC_auth_plugin_ಆವೃತ್ತಿ)(ಶೂನ್ಯ);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*FUNC_plugin_ಆವೃತ್ತಿ)(ಪೂರ್ಣಾಂಕ, ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ *);


ಶೂನ್ಯ LIB_ERROR(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
	ಅಕ್ಷರ *buf;
	Format_ಸಂದೇಶ(FORMAT_ಸಂದೇಶ_ALLOCATE_ಬಫರ್ | FORMAT_ಸಂದೇಶ_FROM_SYSTEM,
			ಖಾಲಿ, Get_ಕೊನೆಯ_Error(), LANG_NEUTRAL, (LPTSTR)&buf, 0, ಖಾಲಿ);
	log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Load error: %s", buf);
	LocalFree(buf);
#ಇಲ್ಲದಿದ್ದರೆ
	log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Load error: %s", dlerror());
#ಆದರೆ_ಅಂತ್ಯ
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ plugin_ತುಂಬು_single(mosquitto_plugin_id_t *plugin)
{
	ಶೂನ್ಯ *lib;
	ಪೂರ್ಣಾಂಕ (*plugin_ಆವೃತ್ತಿ)(ಪೂರ್ಣಾಂಕ, ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ *) = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ (*plugin_auth_ಆವೃತ್ತಿ)(ಶೂನ್ಯ) = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ ಆವೃತ್ತಿ_2;
	ಪೂರ್ಣಾಂಕ rc;
	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ plugin_versions[] = {5, 4, 3, 2};
	ಪೂರ್ಣಾಂಕ plugin_ಆವೃತ್ತಿ_ಎಣಿಕೆ = ಗಾತ್ರ(plugin_versions)/ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ);

	ಆದರೆ(plugin->config.security_option_ಎಣಿಕೆ == 0){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}

	memset(&plugin->lib, 0, ಗಾತ್ರ(ರಚನೆ mosquitto__plugin_lib));

	log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Loading plugin: %s", plugin->config.ಮಾರ್ಗ);

	lib = LIB_ತುಂಬು(plugin->config.ಮಾರ್ಗ);
	ಆದರೆ(!lib){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR,
				"Error: Unable to load plugin \"%s\".", plugin->config.ಮಾರ್ಗ);
		LIB_ERROR();
		ಹಿಂತಿರುಗಿ MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = ಖಾಲಿ;
	ಆದರೆ((plugin_ಆವೃತ್ತಿ = (FUNC_plugin_ಆವೃತ್ತಿ)LIB_SYM(lib, "mosquitto_plugin_version"))){
		ಆವೃತ್ತಿ_2 = plugin_ಆವೃತ್ತಿ(plugin_ಆವೃತ್ತಿ_ಎಣಿಕೆ, plugin_versions);
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((plugin_auth_ಆವೃತ್ತಿ = (FUNC_auth_plugin_ಆವೃತ್ತಿ)LIB_SYM(lib, "mosquitto_auth_plugin_version"))){
		ಆವೃತ್ತಿ_2 = plugin_auth_ಆವೃತ್ತಿ();
	}ಇಲ್ಲದಿದ್ದರೆ{
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_version() or mosquitto_plugin_version().");
		LIB_ERROR();
		LIB_CLOSE(lib);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.ಆವೃತ್ತಿ_2 = ಆವೃತ್ತಿ_2;
	ಆದರೆ(ಆವೃತ್ತಿ_2 == 5){
		rc = plugin_ತುಂಬು_v5(plugin, lib);
		ಆದರೆ(rc){
			ಹಿಂತಿರುಗಿ rc;
		}
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಆವೃತ್ತಿ_2 == 4){
		rc = plugin_ತುಂಬು_v4(plugin, lib);
		ಆದರೆ(rc){
			ಹಿಂತಿರುಗಿ rc;
		}
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಆವೃತ್ತಿ_2 == 3){
		rc = plugin_ತುಂಬು_v3(plugin, lib);
		ಆದರೆ(rc){
			ಹಿಂತಿರುಗಿ rc;
		}
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಆವೃತ್ತಿ_2 == 2){
		rc = plugin_ತುಂಬು_v2(plugin, lib);
		ಆದರೆ(rc){
			ಹಿಂತಿರುಗಿ rc;
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR,
				"Error: Unsupported auth plugin version (got %d, expected %d).",
				ಆವೃತ್ತಿ_2, MOSQ_PLUGIN_ಆವೃತ್ತಿ);
		LIB_ERROR();

		LIB_CLOSE(lib);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_UNKNOWN;
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ plugin_ತುಂಬು_all(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ rc = MOSQ_ERR_SUCCESS;

	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<db.plugin_ಎಣಿಕೆ; i++){
		rc = plugin_ತುಂಬು_single(db.plugins[i]);
		ಆದರೆ(rc){
			ಹಿಂತಿರುಗಿ rc;
		}
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ plugin__security_init_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ(plugin->lib.ಆವೃತ್ತಿ_2 == 5){
		rc = MOSQ_ERR_SUCCESS;
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(plugin->lib.ಆವೃತ್ತಿ_2 == 4){
		rc = plugin->lib.security_init_v4(
				plugin->lib.user_ದತ್ತಾಂಶ,
				plugin->config.options,
				plugin->config.option_ಎಣಿಕೆ,
				reload);

	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(plugin->lib.ಆವೃತ್ತಿ_2 == 3){
		rc = plugin->lib.security_init_v3(
				plugin->lib.user_ದತ್ತಾಂಶ,
				plugin->config.options,
				plugin->config.option_ಎಣಿಕೆ,
				reload);

	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(plugin->lib.ಆವೃತ್ತಿ_2 == 2){
		rc = plugin->lib.security_init_v2(
				plugin->lib.user_ದತ್ತಾಂಶ,
				(ರಚನೆ mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_ಎಣಿಕೆ,
				reload);
	}ಇಲ್ಲದಿದ್ದರೆ{
		rc = MOSQ_ERR_INVAL;
	}

	ಹಿಂತಿರುಗಿ rc;
}


ಪೂರ್ಣಾಂಕ mosquitto_security_init(bool reload)
{
	ಪೂರ್ಣಾಂಕ rc;

	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<db.plugin_ಎಣಿಕೆ; i++){
		rc = plugin__security_init_single(db.plugins[i], reload);
		ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
			ಹಿಂತಿರುಗಿ rc;
		}
	}
	rc = mosquitto_security_init_default();
	ಹಿಂತಿರುಗಿ rc;
}
