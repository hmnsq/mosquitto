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
	सचे(prefix){
		सचे(topic){
			size_t len = strlen(topic) + strlen(prefix)+1;
			*remap_topic = mosquitto_malloc(len+1);
			सचे(!(*remap_topic)){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
				पटिनिवत्त MOSQ_ERR_NOMEM;
			}
			snprintf(*remap_topic, len+1, "%s%s", prefix, topic);
			(*remap_topic)[len] = '\0';
		}अञ्ञथा{
			*remap_topic = mosquitto_strdup(prefix);
			सचे(!(*remap_topic)){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
				पटिनिवत्त MOSQ_ERR_NOMEM;
			}
		}
	}अञ्ञथा{
		*remap_topic = mosquitto_strdup(topic);
		सचे(!(*remap_topic)){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
			पटिनिवत्त MOSQ_ERR_NOMEM;
		}
	}
	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क bridge__create_prefix(वर्ण **full_prefix, ध्रुव वर्ण *topic, ध्रुव वर्ण *prefix, ध्रुव वर्ण *direction)
{
	size_t len;

	सचे(!prefix || strlen(prefix) != 0){
		सचे(mosquitto_pub_topic_check(prefix) != MOSQ_ERR_SUCCESS){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", prefix);
			पटिनिवत्त MOSQ_ERR_INVAL;
		}
	}

	सचे(topic){
		len = strlen(topic) + strlen(prefix) + 1;
	}अञ्ञथा{
		len = strlen(prefix) + 1;
	}
	*full_prefix = mosquitto_malloc(len);
	सचे(*full_prefix == रिक्त){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		पटिनिवत्त MOSQ_ERR_NOMEM;
	}

	सचे(topic){
		/* Print full_prefix+pattern to check for validity */
		snprintf(*full_prefix, len, "%s%s", prefix, topic);
	}अञ्ञथा{
		snprintf(*full_prefix, len, "%s", prefix);
	}

	सचे(mosquitto_sub_topic_check(*full_prefix) != MOSQ_ERR_SUCCESS){
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Invalid bridge topic %s prefix and pattern combination '%s'.",
				direction, *full_prefix);

		पटिनिवत्त MOSQ_ERR_INVAL;
	}

	/* Print just the prefix for storage */
	snprintf(*full_prefix, len, "%s", prefix);

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


स्थिर संरचना mosquitto__bridge_topic *bridge__find_topic(संरचना mosquitto__bridge *bridge, ध्रुव वर्ण *topic, गणना mosquitto__bridge_direction direction, uint8_t qos, ध्रुव वर्ण *local_prefix, ध्रुव वर्ण *remote_prefix)
{
	संरचना mosquitto__bridge_topic *cur_topic = रिक्त;
	bool found = false;

	LL_FOREACH(bridge->topics, cur_topic){
		सचे(cur_topic->direction != direction){
			अनुवर्त;
		}
		सचे(cur_topic->qos != qos){
			अनुवर्त;
		}
		सचे(cur_topic->topic != रिक्त && topic != रिक्त){
			सचे(strcmp(cur_topic->topic, topic)){
				अनुवर्त;
			}
		}
		सचे(cur_topic->local_prefix != रिक्त && local_prefix != रिक्त){
			सचे(strcmp(cur_topic->local_prefix, local_prefix)){
				अनुवर्त;
			}
		}
		सचे(cur_topic->remote_prefix != रिक्त && remote_prefix != रिक्त){
			सचे(strcmp(cur_topic->remote_prefix, remote_prefix)){
				अनुवर्त;
			}
		}
		found = true;
		भङ्ग;
	}
	सचे(!found){
		cur_topic = रिक्त;
	}

	पटिनिवत्त cur_topic;
}


