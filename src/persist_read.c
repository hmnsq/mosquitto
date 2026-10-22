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
#ضمن <utlist.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "persist.h"
#ضمن "util_mosq.h"

uint32_t db_نسخو;

ثابت بنانشان حرف magic[15] = {0x00, 0xB5, 0x00, 'm', 'o', 's', 'q', 'u', 'i', 't', 't', 'o', ' ', 'd', 'b'};
ساكن ڊگهو بنياد_نياپو_ڳڻپ = 0;
ساكن ڊگهو retained_ڳڻپ = 0;
ساكن ڊگهو client_ڳڻپ = 0;
ساكن ڊگهو subscription_ڳڻپ = 0;
ساكن ڊگهو client_نياپو_ڳڻپ = 0;

ساكن صحيح persist__restore_sub(ثابت بنية mosquitto_subscription *sub);

ساكن بنية mosquitto *persist_ڳوليو_or_جوڙيو_context(ثابت حرف *clientid, uint16_t آخري_mid)
{
	بنية mosquitto *context;

	إذا(!clientid){
		أرجع عدم;
	}

	context = عدم;
	HASH_ڳوليو(hh_id, db.contexts_by_id, clientid, لڙي_ڊيگهه(clientid), context);
	إذا(!context){
		context = context__init();
		إذا(!context){
			أرجع عدم;
		}
		context->id = mosquitto_strdup(clientid);
		إذا(!context->id){
			mosquitto_FREE(context);
			أرجع عدم;
		}

		context->clean_شروعات = false;

		context_جوڙيو_منزل_by_id(context);
	}
	إذا(آخري_mid){
		context->آخري_mid = آخري_mid;
	}
	أرجع context;
}


