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

#समावेश "config.h"

#समावेश <assert.h>
#समावेश <stdio.h>
#समावेश <string.h>

#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/mqtt_protocol.h"
#समावेश "util_mosq.h"

#समावेश "utlist.h"

अटल काष्ठा mosquitto__subhier *sub__add_hier_entry(काष्ठा mosquitto__subhier *parent, काष्ठा mosquitto__subhier **sibling, स्थिर अक्षर *topic, uint16_t len);

अटल अचिन्हित पूर्णांक hashv_plus = 0;
अटल अचिन्हित पूर्णांक hashv_hash = 0;


अटल पूर्णांक subs__send(काष्ठा mosquitto__subleaf *leaf, स्थिर अक्षर *topic, uint8_t qos, पूर्णांक retain, काष्ठा mosquitto_base_संदेश *stored)
{
	bool client_retain;
	uint16_t mid;
	uint8_t client_qos, संदेश_qos;
	पूर्णांक rc2;

	/* Check for ACL topic access. */
	rc2 = mosquitto_acl_check(leaf->context, topic, stored->तथ्य.payloadlen, stored->तथ्य.payload, stored->तथ्य.qos, stored->तथ्य.retain, stored->तथ्य.properties, MOSQ_ACL_READ);
	अगर(rc2 == MOSQ_ERR_ACL_DENIED){
		वापस MOSQ_ERR_SUCCESS;
	}अन्यथा अगर(rc2 == MOSQ_ERR_SUCCESS){
		client_qos = MQTT_SUB_OPT_GET_QOS(leaf->subscription_options);

		अगर(db.config->upgrade_outgoing_qos){
			संदेश_qos = client_qos;
		}अन्यथा{
			अगर(qos > client_qos){
				संदेश_qos = client_qos;
			}अन्यथा{
				संदेश_qos = qos;
			}
		}
		अगर(संदेश_qos){
			mid = mosquitto__mid_generate(leaf->context);
		}अन्यथा{
			mid = 0;
		}
		अगर(MQTT_SUB_OPT_GET_RETAIN_AS_PUBLISHED(leaf->subscription_options)){
			client_retain = retain;
		}अन्यथा{
			client_retain = false;
		}
		अगर(db_message_जोड़ो_outgoing(leaf->context, 0, mid, संदेश_qos, client_retain, stored, leaf->identifier, true, true) == 1){
			वापस 1;
		}
	}अन्यथा{
		वापस 1; /* Application error */
	}
	वापस 0;
}


अटल पूर्णांक subs__shared_process(काष्ठा mosquitto__subhier *hier, स्थिर अक्षर *topic, uint8_t qos, पूर्णांक retain, काष्ठा mosquitto_base_संदेश *stored)
{
	पूर्णांक rc = 0, rc2;
	काष्ठा mosquitto__subshared *shared, *shared_tmp;
	काष्ठा mosquitto__subleaf *leaf;

	HASH_ITER(hh, hier->shared, shared, shared_tmp){
		leaf = shared->subs;
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		/* Remove current from the top, add back to the bottom */
		DL_DELETE(shared->subs, leaf);
		DL_APPEND(shared->subs, leaf);

		अगर(rc2){
			rc = 1;
		}
	}

	वापस rc;
}


