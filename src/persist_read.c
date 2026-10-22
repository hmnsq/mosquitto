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

#شامل "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#شامل <arpa/inet.h>
#endif
#شامل <assert.h>
#شامل <errno.h>
#شامل <fcntl.h>
#شامل <stdio.h>
#شامل <string.h>
#شامل <sys/stat.h>
#شامل <time.h>
#شامل <utlist.h>

#شامل "mosquitto_broker_internal.h"
#شامل "persist.h"
#شامل "util_mosq.h"

uint32_t db_version;

ثابت unsigned نویسه magic[15] = {0x00, 0xB5, 0x00, 'm', 'o', 's', 'q', 'u', 'i', 't', 't', 'o', ' ', 'd', 'b'};
ایستا long base_پیام_شمار = 0;
ایستا long retained_شمار = 0;
ایستا long client_شمار = 0;
ایستا long subscription_شمار = 0;
ایستا long client_پیام_شمار = 0;

ایستا صحیح persist__restore_sub(ثابت ساختار mosquitto_subscription *sub);

ایستا ساختار mosquitto *persist__find_or_add_context(ثابت نویسه *clientid, uint16_t last_mid)
{
	ساختار mosquitto *context;

	اگر(!clientid){
		بازگشت پوچ;
	}

	context = پوچ;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	اگر(!context){
		context = context__init();
		اگر(!context){
			بازگشت پوچ;
		}
		context->id = mosquitto_strdup(clientid);
		اگر(!context->id){
			mosquitto_FREE(context);
			بازگشت پوچ;
		}

		context->clean_start = false;

		context__add_to_by_id(context);
	}
	اگر(last_mid){
		context->last_mid = last_mid;
	}
	بازگشت context;
}


