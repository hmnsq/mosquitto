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

স্থির কাঠামো mosquitto__subhier *sub__add_hier_entry(কাঠামো mosquitto__subhier *parent, কাঠামো mosquitto__subhier **sibling, ধ্রুবক অক্ষর *topic, uint16_t len);

স্থির unsigned পূর্ণসংখ্যা hashv_plus = 0;
স্থির unsigned পূর্ণসংখ্যা hashv_hash = 0;


স্থির পূর্ণসংখ্যা subs__send(কাঠামো mosquitto__subleaf *leaf, ধ্রুবক অক্ষর *topic, uint8_t qos, পূর্ণসংখ্যা retain, কাঠামো mosquitto_base_বার্তা *stored)
{
	bool client_retain;
	uint16_t mid;
	uint8_t client_qos, বার্তা_qos;
	পূর্ণসংখ্যা rc2;

	/* Check for ACL topic access. */
	rc2 = mosquitto_acl_check(leaf->context, topic, stored->তথ্য.payloadlen, stored->তথ্য.payload, stored->তথ্য.qos, stored->তথ্য.retain, stored->তথ্য.properties, MOSQ_ACL_READ);
	যদি(rc2 == MOSQ_ERR_ACL_DENIED){
		ফেরত MOSQ_ERR_SUCCESS;
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
			ফেরত 1;
		}
	}অন্যথা{
		ফেরত 1; /* Application error */
	}
	ফেরত 0;
}


স্থির পূর্ণসংখ্যা subs__shared_process(কাঠামো mosquitto__subhier *hier, ধ্রুবক অক্ষর *topic, uint8_t qos, পূর্ণসংখ্যা retain, কাঠামো mosquitto_base_বার্তা *stored)
{
	পূর্ণসংখ্যা rc = 0, rc2;
	কাঠামো mosquitto__subshared *shared, *shared_tmp;
	কাঠামো mosquitto__subleaf *leaf;

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

	ফেরত rc;
}


