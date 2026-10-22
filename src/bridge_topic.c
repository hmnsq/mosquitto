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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ "mosquitto.h"
#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "utlist.h"

#ifdef WITH_BRIDGE


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ bridge__create_remap_topic(ಧ್ರುವ ವರ್ಣ *prefix, ಧ್ರುವ ವರ್ಣ *topic, ವರ್ಣ **remap_topic)
{
	ಯದಿ(prefix){
		ಯದಿ(topic){
			size_t len = strlen(topic) + strlen(prefix)+1;
			*remap_topic = mosquitto_malloc(len+1);
			ಯದಿ(!(*remap_topic)){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
			}
			snprintf(*remap_topic, len+1, "%s%s", prefix, topic);
			(*remap_topic)[len] = '\0';
		}ಅನ್ಯಥಾ{
			*remap_topic = mosquitto_strdup(prefix);
			ಯದಿ(!(*remap_topic)){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
			}
		}
	}ಅನ್ಯಥಾ{
		*remap_topic = mosquitto_strdup(topic);
		ಯದಿ(!(*remap_topic)){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ bridge__create_prefix(ವರ್ಣ **full_prefix, ಧ್ರುವ ವರ್ಣ *topic, ಧ್ರುವ ವರ್ಣ *prefix, ಧ್ರುವ ವರ್ಣ *direction)
{
	size_t len;

	ಯದಿ(!prefix || strlen(prefix) != 0){
		ಯದಿ(mosquitto_pub_topic_check(prefix) != MOSQ_ERR_SUCCESS){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", prefix);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
	}

	ಯದಿ(topic){
		len = strlen(topic) + strlen(prefix) + 1;
	}ಅನ್ಯಥಾ{
		len = strlen(prefix) + 1;
	}
	*full_prefix = mosquitto_malloc(len);
	ಯದಿ(*full_prefix == ರಿಕ್ತ){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}

	ಯದಿ(topic){
		/* Print full_prefix+pattern to check for validity */
		snprintf(*full_prefix, len, "%s%s", prefix, topic);
	}ಅನ್ಯಥಾ{
		snprintf(*full_prefix, len, "%s", prefix);
	}

	ಯದಿ(mosquitto_sub_topic_check(*full_prefix) != MOSQ_ERR_SUCCESS){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR,
				"Error: Invalid bridge topic %s prefix and pattern combination '%s'.",
				direction, *full_prefix);

		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	/* Print just the prefix for storage */
	snprintf(*full_prefix, len, "%s", prefix);

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಸಂರಚನಾ mosquitto__bridge_topic *bridge__find_topic(ಸಂರಚನಾ mosquitto__bridge *bridge, ಧ್ರುವ ವರ್ಣ *topic, ಗಣನಾ mosquitto__bridge_direction direction, uint8_t qos, ಧ್ರುವ ವರ್ಣ *local_prefix, ಧ್ರುವ ವರ್ಣ *remote_prefix)
{
	ಸಂರಚನಾ mosquitto__bridge_topic *cur_topic = ರಿಕ್ತ;
	bool found = false;

	LL_FOREACH(bridge->topics, cur_topic){
		ಯದಿ(cur_topic->direction != direction){
			ಅನುವರ್ತ;
		}
		ಯದಿ(cur_topic->qos != qos){
			ಅನುವರ್ತ;
		}
		ಯದಿ(cur_topic->topic != ರಿಕ್ತ && topic != ರಿಕ್ತ){
			ಯದಿ(strcmp(cur_topic->topic, topic)){
				ಅನುವರ್ತ;
			}
		}
		ಯದಿ(cur_topic->local_prefix != ರಿಕ್ತ && local_prefix != ರಿಕ್ತ){
			ಯದಿ(strcmp(cur_topic->local_prefix, local_prefix)){
				ಅನುವರ್ತ;
			}
		}
		ಯದಿ(cur_topic->remote_prefix != ರಿಕ್ತ && remote_prefix != ರಿಕ್ತ){
			ಯದಿ(strcmp(cur_topic->remote_prefix, remote_prefix)){
				ಅನುವರ್ತ;
			}
		}
		found = true;
		ಭಙ್ಗ;
	}
	ಯದಿ(!found){
		cur_topic = ರಿಕ್ತ;
	}

	ಪ್ರತ್ಯಾಗಮ cur_topic;
}


ಶೂನ್ಯ bridge__cleanup_topics(ಸಂರಚನಾ mosquitto__bridge *bridge)
{
	ಸಂರಚನಾ mosquitto__bridge_topic *topic, *topic_tmp;

	ಯದಿ(!bridge){
		ಪ್ರತ್ಯಾಗಮ;
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
ಪೂರ್ಣಾಙ್ಕ bridge__add_topic(ಸಂರಚನಾ mosquitto__bridge *bridge, ಧ್ರುವ ವರ್ಣ *topic, ಗಣನಾ mosquitto__bridge_direction direction, uint8_t qos, ಧ್ರುವ ವರ್ಣ *local_prefix, ಧ್ರುವ ವರ್ಣ *remote_prefix)
{
	ಸಂರಚನಾ mosquitto__bridge_topic *cur_topic;

	ಯದಿ(bridge == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}
	ಯದಿ(direction != bd_out && direction != bd_in && direction != bd_both){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}
	ಯದಿ(qos > 2){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}
	ಯದಿ(local_prefix && mosquitto_pub_topic_check(local_prefix)){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", local_prefix);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}
	ಯದಿ(remote_prefix && mosquitto_pub_topic_check(remote_prefix)){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid bridge topic remote prefix '%s'.", remote_prefix);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}
	ಯದಿ((topic == ರಿಕ್ತ || !strcmp(topic, "\"\"")) &&
			(local_prefix == ರಿಕ್ತ || remote_prefix == ರಿಕ್ತ)){

		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid bridge remapping.");
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	ಯದಿ(bridge__find_topic(bridge, topic, direction, qos, local_prefix, remote_prefix) != ರಿಕ್ತ){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Duplicate bridge topic '%s', skipping", topic);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}

	bridge->topic_ಎಣಿಕೆ++;
	cur_topic = mosquitto_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__bridge_topic));
	ಯದಿ(cur_topic == ರಿಕ್ತ){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}
	cur_topic->next = ರಿಕ್ತ;

	cur_topic->direction = direction;
	cur_topic->qos = qos;
	cur_topic->local_prefix = ರಿಕ್ತ;
	cur_topic->remote_prefix = ರಿಕ್ತ;

	ಯದಿ(topic == ರಿಕ್ತ || !strcmp(topic, "\"\"")){
		cur_topic->topic = ರಿಕ್ತ;
	}ಅನ್ಯಥಾ{
		cur_topic->topic = mosquitto_strdup(topic);
		ಯದಿ(cur_topic->topic == ರಿಕ್ತ){
			ಗಚ್ಛ error;
		}
	}

	ಯದಿ(local_prefix || remote_prefix){
		bridge->topic_remapping = true;
		ಯದಿ(local_prefix){
			ಯದಿ(bridge__create_prefix(&cur_topic->local_prefix, cur_topic->topic, local_prefix, "local")){
				ಗಚ್ಛ error;
			}
		}
		ಯದಿ(remote_prefix){
			ಯದಿ(bridge__create_prefix(&cur_topic->remote_prefix, cur_topic->topic, remote_prefix, "local")){
				ಗಚ್ಛ error;
			}
		}
	}

	ಯದಿ(bridge__create_remap_topic(cur_topic->local_prefix,
			cur_topic->topic, &cur_topic->local_topic)){

		ಗಚ್ಛ error;
	}

	ಯದಿ(bridge__create_remap_topic(cur_topic->remote_prefix,
			cur_topic->topic, &cur_topic->remote_topic)){

		ಗಚ್ಛ error;
	}

	LL_APPEND(bridge->topics, cur_topic);

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;

error:
	mosquitto_FREE(cur_topic->local_prefix);
	mosquitto_FREE(cur_topic->remote_prefix);
	mosquitto_FREE(cur_topic->local_topic);
	mosquitto_FREE(cur_topic->remote_topic);
	mosquitto_FREE(cur_topic->topic);
	mosquitto_FREE(cur_topic);
	log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
}


ಪೂರ್ಣಾಙ್ಕ bridge__remap_topic_in(ಸಂರಚನಾ mosquitto *context, ವರ್ಣ **topic)
{
	ಸಂರಚನಾ mosquitto__bridge_topic *cur_topic;
	bool match;

	ಯದಿ(context->bridge && context->bridge->topics && context->bridge->topic_remapping){
		LL_FOREACH(context->bridge->topics, cur_topic){
			ಯದಿ((cur_topic->direction == bd_both || cur_topic->direction == bd_in)
					&& (cur_topic->remote_prefix || cur_topic->local_prefix)){

				/* Topic mapping required on this topic if the message matches */

				ಪೂರ್ಣಾಙ್ಕ rc = mosquitto_topic_matches_sub(cur_topic->remote_topic, *topic, &match);
				ಯದಿ(rc){
					mosquitto_FREE(*topic);
					ಪ್ರತ್ಯಾಗಮ rc;
				}
				ಯದಿ(match){
					ವರ್ಣ *topic_temp;

					ಯದಿ(cur_topic->remote_prefix){
						/* This prefix needs removing. */
						ಯದಿ(!strncmp(cur_topic->remote_prefix, *topic, strlen(cur_topic->remote_prefix))){
							topic_temp = mosquitto_strdup((*topic)+strlen(cur_topic->remote_prefix));
							ಯದಿ(!topic_temp){
								mosquitto_FREE(*topic);
								ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
							}
							mosquitto_FREE(*topic);
							*topic = topic_temp;
						}
					}

					ಯದಿ(cur_topic->local_prefix){
						/* This prefix needs adding. */
						size_t len = strlen(*topic) + strlen(cur_topic->local_prefix)+1;
						topic_temp = mosquitto_malloc(len+1);
						ಯದಿ(!topic_temp){
							mosquitto_FREE(*topic);
							ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
						}
						snprintf(topic_temp, len, "%s%s", cur_topic->local_prefix, *topic);
						topic_temp[len] = '\0';

						mosquitto_FREE(*topic);
						*topic = topic_temp;
					}
					ಭಙ್ಗ;
				}
			}
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}

#endif
