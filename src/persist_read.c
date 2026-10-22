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
#ضمن <utlist.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "persist.h"
#ضمن "util_mosq.h"

uint32_t db_نسخه;

ثابت بینښه حرف magic[15] = {0x00, 0xB5, 0x00, 'm', 'o', 's', 'q', 'u', 'i', 't', 't', 'o', ' ', 'd', 'b'};
ساكن اوږد بنسټ_پیغام_شمېر = 0;
ساكن اوږد retained_شمېر = 0;
ساكن اوږد client_شمېر = 0;
ساكن اوږد subscription_شمېر = 0;
ساكن اوږد client_پیغام_شمېر = 0;

ساكن صحيح persist__restore_sub(ثابت بنية mosquitto_subscription *sub);

ساكن بنية mosquitto *persist_پیدا_کړه_or_جمع_کړه_context(ثابت حرف *clientid, uint16_t وروستی_mid)
{
	بنية mosquitto *context;

	إذا(!clientid){
		أرجع عدم;
	}

	context = عدم;
	HASH_پیدا_کړه(hh_id, db.contexts_by_id, clientid, مزي_اوږدوالی(clientid), context);
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

		context->clean_پیل = false;

		context_جمع_کړه_منزل_by_id(context);
	}
	إذا(وروستی_mid){
		context->وروستی_mid = وروستی_mid;
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


ساكن صحيح persist_client_پیغام_restore(بنية P_client_پیغام *chunk)
{
	بنية mosquitto_client_پیغام *cmsg;
	بنية mosquitto_بنسټ_پیغام *پیغام;
	بنية mosquitto *context;
	بنية mosquitto_پیغام_معلومات *پیغام_معلومات;

	HASH_پیدا_کړه(hh, db.پیغام_store, &chunk->F.store_id, حجم(chunk->F.store_id), پیغام);
	إذا(!پیغام){
		/* Can't find message - probably expired */
		أرجع MOSQ_ERR_SUCCESS;
	}

	context = persist_پیدا_کړه_or_جمع_کړه_context(chunk->clientid, 0);
	إذا(!context){
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Persistence file contains client message with no matching client. File may be corrupt.");
		أرجع 0;
	}

	cmsg = mosquitto_calloc(1, حجم(بنية mosquitto_client_پیغام));
	إذا(!cmsg){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		أرجع MOSQ_ERR_NOMEM;
	}

	cmsg->بل = عدم;
	cmsg->بنسټ_پیغام = عدم;
	cmsg->معلومات.cmsg_id = ++context->وروستی_cmsg_id;
	cmsg->معلومات.mid = chunk->F.mid;
	cmsg->معلومات.qos = chunk->F.qos;
	cmsg->معلومات.retain = (chunk->F.retain_dup&0xF0)>>4;
	cmsg->معلومات.direction = chunk->F.direction;
	cmsg->معلومات.state = chunk->F.state;
	cmsg->معلومات.dup = chunk->F.retain_dup&0x0F;
	cmsg->معلومات.subscription_identifier = chunk->subscription_identifier;

	cmsg->بنسټ_پیغام = پیغام;
	db_پیغام_store_ref_inc(cmsg->بنسټ_پیغام);

	إذا(cmsg->معلومات.direction == mosq_md_بهر){
		پیغام_معلومات = &context->msgs_بهر;
	}وإلا{
		پیغام_معلومات = &context->msgs_دننه;
	}

	إذا(chunk->F.state == mosq_ms_queued || (chunk->F.qos > 0 && پیغام_معلومات->inflight_quota == 0)){
		DL_APPEND(پیغام_معلومات->queued, cmsg);
		db_پیغام_جمع_کړه_منزل_queued_stats(پیغام_معلومات, cmsg);
	}وإلا{
		DL_APPEND(پیغام_معلومات->inflight, cmsg);
		إذا(chunk->F.qos > 0 && پیغام_معلومات->inflight_quota > 0){
			پیغام_معلومات->inflight_quota--;
		}
		db_پیغام_جمع_کړه_منزل_inflight_stats(پیغام_معلومات, cmsg);
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

	context = persist_پیدا_کړه_or_جمع_کړه_context(chunk.clientid, chunk.F.وروستی_mid);
	إذا(context){
		context->session_expiry_وخت = chunk.F.session_expiry_وخت;
		context->session_expiry_interval = chunk.F.session_expiry_interval;
		إذا(chunk.username && !context->username){
			/* username is not freed here, it is now owned by context */
			context->username = chunk.username;
			chunk.username = عدم;
		}
		/* in per_listener_settings mode, try to find the listener by persisted port */
		إذا(db.config->فی_listener_settings && !context->listener && chunk.F.listener_پورټ > 0){
			لكل(صحيح i=0; i < db.config->listener_شمېر; i++){
				إذا(db.config->listeners[i].پورټ == chunk.F.listener_پورټ){
					context->listener = &db.config->listeners[i];
					اكسر;
				}
			}
		}
		session_expiry_جمع_کړه_from_persistence(context, chunk.F.session_expiry_وخت);
	}وإلا{
		rc = 1;
	}

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.username);
	إذا(rc == 0){
		client_شمېر++;
	}
	أرجع rc;
}


ساكن صحيح persist_client_پیغام_chunk_restore(FILE *db_fptr, uint32_t اوږدتیا)
{
	بنية P_client_پیغام chunk;
	صحيح rc;

	memset(&chunk, 0, حجم(بنية P_client_پیغام));

	إذا(db_نسخه == 6 || db_نسخه == 5){
		rc = persist_chunk_client_پیغام_read_v56(db_fptr, &chunk, اوږدتیا);
	}وإلا{
		rc = persist_chunk_client_پیغام_read_v234(db_fptr, &chunk);
	}
	إذا(rc){
		أرجع rc;
	}

	rc = persist_client_پیغام_restore(&chunk);
	mosquitto_FREE(chunk.clientid);

	إذا(rc == 0){
		client_پیغام_شمېر++;
	}
	أرجع rc;
}


ساكن صحيح persist_بنسټ_پیغام_chunk_restore(FILE *db_fptr, uint32_t اوږدتیا)
{
	بنية P_بنسټ_پیغام chunk;
	بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام = عدم;
	int64_t پیغام_expiry_interval64;
	uint32_t پیغام_expiry_interval;
	uint32_t *p_پیغام_expiry_interval = عدم;
	صحيح rc = 0;

	memset(&chunk, 0, حجم(بنية P_بنسټ_پیغام));

	إذا(db_نسخه == 6 || db_نسخه == 5){
		rc = persist_chunk_بنسټ_پیغام_read_v56(db_fptr, &chunk, اوږدتیا);
	}وإلا{
		rc = persist_chunk_بنسټ_پیغام_read_v234(db_fptr, &chunk, db_نسخه);
	}
	إذا(rc){
		أرجع rc;
	}
	إذا(chunk.F.topic_len == 0){
		rc = MOSQ_ERR_INVAL;
		اذهب cleanup;
	}

	إذا(chunk.F.سرچینه_پورټ){
		لكل(صحيح i=0; i<db.config->listener_شمېر; i++){
			إذا(db.config->listeners[i].پورټ == chunk.F.سرچینه_پورټ){
				chunk.سرچینه.listener = &db.config->listeners[i];
				اكسر;
			}
		}
	}

	إذا(chunk.F.expiry_وخت > 0){
		پیغام_expiry_interval64 = chunk.F.expiry_وخت - وخت(عدم);
		إذا(پیغام_expiry_interval64 < 0 || پیغام_expiry_interval64 > UINT32_MAX){
			/* Expired message */
			rc = MOSQ_ERR_SUCCESS;
			اذهب cleanup;
		}وإلا{
			پیغام_expiry_interval = (uint32_t)پیغام_expiry_interval64;
		}
		p_پیغام_expiry_interval = &پیغام_expiry_interval;
	}

	بنسټ_پیغام = mosquitto_calloc(1, حجم(بنية mosquitto_بنسټ_پیغام));
	إذا(بنسټ_پیغام == عدم){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		rc = MOSQ_ERR_NOMEM;
		اذهب cleanup;
	}

	بنسټ_پیغام->معلومات.store_id = chunk.F.store_id;
	بنسټ_پیغام->معلومات.سرچینه_mid = chunk.F.سرچینه_mid;
	بنسټ_پیغام->معلومات.topic = chunk.topic;
	بنسټ_پیغام->معلومات.qos = chunk.F.qos;
	بنسټ_پیغام->معلومات.payloadlen = chunk.F.payloadlen;
	بنسټ_پیغام->معلومات.retain = chunk.F.retain;
	بنسټ_پیغام->معلومات.properties = chunk.properties;
	بنسټ_پیغام->معلومات.payload = chunk.payload;
	بنسټ_پیغام->سرچینه_listener = chunk.سرچینه.listener;

	rc = db_پیغام_store(&chunk.سرچینه, بنسټ_پیغام, p_پیغام_expiry_interval,
			mosq_mo_client);

	mosquitto_FREE(chunk.سرچینه.id);
	mosquitto_FREE(chunk.سرچینه.username);

	إذا(rc == MOSQ_ERR_SUCCESS){
		بنسټ_پیغام_شمېر++;
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا{
		أرجع rc;
	}
cleanup:
	mosquitto_FREE(chunk.سرچینه.id);
	mosquitto_FREE(chunk.سرچینه.username);
	mosquitto_FREE(chunk.topic);
	mosquitto_FREE(chunk.payload);
	أرجع rc;
}


ساكن صحيح persist__retain_chunk_restore(FILE *db_fptr)
{
	بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام;
	بنية P_retain chunk;
	صحيح rc;
	حرف **وېشل_topics;
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

	HASH_پیدا_کړه(hh, db.پیغام_store, &chunk.F.store_id, حجم(chunk.F.store_id), بنسټ_پیغام);
	إذا(بنسټ_پیغام){
		rc = sub__topic_tokenise(بنسټ_پیغام->معلومات.topic, &local_topic, &وېشل_topics, عدم);
		إذا(rc){
			أرجع rc;
		}
		retain__store(بنسټ_پیغام->معلومات.topic, بنسټ_پیغام, وېشل_topics, true);
		mosquitto_FREE(local_topic);
		mosquitto_FREE(وېشل_topics);
		retained_شمېر++;
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
		subscription_شمېر++;
	}

	أرجع rc;
}


صحيح persist_chunk_سرلیک_read(FILE *db_fptr, uint32_t *chunk, uint32_t *اوږدتیا)
{
	إذا(db_نسخه == 6 || db_نسخه == 5){
		أرجع persist_chunk_سرلیک_read_v56(db_fptr, chunk, اوږدتیا);
	}وإلا{
		أرجع persist_chunk_سرلیک_read_v234(db_fptr, chunk, اوږدتیا);
	}
}


صحيح persist__restore(فراغ)
{
	FILE *fptr;
	حرف سرلیک[15];
	صحيح rc = 0;
	uint32_t دوراني_ازموینه;
	uint32_t i32temp;
	uint32_t chunk, اوږدتیا;
	size_t rlen;
	حرف *err;
	بنية PF_cfg cfg_chunk;

	أكد(db.config);

	إذا(!db.config->persistence || db.config->persistence_filepath == عدم){
		أرجع MOSQ_ERR_SUCCESS;
	}

	db.پیغام_store = عدم;
	بنسټ_پیغام_شمېر = 0;
	retained_شمېر = 0;
	client_شمېر = 0;
	subscription_شمېر = 0;
	client_پیغام_شمېر = 0;

	fptr = mosquitto_fopen(db.config->persistence_filepath, "rb", true);
	إذا(fptr == عدم){
		أرجع MOSQ_ERR_SUCCESS;
	}
	rlen = fread(&سرلیک, 1, 15, fptr);
	إذا(rlen == 0){
		fclose(fptr);
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Persistence file is empty.");
		أرجع 0;
	}وإلا إذا(rlen != 15){
		اذهب error;
	}
	إذا(!memcmp(سرلیک, magic, 15)){
		/* Restore DB as normal */
		read_e(fptr, &دوراني_ازموینه, حجم(uint32_t));
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

		طالما(persist_chunk_سرلیک_read(fptr, &chunk, &اوږدتیا) == MOSQ_ERR_SUCCESS){
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
								cfg_chunk.dbid_اندازه, (بینښه اوږد)حجم(dbid_t));
						fclose(fptr);
						أرجع MOSQ_ERR_INVAL;
					}
					db.وروستی_db_id = cfg_chunk.وروستی_db_id;
					اكسر;

				حالة DB_CHUNK_بنسټ_پیغام:
					rc = persist_بنسټ_پیغام_chunk_restore(fptr, اوږدتیا);
					إذا(rc){
						fclose(fptr);
						أرجع rc;
					}
					اكسر;

				حالة DB_CHUNK_CLIENT_پیغام:
					rc = persist_client_پیغام_chunk_restore(fptr, اوږدتیا);
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
					إذا(fseek(fptr, اوږدتیا, SEEK_CUR) < 0){
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

	log__printf(عدم, MOSQ_LOG_INFO, "Restored %ld base messages", بنسټ_پیغام_شمېر);
	log__printf(عدم, MOSQ_LOG_INFO, "Restored %ld retained messages", retained_شمېر);
	log__printf(عدم, MOSQ_LOG_INFO, "Restored %ld clients", client_شمېر);
	log__printf(عدم, MOSQ_LOG_INFO, "Restored %ld subscriptions", subscription_شمېر);
	log__printf(عدم, MOSQ_LOG_INFO, "Restored %ld client messages", client_پیغام_شمېر);

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

	context = persist_پیدا_کړه_or_جمع_کړه_context(sub->clientid, 0);
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}
	أرجع sub_جمع_کړه(context, sub);
}

#د_که_پای
