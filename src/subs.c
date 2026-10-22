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

#include "config.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "mosquitto_broker_internal.h"
#include "mosquitto/mqtt_protocol.h"
#include "util_mosq.h"

#include "utlist.h"

static sa_nracanaa mosquitto__subhier *sub__add_hier_entry(sa_nracanaa mosquitto__subhier *parent, sa_nracanaa mosquitto__subhier **sibling, const var_na *topic, uint16_t len);

static unsigned ຈຳນວນເຕັມ hashv_plus = 0;
static unsigned ຈຳນວນເຕັມ hashv_hash = 0;


static ຈຳນວນເຕັມ subs__send(sa_nracanaa mosquitto__subleaf *leaf, const var_na *topic, uint8_t qos, ຈຳນວນເຕັມ retain, sa_nracanaa mosquitto__base_msg *stored)
{
	bool client_retain;
	uint16_t mid;
	uint8_t client_qos, msg_qos;
	ຈຳນວນເຕັມ rc2;

	/* Check for ACL topic access. */
	rc2 = mosquitto_acl_check(leaf->context, topic, stored->data.payloadlen, stored->data.payload, stored->data.qos, stored->data.retain, stored->data.properties, MOSQ_ACL_READ);
	ຖ້າ(rc2 == MOSQ_ERR_ACL_DENIED){
		ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
	}ບໍ່ດັ່ງນັ້ນ ຖ້າ(rc2 == MOSQ_ERR_SUCCESS){
		client_qos = MQTT_SUB_OPT_GET_QOS(leaf->subscription_options);

		ຖ້າ(db.config->upgrade_outgoing_qos){
			msg_qos = client_qos;
		}ບໍ່ດັ່ງນັ້ນ{
			ຖ້າ(qos > client_qos){
				msg_qos = client_qos;
			}ບໍ່ດັ່ງນັ້ນ{
				msg_qos = qos;
			}
		}
		ຖ້າ(msg_qos){
			mid = mosquitto__mid_generate(leaf->context);
		}ບໍ່ດັ່ງນັ້ນ{
			mid = 0;
		}
		ຖ້າ(MQTT_SUB_OPT_GET_RETAIN_AS_PUBLISHED(leaf->subscription_options)){
			client_retain = retain;
		}ບໍ່ດັ່ງນັ້ນ{
			client_retain = false;
		}
		ຖ້າ(db__message_insert_outgoing(leaf->context, 0, mid, msg_qos, client_retain, stored, leaf->identifier, true, true) == 1){
			ສົ່ງຄືນ 1;
		}
	}ບໍ່ດັ່ງນັ້ນ{
		ສົ່ງຄືນ 1; /* Application error */
	}
	ສົ່ງຄືນ 0;
}


static ຈຳນວນເຕັມ subs__shared_process(sa_nracanaa mosquitto__subhier *hier, const var_na *topic, uint8_t qos, ຈຳນວນເຕັມ retain, sa_nracanaa mosquitto__base_msg *stored)
{
	ຈຳນວນເຕັມ rc = 0, rc2;
	sa_nracanaa mosquitto__subshared *shared, *shared_tmp;
	sa_nracanaa mosquitto__subleaf *leaf;

	HASH_ITER(hh, hier->shared, shared, shared_tmp){
		leaf = shared->subs;
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		/* Remove current from the top, add back to the bottom */
		DL_DELETE(shared->subs, leaf);
		DL_APPEND(shared->subs, leaf);

		ຖ້າ(rc2){
			rc = 1;
		}
	}

	ສົ່ງຄືນ rc;
}


