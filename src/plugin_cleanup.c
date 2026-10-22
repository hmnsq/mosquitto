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

#ضمن "config.h"

#ضمن <stdio.h>
#ضمن <string.h>

#ضمن "mosquitto/broker.h"
#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/broker_plugin.h"
#ضمن "lib_load.h"
#ضمن "utlist.h"

ساكن صحيح plugin__security_cleanup_single(mosquitto_plugin_id_t *plugin, bool reload);


ساكن فراغ plugin__unload_single(mosquitto_plugin_id_t *plugin)
{
	بنية control_endpoint *ep, *tmp;

	/* Run plugin cleanup function */
	ܐܢ(plugin->lib.version == 5){
		ܐܢ(plugin->lib.plugin_cleanup_v5){
			plugin->lib.plugin_cleanup_v5(
					plugin->lib.user_data,
					plugin->config.options,
					plugin->config.option_count);
		}
	}ܐܚܪܢܐ ܐܢ(plugin->lib.version == 4){
		ܐܢ(plugin->lib.plugin_cleanup_v4){
			plugin->lib.plugin_cleanup_v4(
					plugin->lib.user_data,
					plugin->config.options,
					plugin->config.option_count);
		}
	}ܐܚܪܢܐ ܐܢ(plugin->lib.version == 3){
		ܐܢ(plugin->lib.plugin_cleanup_v3){
			plugin->lib.plugin_cleanup_v3(
					plugin->lib.user_data,
					plugin->config.options,
					plugin->config.option_count);
		}
	}ܐܚܪܢܐ ܐܢ(plugin->lib.version == 2){
		ܐܢ(plugin->lib.plugin_cleanup_v2){
			plugin->lib.plugin_cleanup_v2(
					plugin->lib.user_data,
					(بنية mosquitto_auth_opt *)plugin->config.options,
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

	ܐܢ(plugin->lib.lib){
		//LIB_CLOSE(plugin->lib.lib);
	}
	memset(&plugin->lib, 0, حجم(بنية mosquitto__plugin_lib));
}


صحيح plugin__unload_all(فراغ)
{
	لكل(صحيح i=0; i<db.plugin_count; i++){
		plugin__unload_single(db.plugins[i]);
	}

	mosquitto_security_cleanup(false);

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


ساكن صحيح plugin__security_cleanup_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	صحيح rc;

	ܐܢ(plugin->lib.version == 5){
		rc = MOSQ_ERR_SUCCESS;
	}ܐܚܪܢܐ ܐܢ(plugin->lib.version == 4){
		rc = plugin->lib.security_cleanup_v4(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_count,
				reload);

	}ܐܚܪܢܐ ܐܢ(plugin->lib.version == 3){
		rc = plugin->lib.security_cleanup_v3(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_count,
				reload);

	}ܐܚܪܢܐ ܐܢ(plugin->lib.version == 2){
		rc = plugin->lib.security_cleanup_v2(
				plugin->lib.user_data,
				(بنية mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_count,
				reload);
	}ܐܚܪܢܐ{
		rc = MOSQ_ERR_INVAL;
	}

	ܦܢܐ rc;
}


صحيح mosquitto_security_cleanup(bool reload)
{
	لكل(صحيح i=0; i<db.plugin_count; i++){
		plugin__security_cleanup_single(db.plugins[i], reload);
	}

	ܦܢܐ mosquitto_security_cleanup_default();
}
