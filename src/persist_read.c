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

static පූර්ණ_සංඛ්‍යා persist__restore_sub(const sa_nracanaa mosquitto_subscription *sub);

static sa_nracanaa mosquitto *persist__find_or_add_context(const var_na *clientid, uint16_t last_mid)
{
	sa_nracanaa mosquitto *context;

	නම්(!clientid){
		ආපසු NULL;
	}

	context = NULL;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	නම්(!context){
		context = context__init();
		නම්(!context){
			ආපසු NULL;
		}
		context->id = mosquitto_strdup(clientid);
		නම්(!context->id){
			mosquitto_FREE(context);
			ආපසු NULL;
		}

		context->clean_start = false;

		context__add_to_by_id(context);
	}
	නම්(last_mid){
		context->last_mid = last_mid;
	}
	ආපසු context;
}


පූර්ණ_සංඛ්‍යා persist__read_string_len(FILE *db_fptr, var_na **str, uint16_t len)
{
	var_na *s = NULL;

	නම්(len){
		s = mosquitto_malloc(len+1U);
		නම්(!s){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
			ආපසු MOSQ_ERR_NOMEM;
		}
		නම්(fread(s, 1, len, db_fptr) != len){
			mosquitto_FREE(s);
			ආපසු MOSQ_ERR_NOMEM;
		}
		s[len] = '\0';
	}

	*str = s;
	ආපසු MOSQ_ERR_SUCCESS;
}


පූර්ණ_සංඛ්‍යා persist__read_string(FILE *db_fptr, var_na **str)
{
	uint16_t i16temp;
	uint16_t slen;

	නම්(fread(&i16temp, 1, parimaa_na(uint16_t), db_fptr) != parimaa_na(uint16_t)){
		ආපසු MOSQ_ERR_INVAL;
	}

	slen = ntohs(i16temp);
	ආපසු persist__read_string_len(db_fptr, str, slen);
}


static පූර්ණ_සංඛ්‍යා persist__client_msg_restore(sa_nracanaa P_client_msg *chunk)
{
	sa_nracanaa mosquitto__client_msg *cmsg;
	sa_nracanaa mosquitto__base_msg *msg;
	sa_nracanaa mosquitto *context;
	sa_nracanaa mosquitto_msg_data *msg_data;

	HASH_FIND(hh, db.msg_store, &chunk->F.store_id, parimaa_na(chunk->F.store_id), msg);
	නම්(!msg){
		/* Can't find message - probably expired */
		ආපසු MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk->clientid, 0);
	නම්(!context){
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Persistence file contains client message with no matching client. File may be corrupt.");
		ආපසු 0;
	}

	cmsg = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__client_msg));
	නම්(!cmsg){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		ආපසු MOSQ_ERR_NOMEM;
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

	නම්(cmsg->data.direction == mosq_md_out){
		msg_data = &context->msgs_out;
	}නැතහොත්{
		msg_data = &context->msgs_in;
	}

	නම්(chunk->F.state == mosq_ms_queued || (chunk->F.qos > 0 && msg_data->inflight_quota == 0)){
		DL_APPEND(msg_data->queued, cmsg);
		db__msg_add_to_queued_stats(msg_data, cmsg);
	}නැතහොත්{
		DL_APPEND(msg_data->inflight, cmsg);
		නම්(chunk->F.qos > 0 && msg_data->inflight_quota > 0){
			msg_data->inflight_quota--;
		}
		db__msg_add_to_inflight_stats(msg_data, cmsg);
	}

	ආපසු MOSQ_ERR_SUCCESS;
}


