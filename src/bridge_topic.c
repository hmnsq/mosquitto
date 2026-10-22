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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ "mosquitto.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "utlist.h"

#ifdef WITH_BRIDGE


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା bridge__create_remap_topic(ଅଚଳ ଅକ୍ଷର *prefix, ଅଚଳ ଅକ୍ଷର *topic, ଅକ୍ଷର **remap_topic)
{
	ଯଦି(prefix){
		ଯଦି(topic){
			size_t len = strlen(topic) + strlen(prefix)+1;
			*remap_topic = mosquitto_malloc(len+1);
			ଯଦି(!(*remap_topic)){
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
				ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
			}
			snprintf(*remap_topic, len+1, "%s%s", prefix, topic);
			(*remap_topic)[len] = '\0';
		}ଅନ୍ୟଥା{
			*remap_topic = mosquitto_strdup(prefix);
			ଯଦି(!(*remap_topic)){
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
				ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
			}
		}
	}ଅନ୍ୟଥା{
		*remap_topic = mosquitto_strdup(topic);
		ଯଦି(!(*remap_topic)){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା bridge__create_prefix(ଅକ୍ଷର **full_prefix, ଅଚଳ ଅକ୍ଷର *topic, ଅଚଳ ଅକ୍ଷର *prefix, ଅଚଳ ଅକ୍ଷର *direction)
{
	size_t len;

	ଯଦି(!prefix || strlen(prefix) != 0){
		ଯଦି(mosquitto_pub_topic_check(prefix) != MOSQ_ERR_SUCCESS){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", prefix);
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
	}

	ଯଦି(topic){
		len = strlen(topic) + strlen(prefix) + 1;
	}ଅନ୍ୟଥା{
		len = strlen(prefix) + 1;
	}
	*full_prefix = mosquitto_malloc(len);
	ଯଦି(*full_prefix == ଖାଲି){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	ଯଦି(topic){
		/* Print full_prefix+pattern to check for validity */
		snprintf(*full_prefix, len, "%s%s", prefix, topic);
	}ଅନ୍ୟଥା{
		snprintf(*full_prefix, len, "%s", prefix);
	}

	ଯଦି(mosquitto_sub_topic_check(*full_prefix) != MOSQ_ERR_SUCCESS){
		log__printf(ଖାଲି, MOSQ_LOG_ERR,
				"Error: Invalid bridge topic %s prefix and pattern combination '%s'.",
				direction, *full_prefix);

		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	/* Print just the prefix for storage */
	snprintf(*full_prefix, len, "%s", prefix);

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ସଂରଚନା mosquitto__bridge_topic *bridge__find_topic(ସଂରଚନା mosquitto__bridge *bridge, ଅଚଳ ଅକ୍ଷର *topic, ଗଣନା mosquitto__bridge_direction direction, uint8_t qos, ଅଚଳ ଅକ୍ଷର *local_prefix, ଅଚଳ ଅକ୍ଷର *remote_prefix)
{
	ସଂରଚନା mosquitto__bridge_topic *cur_topic = ଖାଲି;
	bool found = false;

	LL_FOREACH(bridge->topics, cur_topic){
		ଯଦି(cur_topic->direction != direction){
			ଜାରି;
		}
		ଯଦି(cur_topic->qos != qos){
			ଜାରି;
		}
		ଯଦି(cur_topic->topic != ଖାଲି && topic != ଖାଲି){
			ଯଦି(strcmp(cur_topic->topic, topic)){
				ଜାରି;
			}
		}
		ଯଦି(cur_topic->local_prefix != ଖାଲି && local_prefix != ଖାଲି){
			ଯଦି(strcmp(cur_topic->local_prefix, local_prefix)){
				ଜାରି;
			}
		}
		ଯଦି(cur_topic->remote_prefix != ଖାଲି && remote_prefix != ଖାଲି){
			ଯଦି(strcmp(cur_topic->remote_prefix, remote_prefix)){
				ଜାରି;
			}
		}
		found = true;
		ଭାଙ୍ଗ;
	}
	ଯଦି(!found){
		cur_topic = ଖାଲି;
	}

	ଫେରନ୍ତୁ cur_topic;
}


ଶୂନ୍ୟ bridge__cleanup_topics(ସଂରଚନା mosquitto__bridge *bridge)
{
	ସଂରଚନା mosquitto__bridge_topic *topic, *topic_tmp;

	ଯଦି(!bridge){
		ଫେରନ୍ତୁ;
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
ପୂର୍ଣ୍ଣସଂଖ୍ୟା bridge__add_topic(ସଂରଚନା mosquitto__bridge *bridge, ଅଚଳ ଅକ୍ଷର *topic, ଗଣନା mosquitto__bridge_direction direction, uint8_t qos, ଅଚଳ ଅକ୍ଷର *local_prefix, ଅଚଳ ଅକ୍ଷର *remote_prefix)
{
	ସଂରଚନା mosquitto__bridge_topic *cur_topic;

	ଯଦି(bridge == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}
	ଯଦି(direction != bd_out && direction != bd_in && direction != bd_both){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}
	ଯଦି(qos > 2){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}
	ଯଦି(local_prefix && mosquitto_pub_topic_check(local_prefix)){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", local_prefix);
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}
	ଯଦି(remote_prefix && mosquitto_pub_topic_check(remote_prefix)){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid bridge topic remote prefix '%s'.", remote_prefix);
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}
	ଯଦି((topic == ଖାଲି || !strcmp(topic, "\"\"")) &&
			(local_prefix == ଖାଲି || remote_prefix == ଖାଲି)){

		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid bridge remapping.");
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	ଯଦି(bridge__find_topic(bridge, topic, direction, qos, local_prefix, remote_prefix) != ଖାଲି){
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "Duplicate bridge topic '%s', skipping", topic);
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

	bridge->topic_ଗଣନା++;
	cur_topic = mosquitto_calloc(1, ଆକାର(ସଂରଚନା mosquitto__bridge_topic));
	ଯଦି(cur_topic == ଖାଲି){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}
	cur_topic->next = ଖାଲି;

	cur_topic->direction = direction;
	cur_topic->qos = qos;
	cur_topic->local_prefix = ଖାଲି;
	cur_topic->remote_prefix = ଖାଲି;

	ଯଦି(topic == ଖାଲି || !strcmp(topic, "\"\"")){
		cur_topic->topic = ଖାଲି;
	}ଅନ୍ୟଥା{
		cur_topic->topic = mosquitto_strdup(topic);
		ଯଦି(cur_topic->topic == ଖାଲି){
			ଯାଅ error;
		}
	}

	ଯଦି(local_prefix || remote_prefix){
		bridge->topic_remapping = true;
		ଯଦି(local_prefix){
			ଯଦି(bridge__create_prefix(&cur_topic->local_prefix, cur_topic->topic, local_prefix, "local")){
				ଯାଅ error;
			}
		}
		ଯଦି(remote_prefix){
			ଯଦି(bridge__create_prefix(&cur_topic->remote_prefix, cur_topic->topic, remote_prefix, "local")){
				ଯାଅ error;
			}
		}
	}

	ଯଦି(bridge__create_remap_topic(cur_topic->local_prefix,
			cur_topic->topic, &cur_topic->local_topic)){

		ଯାଅ error;
	}

	ଯଦି(bridge__create_remap_topic(cur_topic->remote_prefix,
			cur_topic->topic, &cur_topic->remote_topic)){

		ଯାଅ error;
	}

	LL_APPEND(bridge->topics, cur_topic);

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;

error:
	mosquitto_FREE(cur_topic->local_prefix);
	mosquitto_FREE(cur_topic->remote_prefix);
	mosquitto_FREE(cur_topic->local_topic);
	mosquitto_FREE(cur_topic->remote_topic);
	mosquitto_FREE(cur_topic->topic);
	mosquitto_FREE(cur_topic);
	log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
	ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା bridge__remap_topic_in(ସଂରଚନା mosquitto *context, ଅକ୍ଷର **topic)
{
	ସଂରଚନା mosquitto__bridge_topic *cur_topic;
	bool match;

	ଯଦି(context->bridge && context->bridge->topics && context->bridge->topic_remapping){
		LL_FOREACH(context->bridge->topics, cur_topic){
			ଯଦି((cur_topic->direction == bd_both || cur_topic->direction == bd_in)
					&& (cur_topic->remote_prefix || cur_topic->local_prefix)){

				/* Topic mapping required on this topic if the message matches */

				ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = mosquitto_topic_matches_sub(cur_topic->remote_topic, *topic, &match);
				ଯଦି(rc){
					mosquitto_FREE(*topic);
					ଫେରନ୍ତୁ rc;
				}
				ଯଦି(match){
					ଅକ୍ଷର *topic_temp;

					ଯଦି(cur_topic->remote_prefix){
						/* This prefix needs removing. */
						ଯଦି(!strncmp(cur_topic->remote_prefix, *topic, strlen(cur_topic->remote_prefix))){
							topic_temp = mosquitto_strdup((*topic)+strlen(cur_topic->remote_prefix));
							ଯଦି(!topic_temp){
								mosquitto_FREE(*topic);
								ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
							}
							mosquitto_FREE(*topic);
							*topic = topic_temp;
						}
					}

					ଯଦି(cur_topic->local_prefix){
						/* This prefix needs adding. */
						size_t len = strlen(*topic) + strlen(cur_topic->local_prefix)+1;
						topic_temp = mosquitto_malloc(len+1);
						ଯଦି(!topic_temp){
							mosquitto_FREE(*topic);
							ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
						}
						snprintf(topic_temp, len, "%s%s", cur_topic->local_prefix, *topic);
						topic_temp[len] = '\0';

						mosquitto_FREE(*topic);
						*topic = topic_temp;
					}
					ଭାଙ୍ଗ;
				}
			}
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}

#endif
