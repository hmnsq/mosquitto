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

#شامل "config.h"

#شامل <assert.h>
#شامل <stdio.h>
#شامل <string.h>

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "util_mosq.h"

#شامل "utlist.h"

ساکن ساخت mosquitto__subhier *sub_جوڑو_hier_اندراج(ساخت mosquitto__subhier *والد, ساخت mosquitto__subhier **sibling, ثابت حرف *topic, uint16_t len);

ساکن غیرعلامتی صحیح hashv_plus = 0;
ساکن غیرعلامتی صحیح hashv_hash = 0;


ساکن صحیح subs__send(ساخت mosquitto__subleaf *برگ, ثابت حرف *topic, uint8_t qos, صحیح retain, ساخت mosquitto_بنیاد_پیغام *stored)
{
	bool client_retain;
	uint16_t mid;
	uint8_t client_qos, پیغام_qos;
	صحیح rc2;

	/* Check for ACL topic access. */
	rc2 = mosquitto_acl_check(برگ->context, topic, stored->ڈیٹا.payloadlen, stored->ڈیٹا.payload, stored->ڈیٹا.qos, stored->ڈیٹا.retain, stored->ڈیٹا.properties, MOSQ_ACL_READ);
	اگر(rc2 == MOSQ_ERR_ACL_DENIED){
		واپس MOSQ_ERR_SUCCESS;
	}ورنہ اگر(rc2 == MOSQ_ERR_SUCCESS){
		client_qos = MQTT_SUB_OPT_GET_QOS(برگ->subscription_options);

		اگر(db.config->upgrade_outgoing_qos){
			پیغام_qos = client_qos;
		}ورنہ{
			اگر(qos > client_qos){
				پیغام_qos = client_qos;
			}ورنہ{
				پیغام_qos = qos;
			}
		}
		اگر(پیغام_qos){
			mid = mosquitto__mid_generate(برگ->context);
		}ورنہ{
			mid = 0;
		}
		اگر(MQTT_SUB_OPT_GET_RETAIN_AS_PUBLISHED(برگ->subscription_options)){
			client_retain = retain;
		}ورنہ{
			client_retain = false;
		}
		اگر(db_پیغام_داخل_کرو_outgoing(برگ->context, 0, mid, پیغام_qos, client_retain, stored, برگ->identifier, true, true) == 1){
			واپس 1;
		}
	}ورنہ{
		واپس 1; /* Application error */
	}
	واپس 0;
}


ساکن صحیح subs__shared_process(ساخت mosquitto__subhier *hier, ثابت حرف *topic, uint8_t qos, صحیح retain, ساخت mosquitto_بنیاد_پیغام *stored)
{
	صحیح rc = 0, rc2;
	ساخت mosquitto__subshared *shared, *shared_tmp;
	ساخت mosquitto__subleaf *برگ;

	HASH_ITER(hh, hier->shared, shared, shared_tmp){
		برگ = shared->subs;
		rc2 = subs__send(برگ, topic, qos, retain, stored);
		/* Remove current from the top, add back to the bottom */
		DL_DELETE(shared->subs, برگ);
		DL_APPEND(shared->subs, برگ);

		اگر(rc2){
			rc = 1;
		}
	}

	واپس rc;
}


