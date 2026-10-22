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
#ਅਨ੍ਤਰ੍ਭਾਵ <utlist.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "persist.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"

uint32_t db_ਸੰਸਕਰਣ;

ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ magic[15] = {0x00, 0xB5, 0x00, 'm', 'o', 's', 'q', 'u', 'i', 't', 't', 'o', ' ', 'd', 'b'};
ਸਥਿਰ ਲੰਮਾ ਅਧਾਰ_ਸੁਨੇਹਾ_ਗਿਣਤੀ = 0;
ਸਥਿਰ ਲੰਮਾ retained_ਗਿਣਤੀ = 0;
ਸਥਿਰ ਲੰਮਾ client_ਗਿਣਤੀ = 0;
ਸਥਿਰ ਲੰਮਾ subscription_ਗਿਣਤੀ = 0;
ਸਥਿਰ ਲੰਮਾ client_ਸੁਨੇਹਾ_ਗਿਣਤੀ = 0;

ਸਥਿਰ ਪੂਰਨ persist__restore_sub(ਅਚਲ ਬਣਤਰ mosquitto_subscription *sub);

ਸਥਿਰ ਬਣਤਰ mosquitto *persist_ਲੱਭੋ_or_ਜੋੜੋ_context(ਅਚਲ ਅੱਖਰ *clientid, uint16_t ਆਖਰੀ_mid)
{
	ਬਣਤਰ mosquitto *context;

	ਜੇ(!clientid){
		ਵਾਪਸ ਨਿਰਾ;
	}

	context = ਨਿਰਾ;
	HASH_ਲੱਭੋ(hh_id, db.contexts_by_id, clientid, ਲੜੀ_ਲੰਬਾਈ(clientid), context);
	ਜੇ(!context){
		context = context__init();
		ਜੇ(!context){
			ਵਾਪਸ ਨਿਰਾ;
		}
		context->id = mosquitto_strdup(clientid);
		ਜੇ(!context->id){
			mosquitto_FREE(context);
			ਵਾਪਸ ਨਿਰਾ;
		}

		context->clean_ਸ਼ੁਰੂ = false;

		context_ਜੋੜੋ_ਮੰਜ਼ਿਲ_by_id(context);
	}
	ਜੇ(ਆਖਰੀ_mid){
		context->ਆਖਰੀ_mid = ਆਖਰੀ_mid;
	}
	ਵਾਪਸ context;
}


