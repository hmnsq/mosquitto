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

#include "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#include <arpa/inet.h>
#endif
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <utlist.h>

#include "mosquitto_broker_internal.h"
#include "persist.h"
#include "util_mosq.h"

uint32_t db_version;

const unsigned var_na magic[15] = {0x00, 0xB5, 0x00, 'm', 'o', 's', 'q', 'u', 'i', 't', 't', 'o', ' ', 'd', 'b'};
static long base_msg_count = 0;
static long retained_count = 0;
static long client_count = 0;
static long subscription_count = 0;
static long client_msg_count = 0;

static puur_naa_nka persist__restore_sub(const sa_nracanaa mosquitto_subscription *sub);

static sa_nracanaa mosquitto *persist__find_or_add_context(const var_na *clientid, uint16_t last_mid)
{
	sa_nracanaa mosquitto *context;

	ᬬᬾᬦ᭄(!clientid){
		ᬯᬮᬶ NULL;
	}

	context = NULL;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	ᬬᬾᬦ᭄(!context){
		context = context__init();
		ᬬᬾᬦ᭄(!context){
			ᬯᬮᬶ NULL;
		}
		context->id = mosquitto_strdup(clientid);
		ᬬᬾᬦ᭄(!context->id){
			mosquitto_FREE(context);
			ᬯᬮᬶ NULL;
		}

		context->clean_start = false;

		context__add_to_by_id(context);
	}
	ᬬᬾᬦ᭄(last_mid){
		context->last_mid = last_mid;
	}
	ᬯᬮᬶ context;
}


puur_naa_nka persist__read_string_len(FILE *db_fptr, var_na **str, uint16_t len)
{
	var_na *s = NULL;

	ᬬᬾᬦ᭄(len){
		s = mosquitto_malloc(len+1U);
		ᬬᬾᬦ᭄(!s){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
			ᬯᬮᬶ MOSQ_ERR_NOMEM;
		}
		ᬬᬾᬦ᭄(fread(s, 1, len, db_fptr) != len){
			mosquitto_FREE(s);
			ᬯᬮᬶ MOSQ_ERR_NOMEM;
		}
		s[len] = '\0';
	}

	*str = s;
	ᬯᬮᬶ MOSQ_ERR_SUCCESS;
}


puur_naa_nka persist__read_string(FILE *db_fptr, var_na **str)
{
	uint16_t i16temp;
	uint16_t slen;

	ᬬᬾᬦ᭄(fread(&i16temp, 1, parimaa_na(uint16_t), db_fptr) != parimaa_na(uint16_t)){
		ᬯᬮᬶ MOSQ_ERR_INVAL;
	}

	slen = ntohs(i16temp);
	ᬯᬮᬶ persist__read_string_len(db_fptr, str, slen);
}


