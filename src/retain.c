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

#ضمن "config.h"

#ضمن <assert.h>
#ضمن <stdio.h>
#ضمن <string.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "util_mosq.h"

#ضمن "utlist.h"

ساكن time_t next_expire_check = 0;

ساكن بنية mosquitto__retainhier *retain__add_hier_entry(بنية mosquitto__retainhier *parent, بنية mosquitto__retainhier **sibling, ثابت حرف *topic, uint16_t len)
{
	بنية mosquitto__retainhier *child;

	أكد(sibling);

	child = mosquitto_calloc(1, حجم(بنية mosquitto__retainhier) + len + 1);
	אם(!child){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		החזר عدم;
	}
	child->parent = parent;
	child->topic_len = len;
	אם(len > 0){
		strncpy(child->topic, topic, (size_t)(len+1));
	}

	HASH_ADD(hh, *sibling, topic, child->topic_len, child);

	החזר child;
}


صحيح retain__init(فراغ)
{
	بنية mosquitto__retainhier *retainhier;

	retainhier = retain__add_hier_entry(عدم, &db.retains, "", 0);
	אם(!retainhier){
		החזר MOSQ_ERR_NOMEM;
	}

	retainhier = retain__add_hier_entry(عدم, &db.retains, "$SYS", (uint16_t)strlen("$SYS"));
	אם(!retainhier){
		החזר MOSQ_ERR_NOMEM;
	}

	החזר MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_persist_retain_msg_set(ثابت حرف *topic, uint64_t base_msg_id)
{
	بنية mosquitto__base_msg *base_msg;
	صحيح rc = MOSQ_ERR_UNKNOWN;
	حرف **split_topics = عدم;
	حرف *local_topic = عدم;

	אם(topic == عدم){
		החזר MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh, db.msg_store, &base_msg_id, حجم(base_msg_id), base_msg);
	אם(base_msg){
		אם(sub__topic_tokenise(topic, &local_topic, &split_topics, عدم)){
			החזר MOSQ_ERR_NOMEM;
		}

		rc = retain__store(topic, base_msg, split_topics, false);
		mosquitto_free(split_topics);
		mosquitto_free(local_topic);
	}

	החזר rc;
}


BROKER_EXPORT صحيح mosquitto_persist_retain_msg_delete(ثابت حرف *topic)
{
	بنية mosquitto__base_msg base_msg;
	صحيح rc = MOSQ_ERR_UNKNOWN;
	حرف **split_topics = عدم;
	حرف *local_topic = عدم;

	אם(topic == عدم){
		החזר MOSQ_ERR_INVAL;
	}

	memset(&base_msg, 0, حجم(base_msg));
	base_msg.ref_count = 10; /* Ensure this isn't freed */

	אם(sub__topic_tokenise(topic, &local_topic, &split_topics, عدم)){
		החזר MOSQ_ERR_NOMEM;
	}

	/* With stored->payloadlen == 0, this means the message will be removed */
	rc = retain__store(topic, &base_msg, split_topics, false);
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);

	החזר rc;
}


فراغ retain__clean_empty_hierarchy(بنية mosquitto__retainhier *retainhier)
{
	כלעוד(retainhier){
		אם(retainhier->children || retainhier->retained || retainhier->parent == عدم){
			/* Entry is being used */
			החזר;
		}אחרת{
			HASH_DELETE(hh, retainhier->parent->children, retainhier);

			بنية mosquitto__retainhier *parent = retainhier->parent;
			mosquitto_FREE(retainhier);
			retainhier = parent;
		}
	}
}


