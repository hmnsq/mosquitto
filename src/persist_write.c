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

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#include <arpa/inet.h>
#endif
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "mosquitto_broker_internal.h"
#include "persist.h"
#include "util_mosq.h"


static số_nguyên persist__client_messages_save(FILE *db_fptr, struct mosquitto *context, struct mosquitto__client_msg *queue)
{
	struct P_client_msg chunk;
	struct mosquitto__client_msg *cmsg;
	số_nguyên rc;

	assert(db_fptr);
	assert(context);

	cmsg = queue;
	trong_khi(cmsg){
		nếu(!strncmp(cmsg->base_msg->data.topic, "$SYS", 4)
				&& cmsg->base_msg->ref_count <= 1
				&& cmsg->base_msg->dest_id_count == 0){

			/* This $SYS message won't have been persisted, so we can't persist
			 * this client message. */
			cmsg = cmsg->next;
			continue;
		}

		memset(&chunk, 0, sizeof(struct P_client_msg));

		chunk.F.store_id = cmsg->base_msg->data.store_id;
		chunk.F.mid = cmsg->data.mid;
		chunk.F.id_len = (uint16_t)strlen(context->id);
		chunk.F.qos = cmsg->data.qos;
		chunk.F.retain_dup = (uint8_t)((cmsg->data.retain&0x0F)<<4 | (cmsg->data.dup&0x0F));
		chunk.F.direction = (uint8_t)cmsg->data.direction;
		chunk.F.state = (uint8_t)cmsg->data.state;
		chunk.clientid = context->id;
		chunk.subscription_identifier = cmsg->data.subscription_identifier;

		rc = persist__chunk_client_msg_write_v6(db_fptr, &chunk);
		nếu(rc){
			trả_về rc;
		}

		cmsg = cmsg->next;
	}

	trả_về MOSQ_ERR_SUCCESS;
}


static số_nguyên persist__message_store_save(FILE *db_fptr)
{
	struct P_base_msg chunk;
	struct mosquitto__base_msg *base_msg, *base_msg_tmp;
	số_nguyên rc;

	assert(db_fptr);

	base_msg = db.msg_store;
	HASH_ITER(hh, db.msg_store, base_msg, base_msg_tmp){
		nếu(base_msg->ref_count < 1 || base_msg->data.topic == NULL){
			continue;
		}

		memset(&chunk, 0, sizeof(struct P_base_msg));

		nếu(!strncmp(base_msg->data.topic, "$SYS", 4)){
			nếu(base_msg->ref_count <= 1 && base_msg->dest_id_count == 0){
				/* $SYS messages that are only retained shouldn't be persisted. */
				continue;
			}
			/* Don't save $SYS messages as retained otherwise they can give
			 * misleading information when reloaded. They should still be saved
			 * because a disconnected durable client may have them in their
			 * queue. */
			chunk.F.retain = 0;
		}không_thì{
			chunk.F.retain = (uint8_t)base_msg->data.retain;
		}

		chunk.F.store_id = base_msg->data.store_id;
		chunk.F.expiry_time = base_msg->data.expiry_time;
		chunk.F.payloadlen = base_msg->data.payloadlen;
		chunk.F.source_mid = base_msg->data.source_mid;
		nếu(base_msg->data.source_id){
			chunk.F.source_id_len = (uint16_t)strlen(base_msg->data.source_id);
			chunk.source.id = base_msg->data.source_id;
		}không_thì{
			chunk.F.source_id_len = 0;
			chunk.source.id = NULL;
		}
		nếu(base_msg->data.source_username){
			chunk.F.source_username_len = (uint16_t)strlen(base_msg->data.source_username);
			chunk.source.username = base_msg->data.source_username;
		}không_thì{
			chunk.F.source_username_len = 0;
			chunk.source.username = NULL;
		}

		chunk.F.topic_len = (uint16_t)strlen(base_msg->data.topic);
		chunk.topic = base_msg->data.topic;

		nếu(base_msg->source_listener){
			chunk.F.source_port = base_msg->source_listener->port;
		}không_thì{
			chunk.F.source_port = 0;
		}
		chunk.F.qos = base_msg->data.qos;
		chunk.payload = base_msg->data.payload;
		chunk.properties = base_msg->data.properties;

		rc = persist__chunk_message_store_write_v6(db_fptr, &chunk);
		nếu(rc){
			trả_về rc;
		}
	}

	trả_về MOSQ_ERR_SUCCESS;
}


