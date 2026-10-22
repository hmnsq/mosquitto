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
	ᮚᮦᮔ᮪(prefix){
		ᮚᮦᮔ᮪(topic){
			size_t len = strlen(topic) + strlen(prefix)+1;
			*remap_topic = mosquitto_malloc(len+1);
			ᮚᮦᮔ᮪(!(*remap_topic)){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
				ᮝᮜᮤ MOSQ_ERR_NOMEM;
			}
			snprintf(*remap_topic, len+1, "%s%s", prefix, topic);
			(*remap_topic)[len] = '\0';
		}ᮜᮤᮚ{
			*remap_topic = mosquitto_strdup(prefix);
			ᮚᮦᮔ᮪(!(*remap_topic)){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
				ᮝᮜᮤ MOSQ_ERR_NOMEM;
			}
		}
	}ᮜᮤᮚ{
		*remap_topic = mosquitto_strdup(topic);
		ᮚᮦᮔ᮪(!(*remap_topic)){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
			ᮝᮜᮤ MOSQ_ERR_NOMEM;
		}
	}
	ᮝᮜᮤ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka bridge__create_prefix(var_na **full_prefix, const var_na *topic, const var_na *prefix, const var_na *direction)
{
	size_t len;

	ᮚᮦᮔ᮪(!prefix || strlen(prefix) != 0){
		ᮚᮦᮔ᮪(mosquitto_pub_topic_check(prefix) != MOSQ_ERR_SUCCESS){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", prefix);
			ᮝᮜᮤ MOSQ_ERR_INVAL;
		}
	}

	ᮚᮦᮔ᮪(topic){
		len = strlen(topic) + strlen(prefix) + 1;
	}ᮜᮤᮚ{
		len = strlen(prefix) + 1;
	}
	*full_prefix = mosquitto_malloc(len);
	ᮚᮦᮔ᮪(*full_prefix == NULL){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		ᮝᮜᮤ MOSQ_ERR_NOMEM;
	}

	ᮚᮦᮔ᮪(topic){
		/* Print full_prefix+pattern to check for validity */
		snprintf(*full_prefix, len, "%s%s", prefix, topic);
	}ᮜᮤᮚ{
		snprintf(*full_prefix, len, "%s", prefix);
	}

	ᮚᮦᮔ᮪(mosquitto_sub_topic_check(*full_prefix) != MOSQ_ERR_SUCCESS){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Invalid bridge topic %s prefix and pattern combination '%s'.",
				direction, *full_prefix);

		ᮝᮜᮤ MOSQ_ERR_INVAL;
	}

	/* Print just the prefix for storage */
	snprintf(*full_prefix, len, "%s", prefix);

	ᮝᮜᮤ MOSQ_ERR_SUCCESS;
}


static sa_nracanaa mosquitto__bridge_topic *bridge__find_topic(sa_nracanaa mosquitto__bridge *bridge, const var_na *topic, ga_nanaa mosquitto__bridge_direction direction, uint8_t qos, const var_na *local_prefix, const var_na *remote_prefix)
{
	sa_nracanaa mosquitto__bridge_topic *cur_topic = NULL;
	bool found = false;

	LL_FOREACH(bridge->topics, cur_topic){
		ᮚᮦᮔ᮪(cur_topic->direction != direction){
			continue;
		}
		ᮚᮦᮔ᮪(cur_topic->qos != qos){
			continue;
		}
		ᮚᮦᮔ᮪(cur_topic->topic != NULL && topic != NULL){
			ᮚᮦᮔ᮪(strcmp(cur_topic->topic, topic)){
				continue;
			}
		}
		ᮚᮦᮔ᮪(cur_topic->local_prefix != NULL && local_prefix != NULL){
			ᮚᮦᮔ᮪(strcmp(cur_topic->local_prefix, local_prefix)){
				continue;
			}
		}
		ᮚᮦᮔ᮪(cur_topic->remote_prefix != NULL && remote_prefix != NULL){
			ᮚᮦᮔ᮪(strcmp(cur_topic->remote_prefix, remote_prefix)){
				continue;
			}
		}
		found = true;
		bha_nga;
	}
	ᮚᮦᮔ᮪(!found){
		cur_topic = NULL;
	}

	ᮝᮜᮤ cur_topic;
}


