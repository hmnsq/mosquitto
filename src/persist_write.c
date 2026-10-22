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

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#ضمن <arpa/inet.h>
#endif
#ضمن <assert.h>
#ضمن <errno.h>
#ضمن <fcntl.h>
#ضمن <stdio.h>
#ضمن <string.h>
#ضمن <sys/stat.h>
#ضمن <time.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "persist.h"
#ضمن "util_mosq.h"


ساكن صحيح persist__client_messages_save(FILE *db_fptr, بنية mosquitto *context, بنية mosquitto__client_msg *queue)
{
	بنية P_client_msg chunk;
	بنية mosquitto__client_msg *cmsg;
	صحيح rc;

	أكد(db_fptr);
	أكد(context);

	cmsg = queue;
	طالما(cmsg){
		𐤀𐤌(!strncmp(cmsg->base_msg->data.topic, "$SYS", 4)
				&& cmsg->base_msg->ref_count <= 1
				&& cmsg->base_msg->dest_id_count == 0){

			/* This $SYS message won't have been persisted, so we can't persist
			 * this client message. */
			cmsg = cmsg->next;
			استمر;
		}

		memset(&chunk, 0, حجم(بنية P_client_msg));

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
		𐤀𐤌(rc){
			أرجع rc;
		}

		cmsg = cmsg->next;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح persist__message_store_save(FILE *db_fptr)
{
	بنية P_base_msg chunk;
	بنية mosquitto__base_msg *base_msg, *base_msg_tmp;
	صحيح rc;

	أكد(db_fptr);

	base_msg = db.msg_store;
	HASH_ITER(hh, db.msg_store, base_msg, base_msg_tmp){
		𐤀𐤌(base_msg->ref_count < 1 || base_msg->data.topic == عدم){
			استمر;
		}

		memset(&chunk, 0, حجم(بنية P_base_msg));

		𐤀𐤌(!strncmp(base_msg->data.topic, "$SYS", 4)){
			𐤀𐤌(base_msg->ref_count <= 1 && base_msg->dest_id_count == 0){
				/* $SYS messages that are only retained shouldn't be persisted. */
				استمر;
			}
			/* Don't save $SYS messages as retained otherwise they can give
			 * misleading information when reloaded. They should still be saved
			 * because a disconnected durable client may have them in their
			 * queue. */
			chunk.F.retain = 0;
		}وإلا{
			chunk.F.retain = (uint8_t)base_msg->data.retain;
		}

		chunk.F.store_id = base_msg->data.store_id;
		chunk.F.expiry_time = base_msg->data.expiry_time;
		chunk.F.payloadlen = base_msg->data.payloadlen;
		chunk.F.source_mid = base_msg->data.source_mid;
		𐤀𐤌(base_msg->data.source_id){
			chunk.F.source_id_len = (uint16_t)strlen(base_msg->data.source_id);
			chunk.source.id = base_msg->data.source_id;
		}وإلا{
			chunk.F.source_id_len = 0;
			chunk.source.id = عدم;
		}
		𐤀𐤌(base_msg->data.source_username){
			chunk.F.source_username_len = (uint16_t)strlen(base_msg->data.source_username);
			chunk.source.username = base_msg->data.source_username;
		}وإلا{
			chunk.F.source_username_len = 0;
			chunk.source.username = عدم;
		}

		chunk.F.topic_len = (uint16_t)strlen(base_msg->data.topic);
		chunk.topic = base_msg->data.topic;

		𐤀𐤌(base_msg->source_listener){
			chunk.F.source_port = base_msg->source_listener->port;
		}وإلا{
			chunk.F.source_port = 0;
		}
		chunk.F.qos = base_msg->data.qos;
		chunk.payload = base_msg->data.payload;
		chunk.properties = base_msg->data.properties;

		rc = persist__chunk_message_store_write_v6(db_fptr, &chunk);
		𐤀𐤌(rc){
			أرجع rc;
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح persist__client_save(FILE *db_fptr)
{
	بنية mosquitto *context, *ctxt_tmp;
	بنية P_client chunk;
	صحيح rc;

	أكد(db_fptr);

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		memset(&chunk, 0, حجم(بنية P_client));

		𐤀𐤌(context &&
				context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE &&
#ifdef WITH_BRIDGE
				((!context->bridge && context->clean_start == false)
				|| (context->bridge && context->bridge->clean_start_local == false))
#وإلا
				context->clean_start == false
#endif
				){
			chunk.F.session_expiry_time = context->session_expiry_time;
			𐤀𐤌(context->session_expiry_interval != MQTT_SESSION_EXPIRY_NEVER
					&& context->session_expiry_time == 0){

				chunk.F.session_expiry_time = context->session_expiry_interval + db.now_real_s;
			}وإلا{
				chunk.F.session_expiry_time = context->session_expiry_time;
			}
			chunk.F.session_expiry_interval = context->session_expiry_interval;
			chunk.F.last_mid = context->last_mid;
			chunk.F.id_len = (uint16_t)strlen(context->id);
			chunk.clientid = context->id;
			𐤀𐤌(context->username){
				chunk.F.username_len = (uint16_t)strlen(context->username);
				chunk.username = context->username;
			}
			𐤀𐤌(context->listener){
				chunk.F.listener_port = context->listener->port;
			}

			𐤀𐤌(chunk.F.id_len == 0){
				/* This should never happen, but in case we have a client with
				 * zero length ID, don't persist them. */
				استمر;
			}

			rc = persist__chunk_client_write_v6(db_fptr, &chunk);
			𐤀𐤌(rc){
				أرجع rc;
			}

			𐤀𐤌(persist__client_messages_save(db_fptr, context, context->msgs_in.inflight)){
				أرجع 1;
			}
			𐤀𐤌(persist__client_messages_save(db_fptr, context, context->msgs_in.queued)){
				أرجع 1;
			}
			𐤀𐤌(persist__client_messages_save(db_fptr, context, context->msgs_out.inflight)){
				أرجع 1;
			}
			𐤀𐤌(persist__client_messages_save(db_fptr, context, context->msgs_out.queued)){
				أرجع 1;
			}
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح persist__subs_save(FILE *db_fptr, بنية mosquitto__subhier *node, ثابت حرف *topic, صحيح level)
{
	بنية mosquitto__subhier *subhier, *subhier_tmp;
	بنية mosquitto__subleaf *sub;
	بنية P_sub sub_chunk;
	حرف *thistopic;
	size_t slen;
	صحيح rc;

	slen = strlen(topic) + node->topic_len + 2;
	thistopic = mosquitto_malloc(حجم(حرف)*slen);
	𐤀𐤌(!thistopic){
		أرجع MOSQ_ERR_NOMEM;
	}
	𐤀𐤌(level > 1 || strlen(topic)){
		snprintf(thistopic, slen, "%s/%s", topic, node->topic);
	}وإلا{
		snprintf(thistopic, slen, "%s", node->topic);
	}

	sub = node->subs;
	طالما(sub){
		𐤀𐤌(sub->context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE
				&& sub->context->clean_start == false
				&& sub->context->id){

			memset(&sub_chunk, 0, حجم(بنية P_sub));

			sub_chunk.F.identifier = sub->identifier;
			sub_chunk.F.id_len = (uint16_t)strlen(sub->context->id);
			sub_chunk.F.topic_len = (uint16_t)strlen(thistopic);
			sub_chunk.F.qos = MQTT_SUB_OPT_GET_QOS(sub->subscription_options);
			sub_chunk.F.options = sub->subscription_options & 0xFC;
			sub_chunk.clientid = sub->context->id;
			sub_chunk.topic = thistopic;

			rc = persist__chunk_sub_write_v6(db_fptr, &sub_chunk);
			𐤀𐤌(rc){
				mosquitto_FREE(thistopic);
				أرجع rc;
			}
		}
		sub = sub->next;
	}

	HASH_ITER(hh, node->children, subhier, subhier_tmp){
		persist__subs_save(db_fptr, subhier, thistopic, level+1);
	}
	mosquitto_FREE(thistopic);
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح persist__subs_save_all(FILE *db_fptr)
{
	بنية mosquitto__subhier *subhier, *subhier_tmp;

	HASH_ITER(hh, db.normal_subs, subhier, subhier_tmp){
		𐤀𐤌(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	HASH_ITER(hh, db.shared_subs, subhier, subhier_tmp){
		𐤀𐤌(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح persist__retain_save(FILE *db_fptr, بنية mosquitto__retainhier *node, صحيح level)
{
	بنية mosquitto__retainhier *retainhier, *retainhier_tmp;
	بنية P_retain retain_chunk;
	صحيح rc;

	𐤀𐤌(node->retained && strncmp(node->retained->data.topic, "$SYS", 4)){
		memset(&retain_chunk, 0, حجم(بنية P_retain));

		/* Don't save $SYS messages. */
		retain_chunk.F.store_id = node->retained->data.store_id;
		rc = persist__chunk_retain_write_v6(db_fptr, &retain_chunk);
		𐤀𐤌(rc){
			أرجع rc;
		}
	}

	HASH_ITER(hh, node->children, retainhier, retainhier_tmp){
		persist__retain_save(db_fptr, retainhier, level+1);
	}
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح persist__retain_save_all(FILE *db_fptr)
{
	بنية mosquitto__retainhier *retainhier, *retainhier_tmp;

	HASH_ITER(hh, db.retains, retainhier, retainhier_tmp){
		𐤀𐤌(retainhier->children){
			persist__retain_save(db_fptr, retainhier->children, 0);
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}

ساكن صحيح persist__write_data(FILE *db_fptr, فراغ *user_data);


ساكن فراغ persist__log_write_error(ثابت حرف *msg)
{
	log__printf(عدم, MOSQ_LOG_ERR, "Error saving in-memory database, %s", msg);
}


صحيح persist__backup(bool shutdown)
{
	𐤀𐤌(db.config == عدم){
		أرجع MOSQ_ERR_INVAL;
	}
	𐤀𐤌(db.config->persistence == false){
		أرجع MOSQ_ERR_SUCCESS;
	}
	𐤀𐤌(db.config->persistence_filepath == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	log__printf(عدم, MOSQ_LOG_INFO, "Saving in-memory database to %s.", db.config->persistence_filepath);

	أرجع mosquitto_write_file(db.config->persistence_filepath, true, &persist__write_data, &shutdown, &persist__log_write_error);
}


ساكن صحيح persist__write_data(FILE *db_fptr, فراغ *user_data)
{
	bool shutdown = *(bool *)(user_data);
	uint32_t db_version_w = htonl(MOSQ_DB_VERSION);
	uint32_t crc = 0;
	ثابت حرف *err;
	بنية PF_cfg cfg_chunk;
	صحيح rc = MOSQ_ERR_UNKNOWN;

	/* Header */
	write_e(db_fptr, magic, 15);
	write_e(db_fptr, &crc, حجم(uint32_t));
	write_e(db_fptr, &db_version_w, حجم(uint32_t));

	memset(&cfg_chunk, 0, حجم(بنية PF_cfg));
	cfg_chunk.last_db_id = db.last_db_id;
	cfg_chunk.shutdown = shutdown;
	cfg_chunk.dbid_size = حجم(dbid_t);
	𐤀𐤌(persist__chunk_cfg_write_v6(db_fptr, &cfg_chunk)){
		اذهب error;
	}

	𐤀𐤌(persist__message_store_save(db_fptr)){
		اذهب error;
	}

	𐤀𐤌(persist__client_save(db_fptr)
			|| persist__subs_save_all(db_fptr)
			|| persist__retain_save_all(db_fptr)){
		اذهب error;
	}
	أرجع MOSQ_ERR_SUCCESS;

error:
	err = strerror(errno);
	log__printf(عدم, MOSQ_LOG_ERR, "Error during saving in-memory database %s: %s.", db.config->persistence_filepath, err);
	𐤀𐤌(db_fptr){
		fclose(db_fptr);
	}
	أرجع rc;
}


#endif
