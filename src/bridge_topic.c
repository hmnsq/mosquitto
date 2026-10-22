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

#अन्तर्भाव "config.h"

#अन्तर्भाव "mosquitto.h"
#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "utlist.h"

#यदि_परिभाषितम् WITH_BRIDGE


स्थिर पूर्णाङ्क bridge__create_remap_topic(ध्रुव वर्ण *उपसर्गः, ध्रुव वर्ण *topic, वर्ण **remap_topic)
{
	यदि(उपसर्गः){
		यदि(topic){
			size_t len = वाक्यदैर्घ्यम्(topic) + वाक्यदैर्घ्यम्(उपसर्गः)+1;
			*remap_topic = mosquitto_malloc(len+1);
			यदि(!(*remap_topic)){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
			snprintf(*remap_topic, len+1, "%s%s", उपसर्गः, topic);
			(*remap_topic)[len] = '\0';
		}अन्यथा{
			*remap_topic = mosquitto_strdup(उपसर्गः);
			यदि(!(*remap_topic)){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
		}
	}अन्यथा{
		*remap_topic = mosquitto_strdup(topic);
		यदि(!(*remap_topic)){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क bridge_create_उपसर्गः(वर्ण **पूर्णम्_उपसर्गः, ध्रुव वर्ण *topic, ध्रुव वर्ण *उपसर्गः, ध्रुव वर्ण *direction)
{
	size_t len;

	यदि(!उपसर्गः || वाक्यदैर्घ्यम्(उपसर्गः) != 0){
		यदि(mosquitto_pub_topic_check(उपसर्गः) != MOSQ_ERR_SUCCESS){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", उपसर्गः);
			प्रत्यागम MOSQ_ERR_INVAL;
		}
	}

	यदि(topic){
		len = वाक्यदैर्घ्यम्(topic) + वाक्यदैर्घ्यम्(उपसर्गः) + 1;
	}अन्यथा{
		len = वाक्यदैर्घ्यम्(उपसर्गः) + 1;
	}
	*पूर्णम्_उपसर्गः = mosquitto_malloc(len);
	यदि(*पूर्णम्_उपसर्गः == रिक्त){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	यदि(topic){
		/* Print full_prefix+pattern to check for validity */
		snprintf(*पूर्णम्_उपसर्गः, len, "%s%s", उपसर्गः, topic);
	}अन्यथा{
		snprintf(*पूर्णम्_उपसर्गः, len, "%s", उपसर्गः);
	}

	यदि(mosquitto_sub_topic_check(*पूर्णम्_उपसर्गः) != MOSQ_ERR_SUCCESS){
		log__printf(रिक्त, MOSQ_LOG_ERR,
				"Error: Invalid bridge topic %s prefix and pattern combination '%s'.",
				direction, *पूर्णम्_उपसर्गः);

		प्रत्यागम MOSQ_ERR_INVAL;
	}

	/* Print just the prefix for storage */
	snprintf(*पूर्णम्_उपसर्गः, len, "%s", उपसर्गः);

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर संरचना mosquitto__bridge_topic *bridge__find_topic(संरचना mosquitto__bridge *bridge, ध्रुव वर्ण *topic, गणना mosquitto__bridge_direction direction, uint8_t qos, ध्रुव वर्ण *local_उपसर्गः, ध्रुव वर्ण *remote_उपसर्गः)
{
	संरचना mosquitto__bridge_topic *cur_topic = रिक्त;
	bool प्राप्तम् = false;

	LL_FOREACH(bridge->topics, cur_topic){
		यदि(cur_topic->direction != direction){
			अनुवर्त;
		}
		यदि(cur_topic->qos != qos){
			अनुवर्त;
		}
		यदि(cur_topic->topic != रिक्त && topic != रिक्त){
			यदि(वाक्यं_तुलय(cur_topic->topic, topic)){
				अनुवर्त;
			}
		}
		यदि(cur_topic->local_उपसर्गः != रिक्त && local_उपसर्गः != रिक्त){
			यदि(वाक्यं_तुलय(cur_topic->local_उपसर्गः, local_उपसर्गः)){
				अनुवर्त;
			}
		}
		यदि(cur_topic->remote_उपसर्गः != रिक्त && remote_उपसर्गः != रिक्त){
			यदि(वाक्यं_तुलय(cur_topic->remote_उपसर्गः, remote_उपसर्गः)){
				अनुवर्त;
			}
		}
		प्राप्तम् = true;
		भङ्ग;
	}
	यदि(!प्राप्तम्){
		cur_topic = रिक्त;
	}

	प्रत्यागम cur_topic;
}


शून्य bridge__cleanup_topics(संरचना mosquitto__bridge *bridge)
{
	संरचना mosquitto__bridge_topic *topic, *topic_tmp;

	यदि(!bridge){
		प्रत्यागम;
	}

	LL_FOREACH_SAFE(bridge->topics, topic, topic_tmp){
		LL_DELETE(bridge->topics, topic);
		mosquitto_free(topic->local_उपसर्गः);
		mosquitto_free(topic->remote_उपसर्गः);
		mosquitto_free(topic->local_topic);
		mosquitto_free(topic->remote_topic);
		mosquitto_free(topic->topic);
		mosquitto_free(topic);
	}
}


/* topic <topic> [[[out | in | both] qos-level] local-prefix remote-prefix] */
पूर्णाङ्क bridge_सङ्कलय_topic(संरचना mosquitto__bridge *bridge, ध्रुव वर्ण *topic, गणना mosquitto__bridge_direction direction, uint8_t qos, ध्रुव वर्ण *local_उपसर्गः, ध्रुव वर्ण *remote_उपसर्गः)
{
	संरचना mosquitto__bridge_topic *cur_topic;

	यदि(bridge == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि(direction != bd_बहिर्गमनम् && direction != bd_अन्तर् && direction != bd_both){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि(qos > 2){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि(local_उपसर्गः && mosquitto_pub_topic_check(local_उपसर्गः)){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge topic local prefix '%s'.", local_उपसर्गः);
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि(remote_उपसर्गः && mosquitto_pub_topic_check(remote_उपसर्गः)){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge topic remote prefix '%s'.", remote_उपसर्गः);
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि((topic == रिक्त || !वाक्यं_तुलय(topic, "\"\"")) &&
			(local_उपसर्गः == रिक्त || remote_उपसर्गः == रिक्त)){

		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge remapping.");
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	यदि(bridge__find_topic(bridge, topic, direction, qos, local_उपसर्गः, remote_उपसर्गः) != रिक्त){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Duplicate bridge topic '%s', skipping", topic);
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	bridge->topic_गणनम्++;
	cur_topic = mosquitto_calloc(1, परिमाण(संरचना mosquitto__bridge_topic));
	यदि(cur_topic == रिक्त){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	cur_topic->अग्र = रिक्त;

	cur_topic->direction = direction;
	cur_topic->qos = qos;
	cur_topic->local_उपसर्गः = रिक्त;
	cur_topic->remote_उपसर्गः = रिक्त;

	यदि(topic == रिक्त || !वाक्यं_तुलय(topic, "\"\"")){
		cur_topic->topic = रिक्त;
	}अन्यथा{
		cur_topic->topic = mosquitto_strdup(topic);
		यदि(cur_topic->topic == रिक्त){
			गच्छ error;
		}
	}

	यदि(local_उपसर्गः || remote_उपसर्गः){
		bridge->topic_remapping = true;
		यदि(local_उपसर्गः){
			यदि(bridge_create_उपसर्गः(&cur_topic->local_उपसर्गः, cur_topic->topic, local_उपसर्गः, "local")){
				गच्छ error;
			}
		}
		यदि(remote_उपसर्गः){
			यदि(bridge_create_उपसर्गः(&cur_topic->remote_उपसर्गः, cur_topic->topic, remote_उपसर्गः, "local")){
				गच्छ error;
			}
		}
	}

	यदि(bridge__create_remap_topic(cur_topic->local_उपसर्गः,
			cur_topic->topic, &cur_topic->local_topic)){

		गच्छ error;
	}

	यदि(bridge__create_remap_topic(cur_topic->remote_उपसर्गः,
			cur_topic->topic, &cur_topic->remote_topic)){

		गच्छ error;
	}

	LL_APPEND(bridge->topics, cur_topic);

	प्रत्यागम MOSQ_ERR_SUCCESS;

error:
	mosquitto_FREE(cur_topic->local_उपसर्गः);
	mosquitto_FREE(cur_topic->remote_उपसर्गः);
	mosquitto_FREE(cur_topic->local_topic);
	mosquitto_FREE(cur_topic->remote_topic);
	mosquitto_FREE(cur_topic->topic);
	mosquitto_FREE(cur_topic);
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
	प्रत्यागम MOSQ_ERR_NOMEM;
}


पूर्णाङ्क bridge_remap_topic_अन्तर्(संरचना mosquitto *context, वर्ण **topic)
{
	संरचना mosquitto__bridge_topic *cur_topic;
	bool मेलनम्;

	यदि(context->bridge && context->bridge->topics && context->bridge->topic_remapping){
		LL_FOREACH(context->bridge->topics, cur_topic){
			यदि((cur_topic->direction == bd_both || cur_topic->direction == bd_अन्तर्)
					&& (cur_topic->remote_उपसर्गः || cur_topic->local_उपसर्गः)){

				/* Topic mapping required on this topic if the message matches */

				पूर्णाङ्क rc = mosquitto_topic_matches_sub(cur_topic->remote_topic, *topic, &मेलनम्);
				यदि(rc){
					mosquitto_FREE(*topic);
					प्रत्यागम rc;
				}
				यदि(मेलनम्){
					वर्ण *topic_अस्थायी;

					यदि(cur_topic->remote_उपसर्गः){
						/* This prefix needs removing. */
						यदि(!वाक्ये_तुलय(cur_topic->remote_उपसर्गः, *topic, वाक्यदैर्घ्यम्(cur_topic->remote_उपसर्गः))){
							topic_अस्थायी = mosquitto_strdup((*topic)+वाक्यदैर्घ्यम्(cur_topic->remote_उपसर्गः));
							यदि(!topic_अस्थायी){
								mosquitto_FREE(*topic);
								प्रत्यागम MOSQ_ERR_NOMEM;
							}
							mosquitto_FREE(*topic);
							*topic = topic_अस्थायी;
						}
					}

					यदि(cur_topic->local_उपसर्गः){
						/* This prefix needs adding. */
						size_t len = वाक्यदैर्घ्यम्(*topic) + वाक्यदैर्घ्यम्(cur_topic->local_उपसर्गः)+1;
						topic_अस्थायी = mosquitto_malloc(len+1);
						यदि(!topic_अस्थायी){
							mosquitto_FREE(*topic);
							प्रत्यागम MOSQ_ERR_NOMEM;
						}
						snprintf(topic_अस्थायी, len, "%s%s", cur_topic->local_उपसर्गः, *topic);
						topic_अस्थायी[len] = '\0';

						mosquitto_FREE(*topic);
						*topic = topic_अस्थायी;
					}
					भङ्ग;
				}
			}
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}

#यदि_अन्तः
