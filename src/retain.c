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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "utlist.h"

ਸਥਿਰ time_t ਅਗਲਾ_expire_check = 0;

ਸਥਿਰ ਬਣਤਰ mosquitto__retainhier *retain_ਜੋੜੋ_hier_ਇੰਦਰਾਜ(ਬਣਤਰ mosquitto__retainhier *ਮਾਪਾ, ਬਣਤਰ mosquitto__retainhier **sibling, ਅਚਲ ਅੱਖਰ *topic, uint16_t len)
{
	ਬਣਤਰ mosquitto__retainhier *ਬੱਚਾ;

	ਪ੍ਰਤਿਜ੍ਞਾ(sibling);

	ਬੱਚਾ = mosquitto_calloc(1, ਆਕਾਰ(ਬਣਤਰ mosquitto__retainhier) + len + 1);
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


ਪੂਰਨ retain__init(ਖਾਲੀ)
{
	ਬਣਤਰ mosquitto__retainhier *retainhier;

	retainhier = retain_ਜੋੜੋ_hier_ਇੰਦਰਾਜ(ਨਿਰਾ, &db.retains, "", 0);
	ਜੇ(!retainhier){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	retainhier = retain_ਜੋੜੋ_hier_ਇੰਦਰਾਜ(ਨਿਰਾ, &db.retains, "$SYS", (uint16_t)ਲੜੀ_ਲੰਬਾਈ("$SYS"));
	ਜੇ(!retainhier){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ਪੂਰਨ mosquitto_persist_retain_ਸੁਨੇਹਾ_ਸੈੱਟ(ਅਚਲ ਅੱਖਰ *topic, uint64_t ਅਧਾਰ_ਸੁਨੇਹਾ_id)
{
	ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ;
	ਪੂਰਨ rc = MOSQ_ERR_UNKNOWN;
	ਅੱਖਰ **ਵੰਡੋ_topics = ਨਿਰਾ;
	ਅੱਖਰ *local_topic = ਨਿਰਾ;

	ਜੇ(topic == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	HASH_ਲੱਭੋ(hh, db.ਸੁਨੇਹਾ_store, &ਅਧਾਰ_ਸੁਨੇਹਾ_id, ਆਕਾਰ(ਅਧਾਰ_ਸੁਨੇਹਾ_id), ਅਧਾਰ_ਸੁਨੇਹਾ);
	ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ){
		ਜੇ(sub__topic_tokenise(topic, &local_topic, &ਵੰਡੋ_topics, ਨਿਰਾ)){
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}

		rc = retain__store(topic, ਅਧਾਰ_ਸੁਨੇਹਾ, ਵੰਡੋ_topics, false);
		mosquitto_free(ਵੰਡੋ_topics);
		mosquitto_free(local_topic);
	}

	ਵਾਪਸ rc;
}


BROKER_EXPORT ਪੂਰਨ mosquitto_persist_retain_ਸੁਨੇਹਾ_delete(ਅਚਲ ਅੱਖਰ *topic)
{
	ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ ਅਧਾਰ_ਸੁਨੇਹਾ;
	ਪੂਰਨ rc = MOSQ_ERR_UNKNOWN;
	ਅੱਖਰ **ਵੰਡੋ_topics = ਨਿਰਾ;
	ਅੱਖਰ *local_topic = ਨਿਰਾ;

	ਜੇ(topic == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	memset(&ਅਧਾਰ_ਸੁਨੇਹਾ, 0, ਆਕਾਰ(ਅਧਾਰ_ਸੁਨੇਹਾ));
	ਅਧਾਰ_ਸੁਨੇਹਾ.ref_ਗਿਣਤੀ = 10; /* Ensure this isn't freed */

	ਜੇ(sub__topic_tokenise(topic, &local_topic, &ਵੰਡੋ_topics, ਨਿਰਾ)){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	/* With stored->payloadlen == 0, this means the message will be removed */
	rc = retain__store(topic, &ਅਧਾਰ_ਸੁਨੇਹਾ, ਵੰਡੋ_topics, false);
	mosquitto_FREE(ਵੰਡੋ_topics);
	mosquitto_FREE(local_topic);

	ਵਾਪਸ rc;
}


ਖਾਲੀ retain_clean_ਖਾਲੀ_ਥਾਵਾਂ_hierarchy(ਬਣਤਰ mosquitto__retainhier *retainhier)
{
	ਜਦੋਂ(retainhier){
		ਜੇ(retainhier->children || retainhier->retained || retainhier->ਮਾਪਾ == ਨਿਰਾ){
			/* Entry is being used */
			ਵਾਪਸ;
		}ਨਹੀਂਤਾਂ{
			HASH_DELETE(hh, retainhier->ਮਾਪਾ->children, retainhier);

			ਬਣਤਰ mosquitto__retainhier *ਮਾਪਾ = retainhier->ਮਾਪਾ;
			mosquitto_FREE(retainhier);
			retainhier = ਮਾਪਾ;
		}
	}
}


ਪੂਰਨ retain__store(ਅਚਲ ਅੱਖਰ *topic, ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ, ਅੱਖਰ **ਵੰਡੋ_topics, bool persist)
{
	ਬਣਤਰ mosquitto__retainhier *retainhier;
	ਬਣਤਰ mosquitto__retainhier *branch;
	size_t slen;

	ਪ੍ਰਤਿਜ੍ਞਾ(ਅਧਾਰ_ਸੁਨੇਹਾ);
	ਪ੍ਰਤਿਜ੍ਞਾ(ਵੰਡੋ_topics);

	HASH_ਲੱਭੋ(hh, db.retains, ਵੰਡੋ_topics[0], ਲੜੀ_ਲੰਬਾਈ(ਵੰਡੋ_topics[0]), retainhier);
	ਜੇ(retainhier == ਨਿਰਾ){
		retainhier = retain_ਜੋੜੋ_hier_ਇੰਦਰਾਜ(ਨਿਰਾ, &db.retains, ਵੰਡੋ_topics[0], (uint16_t)ਲੜੀ_ਲੰਬਾਈ(ਵੰਡੋ_topics[0]));
		ਜੇ(!retainhier){
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}
	}

	ਲਈ(ਪੂਰਨ i=0; ਵੰਡੋ_topics[i] != ਨਿਰਾ; i++){
		slen = ਲੜੀ_ਲੰਬਾਈ(ਵੰਡੋ_topics[i]);
		HASH_ਲੱਭੋ(hh, retainhier->children, ਵੰਡੋ_topics[i], slen, branch);
		ਜੇ(branch == ਨਿਰਾ){
			branch = retain_ਜੋੜੋ_hier_ਇੰਦਰਾਜ(retainhier, &retainhier->children, ਵੰਡੋ_topics[i], (uint16_t)slen);
			ਜੇ(branch == ਨਿਰਾ){
				ਵਾਪਸ MOSQ_ERR_NOMEM;
			}
		}
		retainhier = branch;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_PERSISTENCE
	ਜੇ(ਲੜੀ_ਤੁਲਨਾ(topic, "$SYS", 4)){
		/* Retained messages count as a persistence change, but only if
		 * they aren't for $SYS. */
		db.persistence_changes++;
	}
#ਨਹੀਂਤਾਂ
	ਅਣਵਰਤਿਆ(topic);
#ਜੇ_ਅੰਤ

	ਜੇ(retainhier->retained){
		ਜੇ(retainhier->retained == ਅਧਾਰ_ਸੁਨੇਹਾ){
			/* This may occur if multiple persistence providers are used */
			ਵਾਪਸ MOSQ_ERR_SUCCESS;
		}

		ਜੇ(persist && retainhier->retained->ਅੰਕੜੇ.topic[0] != '$' && ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen == 0){
			/* Only delete if another retained message isn't replacing this one */
			plugin_persist_handle_retain_ਸੁਨੇਹਾ_delete(retainhier->retained);
		}
		db_ਸੁਨੇਹਾ_store_ref_dec(&retainhier->retained);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_SYS_ਰੁੱਖ
		db.retained_ਗਿਣਤੀ--;
#ਜੇ_ਅੰਤ
		ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen == 0){
			retainhier->retained = ਨਿਰਾ;
			retain_clean_ਖਾਲੀ_ਥਾਵਾਂ_hierarchy(retainhier);
		}
	}
	ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen){
		retainhier->retained = ਅਧਾਰ_ਸੁਨੇਹਾ;
		db_ਸੁਨੇਹਾ_store_ref_inc(retainhier->retained);
		ਜੇ(persist && retainhier->retained->ਅੰਕੜੇ.topic[0] != '$'){
			plugin_persist_handle_ਅਧਾਰ_ਸੁਨੇਹਾ_ਜੋੜੋ(retainhier->retained);
			plugin_persist_handle_retain_ਸੁਨੇਹਾ_ਸੈੱਟ(retainhier->retained);
		}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_SYS_ਰੁੱਖ
		db.retained_ਗਿਣਤੀ++;
#ਜੇ_ਅੰਤ
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ bool retain_delete_expired_ਸੁਨੇਹਾ(ਬਣਤਰ mosquitto__retainhier *branch)
{
	ਜੇ(branch->retained && branch->retained->ਅੰਕੜੇ.expiry_ਸਮਾਂ > 0 && db.now_real_s >= branch->retained->ਅੰਕੜੇ.expiry_ਸਮਾਂ){
		plugin_persist_handle_retain_ਸੁਨੇਹਾ_delete(branch->retained);
		db_ਸੁਨੇਹਾ_store_ref_dec(&branch->retained);
		branch->retained = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_SYS_ਰੁੱਖ
		db.retained_ਗਿਣਤੀ--;
#ਜੇ_ਅੰਤ
		ਵਾਪਸ true;
	}
	ਵਾਪਸ false;
}


ਸਥਿਰ ਪੂਰਨ retain__process(ਬਣਤਰ mosquitto__retainhier *branch, ਬਣਤਰ mosquitto *context, ਅਚਲ ਬਣਤਰ mosquitto_subscription *sub)
{
	ਪੂਰਨ rc = 0;
	uint8_t qos, sub_qos;
	uint16_t mid;
	ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *retained;

	ਜੇ(retain_delete_expired_ਸੁਨੇਹਾ(branch)){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

	retained = branch->retained;

	rc = mosquitto_acl_check(context, retained->ਅੰਕੜੇ.topic, retained->ਅੰਕੜੇ.payloadlen, retained->ਅੰਕੜੇ.payload,
			retained->ਅੰਕੜੇ.qos, retained->ਅੰਕੜੇ.retain, retained->ਅੰਕੜੇ.properties, MOSQ_ACL_READ);
	ਜੇ(rc == MOSQ_ERR_ACL_DENIED){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}ਨਹੀਂਤਾਂ ਜੇ(rc != MOSQ_ERR_SUCCESS){
		ਵਾਪਸ rc;
	}

	/* Check for original source access */
	ਜੇ(db.config->check_retain_ਸਰੋਤ && retained->origin != mosq_mo_broker && retained->ਅੰਕੜੇ.ਸਰੋਤ_id){
		ਬਣਤਰ mosquitto retain_ctxt;
		memset(&retain_ctxt, 0, ਆਕਾਰ(ਬਣਤਰ mosquitto));

		retain_ctxt.id = retained->ਅੰਕੜੇ.ਸਰੋਤ_id;
		retain_ctxt.username = retained->ਅੰਕੜੇ.ਸਰੋਤ_username;
		retain_ctxt.listener = retained->ਸਰੋਤ_listener;

		rc = mosquitto_acl_check(&retain_ctxt, retained->ਅੰਕੜੇ.topic, retained->ਅੰਕੜੇ.payloadlen, retained->ਅੰਕੜੇ.payload,
				retained->ਅੰਕੜੇ.qos, retained->ਅੰਕੜੇ.retain, retained->ਅੰਕੜੇ.properties, MOSQ_ACL_WRITE);
		ਜੇ(rc == MOSQ_ERR_ACL_DENIED){
			ਵਾਪਸ MOSQ_ERR_SUCCESS;
		}ਨਹੀਂਤਾਂ ਜੇ(rc != MOSQ_ERR_SUCCESS){
			ਵਾਪਸ rc;
		}
	}

	sub_qos = sub->options & 0x03;
	ਜੇ(db.config->upgrade_outgoing_qos){
		qos = sub_qos;
	}ਨਹੀਂਤਾਂ{
		qos = retained->ਅੰਕੜੇ.qos;
		ਜੇ(qos > sub_qos){
			qos = sub_qos;
		}
	}
	ਜੇ(qos > 0){
		mid = mosquitto__mid_generate(context);
	}ਨਹੀਂਤਾਂ{
		mid = 0;
	}
	ਵਾਪਸ db_ਸੁਨੇਹਾ_ਪਾਓ_outgoing(context, 0, mid, qos, true, retained, sub->identifier, false, true);
}


ਸਥਿਰ ਪੂਰਨ retain_ਖੋਜ(ਬਣਤਰ mosquitto__retainhier *retainhier, ਅੱਖਰ **ਵੰਡੋ_topics, ਬਣਤਰ mosquitto *context, ਅਚਲ ਬਣਤਰ mosquitto_subscription *sub, ਪੂਰਨ level)
{
	ਬਣਤਰ mosquitto__retainhier *branch, *branch_tmp;
	ਪੂਰਨ flag = 0;

	ਜੇ(!strcmp(ਵੰਡੋ_topics[0], "#") && ਵੰਡੋ_topics[1] == ਨਿਰਾ){
		HASH_ITER(hh, retainhier->children, branch, branch_tmp){
			/* Set flag to indicate that we should check for retained messages
			 * on "foo" when we are subscribing to e.g. "foo/#" and then exit
			 * this function and return to an earlier retain__search().
			 */
			flag = -1;
			ਜੇ(branch->retained){
				retain__process(branch, context, sub);
			}
			ਜੇ(branch->children){
				retain_ਖੋਜ(branch, ਵੰਡੋ_topics, context, sub, level+1);
			}
		}
	}ਨਹੀਂਤਾਂ{
		ਜੇ(!strcmp(ਵੰਡੋ_topics[0], "+")){
			HASH_ITER(hh, retainhier->children, branch, branch_tmp){
				ਜੇ(ਵੰਡੋ_topics[1] != ਨਿਰਾ){
					ਜੇ(retain_ਖੋਜ(branch, &(ਵੰਡੋ_topics[1]), context, sub, level+1) == -1
							|| (ਵੰਡੋ_topics[1] != ਨਿਰਾ && !strcmp(ਵੰਡੋ_topics[1], "#") && level>0)){

						ਜੇ(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}ਨਹੀਂਤਾਂ{
					ਜੇ(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}ਨਹੀਂਤਾਂ{
			HASH_ਲੱਭੋ(hh, retainhier->children, ਵੰਡੋ_topics[0], ਲੜੀ_ਲੰਬਾਈ(ਵੰਡੋ_topics[0]), branch);
			ਜੇ(branch){
				ਜੇ(ਵੰਡੋ_topics[1] != ਨਿਰਾ){
					ਜੇ(retain_ਖੋਜ(branch, &(ਵੰਡੋ_topics[1]), context, sub, level+1) == -1
							|| (ਵੰਡੋ_topics[1] != ਨਿਰਾ && !strcmp(ਵੰਡੋ_topics[1], "#") && level>0)){

						ਜੇ(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}ਨਹੀਂਤਾਂ{
					ਜੇ(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}
	}
	ਵਾਪਸ flag;
}


ਪੂਰਨ retain_ਕਤਾਰ(ਬਣਤਰ mosquitto *context, ਅਚਲ ਬਣਤਰ mosquitto_subscription *sub)
{
	ਬਣਤਰ mosquitto__retainhier *retainhier;
	ਅੱਖਰ *local_sub;
	ਅੱਖਰ **ਵੰਡੋ_topics;
	ਪੂਰਨ rc;

	ਪ੍ਰਤਿਜ੍ਞਾ(context);
	ਪ੍ਰਤਿਜ੍ਞਾ(sub);

	ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(sub->topic_filter, "$share/", ਲੜੀ_ਲੰਬਾਈ("$share/"))){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &ਵੰਡੋ_topics, ਨਿਰਾ);
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}

	HASH_ਲੱਭੋ(hh, db.retains, ਵੰਡੋ_topics[0], ਲੜੀ_ਲੰਬਾਈ(ਵੰਡੋ_topics[0]), retainhier);

	ਜੇ(retainhier){
		retain_ਖੋਜ(retainhier, ਵੰਡੋ_topics, context, sub, 0);
	}
	mosquitto_FREE(local_sub);
	mosquitto_FREE(ਵੰਡੋ_topics);

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਖਾਲੀ retain__expire(ਬਣਤਰ mosquitto__retainhier **retainhier)
{
	ਬਣਤਰ mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		retain__expire(&peer->children);

		ਬਣਤਰ mosquitto__retainhier *ਮਾਪਾ = peer->ਮਾਪਾ;
		ਜੇ(retain_delete_expired_ਸੁਨੇਹਾ(peer)){
			retain_clean_ਖਾਲੀ_ਥਾਵਾਂ_hierarchy(ਮਾਪਾ);
		}
	}
}


ਖਾਲੀ retain__clean(ਬਣਤਰ mosquitto__retainhier **retainhier)
{
	ਬਣਤਰ mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		ਜੇ(peer->retained){
			db_ਸੁਨੇਹਾ_store_ref_dec(&peer->retained);
		}
		retain__clean(&peer->children);

		HASH_DELETE(hh, *retainhier, peer);
		mosquitto_FREE(peer);
	}
}


ਖਾਲੀ retain__expiry_check(ਖਾਲੀ)
{
	ਜੇ(db.config->retain_expiry_interval > 0 && db.now_s > ਅਗਲਾ_expire_check){
		retain__expire(&db.retains);
		ਅਗਲਾ_expire_check = db.now_s + db.config->retain_expiry_interval;
	}
}
