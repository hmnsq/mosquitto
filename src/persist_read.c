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
ساكن long base_ڤسنن_کيراءن = 0;
ساكن long retained_کيراءن = 0;
ساكن long client_کيراءن = 0;
ساكن long subscription_کيراءن = 0;
ساكن long client_ڤسنن_کيراءن = 0;

ساكن صحيح persist__restore_sub(ثابت بنية mosquitto_subscription *sub);

ساكن بنية mosquitto *persist__find_or_add_context(ثابت حرف *clientid, uint16_t last_mid)
{
	بنية mosquitto *context;

	إذا(!clientid){
		أرجع عدم;
	}

	context = عدم;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
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

		context->clean_start = false;

		context__add_to_by_id(context);
	}
	إذا(last_mid){
		context->last_mid = last_mid;
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


ساكن صحيح persist_client_ڤسنن_restore(بنية P_client_ڤسنن *chunk)
{
	بنية mosquitto_client_ڤسنن *cmsg;
	بنية mosquitto_base_ڤسنن *ڤسنن;
	بنية mosquitto *context;
	بنية mosquitto_ڤسنن_data *ڤسنن_data;

	HASH_FIND(hh, db.ڤسنن_store, &chunk->F.store_id, حجم(chunk->F.store_id), ڤسنن);
	إذا(!ڤسنن){
		/* Can't find message - probably expired */
		أرجع MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk->clientid, 0);
	إذا(!context){
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Persistence file contains client message with no matching client. File may be corrupt.");
		أرجع 0;
	}

	cmsg = mosquitto_calloc(1, حجم(بنية mosquitto_client_ڤسنن));
	إذا(!cmsg){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		أرجع MOSQ_ERR_NOMEM;
	}

	cmsg->next = عدم;
	cmsg->base_ڤسنن = عدم;
	cmsg->data.cmsg_id = ++context->last_cmsg_id;
	cmsg->data.mid = chunk->F.mid;
	cmsg->data.qos = chunk->F.qos;
	cmsg->data.retain = (chunk->F.retain_dup&0xF0)>>4;
	cmsg->data.direction = chunk->F.direction;
	cmsg->data.state = chunk->F.state;
	cmsg->data.dup = chunk->F.retain_dup&0x0F;
	cmsg->data.subscription_identifier = chunk->subscription_identifier;

	cmsg->base_ڤسنن = ڤسنن;
	db_ڤسنن_store_ref_inc(cmsg->base_ڤسنن);

	إذا(cmsg->data.direction == mosq_md_out){
		ڤسنن_data = &context->msgs_out;
	}وإلا{
		ڤسنن_data = &context->msgs_in;
	}

	إذا(chunk->F.state == mosq_ms_queued || (chunk->F.qos > 0 && ڤسنن_data->inflight_quota == 0)){
		DL_APPEND(ڤسنن_data->queued, cmsg);
		db_ڤسنن_add_to_queued_stats(ڤسنن_data, cmsg);
	}وإلا{
		DL_APPEND(ڤسنن_data->inflight, cmsg);
		إذا(chunk->F.qos > 0 && ڤسنن_data->inflight_quota > 0){
			ڤسنن_data->inflight_quota--;
		}
		db_ڤسنن_add_to_inflight_stats(ڤسنن_data, cmsg);
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح persist__client_chunk_restore(FILE *db_fptr)
{
	صحيح rc = 0;
	بنية mosquitto *context;
	بنية P_client chunk;

	memset(&chunk, 0, حجم(بنية P_client));

	إذا(db_version == 6 || db_version == 5){
		rc = persist__chunk_client_read_v56(db_fptr, &chunk, db_version);
	}وإلا{
		rc = persist__chunk_client_read_v234(db_fptr, &chunk, db_version);
	}
	إذا(rc > 0){
		أرجع rc;
	}وإلا إذا(rc < 0){
		/* Client not loaded, but otherwise not an error */
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Empty client entry found in persistence database, it may be corrupt.");
		أرجع MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk.clientid, chunk.F.last_mid);
	إذا(context){
		context->session_expiry_time = chunk.F.session_expiry_time;
		context->session_expiry_interval = chunk.F.session_expiry_interval;
		إذا(chunk.username && !context->username){
			/* username is not freed here, it is now owned by context */
			context->username = chunk.username;
			chunk.username = عدم;
		}
		/* in per_listener_settings mode, try to find the listener by persisted port */
		إذا(db.config->per_listener_settings && !context->listener && chunk.F.listener_port > 0){
			لكل(صحيح i=0; i < db.config->listener_کيراءن; i++){
				إذا(db.config->listeners[i].port == chunk.F.listener_port){
					context->listener = &db.config->listeners[i];
					اكسر;
				}
			}
		}
		session_expiry__add_from_persistence(context, chunk.F.session_expiry_time);
	}وإلا{
		rc = 1;
	}

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.username);
	إذا(rc == 0){
		client_کيراءن++;
	}
	أرجع rc;
}


ساكن صحيح persist_client_ڤسنن_chunk_restore(FILE *db_fptr, uint32_t length)
{
	بنية P_client_ڤسنن chunk;
	صحيح rc;

	memset(&chunk, 0, حجم(بنية P_client_ڤسنن));

	إذا(db_version == 6 || db_version == 5){
		rc = persist_chunk_client_ڤسنن_read_v56(db_fptr, &chunk, length);
	}وإلا{
		rc = persist_chunk_client_ڤسنن_read_v234(db_fptr, &chunk);
	}
	إذا(rc){
		أرجع rc;
	}

	rc = persist_client_ڤسنن_restore(&chunk);
	mosquitto_FREE(chunk.clientid);

	إذا(rc == 0){
		client_ڤسنن_کيراءن++;
	}
	أرجع rc;
}


ساكن صحيح persist_base_ڤسنن_chunk_restore(FILE *db_fptr, uint32_t length)
{
	بنية P_base_ڤسنن chunk;
	بنية mosquitto_base_ڤسنن *base_ڤسنن = عدم;
	int64_t message_expiry_interval64;
	uint32_t message_expiry_interval;
	uint32_t *p_message_expiry_interval = عدم;
	صحيح rc = 0;

	memset(&chunk, 0, حجم(بنية P_base_ڤسنن));

	إذا(db_version == 6 || db_version == 5){
		rc = persist_chunk_base_ڤسنن_read_v56(db_fptr, &chunk, length);
	}وإلا{
		rc = persist_chunk_base_ڤسنن_read_v234(db_fptr, &chunk, db_version);
	}
	إذا(rc){
		أرجع rc;
	}
	إذا(chunk.F.topic_len == 0){
		rc = MOSQ_ERR_INVAL;
		اذهب cleanup;
	}

	إذا(chunk.F.source_port){
		لكل(صحيح i=0; i<db.config->listener_کيراءن; i++){
			إذا(db.config->listeners[i].port == chunk.F.source_port){
				chunk.source.listener = &db.config->listeners[i];
				اكسر;
			}
		}
	}

	إذا(chunk.F.expiry_time > 0){
		message_expiry_interval64 = chunk.F.expiry_time - time(عدم);
		إذا(message_expiry_interval64 < 0 || message_expiry_interval64 > UINT32_MAX){
			/* Expired message */
			rc = MOSQ_ERR_SUCCESS;
			اذهب cleanup;
		}وإلا{
			message_expiry_interval = (uint32_t)message_expiry_interval64;
		}
		p_message_expiry_interval = &message_expiry_interval;
	}

	base_ڤسنن = mosquitto_calloc(1, حجم(بنية mosquitto_base_ڤسنن));
	إذا(base_ڤسنن == عدم){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		rc = MOSQ_ERR_NOMEM;
		اذهب cleanup;
	}

	base_ڤسنن->data.store_id = chunk.F.store_id;
	base_ڤسنن->data.source_mid = chunk.F.source_mid;
	base_ڤسنن->data.topic = chunk.topic;
	base_ڤسنن->data.qos = chunk.F.qos;
	base_ڤسنن->data.payloadlen = chunk.F.payloadlen;
	base_ڤسنن->data.retain = chunk.F.retain;
	base_ڤسنن->data.properties = chunk.properties;
	base_ڤسنن->data.payload = chunk.payload;
	base_ڤسنن->source_listener = chunk.source.listener;

	rc = db__message_store(&chunk.source, base_ڤسنن, p_message_expiry_interval,
			mosq_mo_client);

	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);

	إذا(rc == MOSQ_ERR_SUCCESS){
		base_ڤسنن_کيراءن++;
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا{
		أرجع rc;
	}
cleanup:
	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);
	mosquitto_FREE(chunk.topic);
	mosquitto_FREE(chunk.payload);
	أرجع rc;
}


ساكن صحيح persist__retain_chunk_restore(FILE *db_fptr)
{
	بنية mosquitto_base_ڤسنن *base_ڤسنن;
	بنية P_retain chunk;
	صحيح rc;
	حرف **split_topics;
	حرف *local_topic;

	memset(&chunk, 0, حجم(بنية P_retain));

	إذا(db_version == 6 || db_version == 5){
		rc = persist__chunk_retain_read_v56(db_fptr, &chunk);
	}وإلا{
		rc = persist__chunk_retain_read_v234(db_fptr, &chunk);
	}
	إذا(rc){
		أرجع rc;
	}

	HASH_FIND(hh, db.ڤسنن_store, &chunk.F.store_id, حجم(chunk.F.store_id), base_ڤسنن);
	إذا(base_ڤسنن){
		rc = sub__topic_tokenise(base_ڤسنن->data.topic, &local_topic, &split_topics, عدم);
		إذا(rc){
			أرجع rc;
		}
		retain__store(base_ڤسنن->data.topic, base_ڤسنن, split_topics, true);
		mosquitto_FREE(local_topic);
		mosquitto_FREE(split_topics);
		retained_کيراءن++;
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

	إذا(db_version == 6 || db_version == 5){
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
		subscription_کيراءن++;
	}

	أرجع rc;
}


صحيح persist__chunk_header_read(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	إذا(db_version == 6 || db_version == 5){
		أرجع persist__chunk_header_read_v56(db_fptr, chunk, length);
	}وإلا{
		أرجع persist__chunk_header_read_v234(db_fptr, chunk, length);
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

	إذا(!db.config->persistence || db.config->persistence_filepath == عدم){
		أرجع MOSQ_ERR_SUCCESS;
	}

	db.ڤسنن_store = عدم;
	base_ڤسنن_کيراءن = 0;
	retained_کيراءن = 0;
	client_کيراءن = 0;
	subscription_کيراءن = 0;
	client_ڤسنن_کيراءن = 0;

	fptr = mosquitto_fopen(db.config->persistence_filepath, "rb", true);
	إذا(fptr == عدم){
		أرجع MOSQ_ERR_SUCCESS;
	}
	rlen = fread(&header, 1, 15, fptr);
	إذا(rlen == 0){
		fclose(fptr);
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Persistence file is empty.");
		أرجع 0;
	}وإلا إذا(rlen != 15){
		اذهب error;
	}
	إذا(!memcmp(header, magic, 15)){
		/* Restore DB as normal */
		read_e(fptr, &crc, حجم(uint32_t));
		read_e(fptr, &i32temp, حجم(uint32_t));
		db_version = ntohl(i32temp);
		/* IMPORTANT - this is where compatibility checks are made.
		 * Is your DB change still compatible with previous versions?
		 */
		إذا(db_version != MOSQ_DB_VERSION){
			إذا(db_version == 5){
				/* Addition of username and listener_port to client chunk in v6 */
			}وإلا إذا(db_version == 4){
			}وإلا إذا(db_version == 3){
				/* Addition of source_username and source_port to msg_store chunk in v4, v1.5.6 */
			}وإلا إذا(db_version == 2){
				/* Addition of disconnect_t to client chunk in v3. */
			}وإلا{
				fclose(fptr);
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Unsupported persistent database format version %d (need version %d).", db_version, MOSQ_DB_VERSION);
				أرجع MOSQ_ERR_INVAL;
			}
		}

		طالما(persist__chunk_header_read(fptr, &chunk, &length) == MOSQ_ERR_SUCCESS){
			بدل(chunk){
				حالة DB_CHUNK_CFG:
					إذا(db_version == 6 || db_version == 5){
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
					إذا(cfg_chunk.dbid_size != حجم(dbid_t)){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Incompatible database configuration (dbid size is %d bytes, expected %lu)",
								cfg_chunk.dbid_size, (unsigned long)حجم(dbid_t));
						fclose(fptr);
						أرجع MOSQ_ERR_INVAL;
					}
					db.last_db_id = cfg_chunk.last_db_id;
					اكسر;

				حالة DB_CHUNK_BASE_ڤسنن:
					rc = persist_base_ڤسنن_chunk_restore(fptr, length);
					إذا(rc){
						fclose(fptr);
						أرجع rc;
					}
					اكسر;

				حالة DB_CHUNK_CLIENT_ڤسنن:
					rc = persist_client_ڤسنن_chunk_restore(fptr, length);
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
					إذا(fseek(fptr, length, SEEK_CUR) < 0){
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

	log__printf(عدم, MOSQ_LOG_INFO, "Restored %ld base messages", base_ڤسنن_کيراءن);
	log__printf(عدم, MOSQ_LOG_INFO, "Restored %ld retained messages", retained_کيراءن);
	log__printf(عدم, MOSQ_LOG_INFO, "Restored %ld clients", client_کيراءن);
	log__printf(عدم, MOSQ_LOG_INFO, "Restored %ld subscriptions", subscription_کيراءن);
	log__printf(عدم, MOSQ_LOG_INFO, "Restored %ld client messages", client_ڤسنن_کيراءن);

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

	context = persist__find_or_add_context(sub->clientid, 0);
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}
	أرجع sub__add(context, sub);
}

#endif
