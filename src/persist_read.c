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

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#ضمن <arpa/inet.h>
#endif
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

uint32_t db_version;

ثابت unsigned حرف magic[15] = {0x00, 0xB5, 0x00, 'm', 'o', 's', 'q', 'u', 'i', 't', 't', 'o', ' ', 'd', 'b'};
ساكن long base_msg_count = 0;
ساكن long retained_count = 0;
ساكن long client_count = 0;
ساكن long subscription_count = 0;
ساكن long client_msg_count = 0;

ساكن صحيح persist__restore_sub(ثابت بنية mosquitto_subscription *sub);

ساكن بنية mosquitto *persist__find_or_add_context(ثابت حرف *clientid, uint16_t last_mid)
{
	بنية mosquitto *context;

	אם(!clientid){
		החזר عدم;
	}

	context = عدم;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	אם(!context){
		context = context__init();
		אם(!context){
			החזר عدم;
		}
		context->id = mosquitto_strdup(clientid);
		אם(!context->id){
			mosquitto_FREE(context);
			החזר عدم;
		}

		context->clean_start = false;

		context__add_to_by_id(context);
	}
	אם(last_mid){
		context->last_mid = last_mid;
	}
	החזר context;
}


صحيح persist__read_string_len(FILE *db_fptr, حرف **str, uint16_t len)
{
	حرف *s = عدم;

	אם(len){
		s = mosquitto_malloc(len+1U);
		אם(!s){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
			החזר MOSQ_ERR_NOMEM;
		}
		אם(fread(s, 1, len, db_fptr) != len){
			mosquitto_FREE(s);
			החזר MOSQ_ERR_NOMEM;
		}
		s[len] = '\0';
	}

	*str = s;
	החזר MOSQ_ERR_SUCCESS;
}


صحيح persist__read_string(FILE *db_fptr, حرف **str)
{
	uint16_t i16temp;
	uint16_t slen;

	אם(fread(&i16temp, 1, حجم(uint16_t), db_fptr) != حجم(uint16_t)){
		החזר MOSQ_ERR_INVAL;
	}

	slen = ntohs(i16temp);
	החזר persist__read_string_len(db_fptr, str, slen);
}


ساكن صحيح persist__client_msg_restore(بنية P_client_msg *chunk)
{
	بنية mosquitto__client_msg *cmsg;
	بنية mosquitto__base_msg *msg;
	بنية mosquitto *context;
	بنية mosquitto_msg_data *msg_data;

	HASH_FIND(hh, db.msg_store, &chunk->F.store_id, حجم(chunk->F.store_id), msg);
	אם(!msg){
		/* Can't find message - probably expired */
		החזר MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk->clientid, 0);
	אם(!context){
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Persistence file contains client message with no matching client. File may be corrupt.");
		החזר 0;
	}

	cmsg = mosquitto_calloc(1, حجم(بنية mosquitto__client_msg));
	אם(!cmsg){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		החזר MOSQ_ERR_NOMEM;
	}

	cmsg->next = عدم;
	cmsg->base_msg = عدم;
	cmsg->data.cmsg_id = ++context->last_cmsg_id;
	cmsg->data.mid = chunk->F.mid;
	cmsg->data.qos = chunk->F.qos;
	cmsg->data.retain = (chunk->F.retain_dup&0xF0)>>4;
	cmsg->data.direction = chunk->F.direction;
	cmsg->data.state = chunk->F.state;
	cmsg->data.dup = chunk->F.retain_dup&0x0F;
	cmsg->data.subscription_identifier = chunk->subscription_identifier;

	cmsg->base_msg = msg;
	db__msg_store_ref_inc(cmsg->base_msg);

	אם(cmsg->data.direction == mosq_md_out){
		msg_data = &context->msgs_out;
	}אחרת{
		msg_data = &context->msgs_in;
	}

	אם(chunk->F.state == mosq_ms_queued || (chunk->F.qos > 0 && msg_data->inflight_quota == 0)){
		DL_APPEND(msg_data->queued, cmsg);
		db__msg_add_to_queued_stats(msg_data, cmsg);
	}אחרת{
		DL_APPEND(msg_data->inflight, cmsg);
		אם(chunk->F.qos > 0 && msg_data->inflight_quota > 0){
			msg_data->inflight_quota--;
		}
		db__msg_add_to_inflight_stats(msg_data, cmsg);
	}

	החזר MOSQ_ERR_SUCCESS;
}


