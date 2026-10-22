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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ <cjson/cJSON.h>
#அந்தர்பாவ <errno.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <sys/stat.h>
#அந்தர்பாவ <utlist.h>

#அந்தர்பாவ "json_help.h"
#அந்தர்பாவ "mosquitto.h"
#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto/broker.h"
#அந்தர்பாவ "mosquitto/broker_control.h"
#அந்தர்பாவ "mosquitto/broker_plugin.h"
#அந்தர்பாவ "mosquitto/mqtt_protocol.h"

நிலையான mosquitto_plugin_id_t plg_id;
நிலையான முழு broker__handle_control(கட்டமைப்பு mosquitto_control_cmd *cmd, வெறுமை *userdata);


நிலையான முழு add_plugin_info(cJSON *j_plugins, mosquitto_plugin_id_t *pid)
{
	cJSON *j_plugin, *j_eps, *j_ep;
	கட்டமைப்பு control_endpoint *ep;

	எனில்(pid->plugin_name == வெற்று){
		திரும்பு MOSQ_ERR_SUCCESS;
	}

	j_plugin = cJSON_CreateObject();
	எனில்(j_plugin == வெற்று){
		திரும்பு MOSQ_ERR_NOMEM;
	}

	எனில்(cJSON_AddStringToObject(j_plugin, "name", pid->plugin_name) == வெற்று
			|| (pid->plugin_version && cJSON_AddStringToObject(j_plugin, "version", pid->plugin_version) == வெற்று)
			|| (pid->listener && c_JSON_Add_எண்_To_Object(j_plugin, "port", pid->listener->port) == வெற்று)
			|| (j_eps = cJSON_AddArrayToObject(j_plugin, "control-endpoints")) == வெற்று
			){

		cJSON_Delete(j_plugin);
		திரும்பு MOSQ_ERR_NOMEM;
	}

	DL_FOREACH(pid->control_endpoints, ep){
		j_ep = cJSON_CreateString(ep->topic);
		எனில்(j_ep == வெற்று){
			cJSON_Delete(j_plugin);
			திரும்பு MOSQ_ERR_NOMEM;
		}
		cJSON_AddItemToArray(j_eps, j_ep);
	}

	cJSON_AddItemToArray(j_plugins, j_plugin);
	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு broker__process_list_plugins(கட்டமைப்பு mosquitto_control_cmd *cmd)
{
	cJSON *மரம், *j_data, *j_plugins;
	மாறிலி எழுத்து *admin_clientid, *admin_username;

	மரம் = cJSON_CreateObject();
	எனில்(மரம் == வெற்று){
		mosquitto_control_command_reply(cmd, "Internal error");
		திரும்பு MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listPlugins",
			admin_clientid, admin_username);

	எனில்(cJSON_AddStringToObject(மரம், "command", "listPlugins") == வெற்று
			|| ((j_data = cJSON_AddObjectToObject(மரம், "data")) == வெற்று)
			|| (cmd->correlation_data && cJSON_AddStringToObject(மரம், "correlationData", cmd->correlation_data) == வெற்று)
			){

		செல் internal_error;
	}

	j_plugins = cJSON_AddArrayToObject(j_data, "plugins");
	எனில்(j_plugins == வெற்று){
		செல் internal_error;
	}

	க்கு(முழு i=0; i<db.plugin_மொத்தம்; i++){
		எனில்(add_plugin_info(j_plugins, db.plugins[i])){
			செல் internal_error;
		}
	}

	cJSON_AddItemToArray(cmd->j_responses, மரம்);

	திரும்பு MOSQ_ERR_SUCCESS;

internal_error:
	cJSON_Delete(மரம்);
	mosquitto_control_command_reply(cmd, "Internal error");
	திரும்பு MOSQ_ERR_NOMEM;
}


நிலையான முழு add_listener(cJSON *j_listeners, கட்டமைப்பு mosquitto__listener *listener)
{
	cJSON *j_listener;
	மாறிலி எழுத்து *protocol = வெற்று;

	j_listener = cJSON_CreateObject();
	எனில்(j_listener == வெற்று){
		திரும்பு MOSQ_ERR_NOMEM;
	}
	cJSON_AddItemToArray(j_listeners, j_listener);

	எனில்(listener->protocol == mp_mqtt){
		protocol = "mqtt";
	}இல்லையெனில் எனில்(listener->protocol == mp_websockets){
		protocol = "mqtt+websockets";
	}

	எனில்(c_JSON_Add_எண்_To_Object(j_listener, "port", listener->port) == வெற்று
			|| (protocol && cJSON_AddStringToObject(j_listener, "protocol", protocol) == வெற்று)
			|| (listener->host && cJSON_AddStringToObject(j_listener, "bind-address", listener->host) == வெற்று)
#ifdef WITH_UNIX_SOCKETS
			|| (listener->unix_socket_path && cJSON_AddStringToObject(j_listener, "socket-path", listener->unix_socket_path) == வெற்று)
#endif
			){

		திரும்பு MOSQ_ERR_NOMEM;
	}

	எனில்(
#ifdef WITH_TLS
			cJSON_AddBoolToObject(j_listener, "tls", listener->ssl_ctx != வெற்று) == வெற்று
#இல்லையெனில்
			cJSON_AddBoolToObject(j_listener, "tls", false) == வெற்று
#endif
			){

		திரும்பு MOSQ_ERR_NOMEM;
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு broker__process_list_listeners(கட்டமைப்பு mosquitto_control_cmd *cmd)
{
	cJSON *மரம், *j_data, *j_listeners;
	மாறிலி எழுத்து *admin_clientid, *admin_username;

	மரம் = cJSON_CreateObject();
	எனில்(மரம் == வெற்று){
		mosquitto_control_command_reply(cmd, "Internal error");
		திரும்பு MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listListeners",
			admin_clientid, admin_username);

	எனில்(cJSON_AddStringToObject(மரம், "command", "listListeners") == வெற்று
			|| ((j_data = cJSON_AddObjectToObject(மரம், "data")) == வெற்று)
			|| (cmd->correlation_data && cJSON_AddStringToObject(மரம், "correlationData", cmd->correlation_data) == வெற்று)
			){

		செல் internal_error;
	}

	j_listeners = cJSON_AddArrayToObject(j_data, "listeners");
	எனில்(j_listeners == வெற்று){
		செல் internal_error;
	}

	க்கு(முழு i=0; i<db.config->listener_மொத்தம்; i++){
		எனில்(add_listener(j_listeners, &db.config->listeners[i])){
			செல் internal_error;
		}
	}

	cJSON_AddItemToArray(cmd->j_responses, மரம்);

	திரும்பு MOSQ_ERR_SUCCESS;

internal_error:
	cJSON_Delete(மரம்);
	mosquitto_control_command_reply(cmd, "Internal error");
	திரும்பு MOSQ_ERR_NOMEM;
}


நிலையான முழு broker_control_callback(முழு event, வெறுமை *event_data, வெறுமை *userdata)
{
	கட்டமைப்பு mosquitto_evt_control *ed = event_data;

	UNUSED(event);

	திரும்பு mosquitto_control_generic_callback(ed, "$CONTROL/broker/v1/response", userdata, broker__handle_control);
}


வெறுமை broker_control__init(வெறுமை)
{
	memset(&plg_id, 0, அளவு(plg_id));

	எனில்(db.config->enable_control_api){
		mosquitto_callback_register(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1", வெற்று);
	}
}


வெறுமை broker_control__cleanup(வெறுமை)
{
	mosquitto_callback_unregister(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1");
}


வெறுமை broker_control__reload(வெறுமை)
{
	broker_control__cleanup();
	broker_control__init();
}


/* ################################################################
 * #
 * # $CONTROL/broker/v1 handler
 * #
 * ################################################################ */


நிலையான முழு broker__handle_control(கட்டமைப்பு mosquitto_control_cmd *cmd, வெறுமை *userdata)
{
	முழு rc = MOSQ_ERR_SUCCESS;

	UNUSED(userdata);

	எனில்(!strcasecmp(cmd->command_name, "listPlugins")){
		rc = broker__process_list_plugins(cmd);
	}இல்லையெனில் எனில்(!strcasecmp(cmd->command_name, "listListeners")){
		rc = broker__process_list_listeners(cmd);

		/* Unknown */
	}இல்லையெனில்{
		mosquitto_control_command_reply(cmd, "Unknown command");
		rc = MOSQ_ERR_INVAL;
	}
	திரும்பு rc;
}
