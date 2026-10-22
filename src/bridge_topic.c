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

#समावेश "config.h"

#समावेश "mosquitto.h"
#समावेश "mosquitto_broker_internal.h"
#समावेश "utlist.h"

#अगर_घोषित WITH_BRIDGE


अटल पूर्णांक bridge__create_remap_topic(स्थिर अक्षर *prefix, स्थिर अक्षर *topic, अक्षर **remap_topic)
{
	अगर(prefix){
		अगर(topic){
			माप_प्रकार len = म_माप(topic) + म_माप(prefix)+1;
			*remap_topic = mosquitto_malloc(len+1);
			अगर(!(*remap_topic)){
				log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
				वापस MOSQ_ERR_NOMEM;
			}
			snprintf(*remap_topic, len+1, "%s%s", prefix, topic);
			(*remap_topic)[len] = '\0';
		}अन्यथा{
			*remap_topic = mosquitto_strdup(prefix);
			अगर(!(*remap_topic)){
				log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
				वापस MOSQ_ERR_NOMEM;
			}
		}
	}अन्यथा{
		*remap_topic = mosquitto_strdup(topic);
		अगर(!(*remap_topic)){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
			वापस MOSQ_ERR_NOMEM;
		}
	}
	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक bridge__create_prefix(अक्षर **full_prefix, स्थिर अक्षर *topic, स्थिर अक्षर *prefix, स्थिर अक्षर *direction)
{
	माप_प्रकार len;

	अगर(!prefix || म_माप(prefix) != 0){
		अगर(mosquitto_pub_topic_check(prefix) != MOSQ_ERR_SUCCESS){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", prefix);
			वापस MOSQ_ERR_INVAL;
		}
	}

	अगर(topic){
		len = म_माप(topic) + म_माप(prefix) + 1;
	}अन्यथा{
		len = म_माप(prefix) + 1;
	}
	*full_prefix = mosquitto_malloc(len);
	अगर(*full_prefix == शून्य){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
		वापस MOSQ_ERR_NOMEM;
	}

	अगर(topic){
		/* Print full_prefix+pattern to check for validity */
		snprintf(*full_prefix, len, "%s%s", prefix, topic);
	}अन्यथा{
		snprintf(*full_prefix, len, "%s", prefix);
	}

	अगर(mosquitto_sub_topic_check(*full_prefix) != MOSQ_ERR_SUCCESS){
		log__printf(शून्य, MOSQ_LOG_ERR,
				"Error: Invalid bridge topic %s prefix and pattern combination '%s'.",
				direction, *full_prefix);

		वापस MOSQ_ERR_INVAL;
	}

	/* Print just the prefix for storage */
	snprintf(*full_prefix, len, "%s", prefix);

	वापस MOSQ_ERR_SUCCESS;
}


अटल काष्ठा mosquitto__bridge_topic *bridge__find_topic(काष्ठा mosquitto__bridge *bridge, स्थिर अक्षर *topic, क्रमागत mosquitto__bridge_direction direction, uint8_t qos, स्थिर अक्षर *local_prefix, स्थिर अक्षर *remote_prefix)
{
	काष्ठा mosquitto__bridge_topic *cur_topic = शून्य;
	bool found = false;

	LL_FOREACH(bridge->topics, cur_topic){
		अगर(cur_topic->direction != direction){
			जारी;
		}
		अगर(cur_topic->qos != qos){
			जारी;
		}
		अगर(cur_topic->topic != शून्य && topic != शून्य){
			अगर(म_भेद(cur_topic->topic, topic)){
				जारी;
			}
		}
		अगर(cur_topic->local_prefix != शून्य && local_prefix != शून्य){
			अगर(म_भेद(cur_topic->local_prefix, local_prefix)){
				जारी;
			}
		}
		अगर(cur_topic->remote_prefix != शून्य && remote_prefix != शून्य){
			अगर(म_भेद(cur_topic->remote_prefix, remote_prefix)){
				जारी;
			}
		}
		found = true;
		अवरोध;
	}
	अगर(!found){
		cur_topic = शून्य;
	}

	वापस cur_topic;
}


व्योम bridge__cleanup_topics(काष्ठा mosquitto__bridge *bridge)
{
	काष्ठा mosquitto__bridge_topic *topic, *topic_tmp;

	अगर(!bridge){
		वापस;
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
पूर्णांक bridge__add_topic(काष्ठा mosquitto__bridge *bridge, स्थिर अक्षर *topic, क्रमागत mosquitto__bridge_direction direction, uint8_t qos, स्थिर अक्षर *local_prefix, स्थिर अक्षर *remote_prefix)
{
	काष्ठा mosquitto__bridge_topic *cur_topic;

	अगर(bridge == शून्य){
		वापस MOSQ_ERR_INVAL;
	}
	अगर(direction != bd_out && direction != bd_in && direction != bd_both){
		वापस MOSQ_ERR_INVAL;
	}
	अगर(qos > 2){
		वापस MOSQ_ERR_INVAL;
	}
	अगर(local_prefix && mosquitto_pub_topic_check(local_prefix)){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", local_prefix);
		वापस MOSQ_ERR_INVAL;
	}
	अगर(remote_prefix && mosquitto_pub_topic_check(remote_prefix)){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid bridge topic remote prefix '%s'.", remote_prefix);
		वापस MOSQ_ERR_INVAL;
	}
	अगर((topic == शून्य || !म_भेद(topic, "\"\"")) &&
			(local_prefix == शून्य || remote_prefix == शून्य)){

		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid bridge remapping.");
		वापस MOSQ_ERR_INVAL;
	}

	अगर(bridge__find_topic(bridge, topic, direction, qos, local_prefix, remote_prefix) != शून्य){
		log__printf(शून्य, MOSQ_LOG_INFO, "Duplicate bridge topic '%s', skipping", topic);
		वापस MOSQ_ERR_SUCCESS;
	}

	bridge->topic_गिनती++;
	cur_topic = mosquitto_calloc(1, माप(काष्ठा mosquitto__bridge_topic));
	अगर(cur_topic == शून्य){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
		वापस MOSQ_ERR_NOMEM;
	}
	cur_topic->अग्र = शून्य;

	cur_topic->direction = direction;
	cur_topic->qos = qos;
	cur_topic->local_prefix = शून्य;
	cur_topic->remote_prefix = शून्य;

	अगर(topic == शून्य || !म_भेद(topic, "\"\"")){
		cur_topic->topic = शून्य;
	}अन्यथा{
		cur_topic->topic = mosquitto_strdup(topic);
		अगर(cur_topic->topic == शून्य){
			जाओ error;
		}
	}

	अगर(local_prefix || remote_prefix){
		bridge->topic_remapping = true;
		अगर(local_prefix){
			अगर(bridge__create_prefix(&cur_topic->local_prefix, cur_topic->topic, local_prefix, "local")){
				जाओ error;
			}
		}
		अगर(remote_prefix){
			अगर(bridge__create_prefix(&cur_topic->remote_prefix, cur_topic->topic, remote_prefix, "local")){
				जाओ error;
			}
		}
	}

	अगर(bridge__create_remap_topic(cur_topic->local_prefix,
			cur_topic->topic, &cur_topic->local_topic)){

		जाओ error;
	}

	अगर(bridge__create_remap_topic(cur_topic->remote_prefix,
			cur_topic->topic, &cur_topic->remote_topic)){

		जाओ error;
	}

	LL_APPEND(bridge->topics, cur_topic);

	वापस MOSQ_ERR_SUCCESS;

error:
	mosquitto_FREE(cur_topic->local_prefix);
	mosquitto_FREE(cur_topic->remote_prefix);
	mosquitto_FREE(cur_topic->local_topic);
	mosquitto_FREE(cur_topic->remote_topic);
	mosquitto_FREE(cur_topic->topic);
	mosquitto_FREE(cur_topic);
	log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
	वापस MOSQ_ERR_NOMEM;
}


पूर्णांक bridge__remap_topic_in(काष्ठा mosquitto *context, अक्षर **topic)
{
	काष्ठा mosquitto__bridge_topic *cur_topic;
	bool match;

	अगर(context->bridge && context->bridge->topics && context->bridge->topic_remapping){
		LL_FOREACH(context->bridge->topics, cur_topic){
			अगर((cur_topic->direction == bd_both || cur_topic->direction == bd_in)
					&& (cur_topic->remote_prefix || cur_topic->local_prefix)){

				/* Topic mapping required on this topic if the message matches */

				पूर्णांक rc = mosquitto_topic_matches_sub(cur_topic->remote_topic, *topic, &match);
				अगर(rc){
					mosquitto_FREE(*topic);
					वापस rc;
				}
				अगर(match){
					अक्षर *topic_अस्थायी;

					अगर(cur_topic->remote_prefix){
						/* This prefix needs removing. */
						अगर(!म_भेदन(cur_topic->remote_prefix, *topic, म_माप(cur_topic->remote_prefix))){
							topic_अस्थायी = mosquitto_strdup((*topic)+म_माप(cur_topic->remote_prefix));
							अगर(!topic_अस्थायी){
								mosquitto_FREE(*topic);
								वापस MOSQ_ERR_NOMEM;
							}
							mosquitto_FREE(*topic);
							*topic = topic_अस्थायी;
						}
					}

					अगर(cur_topic->local_prefix){
						/* This prefix needs adding. */
						माप_प्रकार len = म_माप(*topic) + म_माप(cur_topic->local_prefix)+1;
						topic_अस्थायी = mosquitto_malloc(len+1);
						अगर(!topic_अस्थायी){
							mosquitto_FREE(*topic);
							वापस MOSQ_ERR_NOMEM;
						}
						snprintf(topic_अस्थायी, len, "%s%s", cur_topic->local_prefix, *topic);
						topic_अस्थायी[len] = '\0';

						mosquitto_FREE(*topic);
						*topic = topic_अस्थायी;
					}
					अवरोध;
				}
			}
		}
	}

	वापस MOSQ_ERR_SUCCESS;
}

#पूर्ण_अगर