static ຈຳນວນເຕັມ subs__process(sa_nracanaa mosquitto__subhier *hier, const var_na *source_id, const var_na *topic, uint8_t qos, ຈຳນວນເຕັມ retain, sa_nracanaa mosquitto__base_msg *stored)
{
	ຈຳນວນເຕັມ rc = 0;
	ຈຳນວນເຕັມ rc2;
	sa_nracanaa mosquitto__subleaf *leaf;

	rc = subs__shared_process(hier, topic, qos, retain, stored);

	leaf = hier->subs;
	ໃນຂະນະທີ່(source_id && leaf){
		ຖ້າ(!leaf->context->id || (MQTT_SUB_OPT_GET_NO_LOCAL(leaf->subscription_options) && !strcmp(leaf->context->id, source_id))){
			leaf = leaf->next;
			continue;
		}
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		ຖ້າ(rc2){
			rc = 1;
		}
		leaf = leaf->next;
	}
	ຖ້າ(hier->subs || hier->shared){
		ສົ່ງຄືນ rc;
	}ບໍ່ດັ່ງນັ້ນ{
		ສົ່ງຄືນ MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


static ຈຳນວນເຕັມ sub__add_leaf(sa_nracanaa mosquitto *context, const sa_nracanaa mosquitto_subscription *sub, sa_nracanaa mosquitto__subleaf **head, sa_nracanaa mosquitto__subleaf **newleaf)
{
	sa_nracanaa mosquitto__subleaf *leaf;

	*newleaf = NULL;
	leaf = *head;

	ໃນຂະນະທີ່(leaf){
		ຖ້າ(leaf->context && leaf->context->id && !strcmp(leaf->context->id, context->id)){
			/* Client making a second subscription to same topic. Only
			 * need to update QoS. Return MOSQ_ERR_SUB_EXISTS to
			 * indicate this to the calling function. */
			leaf->identifier = sub->identifier;
			leaf->subscription_options = sub->options;
			ສົ່ງຄືນ MOSQ_ERR_SUB_EXISTS;
		}
		leaf = leaf->next;
	}
	leaf = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__subleaf) + strlen(sub->topic_filter) + 1);
	ຖ້າ(!leaf){
		ສົ່ງຄືນ MOSQ_ERR_NOMEM;
	}
	leaf->context = context;
	leaf->identifier = sub->identifier;
	leaf->subscription_options = sub->options;
	strcpy(leaf->topic_filter, sub->topic_filter);

	DL_APPEND(*head, leaf);
	*newleaf = leaf;

	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}


static void sub__remove_shared_leaf(sa_nracanaa mosquitto__subhier *subhier, sa_nracanaa mosquitto__subshared *shared, sa_nracanaa mosquitto__subleaf *leaf)
{
	DL_DELETE(shared->subs, leaf);
	ຖ້າ(shared->subs == NULL){
		HASH_DELETE(hh, subhier->shared, shared);
		mosquitto_FREE(shared);
	}
}


