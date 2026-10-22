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

#ضمن "config.h"

#ضمن <cjson/cJSON.h>
#ضمن <errno.h>
#ضمن <stdio.h>
#ضمن <stdlib.h>
#ضمن <string.h>
#ضمن <sys/stat.h>
#ضمن <utlist.h>

#ضمن "json_help.h"
#ضمن "mosquitto.h"
#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/broker.h"
#ضمن "mosquitto/broker_control.h"
#ضمن "mosquitto/broker_plugin.h"
#ضمن "mosquitto/mqtt_protocol.h"

ساكن mosquitto_plugin_id_t plg_id;
ساكن صحيح broker__handle_control(بنية mosquitto_control_cmd *cmd, فراغ *userdata);


ساكن صحيح جمع_کن_plugin_info(cJSON *j_plugins, mosquitto_plugin_id_t *pid)
{
	cJSON *j_plugin, *j_eps, *j_ep;
	بنية control_endpoint *ep;

	إذا(pid->plugin_name == عدم){
		أرجع MOSQ_ERR_SUCCESS;
	}

	j_plugin = cJSON_CreateObject();
	إذا(j_plugin == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}

	إذا(c_JSON_جمع_کن_String_مقصد_Object(j_plugin, "name", pid->plugin_name) == عدم
			|| (pid->plugin_نسخه && c_JSON_جمع_کن_String_مقصد_Object(j_plugin, "version", pid->plugin_نسخه) == عدم)
			|| (pid->listener && c_JSON_جمع_کن_عدد_مقصد_Object(j_plugin, "port", pid->listener->پورت) == عدم)
			|| (j_eps = c_JSON_جمع_کن_Array_مقصد_Object(j_plugin, "control-endpoints")) == عدم
			){

		cJSON_Delete(j_plugin);
		أرجع MOSQ_ERR_NOMEM;
	}

	DL_FOREACH(pid->control_endpoints, ep){
		j_ep = cJSON_CreateString(ep->topic);
		إذا(j_ep == عدم){
			cJSON_Delete(j_plugin);
			أرجع MOSQ_ERR_NOMEM;
		}
		c_JSON_جمع_کن_قلم_مقصد_Array(j_eps, j_ep);
	}

	c_JSON_جمع_کن_قلم_مقصد_Array(j_plugins, j_plugin);
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح broker__process_list_plugins(بنية mosquitto_control_cmd *cmd)
{
	cJSON *درخت, *j_داده, *j_plugins;
	ثابت حرف *admin_clientid, *admin_username;

	درخت = cJSON_CreateObject();
	إذا(درخت == عدم){
		mosquitto_control_command_پاسخ(cmd, "Internal error");
		أرجع MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listPlugins",
			admin_clientid, admin_username);

	إذا(c_JSON_جمع_کن_String_مقصد_Object(درخت, "command", "listPlugins") == عدم
			|| ((j_داده = c_JSON_جمع_کن_Object_مقصد_Object(درخت, "data")) == عدم)
			|| (cmd->correlation_داده && c_JSON_جمع_کن_String_مقصد_Object(درخت, "correlationData", cmd->correlation_داده) == عدم)
			){

		اذهب داخلی_error;
	}

	j_plugins = c_JSON_جمع_کن_Array_مقصد_Object(j_داده, "plugins");
	إذا(j_plugins == عدم){
		اذهب داخلی_error;
	}

	لكل(صحيح i=0; i<db.plugin_شمار; i++){
		إذا(جمع_کن_plugin_info(j_plugins, db.plugins[i])){
			اذهب داخلی_error;
		}
	}

	c_JSON_جمع_کن_قلم_مقصد_Array(cmd->j_responses, درخت);

	أرجع MOSQ_ERR_SUCCESS;

داخلی_error:
	cJSON_Delete(درخت);
	mosquitto_control_command_پاسخ(cmd, "Internal error");
	أرجع MOSQ_ERR_NOMEM;
}


ساكن صحيح جمع_کن_listener(cJSON *j_listeners, بنية mosquitto__listener *listener)
{
	cJSON *j_listener;
	ثابت حرف *protocol = عدم;

	j_listener = cJSON_CreateObject();
	إذا(j_listener == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}
	c_JSON_جمع_کن_قلم_مقصد_Array(j_listeners, j_listener);

	إذا(listener->protocol == mp_mqtt){
		protocol = "mqtt";
	}وإلا إذا(listener->protocol == mp_websockets){
		protocol = "mqtt+websockets";
	}

	إذا(c_JSON_جمع_کن_عدد_مقصد_Object(j_listener, "port", listener->پورت) == عدم
			|| (protocol && c_JSON_جمع_کن_String_مقصد_Object(j_listener, "protocol", protocol) == عدم)
			|| (listener->host && c_JSON_جمع_کن_String_مقصد_Object(j_listener, "bind-address", listener->host) == عدم)
#اگر_تعریف_شده WITH_UNIX_SOCKETS
			|| (listener->unix_socket_مسیر && c_JSON_جمع_کن_String_مقصد_Object(j_listener, "socket-path", listener->unix_socket_مسیر) == عدم)
#پایان_اگر
			){

		أرجع MOSQ_ERR_NOMEM;
	}

	إذا(
#اگر_تعریف_شده WITH_TLS
			c_JSON_جمع_کن_Bool_مقصد_Object(j_listener, "tls", listener->ssl_ctx != عدم) == عدم
#وإلا
			c_JSON_جمع_کن_Bool_مقصد_Object(j_listener, "tls", false) == عدم
#پایان_اگر
			){

		أرجع MOSQ_ERR_NOMEM;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح broker__process_list_listeners(بنية mosquitto_control_cmd *cmd)
{
	cJSON *درخت, *j_داده, *j_listeners;
	ثابت حرف *admin_clientid, *admin_username;

	درخت = cJSON_CreateObject();
	إذا(درخت == عدم){
		mosquitto_control_command_پاسخ(cmd, "Internal error");
		أرجع MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listListeners",
			admin_clientid, admin_username);

	إذا(c_JSON_جمع_کن_String_مقصد_Object(درخت, "command", "listListeners") == عدم
			|| ((j_داده = c_JSON_جمع_کن_Object_مقصد_Object(درخت, "data")) == عدم)
			|| (cmd->correlation_داده && c_JSON_جمع_کن_String_مقصد_Object(درخت, "correlationData", cmd->correlation_داده) == عدم)
			){

		اذهب داخلی_error;
	}

	j_listeners = c_JSON_جمع_کن_Array_مقصد_Object(j_داده, "listeners");
	إذا(j_listeners == عدم){
		اذهب داخلی_error;
	}

	لكل(صحيح i=0; i<db.config->listener_شمار; i++){
		إذا(جمع_کن_listener(j_listeners, &db.config->listeners[i])){
			اذهب داخلی_error;
		}
	}

	c_JSON_جمع_کن_قلم_مقصد_Array(cmd->j_responses, درخت);

	أرجع MOSQ_ERR_SUCCESS;

داخلی_error:
	cJSON_Delete(درخت);
	mosquitto_control_command_پاسخ(cmd, "Internal error");
	أرجع MOSQ_ERR_NOMEM;
}


ساكن صحيح broker_control_callback(صحيح event, فراغ *event_داده, فراغ *userdata)
{
	بنية mosquitto_evt_control *ed = event_داده;

	استفادهنشده(event);

	أرجع mosquitto_control_generic_callback(ed, "$CONTROL/broker/v1/response", userdata, broker__handle_control);
}


فراغ broker_control__init(فراغ)
{
	memset(&plg_id, 0, حجم(plg_id));

	إذا(db.config->enable_control_api){
		mosquitto_callback_ثبات(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1", عدم);
	}
}


فراغ broker_control__cleanup(فراغ)
{
	mosquitto_callback_unregister(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1");
}


فراغ broker_control__reload(فراغ)
{
	broker_control__cleanup();
	broker_control__init();
}


/* ################################################################
 * #
 * # $CONTROL/broker/v1 handler
 * #
 * ################################################################ */


ساكن صحيح broker__handle_control(بنية mosquitto_control_cmd *cmd, فراغ *userdata)
{
	صحيح rc = MOSQ_ERR_SUCCESS;

	استفادهنشده(userdata);

	إذا(!strcasecmp(cmd->command_name, "listPlugins")){
		rc = broker__process_list_plugins(cmd);
	}وإلا إذا(!strcasecmp(cmd->command_name, "listListeners")){
		rc = broker__process_list_listeners(cmd);

		/* Unknown */
	}وإلا{
		mosquitto_control_command_پاسخ(cmd, "Unknown command");
		rc = MOSQ_ERR_INVAL;
	}
	أرجع rc;
}
