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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ "mosquitto.h"
#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "utlist.h"

#ifdef WITH_BRIDGE


நிலையான முழு bridge__create_remap_topic(மாறிலி எழுத்து *prefix, மாறிலி எழுத்து *topic, எழுத்து **remap_topic)
{
	எனில்(prefix){
		எனில்(topic){
			size_t len = strlen(topic) + strlen(prefix)+1;
			*remap_topic = mosquitto_malloc(len+1);
			எனில்(!(*remap_topic)){
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
				திரும்பு MOSQ_ERR_NOMEM;
			}
			snprintf(*remap_topic, len+1, "%s%s", prefix, topic);
			(*remap_topic)[len] = '\0';
		}இல்லையெனில்{
			*remap_topic = mosquitto_strdup(prefix);
			எனில்(!(*remap_topic)){
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
				திரும்பு MOSQ_ERR_NOMEM;
			}
		}
	}இல்லையெனில்{
		*remap_topic = mosquitto_strdup(topic);
		எனில்(!(*remap_topic)){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
			திரும்பு MOSQ_ERR_NOMEM;
		}
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு bridge__create_prefix(எழுத்து **full_prefix, மாறிலி எழுத்து *topic, மாறிலி எழுத்து *prefix, மாறிலி எழுத்து *direction)
{
	size_t len;

	எனில்(!prefix || strlen(prefix) != 0){
		எனில்(mosquitto_pub_topic_check(prefix) != MOSQ_ERR_SUCCESS){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", prefix);
			திரும்பு MOSQ_ERR_INVAL;
		}
	}

	எனில்(topic){
		len = strlen(topic) + strlen(prefix) + 1;
	}இல்லையெனில்{
		len = strlen(prefix) + 1;
	}
	*full_prefix = mosquitto_malloc(len);
	எனில்(*full_prefix == வெற்று){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
		திரும்பு MOSQ_ERR_NOMEM;
	}

	எனில்(topic){
		/* Print full_prefix+pattern to check for validity */
		snprintf(*full_prefix, len, "%s%s", prefix, topic);
	}இல்லையெனில்{
		snprintf(*full_prefix, len, "%s", prefix);
	}

	எனில்(mosquitto_sub_topic_check(*full_prefix) != MOSQ_ERR_SUCCESS){
		log__printf(வெற்று, MOSQ_LOG_ERR,
				"Error: Invalid bridge topic %s prefix and pattern combination '%s'.",
				direction, *full_prefix);

		திரும்பு MOSQ_ERR_INVAL;
	}

	/* Print just the prefix for storage */
	snprintf(*full_prefix, len, "%s", prefix);

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான கட்டமைப்பு mosquitto__bridge_topic *bridge__find_topic(கட்டமைப்பு mosquitto__bridge *bridge, மாறிலி எழுத்து *topic, எண்ணிக்கை mosquitto__bridge_direction direction, uint8_t qos, மாறிலி எழுத்து *local_prefix, மாறிலி எழுத்து *remote_prefix)
{
	கட்டமைப்பு mosquitto__bridge_topic *cur_topic = வெற்று;
	bool found = false;

	LL_FOREACH(bridge->topics, cur_topic){
		எனில்(cur_topic->direction != direction){
			தொடர்;
		}
		எனில்(cur_topic->qos != qos){
			தொடர்;
		}
		எனில்(cur_topic->topic != வெற்று && topic != வெற்று){
			எனில்(strcmp(cur_topic->topic, topic)){
				தொடர்;
			}
		}
		எனில்(cur_topic->local_prefix != வெற்று && local_prefix != வெற்று){
			எனில்(strcmp(cur_topic->local_prefix, local_prefix)){
				தொடர்;
			}
		}
		எனில்(cur_topic->remote_prefix != வெற்று && remote_prefix != வெற்று){
			எனில்(strcmp(cur_topic->remote_prefix, remote_prefix)){
				தொடர்;
			}
		}
		found = true;
		நிறுத்து;
	}
	எனில்(!found){
		cur_topic = வெற்று;
	}

	திரும்பு cur_topic;
}


வெறுமை bridge__cleanup_topics(கட்டமைப்பு mosquitto__bridge *bridge)
{
	கட்டமைப்பு mosquitto__bridge_topic *topic, *topic_tmp;

	எனில்(!bridge){
		திரும்பு;
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
முழு bridge__add_topic(கட்டமைப்பு mosquitto__bridge *bridge, மாறிலி எழுத்து *topic, எண்ணிக்கை mosquitto__bridge_direction direction, uint8_t qos, மாறிலி எழுத்து *local_prefix, மாறிலி எழுத்து *remote_prefix)
{
	கட்டமைப்பு mosquitto__bridge_topic *cur_topic;

	எனில்(bridge == வெற்று){
		திரும்பு MOSQ_ERR_INVAL;
	}
	எனில்(direction != bd_out && direction != bd_in && direction != bd_both){
		திரும்பு MOSQ_ERR_INVAL;
	}
	எனில்(qos > 2){
		திரும்பு MOSQ_ERR_INVAL;
	}
	எனில்(local_prefix && mosquitto_pub_topic_check(local_prefix)){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", local_prefix);
		திரும்பு MOSQ_ERR_INVAL;
	}
	எனில்(remote_prefix && mosquitto_pub_topic_check(remote_prefix)){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid bridge topic remote prefix '%s'.", remote_prefix);
		திரும்பு MOSQ_ERR_INVAL;
	}
	எனில்((topic == வெற்று || !strcmp(topic, "\"\"")) &&
			(local_prefix == வெற்று || remote_prefix == வெற்று)){

		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid bridge remapping.");
		திரும்பு MOSQ_ERR_INVAL;
	}

	எனில்(bridge__find_topic(bridge, topic, direction, qos, local_prefix, remote_prefix) != வெற்று){
		log__printf(வெற்று, MOSQ_LOG_INFO, "Duplicate bridge topic '%s', skipping", topic);
		திரும்பு MOSQ_ERR_SUCCESS;
	}

	bridge->topic_மொத்தம்++;
	cur_topic = mosquitto_calloc(1, அளவு(கட்டமைப்பு mosquitto__bridge_topic));
	எனில்(cur_topic == வெற்று){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
		திரும்பு MOSQ_ERR_NOMEM;
	}
	cur_topic->next = வெற்று;

	cur_topic->direction = direction;
	cur_topic->qos = qos;
	cur_topic->local_prefix = வெற்று;
	cur_topic->remote_prefix = வெற்று;

	எனில்(topic == வெற்று || !strcmp(topic, "\"\"")){
		cur_topic->topic = வெற்று;
	}இல்லையெனில்{
		cur_topic->topic = mosquitto_strdup(topic);
		எனில்(cur_topic->topic == வெற்று){
			செல் error;
		}
	}

	எனில்(local_prefix || remote_prefix){
		bridge->topic_remapping = true;
		எனில்(local_prefix){
			எனில்(bridge__create_prefix(&cur_topic->local_prefix, cur_topic->topic, local_prefix, "local")){
				செல் error;
			}
		}
		எனில்(remote_prefix){
			எனில்(bridge__create_prefix(&cur_topic->remote_prefix, cur_topic->topic, remote_prefix, "local")){
				செல் error;
			}
		}
	}

	எனில்(bridge__create_remap_topic(cur_topic->local_prefix,
			cur_topic->topic, &cur_topic->local_topic)){

		செல் error;
	}

	எனில்(bridge__create_remap_topic(cur_topic->remote_prefix,
			cur_topic->topic, &cur_topic->remote_topic)){

		செல் error;
	}

	LL_APPEND(bridge->topics, cur_topic);

	திரும்பு MOSQ_ERR_SUCCESS;

error:
	mosquitto_FREE(cur_topic->local_prefix);
	mosquitto_FREE(cur_topic->remote_prefix);
	mosquitto_FREE(cur_topic->local_topic);
	mosquitto_FREE(cur_topic->remote_topic);
	mosquitto_FREE(cur_topic->topic);
	mosquitto_FREE(cur_topic);
	log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
	திரும்பு MOSQ_ERR_NOMEM;
}


முழு bridge__remap_topic_in(கட்டமைப்பு mosquitto *context, எழுத்து **topic)
{
	கட்டமைப்பு mosquitto__bridge_topic *cur_topic;
	bool match;

	எனில்(context->bridge && context->bridge->topics && context->bridge->topic_remapping){
		LL_FOREACH(context->bridge->topics, cur_topic){
			எனில்((cur_topic->direction == bd_both || cur_topic->direction == bd_in)
					&& (cur_topic->remote_prefix || cur_topic->local_prefix)){

				/* Topic mapping required on this topic if the message matches */

				முழு rc = mosquitto_topic_matches_sub(cur_topic->remote_topic, *topic, &match);
				எனில்(rc){
					mosquitto_FREE(*topic);
					திரும்பு rc;
				}
				எனில்(match){
					எழுத்து *topic_temp;

					எனில்(cur_topic->remote_prefix){
						/* This prefix needs removing. */
						எனில்(!strncmp(cur_topic->remote_prefix, *topic, strlen(cur_topic->remote_prefix))){
							topic_temp = mosquitto_strdup((*topic)+strlen(cur_topic->remote_prefix));
							எனில்(!topic_temp){
								mosquitto_FREE(*topic);
								திரும்பு MOSQ_ERR_NOMEM;
							}
							mosquitto_FREE(*topic);
							*topic = topic_temp;
						}
					}

					எனில்(cur_topic->local_prefix){
						/* This prefix needs adding. */
						size_t len = strlen(*topic) + strlen(cur_topic->local_prefix)+1;
						topic_temp = mosquitto_malloc(len+1);
						எனில்(!topic_temp){
							mosquitto_FREE(*topic);
							திரும்பு MOSQ_ERR_NOMEM;
						}
						snprintf(topic_temp, len, "%s%s", cur_topic->local_prefix, *topic);
						topic_temp[len] = '\0';

						mosquitto_FREE(*topic);
						*topic = topic_temp;
					}
					நிறுத்து;
				}
			}
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}

#endif
