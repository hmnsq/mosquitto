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

#अन्तर्भाव "config.h"

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "mosquitto/broker.h"
#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/broker_plugin.h"
#अन्तर्भाव "lib_load.h"
#अन्तर्भाव "utlist.h"

प्रकारनिर्देश पूर्णाङ्क (*FUNC_auth_plugin_version)(शून्य);
प्रकारनिर्देश पूर्णाङ्क (*FUNC_plugin_version)(पूर्णाङ्क, ध्रुव पूर्णाङ्क *);


शून्य LIB_ERROR(शून्य)
{
#ifdef WIN32
	वर्ण *buf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			रिक्त, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, रिक्त);
	log__printf(रिक्त, MOSQ_LOG_ERR, "Load error: %s", buf);
	LocalFree(buf);
#अञ्ञथा
	log__printf(रिक्त, MOSQ_LOG_ERR, "Load error: %s", dlerror());
#endif
}


स्थिर पूर्णाङ्क plugin__load_single(mosquitto_plugin_id_t *plugin)
{
	शून्य *lib;
	पूर्णाङ्क (*plugin_version)(पूर्णाङ्क, ध्रुव पूर्णाङ्क *) = रिक्त;
	पूर्णाङ्क (*plugin_auth_version)(शून्य) = रिक्त;
	पूर्णाङ्क version;
	पूर्णाङ्क rc;
	ध्रुव पूर्णाङ्क plugin_versions[] = {5, 4, 3, 2};
	पूर्णाङ्क plugin_version_गणनम् = परिमाण(plugin_versions)/परिमाण(पूर्णाङ्क);

	सचे(plugin->config.security_option_गणनम् == 0){
		पटिनिवत्त MOSQ_ERR_SUCCESS;
	}

	memset(&plugin->lib, 0, परिमाण(संरचना mosquitto__plugin_lib));

	log__printf(रिक्त, MOSQ_LOG_INFO, "Loading plugin: %s", plugin->config.path);

	lib = LIB_LOAD(plugin->config.path);
	सचे(!lib){
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unable to load plugin \"%s\".", plugin->config.path);
		LIB_ERROR();
		पटिनिवत्त MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = रिक्त;
	सचे((plugin_version = (FUNC_plugin_version)LIB_SYM(lib, "mosquitto_plugin_version"))){
		version = plugin_version(plugin_version_गणनम्, plugin_versions);
	}अञ्ञथा सचे((plugin_auth_version = (FUNC_auth_plugin_version)LIB_SYM(lib, "mosquitto_auth_plugin_version"))){
		version = plugin_auth_version();
	}अञ्ञथा{
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_version() or mosquitto_plugin_version().");
		LIB_ERROR();
		LIB_CLOSE(lib);
		पटिनिवत्त MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.version = version;
	सचे(version == 5){
		rc = plugin__load_v5(plugin, lib);
		सचे(rc){
			पटिनिवत्त rc;
		}
	}अञ्ञथा सचे(version == 4){
		rc = plugin__load_v4(plugin, lib);
		सचे(rc){
			पटिनिवत्त rc;
		}
	}अञ्ञथा सचे(version == 3){
		rc = plugin__load_v3(plugin, lib);
		सचे(rc){
			पटिनिवत्त rc;
		}
	}अञ्ञथा सचे(version == 2){
		rc = plugin__load_v2(plugin, lib);
		सचे(rc){
			पटिनिवत्त rc;
		}
	}अञ्ञथा{
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unsupported auth plugin version (got %d, expected %d).",
				version, MOSQ_PLUGIN_VERSION);
		LIB_ERROR();

		LIB_CLOSE(lib);
		पटिनिवत्त MOSQ_ERR_UNKNOWN;
	}
	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क plugin__load_all(शून्य)
{
	पूर्णाङ्क rc = MOSQ_ERR_SUCCESS;

	अत्थाय(पूर्णाङ्क i=0; i<db.plugin_गणनम्; i++){
		rc = plugin__load_single(db.plugins[i]);
		सचे(rc){
			पटिनिवत्त rc;
		}
	}
	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क plugin__security_init_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	पूर्णाङ्क rc;

	सचे(plugin->lib.version == 5){
		rc = MOSQ_ERR_SUCCESS;
	}अञ्ञथा सचे(plugin->lib.version == 4){
		rc = plugin->lib.security_init_v4(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_गणनम्,
				reload);

	}अञ्ञथा सचे(plugin->lib.version == 3){
		rc = plugin->lib.security_init_v3(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_गणनम्,
				reload);

	}अञ्ञथा सचे(plugin->lib.version == 2){
		rc = plugin->lib.security_init_v2(
				plugin->lib.user_data,
				(संरचना mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_गणनम्,
				reload);
	}अञ्ञथा{
		rc = MOSQ_ERR_INVAL;
	}

	पटिनिवत्त rc;
}


पूर्णाङ्क mosquitto_security_init(bool reload)
{
	पूर्णाङ्क rc;

	अत्थाय(पूर्णाङ्क i=0; i<db.plugin_गणनम्; i++){
		rc = plugin__security_init_single(db.plugins[i], reload);
		सचे(rc != MOSQ_ERR_SUCCESS){
			पटिनिवत्त rc;
		}
	}
	rc = mosquitto_security_init_default();
	पटिनिवत्त rc;
}
