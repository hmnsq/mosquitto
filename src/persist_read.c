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

#அந்தர்பாவ "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#அந்தர்பாவ <arpa/inet.h>
#endif
#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <errno.h>
#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <sys/stat.h>
#அந்தர்பாவ <time.h>
#அந்தர்பாவ <utlist.h>

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "persist.h"
#அந்தர்பாவ "util_mosq.h"

uint32_t db_version;

மாறிலி unsigned எழுத்து magic[15] = {0x00, 0xB5, 0x00, 'm', 'o', 's', 'q', 'u', 'i', 't', 't', 'o', ' ', 'd', 'b'};
நிலையான long base_செய்தி_மொத்தம் = 0;
நிலையான long retained_மொத்தம் = 0;
நிலையான long client_மொத்தம் = 0;
நிலையான long subscription_மொத்தம் = 0;
நிலையான long client_செய்தி_மொத்தம் = 0;

நிலையான முழு persist__restore_sub(மாறிலி கட்டமைப்பு mosquitto_subscription *sub);

நிலையான கட்டமைப்பு mosquitto *persist__find_or_add_context(மாறிலி எழுத்து *clientid, uint16_t last_mid)
{
	கட்டமைப்பு mosquitto *context;

	எனில்(!clientid){
		திரும்பு வெற்று;
	}

	context = வெற்று;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	எனில்(!context){
		context = context__init();
		எனில்(!context){
			திரும்பு வெற்று;
		}
		context->id = mosquitto_strdup(clientid);
		எனில்(!context->id){
			mosquitto_FREE(context);
			திரும்பு வெற்று;
		}

		context->clean_start = false;

		context__add_to_by_id(context);
	}
	எனில்(last_mid){
		context->last_mid = last_mid;
	}
	திரும்பு context;
}


