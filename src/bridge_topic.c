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

#include "config.h"

#include "mosquitto.h"
#include "mosquitto_broker_internal.h"
#include "utlist.h"

#ifdef WITH_BRIDGE


static puur_naa_nka bridge__create_remap_topic(const var_na *prefix, const var_na *topic, var_na **remap_topic)
{
	ᯀᯮᯝ(prefix){
		ᯀᯮᯝ(topic){
			size_t len = strlen(topic) + strlen(prefix)+1;
			*remap_topic = mosquitto_malloc(len+1);
			ᯀᯮᯝ(!(*remap_topic)){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
				ᯀᯮᯞ MOSQ_ERR_NOMEM;
			}
			snprintf(*remap_topic, len+1, "%s%s", prefix, topic);
			(*remap_topic)[len] = '\0';
		}ᯀᯮᯝᯂᯪ{
			*remap_topic = mosquitto_strdup(prefix);
			ᯀᯮᯝ(!(*remap_topic)){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
				ᯀᯮᯞ MOSQ_ERR_NOMEM;
			}
		}
	}ᯀᯮᯝᯂᯪ{
		*remap_topic = mosquitto_strdup(topic);
		ᯀᯮᯝ(!(*remap_topic)){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
			ᯀᯮᯞ MOSQ_ERR_NOMEM;
		}
	}
	ᯀᯮᯞ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka bridge__create_prefix(var_na **full_prefix, const var_na *topic, const var_na *prefix, const var_na *direction)
{
	size_t len;

	ᯀᯮᯝ(!prefix || strlen(prefix) != 0){
		ᯀᯮᯝ(mosquitto_pub_topic_check(prefix) != MOSQ_ERR_SUCCESS){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", prefix);
			ᯀᯮᯞ MOSQ_ERR_INVAL;
		}
	}

	ᯀᯮᯝ(topic){
		len = strlen(topic) + strlen(prefix) + 1;
	}ᯀᯮᯝᯂᯪ{
		len = strlen(prefix) + 1;
	}
	*full_prefix = mosquitto_malloc(len);
	ᯀᯮᯝ(*full_prefix == NULL){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		ᯀᯮᯞ MOSQ_ERR_NOMEM;
	}

	ᯀᯮᯝ(topic){
		/* Print full_prefix+pattern to check for validity */
		snprintf(*full_prefix, len, "%s%s", prefix, topic);
	}ᯀᯮᯝᯂᯪ{
		snprintf(*full_prefix, len, "%s", prefix);
	}

	ᯀᯮᯝ(mosquitto_sub_topic_check(*full_prefix) != MOSQ_ERR_SUCCESS){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Invalid bridge topic %s prefix and pattern combination '%s'.",
				direction, *full_prefix);

		ᯀᯮᯞ MOSQ_ERR_INVAL;
	}

	/* Print just the prefix for storage */
	snprintf(*full_prefix, len, "%s", prefix);

	ᯀᯮᯞ MOSQ_ERR_SUCCESS;
}


static sa_nracanaa mosquitto__bridge_topic *bridge__find_topic(sa_nracanaa mosquitto__bridge *bridge, const var_na *topic, ga_nanaa mosquitto__bridge_direction direction, uint8_t qos, const var_na *local_prefix, const var_na *remote_prefix)
{
	sa_nracanaa mosquitto__bridge_topic *cur_topic = NULL;
	bool found = false;

	LL_FOREACH(bridge->topics, cur_topic){
		ᯀᯮᯝ(cur_topic->direction != direction){
			continue;
		}
		ᯀᯮᯝ(cur_topic->qos != qos){
			continue;
		}
		ᯀᯮᯝ(cur_topic->topic != NULL && topic != NULL){
			ᯀᯮᯝ(strcmp(cur_topic->topic, topic)){
				continue;
			}
		}
		ᯀᯮᯝ(cur_topic->local_prefix != NULL && local_prefix != NULL){
			ᯀᯮᯝ(strcmp(cur_topic->local_prefix, local_prefix)){
				continue;
			}
		}
		ᯀᯮᯝ(cur_topic->remote_prefix != NULL && remote_prefix != NULL){
			ᯀᯮᯝ(strcmp(cur_topic->remote_prefix, remote_prefix)){
				continue;
			}
		}
		found = true;
		bha_nga;
	}
	ᯀᯮᯝ(!found){
		cur_topic = NULL;
	}

	ᯀᯮᯞ cur_topic;
}


