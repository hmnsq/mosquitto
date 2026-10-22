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

static ကိန်းပြည့် persist__restore_sub(const sa_nracanaa mosquitto_subscription *sub);

static sa_nracanaa mosquitto *persist__find_or_add_context(const var_na *clientid, uint16_t last_mid)
{
	sa_nracanaa mosquitto *context;

	အကယ်၍(!clientid){
		ပြန်ပေး NULL;
	}

	context = NULL;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	အကယ်၍(!context){
		context = context__init();
		အကယ်၍(!context){
			ပြန်ပေး NULL;
		}
		context->id = mosquitto_strdup(clientid);
		အကယ်၍(!context->id){
			mosquitto_FREE(context);
			ပြန်ပေး NULL;
		}

		context->clean_start = false;

		context__add_to_by_id(context);
	}
	အကယ်၍(last_mid){
		context->last_mid = last_mid;
	}
	ပြန်ပေး context;
}


ကိန်းပြည့် persist__read_string_len(FILE *db_fptr, var_na **str, uint16_t len)
{
	var_na *s = NULL;

	အကယ်၍(len){
		s = mosquitto_malloc(len+1U);
		အကယ်၍(!s){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
			ပြန်ပေး MOSQ_ERR_NOMEM;
		}
		အကယ်၍(fread(s, 1, len, db_fptr) != len){
			mosquitto_FREE(s);
			ပြန်ပေး MOSQ_ERR_NOMEM;
		}
		s[len] = '\0';
	}

	*str = s;
	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


ကိန်းပြည့် persist__read_string(FILE *db_fptr, var_na **str)
{
	uint16_t i16temp;
	uint16_t slen;

	အကယ်၍(fread(&i16temp, 1, parimaa_na(uint16_t), db_fptr) != parimaa_na(uint16_t)){
		ပြန်ပေး MOSQ_ERR_INVAL;
	}

	slen = ntohs(i16temp);
	ပြန်ပေး persist__read_string_len(db_fptr, str, slen);
}


static ကိန်းပြည့် persist__client_msg_restore(sa_nracanaa P_client_msg *chunk)
{
	sa_nracanaa mosquitto__client_msg *cmsg;
	sa_nracanaa mosquitto__base_msg *msg;
	sa_nracanaa mosquitto *context;
	sa_nracanaa mosquitto_msg_data *msg_data;

	HASH_FIND(hh, db.msg_store, &chunk->F.store_id, parimaa_na(chunk->F.store_id), msg);
	အကယ်၍(!msg){
		/* Can't find message - probably expired */
		ပြန်ပေး MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk->clientid, 0);
	အကယ်၍(!context){
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Persistence file contains client message with no matching client. File may be corrupt.");
		ပြန်ပေး 0;
	}

	cmsg = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__client_msg));
	အကယ်၍(!cmsg){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		ပြန်ပေး MOSQ_ERR_NOMEM;
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

	အကယ်၍(cmsg->data.direction == mosq_md_out){
		msg_data = &context->msgs_out;
	}မဟုတ်ပါက{
		msg_data = &context->msgs_in;
	}

	အကယ်၍(chunk->F.state == mosq_ms_queued || (chunk->F.qos > 0 && msg_data->inflight_quota == 0)){
		DL_APPEND(msg_data->queued, cmsg);
		db__msg_add_to_queued_stats(msg_data, cmsg);
	}မဟုတ်ပါက{
		DL_APPEND(msg_data->inflight, cmsg);
		အကယ်၍(chunk->F.qos > 0 && msg_data->inflight_quota > 0){
			msg_data->inflight_quota--;
		}
		db__msg_add_to_inflight_stats(msg_data, cmsg);
	}

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


static ကိန်းပြည့် persist__client_chunk_restore(FILE *db_fptr)
{
	ကိန်းပြည့် rc = 0;
	sa_nracanaa mosquitto *context;
	sa_nracanaa P_client chunk;

	memset(&chunk, 0, parimaa_na(sa_nracanaa P_client));

	အကယ်၍(db_version == 6 || db_version == 5){
		rc = persist__chunk_client_read_v56(db_fptr, &chunk, db_version);
	}မဟုတ်ပါက{
		rc = persist__chunk_client_read_v234(db_fptr, &chunk, db_version);
	}
	အကယ်၍(rc > 0){
		ပြန်ပေး rc;
	}မဟုတ်ပါက အကယ်၍(rc < 0){
		/* Client not loaded, but otherwise not an error */
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Empty client entry found in persistence database, it may be corrupt.");
		ပြန်ပေး MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk.clientid, chunk.F.last_mid);
	အကယ်၍(context){
		context->session_expiry_time = chunk.F.session_expiry_time;
		context->session_expiry_interval = chunk.F.session_expiry_interval;
		အကယ်၍(chunk.username && !context->username){
			/* username is not freed here, it is now owned by context */
			context->username = chunk.username;
			chunk.username = NULL;
		}
		/* in per_listener_settings mode, try to find the listener by persisted port */
		အကယ်၍(db.config->per_listener_settings && !context->listener && chunk.F.listener_port > 0){
			အတွက်(ကိန်းပြည့် i=0; i < db.config->listener_count; i++){
				အကယ်၍(db.config->listeners[i].port == chunk.F.listener_port){
					context->listener = &db.config->listeners[i];
					bha_nga;
				}
			}
		}
		session_expiry__add_from_persistence(context, chunk.F.session_expiry_time);
	}မဟုတ်ပါက{
		rc = 1;
	}

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.username);
	အကယ်၍(rc == 0){
		client_count++;
	}
	ပြန်ပေး rc;
}