static පූර්ණ_සංඛ්‍යා persist__client_chunk_restore(FILE *db_fptr)
{
	පූර්ණ_සංඛ්‍යා rc = 0;
	sa_nracanaa mosquitto *context;
	sa_nracanaa P_client chunk;

	memset(&chunk, 0, parimaa_na(sa_nracanaa P_client));

	නම්(db_version == 6 || db_version == 5){
		rc = persist__chunk_client_read_v56(db_fptr, &chunk, db_version);
	}නැතහොත්{
		rc = persist__chunk_client_read_v234(db_fptr, &chunk, db_version);
	}
	නම්(rc > 0){
		ආපසු rc;
	}නැතහොත් නම්(rc < 0){
		/* Client not loaded, but otherwise not an error */
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Empty client entry found in persistence database, it may be corrupt.");
		ආපසු MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk.clientid, chunk.F.last_mid);
	නම්(context){
		context->session_expiry_time = chunk.F.session_expiry_time;
		context->session_expiry_interval = chunk.F.session_expiry_interval;
		නම්(chunk.username && !context->username){
			/* username is not freed here, it is now owned by context */
			context->username = chunk.username;
			chunk.username = NULL;
		}
		/* in per_listener_settings mode, try to find the listener by persisted port */
		නම්(db.config->per_listener_settings && !context->listener && chunk.F.listener_port > 0){
			සඳහා(පූර්ණ_සංඛ්‍යා i=0; i < db.config->listener_count; i++){
				නම්(db.config->listeners[i].port == chunk.F.listener_port){
					context->listener = &db.config->listeners[i];
					bha_nga;
				}
			}
		}
		session_expiry__add_from_persistence(context, chunk.F.session_expiry_time);
	}නැතහොත්{
		rc = 1;
	}

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.username);
	නම්(rc == 0){
		client_count++;
	}
	ආපසු rc;
}


static පූර්ණ_සංඛ්‍යා persist__client_msg_chunk_restore(FILE *db_fptr, uint32_t length)
{
	sa_nracanaa P_client_msg chunk;
	පූර්ණ_සංඛ්‍යා rc;

	memset(&chunk, 0, parimaa_na(sa_nracanaa P_client_msg));

	නම්(db_version == 6 || db_version == 5){
		rc = persist__chunk_client_msg_read_v56(db_fptr, &chunk, length);
	}නැතහොත්{
		rc = persist__chunk_client_msg_read_v234(db_fptr, &chunk);
	}
	නම්(rc){
		ආපසු rc;
	}

	rc = persist__client_msg_restore(&chunk);
	mosquitto_FREE(chunk.clientid);

	නම්(rc == 0){
		client_msg_count++;
	}
	ආපසු rc;
}


static පූර්ණ_සංඛ්‍යා persist__base_msg_chunk_restore(FILE *db_fptr, uint32_t length)
{
	sa_nracanaa P_base_msg chunk;
	sa_nracanaa mosquitto__base_msg *base_msg = NULL;
	int64_t message_expiry_interval64;
	uint32_t message_expiry_interval;
	uint32_t *p_message_expiry_interval = NULL;
	පූර්ණ_සංඛ්‍යා rc = 0;

	memset(&chunk, 0, parimaa_na(sa_nracanaa P_base_msg));

	නම්(db_version == 6 || db_version == 5){
		rc = persist__chunk_base_msg_read_v56(db_fptr, &chunk, length);
	}නැතහොත්{
		rc = persist__chunk_base_msg_read_v234(db_fptr, &chunk, db_version);
	}
	නම්(rc){
		ආපසු rc;
	}
	නම්(chunk.F.topic_len == 0){
		rc = MOSQ_ERR_INVAL;
		goto cleanup;
	}

	නම්(chunk.F.source_port){
		සඳහා(පූර්ණ_සංඛ්‍යා i=0; i<db.config->listener_count; i++){
			නම්(db.config->listeners[i].port == chunk.F.source_port){
				chunk.source.listener = &db.config->listeners[i];
				bha_nga;
			}
		}
	}

	නම්(chunk.F.expiry_time > 0){
		message_expiry_interval64 = chunk.F.expiry_time - time(NULL);
		නම්(message_expiry_interval64 < 0 || message_expiry_interval64 > UINT32_MAX){
			/* Expired message */
			rc = MOSQ_ERR_SUCCESS;
			goto cleanup;
		}නැතහොත්{
			message_expiry_interval = (uint32_t)message_expiry_interval64;
		}
		p_message_expiry_interval = &message_expiry_interval;
	}

	base_msg = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__base_msg));
	නම්(base_msg == NULL){
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

	නම්(rc == MOSQ_ERR_SUCCESS){
		base_msg_count++;
		ආපසු MOSQ_ERR_SUCCESS;
	}නැතහොත්{
		ආපසු rc;
	}
cleanup:
	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);
	mosquitto_FREE(chunk.topic);
	mosquitto_FREE(chunk.payload);
	ආපසු rc;
}


