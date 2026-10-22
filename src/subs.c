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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"

#ಅನ್ತರ್ಭಾವ "utlist.h"

ಸ್ಥಿರ ಸಂರಚನಾ mosquitto__subhier *sub__add_hier_entry(ಸಂರಚನಾ mosquitto__subhier *parent, ಸಂರಚನಾ mosquitto__subhier **sibling, ಧ್ರುವ ವರ್ಣ *topic, uint16_t len);

ಸ್ಥಿರ unsigned ಪೂರ್ಣಾಙ್ಕ hashv_plus = 0;
ಸ್ಥಿರ unsigned ಪೂರ್ಣಾಙ್ಕ hashv_hash = 0;


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ subs__send(ಸಂರಚನಾ mosquitto__subleaf *leaf, ಧ್ರುವ ವರ್ಣ *topic, uint8_t qos, ಪೂರ್ಣಾಙ್ಕ retain, ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *stored)
{
	bool client_retain;
	uint16_t mid;
	uint8_t client_qos, ಸಂದೇಶ_qos;
	ಪೂರ್ಣಾಙ್ಕ rc2;

	/* Check for ACL topic access. */
	rc2 = mosquitto_acl_check(leaf->context, topic, stored->data.payloadlen, stored->data.payload, stored->data.qos, stored->data.retain, stored->data.properties, MOSQ_ACL_READ);
	ಯದಿ(rc2 == MOSQ_ERR_ACL_DENIED){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}ಅನ್ಯಥಾ ಯದಿ(rc2 == MOSQ_ERR_SUCCESS){
		client_qos = MQTT_SUB_OPT_GET_QOS(leaf->subscription_options);

		ಯದಿ(db.config->upgrade_outgoing_qos){
			ಸಂದೇಶ_qos = client_qos;
		}ಅನ್ಯಥಾ{
			ಯದಿ(qos > client_qos){
				ಸಂದೇಶ_qos = client_qos;
			}ಅನ್ಯಥಾ{
				ಸಂದೇಶ_qos = qos;
			}
		}
		ಯದಿ(ಸಂದೇಶ_qos){
			mid = mosquitto__mid_generate(leaf->context);
		}ಅನ್ಯಥಾ{
			mid = 0;
		}
		ಯದಿ(MQTT_SUB_OPT_GET_RETAIN_AS_PUBLISHED(leaf->subscription_options)){
			client_retain = retain;
		}ಅನ್ಯಥಾ{
			client_retain = false;
		}
		ಯದಿ(db_message_ಸೇರಿಸು_outgoing(leaf->context, 0, mid, ಸಂದೇಶ_qos, client_retain, stored, leaf->identifier, true, true) == 1){
			ಪ್ರತ್ಯಾಗಮ 1;
		}
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ 1; /* Application error */
	}
	ಪ್ರತ್ಯಾಗಮ 0;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ subs__shared_process(ಸಂರಚನಾ mosquitto__subhier *hier, ಧ್ರುವ ವರ್ಣ *topic, uint8_t qos, ಪೂರ್ಣಾಙ್ಕ retain, ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *stored)
{
	ಪೂರ್ಣಾಙ್ಕ rc = 0, rc2;
	ಸಂರಚನಾ mosquitto__subshared *shared, *shared_tmp;
	ಸಂರಚನಾ mosquitto__subleaf *leaf;

	HASH_ITER(hh, hier->shared, shared, shared_tmp){
		leaf = shared->subs;
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		/* Remove current from the top, add back to the bottom */
		DL_DELETE(shared->subs, leaf);
		DL_APPEND(shared->subs, leaf);

		ಯದಿ(rc2){
			rc = 1;
		}
	}

	ಪ್ರತ್ಯಾಗಮ rc;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ subs__process(ಸಂರಚನಾ mosquitto__subhier *hier, ಧ್ರುವ ವರ್ಣ *source_id, ಧ್ರುವ ವರ್ಣ *topic, uint8_t qos, ಪೂರ್ಣಾಙ್ಕ retain, ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *stored)
{
	ಪೂರ್ಣಾಙ್ಕ rc = 0;
	ಪೂರ್ಣಾಙ್ಕ rc2;
	ಸಂರಚನಾ mosquitto__subleaf *leaf;

	rc = subs__shared_process(hier, topic, qos, retain, stored);

	leaf = hier->subs;
	ಯಾವತ್(source_id && leaf){
		ಯದಿ(!leaf->context->id || (MQTT_SUB_OPT_GET_NO_LOCAL(leaf->subscription_options) && !strcmp(leaf->context->id, source_id))){
			leaf = leaf->next;
			ಅನುವರ್ತ;
		}
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		ಯದಿ(rc2){
			rc = 1;
		}
		leaf = leaf->next;
	}
	ಯದಿ(hier->subs || hier->shared){
		ಪ್ರತ್ಯಾಗಮ rc;
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sub__add_leaf(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ಸಂರಚನಾ mosquitto_subscription *sub, ಸಂರಚನಾ mosquitto__subleaf **head, ಸಂರಚನಾ mosquitto__subleaf **newleaf)
{
	ಸಂರಚನಾ mosquitto__subleaf *leaf;

	*newleaf = ರಿಕ್ತ;
	leaf = *head;

	ಯಾವತ್(leaf){
		ಯದಿ(leaf->context && leaf->context->id && !strcmp(leaf->context->id, context->id)){
			/* Client making a second subscription to same topic. Only
			 * need to update QoS. Return MOSQ_ERR_SUB_EXISTS to
			 * indicate this to the calling function. */
			leaf->identifier = sub->identifier;
			leaf->subscription_options = sub->options;
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUB_EXISTS;
		}
		leaf = leaf->next;
	}
	leaf = mosquitto_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__subleaf) + strlen(sub->topic_filter) + 1);
	ಯದಿ(!leaf){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}
	leaf->context = context;
	leaf->identifier = sub->identifier;
	leaf->subscription_options = sub->options;
	strcpy(leaf->topic_filter, sub->topic_filter);

	DL_APPEND(*head, leaf);
	*newleaf = leaf;

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಶೂನ್ಯ sub__remove_shared_leaf(ಸಂರಚನಾ mosquitto__subhier *subhier, ಸಂರಚನಾ mosquitto__subshared *shared, ಸಂರಚನಾ mosquitto__subleaf *leaf)
{
	DL_DELETE(shared->subs, leaf);
	ಯದಿ(shared->subs == ರಿಕ್ತ){
		HASH_DELETE(hh, subhier->shared, shared);
		mosquitto_FREE(shared);
	}
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sub__add_shared(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ಸಂರಚನಾ mosquitto_subscription *sub, ಸಂರಚನಾ mosquitto__subhier *subhier, ಧ್ರುವ ವರ್ಣ *sharename)
{
	ಸಂರಚನಾ mosquitto__subleaf *newleaf;
	ಸಂರಚನಾ mosquitto__subshared *shared = ರಿಕ್ತ;
	ಸಂರಚನಾ mosquitto__subleaf **subs;
	size_t slen;
	ಪೂರ್ಣಾಙ್ಕ rc;
	unsigned hashv;

	slen = strlen(sharename);

	HASH_ಮೌಲ್ಯ(sharename, slen, hashv);

	HASH_FIND_BYHASHVALUE(hh, subhier->shared, sharename, slen, hashv, shared);
	ಯದಿ(shared == ರಿಕ್ತ){
		shared = mosquitto_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__subshared) + slen + 1);
		ಯದಿ(!shared){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}
		strncpy(shared->name, sharename, slen+1);

		HASH_ADD_BYHASHVALUE(hh, subhier->shared, name, slen, hashv, shared);
	}

	rc = sub__add_leaf(context, sub, &shared->subs, &newleaf);
	ಯದಿ(rc > 0){
		ಯದಿ(shared->subs == ರಿಕ್ತ){
			HASH_DELETE(hh, subhier->shared, shared);
			mosquitto_FREE(shared);
		}
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಯದಿ(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = shared;

		bool assigned = false;
		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<context->subs_capacity; i++){
			ಯದಿ(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_ಎಣಿಕೆ++;
				assigned = true;
				ಭಙ್ಗ;
			}
		}
		ಯದಿ(assigned == false){
			subs = mosquitto_realloc(context->subs, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__subleaf *)*(size_t)(context->subs_capacity + 1));
			ಯದಿ(!subs){
				sub__remove_shared_leaf(subhier, shared, newleaf);
				mosquitto_FREE(newleaf);
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_capacity++;
			context->subs_ಎಣಿಕೆ++;
			context->subs[context->subs_capacity-1] = newleaf;
		}
#ifdef WITH_SYS_ವೃಕ್ಷ
		db.shared_subscription_ಎಣಿಕೆ++;
#endif
	}

	ಯದಿ(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		ಪ್ರತ್ಯಾಗಮ rc;
	}ಅನ್ಯಥಾ{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sub__add_normal(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ಸಂರಚನಾ mosquitto_subscription *sub, ಸಂರಚನಾ mosquitto__subhier *subhier)
{
	ಸಂರಚನಾ mosquitto__subleaf *newleaf = ರಿಕ್ತ;
	ಸಂರಚನಾ mosquitto__subleaf **subs;
	ಪೂರ್ಣಾಙ್ಕ rc;

	rc = sub__add_leaf(context, sub, &subhier->subs, &newleaf);
	ಯದಿ(rc > 0){
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಯದಿ(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = ರಿಕ್ತ;

		bool assigned = false;
		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<context->subs_capacity; i++){
			ಯದಿ(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_ಎಣಿಕೆ++;
				assigned = true;
				ಭಙ್ಗ;
			}
		}
		ಯದಿ(assigned == false){
			subs = mosquitto_realloc(context->subs, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__subleaf *)*(size_t)(context->subs_capacity + 1));
			ಯದಿ(!subs){
				DL_DELETE(subhier->subs, newleaf);
				mosquitto_FREE(newleaf);
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_capacity++;
			context->subs_ಎಣಿಕೆ++;
			context->subs[context->subs_capacity-1] = newleaf;
		}
#ifdef WITH_SYS_ವೃಕ್ಷ
		db.subscription_ಎಣಿಕೆ++;
#endif
	}

	ಯದಿ(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		ಪ್ರತ್ಯಾಗಮ rc;
	}ಅನ್ಯಥಾ{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sub__add_context(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ಸಂರಚನಾ mosquitto_subscription *sub, ಸಂರಚನಾ mosquitto__subhier *subhier, ವರ್ಣ *ಧ್ರುವ *ಧ್ರುವ topics, ಧ್ರುವ ವರ್ಣ *sharename)
{
	ಸಂರಚನಾ mosquitto__subhier *branch;
	ಪೂರ್ಣಾಙ್ಕ topic_index = 0;
	size_t topiclen;

	/* Find leaf node */
	ಯಾವತ್(topics && topics[topic_index] != ರಿಕ್ತ){
		topiclen = strlen(topics[topic_index]);
		ಯದಿ(topiclen > UINT16_MAX){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
		HASH_FIND(hh, subhier->children, topics[topic_index], topiclen, branch);
		ಯದಿ(!branch){
			/* Not found */
			branch = sub__add_hier_entry(subhier, &subhier->children, topics[topic_index], (uint16_t)topiclen);
			ಯದಿ(!branch){
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
			}
		}
		subhier = branch;
		topic_index++;
	}

	/* Add add our context */
	ಯದಿ(context && context->id){
		ಯದಿ(sharename){
			ಪ್ರತ್ಯಾಗಮ sub__add_shared(context, sub, subhier, sharename);
		}ಅನ್ಯಥಾ{
			ಪ್ರತ್ಯಾಗಮ sub__add_normal(context, sub, subhier);
		}
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sub__remove_normal(ಸಂರಚನಾ mosquitto *context, ಸಂರಚನಾ mosquitto__subhier *subhier, uint8_t *reason)
{
	ಸಂರಚನಾ mosquitto__subleaf *leaf;

	leaf = subhier->subs;
	ಯಾವತ್(leaf){
		ಯದಿ(leaf->context==context){
#ifdef WITH_SYS_ವೃಕ್ಷ
			db.subscription_ಎಣಿಕೆ--;
#endif
			DL_DELETE(subhier->subs, leaf);

			/* Remove the reference to the sub that the client is keeping.
			 * It would be nice to be able to use the reference directly,
			 * but that would involve keeping a copy of the topic string in
			 * each subleaf. Might be worth considering though. */
			ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<context->subs_capacity; i++){
				ಯದಿ(context->subs[i] && context->subs[i]->hier == subhier){
					context->subs_ಎಣಿಕೆ--;
					mosquitto_free(context->subs[i]);
					context->subs[i] = ರಿಕ್ತ;
					ಭಙ್ಗ;
				}
			}
			*reason = 0;
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
		}
		leaf = leaf->next;
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NO_SUBSCRIBERS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sub__remove_shared(ಸಂರಚನಾ mosquitto *context, ಸಂರಚನಾ mosquitto__subhier *subhier, uint8_t *reason, ಧ್ರುವ ವರ್ಣ *sharename)
{
	ಸಂರಚನಾ mosquitto__subshared *shared;

	HASH_FIND(hh, subhier->shared, sharename, strlen(sharename), shared);
	ಯದಿ(shared){
		ಸಂರಚನಾ mosquitto__subleaf *leaf = shared->subs;
		ಯಾವತ್(leaf){
			ಯದಿ(leaf->context==context){
#ifdef WITH_SYS_ವೃಕ್ಷ
				db.shared_subscription_ಎಣಿಕೆ--;
#endif
				DL_DELETE(shared->subs, leaf);

				/* Remove the reference to the sub that the client is keeping.
				* It would be nice to be able to use the reference directly,
				* but that would involve keeping a copy of the topic string in
				* each subleaf. Might be worth considering though. */
				ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<context->subs_capacity; i++){
					ಯದಿ(context->subs[i]
							&& context->subs[i]->hier == subhier
							&& context->subs[i]->shared == shared){

						mosquitto_free(context->subs[i]);
						context->subs[i] = ರಿಕ್ತ;
						context->subs_ಎಣಿಕೆ--;
						ಭಙ್ಗ;
					}
				}

				ಯದಿ(shared->subs == ರಿಕ್ತ){
					HASH_DELETE(hh, subhier->shared, shared);
					mosquitto_FREE(shared);
				}

				*reason = 0;
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
			}
			leaf = leaf->next;
		}
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NO_SUBSCRIBERS;
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sub__remove_recurse(ಸಂರಚನಾ mosquitto *context, ಸಂರಚನಾ mosquitto__subhier *subhier, ವರ್ಣ **topics, uint8_t *reason, ಧ್ರುವ ವರ್ಣ *sharename)
{
	ಸಂರಚನಾ mosquitto__subhier *branch;

	ಯದಿ(topics == ರಿಕ್ತ || topics[0] == ರಿಕ್ತ){
		ಯದಿ(sharename){
			ಪ್ರತ್ಯಾಗಮ sub__remove_shared(context, subhier, reason, sharename);
		}ಅನ್ಯಥಾ{
			ಪ್ರತ್ಯಾಗಮ sub__remove_normal(context, subhier, reason);
		}
	}

	HASH_FIND(hh, subhier->children, topics[0], strlen(topics[0]), branch);
	ಯದಿ(branch){
		sub__remove_recurse(context, branch, &(topics[1]), reason, sharename);
		ಯದಿ(!branch->children && !branch->subs && !branch->shared){
			HASH_DELETE(hh, subhier->children, branch);
			mosquitto_FREE(branch);
		}
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sub_ಹುಡುಕು(ಸಂರಚನಾ mosquitto__subhier *subhier, ವರ್ಣ **split_topics, ಧ್ರುವ ವರ್ಣ *source_id, ಧ್ರುವ ವರ್ಣ *topic, uint8_t qos, ಪೂರ್ಣಾಙ್ಕ retain, ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *stored)
{
	/* FIXME - need to take into account source_id if the client is a bridge */
	ಸಂರಚನಾ mosquitto__subhier *branch;
	ಪೂರ್ಣಾಙ್ಕ rc;
	bool have_subscribers = false;

	ಯದಿ(split_topics && split_topics[0]){
		/* Check for literal match */
		HASH_FIND(hh, subhier->children, split_topics[0], strlen(split_topics[0]), branch);

		ಯದಿ(branch){
			rc = sub_ಹುಡುಕು(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			ಯದಿ(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}ಅನ್ಯಥಾ ಯದಿ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				ಪ್ರತ್ಯಾಗಮ rc;
			}
			ಯದಿ(split_topics[1] == ರಿಕ್ತ){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				ಯದಿ(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}ಅನ್ಯಥಾ ಯದಿ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					ಪ್ರತ್ಯಾಗಮ rc;
				}
			}
		}

		/* Check for + match */
		HASH_FIND_BYHASHVALUE(hh, subhier->children, "+", 1, hashv_plus, branch);

		ಯದಿ(branch){
			rc = sub_ಹುಡುಕು(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			ಯದಿ(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}ಅನ್ಯಥಾ ಯದಿ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				ಪ್ರತ್ಯಾಗಮ rc;
			}
			ಯದಿ(split_topics[1] == ರಿಕ್ತ){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				ಯದಿ(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}ಅನ್ಯಥಾ ಯದಿ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					ಪ್ರತ್ಯಾಗಮ rc;
				}
			}
		}
	}

	/* Check for # match */
	HASH_FIND_BYHASHVALUE(hh, subhier->children, "#", 1, hashv_hash, branch);
	ಯದಿ(branch && !branch->children){
		/* The topic matches due to a # wildcard - process the
		 * subscriptions but *don't* return. Although this branch has ended
		 * there may still be other subscriptions to deal with.
		 */
		rc = subs__process(branch, source_id, topic, qos, retain, stored);
		ಯದಿ(rc == MOSQ_ERR_SUCCESS){
			have_subscribers = true;
		}ಅನ್ಯಥಾ ಯದಿ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}

	ಯದಿ(have_subscribers){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


ಸ್ಥಿರ ಸಂರಚನಾ mosquitto__subhier *sub__add_hier_entry(ಸಂರಚನಾ mosquitto__subhier *parent, ಸಂರಚನಾ mosquitto__subhier **sibling, ಧ್ರುವ ವರ್ಣ *topic, uint16_t len)
{
	ಸಂರಚನಾ mosquitto__subhier *child;

	ಪ್ರತಿಜ್ಞಾ(sibling);

	child = mosquitto_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__subhier) + len + 1);
	ಯದಿ(!child){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	child->parent = parent;
	child->topic_len = len;
	ಯದಿ(len > 0){
		strncpy(child->topic, topic, (size_t)(len+1));
	}

	HASH_ADD(hh, *sibling, topic, child->topic_len, child);

	ಪ್ರತ್ಯಾಗಮ child;
}


ಪೂರ್ಣಾಙ್ಕ sub__add(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ಸಂರಚನಾ mosquitto_subscription *sub)
{
	ಪೂರ್ಣಾಙ್ಕ rc = 0;
	ಸಂರಚನಾ mosquitto__subhier *subhier;
	ಧ್ರುವ ವರ್ಣ *sharename = ರಿಕ್ತ;
	ವರ್ಣ *local_sub;
	ವರ್ಣ **topics;
	size_t topiclen;

	ಪ್ರತಿಜ್ಞಾ(sub);
	ಪ್ರತಿಜ್ಞಾ(sub->topic_filter);

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &topics, &sharename);
	ಯದಿ(rc){
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	topiclen = strlen(topics[0]);
	ಯದಿ(topiclen > UINT16_MAX){
		mosquitto_FREE(local_sub);
		mosquitto_FREE(topics);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	ಯದಿ(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], topiclen, subhier);
		ಯದಿ(!subhier){
			subhier = sub__add_hier_entry(ರಿಕ್ತ, &db.shared_subs, topics[0], (uint16_t)topiclen);
			ಯದಿ(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
			}
		}
	}ಅನ್ಯಥಾ{
		HASH_FIND(hh, db.normal_subs, topics[0], topiclen, subhier);
		ಯದಿ(!subhier){
			subhier = sub__add_hier_entry(ರಿಕ್ತ, &db.normal_subs, topics[0], (uint16_t)topiclen);
			ಯದಿ(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
			}
		}
	}
	rc = sub__add_context(context, sub, subhier, topics, sharename);

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	ಪ್ರತ್ಯಾಗಮ rc;
}


ಪೂರ್ಣಾಙ್ಕ sub__remove(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ವರ್ಣ *sub, uint8_t *reason)
{
	ಪೂರ್ಣಾಙ್ಕ rc = 0;
	ಸಂರಚನಾ mosquitto__subhier *subhier;
	ಧ್ರುವ ವರ್ಣ *sharename = ರಿಕ್ತ;
	ವರ್ಣ *local_sub = ರಿಕ್ತ;
	ವರ್ಣ **topics = ರಿಕ್ತ;

	ಪ್ರತಿಜ್ಞಾ(sub);

	rc = sub__topic_tokenise(sub, &local_sub, &topics, &sharename);
	ಯದಿ(rc){
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಯದಿ(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], strlen(topics[0]), subhier);
	}ಅನ್ಯಥಾ{
		HASH_FIND(hh, db.normal_subs, topics[0], strlen(topics[0]), subhier);
	}
	ಯದಿ(subhier){
		*reason = MQTT_RC_NO_SUBSCRIPTION_EXISTED;
		rc = sub__remove_recurse(context, subhier, topics, reason, sharename);
	}

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	ಪ್ರತ್ಯಾಗಮ rc;
}


ಪೂರ್ಣಾಙ್ಕ sub__messages_queue(ಧ್ರುವ ವರ್ಣ *source_id, uint8_t qos, ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ **stored)
{
	ಪೂರ್ಣಾಙ್ಕ rc = MOSQ_ERR_SUCCESS, rc2;
	ಪೂರ್ಣಾಙ್ಕ rc_normal = MOSQ_ERR_NO_SUBSCRIBERS, rc_shared = MOSQ_ERR_NO_SUBSCRIBERS;
	ಸಂರಚನಾ mosquitto__subhier *subhier;
	ವರ್ಣ **split_topics = ರಿಕ್ತ;
	ವರ್ಣ *local_topic = ರಿಕ್ತ;
	unsigned hashv;
	size_t topiclen;
	bool retain = (*stored)->data.retain;
	ಧ್ರುವ ವರ್ಣ *topic = (*stored)->data.topic;

	ಪ್ರತಿಜ್ಞಾ(topic);

	ಯದಿ(sub__topic_tokenise(topic, &local_topic, &split_topics, ರಿಕ್ತ)){
		ಪ್ರತ್ಯಾಗಮ 1;
	}

	/* Protect this message until we have sent it to all
	clients - this is required because websockets client calls
	db__message_write(), which could remove the message if ref_count==0.
	*/
	db_ಸಂದೇಶ_store_ref_inc(*stored);

	topiclen = strlen(split_topics[0]);
	HASH_ಮೌಲ್ಯ(split_topics[0], topiclen, hashv);
	HASH_FIND_BYHASHVALUE(hh, db.normal_subs, split_topics[0], topiclen, hashv, subhier);
	ಯದಿ(subhier){
		rc_normal = sub_ಹುಡುಕು(subhier, split_topics, source_id, topic, qos, retain, *stored);
		ಯದಿ(rc_normal > 0){
			rc = rc_normal;
			ಗಚ್ಛ end;
		}
	}

	HASH_FIND_BYHASHVALUE(hh, db.shared_subs, split_topics[0], topiclen, hashv, subhier);
	ಯದಿ(subhier){
		rc_shared = sub_ಹುಡುಕು(subhier, split_topics, source_id, topic, qos, retain, *stored);
		ಯದಿ(rc_shared > 0){
			rc = rc_shared;
			ಗಚ್ಛ end;
		}
	}

	ಯದಿ(rc_normal == MOSQ_ERR_NO_SUBSCRIBERS && rc_shared == MOSQ_ERR_NO_SUBSCRIBERS){
		rc = MOSQ_ERR_NO_SUBSCRIBERS;
	}

	ಯದಿ(retain){
		rc2 = retain__store(topic, *stored, split_topics, true);
		ಯದಿ(rc2){
			rc = rc2;
		}
	}

end:
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);
	/* Remove our reference and free if needed. */
	db_ಸಂದೇಶ_store_ref_dec(stored);

	ಪ್ರತ್ಯಾಗಮ rc;
}


/* Remove a subhier element, and return its parent if that needs freeing as well. */
ಸ್ಥಿರ ಸಂರಚನಾ mosquitto__subhier *tmp_remove_subs(ಸಂರಚನಾ mosquitto__subhier *sub)
{
	ಸಂರಚನಾ mosquitto__subhier *parent;

	ಯದಿ(!sub || !sub->parent){
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	ಯದಿ(sub->children || sub->subs){
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	parent = sub->parent;
	HASH_DELETE(hh, parent->children, sub);
	mosquitto_FREE(sub);

	ಯದಿ(parent->subs == ರಿಕ್ತ
			&& parent->children == ರಿಕ್ತ
			&& parent->shared == ರಿಕ್ತ
			&& parent->parent){

		ಪ್ರತ್ಯಾಗಮ parent;
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
}


/* Remove all subscriptions for a client.
 */
ಪೂರ್ಣಾಙ್ಕ sub__clean_session(ಸಂರಚನಾ mosquitto *context)
{
	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<context->subs_capacity; i++){
		ಯದಿ(context->subs[i] == ರಿಕ್ತ || context->subs[i]->hier == ರಿಕ್ತ){
			ಅನುವರ್ತ;
		}

		ಸಂರಚನಾ mosquitto__subhier *hier = context->subs[i]->hier;
		ಸಂರಚನಾ mosquitto__subleaf *leaf;

		plugin_persist__handle_subscription_delete(context, context->subs[i]->topic_filter);
		ಯದಿ(context->subs[i]->shared){
			leaf = context->subs[i]->shared->subs;
			ಯಾವತ್(leaf){
				ಯದಿ(leaf->context==context){
#ifdef WITH_SYS_ವೃಕ್ಷ
					db.shared_subscription_ಎಣಿಕೆ--;
#endif
					sub__remove_shared_leaf(context->subs[i]->hier, context->subs[i]->shared, leaf);
					ಭಙ್ಗ;
				}
				leaf = leaf->next;
			}
		}ಅನ್ಯಥಾ{
			leaf = hier->subs;
			ಯಾವತ್(leaf){
				ಯದಿ(leaf->context==context){
#ifdef WITH_SYS_ವೃಕ್ಷ
					db.subscription_ಎಣಿಕೆ--;
#endif
					DL_DELETE(hier->subs, leaf);
					ಭಙ್ಗ;
				}
				leaf = leaf->next;
			}
		}
		mosquitto_FREE(context->subs[i]);

		ಯದಿ(hier->subs == ರಿಕ್ತ
				&& hier->children == ರಿಕ್ತ
				&& hier->shared == ರಿಕ್ತ
				&& hier->parent){

			ಕುರು{
				hier = tmp_remove_subs(hier);
			}ಯಾವತ್(hier);
		}
	}
	mosquitto_FREE(context->subs);
	context->subs_capacity = 0;
	context->subs_ಎಣಿಕೆ = 0;

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಶೂನ್ಯ sub_ವೃಕ್ಷ_print(ಸಂರಚನಾ mosquitto__subhier *ಬೇರು, ಪೂರ್ಣಾಙ್ಕ level)
{
	ಪೂರ್ಣಾಙ್ಕ i;
	ಸಂರಚನಾ mosquitto__subhier *branch, *branch_tmp;
	ಸಂರಚನಾ mosquitto__subleaf *leaf;

	HASH_ITER(hh, ಬೇರು, branch, branch_tmp){
		ಯದಿ(level > -1){
			ಪ್ರತಿ(i=0; i<(level+2)*2; i++){
				printf(" ");
			}
			printf("%s", branch->topic);
			leaf = branch->subs;
			ಯಾವತ್(leaf){
				ಯದಿ(leaf->context){
					printf(" (%s, %d)", leaf->context->id, MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}ಅನ್ಯಥಾ{
					printf(" (%s, %d)", "", MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}
				leaf = leaf->next;
			}
			printf("\n");
		}

		sub_ವೃಕ್ಷ_print(branch->children, level+1);
	}
}


ಪೂರ್ಣಾಙ್ಕ sub__init(ಶೂನ್ಯ)
{
	HASH_ಮೌಲ್ಯ("+", 1, hashv_plus);
	HASH_ಮೌಲ್ಯ("#", 1, hashv_hash);

	ಯದಿ(sub__add_hier_entry(ರಿಕ್ತ, &db.shared_subs, "", 0) == ರಿಕ್ತ
			|| sub__add_hier_entry(ರಿಕ್ತ, &db.normal_subs, "", 0) == ರಿಕ್ತ
			|| sub__add_hier_entry(ರಿಕ್ತ, &db.normal_subs, "$SYS", (uint16_t)strlen("$SYS")) == ರಿಕ್ತ
			){

		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}
}