static số_nguyên persist__client_save(FILE *db_fptr)
{
	struct mosquitto *context, *ctxt_tmp;
	struct P_client chunk;
	số_nguyên rc;

	assert(db_fptr);

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		memset(&chunk, 0, sizeof(struct P_client));

		nếu(context &&
				context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE &&
#ifdef WITH_BRIDGE
				((!context->bridge && context->clean_start == false)
				|| (context->bridge && context->bridge->clean_start_local == false))
#không_thì
				context->clean_start == false
#endif
				){
			chunk.F.session_expiry_time = context->session_expiry_time;
			nếu(context->session_expiry_interval != MQTT_SESSION_EXPIRY_NEVER
					&& context->session_expiry_time == 0){

				chunk.F.session_expiry_time = context->session_expiry_interval + db.now_real_s;
			}không_thì{
				chunk.F.session_expiry_time = context->session_expiry_time;
			}
			chunk.F.session_expiry_interval = context->session_expiry_interval;
			chunk.F.last_mid = context->last_mid;
			chunk.F.id_len = (uint16_t)strlen(context->id);
			chunk.clientid = context->id;
			nếu(context->username){
				chunk.F.username_len = (uint16_t)strlen(context->username);
				chunk.username = context->username;
			}
			nếu(context->listener){
				chunk.F.listener_port = context->listener->port;
			}

			nếu(chunk.F.id_len == 0){
				/* This should never happen, but in case we have a client with
				 * zero length ID, don't persist them. */
				continue;
			}

			rc = persist__chunk_client_write_v6(db_fptr, &chunk);
			nếu(rc){
				trả_về rc;
			}

			nếu(persist__client_messages_save(db_fptr, context, context->msgs_in.inflight)){
				trả_về 1;
			}
			nếu(persist__client_messages_save(db_fptr, context, context->msgs_in.queued)){
				trả_về 1;
			}
			nếu(persist__client_messages_save(db_fptr, context, context->msgs_out.inflight)){
				trả_về 1;
			}
			nếu(persist__client_messages_save(db_fptr, context, context->msgs_out.queued)){
				trả_về 1;
			}
		}
	}

	trả_về MOSQ_ERR_SUCCESS;
}


static số_nguyên persist__subs_save(FILE *db_fptr, struct mosquitto__subhier *node, const char *topic, số_nguyên level)
{
	struct mosquitto__subhier *subhier, *subhier_tmp;
	struct mosquitto__subleaf *sub;
	struct P_sub sub_chunk;
	char *thistopic;
	size_t slen;
	số_nguyên rc;

	slen = strlen(topic) + node->topic_len + 2;
	thistopic = mosquitto_malloc(sizeof(char)*slen);
	nếu(!thistopic){
		trả_về MOSQ_ERR_NOMEM;
	}
	nếu(level > 1 || strlen(topic)){
		snprintf(thistopic, slen, "%s/%s", topic, node->topic);
	}không_thì{
		snprintf(thistopic, slen, "%s", node->topic);
	}

	sub = node->subs;
	trong_khi(sub){
		nếu(sub->context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE
				&& sub->context->clean_start == false
				&& sub->context->id){

			memset(&sub_chunk, 0, sizeof(struct P_sub));

			sub_chunk.F.identifier = sub->identifier;
			sub_chunk.F.id_len = (uint16_t)strlen(sub->context->id);
			sub_chunk.F.topic_len = (uint16_t)strlen(thistopic);
			sub_chunk.F.qos = MQTT_SUB_OPT_GET_QOS(sub->subscription_options);
			sub_chunk.F.options = sub->subscription_options & 0xFC;
			sub_chunk.clientid = sub->context->id;
			sub_chunk.topic = thistopic;

			rc = persist__chunk_sub_write_v6(db_fptr, &sub_chunk);
			nếu(rc){
				mosquitto_FREE(thistopic);
				trả_về rc;
			}
		}
		sub = sub->next;
	}

	HASH_ITER(hh, node->children, subhier, subhier_tmp){
		persist__subs_save(db_fptr, subhier, thistopic, level+1);
	}
	mosquitto_FREE(thistopic);
	trả_về MOSQ_ERR_SUCCESS;
}