முழு persist__read_string_len(FILE *db_fptr, எழுத்து **str, uint16_t len)
{
	எழுத்து *s = வெற்று;

	எனில்(len){
		s = mosquitto_malloc(len+1U);
		எனில்(!s){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
			திரும்பு MOSQ_ERR_NOMEM;
		}
		எனில்(fread(s, 1, len, db_fptr) != len){
			mosquitto_FREE(s);
			திரும்பு MOSQ_ERR_NOMEM;
		}
		s[len] = '\0';
	}

	*str = s;
	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு persist__read_string(FILE *db_fptr, எழுத்து **str)
{
	uint16_t i16temp;
	uint16_t slen;

	எனில்(fread(&i16temp, 1, அளவு(uint16_t), db_fptr) != அளவு(uint16_t)){
		திரும்பு MOSQ_ERR_INVAL;
	}

	slen = ntohs(i16temp);
	திரும்பு persist__read_string_len(db_fptr, str, slen);
}


நிலையான முழு persist_client_செய்தி_restore(கட்டமைப்பு P_client_செய்தி *chunk)
{
	கட்டமைப்பு mosquitto_client_செய்தி *cmsg;
	கட்டமைப்பு mosquitto_base_செய்தி *செய்தி;
	கட்டமைப்பு mosquitto *context;
	கட்டமைப்பு mosquitto_செய்தி_data *செய்தி_data;

	HASH_FIND(hh, db.செய்தி_store, &chunk->F.store_id, அளவு(chunk->F.store_id), செய்தி);
	எனில்(!செய்தி){
		/* Can't find message - probably expired */
		திரும்பு MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk->clientid, 0);
	எனில்(!context){
		log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Persistence file contains client message with no matching client. File may be corrupt.");
		திரும்பு 0;
	}

	cmsg = mosquitto_calloc(1, அளவு(கட்டமைப்பு mosquitto_client_செய்தி));
	எனில்(!cmsg){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
		திரும்பு MOSQ_ERR_NOMEM;
	}

	cmsg->next = வெற்று;
	cmsg->base_செய்தி = வெற்று;
	cmsg->data.cmsg_id = ++context->last_cmsg_id;
	cmsg->data.mid = chunk->F.mid;
	cmsg->data.qos = chunk->F.qos;
	cmsg->data.retain = (chunk->F.retain_dup&0xF0)>>4;
	cmsg->data.direction = chunk->F.direction;
	cmsg->data.state = chunk->F.state;
	cmsg->data.dup = chunk->F.retain_dup&0x0F;
	cmsg->data.subscription_identifier = chunk->subscription_identifier;

	cmsg->base_செய்தி = செய்தி;
	db_செய்தி_store_ref_inc(cmsg->base_செய்தி);

	எனில்(cmsg->data.direction == mosq_md_out){
		செய்தி_data = &context->msgs_out;
	}இல்லையெனில்{
		செய்தி_data = &context->msgs_in;
	}

	எனில்(chunk->F.state == mosq_ms_queued || (chunk->F.qos > 0 && செய்தி_data->inflight_quota == 0)){
		DL_APPEND(செய்தி_data->queued, cmsg);
		db_செய்தி_add_to_queued_stats(செய்தி_data, cmsg);
	}இல்லையெனில்{
		DL_APPEND(செய்தி_data->inflight, cmsg);
		எனில்(chunk->F.qos > 0 && செய்தி_data->inflight_quota > 0){
			செய்தி_data->inflight_quota--;
		}
		db_செய்தி_add_to_inflight_stats(செய்தி_data, cmsg);
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு persist__client_chunk_restore(FILE *db_fptr)
{
	முழு rc = 0;
	கட்டமைப்பு mosquitto *context;
	கட்டமைப்பு P_client chunk;

	memset(&chunk, 0, அளவு(கட்டமைப்பு P_client));

	எனில்(db_version == 6 || db_version == 5){
		rc = persist__chunk_client_read_v56(db_fptr, &chunk, db_version);
	}இல்லையெனில்{
		rc = persist__chunk_client_read_v234(db_fptr, &chunk, db_version);
	}
	எனில்(rc > 0){
		திரும்பு rc;
	}இல்லையெனில் எனில்(rc < 0){
		/* Client not loaded, but otherwise not an error */
		log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Empty client entry found in persistence database, it may be corrupt.");
		திரும்பு MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk.clientid, chunk.F.last_mid);
	எனில்(context){
		context->session_expiry_time = chunk.F.session_expiry_time;
		context->session_expiry_interval = chunk.F.session_expiry_interval;
		எனில்(chunk.username && !context->username){
			/* username is not freed here, it is now owned by context */
			context->username = chunk.username;
			chunk.username = வெற்று;
		}
		/* in per_listener_settings mode, try to find the listener by persisted port */
		எனில்(db.config->per_listener_settings && !context->listener && chunk.F.listener_port > 0){
			க்கு(முழு i=0; i < db.config->listener_மொத்தம்; i++){
				எனில்(db.config->listeners[i].port == chunk.F.listener_port){
					context->listener = &db.config->listeners[i];
					நிறுத்து;
				}
			}
		}
		session_expiry__add_from_persistence(context, chunk.F.session_expiry_time);
	}இல்லையெனில்{
		rc = 1;
	}

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.username);
	எனில்(rc == 0){
		client_மொத்தம்++;
	}
	திரும்பு rc;
}


நிலையான முழு persist_client_செய்தி_chunk_restore(FILE *db_fptr, uint32_t length)
{
	கட்டமைப்பு P_client_செய்தி chunk;
	முழு rc;

	memset(&chunk, 0, அளவு(கட்டமைப்பு P_client_செய்தி));

	எனில்(db_version == 6 || db_version == 5){
		rc = persist_chunk_client_செய்தி_read_v56(db_fptr, &chunk, length);
	}இல்லையெனில்{
		rc = persist_chunk_client_செய்தி_read_v234(db_fptr, &chunk);
	}
	எனில்(rc){
		திரும்பு rc;
	}

	rc = persist_client_செய்தி_restore(&chunk);
	mosquitto_FREE(chunk.clientid);

	எனில்(rc == 0){
		client_செய்தி_மொத்தம்++;
	}
	திரும்பு rc;
}


நிலையான முழு persist_base_செய்தி_chunk_restore(FILE *db_fptr, uint32_t length)
{
	கட்டமைப்பு P_base_செய்தி chunk;
	கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி = வெற்று;
	int64_t message_expiry_interval64;
	uint32_t message_expiry_interval;
	uint32_t *p_message_expiry_interval = வெற்று;
	முழு rc = 0;

	memset(&chunk, 0, அளவு(கட்டமைப்பு P_base_செய்தி));

	எனில்(db_version == 6 || db_version == 5){
		rc = persist_chunk_base_செய்தி_read_v56(db_fptr, &chunk, length);
	}இல்லையெனில்{
		rc = persist_chunk_base_செய்தி_read_v234(db_fptr, &chunk, db_version);
	}
	எனில்(rc){
		திரும்பு rc;
	}
	எனில்(chunk.F.topic_len == 0){
		rc = MOSQ_ERR_INVAL;
		செல் cleanup;
	}

	எனில்(chunk.F.source_port){
		க்கு(முழு i=0; i<db.config->listener_மொத்தம்; i++){
			எனில்(db.config->listeners[i].port == chunk.F.source_port){
				chunk.source.listener = &db.config->listeners[i];
				நிறுத்து;
			}
		}
	}

	எனில்(chunk.F.expiry_time > 0){
		message_expiry_interval64 = chunk.F.expiry_time - time(வெற்று);
		எனில்(message_expiry_interval64 < 0 || message_expiry_interval64 > UINT32_MAX){
			/* Expired message */
			rc = MOSQ_ERR_SUCCESS;
			செல் cleanup;
		}இல்லையெனில்{
			message_expiry_interval = (uint32_t)message_expiry_interval64;
		}
		p_message_expiry_interval = &message_expiry_interval;
	}

	base_செய்தி = mosquitto_calloc(1, அளவு(கட்டமைப்பு mosquitto_base_செய்தி));
	எனில்(base_செய்தி == வெற்று){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
		rc = MOSQ_ERR_NOMEM;
		செல் cleanup;
	}

	base_செய்தி->data.store_id = chunk.F.store_id;
	base_செய்தி->data.source_mid = chunk.F.source_mid;
	base_செய்தி->data.topic = chunk.topic;
	base_செய்தி->data.qos = chunk.F.qos;
	base_செய்தி->data.payloadlen = chunk.F.payloadlen;
	base_செய்தி->data.retain = chunk.F.retain;
	base_செய்தி->data.properties = chunk.properties;
	base_செய்தி->data.payload = chunk.payload;
	base_செய்தி->source_listener = chunk.source.listener;

	rc = db__message_store(&chunk.source, base_செய்தி, p_message_expiry_interval,
			mosq_mo_client);

	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);

	எனில்(rc == MOSQ_ERR_SUCCESS){
		base_செய்தி_மொத்தம்++;
		திரும்பு MOSQ_ERR_SUCCESS;
	}இல்லையெனில்{
		திரும்பு rc;
	}
cleanup:
	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);
	mosquitto_FREE(chunk.topic);
	mosquitto_FREE(chunk.payload);
	திரும்பு rc;
}