স্থির পূর্ণসংখ্যা subs__process(কাঠামো mosquitto__subhier *hier, ধ্রুবক অক্ষর *source_id, ধ্রুবক অক্ষর *topic, uint8_t qos, পূর্ণসংখ্যা retain, কাঠামো mosquitto_base_বার্তা *stored)
{
	পূর্ণসংখ্যা rc = 0;
	পূর্ণসংখ্যা rc2;
	কাঠামো mosquitto__subleaf *leaf;

	rc = subs__shared_process(hier, topic, qos, retain, stored);

	leaf = hier->subs;
	যখন(source_id && leaf){
		যদি(!leaf->context->id || (MQTT_SUB_OPT_GET_NO_LOCAL(leaf->subscription_options) && !strcmp(leaf->context->id, source_id))){
			leaf = leaf->পরবর্তী;
			চালু;
		}
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		যদি(rc2){
			rc = 1;
		}
		leaf = leaf->পরবর্তী;
	}
	যদি(hier->subs || hier->shared){
		ফেরত rc;
	}অন্যথা{
		ফেরত MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


স্থির পূর্ণসংখ্যা sub__add_leaf(কাঠামো mosquitto *context, ধ্রুবক কাঠামো mosquitto_subscription *sub, কাঠামো mosquitto__subleaf **শীর্ষ, কাঠামো mosquitto__subleaf **newleaf)
{
	কাঠামো mosquitto__subleaf *leaf;

	*newleaf = ফাঁকা;
	leaf = *শীর্ষ;

	যখন(leaf){
		যদি(leaf->context && leaf->context->id && !strcmp(leaf->context->id, context->id)){
			/* Client making a second subscription to same topic. Only
			 * need to update QoS. Return MOSQ_ERR_SUB_EXISTS to
			 * indicate this to the calling function. */
			leaf->identifier = sub->identifier;
			leaf->subscription_options = sub->options;
			ফেরত MOSQ_ERR_SUB_EXISTS;
		}
		leaf = leaf->পরবর্তী;
	}
	leaf = mosquitto_calloc(1, আকার(কাঠামো mosquitto__subleaf) + strlen(sub->topic_filter) + 1);
	যদি(!leaf){
		ফেরত MOSQ_ERR_NOMEM;
	}
	leaf->context = context;
	leaf->identifier = sub->identifier;
	leaf->subscription_options = sub->options;
	strcpy(leaf->topic_filter, sub->topic_filter);

	DL_APPEND(*শীর্ষ, leaf);
	*newleaf = leaf;

	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির শূন্য sub__remove_shared_leaf(কাঠামো mosquitto__subhier *subhier, কাঠামো mosquitto__subshared *shared, কাঠামো mosquitto__subleaf *leaf)
{
	DL_DELETE(shared->subs, leaf);
	যদি(shared->subs == ফাঁকা){
		HASH_DELETE(hh, subhier->shared, shared);
		mosquitto_FREE(shared);
	}
}


স্থির পূর্ণসংখ্যা sub__add_shared(কাঠামো mosquitto *context, ধ্রুবক কাঠামো mosquitto_subscription *sub, কাঠামো mosquitto__subhier *subhier, ধ্রুবক অক্ষর *sharename)
{
	কাঠামো mosquitto__subleaf *newleaf;
	কাঠামো mosquitto__subshared *shared = ফাঁকা;
	কাঠামো mosquitto__subleaf **subs;
	size_t slen;
	পূর্ণসংখ্যা rc;
	unsigned hashv;

	slen = strlen(sharename);

	HASH_মান(sharename, slen, hashv);

	HASH_FIND_BYHASHVALUE(hh, subhier->shared, sharename, slen, hashv, shared);
	যদি(shared == ফাঁকা){
		shared = mosquitto_calloc(1, আকার(কাঠামো mosquitto__subshared) + slen + 1);
		যদি(!shared){
			ফেরত MOSQ_ERR_NOMEM;
		}
		strncpy(shared->name, sharename, slen+1);

		HASH_ADD_BYHASHVALUE(hh, subhier->shared, name, slen, hashv, shared);
	}

	rc = sub__add_leaf(context, sub, &shared->subs, &newleaf);
	যদি(rc > 0){
		যদি(shared->subs == ফাঁকা){
			HASH_DELETE(hh, subhier->shared, shared);
			mosquitto_FREE(shared);
		}
		ফেরত rc;
	}

	যদি(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = shared;

		bool assigned = false;
		জন্য(পূর্ণসংখ্যা i=0; i<context->subs_capacity; i++){
			যদি(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_গুনতি++;
				assigned = true;
				বিরতি;
			}
		}
		যদি(assigned == false){
			subs = mosquitto_realloc(context->subs, আকার(কাঠামো mosquitto__subleaf *)*(size_t)(context->subs_capacity + 1));
			যদি(!subs){
				sub__remove_shared_leaf(subhier, shared, newleaf);
				mosquitto_FREE(newleaf);
				ফেরত MOSQ_ERR_NOMEM;
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
		ফেরত rc;
	}অন্যথা{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		ফেরত MOSQ_ERR_SUCCESS;
	}
}


স্থির পূর্ণসংখ্যা sub__add_normal(কাঠামো mosquitto *context, ধ্রুবক কাঠামো mosquitto_subscription *sub, কাঠামো mosquitto__subhier *subhier)
{
	কাঠামো mosquitto__subleaf *newleaf = ফাঁকা;
	কাঠামো mosquitto__subleaf **subs;
	পূর্ণসংখ্যা rc;

	rc = sub__add_leaf(context, sub, &subhier->subs, &newleaf);
	যদি(rc > 0){
		ফেরত rc;
	}

	যদি(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = ফাঁকা;

		bool assigned = false;
		জন্য(পূর্ণসংখ্যা i=0; i<context->subs_capacity; i++){
			যদি(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_গুনতি++;
				assigned = true;
				বিরতি;
			}
		}
		যদি(assigned == false){
			subs = mosquitto_realloc(context->subs, আকার(কাঠামো mosquitto__subleaf *)*(size_t)(context->subs_capacity + 1));
			যদি(!subs){
				DL_DELETE(subhier->subs, newleaf);
				mosquitto_FREE(newleaf);
				ফেরত MOSQ_ERR_NOMEM;
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
		ফেরত rc;
	}অন্যথা{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		ফেরত MOSQ_ERR_SUCCESS;
	}
}


স্থির পূর্ণসংখ্যা sub__add_context(কাঠামো mosquitto *context, ধ্রুবক কাঠামো mosquitto_subscription *sub, কাঠামো mosquitto__subhier *subhier, অক্ষর *ধ্রুবক *ধ্রুবক topics, ধ্রুবক অক্ষর *sharename)
{
	কাঠামো mosquitto__subhier *branch;
	পূর্ণসংখ্যা topic_সূচক = 0;
	size_t topiclen;

	/* Find leaf node */
	যখন(topics && topics[topic_সূচক] != ফাঁকা){
		topiclen = strlen(topics[topic_সূচক]);
		যদি(topiclen > UINT16_MAX){
			ফেরত MOSQ_ERR_INVAL;
		}
		HASH_FIND(hh, subhier->children, topics[topic_সূচক], topiclen, branch);
		যদি(!branch){
			/* Not found */
			branch = sub__add_hier_entry(subhier, &subhier->children, topics[topic_সূচক], (uint16_t)topiclen);
			যদি(!branch){
				ফেরত MOSQ_ERR_NOMEM;
			}
		}
		subhier = branch;
		topic_সূচক++;
	}

	/* Add add our context */
	যদি(context && context->id){
		যদি(sharename){
			ফেরত sub__add_shared(context, sub, subhier, sharename);
		}অন্যথা{
			ফেরত sub__add_normal(context, sub, subhier);
		}
	}অন্যথা{
		ফেরত MOSQ_ERR_SUCCESS;
	}
}


স্থির পূর্ণসংখ্যা sub__remove_normal(কাঠামো mosquitto *context, কাঠামো mosquitto__subhier *subhier, uint8_t *reason)
{
	কাঠামো mosquitto__subleaf *leaf;

	leaf = subhier->subs;
	যখন(leaf){
		যদি(leaf->context==context){
#ifdef WITH_SYS_বৃক্ষ
			db.subscription_গুনতি--;
#endif
			DL_DELETE(subhier->subs, leaf);

			/* Remove the reference to the sub that the client is keeping.
			 * It would be nice to be able to use the reference directly,
			 * but that would involve keeping a copy of the topic string in
			 * each subleaf. Might be worth considering though. */
			জন্য(পূর্ণসংখ্যা i=0; i<context->subs_capacity; i++){
				যদি(context->subs[i] && context->subs[i]->hier == subhier){
					context->subs_গুনতি--;
					mosquitto_free(context->subs[i]);
					context->subs[i] = ফাঁকা;
					বিরতি;
				}
			}
			*reason = 0;
			ফেরত MOSQ_ERR_SUCCESS;
		}
		leaf = leaf->পরবর্তী;
	}
	ফেরত MOSQ_ERR_NO_SUBSCRIBERS;
}


স্থির পূর্ণসংখ্যা sub__remove_shared(কাঠামো mosquitto *context, কাঠামো mosquitto__subhier *subhier, uint8_t *reason, ধ্রুবক অক্ষর *sharename)
{
	কাঠামো mosquitto__subshared *shared;

	HASH_FIND(hh, subhier->shared, sharename, strlen(sharename), shared);
	যদি(shared){
		কাঠামো mosquitto__subleaf *leaf = shared->subs;
		যখন(leaf){
			যদি(leaf->context==context){
#ifdef WITH_SYS_বৃক্ষ
				db.shared_subscription_গুনতি--;
#endif
				DL_DELETE(shared->subs, leaf);

				/* Remove the reference to the sub that the client is keeping.
				* It would be nice to be able to use the reference directly,
				* but that would involve keeping a copy of the topic string in
				* each subleaf. Might be worth considering though. */
				জন্য(পূর্ণসংখ্যা i=0; i<context->subs_capacity; i++){
					যদি(context->subs[i]
							&& context->subs[i]->hier == subhier
							&& context->subs[i]->shared == shared){

						mosquitto_free(context->subs[i]);
						context->subs[i] = ফাঁকা;
						context->subs_গুনতি--;
						বিরতি;
					}
				}

				যদি(shared->subs == ফাঁকা){
					HASH_DELETE(hh, subhier->shared, shared);
					mosquitto_FREE(shared);
				}

				*reason = 0;
				ফেরত MOSQ_ERR_SUCCESS;
			}
			leaf = leaf->পরবর্তী;
		}
		ফেরত MOSQ_ERR_NO_SUBSCRIBERS;
	}অন্যথা{
		ফেরত MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


স্থির পূর্ণসংখ্যা sub__remove_recurse(কাঠামো mosquitto *context, কাঠামো mosquitto__subhier *subhier, অক্ষর **topics, uint8_t *reason, ধ্রুবক অক্ষর *sharename)
{
	কাঠামো mosquitto__subhier *branch;

	যদি(topics == ফাঁকা || topics[0] == ফাঁকা){
		যদি(sharename){
			ফেরত sub__remove_shared(context, subhier, reason, sharename);
		}অন্যথা{
			ফেরত sub__remove_normal(context, subhier, reason);
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
	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা sub_অনুসন্ধান(কাঠামো mosquitto__subhier *subhier, অক্ষর **split_topics, ধ্রুবক অক্ষর *source_id, ধ্রুবক অক্ষর *topic, uint8_t qos, পূর্ণসংখ্যা retain, কাঠামো mosquitto_base_বার্তা *stored)
{
	/* FIXME - need to take into account source_id if the client is a bridge */
	কাঠামো mosquitto__subhier *branch;
	পূর্ণসংখ্যা rc;
	bool have_subscribers = false;

	যদি(split_topics && split_topics[0]){
		/* Check for literal match */
		HASH_FIND(hh, subhier->children, split_topics[0], strlen(split_topics[0]), branch);

		যদি(branch){
			rc = sub_অনুসন্ধান(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			যদি(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}অন্যথা যদি(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				ফেরত rc;
			}
			যদি(split_topics[1] == ফাঁকা){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				যদি(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}অন্যথা যদি(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					ফেরত rc;
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
				ফেরত rc;
			}
			যদি(split_topics[1] == ফাঁকা){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				যদি(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}অন্যথা যদি(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					ফেরত rc;
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
			ফেরত rc;
		}
	}

	যদি(have_subscribers){
		ফেরত MOSQ_ERR_SUCCESS;
	}অন্যথা{
		ফেরত MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


স্থির কাঠামো mosquitto__subhier *sub__add_hier_entry(কাঠামো mosquitto__subhier *parent, কাঠামো mosquitto__subhier **sibling, ধ্রুবক অক্ষর *topic, uint16_t len)
{
	কাঠামো mosquitto__subhier *child;

	প্রতিজ্ঞা(sibling);

	child = mosquitto_calloc(1, আকার(কাঠামো mosquitto__subhier) + len + 1);
	যদি(!child){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Out of memory.");
		ফেরত ফাঁকা;
	}
	child->parent = parent;
	child->topic_len = len;
	যদি(len > 0){
		strncpy(child->topic, topic, (size_t)(len+1));
	}

	HASH_ADD(hh, *sibling, topic, child->topic_len, child);

	ফেরত child;
}


পূর্ণসংখ্যা sub__add(কাঠামো mosquitto *context, ধ্রুবক কাঠামো mosquitto_subscription *sub)
{
	পূর্ণসংখ্যা rc = 0;
	কাঠামো mosquitto__subhier *subhier;
	ধ্রুবক অক্ষর *sharename = ফাঁকা;
	অক্ষর *local_sub;
	অক্ষর **topics;
	size_t topiclen;

	প্রতিজ্ঞা(sub);
	প্রতিজ্ঞা(sub->topic_filter);

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &topics, &sharename);
	যদি(rc){
		ফেরত rc;
	}

	topiclen = strlen(topics[0]);
	যদি(topiclen > UINT16_MAX){
		mosquitto_FREE(local_sub);
		mosquitto_FREE(topics);
		ফেরত MOSQ_ERR_INVAL;
	}

	যদি(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], topiclen, subhier);
		যদি(!subhier){
			subhier = sub__add_hier_entry(ফাঁকা, &db.shared_subs, topics[0], (uint16_t)topiclen);
			যদি(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Out of memory.");
				ফেরত MOSQ_ERR_NOMEM;
			}
		}
	}অন্যথা{
		HASH_FIND(hh, db.normal_subs, topics[0], topiclen, subhier);
		যদি(!subhier){
			subhier = sub__add_hier_entry(ফাঁকা, &db.normal_subs, topics[0], (uint16_t)topiclen);
			যদি(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Out of memory.");
				ফেরত MOSQ_ERR_NOMEM;
			}
		}
	}
	rc = sub__add_context(context, sub, subhier, topics, sharename);

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	ফেরত rc;
}


পূর্ণসংখ্যা sub__remove(কাঠামো mosquitto *context, ধ্রুবক অক্ষর *sub, uint8_t *reason)
{
	পূর্ণসংখ্যা rc = 0;
	কাঠামো mosquitto__subhier *subhier;
	ধ্রুবক অক্ষর *sharename = ফাঁকা;
	অক্ষর *local_sub = ফাঁকা;
	অক্ষর **topics = ফাঁকা;

	প্রতিজ্ঞা(sub);

	rc = sub__topic_tokenise(sub, &local_sub, &topics, &sharename);
	যদি(rc){
		ফেরত rc;
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

	ফেরত rc;
}


পূর্ণসংখ্যা sub__messages_queue(ধ্রুবক অক্ষর *source_id, uint8_t qos, কাঠামো mosquitto_base_বার্তা **stored)
{
	পূর্ণসংখ্যা rc = MOSQ_ERR_SUCCESS, rc2;
	পূর্ণসংখ্যা rc_normal = MOSQ_ERR_NO_SUBSCRIBERS, rc_shared = MOSQ_ERR_NO_SUBSCRIBERS;
	কাঠামো mosquitto__subhier *subhier;
	অক্ষর **split_topics = ফাঁকা;
	অক্ষর *local_topic = ফাঁকা;
	unsigned hashv;
	size_t topiclen;
	bool retain = (*stored)->তথ্য.retain;
	ধ্রুবক অক্ষর *topic = (*stored)->তথ্য.topic;

	প্রতিজ্ঞা(topic);

	যদি(sub__topic_tokenise(topic, &local_topic, &split_topics, ফাঁকা)){
		ফেরত 1;
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
			যাও end;
		}
	}

	HASH_FIND_BYHASHVALUE(hh, db.shared_subs, split_topics[0], topiclen, hashv, subhier);
	যদি(subhier){
		rc_shared = sub_অনুসন্ধান(subhier, split_topics, source_id, topic, qos, retain, *stored);
		যদি(rc_shared > 0){
			rc = rc_shared;
			যাও end;
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

	ফেরত rc;
}


/* Remove a subhier element, and return its parent if that needs freeing as well. */
স্থির কাঠামো mosquitto__subhier *tmp_remove_subs(কাঠামো mosquitto__subhier *sub)
{
	কাঠামো mosquitto__subhier *parent;

	যদি(!sub || !sub->parent){
		ফেরত ফাঁকা;
	}

	যদি(sub->children || sub->subs){
		ফেরত ফাঁকা;
	}

	parent = sub->parent;
	HASH_DELETE(hh, parent->children, sub);
	mosquitto_FREE(sub);

	যদি(parent->subs == ফাঁকা
			&& parent->children == ফাঁকা
			&& parent->shared == ফাঁকা
			&& parent->parent){

		ফেরত parent;
	}অন্যথা{
		ফেরত ফাঁকা;
	}
}


/* Remove all subscriptions for a client.
 */
পূর্ণসংখ্যা sub__clean_session(কাঠামো mosquitto *context)
{
	জন্য(পূর্ণসংখ্যা i=0; i<context->subs_capacity; i++){
		যদি(context->subs[i] == ফাঁকা || context->subs[i]->hier == ফাঁকা){
			চালু;
		}

		কাঠামো mosquitto__subhier *hier = context->subs[i]->hier;
		কাঠামো mosquitto__subleaf *leaf;

		plugin_persist__handle_subscription_delete(context, context->subs[i]->topic_filter);
		যদি(context->subs[i]->shared){
			leaf = context->subs[i]->shared->subs;
			যখন(leaf){
				যদি(leaf->context==context){
#ifdef WITH_SYS_বৃক্ষ
					db.shared_subscription_গুনতি--;
#endif
					sub__remove_shared_leaf(context->subs[i]->hier, context->subs[i]->shared, leaf);
					বিরতি;
				}
				leaf = leaf->পরবর্তী;
			}
		}অন্যথা{
			leaf = hier->subs;
			যখন(leaf){
				যদি(leaf->context==context){
#ifdef WITH_SYS_বৃক্ষ
					db.subscription_গুনতি--;
#endif
					DL_DELETE(hier->subs, leaf);
					বিরতি;
				}
				leaf = leaf->পরবর্তী;
			}
		}
		mosquitto_FREE(context->subs[i]);

		যদি(hier->subs == ফাঁকা
				&& hier->children == ফাঁকা
				&& hier->shared == ফাঁকা
				&& hier->parent){

			করো{
				hier = tmp_remove_subs(hier);
			}যখন(hier);
		}
	}
	mosquitto_FREE(context->subs);
	context->subs_capacity = 0;
	context->subs_গুনতি = 0;

	ফেরত MOSQ_ERR_SUCCESS;
}


শূন্য sub_বৃক্ষ_print(কাঠামো mosquitto__subhier *মূল, পূর্ণসংখ্যা level)
{
	পূর্ণসংখ্যা i;
	কাঠামো mosquitto__subhier *branch, *branch_tmp;
	কাঠামো mosquitto__subleaf *leaf;

	HASH_ITER(hh, মূল, branch, branch_tmp){
		যদি(level > -1){
			জন্য(i=0; i<(level+2)*2; i++){
				printf(" ");
			}
			printf("%s", branch->topic);
			leaf = branch->subs;
			যখন(leaf){
				যদি(leaf->context){
					printf(" (%s, %d)", leaf->context->id, MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}অন্যথা{
					printf(" (%s, %d)", "", MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}
				leaf = leaf->পরবর্তী;
			}
			printf("\n");
		}

		sub_বৃক্ষ_print(branch->children, level+1);
	}
}


পূর্ণসংখ্যা sub__init(শূন্য)
{
	HASH_মান("+", 1, hashv_plus);
	HASH_মান("#", 1, hashv_hash);

	যদি(sub__add_hier_entry(ফাঁকা, &db.shared_subs, "", 0) == ফাঁকা
			|| sub__add_hier_entry(ফাঁকা, &db.normal_subs, "", 0) == ফাঁকা
			|| sub__add_hier_entry(ফাঁকা, &db.normal_subs, "$SYS", (uint16_t)strlen("$SYS")) == ফাঁকা
			){

		ফেরত MOSQ_ERR_NOMEM;
	}অন্যথা{
		ফেরত MOSQ_ERR_SUCCESS;
	}
}