صحيح retain__store(ثابت حرف *topic, بنية mosquitto__base_msg *base_msg, حرف **split_topics, bool persist)
{
	بنية mosquitto__retainhier *retainhier;
	بنية mosquitto__retainhier *branch;
	size_t slen;

	أكد(base_msg);
	أكد(split_topics);

	HASH_FIND(hh, db.retains, split_topics[0], strlen(split_topics[0]), retainhier);
	אם(retainhier == عدم){
		retainhier = retain__add_hier_entry(عدم, &db.retains, split_topics[0], (uint16_t)strlen(split_topics[0]));
		אם(!retainhier){
			החזר MOSQ_ERR_NOMEM;
		}
	}

	עבור(صحيح i=0; split_topics[i] != عدم; i++){
		slen = strlen(split_topics[i]);
		HASH_FIND(hh, retainhier->children, split_topics[i], slen, branch);
		אם(branch == عدم){
			branch = retain__add_hier_entry(retainhier, &retainhier->children, split_topics[i], (uint16_t)slen);
			אם(branch == عدم){
				החזר MOSQ_ERR_NOMEM;
			}
		}
		retainhier = branch;
	}

#ifdef WITH_PERSISTENCE
	אם(strncmp(topic, "$SYS", 4)){
		/* Retained messages count as a persistence change, but only if
		 * they aren't for $SYS. */
		db.persistence_changes++;
	}
#אחרת
	UNUSED(topic);
#endif

	אם(retainhier->retained){
		אם(retainhier->retained == base_msg){
			/* This may occur if multiple persistence providers are used */
			החזר MOSQ_ERR_SUCCESS;
		}

		אם(persist && retainhier->retained->data.topic[0] != '$' && base_msg->data.payloadlen == 0){
			/* Only delete if another retained message isn't replacing this one */
			plugin_persist__handle_retain_msg_delete(retainhier->retained);
		}
		db__msg_store_ref_dec(&retainhier->retained);
#ifdef WITH_SYS_TREE
		db.retained_count--;
#endif
		אם(base_msg->data.payloadlen == 0){
			retainhier->retained = عدم;
			retain__clean_empty_hierarchy(retainhier);
		}
	}
	אם(base_msg->data.payloadlen){
		retainhier->retained = base_msg;
		db__msg_store_ref_inc(retainhier->retained);
		אם(persist && retainhier->retained->data.topic[0] != '$'){
			plugin_persist__handle_base_msg_add(retainhier->retained);
			plugin_persist__handle_retain_msg_set(retainhier->retained);
		}
#ifdef WITH_SYS_TREE
		db.retained_count++;
#endif
	}

	החזר MOSQ_ERR_SUCCESS;
}


ساكن bool retain__delete_expired_msg(بنية mosquitto__retainhier *branch)
{
	אם(branch->retained && branch->retained->data.expiry_time > 0 && db.now_real_s >= branch->retained->data.expiry_time){
		plugin_persist__handle_retain_msg_delete(branch->retained);
		db__msg_store_ref_dec(&branch->retained);
		branch->retained = عدم;
#ifdef WITH_SYS_TREE
		db.retained_count--;
#endif
		החזר true;
	}
	החזר false;
}


ساكن صحيح retain__process(بنية mosquitto__retainhier *branch, بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub)
{
	صحيح rc = 0;
	uint8_t qos, sub_qos;
	uint16_t mid;
	بنية mosquitto__base_msg *retained;

	אם(retain__delete_expired_msg(branch)){
		החזר MOSQ_ERR_SUCCESS;
	}

	retained = branch->retained;

	rc = mosquitto_acl_check(context, retained->data.topic, retained->data.payloadlen, retained->data.payload,
			retained->data.qos, retained->data.retain, retained->data.properties, MOSQ_ACL_READ);
	אם(rc == MOSQ_ERR_ACL_DENIED){
		החזר MOSQ_ERR_SUCCESS;
	}אחרת אם(rc != MOSQ_ERR_SUCCESS){
		החזר rc;
	}

	/* Check for original source access */
	אם(db.config->check_retain_source && retained->origin != mosq_mo_broker && retained->data.source_id){
		بنية mosquitto retain_ctxt;
		memset(&retain_ctxt, 0, حجم(بنية mosquitto));

		retain_ctxt.id = retained->data.source_id;
		retain_ctxt.username = retained->data.source_username;
		retain_ctxt.listener = retained->source_listener;

		rc = mosquitto_acl_check(&retain_ctxt, retained->data.topic, retained->data.payloadlen, retained->data.payload,
				retained->data.qos, retained->data.retain, retained->data.properties, MOSQ_ACL_WRITE);
		אם(rc == MOSQ_ERR_ACL_DENIED){
			החזר MOSQ_ERR_SUCCESS;
		}אחרת אם(rc != MOSQ_ERR_SUCCESS){
			החזר rc;
		}
	}

	sub_qos = sub->options & 0x03;
	אם(db.config->upgrade_outgoing_qos){
		qos = sub_qos;
	}אחרת{
		qos = retained->data.qos;
		אם(qos > sub_qos){
			qos = sub_qos;
		}
	}
	אם(qos > 0){
		mid = mosquitto__mid_generate(context);
	}אחרת{
		mid = 0;
	}
	החזר db__message_insert_outgoing(context, 0, mid, qos, true, retained, sub->identifier, false, true);
}


