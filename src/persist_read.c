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
#شامل <utlist.h>

#شامل "mosquitto_broker_internal.h"
#شامل "persist.h"
#شامل "util_mosq.h"

uint32_t db_نسخہ;

ثابت غیرعلامتی حرف magic[15] = {0x00, 0xB5, 0x00, 'm', 'o', 's', 'q', 'u', 'i', 't', 't', 'o', ' ', 'd', 'b'};
ساکن طویل بنیاد_پیغام_گنتی = 0;
ساکن طویل retained_گنتی = 0;
ساکن طویل client_گنتی = 0;
ساکن طویل subscription_گنتی = 0;
ساکن طویل client_پیغام_گنتی = 0;

ساکن صحیح persist__restore_sub(ثابت ساخت mosquitto_subscription *sub);

ساکن ساخت mosquitto *persist_ڈھونڈو_or_جوڑو_context(ثابت حرف *clientid, uint16_t آخری_mid)
{
	ساخت mosquitto *context;

	اگر(!clientid){
		واپس تہی;
	}

	context = تہی;
	HASH_ڈھونڈو(hh_id, db.contexts_by_id, clientid, لڑی_لمبائی(clientid), context);
	اگر(!context){
		context = context__init();
		اگر(!context){
			واپس تہی;
		}
		context->id = mosquitto_strdup(clientid);
		اگر(!context->id){
			mosquitto_FREE(context);
			واپس تہی;
		}

		context->clean_آغاز = false;

		context_جوڑو_مقصد_by_id(context);
	}
	اگر(آخری_mid){
		context->آخری_mid = آخری_mid;
	}
	واپس context;
}


صحیح persist__read_string_len(FILE *db_fptr, حرف **str, uint16_t len)
{
	حرف *s = تہی;

	اگر(len){
		s = mosquitto_malloc(len+1U);
		اگر(!s){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
			واپس MOSQ_ERR_NOMEM;
		}
		اگر(fread(s, 1, len, db_fptr) != len){
			mosquitto_FREE(s);
			واپس MOSQ_ERR_NOMEM;
		}
		s[len] = '\0';
	}

	*str = s;
	واپس MOSQ_ERR_SUCCESS;
}


صحیح persist__read_string(FILE *db_fptr, حرف **str)
{
	uint16_t i16temp;
	uint16_t slen;

	اگر(fread(&i16temp, 1, حجم(uint16_t), db_fptr) != حجم(uint16_t)){
		واپس MOSQ_ERR_INVAL;
	}

	slen = ntohs(i16temp);
	واپس persist__read_string_len(db_fptr, str, slen);
}


