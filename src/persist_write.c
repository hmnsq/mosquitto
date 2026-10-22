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

#جے_تعریف_شدہ WITH_PERSISTENCE

#جے_بنا_تعریف WIN32
#ضمن <arpa/inet.h>
#جے_انت
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


ساكن صحيح persist__client_messages_save(FILE *db_fptr, بنية mosquitto *context, بنية mosquitto_client_پیغام *قطار)
{
	بنية P_client_پیغام chunk;
	بنية mosquitto_client_پیغام *cmsg;
	صحيح rc;

	أكد(db_fptr);
	أكد(context);

	cmsg = قطار;
	طالما(cmsg){
		إذا(!لڑی_مقابلہ(cmsg->ادھار_پیغام->انکڑے.topic, "$SYS", 4)
				&& cmsg->ادھار_پیغام->ref_گنتی <= 1
				&& cmsg->ادھار_پیغام->dest_id_گنتی == 0){

			/* This $SYS message won't have been persisted, so we can't persist
			 * this client message. */
			cmsg = cmsg->اگلا;
			استمر;
		}

		memset(&chunk, 0, حجم(بنية P_client_پیغام));

		chunk.F.store_id = cmsg->ادھار_پیغام->انکڑے.store_id;
		chunk.F.mid = cmsg->انکڑے.mid;
		chunk.F.id_len = (uint16_t)لڑی_لمبائی(context->id);
		chunk.F.qos = cmsg->انکڑے.qos;
		chunk.F.retain_dup = (uint8_t)((cmsg->انکڑے.retain&0x0F)<<4 | (cmsg->انکڑے.dup&0x0F));
		chunk.F.direction = (uint8_t)cmsg->انکڑے.direction;
		chunk.F.state = (uint8_t)cmsg->انکڑے.state;
		chunk.clientid = context->id;
		chunk.subscription_identifier = cmsg->انکڑے.subscription_identifier;

		rc = persist_chunk_client_پیغام_write_v6(db_fptr, &chunk);
		إذا(rc){
			أرجع rc;
		}

		cmsg = cmsg->اگلا;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح persist_سنیہا_store_save(FILE *db_fptr)
{
	بنية P_ادھار_پیغام chunk;
	بنية mosquitto_ادھار_پیغام *ادھار_پیغام, *ادھار_پیغام_tmp;
	صحيح rc;

	أكد(db_fptr);

	ادھار_پیغام = db.پیغام_store;
	HASH_ITER(hh, db.پیغام_store, ادھار_پیغام, ادھار_پیغام_tmp){
		إذا(ادھار_پیغام->ref_گنتی < 1 || ادھار_پیغام->انکڑے.topic == عدم){
			استمر;
		}

		memset(&chunk, 0, حجم(بنية P_ادھار_پیغام));

		إذا(!لڑی_مقابلہ(ادھار_پیغام->انکڑے.topic, "$SYS", 4)){
			إذا(ادھار_پیغام->ref_گنتی <= 1 && ادھار_پیغام->dest_id_گنتی == 0){
				/* $SYS messages that are only retained shouldn't be persisted. */
				استمر;
			}
			/* Don't save $SYS messages as retained otherwise they can give
			 * misleading information when reloaded. They should still be saved
			 * because a disconnected durable client may have them in their
			 * queue. */
			chunk.F.retain = 0;
		}وإلا{
			chunk.F.retain = (uint8_t)ادھار_پیغام->انکڑے.retain;
		}

		chunk.F.store_id = ادھار_پیغام->انکڑے.store_id;
		chunk.F.expiry_ویلا = ادھار_پیغام->انکڑے.expiry_ویلا;
		chunk.F.payloadlen = ادھار_پیغام->انکڑے.payloadlen;
		chunk.F.سروت_mid = ادھار_پیغام->انکڑے.سروت_mid;
		إذا(ادھار_پیغام->انکڑے.سروت_id){
			chunk.F.سروت_id_len = (uint16_t)لڑی_لمبائی(ادھار_پیغام->انکڑے.سروت_id);
			chunk.سروت.id = ادھار_پیغام->انکڑے.سروت_id;
		}وإلا{
			chunk.F.سروت_id_len = 0;
			chunk.سروت.id = عدم;
		}
		إذا(ادھار_پیغام->انکڑے.سروت_username){
			chunk.F.سروت_username_len = (uint16_t)لڑی_لمبائی(ادھار_پیغام->انکڑے.سروت_username);
			chunk.سروت.username = ادھار_پیغام->انکڑے.سروت_username;
		}وإلا{
			chunk.F.سروت_username_len = 0;
			chunk.سروت.username = عدم;
		}

		chunk.F.topic_len = (uint16_t)لڑی_لمبائی(ادھار_پیغام->انکڑے.topic);
		chunk.topic = ادھار_پیغام->انکڑے.topic;

		إذا(ادھار_پیغام->سروت_listener){
			chunk.F.سروت_پورٹ = ادھار_پیغام->سروت_listener->پورٹ;
		}وإلا{
			chunk.F.سروت_پورٹ = 0;
		}
		chunk.F.qos = ادھار_پیغام->انکڑے.qos;
		chunk.payload = ادھار_پیغام->انکڑے.payload;
		chunk.properties = ادھار_پیغام->انکڑے.properties;

		rc = persist_chunk_سنیہا_store_write_v6(db_fptr, &chunk);
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
#جے_تعریف_شدہ WITH_BRIDGE
				((!context->bridge && context->clean_شروع == false)
				|| (context->bridge && context->bridge->clean_شروع_local == false))
#وإلا
				context->clean_شروع == false
#جے_انت
				){
			chunk.F.session_expiry_ویلا = context->session_expiry_ویلا;
			إذا(context->session_expiry_interval != MQTT_SESSION_EXPIRY_NEVER
					&& context->session_expiry_ویلا == 0){

				chunk.F.session_expiry_ویلا = context->session_expiry_interval + db.now_real_s;
			}وإلا{
				chunk.F.session_expiry_ویلا = context->session_expiry_ویلا;
			}
			chunk.F.session_expiry_interval = context->session_expiry_interval;
			chunk.F.آخری_mid = context->آخری_mid;
			chunk.F.id_len = (uint16_t)لڑی_لمبائی(context->id);
			chunk.clientid = context->id;
			إذا(context->username){
				chunk.F.username_len = (uint16_t)لڑی_لمبائی(context->username);
				chunk.username = context->username;
			}
			إذا(context->listener){
				chunk.F.listener_پورٹ = context->listener->پورٹ;
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

			إذا(persist__client_messages_save(db_fptr, context, context->msgs_وچ.inflight)){
				أرجع 1;
			}
			إذا(persist__client_messages_save(db_fptr, context, context->msgs_وچ.queued)){
				أرجع 1;
			}
			إذا(persist__client_messages_save(db_fptr, context, context->msgs_باہرلا.inflight)){
				أرجع 1;
			}
			إذا(persist__client_messages_save(db_fptr, context, context->msgs_باہرلا.queued)){
				أرجع 1;
			}
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح persist__subs_save(FILE *db_fptr, بنية mosquitto__subhier *گرہ, ثابت حرف *topic, صحيح level)
{
	بنية mosquitto__subhier *subhier, *subhier_tmp;
	بنية mosquitto__subleaf *sub;
	بنية P_sub sub_chunk;
	حرف *thistopic;
	size_t slen;
	صحيح rc;

	slen = لڑی_لمبائی(topic) + گرہ->topic_len + 2;
	thistopic = mosquitto_malloc(حجم(حرف)*slen);
	إذا(!thistopic){
		أرجع MOSQ_ERR_NOMEM;
	}
	إذا(level > 1 || لڑی_لمبائی(topic)){
		snprintf(thistopic, slen, "%s/%s", topic, گرہ->topic);
	}وإلا{
		snprintf(thistopic, slen, "%s", گرہ->topic);
	}

	sub = گرہ->subs;
	طالما(sub){
		إذا(sub->context->session_expiry_interval != MQTT_SESSION_EXPIRY_فوری
				&& sub->context->clean_شروع == false
				&& sub->context->id){

			memset(&sub_chunk, 0, حجم(بنية P_sub));

			sub_chunk.F.identifier = sub->identifier;
			sub_chunk.F.id_len = (uint16_t)لڑی_لمبائی(sub->context->id);
			sub_chunk.F.topic_len = (uint16_t)لڑی_لمبائی(thistopic);
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
		sub = sub->اگلا;
	}

	HASH_ITER(hh, گرہ->children, subhier, subhier_tmp){
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


ساكن صحيح persist__retain_save(FILE *db_fptr, بنية mosquitto__retainhier *گرہ, صحيح level)
{
	بنية mosquitto__retainhier *retainhier, *retainhier_tmp;
	بنية P_retain retain_chunk;
	صحيح rc;

	إذا(گرہ->retained && لڑی_مقابلہ(گرہ->retained->انکڑے.topic, "$SYS", 4)){
		memset(&retain_chunk, 0, حجم(بنية P_retain));

		/* Don't save $SYS messages. */
		retain_chunk.F.store_id = گرہ->retained->انکڑے.store_id;
		rc = persist__chunk_retain_write_v6(db_fptr, &retain_chunk);
		إذا(rc){
			أرجع rc;
		}
	}

	HASH_ITER(hh, گرہ->children, retainhier, retainhier_tmp){
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

ساكن صحيح persist_write_انکڑے(FILE *db_fptr, فراغ *user_انکڑے);


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

	أرجع mosquitto_write_file(db.config->persistence_filepath, true, &persist_write_انکڑے, &shutdown, &persist__log_write_error);
}


ساكن صحيح persist_write_انکڑے(FILE *db_fptr, فراغ *user_انکڑے)
{
	bool shutdown = *(bool *)(user_انکڑے);
	uint32_t db_نسخہ_w = htonl(MOSQ_DB_نسخہ);
	uint32_t چکری_جانچ = 0;
	ثابت حرف *err;
	بنية PF_cfg cfg_chunk;
	صحيح rc = MOSQ_ERR_UNKNOWN;

	/*سرخی*/
	write_e(db_fptr, magic, 15);
	write_e(db_fptr, &چکری_جانچ, حجم(uint32_t));
	write_e(db_fptr, &db_نسخہ_w, حجم(uint32_t));

	memset(&cfg_chunk, 0, حجم(بنية PF_cfg));
	cfg_chunk.آخری_db_id = db.آخری_db_id;
	cfg_chunk.shutdown = shutdown;
	cfg_chunk.dbid_ناپ = حجم(dbid_t);
	إذا(persist__chunk_cfg_write_v6(db_fptr, &cfg_chunk)){
		اذهب error;
	}

	إذا(persist_سنیہا_store_save(db_fptr)){
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


#جے_انت