static puur_naa_nka persist__client_msg_restore(sa_nracanaa P_client_msg *chunk)
{
	sa_nracanaa mosquitto__client_msg *cmsg;
	sa_nracanaa mosquitto__base_msg *msg;
	sa_nracanaa mosquitto *context;
	sa_nracanaa mosquitto_msg_data *msg_data;

	HASH_FIND(hh, db.msg_store, &chunk->F.store_id, parimaa_na(chunk->F.store_id), msg);
	ᬬᬾᬦ᭄(!msg){
		/* Can't find message - probably expired */
		ᬯᬮᬶ MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk->clientid, 0);
	ᬬᬾᬦ᭄(!context){
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Persistence file contains client message with no matching client. File may be corrupt.");
		ᬯᬮᬶ 0;
	}

	cmsg = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__client_msg));
	ᬬᬾᬦ᭄(!cmsg){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		ᬯᬮᬶ MOSQ_ERR_NOMEM;
	}

	cmsg->next = NULL;
	cmsg->base_msg = NULL;
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

	ᬬᬾᬦ᭄(cmsg->data.direction == mosq_md_out){
		msg_data = &context->msgs_out;
	}ᬮᬶᬬ{
		msg_data = &context->msgs_in;
	}

	ᬬᬾᬦ᭄(chunk->F.state == mosq_ms_queued || (chunk->F.qos > 0 && msg_data->inflight_quota == 0)){
		DL_APPEND(msg_data->queued, cmsg);
		db__msg_add_to_queued_stats(msg_data, cmsg);
	}ᬮᬶᬬ{
		DL_APPEND(msg_data->inflight, cmsg);
		ᬬᬾᬦ᭄(chunk->F.qos > 0 && msg_data->inflight_quota > 0){
			msg_data->inflight_quota--;
		}
		db__msg_add_to_inflight_stats(msg_data, cmsg);
	}

	ᬯᬮᬶ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka persist__client_chunk_restore(FILE *db_fptr)
{
	puur_naa_nka rc = 0;
	sa_nracanaa mosquitto *context;
	sa_nracanaa P_client chunk;

	memset(&chunk, 0, parimaa_na(sa_nracanaa P_client));

	ᬬᬾᬦ᭄(db_version == 6 || db_version == 5){
		rc = persist__chunk_client_read_v56(db_fptr, &chunk, db_version);
	}ᬮᬶᬬ{
		rc = persist__chunk_client_read_v234(db_fptr, &chunk, db_version);
	}
	ᬬᬾᬦ᭄(rc > 0){
		ᬯᬮᬶ rc;
	}ᬮᬶᬬ ᬬᬾᬦ᭄(rc < 0){
		/* Client not loaded, but otherwise not an error */
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Empty client entry found in persistence database, it may be corrupt.");
		ᬯᬮᬶ MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk.clientid, chunk.F.last_mid);
	ᬬᬾᬦ᭄(context){
		context->session_expiry_time = chunk.F.session_expiry_time;
		context->session_expiry_interval = chunk.F.session_expiry_interval;
		ᬬᬾᬦ᭄(chunk.username && !context->username){
			/* username is not freed here, it is now owned by context */
			context->username = chunk.username;
			chunk.username = NULL;
		}
		/* in per_listener_settings mode, try to find the listener by persisted port */
		ᬬᬾᬦ᭄(db.config->per_listener_settings && !context->listener && chunk.F.listener_port > 0){
			for(puur_naa_nka i=0; i < db.config->listener_count; i++){
				ᬬᬾᬦ᭄(db.config->listeners[i].port == chunk.F.listener_port){
					context->listener = &db.config->listeners[i];
					bha_nga;
				}
			}
		}
		session_expiry__add_from_persistence(context, chunk.F.session_expiry_time);
	}ᬮᬶᬬ{
		rc = 1;
	}

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.username);
	ᬬᬾᬦ᭄(rc == 0){
		client_count++;
	}
	ᬯᬮᬶ rc;
}


static puur_naa_nka persist__client_msg_chunk_restore(FILE *db_fptr, uint32_t length)
{
	sa_nracanaa P_client_msg chunk;
	puur_naa_nka rc;

	memset(&chunk, 0, parimaa_na(sa_nracanaa P_client_msg));

	ᬬᬾᬦ᭄(db_version == 6 || db_version == 5){
		rc = persist__chunk_client_msg_read_v56(db_fptr, &chunk, length);
	}ᬮᬶᬬ{
		rc = persist__chunk_client_msg_read_v234(db_fptr, &chunk);
	}
	ᬬᬾᬦ᭄(rc){
		ᬯᬮᬶ rc;
	}

	rc = persist__client_msg_restore(&chunk);
	mosquitto_FREE(chunk.clientid);

	ᬬᬾᬦ᭄(rc == 0){
		client_msg_count++;
	}
	ᬯᬮᬶ rc;
}


