/*
Copyright (c) 2010-2021 Roger Light <roger@atchoo.org>

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

/* A note on matching topic subscriptions.
 *
 * Topics can be up to 32767 characters in length. The / character is used as a
 * hierarchy delimiter. Messages are published to a particular topic.
 * Clients may subscribe to particular topics directly, but may also use
 * wildcards in subscriptions.  The + and # characters are used as wildcards.
 * The # wildcard can be used at the end of a subscription only, and is a
 * wildcard for the level of hierarchy at which it is placed and all subsequent
 * levels.
 * The + wildcard may be used at any point within the subscription and is a
 * wildcard for only the level of hierarchy at which it is placed.
 * Neither wildcard may be used as part of a substring.
 * Valid:
 * 	a/b/+
 * 	a/+/c
 * 	a/#
 * 	a/b/#
 * 	#
 * 	+/b/c
 * 	+/+/+
 * Invalid:
 *	a/#/c
 *	a+/b/c
 * Valid but non-matching:
 *	a/b
 *	a/+
 *	+/b
 *	b/c/a
 *	a/b/d
 */

#अन्तर्भाव "config.h"

#अन्तर्भाव <assert.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "util_mosq.h"

#अन्तर्भाव "utlist.h"

स्थिर संरचना mosquitto__subhier *sub_सङ्कलय_hier_प्रविष्टिः(संरचना mosquitto__subhier *parent, संरचना mosquitto__subhier **sibling, ध्रुव वर्ण *topic, uint16_t len);

स्थिर अचिह्नित पूर्णाङ्क hashv_plus = 0;
स्थिर अचिह्नित पूर्णाङ्क hashv_hash = 0;


स्थिर पूर्णाङ्क subs__send(संरचना mosquitto__subleaf *पर्णम्, ध्रुव वर्ण *topic, uint8_t qos, पूर्णाङ्क retain, संरचना mosquitto_आधारः_सन्देशः *stored)
{
	bool client_retain;
	uint16_t mid;
	uint8_t client_qos, सन्देशः_qos;
	पूर्णाङ्क rc2;

	/* Check for ACL topic access. */
	rc2 = mosquitto_acl_check(पर्णम्->context, topic, stored->दत्त.payloadlen, stored->दत्त.payload, stored->दत्त.qos, stored->दत्त.retain, stored->दत्त.properties, MOSQ_ACL_READ);
	यदि(rc2 == MOSQ_ERR_ACL_DENIED){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}अन्यथा यदि(rc2 == MOSQ_ERR_SUCCESS){
		client_qos = MQTT_SUB_OPT_GET_QOS(पर्णम्->subscription_options);

		यदि(db.config->upgrade_outgoing_qos){
			सन्देशः_qos = client_qos;
		}अन्यथा{
			यदि(qos > client_qos){
				सन्देशः_qos = client_qos;
			}अन्यथा{
				सन्देशः_qos = qos;
			}
		}
		यदि(सन्देशः_qos){
			mid = mosquitto__mid_generate(पर्णम्->context);
		}अन्यथा{
			mid = 0;
		}
		यदि(MQTT_SUB_OPT_GET_RETAIN_AS_PUBLISHED(पर्णम्->subscription_options)){
			client_retain = retain;
		}अन्यथा{
			client_retain = false;
		}
		यदि(db_सन्देशः_निवेशय_outgoing(पर्णम्->context, 0, mid, सन्देशः_qos, client_retain, stored, पर्णम्->identifier, true, true) == 1){
			प्रत्यागम 1;
		}
	}अन्यथा{
		प्रत्यागम 1; /* Application error */
	}
	प्रत्यागम 0;
}


स्थिर पूर्णाङ्क subs__shared_process(संरचना mosquitto__subhier *hier, ध्रुव वर्ण *topic, uint8_t qos, पूर्णाङ्क retain, संरचना mosquitto_आधारः_सन्देशः *stored)
{
	पूर्णाङ्क rc = 0, rc2;
	संरचना mosquitto__subshared *shared, *shared_tmp;
	संरचना mosquitto__subleaf *पर्णम्;

	HASH_ITER(hh, hier->shared, shared, shared_tmp){
		पर्णम् = shared->subs;
		rc2 = subs__send(पर्णम्, topic, qos, retain, stored);
		/* Remove current from the top, add back to the bottom */
		DL_DELETE(shared->subs, पर्णम्);
		DL_APPEND(shared->subs, पर्णम्);

		यदि(rc2){
			rc = 1;
		}
	}

	प्रत्यागम rc;
}


