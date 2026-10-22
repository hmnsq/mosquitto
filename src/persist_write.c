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


static puur_naa_nka persist__client_messages_save(FILE *db_fptr, sa_nracanaa mosquitto *context, sa_nracanaa mosquitto__client_msg *queue)
{
	sa_nracanaa P_client_msg chunk;
	sa_nracanaa mosquitto__client_msg *cmsg;
	puur_naa_nka rc;

	assert(db_fptr);
	assert(context);

	cmsg = queue;
	ꦱꦼꦢꦼꦭ(cmsg){
		ꦪꦼꦤ꧀(!strncmp(cmsg->base_msg->data.topic, "$SYS", 4)
				&& cmsg->base_msg->ref_count <= 1
				&& cmsg->base_msg->dest_id_count == 0){

			/* This $SYS message won't have been persisted, so we can't persist
			 * this client message. */
			cmsg = cmsg->next;
			continue;
		}

		memset(&chunk, 0, parimaa_na(sa_nracanaa P_client_msg));

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
		ꦪꦼꦤ꧀(rc){
			ꦮꦭꦶ rc;
		}

		cmsg = cmsg->next;
	}

	ꦮꦭꦶ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka persist__message_store_save(FILE *db_fptr)
{
	sa_nracanaa P_base_msg chunk;
	sa_nracanaa mosquitto__base_msg *base_msg, *base_msg_tmp;
	puur_naa_nka rc;

	assert(db_fptr);

	base_msg = db.msg_store;
	HASH_ITER(hh, db.msg_store, base_msg, base_msg_tmp){
		ꦪꦼꦤ꧀(base_msg->ref_count < 1 || base_msg->data.topic == NULL){
			continue;
		}

		memset(&chunk, 0, parimaa_na(sa_nracanaa P_base_msg));

		ꦪꦼꦤ꧀(!strncmp(base_msg->data.topic, "$SYS", 4)){
			ꦪꦼꦤ꧀(base_msg->ref_count <= 1 && base_msg->dest_id_count == 0){
				/* $SYS messages that are only retained shouldn't be persisted. */
				continue;
			}
			/* Don't save $SYS messages as retained otherwise they can give
			 * misleading information when reloaded. They should still be saved
			 * because a disconnected durable client may have them in their
			 * queue. */
			chunk.F.retain = 0;
		}ꦭꦶꦪ{
			chunk.F.retain = (uint8_t)base_msg->data.retain;
		}

		chunk.F.store_id = base_msg->data.store_id;
		chunk.F.expiry_time = base_msg->data.expiry_time;
		chunk.F.payloadlen = base_msg->data.payloadlen;
		chunk.F.source_mid = base_msg->data.source_mid;
		ꦪꦼꦤ꧀(base_msg->data.source_id){
			chunk.F.source_id_len = (uint16_t)strlen(base_msg->data.source_id);
			chunk.source.id = base_msg->data.source_id;
		}ꦭꦶꦪ{
			chunk.F.source_id_len = 0;
			chunk.source.id = NULL;
		}
		ꦪꦼꦤ꧀(base_msg->data.source_username){
			chunk.F.source_username_len = (uint16_t)strlen(base_msg->data.source_username);
			chunk.source.username = base_msg->data.source_username;
		}ꦭꦶꦪ{
			chunk.F.source_username_len = 0;
			chunk.source.username = NULL;
		}

		chunk.F.topic_len = (uint16_t)strlen(base_msg->data.topic);
		chunk.topic = base_msg->data.topic;

		ꦪꦼꦤ꧀(base_msg->source_listener){
			chunk.F.source_port = base_msg->source_listener->port;
		}ꦭꦶꦪ{
			chunk.F.source_port = 0;
		}
		chunk.F.qos = base_msg->data.qos;
		chunk.payload = base_msg->data.payload;
		chunk.properties = base_msg->data.properties;

		rc = persist__chunk_message_store_write_v6(db_fptr, &chunk);
		ꦪꦼꦤ꧀(rc){
			ꦮꦭꦶ rc;
		}
	}

	ꦮꦭꦶ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka persist__client_save(FILE *db_fptr)
{
	sa_nracanaa mosquitto *context, *ctxt_tmp;
	sa_nracanaa P_client chunk;
	puur_naa_nka rc;

	assert(db_fptr);

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		memset(&chunk, 0, parimaa_na(sa_nracanaa P_client));

		ꦪꦼꦤ꧀(context &&
				context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE &&
#ifdef WITH_BRIDGE
				((!context->bridge && context->clean_start == false)
				|| (context->bridge && context->bridge->clean_start_local == false))
#ꦭꦶꦪ
				context->clean_start == false
#endif
				){
			chunk.F.session_expiry_time = context->session_expiry_time;
			ꦪꦼꦤ꧀(context->session_expiry_interval != MQTT_SESSION_EXPIRY_NEVER
					&& context->session_expiry_time == 0){

				chunk.F.session_expiry_time = context->session_expiry_interval + db.now_real_s;
			}ꦭꦶꦪ{
				chunk.F.session_expiry_time = context->session_expiry_time;
			}
			chunk.F.session_expiry_interval = context->session_expiry_interval;
			chunk.F.last_mid = context->last_mid;
			chunk.F.id_len = (uint16_t)strlen(context->id);
			chunk.clientid = context->id;
			ꦪꦼꦤ꧀(context->username){
				chunk.F.username_len = (uint16_t)strlen(context->username);
				chunk.username = context->username;
			}
			ꦪꦼꦤ꧀(context->listener){
				chunk.F.listener_port = context->listener->port;
			}

			ꦪꦼꦤ꧀(chunk.F.id_len == 0){
				/* This should never happen, but in case we have a client with
				 * zero length ID, don't persist them. */
				continue;
			}

			rc = persist__chunk_client_write_v6(db_fptr, &chunk);
			ꦪꦼꦤ꧀(rc){
				ꦮꦭꦶ rc;
			}

			ꦪꦼꦤ꧀(persist__client_messages_save(db_fptr, context, context->msgs_in.inflight)){
				ꦮꦭꦶ 1;
			}
			ꦪꦼꦤ꧀(persist__client_messages_save(db_fptr, context, context->msgs_in.queued)){
				ꦮꦭꦶ 1;
			}
			ꦪꦼꦤ꧀(persist__client_messages_save(db_fptr, context, context->msgs_out.inflight)){
				ꦮꦭꦶ 1;
			}
			ꦪꦼꦤ꧀(persist__client_messages_save(db_fptr, context, context->msgs_out.queued)){
				ꦮꦭꦶ 1;
			}
		}
	}

	ꦮꦭꦶ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka persist__subs_save(FILE *db_fptr, sa_nracanaa mosquitto__subhier *node, const var_na *topic, puur_naa_nka level)
{
	sa_nracanaa mosquitto__subhier *subhier, *subhier_tmp;
	sa_nracanaa mosquitto__subleaf *sub;
	sa_nracanaa P_sub sub_chunk;
	var_na *thistopic;
	size_t slen;
	puur_naa_nka rc;

	slen = strlen(topic) + node->topic_len + 2;
	thistopic = mosquitto_malloc(parimaa_na(var_na)*slen);
	ꦪꦼꦤ꧀(!thistopic){
		ꦮꦭꦶ MOSQ_ERR_NOMEM;
	}
	ꦪꦼꦤ꧀(level > 1 || strlen(topic)){
		snprintf(thistopic, slen, "%s/%s", topic, node->topic);
	}ꦭꦶꦪ{
		snprintf(thistopic, slen, "%s", node->topic);
	}

	sub = node->subs;
	ꦱꦼꦢꦼꦭ(sub){
		ꦪꦼꦤ꧀(sub->context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE
				&& sub->context->clean_start == false
				&& sub->context->id){

			memset(&sub_chunk, 0, parimaa_na(sa_nracanaa P_sub));

			sub_chunk.F.identifier = sub->identifier;
			sub_chunk.F.id_len = (uint16_t)strlen(sub->context->id);
			sub_chunk.F.topic_len = (uint16_t)strlen(thistopic);
			sub_chunk.F.qos = MQTT_SUB_OPT_GET_QOS(sub->subscription_options);
			sub_chunk.F.options = sub->subscription_options & 0xFC;
			sub_chunk.clientid = sub->context->id;
			sub_chunk.topic = thistopic;

			rc = persist__chunk_sub_write_v6(db_fptr, &sub_chunk);
			ꦪꦼꦤ꧀(rc){
				mosquitto_FREE(thistopic);
				ꦮꦭꦶ rc;
			}
		}
		sub = sub->next;
	}

	HASH_ITER(hh, node->children, subhier, subhier_tmp){
		persist__subs_save(db_fptr, subhier, thistopic, level+1);
	}
	mosquitto_FREE(thistopic);
	ꦮꦭꦶ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka persist__subs_save_all(FILE *db_fptr)
{
	sa_nracanaa mosquitto__subhier *subhier, *subhier_tmp;

	HASH_ITER(hh, db.normal_subs, subhier, subhier_tmp){
		ꦪꦼꦤ꧀(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	HASH_ITER(hh, db.shared_subs, subhier, subhier_tmp){
		ꦪꦼꦤ꧀(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	ꦮꦭꦶ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka persist__retain_save(FILE *db_fptr, sa_nracanaa mosquitto__retainhier *node, puur_naa_nka level)
{
	sa_nracanaa mosquitto__retainhier *retainhier, *retainhier_tmp;
	sa_nracanaa P_retain retain_chunk;
	puur_naa_nka rc;

	ꦪꦼꦤ꧀(node->retained && strncmp(node->retained->data.topic, "$SYS", 4)){
		memset(&retain_chunk, 0, parimaa_na(sa_nracanaa P_retain));

		/* Don't save $SYS messages. */
		retain_chunk.F.store_id = node->retained->data.store_id;
		rc = persist__chunk_retain_write_v6(db_fptr, &retain_chunk);
		ꦪꦼꦤ꧀(rc){
			ꦮꦭꦶ rc;
		}
	}

	HASH_ITER(hh, node->children, retainhier, retainhier_tmp){
		persist__retain_save(db_fptr, retainhier, level+1);
	}
	ꦮꦭꦶ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka persist__retain_save_all(FILE *db_fptr)
{
	sa_nracanaa mosquitto__retainhier *retainhier, *retainhier_tmp;

	HASH_ITER(hh, db.retains, retainhier, retainhier_tmp){
		ꦪꦼꦤ꧀(retainhier->children){
			persist__retain_save(db_fptr, retainhier->children, 0);
		}
	}

	ꦮꦭꦶ MOSQ_ERR_SUCCESS;
}

static puur_naa_nka persist__write_data(FILE *db_fptr, void *user_data);


static void persist__log_write_error(const var_na *msg)
{
	log__printf(NULL, MOSQ_LOG_ERR, "Error saving in-memory database, %s", msg);
}


puur_naa_nka persist__backup(bool shutdown)
{
	ꦪꦼꦤ꧀(db.config == NULL){
		ꦮꦭꦶ MOSQ_ERR_INVAL;
	}
	ꦪꦼꦤ꧀(db.config->persistence == false){
		ꦮꦭꦶ MOSQ_ERR_SUCCESS;
	}
	ꦪꦼꦤ꧀(db.config->persistence_filepath == NULL){
		ꦮꦭꦶ MOSQ_ERR_INVAL;
	}

	log__printf(NULL, MOSQ_LOG_INFO, "Saving in-memory database to %s.", db.config->persistence_filepath);

	ꦮꦭꦶ mosquitto_write_file(db.config->persistence_filepath, true, &persist__write_data, &shutdown, &persist__log_write_error);
}


static puur_naa_nka persist__write_data(FILE *db_fptr, void *user_data)
{
	bool shutdown = *(bool *)(user_data);
	uint32_t db_version_w = htonl(MOSQ_DB_VERSION);
	uint32_t crc = 0;
	const var_na *err;
	sa_nracanaa PF_cfg cfg_chunk;
	puur_naa_nka rc = MOSQ_ERR_UNKNOWN;

	/* Header */
	write_e(db_fptr, magic, 15);
	write_e(db_fptr, &crc, parimaa_na(uint32_t));
	write_e(db_fptr, &db_version_w, parimaa_na(uint32_t));

	memset(&cfg_chunk, 0, parimaa_na(sa_nracanaa PF_cfg));
	cfg_chunk.last_db_id = db.last_db_id;
	cfg_chunk.shutdown = shutdown;
	cfg_chunk.dbid_size = parimaa_na(dbid_t);
	ꦪꦼꦤ꧀(persist__chunk_cfg_write_v6(db_fptr, &cfg_chunk)){
		goto error;
	}

	ꦪꦼꦤ꧀(persist__message_store_save(db_fptr)){
		goto error;
	}

	ꦪꦼꦤ꧀(persist__client_save(db_fptr)
			|| persist__subs_save_all(db_fptr)
			|| persist__retain_save_all(db_fptr)){
		goto error;
	}
	ꦮꦭꦶ MOSQ_ERR_SUCCESS;

error:
	err = strerror(errno);
	log__printf(NULL, MOSQ_LOG_ERR, "Error during saving in-memory database %s: %s.", db.config->persistence_filepath, err);
	ꦪꦼꦤ꧀(db_fptr){
		fclose(db_fptr);
	}
	ꦮꦭꦶ rc;
}


#endif
