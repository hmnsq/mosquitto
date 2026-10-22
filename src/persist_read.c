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

#অন্তর্ভাব "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#অন্তর্ভাব <arpa/inet.h>
#endif
#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <errno.h>
#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <sys/stat.h>
#অন্তর্ভাব <time.h>
#অন্তর্ভাব <utlist.h>

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "persist.h"
#অন্তর্ভাব "util_mosq.h"

uint32_t db_version;

ধ্রুবক unsigned অক্ষর magic[15] = {0x00, 0xB5, 0x00, 'm', 'o', 's', 'q', 'u', 'i', 't', 't', 'o', ' ', 'd', 'b'};
স্থির long base_বার্তা_গুনতি = 0;
স্থির long retained_গুনতি = 0;
স্থির long client_গুনতি = 0;
স্থির long subscription_গুনতি = 0;
স্থির long client_বার্তা_গুনতি = 0;

স্থির পূর্ণসংখ্যা persist__restore_sub(ধ্রুবক কাঠামো mosquitto_subscription *sub);

স্থির কাঠামো mosquitto *persist__find_or_add_context(ধ্রুবক অক্ষর *clientid, uint16_t last_mid)
{
	কাঠামো mosquitto *context;

	যদি(!clientid){
		ফেরত ফাঁকা;
	}

	context = ফাঁকা;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	যদি(!context){
		context = context__init();
		যদি(!context){
			ফেরত ফাঁকা;
		}
		context->id = mosquitto_strdup(clientid);
		যদি(!context->id){
			mosquitto_FREE(context);
			ফেরত ফাঁকা;
		}

		context->clean_start = false;

		context__add_to_by_id(context);
	}
	যদি(last_mid){
		context->last_mid = last_mid;
	}
	ফেরত context;
}