static ຈຳນວນເຕັມ sub__add_shared(sa_nracanaa mosquitto *context, const sa_nracanaa mosquitto_subscription *sub, sa_nracanaa mosquitto__subhier *subhier, const var_na *sharename)
{
	sa_nracanaa mosquitto__subleaf *newleaf;
	sa_nracanaa mosquitto__subshared *shared = NULL;
	sa_nracanaa mosquitto__subleaf **subs;
	size_t slen;
	ຈຳນວນເຕັມ rc;
	unsigned hashv;

	slen = strlen(sharename);

	HASH_VALUE(sharename, slen, hashv);

	HASH_FIND_BYHASHVALUE(hh, subhier->shared, sharename, slen, hashv, shared);
	ຖ້າ(shared == NULL){
		shared = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__subshared) + slen + 1);
		ຖ້າ(!shared){
			ສົ່ງຄືນ MOSQ_ERR_NOMEM;
		}
		strncpy(shared->name, sharename, slen+1);

		HASH_ADD_BYHASHVALUE(hh, subhier->shared, name, slen, hashv, shared);
	}

	rc = sub__add_leaf(context, sub, &shared->subs, &newleaf);
	ຖ້າ(rc > 0){
		ຖ້າ(shared->subs == NULL){
			HASH_DELETE(hh, subhier->shared, shared);
			mosquitto_FREE(shared);
		}
		ສົ່ງຄືນ rc;
	}

	ຖ້າ(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = shared;

		bool assigned = false;
		ສຳລັບ(ຈຳນວນເຕັມ i=0; i<context->subs_capacity; i++){
			ຖ້າ(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_count++;
				assigned = true;
				bha_nga;
			}
		}
		ຖ້າ(assigned == false){
			subs = mosquitto_realloc(context->subs, parimaa_na(sa_nracanaa mosquitto__subleaf *)*(size_t)(context->subs_capacity + 1));
			ຖ້າ(!subs){
				sub__remove_shared_leaf(subhier, shared, newleaf);
				mosquitto_FREE(newleaf);
				ສົ່ງຄືນ MOSQ_ERR_NOMEM;
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

	ຖ້າ(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		ສົ່ງຄືນ rc;
	}ບໍ່ດັ່ງນັ້ນ{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
	}
}


static ຈຳນວນເຕັມ sub__add_normal(sa_nracanaa mosquitto *context, const sa_nracanaa mosquitto_subscription *sub, sa_nracanaa mosquitto__subhier *subhier)
{
	sa_nracanaa mosquitto__subleaf *newleaf = NULL;
	sa_nracanaa mosquitto__subleaf **subs;
	ຈຳນວນເຕັມ rc;

	rc = sub__add_leaf(context, sub, &subhier->subs, &newleaf);
	ຖ້າ(rc > 0){
		ສົ່ງຄືນ rc;
	}

	ຖ້າ(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = NULL;

		bool assigned = false;
		ສຳລັບ(ຈຳນວນເຕັມ i=0; i<context->subs_capacity; i++){
			ຖ້າ(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_count++;
				assigned = true;
				bha_nga;
			}
		}
		ຖ້າ(assigned == false){
			subs = mosquitto_realloc(context->subs, parimaa_na(sa_nracanaa mosquitto__subleaf *)*(size_t)(context->subs_capacity + 1));
			ຖ້າ(!subs){
				DL_DELETE(subhier->subs, newleaf);
				mosquitto_FREE(newleaf);
				ສົ່ງຄືນ MOSQ_ERR_NOMEM;
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

	ຖ້າ(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		ສົ່ງຄືນ rc;
	}ບໍ່ດັ່ງນັ້ນ{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
	}
}


static ຈຳນວນເຕັມ sub__add_context(sa_nracanaa mosquitto *context, const sa_nracanaa mosquitto_subscription *sub, sa_nracanaa mosquitto__subhier *subhier, var_na *const *const topics, const var_na *sharename)
{
	sa_nracanaa mosquitto__subhier *branch;
	ຈຳນວນເຕັມ topic_index = 0;
	size_t topiclen;

	/* Find leaf node */
	ໃນຂະນະທີ່(topics && topics[topic_index] != NULL){
		topiclen = strlen(topics[topic_index]);
		ຖ້າ(topiclen > UINT16_MAX){
			ສົ່ງຄືນ MOSQ_ERR_INVAL;
		}
		HASH_FIND(hh, subhier->children, topics[topic_index], topiclen, branch);
		ຖ້າ(!branch){
			/* Not found */
			branch = sub__add_hier_entry(subhier, &subhier->children, topics[topic_index], (uint16_t)topiclen);
			ຖ້າ(!branch){
				ສົ່ງຄືນ MOSQ_ERR_NOMEM;
			}
		}
		subhier = branch;
		topic_index++;
	}

	/* Add add our context */
	ຖ້າ(context && context->id){
		ຖ້າ(sharename){
			ສົ່ງຄືນ sub__add_shared(context, sub, subhier, sharename);
		}ບໍ່ດັ່ງນັ້ນ{
			ສົ່ງຄືນ sub__add_normal(context, sub, subhier);
		}
	}ບໍ່ດັ່ງນັ້ນ{
		ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
	}
}


static ຈຳນວນເຕັມ sub__remove_normal(sa_nracanaa mosquitto *context, sa_nracanaa mosquitto__subhier *subhier, uint8_t *reason)
{
	sa_nracanaa mosquitto__subleaf *leaf;

	leaf = subhier->subs;
	ໃນຂະນະທີ່(leaf){
		ຖ້າ(leaf->context==context){
#ifdef WITH_SYS_TREE
			db.subscription_count--;
#endif
			DL_DELETE(subhier->subs, leaf);

			/* Remove the reference to the sub that the client is keeping.
			 * It would be nice to be able to use the reference directly,
			 * but that would involve keeping a copy of the topic string in
			 * each subleaf. Might be worth considering though. */
			ສຳລັບ(ຈຳນວນເຕັມ i=0; i<context->subs_capacity; i++){
				ຖ້າ(context->subs[i] && context->subs[i]->hier == subhier){
					context->subs_count--;
					mosquitto_free(context->subs[i]);
					context->subs[i] = NULL;
					bha_nga;
				}
			}
			*reason = 0;
			ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
		}
		leaf = leaf->next;
	}
	ສົ່ງຄືນ MOSQ_ERR_NO_SUBSCRIBERS;
}


static ຈຳນວນເຕັມ sub__remove_shared(sa_nracanaa mosquitto *context, sa_nracanaa mosquitto__subhier *subhier, uint8_t *reason, const var_na *sharename)
{
	sa_nracanaa mosquitto__subshared *shared;

	HASH_FIND(hh, subhier->shared, sharename, strlen(sharename), shared);
	ຖ້າ(shared){
		sa_nracanaa mosquitto__subleaf *leaf = shared->subs;
		ໃນຂະນະທີ່(leaf){
			ຖ້າ(leaf->context==context){
#ifdef WITH_SYS_TREE
				db.shared_subscription_count--;
#endif
				DL_DELETE(shared->subs, leaf);

				/* Remove the reference to the sub that the client is keeping.
				* It would be nice to be able to use the reference directly,
				* but that would involve keeping a copy of the topic string in
				* each subleaf. Might be worth considering though. */
				ສຳລັບ(ຈຳນວນເຕັມ i=0; i<context->subs_capacity; i++){
					ຖ້າ(context->subs[i]
							&& context->subs[i]->hier == subhier
							&& context->subs[i]->shared == shared){

						mosquitto_free(context->subs[i]);
						context->subs[i] = NULL;
						context->subs_count--;
						bha_nga;
					}
				}

				ຖ້າ(shared->subs == NULL){
					HASH_DELETE(hh, subhier->shared, shared);
					mosquitto_FREE(shared);
				}

				*reason = 0;
				ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
			}
			leaf = leaf->next;
		}
		ສົ່ງຄືນ MOSQ_ERR_NO_SUBSCRIBERS;
	}ບໍ່ດັ່ງນັ້ນ{
		ສົ່ງຄືນ MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


static ຈຳນວນເຕັມ sub__remove_recurse(sa_nracanaa mosquitto *context, sa_nracanaa mosquitto__subhier *subhier, var_na **topics, uint8_t *reason, const var_na *sharename)
{
	sa_nracanaa mosquitto__subhier *branch;

	ຖ້າ(topics == NULL || topics[0] == NULL){
		ຖ້າ(sharename){
			ສົ່ງຄືນ sub__remove_shared(context, subhier, reason, sharename);
		}ບໍ່ດັ່ງນັ້ນ{
			ສົ່ງຄືນ sub__remove_normal(context, subhier, reason);
		}
	}

	HASH_FIND(hh, subhier->children, topics[0], strlen(topics[0]), branch);
	ຖ້າ(branch){
		sub__remove_recurse(context, branch, &(topics[1]), reason, sharename);
		ຖ້າ(!branch->children && !branch->subs && !branch->shared){
			HASH_DELETE(hh, subhier->children, branch);
			mosquitto_FREE(branch);
		}
	}
	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}


static ຈຳນວນເຕັມ sub__search(sa_nracanaa mosquitto__subhier *subhier, var_na **split_topics, const var_na *source_id, const var_na *topic, uint8_t qos, ຈຳນວນເຕັມ retain, sa_nracanaa mosquitto__base_msg *stored)
{
	/* FIXME - need to take into account source_id if the client is a bridge */
	sa_nracanaa mosquitto__subhier *branch;
	ຈຳນວນເຕັມ rc;
	bool have_subscribers = false;

	ຖ້າ(split_topics && split_topics[0]){
		/* Check for literal match */
		HASH_FIND(hh, subhier->children, split_topics[0], strlen(split_topics[0]), branch);

		ຖ້າ(branch){
			rc = sub__search(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			ຖ້າ(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}ບໍ່ດັ່ງນັ້ນ ຖ້າ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				ສົ່ງຄືນ rc;
			}
			ຖ້າ(split_topics[1] == NULL){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				ຖ້າ(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}ບໍ່ດັ່ງນັ້ນ ຖ້າ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					ສົ່ງຄືນ rc;
				}
			}
		}

		/* Check for + match */
		HASH_FIND_BYHASHVALUE(hh, subhier->children, "+", 1, hashv_plus, branch);

		ຖ້າ(branch){
			rc = sub__search(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			ຖ້າ(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}ບໍ່ດັ່ງນັ້ນ ຖ້າ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				ສົ່ງຄືນ rc;
			}
			ຖ້າ(split_topics[1] == NULL){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				ຖ້າ(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}ບໍ່ດັ່ງນັ້ນ ຖ້າ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					ສົ່ງຄືນ rc;
				}
			}
		}
	}

	/* Check for # match */
	HASH_FIND_BYHASHVALUE(hh, subhier->children, "#", 1, hashv_hash, branch);
	ຖ້າ(branch && !branch->children){
		/* The topic matches due to a # wildcard - process the
		 * subscriptions but *don't* return. Although this branch has ended
		 * there may still be other subscriptions to deal with.
		 */
		rc = subs__process(branch, source_id, topic, qos, retain, stored);
		ຖ້າ(rc == MOSQ_ERR_SUCCESS){
			have_subscribers = true;
		}ບໍ່ດັ່ງນັ້ນ ຖ້າ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
			ສົ່ງຄືນ rc;
		}
	}

	ຖ້າ(have_subscribers){
		ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
	}ບໍ່ດັ່ງນັ້ນ{
		ສົ່ງຄືນ MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


static sa_nracanaa mosquitto__subhier *sub__add_hier_entry(sa_nracanaa mosquitto__subhier *parent, sa_nracanaa mosquitto__subhier **sibling, const var_na *topic, uint16_t len)
{
	sa_nracanaa mosquitto__subhier *child;

	assert(sibling);

	child = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__subhier) + len + 1);
	ຖ້າ(!child){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		ສົ່ງຄືນ NULL;
	}
	child->parent = parent;
	child->topic_len = len;
	ຖ້າ(len > 0){
		strncpy(child->topic, topic, (size_t)(len+1));
	}

	HASH_ADD(hh, *sibling, topic, child->topic_len, child);

	ສົ່ງຄືນ child;
}


ຈຳນວນເຕັມ sub__add(sa_nracanaa mosquitto *context, const sa_nracanaa mosquitto_subscription *sub)
{
	ຈຳນວນເຕັມ rc = 0;
	sa_nracanaa mosquitto__subhier *subhier;
	const var_na *sharename = NULL;
	var_na *local_sub;
	var_na **topics;
	size_t topiclen;

	assert(sub);
	assert(sub->topic_filter);

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &topics, &sharename);
	ຖ້າ(rc){
		ສົ່ງຄືນ rc;
	}

	topiclen = strlen(topics[0]);
	ຖ້າ(topiclen > UINT16_MAX){
		mosquitto_FREE(local_sub);
		mosquitto_FREE(topics);
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}

	ຖ້າ(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], topiclen, subhier);
		ຖ້າ(!subhier){
			subhier = sub__add_hier_entry(NULL, &db.shared_subs, topics[0], (uint16_t)topiclen);
			ຖ້າ(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
				ສົ່ງຄືນ MOSQ_ERR_NOMEM;
			}
		}
	}ບໍ່ດັ່ງນັ້ນ{
		HASH_FIND(hh, db.normal_subs, topics[0], topiclen, subhier);
		ຖ້າ(!subhier){
			subhier = sub__add_hier_entry(NULL, &db.normal_subs, topics[0], (uint16_t)topiclen);
			ຖ້າ(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
				ສົ່ງຄືນ MOSQ_ERR_NOMEM;
			}
		}
	}
	rc = sub__add_context(context, sub, subhier, topics, sharename);

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	ສົ່ງຄືນ rc;
}


ຈຳນວນເຕັມ sub__remove(sa_nracanaa mosquitto *context, const var_na *sub, uint8_t *reason)
{
	ຈຳນວນເຕັມ rc = 0;
	sa_nracanaa mosquitto__subhier *subhier;
	const var_na *sharename = NULL;
	var_na *local_sub = NULL;
	var_na **topics = NULL;

	assert(sub);

	rc = sub__topic_tokenise(sub, &local_sub, &topics, &sharename);
	ຖ້າ(rc){
		ສົ່ງຄືນ rc;
	}

	ຖ້າ(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], strlen(topics[0]), subhier);
	}ບໍ່ດັ່ງນັ້ນ{
		HASH_FIND(hh, db.normal_subs, topics[0], strlen(topics[0]), subhier);
	}
	ຖ້າ(subhier){
		*reason = MQTT_RC_NO_SUBSCRIPTION_EXISTED;
		rc = sub__remove_recurse(context, subhier, topics, reason, sharename);
	}

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	ສົ່ງຄືນ rc;
}


