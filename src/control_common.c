#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <mosquitto/mqtt_protocol.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mosquitto/broker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mosquitto/broker_control.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <cjson/cJSON.h>
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CJSON_ਸੰਸਕਰਣ_ਭਰਿਆ (CJSON_ਸੰਸਕਰਣ_MAJOR*1000000+CJSON_ਸੰਸਕਰਣ_MINOR*1000+CJSON_ਸੰਸਕਰਣ_PATCH)


ਖਾਲੀ mosquitto_control_command_ਜਵਾਬ(ਬਣਤਰ mosquitto_control_cmd *cmd, ਅਚਲ ਅੱਖਰ *error)
{
	cJSON *j_response;

	j_response = cJSON_CreateObject();
	ਜੇ(j_response == ਨਿਰਾ){
		ਵਾਪਸ;
	}

	ਜੇ(c_JSON_ਜੋੜੋ_String_ਮੰਜ਼ਿਲ_Object(j_response, "command", cmd->command_name) == ਨਿਰਾ
			|| (error && c_JSON_ਜੋੜੋ_String_ਮੰਜ਼ਿਲ_Object(j_response, "error", error) == ਨਿਰਾ)
			|| (cmd->correlation_ਅੰਕੜੇ && c_JSON_ਜੋੜੋ_String_ਮੰਜ਼ਿਲ_Object(j_response, "correlationData", cmd->correlation_ਅੰਕੜੇ) == ਨਿਰਾ)
			){

		cJSON_Delete(j_response);
		ਵਾਪਸ;
	}

	c_JSON_ਜੋੜੋ_ਵਸਤੂ_ਮੰਜ਼ਿਲ_Array(cmd->j_responses, j_response);
}


ਖਾਲੀ mosquitto_control_send_response(cJSON *ਰੁੱਖ, ਅਚਲ ਅੱਖਰ *topic)
{
	ਅੱਖਰ *payload;
	size_t payload_len;

	payload = cJSON_PrintUnformatted(ਰੁੱਖ);
	cJSON_Delete(ਰੁੱਖ);
	ਜੇ(payload == ਨਿਰਾ){
		ਵਾਪਸ;
	}

	payload_len = ਲੜੀ_ਲੰਬਾਈ(payload);
	ਜੇ(payload_len > MQTT_MAX_PAYLOAD){
		ਮੁਕਤ(payload);
		ਵਾਪਸ;
	}
	mosquitto_broker_publish(ਨਿਰਾ, topic, (ਪੂਰਨ)payload_len, payload, 0, 0, ਨਿਰਾ);
}


ਸਥਿਰ ਪੂਰਨ control__generic_handle_commands(ਬਣਤਰ mosquitto_control_cmd *cmd, cJSON *commands, ਖਾਲੀ *userdata, ਪੂਰਨ (*cmd_cb)(ਬਣਤਰ mosquitto_control_cmd *cmd, ਖਾਲੀ *userdata))
{
	cJSON *aiter;

	cJSON_ArrayForEach(aiter, commands){
		cmd->command_name = "Unknown command";
		ਜੇ(cJSON_IsObject(aiter)){
			cJSON *j_tmp = c_JSON_Get_Object_ਵਸਤੂ(aiter, "command");
			ਅਚਲ ਅੱਖਰ *command = c_JSON_Get_String_ਮੁੱਲ(j_tmp);
			ਜੇ(command){
				cmd->j_command = aiter;
				cmd->correlation_ਅੰਕੜੇ = ਨਿਰਾ;
				cmd->command_name = command;

				j_tmp = c_JSON_Get_Object_ਵਸਤੂ(aiter, "correlationData");
				ਜੇ(j_tmp){
					ਜੇ(cJSON_IsString(j_tmp)){
						cmd->correlation_ਅੰਕੜੇ = c_JSON_Get_String_ਮੁੱਲ(j_tmp);
					}ਨਹੀਂਤਾਂ{
						mosquitto_control_command_ਜਵਾਬ(cmd, "Invalid correlationData data type.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}

				cmd_cb(cmd, userdata);
			}ਨਹੀਂਤਾਂ{
				mosquitto_control_command_ਜਵਾਬ(cmd, "Missing command");
				ਵਾਪਸ MOSQ_ERR_INVAL;
			}
		}ਨਹੀਂਤਾਂ{
			mosquitto_control_command_ਜਵਾਬ(cmd, "Command not an object");
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ mosquitto_control_generic_callback(ਬਣਤਰ mosquitto_evt_control *event_ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *response_topic, ਖਾਲੀ *userdata,
		ਪੂਰਨ (*cmd_cb)(ਬਣਤਰ mosquitto_control_cmd *cmd, ਖਾਲੀ *userdata))

{
	ਬਣਤਰ mosquitto_evt_control *ed = event_ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto_control_cmd cmd;
	cJSON *ਰੁੱਖ, *commands;
	cJSON *j_response_ਰੁੱਖ;

	ਜੇ(!event_ਅੰਕੜੇ || !cmd_cb){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	memset(&cmd, 0, ਆਕਾਰ(cmd));
	cmd.command_name = "Unknown command";
	cmd.client = ed->client;

	/* Create object for responses */
	j_response_ਰੁੱਖ = cJSON_CreateObject();
	ਜੇ(j_response_ਰੁੱਖ == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}
	cmd.j_responses = c_JSON_ਜੋੜੋ_Array_ਮੰਜ਼ਿਲ_Object(j_response_ਰੁੱਖ, "responses");
	ਜੇ(cmd.j_responses == ਨਿਰਾ){
		cJSON_Delete(j_response_ਰੁੱਖ);
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	/* Parse cJSON tree.
	 * Using cJSON_ParseWithLength() is the best choice here, but Mosquitto
	 * always adds an extra 0 to the end of the payload memory, so using
	 * cJSON_Parse() on its own will still not overrun. */
#ਜੇ CJSON_ਸੰਸਕਰਣ_ਭਰਿਆ < 1007013
	ਰੁੱਖ = cJSON_Parse(ed->payload);
#ਨਹੀਂਤਾਂ
	ਰੁੱਖ = c_JSON_Parse_With_ਲੰਮਾਈ(ed->payload, ed->payloadlen);
#ਜੇ_ਅੰਤ
	ਜੇ(ਰੁੱਖ == ਨਿਰਾ){
		mosquitto_control_command_ਜਵਾਬ(&cmd, "Payload not valid JSON");
		mosquitto_control_send_response(j_response_ਰੁੱਖ, response_topic);
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}
	commands = c_JSON_Get_Object_ਵਸਤੂ(ਰੁੱਖ, "commands");
	ਜੇ(commands == ਨਿਰਾ || !cJSON_IsArray(commands)){
		cJSON_Delete(ਰੁੱਖ);
		mosquitto_control_command_ਜਵਾਬ(&cmd, "Invalid/missing commands");
		mosquitto_control_send_response(j_response_ਰੁੱਖ, response_topic);
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

	/* Handle commands */
	control__generic_handle_commands(&cmd, commands, userdata, cmd_cb);
	cJSON_Delete(ਰੁੱਖ);

	mosquitto_control_send_response(j_response_ਰੁੱਖ, response_topic);

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}
