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

const unsigned char magic[15] = {0x00, 0xB5, 0x00, 'm', 'o', 's', 'q', 'u', 'i', 't', 't', 'o', ' ', 'd', 'b'};
static long base_msg_count = 0;
static long retained_count = 0;
static long client_count = 0;
static long subscription_count = 0;
static long client_msg_count = 0;

static 整数 persist__restore_sub(const struct mosquitto_subscription *sub);

static struct mosquitto *persist__find_or_add_context(const char *clientid, uint16_t last_mid)
{
	struct mosquitto *context;

	もし(!clientid){
		戻る NULL;
	}

	context = NULL;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	もし(!context){
		context = context__init();
		もし(!context){
			戻る NULL;
		}
		context->id = mosquitto_strdup(clientid);
		もし(!context->id){
			mosquitto_FREE(context);
			戻る NULL;
		}

		context->clean_start = false;

		context__add_to_by_id(context);
	}
	もし(last_mid){
		context->last_mid = last_mid;
	}
	戻る context;
}


整数 persist__read_string_len(FILE *db_fptr, char **str, uint16_t len)
{
	char *s = NULL;

	もし(len){
		s = mosquitto_malloc(len+1U);
		もし(!s){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
			戻る MOSQ_ERR_NOMEM;
		}
		もし(fread(s, 1, len, db_fptr) != len){
			mosquitto_FREE(s);
			戻る MOSQ_ERR_NOMEM;
		}
		s[len] = '\0';
	}

	*str = s;
	戻る MOSQ_ERR_SUCCESS;
}


整数 persist__read_string(FILE *db_fptr, char **str)
{
	uint16_t i16temp;
	uint16_t slen;

	もし(fread(&i16temp, 1, sizeof(uint16_t), db_fptr) != sizeof(uint16_t)){
		戻る MOSQ_ERR_INVAL;
	}

	slen = ntohs(i16temp);
	戻る persist__read_string_len(db_fptr, str, slen);
}


static 整数 persist__client_msg_restore(struct P_client_msg *chunk)
{
	struct mosquitto__client_msg *cmsg;
	struct mosquitto__base_msg *msg;
	struct mosquitto *context;
	struct mosquitto_msg_data *msg_data;

	HASH_FIND(hh, db.msg_store, &chunk->F.store_id, sizeof(chunk->F.store_id), msg);
	もし(!msg){
		/* Can't find message - probably expired */
		戻る MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk->clientid, 0);
	もし(!context){
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Persistence file contains client message with no matching client. File may be corrupt.");
		戻る 0;
	}

	cmsg = mosquitto_calloc(1, sizeof(struct mosquitto__client_msg));
	もし(!cmsg){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		戻る MOSQ_ERR_NOMEM;
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

	もし(cmsg->data.direction == mosq_md_out){
		msg_data = &context->msgs_out;
	}否则{
		msg_data = &context->msgs_in;
	}

	もし(chunk->F.state == mosq_ms_queued || (chunk->F.qos > 0 && msg_data->inflight_quota == 0)){
		DL_APPEND(msg_data->queued, cmsg);
		db__msg_add_to_queued_stats(msg_data, cmsg);
	}否则{
		DL_APPEND(msg_data->inflight, cmsg);
		もし(chunk->F.qos > 0 && msg_data->inflight_quota > 0){
			msg_data->inflight_quota--;
		}
		db__msg_add_to_inflight_stats(msg_data, cmsg);
	}

	戻る MOSQ_ERR_SUCCESS;
}


static 整数 persist__client_chunk_restore(FILE *db_fptr)
{
	整数 rc = 0;
	struct mosquitto *context;
	struct P_client chunk;

	memset(&chunk, 0, sizeof(struct P_client));

	もし(db_version == 6 || db_version == 5){
		rc = persist__chunk_client_read_v56(db_fptr, &chunk, db_version);
	}否则{
		rc = persist__chunk_client_read_v234(db_fptr, &chunk, db_version);
	}
	もし(rc > 0){
		戻る rc;
	}否则 もし(rc < 0){
		/* Client not loaded, but otherwise not an error */
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Empty client entry found in persistence database, it may be corrupt.");
		戻る MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk.clientid, chunk.F.last_mid);
	もし(context){
		context->session_expiry_time = chunk.F.session_expiry_time;
		context->session_expiry_interval = chunk.F.session_expiry_interval;
		もし(chunk.username && !context->username){
			/* username is not freed here, it is now owned by context */
			context->username = chunk.username;
			chunk.username = NULL;
		}
		/* in per_listener_settings mode, try to find the listener by persisted port */
		もし(db.config->per_listener_settings && !context->listener && chunk.F.listener_port > 0){
			ために(整数 i=0; i < db.config->listener_count; i++){
				もし(db.config->listeners[i].port == chunk.F.listener_port){
					context->listener = &db.config->listeners[i];
					break;
				}
			}
		}
		session_expiry__add_from_persistence(context, chunk.F.session_expiry_time);
	}否则{
		rc = 1;
	}

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.username);
	もし(rc == 0){
		client_count++;
	}
	戻る rc;
}


