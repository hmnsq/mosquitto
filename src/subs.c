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

स्थिर संरचना mosquitto__subhier *sub__add_hier_entry(संरचना mosquitto__subhier *parent, संरचना mosquitto__subhier **sibling, ध्रुव वर्ण *topic, uint16_t len);

स्थिर unsigned पूर्णाङ्क hashv_plus = 0;
स्थिर unsigned पूर्णाङ्क hashv_hash = 0;


स्थिर पूर्णाङ्क subs__send(संरचना mosquitto__subleaf *leaf, ध्रुव वर्ण *topic, uint8_t qos, पूर्णाङ्क retain, संरचना mosquitto_base_सन्देशः *stored)
{
	bool client_retain;
	uint16_t mid;
	uint8_t client_qos, सन्देशः_qos;
	पूर्णाङ्क rc2;

	/* Check for ACL topic access. */
	rc2 = mosquitto_acl_check(leaf->context, topic, stored->data.payloadlen, stored->data.payload, stored->data.qos, stored->data.retain, stored->data.properties, MOSQ_ACL_READ);
	सचे(rc2 == MOSQ_ERR_ACL_DENIED){
		पटिनिवत्त MOSQ_ERR_SUCCESS;
	}अञ्ञथा सचे(rc2 == MOSQ_ERR_SUCCESS){
		client_qos = MQTT_SUB_OPT_GET_QOS(leaf->subscription_options);

		सचे(db.config->upgrade_outgoing_qos){
			सन्देशः_qos = client_qos;
		}अञ्ञथा{
			सचे(qos > client_qos){
				सन्देशः_qos = client_qos;
			}अञ्ञथा{
				सन्देशः_qos = qos;
			}
		}
		सचे(सन्देशः_qos){
			mid = mosquitto__mid_generate(leaf->context);
		}अञ्ञथा{
			mid = 0;
		}
		सचे(MQTT_SUB_OPT_GET_RETAIN_AS_PUBLISHED(leaf->subscription_options)){
			client_retain = retain;
		}अञ्ञथा{
			client_retain = false;
		}
		सचे(db_message_निवेशय_outgoing(leaf->context, 0, mid, सन्देशः_qos, client_retain, stored, leaf->identifier, true, true) == 1){
			पटिनिवत्त 1;
		}
	}अञ्ञथा{
		पटिनिवत्त 1; /* Application error */
	}
	पटिनिवत्त 0;
}


स्थिर पूर्णाङ्क subs__shared_process(संरचना mosquitto__subhier *hier, ध्रुव वर्ण *topic, uint8_t qos, पूर्णाङ्क retain, संरचना mosquitto_base_सन्देशः *stored)
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

		सचे(rc2){
			rc = 1;
		}
	}

	पटिनिवत्त rc;
}


