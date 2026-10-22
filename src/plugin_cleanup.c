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

स्थिर पूर्णाङ्क plugin__security_cleanup_single(mosquitto_plugin_id_t *plugin, bool reload);


स्थिर शून्य plugin__unload_single(mosquitto_plugin_id_t *plugin)
{
	संरचना control_endpoint *ep, *tmp;

	/* Run plugin cleanup function */
	जइ(plugin->lib.version == 5){
		जइ(plugin->lib.plugin_cleanup_v5){
			plugin->lib.plugin_cleanup_v5(
					plugin->lib.user_data,
					plugin->config.options,
					plugin->config.option_गणनम्);
		}
	}अण्णहा जइ(plugin->lib.version == 4){
		जइ(plugin->lib.plugin_cleanup_v4){
			plugin->lib.plugin_cleanup_v4(
					plugin->lib.user_data,
					plugin->config.options,
					plugin->config.option_गणनम्);
		}
	}अण्णहा जइ(plugin->lib.version == 3){
		जइ(plugin->lib.plugin_cleanup_v3){
			plugin->lib.plugin_cleanup_v3(
					plugin->lib.user_data,
					plugin->config.options,
					plugin->config.option_गणनम्);
		}
	}अण्णहा जइ(plugin->lib.version == 2){
		जइ(plugin->lib.plugin_cleanup_v2){
			plugin->lib.plugin_cleanup_v2(
					plugin->lib.user_data,
					(संरचना mosquitto_auth_opt *)plugin->config.options,
					plugin->config.option_गणनम्);
		}
	}

	plugin__callback_unregister_all(plugin);
	mosquitto_FREE(plugin->plugin_name);
	mosquitto_FREE(plugin->plugin_version);
	DL_FOREACH_SAFE(plugin->control_endpoints, ep, tmp){
		DL_DELETE(plugin->control_endpoints, ep);
		mosquitto_FREE(ep);
	}

	जइ(plugin->lib.lib){
		//LIB_CLOSE(plugin->lib.lib);
	}
	memset(&plugin->lib, 0, परिमाण(संरचना mosquitto__plugin_lib));
}


पूर्णाङ्क plugin__unload_all(शून्य)
{
	प्रति(पूर्णाङ्क i=0; i<db.plugin_गणनम्; i++){
		plugin__unload_single(db.plugins[i]);
	}

	mosquitto_security_cleanup(false);

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क plugin__security_cleanup_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	पूर्णाङ्क rc;

	जइ(plugin->lib.version == 5){
		rc = MOSQ_ERR_SUCCESS;
	}अण्णहा जइ(plugin->lib.version == 4){
		rc = plugin->lib.security_cleanup_v4(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_गणनम्,
				reload);

	}अण्णहा जइ(plugin->lib.version == 3){
		rc = plugin->lib.security_cleanup_v3(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_गणनम्,
				reload);

	}अण्णहा जइ(plugin->lib.version == 2){
		rc = plugin->lib.security_cleanup_v2(
				plugin->lib.user_data,
				(संरचना mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_गणनम्,
				reload);
	}अण्णहा{
		rc = MOSQ_ERR_INVAL;
	}

	प्रत्यागम rc;
}


पूर्णाङ्क mosquitto_security_cleanup(bool reload)
{
	प्रति(पूर्णाङ्क i=0; i<db.plugin_गणनम्; i++){
		plugin__security_cleanup_single(db.plugins[i], reload);
	}

	प्रत्यागम mosquitto_security_cleanup_default();
}
