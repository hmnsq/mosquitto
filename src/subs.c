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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto/mqtt_protocol.h"
#অন্তর্ভাব "util_mosq.h"

#অন্তর্ভাব "utlist.h"

স্থির সংরচনা mosquitto__subhier *sub__add_hier_entry(সংরচনা mosquitto__subhier *parent, সংরচনা mosquitto__subhier **sibling, ধ্রুব বর্ণ *topic, uint16_t len);

স্থির unsigned পূর্ণাঙ্ক hashv_plus = 0;
স্থির unsigned পূর্ণাঙ্ক hashv_hash = 0;


স্থির পূর্ণাঙ্ক subs__send(সংরচনা mosquitto__subleaf *leaf, ধ্রুব বর্ণ *topic, uint8_t qos, পূর্ণাঙ্ক retain, সংরচনা mosquitto_base_বার্তা *stored)
{
	bool client_retain;
	uint16_t mid;
	uint8_t client_qos, বার্তা_qos;
	পূর্ণাঙ্ক rc2;

	/* Check for ACL topic access. */
	rc2 = mosquitto_acl_check(leaf->context, topic, stored->data.payloadlen, stored->data.payload, stored->data.qos, stored->data.retain, stored->data.properties, MOSQ_ACL_READ);
	যদি(rc2 == MOSQ_ERR_ACL_DENIED){
		প্রত্যাগম MOSQ_ERR_SUCCESS;
	}অন্যথা যদি(rc2 == MOSQ_ERR_SUCCESS){
		client_qos = MQTT_SUB_OPT_GET_QOS(leaf->subscription_options);

		যদি(db.config->upgrade_outgoing_qos){
			বার্তা_qos = client_qos;
		}অন্যথা{
			যদি(qos > client_qos){
				বার্তা_qos = client_qos;
			}অন্যথা{
				বার্তা_qos = qos;
			}
		}
		যদি(বার্তা_qos){
			mid = mosquitto__mid_generate(leaf->context);
		}অন্যথা{
			mid = 0;
		}
		যদি(MQTT_SUB_OPT_GET_RETAIN_AS_PUBLISHED(leaf->subscription_options)){
			client_retain = retain;
		}অন্যথা{
			client_retain = false;
		}
		যদি(db_message_ঢোকাও_outgoing(leaf->context, 0, mid, বার্তা_qos, client_retain, stored, leaf->identifier, true, true) == 1){
			প্রত্যাগম 1;
		}
	}অন্যথা{
		প্রত্যাগম 1; /* Application error */
	}
	প্রত্যাগম 0;
}


স্থির পূর্ণাঙ্ক subs__shared_process(সংরচনা mosquitto__subhier *hier, ধ্রুব বর্ণ *topic, uint8_t qos, পূর্ণাঙ্ক retain, সংরচনা mosquitto_base_বার্তা *stored)
{
	পূর্ণাঙ্ক rc = 0, rc2;
	সংরচনা mosquitto__subshared *shared, *shared_tmp;
	সংরচনা mosquitto__subleaf *leaf;

	HASH_ITER(hh, hier->shared, shared, shared_tmp){
		leaf = shared->subs;
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		/* Remove current from the top, add back to the bottom */
		DL_DELETE(shared->subs, leaf);
		DL_APPEND(shared->subs, leaf);

		যদি(rc2){
			rc = 1;
		}
	}

	প্রত্যাগম rc;
}


