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

#که_تعریف_شوی WITH_BRIDGE


ساكن صحيح bridge__create_remap_topic(ثابت حرف *مختاړی, ثابت حرف *topic, حرف **remap_topic)
{
	إذا(مختاړی){
		إذا(topic){
			size_t len = مزي_اوږدوالی(topic) + مزي_اوږدوالی(مختاړی)+1;
			*remap_topic = mosquitto_malloc(len+1);
			إذا(!(*remap_topic)){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
				أرجع MOSQ_ERR_NOMEM;
			}
			snprintf(*remap_topic, len+1, "%s%s", مختاړی, topic);
			(*remap_topic)[len] = '\0';
		}وإلا{
			*remap_topic = mosquitto_strdup(مختاړی);
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


ساكن صحيح bridge_create_مختاړی(حرف **ډک_مختاړی, ثابت حرف *topic, ثابت حرف *مختاړی, ثابت حرف *direction)
{
	size_t len;

	إذا(!مختاړی || مزي_اوږدوالی(مختاړی) != 0){
		إذا(mosquitto_pub_topic_check(مختاړی) != MOSQ_ERR_SUCCESS){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", مختاړی);
			أرجع MOSQ_ERR_INVAL;
		}
	}

	إذا(topic){
		len = مزي_اوږدوالی(topic) + مزي_اوږدوالی(مختاړی) + 1;
	}وإلا{
		len = مزي_اوږدوالی(مختاړی) + 1;
	}
	*ډک_مختاړی = mosquitto_malloc(len);
	إذا(*ډک_مختاړی == عدم){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		أرجع MOSQ_ERR_NOMEM;
	}

	إذا(topic){
		/* Print full_prefix+pattern to check for validity */
		snprintf(*ډک_مختاړی, len, "%s%s", مختاړی, topic);
	}وإلا{
		snprintf(*ډک_مختاړی, len, "%s", مختاړی);
	}

	إذا(mosquitto_sub_topic_check(*ډک_مختاړی) != MOSQ_ERR_SUCCESS){
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Invalid bridge topic %s prefix and pattern combination '%s'.",
				direction, *ډک_مختاړی);

		أرجع MOSQ_ERR_INVAL;
	}

	/* Print just the prefix for storage */
	snprintf(*ډک_مختاړی, len, "%s", مختاړی);

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن بنية mosquitto__bridge_topic *bridge_پیدا_کړه_topic(بنية mosquitto__bridge *bridge, ثابت حرف *topic, تعداد mosquitto__bridge_direction direction, uint8_t qos, ثابت حرف *local_مختاړی, ثابت حرف *remote_مختاړی)
{
	بنية mosquitto__bridge_topic *cur_topic = عدم;
	bool وموندل = false;

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
		إذا(cur_topic->local_مختاړی != عدم && local_مختاړی != عدم){
			إذا(strcmp(cur_topic->local_مختاړی, local_مختاړی)){
				استمر;
			}
		}
		إذا(cur_topic->remote_مختاړی != عدم && remote_مختاړی != عدم){
			إذا(strcmp(cur_topic->remote_مختاړی, remote_مختاړی)){
				استمر;
			}
		}
		وموندل = true;
		اكسر;
	}
	إذا(!وموندل){
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

	LL_FOREACH_خوندي(bridge->topics, topic, topic_tmp){
		LL_DELETE(bridge->topics, topic);
		mosquitto_free(topic->local_مختاړی);
		mosquitto_free(topic->remote_مختاړی);
		mosquitto_free(topic->local_topic);
		mosquitto_free(topic->remote_topic);
		mosquitto_free(topic->topic);
		mosquitto_free(topic);
	}
}


/* topic <topic> [[[out | in | both] qos-level] local-prefix remote-prefix] */
صحيح bridge_جمع_کړه_topic(بنية mosquitto__bridge *bridge, ثابت حرف *topic, تعداد mosquitto__bridge_direction direction, uint8_t qos, ثابت حرف *local_مختاړی, ثابت حرف *remote_مختاړی)
{
	بنية mosquitto__bridge_topic *cur_topic;

	إذا(bridge == عدم){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(direction != bd_بهر && direction != bd_دننه && direction != bd_both){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(qos > 2){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(local_مختاړی && mosquitto_pub_topic_check(local_مختاړی)){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", local_مختاړی);
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(remote_مختاړی && mosquitto_pub_topic_check(remote_مختاړی)){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge topic remote prefix '%s'.", remote_مختاړی);
		أرجع MOSQ_ERR_INVAL;
	}
	إذا((topic == عدم || !strcmp(topic, "\"\"")) &&
			(local_مختاړی == عدم || remote_مختاړی == عدم)){

		log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge remapping.");
		أرجع MOSQ_ERR_INVAL;
	}

	إذا(bridge_پیدا_کړه_topic(bridge, topic, direction, qos, local_مختاړی, remote_مختاړی) != عدم){
		log__printf(عدم, MOSQ_LOG_INFO, "Duplicate bridge topic '%s', skipping", topic);
		أرجع MOSQ_ERR_SUCCESS;
	}

	bridge->topic_شمېر++;
	cur_topic = mosquitto_calloc(1, حجم(بنية mosquitto__bridge_topic));
	إذا(cur_topic == عدم){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		أرجع MOSQ_ERR_NOMEM;
	}
	cur_topic->بل = عدم;

	cur_topic->direction = direction;
	cur_topic->qos = qos;
	cur_topic->local_مختاړی = عدم;
	cur_topic->remote_مختاړی = عدم;

	إذا(topic == عدم || !strcmp(topic, "\"\"")){
		cur_topic->topic = عدم;
	}وإلا{
		cur_topic->topic = mosquitto_strdup(topic);
		إذا(cur_topic->topic == عدم){
			اذهب error;
		}
	}

	إذا(local_مختاړی || remote_مختاړی){
		bridge->topic_remapping = true;
		إذا(local_مختاړی){
			إذا(bridge_create_مختاړی(&cur_topic->local_مختاړی, cur_topic->topic, local_مختاړی, "local")){
				اذهب error;
			}
		}
		إذا(remote_مختاړی){
			إذا(bridge_create_مختاړی(&cur_topic->remote_مختاړی, cur_topic->topic, remote_مختاړی, "local")){
				اذهب error;
			}
		}
	}

	إذا(bridge__create_remap_topic(cur_topic->local_مختاړی,
			cur_topic->topic, &cur_topic->local_topic)){

		اذهب error;
	}

	إذا(bridge__create_remap_topic(cur_topic->remote_مختاړی,
			cur_topic->topic, &cur_topic->remote_topic)){

		اذهب error;
	}

	LL_APPEND(bridge->topics, cur_topic);

	أرجع MOSQ_ERR_SUCCESS;

error:
	mosquitto_FREE(cur_topic->local_مختاړی);
	mosquitto_FREE(cur_topic->remote_مختاړی);
	mosquitto_FREE(cur_topic->local_topic);
	mosquitto_FREE(cur_topic->remote_topic);
	mosquitto_FREE(cur_topic->topic);
	mosquitto_FREE(cur_topic);
	log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
	أرجع MOSQ_ERR_NOMEM;
}


صحيح bridge_remap_topic_دننه(بنية mosquitto *context, حرف **topic)
{
	بنية mosquitto__bridge_topic *cur_topic;
	bool سمون;

	إذا(context->bridge && context->bridge->topics && context->bridge->topic_remapping){
		LL_FOREACH(context->bridge->topics, cur_topic){
			إذا((cur_topic->direction == bd_both || cur_topic->direction == bd_دننه)
					&& (cur_topic->remote_مختاړی || cur_topic->local_مختاړی)){

				/* Topic mapping required on this topic if the message matches */

				صحيح rc = mosquitto_topic_matches_sub(cur_topic->remote_topic, *topic, &سمون);
				إذا(rc){
					mosquitto_FREE(*topic);
					أرجع rc;
				}
				إذا(سمون){
					حرف *topic_لنډمهاله;

					إذا(cur_topic->remote_مختاړی){
						/* This prefix needs removing. */
						إذا(!مزي_پرتله(cur_topic->remote_مختاړی, *topic, مزي_اوږدوالی(cur_topic->remote_مختاړی))){
							topic_لنډمهاله = mosquitto_strdup((*topic)+مزي_اوږدوالی(cur_topic->remote_مختاړی));
							إذا(!topic_لنډمهاله){
								mosquitto_FREE(*topic);
								أرجع MOSQ_ERR_NOMEM;
							}
							mosquitto_FREE(*topic);
							*topic = topic_لنډمهاله;
						}
					}

					إذا(cur_topic->local_مختاړی){
						/* This prefix needs adding. */
						size_t len = مزي_اوږدوالی(*topic) + مزي_اوږدوالی(cur_topic->local_مختاړی)+1;
						topic_لنډمهاله = mosquitto_malloc(len+1);
						إذا(!topic_لنډمهاله){
							mosquitto_FREE(*topic);
							أرجع MOSQ_ERR_NOMEM;
						}
						snprintf(topic_لنډمهاله, len, "%s%s", cur_topic->local_مختاړی, *topic);
						topic_لنډمهاله[len] = '\0';

						mosquitto_FREE(*topic);
						*topic = topic_لنډمهاله;
					}
					اكسر;
				}
			}
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}

#د_که_پای
