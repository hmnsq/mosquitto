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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব "mosquitto.h"
#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "utlist.h"

#ifdef WITH_BRIDGE


স্থির পূর্ণসংখ্যা bridge__create_remap_topic(ধ্রুবক অক্ষর *prefix, ধ্রুবক অক্ষর *topic, অক্ষর **remap_topic)
{
	যদি(prefix){
		যদি(topic){
			size_t len = strlen(topic) + strlen(prefix)+1;
			*remap_topic = mosquitto_malloc(len+1);
			যদি(!(*remap_topic)){
				log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Out of memory.");
				ফেরত MOSQ_ERR_NOMEM;
			}
			snprintf(*remap_topic, len+1, "%s%s", prefix, topic);
			(*remap_topic)[len] = '\0';
		}অন্যথা{
			*remap_topic = mosquitto_strdup(prefix);
			যদি(!(*remap_topic)){
				log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Out of memory.");
				ফেরত MOSQ_ERR_NOMEM;
			}
		}
	}অন্যথা{
		*remap_topic = mosquitto_strdup(topic);
		যদি(!(*remap_topic)){
			log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Out of memory.");
			ফেরত MOSQ_ERR_NOMEM;
		}
	}
	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা bridge__create_prefix(অক্ষর **full_prefix, ধ্রুবক অক্ষর *topic, ধ্রুবক অক্ষর *prefix, ধ্রুবক অক্ষর *direction)
{
	size_t len;

	যদি(!prefix || strlen(prefix) != 0){
		যদি(mosquitto_pub_topic_check(prefix) != MOSQ_ERR_SUCCESS){
			log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", prefix);
			ফেরত MOSQ_ERR_INVAL;
		}
	}

	যদি(topic){
		len = strlen(topic) + strlen(prefix) + 1;
	}অন্যথা{
		len = strlen(prefix) + 1;
	}
	*full_prefix = mosquitto_malloc(len);
	যদি(*full_prefix == ফাঁকা){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Out of memory.");
		ফেরত MOSQ_ERR_NOMEM;
	}

	যদি(topic){
		/* Print full_prefix+pattern to check for validity */
		snprintf(*full_prefix, len, "%s%s", prefix, topic);
	}অন্যথা{
		snprintf(*full_prefix, len, "%s", prefix);
	}

	যদি(mosquitto_sub_topic_check(*full_prefix) != MOSQ_ERR_SUCCESS){
		log__printf(ফাঁকা, MOSQ_LOG_ERR,
				"Error: Invalid bridge topic %s prefix and pattern combination '%s'.",
				direction, *full_prefix);

		ফেরত MOSQ_ERR_INVAL;
	}

	/* Print just the prefix for storage */
	snprintf(*full_prefix, len, "%s", prefix);

	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির কাঠামো mosquitto__bridge_topic *bridge__find_topic(কাঠামো mosquitto__bridge *bridge, ধ্রুবক অক্ষর *topic, গণনা mosquitto__bridge_direction direction, uint8_t qos, ধ্রুবক অক্ষর *local_prefix, ধ্রুবক অক্ষর *remote_prefix)
{
	কাঠামো mosquitto__bridge_topic *cur_topic = ফাঁকা;
	bool found = false;

	LL_FOREACH(bridge->topics, cur_topic){
		যদি(cur_topic->direction != direction){
			চালু;
		}
		যদি(cur_topic->qos != qos){
			চালু;
		}
		যদি(cur_topic->topic != ফাঁকা && topic != ফাঁকা){
			যদি(strcmp(cur_topic->topic, topic)){
				চালু;
			}
		}
		যদি(cur_topic->local_prefix != ফাঁকা && local_prefix != ফাঁকা){
			যদি(strcmp(cur_topic->local_prefix, local_prefix)){
				চালু;
			}
		}
		যদি(cur_topic->remote_prefix != ফাঁকা && remote_prefix != ফাঁকা){
			যদি(strcmp(cur_topic->remote_prefix, remote_prefix)){
				চালু;
			}
		}
		found = true;
		বিরতি;
	}
	যদি(!found){
		cur_topic = ফাঁকা;
	}

	ফেরত cur_topic;
}


শূন্য bridge__cleanup_topics(কাঠামো mosquitto__bridge *bridge)
{
	কাঠামো mosquitto__bridge_topic *topic, *topic_tmp;

	যদি(!bridge){
		ফেরত;
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
পূর্ণসংখ্যা bridge__add_topic(কাঠামো mosquitto__bridge *bridge, ধ্রুবক অক্ষর *topic, গণনা mosquitto__bridge_direction direction, uint8_t qos, ধ্রুবক অক্ষর *local_prefix, ধ্রুবক অক্ষর *remote_prefix)
{
	কাঠামো mosquitto__bridge_topic *cur_topic;

	যদি(bridge == ফাঁকা){
		ফেরত MOSQ_ERR_INVAL;
	}
	যদি(direction != bd_out && direction != bd_in && direction != bd_both){
		ফেরত MOSQ_ERR_INVAL;
	}
	যদি(qos > 2){
		ফেরত MOSQ_ERR_INVAL;
	}
	যদি(local_prefix && mosquitto_pub_topic_check(local_prefix)){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", local_prefix);
		ফেরত MOSQ_ERR_INVAL;
	}
	যদি(remote_prefix && mosquitto_pub_topic_check(remote_prefix)){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Invalid bridge topic remote prefix '%s'.", remote_prefix);
		ফেরত MOSQ_ERR_INVAL;
	}
	যদি((topic == ফাঁকা || !strcmp(topic, "\"\"")) &&
			(local_prefix == ফাঁকা || remote_prefix == ফাঁকা)){

		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Invalid bridge remapping.");
		ফেরত MOSQ_ERR_INVAL;
	}

	যদি(bridge__find_topic(bridge, topic, direction, qos, local_prefix, remote_prefix) != ফাঁকা){
		log__printf(ফাঁকা, MOSQ_LOG_INFO, "Duplicate bridge topic '%s', skipping", topic);
		ফেরত MOSQ_ERR_SUCCESS;
	}

	bridge->topic_গুনতি++;
	cur_topic = mosquitto_calloc(1, আকার(কাঠামো mosquitto__bridge_topic));
	যদি(cur_topic == ফাঁকা){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Out of memory.");
		ফেরত MOSQ_ERR_NOMEM;
	}
	cur_topic->পরবর্তী = ফাঁকা;

	cur_topic->direction = direction;
	cur_topic->qos = qos;
	cur_topic->local_prefix = ফাঁকা;
	cur_topic->remote_prefix = ফাঁকা;

	যদি(topic == ফাঁকা || !strcmp(topic, "\"\"")){
		cur_topic->topic = ফাঁকা;
	}অন্যথা{
		cur_topic->topic = mosquitto_strdup(topic);
		যদি(cur_topic->topic == ফাঁকা){
			যাও error;
		}
	}

	যদি(local_prefix || remote_prefix){
		bridge->topic_remapping = true;
		যদি(local_prefix){
			যদি(bridge__create_prefix(&cur_topic->local_prefix, cur_topic->topic, local_prefix, "local")){
				যাও error;
			}
		}
		যদি(remote_prefix){
			যদি(bridge__create_prefix(&cur_topic->remote_prefix, cur_topic->topic, remote_prefix, "local")){
				যাও error;
			}
		}
	}

	যদি(bridge__create_remap_topic(cur_topic->local_prefix,
			cur_topic->topic, &cur_topic->local_topic)){

		যাও error;
	}

	যদি(bridge__create_remap_topic(cur_topic->remote_prefix,
			cur_topic->topic, &cur_topic->remote_topic)){

		যাও error;
	}

	LL_APPEND(bridge->topics, cur_topic);

	ফেরত MOSQ_ERR_SUCCESS;

error:
	mosquitto_FREE(cur_topic->local_prefix);
	mosquitto_FREE(cur_topic->remote_prefix);
	mosquitto_FREE(cur_topic->local_topic);
	mosquitto_FREE(cur_topic->remote_topic);
	mosquitto_FREE(cur_topic->topic);
	mosquitto_FREE(cur_topic);
	log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Out of memory.");
	ফেরত MOSQ_ERR_NOMEM;
}


পূর্ণসংখ্যা bridge__remap_topic_in(কাঠামো mosquitto *context, অক্ষর **topic)
{
	কাঠামো mosquitto__bridge_topic *cur_topic;
	bool match;

	যদি(context->bridge && context->bridge->topics && context->bridge->topic_remapping){
		LL_FOREACH(context->bridge->topics, cur_topic){
			যদি((cur_topic->direction == bd_both || cur_topic->direction == bd_in)
					&& (cur_topic->remote_prefix || cur_topic->local_prefix)){

				/* Topic mapping required on this topic if the message matches */

				পূর্ণসংখ্যা rc = mosquitto_topic_matches_sub(cur_topic->remote_topic, *topic, &match);
				যদি(rc){
					mosquitto_FREE(*topic);
					ফেরত rc;
				}
				যদি(match){
					অক্ষর *topic_অস্থায়ী;

					যদি(cur_topic->remote_prefix){
						/* This prefix needs removing. */
						যদি(!strncmp(cur_topic->remote_prefix, *topic, strlen(cur_topic->remote_prefix))){
							topic_অস্থায়ী = mosquitto_strdup((*topic)+strlen(cur_topic->remote_prefix));
							যদি(!topic_অস্থায়ী){
								mosquitto_FREE(*topic);
								ফেরত MOSQ_ERR_NOMEM;
							}
							mosquitto_FREE(*topic);
							*topic = topic_অস্থায়ী;
						}
					}

					যদি(cur_topic->local_prefix){
						/* This prefix needs adding. */
						size_t len = strlen(*topic) + strlen(cur_topic->local_prefix)+1;
						topic_অস্থায়ী = mosquitto_malloc(len+1);
						যদি(!topic_অস্থায়ী){
							mosquitto_FREE(*topic);
							ফেরত MOSQ_ERR_NOMEM;
						}
						snprintf(topic_অস্থায়ী, len, "%s%s", cur_topic->local_prefix, *topic);
						topic_অস্থায়ী[len] = '\0';

						mosquitto_FREE(*topic);
						*topic = topic_অস্থায়ী;
					}
					বিরতি;
				}
			}
		}
	}

	ফেরত MOSQ_ERR_SUCCESS;
}

#endif
