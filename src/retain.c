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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto/mqtt_protocol.h"
#அந்தர்பாவ "util_mosq.h"

#அந்தர்பாவ "utlist.h"

நிலையான time_t next_expire_check = 0;

நிலையான கட்டமைப்பு mosquitto__retainhier *retain__add_hier_entry(கட்டமைப்பு mosquitto__retainhier *parent, கட்டமைப்பு mosquitto__retainhier **sibling, மாறிலி எழுத்து *topic, uint16_t len)
{
	கட்டமைப்பு mosquitto__retainhier *child;

	ப்ரதிஜ்ஞா(sibling);

	child = mosquitto_calloc(1, அளவு(கட்டமைப்பு mosquitto__retainhier) + len + 1);
	எனில்(!child){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
		திரும்பு வெற்று;
	}
	child->parent = parent;
	child->topic_len = len;
	எனில்(len > 0){
		strncpy(child->topic, topic, (size_t)(len+1));
	}

	HASH_ADD(hh, *sibling, topic, child->topic_len, child);

	திரும்பு child;
}


முழு retain__init(வெறுமை)
{
	கட்டமைப்பு mosquitto__retainhier *retainhier;

	retainhier = retain__add_hier_entry(வெற்று, &db.retains, "", 0);
	எனில்(!retainhier){
		திரும்பு MOSQ_ERR_NOMEM;
	}

	retainhier = retain__add_hier_entry(வெற்று, &db.retains, "$SYS", (uint16_t)strlen("$SYS"));
	எனில்(!retainhier){
		திரும்பு MOSQ_ERR_NOMEM;
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT முழு mosquitto_persist_retain_செய்தி_set(மாறிலி எழுத்து *topic, uint64_t base_செய்தி_id)
{
	கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி;
	முழு rc = MOSQ_ERR_UNKNOWN;
	எழுத்து **split_topics = வெற்று;
	எழுத்து *local_topic = வெற்று;

	எனில்(topic == வெற்று){
		திரும்பு MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh, db.செய்தி_store, &base_செய்தி_id, அளவு(base_செய்தி_id), base_செய்தி);
	எனில்(base_செய்தி){
		எனில்(sub__topic_tokenise(topic, &local_topic, &split_topics, வெற்று)){
			திரும்பு MOSQ_ERR_NOMEM;
		}

		rc = retain__store(topic, base_செய்தி, split_topics, false);
		mosquitto_free(split_topics);
		mosquitto_free(local_topic);
	}

	திரும்பு rc;
}


BROKER_EXPORT முழு mosquitto_persist_retain_செய்தி_delete(மாறிலி எழுத்து *topic)
{
	கட்டமைப்பு mosquitto_base_செய்தி base_செய்தி;
	முழு rc = MOSQ_ERR_UNKNOWN;
	எழுத்து **split_topics = வெற்று;
	எழுத்து *local_topic = வெற்று;

	எனில்(topic == வெற்று){
		திரும்பு MOSQ_ERR_INVAL;
	}

	memset(&base_செய்தி, 0, அளவு(base_செய்தி));
	base_செய்தி.ref_மொத்தம் = 10; /* Ensure this isn't freed */

	எனில்(sub__topic_tokenise(topic, &local_topic, &split_topics, வெற்று)){
		திரும்பு MOSQ_ERR_NOMEM;
	}

	/* With stored->payloadlen == 0, this means the message will be removed */
	rc = retain__store(topic, &base_செய்தி, split_topics, false);
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);

	திரும்பு rc;
}


வெறுமை retain__clean_empty_hierarchy(கட்டமைப்பு mosquitto__retainhier *retainhier)
{
	போது(retainhier){
		எனில்(retainhier->children || retainhier->retained || retainhier->parent == வெற்று){
			/* Entry is being used */
			திரும்பு;
		}இல்லையெனில்{
			HASH_DELETE(hh, retainhier->parent->children, retainhier);

			கட்டமைப்பு mosquitto__retainhier *parent = retainhier->parent;
			mosquitto_FREE(retainhier);
			retainhier = parent;
		}
	}
}


முழு retain__store(மாறிலி எழுத்து *topic, கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி, எழுத்து **split_topics, bool persist)
{
	கட்டமைப்பு mosquitto__retainhier *retainhier;
	கட்டமைப்பு mosquitto__retainhier *branch;
	size_t slen;

	ப்ரதிஜ்ஞா(base_செய்தி);
	ப்ரதிஜ்ஞா(split_topics);

	HASH_FIND(hh, db.retains, split_topics[0], strlen(split_topics[0]), retainhier);
	எனில்(retainhier == வெற்று){
		retainhier = retain__add_hier_entry(வெற்று, &db.retains, split_topics[0], (uint16_t)strlen(split_topics[0]));
		எனில்(!retainhier){
			திரும்பு MOSQ_ERR_NOMEM;
		}
	}

	க்கு(முழு i=0; split_topics[i] != வெற்று; i++){
		slen = strlen(split_topics[i]);
		HASH_FIND(hh, retainhier->children, split_topics[i], slen, branch);
		எனில்(branch == வெற்று){
			branch = retain__add_hier_entry(retainhier, &retainhier->children, split_topics[i], (uint16_t)slen);
			எனில்(branch == வெற்று){
				திரும்பு MOSQ_ERR_NOMEM;
			}
		}
		retainhier = branch;
	}

#ifdef WITH_PERSISTENCE
	எனில்(strncmp(topic, "$SYS", 4)){
		/* Retained messages count as a persistence change, but only if
		 * they aren't for $SYS. */
		db.persistence_changes++;
	}
#இல்லையெனில்
	UNUSED(topic);
#endif

	எனில்(retainhier->retained){
		எனில்(retainhier->retained == base_செய்தி){
			/* This may occur if multiple persistence providers are used */
			திரும்பு MOSQ_ERR_SUCCESS;
		}

		எனில்(persist && retainhier->retained->data.topic[0] != '$' && base_செய்தி->data.payloadlen == 0){
			/* Only delete if another retained message isn't replacing this one */
			plugin_persist_handle_retain_செய்தி_delete(retainhier->retained);
		}
		db_செய்தி_store_ref_dec(&retainhier->retained);
#ifdef WITH_SYS_மரம்
		db.retained_மொத்தம்--;
#endif
		எனில்(base_செய்தி->data.payloadlen == 0){
			retainhier->retained = வெற்று;
			retain__clean_empty_hierarchy(retainhier);
		}
	}
	எனில்(base_செய்தி->data.payloadlen){
		retainhier->retained = base_செய்தி;
		db_செய்தி_store_ref_inc(retainhier->retained);
		எனில்(persist && retainhier->retained->data.topic[0] != '$'){
			plugin_persist_handle_base_செய்தி_add(retainhier->retained);
			plugin_persist_handle_retain_செய்தி_set(retainhier->retained);
		}
#ifdef WITH_SYS_மரம்
		db.retained_மொத்தம்++;
#endif
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான bool retain_delete_expired_செய்தி(கட்டமைப்பு mosquitto__retainhier *branch)
{
	எனில்(branch->retained && branch->retained->data.expiry_time > 0 && db.now_real_s >= branch->retained->data.expiry_time){
		plugin_persist_handle_retain_செய்தி_delete(branch->retained);
		db_செய்தி_store_ref_dec(&branch->retained);
		branch->retained = வெற்று;
#ifdef WITH_SYS_மரம்
		db.retained_மொத்தம்--;
#endif
		திரும்பு true;
	}
	திரும்பு false;
}


நிலையான முழு retain__process(கட்டமைப்பு mosquitto__retainhier *branch, கட்டமைப்பு mosquitto *context, மாறிலி கட்டமைப்பு mosquitto_subscription *sub)
{
	முழு rc = 0;
	uint8_t qos, sub_qos;
	uint16_t mid;
	கட்டமைப்பு mosquitto_base_செய்தி *retained;

	எனில்(retain_delete_expired_செய்தி(branch)){
		திரும்பு MOSQ_ERR_SUCCESS;
	}

	retained = branch->retained;

	rc = mosquitto_acl_check(context, retained->data.topic, retained->data.payloadlen, retained->data.payload,
			retained->data.qos, retained->data.retain, retained->data.properties, MOSQ_ACL_READ);
	எனில்(rc == MOSQ_ERR_ACL_DENIED){
		திரும்பு MOSQ_ERR_SUCCESS;
	}இல்லையெனில் எனில்(rc != MOSQ_ERR_SUCCESS){
		திரும்பு rc;
	}

	/* Check for original source access */
	எனில்(db.config->check_retain_source && retained->origin != mosq_mo_broker && retained->data.source_id){
		கட்டமைப்பு mosquitto retain_ctxt;
		memset(&retain_ctxt, 0, அளவு(கட்டமைப்பு mosquitto));

		retain_ctxt.id = retained->data.source_id;
		retain_ctxt.username = retained->data.source_username;
		retain_ctxt.listener = retained->source_listener;

		rc = mosquitto_acl_check(&retain_ctxt, retained->data.topic, retained->data.payloadlen, retained->data.payload,
				retained->data.qos, retained->data.retain, retained->data.properties, MOSQ_ACL_WRITE);
		எனில்(rc == MOSQ_ERR_ACL_DENIED){
			திரும்பு MOSQ_ERR_SUCCESS;
		}இல்லையெனில் எனில்(rc != MOSQ_ERR_SUCCESS){
			திரும்பு rc;
		}
	}

	sub_qos = sub->options & 0x03;
	எனில்(db.config->upgrade_outgoing_qos){
		qos = sub_qos;
	}இல்லையெனில்{
		qos = retained->data.qos;
		எனில்(qos > sub_qos){
			qos = sub_qos;
		}
	}
	எனில்(qos > 0){
		mid = mosquitto__mid_generate(context);
	}இல்லையெனில்{
		mid = 0;
	}
	திரும்பு db_message_செருகு_outgoing(context, 0, mid, qos, true, retained, sub->identifier, false, true);
}


நிலையான முழு retain_தேடல்(கட்டமைப்பு mosquitto__retainhier *retainhier, எழுத்து **split_topics, கட்டமைப்பு mosquitto *context, மாறிலி கட்டமைப்பு mosquitto_subscription *sub, முழு level)
{
	கட்டமைப்பு mosquitto__retainhier *branch, *branch_tmp;
	முழு flag = 0;

	எனில்(!strcmp(split_topics[0], "#") && split_topics[1] == வெற்று){
		HASH_ITER(hh, retainhier->children, branch, branch_tmp){
			/* Set flag to indicate that we should check for retained messages
			 * on "foo" when we are subscribing to e.g. "foo/#" and then exit
			 * this function and return to an earlier retain__search().
			 */
			flag = -1;
			எனில்(branch->retained){
				retain__process(branch, context, sub);
			}
			எனில்(branch->children){
				retain_தேடல்(branch, split_topics, context, sub, level+1);
			}
		}
	}இல்லையெனில்{
		எனில்(!strcmp(split_topics[0], "+")){
			HASH_ITER(hh, retainhier->children, branch, branch_tmp){
				எனில்(split_topics[1] != வெற்று){
					எனில்(retain_தேடல்(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != வெற்று && !strcmp(split_topics[1], "#") && level>0)){

						எனில்(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}இல்லையெனில்{
					எனில்(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}இல்லையெனில்{
			HASH_FIND(hh, retainhier->children, split_topics[0], strlen(split_topics[0]), branch);
			எனில்(branch){
				எனில்(split_topics[1] != வெற்று){
					எனில்(retain_தேடல்(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != வெற்று && !strcmp(split_topics[1], "#") && level>0)){

						எனில்(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}இல்லையெனில்{
					எனில்(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}
	}
	திரும்பு flag;
}


முழு retain__queue(கட்டமைப்பு mosquitto *context, மாறிலி கட்டமைப்பு mosquitto_subscription *sub)
{
	கட்டமைப்பு mosquitto__retainhier *retainhier;
	எழுத்து *local_sub;
	எழுத்து **split_topics;
	முழு rc;

	ப்ரதிஜ்ஞா(context);
	ப்ரதிஜ்ஞா(sub);

	எனில்(!strncmp(sub->topic_filter, "$share/", strlen("$share/"))){
		திரும்பு MOSQ_ERR_SUCCESS;
	}

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &split_topics, வெற்று);
	எனில்(rc){
		திரும்பு rc;
	}

	HASH_FIND(hh, db.retains, split_topics[0], strlen(split_topics[0]), retainhier);

	எனில்(retainhier){
		retain_தேடல்(retainhier, split_topics, context, sub, 0);
	}
	mosquitto_FREE(local_sub);
	mosquitto_FREE(split_topics);

	திரும்பு MOSQ_ERR_SUCCESS;
}


வெறுமை retain__expire(கட்டமைப்பு mosquitto__retainhier **retainhier)
{
	கட்டமைப்பு mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		retain__expire(&peer->children);

		கட்டமைப்பு mosquitto__retainhier *parent = peer->parent;
		எனில்(retain_delete_expired_செய்தி(peer)){
			retain__clean_empty_hierarchy(parent);
		}
	}
}


வெறுமை retain__clean(கட்டமைப்பு mosquitto__retainhier **retainhier)
{
	கட்டமைப்பு mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		எனில்(peer->retained){
			db_செய்தி_store_ref_dec(&peer->retained);
		}
		retain__clean(&peer->children);

		HASH_DELETE(hh, *retainhier, peer);
		mosquitto_FREE(peer);
	}
}


வெறுமை retain__expiry_check(வெறுமை)
{
	எனில்(db.config->retain_expiry_interval > 0 && db.now_s > next_expire_check){
		retain__expire(&db.retains);
		next_expire_check = db.now_s + db.config->retain_expiry_interval;
	}
}
