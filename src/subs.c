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

#شامل "config.h"

#شامل <assert.h>
#شامل <stdio.h>
#شامل <string.h>

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "util_mosq.h"

#شامل "utlist.h"

ایستا ساختار mosquitto__subhier *sub__add_hier_entry(ساختار mosquitto__subhier *parent, ساختار mosquitto__subhier **sibling, ثابت نویسه *topic, uint16_t len);

ایستا unsigned صحیح hashv_plus = 0;
ایستا unsigned صحیح hashv_hash = 0;


ایستا صحیح subs__send(ساختار mosquitto__subleaf *leaf, ثابت نویسه *topic, uint8_t qos, صحیح retain, ساختار mosquitto_base_پیام *stored)
{
	bool client_retain;
	uint16_t mid;
	uint8_t client_qos, پیام_qos;
	صحیح rc2;

	/* Check for ACL topic access. */
	rc2 = mosquitto_acl_check(leaf->context, topic, stored->data.payloadlen, stored->data.payload, stored->data.qos, stored->data.retain, stored->data.properties, MOSQ_ACL_READ);
	اگر(rc2 == MOSQ_ERR_ACL_DENIED){
		بازگشت MOSQ_ERR_SUCCESS;
	}وگرنه اگر(rc2 == MOSQ_ERR_SUCCESS){
		client_qos = MQTT_SUB_OPT_GET_QOS(leaf->subscription_options);

		اگر(db.config->upgrade_outgoing_qos){
			پیام_qos = client_qos;
		}وگرنه{
			اگر(qos > client_qos){
				پیام_qos = client_qos;
			}وگرنه{
				پیام_qos = qos;
			}
		}
		اگر(پیام_qos){
			mid = mosquitto__mid_generate(leaf->context);
		}وگرنه{
			mid = 0;
		}
		اگر(MQTT_SUB_OPT_GET_RETAIN_AS_PUBLISHED(leaf->subscription_options)){
			client_retain = retain;
		}وگرنه{
			client_retain = false;
		}
		اگر(db_message_درج_outgoing(leaf->context, 0, mid, پیام_qos, client_retain, stored, leaf->identifier, true, true) == 1){
			بازگشت 1;
		}
	}وگرنه{
		بازگشت 1; /* Application error */
	}
	بازگشت 0;
}


ایستا صحیح subs__shared_process(ساختار mosquitto__subhier *hier, ثابت نویسه *topic, uint8_t qos, صحیح retain, ساختار mosquitto_base_پیام *stored)
{
	صحیح rc = 0, rc2;
	ساختار mosquitto__subshared *shared, *shared_tmp;
	ساختار mosquitto__subleaf *leaf;

	HASH_ITER(hh, hier->shared, shared, shared_tmp){
		leaf = shared->subs;
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		/* Remove current from the top, add back to the bottom */
		DL_DELETE(shared->subs, leaf);
		DL_APPEND(shared->subs, leaf);

		اگر(rc2){
			rc = 1;
		}
	}

	بازگشت rc;
}