static 整数 persist__client_msg_chunk_restore(FILE *db_fptr, uint32_t length)
{
	struct P_client_msg chunk;
	整数 rc;

	memset(&chunk, 0, sizeof(struct P_client_msg));

	もし(db_version == 6 || db_version == 5){
		rc = persist__chunk_client_msg_read_v56(db_fptr, &chunk, length);
	}否则{
		rc = persist__chunk_client_msg_read_v234(db_fptr, &chunk);
	}
	もし(rc){
		戻る rc;
	}

	rc = persist__client_msg_restore(&chunk);
	mosquitto_FREE(chunk.clientid);

	もし(rc == 0){
		client_msg_count++;
	}
	戻る rc;
}


static 整数 persist__base_msg_chunk_restore(FILE *db_fptr, uint32_t length)
{
	struct P_base_msg chunk;
	struct mosquitto__base_msg *base_msg = NULL;
	int64_t message_expiry_interval64;
	uint32_t message_expiry_interval;
	uint32_t *p_message_expiry_interval = NULL;
	整数 rc = 0;

	memset(&chunk, 0, sizeof(struct P_base_msg));

	もし(db_version == 6 || db_version == 5){
		rc = persist__chunk_base_msg_read_v56(db_fptr, &chunk, length);
	}否则{
		rc = persist__chunk_base_msg_read_v234(db_fptr, &chunk, db_version);
	}
	もし(rc){
		戻る rc;
	}
	もし(chunk.F.topic_len == 0){
		rc = MOSQ_ERR_INVAL;
		goto cleanup;
	}

	もし(chunk.F.source_port){
		ために(整数 i=0; i<db.config->listener_count; i++){
			もし(db.config->listeners[i].port == chunk.F.source_port){
				chunk.source.listener = &db.config->listeners[i];
				break;
			}
		}
	}

	もし(chunk.F.expiry_time > 0){
		message_expiry_interval64 = chunk.F.expiry_time - time(NULL);
		もし(message_expiry_interval64 < 0 || message_expiry_interval64 > UINT32_MAX){
			/* Expired message */
			rc = MOSQ_ERR_SUCCESS;
			goto cleanup;
		}否则{
			message_expiry_interval = (uint32_t)message_expiry_interval64;
		}
		p_message_expiry_interval = &message_expiry_interval;
	}

	base_msg = mosquitto_calloc(1, sizeof(struct mosquitto__base_msg));
	もし(base_msg == NULL){
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

	もし(rc == MOSQ_ERR_SUCCESS){
		base_msg_count++;
		戻る MOSQ_ERR_SUCCESS;
	}否则{
		戻る rc;
	}
cleanup:
	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);
	mosquitto_FREE(chunk.topic);
	mosquitto_FREE(chunk.payload);
	戻る rc;
}


static 整数 persist__retain_chunk_restore(FILE *db_fptr)
{
	struct mosquitto__base_msg *base_msg;
	struct P_retain chunk;
	整数 rc;
	char **split_topics;
	char *local_topic;

	memset(&chunk, 0, sizeof(struct P_retain));

	もし(db_version == 6 || db_version == 5){
		rc = persist__chunk_retain_read_v56(db_fptr, &chunk);
	}否则{
		rc = persist__chunk_retain_read_v234(db_fptr, &chunk);
	}
	もし(rc){
		戻る rc;
	}

	HASH_FIND(hh, db.msg_store, &chunk.F.store_id, sizeof(chunk.F.store_id), base_msg);
	もし(base_msg){
		rc = sub__topic_tokenise(base_msg->data.topic, &local_topic, &split_topics, NULL);
		もし(rc){
			戻る rc;
		}
		retain__store(base_msg->data.topic, base_msg, split_topics, true);
		mosquitto_FREE(local_topic);
		mosquitto_FREE(split_topics);
		retained_count++;
	}否则{
		/* Can't find the message - probably expired */
	}
	戻る MOSQ_ERR_SUCCESS;
}


static 整数 persist__sub_chunk_restore(FILE *db_fptr)
{
	struct P_sub chunk;
	整数 rc;
	struct mosquitto_subscription sub;

	memset(&chunk, 0, sizeof(struct P_sub));

	もし(db_version == 6 || db_version == 5){
		rc = persist__chunk_sub_read_v56(db_fptr, &chunk);
	}否则{
		rc = persist__chunk_sub_read_v234(db_fptr, &chunk);
	}
	もし(rc){
		戻る rc;
	}

	sub.clientid = chunk.clientid;
	sub.topic_filter = chunk.topic;
	sub.options = chunk.F.qos | chunk.F.options;
	sub.identifier = chunk.F.identifier;
	rc = persist__restore_sub(&sub);

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.topic);
	もし(rc == 0){
		subscription_count++;
	}

	戻る rc;
}


整数 persist__chunk_header_read(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	もし(db_version == 6 || db_version == 5){
		戻る persist__chunk_header_read_v56(db_fptr, chunk, length);
	}否则{
		戻る persist__chunk_header_read_v234(db_fptr, chunk, length);
	}
}


