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
	නම්(j_response == NULL){
		ආපසු;
	}

	නම්(cJSON_AddStringToObject(j_response, "command", cmd->command_name) == NULL
			|| (error && cJSON_AddStringToObject(j_response, "error", error) == NULL)
			|| (cmd->correlation_data && cJSON_AddStringToObject(j_response, "correlationData", cmd->correlation_data) == NULL)
			){

		cJSON_Delete(j_response);
		ආපසු;
	}

	cJSON_AddItemToArray(cmd->j_responses, j_response);
}


void mosquitto_control_send_response(cJSON *tree, const var_na *topic)
{
	var_na *payload;
	size_t payload_len;

	payload = cJSON_PrintUnformatted(tree);
	cJSON_Delete(tree);
	නම්(payload == NULL){
		ආපසු;
	}

	payload_len = strlen(payload);
	නම්(payload_len > MQTT_MAX_PAYLOAD){
		free(payload);
		ආපසු;
	}
	mosquitto_broker_publish(NULL, topic, (පූර්ණ_සංඛ්‍යා)payload_len, payload, 0, 0, NULL);
}


static පූර්ණ_සංඛ්‍යා control__generic_handle_commands(sa_nracanaa mosquitto_control_cmd *cmd, cJSON *commands, void *userdata, පූර්ණ_සංඛ්‍යා (*cmd_cb)(sa_nracanaa mosquitto_control_cmd *cmd, void *userdata))
{
	cJSON *aiter;

	cJSON_ArrayForEach(aiter, commands){
		cmd->command_name = "Unknown command";
		නම්(cJSON_IsObject(aiter)){
			cJSON *j_tmp = cJSON_GetObjectItem(aiter, "command");
			const var_na *command = cJSON_GetStringValue(j_tmp);
			නම්(command){
				cmd->j_command = aiter;
				cmd->correlation_data = NULL;
				cmd->command_name = command;

				j_tmp = cJSON_GetObjectItem(aiter, "correlationData");
				නම්(j_tmp){
					නම්(cJSON_IsString(j_tmp)){
						cmd->correlation_data = cJSON_GetStringValue(j_tmp);
					}නැතහොත්{
						mosquitto_control_command_reply(cmd, "Invalid correlationData data type.");
						ආපසු MOSQ_ERR_INVAL;
					}
				}

				cmd_cb(cmd, userdata);
			}නැතහොත්{
				mosquitto_control_command_reply(cmd, "Missing command");
				ආපසු MOSQ_ERR_INVAL;
			}
		}නැතහොත්{
			mosquitto_control_command_reply(cmd, "Command not an object");
			ආපසු MOSQ_ERR_INVAL;
		}
	}
	ආපසු MOSQ_ERR_SUCCESS;
}


පූර්ණ_සංඛ්‍යා mosquitto_control_generic_callback(sa_nracanaa mosquitto_evt_control *event_data, const var_na *response_topic, void *userdata,
		පූර්ණ_සංඛ්‍යා (*cmd_cb)(sa_nracanaa mosquitto_control_cmd *cmd, void *userdata))

{
	sa_nracanaa mosquitto_evt_control *ed = event_data;
	sa_nracanaa mosquitto_control_cmd cmd;
	cJSON *tree, *commands;
	cJSON *j_response_tree;

	නම්(!event_data || !cmd_cb){
		ආපසු MOSQ_ERR_INVAL;
	}

	memset(&cmd, 0, parimaa_na(cmd));
	cmd.command_name = "Unknown command";
	cmd.client = ed->client;

	/* Create object for responses */
	j_response_tree = cJSON_CreateObject();
	නම්(j_response_tree == NULL){
		ආපසු MOSQ_ERR_NOMEM;
	}
	cmd.j_responses = cJSON_AddArrayToObject(j_response_tree, "responses");
	නම්(cmd.j_responses == NULL){
		cJSON_Delete(j_response_tree);
		ආපසු MOSQ_ERR_NOMEM;
	}

	/* Parse cJSON tree.
	 * Using cJSON_ParseWithLength() is the best choice here, but Mosquitto
	 * always adds an extra 0 to the end of the payload memory, so using
	 * cJSON_Parse() on its own will still not overrun. */
#නම් CJSON_VERSION_FULL < 1007013
	tree = cJSON_Parse(ed->payload);
#නැතහොත්
	tree = cJSON_ParseWithLength(ed->payload, ed->payloadlen);
#endif
	නම්(tree == NULL){
		mosquitto_control_command_reply(&cmd, "Payload not valid JSON");
		mosquitto_control_send_response(j_response_tree, response_topic);
		ආපසු MOSQ_ERR_SUCCESS;
	}
	commands = cJSON_GetObjectItem(tree, "commands");
	නම්(commands == NULL || !cJSON_IsArray(commands)){
		cJSON_Delete(tree);
		mosquitto_control_command_reply(&cmd, "Invalid/missing commands");
		mosquitto_control_send_response(j_response_tree, response_topic);
		ආපසු MOSQ_ERR_SUCCESS;
	}

	/* Handle commands */
	control__generic_handle_commands(&cmd, commands, userdata, cmd_cb);
	cJSON_Delete(tree);

	mosquitto_control_send_response(j_response_tree, response_topic);

	ආපසු MOSQ_ERR_SUCCESS;
}
