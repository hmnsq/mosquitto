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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ <cjson/cJSON.h>
#અન્તર્ભાવ <errno.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <sys/stat.h>
#અન્તર્ભાવ <utlist.h>

#અન્તર્ભાવ "json_help.h"
#અન્તર્ભાવ "mosquitto.h"
#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/broker.h"
#અન્તર્ભાવ "mosquitto/broker_control.h"
#અન્તર્ભાવ "mosquitto/broker_plugin.h"
#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"

સ્થિર mosquitto_plugin_id_t plg_id;
સ્થિર પૂર્ણાંક broker__handle_control(સંરચના mosquitto_control_cmd *cmd, શૂન્ય *userdata);


સ્થિર પૂર્ણાંક add_plugin_info(cJSON *j_plugins, mosquitto_plugin_id_t *pid)
{
	cJSON *j_plugin, *j_eps, *j_ep;
	સંરચના control_endpoint *ep;

	જો(pid->plugin_name == ખાલી){
		પાછા MOSQ_ERR_SUCCESS;
	}

	j_plugin = cJSON_CreateObject();
	જો(j_plugin == ખાલી){
		પાછા MOSQ_ERR_NOMEM;
	}

	જો(cJSON_AddStringToObject(j_plugin, "name", pid->plugin_name) == ખાલી
			|| (pid->plugin_version && cJSON_AddStringToObject(j_plugin, "version", pid->plugin_version) == ખાલી)
			|| (pid->listener && c_JSON_Add_સંખ્યા_To_Object(j_plugin, "port", pid->listener->port) == ખાલી)
			|| (j_eps = cJSON_AddArrayToObject(j_plugin, "control-endpoints")) == ખાલી
			){

		cJSON_Delete(j_plugin);
		પાછા MOSQ_ERR_NOMEM;
	}

	DL_FOREACH(pid->control_endpoints, ep){
		j_ep = cJSON_CreateString(ep->topic);
		જો(j_ep == ખાલી){
			cJSON_Delete(j_plugin);
			પાછા MOSQ_ERR_NOMEM;
		}
		cJSON_AddItemToArray(j_eps, j_ep);
	}

	cJSON_AddItemToArray(j_plugins, j_plugin);
	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક broker__process_list_plugins(સંરચના mosquitto_control_cmd *cmd)
{
	cJSON *વૃક્ષ, *j_data, *j_plugins;
	અચલ અક્ષર *admin_clientid, *admin_username;

	વૃક્ષ = cJSON_CreateObject();
	જો(વૃક્ષ == ખાલી){
		mosquitto_control_command_reply(cmd, "Internal error");
		પાછા MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listPlugins",
			admin_clientid, admin_username);

	જો(cJSON_AddStringToObject(વૃક્ષ, "command", "listPlugins") == ખાલી
			|| ((j_data = cJSON_AddObjectToObject(વૃક્ષ, "data")) == ખાલી)
			|| (cmd->correlation_data && cJSON_AddStringToObject(વૃક્ષ, "correlationData", cmd->correlation_data) == ખાલી)
			){

		જાઓ internal_error;
	}

	j_plugins = cJSON_AddArrayToObject(j_data, "plugins");
	જો(j_plugins == ખાલી){
		જાઓ internal_error;
	}

	માટે(પૂર્ણાંક i=0; i<db.plugin_ગણતરી; i++){
		જો(add_plugin_info(j_plugins, db.plugins[i])){
			જાઓ internal_error;
		}
	}

	cJSON_AddItemToArray(cmd->j_responses, વૃક્ષ);

	પાછા MOSQ_ERR_SUCCESS;

internal_error:
	cJSON_Delete(વૃક્ષ);
	mosquitto_control_command_reply(cmd, "Internal error");
	પાછા MOSQ_ERR_NOMEM;
}


સ્થિર પૂર્ણાંક add_listener(cJSON *j_listeners, સંરચના mosquitto__listener *listener)
{
	cJSON *j_listener;
	અચલ અક્ષર *protocol = ખાલી;

	j_listener = cJSON_CreateObject();
	જો(j_listener == ખાલી){
		પાછા MOSQ_ERR_NOMEM;
	}
	cJSON_AddItemToArray(j_listeners, j_listener);

	જો(listener->protocol == mp_mqtt){
		protocol = "mqtt";
	}નહીંતર જો(listener->protocol == mp_websockets){
		protocol = "mqtt+websockets";
	}

	જો(c_JSON_Add_સંખ્યા_To_Object(j_listener, "port", listener->port) == ખાલી
			|| (protocol && cJSON_AddStringToObject(j_listener, "protocol", protocol) == ખાલી)
			|| (listener->host && cJSON_AddStringToObject(j_listener, "bind-address", listener->host) == ખાલી)
#ifdef WITH_UNIX_SOCKETS
			|| (listener->unix_socket_path && cJSON_AddStringToObject(j_listener, "socket-path", listener->unix_socket_path) == ખાલી)
#endif
			){

		પાછા MOSQ_ERR_NOMEM;
	}

	જો(
#ifdef WITH_TLS
			cJSON_AddBoolToObject(j_listener, "tls", listener->ssl_ctx != ખાલી) == ખાલી
#નહીંતર
			cJSON_AddBoolToObject(j_listener, "tls", false) == ખાલી
#endif
			){

		પાછા MOSQ_ERR_NOMEM;
	}

	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક broker__process_list_listeners(સંરચના mosquitto_control_cmd *cmd)
{
	cJSON *વૃક્ષ, *j_data, *j_listeners;
	અચલ અક્ષર *admin_clientid, *admin_username;

	વૃક્ષ = cJSON_CreateObject();
	જો(વૃક્ષ == ખાલી){
		mosquitto_control_command_reply(cmd, "Internal error");
		પાછા MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listListeners",
			admin_clientid, admin_username);

	જો(cJSON_AddStringToObject(વૃક્ષ, "command", "listListeners") == ખાલી
			|| ((j_data = cJSON_AddObjectToObject(વૃક્ષ, "data")) == ખાલી)
			|| (cmd->correlation_data && cJSON_AddStringToObject(વૃક્ષ, "correlationData", cmd->correlation_data) == ખાલી)
			){

		જાઓ internal_error;
	}

	j_listeners = cJSON_AddArrayToObject(j_data, "listeners");
	જો(j_listeners == ખાલી){
		જાઓ internal_error;
	}

	માટે(પૂર્ણાંક i=0; i<db.config->listener_ગણતરી; i++){
		જો(add_listener(j_listeners, &db.config->listeners[i])){
			જાઓ internal_error;
		}
	}

	cJSON_AddItemToArray(cmd->j_responses, વૃક્ષ);

	પાછા MOSQ_ERR_SUCCESS;

internal_error:
	cJSON_Delete(વૃક્ષ);
	mosquitto_control_command_reply(cmd, "Internal error");
	પાછા MOSQ_ERR_NOMEM;
}


સ્થિર પૂર્ણાંક broker_control_callback(પૂર્ણાંક event, શૂન્ય *event_data, શૂન્ય *userdata)
{
	સંરચના mosquitto_evt_control *ed = event_data;

	UNUSED(event);

	પાછા mosquitto_control_generic_callback(ed, "$CONTROL/broker/v1/response", userdata, broker__handle_control);
}


શૂન્ય broker_control__init(શૂન્ય)
{
	memset(&plg_id, 0, કદ(plg_id));

	જો(db.config->enable_control_api){
		mosquitto_callback_register(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1", ખાલી);
	}
}


શૂન્ય broker_control__cleanup(શૂન્ય)
{
	mosquitto_callback_unregister(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1");
}


શૂન્ય broker_control__reload(શૂન્ય)
{
	broker_control__cleanup();
	broker_control__init();
}


/* ################################################################
 * #
 * # $CONTROL/broker/v1 handler
 * #
 * ################################################################ */


સ્થિર પૂર્ણાંક broker__handle_control(સંરચના mosquitto_control_cmd *cmd, શૂન્ય *userdata)
{
	પૂર્ણાંક rc = MOSQ_ERR_SUCCESS;

	UNUSED(userdata);

	જો(!strcasecmp(cmd->command_name, "listPlugins")){
		rc = broker__process_list_plugins(cmd);
	}નહીંતર જો(!strcasecmp(cmd->command_name, "listListeners")){
		rc = broker__process_list_listeners(cmd);

		/* Unknown */
	}નહીંતર{
		mosquitto_control_command_reply(cmd, "Unknown command");
		rc = MOSQ_ERR_INVAL;
	}
	પાછા rc;
}
