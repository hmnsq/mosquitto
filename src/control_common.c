#include "config.h"

#include <mosquitto/mqtt_protocol.h>
#include <mosquitto/broker.h>
#include <mosquitto/broker_control.h>

#include <stdlib.h>
#include <string.h>

#include <cjson/cJSON.h>
#define CJSON_VERSION_FULL (CJSON_VERSION_MAJOR*1000000+CJSON_VERSION_MINOR*1000+CJSON_VERSION_PATCH)


void mosquitto_control_command_reply(struct mosquitto_control_cmd *cmd, const char *error)
{
	cJSON *j_response;

	j_response = cJSON_CreateObject();
	만약(j_response == NULL){
		반환;
	}

	만약(cJSON_AddStringToObject(j_response, "command", cmd->command_name) == NULL
			|| (error && cJSON_AddStringToObject(j_response, "error", error) == NULL)
			|| (cmd->correlation_data && cJSON_AddStringToObject(j_response, "correlationData", cmd->correlation_data) == NULL)
			){

		cJSON_Delete(j_response);
		반환;
	}

	cJSON_AddItemToArray(cmd->j_responses, j_response);
}


void mosquitto_control_send_response(cJSON *tree, const char *topic)
{
	char *payload;
	size_t payload_len;

	payload = cJSON_PrintUnformatted(tree);
	cJSON_Delete(tree);
	만약(payload == NULL){
		반환;
	}

	payload_len = strlen(payload);
	만약(payload_len > MQTT_MAX_PAYLOAD){
		free(payload);
		반환;
	}
	mosquitto_broker_publish(NULL, topic, (정수)payload_len, payload, 0, 0, NULL);
}


static 정수 control__generic_handle_commands(struct mosquitto_control_cmd *cmd, cJSON *commands, void *userdata, 정수 (*cmd_cb)(struct mosquitto_control_cmd *cmd, void *userdata))
{
	cJSON *aiter;

	cJSON_ArrayForEach(aiter, commands){
		cmd->command_name = "Unknown command";
		만약(cJSON_IsObject(aiter)){
			cJSON *j_tmp = cJSON_GetObjectItem(aiter, "command");
			const char *command = cJSON_GetStringValue(j_tmp);
			만약(command){
				cmd->j_command = aiter;
				cmd->correlation_data = NULL;
				cmd->command_name = command;

				j_tmp = cJSON_GetObjectItem(aiter, "correlationData");
				만약(j_tmp){
					만약(cJSON_IsString(j_tmp)){
						cmd->correlation_data = cJSON_GetStringValue(j_tmp);
					}아니면{
						mosquitto_control_command_reply(cmd, "Invalid correlationData data type.");
						반환 MOSQ_ERR_INVAL;
					}
				}

				cmd_cb(cmd, userdata);
			}아니면{
				mosquitto_control_command_reply(cmd, "Missing command");
				반환 MOSQ_ERR_INVAL;
			}
		}아니면{
			mosquitto_control_command_reply(cmd, "Command not an object");
			반환 MOSQ_ERR_INVAL;
		}
	}
	반환 MOSQ_ERR_SUCCESS;
}


정수 mosquitto_control_generic_callback(struct mosquitto_evt_control *event_data, const char *response_topic, void *userdata,
		정수 (*cmd_cb)(struct mosquitto_control_cmd *cmd, void *userdata))

{
	struct mosquitto_evt_control *ed = event_data;
	struct mosquitto_control_cmd cmd;
	cJSON *tree, *commands;
	cJSON *j_response_tree;

	만약(!event_data || !cmd_cb){
		반환 MOSQ_ERR_INVAL;
	}

	memset(&cmd, 0, sizeof(cmd));
	cmd.command_name = "Unknown command";
	cmd.client = ed->client;

	/* Create object for responses */
	j_response_tree = cJSON_CreateObject();
	만약(j_response_tree == NULL){
		반환 MOSQ_ERR_NOMEM;
	}
	cmd.j_responses = cJSON_AddArrayToObject(j_response_tree, "responses");
	만약(cmd.j_responses == NULL){
		cJSON_Delete(j_response_tree);
		반환 MOSQ_ERR_NOMEM;
	}

	/* Parse cJSON tree.
	 * Using cJSON_ParseWithLength() is the best choice here, but Mosquitto
	 * always adds an extra 0 to the end of the payload memory, so using
	 * cJSON_Parse() on its own will still not overrun. */
#만약 CJSON_VERSION_FULL < 1007013
	tree = cJSON_Parse(ed->payload);
#아니면
	tree = cJSON_ParseWithLength(ed->payload, ed->payloadlen);
#endif
	만약(tree == NULL){
		mosquitto_control_command_reply(&cmd, "Payload not valid JSON");
		mosquitto_control_send_response(j_response_tree, response_topic);
		반환 MOSQ_ERR_SUCCESS;
	}
	commands = cJSON_GetObjectItem(tree, "commands");
	만약(commands == NULL || !cJSON_IsArray(commands)){
		cJSON_Delete(tree);
		mosquitto_control_command_reply(&cmd, "Invalid/missing commands");
		mosquitto_control_send_response(j_response_tree, response_topic);
		반환 MOSQ_ERR_SUCCESS;
	}

	/* Handle commands */
	control__generic_handle_commands(&cmd, commands, userdata, cmd_cb);
	cJSON_Delete(tree);

	mosquitto_control_send_response(j_response_tree, response_topic);

	반환 MOSQ_ERR_SUCCESS;
}
