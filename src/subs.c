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

static struct mosquitto__subhier *sub__add_hier_entry(struct mosquitto__subhier *parent, struct mosquitto__subhier **sibling, const char *topic, uint16_t len);

static unsigned 정수 hashv_plus = 0;
static unsigned 정수 hashv_hash = 0;


static 정수 subs__send(struct mosquitto__subleaf *leaf, const char *topic, uint8_t qos, 정수 retain, struct mosquitto__base_msg *stored)
{
	bool client_retain;
	uint16_t mid;
	uint8_t client_qos, msg_qos;
	정수 rc2;

	/* Check for ACL topic access. */
	rc2 = mosquitto_acl_check(leaf->context, topic, stored->data.payloadlen, stored->data.payload, stored->data.qos, stored->data.retain, stored->data.properties, MOSQ_ACL_READ);
	만약(rc2 == MOSQ_ERR_ACL_DENIED){
		반환 MOSQ_ERR_SUCCESS;
	}아니면 만약(rc2 == MOSQ_ERR_SUCCESS){
		client_qos = MQTT_SUB_OPT_GET_QOS(leaf->subscription_options);

		만약(db.config->upgrade_outgoing_qos){
			msg_qos = client_qos;
		}아니면{
			만약(qos > client_qos){
				msg_qos = client_qos;
			}아니면{
				msg_qos = qos;
			}
		}
		만약(msg_qos){
			mid = mosquitto__mid_generate(leaf->context);
		}아니면{
			mid = 0;
		}
		만약(MQTT_SUB_OPT_GET_RETAIN_AS_PUBLISHED(leaf->subscription_options)){
			client_retain = retain;
		}아니면{
			client_retain = false;
		}
		만약(db__message_insert_outgoing(leaf->context, 0, mid, msg_qos, client_retain, stored, leaf->identifier, true, true) == 1){
			반환 1;
		}
	}아니면{
		반환 1; /* Application error */
	}
	반환 0;
}


static 정수 subs__shared_process(struct mosquitto__subhier *hier, const char *topic, uint8_t qos, 정수 retain, struct mosquitto__base_msg *stored)
{
	정수 rc = 0, rc2;
	struct mosquitto__subshared *shared, *shared_tmp;
	struct mosquitto__subleaf *leaf;

	HASH_ITER(hh, hier->shared, shared, shared_tmp){
		leaf = shared->subs;
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		/* Remove current from the top, add back to the bottom */
		DL_DELETE(shared->subs, leaf);
		DL_APPEND(shared->subs, leaf);

		만약(rc2){
			rc = 1;
		}
	}

	반환 rc;
}


