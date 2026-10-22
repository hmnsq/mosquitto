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

static số_nguyên persist__restore_sub(const struct mosquitto_subscription *sub);

static struct mosquitto *persist__find_or_add_context(const char *clientid, uint16_t last_mid)
{
	struct mosquitto *context;

	nếu(!clientid){
		trả_về NULL;
	}

	context = NULL;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	nếu(!context){
		context = context__init();
		nếu(!context){
			trả_về NULL;
		}
		context->id = mosquitto_strdup(clientid);
		nếu(!context->id){
			mosquitto_FREE(context);
			trả_về NULL;
		}

		context->clean_start = false;

		context__add_to_by_id(context);
	}
	nếu(last_mid){
		context->last_mid = last_mid;
	}
	trả_về context;
}


số_nguyên persist__read_string_len(FILE *db_fptr, char **str, uint16_t len)
{
	char *s = NULL;

	nếu(len){
		s = mosquitto_malloc(len+1U);
		nếu(!s){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
			trả_về MOSQ_ERR_NOMEM;
		}
		nếu(fread(s, 1, len, db_fptr) != len){
			mosquitto_FREE(s);
			trả_về MOSQ_ERR_NOMEM;
		}
		s[len] = '\0';
	}

	*str = s;
	trả_về MOSQ_ERR_SUCCESS;
}


số_nguyên persist__read_string(FILE *db_fptr, char **str)
{
	uint16_t i16temp;
	uint16_t slen;

	nếu(fread(&i16temp, 1, sizeof(uint16_t), db_fptr) != sizeof(uint16_t)){
		trả_về MOSQ_ERR_INVAL;
	}

	slen = ntohs(i16temp);
	trả_về persist__read_string_len(db_fptr, str, slen);
}


static số_nguyên persist__client_msg_restore(struct P_client_msg *chunk)
{
	struct mosquitto__client_msg *cmsg;
	struct mosquitto__base_msg *msg;
	struct mosquitto *context;
	struct mosquitto_msg_data *msg_data;

	HASH_FIND(hh, db.msg_store, &chunk->F.store_id, sizeof(chunk->F.store_id), msg);
	nếu(!msg){
		/* Can't find message - probably expired */
		trả_về MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk->clientid, 0);
	nếu(!context){
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Persistence file contains client message with no matching client. File may be corrupt.");
		trả_về 0;
	}

	cmsg = mosquitto_calloc(1, sizeof(struct mosquitto__client_msg));
	nếu(!cmsg){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		trả_về MOSQ_ERR_NOMEM;
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

	nếu(cmsg->data.direction == mosq_md_out){
		msg_data = &context->msgs_out;
	}không_thì{
		msg_data = &context->msgs_in;
	}

	nếu(chunk->F.state == mosq_ms_queued || (chunk->F.qos > 0 && msg_data->inflight_quota == 0)){
		DL_APPEND(msg_data->queued, cmsg);
		db__msg_add_to_queued_stats(msg_data, cmsg);
	}không_thì{
		DL_APPEND(msg_data->inflight, cmsg);
		nếu(chunk->F.qos > 0 && msg_data->inflight_quota > 0){
			msg_data->inflight_quota--;
		}
		db__msg_add_to_inflight_stats(msg_data, cmsg);
	}

	trả_về MOSQ_ERR_SUCCESS;
}


static số_nguyên persist__client_chunk_restore(FILE *db_fptr)
{
	số_nguyên rc = 0;
	struct mosquitto *context;
	struct P_client chunk;

	memset(&chunk, 0, sizeof(struct P_client));

	nếu(db_version == 6 || db_version == 5){
		rc = persist__chunk_client_read_v56(db_fptr, &chunk, db_version);
	}không_thì{
		rc = persist__chunk_client_read_v234(db_fptr, &chunk, db_version);
	}
	nếu(rc > 0){
		trả_về rc;
	}không_thì nếu(rc < 0){
		/* Client not loaded, but otherwise not an error */
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Empty client entry found in persistence database, it may be corrupt.");
		trả_về MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk.clientid, chunk.F.last_mid);
	nếu(context){
		context->session_expiry_time = chunk.F.session_expiry_time;
		context->session_expiry_interval = chunk.F.session_expiry_interval;
		nếu(chunk.username && !context->username){
			/* username is not freed here, it is now owned by context */
			context->username = chunk.username;
			chunk.username = NULL;
		}
		/* in per_listener_settings mode, try to find the listener by persisted port */
		nếu(db.config->per_listener_settings && !context->listener && chunk.F.listener_port > 0){
			cho(số_nguyên i=0; i < db.config->listener_count; i++){
				nếu(db.config->listeners[i].port == chunk.F.listener_port){
					context->listener = &db.config->listeners[i];
					break;
				}
			}
		}
		session_expiry__add_from_persistence(context, chunk.F.session_expiry_time);
	}không_thì{
		rc = 1;
	}

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.username);
	nếu(rc == 0){
		client_count++;
	}
	trả_về rc;
}


