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

अटल पूर्णांक plugin__security_cleanup_single(mosquitto_plugin_id_t *plugin, bool reload);


अटल व्योम plugin__unload_single(mosquitto_plugin_id_t *plugin)
{
	काष्ठा control_endpoint *ep, *tmp;

	/* Run plugin cleanup function */
	अगर(plugin->lib.version == 5){
		अगर(plugin->lib.plugin_cleanup_v5){
			plugin->lib.plugin_cleanup_v5(
					plugin->lib.user_तथ्य,
					plugin->config.options,
					plugin->config.option_गिनती);
		}
	}अन्यथा अगर(plugin->lib.version == 4){
		अगर(plugin->lib.plugin_cleanup_v4){
			plugin->lib.plugin_cleanup_v4(
					plugin->lib.user_तथ्य,
					plugin->config.options,
					plugin->config.option_गिनती);
		}
	}अन्यथा अगर(plugin->lib.version == 3){
		अगर(plugin->lib.plugin_cleanup_v3){
			plugin->lib.plugin_cleanup_v3(
					plugin->lib.user_तथ्य,
					plugin->config.options,
					plugin->config.option_गिनती);
		}
	}अन्यथा अगर(plugin->lib.version == 2){
		अगर(plugin->lib.plugin_cleanup_v2){
			plugin->lib.plugin_cleanup_v2(
					plugin->lib.user_तथ्य,
					(काष्ठा mosquitto_auth_opt *)plugin->config.options,
					plugin->config.option_गिनती);
		}
	}

	plugin__callback_unregister_all(plugin);
	mosquitto_FREE(plugin->plugin_name);
	mosquitto_FREE(plugin->plugin_version);
	DL_FOREACH_SAFE(plugin->control_endpoints, ep, tmp){
		DL_DELETE(plugin->control_endpoints, ep);
		mosquitto_FREE(ep);
	}

	अगर(plugin->lib.lib){
		//LIB_CLOSE(plugin->lib.lib);
	}
	स_रखो(&plugin->lib, 0, माप(काष्ठा mosquitto__plugin_lib));
}


पूर्णांक plugin__unload_all(व्योम)
{
	क्रम(पूर्णांक i=0; i<db.plugin_गिनती; i++){
		plugin__unload_single(db.plugins[i]);
	}

	mosquitto_security_cleanup(false);

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक plugin__security_cleanup_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	पूर्णांक rc;

	अगर(plugin->lib.version == 5){
		rc = MOSQ_ERR_SUCCESS;
	}अन्यथा अगर(plugin->lib.version == 4){
		rc = plugin->lib.security_cleanup_v4(
				plugin->lib.user_तथ्य,
				plugin->config.options,
				plugin->config.option_गिनती,
				reload);

	}अन्यथा अगर(plugin->lib.version == 3){
		rc = plugin->lib.security_cleanup_v3(
				plugin->lib.user_तथ्य,
				plugin->config.options,
				plugin->config.option_गिनती,
				reload);

	}अन्यथा अगर(plugin->lib.version == 2){
		rc = plugin->lib.security_cleanup_v2(
				plugin->lib.user_तथ्य,
				(काष्ठा mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_गिनती,
				reload);
	}अन्यथा{
		rc = MOSQ_ERR_INVAL;
	}

	वापस rc;
}


पूर्णांक mosquitto_security_cleanup(bool reload)
{
	क्रम(पूर्णांक i=0; i<db.plugin_गिनती; i++){
		plugin__security_cleanup_single(db.plugins[i], reload);
	}

	वापस mosquitto_security_cleanup_default();
}