ຈຳນວນເຕັມ sub__messages_queue(const var_na *source_id, uint8_t qos, sa_nracanaa mosquitto__base_msg **stored)
{
	ຈຳນວນເຕັມ rc = MOSQ_ERR_SUCCESS, rc2;
	ຈຳນວນເຕັມ rc_normal = MOSQ_ERR_NO_SUBSCRIBERS, rc_shared = MOSQ_ERR_NO_SUBSCRIBERS;
	sa_nracanaa mosquitto__subhier *subhier;
	var_na **split_topics = NULL;
	var_na *local_topic = NULL;
	unsigned hashv;
	size_t topiclen;
	bool retain = (*stored)->data.retain;
	const var_na *topic = (*stored)->data.topic;

	assert(topic);

	ຖ້າ(sub__topic_tokenise(topic, &local_topic, &split_topics, NULL)){
		ສົ່ງຄືນ 1;
	}

	/* Protect this message until we have sent it to all
	clients - this is required because websockets client calls
	db__message_write(), which could remove the message if ref_count==0.
	*/
	db__msg_store_ref_inc(*stored);

	topiclen = strlen(split_topics[0]);
	HASH_VALUE(split_topics[0], topiclen, hashv);
	HASH_FIND_BYHASHVALUE(hh, db.normal_subs, split_topics[0], topiclen, hashv, subhier);
	ຖ້າ(subhier){
		rc_normal = sub__search(subhier, split_topics, source_id, topic, qos, retain, *stored);
		ຖ້າ(rc_normal > 0){
			rc = rc_normal;
			goto end;
		}
	}

	HASH_FIND_BYHASHVALUE(hh, db.shared_subs, split_topics[0], topiclen, hashv, subhier);
	ຖ້າ(subhier){
		rc_shared = sub__search(subhier, split_topics, source_id, topic, qos, retain, *stored);
		ຖ້າ(rc_shared > 0){
			rc = rc_shared;
			goto end;
		}
	}

	ຖ້າ(rc_normal == MOSQ_ERR_NO_SUBSCRIBERS && rc_shared == MOSQ_ERR_NO_SUBSCRIBERS){
		rc = MOSQ_ERR_NO_SUBSCRIBERS;
	}

	ຖ້າ(retain){
		rc2 = retain__store(topic, *stored, split_topics, true);
		ຖ້າ(rc2){
			rc = rc2;
		}
	}

end:
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);
	/* Remove our reference and free if needed. */
	db__msg_store_ref_dec(stored);

	ສົ່ງຄືນ rc;
}