ساکن صحیح persist_client_پیغام_restore(ساخت P_client_پیغام *chunk)
{
	ساخت mosquitto_client_پیغام *cmsg;
	ساخت mosquitto_بنیاد_پیغام *پیغام;
	ساخت mosquitto *context;
	ساخت mosquitto_پیغام_ڈیٹا *پیغام_ڈیٹا;

	HASH_ڈھونڈو(hh, db.پیغام_store, &chunk->F.store_id, حجم(chunk->F.store_id), پیغام);
	اگر(!پیغام){
		/* Can't find message - probably expired */
		واپس MOSQ_ERR_SUCCESS;
	}

	context = persist_ڈھونڈو_or_جوڑو_context(chunk->clientid, 0);
	اگر(!context){
		log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Persistence file contains client message with no matching client. File may be corrupt.");
		واپس 0;
	}

	cmsg = mosquitto_calloc(1, حجم(ساخت mosquitto_client_پیغام));
	اگر(!cmsg){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
		واپس MOSQ_ERR_NOMEM;
	}

	cmsg->اگلا = تہی;
	cmsg->بنیاد_پیغام = تہی;
	cmsg->ڈیٹا.cmsg_id = ++context->آخری_cmsg_id;
	cmsg->ڈیٹا.mid = chunk->F.mid;
	cmsg->ڈیٹا.qos = chunk->F.qos;
	cmsg->ڈیٹا.retain = (chunk->F.retain_dup&0xF0)>>4;
	cmsg->ڈیٹا.direction = chunk->F.direction;
	cmsg->ڈیٹا.state = chunk->F.state;
	cmsg->ڈیٹا.dup = chunk->F.retain_dup&0x0F;
	cmsg->ڈیٹا.subscription_identifier = chunk->subscription_identifier;

	cmsg->بنیاد_پیغام = پیغام;
	db_پیغام_store_ref_inc(cmsg->بنیاد_پیغام);

	اگر(cmsg->ڈیٹا.direction == mosq_md_برآمد){
		پیغام_ڈیٹا = &context->msgs_برآمد;
	}ورنہ{
		پیغام_ڈیٹا = &context->msgs_اندر;
	}

	اگر(chunk->F.state == mosq_ms_queued || (chunk->F.qos > 0 && پیغام_ڈیٹا->inflight_quota == 0)){
		DL_APPEND(پیغام_ڈیٹا->queued, cmsg);
		db_پیغام_جوڑو_مقصد_queued_stats(پیغام_ڈیٹا, cmsg);
	}ورنہ{
		DL_APPEND(پیغام_ڈیٹا->inflight, cmsg);
		اگر(chunk->F.qos > 0 && پیغام_ڈیٹا->inflight_quota > 0){
			پیغام_ڈیٹا->inflight_quota--;
		}
		db_پیغام_جوڑو_مقصد_inflight_stats(پیغام_ڈیٹا, cmsg);
	}

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح persist__client_chunk_restore(FILE *db_fptr)
{
	صحیح rc = 0;
	ساخت mosquitto *context;
	ساخت P_client chunk;

	memset(&chunk, 0, حجم(ساخت P_client));

	اگر(db_نسخہ == 6 || db_نسخہ == 5){
		rc = persist__chunk_client_read_v56(db_fptr, &chunk, db_نسخہ);
	}ورنہ{
		rc = persist__chunk_client_read_v234(db_fptr, &chunk, db_نسخہ);
	}
	اگر(rc > 0){
		واپس rc;
	}ورنہ اگر(rc < 0){
		/* Client not loaded, but otherwise not an error */
		log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Empty client entry found in persistence database, it may be corrupt.");
		واپس MOSQ_ERR_SUCCESS;
	}

	context = persist_ڈھونڈو_or_جوڑو_context(chunk.clientid, chunk.F.آخری_mid);
	اگر(context){
		context->session_expiry_وقت = chunk.F.session_expiry_وقت;
		context->session_expiry_interval = chunk.F.session_expiry_interval;
		اگر(chunk.username && !context->username){
			/* username is not freed here, it is now owned by context */
			context->username = chunk.username;
			chunk.username = تہی;
		}
		/* in per_listener_settings mode, try to find the listener by persisted port */
		اگر(db.config->فی_listener_settings && !context->listener && chunk.F.listener_پورٹ > 0){
			برائے(صحیح i=0; i < db.config->listener_گنتی; i++){
				اگر(db.config->listeners[i].پورٹ == chunk.F.listener_پورٹ){
					context->listener = &db.config->listeners[i];
					توڑو;
				}
			}
		}
		session_expiry_جوڑو_from_persistence(context, chunk.F.session_expiry_وقت);
	}ورنہ{
		rc = 1;
	}

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.username);
	اگر(rc == 0){
		client_گنتی++;
	}
	واپس rc;
}


ساکن صحیح persist_client_پیغام_chunk_restore(FILE *db_fptr, uint32_t لمبائی)
{
	ساخت P_client_پیغام chunk;
	صحیح rc;

	memset(&chunk, 0, حجم(ساخت P_client_پیغام));

	اگر(db_نسخہ == 6 || db_نسخہ == 5){
		rc = persist_chunk_client_پیغام_read_v56(db_fptr, &chunk, لمبائی);
	}ورنہ{
		rc = persist_chunk_client_پیغام_read_v234(db_fptr, &chunk);
	}
	اگر(rc){
		واپس rc;
	}

	rc = persist_client_پیغام_restore(&chunk);
	mosquitto_FREE(chunk.clientid);

	اگر(rc == 0){
		client_پیغام_گنتی++;
	}
	واپس rc;
}


