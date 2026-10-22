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
#ضمن <utlist.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "persist.h"
#ضمن "util_mosq.h"

uint32_t db_نسخه;

ثابت بیعلامت حرف magic[15] = {0x00, 0xB5, 0x00, 'm', 'o', 's', 'q', 'u', 'i', 't', 't', 'o', ' ', 'd', 'b'};
ساكن دراز پایه_پیام_شمار = 0;
ساكن دراز retained_شمار = 0;
ساكن دراز client_شمار = 0;
ساكن دراز subscription_شمار = 0;
ساكن دراز client_پیام_شمار = 0;

ساكن صحيح persist__restore_sub(ثابت بنية mosquitto_subscription *sub);

ساكن بنية mosquitto *persist_پیدا_کن_or_جمع_کن_context(ثابت حرف *clientid, uint16_t آخرین_mid)
{
	بنية mosquitto *context;

	إذا(!clientid){
		أرجع عدم;
	}

	context = عدم;
	HASH_پیدا_کن(hh_id, db.contexts_by_id, clientid, طول_رشته(clientid), context);
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

		context->clean_شروع = false;

		context_جمع_کن_مقصد_by_id(context);
	}
	إذا(آخرین_mid){
		context->آخرین_mid = آخرین_mid;
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


ساكن صحيح persist_client_پیام_restore(بنية P_client_پیام *chunk)
{
	بنية mosquitto_client_پیام *cmsg;
	بنية mosquitto_پایه_پیام *پیام;
	بنية mosquitto *context;
	بنية mosquitto_پیام_داده *پیام_داده;

	HASH_پیدا_کن(hh, db.پیام_store, &chunk->F.store_id, حجم(chunk->F.store_id), پیام);
	إذا(!پیام){
		/* Can't find message - probably expired */
		أرجع MOSQ_ERR_SUCCESS;
	}

	context = persist_پیدا_کن_or_جمع_کن_context(chunk->clientid, 0);
	إذا(!context){
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Persistence file contains client message with no matching client. File may be corrupt.");
		أرجع 0;
	}

	cmsg = mosquitto_calloc(1, حجم(بنية mosquitto_client_پیام));
	إذا(!cmsg){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		أرجع MOSQ_ERR_NOMEM;
	}

	cmsg->بعدی = عدم;
	cmsg->پایه_پیام = عدم;
	cmsg->داده.cmsg_id = ++context->آخرین_cmsg_id;
	cmsg->داده.mid = chunk->F.mid;
	cmsg->داده.qos = chunk->F.qos;
	cmsg->داده.retain = (chunk->F.retain_dup&0xF0)>>4;
	cmsg->داده.direction = chunk->F.direction;
	cmsg->داده.state = chunk->F.state;
	cmsg->داده.dup = chunk->F.retain_dup&0x0F;
	cmsg->داده.subscription_identifier = chunk->subscription_identifier;

	cmsg->پایه_پیام = پیام;
	db_پیام_store_ref_inc(cmsg->پایه_پیام);

	إذا(cmsg->داده.direction == mosq_md_بیرون){
		پیام_داده = &context->msgs_بیرون;
	}وإلا{
		پیام_داده = &context->msgs_درون;
	}

	إذا(chunk->F.state == mosq_ms_queued || (chunk->F.qos > 0 && پیام_داده->inflight_quota == 0)){
		DL_APPEND(پیام_داده->queued, cmsg);
		db_پیام_جمع_کن_مقصد_queued_stats(پیام_داده, cmsg);
	}وإلا{
		DL_APPEND(پیام_داده->inflight, cmsg);
		إذا(chunk->F.qos > 0 && پیام_داده->inflight_quota > 0){
			پیام_داده->inflight_quota--;
		}
		db_پیام_جمع_کن_مقصد_inflight_stats(پیام_داده, cmsg);
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح persist__client_chunk_restore(FILE *db_fptr)
{
	صحيح rc = 0;
	بنية mosquitto *context;
	بنية P_client chunk;

	memset(&chunk, 0, حجم(بنية P_client));

	إذا(db_نسخه == 6 || db_نسخه == 5){
		rc = persist__chunk_client_read_v56(db_fptr, &chunk, db_نسخه);
	}وإلا{
		rc = persist__chunk_client_read_v234(db_fptr, &chunk, db_نسخه);
	}
	إذا(rc > 0){
		أرجع rc;
	}وإلا إذا(rc < 0){
		/* Client not loaded, but otherwise not an error */
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Empty client entry found in persistence database, it may be corrupt.");
		أرجع MOSQ_ERR_SUCCESS;
	}

	context = persist_پیدا_کن_or_جمع_کن_context(chunk.clientid, chunk.F.آخرین_mid);
	إذا(context){
		context->session_expiry_زمان = chunk.F.session_expiry_زمان;
		context->session_expiry_interval = chunk.F.session_expiry_interval;
		إذا(chunk.username && !context->username){
			/* username is not freed here, it is now owned by context */
			context->username = chunk.username;
			chunk.username = عدم;
		}
		/* in per_listener_settings mode, try to find the listener by persisted port */
		إذا(db.config->در_هر_listener_settings && !context->listener && chunk.F.listener_پورت > 0){
			لكل(صحيح i=0; i < db.config->listener_شمار; i++){
				إذا(db.config->listeners[i].پورت == chunk.F.listener_پورت){
					context->listener = &db.config->listeners[i];
					اكسر;
				}
			}
		}
		session_expiry_جمع_کن_from_persistence(context, chunk.F.session_expiry_زمان);
	}وإلا{
		rc = 1;
	}

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.username);
	إذا(rc == 0){
		client_شمار++;
	}
	أرجع rc;
}


ساكن صحيح persist_client_پیام_chunk_restore(FILE *db_fptr, uint32_t درازی)
{
	بنية P_client_پیام chunk;
	صحيح rc;

	memset(&chunk, 0, حجم(بنية P_client_پیام));

	إذا(db_نسخه == 6 || db_نسخه == 5){
		rc = persist_chunk_client_پیام_read_v56(db_fptr, &chunk, درازی);
	}وإلا{
		rc = persist_chunk_client_پیام_read_v234(db_fptr, &chunk);
	}
	إذا(rc){
		أرجع rc;
	}

	rc = persist_client_پیام_restore(&chunk);
	mosquitto_FREE(chunk.clientid);

	إذا(rc == 0){
		client_پیام_شمار++;
	}
	أرجع rc;
}


ساكن صحيح persist_پایه_پیام_chunk_restore(FILE *db_fptr, uint32_t درازی)
{
	بنية P_پایه_پیام chunk;
	بنية mosquitto_پایه_پیام *پایه_پیام = عدم;
	int64_t پیام_expiry_interval64;
	uint32_t پیام_expiry_interval;
	uint32_t *p_پیام_expiry_interval = عدم;
	صحيح rc = 0;

	memset(&chunk, 0, حجم(بنية P_پایه_پیام));

	إذا(db_نسخه == 6 || db_نسخه == 5){
		rc = persist_chunk_پایه_پیام_read_v56(db_fptr, &chunk, درازی);
	}وإلا{
		rc = persist_chunk_پایه_پیام_read_v234(db_fptr, &chunk, db_نسخه);
	}
	إذا(rc){
		أرجع rc;
	}
	إذا(chunk.F.topic_len == 0){
		rc = MOSQ_ERR_INVAL;
		اذهب cleanup;
	}

	إذا(chunk.F.منبع_پورت){
		لكل(صحيح i=0; i<db.config->listener_شمار; i++){
			إذا(db.config->listeners[i].پورت == chunk.F.منبع_پورت){
				chunk.منبع.listener = &db.config->listeners[i];
				اكسر;
			}
		}
	}

	إذا(chunk.F.expiry_زمان > 0){
		پیام_expiry_interval64 = chunk.F.expiry_زمان - زمان(عدم);
		إذا(پیام_expiry_interval64 < 0 || پیام_expiry_interval64 > UINT32_MAX){
			/* Expired message */
			rc = MOSQ_ERR_SUCCESS;
			اذهب cleanup;
		}وإلا{
			پیام_expiry_interval = (uint32_t)پیام_expiry_interval64;
		}
		p_پیام_expiry_interval = &پیام_expiry_interval;
	}

	پایه_پیام = mosquitto_calloc(1, حجم(بنية mosquitto_پایه_پیام));
	إذا(پایه_پیام == عدم){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		rc = MOSQ_ERR_NOMEM;
		اذهب cleanup;
	}

	پایه_پیام->داده.store_id = chunk.F.store_id;
	پایه_پیام->داده.منبع_mid = chunk.F.منبع_mid;
	پایه_پیام->داده.topic = chunk.topic;
	پایه_پیام->داده.qos = chunk.F.qos;
	پایه_پیام->داده.payloadlen = chunk.F.payloadlen;
	پایه_پیام->داده.retain = chunk.F.retain;
	پایه_پیام->داده.properties = chunk.properties;
	پایه_پیام->داده.payload = chunk.payload;
	پایه_پیام->منبع_listener = chunk.منبع.listener;

	rc = db_پیام_store(&chunk.منبع, پایه_پیام, p_پیام_expiry_interval,
			mosq_mo_client);

	mosquitto_FREE(chunk.منبع.id);
	mosquitto_FREE(chunk.منبع.username);

	إذا(rc == MOSQ_ERR_SUCCESS){
		پایه_پیام_شمار++;
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا{
		أرجع rc;
	}
cleanup:
	mosquitto_FREE(chunk.منبع.id);
	mosquitto_FREE(chunk.منبع.username);
	mosquitto_FREE(chunk.topic);
	mosquitto_FREE(chunk.payload);
	أرجع rc;
}


ساكن صحيح persist__retain_chunk_restore(FILE *db_fptr)
{
	بنية mosquitto_پایه_پیام *پایه_پیام;
	بنية P_retain chunk;
	صحيح rc;
	حرف **تقسیم_topics;
	حرف *local_topic;

	memset(&chunk, 0, حجم(بنية P_retain));

	إذا(db_نسخه == 6 || db_نسخه == 5){
		rc = persist__chunk_retain_read_v56(db_fptr, &chunk);
	}وإلا{
		rc = persist__chunk_retain_read_v234(db_fptr, &chunk);
	}
	إذا(rc){
		أرجع rc;
	}

	HASH_پیدا_کن(hh, db.پیام_store, &chunk.F.store_id, حجم(chunk.F.store_id), پایه_پیام);
	إذا(پایه_پیام){
		rc = sub__topic_tokenise(پایه_پیام->داده.topic, &local_topic, &تقسیم_topics, عدم);
		إذا(rc){
			أرجع rc;
		}
		retain__store(پایه_پیام->داده.topic, پایه_پیام, تقسیم_topics, true);
		mosquitto_FREE(local_topic);
		mosquitto_FREE(تقسیم_topics);
		retained_شمار++;
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

	إذا(db_نسخه == 6 || db_نسخه == 5){
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
		subscription_شمار++;
	}

	أرجع rc;
}


صحيح persist_chunk_سرآیند_read(FILE *db_fptr, uint32_t *chunk, uint32_t *درازی)
{
	إذا(db_نسخه == 6 || db_نسخه == 5){
		أرجع persist_chunk_سرآیند_read_v56(db_fptr, chunk, درازی);
	}وإلا{
		أرجع persist_chunk_سرآیند_read_v234(db_fptr, chunk, درازی);
	}
}


صحيح persist__restore(فراغ)
{
	FILE *fptr;
	حرف سرآیند[15];
	صحيح rc = 0;
	uint32_t بررسی_چرخهای;
	uint32_t i32temp;
	uint32_t chunk, درازی;
	size_t rlen;
	حرف *err;
	بنية PF_cfg cfg_chunk;

	أكد(db.config);

	إذا(!db.config->persistence || db.config->persistence_filepath == عدم){
		أرجع MOSQ_ERR_SUCCESS;
	}

	db.پیام_store = عدم;
	پایه_پیام_شمار = 0;
	retained_شمار = 0;
	client_شمار = 0;
	subscription_شمار = 0;
	client_پیام_شمار = 0;

	fptr = mosquitto_fopen(db.config->persistence_filepath, "rb", true);
	إذا(fptr == عدم){
		أرجع MOSQ_ERR_SUCCESS;
	}
	rlen = fread(&سرآیند, 1, 15, fptr);
	إذا(rlen == 0){
		fclose(fptr);
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Persistence file is empty.");
		أرجع 0;
	}وإلا إذا(rlen != 15){
		اذهب error;
	}
	إذا(!memcmp(سرآیند, magic, 15)){
		/* Restore DB as normal */
		read_e(fptr, &بررسی_چرخهای, حجم(uint32_t));
		read_e(fptr, &i32temp, حجم(uint32_t));
		db_نسخه = ntohl(i32temp);
		/* IMPORTANT - this is where compatibility checks are made.
		 * Is your DB change still compatible with previous versions?
		 */
		إذا(db_نسخه != MOSQ_DB_نسخه){
			إذا(db_نسخه == 5){
				/* Addition of username and listener_port to client chunk in v6 */
			}وإلا إذا(db_نسخه == 4){
			}وإلا إذا(db_نسخه == 3){
				/* Addition of source_username and source_port to msg_store chunk in v4, v1.5.6 */
			}وإلا إذا(db_نسخه == 2){
				/* Addition of disconnect_t to client chunk in v3. */
			}وإلا{
				fclose(fptr);
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Unsupported persistent database format version %d (need version %d).", db_نسخه, MOSQ_DB_نسخه);
				أرجع MOSQ_ERR_INVAL;
			}
		}

		طالما(persist_chunk_سرآیند_read(fptr, &chunk, &درازی) == MOSQ_ERR_SUCCESS){
			بدل(chunk){
				حالة DB_CHUNK_CFG:
					إذا(db_نسخه == 6 || db_نسخه == 5){
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
					إذا(cfg_chunk.dbid_اندازه != حجم(dbid_t)){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Incompatible database configuration (dbid size is %d bytes, expected %lu)",
								cfg_chunk.dbid_اندازه, (بیعلامت دراز)حجم(dbid_t));
						fclose(fptr);
						أرجع MOSQ_ERR_INVAL;
					}
					db.آخرین_db_id = cfg_chunk.آخرین_db_id;
					اكسر;

				حالة DB_CHUNK_پایه_پیام:
					rc = persist_پایه_پیام_chunk_restore(fptr, درازی);
					إذا(rc){
						fclose(fptr);
						أرجع rc;
					}
					اكسر;

				حالة DB_CHUNK_CLIENT_پیام:
					rc = persist_client_پیام_chunk_restore(fptr, درازی);
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
					إذا(fseek(fptr, درازی, SEEK_CUR) < 0){
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

	log__printf(عدم, MOSQ_LOG_INFO, "Restored %ld base messages", پایه_پیام_شمار);
	log__printf(عدم, MOSQ_LOG_INFO, "Restored %ld retained messages", retained_شمار);
	log__printf(عدم, MOSQ_LOG_INFO, "Restored %ld clients", client_شمار);
	log__printf(عدم, MOSQ_LOG_INFO, "Restored %ld subscriptions", subscription_شمار);
	log__printf(عدم, MOSQ_LOG_INFO, "Restored %ld client messages", client_پیام_شمار);

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

	context = persist_پیدا_کن_or_جمع_کن_context(sub->clientid, 0);
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}
	أرجع sub_جمع_کن(context, sub);
}

#پایان_اگر
