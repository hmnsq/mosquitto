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

#include "config.h"

#include <stdio.h>
#include <string.h>

#include "mosquitto/broker.h"
#include "mosquitto_broker_internal.h"
#include "mosquitto/broker_plugin.h"
#include "lib_load.h"
#include "utlist.h"

static puur_naa_nka plugin__security_cleanup_single(mosquitto_plugin_id_t *plugin, bool reload);


static void plugin__unload_single(mosquitto_plugin_id_t *plugin)
{
	sa_nracanaa control_endpoint *ep, *tmp;

	/* Run plugin cleanup function */
	ᱡᱩᱫᱤ(plugin->lib.version == 5){
		ᱡᱩᱫᱤ(plugin->lib.plugin_cleanup_v5){
			plugin->lib.plugin_cleanup_v5(
					plugin->lib.user_data,
					plugin->config.options,
					plugin->config.option_count);
		}
	}ᱵᱟᱝ ᱡᱩᱫᱤ(plugin->lib.version == 4){
		ᱡᱩᱫᱤ(plugin->lib.plugin_cleanup_v4){
			plugin->lib.plugin_cleanup_v4(
					plugin->lib.user_data,
					plugin->config.options,
					plugin->config.option_count);
		}
	}ᱵᱟᱝ ᱡᱩᱫᱤ(plugin->lib.version == 3){
		ᱡᱩᱫᱤ(plugin->lib.plugin_cleanup_v3){
			plugin->lib.plugin_cleanup_v3(
					plugin->lib.user_data,
					plugin->config.options,
					plugin->config.option_count);
		}
	}ᱵᱟᱝ ᱡᱩᱫᱤ(plugin->lib.version == 2){
		ᱡᱩᱫᱤ(plugin->lib.plugin_cleanup_v2){
			plugin->lib.plugin_cleanup_v2(
					plugin->lib.user_data,
					(sa_nracanaa mosquitto_auth_opt *)plugin->config.options,
					plugin->config.option_count);
		}
	}

	plugin__callback_unregister_all(plugin);
	mosquitto_FREE(plugin->plugin_name);
	mosquitto_FREE(plugin->plugin_version);
	DL_FOREACH_SAFE(plugin->control_endpoints, ep, tmp){
		DL_DELETE(plugin->control_endpoints, ep);
		mosquitto_FREE(ep);
	}

	ᱡᱩᱫᱤ(plugin->lib.lib){
		//LIB_CLOSE(plugin->lib.lib);
	}
	memset(&plugin->lib, 0, parimaa_na(sa_nracanaa mosquitto__plugin_lib));
}


puur_naa_nka plugin__unload_all(void)
{
	for(puur_naa_nka i=0; i<db.plugin_count; i++){
		plugin__unload_single(db.plugins[i]);
	}

	mosquitto_security_cleanup(false);

	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka plugin__security_cleanup_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	puur_naa_nka rc;

	ᱡᱩᱫᱤ(plugin->lib.version == 5){
		rc = MOSQ_ERR_SUCCESS;
	}ᱵᱟᱝ ᱡᱩᱫᱤ(plugin->lib.version == 4){
		rc = plugin->lib.security_cleanup_v4(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_count,
				reload);

	}ᱵᱟᱝ ᱡᱩᱫᱤ(plugin->lib.version == 3){
		rc = plugin->lib.security_cleanup_v3(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_count,
				reload);

	}ᱵᱟᱝ ᱡᱩᱫᱤ(plugin->lib.version == 2){
		rc = plugin->lib.security_cleanup_v2(
				plugin->lib.user_data,
				(sa_nracanaa mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_count,
				reload);
	}ᱵᱟᱝ{
		rc = MOSQ_ERR_INVAL;
	}

	ᱨᱩᱣᱟᱹᱲ rc;
}


puur_naa_nka mosquitto_security_cleanup(bool reload)
{
	for(puur_naa_nka i=0; i<db.plugin_count; i++){
		plugin__security_cleanup_single(db.plugins[i], reload);
	}

	ᱨᱩᱣᱟᱹᱲ mosquitto_security_cleanup_default();
}
