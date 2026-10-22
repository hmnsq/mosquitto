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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <cjson/cJSON.h>
#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ಅನ್ತರ್ಭಾವ <utlist.h>

#ಅನ್ತರ್ಭಾವ "json_help.h"
#ಅನ್ತರ್ಭಾವ "mosquitto.h"
#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/broker.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/broker_control.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/broker_plugin.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"

ಸ್ಥಿರ mosquitto_plugin_id_t plg_id;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ broker__handle_control(ಸಂರಚನಾ mosquitto_control_cmd *cmd, ಶೂನ್ಯ *userdata);


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ add_plugin_info(cJSON *j_plugins, mosquitto_plugin_id_t *pid)
{
	cJSON *j_plugin, *j_eps, *j_ep;
	ಸಂರಚನಾ control_endpoint *ep;

	ಯದಿ(pid->plugin_name == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}

	j_plugin = cJSON_CreateObject();
	ಯದಿ(j_plugin == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}

	ಯದಿ(cJSON_AddStringToObject(j_plugin, "name", pid->plugin_name) == ರಿಕ್ತ
			|| (pid->plugin_version && cJSON_AddStringToObject(j_plugin, "version", pid->plugin_version) == ರಿಕ್ತ)
			|| (pid->listener && c_JSON_Add_ಸಂಖ್ಯೆ_To_Object(j_plugin, "port", pid->listener->port) == ರಿಕ್ತ)
			|| (j_eps = cJSON_AddArrayToObject(j_plugin, "control-endpoints")) == ರಿಕ್ತ
			){

		cJSON_Delete(j_plugin);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}

	DL_FOREACH(pid->control_endpoints, ep){
		j_ep = cJSON_CreateString(ep->topic);
		ಯದಿ(j_ep == ರಿಕ್ತ){
			cJSON_Delete(j_plugin);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}
		cJSON_AddItemToArray(j_eps, j_ep);
	}

	cJSON_AddItemToArray(j_plugins, j_plugin);
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ broker__process_list_plugins(ಸಂರಚನಾ mosquitto_control_cmd *cmd)
{
	cJSON *ವೃಕ್ಷ, *j_data, *j_plugins;
	ಧ್ರುವ ವರ್ಣ *admin_clientid, *admin_username;

	ವೃಕ್ಷ = cJSON_CreateObject();
	ಯದಿ(ವೃಕ್ಷ == ರಿಕ್ತ){
		mosquitto_control_command_reply(cmd, "Internal error");
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listPlugins",
			admin_clientid, admin_username);

	ಯದಿ(cJSON_AddStringToObject(ವೃಕ್ಷ, "command", "listPlugins") == ರಿಕ್ತ
			|| ((j_data = cJSON_AddObjectToObject(ವೃಕ್ಷ, "data")) == ರಿಕ್ತ)
			|| (cmd->correlation_data && cJSON_AddStringToObject(ವೃಕ್ಷ, "correlationData", cmd->correlation_data) == ರಿಕ್ತ)
			){

		ಗಚ್ಛ internal_error;
	}

	j_plugins = cJSON_AddArrayToObject(j_data, "plugins");
	ಯದಿ(j_plugins == ರಿಕ್ತ){
		ಗಚ್ಛ internal_error;
	}

	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<db.plugin_ಎಣಿಕೆ; i++){
		ಯದಿ(add_plugin_info(j_plugins, db.plugins[i])){
			ಗಚ್ಛ internal_error;
		}
	}

	cJSON_AddItemToArray(cmd->j_responses, ವೃಕ್ಷ);

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;

internal_error:
	cJSON_Delete(ವೃಕ್ಷ);
	mosquitto_control_command_reply(cmd, "Internal error");
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ add_listener(cJSON *j_listeners, ಸಂರಚನಾ mosquitto__listener *listener)
{
	cJSON *j_listener;
	ಧ್ರುವ ವರ್ಣ *protocol = ರಿಕ್ತ;

	j_listener = cJSON_CreateObject();
	ಯದಿ(j_listener == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}
	cJSON_AddItemToArray(j_listeners, j_listener);

	ಯದಿ(listener->protocol == mp_mqtt){
		protocol = "mqtt";
	}ಅನ್ಯಥಾ ಯದಿ(listener->protocol == mp_websockets){
		protocol = "mqtt+websockets";
	}

	ಯದಿ(c_JSON_Add_ಸಂಖ್ಯೆ_To_Object(j_listener, "port", listener->port) == ರಿಕ್ತ
			|| (protocol && cJSON_AddStringToObject(j_listener, "protocol", protocol) == ರಿಕ್ತ)
			|| (listener->host && cJSON_AddStringToObject(j_listener, "bind-address", listener->host) == ರಿಕ್ತ)
#ifdef WITH_UNIX_SOCKETS
			|| (listener->unix_socket_path && cJSON_AddStringToObject(j_listener, "socket-path", listener->unix_socket_path) == ರಿಕ್ತ)
#endif
			){

		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}

	ಯದಿ(
#ifdef WITH_TLS
			cJSON_AddBoolToObject(j_listener, "tls", listener->ssl_ctx != ರಿಕ್ತ) == ರಿಕ್ತ
#ಅನ್ಯಥಾ
			cJSON_AddBoolToObject(j_listener, "tls", false) == ರಿಕ್ತ
#endif
			){

		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ broker__process_list_listeners(ಸಂರಚನಾ mosquitto_control_cmd *cmd)
{
	cJSON *ವೃಕ್ಷ, *j_data, *j_listeners;
	ಧ್ರುವ ವರ್ಣ *admin_clientid, *admin_username;

	ವೃಕ್ಷ = cJSON_CreateObject();
	ಯದಿ(ವೃಕ್ಷ == ರಿಕ್ತ){
		mosquitto_control_command_reply(cmd, "Internal error");
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listListeners",
			admin_clientid, admin_username);

	ಯದಿ(cJSON_AddStringToObject(ವೃಕ್ಷ, "command", "listListeners") == ರಿಕ್ತ
			|| ((j_data = cJSON_AddObjectToObject(ವೃಕ್ಷ, "data")) == ರಿಕ್ತ)
			|| (cmd->correlation_data && cJSON_AddStringToObject(ವೃಕ್ಷ, "correlationData", cmd->correlation_data) == ರಿಕ್ತ)
			){

		ಗಚ್ಛ internal_error;
	}

	j_listeners = cJSON_AddArrayToObject(j_data, "listeners");
	ಯದಿ(j_listeners == ರಿಕ್ತ){
		ಗಚ್ಛ internal_error;
	}

	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
		ಯದಿ(add_listener(j_listeners, &db.config->listeners[i])){
			ಗಚ್ಛ internal_error;
		}
	}

	cJSON_AddItemToArray(cmd->j_responses, ವೃಕ್ಷ);

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;

internal_error:
	cJSON_Delete(ವೃಕ್ಷ);
	mosquitto_control_command_reply(cmd, "Internal error");
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ broker_control_callback(ಪೂರ್ಣಾಙ್ಕ event, ಶೂನ್ಯ *event_data, ಶೂನ್ಯ *userdata)
{
	ಸಂರಚನಾ mosquitto_evt_control *ed = event_data;

	UNUSED(event);

	ಪ್ರತ್ಯಾಗಮ mosquitto_control_generic_callback(ed, "$CONTROL/broker/v1/response", userdata, broker__handle_control);
}


ಶೂನ್ಯ broker_control__init(ಶೂನ್ಯ)
{
	memset(&plg_id, 0, ಪರಿಮಾಣ(plg_id));

	ಯದಿ(db.config->enable_control_api){
		mosquitto_callback_register(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1", ರಿಕ್ತ);
	}
}


ಶೂನ್ಯ broker_control__cleanup(ಶೂನ್ಯ)
{
	mosquitto_callback_unregister(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1");
}


ಶೂನ್ಯ broker_control__reload(ಶೂನ್ಯ)
{
	broker_control__cleanup();
	broker_control__init();
}


/* ################################################################
 * #
 * # $CONTROL/broker/v1 handler
 * #
 * ################################################################ */


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ broker__handle_control(ಸಂರಚನಾ mosquitto_control_cmd *cmd, ಶೂನ್ಯ *userdata)
{
	ಪೂರ್ಣಾಙ್ಕ rc = MOSQ_ERR_SUCCESS;

	UNUSED(userdata);

	ಯದಿ(!strcasecmp(cmd->command_name, "listPlugins")){
		rc = broker__process_list_plugins(cmd);
	}ಅನ್ಯಥಾ ಯದಿ(!strcasecmp(cmd->command_name, "listListeners")){
		rc = broker__process_list_listeners(cmd);

		/* Unknown */
	}ಅನ್ಯಥಾ{
		mosquitto_control_command_reply(cmd, "Unknown command");
		rc = MOSQ_ERR_INVAL;
	}
	ಪ್ರತ್ಯಾಗಮ rc;
}