ساكن صحيح persist__client_chunk_restore(FILE *db_fptr)
{
	صحيح rc = 0;
	بنية mosquitto *context;
	بنية P_client chunk;

	memset(&chunk, 0, حجم(بنية P_client));

	אם(db_version == 6 || db_version == 5){
		rc = persist__chunk_client_read_v56(db_fptr, &chunk, db_version);
	}אחרת{
		rc = persist__chunk_client_read_v234(db_fptr, &chunk, db_version);
	}
	אם(rc > 0){
		החזר rc;
	}אחרת אם(rc < 0){
		/* Client not loaded, but otherwise not an error */
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Empty client entry found in persistence database, it may be corrupt.");
		החזר MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk.clientid, chunk.F.last_mid);
	אם(context){
		context->session_expiry_time = chunk.F.session_expiry_time;
		context->session_expiry_interval = chunk.F.session_expiry_interval;
		אם(chunk.username && !context->username){
			/* username is not freed here, it is now owned by context */
			context->username = chunk.username;
			chunk.username = عدم;
		}
		/* in per_listener_settings mode, try to find the listener by persisted port */
		אם(db.config->per_listener_settings && !context->listener && chunk.F.listener_port > 0){
			עבור(صحيح i=0; i < db.config->listener_count; i++){
				אם(db.config->listeners[i].port == chunk.F.listener_port){
					context->listener = &db.config->listeners[i];
					اكسر;
				}
			}
		}
		session_expiry__add_from_persistence(context, chunk.F.session_expiry_time);
	}אחרת{
		rc = 1;
	}

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.username);
	אם(rc == 0){
		client_count++;
	}
	החזר rc;
}


ساكن صحيح persist__client_msg_chunk_restore(FILE *db_fptr, uint32_t length)
{
	بنية P_client_msg chunk;
	صحيح rc;

	memset(&chunk, 0, حجم(بنية P_client_msg));

	אם(db_version == 6 || db_version == 5){
		rc = persist__chunk_client_msg_read_v56(db_fptr, &chunk, length);
	}אחרת{
		rc = persist__chunk_client_msg_read_v234(db_fptr, &chunk);
	}
	אם(rc){
		החזר rc;
	}

	rc = persist__client_msg_restore(&chunk);
	mosquitto_FREE(chunk.clientid);

	אם(rc == 0){
		client_msg_count++;
	}
	החזר rc;
}


ساكن صحيح persist__base_msg_chunk_restore(FILE *db_fptr, uint32_t length)
{
	بنية P_base_msg chunk;
	بنية mosquitto__base_msg *base_msg = عدم;
	int64_t message_expiry_interval64;
	uint32_t message_expiry_interval;
	uint32_t *p_message_expiry_interval = عدم;
	صحيح rc = 0;

	memset(&chunk, 0, حجم(بنية P_base_msg));

	אם(db_version == 6 || db_version == 5){
		rc = persist__chunk_base_msg_read_v56(db_fptr, &chunk, length);
	}אחרת{
		rc = persist__chunk_base_msg_read_v234(db_fptr, &chunk, db_version);
	}
	אם(rc){
		החזר rc;
	}
	אם(chunk.F.topic_len == 0){
		rc = MOSQ_ERR_INVAL;
		اذهب cleanup;
	}

	אם(chunk.F.source_port){
		עבור(صحيح i=0; i<db.config->listener_count; i++){
			אם(db.config->listeners[i].port == chunk.F.source_port){
				chunk.source.listener = &db.config->listeners[i];
				اكسر;
			}
		}
	}

	אם(chunk.F.expiry_time > 0){
		message_expiry_interval64 = chunk.F.expiry_time - time(عدم);
		אם(message_expiry_interval64 < 0 || message_expiry_interval64 > UINT32_MAX){
			/* Expired message */
			rc = MOSQ_ERR_SUCCESS;
			اذهب cleanup;
		}אחרת{
			message_expiry_interval = (uint32_t)message_expiry_interval64;
		}
		p_message_expiry_interval = &message_expiry_interval;
	}

	base_msg = mosquitto_calloc(1, حجم(بنية mosquitto__base_msg));
	אם(base_msg == عدم){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		rc = MOSQ_ERR_NOMEM;
		اذهب cleanup;
	}

	base_msg->data.store_id = chunk.F.store_id;
	base_msg->data.source_mid = chunk.F.source_mid;
	base_msg->data.topic = chunk.topic;
	base_msg->data.qos = chunk.F.qos;
	base_msg->data.payloadlen = chunk.F.payloadlen;
	base_msg->data.retain = chunk.F.retain;
	base_msg->data.properties = chunk.properties;
	base_msg->data.payload = chunk.payload;
	base_msg->source_listener = chunk.source.listener;

	rc = db__message_store(&chunk.source, base_msg, p_message_expiry_interval,
			mosq_mo_client);

	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);

	אם(rc == MOSQ_ERR_SUCCESS){
		base_msg_count++;
		החזר MOSQ_ERR_SUCCESS;
	}אחרת{
		החזר rc;
	}
