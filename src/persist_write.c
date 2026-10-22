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

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#شامل <arpa/inet.h>
#endif
#شامل <assert.h>
#شامل <errno.h>
#شامل <fcntl.h>
#شامل <stdio.h>
#شامل <string.h>
#شامل <sys/stat.h>
#شامل <time.h>

#شامل "mosquitto_broker_internal.h"
#شامل "persist.h"
#شامل "util_mosq.h"


ایستا صحیح persist__client_messages_save(FILE *db_fptr, ساختار mosquitto *context, ساختار mosquitto_client_پیام *queue)
{
	ساختار P_client_پیام chunk;
	ساختار mosquitto_client_پیام *cmsg;
	صحیح rc;

	أكد(db_fptr);
	أكد(context);

	cmsg = queue;
	وقتی(cmsg){
		اگر(!strncmp(cmsg->base_پیام->data.topic, "$SYS", 4)
				&& cmsg->base_پیام->ref_شمار <= 1
				&& cmsg->base_پیام->dest_id_شمار == 0){

			/* This $SYS message won't have been persisted, so we can't persist
			 * this client message. */
			cmsg = cmsg->next;
			ادامه;
		}

		memset(&chunk, 0, اندازه(ساختار P_client_پیام));

		chunk.F.store_id = cmsg->base_پیام->data.store_id;
		chunk.F.mid = cmsg->data.mid;
		chunk.F.id_len = (uint16_t)strlen(context->id);
		chunk.F.qos = cmsg->data.qos;
		chunk.F.retain_dup = (uint8_t)((cmsg->data.retain&0x0F)<<4 | (cmsg->data.dup&0x0F));
		chunk.F.direction = (uint8_t)cmsg->data.direction;
		chunk.F.state = (uint8_t)cmsg->data.state;
		chunk.clientid = context->id;
		chunk.subscription_identifier = cmsg->data.subscription_identifier;

		rc = persist_chunk_client_پیام_write_v6(db_fptr, &chunk);
		اگر(rc){
			بازگشت rc;
		}

		cmsg = cmsg->next;
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح persist__message_store_save(FILE *db_fptr)
{
	ساختار P_base_پیام chunk;
	ساختار mosquitto_base_پیام *base_پیام, *base_پیام_tmp;
	صحیح rc;

	أكد(db_fptr);

	base_پیام = db.پیام_store;
	HASH_ITER(hh, db.پیام_store, base_پیام, base_پیام_tmp){
		اگر(base_پیام->ref_شمار < 1 || base_پیام->data.topic == پوچ){
			ادامه;
		}

		memset(&chunk, 0, اندازه(ساختار P_base_پیام));

		اگر(!strncmp(base_پیام->data.topic, "$SYS", 4)){
			اگر(base_پیام->ref_شمار <= 1 && base_پیام->dest_id_شمار == 0){
				/* $SYS messages that are only retained shouldn't be persisted. */
				ادامه;
			}
			/* Don't save $SYS messages as retained otherwise they can give
			 * misleading information when reloaded. They should still be saved
			 * because a disconnected durable client may have them in their
			 * queue. */
			chunk.F.retain = 0;
		}وگرنه{
			chunk.F.retain = (uint8_t)base_پیام->data.retain;
		}

		chunk.F.store_id = base_پیام->data.store_id;
		chunk.F.expiry_time = base_پیام->data.expiry_time;
		chunk.F.payloadlen = base_پیام->data.payloadlen;
		chunk.F.source_mid = base_پیام->data.source_mid;
		اگر(base_پیام->data.source_id){
			chunk.F.source_id_len = (uint16_t)strlen(base_پیام->data.source_id);
			chunk.source.id = base_پیام->data.source_id;
		}وگرنه{
			chunk.F.source_id_len = 0;
			chunk.source.id = پوچ;
		}
		اگر(base_پیام->data.source_username){
			chunk.F.source_username_len = (uint16_t)strlen(base_پیام->data.source_username);
			chunk.source.username = base_پیام->data.source_username;
		}وگرنه{
			chunk.F.source_username_len = 0;
			chunk.source.username = پوچ;
		}

		chunk.F.topic_len = (uint16_t)strlen(base_پیام->data.topic);
		chunk.topic = base_پیام->data.topic;

		اگر(base_پیام->source_listener){
			chunk.F.source_port = base_پیام->source_listener->port;
		}وگرنه{
			chunk.F.source_port = 0;
		}
		chunk.F.qos = base_پیام->data.qos;
		chunk.payload = base_پیام->data.payload;
		chunk.properties = base_پیام->data.properties;

		rc = persist__chunk_message_store_write_v6(db_fptr, &chunk);
		اگر(rc){
			بازگشت rc;
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح persist__client_save(FILE *db_fptr)
{
	ساختار mosquitto *context, *ctxt_tmp;
	ساختار P_client chunk;
	صحیح rc;

	أكد(db_fptr);

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		memset(&chunk, 0, اندازه(ساختار P_client));

		اگر(context &&
				context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE &&
#ifdef WITH_BRIDGE
				((!context->bridge && context->clean_start == false)
				|| (context->bridge && context->bridge->clean_start_local == false))
#وگرنه
				context->clean_start == false
#endif
				){
			chunk.F.session_expiry_time = context->session_expiry_time;
			اگر(context->session_expiry_interval != MQTT_SESSION_EXPIRY_NEVER
					&& context->session_expiry_time == 0){

				chunk.F.session_expiry_time = context->session_expiry_interval + db.now_real_s;
			}وگرنه{
				chunk.F.session_expiry_time = context->session_expiry_time;
			}
			chunk.F.session_expiry_interval = context->session_expiry_interval;
			chunk.F.last_mid = context->last_mid;
			chunk.F.id_len = (uint16_t)strlen(context->id);
			chunk.clientid = context->id;
			اگر(context->username){
				chunk.F.username_len = (uint16_t)strlen(context->username);
				chunk.username = context->username;
			}
			اگر(context->listener){
				chunk.F.listener_port = context->listener->port;
			}

			اگر(chunk.F.id_len == 0){
				/* This should never happen, but in case we have a client with
				 * zero length ID, don't persist them. */
				ادامه;
			}

			rc = persist__chunk_client_write_v6(db_fptr, &chunk);
			اگر(rc){
				بازگشت rc;
			}

			اگر(persist__client_messages_save(db_fptr, context, context->msgs_in.inflight)){
				بازگشت 1;
			}
			اگر(persist__client_messages_save(db_fptr, context, context->msgs_in.queued)){
				بازگشت 1;
			}
			اگر(persist__client_messages_save(db_fptr, context, context->msgs_out.inflight)){
				بازگشت 1;
			}
			اگر(persist__client_messages_save(db_fptr, context, context->msgs_out.queued)){
				بازگشت 1;
			}
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح persist__subs_save(FILE *db_fptr, ساختار mosquitto__subhier *گره, ثابت نویسه *topic, صحیح level)
{
	ساختار mosquitto__subhier *subhier, *subhier_tmp;
	ساختار mosquitto__subleaf *sub;
	ساختار P_sub sub_chunk;
	نویسه *thistopic;
	size_t slen;
	صحیح rc;

	slen = strlen(topic) + گره->topic_len + 2;
	thistopic = mosquitto_malloc(اندازه(نویسه)*slen);
	اگر(!thistopic){
		بازگشت MOSQ_ERR_NOMEM;
	}
	اگر(level > 1 || strlen(topic)){
		snprintf(thistopic, slen, "%s/%s", topic, گره->topic);
	}وگرنه{
		snprintf(thistopic, slen, "%s", گره->topic);
	}

	sub = گره->subs;
	وقتی(sub){
		اگر(sub->context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE
				&& sub->context->clean_start == false
				&& sub->context->id){

			memset(&sub_chunk, 0, اندازه(ساختار P_sub));

			sub_chunk.F.identifier = sub->identifier;
			sub_chunk.F.id_len = (uint16_t)strlen(sub->context->id);
			sub_chunk.F.topic_len = (uint16_t)strlen(thistopic);
			sub_chunk.F.qos = MQTT_SUB_OPT_GET_QOS(sub->subscription_options);
			sub_chunk.F.options = sub->subscription_options & 0xFC;
			sub_chunk.clientid = sub->context->id;
			sub_chunk.topic = thistopic;

			rc = persist__chunk_sub_write_v6(db_fptr, &sub_chunk);
			اگر(rc){
				mosquitto_FREE(thistopic);
				بازگشت rc;
			}
		}
		sub = sub->next;
	}

	HASH_ITER(hh, گره->children, subhier, subhier_tmp){
		persist__subs_save(db_fptr, subhier, thistopic, level+1);
	}
	mosquitto_FREE(thistopic);
	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح persist__subs_save_all(FILE *db_fptr)
{
	ساختار mosquitto__subhier *subhier, *subhier_tmp;

	HASH_ITER(hh, db.normal_subs, subhier, subhier_tmp){
		اگر(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	HASH_ITER(hh, db.shared_subs, subhier, subhier_tmp){
		اگر(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح persist__retain_save(FILE *db_fptr, ساختار mosquitto__retainhier *گره, صحیح level)
{
	ساختار mosquitto__retainhier *retainhier, *retainhier_tmp;
	ساختار P_retain retain_chunk;
	صحیح rc;

	اگر(گره->retained && strncmp(گره->retained->data.topic, "$SYS", 4)){
		memset(&retain_chunk, 0, اندازه(ساختار P_retain));

		/* Don't save $SYS messages. */
		retain_chunk.F.store_id = گره->retained->data.store_id;
		rc = persist__chunk_retain_write_v6(db_fptr, &retain_chunk);
		اگر(rc){
			بازگشت rc;
		}
	}

	HASH_ITER(hh, گره->children, retainhier, retainhier_tmp){
		persist__retain_save(db_fptr, retainhier, level+1);
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح persist__retain_save_all(FILE *db_fptr)
{
	ساختار mosquitto__retainhier *retainhier, *retainhier_tmp;

	HASH_ITER(hh, db.retains, retainhier, retainhier_tmp){
		اگر(retainhier->children){
			persist__retain_save(db_fptr, retainhier->children, 0);
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
}

ایستا صحیح persist__write_data(FILE *db_fptr, تهی *user_data);


ایستا تهی persist__log_write_error(ثابت نویسه *پیام)
{
	log__printf(پوچ, MOSQ_LOG_ERR, "Error saving in-memory database, %s", پیام);
}


صحیح persist__backup(bool shutdown)
{
	اگر(db.config == پوچ){
		بازگشت MOSQ_ERR_INVAL;
	}
	اگر(db.config->persistence == false){
		بازگشت MOSQ_ERR_SUCCESS;
	}
	اگر(db.config->persistence_filepath == پوچ){
		بازگشت MOSQ_ERR_INVAL;
	}

	log__printf(پوچ, MOSQ_LOG_INFO, "Saving in-memory database to %s.", db.config->persistence_filepath);

	بازگشت mosquitto_write_file(db.config->persistence_filepath, true, &persist__write_data, &shutdown, &persist__log_write_error);
}


ایستا صحیح persist__write_data(FILE *db_fptr, تهی *user_data)
{
	bool shutdown = *(bool *)(user_data);
	uint32_t db_version_w = htonl(MOSQ_DB_VERSION);
	uint32_t crc = 0;
	ثابت نویسه *err;
	ساختار PF_cfg cfg_chunk;
	صحیح rc = MOSQ_ERR_UNKNOWN;

	/* Header */
	write_e(db_fptr, magic, 15);
	write_e(db_fptr, &crc, اندازه(uint32_t));
	write_e(db_fptr, &db_version_w, اندازه(uint32_t));

	memset(&cfg_chunk, 0, اندازه(ساختار PF_cfg));
	cfg_chunk.last_db_id = db.last_db_id;
	cfg_chunk.shutdown = shutdown;
	cfg_chunk.dbid_size = اندازه(dbid_t);
	اگر(persist__chunk_cfg_write_v6(db_fptr, &cfg_chunk)){
		برو error;
	}

	اگر(persist__message_store_save(db_fptr)){
		برو error;
	}

	اگر(persist__client_save(db_fptr)
			|| persist__subs_save_all(db_fptr)
			|| persist__retain_save_all(db_fptr)){
		برو error;
	}
	بازگشت MOSQ_ERR_SUCCESS;

error:
	err = strerror(errno);
	log__printf(پوچ, MOSQ_LOG_ERR, "Error during saving in-memory database %s: %s.", db.config->persistence_filepath, err);
	اگر(db_fptr){
		fclose(db_fptr);
	}
	بازگشت rc;
}


#endif