صحیح persist__read_string_len(FILE *db_fptr, نویسه **str, uint16_t len)
{
	نویسه *s = پوچ;

	اگر(len){
		s = mosquitto_malloc(len+1U);
		اگر(!s){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
			بازگشت MOSQ_ERR_NOMEM;
		}
		اگر(fread(s, 1, len, db_fptr) != len){
			mosquitto_FREE(s);
			بازگشت MOSQ_ERR_NOMEM;
		}
		s[len] = '\0';
	}

	*str = s;
	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح persist__read_string(FILE *db_fptr, نویسه **str)
{
	uint16_t i16temp;
	uint16_t slen;

	اگر(fread(&i16temp, 1, اندازه(uint16_t), db_fptr) != اندازه(uint16_t)){
		بازگشت MOSQ_ERR_INVAL;
	}

	slen = ntohs(i16temp);
	بازگشت persist__read_string_len(db_fptr, str, slen);
}


ایستا صحیح persist_client_پیام_restore(ساختار P_client_پیام *chunk)
{
	ساختار mosquitto_client_پیام *cmsg;
	ساختار mosquitto_base_پیام *پیام;
	ساختار mosquitto *context;
	ساختار mosquitto_پیام_data *پیام_data;

	HASH_FIND(hh, db.پیام_store, &chunk->F.store_id, اندازه(chunk->F.store_id), پیام);
	اگر(!پیام){
		/* Can't find message - probably expired */
		بازگشت MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk->clientid, 0);
	اگر(!context){
		log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Persistence file contains client message with no matching client. File may be corrupt.");
		بازگشت 0;
	}

	cmsg = mosquitto_calloc(1, اندازه(ساختار mosquitto_client_پیام));
	اگر(!cmsg){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
		بازگشت MOSQ_ERR_NOMEM;
	}

	cmsg->next = پوچ;
	cmsg->base_پیام = پوچ;
	cmsg->data.cmsg_id = ++context->last_cmsg_id;
	cmsg->data.mid = chunk->F.mid;
	cmsg->data.qos = chunk->F.qos;
	cmsg->data.retain = (chunk->F.retain_dup&0xF0)>>4;
	cmsg->data.direction = chunk->F.direction;
	cmsg->data.state = chunk->F.state;
	cmsg->data.dup = chunk->F.retain_dup&0x0F;
	cmsg->data.subscription_identifier = chunk->subscription_identifier;

	cmsg->base_پیام = پیام;
	db_پیام_store_ref_inc(cmsg->base_پیام);

	اگر(cmsg->data.direction == mosq_md_out){
		پیام_data = &context->msgs_out;
	}وگرنه{
		پیام_data = &context->msgs_in;
	}

	اگر(chunk->F.state == mosq_ms_queued || (chunk->F.qos > 0 && پیام_data->inflight_quota == 0)){
		DL_APPEND(پیام_data->queued, cmsg);
		db_پیام_add_to_queued_stats(پیام_data, cmsg);
	}وگرنه{
		DL_APPEND(پیام_data->inflight, cmsg);
		اگر(chunk->F.qos > 0 && پیام_data->inflight_quota > 0){
			پیام_data->inflight_quota--;
		}
		db_پیام_add_to_inflight_stats(پیام_data, cmsg);
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح persist__client_chunk_restore(FILE *db_fptr)
{
	صحیح rc = 0;
	ساختار mosquitto *context;
	ساختار P_client chunk;

	memset(&chunk, 0, اندازه(ساختار P_client));

	اگر(db_version == 6 || db_version == 5){
		rc = persist__chunk_client_read_v56(db_fptr, &chunk, db_version);
	}وگرنه{
		rc = persist__chunk_client_read_v234(db_fptr, &chunk, db_version);
	}
	اگر(rc > 0){
		بازگشت rc;
	}وگرنه اگر(rc < 0){
		/* Client not loaded, but otherwise not an error */
		log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Empty client entry found in persistence database, it may be corrupt.");
		بازگشت MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk.clientid, chunk.F.last_mid);
	اگر(context){
		context->session_expiry_time = chunk.F.session_expiry_time;
		context->session_expiry_interval = chunk.F.session_expiry_interval;
		اگر(chunk.username && !context->username){
			/* username is not freed here, it is now owned by context */
			context->username = chunk.username;
			chunk.username = پوچ;
		}
		/* in per_listener_settings mode, try to find the listener by persisted port */
		اگر(db.config->per_listener_settings && !context->listener && chunk.F.listener_port > 0){
			برای(صحیح i=0; i < db.config->listener_شمار; i++){
				اگر(db.config->listeners[i].port == chunk.F.listener_port){
					context->listener = &db.config->listeners[i];
					بشکن;
				}
			}
		}
		session_expiry__add_from_persistence(context, chunk.F.session_expiry_time);
	}وگرنه{
		rc = 1;
	}

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.username);
	اگر(rc == 0){
		client_شمار++;
	}
	بازگشت rc;
}


ایستا صحیح persist_client_پیام_chunk_restore(FILE *db_fptr, uint32_t length)
{
	ساختار P_client_پیام chunk;
	صحیح rc;

	memset(&chunk, 0, اندازه(ساختار P_client_پیام));

	اگر(db_version == 6 || db_version == 5){
		rc = persist_chunk_client_پیام_read_v56(db_fptr, &chunk, length);
	}وگرنه{
		rc = persist_chunk_client_پیام_read_v234(db_fptr, &chunk);
	}
	اگر(rc){
		بازگشت rc;
	}

	rc = persist_client_پیام_restore(&chunk);
	mosquitto_FREE(chunk.clientid);

	اگر(rc == 0){
		client_پیام_شمار++;
	}
	بازگشت rc;
}


ایستا صحیح persist_base_پیام_chunk_restore(FILE *db_fptr, uint32_t length)
{
	ساختار P_base_پیام chunk;
	ساختار mosquitto_base_پیام *base_پیام = پوچ;
	int64_t message_expiry_interval64;
	uint32_t message_expiry_interval;
	uint32_t *p_message_expiry_interval = پوچ;
	صحیح rc = 0;

	memset(&chunk, 0, اندازه(ساختار P_base_پیام));

	اگر(db_version == 6 || db_version == 5){
		rc = persist_chunk_base_پیام_read_v56(db_fptr, &chunk, length);
	}وگرنه{
		rc = persist_chunk_base_پیام_read_v234(db_fptr, &chunk, db_version);
	}
	اگر(rc){
		بازگشت rc;
	}
	اگر(chunk.F.topic_len == 0){
		rc = MOSQ_ERR_INVAL;
		برو cleanup;
	}

	اگر(chunk.F.source_port){
		برای(صحیح i=0; i<db.config->listener_شمار; i++){
			اگر(db.config->listeners[i].port == chunk.F.source_port){
				chunk.source.listener = &db.config->listeners[i];
				بشکن;
			}
		}
	}

	اگر(chunk.F.expiry_time > 0){
		message_expiry_interval64 = chunk.F.expiry_time - time(پوچ);
		اگر(message_expiry_interval64 < 0 || message_expiry_interval64 > UINT32_MAX){
			/* Expired message */
			rc = MOSQ_ERR_SUCCESS;
			برو cleanup;
		}وگرنه{
			message_expiry_interval = (uint32_t)message_expiry_interval64;
		}
		p_message_expiry_interval = &message_expiry_interval;
	}

	base_پیام = mosquitto_calloc(1, اندازه(ساختار mosquitto_base_پیام));
	اگر(base_پیام == پوچ){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
		rc = MOSQ_ERR_NOMEM;
		برو cleanup;
	}

	base_پیام->data.store_id = chunk.F.store_id;
	base_پیام->data.source_mid = chunk.F.source_mid;
	base_پیام->data.topic = chunk.topic;
	base_پیام->data.qos = chunk.F.qos;
	base_پیام->data.payloadlen = chunk.F.payloadlen;
	base_پیام->data.retain = chunk.F.retain;
	base_پیام->data.properties = chunk.properties;
	base_پیام->data.payload = chunk.payload;
	base_پیام->source_listener = chunk.source.listener;

	rc = db__message_store(&chunk.source, base_پیام, p_message_expiry_interval,
			mosq_mo_client);

	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);

	اگر(rc == MOSQ_ERR_SUCCESS){
		base_پیام_شمار++;
		بازگشت MOSQ_ERR_SUCCESS;
	}وگرنه{
		بازگشت rc;
	}
cleanup:
	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);
	mosquitto_FREE(chunk.topic);
	mosquitto_FREE(chunk.payload);
	بازگشت rc;
}


