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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"

#ଅନ୍ତର୍ଭାଵ "utlist.h"

ସ୍ଥିର ସଂରଚନା mosquitto__subhier *sub__add_hier_entry(ସଂରଚନା mosquitto__subhier *parent, ସଂରଚନା mosquitto__subhier **sibling, ଅଚଳ ଅକ୍ଷର *topic, uint16_t len);

ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashv_plus = 0;
ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashv_hash = 0;


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା subs__send(ସଂରଚନା mosquitto__subleaf *leaf, ଅଚଳ ଅକ୍ଷର *topic, uint8_t qos, ପୂର୍ଣ୍ଣସଂଖ୍ୟା retain, ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *stored)
{
	bool client_retain;
	uint16_t mid;
	uint8_t client_qos, ସନ୍ଦେଶ_qos;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc2;

	/* Check for ACL topic access. */
	rc2 = mosquitto_acl_check(leaf->context, topic, stored->data.payloadlen, stored->data.payload, stored->data.qos, stored->data.retain, stored->data.properties, MOSQ_ACL_READ);
	ଯଦି(rc2 == MOSQ_ERR_ACL_DENIED){
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}ଅନ୍ୟଥା ଯଦି(rc2 == MOSQ_ERR_SUCCESS){
		client_qos = MQTT_SUB_OPT_GET_QOS(leaf->subscription_options);

		ଯଦି(db.config->upgrade_outgoing_qos){
			ସନ୍ଦେଶ_qos = client_qos;
		}ଅନ୍ୟଥା{
			ଯଦି(qos > client_qos){
				ସନ୍ଦେଶ_qos = client_qos;
			}ଅନ୍ୟଥା{
				ସନ୍ଦେଶ_qos = qos;
			}
		}
		ଯଦି(ସନ୍ଦେଶ_qos){
			mid = mosquitto__mid_generate(leaf->context);
		}ଅନ୍ୟଥା{
			mid = 0;
		}
		ଯଦି(MQTT_SUB_OPT_GET_RETAIN_AS_PUBLISHED(leaf->subscription_options)){
			client_retain = retain;
		}ଅନ୍ୟଥା{
			client_retain = false;
		}
		ଯଦି(db_message_ଭର୍ତ୍ତି_outgoing(leaf->context, 0, mid, ସନ୍ଦେଶ_qos, client_retain, stored, leaf->identifier, true, true) == 1){
			ଫେରନ୍ତୁ 1;
		}
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ 1; /* Application error */
	}
	ଫେରନ୍ତୁ 0;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା subs__shared_process(ସଂରଚନା mosquitto__subhier *hier, ଅଚଳ ଅକ୍ଷର *topic, uint8_t qos, ପୂର୍ଣ୍ଣସଂଖ୍ୟା retain, ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *stored)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0, rc2;
	ସଂରଚନା mosquitto__subshared *shared, *shared_tmp;
	ସଂରଚନା mosquitto__subleaf *leaf;

	HASH_ITER(hh, hier->shared, shared, shared_tmp){
		leaf = shared->subs;
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		/* Remove current from the top, add back to the bottom */
		DL_DELETE(shared->subs, leaf);
		DL_APPEND(shared->subs, leaf);

		ଯଦି(rc2){
			rc = 1;
		}
	}

	ଫେରନ୍ତୁ rc;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା subs__process(ସଂରଚନା mosquitto__subhier *hier, ଅଚଳ ଅକ୍ଷର *source_id, ଅଚଳ ଅକ୍ଷର *topic, uint8_t qos, ପୂର୍ଣ୍ଣସଂଖ୍ୟା retain, ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *stored)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc2;
	ସଂରଚନା mosquitto__subleaf *leaf;

	rc = subs__shared_process(hier, topic, qos, retain, stored);

	leaf = hier->subs;
	ଯେତେବେଳେ(source_id && leaf){
		ଯଦି(!leaf->context->id || (MQTT_SUB_OPT_GET_NO_LOCAL(leaf->subscription_options) && !strcmp(leaf->context->id, source_id))){
			leaf = leaf->next;
			ଜାରି;
		}
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		ଯଦି(rc2){
			rc = 1;
		}
		leaf = leaf->next;
	}
	ଯଦି(hier->subs || hier->shared){
		ଫେରନ୍ତୁ rc;
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sub__add_leaf(ସଂରଚନା mosquitto *context, ଅଚଳ ସଂରଚନା mosquitto_subscription *sub, ସଂରଚନା mosquitto__subleaf **head, ସଂରଚନା mosquitto__subleaf **newleaf)
{
	ସଂରଚନା mosquitto__subleaf *leaf;

	*newleaf = ଖାଲି;
	leaf = *head;

	ଯେତେବେଳେ(leaf){
		ଯଦି(leaf->context && leaf->context->id && !strcmp(leaf->context->id, context->id)){
			/* Client making a second subscription to same topic. Only
			 * need to update QoS. Return MOSQ_ERR_SUB_EXISTS to
			 * indicate this to the calling function. */
			leaf->identifier = sub->identifier;
			leaf->subscription_options = sub->options;
			ଫେରନ୍ତୁ MOSQ_ERR_SUB_EXISTS;
		}
		leaf = leaf->next;
	}
	leaf = mosquitto_calloc(1, ଆକାର(ସଂରଚନା mosquitto__subleaf) + strlen(sub->topic_filter) + 1);
	ଯଦି(!leaf){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}
	leaf->context = context;
	leaf->identifier = sub->identifier;
	leaf->subscription_options = sub->options;
	strcpy(leaf->topic_filter, sub->topic_filter);

	DL_APPEND(*head, leaf);
	*newleaf = leaf;

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ଶୂନ୍ୟ sub__remove_shared_leaf(ସଂରଚନା mosquitto__subhier *subhier, ସଂରଚନା mosquitto__subshared *shared, ସଂରଚନା mosquitto__subleaf *leaf)
{
	DL_DELETE(shared->subs, leaf);
	ଯଦି(shared->subs == ଖାଲି){
		HASH_DELETE(hh, subhier->shared, shared);
		mosquitto_FREE(shared);
	}
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sub__add_shared(ସଂରଚନା mosquitto *context, ଅଚଳ ସଂରଚନା mosquitto_subscription *sub, ସଂରଚନା mosquitto__subhier *subhier, ଅଚଳ ଅକ୍ଷର *sharename)
{
	ସଂରଚନା mosquitto__subleaf *newleaf;
	ସଂରଚନା mosquitto__subshared *shared = ଖାଲି;
	ସଂରଚନା mosquitto__subleaf **subs;
	size_t slen;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	unsigned hashv;

	slen = strlen(sharename);

	HASH_ମୂଲ୍ୟ(sharename, slen, hashv);

	HASH_FIND_BYHASHVALUE(hh, subhier->shared, sharename, slen, hashv, shared);
	ଯଦି(shared == ଖାଲି){
		shared = mosquitto_calloc(1, ଆକାର(ସଂରଚନା mosquitto__subshared) + slen + 1);
		ଯଦି(!shared){
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}
		strncpy(shared->name, sharename, slen+1);

		HASH_ADD_BYHASHVALUE(hh, subhier->shared, name, slen, hashv, shared);
	}

	rc = sub__add_leaf(context, sub, &shared->subs, &newleaf);
	ଯଦି(rc > 0){
		ଯଦି(shared->subs == ଖାଲି){
			HASH_DELETE(hh, subhier->shared, shared);
			mosquitto_FREE(shared);
		}
		ଫେରନ୍ତୁ rc;
	}

	ଯଦି(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = shared;

		bool assigned = false;
		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<context->subs_capacity; i++){
			ଯଦି(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_ଗଣନା++;
				assigned = true;
				ଭାଙ୍ଗ;
			}
		}
		ଯଦି(assigned == false){
			subs = mosquitto_realloc(context->subs, ଆକାର(ସଂରଚନା mosquitto__subleaf *)*(size_t)(context->subs_capacity + 1));
			ଯଦି(!subs){
				sub__remove_shared_leaf(subhier, shared, newleaf);
				mosquitto_FREE(newleaf);
				ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_capacity++;
			context->subs_ଗଣନା++;
			context->subs[context->subs_capacity-1] = newleaf;
		}
#ifdef WITH_SYS_ବୃକ୍ଷ
		db.shared_subscription_ଗଣନା++;
#endif
	}

	ଯଦି(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		ଫେରନ୍ତୁ rc;
	}ଅନ୍ୟଥା{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sub__add_normal(ସଂରଚନା mosquitto *context, ଅଚଳ ସଂରଚନା mosquitto_subscription *sub, ସଂରଚନା mosquitto__subhier *subhier)
{
	ସଂରଚନା mosquitto__subleaf *newleaf = ଖାଲି;
	ସଂରଚନା mosquitto__subleaf **subs;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	rc = sub__add_leaf(context, sub, &subhier->subs, &newleaf);
	ଯଦି(rc > 0){
		ଫେରନ୍ତୁ rc;
	}

	ଯଦି(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = ଖାଲି;

		bool assigned = false;
		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<context->subs_capacity; i++){
			ଯଦି(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_ଗଣନା++;
				assigned = true;
				ଭାଙ୍ଗ;
			}
		}
		ଯଦି(assigned == false){
			subs = mosquitto_realloc(context->subs, ଆକାର(ସଂରଚନା mosquitto__subleaf *)*(size_t)(context->subs_capacity + 1));
			ଯଦି(!subs){
				DL_DELETE(subhier->subs, newleaf);
				mosquitto_FREE(newleaf);
				ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_capacity++;
			context->subs_ଗଣନା++;
			context->subs[context->subs_capacity-1] = newleaf;
		}
#ifdef WITH_SYS_ବୃକ୍ଷ
		db.subscription_ଗଣନା++;
#endif
	}

	ଯଦି(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		ଫେରନ୍ତୁ rc;
	}ଅନ୍ୟଥା{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sub__add_context(ସଂରଚନା mosquitto *context, ଅଚଳ ସଂରଚନା mosquitto_subscription *sub, ସଂରଚନା mosquitto__subhier *subhier, ଅକ୍ଷର *ଅଚଳ *ଅଚଳ topics, ଅଚଳ ଅକ୍ଷର *sharename)
{
	ସଂରଚନା mosquitto__subhier *branch;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା topic_index = 0;
	size_t topiclen;

	/* Find leaf node */
	ଯେତେବେଳେ(topics && topics[topic_index] != ଖାଲି){
		topiclen = strlen(topics[topic_index]);
		ଯଦି(topiclen > UINT16_MAX){
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
		HASH_FIND(hh, subhier->children, topics[topic_index], topiclen, branch);
		ଯଦି(!branch){
			/* Not found */
			branch = sub__add_hier_entry(subhier, &subhier->children, topics[topic_index], (uint16_t)topiclen);
			ଯଦି(!branch){
				ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
			}
		}
		subhier = branch;
		topic_index++;
	}

	/* Add add our context */
	ଯଦି(context && context->id){
		ଯଦି(sharename){
			ଫେରନ୍ତୁ sub__add_shared(context, sub, subhier, sharename);
		}ଅନ୍ୟଥା{
			ଫେରନ୍ତୁ sub__add_normal(context, sub, subhier);
		}
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sub__remove_normal(ସଂରଚନା mosquitto *context, ସଂରଚନା mosquitto__subhier *subhier, uint8_t *reason)
{
	ସଂରଚନା mosquitto__subleaf *leaf;

	leaf = subhier->subs;
	ଯେତେବେଳେ(leaf){
		ଯଦି(leaf->context==context){
#ifdef WITH_SYS_ବୃକ୍ଷ
			db.subscription_ଗଣନା--;
#endif
			DL_DELETE(subhier->subs, leaf);

			/* Remove the reference to the sub that the client is keeping.
			 * It would be nice to be able to use the reference directly,
			 * but that would involve keeping a copy of the topic string in
			 * each subleaf. Might be worth considering though. */
			ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<context->subs_capacity; i++){
				ଯଦି(context->subs[i] && context->subs[i]->hier == subhier){
					context->subs_ଗଣନା--;
					mosquitto_free(context->subs[i]);
					context->subs[i] = ଖାଲି;
					ଭାଙ୍ଗ;
				}
			}
			*reason = 0;
			ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
		}
		leaf = leaf->next;
	}
	ଫେରନ୍ତୁ MOSQ_ERR_NO_SUBSCRIBERS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sub__remove_shared(ସଂରଚନା mosquitto *context, ସଂରଚନା mosquitto__subhier *subhier, uint8_t *reason, ଅଚଳ ଅକ୍ଷର *sharename)
{
	ସଂରଚନା mosquitto__subshared *shared;

	HASH_FIND(hh, subhier->shared, sharename, strlen(sharename), shared);
	ଯଦି(shared){
		ସଂରଚନା mosquitto__subleaf *leaf = shared->subs;
		ଯେତେବେଳେ(leaf){
			ଯଦି(leaf->context==context){
#ifdef WITH_SYS_ବୃକ୍ଷ
				db.shared_subscription_ଗଣନା--;
#endif
				DL_DELETE(shared->subs, leaf);

				/* Remove the reference to the sub that the client is keeping.
				* It would be nice to be able to use the reference directly,
				* but that would involve keeping a copy of the topic string in
				* each subleaf. Might be worth considering though. */
				ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<context->subs_capacity; i++){
					ଯଦି(context->subs[i]
							&& context->subs[i]->hier == subhier
							&& context->subs[i]->shared == shared){

						mosquitto_free(context->subs[i]);
						context->subs[i] = ଖାଲି;
						context->subs_ଗଣନା--;
						ଭାଙ୍ଗ;
					}
				}

				ଯଦି(shared->subs == ଖାଲି){
					HASH_DELETE(hh, subhier->shared, shared);
					mosquitto_FREE(shared);
				}

				*reason = 0;
				ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
			}
			leaf = leaf->next;
		}
		ଫେରନ୍ତୁ MOSQ_ERR_NO_SUBSCRIBERS;
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sub__remove_recurse(ସଂରଚନା mosquitto *context, ସଂରଚନା mosquitto__subhier *subhier, ଅକ୍ଷର **topics, uint8_t *reason, ଅଚଳ ଅକ୍ଷର *sharename)
{
	ସଂରଚନା mosquitto__subhier *branch;

	ଯଦି(topics == ଖାଲି || topics[0] == ଖାଲି){
		ଯଦି(sharename){
			ଫେରନ୍ତୁ sub__remove_shared(context, subhier, reason, sharename);
		}ଅନ୍ୟଥା{
			ଫେରନ୍ତୁ sub__remove_normal(context, subhier, reason);
		}
	}

	HASH_FIND(hh, subhier->children, topics[0], strlen(topics[0]), branch);
	ଯଦି(branch){
		sub__remove_recurse(context, branch, &(topics[1]), reason, sharename);
		ଯଦି(!branch->children && !branch->subs && !branch->shared){
			HASH_DELETE(hh, subhier->children, branch);
			mosquitto_FREE(branch);
		}
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sub_ସନ୍ଧାନ(ସଂରଚନା mosquitto__subhier *subhier, ଅକ୍ଷର **split_topics, ଅଚଳ ଅକ୍ଷର *source_id, ଅଚଳ ଅକ୍ଷର *topic, uint8_t qos, ପୂର୍ଣ୍ଣସଂଖ୍ୟା retain, ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *stored)
{
	/* FIXME - need to take into account source_id if the client is a bridge */
	ସଂରଚନା mosquitto__subhier *branch;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	bool have_subscribers = false;

	ଯଦି(split_topics && split_topics[0]){
		/* Check for literal match */
		HASH_FIND(hh, subhier->children, split_topics[0], strlen(split_topics[0]), branch);

		ଯଦି(branch){
			rc = sub_ସନ୍ଧାନ(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			ଯଦି(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}ଅନ୍ୟଥା ଯଦି(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				ଫେରନ୍ତୁ rc;
			}
			ଯଦି(split_topics[1] == ଖାଲି){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				ଯଦି(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}ଅନ୍ୟଥା ଯଦି(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					ଫେରନ୍ତୁ rc;
				}
			}
		}

		/* Check for + match */
		HASH_FIND_BYHASHVALUE(hh, subhier->children, "+", 1, hashv_plus, branch);

		ଯଦି(branch){
			rc = sub_ସନ୍ଧାନ(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			ଯଦି(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}ଅନ୍ୟଥା ଯଦି(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				ଫେରନ୍ତୁ rc;
			}
			ଯଦି(split_topics[1] == ଖାଲି){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				ଯଦି(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}ଅନ୍ୟଥା ଯଦି(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					ଫେରନ୍ତୁ rc;
				}
			}
		}
	}

	/* Check for # match */
	HASH_FIND_BYHASHVALUE(hh, subhier->children, "#", 1, hashv_hash, branch);
	ଯଦି(branch && !branch->children){
		/* The topic matches due to a # wildcard - process the
		 * subscriptions but *don't* return. Although this branch has ended
		 * there may still be other subscriptions to deal with.
		 */
		rc = subs__process(branch, source_id, topic, qos, retain, stored);
		ଯଦି(rc == MOSQ_ERR_SUCCESS){
			have_subscribers = true;
		}ଅନ୍ୟଥା ଯଦି(rc != MOSQ_ERR_NO_SUBSCRIBERS){
			ଫେରନ୍ତୁ rc;
		}
	}

	ଯଦି(have_subscribers){
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


ସ୍ଥିର ସଂରଚନା mosquitto__subhier *sub__add_hier_entry(ସଂରଚନା mosquitto__subhier *parent, ସଂରଚନା mosquitto__subhier **sibling, ଅଚଳ ଅକ୍ଷର *topic, uint16_t len)
{
	ସଂରଚନା mosquitto__subhier *child;

	ପ୍ରତିଜ୍ଞା(sibling);

	child = mosquitto_calloc(1, ଆକାର(ସଂରଚନା mosquitto__subhier) + len + 1);
	ଯଦି(!child){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
		ଫେରନ୍ତୁ ଖାଲି;
	}
	child->parent = parent;
	child->topic_len = len;
	ଯଦି(len > 0){
		strncpy(child->topic, topic, (size_t)(len+1));
	}

	HASH_ADD(hh, *sibling, topic, child->topic_len, child);

	ଫେରନ୍ତୁ child;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା sub__add(ସଂରଚନା mosquitto *context, ଅଚଳ ସଂରଚନା mosquitto_subscription *sub)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;
	ସଂରଚନା mosquitto__subhier *subhier;
	ଅଚଳ ଅକ୍ଷର *sharename = ଖାଲି;
	ଅକ୍ଷର *local_sub;
	ଅକ୍ଷର **topics;
	size_t topiclen;

	ପ୍ରତିଜ୍ଞା(sub);
	ପ୍ରତିଜ୍ଞା(sub->topic_filter);

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &topics, &sharename);
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}

	topiclen = strlen(topics[0]);
	ଯଦି(topiclen > UINT16_MAX){
		mosquitto_FREE(local_sub);
		mosquitto_FREE(topics);
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	ଯଦି(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], topiclen, subhier);
		ଯଦି(!subhier){
			subhier = sub__add_hier_entry(ଖାଲି, &db.shared_subs, topics[0], (uint16_t)topiclen);
			ଯଦି(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
				ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
			}
		}
	}ଅନ୍ୟଥା{
		HASH_FIND(hh, db.normal_subs, topics[0], topiclen, subhier);
		ଯଦି(!subhier){
			subhier = sub__add_hier_entry(ଖାଲି, &db.normal_subs, topics[0], (uint16_t)topiclen);
			ଯଦି(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
				ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
			}
		}
	}
	rc = sub__add_context(context, sub, subhier, topics, sharename);

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	ଫେରନ୍ତୁ rc;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା sub__remove(ସଂରଚନା mosquitto *context, ଅଚଳ ଅକ୍ଷର *sub, uint8_t *reason)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;
	ସଂରଚନା mosquitto__subhier *subhier;
	ଅଚଳ ଅକ୍ଷର *sharename = ଖାଲି;
	ଅକ୍ଷର *local_sub = ଖାଲି;
	ଅକ୍ଷର **topics = ଖାଲି;

	ପ୍ରତିଜ୍ଞା(sub);

	rc = sub__topic_tokenise(sub, &local_sub, &topics, &sharename);
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}

	ଯଦି(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], strlen(topics[0]), subhier);
	}ଅନ୍ୟଥା{
		HASH_FIND(hh, db.normal_subs, topics[0], strlen(topics[0]), subhier);
	}
	ଯଦି(subhier){
		*reason = MQTT_RC_NO_SUBSCRIPTION_EXISTED;
		rc = sub__remove_recurse(context, subhier, topics, reason, sharename);
	}

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	ଫେରନ୍ତୁ rc;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା sub__messages_queue(ଅଚଳ ଅକ୍ଷର *source_id, uint8_t qos, ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ **stored)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = MOSQ_ERR_SUCCESS, rc2;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc_normal = MOSQ_ERR_NO_SUBSCRIBERS, rc_shared = MOSQ_ERR_NO_SUBSCRIBERS;
	ସଂରଚନା mosquitto__subhier *subhier;
	ଅକ୍ଷର **split_topics = ଖାଲି;
	ଅକ୍ଷର *local_topic = ଖାଲି;
	unsigned hashv;
	size_t topiclen;
	bool retain = (*stored)->data.retain;
	ଅଚଳ ଅକ୍ଷର *topic = (*stored)->data.topic;

	ପ୍ରତିଜ୍ଞା(topic);

	ଯଦି(sub__topic_tokenise(topic, &local_topic, &split_topics, ଖାଲି)){
		ଫେରନ୍ତୁ 1;
	}

	/* Protect this message until we have sent it to all
	clients - this is required because websockets client calls
	db__message_write(), which could remove the message if ref_count==0.
	*/
	db_ସନ୍ଦେଶ_store_ref_inc(*stored);

	topiclen = strlen(split_topics[0]);
	HASH_ମୂଲ୍ୟ(split_topics[0], topiclen, hashv);
	HASH_FIND_BYHASHVALUE(hh, db.normal_subs, split_topics[0], topiclen, hashv, subhier);
	ଯଦି(subhier){
		rc_normal = sub_ସନ୍ଧାନ(subhier, split_topics, source_id, topic, qos, retain, *stored);
		ଯଦି(rc_normal > 0){
			rc = rc_normal;
			ଯାଅ end;
		}
	}

	HASH_FIND_BYHASHVALUE(hh, db.shared_subs, split_topics[0], topiclen, hashv, subhier);
	ଯଦି(subhier){
		rc_shared = sub_ସନ୍ଧାନ(subhier, split_topics, source_id, topic, qos, retain, *stored);
		ଯଦି(rc_shared > 0){
			rc = rc_shared;
			ଯାଅ end;
		}
	}

	ଯଦି(rc_normal == MOSQ_ERR_NO_SUBSCRIBERS && rc_shared == MOSQ_ERR_NO_SUBSCRIBERS){
		rc = MOSQ_ERR_NO_SUBSCRIBERS;
	}

	ଯଦି(retain){
		rc2 = retain__store(topic, *stored, split_topics, true);
		ଯଦି(rc2){
			rc = rc2;
		}
	}

end:
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);
	/* Remove our reference and free if needed. */
	db_ସନ୍ଦେଶ_store_ref_dec(stored);

	ଫେରନ୍ତୁ rc;
}


/* Remove a subhier element, and return its parent if that needs freeing as well. */
ସ୍ଥିର ସଂରଚନା mosquitto__subhier *tmp_remove_subs(ସଂରଚନା mosquitto__subhier *sub)
{
	ସଂରଚନା mosquitto__subhier *parent;

	ଯଦି(!sub || !sub->parent){
		ଫେରନ୍ତୁ ଖାଲି;
	}

	ଯଦି(sub->children || sub->subs){
		ଫେରନ୍ତୁ ଖାଲି;
	}

	parent = sub->parent;
	HASH_DELETE(hh, parent->children, sub);
	mosquitto_FREE(sub);

	ଯଦି(parent->subs == ଖାଲି
			&& parent->children == ଖାଲି
			&& parent->shared == ଖାଲି
			&& parent->parent){

		ଫେରନ୍ତୁ parent;
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ ଖାଲି;
	}
}


/* Remove all subscriptions for a client.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା sub__clean_session(ସଂରଚନା mosquitto *context)
{
	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<context->subs_capacity; i++){
		ଯଦି(context->subs[i] == ଖାଲି || context->subs[i]->hier == ଖାଲି){
			ଜାରି;
		}

		ସଂରଚନା mosquitto__subhier *hier = context->subs[i]->hier;
		ସଂରଚନା mosquitto__subleaf *leaf;

		plugin_persist__handle_subscription_delete(context, context->subs[i]->topic_filter);
		ଯଦି(context->subs[i]->shared){
			leaf = context->subs[i]->shared->subs;
			ଯେତେବେଳେ(leaf){
				ଯଦି(leaf->context==context){
#ifdef WITH_SYS_ବୃକ୍ଷ
					db.shared_subscription_ଗଣନା--;
#endif
					sub__remove_shared_leaf(context->subs[i]->hier, context->subs[i]->shared, leaf);
					ଭାଙ୍ଗ;
				}
				leaf = leaf->next;
			}
		}ଅନ୍ୟଥା{
			leaf = hier->subs;
			ଯେତେବେଳେ(leaf){
				ଯଦି(leaf->context==context){
#ifdef WITH_SYS_ବୃକ୍ଷ
					db.subscription_ଗଣନା--;
#endif
					DL_DELETE(hier->subs, leaf);
					ଭାଙ୍ଗ;
				}
				leaf = leaf->next;
			}
		}
		mosquitto_FREE(context->subs[i]);

		ଯଦି(hier->subs == ଖାଲି
				&& hier->children == ଖାଲି
				&& hier->shared == ଖାଲି
				&& hier->parent){

			କର{
				hier = tmp_remove_subs(hier);
			}ଯେତେବେଳେ(hier);
		}
	}
	mosquitto_FREE(context->subs);
	context->subs_capacity = 0;
	context->subs_ଗଣନା = 0;

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ଶୂନ୍ୟ sub_ବୃକ୍ଷ_print(ସଂରଚନା mosquitto__subhier *ମୂଳ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା level)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ସଂରଚନା mosquitto__subhier *branch, *branch_tmp;
	ସଂରଚନା mosquitto__subleaf *leaf;

	HASH_ITER(hh, ମୂଳ, branch, branch_tmp){
		ଯଦି(level > -1){
			ପାଇଁ(i=0; i<(level+2)*2; i++){
				printf(" ");
			}
			printf("%s", branch->topic);
			leaf = branch->subs;
			ଯେତେବେଳେ(leaf){
				ଯଦି(leaf->context){
					printf(" (%s, %d)", leaf->context->id, MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}ଅନ୍ୟଥା{
					printf(" (%s, %d)", "", MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}
				leaf = leaf->next;
			}
			printf("\n");
		}

		sub_ବୃକ୍ଷ_print(branch->children, level+1);
	}
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା sub__init(ଶୂନ୍ୟ)
{
	HASH_ମୂଲ୍ୟ("+", 1, hashv_plus);
	HASH_ମୂଲ୍ୟ("#", 1, hashv_hash);

	ଯଦି(sub__add_hier_entry(ଖାଲି, &db.shared_subs, "", 0) == ଖାଲି
			|| sub__add_hier_entry(ଖାଲି, &db.normal_subs, "", 0) == ଖାଲି
			|| sub__add_hier_entry(ଖାଲି, &db.normal_subs, "$SYS", (uint16_t)strlen("$SYS")) == ଖାଲି
			){

		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}
}
