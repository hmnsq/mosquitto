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

#અન્તર્ભાવ "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#અન્તર્ભાવ <arpa/inet.h>
#endif
#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <errno.h>
#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <sys/stat.h>
#અન્તર્ભાવ <time.h>
#અન્તર્ભાવ <utlist.h>

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "persist.h"
#અન્તર્ભાવ "util_mosq.h"

uint32_t db_version;

અચલ unsigned અક્ષર magic[15] = {0x00, 0xB5, 0x00, 'm', 'o', 's', 'q', 'u', 'i', 't', 't', 'o', ' ', 'd', 'b'};
સ્થિર long base_સંદેશ_ગણતરી = 0;
સ્થિર long retained_ગણતરી = 0;
સ્થિર long client_ગણતરી = 0;
સ્થિર long subscription_ગણતરી = 0;
સ્થિર long client_સંદેશ_ગણતરી = 0;

સ્થિર પૂર્ણાંક persist__restore_sub(અચલ સંરચના mosquitto_subscription *sub);

સ્થિર સંરચના mosquitto *persist__find_or_add_context(અચલ અક્ષર *clientid, uint16_t last_mid)
{
	સંરચના mosquitto *context;

	જો(!clientid){
		પાછા ખાલી;
	}

	context = ખાલી;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	જો(!context){
		context = context__init();
		જો(!context){
			પાછા ખાલી;
		}
		context->id = mosquitto_strdup(clientid);
		જો(!context->id){
			mosquitto_FREE(context);
			પાછા ખાલી;
		}

		context->clean_start = false;

		context__add_to_by_id(context);
	}
	જો(last_mid){
		context->last_mid = last_mid;
	}
	પાછા context;
}


