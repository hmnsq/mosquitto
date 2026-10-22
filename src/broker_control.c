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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ <cjson/cJSON.h>
#അന്തര്ഭാവ <errno.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <sys/stat.h>
#അന്തര്ഭാവ <utlist.h>

#അന്തര്ഭാവ "json_help.h"
#അന്തര്ഭാവ "mosquitto.h"
#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/broker.h"
#അന്തര്ഭാവ "mosquitto/broker_control.h"
#അന്തര്ഭാവ "mosquitto/broker_plugin.h"
#അന്തര്ഭാവ "mosquitto/mqtt_protocol.h"

സ്ഥിതം mosquitto_plugin_id_t plg_id;
സ്ഥിതം പൂർണ്ണസംഖ്യ broker__handle_control(ഘടന mosquitto_control_cmd *cmd, ശൂന്യം *userdata);


സ്ഥിതം പൂർണ്ണസംഖ്യ add_plugin_info(cJSON *j_plugins, mosquitto_plugin_id_t *pid)
{
	cJSON *j_plugin, *j_eps, *j_ep;
	ഘടന control_endpoint *ep;

	എങ്കിൽ(pid->plugin_name == ശൂന്യ){
		തിരികെ MOSQ_ERR_SUCCESS;
	}

	j_plugin = cJSON_CreateObject();
	എങ്കിൽ(j_plugin == ശൂന്യ){
		തിരികെ MOSQ_ERR_NOMEM;
	}

	എങ്കിൽ(cJSON_AddStringToObject(j_plugin, "name", pid->plugin_name) == ശൂന്യ
			|| (pid->plugin_version && cJSON_AddStringToObject(j_plugin, "version", pid->plugin_version) == ശൂന്യ)
			|| (pid->listener && c_JSON_Add_സംഖ്യ_To_Object(j_plugin, "port", pid->listener->port) == ശൂന്യ)
			|| (j_eps = cJSON_AddArrayToObject(j_plugin, "control-endpoints")) == ശൂന്യ
			){

		cJSON_Delete(j_plugin);
		തിരികെ MOSQ_ERR_NOMEM;
	}

	DL_FOREACH(pid->control_endpoints, ep){
		j_ep = cJSON_CreateString(ep->topic);
		എങ്കിൽ(j_ep == ശൂന്യ){
			cJSON_Delete(j_plugin);
			തിരികെ MOSQ_ERR_NOMEM;
		}
		cJSON_AddItemToArray(j_eps, j_ep);
	}

	cJSON_AddItemToArray(j_plugins, j_plugin);
	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ broker__process_list_plugins(ഘടന mosquitto_control_cmd *cmd)
{
	cJSON *വൃക്ഷം, *j_data, *j_plugins;
	സ്ഥിരം അക്ഷരം *admin_clientid, *admin_username;

	വൃക്ഷം = cJSON_CreateObject();
	എങ്കിൽ(വൃക്ഷം == ശൂന്യ){
		mosquitto_control_command_reply(cmd, "Internal error");
		തിരികെ MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listPlugins",
			admin_clientid, admin_username);

	എങ്കിൽ(cJSON_AddStringToObject(വൃക്ഷം, "command", "listPlugins") == ശൂന്യ
			|| ((j_data = cJSON_AddObjectToObject(വൃക്ഷം, "data")) == ശൂന്യ)
			|| (cmd->correlation_data && cJSON_AddStringToObject(വൃക്ഷം, "correlationData", cmd->correlation_data) == ശൂന്യ)
			){

		പോകുക internal_error;
	}

	j_plugins = cJSON_AddArrayToObject(j_data, "plugins");
	എങ്കിൽ(j_plugins == ശൂന്യ){
		പോകുക internal_error;
	}

	വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<db.plugin_എണ്ണം; i++){
		എങ്കിൽ(add_plugin_info(j_plugins, db.plugins[i])){
			പോകുക internal_error;
		}
	}

	cJSON_AddItemToArray(cmd->j_responses, വൃക്ഷം);

	തിരികെ MOSQ_ERR_SUCCESS;

internal_error:
	cJSON_Delete(വൃക്ഷം);
	mosquitto_control_command_reply(cmd, "Internal error");
	തിരികെ MOSQ_ERR_NOMEM;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ add_listener(cJSON *j_listeners, ഘടന mosquitto__listener *listener)
{
	cJSON *j_listener;
	സ്ഥിരം അക്ഷരം *protocol = ശൂന്യ;

	j_listener = cJSON_CreateObject();
	എങ്കിൽ(j_listener == ശൂന്യ){
		തിരികെ MOSQ_ERR_NOMEM;
	}
	cJSON_AddItemToArray(j_listeners, j_listener);

	എങ്കിൽ(listener->protocol == mp_mqtt){
		protocol = "mqtt";
	}അല്ലെങ്കിൽ എങ്കിൽ(listener->protocol == mp_websockets){
		protocol = "mqtt+websockets";
	}

	എങ്കിൽ(c_JSON_Add_സംഖ്യ_To_Object(j_listener, "port", listener->port) == ശൂന്യ
			|| (protocol && cJSON_AddStringToObject(j_listener, "protocol", protocol) == ശൂന്യ)
			|| (listener->host && cJSON_AddStringToObject(j_listener, "bind-address", listener->host) == ശൂന്യ)
#ifdef WITH_UNIX_SOCKETS
			|| (listener->unix_socket_path && cJSON_AddStringToObject(j_listener, "socket-path", listener->unix_socket_path) == ശൂന്യ)
#endif
			){

		തിരികെ MOSQ_ERR_NOMEM;
	}

	എങ്കിൽ(
#ifdef WITH_TLS
			cJSON_AddBoolToObject(j_listener, "tls", listener->ssl_ctx != ശൂന്യ) == ശൂന്യ
#അല്ലെങ്കിൽ
			cJSON_AddBoolToObject(j_listener, "tls", false) == ശൂന്യ
#endif
			){

		തിരികെ MOSQ_ERR_NOMEM;
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ broker__process_list_listeners(ഘടന mosquitto_control_cmd *cmd)
{
	cJSON *വൃക്ഷം, *j_data, *j_listeners;
	സ്ഥിരം അക്ഷരം *admin_clientid, *admin_username;

	വൃക്ഷം = cJSON_CreateObject();
	എങ്കിൽ(വൃക്ഷം == ശൂന്യ){
		mosquitto_control_command_reply(cmd, "Internal error");
		തിരികെ MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listListeners",
			admin_clientid, admin_username);

	എങ്കിൽ(cJSON_AddStringToObject(വൃക്ഷം, "command", "listListeners") == ശൂന്യ
			|| ((j_data = cJSON_AddObjectToObject(വൃക്ഷം, "data")) == ശൂന്യ)
			|| (cmd->correlation_data && cJSON_AddStringToObject(വൃക്ഷം, "correlationData", cmd->correlation_data) == ശൂന്യ)
			){

		പോകുക internal_error;
	}

	j_listeners = cJSON_AddArrayToObject(j_data, "listeners");
	എങ്കിൽ(j_listeners == ശൂന്യ){
		പോകുക internal_error;
	}

	വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<db.config->listener_എണ്ണം; i++){
		എങ്കിൽ(add_listener(j_listeners, &db.config->listeners[i])){
			പോകുക internal_error;
		}
	}

	cJSON_AddItemToArray(cmd->j_responses, വൃക്ഷം);

	തിരികെ MOSQ_ERR_SUCCESS;

internal_error:
	cJSON_Delete(വൃക്ഷം);
	mosquitto_control_command_reply(cmd, "Internal error");
	തിരികെ MOSQ_ERR_NOMEM;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ broker_control_callback(പൂർണ്ണസംഖ്യ event, ശൂന്യം *event_data, ശൂന്യം *userdata)
{
	ഘടന mosquitto_evt_control *ed = event_data;

	UNUSED(event);

	തിരികെ mosquitto_control_generic_callback(ed, "$CONTROL/broker/v1/response", userdata, broker__handle_control);
}


ശൂന്യം broker_control__init(ശൂന്യം)
{
	memset(&plg_id, 0, വലുപ്പം(plg_id));

	എങ്കിൽ(db.config->enable_control_api){
		mosquitto_callback_register(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1", ശൂന്യ);
	}
}


ശൂന്യം broker_control__cleanup(ശൂന്യം)
{
	mosquitto_callback_unregister(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1");
}


ശൂന്യം broker_control__reload(ശൂന്യം)
{
	broker_control__cleanup();
	broker_control__init();
}


/* ################################################################
 * #
 * # $CONTROL/broker/v1 handler
 * #
 * ################################################################ */


സ്ഥിതം പൂർണ്ണസംഖ്യ broker__handle_control(ഘടന mosquitto_control_cmd *cmd, ശൂന്യം *userdata)
{
	പൂർണ്ണസംഖ്യ rc = MOSQ_ERR_SUCCESS;

	UNUSED(userdata);

	എങ്കിൽ(!strcasecmp(cmd->command_name, "listPlugins")){
		rc = broker__process_list_plugins(cmd);
	}അല്ലെങ്കിൽ എങ്കിൽ(!strcasecmp(cmd->command_name, "listListeners")){
		rc = broker__process_list_listeners(cmd);

		/* Unknown */
	}അല്ലെങ്കിൽ{
		mosquitto_control_command_reply(cmd, "Unknown command");
		rc = MOSQ_ERR_INVAL;
	}
	തിരികെ rc;
}
