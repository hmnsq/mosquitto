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

#جے_تعریف_شدہ WITH_BRIDGE


ساكن صحيح bridge__create_remap_topic(ثابت حرف *اگیتر, ثابت حرف *topic, حرف **remap_topic)
{
	إذا(اگیتر){
		إذا(topic){
			size_t len = لڑی_لمبائی(topic) + لڑی_لمبائی(اگیتر)+1;
			*remap_topic = mosquitto_malloc(len+1);
			إذا(!(*remap_topic)){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
				أرجع MOSQ_ERR_NOMEM;
			}
			snprintf(*remap_topic, len+1, "%s%s", اگیتر, topic);
			(*remap_topic)[len] = '\0';
		}وإلا{
			*remap_topic = mosquitto_strdup(اگیتر);
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


ساكن صحيح bridge_create_اگیتر(حرف **بھریا_اگیتر, ثابت حرف *topic, ثابت حرف *اگیتر, ثابت حرف *direction)
{
	size_t len;

	إذا(!اگیتر || لڑی_لمبائی(اگیتر) != 0){
		إذا(mosquitto_pub_topic_check(اگیتر) != MOSQ_ERR_SUCCESS){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", اگیتر);
			أرجع MOSQ_ERR_INVAL;
		}
	}

	إذا(topic){
		len = لڑی_لمبائی(topic) + لڑی_لمبائی(اگیتر) + 1;
	}وإلا{
		len = لڑی_لمبائی(اگیتر) + 1;
	}
	*بھریا_اگیتر = mosquitto_malloc(len);
	إذا(*بھریا_اگیتر == عدم){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		أرجع MOSQ_ERR_NOMEM;
	}

	إذا(topic){
		/* Print full_prefix+pattern to check for validity */
		snprintf(*بھریا_اگیتر, len, "%s%s", اگیتر, topic);
	}وإلا{
		snprintf(*بھریا_اگیتر, len, "%s", اگیتر);
	}

	إذا(mosquitto_sub_topic_check(*بھریا_اگیتر) != MOSQ_ERR_SUCCESS){
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Invalid bridge topic %s prefix and pattern combination '%s'.",
				direction, *بھریا_اگیتر);

		أرجع MOSQ_ERR_INVAL;
	}

	/* Print just the prefix for storage */
	snprintf(*بھریا_اگیتر, len, "%s", اگیتر);

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن بنية mosquitto__bridge_topic *bridge_لبھو_topic(بنية mosquitto__bridge *bridge, ثابت حرف *topic, تعداد mosquitto__bridge_direction direction, uint8_t qos, ثابت حرف *local_اگیتر, ثابت حرف *remote_اگیتر)
{
	بنية mosquitto__bridge_topic *cur_topic = عدم;
	bool لبھیا = false;

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
		إذا(cur_topic->local_اگیتر != عدم && local_اگیتر != عدم){
			إذا(strcmp(cur_topic->local_اگیتر, local_اگیتر)){
				استمر;
			}
		}
		إذا(cur_topic->remote_اگیتر != عدم && remote_اگیتر != عدم){
			إذا(strcmp(cur_topic->remote_اگیتر, remote_اگیتر)){
				استمر;
			}
		}
		لبھیا = true;
		اكسر;
	}
	إذا(!لبھیا){
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

	LL_FOREACH_محفوظ(bridge->topics, topic, topic_tmp){
		LL_DELETE(bridge->topics, topic);
		mosquitto_free(topic->local_اگیتر);
		mosquitto_free(topic->remote_اگیتر);
		mosquitto_free(topic->local_topic);
		mosquitto_free(topic->remote_topic);
		mosquitto_free(topic->topic);
		mosquitto_free(topic);
	}
}


/* topic <topic> [[[out | in | both] qos-level] local-prefix remote-prefix] */
صحيح bridge_جوڑو_topic(بنية mosquitto__bridge *bridge, ثابت حرف *topic, تعداد mosquitto__bridge_direction direction, uint8_t qos, ثابت حرف *local_اگیتر, ثابت حرف *remote_اگیتر)
{
	بنية mosquitto__bridge_topic *cur_topic;

	إذا(bridge == عدم){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(direction != bd_باہرلا && direction != bd_وچ && direction != bd_both){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(qos > 2){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(local_اگیتر && mosquitto_pub_topic_check(local_اگیتر)){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", local_اگیتر);
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(remote_اگیتر && mosquitto_pub_topic_check(remote_اگیتر)){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge topic remote prefix '%s'.", remote_اگیتر);
		أرجع MOSQ_ERR_INVAL;
	}
	إذا((topic == عدم || !strcmp(topic, "\"\"")) &&
			(local_اگیتر == عدم || remote_اگیتر == عدم)){

		log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge remapping.");
		أرجع MOSQ_ERR_INVAL;
	}

	إذا(bridge_لبھو_topic(bridge, topic, direction, qos, local_اگیتر, remote_اگیتر) != عدم){
		log__printf(عدم, MOSQ_LOG_INFO, "Duplicate bridge topic '%s', skipping", topic);
		أرجع MOSQ_ERR_SUCCESS;
	}

	bridge->topic_گنتی++;
	cur_topic = mosquitto_calloc(1, حجم(بنية mosquitto__bridge_topic));
	إذا(cur_topic == عدم){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		أرجع MOSQ_ERR_NOMEM;
	}
	cur_topic->اگلا = عدم;

	cur_topic->direction = direction;
	cur_topic->qos = qos;
	cur_topic->local_اگیتر = عدم;
	cur_topic->remote_اگیتر = عدم;

	إذا(topic == عدم || !strcmp(topic, "\"\"")){
		cur_topic->topic = عدم;
	}وإلا{
		cur_topic->topic = mosquitto_strdup(topic);
		إذا(cur_topic->topic == عدم){
			اذهب error;
		}
	}

	إذا(local_اگیتر || remote_اگیتر){
		bridge->topic_remapping = true;
		إذا(local_اگیتر){
			إذا(bridge_create_اگیتر(&cur_topic->local_اگیتر, cur_topic->topic, local_اگیتر, "local")){
				اذهب error;
			}
		}
		إذا(remote_اگیتر){
			إذا(bridge_create_اگیتر(&cur_topic->remote_اگیتر, cur_topic->topic, remote_اگیتر, "local")){
				اذهب error;
			}
		}
	}

	إذا(bridge__create_remap_topic(cur_topic->local_اگیتر,
			cur_topic->topic, &cur_topic->local_topic)){

		اذهب error;
	}

	إذا(bridge__create_remap_topic(cur_topic->remote_اگیتر,
			cur_topic->topic, &cur_topic->remote_topic)){

		اذهب error;
	}

	LL_APPEND(bridge->topics, cur_topic);

	أرجع MOSQ_ERR_SUCCESS;

error:
	mosquitto_FREE(cur_topic->local_اگیتر);
	mosquitto_FREE(cur_topic->remote_اگیتر);
	mosquitto_FREE(cur_topic->local_topic);
	mosquitto_FREE(cur_topic->remote_topic);
	mosquitto_FREE(cur_topic->topic);
	mosquitto_FREE(cur_topic);
	log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
	أرجع MOSQ_ERR_NOMEM;
}


صحيح bridge_remap_topic_وچ(بنية mosquitto *context, حرف **topic)
{
	بنية mosquitto__bridge_topic *cur_topic;
	bool میل;

	إذا(context->bridge && context->bridge->topics && context->bridge->topic_remapping){
		LL_FOREACH(context->bridge->topics, cur_topic){
			إذا((cur_topic->direction == bd_both || cur_topic->direction == bd_وچ)
					&& (cur_topic->remote_اگیتر || cur_topic->local_اگیتر)){

				/* Topic mapping required on this topic if the message matches */

				صحيح rc = mosquitto_topic_matches_sub(cur_topic->remote_topic, *topic, &میل);
				إذا(rc){
					mosquitto_FREE(*topic);
					أرجع rc;
				}
				إذا(میل){
					حرف *topic_عارضی;

					إذا(cur_topic->remote_اگیتر){
						/* This prefix needs removing. */
						إذا(!لڑی_مقابلہ(cur_topic->remote_اگیتر, *topic, لڑی_لمبائی(cur_topic->remote_اگیتر))){
							topic_عارضی = mosquitto_strdup((*topic)+لڑی_لمبائی(cur_topic->remote_اگیتر));
							إذا(!topic_عارضی){
								mosquitto_FREE(*topic);
								أرجع MOSQ_ERR_NOMEM;
							}
							mosquitto_FREE(*topic);
							*topic = topic_عارضی;
						}
					}

					إذا(cur_topic->local_اگیتر){
						/* This prefix needs adding. */
						size_t len = لڑی_لمبائی(*topic) + لڑی_لمبائی(cur_topic->local_اگیتر)+1;
						topic_عارضی = mosquitto_malloc(len+1);
						إذا(!topic_عارضی){
							mosquitto_FREE(*topic);
							أرجع MOSQ_ERR_NOMEM;
						}
						snprintf(topic_عارضی, len, "%s%s", cur_topic->local_اگیتر, *topic);
						topic_عارضی[len] = '\0';

						mosquitto_FREE(*topic);
						*topic = topic_عارضی;
					}
					اكسر;
				}
			}
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}

#جے_انت