પૂર્ણાંક persist__read_string_len(FILE *db_fptr, અક્ષર **str, uint16_t len)
{
	અક્ષર *s = ખાલી;

	જો(len){
		s = mosquitto_malloc(len+1U);
		જો(!s){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
			પાછા MOSQ_ERR_NOMEM;
		}
		જો(fread(s, 1, len, db_fptr) != len){
			mosquitto_FREE(s);
			પાછા MOSQ_ERR_NOMEM;
		}
		s[len] = '\0';
	}

	*str = s;
	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક persist__read_string(FILE *db_fptr, અક્ષર **str)
{
	uint16_t i16temp;
	uint16_t slen;

	જો(fread(&i16temp, 1, કદ(uint16_t), db_fptr) != કદ(uint16_t)){
		પાછા MOSQ_ERR_INVAL;
	}

	slen = ntohs(i16temp);
	પાછા persist__read_string_len(db_fptr, str, slen);
}


સ્થિર પૂર્ણાંક persist_client_સંદેશ_restore(સંરચના P_client_સંદેશ *chunk)
{
	સંરચના mosquitto_client_સંદેશ *cmsg;
	સંરચના mosquitto_base_સંદેશ *સંદેશ;
	સંરચના mosquitto *context;
	સંરચના mosquitto_સંદેશ_data *સંદેશ_data;

	HASH_FIND(hh, db.સંદેશ_store, &chunk->F.store_id, કદ(chunk->F.store_id), સંદેશ);
	જો(!સંદેશ){
		/* Can't find message - probably expired */
		પાછા MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk->clientid, 0);
	જો(!context){
		log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Persistence file contains client message with no matching client. File may be corrupt.");
		પાછા 0;
	}

	cmsg = mosquitto_calloc(1, કદ(સંરચના mosquitto_client_સંદેશ));
	જો(!cmsg){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
		પાછા MOSQ_ERR_NOMEM;
	}

	cmsg->next = ખાલી;
	cmsg->base_સંદેશ = ખાલી;
	cmsg->data.cmsg_id = ++context->last_cmsg_id;
	cmsg->data.mid = chunk->F.mid;
	cmsg->data.qos = chunk->F.qos;
	cmsg->data.retain = (chunk->F.retain_dup&0xF0)>>4;
	cmsg->data.direction = chunk->F.direction;
	cmsg->data.state = chunk->F.state;
	cmsg->data.dup = chunk->F.retain_dup&0x0F;
	cmsg->data.subscription_identifier = chunk->subscription_identifier;

	cmsg->base_સંદેશ = સંદેશ;
	db_સંદેશ_store_ref_inc(cmsg->base_સંદેશ);

	જો(cmsg->data.direction == mosq_md_out){
		સંદેશ_data = &context->msgs_out;
	}નહીંતર{
		સંદેશ_data = &context->msgs_in;
	}

	જો(chunk->F.state == mosq_ms_queued || (chunk->F.qos > 0 && સંદેશ_data->inflight_quota == 0)){
		DL_APPEND(સંદેશ_data->queued, cmsg);
		db_સંદેશ_add_to_queued_stats(સંદેશ_data, cmsg);
	}નહીંતર{
		DL_APPEND(સંદેશ_data->inflight, cmsg);
		જો(chunk->F.qos > 0 && સંદેશ_data->inflight_quota > 0){
			સંદેશ_data->inflight_quota--;
		}
		db_સંદેશ_add_to_inflight_stats(સંદેશ_data, cmsg);
	}

	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક persist__client_chunk_restore(FILE *db_fptr)
{
	પૂર્ણાંક rc = 0;
	સંરચના mosquitto *context;
	સંરચના P_client chunk;

	memset(&chunk, 0, કદ(સંરચના P_client));

	જો(db_version == 6 || db_version == 5){
		rc = persist__chunk_client_read_v56(db_fptr, &chunk, db_version);
	}નહીંતર{
		rc = persist__chunk_client_read_v234(db_fptr, &chunk, db_version);
	}
	જો(rc > 0){
		પાછા rc;
	}નહીંતર જો(rc < 0){
		/* Client not loaded, but otherwise not an error */
		log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Empty client entry found in persistence database, it may be corrupt.");
		પાછા MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk.clientid, chunk.F.last_mid);
	જો(context){
		context->session_expiry_time = chunk.F.session_expiry_time;
		context->session_expiry_interval = chunk.F.session_expiry_interval;
		જો(chunk.username && !context->username){
			/* username is not freed here, it is now owned by context */
			context->username = chunk.username;
			chunk.username = ખાલી;
		}
		/* in per_listener_settings mode, try to find the listener by persisted port */
		જો(db.config->per_listener_settings && !context->listener && chunk.F.listener_port > 0){
			માટે(પૂર્ણાંક i=0; i < db.config->listener_ગણતરી; i++){
				જો(db.config->listeners[i].port == chunk.F.listener_port){
					context->listener = &db.config->listeners[i];
					તોડો;
				}
			}
		}
		session_expiry__add_from_persistence(context, chunk.F.session_expiry_time);
	}નહીંતર{
		rc = 1;
	}

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.username);
	જો(rc == 0){
		client_ગણતરી++;
	}
	પાછા rc;
}


સ્થિર પૂર્ણાંક persist_client_સંદેશ_chunk_restore(FILE *db_fptr, uint32_t length)
{
	સંરચના P_client_સંદેશ chunk;
	પૂર્ણાંક rc;

	memset(&chunk, 0, કદ(સંરચના P_client_સંદેશ));

	જો(db_version == 6 || db_version == 5){
		rc = persist_chunk_client_સંદેશ_read_v56(db_fptr, &chunk, length);
	}નહીંતર{
		rc = persist_chunk_client_સંદેશ_read_v234(db_fptr, &chunk);
	}
	જો(rc){
		પાછા rc;
	}

	rc = persist_client_સંદેશ_restore(&chunk);
	mosquitto_FREE(chunk.clientid);

	જો(rc == 0){
		client_સંદેશ_ગણતરી++;
	}
	પાછા rc;
}


સ્થિર પૂર્ણાંક persist_base_સંદેશ_chunk_restore(FILE *db_fptr, uint32_t length)
{
	સંરચના P_base_સંદેશ chunk;
	સંરચના mosquitto_base_સંદેશ *base_સંદેશ = ખાલી;
	int64_t message_expiry_interval64;
	uint32_t message_expiry_interval;
	uint32_t *p_message_expiry_interval = ખાલી;
	પૂર્ણાંક rc = 0;

	memset(&chunk, 0, કદ(સંરચના P_base_સંદેશ));

	જો(db_version == 6 || db_version == 5){
		rc = persist_chunk_base_સંદેશ_read_v56(db_fptr, &chunk, length);
	}નહીંતર{
		rc = persist_chunk_base_સંદેશ_read_v234(db_fptr, &chunk, db_version);
	}
	જો(rc){
		પાછા rc;
	}
	જો(chunk.F.topic_len == 0){
		rc = MOSQ_ERR_INVAL;
		જાઓ cleanup;
	}

	જો(chunk.F.source_port){
		માટે(પૂર્ણાંક i=0; i<db.config->listener_ગણતરી; i++){
			જો(db.config->listeners[i].port == chunk.F.source_port){
				chunk.source.listener = &db.config->listeners[i];
				તોડો;
			}
		}
	}

	જો(chunk.F.expiry_time > 0){
		message_expiry_interval64 = chunk.F.expiry_time - time(ખાલી);
		જો(message_expiry_interval64 < 0 || message_expiry_interval64 > UINT32_MAX){
			/* Expired message */
			rc = MOSQ_ERR_SUCCESS;
			જાઓ cleanup;
		}નહીંતર{
			message_expiry_interval = (uint32_t)message_expiry_interval64;
		}
		p_message_expiry_interval = &message_expiry_interval;
	}

	base_સંદેશ = mosquitto_calloc(1, કદ(સંરચના mosquitto_base_સંદેશ));
	જો(base_સંદેશ == ખાલી){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
		rc = MOSQ_ERR_NOMEM;
		જાઓ cleanup;
	}

	base_સંદેશ->data.store_id = chunk.F.store_id;
	base_સંદેશ->data.source_mid = chunk.F.source_mid;
	base_સંદેશ->data.topic = chunk.topic;
	base_સંદેશ->data.qos = chunk.F.qos;
	base_સંદેશ->data.payloadlen = chunk.F.payloadlen;
	base_સંદેશ->data.retain = chunk.F.retain;
	base_સંદેશ->data.properties = chunk.properties;
	base_સંદેશ->data.payload = chunk.payload;
	base_સંદેશ->source_listener = chunk.source.listener;

	rc = db__message_store(&chunk.source, base_સંદેશ, p_message_expiry_interval,
			mosq_mo_client);

	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);

	જો(rc == MOSQ_ERR_SUCCESS){
		base_સંદેશ_ગણતરી++;
		પાછા MOSQ_ERR_SUCCESS;
	}નહીંતર{
		પાછા rc;
	}
cleanup:
	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);
	mosquitto_FREE(chunk.topic);
	mosquitto_FREE(chunk.payload);
	પાછા rc;
}


