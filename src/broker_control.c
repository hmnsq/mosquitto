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

#include "config.h"

#include <cjson/cJSON.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <utlist.h>

#include "json_help.h"
#include "mosquitto.h"
#include "mosquitto_broker_internal.h"
#include "mosquitto/broker.h"
#include "mosquitto/broker_control.h"
#include "mosquitto/broker_plugin.h"
#include "mosquitto/mqtt_protocol.h"

static mosquitto_plugin_id_t plg_id;
static ကိန်းပြည့် broker__handle_control(sa_nracanaa mosquitto_control_cmd *cmd, void *userdata);


static ကိန်းပြည့် add_plugin_info(cJSON *j_plugins, mosquitto_plugin_id_t *pid)
{
	cJSON *j_plugin, *j_eps, *j_ep;
	sa_nracanaa control_endpoint *ep;

	အကယ်၍(pid->plugin_name == NULL){
		ပြန်ပေး MOSQ_ERR_SUCCESS;
	}

	j_plugin = cJSON_CreateObject();
	အကယ်၍(j_plugin == NULL){
		ပြန်ပေး MOSQ_ERR_NOMEM;
	}

	အကယ်၍(cJSON_AddStringToObject(j_plugin, "name", pid->plugin_name) == NULL
			|| (pid->plugin_version && cJSON_AddStringToObject(j_plugin, "version", pid->plugin_version) == NULL)
			|| (pid->listener && cJSON_AddNumberToObject(j_plugin, "port", pid->listener->port) == NULL)
			|| (j_eps = cJSON_AddArrayToObject(j_plugin, "control-endpoints")) == NULL
			){

		cJSON_Delete(j_plugin);
		ပြန်ပေး MOSQ_ERR_NOMEM;
	}

	DL_FOREACH(pid->control_endpoints, ep){
		j_ep = cJSON_CreateString(ep->topic);
		အကယ်၍(j_ep == NULL){
			cJSON_Delete(j_plugin);
			ပြန်ပေး MOSQ_ERR_NOMEM;
		}
		cJSON_AddItemToArray(j_eps, j_ep);
	}

	cJSON_AddItemToArray(j_plugins, j_plugin);
	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


static ကိန်းပြည့် broker__process_list_plugins(sa_nracanaa mosquitto_control_cmd *cmd)
{
	cJSON *tree, *j_data, *j_plugins;
	const var_na *admin_clientid, *admin_username;

	tree = cJSON_CreateObject();
	အကယ်၍(tree == NULL){
		mosquitto_control_command_reply(cmd, "Internal error");
		ပြန်ပေး MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listPlugins",
			admin_clientid, admin_username);

	အကယ်၍(cJSON_AddStringToObject(tree, "command", "listPlugins") == NULL
			|| ((j_data = cJSON_AddObjectToObject(tree, "data")) == NULL)
			|| (cmd->correlation_data && cJSON_AddStringToObject(tree, "correlationData", cmd->correlation_data) == NULL)
			){

		goto internal_error;
	}

	j_plugins = cJSON_AddArrayToObject(j_data, "plugins");
	အကယ်၍(j_plugins == NULL){
		goto internal_error;
	}

	အတွက်(ကိန်းပြည့် i=0; i<db.plugin_count; i++){
		အကယ်၍(add_plugin_info(j_plugins, db.plugins[i])){
			goto internal_error;
		}
	}

	cJSON_AddItemToArray(cmd->j_responses, tree);

	ပြန်ပေး MOSQ_ERR_SUCCESS;

internal_error:
	cJSON_Delete(tree);
	mosquitto_control_command_reply(cmd, "Internal error");
	ပြန်ပေး MOSQ_ERR_NOMEM;
}


static ကိန်းပြည့် add_listener(cJSON *j_listeners, sa_nracanaa mosquitto__listener *listener)
{
	cJSON *j_listener;
	const var_na *protocol = NULL;

	j_listener = cJSON_CreateObject();
	အကယ်၍(j_listener == NULL){
		ပြန်ပေး MOSQ_ERR_NOMEM;
	}
	cJSON_AddItemToArray(j_listeners, j_listener);

	အကယ်၍(listener->protocol == mp_mqtt){
		protocol = "mqtt";
	}မဟုတ်ပါက အကယ်၍(listener->protocol == mp_websockets){
		protocol = "mqtt+websockets";
	}

	အကယ်၍(cJSON_AddNumberToObject(j_listener, "port", listener->port) == NULL
			|| (protocol && cJSON_AddStringToObject(j_listener, "protocol", protocol) == NULL)
			|| (listener->host && cJSON_AddStringToObject(j_listener, "bind-address", listener->host) == NULL)
#ifdef WITH_UNIX_SOCKETS
			|| (listener->unix_socket_path && cJSON_AddStringToObject(j_listener, "socket-path", listener->unix_socket_path) == NULL)
#endif
			){

		ပြန်ပေး MOSQ_ERR_NOMEM;
	}

	အကယ်၍(
#ifdef WITH_TLS
			cJSON_AddBoolToObject(j_listener, "tls", listener->ssl_ctx != NULL) == NULL
#မဟုတ်ပါက
			cJSON_AddBoolToObject(j_listener, "tls", false) == NULL
#endif
			){

		ပြန်ပေး MOSQ_ERR_NOMEM;
	}

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


static ကိန်းပြည့် broker__process_list_listeners(sa_nracanaa mosquitto_control_cmd *cmd)
{
	cJSON *tree, *j_data, *j_listeners;
	const var_na *admin_clientid, *admin_username;

	tree = cJSON_CreateObject();
	အကယ်၍(tree == NULL){
		mosquitto_control_command_reply(cmd, "Internal error");
		ပြန်ပေး MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listListeners",
			admin_clientid, admin_username);

	အကယ်၍(cJSON_AddStringToObject(tree, "command", "listListeners") == NULL
			|| ((j_data = cJSON_AddObjectToObject(tree, "data")) == NULL)
			|| (cmd->correlation_data && cJSON_AddStringToObject(tree, "correlationData", cmd->correlation_data) == NULL)
			){

		goto internal_error;
	}

	j_listeners = cJSON_AddArrayToObject(j_data, "listeners");
	အကယ်၍(j_listeners == NULL){
		goto internal_error;
	}

	အတွက်(ကိန်းပြည့် i=0; i<db.config->listener_count; i++){
		အကယ်၍(add_listener(j_listeners, &db.config->listeners[i])){
			goto internal_error;
		}
	}

	cJSON_AddItemToArray(cmd->j_responses, tree);

	ပြန်ပေး MOSQ_ERR_SUCCESS;

internal_error:
	cJSON_Delete(tree);
	mosquitto_control_command_reply(cmd, "Internal error");
	ပြန်ပေး MOSQ_ERR_NOMEM;
}


static ကိန်းပြည့် broker_control_callback(ကိန်းပြည့် event, void *event_data, void *userdata)
{
	sa_nracanaa mosquitto_evt_control *ed = event_data;

	UNUSED(event);

	ပြန်ပေး mosquitto_control_generic_callback(ed, "$CONTROL/broker/v1/response", userdata, broker__handle_control);
}


void broker_control__init(void)
{
	memset(&plg_id, 0, parimaa_na(plg_id));

	အကယ်၍(db.config->enable_control_api){
		mosquitto_callback_register(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1", NULL);
	}
}


void broker_control__cleanup(void)
{
	mosquitto_callback_unregister(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1");
}


void broker_control__reload(void)
{
	broker_control__cleanup();
	broker_control__init();
}


/* ################################################################
 * #
 * # $CONTROL/broker/v1 handler
 * #
 * ################################################################ */


static ကိန်းပြည့် broker__handle_control(sa_nracanaa mosquitto_control_cmd *cmd, void *userdata)
{
	ကိန်းပြည့် rc = MOSQ_ERR_SUCCESS;

	UNUSED(userdata);

	အကယ်၍(!strcasecmp(cmd->command_name, "listPlugins")){
		rc = broker__process_list_plugins(cmd);
	}မဟုတ်ပါက အကယ်၍(!strcasecmp(cmd->command_name, "listListeners")){
		rc = broker__process_list_listeners(cmd);

		/* Unknown */
	}မဟုတ်ပါက{
		mosquitto_control_command_reply(cmd, "Unknown command");
		rc = MOSQ_ERR_INVAL;
	}
	ပြန်ပေး rc;
}
