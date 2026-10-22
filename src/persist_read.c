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

#অন্তর্ভাৱ "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#অন্তর্ভাৱ <arpa/inet.h>
#endif
#অন্তর্ভাৱ <assert.h>
#অন্তর্ভাৱ <errno.h>
#অন্তর্ভাৱ <fcntl.h>
#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <string.h>
#অন্তর্ভাৱ <sys/stat.h>
#অন্তর্ভাৱ <time.h>
#অন্তর্ভাৱ <utlist.h>

#অন্তর্ভাৱ "mosquitto_broker_internal.h"
#অন্তর্ভাৱ "persist.h"
#অন্তর্ভাৱ "util_mosq.h"

uint32_t db_version;

ধ্রুৱ unsigned ৱর্ণ magic[15] = {0x00, 0xB5, 0x00, 'm', 'o', 's', 'q', 'u', 'i', 't', 't', 'o', ' ', 'd', 'b'};
স্থির long base_বাৰ্তা_গুনতি = 0;
স্থির long retained_গুনতি = 0;
স্থির long client_গুনতি = 0;
স্থির long subscription_গুনতি = 0;
স্থির long client_বাৰ্তা_গুনতি = 0;

স্থির পূর্ণাঙ্ক persist__restore_sub(ধ্রুৱ সংরচনা mosquitto_subscription *sub);

স্থির সংরচনা mosquitto *persist__find_or_add_context(ধ্রুৱ ৱর্ণ *clientid, uint16_t last_mid)
{
	সংরচনা mosquitto *context;

	যদি(!clientid){
		ঘূৰাই রিক্ত;
	}

	context = রিক্ত;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	যদি(!context){
		context = context__init();
		যদি(!context){
			ঘূৰাই রিক্ত;
		}
		context->id = mosquitto_strdup(clientid);
		যদি(!context->id){
			mosquitto_FREE(context);
			ঘূৰাই রিক্ত;
		}

		context->clean_start = false;

		context__add_to_by_id(context);
	}
	যদি(last_mid){
		context->last_mid = last_mid;
	}
	ঘূৰাই context;
}


