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

static struct mosquitto__retainhier *retain__add_hier_entry(struct mosquitto__retainhier *parent, struct mosquitto__retainhier **sibling, const char *topic, uint16_t len)
{
	struct mosquitto__retainhier *child;

	assert(sibling);

	child = mosquitto_calloc(1, sizeof(struct mosquitto__retainhier) + len + 1);
	якщо(!child){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		повернути NULL;
	}
	child->parent = parent;
	child->topic_len = len;
	якщо(len > 0){
		strncpy(child->topic, topic, (size_t)(len+1));
	}

	HASH_ADD(hh, *sibling, topic, child->topic_len, child);

	повернути child;
}


ціле retain__init(void)
{
	struct mosquitto__retainhier *retainhier;

	retainhier = retain__add_hier_entry(NULL, &db.retains, "", 0);
	якщо(!retainhier){
		повернути MOSQ_ERR_NOMEM;
	}

	retainhier = retain__add_hier_entry(NULL, &db.retains, "$SYS", (uint16_t)strlen("$SYS"));
	якщо(!retainhier){
		повернути MOSQ_ERR_NOMEM;
	}

	повернути MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ціле mosquitto_persist_retain_msg_set(const char *topic, uint64_t base_msg_id)
{
	struct mosquitto__base_msg *base_msg;
	ціле rc = MOSQ_ERR_UNKNOWN;
	char **split_topics = NULL;
	char *local_topic = NULL;

	якщо(topic == NULL){
		повернути MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh, db.msg_store, &base_msg_id, sizeof(base_msg_id), base_msg);
	якщо(base_msg){
		якщо(sub__topic_tokenise(topic, &local_topic, &split_topics, NULL)){
			повернути MOSQ_ERR_NOMEM;
		}

		rc = retain__store(topic, base_msg, split_topics, false);
		mosquitto_free(split_topics);
		mosquitto_free(local_topic);
	}

	повернути rc;
}


BROKER_EXPORT ціле mosquitto_persist_retain_msg_delete(const char *topic)
{
	struct mosquitto__base_msg base_msg;
	ціле rc = MOSQ_ERR_UNKNOWN;
	char **split_topics = NULL;
	char *local_topic = NULL;

	якщо(topic == NULL){
		повернути MOSQ_ERR_INVAL;
	}

	memset(&base_msg, 0, sizeof(base_msg));
	base_msg.ref_count = 10; /* Ensure this isn't freed */

	якщо(sub__topic_tokenise(topic, &local_topic, &split_topics, NULL)){
		повернути MOSQ_ERR_NOMEM;
	}

	/* With stored->payloadlen == 0, this means the message will be removed */
	rc = retain__store(topic, &base_msg, split_topics, false);
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);

	повернути rc;
}


void retain__clean_empty_hierarchy(struct mosquitto__retainhier *retainhier)
{
	поки(retainhier){
		якщо(retainhier->children || retainhier->retained || retainhier->parent == NULL){
			/* Entry is being used */
			повернути;
		}інакше{
			HASH_DELETE(hh, retainhier->parent->children, retainhier);

			struct mosquitto__retainhier *parent = retainhier->parent;
			mosquitto_FREE(retainhier);
			retainhier = parent;
		}
	}
}


