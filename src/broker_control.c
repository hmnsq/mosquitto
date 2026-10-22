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

#अन्तर्भाव "config.h"

#अन्तर्भाव <cjson/cJSON.h>
#अन्तर्भाव <errno.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <sys/stat.h>
#अन्तर्भाव <utlist.h>

#अन्तर्भाव "json_help.h"
#अन्तर्भाव "mosquitto.h"
#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/broker.h"
#अन्तर्भाव "mosquitto/broker_control.h"
#अन्तर्भाव "mosquitto/broker_plugin.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"

स्थिर mosquitto_plugin_id_t plg_id;
स्थिर पूर्णाङ्क broker__handle_control(संरचना mosquitto_control_cmd *cmd, शून्य *userdata);


स्थिर पूर्णाङ्क add_plugin_info(cJSON *j_plugins, mosquitto_plugin_id_t *pid)
{
	cJSON *j_plugin, *j_eps, *j_ep;
	संरचना control_endpoint *ep;

	जइ(pid->plugin_name == रिक्त){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	j_plugin = cJSON_CreateObject();
	जइ(j_plugin == रिक्त){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	जइ(cJSON_AddStringToObject(j_plugin, "name", pid->plugin_name) == रिक्त
			|| (pid->plugin_version && cJSON_AddStringToObject(j_plugin, "version", pid->plugin_version) == रिक्त)
			|| (pid->listener && c_JSON_Add_सङ्ख्या_To_Object(j_plugin, "port", pid->listener->port) == रिक्त)
			|| (j_eps = cJSON_AddArrayToObject(j_plugin, "control-endpoints")) == रिक्त
			){

		cJSON_Delete(j_plugin);
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	DL_FOREACH(pid->control_endpoints, ep){
		j_ep = cJSON_CreateString(ep->topic);
		जइ(j_ep == रिक्त){
			cJSON_Delete(j_plugin);
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
		cJSON_AddItemToArray(j_eps, j_ep);
	}

	cJSON_AddItemToArray(j_plugins, j_plugin);
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क broker__process_list_plugins(संरचना mosquitto_control_cmd *cmd)
{
	cJSON *वृक्षः, *j_data, *j_plugins;
	ध्रुव वर्ण *admin_clientid, *admin_username;

	वृक्षः = cJSON_CreateObject();
	जइ(वृक्षः == रिक्त){
		mosquitto_control_command_reply(cmd, "Internal error");
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listPlugins",
			admin_clientid, admin_username);

	जइ(cJSON_AddStringToObject(वृक्षः, "command", "listPlugins") == रिक्त
			|| ((j_data = cJSON_AddObjectToObject(वृक्षः, "data")) == रिक्त)
			|| (cmd->correlation_data && cJSON_AddStringToObject(वृक्षः, "correlationData", cmd->correlation_data) == रिक्त)
			){

		गच्छ internal_error;
	}

	j_plugins = cJSON_AddArrayToObject(j_data, "plugins");
	जइ(j_plugins == रिक्त){
		गच्छ internal_error;
	}

	प्रति(पूर्णाङ्क i=0; i<db.plugin_गणनम्; i++){
		जइ(add_plugin_info(j_plugins, db.plugins[i])){
			गच्छ internal_error;
		}
	}

	cJSON_AddItemToArray(cmd->j_responses, वृक्षः);

	प्रत्यागम MOSQ_ERR_SUCCESS;

internal_error:
	cJSON_Delete(वृक्षः);
	mosquitto_control_command_reply(cmd, "Internal error");
	प्रत्यागम MOSQ_ERR_NOMEM;
}


स्थिर पूर्णाङ्क add_listener(cJSON *j_listeners, संरचना mosquitto__listener *listener)
{
	cJSON *j_listener;
	ध्रुव वर्ण *protocol = रिक्त;

	j_listener = cJSON_CreateObject();
	जइ(j_listener == रिक्त){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	cJSON_AddItemToArray(j_listeners, j_listener);

	जइ(listener->protocol == mp_mqtt){
		protocol = "mqtt";
	}अण्णहा जइ(listener->protocol == mp_websockets){
		protocol = "mqtt+websockets";
	}

	जइ(c_JSON_Add_सङ्ख्या_To_Object(j_listener, "port", listener->port) == रिक्त
			|| (protocol && cJSON_AddStringToObject(j_listener, "protocol", protocol) == रिक्त)
			|| (listener->host && cJSON_AddStringToObject(j_listener, "bind-address", listener->host) == रिक्त)
#ifdef WITH_UNIX_SOCKETS
			|| (listener->unix_socket_path && cJSON_AddStringToObject(j_listener, "socket-path", listener->unix_socket_path) == रिक्त)
#endif
			){

		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	जइ(
#ifdef WITH_TLS
			cJSON_AddBoolToObject(j_listener, "tls", listener->ssl_ctx != रिक्त) == रिक्त
#अण्णहा
			cJSON_AddBoolToObject(j_listener, "tls", false) == रिक्त
#endif
			){

		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क broker__process_list_listeners(संरचना mosquitto_control_cmd *cmd)
{
	cJSON *वृक्षः, *j_data, *j_listeners;
	ध्रुव वर्ण *admin_clientid, *admin_username;

	वृक्षः = cJSON_CreateObject();
	जइ(वृक्षः == रिक्त){
		mosquitto_control_command_reply(cmd, "Internal error");
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listListeners",
			admin_clientid, admin_username);

	जइ(cJSON_AddStringToObject(वृक्षः, "command", "listListeners") == रिक्त
			|| ((j_data = cJSON_AddObjectToObject(वृक्षः, "data")) == रिक्त)
			|| (cmd->correlation_data && cJSON_AddStringToObject(वृक्षः, "correlationData", cmd->correlation_data) == रिक्त)
			){

		गच्छ internal_error;
	}

	j_listeners = cJSON_AddArrayToObject(j_data, "listeners");
	जइ(j_listeners == रिक्त){
		गच्छ internal_error;
	}

	प्रति(पूर्णाङ्क i=0; i<db.config->listener_गणनम्; i++){
		जइ(add_listener(j_listeners, &db.config->listeners[i])){
			गच्छ internal_error;
		}
	}

	cJSON_AddItemToArray(cmd->j_responses, वृक्षः);

	प्रत्यागम MOSQ_ERR_SUCCESS;

internal_error:
	cJSON_Delete(वृक्षः);
	mosquitto_control_command_reply(cmd, "Internal error");
	प्रत्यागम MOSQ_ERR_NOMEM;
}


स्थिर पूर्णाङ्क broker_control_callback(पूर्णाङ्क event, शून्य *event_data, शून्य *userdata)
{
	संरचना mosquitto_evt_control *ed = event_data;

	UNUSED(event);

	प्रत्यागम mosquitto_control_generic_callback(ed, "$CONTROL/broker/v1/response", userdata, broker__handle_control);
}


शून्य broker_control__init(शून्य)
{
	memset(&plg_id, 0, परिमाण(plg_id));

	जइ(db.config->enable_control_api){
		mosquitto_callback_register(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1", रिक्त);
	}
}


शून्य broker_control__cleanup(शून्य)
{
	mosquitto_callback_unregister(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1");
}


शून्य broker_control__reload(शून्य)
{
	broker_control__cleanup();
	broker_control__init();
}


/* ################################################################
 * #
 * # $CONTROL/broker/v1 handler
 * #
 * ################################################################ */


स्थिर पूर्णाङ्क broker__handle_control(संरचना mosquitto_control_cmd *cmd, शून्य *userdata)
{
	पूर्णाङ्क rc = MOSQ_ERR_SUCCESS;

	UNUSED(userdata);

	जइ(!strcasecmp(cmd->command_name, "listPlugins")){
		rc = broker__process_list_plugins(cmd);
	}अण्णहा जइ(!strcasecmp(cmd->command_name, "listListeners")){
		rc = broker__process_list_listeners(cmd);

		/* Unknown */
	}अण्णहा{
		mosquitto_control_command_reply(cmd, "Unknown command");
		rc = MOSQ_ERR_INVAL;
	}
	प्रत्यागम rc;
}