static số_nguyên persist__client_msg_chunk_restore(FILE *db_fptr, uint32_t length)
{
	struct P_client_msg chunk;
	số_nguyên rc;

	memset(&chunk, 0, sizeof(struct P_client_msg));

	nếu(db_version == 6 || db_version == 5){
		rc = persist__chunk_client_msg_read_v56(db_fptr, &chunk, length);
	}không_thì{
		rc = persist__chunk_client_msg_read_v234(db_fptr, &chunk);
	}
	nếu(rc){
		trả_về rc;
	}

	rc = persist__client_msg_restore(&chunk);
	mosquitto_FREE(chunk.clientid);

	nếu(rc == 0){
		client_msg_count++;
	}
	trả_về rc;
}


static số_nguyên persist__base_msg_chunk_restore(FILE *db_fptr, uint32_t length)
{
	struct P_base_msg chunk;
	struct mosquitto__base_msg *base_msg = NULL;
	int64_t message_expiry_interval64;
	uint32_t message_expiry_interval;
	uint32_t *p_message_expiry_interval = NULL;
	số_nguyên rc = 0;

	memset(&chunk, 0, sizeof(struct P_base_msg));

	nếu(db_version == 6 || db_version == 5){
		rc = persist__chunk_base_msg_read_v56(db_fptr, &chunk, length);
	}không_thì{
		rc = persist__chunk_base_msg_read_v234(db_fptr, &chunk, db_version);
	}
	nếu(rc){
		trả_về rc;
	}
	nếu(chunk.F.topic_len == 0){
		rc = MOSQ_ERR_INVAL;
		goto cleanup;
	}

	nếu(chunk.F.source_port){
		cho(số_nguyên i=0; i<db.config->listener_count; i++){
			nếu(db.config->listeners[i].port == chunk.F.source_port){
				chunk.source.listener = &db.config->listeners[i];
				break;
			}
		}
	}

	nếu(chunk.F.expiry_time > 0){
		message_expiry_interval64 = chunk.F.expiry_time - time(NULL);
		nếu(message_expiry_interval64 < 0 || message_expiry_interval64 > UINT32_MAX){
			/* Expired message */
			rc = MOSQ_ERR_SUCCESS;
			goto cleanup;
		}không_thì{
			message_expiry_interval = (uint32_t)message_expiry_interval64;
		}
		p_message_expiry_interval = &message_expiry_interval;
	}

	base_msg = mosquitto_calloc(1, sizeof(struct mosquitto__base_msg));
	nếu(base_msg == NULL){
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

	nếu(rc == MOSQ_ERR_SUCCESS){
		base_msg_count++;
		trả_về MOSQ_ERR_SUCCESS;
	}không_thì{
		trả_về rc;
	}
cleanup:
	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);
	mosquitto_FREE(chunk.topic);
	mosquitto_FREE(chunk.payload);
	trả_về rc;
}


