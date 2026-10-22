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

स्थिर संरचना mosquitto__subhier *sub__add_hier_entry(संरचना mosquitto__subhier *parent, संरचना mosquitto__subhier **sibling, स्थिरांक अक्षर *topic, uint16_t len);

स्थिर अचिन्हित पूर्णांक hashv_plus = 0;
स्थिर अचिन्हित पूर्णांक hashv_hash = 0;


स्थिर पूर्णांक subs__send(संरचना mosquitto__subleaf *leaf, स्थिरांक अक्षर *topic, uint8_t qos, पूर्णांक retain, संरचना mosquitto_base_संदेश *stored)
{
	bool client_retain;
	uint16_t mid;
	uint8_t client_qos, संदेश_qos;
	पूर्णांक rc2;

	/* Check for ACL topic access. */
	rc2 = mosquitto_acl_check(leaf->context, topic, stored->data.payloadlen, stored->data.payload, stored->data.qos, stored->data.retain, stored->data.properties, MOSQ_ACL_READ);
	जर(rc2 == MOSQ_ERR_ACL_DENIED){
		परत MOSQ_ERR_SUCCESS;
	}अन्यथा जर(rc2 == MOSQ_ERR_SUCCESS){
		client_qos = MQTT_SUB_OPT_GET_QOS(leaf->subscription_options);

		जर(db.config->upgrade_outgoing_qos){
			संदेश_qos = client_qos;
		}अन्यथा{
			जर(qos > client_qos){
				संदेश_qos = client_qos;
			}अन्यथा{
				संदेश_qos = qos;
			}
		}
		जर(संदेश_qos){
			mid = mosquitto__mid_generate(leaf->context);
		}अन्यथा{
			mid = 0;
		}
		जर(MQTT_SUB_OPT_GET_RETAIN_AS_PUBLISHED(leaf->subscription_options)){
			client_retain = retain;
		}अन्यथा{
			client_retain = false;
		}
		जर(db_message_घाला_outgoing(leaf->context, 0, mid, संदेश_qos, client_retain, stored, leaf->identifier, true, true) == 1){
			परत 1;
		}
	}अन्यथा{
		परत 1; /* Application error */
	}
	परत 0;
}


स्थिर पूर्णांक subs__shared_process(संरचना mosquitto__subhier *hier, स्थिरांक अक्षर *topic, uint8_t qos, पूर्णांक retain, संरचना mosquitto_base_संदेश *stored)
{
	पूर्णांक rc = 0, rc2;
	संरचना mosquitto__subshared *shared, *shared_tmp;
	संरचना mosquitto__subleaf *leaf;

	HASH_ITER(hh, hier->shared, shared, shared_tmp){
		leaf = shared->subs;
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		/* Remove current from the top, add back to the bottom */
		DL_DELETE(shared->subs, leaf);
		DL_APPEND(shared->subs, leaf);

		जर(rc2){
			rc = 1;
		}
	}

	परत rc;
}


