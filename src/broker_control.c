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

#समावेश "config.h"

#समावेश <cjson/cJSON.h>
#समावेश <errno.h>
#समावेश <stdio.h>
#समावेश <stdlib.h>
#समावेश <string.h>
#समावेश <sys/stat.h>
#समावेश <utlist.h>

#समावेश "json_help.h"
#समावेश "mosquitto.h"
#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/broker.h"
#समावेश "mosquitto/broker_control.h"
#समावेश "mosquitto/broker_plugin.h"
#समावेश "mosquitto/mqtt_protocol.h"

अटल mosquitto_plugin_id_t plg_id;
अटल पूर्णांक broker__handle_control(काष्ठा mosquitto_control_cmd *cmd, व्योम *userdata);


अटल पूर्णांक add_plugin_info(cJSON *j_plugins, mosquitto_plugin_id_t *pid)
{
	cJSON *j_plugin, *j_eps, *j_ep;
	काष्ठा control_endpoint *ep;

	अगर(pid->plugin_name == शून्य){
		वापस MOSQ_ERR_SUCCESS;
	}

	j_plugin = cJSON_CreateObject();
	अगर(j_plugin == शून्य){
		वापस MOSQ_ERR_NOMEM;
	}

	अगर(cJSON_AddStringToObject(j_plugin, "name", pid->plugin_name) == शून्य
			|| (pid->plugin_version && cJSON_AddStringToObject(j_plugin, "version", pid->plugin_version) == शून्य)
			|| (pid->listener && c_JSON_Add_संख्या_To_Object(j_plugin, "port", pid->listener->port) == शून्य)
			|| (j_eps = cJSON_AddArrayToObject(j_plugin, "control-endpoints")) == शून्य
			){

		cJSON_Delete(j_plugin);
		वापस MOSQ_ERR_NOMEM;
	}

	DL_FOREACH(pid->control_endpoints, ep){
		j_ep = cJSON_CreateString(ep->topic);
		अगर(j_ep == शून्य){
			cJSON_Delete(j_plugin);
			वापस MOSQ_ERR_NOMEM;
		}
		cJSON_AddItemToArray(j_eps, j_ep);
	}

	cJSON_AddItemToArray(j_plugins, j_plugin);
	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक broker__process_list_plugins(काष्ठा mosquitto_control_cmd *cmd)
{
	cJSON *वृक्ष, *j_तथ्य, *j_plugins;
	स्थिर अक्षर *admin_clientid, *admin_username;

	वृक्ष = cJSON_CreateObject();
	अगर(वृक्ष == शून्य){
		mosquitto_control_command_reply(cmd, "Internal error");
		वापस MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listPlugins",
			admin_clientid, admin_username);

	अगर(cJSON_AddStringToObject(वृक्ष, "command", "listPlugins") == शून्य
			|| ((j_तथ्य = cJSON_AddObjectToObject(वृक्ष, "data")) == शून्य)
			|| (cmd->correlation_तथ्य && cJSON_AddStringToObject(वृक्ष, "correlationData", cmd->correlation_तथ्य) == शून्य)
			){

		जाओ internal_error;
	}

	j_plugins = cJSON_AddArrayToObject(j_तथ्य, "plugins");
	अगर(j_plugins == शून्य){
		जाओ internal_error;
	}

	क्रम(पूर्णांक i=0; i<db.plugin_गिनती; i++){
		अगर(add_plugin_info(j_plugins, db.plugins[i])){
			जाओ internal_error;
		}
	}

	cJSON_AddItemToArray(cmd->j_responses, वृक्ष);

	वापस MOSQ_ERR_SUCCESS;

internal_error:
	cJSON_Delete(वृक्ष);
	mosquitto_control_command_reply(cmd, "Internal error");
	वापस MOSQ_ERR_NOMEM;
}


अटल पूर्णांक add_listener(cJSON *j_listeners, काष्ठा mosquitto__listener *listener)
{
	cJSON *j_listener;
	स्थिर अक्षर *protocol = शून्य;

	j_listener = cJSON_CreateObject();
	अगर(j_listener == शून्य){
		वापस MOSQ_ERR_NOMEM;
	}
	cJSON_AddItemToArray(j_listeners, j_listener);

	अगर(listener->protocol == mp_mqtt){
		protocol = "mqtt";
	}अन्यथा अगर(listener->protocol == mp_websockets){
		protocol = "mqtt+websockets";
	}

	अगर(c_JSON_Add_संख्या_To_Object(j_listener, "port", listener->port) == शून्य
			|| (protocol && cJSON_AddStringToObject(j_listener, "protocol", protocol) == शून्य)
			|| (listener->host && cJSON_AddStringToObject(j_listener, "bind-address", listener->host) == शून्य)
#अगर_घोषित WITH_UNIX_SOCKETS
			|| (listener->unix_socket_path && cJSON_AddStringToObject(j_listener, "socket-path", listener->unix_socket_path) == शून्य)
#पूर्ण_अगर
			){

		वापस MOSQ_ERR_NOMEM;
	}

	अगर(
#अगर_घोषित WITH_TLS
			cJSON_AddBoolToObject(j_listener, "tls", listener->ssl_ctx != शून्य) == शून्य
#अन्यथा
			cJSON_AddBoolToObject(j_listener, "tls", false) == शून्य
#पूर्ण_अगर
			){

		वापस MOSQ_ERR_NOMEM;
	}

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक broker__process_list_listeners(काष्ठा mosquitto_control_cmd *cmd)
{
	cJSON *वृक्ष, *j_तथ्य, *j_listeners;
	स्थिर अक्षर *admin_clientid, *admin_username;

	वृक्ष = cJSON_CreateObject();
	अगर(वृक्ष == शून्य){
		mosquitto_control_command_reply(cmd, "Internal error");
		वापस MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listListeners",
			admin_clientid, admin_username);

	अगर(cJSON_AddStringToObject(वृक्ष, "command", "listListeners") == शून्य
			|| ((j_तथ्य = cJSON_AddObjectToObject(वृक्ष, "data")) == शून्य)
			|| (cmd->correlation_तथ्य && cJSON_AddStringToObject(वृक्ष, "correlationData", cmd->correlation_तथ्य) == शून्य)
			){

		जाओ internal_error;
	}

	j_listeners = cJSON_AddArrayToObject(j_तथ्य, "listeners");
	अगर(j_listeners == शून्य){
		जाओ internal_error;
	}

	क्रम(पूर्णांक i=0; i<db.config->listener_गिनती; i++){
		अगर(add_listener(j_listeners, &db.config->listeners[i])){
			जाओ internal_error;
		}
	}

	cJSON_AddItemToArray(cmd->j_responses, वृक्ष);

	वापस MOSQ_ERR_SUCCESS;

internal_error:
	cJSON_Delete(वृक्ष);
	mosquitto_control_command_reply(cmd, "Internal error");
	वापस MOSQ_ERR_NOMEM;
}


अटल पूर्णांक broker_control_callback(पूर्णांक event, व्योम *event_तथ्य, व्योम *userdata)
{
	काष्ठा mosquitto_evt_control *ed = event_तथ्य;

	UNUSED(event);

	वापस mosquitto_control_generic_callback(ed, "$CONTROL/broker/v1/response", userdata, broker__handle_control);
}


व्योम broker_control__init(व्योम)
{
	स_रखो(&plg_id, 0, माप(plg_id));

	अगर(db.config->enable_control_api){
		mosquitto_callback_register(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1", शून्य);
	}
}


व्योम broker_control__cleanup(व्योम)
{
	mosquitto_callback_unregister(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1");
}


व्योम broker_control__reload(व्योम)
{
	broker_control__cleanup();
	broker_control__init();
}


/* ################################################################
 * #
 * # $CONTROL/broker/v1 handler
 * #
 * ################################################################ */


अटल पूर्णांक broker__handle_control(काष्ठा mosquitto_control_cmd *cmd, व्योम *userdata)
{
	पूर्णांक rc = MOSQ_ERR_SUCCESS;

	UNUSED(userdata);

	अगर(!strcasecmp(cmd->command_name, "listPlugins")){
		rc = broker__process_list_plugins(cmd);
	}अन्यथा अगर(!strcasecmp(cmd->command_name, "listListeners")){
		rc = broker__process_list_listeners(cmd);

		/* Unknown */
	}अन्यथा{
		mosquitto_control_command_reply(cmd, "Unknown command");
		rc = MOSQ_ERR_INVAL;
	}
	वापस rc;
}