cleanup:
	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);
	mosquitto_FREE(chunk.topic);
	mosquitto_FREE(chunk.payload);
	החזר rc;
}


ساكن صحيح persist__retain_chunk_restore(FILE *db_fptr)
{
	بنية mosquitto__base_msg *base_msg;
	بنية P_retain chunk;
	صحيح rc;
	حرف **split_topics;
	حرف *local_topic;

	memset(&chunk, 0, حجم(بنية P_retain));

	אם(db_version == 6 || db_version == 5){
		rc = persist__chunk_retain_read_v56(db_fptr, &chunk);
	}אחרת{
		rc = persist__chunk_retain_read_v234(db_fptr, &chunk);
	}
	אם(rc){
		החזר rc;
	}

	HASH_FIND(hh, db.msg_store, &chunk.F.store_id, حجم(chunk.F.store_id), base_msg);
	אם(base_msg){
		rc = sub__topic_tokenise(base_msg->data.topic, &local_topic, &split_topics, عدم);
		אם(rc){
			החזר rc;
		}
		retain__store(base_msg->data.topic, base_msg, split_topics, true);
		mosquitto_FREE(local_topic);
		mosquitto_FREE(split_topics);
		retained_count++;
	}אחרת{
		/* Can't find the message - probably expired */
	}
	החזר MOSQ_ERR_SUCCESS;
}


ساكن صحيح persist__sub_chunk_restore(FILE *db_fptr)
{
	بنية P_sub chunk;
	صحيح rc;
	بنية mosquitto_subscription sub;

	memset(&chunk, 0, حجم(بنية P_sub));

	אם(db_version == 6 || db_version == 5){
		rc = persist__chunk_sub_read_v56(db_fptr, &chunk);
	}אחרת{
		rc = persist__chunk_sub_read_v234(db_fptr, &chunk);
	}
	אם(rc){
		החזר rc;
	}

	sub.clientid = chunk.clientid;
	sub.topic_filter = chunk.topic;
	sub.options = chunk.F.qos | chunk.F.options;
	sub.identifier = chunk.F.identifier;
	rc = persist__restore_sub(&sub);

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.topic);
	אם(rc == 0){
		subscription_count++;
	}

	החזר rc;
}


صحيح persist__chunk_header_read(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	אם(db_version == 6 || db_version == 5){
		החזר persist__chunk_header_read_v56(db_fptr, chunk, length);
	}אחרת{
		החזר persist__chunk_header_read_v234(db_fptr, chunk, length);
	}
}


