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

प्रकारव्याख्या पूर्णांक (*FUNC_auth_plugin_version)(शून्य);
प्रकारव्याख्या पूर्णांक (*FUNC_plugin_version)(पूर्णांक, स्थिरांक पूर्णांक *);


शून्य LIB_ERROR(शून्य)
{
#ifdef WIN32
	अक्षर *buf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			रिक्त, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, रिक्त);
	log__printf(रिक्त, MOSQ_LOG_ERR, "Load error: %s", buf);
	LocalFree(buf);
#अन्यथा
	log__printf(रिक्त, MOSQ_LOG_ERR, "Load error: %s", dlerror());
#endif
}


स्थिर पूर्णांक plugin__load_single(mosquitto_plugin_id_t *plugin)
{
	शून्य *lib;
	पूर्णांक (*plugin_version)(पूर्णांक, स्थिरांक पूर्णांक *) = रिक्त;
	पूर्णांक (*plugin_auth_version)(शून्य) = रिक्त;
	पूर्णांक version;
	पूर्णांक rc;
	स्थिरांक पूर्णांक plugin_versions[] = {5, 4, 3, 2};
	पूर्णांक plugin_version_मोजणी = आकार(plugin_versions)/आकार(पूर्णांक);

	जर(plugin->config.security_option_मोजणी == 0){
		परत MOSQ_ERR_SUCCESS;
	}

	स_रखो(&plugin->lib, 0, आकार(संरचना mosquitto__plugin_lib));

	log__printf(रिक्त, MOSQ_LOG_INFO, "Loading plugin: %s", plugin->config.path);

	lib = LIB_LOAD(plugin->config.path);
	जर(!lib){
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unable to load plugin \"%s\".", plugin->config.path);
		LIB_ERROR();
		परत MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = रिक्त;
	जर((plugin_version = (FUNC_plugin_version)LIB_SYM(lib, "mosquitto_plugin_version"))){
		version = plugin_version(plugin_version_मोजणी, plugin_versions);
	}अन्यथा जर((plugin_auth_version = (FUNC_auth_plugin_version)LIB_SYM(lib, "mosquitto_auth_plugin_version"))){
		version = plugin_auth_version();
	}अन्यथा{
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_version() or mosquitto_plugin_version().");
		LIB_ERROR();
		LIB_CLOSE(lib);
		परत MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.version = version;
	जर(version == 5){
		rc = plugin__load_v5(plugin, lib);
		जर(rc){
			परत rc;
		}
	}अन्यथा जर(version == 4){
		rc = plugin__load_v4(plugin, lib);
		जर(rc){
			परत rc;
		}
	}अन्यथा जर(version == 3){
		rc = plugin__load_v3(plugin, lib);
		जर(rc){
			परत rc;
		}
	}अन्यथा जर(version == 2){
		rc = plugin__load_v2(plugin, lib);
		जर(rc){
			परत rc;
		}
	}अन्यथा{
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unsupported auth plugin version (got %d, expected %d).",
				version, MOSQ_PLUGIN_VERSION);
		LIB_ERROR();

		LIB_CLOSE(lib);
		परत MOSQ_ERR_UNKNOWN;
	}
	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक plugin__load_all(शून्य)
{
	पूर्णांक rc = MOSQ_ERR_SUCCESS;

	साठी(पूर्णांक i=0; i<db.plugin_मोजणी; i++){
		rc = plugin__load_single(db.plugins[i]);
		जर(rc){
			परत rc;
		}
	}
	परत MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णांक plugin__security_init_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	पूर्णांक rc;

	जर(plugin->lib.version == 5){
		rc = MOSQ_ERR_SUCCESS;
	}अन्यथा जर(plugin->lib.version == 4){
		rc = plugin->lib.security_init_v4(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_मोजणी,
				reload);

	}अन्यथा जर(plugin->lib.version == 3){
		rc = plugin->lib.security_init_v3(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_मोजणी,
				reload);

	}अन्यथा जर(plugin->lib.version == 2){
		rc = plugin->lib.security_init_v2(
				plugin->lib.user_data,
				(संरचना mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_मोजणी,
				reload);
	}अन्यथा{
		rc = MOSQ_ERR_INVAL;
	}

	परत rc;
}


पूर्णांक mosquitto_security_init(bool reload)
{
	पूर्णांक rc;

	साठी(पूर्णांक i=0; i<db.plugin_मोजणी; i++){
		rc = plugin__security_init_single(db.plugins[i], reload);
		जर(rc != MOSQ_ERR_SUCCESS){
			परत rc;
		}
	}
	rc = mosquitto_security_init_default();
	परत rc;
}
