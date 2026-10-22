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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_PERSISTENCE

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <time.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "persist.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"


ਸਥਿਰ ਪੂਰਨ persist__client_messages_save(FILE *db_fptr, ਬਣਤਰ mosquitto *context, ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *ਕਤਾਰ)
{
	ਬਣਤਰ P_client_ਸੁਨੇਹਾ chunk;
	ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *cmsg;
	ਪੂਰਨ rc;

	ਪ੍ਰਤਿਜ੍ਞਾ(db_fptr);
	ਪ੍ਰਤਿਜ੍ਞਾ(context);

	cmsg = ਕਤਾਰ;
	ਜਦੋਂ(cmsg){
		ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(cmsg->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic, "$SYS", 4)
				&& cmsg->ਅਧਾਰ_ਸੁਨੇਹਾ->ref_ਗਿਣਤੀ <= 1
				&& cmsg->ਅਧਾਰ_ਸੁਨੇਹਾ->dest_id_ਗਿਣਤੀ == 0){

			/* This $SYS message won't have been persisted, so we can't persist
			 * this client message. */
			cmsg = cmsg->ਅਗਲਾ;
			ਜਾਰੀ;
		}

		memset(&chunk, 0, ਆਕਾਰ(ਬਣਤਰ P_client_ਸੁਨੇਹਾ));

		chunk.F.store_id = cmsg->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.store_id;
		chunk.F.mid = cmsg->ਅੰਕੜੇ.mid;
		chunk.F.id_len = (uint16_t)ਲੜੀ_ਲੰਬਾਈ(context->id);
		chunk.F.qos = cmsg->ਅੰਕੜੇ.qos;
		chunk.F.retain_dup = (uint8_t)((cmsg->ਅੰਕੜੇ.retain&0x0F)<<4 | (cmsg->ਅੰਕੜੇ.dup&0x0F));
		chunk.F.direction = (uint8_t)cmsg->ਅੰਕੜੇ.direction;
		chunk.F.state = (uint8_t)cmsg->ਅੰਕੜੇ.state;
		chunk.clientid = context->id;
		chunk.subscription_identifier = cmsg->ਅੰਕੜੇ.subscription_identifier;

		rc = persist_chunk_client_ਸੁਨੇਹਾ_write_v6(db_fptr, &chunk);
		ਜੇ(rc){
			ਵਾਪਸ rc;
		}

		cmsg = cmsg->ਅਗਲਾ;
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ persist_ਸੁਨੇਹਾ_store_save(FILE *db_fptr)
{
	ਬਣਤਰ P_ਅਧਾਰ_ਸੁਨੇਹਾ chunk;
	ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ, *ਅਧਾਰ_ਸੁਨੇਹਾ_tmp;
	ਪੂਰਨ rc;

	ਪ੍ਰਤਿਜ੍ਞਾ(db_fptr);

	ਅਧਾਰ_ਸੁਨੇਹਾ = db.ਸੁਨੇਹਾ_store;
	HASH_ITER(hh, db.ਸੁਨੇਹਾ_store, ਅਧਾਰ_ਸੁਨੇਹਾ, ਅਧਾਰ_ਸੁਨੇਹਾ_tmp){
		ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ref_ਗਿਣਤੀ < 1 || ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic == ਨਿਰਾ){
			ਜਾਰੀ;
		}

		memset(&chunk, 0, ਆਕਾਰ(ਬਣਤਰ P_ਅਧਾਰ_ਸੁਨੇਹਾ));

		ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic, "$SYS", 4)){
			ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ref_ਗਿਣਤੀ <= 1 && ਅਧਾਰ_ਸੁਨੇਹਾ->dest_id_ਗਿਣਤੀ == 0){
				/* $SYS messages that are only retained shouldn't be persisted. */
				ਜਾਰੀ;
			}
			/* Don't save $SYS messages as retained otherwise they can give
			 * misleading information when reloaded. They should still be saved
			 * because a disconnected durable client may have them in their
			 * queue. */
			chunk.F.retain = 0;
		}ਨਹੀਂਤਾਂ{
			chunk.F.retain = (uint8_t)ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.retain;
		}

		chunk.F.store_id = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.store_id;
		chunk.F.expiry_ਸਮਾਂ = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.expiry_ਸਮਾਂ;
		chunk.F.payloadlen = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen;
		chunk.F.ਸਰੋਤ_mid = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_mid;
		ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_id){
			chunk.F.ਸਰੋਤ_id_len = (uint16_t)ਲੜੀ_ਲੰਬਾਈ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_id);
			chunk.ਸਰੋਤ.id = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_id;
		}ਨਹੀਂਤਾਂ{
			chunk.F.ਸਰੋਤ_id_len = 0;
			chunk.ਸਰੋਤ.id = ਨਿਰਾ;
		}
		ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_username){
			chunk.F.ਸਰੋਤ_username_len = (uint16_t)ਲੜੀ_ਲੰਬਾਈ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_username);
			chunk.ਸਰੋਤ.username = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_username;
		}ਨਹੀਂਤਾਂ{
			chunk.F.ਸਰੋਤ_username_len = 0;
			chunk.ਸਰੋਤ.username = ਨਿਰਾ;
		}

		chunk.F.topic_len = (uint16_t)ਲੜੀ_ਲੰਬਾਈ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic);
		chunk.topic = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic;

		ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਸਰੋਤ_listener){
			chunk.F.ਸਰੋਤ_ਪੋਰਟ = ਅਧਾਰ_ਸੁਨੇਹਾ->ਸਰੋਤ_listener->ਪੋਰਟ;
		}ਨਹੀਂਤਾਂ{
			chunk.F.ਸਰੋਤ_ਪੋਰਟ = 0;
		}
		chunk.F.qos = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos;
		chunk.payload = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payload;
		chunk.properties = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.properties;

		rc = persist_chunk_ਸੁਨੇਹਾ_store_write_v6(db_fptr, &chunk);
		ਜੇ(rc){
			ਵਾਪਸ rc;
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ persist__client_save(FILE *db_fptr)
{
	ਬਣਤਰ mosquitto *context, *ctxt_tmp;
	ਬਣਤਰ P_client chunk;
	ਪੂਰਨ rc;

	ਪ੍ਰਤਿਜ੍ਞਾ(db_fptr);

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		memset(&chunk, 0, ਆਕਾਰ(ਬਣਤਰ P_client));

		ਜੇ(context &&
				context->session_expiry_interval != MQTT_SESSION_EXPIRY_ਤੁਰੰਤ &&
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
				((!context->bridge && context->clean_ਸ਼ੁਰੂ == false)
				|| (context->bridge && context->bridge->clean_ਸ਼ੁਰੂ_local == false))
#ਨਹੀਂਤਾਂ
				context->clean_ਸ਼ੁਰੂ == false
#ਜੇ_ਅੰਤ
				){
			chunk.F.session_expiry_ਸਮਾਂ = context->session_expiry_ਸਮਾਂ;
			ਜੇ(context->session_expiry_interval != MQTT_SESSION_EXPIRY_NEVER
					&& context->session_expiry_ਸਮਾਂ == 0){

				chunk.F.session_expiry_ਸਮਾਂ = context->session_expiry_interval + db.now_real_s;
			}ਨਹੀਂਤਾਂ{
				chunk.F.session_expiry_ਸਮਾਂ = context->session_expiry_ਸਮਾਂ;
			}
			chunk.F.session_expiry_interval = context->session_expiry_interval;
			chunk.F.ਆਖਰੀ_mid = context->ਆਖਰੀ_mid;
			chunk.F.id_len = (uint16_t)ਲੜੀ_ਲੰਬਾਈ(context->id);
			chunk.clientid = context->id;
			ਜੇ(context->username){
				chunk.F.username_len = (uint16_t)ਲੜੀ_ਲੰਬਾਈ(context->username);
				chunk.username = context->username;
			}
			ਜੇ(context->listener){
				chunk.F.listener_ਪੋਰਟ = context->listener->ਪੋਰਟ;
			}

			ਜੇ(chunk.F.id_len == 0){
				/* This should never happen, but in case we have a client with
				 * zero length ID, don't persist them. */
				ਜਾਰੀ;
			}

			rc = persist__chunk_client_write_v6(db_fptr, &chunk);
			ਜੇ(rc){
				ਵਾਪਸ rc;
			}

			ਜੇ(persist__client_messages_save(db_fptr, context, context->msgs_ਵਿੱਚ.inflight)){
				ਵਾਪਸ 1;
			}
			ਜੇ(persist__client_messages_save(db_fptr, context, context->msgs_ਵਿੱਚ.queued)){
				ਵਾਪਸ 1;
			}
			ਜੇ(persist__client_messages_save(db_fptr, context, context->msgs_ਬਾਹਰਲਾ.inflight)){
				ਵਾਪਸ 1;
			}
			ਜੇ(persist__client_messages_save(db_fptr, context, context->msgs_ਬਾਹਰਲਾ.queued)){
				ਵਾਪਸ 1;
			}
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ persist__subs_save(FILE *db_fptr, ਬਣਤਰ mosquitto__subhier *ਗੰਢ, ਅਚਲ ਅੱਖਰ *topic, ਪੂਰਨ level)
{
	ਬਣਤਰ mosquitto__subhier *subhier, *subhier_tmp;
	ਬਣਤਰ mosquitto__subleaf *sub;
	ਬਣਤਰ P_sub sub_chunk;
	ਅੱਖਰ *thistopic;
	size_t slen;
	ਪੂਰਨ rc;

	slen = ਲੜੀ_ਲੰਬਾਈ(topic) + ਗੰਢ->topic_len + 2;
	thistopic = mosquitto_malloc(ਆਕਾਰ(ਅੱਖਰ)*slen);
	ਜੇ(!thistopic){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}
	ਜੇ(level > 1 || ਲੜੀ_ਲੰਬਾਈ(topic)){
		snprintf(thistopic, slen, "%s/%s", topic, ਗੰਢ->topic);
	}ਨਹੀਂਤਾਂ{
		snprintf(thistopic, slen, "%s", ਗੰਢ->topic);
	}

	sub = ਗੰਢ->subs;
	ਜਦੋਂ(sub){
		ਜੇ(sub->context->session_expiry_interval != MQTT_SESSION_EXPIRY_ਤੁਰੰਤ
				&& sub->context->clean_ਸ਼ੁਰੂ == false
				&& sub->context->id){

			memset(&sub_chunk, 0, ਆਕਾਰ(ਬਣਤਰ P_sub));

			sub_chunk.F.identifier = sub->identifier;
			sub_chunk.F.id_len = (uint16_t)ਲੜੀ_ਲੰਬਾਈ(sub->context->id);
			sub_chunk.F.topic_len = (uint16_t)ਲੜੀ_ਲੰਬਾਈ(thistopic);
			sub_chunk.F.qos = MQTT_SUB_OPT_GET_QOS(sub->subscription_options);
			sub_chunk.F.options = sub->subscription_options & 0xFC;
			sub_chunk.clientid = sub->context->id;
			sub_chunk.topic = thistopic;

			rc = persist__chunk_sub_write_v6(db_fptr, &sub_chunk);
			ਜੇ(rc){
				mosquitto_FREE(thistopic);
				ਵਾਪਸ rc;
			}
		}
		sub = sub->ਅਗਲਾ;
	}

	HASH_ITER(hh, ਗੰਢ->children, subhier, subhier_tmp){
		persist__subs_save(db_fptr, subhier, thistopic, level+1);
	}
	mosquitto_FREE(thistopic);
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ persist__subs_save_all(FILE *db_fptr)
{
	ਬਣਤਰ mosquitto__subhier *subhier, *subhier_tmp;

	HASH_ITER(hh, db.ਸਾਧਾਰਨ_subs, subhier, subhier_tmp){
		ਜੇ(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	HASH_ITER(hh, db.shared_subs, subhier, subhier_tmp){
		ਜੇ(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ persist__retain_save(FILE *db_fptr, ਬਣਤਰ mosquitto__retainhier *ਗੰਢ, ਪੂਰਨ level)
{
	ਬਣਤਰ mosquitto__retainhier *retainhier, *retainhier_tmp;
	ਬਣਤਰ P_retain retain_chunk;
	ਪੂਰਨ rc;

	ਜੇ(ਗੰਢ->retained && ਲੜੀ_ਤੁਲਨਾ(ਗੰਢ->retained->ਅੰਕੜੇ.topic, "$SYS", 4)){
		memset(&retain_chunk, 0, ਆਕਾਰ(ਬਣਤਰ P_retain));

		/* Don't save $SYS messages. */
		retain_chunk.F.store_id = ਗੰਢ->retained->ਅੰਕੜੇ.store_id;
		rc = persist__chunk_retain_write_v6(db_fptr, &retain_chunk);
		ਜੇ(rc){
			ਵਾਪਸ rc;
		}
	}

	HASH_ITER(hh, ਗੰਢ->children, retainhier, retainhier_tmp){
		persist__retain_save(db_fptr, retainhier, level+1);
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ persist__retain_save_all(FILE *db_fptr)
{
	ਬਣਤਰ mosquitto__retainhier *retainhier, *retainhier_tmp;

	HASH_ITER(hh, db.retains, retainhier, retainhier_tmp){
		ਜੇ(retainhier->children){
			persist__retain_save(db_fptr, retainhier->children, 0);
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}

ਸਥਿਰ ਪੂਰਨ persist_write_ਅੰਕੜੇ(FILE *db_fptr, ਖਾਲੀ *user_ਅੰਕੜੇ);


ਸਥਿਰ ਖਾਲੀ persist__log_write_error(ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ)
{
	log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error saving in-memory database, %s", ਸੁਨੇਹਾ);
}


ਪੂਰਨ persist__backup(bool shutdown)
{
	ਜੇ(db.config == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}
	ਜੇ(db.config->persistence == false){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}
	ਜੇ(db.config->persistence_filepath == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Saving in-memory database to %s.", db.config->persistence_filepath);

	ਵਾਪਸ mosquitto_write_file(db.config->persistence_filepath, true, &persist_write_ਅੰਕੜੇ, &shutdown, &persist__log_write_error);
}


ਸਥਿਰ ਪੂਰਨ persist_write_ਅੰਕੜੇ(FILE *db_fptr, ਖਾਲੀ *user_ਅੰਕੜੇ)
{
	bool shutdown = *(bool *)(user_ਅੰਕੜੇ);
	uint32_t db_ਸੰਸਕਰਣ_w = htonl(MOSQ_DB_ਸੰਸਕਰਣ);
	uint32_t ਚੱਕਰੀ_ਜਾਂਚ = 0;
	ਅਚਲ ਅੱਖਰ *err;
	ਬਣਤਰ PF_cfg cfg_chunk;
	ਪੂਰਨ rc = MOSQ_ERR_UNKNOWN;

	/*ਸਿਰਲੇਖ*/
	write_e(db_fptr, magic, 15);
	write_e(db_fptr, &ਚੱਕਰੀ_ਜਾਂਚ, ਆਕਾਰ(uint32_t));
	write_e(db_fptr, &db_ਸੰਸਕਰਣ_w, ਆਕਾਰ(uint32_t));

	memset(&cfg_chunk, 0, ਆਕਾਰ(ਬਣਤਰ PF_cfg));
	cfg_chunk.ਆਖਰੀ_db_id = db.ਆਖਰੀ_db_id;
	cfg_chunk.shutdown = shutdown;
	cfg_chunk.dbid_ਮਾਪ = ਆਕਾਰ(dbid_t);
	ਜੇ(persist__chunk_cfg_write_v6(db_fptr, &cfg_chunk)){
		ਜਾਓ error;
	}

	ਜੇ(persist_ਸੁਨੇਹਾ_store_save(db_fptr)){
		ਜਾਓ error;
	}

	ਜੇ(persist__client_save(db_fptr)
			|| persist__subs_save_all(db_fptr)
			|| persist__retain_save_all(db_fptr)){
		ਜਾਓ error;
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;

error:
	err = strerror(errno);
	log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error during saving in-memory database %s: %s.", db.config->persistence_filepath, err);
	ਜੇ(db_fptr){
		fclose(db_fptr);
	}
	ਵਾਪਸ rc;
}


#ਜੇ_ਅੰਤ
