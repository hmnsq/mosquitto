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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ broker__handle_control(ರಚನೆ mosquitto_control_cmd *cmd, ಶೂನ್ಯ *userdata);


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_plugin_info(cJSON *j_plugins, mosquitto_plugin_id_t *pid)
{
	cJSON *j_plugin, *j_eps, *j_ep;
	ರಚನೆ control_endpoint *ep;

	ಆದರೆ(pid->plugin_name == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}

	j_plugin = cJSON_CreateObject();
	ಆದರೆ(j_plugin == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}

	ಆದರೆ(c_JSON_ಕೂಡಿಸು_String_ಗಮ್ಯ_Object(j_plugin, "name", pid->plugin_name) == ಖಾಲಿ
			|| (pid->plugin_ಆವೃತ್ತಿ && c_JSON_ಕೂಡಿಸು_String_ಗಮ್ಯ_Object(j_plugin, "version", pid->plugin_ಆವೃತ್ತಿ) == ಖಾಲಿ)
			|| (pid->listener && c_JSON_ಕೂಡಿಸು_ಸಂಖ್ಯೆ_ಗಮ್ಯ_Object(j_plugin, "port", pid->listener->ಪೋರ್ಟ್) == ಖಾಲಿ)
			|| (j_eps = c_JSON_ಕೂಡಿಸು_Array_ಗಮ್ಯ_Object(j_plugin, "control-endpoints")) == ಖಾಲಿ
			){

		cJSON_Delete(j_plugin);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}

	DL_FOREACH(pid->control_endpoints, ep){
		j_ep = cJSON_CreateString(ep->topic);
		ಆದರೆ(j_ep == ಖಾಲಿ){
			cJSON_Delete(j_plugin);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}
		c_JSON_ಕೂಡಿಸು_ಅಂಶ_ಗಮ್ಯ_Array(j_eps, j_ep);
	}

	c_JSON_ಕೂಡಿಸು_ಅಂಶ_ಗಮ್ಯ_Array(j_plugins, j_plugin);
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ broker__process_list_plugins(ರಚನೆ mosquitto_control_cmd *cmd)
{
	cJSON *ವೃಕ್ಷ, *j_ದತ್ತಾಂಶ, *j_plugins;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *admin_clientid, *admin_username;

	ವೃಕ್ಷ = cJSON_CreateObject();
	ಆದರೆ(ವೃಕ್ಷ == ಖಾಲಿ){
		mosquitto_control_command_ಉತ್ತರ(cmd, "Internal error");
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listPlugins",
			admin_clientid, admin_username);

	ಆದರೆ(c_JSON_ಕೂಡಿಸು_String_ಗಮ್ಯ_Object(ವೃಕ್ಷ, "command", "listPlugins") == ಖಾಲಿ
			|| ((j_ದತ್ತಾಂಶ = c_JSON_ಕೂಡಿಸು_Object_ಗಮ್ಯ_Object(ವೃಕ್ಷ, "data")) == ಖಾಲಿ)
			|| (cmd->correlation_ದತ್ತಾಂಶ && c_JSON_ಕೂಡಿಸು_String_ಗಮ್ಯ_Object(ವೃಕ್ಷ, "correlationData", cmd->correlation_ದತ್ತಾಂಶ) == ಖಾಲಿ)
			){

		ಹೋಗು ಆಂತರಿಕ_error;
	}

	j_plugins = c_JSON_ಕೂಡಿಸು_Array_ಗಮ್ಯ_Object(j_ದತ್ತಾಂಶ, "plugins");
	ಆದರೆ(j_plugins == ಖಾಲಿ){
		ಹೋಗು ಆಂತರಿಕ_error;
	}

	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<db.plugin_ಎಣಿಕೆ; i++){
		ಆದರೆ(ಕೂಡಿಸು_plugin_info(j_plugins, db.plugins[i])){
			ಹೋಗು ಆಂತರಿಕ_error;
		}
	}

	c_JSON_ಕೂಡಿಸು_ಅಂಶ_ಗಮ್ಯ_Array(cmd->j_responses, ವೃಕ್ಷ);

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;

ಆಂತರಿಕ_error:
	cJSON_Delete(ವೃಕ್ಷ);
	mosquitto_control_command_ಉತ್ತರ(cmd, "Internal error");
	ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_listener(cJSON *j_listeners, ರಚನೆ mosquitto__listener *listener)
{
	cJSON *j_listener;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *protocol = ಖಾಲಿ;

	j_listener = cJSON_CreateObject();
	ಆದರೆ(j_listener == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}
	c_JSON_ಕೂಡಿಸು_ಅಂಶ_ಗಮ್ಯ_Array(j_listeners, j_listener);

	ಆದರೆ(listener->protocol == mp_mqtt){
		protocol = "mqtt";
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(listener->protocol == mp_websockets){
		protocol = "mqtt+websockets";
	}

	ಆದರೆ(c_JSON_ಕೂಡಿಸು_ಸಂಖ್ಯೆ_ಗಮ್ಯ_Object(j_listener, "port", listener->ಪೋರ್ಟ್) == ಖಾಲಿ
			|| (protocol && c_JSON_ಕೂಡಿಸು_String_ಗಮ್ಯ_Object(j_listener, "protocol", protocol) == ಖಾಲಿ)
			|| (listener->host && c_JSON_ಕೂಡಿಸು_String_ಗಮ್ಯ_Object(j_listener, "bind-address", listener->host) == ಖಾಲಿ)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_UNIX_SOCKETS
			|| (listener->unix_socket_ಮಾರ್ಗ && c_JSON_ಕೂಡಿಸು_String_ಗಮ್ಯ_Object(j_listener, "socket-path", listener->unix_socket_ಮಾರ್ಗ) == ಖಾಲಿ)
#ಆದರೆ_ಅಂತ್ಯ
			){

		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}

	ಆದರೆ(
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
			c_JSON_ಕೂಡಿಸು_Bool_ಗಮ್ಯ_Object(j_listener, "tls", listener->ssl_ctx != ಖಾಲಿ) == ಖಾಲಿ
#ಇಲ್ಲದಿದ್ದರೆ
			c_JSON_ಕೂಡಿಸು_Bool_ಗಮ್ಯ_Object(j_listener, "tls", false) == ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
			){

		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ broker__process_list_listeners(ರಚನೆ mosquitto_control_cmd *cmd)
{
	cJSON *ವೃಕ್ಷ, *j_ದತ್ತಾಂಶ, *j_listeners;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *admin_clientid, *admin_username;

	ವೃಕ್ಷ = cJSON_CreateObject();
	ಆದರೆ(ವೃಕ್ಷ == ಖಾಲಿ){
		mosquitto_control_command_ಉತ್ತರ(cmd, "Internal error");
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listListeners",
			admin_clientid, admin_username);

	ಆದರೆ(c_JSON_ಕೂಡಿಸು_String_ಗಮ್ಯ_Object(ವೃಕ್ಷ, "command", "listListeners") == ಖಾಲಿ
			|| ((j_ದತ್ತಾಂಶ = c_JSON_ಕೂಡಿಸು_Object_ಗಮ್ಯ_Object(ವೃಕ್ಷ, "data")) == ಖಾಲಿ)
			|| (cmd->correlation_ದತ್ತಾಂಶ && c_JSON_ಕೂಡಿಸು_String_ಗಮ್ಯ_Object(ವೃಕ್ಷ, "correlationData", cmd->correlation_ದತ್ತಾಂಶ) == ಖಾಲಿ)
			){

		ಹೋಗು ಆಂತರಿಕ_error;
	}

	j_listeners = c_JSON_ಕೂಡಿಸು_Array_ಗಮ್ಯ_Object(j_ದತ್ತಾಂಶ, "listeners");
	ಆದರೆ(j_listeners == ಖಾಲಿ){
		ಹೋಗು ಆಂತರಿಕ_error;
	}

	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
		ಆದರೆ(ಕೂಡಿಸು_listener(j_listeners, &db.config->listeners[i])){
			ಹೋಗು ಆಂತರಿಕ_error;
		}
	}

	c_JSON_ಕೂಡಿಸು_ಅಂಶ_ಗಮ್ಯ_Array(cmd->j_responses, ವೃಕ್ಷ);

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;

ಆಂತರಿಕ_error:
	cJSON_Delete(ವೃಕ್ಷ);
	mosquitto_control_command_ಉತ್ತರ(cmd, "Internal error");
	ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ broker_control_callback(ಪೂರ್ಣಾಂಕ event, ಶೂನ್ಯ *event_ದತ್ತಾಂಶ, ಶೂನ್ಯ *userdata)
{
	ರಚನೆ mosquitto_evt_control *ed = event_ದತ್ತಾಂಶ;

	ಬಳಸದ(event);

	ಹಿಂತಿರುಗಿ mosquitto_control_generic_callback(ed, "$CONTROL/broker/v1/response", userdata, broker__handle_control);
}


ಶೂನ್ಯ broker_control__init(ಶೂನ್ಯ)
{
	memset(&plg_id, 0, ಗಾತ್ರ(plg_id));

	ಆದರೆ(db.config->enable_control_api){
		mosquitto_callback_ರಿಜಿಸ್ಟರ್(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1", ಖಾಲಿ);
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


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ broker__handle_control(ರಚನೆ mosquitto_control_cmd *cmd, ಶೂನ್ಯ *userdata)
{
	ಪೂರ್ಣಾಂಕ rc = MOSQ_ERR_SUCCESS;

	ಬಳಸದ(userdata);

	ಆದರೆ(!strcasecmp(cmd->command_name, "listPlugins")){
		rc = broker__process_list_plugins(cmd);
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcasecmp(cmd->command_name, "listListeners")){
		rc = broker__process_list_listeners(cmd);

		/* Unknown */
	}ಇಲ್ಲದಿದ್ದರೆ{
		mosquitto_control_command_ಉತ್ತರ(cmd, "Unknown command");
		rc = MOSQ_ERR_INVAL;
	}
	ಹಿಂತಿರುಗಿ rc;
}
