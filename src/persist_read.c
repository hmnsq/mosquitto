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

अचर अचिन्हित वर्ण magic[15] = {0x00, 0xB5, 0x00, 'm', 'o', 's', 'q', 'u', 'i', 't', 't', 'o', ' ', 'd', 'b'};
स्थिर दीर्घ base_सन्देश_गन्ती = 0;
स्थिर दीर्घ retained_गन्ती = 0;
स्थिर दीर्घ client_गन्ती = 0;
स्थिर दीर्घ subscription_गन्ती = 0;
स्थिर दीर्घ client_सन्देश_गन्ती = 0;

स्थिर पूर्णाङ्क persist__restore_sub(अचर संरचना mosquitto_subscription *sub);

स्थिर संरचना mosquitto *persist__find_or_add_context(अचर वर्ण *clientid, uint16_t last_mid)
{
	संरचना mosquitto *context;

	यदि(!clientid){
		फर्काऊ रिक्त;
	}

	context = रिक्त;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, म_माप(clientid), context);
	यदि(!context){
		context = context__init();
		यदि(!context){
			फर्काऊ रिक्त;
		}
		context->id = mosquitto_strdup(clientid);
		यदि(!context->id){
			mosquitto_FREE(context);
			फर्काऊ रिक्त;
		}

		context->clean_start = false;

		context__add_to_by_id(context);
	}
	यदि(last_mid){
		context->last_mid = last_mid;
	}
	फर्काऊ context;
}