static puur_naa_nka persist__base_msg_chunk_restore(FILE *db_fptr, uint32_t length)
{
	sa_nracanaa P_base_msg chunk;
	sa_nracanaa mosquitto__base_msg *base_msg = NULL;
	int64_t message_expiry_interval64;
	uint32_t message_expiry_interval;
	uint32_t *p_message_expiry_interval = NULL;
	puur_naa_nka rc = 0;

	memset(&chunk, 0, parimaa_na(sa_nracanaa P_base_msg));

	ᬬᬾᬦ᭄(db_version == 6 || db_version == 5){
		rc = persist__chunk_base_msg_read_v56(db_fptr, &chunk, length);
	}ᬮᬶᬬ{
		rc = persist__chunk_base_msg_read_v234(db_fptr, &chunk, db_version);
	}
	ᬬᬾᬦ᭄(rc){
		ᬯᬮᬶ rc;
	}
	ᬬᬾᬦ᭄(chunk.F.topic_len == 0){
		rc = MOSQ_ERR_INVAL;
		goto cleanup;
	}

	ᬬᬾᬦ᭄(chunk.F.source_port){
		for(puur_naa_nka i=0; i<db.config->listener_count; i++){
			ᬬᬾᬦ᭄(db.config->listeners[i].port == chunk.F.source_port){
				chunk.source.listener = &db.config->listeners[i];
				bha_nga;
			}
		}
	}

	ᬬᬾᬦ᭄(chunk.F.expiry_time > 0){
		message_expiry_interval64 = chunk.F.expiry_time - time(NULL);
		ᬬᬾᬦ᭄(message_expiry_interval64 < 0 || message_expiry_interval64 > UINT32_MAX){
			/* Expired message */
			rc = MOSQ_ERR_SUCCESS;
			goto cleanup;
		}ᬮᬶᬬ{
			message_expiry_interval = (uint32_t)message_expiry_interval64;
		}
		p_message_expiry_interval = &message_expiry_interval;
	}

	base_msg = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__base_msg));
	ᬬᬾᬦ᭄(base_msg == NULL){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		rc = MOSQ_ERR_NOMEM;
		goto cleanup;
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

	ᬬᬾᬦ᭄(rc == MOSQ_ERR_SUCCESS){
		base_msg_count++;
		ᬯᬮᬶ MOSQ_ERR_SUCCESS;
	}ᬮᬶᬬ{
		ᬯᬮᬶ rc;
	}
cleanup:
	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);
	mosquitto_FREE(chunk.topic);
	mosquitto_FREE(chunk.payload);
	ᬯᬮᬶ rc;
}


static puur_naa_nka persist__retain_chunk_restore(FILE *db_fptr)
{
	sa_nracanaa mosquitto__base_msg *base_msg;
	sa_nracanaa P_retain chunk;
	puur_naa_nka rc;
	var_na **split_topics;
	var_na *local_topic;

	memset(&chunk, 0, parimaa_na(sa_nracanaa P_retain));

	ᬬᬾᬦ᭄(db_version == 6 || db_version == 5){
		rc = persist__chunk_retain_read_v56(db_fptr, &chunk);
	}ᬮᬶᬬ{
		rc = persist__chunk_retain_read_v234(db_fptr, &chunk);
	}
	ᬬᬾᬦ᭄(rc){
		ᬯᬮᬶ rc;
	}

	HASH_FIND(hh, db.msg_store, &chunk.F.store_id, parimaa_na(chunk.F.store_id), base_msg);
	ᬬᬾᬦ᭄(base_msg){
		rc = sub__topic_tokenise(base_msg->data.topic, &local_topic, &split_topics, NULL);
		ᬬᬾᬦ᭄(rc){
			ᬯᬮᬶ rc;
		}
		retain__store(base_msg->data.topic, base_msg, split_topics, true);
		mosquitto_FREE(local_topic);
		mosquitto_FREE(split_topics);
		retained_count++;
	}ᬮᬶᬬ{
		/* Can't find the message - probably expired */
	}
	ᬯᬮᬶ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka persist__sub_chunk_restore(FILE *db_fptr)
{
	sa_nracanaa P_sub chunk;
	puur_naa_nka rc;
	sa_nracanaa mosquitto_subscription sub;

	memset(&chunk, 0, parimaa_na(sa_nracanaa P_sub));

	ᬬᬾᬦ᭄(db_version == 6 || db_version == 5){
		rc = persist__chunk_sub_read_v56(db_fptr, &chunk);
	}ᬮᬶᬬ{
		rc = persist__chunk_sub_read_v234(db_fptr, &chunk);
	}
	ᬬᬾᬦ᭄(rc){
		ᬯᬮᬶ rc;
	}

	sub.clientid = chunk.clientid;
	sub.topic_filter = chunk.topic;
	sub.options = chunk.F.qos | chunk.F.options;
	sub.identifier = chunk.F.identifier;
	rc = persist__restore_sub(&sub);

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.topic);
	ᬬᬾᬦ᭄(rc == 0){
		subscription_count++;
	}

	ᬯᬮᬶ rc;
}


