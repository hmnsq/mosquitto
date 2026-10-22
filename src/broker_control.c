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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <cjson/cJSON.h>
#ଅନ୍ତର୍ଭାଵ <errno.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>
#ଅନ୍ତର୍ଭାଵ <utlist.h>

#ଅନ୍ତର୍ଭାଵ "json_help.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/broker.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/broker_control.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/broker_plugin.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"

ସ୍ଥିର mosquitto_plugin_id_t plg_id;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା broker__handle_control(ସଂରଚନା mosquitto_control_cmd *cmd, ଶୂନ୍ୟ *userdata);


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା add_plugin_info(cJSON *j_plugins, mosquitto_plugin_id_t *pid)
{
	cJSON *j_plugin, *j_eps, *j_ep;
	ସଂରଚନା control_endpoint *ep;

	ଯଦି(pid->plugin_name == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

	j_plugin = cJSON_CreateObject();
	ଯଦି(j_plugin == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	ଯଦି(cJSON_AddStringToObject(j_plugin, "name", pid->plugin_name) == ଖାଲି
			|| (pid->plugin_version && cJSON_AddStringToObject(j_plugin, "version", pid->plugin_version) == ଖାଲି)
			|| (pid->listener && c_JSON_Add_ସଂଖ୍ୟା_To_Object(j_plugin, "port", pid->listener->port) == ଖାଲି)
			|| (j_eps = cJSON_AddArrayToObject(j_plugin, "control-endpoints")) == ଖାଲି
			){

		cJSON_Delete(j_plugin);
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	DL_FOREACH(pid->control_endpoints, ep){
		j_ep = cJSON_CreateString(ep->topic);
		ଯଦି(j_ep == ଖାଲି){
			cJSON_Delete(j_plugin);
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}
		cJSON_AddItemToArray(j_eps, j_ep);
	}

	cJSON_AddItemToArray(j_plugins, j_plugin);
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା broker__process_list_plugins(ସଂରଚନା mosquitto_control_cmd *cmd)
{
	cJSON *ବୃକ୍ଷ, *j_data, *j_plugins;
	ଅଚଳ ଅକ୍ଷର *admin_clientid, *admin_username;

	ବୃକ୍ଷ = cJSON_CreateObject();
	ଯଦି(ବୃକ୍ଷ == ଖାଲି){
		mosquitto_control_command_reply(cmd, "Internal error");
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listPlugins",
			admin_clientid, admin_username);

	ଯଦି(cJSON_AddStringToObject(ବୃକ୍ଷ, "command", "listPlugins") == ଖାଲି
			|| ((j_data = cJSON_AddObjectToObject(ବୃକ୍ଷ, "data")) == ଖାଲି)
			|| (cmd->correlation_data && cJSON_AddStringToObject(ବୃକ୍ଷ, "correlationData", cmd->correlation_data) == ଖାଲି)
			){

		ଯାଅ internal_error;
	}

	j_plugins = cJSON_AddArrayToObject(j_data, "plugins");
	ଯଦି(j_plugins == ଖାଲି){
		ଯାଅ internal_error;
	}

	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<db.plugin_ଗଣନା; i++){
		ଯଦି(add_plugin_info(j_plugins, db.plugins[i])){
			ଯାଅ internal_error;
		}
	}

	cJSON_AddItemToArray(cmd->j_responses, ବୃକ୍ଷ);

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;

internal_error:
	cJSON_Delete(ବୃକ୍ଷ);
	mosquitto_control_command_reply(cmd, "Internal error");
	ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା add_listener(cJSON *j_listeners, ସଂରଚନା mosquitto__listener *listener)
{
	cJSON *j_listener;
	ଅଚଳ ଅକ୍ଷର *protocol = ଖାଲି;

	j_listener = cJSON_CreateObject();
	ଯଦି(j_listener == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}
	cJSON_AddItemToArray(j_listeners, j_listener);

	ଯଦି(listener->protocol == mp_mqtt){
		protocol = "mqtt";
	}ଅନ୍ୟଥା ଯଦି(listener->protocol == mp_websockets){
		protocol = "mqtt+websockets";
	}

	ଯଦି(c_JSON_Add_ସଂଖ୍ୟା_To_Object(j_listener, "port", listener->port) == ଖାଲି
			|| (protocol && cJSON_AddStringToObject(j_listener, "protocol", protocol) == ଖାଲି)
			|| (listener->host && cJSON_AddStringToObject(j_listener, "bind-address", listener->host) == ଖାଲି)
#ifdef WITH_UNIX_SOCKETS
			|| (listener->unix_socket_path && cJSON_AddStringToObject(j_listener, "socket-path", listener->unix_socket_path) == ଖାଲି)
#endif
			){

		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	ଯଦି(
#ifdef WITH_TLS
			cJSON_AddBoolToObject(j_listener, "tls", listener->ssl_ctx != ଖାଲି) == ଖାଲି
#ଅନ୍ୟଥା
			cJSON_AddBoolToObject(j_listener, "tls", false) == ଖାଲି
#endif
			){

		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା broker__process_list_listeners(ସଂରଚନା mosquitto_control_cmd *cmd)
{
	cJSON *ବୃକ୍ଷ, *j_data, *j_listeners;
	ଅଚଳ ଅକ୍ଷର *admin_clientid, *admin_username;

	ବୃକ୍ଷ = cJSON_CreateObject();
	ଯଦି(ବୃକ୍ଷ == ଖାଲି){
		mosquitto_control_command_reply(cmd, "Internal error");
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listListeners",
			admin_clientid, admin_username);

	ଯଦି(cJSON_AddStringToObject(ବୃକ୍ଷ, "command", "listListeners") == ଖାଲି
			|| ((j_data = cJSON_AddObjectToObject(ବୃକ୍ଷ, "data")) == ଖାଲି)
			|| (cmd->correlation_data && cJSON_AddStringToObject(ବୃକ୍ଷ, "correlationData", cmd->correlation_data) == ଖାଲି)
			){

		ଯାଅ internal_error;
	}

	j_listeners = cJSON_AddArrayToObject(j_data, "listeners");
	ଯଦି(j_listeners == ଖାଲି){
		ଯାଅ internal_error;
	}

	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<db.config->listener_ଗଣନା; i++){
		ଯଦି(add_listener(j_listeners, &db.config->listeners[i])){
			ଯାଅ internal_error;
		}
	}

	cJSON_AddItemToArray(cmd->j_responses, ବୃକ୍ଷ);

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;

internal_error:
	cJSON_Delete(ବୃକ୍ଷ);
	mosquitto_control_command_reply(cmd, "Internal error");
	ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା broker_control_callback(ପୂର୍ଣ୍ଣସଂଖ୍ୟା event, ଶୂନ୍ୟ *event_data, ଶୂନ୍ୟ *userdata)
{
	ସଂରଚନା mosquitto_evt_control *ed = event_data;

	UNUSED(event);

	ଫେରନ୍ତୁ mosquitto_control_generic_callback(ed, "$CONTROL/broker/v1/response", userdata, broker__handle_control);
}


ଶୂନ୍ୟ broker_control__init(ଶୂନ୍ୟ)
{
	memset(&plg_id, 0, ଆକାର(plg_id));

	ଯଦି(db.config->enable_control_api){
		mosquitto_callback_register(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1", ଖାଲି);
	}
}


ଶୂନ୍ୟ broker_control__cleanup(ଶୂନ୍ୟ)
{
	mosquitto_callback_unregister(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1");
}


ଶୂନ୍ୟ broker_control__reload(ଶୂନ୍ୟ)
{
	broker_control__cleanup();
	broker_control__init();
}


/* ################################################################
 * #
 * # $CONTROL/broker/v1 handler
 * #
 * ################################################################ */


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା broker__handle_control(ସଂରଚନା mosquitto_control_cmd *cmd, ଶୂନ୍ୟ *userdata)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = MOSQ_ERR_SUCCESS;

	UNUSED(userdata);

	ଯଦି(!strcasecmp(cmd->command_name, "listPlugins")){
		rc = broker__process_list_plugins(cmd);
	}ଅନ୍ୟଥା ଯଦି(!strcasecmp(cmd->command_name, "listListeners")){
		rc = broker__process_list_listeners(cmd);

		/* Unknown */
	}ଅନ୍ୟଥା{
		mosquitto_control_command_reply(cmd, "Unknown command");
		rc = MOSQ_ERR_INVAL;
	}
	ଫେରନ୍ତୁ rc;
}
