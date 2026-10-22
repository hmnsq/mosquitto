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

#اگر_تعریف_شدہ WITH_PERSISTENCE

#اگر_غیر_تعریف_شدہ WIN32
#شامل <arpa/inet.h>
#اگر_ختم
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


ساکن صحیح persist__client_messages_save(FILE *db_fptr, ساخت mosquitto *context, ساخت mosquitto_client_پیغام *قطار)
{
	ساخت P_client_پیغام chunk;
	ساخت mosquitto_client_پیغام *cmsg;
	صحیح rc;

	أكد(db_fptr);
	أكد(context);

	cmsg = قطار;
	جب(cmsg){
		اگر(!لڑی_موازنہ(cmsg->بنیاد_پیغام->ڈیٹا.topic, "$SYS", 4)
				&& cmsg->بنیاد_پیغام->ref_گنتی <= 1
				&& cmsg->بنیاد_پیغام->dest_id_گنتی == 0){

			/* This $SYS message won't have been persisted, so we can't persist
			 * this client message. */
			cmsg = cmsg->اگلا;
			جاری;
		}

		memset(&chunk, 0, حجم(ساخت P_client_پیغام));

		chunk.F.store_id = cmsg->بنیاد_پیغام->ڈیٹا.store_id;
		chunk.F.mid = cmsg->ڈیٹا.mid;
		chunk.F.id_len = (uint16_t)لڑی_لمبائی(context->id);
		chunk.F.qos = cmsg->ڈیٹا.qos;
		chunk.F.retain_dup = (uint8_t)((cmsg->ڈیٹا.retain&0x0F)<<4 | (cmsg->ڈیٹا.dup&0x0F));
		chunk.F.direction = (uint8_t)cmsg->ڈیٹا.direction;
		chunk.F.state = (uint8_t)cmsg->ڈیٹا.state;
		chunk.clientid = context->id;
		chunk.subscription_identifier = cmsg->ڈیٹا.subscription_identifier;

		rc = persist_chunk_client_پیغام_write_v6(db_fptr, &chunk);
		اگر(rc){
			واپس rc;
		}

		cmsg = cmsg->اگلا;
	}

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح persist_پیغام_store_save(FILE *db_fptr)
{
	ساخت P_بنیاد_پیغام chunk;
	ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام, *بنیاد_پیغام_tmp;
	صحیح rc;

	أكد(db_fptr);

	بنیاد_پیغام = db.پیغام_store;
	HASH_ITER(hh, db.پیغام_store, بنیاد_پیغام, بنیاد_پیغام_tmp){
		اگر(بنیاد_پیغام->ref_گنتی < 1 || بنیاد_پیغام->ڈیٹا.topic == تہی){
			جاری;
		}

		memset(&chunk, 0, حجم(ساخت P_بنیاد_پیغام));

		اگر(!لڑی_موازنہ(بنیاد_پیغام->ڈیٹا.topic, "$SYS", 4)){
			اگر(بنیاد_پیغام->ref_گنتی <= 1 && بنیاد_پیغام->dest_id_گنتی == 0){
				/* $SYS messages that are only retained shouldn't be persisted. */
				جاری;
			}
			/* Don't save $SYS messages as retained otherwise they can give
			 * misleading information when reloaded. They should still be saved
			 * because a disconnected durable client may have them in their
			 * queue. */
			chunk.F.retain = 0;
		}ورنہ{
			chunk.F.retain = (uint8_t)بنیاد_پیغام->ڈیٹا.retain;
		}

		chunk.F.store_id = بنیاد_پیغام->ڈیٹا.store_id;
		chunk.F.expiry_وقت = بنیاد_پیغام->ڈیٹا.expiry_وقت;
		chunk.F.payloadlen = بنیاد_پیغام->ڈیٹا.payloadlen;
		chunk.F.ماخذ_mid = بنیاد_پیغام->ڈیٹا.ماخذ_mid;
		اگر(بنیاد_پیغام->ڈیٹا.ماخذ_id){
			chunk.F.ماخذ_id_len = (uint16_t)لڑی_لمبائی(بنیاد_پیغام->ڈیٹا.ماخذ_id);
			chunk.ماخذ.id = بنیاد_پیغام->ڈیٹا.ماخذ_id;
		}ورنہ{
			chunk.F.ماخذ_id_len = 0;
			chunk.ماخذ.id = تہی;
		}
		اگر(بنیاد_پیغام->ڈیٹا.ماخذ_username){
			chunk.F.ماخذ_username_len = (uint16_t)لڑی_لمبائی(بنیاد_پیغام->ڈیٹا.ماخذ_username);
			chunk.ماخذ.username = بنیاد_پیغام->ڈیٹا.ماخذ_username;
		}ورنہ{
			chunk.F.ماخذ_username_len = 0;
			chunk.ماخذ.username = تہی;
		}

		chunk.F.topic_len = (uint16_t)لڑی_لمبائی(بنیاد_پیغام->ڈیٹا.topic);
		chunk.topic = بنیاد_پیغام->ڈیٹا.topic;

		اگر(بنیاد_پیغام->ماخذ_listener){
			chunk.F.ماخذ_پورٹ = بنیاد_پیغام->ماخذ_listener->پورٹ;
		}ورنہ{
			chunk.F.ماخذ_پورٹ = 0;
		}
		chunk.F.qos = بنیاد_پیغام->ڈیٹا.qos;
		chunk.payload = بنیاد_پیغام->ڈیٹا.payload;
		chunk.properties = بنیاد_پیغام->ڈیٹا.properties;

		rc = persist_chunk_پیغام_store_write_v6(db_fptr, &chunk);
		اگر(rc){
			واپس rc;
		}
	}

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح persist__client_save(FILE *db_fptr)
{
	ساخت mosquitto *context, *ctxt_tmp;
	ساخت P_client chunk;
	صحیح rc;

	أكد(db_fptr);

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		memset(&chunk, 0, حجم(ساخت P_client));

		اگر(context &&
				context->session_expiry_interval != MQTT_SESSION_EXPIRY_فوری &&
#اگر_تعریف_شدہ WITH_BRIDGE
				((!context->bridge && context->clean_آغاز == false)
				|| (context->bridge && context->bridge->clean_آغاز_local == false))
#ورنہ
				context->clean_آغاز == false
#اگر_ختم
				){
			chunk.F.session_expiry_وقت = context->session_expiry_وقت;
			اگر(context->session_expiry_interval != MQTT_SESSION_EXPIRY_NEVER
					&& context->session_expiry_وقت == 0){

				chunk.F.session_expiry_وقت = context->session_expiry_interval + db.now_real_s;
			}ورنہ{
				chunk.F.session_expiry_وقت = context->session_expiry_وقت;
			}
			chunk.F.session_expiry_interval = context->session_expiry_interval;
			chunk.F.آخری_mid = context->آخری_mid;
			chunk.F.id_len = (uint16_t)لڑی_لمبائی(context->id);
			chunk.clientid = context->id;
			اگر(context->username){
				chunk.F.username_len = (uint16_t)لڑی_لمبائی(context->username);
				chunk.username = context->username;
			}
			اگر(context->listener){
				chunk.F.listener_پورٹ = context->listener->پورٹ;
			}

			اگر(chunk.F.id_len == 0){
				/* This should never happen, but in case we have a client with
				 * zero length ID, don't persist them. */
				جاری;
			}

			rc = persist__chunk_client_write_v6(db_fptr, &chunk);
			اگر(rc){
				واپس rc;
			}

			اگر(persist__client_messages_save(db_fptr, context, context->msgs_اندر.inflight)){
				واپس 1;
			}
			اگر(persist__client_messages_save(db_fptr, context, context->msgs_اندر.queued)){
				واپس 1;
			}
			اگر(persist__client_messages_save(db_fptr, context, context->msgs_برآمد.inflight)){
				واپس 1;
			}
			اگر(persist__client_messages_save(db_fptr, context, context->msgs_برآمد.queued)){
				واپس 1;
			}
		}
	}

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح persist__subs_save(FILE *db_fptr, ساخت mosquitto__subhier *گرہ, ثابت حرف *topic, صحیح level)
{
	ساخت mosquitto__subhier *subhier, *subhier_tmp;
	ساخت mosquitto__subleaf *sub;
	ساخت P_sub sub_chunk;
	حرف *thistopic;
	size_t slen;
	صحیح rc;

	slen = لڑی_لمبائی(topic) + گرہ->topic_len + 2;
	thistopic = mosquitto_malloc(حجم(حرف)*slen);
	اگر(!thistopic){
		واپس MOSQ_ERR_NOMEM;
	}
	اگر(level > 1 || لڑی_لمبائی(topic)){
		snprintf(thistopic, slen, "%s/%s", topic, گرہ->topic);
	}ورنہ{
		snprintf(thistopic, slen, "%s", گرہ->topic);
	}

	sub = گرہ->subs;
	جب(sub){
		اگر(sub->context->session_expiry_interval != MQTT_SESSION_EXPIRY_فوری
				&& sub->context->clean_آغاز == false
				&& sub->context->id){

			memset(&sub_chunk, 0, حجم(ساخت P_sub));

			sub_chunk.F.identifier = sub->identifier;
			sub_chunk.F.id_len = (uint16_t)لڑی_لمبائی(sub->context->id);
			sub_chunk.F.topic_len = (uint16_t)لڑی_لمبائی(thistopic);
			sub_chunk.F.qos = MQTT_SUB_OPT_GET_QOS(sub->subscription_options);
			sub_chunk.F.options = sub->subscription_options & 0xFC;
			sub_chunk.clientid = sub->context->id;
			sub_chunk.topic = thistopic;

			rc = persist__chunk_sub_write_v6(db_fptr, &sub_chunk);
			اگر(rc){
				mosquitto_FREE(thistopic);
				واپس rc;
			}
		}
		sub = sub->اگلا;
	}

	HASH_ITER(hh, گرہ->children, subhier, subhier_tmp){
		persist__subs_save(db_fptr, subhier, thistopic, level+1);
	}
	mosquitto_FREE(thistopic);
	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح persist__subs_save_all(FILE *db_fptr)
{
	ساخت mosquitto__subhier *subhier, *subhier_tmp;

	HASH_ITER(hh, db.معمول_subs, subhier, subhier_tmp){
		اگر(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	HASH_ITER(hh, db.shared_subs, subhier, subhier_tmp){
		اگر(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح persist__retain_save(FILE *db_fptr, ساخت mosquitto__retainhier *گرہ, صحیح level)
{
	ساخت mosquitto__retainhier *retainhier, *retainhier_tmp;
	ساخت P_retain retain_chunk;
	صحیح rc;

	اگر(گرہ->retained && لڑی_موازنہ(گرہ->retained->ڈیٹا.topic, "$SYS", 4)){
		memset(&retain_chunk, 0, حجم(ساخت P_retain));

		/* Don't save $SYS messages. */
		retain_chunk.F.store_id = گرہ->retained->ڈیٹا.store_id;
		rc = persist__chunk_retain_write_v6(db_fptr, &retain_chunk);
		اگر(rc){
			واپس rc;
		}
	}

	HASH_ITER(hh, گرہ->children, retainhier, retainhier_tmp){
		persist__retain_save(db_fptr, retainhier, level+1);
	}
	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح persist__retain_save_all(FILE *db_fptr)
{
	ساخت mosquitto__retainhier *retainhier, *retainhier_tmp;

	HASH_ITER(hh, db.retains, retainhier, retainhier_tmp){
		اگر(retainhier->children){
			persist__retain_save(db_fptr, retainhier->children, 0);
		}
	}

	واپس MOSQ_ERR_SUCCESS;
}

ساکن صحیح persist_write_ڈیٹا(FILE *db_fptr, خالی *user_ڈیٹا);


ساکن خالی persist__log_write_error(ثابت حرف *پیغام)
{
	log__printf(تہی, MOSQ_LOG_ERR, "Error saving in-memory database, %s", پیغام);
}


صحیح persist__backup(bool shutdown)
{
	اگر(db.config == تہی){
		واپس MOSQ_ERR_INVAL;
	}
	اگر(db.config->persistence == false){
		واپس MOSQ_ERR_SUCCESS;
	}
	اگر(db.config->persistence_filepath == تہی){
		واپس MOSQ_ERR_INVAL;
	}

	log__printf(تہی, MOSQ_LOG_INFO, "Saving in-memory database to %s.", db.config->persistence_filepath);

	واپس mosquitto_write_file(db.config->persistence_filepath, true, &persist_write_ڈیٹا, &shutdown, &persist__log_write_error);
}


ساکن صحیح persist_write_ڈیٹا(FILE *db_fptr, خالی *user_ڈیٹا)
{
	bool shutdown = *(bool *)(user_ڈیٹا);
	uint32_t db_نسخہ_w = htonl(MOSQ_DB_نسخہ);
	uint32_t چکری_جانچ = 0;
	ثابت حرف *err;
	ساخت PF_cfg cfg_chunk;
	صحیح rc = MOSQ_ERR_UNKNOWN;

	/*سرخی*/
	write_e(db_fptr, magic, 15);
	write_e(db_fptr, &چکری_جانچ, حجم(uint32_t));
	write_e(db_fptr, &db_نسخہ_w, حجم(uint32_t));

	memset(&cfg_chunk, 0, حجم(ساخت PF_cfg));
	cfg_chunk.آخری_db_id = db.آخری_db_id;
	cfg_chunk.shutdown = shutdown;
	cfg_chunk.dbid_سائز = حجم(dbid_t);
	اگر(persist__chunk_cfg_write_v6(db_fptr, &cfg_chunk)){
		جاؤ error;
	}

	اگر(persist_پیغام_store_save(db_fptr)){
		جاؤ error;
	}

	اگر(persist__client_save(db_fptr)
			|| persist__subs_save_all(db_fptr)
			|| persist__retain_save_all(db_fptr)){
		جاؤ error;
	}
	واپس MOSQ_ERR_SUCCESS;

error:
	err = strerror(errno);
	log__printf(تہی, MOSQ_LOG_ERR, "Error during saving in-memory database %s: %s.", db.config->persistence_filepath, err);
	اگر(db_fptr){
		fclose(db_fptr);
	}
	واپس rc;
}


#اگر_ختم
