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
	إذا(!child){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		أرجع عدم;
	}
	child->parent = parent;
	child->topic_len = len;
	إذا(len > 0){
		strncpy(child->topic, topic, (size_t)(len+1));
	}

	HASH_ADD(hh, *sibling, topic, child->topic_len, child);

	أرجع child;
}


صحيح retain__init(فراغ)
{
	بنية mosquitto__retainhier *retainhier;

	retainhier = retain__add_hier_entry(عدم, &db.retains, "", 0);
	إذا(!retainhier){
		أرجع MOSQ_ERR_NOMEM;
	}

	retainhier = retain__add_hier_entry(عدم, &db.retains, "$SYS", (uint16_t)strlen("$SYS"));
	إذا(!retainhier){
		أرجع MOSQ_ERR_NOMEM;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_persist_retain_رسالة_set(ثابت حرف *topic, uint64_t base_رسالة_id)
{
	بنية mosquitto_base_رسالة *base_رسالة;
	صحيح rc = MOSQ_ERR_UNKNOWN;
	حرف **split_topics = عدم;
	حرف *local_topic = عدم;

	إذا(topic == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh, db.رسالة_store, &base_رسالة_id, حجم(base_رسالة_id), base_رسالة);
	إذا(base_رسالة){
		إذا(sub__topic_tokenise(topic, &local_topic, &split_topics, عدم)){
			أرجع MOSQ_ERR_NOMEM;
		}

		rc = retain__store(topic, base_رسالة, split_topics, false);
		mosquitto_free(split_topics);
		mosquitto_free(local_topic);
	}

	أرجع rc;
}


BROKER_EXPORT صحيح mosquitto_persist_retain_رسالة_delete(ثابت حرف *topic)
{
	بنية mosquitto_base_رسالة base_رسالة;
	صحيح rc = MOSQ_ERR_UNKNOWN;
	حرف **split_topics = عدم;
	حرف *local_topic = عدم;

	إذا(topic == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	memset(&base_رسالة, 0, حجم(base_رسالة));
	base_رسالة.ref_عدد = 10; /* Ensure this isn't freed */

	إذا(sub__topic_tokenise(topic, &local_topic, &split_topics, عدم)){
		أرجع MOSQ_ERR_NOMEM;
	}

	/* With stored->payloadlen == 0, this means the message will be removed */
	rc = retain__store(topic, &base_رسالة, split_topics, false);
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);

	أرجع rc;
}


فراغ retain__clean_empty_hierarchy(بنية mosquitto__retainhier *retainhier)
{
	طالما(retainhier){
		إذا(retainhier->children || retainhier->retained || retainhier->parent == عدم){
			/* Entry is being used */
			أرجع;
		}وإلا{
			HASH_DELETE(hh, retainhier->parent->children, retainhier);

			بنية mosquitto__retainhier *parent = retainhier->parent;
			mosquitto_FREE(retainhier);
			retainhier = parent;
		}
	}
}


صحيح retain__store(ثابت حرف *topic, بنية mosquitto_base_رسالة *base_رسالة, حرف **split_topics, bool persist)
{
	بنية mosquitto__retainhier *retainhier;
	بنية mosquitto__retainhier *branch;
	size_t slen;

	أكد(base_رسالة);
	أكد(split_topics);

	HASH_FIND(hh, db.retains, split_topics[0], strlen(split_topics[0]), retainhier);
	إذا(retainhier == عدم){
		retainhier = retain__add_hier_entry(عدم, &db.retains, split_topics[0], (uint16_t)strlen(split_topics[0]));
		إذا(!retainhier){
			أرجع MOSQ_ERR_NOMEM;
		}
	}

	لكل(صحيح i=0; split_topics[i] != عدم; i++){
		slen = strlen(split_topics[i]);
		HASH_FIND(hh, retainhier->children, split_topics[i], slen, branch);
		إذا(branch == عدم){
			branch = retain__add_hier_entry(retainhier, &retainhier->children, split_topics[i], (uint16_t)slen);
			إذا(branch == عدم){
				أرجع MOSQ_ERR_NOMEM;
			}
		}
		retainhier = branch;
	}

#ifdef WITH_PERSISTENCE
	إذا(strncmp(topic, "$SYS", 4)){
		/* Retained messages count as a persistence change, but only if
		 * they aren't for $SYS. */
		db.persistence_changes++;
	}
#وإلا
	UNUSED(topic);
#endif

	إذا(retainhier->retained){
		إذا(retainhier->retained == base_رسالة){
			/* This may occur if multiple persistence providers are used */
			أرجع MOSQ_ERR_SUCCESS;
		}

		إذا(persist && retainhier->retained->data.topic[0] != '$' && base_رسالة->data.payloadlen == 0){
			/* Only delete if another retained message isn't replacing this one */
			plugin_persist_handle_retain_رسالة_delete(retainhier->retained);
		}
		db_رسالة_store_ref_dec(&retainhier->retained);
#ifdef WITH_SYS_شجرة
		db.retained_عدد--;
#endif
		إذا(base_رسالة->data.payloadlen == 0){
			retainhier->retained = عدم;
			retain__clean_empty_hierarchy(retainhier);
		}
	}
	إذا(base_رسالة->data.payloadlen){
		retainhier->retained = base_رسالة;
		db_رسالة_store_ref_inc(retainhier->retained);
		إذا(persist && retainhier->retained->data.topic[0] != '$'){
			plugin_persist_handle_base_رسالة_add(retainhier->retained);
			plugin_persist_handle_retain_رسالة_set(retainhier->retained);
		}
#ifdef WITH_SYS_شجرة
		db.retained_عدد++;
#endif
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن bool retain_delete_expired_رسالة(بنية mosquitto__retainhier *branch)
{
	إذا(branch->retained && branch->retained->data.expiry_time > 0 && db.now_real_s >= branch->retained->data.expiry_time){
		plugin_persist_handle_retain_رسالة_delete(branch->retained);
		db_رسالة_store_ref_dec(&branch->retained);
		branch->retained = عدم;
#ifdef WITH_SYS_شجرة
		db.retained_عدد--;
#endif
		أرجع true;
	}
	أرجع false;
}


ساكن صحيح retain__process(بنية mosquitto__retainhier *branch, بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub)
{
	صحيح rc = 0;
	uint8_t qos, sub_qos;
	uint16_t mid;
	بنية mosquitto_base_رسالة *retained;

	إذا(retain_delete_expired_رسالة(branch)){
		أرجع MOSQ_ERR_SUCCESS;
	}

	retained = branch->retained;

	rc = mosquitto_acl_check(context, retained->data.topic, retained->data.payloadlen, retained->data.payload,
			retained->data.qos, retained->data.retain, retained->data.properties, MOSQ_ACL_READ);
	إذا(rc == MOSQ_ERR_ACL_DENIED){
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا إذا(rc != MOSQ_ERR_SUCCESS){
		أرجع rc;
	}

	/* Check for original source access */
	إذا(db.config->check_retain_source && retained->origin != mosq_mo_broker && retained->data.source_id){
		بنية mosquitto retain_ctxt;
		memset(&retain_ctxt, 0, حجم(بنية mosquitto));

		retain_ctxt.id = retained->data.source_id;
		retain_ctxt.username = retained->data.source_username;
		retain_ctxt.listener = retained->source_listener;

		rc = mosquitto_acl_check(&retain_ctxt, retained->data.topic, retained->data.payloadlen, retained->data.payload,
				retained->data.qos, retained->data.retain, retained->data.properties, MOSQ_ACL_WRITE);
		إذا(rc == MOSQ_ERR_ACL_DENIED){
			أرجع MOSQ_ERR_SUCCESS;
		}وإلا إذا(rc != MOSQ_ERR_SUCCESS){
			أرجع rc;
		}
	}

	sub_qos = sub->options & 0x03;
	إذا(db.config->upgrade_outgoing_qos){
		qos = sub_qos;
	}وإلا{
		qos = retained->data.qos;
		إذا(qos > sub_qos){
			qos = sub_qos;
		}
	}
	إذا(qos > 0){
		mid = mosquitto__mid_generate(context);
	}وإلا{
		mid = 0;
	}
	أرجع db_message_أدرج_outgoing(context, 0, mid, qos, true, retained, sub->identifier, false, true);
}


ساكن صحيح retain_بحث(بنية mosquitto__retainhier *retainhier, حرف **split_topics, بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub, صحيح level)
{
	بنية mosquitto__retainhier *branch, *branch_tmp;
	صحيح flag = 0;

	إذا(!strcmp(split_topics[0], "#") && split_topics[1] == عدم){
		HASH_ITER(hh, retainhier->children, branch, branch_tmp){
			/* Set flag to indicate that we should check for retained messages
			 * on "foo" when we are subscribing to e.g. "foo/#" and then exit
			 * this function and return to an earlier retain__search().
			 */
			flag = -1;
			إذا(branch->retained){
				retain__process(branch, context, sub);
			}
			إذا(branch->children){
				retain_بحث(branch, split_topics, context, sub, level+1);
			}
		}
	}وإلا{
		إذا(!strcmp(split_topics[0], "+")){
			HASH_ITER(hh, retainhier->children, branch, branch_tmp){
				إذا(split_topics[1] != عدم){
					إذا(retain_بحث(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != عدم && !strcmp(split_topics[1], "#") && level>0)){

						إذا(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}وإلا{
					إذا(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}وإلا{
			HASH_FIND(hh, retainhier->children, split_topics[0], strlen(split_topics[0]), branch);
			إذا(branch){
				إذا(split_topics[1] != عدم){
					إذا(retain_بحث(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != عدم && !strcmp(split_topics[1], "#") && level>0)){

						إذا(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}وإلا{
					إذا(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}
	}
	أرجع flag;
}


صحيح retain__queue(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub)
{
	بنية mosquitto__retainhier *retainhier;
	حرف *local_sub;
	حرف **split_topics;
	صحيح rc;

	أكد(context);
	أكد(sub);

	إذا(!strncmp(sub->topic_filter, "$share/", strlen("$share/"))){
		أرجع MOSQ_ERR_SUCCESS;
	}

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &split_topics, عدم);
	إذا(rc){
		أرجع rc;
	}

	HASH_FIND(hh, db.retains, split_topics[0], strlen(split_topics[0]), retainhier);

	إذا(retainhier){
		retain_بحث(retainhier, split_topics, context, sub, 0);
	}
	mosquitto_FREE(local_sub);
	mosquitto_FREE(split_topics);

	أرجع MOSQ_ERR_SUCCESS;
}


فراغ retain__expire(بنية mosquitto__retainhier **retainhier)
{
	بنية mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		retain__expire(&peer->children);

		بنية mosquitto__retainhier *parent = peer->parent;
		إذا(retain_delete_expired_رسالة(peer)){
			retain__clean_empty_hierarchy(parent);
		}
	}
}


فراغ retain__clean(بنية mosquitto__retainhier **retainhier)
{
	بنية mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		إذا(peer->retained){
			db_رسالة_store_ref_dec(&peer->retained);
		}
		retain__clean(&peer->children);

		HASH_DELETE(hh, *retainhier, peer);
		mosquitto_FREE(peer);
	}
}


فراغ retain__expiry_check(فراغ)
{
	إذا(db.config->retain_expiry_interval > 0 && db.now_s > next_expire_check){
		retain__expire(&db.retains);
		next_expire_check = db.now_s + db.config->retain_expiry_interval;
	}
}