अटल पूर्णांक subs__process(काष्ठा mosquitto__subhier *hier, स्थिर अक्षर *source_id, स्थिर अक्षर *topic, uint8_t qos, पूर्णांक retain, काष्ठा mosquitto_base_संदेश *stored)
{
	पूर्णांक rc = 0;
	पूर्णांक rc2;
	काष्ठा mosquitto__subleaf *leaf;

	rc = subs__shared_process(hier, topic, qos, retain, stored);

	leaf = hier->subs;
	जबतक(source_id && leaf){
		अगर(!leaf->context->id || (MQTT_SUB_OPT_GET_NO_LOCAL(leaf->subscription_options) && !म_भेद(leaf->context->id, source_id))){
			leaf = leaf->अग्र;
			जारी;
		}
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		अगर(rc2){
			rc = 1;
		}
		leaf = leaf->अग्र;
	}
	अगर(hier->subs || hier->shared){
		वापस rc;
	}अन्यथा{
		वापस MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


अटल पूर्णांक sub__add_leaf(काष्ठा mosquitto *context, स्थिर काष्ठा mosquitto_subscription *sub, काष्ठा mosquitto__subleaf **शीर्ष, काष्ठा mosquitto__subleaf **newleaf)
{
	काष्ठा mosquitto__subleaf *leaf;

	*newleaf = शून्य;
	leaf = *शीर्ष;

	जबतक(leaf){
		अगर(leaf->context && leaf->context->id && !म_भेद(leaf->context->id, context->id)){
			/* Client making a second subscription to same topic. Only
			 * need to update QoS. Return MOSQ_ERR_SUB_EXISTS to
			 * indicate this to the calling function. */
			leaf->identifier = sub->identifier;
			leaf->subscription_options = sub->options;
			वापस MOSQ_ERR_SUB_EXISTS;
		}
		leaf = leaf->अग्र;
	}
	leaf = mosquitto_calloc(1, माप(काष्ठा mosquitto__subleaf) + म_माप(sub->topic_filter) + 1);
	अगर(!leaf){
		वापस MOSQ_ERR_NOMEM;
	}
	leaf->context = context;
	leaf->identifier = sub->identifier;
	leaf->subscription_options = sub->options;
	म_नकल(leaf->topic_filter, sub->topic_filter);

	DL_APPEND(*शीर्ष, leaf);
	*newleaf = leaf;

	वापस MOSQ_ERR_SUCCESS;
}


अटल व्योम sub__remove_shared_leaf(काष्ठा mosquitto__subhier *subhier, काष्ठा mosquitto__subshared *shared, काष्ठा mosquitto__subleaf *leaf)
{
	DL_DELETE(shared->subs, leaf);
	अगर(shared->subs == शून्य){
		HASH_DELETE(hh, subhier->shared, shared);
		mosquitto_FREE(shared);
	}
}


अटल पूर्णांक sub__add_shared(काष्ठा mosquitto *context, स्थिर काष्ठा mosquitto_subscription *sub, काष्ठा mosquitto__subhier *subhier, स्थिर अक्षर *sharename)
{
	काष्ठा mosquitto__subleaf *newleaf;
	काष्ठा mosquitto__subshared *shared = शून्य;
	काष्ठा mosquitto__subleaf **subs;
	माप_प्रकार slen;
	पूर्णांक rc;
	अचिन्हित hashv;

	slen = म_माप(sharename);

	HASH_मूल्य(sharename, slen, hashv);

	HASH_FIND_BYHASHVALUE(hh, subhier->shared, sharename, slen, hashv, shared);
	अगर(shared == शून्य){
		shared = mosquitto_calloc(1, माप(काष्ठा mosquitto__subshared) + slen + 1);
		अगर(!shared){
			वापस MOSQ_ERR_NOMEM;
		}
		म_नकलन(shared->name, sharename, slen+1);

		HASH_ADD_BYHASHVALUE(hh, subhier->shared, name, slen, hashv, shared);
	}

	rc = sub__add_leaf(context, sub, &shared->subs, &newleaf);
	अगर(rc > 0){
		अगर(shared->subs == शून्य){
			HASH_DELETE(hh, subhier->shared, shared);
			mosquitto_FREE(shared);
		}
		वापस rc;
	}

	अगर(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = shared;

		bool assigned = false;
		क्रम(पूर्णांक i=0; i<context->subs_capacity; i++){
			अगर(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_गिनती++;
				assigned = true;
				अवरोध;
			}
		}
		अगर(assigned == false){
			subs = mosquitto_realloc(context->subs, माप(काष्ठा mosquitto__subleaf *)*(माप_प्रकार)(context->subs_capacity + 1));
			अगर(!subs){
				sub__remove_shared_leaf(subhier, shared, newleaf);
				mosquitto_FREE(newleaf);
				वापस MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_capacity++;
			context->subs_गिनती++;
			context->subs[context->subs_capacity-1] = newleaf;
		}
#अगर_घोषित WITH_SYS_वृक्ष
		db.shared_subscription_गिनती++;
#पूर्ण_अगर
	}

	अगर(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		वापस rc;
	}अन्यथा{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		वापस MOSQ_ERR_SUCCESS;
	}
}


अटल पूर्णांक sub__add_normal(काष्ठा mosquitto *context, स्थिर काष्ठा mosquitto_subscription *sub, काष्ठा mosquitto__subhier *subhier)
{
	काष्ठा mosquitto__subleaf *newleaf = शून्य;
	काष्ठा mosquitto__subleaf **subs;
	पूर्णांक rc;

	rc = sub__add_leaf(context, sub, &subhier->subs, &newleaf);
	अगर(rc > 0){
		वापस rc;
	}

	अगर(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = शून्य;

		bool assigned = false;
		क्रम(पूर्णांक i=0; i<context->subs_capacity; i++){
			अगर(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_गिनती++;
				assigned = true;
				अवरोध;
			}
		}
		अगर(assigned == false){
			subs = mosquitto_realloc(context->subs, माप(काष्ठा mosquitto__subleaf *)*(माप_प्रकार)(context->subs_capacity + 1));
			अगर(!subs){
				DL_DELETE(subhier->subs, newleaf);
				mosquitto_FREE(newleaf);
				वापस MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_capacity++;
			context->subs_गिनती++;
			context->subs[context->subs_capacity-1] = newleaf;
		}
#अगर_घोषित WITH_SYS_वृक्ष
		db.subscription_गिनती++;
#पूर्ण_अगर
	}

	अगर(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		वापस rc;
	}अन्यथा{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		वापस MOSQ_ERR_SUCCESS;
	}
}


अटल पूर्णांक sub__add_context(काष्ठा mosquitto *context, स्थिर काष्ठा mosquitto_subscription *sub, काष्ठा mosquitto__subhier *subhier, अक्षर *स्थिर *स्थिर topics, स्थिर अक्षर *sharename)
{
	काष्ठा mosquitto__subhier *branch;
	पूर्णांक topic_सूचकांक = 0;
	माप_प्रकार topiclen;

	/* Find leaf node */
	जबतक(topics && topics[topic_सूचकांक] != शून्य){
		topiclen = म_माप(topics[topic_सूचकांक]);
		अगर(topiclen > UINT16_MAX){
			वापस MOSQ_ERR_INVAL;
		}
		HASH_FIND(hh, subhier->children, topics[topic_सूचकांक], topiclen, branch);
		अगर(!branch){
			/* Not found */
			branch = sub__add_hier_entry(subhier, &subhier->children, topics[topic_सूचकांक], (uint16_t)topiclen);
			अगर(!branch){
				वापस MOSQ_ERR_NOMEM;
			}
		}
		subhier = branch;
		topic_सूचकांक++;
	}

	/* Add add our context */
	अगर(context && context->id){
		अगर(sharename){
			वापस sub__add_shared(context, sub, subhier, sharename);
		}अन्यथा{
			वापस sub__add_normal(context, sub, subhier);
		}
	}अन्यथा{
		वापस MOSQ_ERR_SUCCESS;
	}
}


अटल पूर्णांक sub__remove_normal(काष्ठा mosquitto *context, काष्ठा mosquitto__subhier *subhier, uint8_t *reason)
{
	काष्ठा mosquitto__subleaf *leaf;

	leaf = subhier->subs;
	जबतक(leaf){
		अगर(leaf->context==context){
#अगर_घोषित WITH_SYS_वृक्ष
			db.subscription_गिनती--;
#पूर्ण_अगर
			DL_DELETE(subhier->subs, leaf);

			/* Remove the reference to the sub that the client is keeping.
			 * It would be nice to be able to use the reference directly,
			 * but that would involve keeping a copy of the topic string in
			 * each subleaf. Might be worth considering though. */
			क्रम(पूर्णांक i=0; i<context->subs_capacity; i++){
				अगर(context->subs[i] && context->subs[i]->hier == subhier){
					context->subs_गिनती--;
					mosquitto_free(context->subs[i]);
					context->subs[i] = शून्य;
					अवरोध;
				}
			}
			*reason = 0;
			वापस MOSQ_ERR_SUCCESS;
		}
		leaf = leaf->अग्र;
	}
	वापस MOSQ_ERR_NO_SUBSCRIBERS;
}


अटल पूर्णांक sub__remove_shared(काष्ठा mosquitto *context, काष्ठा mosquitto__subhier *subhier, uint8_t *reason, स्थिर अक्षर *sharename)
{
	काष्ठा mosquitto__subshared *shared;

	HASH_FIND(hh, subhier->shared, sharename, म_माप(sharename), shared);
	अगर(shared){
		काष्ठा mosquitto__subleaf *leaf = shared->subs;
		जबतक(leaf){
			अगर(leaf->context==context){
#अगर_घोषित WITH_SYS_वृक्ष
				db.shared_subscription_गिनती--;
#पूर्ण_अगर
				DL_DELETE(shared->subs, leaf);

				/* Remove the reference to the sub that the client is keeping.
				* It would be nice to be able to use the reference directly,
				* but that would involve keeping a copy of the topic string in
				* each subleaf. Might be worth considering though. */
				क्रम(पूर्णांक i=0; i<context->subs_capacity; i++){
					अगर(context->subs[i]
							&& context->subs[i]->hier == subhier
							&& context->subs[i]->shared == shared){

						mosquitto_free(context->subs[i]);
						context->subs[i] = शून्य;
						context->subs_गिनती--;
						अवरोध;
					}
				}

				अगर(shared->subs == शून्य){
					HASH_DELETE(hh, subhier->shared, shared);
					mosquitto_FREE(shared);
				}

				*reason = 0;
				वापस MOSQ_ERR_SUCCESS;
			}
			leaf = leaf->अग्र;
		}
		वापस MOSQ_ERR_NO_SUBSCRIBERS;
	}अन्यथा{
		वापस MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


अटल पूर्णांक sub__remove_recurse(काष्ठा mosquitto *context, काष्ठा mosquitto__subhier *subhier, अक्षर **topics, uint8_t *reason, स्थिर अक्षर *sharename)
{
	काष्ठा mosquitto__subhier *branch;

	अगर(topics == शून्य || topics[0] == शून्य){
		अगर(sharename){
			वापस sub__remove_shared(context, subhier, reason, sharename);
		}अन्यथा{
			वापस sub__remove_normal(context, subhier, reason);
		}
	}

	HASH_FIND(hh, subhier->children, topics[0], म_माप(topics[0]), branch);
	अगर(branch){
		sub__remove_recurse(context, branch, &(topics[1]), reason, sharename);
		अगर(!branch->children && !branch->subs && !branch->shared){
			HASH_DELETE(hh, subhier->children, branch);
			mosquitto_FREE(branch);
		}
	}
	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक sub_खोज(काष्ठा mosquitto__subhier *subhier, अक्षर **split_topics, स्थिर अक्षर *source_id, स्थिर अक्षर *topic, uint8_t qos, पूर्णांक retain, काष्ठा mosquitto_base_संदेश *stored)
{
	/* FIXME - need to take into account source_id if the client is a bridge */
	काष्ठा mosquitto__subhier *branch;
	पूर्णांक rc;
	bool have_subscribers = false;

	अगर(split_topics && split_topics[0]){
		/* Check for literal match */
		HASH_FIND(hh, subhier->children, split_topics[0], म_माप(split_topics[0]), branch);

		अगर(branch){
			rc = sub_खोज(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			अगर(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}अन्यथा अगर(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				वापस rc;
			}
			अगर(split_topics[1] == शून्य){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				अगर(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}अन्यथा अगर(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					वापस rc;
				}
			}
		}

		/* Check for + match */
		HASH_FIND_BYHASHVALUE(hh, subhier->children, "+", 1, hashv_plus, branch);

		अगर(branch){
			rc = sub_खोज(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			अगर(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}अन्यथा अगर(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				वापस rc;
			}
			अगर(split_topics[1] == शून्य){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				अगर(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}अन्यथा अगर(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					वापस rc;
				}
			}
		}
	}

	/* Check for # match */
	HASH_FIND_BYHASHVALUE(hh, subhier->children, "#", 1, hashv_hash, branch);
	अगर(branch && !branch->children){
		/* The topic matches due to a # wildcard - process the
		 * subscriptions but *don't* return. Although this branch has ended
		 * there may still be other subscriptions to deal with.
		 */
		rc = subs__process(branch, source_id, topic, qos, retain, stored);
		अगर(rc == MOSQ_ERR_SUCCESS){
			have_subscribers = true;
		}अन्यथा अगर(rc != MOSQ_ERR_NO_SUBSCRIBERS){
			वापस rc;
		}
	}

	अगर(have_subscribers){
		वापस MOSQ_ERR_SUCCESS;
	}अन्यथा{
		वापस MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


अटल काष्ठा mosquitto__subhier *sub__add_hier_entry(काष्ठा mosquitto__subhier *parent, काष्ठा mosquitto__subhier **sibling, स्थिर अक्षर *topic, uint16_t len)
{
	काष्ठा mosquitto__subhier *child;

	निश्चित(sibling);

	child = mosquitto_calloc(1, माप(काष्ठा mosquitto__subhier) + len + 1);
	अगर(!child){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
		वापस शून्य;
	}
	child->parent = parent;
	child->topic_len = len;
	अगर(len > 0){
		म_नकलन(child->topic, topic, (माप_प्रकार)(len+1));
	}

	HASH_ADD(hh, *sibling, topic, child->topic_len, child);

	वापस child;
}


पूर्णांक sub__add(काष्ठा mosquitto *context, स्थिर काष्ठा mosquitto_subscription *sub)
{
	पूर्णांक rc = 0;
	काष्ठा mosquitto__subhier *subhier;
	स्थिर अक्षर *sharename = शून्य;
	अक्षर *local_sub;
	अक्षर **topics;
	माप_प्रकार topiclen;

	निश्चित(sub);
	निश्चित(sub->topic_filter);

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &topics, &sharename);
	अगर(rc){
		वापस rc;
	}

	topiclen = म_माप(topics[0]);
	अगर(topiclen > UINT16_MAX){
		mosquitto_FREE(local_sub);
		mosquitto_FREE(topics);
		वापस MOSQ_ERR_INVAL;
	}

	अगर(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], topiclen, subhier);
		अगर(!subhier){
			subhier = sub__add_hier_entry(शून्य, &db.shared_subs, topics[0], (uint16_t)topiclen);
			अगर(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
				वापस MOSQ_ERR_NOMEM;
			}
		}
	}अन्यथा{
		HASH_FIND(hh, db.normal_subs, topics[0], topiclen, subhier);
		अगर(!subhier){
			subhier = sub__add_hier_entry(शून्य, &db.normal_subs, topics[0], (uint16_t)topiclen);
			अगर(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
				वापस MOSQ_ERR_NOMEM;
			}
		}
	}
	rc = sub__add_context(context, sub, subhier, topics, sharename);

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	वापस rc;
}


पूर्णांक sub__remove(काष्ठा mosquitto *context, स्थिर अक्षर *sub, uint8_t *reason)
{
	पूर्णांक rc = 0;
	काष्ठा mosquitto__subhier *subhier;
	स्थिर अक्षर *sharename = शून्य;
	अक्षर *local_sub = शून्य;
	अक्षर **topics = शून्य;

	निश्चित(sub);

	rc = sub__topic_tokenise(sub, &local_sub, &topics, &sharename);
	अगर(rc){
		वापस rc;
	}

	अगर(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], म_माप(topics[0]), subhier);
	}अन्यथा{
		HASH_FIND(hh, db.normal_subs, topics[0], म_माप(topics[0]), subhier);
	}
	अगर(subhier){
		*reason = MQTT_RC_NO_SUBSCRIPTION_EXISTED;
		rc = sub__remove_recurse(context, subhier, topics, reason, sharename);
	}

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	वापस rc;
}


पूर्णांक sub__messages_queue(स्थिर अक्षर *source_id, uint8_t qos, काष्ठा mosquitto_base_संदेश **stored)
{
	पूर्णांक rc = MOSQ_ERR_SUCCESS, rc2;
	पूर्णांक rc_normal = MOSQ_ERR_NO_SUBSCRIBERS, rc_shared = MOSQ_ERR_NO_SUBSCRIBERS;
	काष्ठा mosquitto__subhier *subhier;
	अक्षर **split_topics = शून्य;
	अक्षर *local_topic = शून्य;
	अचिन्हित hashv;
	माप_प्रकार topiclen;
	bool retain = (*stored)->तथ्य.retain;
	स्थिर अक्षर *topic = (*stored)->तथ्य.topic;

	निश्चित(topic);

	अगर(sub__topic_tokenise(topic, &local_topic, &split_topics, शून्य)){
		वापस 1;
	}

	/* Protect this message until we have sent it to all
	clients - this is required because websockets client calls
	db__message_write(), which could remove the message if ref_count==0.
	*/
	db_संदेश_store_ref_inc(*stored);

	topiclen = म_माप(split_topics[0]);
	HASH_मूल्य(split_topics[0], topiclen, hashv);
	HASH_FIND_BYHASHVALUE(hh, db.normal_subs, split_topics[0], topiclen, hashv, subhier);
	अगर(subhier){
		rc_normal = sub_खोज(subhier, split_topics, source_id, topic, qos, retain, *stored);
		अगर(rc_normal > 0){
			rc = rc_normal;
			जाओ end;
		}
	}

	HASH_FIND_BYHASHVALUE(hh, db.shared_subs, split_topics[0], topiclen, hashv, subhier);
	अगर(subhier){
		rc_shared = sub_खोज(subhier, split_topics, source_id, topic, qos, retain, *stored);
		अगर(rc_shared > 0){
			rc = rc_shared;
			जाओ end;
		}
	}

	अगर(rc_normal == MOSQ_ERR_NO_SUBSCRIBERS && rc_shared == MOSQ_ERR_NO_SUBSCRIBERS){
		rc = MOSQ_ERR_NO_SUBSCRIBERS;
	}

	अगर(retain){
		rc2 = retain__store(topic, *stored, split_topics, true);
		अगर(rc2){
			rc = rc2;
		}
	}

end:
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);
	/* Remove our reference and free if needed. */
	db_संदेश_store_ref_dec(stored);

	वापस rc;
}


/* Remove a subhier element, and return its parent if that needs freeing as well. */
अटल काष्ठा mosquitto__subhier *tmp_remove_subs(काष्ठा mosquitto__subhier *sub)
{
	काष्ठा mosquitto__subhier *parent;

	अगर(!sub || !sub->parent){
		वापस शून्य;
	}

	अगर(sub->children || sub->subs){
		वापस शून्य;
	}

	parent = sub->parent;
	HASH_DELETE(hh, parent->children, sub);
	mosquitto_FREE(sub);

	अगर(parent->subs == शून्य
			&& parent->children == शून्य
			&& parent->shared == शून्य
			&& parent->parent){

		वापस parent;
	}अन्यथा{
		वापस शून्य;
	}
}


/* Remove all subscriptions for a client.
 */
पूर्णांक sub__clean_session(काष्ठा mosquitto *context)
{
	क्रम(पूर्णांक i=0; i<context->subs_capacity; i++){
		अगर(context->subs[i] == शून्य || context->subs[i]->hier == शून्य){
			जारी;
		}

		काष्ठा mosquitto__subhier *hier = context->subs[i]->hier;
		काष्ठा mosquitto__subleaf *leaf;

		plugin_persist__handle_subscription_delete(context, context->subs[i]->topic_filter);
		अगर(context->subs[i]->shared){
			leaf = context->subs[i]->shared->subs;
			जबतक(leaf){
				अगर(leaf->context==context){
#अगर_घोषित WITH_SYS_वृक्ष
					db.shared_subscription_गिनती--;
#पूर्ण_अगर
					sub__remove_shared_leaf(context->subs[i]->hier, context->subs[i]->shared, leaf);
					अवरोध;
				}
				leaf = leaf->अग्र;
			}
		}अन्यथा{
			leaf = hier->subs;
			जबतक(leaf){
				अगर(leaf->context==context){
#अगर_घोषित WITH_SYS_वृक्ष
					db.subscription_गिनती--;
#पूर्ण_अगर
					DL_DELETE(hier->subs, leaf);
					अवरोध;
				}
				leaf = leaf->अग्र;
			}
		}
		mosquitto_FREE(context->subs[i]);

		अगर(hier->subs == शून्य
				&& hier->children == शून्य
				&& hier->shared == शून्य
				&& hier->parent){

			करो{
				hier = tmp_remove_subs(hier);
			}जबतक(hier);
		}
	}
	mosquitto_FREE(context->subs);
	context->subs_capacity = 0;
	context->subs_गिनती = 0;

	वापस MOSQ_ERR_SUCCESS;
}


व्योम sub_वृक्ष_print(काष्ठा mosquitto__subhier *मूल, पूर्णांक level)
{
	पूर्णांक i;
	काष्ठा mosquitto__subhier *branch, *branch_tmp;
	काष्ठा mosquitto__subleaf *leaf;

	HASH_ITER(hh, मूल, branch, branch_tmp){
		अगर(level > -1){
			क्रम(i=0; i<(level+2)*2; i++){
				म_लिखो(" ");
			}
			म_लिखो("%s", branch->topic);
			leaf = branch->subs;
			जबतक(leaf){
				अगर(leaf->context){
					म_लिखो(" (%s, %d)", leaf->context->id, MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}अन्यथा{
					म_लिखो(" (%s, %d)", "", MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}
				leaf = leaf->अग्र;
			}
			म_लिखो("\n");
		}

		sub_वृक्ष_print(branch->children, level+1);
	}
}


पूर्णांक sub__init(व्योम)
{
	HASH_मूल्य("+", 1, hashv_plus);
	HASH_मूल्य("#", 1, hashv_hash);

	अगर(sub__add_hier_entry(शून्य, &db.shared_subs, "", 0) == शून्य
			|| sub__add_hier_entry(शून्य, &db.normal_subs, "", 0) == शून्य
			|| sub__add_hier_entry(शून्य, &db.normal_subs, "$SYS", (uint16_t)म_माप("$SYS")) == शून्य
			){

		वापस MOSQ_ERR_NOMEM;
	}अन्यथा{
		वापस MOSQ_ERR_SUCCESS;
	}
}