void bridge__cleanup_topics(sa_nracanaa mosquitto__bridge *bridge)
{
	sa_nracanaa mosquitto__bridge_topic *topic, *topic_tmp;

	ᮚᮦᮔ᮪(!bridge){
		ᮝᮜᮤ;
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

	ᮚᮦᮔ᮪(bridge == NULL){
		ᮝᮜᮤ MOSQ_ERR_INVAL;
	}
	ᮚᮦᮔ᮪(direction != bd_out && direction != bd_in && direction != bd_both){
		ᮝᮜᮤ MOSQ_ERR_INVAL;
	}
	ᮚᮦᮔ᮪(qos > 2){
		ᮝᮜᮤ MOSQ_ERR_INVAL;
	}
	ᮚᮦᮔ᮪(local_prefix && mosquitto_pub_topic_check(local_prefix)){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", local_prefix);
		ᮝᮜᮤ MOSQ_ERR_INVAL;
	}
	ᮚᮦᮔ᮪(remote_prefix && mosquitto_pub_topic_check(remote_prefix)){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge topic remote prefix '%s'.", remote_prefix);
		ᮝᮜᮤ MOSQ_ERR_INVAL;
	}
	ᮚᮦᮔ᮪((topic == NULL || !strcmp(topic, "\"\"")) &&
			(local_prefix == NULL || remote_prefix == NULL)){

		log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge remapping.");
		ᮝᮜᮤ MOSQ_ERR_INVAL;
	}

	ᮚᮦᮔ᮪(bridge__find_topic(bridge, topic, direction, qos, local_prefix, remote_prefix) != NULL){
		log__printf(NULL, MOSQ_LOG_INFO, "Duplicate bridge topic '%s', skipping", topic);
		ᮝᮜᮤ MOSQ_ERR_SUCCESS;
	}

	bridge->topic_count++;
	cur_topic = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__bridge_topic));
	ᮚᮦᮔ᮪(cur_topic == NULL){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		ᮝᮜᮤ MOSQ_ERR_NOMEM;
	}
	cur_topic->next = NULL;

	cur_topic->direction = direction;
	cur_topic->qos = qos;
	cur_topic->local_prefix = NULL;
	cur_topic->remote_prefix = NULL;

	ᮚᮦᮔ᮪(topic == NULL || !strcmp(topic, "\"\"")){
		cur_topic->topic = NULL;
	}ᮜᮤᮚ{
		cur_topic->topic = mosquitto_strdup(topic);
		ᮚᮦᮔ᮪(cur_topic->topic == NULL){
			goto error;
		}
	}

	ᮚᮦᮔ᮪(local_prefix || remote_prefix){
		bridge->topic_remapping = true;
		ᮚᮦᮔ᮪(local_prefix){
			ᮚᮦᮔ᮪(bridge__create_prefix(&cur_topic->local_prefix, cur_topic->topic, local_prefix, "local")){
				goto error;
			}
		}
		ᮚᮦᮔ᮪(remote_prefix){
			ᮚᮦᮔ᮪(bridge__create_prefix(&cur_topic->remote_prefix, cur_topic->topic, remote_prefix, "local")){
				goto error;
			}
		}
	}

	ᮚᮦᮔ᮪(bridge__create_remap_topic(cur_topic->local_prefix,
			cur_topic->topic, &cur_topic->local_topic)){

		goto error;
	}

	ᮚᮦᮔ᮪(bridge__create_remap_topic(cur_topic->remote_prefix,
			cur_topic->topic, &cur_topic->remote_topic)){

		goto error;
	}

	LL_APPEND(bridge->topics, cur_topic);

	ᮝᮜᮤ MOSQ_ERR_SUCCESS;

error:
	mosquitto_FREE(cur_topic->local_prefix);
	mosquitto_FREE(cur_topic->remote_prefix);
	mosquitto_FREE(cur_topic->local_topic);
	mosquitto_FREE(cur_topic->remote_topic);
	mosquitto_FREE(cur_topic->topic);
	mosquitto_FREE(cur_topic);
	log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
	ᮝᮜᮤ MOSQ_ERR_NOMEM;
}


puur_naa_nka bridge__remap_topic_in(sa_nracanaa mosquitto *context, var_na **topic)
{
	sa_nracanaa mosquitto__bridge_topic *cur_topic;
	bool match;

	ᮚᮦᮔ᮪(context->bridge && context->bridge->topics && context->bridge->topic_remapping){
		LL_FOREACH(context->bridge->topics, cur_topic){
			ᮚᮦᮔ᮪((cur_topic->direction == bd_both || cur_topic->direction == bd_in)
					&& (cur_topic->remote_prefix || cur_topic->local_prefix)){

				/* Topic mapping required on this topic if the message matches */

				puur_naa_nka rc = mosquitto_topic_matches_sub(cur_topic->remote_topic, *topic, &match);
				ᮚᮦᮔ᮪(rc){
					mosquitto_FREE(*topic);
					ᮝᮜᮤ rc;
				}
				ᮚᮦᮔ᮪(match){
					var_na *topic_temp;

					ᮚᮦᮔ᮪(cur_topic->remote_prefix){
						/* This prefix needs removing. */
						ᮚᮦᮔ᮪(!strncmp(cur_topic->remote_prefix, *topic, strlen(cur_topic->remote_prefix))){
							topic_temp = mosquitto_strdup((*topic)+strlen(cur_topic->remote_prefix));
							ᮚᮦᮔ᮪(!topic_temp){
								mosquitto_FREE(*topic);
								ᮝᮜᮤ MOSQ_ERR_NOMEM;
							}
							mosquitto_FREE(*topic);
							*topic = topic_temp;
						}
					}

					ᮚᮦᮔ᮪(cur_topic->local_prefix){
						/* This prefix needs adding. */
						size_t len = strlen(*topic) + strlen(cur_topic->local_prefix)+1;
						topic_temp = mosquitto_malloc(len+1);
						ᮚᮦᮔ᮪(!topic_temp){
							mosquitto_FREE(*topic);
							ᮝᮜᮤ MOSQ_ERR_NOMEM;
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

	ᮝᮜᮤ MOSQ_ERR_SUCCESS;
}

#endif