পূর্ণাঙ্ক persist__read_string_len(FILE *db_fptr, ৱর্ণ **str, uint16_t len)
{
	ৱর্ণ *s = রিক্ত;

	যদি(len){
		s = mosquitto_malloc(len+1U);
		যদি(!s){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
			ঘূৰাই MOSQ_ERR_NOMEM;
		}
		যদি(fread(s, 1, len, db_fptr) != len){
			mosquitto_FREE(s);
			ঘূৰাই MOSQ_ERR_NOMEM;
		}
		s[len] = '\0';
	}

	*str = s;
	ঘূৰাই MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক persist__read_string(FILE *db_fptr, ৱর্ণ **str)
{
	uint16_t i16temp;
	uint16_t slen;

	যদি(fread(&i16temp, 1, পরিমাণ(uint16_t), db_fptr) != পরিমাণ(uint16_t)){
		ঘূৰাই MOSQ_ERR_INVAL;
	}

	slen = ntohs(i16temp);
	ঘূৰাই persist__read_string_len(db_fptr, str, slen);
}


স্থির পূর্ণাঙ্ক persist_client_বাৰ্তা_restore(সংরচনা P_client_বাৰ্তা *chunk)
{
	সংরচনা mosquitto_client_বাৰ্তা *cmsg;
	সংরচনা mosquitto_base_বাৰ্তা *বাৰ্তা;
	সংরচনা mosquitto *context;
	সংরচনা mosquitto_বাৰ্তা_data *বাৰ্তা_data;

	HASH_FIND(hh, db.বাৰ্তা_store, &chunk->F.store_id, পরিমাণ(chunk->F.store_id), বাৰ্তা);
	যদি(!বাৰ্তা){
		/* Can't find message - probably expired */
		ঘূৰাই MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk->clientid, 0);
	যদি(!context){
		log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Persistence file contains client message with no matching client. File may be corrupt.");
		ঘূৰাই 0;
	}

	cmsg = mosquitto_calloc(1, পরিমাণ(সংরচনা mosquitto_client_বাৰ্তা));
	যদি(!cmsg){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
		ঘূৰাই MOSQ_ERR_NOMEM;
	}

	cmsg->next = রিক্ত;
	cmsg->base_বাৰ্তা = রিক্ত;
	cmsg->data.cmsg_id = ++context->last_cmsg_id;
	cmsg->data.mid = chunk->F.mid;
	cmsg->data.qos = chunk->F.qos;
	cmsg->data.retain = (chunk->F.retain_dup&0xF0)>>4;
	cmsg->data.direction = chunk->F.direction;
	cmsg->data.state = chunk->F.state;
	cmsg->data.dup = chunk->F.retain_dup&0x0F;
	cmsg->data.subscription_identifier = chunk->subscription_identifier;

	cmsg->base_বাৰ্তা = বাৰ্তা;
	db_বাৰ্তা_store_ref_inc(cmsg->base_বাৰ্তা);

	যদি(cmsg->data.direction == mosq_md_out){
		বাৰ্তা_data = &context->msgs_out;
	}অন্যথা{
		বাৰ্তা_data = &context->msgs_in;
	}

	যদি(chunk->F.state == mosq_ms_queued || (chunk->F.qos > 0 && বাৰ্তা_data->inflight_quota == 0)){
		DL_APPEND(বাৰ্তা_data->queued, cmsg);
		db_বাৰ্তা_add_to_queued_stats(বাৰ্তা_data, cmsg);
	}অন্যথা{
		DL_APPEND(বাৰ্তা_data->inflight, cmsg);
		যদি(chunk->F.qos > 0 && বাৰ্তা_data->inflight_quota > 0){
			বাৰ্তা_data->inflight_quota--;
		}
		db_বাৰ্তা_add_to_inflight_stats(বাৰ্তা_data, cmsg);
	}

	ঘূৰাই MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণাঙ্ক persist__client_chunk_restore(FILE *db_fptr)
{
	পূর্ণাঙ্ক rc = 0;
	সংরচনা mosquitto *context;
	সংরচনা P_client chunk;

	memset(&chunk, 0, পরিমাণ(সংরচনা P_client));

	যদি(db_version == 6 || db_version == 5){
		rc = persist__chunk_client_read_v56(db_fptr, &chunk, db_version);
	}অন্যথা{
		rc = persist__chunk_client_read_v234(db_fptr, &chunk, db_version);
	}
	যদি(rc > 0){
		ঘূৰাই rc;
	}অন্যথা যদি(rc < 0){
		/* Client not loaded, but otherwise not an error */
		log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Empty client entry found in persistence database, it may be corrupt.");
		ঘূৰাই MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk.clientid, chunk.F.last_mid);
	যদি(context){
		context->session_expiry_time = chunk.F.session_expiry_time;
		context->session_expiry_interval = chunk.F.session_expiry_interval;
		যদি(chunk.username && !context->username){
			/* username is not freed here, it is now owned by context */
			context->username = chunk.username;
			chunk.username = রিক্ত;
		}
		/* in per_listener_settings mode, try to find the listener by persisted port */
		যদি(db.config->per_listener_settings && !context->listener && chunk.F.listener_port > 0){
			বাবে(পূর্ণাঙ্ক i=0; i < db.config->listener_গুনতি; i++){
				যদি(db.config->listeners[i].port == chunk.F.listener_port){
					context->listener = &db.config->listeners[i];
					ভঙ্গ;
				}
			}
		}
		session_expiry__add_from_persistence(context, chunk.F.session_expiry_time);
	}অন্যথা{
		rc = 1;
	}

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.username);
	যদি(rc == 0){
		client_গুনতি++;
	}
	ঘূৰাই rc;
}


স্থির পূর্ণাঙ্ক persist_client_বাৰ্তা_chunk_restore(FILE *db_fptr, uint32_t length)
{
	সংরচনা P_client_বাৰ্তা chunk;
	পূর্ণাঙ্ক rc;

	memset(&chunk, 0, পরিমাণ(সংরচনা P_client_বাৰ্তা));

	যদি(db_version == 6 || db_version == 5){
		rc = persist_chunk_client_বাৰ্তা_read_v56(db_fptr, &chunk, length);
	}অন্যথা{
		rc = persist_chunk_client_বাৰ্তা_read_v234(db_fptr, &chunk);
	}
	যদি(rc){
		ঘূৰাই rc;
	}

	rc = persist_client_বাৰ্তা_restore(&chunk);
	mosquitto_FREE(chunk.clientid);

	যদি(rc == 0){
		client_বাৰ্তা_গুনতি++;
	}
	ঘূৰাই rc;
}


স্থির পূর্ণাঙ্ক persist_base_বাৰ্তা_chunk_restore(FILE *db_fptr, uint32_t length)
{
	সংরচনা P_base_বাৰ্তা chunk;
	সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা = রিক্ত;
	int64_t message_expiry_interval64;
	uint32_t message_expiry_interval;
	uint32_t *p_message_expiry_interval = রিক্ত;
	পূর্ণাঙ্ক rc = 0;

	memset(&chunk, 0, পরিমাণ(সংরচনা P_base_বাৰ্তা));

	যদি(db_version == 6 || db_version == 5){
		rc = persist_chunk_base_বাৰ্তা_read_v56(db_fptr, &chunk, length);
	}অন্যথা{
		rc = persist_chunk_base_বাৰ্তা_read_v234(db_fptr, &chunk, db_version);
	}
	যদি(rc){
		ঘূৰাই rc;
	}
	যদি(chunk.F.topic_len == 0){
		rc = MOSQ_ERR_INVAL;
		গচ্ছ cleanup;
	}

	যদি(chunk.F.source_port){
		বাবে(পূর্ণাঙ্ক i=0; i<db.config->listener_গুনতি; i++){
			যদি(db.config->listeners[i].port == chunk.F.source_port){
				chunk.source.listener = &db.config->listeners[i];
				ভঙ্গ;
			}
		}
	}

	যদি(chunk.F.expiry_time > 0){
		message_expiry_interval64 = chunk.F.expiry_time - time(রিক্ত);
		যদি(message_expiry_interval64 < 0 || message_expiry_interval64 > UINT32_MAX){
			/* Expired message */
			rc = MOSQ_ERR_SUCCESS;
			গচ্ছ cleanup;
		}অন্যথা{
			message_expiry_interval = (uint32_t)message_expiry_interval64;
		}
		p_message_expiry_interval = &message_expiry_interval;
	}

	base_বাৰ্তা = mosquitto_calloc(1, পরিমাণ(সংরচনা mosquitto_base_বাৰ্তা));
	যদি(base_বাৰ্তা == রিক্ত){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
		rc = MOSQ_ERR_NOMEM;
		গচ্ছ cleanup;
	}

	base_বাৰ্তা->data.store_id = chunk.F.store_id;
	base_বাৰ্তা->data.source_mid = chunk.F.source_mid;
	base_বাৰ্তা->data.topic = chunk.topic;
	base_বাৰ্তা->data.qos = chunk.F.qos;
	base_বাৰ্তা->data.payloadlen = chunk.F.payloadlen;
	base_বাৰ্তা->data.retain = chunk.F.retain;
	base_বাৰ্তা->data.properties = chunk.properties;
	base_বাৰ্তা->data.payload = chunk.payload;
	base_বাৰ্তা->source_listener = chunk.source.listener;

	rc = db__message_store(&chunk.source, base_বাৰ্তা, p_message_expiry_interval,
			mosq_mo_client);

	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);

	যদি(rc == MOSQ_ERR_SUCCESS){
		base_বাৰ্তা_গুনতি++;
		ঘূৰাই MOSQ_ERR_SUCCESS;
	}অন্যথা{
		ঘূৰাই rc;
	}
cleanup:
	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);
	mosquitto_FREE(chunk.topic);
	mosquitto_FREE(chunk.payload);
	ঘূৰাই rc;
}


