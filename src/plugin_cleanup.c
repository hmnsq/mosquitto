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

स्थिर पूर्णांक plugin__security_cleanup_single(mosquitto_plugin_id_t *plugin, bool reload);


स्थिर शून्य plugin__unload_single(mosquitto_plugin_id_t *plugin)
{
	संरचना control_endpoint *ep, *tmp;

	/* Run plugin cleanup function */
	जर(plugin->lib.version == 5){
		जर(plugin->lib.plugin_cleanup_v5){
			plugin->lib.plugin_cleanup_v5(
					plugin->lib.user_data,
					plugin->config.options,
					plugin->config.option_मोजणी);
		}
	}अन्यथा जर(plugin->lib.version == 4){
		जर(plugin->lib.plugin_cleanup_v4){
			plugin->lib.plugin_cleanup_v4(
					plugin->lib.user_data,
					plugin->config.options,
					plugin->config.option_मोजणी);
		}
	}अन्यथा जर(plugin->lib.version == 3){
		जर(plugin->lib.plugin_cleanup_v3){
			plugin->lib.plugin_cleanup_v3(
					plugin->lib.user_data,
					plugin->config.options,
					plugin->config.option_मोजणी);
		}
	}अन्यथा जर(plugin->lib.version == 2){
		जर(plugin->lib.plugin_cleanup_v2){
			plugin->lib.plugin_cleanup_v2(
					plugin->lib.user_data,
					(संरचना mosquitto_auth_opt *)plugin->config.options,
					plugin->config.option_मोजणी);
		}
	}

	plugin__callback_unregister_all(plugin);
	mosquitto_FREE(plugin->plugin_name);
	mosquitto_FREE(plugin->plugin_version);
	DL_FOREACH_SAFE(plugin->control_endpoints, ep, tmp){
		DL_DELETE(plugin->control_endpoints, ep);
		mosquitto_FREE(ep);
	}

	जर(plugin->lib.lib){
		//LIB_CLOSE(plugin->lib.lib);
	}
	स_रखो(&plugin->lib, 0, आकार(संरचना mosquitto__plugin_lib));
}


पूर्णांक plugin__unload_all(शून्य)
{
	साठी(पूर्णांक i=0; i<db.plugin_मोजणी; i++){
		plugin__unload_single(db.plugins[i]);
	}

	mosquitto_security_cleanup(false);

	परत MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णांक plugin__security_cleanup_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	पूर्णांक rc;

	जर(plugin->lib.version == 5){
		rc = MOSQ_ERR_SUCCESS;
	}अन्यथा जर(plugin->lib.version == 4){
		rc = plugin->lib.security_cleanup_v4(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_मोजणी,
				reload);

	}अन्यथा जर(plugin->lib.version == 3){
		rc = plugin->lib.security_cleanup_v3(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_मोजणी,
				reload);

	}अन्यथा जर(plugin->lib.version == 2){
		rc = plugin->lib.security_cleanup_v2(
				plugin->lib.user_data,
				(संरचना mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_मोजणी,
				reload);
	}अन्यथा{
		rc = MOSQ_ERR_INVAL;
	}

	परत rc;
}


पूर्णांक mosquitto_security_cleanup(bool reload)
{
	साठी(पूर्णांक i=0; i<db.plugin_मोजणी; i++){
		plugin__security_cleanup_single(db.plugins[i], reload);
	}

	परत mosquitto_security_cleanup_default();
}