ایستا صحیح persist__retain_chunk_restore(FILE *db_fptr)
{
	ساختار mosquitto_base_پیام *base_پیام;
	ساختار P_retain chunk;
	صحیح rc;
	نویسه **split_topics;
	نویسه *local_topic;

	memset(&chunk, 0, اندازه(ساختار P_retain));

	اگر(db_version == 6 || db_version == 5){
		rc = persist__chunk_retain_read_v56(db_fptr, &chunk);
	}وگرنه{
		rc = persist__chunk_retain_read_v234(db_fptr, &chunk);
	}
	اگر(rc){
		بازگشت rc;
	}

	HASH_FIND(hh, db.پیام_store, &chunk.F.store_id, اندازه(chunk.F.store_id), base_پیام);
	اگر(base_پیام){
		rc = sub__topic_tokenise(base_پیام->data.topic, &local_topic, &split_topics, پوچ);
		اگر(rc){
			بازگشت rc;
		}
		retain__store(base_پیام->data.topic, base_پیام, split_topics, true);
		mosquitto_FREE(local_topic);
		mosquitto_FREE(split_topics);
		retained_شمار++;
	}وگرنه{
		/* Can't find the message - probably expired */
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح persist__sub_chunk_restore(FILE *db_fptr)
{
	ساختار P_sub chunk;
	صحیح rc;
	ساختار mosquitto_subscription sub;

	memset(&chunk, 0, اندازه(ساختار P_sub));

	اگر(db_version == 6 || db_version == 5){
		rc = persist__chunk_sub_read_v56(db_fptr, &chunk);
	}وگرنه{
		rc = persist__chunk_sub_read_v234(db_fptr, &chunk);
	}
	اگر(rc){
		بازگشت rc;
	}

	sub.clientid = chunk.clientid;
	sub.topic_filter = chunk.topic;
	sub.options = chunk.F.qos | chunk.F.options;
	sub.identifier = chunk.F.identifier;
	rc = persist__restore_sub(&sub);

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.topic);
	اگر(rc == 0){
		subscription_شمار++;
	}

	بازگشت rc;
}


صحیح persist__chunk_header_read(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	اگر(db_version == 6 || db_version == 5){
		بازگشت persist__chunk_header_read_v56(db_fptr, chunk, length);
	}وگرنه{
		بازگشت persist__chunk_header_read_v234(db_fptr, chunk, length);
	}
}


صحیح persist__restore(تهی)
{
	FILE *fptr;
	نویسه header[15];
	صحیح rc = 0;
	uint32_t crc;
	uint32_t i32temp;
	uint32_t chunk, length;
	size_t rlen;
	نویسه *err;
	ساختار PF_cfg cfg_chunk;

	أكد(db.config);

	اگر(!db.config->persistence || db.config->persistence_filepath == پوچ){
		بازگشت MOSQ_ERR_SUCCESS;
	}

	db.پیام_store = پوچ;
	base_پیام_شمار = 0;
	retained_شمار = 0;
	client_شمار = 0;
	subscription_شمار = 0;
	client_پیام_شمار = 0;

	fptr = mosquitto_fopen(db.config->persistence_filepath, "rb", true);
	اگر(fptr == پوچ){
		بازگشت MOSQ_ERR_SUCCESS;
	}
	rlen = fread(&header, 1, 15, fptr);
	اگر(rlen == 0){
		fclose(fptr);
		log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Persistence file is empty.");
		بازگشت 0;
	}وگرنه اگر(rlen != 15){
		برو error;
	}
	اگر(!memcmp(header, magic, 15)){
		/* Restore DB as normal */
		read_e(fptr, &crc, اندازه(uint32_t));
		read_e(fptr, &i32temp, اندازه(uint32_t));
		db_version = ntohl(i32temp);
		/* IMPORTANT - this is where compatibility checks are made.
		 * Is your DB change still compatible with previous versions?
		 */
		اگر(db_version != MOSQ_DB_VERSION){
			اگر(db_version == 5){
				/* Addition of username and listener_port to client chunk in v6 */
			}وگرنه اگر(db_version == 4){
			}وگرنه اگر(db_version == 3){
				/* Addition of source_username and source_port to msg_store chunk in v4, v1.5.6 */
			}وگرنه اگر(db_version == 2){
				/* Addition of disconnect_t to client chunk in v3. */
			}وگرنه{
				fclose(fptr);
				log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unsupported persistent database format version %d (need version %d).", db_version, MOSQ_DB_VERSION);
				بازگشت MOSQ_ERR_INVAL;
			}
		}

		وقتی(persist__chunk_header_read(fptr, &chunk, &length) == MOSQ_ERR_SUCCESS){
			انتخاب(chunk){
				حالت DB_CHUNK_CFG:
					اگر(db_version == 6 || db_version == 5){
						rc = persist__chunk_cfg_read_v56(fptr, &cfg_chunk);
						اگر(rc){
							fclose(fptr);
							بازگشت rc;
						}
					}وگرنه{
						rc = persist__chunk_cfg_read_v234(fptr, &cfg_chunk);
						اگر(rc){
							fclose(fptr);
							بازگشت rc;
						}
					}
					اگر(cfg_chunk.dbid_size != اندازه(dbid_t)){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Incompatible database configuration (dbid size is %d bytes, expected %lu)",
								cfg_chunk.dbid_size, (unsigned long)اندازه(dbid_t));
						fclose(fptr);
						بازگشت MOSQ_ERR_INVAL;
					}
					db.last_db_id = cfg_chunk.last_db_id;
					بشکن;

				حالت DB_CHUNK_BASE_پیام:
					rc = persist_base_پیام_chunk_restore(fptr, length);
					اگر(rc){
						fclose(fptr);
						بازگشت rc;
					}
					بشکن;

				حالت DB_CHUNK_CLIENT_پیام:
					rc = persist_client_پیام_chunk_restore(fptr, length);
					اگر(rc){
						fclose(fptr);
						بازگشت rc;
					}
					بشکن;

				حالت DB_CHUNK_RETAIN:
					rc = persist__retain_chunk_restore(fptr);
					اگر(rc){
						fclose(fptr);
						بازگشت rc;
					}
					بشکن;

				حالت DB_CHUNK_SUB:
					rc = persist__sub_chunk_restore(fptr);
					اگر(rc){
						fclose(fptr);
						بازگشت rc;
					}
					بشکن;

				حالت DB_CHUNK_CLIENT:
					rc = persist__client_chunk_restore(fptr);
					اگر(rc){
						fclose(fptr);
						بازگشت rc;
					}
					بشکن;

				پیشفرض:
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Unsupported chunk \"%d\" in persistent database file. Ignoring.", chunk);
					اگر(fseek(fptr, length, SEEK_CUR) < 0){
						fclose(fptr);
						بازگشت MOSQ_ERR_INVAL;
					}
					بشکن;
			}
		}
	}وگرنه{
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to restore persistent database. Unrecognised file format.");
		rc = 1;
	}

	fclose(fptr);

	log__printf(پوچ, MOSQ_LOG_INFO, "Restored %ld base messages", base_پیام_شمار);
	log__printf(پوچ, MOSQ_LOG_INFO, "Restored %ld retained messages", retained_شمار);
	log__printf(پوچ, MOSQ_LOG_INFO, "Restored %ld clients", client_شمار);
	log__printf(پوچ, MOSQ_LOG_INFO, "Restored %ld subscriptions", subscription_شمار);
	log__printf(پوچ, MOSQ_LOG_INFO, "Restored %ld client messages", client_پیام_شمار);

	بازگشت rc;
error:
	err = strerror(errno);
	log__printf(پوچ, MOSQ_LOG_ERR, "Error: %s.", err);
	اگر(fptr){
		fclose(fptr);
	}
	بازگشت MOSQ_ERR_ERRNO;
}


ایستا صحیح persist__restore_sub(ثابت ساختار mosquitto_subscription *sub)
{
	ساختار mosquitto *context;

	اگر(!sub->clientid){
		log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no client id, ignoring.");
		بازگشت MOSQ_ERR_SUCCESS;
	}
	اگر(!sub->topic_filter){
		log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no topic filter, ignoring.");
		بازگشت MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(sub->clientid, 0);
	اگر(!context){
		بازگشت MOSQ_ERR_INVAL;
	}
	بازگشت sub__add(context, sub);
}

#endif
