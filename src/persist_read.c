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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif
#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <errno.h>
#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>
#ଅନ୍ତର୍ଭାଵ <time.h>
#ଅନ୍ତର୍ଭାଵ <utlist.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "persist.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"

uint32_t db_version;

ଅଚଳ unsigned ଅକ୍ଷର magic[15] = {0x00, 0xB5, 0x00, 'm', 'o', 's', 'q', 'u', 'i', 't', 't', 'o', ' ', 'd', 'b'};
ସ୍ଥିର long base_ସନ୍ଦେଶ_ଗଣନା = 0;
ସ୍ଥିର long retained_ଗଣନା = 0;
ସ୍ଥିର long client_ଗଣନା = 0;
ସ୍ଥିର long subscription_ଗଣନା = 0;
ସ୍ଥିର long client_ସନ୍ଦେଶ_ଗଣନା = 0;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__restore_sub(ଅଚଳ ସଂରଚନା mosquitto_subscription *sub);

ସ୍ଥିର ସଂରଚନା mosquitto *persist__find_or_add_context(ଅଚଳ ଅକ୍ଷର *clientid, uint16_t last_mid)
{
	ସଂରଚନା mosquitto *context;

	ଯଦି(!clientid){
		ଫେରନ୍ତୁ ଖାଲି;
	}

	context = ଖାଲି;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	ଯଦି(!context){
		context = context__init();
		ଯଦି(!context){
			ଫେରନ୍ତୁ ଖାଲି;
		}
		context->id = mosquitto_strdup(clientid);
		ଯଦି(!context->id){
			mosquitto_FREE(context);
			ଫେରନ୍ତୁ ଖାଲି;
		}

		context->clean_start = false;

		context__add_to_by_id(context);
	}
	ଯଦି(last_mid){
		context->last_mid = last_mid;
	}
	ଫେରନ୍ତୁ context;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__read_string_len(FILE *db_fptr, ଅକ୍ଷର **str, uint16_t len)
{
	ଅକ୍ଷର *s = ଖାଲି;

	ଯଦି(len){
		s = mosquitto_malloc(len+1U);
		ଯଦି(!s){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}
		ଯଦି(fread(s, 1, len, db_fptr) != len){
			mosquitto_FREE(s);
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}
		s[len] = '\0';
	}

	*str = s;
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__read_string(FILE *db_fptr, ଅକ୍ଷର **str)
{
	uint16_t i16temp;
	uint16_t slen;

	ଯଦି(fread(&i16temp, 1, ଆକାର(uint16_t), db_fptr) != ଆକାର(uint16_t)){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	slen = ntohs(i16temp);
	ଫେରନ୍ତୁ persist__read_string_len(db_fptr, str, slen);
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist_client_ସନ୍ଦେଶ_restore(ସଂରଚନା P_client_ସନ୍ଦେଶ *chunk)
{
	ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *cmsg;
	ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *ସନ୍ଦେଶ;
	ସଂରଚନା mosquitto *context;
	ସଂରଚନା mosquitto_ସନ୍ଦେଶ_data *ସନ୍ଦେଶ_data;

	HASH_FIND(hh, db.ସନ୍ଦେଶ_store, &chunk->F.store_id, ଆକାର(chunk->F.store_id), ସନ୍ଦେଶ);
	ଯଦି(!ସନ୍ଦେଶ){
		/* Can't find message - probably expired */
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk->clientid, 0);
	ଯଦି(!context){
		log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Persistence file contains client message with no matching client. File may be corrupt.");
		ଫେରନ୍ତୁ 0;
	}

	cmsg = mosquitto_calloc(1, ଆକାର(ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ));
	ଯଦି(!cmsg){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	cmsg->next = ଖାଲି;
	cmsg->base_ସନ୍ଦେଶ = ଖାଲି;
	cmsg->data.cmsg_id = ++context->last_cmsg_id;
	cmsg->data.mid = chunk->F.mid;
	cmsg->data.qos = chunk->F.qos;
	cmsg->data.retain = (chunk->F.retain_dup&0xF0)>>4;
	cmsg->data.direction = chunk->F.direction;
	cmsg->data.state = chunk->F.state;
	cmsg->data.dup = chunk->F.retain_dup&0x0F;
	cmsg->data.subscription_identifier = chunk->subscription_identifier;

	cmsg->base_ସନ୍ଦେଶ = ସନ୍ଦେଶ;
	db_ସନ୍ଦେଶ_store_ref_inc(cmsg->base_ସନ୍ଦେଶ);

	ଯଦି(cmsg->data.direction == mosq_md_out){
		ସନ୍ଦେଶ_data = &context->msgs_out;
	}ଅନ୍ୟଥା{
		ସନ୍ଦେଶ_data = &context->msgs_in;
	}

	ଯଦି(chunk->F.state == mosq_ms_queued || (chunk->F.qos > 0 && ସନ୍ଦେଶ_data->inflight_quota == 0)){
		DL_APPEND(ସନ୍ଦେଶ_data->queued, cmsg);
		db_ସନ୍ଦେଶ_add_to_queued_stats(ସନ୍ଦେଶ_data, cmsg);
	}ଅନ୍ୟଥା{
		DL_APPEND(ସନ୍ଦେଶ_data->inflight, cmsg);
		ଯଦି(chunk->F.qos > 0 && ସନ୍ଦେଶ_data->inflight_quota > 0){
			ସନ୍ଦେଶ_data->inflight_quota--;
		}
		db_ସନ୍ଦେଶ_add_to_inflight_stats(ସନ୍ଦେଶ_data, cmsg);
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__client_chunk_restore(FILE *db_fptr)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;
	ସଂରଚନା mosquitto *context;
	ସଂରଚନା P_client chunk;

	memset(&chunk, 0, ଆକାର(ସଂରଚନା P_client));

	ଯଦି(db_version == 6 || db_version == 5){
		rc = persist__chunk_client_read_v56(db_fptr, &chunk, db_version);
	}ଅନ୍ୟଥା{
		rc = persist__chunk_client_read_v234(db_fptr, &chunk, db_version);
	}
	ଯଦି(rc > 0){
		ଫେରନ୍ତୁ rc;
	}ଅନ୍ୟଥା ଯଦି(rc < 0){
		/* Client not loaded, but otherwise not an error */
		log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Empty client entry found in persistence database, it may be corrupt.");
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk.clientid, chunk.F.last_mid);
	ଯଦି(context){
		context->session_expiry_time = chunk.F.session_expiry_time;
		context->session_expiry_interval = chunk.F.session_expiry_interval;
		ଯଦି(chunk.username && !context->username){
			/* username is not freed here, it is now owned by context */
			context->username = chunk.username;
			chunk.username = ଖାଲି;
		}
		/* in per_listener_settings mode, try to find the listener by persisted port */
		ଯଦି(db.config->per_listener_settings && !context->listener && chunk.F.listener_port > 0){
			ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i < db.config->listener_ଗଣନା; i++){
				ଯଦି(db.config->listeners[i].port == chunk.F.listener_port){
					context->listener = &db.config->listeners[i];
					ଭାଙ୍ଗ;
				}
			}
		}
		session_expiry__add_from_persistence(context, chunk.F.session_expiry_time);
	}ଅନ୍ୟଥା{
		rc = 1;
	}

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.username);
	ଯଦି(rc == 0){
		client_ଗଣନା++;
	}
	ଫେରନ୍ତୁ rc;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist_client_ସନ୍ଦେଶ_chunk_restore(FILE *db_fptr, uint32_t length)
{
	ସଂରଚନା P_client_ସନ୍ଦେଶ chunk;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	memset(&chunk, 0, ଆକାର(ସଂରଚନା P_client_ସନ୍ଦେଶ));

	ଯଦି(db_version == 6 || db_version == 5){
		rc = persist_chunk_client_ସନ୍ଦେଶ_read_v56(db_fptr, &chunk, length);
	}ଅନ୍ୟଥା{
		rc = persist_chunk_client_ସନ୍ଦେଶ_read_v234(db_fptr, &chunk);
	}
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}

	rc = persist_client_ସନ୍ଦେଶ_restore(&chunk);
	mosquitto_FREE(chunk.clientid);

	ଯଦି(rc == 0){
		client_ସନ୍ଦେଶ_ଗଣନା++;
	}
	ଫେରନ୍ତୁ rc;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist_base_ସନ୍ଦେଶ_chunk_restore(FILE *db_fptr, uint32_t length)
{
	ସଂରଚନା P_base_ସନ୍ଦେଶ chunk;
	ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ = ଖାଲି;
	int64_t message_expiry_interval64;
	uint32_t message_expiry_interval;
	uint32_t *p_message_expiry_interval = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;

	memset(&chunk, 0, ଆକାର(ସଂରଚନା P_base_ସନ୍ଦେଶ));

	ଯଦି(db_version == 6 || db_version == 5){
		rc = persist_chunk_base_ସନ୍ଦେଶ_read_v56(db_fptr, &chunk, length);
	}ଅନ୍ୟଥା{
		rc = persist_chunk_base_ସନ୍ଦେଶ_read_v234(db_fptr, &chunk, db_version);
	}
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}
	ଯଦି(chunk.F.topic_len == 0){
		rc = MOSQ_ERR_INVAL;
		ଯାଅ cleanup;
	}

	ଯଦି(chunk.F.source_port){
		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<db.config->listener_ଗଣନା; i++){
			ଯଦି(db.config->listeners[i].port == chunk.F.source_port){
				chunk.source.listener = &db.config->listeners[i];
				ଭାଙ୍ଗ;
			}
		}
	}

	ଯଦି(chunk.F.expiry_time > 0){
		message_expiry_interval64 = chunk.F.expiry_time - time(ଖାଲି);
		ଯଦି(message_expiry_interval64 < 0 || message_expiry_interval64 > UINT32_MAX){
			/* Expired message */
			rc = MOSQ_ERR_SUCCESS;
			ଯାଅ cleanup;
		}ଅନ୍ୟଥା{
			message_expiry_interval = (uint32_t)message_expiry_interval64;
		}
		p_message_expiry_interval = &message_expiry_interval;
	}

	base_ସନ୍ଦେଶ = mosquitto_calloc(1, ଆକାର(ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ));
	ଯଦି(base_ସନ୍ଦେଶ == ଖାଲି){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
		rc = MOSQ_ERR_NOMEM;
		ଯାଅ cleanup;
	}

	base_ସନ୍ଦେଶ->data.store_id = chunk.F.store_id;
	base_ସନ୍ଦେଶ->data.source_mid = chunk.F.source_mid;
	base_ସନ୍ଦେଶ->data.topic = chunk.topic;
	base_ସନ୍ଦେଶ->data.qos = chunk.F.qos;
	base_ସନ୍ଦେଶ->data.payloadlen = chunk.F.payloadlen;
	base_ସନ୍ଦେଶ->data.retain = chunk.F.retain;
	base_ସନ୍ଦେଶ->data.properties = chunk.properties;
	base_ସନ୍ଦେଶ->data.payload = chunk.payload;
	base_ସନ୍ଦେଶ->source_listener = chunk.source.listener;

	rc = db__message_store(&chunk.source, base_ସନ୍ଦେଶ, p_message_expiry_interval,
			mosq_mo_client);

	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);

	ଯଦି(rc == MOSQ_ERR_SUCCESS){
		base_ସନ୍ଦେଶ_ଗଣନା++;
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ rc;
	}
cleanup:
	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);
	mosquitto_FREE(chunk.topic);
	mosquitto_FREE(chunk.payload);
	ଫେରନ୍ତୁ rc;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__retain_chunk_restore(FILE *db_fptr)
{
	ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ;
	ସଂରଚନା P_retain chunk;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ଅକ୍ଷର **split_topics;
	ଅକ୍ଷର *local_topic;

	memset(&chunk, 0, ଆକାର(ସଂରଚନା P_retain));

	ଯଦି(db_version == 6 || db_version == 5){
		rc = persist__chunk_retain_read_v56(db_fptr, &chunk);
	}ଅନ୍ୟଥା{
		rc = persist__chunk_retain_read_v234(db_fptr, &chunk);
	}
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}

	HASH_FIND(hh, db.ସନ୍ଦେଶ_store, &chunk.F.store_id, ଆକାର(chunk.F.store_id), base_ସନ୍ଦେଶ);
	ଯଦି(base_ସନ୍ଦେଶ){
		rc = sub__topic_tokenise(base_ସନ୍ଦେଶ->data.topic, &local_topic, &split_topics, ଖାଲି);
		ଯଦି(rc){
			ଫେରନ୍ତୁ rc;
		}
		retain__store(base_ସନ୍ଦେଶ->data.topic, base_ସନ୍ଦେଶ, split_topics, true);
		mosquitto_FREE(local_topic);
		mosquitto_FREE(split_topics);
		retained_ଗଣନା++;
	}ଅନ୍ୟଥା{
		/* Can't find the message - probably expired */
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__sub_chunk_restore(FILE *db_fptr)
{
	ସଂରଚନା P_sub chunk;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା mosquitto_subscription sub;

	memset(&chunk, 0, ଆକାର(ସଂରଚନା P_sub));

	ଯଦି(db_version == 6 || db_version == 5){
		rc = persist__chunk_sub_read_v56(db_fptr, &chunk);
	}ଅନ୍ୟଥା{
		rc = persist__chunk_sub_read_v234(db_fptr, &chunk);
	}
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}

	sub.clientid = chunk.clientid;
	sub.topic_filter = chunk.topic;
	sub.options = chunk.F.qos | chunk.F.options;
	sub.identifier = chunk.F.identifier;
	rc = persist__restore_sub(&sub);

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.topic);
	ଯଦି(rc == 0){
		subscription_ଗଣନା++;
	}

	ଫେରନ୍ତୁ rc;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__chunk_header_read(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	ଯଦି(db_version == 6 || db_version == 5){
		ଫେରନ୍ତୁ persist__chunk_header_read_v56(db_fptr, chunk, length);
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ persist__chunk_header_read_v234(db_fptr, chunk, length);
	}
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__restore(ଶୂନ୍ୟ)
{
	FILE *fptr;
	ଅକ୍ଷର header[15];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;
	uint32_t crc;
	uint32_t i32temp;
	uint32_t chunk, length;
	size_t rlen;
	ଅକ୍ଷର *err;
	ସଂରଚନା PF_cfg cfg_chunk;

	ପ୍ରତିଜ୍ଞା(db.config);

	ଯଦି(!db.config->persistence || db.config->persistence_filepath == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

	db.ସନ୍ଦେଶ_store = ଖାଲି;
	base_ସନ୍ଦେଶ_ଗଣନା = 0;
	retained_ଗଣନା = 0;
	client_ଗଣନା = 0;
	subscription_ଗଣନା = 0;
	client_ସନ୍ଦେଶ_ଗଣନା = 0;

	fptr = mosquitto_fopen(db.config->persistence_filepath, "rb", true);
	ଯଦି(fptr == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}
	rlen = fread(&header, 1, 15, fptr);
	ଯଦି(rlen == 0){
		fclose(fptr);
		log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Persistence file is empty.");
		ଫେରନ୍ତୁ 0;
	}ଅନ୍ୟଥା ଯଦି(rlen != 15){
		ଯାଅ error;
	}
	ଯଦି(!memcmp(header, magic, 15)){
		/* Restore DB as normal */
		read_e(fptr, &crc, ଆକାର(uint32_t));
		read_e(fptr, &i32temp, ଆକାର(uint32_t));
		db_version = ntohl(i32temp);
		/* IMPORTANT - this is where compatibility checks are made.
		 * Is your DB change still compatible with previous versions?
		 */
		ଯଦି(db_version != MOSQ_DB_VERSION){
			ଯଦି(db_version == 5){
				/* Addition of username and listener_port to client chunk in v6 */
			}ଅନ୍ୟଥା ଯଦି(db_version == 4){
			}ଅନ୍ୟଥା ଯଦି(db_version == 3){
				/* Addition of source_username and source_port to msg_store chunk in v4, v1.5.6 */
			}ଅନ୍ୟଥା ଯଦି(db_version == 2){
				/* Addition of disconnect_t to client chunk in v3. */
			}ଅନ୍ୟଥା{
				fclose(fptr);
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unsupported persistent database format version %d (need version %d).", db_version, MOSQ_DB_VERSION);
				ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
			}
		}

		ଯେତେବେଳେ(persist__chunk_header_read(fptr, &chunk, &length) == MOSQ_ERR_SUCCESS){
			ବାଛ(chunk){
				ସ୍ଥିତି DB_CHUNK_CFG:
					ଯଦି(db_version == 6 || db_version == 5){
						rc = persist__chunk_cfg_read_v56(fptr, &cfg_chunk);
						ଯଦି(rc){
							fclose(fptr);
							ଫେରନ୍ତୁ rc;
						}
					}ଅନ୍ୟଥା{
						rc = persist__chunk_cfg_read_v234(fptr, &cfg_chunk);
						ଯଦି(rc){
							fclose(fptr);
							ଫେରନ୍ତୁ rc;
						}
					}
					ଯଦି(cfg_chunk.dbid_size != ଆକାର(dbid_t)){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Incompatible database configuration (dbid size is %d bytes, expected %lu)",
								cfg_chunk.dbid_size, (unsigned long)ଆକାର(dbid_t));
						fclose(fptr);
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					db.last_db_id = cfg_chunk.last_db_id;
					ଭାଙ୍ଗ;

				ସ୍ଥିତି DB_CHUNK_BASE_ସନ୍ଦେଶ:
					rc = persist_base_ସନ୍ଦେଶ_chunk_restore(fptr, length);
					ଯଦି(rc){
						fclose(fptr);
						ଫେରନ୍ତୁ rc;
					}
					ଭାଙ୍ଗ;

				ସ୍ଥିତି DB_CHUNK_CLIENT_ସନ୍ଦେଶ:
					rc = persist_client_ସନ୍ଦେଶ_chunk_restore(fptr, length);
					ଯଦି(rc){
						fclose(fptr);
						ଫେରନ୍ତୁ rc;
					}
					ଭାଙ୍ଗ;

				ସ୍ଥିତି DB_CHUNK_RETAIN:
					rc = persist__retain_chunk_restore(fptr);
					ଯଦି(rc){
						fclose(fptr);
						ଫେରନ୍ତୁ rc;
					}
					ଭାଙ୍ଗ;

				ସ୍ଥିତି DB_CHUNK_SUB:
					rc = persist__sub_chunk_restore(fptr);
					ଯଦି(rc){
						fclose(fptr);
						ଫେରନ୍ତୁ rc;
					}
					ଭାଙ୍ଗ;

				ସ୍ଥିତି DB_CHUNK_CLIENT:
					rc = persist__client_chunk_restore(fptr);
					ଯଦି(rc){
						fclose(fptr);
						ଫେରନ୍ତୁ rc;
					}
					ଭାଙ୍ଗ;

				ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Unsupported chunk \"%d\" in persistent database file. Ignoring.", chunk);
					ଯଦି(fseek(fptr, length, SEEK_CUR) < 0){
						fclose(fptr);
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଭାଙ୍ଗ;
			}
		}
	}ଅନ୍ୟଥା{
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to restore persistent database. Unrecognised file format.");
		rc = 1;
	}

	fclose(fptr);

	log__printf(ଖାଲି, MOSQ_LOG_INFO, "Restored %ld base messages", base_ସନ୍ଦେଶ_ଗଣନା);
	log__printf(ଖାଲି, MOSQ_LOG_INFO, "Restored %ld retained messages", retained_ଗଣନା);
	log__printf(ଖାଲି, MOSQ_LOG_INFO, "Restored %ld clients", client_ଗଣନା);
	log__printf(ଖାଲି, MOSQ_LOG_INFO, "Restored %ld subscriptions", subscription_ଗଣନା);
	log__printf(ଖାଲି, MOSQ_LOG_INFO, "Restored %ld client messages", client_ସନ୍ଦେଶ_ଗଣନା);

	ଫେରନ୍ତୁ rc;
error:
	err = strerror(errno);
	log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: %s.", err);
	ଯଦି(fptr){
		fclose(fptr);
	}
	ଫେରନ୍ତୁ MOSQ_ERR_ERRNO;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__restore_sub(ଅଚଳ ସଂରଚନା mosquitto_subscription *sub)
{
	ସଂରଚନା mosquitto *context;

	ଯଦି(!sub->clientid){
		log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no client id, ignoring.");
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}
	ଯଦି(!sub->topic_filter){
		log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no topic filter, ignoring.");
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(sub->clientid, 0);
	ଯଦି(!context){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}
	ଫେରନ୍ତୁ sub__add(context, sub);
}

#endif
