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

#شامل "config.h"

#شامل <assert.h>
#شامل <stdio.h>
#شامل <string.h>

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "util_mosq.h"

#شامل "utlist.h"

ایستا time_t next_expire_check = 0;

ایستا ساختار mosquitto__retainhier *retain__add_hier_entry(ساختار mosquitto__retainhier *parent, ساختار mosquitto__retainhier **sibling, ثابت نویسه *topic, uint16_t len)
{
	ساختار mosquitto__retainhier *child;

	أكد(sibling);

	child = mosquitto_calloc(1, اندازه(ساختار mosquitto__retainhier) + len + 1);
	اگر(!child){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
		بازگشت پوچ;
	}
	child->parent = parent;
	child->topic_len = len;
	اگر(len > 0){
		strncpy(child->topic, topic, (size_t)(len+1));
	}

	HASH_ADD(hh, *sibling, topic, child->topic_len, child);

	بازگشت child;
}


صحیح retain__init(تهی)
{
	ساختار mosquitto__retainhier *retainhier;

	retainhier = retain__add_hier_entry(پوچ, &db.retains, "", 0);
	اگر(!retainhier){
		بازگشت MOSQ_ERR_NOMEM;
	}

	retainhier = retain__add_hier_entry(پوچ, &db.retains, "$SYS", (uint16_t)strlen("$SYS"));
	اگر(!retainhier){
		بازگشت MOSQ_ERR_NOMEM;
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحیح mosquitto_persist_retain_پیام_set(ثابت نویسه *topic, uint64_t base_پیام_id)
{
	ساختار mosquitto_base_پیام *base_پیام;
	صحیح rc = MOSQ_ERR_UNKNOWN;
	نویسه **split_topics = پوچ;
	نویسه *local_topic = پوچ;

	اگر(topic == پوچ){
		بازگشت MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh, db.پیام_store, &base_پیام_id, اندازه(base_پیام_id), base_پیام);
	اگر(base_پیام){
		اگر(sub__topic_tokenise(topic, &local_topic, &split_topics, پوچ)){
			بازگشت MOSQ_ERR_NOMEM;
		}

		rc = retain__store(topic, base_پیام, split_topics, false);
		mosquitto_free(split_topics);
		mosquitto_free(local_topic);
	}

	بازگشت rc;
}


BROKER_EXPORT صحیح mosquitto_persist_retain_پیام_delete(ثابت نویسه *topic)
{
	ساختار mosquitto_base_پیام base_پیام;
	صحیح rc = MOSQ_ERR_UNKNOWN;
	نویسه **split_topics = پوچ;
	نویسه *local_topic = پوچ;

	اگر(topic == پوچ){
		بازگشت MOSQ_ERR_INVAL;
	}

	memset(&base_پیام, 0, اندازه(base_پیام));
	base_پیام.ref_شمار = 10; /* Ensure this isn't freed */

	اگر(sub__topic_tokenise(topic, &local_topic, &split_topics, پوچ)){
		بازگشت MOSQ_ERR_NOMEM;
	}

	/* With stored->payloadlen == 0, this means the message will be removed */
	rc = retain__store(topic, &base_پیام, split_topics, false);
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);

	بازگشت rc;
}


تهی retain__clean_empty_hierarchy(ساختار mosquitto__retainhier *retainhier)
{
	وقتی(retainhier){
		اگر(retainhier->children || retainhier->retained || retainhier->parent == پوچ){
			/* Entry is being used */
			بازگشت;
		}وگرنه{
			HASH_DELETE(hh, retainhier->parent->children, retainhier);

			ساختار mosquitto__retainhier *parent = retainhier->parent;
			mosquitto_FREE(retainhier);
			retainhier = parent;
		}
	}
}


صحیح retain__store(ثابت نویسه *topic, ساختار mosquitto_base_پیام *base_پیام, نویسه **split_topics, bool persist)
{
	ساختار mosquitto__retainhier *retainhier;
	ساختار mosquitto__retainhier *branch;
	size_t slen;

	أكد(base_پیام);
	أكد(split_topics);

	HASH_FIND(hh, db.retains, split_topics[0], strlen(split_topics[0]), retainhier);
	اگر(retainhier == پوچ){
		retainhier = retain__add_hier_entry(پوچ, &db.retains, split_topics[0], (uint16_t)strlen(split_topics[0]));
		اگر(!retainhier){
			بازگشت MOSQ_ERR_NOMEM;
		}
	}

	برای(صحیح i=0; split_topics[i] != پوچ; i++){
		slen = strlen(split_topics[i]);
		HASH_FIND(hh, retainhier->children, split_topics[i], slen, branch);
		اگر(branch == پوچ){
			branch = retain__add_hier_entry(retainhier, &retainhier->children, split_topics[i], (uint16_t)slen);
			اگر(branch == پوچ){
				بازگشت MOSQ_ERR_NOMEM;
			}
		}
		retainhier = branch;
	}

#ifdef WITH_PERSISTENCE
	اگر(strncmp(topic, "$SYS", 4)){
		/* Retained messages count as a persistence change, but only if
		 * they aren't for $SYS. */
		db.persistence_changes++;
	}
#وگرنه
	UNUSED(topic);
#endif

	اگر(retainhier->retained){
		اگر(retainhier->retained == base_پیام){
			/* This may occur if multiple persistence providers are used */
			بازگشت MOSQ_ERR_SUCCESS;
		}

		اگر(persist && retainhier->retained->data.topic[0] != '$' && base_پیام->data.payloadlen == 0){
			/* Only delete if another retained message isn't replacing this one */
			plugin_persist_handle_retain_پیام_delete(retainhier->retained);
		}
		db_پیام_store_ref_dec(&retainhier->retained);
#ifdef WITH_SYS_درخت
		db.retained_شمار--;
#endif
		اگر(base_پیام->data.payloadlen == 0){
			retainhier->retained = پوچ;
			retain__clean_empty_hierarchy(retainhier);
		}
	}
	اگر(base_پیام->data.payloadlen){
		retainhier->retained = base_پیام;
		db_پیام_store_ref_inc(retainhier->retained);
		اگر(persist && retainhier->retained->data.topic[0] != '$'){
			plugin_persist_handle_base_پیام_add(retainhier->retained);
			plugin_persist_handle_retain_پیام_set(retainhier->retained);
		}
#ifdef WITH_SYS_درخت
		db.retained_شمار++;
#endif
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا bool retain_delete_expired_پیام(ساختار mosquitto__retainhier *branch)
{
	اگر(branch->retained && branch->retained->data.expiry_time > 0 && db.now_real_s >= branch->retained->data.expiry_time){
		plugin_persist_handle_retain_پیام_delete(branch->retained);
		db_پیام_store_ref_dec(&branch->retained);
		branch->retained = پوچ;
#ifdef WITH_SYS_درخت
		db.retained_شمار--;
#endif
		بازگشت true;
	}
	بازگشت false;
}


ایستا صحیح retain__process(ساختار mosquitto__retainhier *branch, ساختار mosquitto *context, ثابت ساختار mosquitto_subscription *sub)
{
	صحیح rc = 0;
	uint8_t qos, sub_qos;
	uint16_t mid;
	ساختار mosquitto_base_پیام *retained;

	اگر(retain_delete_expired_پیام(branch)){
		بازگشت MOSQ_ERR_SUCCESS;
	}

	retained = branch->retained;

	rc = mosquitto_acl_check(context, retained->data.topic, retained->data.payloadlen, retained->data.payload,
			retained->data.qos, retained->data.retain, retained->data.properties, MOSQ_ACL_READ);
	اگر(rc == MOSQ_ERR_ACL_DENIED){
		بازگشت MOSQ_ERR_SUCCESS;
	}وگرنه اگر(rc != MOSQ_ERR_SUCCESS){
		بازگشت rc;
	}

	/* Check for original source access */
	اگر(db.config->check_retain_source && retained->origin != mosq_mo_broker && retained->data.source_id){
		ساختار mosquitto retain_ctxt;
		memset(&retain_ctxt, 0, اندازه(ساختار mosquitto));

		retain_ctxt.id = retained->data.source_id;
		retain_ctxt.username = retained->data.source_username;
		retain_ctxt.listener = retained->source_listener;

		rc = mosquitto_acl_check(&retain_ctxt, retained->data.topic, retained->data.payloadlen, retained->data.payload,
				retained->data.qos, retained->data.retain, retained->data.properties, MOSQ_ACL_WRITE);
		اگر(rc == MOSQ_ERR_ACL_DENIED){
			بازگشت MOSQ_ERR_SUCCESS;
		}وگرنه اگر(rc != MOSQ_ERR_SUCCESS){
			بازگشت rc;
		}
	}

	sub_qos = sub->options & 0x03;
	اگر(db.config->upgrade_outgoing_qos){
		qos = sub_qos;
	}وگرنه{
		qos = retained->data.qos;
		اگر(qos > sub_qos){
			qos = sub_qos;
		}
	}
	اگر(qos > 0){
		mid = mosquitto__mid_generate(context);
	}وگرنه{
		mid = 0;
	}
	بازگشت db_message_درج_outgoing(context, 0, mid, qos, true, retained, sub->identifier, false, true);
}


ایستا صحیح retain_جستجو(ساختار mosquitto__retainhier *retainhier, نویسه **split_topics, ساختار mosquitto *context, ثابت ساختار mosquitto_subscription *sub, صحیح level)
{
	ساختار mosquitto__retainhier *branch, *branch_tmp;
	صحیح flag = 0;

	اگر(!strcmp(split_topics[0], "#") && split_topics[1] == پوچ){
		HASH_ITER(hh, retainhier->children, branch, branch_tmp){
			/* Set flag to indicate that we should check for retained messages
			 * on "foo" when we are subscribing to e.g. "foo/#" and then exit
			 * this function and return to an earlier retain__search().
			 */
			flag = -1;
			اگر(branch->retained){
				retain__process(branch, context, sub);
			}
			اگر(branch->children){
				retain_جستجو(branch, split_topics, context, sub, level+1);
			}
		}
	}وگرنه{
		اگر(!strcmp(split_topics[0], "+")){
			HASH_ITER(hh, retainhier->children, branch, branch_tmp){
				اگر(split_topics[1] != پوچ){
					اگر(retain_جستجو(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != پوچ && !strcmp(split_topics[1], "#") && level>0)){

						اگر(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}وگرنه{
					اگر(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}وگرنه{
			HASH_FIND(hh, retainhier->children, split_topics[0], strlen(split_topics[0]), branch);
			اگر(branch){
				اگر(split_topics[1] != پوچ){
					اگر(retain_جستجو(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != پوچ && !strcmp(split_topics[1], "#") && level>0)){

						اگر(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}وگرنه{
					اگر(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}
	}
	بازگشت flag;
}


صحیح retain__queue(ساختار mosquitto *context, ثابت ساختار mosquitto_subscription *sub)
{
	ساختار mosquitto__retainhier *retainhier;
	نویسه *local_sub;
	نویسه **split_topics;
	صحیح rc;

	أكد(context);
	أكد(sub);

	اگر(!strncmp(sub->topic_filter, "$share/", strlen("$share/"))){
		بازگشت MOSQ_ERR_SUCCESS;
	}

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &split_topics, پوچ);
	اگر(rc){
		بازگشت rc;
	}

	HASH_FIND(hh, db.retains, split_topics[0], strlen(split_topics[0]), retainhier);

	اگر(retainhier){
		retain_جستجو(retainhier, split_topics, context, sub, 0);
	}
	mosquitto_FREE(local_sub);
	mosquitto_FREE(split_topics);

	بازگشت MOSQ_ERR_SUCCESS;
}


تهی retain__expire(ساختار mosquitto__retainhier **retainhier)
{
	ساختار mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		retain__expire(&peer->children);

		ساختار mosquitto__retainhier *parent = peer->parent;
		اگر(retain_delete_expired_پیام(peer)){
			retain__clean_empty_hierarchy(parent);
		}
	}
}


تهی retain__clean(ساختار mosquitto__retainhier **retainhier)
{
	ساختار mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		اگر(peer->retained){
			db_پیام_store_ref_dec(&peer->retained);
		}
		retain__clean(&peer->children);

		HASH_DELETE(hh, *retainhier, peer);
		mosquitto_FREE(peer);
	}
}


تهی retain__expiry_check(تهی)
{
	اگر(db.config->retain_expiry_interval > 0 && db.now_s > next_expire_check){
		retain__expire(&db.retains);
		next_expire_check = db.now_s + db.config->retain_expiry_interval;
	}
}