ساكن صحيح retain__search(بنية mosquitto__retainhier *retainhier, حرف **split_topics, بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub, صحيح level)
{
	بنية mosquitto__retainhier *branch, *branch_tmp;
	صحيح flag = 0;

	אם(!strcmp(split_topics[0], "#") && split_topics[1] == عدم){
		HASH_ITER(hh, retainhier->children, branch, branch_tmp){
			/* Set flag to indicate that we should check for retained messages
			 * on "foo" when we are subscribing to e.g. "foo/#" and then exit
			 * this function and return to an earlier retain__search().
			 */
			flag = -1;
			אם(branch->retained){
				retain__process(branch, context, sub);
			}
			אם(branch->children){
				retain__search(branch, split_topics, context, sub, level+1);
			}
		}
	}אחרת{
		אם(!strcmp(split_topics[0], "+")){
			HASH_ITER(hh, retainhier->children, branch, branch_tmp){
				אם(split_topics[1] != عدم){
					אם(retain__search(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != عدم && !strcmp(split_topics[1], "#") && level>0)){

						אם(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}אחרת{
					אם(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}אחרת{
			HASH_FIND(hh, retainhier->children, split_topics[0], strlen(split_topics[0]), branch);
			אם(branch){
				אם(split_topics[1] != عدم){
					אם(retain__search(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != عدم && !strcmp(split_topics[1], "#") && level>0)){

						אם(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}אחרת{
					אם(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}
	}
	החזר flag;
}


صحيح retain__queue(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub)
{
	بنية mosquitto__retainhier *retainhier;
	حرف *local_sub;
	حرف **split_topics;
	صحيح rc;

	أكد(context);
	أكد(sub);

	אם(!strncmp(sub->topic_filter, "$share/", strlen("$share/"))){
		החזר MOSQ_ERR_SUCCESS;
	}

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &split_topics, عدم);
	אם(rc){
		החזר rc;
	}

	HASH_FIND(hh, db.retains, split_topics[0], strlen(split_topics[0]), retainhier);

	אם(retainhier){
		retain__search(retainhier, split_topics, context, sub, 0);
	}
	mosquitto_FREE(local_sub);
	mosquitto_FREE(split_topics);

	החזר MOSQ_ERR_SUCCESS;
}


فراغ retain__expire(بنية mosquitto__retainhier **retainhier)
{
	بنية mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		retain__expire(&peer->children);

		بنية mosquitto__retainhier *parent = peer->parent;
		אם(retain__delete_expired_msg(peer)){
			retain__clean_empty_hierarchy(parent);
		}
	}
}


فراغ retain__clean(بنية mosquitto__retainhier **retainhier)
{
	بنية mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		אם(peer->retained){
			db__msg_store_ref_dec(&peer->retained);
		}
		retain__clean(&peer->children);

		HASH_DELETE(hh, *retainhier, peer);
		mosquitto_FREE(peer);
	}
}


فراغ retain__expiry_check(فراغ)
{
	אם(db.config->retain_expiry_interval > 0 && db.now_s > next_expire_check){
		retain__expire(&db.retains);
		next_expire_check = db.now_s + db.config->retain_expiry_interval;
	}
}