static số_nguyên persist__retain_chunk_restore(FILE *db_fptr)
{
	struct mosquitto__base_msg *base_msg;
	struct P_retain chunk;
	số_nguyên rc;
	char **split_topics;
	char *local_topic;

	memset(&chunk, 0, sizeof(struct P_retain));

	nếu(db_version == 6 || db_version == 5){
		rc = persist__chunk_retain_read_v56(db_fptr, &chunk);
	}không_thì{
		rc = persist__chunk_retain_read_v234(db_fptr, &chunk);
	}
	nếu(rc){
		trả_về rc;
	}

	HASH_FIND(hh, db.msg_store, &chunk.F.store_id, sizeof(chunk.F.store_id), base_msg);
	nếu(base_msg){
		rc = sub__topic_tokenise(base_msg->data.topic, &local_topic, &split_topics, NULL);
		nếu(rc){
			trả_về rc;
		}
		retain__store(base_msg->data.topic, base_msg, split_topics, true);
		mosquitto_FREE(local_topic);
		mosquitto_FREE(split_topics);
		retained_count++;
	}không_thì{
		/* Can't find the message - probably expired */
	}
	trả_về MOSQ_ERR_SUCCESS;
}


static số_nguyên persist__sub_chunk_restore(FILE *db_fptr)
{
	struct P_sub chunk;
	số_nguyên rc;
	struct mosquitto_subscription sub;

	memset(&chunk, 0, sizeof(struct P_sub));

	nếu(db_version == 6 || db_version == 5){
		rc = persist__chunk_sub_read_v56(db_fptr, &chunk);
	}không_thì{
		rc = persist__chunk_sub_read_v234(db_fptr, &chunk);
	}
	nếu(rc){
		trả_về rc;
	}

	sub.clientid = chunk.clientid;
	sub.topic_filter = chunk.topic;
	sub.options = chunk.F.qos | chunk.F.options;
	sub.identifier = chunk.F.identifier;
	rc = persist__restore_sub(&sub);

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.topic);
	nếu(rc == 0){
		subscription_count++;
	}

	trả_về rc;
}


số_nguyên persist__chunk_header_read(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	nếu(db_version == 6 || db_version == 5){
		trả_về persist__chunk_header_read_v56(db_fptr, chunk, length);
	}không_thì{
		trả_về persist__chunk_header_read_v234(db_fptr, chunk, length);
	}
}


