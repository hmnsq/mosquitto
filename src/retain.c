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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"

#ಅನ್ತರ್ಭಾವ "utlist.h"

ಸ್ಥಿರ time_t next_expire_check = 0;

ಸ್ಥಿರ ಸಂರಚನಾ mosquitto__retainhier *retain__add_hier_entry(ಸಂರಚನಾ mosquitto__retainhier *parent, ಸಂರಚನಾ mosquitto__retainhier **sibling, ಧ್ರುವ ವರ್ಣ *topic, uint16_t len)
{
	ಸಂರಚನಾ mosquitto__retainhier *child;

	ಪ್ರತಿಜ್ಞಾ(sibling);

	child = mosquitto_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__retainhier) + len + 1);
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


ಪೂರ್ಣಾಙ್ಕ retain__init(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ mosquitto__retainhier *retainhier;

	retainhier = retain__add_hier_entry(ರಿಕ್ತ, &db.retains, "", 0);
	ಯದಿ(!retainhier){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}

	retainhier = retain__add_hier_entry(ರಿಕ್ತ, &db.retains, "$SYS", (uint16_t)strlen("$SYS"));
	ಯದಿ(!retainhier){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_persist_retain_ಸಂದೇಶ_set(ಧ್ರುವ ವರ್ಣ *topic, uint64_t base_ಸಂದೇಶ_id)
{
	ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ;
	ಪೂರ್ಣಾಙ್ಕ rc = MOSQ_ERR_UNKNOWN;
	ವರ್ಣ **split_topics = ರಿಕ್ತ;
	ವರ್ಣ *local_topic = ರಿಕ್ತ;

	ಯದಿ(topic == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh, db.ಸಂದೇಶ_store, &base_ಸಂದೇಶ_id, ಪರಿಮಾಣ(base_ಸಂದೇಶ_id), base_ಸಂದೇಶ);
	ಯದಿ(base_ಸಂದೇಶ){
		ಯದಿ(sub__topic_tokenise(topic, &local_topic, &split_topics, ರಿಕ್ತ)){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}

		rc = retain__store(topic, base_ಸಂದೇಶ, split_topics, false);
		mosquitto_free(split_topics);
		mosquitto_free(local_topic);
	}

	ಪ್ರತ್ಯಾಗಮ rc;
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_persist_retain_ಸಂದೇಶ_delete(ಧ್ರುವ ವರ್ಣ *topic)
{
	ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ base_ಸಂದೇಶ;
	ಪೂರ್ಣಾಙ್ಕ rc = MOSQ_ERR_UNKNOWN;
	ವರ್ಣ **split_topics = ರಿಕ್ತ;
	ವರ್ಣ *local_topic = ರಿಕ್ತ;

	ಯದಿ(topic == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	memset(&base_ಸಂದೇಶ, 0, ಪರಿಮಾಣ(base_ಸಂದೇಶ));
	base_ಸಂದೇಶ.ref_ಎಣಿಕೆ = 10; /* Ensure this isn't freed */

	ಯದಿ(sub__topic_tokenise(topic, &local_topic, &split_topics, ರಿಕ್ತ)){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}

	/* With stored->payloadlen == 0, this means the message will be removed */
	rc = retain__store(topic, &base_ಸಂದೇಶ, split_topics, false);
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);

	ಪ್ರತ್ಯಾಗಮ rc;
}


ಶೂನ್ಯ retain__clean_empty_hierarchy(ಸಂರಚನಾ mosquitto__retainhier *retainhier)
{
	ಯಾವತ್(retainhier){
		ಯದಿ(retainhier->children || retainhier->retained || retainhier->parent == ರಿಕ್ತ){
			/* Entry is being used */
			ಪ್ರತ್ಯಾಗಮ;
		}ಅನ್ಯಥಾ{
			HASH_DELETE(hh, retainhier->parent->children, retainhier);

			ಸಂರಚನಾ mosquitto__retainhier *parent = retainhier->parent;
			mosquitto_FREE(retainhier);
			retainhier = parent;
		}
	}
}


ಪೂರ್ಣಾಙ್ಕ retain__store(ಧ್ರುವ ವರ್ಣ *topic, ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ, ವರ್ಣ **split_topics, bool persist)
{
	ಸಂರಚನಾ mosquitto__retainhier *retainhier;
	ಸಂರಚನಾ mosquitto__retainhier *branch;
	size_t slen;

	ಪ್ರತಿಜ್ಞಾ(base_ಸಂದೇಶ);
	ಪ್ರತಿಜ್ಞಾ(split_topics);

	HASH_FIND(hh, db.retains, split_topics[0], strlen(split_topics[0]), retainhier);
	ಯದಿ(retainhier == ರಿಕ್ತ){
		retainhier = retain__add_hier_entry(ರಿಕ್ತ, &db.retains, split_topics[0], (uint16_t)strlen(split_topics[0]));
		ಯದಿ(!retainhier){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}
	}

	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; split_topics[i] != ರಿಕ್ತ; i++){
		slen = strlen(split_topics[i]);
		HASH_FIND(hh, retainhier->children, split_topics[i], slen, branch);
		ಯದಿ(branch == ರಿಕ್ತ){
			branch = retain__add_hier_entry(retainhier, &retainhier->children, split_topics[i], (uint16_t)slen);
			ಯದಿ(branch == ರಿಕ್ತ){
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
			}
		}
		retainhier = branch;
	}

#ifdef WITH_PERSISTENCE
	ಯದಿ(strncmp(topic, "$SYS", 4)){
		/* Retained messages count as a persistence change, but only if
		 * they aren't for $SYS. */
		db.persistence_changes++;
	}
#ಅನ್ಯಥಾ
	UNUSED(topic);
#endif

	ಯದಿ(retainhier->retained){
		ಯದಿ(retainhier->retained == base_ಸಂದೇಶ){
			/* This may occur if multiple persistence providers are used */
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
		}

		ಯದಿ(persist && retainhier->retained->data.topic[0] != '$' && base_ಸಂದೇಶ->data.payloadlen == 0){
			/* Only delete if another retained message isn't replacing this one */
			plugin_persist_handle_retain_ಸಂದೇಶ_delete(retainhier->retained);
		}
		db_ಸಂದೇಶ_store_ref_dec(&retainhier->retained);
#ifdef WITH_SYS_ವೃಕ್ಷ
		db.retained_ಎಣಿಕೆ--;
#endif
		ಯದಿ(base_ಸಂದೇಶ->data.payloadlen == 0){
			retainhier->retained = ರಿಕ್ತ;
			retain__clean_empty_hierarchy(retainhier);
		}
	}
	ಯದಿ(base_ಸಂದೇಶ->data.payloadlen){
		retainhier->retained = base_ಸಂದೇಶ;
		db_ಸಂದೇಶ_store_ref_inc(retainhier->retained);
		ಯದಿ(persist && retainhier->retained->data.topic[0] != '$'){
			plugin_persist_handle_base_ಸಂದೇಶ_add(retainhier->retained);
			plugin_persist_handle_retain_ಸಂದೇಶ_set(retainhier->retained);
		}
#ifdef WITH_SYS_ವೃಕ್ಷ
		db.retained_ಎಣಿಕೆ++;
#endif
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ bool retain_delete_expired_ಸಂದೇಶ(ಸಂರಚನಾ mosquitto__retainhier *branch)
{
	ಯದಿ(branch->retained && branch->retained->data.expiry_time > 0 && db.now_real_s >= branch->retained->data.expiry_time){
		plugin_persist_handle_retain_ಸಂದೇಶ_delete(branch->retained);
		db_ಸಂದೇಶ_store_ref_dec(&branch->retained);
		branch->retained = ರಿಕ್ತ;
#ifdef WITH_SYS_ವೃಕ್ಷ
		db.retained_ಎಣಿಕೆ--;
#endif
		ಪ್ರತ್ಯಾಗಮ true;
	}
	ಪ್ರತ್ಯಾಗಮ false;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ retain__process(ಸಂರಚನಾ mosquitto__retainhier *branch, ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ಸಂರಚನಾ mosquitto_subscription *sub)
{
	ಪೂರ್ಣಾಙ್ಕ rc = 0;
	uint8_t qos, sub_qos;
	uint16_t mid;
	ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *retained;

	ಯದಿ(retain_delete_expired_ಸಂದೇಶ(branch)){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}

	retained = branch->retained;

	rc = mosquitto_acl_check(context, retained->data.topic, retained->data.payloadlen, retained->data.payload,
			retained->data.qos, retained->data.retain, retained->data.properties, MOSQ_ACL_READ);
	ಯದಿ(rc == MOSQ_ERR_ACL_DENIED){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}ಅನ್ಯಥಾ ಯದಿ(rc != MOSQ_ERR_SUCCESS){
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	/* Check for original source access */
	ಯದಿ(db.config->check_retain_source && retained->origin != mosq_mo_broker && retained->data.source_id){
		ಸಂರಚನಾ mosquitto retain_ctxt;
		memset(&retain_ctxt, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto));

		retain_ctxt.id = retained->data.source_id;
		retain_ctxt.username = retained->data.source_username;
		retain_ctxt.listener = retained->source_listener;

		rc = mosquitto_acl_check(&retain_ctxt, retained->data.topic, retained->data.payloadlen, retained->data.payload,
				retained->data.qos, retained->data.retain, retained->data.properties, MOSQ_ACL_WRITE);
		ಯದಿ(rc == MOSQ_ERR_ACL_DENIED){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
		}ಅನ್ಯಥಾ ಯದಿ(rc != MOSQ_ERR_SUCCESS){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}

	sub_qos = sub->options & 0x03;
	ಯದಿ(db.config->upgrade_outgoing_qos){
		qos = sub_qos;
	}ಅನ್ಯಥಾ{
		qos = retained->data.qos;
		ಯದಿ(qos > sub_qos){
			qos = sub_qos;
		}
	}
	ಯದಿ(qos > 0){
		mid = mosquitto__mid_generate(context);
	}ಅನ್ಯಥಾ{
		mid = 0;
	}
	ಪ್ರತ್ಯಾಗಮ db_message_ಸೇರಿಸು_outgoing(context, 0, mid, qos, true, retained, sub->identifier, false, true);
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ retain_ಹುಡುಕು(ಸಂರಚನಾ mosquitto__retainhier *retainhier, ವರ್ಣ **split_topics, ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ಸಂರಚನಾ mosquitto_subscription *sub, ಪೂರ್ಣಾಙ್ಕ level)
{
	ಸಂರಚನಾ mosquitto__retainhier *branch, *branch_tmp;
	ಪೂರ್ಣಾಙ್ಕ flag = 0;

	ಯದಿ(!strcmp(split_topics[0], "#") && split_topics[1] == ರಿಕ್ತ){
		HASH_ITER(hh, retainhier->children, branch, branch_tmp){
			/* Set flag to indicate that we should check for retained messages
			 * on "foo" when we are subscribing to e.g. "foo/#" and then exit
			 * this function and return to an earlier retain__search().
			 */
			flag = -1;
			ಯದಿ(branch->retained){
				retain__process(branch, context, sub);
			}
			ಯದಿ(branch->children){
				retain_ಹುಡುಕು(branch, split_topics, context, sub, level+1);
			}
		}
	}ಅನ್ಯಥಾ{
		ಯದಿ(!strcmp(split_topics[0], "+")){
			HASH_ITER(hh, retainhier->children, branch, branch_tmp){
				ಯದಿ(split_topics[1] != ರಿಕ್ತ){
					ಯದಿ(retain_ಹುಡುಕು(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != ರಿಕ್ತ && !strcmp(split_topics[1], "#") && level>0)){

						ಯದಿ(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}ಅನ್ಯಥಾ{
					ಯದಿ(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}ಅನ್ಯಥಾ{
			HASH_FIND(hh, retainhier->children, split_topics[0], strlen(split_topics[0]), branch);
			ಯದಿ(branch){
				ಯದಿ(split_topics[1] != ರಿಕ್ತ){
					ಯದಿ(retain_ಹುಡುಕು(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != ರಿಕ್ತ && !strcmp(split_topics[1], "#") && level>0)){

						ಯದಿ(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}ಅನ್ಯಥಾ{
					ಯದಿ(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}
	}
	ಪ್ರತ್ಯಾಗಮ flag;
}


ಪೂರ್ಣಾಙ್ಕ retain__queue(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ಸಂರಚನಾ mosquitto_subscription *sub)
{
	ಸಂರಚನಾ mosquitto__retainhier *retainhier;
	ವರ್ಣ *local_sub;
	ವರ್ಣ **split_topics;
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಪ್ರತಿಜ್ಞಾ(context);
	ಪ್ರತಿಜ್ಞಾ(sub);

	ಯದಿ(!strncmp(sub->topic_filter, "$share/", strlen("$share/"))){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &split_topics, ರಿಕ್ತ);
	ಯದಿ(rc){
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	HASH_FIND(hh, db.retains, split_topics[0], strlen(split_topics[0]), retainhier);

	ಯದಿ(retainhier){
		retain_ಹುಡುಕು(retainhier, split_topics, context, sub, 0);
	}
	mosquitto_FREE(local_sub);
	mosquitto_FREE(split_topics);

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಶೂನ್ಯ retain__expire(ಸಂರಚನಾ mosquitto__retainhier **retainhier)
{
	ಸಂರಚನಾ mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		retain__expire(&peer->children);

		ಸಂರಚನಾ mosquitto__retainhier *parent = peer->parent;
		ಯದಿ(retain_delete_expired_ಸಂದೇಶ(peer)){
			retain__clean_empty_hierarchy(parent);
		}
	}
}


ಶೂನ್ಯ retain__clean(ಸಂರಚನಾ mosquitto__retainhier **retainhier)
{
	ಸಂರಚನಾ mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		ಯದಿ(peer->retained){
			db_ಸಂದೇಶ_store_ref_dec(&peer->retained);
		}
		retain__clean(&peer->children);

		HASH_DELETE(hh, *retainhier, peer);
		mosquitto_FREE(peer);
	}
}


ಶೂನ್ಯ retain__expiry_check(ಶೂನ್ಯ)
{
	ಯದಿ(db.config->retain_expiry_interval > 0 && db.now_s > next_expire_check){
		retain__expire(&db.retains);
		next_expire_check = db.now_s + db.config->retain_expiry_interval;
	}
}
