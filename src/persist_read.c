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

#अन्तर्भाव "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#अन्तर्भाव <arpa/inet.h>
#endif
#अन्तर्भाव <assert.h>
#अन्तर्भाव <errno.h>
#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <sys/stat.h>
#अन्तर्भाव <time.h>
#अन्तर्भाव <utlist.h>

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "persist.h"
#अन्तर्भाव "util_mosq.h"

uint32_t db_version;

ध्रुव unsigned वर्ण magic[15] = {0x00, 0xB5, 0x00, 'm', 'o', 's', 'q', 'u', 'i', 't', 't', 'o', ' ', 'd', 'b'};
स्थिर long base_सन्देशः_गणनम् = 0;
स्थिर long retained_गणनम् = 0;
स्थिर long client_गणनम् = 0;
स्थिर long subscription_गणनम् = 0;
स्थिर long client_सन्देशः_गणनम् = 0;

स्थिर पूर्णाङ्क persist__restore_sub(ध्रुव संरचना mosquitto_subscription *sub);

स्थिर संरचना mosquitto *persist__find_or_add_context(ध्रुव वर्ण *clientid, uint16_t last_mid)
{
	संरचना mosquitto *context;

	जइ(!clientid){
		प्रत्यागम रिक्त;
	}

	context = रिक्त;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	जइ(!context){
		context = context__init();
		जइ(!context){
			प्रत्यागम रिक्त;
		}
		context->id = mosquitto_strdup(clientid);
		जइ(!context->id){
			mosquitto_FREE(context);
			प्रत्यागम रिक्त;
		}

		context->clean_start = false;

		context__add_to_by_id(context);
	}
	जइ(last_mid){
		context->last_mid = last_mid;
	}
	प्रत्यागम context;
}