पूर्णाङ्क persist__read_string_len(खाता *db_fptr, वर्ण **str, uint16_t len)
{
	वर्ण *s = रिक्त;

	यदि(len){
		s = mosquitto_malloc(len+1U);
		यदि(!s){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
			फर्काऊ MOSQ_ERR_NOMEM;
		}
		यदि(ख_पढ़ो(s, 1, len, db_fptr) != len){
			mosquitto_FREE(s);
			फर्काऊ MOSQ_ERR_NOMEM;
		}
		s[len] = '\0';
	}

	*str = s;
	फर्काऊ MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क persist__read_string(खाता *db_fptr, वर्ण **str)
{
	uint16_t i16temp;
	uint16_t slen;

	यदि(ख_पढ़ो(&i16temp, 1, परिमाण(uint16_t), db_fptr) != परिमाण(uint16_t)){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	slen = ntohs(i16temp);
	फर्काऊ persist__read_string_len(db_fptr, str, slen);
}


स्थिर पूर्णाङ्क persist_client_सन्देश_restore(संरचना P_client_सन्देश *chunk)
{
	संरचना mosquitto_client_सन्देश *cmsg;
	संरचना mosquitto_base_सन्देश *सन्देश;
	संरचना mosquitto *context;
	संरचना mosquitto_सन्देश_data *सन्देश_data;

	HASH_FIND(hh, db.सन्देश_store, &chunk->F.store_id, परिमाण(chunk->F.store_id), सन्देश);
	यदि(!सन्देश){
		/* Can't find message - probably expired */
		फर्काऊ MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk->clientid, 0);
	यदि(!context){
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Persistence file contains client message with no matching client. File may be corrupt.");
		फर्काऊ 0;
	}

	cmsg = mosquitto_calloc(1, परिमाण(संरचना mosquitto_client_सन्देश));
	यदि(!cmsg){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		फर्काऊ MOSQ_ERR_NOMEM;
	}

	cmsg->next = रिक्त;
	cmsg->base_सन्देश = रिक्त;
	cmsg->data.cmsg_id = ++context->last_cmsg_id;
	cmsg->data.mid = chunk->F.mid;
	cmsg->data.qos = chunk->F.qos;
	cmsg->data.retain = (chunk->F.retain_dup&0xF0)>>4;
	cmsg->data.direction = chunk->F.direction;
	cmsg->data.state = chunk->F.state;
	cmsg->data.dup = chunk->F.retain_dup&0x0F;
	cmsg->data.subscription_identifier = chunk->subscription_identifier;

	cmsg->base_सन्देश = सन्देश;
	db_सन्देश_store_ref_inc(cmsg->base_सन्देश);

	यदि(cmsg->data.direction == mosq_md_out){
		सन्देश_data = &context->msgs_out;
	}अन्यथा{
		सन्देश_data = &context->msgs_in;
	}

	यदि(chunk->F.state == mosq_ms_queued || (chunk->F.qos > 0 && सन्देश_data->inflight_quota == 0)){
		DL_APPEND(सन्देश_data->queued, cmsg);
		db_सन्देश_add_to_queued_stats(सन्देश_data, cmsg);
	}अन्यथा{
		DL_APPEND(सन्देश_data->inflight, cmsg);
		यदि(chunk->F.qos > 0 && सन्देश_data->inflight_quota > 0){
			सन्देश_data->inflight_quota--;
		}
		db_सन्देश_add_to_inflight_stats(सन्देश_data, cmsg);
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क persist__client_chunk_restore(खाता *db_fptr)
{
	पूर्णाङ्क rc = 0;
	संरचना mosquitto *context;
	संरचना P_client chunk;

	स_रखो(&chunk, 0, परिमाण(संरचना P_client));

	यदि(db_version == 6 || db_version == 5){
		rc = persist__chunk_client_read_v56(db_fptr, &chunk, db_version);
	}अन्यथा{
		rc = persist__chunk_client_read_v234(db_fptr, &chunk, db_version);
	}
	यदि(rc > 0){
		फर्काऊ rc;
	}अन्यथा यदि(rc < 0){
		/* Client not loaded, but otherwise not an error */
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Empty client entry found in persistence database, it may be corrupt.");
		फर्काऊ MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk.clientid, chunk.F.last_mid);
	यदि(context){
		context->session_expiry_time = chunk.F.session_expiry_time;
		context->session_expiry_interval = chunk.F.session_expiry_interval;
		यदि(chunk.username && !context->username){
			/* username is not freed here, it is now owned by context */
			context->username = chunk.username;
			chunk.username = रिक्त;
		}
		/* in per_listener_settings mode, try to find the listener by persisted port */
		यदि(db.config->per_listener_settings && !context->listener && chunk.F.listener_port > 0){
			लागि(पूर्णाङ्क i=0; i < db.config->listener_गन्ती; i++){
				यदि(db.config->listeners[i].port == chunk.F.listener_port){
					context->listener = &db.config->listeners[i];
					रोक;
				}
			}
		}
		session_expiry__add_from_persistence(context, chunk.F.session_expiry_time);
	}अन्यथा{
		rc = 1;
	}

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.username);
	यदि(rc == 0){
		client_गन्ती++;
	}
	फर्काऊ rc;
}


स्थिर पूर्णाङ्क persist_client_सन्देश_chunk_restore(खाता *db_fptr, uint32_t length)
{
	संरचना P_client_सन्देश chunk;
	पूर्णाङ्क rc;

	स_रखो(&chunk, 0, परिमाण(संरचना P_client_सन्देश));

	यदि(db_version == 6 || db_version == 5){
		rc = persist_chunk_client_सन्देश_read_v56(db_fptr, &chunk, length);
	}अन्यथा{
		rc = persist_chunk_client_सन्देश_read_v234(db_fptr, &chunk);
	}
	यदि(rc){
		फर्काऊ rc;
	}

	rc = persist_client_सन्देश_restore(&chunk);
	mosquitto_FREE(chunk.clientid);

	यदि(rc == 0){
		client_सन्देश_गन्ती++;
	}
	फर्काऊ rc;
}


स्थिर पूर्णाङ्क persist_base_सन्देश_chunk_restore(खाता *db_fptr, uint32_t length)
{
	संरचना P_base_सन्देश chunk;
	संरचना mosquitto_base_सन्देश *base_सन्देश = रिक्त;
	int64_t message_expiry_interval64;
	uint32_t message_expiry_interval;
	uint32_t *p_message_expiry_interval = रिक्त;
	पूर्णाङ्क rc = 0;

	स_रखो(&chunk, 0, परिमाण(संरचना P_base_सन्देश));

	यदि(db_version == 6 || db_version == 5){
		rc = persist_chunk_base_सन्देश_read_v56(db_fptr, &chunk, length);
	}अन्यथा{
		rc = persist_chunk_base_सन्देश_read_v234(db_fptr, &chunk, db_version);
	}
	यदि(rc){
		फर्काऊ rc;
	}
	यदि(chunk.F.topic_len == 0){
		rc = MOSQ_ERR_INVAL;
		गच्छ cleanup;
	}

	यदि(chunk.F.source_port){
		लागि(पूर्णाङ्क i=0; i<db.config->listener_गन्ती; i++){
			यदि(db.config->listeners[i].port == chunk.F.source_port){
				chunk.source.listener = &db.config->listeners[i];
				रोक;
			}
		}
	}

	यदि(chunk.F.expiry_time > 0){
		message_expiry_interval64 = chunk.F.expiry_time - समय(रिक्त);
		यदि(message_expiry_interval64 < 0 || message_expiry_interval64 > UINT32_MAX){
			/* Expired message */
			rc = MOSQ_ERR_SUCCESS;
			गच्छ cleanup;
		}अन्यथा{
			message_expiry_interval = (uint32_t)message_expiry_interval64;
		}
		p_message_expiry_interval = &message_expiry_interval;
	}

	base_सन्देश = mosquitto_calloc(1, परिमाण(संरचना mosquitto_base_सन्देश));
	यदि(base_सन्देश == रिक्त){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		rc = MOSQ_ERR_NOMEM;
		गच्छ cleanup;
	}

	base_सन्देश->data.store_id = chunk.F.store_id;
	base_सन्देश->data.source_mid = chunk.F.source_mid;
	base_सन्देश->data.topic = chunk.topic;
	base_सन्देश->data.qos = chunk.F.qos;
	base_सन्देश->data.payloadlen = chunk.F.payloadlen;
	base_सन्देश->data.retain = chunk.F.retain;
	base_सन्देश->data.properties = chunk.properties;
	base_सन्देश->data.payload = chunk.payload;
	base_सन्देश->source_listener = chunk.source.listener;

	rc = db__message_store(&chunk.source, base_सन्देश, p_message_expiry_interval,
			mosq_mo_client);

	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);

	यदि(rc == MOSQ_ERR_SUCCESS){
		base_सन्देश_गन्ती++;
		फर्काऊ MOSQ_ERR_SUCCESS;
	}अन्यथा{
		फर्काऊ rc;
	}
cleanup:
	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);
	mosquitto_FREE(chunk.topic);
	mosquitto_FREE(chunk.payload);
	फर्काऊ rc;
}


स्थिर पूर्णाङ्क persist__retain_chunk_restore(खाता *db_fptr)
{
	संरचना mosquitto_base_सन्देश *base_सन्देश;
	संरचना P_retain chunk;
	पूर्णाङ्क rc;
	वर्ण **split_topics;
	वर्ण *local_topic;

	स_रखो(&chunk, 0, परिमाण(संरचना P_retain));

	यदि(db_version == 6 || db_version == 5){
		rc = persist__chunk_retain_read_v56(db_fptr, &chunk);
	}अन्यथा{
		rc = persist__chunk_retain_read_v234(db_fptr, &chunk);
	}
	यदि(rc){
		फर्काऊ rc;
	}

	HASH_FIND(hh, db.सन्देश_store, &chunk.F.store_id, परिमाण(chunk.F.store_id), base_सन्देश);
	यदि(base_सन्देश){
		rc = sub__topic_tokenise(base_सन्देश->data.topic, &local_topic, &split_topics, रिक्त);
		यदि(rc){
			फर्काऊ rc;
		}
		retain__store(base_सन्देश->data.topic, base_सन्देश, split_topics, true);
		mosquitto_FREE(local_topic);
		mosquitto_FREE(split_topics);
		retained_गन्ती++;
	}अन्यथा{
		/* Can't find the message - probably expired */
	}
	फर्काऊ MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क persist__sub_chunk_restore(खाता *db_fptr)
{
	संरचना P_sub chunk;
	पूर्णाङ्क rc;
	संरचना mosquitto_subscription sub;

	स_रखो(&chunk, 0, परिमाण(संरचना P_sub));

	यदि(db_version == 6 || db_version == 5){
		rc = persist__chunk_sub_read_v56(db_fptr, &chunk);
	}अन्यथा{
		rc = persist__chunk_sub_read_v234(db_fptr, &chunk);
	}
	यदि(rc){
		फर्काऊ rc;
	}

	sub.clientid = chunk.clientid;
	sub.topic_filter = chunk.topic;
	sub.options = chunk.F.qos | chunk.F.options;
	sub.identifier = chunk.F.identifier;
	rc = persist__restore_sub(&sub);

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.topic);
	यदि(rc == 0){
		subscription_गन्ती++;
	}

	फर्काऊ rc;
}


पूर्णाङ्क persist__chunk_header_read(खाता *db_fptr, uint32_t *chunk, uint32_t *length)
{
	यदि(db_version == 6 || db_version == 5){
		फर्काऊ persist__chunk_header_read_v56(db_fptr, chunk, length);
	}अन्यथा{
		फर्काऊ persist__chunk_header_read_v234(db_fptr, chunk, length);
	}
}


पूर्णाङ्क persist__restore(शून्य)
{
	खाता *fptr;
	वर्ण header[15];
	पूर्णाङ्क rc = 0;
	uint32_t crc;
	uint32_t i32temp;
	uint32_t chunk, length;
	माप_प्रकार rlen;
	वर्ण *err;
	संरचना PF_cfg cfg_chunk;

	प्रतिज्ञा(db.config);

	यदि(!db.config->persistence || db.config->persistence_filepath == रिक्त){
		फर्काऊ MOSQ_ERR_SUCCESS;
	}

	db.सन्देश_store = रिक्त;
	base_सन्देश_गन्ती = 0;
	retained_गन्ती = 0;
	client_गन्ती = 0;
	subscription_गन्ती = 0;
	client_सन्देश_गन्ती = 0;

	fptr = mosquitto_fopen(db.config->persistence_filepath, "rb", true);
	यदि(fptr == रिक्त){
		फर्काऊ MOSQ_ERR_SUCCESS;
	}
	rlen = ख_पढ़ो(&header, 1, 15, fptr);
	यदि(rlen == 0){
		ख_बंद(fptr);
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Persistence file is empty.");
		फर्काऊ 0;
	}अन्यथा यदि(rlen != 15){
		गच्छ error;
	}
	यदि(!स_भेद(header, magic, 15)){
		/* Restore DB as normal */
		read_e(fptr, &crc, परिमाण(uint32_t));
		read_e(fptr, &i32temp, परिमाण(uint32_t));
		db_version = ntohl(i32temp);
		/* IMPORTANT - this is where compatibility checks are made.
		 * Is your DB change still compatible with previous versions?
		 */
		यदि(db_version != MOSQ_DB_VERSION){
			यदि(db_version == 5){
				/* Addition of username and listener_port to client chunk in v6 */
			}अन्यथा यदि(db_version == 4){
			}अन्यथा यदि(db_version == 3){
				/* Addition of source_username and source_port to msg_store chunk in v4, v1.5.6 */
			}अन्यथा यदि(db_version == 2){
				/* Addition of disconnect_t to client chunk in v3. */
			}अन्यथा{
				ख_बंद(fptr);
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unsupported persistent database format version %d (need version %d).", db_version, MOSQ_DB_VERSION);
				फर्काऊ MOSQ_ERR_INVAL;
			}
		}

		जबसम्म(persist__chunk_header_read(fptr, &chunk, &length) == MOSQ_ERR_SUCCESS){
			विकल्प(chunk){
				पक्ष DB_CHUNK_CFG:
					यदि(db_version == 6 || db_version == 5){
						rc = persist__chunk_cfg_read_v56(fptr, &cfg_chunk);
						यदि(rc){
							ख_बंद(fptr);
							फर्काऊ rc;
						}
					}अन्यथा{
						rc = persist__chunk_cfg_read_v234(fptr, &cfg_chunk);
						यदि(rc){
							ख_बंद(fptr);
							फर्काऊ rc;
						}
					}
					यदि(cfg_chunk.dbid_size != परिमाण(dbid_t)){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Incompatible database configuration (dbid size is %d bytes, expected %lu)",
								cfg_chunk.dbid_size, (अचिन्हित दीर्घ)परिमाण(dbid_t));
						ख_बंद(fptr);
						फर्काऊ MOSQ_ERR_INVAL;
					}
					db.last_db_id = cfg_chunk.last_db_id;
					रोक;

				पक्ष DB_CHUNK_BASE_सन्देश:
					rc = persist_base_सन्देश_chunk_restore(fptr, length);
					यदि(rc){
						ख_बंद(fptr);
						फर्काऊ rc;
					}
					रोक;

				पक्ष DB_CHUNK_CLIENT_सन्देश:
					rc = persist_client_सन्देश_chunk_restore(fptr, length);
					यदि(rc){
						ख_बंद(fptr);
						फर्काऊ rc;
					}
					रोक;

				पक्ष DB_CHUNK_RETAIN:
					rc = persist__retain_chunk_restore(fptr);
					यदि(rc){
						ख_बंद(fptr);
						फर्काऊ rc;
					}
					रोक;

				पक्ष DB_CHUNK_SUB:
					rc = persist__sub_chunk_restore(fptr);
					यदि(rc){
						ख_बंद(fptr);
						फर्काऊ rc;
					}
					रोक;

				पक्ष DB_CHUNK_CLIENT:
					rc = persist__client_chunk_restore(fptr);
					यदि(rc){
						ख_बंद(fptr);
						फर्काऊ rc;
					}
					रोक;

				स्वभाव:
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Unsupported chunk \"%d\" in persistent database file. Ignoring.", chunk);
					यदि(ख_जाओ(fptr, length, प्रस्तुत_से) < 0){
						ख_बंद(fptr);
						फर्काऊ MOSQ_ERR_INVAL;
					}
					रोक;
			}
		}
	}अन्यथा{
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to restore persistent database. Unrecognised file format.");
		rc = 1;
	}

	ख_बंद(fptr);

	log__printf(रिक्त, MOSQ_LOG_INFO, "Restored %ld base messages", base_सन्देश_गन्ती);
	log__printf(रिक्त, MOSQ_LOG_INFO, "Restored %ld retained messages", retained_गन्ती);
	log__printf(रिक्त, MOSQ_LOG_INFO, "Restored %ld clients", client_गन्ती);
	log__printf(रिक्त, MOSQ_LOG_INFO, "Restored %ld subscriptions", subscription_गन्ती);
	log__printf(रिक्त, MOSQ_LOG_INFO, "Restored %ld client messages", client_सन्देश_गन्ती);

	फर्काऊ rc;
error:
	err = म_त्रुटि(त्रुटि_सं);
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s.", err);
	यदि(fptr){
		ख_बंद(fptr);
	}
	फर्काऊ MOSQ_ERR_ERRNO;
}


स्थिर पूर्णाङ्क persist__restore_sub(अचर संरचना mosquitto_subscription *sub)
{
	संरचना mosquitto *context;

	यदि(!sub->clientid){
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no client id, ignoring.");
		फर्काऊ MOSQ_ERR_SUCCESS;
	}
	यदि(!sub->topic_filter){
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no topic filter, ignoring.");
		फर्काऊ MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(sub->clientid, 0);
	यदि(!context){
		फर्काऊ MOSQ_ERR_INVAL;
	}
	फर्काऊ sub__add(context, sub);
}

#endif
