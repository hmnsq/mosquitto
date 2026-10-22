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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <cjson/cJSON.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <utlist.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "json_help.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/broker.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/broker_control.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/broker_plugin.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"

ਸਥਿਰ mosquitto_plugin_id_t plg_id;
ਸਥਿਰ ਪੂਰਨ broker__handle_control(ਬਣਤਰ mosquitto_control_cmd *cmd, ਖਾਲੀ *userdata);


ਸਥਿਰ ਪੂਰਨ ਜੋੜੋ_plugin_info(cJSON *j_plugins, mosquitto_plugin_id_t *pid)
{
	cJSON *j_plugin, *j_eps, *j_ep;
	ਬਣਤਰ control_endpoint *ep;

	ਜੇ(pid->plugin_name == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

	j_plugin = cJSON_CreateObject();
	ਜੇ(j_plugin == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	ਜੇ(c_JSON_ਜੋੜੋ_String_ਮੰਜ਼ਿਲ_Object(j_plugin, "name", pid->plugin_name) == ਨਿਰਾ
			|| (pid->plugin_ਸੰਸਕਰਣ && c_JSON_ਜੋੜੋ_String_ਮੰਜ਼ਿਲ_Object(j_plugin, "version", pid->plugin_ਸੰਸਕਰਣ) == ਨਿਰਾ)
			|| (pid->listener && c_JSON_ਜੋੜੋ_ਸੰਖਿਆ_ਮੰਜ਼ਿਲ_Object(j_plugin, "port", pid->listener->ਪੋਰਟ) == ਨਿਰਾ)
			|| (j_eps = c_JSON_ਜੋੜੋ_Array_ਮੰਜ਼ਿਲ_Object(j_plugin, "control-endpoints")) == ਨਿਰਾ
			){

		cJSON_Delete(j_plugin);
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	DL_FOREACH(pid->control_endpoints, ep){
		j_ep = cJSON_CreateString(ep->topic);
		ਜੇ(j_ep == ਨਿਰਾ){
			cJSON_Delete(j_plugin);
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}
		c_JSON_ਜੋੜੋ_ਵਸਤੂ_ਮੰਜ਼ਿਲ_Array(j_eps, j_ep);
	}

	c_JSON_ਜੋੜੋ_ਵਸਤੂ_ਮੰਜ਼ਿਲ_Array(j_plugins, j_plugin);
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ broker__process_list_plugins(ਬਣਤਰ mosquitto_control_cmd *cmd)
{
	cJSON *ਰੁੱਖ, *j_ਅੰਕੜੇ, *j_plugins;
	ਅਚਲ ਅੱਖਰ *admin_clientid, *admin_username;

	ਰੁੱਖ = cJSON_CreateObject();
	ਜੇ(ਰੁੱਖ == ਨਿਰਾ){
		mosquitto_control_command_ਜਵਾਬ(cmd, "Internal error");
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listPlugins",
			admin_clientid, admin_username);

	ਜੇ(c_JSON_ਜੋੜੋ_String_ਮੰਜ਼ਿਲ_Object(ਰੁੱਖ, "command", "listPlugins") == ਨਿਰਾ
			|| ((j_ਅੰਕੜੇ = c_JSON_ਜੋੜੋ_Object_ਮੰਜ਼ਿਲ_Object(ਰੁੱਖ, "data")) == ਨਿਰਾ)
			|| (cmd->correlation_ਅੰਕੜੇ && c_JSON_ਜੋੜੋ_String_ਮੰਜ਼ਿਲ_Object(ਰੁੱਖ, "correlationData", cmd->correlation_ਅੰਕੜੇ) == ਨਿਰਾ)
			){

		ਜਾਓ ਅੰਦਰੂਨੀ_error;
	}

	j_plugins = c_JSON_ਜੋੜੋ_Array_ਮੰਜ਼ਿਲ_Object(j_ਅੰਕੜੇ, "plugins");
	ਜੇ(j_plugins == ਨਿਰਾ){
		ਜਾਓ ਅੰਦਰੂਨੀ_error;
	}

	ਲਈ(ਪੂਰਨ i=0; i<db.plugin_ਗਿਣਤੀ; i++){
		ਜੇ(ਜੋੜੋ_plugin_info(j_plugins, db.plugins[i])){
			ਜਾਓ ਅੰਦਰੂਨੀ_error;
		}
	}

	c_JSON_ਜੋੜੋ_ਵਸਤੂ_ਮੰਜ਼ਿਲ_Array(cmd->j_responses, ਰੁੱਖ);

	ਵਾਪਸ MOSQ_ERR_SUCCESS;

ਅੰਦਰੂਨੀ_error:
	cJSON_Delete(ਰੁੱਖ);
	mosquitto_control_command_ਜਵਾਬ(cmd, "Internal error");
	ਵਾਪਸ MOSQ_ERR_NOMEM;
}


ਸਥਿਰ ਪੂਰਨ ਜੋੜੋ_listener(cJSON *j_listeners, ਬਣਤਰ mosquitto__listener *listener)
{
	cJSON *j_listener;
	ਅਚਲ ਅੱਖਰ *protocol = ਨਿਰਾ;

	j_listener = cJSON_CreateObject();
	ਜੇ(j_listener == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}
	c_JSON_ਜੋੜੋ_ਵਸਤੂ_ਮੰਜ਼ਿਲ_Array(j_listeners, j_listener);

	ਜੇ(listener->protocol == mp_mqtt){
		protocol = "mqtt";
	}ਨਹੀਂਤਾਂ ਜੇ(listener->protocol == mp_websockets){
		protocol = "mqtt+websockets";
	}

	ਜੇ(c_JSON_ਜੋੜੋ_ਸੰਖਿਆ_ਮੰਜ਼ਿਲ_Object(j_listener, "port", listener->ਪੋਰਟ) == ਨਿਰਾ
			|| (protocol && c_JSON_ਜੋੜੋ_String_ਮੰਜ਼ਿਲ_Object(j_listener, "protocol", protocol) == ਨਿਰਾ)
			|| (listener->host && c_JSON_ਜੋੜੋ_String_ਮੰਜ਼ਿਲ_Object(j_listener, "bind-address", listener->host) == ਨਿਰਾ)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_UNIX_SOCKETS
			|| (listener->unix_socket_ਰਸਤਾ && c_JSON_ਜੋੜੋ_String_ਮੰਜ਼ਿਲ_Object(j_listener, "socket-path", listener->unix_socket_ਰਸਤਾ) == ਨਿਰਾ)
#ਜੇ_ਅੰਤ
			){

		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	ਜੇ(
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
			c_JSON_ਜੋੜੋ_Bool_ਮੰਜ਼ਿਲ_Object(j_listener, "tls", listener->ssl_ctx != ਨਿਰਾ) == ਨਿਰਾ
#ਨਹੀਂਤਾਂ
			c_JSON_ਜੋੜੋ_Bool_ਮੰਜ਼ਿਲ_Object(j_listener, "tls", false) == ਨਿਰਾ
#ਜੇ_ਅੰਤ
			){

		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ broker__process_list_listeners(ਬਣਤਰ mosquitto_control_cmd *cmd)
{
	cJSON *ਰੁੱਖ, *j_ਅੰਕੜੇ, *j_listeners;
	ਅਚਲ ਅੱਖਰ *admin_clientid, *admin_username;

	ਰੁੱਖ = cJSON_CreateObject();
	ਜੇ(ਰੁੱਖ == ਨਿਰਾ){
		mosquitto_control_command_ਜਵਾਬ(cmd, "Internal error");
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listListeners",
			admin_clientid, admin_username);

	ਜੇ(c_JSON_ਜੋੜੋ_String_ਮੰਜ਼ਿਲ_Object(ਰੁੱਖ, "command", "listListeners") == ਨਿਰਾ
			|| ((j_ਅੰਕੜੇ = c_JSON_ਜੋੜੋ_Object_ਮੰਜ਼ਿਲ_Object(ਰੁੱਖ, "data")) == ਨਿਰਾ)
			|| (cmd->correlation_ਅੰਕੜੇ && c_JSON_ਜੋੜੋ_String_ਮੰਜ਼ਿਲ_Object(ਰੁੱਖ, "correlationData", cmd->correlation_ਅੰਕੜੇ) == ਨਿਰਾ)
			){

		ਜਾਓ ਅੰਦਰੂਨੀ_error;
	}

	j_listeners = c_JSON_ਜੋੜੋ_Array_ਮੰਜ਼ਿਲ_Object(j_ਅੰਕੜੇ, "listeners");
	ਜੇ(j_listeners == ਨਿਰਾ){
		ਜਾਓ ਅੰਦਰੂਨੀ_error;
	}

	ਲਈ(ਪੂਰਨ i=0; i<db.config->listener_ਗਿਣਤੀ; i++){
		ਜੇ(ਜੋੜੋ_listener(j_listeners, &db.config->listeners[i])){
			ਜਾਓ ਅੰਦਰੂਨੀ_error;
		}
	}

	c_JSON_ਜੋੜੋ_ਵਸਤੂ_ਮੰਜ਼ਿਲ_Array(cmd->j_responses, ਰੁੱਖ);

	ਵਾਪਸ MOSQ_ERR_SUCCESS;

ਅੰਦਰੂਨੀ_error:
	cJSON_Delete(ਰੁੱਖ);
	mosquitto_control_command_ਜਵਾਬ(cmd, "Internal error");
	ਵਾਪਸ MOSQ_ERR_NOMEM;
}


ਸਥਿਰ ਪੂਰਨ broker_control_callback(ਪੂਰਨ event, ਖਾਲੀ *event_ਅੰਕੜੇ, ਖਾਲੀ *userdata)
{
	ਬਣਤਰ mosquitto_evt_control *ed = event_ਅੰਕੜੇ;

	ਅਣਵਰਤਿਆ(event);

	ਵਾਪਸ mosquitto_control_generic_callback(ed, "$CONTROL/broker/v1/response", userdata, broker__handle_control);
}


ਖਾਲੀ broker_control__init(ਖਾਲੀ)
{
	memset(&plg_id, 0, ਆਕਾਰ(plg_id));

	ਜੇ(db.config->enable_control_api){
		mosquitto_callback_ਰਜਿਸਟਰ(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1", ਨਿਰਾ);
	}
}


ਖਾਲੀ broker_control__cleanup(ਖਾਲੀ)
{
	mosquitto_callback_unregister(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1");
}


ਖਾਲੀ broker_control__reload(ਖਾਲੀ)
{
	broker_control__cleanup();
	broker_control__init();
}


/* ################################################################
 * #
 * # $CONTROL/broker/v1 handler
 * #
 * ################################################################ */


ਸਥਿਰ ਪੂਰਨ broker__handle_control(ਬਣਤਰ mosquitto_control_cmd *cmd, ਖਾਲੀ *userdata)
{
	ਪੂਰਨ rc = MOSQ_ERR_SUCCESS;

	ਅਣਵਰਤਿਆ(userdata);

	ਜੇ(!strcasecmp(cmd->command_name, "listPlugins")){
		rc = broker__process_list_plugins(cmd);
	}ਨਹੀਂਤਾਂ ਜੇ(!strcasecmp(cmd->command_name, "listListeners")){
		rc = broker__process_list_listeners(cmd);

		/* Unknown */
	}ਨਹੀਂਤਾਂ{
		mosquitto_control_command_ਜਵਾਬ(cmd, "Unknown command");
		rc = MOSQ_ERR_INVAL;
	}
	ਵਾਪਸ rc;
}