স্থির পূর্ণাঙ্ক persist__retain_chunk_restore(FILE *db_fptr)
{
	সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা;
	সংরচনা P_retain chunk;
	পূর্ণাঙ্ক rc;
	ৱর্ণ **split_topics;
	ৱর্ণ *local_topic;

	memset(&chunk, 0, পরিমাণ(সংরচনা P_retain));

	যদি(db_version == 6 || db_version == 5){
		rc = persist__chunk_retain_read_v56(db_fptr, &chunk);
	}অন্যথা{
		rc = persist__chunk_retain_read_v234(db_fptr, &chunk);
	}
	যদি(rc){
		ঘূৰাই rc;
	}

	HASH_FIND(hh, db.বাৰ্তা_store, &chunk.F.store_id, পরিমাণ(chunk.F.store_id), base_বাৰ্তা);
	যদি(base_বাৰ্তা){
		rc = sub__topic_tokenise(base_বাৰ্তা->data.topic, &local_topic, &split_topics, রিক্ত);
		যদি(rc){
			ঘূৰাই rc;
		}
		retain__store(base_বাৰ্তা->data.topic, base_বাৰ্তা, split_topics, true);
		mosquitto_FREE(local_topic);
		mosquitto_FREE(split_topics);
		retained_গুনতি++;
	}অন্যথা{
		/* Can't find the message - probably expired */
	}
	ঘূৰাই MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণাঙ্ক persist__sub_chunk_restore(FILE *db_fptr)
{
	সংরচনা P_sub chunk;
	পূর্ণাঙ্ক rc;
	সংরচনা mosquitto_subscription sub;

	memset(&chunk, 0, পরিমাণ(সংরচনা P_sub));

	যদি(db_version == 6 || db_version == 5){
		rc = persist__chunk_sub_read_v56(db_fptr, &chunk);
	}অন্যথা{
		rc = persist__chunk_sub_read_v234(db_fptr, &chunk);
	}
	যদি(rc){
		ঘূৰাই rc;
	}

	sub.clientid = chunk.clientid;
	sub.topic_filter = chunk.topic;
	sub.options = chunk.F.qos | chunk.F.options;
	sub.identifier = chunk.F.identifier;
	rc = persist__restore_sub(&sub);

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.topic);
	যদি(rc == 0){
		subscription_গুনতি++;
	}

	ঘূৰাই rc;
}