স্থির পূর্ণাঙ্ক subs__process(সংরচনা mosquitto__subhier *hier, ধ্রুব বর্ণ *source_id, ধ্রুব বর্ণ *topic, uint8_t qos, পূর্ণাঙ্ক retain, সংরচনা mosquitto_base_বার্তা *stored)
{
	পূর্ণাঙ্ক rc = 0;
	পূর্ণাঙ্ক rc2;
	সংরচনা mosquitto__subleaf *leaf;

	rc = subs__shared_process(hier, topic, qos, retain, stored);

	leaf = hier->subs;
	যাবত্(source_id && leaf){
		যদি(!leaf->context->id || (MQTT_SUB_OPT_GET_NO_LOCAL(leaf->subscription_options) && !strcmp(leaf->context->id, source_id))){
			leaf = leaf->next;
			অনুবর্ত;
		}
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		যদি(rc2){
			rc = 1;
		}
		leaf = leaf->next;
	}
	যদি(hier->subs || hier->shared){
		প্রত্যাগম rc;
	}অন্যথা{
		প্রত্যাগম MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


স্থির পূর্ণাঙ্ক sub__add_leaf(সংরচনা mosquitto *context, ধ্রুব সংরচনা mosquitto_subscription *sub, সংরচনা mosquitto__subleaf **head, সংরচনা mosquitto__subleaf **newleaf)
{
	সংরচনা mosquitto__subleaf *leaf;

	*newleaf = রিক্ত;
	leaf = *head;

	যাবত্(leaf){
		যদি(leaf->context && leaf->context->id && !strcmp(leaf->context->id, context->id)){
			/* Client making a second subscription to same topic. Only
			 * need to update QoS. Return MOSQ_ERR_SUB_EXISTS to
			 * indicate this to the calling function. */
			leaf->identifier = sub->identifier;
			leaf->subscription_options = sub->options;
			প্রত্যাগম MOSQ_ERR_SUB_EXISTS;
		}
		leaf = leaf->next;
	}
	leaf = mosquitto_calloc(1, পরিমাণ(সংরচনা mosquitto__subleaf) + strlen(sub->topic_filter) + 1);
	যদি(!leaf){
		প্রত্যাগম MOSQ_ERR_NOMEM;
	}
	leaf->context = context;
	leaf->identifier = sub->identifier;
	leaf->subscription_options = sub->options;
	strcpy(leaf->topic_filter, sub->topic_filter);

	DL_APPEND(*head, leaf);
	*newleaf = leaf;

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


স্থির শূন্য sub__remove_shared_leaf(সংরচনা mosquitto__subhier *subhier, সংরচনা mosquitto__subshared *shared, সংরচনা mosquitto__subleaf *leaf)
{
	DL_DELETE(shared->subs, leaf);
	যদি(shared->subs == রিক্ত){
		HASH_DELETE(hh, subhier->shared, shared);
		mosquitto_FREE(shared);
	}
}


স্থির পূর্ণাঙ্ক sub__add_shared(সংরচনা mosquitto *context, ধ্রুব সংরচনা mosquitto_subscription *sub, সংরচনা mosquitto__subhier *subhier, ধ্রুব বর্ণ *sharename)
{
	সংরচনা mosquitto__subleaf *newleaf;
	সংরচনা mosquitto__subshared *shared = রিক্ত;
	সংরচনা mosquitto__subleaf **subs;
	size_t slen;
	পূর্ণাঙ্ক rc;
	unsigned hashv;

	slen = strlen(sharename);

	HASH_মান(sharename, slen, hashv);

	HASH_FIND_BYHASHVALUE(hh, subhier->shared, sharename, slen, hashv, shared);
	যদি(shared == রিক্ত){
		shared = mosquitto_calloc(1, পরিমাণ(সংরচনা mosquitto__subshared) + slen + 1);
		যদি(!shared){
			প্রত্যাগম MOSQ_ERR_NOMEM;
		}
		strncpy(shared->name, sharename, slen+1);

		HASH_ADD_BYHASHVALUE(hh, subhier->shared, name, slen, hashv, shared);
	}

	rc = sub__add_leaf(context, sub, &shared->subs, &newleaf);
	যদি(rc > 0){
		যদি(shared->subs == রিক্ত){
			HASH_DELETE(hh, subhier->shared, shared);
			mosquitto_FREE(shared);
		}
		প্রত্যাগম rc;
	}

	যদি(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = shared;

		bool assigned = false;
		প্রতি(পূর্ণাঙ্ক i=0; i<context->subs_capacity; i++){
			যদি(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_গুনতি++;
				assigned = true;
				ভঙ্গ;
			}
		}
		যদি(assigned == false){
			subs = mosquitto_realloc(context->subs, পরিমাণ(সংরচনা mosquitto__subleaf *)*(size_t)(context->subs_capacity + 1));
			যদি(!subs){
				sub__remove_shared_leaf(subhier, shared, newleaf);
				mosquitto_FREE(newleaf);
				প্রত্যাগম MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_capacity++;
			context->subs_গুনতি++;
			context->subs[context->subs_capacity-1] = newleaf;
		}
#ifdef WITH_SYS_বৃক্ষ
		db.shared_subscription_গুনতি++;
#endif
	}

	যদি(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		প্রত্যাগম rc;
	}অন্যথা{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		প্রত্যাগম MOSQ_ERR_SUCCESS;
	}
}


স্থির পূর্ণাঙ্ক sub__add_normal(সংরচনা mosquitto *context, ধ্রুব সংরচনা mosquitto_subscription *sub, সংরচনা mosquitto__subhier *subhier)
{
	সংরচনা mosquitto__subleaf *newleaf = রিক্ত;
	সংরচনা mosquitto__subleaf **subs;
	পূর্ণাঙ্ক rc;

	rc = sub__add_leaf(context, sub, &subhier->subs, &newleaf);
	যদি(rc > 0){
		প্রত্যাগম rc;
	}

	যদি(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = রিক্ত;

		bool assigned = false;
		প্রতি(পূর্ণাঙ্ক i=0; i<context->subs_capacity; i++){
			যদি(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_গুনতি++;
				assigned = true;
				ভঙ্গ;
			}
		}
		যদি(assigned == false){
			subs = mosquitto_realloc(context->subs, পরিমাণ(সংরচনা mosquitto__subleaf *)*(size_t)(context->subs_capacity + 1));
			যদি(!subs){
				DL_DELETE(subhier->subs, newleaf);
				mosquitto_FREE(newleaf);
				প্রত্যাগম MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_capacity++;
			context->subs_গুনতি++;
			context->subs[context->subs_capacity-1] = newleaf;
		}
#ifdef WITH_SYS_বৃক্ষ
		db.subscription_গুনতি++;
#endif
	}

	যদি(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		প্রত্যাগম rc;
	}অন্যথা{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		প্রত্যাগম MOSQ_ERR_SUCCESS;
	}
}


স্থির পূর্ণাঙ্ক sub__add_context(সংরচনা mosquitto *context, ধ্রুব সংরচনা mosquitto_subscription *sub, সংরচনা mosquitto__subhier *subhier, বর্ণ *ধ্রুব *ধ্রুব topics, ধ্রুব বর্ণ *sharename)
{
	সংরচনা mosquitto__subhier *branch;
	পূর্ণাঙ্ক topic_index = 0;
	size_t topiclen;

	/* Find leaf node */
	যাবত্(topics && topics[topic_index] != রিক্ত){
		topiclen = strlen(topics[topic_index]);
		যদি(topiclen > UINT16_MAX){
			প্রত্যাগম MOSQ_ERR_INVAL;
		}
		HASH_FIND(hh, subhier->children, topics[topic_index], topiclen, branch);
		যদি(!branch){
			/* Not found */
			branch = sub__add_hier_entry(subhier, &subhier->children, topics[topic_index], (uint16_t)topiclen);
			যদি(!branch){
				প্রত্যাগম MOSQ_ERR_NOMEM;
			}
		}
		subhier = branch;
		topic_index++;
	}

	/* Add add our context */
	যদি(context && context->id){
		যদি(sharename){
			প্রত্যাগম sub__add_shared(context, sub, subhier, sharename);
		}অন্যথা{
			প্রত্যাগম sub__add_normal(context, sub, subhier);
		}
	}অন্যথা{
		প্রত্যাগম MOSQ_ERR_SUCCESS;
	}
}


স্থির পূর্ণাঙ্ক sub__remove_normal(সংরচনা mosquitto *context, সংরচনা mosquitto__subhier *subhier, uint8_t *reason)
{
	সংরচনা mosquitto__subleaf *leaf;

	leaf = subhier->subs;
	যাবত্(leaf){
		যদি(leaf->context==context){
#ifdef WITH_SYS_বৃক্ষ
			db.subscription_গুনতি--;
#endif
			DL_DELETE(subhier->subs, leaf);

			/* Remove the reference to the sub that the client is keeping.
			 * It would be nice to be able to use the reference directly,
			 * but that would involve keeping a copy of the topic string in
			 * each subleaf. Might be worth considering though. */
			প্রতি(পূর্ণাঙ্ক i=0; i<context->subs_capacity; i++){
				যদি(context->subs[i] && context->subs[i]->hier == subhier){
					context->subs_গুনতি--;
					mosquitto_free(context->subs[i]);
					context->subs[i] = রিক্ত;
					ভঙ্গ;
				}
			}
			*reason = 0;
			প্রত্যাগম MOSQ_ERR_SUCCESS;
		}
		leaf = leaf->next;
	}
	প্রত্যাগম MOSQ_ERR_NO_SUBSCRIBERS;
}


স্থির পূর্ণাঙ্ক sub__remove_shared(সংরচনা mosquitto *context, সংরচনা mosquitto__subhier *subhier, uint8_t *reason, ধ্রুব বর্ণ *sharename)
{
	সংরচনা mosquitto__subshared *shared;

	HASH_FIND(hh, subhier->shared, sharename, strlen(sharename), shared);
	যদি(shared){
		সংরচনা mosquitto__subleaf *leaf = shared->subs;
		যাবত্(leaf){
			যদি(leaf->context==context){
#ifdef WITH_SYS_বৃক্ষ
				db.shared_subscription_গুনতি--;
#endif
				DL_DELETE(shared->subs, leaf);

				/* Remove the reference to the sub that the client is keeping.
				* It would be nice to be able to use the reference directly,
				* but that would involve keeping a copy of the topic string in
				* each subleaf. Might be worth considering though. */
				প্রতি(পূর্ণাঙ্ক i=0; i<context->subs_capacity; i++){
					যদি(context->subs[i]
							&& context->subs[i]->hier == subhier
							&& context->subs[i]->shared == shared){

						mosquitto_free(context->subs[i]);
						context->subs[i] = রিক্ত;
						context->subs_গুনতি--;
						ভঙ্গ;
					}
				}

				যদি(shared->subs == রিক্ত){
					HASH_DELETE(hh, subhier->shared, shared);
					mosquitto_FREE(shared);
				}

				*reason = 0;
				প্রত্যাগম MOSQ_ERR_SUCCESS;
			}
			leaf = leaf->next;
		}
		প্রত্যাগম MOSQ_ERR_NO_SUBSCRIBERS;
	}অন্যথা{
		প্রত্যাগম MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


স্থির পূর্ণাঙ্ক sub__remove_recurse(সংরচনা mosquitto *context, সংরচনা mosquitto__subhier *subhier, বর্ণ **topics, uint8_t *reason, ধ্রুব বর্ণ *sharename)
{
	সংরচনা mosquitto__subhier *branch;

	যদি(topics == রিক্ত || topics[0] == রিক্ত){
		যদি(sharename){
			প্রত্যাগম sub__remove_shared(context, subhier, reason, sharename);
		}অন্যথা{
			প্রত্যাগম sub__remove_normal(context, subhier, reason);
		}
	}

	HASH_FIND(hh, subhier->children, topics[0], strlen(topics[0]), branch);
	যদি(branch){
		sub__remove_recurse(context, branch, &(topics[1]), reason, sharename);
		যদি(!branch->children && !branch->subs && !branch->shared){
			HASH_DELETE(hh, subhier->children, branch);
			mosquitto_FREE(branch);
		}
	}
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণাঙ্ক sub_অনুসন্ধান(সংরচনা mosquitto__subhier *subhier, বর্ণ **split_topics, ধ্রুব বর্ণ *source_id, ধ্রুব বর্ণ *topic, uint8_t qos, পূর্ণাঙ্ক retain, সংরচনা mosquitto_base_বার্তা *stored)
{
	/* FIXME - need to take into account source_id if the client is a bridge */
	সংরচনা mosquitto__subhier *branch;
	পূর্ণাঙ্ক rc;
	bool have_subscribers = false;

	যদি(split_topics && split_topics[0]){
		/* Check for literal match */
		HASH_FIND(hh, subhier->children, split_topics[0], strlen(split_topics[0]), branch);

		যদি(branch){
			rc = sub_অনুসন্ধান(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			যদি(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}অন্যথা যদি(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				প্রত্যাগম rc;
			}
			যদি(split_topics[1] == রিক্ত){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				যদি(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}অন্যথা যদি(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					প্রত্যাগম rc;
				}
			}
		}

		/* Check for + match */
		HASH_FIND_BYHASHVALUE(hh, subhier->children, "+", 1, hashv_plus, branch);

		যদি(branch){
			rc = sub_অনুসন্ধান(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			যদি(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}অন্যথা যদি(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				প্রত্যাগম rc;
			}
			যদি(split_topics[1] == রিক্ত){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				যদি(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}অন্যথা যদি(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					প্রত্যাগম rc;
				}
			}
		}
	}

	/* Check for # match */
	HASH_FIND_BYHASHVALUE(hh, subhier->children, "#", 1, hashv_hash, branch);
	যদি(branch && !branch->children){
		/* The topic matches due to a # wildcard - process the
		 * subscriptions but *don't* return. Although this branch has ended
		 * there may still be other subscriptions to deal with.
		 */
		rc = subs__process(branch, source_id, topic, qos, retain, stored);
		যদি(rc == MOSQ_ERR_SUCCESS){
			have_subscribers = true;
		}অন্যথা যদি(rc != MOSQ_ERR_NO_SUBSCRIBERS){
			প্রত্যাগম rc;
		}
	}

	যদি(have_subscribers){
		প্রত্যাগম MOSQ_ERR_SUCCESS;
	}অন্যথা{
		প্রত্যাগম MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


স্থির সংরচনা mosquitto__subhier *sub__add_hier_entry(সংরচনা mosquitto__subhier *parent, সংরচনা mosquitto__subhier **sibling, ধ্রুব বর্ণ *topic, uint16_t len)
{
	সংরচনা mosquitto__subhier *child;

	প্রতিজ্ঞা(sibling);

	child = mosquitto_calloc(1, পরিমাণ(সংরচনা mosquitto__subhier) + len + 1);
	যদি(!child){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
		প্রত্যাগম রিক্ত;
	}
	child->parent = parent;
	child->topic_len = len;
	যদি(len > 0){
		strncpy(child->topic, topic, (size_t)(len+1));
	}

	HASH_ADD(hh, *sibling, topic, child->topic_len, child);

	প্রত্যাগম child;
}


পূর্ণাঙ্ক sub__add(সংরচনা mosquitto *context, ধ্রুব সংরচনা mosquitto_subscription *sub)
{
	পূর্ণাঙ্ক rc = 0;
	সংরচনা mosquitto__subhier *subhier;
	ধ্রুব বর্ণ *sharename = রিক্ত;
	বর্ণ *local_sub;
	বর্ণ **topics;
	size_t topiclen;

	প্রতিজ্ঞা(sub);
	প্রতিজ্ঞা(sub->topic_filter);

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &topics, &sharename);
	যদি(rc){
		প্রত্যাগম rc;
	}

	topiclen = strlen(topics[0]);
	যদি(topiclen > UINT16_MAX){
		mosquitto_FREE(local_sub);
		mosquitto_FREE(topics);
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	যদি(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], topiclen, subhier);
		যদি(!subhier){
			subhier = sub__add_hier_entry(রিক্ত, &db.shared_subs, topics[0], (uint16_t)topiclen);
			যদি(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
				প্রত্যাগম MOSQ_ERR_NOMEM;
			}
		}
	}অন্যথা{
		HASH_FIND(hh, db.normal_subs, topics[0], topiclen, subhier);
		যদি(!subhier){
			subhier = sub__add_hier_entry(রিক্ত, &db.normal_subs, topics[0], (uint16_t)topiclen);
			যদি(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
				প্রত্যাগম MOSQ_ERR_NOMEM;
			}
		}
	}
	rc = sub__add_context(context, sub, subhier, topics, sharename);

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	প্রত্যাগম rc;
}


পূর্ণাঙ্ক sub__remove(সংরচনা mosquitto *context, ধ্রুব বর্ণ *sub, uint8_t *reason)
{
	পূর্ণাঙ্ক rc = 0;
	সংরচনা mosquitto__subhier *subhier;
	ধ্রুব বর্ণ *sharename = রিক্ত;
	বর্ণ *local_sub = রিক্ত;
	বর্ণ **topics = রিক্ত;

	প্রতিজ্ঞা(sub);

	rc = sub__topic_tokenise(sub, &local_sub, &topics, &sharename);
	যদি(rc){
		প্রত্যাগম rc;
	}

	যদি(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], strlen(topics[0]), subhier);
	}অন্যথা{
		HASH_FIND(hh, db.normal_subs, topics[0], strlen(topics[0]), subhier);
	}
	যদি(subhier){
		*reason = MQTT_RC_NO_SUBSCRIPTION_EXISTED;
		rc = sub__remove_recurse(context, subhier, topics, reason, sharename);
	}

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	প্রত্যাগম rc;
}


পূর্ণাঙ্ক sub__messages_queue(ধ্রুব বর্ণ *source_id, uint8_t qos, সংরচনা mosquitto_base_বার্তা **stored)
{
	পূর্ণাঙ্ক rc = MOSQ_ERR_SUCCESS, rc2;
	পূর্ণাঙ্ক rc_normal = MOSQ_ERR_NO_SUBSCRIBERS, rc_shared = MOSQ_ERR_NO_SUBSCRIBERS;
	সংরচনা mosquitto__subhier *subhier;
	বর্ণ **split_topics = রিক্ত;
	বর্ণ *local_topic = রিক্ত;
	unsigned hashv;
	size_t topiclen;
	bool retain = (*stored)->data.retain;
	ধ্রুব বর্ণ *topic = (*stored)->data.topic;

	প্রতিজ্ঞা(topic);

	যদি(sub__topic_tokenise(topic, &local_topic, &split_topics, রিক্ত)){
		প্রত্যাগম 1;
	}

	/* Protect this message until we have sent it to all
	clients - this is required because websockets client calls
	db__message_write(), which could remove the message if ref_count==0.
	*/
	db_বার্তা_store_ref_inc(*stored);

	topiclen = strlen(split_topics[0]);
	HASH_মান(split_topics[0], topiclen, hashv);
	HASH_FIND_BYHASHVALUE(hh, db.normal_subs, split_topics[0], topiclen, hashv, subhier);
	যদি(subhier){
		rc_normal = sub_অনুসন্ধান(subhier, split_topics, source_id, topic, qos, retain, *stored);
		যদি(rc_normal > 0){
			rc = rc_normal;
			গচ্ছ end;
		}
	}

	HASH_FIND_BYHASHVALUE(hh, db.shared_subs, split_topics[0], topiclen, hashv, subhier);
	যদি(subhier){
		rc_shared = sub_অনুসন্ধান(subhier, split_topics, source_id, topic, qos, retain, *stored);
		যদি(rc_shared > 0){
			rc = rc_shared;
			গচ্ছ end;
		}
	}

	যদি(rc_normal == MOSQ_ERR_NO_SUBSCRIBERS && rc_shared == MOSQ_ERR_NO_SUBSCRIBERS){
		rc = MOSQ_ERR_NO_SUBSCRIBERS;
	}

	যদি(retain){
		rc2 = retain__store(topic, *stored, split_topics, true);
		যদি(rc2){
			rc = rc2;
		}
	}

end:
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);
	/* Remove our reference and free if needed. */
	db_বার্তা_store_ref_dec(stored);

	প্রত্যাগম rc;
}


/* Remove a subhier element, and return its parent if that needs freeing as well. */
স্থির সংরচনা mosquitto__subhier *tmp_remove_subs(সংরচনা mosquitto__subhier *sub)
{
	সংরচনা mosquitto__subhier *parent;

	যদি(!sub || !sub->parent){
		প্রত্যাগম রিক্ত;
	}

	যদি(sub->children || sub->subs){
		প্রত্যাগম রিক্ত;
	}

	parent = sub->parent;
	HASH_DELETE(hh, parent->children, sub);
	mosquitto_FREE(sub);

	যদি(parent->subs == রিক্ত
			&& parent->children == রিক্ত
			&& parent->shared == রিক্ত
			&& parent->parent){

		প্রত্যাগম parent;
	}অন্যথা{
		প্রত্যাগম রিক্ত;
	}
}


/* Remove all subscriptions for a client.
 */
পূর্ণাঙ্ক sub__clean_session(সংরচনা mosquitto *context)
{
	প্রতি(পূর্ণাঙ্ক i=0; i<context->subs_capacity; i++){
		যদি(context->subs[i] == রিক্ত || context->subs[i]->hier == রিক্ত){
			অনুবর্ত;
		}

		সংরচনা mosquitto__subhier *hier = context->subs[i]->hier;
		সংরচনা mosquitto__subleaf *leaf;

		plugin_persist__handle_subscription_delete(context, context->subs[i]->topic_filter);
		যদি(context->subs[i]->shared){
			leaf = context->subs[i]->shared->subs;
			যাবত্(leaf){
				যদি(leaf->context==context){
#ifdef WITH_SYS_বৃক্ষ
					db.shared_subscription_গুনতি--;
#endif
					sub__remove_shared_leaf(context->subs[i]->hier, context->subs[i]->shared, leaf);
					ভঙ্গ;
				}
				leaf = leaf->next;
			}
		}অন্যথা{
			leaf = hier->subs;
			যাবত্(leaf){
				যদি(leaf->context==context){
#ifdef WITH_SYS_বৃক্ষ
					db.subscription_গুনতি--;
#endif
					DL_DELETE(hier->subs, leaf);
					ভঙ্গ;
				}
				leaf = leaf->next;
			}
		}
		mosquitto_FREE(context->subs[i]);

		যদি(hier->subs == রিক্ত
				&& hier->children == রিক্ত
				&& hier->shared == রিক্ত
				&& hier->parent){

			কুরু{
				hier = tmp_remove_subs(hier);
			}যাবত্(hier);
		}
	}
	mosquitto_FREE(context->subs);
	context->subs_capacity = 0;
	context->subs_গুনতি = 0;

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


শূন্য sub_বৃক্ষ_print(সংরচনা mosquitto__subhier *মূল, পূর্ণাঙ্ক level)
{
	পূর্ণাঙ্ক i;
	সংরচনা mosquitto__subhier *branch, *branch_tmp;
	সংরচনা mosquitto__subleaf *leaf;

	HASH_ITER(hh, মূল, branch, branch_tmp){
		যদি(level > -1){
			প্রতি(i=0; i<(level+2)*2; i++){
				printf(" ");
			}
			printf("%s", branch->topic);
			leaf = branch->subs;
			যাবত্(leaf){
				যদি(leaf->context){
					printf(" (%s, %d)", leaf->context->id, MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}অন্যথা{
					printf(" (%s, %d)", "", MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}
				leaf = leaf->next;
			}
			printf("\n");
		}

		sub_বৃক্ষ_print(branch->children, level+1);
	}
}


পূর্ণাঙ্ক sub__init(শূন্য)
{
	HASH_মান("+", 1, hashv_plus);
	HASH_মান("#", 1, hashv_hash);

	যদি(sub__add_hier_entry(রিক্ত, &db.shared_subs, "", 0) == রিক্ত
			|| sub__add_hier_entry(রিক্ত, &db.normal_subs, "", 0) == রিক্ত
			|| sub__add_hier_entry(রিক্ত, &db.normal_subs, "$SYS", (uint16_t)strlen("$SYS")) == রিক্ত
			){

		প্রত্যাগম MOSQ_ERR_NOMEM;
	}অন্যথা{
		প্রত্যাগম MOSQ_ERR_SUCCESS;
	}
}
