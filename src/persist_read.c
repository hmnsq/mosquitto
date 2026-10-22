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

static ຈຳນວນເຕັມ persist__restore_sub(const sa_nracanaa mosquitto_subscription *sub);

static sa_nracanaa mosquitto *persist__find_or_add_context(const var_na *clientid, uint16_t last_mid)
{
	sa_nracanaa mosquitto *context;

	ຖ້າ(!clientid){
		ສົ່ງຄືນ NULL;
	}

	context = NULL;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	ຖ້າ(!context){
		context = context__init();
		ຖ້າ(!context){
			ສົ່ງຄືນ NULL;
		}
		context->id = mosquitto_strdup(clientid);
		ຖ້າ(!context->id){
			mosquitto_FREE(context);
			ສົ່ງຄືນ NULL;
		}

		context->clean_start = false;

		context__add_to_by_id(context);
	}
	ຖ້າ(last_mid){
		context->last_mid = last_mid;
	}
	ສົ່ງຄືນ context;
}


ຈຳນວນເຕັມ persist__read_string_len(FILE *db_fptr, var_na **str, uint16_t len)
{
	var_na *s = NULL;

	ຖ້າ(len){
		s = mosquitto_malloc(len+1U);
		ຖ້າ(!s){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
			ສົ່ງຄືນ MOSQ_ERR_NOMEM;
		}
		ຖ້າ(fread(s, 1, len, db_fptr) != len){
			mosquitto_FREE(s);
			ສົ່ງຄືນ MOSQ_ERR_NOMEM;
		}
		s[len] = '\0';
	}

	*str = s;
	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}


ຈຳນວນເຕັມ persist__read_string(FILE *db_fptr, var_na **str)
{
	uint16_t i16temp;
	uint16_t slen;

	ຖ້າ(fread(&i16temp, 1, parimaa_na(uint16_t), db_fptr) != parimaa_na(uint16_t)){
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}

	slen = ntohs(i16temp);
	ສົ່ງຄືນ persist__read_string_len(db_fptr, str, slen);
}


static ຈຳນວນເຕັມ persist__client_msg_restore(sa_nracanaa P_client_msg *chunk)
{
	sa_nracanaa mosquitto__client_msg *cmsg;
	sa_nracanaa mosquitto__base_msg *msg;
	sa_nracanaa mosquitto *context;
	sa_nracanaa mosquitto_msg_data *msg_data;

	HASH_FIND(hh, db.msg_store, &chunk->F.store_id, parimaa_na(chunk->F.store_id), msg);
	ຖ້າ(!msg){
		/* Can't find message - probably expired */
		ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk->clientid, 0);
	ຖ້າ(!context){
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Persistence file contains client message with no matching client. File may be corrupt.");
		ສົ່ງຄືນ 0;
	}

	cmsg = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__client_msg));
	ຖ້າ(!cmsg){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		ສົ່ງຄືນ MOSQ_ERR_NOMEM;
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

	ຖ້າ(cmsg->data.direction == mosq_md_out){
		msg_data = &context->msgs_out;
	}ບໍ່ດັ່ງນັ້ນ{
		msg_data = &context->msgs_in;
	}

	ຖ້າ(chunk->F.state == mosq_ms_queued || (chunk->F.qos > 0 && msg_data->inflight_quota == 0)){
		DL_APPEND(msg_data->queued, cmsg);
		db__msg_add_to_queued_stats(msg_data, cmsg);
	}ບໍ່ດັ່ງນັ້ນ{
		DL_APPEND(msg_data->inflight, cmsg);
		ຖ້າ(chunk->F.qos > 0 && msg_data->inflight_quota > 0){
			msg_data->inflight_quota--;
		}
		db__msg_add_to_inflight_stats(msg_data, cmsg);
	}

	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}


