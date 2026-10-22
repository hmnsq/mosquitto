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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"
#અન્તર્ભાવ "util_mosq.h"

#અન્તર્ભાવ "utlist.h"

સ્થિર સંરચના mosquitto__subhier *sub__add_hier_entry(સંરચના mosquitto__subhier *parent, સંરચના mosquitto__subhier **sibling, અચલ અક્ષર *topic, uint16_t len);

સ્થિર unsigned પૂર્ણાંક hashv_plus = 0;
સ્થિર unsigned પૂર્ણાંક hashv_hash = 0;


સ્થિર પૂર્ણાંક subs__send(સંરચના mosquitto__subleaf *leaf, અચલ અક્ષર *topic, uint8_t qos, પૂર્ણાંક retain, સંરચના mosquitto_base_સંદેશ *stored)
{
	bool client_retain;
	uint16_t mid;
	uint8_t client_qos, સંદેશ_qos;
	પૂર્ણાંક rc2;

	/* Check for ACL topic access. */
	rc2 = mosquitto_acl_check(leaf->context, topic, stored->data.payloadlen, stored->data.payload, stored->data.qos, stored->data.retain, stored->data.properties, MOSQ_ACL_READ);
	જો(rc2 == MOSQ_ERR_ACL_DENIED){
		પાછા MOSQ_ERR_SUCCESS;
	}નહીંતર જો(rc2 == MOSQ_ERR_SUCCESS){
		client_qos = MQTT_SUB_OPT_GET_QOS(leaf->subscription_options);

		જો(db.config->upgrade_outgoing_qos){
			સંદેશ_qos = client_qos;
		}નહીંતર{
			જો(qos > client_qos){
				સંદેશ_qos = client_qos;
			}નહીંતર{
				સંદેશ_qos = qos;
			}
		}
		જો(સંદેશ_qos){
			mid = mosquitto__mid_generate(leaf->context);
		}નહીંતર{
			mid = 0;
		}
		જો(MQTT_SUB_OPT_GET_RETAIN_AS_PUBLISHED(leaf->subscription_options)){
			client_retain = retain;
		}નહીંતર{
			client_retain = false;
		}
		જો(db_message_ઉમેરો_outgoing(leaf->context, 0, mid, સંદેશ_qos, client_retain, stored, leaf->identifier, true, true) == 1){
			પાછા 1;
		}
	}નહીંતર{
		પાછા 1; /* Application error */
	}
	પાછા 0;
}


સ્થિર પૂર્ણાંક subs__shared_process(સંરચના mosquitto__subhier *hier, અચલ અક્ષર *topic, uint8_t qos, પૂર્ણાંક retain, સંરચના mosquitto_base_સંદેશ *stored)
{
	પૂર્ણાંક rc = 0, rc2;
	સંરચના mosquitto__subshared *shared, *shared_tmp;
	સંરચના mosquitto__subleaf *leaf;

	HASH_ITER(hh, hier->shared, shared, shared_tmp){
		leaf = shared->subs;
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		/* Remove current from the top, add back to the bottom */
		DL_DELETE(shared->subs, leaf);
		DL_APPEND(shared->subs, leaf);

		જો(rc2){
			rc = 1;
		}
	}

	પાછા rc;
}


