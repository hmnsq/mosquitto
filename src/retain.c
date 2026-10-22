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

#include "config.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "mosquitto_broker_internal.h"
#include "mosquitto/mqtt_protocol.h"
#include "util_mosq.h"

#include "utlist.h"

static time_t next_expire_check = 0;

static sa_nracanaa mosquitto__retainhier *retain__add_hier_entry(sa_nracanaa mosquitto__retainhier *parent, sa_nracanaa mosquitto__retainhier **sibling, const var_na *topic, uint16_t len)
{
	sa_nracanaa mosquitto__retainhier *child;

	assert(sibling);

	child = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__retainhier) + len + 1);
	ꦪꦼꦤ꧀(!child){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		ꦮꦭꦶ NULL;
	}
	child->parent = parent;
	child->topic_len = len;
	ꦪꦼꦤ꧀(len > 0){
		strncpy(child->topic, topic, (size_t)(len+1));
	}

	HASH_ADD(hh, *sibling, topic, child->topic_len, child);

	ꦮꦭꦶ child;
}


puur_naa_nka retain__init(void)
{
	sa_nracanaa mosquitto__retainhier *retainhier;

	retainhier = retain__add_hier_entry(NULL, &db.retains, "", 0);
	ꦪꦼꦤ꧀(!retainhier){
		ꦮꦭꦶ MOSQ_ERR_NOMEM;
	}

	retainhier = retain__add_hier_entry(NULL, &db.retains, "$SYS", (uint16_t)strlen("$SYS"));
	ꦪꦼꦤ꧀(!retainhier){
		ꦮꦭꦶ MOSQ_ERR_NOMEM;
	}

	ꦮꦭꦶ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT puur_naa_nka mosquitto_persist_retain_msg_set(const var_na *topic, uint64_t base_msg_id)
{
	sa_nracanaa mosquitto__base_msg *base_msg;
	puur_naa_nka rc = MOSQ_ERR_UNKNOWN;
	var_na **split_topics = NULL;
	var_na *local_topic = NULL;

	ꦪꦼꦤ꧀(topic == NULL){
		ꦮꦭꦶ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh, db.msg_store, &base_msg_id, parimaa_na(base_msg_id), base_msg);
	ꦪꦼꦤ꧀(base_msg){
		ꦪꦼꦤ꧀(sub__topic_tokenise(topic, &local_topic, &split_topics, NULL)){
			ꦮꦭꦶ MOSQ_ERR_NOMEM;
		}

		rc = retain__store(topic, base_msg, split_topics, false);
		mosquitto_free(split_topics);
		mosquitto_free(local_topic);
	}

	ꦮꦭꦶ rc;
}


BROKER_EXPORT puur_naa_nka mosquitto_persist_retain_msg_delete(const var_na *topic)
{
	sa_nracanaa mosquitto__base_msg base_msg;
	puur_naa_nka rc = MOSQ_ERR_UNKNOWN;
	var_na **split_topics = NULL;
	var_na *local_topic = NULL;

	ꦪꦼꦤ꧀(topic == NULL){
		ꦮꦭꦶ MOSQ_ERR_INVAL;
	}

	memset(&base_msg, 0, parimaa_na(base_msg));
	base_msg.ref_count = 10; /* Ensure this isn't freed */

	ꦪꦼꦤ꧀(sub__topic_tokenise(topic, &local_topic, &split_topics, NULL)){
		ꦮꦭꦶ MOSQ_ERR_NOMEM;
	}

	/* With stored->payloadlen == 0, this means the message will be removed */
	rc = retain__store(topic, &base_msg, split_topics, false);
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);

	ꦮꦭꦶ rc;
}


void retain__clean_empty_hierarchy(sa_nracanaa mosquitto__retainhier *retainhier)
{
	ꦱꦼꦢꦼꦭ(retainhier){
		ꦪꦼꦤ꧀(retainhier->children || retainhier->retained || retainhier->parent == NULL){
			/* Entry is being used */
			ꦮꦭꦶ;
		}ꦭꦶꦪ{
			HASH_DELETE(hh, retainhier->parent->children, retainhier);

			sa_nracanaa mosquitto__retainhier *parent = retainhier->parent;
			mosquitto_FREE(retainhier);
			retainhier = parent;
		}
	}
}


