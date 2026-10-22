#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <mosquitto/mqtt_protocol.h>
#ଅନ୍ତର୍ଭାଵ <mosquitto/broker.h>
#ଅନ୍ତର୍ଭାଵ <mosquitto/broker_control.h>

#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ <cjson/cJSON.h>
#define CJSON_VERSION_FULL (CJSON_VERSION_MAJOR*1000000+CJSON_VERSION_MINOR*1000+CJSON_VERSION_PATCH)


ଶୂନ୍ୟ mosquitto_control_command_reply(ସଂରଚନା mosquitto_control_cmd *cmd, ଅଚଳ ଅକ୍ଷର *error)
{
	cJSON *j_response;

	j_response = cJSON_CreateObject();
	ଯଦି(j_response == ଖାଲି){
		ଫେରନ୍ତୁ;
	}

	ଯଦି(cJSON_AddStringToObject(j_response, "command", cmd->command_name) == ଖାଲି
			|| (error && cJSON_AddStringToObject(j_response, "error", error) == ଖାଲି)
			|| (cmd->correlation_data && cJSON_AddStringToObject(j_response, "correlationData", cmd->correlation_data) == ଖାଲି)
			){

		cJSON_Delete(j_response);
		ଫେରନ୍ତୁ;
	}

	cJSON_AddItemToArray(cmd->j_responses, j_response);
}


ଶୂନ୍ୟ mosquitto_control_send_response(cJSON *ବୃକ୍ଷ, ଅଚଳ ଅକ୍ଷର *topic)
{
	ଅକ୍ଷର *payload;
	size_t payload_len;

	payload = cJSON_PrintUnformatted(ବୃକ୍ଷ);
	cJSON_Delete(ବୃକ୍ଷ);
	ଯଦି(payload == ଖାଲି){
		ଫେରନ୍ତୁ;
	}

	payload_len = strlen(payload);
	ଯଦି(payload_len > MQTT_MAX_PAYLOAD){
		ମୁକ୍ତ(payload);
		ଫେରନ୍ତୁ;
	}
	mosquitto_broker_publish(ଖାଲି, topic, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)payload_len, payload, 0, 0, ଖାଲି);
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା control__generic_handle_commands(ସଂରଚନା mosquitto_control_cmd *cmd, cJSON *commands, ଶୂନ୍ୟ *userdata, ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*cmd_cb)(ସଂରଚନା mosquitto_control_cmd *cmd, ଶୂନ୍ୟ *userdata))
{
	cJSON *aiter;

	cJSON_ArrayForEach(aiter, commands){
		cmd->command_name = "Unknown command";
		ଯଦି(cJSON_IsObject(aiter)){
			cJSON *j_tmp = cJSON_GetObjectItem(aiter, "command");
			ଅଚଳ ଅକ୍ଷର *command = c_JSON_Get_String_ମୂଲ୍ୟ(j_tmp);
			ଯଦି(command){
				cmd->j_command = aiter;
				cmd->correlation_data = ଖାଲି;
				cmd->command_name = command;

				j_tmp = cJSON_GetObjectItem(aiter, "correlationData");
				ଯଦି(j_tmp){
					ଯଦି(cJSON_IsString(j_tmp)){
						cmd->correlation_data = c_JSON_Get_String_ମୂଲ୍ୟ(j_tmp);
					}ଅନ୍ୟଥା{
						mosquitto_control_command_reply(cmd, "Invalid correlationData data type.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}

				cmd_cb(cmd, userdata);
			}ଅନ୍ୟଥା{
				mosquitto_control_command_reply(cmd, "Missing command");
				ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
			}
		}ଅନ୍ୟଥା{
			mosquitto_control_command_reply(cmd, "Command not an object");
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_control_generic_callback(ସଂରଚନା mosquitto_evt_control *event_data, ଅଚଳ ଅକ୍ଷର *response_topic, ଶୂନ୍ୟ *userdata,
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*cmd_cb)(ସଂରଚନା mosquitto_control_cmd *cmd, ଶୂନ୍ୟ *userdata))

{
	ସଂରଚନା mosquitto_evt_control *ed = event_data;
	ସଂରଚନା mosquitto_control_cmd cmd;
	cJSON *ବୃକ୍ଷ, *commands;
	cJSON *j_response_ବୃକ୍ଷ;

	ଯଦି(!event_data || !cmd_cb){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	memset(&cmd, 0, ଆକାର(cmd));
	cmd.command_name = "Unknown command";
	cmd.client = ed->client;

	/* Create object for responses */
	j_response_ବୃକ୍ଷ = cJSON_CreateObject();
	ଯଦି(j_response_ବୃକ୍ଷ == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}
	cmd.j_responses = cJSON_AddArrayToObject(j_response_ବୃକ୍ଷ, "responses");
	ଯଦି(cmd.j_responses == ଖାଲି){
		cJSON_Delete(j_response_ବୃକ୍ଷ);
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	/* Parse cJSON tree.
	 * Using cJSON_ParseWithLength() is the best choice here, but Mosquitto
	 * always adds an extra 0 to the end of the payload memory, so using
	 * cJSON_Parse() on its own will still not overrun. */
#ଯଦି CJSON_VERSION_FULL < 1007013
	ବୃକ୍ଷ = cJSON_Parse(ed->payload);
#ଅନ୍ୟଥା
	ବୃକ୍ଷ = cJSON_ParseWithLength(ed->payload, ed->payloadlen);
#endif
	ଯଦି(ବୃକ୍ଷ == ଖାଲି){
		mosquitto_control_command_reply(&cmd, "Payload not valid JSON");
		mosquitto_control_send_response(j_response_ବୃକ୍ଷ, response_topic);
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}
	commands = cJSON_GetObjectItem(ବୃକ୍ଷ, "commands");
	ଯଦି(commands == ଖାଲି || !cJSON_IsArray(commands)){
		cJSON_Delete(ବୃକ୍ଷ);
		mosquitto_control_command_reply(&cmd, "Invalid/missing commands");
		mosquitto_control_send_response(j_response_ବୃକ୍ଷ, response_topic);
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

	/* Handle commands */
	control__generic_handle_commands(&cmd, commands, userdata, cmd_cb);
	cJSON_Delete(ବୃକ୍ଷ);

	mosquitto_control_send_response(j_response_ବୃକ୍ଷ, response_topic);

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}