صحيح persist__restore(فراغ)
{
	FILE *fptr;
	حرف header[15];
	صحيح rc = 0;
	uint32_t crc;
	uint32_t i32temp;
	uint32_t chunk, length;
	size_t rlen;
	حرف *err;
	بنية PF_cfg cfg_chunk;

	أكد(db.config);

	אם(!db.config->persistence || db.config->persistence_filepath == عدم){
		החזר MOSQ_ERR_SUCCESS;
	}

	db.msg_store = عدم;
	base_msg_count = 0;
	retained_count = 0;
	client_count = 0;
	subscription_count = 0;
	client_msg_count = 0;

	fptr = mosquitto_fopen(db.config->persistence_filepath, "rb", true);
	אם(fptr == عدم){
		החזר MOSQ_ERR_SUCCESS;
	}
	rlen = fread(&header, 1, 15, fptr);
	אם(rlen == 0){
		fclose(fptr);
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Persistence file is empty.");
		החזר 0;
	}אחרת אם(rlen != 15){
		اذهب error;
	}
	אם(!memcmp(header, magic, 15)){
		/* Restore DB as normal */
		read_e(fptr, &crc, حجم(uint32_t));
		read_e(fptr, &i32temp, حجم(uint32_t));
		db_version = ntohl(i32temp);
		/* IMPORTANT - this is where compatibility checks are made.
		 * Is your DB change still compatible with previous versions?
		 */
		אם(db_version != MOSQ_DB_VERSION){
			אם(db_version == 5){
				/* Addition of username and listener_port to client chunk in v6 */
			}אחרת אם(db_version == 4){
			}אחרת אם(db_version == 3){
				/* Addition of source_username and source_port to msg_store chunk in v4, v1.5.6 */
			}אחרת אם(db_version == 2){
				/* Addition of disconnect_t to client chunk in v3. */
			}אחרת{
				fclose(fptr);
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Unsupported persistent database format version %d (need version %d).", db_version, MOSQ_DB_VERSION);
				החזר MOSQ_ERR_INVAL;
			}
		}

		כלעוד(persist__chunk_header_read(fptr, &chunk, &length) == MOSQ_ERR_SUCCESS){
			بدل(chunk){
				حالة DB_CHUNK_CFG:
					אם(db_version == 6 || db_version == 5){
						rc = persist__chunk_cfg_read_v56(fptr, &cfg_chunk);
						אם(rc){
							fclose(fptr);
							החזר rc;
						}
					}אחרת{
						rc = persist__chunk_cfg_read_v234(fptr, &cfg_chunk);
						אם(rc){
							fclose(fptr);
							החזר rc;
						}
					}
					אם(cfg_chunk.dbid_size != حجم(dbid_t)){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Incompatible database configuration (dbid size is %d bytes, expected %lu)",
								cfg_chunk.dbid_size, (unsigned long)حجم(dbid_t));
						fclose(fptr);
						החזר MOSQ_ERR_INVAL;
					}
					db.last_db_id = cfg_chunk.last_db_id;
					اكسر;

				حالة DB_CHUNK_BASE_MSG:
					rc = persist__base_msg_chunk_restore(fptr, length);
					אם(rc){
						fclose(fptr);
						החזר rc;
					}
					اكسر;

				حالة DB_CHUNK_CLIENT_MSG:
					rc = persist__client_msg_chunk_restore(fptr, length);
					אם(rc){
						fclose(fptr);
						החזר rc;
					}
					اكسر;

				حالة DB_CHUNK_RETAIN:
					rc = persist__retain_chunk_restore(fptr);
					אם(rc){
						fclose(fptr);
						החזר rc;
					}
					اكسر;

				حالة DB_CHUNK_SUB:
					rc = persist__sub_chunk_restore(fptr);
					אם(rc){
						fclose(fptr);
						החזר rc;
					}
					اكسر;

				حالة DB_CHUNK_CLIENT:
					rc = persist__client_chunk_restore(fptr);
					אם(rc){
						fclose(fptr);
						החזר rc;
					}
					اكسر;

				افتراضي:
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Unsupported chunk \"%d\" in persistent database file. Ignoring.", chunk);
					אם(fseek(fptr, length, SEEK_CUR) < 0){
						fclose(fptr);
						החזר MOSQ_ERR_INVAL;
					}
					اكسر;
			}
		}
	}אחרת{
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to restore persistent database. Unrecognised file format.");
		rc = 1;
	}

	fclose(fptr);

	log__printf(عدم, MOSQ_LOG_INFO, "Restored %ld base messages", base_msg_count);
	log__printf(عدم, MOSQ_LOG_INFO, "Restored %ld retained messages", retained_count);
	log__printf(عدم, MOSQ_LOG_INFO, "Restored %ld clients", client_count);
	log__printf(عدم, MOSQ_LOG_INFO, "Restored %ld subscriptions", subscription_count);
	log__printf(عدم, MOSQ_LOG_INFO, "Restored %ld client messages", client_msg_count);

	החזר rc;
error:
	err = strerror(errno);
	log__printf(عدم, MOSQ_LOG_ERR, "Error: %s.", err);
	אם(fptr){
		fclose(fptr);
	}
	החזר MOSQ_ERR_ERRNO;
}


ساكن صحيح persist__restore_sub(ثابت بنية mosquitto_subscription *sub)
{
	بنية mosquitto *context;

	אם(!sub->clientid){
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no client id, ignoring.");
		החזר MOSQ_ERR_SUCCESS;
	}
	אם(!sub->topic_filter){
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no topic filter, ignoring.");
		החזר MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(sub->clientid, 0);
	אם(!context){
		החזר MOSQ_ERR_INVAL;
	}
	החזר sub__add(context, sub);
}

#endif
