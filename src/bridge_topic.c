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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ "mosquitto.h"
#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "utlist.h"

#ifdef WITH_BRIDGE


સ્થિર પૂર્ણાંક bridge__create_remap_topic(અચલ અક્ષર *prefix, અચલ અક્ષર *topic, અક્ષર **remap_topic)
{
	જો(prefix){
		જો(topic){
			size_t len = strlen(topic) + strlen(prefix)+1;
			*remap_topic = mosquitto_malloc(len+1);
			જો(!(*remap_topic)){
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
				પાછા MOSQ_ERR_NOMEM;
			}
			snprintf(*remap_topic, len+1, "%s%s", prefix, topic);
			(*remap_topic)[len] = '\0';
		}નહીંતર{
			*remap_topic = mosquitto_strdup(prefix);
			જો(!(*remap_topic)){
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
				પાછા MOSQ_ERR_NOMEM;
			}
		}
	}નહીંતર{
		*remap_topic = mosquitto_strdup(topic);
		જો(!(*remap_topic)){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
			પાછા MOSQ_ERR_NOMEM;
		}
	}
	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક bridge__create_prefix(અક્ષર **full_prefix, અચલ અક્ષર *topic, અચલ અક્ષર *prefix, અચલ અક્ષર *direction)
{
	size_t len;

	જો(!prefix || strlen(prefix) != 0){
		જો(mosquitto_pub_topic_check(prefix) != MOSQ_ERR_SUCCESS){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", prefix);
			પાછા MOSQ_ERR_INVAL;
		}
	}

	જો(topic){
		len = strlen(topic) + strlen(prefix) + 1;
	}નહીંતર{
		len = strlen(prefix) + 1;
	}
	*full_prefix = mosquitto_malloc(len);
	જો(*full_prefix == ખાલી){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
		પાછા MOSQ_ERR_NOMEM;
	}

	જો(topic){
		/* Print full_prefix+pattern to check for validity */
		snprintf(*full_prefix, len, "%s%s", prefix, topic);
	}નહીંતર{
		snprintf(*full_prefix, len, "%s", prefix);
	}

	જો(mosquitto_sub_topic_check(*full_prefix) != MOSQ_ERR_SUCCESS){
		log__printf(ખાલી, MOSQ_LOG_ERR,
				"Error: Invalid bridge topic %s prefix and pattern combination '%s'.",
				direction, *full_prefix);

		પાછા MOSQ_ERR_INVAL;
	}

	/* Print just the prefix for storage */
	snprintf(*full_prefix, len, "%s", prefix);

	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર સંરચના mosquitto__bridge_topic *bridge__find_topic(સંરચના mosquitto__bridge *bridge, અચલ અક્ષર *topic, ગણના mosquitto__bridge_direction direction, uint8_t qos, અચલ અક્ષર *local_prefix, અચલ અક્ષર *remote_prefix)
{
	સંરચના mosquitto__bridge_topic *cur_topic = ખાલી;
	bool found = false;

	LL_FOREACH(bridge->topics, cur_topic){
		જો(cur_topic->direction != direction){
			ચાલુ;
		}
		જો(cur_topic->qos != qos){
			ચાલુ;
		}
		જો(cur_topic->topic != ખાલી && topic != ખાલી){
			જો(strcmp(cur_topic->topic, topic)){
				ચાલુ;
			}
		}
		જો(cur_topic->local_prefix != ખાલી && local_prefix != ખાલી){
			જો(strcmp(cur_topic->local_prefix, local_prefix)){
				ચાલુ;
			}
		}
		જો(cur_topic->remote_prefix != ખાલી && remote_prefix != ખાલી){
			જો(strcmp(cur_topic->remote_prefix, remote_prefix)){
				ચાલુ;
			}
		}
		found = true;
		તોડો;
	}
	જો(!found){
		cur_topic = ખાલી;
	}

	પાછા cur_topic;
}


શૂન્ય bridge__cleanup_topics(સંરચના mosquitto__bridge *bridge)
{
	સંરચના mosquitto__bridge_topic *topic, *topic_tmp;

	જો(!bridge){
		પાછા;
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
પૂર્ણાંક bridge__add_topic(સંરચના mosquitto__bridge *bridge, અચલ અક્ષર *topic, ગણના mosquitto__bridge_direction direction, uint8_t qos, અચલ અક્ષર *local_prefix, અચલ અક્ષર *remote_prefix)
{
	સંરચના mosquitto__bridge_topic *cur_topic;

	જો(bridge == ખાલી){
		પાછા MOSQ_ERR_INVAL;
	}
	જો(direction != bd_out && direction != bd_in && direction != bd_both){
		પાછા MOSQ_ERR_INVAL;
	}
	જો(qos > 2){
		પાછા MOSQ_ERR_INVAL;
	}
	જો(local_prefix && mosquitto_pub_topic_check(local_prefix)){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", local_prefix);
		પાછા MOSQ_ERR_INVAL;
	}
	જો(remote_prefix && mosquitto_pub_topic_check(remote_prefix)){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid bridge topic remote prefix '%s'.", remote_prefix);
		પાછા MOSQ_ERR_INVAL;
	}
	જો((topic == ખાલી || !strcmp(topic, "\"\"")) &&
			(local_prefix == ખાલી || remote_prefix == ખાલી)){

		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid bridge remapping.");
		પાછા MOSQ_ERR_INVAL;
	}

	જો(bridge__find_topic(bridge, topic, direction, qos, local_prefix, remote_prefix) != ખાલી){
		log__printf(ખાલી, MOSQ_LOG_INFO, "Duplicate bridge topic '%s', skipping", topic);
		પાછા MOSQ_ERR_SUCCESS;
	}

	bridge->topic_ગણતરી++;
	cur_topic = mosquitto_calloc(1, કદ(સંરચના mosquitto__bridge_topic));
	જો(cur_topic == ખાલી){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
		પાછા MOSQ_ERR_NOMEM;
	}
	cur_topic->next = ખાલી;

	cur_topic->direction = direction;
	cur_topic->qos = qos;
	cur_topic->local_prefix = ખાલી;
	cur_topic->remote_prefix = ખાલી;

	જો(topic == ખાલી || !strcmp(topic, "\"\"")){
		cur_topic->topic = ખાલી;
	}નહીંતર{
		cur_topic->topic = mosquitto_strdup(topic);
		જો(cur_topic->topic == ખાલી){
			જાઓ error;
		}
	}

	જો(local_prefix || remote_prefix){
		bridge->topic_remapping = true;
		જો(local_prefix){
			જો(bridge__create_prefix(&cur_topic->local_prefix, cur_topic->topic, local_prefix, "local")){
				જાઓ error;
			}
		}
		જો(remote_prefix){
			જો(bridge__create_prefix(&cur_topic->remote_prefix, cur_topic->topic, remote_prefix, "local")){
				જાઓ error;
			}
		}
	}

	જો(bridge__create_remap_topic(cur_topic->local_prefix,
			cur_topic->topic, &cur_topic->local_topic)){

		જાઓ error;
	}

	જો(bridge__create_remap_topic(cur_topic->remote_prefix,
			cur_topic->topic, &cur_topic->remote_topic)){

		જાઓ error;
	}

	LL_APPEND(bridge->topics, cur_topic);

	પાછા MOSQ_ERR_SUCCESS;

error:
	mosquitto_FREE(cur_topic->local_prefix);
	mosquitto_FREE(cur_topic->remote_prefix);
	mosquitto_FREE(cur_topic->local_topic);
	mosquitto_FREE(cur_topic->remote_topic);
	mosquitto_FREE(cur_topic->topic);
	mosquitto_FREE(cur_topic);
	log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
	પાછા MOSQ_ERR_NOMEM;
}


પૂર્ણાંક bridge__remap_topic_in(સંરચના mosquitto *context, અક્ષર **topic)
{
	સંરચના mosquitto__bridge_topic *cur_topic;
	bool match;

	જો(context->bridge && context->bridge->topics && context->bridge->topic_remapping){
		LL_FOREACH(context->bridge->topics, cur_topic){
			જો((cur_topic->direction == bd_both || cur_topic->direction == bd_in)
					&& (cur_topic->remote_prefix || cur_topic->local_prefix)){

				/* Topic mapping required on this topic if the message matches */

				પૂર્ણાંક rc = mosquitto_topic_matches_sub(cur_topic->remote_topic, *topic, &match);
				જો(rc){
					mosquitto_FREE(*topic);
					પાછા rc;
				}
				જો(match){
					અક્ષર *topic_temp;

					જો(cur_topic->remote_prefix){
						/* This prefix needs removing. */
						જો(!strncmp(cur_topic->remote_prefix, *topic, strlen(cur_topic->remote_prefix))){
							topic_temp = mosquitto_strdup((*topic)+strlen(cur_topic->remote_prefix));
							જો(!topic_temp){
								mosquitto_FREE(*topic);
								પાછા MOSQ_ERR_NOMEM;
							}
							mosquitto_FREE(*topic);
							*topic = topic_temp;
						}
					}

					જો(cur_topic->local_prefix){
						/* This prefix needs adding. */
						size_t len = strlen(*topic) + strlen(cur_topic->local_prefix)+1;
						topic_temp = mosquitto_malloc(len+1);
						જો(!topic_temp){
							mosquitto_FREE(*topic);
							પાછા MOSQ_ERR_NOMEM;
						}
						snprintf(topic_temp, len, "%s%s", cur_topic->local_prefix, *topic);
						topic_temp[len] = '\0';

						mosquitto_FREE(*topic);
						*topic = topic_temp;
					}
					તોડો;
				}
			}
		}
	}

	પાછા MOSQ_ERR_SUCCESS;
}

#endif
