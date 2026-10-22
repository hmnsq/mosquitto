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

ساكن بنية mosquitto__subhier *sub_جمع_کن_hier_مدخل(بنية mosquitto__subhier *والد, بنية mosquitto__subhier **sibling, ثابت حرف *topic, uint16_t len);

ساكن بیعلامت صحيح hashv_plus = 0;
ساكن بیعلامت صحيح hashv_hash = 0;


ساكن صحيح subs__send(بنية mosquitto__subleaf *برگ, ثابت حرف *topic, uint8_t qos, صحيح retain, بنية mosquitto_پایه_پیام *stored)
{
	bool client_retain;
	uint16_t mid;
	uint8_t client_qos, پیام_qos;
	صحيح rc2;

	/* Check for ACL topic access. */
	rc2 = mosquitto_acl_check(برگ->context, topic, stored->داده.payloadlen, stored->داده.payload, stored->داده.qos, stored->داده.retain, stored->داده.properties, MOSQ_ACL_READ);
	إذا(rc2 == MOSQ_ERR_ACL_DENIED){
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا إذا(rc2 == MOSQ_ERR_SUCCESS){
		client_qos = MQTT_SUB_OPT_GET_QOS(برگ->subscription_options);

		إذا(db.config->upgrade_outgoing_qos){
			پیام_qos = client_qos;
		}وإلا{
			إذا(qos > client_qos){
				پیام_qos = client_qos;
			}وإلا{
				پیام_qos = qos;
			}
		}
		إذا(پیام_qos){
			mid = mosquitto__mid_generate(برگ->context);
		}وإلا{
			mid = 0;
		}
		إذا(MQTT_SUB_OPT_GET_RETAIN_AS_PUBLISHED(برگ->subscription_options)){
			client_retain = retain;
		}وإلا{
			client_retain = false;
		}
		إذا(db_پیام_درج_outgoing(برگ->context, 0, mid, پیام_qos, client_retain, stored, برگ->identifier, true, true) == 1){
			أرجع 1;
		}
	}وإلا{
		أرجع 1; /* Application error */
	}
	أرجع 0;
}


ساكن صحيح subs__shared_process(بنية mosquitto__subhier *hier, ثابت حرف *topic, uint8_t qos, صحيح retain, بنية mosquitto_پایه_پیام *stored)
{
	صحيح rc = 0, rc2;
	بنية mosquitto__subshared *shared, *shared_tmp;
	بنية mosquitto__subleaf *برگ;

	HASH_ITER(hh, hier->shared, shared, shared_tmp){
		برگ = shared->subs;
		rc2 = subs__send(برگ, topic, qos, retain, stored);
		/* Remove current from the top, add back to the bottom */
		DL_DELETE(shared->subs, برگ);
		DL_APPEND(shared->subs, برگ);

		إذا(rc2){
			rc = 1;
		}
	}

	أرجع rc;
}


ساكن صحيح subs__process(بنية mosquitto__subhier *hier, ثابت حرف *منبع_id, ثابت حرف *topic, uint8_t qos, صحيح retain, بنية mosquitto_پایه_پیام *stored)
{
	صحيح rc = 0;
	صحيح rc2;
	بنية mosquitto__subleaf *برگ;

	rc = subs__shared_process(hier, topic, qos, retain, stored);

	برگ = hier->subs;
	طالما(منبع_id && برگ){
		إذا(!برگ->context->id || (MQTT_SUB_OPT_GET_NO_LOCAL(برگ->subscription_options) && !strcmp(برگ->context->id, منبع_id))){
			برگ = برگ->بعدی;
			استمر;
		}
		rc2 = subs__send(برگ, topic, qos, retain, stored);
		إذا(rc2){
			rc = 1;
		}
		برگ = برگ->بعدی;
	}
	إذا(hier->subs || hier->shared){
		أرجع rc;
	}وإلا{
		أرجع MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


ساكن صحيح sub_جمع_کن_برگ(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub, بنية mosquitto__subleaf **سر, بنية mosquitto__subleaf **newleaf)
{
	بنية mosquitto__subleaf *برگ;

	*newleaf = عدم;
	برگ = *سر;

	طالما(برگ){
		إذا(برگ->context && برگ->context->id && !strcmp(برگ->context->id, context->id)){
			/* Client making a second subscription to same topic. Only
			 * need to update QoS. Return MOSQ_ERR_SUB_EXISTS to
			 * indicate this to the calling function. */
			برگ->identifier = sub->identifier;
			برگ->subscription_options = sub->options;
			أرجع MOSQ_ERR_SUB_EXISTS;
		}
		برگ = برگ->بعدی;
	}
	برگ = mosquitto_calloc(1, حجم(بنية mosquitto__subleaf) + طول_رشته(sub->topic_filter) + 1);
	إذا(!برگ){
		أرجع MOSQ_ERR_NOMEM;
	}
	برگ->context = context;
	برگ->identifier = sub->identifier;
	برگ->subscription_options = sub->options;
	strcpy(برگ->topic_filter, sub->topic_filter);

	DL_APPEND(*سر, برگ);
	*newleaf = برگ;

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن فراغ sub_remove_shared_برگ(بنية mosquitto__subhier *subhier, بنية mosquitto__subshared *shared, بنية mosquitto__subleaf *برگ)
{
	DL_DELETE(shared->subs, برگ);
	إذا(shared->subs == عدم){
		HASH_DELETE(hh, subhier->shared, shared);
		mosquitto_FREE(shared);
	}
}


ساكن صحيح sub_جمع_کن_shared(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub, بنية mosquitto__subhier *subhier, ثابت حرف *sharename)
{
	بنية mosquitto__subleaf *newleaf;
	بنية mosquitto__subshared *shared = عدم;
	بنية mosquitto__subleaf **subs;
	size_t slen;
	صحيح rc;
	بیعلامت hashv;

	slen = طول_رشته(sharename);

	HASH_مقدار(sharename, slen, hashv);

	HASH_پیدا_کن_BYHASHVALUE(hh, subhier->shared, sharename, slen, hashv, shared);
	إذا(shared == عدم){
		shared = mosquitto_calloc(1, حجم(بنية mosquitto__subshared) + slen + 1);
		إذا(!shared){
			أرجع MOSQ_ERR_NOMEM;
		}
		strncpy(shared->name, sharename, slen+1);

		HASH_جمع_کن_BYHASHVALUE(hh, subhier->shared, name, slen, hashv, shared);
	}

	rc = sub_جمع_کن_برگ(context, sub, &shared->subs, &newleaf);
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
				context->subs_شمار++;
				assigned = true;
				اكسر;
			}
		}
		إذا(assigned == false){
			subs = mosquitto_realloc(context->subs, حجم(بنية mosquitto__subleaf *)*(size_t)(context->subs_ظرفیت + 1));
			إذا(!subs){
				sub_remove_shared_برگ(subhier, shared, newleaf);
				mosquitto_FREE(newleaf);
				أرجع MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_ظرفیت++;
			context->subs_شمار++;
			context->subs[context->subs_ظرفیت-1] = newleaf;
		}
#اگر_تعریف_شده WITH_SYS_درخت
		db.shared_subscription_شمار++;
#پایان_اگر
	}

	إذا(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		أرجع rc;
	}وإلا{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		أرجع MOSQ_ERR_SUCCESS;
	}
}


ساكن صحيح sub_جمع_کن_نرمال(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub, بنية mosquitto__subhier *subhier)
{
	بنية mosquitto__subleaf *newleaf = عدم;
	بنية mosquitto__subleaf **subs;
	صحيح rc;

	rc = sub_جمع_کن_برگ(context, sub, &subhier->subs, &newleaf);
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
				context->subs_شمار++;
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
			context->subs_شمار++;
			context->subs[context->subs_ظرفیت-1] = newleaf;
		}
#اگر_تعریف_شده WITH_SYS_درخت
		db.subscription_شمار++;
#پایان_اگر
	}

	إذا(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		أرجع rc;
	}وإلا{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		أرجع MOSQ_ERR_SUCCESS;
	}
}