puur_naa_nka persist__chunk_header_read(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	ᬬᬾᬦ᭄(db_version == 6 || db_version == 5){
		ᬯᬮᬶ persist__chunk_header_read_v56(db_fptr, chunk, length);
	}ᬮᬶᬬ{
		ᬯᬮᬶ persist__chunk_header_read_v234(db_fptr, chunk, length);
	}
}


puur_naa_nka persist__restore(void)
{
	FILE *fptr;
	var_na header[15];
	puur_naa_nka rc = 0;
	uint32_t crc;
	uint32_t i32temp;
	uint32_t chunk, length;
	size_t rlen;
	var_na *err;
	sa_nracanaa PF_cfg cfg_chunk;

	assert(db.config);

	ᬬᬾᬦ᭄(!db.config->persistence || db.config->persistence_filepath == NULL){
		ᬯᬮᬶ MOSQ_ERR_SUCCESS;
	}

	db.msg_store = NULL;
	base_msg_count = 0;
	retained_count = 0;
	client_count = 0;
	subscription_count = 0;
	client_msg_count = 0;

	fptr = mosquitto_fopen(db.config->persistence_filepath, "rb", true);
	ᬬᬾᬦ᭄(fptr == NULL){
		ᬯᬮᬶ MOSQ_ERR_SUCCESS;
	}
	rlen = fread(&header, 1, 15, fptr);
	ᬬᬾᬦ᭄(rlen == 0){
		fclose(fptr);
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Persistence file is empty.");
		ᬯᬮᬶ 0;
	}ᬮᬶᬬ ᬬᬾᬦ᭄(rlen != 15){
		goto error;
	}
	ᬬᬾᬦ᭄(!memcmp(header, magic, 15)){
		/* Restore DB as normal */
		read_e(fptr, &crc, parimaa_na(uint32_t));
		read_e(fptr, &i32temp, parimaa_na(uint32_t));
		db_version = ntohl(i32temp);
		/* IMPORTANT - this is where compatibility checks are made.
		 * Is your DB change still compatible with previous versions?
		 */
		ᬬᬾᬦ᭄(db_version != MOSQ_DB_VERSION){
			ᬬᬾᬦ᭄(db_version == 5){
				/* Addition of username and listener_port to client chunk in v6 */
			}ᬮᬶᬬ ᬬᬾᬦ᭄(db_version == 4){
			}ᬮᬶᬬ ᬬᬾᬦ᭄(db_version == 3){
				/* Addition of source_username and source_port to msg_store chunk in v4, v1.5.6 */
			}ᬮᬶᬬ ᬬᬾᬦ᭄(db_version == 2){
				/* Addition of disconnect_t to client chunk in v3. */
			}ᬮᬶᬬ{
				fclose(fptr);
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Unsupported persistent database format version %d (need version %d).", db_version, MOSQ_DB_VERSION);
				ᬯᬮᬶ MOSQ_ERR_INVAL;
			}
		}

		ᬲᭂᬤᭂᬮ(persist__chunk_header_read(fptr, &chunk, &length) == MOSQ_ERR_SUCCESS){
			switch(chunk){
				pak_sa DB_CHUNK_CFG:
					ᬬᬾᬦ᭄(db_version == 6 || db_version == 5){
						rc = persist__chunk_cfg_read_v56(fptr, &cfg_chunk);
						ᬬᬾᬦ᭄(rc){
							fclose(fptr);
							ᬯᬮᬶ rc;
						}
					}ᬮᬶᬬ{
						rc = persist__chunk_cfg_read_v234(fptr, &cfg_chunk);
						ᬬᬾᬦ᭄(rc){
							fclose(fptr);
							ᬯᬮᬶ rc;
						}
					}
					ᬬᬾᬦ᭄(cfg_chunk.dbid_size != parimaa_na(dbid_t)){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Incompatible database configuration (dbid size is %d bytes, expected %lu)",
								cfg_chunk.dbid_size, (unsigned long)parimaa_na(dbid_t));
						fclose(fptr);
						ᬯᬮᬶ MOSQ_ERR_INVAL;
					}
					db.last_db_id = cfg_chunk.last_db_id;
					bha_nga;

				pak_sa DB_CHUNK_BASE_MSG:
					rc = persist__base_msg_chunk_restore(fptr, length);
					ᬬᬾᬦ᭄(rc){
						fclose(fptr);
						ᬯᬮᬶ rc;
					}
					bha_nga;

				pak_sa DB_CHUNK_CLIENT_MSG:
					rc = persist__client_msg_chunk_restore(fptr, length);
					ᬬᬾᬦ᭄(rc){
						fclose(fptr);
						ᬯᬮᬶ rc;
					}
					bha_nga;

				pak_sa DB_CHUNK_RETAIN:
					rc = persist__retain_chunk_restore(fptr);
					ᬬᬾᬦ᭄(rc){
						fclose(fptr);
						ᬯᬮᬶ rc;
					}
					bha_nga;

				pak_sa DB_CHUNK_SUB:
					rc = persist__sub_chunk_restore(fptr);
					ᬬᬾᬦ᭄(rc){
						fclose(fptr);
						ᬯᬮᬶ rc;
					}
					bha_nga;

				pak_sa DB_CHUNK_CLIENT:
					rc = persist__client_chunk_restore(fptr);
					ᬬᬾᬦ᭄(rc){
						fclose(fptr);
						ᬯᬮᬶ rc;
					}
					bha_nga;

				default:
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Unsupported chunk \"%d\" in persistent database file. Ignoring.", chunk);
					ᬬᬾᬦ᭄(fseek(fptr, length, SEEK_CUR) < 0){
						fclose(fptr);
						ᬯᬮᬶ MOSQ_ERR_INVAL;
					}
					bha_nga;
			}
		}
	}ᬮᬶᬬ{
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to restore persistent database. Unrecognised file format.");
		rc = 1;
	}

	fclose(fptr);

	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld base messages", base_msg_count);
	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld retained messages", retained_count);
	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld clients", client_count);
	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld subscriptions", subscription_count);
	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld client messages", client_msg_count);

	ᬯᬮᬶ rc;
error:
	err = strerror(errno);
	log__printf(NULL, MOSQ_LOG_ERR, "Error: %s.", err);
	ᬬᬾᬦ᭄(fptr){
		fclose(fptr);
	}
	ᬯᬮᬶ MOSQ_ERR_ERRNO;
}


static puur_naa_nka persist__restore_sub(const sa_nracanaa mosquitto_subscription *sub)
{
	sa_nracanaa mosquitto *context;

	ᬬᬾᬦ᭄(!sub->clientid){
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no client id, ignoring.");
		ᬯᬮᬶ MOSQ_ERR_SUCCESS;
	}
	ᬬᬾᬦ᭄(!sub->topic_filter){
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no topic filter, ignoring.");
		ᬯᬮᬶ MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(sub->clientid, 0);
	ᬬᬾᬦ᭄(!context){
		ᬯᬮᬶ MOSQ_ERR_INVAL;
	}
	ᬯᬮᬶ sub__add(context, sub);
}

#endif