शून्य bridge__cleanup_topics(संरचना mosquitto__bridge *bridge)
{
	संरचना mosquitto__bridge_topic *topic, *topic_tmp;

	सचे(!bridge){
		पटिनिवत्त;
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

	सचे(bridge == रिक्त){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}
	सचे(direction != bd_out && direction != bd_in && direction != bd_both){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}
	सचे(qos > 2){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}
	सचे(local_prefix && mosquitto_pub_topic_check(local_prefix)){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", local_prefix);
		पटिनिवत्त MOSQ_ERR_INVAL;
	}
	सचे(remote_prefix && mosquitto_pub_topic_check(remote_prefix)){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge topic remote prefix '%s'.", remote_prefix);
		पटिनिवत्त MOSQ_ERR_INVAL;
	}
	सचे((topic == रिक्त || !strcmp(topic, "\"\"")) &&
			(local_prefix == रिक्त || remote_prefix == रिक्त)){

		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge remapping.");
		पटिनिवत्त MOSQ_ERR_INVAL;
	}

	सचे(bridge__find_topic(bridge, topic, direction, qos, local_prefix, remote_prefix) != रिक्त){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Duplicate bridge topic '%s', skipping", topic);
		पटिनिवत्त MOSQ_ERR_SUCCESS;
	}

	bridge->topic_गणनम्++;
	cur_topic = mosquitto_calloc(1, परिमाण(संरचना mosquitto__bridge_topic));
	सचे(cur_topic == रिक्त){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		पटिनिवत्त MOSQ_ERR_NOMEM;
	}
	cur_topic->next = रिक्त;

	cur_topic->direction = direction;
	cur_topic->qos = qos;
	cur_topic->local_prefix = रिक्त;
	cur_topic->remote_prefix = रिक्त;

	सचे(topic == रिक्त || !strcmp(topic, "\"\"")){
		cur_topic->topic = रिक्त;
	}अञ्ञथा{
		cur_topic->topic = mosquitto_strdup(topic);
		सचे(cur_topic->topic == रिक्त){
			गच्छ error;
		}
	}

	सचे(local_prefix || remote_prefix){
		bridge->topic_remapping = true;
		सचे(local_prefix){
			सचे(bridge__create_prefix(&cur_topic->local_prefix, cur_topic->topic, local_prefix, "local")){
				गच्छ error;
			}
		}
		सचे(remote_prefix){
			सचे(bridge__create_prefix(&cur_topic->remote_prefix, cur_topic->topic, remote_prefix, "local")){
				गच्छ error;
			}
		}
	}

	सचे(bridge__create_remap_topic(cur_topic->local_prefix,
			cur_topic->topic, &cur_topic->local_topic)){

		गच्छ error;
	}

	सचे(bridge__create_remap_topic(cur_topic->remote_prefix,
			cur_topic->topic, &cur_topic->remote_topic)){

		गच्छ error;
	}

	LL_APPEND(bridge->topics, cur_topic);

	पटिनिवत्त MOSQ_ERR_SUCCESS;

error:
	mosquitto_FREE(cur_topic->local_prefix);
	mosquitto_FREE(cur_topic->remote_prefix);
	mosquitto_FREE(cur_topic->local_topic);
	mosquitto_FREE(cur_topic->remote_topic);
	mosquitto_FREE(cur_topic->topic);
	mosquitto_FREE(cur_topic);
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
	पटिनिवत्त MOSQ_ERR_NOMEM;
}


पूर्णाङ्क bridge__remap_topic_in(संरचना mosquitto *context, वर्ण **topic)
{
	संरचना mosquitto__bridge_topic *cur_topic;
	bool match;

	सचे(context->bridge && context->bridge->topics && context->bridge->topic_remapping){
		LL_FOREACH(context->bridge->topics, cur_topic){
			सचे((cur_topic->direction == bd_both || cur_topic->direction == bd_in)
					&& (cur_topic->remote_prefix || cur_topic->local_prefix)){

				/* Topic mapping required on this topic if the message matches */

				पूर्णाङ्क rc = mosquitto_topic_matches_sub(cur_topic->remote_topic, *topic, &match);
				सचे(rc){
					mosquitto_FREE(*topic);
					पटिनिवत्त rc;
				}
				सचे(match){
					वर्ण *topic_temp;

					सचे(cur_topic->remote_prefix){
						/* This prefix needs removing. */
						सचे(!strncmp(cur_topic->remote_prefix, *topic, strlen(cur_topic->remote_prefix))){
							topic_temp = mosquitto_strdup((*topic)+strlen(cur_topic->remote_prefix));
							सचे(!topic_temp){
								mosquitto_FREE(*topic);
								पटिनिवत्त MOSQ_ERR_NOMEM;
							}
							mosquitto_FREE(*topic);
							*topic = topic_temp;
						}
					}

					सचे(cur_topic->local_prefix){
						/* This prefix needs adding. */
						size_t len = strlen(*topic) + strlen(cur_topic->local_prefix)+1;
						topic_temp = mosquitto_malloc(len+1);
						सचे(!topic_temp){
							mosquitto_FREE(*topic);
							पटिनिवत्त MOSQ_ERR_NOMEM;
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

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}

#endif