ساکن صحیح persist_بنیاد_پیغام_chunk_restore(FILE *db_fptr, uint32_t لمبائی)
{
	ساخت P_بنیاد_پیغام chunk;
	ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام = تہی;
	int64_t پیغام_expiry_interval64;
	uint32_t پیغام_expiry_interval;
	uint32_t *p_پیغام_expiry_interval = تہی;
	صحیح rc = 0;

	memset(&chunk, 0, حجم(ساخت P_بنیاد_پیغام));

	اگر(db_نسخہ == 6 || db_نسخہ == 5){
		rc = persist_chunk_بنیاد_پیغام_read_v56(db_fptr, &chunk, لمبائی);
	}ورنہ{
		rc = persist_chunk_بنیاد_پیغام_read_v234(db_fptr, &chunk, db_نسخہ);
	}
	اگر(rc){
		واپس rc;
	}
	اگر(chunk.F.topic_len == 0){
		rc = MOSQ_ERR_INVAL;
		جاؤ cleanup;
	}

	اگر(chunk.F.ماخذ_پورٹ){
		برائے(صحیح i=0; i<db.config->listener_گنتی; i++){
			اگر(db.config->listeners[i].پورٹ == chunk.F.ماخذ_پورٹ){
				chunk.ماخذ.listener = &db.config->listeners[i];
				توڑو;
			}
		}
	}

	اگر(chunk.F.expiry_وقت > 0){
		پیغام_expiry_interval64 = chunk.F.expiry_وقت - وقت(تہی);
		اگر(پیغام_expiry_interval64 < 0 || پیغام_expiry_interval64 > UINT32_MAX){
			/* Expired message */
			rc = MOSQ_ERR_SUCCESS;
			جاؤ cleanup;
		}ورنہ{
			پیغام_expiry_interval = (uint32_t)پیغام_expiry_interval64;
		}
		p_پیغام_expiry_interval = &پیغام_expiry_interval;
	}

	بنیاد_پیغام = mosquitto_calloc(1, حجم(ساخت mosquitto_بنیاد_پیغام));
	اگر(بنیاد_پیغام == تہی){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
		rc = MOSQ_ERR_NOMEM;
		جاؤ cleanup;
	}

	بنیاد_پیغام->ڈیٹا.store_id = chunk.F.store_id;
	بنیاد_پیغام->ڈیٹا.ماخذ_mid = chunk.F.ماخذ_mid;
	بنیاد_پیغام->ڈیٹا.topic = chunk.topic;
	بنیاد_پیغام->ڈیٹا.qos = chunk.F.qos;
	بنیاد_پیغام->ڈیٹا.payloadlen = chunk.F.payloadlen;
	بنیاد_پیغام->ڈیٹا.retain = chunk.F.retain;
	بنیاد_پیغام->ڈیٹا.properties = chunk.properties;
	بنیاد_پیغام->ڈیٹا.payload = chunk.payload;
	بنیاد_پیغام->ماخذ_listener = chunk.ماخذ.listener;

	rc = db_پیغام_store(&chunk.ماخذ, بنیاد_پیغام, p_پیغام_expiry_interval,
			mosq_mo_client);

	mosquitto_FREE(chunk.ماخذ.id);
	mosquitto_FREE(chunk.ماخذ.username);

	اگر(rc == MOSQ_ERR_SUCCESS){
		بنیاد_پیغام_گنتی++;
		واپس MOSQ_ERR_SUCCESS;
	}ورنہ{
		واپس rc;
	}
cleanup:
	mosquitto_FREE(chunk.ماخذ.id);
	mosquitto_FREE(chunk.ماخذ.username);
	mosquitto_FREE(chunk.topic);
	mosquitto_FREE(chunk.payload);
	واپس rc;
}


