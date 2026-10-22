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

#شامل "config.h"

#شامل "mosquitto.h"
#شامل "mosquitto_broker_internal.h"
#شامل "utlist.h"

#ifdef WITH_BRIDGE


ایستا صحیح bridge__create_remap_topic(ثابت نویسه *prefix, ثابت نویسه *topic, نویسه **remap_topic)
{
	اگر(prefix){
		اگر(topic){
			size_t len = strlen(topic) + strlen(prefix)+1;
			*remap_topic = mosquitto_malloc(len+1);
			اگر(!(*remap_topic)){
				log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
				بازگشت MOSQ_ERR_NOMEM;
			}
			snprintf(*remap_topic, len+1, "%s%s", prefix, topic);
			(*remap_topic)[len] = '\0';
		}وگرنه{
			*remap_topic = mosquitto_strdup(prefix);
			اگر(!(*remap_topic)){
				log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
				بازگشت MOSQ_ERR_NOMEM;
			}
		}
	}وگرنه{
		*remap_topic = mosquitto_strdup(topic);
		اگر(!(*remap_topic)){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
			بازگشت MOSQ_ERR_NOMEM;
		}
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح bridge__create_prefix(نویسه **full_prefix, ثابت نویسه *topic, ثابت نویسه *prefix, ثابت نویسه *direction)
{
	size_t len;

	اگر(!prefix || strlen(prefix) != 0){
		اگر(mosquitto_pub_topic_check(prefix) != MOSQ_ERR_SUCCESS){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", prefix);
			بازگشت MOSQ_ERR_INVAL;
		}
	}

	اگر(topic){
		len = strlen(topic) + strlen(prefix) + 1;
	}وگرنه{
		len = strlen(prefix) + 1;
	}
	*full_prefix = mosquitto_malloc(len);
	اگر(*full_prefix == پوچ){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
		بازگشت MOSQ_ERR_NOMEM;
	}

	اگر(topic){
		/* Print full_prefix+pattern to check for validity */
		snprintf(*full_prefix, len, "%s%s", prefix, topic);
	}وگرنه{
		snprintf(*full_prefix, len, "%s", prefix);
	}

	اگر(mosquitto_sub_topic_check(*full_prefix) != MOSQ_ERR_SUCCESS){
		log__printf(پوچ, MOSQ_LOG_ERR,
				"Error: Invalid bridge topic %s prefix and pattern combination '%s'.",
				direction, *full_prefix);

		بازگشت MOSQ_ERR_INVAL;
	}

	/* Print just the prefix for storage */
	snprintf(*full_prefix, len, "%s", prefix);

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا ساختار mosquitto__bridge_topic *bridge__find_topic(ساختار mosquitto__bridge *bridge, ثابت نویسه *topic, شمارش mosquitto__bridge_direction direction, uint8_t qos, ثابت نویسه *local_prefix, ثابت نویسه *remote_prefix)
{
	ساختار mosquitto__bridge_topic *cur_topic = پوچ;
	bool found = false;

	LL_FOREACH(bridge->topics, cur_topic){
		اگر(cur_topic->direction != direction){
			ادامه;
		}
		اگر(cur_topic->qos != qos){
			ادامه;
		}
		اگر(cur_topic->topic != پوچ && topic != پوچ){
			اگر(strcmp(cur_topic->topic, topic)){
				ادامه;
			}
		}
		اگر(cur_topic->local_prefix != پوچ && local_prefix != پوچ){
			اگر(strcmp(cur_topic->local_prefix, local_prefix)){
				ادامه;
			}
		}
		اگر(cur_topic->remote_prefix != پوچ && remote_prefix != پوچ){
			اگر(strcmp(cur_topic->remote_prefix, remote_prefix)){
				ادامه;
			}
		}
		found = true;
		بشکن;
	}
	اگر(!found){
		cur_topic = پوچ;
	}

	بازگشت cur_topic;
}


تهی bridge__cleanup_topics(ساختار mosquitto__bridge *bridge)
{
	ساختار mosquitto__bridge_topic *topic, *topic_tmp;

	اگر(!bridge){
		بازگشت;
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
صحیح bridge__add_topic(ساختار mosquitto__bridge *bridge, ثابت نویسه *topic, شمارش mosquitto__bridge_direction direction, uint8_t qos, ثابت نویسه *local_prefix, ثابت نویسه *remote_prefix)
{
	ساختار mosquitto__bridge_topic *cur_topic;

	اگر(bridge == پوچ){
		بازگشت MOSQ_ERR_INVAL;
	}
	اگر(direction != bd_out && direction != bd_in && direction != bd_both){
		بازگشت MOSQ_ERR_INVAL;
	}
	اگر(qos > 2){
		بازگشت MOSQ_ERR_INVAL;
	}
	اگر(local_prefix && mosquitto_pub_topic_check(local_prefix)){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", local_prefix);
		بازگشت MOSQ_ERR_INVAL;
	}
	اگر(remote_prefix && mosquitto_pub_topic_check(remote_prefix)){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid bridge topic remote prefix '%s'.", remote_prefix);
		بازگشت MOSQ_ERR_INVAL;
	}
	اگر((topic == پوچ || !strcmp(topic, "\"\"")) &&
			(local_prefix == پوچ || remote_prefix == پوچ)){

		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid bridge remapping.");
		بازگشت MOSQ_ERR_INVAL;
	}

	اگر(bridge__find_topic(bridge, topic, direction, qos, local_prefix, remote_prefix) != پوچ){
		log__printf(پوچ, MOSQ_LOG_INFO, "Duplicate bridge topic '%s', skipping", topic);
		بازگشت MOSQ_ERR_SUCCESS;
	}

	bridge->topic_شمار++;
	cur_topic = mosquitto_calloc(1, اندازه(ساختار mosquitto__bridge_topic));
	اگر(cur_topic == پوچ){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
		بازگشت MOSQ_ERR_NOMEM;
	}
	cur_topic->next = پوچ;

	cur_topic->direction = direction;
	cur_topic->qos = qos;
	cur_topic->local_prefix = پوچ;
	cur_topic->remote_prefix = پوچ;

	اگر(topic == پوچ || !strcmp(topic, "\"\"")){
		cur_topic->topic = پوچ;
	}وگرنه{
		cur_topic->topic = mosquitto_strdup(topic);
		اگر(cur_topic->topic == پوچ){
			برو error;
		}
	}

	اگر(local_prefix || remote_prefix){
		bridge->topic_remapping = true;
		اگر(local_prefix){
			اگر(bridge__create_prefix(&cur_topic->local_prefix, cur_topic->topic, local_prefix, "local")){
				برو error;
			}
		}
		اگر(remote_prefix){
			اگر(bridge__create_prefix(&cur_topic->remote_prefix, cur_topic->topic, remote_prefix, "local")){
				برو error;
			}
		}
	}

	اگر(bridge__create_remap_topic(cur_topic->local_prefix,
			cur_topic->topic, &cur_topic->local_topic)){

		برو error;
	}

	اگر(bridge__create_remap_topic(cur_topic->remote_prefix,
			cur_topic->topic, &cur_topic->remote_topic)){

		برو error;
	}

	LL_APPEND(bridge->topics, cur_topic);

	بازگشت MOSQ_ERR_SUCCESS;

error:
	mosquitto_FREE(cur_topic->local_prefix);
	mosquitto_FREE(cur_topic->remote_prefix);
	mosquitto_FREE(cur_topic->local_topic);
	mosquitto_FREE(cur_topic->remote_topic);
	mosquitto_FREE(cur_topic->topic);
	mosquitto_FREE(cur_topic);
	log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
	بازگشت MOSQ_ERR_NOMEM;
}


صحیح bridge__remap_topic_in(ساختار mosquitto *context, نویسه **topic)
{
	ساختار mosquitto__bridge_topic *cur_topic;
	bool match;

	اگر(context->bridge && context->bridge->topics && context->bridge->topic_remapping){
		LL_FOREACH(context->bridge->topics, cur_topic){
			اگر((cur_topic->direction == bd_both || cur_topic->direction == bd_in)
					&& (cur_topic->remote_prefix || cur_topic->local_prefix)){

				/* Topic mapping required on this topic if the message matches */

				صحیح rc = mosquitto_topic_matches_sub(cur_topic->remote_topic, *topic, &match);
				اگر(rc){
					mosquitto_FREE(*topic);
					بازگشت rc;
				}
				اگر(match){
					نویسه *topic_temp;

					اگر(cur_topic->remote_prefix){
						/* This prefix needs removing. */
						اگر(!strncmp(cur_topic->remote_prefix, *topic, strlen(cur_topic->remote_prefix))){
							topic_temp = mosquitto_strdup((*topic)+strlen(cur_topic->remote_prefix));
							اگر(!topic_temp){
								mosquitto_FREE(*topic);
								بازگشت MOSQ_ERR_NOMEM;
							}
							mosquitto_FREE(*topic);
							*topic = topic_temp;
						}
					}

					اگر(cur_topic->local_prefix){
						/* This prefix needs adding. */
						size_t len = strlen(*topic) + strlen(cur_topic->local_prefix)+1;
						topic_temp = mosquitto_malloc(len+1);
						اگر(!topic_temp){
							mosquitto_FREE(*topic);
							بازگشت MOSQ_ERR_NOMEM;
						}
						snprintf(topic_temp, len, "%s%s", cur_topic->local_prefix, *topic);
						topic_temp[len] = '\0';

						mosquitto_FREE(*topic);
						*topic = topic_temp;
					}
					بشکن;
				}
			}
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
}

#endif
