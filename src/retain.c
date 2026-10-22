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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"
#અન્તર્ભાવ "util_mosq.h"

#અન્તર્ભાવ "utlist.h"

સ્થિર time_t next_expire_check = 0;

સ્થિર સંરચના mosquitto__retainhier *retain__add_hier_entry(સંરચના mosquitto__retainhier *parent, સંરચના mosquitto__retainhier **sibling, અચલ અક્ષર *topic, uint16_t len)
{
	સંરચના mosquitto__retainhier *child;

	પ્રતિજ્ઞા(sibling);

	child = mosquitto_calloc(1, કદ(સંરચના mosquitto__retainhier) + len + 1);
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


પૂર્ણાંક retain__init(શૂન્ય)
{
	સંરચના mosquitto__retainhier *retainhier;

	retainhier = retain__add_hier_entry(ખાલી, &db.retains, "", 0);
	જો(!retainhier){
		પાછા MOSQ_ERR_NOMEM;
	}

	retainhier = retain__add_hier_entry(ખાલી, &db.retains, "$SYS", (uint16_t)strlen("$SYS"));
	જો(!retainhier){
		પાછા MOSQ_ERR_NOMEM;
	}

	પાછા MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT પૂર્ણાંક mosquitto_persist_retain_સંદેશ_set(અચલ અક્ષર *topic, uint64_t base_સંદેશ_id)
{
	સંરચના mosquitto_base_સંદેશ *base_સંદેશ;
	પૂર્ણાંક rc = MOSQ_ERR_UNKNOWN;
	અક્ષર **split_topics = ખાલી;
	અક્ષર *local_topic = ખાલી;

	જો(topic == ખાલી){
		પાછા MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh, db.સંદેશ_store, &base_સંદેશ_id, કદ(base_સંદેશ_id), base_સંદેશ);
	જો(base_સંદેશ){
		જો(sub__topic_tokenise(topic, &local_topic, &split_topics, ખાલી)){
			પાછા MOSQ_ERR_NOMEM;
		}

		rc = retain__store(topic, base_સંદેશ, split_topics, false);
		mosquitto_free(split_topics);
		mosquitto_free(local_topic);
	}

	પાછા rc;
}


BROKER_EXPORT પૂર્ણાંક mosquitto_persist_retain_સંદેશ_delete(અચલ અક્ષર *topic)
{
	સંરચના mosquitto_base_સંદેશ base_સંદેશ;
	પૂર્ણાંક rc = MOSQ_ERR_UNKNOWN;
	અક્ષર **split_topics = ખાલી;
	અક્ષર *local_topic = ખાલી;

	જો(topic == ખાલી){
		પાછા MOSQ_ERR_INVAL;
	}

	memset(&base_સંદેશ, 0, કદ(base_સંદેશ));
	base_સંદેશ.ref_ગણતરી = 10; /* Ensure this isn't freed */

	જો(sub__topic_tokenise(topic, &local_topic, &split_topics, ખાલી)){
		પાછા MOSQ_ERR_NOMEM;
	}

	/* With stored->payloadlen == 0, this means the message will be removed */
	rc = retain__store(topic, &base_સંદેશ, split_topics, false);
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);

	પાછા rc;
}


શૂન્ય retain__clean_empty_hierarchy(સંરચના mosquitto__retainhier *retainhier)
{
	જ્યારે(retainhier){
		જો(retainhier->children || retainhier->retained || retainhier->parent == ખાલી){
			/* Entry is being used */
			પાછા;
		}નહીંતર{
			HASH_DELETE(hh, retainhier->parent->children, retainhier);

			સંરચના mosquitto__retainhier *parent = retainhier->parent;
			mosquitto_FREE(retainhier);
			retainhier = parent;
		}
	}
}


પૂર્ણાંક retain__store(અચલ અક્ષર *topic, સંરચના mosquitto_base_સંદેશ *base_સંદેશ, અક્ષર **split_topics, bool persist)
{
	સંરચના mosquitto__retainhier *retainhier;
	સંરચના mosquitto__retainhier *branch;
	size_t slen;

	પ્રતિજ્ઞા(base_સંદેશ);
	પ્રતિજ્ઞા(split_topics);

	HASH_FIND(hh, db.retains, split_topics[0], strlen(split_topics[0]), retainhier);
	જો(retainhier == ખાલી){
		retainhier = retain__add_hier_entry(ખાલી, &db.retains, split_topics[0], (uint16_t)strlen(split_topics[0]));
		જો(!retainhier){
			પાછા MOSQ_ERR_NOMEM;
		}
	}

	માટે(પૂર્ણાંક i=0; split_topics[i] != ખાલી; i++){
		slen = strlen(split_topics[i]);
		HASH_FIND(hh, retainhier->children, split_topics[i], slen, branch);
		જો(branch == ખાલી){
			branch = retain__add_hier_entry(retainhier, &retainhier->children, split_topics[i], (uint16_t)slen);
			જો(branch == ખાલી){
				પાછા MOSQ_ERR_NOMEM;
			}
		}
		retainhier = branch;
	}

#ifdef WITH_PERSISTENCE
	જો(strncmp(topic, "$SYS", 4)){
		/* Retained messages count as a persistence change, but only if
		 * they aren't for $SYS. */
		db.persistence_changes++;
	}
#નહીંતર
	UNUSED(topic);
#endif

	જો(retainhier->retained){
		જો(retainhier->retained == base_સંદેશ){
			/* This may occur if multiple persistence providers are used */
			પાછા MOSQ_ERR_SUCCESS;
		}

		જો(persist && retainhier->retained->data.topic[0] != '$' && base_સંદેશ->data.payloadlen == 0){
			/* Only delete if another retained message isn't replacing this one */
			plugin_persist_handle_retain_સંદેશ_delete(retainhier->retained);
		}
		db_સંદેશ_store_ref_dec(&retainhier->retained);
#ifdef WITH_SYS_વૃક્ષ
		db.retained_ગણતરી--;
#endif
		જો(base_સંદેશ->data.payloadlen == 0){
			retainhier->retained = ખાલી;
			retain__clean_empty_hierarchy(retainhier);
		}
	}
	જો(base_સંદેશ->data.payloadlen){
		retainhier->retained = base_સંદેશ;
		db_સંદેશ_store_ref_inc(retainhier->retained);
		જો(persist && retainhier->retained->data.topic[0] != '$'){
			plugin_persist_handle_base_સંદેશ_add(retainhier->retained);
			plugin_persist_handle_retain_સંદેશ_set(retainhier->retained);
		}
#ifdef WITH_SYS_વૃક્ષ
		db.retained_ગણતરી++;
#endif
	}

	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર bool retain_delete_expired_સંદેશ(સંરચના mosquitto__retainhier *branch)
{
	જો(branch->retained && branch->retained->data.expiry_time > 0 && db.now_real_s >= branch->retained->data.expiry_time){
		plugin_persist_handle_retain_સંદેશ_delete(branch->retained);
		db_સંદેશ_store_ref_dec(&branch->retained);
		branch->retained = ખાલી;
#ifdef WITH_SYS_વૃક્ષ
		db.retained_ગણતરી--;
#endif
		પાછા true;
	}
	પાછા false;
}


સ્થિર પૂર્ણાંક retain__process(સંરચના mosquitto__retainhier *branch, સંરચના mosquitto *context, અચલ સંરચના mosquitto_subscription *sub)
{
	પૂર્ણાંક rc = 0;
	uint8_t qos, sub_qos;
	uint16_t mid;
	સંરચના mosquitto_base_સંદેશ *retained;

	જો(retain_delete_expired_સંદેશ(branch)){
		પાછા MOSQ_ERR_SUCCESS;
	}

	retained = branch->retained;

	rc = mosquitto_acl_check(context, retained->data.topic, retained->data.payloadlen, retained->data.payload,
			retained->data.qos, retained->data.retain, retained->data.properties, MOSQ_ACL_READ);
	જો(rc == MOSQ_ERR_ACL_DENIED){
		પાછા MOSQ_ERR_SUCCESS;
	}નહીંતર જો(rc != MOSQ_ERR_SUCCESS){
		પાછા rc;
	}

	/* Check for original source access */
	જો(db.config->check_retain_source && retained->origin != mosq_mo_broker && retained->data.source_id){
		સંરચના mosquitto retain_ctxt;
		memset(&retain_ctxt, 0, કદ(સંરચના mosquitto));

		retain_ctxt.id = retained->data.source_id;
		retain_ctxt.username = retained->data.source_username;
		retain_ctxt.listener = retained->source_listener;

		rc = mosquitto_acl_check(&retain_ctxt, retained->data.topic, retained->data.payloadlen, retained->data.payload,
				retained->data.qos, retained->data.retain, retained->data.properties, MOSQ_ACL_WRITE);
		જો(rc == MOSQ_ERR_ACL_DENIED){
			પાછા MOSQ_ERR_SUCCESS;
		}નહીંતર જો(rc != MOSQ_ERR_SUCCESS){
			પાછા rc;
		}
	}

	sub_qos = sub->options & 0x03;
	જો(db.config->upgrade_outgoing_qos){
		qos = sub_qos;
	}નહીંતર{
		qos = retained->data.qos;
		જો(qos > sub_qos){
			qos = sub_qos;
		}
	}
	જો(qos > 0){
		mid = mosquitto__mid_generate(context);
	}નહીંતર{
		mid = 0;
	}
	પાછા db_message_ઉમેરો_outgoing(context, 0, mid, qos, true, retained, sub->identifier, false, true);
}


સ્થિર પૂર્ણાંક retain_શોધ(સંરચના mosquitto__retainhier *retainhier, અક્ષર **split_topics, સંરચના mosquitto *context, અચલ સંરચના mosquitto_subscription *sub, પૂર્ણાંક level)
{
	સંરચના mosquitto__retainhier *branch, *branch_tmp;
	પૂર્ણાંક flag = 0;

	જો(!strcmp(split_topics[0], "#") && split_topics[1] == ખાલી){
		HASH_ITER(hh, retainhier->children, branch, branch_tmp){
			/* Set flag to indicate that we should check for retained messages
			 * on "foo" when we are subscribing to e.g. "foo/#" and then exit
			 * this function and return to an earlier retain__search().
			 */
			flag = -1;
			જો(branch->retained){
				retain__process(branch, context, sub);
			}
			જો(branch->children){
				retain_શોધ(branch, split_topics, context, sub, level+1);
			}
		}
	}નહીંતર{
		જો(!strcmp(split_topics[0], "+")){
			HASH_ITER(hh, retainhier->children, branch, branch_tmp){
				જો(split_topics[1] != ખાલી){
					જો(retain_શોધ(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != ખાલી && !strcmp(split_topics[1], "#") && level>0)){

						જો(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}નહીંતર{
					જો(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}નહીંતર{
			HASH_FIND(hh, retainhier->children, split_topics[0], strlen(split_topics[0]), branch);
			જો(branch){
				જો(split_topics[1] != ખાલી){
					જો(retain_શોધ(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != ખાલી && !strcmp(split_topics[1], "#") && level>0)){

						જો(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}નહીંતર{
					જો(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}
	}
	પાછા flag;
}


પૂર્ણાંક retain__queue(સંરચના mosquitto *context, અચલ સંરચના mosquitto_subscription *sub)
{
	સંરચના mosquitto__retainhier *retainhier;
	અક્ષર *local_sub;
	અક્ષર **split_topics;
	પૂર્ણાંક rc;

	પ્રતિજ્ઞા(context);
	પ્રતિજ્ઞા(sub);

	જો(!strncmp(sub->topic_filter, "$share/", strlen("$share/"))){
		પાછા MOSQ_ERR_SUCCESS;
	}

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &split_topics, ખાલી);
	જો(rc){
		પાછા rc;
	}

	HASH_FIND(hh, db.retains, split_topics[0], strlen(split_topics[0]), retainhier);

	જો(retainhier){
		retain_શોધ(retainhier, split_topics, context, sub, 0);
	}
	mosquitto_FREE(local_sub);
	mosquitto_FREE(split_topics);

	પાછા MOSQ_ERR_SUCCESS;
}


શૂન્ય retain__expire(સંરચના mosquitto__retainhier **retainhier)
{
	સંરચના mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		retain__expire(&peer->children);

		સંરચના mosquitto__retainhier *parent = peer->parent;
		જો(retain_delete_expired_સંદેશ(peer)){
			retain__clean_empty_hierarchy(parent);
		}
	}
}


શૂન્ય retain__clean(સંરચના mosquitto__retainhier **retainhier)
{
	સંરચના mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		જો(peer->retained){
			db_સંદેશ_store_ref_dec(&peer->retained);
		}
		retain__clean(&peer->children);

		HASH_DELETE(hh, *retainhier, peer);
		mosquitto_FREE(peer);
	}
}


શૂન્ય retain__expiry_check(શૂન્ય)
{
	જો(db.config->retain_expiry_interval > 0 && db.now_s > next_expire_check){
		retain__expire(&db.retains);
		next_expire_check = db.now_s + db.config->retain_expiry_interval;
	}
}
