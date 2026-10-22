#include "config.h"

#include <mosquitto/mqtt_protocol.h>
#include <mosquitto/broker.h>
#include <mosquitto/broker_control.h>

#include <stdlib.h>
#include <string.h>

#include <cjson/cJSON.h>
#define CJSON_VERSION_FULL (CJSON_VERSION_MAJOR*1000000+CJSON_VERSION_MINOR*1000+CJSON_VERSION_PATCH)


void mosquitto_control_command_reply(sa_nracanaa mosquitto_control_cmd *cmd, const var_na *error)
{
	cJSON *j_response;

	j_response = cJSON_CreateObject();
	གལ་ཏེ(j_response == NULL){
		ཕྱིར;
	}

	གལ་ཏེ(cJSON_AddStringToObject(j_response, "command", cmd->command_name) == NULL
			|| (error && cJSON_AddStringToObject(j_response, "error", error) == NULL)
			|| (cmd->correlation_data && cJSON_AddStringToObject(j_response, "correlationData", cmd->correlation_data) == NULL)
			){

		cJSON_Delete(j_response);
		ཕྱིར;
	}

	cJSON_AddItemToArray(cmd->j_responses, j_response);
}


void mosquitto_control_send_response(cJSON *tree, const var_na *topic)
{
	var_na *payload;
	size_t payload_len;

	payload = cJSON_PrintUnformatted(tree);
	cJSON_Delete(tree);
	གལ་ཏེ(payload == NULL){
		ཕྱིར;
	}

	payload_len = strlen(payload);
	གལ་ཏེ(payload_len > MQTT_MAX_PAYLOAD){
		free(payload);
		ཕྱིར;
	}
	mosquitto_broker_publish(NULL, topic, (ཧྲིལ)payload_len, payload, 0, 0, NULL);
}


static ཧྲིལ control__generic_handle_commands(sa_nracanaa mosquitto_control_cmd *cmd, cJSON *commands, void *userdata, ཧྲིལ (*cmd_cb)(sa_nracanaa mosquitto_control_cmd *cmd, void *userdata))
{
	cJSON *aiter;

	cJSON_ArrayForEach(aiter, commands){
		cmd->command_name = "Unknown command";
		གལ་ཏེ(cJSON_IsObject(aiter)){
			cJSON *j_tmp = cJSON_GetObjectItem(aiter, "command");
			const var_na *command = cJSON_GetStringValue(j_tmp);
			གལ་ཏེ(command){
				cmd->j_command = aiter;
				cmd->correlation_data = NULL;
				cmd->command_name = command;

				j_tmp = cJSON_GetObjectItem(aiter, "correlationData");
				གལ་ཏེ(j_tmp){
					གལ་ཏེ(cJSON_IsString(j_tmp)){
						cmd->correlation_data = cJSON_GetStringValue(j_tmp);
					}དེ་མིན{
						mosquitto_control_command_reply(cmd, "Invalid correlationData data type.");
						ཕྱིར MOSQ_ERR_INVAL;
					}
				}

				cmd_cb(cmd, userdata);
			}དེ་མིན{
				mosquitto_control_command_reply(cmd, "Missing command");
				ཕྱིར MOSQ_ERR_INVAL;
			}
		}དེ་མིན{
			mosquitto_control_command_reply(cmd, "Command not an object");
			ཕྱིར MOSQ_ERR_INVAL;
		}
	}
	ཕྱིར MOSQ_ERR_SUCCESS;
}


ཧྲིལ mosquitto_control_generic_callback(sa_nracanaa mosquitto_evt_control *event_data, const var_na *response_topic, void *userdata,
		ཧྲིལ (*cmd_cb)(sa_nracanaa mosquitto_control_cmd *cmd, void *userdata))

{
	sa_nracanaa mosquitto_evt_control *ed = event_data;
	sa_nracanaa mosquitto_control_cmd cmd;
	cJSON *tree, *commands;
	cJSON *j_response_tree;

	གལ་ཏེ(!event_data || !cmd_cb){
		ཕྱིར MOSQ_ERR_INVAL;
	}

	memset(&cmd, 0, parimaa_na(cmd));
	cmd.command_name = "Unknown command";
	cmd.client = ed->client;

	/* Create object for responses */
	j_response_tree = cJSON_CreateObject();
	གལ་ཏེ(j_response_tree == NULL){
		ཕྱིར MOSQ_ERR_NOMEM;
	}
	cmd.j_responses = cJSON_AddArrayToObject(j_response_tree, "responses");
	གལ་ཏེ(cmd.j_responses == NULL){
		cJSON_Delete(j_response_tree);
		ཕྱིར MOSQ_ERR_NOMEM;
	}

	/* Parse cJSON tree.
	 * Using cJSON_ParseWithLength() is the best choice here, but Mosquitto
	 * always adds an extra 0 to the end of the payload memory, so using
	 * cJSON_Parse() on its own will still not overrun. */
#གལ་ཏེ CJSON_VERSION_FULL < 1007013
	tree = cJSON_Parse(ed->payload);
#དེ་མིན
	tree = cJSON_ParseWithLength(ed->payload, ed->payloadlen);
#endif
	གལ་ཏེ(tree == NULL){
		mosquitto_control_command_reply(&cmd, "Payload not valid JSON");
		mosquitto_control_send_response(j_response_tree, response_topic);
		ཕྱིར MOSQ_ERR_SUCCESS;
	}
	commands = cJSON_GetObjectItem(tree, "commands");
	གལ་ཏེ(commands == NULL || !cJSON_IsArray(commands)){
		cJSON_Delete(tree);
		mosquitto_control_command_reply(&cmd, "Invalid/missing commands");
		mosquitto_control_send_response(j_response_tree, response_topic);
		ཕྱིར MOSQ_ERR_SUCCESS;
	}

	/* Handle commands */
	control__generic_handle_commands(&cmd, commands, userdata, cmd_cb);
	cJSON_Delete(tree);

	mosquitto_control_send_response(j_response_tree, response_topic);

	ཕྱིར MOSQ_ERR_SUCCESS;
}