static ကိန်းပြည့် persist__client_msg_chunk_restore(FILE *db_fptr, uint32_t length)
{
	sa_nracanaa P_client_msg chunk;
	ကိန်းပြည့် rc;

	memset(&chunk, 0, parimaa_na(sa_nracanaa P_client_msg));

	အကယ်၍(db_version == 6 || db_version == 5){
		rc = persist__chunk_client_msg_read_v56(db_fptr, &chunk, length);
	}မဟုတ်ပါက{
		rc = persist__chunk_client_msg_read_v234(db_fptr, &chunk);
	}
	အကယ်၍(rc){
		ပြန်ပေး rc;
	}

	rc = persist__client_msg_restore(&chunk);
	mosquitto_FREE(chunk.clientid);

	အကယ်၍(rc == 0){
		client_msg_count++;
	}
	ပြန်ပေး rc;
}


static ကိန်းပြည့် persist__base_msg_chunk_restore(FILE *db_fptr, uint32_t length)
{
	sa_nracanaa P_base_msg chunk;
	sa_nracanaa mosquitto__base_msg *base_msg = NULL;
	int64_t message_expiry_interval64;
	uint32_t message_expiry_interval;
	uint32_t *p_message_expiry_interval = NULL;
	ကိန်းပြည့် rc = 0;

	memset(&chunk, 0, parimaa_na(sa_nracanaa P_base_msg));

	အကယ်၍(db_version == 6 || db_version == 5){
		rc = persist__chunk_base_msg_read_v56(db_fptr, &chunk, length);
	}မဟုတ်ပါက{
		rc = persist__chunk_base_msg_read_v234(db_fptr, &chunk, db_version);
	}
	အကယ်၍(rc){
		ပြန်ပေး rc;
	}
	အကယ်၍(chunk.F.topic_len == 0){
		rc = MOSQ_ERR_INVAL;
		goto cleanup;
	}

	အကယ်၍(chunk.F.source_port){
		အတွက်(ကိန်းပြည့် i=0; i<db.config->listener_count; i++){
			အကယ်၍(db.config->listeners[i].port == chunk.F.source_port){
				chunk.source.listener = &db.config->listeners[i];
				bha_nga;
			}
		}
	}

	အကယ်၍(chunk.F.expiry_time > 0){
		message_expiry_interval64 = chunk.F.expiry_time - time(NULL);
		အကယ်၍(message_expiry_interval64 < 0 || message_expiry_interval64 > UINT32_MAX){
			/* Expired message */
			rc = MOSQ_ERR_SUCCESS;
			goto cleanup;
		}မဟုတ်ပါက{
			message_expiry_interval = (uint32_t)message_expiry_interval64;
		}
		p_message_expiry_interval = &message_expiry_interval;
	}

	base_msg = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__base_msg));
	အကယ်၍(base_msg == NULL){
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

	အကယ်၍(rc == MOSQ_ERR_SUCCESS){
		base_msg_count++;
		ပြန်ပေး MOSQ_ERR_SUCCESS;
	}မဟုတ်ပါက{
		ပြန်ပေး rc;
	}
cleanup:
	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);
	mosquitto_FREE(chunk.topic);
	mosquitto_FREE(chunk.payload);
	ပြန်ပေး rc;
}


