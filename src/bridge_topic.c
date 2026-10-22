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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utlist.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE


ਸਥਿਰ ਪੂਰਨ bridge__create_remap_topic(ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, ਅਚਲ ਅੱਖਰ *topic, ਅੱਖਰ **remap_topic)
{
	ਜੇ(ਅਗੇਤਰ){
		ਜੇ(topic){
			size_t len = ਲੜੀ_ਲੰਬਾਈ(topic) + ਲੜੀ_ਲੰਬਾਈ(ਅਗੇਤਰ)+1;
			*remap_topic = mosquitto_malloc(len+1);
			ਜੇ(!(*remap_topic)){
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
				ਵਾਪਸ MOSQ_ERR_NOMEM;
			}
			snprintf(*remap_topic, len+1, "%s%s", ਅਗੇਤਰ, topic);
			(*remap_topic)[len] = '\0';
		}ਨਹੀਂਤਾਂ{
			*remap_topic = mosquitto_strdup(ਅਗੇਤਰ);
			ਜੇ(!(*remap_topic)){
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
				ਵਾਪਸ MOSQ_ERR_NOMEM;
			}
		}
	}ਨਹੀਂਤਾਂ{
		*remap_topic = mosquitto_strdup(topic);
		ਜੇ(!(*remap_topic)){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ bridge_create_ਅਗੇਤਰ(ਅੱਖਰ **ਭਰਿਆ_ਅਗੇਤਰ, ਅਚਲ ਅੱਖਰ *topic, ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, ਅਚਲ ਅੱਖਰ *direction)
{
	size_t len;

	ਜੇ(!ਅਗੇਤਰ || ਲੜੀ_ਲੰਬਾਈ(ਅਗੇਤਰ) != 0){
		ਜੇ(mosquitto_pub_topic_check(ਅਗੇਤਰ) != MOSQ_ERR_SUCCESS){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", ਅਗੇਤਰ);
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
	}

	ਜੇ(topic){
		len = ਲੜੀ_ਲੰਬਾਈ(topic) + ਲੜੀ_ਲੰਬਾਈ(ਅਗੇਤਰ) + 1;
	}ਨਹੀਂਤਾਂ{
		len = ਲੜੀ_ਲੰਬਾਈ(ਅਗੇਤਰ) + 1;
	}
	*ਭਰਿਆ_ਅਗੇਤਰ = mosquitto_malloc(len);
	ਜੇ(*ਭਰਿਆ_ਅਗੇਤਰ == ਨਿਰਾ){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	ਜੇ(topic){
		/* Print full_prefix+pattern to check for validity */
		snprintf(*ਭਰਿਆ_ਅਗੇਤਰ, len, "%s%s", ਅਗੇਤਰ, topic);
	}ਨਹੀਂਤਾਂ{
		snprintf(*ਭਰਿਆ_ਅਗੇਤਰ, len, "%s", ਅਗੇਤਰ);
	}

	ਜੇ(mosquitto_sub_topic_check(*ਭਰਿਆ_ਅਗੇਤਰ) != MOSQ_ERR_SUCCESS){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR,
				"Error: Invalid bridge topic %s prefix and pattern combination '%s'.",
				direction, *ਭਰਿਆ_ਅਗੇਤਰ);

		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	/* Print just the prefix for storage */
	snprintf(*ਭਰਿਆ_ਅਗੇਤਰ, len, "%s", ਅਗੇਤਰ);

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਬਣਤਰ mosquitto__bridge_topic *bridge_ਲੱਭੋ_topic(ਬਣਤਰ mosquitto__bridge *bridge, ਅਚਲ ਅੱਖਰ *topic, ਗਿਣਤੀ mosquitto__bridge_direction direction, uint8_t qos, ਅਚਲ ਅੱਖਰ *local_ਅਗੇਤਰ, ਅਚਲ ਅੱਖਰ *remote_ਅਗੇਤਰ)
{
	ਬਣਤਰ mosquitto__bridge_topic *cur_topic = ਨਿਰਾ;
	bool ਮਿਲਿਆ = false;

	LL_FOREACH(bridge->topics, cur_topic){
		ਜੇ(cur_topic->direction != direction){
			ਜਾਰੀ;
		}
		ਜੇ(cur_topic->qos != qos){
			ਜਾਰੀ;
		}
		ਜੇ(cur_topic->topic != ਨਿਰਾ && topic != ਨਿਰਾ){
			ਜੇ(strcmp(cur_topic->topic, topic)){
				ਜਾਰੀ;
			}
		}
		ਜੇ(cur_topic->local_ਅਗੇਤਰ != ਨਿਰਾ && local_ਅਗੇਤਰ != ਨਿਰਾ){
			ਜੇ(strcmp(cur_topic->local_ਅਗੇਤਰ, local_ਅਗੇਤਰ)){
				ਜਾਰੀ;
			}
		}
		ਜੇ(cur_topic->remote_ਅਗੇਤਰ != ਨਿਰਾ && remote_ਅਗੇਤਰ != ਨਿਰਾ){
			ਜੇ(strcmp(cur_topic->remote_ਅਗੇਤਰ, remote_ਅਗੇਤਰ)){
				ਜਾਰੀ;
			}
		}
		ਮਿਲਿਆ = true;
		ਰੋਕੋ;
	}
	ਜੇ(!ਮਿਲਿਆ){
		cur_topic = ਨਿਰਾ;
	}

	ਵਾਪਸ cur_topic;
}


ਖਾਲੀ bridge__cleanup_topics(ਬਣਤਰ mosquitto__bridge *bridge)
{
	ਬਣਤਰ mosquitto__bridge_topic *topic, *topic_tmp;

	ਜੇ(!bridge){
		ਵਾਪਸ;
	}

	LL_FOREACH_ਸੁਰੱਖਿਅਤ(bridge->topics, topic, topic_tmp){
		LL_DELETE(bridge->topics, topic);
		mosquitto_free(topic->local_ਅਗੇਤਰ);
		mosquitto_free(topic->remote_ਅਗੇਤਰ);
		mosquitto_free(topic->local_topic);
		mosquitto_free(topic->remote_topic);
		mosquitto_free(topic->topic);
		mosquitto_free(topic);
	}
}


/* topic <topic> [[[out | in | both] qos-level] local-prefix remote-prefix] */
ਪੂਰਨ bridge_ਜੋੜੋ_topic(ਬਣਤਰ mosquitto__bridge *bridge, ਅਚਲ ਅੱਖਰ *topic, ਗਿਣਤੀ mosquitto__bridge_direction direction, uint8_t qos, ਅਚਲ ਅੱਖਰ *local_ਅਗੇਤਰ, ਅਚਲ ਅੱਖਰ *remote_ਅਗੇਤਰ)
{
	ਬਣਤਰ mosquitto__bridge_topic *cur_topic;

	ਜੇ(bridge == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}
	ਜੇ(direction != bd_ਬਾਹਰਲਾ && direction != bd_ਵਿੱਚ && direction != bd_both){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}
	ਜੇ(qos > 2){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}
	ਜੇ(local_ਅਗੇਤਰ && mosquitto_pub_topic_check(local_ਅਗੇਤਰ)){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", local_ਅਗੇਤਰ);
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}
	ਜੇ(remote_ਅਗੇਤਰ && mosquitto_pub_topic_check(remote_ਅਗੇਤਰ)){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid bridge topic remote prefix '%s'.", remote_ਅਗੇਤਰ);
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}
	ਜੇ((topic == ਨਿਰਾ || !strcmp(topic, "\"\"")) &&
			(local_ਅਗੇਤਰ == ਨਿਰਾ || remote_ਅਗੇਤਰ == ਨਿਰਾ)){

		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid bridge remapping.");
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	ਜੇ(bridge_ਲੱਭੋ_topic(bridge, topic, direction, qos, local_ਅਗੇਤਰ, remote_ਅਗੇਤਰ) != ਨਿਰਾ){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Duplicate bridge topic '%s', skipping", topic);
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

	bridge->topic_ਗਿਣਤੀ++;
	cur_topic = mosquitto_calloc(1, ਆਕਾਰ(ਬਣਤਰ mosquitto__bridge_topic));
	ਜੇ(cur_topic == ਨਿਰਾ){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}
	cur_topic->ਅਗਲਾ = ਨਿਰਾ;

	cur_topic->direction = direction;
	cur_topic->qos = qos;
	cur_topic->local_ਅਗੇਤਰ = ਨਿਰਾ;
	cur_topic->remote_ਅਗੇਤਰ = ਨਿਰਾ;

	ਜੇ(topic == ਨਿਰਾ || !strcmp(topic, "\"\"")){
		cur_topic->topic = ਨਿਰਾ;
	}ਨਹੀਂਤਾਂ{
		cur_topic->topic = mosquitto_strdup(topic);
		ਜੇ(cur_topic->topic == ਨਿਰਾ){
			ਜਾਓ error;
		}
	}

	ਜੇ(local_ਅਗੇਤਰ || remote_ਅਗੇਤਰ){
		bridge->topic_remapping = true;
		ਜੇ(local_ਅਗੇਤਰ){
			ਜੇ(bridge_create_ਅਗੇਤਰ(&cur_topic->local_ਅਗੇਤਰ, cur_topic->topic, local_ਅਗੇਤਰ, "local")){
				ਜਾਓ error;
			}
		}
		ਜੇ(remote_ਅਗੇਤਰ){
			ਜੇ(bridge_create_ਅਗੇਤਰ(&cur_topic->remote_ਅਗੇਤਰ, cur_topic->topic, remote_ਅਗੇਤਰ, "local")){
				ਜਾਓ error;
			}
		}
	}

	ਜੇ(bridge__create_remap_topic(cur_topic->local_ਅਗੇਤਰ,
			cur_topic->topic, &cur_topic->local_topic)){

		ਜਾਓ error;
	}

	ਜੇ(bridge__create_remap_topic(cur_topic->remote_ਅਗੇਤਰ,
			cur_topic->topic, &cur_topic->remote_topic)){

		ਜਾਓ error;
	}

	LL_APPEND(bridge->topics, cur_topic);

	ਵਾਪਸ MOSQ_ERR_SUCCESS;

error:
	mosquitto_FREE(cur_topic->local_ਅਗੇਤਰ);
	mosquitto_FREE(cur_topic->remote_ਅਗੇਤਰ);
	mosquitto_FREE(cur_topic->local_topic);
	mosquitto_FREE(cur_topic->remote_topic);
	mosquitto_FREE(cur_topic->topic);
	mosquitto_FREE(cur_topic);
	log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
	ਵਾਪਸ MOSQ_ERR_NOMEM;
}


ਪੂਰਨ bridge_remap_topic_ਵਿੱਚ(ਬਣਤਰ mosquitto *context, ਅੱਖਰ **topic)
{
	ਬਣਤਰ mosquitto__bridge_topic *cur_topic;
	bool ਮੇਲ;

	ਜੇ(context->bridge && context->bridge->topics && context->bridge->topic_remapping){
		LL_FOREACH(context->bridge->topics, cur_topic){
			ਜੇ((cur_topic->direction == bd_both || cur_topic->direction == bd_ਵਿੱਚ)
					&& (cur_topic->remote_ਅਗੇਤਰ || cur_topic->local_ਅਗੇਤਰ)){

				/* Topic mapping required on this topic if the message matches */

				ਪੂਰਨ rc = mosquitto_topic_matches_sub(cur_topic->remote_topic, *topic, &ਮੇਲ);
				ਜੇ(rc){
					mosquitto_FREE(*topic);
					ਵਾਪਸ rc;
				}
				ਜੇ(ਮੇਲ){
					ਅੱਖਰ *topic_ਆਰਜ਼ੀ;

					ਜੇ(cur_topic->remote_ਅਗੇਤਰ){
						/* This prefix needs removing. */
						ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(cur_topic->remote_ਅਗੇਤਰ, *topic, ਲੜੀ_ਲੰਬਾਈ(cur_topic->remote_ਅਗੇਤਰ))){
							topic_ਆਰਜ਼ੀ = mosquitto_strdup((*topic)+ਲੜੀ_ਲੰਬਾਈ(cur_topic->remote_ਅਗੇਤਰ));
							ਜੇ(!topic_ਆਰਜ਼ੀ){
								mosquitto_FREE(*topic);
								ਵਾਪਸ MOSQ_ERR_NOMEM;
							}
							mosquitto_FREE(*topic);
							*topic = topic_ਆਰਜ਼ੀ;
						}
					}

					ਜੇ(cur_topic->local_ਅਗੇਤਰ){
						/* This prefix needs adding. */
						size_t len = ਲੜੀ_ਲੰਬਾਈ(*topic) + ਲੜੀ_ਲੰਬਾਈ(cur_topic->local_ਅਗੇਤਰ)+1;
						topic_ਆਰਜ਼ੀ = mosquitto_malloc(len+1);
						ਜੇ(!topic_ਆਰਜ਼ੀ){
							mosquitto_FREE(*topic);
							ਵਾਪਸ MOSQ_ERR_NOMEM;
						}
						snprintf(topic_ਆਰਜ਼ੀ, len, "%s%s", cur_topic->local_ਅਗੇਤਰ, *topic);
						topic_ਆਰਜ਼ੀ[len] = '\0';

						mosquitto_FREE(*topic);
						*topic = topic_ਆਰਜ਼ੀ;
					}
					ਰੋਕੋ;
				}
			}
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}

#ਜੇ_ਅੰਤ
