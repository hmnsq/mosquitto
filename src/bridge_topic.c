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

#అన్తర్భావ "config.h"

#అన్తర్భావ "mosquitto.h"
#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "utlist.h"

#నిర్వచితమైతే WITH_BRIDGE


స్థిర పూర్ణాంకం bridge__create_remap_topic(స్థిరరాశి అక్షరం *ఉపసర్గ, స్థిరరాశి అక్షరం *topic, అక్షరం **remap_topic)
{
	ఒకవేళ(ఉపసర్గ){
		ఒకవేళ(topic){
			size_t len = అక్షరశ్రేణి_పొడవు(topic) + అక్షరశ్రేణి_పొడవు(ఉపసర్గ)+1;
			*remap_topic = mosquitto_malloc(len+1);
			ఒకవేళ(!(*remap_topic)){
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
				తిరిగి MOSQ_ERR_NOMEM;
			}
			snprintf(*remap_topic, len+1, "%s%s", ఉపసర్గ, topic);
			(*remap_topic)[len] = '\0';
		}లేకపోతే{
			*remap_topic = mosquitto_strdup(ఉపసర్గ);
			ఒకవేళ(!(*remap_topic)){
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
				తిరిగి MOSQ_ERR_NOMEM;
			}
		}
	}లేకపోతే{
		*remap_topic = mosquitto_strdup(topic);
		ఒకవేళ(!(*remap_topic)){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
			తిరిగి MOSQ_ERR_NOMEM;
		}
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం bridge_create_ఉపసర్గ(అక్షరం **నిండిన_ఉపసర్గ, స్థిరరాశి అక్షరం *topic, స్థిరరాశి అక్షరం *ఉపసర్గ, స్థిరరాశి అక్షరం *direction)
{
	size_t len;

	ఒకవేళ(!ఉపసర్గ || అక్షరశ్రేణి_పొడవు(ఉపసర్గ) != 0){
		ఒకవేళ(mosquitto_pub_topic_check(ఉపసర్గ) != MOSQ_ERR_SUCCESS){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", ఉపసర్గ);
			తిరిగి MOSQ_ERR_INVAL;
		}
	}

	ఒకవేళ(topic){
		len = అక్షరశ్రేణి_పొడవు(topic) + అక్షరశ్రేణి_పొడవు(ఉపసర్గ) + 1;
	}లేకపోతే{
		len = అక్షరశ్రేణి_పొడవు(ఉపసర్గ) + 1;
	}
	*నిండిన_ఉపసర్గ = mosquitto_malloc(len);
	ఒకవేళ(*నిండిన_ఉపసర్గ == ఖాళీ){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
		తిరిగి MOSQ_ERR_NOMEM;
	}

	ఒకవేళ(topic){
		/* Print full_prefix+pattern to check for validity */
		snprintf(*నిండిన_ఉపసర్గ, len, "%s%s", ఉపసర్గ, topic);
	}లేకపోతే{
		snprintf(*నిండిన_ఉపసర్గ, len, "%s", ఉపసర్గ);
	}

	ఒకవేళ(mosquitto_sub_topic_check(*నిండిన_ఉపసర్గ) != MOSQ_ERR_SUCCESS){
		log__printf(ఖాళీ, MOSQ_LOG_ERR,
				"Error: Invalid bridge topic %s prefix and pattern combination '%s'.",
				direction, *నిండిన_ఉపసర్గ);

		తిరిగి MOSQ_ERR_INVAL;
	}

	/* Print just the prefix for storage */
	snprintf(*నిండిన_ఉపసర్గ, len, "%s", ఉపసర్గ);

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర నిర్మాణం mosquitto__bridge_topic *bridge_కనుగొను_topic(నిర్మాణం mosquitto__bridge *bridge, స్థిరరాశి అక్షరం *topic, గణన mosquitto__bridge_direction direction, uint8_t qos, స్థిరరాశి అక్షరం *local_ఉపసర్గ, స్థిరరాశి అక్షరం *remote_ఉపసర్గ)
{
	నిర్మాణం mosquitto__bridge_topic *cur_topic = ఖాళీ;
	bool దొరికింది = false;

	LL_FOREACH(bridge->topics, cur_topic){
		ఒకవేళ(cur_topic->direction != direction){
			కొనసాగు;
		}
		ఒకవేళ(cur_topic->qos != qos){
			కొనసాగు;
		}
		ఒకవేళ(cur_topic->topic != ఖాళీ && topic != ఖాళీ){
			ఒకవేళ(strcmp(cur_topic->topic, topic)){
				కొనసాగు;
			}
		}
		ఒకవేళ(cur_topic->local_ఉపసర్గ != ఖాళీ && local_ఉపసర్గ != ఖాళీ){
			ఒకవేళ(strcmp(cur_topic->local_ఉపసర్గ, local_ఉపసర్గ)){
				కొనసాగు;
			}
		}
		ఒకవేళ(cur_topic->remote_ఉపసర్గ != ఖాళీ && remote_ఉపసర్గ != ఖాళీ){
			ఒకవేళ(strcmp(cur_topic->remote_ఉపసర్గ, remote_ఉపసర్గ)){
				కొనసాగు;
			}
		}
		దొరికింది = true;
		ఆపు;
	}
	ఒకవేళ(!దొరికింది){
		cur_topic = ఖాళీ;
	}

	తిరిగి cur_topic;
}


శూన్య bridge__cleanup_topics(నిర్మాణం mosquitto__bridge *bridge)
{
	నిర్మాణం mosquitto__bridge_topic *topic, *topic_tmp;

	ఒకవేళ(!bridge){
		తిరిగి;
	}

	LL_FOREACH_సురక్షిత(bridge->topics, topic, topic_tmp){
		LL_DELETE(bridge->topics, topic);
		mosquitto_free(topic->local_ఉపసర్గ);
		mosquitto_free(topic->remote_ఉపసర్గ);
		mosquitto_free(topic->local_topic);
		mosquitto_free(topic->remote_topic);
		mosquitto_free(topic->topic);
		mosquitto_free(topic);
	}
}


/* topic <topic> [[[out | in | both] qos-level] local-prefix remote-prefix] */
పూర్ణాంకం bridge_కూడు_topic(నిర్మాణం mosquitto__bridge *bridge, స్థిరరాశి అక్షరం *topic, గణన mosquitto__bridge_direction direction, uint8_t qos, స్థిరరాశి అక్షరం *local_ఉపసర్గ, స్థిరరాశి అక్షరం *remote_ఉపసర్గ)
{
	నిర్మాణం mosquitto__bridge_topic *cur_topic;

	ఒకవేళ(bridge == ఖాళీ){
		తిరిగి MOSQ_ERR_INVAL;
	}
	ఒకవేళ(direction != bd_బహిర్గతం && direction != bd_లోపల && direction != bd_both){
		తిరిగి MOSQ_ERR_INVAL;
	}
	ఒకవేళ(qos > 2){
		తిరిగి MOSQ_ERR_INVAL;
	}
	ఒకవేళ(local_ఉపసర్గ && mosquitto_pub_topic_check(local_ఉపసర్గ)){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", local_ఉపసర్గ);
		తిరిగి MOSQ_ERR_INVAL;
	}
	ఒకవేళ(remote_ఉపసర్గ && mosquitto_pub_topic_check(remote_ఉపసర్గ)){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid bridge topic remote prefix '%s'.", remote_ఉపసర్గ);
		తిరిగి MOSQ_ERR_INVAL;
	}
	ఒకవేళ((topic == ఖాళీ || !strcmp(topic, "\"\"")) &&
			(local_ఉపసర్గ == ఖాళీ || remote_ఉపసర్గ == ఖాళీ)){

		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid bridge remapping.");
		తిరిగి MOSQ_ERR_INVAL;
	}

	ఒకవేళ(bridge_కనుగొను_topic(bridge, topic, direction, qos, local_ఉపసర్గ, remote_ఉపసర్గ) != ఖాళీ){
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Duplicate bridge topic '%s', skipping", topic);
		తిరిగి MOSQ_ERR_SUCCESS;
	}

	bridge->topic_లెక్క++;
	cur_topic = mosquitto_calloc(1, పరిమాణం(నిర్మాణం mosquitto__bridge_topic));
	ఒకవేళ(cur_topic == ఖాళీ){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
		తిరిగి MOSQ_ERR_NOMEM;
	}
	cur_topic->తదుపరి = ఖాళీ;

	cur_topic->direction = direction;
	cur_topic->qos = qos;
	cur_topic->local_ఉపసర్గ = ఖాళీ;
	cur_topic->remote_ఉపసర్గ = ఖాళీ;

	ఒకవేళ(topic == ఖాళీ || !strcmp(topic, "\"\"")){
		cur_topic->topic = ఖాళీ;
	}లేకపోతే{
		cur_topic->topic = mosquitto_strdup(topic);
		ఒకవేళ(cur_topic->topic == ఖాళీ){
			వెళ్ళు error;
		}
	}

	ఒకవేళ(local_ఉపసర్గ || remote_ఉపసర్గ){
		bridge->topic_remapping = true;
		ఒకవేళ(local_ఉపసర్గ){
			ఒకవేళ(bridge_create_ఉపసర్గ(&cur_topic->local_ఉపసర్గ, cur_topic->topic, local_ఉపసర్గ, "local")){
				వెళ్ళు error;
			}
		}
		ఒకవేళ(remote_ఉపసర్గ){
			ఒకవేళ(bridge_create_ఉపసర్గ(&cur_topic->remote_ఉపసర్గ, cur_topic->topic, remote_ఉపసర్గ, "local")){
				వెళ్ళు error;
			}
		}
	}

	ఒకవేళ(bridge__create_remap_topic(cur_topic->local_ఉపసర్గ,
			cur_topic->topic, &cur_topic->local_topic)){

		వెళ్ళు error;
	}

	ఒకవేళ(bridge__create_remap_topic(cur_topic->remote_ఉపసర్గ,
			cur_topic->topic, &cur_topic->remote_topic)){

		వెళ్ళు error;
	}

	LL_APPEND(bridge->topics, cur_topic);

	తిరిగి MOSQ_ERR_SUCCESS;

error:
	mosquitto_FREE(cur_topic->local_ఉపసర్గ);
	mosquitto_FREE(cur_topic->remote_ఉపసర్గ);
	mosquitto_FREE(cur_topic->local_topic);
	mosquitto_FREE(cur_topic->remote_topic);
	mosquitto_FREE(cur_topic->topic);
	mosquitto_FREE(cur_topic);
	log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
	తిరిగి MOSQ_ERR_NOMEM;
}


పూర్ణాంకం bridge_remap_topic_లోపల(నిర్మాణం mosquitto *context, అక్షరం **topic)
{
	నిర్మాణం mosquitto__bridge_topic *cur_topic;
	bool సరిపోలిక;

	ఒకవేళ(context->bridge && context->bridge->topics && context->bridge->topic_remapping){
		LL_FOREACH(context->bridge->topics, cur_topic){
			ఒకవేళ((cur_topic->direction == bd_both || cur_topic->direction == bd_లోపల)
					&& (cur_topic->remote_ఉపసర్గ || cur_topic->local_ఉపసర్గ)){

				/* Topic mapping required on this topic if the message matches */

				పూర్ణాంకం rc = mosquitto_topic_matches_sub(cur_topic->remote_topic, *topic, &సరిపోలిక);
				ఒకవేళ(rc){
					mosquitto_FREE(*topic);
					తిరిగి rc;
				}
				ఒకవేళ(సరిపోలిక){
					అక్షరం *topic_తాత్కాలిక;

					ఒకవేళ(cur_topic->remote_ఉపసర్గ){
						/* This prefix needs removing. */
						ఒకవేళ(!అక్షరశ్రేణి_పోలిక(cur_topic->remote_ఉపసర్గ, *topic, అక్షరశ్రేణి_పొడవు(cur_topic->remote_ఉపసర్గ))){
							topic_తాత్కాలిక = mosquitto_strdup((*topic)+అక్షరశ్రేణి_పొడవు(cur_topic->remote_ఉపసర్గ));
							ఒకవేళ(!topic_తాత్కాలిక){
								mosquitto_FREE(*topic);
								తిరిగి MOSQ_ERR_NOMEM;
							}
							mosquitto_FREE(*topic);
							*topic = topic_తాత్కాలిక;
						}
					}

					ఒకవేళ(cur_topic->local_ఉపసర్గ){
						/* This prefix needs adding. */
						size_t len = అక్షరశ్రేణి_పొడవు(*topic) + అక్షరశ్రేణి_పొడవు(cur_topic->local_ఉపసర్గ)+1;
						topic_తాత్కాలిక = mosquitto_malloc(len+1);
						ఒకవేళ(!topic_తాత్కాలిక){
							mosquitto_FREE(*topic);
							తిరిగి MOSQ_ERR_NOMEM;
						}
						snprintf(topic_తాత్కాలిక, len, "%s%s", cur_topic->local_ఉపసర్గ, *topic);
						topic_తాత్కాలిక[len] = '\0';

						mosquitto_FREE(*topic);
						*topic = topic_తాత్కాలిక;
					}
					ఆపు;
				}
			}
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}

#అయితే_ముగింపు
