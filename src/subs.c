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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/mqtt_protocol.h"
#അന്തര്ഭാവ "util_mosq.h"

#അന്തര്ഭാവ "utlist.h"

സ്ഥിതം ഘടന mosquitto__subhier *sub__add_hier_entry(ഘടന mosquitto__subhier *parent, ഘടന mosquitto__subhier **sibling, സ്ഥിരം അക്ഷരം *topic, uint16_t len);

സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ hashv_plus = 0;
സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ hashv_hash = 0;


സ്ഥിതം പൂർണ്ണസംഖ്യ subs__send(ഘടന mosquitto__subleaf *leaf, സ്ഥിരം അക്ഷരം *topic, uint8_t qos, പൂർണ്ണസംഖ്യ retain, ഘടന mosquitto_base_സന്ദേശം *stored)
{
	bool client_retain;
	uint16_t mid;
	uint8_t client_qos, സന്ദേശം_qos;
	പൂർണ്ണസംഖ്യ rc2;

	/* Check for ACL topic access. */
	rc2 = mosquitto_acl_check(leaf->context, topic, stored->data.payloadlen, stored->data.payload, stored->data.qos, stored->data.retain, stored->data.properties, MOSQ_ACL_READ);
	എങ്കിൽ(rc2 == MOSQ_ERR_ACL_DENIED){
		തിരികെ MOSQ_ERR_SUCCESS;
	}അല്ലെങ്കിൽ എങ്കിൽ(rc2 == MOSQ_ERR_SUCCESS){
		client_qos = MQTT_SUB_OPT_GET_QOS(leaf->subscription_options);

		എങ്കിൽ(db.config->upgrade_outgoing_qos){
			സന്ദേശം_qos = client_qos;
		}അല്ലെങ്കിൽ{
			എങ്കിൽ(qos > client_qos){
				സന്ദേശം_qos = client_qos;
			}അല്ലെങ്കിൽ{
				സന്ദേശം_qos = qos;
			}
		}
		എങ്കിൽ(സന്ദേശം_qos){
			mid = mosquitto__mid_generate(leaf->context);
		}അല്ലെങ്കിൽ{
			mid = 0;
		}
		എങ്കിൽ(MQTT_SUB_OPT_GET_RETAIN_AS_PUBLISHED(leaf->subscription_options)){
			client_retain = retain;
		}അല്ലെങ്കിൽ{
			client_retain = false;
		}
		എങ്കിൽ(db_message_തിരുകുക_outgoing(leaf->context, 0, mid, സന്ദേശം_qos, client_retain, stored, leaf->identifier, true, true) == 1){
			തിരികെ 1;
		}
	}അല്ലെങ്കിൽ{
		തിരികെ 1; /* Application error */
	}
	തിരികെ 0;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ subs__shared_process(ഘടന mosquitto__subhier *hier, സ്ഥിരം അക്ഷരം *topic, uint8_t qos, പൂർണ്ണസംഖ്യ retain, ഘടന mosquitto_base_സന്ദേശം *stored)
{
	പൂർണ്ണസംഖ്യ rc = 0, rc2;
	ഘടന mosquitto__subshared *shared, *shared_tmp;
	ഘടന mosquitto__subleaf *leaf;

	HASH_ITER(hh, hier->shared, shared, shared_tmp){
		leaf = shared->subs;
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		/* Remove current from the top, add back to the bottom */
		DL_DELETE(shared->subs, leaf);
		DL_APPEND(shared->subs, leaf);

		എങ്കിൽ(rc2){
			rc = 1;
		}
	}

	തിരികെ rc;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ subs__process(ഘടന mosquitto__subhier *hier, സ്ഥിരം അക്ഷരം *source_id, സ്ഥിരം അക്ഷരം *topic, uint8_t qos, പൂർണ്ണസംഖ്യ retain, ഘടന mosquitto_base_സന്ദേശം *stored)
{
	പൂർണ്ണസംഖ്യ rc = 0;
	പൂർണ്ണസംഖ്യ rc2;
	ഘടന mosquitto__subleaf *leaf;

	rc = subs__shared_process(hier, topic, qos, retain, stored);

	leaf = hier->subs;
	ആകുമ്പോൾ(source_id && leaf){
		എങ്കിൽ(!leaf->context->id || (MQTT_SUB_OPT_GET_NO_LOCAL(leaf->subscription_options) && !strcmp(leaf->context->id, source_id))){
			leaf = leaf->next;
			തുടരുക;
		}
		rc2 = subs__send(leaf, topic, qos, retain, stored);
		എങ്കിൽ(rc2){
			rc = 1;
		}
		leaf = leaf->next;
	}
	എങ്കിൽ(hier->subs || hier->shared){
		തിരികെ rc;
	}അല്ലെങ്കിൽ{
		തിരികെ MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


സ്ഥിതം പൂർണ്ണസംഖ്യ sub__add_leaf(ഘടന mosquitto *context, സ്ഥിരം ഘടന mosquitto_subscription *sub, ഘടന mosquitto__subleaf **head, ഘടന mosquitto__subleaf **newleaf)
{
	ഘടന mosquitto__subleaf *leaf;

	*newleaf = ശൂന്യ;
	leaf = *head;

	ആകുമ്പോൾ(leaf){
		എങ്കിൽ(leaf->context && leaf->context->id && !strcmp(leaf->context->id, context->id)){
			/* Client making a second subscription to same topic. Only
			 * need to update QoS. Return MOSQ_ERR_SUB_EXISTS to
			 * indicate this to the calling function. */
			leaf->identifier = sub->identifier;
			leaf->subscription_options = sub->options;
			തിരികെ MOSQ_ERR_SUB_EXISTS;
		}
		leaf = leaf->next;
	}
	leaf = mosquitto_calloc(1, വലുപ്പം(ഘടന mosquitto__subleaf) + strlen(sub->topic_filter) + 1);
	എങ്കിൽ(!leaf){
		തിരികെ MOSQ_ERR_NOMEM;
	}
	leaf->context = context;
	leaf->identifier = sub->identifier;
	leaf->subscription_options = sub->options;
	strcpy(leaf->topic_filter, sub->topic_filter);

	DL_APPEND(*head, leaf);
	*newleaf = leaf;

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം ശൂന്യം sub__remove_shared_leaf(ഘടന mosquitto__subhier *subhier, ഘടന mosquitto__subshared *shared, ഘടന mosquitto__subleaf *leaf)
{
	DL_DELETE(shared->subs, leaf);
	എങ്കിൽ(shared->subs == ശൂന്യ){
		HASH_DELETE(hh, subhier->shared, shared);
		mosquitto_FREE(shared);
	}
}


സ്ഥിതം പൂർണ്ണസംഖ്യ sub__add_shared(ഘടന mosquitto *context, സ്ഥിരം ഘടന mosquitto_subscription *sub, ഘടന mosquitto__subhier *subhier, സ്ഥിരം അക്ഷരം *sharename)
{
	ഘടന mosquitto__subleaf *newleaf;
	ഘടന mosquitto__subshared *shared = ശൂന്യ;
	ഘടന mosquitto__subleaf **subs;
	size_t slen;
	പൂർണ്ണസംഖ്യ rc;
	unsigned hashv;

	slen = strlen(sharename);

	HASH_മൂല്യം(sharename, slen, hashv);

	HASH_FIND_BYHASHVALUE(hh, subhier->shared, sharename, slen, hashv, shared);
	എങ്കിൽ(shared == ശൂന്യ){
		shared = mosquitto_calloc(1, വലുപ്പം(ഘടന mosquitto__subshared) + slen + 1);
		എങ്കിൽ(!shared){
			തിരികെ MOSQ_ERR_NOMEM;
		}
		strncpy(shared->name, sharename, slen+1);

		HASH_ADD_BYHASHVALUE(hh, subhier->shared, name, slen, hashv, shared);
	}

	rc = sub__add_leaf(context, sub, &shared->subs, &newleaf);
	എങ്കിൽ(rc > 0){
		എങ്കിൽ(shared->subs == ശൂന്യ){
			HASH_DELETE(hh, subhier->shared, shared);
			mosquitto_FREE(shared);
		}
		തിരികെ rc;
	}

	എങ്കിൽ(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = shared;

		bool assigned = false;
		വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<context->subs_capacity; i++){
			എങ്കിൽ(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_എണ്ണം++;
				assigned = true;
				നിർത്തുക;
			}
		}
		എങ്കിൽ(assigned == false){
			subs = mosquitto_realloc(context->subs, വലുപ്പം(ഘടന mosquitto__subleaf *)*(size_t)(context->subs_capacity + 1));
			എങ്കിൽ(!subs){
				sub__remove_shared_leaf(subhier, shared, newleaf);
				mosquitto_FREE(newleaf);
				തിരികെ MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_capacity++;
			context->subs_എണ്ണം++;
			context->subs[context->subs_capacity-1] = newleaf;
		}
#ifdef WITH_SYS_വൃക്ഷം
		db.shared_subscription_എണ്ണം++;
#endif
	}

	എങ്കിൽ(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		തിരികെ rc;
	}അല്ലെങ്കിൽ{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		തിരികെ MOSQ_ERR_SUCCESS;
	}
}


സ്ഥിതം പൂർണ്ണസംഖ്യ sub__add_normal(ഘടന mosquitto *context, സ്ഥിരം ഘടന mosquitto_subscription *sub, ഘടന mosquitto__subhier *subhier)
{
	ഘടന mosquitto__subleaf *newleaf = ശൂന്യ;
	ഘടന mosquitto__subleaf **subs;
	പൂർണ്ണസംഖ്യ rc;

	rc = sub__add_leaf(context, sub, &subhier->subs, &newleaf);
	എങ്കിൽ(rc > 0){
		തിരികെ rc;
	}

	എങ്കിൽ(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = ശൂന്യ;

		bool assigned = false;
		വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<context->subs_capacity; i++){
			എങ്കിൽ(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_എണ്ണം++;
				assigned = true;
				നിർത്തുക;
			}
		}
		എങ്കിൽ(assigned == false){
			subs = mosquitto_realloc(context->subs, വലുപ്പം(ഘടന mosquitto__subleaf *)*(size_t)(context->subs_capacity + 1));
			എങ്കിൽ(!subs){
				DL_DELETE(subhier->subs, newleaf);
				mosquitto_FREE(newleaf);
				തിരികെ MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_capacity++;
			context->subs_എണ്ണം++;
			context->subs[context->subs_capacity-1] = newleaf;
		}
#ifdef WITH_SYS_വൃക്ഷം
		db.subscription_എണ്ണം++;
#endif
	}

	എങ്കിൽ(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		തിരികെ rc;
	}അല്ലെങ്കിൽ{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		തിരികെ MOSQ_ERR_SUCCESS;
	}
}


സ്ഥിതം പൂർണ്ണസംഖ്യ sub__add_context(ഘടന mosquitto *context, സ്ഥിരം ഘടന mosquitto_subscription *sub, ഘടന mosquitto__subhier *subhier, അക്ഷരം *സ്ഥിരം *സ്ഥിരം topics, സ്ഥിരം അക്ഷരം *sharename)
{
	ഘടന mosquitto__subhier *branch;
	പൂർണ്ണസംഖ്യ topic_index = 0;
	size_t topiclen;

	/* Find leaf node */
	ആകുമ്പോൾ(topics && topics[topic_index] != ശൂന്യ){
		topiclen = strlen(topics[topic_index]);
		എങ്കിൽ(topiclen > UINT16_MAX){
			തിരികെ MOSQ_ERR_INVAL;
		}
		HASH_FIND(hh, subhier->children, topics[topic_index], topiclen, branch);
		എങ്കിൽ(!branch){
			/* Not found */
			branch = sub__add_hier_entry(subhier, &subhier->children, topics[topic_index], (uint16_t)topiclen);
			എങ്കിൽ(!branch){
				തിരികെ MOSQ_ERR_NOMEM;
			}
		}
		subhier = branch;
		topic_index++;
	}

	/* Add add our context */
	എങ്കിൽ(context && context->id){
		എങ്കിൽ(sharename){
			തിരികെ sub__add_shared(context, sub, subhier, sharename);
		}അല്ലെങ്കിൽ{
			തിരികെ sub__add_normal(context, sub, subhier);
		}
	}അല്ലെങ്കിൽ{
		തിരികെ MOSQ_ERR_SUCCESS;
	}
}


സ്ഥിതം പൂർണ്ണസംഖ്യ sub__remove_normal(ഘടന mosquitto *context, ഘടന mosquitto__subhier *subhier, uint8_t *reason)
{
	ഘടന mosquitto__subleaf *leaf;

	leaf = subhier->subs;
	ആകുമ്പോൾ(leaf){
		എങ്കിൽ(leaf->context==context){
#ifdef WITH_SYS_വൃക്ഷം
			db.subscription_എണ്ണം--;
#endif
			DL_DELETE(subhier->subs, leaf);

			/* Remove the reference to the sub that the client is keeping.
			 * It would be nice to be able to use the reference directly,
			 * but that would involve keeping a copy of the topic string in
			 * each subleaf. Might be worth considering though. */
			വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<context->subs_capacity; i++){
				എങ്കിൽ(context->subs[i] && context->subs[i]->hier == subhier){
					context->subs_എണ്ണം--;
					mosquitto_free(context->subs[i]);
					context->subs[i] = ശൂന്യ;
					നിർത്തുക;
				}
			}
			*reason = 0;
			തിരികെ MOSQ_ERR_SUCCESS;
		}
		leaf = leaf->next;
	}
	തിരികെ MOSQ_ERR_NO_SUBSCRIBERS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ sub__remove_shared(ഘടന mosquitto *context, ഘടന mosquitto__subhier *subhier, uint8_t *reason, സ്ഥിരം അക്ഷരം *sharename)
{
	ഘടന mosquitto__subshared *shared;

	HASH_FIND(hh, subhier->shared, sharename, strlen(sharename), shared);
	എങ്കിൽ(shared){
		ഘടന mosquitto__subleaf *leaf = shared->subs;
		ആകുമ്പോൾ(leaf){
			എങ്കിൽ(leaf->context==context){
#ifdef WITH_SYS_വൃക്ഷം
				db.shared_subscription_എണ്ണം--;
#endif
				DL_DELETE(shared->subs, leaf);

				/* Remove the reference to the sub that the client is keeping.
				* It would be nice to be able to use the reference directly,
				* but that would involve keeping a copy of the topic string in
				* each subleaf. Might be worth considering though. */
				വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<context->subs_capacity; i++){
					എങ്കിൽ(context->subs[i]
							&& context->subs[i]->hier == subhier
							&& context->subs[i]->shared == shared){

						mosquitto_free(context->subs[i]);
						context->subs[i] = ശൂന്യ;
						context->subs_എണ്ണം--;
						നിർത്തുക;
					}
				}

				എങ്കിൽ(shared->subs == ശൂന്യ){
					HASH_DELETE(hh, subhier->shared, shared);
					mosquitto_FREE(shared);
				}

				*reason = 0;
				തിരികെ MOSQ_ERR_SUCCESS;
			}
			leaf = leaf->next;
		}
		തിരികെ MOSQ_ERR_NO_SUBSCRIBERS;
	}അല്ലെങ്കിൽ{
		തിരികെ MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


സ്ഥിതം പൂർണ്ണസംഖ്യ sub__remove_recurse(ഘടന mosquitto *context, ഘടന mosquitto__subhier *subhier, അക്ഷരം **topics, uint8_t *reason, സ്ഥിരം അക്ഷരം *sharename)
{
	ഘടന mosquitto__subhier *branch;

	എങ്കിൽ(topics == ശൂന്യ || topics[0] == ശൂന്യ){
		എങ്കിൽ(sharename){
			തിരികെ sub__remove_shared(context, subhier, reason, sharename);
		}അല്ലെങ്കിൽ{
			തിരികെ sub__remove_normal(context, subhier, reason);
		}
	}

	HASH_FIND(hh, subhier->children, topics[0], strlen(topics[0]), branch);
	എങ്കിൽ(branch){
		sub__remove_recurse(context, branch, &(topics[1]), reason, sharename);
		എങ്കിൽ(!branch->children && !branch->subs && !branch->shared){
			HASH_DELETE(hh, subhier->children, branch);
			mosquitto_FREE(branch);
		}
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ sub_തിരയൽ(ഘടന mosquitto__subhier *subhier, അക്ഷരം **split_topics, സ്ഥിരം അക്ഷരം *source_id, സ്ഥിരം അക്ഷരം *topic, uint8_t qos, പൂർണ്ണസംഖ്യ retain, ഘടന mosquitto_base_സന്ദേശം *stored)
{
	/* FIXME - need to take into account source_id if the client is a bridge */
	ഘടന mosquitto__subhier *branch;
	പൂർണ്ണസംഖ്യ rc;
	bool have_subscribers = false;

	എങ്കിൽ(split_topics && split_topics[0]){
		/* Check for literal match */
		HASH_FIND(hh, subhier->children, split_topics[0], strlen(split_topics[0]), branch);

		എങ്കിൽ(branch){
			rc = sub_തിരയൽ(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			എങ്കിൽ(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}അല്ലെങ്കിൽ എങ്കിൽ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				തിരികെ rc;
			}
			എങ്കിൽ(split_topics[1] == ശൂന്യ){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				എങ്കിൽ(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}അല്ലെങ്കിൽ എങ്കിൽ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					തിരികെ rc;
				}
			}
		}

		/* Check for + match */
		HASH_FIND_BYHASHVALUE(hh, subhier->children, "+", 1, hashv_plus, branch);

		എങ്കിൽ(branch){
			rc = sub_തിരയൽ(branch, &(split_topics[1]), source_id, topic, qos, retain, stored);
			എങ്കിൽ(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}അല്ലെങ്കിൽ എങ്കിൽ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				തിരികെ rc;
			}
			എങ്കിൽ(split_topics[1] == ശൂന്യ){ /* End of list */
				rc = subs__process(branch, source_id, topic, qos, retain, stored);
				എങ്കിൽ(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}അല്ലെങ്കിൽ എങ്കിൽ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					തിരികെ rc;
				}
			}
		}
	}

	/* Check for # match */
	HASH_FIND_BYHASHVALUE(hh, subhier->children, "#", 1, hashv_hash, branch);
	എങ്കിൽ(branch && !branch->children){
		/* The topic matches due to a # wildcard - process the
		 * subscriptions but *don't* return. Although this branch has ended
		 * there may still be other subscriptions to deal with.
		 */
		rc = subs__process(branch, source_id, topic, qos, retain, stored);
		എങ്കിൽ(rc == MOSQ_ERR_SUCCESS){
			have_subscribers = true;
		}അല്ലെങ്കിൽ എങ്കിൽ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
			തിരികെ rc;
		}
	}

	എങ്കിൽ(have_subscribers){
		തിരികെ MOSQ_ERR_SUCCESS;
	}അല്ലെങ്കിൽ{
		തിരികെ MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


സ്ഥിതം ഘടന mosquitto__subhier *sub__add_hier_entry(ഘടന mosquitto__subhier *parent, ഘടന mosquitto__subhier **sibling, സ്ഥിരം അക്ഷരം *topic, uint16_t len)
{
	ഘടന mosquitto__subhier *child;

	പ്രതിജ്ഞാ(sibling);

	child = mosquitto_calloc(1, വലുപ്പം(ഘടന mosquitto__subhier) + len + 1);
	എങ്കിൽ(!child){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Out of memory.");
		തിരികെ ശൂന്യ;
	}
	child->parent = parent;
	child->topic_len = len;
	എങ്കിൽ(len > 0){
		strncpy(child->topic, topic, (size_t)(len+1));
	}

	HASH_ADD(hh, *sibling, topic, child->topic_len, child);

	തിരികെ child;
}


പൂർണ്ണസംഖ്യ sub__add(ഘടന mosquitto *context, സ്ഥിരം ഘടന mosquitto_subscription *sub)
{
	പൂർണ്ണസംഖ്യ rc = 0;
	ഘടന mosquitto__subhier *subhier;
	സ്ഥിരം അക്ഷരം *sharename = ശൂന്യ;
	അക്ഷരം *local_sub;
	അക്ഷരം **topics;
	size_t topiclen;

	പ്രതിജ്ഞാ(sub);
	പ്രതിജ്ഞാ(sub->topic_filter);

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &topics, &sharename);
	എങ്കിൽ(rc){
		തിരികെ rc;
	}

	topiclen = strlen(topics[0]);
	എങ്കിൽ(topiclen > UINT16_MAX){
		mosquitto_FREE(local_sub);
		mosquitto_FREE(topics);
		തിരികെ MOSQ_ERR_INVAL;
	}

	എങ്കിൽ(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], topiclen, subhier);
		എങ്കിൽ(!subhier){
			subhier = sub__add_hier_entry(ശൂന്യ, &db.shared_subs, topics[0], (uint16_t)topiclen);
			എങ്കിൽ(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Out of memory.");
				തിരികെ MOSQ_ERR_NOMEM;
			}
		}
	}അല്ലെങ്കിൽ{
		HASH_FIND(hh, db.normal_subs, topics[0], topiclen, subhier);
		എങ്കിൽ(!subhier){
			subhier = sub__add_hier_entry(ശൂന്യ, &db.normal_subs, topics[0], (uint16_t)topiclen);
			എങ്കിൽ(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Out of memory.");
				തിരികെ MOSQ_ERR_NOMEM;
			}
		}
	}
	rc = sub__add_context(context, sub, subhier, topics, sharename);

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	തിരികെ rc;
}


പൂർണ്ണസംഖ്യ sub__remove(ഘടന mosquitto *context, സ്ഥിരം അക്ഷരം *sub, uint8_t *reason)
{
	പൂർണ്ണസംഖ്യ rc = 0;
	ഘടന mosquitto__subhier *subhier;
	സ്ഥിരം അക്ഷരം *sharename = ശൂന്യ;
	അക്ഷരം *local_sub = ശൂന്യ;
	അക്ഷരം **topics = ശൂന്യ;

	പ്രതിജ്ഞാ(sub);

	rc = sub__topic_tokenise(sub, &local_sub, &topics, &sharename);
	എങ്കിൽ(rc){
		തിരികെ rc;
	}

	എങ്കിൽ(sharename){
		HASH_FIND(hh, db.shared_subs, topics[0], strlen(topics[0]), subhier);
	}അല്ലെങ്കിൽ{
		HASH_FIND(hh, db.normal_subs, topics[0], strlen(topics[0]), subhier);
	}
	എങ്കിൽ(subhier){
		*reason = MQTT_RC_NO_SUBSCRIPTION_EXISTED;
		rc = sub__remove_recurse(context, subhier, topics, reason, sharename);
	}

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	തിരികെ rc;
}


പൂർണ്ണസംഖ്യ sub__messages_queue(സ്ഥിരം അക്ഷരം *source_id, uint8_t qos, ഘടന mosquitto_base_സന്ദേശം **stored)
{
	പൂർണ്ണസംഖ്യ rc = MOSQ_ERR_SUCCESS, rc2;
	പൂർണ്ണസംഖ്യ rc_normal = MOSQ_ERR_NO_SUBSCRIBERS, rc_shared = MOSQ_ERR_NO_SUBSCRIBERS;
	ഘടന mosquitto__subhier *subhier;
	അക്ഷരം **split_topics = ശൂന്യ;
	അക്ഷരം *local_topic = ശൂന്യ;
	unsigned hashv;
	size_t topiclen;
	bool retain = (*stored)->data.retain;
	സ്ഥിരം അക്ഷരം *topic = (*stored)->data.topic;

	പ്രതിജ്ഞാ(topic);

	എങ്കിൽ(sub__topic_tokenise(topic, &local_topic, &split_topics, ശൂന്യ)){
		തിരികെ 1;
	}

	/* Protect this message until we have sent it to all
	clients - this is required because websockets client calls
	db__message_write(), which could remove the message if ref_count==0.
	*/
	db_സന്ദേശം_store_ref_inc(*stored);

	topiclen = strlen(split_topics[0]);
	HASH_മൂല്യം(split_topics[0], topiclen, hashv);
	HASH_FIND_BYHASHVALUE(hh, db.normal_subs, split_topics[0], topiclen, hashv, subhier);
	എങ്കിൽ(subhier){
		rc_normal = sub_തിരയൽ(subhier, split_topics, source_id, topic, qos, retain, *stored);
		എങ്കിൽ(rc_normal > 0){
			rc = rc_normal;
			പോകുക end;
		}
	}

	HASH_FIND_BYHASHVALUE(hh, db.shared_subs, split_topics[0], topiclen, hashv, subhier);
	എങ്കിൽ(subhier){
		rc_shared = sub_തിരയൽ(subhier, split_topics, source_id, topic, qos, retain, *stored);
		എങ്കിൽ(rc_shared > 0){
			rc = rc_shared;
			പോകുക end;
		}
	}

	എങ്കിൽ(rc_normal == MOSQ_ERR_NO_SUBSCRIBERS && rc_shared == MOSQ_ERR_NO_SUBSCRIBERS){
		rc = MOSQ_ERR_NO_SUBSCRIBERS;
	}

	എങ്കിൽ(retain){
		rc2 = retain__store(topic, *stored, split_topics, true);
		എങ്കിൽ(rc2){
			rc = rc2;
		}
	}

end:
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);
	/* Remove our reference and free if needed. */
	db_സന്ദേശം_store_ref_dec(stored);

	തിരികെ rc;
}


/* Remove a subhier element, and return its parent if that needs freeing as well. */
സ്ഥിതം ഘടന mosquitto__subhier *tmp_remove_subs(ഘടന mosquitto__subhier *sub)
{
	ഘടന mosquitto__subhier *parent;

	എങ്കിൽ(!sub || !sub->parent){
		തിരികെ ശൂന്യ;
	}

	എങ്കിൽ(sub->children || sub->subs){
		തിരികെ ശൂന്യ;
	}

	parent = sub->parent;
	HASH_DELETE(hh, parent->children, sub);
	mosquitto_FREE(sub);

	എങ്കിൽ(parent->subs == ശൂന്യ
			&& parent->children == ശൂന്യ
			&& parent->shared == ശൂന്യ
			&& parent->parent){

		തിരികെ parent;
	}അല്ലെങ്കിൽ{
		തിരികെ ശൂന്യ;
	}
}


/* Remove all subscriptions for a client.
 */
പൂർണ്ണസംഖ്യ sub__clean_session(ഘടന mosquitto *context)
{
	വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<context->subs_capacity; i++){
		എങ്കിൽ(context->subs[i] == ശൂന്യ || context->subs[i]->hier == ശൂന്യ){
			തുടരുക;
		}

		ഘടന mosquitto__subhier *hier = context->subs[i]->hier;
		ഘടന mosquitto__subleaf *leaf;

		plugin_persist__handle_subscription_delete(context, context->subs[i]->topic_filter);
		എങ്കിൽ(context->subs[i]->shared){
			leaf = context->subs[i]->shared->subs;
			ആകുമ്പോൾ(leaf){
				എങ്കിൽ(leaf->context==context){
#ifdef WITH_SYS_വൃക്ഷം
					db.shared_subscription_എണ്ണം--;
#endif
					sub__remove_shared_leaf(context->subs[i]->hier, context->subs[i]->shared, leaf);
					നിർത്തുക;
				}
				leaf = leaf->next;
			}
		}അല്ലെങ്കിൽ{
			leaf = hier->subs;
			ആകുമ്പോൾ(leaf){
				എങ്കിൽ(leaf->context==context){
#ifdef WITH_SYS_വൃക്ഷം
					db.subscription_എണ്ണം--;
#endif
					DL_DELETE(hier->subs, leaf);
					നിർത്തുക;
				}
				leaf = leaf->next;
			}
		}
		mosquitto_FREE(context->subs[i]);

		എങ്കിൽ(hier->subs == ശൂന്യ
				&& hier->children == ശൂന്യ
				&& hier->shared == ശൂന്യ
				&& hier->parent){

			ചെയ്യുക{
				hier = tmp_remove_subs(hier);
			}ആകുമ്പോൾ(hier);
		}
	}
	mosquitto_FREE(context->subs);
	context->subs_capacity = 0;
	context->subs_എണ്ണം = 0;

	തിരികെ MOSQ_ERR_SUCCESS;
}


ശൂന്യം sub_വൃക്ഷം_print(ഘടന mosquitto__subhier *വേര്, പൂർണ്ണസംഖ്യ level)
{
	പൂർണ്ണസംഖ്യ i;
	ഘടന mosquitto__subhier *branch, *branch_tmp;
	ഘടന mosquitto__subleaf *leaf;

	HASH_ITER(hh, വേര്, branch, branch_tmp){
		എങ്കിൽ(level > -1){
			വേണ്ടി(i=0; i<(level+2)*2; i++){
				printf(" ");
			}
			printf("%s", branch->topic);
			leaf = branch->subs;
			ആകുമ്പോൾ(leaf){
				എങ്കിൽ(leaf->context){
					printf(" (%s, %d)", leaf->context->id, MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}അല്ലെങ്കിൽ{
					printf(" (%s, %d)", "", MQTT_SUB_OPT_GET_QOS(leaf->subscription_options));
				}
				leaf = leaf->next;
			}
			printf("\n");
		}

		sub_വൃക്ഷം_print(branch->children, level+1);
	}
}


പൂർണ്ണസംഖ്യ sub__init(ശൂന്യം)
{
	HASH_മൂല്യം("+", 1, hashv_plus);
	HASH_മൂല്യം("#", 1, hashv_hash);

	എങ്കിൽ(sub__add_hier_entry(ശൂന്യ, &db.shared_subs, "", 0) == ശൂന്യ
			|| sub__add_hier_entry(ശൂന്യ, &db.normal_subs, "", 0) == ശൂന്യ
			|| sub__add_hier_entry(ശൂന്യ, &db.normal_subs, "$SYS", (uint16_t)strlen("$SYS")) == ശൂന്യ
			){

		തിരികെ MOSQ_ERR_NOMEM;
	}അല്ലെങ്കിൽ{
		തിരികെ MOSQ_ERR_SUCCESS;
	}
}