ساكن صحيح sub_جمع_کن_context(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub, بنية mosquitto__subhier *subhier, حرف *ثابت *ثابت topics, ثابت حرف *sharename)
{
	بنية mosquitto__subhier *branch;
	صحيح topic_شاخص = 0;
	size_t topiclen;

	/* Find leaf node */
	طالما(topics && topics[topic_شاخص] != عدم){
		topiclen = طول_رشته(topics[topic_شاخص]);
		إذا(topiclen > UINT16_MAX){
			أرجع MOSQ_ERR_INVAL;
		}
		HASH_پیدا_کن(hh, subhier->children, topics[topic_شاخص], topiclen, branch);
		إذا(!branch){
			/* Not found */
			branch = sub_جمع_کن_hier_مدخل(subhier, &subhier->children, topics[topic_شاخص], (uint16_t)topiclen);
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
			أرجع sub_جمع_کن_shared(context, sub, subhier, sharename);
		}وإلا{
			أرجع sub_جمع_کن_نرمال(context, sub, subhier);
		}
	}وإلا{
		أرجع MOSQ_ERR_SUCCESS;
	}
}


ساكن صحيح sub_remove_نرمال(بنية mosquitto *context, بنية mosquitto__subhier *subhier, uint8_t *reason)
{
	بنية mosquitto__subleaf *برگ;

	برگ = subhier->subs;
	طالما(برگ){
		إذا(برگ->context==context){
#اگر_تعریف_شده WITH_SYS_درخت
			db.subscription_شمار--;
#پایان_اگر
			DL_DELETE(subhier->subs, برگ);

			/* Remove the reference to the sub that the client is keeping.
			 * It would be nice to be able to use the reference directly,
			 * but that would involve keeping a copy of the topic string in
			 * each subleaf. Might be worth considering though. */
			لكل(صحيح i=0; i<context->subs_ظرفیت; i++){
				إذا(context->subs[i] && context->subs[i]->hier == subhier){
					context->subs_شمار--;
					mosquitto_free(context->subs[i]);
					context->subs[i] = عدم;
					اكسر;
				}
			}
			*reason = 0;
			أرجع MOSQ_ERR_SUCCESS;
		}
		برگ = برگ->بعدی;
	}
	أرجع MOSQ_ERR_NO_SUBSCRIBERS;
}


