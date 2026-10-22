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


स्थिर पूर्णांक bridge__create_remap_topic(स्थिरांक अक्षर *prefix, स्थिरांक अक्षर *topic, अक्षर **remap_topic)
{
	जर(prefix){
		जर(topic){
			माप_प्रकार len = म_माप(topic) + म_माप(prefix)+1;
			*remap_topic = mosquitto_malloc(len+1);
			जर(!(*remap_topic)){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
				परत MOSQ_ERR_NOMEM;
			}
			snprintf(*remap_topic, len+1, "%s%s", prefix, topic);
			(*remap_topic)[len] = '\0';
		}अन्यथा{
			*remap_topic = mosquitto_strdup(prefix);
			जर(!(*remap_topic)){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
				परत MOSQ_ERR_NOMEM;
			}
		}
	}अन्यथा{
		*remap_topic = mosquitto_strdup(topic);
		जर(!(*remap_topic)){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
			परत MOSQ_ERR_NOMEM;
		}
	}
	परत MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णांक bridge__create_prefix(अक्षर **full_prefix, स्थिरांक अक्षर *topic, स्थिरांक अक्षर *prefix, स्थिरांक अक्षर *direction)
{
	माप_प्रकार len;

	जर(!prefix || म_माप(prefix) != 0){
		जर(mosquitto_pub_topic_check(prefix) != MOSQ_ERR_SUCCESS){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", prefix);
			परत MOSQ_ERR_INVAL;
		}
	}

	जर(topic){
		len = म_माप(topic) + म_माप(prefix) + 1;
	}अन्यथा{
		len = म_माप(prefix) + 1;
	}
	*full_prefix = mosquitto_malloc(len);
	जर(*full_prefix == रिक्त){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		परत MOSQ_ERR_NOMEM;
	}

	जर(topic){
		/* Print full_prefix+pattern to check for validity */
		snprintf(*full_prefix, len, "%s%s", prefix, topic);
	}अन्यथा{
		snprintf(*full_prefix, len, "%s", prefix);
	}

	जर(mosquitto_sub_topic_check(*full_prefix) != MOSQ_ERR_SUCCESS){
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Invalid bridge topic %s prefix and pattern combination '%s'.",
				direction, *full_prefix);

		परत MOSQ_ERR_INVAL;
	}

	/* Print just the prefix for storage */
	snprintf(*full_prefix, len, "%s", prefix);

	परत MOSQ_ERR_SUCCESS;
}


स्थिर संरचना mosquitto__bridge_topic *bridge__find_topic(संरचना mosquitto__bridge *bridge, स्थिरांक अक्षर *topic, गणना mosquitto__bridge_direction direction, uint8_t qos, स्थिरांक अक्षर *local_prefix, स्थिरांक अक्षर *remote_prefix)
{
	संरचना mosquitto__bridge_topic *cur_topic = रिक्त;
	bool found = false;

	LL_FOREACH(bridge->topics, cur_topic){
		जर(cur_topic->direction != direction){
			सुरूठेवा;
		}
		जर(cur_topic->qos != qos){
			सुरूठेवा;
		}
		जर(cur_topic->topic != रिक्त && topic != रिक्त){
			जर(म_भेद(cur_topic->topic, topic)){
				सुरूठेवा;
			}
		}
		जर(cur_topic->local_prefix != रिक्त && local_prefix != रिक्त){
			जर(म_भेद(cur_topic->local_prefix, local_prefix)){
				सुरूठेवा;
			}
		}
		जर(cur_topic->remote_prefix != रिक्त && remote_prefix != रिक्त){
			जर(म_भेद(cur_topic->remote_prefix, remote_prefix)){
				सुरूठेवा;
			}
		}
		found = true;
		थांबा;
	}
	जर(!found){
		cur_topic = रिक्त;
	}

	परत cur_topic;
}