পূর্ণাঙ্ক persist__chunk_header_read(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	যদি(db_version == 6 || db_version == 5){
		ঘূৰাই persist__chunk_header_read_v56(db_fptr, chunk, length);
	}অন্যথা{
		ঘূৰাই persist__chunk_header_read_v234(db_fptr, chunk, length);
	}
}


পূর্ণাঙ্ক persist__restore(শূন্য)
{
	FILE *fptr;
	ৱর্ণ header[15];
	পূর্ণাঙ্ক rc = 0;
	uint32_t crc;
	uint32_t i32temp;
	uint32_t chunk, length;
	size_t rlen;
	ৱর্ণ *err;
	সংরচনা PF_cfg cfg_chunk;

	প্রতিজ্ঞা(db.config);

	যদি(!db.config->persistence || db.config->persistence_filepath == রিক্ত){
		ঘূৰাই MOSQ_ERR_SUCCESS;
	}

	db.বাৰ্তা_store = রিক্ত;
	base_বাৰ্তা_গুনতি = 0;
	retained_গুনতি = 0;
	client_গুনতি = 0;
	subscription_গুনতি = 0;
	client_বাৰ্তা_গুনতি = 0;

	fptr = mosquitto_fopen(db.config->persistence_filepath, "rb", true);
	যদি(fptr == রিক্ত){
		ঘূৰাই MOSQ_ERR_SUCCESS;
	}
	rlen = fread(&header, 1, 15, fptr);
	যদি(rlen == 0){
		fclose(fptr);
		log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Persistence file is empty.");
		ঘূৰাই 0;
	}অন্যথা যদি(rlen != 15){
		গচ্ছ error;
	}
	যদি(!memcmp(header, magic, 15)){
		/* Restore DB as normal */
		read_e(fptr, &crc, পরিমাণ(uint32_t));
		read_e(fptr, &i32temp, পরিমাণ(uint32_t));
		db_version = ntohl(i32temp);
		/* IMPORTANT - this is where compatibility checks are made.
		 * Is your DB change still compatible with previous versions?
		 */
		যদি(db_version != MOSQ_DB_VERSION){
			যদি(db_version == 5){
				/* Addition of username and listener_port to client chunk in v6 */
			}অন্যথা যদি(db_version == 4){
			}অন্যথা যদি(db_version == 3){
				/* Addition of source_username and source_port to msg_store chunk in v4, v1.5.6 */
			}অন্যথা যদি(db_version == 2){
				/* Addition of disconnect_t to client chunk in v3. */
			}অন্যথা{
				fclose(fptr);
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unsupported persistent database format version %d (need version %d).", db_version, MOSQ_DB_VERSION);
				ঘূৰাই MOSQ_ERR_INVAL;
			}
		}

		যেতিয়ালৈ(persist__chunk_header_read(fptr, &chunk, &length) == MOSQ_ERR_SUCCESS){
			ৱিকল্প(chunk){
				পক্ষ DB_CHUNK_CFG:
					যদি(db_version == 6 || db_version == 5){
						rc = persist__chunk_cfg_read_v56(fptr, &cfg_chunk);
						যদি(rc){
							fclose(fptr);
							ঘূৰাই rc;
						}
					}অন্যথা{
						rc = persist__chunk_cfg_read_v234(fptr, &cfg_chunk);
						যদি(rc){
							fclose(fptr);
							ঘূৰাই rc;
						}
					}
					যদি(cfg_chunk.dbid_size != পরিমাণ(dbid_t)){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Incompatible database configuration (dbid size is %d bytes, expected %lu)",
								cfg_chunk.dbid_size, (unsigned long)পরিমাণ(dbid_t));
						fclose(fptr);
						ঘূৰাই MOSQ_ERR_INVAL;
					}
					db.last_db_id = cfg_chunk.last_db_id;
					ভঙ্গ;

				পক্ষ DB_CHUNK_BASE_বাৰ্তা:
					rc = persist_base_বাৰ্তা_chunk_restore(fptr, length);
					যদি(rc){
						fclose(fptr);
						ঘূৰাই rc;
					}
					ভঙ্গ;

				পক্ষ DB_CHUNK_CLIENT_বাৰ্তা:
					rc = persist_client_বাৰ্তা_chunk_restore(fptr, length);
					যদি(rc){
						fclose(fptr);
						ঘূৰাই rc;
					}
					ভঙ্গ;

				পক্ষ DB_CHUNK_RETAIN:
					rc = persist__retain_chunk_restore(fptr);
					যদি(rc){
						fclose(fptr);
						ঘূৰাই rc;
					}
					ভঙ্গ;

				পক্ষ DB_CHUNK_SUB:
					rc = persist__sub_chunk_restore(fptr);
					যদি(rc){
						fclose(fptr);
						ঘূৰাই rc;
					}
					ভঙ্গ;

				পক্ষ DB_CHUNK_CLIENT:
					rc = persist__client_chunk_restore(fptr);
					যদি(rc){
						fclose(fptr);
						ঘূৰাই rc;
					}
					ভঙ্গ;

				স্ৱভাৱ:
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Unsupported chunk \"%d\" in persistent database file. Ignoring.", chunk);
					যদি(fseek(fptr, length, SEEK_CUR) < 0){
						fclose(fptr);
						ঘূৰাই MOSQ_ERR_INVAL;
					}
					ভঙ্গ;
			}
		}
	}অন্যথা{
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to restore persistent database. Unrecognised file format.");
		rc = 1;
	}

	fclose(fptr);

	log__printf(রিক্ত, MOSQ_LOG_INFO, "Restored %ld base messages", base_বাৰ্তা_গুনতি);
	log__printf(রিক্ত, MOSQ_LOG_INFO, "Restored %ld retained messages", retained_গুনতি);
	log__printf(রিক্ত, MOSQ_LOG_INFO, "Restored %ld clients", client_গুনতি);
	log__printf(রিক্ত, MOSQ_LOG_INFO, "Restored %ld subscriptions", subscription_গুনতি);
	log__printf(রিক্ত, MOSQ_LOG_INFO, "Restored %ld client messages", client_বাৰ্তা_গুনতি);

	ঘূৰাই rc;
error:
	err = strerror(errno);
	log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: %s.", err);
	যদি(fptr){
		fclose(fptr);
	}
	ঘূৰাই MOSQ_ERR_ERRNO;
}


স্থির পূর্ণাঙ্ক persist__restore_sub(ধ্রুৱ সংরচনা mosquitto_subscription *sub)
{
	সংরচনা mosquitto *context;

	যদি(!sub->clientid){
		log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no client id, ignoring.");
		ঘূৰাই MOSQ_ERR_SUCCESS;
	}
	যদি(!sub->topic_filter){
		log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no topic filter, ignoring.");
		ঘূৰাই MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(sub->clientid, 0);
	যদি(!context){
		ঘূৰাই MOSQ_ERR_INVAL;
	}
	ঘূৰাই sub__add(context, sub);
}

#endif
