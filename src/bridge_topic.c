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


static 정수 bridge__create_remap_topic(const char *prefix, const char *topic, char **remap_topic)
{
	만약(prefix){
		만약(topic){
			size_t len = strlen(topic) + strlen(prefix)+1;
			*remap_topic = mosquitto_malloc(len+1);
			만약(!(*remap_topic)){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
				반환 MOSQ_ERR_NOMEM;
			}
			snprintf(*remap_topic, len+1, "%s%s", prefix, topic);
			(*remap_topic)[len] = '\0';
		}아니면{
			*remap_topic = mosquitto_strdup(prefix);
			만약(!(*remap_topic)){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
				반환 MOSQ_ERR_NOMEM;
			}
		}
	}아니면{
		*remap_topic = mosquitto_strdup(topic);
		만약(!(*remap_topic)){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
			반환 MOSQ_ERR_NOMEM;
		}
	}
	반환 MOSQ_ERR_SUCCESS;
}


static 정수 bridge__create_prefix(char **full_prefix, const char *topic, const char *prefix, const char *direction)
{
	size_t len;

	만약(!prefix || strlen(prefix) != 0){
		만약(mosquitto_pub_topic_check(prefix) != MOSQ_ERR_SUCCESS){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", prefix);
			반환 MOSQ_ERR_INVAL;
		}
	}

	만약(topic){
		len = strlen(topic) + strlen(prefix) + 1;
	}아니면{
		len = strlen(prefix) + 1;
	}
	*full_prefix = mosquitto_malloc(len);
	만약(*full_prefix == NULL){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		반환 MOSQ_ERR_NOMEM;
	}

	만약(topic){
		/* Print full_prefix+pattern to check for validity */
		snprintf(*full_prefix, len, "%s%s", prefix, topic);
	}아니면{
		snprintf(*full_prefix, len, "%s", prefix);
	}

	만약(mosquitto_sub_topic_check(*full_prefix) != MOSQ_ERR_SUCCESS){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Invalid bridge topic %s prefix and pattern combination '%s'.",
				direction, *full_prefix);

		반환 MOSQ_ERR_INVAL;
	}

	/* Print just the prefix for storage */
	snprintf(*full_prefix, len, "%s", prefix);

	반환 MOSQ_ERR_SUCCESS;
}


static struct mosquitto__bridge_topic *bridge__find_topic(struct mosquitto__bridge *bridge, const char *topic, enum mosquitto__bridge_direction direction, uint8_t qos, const char *local_prefix, const char *remote_prefix)
{
	struct mosquitto__bridge_topic *cur_topic = NULL;
	bool found = false;

	LL_FOREACH(bridge->topics, cur_topic){
		만약(cur_topic->direction != direction){
			continue;
		}
		만약(cur_topic->qos != qos){
			continue;
		}
		만약(cur_topic->topic != NULL && topic != NULL){
			만약(strcmp(cur_topic->topic, topic)){
				continue;
			}
		}
		만약(cur_topic->local_prefix != NULL && local_prefix != NULL){
			만약(strcmp(cur_topic->local_prefix, local_prefix)){
				continue;
			}
		}
		만약(cur_topic->remote_prefix != NULL && remote_prefix != NULL){
			만약(strcmp(cur_topic->remote_prefix, remote_prefix)){
				continue;
			}
		}
		found = true;
		break;
	}
	만약(!found){
		cur_topic = NULL;
	}

	반환 cur_topic;
}


