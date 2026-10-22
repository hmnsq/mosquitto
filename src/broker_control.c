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


ساكن صحيح add_plugin_info(cJSON *j_plugins, mosquitto_plugin_id_t *pid)
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

	إذا(cJSON_AddStringToObject(j_plugin, "name", pid->plugin_name) == عدم
			|| (pid->plugin_version && cJSON_AddStringToObject(j_plugin, "version", pid->plugin_version) == عدم)
			|| (pid->listener && c_JSON_Add_سان_To_Object(j_plugin, "port", pid->listener->port) == عدم)
			|| (j_eps = cJSON_AddArrayToObject(j_plugin, "control-endpoints")) == عدم
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
		cJSON_AddItemToArray(j_eps, j_ep);
	}

	cJSON_AddItemToArray(j_plugins, j_plugin);
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح broker__process_list_plugins(بنية mosquitto_control_cmd *cmd)
{
	cJSON *دەرەخ, *j_data, *j_plugins;
	ثابت حرف *admin_clientid, *admin_username;

	دەرەخ = cJSON_CreateObject();
	إذا(دەرەخ == عدم){
		mosquitto_control_command_reply(cmd, "Internal error");
		أرجع MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listPlugins",
			admin_clientid, admin_username);

	إذا(cJSON_AddStringToObject(دەرەخ, "command", "listPlugins") == عدم
			|| ((j_data = cJSON_AddObjectToObject(دەرەخ, "data")) == عدم)
			|| (cmd->correlation_data && cJSON_AddStringToObject(دەرەخ, "correlationData", cmd->correlation_data) == عدم)
			){

		اذهب internal_error;
	}

	j_plugins = cJSON_AddArrayToObject(j_data, "plugins");
	إذا(j_plugins == عدم){
		اذهب internal_error;
	}

	لكل(صحيح i=0; i<db.plugin_سانى; i++){
		إذا(add_plugin_info(j_plugins, db.plugins[i])){
			اذهب internal_error;
		}
	}

	cJSON_AddItemToArray(cmd->j_responses, دەرەخ);

	أرجع MOSQ_ERR_SUCCESS;

internal_error:
	cJSON_Delete(دەرەخ);
	mosquitto_control_command_reply(cmd, "Internal error");
	أرجع MOSQ_ERR_NOMEM;
}


ساكن صحيح add_listener(cJSON *j_listeners, بنية mosquitto__listener *listener)
{
	cJSON *j_listener;
	ثابت حرف *protocol = عدم;

	j_listener = cJSON_CreateObject();
	إذا(j_listener == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}
	cJSON_AddItemToArray(j_listeners, j_listener);

	إذا(listener->protocol == mp_mqtt){
		protocol = "mqtt";
	}وإلا إذا(listener->protocol == mp_websockets){
		protocol = "mqtt+websockets";
	}

	إذا(c_JSON_Add_سان_To_Object(j_listener, "port", listener->port) == عدم
			|| (protocol && cJSON_AddStringToObject(j_listener, "protocol", protocol) == عدم)
			|| (listener->host && cJSON_AddStringToObject(j_listener, "bind-address", listener->host) == عدم)
#ifdef WITH_UNIX_SOCKETS
			|| (listener->unix_socket_path && cJSON_AddStringToObject(j_listener, "socket-path", listener->unix_socket_path) == عدم)
#endif
			){

		أرجع MOSQ_ERR_NOMEM;
	}

	إذا(
#ifdef WITH_TLS
			cJSON_AddBoolToObject(j_listener, "tls", listener->ssl_ctx != عدم) == عدم
#وإلا
			cJSON_AddBoolToObject(j_listener, "tls", false) == عدم
#endif
			){

		أرجع MOSQ_ERR_NOMEM;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح broker__process_list_listeners(بنية mosquitto_control_cmd *cmd)
{
	cJSON *دەرەخ, *j_data, *j_listeners;
	ثابت حرف *admin_clientid, *admin_username;

	دەرەخ = cJSON_CreateObject();
	إذا(دەرەخ == عدم){
		mosquitto_control_command_reply(cmd, "Internal error");
		أرجع MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listListeners",
			admin_clientid, admin_username);

	إذا(cJSON_AddStringToObject(دەرەخ, "command", "listListeners") == عدم
			|| ((j_data = cJSON_AddObjectToObject(دەرەخ, "data")) == عدم)
			|| (cmd->correlation_data && cJSON_AddStringToObject(دەرەخ, "correlationData", cmd->correlation_data) == عدم)
			){

		اذهب internal_error;
	}

	j_listeners = cJSON_AddArrayToObject(j_data, "listeners");
	إذا(j_listeners == عدم){
		اذهب internal_error;
	}

	لكل(صحيح i=0; i<db.config->listener_سانى; i++){
		إذا(add_listener(j_listeners, &db.config->listeners[i])){
			اذهب internal_error;
		}
	}

	cJSON_AddItemToArray(cmd->j_responses, دەرەخ);

	أرجع MOSQ_ERR_SUCCESS;

internal_error:
	cJSON_Delete(دەرەخ);
	mosquitto_control_command_reply(cmd, "Internal error");
	أرجع MOSQ_ERR_NOMEM;
}


ساكن صحيح broker_control_callback(صحيح event, فراغ *event_data, فراغ *userdata)
{
	بنية mosquitto_evt_control *ed = event_data;

	UNUSED(event);

	أرجع mosquitto_control_generic_callback(ed, "$CONTROL/broker/v1/response", userdata, broker__handle_control);
}


فراغ broker_control__init(فراغ)
{
	memset(&plg_id, 0, حجم(plg_id));

	إذا(db.config->enable_control_api){
		mosquitto_callback_register(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1", عدم);
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

	UNUSED(userdata);

	إذا(!strcasecmp(cmd->command_name, "listPlugins")){
		rc = broker__process_list_plugins(cmd);
	}وإلا إذا(!strcasecmp(cmd->command_name, "listListeners")){
		rc = broker__process_list_listeners(cmd);

		/* Unknown */
	}وإلا{
		mosquitto_control_command_reply(cmd, "Unknown command");
		rc = MOSQ_ERR_INVAL;
	}
	أرجع rc;
}
