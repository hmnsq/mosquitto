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


स्थिर पूर्णाङ्क सङ्कलय_plugin_info(cJSON *j_plugins, mosquitto_plugin_id_t *pid)
{
	cJSON *j_plugin, *j_eps, *j_ep;
	संरचना control_endpoint *ep;

	यदि(pid->plugin_name == रिक्त){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	j_plugin = cJSON_CreateObject();
	यदि(j_plugin == रिक्त){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	यदि(c_JSON_सङ्कलय_String_गन्तव्यम्_Object(j_plugin, "name", pid->plugin_name) == रिक्त
			|| (pid->plugin_संस्करणम् && c_JSON_सङ्कलय_String_गन्तव्यम्_Object(j_plugin, "version", pid->plugin_संस्करणम्) == रिक्त)
			|| (pid->listener && c_JSON_सङ्कलय_सङ्ख्या_गन्तव्यम्_Object(j_plugin, "port", pid->listener->पत्तनम्) == रिक्त)
			|| (j_eps = c_JSON_सङ्कलय_Array_गन्तव्यम्_Object(j_plugin, "control-endpoints")) == रिक्त
			){

		cJSON_Delete(j_plugin);
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	DL_FOREACH(pid->control_endpoints, ep){
		j_ep = cJSON_CreateString(ep->topic);
		यदि(j_ep == रिक्त){
			cJSON_Delete(j_plugin);
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
		c_JSON_सङ्कलय_Item_गन्तव्यम्_Array(j_eps, j_ep);
	}

	c_JSON_सङ्कलय_Item_गन्तव्यम्_Array(j_plugins, j_plugin);
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क broker__process_list_plugins(संरचना mosquitto_control_cmd *cmd)
{
	cJSON *वृक्षः, *j_दत्त, *j_plugins;
	ध्रुव वर्ण *admin_clientid, *admin_username;

	वृक्षः = cJSON_CreateObject();
	यदि(वृक्षः == रिक्त){
		mosquitto_control_command_प्रत्युत्तरम्(cmd, "Internal error");
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listPlugins",
			admin_clientid, admin_username);

	यदि(c_JSON_सङ्कलय_String_गन्तव्यम्_Object(वृक्षः, "command", "listPlugins") == रिक्त
			|| ((j_दत्त = c_JSON_सङ्कलय_Object_गन्तव्यम्_Object(वृक्षः, "data")) == रिक्त)
			|| (cmd->correlation_दत्त && c_JSON_सङ्कलय_String_गन्तव्यम्_Object(वृक्षः, "correlationData", cmd->correlation_दत्त) == रिक्त)
			){

		गच्छ आन्तरिकम्_error;
	}

	j_plugins = c_JSON_सङ्कलय_Array_गन्तव्यम्_Object(j_दत्त, "plugins");
	यदि(j_plugins == रिक्त){
		गच्छ आन्तरिकम्_error;
	}

	प्रति(पूर्णाङ्क i=0; i<db.plugin_गणनम्; i++){
		यदि(सङ्कलय_plugin_info(j_plugins, db.plugins[i])){
			गच्छ आन्तरिकम्_error;
		}
	}

	c_JSON_सङ्कलय_Item_गन्तव्यम्_Array(cmd->j_responses, वृक्षः);

	प्रत्यागम MOSQ_ERR_SUCCESS;

आन्तरिकम्_error:
	cJSON_Delete(वृक्षः);
	mosquitto_control_command_प्रत्युत्तरम्(cmd, "Internal error");
	प्रत्यागम MOSQ_ERR_NOMEM;
}


स्थिर पूर्णाङ्क सङ्कलय_listener(cJSON *j_listeners, संरचना mosquitto__listener *listener)
{
	cJSON *j_listener;
	ध्रुव वर्ण *protocol = रिक्त;

	j_listener = cJSON_CreateObject();
	यदि(j_listener == रिक्त){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	c_JSON_सङ्कलय_Item_गन्तव्यम्_Array(j_listeners, j_listener);

	यदि(listener->protocol == mp_mqtt){
		protocol = "mqtt";
	}अन्यथा यदि(listener->protocol == mp_websockets){
		protocol = "mqtt+websockets";
	}

	यदि(c_JSON_सङ्कलय_सङ्ख्या_गन्तव्यम्_Object(j_listener, "port", listener->पत्तनम्) == रिक्त
			|| (protocol && c_JSON_सङ्कलय_String_गन्तव्यम्_Object(j_listener, "protocol", protocol) == रिक्त)
			|| (listener->host && c_JSON_सङ्कलय_String_गन्तव्यम्_Object(j_listener, "bind-address", listener->host) == रिक्त)
#यदि_परिभाषितम् WITH_UNIX_SOCKETS
			|| (listener->unix_socket_मार्गः && c_JSON_सङ्कलय_String_गन्तव्यम्_Object(j_listener, "socket-path", listener->unix_socket_मार्गः) == रिक्त)
#यदि_अन्तः
			){

		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	यदि(
#यदि_परिभाषितम् WITH_TLS
			c_JSON_सङ्कलय_Bool_गन्तव्यम्_Object(j_listener, "tls", listener->ssl_ctx != रिक्त) == रिक्त
#अन्यथा
			c_JSON_सङ्कलय_Bool_गन्तव्यम्_Object(j_listener, "tls", false) == रिक्त
#यदि_अन्तः
			){

		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क broker__process_list_listeners(संरचना mosquitto_control_cmd *cmd)
{
	cJSON *वृक्षः, *j_दत्त, *j_listeners;
	ध्रुव वर्ण *admin_clientid, *admin_username;

	वृक्षः = cJSON_CreateObject();
	यदि(वृक्षः == रिक्त){
		mosquitto_control_command_प्रत्युत्तरम्(cmd, "Internal error");
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	admin_clientid = mosquitto_client_id(cmd->client);
	admin_username = mosquitto_client_username(cmd->client);
	mosquitto_log_printf(MOSQ_LOG_INFO, "Broker: %s/%s | listListeners",
			admin_clientid, admin_username);

	यदि(c_JSON_सङ्कलय_String_गन्तव्यम्_Object(वृक्षः, "command", "listListeners") == रिक्त
			|| ((j_दत्त = c_JSON_सङ्कलय_Object_गन्तव्यम्_Object(वृक्षः, "data")) == रिक्त)
			|| (cmd->correlation_दत्त && c_JSON_सङ्कलय_String_गन्तव्यम्_Object(वृक्षः, "correlationData", cmd->correlation_दत्त) == रिक्त)
			){

		गच्छ आन्तरिकम्_error;
	}

	j_listeners = c_JSON_सङ्कलय_Array_गन्तव्यम्_Object(j_दत्त, "listeners");
	यदि(j_listeners == रिक्त){
		गच्छ आन्तरिकम्_error;
	}

	प्रति(पूर्णाङ्क i=0; i<db.config->listener_गणनम्; i++){
		यदि(सङ्कलय_listener(j_listeners, &db.config->listeners[i])){
			गच्छ आन्तरिकम्_error;
		}
	}

	c_JSON_सङ्कलय_Item_गन्तव्यम्_Array(cmd->j_responses, वृक्षः);

	प्रत्यागम MOSQ_ERR_SUCCESS;

आन्तरिकम्_error:
	cJSON_Delete(वृक्षः);
	mosquitto_control_command_प्रत्युत्तरम्(cmd, "Internal error");
	प्रत्यागम MOSQ_ERR_NOMEM;
}


स्थिर पूर्णाङ्क broker_control_callback(पूर्णाङ्क event, शून्य *event_दत्त, शून्य *userdata)
{
	संरचना mosquitto_evt_control *ed = event_दत्त;

	अप्रयुक्तम्(event);

	प्रत्यागम mosquitto_control_generic_callback(ed, "$CONTROL/broker/v1/response", userdata, broker__handle_control);
}


शून्य broker_control__init(शून्य)
{
	स्मृतिं_पूरय(&plg_id, 0, परिमाण(plg_id));

	यदि(db.config->enable_control_api){
		mosquitto_callback_पञ्जिकाकोशः(&plg_id, MOSQ_EVT_CONTROL, broker_control_callback, "$CONTROL/broker/v1", रिक्त);
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

	अप्रयुक्तम्(userdata);

	यदि(!strcasecmp(cmd->command_name, "listPlugins")){
		rc = broker__process_list_plugins(cmd);
	}अन्यथा यदि(!strcasecmp(cmd->command_name, "listListeners")){
		rc = broker__process_list_listeners(cmd);

		/* Unknown */
	}अन्यथा{
		mosquitto_control_command_प्रत्युत्तरम्(cmd, "Unknown command");
		rc = MOSQ_ERR_INVAL;
	}
	प्रत्यागम rc;
}