صحيح persist__read_string_len(FILE *db_fptr, حرف **str, uint16_t len)
{
	حرف *s = عدم;

	إذا(len){
		s = mosquitto_malloc(len+1U);
		إذا(!s){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
			أرجع MOSQ_ERR_NOMEM;
		}
		إذا(fread(s, 1, len, db_fptr) != len){
			mosquitto_FREE(s);
			أرجع MOSQ_ERR_NOMEM;
		}
		s[len] = '\0';
	}

	*str = s;
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح persist__read_string(FILE *db_fptr, حرف **str)
{
	uint16_t i16temp;
	uint16_t slen;

	إذا(fread(&i16temp, 1, حجم(uint16_t), db_fptr) != حجم(uint16_t)){
		أرجع MOSQ_ERR_INVAL;
	}

	slen = ntohs(i16temp);
	أرجع persist__read_string_len(db_fptr, str, slen);
}


ساكن صحيح persist_client_نياپو_restore(بنية P_client_نياپو *chunk)
{
	بنية mosquitto_client_نياپو *cmsg;
	بنية mosquitto_بنياد_نياپو *نياپو;
	بنية mosquitto *context;
	بنية mosquitto_نياپو_معلومات *نياپو_معلومات;

	HASH_ڳوليو(hh, db.نياپو_store, &chunk->F.store_id, حجم(chunk->F.store_id), نياپو);
	إذا(!نياپو){
		/* Can't find message - probably expired */
		أرجع MOSQ_ERR_SUCCESS;
	}

	context = persist_ڳوليو_or_جوڙيو_context(chunk->clientid, 0);
	إذا(!context){
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Persistence file contains client message with no matching client. File may be corrupt.");
		أرجع 0;
	}

	cmsg = mosquitto_calloc(1, حجم(بنية mosquitto_client_نياپو));
	إذا(!cmsg){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		أرجع MOSQ_ERR_NOMEM;
	}

	cmsg->ايندڙ = عدم;
	cmsg->بنياد_نياپو = عدم;
	cmsg->معلومات.cmsg_id = ++context->آخري_cmsg_id;
	cmsg->معلومات.mid = chunk->F.mid;
	cmsg->معلومات.qos = chunk->F.qos;
	cmsg->معلومات.retain = (chunk->F.retain_dup&0xF0)>>4;
	cmsg->معلومات.direction = chunk->F.direction;
	cmsg->معلومات.state = chunk->F.state;
	cmsg->معلومات.dup = chunk->F.retain_dup&0x0F;
	cmsg->معلومات.subscription_identifier = chunk->subscription_identifier;

	cmsg->بنياد_نياپو = نياپو;
	db_نياپو_store_ref_inc(cmsg->بنياد_نياپو);

	إذا(cmsg->معلومات.direction == mosq_md_ٻاهريون){
		نياپو_معلومات = &context->msgs_ٻاهريون;
	}وإلا{
		نياپو_معلومات = &context->msgs_اندريون;
	}

	إذا(chunk->F.state == mosq_ms_queued || (chunk->F.qos > 0 && نياپو_معلومات->inflight_quota == 0)){
		DL_APPEND(نياپو_معلومات->queued, cmsg);
		db_نياپو_جوڙيو_منزل_queued_stats(نياپو_معلومات, cmsg);
	}وإلا{
		DL_APPEND(نياپو_معلومات->inflight, cmsg);
		إذا(chunk->F.qos > 0 && نياپو_معلومات->inflight_quota > 0){
			نياپو_معلومات->inflight_quota--;
		}
		db_نياپو_جوڙيو_منزل_inflight_stats(نياپو_معلومات, cmsg);
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح persist__client_chunk_restore(FILE *db_fptr)
{
	صحيح rc = 0;
	بنية mosquitto *context;
	بنية P_client chunk;

	memset(&chunk, 0, حجم(بنية P_client));

	إذا(db_نسخو == 6 || db_نسخو == 5){
		rc = persist__chunk_client_read_v56(db_fptr, &chunk, db_نسخو);
	}وإلا{
		rc = persist__chunk_client_read_v234(db_fptr, &chunk, db_نسخو);
	}
	إذا(rc > 0){
		أرجع rc;
	}وإلا إذا(rc < 0){
		/* Client not loaded, but otherwise not an error */
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Empty client entry found in persistence database, it may be corrupt.");
		أرجع MOSQ_ERR_SUCCESS;
	}

	context = persist_ڳوليو_or_جوڙيو_context(chunk.clientid, chunk.F.آخري_mid);
	إذا(context){
		context->session_expiry_وقت = chunk.F.session_expiry_وقت;
		context->session_expiry_interval = chunk.F.session_expiry_interval;
		إذا(chunk.username && !context->username){
			/* username is not freed here, it is now owned by context */
			context->username = chunk.username;
			chunk.username = عدم;
		}
		/* in per_listener_settings mode, try to find the listener by persisted port */
		إذا(db.config->في_listener_settings && !context->listener && chunk.F.listener_پورٽ > 0){
			لكل(صحيح i=0; i < db.config->listener_ڳڻپ; i++){
				إذا(db.config->listeners[i].پورٽ == chunk.F.listener_پورٽ){
					context->listener = &db.config->listeners[i];
					اكسر;
				}
			}
		}
		session_expiry_جوڙيو_from_persistence(context, chunk.F.session_expiry_وقت);
	}وإلا{
		rc = 1;
	}

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.username);
	إذا(rc == 0){
		client_ڳڻپ++;
	}
	أرجع rc;
}


ساكن صحيح persist_client_نياپو_chunk_restore(FILE *db_fptr, uint32_t ڊيگهائي)
{
	بنية P_client_نياپو chunk;
	صحيح rc;

	memset(&chunk, 0, حجم(بنية P_client_نياپو));

	إذا(db_نسخو == 6 || db_نسخو == 5){
		rc = persist_chunk_client_نياپو_read_v56(db_fptr, &chunk, ڊيگهائي);
	}وإلا{
		rc = persist_chunk_client_نياپو_read_v234(db_fptr, &chunk);
	}
	إذا(rc){
		أرجع rc;
	}

	rc = persist_client_نياپو_restore(&chunk);
	mosquitto_FREE(chunk.clientid);

	إذا(rc == 0){
		client_نياپو_ڳڻپ++;
	}
	أرجع rc;
}


ساكن صحيح persist_بنياد_نياپو_chunk_restore(FILE *db_fptr, uint32_t ڊيگهائي)
{
	بنية P_بنياد_نياپو chunk;
	بنية mosquitto_بنياد_نياپو *بنياد_نياپو = عدم;
	int64_t پيغام_expiry_interval64;
	uint32_t پيغام_expiry_interval;
	uint32_t *p_پيغام_expiry_interval = عدم;
	صحيح rc = 0;

	memset(&chunk, 0, حجم(بنية P_بنياد_نياپو));

	إذا(db_نسخو == 6 || db_نسخو == 5){
		rc = persist_chunk_بنياد_نياپو_read_v56(db_fptr, &chunk, ڊيگهائي);
	}وإلا{
		rc = persist_chunk_بنياد_نياپو_read_v234(db_fptr, &chunk, db_نسخو);
	}
	إذا(rc){
		أرجع rc;
	}
	إذا(chunk.F.topic_len == 0){
		rc = MOSQ_ERR_INVAL;
		اذهب cleanup;
	}

	إذا(chunk.F.ذريعو_پورٽ){
		لكل(صحيح i=0; i<db.config->listener_ڳڻپ; i++){
			إذا(db.config->listeners[i].پورٽ == chunk.F.ذريعو_پورٽ){
				chunk.ذريعو.listener = &db.config->listeners[i];
				اكسر;
			}
		}
	}

	إذا(chunk.F.expiry_وقت > 0){
		پيغام_expiry_interval64 = chunk.F.expiry_وقت - وقت(عدم);
		إذا(پيغام_expiry_interval64 < 0 || پيغام_expiry_interval64 > UINT32_MAX){
			/* Expired message */
			rc = MOSQ_ERR_SUCCESS;
			اذهب cleanup;
		}وإلا{
			پيغام_expiry_interval = (uint32_t)پيغام_expiry_interval64;
		}
		p_پيغام_expiry_interval = &پيغام_expiry_interval;
	}

	بنياد_نياپو = mosquitto_calloc(1, حجم(بنية mosquitto_بنياد_نياپو));
	إذا(بنياد_نياپو == عدم){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		rc = MOSQ_ERR_NOMEM;
		اذهب cleanup;
	}

	بنياد_نياپو->معلومات.store_id = chunk.F.store_id;
	بنياد_نياپو->معلومات.ذريعو_mid = chunk.F.ذريعو_mid;
	بنياد_نياپو->معلومات.topic = chunk.topic;
	بنياد_نياپو->معلومات.qos = chunk.F.qos;
	بنياد_نياپو->معلومات.payloadlen = chunk.F.payloadlen;
	بنياد_نياپو->معلومات.retain = chunk.F.retain;
	بنياد_نياپو->معلومات.properties = chunk.properties;
	بنياد_نياپو->معلومات.payload = chunk.payload;
	بنياد_نياپو->ذريعو_listener = chunk.ذريعو.listener;

	rc = db_پيغام_store(&chunk.ذريعو, بنياد_نياپو, p_پيغام_expiry_interval,
			mosq_mo_client);

	mosquitto_FREE(chunk.ذريعو.id);
	mosquitto_FREE(chunk.ذريعو.username);

	إذا(rc == MOSQ_ERR_SUCCESS){
		بنياد_نياپو_ڳڻپ++;
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا{
		أرجع rc;
	}
cleanup:
	mosquitto_FREE(chunk.ذريعو.id);
	mosquitto_FREE(chunk.ذريعو.username);
	mosquitto_FREE(chunk.topic);
	mosquitto_FREE(chunk.payload);
	أرجع rc;
}


ساكن صحيح persist__retain_chunk_restore(FILE *db_fptr)
{
	بنية mosquitto_بنياد_نياپو *بنياد_نياپو;
	بنية P_retain chunk;
	صحيح rc;
	حرف **ورهايو_topics;
	حرف *local_topic;

	memset(&chunk, 0, حجم(بنية P_retain));

	إذا(db_نسخو == 6 || db_نسخو == 5){
		rc = persist__chunk_retain_read_v56(db_fptr, &chunk);
	}وإلا{
		rc = persist__chunk_retain_read_v234(db_fptr, &chunk);
	}
	إذا(rc){
		أرجع rc;
	}

	HASH_ڳوليو(hh, db.نياپو_store, &chunk.F.store_id, حجم(chunk.F.store_id), بنياد_نياپو);
	إذا(بنياد_نياپو){
		rc = sub__topic_tokenise(بنياد_نياپو->معلومات.topic, &local_topic, &ورهايو_topics, عدم);
		إذا(rc){
			أرجع rc;
		}
		retain__store(بنياد_نياپو->معلومات.topic, بنياد_نياپو, ورهايو_topics, true);
		mosquitto_FREE(local_topic);
		mosquitto_FREE(ورهايو_topics);
		retained_ڳڻپ++;
	}وإلا{
		/* Can't find the message - probably expired */
	}
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح persist__sub_chunk_restore(FILE *db_fptr)
{
	بنية P_sub chunk;
	صحيح rc;
	بنية mosquitto_subscription sub;

	memset(&chunk, 0, حجم(بنية P_sub));

	إذا(db_نسخو == 6 || db_نسخو == 5){
		rc = persist__chunk_sub_read_v56(db_fptr, &chunk);
	}وإلا{
		rc = persist__chunk_sub_read_v234(db_fptr, &chunk);
	}
	إذا(rc){
		أرجع rc;
	}

	sub.clientid = chunk.clientid;
	sub.topic_filter = chunk.topic;
	sub.options = chunk.F.qos | chunk.F.options;
	sub.identifier = chunk.F.identifier;
	rc = persist__restore_sub(&sub);

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.topic);
	إذا(rc == 0){
		subscription_ڳڻپ++;
	}

	أرجع rc;
}


صحيح persist_chunk_سرو_read(FILE *db_fptr, uint32_t *chunk, uint32_t *ڊيگهائي)
{
	إذا(db_نسخو == 6 || db_نسخو == 5){
		أرجع persist_chunk_سرو_read_v56(db_fptr, chunk, ڊيگهائي);
	}وإلا{
		أرجع persist_chunk_سرو_read_v234(db_fptr, chunk, ڊيگهائي);
	}
}


صحيح persist__restore(فراغ)
{
	FILE *fptr;
	حرف سرو[15];
	صحيح rc = 0;
	uint32_t چڪري_جاچ;
	uint32_t i32temp;
	uint32_t chunk, ڊيگهائي;
	size_t rlen;
	حرف *err;
	بنية PF_cfg cfg_chunk;

	أكد(db.config);

	إذا(!db.config->persistence || db.config->persistence_filepath == عدم){
		أرجع MOSQ_ERR_SUCCESS;
	}

	db.نياپو_store = عدم;
	بنياد_نياپو_ڳڻپ = 0;
	retained_ڳڻپ = 0;
	client_ڳڻپ = 0;
	subscription_ڳڻپ = 0;
	client_نياپو_ڳڻپ = 0;

	fptr = mosquitto_fopen(db.config->persistence_filepath, "rb", true);
	إذا(fptr == عدم){
		أرجع MOSQ_ERR_SUCCESS;
	}
	rlen = fread(&سرو, 1, 15, fptr);
	إذا(rlen == 0){
		fclose(fptr);
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Persistence file is empty.");
		أرجع 0;
	}وإلا إذا(rlen != 15){
		اذهب error;
	}
	إذا(!memcmp(سرو, magic, 15)){
		/* Restore DB as normal */
		read_e(fptr, &چڪري_جاچ, حجم(uint32_t));
		read_e(fptr, &i32temp, حجم(uint32_t));
		db_نسخو = ntohl(i32temp);
		/* IMPORTANT - this is where compatibility checks are made.
		 * Is your DB change still compatible with previous versions?
		 */
		إذا(db_نسخو != MOSQ_DB_نسخو){
			إذا(db_نسخو == 5){
				/* Addition of username and listener_port to client chunk in v6 */
			}وإلا إذا(db_نسخو == 4){
			}وإلا إذا(db_نسخو == 3){
				/* Addition of source_username and source_port to msg_store chunk in v4, v1.5.6 */
			}وإلا إذا(db_نسخو == 2){
				/* Addition of disconnect_t to client chunk in v3. */
			}وإلا{
				fclose(fptr);
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Unsupported persistent database format version %d (need version %d).", db_نسخو, MOSQ_DB_نسخو);
				أرجع MOSQ_ERR_INVAL;
			}
		}

		طالما(persist_chunk_سرو_read(fptr, &chunk, &ڊيگهائي) == MOSQ_ERR_SUCCESS){
			بدل(chunk){
				حالة DB_CHUNK_CFG:
					إذا(db_نسخو == 6 || db_نسخو == 5){
						rc = persist__chunk_cfg_read_v56(fptr, &cfg_chunk);
						إذا(rc){
							fclose(fptr);
							أرجع rc;
						}
					}وإلا{
						rc = persist__chunk_cfg_read_v234(fptr, &cfg_chunk);
						إذا(rc){
							fclose(fptr);
							أرجع rc;
						}
					}
					إذا(cfg_chunk.dbid_سائيز != حجم(dbid_t)){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Incompatible database configuration (dbid size is %d bytes, expected %lu)",
								cfg_chunk.dbid_سائيز, (بنانشان ڊگهو)حجم(dbid_t));
						fclose(fptr);
						أرجع MOSQ_ERR_INVAL;
					}
					db.آخري_db_id = cfg_chunk.آخري_db_id;
					اكسر;

				حالة DB_CHUNK_بنياد_نياپو:
					rc = persist_بنياد_نياپو_chunk_restore(fptr, ڊيگهائي);
					إذا(rc){
						fclose(fptr);
						أرجع rc;
					}
					اكسر;

				حالة DB_CHUNK_CLIENT_نياپو:
					rc = persist_client_نياپو_chunk_restore(fptr, ڊيگهائي);
					إذا(rc){
						fclose(fptr);
						أرجع rc;
					}
					اكسر;

				حالة DB_CHUNK_RETAIN:
					rc = persist__retain_chunk_restore(fptr);
					إذا(rc){
						fclose(fptr);
						أرجع rc;
					}
					اكسر;

				حالة DB_CHUNK_SUB:
					rc = persist__sub_chunk_restore(fptr);
					إذا(rc){
						fclose(fptr);
						أرجع rc;
					}
					اكسر;

				حالة DB_CHUNK_CLIENT:
					rc = persist__client_chunk_restore(fptr);
					إذا(rc){
						fclose(fptr);
						أرجع rc;
					}
					اكسر;

				افتراضي:
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Unsupported chunk \"%d\" in persistent database file. Ignoring.", chunk);
					إذا(fseek(fptr, ڊيگهائي, SEEK_CUR) < 0){
						fclose(fptr);
						أرجع MOSQ_ERR_INVAL;
					}
					اكسر;
			}
		}
	}وإلا{
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to restore persistent database. Unrecognised file format.");
		rc = 1;
	}

	fclose(fptr);

	log__printf(عدم, MOSQ_LOG_INFO, "Restored %ld base messages", بنياد_نياپو_ڳڻپ);
	log__printf(عدم, MOSQ_LOG_INFO, "Restored %ld retained messages", retained_ڳڻپ);
	log__printf(عدم, MOSQ_LOG_INFO, "Restored %ld clients", client_ڳڻپ);
	log__printf(عدم, MOSQ_LOG_INFO, "Restored %ld subscriptions", subscription_ڳڻپ);
	log__printf(عدم, MOSQ_LOG_INFO, "Restored %ld client messages", client_نياپو_ڳڻپ);

	أرجع rc;
error:
	err = strerror(errno);
	log__printf(عدم, MOSQ_LOG_ERR, "Error: %s.", err);
	إذا(fptr){
		fclose(fptr);
	}
	أرجع MOSQ_ERR_ERRNO;
}


ساكن صحيح persist__restore_sub(ثابت بنية mosquitto_subscription *sub)
{
	بنية mosquitto *context;

	إذا(!sub->clientid){
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no client id, ignoring.");
		أرجع MOSQ_ERR_SUCCESS;
	}
	إذا(!sub->topic_filter){
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no topic filter, ignoring.");
		أرجع MOSQ_ERR_SUCCESS;
	}

	context = persist_ڳوليو_or_جوڙيو_context(sub->clientid, 0);
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}
	أرجع sub_جوڙيو(context, sub);
}

#جيڪڏهن_پورو
