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

#شامل "config.h"

#شامل "mosquitto.h"
#شامل "mosquitto_broker_internal.h"
#شامل "utlist.h"

#اگر_تعریف_شدہ WITH_BRIDGE


ساکن صحیح bridge__create_remap_topic(ثابت حرف *سابقہ, ثابت حرف *topic, حرف **remap_topic)
{
	اگر(سابقہ){
		اگر(topic){
			size_t len = لڑی_لمبائی(topic) + لڑی_لمبائی(سابقہ)+1;
			*remap_topic = mosquitto_malloc(len+1);
			اگر(!(*remap_topic)){
				log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
				واپس MOSQ_ERR_NOMEM;
			}
			snprintf(*remap_topic, len+1, "%s%s", سابقہ, topic);
			(*remap_topic)[len] = '\0';
		}ورنہ{
			*remap_topic = mosquitto_strdup(سابقہ);
			اگر(!(*remap_topic)){
				log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
				واپس MOSQ_ERR_NOMEM;
			}
		}
	}ورنہ{
		*remap_topic = mosquitto_strdup(topic);
		اگر(!(*remap_topic)){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
			واپس MOSQ_ERR_NOMEM;
		}
	}
	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح bridge_create_سابقہ(حرف **بھرا_سابقہ, ثابت حرف *topic, ثابت حرف *سابقہ, ثابت حرف *direction)
{
	size_t len;

	اگر(!سابقہ || لڑی_لمبائی(سابقہ) != 0){
		اگر(mosquitto_pub_topic_check(سابقہ) != MOSQ_ERR_SUCCESS){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", سابقہ);
			واپس MOSQ_ERR_INVAL;
		}
	}

	اگر(topic){
		len = لڑی_لمبائی(topic) + لڑی_لمبائی(سابقہ) + 1;
	}ورنہ{
		len = لڑی_لمبائی(سابقہ) + 1;
	}
	*بھرا_سابقہ = mosquitto_malloc(len);
	اگر(*بھرا_سابقہ == تہی){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
		واپس MOSQ_ERR_NOMEM;
	}

	اگر(topic){
		/* Print full_prefix+pattern to check for validity */
		snprintf(*بھرا_سابقہ, len, "%s%s", سابقہ, topic);
	}ورنہ{
		snprintf(*بھرا_سابقہ, len, "%s", سابقہ);
	}

	اگر(mosquitto_sub_topic_check(*بھرا_سابقہ) != MOSQ_ERR_SUCCESS){
		log__printf(تہی, MOSQ_LOG_ERR,
				"Error: Invalid bridge topic %s prefix and pattern combination '%s'.",
				direction, *بھرا_سابقہ);

		واپس MOSQ_ERR_INVAL;
	}

	/* Print just the prefix for storage */
	snprintf(*بھرا_سابقہ, len, "%s", سابقہ);

	واپس MOSQ_ERR_SUCCESS;
}


ساکن ساخت mosquitto__bridge_topic *bridge_ڈھونڈو_topic(ساخت mosquitto__bridge *bridge, ثابت حرف *topic, شمار mosquitto__bridge_direction direction, uint8_t qos, ثابت حرف *local_سابقہ, ثابت حرف *remote_سابقہ)
{
	ساخت mosquitto__bridge_topic *cur_topic = تہی;
	bool ملا = false;

	LL_FOREACH(bridge->topics, cur_topic){
		اگر(cur_topic->direction != direction){
			جاری;
		}
		اگر(cur_topic->qos != qos){
			جاری;
		}
		اگر(cur_topic->topic != تہی && topic != تہی){
			اگر(strcmp(cur_topic->topic, topic)){
				جاری;
			}
		}
		اگر(cur_topic->local_سابقہ != تہی && local_سابقہ != تہی){
			اگر(strcmp(cur_topic->local_سابقہ, local_سابقہ)){
				جاری;
			}
		}
		اگر(cur_topic->remote_سابقہ != تہی && remote_سابقہ != تہی){
			اگر(strcmp(cur_topic->remote_سابقہ, remote_سابقہ)){
				جاری;
			}
		}
		ملا = true;
		توڑو;
	}
	اگر(!ملا){
		cur_topic = تہی;
	}

	واپس cur_topic;
}


خالی bridge__cleanup_topics(ساخت mosquitto__bridge *bridge)
{
	ساخت mosquitto__bridge_topic *topic, *topic_tmp;

	اگر(!bridge){
		واپس;
	}

	LL_FOREACH_محفوظ(bridge->topics, topic, topic_tmp){
		LL_DELETE(bridge->topics, topic);
		mosquitto_free(topic->local_سابقہ);
		mosquitto_free(topic->remote_سابقہ);
		mosquitto_free(topic->local_topic);
		mosquitto_free(topic->remote_topic);
		mosquitto_free(topic->topic);
		mosquitto_free(topic);
	}
}


/* topic <topic> [[[out | in | both] qos-level] local-prefix remote-prefix] */
صحیح bridge_جوڑو_topic(ساخت mosquitto__bridge *bridge, ثابت حرف *topic, شمار mosquitto__bridge_direction direction, uint8_t qos, ثابت حرف *local_سابقہ, ثابت حرف *remote_سابقہ)
{
	ساخت mosquitto__bridge_topic *cur_topic;

	اگر(bridge == تہی){
		واپس MOSQ_ERR_INVAL;
	}
	اگر(direction != bd_برآمد && direction != bd_اندر && direction != bd_both){
		واپس MOSQ_ERR_INVAL;
	}
	اگر(qos > 2){
		واپس MOSQ_ERR_INVAL;
	}
	اگر(local_سابقہ && mosquitto_pub_topic_check(local_سابقہ)){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", local_سابقہ);
		واپس MOSQ_ERR_INVAL;
	}
	اگر(remote_سابقہ && mosquitto_pub_topic_check(remote_سابقہ)){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid bridge topic remote prefix '%s'.", remote_سابقہ);
		واپس MOSQ_ERR_INVAL;
	}
	اگر((topic == تہی || !strcmp(topic, "\"\"")) &&
			(local_سابقہ == تہی || remote_سابقہ == تہی)){

		log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid bridge remapping.");
		واپس MOSQ_ERR_INVAL;
	}

	اگر(bridge_ڈھونڈو_topic(bridge, topic, direction, qos, local_سابقہ, remote_سابقہ) != تہی){
		log__printf(تہی, MOSQ_LOG_INFO, "Duplicate bridge topic '%s', skipping", topic);
		واپس MOSQ_ERR_SUCCESS;
	}

	bridge->topic_گنتی++;
	cur_topic = mosquitto_calloc(1, حجم(ساخت mosquitto__bridge_topic));
	اگر(cur_topic == تہی){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
		واپس MOSQ_ERR_NOMEM;
	}
	cur_topic->اگلا = تہی;

	cur_topic->direction = direction;
	cur_topic->qos = qos;
	cur_topic->local_سابقہ = تہی;
	cur_topic->remote_سابقہ = تہی;

	اگر(topic == تہی || !strcmp(topic, "\"\"")){
		cur_topic->topic = تہی;
	}ورنہ{
		cur_topic->topic = mosquitto_strdup(topic);
		اگر(cur_topic->topic == تہی){
			جاؤ error;
		}
	}

	اگر(local_سابقہ || remote_سابقہ){
		bridge->topic_remapping = true;
		اگر(local_سابقہ){
			اگر(bridge_create_سابقہ(&cur_topic->local_سابقہ, cur_topic->topic, local_سابقہ, "local")){
				جاؤ error;
			}
		}
		اگر(remote_سابقہ){
			اگر(bridge_create_سابقہ(&cur_topic->remote_سابقہ, cur_topic->topic, remote_سابقہ, "local")){
				جاؤ error;
			}
		}
	}

	اگر(bridge__create_remap_topic(cur_topic->local_سابقہ,
			cur_topic->topic, &cur_topic->local_topic)){

		جاؤ error;
	}

	اگر(bridge__create_remap_topic(cur_topic->remote_سابقہ,
			cur_topic->topic, &cur_topic->remote_topic)){

		جاؤ error;
	}

	LL_APPEND(bridge->topics, cur_topic);

	واپس MOSQ_ERR_SUCCESS;

error:
	mosquitto_FREE(cur_topic->local_سابقہ);
	mosquitto_FREE(cur_topic->remote_سابقہ);
	mosquitto_FREE(cur_topic->local_topic);
	mosquitto_FREE(cur_topic->remote_topic);
	mosquitto_FREE(cur_topic->topic);
	mosquitto_FREE(cur_topic);
	log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
	واپس MOSQ_ERR_NOMEM;
}


صحیح bridge_remap_topic_اندر(ساخت mosquitto *context, حرف **topic)
{
	ساخت mosquitto__bridge_topic *cur_topic;
	bool مطابقت;

	اگر(context->bridge && context->bridge->topics && context->bridge->topic_remapping){
		LL_FOREACH(context->bridge->topics, cur_topic){
			اگر((cur_topic->direction == bd_both || cur_topic->direction == bd_اندر)
					&& (cur_topic->remote_سابقہ || cur_topic->local_سابقہ)){

				/* Topic mapping required on this topic if the message matches */

				صحیح rc = mosquitto_topic_matches_sub(cur_topic->remote_topic, *topic, &مطابقت);
				اگر(rc){
					mosquitto_FREE(*topic);
					واپس rc;
				}
				اگر(مطابقت){
					حرف *topic_عارضی;

					اگر(cur_topic->remote_سابقہ){
						/* This prefix needs removing. */
						اگر(!لڑی_موازنہ(cur_topic->remote_سابقہ, *topic, لڑی_لمبائی(cur_topic->remote_سابقہ))){
							topic_عارضی = mosquitto_strdup((*topic)+لڑی_لمبائی(cur_topic->remote_سابقہ));
							اگر(!topic_عارضی){
								mosquitto_FREE(*topic);
								واپس MOSQ_ERR_NOMEM;
							}
							mosquitto_FREE(*topic);
							*topic = topic_عارضی;
						}
					}

					اگر(cur_topic->local_سابقہ){
						/* This prefix needs adding. */
						size_t len = لڑی_لمبائی(*topic) + لڑی_لمبائی(cur_topic->local_سابقہ)+1;
						topic_عارضی = mosquitto_malloc(len+1);
						اگر(!topic_عارضی){
							mosquitto_FREE(*topic);
							واپس MOSQ_ERR_NOMEM;
						}
						snprintf(topic_عارضی, len, "%s%s", cur_topic->local_سابقہ, *topic);
						topic_عارضی[len] = '\0';

						mosquitto_FREE(*topic);
						*topic = topic_عارضی;
					}
					توڑو;
				}
			}
		}
	}

	واپس MOSQ_ERR_SUCCESS;
}

#اگر_ختم
