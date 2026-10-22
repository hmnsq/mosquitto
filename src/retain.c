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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto/mqtt_protocol.h"
#অন্তর্ভাব "util_mosq.h"

#অন্তর্ভাব "utlist.h"

স্থির time_t next_expire_check = 0;

স্থির সংরচনা mosquitto__retainhier *retain__add_hier_entry(সংরচনা mosquitto__retainhier *parent, সংরচনা mosquitto__retainhier **sibling, ধ্রুব বর্ণ *topic, uint16_t len)
{
	সংরচনা mosquitto__retainhier *child;

	প্রতিজ্ঞা(sibling);

	child = mosquitto_calloc(1, পরিমাণ(সংরচনা mosquitto__retainhier) + len + 1);
	যদি(!child){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
		প্রত্যাগম রিক্ত;
	}
	child->parent = parent;
	child->topic_len = len;
	যদি(len > 0){
		strncpy(child->topic, topic, (size_t)(len+1));
	}

	HASH_ADD(hh, *sibling, topic, child->topic_len, child);

	প্রত্যাগম child;
}


পূর্ণাঙ্ক retain__init(শূন্য)
{
	সংরচনা mosquitto__retainhier *retainhier;

	retainhier = retain__add_hier_entry(রিক্ত, &db.retains, "", 0);
	যদি(!retainhier){
		প্রত্যাগম MOSQ_ERR_NOMEM;
	}

	retainhier = retain__add_hier_entry(রিক্ত, &db.retains, "$SYS", (uint16_t)strlen("$SYS"));
	যদি(!retainhier){
		প্রত্যাগম MOSQ_ERR_NOMEM;
	}

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_persist_retain_বার্তা_set(ধ্রুব বর্ণ *topic, uint64_t base_বার্তা_id)
{
	সংরচনা mosquitto_base_বার্তা *base_বার্তা;
	পূর্ণাঙ্ক rc = MOSQ_ERR_UNKNOWN;
	বর্ণ **split_topics = রিক্ত;
	বর্ণ *local_topic = রিক্ত;

	যদি(topic == রিক্ত){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh, db.বার্তা_store, &base_বার্তা_id, পরিমাণ(base_বার্তা_id), base_বার্তা);
	যদি(base_বার্তা){
		যদি(sub__topic_tokenise(topic, &local_topic, &split_topics, রিক্ত)){
			প্রত্যাগম MOSQ_ERR_NOMEM;
		}

		rc = retain__store(topic, base_বার্তা, split_topics, false);
		mosquitto_free(split_topics);
		mosquitto_free(local_topic);
	}

	প্রত্যাগম rc;
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_persist_retain_বার্তা_delete(ধ্রুব বর্ণ *topic)
{
	সংরচনা mosquitto_base_বার্তা base_বার্তা;
	পূর্ণাঙ্ক rc = MOSQ_ERR_UNKNOWN;
	বর্ণ **split_topics = রিক্ত;
	বর্ণ *local_topic = রিক্ত;

	যদি(topic == রিক্ত){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	memset(&base_বার্তা, 0, পরিমাণ(base_বার্তা));
	base_বার্তা.ref_গুনতি = 10; /* Ensure this isn't freed */

	যদি(sub__topic_tokenise(topic, &local_topic, &split_topics, রিক্ত)){
		প্রত্যাগম MOSQ_ERR_NOMEM;
	}

	/* With stored->payloadlen == 0, this means the message will be removed */
	rc = retain__store(topic, &base_বার্তা, split_topics, false);
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);

	প্রত্যাগম rc;
}


শূন্য retain__clean_empty_hierarchy(সংরচনা mosquitto__retainhier *retainhier)
{
	যাবত্(retainhier){
		যদি(retainhier->children || retainhier->retained || retainhier->parent == রিক্ত){
			/* Entry is being used */
			প্রত্যাগম;
		}অন্যথা{
			HASH_DELETE(hh, retainhier->parent->children, retainhier);

			সংরচনা mosquitto__retainhier *parent = retainhier->parent;
			mosquitto_FREE(retainhier);
			retainhier = parent;
		}
	}
}


পূর্ণাঙ্ক retain__store(ধ্রুব বর্ণ *topic, সংরচনা mosquitto_base_বার্তা *base_বার্তা, বর্ণ **split_topics, bool persist)
{
	সংরচনা mosquitto__retainhier *retainhier;
	সংরচনা mosquitto__retainhier *branch;
	size_t slen;

	প্রতিজ্ঞা(base_বার্তা);
	প্রতিজ্ঞা(split_topics);

	HASH_FIND(hh, db.retains, split_topics[0], strlen(split_topics[0]), retainhier);
	যদি(retainhier == রিক্ত){
		retainhier = retain__add_hier_entry(রিক্ত, &db.retains, split_topics[0], (uint16_t)strlen(split_topics[0]));
		যদি(!retainhier){
			প্রত্যাগম MOSQ_ERR_NOMEM;
		}
	}

	প্রতি(পূর্ণাঙ্ক i=0; split_topics[i] != রিক্ত; i++){
		slen = strlen(split_topics[i]);
		HASH_FIND(hh, retainhier->children, split_topics[i], slen, branch);
		যদি(branch == রিক্ত){
			branch = retain__add_hier_entry(retainhier, &retainhier->children, split_topics[i], (uint16_t)slen);
			যদি(branch == রিক্ত){
				প্রত্যাগম MOSQ_ERR_NOMEM;
			}
		}
		retainhier = branch;
	}

#ifdef WITH_PERSISTENCE
	যদি(strncmp(topic, "$SYS", 4)){
		/* Retained messages count as a persistence change, but only if
		 * they aren't for $SYS. */
		db.persistence_changes++;
	}
#অন্যথা
	UNUSED(topic);
#endif

	যদি(retainhier->retained){
		যদি(retainhier->retained == base_বার্তা){
			/* This may occur if multiple persistence providers are used */
			প্রত্যাগম MOSQ_ERR_SUCCESS;
		}

		যদি(persist && retainhier->retained->data.topic[0] != '$' && base_বার্তা->data.payloadlen == 0){
			/* Only delete if another retained message isn't replacing this one */
			plugin_persist_handle_retain_বার্তা_delete(retainhier->retained);
		}
		db_বার্তা_store_ref_dec(&retainhier->retained);
#ifdef WITH_SYS_বৃক্ষ
		db.retained_গুনতি--;
#endif
		যদি(base_বার্তা->data.payloadlen == 0){
			retainhier->retained = রিক্ত;
			retain__clean_empty_hierarchy(retainhier);
		}
	}
	যদি(base_বার্তা->data.payloadlen){
		retainhier->retained = base_বার্তা;
		db_বার্তা_store_ref_inc(retainhier->retained);
		যদি(persist && retainhier->retained->data.topic[0] != '$'){
			plugin_persist_handle_base_বার্তা_add(retainhier->retained);
			plugin_persist_handle_retain_বার্তা_set(retainhier->retained);
		}
#ifdef WITH_SYS_বৃক্ষ
		db.retained_গুনতি++;
#endif
	}

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


স্থির bool retain_delete_expired_বার্তা(সংরচনা mosquitto__retainhier *branch)
{
	যদি(branch->retained && branch->retained->data.expiry_time > 0 && db.now_real_s >= branch->retained->data.expiry_time){
		plugin_persist_handle_retain_বার্তা_delete(branch->retained);
		db_বার্তা_store_ref_dec(&branch->retained);
		branch->retained = রিক্ত;
#ifdef WITH_SYS_বৃক্ষ
		db.retained_গুনতি--;
#endif
		প্রত্যাগম true;
	}
	প্রত্যাগম false;
}


স্থির পূর্ণাঙ্ক retain__process(সংরচনা mosquitto__retainhier *branch, সংরচনা mosquitto *context, ধ্রুব সংরচনা mosquitto_subscription *sub)
{
	পূর্ণাঙ্ক rc = 0;
	uint8_t qos, sub_qos;
	uint16_t mid;
	সংরচনা mosquitto_base_বার্তা *retained;

	যদি(retain_delete_expired_বার্তা(branch)){
		প্রত্যাগম MOSQ_ERR_SUCCESS;
	}

	retained = branch->retained;

	rc = mosquitto_acl_check(context, retained->data.topic, retained->data.payloadlen, retained->data.payload,
			retained->data.qos, retained->data.retain, retained->data.properties, MOSQ_ACL_READ);
	যদি(rc == MOSQ_ERR_ACL_DENIED){
		প্রত্যাগম MOSQ_ERR_SUCCESS;
	}অন্যথা যদি(rc != MOSQ_ERR_SUCCESS){
		প্রত্যাগম rc;
	}

	/* Check for original source access */
	যদি(db.config->check_retain_source && retained->origin != mosq_mo_broker && retained->data.source_id){
		সংরচনা mosquitto retain_ctxt;
		memset(&retain_ctxt, 0, পরিমাণ(সংরচনা mosquitto));

		retain_ctxt.id = retained->data.source_id;
		retain_ctxt.username = retained->data.source_username;
		retain_ctxt.listener = retained->source_listener;

		rc = mosquitto_acl_check(&retain_ctxt, retained->data.topic, retained->data.payloadlen, retained->data.payload,
				retained->data.qos, retained->data.retain, retained->data.properties, MOSQ_ACL_WRITE);
		যদি(rc == MOSQ_ERR_ACL_DENIED){
			প্রত্যাগম MOSQ_ERR_SUCCESS;
		}অন্যথা যদি(rc != MOSQ_ERR_SUCCESS){
			প্রত্যাগম rc;
		}
	}

	sub_qos = sub->options & 0x03;
	যদি(db.config->upgrade_outgoing_qos){
		qos = sub_qos;
	}অন্যথা{
		qos = retained->data.qos;
		যদি(qos > sub_qos){
			qos = sub_qos;
		}
	}
	যদি(qos > 0){
		mid = mosquitto__mid_generate(context);
	}অন্যথা{
		mid = 0;
	}
	প্রত্যাগম db_message_ঢোকাও_outgoing(context, 0, mid, qos, true, retained, sub->identifier, false, true);
}


স্থির পূর্ণাঙ্ক retain_অনুসন্ধান(সংরচনা mosquitto__retainhier *retainhier, বর্ণ **split_topics, সংরচনা mosquitto *context, ধ্রুব সংরচনা mosquitto_subscription *sub, পূর্ণাঙ্ক level)
{
	সংরচনা mosquitto__retainhier *branch, *branch_tmp;
	পূর্ণাঙ্ক flag = 0;

	যদি(!strcmp(split_topics[0], "#") && split_topics[1] == রিক্ত){
		HASH_ITER(hh, retainhier->children, branch, branch_tmp){
			/* Set flag to indicate that we should check for retained messages
			 * on "foo" when we are subscribing to e.g. "foo/#" and then exit
			 * this function and return to an earlier retain__search().
			 */
			flag = -1;
			যদি(branch->retained){
				retain__process(branch, context, sub);
			}
			যদি(branch->children){
				retain_অনুসন্ধান(branch, split_topics, context, sub, level+1);
			}
		}
	}অন্যথা{
		যদি(!strcmp(split_topics[0], "+")){
			HASH_ITER(hh, retainhier->children, branch, branch_tmp){
				যদি(split_topics[1] != রিক্ত){
					যদি(retain_অনুসন্ধান(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != রিক্ত && !strcmp(split_topics[1], "#") && level>0)){

						যদি(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}অন্যথা{
					যদি(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}অন্যথা{
			HASH_FIND(hh, retainhier->children, split_topics[0], strlen(split_topics[0]), branch);
			যদি(branch){
				যদি(split_topics[1] != রিক্ত){
					যদি(retain_অনুসন্ধান(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != রিক্ত && !strcmp(split_topics[1], "#") && level>0)){

						যদি(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}অন্যথা{
					যদি(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}
	}
	প্রত্যাগম flag;
}


পূর্ণাঙ্ক retain__queue(সংরচনা mosquitto *context, ধ্রুব সংরচনা mosquitto_subscription *sub)
{
	সংরচনা mosquitto__retainhier *retainhier;
	বর্ণ *local_sub;
	বর্ণ **split_topics;
	পূর্ণাঙ্ক rc;

	প্রতিজ্ঞা(context);
	প্রতিজ্ঞা(sub);

	যদি(!strncmp(sub->topic_filter, "$share/", strlen("$share/"))){
		প্রত্যাগম MOSQ_ERR_SUCCESS;
	}

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &split_topics, রিক্ত);
	যদি(rc){
		প্রত্যাগম rc;
	}

	HASH_FIND(hh, db.retains, split_topics[0], strlen(split_topics[0]), retainhier);

	যদি(retainhier){
		retain_অনুসন্ধান(retainhier, split_topics, context, sub, 0);
	}
	mosquitto_FREE(local_sub);
	mosquitto_FREE(split_topics);

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


শূন্য retain__expire(সংরচনা mosquitto__retainhier **retainhier)
{
	সংরচনা mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		retain__expire(&peer->children);

		সংরচনা mosquitto__retainhier *parent = peer->parent;
		যদি(retain_delete_expired_বার্তা(peer)){
			retain__clean_empty_hierarchy(parent);
		}
	}
}


শূন্য retain__clean(সংরচনা mosquitto__retainhier **retainhier)
{
	সংরচনা mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		যদি(peer->retained){
			db_বার্তা_store_ref_dec(&peer->retained);
		}
		retain__clean(&peer->children);

		HASH_DELETE(hh, *retainhier, peer);
		mosquitto_FREE(peer);
	}
}


শূন্য retain__expiry_check(শূন্য)
{
	যদি(db.config->retain_expiry_interval > 0 && db.now_s > next_expire_check){
		retain__expire(&db.retains);
		next_expire_check = db.now_s + db.config->retain_expiry_interval;
	}
}