পূর্ণসংখ্যা persist__read_string_len(FILE *db_fptr, অক্ষর **str, uint16_t len)
{
	অক্ষর *s = ফাঁকা;

	যদি(len){
		s = mosquitto_malloc(len+1U);
		যদি(!s){
			log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Out of memory.");
			ফেরত MOSQ_ERR_NOMEM;
		}
		যদি(fread(s, 1, len, db_fptr) != len){
			mosquitto_FREE(s);
			ফেরত MOSQ_ERR_NOMEM;
		}
		s[len] = '\0';
	}

	*str = s;
	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা persist__read_string(FILE *db_fptr, অক্ষর **str)
{
	uint16_t i16temp;
	uint16_t slen;

	যদি(fread(&i16temp, 1, আকার(uint16_t), db_fptr) != আকার(uint16_t)){
		ফেরত MOSQ_ERR_INVAL;
	}

	slen = ntohs(i16temp);
	ফেরত persist__read_string_len(db_fptr, str, slen);
}


স্থির পূর্ণসংখ্যা persist_client_বার্তা_restore(কাঠামো P_client_বার্তা *chunk)
{
	কাঠামো mosquitto_client_বার্তা *cmsg;
	কাঠামো mosquitto_base_বার্তা *বার্তা;
	কাঠামো mosquitto *context;
	কাঠামো mosquitto_বার্তা_তথ্য *বার্তা_তথ্য;

	HASH_FIND(hh, db.বার্তা_store, &chunk->F.store_id, আকার(chunk->F.store_id), বার্তা);
	যদি(!বার্তা){
		/* Can't find message - probably expired */
		ফেরত MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk->clientid, 0);
	যদি(!context){
		log__printf(ফাঁকা, MOSQ_LOG_WARNING, "Warning: Persistence file contains client message with no matching client. File may be corrupt.");
		ফেরত 0;
	}

	cmsg = mosquitto_calloc(1, আকার(কাঠামো mosquitto_client_বার্তা));
	যদি(!cmsg){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Out of memory.");
		ফেরত MOSQ_ERR_NOMEM;
	}

	cmsg->পরবর্তী = ফাঁকা;
	cmsg->base_বার্তা = ফাঁকা;
	cmsg->তথ্য.cmsg_id = ++context->last_cmsg_id;
	cmsg->তথ্য.mid = chunk->F.mid;
	cmsg->তথ্য.qos = chunk->F.qos;
	cmsg->তথ্য.retain = (chunk->F.retain_dup&0xF0)>>4;
	cmsg->তথ্য.direction = chunk->F.direction;
	cmsg->তথ্য.state = chunk->F.state;
	cmsg->তথ্য.dup = chunk->F.retain_dup&0x0F;
	cmsg->তথ্য.subscription_identifier = chunk->subscription_identifier;

	cmsg->base_বার্তা = বার্তা;
	db_বার্তা_store_ref_inc(cmsg->base_বার্তা);

	যদি(cmsg->তথ্য.direction == mosq_md_out){
		বার্তা_তথ্য = &context->msgs_out;
	}অন্যথা{
		বার্তা_তথ্য = &context->msgs_in;
	}

	যদি(chunk->F.state == mosq_ms_queued || (chunk->F.qos > 0 && বার্তা_তথ্য->inflight_quota == 0)){
		DL_APPEND(বার্তা_তথ্য->queued, cmsg);
		db_বার্তা_add_to_queued_stats(বার্তা_তথ্য, cmsg);
	}অন্যথা{
		DL_APPEND(বার্তা_তথ্য->inflight, cmsg);
		যদি(chunk->F.qos > 0 && বার্তা_তথ্য->inflight_quota > 0){
			বার্তা_তথ্য->inflight_quota--;
		}
		db_বার্তা_add_to_inflight_stats(বার্তা_তথ্য, cmsg);
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা persist__client_chunk_restore(FILE *db_fptr)
{
	পূর্ণসংখ্যা rc = 0;
	কাঠামো mosquitto *context;
	কাঠামো P_client chunk;

	memset(&chunk, 0, আকার(কাঠামো P_client));

	যদি(db_version == 6 || db_version == 5){
		rc = persist__chunk_client_read_v56(db_fptr, &chunk, db_version);
	}অন্যথা{
		rc = persist__chunk_client_read_v234(db_fptr, &chunk, db_version);
	}
	যদি(rc > 0){
		ফেরত rc;
	}অন্যথা যদি(rc < 0){
		/* Client not loaded, but otherwise not an error */
		log__printf(ফাঁকা, MOSQ_LOG_WARNING, "Warning: Empty client entry found in persistence database, it may be corrupt.");
		ফেরত MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk.clientid, chunk.F.last_mid);
	যদি(context){
		context->session_expiry_time = chunk.F.session_expiry_time;
		context->session_expiry_interval = chunk.F.session_expiry_interval;
		যদি(chunk.username && !context->username){
			/* username is not freed here, it is now owned by context */
			context->username = chunk.username;
			chunk.username = ফাঁকা;
		}
		/* in per_listener_settings mode, try to find the listener by persisted port */
		যদি(db.config->per_listener_settings && !context->listener && chunk.F.listener_port > 0){
			জন্য(পূর্ণসংখ্যা i=0; i < db.config->listener_গুনতি; i++){
				যদি(db.config->listeners[i].port == chunk.F.listener_port){
					context->listener = &db.config->listeners[i];
					বিরতি;
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
	ফেরত rc;
}


স্থির পূর্ণসংখ্যা persist_client_বার্তা_chunk_restore(FILE *db_fptr, uint32_t দৈর্ঘ্য)
{
	কাঠামো P_client_বার্তা chunk;
	পূর্ণসংখ্যা rc;

	memset(&chunk, 0, আকার(কাঠামো P_client_বার্তা));

	যদি(db_version == 6 || db_version == 5){
		rc = persist_chunk_client_বার্তা_read_v56(db_fptr, &chunk, দৈর্ঘ্য);
	}অন্যথা{
		rc = persist_chunk_client_বার্তা_read_v234(db_fptr, &chunk);
	}
	যদি(rc){
		ফেরত rc;
	}

	rc = persist_client_বার্তা_restore(&chunk);
	mosquitto_FREE(chunk.clientid);

	যদি(rc == 0){
		client_বার্তা_গুনতি++;
	}
	ফেরত rc;
}


স্থির পূর্ণসংখ্যা persist_base_বার্তা_chunk_restore(FILE *db_fptr, uint32_t দৈর্ঘ্য)
{
	কাঠামো P_base_বার্তা chunk;
	কাঠামো mosquitto_base_বার্তা *base_বার্তা = ফাঁকা;
	int64_t message_expiry_interval64;
	uint32_t message_expiry_interval;
	uint32_t *p_message_expiry_interval = ফাঁকা;
	পূর্ণসংখ্যা rc = 0;

	memset(&chunk, 0, আকার(কাঠামো P_base_বার্তা));

	যদি(db_version == 6 || db_version == 5){
		rc = persist_chunk_base_বার্তা_read_v56(db_fptr, &chunk, দৈর্ঘ্য);
	}অন্যথা{
		rc = persist_chunk_base_বার্তা_read_v234(db_fptr, &chunk, db_version);
	}
	যদি(rc){
		ফেরত rc;
	}
	যদি(chunk.F.topic_len == 0){
		rc = MOSQ_ERR_INVAL;
		যাও cleanup;
	}

	যদি(chunk.F.source_port){
		জন্য(পূর্ণসংখ্যা i=0; i<db.config->listener_গুনতি; i++){
			যদি(db.config->listeners[i].port == chunk.F.source_port){
				chunk.source.listener = &db.config->listeners[i];
				বিরতি;
			}
		}
	}

	যদি(chunk.F.expiry_time > 0){
		message_expiry_interval64 = chunk.F.expiry_time - time(ফাঁকা);
		যদি(message_expiry_interval64 < 0 || message_expiry_interval64 > UINT32_MAX){
			/* Expired message */
			rc = MOSQ_ERR_SUCCESS;
			যাও cleanup;
		}অন্যথা{
			message_expiry_interval = (uint32_t)message_expiry_interval64;
		}
		p_message_expiry_interval = &message_expiry_interval;
	}

	base_বার্তা = mosquitto_calloc(1, আকার(কাঠামো mosquitto_base_বার্তা));
	যদি(base_বার্তা == ফাঁকা){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Out of memory.");
		rc = MOSQ_ERR_NOMEM;
		যাও cleanup;
	}

	base_বার্তা->তথ্য.store_id = chunk.F.store_id;
	base_বার্তা->তথ্য.source_mid = chunk.F.source_mid;
	base_বার্তা->তথ্য.topic = chunk.topic;
	base_বার্তা->তথ্য.qos = chunk.F.qos;
	base_বার্তা->তথ্য.payloadlen = chunk.F.payloadlen;
	base_বার্তা->তথ্য.retain = chunk.F.retain;
	base_বার্তা->তথ্য.properties = chunk.properties;
	base_বার্তা->তথ্য.payload = chunk.payload;
	base_বার্তা->source_listener = chunk.source.listener;

	rc = db__message_store(&chunk.source, base_বার্তা, p_message_expiry_interval,
			mosq_mo_client);

	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);

	যদি(rc == MOSQ_ERR_SUCCESS){
		base_বার্তা_গুনতি++;
		ফেরত MOSQ_ERR_SUCCESS;
	}অন্যথা{
		ফেরত rc;
	}
cleanup:
	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);
	mosquitto_FREE(chunk.topic);
	mosquitto_FREE(chunk.payload);
	ফেরত rc;
}


স্থির পূর্ণসংখ্যা persist__retain_chunk_restore(FILE *db_fptr)
{
	কাঠামো mosquitto_base_বার্তা *base_বার্তা;
	কাঠামো P_retain chunk;
	পূর্ণসংখ্যা rc;
	অক্ষর **split_topics;
	অক্ষর *local_topic;

	memset(&chunk, 0, আকার(কাঠামো P_retain));

	যদি(db_version == 6 || db_version == 5){
		rc = persist__chunk_retain_read_v56(db_fptr, &chunk);
	}অন্যথা{
		rc = persist__chunk_retain_read_v234(db_fptr, &chunk);
	}
	যদি(rc){
		ফেরত rc;
	}

	HASH_FIND(hh, db.বার্তা_store, &chunk.F.store_id, আকার(chunk.F.store_id), base_বার্তা);
	যদি(base_বার্তা){
		rc = sub__topic_tokenise(base_বার্তা->তথ্য.topic, &local_topic, &split_topics, ফাঁকা);
		যদি(rc){
			ফেরত rc;
		}
		retain__store(base_বার্তা->তথ্য.topic, base_বার্তা, split_topics, true);
		mosquitto_FREE(local_topic);
		mosquitto_FREE(split_topics);
		retained_গুনতি++;
	}অন্যথা{
		/* Can't find the message - probably expired */
	}
	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা persist__sub_chunk_restore(FILE *db_fptr)
{
	কাঠামো P_sub chunk;
	পূর্ণসংখ্যা rc;
	কাঠামো mosquitto_subscription sub;

	memset(&chunk, 0, আকার(কাঠামো P_sub));

	যদি(db_version == 6 || db_version == 5){
		rc = persist__chunk_sub_read_v56(db_fptr, &chunk);
	}অন্যথা{
		rc = persist__chunk_sub_read_v234(db_fptr, &chunk);
	}
	যদি(rc){
		ফেরত rc;
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

	ফেরত rc;
}


পূর্ণসংখ্যা persist__chunk_header_read(FILE *db_fptr, uint32_t *chunk, uint32_t *দৈর্ঘ্য)
{
	যদি(db_version == 6 || db_version == 5){
		ফেরত persist__chunk_header_read_v56(db_fptr, chunk, দৈর্ঘ্য);
	}অন্যথা{
		ফেরত persist__chunk_header_read_v234(db_fptr, chunk, দৈর্ঘ্য);
	}
}


পূর্ণসংখ্যা persist__restore(শূন্য)
{
	FILE *fptr;
	অক্ষর header[15];
	পূর্ণসংখ্যা rc = 0;
	uint32_t crc;
	uint32_t i32temp;
	uint32_t chunk, দৈর্ঘ্য;
	size_t rlen;
	অক্ষর *err;
	কাঠামো PF_cfg cfg_chunk;

	প্রতিজ্ঞা(db.config);

	যদি(!db.config->persistence || db.config->persistence_filepath == ফাঁকা){
		ফেরত MOSQ_ERR_SUCCESS;
	}

	db.বার্তা_store = ফাঁকা;
	base_বার্তা_গুনতি = 0;
	retained_গুনতি = 0;
	client_গুনতি = 0;
	subscription_গুনতি = 0;
	client_বার্তা_গুনতি = 0;

	fptr = mosquitto_fopen(db.config->persistence_filepath, "rb", true);
	যদি(fptr == ফাঁকা){
		ফেরত MOSQ_ERR_SUCCESS;
	}
	rlen = fread(&header, 1, 15, fptr);
	যদি(rlen == 0){
		fclose(fptr);
		log__printf(ফাঁকা, MOSQ_LOG_WARNING, "Warning: Persistence file is empty.");
		ফেরত 0;
	}অন্যথা যদি(rlen != 15){
		যাও error;
	}
	যদি(!memcmp(header, magic, 15)){
		/* Restore DB as normal */
		read_e(fptr, &crc, আকার(uint32_t));
		read_e(fptr, &i32temp, আকার(uint32_t));
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
				log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unsupported persistent database format version %d (need version %d).", db_version, MOSQ_DB_VERSION);
				ফেরত MOSQ_ERR_INVAL;
			}
		}

		যখন(persist__chunk_header_read(fptr, &chunk, &দৈর্ঘ্য) == MOSQ_ERR_SUCCESS){
			নির্বাচন(chunk){
				ক্ষেত্র DB_CHUNK_CFG:
					যদি(db_version == 6 || db_version == 5){
						rc = persist__chunk_cfg_read_v56(fptr, &cfg_chunk);
						যদি(rc){
							fclose(fptr);
							ফেরত rc;
						}
					}অন্যথা{
						rc = persist__chunk_cfg_read_v234(fptr, &cfg_chunk);
						যদি(rc){
							fclose(fptr);
							ফেরত rc;
						}
					}
					যদি(cfg_chunk.dbid_আকার != আকার(dbid_t)){
						log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Incompatible database configuration (dbid size is %d bytes, expected %lu)",
								cfg_chunk.dbid_আকার, (unsigned long)আকার(dbid_t));
						fclose(fptr);
						ফেরত MOSQ_ERR_INVAL;
					}
					db.last_db_id = cfg_chunk.last_db_id;
					বিরতি;

				ক্ষেত্র DB_CHUNK_BASE_বার্তা:
					rc = persist_base_বার্তা_chunk_restore(fptr, দৈর্ঘ্য);
					যদি(rc){
						fclose(fptr);
						ফেরত rc;
					}
					বিরতি;

				ক্ষেত্র DB_CHUNK_CLIENT_বার্তা:
					rc = persist_client_বার্তা_chunk_restore(fptr, দৈর্ঘ্য);
					যদি(rc){
						fclose(fptr);
						ফেরত rc;
					}
					বিরতি;

				ক্ষেত্র DB_CHUNK_RETAIN:
					rc = persist__retain_chunk_restore(fptr);
					যদি(rc){
						fclose(fptr);
						ফেরত rc;
					}
					বিরতি;

				ক্ষেত্র DB_CHUNK_SUB:
					rc = persist__sub_chunk_restore(fptr);
					যদি(rc){
						fclose(fptr);
						ফেরত rc;
					}
					বিরতি;

				ক্ষেত্র DB_CHUNK_CLIENT:
					rc = persist__client_chunk_restore(fptr);
					যদি(rc){
						fclose(fptr);
						ফেরত rc;
					}
					বিরতি;

				পূর্বনির্ধারিত:
					log__printf(ফাঁকা, MOSQ_LOG_WARNING, "Warning: Unsupported chunk \"%d\" in persistent database file. Ignoring.", chunk);
					যদি(fseek(fptr, দৈর্ঘ্য, SEEK_CUR) < 0){
						fclose(fptr);
						ফেরত MOSQ_ERR_INVAL;
					}
					বিরতি;
			}
		}
	}অন্যথা{
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to restore persistent database. Unrecognised file format.");
		rc = 1;
	}

	fclose(fptr);

	log__printf(ফাঁকা, MOSQ_LOG_INFO, "Restored %ld base messages", base_বার্তা_গুনতি);
	log__printf(ফাঁকা, MOSQ_LOG_INFO, "Restored %ld retained messages", retained_গুনতি);
	log__printf(ফাঁকা, MOSQ_LOG_INFO, "Restored %ld clients", client_গুনতি);
	log__printf(ফাঁকা, MOSQ_LOG_INFO, "Restored %ld subscriptions", subscription_গুনতি);
	log__printf(ফাঁকা, MOSQ_LOG_INFO, "Restored %ld client messages", client_বার্তা_গুনতি);

	ফেরত rc;
error:
	err = strerror(errno);
	log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: %s.", err);
	যদি(fptr){
		fclose(fptr);
	}
	ফেরত MOSQ_ERR_ERRNO;
}


স্থির পূর্ণসংখ্যা persist__restore_sub(ধ্রুবক কাঠামো mosquitto_subscription *sub)
{
	কাঠামো mosquitto *context;

	যদি(!sub->clientid){
		log__printf(ফাঁকা, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no client id, ignoring.");
		ফেরত MOSQ_ERR_SUCCESS;
	}
	যদি(!sub->topic_filter){
		log__printf(ফাঁকা, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no topic filter, ignoring.");
		ফেরত MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(sub->clientid, 0);
	যদি(!context){
		ফেরত MOSQ_ERR_INVAL;
	}
	ফেরত sub__add(context, sub);
}

#endif