स्थिर पूर्णाङ्क subs__process(संरचना mosquitto__subhier *hier, ध्रुव वर्ण *source_id, ध्रुव वर्ण *topic, uint8_t qos, पूर्णाङ्क retain, संरचना mosquitto_base_सन्देशः *stored)
{
	पूर्णाङ्क rc = 0;
	पूर्णाङ्क rc2;
	संरचना mosquitto__subleaf *leaf;

	rc = subs__shared_process(hier, topic, qos, retain, stored);

	leaf = hier->subs;
	याव(source_id && leaf){
		सचे(!leaf->context->id || (MQTT_SUB_OPT_GET_NO_LOCAL(leaf->subscription_options) && !strcmp(leaf->context->id, source_id))){
			leaf = leaf->next;
			अनुवर्त;
		}
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		सचे(rc2){
			rc = 1;
		}
		leaf = leaf->next;
	}
	सचे(hier->subs || hier->shared){
		पटिनिवत्त rc;
	}अञ्ञथा{
		पटिनिवत्त MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


स्थिर पूर्णाङ्क sub__add_leaf(संरचना mosquitto *context, ध्रुव संरचना mosquitto_subscription *sub, संरचना mosquitto__subleaf **head, संरचना mosquitto__subleaf **newleaf)
{
	संरचना mosquitto__subleaf *leaf;

	*newleaf = रिक्त;
	leaf = *head;

	याव(leaf){
		सचे(leaf->context && leaf->context->id && !strcmp(leaf->context->id, context->id)){
			/* Client making a second subscription to same topic. Only
			 * need to update QoS. Return MOSQ_ERR_SUB_EXISTS to
			 * indicate this to the calling function. */
			leaf->identifier = sub->identifier;
			leaf->subscription_options = sub->options;
			पटिनिवत्त MOSQ_ERR_SUB_EXISTS;
		}
		leaf = leaf->next;
	}
	leaf = mosquitto_calloc(1, परिमाण(संरचना mosquitto__subleaf) + strlen(sub->topic_filter) + 1);
	सचे(!leaf){
		पटिनिवत्त MOSQ_ERR_NOMEM;
	}
	leaf->context = context;
	leaf->identifier = sub->identifier;
	leaf->subscription_options = sub->options;
	strcpy(leaf->topic_filter, sub->topic_filter);

	DL_APPEND(*head, leaf);
	*newleaf = leaf;

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


स्थिर शून्य sub__remove_shared_leaf(संरचना mosquitto__subhier *subhier, संरचना mosquitto__subshared *shared, संरचना mosquitto__subleaf *leaf)
{
	DL_DELETE(shared->subs, leaf);
	सचे(shared->subs == रिक्त){
		HASH_DELETE(hh, subhier->shared, shared);
		mosquitto_FREE(shared);
	}
}


स्थिर पूर्णाङ्क sub__add_shared(संरचना mosquitto *context, ध्रुव संरचना mosquitto_subscription *sub, संरचना mosquitto__subhier *subhier, ध्रुव वर्ण *sharename)
{
	संरचना mosquitto__subleaf *newleaf;
	संरचना mosquitto__subshared *shared = रिक्त;
	संरचना mosquitto__subleaf **subs;
	size_t slen;
	पूर्णाङ्क rc;
	unsigned hashv;

	slen = strlen(sharename);

	HASH_मानम्(sharename, slen, hashv);

	HASH_FIND_BYHASHVALUE(hh, subhier->shared, sharename, slen, hashv, shared);
	सचे(shared == रिक्त){
		shared = mosquitto_calloc(1, परिमाण(संरचना mosquitto__subshared) + slen + 1);
		सचे(!shared){
			पटिनिवत्त MOSQ_ERR_NOMEM;
		}
		strncpy(shared->name, sharename, slen+1);

		HASH_ADD_BYHASHVALUE(hh, subhier->shared, name, slen, hashv, shared);
	}

	rc = sub__add_leaf(context, sub, &shared->subs, &newleaf);
	सचे(rc > 0){
		सचे(shared->subs == रिक्त){
			HASH_DELETE(hh, subhier->shared, shared);
			mosquitto_FREE(shared);
		}
		पटिनिवत्त rc;
	}

	सचे(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = shared;

		bool assigned = false;
		अत्थाय(पूर्णाङ्क i=0; i<context->subs_capacity; i++){
			सचे(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_गणनम्++;
				assigned = true;
				भङ्ग;
			}
		}
		सचे(assigned == false){
			subs = mosquitto_realloc(context->subs, परिमाण(संरचना mosquitto__subleaf *)*(size_t)(context->subs_capacity + 1));
			सचे(!subs){
				sub__remove_shared_leaf(subhier, shared, newleaf);
				mosquitto_FREE(newleaf);
				पटिनिवत्त MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_capacity++;
			context->subs_गणनम्++;
			context->subs[context->subs_capacity-1] = newleaf;
		}
#ifdef WITH_SYS_वृक्षः
		db.shared_subscription_गणनम्++;
#endif
	}

	सचे(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		पटिनिवत्त rc;
	}अञ्ञथा{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		पटिनिवत्त MOSQ_ERR_SUCCESS;
	}
}


स्थिर पूर्णाङ्क sub__add_normal(संरचना mosquitto *context, ध्रुव संरचना mosquitto_subscription *sub, संरचना mosquitto__subhier *subhier)
{
	संरचना mosquitto__subleaf *newleaf = रिक्त;
	संरचना mosquitto__subleaf **subs;
	पूर्णाङ्क rc;

	rc = sub__add_leaf(context, sub, &subhier->subs, &newleaf);
	सचे(rc > 0){
		पटिनिवत्त rc;
	}

	सचे(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = रिक्त;

		bool assigned = false;
		अत्थाय(पूर्णाङ्क i=0; i<context->subs_capacity; i++){
			सचे(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_गणनम्++;
				assigned = true;
				भङ्ग;
			}
		}
		सचे(assigned == false){
			subs = mosquitto_realloc(context->subs, परिमाण(संरचना mosquitto__subleaf *)*(size_t)(context->subs_capacity + 1));
			सचे(!subs){
				DL_DELETE(subhier->subs, newleaf);
				mosquitto_FREE(newleaf);
				पटिनिवत्त MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_capacity++;
			context->subs_गणनम्++;
			context->subs[context->subs_capacity-1] = newleaf;
		}
#ifdef WITH_SYS_वृक्षः
		db.subscription_गणनम्++;
#endif
	}

	सचे(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		पटिनिवत्त rc;
	}अञ्ञथा{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		पटिनिवत्त MOSQ_ERR_SUCCESS;
	}
}


स्थिर पूर्णाङ्क sub__add_context(संरचना mosquitto *context, ध्रुव संरचना mosquitto_subscription *sub, संरचना mosquitto__subhier *subhier, वर्ण *ध्रुव *ध्रुव topics, ध्रुव वर्ण *sharename)
{
	संरचना mosquitto__subhier *branch;
	पूर्णाङ्क topic_index = 0;
	size_t topiclen;

	/* Find leaf node */
	याव(topics && topics[topic_index] != रिक्त){
		topiclen = strlen(topics[topic_index]);
		सचे(topiclen > UINT16_MAX){
			पटिनिवत्त MOSQ_ERR_INVAL;
		}
		HASH_FIND(hh, subhier->children, topics[topic_index], topiclen, branch);
		सचे(!branch){
			/* Not found */
			branch = sub__add_hier_entry(subhier, &subhier->children, topics[topic_index], (uint16_t)topiclen);
			सचे(!branch){
				पटिनिवत्त MOSQ_ERR_NOMEM;
			}
		}
		subhier = branch;
		topic_index++;
	}

	/* Add add our context */
	सचे(context && context->id){
		सचे(sharename){
			पटिनिवत्त sub__add_shared(context, sub, subhier, sharename);
		}अञ्ञथा{
			पटिनिवत्त sub__add_normal(context, sub, subhier);
		}
	}अञ्ञथा{
		पटिनिवत्त MOSQ_ERR_SUCCESS;
	}
}


स्थिर पूर्णाङ्क sub__remove_normal(संरचना mosquitto *context, संरचना mosquitto__subhier *subhier, uint8_t *reason)
{
	संरचना mosquitto__subleaf *leaf;

	leaf = subhier->subs;
	याव(leaf){
		सचे(leaf->context==context){
#ifdef WITH_SYS_वृक्षः
			db.subscription_गणनम्--;
#endif
			DL_DELETE(subhier->subs, leaf);

			/* Remove the reference to the sub that the client is keeping.
			 * It would be nice to be able to use the reference directly,
			 * but that would involve keeping a copy of the topic string in
			 * each subleaf. Might be worth considering though. */
			अत्थाय(पूर्णाङ्क i=0; i<context->subs_capacity; i++){
				सचे(context->subs[i] && context->subs[i]->hier == subhier){
					context->subs_गणनम्--;
					mosquitto_free(context->subs[i]);
					context->subs[i] = रिक्त;
					भङ्ग;
				}
			}
			*reason = 0;
			पटिनिवत्त MOSQ_ERR_SUCCESS;
		}
		leaf = leaf->next;
	}
	पटिनिवत्त MOSQ_ERR_NO_SUBSCRIBERS;
}


स्थिर पूर्णाङ्क sub__remove_shared(संरचना mosquitto *context, संरचना mosquitto__subhier *subhier, uint8_t *reason, ध्रुव वर्ण *sharename)
{
	संरचना mosquitto__subshared *shared;

	HASH_FIND(hh, subhier->shared, sharename, strlen(sharename), shared);
	सचे(shared){
		संरचना mosquitto__subleaf *leaf = shared->subs;
		याव(leaf){
			सचे(leaf->context==context){
#ifdef WITH_SYS_वृक्षः
				db.shared_subscription_गणनम्--;
#endif
				DL_DELETE(shared->subs, leaf);

				/* Remove the reference to the sub that the client is keeping.
				* It would be nice to be able to use the reference directly,
				* but that would involve keeping a copy of the topic string in
				* each subleaf. Might be worth considering though. */
				अत्थाय(पूर्णाङ्क i=0; i<context->subs_capacity; i++){
					सचे(context->subs[i]
							&& context->subs[i]->hier == subhier
							&& context->subs[i]->shared == shared){

						mosquitto_free(context->subs[i]);
						context->subs[i] = रिक्त;
						context->subs_गणनम्--;
						भङ्ग;
					}
				}

				सचे(shared->subs == रिक्त){
					HASH_DELETE(hh, subhier->shared, shared);
					mosquitto_FREE(shared);
				}

				*reason = 0;
				पटिनिवत्त MOSQ_ERR_SUCCESS;
			}
			leaf = leaf->next;
		}
		पटिनिवत्त MOSQ_ERR_NO_SUBSCRIBERS;
	}अञ्ञथा{
		पटिनिवत्त MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


स्थिर पूर्णाङ्क sub__remove_recurse(संरचना mosquitto *context, संरचना mosquitto__subhier *subhier, वर्ण **topics, uint8_t *reason, ध्रुव वर्ण *sharename)
{
	संरचना mosquitto__subhier *branch;

	सचे(topics == रिक्त || topics[0] == रिक्त){
		सचे(sharename){
			पटिनिवत्त sub__remove_shared(context, subhier, reason, sharename);
		}अञ्ञथा{
			पटिनिवत्त sub__remove_normal(context, subhier, reason);
		}
	}

	HASH_FIND(hh, subhier->children, topics[0], strlen(topics[0]), branch);
	सचे(branch){
		sub__remove_recurse(context, branch, &(topics[1]), reason, sharename);
		सचे(!branch->children && !branch->subs && !branch->shared){
			HASH_DELETE(hh, subhier->children, branch);
			mosquitto_FREE(branch);
		}
	}
	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क sub_अन्वेषणम्(संरचना mosquitto__subhier *subhier, वर्ण **split_topics, ध्रुव वर्ण *source_id, ध्रुव वर्ण *topic, uint8_t qos, पूर्णाङ्क retain, संरचना mosquitto_base_सन्देशः *stored)
{
	/* FIXME - need to take into account source_id if the client is a bridge */
	संरचना mosquitto__subhier *branch;
	पूर्णाङ्क rc;
	bool have_subscribers = false;

	सचे(split_topics && split_topics[0]){
		/* Check for literal match */
		HASH_FIND(hh, subhier->children, split_topics[0], strlen(split_topics[0]), branch);

		सचे(branch){
			rc = sub_अन्वेषणम्(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			सचे(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}अञ्ञथा सचे(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				पटिनिवत्त rc;
			}
			सचे(split_topics[1] == रिक्त){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				सचे(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}अञ्ञथा सचे(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					पटिनिवत्त rc;
				}
			}
		}

		/* Check for + match */
		HASH_FIND_BYHASHVALUE(hh, subhier->children, "+", 1, hashv_plus, branch);

		सचे(branch){
			rc = sub_अन्वेषणम्(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			सचे(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}अञ्ञथा सचे(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				पटिनिवत्त rc;
			}
			सचे(split_topics[1] == रिक्त){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				सचे(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}अञ्ञथा सचे(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					पटिनिवत्त rc;
				}
			}
		}
	}

	/* Check for # match */
	HASH_FIND_BYHASHVALUE(hh, subhier->children, "#", 1, hashv_hash, branch);
	सचे(branch && !branch->children){
		/* The topic matches due to a # wildcard - process the
		 * subscriptions but *don't* return. Although this branch has ended
		 * there may still be other subscriptions to deal with.
		 */
		rc = subs__process(branch, source_id, topic, qos, retain, stored);
		सचे(rc == MOSQ_ERR_SUCCESS){
			have_subscribers = true;
		}अञ्ञथा सचे(rc != MOSQ_ERR_NO_SUBSCRIBERS){
			पटिनिवत्त rc;
		}
	}

	सचे(have_subscribers){
		पटिनिवत्त MOSQ_ERR_SUCCESS;
	}अञ्ञथा{
		पटिनिवत्त MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


स्थिर संरचना mosquitto__subhier *sub__add_hier_entry(संरचना mosquitto__subhier *parent, संरचना mosquitto__subhier **sibling, ध्रुव वर्ण *topic, uint16_t len)
{
	संरचना mosquitto__subhier *child;

	प्रतिज्ञा(sibling);

	child = mosquitto_calloc(1, परिमाण(संरचना mosquitto__subhier) + len + 1);
	सचे(!child){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		पटिनिवत्त रिक्त;
	}
	child->parent = parent;
	child->topic_len = len;
	सचे(len > 0){
		strncpy(child->topic, topic, (size_t)(len+1));
	}

	HASH_ADD(hh, *sibling, topic, child->topic_len, child);

	पटिनिवत्त child;
}


पूर्णाङ्क sub__add(संरचना mosquitto *context, ध्रुव संरचना mosquitto_subscription *sub)
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
	सचे(rc){
		पटिनिवत्त rc;
	}

	topiclen = strlen(topics[0]);
	सचे(topiclen > UINT16_MAX){
		mosquitto_FREE(local_sub);
		mosquitto_FREE(topics);
		पटिनिवत्त MOSQ_ERR_INVAL;
	}

	सचे(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], topiclen, subhier);
		सचे(!subhier){
			subhier = sub__add_hier_entry(रिक्त, &db.shared_subs, topics[0], (uint16_t)topiclen);
			सचे(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
				पटिनिवत्त MOSQ_ERR_NOMEM;
			}
		}
	}अञ्ञथा{
		HASH_FIND(hh, db.normal_subs, topics[0], topiclen, subhier);
		सचे(!subhier){
			subhier = sub__add_hier_entry(रिक्त, &db.normal_subs, topics[0], (uint16_t)topiclen);
			सचे(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
				पटिनिवत्त MOSQ_ERR_NOMEM;
			}
		}
	}
	rc = sub__add_context(context, sub, subhier, topics, sharename);

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	पटिनिवत्त rc;
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
	सचे(rc){
		पटिनिवत्त rc;
	}

	सचे(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], strlen(topics[0]), subhier);
	}अञ्ञथा{
		HASH_FIND(hh, db.normal_subs, topics[0], strlen(topics[0]), subhier);
	}
	सचे(subhier){
		*reason = MQTT_RC_NO_SUBSCRIPTION_EXISTED;
		rc = sub__remove_recurse(context, subhier, topics, reason, sharename);
	}

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	पटिनिवत्त rc;
}


पूर्णाङ्क sub__messages_queue(ध्रुव वर्ण *source_id, uint8_t qos, संरचना mosquitto_base_सन्देशः **stored)
{
	पूर्णाङ्क rc = MOSQ_ERR_SUCCESS, rc2;
	पूर्णाङ्क rc_normal = MOSQ_ERR_NO_SUBSCRIBERS, rc_shared = MOSQ_ERR_NO_SUBSCRIBERS;
	संरचना mosquitto__subhier *subhier;
	वर्ण **split_topics = रिक्त;
	वर्ण *local_topic = रिक्त;
	unsigned hashv;
	size_t topiclen;
	bool retain = (*stored)->data.retain;
	ध्रुव वर्ण *topic = (*stored)->data.topic;

	प्रतिज्ञा(topic);

	सचे(sub__topic_tokenise(topic, &local_topic, &split_topics, रिक्त)){
		पटिनिवत्त 1;
	}

	/* Protect this message until we have sent it to all
	clients - this is required because websockets client calls
	db__message_write(), which could remove the message if ref_count==0.
	*/
	db_सन्देशः_store_ref_inc(*stored);

	topiclen = strlen(split_topics[0]);
	HASH_मानम्(split_topics[0], topiclen, hashv);
	HASH_FIND_BYHASHVALUE(hh, db.normal_subs, split_topics[0], topiclen, hashv, subhier);
	सचे(subhier){
		rc_normal = sub_अन्वेषणम्(subhier, split_topics, source_id, topic, qos, retain, *stored);
		सचे(rc_normal > 0){
			rc = rc_normal;
			गच्छ end;
		}
	}

	HASH_FIND_BYHASHVALUE(hh, db.shared_subs, split_topics[0], topiclen, hashv, subhier);
	सचे(subhier){
		rc_shared = sub_अन्वेषणम्(subhier, split_topics, source_id, topic, qos, retain, *stored);
		सचे(rc_shared > 0){
			rc = rc_shared;
			गच्छ end;
		}
	}

	सचे(rc_normal == MOSQ_ERR_NO_SUBSCRIBERS && rc_shared == MOSQ_ERR_NO_SUBSCRIBERS){
		rc = MOSQ_ERR_NO_SUBSCRIBERS;
	}

	सचे(retain){
		rc2 = retain__store(topic, *stored, split_topics, true);
		सचे(rc2){
			rc = rc2;
		}
	}

end:
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);
	/* Remove our reference and free if needed. */
	db_सन्देशः_store_ref_dec(stored);

	पटिनिवत्त rc;
}


/* Remove a subhier element, and return its parent if that needs freeing as well. */
स्थिर संरचना mosquitto__subhier *tmp_remove_subs(संरचना mosquitto__subhier *sub)
{
	संरचना mosquitto__subhier *parent;

	सचे(!sub || !sub->parent){
		पटिनिवत्त रिक्त;
	}

	सचे(sub->children || sub->subs){
		पटिनिवत्त रिक्त;
	}

	parent = sub->parent;
	HASH_DELETE(hh, parent->children, sub);
	mosquitto_FREE(sub);

	सचे(parent->subs == रिक्त
			&& parent->children == रिक्त
			&& parent->shared == रिक्त
			&& parent->parent){

		पटिनिवत्त parent;
	}अञ्ञथा{
		पटिनिवत्त रिक्त;
	}
}


/* Remove all subscriptions for a client.
 */
पूर्णाङ्क sub__clean_session(संरचना mosquitto *context)
{
	अत्थाय(पूर्णाङ्क i=0; i<context->subs_capacity; i++){
		सचे(context->subs[i] == रिक्त || context->subs[i]->hier == रिक्त){
			अनुवर्त;
		}

		संरचना mosquitto__subhier *hier = context->subs[i]->hier;
		संरचना mosquitto__subleaf *leaf;

		plugin_persist__handle_subscription_delete(context, context->subs[i]->topic_filter);
		सचे(context->subs[i]->shared){
			leaf = context->subs[i]->shared->subs;
			याव(leaf){
				सचे(leaf->context==context){
#ifdef WITH_SYS_वृक्षः
					db.shared_subscription_गणनम्--;
#endif
					sub__remove_shared_leaf(context->subs[i]->hier, context->subs[i]->shared, leaf);
					भङ्ग;
				}
				leaf = leaf->next;
			}
		}अञ्ञथा{
			leaf = hier->subs;
			याव(leaf){
				सचे(leaf->context==context){
#ifdef WITH_SYS_वृक्षः
					db.subscription_गणनम्--;
#endif
					DL_DELETE(hier->subs, leaf);
					भङ्ग;
				}
				leaf = leaf->next;
			}
		}
		mosquitto_FREE(context->subs[i]);

		सचे(hier->subs == रिक्त
				&& hier->children == रिक्त
				&& hier->shared == रिक्त
				&& hier->parent){

			करोहि{
				hier = tmp_remove_subs(hier);
			}याव(hier);
		}
	}
	mosquitto_FREE(context->subs);
	context->subs_capacity = 0;
	context->subs_गणनम् = 0;

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


शून्य sub_वृक्षः_print(संरचना mosquitto__subhier *मूलम्, पूर्णाङ्क level)
{
	पूर्णाङ्क i;
	संरचना mosquitto__subhier *branch, *branch_tmp;
	संरचना mosquitto__subleaf *leaf;

	HASH_ITER(hh, मूलम्, branch, branch_tmp){
		सचे(level > -1){
			अत्थाय(i=0; i<(level+2)*2; i++){
				printf(" ");
			}
			printf("%s", branch->topic);
			leaf = branch->subs;
			याव(leaf){
				सचे(leaf->context){
					printf(" (%s, %d)", leaf->context->id, MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}अञ्ञथा{
					printf(" (%s, %d)", "", MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}
				leaf = leaf->next;
			}
			printf("\n");
		}

		sub_वृक्षः_print(branch->children, level+1);
	}
}


पूर्णाङ्क sub__init(शून्य)
{
	HASH_मानम्("+", 1, hashv_plus);
	HASH_मानम्("#", 1, hashv_hash);

	सचे(sub__add_hier_entry(रिक्त, &db.shared_subs, "", 0) == रिक्त
			|| sub__add_hier_entry(रिक्त, &db.normal_subs, "", 0) == रिक्त
			|| sub__add_hier_entry(रिक्त, &db.normal_subs, "$SYS", (uint16_t)strlen("$SYS")) == रिक्त
			){

		पटिनिवत्त MOSQ_ERR_NOMEM;
	}अञ्ञथा{
		पटिनिवत्त MOSQ_ERR_SUCCESS;
	}
}