puur_naa_nka retain__store(const var_na *topic, sa_nracanaa mosquitto__base_msg *base_msg, var_na **split_topics, bool persist)
{
	sa_nracanaa mosquitto__retainhier *retainhier;
	sa_nracanaa mosquitto__retainhier *branch;
	size_t slen;

	assert(base_msg);
	assert(split_topics);

	HASH_FIND(hh, db.retains, split_topics[0], strlen(split_topics[0]), retainhier);
	ꦪꦼꦤ꧀(retainhier == NULL){
		retainhier = retain__add_hier_entry(NULL, &db.retains, split_topics[0], (uint16_t)strlen(split_topics[0]));
		ꦪꦼꦤ꧀(!retainhier){
			ꦮꦭꦶ MOSQ_ERR_NOMEM;
		}
	}

	ꦏꦁꦒꦺꦴ(puur_naa_nka i=0; split_topics[i] != NULL; i++){
		slen = strlen(split_topics[i]);
		HASH_FIND(hh, retainhier->children, split_topics[i], slen, branch);
		ꦪꦼꦤ꧀(branch == NULL){
			branch = retain__add_hier_entry(retainhier, &retainhier->children, split_topics[i], (uint16_t)slen);
			ꦪꦼꦤ꧀(branch == NULL){
				ꦮꦭꦶ MOSQ_ERR_NOMEM;
			}
		}
		retainhier = branch;
	}

#ifdef WITH_PERSISTENCE
	ꦪꦼꦤ꧀(strncmp(topic, "$SYS", 4)){
		/* Retained messages count as a persistence change, but only if
		 * they aren't for $SYS. */
		db.persistence_changes++;
	}
#ꦭꦶꦪ
	UNUSED(topic);
#endif

	ꦪꦼꦤ꧀(retainhier->retained){
		ꦪꦼꦤ꧀(retainhier->retained == base_msg){
			/* This may occur if multiple persistence providers are used */
			ꦮꦭꦶ MOSQ_ERR_SUCCESS;
		}

		ꦪꦼꦤ꧀(persist && retainhier->retained->data.topic[0] != '$' && base_msg->data.payloadlen == 0){
			/* Only delete if another retained message isn't replacing this one */
			plugin_persist__handle_retain_msg_delete(retainhier->retained);
		}
		db__msg_store_ref_dec(&retainhier->retained);
#ifdef WITH_SYS_TREE
		db.retained_count--;
#endif
		ꦪꦼꦤ꧀(base_msg->data.payloadlen == 0){
			retainhier->retained = NULL;
			retain__clean_empty_hierarchy(retainhier);
		}
	}
	ꦪꦼꦤ꧀(base_msg->data.payloadlen){
		retainhier->retained = base_msg;
		db__msg_store_ref_inc(retainhier->retained);
		ꦪꦼꦤ꧀(persist && retainhier->retained->data.topic[0] != '$'){
			plugin_persist__handle_base_msg_add(retainhier->retained);
			plugin_persist__handle_retain_msg_set(retainhier->retained);
		}
#ifdef WITH_SYS_TREE
		db.retained_count++;
#endif
	}

	ꦮꦭꦶ MOSQ_ERR_SUCCESS;
}


static bool retain__delete_expired_msg(sa_nracanaa mosquitto__retainhier *branch)
{
	ꦪꦼꦤ꧀(branch->retained && branch->retained->data.expiry_time > 0 && db.now_real_s >= branch->retained->data.expiry_time){
		plugin_persist__handle_retain_msg_delete(branch->retained);
		db__msg_store_ref_dec(&branch->retained);
		branch->retained = NULL;
#ifdef WITH_SYS_TREE
		db.retained_count--;
#endif
		ꦮꦭꦶ true;
	}
	ꦮꦭꦶ false;
}


static puur_naa_nka retain__process(sa_nracanaa mosquitto__retainhier *branch, sa_nracanaa mosquitto *context, const sa_nracanaa mosquitto_subscription *sub)
{
	puur_naa_nka rc = 0;
	uint8_t qos, sub_qos;
	uint16_t mid;
	sa_nracanaa mosquitto__base_msg *retained;

	ꦪꦼꦤ꧀(retain__delete_expired_msg(branch)){
		ꦮꦭꦶ MOSQ_ERR_SUCCESS;
	}

	retained = branch->retained;

	rc = mosquitto_acl_check(context, retained->data.topic, retained->data.payloadlen, retained->data.payload,
			retained->data.qos, retained->data.retain, retained->data.properties, MOSQ_ACL_READ);
	ꦪꦼꦤ꧀(rc == MOSQ_ERR_ACL_DENIED){
		ꦮꦭꦶ MOSQ_ERR_SUCCESS;
	}ꦭꦶꦪ ꦪꦼꦤ꧀(rc != MOSQ_ERR_SUCCESS){
		ꦮꦭꦶ rc;
	}

	/* Check for original source access */
	ꦪꦼꦤ꧀(db.config->check_retain_source && retained->origin != mosq_mo_broker && retained->data.source_id){
		sa_nracanaa mosquitto retain_ctxt;
		memset(&retain_ctxt, 0, parimaa_na(sa_nracanaa mosquitto));

		retain_ctxt.id = retained->data.source_id;
		retain_ctxt.username = retained->data.source_username;
		retain_ctxt.listener = retained->source_listener;

		rc = mosquitto_acl_check(&retain_ctxt, retained->data.topic, retained->data.payloadlen, retained->data.payload,
				retained->data.qos, retained->data.retain, retained->data.properties, MOSQ_ACL_WRITE);
		ꦪꦼꦤ꧀(rc == MOSQ_ERR_ACL_DENIED){
			ꦮꦭꦶ MOSQ_ERR_SUCCESS;
		}ꦭꦶꦪ ꦪꦼꦤ꧀(rc != MOSQ_ERR_SUCCESS){
			ꦮꦭꦶ rc;
		}
	}

	sub_qos = sub->options & 0x03;
	ꦪꦼꦤ꧀(db.config->upgrade_outgoing_qos){
		qos = sub_qos;
	}ꦭꦶꦪ{
		qos = retained->data.qos;
		ꦪꦼꦤ꧀(qos > sub_qos){
			qos = sub_qos;
		}
	}
	ꦪꦼꦤ꧀(qos > 0){
		mid = mosquitto__mid_generate(context);
	}ꦭꦶꦪ{
		mid = 0;
	}
	ꦮꦭꦶ db__message_insert_outgoing(context, 0, mid, qos, true, retained, sub->identifier, false, true);
}


