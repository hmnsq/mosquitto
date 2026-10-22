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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ "mosquitto.h"
#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "utlist.h"

#ifdef WITH_BRIDGE


സ്ഥിതം പൂർണ്ണസംഖ്യ bridge__create_remap_topic(സ്ഥിരം അക്ഷരം *prefix, സ്ഥിരം അക്ഷരം *topic, അക്ഷരം **remap_topic)
{
	എങ്കിൽ(prefix){
		എങ്കിൽ(topic){
			size_t len = strlen(topic) + strlen(prefix)+1;
			*remap_topic = mosquitto_malloc(len+1);
			എങ്കിൽ(!(*remap_topic)){
				log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Out of memory.");
				തിരികെ MOSQ_ERR_NOMEM;
			}
			snprintf(*remap_topic, len+1, "%s%s", prefix, topic);
			(*remap_topic)[len] = '\0';
		}അല്ലെങ്കിൽ{
			*remap_topic = mosquitto_strdup(prefix);
			എങ്കിൽ(!(*remap_topic)){
				log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Out of memory.");
				തിരികെ MOSQ_ERR_NOMEM;
			}
		}
	}അല്ലെങ്കിൽ{
		*remap_topic = mosquitto_strdup(topic);
		എങ്കിൽ(!(*remap_topic)){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Out of memory.");
			തിരികെ MOSQ_ERR_NOMEM;
		}
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ bridge__create_prefix(അക്ഷരം **full_prefix, സ്ഥിരം അക്ഷരം *topic, സ്ഥിരം അക്ഷരം *prefix, സ്ഥിരം അക്ഷരം *direction)
{
	size_t len;

	എങ്കിൽ(!prefix || strlen(prefix) != 0){
		എങ്കിൽ(mosquitto_pub_topic_check(prefix) != MOSQ_ERR_SUCCESS){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", prefix);
			തിരികെ MOSQ_ERR_INVAL;
		}
	}

	എങ്കിൽ(topic){
		len = strlen(topic) + strlen(prefix) + 1;
	}അല്ലെങ്കിൽ{
		len = strlen(prefix) + 1;
	}
	*full_prefix = mosquitto_malloc(len);
	എങ്കിൽ(*full_prefix == ശൂന്യ){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Out of memory.");
		തിരികെ MOSQ_ERR_NOMEM;
	}

	എങ്കിൽ(topic){
		/* Print full_prefix+pattern to check for validity */
		snprintf(*full_prefix, len, "%s%s", prefix, topic);
	}അല്ലെങ്കിൽ{
		snprintf(*full_prefix, len, "%s", prefix);
	}

	എങ്കിൽ(mosquitto_sub_topic_check(*full_prefix) != MOSQ_ERR_SUCCESS){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR,
				"Error: Invalid bridge topic %s prefix and pattern combination '%s'.",
				direction, *full_prefix);

		തിരികെ MOSQ_ERR_INVAL;
	}

	/* Print just the prefix for storage */
	snprintf(*full_prefix, len, "%s", prefix);

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം ഘടന mosquitto__bridge_topic *bridge__find_topic(ഘടന mosquitto__bridge *bridge, സ്ഥിരം അക്ഷരം *topic, എണ്ണൽ mosquitto__bridge_direction direction, uint8_t qos, സ്ഥിരം അക്ഷരം *local_prefix, സ്ഥിരം അക്ഷരം *remote_prefix)
{
	ഘടന mosquitto__bridge_topic *cur_topic = ശൂന്യ;
	bool found = false;

	LL_FOREACH(bridge->topics, cur_topic){
		എങ്കിൽ(cur_topic->direction != direction){
			തുടരുക;
		}
		എങ്കിൽ(cur_topic->qos != qos){
			തുടരുക;
		}
		എങ്കിൽ(cur_topic->topic != ശൂന്യ && topic != ശൂന്യ){
			എങ്കിൽ(strcmp(cur_topic->topic, topic)){
				തുടരുക;
			}
		}
		എങ്കിൽ(cur_topic->local_prefix != ശൂന്യ && local_prefix != ശൂന്യ){
			എങ്കിൽ(strcmp(cur_topic->local_prefix, local_prefix)){
				തുടരുക;
			}
		}
		എങ്കിൽ(cur_topic->remote_prefix != ശൂന്യ && remote_prefix != ശൂന്യ){
			എങ്കിൽ(strcmp(cur_topic->remote_prefix, remote_prefix)){
				തുടരുക;
			}
		}
		found = true;
		നിർത്തുക;
	}
	എങ്കിൽ(!found){
		cur_topic = ശൂന്യ;
	}

	തിരികെ cur_topic;
}


ശൂന്യം bridge__cleanup_topics(ഘടന mosquitto__bridge *bridge)
{
	ഘടന mosquitto__bridge_topic *topic, *topic_tmp;

	എങ്കിൽ(!bridge){
		തിരികെ;
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
പൂർണ്ണസംഖ്യ bridge__add_topic(ഘടന mosquitto__bridge *bridge, സ്ഥിരം അക്ഷരം *topic, എണ്ണൽ mosquitto__bridge_direction direction, uint8_t qos, സ്ഥിരം അക്ഷരം *local_prefix, സ്ഥിരം അക്ഷരം *remote_prefix)
{
	ഘടന mosquitto__bridge_topic *cur_topic;

	എങ്കിൽ(bridge == ശൂന്യ){
		തിരികെ MOSQ_ERR_INVAL;
	}
	എങ്കിൽ(direction != bd_out && direction != bd_in && direction != bd_both){
		തിരികെ MOSQ_ERR_INVAL;
	}
	എങ്കിൽ(qos > 2){
		തിരികെ MOSQ_ERR_INVAL;
	}
	എങ്കിൽ(local_prefix && mosquitto_pub_topic_check(local_prefix)){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", local_prefix);
		തിരികെ MOSQ_ERR_INVAL;
	}
	എങ്കിൽ(remote_prefix && mosquitto_pub_topic_check(remote_prefix)){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Invalid bridge topic remote prefix '%s'.", remote_prefix);
		തിരികെ MOSQ_ERR_INVAL;
	}
	എങ്കിൽ((topic == ശൂന്യ || !strcmp(topic, "\"\"")) &&
			(local_prefix == ശൂന്യ || remote_prefix == ശൂന്യ)){

		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Invalid bridge remapping.");
		തിരികെ MOSQ_ERR_INVAL;
	}

	എങ്കിൽ(bridge__find_topic(bridge, topic, direction, qos, local_prefix, remote_prefix) != ശൂന്യ){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Duplicate bridge topic '%s', skipping", topic);
		തിരികെ MOSQ_ERR_SUCCESS;
	}

	bridge->topic_എണ്ണം++;
	cur_topic = mosquitto_calloc(1, വലുപ്പം(ഘടന mosquitto__bridge_topic));
	എങ്കിൽ(cur_topic == ശൂന്യ){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Out of memory.");
		തിരികെ MOSQ_ERR_NOMEM;
	}
	cur_topic->next = ശൂന്യ;

	cur_topic->direction = direction;
	cur_topic->qos = qos;
	cur_topic->local_prefix = ശൂന്യ;
	cur_topic->remote_prefix = ശൂന്യ;

	എങ്കിൽ(topic == ശൂന്യ || !strcmp(topic, "\"\"")){
		cur_topic->topic = ശൂന്യ;
	}അല്ലെങ്കിൽ{
		cur_topic->topic = mosquitto_strdup(topic);
		എങ്കിൽ(cur_topic->topic == ശൂന്യ){
			പോകുക error;
		}
	}

	എങ്കിൽ(local_prefix || remote_prefix){
		bridge->topic_remapping = true;
		എങ്കിൽ(local_prefix){
			എങ്കിൽ(bridge__create_prefix(&cur_topic->local_prefix, cur_topic->topic, local_prefix, "local")){
				പോകുക error;
			}
		}
		എങ്കിൽ(remote_prefix){
			എങ്കിൽ(bridge__create_prefix(&cur_topic->remote_prefix, cur_topic->topic, remote_prefix, "local")){
				പോകുക error;
			}
		}
	}

	എങ്കിൽ(bridge__create_remap_topic(cur_topic->local_prefix,
			cur_topic->topic, &cur_topic->local_topic)){

		പോകുക error;
	}

	എങ്കിൽ(bridge__create_remap_topic(cur_topic->remote_prefix,
			cur_topic->topic, &cur_topic->remote_topic)){

		പോകുക error;
	}

	LL_APPEND(bridge->topics, cur_topic);

	തിരികെ MOSQ_ERR_SUCCESS;

error:
	mosquitto_FREE(cur_topic->local_prefix);
	mosquitto_FREE(cur_topic->remote_prefix);
	mosquitto_FREE(cur_topic->local_topic);
	mosquitto_FREE(cur_topic->remote_topic);
	mosquitto_FREE(cur_topic->topic);
	mosquitto_FREE(cur_topic);
	log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Out of memory.");
	തിരികെ MOSQ_ERR_NOMEM;
}


പൂർണ്ണസംഖ്യ bridge__remap_topic_in(ഘടന mosquitto *context, അക്ഷരം **topic)
{
	ഘടന mosquitto__bridge_topic *cur_topic;
	bool match;

	എങ്കിൽ(context->bridge && context->bridge->topics && context->bridge->topic_remapping){
		LL_FOREACH(context->bridge->topics, cur_topic){
			എങ്കിൽ((cur_topic->direction == bd_both || cur_topic->direction == bd_in)
					&& (cur_topic->remote_prefix || cur_topic->local_prefix)){

				/* Topic mapping required on this topic if the message matches */

				പൂർണ്ണസംഖ്യ rc = mosquitto_topic_matches_sub(cur_topic->remote_topic, *topic, &match);
				എങ്കിൽ(rc){
					mosquitto_FREE(*topic);
					തിരികെ rc;
				}
				എങ്കിൽ(match){
					അക്ഷരം *topic_temp;

					എങ്കിൽ(cur_topic->remote_prefix){
						/* This prefix needs removing. */
						എങ്കിൽ(!strncmp(cur_topic->remote_prefix, *topic, strlen(cur_topic->remote_prefix))){
							topic_temp = mosquitto_strdup((*topic)+strlen(cur_topic->remote_prefix));
							എങ്കിൽ(!topic_temp){
								mosquitto_FREE(*topic);
								തിരികെ MOSQ_ERR_NOMEM;
							}
							mosquitto_FREE(*topic);
							*topic = topic_temp;
						}
					}

					എങ്കിൽ(cur_topic->local_prefix){
						/* This prefix needs adding. */
						size_t len = strlen(*topic) + strlen(cur_topic->local_prefix)+1;
						topic_temp = mosquitto_malloc(len+1);
						എങ്കിൽ(!topic_temp){
							mosquitto_FREE(*topic);
							തിരികെ MOSQ_ERR_NOMEM;
						}
						snprintf(topic_temp, len, "%s%s", cur_topic->local_prefix, *topic);
						topic_temp[len] = '\0';

						mosquitto_FREE(*topic);
						*topic = topic_temp;
					}
					നിർത്തുക;
				}
			}
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}

#endif