ساکن صحیح persist__retain_chunk_restore(FILE *db_fptr)
{
	ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام;
	ساخت P_retain chunk;
	صحیح rc;
	حرف **بانٹو_topics;
	حرف *local_topic;

	memset(&chunk, 0, حجم(ساخت P_retain));

	اگر(db_نسخہ == 6 || db_نسخہ == 5){
		rc = persist__chunk_retain_read_v56(db_fptr, &chunk);
	}ورنہ{
		rc = persist__chunk_retain_read_v234(db_fptr, &chunk);
	}
	اگر(rc){
		واپس rc;
	}

	HASH_ڈھونڈو(hh, db.پیغام_store, &chunk.F.store_id, حجم(chunk.F.store_id), بنیاد_پیغام);
	اگر(بنیاد_پیغام){
		rc = sub__topic_tokenise(بنیاد_پیغام->ڈیٹا.topic, &local_topic, &بانٹو_topics, تہی);
		اگر(rc){
			واپس rc;
		}
		retain__store(بنیاد_پیغام->ڈیٹا.topic, بنیاد_پیغام, بانٹو_topics, true);
		mosquitto_FREE(local_topic);
		mosquitto_FREE(بانٹو_topics);
		retained_گنتی++;
	}ورنہ{
		/* Can't find the message - probably expired */
	}
	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح persist__sub_chunk_restore(FILE *db_fptr)
{
	ساخت P_sub chunk;
	صحیح rc;
	ساخت mosquitto_subscription sub;

	memset(&chunk, 0, حجم(ساخت P_sub));

	اگر(db_نسخہ == 6 || db_نسخہ == 5){
		rc = persist__chunk_sub_read_v56(db_fptr, &chunk);
	}ورنہ{
		rc = persist__chunk_sub_read_v234(db_fptr, &chunk);
	}
	اگر(rc){
		واپس rc;
	}

	sub.clientid = chunk.clientid;
	sub.topic_filter = chunk.topic;
	sub.options = chunk.F.qos | chunk.F.options;
	sub.identifier = chunk.F.identifier;
	rc = persist__restore_sub(&sub);

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.topic);
	اگر(rc == 0){
		subscription_گنتی++;
	}

	واپس rc;
}


صحیح persist_chunk_سرخی_read(FILE *db_fptr, uint32_t *chunk, uint32_t *لمبائی)
{
	اگر(db_نسخہ == 6 || db_نسخہ == 5){
		واپس persist_chunk_سرخی_read_v56(db_fptr, chunk, لمبائی);
	}ورنہ{
		واپس persist_chunk_سرخی_read_v234(db_fptr, chunk, لمبائی);
	}
}