static 정수 subs__process(struct mosquitto__subhier *hier, const char *source_id, const char *topic, uint8_t qos, 정수 retain, struct mosquitto__base_msg *stored)
{
	정수 rc = 0;
	정수 rc2;
	struct mosquitto__subleaf *leaf;

	rc = subs__shared_process(hier, topic, qos, retain, stored);

	leaf = hier->subs;
	동안(source_id && leaf){
		만약(!leaf->context->id || (MQTT_SUB_OPT_GET_NO_LOCAL(leaf->subscription_options) && !strcmp(leaf->context->id, source_id))){
			leaf = leaf->next;
			continue;
		}
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		만약(rc2){
			rc = 1;
		}
		leaf = leaf->next;
	}
	만약(hier->subs || hier->shared){
		반환 rc;
	}아니면{
		반환 MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


static 정수 sub__add_leaf(struct mosquitto *context, const struct mosquitto_subscription *sub, struct mosquitto__subleaf **head, struct mosquitto__subleaf **newleaf)
{
	struct mosquitto__subleaf *leaf;

	*newleaf = NULL;
	leaf = *head;

	동안(leaf){
		만약(leaf->context && leaf->context->id && !strcmp(leaf->context->id, context->id)){
			/* Client making a second subscription to same topic. Only
			 * need to update QoS. Return MOSQ_ERR_SUB_EXISTS to
			 * indicate this to the calling function. */
			leaf->identifier = sub->identifier;
			leaf->subscription_options = sub->options;
			반환 MOSQ_ERR_SUB_EXISTS;
		}
		leaf = leaf->next;
	}
	leaf = mosquitto_calloc(1, sizeof(struct mosquitto__subleaf) + strlen(sub->topic_filter) + 1);
	만약(!leaf){
		반환 MOSQ_ERR_NOMEM;
	}
	leaf->context = context;
	leaf->identifier = sub->identifier;
	leaf->subscription_options = sub->options;
	strcpy(leaf->topic_filter, sub->topic_filter);

	DL_APPEND(*head, leaf);
	*newleaf = leaf;

	반환 MOSQ_ERR_SUCCESS;
}


static void sub__remove_shared_leaf(struct mosquitto__subhier *subhier, struct mosquitto__subshared *shared, struct mosquitto__subleaf *leaf)
{
	DL_DELETE(shared->subs, leaf);
	만약(shared->subs == NULL){
		HASH_DELETE(hh, subhier->shared, shared);
		mosquitto_FREE(shared);
	}
}


static 정수 sub__add_shared(struct mosquitto *context, const struct mosquitto_subscription *sub, struct mosquitto__subhier *subhier, const char *sharename)
{
	struct mosquitto__subleaf *newleaf;
	struct mosquitto__subshared *shared = NULL;
	struct mosquitto__subleaf **subs;
	size_t slen;
	정수 rc;
	unsigned hashv;

	slen = strlen(sharename);

	HASH_VALUE(sharename, slen, hashv);

	HASH_FIND_BYHASHVALUE(hh, subhier->shared, sharename, slen, hashv, shared);
	만약(shared == NULL){
		shared = mosquitto_calloc(1, sizeof(struct mosquitto__subshared) + slen + 1);
		만약(!shared){
			반환 MOSQ_ERR_NOMEM;
		}
		strncpy(shared->name, sharename, slen+1);

		HASH_ADD_BYHASHVALUE(hh, subhier->shared, name, slen, hashv, shared);
	}

	rc = sub__add_leaf(context, sub, &shared->subs, &newleaf);
	만약(rc > 0){
		만약(shared->subs == NULL){
			HASH_DELETE(hh, subhier->shared, shared);
			mosquitto_FREE(shared);
		}
		반환 rc;
	}

	만약(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = shared;

		bool assigned = false;
		위해(정수 i=0; i<context->subs_capacity; i++){
			만약(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_count++;
				assigned = true;
				break;
			}
		}
		만약(assigned == false){
			subs = mosquitto_realloc(context->subs, sizeof(struct mosquitto__subleaf *)*(size_t)(context->subs_capacity + 1));
			만약(!subs){
				sub__remove_shared_leaf(subhier, shared, newleaf);
				mosquitto_FREE(newleaf);
				반환 MOSQ_ERR_NOMEM;
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

	만약(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		반환 rc;
	}아니면{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		반환 MOSQ_ERR_SUCCESS;
	}
}


static 정수 sub__add_normal(struct mosquitto *context, const struct mosquitto_subscription *sub, struct mosquitto__subhier *subhier)
{
	struct mosquitto__subleaf *newleaf = NULL;
	struct mosquitto__subleaf **subs;
	정수 rc;

	rc = sub__add_leaf(context, sub, &subhier->subs, &newleaf);
	만약(rc > 0){
		반환 rc;
	}

	만약(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = NULL;

		bool assigned = false;
		위해(정수 i=0; i<context->subs_capacity; i++){
			만약(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_count++;
				assigned = true;
				break;
			}
		}
		만약(assigned == false){
			subs = mosquitto_realloc(context->subs, sizeof(struct mosquitto__subleaf *)*(size_t)(context->subs_capacity + 1));
			만약(!subs){
				DL_DELETE(subhier->subs, newleaf);
				mosquitto_FREE(newleaf);
				반환 MOSQ_ERR_NOMEM;
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

	만약(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		반환 rc;
	}아니면{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		반환 MOSQ_ERR_SUCCESS;
	}
}


static 정수 sub__add_context(struct mosquitto *context, const struct mosquitto_subscription *sub, struct mosquitto__subhier *subhier, char *const *const topics, const char *sharename)
{
	struct mosquitto__subhier *branch;
	정수 topic_index = 0;
	size_t topiclen;

	/* Find leaf node */
	동안(topics && topics[topic_index] != NULL){
		topiclen = strlen(topics[topic_index]);
		만약(topiclen > UINT16_MAX){
			반환 MOSQ_ERR_INVAL;
		}
		HASH_FIND(hh, subhier->children, topics[topic_index], topiclen, branch);
		만약(!branch){
			/* Not found */
			branch = sub__add_hier_entry(subhier, &subhier->children, topics[topic_index], (uint16_t)topiclen);
			만약(!branch){
				반환 MOSQ_ERR_NOMEM;
			}
		}
		subhier = branch;
		topic_index++;
	}

	/* Add add our context */
	만약(context && context->id){
		만약(sharename){
			반환 sub__add_shared(context, sub, subhier, sharename);
		}아니면{
			반환 sub__add_normal(context, sub, subhier);
		}
	}아니면{
		반환 MOSQ_ERR_SUCCESS;
	}
}


static 정수 sub__remove_normal(struct mosquitto *context, struct mosquitto__subhier *subhier, uint8_t *reason)
{
	struct mosquitto__subleaf *leaf;

	leaf = subhier->subs;
	동안(leaf){
		만약(leaf->context==context){
#ifdef WITH_SYS_TREE
			db.subscription_count--;
#endif
			DL_DELETE(subhier->subs, leaf);

			/* Remove the reference to the sub that the client is keeping.
			 * It would be nice to be able to use the reference directly,
			 * but that would involve keeping a copy of the topic string in
			 * each subleaf. Might be worth considering though. */
			위해(정수 i=0; i<context->subs_capacity; i++){
				만약(context->subs[i] && context->subs[i]->hier == subhier){
					context->subs_count--;
					mosquitto_free(context->subs[i]);
					context->subs[i] = NULL;
					break;
				}
			}
			*reason = 0;
			반환 MOSQ_ERR_SUCCESS;
		}
		leaf = leaf->next;
	}
	반환 MOSQ_ERR_NO_SUBSCRIBERS;
}


static 정수 sub__remove_shared(struct mosquitto *context, struct mosquitto__subhier *subhier, uint8_t *reason, const char *sharename)
{
	struct mosquitto__subshared *shared;

	HASH_FIND(hh, subhier->shared, sharename, strlen(sharename), shared);
	만약(shared){
		struct mosquitto__subleaf *leaf = shared->subs;
		동안(leaf){
			만약(leaf->context==context){
#ifdef WITH_SYS_TREE
				db.shared_subscription_count--;
#endif
				DL_DELETE(shared->subs, leaf);

				/* Remove the reference to the sub that the client is keeping.
				* It would be nice to be able to use the reference directly,
				* but that would involve keeping a copy of the topic string in
				* each subleaf. Might be worth considering though. */
				위해(정수 i=0; i<context->subs_capacity; i++){
					만약(context->subs[i]
							&& context->subs[i]->hier == subhier
							&& context->subs[i]->shared == shared){

						mosquitto_free(context->subs[i]);
						context->subs[i] = NULL;
						context->subs_count--;
						break;
					}
				}

				만약(shared->subs == NULL){
					HASH_DELETE(hh, subhier->shared, shared);
					mosquitto_FREE(shared);
				}

				*reason = 0;
				반환 MOSQ_ERR_SUCCESS;
			}
			leaf = leaf->next;
		}
		반환 MOSQ_ERR_NO_SUBSCRIBERS;
	}아니면{
		반환 MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


static 정수 sub__remove_recurse(struct mosquitto *context, struct mosquitto__subhier *subhier, char **topics, uint8_t *reason, const char *sharename)
{
	struct mosquitto__subhier *branch;

	만약(topics == NULL || topics[0] == NULL){
		만약(sharename){
			반환 sub__remove_shared(context, subhier, reason, sharename);
		}아니면{
			반환 sub__remove_normal(context, subhier, reason);
		}
	}

	HASH_FIND(hh, subhier->children, topics[0], strlen(topics[0]), branch);
	만약(branch){
		sub__remove_recurse(context, branch, &(topics[1]), reason, sharename);
		만약(!branch->children && !branch->subs && !branch->shared){
			HASH_DELETE(hh, subhier->children, branch);
			mosquitto_FREE(branch);
		}
	}
	반환 MOSQ_ERR_SUCCESS;
}


static 정수 sub__search(struct mosquitto__subhier *subhier, char **split_topics, const char *source_id, const char *topic, uint8_t qos, 정수 retain, struct mosquitto__base_msg *stored)
{
	/* FIXME - need to take into account source_id if the client is a bridge */
	struct mosquitto__subhier *branch;
	정수 rc;
	bool have_subscribers = false;

	만약(split_topics && split_topics[0]){
		/* Check for literal match */
		HASH_FIND(hh, subhier->children, split_topics[0], strlen(split_topics[0]), branch);

		만약(branch){
			rc = sub__search(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			만약(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}아니면 만약(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				반환 rc;
			}
			만약(split_topics[1] == NULL){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				만약(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}아니면 만약(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					반환 rc;
				}
			}
		}

		/* Check for + match */
		HASH_FIND_BYHASHVALUE(hh, subhier->children, "+", 1, hashv_plus, branch);

		만약(branch){
			rc = sub__search(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			만약(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}아니면 만약(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				반환 rc;
			}
			만약(split_topics[1] == NULL){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				만약(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}아니면 만약(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					반환 rc;
				}
			}
		}
	}

	/* Check for # match */
	HASH_FIND_BYHASHVALUE(hh, subhier->children, "#", 1, hashv_hash, branch);
	만약(branch && !branch->children){
		/* The topic matches due to a # wildcard - process the
		 * subscriptions but *don't* return. Although this branch has ended
		 * there may still be other subscriptions to deal with.
		 */
		rc = subs__process(branch, source_id, topic, qos, retain, stored);
		만약(rc == MOSQ_ERR_SUCCESS){
			have_subscribers = true;
		}아니면 만약(rc != MOSQ_ERR_NO_SUBSCRIBERS){
			반환 rc;
		}
	}

	만약(have_subscribers){
		반환 MOSQ_ERR_SUCCESS;
	}아니면{
		반환 MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


static struct mosquitto__subhier *sub__add_hier_entry(struct mosquitto__subhier *parent, struct mosquitto__subhier **sibling, const char *topic, uint16_t len)
{
	struct mosquitto__subhier *child;

	assert(sibling);

	child = mosquitto_calloc(1, sizeof(struct mosquitto__subhier) + len + 1);
	만약(!child){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		반환 NULL;
	}
	child->parent = parent;
	child->topic_len = len;
	만약(len > 0){
		strncpy(child->topic, topic, (size_t)(len+1));
	}

	HASH_ADD(hh, *sibling, topic, child->topic_len, child);

	반환 child;
}


정수 sub__add(struct mosquitto *context, const struct mosquitto_subscription *sub)
{
	정수 rc = 0;
	struct mosquitto__subhier *subhier;
	const char *sharename = NULL;
	char *local_sub;
	char **topics;
	size_t topiclen;

	assert(sub);
	assert(sub->topic_filter);

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &topics, &sharename);
	만약(rc){
		반환 rc;
	}

	topiclen = strlen(topics[0]);
	만약(topiclen > UINT16_MAX){
		mosquitto_FREE(local_sub);
		mosquitto_FREE(topics);
		반환 MOSQ_ERR_INVAL;
	}

	만약(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], topiclen, subhier);
		만약(!subhier){
			subhier = sub__add_hier_entry(NULL, &db.shared_subs, topics[0], (uint16_t)topiclen);
			만약(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
				반환 MOSQ_ERR_NOMEM;
			}
		}
	}아니면{
		HASH_FIND(hh, db.normal_subs, topics[0], topiclen, subhier);
		만약(!subhier){
			subhier = sub__add_hier_entry(NULL, &db.normal_subs, topics[0], (uint16_t)topiclen);
			만약(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
				반환 MOSQ_ERR_NOMEM;
			}
		}
	}
	rc = sub__add_context(context, sub, subhier, topics, sharename);

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	반환 rc;
}


정수 sub__remove(struct mosquitto *context, const char *sub, uint8_t *reason)
{
	정수 rc = 0;
	struct mosquitto__subhier *subhier;
	const char *sharename = NULL;
	char *local_sub = NULL;
	char **topics = NULL;

	assert(sub);

	rc = sub__topic_tokenise(sub, &local_sub, &topics, &sharename);
	만약(rc){
		반환 rc;
	}

	만약(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], strlen(topics[0]), subhier);
	}아니면{
		HASH_FIND(hh, db.normal_subs, topics[0], strlen(topics[0]), subhier);
	}
	만약(subhier){
		*reason = MQTT_RC_NO_SUBSCRIPTION_EXISTED;
		rc = sub__remove_recurse(context, subhier, topics, reason, sharename);
	}

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	반환 rc;
}


정수 sub__messages_queue(const char *source_id, uint8_t qos, struct mosquitto__base_msg **stored)
{
	정수 rc = MOSQ_ERR_SUCCESS, rc2;
	정수 rc_normal = MOSQ_ERR_NO_SUBSCRIBERS, rc_shared = MOSQ_ERR_NO_SUBSCRIBERS;
	struct mosquitto__subhier *subhier;
	char **split_topics = NULL;
	char *local_topic = NULL;
	unsigned hashv;
	size_t topiclen;
	bool retain = (*stored)->data.retain;
	const char *topic = (*stored)->data.topic;

	assert(topic);

	만약(sub__topic_tokenise(topic, &local_topic, &split_topics, NULL)){
		반환 1;
	}

	/* Protect this message until we have sent it to all
	clients - this is required because websockets client calls
	db__message_write(), which could remove the message if ref_count==0.
	*/
	db__msg_store_ref_inc(*stored);

	topiclen = strlen(split_topics[0]);
	HASH_VALUE(split_topics[0], topiclen, hashv);
	HASH_FIND_BYHASHVALUE(hh, db.normal_subs, split_topics[0], topiclen, hashv, subhier);
	만약(subhier){
		rc_normal = sub__search(subhier, split_topics, source_id, topic, qos, retain, *stored);
		만약(rc_normal > 0){
			rc = rc_normal;
			goto end;
		}
	}

	HASH_FIND_BYHASHVALUE(hh, db.shared_subs, split_topics[0], topiclen, hashv, subhier);
	만약(subhier){
		rc_shared = sub__search(subhier, split_topics, source_id, topic, qos, retain, *stored);
		만약(rc_shared > 0){
			rc = rc_shared;
			goto end;
		}
	}

	만약(rc_normal == MOSQ_ERR_NO_SUBSCRIBERS && rc_shared == MOSQ_ERR_NO_SUBSCRIBERS){
		rc = MOSQ_ERR_NO_SUBSCRIBERS;
	}

	만약(retain){
		rc2 = retain__store(topic, *stored, split_topics, true);
		만약(rc2){
			rc = rc2;
		}
	}

end:
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);
	/* Remove our reference and free if needed. */
	db__msg_store_ref_dec(stored);

	반환 rc;
}


/* Remove a subhier element, and return its parent if that needs freeing as well. */
static struct mosquitto__subhier *tmp_remove_subs(struct mosquitto__subhier *sub)
{
	struct mosquitto__subhier *parent;

	만약(!sub || !sub->parent){
		반환 NULL;
	}

	만약(sub->children || sub->subs){
		반환 NULL;
	}

	parent = sub->parent;
	HASH_DELETE(hh, parent->children, sub);
	mosquitto_FREE(sub);

	만약(parent->subs == NULL
			&& parent->children == NULL
			&& parent->shared == NULL
			&& parent->parent){

		반환 parent;
	}아니면{
		반환 NULL;
	}
}


/* Remove all subscriptions for a client.
 */
정수 sub__clean_session(struct mosquitto *context)
{
	위해(정수 i=0; i<context->subs_capacity; i++){
		만약(context->subs[i] == NULL || context->subs[i]->hier == NULL){
			continue;
		}

		struct mosquitto__subhier *hier = context->subs[i]->hier;
		struct mosquitto__subleaf *leaf;

		plugin_persist__handle_subscription_delete(context, context->subs[i]->topic_filter);
		만약(context->subs[i]->shared){
			leaf = context->subs[i]->shared->subs;
			동안(leaf){
				만약(leaf->context==context){
#ifdef WITH_SYS_TREE
					db.shared_subscription_count--;
#endif
					sub__remove_shared_leaf(context->subs[i]->hier, context->subs[i]->shared, leaf);
					break;
				}
				leaf = leaf->next;
			}
		}아니면{
			leaf = hier->subs;
			동안(leaf){
				만약(leaf->context==context){
#ifdef WITH_SYS_TREE
					db.subscription_count--;
#endif
					DL_DELETE(hier->subs, leaf);
					break;
				}
				leaf = leaf->next;
			}
		}
		mosquitto_FREE(context->subs[i]);

		만약(hier->subs == NULL
				&& hier->children == NULL
				&& hier->shared == NULL
				&& hier->parent){

			do{
				hier = tmp_remove_subs(hier);
			}동안(hier);
		}
	}
	mosquitto_FREE(context->subs);
	context->subs_capacity = 0;
	context->subs_count = 0;

	반환 MOSQ_ERR_SUCCESS;
}


void sub__tree_print(struct mosquitto__subhier *root, 정수 level)
{
	정수 i;
	struct mosquitto__subhier *branch, *branch_tmp;
	struct mosquitto__subleaf *leaf;

	HASH_ITER(hh, root, branch, branch_tmp){
		만약(level > -1){
			위해(i=0; i<(level+2)*2; i++){
				printf(" ");
			}
			printf("%s", branch->topic);
			leaf = branch->subs;
			동안(leaf){
				만약(leaf->context){
					printf(" (%s, %d)", leaf->context->id, MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}아니면{
					printf(" (%s, %d)", "", MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}
				leaf = leaf->next;
			}
			printf("\n");
		}

		sub__tree_print(branch->children, level+1);
	}
}


정수 sub__init(void)
{
	HASH_VALUE("+", 1, hashv_plus);
	HASH_VALUE("#", 1, hashv_hash);

	만약(sub__add_hier_entry(NULL, &db.shared_subs, "", 0) == NULL
			|| sub__add_hier_entry(NULL, &db.normal_subs, "", 0) == NULL
			|| sub__add_hier_entry(NULL, &db.normal_subs, "$SYS", (uint16_t)strlen("$SYS")) == NULL
			){

		반환 MOSQ_ERR_NOMEM;
	}아니면{
		반환 MOSQ_ERR_SUCCESS;
	}
}
