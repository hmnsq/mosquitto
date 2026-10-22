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

#شامل "config.h"

#شامل <stdio.h>
#شامل <string.h>

#شامل "mosquitto/broker.h"
#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/broker_plugin.h"
#شامل "lib_load.h"
#شامل "utlist.h"

ایستا صحیح plugin__security_cleanup_single(mosquitto_plugin_id_t *plugin, bool reload);


ایستا تهی plugin__unload_single(mosquitto_plugin_id_t *plugin)
{
	ساختار control_endpoint *ep, *tmp;

	/* Run plugin cleanup function */
	اگر(plugin->lib.version == 5){
		اگر(plugin->lib.plugin_cleanup_v5){
			plugin->lib.plugin_cleanup_v5(
					plugin->lib.user_data,
					plugin->config.options,
					plugin->config.option_شمار);
		}
	}وگرنه اگر(plugin->lib.version == 4){
		اگر(plugin->lib.plugin_cleanup_v4){
			plugin->lib.plugin_cleanup_v4(
					plugin->lib.user_data,
					plugin->config.options,
					plugin->config.option_شمار);
		}
	}وگرنه اگر(plugin->lib.version == 3){
		اگر(plugin->lib.plugin_cleanup_v3){
			plugin->lib.plugin_cleanup_v3(
					plugin->lib.user_data,
					plugin->config.options,
					plugin->config.option_شمار);
		}
	}وگرنه اگر(plugin->lib.version == 2){
		اگر(plugin->lib.plugin_cleanup_v2){
			plugin->lib.plugin_cleanup_v2(
					plugin->lib.user_data,
					(ساختار mosquitto_auth_opt *)plugin->config.options,
					plugin->config.option_شمار);
		}
	}

	plugin__callback_unregister_all(plugin);
	mosquitto_FREE(plugin->plugin_name);
	mosquitto_FREE(plugin->plugin_version);
	DL_FOREACH_SAFE(plugin->control_endpoints, ep, tmp){
		DL_DELETE(plugin->control_endpoints, ep);
		mosquitto_FREE(ep);
	}

	اگر(plugin->lib.lib){
		//LIB_CLOSE(plugin->lib.lib);
	}
	memset(&plugin->lib, 0, اندازه(ساختار mosquitto__plugin_lib));
}


صحیح plugin__unload_all(تهی)
{
	برای(صحیح i=0; i<db.plugin_شمار; i++){
		plugin__unload_single(db.plugins[i]);
	}

	mosquitto_security_cleanup(false);

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح plugin__security_cleanup_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	صحیح rc;

	اگر(plugin->lib.version == 5){
		rc = MOSQ_ERR_SUCCESS;
	}وگرنه اگر(plugin->lib.version == 4){
		rc = plugin->lib.security_cleanup_v4(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_شمار,
				reload);

	}وگرنه اگر(plugin->lib.version == 3){
		rc = plugin->lib.security_cleanup_v3(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_شمار,
				reload);

	}وگرنه اگر(plugin->lib.version == 2){
		rc = plugin->lib.security_cleanup_v2(
				plugin->lib.user_data,
				(ساختار mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_شمار,
				reload);
	}وگرنه{
		rc = MOSQ_ERR_INVAL;
	}

	بازگشت rc;
}


صحیح mosquitto_security_cleanup(bool reload)
{
	برای(صحیح i=0; i<db.plugin_شمار; i++){
		plugin__security_cleanup_single(db.plugins[i], reload);
	}

	بازگشت mosquitto_security_cleanup_default();
}