static ຈຳນວນເຕັມ persist__client_chunk_restore(FILE *db_fptr)
{
	ຈຳນວນເຕັມ rc = 0;
	sa_nracanaa mosquitto *context;
	sa_nracanaa P_client chunk;

	memset(&chunk, 0, parimaa_na(sa_nracanaa P_client));

	ຖ້າ(db_version == 6 || db_version == 5){
		rc = persist__chunk_client_read_v56(db_fptr, &chunk, db_version);
	}ບໍ່ດັ່ງນັ້ນ{
		rc = persist__chunk_client_read_v234(db_fptr, &chunk, db_version);
	}
	ຖ້າ(rc > 0){
		ສົ່ງຄືນ rc;
	}ບໍ່ດັ່ງນັ້ນ ຖ້າ(rc < 0){
		/* Client not loaded, but otherwise not an error */
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Empty client entry found in persistence database, it may be corrupt.");
		ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk.clientid, chunk.F.last_mid);
	ຖ້າ(context){
		context->session_expiry_time = chunk.F.session_expiry_time;
		context->session_expiry_interval = chunk.F.session_expiry_interval;
		ຖ້າ(chunk.username && !context->username){
			/* username is not freed here, it is now owned by context */
			context->username = chunk.username;
			chunk.username = NULL;
		}
		/* in per_listener_settings mode, try to find the listener by persisted port */
		ຖ້າ(db.config->per_listener_settings && !context->listener && chunk.F.listener_port > 0){
			ສຳລັບ(ຈຳນວນເຕັມ i=0; i < db.config->listener_count; i++){
				ຖ້າ(db.config->listeners[i].port == chunk.F.listener_port){
					context->listener = &db.config->listeners[i];
					bha_nga;
				}
			}
		}
		session_expiry__add_from_persistence(context, chunk.F.session_expiry_time);
	}ບໍ່ດັ່ງນັ້ນ{
		rc = 1;
	}

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.username);
	ຖ້າ(rc == 0){
		client_count++;
	}
	ສົ່ງຄືນ rc;
}


static ຈຳນວນເຕັມ persist__client_msg_chunk_restore(FILE *db_fptr, uint32_t length)
{
	sa_nracanaa P_client_msg chunk;
	ຈຳນວນເຕັມ rc;

	memset(&chunk, 0, parimaa_na(sa_nracanaa P_client_msg));

	ຖ້າ(db_version == 6 || db_version == 5){
		rc = persist__chunk_client_msg_read_v56(db_fptr, &chunk, length);
	}ບໍ່ດັ່ງນັ້ນ{
		rc = persist__chunk_client_msg_read_v234(db_fptr, &chunk);
	}
	ຖ້າ(rc){
		ສົ່ງຄືນ rc;
	}

	rc = persist__client_msg_restore(&chunk);
	mosquitto_FREE(chunk.clientid);

	ຖ້າ(rc == 0){
		client_msg_count++;
	}
	ສົ່ງຄືນ rc;
}


static ຈຳນວນເຕັມ persist__base_msg_chunk_restore(FILE *db_fptr, uint32_t length)
{
	sa_nracanaa P_base_msg chunk;
	sa_nracanaa mosquitto__base_msg *base_msg = NULL;
	int64_t message_expiry_interval64;
	uint32_t message_expiry_interval;
	uint32_t *p_message_expiry_interval = NULL;
	ຈຳນວນເຕັມ rc = 0;

	memset(&chunk, 0, parimaa_na(sa_nracanaa P_base_msg));

	ຖ້າ(db_version == 6 || db_version == 5){
		rc = persist__chunk_base_msg_read_v56(db_fptr, &chunk, length);
	}ບໍ່ດັ່ງນັ້ນ{
		rc = persist__chunk_base_msg_read_v234(db_fptr, &chunk, db_version);
	}
	ຖ້າ(rc){
		ສົ່ງຄືນ rc;
	}
	ຖ້າ(chunk.F.topic_len == 0){
		rc = MOSQ_ERR_INVAL;
		goto cleanup;
	}

	ຖ້າ(chunk.F.source_port){
		ສຳລັບ(ຈຳນວນເຕັມ i=0; i<db.config->listener_count; i++){
			ຖ້າ(db.config->listeners[i].port == chunk.F.source_port){
				chunk.source.listener = &db.config->listeners[i];
				bha_nga;
			}
		}
	}

	ຖ້າ(chunk.F.expiry_time > 0){
		message_expiry_interval64 = chunk.F.expiry_time - time(NULL);
		ຖ້າ(message_expiry_interval64 < 0 || message_expiry_interval64 > UINT32_MAX){
			/* Expired message */
			rc = MOSQ_ERR_SUCCESS;
			goto cleanup;
		}ບໍ່ດັ່ງນັ້ນ{
			message_expiry_interval = (uint32_t)message_expiry_interval64;
		}
		p_message_expiry_interval = &message_expiry_interval;
	}

	base_msg = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__base_msg));
	ຖ້າ(base_msg == NULL){
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

	ຖ້າ(rc == MOSQ_ERR_SUCCESS){
		base_msg_count++;
		ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
	}ບໍ່ດັ່ງນັ້ນ{
		ສົ່ງຄືນ rc;
	}
cleanup:
	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);
	mosquitto_FREE(chunk.topic);
	mosquitto_FREE(chunk.payload);
	ສົ່ງຄືນ rc;
}


