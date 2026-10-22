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

स्थिर संरचना mosquitto__subhier *sub__add_hier_entry(संरचना mosquitto__subhier *parent, संरचना mosquitto__subhier **sibling, अचर वर्ण *topic, uint16_t len);

स्थिर अचिन्हित पूर्णाङ्क hashv_plus = 0;
स्थिर अचिन्हित पूर्णाङ्क hashv_hash = 0;


स्थिर पूर्णाङ्क subs__send(संरचना mosquitto__subleaf *leaf, अचर वर्ण *topic, uint8_t qos, पूर्णाङ्क retain, संरचना mosquitto_base_सन्देश *stored)
{
	bool client_retain;
	uint16_t mid;
	uint8_t client_qos, सन्देश_qos;
	पूर्णाङ्क rc2;

	/* Check for ACL topic access. */
	rc2 = mosquitto_acl_check(leaf->context, topic, stored->data.payloadlen, stored->data.payload, stored->data.qos, stored->data.retain, stored->data.properties, MOSQ_ACL_READ);
	यदि(rc2 == MOSQ_ERR_ACL_DENIED){
		फर्काऊ MOSQ_ERR_SUCCESS;
	}अन्यथा यदि(rc2 == MOSQ_ERR_SUCCESS){
		client_qos = MQTT_SUB_OPT_GET_QOS(leaf->subscription_options);

		यदि(db.config->upgrade_outgoing_qos){
			सन्देश_qos = client_qos;
		}अन्यथा{
			यदि(qos > client_qos){
				सन्देश_qos = client_qos;
			}अन्यथा{
				सन्देश_qos = qos;
			}
		}
		यदि(सन्देश_qos){
			mid = mosquitto__mid_generate(leaf->context);
		}अन्यथा{
			mid = 0;
		}
		यदि(MQTT_SUB_OPT_GET_RETAIN_AS_PUBLISHED(leaf->subscription_options)){
			client_retain = retain;
		}अन्यथा{
			client_retain = false;
		}
		यदि(db_message_घुसाउ_outgoing(leaf->context, 0, mid, सन्देश_qos, client_retain, stored, leaf->identifier, true, true) == 1){
			फर्काऊ 1;
		}
	}अन्यथा{
		फर्काऊ 1; /* Application error */
	}
	फर्काऊ 0;
}


स्थिर पूर्णाङ्क subs__shared_process(संरचना mosquitto__subhier *hier, अचर वर्ण *topic, uint8_t qos, पूर्णाङ्क retain, संरचना mosquitto_base_सन्देश *stored)
{
	पूर्णाङ्क rc = 0, rc2;
	संरचना mosquitto__subshared *shared, *shared_tmp;
	संरचना mosquitto__subleaf *leaf;

	HASH_ITER(hh, hier->shared, shared, shared_tmp){
		leaf = shared->subs;
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		/* Remove current from the top, add back to the bottom */
		DL_DELETE(shared->subs, leaf);
		DL_APPEND(shared->subs, leaf);

		यदि(rc2){
			rc = 1;
		}
	}

	फर्काऊ rc;
}