સ્થિર પૂર્ણાંક subs__process(સંરચના mosquitto__subhier *hier, અચલ અક્ષર *source_id, અચલ અક્ષર *topic, uint8_t qos, પૂર્ણાંક retain, સંરચના mosquitto_base_સંદેશ *stored)
{
	પૂર્ણાંક rc = 0;
	પૂર્ણાંક rc2;
	સંરચના mosquitto__subleaf *leaf;

	rc = subs__shared_process(hier, topic, qos, retain, stored);

	leaf = hier->subs;
	જ્યારે(source_id && leaf){
		જો(!leaf->context->id || (MQTT_SUB_OPT_GET_NO_LOCAL(leaf->subscription_options) && !strcmp(leaf->context->id, source_id))){
			leaf = leaf->next;
			ચાલુ;
		}
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		જો(rc2){
			rc = 1;
		}
		leaf = leaf->next;
	}
	જો(hier->subs || hier->shared){
		પાછા rc;
	}નહીંતર{
		પાછા MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


સ્થિર પૂર્ણાંક sub__add_leaf(સંરચના mosquitto *context, અચલ સંરચના mosquitto_subscription *sub, સંરચના mosquitto__subleaf **head, સંરચના mosquitto__subleaf **newleaf)
{
	સંરચના mosquitto__subleaf *leaf;

	*newleaf = ખાલી;
	leaf = *head;

	જ્યારે(leaf){
		જો(leaf->context && leaf->context->id && !strcmp(leaf->context->id, context->id)){
			/* Client making a second subscription to same topic. Only
			 * need to update QoS. Return MOSQ_ERR_SUB_EXISTS to
			 * indicate this to the calling function. */
			leaf->identifier = sub->identifier;
			leaf->subscription_options = sub->options;
			પાછા MOSQ_ERR_SUB_EXISTS;
		}
		leaf = leaf->next;
	}
	leaf = mosquitto_calloc(1, કદ(સંરચના mosquitto__subleaf) + strlen(sub->topic_filter) + 1);
	જો(!leaf){
		પાછા MOSQ_ERR_NOMEM;
	}
	leaf->context = context;
	leaf->identifier = sub->identifier;
	leaf->subscription_options = sub->options;
	strcpy(leaf->topic_filter, sub->topic_filter);

	DL_APPEND(*head, leaf);
	*newleaf = leaf;

	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર શૂન્ય sub__remove_shared_leaf(સંરચના mosquitto__subhier *subhier, સંરચના mosquitto__subshared *shared, સંરચના mosquitto__subleaf *leaf)
{
	DL_DELETE(shared->subs, leaf);
	જો(shared->subs == ખાલી){
		HASH_DELETE(hh, subhier->shared, shared);
		mosquitto_FREE(shared);
	}
}


સ્થિર પૂર્ણાંક sub__add_shared(સંરચના mosquitto *context, અચલ સંરચના mosquitto_subscription *sub, સંરચના mosquitto__subhier *subhier, અચલ અક્ષર *sharename)
{
	સંરચના mosquitto__subleaf *newleaf;
	સંરચના mosquitto__subshared *shared = ખાલી;
	સંરચના mosquitto__subleaf **subs;
	size_t slen;
	પૂર્ણાંક rc;
	unsigned hashv;

	slen = strlen(sharename);

	HASH_કિંમત(sharename, slen, hashv);

	HASH_FIND_BYHASHVALUE(hh, subhier->shared, sharename, slen, hashv, shared);
	જો(shared == ખાલી){
		shared = mosquitto_calloc(1, કદ(સંરચના mosquitto__subshared) + slen + 1);
		જો(!shared){
			પાછા MOSQ_ERR_NOMEM;
		}
		strncpy(shared->name, sharename, slen+1);

		HASH_ADD_BYHASHVALUE(hh, subhier->shared, name, slen, hashv, shared);
	}

	rc = sub__add_leaf(context, sub, &shared->subs, &newleaf);
	જો(rc > 0){
		જો(shared->subs == ખાલી){
			HASH_DELETE(hh, subhier->shared, shared);
			mosquitto_FREE(shared);
		}
		પાછા rc;
	}

	જો(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = shared;

		bool assigned = false;
		માટે(પૂર્ણાંક i=0; i<context->subs_capacity; i++){
			જો(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_ગણતરી++;
				assigned = true;
				તોડો;
			}
		}
		જો(assigned == false){
			subs = mosquitto_realloc(context->subs, કદ(સંરચના mosquitto__subleaf *)*(size_t)(context->subs_capacity + 1));
			જો(!subs){
				sub__remove_shared_leaf(subhier, shared, newleaf);
				mosquitto_FREE(newleaf);
				પાછા MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_capacity++;
			context->subs_ગણતરી++;
			context->subs[context->subs_capacity-1] = newleaf;
		}
#ifdef WITH_SYS_વૃક્ષ
		db.shared_subscription_ગણતરી++;
#endif
	}

	જો(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		પાછા rc;
	}નહીંતર{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		પાછા MOSQ_ERR_SUCCESS;
	}
}


સ્થિર પૂર્ણાંક sub__add_normal(સંરચના mosquitto *context, અચલ સંરચના mosquitto_subscription *sub, સંરચના mosquitto__subhier *subhier)
{
	સંરચના mosquitto__subleaf *newleaf = ખાલી;
	સંરચના mosquitto__subleaf **subs;
	પૂર્ણાંક rc;

	rc = sub__add_leaf(context, sub, &subhier->subs, &newleaf);
	જો(rc > 0){
		પાછા rc;
	}

	જો(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = ખાલી;

		bool assigned = false;
		માટે(પૂર્ણાંક i=0; i<context->subs_capacity; i++){
			જો(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_ગણતરી++;
				assigned = true;
				તોડો;
			}
		}
		જો(assigned == false){
			subs = mosquitto_realloc(context->subs, કદ(સંરચના mosquitto__subleaf *)*(size_t)(context->subs_capacity + 1));
			જો(!subs){
				DL_DELETE(subhier->subs, newleaf);
				mosquitto_FREE(newleaf);
				પાછા MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_capacity++;
			context->subs_ગણતરી++;
			context->subs[context->subs_capacity-1] = newleaf;
		}
#ifdef WITH_SYS_વૃક્ષ
		db.subscription_ગણતરી++;
#endif
	}

	જો(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		પાછા rc;
	}નહીંતર{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		પાછા MOSQ_ERR_SUCCESS;
	}
}


સ્થિર પૂર્ણાંક sub__add_context(સંરચના mosquitto *context, અચલ સંરચના mosquitto_subscription *sub, સંરચના mosquitto__subhier *subhier, અક્ષર *અચલ *અચલ topics, અચલ અક્ષર *sharename)
{
	સંરચના mosquitto__subhier *branch;
	પૂર્ણાંક topic_index = 0;
	size_t topiclen;

	/* Find leaf node */
	જ્યારે(topics && topics[topic_index] != ખાલી){
		topiclen = strlen(topics[topic_index]);
		જો(topiclen > UINT16_MAX){
			પાછા MOSQ_ERR_INVAL;
		}
		HASH_FIND(hh, subhier->children, topics[topic_index], topiclen, branch);
		જો(!branch){
			/* Not found */
			branch = sub__add_hier_entry(subhier, &subhier->children, topics[topic_index], (uint16_t)topiclen);
			જો(!branch){
				પાછા MOSQ_ERR_NOMEM;
			}
		}
		subhier = branch;
		topic_index++;
	}

	/* Add add our context */
	જો(context && context->id){
		જો(sharename){
			પાછા sub__add_shared(context, sub, subhier, sharename);
		}નહીંતર{
			પાછા sub__add_normal(context, sub, subhier);
		}
	}નહીંતર{
		પાછા MOSQ_ERR_SUCCESS;
	}
}


સ્થિર પૂર્ણાંક sub__remove_normal(સંરચના mosquitto *context, સંરચના mosquitto__subhier *subhier, uint8_t *reason)
{
	સંરચના mosquitto__subleaf *leaf;

	leaf = subhier->subs;
	જ્યારે(leaf){
		જો(leaf->context==context){
#ifdef WITH_SYS_વૃક્ષ
			db.subscription_ગણતરી--;
#endif
			DL_DELETE(subhier->subs, leaf);

			/* Remove the reference to the sub that the client is keeping.
			 * It would be nice to be able to use the reference directly,
			 * but that would involve keeping a copy of the topic string in
			 * each subleaf. Might be worth considering though. */
			માટે(પૂર્ણાંક i=0; i<context->subs_capacity; i++){
				જો(context->subs[i] && context->subs[i]->hier == subhier){
					context->subs_ગણતરી--;
					mosquitto_free(context->subs[i]);
					context->subs[i] = ખાલી;
					તોડો;
				}
			}
			*reason = 0;
			પાછા MOSQ_ERR_SUCCESS;
		}
		leaf = leaf->next;
	}
	પાછા MOSQ_ERR_NO_SUBSCRIBERS;
}


સ્થિર પૂર્ણાંક sub__remove_shared(સંરચના mosquitto *context, સંરચના mosquitto__subhier *subhier, uint8_t *reason, અચલ અક્ષર *sharename)
{
	સંરચના mosquitto__subshared *shared;

	HASH_FIND(hh, subhier->shared, sharename, strlen(sharename), shared);
	જો(shared){
		સંરચના mosquitto__subleaf *leaf = shared->subs;
		જ્યારે(leaf){
			જો(leaf->context==context){
#ifdef WITH_SYS_વૃક્ષ
				db.shared_subscription_ગણતરી--;
#endif
				DL_DELETE(shared->subs, leaf);

				/* Remove the reference to the sub that the client is keeping.
				* It would be nice to be able to use the reference directly,
				* but that would involve keeping a copy of the topic string in
				* each subleaf. Might be worth considering though. */
				માટે(પૂર્ણાંક i=0; i<context->subs_capacity; i++){
					જો(context->subs[i]
							&& context->subs[i]->hier == subhier
							&& context->subs[i]->shared == shared){

						mosquitto_free(context->subs[i]);
						context->subs[i] = ખાલી;
						context->subs_ગણતરી--;
						તોડો;
					}
				}

				જો(shared->subs == ખાલી){
					HASH_DELETE(hh, subhier->shared, shared);
					mosquitto_FREE(shared);
				}

				*reason = 0;
				પાછા MOSQ_ERR_SUCCESS;
			}
			leaf = leaf->next;
		}
		પાછા MOSQ_ERR_NO_SUBSCRIBERS;
	}નહીંતર{
		પાછા MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


સ્થિર પૂર્ણાંક sub__remove_recurse(સંરચના mosquitto *context, સંરચના mosquitto__subhier *subhier, અક્ષર **topics, uint8_t *reason, અચલ અક્ષર *sharename)
{
	સંરચના mosquitto__subhier *branch;

	જો(topics == ખાલી || topics[0] == ખાલી){
		જો(sharename){
			પાછા sub__remove_shared(context, subhier, reason, sharename);
		}નહીંતર{
			પાછા sub__remove_normal(context, subhier, reason);
		}
	}

	HASH_FIND(hh, subhier->children, topics[0], strlen(topics[0]), branch);
	જો(branch){
		sub__remove_recurse(context, branch, &(topics[1]), reason, sharename);
		જો(!branch->children && !branch->subs && !branch->shared){
			HASH_DELETE(hh, subhier->children, branch);
			mosquitto_FREE(branch);
		}
	}
	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક sub_શોધ(સંરચના mosquitto__subhier *subhier, અક્ષર **split_topics, અચલ અક્ષર *source_id, અચલ અક્ષર *topic, uint8_t qos, પૂર્ણાંક retain, સંરચના mosquitto_base_સંદેશ *stored)
{
	/* FIXME - need to take into account source_id if the client is a bridge */
	સંરચના mosquitto__subhier *branch;
	પૂર્ણાંક rc;
	bool have_subscribers = false;

	જો(split_topics && split_topics[0]){
		/* Check for literal match */
		HASH_FIND(hh, subhier->children, split_topics[0], strlen(split_topics[0]), branch);

		જો(branch){
			rc = sub_શોધ(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			જો(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}નહીંતર જો(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				પાછા rc;
			}
			જો(split_topics[1] == ખાલી){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				જો(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}નહીંતર જો(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					પાછા rc;
				}
			}
		}

		/* Check for + match */
		HASH_FIND_BYHASHVALUE(hh, subhier->children, "+", 1, hashv_plus, branch);

		જો(branch){
			rc = sub_શોધ(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			જો(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}નહીંતર જો(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				પાછા rc;
			}
			જો(split_topics[1] == ખાલી){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				જો(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}નહીંતર જો(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					પાછા rc;
				}
			}
		}
	}

	/* Check for # match */
	HASH_FIND_BYHASHVALUE(hh, subhier->children, "#", 1, hashv_hash, branch);
	જો(branch && !branch->children){
		/* The topic matches due to a # wildcard - process the
		 * subscriptions but *don't* return. Although this branch has ended
		 * there may still be other subscriptions to deal with.
		 */
		rc = subs__process(branch, source_id, topic, qos, retain, stored);
		જો(rc == MOSQ_ERR_SUCCESS){
			have_subscribers = true;
		}નહીંતર જો(rc != MOSQ_ERR_NO_SUBSCRIBERS){
			પાછા rc;
		}
	}

	જો(have_subscribers){
		પાછા MOSQ_ERR_SUCCESS;
	}નહીંતર{
		પાછા MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


સ્થિર સંરચના mosquitto__subhier *sub__add_hier_entry(સંરચના mosquitto__subhier *parent, સંરચના mosquitto__subhier **sibling, અચલ અક્ષર *topic, uint16_t len)
{
	સંરચના mosquitto__subhier *child;

	પ્રતિજ્ઞા(sibling);

	child = mosquitto_calloc(1, કદ(સંરચના mosquitto__subhier) + len + 1);
	જો(!child){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
		પાછા ખાલી;
	}
	child->parent = parent;
	child->topic_len = len;
	જો(len > 0){
		strncpy(child->topic, topic, (size_t)(len+1));
	}

	HASH_ADD(hh, *sibling, topic, child->topic_len, child);

	પાછા child;
}


પૂર્ણાંક sub__add(સંરચના mosquitto *context, અચલ સંરચના mosquitto_subscription *sub)
{
	પૂર્ણાંક rc = 0;
	સંરચના mosquitto__subhier *subhier;
	અચલ અક્ષર *sharename = ખાલી;
	અક્ષર *local_sub;
	અક્ષર **topics;
	size_t topiclen;

	પ્રતિજ્ઞા(sub);
	પ્રતિજ્ઞા(sub->topic_filter);

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &topics, &sharename);
	જો(rc){
		પાછા rc;
	}

	topiclen = strlen(topics[0]);
	જો(topiclen > UINT16_MAX){
		mosquitto_FREE(local_sub);
		mosquitto_FREE(topics);
		પાછા MOSQ_ERR_INVAL;
	}

	જો(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], topiclen, subhier);
		જો(!subhier){
			subhier = sub__add_hier_entry(ખાલી, &db.shared_subs, topics[0], (uint16_t)topiclen);
			જો(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
				પાછા MOSQ_ERR_NOMEM;
			}
		}
	}નહીંતર{
		HASH_FIND(hh, db.normal_subs, topics[0], topiclen, subhier);
		જો(!subhier){
			subhier = sub__add_hier_entry(ખાલી, &db.normal_subs, topics[0], (uint16_t)topiclen);
			જો(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
				પાછા MOSQ_ERR_NOMEM;
			}
		}
	}
	rc = sub__add_context(context, sub, subhier, topics, sharename);

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	પાછા rc;
}


પૂર્ણાંક sub__remove(સંરચના mosquitto *context, અચલ અક્ષર *sub, uint8_t *reason)
{
	પૂર્ણાંક rc = 0;
	સંરચના mosquitto__subhier *subhier;
	અચલ અક્ષર *sharename = ખાલી;
	અક્ષર *local_sub = ખાલી;
	અક્ષર **topics = ખાલી;

	પ્રતિજ્ઞા(sub);

	rc = sub__topic_tokenise(sub, &local_sub, &topics, &sharename);
	જો(rc){
		પાછા rc;
	}

	જો(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], strlen(topics[0]), subhier);
	}નહીંતર{
		HASH_FIND(hh, db.normal_subs, topics[0], strlen(topics[0]), subhier);
	}
	જો(subhier){
		*reason = MQTT_RC_NO_SUBSCRIPTION_EXISTED;
		rc = sub__remove_recurse(context, subhier, topics, reason, sharename);
	}

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	પાછા rc;
}


પૂર્ણાંક sub__messages_queue(અચલ અક્ષર *source_id, uint8_t qos, સંરચના mosquitto_base_સંદેશ **stored)
{
	પૂર્ણાંક rc = MOSQ_ERR_SUCCESS, rc2;
	પૂર્ણાંક rc_normal = MOSQ_ERR_NO_SUBSCRIBERS, rc_shared = MOSQ_ERR_NO_SUBSCRIBERS;
	સંરચના mosquitto__subhier *subhier;
	અક્ષર **split_topics = ખાલી;
	અક્ષર *local_topic = ખાલી;
	unsigned hashv;
	size_t topiclen;
	bool retain = (*stored)->data.retain;
	અચલ અક્ષર *topic = (*stored)->data.topic;

	પ્રતિજ્ઞા(topic);

	જો(sub__topic_tokenise(topic, &local_topic, &split_topics, ખાલી)){
		પાછા 1;
	}

	/* Protect this message until we have sent it to all
	clients - this is required because websockets client calls
	db__message_write(), which could remove the message if ref_count==0.
	*/
	db_સંદેશ_store_ref_inc(*stored);

	topiclen = strlen(split_topics[0]);
	HASH_કિંમત(split_topics[0], topiclen, hashv);
	HASH_FIND_BYHASHVALUE(hh, db.normal_subs, split_topics[0], topiclen, hashv, subhier);
	જો(subhier){
		rc_normal = sub_શોધ(subhier, split_topics, source_id, topic, qos, retain, *stored);
		જો(rc_normal > 0){
			rc = rc_normal;
			જાઓ end;
		}
	}

	HASH_FIND_BYHASHVALUE(hh, db.shared_subs, split_topics[0], topiclen, hashv, subhier);
	જો(subhier){
		rc_shared = sub_શોધ(subhier, split_topics, source_id, topic, qos, retain, *stored);
		જો(rc_shared > 0){
			rc = rc_shared;
			જાઓ end;
		}
	}

	જો(rc_normal == MOSQ_ERR_NO_SUBSCRIBERS && rc_shared == MOSQ_ERR_NO_SUBSCRIBERS){
		rc = MOSQ_ERR_NO_SUBSCRIBERS;
	}

	જો(retain){
		rc2 = retain__store(topic, *stored, split_topics, true);
		જો(rc2){
			rc = rc2;
		}
	}

end:
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);
	/* Remove our reference and free if needed. */
	db_સંદેશ_store_ref_dec(stored);

	પાછા rc;
}


/* Remove a subhier element, and return its parent if that needs freeing as well. */
સ્થિર સંરચના mosquitto__subhier *tmp_remove_subs(સંરચના mosquitto__subhier *sub)
{
	સંરચના mosquitto__subhier *parent;

	જો(!sub || !sub->parent){
		પાછા ખાલી;
	}

	જો(sub->children || sub->subs){
		પાછા ખાલી;
	}

	parent = sub->parent;
	HASH_DELETE(hh, parent->children, sub);
	mosquitto_FREE(sub);

	જો(parent->subs == ખાલી
			&& parent->children == ખાલી
			&& parent->shared == ખાલી
			&& parent->parent){

		પાછા parent;
	}નહીંતર{
		પાછા ખાલી;
	}
}


/* Remove all subscriptions for a client.
 */
પૂર્ણાંક sub__clean_session(સંરચના mosquitto *context)
{
	માટે(પૂર્ણાંક i=0; i<context->subs_capacity; i++){
		જો(context->subs[i] == ખાલી || context->subs[i]->hier == ખાલી){
			ચાલુ;
		}

		સંરચના mosquitto__subhier *hier = context->subs[i]->hier;
		સંરચના mosquitto__subleaf *leaf;

		plugin_persist__handle_subscription_delete(context, context->subs[i]->topic_filter);
		જો(context->subs[i]->shared){
			leaf = context->subs[i]->shared->subs;
			જ્યારે(leaf){
				જો(leaf->context==context){
#ifdef WITH_SYS_વૃક્ષ
					db.shared_subscription_ગણતરી--;
#endif
					sub__remove_shared_leaf(context->subs[i]->hier, context->subs[i]->shared, leaf);
					તોડો;
				}
				leaf = leaf->next;
			}
		}નહીંતર{
			leaf = hier->subs;
			જ્યારે(leaf){
				જો(leaf->context==context){
#ifdef WITH_SYS_વૃક્ષ
					db.subscription_ગણતરી--;
#endif
					DL_DELETE(hier->subs, leaf);
					તોડો;
				}
				leaf = leaf->next;
			}
		}
		mosquitto_FREE(context->subs[i]);

		જો(hier->subs == ખાલી
				&& hier->children == ખાલી
				&& hier->shared == ખાલી
				&& hier->parent){

			કરો{
				hier = tmp_remove_subs(hier);
			}જ્યારે(hier);
		}
	}
	mosquitto_FREE(context->subs);
	context->subs_capacity = 0;
	context->subs_ગણતરી = 0;

	પાછા MOSQ_ERR_SUCCESS;
}


શૂન્ય sub_વૃક્ષ_print(સંરચના mosquitto__subhier *મૂળ, પૂર્ણાંક level)
{
	પૂર્ણાંક i;
	સંરચના mosquitto__subhier *branch, *branch_tmp;
	સંરચના mosquitto__subleaf *leaf;

	HASH_ITER(hh, મૂળ, branch, branch_tmp){
		જો(level > -1){
			માટે(i=0; i<(level+2)*2; i++){
				printf(" ");
			}
			printf("%s", branch->topic);
			leaf = branch->subs;
			જ્યારે(leaf){
				જો(leaf->context){
					printf(" (%s, %d)", leaf->context->id, MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}નહીંતર{
					printf(" (%s, %d)", "", MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}
				leaf = leaf->next;
			}
			printf("\n");
		}

		sub_વૃક્ષ_print(branch->children, level+1);
	}
}


પૂર્ણાંક sub__init(શૂન્ય)
{
	HASH_કિંમત("+", 1, hashv_plus);
	HASH_કિંમત("#", 1, hashv_hash);

	જો(sub__add_hier_entry(ખાલી, &db.shared_subs, "", 0) == ખાલી
			|| sub__add_hier_entry(ખાલી, &db.normal_subs, "", 0) == ખાલી
			|| sub__add_hier_entry(ખાલી, &db.normal_subs, "$SYS", (uint16_t)strlen("$SYS")) == ખાલી
			){

		પાછા MOSQ_ERR_NOMEM;
	}નહીંતર{
		પાછા MOSQ_ERR_SUCCESS;
	}
}