ساكن صحيح sub__remove_shared(بنية mosquitto *context, بنية mosquitto__subhier *subhier, uint8_t *reason, ثابت حرف *sharename)
{
	بنية mosquitto__subshared *shared;

	HASH_پیدا_کن(hh, subhier->shared, sharename, طول_رشته(sharename), shared);
	إذا(shared){
		بنية mosquitto__subleaf *برگ = shared->subs;
		طالما(برگ){
			إذا(برگ->context==context){
#اگر_تعریف_شده WITH_SYS_درخت
				db.shared_subscription_شمار--;
#پایان_اگر
				DL_DELETE(shared->subs, برگ);

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
						context->subs_شمار--;
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
			برگ = برگ->بعدی;
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
			أرجع sub_remove_نرمال(context, subhier, reason);
		}
	}

	HASH_پیدا_کن(hh, subhier->children, topics[0], طول_رشته(topics[0]), branch);
	إذا(branch){
		sub__remove_recurse(context, branch, &(topics[1]), reason, sharename);
		إذا(!branch->children && !branch->subs && !branch->shared){
			HASH_DELETE(hh, subhier->children, branch);
			mosquitto_FREE(branch);
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح sub_جستجو(بنية mosquitto__subhier *subhier, حرف **تقسیم_topics, ثابت حرف *منبع_id, ثابت حرف *topic, uint8_t qos, صحيح retain, بنية mosquitto_پایه_پیام *stored)
{
	/* FIXME - need to take into account source_id if the client is a bridge */
	بنية mosquitto__subhier *branch;
	صحيح rc;
	bool have_subscribers = false;

	إذا(تقسیم_topics && تقسیم_topics[0]){
		/* Check for literal match */
		HASH_پیدا_کن(hh, subhier->children, تقسیم_topics[0], طول_رشته(تقسیم_topics[0]), branch);

		إذا(branch){
			rc = sub_جستجو(branch, &(تقسیم_topics[1]), منبع_id, topic, qos, retain, stored);
			إذا(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}وإلا إذا(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				أرجع rc;
			}
			إذا(تقسیم_topics[1] == عدم){ /* End of list */
				rc = subs__process(branch, منبع_id, topic, qos, retain, stored);
				إذا(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}وإلا إذا(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					أرجع rc;
				}
			}
		}

		/* Check for + match */
		HASH_پیدا_کن_BYHASHVALUE(hh, subhier->children, "+", 1, hashv_plus, branch);

		إذا(branch){
			rc = sub_جستجو(branch, &(تقسیم_topics[1]), منبع_id, topic, qos, retain, stored);
			إذا(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}وإلا إذا(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				أرجع rc;
			}
			إذا(تقسیم_topics[1] == عدم){ /* End of list */
				rc = subs__process(branch, منبع_id, topic, qos, retain, stored);
				إذا(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}وإلا إذا(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					أرجع rc;
				}
			}
		}
	}

	/* Check for # match */
	HASH_پیدا_کن_BYHASHVALUE(hh, subhier->children, "#", 1, hashv_hash, branch);
	إذا(branch && !branch->children){
		/* The topic matches due to a # wildcard - process the
		 * subscriptions but *don't* return. Although this branch has ended
		 * there may still be other subscriptions to deal with.
		 */
		rc = subs__process(branch, منبع_id, topic, qos, retain, stored);
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


ساكن بنية mosquitto__subhier *sub_جمع_کن_hier_مدخل(بنية mosquitto__subhier *والد, بنية mosquitto__subhier **sibling, ثابت حرف *topic, uint16_t len)
{
	بنية mosquitto__subhier *فرزند;

	أكد(sibling);

	فرزند = mosquitto_calloc(1, حجم(بنية mosquitto__subhier) + len + 1);
	إذا(!فرزند){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		أرجع عدم;
	}
	فرزند->والد = والد;
	فرزند->topic_len = len;
	إذا(len > 0){
		strncpy(فرزند->topic, topic, (size_t)(len+1));
	}

	HASH_جمع_کن(hh, *sibling, topic, فرزند->topic_len, فرزند);

	أرجع فرزند;
}


صحيح sub_جمع_کن(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub)
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

	topiclen = طول_رشته(topics[0]);
	إذا(topiclen > UINT16_MAX){
		mosquitto_FREE(local_sub);
		mosquitto_FREE(topics);
		أرجع MOSQ_ERR_INVAL;
	}

	إذا(sharename){
		HASH_پیدا_کن(hh, db.shared_subs, topics[0], topiclen, subhier);
		إذا(!subhier){
			subhier = sub_جمع_کن_hier_مدخل(عدم, &db.shared_subs, topics[0], (uint16_t)topiclen);
			إذا(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
				أرجع MOSQ_ERR_NOMEM;
			}
		}
	}وإلا{
		HASH_پیدا_کن(hh, db.نرمال_subs, topics[0], topiclen, subhier);
		إذا(!subhier){
			subhier = sub_جمع_کن_hier_مدخل(عدم, &db.نرمال_subs, topics[0], (uint16_t)topiclen);
			إذا(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
				أرجع MOSQ_ERR_NOMEM;
			}
		}
	}
	rc = sub_جمع_کن_context(context, sub, subhier, topics, sharename);

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
		HASH_پیدا_کن(hh, db.shared_subs, topics[0], طول_رشته(topics[0]), subhier);
	}وإلا{
		HASH_پیدا_کن(hh, db.نرمال_subs, topics[0], طول_رشته(topics[0]), subhier);
	}
	إذا(subhier){
		*reason = MQTT_RC_NO_SUBSCRIPTION_EXISTED;
		rc = sub__remove_recurse(context, subhier, topics, reason, sharename);
	}

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	أرجع rc;
}


صحيح sub_messages_صف(ثابت حرف *منبع_id, uint8_t qos, بنية mosquitto_پایه_پیام **stored)
{
	صحيح rc = MOSQ_ERR_SUCCESS, rc2;
	صحيح rc_نرمال = MOSQ_ERR_NO_SUBSCRIBERS, rc_shared = MOSQ_ERR_NO_SUBSCRIBERS;
	بنية mosquitto__subhier *subhier;
	حرف **تقسیم_topics = عدم;
	حرف *local_topic = عدم;
	بیعلامت hashv;
	size_t topiclen;
	bool retain = (*stored)->داده.retain;
	ثابت حرف *topic = (*stored)->داده.topic;

	أكد(topic);

	إذا(sub__topic_tokenise(topic, &local_topic, &تقسیم_topics, عدم)){
		أرجع 1;
	}

	/* Protect this message until we have sent it to all
	clients - this is required because websockets client calls
	db__message_write(), which could remove the message if ref_count==0.
	*/
	db_پیام_store_ref_inc(*stored);

	topiclen = طول_رشته(تقسیم_topics[0]);
	HASH_مقدار(تقسیم_topics[0], topiclen, hashv);
	HASH_پیدا_کن_BYHASHVALUE(hh, db.نرمال_subs, تقسیم_topics[0], topiclen, hashv, subhier);
	إذا(subhier){
		rc_نرمال = sub_جستجو(subhier, تقسیم_topics, منبع_id, topic, qos, retain, *stored);
		إذا(rc_نرمال > 0){
			rc = rc_نرمال;
			اذهب انتها;
		}
	}

	HASH_پیدا_کن_BYHASHVALUE(hh, db.shared_subs, تقسیم_topics[0], topiclen, hashv, subhier);
	إذا(subhier){
		rc_shared = sub_جستجو(subhier, تقسیم_topics, منبع_id, topic, qos, retain, *stored);
		إذا(rc_shared > 0){
			rc = rc_shared;
			اذهب انتها;
		}
	}

	إذا(rc_نرمال == MOSQ_ERR_NO_SUBSCRIBERS && rc_shared == MOSQ_ERR_NO_SUBSCRIBERS){
		rc = MOSQ_ERR_NO_SUBSCRIBERS;
	}

	إذا(retain){
		rc2 = retain__store(topic, *stored, تقسیم_topics, true);
		إذا(rc2){
			rc = rc2;
		}
	}

انتها:
	mosquitto_FREE(تقسیم_topics);
	mosquitto_FREE(local_topic);
	/* Remove our reference and free if needed. */
	db_پیام_store_ref_dec(stored);

	أرجع rc;
}


/* Remove a subhier element, and return its parent if that needs freeing as well. */
ساكن بنية mosquitto__subhier *tmp_remove_subs(بنية mosquitto__subhier *sub)
{
	بنية mosquitto__subhier *والد;

	إذا(!sub || !sub->والد){
		أرجع عدم;
	}

	إذا(sub->children || sub->subs){
		أرجع عدم;
	}

	والد = sub->والد;
	HASH_DELETE(hh, والد->children, sub);
	mosquitto_FREE(sub);

	إذا(والد->subs == عدم
			&& والد->children == عدم
			&& والد->shared == عدم
			&& والد->والد){

		أرجع والد;
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
		بنية mosquitto__subleaf *برگ;

		plugin_persist__handle_subscription_delete(context, context->subs[i]->topic_filter);
		إذا(context->subs[i]->shared){
			برگ = context->subs[i]->shared->subs;
			طالما(برگ){
				إذا(برگ->context==context){
#اگر_تعریف_شده WITH_SYS_درخت
					db.shared_subscription_شمار--;
#پایان_اگر
					sub_remove_shared_برگ(context->subs[i]->hier, context->subs[i]->shared, برگ);
					اكسر;
				}
				برگ = برگ->بعدی;
			}
		}وإلا{
			برگ = hier->subs;
			طالما(برگ){
				إذا(برگ->context==context){
#اگر_تعریف_شده WITH_SYS_درخت
					db.subscription_شمار--;
#پایان_اگر
					DL_DELETE(hier->subs, برگ);
					اكسر;
				}
				برگ = برگ->بعدی;
			}
		}
		mosquitto_FREE(context->subs[i]);

		إذا(hier->subs == عدم
				&& hier->children == عدم
				&& hier->shared == عدم
				&& hier->والد){

			افعل{
				hier = tmp_remove_subs(hier);
			}طالما(hier);
		}
	}
	mosquitto_FREE(context->subs);
	context->subs_ظرفیت = 0;
	context->subs_شمار = 0;

	أرجع MOSQ_ERR_SUCCESS;
}


فراغ sub_درخت_print(بنية mosquitto__subhier *ریشه, صحيح level)
{
	صحيح i;
	بنية mosquitto__subhier *branch, *branch_tmp;
	بنية mosquitto__subleaf *برگ;

	HASH_ITER(hh, ریشه, branch, branch_tmp){
		إذا(level > -1){
			لكل(i=0; i<(level+2)*2; i++){
				چاپ_کن(" ");
			}
			چاپ_کن("%s", branch->topic);
			برگ = branch->subs;
			طالما(برگ){
				إذا(برگ->context){
					چاپ_کن(" (%s, %d)", برگ->context->id, MQTT_SUB_OPT_GET_QOS(برگ->subscription_options));
				}وإلا{
					چاپ_کن(" (%s, %d)", "", MQTT_SUB_OPT_GET_QOS(برگ->subscription_options));
				}
				برگ = برگ->بعدی;
			}
			چاپ_کن("\n");
		}

		sub_درخت_print(branch->children, level+1);
	}
}


صحيح sub__init(فراغ)
{
	HASH_مقدار("+", 1, hashv_plus);
	HASH_مقدار("#", 1, hashv_hash);

	إذا(sub_جمع_کن_hier_مدخل(عدم, &db.shared_subs, "", 0) == عدم
			|| sub_جمع_کن_hier_مدخل(عدم, &db.نرمال_subs, "", 0) == عدم
			|| sub_جمع_کن_hier_مدخل(عدم, &db.نرمال_subs, "$SYS", (uint16_t)طول_رشته("$SYS")) == عدم
			){

		أرجع MOSQ_ERR_NOMEM;
	}وإلا{
		أرجع MOSQ_ERR_SUCCESS;
	}
}