સ્થિર પૂર્ણાંક persist__retain_chunk_restore(FILE *db_fptr)
{
	સંરચના mosquitto_base_સંદેશ *base_સંદેશ;
	સંરચના P_retain chunk;
	પૂર્ણાંક rc;
	અક્ષર **split_topics;
	અક્ષર *local_topic;

	memset(&chunk, 0, કદ(સંરચના P_retain));

	જો(db_version == 6 || db_version == 5){
		rc = persist__chunk_retain_read_v56(db_fptr, &chunk);
	}નહીંતર{
		rc = persist__chunk_retain_read_v234(db_fptr, &chunk);
	}
	જો(rc){
		પાછા rc;
	}

	HASH_FIND(hh, db.સંદેશ_store, &chunk.F.store_id, કદ(chunk.F.store_id), base_સંદેશ);
	જો(base_સંદેશ){
		rc = sub__topic_tokenise(base_સંદેશ->data.topic, &local_topic, &split_topics, ખાલી);
		જો(rc){
			પાછા rc;
		}
		retain__store(base_સંદેશ->data.topic, base_સંદેશ, split_topics, true);
		mosquitto_FREE(local_topic);
		mosquitto_FREE(split_topics);
		retained_ગણતરી++;
	}નહીંતર{
		/* Can't find the message - probably expired */
	}
	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક persist__sub_chunk_restore(FILE *db_fptr)
{
	સંરચના P_sub chunk;
	પૂર્ણાંક rc;
	સંરચના mosquitto_subscription sub;

	memset(&chunk, 0, કદ(સંરચના P_sub));

	જો(db_version == 6 || db_version == 5){
		rc = persist__chunk_sub_read_v56(db_fptr, &chunk);
	}નહીંતર{
		rc = persist__chunk_sub_read_v234(db_fptr, &chunk);
	}
	જો(rc){
		પાછા rc;
	}

	sub.clientid = chunk.clientid;
	sub.topic_filter = chunk.topic;
	sub.options = chunk.F.qos | chunk.F.options;
	sub.identifier = chunk.F.identifier;
	rc = persist__restore_sub(&sub);

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.topic);
	જો(rc == 0){
		subscription_ગણતરી++;
	}

	પાછા rc;
}