static số_nguyên persist__subs_save_all(FILE *db_fptr)
{
	struct mosquitto__subhier *subhier, *subhier_tmp;

	HASH_ITER(hh, db.normal_subs, subhier, subhier_tmp){
		nếu(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	HASH_ITER(hh, db.shared_subs, subhier, subhier_tmp){
		nếu(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	trả_về MOSQ_ERR_SUCCESS;
}


static số_nguyên persist__retain_save(FILE *db_fptr, struct mosquitto__retainhier *node, số_nguyên level)
{
	struct mosquitto__retainhier *retainhier, *retainhier_tmp;
	struct P_retain retain_chunk;
	số_nguyên rc;

	nếu(node->retained && strncmp(node->retained->data.topic, "$SYS", 4)){
		memset(&retain_chunk, 0, sizeof(struct P_retain));

		/* Don't save $SYS messages. */
		retain_chunk.F.store_id = node->retained->data.store_id;
		rc = persist__chunk_retain_write_v6(db_fptr, &retain_chunk);
		nếu(rc){
			trả_về rc;
		}
	}

	HASH_ITER(hh, node->children, retainhier, retainhier_tmp){
		persist__retain_save(db_fptr, retainhier, level+1);
	}
	trả_về MOSQ_ERR_SUCCESS;
}


static số_nguyên persist__retain_save_all(FILE *db_fptr)
{
	struct mosquitto__retainhier *retainhier, *retainhier_tmp;

	HASH_ITER(hh, db.retains, retainhier, retainhier_tmp){
		nếu(retainhier->children){
			persist__retain_save(db_fptr, retainhier->children, 0);
		}
	}

	trả_về MOSQ_ERR_SUCCESS;
}

static số_nguyên persist__write_data(FILE *db_fptr, void *user_data);


static void persist__log_write_error(const char *msg)
{
	log__printf(NULL, MOSQ_LOG_ERR, "Error saving in-memory database, %s", msg);
}


số_nguyên persist__backup(bool shutdown)
{
	nếu(db.config == NULL){
		trả_về MOSQ_ERR_INVAL;
	}
	nếu(db.config->persistence == false){
		trả_về MOSQ_ERR_SUCCESS;
	}
	nếu(db.config->persistence_filepath == NULL){
		trả_về MOSQ_ERR_INVAL;
	}

	log__printf(NULL, MOSQ_LOG_INFO, "Saving in-memory database to %s.", db.config->persistence_filepath);

	trả_về mosquitto_write_file(db.config->persistence_filepath, true, &persist__write_data, &shutdown, &persist__log_write_error);
}


static số_nguyên persist__write_data(FILE *db_fptr, void *user_data)
{
	bool shutdown = *(bool *)(user_data);
	uint32_t db_version_w = htonl(MOSQ_DB_VERSION);
	uint32_t crc = 0;
	const char *err;
	struct PF_cfg cfg_chunk;
	số_nguyên rc = MOSQ_ERR_UNKNOWN;

	/* Header */
	write_e(db_fptr, magic, 15);
	write_e(db_fptr, &crc, sizeof(uint32_t));
	write_e(db_fptr, &db_version_w, sizeof(uint32_t));

	memset(&cfg_chunk, 0, sizeof(struct PF_cfg));
	cfg_chunk.last_db_id = db.last_db_id;
	cfg_chunk.shutdown = shutdown;
	cfg_chunk.dbid_size = sizeof(dbid_t);
	nếu(persist__chunk_cfg_write_v6(db_fptr, &cfg_chunk)){
		goto error;
	}

	nếu(persist__message_store_save(db_fptr)){
		goto error;
	}

	nếu(persist__client_save(db_fptr)
			|| persist__subs_save_all(db_fptr)
			|| persist__retain_save_all(db_fptr)){
		goto error;
	}
	trả_về MOSQ_ERR_SUCCESS;

error:
	err = strerror(errno);
	log__printf(NULL, MOSQ_LOG_ERR, "Error during saving in-memory database %s: %s.", db.config->persistence_filepath, err);
	nếu(db_fptr){
		fclose(db_fptr);
	}
	trả_về rc;
}


#endif
