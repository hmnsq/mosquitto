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
	もし(j_response == NULL){
		戻る;
	}

	もし(cJSON_AddStringToObject(j_response, "command", cmd->command_name) == NULL
			|| (error && cJSON_AddStringToObject(j_response, "error", error) == NULL)
			|| (cmd->correlation_data && cJSON_AddStringToObject(j_response, "correlationData", cmd->correlation_data) == NULL)
			){

		cJSON_Delete(j_response);
		戻る;
	}

	cJSON_AddItemToArray(cmd->j_responses, j_response);
}


void mosquitto_control_send_response(cJSON *tree, const char *topic)
{
	char *payload;
	size_t payload_len;

	payload = cJSON_PrintUnformatted(tree);
	cJSON_Delete(tree);
	もし(payload == NULL){
		戻る;
	}

	payload_len = strlen(payload);
	もし(payload_len > MQTT_MAX_PAYLOAD){
		free(payload);
		戻る;
	}
	mosquitto_broker_publish(NULL, topic, (整数)payload_len, payload, 0, 0, NULL);
}


static 整数 control__generic_handle_commands(struct mosquitto_control_cmd *cmd, cJSON *commands, void *userdata, 整数 (*cmd_cb)(struct mosquitto_control_cmd *cmd, void *userdata))
{
	cJSON *aiter;

	cJSON_ArrayForEach(aiter, commands){
		cmd->command_name = "Unknown command";
		もし(cJSON_IsObject(aiter)){
			cJSON *j_tmp = cJSON_GetObjectItem(aiter, "command");
			const char *command = cJSON_GetStringValue(j_tmp);
			もし(command){
				cmd->j_command = aiter;
				cmd->correlation_data = NULL;
				cmd->command_name = command;

				j_tmp = cJSON_GetObjectItem(aiter, "correlationData");
				もし(j_tmp){
					もし(cJSON_IsString(j_tmp)){
						cmd->correlation_data = cJSON_GetStringValue(j_tmp);
					}否则{
						mosquitto_control_command_reply(cmd, "Invalid correlationData data type.");
						戻る MOSQ_ERR_INVAL;
					}
				}

				cmd_cb(cmd, userdata);
			}否则{
				mosquitto_control_command_reply(cmd, "Missing command");
				戻る MOSQ_ERR_INVAL;
			}
		}否则{
			mosquitto_control_command_reply(cmd, "Command not an object");
			戻る MOSQ_ERR_INVAL;
		}
	}
	戻る MOSQ_ERR_SUCCESS;
}


整数 mosquitto_control_generic_callback(struct mosquitto_evt_control *event_data, const char *response_topic, void *userdata,
		整数 (*cmd_cb)(struct mosquitto_control_cmd *cmd, void *userdata))

{
	struct mosquitto_evt_control *ed = event_data;
	struct mosquitto_control_cmd cmd;
	cJSON *tree, *commands;
	cJSON *j_response_tree;

	もし(!event_data || !cmd_cb){
		戻る MOSQ_ERR_INVAL;
	}

	memset(&cmd, 0, sizeof(cmd));
	cmd.command_name = "Unknown command";
	cmd.client = ed->client;

	/* Create object for responses */
	j_response_tree = cJSON_CreateObject();
	もし(j_response_tree == NULL){
		戻る MOSQ_ERR_NOMEM;
	}
	cmd.j_responses = cJSON_AddArrayToObject(j_response_tree, "responses");
	もし(cmd.j_responses == NULL){
		cJSON_Delete(j_response_tree);
		戻る MOSQ_ERR_NOMEM;
	}

	/* Parse cJSON tree.
	 * Using cJSON_ParseWithLength() is the best choice here, but Mosquitto
	 * always adds an extra 0 to the end of the payload memory, so using
	 * cJSON_Parse() on its own will still not overrun. */
#もし CJSON_VERSION_FULL < 1007013
	tree = cJSON_Parse(ed->payload);
#否则
	tree = cJSON_ParseWithLength(ed->payload, ed->payloadlen);
#endif
	もし(tree == NULL){
		mosquitto_control_command_reply(&cmd, "Payload not valid JSON");
		mosquitto_control_send_response(j_response_tree, response_topic);
		戻る MOSQ_ERR_SUCCESS;
	}
	commands = cJSON_GetObjectItem(tree, "commands");
	もし(commands == NULL || !cJSON_IsArray(commands)){
		cJSON_Delete(tree);
		mosquitto_control_command_reply(&cmd, "Invalid/missing commands");
		mosquitto_control_send_response(j_response_tree, response_topic);
		戻る MOSQ_ERR_SUCCESS;
	}

	/* Handle commands */
	control__generic_handle_commands(&cmd, commands, userdata, cmd_cb);
	cJSON_Delete(tree);

	mosquitto_control_send_response(j_response_tree, response_topic);

	戻る MOSQ_ERR_SUCCESS;
}
