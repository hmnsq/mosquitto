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

ایستا mosquitto_plugin_id_t plg_id;
ایستا صحیح broker__handle_control(ساختار mosquitto_control_cmd *cmd, تهی *userdata);


ایستا صحیح add_plugin_info(cJSON *j_plugins, mosquitto_plugin_id_t *pid)
{
	cJSON *j_plugin, *j_eps, *j_ep;
	ساختار control_endpoint *ep;

	اگر(pid->plugin_name == پوچ){
		بازگشت MOSQ_ERR_SUCCESS;
	}

	j_plugin = cJSON_CreateObject();
	اگر(j_plugin == پوچ){
		بازگشت MOSQ_ERR_NOMEM;
	}

	اگر(cJSON_AddStringToObject(j_plugin, "name", pid->plugin_name) == پوچ
			|| (pid->plugin_version && cJSON_AddStringToObject(j_plugin, "version", pid->plugin_version) == پوچ)
			|| (pid->listener && c_JSON_Add_عدد_To_Object(j_plugin, "port", pid->listener->port) == پوچ)
			|| (j_eps = cJSON_AddArrayToObject(j_plugin, "control-endpoints")) == پوچ
			){

		cJSON_Delete(j_plugin);
		بازگشت MOSQ_ERR_NOMEM;
	}

	DL_FOREACH(pid->control_endpoints, ep){
		j_ep = cJSON_CreateString(ep->topic);
		اگر(j_ep == پوچ){
			cJSON_Delete(j_plugin);
			بازگشت MOSQ_ERR_NOMEM;
		}
		cJSON_AddItemToArray(j_eps, j_ep);
	}

	cJSON_AddItemToArray(j_plugins, j_plugin);
	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح broker__process_list_plugins(ساختار mosquitto_control_cmd *cmd)
{
	cJSON *درخت, *j_data, *j_plugins;
	ثابت نویسه *admin_clientid, *admin_username;

	درخت = cJSON_CreateObject();
	اگر(درخت == پوچ){
		mosquitto_control_command_reply(cmd, "Internal error");
		بازگشت MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listPlugins",
			admin_clientid, admin_username);

	اگر(cJSON_AddStringToObject(درخت, "command", "listPlugins") == پوچ
			|| ((j_data = cJSON_AddObjectToObject(درخت, "data")) == پوچ)
			|| (cmd->correlation_data && cJSON_AddStringToObject(درخت, "correlationData", cmd->correlation_data) == پوچ)
			){

		برو internal_error;
	}

	j_plugins = cJSON_AddArrayToObject(j_data, "plugins");
	اگر(j_plugins == پوچ){
		برو internal_error;
	}

	برای(صحیح i=0; i<db.plugin_شمار; i++){
		اگر(add_plugin_info(j_plugins, db.plugins[i])){
			برو internal_error;
		}
	}

	cJSON_AddItemToArray(cmd->j_responses, درخت);

	بازگشت MOSQ_ERR_SUCCESS;

internal_error:
	cJSON_Delete(درخت);
	mosquitto_control_command_reply(cmd, "Internal error");
	بازگشت MOSQ_ERR_NOMEM;
}


ایستا صحیح add_listener(cJSON *j_listeners, ساختار mosquitto__listener *listener)
{
	cJSON *j_listener;
	ثابت نویسه *protocol = پوچ;

	j_listener = cJSON_CreateObject();
	اگر(j_listener == پوچ){
		بازگشت MOSQ_ERR_NOMEM;
	}
	cJSON_AddItemToArray(j_listeners, j_listener);

	اگر(listener->protocol == mp_mqtt){
		protocol = "mqtt";
	}وگرنه اگر(listener->protocol == mp_websockets){
		protocol = "mqtt+websockets";
	}

	اگر(c_JSON_Add_عدد_To_Object(j_listener, "port", listener->port) == پوچ
			|| (protocol && cJSON_AddStringToObject(j_listener, "protocol", protocol) == پوچ)
			|| (listener->host && cJSON_AddStringToObject(j_listener, "bind-address", listener->host) == پوچ)
#ifdef WITH_UNIX_SOCKETS
			|| (listener->unix_socket_path && cJSON_AddStringToObject(j_listener, "socket-path", listener->unix_socket_path) == پوچ)
#endif
			){

		بازگشت MOSQ_ERR_NOMEM;
	}

	اگر(
#ifdef WITH_TLS
			cJSON_AddBoolToObject(j_listener, "tls", listener->ssl_ctx != پوچ) == پوچ
#وگرنه
			cJSON_AddBoolToObject(j_listener, "tls", false) == پوچ
#endif
			){

		بازگشت MOSQ_ERR_NOMEM;
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح broker__process_list_listeners(ساختار mosquitto_control_cmd *cmd)
{
	cJSON *درخت, *j_data, *j_listeners;
	ثابت نویسه *admin_clientid, *admin_username;

	درخت = cJSON_CreateObject();
	اگر(درخت == پوچ){
		mosquitto_control_command_reply(cmd, "Internal error");
		بازگشت MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listListeners",
			admin_clientid, admin_username);

	اگر(cJSON_AddStringToObject(درخت, "command", "listListeners") == پوچ
			|| ((j_data = cJSON_AddObjectToObject(درخت, "data")) == پوچ)
			|| (cmd->correlation_data && cJSON_AddStringToObject(درخت, "correlationData", cmd->correlation_data) == پوچ)
			){

		برو internal_error;
	}

	j_listeners = cJSON_AddArrayToObject(j_data, "listeners");
	اگر(j_listeners == پوچ){
		برو internal_error;
	}

	برای(صحیح i=0; i<db.config->listener_شمار; i++){
		اگر(add_listener(j_listeners, &db.config->listeners[i])){
			برو internal_error;
		}
	}

	cJSON_AddItemToArray(cmd->j_responses, درخت);

	بازگشت MOSQ_ERR_SUCCESS;

internal_error:
	cJSON_Delete(درخت);
	mosquitto_control_command_reply(cmd, "Internal error");
	بازگشت MOSQ_ERR_NOMEM;
}


ایستا صحیح broker_control_callback(صحیح event, تهی *event_data, تهی *userdata)
{
	ساختار mosquitto_evt_control *ed = event_data;

	UNUSED(event);

	بازگشت mosquitto_control_generic_callback(ed, "$CONTROL/broker/v1/response", userdata, broker__handle_control);
}


تهی broker_control__init(تهی)
{
	memset(&plg_id, 0, اندازه(plg_id));

	اگر(db.config->enable_control_api){
		mosquitto_callback_register(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1", پوچ);
	}
}


تهی broker_control__cleanup(تهی)
{
	mosquitto_callback_unregister(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1");
}


تهی broker_control__reload(تهی)
{
	broker_control__cleanup();
	broker_control__init();
}


/* ################################################################
 * #
 * # $CONTROL/broker/v1 handler
 * #
 * ################################################################ */


ایستا صحیح broker__handle_control(ساختار mosquitto_control_cmd *cmd, تهی *userdata)
{
	صحیح rc = MOSQ_ERR_SUCCESS;

	UNUSED(userdata);

	اگر(!strcasecmp(cmd->command_name, "listPlugins")){
		rc = broker__process_list_plugins(cmd);
	}وگرنه اگر(!strcasecmp(cmd->command_name, "listListeners")){
		rc = broker__process_list_listeners(cmd);

		/* Unknown */
	}وگرنه{
		mosquitto_control_command_reply(cmd, "Unknown command");
		rc = MOSQ_ERR_INVAL;
	}
	بازگشت rc;
}