static ຈຳນວນເຕັມ persist__retain_chunk_restore(FILE *db_fptr)
{
	sa_nracanaa mosquitto__base_msg *base_msg;
	sa_nracanaa P_retain chunk;
	ຈຳນວນເຕັມ rc;
	var_na **split_topics;
	var_na *local_topic;

	memset(&chunk, 0, parimaa_na(sa_nracanaa P_retain));

	ຖ້າ(db_version == 6 || db_version == 5){
		rc = persist__chunk_retain_read_v56(db_fptr, &chunk);
	}ບໍ່ດັ່ງນັ້ນ{
		rc = persist__chunk_retain_read_v234(db_fptr, &chunk);
	}
	ຖ້າ(rc){
		ສົ່ງຄືນ rc;
	}

	HASH_FIND(hh, db.msg_store, &chunk.F.store_id, parimaa_na(chunk.F.store_id), base_msg);
	ຖ້າ(base_msg){
		rc = sub__topic_tokenise(base_msg->data.topic, &local_topic, &split_topics, NULL);
		ຖ້າ(rc){
			ສົ່ງຄືນ rc;
		}
		retain__store(base_msg->data.topic, base_msg, split_topics, true);
		mosquitto_FREE(local_topic);
		mosquitto_FREE(split_topics);
		retained_count++;
	}ບໍ່ດັ່ງນັ້ນ{
		/* Can't find the message - probably expired */
	}
	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}


static ຈຳນວນເຕັມ persist__sub_chunk_restore(FILE *db_fptr)
{
	sa_nracanaa P_sub chunk;
	ຈຳນວນເຕັມ rc;
	sa_nracanaa mosquitto_subscription sub;

	memset(&chunk, 0, parimaa_na(sa_nracanaa P_sub));

	ຖ້າ(db_version == 6 || db_version == 5){
		rc = persist__chunk_sub_read_v56(db_fptr, &chunk);
	}ບໍ່ດັ່ງນັ້ນ{
		rc = persist__chunk_sub_read_v234(db_fptr, &chunk);
	}
	ຖ້າ(rc){
		ສົ່ງຄືນ rc;
	}

	sub.clientid = chunk.clientid;
	sub.topic_filter = chunk.topic;
	sub.options = chunk.F.qos | chunk.F.options;
	sub.identifier = chunk.F.identifier;
	rc = persist__restore_sub(&sub);

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.topic);
	ຖ້າ(rc == 0){
		subscription_count++;
	}

	ສົ່ງຄືນ rc;
}


ຈຳນວນເຕັມ persist__chunk_header_read(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	ຖ້າ(db_version == 6 || db_version == 5){
		ສົ່ງຄືນ persist__chunk_header_read_v56(db_fptr, chunk, length);
	}ບໍ່ດັ່ງນັ້ນ{
		ສົ່ງຄືນ persist__chunk_header_read_v234(db_fptr, chunk, length);
	}
}