ціле retain__store(const char *topic, struct mosquitto__base_msg *base_msg, char **split_topics, bool persist)
{
	struct mosquitto__retainhier *retainhier;
	struct mosquitto__retainhier *branch;
	size_t slen;

	assert(base_msg);
	assert(split_topics);

	HASH_FIND(hh, db.retains, split_topics[0], strlen(split_topics[0]), retainhier);
	якщо(retainhier == NULL){
		retainhier = retain__add_hier_entry(NULL, &db.retains, split_topics[0], (uint16_t)strlen(split_topics[0]));
		якщо(!retainhier){
			повернути MOSQ_ERR_NOMEM;
		}
	}

	для(ціле i=0; split_topics[i] != NULL; i++){
		slen = strlen(split_topics[i]);
		HASH_FIND(hh, retainhier->children, split_topics[i], slen, branch);
		якщо(branch == NULL){
			branch = retain__add_hier_entry(retainhier, &retainhier->children, split_topics[i], (uint16_t)slen);
			якщо(branch == NULL){
				повернути MOSQ_ERR_NOMEM;
			}
		}
		retainhier = branch;
	}

#ifdef WITH_PERSISTENCE
	якщо(strncmp(topic, "$SYS", 4)){
		/* Retained messages count as a persistence change, but only if
		 * they aren't for $SYS. */
		db.persistence_changes++;
	}
#інакше
	UNUSED(topic);
#endif

	якщо(retainhier->retained){
		якщо(retainhier->retained == base_msg){
			/* This may occur if multiple persistence providers are used */
			повернути MOSQ_ERR_SUCCESS;
		}

		якщо(persist && retainhier->retained->data.topic[0] != '$' && base_msg->data.payloadlen == 0){
			/* Only delete if another retained message isn't replacing this one */
			plugin_persist__handle_retain_msg_delete(retainhier->retained);
		}
		db__msg_store_ref_dec(&retainhier->retained);
#ifdef WITH_SYS_TREE
		db.retained_count--;
#endif
		якщо(base_msg->data.payloadlen == 0){
			retainhier->retained = NULL;
			retain__clean_empty_hierarchy(retainhier);
		}
	}
	якщо(base_msg->data.payloadlen){
		retainhier->retained = base_msg;
		db__msg_store_ref_inc(retainhier->retained);
		якщо(persist && retainhier->retained->data.topic[0] != '$'){
			plugin_persist__handle_base_msg_add(retainhier->retained);
			plugin_persist__handle_retain_msg_set(retainhier->retained);
		}
#ifdef WITH_SYS_TREE
		db.retained_count++;
#endif
	}

	повернути MOSQ_ERR_SUCCESS;
}


static bool retain__delete_expired_msg(struct mosquitto__retainhier *branch)
{
	якщо(branch->retained && branch->retained->data.expiry_time > 0 && db.now_real_s >= branch->retained->data.expiry_time){
		plugin_persist__handle_retain_msg_delete(branch->retained);
		db__msg_store_ref_dec(&branch->retained);
		branch->retained = NULL;
#ifdef WITH_SYS_TREE
		db.retained_count--;
#endif
		повернути true;
	}
	повернути false;
}


static ціле retain__process(struct mosquitto__retainhier *branch, struct mosquitto *context, const struct mosquitto_subscription *sub)
{
	ціле rc = 0;
	uint8_t qos, sub_qos;
	uint16_t mid;
	struct mosquitto__base_msg *retained;

	якщо(retain__delete_expired_msg(branch)){
		повернути MOSQ_ERR_SUCCESS;
	}

	retained = branch->retained;

	rc = mosquitto_acl_check(context, retained->data.topic, retained->data.payloadlen, retained->data.payload,
			retained->data.qos, retained->data.retain, retained->data.properties, MOSQ_ACL_READ);
	якщо(rc == MOSQ_ERR_ACL_DENIED){
		повернути MOSQ_ERR_SUCCESS;
	}інакше якщо(rc != MOSQ_ERR_SUCCESS){
		повернути rc;
	}

	/* Check for original source access */
	якщо(db.config->check_retain_source && retained->origin != mosq_mo_broker && retained->data.source_id){
		struct mosquitto retain_ctxt;
		memset(&retain_ctxt, 0, sizeof(struct mosquitto));

		retain_ctxt.id = retained->data.source_id;
		retain_ctxt.username = retained->data.source_username;
		retain_ctxt.listener = retained->source_listener;

		rc = mosquitto_acl_check(&retain_ctxt, retained->data.topic, retained->data.payloadlen, retained->data.payload,
				retained->data.qos, retained->data.retain, retained->data.properties, MOSQ_ACL_WRITE);
		якщо(rc == MOSQ_ERR_ACL_DENIED){
			повернути MOSQ_ERR_SUCCESS;
		}інакше якщо(rc != MOSQ_ERR_SUCCESS){
			повернути rc;
		}
	}

	sub_qos = sub->options & 0x03;
	якщо(db.config->upgrade_outgoing_qos){
		qos = sub_qos;
	}інакше{
		qos = retained->data.qos;
		якщо(qos > sub_qos){
			qos = sub_qos;
		}
	}
	якщо(qos > 0){
		mid = mosquitto__mid_generate(context);
	}інакше{
		mid = 0;
	}
	повернути db__message_insert_outgoing(context, 0, mid, qos, true, retained, sub->identifier, false, true);
}