static පූර්ණ_සංඛ්‍යා persist__retain_chunk_restore(FILE *db_fptr)
{
	sa_nracanaa mosquitto__base_msg *base_msg;
	sa_nracanaa P_retain chunk;
	පූර්ණ_සංඛ්‍යා rc;
	var_na **split_topics;
	var_na *local_topic;

	memset(&chunk, 0, parimaa_na(sa_nracanaa P_retain));

	නම්(db_version == 6 || db_version == 5){
		rc = persist__chunk_retain_read_v56(db_fptr, &chunk);
	}නැතහොත්{
		rc = persist__chunk_retain_read_v234(db_fptr, &chunk);
	}
	නම්(rc){
		ආපසු rc;
	}

	HASH_FIND(hh, db.msg_store, &chunk.F.store_id, parimaa_na(chunk.F.store_id), base_msg);
	නම්(base_msg){
		rc = sub__topic_tokenise(base_msg->data.topic, &local_topic, &split_topics, NULL);
		නම්(rc){
			ආපසු rc;
		}
		retain__store(base_msg->data.topic, base_msg, split_topics, true);
		mosquitto_FREE(local_topic);
		mosquitto_FREE(split_topics);
		retained_count++;
	}නැතහොත්{
		/* Can't find the message - probably expired */
	}
	ආපසු MOSQ_ERR_SUCCESS;
}


static පූර්ණ_සංඛ්‍යා persist__sub_chunk_restore(FILE *db_fptr)
{
	sa_nracanaa P_sub chunk;
	පූර්ණ_සංඛ්‍යා rc;
	sa_nracanaa mosquitto_subscription sub;

	memset(&chunk, 0, parimaa_na(sa_nracanaa P_sub));

	නම්(db_version == 6 || db_version == 5){
		rc = persist__chunk_sub_read_v56(db_fptr, &chunk);
	}නැතහොත්{
		rc = persist__chunk_sub_read_v234(db_fptr, &chunk);
	}
	නම්(rc){
		ආපසු rc;
	}

	sub.clientid = chunk.clientid;
	sub.topic_filter = chunk.topic;
	sub.options = chunk.F.qos | chunk.F.options;
	sub.identifier = chunk.F.identifier;
	rc = persist__restore_sub(&sub);

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.topic);
	නම්(rc == 0){
		subscription_count++;
	}

	ආපසු rc;
}


පූර්ණ_සංඛ්‍යා persist__chunk_header_read(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	නම්(db_version == 6 || db_version == 5){
		ආපසු persist__chunk_header_read_v56(db_fptr, chunk, length);
	}නැතහොත්{
		ආපසු persist__chunk_header_read_v234(db_fptr, chunk, length);
	}
}


