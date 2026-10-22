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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/broker.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/broker_plugin.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "lib_load.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utlist.h"

ਸਥਿਰ ਪੂਰਨ plugin__security_cleanup_single(mosquitto_plugin_id_t *plugin, bool reload);


ਸਥਿਰ ਖਾਲੀ plugin__unload_single(mosquitto_plugin_id_t *plugin)
{
	ਬਣਤਰ control_endpoint *ep, *tmp;

	/* Run plugin cleanup function */
	ਜੇ(plugin->lib.ਸੰਸਕਰਣ_2 == 5){
		ਜੇ(plugin->lib.plugin_cleanup_v5){
			plugin->lib.plugin_cleanup_v5(
					plugin->lib.user_ਅੰਕੜੇ,
					plugin->config.options,
					plugin->config.option_ਗਿਣਤੀ);
		}
	}ਨਹੀਂਤਾਂ ਜੇ(plugin->lib.ਸੰਸਕਰਣ_2 == 4){
		ਜੇ(plugin->lib.plugin_cleanup_v4){
			plugin->lib.plugin_cleanup_v4(
					plugin->lib.user_ਅੰਕੜੇ,
					plugin->config.options,
					plugin->config.option_ਗਿਣਤੀ);
		}
	}ਨਹੀਂਤਾਂ ਜੇ(plugin->lib.ਸੰਸਕਰਣ_2 == 3){
		ਜੇ(plugin->lib.plugin_cleanup_v3){
			plugin->lib.plugin_cleanup_v3(
					plugin->lib.user_ਅੰਕੜੇ,
					plugin->config.options,
					plugin->config.option_ਗਿਣਤੀ);
		}
	}ਨਹੀਂਤਾਂ ਜੇ(plugin->lib.ਸੰਸਕਰਣ_2 == 2){
		ਜੇ(plugin->lib.plugin_cleanup_v2){
			plugin->lib.plugin_cleanup_v2(
					plugin->lib.user_ਅੰਕੜੇ,
					(ਬਣਤਰ mosquitto_auth_opt *)plugin->config.options,
					plugin->config.option_ਗਿਣਤੀ);
		}
	}

	plugin__callback_unregister_all(plugin);
	mosquitto_FREE(plugin->plugin_name);
	mosquitto_FREE(plugin->plugin_ਸੰਸਕਰਣ);
	DL_FOREACH_ਸੁਰੱਖਿਅਤ(plugin->control_endpoints, ep, tmp){
		DL_DELETE(plugin->control_endpoints, ep);
		mosquitto_FREE(ep);
	}

	ਜੇ(plugin->lib.lib){
		//LIB_CLOSE(plugin->lib.lib);
	}
	memset(&plugin->lib, 0, ਆਕਾਰ(ਬਣਤਰ mosquitto__plugin_lib));
}


ਪੂਰਨ plugin__unload_all(ਖਾਲੀ)
{
	ਲਈ(ਪੂਰਨ i=0; i<db.plugin_ਗਿਣਤੀ; i++){
		plugin__unload_single(db.plugins[i]);
	}

	mosquitto_security_cleanup(false);

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ plugin__security_cleanup_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	ਪੂਰਨ rc;

	ਜੇ(plugin->lib.ਸੰਸਕਰਣ_2 == 5){
		rc = MOSQ_ERR_SUCCESS;
	}ਨਹੀਂਤਾਂ ਜੇ(plugin->lib.ਸੰਸਕਰਣ_2 == 4){
		rc = plugin->lib.security_cleanup_v4(
				plugin->lib.user_ਅੰਕੜੇ,
				plugin->config.options,
				plugin->config.option_ਗਿਣਤੀ,
				reload);

	}ਨਹੀਂਤਾਂ ਜੇ(plugin->lib.ਸੰਸਕਰਣ_2 == 3){
		rc = plugin->lib.security_cleanup_v3(
				plugin->lib.user_ਅੰਕੜੇ,
				plugin->config.options,
				plugin->config.option_ਗਿਣਤੀ,
				reload);

	}ਨਹੀਂਤਾਂ ਜੇ(plugin->lib.ਸੰਸਕਰਣ_2 == 2){
		rc = plugin->lib.security_cleanup_v2(
				plugin->lib.user_ਅੰਕੜੇ,
				(ਬਣਤਰ mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_ਗਿਣਤੀ,
				reload);
	}ਨਹੀਂਤਾਂ{
		rc = MOSQ_ERR_INVAL;
	}

	ਵਾਪਸ rc;
}


ਪੂਰਨ mosquitto_security_cleanup(bool reload)
{
	ਲਈ(ਪੂਰਨ i=0; i<db.plugin_ਗਿਣਤੀ; i++){
		plugin__security_cleanup_single(db.plugins[i], reload);
	}

	ਵਾਪਸ mosquitto_security_cleanup_default();
}