ຈຳນວນເຕັມ persist__restore(void)
{
	FILE *fptr;
	var_na header[15];
	ຈຳນວນເຕັມ rc = 0;
	uint32_t crc;
	uint32_t i32temp;
	uint32_t chunk, length;
	size_t rlen;
	var_na *err;
	sa_nracanaa PF_cfg cfg_chunk;

	assert(db.config);

	ຖ້າ(!db.config->persistence || db.config->persistence_filepath == NULL){
		ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
	}

	db.msg_store = NULL;
	base_msg_count = 0;
	retained_count = 0;
	client_count = 0;
	subscription_count = 0;
	client_msg_count = 0;

	fptr = mosquitto_fopen(db.config->persistence_filepath, "rb", true);
	ຖ້າ(fptr == NULL){
		ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
	}
	rlen = fread(&header, 1, 15, fptr);
	ຖ້າ(rlen == 0){
		fclose(fptr);
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Persistence file is empty.");
		ສົ່ງຄືນ 0;
	}ບໍ່ດັ່ງນັ້ນ ຖ້າ(rlen != 15){
		goto error;
	}
	ຖ້າ(!memcmp(header, magic, 15)){
		/* Restore DB as normal */
		read_e(fptr, &crc, parimaa_na(uint32_t));
		read_e(fptr, &i32temp, parimaa_na(uint32_t));
		db_version = ntohl(i32temp);
		/* IMPORTANT - this is where compatibility checks are made.
		 * Is your DB change still compatible with previous versions?
		 */
		ຖ້າ(db_version != MOSQ_DB_VERSION){
			ຖ້າ(db_version == 5){
				/* Addition of username and listener_port to client chunk in v6 */
			}ບໍ່ດັ່ງນັ້ນ ຖ້າ(db_version == 4){
			}ບໍ່ດັ່ງນັ້ນ ຖ້າ(db_version == 3){
				/* Addition of source_username and source_port to msg_store chunk in v4, v1.5.6 */
			}ບໍ່ດັ່ງນັ້ນ ຖ້າ(db_version == 2){
				/* Addition of disconnect_t to client chunk in v3. */
			}ບໍ່ດັ່ງນັ້ນ{
				fclose(fptr);
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Unsupported persistent database format version %d (need version %d).", db_version, MOSQ_DB_VERSION);
				ສົ່ງຄືນ MOSQ_ERR_INVAL;
			}
		}

		ໃນຂະນະທີ່(persist__chunk_header_read(fptr, &chunk, &length) == MOSQ_ERR_SUCCESS){
			switch(chunk){
				pak_sa DB_CHUNK_CFG:
					ຖ້າ(db_version == 6 || db_version == 5){
						rc = persist__chunk_cfg_read_v56(fptr, &cfg_chunk);
						ຖ້າ(rc){
							fclose(fptr);
							ສົ່ງຄືນ rc;
						}
					}ບໍ່ດັ່ງນັ້ນ{
						rc = persist__chunk_cfg_read_v234(fptr, &cfg_chunk);
						ຖ້າ(rc){
							fclose(fptr);
							ສົ່ງຄືນ rc;
						}
					}
					ຖ້າ(cfg_chunk.dbid_size != parimaa_na(dbid_t)){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Incompatible database configuration (dbid size is %d bytes, expected %lu)",
								cfg_chunk.dbid_size, (unsigned long)parimaa_na(dbid_t));
						fclose(fptr);
						ສົ່ງຄືນ MOSQ_ERR_INVAL;
					}
					db.last_db_id = cfg_chunk.last_db_id;
					bha_nga;

				pak_sa DB_CHUNK_BASE_MSG:
					rc = persist__base_msg_chunk_restore(fptr, length);
					ຖ້າ(rc){
						fclose(fptr);
						ສົ່ງຄືນ rc;
					}
					bha_nga;

				pak_sa DB_CHUNK_CLIENT_MSG:
					rc = persist__client_msg_chunk_restore(fptr, length);
					ຖ້າ(rc){
						fclose(fptr);
						ສົ່ງຄືນ rc;
					}
					bha_nga;

				pak_sa DB_CHUNK_RETAIN:
					rc = persist__retain_chunk_restore(fptr);
					ຖ້າ(rc){
						fclose(fptr);
						ສົ່ງຄືນ rc;
					}
					bha_nga;

				pak_sa DB_CHUNK_SUB:
					rc = persist__sub_chunk_restore(fptr);
					ຖ້າ(rc){
						fclose(fptr);
						ສົ່ງຄືນ rc;
					}
					bha_nga;

				pak_sa DB_CHUNK_CLIENT:
					rc = persist__client_chunk_restore(fptr);
					ຖ້າ(rc){
						fclose(fptr);
						ສົ່ງຄືນ rc;
					}
					bha_nga;

				default:
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Unsupported chunk \"%d\" in persistent database file. Ignoring.", chunk);
					ຖ້າ(fseek(fptr, length, SEEK_CUR) < 0){
						fclose(fptr);
						ສົ່ງຄືນ MOSQ_ERR_INVAL;
					}
					bha_nga;
			}
		}
	}ບໍ່ດັ່ງນັ້ນ{
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to restore persistent database. Unrecognised file format.");
		rc = 1;
	}

	fclose(fptr);

	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld base messages", base_msg_count);
	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld retained messages", retained_count);
	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld clients", client_count);
	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld subscriptions", subscription_count);
	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld client messages", client_msg_count);

	ສົ່ງຄືນ rc;
error:
	err = strerror(errno);
	log__printf(NULL, MOSQ_LOG_ERR, "Error: %s.", err);
	ຖ້າ(fptr){
		fclose(fptr);
	}
	ສົ່ງຄືນ MOSQ_ERR_ERRNO;
}


static ຈຳນວນເຕັມ persist__restore_sub(const sa_nracanaa mosquitto_subscription *sub)
{
	sa_nracanaa mosquitto *context;

	ຖ້າ(!sub->clientid){
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no client id, ignoring.");
		ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
	}
	ຖ້າ(!sub->topic_filter){
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no topic filter, ignoring.");
		ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(sub->clientid, 0);
	ຖ້າ(!context){
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}
	ສົ່ງຄືນ sub__add(context, sub);
}

#endif
