/*
Copyright (c) 2021 Roger Light <roger@atchoo.org>

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

#شامل <cjson/cJSON.h>
#شامل <errno.h>
#شامل <stdio.h>
#شامل <stdlib.h>
#شامل <string.h>
#شامل <sys/stat.h>
#شامل <utlist.h>

#شامل "json_help.h"
#شامل "mosquitto.h"
#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/broker.h"
#شامل "mosquitto/broker_control.h"
#شامل "mosquitto/broker_plugin.h"
#شامل "mosquitto/mqtt_protocol.h"

ساکن mosquitto_plugin_id_t plg_id;
ساکن صحیح broker__handle_control(ساخت mosquitto_control_cmd *cmd, خالی *userdata);


ساکن صحیح جوڑو_plugin_info(cJSON *j_plugins, mosquitto_plugin_id_t *pid)
{
	cJSON *j_plugin, *j_eps, *j_ep;
	ساخت control_endpoint *ep;

	اگر(pid->plugin_name == تہی){
		واپس MOSQ_ERR_SUCCESS;
	}

	j_plugin = cJSON_CreateObject();
	اگر(j_plugin == تہی){
		واپس MOSQ_ERR_NOMEM;
	}

	اگر(c_JSON_جوڑو_String_مقصد_Object(j_plugin, "name", pid->plugin_name) == تہی
			|| (pid->plugin_نسخہ && c_JSON_جوڑو_String_مقصد_Object(j_plugin, "version", pid->plugin_نسخہ) == تہی)
			|| (pid->listener && c_JSON_جوڑو_نمبر_مقصد_Object(j_plugin, "port", pid->listener->پورٹ) == تہی)
			|| (j_eps = c_JSON_جوڑو_Array_مقصد_Object(j_plugin, "control-endpoints")) == تہی
			){

		cJSON_Delete(j_plugin);
		واپس MOSQ_ERR_NOMEM;
	}

	DL_FOREACH(pid->control_endpoints, ep){
		j_ep = cJSON_CreateString(ep->topic);
		اگر(j_ep == تہی){
			cJSON_Delete(j_plugin);
			واپس MOSQ_ERR_NOMEM;
		}
		c_JSON_جوڑو_شے_مقصد_Array(j_eps, j_ep);
	}

	c_JSON_جوڑو_شے_مقصد_Array(j_plugins, j_plugin);
	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح broker__process_list_plugins(ساخت mosquitto_control_cmd *cmd)
{
	cJSON *درخت, *j_ڈیٹا, *j_plugins;
	ثابت حرف *admin_clientid, *admin_username;

	درخت = cJSON_CreateObject();
	اگر(درخت == تہی){
		mosquitto_control_command_جواب(cmd, "Internal error");
		واپس MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listPlugins",
			admin_clientid, admin_username);

	اگر(c_JSON_جوڑو_String_مقصد_Object(درخت, "command", "listPlugins") == تہی
			|| ((j_ڈیٹا = c_JSON_جوڑو_Object_مقصد_Object(درخت, "data")) == تہی)
			|| (cmd->correlation_ڈیٹا && c_JSON_جوڑو_String_مقصد_Object(درخت, "correlationData", cmd->correlation_ڈیٹا) == تہی)
			){

		جاؤ اندرونی_error;
	}

	j_plugins = c_JSON_جوڑو_Array_مقصد_Object(j_ڈیٹا, "plugins");
	اگر(j_plugins == تہی){
		جاؤ اندرونی_error;
	}

	برائے(صحیح i=0; i<db.plugin_گنتی; i++){
		اگر(جوڑو_plugin_info(j_plugins, db.plugins[i])){
			جاؤ اندرونی_error;
		}
	}

	c_JSON_جوڑو_شے_مقصد_Array(cmd->j_responses, درخت);

	واپس MOSQ_ERR_SUCCESS;

اندرونی_error:
	cJSON_Delete(درخت);
	mosquitto_control_command_جواب(cmd, "Internal error");
	واپس MOSQ_ERR_NOMEM;
}


ساکن صحیح جوڑو_listener(cJSON *j_listeners, ساخت mosquitto__listener *listener)
{
	cJSON *j_listener;
	ثابت حرف *protocol = تہی;

	j_listener = cJSON_CreateObject();
	اگر(j_listener == تہی){
		واپس MOSQ_ERR_NOMEM;
	}
	c_JSON_جوڑو_شے_مقصد_Array(j_listeners, j_listener);

	اگر(listener->protocol == mp_mqtt){
		protocol = "mqtt";
	}ورنہ اگر(listener->protocol == mp_websockets){
		protocol = "mqtt+websockets";
	}

	اگر(c_JSON_جوڑو_نمبر_مقصد_Object(j_listener, "port", listener->پورٹ) == تہی
			|| (protocol && c_JSON_جوڑو_String_مقصد_Object(j_listener, "protocol", protocol) == تہی)
			|| (listener->host && c_JSON_جوڑو_String_مقصد_Object(j_listener, "bind-address", listener->host) == تہی)
#اگر_تعریف_شدہ WITH_UNIX_SOCKETS
			|| (listener->unix_socket_راستہ && c_JSON_جوڑو_String_مقصد_Object(j_listener, "socket-path", listener->unix_socket_راستہ) == تہی)
#اگر_ختم
			){

		واپس MOSQ_ERR_NOMEM;
	}

	اگر(
#اگر_تعریف_شدہ WITH_TLS
			c_JSON_جوڑو_Bool_مقصد_Object(j_listener, "tls", listener->ssl_ctx != تہی) == تہی
#ورنہ
			c_JSON_جوڑو_Bool_مقصد_Object(j_listener, "tls", false) == تہی
#اگر_ختم
			){

		واپس MOSQ_ERR_NOMEM;
	}

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح broker__process_list_listeners(ساخت mosquitto_control_cmd *cmd)
{
	cJSON *درخت, *j_ڈیٹا, *j_listeners;
	ثابت حرف *admin_clientid, *admin_username;

	درخت = cJSON_CreateObject();
	اگر(درخت == تہی){
		mosquitto_control_command_جواب(cmd, "Internal error");
		واپس MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listListeners",
			admin_clientid, admin_username);

	اگر(c_JSON_جوڑو_String_مقصد_Object(درخت, "command", "listListeners") == تہی
			|| ((j_ڈیٹا = c_JSON_جوڑو_Object_مقصد_Object(درخت, "data")) == تہی)
			|| (cmd->correlation_ڈیٹا && c_JSON_جوڑو_String_مقصد_Object(درخت, "correlationData", cmd->correlation_ڈیٹا) == تہی)
			){

		جاؤ اندرونی_error;
	}

	j_listeners = c_JSON_جوڑو_Array_مقصد_Object(j_ڈیٹا, "listeners");
	اگر(j_listeners == تہی){
		جاؤ اندرونی_error;
	}

	برائے(صحیح i=0; i<db.config->listener_گنتی; i++){
		اگر(جوڑو_listener(j_listeners, &db.config->listeners[i])){
			جاؤ اندرونی_error;
		}
	}

	c_JSON_جوڑو_شے_مقصد_Array(cmd->j_responses, درخت);

	واپس MOSQ_ERR_SUCCESS;

اندرونی_error:
	cJSON_Delete(درخت);
	mosquitto_control_command_جواب(cmd, "Internal error");
	واپس MOSQ_ERR_NOMEM;
}


ساکن صحیح broker_control_callback(صحیح event, خالی *event_ڈیٹا, خالی *userdata)
{
	ساخت mosquitto_evt_control *ed = event_ڈیٹا;

	غیر_مستعمل(event);

	واپس mosquitto_control_generic_callback(ed, "$CONTROL/broker/v1/response", userdata, broker__handle_control);
}


خالی broker_control__init(خالی)
{
	memset(&plg_id, 0, حجم(plg_id));

	اگر(db.config->enable_control_api){
		mosquitto_callback_رجسٹر(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1", تہی);
	}
}


خالی broker_control__cleanup(خالی)
{
	mosquitto_callback_unregister(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1");
}


خالی broker_control__reload(خالی)
{
	broker_control__cleanup();
	broker_control__init();
}


/* ################################################################
 * #
 * # $CONTROL/broker/v1 handler
 * #
 * ################################################################ */


ساکن صحیح broker__handle_control(ساخت mosquitto_control_cmd *cmd, خالی *userdata)
{
	صحیح rc = MOSQ_ERR_SUCCESS;

	غیر_مستعمل(userdata);

	اگر(!strcasecmp(cmd->command_name, "listPlugins")){
		rc = broker__process_list_plugins(cmd);
	}ورنہ اگر(!strcasecmp(cmd->command_name, "listListeners")){
		rc = broker__process_list_listeners(cmd);

		/* Unknown */
	}ورنہ{
		mosquitto_control_command_جواب(cmd, "Unknown command");
		rc = MOSQ_ERR_INVAL;
	}
	واپس rc;
}
