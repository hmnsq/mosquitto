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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "mosquitto/broker.h"
#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/broker_plugin.h"
#અન્તર્ભાવ "lib_load.h"
#અન્તર્ભાવ "utlist.h"

સ્થિર પૂર્ણાંક plugin__security_cleanup_single(mosquitto_plugin_id_t *plugin, bool reload);


સ્થિર શૂન્ય plugin__unload_single(mosquitto_plugin_id_t *plugin)
{
	સંરચના control_endpoint *ep, *tmp;

	/* Run plugin cleanup function */
	જો(plugin->lib.version == 5){
		જો(plugin->lib.plugin_cleanup_v5){
			plugin->lib.plugin_cleanup_v5(
					plugin->lib.user_data,
					plugin->config.options,
					plugin->config.option_ગણતરી);
		}
	}નહીંતર જો(plugin->lib.version == 4){
		જો(plugin->lib.plugin_cleanup_v4){
			plugin->lib.plugin_cleanup_v4(
					plugin->lib.user_data,
					plugin->config.options,
					plugin->config.option_ગણતરી);
		}
	}નહીંતર જો(plugin->lib.version == 3){
		જો(plugin->lib.plugin_cleanup_v3){
			plugin->lib.plugin_cleanup_v3(
					plugin->lib.user_data,
					plugin->config.options,
					plugin->config.option_ગણતરી);
		}
	}નહીંતર જો(plugin->lib.version == 2){
		જો(plugin->lib.plugin_cleanup_v2){
			plugin->lib.plugin_cleanup_v2(
					plugin->lib.user_data,
					(સંરચના mosquitto_auth_opt *)plugin->config.options,
					plugin->config.option_ગણતરી);
		}
	}

	plugin__callback_unregister_all(plugin);
	mosquitto_FREE(plugin->plugin_name);
	mosquitto_FREE(plugin->plugin_version);
	DL_FOREACH_SAFE(plugin->control_endpoints, ep, tmp){
		DL_DELETE(plugin->control_endpoints, ep);
		mosquitto_FREE(ep);
	}

	જો(plugin->lib.lib){
		//LIB_CLOSE(plugin->lib.lib);
	}
	memset(&plugin->lib, 0, કદ(સંરચના mosquitto__plugin_lib));
}


પૂર્ણાંક plugin__unload_all(શૂન્ય)
{
	માટે(પૂર્ણાંક i=0; i<db.plugin_ગણતરી; i++){
		plugin__unload_single(db.plugins[i]);
	}

	mosquitto_security_cleanup(false);

	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક plugin__security_cleanup_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	પૂર્ણાંક rc;

	જો(plugin->lib.version == 5){
		rc = MOSQ_ERR_SUCCESS;
	}નહીંતર જો(plugin->lib.version == 4){
		rc = plugin->lib.security_cleanup_v4(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_ગણતરી,
				reload);

	}નહીંતર જો(plugin->lib.version == 3){
		rc = plugin->lib.security_cleanup_v3(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_ગણતરી,
				reload);

	}નહીંતર જો(plugin->lib.version == 2){
		rc = plugin->lib.security_cleanup_v2(
				plugin->lib.user_data,
				(સંરચના mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_ગણતરી,
				reload);
	}નહીંતર{
		rc = MOSQ_ERR_INVAL;
	}

	પાછા rc;
}


પૂર્ણાંક mosquitto_security_cleanup(bool reload)
{
	માટે(પૂર્ણાંક i=0; i<db.plugin_ગણતરી; i++){
		plugin__security_cleanup_single(db.plugins[i], reload);
	}

	પાછા mosquitto_security_cleanup_default();
}
