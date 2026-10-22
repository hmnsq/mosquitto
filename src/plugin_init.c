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

प्रकारनिर्देश पूर्णाङ्क (*FUNC_auth_plugin_संस्करणम्)(शून्य);
प्रकारनिर्देश पूर्णाङ्क (*FUNC_plugin_संस्करणम्)(पूर्णाङ्क, ध्रुव पूर्णाङ्क *);


शून्य LIB_ERROR(शून्य)
{
#यदि_परिभाषितम् WIN32
	वर्ण *buf;
	Format_सन्देशः(FORMAT_सन्देशः_ALLOCATE_बफर | FORMAT_सन्देशः_FROM_SYSTEM,
			रिक्त, Get_चरमः_Error(), LANG_NEUTRAL, (LPTSTR)&buf, 0, रिक्त);
	log__printf(रिक्त, MOSQ_LOG_ERR, "Load error: %s", buf);
	LocalFree(buf);
#अन्यथा
	log__printf(रिक्त, MOSQ_LOG_ERR, "Load error: %s", dlerror());
#यदि_अन्तः
}


स्थिर पूर्णाङ्क plugin_आरोपय_single(mosquitto_plugin_id_t *plugin)
{
	शून्य *lib;
	पूर्णाङ्क (*plugin_संस्करणम्)(पूर्णाङ्क, ध्रुव पूर्णाङ्क *) = रिक्त;
	पूर्णाङ्क (*plugin_auth_संस्करणम्)(शून्य) = रिक्त;
	पूर्णाङ्क संस्करणम्_2;
	पूर्णाङ्क rc;
	ध्रुव पूर्णाङ्क plugin_versions[] = {5, 4, 3, 2};
	पूर्णाङ्क plugin_संस्करणम्_गणनम् = परिमाण(plugin_versions)/परिमाण(पूर्णाङ्क);

	यदि(plugin->config.security_option_गणनम् == 0){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	स्मृतिं_पूरय(&plugin->lib, 0, परिमाण(संरचना mosquitto__plugin_lib));

	log__printf(रिक्त, MOSQ_LOG_INFO, "Loading plugin: %s", plugin->config.मार्गः);

	lib = LIB_आरोपय(plugin->config.मार्गः);
	यदि(!lib){
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unable to load plugin \"%s\".", plugin->config.मार्गः);
		LIB_ERROR();
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = रिक्त;
	यदि((plugin_संस्करणम् = (FUNC_plugin_संस्करणम्)LIB_SYM(lib, "mosquitto_plugin_version"))){
		संस्करणम्_2 = plugin_संस्करणम्(plugin_संस्करणम्_गणनम्, plugin_versions);
	}अन्यथा यदि((plugin_auth_संस्करणम् = (FUNC_auth_plugin_संस्करणम्)LIB_SYM(lib, "mosquitto_auth_plugin_version"))){
		संस्करणम्_2 = plugin_auth_संस्करणम्();
	}अन्यथा{
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_version() or mosquitto_plugin_version().");
		LIB_ERROR();
		LIB_CLOSE(lib);
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.संस्करणम्_2 = संस्करणम्_2;
	यदि(संस्करणम्_2 == 5){
		rc = plugin_आरोपय_v5(plugin, lib);
		यदि(rc){
			प्रत्यागम rc;
		}
	}अन्यथा यदि(संस्करणम्_2 == 4){
		rc = plugin_आरोपय_v4(plugin, lib);
		यदि(rc){
			प्रत्यागम rc;
		}
	}अन्यथा यदि(संस्करणम्_2 == 3){
		rc = plugin_आरोपय_v3(plugin, lib);
		यदि(rc){
			प्रत्यागम rc;
		}
	}अन्यथा यदि(संस्करणम्_2 == 2){
		rc = plugin_आरोपय_v2(plugin, lib);
		यदि(rc){
			प्रत्यागम rc;
		}
	}अन्यथा{
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Unsupported auth plugin version (got %d, expected %d).",
				संस्करणम्_2, MOSQ_PLUGIN_संस्करणम्);
		LIB_ERROR();

		LIB_CLOSE(lib);
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क plugin_आरोपय_all(शून्य)
{
	पूर्णाङ्क rc = MOSQ_ERR_SUCCESS;

	प्रति(पूर्णाङ्क i=0; i<db.plugin_गणनम्; i++){
		rc = plugin_आरोपय_single(db.plugins[i]);
		यदि(rc){
			प्रत्यागम rc;
		}
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क plugin__security_init_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	पूर्णाङ्क rc;

	यदि(plugin->lib.संस्करणम्_2 == 5){
		rc = MOSQ_ERR_SUCCESS;
	}अन्यथा यदि(plugin->lib.संस्करणम्_2 == 4){
		rc = plugin->lib.security_init_v4(
				plugin->lib.user_दत्त,
				plugin->config.options,
				plugin->config.option_गणनम्,
				reload);

	}अन्यथा यदि(plugin->lib.संस्करणम्_2 == 3){
		rc = plugin->lib.security_init_v3(
				plugin->lib.user_दत्त,
				plugin->config.options,
				plugin->config.option_गणनम्,
				reload);

	}अन्यथा यदि(plugin->lib.संस्करणम्_2 == 2){
		rc = plugin->lib.security_init_v2(
				plugin->lib.user_दत्त,
				(संरचना mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_गणनम्,
				reload);
	}अन्यथा{
		rc = MOSQ_ERR_INVAL;
	}

	प्रत्यागम rc;
}


पूर्णाङ्क mosquitto_security_init(bool reload)
{
	पूर्णाङ्क rc;

	प्रति(पूर्णाङ्क i=0; i<db.plugin_गणनम्; i++){
		rc = plugin__security_init_single(db.plugins[i], reload);
		यदि(rc != MOSQ_ERR_SUCCESS){
			प्रत्यागम rc;
		}
	}
	rc = mosquitto_security_init_default();
	प्रत्यागम rc;
}
