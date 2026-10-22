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

#ضمن "config.h"

#ضمن "mosquitto.h"
#ضمن "mosquitto_broker_internal.h"
#ضمن "utlist.h"

#اگر_تعریف_شده WITH_BRIDGE


ساكن صحيح bridge__create_remap_topic(ثابت حرف *پیشوند, ثابت حرف *topic, حرف **remap_topic)
{
	إذا(پیشوند){
		إذا(topic){
			size_t len = طول_رشته(topic) + طول_رشته(پیشوند)+1;
			*remap_topic = mosquitto_malloc(len+1);
			إذا(!(*remap_topic)){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
				أرجع MOSQ_ERR_NOMEM;
			}
			snprintf(*remap_topic, len+1, "%s%s", پیشوند, topic);
			(*remap_topic)[len] = '\0';
		}وإلا{
			*remap_topic = mosquitto_strdup(پیشوند);
			إذا(!(*remap_topic)){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
				أرجع MOSQ_ERR_NOMEM;
			}
		}
	}وإلا{
		*remap_topic = mosquitto_strdup(topic);
		إذا(!(*remap_topic)){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
			أرجع MOSQ_ERR_NOMEM;
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح bridge_create_پیشوند(حرف **پر_پیشوند, ثابت حرف *topic, ثابت حرف *پیشوند, ثابت حرف *direction)
{
	size_t len;

	إذا(!پیشوند || طول_رشته(پیشوند) != 0){
		إذا(mosquitto_pub_topic_check(پیشوند) != MOSQ_ERR_SUCCESS){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", پیشوند);
			أرجع MOSQ_ERR_INVAL;
		}
	}

	إذا(topic){
		len = طول_رشته(topic) + طول_رشته(پیشوند) + 1;
	}وإلا{
		len = طول_رشته(پیشوند) + 1;
	}
	*پر_پیشوند = mosquitto_malloc(len);
	إذا(*پر_پیشوند == عدم){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		أرجع MOSQ_ERR_NOMEM;
	}

	إذا(topic){
		/* Print full_prefix+pattern to check for validity */
		snprintf(*پر_پیشوند, len, "%s%s", پیشوند, topic);
	}وإلا{
		snprintf(*پر_پیشوند, len, "%s", پیشوند);
	}

	إذا(mosquitto_sub_topic_check(*پر_پیشوند) != MOSQ_ERR_SUCCESS){
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Invalid bridge topic %s prefix and pattern combination '%s'.",
				direction, *پر_پیشوند);

		أرجع MOSQ_ERR_INVAL;
	}

	/* Print just the prefix for storage */
	snprintf(*پر_پیشوند, len, "%s", پیشوند);

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن بنية mosquitto__bridge_topic *bridge_پیدا_کن_topic(بنية mosquitto__bridge *bridge, ثابت حرف *topic, تعداد mosquitto__bridge_direction direction, uint8_t qos, ثابت حرف *local_پیشوند, ثابت حرف *remote_پیشوند)
{
	بنية mosquitto__bridge_topic *cur_topic = عدم;
	bool یافت = false;

	LL_FOREACH(bridge->topics, cur_topic){
		إذا(cur_topic->direction != direction){
			استمر;
		}
		إذا(cur_topic->qos != qos){
			استمر;
		}
		إذا(cur_topic->topic != عدم && topic != عدم){
			إذا(strcmp(cur_topic->topic, topic)){
				استمر;
			}
		}
		إذا(cur_topic->local_پیشوند != عدم && local_پیشوند != عدم){
			إذا(strcmp(cur_topic->local_پیشوند, local_پیشوند)){
				استمر;
			}
		}
		إذا(cur_topic->remote_پیشوند != عدم && remote_پیشوند != عدم){
			إذا(strcmp(cur_topic->remote_پیشوند, remote_پیشوند)){
				استمر;
			}
		}
		یافت = true;
		اكسر;
	}
	إذا(!یافت){
		cur_topic = عدم;
	}

	أرجع cur_topic;
}


فراغ bridge__cleanup_topics(بنية mosquitto__bridge *bridge)
{
	بنية mosquitto__bridge_topic *topic, *topic_tmp;

	إذا(!bridge){
		أرجع;
	}

	LL_FOREACH_امن(bridge->topics, topic, topic_tmp){
		LL_DELETE(bridge->topics, topic);
		mosquitto_free(topic->local_پیشوند);
		mosquitto_free(topic->remote_پیشوند);
		mosquitto_free(topic->local_topic);
		mosquitto_free(topic->remote_topic);
		mosquitto_free(topic->topic);
		mosquitto_free(topic);
	}
}


/* topic <topic> [[[out | in | both] qos-level] local-prefix remote-prefix] */
صحيح bridge_جمع_کن_topic(بنية mosquitto__bridge *bridge, ثابت حرف *topic, تعداد mosquitto__bridge_direction direction, uint8_t qos, ثابت حرف *local_پیشوند, ثابت حرف *remote_پیشوند)
{
	بنية mosquitto__bridge_topic *cur_topic;

	إذا(bridge == عدم){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(direction != bd_بیرون && direction != bd_درون && direction != bd_both){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(qos > 2){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(local_پیشوند && mosquitto_pub_topic_check(local_پیشوند)){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", local_پیشوند);
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(remote_پیشوند && mosquitto_pub_topic_check(remote_پیشوند)){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge topic remote prefix '%s'.", remote_پیشوند);
		أرجع MOSQ_ERR_INVAL;
	}
	إذا((topic == عدم || !strcmp(topic, "\"\"")) &&
			(local_پیشوند == عدم || remote_پیشوند == عدم)){

		log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge remapping.");
		أرجع MOSQ_ERR_INVAL;
	}

	إذا(bridge_پیدا_کن_topic(bridge, topic, direction, qos, local_پیشوند, remote_پیشوند) != عدم){
		log__printf(عدم, MOSQ_LOG_INFO, "Duplicate bridge topic '%s', skipping", topic);
		أرجع MOSQ_ERR_SUCCESS;
	}

	bridge->topic_شمار++;
	cur_topic = mosquitto_calloc(1, حجم(بنية mosquitto__bridge_topic));
	إذا(cur_topic == عدم){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		أرجع MOSQ_ERR_NOMEM;
	}
	cur_topic->بعدی = عدم;

	cur_topic->direction = direction;
	cur_topic->qos = qos;
	cur_topic->local_پیشوند = عدم;
	cur_topic->remote_پیشوند = عدم;

	إذا(topic == عدم || !strcmp(topic, "\"\"")){
		cur_topic->topic = عدم;
	}وإلا{
		cur_topic->topic = mosquitto_strdup(topic);
		إذا(cur_topic->topic == عدم){
			اذهب error;
		}
	}

	إذا(local_پیشوند || remote_پیشوند){
		bridge->topic_remapping = true;
		إذا(local_پیشوند){
			إذا(bridge_create_پیشوند(&cur_topic->local_پیشوند, cur_topic->topic, local_پیشوند, "local")){
				اذهب error;
			}
		}
		إذا(remote_پیشوند){
			إذا(bridge_create_پیشوند(&cur_topic->remote_پیشوند, cur_topic->topic, remote_پیشوند, "local")){
				اذهب error;
			}
		}
	}

	إذا(bridge__create_remap_topic(cur_topic->local_پیشوند,
			cur_topic->topic, &cur_topic->local_topic)){

		اذهب error;
	}

	إذا(bridge__create_remap_topic(cur_topic->remote_پیشوند,
			cur_topic->topic, &cur_topic->remote_topic)){

		اذهب error;
	}

	LL_APPEND(bridge->topics, cur_topic);

	أرجع MOSQ_ERR_SUCCESS;

error:
	mosquitto_FREE(cur_topic->local_پیشوند);
	mosquitto_FREE(cur_topic->remote_پیشوند);
	mosquitto_FREE(cur_topic->local_topic);
	mosquitto_FREE(cur_topic->remote_topic);
	mosquitto_FREE(cur_topic->topic);
	mosquitto_FREE(cur_topic);
	log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
	أرجع MOSQ_ERR_NOMEM;
}


صحيح bridge_remap_topic_درون(بنية mosquitto *context, حرف **topic)
{
	بنية mosquitto__bridge_topic *cur_topic;
	bool تطابق;

	إذا(context->bridge && context->bridge->topics && context->bridge->topic_remapping){
		LL_FOREACH(context->bridge->topics, cur_topic){
			إذا((cur_topic->direction == bd_both || cur_topic->direction == bd_درون)
					&& (cur_topic->remote_پیشوند || cur_topic->local_پیشوند)){

				/* Topic mapping required on this topic if the message matches */

				صحيح rc = mosquitto_topic_matches_sub(cur_topic->remote_topic, *topic, &تطابق);
				إذا(rc){
					mosquitto_FREE(*topic);
					أرجع rc;
				}
				إذا(تطابق){
					حرف *topic_موقت;

					إذا(cur_topic->remote_پیشوند){
						/* This prefix needs removing. */
						إذا(!مقایسه_رشته(cur_topic->remote_پیشوند, *topic, طول_رشته(cur_topic->remote_پیشوند))){
							topic_موقت = mosquitto_strdup((*topic)+طول_رشته(cur_topic->remote_پیشوند));
							إذا(!topic_موقت){
								mosquitto_FREE(*topic);
								أرجع MOSQ_ERR_NOMEM;
							}
							mosquitto_FREE(*topic);
							*topic = topic_موقت;
						}
					}

					إذا(cur_topic->local_پیشوند){
						/* This prefix needs adding. */
						size_t len = طول_رشته(*topic) + طول_رشته(cur_topic->local_پیشوند)+1;
						topic_موقت = mosquitto_malloc(len+1);
						إذا(!topic_موقت){
							mosquitto_FREE(*topic);
							أرجع MOSQ_ERR_NOMEM;
						}
						snprintf(topic_موقت, len, "%s%s", cur_topic->local_پیشوند, *topic);
						topic_موقت[len] = '\0';

						mosquitto_FREE(*topic);
						*topic = topic_موقت;
					}
					اكسر;
				}
			}
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}

#پایان_اگر