/* Remove a subhier element, and return its parent if that needs freeing as well. */
static sa_nracanaa mosquitto__subhier *tmp_remove_subs(sa_nracanaa mosquitto__subhier *sub)
{
	sa_nracanaa mosquitto__subhier *parent;

	ຖ້າ(!sub || !sub->parent){
		ສົ່ງຄືນ NULL;
	}

	ຖ້າ(sub->children || sub->subs){
		ສົ່ງຄືນ NULL;
	}

	parent = sub->parent;
	HASH_DELETE(hh, parent->children, sub);
	mosquitto_FREE(sub);

	ຖ້າ(parent->subs == NULL
			&& parent->children == NULL
			&& parent->shared == NULL
			&& parent->parent){

		ສົ່ງຄືນ parent;
	}ບໍ່ດັ່ງນັ້ນ{
		ສົ່ງຄືນ NULL;
	}
}


/* Remove all subscriptions for a client.
 */
ຈຳນວນເຕັມ sub__clean_session(sa_nracanaa mosquitto *context)
{
	ສຳລັບ(ຈຳນວນເຕັມ i=0; i<context->subs_capacity; i++){
		ຖ້າ(context->subs[i] == NULL || context->subs[i]->hier == NULL){
			continue;
		}

		sa_nracanaa mosquitto__subhier *hier = context->subs[i]->hier;
		sa_nracanaa mosquitto__subleaf *leaf;

		plugin_persist__handle_subscription_delete(context, context->subs[i]->topic_filter);
		ຖ້າ(context->subs[i]->shared){
			leaf = context->subs[i]->shared->subs;
			ໃນຂະນະທີ່(leaf){
				ຖ້າ(leaf->context==context){
#ifdef WITH_SYS_TREE
					db.shared_subscription_count--;
#endif
					sub__remove_shared_leaf(context->subs[i]->hier, context->subs[i]->shared, leaf);
					bha_nga;
				}
				leaf = leaf->next;
			}
		}ບໍ່ດັ່ງນັ້ນ{
			leaf = hier->subs;
			ໃນຂະນະທີ່(leaf){
				ຖ້າ(leaf->context==context){
#ifdef WITH_SYS_TREE
					db.subscription_count--;
#endif
					DL_DELETE(hier->subs, leaf);
					bha_nga;
				}
				leaf = leaf->next;
			}
		}
		mosquitto_FREE(context->subs[i]);

		ຖ້າ(hier->subs == NULL
				&& hier->children == NULL
				&& hier->shared == NULL
				&& hier->parent){

			do{
				hier = tmp_remove_subs(hier);
			}ໃນຂະນະທີ່(hier);
		}
	}
	mosquitto_FREE(context->subs);
	context->subs_capacity = 0;
	context->subs_count = 0;

	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}