ਪੂਰਨ persist__read_string_len(FILE *db_fptr, ਅੱਖਰ **str, uint16_t len)
{
	ਅੱਖਰ *s = ਨਿਰਾ;

	ਜੇ(len){
		s = mosquitto_malloc(len+1U);
		ਜੇ(!s){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}
		ਜੇ(fread(s, 1, len, db_fptr) != len){
			mosquitto_FREE(s);
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}
		s[len] = '\0';
	}

	*str = s;
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ persist__read_string(FILE *db_fptr, ਅੱਖਰ **str)
{
	uint16_t i16temp;
	uint16_t slen;

	ਜੇ(fread(&i16temp, 1, ਆਕਾਰ(uint16_t), db_fptr) != ਆਕਾਰ(uint16_t)){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	slen = ntohs(i16temp);
	ਵਾਪਸ persist__read_string_len(db_fptr, str, slen);
}


ਸਥਿਰ ਪੂਰਨ persist_client_ਸੁਨੇਹਾ_restore(ਬਣਤਰ P_client_ਸੁਨੇਹਾ *chunk)
{
	ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *cmsg;
	ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਸੁਨੇਹਾ;
	ਬਣਤਰ mosquitto *context;
	ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_ਅੰਕੜੇ *ਸੁਨੇਹਾ_ਅੰਕੜੇ;

	HASH_ਲੱਭੋ(hh, db.ਸੁਨੇਹਾ_store, &chunk->F.store_id, ਆਕਾਰ(chunk->F.store_id), ਸੁਨੇਹਾ);
	ਜੇ(!ਸੁਨੇਹਾ){
		/* Can't find message - probably expired */
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

	context = persist_ਲੱਭੋ_or_ਜੋੜੋ_context(chunk->clientid, 0);
	ਜੇ(!context){
		log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Persistence file contains client message with no matching client. File may be corrupt.");
		ਵਾਪਸ 0;
	}

	cmsg = mosquitto_calloc(1, ਆਕਾਰ(ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ));
	ਜੇ(!cmsg){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	cmsg->ਅਗਲਾ = ਨਿਰਾ;
	cmsg->ਅਧਾਰ_ਸੁਨੇਹਾ = ਨਿਰਾ;
	cmsg->ਅੰਕੜੇ.cmsg_id = ++context->ਆਖਰੀ_cmsg_id;
	cmsg->ਅੰਕੜੇ.mid = chunk->F.mid;
	cmsg->ਅੰਕੜੇ.qos = chunk->F.qos;
	cmsg->ਅੰਕੜੇ.retain = (chunk->F.retain_dup&0xF0)>>4;
	cmsg->ਅੰਕੜੇ.direction = chunk->F.direction;
	cmsg->ਅੰਕੜੇ.state = chunk->F.state;
	cmsg->ਅੰਕੜੇ.dup = chunk->F.retain_dup&0x0F;
	cmsg->ਅੰਕੜੇ.subscription_identifier = chunk->subscription_identifier;

	cmsg->ਅਧਾਰ_ਸੁਨੇਹਾ = ਸੁਨੇਹਾ;
	db_ਸੁਨੇਹਾ_store_ref_inc(cmsg->ਅਧਾਰ_ਸੁਨੇਹਾ);

	ਜੇ(cmsg->ਅੰਕੜੇ.direction == mosq_md_ਬਾਹਰਲਾ){
		ਸੁਨੇਹਾ_ਅੰਕੜੇ = &context->msgs_ਬਾਹਰਲਾ;
	}ਨਹੀਂਤਾਂ{
		ਸੁਨੇਹਾ_ਅੰਕੜੇ = &context->msgs_ਵਿੱਚ;
	}

	ਜੇ(chunk->F.state == mosq_ms_queued || (chunk->F.qos > 0 && ਸੁਨੇਹਾ_ਅੰਕੜੇ->inflight_quota == 0)){
		DL_APPEND(ਸੁਨੇਹਾ_ਅੰਕੜੇ->queued, cmsg);
		db_ਸੁਨੇਹਾ_ਜੋੜੋ_ਮੰਜ਼ਿਲ_queued_stats(ਸੁਨੇਹਾ_ਅੰਕੜੇ, cmsg);
	}ਨਹੀਂਤਾਂ{
		DL_APPEND(ਸੁਨੇਹਾ_ਅੰਕੜੇ->inflight, cmsg);
		ਜੇ(chunk->F.qos > 0 && ਸੁਨੇਹਾ_ਅੰਕੜੇ->inflight_quota > 0){
			ਸੁਨੇਹਾ_ਅੰਕੜੇ->inflight_quota--;
		}
		db_ਸੁਨੇਹਾ_ਜੋੜੋ_ਮੰਜ਼ਿਲ_inflight_stats(ਸੁਨੇਹਾ_ਅੰਕੜੇ, cmsg);
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ persist__client_chunk_restore(FILE *db_fptr)
{
	ਪੂਰਨ rc = 0;
	ਬਣਤਰ mosquitto *context;
	ਬਣਤਰ P_client chunk;

	memset(&chunk, 0, ਆਕਾਰ(ਬਣਤਰ P_client));

	ਜੇ(db_ਸੰਸਕਰਣ == 6 || db_ਸੰਸਕਰਣ == 5){
		rc = persist__chunk_client_read_v56(db_fptr, &chunk, db_ਸੰਸਕਰਣ);
	}ਨਹੀਂਤਾਂ{
		rc = persist__chunk_client_read_v234(db_fptr, &chunk, db_ਸੰਸਕਰਣ);
	}
	ਜੇ(rc > 0){
		ਵਾਪਸ rc;
	}ਨਹੀਂਤਾਂ ਜੇ(rc < 0){
		/* Client not loaded, but otherwise not an error */
		log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Empty client entry found in persistence database, it may be corrupt.");
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

	context = persist_ਲੱਭੋ_or_ਜੋੜੋ_context(chunk.clientid, chunk.F.ਆਖਰੀ_mid);
	ਜੇ(context){
		context->session_expiry_ਸਮਾਂ = chunk.F.session_expiry_ਸਮਾਂ;
		context->session_expiry_interval = chunk.F.session_expiry_interval;
		ਜੇ(chunk.username && !context->username){
			/* username is not freed here, it is now owned by context */
			context->username = chunk.username;
			chunk.username = ਨਿਰਾ;
		}
		/* in per_listener_settings mode, try to find the listener by persisted port */
		ਜੇ(db.config->ਪ੍ਰਤੀ_listener_settings && !context->listener && chunk.F.listener_ਪੋਰਟ > 0){
			ਲਈ(ਪੂਰਨ i=0; i < db.config->listener_ਗਿਣਤੀ; i++){
				ਜੇ(db.config->listeners[i].ਪੋਰਟ == chunk.F.listener_ਪੋਰਟ){
					context->listener = &db.config->listeners[i];
					ਰੋਕੋ;
				}
			}
		}
		session_expiry_ਜੋੜੋ_from_persistence(context, chunk.F.session_expiry_ਸਮਾਂ);
	}ਨਹੀਂਤਾਂ{
		rc = 1;
	}

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.username);
	ਜੇ(rc == 0){
		client_ਗਿਣਤੀ++;
	}
	ਵਾਪਸ rc;
}


ਸਥਿਰ ਪੂਰਨ persist_client_ਸੁਨੇਹਾ_chunk_restore(FILE *db_fptr, uint32_t ਲੰਮਾਈ)
{
	ਬਣਤਰ P_client_ਸੁਨੇਹਾ chunk;
	ਪੂਰਨ rc;

	memset(&chunk, 0, ਆਕਾਰ(ਬਣਤਰ P_client_ਸੁਨੇਹਾ));

	ਜੇ(db_ਸੰਸਕਰਣ == 6 || db_ਸੰਸਕਰਣ == 5){
		rc = persist_chunk_client_ਸੁਨੇਹਾ_read_v56(db_fptr, &chunk, ਲੰਮਾਈ);
	}ਨਹੀਂਤਾਂ{
		rc = persist_chunk_client_ਸੁਨੇਹਾ_read_v234(db_fptr, &chunk);
	}
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}

	rc = persist_client_ਸੁਨੇਹਾ_restore(&chunk);
	mosquitto_FREE(chunk.clientid);

	ਜੇ(rc == 0){
		client_ਸੁਨੇਹਾ_ਗਿਣਤੀ++;
	}
	ਵਾਪਸ rc;
}


ਸਥਿਰ ਪੂਰਨ persist_ਅਧਾਰ_ਸੁਨੇਹਾ_chunk_restore(FILE *db_fptr, uint32_t ਲੰਮਾਈ)
{
	ਬਣਤਰ P_ਅਧਾਰ_ਸੁਨੇਹਾ chunk;
	ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ = ਨਿਰਾ;
	int64_t ਸੁਨੇਹਾ_expiry_interval64;
	uint32_t ਸੁਨੇਹਾ_expiry_interval;
	uint32_t *p_ਸੁਨੇਹਾ_expiry_interval = ਨਿਰਾ;
	ਪੂਰਨ rc = 0;

	memset(&chunk, 0, ਆਕਾਰ(ਬਣਤਰ P_ਅਧਾਰ_ਸੁਨੇਹਾ));

	ਜੇ(db_ਸੰਸਕਰਣ == 6 || db_ਸੰਸਕਰਣ == 5){
		rc = persist_chunk_ਅਧਾਰ_ਸੁਨੇਹਾ_read_v56(db_fptr, &chunk, ਲੰਮਾਈ);
	}ਨਹੀਂਤਾਂ{
		rc = persist_chunk_ਅਧਾਰ_ਸੁਨੇਹਾ_read_v234(db_fptr, &chunk, db_ਸੰਸਕਰਣ);
	}
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}
	ਜੇ(chunk.F.topic_len == 0){
		rc = MOSQ_ERR_INVAL;
		ਜਾਓ cleanup;
	}

	ਜੇ(chunk.F.ਸਰੋਤ_ਪੋਰਟ){
		ਲਈ(ਪੂਰਨ i=0; i<db.config->listener_ਗਿਣਤੀ; i++){
			ਜੇ(db.config->listeners[i].ਪੋਰਟ == chunk.F.ਸਰੋਤ_ਪੋਰਟ){
				chunk.ਸਰੋਤ.listener = &db.config->listeners[i];
				ਰੋਕੋ;
			}
		}
	}

	ਜੇ(chunk.F.expiry_ਸਮਾਂ > 0){
		ਸੁਨੇਹਾ_expiry_interval64 = chunk.F.expiry_ਸਮਾਂ - ਸਮਾਂ(ਨਿਰਾ);
		ਜੇ(ਸੁਨੇਹਾ_expiry_interval64 < 0 || ਸੁਨੇਹਾ_expiry_interval64 > UINT32_MAX){
			/* Expired message */
			rc = MOSQ_ERR_SUCCESS;
			ਜਾਓ cleanup;
		}ਨਹੀਂਤਾਂ{
			ਸੁਨੇਹਾ_expiry_interval = (uint32_t)ਸੁਨੇਹਾ_expiry_interval64;
		}
		p_ਸੁਨੇਹਾ_expiry_interval = &ਸੁਨੇਹਾ_expiry_interval;
	}

	ਅਧਾਰ_ਸੁਨੇਹਾ = mosquitto_calloc(1, ਆਕਾਰ(ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ));
	ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ == ਨਿਰਾ){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
		rc = MOSQ_ERR_NOMEM;
		ਜਾਓ cleanup;
	}

	ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.store_id = chunk.F.store_id;
	ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_mid = chunk.F.ਸਰੋਤ_mid;
	ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic = chunk.topic;
	ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos = chunk.F.qos;
	ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen = chunk.F.payloadlen;
	ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.retain = chunk.F.retain;
	ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.properties = chunk.properties;
	ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payload = chunk.payload;
	ਅਧਾਰ_ਸੁਨੇਹਾ->ਸਰੋਤ_listener = chunk.ਸਰੋਤ.listener;

	rc = db_ਸੁਨੇਹਾ_store(&chunk.ਸਰੋਤ, ਅਧਾਰ_ਸੁਨੇਹਾ, p_ਸੁਨੇਹਾ_expiry_interval,
			mosq_mo_client);

	mosquitto_FREE(chunk.ਸਰੋਤ.id);
	mosquitto_FREE(chunk.ਸਰੋਤ.username);

	ਜੇ(rc == MOSQ_ERR_SUCCESS){
		ਅਧਾਰ_ਸੁਨੇਹਾ_ਗਿਣਤੀ++;
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ rc;
	}
cleanup:
	mosquitto_FREE(chunk.ਸਰੋਤ.id);
	mosquitto_FREE(chunk.ਸਰੋਤ.username);
	mosquitto_FREE(chunk.topic);
	mosquitto_FREE(chunk.payload);
	ਵਾਪਸ rc;
}


ਸਥਿਰ ਪੂਰਨ persist__retain_chunk_restore(FILE *db_fptr)
{
	ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ;
	ਬਣਤਰ P_retain chunk;
	ਪੂਰਨ rc;
	ਅੱਖਰ **ਵੰਡੋ_topics;
	ਅੱਖਰ *local_topic;

	memset(&chunk, 0, ਆਕਾਰ(ਬਣਤਰ P_retain));

	ਜੇ(db_ਸੰਸਕਰਣ == 6 || db_ਸੰਸਕਰਣ == 5){
		rc = persist__chunk_retain_read_v56(db_fptr, &chunk);
	}ਨਹੀਂਤਾਂ{
		rc = persist__chunk_retain_read_v234(db_fptr, &chunk);
	}
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}

	HASH_ਲੱਭੋ(hh, db.ਸੁਨੇਹਾ_store, &chunk.F.store_id, ਆਕਾਰ(chunk.F.store_id), ਅਧਾਰ_ਸੁਨੇਹਾ);
	ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ){
		rc = sub__topic_tokenise(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic, &local_topic, &ਵੰਡੋ_topics, ਨਿਰਾ);
		ਜੇ(rc){
			ਵਾਪਸ rc;
		}
		retain__store(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic, ਅਧਾਰ_ਸੁਨੇਹਾ, ਵੰਡੋ_topics, true);
		mosquitto_FREE(local_topic);
		mosquitto_FREE(ਵੰਡੋ_topics);
		retained_ਗਿਣਤੀ++;
	}ਨਹੀਂਤਾਂ{
		/* Can't find the message - probably expired */
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ persist__sub_chunk_restore(FILE *db_fptr)
{
	ਬਣਤਰ P_sub chunk;
	ਪੂਰਨ rc;
	ਬਣਤਰ mosquitto_subscription sub;

	memset(&chunk, 0, ਆਕਾਰ(ਬਣਤਰ P_sub));

	ਜੇ(db_ਸੰਸਕਰਣ == 6 || db_ਸੰਸਕਰਣ == 5){
		rc = persist__chunk_sub_read_v56(db_fptr, &chunk);
	}ਨਹੀਂਤਾਂ{
		rc = persist__chunk_sub_read_v234(db_fptr, &chunk);
	}
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}

	sub.clientid = chunk.clientid;
	sub.topic_filter = chunk.topic;
	sub.options = chunk.F.qos | chunk.F.options;
	sub.identifier = chunk.F.identifier;
	rc = persist__restore_sub(&sub);

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.topic);
	ਜੇ(rc == 0){
		subscription_ਗਿਣਤੀ++;
	}

	ਵਾਪਸ rc;
}


ਪੂਰਨ persist_chunk_ਸਿਰਲੇਖ_read(FILE *db_fptr, uint32_t *chunk, uint32_t *ਲੰਮਾਈ)
{
	ਜੇ(db_ਸੰਸਕਰਣ == 6 || db_ਸੰਸਕਰਣ == 5){
		ਵਾਪਸ persist_chunk_ਸਿਰਲੇਖ_read_v56(db_fptr, chunk, ਲੰਮਾਈ);
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ persist_chunk_ਸਿਰਲੇਖ_read_v234(db_fptr, chunk, ਲੰਮਾਈ);
	}
}


ਪੂਰਨ persist__restore(ਖਾਲੀ)
{
	FILE *fptr;
	ਅੱਖਰ ਸਿਰਲੇਖ[15];
	ਪੂਰਨ rc = 0;
	uint32_t ਚੱਕਰੀ_ਜਾਂਚ;
	uint32_t i32temp;
	uint32_t chunk, ਲੰਮਾਈ;
	size_t rlen;
	ਅੱਖਰ *err;
	ਬਣਤਰ PF_cfg cfg_chunk;

	ਪ੍ਰਤਿਜ੍ਞਾ(db.config);

	ਜੇ(!db.config->persistence || db.config->persistence_filepath == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

	db.ਸੁਨੇਹਾ_store = ਨਿਰਾ;
	ਅਧਾਰ_ਸੁਨੇਹਾ_ਗਿਣਤੀ = 0;
	retained_ਗਿਣਤੀ = 0;
	client_ਗਿਣਤੀ = 0;
	subscription_ਗਿਣਤੀ = 0;
	client_ਸੁਨੇਹਾ_ਗਿਣਤੀ = 0;

	fptr = mosquitto_fopen(db.config->persistence_filepath, "rb", true);
	ਜੇ(fptr == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}
	rlen = fread(&ਸਿਰਲੇਖ, 1, 15, fptr);
	ਜੇ(rlen == 0){
		fclose(fptr);
		log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Persistence file is empty.");
		ਵਾਪਸ 0;
	}ਨਹੀਂਤਾਂ ਜੇ(rlen != 15){
		ਜਾਓ error;
	}
	ਜੇ(!memcmp(ਸਿਰਲੇਖ, magic, 15)){
		/* Restore DB as normal */
		read_e(fptr, &ਚੱਕਰੀ_ਜਾਂਚ, ਆਕਾਰ(uint32_t));
		read_e(fptr, &i32temp, ਆਕਾਰ(uint32_t));
		db_ਸੰਸਕਰਣ = ntohl(i32temp);
		/* IMPORTANT - this is where compatibility checks are made.
		 * Is your DB change still compatible with previous versions?
		 */
		ਜੇ(db_ਸੰਸਕਰਣ != MOSQ_DB_ਸੰਸਕਰਣ){
			ਜੇ(db_ਸੰਸਕਰਣ == 5){
				/* Addition of username and listener_port to client chunk in v6 */
			}ਨਹੀਂਤਾਂ ਜੇ(db_ਸੰਸਕਰਣ == 4){
			}ਨਹੀਂਤਾਂ ਜੇ(db_ਸੰਸਕਰਣ == 3){
				/* Addition of source_username and source_port to msg_store chunk in v4, v1.5.6 */
			}ਨਹੀਂਤਾਂ ਜੇ(db_ਸੰਸਕਰਣ == 2){
				/* Addition of disconnect_t to client chunk in v3. */
			}ਨਹੀਂਤਾਂ{
				fclose(fptr);
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unsupported persistent database format version %d (need version %d).", db_ਸੰਸਕਰਣ, MOSQ_DB_ਸੰਸਕਰਣ);
				ਵਾਪਸ MOSQ_ERR_INVAL;
			}
		}

		ਜਦੋਂ(persist_chunk_ਸਿਰਲੇਖ_read(fptr, &chunk, &ਲੰਮਾਈ) == MOSQ_ERR_SUCCESS){
			ਚੋਣ(chunk){
				ਹਾਲਤ DB_CHUNK_CFG:
					ਜੇ(db_ਸੰਸਕਰਣ == 6 || db_ਸੰਸਕਰਣ == 5){
						rc = persist__chunk_cfg_read_v56(fptr, &cfg_chunk);
						ਜੇ(rc){
							fclose(fptr);
							ਵਾਪਸ rc;
						}
					}ਨਹੀਂਤਾਂ{
						rc = persist__chunk_cfg_read_v234(fptr, &cfg_chunk);
						ਜੇ(rc){
							fclose(fptr);
							ਵਾਪਸ rc;
						}
					}
					ਜੇ(cfg_chunk.dbid_ਮਾਪ != ਆਕਾਰ(dbid_t)){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Incompatible database configuration (dbid size is %d bytes, expected %lu)",
								cfg_chunk.dbid_ਮਾਪ, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)ਆਕਾਰ(dbid_t));
						fclose(fptr);
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					db.ਆਖਰੀ_db_id = cfg_chunk.ਆਖਰੀ_db_id;
					ਰੋਕੋ;

				ਹਾਲਤ DB_CHUNK_ਅਧਾਰ_ਸੁਨੇਹਾ:
					rc = persist_ਅਧਾਰ_ਸੁਨੇਹਾ_chunk_restore(fptr, ਲੰਮਾਈ);
					ਜੇ(rc){
						fclose(fptr);
						ਵਾਪਸ rc;
					}
					ਰੋਕੋ;

				ਹਾਲਤ DB_CHUNK_CLIENT_ਸੁਨੇਹਾ:
					rc = persist_client_ਸੁਨੇਹਾ_chunk_restore(fptr, ਲੰਮਾਈ);
					ਜੇ(rc){
						fclose(fptr);
						ਵਾਪਸ rc;
					}
					ਰੋਕੋ;

				ਹਾਲਤ DB_CHUNK_RETAIN:
					rc = persist__retain_chunk_restore(fptr);
					ਜੇ(rc){
						fclose(fptr);
						ਵਾਪਸ rc;
					}
					ਰੋਕੋ;

				ਹਾਲਤ DB_CHUNK_SUB:
					rc = persist__sub_chunk_restore(fptr);
					ਜੇ(rc){
						fclose(fptr);
						ਵਾਪਸ rc;
					}
					ਰੋਕੋ;

				ਹਾਲਤ DB_CHUNK_CLIENT:
					rc = persist__client_chunk_restore(fptr);
					ਜੇ(rc){
						fclose(fptr);
						ਵਾਪਸ rc;
					}
					ਰੋਕੋ;

				ਮੂਲ:
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Unsupported chunk \"%d\" in persistent database file. Ignoring.", chunk);
					ਜੇ(fseek(fptr, ਲੰਮਾਈ, SEEK_CUR) < 0){
						fclose(fptr);
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਰੋਕੋ;
			}
		}
	}ਨਹੀਂਤਾਂ{
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to restore persistent database. Unrecognised file format.");
		rc = 1;
	}

	fclose(fptr);

	log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Restored %ld base messages", ਅਧਾਰ_ਸੁਨੇਹਾ_ਗਿਣਤੀ);
	log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Restored %ld retained messages", retained_ਗਿਣਤੀ);
	log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Restored %ld clients", client_ਗਿਣਤੀ);
	log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Restored %ld subscriptions", subscription_ਗਿਣਤੀ);
	log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Restored %ld client messages", client_ਸੁਨੇਹਾ_ਗਿਣਤੀ);

	ਵਾਪਸ rc;
error:
	err = strerror(errno);
	log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: %s.", err);
	ਜੇ(fptr){
		fclose(fptr);
	}
	ਵਾਪਸ MOSQ_ERR_ERRNO;
}


ਸਥਿਰ ਪੂਰਨ persist__restore_sub(ਅਚਲ ਬਣਤਰ mosquitto_subscription *sub)
{
	ਬਣਤਰ mosquitto *context;

	ਜੇ(!sub->clientid){
		log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no client id, ignoring.");
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}
	ਜੇ(!sub->topic_filter){
		log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no topic filter, ignoring.");
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

	context = persist_ਲੱਭੋ_or_ਜੋੜੋ_context(sub->clientid, 0);
	ਜੇ(!context){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}
	ਵਾਪਸ sub_ਜੋੜੋ(context, sub);
}

#ਜੇ_ਅੰਤ