صحیح persist__restore(خالی)
{
	FILE *fptr;
	حرف سرخی[15];
	صحیح rc = 0;
	uint32_t چکری_جانچ;
	uint32_t i32temp;
	uint32_t chunk, لمبائی;
	size_t rlen;
	حرف *err;
	ساخت PF_cfg cfg_chunk;

	أكد(db.config);

	اگر(!db.config->persistence || db.config->persistence_filepath == تہی){
		واپس MOSQ_ERR_SUCCESS;
	}

	db.پیغام_store = تہی;
	بنیاد_پیغام_گنتی = 0;
	retained_گنتی = 0;
	client_گنتی = 0;
	subscription_گنتی = 0;
	client_پیغام_گنتی = 0;

	fptr = mosquitto_fopen(db.config->persistence_filepath, "rb", true);
	اگر(fptr == تہی){
		واپس MOSQ_ERR_SUCCESS;
	}
	rlen = fread(&سرخی, 1, 15, fptr);
	اگر(rlen == 0){
		fclose(fptr);
		log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Persistence file is empty.");
		واپس 0;
	}ورنہ اگر(rlen != 15){
		جاؤ error;
	}
	اگر(!memcmp(سرخی, magic, 15)){
		/* Restore DB as normal */
		read_e(fptr, &چکری_جانچ, حجم(uint32_t));
		read_e(fptr, &i32temp, حجم(uint32_t));
		db_نسخہ = ntohl(i32temp);
		/* IMPORTANT - this is where compatibility checks are made.
		 * Is your DB change still compatible with previous versions?
		 */
		اگر(db_نسخہ != MOSQ_DB_نسخہ){
			اگر(db_نسخہ == 5){
				/* Addition of username and listener_port to client chunk in v6 */
			}ورنہ اگر(db_نسخہ == 4){
			}ورنہ اگر(db_نسخہ == 3){
				/* Addition of source_username and source_port to msg_store chunk in v4, v1.5.6 */
			}ورنہ اگر(db_نسخہ == 2){
				/* Addition of disconnect_t to client chunk in v3. */
			}ورنہ{
				fclose(fptr);
				log__printf(تہی, MOSQ_LOG_ERR, "Error: Unsupported persistent database format version %d (need version %d).", db_نسخہ, MOSQ_DB_نسخہ);
				واپس MOSQ_ERR_INVAL;
			}
		}

		جب(persist_chunk_سرخی_read(fptr, &chunk, &لمبائی) == MOSQ_ERR_SUCCESS){
			انتخاب(chunk){
				حالت DB_CHUNK_CFG:
					اگر(db_نسخہ == 6 || db_نسخہ == 5){
						rc = persist__chunk_cfg_read_v56(fptr, &cfg_chunk);
						اگر(rc){
							fclose(fptr);
							واپس rc;
						}
					}ورنہ{
						rc = persist__chunk_cfg_read_v234(fptr, &cfg_chunk);
						اگر(rc){
							fclose(fptr);
							واپس rc;
						}
					}
					اگر(cfg_chunk.dbid_سائز != حجم(dbid_t)){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Incompatible database configuration (dbid size is %d bytes, expected %lu)",
								cfg_chunk.dbid_سائز, (غیرعلامتی طویل)حجم(dbid_t));
						fclose(fptr);
						واپس MOSQ_ERR_INVAL;
					}
					db.آخری_db_id = cfg_chunk.آخری_db_id;
					توڑو;

				حالت DB_CHUNK_بنیاد_پیغام:
					rc = persist_بنیاد_پیغام_chunk_restore(fptr, لمبائی);
					اگر(rc){
						fclose(fptr);
						واپس rc;
					}
					توڑو;

				حالت DB_CHUNK_CLIENT_پیغام:
					rc = persist_client_پیغام_chunk_restore(fptr, لمبائی);
					اگر(rc){
						fclose(fptr);
						واپس rc;
					}
					توڑو;

				حالت DB_CHUNK_RETAIN:
					rc = persist__retain_chunk_restore(fptr);
					اگر(rc){
						fclose(fptr);
						واپس rc;
					}
					توڑو;

				حالت DB_CHUNK_SUB:
					rc = persist__sub_chunk_restore(fptr);
					اگر(rc){
						fclose(fptr);
						واپس rc;
					}
					توڑو;

				حالت DB_CHUNK_CLIENT:
					rc = persist__client_chunk_restore(fptr);
					اگر(rc){
						fclose(fptr);
						واپس rc;
					}
					توڑو;

				طےشدہ:
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Unsupported chunk \"%d\" in persistent database file. Ignoring.", chunk);
					اگر(fseek(fptr, لمبائی, SEEK_CUR) < 0){
						fclose(fptr);
						واپس MOSQ_ERR_INVAL;
					}
					توڑو;
			}
		}
	}ورنہ{
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to restore persistent database. Unrecognised file format.");
		rc = 1;
	}

	fclose(fptr);

	log__printf(تہی, MOSQ_LOG_INFO, "Restored %ld base messages", بنیاد_پیغام_گنتی);
	log__printf(تہی, MOSQ_LOG_INFO, "Restored %ld retained messages", retained_گنتی);
	log__printf(تہی, MOSQ_LOG_INFO, "Restored %ld clients", client_گنتی);
	log__printf(تہی, MOSQ_LOG_INFO, "Restored %ld subscriptions", subscription_گنتی);
	log__printf(تہی, MOSQ_LOG_INFO, "Restored %ld client messages", client_پیغام_گنتی);

	واپس rc;
error:
	err = strerror(errno);
	log__printf(تہی, MOSQ_LOG_ERR, "Error: %s.", err);
	اگر(fptr){
		fclose(fptr);
	}
	واپس MOSQ_ERR_ERRNO;
}


ساکن صحیح persist__restore_sub(ثابت ساخت mosquitto_subscription *sub)
{
	ساخت mosquitto *context;

	اگر(!sub->clientid){
		log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no client id, ignoring.");
		واپس MOSQ_ERR_SUCCESS;
	}
	اگر(!sub->topic_filter){
		log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no topic filter, ignoring.");
		واپس MOSQ_ERR_SUCCESS;
	}

	context = persist_ڈھونڈو_or_جوڑو_context(sub->clientid, 0);
	اگر(!context){
		واپس MOSQ_ERR_INVAL;
	}
	واپس sub_جوڑو(context, sub);
}

#اگر_ختم