स्थिर पूर्णांक subs__process(संरचना mosquitto__subhier *hier, स्थिरांक अक्षर *source_id, स्थिरांक अक्षर *topic, uint8_t qos, पूर्णांक retain, संरचना mosquitto_base_संदेश *stored)
{
	पूर्णांक rc = 0;
	पूर्णांक rc2;
	संरचना mosquitto__subleaf *leaf;

	rc = subs__shared_process(hier, topic, qos, retain, stored);

	leaf = hier->subs;
	पर्यंत(source_id && leaf){
		जर(!leaf->context->id || (MQTT_SUB_OPT_GET_NO_LOCAL(leaf->subscription_options) && !म_भेद(leaf->context->id, source_id))){
			leaf = leaf->next;
			सुरूठेवा;
		}
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		जर(rc2){
			rc = 1;
		}
		leaf = leaf->next;
	}
	जर(hier->subs || hier->shared){
		परत rc;
	}अन्यथा{
		परत MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


स्थिर पूर्णांक sub__add_leaf(संरचना mosquitto *context, स्थिरांक संरचना mosquitto_subscription *sub, संरचना mosquitto__subleaf **head, संरचना mosquitto__subleaf **newleaf)
{
	संरचना mosquitto__subleaf *leaf;

	*newleaf = रिक्त;
	leaf = *head;

	पर्यंत(leaf){
		जर(leaf->context && leaf->context->id && !म_भेद(leaf->context->id, context->id)){
			/* Client making a second subscription to same topic. Only
			 * need to update QoS. Return MOSQ_ERR_SUB_EXISTS to
			 * indicate this to the calling function. */
			leaf->identifier = sub->identifier;
			leaf->subscription_options = sub->options;
			परत MOSQ_ERR_SUB_EXISTS;
		}
		leaf = leaf->next;
	}
	leaf = mosquitto_calloc(1, आकार(संरचना mosquitto__subleaf) + म_माप(sub->topic_filter) + 1);
	जर(!leaf){
		परत MOSQ_ERR_NOMEM;
	}
	leaf->context = context;
	leaf->identifier = sub->identifier;
	leaf->subscription_options = sub->options;
	म_नकल(leaf->topic_filter, sub->topic_filter);

	DL_APPEND(*head, leaf);
	*newleaf = leaf;

	परत MOSQ_ERR_SUCCESS;
}


स्थिर शून्य sub__remove_shared_leaf(संरचना mosquitto__subhier *subhier, संरचना mosquitto__subshared *shared, संरचना mosquitto__subleaf *leaf)
{
	DL_DELETE(shared->subs, leaf);
	जर(shared->subs == रिक्त){
		HASH_DELETE(hh, subhier->shared, shared);
		mosquitto_FREE(shared);
	}
}


स्थिर पूर्णांक sub__add_shared(संरचना mosquitto *context, स्थिरांक संरचना mosquitto_subscription *sub, संरचना mosquitto__subhier *subhier, स्थिरांक अक्षर *sharename)
{
	संरचना mosquitto__subleaf *newleaf;
	संरचना mosquitto__subshared *shared = रिक्त;
	संरचना mosquitto__subleaf **subs;
	माप_प्रकार slen;
	पूर्णांक rc;
	अचिन्हित hashv;

	slen = म_माप(sharename);

	HASH_मूल्य(sharename, slen, hashv);

	HASH_FIND_BYHASHVALUE(hh, subhier->shared, sharename, slen, hashv, shared);
	जर(shared == रिक्त){
		shared = mosquitto_calloc(1, आकार(संरचना mosquitto__subshared) + slen + 1);
		जर(!shared){
			परत MOSQ_ERR_NOMEM;
		}
		म_नकलन(shared->name, sharename, slen+1);

		HASH_ADD_BYHASHVALUE(hh, subhier->shared, name, slen, hashv, shared);
	}

	rc = sub__add_leaf(context, sub, &shared->subs, &newleaf);
	जर(rc > 0){
		जर(shared->subs == रिक्त){
			HASH_DELETE(hh, subhier->shared, shared);
			mosquitto_FREE(shared);
		}
		परत rc;
	}

	जर(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = shared;

		bool assigned = false;
		साठी(पूर्णांक i=0; i<context->subs_capacity; i++){
			जर(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_मोजणी++;
				assigned = true;
				थांबा;
			}
		}
		जर(assigned == false){
			subs = mosquitto_realloc(context->subs, आकार(संरचना mosquitto__subleaf *)*(माप_प्रकार)(context->subs_capacity + 1));
			जर(!subs){
				sub__remove_shared_leaf(subhier, shared, newleaf);
				mosquitto_FREE(newleaf);
				परत MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_capacity++;
			context->subs_मोजणी++;
			context->subs[context->subs_capacity-1] = newleaf;
		}
#ifdef WITH_SYS_वृक्ष
		db.shared_subscription_मोजणी++;
#endif
	}

	जर(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		परत rc;
	}अन्यथा{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		परत MOSQ_ERR_SUCCESS;
	}
}


स्थिर पूर्णांक sub__add_normal(संरचना mosquitto *context, स्थिरांक संरचना mosquitto_subscription *sub, संरचना mosquitto__subhier *subhier)
{
	संरचना mosquitto__subleaf *newleaf = रिक्त;
	संरचना mosquitto__subleaf **subs;
	पूर्णांक rc;

	rc = sub__add_leaf(context, sub, &subhier->subs, &newleaf);
	जर(rc > 0){
		परत rc;
	}

	जर(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = रिक्त;

		bool assigned = false;
		साठी(पूर्णांक i=0; i<context->subs_capacity; i++){
			जर(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_मोजणी++;
				assigned = true;
				थांबा;
			}
		}
		जर(assigned == false){
			subs = mosquitto_realloc(context->subs, आकार(संरचना mosquitto__subleaf *)*(माप_प्रकार)(context->subs_capacity + 1));
			जर(!subs){
				DL_DELETE(subhier->subs, newleaf);
				mosquitto_FREE(newleaf);
				परत MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_capacity++;
			context->subs_मोजणी++;
			context->subs[context->subs_capacity-1] = newleaf;
		}
#ifdef WITH_SYS_वृक्ष
		db.subscription_मोजणी++;
#endif
	}

	जर(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		परत rc;
	}अन्यथा{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		परत MOSQ_ERR_SUCCESS;
	}
}


स्थिर पूर्णांक sub__add_context(संरचना mosquitto *context, स्थिरांक संरचना mosquitto_subscription *sub, संरचना mosquitto__subhier *subhier, अक्षर *स्थिरांक *स्थिरांक topics, स्थिरांक अक्षर *sharename)
{
	संरचना mosquitto__subhier *branch;
	पूर्णांक topic_index = 0;
	माप_प्रकार topiclen;

	/* Find leaf node */
	पर्यंत(topics && topics[topic_index] != रिक्त){
		topiclen = म_माप(topics[topic_index]);
		जर(topiclen > UINT16_MAX){
			परत MOSQ_ERR_INVAL;
		}
		HASH_FIND(hh, subhier->children, topics[topic_index], topiclen, branch);
		जर(!branch){
			/* Not found */
			branch = sub__add_hier_entry(subhier, &subhier->children, topics[topic_index], (uint16_t)topiclen);
			जर(!branch){
				परत MOSQ_ERR_NOMEM;
			}
		}
		subhier = branch;
		topic_index++;
	}

	/* Add add our context */
	जर(context && context->id){
		जर(sharename){
			परत sub__add_shared(context, sub, subhier, sharename);
		}अन्यथा{
			परत sub__add_normal(context, sub, subhier);
		}
	}अन्यथा{
		परत MOSQ_ERR_SUCCESS;
	}
}


स्थिर पूर्णांक sub__remove_normal(संरचना mosquitto *context, संरचना mosquitto__subhier *subhier, uint8_t *reason)
{
	संरचना mosquitto__subleaf *leaf;

	leaf = subhier->subs;
	पर्यंत(leaf){
		जर(leaf->context==context){
#ifdef WITH_SYS_वृक्ष
			db.subscription_मोजणी--;
#endif
			DL_DELETE(subhier->subs, leaf);

			/* Remove the reference to the sub that the client is keeping.
			 * It would be nice to be able to use the reference directly,
			 * but that would involve keeping a copy of the topic string in
			 * each subleaf. Might be worth considering though. */
			साठी(पूर्णांक i=0; i<context->subs_capacity; i++){
				जर(context->subs[i] && context->subs[i]->hier == subhier){
					context->subs_मोजणी--;
					mosquitto_free(context->subs[i]);
					context->subs[i] = रिक्त;
					थांबा;
				}
			}
			*reason = 0;
			परत MOSQ_ERR_SUCCESS;
		}
		leaf = leaf->next;
	}
	परत MOSQ_ERR_NO_SUBSCRIBERS;
}


स्थिर पूर्णांक sub__remove_shared(संरचना mosquitto *context, संरचना mosquitto__subhier *subhier, uint8_t *reason, स्थिरांक अक्षर *sharename)
{
	संरचना mosquitto__subshared *shared;

	HASH_FIND(hh, subhier->shared, sharename, म_माप(sharename), shared);
	जर(shared){
		संरचना mosquitto__subleaf *leaf = shared->subs;
		पर्यंत(leaf){
			जर(leaf->context==context){
#ifdef WITH_SYS_वृक्ष
				db.shared_subscription_मोजणी--;
#endif
				DL_DELETE(shared->subs, leaf);

				/* Remove the reference to the sub that the client is keeping.
				* It would be nice to be able to use the reference directly,
				* but that would involve keeping a copy of the topic string in
				* each subleaf. Might be worth considering though. */
				साठी(पूर्णांक i=0; i<context->subs_capacity; i++){
					जर(context->subs[i]
							&& context->subs[i]->hier == subhier
							&& context->subs[i]->shared == shared){

						mosquitto_free(context->subs[i]);
						context->subs[i] = रिक्त;
						context->subs_मोजणी--;
						थांबा;
					}
				}

				जर(shared->subs == रिक्त){
					HASH_DELETE(hh, subhier->shared, shared);
					mosquitto_FREE(shared);
				}

				*reason = 0;
				परत MOSQ_ERR_SUCCESS;
			}
			leaf = leaf->next;
		}
		परत MOSQ_ERR_NO_SUBSCRIBERS;
	}अन्यथा{
		परत MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


स्थिर पूर्णांक sub__remove_recurse(संरचना mosquitto *context, संरचना mosquitto__subhier *subhier, अक्षर **topics, uint8_t *reason, स्थिरांक अक्षर *sharename)
{
	संरचना mosquitto__subhier *branch;

	जर(topics == रिक्त || topics[0] == रिक्त){
		जर(sharename){
			परत sub__remove_shared(context, subhier, reason, sharename);
		}अन्यथा{
			परत sub__remove_normal(context, subhier, reason);
		}
	}

	HASH_FIND(hh, subhier->children, topics[0], म_माप(topics[0]), branch);
	जर(branch){
		sub__remove_recurse(context, branch, &(topics[1]), reason, sharename);
		जर(!branch->children && !branch->subs && !branch->shared){
			HASH_DELETE(hh, subhier->children, branch);
			mosquitto_FREE(branch);
		}
	}
	परत MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णांक sub_शोध(संरचना mosquitto__subhier *subhier, अक्षर **split_topics, स्थिरांक अक्षर *source_id, स्थिरांक अक्षर *topic, uint8_t qos, पूर्णांक retain, संरचना mosquitto_base_संदेश *stored)
{
	/* FIXME - need to take into account source_id if the client is a bridge */
	संरचना mosquitto__subhier *branch;
	पूर्णांक rc;
	bool have_subscribers = false;

	जर(split_topics && split_topics[0]){
		/* Check for literal match */
		HASH_FIND(hh, subhier->children, split_topics[0], म_माप(split_topics[0]), branch);

		जर(branch){
			rc = sub_शोध(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			जर(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}अन्यथा जर(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				परत rc;
			}
			जर(split_topics[1] == रिक्त){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				जर(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}अन्यथा जर(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					परत rc;
				}
			}
		}

		/* Check for + match */
		HASH_FIND_BYHASHVALUE(hh, subhier->children, "+", 1, hashv_plus, branch);

		जर(branch){
			rc = sub_शोध(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			जर(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}अन्यथा जर(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				परत rc;
			}
			जर(split_topics[1] == रिक्त){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				जर(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}अन्यथा जर(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					परत rc;
				}
			}
		}
	}

	/* Check for # match */
	HASH_FIND_BYHASHVALUE(hh, subhier->children, "#", 1, hashv_hash, branch);
	जर(branch && !branch->children){
		/* The topic matches due to a # wildcard - process the
		 * subscriptions but *don't* return. Although this branch has ended
		 * there may still be other subscriptions to deal with.
		 */
		rc = subs__process(branch, source_id, topic, qos, retain, stored);
		जर(rc == MOSQ_ERR_SUCCESS){
			have_subscribers = true;
		}अन्यथा जर(rc != MOSQ_ERR_NO_SUBSCRIBERS){
			परत rc;
		}
	}

	जर(have_subscribers){
		परत MOSQ_ERR_SUCCESS;
	}अन्यथा{
		परत MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


स्थिर संरचना mosquitto__subhier *sub__add_hier_entry(संरचना mosquitto__subhier *parent, संरचना mosquitto__subhier **sibling, स्थिरांक अक्षर *topic, uint16_t len)
{
	संरचना mosquitto__subhier *child;

	प्रतिज्ञा(sibling);

	child = mosquitto_calloc(1, आकार(संरचना mosquitto__subhier) + len + 1);
	जर(!child){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		परत रिक्त;
	}
	child->parent = parent;
	child->topic_len = len;
	जर(len > 0){
		म_नकलन(child->topic, topic, (माप_प्रकार)(len+1));
	}

	HASH_ADD(hh, *sibling, topic, child->topic_len, child);

	परत child;
}


पूर्णांक sub__add(संरचना mosquitto *context, स्थिरांक संरचना mosquitto_subscription *sub)
{
	पूर्णांक rc = 0;
	संरचना mosquitto__subhier *subhier;
	स्थिरांक अक्षर *sharename = रिक्त;
	अक्षर *local_sub;
	अक्षर **topics;
	माप_प्रकार topiclen;

	प्रतिज्ञा(sub);
	प्रतिज्ञा(sub->topic_filter);

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &topics, &sharename);
	जर(rc){
		परत rc;
	}

	topiclen = म_माप(topics[0]);
	जर(topiclen > UINT16_MAX){
		mosquitto_FREE(local_sub);
		mosquitto_FREE(topics);
		परत MOSQ_ERR_INVAL;
	}

	जर(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], topiclen, subhier);
		जर(!subhier){
			subhier = sub__add_hier_entry(रिक्त, &db.shared_subs, topics[0], (uint16_t)topiclen);
			जर(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
				परत MOSQ_ERR_NOMEM;
			}
		}
	}अन्यथा{
		HASH_FIND(hh, db.normal_subs, topics[0], topiclen, subhier);
		जर(!subhier){
			subhier = sub__add_hier_entry(रिक्त, &db.normal_subs, topics[0], (uint16_t)topiclen);
			जर(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
				परत MOSQ_ERR_NOMEM;
			}
		}
	}
	rc = sub__add_context(context, sub, subhier, topics, sharename);

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	परत rc;
}


पूर्णांक sub__remove(संरचना mosquitto *context, स्थिरांक अक्षर *sub, uint8_t *reason)
{
	पूर्णांक rc = 0;
	संरचना mosquitto__subhier *subhier;
	स्थिरांक अक्षर *sharename = रिक्त;
	अक्षर *local_sub = रिक्त;
	अक्षर **topics = रिक्त;

	प्रतिज्ञा(sub);

	rc = sub__topic_tokenise(sub, &local_sub, &topics, &sharename);
	जर(rc){
		परत rc;
	}

	जर(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], म_माप(topics[0]), subhier);
	}अन्यथा{
		HASH_FIND(hh, db.normal_subs, topics[0], म_माप(topics[0]), subhier);
	}
	जर(subhier){
		*reason = MQTT_RC_NO_SUBSCRIPTION_EXISTED;
		rc = sub__remove_recurse(context, subhier, topics, reason, sharename);
	}

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	परत rc;
}


पूर्णांक sub__messages_queue(स्थिरांक अक्षर *source_id, uint8_t qos, संरचना mosquitto_base_संदेश **stored)
{
	पूर्णांक rc = MOSQ_ERR_SUCCESS, rc2;
	पूर्णांक rc_normal = MOSQ_ERR_NO_SUBSCRIBERS, rc_shared = MOSQ_ERR_NO_SUBSCRIBERS;
	संरचना mosquitto__subhier *subhier;
	अक्षर **split_topics = रिक्त;
	अक्षर *local_topic = रिक्त;
	अचिन्हित hashv;
	माप_प्रकार topiclen;
	bool retain = (*stored)->data.retain;
	स्थिरांक अक्षर *topic = (*stored)->data.topic;

	प्रतिज्ञा(topic);

	जर(sub__topic_tokenise(topic, &local_topic, &split_topics, रिक्त)){
		परत 1;
	}

	/* Protect this message until we have sent it to all
	clients - this is required because websockets client calls
	db__message_write(), which could remove the message if ref_count==0.
	*/
	db_संदेश_store_ref_inc(*stored);

	topiclen = म_माप(split_topics[0]);
	HASH_मूल्य(split_topics[0], topiclen, hashv);
	HASH_FIND_BYHASHVALUE(hh, db.normal_subs, split_topics[0], topiclen, hashv, subhier);
	जर(subhier){
		rc_normal = sub_शोध(subhier, split_topics, source_id, topic, qos, retain, *stored);
		जर(rc_normal > 0){
			rc = rc_normal;
			जा end;
		}
	}

	HASH_FIND_BYHASHVALUE(hh, db.shared_subs, split_topics[0], topiclen, hashv, subhier);
	जर(subhier){
		rc_shared = sub_शोध(subhier, split_topics, source_id, topic, qos, retain, *stored);
		जर(rc_shared > 0){
			rc = rc_shared;
			जा end;
		}
	}

	जर(rc_normal == MOSQ_ERR_NO_SUBSCRIBERS && rc_shared == MOSQ_ERR_NO_SUBSCRIBERS){
		rc = MOSQ_ERR_NO_SUBSCRIBERS;
	}

	जर(retain){
		rc2 = retain__store(topic, *stored, split_topics, true);
		जर(rc2){
			rc = rc2;
		}
	}

end:
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);
	/* Remove our reference and free if needed. */
	db_संदेश_store_ref_dec(stored);

	परत rc;
}


/* Remove a subhier element, and return its parent if that needs freeing as well. */
स्थिर संरचना mosquitto__subhier *tmp_remove_subs(संरचना mosquitto__subhier *sub)
{
	संरचना mosquitto__subhier *parent;

	जर(!sub || !sub->parent){
		परत रिक्त;
	}

	जर(sub->children || sub->subs){
		परत रिक्त;
	}

	parent = sub->parent;
	HASH_DELETE(hh, parent->children, sub);
	mosquitto_FREE(sub);

	जर(parent->subs == रिक्त
			&& parent->children == रिक्त
			&& parent->shared == रिक्त
			&& parent->parent){

		परत parent;
	}अन्यथा{
		परत रिक्त;
	}
}


/* Remove all subscriptions for a client.
 */
पूर्णांक sub__clean_session(संरचना mosquitto *context)
{
	साठी(पूर्णांक i=0; i<context->subs_capacity; i++){
		जर(context->subs[i] == रिक्त || context->subs[i]->hier == रिक्त){
			सुरूठेवा;
		}

		संरचना mosquitto__subhier *hier = context->subs[i]->hier;
		संरचना mosquitto__subleaf *leaf;

		plugin_persist__handle_subscription_delete(context, context->subs[i]->topic_filter);
		जर(context->subs[i]->shared){
			leaf = context->subs[i]->shared->subs;
			पर्यंत(leaf){
				जर(leaf->context==context){
#ifdef WITH_SYS_वृक्ष
					db.shared_subscription_मोजणी--;
#endif
					sub__remove_shared_leaf(context->subs[i]->hier, context->subs[i]->shared, leaf);
					थांबा;
				}
				leaf = leaf->next;
			}
		}अन्यथा{
			leaf = hier->subs;
			पर्यंत(leaf){
				जर(leaf->context==context){
#ifdef WITH_SYS_वृक्ष
					db.subscription_मोजणी--;
#endif
					DL_DELETE(hier->subs, leaf);
					थांबा;
				}
				leaf = leaf->next;
			}
		}
		mosquitto_FREE(context->subs[i]);

		जर(hier->subs == रिक्त
				&& hier->children == रिक्त
				&& hier->shared == रिक्त
				&& hier->parent){

			करा{
				hier = tmp_remove_subs(hier);
			}पर्यंत(hier);
		}
	}
	mosquitto_FREE(context->subs);
	context->subs_capacity = 0;
	context->subs_मोजणी = 0;

	परत MOSQ_ERR_SUCCESS;
}


शून्य sub_वृक्ष_print(संरचना mosquitto__subhier *मूळ, पूर्णांक level)
{
	पूर्णांक i;
	संरचना mosquitto__subhier *branch, *branch_tmp;
	संरचना mosquitto__subleaf *leaf;

	HASH_ITER(hh, मूळ, branch, branch_tmp){
		जर(level > -1){
			साठी(i=0; i<(level+2)*2; i++){
				म_लिखो(" ");
			}
			म_लिखो("%s", branch->topic);
			leaf = branch->subs;
			पर्यंत(leaf){
				जर(leaf->context){
					म_लिखो(" (%s, %d)", leaf->context->id, MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}अन्यथा{
					म_लिखो(" (%s, %d)", "", MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}
				leaf = leaf->next;
			}
			म_लिखो("\n");
		}

		sub_वृक्ष_print(branch->children, level+1);
	}
}


पूर्णांक sub__init(शून्य)
{
	HASH_मूल्य("+", 1, hashv_plus);
	HASH_मूल्य("#", 1, hashv_hash);

	जर(sub__add_hier_entry(रिक्त, &db.shared_subs, "", 0) == रिक्त
			|| sub__add_hier_entry(रिक्त, &db.normal_subs, "", 0) == रिक्त
			|| sub__add_hier_entry(रिक्त, &db.normal_subs, "$SYS", (uint16_t)म_माप("$SYS")) == रिक्त
			){

		परत MOSQ_ERR_NOMEM;
	}अन्यथा{
		परत MOSQ_ERR_SUCCESS;
	}
}
