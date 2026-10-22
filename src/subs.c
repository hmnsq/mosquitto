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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto/mqtt_protocol.h"
#அந்தர்பாவ "util_mosq.h"

#அந்தர்பாவ "utlist.h"

நிலையான கட்டமைப்பு mosquitto__subhier *sub__add_hier_entry(கட்டமைப்பு mosquitto__subhier *parent, கட்டமைப்பு mosquitto__subhier **sibling, மாறிலி எழுத்து *topic, uint16_t len);

நிலையான unsigned முழு hashv_plus = 0;
நிலையான unsigned முழு hashv_hash = 0;


நிலையான முழு subs__send(கட்டமைப்பு mosquitto__subleaf *leaf, மாறிலி எழுத்து *topic, uint8_t qos, முழு retain, கட்டமைப்பு mosquitto_base_செய்தி *stored)
{
	bool client_retain;
	uint16_t mid;
	uint8_t client_qos, செய்தி_qos;
	முழு rc2;

	/* Check for ACL topic access. */
	rc2 = mosquitto_acl_check(leaf->context, topic, stored->data.payloadlen, stored->data.payload, stored->data.qos, stored->data.retain, stored->data.properties, MOSQ_ACL_READ);
	எனில்(rc2 == MOSQ_ERR_ACL_DENIED){
		திரும்பு MOSQ_ERR_SUCCESS;
	}இல்லையெனில் எனில்(rc2 == MOSQ_ERR_SUCCESS){
		client_qos = MQTT_SUB_OPT_GET_QOS(leaf->subscription_options);

		எனில்(db.config->upgrade_outgoing_qos){
			செய்தி_qos = client_qos;
		}இல்லையெனில்{
			எனில்(qos > client_qos){
				செய்தி_qos = client_qos;
			}இல்லையெனில்{
				செய்தி_qos = qos;
			}
		}
		எனில்(செய்தி_qos){
			mid = mosquitto__mid_generate(leaf->context);
		}இல்லையெனில்{
			mid = 0;
		}
		எனில்(MQTT_SUB_OPT_GET_RETAIN_AS_PUBLISHED(leaf->subscription_options)){
			client_retain = retain;
		}இல்லையெனில்{
			client_retain = false;
		}
		எனில்(db_message_செருகு_outgoing(leaf->context, 0, mid, செய்தி_qos, client_retain, stored, leaf->identifier, true, true) == 1){
			திரும்பு 1;
		}
	}இல்லையெனில்{
		திரும்பு 1; /* Application error */
	}
	திரும்பு 0;
}


நிலையான முழு subs__shared_process(கட்டமைப்பு mosquitto__subhier *hier, மாறிலி எழுத்து *topic, uint8_t qos, முழு retain, கட்டமைப்பு mosquitto_base_செய்தி *stored)
{
	முழு rc = 0, rc2;
	கட்டமைப்பு mosquitto__subshared *shared, *shared_tmp;
	கட்டமைப்பு mosquitto__subleaf *leaf;

	HASH_ITER(hh, hier->shared, shared, shared_tmp){
		leaf = shared->subs;
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		/* Remove current from the top, add back to the bottom */
		DL_DELETE(shared->subs, leaf);
		DL_APPEND(shared->subs, leaf);

		எனில்(rc2){
			rc = 1;
		}
	}

	திரும்பு rc;
}


