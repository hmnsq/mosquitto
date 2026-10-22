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

#अन्तर्भाव "config.h"

#अन्तर्भाव "mosquitto.h"
#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "utlist.h"

#ifdef WITH_BRIDGE


स्थिर पूर्णाङ्क bridge__create_remap_topic(ध्रुव वर्ण *prefix, ध्रुव वर्ण *topic, वर्ण **remap_topic)
{
	यदि(prefix){
		यदि(topic){
			size_t len = strlen(topic) + strlen(prefix)+1;
			*remap_topic = mosquitto_malloc(len+1);
			यदि(!(*remap_topic)){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
			snprintf(*remap_topic, len+1, "%s%s", prefix, topic);
			(*remap_topic)[len] = '\0';
		}अन्यथा{
			*remap_topic = mosquitto_strdup(prefix);
			यदि(!(*remap_topic)){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
		}
	}अन्यथा{
		*remap_topic = mosquitto_strdup(topic);
		यदि(!(*remap_topic)){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क bridge__create_prefix(वर्ण **full_prefix, ध्रुव वर्ण *topic, ध्रुव वर्ण *prefix, ध्रुव वर्ण *direction)
{
	size_t len;

	यदि(!prefix || strlen(prefix) != 0){
		यदि(mosquitto_pub_topic_check(prefix) != MOSQ_ERR_SUCCESS){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", prefix);
			प्रत्यागम MOSQ_ERR_INVAL;
		}
	}

	यदि(topic){
		len = strlen(topic) + strlen(prefix) + 1;
	}अन्यथा{
		len = strlen(prefix) + 1;
	}
	*full_prefix = mosquitto_malloc(len);
	यदि(*full_prefix == रिक्त){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	यदि(topic){
		/* Print full_prefix+pattern to check for validity */
		snprintf(*full_prefix, len, "%s%s", prefix, topic);
	}अन्यथा{
		snprintf(*full_prefix, len, "%s", prefix);
	}

	यदि(mosquitto_sub_topic_check(*full_prefix) != MOSQ_ERR_SUCCESS){
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Invalid bridge topic %s prefix and pattern combination '%s'.",
				direction, *full_prefix);

		प्रत्यागम MOSQ_ERR_INVAL;
	}

	/* Print just the prefix for storage */
	snprintf(*full_prefix, len, "%s", prefix);

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर संरचना mosquitto__bridge_topic *bridge__find_topic(संरचना mosquitto__bridge *bridge, ध्रुव वर्ण *topic, गणना mosquitto__bridge_direction direction, uint8_t qos, ध्रुव वर्ण *local_prefix, ध्रुव वर्ण *remote_prefix)
{
	संरचना mosquitto__bridge_topic *cur_topic = रिक्त;
	bool found = false;

	LL_FOREACH(bridge->topics, cur_topic){
		यदि(cur_topic->direction != direction){
			अनुवर्त;
		}
		यदि(cur_topic->qos != qos){
			अनुवर्त;
		}
		यदि(cur_topic->topic != रिक्त && topic != रिक्त){
			यदि(strcmp(cur_topic->topic, topic)){
				अनुवर्त;
			}
		}
		यदि(cur_topic->local_prefix != रिक्त && local_prefix != रिक्त){
			यदि(strcmp(cur_topic->local_prefix, local_prefix)){
				अनुवर्त;
			}
		}
		यदि(cur_topic->remote_prefix != रिक्त && remote_prefix != रिक्त){
			यदि(strcmp(cur_topic->remote_prefix, remote_prefix)){
				अनुवर्त;
			}
		}
		found = true;
		भङ्ग;
	}
	यदि(!found){
		cur_topic = रिक्त;
	}

	प्रत्यागम cur_topic;
}


शून्य bridge__cleanup_topics(संरचना mosquitto__bridge *bridge)
{
	संरचना mosquitto__bridge_topic *topic, *topic_tmp;

	यदि(!bridge){
		प्रत्यागम;
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
पूर्णाङ्क bridge__add_topic(संरचना mosquitto__bridge *bridge, ध्रुव वर्ण *topic, गणना mosquitto__bridge_direction direction, uint8_t qos, ध्रुव वर्ण *local_prefix, ध्रुव वर्ण *remote_prefix)
{
	संरचना mosquitto__bridge_topic *cur_topic;

	यदि(bridge == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि(direction != bd_out && direction != bd_in && direction != bd_both){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि(qos > 2){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि(local_prefix && mosquitto_pub_topic_check(local_prefix)){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", local_prefix);
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि(remote_prefix && mosquitto_pub_topic_check(remote_prefix)){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge topic remote prefix '%s'.", remote_prefix);
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि((topic == रिक्त || !strcmp(topic, "\"\"")) &&
			(local_prefix == रिक्त || remote_prefix == रिक्त)){

		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge remapping.");
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	यदि(bridge__find_topic(bridge, topic, direction, qos, local_prefix, remote_prefix) != रिक्त){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Duplicate bridge topic '%s', skipping", topic);
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	bridge->topic_गिनती++;
	cur_topic = mosquitto_calloc(1, परिमाण(संरचना mosquitto__bridge_topic));
	यदि(cur_topic == रिक्त){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	cur_topic->next = रिक्त;

	cur_topic->direction = direction;
	cur_topic->qos = qos;
	cur_topic->local_prefix = रिक्त;
	cur_topic->remote_prefix = रिक्त;

	यदि(topic == रिक्त || !strcmp(topic, "\"\"")){
		cur_topic->topic = रिक्त;
	}अन्यथा{
		cur_topic->topic = mosquitto_strdup(topic);
		यदि(cur_topic->topic == रिक्त){
			गच्छ error;
		}
	}

	यदि(local_prefix || remote_prefix){
		bridge->topic_remapping = true;
		यदि(local_prefix){
			यदि(bridge__create_prefix(&cur_topic->local_prefix, cur_topic->topic, local_prefix, "local")){
				गच्छ error;
			}
		}
		यदि(remote_prefix){
			यदि(bridge__create_prefix(&cur_topic->remote_prefix, cur_topic->topic, remote_prefix, "local")){
				गच्छ error;
			}
		}
	}

	यदि(bridge__create_remap_topic(cur_topic->local_prefix,
			cur_topic->topic, &cur_topic->local_topic)){

		गच्छ error;
	}

	यदि(bridge__create_remap_topic(cur_topic->remote_prefix,
			cur_topic->topic, &cur_topic->remote_topic)){

		गच्छ error;
	}

	LL_APPEND(bridge->topics, cur_topic);

	प्रत्यागम MOSQ_ERR_SUCCESS;

error:
	mosquitto_FREE(cur_topic->local_prefix);
	mosquitto_FREE(cur_topic->remote_prefix);
	mosquitto_FREE(cur_topic->local_topic);
	mosquitto_FREE(cur_topic->remote_topic);
	mosquitto_FREE(cur_topic->topic);
	mosquitto_FREE(cur_topic);
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
	प्रत्यागम MOSQ_ERR_NOMEM;
}


पूर्णाङ्क bridge__remap_topic_in(संरचना mosquitto *context, वर्ण **topic)
{
	संरचना mosquitto__bridge_topic *cur_topic;
	bool match;

	यदि(context->bridge && context->bridge->topics && context->bridge->topic_remapping){
		LL_FOREACH(context->bridge->topics, cur_topic){
			यदि((cur_topic->direction == bd_both || cur_topic->direction == bd_in)
					&& (cur_topic->remote_prefix || cur_topic->local_prefix)){

				/* Topic mapping required on this topic if the message matches */

				पूर्णाङ्क rc = mosquitto_topic_matches_sub(cur_topic->remote_topic, *topic, &match);
				यदि(rc){
					mosquitto_FREE(*topic);
					प्रत्यागम rc;
				}
				यदि(match){
					वर्ण *topic_temp;

					यदि(cur_topic->remote_prefix){
						/* This prefix needs removing. */
						यदि(!strncmp(cur_topic->remote_prefix, *topic, strlen(cur_topic->remote_prefix))){
							topic_temp = mosquitto_strdup((*topic)+strlen(cur_topic->remote_prefix));
							यदि(!topic_temp){
								mosquitto_FREE(*topic);
								प्रत्यागम MOSQ_ERR_NOMEM;
							}
							mosquitto_FREE(*topic);
							*topic = topic_temp;
						}
					}

					यदि(cur_topic->local_prefix){
						/* This prefix needs adding. */
						size_t len = strlen(*topic) + strlen(cur_topic->local_prefix)+1;
						topic_temp = mosquitto_malloc(len+1);
						यदि(!topic_temp){
							mosquitto_FREE(*topic);
							प्रत्यागम MOSQ_ERR_NOMEM;
						}
						snprintf(topic_temp, len, "%s%s", cur_topic->local_prefix, *topic);
						topic_temp[len] = '\0';

						mosquitto_FREE(*topic);
						*topic = topic_temp;
					}
					भङ्ग;
				}
			}
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}

#endif
