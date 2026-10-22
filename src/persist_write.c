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

#اگر_تعریف_شده WITH_PERSISTENCE

#اگر_تعریف_نشده WIN32
#ضمن <arpa/inet.h>
#پایان_اگر
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


ساكن صحيح persist__client_messages_save(FILE *db_fptr, بنية mosquitto *context, بنية mosquitto_client_پیام *صف)
{
	بنية P_client_پیام chunk;
	بنية mosquitto_client_پیام *cmsg;
	صحيح rc;

	أكد(db_fptr);
	أكد(context);

	cmsg = صف;
	طالما(cmsg){
		إذا(!مقایسه_رشته(cmsg->پایه_پیام->داده.topic, "$SYS", 4)
				&& cmsg->پایه_پیام->ref_شمار <= 1
				&& cmsg->پایه_پیام->dest_id_شمار == 0){

			/* This $SYS message won't have been persisted, so we can't persist
			 * this client message. */
			cmsg = cmsg->بعدی;
			استمر;
		}

		memset(&chunk, 0, حجم(بنية P_client_پیام));

		chunk.F.store_id = cmsg->پایه_پیام->داده.store_id;
		chunk.F.mid = cmsg->داده.mid;
		chunk.F.id_len = (uint16_t)طول_رشته(context->id);
		chunk.F.qos = cmsg->داده.qos;
		chunk.F.retain_dup = (uint8_t)((cmsg->داده.retain&0x0F)<<4 | (cmsg->داده.dup&0x0F));
		chunk.F.direction = (uint8_t)cmsg->داده.direction;
		chunk.F.state = (uint8_t)cmsg->داده.state;
		chunk.clientid = context->id;
		chunk.subscription_identifier = cmsg->داده.subscription_identifier;

		rc = persist_chunk_client_پیام_write_v6(db_fptr, &chunk);
		إذا(rc){
			أرجع rc;
		}

		cmsg = cmsg->بعدی;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح persist_پیام_store_save(FILE *db_fptr)
{
	بنية P_پایه_پیام chunk;
	بنية mosquitto_پایه_پیام *پایه_پیام, *پایه_پیام_tmp;
	صحيح rc;

	أكد(db_fptr);

	پایه_پیام = db.پیام_store;
	HASH_ITER(hh, db.پیام_store, پایه_پیام, پایه_پیام_tmp){
		إذا(پایه_پیام->ref_شمار < 1 || پایه_پیام->داده.topic == عدم){
			استمر;
		}

		memset(&chunk, 0, حجم(بنية P_پایه_پیام));

		إذا(!مقایسه_رشته(پایه_پیام->داده.topic, "$SYS", 4)){
			إذا(پایه_پیام->ref_شمار <= 1 && پایه_پیام->dest_id_شمار == 0){
				/* $SYS messages that are only retained shouldn't be persisted. */
				استمر;
			}
			/* Don't save $SYS messages as retained otherwise they can give
			 * misleading information when reloaded. They should still be saved
			 * because a disconnected durable client may have them in their
			 * queue. */
			chunk.F.retain = 0;
		}وإلا{
			chunk.F.retain = (uint8_t)پایه_پیام->داده.retain;
		}

		chunk.F.store_id = پایه_پیام->داده.store_id;
		chunk.F.expiry_زمان = پایه_پیام->داده.expiry_زمان;
		chunk.F.payloadlen = پایه_پیام->داده.payloadlen;
		chunk.F.منبع_mid = پایه_پیام->داده.منبع_mid;
		إذا(پایه_پیام->داده.منبع_id){
			chunk.F.منبع_id_len = (uint16_t)طول_رشته(پایه_پیام->داده.منبع_id);
			chunk.منبع.id = پایه_پیام->داده.منبع_id;
		}وإلا{
			chunk.F.منبع_id_len = 0;
			chunk.منبع.id = عدم;
		}
		إذا(پایه_پیام->داده.منبع_username){
			chunk.F.منبع_username_len = (uint16_t)طول_رشته(پایه_پیام->داده.منبع_username);
			chunk.منبع.username = پایه_پیام->داده.منبع_username;
		}وإلا{
			chunk.F.منبع_username_len = 0;
			chunk.منبع.username = عدم;
		}

		chunk.F.topic_len = (uint16_t)طول_رشته(پایه_پیام->داده.topic);
		chunk.topic = پایه_پیام->داده.topic;

		إذا(پایه_پیام->منبع_listener){
			chunk.F.منبع_پورت = پایه_پیام->منبع_listener->پورت;
		}وإلا{
			chunk.F.منبع_پورت = 0;
		}
		chunk.F.qos = پایه_پیام->داده.qos;
		chunk.payload = پایه_پیام->داده.payload;
		chunk.properties = پایه_پیام->داده.properties;

		rc = persist_chunk_پیام_store_write_v6(db_fptr, &chunk);
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
				context->session_expiry_interval != MQTT_SESSION_EXPIRY_فوری &&
#اگر_تعریف_شده WITH_BRIDGE
				((!context->bridge && context->clean_شروع == false)
				|| (context->bridge && context->bridge->clean_شروع_local == false))
#وإلا
				context->clean_شروع == false
#پایان_اگر
				){
			chunk.F.session_expiry_زمان = context->session_expiry_زمان;
			إذا(context->session_expiry_interval != MQTT_SESSION_EXPIRY_NEVER
					&& context->session_expiry_زمان == 0){

				chunk.F.session_expiry_زمان = context->session_expiry_interval + db.now_real_s;
			}وإلا{
				chunk.F.session_expiry_زمان = context->session_expiry_زمان;
			}
			chunk.F.session_expiry_interval = context->session_expiry_interval;
			chunk.F.آخرین_mid = context->آخرین_mid;
			chunk.F.id_len = (uint16_t)طول_رشته(context->id);
			chunk.clientid = context->id;
			إذا(context->username){
				chunk.F.username_len = (uint16_t)طول_رشته(context->username);
				chunk.username = context->username;
			}
			إذا(context->listener){
				chunk.F.listener_پورت = context->listener->پورت;
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

			إذا(persist__client_messages_save(db_fptr, context, context->msgs_درون.inflight)){
				أرجع 1;
			}
			إذا(persist__client_messages_save(db_fptr, context, context->msgs_درون.queued)){
				أرجع 1;
			}
			إذا(persist__client_messages_save(db_fptr, context, context->msgs_بیرون.inflight)){
				أرجع 1;
			}
			إذا(persist__client_messages_save(db_fptr, context, context->msgs_بیرون.queued)){
				أرجع 1;
			}
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح persist__subs_save(FILE *db_fptr, بنية mosquitto__subhier *گره, ثابت حرف *topic, صحيح level)
{
	بنية mosquitto__subhier *subhier, *subhier_tmp;
	بنية mosquitto__subleaf *sub;
	بنية P_sub sub_chunk;
	حرف *thistopic;
	size_t slen;
	صحيح rc;

	slen = طول_رشته(topic) + گره->topic_len + 2;
	thistopic = mosquitto_malloc(حجم(حرف)*slen);
	إذا(!thistopic){
		أرجع MOSQ_ERR_NOMEM;
	}
	إذا(level > 1 || طول_رشته(topic)){
		snprintf(thistopic, slen, "%s/%s", topic, گره->topic);
	}وإلا{
		snprintf(thistopic, slen, "%s", گره->topic);
	}

	sub = گره->subs;
	طالما(sub){
		إذا(sub->context->session_expiry_interval != MQTT_SESSION_EXPIRY_فوری
				&& sub->context->clean_شروع == false
				&& sub->context->id){

			memset(&sub_chunk, 0, حجم(بنية P_sub));

			sub_chunk.F.identifier = sub->identifier;
			sub_chunk.F.id_len = (uint16_t)طول_رشته(sub->context->id);
			sub_chunk.F.topic_len = (uint16_t)طول_رشته(thistopic);
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
		sub = sub->بعدی;
	}

	HASH_ITER(hh, گره->children, subhier, subhier_tmp){
		persist__subs_save(db_fptr, subhier, thistopic, level+1);
	}
	mosquitto_FREE(thistopic);
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح persist__subs_save_all(FILE *db_fptr)
{
	بنية mosquitto__subhier *subhier, *subhier_tmp;

	HASH_ITER(hh, db.نرمال_subs, subhier, subhier_tmp){
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


ساكن صحيح persist__retain_save(FILE *db_fptr, بنية mosquitto__retainhier *گره, صحيح level)
{
	بنية mosquitto__retainhier *retainhier, *retainhier_tmp;
	بنية P_retain retain_chunk;
	صحيح rc;

	إذا(گره->retained && مقایسه_رشته(گره->retained->داده.topic, "$SYS", 4)){
		memset(&retain_chunk, 0, حجم(بنية P_retain));

		/* Don't save $SYS messages. */
		retain_chunk.F.store_id = گره->retained->داده.store_id;
		rc = persist__chunk_retain_write_v6(db_fptr, &retain_chunk);
		إذا(rc){
			أرجع rc;
		}
	}

	HASH_ITER(hh, گره->children, retainhier, retainhier_tmp){
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

ساكن صحيح persist_write_داده(FILE *db_fptr, فراغ *user_داده);


ساكن فراغ persist__log_write_error(ثابت حرف *پیام)
{
	log__printf(عدم, MOSQ_LOG_ERR, "Error saving in-memory database, %s", پیام);
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

	أرجع mosquitto_write_file(db.config->persistence_filepath, true, &persist_write_داده, &shutdown, &persist__log_write_error);
}


ساكن صحيح persist_write_داده(FILE *db_fptr, فراغ *user_داده)
{
	bool shutdown = *(bool *)(user_داده);
	uint32_t db_نسخه_w = htonl(MOSQ_DB_نسخه);
	uint32_t بررسی_چرخهای = 0;
	ثابت حرف *err;
	بنية PF_cfg cfg_chunk;
	صحيح rc = MOSQ_ERR_UNKNOWN;

	/*سرآیند*/
	write_e(db_fptr, magic, 15);
	write_e(db_fptr, &بررسی_چرخهای, حجم(uint32_t));
	write_e(db_fptr, &db_نسخه_w, حجم(uint32_t));

	memset(&cfg_chunk, 0, حجم(بنية PF_cfg));
	cfg_chunk.آخرین_db_id = db.آخرین_db_id;
	cfg_chunk.shutdown = shutdown;
	cfg_chunk.dbid_اندازه = حجم(dbid_t);
	إذا(persist__chunk_cfg_write_v6(db_fptr, &cfg_chunk)){
		اذهب error;
	}

	إذا(persist_پیام_store_save(db_fptr)){
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


#پایان_اگر