நிலையான முழு subs__process(கட்டமைப்பு mosquitto__subhier *hier, மாறிலி எழுத்து *source_id, மாறிலி எழுத்து *topic, uint8_t qos, முழு retain, கட்டமைப்பு mosquitto_base_செய்தி *stored)
{
	முழு rc = 0;
	முழு rc2;
	கட்டமைப்பு mosquitto__subleaf *leaf;

	rc = subs__shared_process(hier, topic, qos, retain, stored);

	leaf = hier->subs;
	போது(source_id && leaf){
		எனில்(!leaf->context->id || (MQTT_SUB_OPT_GET_NO_LOCAL(leaf->subscription_options) && !strcmp(leaf->context->id, source_id))){
			leaf = leaf->next;
			தொடர்;
		}
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		எனில்(rc2){
			rc = 1;
		}
		leaf = leaf->next;
	}
	எனில்(hier->subs || hier->shared){
		திரும்பு rc;
	}இல்லையெனில்{
		திரும்பு MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


நிலையான முழு sub__add_leaf(கட்டமைப்பு mosquitto *context, மாறிலி கட்டமைப்பு mosquitto_subscription *sub, கட்டமைப்பு mosquitto__subleaf **head, கட்டமைப்பு mosquitto__subleaf **newleaf)
{
	கட்டமைப்பு mosquitto__subleaf *leaf;

	*newleaf = வெற்று;
	leaf = *head;

	போது(leaf){
		எனில்(leaf->context && leaf->context->id && !strcmp(leaf->context->id, context->id)){
			/* Client making a second subscription to same topic. Only
			 * need to update QoS. Return MOSQ_ERR_SUB_EXISTS to
			 * indicate this to the calling function. */
			leaf->identifier = sub->identifier;
			leaf->subscription_options = sub->options;
			திரும்பு MOSQ_ERR_SUB_EXISTS;
		}
		leaf = leaf->next;
	}
	leaf = mosquitto_calloc(1, அளவு(கட்டமைப்பு mosquitto__subleaf) + strlen(sub->topic_filter) + 1);
	எனில்(!leaf){
		திரும்பு MOSQ_ERR_NOMEM;
	}
	leaf->context = context;
	leaf->identifier = sub->identifier;
	leaf->subscription_options = sub->options;
	strcpy(leaf->topic_filter, sub->topic_filter);

	DL_APPEND(*head, leaf);
	*newleaf = leaf;

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான வெறுமை sub__remove_shared_leaf(கட்டமைப்பு mosquitto__subhier *subhier, கட்டமைப்பு mosquitto__subshared *shared, கட்டமைப்பு mosquitto__subleaf *leaf)
{
	DL_DELETE(shared->subs, leaf);
	எனில்(shared->subs == வெற்று){
		HASH_DELETE(hh, subhier->shared, shared);
		mosquitto_FREE(shared);
	}
}


நிலையான முழு sub__add_shared(கட்டமைப்பு mosquitto *context, மாறிலி கட்டமைப்பு mosquitto_subscription *sub, கட்டமைப்பு mosquitto__subhier *subhier, மாறிலி எழுத்து *sharename)
{
	கட்டமைப்பு mosquitto__subleaf *newleaf;
	கட்டமைப்பு mosquitto__subshared *shared = வெற்று;
	கட்டமைப்பு mosquitto__subleaf **subs;
	size_t slen;
	முழு rc;
	unsigned hashv;

	slen = strlen(sharename);

	HASH_மதிப்பு(sharename, slen, hashv);

	HASH_FIND_BYHASHVALUE(hh, subhier->shared, sharename, slen, hashv, shared);
	எனில்(shared == வெற்று){
		shared = mosquitto_calloc(1, அளவு(கட்டமைப்பு mosquitto__subshared) + slen + 1);
		எனில்(!shared){
			திரும்பு MOSQ_ERR_NOMEM;
		}
		strncpy(shared->name, sharename, slen+1);

		HASH_ADD_BYHASHVALUE(hh, subhier->shared, name, slen, hashv, shared);
	}

	rc = sub__add_leaf(context, sub, &shared->subs, &newleaf);
	எனில்(rc > 0){
		எனில்(shared->subs == வெற்று){
			HASH_DELETE(hh, subhier->shared, shared);
			mosquitto_FREE(shared);
		}
		திரும்பு rc;
	}

	எனில்(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = shared;

		bool assigned = false;
		க்கு(முழு i=0; i<context->subs_capacity; i++){
			எனில்(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_மொத்தம்++;
				assigned = true;
				நிறுத்து;
			}
		}
		எனில்(assigned == false){
			subs = mosquitto_realloc(context->subs, அளவு(கட்டமைப்பு mosquitto__subleaf *)*(size_t)(context->subs_capacity + 1));
			எனில்(!subs){
				sub__remove_shared_leaf(subhier, shared, newleaf);
				mosquitto_FREE(newleaf);
				திரும்பு MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_capacity++;
			context->subs_மொத்தம்++;
			context->subs[context->subs_capacity-1] = newleaf;
		}
#ifdef WITH_SYS_மரம்
		db.shared_subscription_மொத்தம்++;
#endif
	}

	எனில்(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		திரும்பு rc;
	}இல்லையெனில்{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		திரும்பு MOSQ_ERR_SUCCESS;
	}
}


நிலையான முழு sub__add_normal(கட்டமைப்பு mosquitto *context, மாறிலி கட்டமைப்பு mosquitto_subscription *sub, கட்டமைப்பு mosquitto__subhier *subhier)
{
	கட்டமைப்பு mosquitto__subleaf *newleaf = வெற்று;
	கட்டமைப்பு mosquitto__subleaf **subs;
	முழு rc;

	rc = sub__add_leaf(context, sub, &subhier->subs, &newleaf);
	எனில்(rc > 0){
		திரும்பு rc;
	}

	எனில்(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = வெற்று;

		bool assigned = false;
		க்கு(முழு i=0; i<context->subs_capacity; i++){
			எனில்(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_மொத்தம்++;
				assigned = true;
				நிறுத்து;
			}
		}
		எனில்(assigned == false){
			subs = mosquitto_realloc(context->subs, அளவு(கட்டமைப்பு mosquitto__subleaf *)*(size_t)(context->subs_capacity + 1));
			எனில்(!subs){
				DL_DELETE(subhier->subs, newleaf);
				mosquitto_FREE(newleaf);
				திரும்பு MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_capacity++;
			context->subs_மொத்தம்++;
			context->subs[context->subs_capacity-1] = newleaf;
		}
#ifdef WITH_SYS_மரம்
		db.subscription_மொத்தம்++;
#endif
	}

	எனில்(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		திரும்பு rc;
	}இல்லையெனில்{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		திரும்பு MOSQ_ERR_SUCCESS;
	}
}


நிலையான முழு sub__add_context(கட்டமைப்பு mosquitto *context, மாறிலி கட்டமைப்பு mosquitto_subscription *sub, கட்டமைப்பு mosquitto__subhier *subhier, எழுத்து *மாறிலி *மாறிலி topics, மாறிலி எழுத்து *sharename)
{
	கட்டமைப்பு mosquitto__subhier *branch;
	முழு topic_index = 0;
	size_t topiclen;

	/* Find leaf node */
	போது(topics && topics[topic_index] != வெற்று){
		topiclen = strlen(topics[topic_index]);
		எனில்(topiclen > UINT16_MAX){
			திரும்பு MOSQ_ERR_INVAL;
		}
		HASH_FIND(hh, subhier->children, topics[topic_index], topiclen, branch);
		எனில்(!branch){
			/* Not found */
			branch = sub__add_hier_entry(subhier, &subhier->children, topics[topic_index], (uint16_t)topiclen);
			எனில்(!branch){
				திரும்பு MOSQ_ERR_NOMEM;
			}
		}
		subhier = branch;
		topic_index++;
	}

	/* Add add our context */
	எனில்(context && context->id){
		எனில்(sharename){
			திரும்பு sub__add_shared(context, sub, subhier, sharename);
		}இல்லையெனில்{
			திரும்பு sub__add_normal(context, sub, subhier);
		}
	}இல்லையெனில்{
		திரும்பு MOSQ_ERR_SUCCESS;
	}
}


நிலையான முழு sub__remove_normal(கட்டமைப்பு mosquitto *context, கட்டமைப்பு mosquitto__subhier *subhier, uint8_t *reason)
{
	கட்டமைப்பு mosquitto__subleaf *leaf;

	leaf = subhier->subs;
	போது(leaf){
		எனில்(leaf->context==context){
#ifdef WITH_SYS_மரம்
			db.subscription_மொத்தம்--;
#endif
			DL_DELETE(subhier->subs, leaf);

			/* Remove the reference to the sub that the client is keeping.
			 * It would be nice to be able to use the reference directly,
			 * but that would involve keeping a copy of the topic string in
			 * each subleaf. Might be worth considering though. */
			க்கு(முழு i=0; i<context->subs_capacity; i++){
				எனில்(context->subs[i] && context->subs[i]->hier == subhier){
					context->subs_மொத்தம்--;
					mosquitto_free(context->subs[i]);
					context->subs[i] = வெற்று;
					நிறுத்து;
				}
			}
			*reason = 0;
			திரும்பு MOSQ_ERR_SUCCESS;
		}
		leaf = leaf->next;
	}
	திரும்பு MOSQ_ERR_NO_SUBSCRIBERS;
}


நிலையான முழு sub__remove_shared(கட்டமைப்பு mosquitto *context, கட்டமைப்பு mosquitto__subhier *subhier, uint8_t *reason, மாறிலி எழுத்து *sharename)
{
	கட்டமைப்பு mosquitto__subshared *shared;

	HASH_FIND(hh, subhier->shared, sharename, strlen(sharename), shared);
	எனில்(shared){
		கட்டமைப்பு mosquitto__subleaf *leaf = shared->subs;
		போது(leaf){
			எனில்(leaf->context==context){
#ifdef WITH_SYS_மரம்
				db.shared_subscription_மொத்தம்--;
#endif
				DL_DELETE(shared->subs, leaf);

				/* Remove the reference to the sub that the client is keeping.
				* It would be nice to be able to use the reference directly,
				* but that would involve keeping a copy of the topic string in
				* each subleaf. Might be worth considering though. */
				க்கு(முழு i=0; i<context->subs_capacity; i++){
					எனில்(context->subs[i]
							&& context->subs[i]->hier == subhier
							&& context->subs[i]->shared == shared){

						mosquitto_free(context->subs[i]);
						context->subs[i] = வெற்று;
						context->subs_மொத்தம்--;
						நிறுத்து;
					}
				}

				எனில்(shared->subs == வெற்று){
					HASH_DELETE(hh, subhier->shared, shared);
					mosquitto_FREE(shared);
				}

				*reason = 0;
				திரும்பு MOSQ_ERR_SUCCESS;
			}
			leaf = leaf->next;
		}
		திரும்பு MOSQ_ERR_NO_SUBSCRIBERS;
	}இல்லையெனில்{
		திரும்பு MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


நிலையான முழு sub__remove_recurse(கட்டமைப்பு mosquitto *context, கட்டமைப்பு mosquitto__subhier *subhier, எழுத்து **topics, uint8_t *reason, மாறிலி எழுத்து *sharename)
{
	கட்டமைப்பு mosquitto__subhier *branch;

	எனில்(topics == வெற்று || topics[0] == வெற்று){
		எனில்(sharename){
			திரும்பு sub__remove_shared(context, subhier, reason, sharename);
		}இல்லையெனில்{
			திரும்பு sub__remove_normal(context, subhier, reason);
		}
	}

	HASH_FIND(hh, subhier->children, topics[0], strlen(topics[0]), branch);
	எனில்(branch){
		sub__remove_recurse(context, branch, &(topics[1]), reason, sharename);
		எனில்(!branch->children && !branch->subs && !branch->shared){
			HASH_DELETE(hh, subhier->children, branch);
			mosquitto_FREE(branch);
		}
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு sub_தேடல்(கட்டமைப்பு mosquitto__subhier *subhier, எழுத்து **split_topics, மாறிலி எழுத்து *source_id, மாறிலி எழுத்து *topic, uint8_t qos, முழு retain, கட்டமைப்பு mosquitto_base_செய்தி *stored)
{
	/* FIXME - need to take into account source_id if the client is a bridge */
	கட்டமைப்பு mosquitto__subhier *branch;
	முழு rc;
	bool have_subscribers = false;

	எனில்(split_topics && split_topics[0]){
		/* Check for literal match */
		HASH_FIND(hh, subhier->children, split_topics[0], strlen(split_topics[0]), branch);

		எனில்(branch){
			rc = sub_தேடல்(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			எனில்(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}இல்லையெனில் எனில்(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				திரும்பு rc;
			}
			எனில்(split_topics[1] == வெற்று){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				எனில்(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}இல்லையெனில் எனில்(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					திரும்பு rc;
				}
			}
		}

		/* Check for + match */
		HASH_FIND_BYHASHVALUE(hh, subhier->children, "+", 1, hashv_plus, branch);

		எனில்(branch){
			rc = sub_தேடல்(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			எனில்(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}இல்லையெனில் எனில்(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				திரும்பு rc;
			}
			எனில்(split_topics[1] == வெற்று){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				எனில்(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}இல்லையெனில் எனில்(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					திரும்பு rc;
				}
			}
		}
	}

	/* Check for # match */
	HASH_FIND_BYHASHVALUE(hh, subhier->children, "#", 1, hashv_hash, branch);
	எனில்(branch && !branch->children){
		/* The topic matches due to a # wildcard - process the
		 * subscriptions but *don't* return. Although this branch has ended
		 * there may still be other subscriptions to deal with.
		 */
		rc = subs__process(branch, source_id, topic, qos, retain, stored);
		எனில்(rc == MOSQ_ERR_SUCCESS){
			have_subscribers = true;
		}இல்லையெனில் எனில்(rc != MOSQ_ERR_NO_SUBSCRIBERS){
			திரும்பு rc;
		}
	}

	எனில்(have_subscribers){
		திரும்பு MOSQ_ERR_SUCCESS;
	}இல்லையெனில்{
		திரும்பு MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


நிலையான கட்டமைப்பு mosquitto__subhier *sub__add_hier_entry(கட்டமைப்பு mosquitto__subhier *parent, கட்டமைப்பு mosquitto__subhier **sibling, மாறிலி எழுத்து *topic, uint16_t len)
{
	கட்டமைப்பு mosquitto__subhier *child;

	ப்ரதிஜ்ஞா(sibling);

	child = mosquitto_calloc(1, அளவு(கட்டமைப்பு mosquitto__subhier) + len + 1);
	எனில்(!child){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
		திரும்பு வெற்று;
	}
	child->parent = parent;
	child->topic_len = len;
	எனில்(len > 0){
		strncpy(child->topic, topic, (size_t)(len+1));
	}

	HASH_ADD(hh, *sibling, topic, child->topic_len, child);

	திரும்பு child;
}


முழு sub__add(கட்டமைப்பு mosquitto *context, மாறிலி கட்டமைப்பு mosquitto_subscription *sub)
{
	முழு rc = 0;
	கட்டமைப்பு mosquitto__subhier *subhier;
	மாறிலி எழுத்து *sharename = வெற்று;
	எழுத்து *local_sub;
	எழுத்து **topics;
	size_t topiclen;

	ப்ரதிஜ்ஞா(sub);
	ப்ரதிஜ்ஞா(sub->topic_filter);

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &topics, &sharename);
	எனில்(rc){
		திரும்பு rc;
	}

	topiclen = strlen(topics[0]);
	எனில்(topiclen > UINT16_MAX){
		mosquitto_FREE(local_sub);
		mosquitto_FREE(topics);
		திரும்பு MOSQ_ERR_INVAL;
	}

	எனில்(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], topiclen, subhier);
		எனில்(!subhier){
			subhier = sub__add_hier_entry(வெற்று, &db.shared_subs, topics[0], (uint16_t)topiclen);
			எனில்(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
				திரும்பு MOSQ_ERR_NOMEM;
			}
		}
	}இல்லையெனில்{
		HASH_FIND(hh, db.normal_subs, topics[0], topiclen, subhier);
		எனில்(!subhier){
			subhier = sub__add_hier_entry(வெற்று, &db.normal_subs, topics[0], (uint16_t)topiclen);
			எனில்(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
				திரும்பு MOSQ_ERR_NOMEM;
			}
		}
	}
	rc = sub__add_context(context, sub, subhier, topics, sharename);

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	திரும்பு rc;
}


முழு sub__remove(கட்டமைப்பு mosquitto *context, மாறிலி எழுத்து *sub, uint8_t *reason)
{
	முழு rc = 0;
	கட்டமைப்பு mosquitto__subhier *subhier;
	மாறிலி எழுத்து *sharename = வெற்று;
	எழுத்து *local_sub = வெற்று;
	எழுத்து **topics = வெற்று;

	ப்ரதிஜ்ஞா(sub);

	rc = sub__topic_tokenise(sub, &local_sub, &topics, &sharename);
	எனில்(rc){
		திரும்பு rc;
	}

	எனில்(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], strlen(topics[0]), subhier);
	}இல்லையெனில்{
		HASH_FIND(hh, db.normal_subs, topics[0], strlen(topics[0]), subhier);
	}
	எனில்(subhier){
		*reason = MQTT_RC_NO_SUBSCRIPTION_EXISTED;
		rc = sub__remove_recurse(context, subhier, topics, reason, sharename);
	}

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	திரும்பு rc;
}


முழு sub__messages_queue(மாறிலி எழுத்து *source_id, uint8_t qos, கட்டமைப்பு mosquitto_base_செய்தி **stored)
{
	முழு rc = MOSQ_ERR_SUCCESS, rc2;
	முழு rc_normal = MOSQ_ERR_NO_SUBSCRIBERS, rc_shared = MOSQ_ERR_NO_SUBSCRIBERS;
	கட்டமைப்பு mosquitto__subhier *subhier;
	எழுத்து **split_topics = வெற்று;
	எழுத்து *local_topic = வெற்று;
	unsigned hashv;
	size_t topiclen;
	bool retain = (*stored)->data.retain;
	மாறிலி எழுத்து *topic = (*stored)->data.topic;

	ப்ரதிஜ்ஞா(topic);

	எனில்(sub__topic_tokenise(topic, &local_topic, &split_topics, வெற்று)){
		திரும்பு 1;
	}

	/* Protect this message until we have sent it to all
	clients - this is required because websockets client calls
	db__message_write(), which could remove the message if ref_count==0.
	*/
	db_செய்தி_store_ref_inc(*stored);

	topiclen = strlen(split_topics[0]);
	HASH_மதிப்பு(split_topics[0], topiclen, hashv);
	HASH_FIND_BYHASHVALUE(hh, db.normal_subs, split_topics[0], topiclen, hashv, subhier);
	எனில்(subhier){
		rc_normal = sub_தேடல்(subhier, split_topics, source_id, topic, qos, retain, *stored);
		எனில்(rc_normal > 0){
			rc = rc_normal;
			செல் end;
		}
	}

	HASH_FIND_BYHASHVALUE(hh, db.shared_subs, split_topics[0], topiclen, hashv, subhier);
	எனில்(subhier){
		rc_shared = sub_தேடல்(subhier, split_topics, source_id, topic, qos, retain, *stored);
		எனில்(rc_shared > 0){
			rc = rc_shared;
			செல் end;
		}
	}

	எனில்(rc_normal == MOSQ_ERR_NO_SUBSCRIBERS && rc_shared == MOSQ_ERR_NO_SUBSCRIBERS){
		rc = MOSQ_ERR_NO_SUBSCRIBERS;
	}

	எனில்(retain){
		rc2 = retain__store(topic, *stored, split_topics, true);
		எனில்(rc2){
			rc = rc2;
		}
	}

end:
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);
	/* Remove our reference and free if needed. */
	db_செய்தி_store_ref_dec(stored);

	திரும்பு rc;
}


/* Remove a subhier element, and return its parent if that needs freeing as well. */
நிலையான கட்டமைப்பு mosquitto__subhier *tmp_remove_subs(கட்டமைப்பு mosquitto__subhier *sub)
{
	கட்டமைப்பு mosquitto__subhier *parent;

	எனில்(!sub || !sub->parent){
		திரும்பு வெற்று;
	}

	எனில்(sub->children || sub->subs){
		திரும்பு வெற்று;
	}

	parent = sub->parent;
	HASH_DELETE(hh, parent->children, sub);
	mosquitto_FREE(sub);

	எனில்(parent->subs == வெற்று
			&& parent->children == வெற்று
			&& parent->shared == வெற்று
			&& parent->parent){

		திரும்பு parent;
	}இல்லையெனில்{
		திரும்பு வெற்று;
	}
}


/* Remove all subscriptions for a client.
 */
முழு sub__clean_session(கட்டமைப்பு mosquitto *context)
{
	க்கு(முழு i=0; i<context->subs_capacity; i++){
		எனில்(context->subs[i] == வெற்று || context->subs[i]->hier == வெற்று){
			தொடர்;
		}

		கட்டமைப்பு mosquitto__subhier *hier = context->subs[i]->hier;
		கட்டமைப்பு mosquitto__subleaf *leaf;

		plugin_persist__handle_subscription_delete(context, context->subs[i]->topic_filter);
		எனில்(context->subs[i]->shared){
			leaf = context->subs[i]->shared->subs;
			போது(leaf){
				எனில்(leaf->context==context){
#ifdef WITH_SYS_மரம்
					db.shared_subscription_மொத்தம்--;
#endif
					sub__remove_shared_leaf(context->subs[i]->hier, context->subs[i]->shared, leaf);
					நிறுத்து;
				}
				leaf = leaf->next;
			}
		}இல்லையெனில்{
			leaf = hier->subs;
			போது(leaf){
				எனில்(leaf->context==context){
#ifdef WITH_SYS_மரம்
					db.subscription_மொத்தம்--;
#endif
					DL_DELETE(hier->subs, leaf);
					நிறுத்து;
				}
				leaf = leaf->next;
			}
		}
		mosquitto_FREE(context->subs[i]);

		எனில்(hier->subs == வெற்று
				&& hier->children == வெற்று
				&& hier->shared == வெற்று
				&& hier->parent){

			செய்{
				hier = tmp_remove_subs(hier);
			}போது(hier);
		}
	}
	mosquitto_FREE(context->subs);
	context->subs_capacity = 0;
	context->subs_மொத்தம் = 0;

	திரும்பு MOSQ_ERR_SUCCESS;
}


வெறுமை sub_மரம்_print(கட்டமைப்பு mosquitto__subhier *வேர், முழு level)
{
	முழு i;
	கட்டமைப்பு mosquitto__subhier *branch, *branch_tmp;
	கட்டமைப்பு mosquitto__subleaf *leaf;

	HASH_ITER(hh, வேர், branch, branch_tmp){
		எனில்(level > -1){
			க்கு(i=0; i<(level+2)*2; i++){
				printf(" ");
			}
			printf("%s", branch->topic);
			leaf = branch->subs;
			போது(leaf){
				எனில்(leaf->context){
					printf(" (%s, %d)", leaf->context->id, MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}இல்லையெனில்{
					printf(" (%s, %d)", "", MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}
				leaf = leaf->next;
			}
			printf("\n");
		}

		sub_மரம்_print(branch->children, level+1);
	}
}


முழு sub__init(வெறுமை)
{
	HASH_மதிப்பு("+", 1, hashv_plus);
	HASH_மதிப்பு("#", 1, hashv_hash);

	எனில்(sub__add_hier_entry(வெற்று, &db.shared_subs, "", 0) == வெற்று
			|| sub__add_hier_entry(வெற்று, &db.normal_subs, "", 0) == வெற்று
			|| sub__add_hier_entry(வெற்று, &db.normal_subs, "$SYS", (uint16_t)strlen("$SYS")) == வெற்று
			){

		திரும்பு MOSQ_ERR_NOMEM;
	}இல்லையெனில்{
		திரும்பு MOSQ_ERR_SUCCESS;
	}
}