ساکن صحیح subs__process(ساخت mosquitto__subhier *hier, ثابت حرف *ماخذ_id, ثابت حرف *topic, uint8_t qos, صحیح retain, ساخت mosquitto_بنیاد_پیغام *stored)
{
	صحیح rc = 0;
	صحیح rc2;
	ساخت mosquitto__subleaf *برگ;

	rc = subs__shared_process(hier, topic, qos, retain, stored);

	برگ = hier->subs;
	جب(ماخذ_id && برگ){
		اگر(!برگ->context->id || (MQTT_SUB_OPT_GET_NO_LOCAL(برگ->subscription_options) && !strcmp(برگ->context->id, ماخذ_id))){
			برگ = برگ->اگلا;
			جاری;
		}
		rc2 = subs__send(برگ, topic, qos, retain, stored);
		اگر(rc2){
			rc = 1;
		}
		برگ = برگ->اگلا;
	}
	اگر(hier->subs || hier->shared){
		واپس rc;
	}ورنہ{
		واپس MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


ساکن صحیح sub_جوڑو_برگ(ساخت mosquitto *context, ثابت ساخت mosquitto_subscription *sub, ساخت mosquitto__subleaf **سر, ساخت mosquitto__subleaf **newleaf)
{
	ساخت mosquitto__subleaf *برگ;

	*newleaf = تہی;
	برگ = *سر;

	جب(برگ){
		اگر(برگ->context && برگ->context->id && !strcmp(برگ->context->id, context->id)){
			/* Client making a second subscription to same topic. Only
			 * need to update QoS. Return MOSQ_ERR_SUB_EXISTS to
			 * indicate this to the calling function. */
			برگ->identifier = sub->identifier;
			برگ->subscription_options = sub->options;
			واپس MOSQ_ERR_SUB_EXISTS;
		}
		برگ = برگ->اگلا;
	}
	برگ = mosquitto_calloc(1, حجم(ساخت mosquitto__subleaf) + لڑی_لمبائی(sub->topic_filter) + 1);
	اگر(!برگ){
		واپس MOSQ_ERR_NOMEM;
	}
	برگ->context = context;
	برگ->identifier = sub->identifier;
	برگ->subscription_options = sub->options;
	strcpy(برگ->topic_filter, sub->topic_filter);

	DL_APPEND(*سر, برگ);
	*newleaf = برگ;

	واپس MOSQ_ERR_SUCCESS;
}


ساکن خالی sub_remove_shared_برگ(ساخت mosquitto__subhier *subhier, ساخت mosquitto__subshared *shared, ساخت mosquitto__subleaf *برگ)
{
	DL_DELETE(shared->subs, برگ);
	اگر(shared->subs == تہی){
		HASH_DELETE(hh, subhier->shared, shared);
		mosquitto_FREE(shared);
	}
}


ساکن صحیح sub_جوڑو_shared(ساخت mosquitto *context, ثابت ساخت mosquitto_subscription *sub, ساخت mosquitto__subhier *subhier, ثابت حرف *sharename)
{
	ساخت mosquitto__subleaf *newleaf;
	ساخت mosquitto__subshared *shared = تہی;
	ساخت mosquitto__subleaf **subs;
	size_t slen;
	صحیح rc;
	غیرعلامتی hashv;

	slen = لڑی_لمبائی(sharename);

	HASH_قدر(sharename, slen, hashv);

	HASH_ڈھونڈو_BYHASHVALUE(hh, subhier->shared, sharename, slen, hashv, shared);
	اگر(shared == تہی){
		shared = mosquitto_calloc(1, حجم(ساخت mosquitto__subshared) + slen + 1);
		اگر(!shared){
			واپس MOSQ_ERR_NOMEM;
		}
		strncpy(shared->name, sharename, slen+1);

		HASH_جوڑو_BYHASHVALUE(hh, subhier->shared, name, slen, hashv, shared);
	}

	rc = sub_جوڑو_برگ(context, sub, &shared->subs, &newleaf);
	اگر(rc > 0){
		اگر(shared->subs == تہی){
			HASH_DELETE(hh, subhier->shared, shared);
			mosquitto_FREE(shared);
		}
		واپس rc;
	}

	اگر(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = shared;

		bool assigned = false;
		برائے(صحیح i=0; i<context->subs_استعداد; i++){
			اگر(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_گنتی++;
				assigned = true;
				توڑو;
			}
		}
		اگر(assigned == false){
			subs = mosquitto_realloc(context->subs, حجم(ساخت mosquitto__subleaf *)*(size_t)(context->subs_استعداد + 1));
			اگر(!subs){
				sub_remove_shared_برگ(subhier, shared, newleaf);
				mosquitto_FREE(newleaf);
				واپس MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_استعداد++;
			context->subs_گنتی++;
			context->subs[context->subs_استعداد-1] = newleaf;
		}
#اگر_تعریف_شدہ WITH_SYS_درخت
		db.shared_subscription_گنتی++;
#اگر_ختم
	}

	اگر(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		واپس rc;
	}ورنہ{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		واپس MOSQ_ERR_SUCCESS;
	}
}


ساکن صحیح sub_جوڑو_معمول(ساخت mosquitto *context, ثابت ساخت mosquitto_subscription *sub, ساخت mosquitto__subhier *subhier)
{
	ساخت mosquitto__subleaf *newleaf = تہی;
	ساخت mosquitto__subleaf **subs;
	صحیح rc;

	rc = sub_جوڑو_برگ(context, sub, &subhier->subs, &newleaf);
	اگر(rc > 0){
		واپس rc;
	}

	اگر(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = تہی;

		bool assigned = false;
		برائے(صحیح i=0; i<context->subs_استعداد; i++){
			اگر(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_گنتی++;
				assigned = true;
				توڑو;
			}
		}
		اگر(assigned == false){
			subs = mosquitto_realloc(context->subs, حجم(ساخت mosquitto__subleaf *)*(size_t)(context->subs_استعداد + 1));
			اگر(!subs){
				DL_DELETE(subhier->subs, newleaf);
				mosquitto_FREE(newleaf);
				واپس MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_استعداد++;
			context->subs_گنتی++;
			context->subs[context->subs_استعداد-1] = newleaf;
		}
#اگر_تعریف_شدہ WITH_SYS_درخت
		db.subscription_گنتی++;
#اگر_ختم
	}

	اگر(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		واپس rc;
	}ورنہ{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		واپس MOSQ_ERR_SUCCESS;
	}
}


ساکن صحیح sub_جوڑو_context(ساخت mosquitto *context, ثابت ساخت mosquitto_subscription *sub, ساخت mosquitto__subhier *subhier, حرف *ثابت *ثابت topics, ثابت حرف *sharename)
{
	ساخت mosquitto__subhier *branch;
	صحیح topic_اشاریہ = 0;
	size_t topiclen;

	/* Find leaf node */
	جب(topics && topics[topic_اشاریہ] != تہی){
		topiclen = لڑی_لمبائی(topics[topic_اشاریہ]);
		اگر(topiclen > UINT16_MAX){
			واپس MOSQ_ERR_INVAL;
		}
		HASH_ڈھونڈو(hh, subhier->children, topics[topic_اشاریہ], topiclen, branch);
		اگر(!branch){
			/* Not found */
			branch = sub_جوڑو_hier_اندراج(subhier, &subhier->children, topics[topic_اشاریہ], (uint16_t)topiclen);
			اگر(!branch){
				واپس MOSQ_ERR_NOMEM;
			}
		}
		subhier = branch;
		topic_اشاریہ++;
	}

	/* Add add our context */
	اگر(context && context->id){
		اگر(sharename){
			واپس sub_جوڑو_shared(context, sub, subhier, sharename);
		}ورنہ{
			واپس sub_جوڑو_معمول(context, sub, subhier);
		}
	}ورنہ{
		واپس MOSQ_ERR_SUCCESS;
	}
}


ساکن صحیح sub_remove_معمول(ساخت mosquitto *context, ساخت mosquitto__subhier *subhier, uint8_t *reason)
{
	ساخت mosquitto__subleaf *برگ;

	برگ = subhier->subs;
	جب(برگ){
		اگر(برگ->context==context){
#اگر_تعریف_شدہ WITH_SYS_درخت
			db.subscription_گنتی--;
#اگر_ختم
			DL_DELETE(subhier->subs, برگ);

			/* Remove the reference to the sub that the client is keeping.
			 * It would be nice to be able to use the reference directly,
			 * but that would involve keeping a copy of the topic string in
			 * each subleaf. Might be worth considering though. */
			برائے(صحیح i=0; i<context->subs_استعداد; i++){
				اگر(context->subs[i] && context->subs[i]->hier == subhier){
					context->subs_گنتی--;
					mosquitto_free(context->subs[i]);
					context->subs[i] = تہی;
					توڑو;
				}
			}
			*reason = 0;
			واپس MOSQ_ERR_SUCCESS;
		}
		برگ = برگ->اگلا;
	}
	واپس MOSQ_ERR_NO_SUBSCRIBERS;
}


ساکن صحیح sub__remove_shared(ساخت mosquitto *context, ساخت mosquitto__subhier *subhier, uint8_t *reason, ثابت حرف *sharename)
{
	ساخت mosquitto__subshared *shared;

	HASH_ڈھونڈو(hh, subhier->shared, sharename, لڑی_لمبائی(sharename), shared);
	اگر(shared){
		ساخت mosquitto__subleaf *برگ = shared->subs;
		جب(برگ){
			اگر(برگ->context==context){
#اگر_تعریف_شدہ WITH_SYS_درخت
				db.shared_subscription_گنتی--;
#اگر_ختم
				DL_DELETE(shared->subs, برگ);

				/* Remove the reference to the sub that the client is keeping.
				* It would be nice to be able to use the reference directly,
				* but that would involve keeping a copy of the topic string in
				* each subleaf. Might be worth considering though. */
				برائے(صحیح i=0; i<context->subs_استعداد; i++){
					اگر(context->subs[i]
							&& context->subs[i]->hier == subhier
							&& context->subs[i]->shared == shared){

						mosquitto_free(context->subs[i]);
						context->subs[i] = تہی;
						context->subs_گنتی--;
						توڑو;
					}
				}

				اگر(shared->subs == تہی){
					HASH_DELETE(hh, subhier->shared, shared);
					mosquitto_FREE(shared);
				}

				*reason = 0;
				واپس MOSQ_ERR_SUCCESS;
			}
			برگ = برگ->اگلا;
		}
		واپس MOSQ_ERR_NO_SUBSCRIBERS;
	}ورنہ{
		واپس MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


ساکن صحیح sub__remove_recurse(ساخت mosquitto *context, ساخت mosquitto__subhier *subhier, حرف **topics, uint8_t *reason, ثابت حرف *sharename)
{
	ساخت mosquitto__subhier *branch;

	اگر(topics == تہی || topics[0] == تہی){
		اگر(sharename){
			واپس sub__remove_shared(context, subhier, reason, sharename);
		}ورنہ{
			واپس sub_remove_معمول(context, subhier, reason);
		}
	}

	HASH_ڈھونڈو(hh, subhier->children, topics[0], لڑی_لمبائی(topics[0]), branch);
	اگر(branch){
		sub__remove_recurse(context, branch, &(topics[1]), reason, sharename);
		اگر(!branch->children && !branch->subs && !branch->shared){
			HASH_DELETE(hh, subhier->children, branch);
			mosquitto_FREE(branch);
		}
	}
	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح sub_تلاش(ساخت mosquitto__subhier *subhier, حرف **بانٹو_topics, ثابت حرف *ماخذ_id, ثابت حرف *topic, uint8_t qos, صحیح retain, ساخت mosquitto_بنیاد_پیغام *stored)
{
	/* FIXME - need to take into account source_id if the client is a bridge */
	ساخت mosquitto__subhier *branch;
	صحیح rc;
	bool have_subscribers = false;

	اگر(بانٹو_topics && بانٹو_topics[0]){
		/* Check for literal match */
		HASH_ڈھونڈو(hh, subhier->children, بانٹو_topics[0], لڑی_لمبائی(بانٹو_topics[0]), branch);

		اگر(branch){
			rc = sub_تلاش(branch, &(بانٹو_topics[1]), ماخذ_id, topic, qos, retain, stored);
			اگر(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}ورنہ اگر(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				واپس rc;
			}
			اگر(بانٹو_topics[1] == تہی){ /* End of list */
				rc = subs__process(branch, ماخذ_id, topic, qos, retain, stored);
				اگر(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}ورنہ اگر(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					واپس rc;
				}
			}
		}

		/* Check for + match */
		HASH_ڈھونڈو_BYHASHVALUE(hh, subhier->children, "+", 1, hashv_plus, branch);

		اگر(branch){
			rc = sub_تلاش(branch, &(بانٹو_topics[1]), ماخذ_id, topic, qos, retain, stored);
			اگر(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}ورنہ اگر(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				واپس rc;
			}
			اگر(بانٹو_topics[1] == تہی){ /* End of list */
				rc = subs__process(branch, ماخذ_id, topic, qos, retain, stored);
				اگر(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}ورنہ اگر(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					واپس rc;
				}
			}
		}
	}

	/* Check for # match */
	HASH_ڈھونڈو_BYHASHVALUE(hh, subhier->children, "#", 1, hashv_hash, branch);
	اگر(branch && !branch->children){
		/* The topic matches due to a # wildcard - process the
		 * subscriptions but *don't* return. Although this branch has ended
		 * there may still be other subscriptions to deal with.
		 */
		rc = subs__process(branch, ماخذ_id, topic, qos, retain, stored);
		اگر(rc == MOSQ_ERR_SUCCESS){
			have_subscribers = true;
		}ورنہ اگر(rc != MOSQ_ERR_NO_SUBSCRIBERS){
			واپس rc;
		}
	}

	اگر(have_subscribers){
		واپس MOSQ_ERR_SUCCESS;
	}ورنہ{
		واپس MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


ساکن ساخت mosquitto__subhier *sub_جوڑو_hier_اندراج(ساخت mosquitto__subhier *والد, ساخت mosquitto__subhier **sibling, ثابت حرف *topic, uint16_t len)
{
	ساخت mosquitto__subhier *بچہ;

	أكد(sibling);

	بچہ = mosquitto_calloc(1, حجم(ساخت mosquitto__subhier) + len + 1);
	اگر(!بچہ){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
		واپس تہی;
	}
	بچہ->والد = والد;
	بچہ->topic_len = len;
	اگر(len > 0){
		strncpy(بچہ->topic, topic, (size_t)(len+1));
	}

	HASH_جوڑو(hh, *sibling, topic, بچہ->topic_len, بچہ);

	واپس بچہ;
}


صحیح sub_جوڑو(ساخت mosquitto *context, ثابت ساخت mosquitto_subscription *sub)
{
	صحیح rc = 0;
	ساخت mosquitto__subhier *subhier;
	ثابت حرف *sharename = تہی;
	حرف *local_sub;
	حرف **topics;
	size_t topiclen;

	أكد(sub);
	أكد(sub->topic_filter);

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &topics, &sharename);
	اگر(rc){
		واپس rc;
	}

	topiclen = لڑی_لمبائی(topics[0]);
	اگر(topiclen > UINT16_MAX){
		mosquitto_FREE(local_sub);
		mosquitto_FREE(topics);
		واپس MOSQ_ERR_INVAL;
	}

	اگر(sharename){
		HASH_ڈھونڈو(hh, db.shared_subs, topics[0], topiclen, subhier);
		اگر(!subhier){
			subhier = sub_جوڑو_hier_اندراج(تہی, &db.shared_subs, topics[0], (uint16_t)topiclen);
			اگر(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
				واپس MOSQ_ERR_NOMEM;
			}
		}
	}ورنہ{
		HASH_ڈھونڈو(hh, db.معمول_subs, topics[0], topiclen, subhier);
		اگر(!subhier){
			subhier = sub_جوڑو_hier_اندراج(تہی, &db.معمول_subs, topics[0], (uint16_t)topiclen);
			اگر(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
				واپس MOSQ_ERR_NOMEM;
			}
		}
	}
	rc = sub_جوڑو_context(context, sub, subhier, topics, sharename);

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	واپس rc;
}


صحیح sub__remove(ساخت mosquitto *context, ثابت حرف *sub, uint8_t *reason)
{
	صحیح rc = 0;
	ساخت mosquitto__subhier *subhier;
	ثابت حرف *sharename = تہی;
	حرف *local_sub = تہی;
	حرف **topics = تہی;

	أكد(sub);

	rc = sub__topic_tokenise(sub, &local_sub, &topics, &sharename);
	اگر(rc){
		واپس rc;
	}

	اگر(sharename){
		HASH_ڈھونڈو(hh, db.shared_subs, topics[0], لڑی_لمبائی(topics[0]), subhier);
	}ورنہ{
		HASH_ڈھونڈو(hh, db.معمول_subs, topics[0], لڑی_لمبائی(topics[0]), subhier);
	}
	اگر(subhier){
		*reason = MQTT_RC_NO_SUBSCRIPTION_EXISTED;
		rc = sub__remove_recurse(context, subhier, topics, reason, sharename);
	}

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	واپس rc;
}


صحیح sub_messages_قطار(ثابت حرف *ماخذ_id, uint8_t qos, ساخت mosquitto_بنیاد_پیغام **stored)
{
	صحیح rc = MOSQ_ERR_SUCCESS, rc2;
	صحیح rc_معمول = MOSQ_ERR_NO_SUBSCRIBERS, rc_shared = MOSQ_ERR_NO_SUBSCRIBERS;
	ساخت mosquitto__subhier *subhier;
	حرف **بانٹو_topics = تہی;
	حرف *local_topic = تہی;
	غیرعلامتی hashv;
	size_t topiclen;
	bool retain = (*stored)->ڈیٹا.retain;
	ثابت حرف *topic = (*stored)->ڈیٹا.topic;

	أكد(topic);

	اگر(sub__topic_tokenise(topic, &local_topic, &بانٹو_topics, تہی)){
		واپس 1;
	}

	/* Protect this message until we have sent it to all
	clients - this is required because websockets client calls
	db__message_write(), which could remove the message if ref_count==0.
	*/
	db_پیغام_store_ref_inc(*stored);

	topiclen = لڑی_لمبائی(بانٹو_topics[0]);
	HASH_قدر(بانٹو_topics[0], topiclen, hashv);
	HASH_ڈھونڈو_BYHASHVALUE(hh, db.معمول_subs, بانٹو_topics[0], topiclen, hashv, subhier);
	اگر(subhier){
		rc_معمول = sub_تلاش(subhier, بانٹو_topics, ماخذ_id, topic, qos, retain, *stored);
		اگر(rc_معمول > 0){
			rc = rc_معمول;
			جاؤ انتہا;
		}
	}

	HASH_ڈھونڈو_BYHASHVALUE(hh, db.shared_subs, بانٹو_topics[0], topiclen, hashv, subhier);
	اگر(subhier){
		rc_shared = sub_تلاش(subhier, بانٹو_topics, ماخذ_id, topic, qos, retain, *stored);
		اگر(rc_shared > 0){
			rc = rc_shared;
			جاؤ انتہا;
		}
	}

	اگر(rc_معمول == MOSQ_ERR_NO_SUBSCRIBERS && rc_shared == MOSQ_ERR_NO_SUBSCRIBERS){
		rc = MOSQ_ERR_NO_SUBSCRIBERS;
	}

	اگر(retain){
		rc2 = retain__store(topic, *stored, بانٹو_topics, true);
		اگر(rc2){
			rc = rc2;
		}
	}

انتہا:
	mosquitto_FREE(بانٹو_topics);
	mosquitto_FREE(local_topic);
	/* Remove our reference and free if needed. */
	db_پیغام_store_ref_dec(stored);

	واپس rc;
}


/* Remove a subhier element, and return its parent if that needs freeing as well. */
ساکن ساخت mosquitto__subhier *tmp_remove_subs(ساخت mosquitto__subhier *sub)
{
	ساخت mosquitto__subhier *والد;

	اگر(!sub || !sub->والد){
		واپس تہی;
	}

	اگر(sub->children || sub->subs){
		واپس تہی;
	}

	والد = sub->والد;
	HASH_DELETE(hh, والد->children, sub);
	mosquitto_FREE(sub);

	اگر(والد->subs == تہی
			&& والد->children == تہی
			&& والد->shared == تہی
			&& والد->والد){

		واپس والد;
	}ورنہ{
		واپس تہی;
	}
}


/* Remove all subscriptions for a client.
 */
صحیح sub__clean_session(ساخت mosquitto *context)
{
	برائے(صحیح i=0; i<context->subs_استعداد; i++){
		اگر(context->subs[i] == تہی || context->subs[i]->hier == تہی){
			جاری;
		}

		ساخت mosquitto__subhier *hier = context->subs[i]->hier;
		ساخت mosquitto__subleaf *برگ;

		plugin_persist__handle_subscription_delete(context, context->subs[i]->topic_filter);
		اگر(context->subs[i]->shared){
			برگ = context->subs[i]->shared->subs;
			جب(برگ){
				اگر(برگ->context==context){
#اگر_تعریف_شدہ WITH_SYS_درخت
					db.shared_subscription_گنتی--;
#اگر_ختم
					sub_remove_shared_برگ(context->subs[i]->hier, context->subs[i]->shared, برگ);
					توڑو;
				}
				برگ = برگ->اگلا;
			}
		}ورنہ{
			برگ = hier->subs;
			جب(برگ){
				اگر(برگ->context==context){
#اگر_تعریف_شدہ WITH_SYS_درخت
					db.subscription_گنتی--;
#اگر_ختم
					DL_DELETE(hier->subs, برگ);
					توڑو;
				}
				برگ = برگ->اگلا;
			}
		}
		mosquitto_FREE(context->subs[i]);

		اگر(hier->subs == تہی
				&& hier->children == تہی
				&& hier->shared == تہی
				&& hier->والد){

			کرو{
				hier = tmp_remove_subs(hier);
			}جب(hier);
		}
	}
	mosquitto_FREE(context->subs);
	context->subs_استعداد = 0;
	context->subs_گنتی = 0;

	واپس MOSQ_ERR_SUCCESS;
}


خالی sub_درخت_print(ساخت mosquitto__subhier *جڑ, صحیح level)
{
	صحیح i;
	ساخت mosquitto__subhier *branch, *branch_tmp;
	ساخت mosquitto__subleaf *برگ;

	HASH_ITER(hh, جڑ, branch, branch_tmp){
		اگر(level > -1){
			برائے(i=0; i<(level+2)*2; i++){
				لکھو(" ");
			}
			لکھو("%s", branch->topic);
			برگ = branch->subs;
			جب(برگ){
				اگر(برگ->context){
					لکھو(" (%s, %d)", برگ->context->id, MQTT_SUB_OPT_GET_QOS(برگ->subscription_options));
				}ورنہ{
					لکھو(" (%s, %d)", "", MQTT_SUB_OPT_GET_QOS(برگ->subscription_options));
				}
				برگ = برگ->اگلا;
			}
			لکھو("\n");
		}

		sub_درخت_print(branch->children, level+1);
	}
}


صحیح sub__init(خالی)
{
	HASH_قدر("+", 1, hashv_plus);
	HASH_قدر("#", 1, hashv_hash);

	اگر(sub_جوڑو_hier_اندراج(تہی, &db.shared_subs, "", 0) == تہی
			|| sub_جوڑو_hier_اندراج(تہی, &db.معمول_subs, "", 0) == تہی
			|| sub_جوڑو_hier_اندراج(تہی, &db.معمول_subs, "$SYS", (uint16_t)لڑی_لمبائی("$SYS")) == تہی
			){

		واپس MOSQ_ERR_NOMEM;
	}ورنہ{
		واپس MOSQ_ERR_SUCCESS;
	}
}