પૂર્ણાંક persist__chunk_header_read(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	જો(db_version == 6 || db_version == 5){
		પાછા persist__chunk_header_read_v56(db_fptr, chunk, length);
	}નહીંતર{
		પાછા persist__chunk_header_read_v234(db_fptr, chunk, length);
	}
}


પૂર્ણાંક persist__restore(શૂન્ય)
{
	FILE *fptr;
	અક્ષર header[15];
	પૂર્ણાંક rc = 0;
	uint32_t crc;
	uint32_t i32temp;
	uint32_t chunk, length;
	size_t rlen;
	અક્ષર *err;
	સંરચના PF_cfg cfg_chunk;

	પ્રતિજ્ઞા(db.config);

	જો(!db.config->persistence || db.config->persistence_filepath == ખાલી){
		પાછા MOSQ_ERR_SUCCESS;
	}

	db.સંદેશ_store = ખાલી;
	base_સંદેશ_ગણતરી = 0;
	retained_ગણતરી = 0;
	client_ગણતરી = 0;
	subscription_ગણતરી = 0;
	client_સંદેશ_ગણતરી = 0;

	fptr = mosquitto_fopen(db.config->persistence_filepath, "rb", true);
	જો(fptr == ખાલી){
		પાછા MOSQ_ERR_SUCCESS;
	}
	rlen = fread(&header, 1, 15, fptr);
	જો(rlen == 0){
		fclose(fptr);
		log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Persistence file is empty.");
		પાછા 0;
	}નહીંતર જો(rlen != 15){
		જાઓ error;
	}
	જો(!memcmp(header, magic, 15)){
		/* Restore DB as normal */
		read_e(fptr, &crc, કદ(uint32_t));
		read_e(fptr, &i32temp, કદ(uint32_t));
		db_version = ntohl(i32temp);
		/* IMPORTANT - this is where compatibility checks are made.
		 * Is your DB change still compatible with previous versions?
		 */
		જો(db_version != MOSQ_DB_VERSION){
			જો(db_version == 5){
				/* Addition of username and listener_port to client chunk in v6 */
			}નહીંતર જો(db_version == 4){
			}નહીંતર જો(db_version == 3){
				/* Addition of source_username and source_port to msg_store chunk in v4, v1.5.6 */
			}નહીંતર જો(db_version == 2){
				/* Addition of disconnect_t to client chunk in v3. */
			}નહીંતર{
				fclose(fptr);
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unsupported persistent database format version %d (need version %d).", db_version, MOSQ_DB_VERSION);
				પાછા MOSQ_ERR_INVAL;
			}
		}

		જ્યારે(persist__chunk_header_read(fptr, &chunk, &length) == MOSQ_ERR_SUCCESS){
			પસંદગી(chunk){
				સ્થિતિ DB_CHUNK_CFG:
					જો(db_version == 6 || db_version == 5){
						rc = persist__chunk_cfg_read_v56(fptr, &cfg_chunk);
						જો(rc){
							fclose(fptr);
							પાછા rc;
						}
					}નહીંતર{
						rc = persist__chunk_cfg_read_v234(fptr, &cfg_chunk);
						જો(rc){
							fclose(fptr);
							પાછા rc;
						}
					}
					જો(cfg_chunk.dbid_size != કદ(dbid_t)){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Incompatible database configuration (dbid size is %d bytes, expected %lu)",
								cfg_chunk.dbid_size, (unsigned long)કદ(dbid_t));
						fclose(fptr);
						પાછા MOSQ_ERR_INVAL;
					}
					db.last_db_id = cfg_chunk.last_db_id;
					તોડો;

				સ્થિતિ DB_CHUNK_BASE_સંદેશ:
					rc = persist_base_સંદેશ_chunk_restore(fptr, length);
					જો(rc){
						fclose(fptr);
						પાછા rc;
					}
					તોડો;

				સ્થિતિ DB_CHUNK_CLIENT_સંદેશ:
					rc = persist_client_સંદેશ_chunk_restore(fptr, length);
					જો(rc){
						fclose(fptr);
						પાછા rc;
					}
					તોડો;

				સ્થિતિ DB_CHUNK_RETAIN:
					rc = persist__retain_chunk_restore(fptr);
					જો(rc){
						fclose(fptr);
						પાછા rc;
					}
					તોડો;

				સ્થિતિ DB_CHUNK_SUB:
					rc = persist__sub_chunk_restore(fptr);
					જો(rc){
						fclose(fptr);
						પાછા rc;
					}
					તોડો;

				સ્થિતિ DB_CHUNK_CLIENT:
					rc = persist__client_chunk_restore(fptr);
					જો(rc){
						fclose(fptr);
						પાછા rc;
					}
					તોડો;

				મૂળભૂત:
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Unsupported chunk \"%d\" in persistent database file. Ignoring.", chunk);
					જો(fseek(fptr, length, SEEK_CUR) < 0){
						fclose(fptr);
						પાછા MOSQ_ERR_INVAL;
					}
					તોડો;
			}
		}
	}નહીંતર{
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to restore persistent database. Unrecognised file format.");
		rc = 1;
	}

	fclose(fptr);

	log__printf(ખાલી, MOSQ_LOG_INFO, "Restored %ld base messages", base_સંદેશ_ગણતરી);
	log__printf(ખાલી, MOSQ_LOG_INFO, "Restored %ld retained messages", retained_ગણતરી);
	log__printf(ખાલી, MOSQ_LOG_INFO, "Restored %ld clients", client_ગણતરી);
	log__printf(ખાલી, MOSQ_LOG_INFO, "Restored %ld subscriptions", subscription_ગણતરી);
	log__printf(ખાલી, MOSQ_LOG_INFO, "Restored %ld client messages", client_સંદેશ_ગણતરી);

	પાછા rc;
error:
	err = strerror(errno);
	log__printf(ખાલી, MOSQ_LOG_ERR, "Error: %s.", err);
	જો(fptr){
		fclose(fptr);
	}
	પાછા MOSQ_ERR_ERRNO;
}


સ્થિર પૂર્ણાંક persist__restore_sub(અચલ સંરચના mosquitto_subscription *sub)
{
	સંરચના mosquitto *context;

	જો(!sub->clientid){
		log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no client id, ignoring.");
		પાછા MOSQ_ERR_SUCCESS;
	}
	જો(!sub->topic_filter){
		log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no topic filter, ignoring.");
		પાછા MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(sub->clientid, 0);
	જો(!context){
		પાછા MOSQ_ERR_INVAL;
	}
	પાછા sub__add(context, sub);
}

#endif