நிலையான முழு persist__retain_chunk_restore(FILE *db_fptr)
{
	கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி;
	கட்டமைப்பு P_retain chunk;
	முழு rc;
	எழுத்து **split_topics;
	எழுத்து *local_topic;

	memset(&chunk, 0, அளவு(கட்டமைப்பு P_retain));

	எனில்(db_version == 6 || db_version == 5){
		rc = persist__chunk_retain_read_v56(db_fptr, &chunk);
	}இல்லையெனில்{
		rc = persist__chunk_retain_read_v234(db_fptr, &chunk);
	}
	எனில்(rc){
		திரும்பு rc;
	}

	HASH_FIND(hh, db.செய்தி_store, &chunk.F.store_id, அளவு(chunk.F.store_id), base_செய்தி);
	எனில்(base_செய்தி){
		rc = sub__topic_tokenise(base_செய்தி->data.topic, &local_topic, &split_topics, வெற்று);
		எனில்(rc){
			திரும்பு rc;
		}
		retain__store(base_செய்தி->data.topic, base_செய்தி, split_topics, true);
		mosquitto_FREE(local_topic);
		mosquitto_FREE(split_topics);
		retained_மொத்தம்++;
	}இல்லையெனில்{
		/* Can't find the message - probably expired */
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு persist__sub_chunk_restore(FILE *db_fptr)
{
	கட்டமைப்பு P_sub chunk;
	முழு rc;
	கட்டமைப்பு mosquitto_subscription sub;

	memset(&chunk, 0, அளவு(கட்டமைப்பு P_sub));

	எனில்(db_version == 6 || db_version == 5){
		rc = persist__chunk_sub_read_v56(db_fptr, &chunk);
	}இல்லையெனில்{
		rc = persist__chunk_sub_read_v234(db_fptr, &chunk);
	}
	எனில்(rc){
		திரும்பு rc;
	}

	sub.clientid = chunk.clientid;
	sub.topic_filter = chunk.topic;
	sub.options = chunk.F.qos | chunk.F.options;
	sub.identifier = chunk.F.identifier;
	rc = persist__restore_sub(&sub);

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.topic);
	எனில்(rc == 0){
		subscription_மொத்தம்++;
	}

	திரும்பு rc;
}


முழு persist__chunk_header_read(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	எனில்(db_version == 6 || db_version == 5){
		திரும்பு persist__chunk_header_read_v56(db_fptr, chunk, length);
	}இல்லையெனில்{
		திரும்பு persist__chunk_header_read_v234(db_fptr, chunk, length);
	}
}


முழு persist__restore(வெறுமை)
{
	FILE *fptr;
	எழுத்து header[15];
	முழு rc = 0;
	uint32_t crc;
	uint32_t i32temp;
	uint32_t chunk, length;
	size_t rlen;
	எழுத்து *err;
	கட்டமைப்பு PF_cfg cfg_chunk;

	ப்ரதிஜ்ஞா(db.config);

	எனில்(!db.config->persistence || db.config->persistence_filepath == வெற்று){
		திரும்பு MOSQ_ERR_SUCCESS;
	}

	db.செய்தி_store = வெற்று;
	base_செய்தி_மொத்தம் = 0;
	retained_மொத்தம் = 0;
	client_மொத்தம் = 0;
	subscription_மொத்தம் = 0;
	client_செய்தி_மொத்தம் = 0;

	fptr = mosquitto_fopen(db.config->persistence_filepath, "rb", true);
	எனில்(fptr == வெற்று){
		திரும்பு MOSQ_ERR_SUCCESS;
	}
	rlen = fread(&header, 1, 15, fptr);
	எனில்(rlen == 0){
		fclose(fptr);
		log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Persistence file is empty.");
		திரும்பு 0;
	}இல்லையெனில் எனில்(rlen != 15){
		செல் error;
	}
	எனில்(!memcmp(header, magic, 15)){
		/* Restore DB as normal */
		read_e(fptr, &crc, அளவு(uint32_t));
		read_e(fptr, &i32temp, அளவு(uint32_t));
		db_version = ntohl(i32temp);
		/* IMPORTANT - this is where compatibility checks are made.
		 * Is your DB change still compatible with previous versions?
		 */
		எனில்(db_version != MOSQ_DB_VERSION){
			எனில்(db_version == 5){
				/* Addition of username and listener_port to client chunk in v6 */
			}இல்லையெனில் எனில்(db_version == 4){
			}இல்லையெனில் எனில்(db_version == 3){
				/* Addition of source_username and source_port to msg_store chunk in v4, v1.5.6 */
			}இல்லையெனில் எனில்(db_version == 2){
				/* Addition of disconnect_t to client chunk in v3. */
			}இல்லையெனில்{
				fclose(fptr);
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unsupported persistent database format version %d (need version %d).", db_version, MOSQ_DB_VERSION);
				திரும்பு MOSQ_ERR_INVAL;
			}
		}

		போது(persist__chunk_header_read(fptr, &chunk, &length) == MOSQ_ERR_SUCCESS){
			தேர்வு(chunk){
				நிலை DB_CHUNK_CFG:
					எனில்(db_version == 6 || db_version == 5){
						rc = persist__chunk_cfg_read_v56(fptr, &cfg_chunk);
						எனில்(rc){
							fclose(fptr);
							திரும்பு rc;
						}
					}இல்லையெனில்{
						rc = persist__chunk_cfg_read_v234(fptr, &cfg_chunk);
						எனில்(rc){
							fclose(fptr);
							திரும்பு rc;
						}
					}
					எனில்(cfg_chunk.dbid_size != அளவு(dbid_t)){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Incompatible database configuration (dbid size is %d bytes, expected %lu)",
								cfg_chunk.dbid_size, (unsigned long)அளவு(dbid_t));
						fclose(fptr);
						திரும்பு MOSQ_ERR_INVAL;
					}
					db.last_db_id = cfg_chunk.last_db_id;
					நிறுத்து;

				நிலை DB_CHUNK_BASE_செய்தி:
					rc = persist_base_செய்தி_chunk_restore(fptr, length);
					எனில்(rc){
						fclose(fptr);
						திரும்பு rc;
					}
					நிறுத்து;

				நிலை DB_CHUNK_CLIENT_செய்தி:
					rc = persist_client_செய்தி_chunk_restore(fptr, length);
					எனில்(rc){
						fclose(fptr);
						திரும்பு rc;
					}
					நிறுத்து;

				நிலை DB_CHUNK_RETAIN:
					rc = persist__retain_chunk_restore(fptr);
					எனில்(rc){
						fclose(fptr);
						திரும்பு rc;
					}
					நிறுத்து;

				நிலை DB_CHUNK_SUB:
					rc = persist__sub_chunk_restore(fptr);
					எனில்(rc){
						fclose(fptr);
						திரும்பு rc;
					}
					நிறுத்து;

				நிலை DB_CHUNK_CLIENT:
					rc = persist__client_chunk_restore(fptr);
					எனில்(rc){
						fclose(fptr);
						திரும்பு rc;
					}
					நிறுத்து;

				இயல்பு:
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Unsupported chunk \"%d\" in persistent database file. Ignoring.", chunk);
					எனில்(fseek(fptr, length, SEEK_CUR) < 0){
						fclose(fptr);
						திரும்பு MOSQ_ERR_INVAL;
					}
					நிறுத்து;
			}
		}
	}இல்லையெனில்{
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to restore persistent database. Unrecognised file format.");
		rc = 1;
	}

	fclose(fptr);

	log__printf(வெற்று, MOSQ_LOG_INFO, "Restored %ld base messages", base_செய்தி_மொத்தம்);
	log__printf(வெற்று, MOSQ_LOG_INFO, "Restored %ld retained messages", retained_மொத்தம்);
	log__printf(வெற்று, MOSQ_LOG_INFO, "Restored %ld clients", client_மொத்தம்);
	log__printf(வெற்று, MOSQ_LOG_INFO, "Restored %ld subscriptions", subscription_மொத்தம்);
	log__printf(வெற்று, MOSQ_LOG_INFO, "Restored %ld client messages", client_செய்தி_மொத்தம்);

	திரும்பு rc;
error:
	err = strerror(errno);
	log__printf(வெற்று, MOSQ_LOG_ERR, "Error: %s.", err);
	எனில்(fptr){
		fclose(fptr);
	}
	திரும்பு MOSQ_ERR_ERRNO;
}


நிலையான முழு persist__restore_sub(மாறிலி கட்டமைப்பு mosquitto_subscription *sub)
{
	கட்டமைப்பு mosquitto *context;

	எனில்(!sub->clientid){
		log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no client id, ignoring.");
		திரும்பு MOSQ_ERR_SUCCESS;
	}
	எனில்(!sub->topic_filter){
		log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no topic filter, ignoring.");
		திரும்பு MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(sub->clientid, 0);
	எனில்(!context){
		திரும்பு MOSQ_ERR_INVAL;
	}
	திரும்பு sub__add(context, sub);
}

#endif