void bridge__cleanup_topics(sa_nracanaa mosquitto__bridge *bridge)
{
	sa_nracanaa mosquitto__bridge_topic *topic, *topic_tmp;

	ᯀᯮᯝ(!bridge){
		ᯀᯮᯞ;
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
puur_naa_nka bridge__add_topic(sa_nracanaa mosquitto__bridge *bridge, const var_na *topic, ga_nanaa mosquitto__bridge_direction direction, uint8_t qos, const var_na *local_prefix, const var_na *remote_prefix)
{
	sa_nracanaa mosquitto__bridge_topic *cur_topic;

	ᯀᯮᯝ(bridge == NULL){
		ᯀᯮᯞ MOSQ_ERR_INVAL;
	}
	ᯀᯮᯝ(direction != bd_out && direction != bd_in && direction != bd_both){
		ᯀᯮᯞ MOSQ_ERR_INVAL;
	}
	ᯀᯮᯝ(qos > 2){
		ᯀᯮᯞ MOSQ_ERR_INVAL;
	}
	ᯀᯮᯝ(local_prefix && mosquitto_pub_topic_check(local_prefix)){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", local_prefix);
		ᯀᯮᯞ MOSQ_ERR_INVAL;
	}
	ᯀᯮᯝ(remote_prefix && mosquitto_pub_topic_check(remote_prefix)){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge topic remote prefix '%s'.", remote_prefix);
		ᯀᯮᯞ MOSQ_ERR_INVAL;
	}
	ᯀᯮᯝ((topic == NULL || !strcmp(topic, "\"\"")) &&
			(local_prefix == NULL || remote_prefix == NULL)){

		log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge remapping.");
		ᯀᯮᯞ MOSQ_ERR_INVAL;
	}

	ᯀᯮᯝ(bridge__find_topic(bridge, topic, direction, qos, local_prefix, remote_prefix) != NULL){
		log__printf(NULL, MOSQ_LOG_INFO, "Duplicate bridge topic '%s', skipping", topic);
		ᯀᯮᯞ MOSQ_ERR_SUCCESS;
	}

	bridge->topic_count++;
	cur_topic = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__bridge_topic));
	ᯀᯮᯝ(cur_topic == NULL){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		ᯀᯮᯞ MOSQ_ERR_NOMEM;
	}
	cur_topic->next = NULL;

	cur_topic->direction = direction;
	cur_topic->qos = qos;
	cur_topic->local_prefix = NULL;
	cur_topic->remote_prefix = NULL;

	ᯀᯮᯝ(topic == NULL || !strcmp(topic, "\"\"")){
		cur_topic->topic = NULL;
	}ᯀᯮᯝᯂᯪ{
		cur_topic->topic = mosquitto_strdup(topic);
		ᯀᯮᯝ(cur_topic->topic == NULL){
			goto error;
		}
	}

	ᯀᯮᯝ(local_prefix || remote_prefix){
		bridge->topic_remapping = true;
		ᯀᯮᯝ(local_prefix){
			ᯀᯮᯝ(bridge__create_prefix(&cur_topic->local_prefix, cur_topic->topic, local_prefix, "local")){
				goto error;
			}
		}
		ᯀᯮᯝ(remote_prefix){
			ᯀᯮᯝ(bridge__create_prefix(&cur_topic->remote_prefix, cur_topic->topic, remote_prefix, "local")){
				goto error;
			}
		}
	}

	ᯀᯮᯝ(bridge__create_remap_topic(cur_topic->local_prefix,
			cur_topic->topic, &cur_topic->local_topic)){

		goto error;
	}

	ᯀᯮᯝ(bridge__create_remap_topic(cur_topic->remote_prefix,
			cur_topic->topic, &cur_topic->remote_topic)){

		goto error;
	}

	LL_APPEND(bridge->topics, cur_topic);

	ᯀᯮᯞ MOSQ_ERR_SUCCESS;

error:
	mosquitto_FREE(cur_topic->local_prefix);
	mosquitto_FREE(cur_topic->remote_prefix);
	mosquitto_FREE(cur_topic->local_topic);
	mosquitto_FREE(cur_topic->remote_topic);
	mosquitto_FREE(cur_topic->topic);
	mosquitto_FREE(cur_topic);
	log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
	ᯀᯮᯞ MOSQ_ERR_NOMEM;
}


puur_naa_nka bridge__remap_topic_in(sa_nracanaa mosquitto *context, var_na **topic)
{
	sa_nracanaa mosquitto__bridge_topic *cur_topic;
	bool match;

	ᯀᯮᯝ(context->bridge && context->bridge->topics && context->bridge->topic_remapping){
		LL_FOREACH(context->bridge->topics, cur_topic){
			ᯀᯮᯝ((cur_topic->direction == bd_both || cur_topic->direction == bd_in)
					&& (cur_topic->remote_prefix || cur_topic->local_prefix)){

				/* Topic mapping required on this topic if the message matches */

				puur_naa_nka rc = mosquitto_topic_matches_sub(cur_topic->remote_topic, *topic, &match);
				ᯀᯮᯝ(rc){
					mosquitto_FREE(*topic);
					ᯀᯮᯞ rc;
				}
				ᯀᯮᯝ(match){
					var_na *topic_temp;

					ᯀᯮᯝ(cur_topic->remote_prefix){
						/* This prefix needs removing. */
						ᯀᯮᯝ(!strncmp(cur_topic->remote_prefix, *topic, strlen(cur_topic->remote_prefix))){
							topic_temp = mosquitto_strdup((*topic)+strlen(cur_topic->remote_prefix));
							ᯀᯮᯝ(!topic_temp){
								mosquitto_FREE(*topic);
								ᯀᯮᯞ MOSQ_ERR_NOMEM;
							}
							mosquitto_FREE(*topic);
							*topic = topic_temp;
						}
					}

					ᯀᯮᯝ(cur_topic->local_prefix){
						/* This prefix needs adding. */
						size_t len = strlen(*topic) + strlen(cur_topic->local_prefix)+1;
						topic_temp = mosquitto_malloc(len+1);
						ᯀᯮᯝ(!topic_temp){
							mosquitto_FREE(*topic);
							ᯀᯮᯞ MOSQ_ERR_NOMEM;
						}
						snprintf(topic_temp, len, "%s%s", cur_topic->local_prefix, *topic);
						topic_temp[len] = '\0';

						mosquitto_FREE(*topic);
						*topic = topic_temp;
					}
					bha_nga;
				}
			}
		}
	}

	ᯀᯮᯞ MOSQ_ERR_SUCCESS;
}

#endif
