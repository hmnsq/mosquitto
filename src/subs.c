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

#ضمن "config.h"

#ضمن <assert.h>
#ضمن <stdio.h>
#ضمن <string.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "util_mosq.h"

#ضمن "utlist.h"

ساكن بنية mosquitto__subhier *sub__add_hier_entry(بنية mosquitto__subhier *parent, بنية mosquitto__subhier **sibling, ثابت حرف *topic, uint16_t len);

ساكن unsigned صحيح hashv_plus = 0;
ساكن unsigned صحيح hashv_hash = 0;


ساكن صحيح subs__send(بنية mosquitto__subleaf *leaf, ثابت حرف *topic, uint8_t qos, صحيح retain, بنية mosquitto__base_msg *stored)
{
	bool client_retain;
	uint16_t mid;
	uint8_t client_qos, msg_qos;
	صحيح rc2;

	/* Check for ACL topic access. */
	rc2 = mosquitto_acl_check(leaf->context, topic, stored->data.payloadlen, stored->data.payload, stored->data.qos, stored->data.retain, stored->data.properties, MOSQ_ACL_READ);
	𐤀𐤌(rc2 == MOSQ_ERR_ACL_DENIED){
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا 𐤀𐤌(rc2 == MOSQ_ERR_SUCCESS){
		client_qos = MQTT_SUB_OPT_GET_QOS(leaf->subscription_options);

		𐤀𐤌(db.config->upgrade_outgoing_qos){
			msg_qos = client_qos;
		}وإلا{
			𐤀𐤌(qos > client_qos){
				msg_qos = client_qos;
			}وإلا{
				msg_qos = qos;
			}
		}
		𐤀𐤌(msg_qos){
			mid = mosquitto__mid_generate(leaf->context);
		}وإلا{
			mid = 0;
		}
		𐤀𐤌(MQTT_SUB_OPT_GET_RETAIN_AS_PUBLISHED(leaf->subscription_options)){
			client_retain = retain;
		}وإلا{
			client_retain = false;
		}
		𐤀𐤌(db__message_insert_outgoing(leaf->context, 0, mid, msg_qos, client_retain, stored, leaf->identifier, true, true) == 1){
			أرجع 1;
		}
	}وإلا{
		أرجع 1; /* Application error */
	}
	أرجع 0;
}


ساكن صحيح subs__shared_process(بنية mosquitto__subhier *hier, ثابت حرف *topic, uint8_t qos, صحيح retain, بنية mosquitto__base_msg *stored)
{
	صحيح rc = 0, rc2;
	بنية mosquitto__subshared *shared, *shared_tmp;
	بنية mosquitto__subleaf *leaf;

	HASH_ITER(hh, hier->shared, shared, shared_tmp){
		leaf = shared->subs;
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		/* Remove current from the top, add back to the bottom */
		DL_DELETE(shared->subs, leaf);
		DL_APPEND(shared->subs, leaf);

		𐤀𐤌(rc2){
			rc = 1;
		}
	}

	أرجع rc;
}


ساكن صحيح subs__process(بنية mosquitto__subhier *hier, ثابت حرف *source_id, ثابت حرف *topic, uint8_t qos, صحيح retain, بنية mosquitto__base_msg *stored)
{
	صحيح rc = 0;
	صحيح rc2;
	بنية mosquitto__subleaf *leaf;

	rc = subs__shared_process(hier, topic, qos, retain, stored);

	leaf = hier->subs;
	طالما(source_id && leaf){
		𐤀𐤌(!leaf->context->id || (MQTT_SUB_OPT_GET_NO_LOCAL(leaf->subscription_options) && !strcmp(leaf->context->id, source_id))){
			leaf = leaf->next;
			استمر;
		}
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		𐤀𐤌(rc2){
			rc = 1;
		}
		leaf = leaf->next;
	}
	𐤀𐤌(hier->subs || hier->shared){
		أرجع rc;
	}وإلا{
		أرجع MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


ساكن صحيح sub__add_leaf(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub, بنية mosquitto__subleaf **head, بنية mosquitto__subleaf **newleaf)
{
	بنية mosquitto__subleaf *leaf;

	*newleaf = عدم;
	leaf = *head;

	طالما(leaf){
		𐤀𐤌(leaf->context && leaf->context->id && !strcmp(leaf->context->id, context->id)){
			/* Client making a second subscription to same topic. Only
			 * need to update QoS. Return MOSQ_ERR_SUB_EXISTS to
			 * indicate this to the calling function. */
			leaf->identifier = sub->identifier;
			leaf->subscription_options = sub->options;
			أرجع MOSQ_ERR_SUB_EXISTS;
		}
		leaf = leaf->next;
	}
	leaf = mosquitto_calloc(1, حجم(بنية mosquitto__subleaf) + strlen(sub->topic_filter) + 1);
	𐤀𐤌(!leaf){
		أرجع MOSQ_ERR_NOMEM;
	}
	leaf->context = context;
	leaf->identifier = sub->identifier;
	leaf->subscription_options = sub->options;
	strcpy(leaf->topic_filter, sub->topic_filter);

	DL_APPEND(*head, leaf);
	*newleaf = leaf;

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن فراغ sub__remove_shared_leaf(بنية mosquitto__subhier *subhier, بنية mosquitto__subshared *shared, بنية mosquitto__subleaf *leaf)
{
	DL_DELETE(shared->subs, leaf);
	𐤀𐤌(shared->subs == عدم){
		HASH_DELETE(hh, subhier->shared, shared);
		mosquitto_FREE(shared);
	}
}


ساكن صحيح sub__add_shared(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub, بنية mosquitto__subhier *subhier, ثابت حرف *sharename)
{
	بنية mosquitto__subleaf *newleaf;
	بنية mosquitto__subshared *shared = عدم;
	بنية mosquitto__subleaf **subs;
	size_t slen;
	صحيح rc;
	unsigned hashv;

	slen = strlen(sharename);

	HASH_VALUE(sharename, slen, hashv);

	HASH_FIND_BYHASHVALUE(hh, subhier->shared, sharename, slen, hashv, shared);
	𐤀𐤌(shared == عدم){
		shared = mosquitto_calloc(1, حجم(بنية mosquitto__subshared) + slen + 1);
		𐤀𐤌(!shared){
			أرجع MOSQ_ERR_NOMEM;
		}
		strncpy(shared->name, sharename, slen+1);

		HASH_ADD_BYHASHVALUE(hh, subhier->shared, name, slen, hashv, shared);
	}

	rc = sub__add_leaf(context, sub, &shared->subs, &newleaf);
	𐤀𐤌(rc > 0){
		𐤀𐤌(shared->subs == عدم){
			HASH_DELETE(hh, subhier->shared, shared);
			mosquitto_FREE(shared);
		}
		أرجع rc;
	}

	𐤀𐤌(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = shared;

		bool assigned = false;
		لكل(صحيح i=0; i<context->subs_capacity; i++){
			𐤀𐤌(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_count++;
				assigned = true;
				اكسر;
			}
		}
		𐤀𐤌(assigned == false){
			subs = mosquitto_realloc(context->subs, حجم(بنية mosquitto__subleaf *)*(size_t)(context->subs_capacity + 1));
			𐤀𐤌(!subs){
				sub__remove_shared_leaf(subhier, shared, newleaf);
				mosquitto_FREE(newleaf);
				أرجع MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_capacity++;
			context->subs_count++;
			context->subs[context->subs_capacity-1] = newleaf;
		}
#ifdef WITH_SYS_TREE
		db.shared_subscription_count++;
#endif
	}

	𐤀𐤌(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		أرجع rc;
	}وإلا{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		أرجع MOSQ_ERR_SUCCESS;
	}
}


ساكن صحيح sub__add_normal(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub, بنية mosquitto__subhier *subhier)
{
	بنية mosquitto__subleaf *newleaf = عدم;
	بنية mosquitto__subleaf **subs;
	صحيح rc;

	rc = sub__add_leaf(context, sub, &subhier->subs, &newleaf);
	𐤀𐤌(rc > 0){
		أرجع rc;
	}

	𐤀𐤌(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = عدم;

		bool assigned = false;
		لكل(صحيح i=0; i<context->subs_capacity; i++){
			𐤀𐤌(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_count++;
				assigned = true;
				اكسر;
			}
		}
		𐤀𐤌(assigned == false){
			subs = mosquitto_realloc(context->subs, حجم(بنية mosquitto__subleaf *)*(size_t)(context->subs_capacity + 1));
			𐤀𐤌(!subs){
				DL_DELETE(subhier->subs, newleaf);
				mosquitto_FREE(newleaf);
				أرجع MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_capacity++;
			context->subs_count++;
			context->subs[context->subs_capacity-1] = newleaf;
		}
#ifdef WITH_SYS_TREE
		db.subscription_count++;
#endif
	}

	𐤀𐤌(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		أرجع rc;
	}وإلا{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		أرجع MOSQ_ERR_SUCCESS;
	}
}


ساكن صحيح sub__add_context(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub, بنية mosquitto__subhier *subhier, حرف *ثابت *ثابت topics, ثابت حرف *sharename)
{
	بنية mosquitto__subhier *branch;
	صحيح topic_index = 0;
	size_t topiclen;

	/* Find leaf node */
	طالما(topics && topics[topic_index] != عدم){
		topiclen = strlen(topics[topic_index]);
		𐤀𐤌(topiclen > UINT16_MAX){
			أرجع MOSQ_ERR_INVAL;
		}
		HASH_FIND(hh, subhier->children, topics[topic_index], topiclen, branch);
		𐤀𐤌(!branch){
			/* Not found */
			branch = sub__add_hier_entry(subhier, &subhier->children, topics[topic_index], (uint16_t)topiclen);
			𐤀𐤌(!branch){
				أرجع MOSQ_ERR_NOMEM;
			}
		}
		subhier = branch;
		topic_index++;
	}

	/* Add add our context */
	𐤀𐤌(context && context->id){
		𐤀𐤌(sharename){
			أرجع sub__add_shared(context, sub, subhier, sharename);
		}وإلا{
			أرجع sub__add_normal(context, sub, subhier);
		}
	}وإلا{
		أرجع MOSQ_ERR_SUCCESS;
	}
}


ساكن صحيح sub__remove_normal(بنية mosquitto *context, بنية mosquitto__subhier *subhier, uint8_t *reason)
{
	بنية mosquitto__subleaf *leaf;

	leaf = subhier->subs;
	طالما(leaf){
		𐤀𐤌(leaf->context==context){
#ifdef WITH_SYS_TREE
			db.subscription_count--;
#endif
			DL_DELETE(subhier->subs, leaf);

			/* Remove the reference to the sub that the client is keeping.
			 * It would be nice to be able to use the reference directly,
			 * but that would involve keeping a copy of the topic string in
			 * each subleaf. Might be worth considering though. */
			لكل(صحيح i=0; i<context->subs_capacity; i++){
				𐤀𐤌(context->subs[i] && context->subs[i]->hier == subhier){
					context->subs_count--;
					mosquitto_free(context->subs[i]);
					context->subs[i] = عدم;
					اكسر;
				}
			}
			*reason = 0;
			أرجع MOSQ_ERR_SUCCESS;
		}
		leaf = leaf->next;
	}
	أرجع MOSQ_ERR_NO_SUBSCRIBERS;
}


ساكن صحيح sub__remove_shared(بنية mosquitto *context, بنية mosquitto__subhier *subhier, uint8_t *reason, ثابت حرف *sharename)
{
	بنية mosquitto__subshared *shared;

	HASH_FIND(hh, subhier->shared, sharename, strlen(sharename), shared);
	𐤀𐤌(shared){
		بنية mosquitto__subleaf *leaf = shared->subs;
		طالما(leaf){
			𐤀𐤌(leaf->context==context){
#ifdef WITH_SYS_TREE
				db.shared_subscription_count--;
#endif
				DL_DELETE(shared->subs, leaf);

				/* Remove the reference to the sub that the client is keeping.
				* It would be nice to be able to use the reference directly,
				* but that would involve keeping a copy of the topic string in
				* each subleaf. Might be worth considering though. */
				لكل(صحيح i=0; i<context->subs_capacity; i++){
					𐤀𐤌(context->subs[i]
							&& context->subs[i]->hier == subhier
							&& context->subs[i]->shared == shared){

						mosquitto_free(context->subs[i]);
						context->subs[i] = عدم;
						context->subs_count--;
						اكسر;
					}
				}

				𐤀𐤌(shared->subs == عدم){
					HASH_DELETE(hh, subhier->shared, shared);
					mosquitto_FREE(shared);
				}

				*reason = 0;
				أرجع MOSQ_ERR_SUCCESS;
			}
			leaf = leaf->next;
		}
		أرجع MOSQ_ERR_NO_SUBSCRIBERS;
	}وإلا{
		أرجع MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


ساكن صحيح sub__remove_recurse(بنية mosquitto *context, بنية mosquitto__subhier *subhier, حرف **topics, uint8_t *reason, ثابت حرف *sharename)
{
	بنية mosquitto__subhier *branch;

	𐤀𐤌(topics == عدم || topics[0] == عدم){
		𐤀𐤌(sharename){
			أرجع sub__remove_shared(context, subhier, reason, sharename);
		}وإلا{
			أرجع sub__remove_normal(context, subhier, reason);
		}
	}

	HASH_FIND(hh, subhier->children, topics[0], strlen(topics[0]), branch);
	𐤀𐤌(branch){
		sub__remove_recurse(context, branch, &(topics[1]), reason, sharename);
		𐤀𐤌(!branch->children && !branch->subs && !branch->shared){
			HASH_DELETE(hh, subhier->children, branch);
			mosquitto_FREE(branch);
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح sub__search(بنية mosquitto__subhier *subhier, حرف **split_topics, ثابت حرف *source_id, ثابت حرف *topic, uint8_t qos, صحيح retain, بنية mosquitto__base_msg *stored)
{
	/* FIXME - need to take into account source_id if the client is a bridge */
	بنية mosquitto__subhier *branch;
	صحيح rc;
	bool have_subscribers = false;

	𐤀𐤌(split_topics && split_topics[0]){
		/* Check for literal match */
		HASH_FIND(hh, subhier->children, split_topics[0], strlen(split_topics[0]), branch);

		𐤀𐤌(branch){
			rc = sub__search(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			𐤀𐤌(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}وإلا 𐤀𐤌(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				أرجع rc;
			}
			𐤀𐤌(split_topics[1] == عدم){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				𐤀𐤌(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}وإلا 𐤀𐤌(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					أرجع rc;
				}
			}
		}

		/* Check for + match */
		HASH_FIND_BYHASHVALUE(hh, subhier->children, "+", 1, hashv_plus, branch);

		𐤀𐤌(branch){
			rc = sub__search(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			𐤀𐤌(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}وإلا 𐤀𐤌(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				أرجع rc;
			}
			𐤀𐤌(split_topics[1] == عدم){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				𐤀𐤌(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}وإلا 𐤀𐤌(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					أرجع rc;
				}
			}
		}
	}

	/* Check for # match */
	HASH_FIND_BYHASHVALUE(hh, subhier->children, "#", 1, hashv_hash, branch);
	𐤀𐤌(branch && !branch->children){
		/* The topic matches due to a # wildcard - process the
		 * subscriptions but *don't* return. Although this branch has ended
		 * there may still be other subscriptions to deal with.
		 */
		rc = subs__process(branch, source_id, topic, qos, retain, stored);
		𐤀𐤌(rc == MOSQ_ERR_SUCCESS){
			have_subscribers = true;
		}وإلا 𐤀𐤌(rc != MOSQ_ERR_NO_SUBSCRIBERS){
			أرجع rc;
		}
	}

	𐤀𐤌(have_subscribers){
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا{
		أرجع MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


ساكن بنية mosquitto__subhier *sub__add_hier_entry(بنية mosquitto__subhier *parent, بنية mosquitto__subhier **sibling, ثابت حرف *topic, uint16_t len)
{
	بنية mosquitto__subhier *child;

	أكد(sibling);

	child = mosquitto_calloc(1, حجم(بنية mosquitto__subhier) + len + 1);
	𐤀𐤌(!child){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		أرجع عدم;
	}
	child->parent = parent;
	child->topic_len = len;
	𐤀𐤌(len > 0){
		strncpy(child->topic, topic, (size_t)(len+1));
	}

	HASH_ADD(hh, *sibling, topic, child->topic_len, child);

	أرجع child;
}


صحيح sub__add(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub)
{
	صحيح rc = 0;
	بنية mosquitto__subhier *subhier;
	ثابت حرف *sharename = عدم;
	حرف *local_sub;
	حرف **topics;
	size_t topiclen;

	أكد(sub);
	أكد(sub->topic_filter);

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &topics, &sharename);
	𐤀𐤌(rc){
		أرجع rc;
	}

	topiclen = strlen(topics[0]);
	𐤀𐤌(topiclen > UINT16_MAX){
		mosquitto_FREE(local_sub);
		mosquitto_FREE(topics);
		أرجع MOSQ_ERR_INVAL;
	}

	𐤀𐤌(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], topiclen, subhier);
		𐤀𐤌(!subhier){
			subhier = sub__add_hier_entry(عدم, &db.shared_subs, topics[0], (uint16_t)topiclen);
			𐤀𐤌(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
				أرجع MOSQ_ERR_NOMEM;
			}
		}
	}وإلا{
		HASH_FIND(hh, db.normal_subs, topics[0], topiclen, subhier);
		𐤀𐤌(!subhier){
			subhier = sub__add_hier_entry(عدم, &db.normal_subs, topics[0], (uint16_t)topiclen);
			𐤀𐤌(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
				أرجع MOSQ_ERR_NOMEM;
			}
		}
	}
	rc = sub__add_context(context, sub, subhier, topics, sharename);

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	أرجع rc;
}


صحيح sub__remove(بنية mosquitto *context, ثابت حرف *sub, uint8_t *reason)
{
	صحيح rc = 0;
	بنية mosquitto__subhier *subhier;
	ثابت حرف *sharename = عدم;
	حرف *local_sub = عدم;
	حرف **topics = عدم;

	أكد(sub);

	rc = sub__topic_tokenise(sub, &local_sub, &topics, &sharename);
	𐤀𐤌(rc){
		أرجع rc;
	}

	𐤀𐤌(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], strlen(topics[0]), subhier);
	}وإلا{
		HASH_FIND(hh, db.normal_subs, topics[0], strlen(topics[0]), subhier);
	}
	𐤀𐤌(subhier){
		*reason = MQTT_RC_NO_SUBSCRIPTION_EXISTED;
		rc = sub__remove_recurse(context, subhier, topics, reason, sharename);
	}

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	أرجع rc;
}


صحيح sub__messages_queue(ثابت حرف *source_id, uint8_t qos, بنية mosquitto__base_msg **stored)
{
	صحيح rc = MOSQ_ERR_SUCCESS, rc2;
	صحيح rc_normal = MOSQ_ERR_NO_SUBSCRIBERS, rc_shared = MOSQ_ERR_NO_SUBSCRIBERS;
	بنية mosquitto__subhier *subhier;
	حرف **split_topics = عدم;
	حرف *local_topic = عدم;
	unsigned hashv;
	size_t topiclen;
	bool retain = (*stored)->data.retain;
	ثابت حرف *topic = (*stored)->data.topic;

	أكد(topic);

	𐤀𐤌(sub__topic_tokenise(topic, &local_topic, &split_topics, عدم)){
		أرجع 1;
	}

	/* Protect this message until we have sent it to all
	clients - this is required because websockets client calls
	db__message_write(), which could remove the message if ref_count==0.
	*/
	db__msg_store_ref_inc(*stored);

	topiclen = strlen(split_topics[0]);
	HASH_VALUE(split_topics[0], topiclen, hashv);
	HASH_FIND_BYHASHVALUE(hh, db.normal_subs, split_topics[0], topiclen, hashv, subhier);
	𐤀𐤌(subhier){
		rc_normal = sub__search(subhier, split_topics, source_id, topic, qos, retain, *stored);
		𐤀𐤌(rc_normal > 0){
			rc = rc_normal;
			اذهب end;
		}
	}

	HASH_FIND_BYHASHVALUE(hh, db.shared_subs, split_topics[0], topiclen, hashv, subhier);
	𐤀𐤌(subhier){
		rc_shared = sub__search(subhier, split_topics, source_id, topic, qos, retain, *stored);
		𐤀𐤌(rc_shared > 0){
			rc = rc_shared;
			اذهب end;
		}
	}

	𐤀𐤌(rc_normal == MOSQ_ERR_NO_SUBSCRIBERS && rc_shared == MOSQ_ERR_NO_SUBSCRIBERS){
		rc = MOSQ_ERR_NO_SUBSCRIBERS;
	}

	𐤀𐤌(retain){
		rc2 = retain__store(topic, *stored, split_topics, true);
		𐤀𐤌(rc2){
			rc = rc2;
		}
	}

end:
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);
	/* Remove our reference and free if needed. */
	db__msg_store_ref_dec(stored);

	أرجع rc;
}


/* Remove a subhier element, and return its parent if that needs freeing as well. */
ساكن بنية mosquitto__subhier *tmp_remove_subs(بنية mosquitto__subhier *sub)
{
	بنية mosquitto__subhier *parent;

	𐤀𐤌(!sub || !sub->parent){
		أرجع عدم;
	}

	𐤀𐤌(sub->children || sub->subs){
		أرجع عدم;
	}

	parent = sub->parent;
	HASH_DELETE(hh, parent->children, sub);
	mosquitto_FREE(sub);

	𐤀𐤌(parent->subs == عدم
			&& parent->children == عدم
			&& parent->shared == عدم
			&& parent->parent){

		أرجع parent;
	}وإلا{
		أرجع عدم;
	}
}


/* Remove all subscriptions for a client.
 */
صحيح sub__clean_session(بنية mosquitto *context)
{
	لكل(صحيح i=0; i<context->subs_capacity; i++){
		𐤀𐤌(context->subs[i] == عدم || context->subs[i]->hier == عدم){
			استمر;
		}

		بنية mosquitto__subhier *hier = context->subs[i]->hier;
		بنية mosquitto__subleaf *leaf;

		plugin_persist__handle_subscription_delete(context, context->subs[i]->topic_filter);
		𐤀𐤌(context->subs[i]->shared){
			leaf = context->subs[i]->shared->subs;
			طالما(leaf){
				𐤀𐤌(leaf->context==context){
#ifdef WITH_SYS_TREE
					db.shared_subscription_count--;
#endif
					sub__remove_shared_leaf(context->subs[i]->hier, context->subs[i]->shared, leaf);
					اكسر;
				}
				leaf = leaf->next;
			}
		}وإلا{
			leaf = hier->subs;
			طالما(leaf){
				𐤀𐤌(leaf->context==context){
#ifdef WITH_SYS_TREE
					db.subscription_count--;
#endif
					DL_DELETE(hier->subs, leaf);
					اكسر;
				}
				leaf = leaf->next;
			}
		}
		mosquitto_FREE(context->subs[i]);

		𐤀𐤌(hier->subs == عدم
				&& hier->children == عدم
				&& hier->shared == عدم
				&& hier->parent){

			𐤐𐤏𐤋{
				hier = tmp_remove_subs(hier);
			}طالما(hier);
		}
	}
	mosquitto_FREE(context->subs);
	context->subs_capacity = 0;
	context->subs_count = 0;

	أرجع MOSQ_ERR_SUCCESS;
}


فراغ sub__tree_print(بنية mosquitto__subhier *root, صحيح level)
{
	صحيح i;
	بنية mosquitto__subhier *branch, *branch_tmp;
	بنية mosquitto__subleaf *leaf;

	HASH_ITER(hh, root, branch, branch_tmp){
		𐤀𐤌(level > -1){
			لكل(i=0; i<(level+2)*2; i++){
				printf(" ");
			}
			printf("%s", branch->topic);
			leaf = branch->subs;
			طالما(leaf){
				𐤀𐤌(leaf->context){
					printf(" (%s, %d)", leaf->context->id, MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}وإلا{
					printf(" (%s, %d)", "", MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}
				leaf = leaf->next;
			}
			printf("\n");
		}

		sub__tree_print(branch->children, level+1);
	}
}


صحيح sub__init(فراغ)
{
	HASH_VALUE("+", 1, hashv_plus);
	HASH_VALUE("#", 1, hashv_hash);

	𐤀𐤌(sub__add_hier_entry(عدم, &db.shared_subs, "", 0) == عدم
			|| sub__add_hier_entry(عدم, &db.normal_subs, "", 0) == عدم
			|| sub__add_hier_entry(عدم, &db.normal_subs, "$SYS", (uint16_t)strlen("$SYS")) == عدم
			){

		أرجع MOSQ_ERR_NOMEM;
	}وإلا{
		أرجع MOSQ_ERR_SUCCESS;
	}
}
