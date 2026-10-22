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

#که_تعریف_شوی WITH_PERSISTENCE

#که_نه_تعریف_شوی WIN32
#ضمن <arpa/inet.h>
#د_که_پای
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


ساكن صحيح persist__client_messages_save(FILE *db_fptr, بنية mosquitto *context, بنية mosquitto_client_پیغام *کتار)
{
	بنية P_client_پیغام chunk;
	بنية mosquitto_client_پیغام *cmsg;
	صحيح rc;

	أكد(db_fptr);
	أكد(context);

	cmsg = کتار;
	طالما(cmsg){
		إذا(!مزي_پرتله(cmsg->بنسټ_پیغام->معلومات.topic, "$SYS", 4)
				&& cmsg->بنسټ_پیغام->ref_شمېر <= 1
				&& cmsg->بنسټ_پیغام->dest_id_شمېر == 0){

			/* This $SYS message won't have been persisted, so we can't persist
			 * this client message. */
			cmsg = cmsg->بل;
			استمر;
		}

		memset(&chunk, 0, حجم(بنية P_client_پیغام));

		chunk.F.store_id = cmsg->بنسټ_پیغام->معلومات.store_id;
		chunk.F.mid = cmsg->معلومات.mid;
		chunk.F.id_len = (uint16_t)مزي_اوږدوالی(context->id);
		chunk.F.qos = cmsg->معلومات.qos;
		chunk.F.retain_dup = (uint8_t)((cmsg->معلومات.retain&0x0F)<<4 | (cmsg->معلومات.dup&0x0F));
		chunk.F.direction = (uint8_t)cmsg->معلومات.direction;
		chunk.F.state = (uint8_t)cmsg->معلومات.state;
		chunk.clientid = context->id;
		chunk.subscription_identifier = cmsg->معلومات.subscription_identifier;

		rc = persist_chunk_client_پیغام_write_v6(db_fptr, &chunk);
		إذا(rc){
			أرجع rc;
		}

		cmsg = cmsg->بل;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح persist_پیغام_store_save(FILE *db_fptr)
{
	بنية P_بنسټ_پیغام chunk;
	بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام, *بنسټ_پیغام_tmp;
	صحيح rc;

	أكد(db_fptr);

	بنسټ_پیغام = db.پیغام_store;
	HASH_ITER(hh, db.پیغام_store, بنسټ_پیغام, بنسټ_پیغام_tmp){
		إذا(بنسټ_پیغام->ref_شمېر < 1 || بنسټ_پیغام->معلومات.topic == عدم){
			استمر;
		}

		memset(&chunk, 0, حجم(بنية P_بنسټ_پیغام));

		إذا(!مزي_پرتله(بنسټ_پیغام->معلومات.topic, "$SYS", 4)){
			إذا(بنسټ_پیغام->ref_شمېر <= 1 && بنسټ_پیغام->dest_id_شمېر == 0){
				/* $SYS messages that are only retained shouldn't be persisted. */
				استمر;
			}
			/* Don't save $SYS messages as retained otherwise they can give
			 * misleading information when reloaded. They should still be saved
			 * because a disconnected durable client may have them in their
			 * queue. */
			chunk.F.retain = 0;
		}وإلا{
			chunk.F.retain = (uint8_t)بنسټ_پیغام->معلومات.retain;
		}

		chunk.F.store_id = بنسټ_پیغام->معلومات.store_id;
		chunk.F.expiry_وخت = بنسټ_پیغام->معلومات.expiry_وخت;
		chunk.F.payloadlen = بنسټ_پیغام->معلومات.payloadlen;
		chunk.F.سرچینه_mid = بنسټ_پیغام->معلومات.سرچینه_mid;
		إذا(بنسټ_پیغام->معلومات.سرچینه_id){
			chunk.F.سرچینه_id_len = (uint16_t)مزي_اوږدوالی(بنسټ_پیغام->معلومات.سرچینه_id);
			chunk.سرچینه.id = بنسټ_پیغام->معلومات.سرچینه_id;
		}وإلا{
			chunk.F.سرچینه_id_len = 0;
			chunk.سرچینه.id = عدم;
		}
		إذا(بنسټ_پیغام->معلومات.سرچینه_username){
			chunk.F.سرچینه_username_len = (uint16_t)مزي_اوږدوالی(بنسټ_پیغام->معلومات.سرچینه_username);
			chunk.سرچینه.username = بنسټ_پیغام->معلومات.سرچینه_username;
		}وإلا{
			chunk.F.سرچینه_username_len = 0;
			chunk.سرچینه.username = عدم;
		}

		chunk.F.topic_len = (uint16_t)مزي_اوږدوالی(بنسټ_پیغام->معلومات.topic);
		chunk.topic = بنسټ_پیغام->معلومات.topic;

		إذا(بنسټ_پیغام->سرچینه_listener){
			chunk.F.سرچینه_پورټ = بنسټ_پیغام->سرچینه_listener->پورټ;
		}وإلا{
			chunk.F.سرچینه_پورټ = 0;
		}
		chunk.F.qos = بنسټ_پیغام->معلومات.qos;
		chunk.payload = بنسټ_پیغام->معلومات.payload;
		chunk.properties = بنسټ_پیغام->معلومات.properties;

		rc = persist_chunk_پیغام_store_write_v6(db_fptr, &chunk);
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
				context->session_expiry_interval != MQTT_SESSION_EXPIRY_سمدستي &&
#که_تعریف_شوی WITH_BRIDGE
				((!context->bridge && context->clean_پیل == false)
				|| (context->bridge && context->bridge->clean_پیل_local == false))
#وإلا
				context->clean_پیل == false
#د_که_پای
				){
			chunk.F.session_expiry_وخت = context->session_expiry_وخت;
			إذا(context->session_expiry_interval != MQTT_SESSION_EXPIRY_NEVER
					&& context->session_expiry_وخت == 0){

				chunk.F.session_expiry_وخت = context->session_expiry_interval + db.now_real_s;
			}وإلا{
				chunk.F.session_expiry_وخت = context->session_expiry_وخت;
			}
			chunk.F.session_expiry_interval = context->session_expiry_interval;
			chunk.F.وروستی_mid = context->وروستی_mid;
			chunk.F.id_len = (uint16_t)مزي_اوږدوالی(context->id);
			chunk.clientid = context->id;
			إذا(context->username){
				chunk.F.username_len = (uint16_t)مزي_اوږدوالی(context->username);
				chunk.username = context->username;
			}
			إذا(context->listener){
				chunk.F.listener_پورټ = context->listener->پورټ;
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

			إذا(persist__client_messages_save(db_fptr, context, context->msgs_دننه.inflight)){
				أرجع 1;
			}
			إذا(persist__client_messages_save(db_fptr, context, context->msgs_دننه.queued)){
				أرجع 1;
			}
			إذا(persist__client_messages_save(db_fptr, context, context->msgs_بهر.inflight)){
				أرجع 1;
			}
			إذا(persist__client_messages_save(db_fptr, context, context->msgs_بهر.queued)){
				أرجع 1;
			}
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح persist__subs_save(FILE *db_fptr, بنية mosquitto__subhier *غوټه, ثابت حرف *topic, صحيح level)
{
	بنية mosquitto__subhier *subhier, *subhier_tmp;
	بنية mosquitto__subleaf *sub;
	بنية P_sub sub_chunk;
	حرف *thistopic;
	size_t slen;
	صحيح rc;

	slen = مزي_اوږدوالی(topic) + غوټه->topic_len + 2;
	thistopic = mosquitto_malloc(حجم(حرف)*slen);
	إذا(!thistopic){
		أرجع MOSQ_ERR_NOMEM;
	}
	إذا(level > 1 || مزي_اوږدوالی(topic)){
		snprintf(thistopic, slen, "%s/%s", topic, غوټه->topic);
	}وإلا{
		snprintf(thistopic, slen, "%s", غوټه->topic);
	}

	sub = غوټه->subs;
	طالما(sub){
		إذا(sub->context->session_expiry_interval != MQTT_SESSION_EXPIRY_سمدستي
				&& sub->context->clean_پیل == false
				&& sub->context->id){

			memset(&sub_chunk, 0, حجم(بنية P_sub));

			sub_chunk.F.identifier = sub->identifier;
			sub_chunk.F.id_len = (uint16_t)مزي_اوږدوالی(sub->context->id);
			sub_chunk.F.topic_len = (uint16_t)مزي_اوږدوالی(thistopic);
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
		sub = sub->بل;
	}

	HASH_ITER(hh, غوټه->children, subhier, subhier_tmp){
		persist__subs_save(db_fptr, subhier, thistopic, level+1);
	}
	mosquitto_FREE(thistopic);
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح persist__subs_save_all(FILE *db_fptr)
{
	بنية mosquitto__subhier *subhier, *subhier_tmp;

	HASH_ITER(hh, db.نورمال_subs, subhier, subhier_tmp){
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


ساكن صحيح persist__retain_save(FILE *db_fptr, بنية mosquitto__retainhier *غوټه, صحيح level)
{
	بنية mosquitto__retainhier *retainhier, *retainhier_tmp;
	بنية P_retain retain_chunk;
	صحيح rc;

	إذا(غوټه->retained && مزي_پرتله(غوټه->retained->معلومات.topic, "$SYS", 4)){
		memset(&retain_chunk, 0, حجم(بنية P_retain));

		/* Don't save $SYS messages. */
		retain_chunk.F.store_id = غوټه->retained->معلومات.store_id;
		rc = persist__chunk_retain_write_v6(db_fptr, &retain_chunk);
		إذا(rc){
			أرجع rc;
		}
	}

	HASH_ITER(hh, غوټه->children, retainhier, retainhier_tmp){
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


ساكن فراغ persist__log_write_error(ثابت حرف *پیغام)
{
	log__printf(عدم, MOSQ_LOG_ERR, "Error saving in-memory database, %s", پیغام);
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
	uint32_t db_نسخه_w = htonl(MOSQ_DB_نسخه);
	uint32_t دوراني_ازموینه = 0;
	ثابت حرف *err;
	بنية PF_cfg cfg_chunk;
	صحيح rc = MOSQ_ERR_UNKNOWN;

	/*سرلیک*/
	write_e(db_fptr, magic, 15);
	write_e(db_fptr, &دوراني_ازموینه, حجم(uint32_t));
	write_e(db_fptr, &db_نسخه_w, حجم(uint32_t));

	memset(&cfg_chunk, 0, حجم(بنية PF_cfg));
	cfg_chunk.وروستی_db_id = db.وروستی_db_id;
	cfg_chunk.shutdown = shutdown;
	cfg_chunk.dbid_اندازه = حجم(dbid_t);
	إذا(persist__chunk_cfg_write_v6(db_fptr, &cfg_chunk)){
		اذهب error;
	}

	إذا(persist_پیغام_store_save(db_fptr)){
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


#د_که_پای