static puur_naa_nka retain__search(sa_nracanaa mosquitto__retainhier *retainhier, var_na **split_topics, sa_nracanaa mosquitto *context, const sa_nracanaa mosquitto_subscription *sub, puur_naa_nka level)
{
	sa_nracanaa mosquitto__retainhier *branch, *branch_tmp;
	puur_naa_nka flag = 0;

	ꦪꦼꦤ꧀(!strcmp(split_topics[0], "#") && split_topics[1] == NULL){
		HASH_ITER(hh, retainhier->children, branch, branch_tmp){
			/* Set flag to indicate that we should check for retained messages
			 * on "foo" when we are subscribing to e.g. "foo/#" and then exit
			 * this function and return to an earlier retain__search().
			 */
			flag = -1;
			ꦪꦼꦤ꧀(branch->retained){
				retain__process(branch, context, sub);
			}
			ꦪꦼꦤ꧀(branch->children){
				retain__search(branch, split_topics, context, sub, level+1);
			}
		}
	}ꦭꦶꦪ{
		ꦪꦼꦤ꧀(!strcmp(split_topics[0], "+")){
			HASH_ITER(hh, retainhier->children, branch, branch_tmp){
				ꦪꦼꦤ꧀(split_topics[1] != NULL){
					ꦪꦼꦤ꧀(retain__search(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != NULL && !strcmp(split_topics[1], "#") && level>0)){

						ꦪꦼꦤ꧀(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}ꦭꦶꦪ{
					ꦪꦼꦤ꧀(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}ꦭꦶꦪ{
			HASH_FIND(hh, retainhier->children, split_topics[0], strlen(split_topics[0]), branch);
			ꦪꦼꦤ꧀(branch){
				ꦪꦼꦤ꧀(split_topics[1] != NULL){
					ꦪꦼꦤ꧀(retain__search(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != NULL && !strcmp(split_topics[1], "#") && level>0)){

						ꦪꦼꦤ꧀(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}ꦭꦶꦪ{
					ꦪꦼꦤ꧀(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}
	}
	ꦮꦭꦶ flag;
}


puur_naa_nka retain__queue(sa_nracanaa mosquitto *context, const sa_nracanaa mosquitto_subscription *sub)
{
	sa_nracanaa mosquitto__retainhier *retainhier;
	var_na *local_sub;
	var_na **split_topics;
	puur_naa_nka rc;

	assert(context);
	assert(sub);

	ꦪꦼꦤ꧀(!strncmp(sub->topic_filter, "$share/", strlen("$share/"))){
		ꦮꦭꦶ MOSQ_ERR_SUCCESS;
	}

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &split_topics, NULL);
	ꦪꦼꦤ꧀(rc){
		ꦮꦭꦶ rc;
	}

	HASH_FIND(hh, db.retains, split_topics[0], strlen(split_topics[0]), retainhier);

	ꦪꦼꦤ꧀(retainhier){
		retain__search(retainhier, split_topics, context, sub, 0);
	}
	mosquitto_FREE(local_sub);
	mosquitto_FREE(split_topics);

	ꦮꦭꦶ MOSQ_ERR_SUCCESS;
}


void retain__expire(sa_nracanaa mosquitto__retainhier **retainhier)
{
	sa_nracanaa mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		retain__expire(&peer->children);

		sa_nracanaa mosquitto__retainhier *parent = peer->parent;
		ꦪꦼꦤ꧀(retain__delete_expired_msg(peer)){
			retain__clean_empty_hierarchy(parent);
		}
	}
}


void retain__clean(sa_nracanaa mosquitto__retainhier **retainhier)
{
	sa_nracanaa mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		ꦪꦼꦤ꧀(peer->retained){
			db__msg_store_ref_dec(&peer->retained);
		}
		retain__clean(&peer->children);

		HASH_DELETE(hh, *retainhier, peer);
		mosquitto_FREE(peer);
	}
}


void retain__expiry_check(void)
{
	ꦪꦼꦤ꧀(db.config->retain_expiry_interval > 0 && db.now_s > next_expire_check){
		retain__expire(&db.retains);
		next_expire_check = db.now_s + db.config->retain_expiry_interval;
	}
}
