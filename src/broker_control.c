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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব <cjson/cJSON.h>
#অন্তর্ভাব <errno.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <sys/stat.h>
#অন্তর্ভাব <utlist.h>

#অন্তর্ভাব "json_help.h"
#অন্তর্ভাব "mosquitto.h"
#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto/broker.h"
#অন্তর্ভাব "mosquitto/broker_control.h"
#অন্তর্ভাব "mosquitto/broker_plugin.h"
#অন্তর্ভাব "mosquitto/mqtt_protocol.h"

স্থির mosquitto_plugin_id_t plg_id;
স্থির পূর্ণাঙ্ক broker__handle_control(সংরচনা mosquitto_control_cmd *cmd, শূন্য *userdata);


স্থির পূর্ণাঙ্ক add_plugin_info(cJSON *j_plugins, mosquitto_plugin_id_t *pid)
{
	cJSON *j_plugin, *j_eps, *j_ep;
	সংরচনা control_endpoint *ep;

	যদি(pid->plugin_name == রিক্ত){
		প্রত্যাগম MOSQ_ERR_SUCCESS;
	}

	j_plugin = cJSON_CreateObject();
	যদি(j_plugin == রিক্ত){
		প্রত্যাগম MOSQ_ERR_NOMEM;
	}

	যদি(cJSON_AddStringToObject(j_plugin, "name", pid->plugin_name) == রিক্ত
			|| (pid->plugin_version && cJSON_AddStringToObject(j_plugin, "version", pid->plugin_version) == রিক্ত)
			|| (pid->listener && c_JSON_Add_সংখ্যা_To_Object(j_plugin, "port", pid->listener->port) == রিক্ত)
			|| (j_eps = cJSON_AddArrayToObject(j_plugin, "control-endpoints")) == রিক্ত
			){

		cJSON_Delete(j_plugin);
		প্রত্যাগম MOSQ_ERR_NOMEM;
	}

	DL_FOREACH(pid->control_endpoints, ep){
		j_ep = cJSON_CreateString(ep->topic);
		যদি(j_ep == রিক্ত){
			cJSON_Delete(j_plugin);
			প্রত্যাগম MOSQ_ERR_NOMEM;
		}
		cJSON_AddItemToArray(j_eps, j_ep);
	}

	cJSON_AddItemToArray(j_plugins, j_plugin);
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণাঙ্ক broker__process_list_plugins(সংরচনা mosquitto_control_cmd *cmd)
{
	cJSON *বৃক্ষ, *j_data, *j_plugins;
	ধ্রুব বর্ণ *admin_clientid, *admin_username;

	বৃক্ষ = cJSON_CreateObject();
	যদি(বৃক্ষ == রিক্ত){
		mosquitto_control_command_reply(cmd, "Internal error");
		প্রত্যাগম MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listPlugins",
			admin_clientid, admin_username);

	যদি(cJSON_AddStringToObject(বৃক্ষ, "command", "listPlugins") == রিক্ত
			|| ((j_data = cJSON_AddObjectToObject(বৃক্ষ, "data")) == রিক্ত)
			|| (cmd->correlation_data && cJSON_AddStringToObject(বৃক্ষ, "correlationData", cmd->correlation_data) == রিক্ত)
			){

		গচ্ছ internal_error;
	}

	j_plugins = cJSON_AddArrayToObject(j_data, "plugins");
	যদি(j_plugins == রিক্ত){
		গচ্ছ internal_error;
	}

	প্রতি(পূর্ণাঙ্ক i=0; i<db.plugin_গুনতি; i++){
		যদি(add_plugin_info(j_plugins, db.plugins[i])){
			গচ্ছ internal_error;
		}
	}

	cJSON_AddItemToArray(cmd->j_responses, বৃক্ষ);

	প্রত্যাগম MOSQ_ERR_SUCCESS;

internal_error:
	cJSON_Delete(বৃক্ষ);
	mosquitto_control_command_reply(cmd, "Internal error");
	প্রত্যাগম MOSQ_ERR_NOMEM;
}


স্থির পূর্ণাঙ্ক add_listener(cJSON *j_listeners, সংরচনা mosquitto__listener *listener)
{
	cJSON *j_listener;
	ধ্রুব বর্ণ *protocol = রিক্ত;

	j_listener = cJSON_CreateObject();
	যদি(j_listener == রিক্ত){
		প্রত্যাগম MOSQ_ERR_NOMEM;
	}
	cJSON_AddItemToArray(j_listeners, j_listener);

	যদি(listener->protocol == mp_mqtt){
		protocol = "mqtt";
	}অন্যথা যদি(listener->protocol == mp_websockets){
		protocol = "mqtt+websockets";
	}

	যদি(c_JSON_Add_সংখ্যা_To_Object(j_listener, "port", listener->port) == রিক্ত
			|| (protocol && cJSON_AddStringToObject(j_listener, "protocol", protocol) == রিক্ত)
			|| (listener->host && cJSON_AddStringToObject(j_listener, "bind-address", listener->host) == রিক্ত)
#ifdef WITH_UNIX_SOCKETS
			|| (listener->unix_socket_path && cJSON_AddStringToObject(j_listener, "socket-path", listener->unix_socket_path) == রিক্ত)
#endif
			){

		প্রত্যাগম MOSQ_ERR_NOMEM;
	}

	যদি(
#ifdef WITH_TLS
			cJSON_AddBoolToObject(j_listener, "tls", listener->ssl_ctx != রিক্ত) == রিক্ত
#অন্যথা
			cJSON_AddBoolToObject(j_listener, "tls", false) == রিক্ত
#endif
			){

		প্রত্যাগম MOSQ_ERR_NOMEM;
	}

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণাঙ্ক broker__process_list_listeners(সংরচনা mosquitto_control_cmd *cmd)
{
	cJSON *বৃক্ষ, *j_data, *j_listeners;
	ধ্রুব বর্ণ *admin_clientid, *admin_username;

	বৃক্ষ = cJSON_CreateObject();
	যদি(বৃক্ষ == রিক্ত){
		mosquitto_control_command_reply(cmd, "Internal error");
		প্রত্যাগম MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listListeners",
			admin_clientid, admin_username);

	যদি(cJSON_AddStringToObject(বৃক্ষ, "command", "listListeners") == রিক্ত
			|| ((j_data = cJSON_AddObjectToObject(বৃক্ষ, "data")) == রিক্ত)
			|| (cmd->correlation_data && cJSON_AddStringToObject(বৃক্ষ, "correlationData", cmd->correlation_data) == রিক্ত)
			){

		গচ্ছ internal_error;
	}

	j_listeners = cJSON_AddArrayToObject(j_data, "listeners");
	যদি(j_listeners == রিক্ত){
		গচ্ছ internal_error;
	}

	প্রতি(পূর্ণাঙ্ক i=0; i<db.config->listener_গুনতি; i++){
		যদি(add_listener(j_listeners, &db.config->listeners[i])){
			গচ্ছ internal_error;
		}
	}

	cJSON_AddItemToArray(cmd->j_responses, বৃক্ষ);

	প্রত্যাগম MOSQ_ERR_SUCCESS;

internal_error:
	cJSON_Delete(বৃক্ষ);
	mosquitto_control_command_reply(cmd, "Internal error");
	প্রত্যাগম MOSQ_ERR_NOMEM;
}


স্থির পূর্ণাঙ্ক broker_control_callback(পূর্ণাঙ্ক event, শূন্য *event_data, শূন্য *userdata)
{
	সংরচনা mosquitto_evt_control *ed = event_data;

	UNUSED(event);

	প্রত্যাগম mosquitto_control_generic_callback(ed, "$CONTROL/broker/v1/response", userdata, broker__handle_control);
}


শূন্য broker_control__init(শূন্য)
{
	memset(&plg_id, 0, পরিমাণ(plg_id));

	যদি(db.config->enable_control_api){
		mosquitto_callback_register(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1", রিক্ত);
	}
}


শূন্য broker_control__cleanup(শূন্য)
{
	mosquitto_callback_unregister(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1");
}


শূন্য broker_control__reload(শূন্য)
{
	broker_control__cleanup();
	broker_control__init();
}


/* ################################################################
 * #
 * # $CONTROL/broker/v1 handler
 * #
 * ################################################################ */


স্থির পূর্ণাঙ্ক broker__handle_control(সংরচনা mosquitto_control_cmd *cmd, শূন্য *userdata)
{
	পূর্ণাঙ্ক rc = MOSQ_ERR_SUCCESS;

	UNUSED(userdata);

	যদি(!strcasecmp(cmd->command_name, "listPlugins")){
		rc = broker__process_list_plugins(cmd);
	}অন্যথা যদি(!strcasecmp(cmd->command_name, "listListeners")){
		rc = broker__process_list_listeners(cmd);

		/* Unknown */
	}অন্যথা{
		mosquitto_control_command_reply(cmd, "Unknown command");
		rc = MOSQ_ERR_INVAL;
	}
	প্রত্যাগম rc;
}
