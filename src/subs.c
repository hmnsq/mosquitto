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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "utlist.h"

ਸਥਿਰ ਬਣਤਰ mosquitto__subhier *sub_ਜੋੜੋ_hier_ਇੰਦਰਾਜ(ਬਣਤਰ mosquitto__subhier *ਮਾਪਾ, ਬਣਤਰ mosquitto__subhier **sibling, ਅਚਲ ਅੱਖਰ *topic, uint16_t len);

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ hashv_plus = 0;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ hashv_hash = 0;


ਸਥਿਰ ਪੂਰਨ subs__send(ਬਣਤਰ mosquitto__subleaf *ਪੱਤੀ, ਅਚਲ ਅੱਖਰ *topic, uint8_t qos, ਪੂਰਨ retain, ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *stored)
{
	bool client_retain;
	uint16_t mid;
	uint8_t client_qos, ਸੁਨੇਹਾ_qos;
	ਪੂਰਨ rc2;

	/* Check for ACL topic access. */
	rc2 = mosquitto_acl_check(ਪੱਤੀ->context, topic, stored->ਅੰਕੜੇ.payloadlen, stored->ਅੰਕੜੇ.payload, stored->ਅੰਕੜੇ.qos, stored->ਅੰਕੜੇ.retain, stored->ਅੰਕੜੇ.properties, MOSQ_ACL_READ);
	ਜੇ(rc2 == MOSQ_ERR_ACL_DENIED){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}ਨਹੀਂਤਾਂ ਜੇ(rc2 == MOSQ_ERR_SUCCESS){
		client_qos = MQTT_SUB_OPT_GET_QOS(ਪੱਤੀ->subscription_options);

		ਜੇ(db.config->upgrade_outgoing_qos){
			ਸੁਨੇਹਾ_qos = client_qos;
		}ਨਹੀਂਤਾਂ{
			ਜੇ(qos > client_qos){
				ਸੁਨੇਹਾ_qos = client_qos;
			}ਨਹੀਂਤਾਂ{
				ਸੁਨੇਹਾ_qos = qos;
			}
		}
		ਜੇ(ਸੁਨੇਹਾ_qos){
			mid = mosquitto__mid_generate(ਪੱਤੀ->context);
		}ਨਹੀਂਤਾਂ{
			mid = 0;
		}
		ਜੇ(MQTT_SUB_OPT_GET_RETAIN_AS_PUBLISHED(ਪੱਤੀ->subscription_options)){
			client_retain = retain;
		}ਨਹੀਂਤਾਂ{
			client_retain = false;
		}
		ਜੇ(db_ਸੁਨੇਹਾ_ਪਾਓ_outgoing(ਪੱਤੀ->context, 0, mid, ਸੁਨੇਹਾ_qos, client_retain, stored, ਪੱਤੀ->identifier, true, true) == 1){
			ਵਾਪਸ 1;
		}
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ 1; /* Application error */
	}
	ਵਾਪਸ 0;
}


ਸਥਿਰ ਪੂਰਨ subs__shared_process(ਬਣਤਰ mosquitto__subhier *hier, ਅਚਲ ਅੱਖਰ *topic, uint8_t qos, ਪੂਰਨ retain, ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *stored)
{
	ਪੂਰਨ rc = 0, rc2;
	ਬਣਤਰ mosquitto__subshared *shared, *shared_tmp;
	ਬਣਤਰ mosquitto__subleaf *ਪੱਤੀ;

	HASH_ITER(hh, hier->shared, shared, shared_tmp){
		ਪੱਤੀ = shared->subs;
		rc2 = subs__send(ਪੱਤੀ, topic, qos, retain, stored);
		/* Remove current from the top, add back to the bottom */
		DL_DELETE(shared->subs, ਪੱਤੀ);
		DL_APPEND(shared->subs, ਪੱਤੀ);

		ਜੇ(rc2){
			rc = 1;
		}
	}

	ਵਾਪਸ rc;
}