void bridge__cleanup_topics(struct mosquitto__bridge *bridge)
{
	struct mosquitto__bridge_topic *topic, *topic_tmp;

	만약(!bridge){
		반환;
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
정수 bridge__add_topic(struct mosquitto__bridge *bridge, const char *topic, enum mosquitto__bridge_direction direction, uint8_t qos, const char *local_prefix, const char *remote_prefix)
{
	struct mosquitto__bridge_topic *cur_topic;

	만약(bridge == NULL){
		반환 MOSQ_ERR_INVAL;
	}
	만약(direction != bd_out && direction != bd_in && direction != bd_both){
		반환 MOSQ_ERR_INVAL;
	}
	만약(qos > 2){
		반환 MOSQ_ERR_INVAL;
	}
	만약(local_prefix && mosquitto_pub_topic_check(local_prefix)){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", local_prefix);
		반환 MOSQ_ERR_INVAL;
	}
	만약(remote_prefix && mosquitto_pub_topic_check(remote_prefix)){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge topic remote prefix '%s'.", remote_prefix);
		반환 MOSQ_ERR_INVAL;
	}
	만약((topic == NULL || !strcmp(topic, "\"\"")) &&
			(local_prefix == NULL || remote_prefix == NULL)){

		log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge remapping.");
		반환 MOSQ_ERR_INVAL;
	}

	만약(bridge__find_topic(bridge, topic, direction, qos, local_prefix, remote_prefix) != NULL){
		log__printf(NULL, MOSQ_LOG_INFO, "Duplicate bridge topic '%s', skipping", topic);
		반환 MOSQ_ERR_SUCCESS;
	}

	bridge->topic_count++;
	cur_topic = mosquitto_calloc(1, sizeof(struct mosquitto__bridge_topic));
	만약(cur_topic == NULL){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		반환 MOSQ_ERR_NOMEM;
	}
	cur_topic->next = NULL;

	cur_topic->direction = direction;
	cur_topic->qos = qos;
	cur_topic->local_prefix = NULL;
	cur_topic->remote_prefix = NULL;

	만약(topic == NULL || !strcmp(topic, "\"\"")){
		cur_topic->topic = NULL;
	}아니면{
		cur_topic->topic = mosquitto_strdup(topic);
		만약(cur_topic->topic == NULL){
			goto error;
		}
	}

	만약(local_prefix || remote_prefix){
		bridge->topic_remapping = true;
		만약(local_prefix){
			만약(bridge__create_prefix(&cur_topic->local_prefix, cur_topic->topic, local_prefix, "local")){
				goto error;
			}
		}
		만약(remote_prefix){
			만약(bridge__create_prefix(&cur_topic->remote_prefix, cur_topic->topic, remote_prefix, "local")){
				goto error;
			}
		}
	}

	만약(bridge__create_remap_topic(cur_topic->local_prefix,
			cur_topic->topic, &cur_topic->local_topic)){

		goto error;
	}

	만약(bridge__create_remap_topic(cur_topic->remote_prefix,
			cur_topic->topic, &cur_topic->remote_topic)){

		goto error;
	}

	LL_APPEND(bridge->topics, cur_topic);

	반환 MOSQ_ERR_SUCCESS;

error:
	mosquitto_FREE(cur_topic->local_prefix);
	mosquitto_FREE(cur_topic->remote_prefix);
	mosquitto_FREE(cur_topic->local_topic);
	mosquitto_FREE(cur_topic->remote_topic);
	mosquitto_FREE(cur_topic->topic);
	mosquitto_FREE(cur_topic);
	log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
	반환 MOSQ_ERR_NOMEM;
}


정수 bridge__remap_topic_in(struct mosquitto *context, char **topic)
{
	struct mosquitto__bridge_topic *cur_topic;
	bool match;

	만약(context->bridge && context->bridge->topics && context->bridge->topic_remapping){
		LL_FOREACH(context->bridge->topics, cur_topic){
			만약((cur_topic->direction == bd_both || cur_topic->direction == bd_in)
					&& (cur_topic->remote_prefix || cur_topic->local_prefix)){

				/* Topic mapping required on this topic if the message matches */

				정수 rc = mosquitto_topic_matches_sub(cur_topic->remote_topic, *topic, &match);
				만약(rc){
					mosquitto_FREE(*topic);
					반환 rc;
				}
				만약(match){
					char *topic_temp;

					만약(cur_topic->remote_prefix){
						/* This prefix needs removing. */
						만약(!strncmp(cur_topic->remote_prefix, *topic, strlen(cur_topic->remote_prefix))){
							topic_temp = mosquitto_strdup((*topic)+strlen(cur_topic->remote_prefix));
							만약(!topic_temp){
								mosquitto_FREE(*topic);
								반환 MOSQ_ERR_NOMEM;
							}
							mosquitto_FREE(*topic);
							*topic = topic_temp;
						}
					}

					만약(cur_topic->local_prefix){
						/* This prefix needs adding. */
						size_t len = strlen(*topic) + strlen(cur_topic->local_prefix)+1;
						topic_temp = mosquitto_malloc(len+1);
						만약(!topic_temp){
							mosquitto_FREE(*topic);
							반환 MOSQ_ERR_NOMEM;
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

	반환 MOSQ_ERR_SUCCESS;
}

#endif