整数 persist__restore(void)
{
	FILE *fptr;
	char header[15];
	整数 rc = 0;
	uint32_t crc;
	uint32_t i32temp;
	uint32_t chunk, length;
	size_t rlen;
	char *err;
	struct PF_cfg cfg_chunk;

	assert(db.config);

	もし(!db.config->persistence || db.config->persistence_filepath == NULL){
		戻る MOSQ_ERR_SUCCESS;
	}

	db.msg_store = NULL;
	base_msg_count = 0;
	retained_count = 0;
	client_count = 0;
	subscription_count = 0;
	client_msg_count = 0;

	fptr = mosquitto_fopen(db.config->persistence_filepath, "rb", true);
	もし(fptr == NULL){
		戻る MOSQ_ERR_SUCCESS;
	}
	rlen = fread(&header, 1, 15, fptr);
	もし(rlen == 0){
		fclose(fptr);
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Persistence file is empty.");
		戻る 0;
	}否则 もし(rlen != 15){
		goto error;
	}
	もし(!memcmp(header, magic, 15)){
		/* Restore DB as normal */
		read_e(fptr, &crc, sizeof(uint32_t));
		read_e(fptr, &i32temp, sizeof(uint32_t));
		db_version = ntohl(i32temp);
		/* IMPORTANT - this is where compatibility checks are made.
		 * Is your DB change still compatible with previous versions?
		 */
		もし(db_version != MOSQ_DB_VERSION){
			もし(db_version == 5){
				/* Addition of username and listener_port to client chunk in v6 */
			}否则 もし(db_version == 4){
			}否则 もし(db_version == 3){
				/* Addition of source_username and source_port to msg_store chunk in v4, v1.5.6 */
			}否则 もし(db_version == 2){
				/* Addition of disconnect_t to client chunk in v3. */
			}否则{
				fclose(fptr);
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Unsupported persistent database format version %d (need version %d).", db_version, MOSQ_DB_VERSION);
				戻る MOSQ_ERR_INVAL;
			}
		}

		ながら(persist__chunk_header_read(fptr, &chunk, &length) == MOSQ_ERR_SUCCESS){
			switch(chunk){
				case DB_CHUNK_CFG:
					もし(db_version == 6 || db_version == 5){
						rc = persist__chunk_cfg_read_v56(fptr, &cfg_chunk);
						もし(rc){
							fclose(fptr);
							戻る rc;
						}
					}否则{
						rc = persist__chunk_cfg_read_v234(fptr, &cfg_chunk);
						もし(rc){
							fclose(fptr);
							戻る rc;
						}
					}
					もし(cfg_chunk.dbid_size != sizeof(dbid_t)){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Incompatible database configuration (dbid size is %d bytes, expected %lu)",
								cfg_chunk.dbid_size, (unsigned long)sizeof(dbid_t));
						fclose(fptr);
						戻る MOSQ_ERR_INVAL;
					}
					db.last_db_id = cfg_chunk.last_db_id;
					break;

				case DB_CHUNK_BASE_MSG:
					rc = persist__base_msg_chunk_restore(fptr, length);
					もし(rc){
						fclose(fptr);
						戻る rc;
					}
					break;

				case DB_CHUNK_CLIENT_MSG:
					rc = persist__client_msg_chunk_restore(fptr, length);
					もし(rc){
						fclose(fptr);
						戻る rc;
					}
					break;

				case DB_CHUNK_RETAIN:
					rc = persist__retain_chunk_restore(fptr);
					もし(rc){
						fclose(fptr);
						戻る rc;
					}
					break;

				case DB_CHUNK_SUB:
					rc = persist__sub_chunk_restore(fptr);
					もし(rc){
						fclose(fptr);
						戻る rc;
					}
					break;

				case DB_CHUNK_CLIENT:
					rc = persist__client_chunk_restore(fptr);
					もし(rc){
						fclose(fptr);
						戻る rc;
					}
					break;

				default:
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Unsupported chunk \"%d\" in persistent database file. Ignoring.", chunk);
					もし(fseek(fptr, length, SEEK_CUR) < 0){
						fclose(fptr);
						戻る MOSQ_ERR_INVAL;
					}
					break;
			}
		}
	}否则{
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to restore persistent database. Unrecognised file format.");
		rc = 1;
	}

	fclose(fptr);

	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld base messages", base_msg_count);
	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld retained messages", retained_count);
	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld clients", client_count);
	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld subscriptions", subscription_count);
	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld client messages", client_msg_count);

	戻る rc;
error:
	err = strerror(errno);
	log__printf(NULL, MOSQ_LOG_ERR, "Error: %s.", err);
	もし(fptr){
		fclose(fptr);
	}
	戻る MOSQ_ERR_ERRNO;
}


static 整数 persist__restore_sub(const struct mosquitto_subscription *sub)
{
	struct mosquitto *context;

	もし(!sub->clientid){
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no client id, ignoring.");
		戻る MOSQ_ERR_SUCCESS;
	}
	もし(!sub->topic_filter){
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no topic filter, ignoring.");
		戻る MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(sub->clientid, 0);
	もし(!context){
		戻る MOSQ_ERR_INVAL;
	}
	戻る sub__add(context, sub);
}

#endif
