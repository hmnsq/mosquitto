/*
Copyright (c) 2009-2021 Roger Light <roger@atchoo.org>

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

#include "config.h"

#include "mosquitto.h"
#include "mosquitto_broker_internal.h"
#include "utlist.h"

#ifdef WITH_BRIDGE


static ціле bridge__create_remap_topic(const char *prefix, const char *topic, char **remap_topic)
{
	якщо(prefix){
		якщо(topic){
			size_t len = strlen(topic) + strlen(prefix)+1;
			*remap_topic = mosquitto_malloc(len+1);
			якщо(!(*remap_topic)){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
				повернути MOSQ_ERR_NOMEM;
			}
			snprintf(*remap_topic, len+1, "%s%s", prefix, topic);
			(*remap_topic)[len] = '\0';
		}інакше{
			*remap_topic = mosquitto_strdup(prefix);
			якщо(!(*remap_topic)){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
				повернути MOSQ_ERR_NOMEM;
			}
		}
	}інакше{
		*remap_topic = mosquitto_strdup(topic);
		якщо(!(*remap_topic)){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
			повернути MOSQ_ERR_NOMEM;
		}
	}
	повернути MOSQ_ERR_SUCCESS;
}


static ціле bridge__create_prefix(char **full_prefix, const char *topic, const char *prefix, const char *direction)
{
	size_t len;

	якщо(!prefix || strlen(prefix) != 0){
		якщо(mosquitto_pub_topic_check(prefix) != MOSQ_ERR_SUCCESS){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", prefix);
			повернути MOSQ_ERR_INVAL;
		}
	}

	якщо(topic){
		len = strlen(topic) + strlen(prefix) + 1;
	}інакше{
		len = strlen(prefix) + 1;
	}
	*full_prefix = mosquitto_malloc(len);
	якщо(*full_prefix == NULL){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		повернути MOSQ_ERR_NOMEM;
	}

	якщо(topic){
		/* Print full_prefix+pattern to check for validity */
		snprintf(*full_prefix, len, "%s%s", prefix, topic);
	}інакше{
		snprintf(*full_prefix, len, "%s", prefix);
	}

	якщо(mosquitto_sub_topic_check(*full_prefix) != MOSQ_ERR_SUCCESS){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Invalid bridge topic %s prefix and pattern combination '%s'.",
				direction, *full_prefix);

		повернути MOSQ_ERR_INVAL;
	}

	/* Print just the prefix for storage */
	snprintf(*full_prefix, len, "%s", prefix);

	повернути MOSQ_ERR_SUCCESS;
}


static struct mosquitto__bridge_topic *bridge__find_topic(struct mosquitto__bridge *bridge, const char *topic, enum mosquitto__bridge_direction direction, uint8_t qos, const char *local_prefix, const char *remote_prefix)
{
	struct mosquitto__bridge_topic *cur_topic = NULL;
	bool found = false;

	LL_FOREACH(bridge->topics, cur_topic){
		якщо(cur_topic->direction != direction){
			continue;
		}
		якщо(cur_topic->qos != qos){
			continue;
		}
		якщо(cur_topic->topic != NULL && topic != NULL){
			якщо(strcmp(cur_topic->topic, topic)){
				continue;
			}
		}
		якщо(cur_topic->local_prefix != NULL && local_prefix != NULL){
			якщо(strcmp(cur_topic->local_prefix, local_prefix)){
				continue;
			}
		}
		якщо(cur_topic->remote_prefix != NULL && remote_prefix != NULL){
			якщо(strcmp(cur_topic->remote_prefix, remote_prefix)){
				continue;
			}
		}
		found = true;
		break;
	}
	якщо(!found){
		cur_topic = NULL;
	}

	повернути cur_topic;
}


void bridge__cleanup_topics(struct mosquitto__bridge *bridge)
{
	struct mosquitto__bridge_topic *topic, *topic_tmp;

	якщо(!bridge){
		повернути;
	}

	LL_FOREACH_SAFE(bridge->topics, topic, topic_tmp){
		LL_DELETE(bridge->topics, topic);
		mosquitto_free(topic->local_prefix);
		mosquitto_free(topic->remote_prefix);
		mosquitto_free(topic->local_topic);
		mosquitto_free(topic->remote_topic);
		mosquitto_free(topic->topic);
		mosquitto_free(topic);
	}
}


