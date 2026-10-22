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

ساکن صحیح plugin__security_cleanup_single(mosquitto_plugin_id_t *plugin, bool reload);


ساکن خالی plugin__unload_single(mosquitto_plugin_id_t *plugin)
{
	ساخت control_endpoint *ep, *tmp;

	/* Run plugin cleanup function */
	اگر(plugin->lib.نسخہ_2 == 5){
		اگر(plugin->lib.plugin_cleanup_v5){
			plugin->lib.plugin_cleanup_v5(
					plugin->lib.user_ڈیٹا,
					plugin->config.options,
					plugin->config.option_گنتی);
		}
	}ورنہ اگر(plugin->lib.نسخہ_2 == 4){
		اگر(plugin->lib.plugin_cleanup_v4){
			plugin->lib.plugin_cleanup_v4(
					plugin->lib.user_ڈیٹا,
					plugin->config.options,
					plugin->config.option_گنتی);
		}
	}ورنہ اگر(plugin->lib.نسخہ_2 == 3){
		اگر(plugin->lib.plugin_cleanup_v3){
			plugin->lib.plugin_cleanup_v3(
					plugin->lib.user_ڈیٹا,
					plugin->config.options,
					plugin->config.option_گنتی);
		}
	}ورنہ اگر(plugin->lib.نسخہ_2 == 2){
		اگر(plugin->lib.plugin_cleanup_v2){
			plugin->lib.plugin_cleanup_v2(
					plugin->lib.user_ڈیٹا,
					(ساخت mosquitto_auth_opt *)plugin->config.options,
					plugin->config.option_گنتی);
		}
	}

	plugin__callback_unregister_all(plugin);
	mosquitto_FREE(plugin->plugin_name);
	mosquitto_FREE(plugin->plugin_نسخہ);
	DL_FOREACH_محفوظ(plugin->control_endpoints, ep, tmp){
		DL_DELETE(plugin->control_endpoints, ep);
		mosquitto_FREE(ep);
	}

	اگر(plugin->lib.lib){
		//LIB_CLOSE(plugin->lib.lib);
	}
	memset(&plugin->lib, 0, حجم(ساخت mosquitto__plugin_lib));
}


صحیح plugin__unload_all(خالی)
{
	برائے(صحیح i=0; i<db.plugin_گنتی; i++){
		plugin__unload_single(db.plugins[i]);
	}

	mosquitto_security_cleanup(false);

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح plugin__security_cleanup_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	صحیح rc;

	اگر(plugin->lib.نسخہ_2 == 5){
		rc = MOSQ_ERR_SUCCESS;
	}ورنہ اگر(plugin->lib.نسخہ_2 == 4){
		rc = plugin->lib.security_cleanup_v4(
				plugin->lib.user_ڈیٹا,
				plugin->config.options,
				plugin->config.option_گنتی,
				reload);

	}ورنہ اگر(plugin->lib.نسخہ_2 == 3){
		rc = plugin->lib.security_cleanup_v3(
				plugin->lib.user_ڈیٹا,
				plugin->config.options,
				plugin->config.option_گنتی,
				reload);

	}ورنہ اگر(plugin->lib.نسخہ_2 == 2){
		rc = plugin->lib.security_cleanup_v2(
				plugin->lib.user_ڈیٹا,
				(ساخت mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_گنتی,
				reload);
	}ورنہ{
		rc = MOSQ_ERR_INVAL;
	}

	واپس rc;
}


صحیح mosquitto_security_cleanup(bool reload)
{
	برائے(صحیح i=0; i<db.plugin_گنتی; i++){
		plugin__security_cleanup_single(db.plugins[i], reload);
	}

	واپس mosquitto_security_cleanup_default();
}
