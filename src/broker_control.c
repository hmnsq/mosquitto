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
static ཧྲིལ broker__handle_control(sa_nracanaa mosquitto_control_cmd *cmd, void *userdata);


static ཧྲིལ add_plugin_info(cJSON *j_plugins, mosquitto_plugin_id_t *pid)
{
	cJSON *j_plugin, *j_eps, *j_ep;
	sa_nracanaa control_endpoint *ep;

	གལ་ཏེ(pid->plugin_name == NULL){
		ཕྱིར MOSQ_ERR_SUCCESS;
	}

	j_plugin = cJSON_CreateObject();
	གལ་ཏེ(j_plugin == NULL){
		ཕྱིར MOSQ_ERR_NOMEM;
	}

	གལ་ཏེ(cJSON_AddStringToObject(j_plugin, "name", pid->plugin_name) == NULL
			|| (pid->plugin_version && cJSON_AddStringToObject(j_plugin, "version", pid->plugin_version) == NULL)
			|| (pid->listener && cJSON_AddNumberToObject(j_plugin, "port", pid->listener->port) == NULL)
			|| (j_eps = cJSON_AddArrayToObject(j_plugin, "control-endpoints")) == NULL
			){

		cJSON_Delete(j_plugin);
		ཕྱིར MOSQ_ERR_NOMEM;
	}

	DL_FOREACH(pid->control_endpoints, ep){
		j_ep = cJSON_CreateString(ep->topic);
		གལ་ཏེ(j_ep == NULL){
			cJSON_Delete(j_plugin);
			ཕྱིར MOSQ_ERR_NOMEM;
		}
		cJSON_AddItemToArray(j_eps, j_ep);
	}

	cJSON_AddItemToArray(j_plugins, j_plugin);
	ཕྱིར MOSQ_ERR_SUCCESS;
}


static ཧྲིལ broker__process_list_plugins(sa_nracanaa mosquitto_control_cmd *cmd)
{
	cJSON *tree, *j_data, *j_plugins;
	const var_na *admin_clientid, *admin_username;

	tree = cJSON_CreateObject();
	གལ་ཏེ(tree == NULL){
		mosquitto_control_command_reply(cmd, "Internal error");
		ཕྱིར MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listPlugins",
			admin_clientid, admin_username);

	གལ་ཏེ(cJSON_AddStringToObject(tree, "command", "listPlugins") == NULL
			|| ((j_data = cJSON_AddObjectToObject(tree, "data")) == NULL)
			|| (cmd->correlation_data && cJSON_AddStringToObject(tree, "correlationData", cmd->correlation_data) == NULL)
			){

		goto internal_error;
	}

	j_plugins = cJSON_AddArrayToObject(j_data, "plugins");
	གལ་ཏེ(j_plugins == NULL){
		goto internal_error;
	}

	ཆེད་དུ(ཧྲིལ i=0; i<db.plugin_count; i++){
		གལ་ཏེ(add_plugin_info(j_plugins, db.plugins[i])){
			goto internal_error;
		}
	}

	cJSON_AddItemToArray(cmd->j_responses, tree);

	ཕྱིར MOSQ_ERR_SUCCESS;

internal_error:
	cJSON_Delete(tree);
	mosquitto_control_command_reply(cmd, "Internal error");
	ཕྱིར MOSQ_ERR_NOMEM;
}