पूर्णाङ्क persist__read_string_len(FILE *db_fptr, वर्ण **str, uint16_t len)
{
	वर्ण *s = रिक्त;

	जइ(len){
		s = mosquitto_malloc(len+1U);
		जइ(!s){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
		जइ(fread(s, 1, len, db_fptr) != len){
			mosquitto_FREE(s);
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
		s[len] = '\0';
	}

	*str = s;
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क persist__read_string(FILE *db_fptr, वर्ण **str)
{
	uint16_t i16temp;
	uint16_t slen;

	जइ(fread(&i16temp, 1, परिमाण(uint16_t), db_fptr) != परिमाण(uint16_t)){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	slen = ntohs(i16temp);
	प्रत्यागम persist__read_string_len(db_fptr, str, slen);
}


स्थिर पूर्णाङ्क persist_client_सन्देशः_restore(संरचना P_client_सन्देशः *chunk)
{
	संरचना mosquitto_client_सन्देशः *cmsg;
	संरचना mosquitto_base_सन्देशः *सन्देशः;
	संरचना mosquitto *context;
	संरचना mosquitto_सन्देशः_data *सन्देशः_data;

	HASH_FIND(hh, db.सन्देशः_store, &chunk->F.store_id, परिमाण(chunk->F.store_id), सन्देशः);
	जइ(!सन्देशः){
		/* Can't find message - probably expired */
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk->clientid, 0);
	जइ(!context){
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Persistence file contains client message with no matching client. File may be corrupt.");
		प्रत्यागम 0;
	}

	cmsg = mosquitto_calloc(1, परिमाण(संरचना mosquitto_client_सन्देशः));
	जइ(!cmsg){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	cmsg->next = रिक्त;
	cmsg->base_सन्देशः = रिक्त;
	cmsg->data.cmsg_id = ++context->last_cmsg_id;
	cmsg->data.mid = chunk->F.mid;
	cmsg->data.qos = chunk->F.qos;
	cmsg->data.retain = (chunk->F.retain_dup&0xF0)>>4;
	cmsg->data.direction = chunk->F.direction;
	cmsg->data.state = chunk->F.state;
	cmsg->data.dup = chunk->F.retain_dup&0x0F;
	cmsg->data.subscription_identifier = chunk->subscription_identifier;

	cmsg->base_सन्देशः = सन्देशः;
	db_सन्देशः_store_ref_inc(cmsg->base_सन्देशः);

	जइ(cmsg->data.direction == mosq_md_out){
		सन्देशः_data = &context->msgs_out;
	}अण्णहा{
		सन्देशः_data = &context->msgs_in;
	}

	जइ(chunk->F.state == mosq_ms_queued || (chunk->F.qos > 0 && सन्देशः_data->inflight_quota == 0)){
		DL_APPEND(सन्देशः_data->queued, cmsg);
		db_सन्देशः_add_to_queued_stats(सन्देशः_data, cmsg);
	}अण्णहा{
		DL_APPEND(सन्देशः_data->inflight, cmsg);
		जइ(chunk->F.qos > 0 && सन्देशः_data->inflight_quota > 0){
			सन्देशः_data->inflight_quota--;
		}
		db_सन्देशः_add_to_inflight_stats(सन्देशः_data, cmsg);
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क persist__client_chunk_restore(FILE *db_fptr)
{
	पूर्णाङ्क rc = 0;
	संरचना mosquitto *context;
	संरचना P_client chunk;

	memset(&chunk, 0, परिमाण(संरचना P_client));

	जइ(db_version == 6 || db_version == 5){
		rc = persist__chunk_client_read_v56(db_fptr, &chunk, db_version);
	}अण्णहा{
		rc = persist__chunk_client_read_v234(db_fptr, &chunk, db_version);
	}
	जइ(rc > 0){
		प्रत्यागम rc;
	}अण्णहा जइ(rc < 0){
		/* Client not loaded, but otherwise not an error */
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Empty client entry found in persistence database, it may be corrupt.");
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk.clientid, chunk.F.last_mid);
	जइ(context){
		context->session_expiry_time = chunk.F.session_expiry_time;
		context->session_expiry_interval = chunk.F.session_expiry_interval;
		जइ(chunk.username && !context->username){
			/* username is not freed here, it is now owned by context */
			context->username = chunk.username;
			chunk.username = रिक्त;
		}
		/* in per_listener_settings mode, try to find the listener by persisted port */
		जइ(db.config->per_listener_settings && !context->listener && chunk.F.listener_port > 0){
			प्रति(पूर्णाङ्क i=0; i < db.config->listener_गणनम्; i++){
				जइ(db.config->listeners[i].port == chunk.F.listener_port){
					context->listener = &db.config->listeners[i];
					भङ्ग;
				}
			}
		}
		session_expiry__add_from_persistence(context, chunk.F.session_expiry_time);
	}अण्णहा{
		rc = 1;
	}

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.username);
	जइ(rc == 0){
		client_गणनम्++;
	}
	प्रत्यागम rc;
}


स्थिर पूर्णाङ्क persist_client_सन्देशः_chunk_restore(FILE *db_fptr, uint32_t length)
{
	संरचना P_client_सन्देशः chunk;
	पूर्णाङ्क rc;

	memset(&chunk, 0, परिमाण(संरचना P_client_सन्देशः));

	जइ(db_version == 6 || db_version == 5){
		rc = persist_chunk_client_सन्देशः_read_v56(db_fptr, &chunk, length);
	}अण्णहा{
		rc = persist_chunk_client_सन्देशः_read_v234(db_fptr, &chunk);
	}
	जइ(rc){
		प्रत्यागम rc;
	}

	rc = persist_client_सन्देशः_restore(&chunk);
	mosquitto_FREE(chunk.clientid);

	जइ(rc == 0){
		client_सन्देशः_गणनम्++;
	}
	प्रत्यागम rc;
}


स्थिर पूर्णाङ्क persist_base_सन्देशः_chunk_restore(FILE *db_fptr, uint32_t length)
{
	संरचना P_base_सन्देशः chunk;
	संरचना mosquitto_base_सन्देशः *base_सन्देशः = रिक्त;
	int64_t message_expiry_interval64;
	uint32_t message_expiry_interval;
	uint32_t *p_message_expiry_interval = रिक्त;
	पूर्णाङ्क rc = 0;

	memset(&chunk, 0, परिमाण(संरचना P_base_सन्देशः));

	जइ(db_version == 6 || db_version == 5){
		rc = persist_chunk_base_सन्देशः_read_v56(db_fptr, &chunk, length);
	}अण्णहा{
		rc = persist_chunk_base_सन्देशः_read_v234(db_fptr, &chunk, db_version);
	}
	जइ(rc){
		प्रत्यागम rc;
	}
	जइ(chunk.F.topic_len == 0){
		rc = MOSQ_ERR_INVAL;
		गच्छ cleanup;
	}

	जइ(chunk.F.source_port){
		प्रति(पूर्णाङ्क i=0; i<db.config->listener_गणनम्; i++){
			जइ(db.config->listeners[i].port == chunk.F.source_port){
				chunk.source.listener = &db.config->listeners[i];
				भङ्ग;
			}
		}
	}

	जइ(chunk.F.expiry_time > 0){
		message_expiry_interval64 = chunk.F.expiry_time - time(रिक्त);
		जइ(message_expiry_interval64 < 0 || message_expiry_interval64 > UINT32_MAX){
			/* Expired message */
			rc = MOSQ_ERR_SUCCESS;
			गच्छ cleanup;
		}अण्णहा{
			message_expiry_interval = (uint32_t)message_expiry_interval64;
		}
		p_message_expiry_interval = &message_expiry_interval;
	}

	base_सन्देशः = mosquitto_calloc(1, परिमाण(संरचना mosquitto_base_सन्देशः));
	जइ(base_सन्देशः == रिक्त){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		rc = MOSQ_ERR_NOMEM;
		गच्छ cleanup;
	}

	base_सन्देशः->data.store_id = chunk.F.store_id;
	base_सन्देशः->data.source_mid = chunk.F.source_mid;
	base_सन्देशः->data.topic = chunk.topic;
	base_सन्देशः->data.qos = chunk.F.qos;
	base_सन्देशः->data.payloadlen = chunk.F.payloadlen;
	base_सन्देशः->data.retain = chunk.F.retain;
	base_सन्देशः->data.properties = chunk.properties;
	base_सन्देशः->data.payload = chunk.payload;
	base_सन्देशः->source_listener = chunk.source.listener;

	rc = db__message_store(&chunk.source, base_सन्देशः, p_message_expiry_interval,
			mosq_mo_client);

	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);

	जइ(rc == MOSQ_ERR_SUCCESS){
		base_सन्देशः_गणनम्++;
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}अण्णहा{
		प्रत्यागम rc;
	}
cleanup:
	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);
	mosquitto_FREE(chunk.topic);
	mosquitto_FREE(chunk.payload);
	प्रत्यागम rc;
}


स्थिर पूर्णाङ्क persist__retain_chunk_restore(FILE *db_fptr)
{
	संरचना mosquitto_base_सन्देशः *base_सन्देशः;
	संरचना P_retain chunk;
	पूर्णाङ्क rc;
	वर्ण **split_topics;
	वर्ण *local_topic;

	memset(&chunk, 0, परिमाण(संरचना P_retain));

	जइ(db_version == 6 || db_version == 5){
		rc = persist__chunk_retain_read_v56(db_fptr, &chunk);
	}अण्णहा{
		rc = persist__chunk_retain_read_v234(db_fptr, &chunk);
	}
	जइ(rc){
		प्रत्यागम rc;
	}

	HASH_FIND(hh, db.सन्देशः_store, &chunk.F.store_id, परिमाण(chunk.F.store_id), base_सन्देशः);
	जइ(base_सन्देशः){
		rc = sub__topic_tokenise(base_सन्देशः->data.topic, &local_topic, &split_topics, रिक्त);
		जइ(rc){
			प्रत्यागम rc;
		}
		retain__store(base_सन्देशः->data.topic, base_सन्देशः, split_topics, true);
		mosquitto_FREE(local_topic);
		mosquitto_FREE(split_topics);
		retained_गणनम्++;
	}अण्णहा{
		/* Can't find the message - probably expired */
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क persist__sub_chunk_restore(FILE *db_fptr)
{
	संरचना P_sub chunk;
	पूर्णाङ्क rc;
	संरचना mosquitto_subscription sub;

	memset(&chunk, 0, परिमाण(संरचना P_sub));

	जइ(db_version == 6 || db_version == 5){
		rc = persist__chunk_sub_read_v56(db_fptr, &chunk);
	}अण्णहा{
		rc = persist__chunk_sub_read_v234(db_fptr, &chunk);
	}
	जइ(rc){
		प्रत्यागम rc;
	}

	sub.clientid = chunk.clientid;
	sub.topic_filter = chunk.topic;
	sub.options = chunk.F.qos | chunk.F.options;
	sub.identifier = chunk.F.identifier;
	rc = persist__restore_sub(&sub);

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.topic);
	जइ(rc == 0){
		subscription_गणनम्++;
	}

	प्रत्यागम rc;
}


पूर्णाङ्क persist__chunk_header_read(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	जइ(db_version == 6 || db_version == 5){
		प्रत्यागम persist__chunk_header_read_v56(db_fptr, chunk, length);
	}अण्णहा{
		प्रत्यागम persist__chunk_header_read_v234(db_fptr, chunk, length);
	}
}


पूर्णाङ्क persist__restore(शून्य)
{
	FILE *fptr;
	वर्ण header[15];
	पूर्णाङ्क rc = 0;
	uint32_t crc;
	uint32_t i32temp;
	uint32_t chunk, length;
	size_t rlen;
	वर्ण *err;
	संरचना PF_cfg cfg_chunk;

	प्रतिज्ञा(db.config);

	जइ(!db.config->persistence || db.config->persistence_filepath == रिक्त){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	db.सन्देशः_store = रिक्त;
	base_सन्देशः_गणनम् = 0;
	retained_गणनम् = 0;
	client_गणनम् = 0;
	subscription_गणनम् = 0;
	client_सन्देशः_गणनम् = 0;

	fptr = mosquitto_fopen(db.config->persistence_filepath, "rb", true);
	जइ(fptr == रिक्त){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}
	rlen = fread(&header, 1, 15, fptr);
	जइ(rlen == 0){
		fclose(fptr);
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Persistence file is empty.");
		प्रत्यागम 0;
	}अण्णहा जइ(rlen != 15){
		गच्छ error;
	}
	जइ(!memcmp(header, magic, 15)){
		/* Restore DB as normal */
		read_e(fptr, &crc, परिमाण(uint32_t));
		read_e(fptr, &i32temp, परिमाण(uint32_t));
		db_version = ntohl(i32temp);
		/* IMPORTANT - this is where compatibility checks are made.
		 * Is your DB change still compatible with previous versions?
		 */
		जइ(db_version != MOSQ_DB_VERSION){
			जइ(db_version == 5){
				/* Addition of username and listener_port to client chunk in v6 */
			}अण्णहा जइ(db_version == 4){
			}अण्णहा जइ(db_version == 3){
				/* Addition of source_username and source_port to msg_store chunk in v4, v1.5.6 */
			}अण्णहा जइ(db_version == 2){
				/* Addition of disconnect_t to client chunk in v3. */
			}अण्णहा{
				fclose(fptr);
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unsupported persistent database format version %d (need version %d).", db_version, MOSQ_DB_VERSION);
				प्रत्यागम MOSQ_ERR_INVAL;
			}
		}

		जाव(persist__chunk_header_read(fptr, &chunk, &length) == MOSQ_ERR_SUCCESS){
			विकल्प(chunk){
				पक्ष DB_CHUNK_CFG:
					जइ(db_version == 6 || db_version == 5){
						rc = persist__chunk_cfg_read_v56(fptr, &cfg_chunk);
						जइ(rc){
							fclose(fptr);
							प्रत्यागम rc;
						}
					}अण्णहा{
						rc = persist__chunk_cfg_read_v234(fptr, &cfg_chunk);
						जइ(rc){
							fclose(fptr);
							प्रत्यागम rc;
						}
					}
					जइ(cfg_chunk.dbid_size != परिमाण(dbid_t)){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Incompatible database configuration (dbid size is %d bytes, expected %lu)",
								cfg_chunk.dbid_size, (unsigned long)परिमाण(dbid_t));
						fclose(fptr);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					db.last_db_id = cfg_chunk.last_db_id;
					भङ्ग;

				पक्ष DB_CHUNK_BASE_सन्देशः:
					rc = persist_base_सन्देशः_chunk_restore(fptr, length);
					जइ(rc){
						fclose(fptr);
						प्रत्यागम rc;
					}
					भङ्ग;

				पक्ष DB_CHUNK_CLIENT_सन्देशः:
					rc = persist_client_सन्देशः_chunk_restore(fptr, length);
					जइ(rc){
						fclose(fptr);
						प्रत्यागम rc;
					}
					भङ्ग;

				पक्ष DB_CHUNK_RETAIN:
					rc = persist__retain_chunk_restore(fptr);
					जइ(rc){
						fclose(fptr);
						प्रत्यागम rc;
					}
					भङ्ग;

				पक्ष DB_CHUNK_SUB:
					rc = persist__sub_chunk_restore(fptr);
					जइ(rc){
						fclose(fptr);
						प्रत्यागम rc;
					}
					भङ्ग;

				पक्ष DB_CHUNK_CLIENT:
					rc = persist__client_chunk_restore(fptr);
					जइ(rc){
						fclose(fptr);
						प्रत्यागम rc;
					}
					भङ्ग;

				स्वभाव:
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Unsupported chunk \"%d\" in persistent database file. Ignoring.", chunk);
					जइ(fseek(fptr, length, SEEK_CUR) < 0){
						fclose(fptr);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					भङ्ग;
			}
		}
	}अण्णहा{
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to restore persistent database. Unrecognised file format.");
		rc = 1;
	}

	fclose(fptr);

	log__printf(रिक्त, MOSQ_LOG_INFO, "Restored %ld base messages", base_सन्देशः_गणनम्);
	log__printf(रिक्त, MOSQ_LOG_INFO, "Restored %ld retained messages", retained_गणनम्);
	log__printf(रिक्त, MOSQ_LOG_INFO, "Restored %ld clients", client_गणनम्);
	log__printf(रिक्त, MOSQ_LOG_INFO, "Restored %ld subscriptions", subscription_गणनम्);
	log__printf(रिक्त, MOSQ_LOG_INFO, "Restored %ld client messages", client_सन्देशः_गणनम्);

	प्रत्यागम rc;
error:
	err = strerror(errno);
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s.", err);
	जइ(fptr){
		fclose(fptr);
	}
	प्रत्यागम MOSQ_ERR_ERRNO;
}


स्थिर पूर्णाङ्क persist__restore_sub(ध्रुव संरचना mosquitto_subscription *sub)
{
	संरचना mosquitto *context;

	जइ(!sub->clientid){
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no client id, ignoring.");
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}
	जइ(!sub->topic_filter){
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no topic filter, ignoring.");
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(sub->clientid, 0);
	जइ(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	प्रत्यागम sub__add(context, sub);
}

#endif
