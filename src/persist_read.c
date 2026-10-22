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

#അന്തര്ഭാവ "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#അന്തര്ഭാവ <arpa/inet.h>
#endif
#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <errno.h>
#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <sys/stat.h>
#അന്തര്ഭാവ <time.h>
#അന്തര്ഭാവ <utlist.h>

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "persist.h"
#അന്തര്ഭാവ "util_mosq.h"

uint32_t db_version;

സ്ഥിരം unsigned അക്ഷരം magic[15] = {0x00, 0xB5, 0x00, 'm', 'o', 's', 'q', 'u', 'i', 't', 't', 'o', ' ', 'd', 'b'};
സ്ഥിതം long base_സന്ദേശം_എണ്ണം = 0;
സ്ഥിതം long retained_എണ്ണം = 0;
സ്ഥിതം long client_എണ്ണം = 0;
സ്ഥിതം long subscription_എണ്ണം = 0;
സ്ഥിതം long client_സന്ദേശം_എണ്ണം = 0;

സ്ഥിതം പൂർണ്ണസംഖ്യ persist__restore_sub(സ്ഥിരം ഘടന mosquitto_subscription *sub);

സ്ഥിതം ഘടന mosquitto *persist__find_or_add_context(സ്ഥിരം അക്ഷരം *clientid, uint16_t last_mid)
{
	ഘടന mosquitto *context;

	എങ്കിൽ(!clientid){
		തിരികെ ശൂന്യ;
	}

	context = ശൂന്യ;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	എങ്കിൽ(!context){
		context = context__init();
		എങ്കിൽ(!context){
			തിരികെ ശൂന്യ;
		}
		context->id = mosquitto_strdup(clientid);
		എങ്കിൽ(!context->id){
			mosquitto_FREE(context);
			തിരികെ ശൂന്യ;
		}

		context->clean_start = false;

		context__add_to_by_id(context);
	}
	എങ്കിൽ(last_mid){
		context->last_mid = last_mid;
	}
	തിരികെ context;
}