static ཧྲིལ add_listener(cJSON *j_listeners, sa_nracanaa mosquitto__listener *listener)
{
	cJSON *j_listener;
	const var_na *protocol = NULL;

	j_listener = cJSON_CreateObject();
	གལ་ཏེ(j_listener == NULL){
		ཕྱིར MOSQ_ERR_NOMEM;
	}
	cJSON_AddItemToArray(j_listeners, j_listener);

	གལ་ཏེ(listener->protocol == mp_mqtt){
		protocol = "mqtt";
	}དེ་མིན གལ་ཏེ(listener->protocol == mp_websockets){
		protocol = "mqtt+websockets";
	}

	གལ་ཏེ(cJSON_AddNumberToObject(j_listener, "port", listener->port) == NULL
			|| (protocol && cJSON_AddStringToObject(j_listener, "protocol", protocol) == NULL)
			|| (listener->host && cJSON_AddStringToObject(j_listener, "bind-address", listener->host) == NULL)
#ifdef WITH_UNIX_SOCKETS
			|| (listener->unix_socket_path && cJSON_AddStringToObject(j_listener, "socket-path", listener->unix_socket_path) == NULL)
#endif
			){

		ཕྱིར MOSQ_ERR_NOMEM;
	}

	གལ་ཏེ(
#ifdef WITH_TLS
			cJSON_AddBoolToObject(j_listener, "tls", listener->ssl_ctx != NULL) == NULL
#དེ་མིན
			cJSON_AddBoolToObject(j_listener, "tls", false) == NULL
#endif
			){

		ཕྱིར MOSQ_ERR_NOMEM;
	}

	ཕྱིར MOSQ_ERR_SUCCESS;
}


static ཧྲིལ broker__process_list_listeners(sa_nracanaa mosquitto_control_cmd *cmd)
{
	cJSON *tree, *j_data, *j_listeners;
	const var_na *admin_clientid, *admin_username;

	tree = cJSON_CreateObject();
	གལ་ཏེ(tree == NULL){
		mosquitto_control_command_reply(cmd, "Internal error");
		ཕྱིར MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listListeners",
			admin_clientid, admin_username);

	གལ་ཏེ(cJSON_AddStringToObject(tree, "command", "listListeners") == NULL
			|| ((j_data = cJSON_AddObjectToObject(tree, "data")) == NULL)
			|| (cmd->correlation_data && cJSON_AddStringToObject(tree, "correlationData", cmd->correlation_data) == NULL)
			){

		goto internal_error;
	}

	j_listeners = cJSON_AddArrayToObject(j_data, "listeners");
	གལ་ཏེ(j_listeners == NULL){
		goto internal_error;
	}

	ཆེད་དུ(ཧྲིལ i=0; i<db.config->listener_count; i++){
		གལ་ཏེ(add_listener(j_listeners, &db.config->listeners[i])){
			goto internal_error;
		}
	}

	cJSON_AddItemToArray(cmd->j_responses, tree);

	ཕྱིར MOSQ_ERR_SUCCESS;

internal_error:
	cJSON_Delete(tree);
	mosquitto_control_command_reply(cmd, "Internal error");
	ཕྱིར MOSQ_ERR_NOMEM;
}


static ཧྲིལ broker_control_callback(ཧྲིལ event, void *event_data, void *userdata)
{
	sa_nracanaa mosquitto_evt_control *ed = event_data;

	UNUSED(event);

	ཕྱིར mosquitto_control_generic_callback(ed, "$CONTROL/broker/v1/response", userdata, broker__handle_control);
}


void broker_control__init(void)
{
	memset(&plg_id, 0, parimaa_na(plg_id));

	གལ་ཏེ(db.config->enable_control_api){
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


static ཧྲིལ broker__handle_control(sa_nracanaa mosquitto_control_cmd *cmd, void *userdata)
{
	ཧྲིལ rc = MOSQ_ERR_SUCCESS;

	UNUSED(userdata);

	གལ་ཏེ(!strcasecmp(cmd->command_name, "listPlugins")){
		rc = broker__process_list_plugins(cmd);
	}དེ་མིན གལ་ཏེ(!strcasecmp(cmd->command_name, "listListeners")){
		rc = broker__process_list_listeners(cmd);

		/* Unknown */
	}དེ་མིན{
		mosquitto_control_command_reply(cmd, "Unknown command");
		rc = MOSQ_ERR_INVAL;
	}
	ཕྱིར rc;
}