static ကိန်းပြည့် persist__retain_chunk_restore(FILE *db_fptr)
{
	sa_nracanaa mosquitto__base_msg *base_msg;
	sa_nracanaa P_retain chunk;
	ကိန်းပြည့် rc;
	var_na **split_topics;
	var_na *local_topic;

	memset(&chunk, 0, parimaa_na(sa_nracanaa P_retain));

	အကယ်၍(db_version == 6 || db_version == 5){
		rc = persist__chunk_retain_read_v56(db_fptr, &chunk);
	}မဟုတ်ပါက{
		rc = persist__chunk_retain_read_v234(db_fptr, &chunk);
	}
	အကယ်၍(rc){
		ပြန်ပေး rc;
	}

	HASH_FIND(hh, db.msg_store, &chunk.F.store_id, parimaa_na(chunk.F.store_id), base_msg);
	အကယ်၍(base_msg){
		rc = sub__topic_tokenise(base_msg->data.topic, &local_topic, &split_topics, NULL);
		အကယ်၍(rc){
			ပြန်ပေး rc;
		}
		retain__store(base_msg->data.topic, base_msg, split_topics, true);
		mosquitto_FREE(local_topic);
		mosquitto_FREE(split_topics);
		retained_count++;
	}မဟုတ်ပါက{
		/* Can't find the message - probably expired */
	}
	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


static ကိန်းပြည့် persist__sub_chunk_restore(FILE *db_fptr)
{
	sa_nracanaa P_sub chunk;
	ကိန်းပြည့် rc;
	sa_nracanaa mosquitto_subscription sub;

	memset(&chunk, 0, parimaa_na(sa_nracanaa P_sub));

	အကယ်၍(db_version == 6 || db_version == 5){
		rc = persist__chunk_sub_read_v56(db_fptr, &chunk);
	}မဟုတ်ပါက{
		rc = persist__chunk_sub_read_v234(db_fptr, &chunk);
	}
	အကယ်၍(rc){
		ပြန်ပေး rc;
	}

	sub.clientid = chunk.clientid;
	sub.topic_filter = chunk.topic;
	sub.options = chunk.F.qos | chunk.F.options;
	sub.identifier = chunk.F.identifier;
	rc = persist__restore_sub(&sub);

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.topic);
	အကယ်၍(rc == 0){
		subscription_count++;
	}

	ပြန်ပေး rc;
}


ကိန်းပြည့် persist__chunk_header_read(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	အကယ်၍(db_version == 6 || db_version == 5){
		ပြန်ပေး persist__chunk_header_read_v56(db_fptr, chunk, length);
	}မဟုတ်ပါက{
		ပြန်ပေး persist__chunk_header_read_v234(db_fptr, chunk, length);
	}
}


