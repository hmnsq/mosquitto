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

#అన్తర్భావ "config.h"

#అన్తర్భావ <cjson/cJSON.h>
#అన్తర్భావ <errno.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <sys/stat.h>
#అన్తర్భావ <utlist.h>

#అన్తర్భావ "json_help.h"
#అన్తర్భావ "mosquitto.h"
#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto/broker.h"
#అన్తర్భావ "mosquitto/broker_control.h"
#అన్తర్భావ "mosquitto/broker_plugin.h"
#అన్తర్భావ "mosquitto/mqtt_protocol.h"

స్థిర mosquitto_plugin_id_t plg_id;
స్థిర పూర్ణాంకం broker__handle_control(నిర్మాణం mosquitto_control_cmd *cmd, శూన్య *userdata);


స్థిర పూర్ణాంకం కూడు_plugin_info(cJSON *j_plugins, mosquitto_plugin_id_t *pid)
{
	cJSON *j_plugin, *j_eps, *j_ep;
	నిర్మాణం control_endpoint *ep;

	ఒకవేళ(pid->plugin_name == ఖాళీ){
		తిరిగి MOSQ_ERR_SUCCESS;
	}

	j_plugin = cJSON_CreateObject();
	ఒకవేళ(j_plugin == ఖాళీ){
		తిరిగి MOSQ_ERR_NOMEM;
	}

	ఒకవేళ(c_JSON_కూడు_String_గమ్యం_Object(j_plugin, "name", pid->plugin_name) == ఖాళీ
			|| (pid->plugin_సంస్కరణ && c_JSON_కూడు_String_గమ్యం_Object(j_plugin, "version", pid->plugin_సంస్కరణ) == ఖాళీ)
			|| (pid->listener && c_JSON_కూడు_సంఖ్య_గమ్యం_Object(j_plugin, "port", pid->listener->పోర్టు) == ఖాళీ)
			|| (j_eps = c_JSON_కూడు_Array_గమ్యం_Object(j_plugin, "control-endpoints")) == ఖాళీ
			){

		cJSON_Delete(j_plugin);
		తిరిగి MOSQ_ERR_NOMEM;
	}

	DL_FOREACH(pid->control_endpoints, ep){
		j_ep = cJSON_CreateString(ep->topic);
		ఒకవేళ(j_ep == ఖాళీ){
			cJSON_Delete(j_plugin);
			తిరిగి MOSQ_ERR_NOMEM;
		}
		c_JSON_కూడు_అంశం_గమ్యం_Array(j_eps, j_ep);
	}

	c_JSON_కూడు_అంశం_గమ్యం_Array(j_plugins, j_plugin);
	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం broker__process_list_plugins(నిర్మాణం mosquitto_control_cmd *cmd)
{
	cJSON *వృక్షం, *j_దత్తాంశం, *j_plugins;
	స్థిరరాశి అక్షరం *admin_clientid, *admin_username;

	వృక్షం = cJSON_CreateObject();
	ఒకవేళ(వృక్షం == ఖాళీ){
		mosquitto_control_command_జవాబు(cmd, "Internal error");
		తిరిగి MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listPlugins",
			admin_clientid, admin_username);

	ఒకవేళ(c_JSON_కూడు_String_గమ్యం_Object(వృక్షం, "command", "listPlugins") == ఖాళీ
			|| ((j_దత్తాంశం = c_JSON_కూడు_Object_గమ్యం_Object(వృక్షం, "data")) == ఖాళీ)
			|| (cmd->correlation_దత్తాంశం && c_JSON_కూడు_String_గమ్యం_Object(వృక్షం, "correlationData", cmd->correlation_దత్తాంశం) == ఖాళీ)
			){

		వెళ్ళు అంతర్గత_error;
	}

	j_plugins = c_JSON_కూడు_Array_గమ్యం_Object(j_దత్తాంశం, "plugins");
	ఒకవేళ(j_plugins == ఖాళీ){
		వెళ్ళు అంతర్గత_error;
	}

	కొరకు(పూర్ణాంకం i=0; i<db.plugin_లెక్క; i++){
		ఒకవేళ(కూడు_plugin_info(j_plugins, db.plugins[i])){
			వెళ్ళు అంతర్గత_error;
		}
	}

	c_JSON_కూడు_అంశం_గమ్యం_Array(cmd->j_responses, వృక్షం);

	తిరిగి MOSQ_ERR_SUCCESS;

అంతర్గత_error:
	cJSON_Delete(వృక్షం);
	mosquitto_control_command_జవాబు(cmd, "Internal error");
	తిరిగి MOSQ_ERR_NOMEM;
}


స్థిర పూర్ణాంకం కూడు_listener(cJSON *j_listeners, నిర్మాణం mosquitto__listener *listener)
{
	cJSON *j_listener;
	స్థిరరాశి అక్షరం *protocol = ఖాళీ;

	j_listener = cJSON_CreateObject();
	ఒకవేళ(j_listener == ఖాళీ){
		తిరిగి MOSQ_ERR_NOMEM;
	}
	c_JSON_కూడు_అంశం_గమ్యం_Array(j_listeners, j_listener);

	ఒకవేళ(listener->protocol == mp_mqtt){
		protocol = "mqtt";
	}లేకపోతే ఒకవేళ(listener->protocol == mp_websockets){
		protocol = "mqtt+websockets";
	}

	ఒకవేళ(c_JSON_కూడు_సంఖ్య_గమ్యం_Object(j_listener, "port", listener->పోర్టు) == ఖాళీ
			|| (protocol && c_JSON_కూడు_String_గమ్యం_Object(j_listener, "protocol", protocol) == ఖాళీ)
			|| (listener->host && c_JSON_కూడు_String_గమ్యం_Object(j_listener, "bind-address", listener->host) == ఖాళీ)
#నిర్వచితమైతే WITH_UNIX_SOCKETS
			|| (listener->unix_socket_మార్గం && c_JSON_కూడు_String_గమ్యం_Object(j_listener, "socket-path", listener->unix_socket_మార్గం) == ఖాళీ)
#అయితే_ముగింపు
			){

		తిరిగి MOSQ_ERR_NOMEM;
	}

	ఒకవేళ(
#నిర్వచితమైతే WITH_TLS
			c_JSON_కూడు_Bool_గమ్యం_Object(j_listener, "tls", listener->ssl_ctx != ఖాళీ) == ఖాళీ
#లేకపోతే
			c_JSON_కూడు_Bool_గమ్యం_Object(j_listener, "tls", false) == ఖాళీ
#అయితే_ముగింపు
			){

		తిరిగి MOSQ_ERR_NOMEM;
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం broker__process_list_listeners(నిర్మాణం mosquitto_control_cmd *cmd)
{
	cJSON *వృక్షం, *j_దత్తాంశం, *j_listeners;
	స్థిరరాశి అక్షరం *admin_clientid, *admin_username;

	వృక్షం = cJSON_CreateObject();
	ఒకవేళ(వృక్షం == ఖాళీ){
		mosquitto_control_command_జవాబు(cmd, "Internal error");
		తిరిగి MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listListeners",
			admin_clientid, admin_username);

	ఒకవేళ(c_JSON_కూడు_String_గమ్యం_Object(వృక్షం, "command", "listListeners") == ఖాళీ
			|| ((j_దత్తాంశం = c_JSON_కూడు_Object_గమ్యం_Object(వృక్షం, "data")) == ఖాళీ)
			|| (cmd->correlation_దత్తాంశం && c_JSON_కూడు_String_గమ్యం_Object(వృక్షం, "correlationData", cmd->correlation_దత్తాంశం) == ఖాళీ)
			){

		వెళ్ళు అంతర్గత_error;
	}

	j_listeners = c_JSON_కూడు_Array_గమ్యం_Object(j_దత్తాంశం, "listeners");
	ఒకవేళ(j_listeners == ఖాళీ){
		వెళ్ళు అంతర్గత_error;
	}

	కొరకు(పూర్ణాంకం i=0; i<db.config->listener_లెక్క; i++){
		ఒకవేళ(కూడు_listener(j_listeners, &db.config->listeners[i])){
			వెళ్ళు అంతర్గత_error;
		}
	}

	c_JSON_కూడు_అంశం_గమ్యం_Array(cmd->j_responses, వృక్షం);

	తిరిగి MOSQ_ERR_SUCCESS;

అంతర్గత_error:
	cJSON_Delete(వృక్షం);
	mosquitto_control_command_జవాబు(cmd, "Internal error");
	తిరిగి MOSQ_ERR_NOMEM;
}


స్థిర పూర్ణాంకం broker_control_callback(పూర్ణాంకం event, శూన్య *event_దత్తాంశం, శూన్య *userdata)
{
	నిర్మాణం mosquitto_evt_control *ed = event_దత్తాంశం;

	వాడని(event);

	తిరిగి mosquitto_control_generic_callback(ed, "$CONTROL/broker/v1/response", userdata, broker__handle_control);
}


శూన్య broker_control__init(శూన్య)
{
	memset(&plg_id, 0, పరిమాణం(plg_id));

	ఒకవేళ(db.config->enable_control_api){
		mosquitto_callback_రిజిస్టరు(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1", ఖాళీ);
	}
}


శూన్య broker_control__cleanup(శూన్య)
{
	mosquitto_callback_unregister(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1");
}


శూన్య broker_control__reload(శూన్య)
{
	broker_control__cleanup();
	broker_control__init();
}


/* ################################################################
 * #
 * # $CONTROL/broker/v1 handler
 * #
 * ################################################################ */


స్థిర పూర్ణాంకం broker__handle_control(నిర్మాణం mosquitto_control_cmd *cmd, శూన్య *userdata)
{
	పూర్ణాంకం rc = MOSQ_ERR_SUCCESS;

	వాడని(userdata);

	ఒకవేళ(!strcasecmp(cmd->command_name, "listPlugins")){
		rc = broker__process_list_plugins(cmd);
	}లేకపోతే ఒకవేళ(!strcasecmp(cmd->command_name, "listListeners")){
		rc = broker__process_list_listeners(cmd);

		/* Unknown */
	}లేకపోతే{
		mosquitto_control_command_జవాబు(cmd, "Unknown command");
		rc = MOSQ_ERR_INVAL;
	}
	తిరిగి rc;
}
