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

#ضمن "config.h"

#ضمن "mosquitto.h"
#ضمن "mosquitto_broker_internal.h"
#ضمن "utlist.h"

#ifdef WITH_BRIDGE


ساكن صحيح bridge__create_remap_topic(ثابت حرف *prefix, ثابت حرف *topic, حرف **remap_topic)
{
	ܐܢ(prefix){
		ܐܢ(topic){
			size_t len = strlen(topic) + strlen(prefix)+1;
			*remap_topic = mosquitto_malloc(len+1);
			ܐܢ(!(*remap_topic)){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
				ܦܢܐ MOSQ_ERR_NOMEM;
			}
			snprintf(*remap_topic, len+1, "%s%s", prefix, topic);
			(*remap_topic)[len] = '\0';
		}ܐܚܪܢܐ{
			*remap_topic = mosquitto_strdup(prefix);
			ܐܢ(!(*remap_topic)){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
				ܦܢܐ MOSQ_ERR_NOMEM;
			}
		}
	}ܐܚܪܢܐ{
		*remap_topic = mosquitto_strdup(topic);
		ܐܢ(!(*remap_topic)){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
			ܦܢܐ MOSQ_ERR_NOMEM;
		}
	}
	ܦܢܐ MOSQ_ERR_SUCCESS;
}


ساكن صحيح bridge__create_prefix(حرف **full_prefix, ثابت حرف *topic, ثابت حرف *prefix, ثابت حرف *direction)
{
	size_t len;

	ܐܢ(!prefix || strlen(prefix) != 0){
		ܐܢ(mosquitto_pub_topic_check(prefix) != MOSQ_ERR_SUCCESS){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", prefix);
			ܦܢܐ MOSQ_ERR_INVAL;
		}
	}

	ܐܢ(topic){
		len = strlen(topic) + strlen(prefix) + 1;
	}ܐܚܪܢܐ{
		len = strlen(prefix) + 1;
	}
	*full_prefix = mosquitto_malloc(len);
	ܐܢ(*full_prefix == عدم){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		ܦܢܐ MOSQ_ERR_NOMEM;
	}

	ܐܢ(topic){
		/* Print full_prefix+pattern to check for validity */
		snprintf(*full_prefix, len, "%s%s", prefix, topic);
	}ܐܚܪܢܐ{
		snprintf(*full_prefix, len, "%s", prefix);
	}

	ܐܢ(mosquitto_sub_topic_check(*full_prefix) != MOSQ_ERR_SUCCESS){
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Invalid bridge topic %s prefix and pattern combination '%s'.",
				direction, *full_prefix);

		ܦܢܐ MOSQ_ERR_INVAL;
	}

	/* Print just the prefix for storage */
	snprintf(*full_prefix, len, "%s", prefix);

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


ساكن بنية mosquitto__bridge_topic *bridge__find_topic(بنية mosquitto__bridge *bridge, ثابت حرف *topic, تعداد mosquitto__bridge_direction direction, uint8_t qos, ثابت حرف *local_prefix, ثابت حرف *remote_prefix)
{
	بنية mosquitto__bridge_topic *cur_topic = عدم;
	bool found = false;

	LL_FOREACH(bridge->topics, cur_topic){
		ܐܢ(cur_topic->direction != direction){
			استمر;
		}
		ܐܢ(cur_topic->qos != qos){
			استمر;
		}
		ܐܢ(cur_topic->topic != عدم && topic != عدم){
			ܐܢ(strcmp(cur_topic->topic, topic)){
				استمر;
			}
		}
		ܐܢ(cur_topic->local_prefix != عدم && local_prefix != عدم){
			ܐܢ(strcmp(cur_topic->local_prefix, local_prefix)){
				استمر;
			}
		}
		ܐܢ(cur_topic->remote_prefix != عدم && remote_prefix != عدم){
			ܐܢ(strcmp(cur_topic->remote_prefix, remote_prefix)){
				استمر;
			}
		}
		found = true;
		اكسر;
	}
	ܐܢ(!found){
		cur_topic = عدم;
	}

	ܦܢܐ cur_topic;
}


