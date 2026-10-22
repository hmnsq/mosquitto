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

#समावेश "config.h"

#समावेश <stdio.h>
#समावेश <string.h>

#समावेश "mosquitto/broker.h"
#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/broker_plugin.h"
#समावेश "lib_load.h"
#समावेश "utlist.h"

प्रकार पूर्णांक (*FUNC_auth_plugin_version)(व्योम);
प्रकार पूर्णांक (*FUNC_plugin_version)(पूर्णांक, स्थिर पूर्णांक *);


व्योम LIB_ERROR(व्योम)
{
#अगर_घोषित WIN32
	अक्षर *buf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_बफर | FORMAT_MESSAGE_FROM_SYSTEM,
			शून्य, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, शून्य);
	log__printf(शून्य, MOSQ_LOG_ERR, "Load error: %s", buf);
	LocalFree(buf);
#अन्यथा
	log__printf(शून्य, MOSQ_LOG_ERR, "Load error: %s", dlerror());
#पूर्ण_अगर
}


अटल पूर्णांक plugin__load_single(mosquitto_plugin_id_t *plugin)
{
	व्योम *lib;
	पूर्णांक (*plugin_version)(पूर्णांक, स्थिर पूर्णांक *) = शून्य;
	पूर्णांक (*plugin_auth_version)(व्योम) = शून्य;
	पूर्णांक version;
	पूर्णांक rc;
	स्थिर पूर्णांक plugin_versions[] = {5, 4, 3, 2};
	पूर्णांक plugin_version_गिनती = माप(plugin_versions)/माप(पूर्णांक);

	अगर(plugin->config.security_option_गिनती == 0){
		वापस MOSQ_ERR_SUCCESS;
	}

	स_रखो(&plugin->lib, 0, माप(काष्ठा mosquitto__plugin_lib));

	log__printf(शून्य, MOSQ_LOG_INFO, "Loading plugin: %s", plugin->config.path);

	lib = LIB_LOAD(plugin->config.path);
	अगर(!lib){
		log__printf(शून्य, MOSQ_LOG_ERR,
				"Error: Unable to load plugin \"%s\".", plugin->config.path);
		LIB_ERROR();
		वापस MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = शून्य;
	अगर((plugin_version = (FUNC_plugin_version)LIB_SYM(lib, "mosquitto_plugin_version"))){
		version = plugin_version(plugin_version_गिनती, plugin_versions);
	}अन्यथा अगर((plugin_auth_version = (FUNC_auth_plugin_version)LIB_SYM(lib, "mosquitto_auth_plugin_version"))){
		version = plugin_auth_version();
	}अन्यथा{
		log__printf(शून्य, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_version() or mosquitto_plugin_version().");
		LIB_ERROR();
		LIB_CLOSE(lib);
		वापस MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.version = version;
	अगर(version == 5){
		rc = plugin__load_v5(plugin, lib);
		अगर(rc){
			वापस rc;
		}
	}अन्यथा अगर(version == 4){
		rc = plugin__load_v4(plugin, lib);
		अगर(rc){
			वापस rc;
		}
	}अन्यथा अगर(version == 3){
		rc = plugin__load_v3(plugin, lib);
		अगर(rc){
			वापस rc;
		}
	}अन्यथा अगर(version == 2){
		rc = plugin__load_v2(plugin, lib);
		अगर(rc){
			वापस rc;
		}
	}अन्यथा{
		log__printf(शून्य, MOSQ_LOG_ERR,
				"Error: Unsupported auth plugin version (got %d, expected %d).",
				version, MOSQ_PLUGIN_VERSION);
		LIB_ERROR();

		LIB_CLOSE(lib);
		वापस MOSQ_ERR_UNKNOWN;
	}
	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक plugin__load_all(व्योम)
{
	पूर्णांक rc = MOSQ_ERR_SUCCESS;

	क्रम(पूर्णांक i=0; i<db.plugin_गिनती; i++){
		rc = plugin__load_single(db.plugins[i]);
		अगर(rc){
			वापस rc;
		}
	}
	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक plugin__security_init_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	पूर्णांक rc;

	अगर(plugin->lib.version == 5){
		rc = MOSQ_ERR_SUCCESS;
	}अन्यथा अगर(plugin->lib.version == 4){
		rc = plugin->lib.security_init_v4(
				plugin->lib.user_तथ्य,
				plugin->config.options,
				plugin->config.option_गिनती,
				reload);

	}अन्यथा अगर(plugin->lib.version == 3){
		rc = plugin->lib.security_init_v3(
				plugin->lib.user_तथ्य,
				plugin->config.options,
				plugin->config.option_गिनती,
				reload);

	}अन्यथा अगर(plugin->lib.version == 2){
		rc = plugin->lib.security_init_v2(
				plugin->lib.user_तथ्य,
				(काष्ठा mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_गिनती,
				reload);
	}अन्यथा{
		rc = MOSQ_ERR_INVAL;
	}

	वापस rc;
}


पूर्णांक mosquitto_security_init(bool reload)
{
	पूर्णांक rc;

	क्रम(पूर्णांक i=0; i<db.plugin_गिनती; i++){
		rc = plugin__security_init_single(db.plugins[i], reload);
		अगर(rc != MOSQ_ERR_SUCCESS){
			वापस rc;
		}
	}
	rc = mosquitto_security_init_default();
	वापस rc;
}