ကိန်းပြည့် persist__restore(void)
{
	FILE *fptr;
	var_na header[15];
	ကိန်းပြည့် rc = 0;
	uint32_t crc;
	uint32_t i32temp;
	uint32_t chunk, length;
	size_t rlen;
	var_na *err;
	sa_nracanaa PF_cfg cfg_chunk;

	assert(db.config);

	အကယ်၍(!db.config->persistence || db.config->persistence_filepath == NULL){
		ပြန်ပေး MOSQ_ERR_SUCCESS;
	}

	db.msg_store = NULL;
	base_msg_count = 0;
	retained_count = 0;
	client_count = 0;
	subscription_count = 0;
	client_msg_count = 0;

	fptr = mosquitto_fopen(db.config->persistence_filepath, "rb", true);
	အကယ်၍(fptr == NULL){
		ပြန်ပေး MOSQ_ERR_SUCCESS;
	}
	rlen = fread(&header, 1, 15, fptr);
	အကယ်၍(rlen == 0){
		fclose(fptr);
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Persistence file is empty.");
		ပြန်ပေး 0;
	}မဟုတ်ပါက အကယ်၍(rlen != 15){
		goto error;
	}
	အကယ်၍(!memcmp(header, magic, 15)){
		/* Restore DB as normal */
		read_e(fptr, &crc, parimaa_na(uint32_t));
		read_e(fptr, &i32temp, parimaa_na(uint32_t));
		db_version = ntohl(i32temp);
		/* IMPORTANT - this is where compatibility checks are made.
		 * Is your DB change still compatible with previous versions?
		 */
		အကယ်၍(db_version != MOSQ_DB_VERSION){
			အကယ်၍(db_version == 5){
				/* Addition of username and listener_port to client chunk in v6 */
			}မဟုတ်ပါက အကယ်၍(db_version == 4){
			}မဟုတ်ပါက အကယ်၍(db_version == 3){
				/* Addition of source_username and source_port to msg_store chunk in v4, v1.5.6 */
			}မဟုတ်ပါက အကယ်၍(db_version == 2){
				/* Addition of disconnect_t to client chunk in v3. */
			}မဟုတ်ပါက{
				fclose(fptr);
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Unsupported persistent database format version %d (need version %d).", db_version, MOSQ_DB_VERSION);
				ပြန်ပေး MOSQ_ERR_INVAL;
			}
		}

		နေစဉ်(persist__chunk_header_read(fptr, &chunk, &length) == MOSQ_ERR_SUCCESS){
			switch(chunk){
				pak_sa DB_CHUNK_CFG:
					အကယ်၍(db_version == 6 || db_version == 5){
						rc = persist__chunk_cfg_read_v56(fptr, &cfg_chunk);
						အကယ်၍(rc){
							fclose(fptr);
							ပြန်ပေး rc;
						}
					}မဟုတ်ပါက{
						rc = persist__chunk_cfg_read_v234(fptr, &cfg_chunk);
						အကယ်၍(rc){
							fclose(fptr);
							ပြန်ပေး rc;
						}
					}
					အကယ်၍(cfg_chunk.dbid_size != parimaa_na(dbid_t)){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Incompatible database configuration (dbid size is %d bytes, expected %lu)",
								cfg_chunk.dbid_size, (unsigned long)parimaa_na(dbid_t));
						fclose(fptr);
						ပြန်ပေး MOSQ_ERR_INVAL;
					}
					db.last_db_id = cfg_chunk.last_db_id;
					bha_nga;

				pak_sa DB_CHUNK_BASE_MSG:
					rc = persist__base_msg_chunk_restore(fptr, length);
					အကယ်၍(rc){
						fclose(fptr);
						ပြန်ပေး rc;
					}
					bha_nga;

				pak_sa DB_CHUNK_CLIENT_MSG:
					rc = persist__client_msg_chunk_restore(fptr, length);
					အကယ်၍(rc){
						fclose(fptr);
						ပြန်ပေး rc;
					}
					bha_nga;

				pak_sa DB_CHUNK_RETAIN:
					rc = persist__retain_chunk_restore(fptr);
					အကယ်၍(rc){
						fclose(fptr);
						ပြန်ပေး rc;
					}
					bha_nga;

				pak_sa DB_CHUNK_SUB:
					rc = persist__sub_chunk_restore(fptr);
					အကယ်၍(rc){
						fclose(fptr);
						ပြန်ပေး rc;
					}
					bha_nga;

				pak_sa DB_CHUNK_CLIENT:
					rc = persist__client_chunk_restore(fptr);
					အကယ်၍(rc){
						fclose(fptr);
						ပြန်ပေး rc;
					}
					bha_nga;

				default:
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Unsupported chunk \"%d\" in persistent database file. Ignoring.", chunk);
					အကယ်၍(fseek(fptr, length, SEEK_CUR) < 0){
						fclose(fptr);
						ပြန်ပေး MOSQ_ERR_INVAL;
					}
					bha_nga;
			}
		}
	}မဟုတ်ပါက{
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to restore persistent database. Unrecognised file format.");
		rc = 1;
	}

	fclose(fptr);

	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld base messages", base_msg_count);
	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld retained messages", retained_count);
	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld clients", client_count);
	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld subscriptions", subscription_count);
	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld client messages", client_msg_count);

	ပြန်ပေး rc;
error:
	err = strerror(errno);
	log__printf(NULL, MOSQ_LOG_ERR, "Error: %s.", err);
	အကယ်၍(fptr){
		fclose(fptr);
	}
	ပြန်ပေး MOSQ_ERR_ERRNO;
}


static ကိန်းပြည့် persist__restore_sub(const sa_nracanaa mosquitto_subscription *sub)
{
	sa_nracanaa mosquitto *context;

	အကယ်၍(!sub->clientid){
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no client id, ignoring.");
		ပြန်ပေး MOSQ_ERR_SUCCESS;
	}
	အကယ်၍(!sub->topic_filter){
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no topic filter, ignoring.");
		ပြန်ပေး MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(sub->clientid, 0);
	အကယ်၍(!context){
		ပြန်ပေး MOSQ_ERR_INVAL;
	}
	ပြန်ပေး sub__add(context, sub);
}

#endif