स्थिर पूर्णाङ्क subs__process(संरचना mosquitto__subhier *hier, अचर वर्ण *source_id, अचर वर्ण *topic, uint8_t qos, पूर्णाङ्क retain, संरचना mosquitto_base_सन्देश *stored)
{
	पूर्णाङ्क rc = 0;
	पूर्णाङ्क rc2;
	संरचना mosquitto__subleaf *leaf;

	rc = subs__shared_process(hier, topic, qos, retain, stored);

	leaf = hier->subs;
	जबसम्म(source_id && leaf){
		यदि(!leaf->context->id || (MQTT_SUB_OPT_GET_NO_LOCAL(leaf->subscription_options) && !म_भेद(leaf->context->id, source_id))){
			leaf = leaf->next;
			जारी;
		}
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		यदि(rc2){
			rc = 1;
		}
		leaf = leaf->next;
	}
	यदि(hier->subs || hier->shared){
		फर्काऊ rc;
	}अन्यथा{
		फर्काऊ MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


स्थिर पूर्णाङ्क sub__add_leaf(संरचना mosquitto *context, अचर संरचना mosquitto_subscription *sub, संरचना mosquitto__subleaf **head, संरचना mosquitto__subleaf **newleaf)
{
	संरचना mosquitto__subleaf *leaf;

	*newleaf = रिक्त;
	leaf = *head;

	जबसम्म(leaf){
		यदि(leaf->context && leaf->context->id && !म_भेद(leaf->context->id, context->id)){
			/* Client making a second subscription to same topic. Only
			 * need to update QoS. Return MOSQ_ERR_SUB_EXISTS to
			 * indicate this to the calling function. */
			leaf->identifier = sub->identifier;
			leaf->subscription_options = sub->options;
			फर्काऊ MOSQ_ERR_SUB_EXISTS;
		}
		leaf = leaf->next;
	}
	leaf = mosquitto_calloc(1, परिमाण(संरचना mosquitto__subleaf) + म_माप(sub->topic_filter) + 1);
	यदि(!leaf){
		फर्काऊ MOSQ_ERR_NOMEM;
	}
	leaf->context = context;
	leaf->identifier = sub->identifier;
	leaf->subscription_options = sub->options;
	म_नकल(leaf->topic_filter, sub->topic_filter);

	DL_APPEND(*head, leaf);
	*newleaf = leaf;

	फर्काऊ MOSQ_ERR_SUCCESS;
}


स्थिर शून्य sub__remove_shared_leaf(संरचना mosquitto__subhier *subhier, संरचना mosquitto__subshared *shared, संरचना mosquitto__subleaf *leaf)
{
	DL_DELETE(shared->subs, leaf);
	यदि(shared->subs == रिक्त){
		HASH_DELETE(hh, subhier->shared, shared);
		mosquitto_FREE(shared);
	}
}


स्थिर पूर्णाङ्क sub__add_shared(संरचना mosquitto *context, अचर संरचना mosquitto_subscription *sub, संरचना mosquitto__subhier *subhier, अचर वर्ण *sharename)
{
	संरचना mosquitto__subleaf *newleaf;
	संरचना mosquitto__subshared *shared = रिक्त;
	संरचना mosquitto__subleaf **subs;
	माप_प्रकार slen;
	पूर्णाङ्क rc;
	अचिन्हित hashv;

	slen = म_माप(sharename);

	HASH_मान(sharename, slen, hashv);

	HASH_FIND_BYHASHVALUE(hh, subhier->shared, sharename, slen, hashv, shared);
	यदि(shared == रिक्त){
		shared = mosquitto_calloc(1, परिमाण(संरचना mosquitto__subshared) + slen + 1);
		यदि(!shared){
			फर्काऊ MOSQ_ERR_NOMEM;
		}
		म_नकलन(shared->name, sharename, slen+1);

		HASH_ADD_BYHASHVALUE(hh, subhier->shared, name, slen, hashv, shared);
	}

	rc = sub__add_leaf(context, sub, &shared->subs, &newleaf);
	यदि(rc > 0){
		यदि(shared->subs == रिक्त){
			HASH_DELETE(hh, subhier->shared, shared);
			mosquitto_FREE(shared);
		}
		फर्काऊ rc;
	}

	यदि(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = shared;

		bool assigned = false;
		लागि(पूर्णाङ्क i=0; i<context->subs_capacity; i++){
			यदि(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_गन्ती++;
				assigned = true;
				रोक;
			}
		}
		यदि(assigned == false){
			subs = mosquitto_realloc(context->subs, परिमाण(संरचना mosquitto__subleaf *)*(माप_प्रकार)(context->subs_capacity + 1));
			यदि(!subs){
				sub__remove_shared_leaf(subhier, shared, newleaf);
				mosquitto_FREE(newleaf);
				फर्काऊ MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_capacity++;
			context->subs_गन्ती++;
			context->subs[context->subs_capacity-1] = newleaf;
		}
#ifdef WITH_SYS_रूख
		db.shared_subscription_गन्ती++;
#endif
	}

	यदि(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		फर्काऊ rc;
	}अन्यथा{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		फर्काऊ MOSQ_ERR_SUCCESS;
	}
}


स्थिर पूर्णाङ्क sub__add_normal(संरचना mosquitto *context, अचर संरचना mosquitto_subscription *sub, संरचना mosquitto__subhier *subhier)
{
	संरचना mosquitto__subleaf *newleaf = रिक्त;
	संरचना mosquitto__subleaf **subs;
	पूर्णाङ्क rc;

	rc = sub__add_leaf(context, sub, &subhier->subs, &newleaf);
	यदि(rc > 0){
		फर्काऊ rc;
	}

	यदि(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = रिक्त;

		bool assigned = false;
		लागि(पूर्णाङ्क i=0; i<context->subs_capacity; i++){
			यदि(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_गन्ती++;
				assigned = true;
				रोक;
			}
		}
		यदि(assigned == false){
			subs = mosquitto_realloc(context->subs, परिमाण(संरचना mosquitto__subleaf *)*(माप_प्रकार)(context->subs_capacity + 1));
			यदि(!subs){
				DL_DELETE(subhier->subs, newleaf);
				mosquitto_FREE(newleaf);
				फर्काऊ MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_capacity++;
			context->subs_गन्ती++;
			context->subs[context->subs_capacity-1] = newleaf;
		}
#ifdef WITH_SYS_रूख
		db.subscription_गन्ती++;
#endif
	}

	यदि(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		फर्काऊ rc;
	}अन्यथा{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		फर्काऊ MOSQ_ERR_SUCCESS;
	}
}


स्थिर पूर्णाङ्क sub__add_context(संरचना mosquitto *context, अचर संरचना mosquitto_subscription *sub, संरचना mosquitto__subhier *subhier, वर्ण *अचर *अचर topics, अचर वर्ण *sharename)
{
	संरचना mosquitto__subhier *branch;
	पूर्णाङ्क topic_index = 0;
	माप_प्रकार topiclen;

	/* Find leaf node */
	जबसम्म(topics && topics[topic_index] != रिक्त){
		topiclen = म_माप(topics[topic_index]);
		यदि(topiclen > UINT16_MAX){
			फर्काऊ MOSQ_ERR_INVAL;
		}
		HASH_FIND(hh, subhier->children, topics[topic_index], topiclen, branch);
		यदि(!branch){
			/* Not found */
			branch = sub__add_hier_entry(subhier, &subhier->children, topics[topic_index], (uint16_t)topiclen);
			यदि(!branch){
				फर्काऊ MOSQ_ERR_NOMEM;
			}
		}
		subhier = branch;
		topic_index++;
	}

	/* Add add our context */
	यदि(context && context->id){
		यदि(sharename){
			फर्काऊ sub__add_shared(context, sub, subhier, sharename);
		}अन्यथा{
			फर्काऊ sub__add_normal(context, sub, subhier);
		}
	}अन्यथा{
		फर्काऊ MOSQ_ERR_SUCCESS;
	}
}


स्थिर पूर्णाङ्क sub__remove_normal(संरचना mosquitto *context, संरचना mosquitto__subhier *subhier, uint8_t *reason)
{
	संरचना mosquitto__subleaf *leaf;

	leaf = subhier->subs;
	जबसम्म(leaf){
		यदि(leaf->context==context){
#ifdef WITH_SYS_रूख
			db.subscription_गन्ती--;
#endif
			DL_DELETE(subhier->subs, leaf);

			/* Remove the reference to the sub that the client is keeping.
			 * It would be nice to be able to use the reference directly,
			 * but that would involve keeping a copy of the topic string in
			 * each subleaf. Might be worth considering though. */
			लागि(पूर्णाङ्क i=0; i<context->subs_capacity; i++){
				यदि(context->subs[i] && context->subs[i]->hier == subhier){
					context->subs_गन्ती--;
					mosquitto_free(context->subs[i]);
					context->subs[i] = रिक्त;
					रोक;
				}
			}
			*reason = 0;
			फर्काऊ MOSQ_ERR_SUCCESS;
		}
		leaf = leaf->next;
	}
	फर्काऊ MOSQ_ERR_NO_SUBSCRIBERS;
}


स्थिर पूर्णाङ्क sub__remove_shared(संरचना mosquitto *context, संरचना mosquitto__subhier *subhier, uint8_t *reason, अचर वर्ण *sharename)
{
	संरचना mosquitto__subshared *shared;

	HASH_FIND(hh, subhier->shared, sharename, म_माप(sharename), shared);
	यदि(shared){
		संरचना mosquitto__subleaf *leaf = shared->subs;
		जबसम्म(leaf){
			यदि(leaf->context==context){
#ifdef WITH_SYS_रूख
				db.shared_subscription_गन्ती--;
#endif
				DL_DELETE(shared->subs, leaf);

				/* Remove the reference to the sub that the client is keeping.
				* It would be nice to be able to use the reference directly,
				* but that would involve keeping a copy of the topic string in
				* each subleaf. Might be worth considering though. */
				लागि(पूर्णाङ्क i=0; i<context->subs_capacity; i++){
					यदि(context->subs[i]
							&& context->subs[i]->hier == subhier
							&& context->subs[i]->shared == shared){

						mosquitto_free(context->subs[i]);
						context->subs[i] = रिक्त;
						context->subs_गन्ती--;
						रोक;
					}
				}

				यदि(shared->subs == रिक्त){
					HASH_DELETE(hh, subhier->shared, shared);
					mosquitto_FREE(shared);
				}

				*reason = 0;
				फर्काऊ MOSQ_ERR_SUCCESS;
			}
			leaf = leaf->next;
		}
		फर्काऊ MOSQ_ERR_NO_SUBSCRIBERS;
	}अन्यथा{
		फर्काऊ MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


स्थिर पूर्णाङ्क sub__remove_recurse(संरचना mosquitto *context, संरचना mosquitto__subhier *subhier, वर्ण **topics, uint8_t *reason, अचर वर्ण *sharename)
{
	संरचना mosquitto__subhier *branch;

	यदि(topics == रिक्त || topics[0] == रिक्त){
		यदि(sharename){
			फर्काऊ sub__remove_shared(context, subhier, reason, sharename);
		}अन्यथा{
			फर्काऊ sub__remove_normal(context, subhier, reason);
		}
	}

	HASH_FIND(hh, subhier->children, topics[0], म_माप(topics[0]), branch);
	यदि(branch){
		sub__remove_recurse(context, branch, &(topics[1]), reason, sharename);
		यदि(!branch->children && !branch->subs && !branch->shared){
			HASH_DELETE(hh, subhier->children, branch);
			mosquitto_FREE(branch);
		}
	}
	फर्काऊ MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क sub_खोज(संरचना mosquitto__subhier *subhier, वर्ण **split_topics, अचर वर्ण *source_id, अचर वर्ण *topic, uint8_t qos, पूर्णाङ्क retain, संरचना mosquitto_base_सन्देश *stored)
{
	/* FIXME - need to take into account source_id if the client is a bridge */
	संरचना mosquitto__subhier *branch;
	पूर्णाङ्क rc;
	bool have_subscribers = false;

	यदि(split_topics && split_topics[0]){
		/* Check for literal match */
		HASH_FIND(hh, subhier->children, split_topics[0], म_माप(split_topics[0]), branch);

		यदि(branch){
			rc = sub_खोज(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			यदि(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}अन्यथा यदि(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				फर्काऊ rc;
			}
			यदि(split_topics[1] == रिक्त){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				यदि(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}अन्यथा यदि(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					फर्काऊ rc;
				}
			}
		}

		/* Check for + match */
		HASH_FIND_BYHASHVALUE(hh, subhier->children, "+", 1, hashv_plus, branch);

		यदि(branch){
			rc = sub_खोज(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			यदि(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}अन्यथा यदि(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				फर्काऊ rc;
			}
			यदि(split_topics[1] == रिक्त){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				यदि(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}अन्यथा यदि(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					फर्काऊ rc;
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
			फर्काऊ rc;
		}
	}

	यदि(have_subscribers){
		फर्काऊ MOSQ_ERR_SUCCESS;
	}अन्यथा{
		फर्काऊ MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


स्थिर संरचना mosquitto__subhier *sub__add_hier_entry(संरचना mosquitto__subhier *parent, संरचना mosquitto__subhier **sibling, अचर वर्ण *topic, uint16_t len)
{
	संरचना mosquitto__subhier *child;

	प्रतिज्ञा(sibling);

	child = mosquitto_calloc(1, परिमाण(संरचना mosquitto__subhier) + len + 1);
	यदि(!child){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		फर्काऊ रिक्त;
	}
	child->parent = parent;
	child->topic_len = len;
	यदि(len > 0){
		म_नकलन(child->topic, topic, (माप_प्रकार)(len+1));
	}

	HASH_ADD(hh, *sibling, topic, child->topic_len, child);

	फर्काऊ child;
}


पूर्णाङ्क sub__add(संरचना mosquitto *context, अचर संरचना mosquitto_subscription *sub)
{
	पूर्णाङ्क rc = 0;
	संरचना mosquitto__subhier *subhier;
	अचर वर्ण *sharename = रिक्त;
	वर्ण *local_sub;
	वर्ण **topics;
	माप_प्रकार topiclen;

	प्रतिज्ञा(sub);
	प्रतिज्ञा(sub->topic_filter);

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &topics, &sharename);
	यदि(rc){
		फर्काऊ rc;
	}

	topiclen = म_माप(topics[0]);
	यदि(topiclen > UINT16_MAX){
		mosquitto_FREE(local_sub);
		mosquitto_FREE(topics);
		फर्काऊ MOSQ_ERR_INVAL;
	}

	यदि(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], topiclen, subhier);
		यदि(!subhier){
			subhier = sub__add_hier_entry(रिक्त, &db.shared_subs, topics[0], (uint16_t)topiclen);
			यदि(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
				फर्काऊ MOSQ_ERR_NOMEM;
			}
		}
	}अन्यथा{
		HASH_FIND(hh, db.normal_subs, topics[0], topiclen, subhier);
		यदि(!subhier){
			subhier = sub__add_hier_entry(रिक्त, &db.normal_subs, topics[0], (uint16_t)topiclen);
			यदि(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
				फर्काऊ MOSQ_ERR_NOMEM;
			}
		}
	}
	rc = sub__add_context(context, sub, subhier, topics, sharename);

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	फर्काऊ rc;
}


पूर्णाङ्क sub__remove(संरचना mosquitto *context, अचर वर्ण *sub, uint8_t *reason)
{
	पूर्णाङ्क rc = 0;
	संरचना mosquitto__subhier *subhier;
	अचर वर्ण *sharename = रिक्त;
	वर्ण *local_sub = रिक्त;
	वर्ण **topics = रिक्त;

	प्रतिज्ञा(sub);

	rc = sub__topic_tokenise(sub, &local_sub, &topics, &sharename);
	यदि(rc){
		फर्काऊ rc;
	}

	यदि(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], म_माप(topics[0]), subhier);
	}अन्यथा{
		HASH_FIND(hh, db.normal_subs, topics[0], म_माप(topics[0]), subhier);
	}
	यदि(subhier){
		*reason = MQTT_RC_NO_SUBSCRIPTION_EXISTED;
		rc = sub__remove_recurse(context, subhier, topics, reason, sharename);
	}

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	फर्काऊ rc;
}


पूर्णाङ्क sub__messages_queue(अचर वर्ण *source_id, uint8_t qos, संरचना mosquitto_base_सन्देश **stored)
{
	पूर्णाङ्क rc = MOSQ_ERR_SUCCESS, rc2;
	पूर्णाङ्क rc_normal = MOSQ_ERR_NO_SUBSCRIBERS, rc_shared = MOSQ_ERR_NO_SUBSCRIBERS;
	संरचना mosquitto__subhier *subhier;
	वर्ण **split_topics = रिक्त;
	वर्ण *local_topic = रिक्त;
	अचिन्हित hashv;
	माप_प्रकार topiclen;
	bool retain = (*stored)->data.retain;
	अचर वर्ण *topic = (*stored)->data.topic;

	प्रतिज्ञा(topic);

	यदि(sub__topic_tokenise(topic, &local_topic, &split_topics, रिक्त)){
		फर्काऊ 1;
	}

	/* Protect this message until we have sent it to all
	clients - this is required because websockets client calls
	db__message_write(), which could remove the message if ref_count==0.
	*/
	db_सन्देश_store_ref_inc(*stored);

	topiclen = म_माप(split_topics[0]);
	HASH_मान(split_topics[0], topiclen, hashv);
	HASH_FIND_BYHASHVALUE(hh, db.normal_subs, split_topics[0], topiclen, hashv, subhier);
	यदि(subhier){
		rc_normal = sub_खोज(subhier, split_topics, source_id, topic, qos, retain, *stored);
		यदि(rc_normal > 0){
			rc = rc_normal;
			गच्छ end;
		}
	}

	HASH_FIND_BYHASHVALUE(hh, db.shared_subs, split_topics[0], topiclen, hashv, subhier);
	यदि(subhier){
		rc_shared = sub_खोज(subhier, split_topics, source_id, topic, qos, retain, *stored);
		यदि(rc_shared > 0){
			rc = rc_shared;
			गच्छ end;
		}
	}

	यदि(rc_normal == MOSQ_ERR_NO_SUBSCRIBERS && rc_shared == MOSQ_ERR_NO_SUBSCRIBERS){
		rc = MOSQ_ERR_NO_SUBSCRIBERS;
	}

	यदि(retain){
		rc2 = retain__store(topic, *stored, split_topics, true);
		यदि(rc2){
			rc = rc2;
		}
	}

end:
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);
	/* Remove our reference and free if needed. */
	db_सन्देश_store_ref_dec(stored);

	फर्काऊ rc;
}


/* Remove a subhier element, and return its parent if that needs freeing as well. */
स्थिर संरचना mosquitto__subhier *tmp_remove_subs(संरचना mosquitto__subhier *sub)
{
	संरचना mosquitto__subhier *parent;

	यदि(!sub || !sub->parent){
		फर्काऊ रिक्त;
	}

	यदि(sub->children || sub->subs){
		फर्काऊ रिक्त;
	}

	parent = sub->parent;
	HASH_DELETE(hh, parent->children, sub);
	mosquitto_FREE(sub);

	यदि(parent->subs == रिक्त
			&& parent->children == रिक्त
			&& parent->shared == रिक्त
			&& parent->parent){

		फर्काऊ parent;
	}अन्यथा{
		फर्काऊ रिक्त;
	}
}


/* Remove all subscriptions for a client.
 */
पूर्णाङ्क sub__clean_session(संरचना mosquitto *context)
{
	लागि(पूर्णाङ्क i=0; i<context->subs_capacity; i++){
		यदि(context->subs[i] == रिक्त || context->subs[i]->hier == रिक्त){
			जारी;
		}

		संरचना mosquitto__subhier *hier = context->subs[i]->hier;
		संरचना mosquitto__subleaf *leaf;

		plugin_persist__handle_subscription_delete(context, context->subs[i]->topic_filter);
		यदि(context->subs[i]->shared){
			leaf = context->subs[i]->shared->subs;
			जबसम्म(leaf){
				यदि(leaf->context==context){
#ifdef WITH_SYS_रूख
					db.shared_subscription_गन्ती--;
#endif
					sub__remove_shared_leaf(context->subs[i]->hier, context->subs[i]->shared, leaf);
					रोक;
				}
				leaf = leaf->next;
			}
		}अन्यथा{
			leaf = hier->subs;
			जबसम्म(leaf){
				यदि(leaf->context==context){
#ifdef WITH_SYS_रूख
					db.subscription_गन्ती--;
#endif
					DL_DELETE(hier->subs, leaf);
					रोक;
				}
				leaf = leaf->next;
			}
		}
		mosquitto_FREE(context->subs[i]);

		यदि(hier->subs == रिक्त
				&& hier->children == रिक्त
				&& hier->shared == रिक्त
				&& hier->parent){

			गर{
				hier = tmp_remove_subs(hier);
			}जबसम्म(hier);
		}
	}
	mosquitto_FREE(context->subs);
	context->subs_capacity = 0;
	context->subs_गन्ती = 0;

	फर्काऊ MOSQ_ERR_SUCCESS;
}


शून्य sub_रूख_print(संरचना mosquitto__subhier *जरा, पूर्णाङ्क level)
{
	पूर्णाङ्क i;
	संरचना mosquitto__subhier *branch, *branch_tmp;
	संरचना mosquitto__subleaf *leaf;

	HASH_ITER(hh, जरा, branch, branch_tmp){
		यदि(level > -1){
			लागि(i=0; i<(level+2)*2; i++){
				म_लिखो(" ");
			}
			म_लिखो("%s", branch->topic);
			leaf = branch->subs;
			जबसम्म(leaf){
				यदि(leaf->context){
					म_लिखो(" (%s, %d)", leaf->context->id, MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}अन्यथा{
					म_लिखो(" (%s, %d)", "", MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}
				leaf = leaf->next;
			}
			म_लिखो("\n");
		}

		sub_रूख_print(branch->children, level+1);
	}
}


पूर्णाङ्क sub__init(शून्य)
{
	HASH_मान("+", 1, hashv_plus);
	HASH_मान("#", 1, hashv_hash);

	यदि(sub__add_hier_entry(रिक्त, &db.shared_subs, "", 0) == रिक्त
			|| sub__add_hier_entry(रिक्त, &db.normal_subs, "", 0) == रिक्त
			|| sub__add_hier_entry(रिक्त, &db.normal_subs, "$SYS", (uint16_t)म_माप("$SYS")) == रिक्त
			){

		फर्काऊ MOSQ_ERR_NOMEM;
	}अन्यथा{
		फर्काऊ MOSQ_ERR_SUCCESS;
	}
}
