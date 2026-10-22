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

#ضمن "config.h"

#ضمن <assert.h>
#ضمن <stdio.h>
#ضمن <string.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "util_mosq.h"

#ضمن "utlist.h"

ساكن بنية mosquitto__subhier *sub_جمع_کړه_hier_ثبت(بنية mosquitto__subhier *پلار, بنية mosquitto__subhier **sibling, ثابت حرف *topic, uint16_t len);

ساكن بینښه صحيح hashv_plus = 0;
ساكن بینښه صحيح hashv_hash = 0;


ساكن صحيح subs__send(بنية mosquitto__subleaf *ونې_پاڼه, ثابت حرف *topic, uint8_t qos, صحيح retain, بنية mosquitto_بنسټ_پیغام *stored)
{
	bool client_retain;
	uint16_t mid;
	uint8_t client_qos, پیغام_qos;
	صحيح rc2;

	/* Check for ACL topic access. */
	rc2 = mosquitto_acl_check(ونې_پاڼه->context, topic, stored->معلومات.payloadlen, stored->معلومات.payload, stored->معلومات.qos, stored->معلومات.retain, stored->معلومات.properties, MOSQ_ACL_READ);
	إذا(rc2 == MOSQ_ERR_ACL_DENIED){
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا إذا(rc2 == MOSQ_ERR_SUCCESS){
		client_qos = MQTT_SUB_OPT_GET_QOS(ونې_پاڼه->subscription_options);

		إذا(db.config->upgrade_outgoing_qos){
			پیغام_qos = client_qos;
		}وإلا{
			إذا(qos > client_qos){
				پیغام_qos = client_qos;
			}وإلا{
				پیغام_qos = qos;
			}
		}
		إذا(پیغام_qos){
			mid = mosquitto__mid_generate(ونې_پاڼه->context);
		}وإلا{
			mid = 0;
		}
		إذا(MQTT_SUB_OPT_GET_RETAIN_AS_PUBLISHED(ونې_پاڼه->subscription_options)){
			client_retain = retain;
		}وإلا{
			client_retain = false;
		}
		إذا(db_پیغام_ورننباسه_outgoing(ونې_پاڼه->context, 0, mid, پیغام_qos, client_retain, stored, ونې_پاڼه->identifier, true, true) == 1){
			أرجع 1;
		}
	}وإلا{
		أرجع 1; /* Application error */
	}
	أرجع 0;
}


ساكن صحيح subs__shared_process(بنية mosquitto__subhier *hier, ثابت حرف *topic, uint8_t qos, صحيح retain, بنية mosquitto_بنسټ_پیغام *stored)
{
	صحيح rc = 0, rc2;
	بنية mosquitto__subshared *shared, *shared_tmp;
	بنية mosquitto__subleaf *ونې_پاڼه;

	HASH_ITER(hh, hier->shared, shared, shared_tmp){
		ونې_پاڼه = shared->subs;
		rc2 = subs__send(ونې_پاڼه, topic, qos, retain, stored);
		/* Remove current from the top, add back to the bottom */
		DL_DELETE(shared->subs, ونې_پاڼه);
		DL_APPEND(shared->subs, ونې_پاڼه);

		إذا(rc2){
			rc = 1;
		}
	}

	أرجع rc;
}


ساكن صحيح subs__process(بنية mosquitto__subhier *hier, ثابت حرف *سرچینه_id, ثابت حرف *topic, uint8_t qos, صحيح retain, بنية mosquitto_بنسټ_پیغام *stored)
{
	صحيح rc = 0;
	صحيح rc2;
	بنية mosquitto__subleaf *ونې_پاڼه;

	rc = subs__shared_process(hier, topic, qos, retain, stored);

	ونې_پاڼه = hier->subs;
	طالما(سرچینه_id && ونې_پاڼه){
		إذا(!ونې_پاڼه->context->id || (MQTT_SUB_OPT_GET_NO_LOCAL(ونې_پاڼه->subscription_options) && !strcmp(ونې_پاڼه->context->id, سرچینه_id))){
			ونې_پاڼه = ونې_پاڼه->بل;
			استمر;
		}
		rc2 = subs__send(ونې_پاڼه, topic, qos, retain, stored);
		إذا(rc2){
			rc = 1;
		}
		ونې_پاڼه = ونې_پاڼه->بل;
	}
	إذا(hier->subs || hier->shared){
		أرجع rc;
	}وإلا{
		أرجع MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


ساكن صحيح sub_جمع_کړه_ونې_پاڼه(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub, بنية mosquitto__subleaf **سر, بنية mosquitto__subleaf **newleaf)
{
	بنية mosquitto__subleaf *ونې_پاڼه;

	*newleaf = عدم;
	ونې_پاڼه = *سر;

	طالما(ونې_پاڼه){
		إذا(ونې_پاڼه->context && ونې_پاڼه->context->id && !strcmp(ونې_پاڼه->context->id, context->id)){
			/* Client making a second subscription to same topic. Only
			 * need to update QoS. Return MOSQ_ERR_SUB_EXISTS to
			 * indicate this to the calling function. */
			ونې_پاڼه->identifier = sub->identifier;
			ونې_پاڼه->subscription_options = sub->options;
			أرجع MOSQ_ERR_SUB_EXISTS;
		}
		ونې_پاڼه = ونې_پاڼه->بل;
	}
	ونې_پاڼه = mosquitto_calloc(1, حجم(بنية mosquitto__subleaf) + مزي_اوږدوالی(sub->topic_filter) + 1);
	إذا(!ونې_پاڼه){
		أرجع MOSQ_ERR_NOMEM;
	}
	ونې_پاڼه->context = context;
	ونې_پاڼه->identifier = sub->identifier;
	ونې_پاڼه->subscription_options = sub->options;
	strcpy(ونې_پاڼه->topic_filter, sub->topic_filter);

	DL_APPEND(*سر, ونې_پاڼه);
	*newleaf = ونې_پاڼه;

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن فراغ sub_remove_shared_ونې_پاڼه(بنية mosquitto__subhier *subhier, بنية mosquitto__subshared *shared, بنية mosquitto__subleaf *ونې_پاڼه)
{
	DL_DELETE(shared->subs, ونې_پاڼه);
	إذا(shared->subs == عدم){
		HASH_DELETE(hh, subhier->shared, shared);
		mosquitto_FREE(shared);
	}
}


ساكن صحيح sub_جمع_کړه_shared(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub, بنية mosquitto__subhier *subhier, ثابت حرف *sharename)
{
	بنية mosquitto__subleaf *newleaf;
	بنية mosquitto__subshared *shared = عدم;
	بنية mosquitto__subleaf **subs;
	size_t slen;
	صحيح rc;
	بینښه hashv;

	slen = مزي_اوږدوالی(sharename);

	HASH_ارزښت(sharename, slen, hashv);

	HASH_پیدا_کړه_BYHASHVALUE(hh, subhier->shared, sharename, slen, hashv, shared);
	إذا(shared == عدم){
		shared = mosquitto_calloc(1, حجم(بنية mosquitto__subshared) + slen + 1);
		إذا(!shared){
			أرجع MOSQ_ERR_NOMEM;
		}
		strncpy(shared->name, sharename, slen+1);

		HASH_جمع_کړه_BYHASHVALUE(hh, subhier->shared, name, slen, hashv, shared);
	}

	rc = sub_جمع_کړه_ونې_پاڼه(context, sub, &shared->subs, &newleaf);
	إذا(rc > 0){
		إذا(shared->subs == عدم){
			HASH_DELETE(hh, subhier->shared, shared);
			mosquitto_FREE(shared);
		}
		أرجع rc;
	}

	إذا(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = shared;

		bool assigned = false;
		لكل(صحيح i=0; i<context->subs_ظرفیت; i++){
			إذا(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_شمېر++;
				assigned = true;
				اكسر;
			}
		}
		إذا(assigned == false){
			subs = mosquitto_realloc(context->subs, حجم(بنية mosquitto__subleaf *)*(size_t)(context->subs_ظرفیت + 1));
			إذا(!subs){
				sub_remove_shared_ونې_پاڼه(subhier, shared, newleaf);
				mosquitto_FREE(newleaf);
				أرجع MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_ظرفیت++;
			context->subs_شمېر++;
			context->subs[context->subs_ظرفیت-1] = newleaf;
		}
#که_تعریف_شوی WITH_SYS_ونه
		db.shared_subscription_شمېر++;
#د_که_پای
	}

	إذا(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		أرجع rc;
	}وإلا{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		أرجع MOSQ_ERR_SUCCESS;
	}
}


ساكن صحيح sub_جمع_کړه_نورمال(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub, بنية mosquitto__subhier *subhier)
{
	بنية mosquitto__subleaf *newleaf = عدم;
	بنية mosquitto__subleaf **subs;
	صحيح rc;

	rc = sub_جمع_کړه_ونې_پاڼه(context, sub, &subhier->subs, &newleaf);
	إذا(rc > 0){
		أرجع rc;
	}

	إذا(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = عدم;

		bool assigned = false;
		لكل(صحيح i=0; i<context->subs_ظرفیت; i++){
			إذا(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_شمېر++;
				assigned = true;
				اكسر;
			}
		}
		إذا(assigned == false){
			subs = mosquitto_realloc(context->subs, حجم(بنية mosquitto__subleaf *)*(size_t)(context->subs_ظرفیت + 1));
			إذا(!subs){
				DL_DELETE(subhier->subs, newleaf);
				mosquitto_FREE(newleaf);
				أرجع MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_ظرفیت++;
			context->subs_شمېر++;
			context->subs[context->subs_ظرفیت-1] = newleaf;
		}
#که_تعریف_شوی WITH_SYS_ونه
		db.subscription_شمېر++;
#د_که_پای
	}

	إذا(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		أرجع rc;
	}وإلا{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		أرجع MOSQ_ERR_SUCCESS;
	}
}


ساكن صحيح sub_جمع_کړه_context(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub, بنية mosquitto__subhier *subhier, حرف *ثابت *ثابت topics, ثابت حرف *sharename)
{
	بنية mosquitto__subhier *branch;
	صحيح topic_شاخص = 0;
	size_t topiclen;

	/* Find leaf node */
	طالما(topics && topics[topic_شاخص] != عدم){
		topiclen = مزي_اوږدوالی(topics[topic_شاخص]);
		إذا(topiclen > UINT16_MAX){
			أرجع MOSQ_ERR_INVAL;
		}
		HASH_پیدا_کړه(hh, subhier->children, topics[topic_شاخص], topiclen, branch);
		إذا(!branch){
			/* Not found */
			branch = sub_جمع_کړه_hier_ثبت(subhier, &subhier->children, topics[topic_شاخص], (uint16_t)topiclen);
			إذا(!branch){
				أرجع MOSQ_ERR_NOMEM;
			}
		}
		subhier = branch;
		topic_شاخص++;
	}

	/* Add add our context */
	إذا(context && context->id){
		إذا(sharename){
			أرجع sub_جمع_کړه_shared(context, sub, subhier, sharename);
		}وإلا{
			أرجع sub_جمع_کړه_نورمال(context, sub, subhier);
		}
	}وإلا{
		أرجع MOSQ_ERR_SUCCESS;
	}
}


ساكن صحيح sub_remove_نورمال(بنية mosquitto *context, بنية mosquitto__subhier *subhier, uint8_t *reason)
{
	بنية mosquitto__subleaf *ونې_پاڼه;

	ونې_پاڼه = subhier->subs;
	طالما(ونې_پاڼه){
		إذا(ونې_پاڼه->context==context){
#که_تعریف_شوی WITH_SYS_ونه
			db.subscription_شمېر--;
#د_که_پای
			DL_DELETE(subhier->subs, ونې_پاڼه);

			/* Remove the reference to the sub that the client is keeping.
			 * It would be nice to be able to use the reference directly,
			 * but that would involve keeping a copy of the topic string in
			 * each subleaf. Might be worth considering though. */
			لكل(صحيح i=0; i<context->subs_ظرفیت; i++){
				إذا(context->subs[i] && context->subs[i]->hier == subhier){
					context->subs_شمېر--;
					mosquitto_free(context->subs[i]);
					context->subs[i] = عدم;
					اكسر;
				}
			}
			*reason = 0;
			أرجع MOSQ_ERR_SUCCESS;
		}
		ونې_پاڼه = ونې_پاڼه->بل;
	}
	أرجع MOSQ_ERR_NO_SUBSCRIBERS;
}


ساكن صحيح sub__remove_shared(بنية mosquitto *context, بنية mosquitto__subhier *subhier, uint8_t *reason, ثابت حرف *sharename)
{
	بنية mosquitto__subshared *shared;

	HASH_پیدا_کړه(hh, subhier->shared, sharename, مزي_اوږدوالی(sharename), shared);
	إذا(shared){
		بنية mosquitto__subleaf *ونې_پاڼه = shared->subs;
		طالما(ونې_پاڼه){
			إذا(ونې_پاڼه->context==context){
#که_تعریف_شوی WITH_SYS_ونه
				db.shared_subscription_شمېر--;
#د_که_پای
				DL_DELETE(shared->subs, ونې_پاڼه);

				/* Remove the reference to the sub that the client is keeping.
				* It would be nice to be able to use the reference directly,
				* but that would involve keeping a copy of the topic string in
				* each subleaf. Might be worth considering though. */
				لكل(صحيح i=0; i<context->subs_ظرفیت; i++){
					إذا(context->subs[i]
							&& context->subs[i]->hier == subhier
							&& context->subs[i]->shared == shared){

						mosquitto_free(context->subs[i]);
						context->subs[i] = عدم;
						context->subs_شمېر--;
						اكسر;
					}
				}

				إذا(shared->subs == عدم){
					HASH_DELETE(hh, subhier->shared, shared);
					mosquitto_FREE(shared);
				}

				*reason = 0;
				أرجع MOSQ_ERR_SUCCESS;
			}
			ونې_پاڼه = ونې_پاڼه->بل;
		}
		أرجع MOSQ_ERR_NO_SUBSCRIBERS;
	}وإلا{
		أرجع MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


ساكن صحيح sub__remove_recurse(بنية mosquitto *context, بنية mosquitto__subhier *subhier, حرف **topics, uint8_t *reason, ثابت حرف *sharename)
{
	بنية mosquitto__subhier *branch;

	إذا(topics == عدم || topics[0] == عدم){
		إذا(sharename){
			أرجع sub__remove_shared(context, subhier, reason, sharename);
		}وإلا{
			أرجع sub_remove_نورمال(context, subhier, reason);
		}
	}

	HASH_پیدا_کړه(hh, subhier->children, topics[0], مزي_اوږدوالی(topics[0]), branch);
	إذا(branch){
		sub__remove_recurse(context, branch, &(topics[1]), reason, sharename);
		إذا(!branch->children && !branch->subs && !branch->shared){
			HASH_DELETE(hh, subhier->children, branch);
			mosquitto_FREE(branch);
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح sub_پلټنه(بنية mosquitto__subhier *subhier, حرف **وېشل_topics, ثابت حرف *سرچینه_id, ثابت حرف *topic, uint8_t qos, صحيح retain, بنية mosquitto_بنسټ_پیغام *stored)
{
	/* FIXME - need to take into account source_id if the client is a bridge */
	بنية mosquitto__subhier *branch;
	صحيح rc;
	bool have_subscribers = false;

	إذا(وېشل_topics && وېشل_topics[0]){
		/* Check for literal match */
		HASH_پیدا_کړه(hh, subhier->children, وېشل_topics[0], مزي_اوږدوالی(وېشل_topics[0]), branch);

		إذا(branch){
			rc = sub_پلټنه(branch, &(وېشل_topics[1]), سرچینه_id, topic, qos, retain, stored);
			إذا(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}وإلا إذا(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				أرجع rc;
			}
			إذا(وېشل_topics[1] == عدم){ /* End of list */
				rc = subs__process(branch, سرچینه_id, topic, qos, retain, stored);
				إذا(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}وإلا إذا(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					أرجع rc;
				}
			}
		}

		/* Check for + match */
		HASH_پیدا_کړه_BYHASHVALUE(hh, subhier->children, "+", 1, hashv_plus, branch);

		إذا(branch){
			rc = sub_پلټنه(branch, &(وېشل_topics[1]), سرچینه_id, topic, qos, retain, stored);
			إذا(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}وإلا إذا(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				أرجع rc;
			}
			إذا(وېشل_topics[1] == عدم){ /* End of list */
				rc = subs__process(branch, سرچینه_id, topic, qos, retain, stored);
				إذا(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}وإلا إذا(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					أرجع rc;
				}
			}
		}
	}

	/* Check for # match */
	HASH_پیدا_کړه_BYHASHVALUE(hh, subhier->children, "#", 1, hashv_hash, branch);
	إذا(branch && !branch->children){
		/* The topic matches due to a # wildcard - process the
		 * subscriptions but *don't* return. Although this branch has ended
		 * there may still be other subscriptions to deal with.
		 */
		rc = subs__process(branch, سرچینه_id, topic, qos, retain, stored);
		إذا(rc == MOSQ_ERR_SUCCESS){
			have_subscribers = true;
		}وإلا إذا(rc != MOSQ_ERR_NO_SUBSCRIBERS){
			أرجع rc;
		}
	}

	إذا(have_subscribers){
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا{
		أرجع MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


ساكن بنية mosquitto__subhier *sub_جمع_کړه_hier_ثبت(بنية mosquitto__subhier *پلار, بنية mosquitto__subhier **sibling, ثابت حرف *topic, uint16_t len)
{
	بنية mosquitto__subhier *ماشوم;

	أكد(sibling);

	ماشوم = mosquitto_calloc(1, حجم(بنية mosquitto__subhier) + len + 1);
	إذا(!ماشوم){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		أرجع عدم;
	}
	ماشوم->پلار = پلار;
	ماشوم->topic_len = len;
	إذا(len > 0){
		strncpy(ماشوم->topic, topic, (size_t)(len+1));
	}

	HASH_جمع_کړه(hh, *sibling, topic, ماشوم->topic_len, ماشوم);

	أرجع ماشوم;
}


صحيح sub_جمع_کړه(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub)
{
	صحيح rc = 0;
	بنية mosquitto__subhier *subhier;
	ثابت حرف *sharename = عدم;
	حرف *local_sub;
	حرف **topics;
	size_t topiclen;

	أكد(sub);
	أكد(sub->topic_filter);

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &topics, &sharename);
	إذا(rc){
		أرجع rc;
	}

	topiclen = مزي_اوږدوالی(topics[0]);
	إذا(topiclen > UINT16_MAX){
		mosquitto_FREE(local_sub);
		mosquitto_FREE(topics);
		أرجع MOSQ_ERR_INVAL;
	}

	إذا(sharename){
		HASH_پیدا_کړه(hh, db.shared_subs, topics[0], topiclen, subhier);
		إذا(!subhier){
			subhier = sub_جمع_کړه_hier_ثبت(عدم, &db.shared_subs, topics[0], (uint16_t)topiclen);
			إذا(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
				أرجع MOSQ_ERR_NOMEM;
			}
		}
	}وإلا{
		HASH_پیدا_کړه(hh, db.نورمال_subs, topics[0], topiclen, subhier);
		إذا(!subhier){
			subhier = sub_جمع_کړه_hier_ثبت(عدم, &db.نورمال_subs, topics[0], (uint16_t)topiclen);
			إذا(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
				أرجع MOSQ_ERR_NOMEM;
			}
		}
	}
	rc = sub_جمع_کړه_context(context, sub, subhier, topics, sharename);

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	أرجع rc;
}


صحيح sub__remove(بنية mosquitto *context, ثابت حرف *sub, uint8_t *reason)
{
	صحيح rc = 0;
	بنية mosquitto__subhier *subhier;
	ثابت حرف *sharename = عدم;
	حرف *local_sub = عدم;
	حرف **topics = عدم;

	أكد(sub);

	rc = sub__topic_tokenise(sub, &local_sub, &topics, &sharename);
	إذا(rc){
		أرجع rc;
	}

	إذا(sharename){
		HASH_پیدا_کړه(hh, db.shared_subs, topics[0], مزي_اوږدوالی(topics[0]), subhier);
	}وإلا{
		HASH_پیدا_کړه(hh, db.نورمال_subs, topics[0], مزي_اوږدوالی(topics[0]), subhier);
	}
	إذا(subhier){
		*reason = MQTT_RC_NO_SUBSCRIPTION_EXISTED;
		rc = sub__remove_recurse(context, subhier, topics, reason, sharename);
	}

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	أرجع rc;
}


صحيح sub_messages_کتار(ثابت حرف *سرچینه_id, uint8_t qos, بنية mosquitto_بنسټ_پیغام **stored)
{
	صحيح rc = MOSQ_ERR_SUCCESS, rc2;
	صحيح rc_نورمال = MOSQ_ERR_NO_SUBSCRIBERS, rc_shared = MOSQ_ERR_NO_SUBSCRIBERS;
	بنية mosquitto__subhier *subhier;
	حرف **وېشل_topics = عدم;
	حرف *local_topic = عدم;
	بینښه hashv;
	size_t topiclen;
	bool retain = (*stored)->معلومات.retain;
	ثابت حرف *topic = (*stored)->معلومات.topic;

	أكد(topic);

	إذا(sub__topic_tokenise(topic, &local_topic, &وېشل_topics, عدم)){
		أرجع 1;
	}

	/* Protect this message until we have sent it to all
	clients - this is required because websockets client calls
	db__message_write(), which could remove the message if ref_count==0.
	*/
	db_پیغام_store_ref_inc(*stored);

	topiclen = مزي_اوږدوالی(وېشل_topics[0]);
	HASH_ارزښت(وېشل_topics[0], topiclen, hashv);
	HASH_پیدا_کړه_BYHASHVALUE(hh, db.نورمال_subs, وېشل_topics[0], topiclen, hashv, subhier);
	إذا(subhier){
		rc_نورمال = sub_پلټنه(subhier, وېشل_topics, سرچینه_id, topic, qos, retain, *stored);
		إذا(rc_نورمال > 0){
			rc = rc_نورمال;
			اذهب پای_ټکی;
		}
	}

	HASH_پیدا_کړه_BYHASHVALUE(hh, db.shared_subs, وېشل_topics[0], topiclen, hashv, subhier);
	إذا(subhier){
		rc_shared = sub_پلټنه(subhier, وېشل_topics, سرچینه_id, topic, qos, retain, *stored);
		إذا(rc_shared > 0){
			rc = rc_shared;
			اذهب پای_ټکی;
		}
	}

	إذا(rc_نورمال == MOSQ_ERR_NO_SUBSCRIBERS && rc_shared == MOSQ_ERR_NO_SUBSCRIBERS){
		rc = MOSQ_ERR_NO_SUBSCRIBERS;
	}

	إذا(retain){
		rc2 = retain__store(topic, *stored, وېشل_topics, true);
		إذا(rc2){
			rc = rc2;
		}
	}

پای_ټکی:
	mosquitto_FREE(وېشل_topics);
	mosquitto_FREE(local_topic);
	/* Remove our reference and free if needed. */
	db_پیغام_store_ref_dec(stored);

	أرجع rc;
}


/* Remove a subhier element, and return its parent if that needs freeing as well. */
ساكن بنية mosquitto__subhier *tmp_remove_subs(بنية mosquitto__subhier *sub)
{
	بنية mosquitto__subhier *پلار;

	إذا(!sub || !sub->پلار){
		أرجع عدم;
	}

	إذا(sub->children || sub->subs){
		أرجع عدم;
	}

	پلار = sub->پلار;
	HASH_DELETE(hh, پلار->children, sub);
	mosquitto_FREE(sub);

	إذا(پلار->subs == عدم
			&& پلار->children == عدم
			&& پلار->shared == عدم
			&& پلار->پلار){

		أرجع پلار;
	}وإلا{
		أرجع عدم;
	}
}


/* Remove all subscriptions for a client.
 */
صحيح sub__clean_session(بنية mosquitto *context)
{
	لكل(صحيح i=0; i<context->subs_ظرفیت; i++){
		إذا(context->subs[i] == عدم || context->subs[i]->hier == عدم){
			استمر;
		}

		بنية mosquitto__subhier *hier = context->subs[i]->hier;
		بنية mosquitto__subleaf *ونې_پاڼه;

		plugin_persist__handle_subscription_delete(context, context->subs[i]->topic_filter);
		إذا(context->subs[i]->shared){
			ونې_پاڼه = context->subs[i]->shared->subs;
			طالما(ونې_پاڼه){
				إذا(ونې_پاڼه->context==context){
#که_تعریف_شوی WITH_SYS_ونه
					db.shared_subscription_شمېر--;
#د_که_پای
					sub_remove_shared_ونې_پاڼه(context->subs[i]->hier, context->subs[i]->shared, ونې_پاڼه);
					اكسر;
				}
				ونې_پاڼه = ونې_پاڼه->بل;
			}
		}وإلا{
			ونې_پاڼه = hier->subs;
			طالما(ونې_پاڼه){
				إذا(ونې_پاڼه->context==context){
#که_تعریف_شوی WITH_SYS_ونه
					db.subscription_شمېر--;
#د_که_پای
					DL_DELETE(hier->subs, ونې_پاڼه);
					اكسر;
				}
				ونې_پاڼه = ونې_پاڼه->بل;
			}
		}
		mosquitto_FREE(context->subs[i]);

		إذا(hier->subs == عدم
				&& hier->children == عدم
				&& hier->shared == عدم
				&& hier->پلار){

			افعل{
				hier = tmp_remove_subs(hier);
			}طالما(hier);
		}
	}
	mosquitto_FREE(context->subs);
	context->subs_ظرفیت = 0;
	context->subs_شمېر = 0;

	أرجع MOSQ_ERR_SUCCESS;
}


فراغ sub_ونه_print(بنية mosquitto__subhier *ریښه, صحيح level)
{
	صحيح i;
	بنية mosquitto__subhier *branch, *branch_tmp;
	بنية mosquitto__subleaf *ونې_پاڼه;

	HASH_ITER(hh, ریښه, branch, branch_tmp){
		إذا(level > -1){
			لكل(i=0; i<(level+2)*2; i++){
				چاپ_کړه(" ");
			}
			چاپ_کړه("%s", branch->topic);
			ونې_پاڼه = branch->subs;
			طالما(ونې_پاڼه){
				إذا(ونې_پاڼه->context){
					چاپ_کړه(" (%s, %d)", ونې_پاڼه->context->id, MQTT_SUB_OPT_GET_QOS(ونې_پاڼه->subscription_options));
				}وإلا{
					چاپ_کړه(" (%s, %d)", "", MQTT_SUB_OPT_GET_QOS(ونې_پاڼه->subscription_options));
				}
				ونې_پاڼه = ونې_پاڼه->بل;
			}
			چاپ_کړه("\n");
		}

		sub_ونه_print(branch->children, level+1);
	}
}


صحيح sub__init(فراغ)
{
	HASH_ارزښت("+", 1, hashv_plus);
	HASH_ارزښت("#", 1, hashv_hash);

	إذا(sub_جمع_کړه_hier_ثبت(عدم, &db.shared_subs, "", 0) == عدم
			|| sub_جمع_کړه_hier_ثبت(عدم, &db.نورمال_subs, "", 0) == عدم
			|| sub_جمع_کړه_hier_ثبت(عدم, &db.نورمال_subs, "$SYS", (uint16_t)مزي_اوږدوالی("$SYS")) == عدم
			){

		أرجع MOSQ_ERR_NOMEM;
	}وإلا{
		أرجع MOSQ_ERR_SUCCESS;
	}
}