số_nguyên persist__restore(void)
{
	FILE *fptr;
	char header[15];
	số_nguyên rc = 0;
	uint32_t crc;
	uint32_t i32temp;
	uint32_t chunk, length;
	size_t rlen;
	char *err;
	struct PF_cfg cfg_chunk;

	assert(db.config);

	nếu(!db.config->persistence || db.config->persistence_filepath == NULL){
		trả_về MOSQ_ERR_SUCCESS;
	}

	db.msg_store = NULL;
	base_msg_count = 0;
	retained_count = 0;
	client_count = 0;
	subscription_count = 0;
	client_msg_count = 0;

	fptr = mosquitto_fopen(db.config->persistence_filepath, "rb", true);
	nếu(fptr == NULL){
		trả_về MOSQ_ERR_SUCCESS;
	}
	rlen = fread(&header, 1, 15, fptr);
	nếu(rlen == 0){
		fclose(fptr);
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Persistence file is empty.");
		trả_về 0;
	}không_thì nếu(rlen != 15){
		goto error;
	}
	nếu(!memcmp(header, magic, 15)){
		/* Restore DB as normal */
		read_e(fptr, &crc, sizeof(uint32_t));
		read_e(fptr, &i32temp, sizeof(uint32_t));
		db_version = ntohl(i32temp);
		/* IMPORTANT - this is where compatibility checks are made.
		 * Is your DB change still compatible with previous versions?
		 */
		nếu(db_version != MOSQ_DB_VERSION){
			nếu(db_version == 5){
				/* Addition of username and listener_port to client chunk in v6 */
			}không_thì nếu(db_version == 4){
			}không_thì nếu(db_version == 3){
				/* Addition of source_username and source_port to msg_store chunk in v4, v1.5.6 */
			}không_thì nếu(db_version == 2){
				/* Addition of disconnect_t to client chunk in v3. */
			}không_thì{
				fclose(fptr);
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Unsupported persistent database format version %d (need version %d).", db_version, MOSQ_DB_VERSION);
				trả_về MOSQ_ERR_INVAL;
			}
		}

		trong_khi(persist__chunk_header_read(fptr, &chunk, &length) == MOSQ_ERR_SUCCESS){
			switch(chunk){
				case DB_CHUNK_CFG:
					nếu(db_version == 6 || db_version == 5){
						rc = persist__chunk_cfg_read_v56(fptr, &cfg_chunk);
						nếu(rc){
							fclose(fptr);
							trả_về rc;
						}
					}không_thì{
						rc = persist__chunk_cfg_read_v234(fptr, &cfg_chunk);
						nếu(rc){
							fclose(fptr);
							trả_về rc;
						}
					}
					nếu(cfg_chunk.dbid_size != sizeof(dbid_t)){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Incompatible database configuration (dbid size is %d bytes, expected %lu)",
								cfg_chunk.dbid_size, (unsigned long)sizeof(dbid_t));
						fclose(fptr);
						trả_về MOSQ_ERR_INVAL;
					}
					db.last_db_id = cfg_chunk.last_db_id;
					break;

				case DB_CHUNK_BASE_MSG:
					rc = persist__base_msg_chunk_restore(fptr, length);
					nếu(rc){
						fclose(fptr);
						trả_về rc;
					}
					break;

				case DB_CHUNK_CLIENT_MSG:
					rc = persist__client_msg_chunk_restore(fptr, length);
					nếu(rc){
						fclose(fptr);
						trả_về rc;
					}
					break;

				case DB_CHUNK_RETAIN:
					rc = persist__retain_chunk_restore(fptr);
					nếu(rc){
						fclose(fptr);
						trả_về rc;
					}
					break;

				case DB_CHUNK_SUB:
					rc = persist__sub_chunk_restore(fptr);
					nếu(rc){
						fclose(fptr);
						trả_về rc;
					}
					break;

				case DB_CHUNK_CLIENT:
					rc = persist__client_chunk_restore(fptr);
					nếu(rc){
						fclose(fptr);
						trả_về rc;
					}
					break;

				default:
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Unsupported chunk \"%d\" in persistent database file. Ignoring.", chunk);
					nếu(fseek(fptr, length, SEEK_CUR) < 0){
						fclose(fptr);
						trả_về MOSQ_ERR_INVAL;
					}
					break;
			}
		}
	}không_thì{
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to restore persistent database. Unrecognised file format.");
		rc = 1;
	}

	fclose(fptr);

	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld base messages", base_msg_count);
	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld retained messages", retained_count);
	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld clients", client_count);
	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld subscriptions", subscription_count);
	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld client messages", client_msg_count);

	trả_về rc;
error:
	err = strerror(errno);
	log__printf(NULL, MOSQ_LOG_ERR, "Error: %s.", err);
	nếu(fptr){
		fclose(fptr);
	}
	trả_về MOSQ_ERR_ERRNO;
}


static số_nguyên persist__restore_sub(const struct mosquitto_subscription *sub)
{
	struct mosquitto *context;

	nếu(!sub->clientid){
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no client id, ignoring.");
		trả_về MOSQ_ERR_SUCCESS;
	}
	nếu(!sub->topic_filter){
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no topic filter, ignoring.");
		trả_về MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(sub->clientid, 0);
	nếu(!context){
		trả_về MOSQ_ERR_INVAL;
	}
	trả_về sub__add(context, sub);
}

#endif