ایستا صحیح subs__process(ساختار mosquitto__subhier *hier, ثابت نویسه *source_id, ثابت نویسه *topic, uint8_t qos, صحیح retain, ساختار mosquitto_base_پیام *stored)
{
	صحیح rc = 0;
	صحیح rc2;
	ساختار mosquitto__subleaf *leaf;

	rc = subs__shared_process(hier, topic, qos, retain, stored);

	leaf = hier->subs;
	وقتی(source_id && leaf){
		اگر(!leaf->context->id || (MQTT_SUB_OPT_GET_NO_LOCAL(leaf->subscription_options) && !strcmp(leaf->context->id, source_id))){
			leaf = leaf->next;
			ادامه;
		}
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		اگر(rc2){
			rc = 1;
		}
		leaf = leaf->next;
	}
	اگر(hier->subs || hier->shared){
		بازگشت rc;
	}وگرنه{
		بازگشت MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


ایستا صحیح sub__add_leaf(ساختار mosquitto *context, ثابت ساختار mosquitto_subscription *sub, ساختار mosquitto__subleaf **head, ساختار mosquitto__subleaf **newleaf)
{
	ساختار mosquitto__subleaf *leaf;

	*newleaf = پوچ;
	leaf = *head;

	وقتی(leaf){
		اگر(leaf->context && leaf->context->id && !strcmp(leaf->context->id, context->id)){
			/* Client making a second subscription to same topic. Only
			 * need to update QoS. Return MOSQ_ERR_SUB_EXISTS to
			 * indicate this to the calling function. */
			leaf->identifier = sub->identifier;
			leaf->subscription_options = sub->options;
			بازگشت MOSQ_ERR_SUB_EXISTS;
		}
		leaf = leaf->next;
	}
	leaf = mosquitto_calloc(1, اندازه(ساختار mosquitto__subleaf) + strlen(sub->topic_filter) + 1);
	اگر(!leaf){
		بازگشت MOSQ_ERR_NOMEM;
	}
	leaf->context = context;
	leaf->identifier = sub->identifier;
	leaf->subscription_options = sub->options;
	strcpy(leaf->topic_filter, sub->topic_filter);

	DL_APPEND(*head, leaf);
	*newleaf = leaf;

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا تهی sub__remove_shared_leaf(ساختار mosquitto__subhier *subhier, ساختار mosquitto__subshared *shared, ساختار mosquitto__subleaf *leaf)
{
	DL_DELETE(shared->subs, leaf);
	اگر(shared->subs == پوچ){
		HASH_DELETE(hh, subhier->shared, shared);
		mosquitto_FREE(shared);
	}
}


ایستا صحیح sub__add_shared(ساختار mosquitto *context, ثابت ساختار mosquitto_subscription *sub, ساختار mosquitto__subhier *subhier, ثابت نویسه *sharename)
{
	ساختار mosquitto__subleaf *newleaf;
	ساختار mosquitto__subshared *shared = پوچ;
	ساختار mosquitto__subleaf **subs;
	size_t slen;
	صحیح rc;
	unsigned hashv;

	slen = strlen(sharename);

	HASH_مقدار(sharename, slen, hashv);

	HASH_FIND_BYHASHVALUE(hh, subhier->shared, sharename, slen, hashv, shared);
	اگر(shared == پوچ){
		shared = mosquitto_calloc(1, اندازه(ساختار mosquitto__subshared) + slen + 1);
		اگر(!shared){
			بازگشت MOSQ_ERR_NOMEM;
		}
		strncpy(shared->name, sharename, slen+1);

		HASH_ADD_BYHASHVALUE(hh, subhier->shared, name, slen, hashv, shared);
	}

	rc = sub__add_leaf(context, sub, &shared->subs, &newleaf);
	اگر(rc > 0){
		اگر(shared->subs == پوچ){
			HASH_DELETE(hh, subhier->shared, shared);
			mosquitto_FREE(shared);
		}
		بازگشت rc;
	}

	اگر(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = shared;

		bool assigned = false;
		برای(صحیح i=0; i<context->subs_capacity; i++){
			اگر(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_شمار++;
				assigned = true;
				بشکن;
			}
		}
		اگر(assigned == false){
			subs = mosquitto_realloc(context->subs, اندازه(ساختار mosquitto__subleaf *)*(size_t)(context->subs_capacity + 1));
			اگر(!subs){
				sub__remove_shared_leaf(subhier, shared, newleaf);
				mosquitto_FREE(newleaf);
				بازگشت MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_capacity++;
			context->subs_شمار++;
			context->subs[context->subs_capacity-1] = newleaf;
		}
#ifdef WITH_SYS_درخت
		db.shared_subscription_شمار++;
#endif
	}

	اگر(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		بازگشت rc;
	}وگرنه{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		بازگشت MOSQ_ERR_SUCCESS;
	}
}


ایستا صحیح sub__add_normal(ساختار mosquitto *context, ثابت ساختار mosquitto_subscription *sub, ساختار mosquitto__subhier *subhier)
{
	ساختار mosquitto__subleaf *newleaf = پوچ;
	ساختار mosquitto__subleaf **subs;
	صحیح rc;

	rc = sub__add_leaf(context, sub, &subhier->subs, &newleaf);
	اگر(rc > 0){
		بازگشت rc;
	}

	اگر(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = پوچ;

		bool assigned = false;
		برای(صحیح i=0; i<context->subs_capacity; i++){
			اگر(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_شمار++;
				assigned = true;
				بشکن;
			}
		}
		اگر(assigned == false){
			subs = mosquitto_realloc(context->subs, اندازه(ساختار mosquitto__subleaf *)*(size_t)(context->subs_capacity + 1));
			اگر(!subs){
				DL_DELETE(subhier->subs, newleaf);
				mosquitto_FREE(newleaf);
				بازگشت MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_capacity++;
			context->subs_شمار++;
			context->subs[context->subs_capacity-1] = newleaf;
		}
#ifdef WITH_SYS_درخت
		db.subscription_شمار++;
#endif
	}

	اگر(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		بازگشت rc;
	}وگرنه{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		بازگشت MOSQ_ERR_SUCCESS;
	}
}


ایستا صحیح sub__add_context(ساختار mosquitto *context, ثابت ساختار mosquitto_subscription *sub, ساختار mosquitto__subhier *subhier, نویسه *ثابت *ثابت topics, ثابت نویسه *sharename)
{
	ساختار mosquitto__subhier *branch;
	صحیح topic_index = 0;
	size_t topiclen;

	/* Find leaf node */
	وقتی(topics && topics[topic_index] != پوچ){
		topiclen = strlen(topics[topic_index]);
		اگر(topiclen > UINT16_MAX){
			بازگشت MOSQ_ERR_INVAL;
		}
		HASH_FIND(hh, subhier->children, topics[topic_index], topiclen, branch);
		اگر(!branch){
			/* Not found */
			branch = sub__add_hier_entry(subhier, &subhier->children, topics[topic_index], (uint16_t)topiclen);
			اگر(!branch){
				بازگشت MOSQ_ERR_NOMEM;
			}
		}
		subhier = branch;
		topic_index++;
	}

	/* Add add our context */
	اگر(context && context->id){
		اگر(sharename){
			بازگشت sub__add_shared(context, sub, subhier, sharename);
		}وگرنه{
			بازگشت sub__add_normal(context, sub, subhier);
		}
	}وگرنه{
		بازگشت MOSQ_ERR_SUCCESS;
	}
}


ایستا صحیح sub__remove_normal(ساختار mosquitto *context, ساختار mosquitto__subhier *subhier, uint8_t *reason)
{
	ساختار mosquitto__subleaf *leaf;

	leaf = subhier->subs;
	وقتی(leaf){
		اگر(leaf->context==context){
#ifdef WITH_SYS_درخت
			db.subscription_شمار--;
#endif
			DL_DELETE(subhier->subs, leaf);

			/* Remove the reference to the sub that the client is keeping.
			 * It would be nice to be able to use the reference directly,
			 * but that would involve keeping a copy of the topic string in
			 * each subleaf. Might be worth considering though. */
			برای(صحیح i=0; i<context->subs_capacity; i++){
				اگر(context->subs[i] && context->subs[i]->hier == subhier){
					context->subs_شمار--;
					mosquitto_free(context->subs[i]);
					context->subs[i] = پوچ;
					بشکن;
				}
			}
			*reason = 0;
			بازگشت MOSQ_ERR_SUCCESS;
		}
		leaf = leaf->next;
	}
	بازگشت MOSQ_ERR_NO_SUBSCRIBERS;
}


ایستا صحیح sub__remove_shared(ساختار mosquitto *context, ساختار mosquitto__subhier *subhier, uint8_t *reason, ثابت نویسه *sharename)
{
	ساختار mosquitto__subshared *shared;

	HASH_FIND(hh, subhier->shared, sharename, strlen(sharename), shared);
	اگر(shared){
		ساختار mosquitto__subleaf *leaf = shared->subs;
		وقتی(leaf){
			اگر(leaf->context==context){
#ifdef WITH_SYS_درخت
				db.shared_subscription_شمار--;
#endif
				DL_DELETE(shared->subs, leaf);

				/* Remove the reference to the sub that the client is keeping.
				* It would be nice to be able to use the reference directly,
				* but that would involve keeping a copy of the topic string in
				* each subleaf. Might be worth considering though. */
				برای(صحیح i=0; i<context->subs_capacity; i++){
					اگر(context->subs[i]
							&& context->subs[i]->hier == subhier
							&& context->subs[i]->shared == shared){

						mosquitto_free(context->subs[i]);
						context->subs[i] = پوچ;
						context->subs_شمار--;
						بشکن;
					}
				}

				اگر(shared->subs == پوچ){
					HASH_DELETE(hh, subhier->shared, shared);
					mosquitto_FREE(shared);
				}

				*reason = 0;
				بازگشت MOSQ_ERR_SUCCESS;
			}
			leaf = leaf->next;
		}
		بازگشت MOSQ_ERR_NO_SUBSCRIBERS;
	}وگرنه{
		بازگشت MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


ایستا صحیح sub__remove_recurse(ساختار mosquitto *context, ساختار mosquitto__subhier *subhier, نویسه **topics, uint8_t *reason, ثابت نویسه *sharename)
{
	ساختار mosquitto__subhier *branch;

	اگر(topics == پوچ || topics[0] == پوچ){
		اگر(sharename){
			بازگشت sub__remove_shared(context, subhier, reason, sharename);
		}وگرنه{
			بازگشت sub__remove_normal(context, subhier, reason);
		}
	}

	HASH_FIND(hh, subhier->children, topics[0], strlen(topics[0]), branch);
	اگر(branch){
		sub__remove_recurse(context, branch, &(topics[1]), reason, sharename);
		اگر(!branch->children && !branch->subs && !branch->shared){
			HASH_DELETE(hh, subhier->children, branch);
			mosquitto_FREE(branch);
		}
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح sub_جستجو(ساختار mosquitto__subhier *subhier, نویسه **split_topics, ثابت نویسه *source_id, ثابت نویسه *topic, uint8_t qos, صحیح retain, ساختار mosquitto_base_پیام *stored)
{
	/* FIXME - need to take into account source_id if the client is a bridge */
	ساختار mosquitto__subhier *branch;
	صحیح rc;
	bool have_subscribers = false;

	اگر(split_topics && split_topics[0]){
		/* Check for literal match */
		HASH_FIND(hh, subhier->children, split_topics[0], strlen(split_topics[0]), branch);

		اگر(branch){
			rc = sub_جستجو(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			اگر(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}وگرنه اگر(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				بازگشت rc;
			}
			اگر(split_topics[1] == پوچ){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				اگر(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}وگرنه اگر(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					بازگشت rc;
				}
			}
		}

		/* Check for + match */
		HASH_FIND_BYHASHVALUE(hh, subhier->children, "+", 1, hashv_plus, branch);

		اگر(branch){
			rc = sub_جستجو(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			اگر(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}وگرنه اگر(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				بازگشت rc;
			}
			اگر(split_topics[1] == پوچ){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				اگر(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}وگرنه اگر(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					بازگشت rc;
				}
			}
		}
	}

	/* Check for # match */
	HASH_FIND_BYHASHVALUE(hh, subhier->children, "#", 1, hashv_hash, branch);
	اگر(branch && !branch->children){
		/* The topic matches due to a # wildcard - process the
		 * subscriptions but *don't* return. Although this branch has ended
		 * there may still be other subscriptions to deal with.
		 */
		rc = subs__process(branch, source_id, topic, qos, retain, stored);
		اگر(rc == MOSQ_ERR_SUCCESS){
			have_subscribers = true;
		}وگرنه اگر(rc != MOSQ_ERR_NO_SUBSCRIBERS){
			بازگشت rc;
		}
	}

	اگر(have_subscribers){
		بازگشت MOSQ_ERR_SUCCESS;
	}وگرنه{
		بازگشت MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


ایستا ساختار mosquitto__subhier *sub__add_hier_entry(ساختار mosquitto__subhier *parent, ساختار mosquitto__subhier **sibling, ثابت نویسه *topic, uint16_t len)
{
	ساختار mosquitto__subhier *child;

	أكد(sibling);

	child = mosquitto_calloc(1, اندازه(ساختار mosquitto__subhier) + len + 1);
	اگر(!child){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
		بازگشت پوچ;
	}
	child->parent = parent;
	child->topic_len = len;
	اگر(len > 0){
		strncpy(child->topic, topic, (size_t)(len+1));
	}

	HASH_ADD(hh, *sibling, topic, child->topic_len, child);

	بازگشت child;
}


صحیح sub__add(ساختار mosquitto *context, ثابت ساختار mosquitto_subscription *sub)
{
	صحیح rc = 0;
	ساختار mosquitto__subhier *subhier;
	ثابت نویسه *sharename = پوچ;
	نویسه *local_sub;
	نویسه **topics;
	size_t topiclen;

	أكد(sub);
	أكد(sub->topic_filter);

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &topics, &sharename);
	اگر(rc){
		بازگشت rc;
	}

	topiclen = strlen(topics[0]);
	اگر(topiclen > UINT16_MAX){
		mosquitto_FREE(local_sub);
		mosquitto_FREE(topics);
		بازگشت MOSQ_ERR_INVAL;
	}

	اگر(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], topiclen, subhier);
		اگر(!subhier){
			subhier = sub__add_hier_entry(پوچ, &db.shared_subs, topics[0], (uint16_t)topiclen);
			اگر(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
				بازگشت MOSQ_ERR_NOMEM;
			}
		}
	}وگرنه{
		HASH_FIND(hh, db.normal_subs, topics[0], topiclen, subhier);
		اگر(!subhier){
			subhier = sub__add_hier_entry(پوچ, &db.normal_subs, topics[0], (uint16_t)topiclen);
			اگر(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
				بازگشت MOSQ_ERR_NOMEM;
			}
		}
	}
	rc = sub__add_context(context, sub, subhier, topics, sharename);

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	بازگشت rc;
}


صحیح sub__remove(ساختار mosquitto *context, ثابت نویسه *sub, uint8_t *reason)
{
	صحیح rc = 0;
	ساختار mosquitto__subhier *subhier;
	ثابت نویسه *sharename = پوچ;
	نویسه *local_sub = پوچ;
	نویسه **topics = پوچ;

	أكد(sub);

	rc = sub__topic_tokenise(sub, &local_sub, &topics, &sharename);
	اگر(rc){
		بازگشت rc;
	}

	اگر(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], strlen(topics[0]), subhier);
	}وگرنه{
		HASH_FIND(hh, db.normal_subs, topics[0], strlen(topics[0]), subhier);
	}
	اگر(subhier){
		*reason = MQTT_RC_NO_SUBSCRIPTION_EXISTED;
		rc = sub__remove_recurse(context, subhier, topics, reason, sharename);
	}

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	بازگشت rc;
}


صحیح sub__messages_queue(ثابت نویسه *source_id, uint8_t qos, ساختار mosquitto_base_پیام **stored)
{
	صحیح rc = MOSQ_ERR_SUCCESS, rc2;
	صحیح rc_normal = MOSQ_ERR_NO_SUBSCRIBERS, rc_shared = MOSQ_ERR_NO_SUBSCRIBERS;
	ساختار mosquitto__subhier *subhier;
	نویسه **split_topics = پوچ;
	نویسه *local_topic = پوچ;
	unsigned hashv;
	size_t topiclen;
	bool retain = (*stored)->data.retain;
	ثابت نویسه *topic = (*stored)->data.topic;

	أكد(topic);

	اگر(sub__topic_tokenise(topic, &local_topic, &split_topics, پوچ)){
		بازگشت 1;
	}

	/* Protect this message until we have sent it to all
	clients - this is required because websockets client calls
	db__message_write(), which could remove the message if ref_count==0.
	*/
	db_پیام_store_ref_inc(*stored);

	topiclen = strlen(split_topics[0]);
	HASH_مقدار(split_topics[0], topiclen, hashv);
	HASH_FIND_BYHASHVALUE(hh, db.normal_subs, split_topics[0], topiclen, hashv, subhier);
	اگر(subhier){
		rc_normal = sub_جستجو(subhier, split_topics, source_id, topic, qos, retain, *stored);
		اگر(rc_normal > 0){
			rc = rc_normal;
			برو end;
		}
	}

	HASH_FIND_BYHASHVALUE(hh, db.shared_subs, split_topics[0], topiclen, hashv, subhier);
	اگر(subhier){
		rc_shared = sub_جستجو(subhier, split_topics, source_id, topic, qos, retain, *stored);
		اگر(rc_shared > 0){
			rc = rc_shared;
			برو end;
		}
	}

	اگر(rc_normal == MOSQ_ERR_NO_SUBSCRIBERS && rc_shared == MOSQ_ERR_NO_SUBSCRIBERS){
		rc = MOSQ_ERR_NO_SUBSCRIBERS;
	}

	اگر(retain){
		rc2 = retain__store(topic, *stored, split_topics, true);
		اگر(rc2){
			rc = rc2;
		}
	}

end:
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);
	/* Remove our reference and free if needed. */
	db_پیام_store_ref_dec(stored);

	بازگشت rc;
}


/* Remove a subhier element, and return its parent if that needs freeing as well. */
ایستا ساختار mosquitto__subhier *tmp_remove_subs(ساختار mosquitto__subhier *sub)
{
	ساختار mosquitto__subhier *parent;

	اگر(!sub || !sub->parent){
		بازگشت پوچ;
	}

	اگر(sub->children || sub->subs){
		بازگشت پوچ;
	}

	parent = sub->parent;
	HASH_DELETE(hh, parent->children, sub);
	mosquitto_FREE(sub);

	اگر(parent->subs == پوچ
			&& parent->children == پوچ
			&& parent->shared == پوچ
			&& parent->parent){

		بازگشت parent;
	}وگرنه{
		بازگشت پوچ;
	}
}


/* Remove all subscriptions for a client.
 */
صحیح sub__clean_session(ساختار mosquitto *context)
{
	برای(صحیح i=0; i<context->subs_capacity; i++){
		اگر(context->subs[i] == پوچ || context->subs[i]->hier == پوچ){
			ادامه;
		}

		ساختار mosquitto__subhier *hier = context->subs[i]->hier;
		ساختار mosquitto__subleaf *leaf;

		plugin_persist__handle_subscription_delete(context, context->subs[i]->topic_filter);
		اگر(context->subs[i]->shared){
			leaf = context->subs[i]->shared->subs;
			وقتی(leaf){
				اگر(leaf->context==context){
#ifdef WITH_SYS_درخت
					db.shared_subscription_شمار--;
#endif
					sub__remove_shared_leaf(context->subs[i]->hier, context->subs[i]->shared, leaf);
					بشکن;
				}
				leaf = leaf->next;
			}
		}وگرنه{
			leaf = hier->subs;
			وقتی(leaf){
				اگر(leaf->context==context){
#ifdef WITH_SYS_درخت
					db.subscription_شمار--;
#endif
					DL_DELETE(hier->subs, leaf);
					بشکن;
				}
				leaf = leaf->next;
			}
		}
		mosquitto_FREE(context->subs[i]);

		اگر(hier->subs == پوچ
				&& hier->children == پوچ
				&& hier->shared == پوچ
				&& hier->parent){

			انجامبده{
				hier = tmp_remove_subs(hier);
			}وقتی(hier);
		}
	}
	mosquitto_FREE(context->subs);
	context->subs_capacity = 0;
	context->subs_شمار = 0;

	بازگشت MOSQ_ERR_SUCCESS;
}


تهی sub_درخت_print(ساختار mosquitto__subhier *ریشه, صحیح level)
{
	صحیح i;
	ساختار mosquitto__subhier *branch, *branch_tmp;
	ساختار mosquitto__subleaf *leaf;

	HASH_ITER(hh, ریشه, branch, branch_tmp){
		اگر(level > -1){
			برای(i=0; i<(level+2)*2; i++){
				printf(" ");
			}
			printf("%s", branch->topic);
			leaf = branch->subs;
			وقتی(leaf){
				اگر(leaf->context){
					printf(" (%s, %d)", leaf->context->id, MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}وگرنه{
					printf(" (%s, %d)", "", MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}
				leaf = leaf->next;
			}
			printf("\n");
		}

		sub_درخت_print(branch->children, level+1);
	}
}


صحیح sub__init(تهی)
{
	HASH_مقدار("+", 1, hashv_plus);
	HASH_مقدار("#", 1, hashv_hash);

	اگر(sub__add_hier_entry(پوچ, &db.shared_subs, "", 0) == پوچ
			|| sub__add_hier_entry(پوچ, &db.normal_subs, "", 0) == پوچ
			|| sub__add_hier_entry(پوچ, &db.normal_subs, "$SYS", (uint16_t)strlen("$SYS")) == پوچ
			){

		بازگشت MOSQ_ERR_NOMEM;
	}وگرنه{
		بازگشت MOSQ_ERR_SUCCESS;
	}
}