ਸਥਿਰ ਪੂਰਨ subs__process(ਬਣਤਰ mosquitto__subhier *hier, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ_id, ਅਚਲ ਅੱਖਰ *topic, uint8_t qos, ਪੂਰਨ retain, ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *stored)
{
	ਪੂਰਨ rc = 0;
	ਪੂਰਨ rc2;
	ਬਣਤਰ mosquitto__subleaf *ਪੱਤੀ;

	rc = subs__shared_process(hier, topic, qos, retain, stored);

	ਪੱਤੀ = hier->subs;
	ਜਦੋਂ(ਸਰੋਤ_id && ਪੱਤੀ){
		ਜੇ(!ਪੱਤੀ->context->id || (MQTT_SUB_OPT_GET_NO_LOCAL(ਪੱਤੀ->subscription_options) && !strcmp(ਪੱਤੀ->context->id, ਸਰੋਤ_id))){
			ਪੱਤੀ = ਪੱਤੀ->ਅਗਲਾ;
			ਜਾਰੀ;
		}
		rc2 = subs__send(ਪੱਤੀ, topic, qos, retain, stored);
		ਜੇ(rc2){
			rc = 1;
		}
		ਪੱਤੀ = ਪੱਤੀ->ਅਗਲਾ;
	}
	ਜੇ(hier->subs || hier->shared){
		ਵਾਪਸ rc;
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


ਸਥਿਰ ਪੂਰਨ sub_ਜੋੜੋ_ਪੱਤੀ(ਬਣਤਰ mosquitto *context, ਅਚਲ ਬਣਤਰ mosquitto_subscription *sub, ਬਣਤਰ mosquitto__subleaf **ਸਿਰ, ਬਣਤਰ mosquitto__subleaf **newleaf)
{
	ਬਣਤਰ mosquitto__subleaf *ਪੱਤੀ;

	*newleaf = ਨਿਰਾ;
	ਪੱਤੀ = *ਸਿਰ;

	ਜਦੋਂ(ਪੱਤੀ){
		ਜੇ(ਪੱਤੀ->context && ਪੱਤੀ->context->id && !strcmp(ਪੱਤੀ->context->id, context->id)){
			/* Client making a second subscription to same topic. Only
			 * need to update QoS. Return MOSQ_ERR_SUB_EXISTS to
			 * indicate this to the calling function. */
			ਪੱਤੀ->identifier = sub->identifier;
			ਪੱਤੀ->subscription_options = sub->options;
			ਵਾਪਸ MOSQ_ERR_SUB_EXISTS;
		}
		ਪੱਤੀ = ਪੱਤੀ->ਅਗਲਾ;
	}
	ਪੱਤੀ = mosquitto_calloc(1, ਆਕਾਰ(ਬਣਤਰ mosquitto__subleaf) + ਲੜੀ_ਲੰਬਾਈ(sub->topic_filter) + 1);
	ਜੇ(!ਪੱਤੀ){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}
	ਪੱਤੀ->context = context;
	ਪੱਤੀ->identifier = sub->identifier;
	ਪੱਤੀ->subscription_options = sub->options;
	strcpy(ਪੱਤੀ->topic_filter, sub->topic_filter);

	DL_APPEND(*ਸਿਰ, ਪੱਤੀ);
	*newleaf = ਪੱਤੀ;

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਖਾਲੀ sub_remove_shared_ਪੱਤੀ(ਬਣਤਰ mosquitto__subhier *subhier, ਬਣਤਰ mosquitto__subshared *shared, ਬਣਤਰ mosquitto__subleaf *ਪੱਤੀ)
{
	DL_DELETE(shared->subs, ਪੱਤੀ);
	ਜੇ(shared->subs == ਨਿਰਾ){
		HASH_DELETE(hh, subhier->shared, shared);
		mosquitto_FREE(shared);
	}
}


ਸਥਿਰ ਪੂਰਨ sub_ਜੋੜੋ_shared(ਬਣਤਰ mosquitto *context, ਅਚਲ ਬਣਤਰ mosquitto_subscription *sub, ਬਣਤਰ mosquitto__subhier *subhier, ਅਚਲ ਅੱਖਰ *sharename)
{
	ਬਣਤਰ mosquitto__subleaf *newleaf;
	ਬਣਤਰ mosquitto__subshared *shared = ਨਿਰਾ;
	ਬਣਤਰ mosquitto__subleaf **subs;
	size_t slen;
	ਪੂਰਨ rc;
	ਬਿਨਾਨਿਸ਼ਾਨੀ hashv;

	slen = ਲੜੀ_ਲੰਬਾਈ(sharename);

	HASH_ਮੁੱਲ(sharename, slen, hashv);

	HASH_ਲੱਭੋ_BYHASHVALUE(hh, subhier->shared, sharename, slen, hashv, shared);
	ਜੇ(shared == ਨਿਰਾ){
		shared = mosquitto_calloc(1, ਆਕਾਰ(ਬਣਤਰ mosquitto__subshared) + slen + 1);
		ਜੇ(!shared){
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}
		strncpy(shared->name, sharename, slen+1);

		HASH_ਜੋੜੋ_BYHASHVALUE(hh, subhier->shared, name, slen, hashv, shared);
	}

	rc = sub_ਜੋੜੋ_ਪੱਤੀ(context, sub, &shared->subs, &newleaf);
	ਜੇ(rc > 0){
		ਜੇ(shared->subs == ਨਿਰਾ){
			HASH_DELETE(hh, subhier->shared, shared);
			mosquitto_FREE(shared);
		}
		ਵਾਪਸ rc;
	}

	ਜੇ(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = shared;

		bool assigned = false;
		ਲਈ(ਪੂਰਨ i=0; i<context->subs_ਸਮਰੱਥਾ; i++){
			ਜੇ(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_ਗਿਣਤੀ++;
				assigned = true;
				ਰੋਕੋ;
			}
		}
		ਜੇ(assigned == false){
			subs = mosquitto_realloc(context->subs, ਆਕਾਰ(ਬਣਤਰ mosquitto__subleaf *)*(size_t)(context->subs_ਸਮਰੱਥਾ + 1));
			ਜੇ(!subs){
				sub_remove_shared_ਪੱਤੀ(subhier, shared, newleaf);
				mosquitto_FREE(newleaf);
				ਵਾਪਸ MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_ਸਮਰੱਥਾ++;
			context->subs_ਗਿਣਤੀ++;
			context->subs[context->subs_ਸਮਰੱਥਾ-1] = newleaf;
		}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_SYS_ਰੁੱਖ
		db.shared_subscription_ਗਿਣਤੀ++;
#ਜੇ_ਅੰਤ
	}

	ਜੇ(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		ਵਾਪਸ rc;
	}ਨਹੀਂਤਾਂ{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}
}


ਸਥਿਰ ਪੂਰਨ sub_ਜੋੜੋ_ਸਾਧਾਰਨ(ਬਣਤਰ mosquitto *context, ਅਚਲ ਬਣਤਰ mosquitto_subscription *sub, ਬਣਤਰ mosquitto__subhier *subhier)
{
	ਬਣਤਰ mosquitto__subleaf *newleaf = ਨਿਰਾ;
	ਬਣਤਰ mosquitto__subleaf **subs;
	ਪੂਰਨ rc;

	rc = sub_ਜੋੜੋ_ਪੱਤੀ(context, sub, &subhier->subs, &newleaf);
	ਜੇ(rc > 0){
		ਵਾਪਸ rc;
	}

	ਜੇ(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = ਨਿਰਾ;

		bool assigned = false;
		ਲਈ(ਪੂਰਨ i=0; i<context->subs_ਸਮਰੱਥਾ; i++){
			ਜੇ(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_ਗਿਣਤੀ++;
				assigned = true;
				ਰੋਕੋ;
			}
		}
		ਜੇ(assigned == false){
			subs = mosquitto_realloc(context->subs, ਆਕਾਰ(ਬਣਤਰ mosquitto__subleaf *)*(size_t)(context->subs_ਸਮਰੱਥਾ + 1));
			ਜੇ(!subs){
				DL_DELETE(subhier->subs, newleaf);
				mosquitto_FREE(newleaf);
				ਵਾਪਸ MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_ਸਮਰੱਥਾ++;
			context->subs_ਗਿਣਤੀ++;
			context->subs[context->subs_ਸਮਰੱਥਾ-1] = newleaf;
		}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_SYS_ਰੁੱਖ
		db.subscription_ਗਿਣਤੀ++;
#ਜੇ_ਅੰਤ
	}

	ਜੇ(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		ਵਾਪਸ rc;
	}ਨਹੀਂਤਾਂ{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}
}


ਸਥਿਰ ਪੂਰਨ sub_ਜੋੜੋ_context(ਬਣਤਰ mosquitto *context, ਅਚਲ ਬਣਤਰ mosquitto_subscription *sub, ਬਣਤਰ mosquitto__subhier *subhier, ਅੱਖਰ *ਅਚਲ *ਅਚਲ topics, ਅਚਲ ਅੱਖਰ *sharename)
{
	ਬਣਤਰ mosquitto__subhier *branch;
	ਪੂਰਨ topic_ਸੂਚਕਾਂਕ = 0;
	size_t topiclen;

	/* Find leaf node */
	ਜਦੋਂ(topics && topics[topic_ਸੂਚਕਾਂਕ] != ਨਿਰਾ){
		topiclen = ਲੜੀ_ਲੰਬਾਈ(topics[topic_ਸੂਚਕਾਂਕ]);
		ਜੇ(topiclen > UINT16_MAX){
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
		HASH_ਲੱਭੋ(hh, subhier->children, topics[topic_ਸੂਚਕਾਂਕ], topiclen, branch);
		ਜੇ(!branch){
			/* Not found */
			branch = sub_ਜੋੜੋ_hier_ਇੰਦਰਾਜ(subhier, &subhier->children, topics[topic_ਸੂਚਕਾਂਕ], (uint16_t)topiclen);
			ਜੇ(!branch){
				ਵਾਪਸ MOSQ_ERR_NOMEM;
			}
		}
		subhier = branch;
		topic_ਸੂਚਕਾਂਕ++;
	}

	/* Add add our context */
	ਜੇ(context && context->id){
		ਜੇ(sharename){
			ਵਾਪਸ sub_ਜੋੜੋ_shared(context, sub, subhier, sharename);
		}ਨਹੀਂਤਾਂ{
			ਵਾਪਸ sub_ਜੋੜੋ_ਸਾਧਾਰਨ(context, sub, subhier);
		}
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}
}


ਸਥਿਰ ਪੂਰਨ sub_remove_ਸਾਧਾਰਨ(ਬਣਤਰ mosquitto *context, ਬਣਤਰ mosquitto__subhier *subhier, uint8_t *reason)
{
	ਬਣਤਰ mosquitto__subleaf *ਪੱਤੀ;

	ਪੱਤੀ = subhier->subs;
	ਜਦੋਂ(ਪੱਤੀ){
		ਜੇ(ਪੱਤੀ->context==context){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_SYS_ਰੁੱਖ
			db.subscription_ਗਿਣਤੀ--;
#ਜੇ_ਅੰਤ
			DL_DELETE(subhier->subs, ਪੱਤੀ);

			/* Remove the reference to the sub that the client is keeping.
			 * It would be nice to be able to use the reference directly,
			 * but that would involve keeping a copy of the topic string in
			 * each subleaf. Might be worth considering though. */
			ਲਈ(ਪੂਰਨ i=0; i<context->subs_ਸਮਰੱਥਾ; i++){
				ਜੇ(context->subs[i] && context->subs[i]->hier == subhier){
					context->subs_ਗਿਣਤੀ--;
					mosquitto_free(context->subs[i]);
					context->subs[i] = ਨਿਰਾ;
					ਰੋਕੋ;
				}
			}
			*reason = 0;
			ਵਾਪਸ MOSQ_ERR_SUCCESS;
		}
		ਪੱਤੀ = ਪੱਤੀ->ਅਗਲਾ;
	}
	ਵਾਪਸ MOSQ_ERR_NO_SUBSCRIBERS;
}


ਸਥਿਰ ਪੂਰਨ sub__remove_shared(ਬਣਤਰ mosquitto *context, ਬਣਤਰ mosquitto__subhier *subhier, uint8_t *reason, ਅਚਲ ਅੱਖਰ *sharename)
{
	ਬਣਤਰ mosquitto__subshared *shared;

	HASH_ਲੱਭੋ(hh, subhier->shared, sharename, ਲੜੀ_ਲੰਬਾਈ(sharename), shared);
	ਜੇ(shared){
		ਬਣਤਰ mosquitto__subleaf *ਪੱਤੀ = shared->subs;
		ਜਦੋਂ(ਪੱਤੀ){
			ਜੇ(ਪੱਤੀ->context==context){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_SYS_ਰੁੱਖ
				db.shared_subscription_ਗਿਣਤੀ--;
#ਜੇ_ਅੰਤ
				DL_DELETE(shared->subs, ਪੱਤੀ);

				/* Remove the reference to the sub that the client is keeping.
				* It would be nice to be able to use the reference directly,
				* but that would involve keeping a copy of the topic string in
				* each subleaf. Might be worth considering though. */
				ਲਈ(ਪੂਰਨ i=0; i<context->subs_ਸਮਰੱਥਾ; i++){
					ਜੇ(context->subs[i]
							&& context->subs[i]->hier == subhier
							&& context->subs[i]->shared == shared){

						mosquitto_free(context->subs[i]);
						context->subs[i] = ਨਿਰਾ;
						context->subs_ਗਿਣਤੀ--;
						ਰੋਕੋ;
					}
				}

				ਜੇ(shared->subs == ਨਿਰਾ){
					HASH_DELETE(hh, subhier->shared, shared);
					mosquitto_FREE(shared);
				}

				*reason = 0;
				ਵਾਪਸ MOSQ_ERR_SUCCESS;
			}
			ਪੱਤੀ = ਪੱਤੀ->ਅਗਲਾ;
		}
		ਵਾਪਸ MOSQ_ERR_NO_SUBSCRIBERS;
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


ਸਥਿਰ ਪੂਰਨ sub__remove_recurse(ਬਣਤਰ mosquitto *context, ਬਣਤਰ mosquitto__subhier *subhier, ਅੱਖਰ **topics, uint8_t *reason, ਅਚਲ ਅੱਖਰ *sharename)
{
	ਬਣਤਰ mosquitto__subhier *branch;

	ਜੇ(topics == ਨਿਰਾ || topics[0] == ਨਿਰਾ){
		ਜੇ(sharename){
			ਵਾਪਸ sub__remove_shared(context, subhier, reason, sharename);
		}ਨਹੀਂਤਾਂ{
			ਵਾਪਸ sub_remove_ਸਾਧਾਰਨ(context, subhier, reason);
		}
	}

	HASH_ਲੱਭੋ(hh, subhier->children, topics[0], ਲੜੀ_ਲੰਬਾਈ(topics[0]), branch);
	ਜੇ(branch){
		sub__remove_recurse(context, branch, &(topics[1]), reason, sharename);
		ਜੇ(!branch->children && !branch->subs && !branch->shared){
			HASH_DELETE(hh, subhier->children, branch);
			mosquitto_FREE(branch);
		}
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ sub_ਖੋਜ(ਬਣਤਰ mosquitto__subhier *subhier, ਅੱਖਰ **ਵੰਡੋ_topics, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ_id, ਅਚਲ ਅੱਖਰ *topic, uint8_t qos, ਪੂਰਨ retain, ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *stored)
{
	/* FIXME - need to take into account source_id if the client is a bridge */
	ਬਣਤਰ mosquitto__subhier *branch;
	ਪੂਰਨ rc;
	bool have_subscribers = false;

	ਜੇ(ਵੰਡੋ_topics && ਵੰਡੋ_topics[0]){
		/* Check for literal match */
		HASH_ਲੱਭੋ(hh, subhier->children, ਵੰਡੋ_topics[0], ਲੜੀ_ਲੰਬਾਈ(ਵੰਡੋ_topics[0]), branch);

		ਜੇ(branch){
			rc = sub_ਖੋਜ(branch, &(ਵੰਡੋ_topics[1]), ਸਰੋਤ_id, topic, qos, retain, stored);
			ਜੇ(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}ਨਹੀਂਤਾਂ ਜੇ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				ਵਾਪਸ rc;
			}
			ਜੇ(ਵੰਡੋ_topics[1] == ਨਿਰਾ){ /* End of list */
				rc = subs__process(branch, ਸਰੋਤ_id, topic, qos, retain, stored);
				ਜੇ(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}ਨਹੀਂਤਾਂ ਜੇ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					ਵਾਪਸ rc;
				}
			}
		}

		/* Check for + match */
		HASH_ਲੱਭੋ_BYHASHVALUE(hh, subhier->children, "+", 1, hashv_plus, branch);

		ਜੇ(branch){
			rc = sub_ਖੋਜ(branch, &(ਵੰਡੋ_topics[1]), ਸਰੋਤ_id, topic, qos, retain, stored);
			ਜੇ(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}ਨਹੀਂਤਾਂ ਜੇ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				ਵਾਪਸ rc;
			}
			ਜੇ(ਵੰਡੋ_topics[1] == ਨਿਰਾ){ /* End of list */
				rc = subs__process(branch, ਸਰੋਤ_id, topic, qos, retain, stored);
				ਜੇ(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}ਨਹੀਂਤਾਂ ਜੇ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					ਵਾਪਸ rc;
				}
			}
		}
	}

	/* Check for # match */
	HASH_ਲੱਭੋ_BYHASHVALUE(hh, subhier->children, "#", 1, hashv_hash, branch);
	ਜੇ(branch && !branch->children){
		/* The topic matches due to a # wildcard - process the
		 * subscriptions but *don't* return. Although this branch has ended
		 * there may still be other subscriptions to deal with.
		 */
		rc = subs__process(branch, ਸਰੋਤ_id, topic, qos, retain, stored);
		ਜੇ(rc == MOSQ_ERR_SUCCESS){
			have_subscribers = true;
		}ਨਹੀਂਤਾਂ ਜੇ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
			ਵਾਪਸ rc;
		}
	}

	ਜੇ(have_subscribers){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


ਸਥਿਰ ਬਣਤਰ mosquitto__subhier *sub_ਜੋੜੋ_hier_ਇੰਦਰਾਜ(ਬਣਤਰ mosquitto__subhier *ਮਾਪਾ, ਬਣਤਰ mosquitto__subhier **sibling, ਅਚਲ ਅੱਖਰ *topic, uint16_t len)
{
	ਬਣਤਰ mosquitto__subhier *ਬੱਚਾ;

	ਪ੍ਰਤਿਜ੍ਞਾ(sibling);

	ਬੱਚਾ = mosquitto_calloc(1, ਆਕਾਰ(ਬਣਤਰ mosquitto__subhier) + len + 1);
	ਜੇ(!ਬੱਚਾ){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ਵਾਪਸ ਨਿਰਾ;
	}
	ਬੱਚਾ->ਮਾਪਾ = ਮਾਪਾ;
	ਬੱਚਾ->topic_len = len;
	ਜੇ(len > 0){
		strncpy(ਬੱਚਾ->topic, topic, (size_t)(len+1));
	}

	HASH_ਜੋੜੋ(hh, *sibling, topic, ਬੱਚਾ->topic_len, ਬੱਚਾ);

	ਵਾਪਸ ਬੱਚਾ;
}


ਪੂਰਨ sub_ਜੋੜੋ(ਬਣਤਰ mosquitto *context, ਅਚਲ ਬਣਤਰ mosquitto_subscription *sub)
{
	ਪੂਰਨ rc = 0;
	ਬਣਤਰ mosquitto__subhier *subhier;
	ਅਚਲ ਅੱਖਰ *sharename = ਨਿਰਾ;
	ਅੱਖਰ *local_sub;
	ਅੱਖਰ **topics;
	size_t topiclen;

	ਪ੍ਰਤਿਜ੍ਞਾ(sub);
	ਪ੍ਰਤਿਜ੍ਞਾ(sub->topic_filter);

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &topics, &sharename);
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}

	topiclen = ਲੜੀ_ਲੰਬਾਈ(topics[0]);
	ਜੇ(topiclen > UINT16_MAX){
		mosquitto_FREE(local_sub);
		mosquitto_FREE(topics);
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	ਜੇ(sharename){
		HASH_ਲੱਭੋ(hh, db.shared_subs, topics[0], topiclen, subhier);
		ਜੇ(!subhier){
			subhier = sub_ਜੋੜੋ_hier_ਇੰਦਰਾਜ(ਨਿਰਾ, &db.shared_subs, topics[0], (uint16_t)topiclen);
			ਜੇ(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
				ਵਾਪਸ MOSQ_ERR_NOMEM;
			}
		}
	}ਨਹੀਂਤਾਂ{
		HASH_ਲੱਭੋ(hh, db.ਸਾਧਾਰਨ_subs, topics[0], topiclen, subhier);
		ਜੇ(!subhier){
			subhier = sub_ਜੋੜੋ_hier_ਇੰਦਰਾਜ(ਨਿਰਾ, &db.ਸਾਧਾਰਨ_subs, topics[0], (uint16_t)topiclen);
			ਜੇ(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
				ਵਾਪਸ MOSQ_ERR_NOMEM;
			}
		}
	}
	rc = sub_ਜੋੜੋ_context(context, sub, subhier, topics, sharename);

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	ਵਾਪਸ rc;
}


ਪੂਰਨ sub__remove(ਬਣਤਰ mosquitto *context, ਅਚਲ ਅੱਖਰ *sub, uint8_t *reason)
{
	ਪੂਰਨ rc = 0;
	ਬਣਤਰ mosquitto__subhier *subhier;
	ਅਚਲ ਅੱਖਰ *sharename = ਨਿਰਾ;
	ਅੱਖਰ *local_sub = ਨਿਰਾ;
	ਅੱਖਰ **topics = ਨਿਰਾ;

	ਪ੍ਰਤਿਜ੍ਞਾ(sub);

	rc = sub__topic_tokenise(sub, &local_sub, &topics, &sharename);
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}

	ਜੇ(sharename){
		HASH_ਲੱਭੋ(hh, db.shared_subs, topics[0], ਲੜੀ_ਲੰਬਾਈ(topics[0]), subhier);
	}ਨਹੀਂਤਾਂ{
		HASH_ਲੱਭੋ(hh, db.ਸਾਧਾਰਨ_subs, topics[0], ਲੜੀ_ਲੰਬਾਈ(topics[0]), subhier);
	}
	ਜੇ(subhier){
		*reason = MQTT_RC_NO_SUBSCRIPTION_EXISTED;
		rc = sub__remove_recurse(context, subhier, topics, reason, sharename);
	}

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	ਵਾਪਸ rc;
}


ਪੂਰਨ sub_messages_ਕਤਾਰ(ਅਚਲ ਅੱਖਰ *ਸਰੋਤ_id, uint8_t qos, ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ **stored)
{
	ਪੂਰਨ rc = MOSQ_ERR_SUCCESS, rc2;
	ਪੂਰਨ rc_ਸਾਧਾਰਨ = MOSQ_ERR_NO_SUBSCRIBERS, rc_shared = MOSQ_ERR_NO_SUBSCRIBERS;
	ਬਣਤਰ mosquitto__subhier *subhier;
	ਅੱਖਰ **ਵੰਡੋ_topics = ਨਿਰਾ;
	ਅੱਖਰ *local_topic = ਨਿਰਾ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ hashv;
	size_t topiclen;
	bool retain = (*stored)->ਅੰਕੜੇ.retain;
	ਅਚਲ ਅੱਖਰ *topic = (*stored)->ਅੰਕੜੇ.topic;

	ਪ੍ਰਤਿਜ੍ਞਾ(topic);

	ਜੇ(sub__topic_tokenise(topic, &local_topic, &ਵੰਡੋ_topics, ਨਿਰਾ)){
		ਵਾਪਸ 1;
	}

	/* Protect this message until we have sent it to all
	clients - this is required because websockets client calls
	db__message_write(), which could remove the message if ref_count==0.
	*/
	db_ਸੁਨੇਹਾ_store_ref_inc(*stored);

	topiclen = ਲੜੀ_ਲੰਬਾਈ(ਵੰਡੋ_topics[0]);
	HASH_ਮੁੱਲ(ਵੰਡੋ_topics[0], topiclen, hashv);
	HASH_ਲੱਭੋ_BYHASHVALUE(hh, db.ਸਾਧਾਰਨ_subs, ਵੰਡੋ_topics[0], topiclen, hashv, subhier);
	ਜੇ(subhier){
		rc_ਸਾਧਾਰਨ = sub_ਖੋਜ(subhier, ਵੰਡੋ_topics, ਸਰੋਤ_id, topic, qos, retain, *stored);
		ਜੇ(rc_ਸਾਧਾਰਨ > 0){
			rc = rc_ਸਾਧਾਰਨ;
			ਜਾਓ ਸਿਰਾ;
		}
	}

	HASH_ਲੱਭੋ_BYHASHVALUE(hh, db.shared_subs, ਵੰਡੋ_topics[0], topiclen, hashv, subhier);
	ਜੇ(subhier){
		rc_shared = sub_ਖੋਜ(subhier, ਵੰਡੋ_topics, ਸਰੋਤ_id, topic, qos, retain, *stored);
		ਜੇ(rc_shared > 0){
			rc = rc_shared;
			ਜਾਓ ਸਿਰਾ;
		}
	}

	ਜੇ(rc_ਸਾਧਾਰਨ == MOSQ_ERR_NO_SUBSCRIBERS && rc_shared == MOSQ_ERR_NO_SUBSCRIBERS){
		rc = MOSQ_ERR_NO_SUBSCRIBERS;
	}

	ਜੇ(retain){
		rc2 = retain__store(topic, *stored, ਵੰਡੋ_topics, true);
		ਜੇ(rc2){
			rc = rc2;
		}
	}

ਸਿਰਾ:
	mosquitto_FREE(ਵੰਡੋ_topics);
	mosquitto_FREE(local_topic);
	/* Remove our reference and free if needed. */
	db_ਸੁਨੇਹਾ_store_ref_dec(stored);

	ਵਾਪਸ rc;
}


/* Remove a subhier element, and return its parent if that needs freeing as well. */
ਸਥਿਰ ਬਣਤਰ mosquitto__subhier *tmp_remove_subs(ਬਣਤਰ mosquitto__subhier *sub)
{
	ਬਣਤਰ mosquitto__subhier *ਮਾਪਾ;

	ਜੇ(!sub || !sub->ਮਾਪਾ){
		ਵਾਪਸ ਨਿਰਾ;
	}

	ਜੇ(sub->children || sub->subs){
		ਵਾਪਸ ਨਿਰਾ;
	}

	ਮਾਪਾ = sub->ਮਾਪਾ;
	HASH_DELETE(hh, ਮਾਪਾ->children, sub);
	mosquitto_FREE(sub);

	ਜੇ(ਮਾਪਾ->subs == ਨਿਰਾ
			&& ਮਾਪਾ->children == ਨਿਰਾ
			&& ਮਾਪਾ->shared == ਨਿਰਾ
			&& ਮਾਪਾ->ਮਾਪਾ){

		ਵਾਪਸ ਮਾਪਾ;
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ ਨਿਰਾ;
	}
}


/* Remove all subscriptions for a client.
 */
ਪੂਰਨ sub__clean_session(ਬਣਤਰ mosquitto *context)
{
	ਲਈ(ਪੂਰਨ i=0; i<context->subs_ਸਮਰੱਥਾ; i++){
		ਜੇ(context->subs[i] == ਨਿਰਾ || context->subs[i]->hier == ਨਿਰਾ){
			ਜਾਰੀ;
		}

		ਬਣਤਰ mosquitto__subhier *hier = context->subs[i]->hier;
		ਬਣਤਰ mosquitto__subleaf *ਪੱਤੀ;

		plugin_persist__handle_subscription_delete(context, context->subs[i]->topic_filter);
		ਜੇ(context->subs[i]->shared){
			ਪੱਤੀ = context->subs[i]->shared->subs;
			ਜਦੋਂ(ਪੱਤੀ){
				ਜੇ(ਪੱਤੀ->context==context){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_SYS_ਰੁੱਖ
					db.shared_subscription_ਗਿਣਤੀ--;
#ਜੇ_ਅੰਤ
					sub_remove_shared_ਪੱਤੀ(context->subs[i]->hier, context->subs[i]->shared, ਪੱਤੀ);
					ਰੋਕੋ;
				}
				ਪੱਤੀ = ਪੱਤੀ->ਅਗਲਾ;
			}
		}ਨਹੀਂਤਾਂ{
			ਪੱਤੀ = hier->subs;
			ਜਦੋਂ(ਪੱਤੀ){
				ਜੇ(ਪੱਤੀ->context==context){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_SYS_ਰੁੱਖ
					db.subscription_ਗਿਣਤੀ--;
#ਜੇ_ਅੰਤ
					DL_DELETE(hier->subs, ਪੱਤੀ);
					ਰੋਕੋ;
				}
				ਪੱਤੀ = ਪੱਤੀ->ਅਗਲਾ;
			}
		}
		mosquitto_FREE(context->subs[i]);

		ਜੇ(hier->subs == ਨਿਰਾ
				&& hier->children == ਨਿਰਾ
				&& hier->shared == ਨਿਰਾ
				&& hier->ਮਾਪਾ){

			ਕਰੋ{
				hier = tmp_remove_subs(hier);
			}ਜਦੋਂ(hier);
		}
	}
	mosquitto_FREE(context->subs);
	context->subs_ਸਮਰੱਥਾ = 0;
	context->subs_ਗਿਣਤੀ = 0;

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਖਾਲੀ sub_ਰੁੱਖ_print(ਬਣਤਰ mosquitto__subhier *ਜੜ੍ਹ, ਪੂਰਨ level)
{
	ਪੂਰਨ i;
	ਬਣਤਰ mosquitto__subhier *branch, *branch_tmp;
	ਬਣਤਰ mosquitto__subleaf *ਪੱਤੀ;

	HASH_ITER(hh, ਜੜ੍ਹ, branch, branch_tmp){
		ਜੇ(level > -1){
			ਲਈ(i=0; i<(level+2)*2; i++){
				ਲਿਖੋ(" ");
			}
			ਲਿਖੋ("%s", branch->topic);
			ਪੱਤੀ = branch->subs;
			ਜਦੋਂ(ਪੱਤੀ){
				ਜੇ(ਪੱਤੀ->context){
					ਲਿਖੋ(" (%s, %d)", ਪੱਤੀ->context->id, MQTT_SUB_OPT_GET_QOS(ਪੱਤੀ->subscription_options));
				}ਨਹੀਂਤਾਂ{
					ਲਿਖੋ(" (%s, %d)", "", MQTT_SUB_OPT_GET_QOS(ਪੱਤੀ->subscription_options));
				}
				ਪੱਤੀ = ਪੱਤੀ->ਅਗਲਾ;
			}
			ਲਿਖੋ("\n");
		}

		sub_ਰੁੱਖ_print(branch->children, level+1);
	}
}


ਪੂਰਨ sub__init(ਖਾਲੀ)
{
	HASH_ਮੁੱਲ("+", 1, hashv_plus);
	HASH_ਮੁੱਲ("#", 1, hashv_hash);

	ਜੇ(sub_ਜੋੜੋ_hier_ਇੰਦਰਾਜ(ਨਿਰਾ, &db.shared_subs, "", 0) == ਨਿਰਾ
			|| sub_ਜੋੜੋ_hier_ਇੰਦਰਾਜ(ਨਿਰਾ, &db.ਸਾਧਾਰਨ_subs, "", 0) == ਨਿਰਾ
			|| sub_ਜੋੜੋ_hier_ਇੰਦਰਾਜ(ਨਿਰਾ, &db.ਸਾਧਾਰਨ_subs, "$SYS", (uint16_t)ਲੜੀ_ਲੰਬਾਈ("$SYS")) == ਨਿਰਾ
			){

		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}
}