स्थिर पूर्णाङ्क subs__process(संरचना mosquitto__subhier *hier, ध्रुव वर्ण *source_id, ध्रुव वर्ण *topic, uint8_t qos, पूर्णाङ्क retain, संरचना mosquitto_आधारः_सन्देशः *stored)
{
	पूर्णाङ्क rc = 0;
	पूर्णाङ्क rc2;
	संरचना mosquitto__subleaf *पर्णम्;

	rc = subs__shared_process(hier, topic, qos, retain, stored);

	पर्णम् = hier->subs;
	यावत्(source_id && पर्णम्){
		यदि(!पर्णम्->context->id || (MQTT_SUB_OPT_GET_NO_LOCAL(पर्णम्->subscription_options) && !वाक्यं_तुलय(पर्णम्->context->id, source_id))){
			पर्णम् = पर्णम्->अग्र;
			अनुवर्त;
		}
		rc2 = subs__send(पर्णम्, topic, qos, retain, stored);
		यदि(rc2){
			rc = 1;
		}
		पर्णम् = पर्णम्->अग्र;
	}
	यदि(hier->subs || hier->shared){
		प्रत्यागम rc;
	}अन्यथा{
		प्रत्यागम MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


स्थिर पूर्णाङ्क sub_सङ्कलय_पर्णम्(संरचना mosquitto *context, ध्रुव संरचना mosquitto_subscription *sub, संरचना mosquitto__subleaf **शीर्ष, संरचना mosquitto__subleaf **newleaf)
{
	संरचना mosquitto__subleaf *पर्णम्;

	*newleaf = रिक्त;
	पर्णम् = *शीर्ष;

	यावत्(पर्णम्){
		यदि(पर्णम्->context && पर्णम्->context->id && !वाक्यं_तुलय(पर्णम्->context->id, context->id)){
			/* Client making a second subscription to same topic. Only
			 * need to update QoS. Return MOSQ_ERR_SUB_EXISTS to
			 * indicate this to the calling function. */
			पर्णम्->identifier = sub->identifier;
			पर्णम्->subscription_options = sub->options;
			प्रत्यागम MOSQ_ERR_SUB_EXISTS;
		}
		पर्णम् = पर्णम्->अग्र;
	}
	पर्णम् = mosquitto_calloc(1, परिमाण(संरचना mosquitto__subleaf) + वाक्यदैर्घ्यम्(sub->topic_filter) + 1);
	यदि(!पर्णम्){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	पर्णम्->context = context;
	पर्णम्->identifier = sub->identifier;
	पर्णम्->subscription_options = sub->options;
	वाक्यं_प्रतिलिख(पर्णम्->topic_filter, sub->topic_filter);

	DL_APPEND(*शीर्ष, पर्णम्);
	*newleaf = पर्णम्;

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर शून्य sub_remove_shared_पर्णम्(संरचना mosquitto__subhier *subhier, संरचना mosquitto__subshared *shared, संरचना mosquitto__subleaf *पर्णम्)
{
	DL_DELETE(shared->subs, पर्णम्);
	यदि(shared->subs == रिक्त){
		HASH_DELETE(hh, subhier->shared, shared);
		mosquitto_FREE(shared);
	}
}


स्थिर पूर्णाङ्क sub_सङ्कलय_shared(संरचना mosquitto *context, ध्रुव संरचना mosquitto_subscription *sub, संरचना mosquitto__subhier *subhier, ध्रुव वर्ण *sharename)
{
	संरचना mosquitto__subleaf *newleaf;
	संरचना mosquitto__subshared *shared = रिक्त;
	संरचना mosquitto__subleaf **subs;
	size_t slen;
	पूर्णाङ्क rc;
	अचिह्नित hashv;

	slen = वाक्यदैर्घ्यम्(sharename);

	HASH_मानम्(sharename, slen, hashv);

	HASH_FIND_BYHASHVALUE(hh, subhier->shared, sharename, slen, hashv, shared);
	यदि(shared == रिक्त){
		shared = mosquitto_calloc(1, परिमाण(संरचना mosquitto__subshared) + slen + 1);
		यदि(!shared){
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
		strncpy(shared->name, sharename, slen+1);

		HASH_सङ्कलय_BYHASHVALUE(hh, subhier->shared, name, slen, hashv, shared);
	}

	rc = sub_सङ्कलय_पर्णम्(context, sub, &shared->subs, &newleaf);
	यदि(rc > 0){
		यदि(shared->subs == रिक्त){
			HASH_DELETE(hh, subhier->shared, shared);
			mosquitto_FREE(shared);
		}
		प्रत्यागम rc;
	}

	यदि(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = shared;

		bool assigned = false;
		प्रति(पूर्णाङ्क i=0; i<context->subs_क्षमता; i++){
			यदि(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_गणनम्++;
				assigned = true;
				भङ्ग;
			}
		}
		यदि(assigned == false){
			subs = mosquitto_realloc(context->subs, परिमाण(संरचना mosquitto__subleaf *)*(size_t)(context->subs_क्षमता + 1));
			यदि(!subs){
				sub_remove_shared_पर्णम्(subhier, shared, newleaf);
				mosquitto_FREE(newleaf);
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_क्षमता++;
			context->subs_गणनम्++;
			context->subs[context->subs_क्षमता-1] = newleaf;
		}
#यदि_परिभाषितम् WITH_SYS_वृक्षः
		db.shared_subscription_गणनम्++;
#यदि_अन्तः
	}

	यदि(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		प्रत्यागम rc;
	}अन्यथा{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}
}


स्थिर पूर्णाङ्क sub_सङ्कलय_प्रसामान्यम्(संरचना mosquitto *context, ध्रुव संरचना mosquitto_subscription *sub, संरचना mosquitto__subhier *subhier)
{
	संरचना mosquitto__subleaf *newleaf = रिक्त;
	संरचना mosquitto__subleaf **subs;
	पूर्णाङ्क rc;

	rc = sub_सङ्कलय_पर्णम्(context, sub, &subhier->subs, &newleaf);
	यदि(rc > 0){
		प्रत्यागम rc;
	}

	यदि(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = रिक्त;

		bool assigned = false;
		प्रति(पूर्णाङ्क i=0; i<context->subs_क्षमता; i++){
			यदि(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_गणनम्++;
				assigned = true;
				भङ्ग;
			}
		}
		यदि(assigned == false){
			subs = mosquitto_realloc(context->subs, परिमाण(संरचना mosquitto__subleaf *)*(size_t)(context->subs_क्षमता + 1));
			यदि(!subs){
				DL_DELETE(subhier->subs, newleaf);
				mosquitto_FREE(newleaf);
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_क्षमता++;
			context->subs_गणनम्++;
			context->subs[context->subs_क्षमता-1] = newleaf;
		}
#यदि_परिभाषितम् WITH_SYS_वृक्षः
		db.subscription_गणनम्++;
#यदि_अन्तः
	}

	यदि(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		प्रत्यागम rc;
	}अन्यथा{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}
}


स्थिर पूर्णाङ्क sub_सङ्कलय_context(संरचना mosquitto *context, ध्रुव संरचना mosquitto_subscription *sub, संरचना mosquitto__subhier *subhier, वर्ण *ध्रुव *ध्रुव topics, ध्रुव वर्ण *sharename)
{
	संरचना mosquitto__subhier *branch;
	पूर्णाङ्क topic_सूचकांक = 0;
	size_t topiclen;

	/* Find leaf node */
	यावत्(topics && topics[topic_सूचकांक] != रिक्त){
		topiclen = वाक्यदैर्घ्यम्(topics[topic_सूचकांक]);
		यदि(topiclen > UINT16_MAX){
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		HASH_FIND(hh, subhier->children, topics[topic_सूचकांक], topiclen, branch);
		यदि(!branch){
			/* Not found */
			branch = sub_सङ्कलय_hier_प्रविष्टिः(subhier, &subhier->children, topics[topic_सूचकांक], (uint16_t)topiclen);
			यदि(!branch){
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
		}
		subhier = branch;
		topic_सूचकांक++;
	}

	/* Add add our context */
	यदि(context && context->id){
		यदि(sharename){
			प्रत्यागम sub_सङ्कलय_shared(context, sub, subhier, sharename);
		}अन्यथा{
			प्रत्यागम sub_सङ्कलय_प्रसामान्यम्(context, sub, subhier);
		}
	}अन्यथा{
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}
}


स्थिर पूर्णाङ्क sub_remove_प्रसामान्यम्(संरचना mosquitto *context, संरचना mosquitto__subhier *subhier, uint8_t *reason)
{
	संरचना mosquitto__subleaf *पर्णम्;

	पर्णम् = subhier->subs;
	यावत्(पर्णम्){
		यदि(पर्णम्->context==context){
#यदि_परिभाषितम् WITH_SYS_वृक्षः
			db.subscription_गणनम्--;
#यदि_अन्तः
			DL_DELETE(subhier->subs, पर्णम्);

			/* Remove the reference to the sub that the client is keeping.
			 * It would be nice to be able to use the reference directly,
			 * but that would involve keeping a copy of the topic string in
			 * each subleaf. Might be worth considering though. */
			प्रति(पूर्णाङ्क i=0; i<context->subs_क्षमता; i++){
				यदि(context->subs[i] && context->subs[i]->hier == subhier){
					context->subs_गणनम्--;
					mosquitto_free(context->subs[i]);
					context->subs[i] = रिक्त;
					भङ्ग;
				}
			}
			*reason = 0;
			प्रत्यागम MOSQ_ERR_SUCCESS;
		}
		पर्णम् = पर्णम्->अग्र;
	}
	प्रत्यागम MOSQ_ERR_NO_SUBSCRIBERS;
}


स्थिर पूर्णाङ्क sub__remove_shared(संरचना mosquitto *context, संरचना mosquitto__subhier *subhier, uint8_t *reason, ध्रुव वर्ण *sharename)
{
	संरचना mosquitto__subshared *shared;

	HASH_FIND(hh, subhier->shared, sharename, वाक्यदैर्घ्यम्(sharename), shared);
	यदि(shared){
		संरचना mosquitto__subleaf *पर्णम् = shared->subs;
		यावत्(पर्णम्){
			यदि(पर्णम्->context==context){
#यदि_परिभाषितम् WITH_SYS_वृक्षः
				db.shared_subscription_गणनम्--;
#यदि_अन्तः
				DL_DELETE(shared->subs, पर्णम्);

				/* Remove the reference to the sub that the client is keeping.
				* It would be nice to be able to use the reference directly,
				* but that would involve keeping a copy of the topic string in
				* each subleaf. Might be worth considering though. */
				प्रति(पूर्णाङ्क i=0; i<context->subs_क्षमता; i++){
					यदि(context->subs[i]
							&& context->subs[i]->hier == subhier
							&& context->subs[i]->shared == shared){

						mosquitto_free(context->subs[i]);
						context->subs[i] = रिक्त;
						context->subs_गणनम्--;
						भङ्ग;
					}
				}

				यदि(shared->subs == रिक्त){
					HASH_DELETE(hh, subhier->shared, shared);
					mosquitto_FREE(shared);
				}

				*reason = 0;
				प्रत्यागम MOSQ_ERR_SUCCESS;
			}
			पर्णम् = पर्णम्->अग्र;
		}
		प्रत्यागम MOSQ_ERR_NO_SUBSCRIBERS;
	}अन्यथा{
		प्रत्यागम MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


स्थिर पूर्णाङ्क sub__remove_recurse(संरचना mosquitto *context, संरचना mosquitto__subhier *subhier, वर्ण **topics, uint8_t *reason, ध्रुव वर्ण *sharename)
{
	संरचना mosquitto__subhier *branch;

	यदि(topics == रिक्त || topics[0] == रिक्त){
		यदि(sharename){
			प्रत्यागम sub__remove_shared(context, subhier, reason, sharename);
		}अन्यथा{
			प्रत्यागम sub_remove_प्रसामान्यम्(context, subhier, reason);
		}
	}

	HASH_FIND(hh, subhier->children, topics[0], वाक्यदैर्घ्यम्(topics[0]), branch);
	यदि(branch){
		sub__remove_recurse(context, branch, &(topics[1]), reason, sharename);
		यदि(!branch->children && !branch->subs && !branch->shared){
			HASH_DELETE(hh, subhier->children, branch);
			mosquitto_FREE(branch);
		}
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क sub_अन्वेषणम्(संरचना mosquitto__subhier *subhier, वर्ण **विभज_topics, ध्रुव वर्ण *source_id, ध्रुव वर्ण *topic, uint8_t qos, पूर्णाङ्क retain, संरचना mosquitto_आधारः_सन्देशः *stored)
{
	/* FIXME - need to take into account source_id if the client is a bridge */
	संरचना mosquitto__subhier *branch;
	पूर्णाङ्क rc;
	bool have_subscribers = false;

	यदि(विभज_topics && विभज_topics[0]){
		/* Check for literal match */
		HASH_FIND(hh, subhier->children, विभज_topics[0], वाक्यदैर्घ्यम्(विभज_topics[0]), branch);

		यदि(branch){
			rc = sub_अन्वेषणम्(branch, &(विभज_topics[1]), source_id, topic, qos, retain, stored);
			यदि(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}अन्यथा यदि(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				प्रत्यागम rc;
			}
			यदि(विभज_topics[1] == रिक्त){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				यदि(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}अन्यथा यदि(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					प्रत्यागम rc;
				}
			}
		}

		/* Check for + match */
		HASH_FIND_BYHASHVALUE(hh, subhier->children, "+", 1, hashv_plus, branch);

		यदि(branch){
			rc = sub_अन्वेषणम्(branch, &(विभज_topics[1]), source_id, topic, qos, retain, stored);
			यदि(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}अन्यथा यदि(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				प्रत्यागम rc;
			}
			यदि(विभज_topics[1] == रिक्त){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				यदि(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}अन्यथा यदि(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					प्रत्यागम rc;
				}
			}
		}
	}

	/* Check for # match */
	HASH_FIND_BYHASHVALUE(hh, subhier->children, "#", 1, hashv_hash, branch);
	यदि(branch && !branch->children){
		/* The topic matches due to a # wildcard - process the
		 * subscriptions but *don't* return. Although this branch has ended
		 * there may still be other subscriptions to deal with.
		 */
		rc = subs__process(branch, source_id, topic, qos, retain, stored);
		यदि(rc == MOSQ_ERR_SUCCESS){
			have_subscribers = true;
		}अन्यथा यदि(rc != MOSQ_ERR_NO_SUBSCRIBERS){
			प्रत्यागम rc;
		}
	}

	यदि(have_subscribers){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}अन्यथा{
		प्रत्यागम MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


स्थिर संरचना mosquitto__subhier *sub_सङ्कलय_hier_प्रविष्टिः(संरचना mosquitto__subhier *parent, संरचना mosquitto__subhier **sibling, ध्रुव वर्ण *topic, uint16_t len)
{
	संरचना mosquitto__subhier *child;

	प्रतिज्ञा(sibling);

	child = mosquitto_calloc(1, परिमाण(संरचना mosquitto__subhier) + len + 1);
	यदि(!child){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		प्रत्यागम रिक्त;
	}
	child->parent = parent;
	child->topic_len = len;
	यदि(len > 0){
		strncpy(child->topic, topic, (size_t)(len+1));
	}

	HASH_सङ्कलय(hh, *sibling, topic, child->topic_len, child);

	प्रत्यागम child;
}


पूर्णाङ्क sub_सङ्कलय(संरचना mosquitto *context, ध्रुव संरचना mosquitto_subscription *sub)
{
	पूर्णाङ्क rc = 0;
	संरचना mosquitto__subhier *subhier;
	ध्रुव वर्ण *sharename = रिक्त;
	वर्ण *local_sub;
	वर्ण **topics;
	size_t topiclen;

	प्रतिज्ञा(sub);
	प्रतिज्ञा(sub->topic_filter);

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &topics, &sharename);
	यदि(rc){
		प्रत्यागम rc;
	}

	topiclen = वाक्यदैर्घ्यम्(topics[0]);
	यदि(topiclen > UINT16_MAX){
		mosquitto_FREE(local_sub);
		mosquitto_FREE(topics);
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	यदि(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], topiclen, subhier);
		यदि(!subhier){
			subhier = sub_सङ्कलय_hier_प्रविष्टिः(रिक्त, &db.shared_subs, topics[0], (uint16_t)topiclen);
			यदि(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
		}
	}अन्यथा{
		HASH_FIND(hh, db.प्रसामान्यम्_subs, topics[0], topiclen, subhier);
		यदि(!subhier){
			subhier = sub_सङ्कलय_hier_प्रविष्टिः(रिक्त, &db.प्रसामान्यम्_subs, topics[0], (uint16_t)topiclen);
			यदि(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
		}
	}
	rc = sub_सङ्कलय_context(context, sub, subhier, topics, sharename);

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	प्रत्यागम rc;
}


पूर्णाङ्क sub__remove(संरचना mosquitto *context, ध्रुव वर्ण *sub, uint8_t *reason)
{
	पूर्णाङ्क rc = 0;
	संरचना mosquitto__subhier *subhier;
	ध्रुव वर्ण *sharename = रिक्त;
	वर्ण *local_sub = रिक्त;
	वर्ण **topics = रिक्त;

	प्रतिज्ञा(sub);

	rc = sub__topic_tokenise(sub, &local_sub, &topics, &sharename);
	यदि(rc){
		प्रत्यागम rc;
	}

	यदि(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], वाक्यदैर्घ्यम्(topics[0]), subhier);
	}अन्यथा{
		HASH_FIND(hh, db.प्रसामान्यम्_subs, topics[0], वाक्यदैर्घ्यम्(topics[0]), subhier);
	}
	यदि(subhier){
		*reason = MQTT_RC_NO_SUBSCRIPTION_EXISTED;
		rc = sub__remove_recurse(context, subhier, topics, reason, sharename);
	}

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	प्रत्यागम rc;
}


पूर्णाङ्क sub__messages_queue(ध्रुव वर्ण *source_id, uint8_t qos, संरचना mosquitto_आधारः_सन्देशः **stored)
{
	पूर्णाङ्क rc = MOSQ_ERR_SUCCESS, rc2;
	पूर्णाङ्क rc_प्रसामान्यम् = MOSQ_ERR_NO_SUBSCRIBERS, rc_shared = MOSQ_ERR_NO_SUBSCRIBERS;
	संरचना mosquitto__subhier *subhier;
	वर्ण **विभज_topics = रिक्त;
	वर्ण *local_topic = रिक्त;
	अचिह्नित hashv;
	size_t topiclen;
	bool retain = (*stored)->दत्त.retain;
	ध्रुव वर्ण *topic = (*stored)->दत्त.topic;

	प्रतिज्ञा(topic);

	यदि(sub__topic_tokenise(topic, &local_topic, &विभज_topics, रिक्त)){
		प्रत्यागम 1;
	}

	/* Protect this message until we have sent it to all
	clients - this is required because websockets client calls
	db__message_write(), which could remove the message if ref_count==0.
	*/
	db_सन्देशः_store_ref_inc(*stored);

	topiclen = वाक्यदैर्घ्यम्(विभज_topics[0]);
	HASH_मानम्(विभज_topics[0], topiclen, hashv);
	HASH_FIND_BYHASHVALUE(hh, db.प्रसामान्यम्_subs, विभज_topics[0], topiclen, hashv, subhier);
	यदि(subhier){
		rc_प्रसामान्यम् = sub_अन्वेषणम्(subhier, विभज_topics, source_id, topic, qos, retain, *stored);
		यदि(rc_प्रसामान्यम् > 0){
			rc = rc_प्रसामान्यम्;
			गच्छ अन्तः;
		}
	}

	HASH_FIND_BYHASHVALUE(hh, db.shared_subs, विभज_topics[0], topiclen, hashv, subhier);
	यदि(subhier){
		rc_shared = sub_अन्वेषणम्(subhier, विभज_topics, source_id, topic, qos, retain, *stored);
		यदि(rc_shared > 0){
			rc = rc_shared;
			गच्छ अन्तः;
		}
	}

	यदि(rc_प्रसामान्यम् == MOSQ_ERR_NO_SUBSCRIBERS && rc_shared == MOSQ_ERR_NO_SUBSCRIBERS){
		rc = MOSQ_ERR_NO_SUBSCRIBERS;
	}

	यदि(retain){
		rc2 = retain__store(topic, *stored, विभज_topics, true);
		यदि(rc2){
			rc = rc2;
		}
	}

अन्तः:
	mosquitto_FREE(विभज_topics);
	mosquitto_FREE(local_topic);
	/* Remove our reference and free if needed. */
	db_सन्देशः_store_ref_dec(stored);

	प्रत्यागम rc;
}


/* Remove a subhier element, and return its parent if that needs freeing as well. */
स्थिर संरचना mosquitto__subhier *tmp_remove_subs(संरचना mosquitto__subhier *sub)
{
	संरचना mosquitto__subhier *parent;

	यदि(!sub || !sub->parent){
		प्रत्यागम रिक्त;
	}

	यदि(sub->children || sub->subs){
		प्रत्यागम रिक्त;
	}

	parent = sub->parent;
	HASH_DELETE(hh, parent->children, sub);
	mosquitto_FREE(sub);

	यदि(parent->subs == रिक्त
			&& parent->children == रिक्त
			&& parent->shared == रिक्त
			&& parent->parent){

		प्रत्यागम parent;
	}अन्यथा{
		प्रत्यागम रिक्त;
	}
}


/* Remove all subscriptions for a client.
 */
पूर्णाङ्क sub__clean_session(संरचना mosquitto *context)
{
	प्रति(पूर्णाङ्क i=0; i<context->subs_क्षमता; i++){
		यदि(context->subs[i] == रिक्त || context->subs[i]->hier == रिक्त){
			अनुवर्त;
		}

		संरचना mosquitto__subhier *hier = context->subs[i]->hier;
		संरचना mosquitto__subleaf *पर्णम्;

		plugin_persist__handle_subscription_delete(context, context->subs[i]->topic_filter);
		यदि(context->subs[i]->shared){
			पर्णम् = context->subs[i]->shared->subs;
			यावत्(पर्णम्){
				यदि(पर्णम्->context==context){
#यदि_परिभाषितम् WITH_SYS_वृक्षः
					db.shared_subscription_गणनम्--;
#यदि_अन्तः
					sub_remove_shared_पर्णम्(context->subs[i]->hier, context->subs[i]->shared, पर्णम्);
					भङ्ग;
				}
				पर्णम् = पर्णम्->अग्र;
			}
		}अन्यथा{
			पर्णम् = hier->subs;
			यावत्(पर्णम्){
				यदि(पर्णम्->context==context){
#यदि_परिभाषितम् WITH_SYS_वृक्षः
					db.subscription_गणनम्--;
#यदि_अन्तः
					DL_DELETE(hier->subs, पर्णम्);
					भङ्ग;
				}
				पर्णम् = पर्णम्->अग्र;
			}
		}
		mosquitto_FREE(context->subs[i]);

		यदि(hier->subs == रिक्त
				&& hier->children == रिक्त
				&& hier->shared == रिक्त
				&& hier->parent){

			कुरु{
				hier = tmp_remove_subs(hier);
			}यावत्(hier);
		}
	}
	mosquitto_FREE(context->subs);
	context->subs_क्षमता = 0;
	context->subs_गणनम् = 0;

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


शून्य sub_वृक्षः_print(संरचना mosquitto__subhier *मूलम्, पूर्णाङ्क level)
{
	पूर्णाङ्क i;
	संरचना mosquitto__subhier *branch, *branch_tmp;
	संरचना mosquitto__subleaf *पर्णम्;

	HASH_ITER(hh, मूलम्, branch, branch_tmp){
		यदि(level > -1){
			प्रति(i=0; i<(level+2)*2; i++){
				प्रारूप_लिख(" ");
			}
			प्रारूप_लिख("%s", branch->topic);
			पर्णम् = branch->subs;
			यावत्(पर्णम्){
				यदि(पर्णम्->context){
					प्रारूप_लिख(" (%s, %d)", पर्णम्->context->id, MQTT_SUB_OPT_GET_QOS(पर्णम्->subscription_options));
				}अन्यथा{
					प्रारूप_लिख(" (%s, %d)", "", MQTT_SUB_OPT_GET_QOS(पर्णम्->subscription_options));
				}
				पर्णम् = पर्णम्->अग्र;
			}
			प्रारूप_लिख("\n");
		}

		sub_वृक्षः_print(branch->children, level+1);
	}
}


पूर्णाङ्क sub__init(शून्य)
{
	HASH_मानम्("+", 1, hashv_plus);
	HASH_मानम्("#", 1, hashv_hash);

	यदि(sub_सङ्कलय_hier_प्रविष्टिः(रिक्त, &db.shared_subs, "", 0) == रिक्त
			|| sub_सङ्कलय_hier_प्रविष्टिः(रिक्त, &db.प्रसामान्यम्_subs, "", 0) == रिक्त
			|| sub_सङ्कलय_hier_प्रविष्टिः(रिक्त, &db.प्रसामान्यम्_subs, "$SYS", (uint16_t)वाक्यदैर्घ्यम्("$SYS")) == रिक्त
			){

		प्रत्यागम MOSQ_ERR_NOMEM;
	}अन्यथा{
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}
}