void sub__tree_print(sa_nracanaa mosquitto__subhier *root, ຈຳນວນເຕັມ level)
{
	ຈຳນວນເຕັມ i;
	sa_nracanaa mosquitto__subhier *branch, *branch_tmp;
	sa_nracanaa mosquitto__subleaf *leaf;

	HASH_ITER(hh, root, branch, branch_tmp){
		ຖ້າ(level > -1){
			ສຳລັບ(i=0; i<(level+2)*2; i++){
				printf(" ");
			}
			printf("%s", branch->topic);
			leaf = branch->subs;
			ໃນຂະນະທີ່(leaf){
				ຖ້າ(leaf->context){
					printf(" (%s, %d)", leaf->context->id, MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}ບໍ່ດັ່ງນັ້ນ{
					printf(" (%s, %d)", "", MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}
				leaf = leaf->next;
			}
			printf("\n");
		}

		sub__tree_print(branch->children, level+1);
	}
}


ຈຳນວນເຕັມ sub__init(void)
{
	HASH_VALUE("+", 1, hashv_plus);
	HASH_VALUE("#", 1, hashv_hash);

	ຖ້າ(sub__add_hier_entry(NULL, &db.shared_subs, "", 0) == NULL
			|| sub__add_hier_entry(NULL, &db.normal_subs, "", 0) == NULL
			|| sub__add_hier_entry(NULL, &db.normal_subs, "$SYS", (uint16_t)strlen("$SYS")) == NULL
			){

		ສົ່ງຄືນ MOSQ_ERR_NOMEM;
	}ບໍ່ດັ່ງນັ້ນ{
		ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
	}
}