शून्य bridge__cleanup_topics(संरचना mosquitto__bridge *bridge)
{
	संरचना mosquitto__bridge_topic *topic, *topic_tmp;

	जर(!bridge){
		परत;
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
पूर्णांक bridge__add_topic(संरचना mosquitto__bridge *bridge, स्थिरांक अक्षर *topic, गणना mosquitto__bridge_direction direction, uint8_t qos, स्थिरांक अक्षर *local_prefix, स्थिरांक अक्षर *remote_prefix)
{
	संरचना mosquitto__bridge_topic *cur_topic;

	जर(bridge == रिक्त){
		परत MOSQ_ERR_INVAL;
	}
	जर(direction != bd_out && direction != bd_in && direction != bd_both){
		परत MOSQ_ERR_INVAL;
	}
	जर(qos > 2){
		परत MOSQ_ERR_INVAL;
	}
	जर(local_prefix && mosquitto_pub_topic_check(local_prefix)){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", local_prefix);
		परत MOSQ_ERR_INVAL;
	}
	जर(remote_prefix && mosquitto_pub_topic_check(remote_prefix)){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge topic remote prefix '%s'.", remote_prefix);
		परत MOSQ_ERR_INVAL;
	}
	जर((topic == रिक्त || !म_भेद(topic, "\"\"")) &&
			(local_prefix == रिक्त || remote_prefix == रिक्त)){

		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge remapping.");
		परत MOSQ_ERR_INVAL;
	}

	जर(bridge__find_topic(bridge, topic, direction, qos, local_prefix, remote_prefix) != रिक्त){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Duplicate bridge topic '%s', skipping", topic);
		परत MOSQ_ERR_SUCCESS;
	}

	bridge->topic_मोजणी++;
	cur_topic = mosquitto_calloc(1, आकार(संरचना mosquitto__bridge_topic));
	जर(cur_topic == रिक्त){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		परत MOSQ_ERR_NOMEM;
	}
	cur_topic->next = रिक्त;

	cur_topic->direction = direction;
	cur_topic->qos = qos;
	cur_topic->local_prefix = रिक्त;
	cur_topic->remote_prefix = रिक्त;

	जर(topic == रिक्त || !म_भेद(topic, "\"\"")){
		cur_topic->topic = रिक्त;
	}अन्यथा{
		cur_topic->topic = mosquitto_strdup(topic);
		जर(cur_topic->topic == रिक्त){
			जा error;
		}
	}

	जर(local_prefix || remote_prefix){
		bridge->topic_remapping = true;
		जर(local_prefix){
			जर(bridge__create_prefix(&cur_topic->local_prefix, cur_topic->topic, local_prefix, "local")){
				जा error;
			}
		}
		जर(remote_prefix){
			जर(bridge__create_prefix(&cur_topic->remote_prefix, cur_topic->topic, remote_prefix, "local")){
				जा error;
			}
		}
	}

	जर(bridge__create_remap_topic(cur_topic->local_prefix,
			cur_topic->topic, &cur_topic->local_topic)){

		जा error;
	}

	जर(bridge__create_remap_topic(cur_topic->remote_prefix,
			cur_topic->topic, &cur_topic->remote_topic)){

		जा error;
	}

	LL_APPEND(bridge->topics, cur_topic);

	परत MOSQ_ERR_SUCCESS;

error:
	mosquitto_FREE(cur_topic->local_prefix);
	mosquitto_FREE(cur_topic->remote_prefix);
	mosquitto_FREE(cur_topic->local_topic);
	mosquitto_FREE(cur_topic->remote_topic);
	mosquitto_FREE(cur_topic->topic);
	mosquitto_FREE(cur_topic);
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
	परत MOSQ_ERR_NOMEM;
}


पूर्णांक bridge__remap_topic_in(संरचना mosquitto *context, अक्षर **topic)
{
	संरचना mosquitto__bridge_topic *cur_topic;
	bool match;

	जर(context->bridge && context->bridge->topics && context->bridge->topic_remapping){
		LL_FOREACH(context->bridge->topics, cur_topic){
			जर((cur_topic->direction == bd_both || cur_topic->direction == bd_in)
					&& (cur_topic->remote_prefix || cur_topic->local_prefix)){

				/* Topic mapping required on this topic if the message matches */

				पूर्णांक rc = mosquitto_topic_matches_sub(cur_topic->remote_topic, *topic, &match);
				जर(rc){
					mosquitto_FREE(*topic);
					परत rc;
				}
				जर(match){
					अक्षर *topic_temp;

					जर(cur_topic->remote_prefix){
						/* This prefix needs removing. */
						जर(!म_भेदन(cur_topic->remote_prefix, *topic, म_माप(cur_topic->remote_prefix))){
							topic_temp = mosquitto_strdup((*topic)+म_माप(cur_topic->remote_prefix));
							जर(!topic_temp){
								mosquitto_FREE(*topic);
								परत MOSQ_ERR_NOMEM;
							}
							mosquitto_FREE(*topic);
							*topic = topic_temp;
						}
					}

					जर(cur_topic->local_prefix){
						/* This prefix needs adding. */
						माप_प्रकार len = म_माप(*topic) + म_माप(cur_topic->local_prefix)+1;
						topic_temp = mosquitto_malloc(len+1);
						जर(!topic_temp){
							mosquitto_FREE(*topic);
							परत MOSQ_ERR_NOMEM;
						}
						snprintf(topic_temp, len, "%s%s", cur_topic->local_prefix, *topic);
						topic_temp[len] = '\0';

						mosquitto_FREE(*topic);
						*topic = topic_temp;
					}
					थांबा;
				}
			}
		}
	}

	परत MOSQ_ERR_SUCCESS;
}

#endif