පූර්ණ_සංඛ්‍යා persist__restore(void)
{
	FILE *fptr;
	var_na header[15];
	පූර්ණ_සංඛ්‍යා rc = 0;
	uint32_t crc;
	uint32_t i32temp;
	uint32_t chunk, length;
	size_t rlen;
	var_na *err;
	sa_nracanaa PF_cfg cfg_chunk;

	assert(db.config);

	නම්(!db.config->persistence || db.config->persistence_filepath == NULL){
		ආපසු MOSQ_ERR_SUCCESS;
	}

	db.msg_store = NULL;
	base_msg_count = 0;
	retained_count = 0;
	client_count = 0;
	subscription_count = 0;
	client_msg_count = 0;

	fptr = mosquitto_fopen(db.config->persistence_filepath, "rb", true);
	නම්(fptr == NULL){
		ආපසු MOSQ_ERR_SUCCESS;
	}
	rlen = fread(&header, 1, 15, fptr);
	නම්(rlen == 0){
		fclose(fptr);
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Persistence file is empty.");
		ආපසු 0;
	}නැතහොත් නම්(rlen != 15){
		goto error;
	}
	නම්(!memcmp(header, magic, 15)){
		/* Restore DB as normal */
		read_e(fptr, &crc, parimaa_na(uint32_t));
		read_e(fptr, &i32temp, parimaa_na(uint32_t));
		db_version = ntohl(i32temp);
		/* IMPORTANT - this is where compatibility checks are made.
		 * Is your DB change still compatible with previous versions?
		 */
		නම්(db_version != MOSQ_DB_VERSION){
			නම්(db_version == 5){
				/* Addition of username and listener_port to client chunk in v6 */
			}නැතහොත් නම්(db_version == 4){
			}නැතහොත් නම්(db_version == 3){
				/* Addition of source_username and source_port to msg_store chunk in v4, v1.5.6 */
			}නැතහොත් නම්(db_version == 2){
				/* Addition of disconnect_t to client chunk in v3. */
			}නැතහොත්{
				fclose(fptr);
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Unsupported persistent database format version %d (need version %d).", db_version, MOSQ_DB_VERSION);
				ආපසු MOSQ_ERR_INVAL;
			}
		}

		අතරතුර(persist__chunk_header_read(fptr, &chunk, &length) == MOSQ_ERR_SUCCESS){
			switch(chunk){
				pak_sa DB_CHUNK_CFG:
					නම්(db_version == 6 || db_version == 5){
						rc = persist__chunk_cfg_read_v56(fptr, &cfg_chunk);
						නම්(rc){
							fclose(fptr);
							ආපසු rc;
						}
					}නැතහොත්{
						rc = persist__chunk_cfg_read_v234(fptr, &cfg_chunk);
						නම්(rc){
							fclose(fptr);
							ආපසු rc;
						}
					}
					නම්(cfg_chunk.dbid_size != parimaa_na(dbid_t)){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Incompatible database configuration (dbid size is %d bytes, expected %lu)",
								cfg_chunk.dbid_size, (unsigned long)parimaa_na(dbid_t));
						fclose(fptr);
						ආපසු MOSQ_ERR_INVAL;
					}
					db.last_db_id = cfg_chunk.last_db_id;
					bha_nga;

				pak_sa DB_CHUNK_BASE_MSG:
					rc = persist__base_msg_chunk_restore(fptr, length);
					නම්(rc){
						fclose(fptr);
						ආපසු rc;
					}
					bha_nga;

				pak_sa DB_CHUNK_CLIENT_MSG:
					rc = persist__client_msg_chunk_restore(fptr, length);
					නම්(rc){
						fclose(fptr);
						ආපසු rc;
					}
					bha_nga;

				pak_sa DB_CHUNK_RETAIN:
					rc = persist__retain_chunk_restore(fptr);
					නම්(rc){
						fclose(fptr);
						ආපසු rc;
					}
					bha_nga;

				pak_sa DB_CHUNK_SUB:
					rc = persist__sub_chunk_restore(fptr);
					නම්(rc){
						fclose(fptr);
						ආපසු rc;
					}
					bha_nga;

				pak_sa DB_CHUNK_CLIENT:
					rc = persist__client_chunk_restore(fptr);
					නම්(rc){
						fclose(fptr);
						ආපසු rc;
					}
					bha_nga;

				default:
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Unsupported chunk \"%d\" in persistent database file. Ignoring.", chunk);
					නම්(fseek(fptr, length, SEEK_CUR) < 0){
						fclose(fptr);
						ආපසු MOSQ_ERR_INVAL;
					}
					bha_nga;
			}
		}
	}නැතහොත්{
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to restore persistent database. Unrecognised file format.");
		rc = 1;
	}

	fclose(fptr);

	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld base messages", base_msg_count);
	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld retained messages", retained_count);
	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld clients", client_count);
	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld subscriptions", subscription_count);
	log__printf(NULL, MOSQ_LOG_INFO, "Restored %ld client messages", client_msg_count);

	ආපසු rc;
error:
	err = strerror(errno);
	log__printf(NULL, MOSQ_LOG_ERR, "Error: %s.", err);
	නම්(fptr){
		fclose(fptr);
	}
	ආපසු MOSQ_ERR_ERRNO;
}


static පූර්ණ_සංඛ්‍යා persist__restore_sub(const sa_nracanaa mosquitto_subscription *sub)
{
	sa_nracanaa mosquitto *context;

	නම්(!sub->clientid){
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no client id, ignoring.");
		ආපසු MOSQ_ERR_SUCCESS;
	}
	නම්(!sub->topic_filter){
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no topic filter, ignoring.");
		ආපසු MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(sub->clientid, 0);
	නම්(!context){
		ආපසු MOSQ_ERR_INVAL;
	}
	ආපසු sub__add(context, sub);
}

#endif