/* topic <topic> [[[out | in | both] qos-level] local-prefix remote-prefix] */
ціле bridge__add_topic(struct mosquitto__bridge *bridge, const char *topic, enum mosquitto__bridge_direction direction, uint8_t qos, const char *local_prefix, const char *remote_prefix)
{
	struct mosquitto__bridge_topic *cur_topic;

	якщо(bridge == NULL){
		повернути MOSQ_ERR_INVAL;
	}
	якщо(direction != bd_out && direction != bd_in && direction != bd_both){
		повернути MOSQ_ERR_INVAL;
	}
	якщо(qos > 2){
		повернути MOSQ_ERR_INVAL;
	}
	якщо(local_prefix && mosquitto_pub_topic_check(local_prefix)){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", local_prefix);
		повернути MOSQ_ERR_INVAL;
	}
	якщо(remote_prefix && mosquitto_pub_topic_check(remote_prefix)){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge topic remote prefix '%s'.", remote_prefix);
		повернути MOSQ_ERR_INVAL;
	}
	якщо((topic == NULL || !strcmp(topic, "\"\"")) &&
			(local_prefix == NULL || remote_prefix == NULL)){

		log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge remapping.");
		повернути MOSQ_ERR_INVAL;
	}

	якщо(bridge__find_topic(bridge, topic, direction, qos, local_prefix, remote_prefix) != NULL){
		log__printf(NULL, MOSQ_LOG_INFO, "Duplicate bridge topic '%s', skipping", topic);
		повернути MOSQ_ERR_SUCCESS;
	}

	bridge->topic_count++;
	cur_topic = mosquitto_calloc(1, sizeof(struct mosquitto__bridge_topic));
	якщо(cur_topic == NULL){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		повернути MOSQ_ERR_NOMEM;
	}
	cur_topic->next = NULL;

	cur_topic->direction = direction;
	cur_topic->qos = qos;
	cur_topic->local_prefix = NULL;
	cur_topic->remote_prefix = NULL;

	якщо(topic == NULL || !strcmp(topic, "\"\"")){
		cur_topic->topic = NULL;
	}інакше{
		cur_topic->topic = mosquitto_strdup(topic);
		якщо(cur_topic->topic == NULL){
			goto error;
		}
	}

	якщо(local_prefix || remote_prefix){
		bridge->topic_remapping = true;
		якщо(local_prefix){
			якщо(bridge__create_prefix(&cur_topic->local_prefix, cur_topic->topic, local_prefix, "local")){
				goto error;
			}
		}
		якщо(remote_prefix){
			якщо(bridge__create_prefix(&cur_topic->remote_prefix, cur_topic->topic, remote_prefix, "local")){
				goto error;
			}
		}
	}

	якщо(bridge__create_remap_topic(cur_topic->local_prefix,
			cur_topic->topic, &cur_topic->local_topic)){

		goto error;
	}

	якщо(bridge__create_remap_topic(cur_topic->remote_prefix,
			cur_topic->topic, &cur_topic->remote_topic)){

		goto error;
	}

	LL_APPEND(bridge->topics, cur_topic);

	повернути MOSQ_ERR_SUCCESS;

error:
	mosquitto_FREE(cur_topic->local_prefix);
	mosquitto_FREE(cur_topic->remote_prefix);
	mosquitto_FREE(cur_topic->local_topic);
	mosquitto_FREE(cur_topic->remote_topic);
	mosquitto_FREE(cur_topic->topic);
	mosquitto_FREE(cur_topic);
	log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
	повернути MOSQ_ERR_NOMEM;
}


ціле bridge__remap_topic_in(struct mosquitto *context, char **topic)
{
	struct mosquitto__bridge_topic *cur_topic;
	bool match;

	якщо(context->bridge && context->bridge->topics && context->bridge->topic_remapping){
		LL_FOREACH(context->bridge->topics, cur_topic){
			якщо((cur_topic->direction == bd_both || cur_topic->direction == bd_in)
					&& (cur_topic->remote_prefix || cur_topic->local_prefix)){

				/* Topic mapping required on this topic if the message matches */

				ціле rc = mosquitto_topic_matches_sub(cur_topic->remote_topic, *topic, &match);
				якщо(rc){
					mosquitto_FREE(*topic);
					повернути rc;
				}
				якщо(match){
					char *topic_temp;

					якщо(cur_topic->remote_prefix){
						/* This prefix needs removing. */
						якщо(!strncmp(cur_topic->remote_prefix, *topic, strlen(cur_topic->remote_prefix))){
							topic_temp = mosquitto_strdup((*topic)+strlen(cur_topic->remote_prefix));
							якщо(!topic_temp){
								mosquitto_FREE(*topic);
								повернути MOSQ_ERR_NOMEM;
							}
							mosquitto_FREE(*topic);
							*topic = topic_temp;
						}
					}

					якщо(cur_topic->local_prefix){
						/* This prefix needs adding. */
						size_t len = strlen(*topic) + strlen(cur_topic->local_prefix)+1;
						topic_temp = mosquitto_malloc(len+1);
						якщо(!topic_temp){
							mosquitto_FREE(*topic);
							повернути MOSQ_ERR_NOMEM;
						}
						snprintf(topic_temp, len, "%s%s", cur_topic->local_prefix, *topic);
						topic_temp[len] = '\0';

						mosquitto_FREE(*topic);
						*topic = topic_temp;
					}
					break;
				}
			}
		}
	}

	повернути MOSQ_ERR_SUCCESS;
}

#endif