static ціле retain__search(struct mosquitto__retainhier *retainhier, char **split_topics, struct mosquitto *context, const struct mosquitto_subscription *sub, ціле level)
{
	struct mosquitto__retainhier *branch, *branch_tmp;
	ціле flag = 0;

	якщо(!strcmp(split_topics[0], "#") && split_topics[1] == NULL){
		HASH_ITER(hh, retainhier->children, branch, branch_tmp){
			/* Set flag to indicate that we should check for retained messages
			 * on "foo" when we are subscribing to e.g. "foo/#" and then exit
			 * this function and return to an earlier retain__search().
			 */
			flag = -1;
			якщо(branch->retained){
				retain__process(branch, context, sub);
			}
			якщо(branch->children){
				retain__search(branch, split_topics, context, sub, level+1);
			}
		}
	}інакше{
		якщо(!strcmp(split_topics[0], "+")){
			HASH_ITER(hh, retainhier->children, branch, branch_tmp){
				якщо(split_topics[1] != NULL){
					якщо(retain__search(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != NULL && !strcmp(split_topics[1], "#") && level>0)){

						якщо(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}інакше{
					якщо(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}інакше{
			HASH_FIND(hh, retainhier->children, split_topics[0], strlen(split_topics[0]), branch);
			якщо(branch){
				якщо(split_topics[1] != NULL){
					якщо(retain__search(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != NULL && !strcmp(split_topics[1], "#") && level>0)){

						якщо(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}інакше{
					якщо(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}
	}
	повернути flag;
}


ціле retain__queue(struct mosquitto *context, const struct mosquitto_subscription *sub)
{
	struct mosquitto__retainhier *retainhier;
	char *local_sub;
	char **split_topics;
	ціле rc;

	assert(context);
	assert(sub);

	якщо(!strncmp(sub->topic_filter, "$share/", strlen("$share/"))){
		повернути MOSQ_ERR_SUCCESS;
	}

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &split_topics, NULL);
	якщо(rc){
		повернути rc;
	}

	HASH_FIND(hh, db.retains, split_topics[0], strlen(split_topics[0]), retainhier);

	якщо(retainhier){
		retain__search(retainhier, split_topics, context, sub, 0);
	}
	mosquitto_FREE(local_sub);
	mosquitto_FREE(split_topics);

	повернути MOSQ_ERR_SUCCESS;
}


void retain__expire(struct mosquitto__retainhier **retainhier)
{
	struct mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		retain__expire(&peer->children);

		struct mosquitto__retainhier *parent = peer->parent;
		якщо(retain__delete_expired_msg(peer)){
			retain__clean_empty_hierarchy(parent);
		}
	}
}


void retain__clean(struct mosquitto__retainhier **retainhier)
{
	struct mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		якщо(peer->retained){
			db__msg_store_ref_dec(&peer->retained);
		}
		retain__clean(&peer->children);

		HASH_DELETE(hh, *retainhier, peer);
		mosquitto_FREE(peer);
	}
}


void retain__expiry_check(void)
{
	якщо(db.config->retain_expiry_interval > 0 && db.now_s > next_expire_check){
		retain__expire(&db.retains);
		next_expire_check = db.now_s + db.config->retain_expiry_interval;
	}
}
