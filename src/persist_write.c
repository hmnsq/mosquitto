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

#جيڪڏهن_وصف_ٿيل WITH_PERSISTENCE

#جيڪڏهن_وصف_نه_ٿيل WIN32
#ضمن <arpa/inet.h>
#جيڪڏهن_پورو
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


ساكن صحيح persist__client_messages_save(FILE *db_fptr, بنية mosquitto *context, بنية mosquitto_client_نياپو *قطار)
{
	بنية P_client_نياپو chunk;
	بنية mosquitto_client_نياپو *cmsg;
	صحيح rc;

	أكد(db_fptr);
	أكد(context);

	cmsg = قطار;
	طالما(cmsg){
		إذا(!لڙي_ڀيٽ(cmsg->بنياد_نياپو->معلومات.topic, "$SYS", 4)
				&& cmsg->بنياد_نياپو->ref_ڳڻپ <= 1
				&& cmsg->بنياد_نياپو->dest_id_ڳڻپ == 0){

			/* This $SYS message won't have been persisted, so we can't persist
			 * this client message. */
			cmsg = cmsg->ايندڙ;
			استمر;
		}

		memset(&chunk, 0, حجم(بنية P_client_نياپو));

		chunk.F.store_id = cmsg->بنياد_نياپو->معلومات.store_id;
		chunk.F.mid = cmsg->معلومات.mid;
		chunk.F.id_len = (uint16_t)لڙي_ڊيگهه(context->id);
		chunk.F.qos = cmsg->معلومات.qos;
		chunk.F.retain_dup = (uint8_t)((cmsg->معلومات.retain&0x0F)<<4 | (cmsg->معلومات.dup&0x0F));
		chunk.F.direction = (uint8_t)cmsg->معلومات.direction;
		chunk.F.state = (uint8_t)cmsg->معلومات.state;
		chunk.clientid = context->id;
		chunk.subscription_identifier = cmsg->معلومات.subscription_identifier;

		rc = persist_chunk_client_نياپو_write_v6(db_fptr, &chunk);
		إذا(rc){
			أرجع rc;
		}

		cmsg = cmsg->ايندڙ;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح persist_پيغام_store_save(FILE *db_fptr)
{
	بنية P_بنياد_نياپو chunk;
	بنية mosquitto_بنياد_نياپو *بنياد_نياپو, *بنياد_نياپو_tmp;
	صحيح rc;

	أكد(db_fptr);

	بنياد_نياپو = db.نياپو_store;
	HASH_ITER(hh, db.نياپو_store, بنياد_نياپو, بنياد_نياپو_tmp){
		إذا(بنياد_نياپو->ref_ڳڻپ < 1 || بنياد_نياپو->معلومات.topic == عدم){
			استمر;
		}

		memset(&chunk, 0, حجم(بنية P_بنياد_نياپو));

		إذا(!لڙي_ڀيٽ(بنياد_نياپو->معلومات.topic, "$SYS", 4)){
			إذا(بنياد_نياپو->ref_ڳڻپ <= 1 && بنياد_نياپو->dest_id_ڳڻپ == 0){
				/* $SYS messages that are only retained shouldn't be persisted. */
				استمر;
			}
			/* Don't save $SYS messages as retained otherwise they can give
			 * misleading information when reloaded. They should still be saved
			 * because a disconnected durable client may have them in their
			 * queue. */
			chunk.F.retain = 0;
		}وإلا{
			chunk.F.retain = (uint8_t)بنياد_نياپو->معلومات.retain;
		}

		chunk.F.store_id = بنياد_نياپو->معلومات.store_id;
		chunk.F.expiry_وقت = بنياد_نياپو->معلومات.expiry_وقت;
		chunk.F.payloadlen = بنياد_نياپو->معلومات.payloadlen;
		chunk.F.ذريعو_mid = بنياد_نياپو->معلومات.ذريعو_mid;
		إذا(بنياد_نياپو->معلومات.ذريعو_id){
			chunk.F.ذريعو_id_len = (uint16_t)لڙي_ڊيگهه(بنياد_نياپو->معلومات.ذريعو_id);
			chunk.ذريعو.id = بنياد_نياپو->معلومات.ذريعو_id;
		}وإلا{
			chunk.F.ذريعو_id_len = 0;
			chunk.ذريعو.id = عدم;
		}
		إذا(بنياد_نياپو->معلومات.ذريعو_username){
			chunk.F.ذريعو_username_len = (uint16_t)لڙي_ڊيگهه(بنياد_نياپو->معلومات.ذريعو_username);
			chunk.ذريعو.username = بنياد_نياپو->معلومات.ذريعو_username;
		}وإلا{
			chunk.F.ذريعو_username_len = 0;
			chunk.ذريعو.username = عدم;
		}

		chunk.F.topic_len = (uint16_t)لڙي_ڊيگهه(بنياد_نياپو->معلومات.topic);
		chunk.topic = بنياد_نياپو->معلومات.topic;

		إذا(بنياد_نياپو->ذريعو_listener){
			chunk.F.ذريعو_پورٽ = بنياد_نياپو->ذريعو_listener->پورٽ;
		}وإلا{
			chunk.F.ذريعو_پورٽ = 0;
		}
		chunk.F.qos = بنياد_نياپو->معلومات.qos;
		chunk.payload = بنياد_نياپو->معلومات.payload;
		chunk.properties = بنياد_نياپو->معلومات.properties;

		rc = persist_chunk_پيغام_store_write_v6(db_fptr, &chunk);
		إذا(rc){
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

		إذا(context &&
				context->session_expiry_interval != MQTT_SESSION_EXPIRY_فوري &&
#جيڪڏهن_وصف_ٿيل WITH_BRIDGE
				((!context->bridge && context->clean_شروعات == false)
				|| (context->bridge && context->bridge->clean_شروعات_local == false))
#وإلا
				context->clean_شروعات == false
#جيڪڏهن_پورو
				){
			chunk.F.session_expiry_وقت = context->session_expiry_وقت;
			إذا(context->session_expiry_interval != MQTT_SESSION_EXPIRY_NEVER
					&& context->session_expiry_وقت == 0){

				chunk.F.session_expiry_وقت = context->session_expiry_interval + db.now_real_s;
			}وإلا{
				chunk.F.session_expiry_وقت = context->session_expiry_وقت;
			}
			chunk.F.session_expiry_interval = context->session_expiry_interval;
			chunk.F.آخري_mid = context->آخري_mid;
			chunk.F.id_len = (uint16_t)لڙي_ڊيگهه(context->id);
			chunk.clientid = context->id;
			إذا(context->username){
				chunk.F.username_len = (uint16_t)لڙي_ڊيگهه(context->username);
				chunk.username = context->username;
			}
			إذا(context->listener){
				chunk.F.listener_پورٽ = context->listener->پورٽ;
			}

			إذا(chunk.F.id_len == 0){
				/* This should never happen, but in case we have a client with
				 * zero length ID, don't persist them. */
				استمر;
			}

			rc = persist__chunk_client_write_v6(db_fptr, &chunk);
			إذا(rc){
				أرجع rc;
			}

			إذا(persist__client_messages_save(db_fptr, context, context->msgs_اندريون.inflight)){
				أرجع 1;
			}
			إذا(persist__client_messages_save(db_fptr, context, context->msgs_اندريون.queued)){
				أرجع 1;
			}
			إذا(persist__client_messages_save(db_fptr, context, context->msgs_ٻاهريون.inflight)){
				أرجع 1;
			}
			إذا(persist__client_messages_save(db_fptr, context, context->msgs_ٻاهريون.queued)){
				أرجع 1;
			}
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح persist__subs_save(FILE *db_fptr, بنية mosquitto__subhier *ڳنڍ, ثابت حرف *topic, صحيح level)
{
	بنية mosquitto__subhier *subhier, *subhier_tmp;
	بنية mosquitto__subleaf *sub;
	بنية P_sub sub_chunk;
	حرف *thistopic;
	size_t slen;
	صحيح rc;

	slen = لڙي_ڊيگهه(topic) + ڳنڍ->topic_len + 2;
	thistopic = mosquitto_malloc(حجم(حرف)*slen);
	إذا(!thistopic){
		أرجع MOSQ_ERR_NOMEM;
	}
	إذا(level > 1 || لڙي_ڊيگهه(topic)){
		snprintf(thistopic, slen, "%s/%s", topic, ڳنڍ->topic);
	}وإلا{
		snprintf(thistopic, slen, "%s", ڳنڍ->topic);
	}

	sub = ڳنڍ->subs;
	طالما(sub){
		إذا(sub->context->session_expiry_interval != MQTT_SESSION_EXPIRY_فوري
				&& sub->context->clean_شروعات == false
				&& sub->context->id){

			memset(&sub_chunk, 0, حجم(بنية P_sub));

			sub_chunk.F.identifier = sub->identifier;
			sub_chunk.F.id_len = (uint16_t)لڙي_ڊيگهه(sub->context->id);
			sub_chunk.F.topic_len = (uint16_t)لڙي_ڊيگهه(thistopic);
			sub_chunk.F.qos = MQTT_SUB_OPT_GET_QOS(sub->subscription_options);
			sub_chunk.F.options = sub->subscription_options & 0xFC;
			sub_chunk.clientid = sub->context->id;
			sub_chunk.topic = thistopic;

			rc = persist__chunk_sub_write_v6(db_fptr, &sub_chunk);
			إذا(rc){
				mosquitto_FREE(thistopic);
				أرجع rc;
			}
		}
		sub = sub->ايندڙ;
	}

	HASH_ITER(hh, ڳنڍ->children, subhier, subhier_tmp){
		persist__subs_save(db_fptr, subhier, thistopic, level+1);
	}
	mosquitto_FREE(thistopic);
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح persist__subs_save_all(FILE *db_fptr)
{
	بنية mosquitto__subhier *subhier, *subhier_tmp;

	HASH_ITER(hh, db.عام_subs, subhier, subhier_tmp){
		إذا(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	HASH_ITER(hh, db.shared_subs, subhier, subhier_tmp){
		إذا(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح persist__retain_save(FILE *db_fptr, بنية mosquitto__retainhier *ڳنڍ, صحيح level)
{
	بنية mosquitto__retainhier *retainhier, *retainhier_tmp;
	بنية P_retain retain_chunk;
	صحيح rc;

	إذا(ڳنڍ->retained && لڙي_ڀيٽ(ڳنڍ->retained->معلومات.topic, "$SYS", 4)){
		memset(&retain_chunk, 0, حجم(بنية P_retain));

		/* Don't save $SYS messages. */
		retain_chunk.F.store_id = ڳنڍ->retained->معلومات.store_id;
		rc = persist__chunk_retain_write_v6(db_fptr, &retain_chunk);
		إذا(rc){
			أرجع rc;
		}
	}

	HASH_ITER(hh, ڳنڍ->children, retainhier, retainhier_tmp){
		persist__retain_save(db_fptr, retainhier, level+1);
	}
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح persist__retain_save_all(FILE *db_fptr)
{
	بنية mosquitto__retainhier *retainhier, *retainhier_tmp;

	HASH_ITER(hh, db.retains, retainhier, retainhier_tmp){
		إذا(retainhier->children){
			persist__retain_save(db_fptr, retainhier->children, 0);
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}

ساكن صحيح persist_write_معلومات(FILE *db_fptr, فراغ *user_معلومات);


ساكن فراغ persist__log_write_error(ثابت حرف *نياپو)
{
	log__printf(عدم, MOSQ_LOG_ERR, "Error saving in-memory database, %s", نياپو);
}


صحيح persist__backup(bool shutdown)
{
	إذا(db.config == عدم){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(db.config->persistence == false){
		أرجع MOSQ_ERR_SUCCESS;
	}
	إذا(db.config->persistence_filepath == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	log__printf(عدم, MOSQ_LOG_INFO, "Saving in-memory database to %s.", db.config->persistence_filepath);

	أرجع mosquitto_write_file(db.config->persistence_filepath, true, &persist_write_معلومات, &shutdown, &persist__log_write_error);
}


ساكن صحيح persist_write_معلومات(FILE *db_fptr, فراغ *user_معلومات)
{
	bool shutdown = *(bool *)(user_معلومات);
	uint32_t db_نسخو_w = htonl(MOSQ_DB_نسخو);
	uint32_t چڪري_جاچ = 0;
	ثابت حرف *err;
	بنية PF_cfg cfg_chunk;
	صحيح rc = MOSQ_ERR_UNKNOWN;

	/*سرو*/
	write_e(db_fptr, magic, 15);
	write_e(db_fptr, &چڪري_جاچ, حجم(uint32_t));
	write_e(db_fptr, &db_نسخو_w, حجم(uint32_t));

	memset(&cfg_chunk, 0, حجم(بنية PF_cfg));
	cfg_chunk.آخري_db_id = db.آخري_db_id;
	cfg_chunk.shutdown = shutdown;
	cfg_chunk.dbid_سائيز = حجم(dbid_t);
	إذا(persist__chunk_cfg_write_v6(db_fptr, &cfg_chunk)){
		اذهب error;
	}

	إذا(persist_پيغام_store_save(db_fptr)){
		اذهب error;
	}

	إذا(persist__client_save(db_fptr)
			|| persist__subs_save_all(db_fptr)
			|| persist__retain_save_all(db_fptr)){
		اذهب error;
	}
	أرجع MOSQ_ERR_SUCCESS;

error:
	err = strerror(errno);
	log__printf(عدم, MOSQ_LOG_ERR, "Error during saving in-memory database %s: %s.", db.config->persistence_filepath, err);
	إذا(db_fptr){
		fclose(db_fptr);
	}
	أرجع rc;
}


#جيڪڏهن_پورو