പൂർണ്ണസംഖ്യ persist__read_string_len(FILE *db_fptr, അക്ഷരം **str, uint16_t len)
{
	അക്ഷരം *s = ശൂന്യ;

	എങ്കിൽ(len){
		s = mosquitto_malloc(len+1U);
		എങ്കിൽ(!s){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Out of memory.");
			തിരികെ MOSQ_ERR_NOMEM;
		}
		എങ്കിൽ(fread(s, 1, len, db_fptr) != len){
			mosquitto_FREE(s);
			തിരികെ MOSQ_ERR_NOMEM;
		}
		s[len] = '\0';
	}

	*str = s;
	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ persist__read_string(FILE *db_fptr, അക്ഷരം **str)
{
	uint16_t i16temp;
	uint16_t slen;

	എങ്കിൽ(fread(&i16temp, 1, വലുപ്പം(uint16_t), db_fptr) != വലുപ്പം(uint16_t)){
		തിരികെ MOSQ_ERR_INVAL;
	}

	slen = ntohs(i16temp);
	തിരികെ persist__read_string_len(db_fptr, str, slen);
}


സ്ഥിതം പൂർണ്ണസംഖ്യ persist_client_സന്ദേശം_restore(ഘടന P_client_സന്ദേശം *chunk)
{
	ഘടന mosquitto_client_സന്ദേശം *cmsg;
	ഘടന mosquitto_base_സന്ദേശം *സന്ദേശം;
	ഘടന mosquitto *context;
	ഘടന mosquitto_സന്ദേശം_data *സന്ദേശം_data;

	HASH_FIND(hh, db.സന്ദേശം_store, &chunk->F.store_id, വലുപ്പം(chunk->F.store_id), സന്ദേശം);
	എങ്കിൽ(!സന്ദേശം){
		/* Can't find message - probably expired */
		തിരികെ MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk->clientid, 0);
	എങ്കിൽ(!context){
		log__printf(ശൂന്യ, MOSQ_LOG_WARNING, "Warning: Persistence file contains client message with no matching client. File may be corrupt.");
		തിരികെ 0;
	}

	cmsg = mosquitto_calloc(1, വലുപ്പം(ഘടന mosquitto_client_സന്ദേശം));
	എങ്കിൽ(!cmsg){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Out of memory.");
		തിരികെ MOSQ_ERR_NOMEM;
	}

	cmsg->next = ശൂന്യ;
	cmsg->base_സന്ദേശം = ശൂന്യ;
	cmsg->data.cmsg_id = ++context->last_cmsg_id;
	cmsg->data.mid = chunk->F.mid;
	cmsg->data.qos = chunk->F.qos;
	cmsg->data.retain = (chunk->F.retain_dup&0xF0)>>4;
	cmsg->data.direction = chunk->F.direction;
	cmsg->data.state = chunk->F.state;
	cmsg->data.dup = chunk->F.retain_dup&0x0F;
	cmsg->data.subscription_identifier = chunk->subscription_identifier;

	cmsg->base_സന്ദേശം = സന്ദേശം;
	db_സന്ദേശം_store_ref_inc(cmsg->base_സന്ദേശം);

	എങ്കിൽ(cmsg->data.direction == mosq_md_out){
		സന്ദേശം_data = &context->msgs_out;
	}അല്ലെങ്കിൽ{
		സന്ദേശം_data = &context->msgs_in;
	}

	എങ്കിൽ(chunk->F.state == mosq_ms_queued || (chunk->F.qos > 0 && സന്ദേശം_data->inflight_quota == 0)){
		DL_APPEND(സന്ദേശം_data->queued, cmsg);
		db_സന്ദേശം_add_to_queued_stats(സന്ദേശം_data, cmsg);
	}അല്ലെങ്കിൽ{
		DL_APPEND(സന്ദേശം_data->inflight, cmsg);
		എങ്കിൽ(chunk->F.qos > 0 && സന്ദേശം_data->inflight_quota > 0){
			സന്ദേശം_data->inflight_quota--;
		}
		db_സന്ദേശം_add_to_inflight_stats(സന്ദേശം_data, cmsg);
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ persist__client_chunk_restore(FILE *db_fptr)
{
	പൂർണ്ണസംഖ്യ rc = 0;
	ഘടന mosquitto *context;
	ഘടന P_client chunk;

	memset(&chunk, 0, വലുപ്പം(ഘടന P_client));

	എങ്കിൽ(db_version == 6 || db_version == 5){
		rc = persist__chunk_client_read_v56(db_fptr, &chunk, db_version);
	}അല്ലെങ്കിൽ{
		rc = persist__chunk_client_read_v234(db_fptr, &chunk, db_version);
	}
	എങ്കിൽ(rc > 0){
		തിരികെ rc;
	}അല്ലെങ്കിൽ എങ്കിൽ(rc < 0){
		/* Client not loaded, but otherwise not an error */
		log__printf(ശൂന്യ, MOSQ_LOG_WARNING, "Warning: Empty client entry found in persistence database, it may be corrupt.");
		തിരികെ MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk.clientid, chunk.F.last_mid);
	എങ്കിൽ(context){
		context->session_expiry_time = chunk.F.session_expiry_time;
		context->session_expiry_interval = chunk.F.session_expiry_interval;
		എങ്കിൽ(chunk.username && !context->username){
			/* username is not freed here, it is now owned by context */
			context->username = chunk.username;
			chunk.username = ശൂന്യ;
		}
		/* in per_listener_settings mode, try to find the listener by persisted port */
		എങ്കിൽ(db.config->per_listener_settings && !context->listener && chunk.F.listener_port > 0){
			വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i < db.config->listener_എണ്ണം; i++){
				എങ്കിൽ(db.config->listeners[i].port == chunk.F.listener_port){
					context->listener = &db.config->listeners[i];
					നിർത്തുക;
				}
			}
		}
		session_expiry__add_from_persistence(context, chunk.F.session_expiry_time);
	}അല്ലെങ്കിൽ{
		rc = 1;
	}

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.username);
	എങ്കിൽ(rc == 0){
		client_എണ്ണം++;
	}
	തിരികെ rc;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ persist_client_സന്ദേശം_chunk_restore(FILE *db_fptr, uint32_t length)
{
	ഘടന P_client_സന്ദേശം chunk;
	പൂർണ്ണസംഖ്യ rc;

	memset(&chunk, 0, വലുപ്പം(ഘടന P_client_സന്ദേശം));

	എങ്കിൽ(db_version == 6 || db_version == 5){
		rc = persist_chunk_client_സന്ദേശം_read_v56(db_fptr, &chunk, length);
	}അല്ലെങ്കിൽ{
		rc = persist_chunk_client_സന്ദേശം_read_v234(db_fptr, &chunk);
	}
	എങ്കിൽ(rc){
		തിരികെ rc;
	}

	rc = persist_client_സന്ദേശം_restore(&chunk);
	mosquitto_FREE(chunk.clientid);

	എങ്കിൽ(rc == 0){
		client_സന്ദേശം_എണ്ണം++;
	}
	തിരികെ rc;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ persist_base_സന്ദേശം_chunk_restore(FILE *db_fptr, uint32_t length)
{
	ഘടന P_base_സന്ദേശം chunk;
	ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം = ശൂന്യ;
	int64_t message_expiry_interval64;
	uint32_t message_expiry_interval;
	uint32_t *p_message_expiry_interval = ശൂന്യ;
	പൂർണ്ണസംഖ്യ rc = 0;

	memset(&chunk, 0, വലുപ്പം(ഘടന P_base_സന്ദേശം));

	എങ്കിൽ(db_version == 6 || db_version == 5){
		rc = persist_chunk_base_സന്ദേശം_read_v56(db_fptr, &chunk, length);
	}അല്ലെങ്കിൽ{
		rc = persist_chunk_base_സന്ദേശം_read_v234(db_fptr, &chunk, db_version);
	}
	എങ്കിൽ(rc){
		തിരികെ rc;
	}
	എങ്കിൽ(chunk.F.topic_len == 0){
		rc = MOSQ_ERR_INVAL;
		പോകുക cleanup;
	}

	എങ്കിൽ(chunk.F.source_port){
		വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<db.config->listener_എണ്ണം; i++){
			എങ്കിൽ(db.config->listeners[i].port == chunk.F.source_port){
				chunk.source.listener = &db.config->listeners[i];
				നിർത്തുക;
			}
		}
	}

	എങ്കിൽ(chunk.F.expiry_time > 0){
		message_expiry_interval64 = chunk.F.expiry_time - time(ശൂന്യ);
		എങ്കിൽ(message_expiry_interval64 < 0 || message_expiry_interval64 > UINT32_MAX){
			/* Expired message */
			rc = MOSQ_ERR_SUCCESS;
			പോകുക cleanup;
		}അല്ലെങ്കിൽ{
			message_expiry_interval = (uint32_t)message_expiry_interval64;
		}
		p_message_expiry_interval = &message_expiry_interval;
	}

	base_സന്ദേശം = mosquitto_calloc(1, വലുപ്പം(ഘടന mosquitto_base_സന്ദേശം));
	എങ്കിൽ(base_സന്ദേശം == ശൂന്യ){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Out of memory.");
		rc = MOSQ_ERR_NOMEM;
		പോകുക cleanup;
	}

	base_സന്ദേശം->data.store_id = chunk.F.store_id;
	base_സന്ദേശം->data.source_mid = chunk.F.source_mid;
	base_സന്ദേശം->data.topic = chunk.topic;
	base_സന്ദേശം->data.qos = chunk.F.qos;
	base_സന്ദേശം->data.payloadlen = chunk.F.payloadlen;
	base_സന്ദേശം->data.retain = chunk.F.retain;
	base_സന്ദേശം->data.properties = chunk.properties;
	base_സന്ദേശം->data.payload = chunk.payload;
	base_സന്ദേശം->source_listener = chunk.source.listener;

	rc = db__message_store(&chunk.source, base_സന്ദേശം, p_message_expiry_interval,
			mosq_mo_client);

	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);

	എങ്കിൽ(rc == MOSQ_ERR_SUCCESS){
		base_സന്ദേശം_എണ്ണം++;
		തിരികെ MOSQ_ERR_SUCCESS;
	}അല്ലെങ്കിൽ{
		തിരികെ rc;
	}
cleanup:
	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);
	mosquitto_FREE(chunk.topic);
	mosquitto_FREE(chunk.payload);
	തിരികെ rc;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ persist__retain_chunk_restore(FILE *db_fptr)
{
	ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം;
	ഘടന P_retain chunk;
	പൂർണ്ണസംഖ്യ rc;
	അക്ഷരം **split_topics;
	അക്ഷരം *local_topic;

	memset(&chunk, 0, വലുപ്പം(ഘടന P_retain));

	എങ്കിൽ(db_version == 6 || db_version == 5){
		rc = persist__chunk_retain_read_v56(db_fptr, &chunk);
	}അല്ലെങ്കിൽ{
		rc = persist__chunk_retain_read_v234(db_fptr, &chunk);
	}
	എങ്കിൽ(rc){
		തിരികെ rc;
	}

	HASH_FIND(hh, db.സന്ദേശം_store, &chunk.F.store_id, വലുപ്പം(chunk.F.store_id), base_സന്ദേശം);
	എങ്കിൽ(base_സന്ദേശം){
		rc = sub__topic_tokenise(base_സന്ദേശം->data.topic, &local_topic, &split_topics, ശൂന്യ);
		എങ്കിൽ(rc){
			തിരികെ rc;
		}
		retain__store(base_സന്ദേശം->data.topic, base_സന്ദേശം, split_topics, true);
		mosquitto_FREE(local_topic);
		mosquitto_FREE(split_topics);
		retained_എണ്ണം++;
	}അല്ലെങ്കിൽ{
		/* Can't find the message - probably expired */
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ persist__sub_chunk_restore(FILE *db_fptr)
{
	ഘടന P_sub chunk;
	പൂർണ്ണസംഖ്യ rc;
	ഘടന mosquitto_subscription sub;

	memset(&chunk, 0, വലുപ്പം(ഘടന P_sub));

	എങ്കിൽ(db_version == 6 || db_version == 5){
		rc = persist__chunk_sub_read_v56(db_fptr, &chunk);
	}അല്ലെങ്കിൽ{
		rc = persist__chunk_sub_read_v234(db_fptr, &chunk);
	}
	എങ്കിൽ(rc){
		തിരികെ rc;
	}

	sub.clientid = chunk.clientid;
	sub.topic_filter = chunk.topic;
	sub.options = chunk.F.qos | chunk.F.options;
	sub.identifier = chunk.F.identifier;
	rc = persist__restore_sub(&sub);

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.topic);
	എങ്കിൽ(rc == 0){
		subscription_എണ്ണം++;
	}

	തിരികെ rc;
}


പൂർണ്ണസംഖ്യ persist__chunk_header_read(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	എങ്കിൽ(db_version == 6 || db_version == 5){
		തിരികെ persist__chunk_header_read_v56(db_fptr, chunk, length);
	}അല്ലെങ്കിൽ{
		തിരികെ persist__chunk_header_read_v234(db_fptr, chunk, length);
	}
}


പൂർണ്ണസംഖ്യ persist__restore(ശൂന്യം)
{
	FILE *fptr;
	അക്ഷരം header[15];
	പൂർണ്ണസംഖ്യ rc = 0;
	uint32_t crc;
	uint32_t i32temp;
	uint32_t chunk, length;
	size_t rlen;
	അക്ഷരം *err;
	ഘടന PF_cfg cfg_chunk;

	പ്രതിജ്ഞാ(db.config);

	എങ്കിൽ(!db.config->persistence || db.config->persistence_filepath == ശൂന്യ){
		തിരികെ MOSQ_ERR_SUCCESS;
	}

	db.സന്ദേശം_store = ശൂന്യ;
	base_സന്ദേശം_എണ്ണം = 0;
	retained_എണ്ണം = 0;
	client_എണ്ണം = 0;
	subscription_എണ്ണം = 0;
	client_സന്ദേശം_എണ്ണം = 0;

	fptr = mosquitto_fopen(db.config->persistence_filepath, "rb", true);
	എങ്കിൽ(fptr == ശൂന്യ){
		തിരികെ MOSQ_ERR_SUCCESS;
	}
	rlen = fread(&header, 1, 15, fptr);
	എങ്കിൽ(rlen == 0){
		fclose(fptr);
		log__printf(ശൂന്യ, MOSQ_LOG_WARNING, "Warning: Persistence file is empty.");
		തിരികെ 0;
	}അല്ലെങ്കിൽ എങ്കിൽ(rlen != 15){
		പോകുക error;
	}
	എങ്കിൽ(!memcmp(header, magic, 15)){
		/* Restore DB as normal */
		read_e(fptr, &crc, വലുപ്പം(uint32_t));
		read_e(fptr, &i32temp, വലുപ്പം(uint32_t));
		db_version = ntohl(i32temp);
		/* IMPORTANT - this is where compatibility checks are made.
		 * Is your DB change still compatible with previous versions?
		 */
		എങ്കിൽ(db_version != MOSQ_DB_VERSION){
			എങ്കിൽ(db_version == 5){
				/* Addition of username and listener_port to client chunk in v6 */
			}അല്ലെങ്കിൽ എങ്കിൽ(db_version == 4){
			}അല്ലെങ്കിൽ എങ്കിൽ(db_version == 3){
				/* Addition of source_username and source_port to msg_store chunk in v4, v1.5.6 */
			}അല്ലെങ്കിൽ എങ്കിൽ(db_version == 2){
				/* Addition of disconnect_t to client chunk in v3. */
			}അല്ലെങ്കിൽ{
				fclose(fptr);
				log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unsupported persistent database format version %d (need version %d).", db_version, MOSQ_DB_VERSION);
				തിരികെ MOSQ_ERR_INVAL;
			}
		}

		ആകുമ്പോൾ(persist__chunk_header_read(fptr, &chunk, &length) == MOSQ_ERR_SUCCESS){
			തിരഞ്ഞെടുപ്പ്(chunk){
				അവസ്ഥ DB_CHUNK_CFG:
					എങ്കിൽ(db_version == 6 || db_version == 5){
						rc = persist__chunk_cfg_read_v56(fptr, &cfg_chunk);
						എങ്കിൽ(rc){
							fclose(fptr);
							തിരികെ rc;
						}
					}അല്ലെങ്കിൽ{
						rc = persist__chunk_cfg_read_v234(fptr, &cfg_chunk);
						എങ്കിൽ(rc){
							fclose(fptr);
							തിരികെ rc;
						}
					}
					എങ്കിൽ(cfg_chunk.dbid_size != വലുപ്പം(dbid_t)){
						log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Incompatible database configuration (dbid size is %d bytes, expected %lu)",
								cfg_chunk.dbid_size, (unsigned long)വലുപ്പം(dbid_t));
						fclose(fptr);
						തിരികെ MOSQ_ERR_INVAL;
					}
					db.last_db_id = cfg_chunk.last_db_id;
					നിർത്തുക;

				അവസ്ഥ DB_CHUNK_BASE_സന്ദേശം:
					rc = persist_base_സന്ദേശം_chunk_restore(fptr, length);
					എങ്കിൽ(rc){
						fclose(fptr);
						തിരികെ rc;
					}
					നിർത്തുക;

				അവസ്ഥ DB_CHUNK_CLIENT_സന്ദേശം:
					rc = persist_client_സന്ദേശം_chunk_restore(fptr, length);
					എങ്കിൽ(rc){
						fclose(fptr);
						തിരികെ rc;
					}
					നിർത്തുക;

				അവസ്ഥ DB_CHUNK_RETAIN:
					rc = persist__retain_chunk_restore(fptr);
					എങ്കിൽ(rc){
						fclose(fptr);
						തിരികെ rc;
					}
					നിർത്തുക;

				അവസ്ഥ DB_CHUNK_SUB:
					rc = persist__sub_chunk_restore(fptr);
					എങ്കിൽ(rc){
						fclose(fptr);
						തിരികെ rc;
					}
					നിർത്തുക;

				അവസ്ഥ DB_CHUNK_CLIENT:
					rc = persist__client_chunk_restore(fptr);
					എങ്കിൽ(rc){
						fclose(fptr);
						തിരികെ rc;
					}
					നിർത്തുക;

				സ്ഥിരസ്ഥിതി:
					log__printf(ശൂന്യ, MOSQ_LOG_WARNING, "Warning: Unsupported chunk \"%d\" in persistent database file. Ignoring.", chunk);
					എങ്കിൽ(fseek(fptr, length, SEEK_CUR) < 0){
						fclose(fptr);
						തിരികെ MOSQ_ERR_INVAL;
					}
					നിർത്തുക;
			}
		}
	}അല്ലെങ്കിൽ{
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to restore persistent database. Unrecognised file format.");
		rc = 1;
	}

	fclose(fptr);

	log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Restored %ld base messages", base_സന്ദേശം_എണ്ണം);
	log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Restored %ld retained messages", retained_എണ്ണം);
	log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Restored %ld clients", client_എണ്ണം);
	log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Restored %ld subscriptions", subscription_എണ്ണം);
	log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Restored %ld client messages", client_സന്ദേശം_എണ്ണം);

	തിരികെ rc;
error:
	err = strerror(errno);
	log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: %s.", err);
	എങ്കിൽ(fptr){
		fclose(fptr);
	}
	തിരികെ MOSQ_ERR_ERRNO;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ persist__restore_sub(സ്ഥിരം ഘടന mosquitto_subscription *sub)
{
	ഘടന mosquitto *context;

	എങ്കിൽ(!sub->clientid){
		log__printf(ശൂന്യ, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no client id, ignoring.");
		തിരികെ MOSQ_ERR_SUCCESS;
	}
	എങ്കിൽ(!sub->topic_filter){
		log__printf(ശൂന്യ, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no topic filter, ignoring.");
		തിരികെ MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(sub->clientid, 0);
	എങ്കിൽ(!context){
		തിരികെ MOSQ_ERR_INVAL;
	}
	തിരികെ sub__add(context, sub);
}

#endif