فراغ bridge__cleanup_topics(بنية mosquitto__bridge *bridge)
{
	بنية mosquitto__bridge_topic *topic, *topic_tmp;

	ܐܢ(!bridge){
		ܦܢܐ;
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
صحيح bridge__add_topic(بنية mosquitto__bridge *bridge, ثابت حرف *topic, تعداد mosquitto__bridge_direction direction, uint8_t qos, ثابت حرف *local_prefix, ثابت حرف *remote_prefix)
{
	بنية mosquitto__bridge_topic *cur_topic;

	ܐܢ(bridge == عدم){
		ܦܢܐ MOSQ_ERR_INVAL;
	}
	ܐܢ(direction != bd_out && direction != bd_in && direction != bd_both){
		ܦܢܐ MOSQ_ERR_INVAL;
	}
	ܐܢ(qos > 2){
		ܦܢܐ MOSQ_ERR_INVAL;
	}
	ܐܢ(local_prefix && mosquitto_pub_topic_check(local_prefix)){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", local_prefix);
		ܦܢܐ MOSQ_ERR_INVAL;
	}
	ܐܢ(remote_prefix && mosquitto_pub_topic_check(remote_prefix)){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge topic remote prefix '%s'.", remote_prefix);
		ܦܢܐ MOSQ_ERR_INVAL;
	}
	ܐܢ((topic == عدم || !strcmp(topic, "\"\"")) &&
			(local_prefix == عدم || remote_prefix == عدم)){

		log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge remapping.");
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	ܐܢ(bridge__find_topic(bridge, topic, direction, qos, local_prefix, remote_prefix) != عدم){
		log__printf(عدم, MOSQ_LOG_INFO, "Duplicate bridge topic '%s', skipping", topic);
		ܦܢܐ MOSQ_ERR_SUCCESS;
	}

	bridge->topic_count++;
	cur_topic = mosquitto_calloc(1, حجم(بنية mosquitto__bridge_topic));
	ܐܢ(cur_topic == عدم){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		ܦܢܐ MOSQ_ERR_NOMEM;
	}
	cur_topic->next = عدم;

	cur_topic->direction = direction;
	cur_topic->qos = qos;
	cur_topic->local_prefix = عدم;
	cur_topic->remote_prefix = عدم;

	ܐܢ(topic == عدم || !strcmp(topic, "\"\"")){
		cur_topic->topic = عدم;
	}ܐܚܪܢܐ{
		cur_topic->topic = mosquitto_strdup(topic);
		ܐܢ(cur_topic->topic == عدم){
			اذهب error;
		}
	}

	ܐܢ(local_prefix || remote_prefix){
		bridge->topic_remapping = true;
		ܐܢ(local_prefix){
			ܐܢ(bridge__create_prefix(&cur_topic->local_prefix, cur_topic->topic, local_prefix, "local")){
				اذهب error;
			}
		}
		ܐܢ(remote_prefix){
			ܐܢ(bridge__create_prefix(&cur_topic->remote_prefix, cur_topic->topic, remote_prefix, "local")){
				اذهب error;
			}
		}
	}

	ܐܢ(bridge__create_remap_topic(cur_topic->local_prefix,
			cur_topic->topic, &cur_topic->local_topic)){

		اذهب error;
	}

	ܐܢ(bridge__create_remap_topic(cur_topic->remote_prefix,
			cur_topic->topic, &cur_topic->remote_topic)){

		اذهب error;
	}

	LL_APPEND(bridge->topics, cur_topic);

	ܦܢܐ MOSQ_ERR_SUCCESS;

error:
	mosquitto_FREE(cur_topic->local_prefix);
	mosquitto_FREE(cur_topic->remote_prefix);
	mosquitto_FREE(cur_topic->local_topic);
	mosquitto_FREE(cur_topic->remote_topic);
	mosquitto_FREE(cur_topic->topic);
	mosquitto_FREE(cur_topic);
	log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
	ܦܢܐ MOSQ_ERR_NOMEM;
}


صحيح bridge__remap_topic_in(بنية mosquitto *context, حرف **topic)
{
	بنية mosquitto__bridge_topic *cur_topic;
	bool match;

	ܐܢ(context->bridge && context->bridge->topics && context->bridge->topic_remapping){
		LL_FOREACH(context->bridge->topics, cur_topic){
			ܐܢ((cur_topic->direction == bd_both || cur_topic->direction == bd_in)
					&& (cur_topic->remote_prefix || cur_topic->local_prefix)){

				/* Topic mapping required on this topic if the message matches */

				صحيح rc = mosquitto_topic_matches_sub(cur_topic->remote_topic, *topic, &match);
				ܐܢ(rc){
					mosquitto_FREE(*topic);
					ܦܢܐ rc;
				}
				ܐܢ(match){
					حرف *topic_temp;

					ܐܢ(cur_topic->remote_prefix){
						/* This prefix needs removing. */
						ܐܢ(!strncmp(cur_topic->remote_prefix, *topic, strlen(cur_topic->remote_prefix))){
							topic_temp = mosquitto_strdup((*topic)+strlen(cur_topic->remote_prefix));
							ܐܢ(!topic_temp){
								mosquitto_FREE(*topic);
								ܦܢܐ MOSQ_ERR_NOMEM;
							}
							mosquitto_FREE(*topic);
							*topic = topic_temp;
						}
					}

					ܐܢ(cur_topic->local_prefix){
						/* This prefix needs adding. */
						size_t len = strlen(*topic) + strlen(cur_topic->local_prefix)+1;
						topic_temp = mosquitto_malloc(len+1);
						ܐܢ(!topic_temp){
							mosquitto_FREE(*topic);
							ܦܢܐ MOSQ_ERR_NOMEM;
						}
						snprintf(topic_temp, len, "%s%s", cur_topic->local_prefix, *topic);
						topic_temp[len] = '\0';

						mosquitto_FREE(*topic);
						*topic = topic_temp;
					}
					اكسر;
				}
			}
		}
	}

	ܦܢܐ MOSQ_ERR_SUCCESS;
}

#endif
