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

स्थिरांक अचिन्हित अक्षर magic[15] = {0x00, 0xB5, 0x00, 'm', 'o', 's', 'q', 'u', 'i', 't', 't', 'o', ' ', 'd', 'b'};
स्थिर दीर्घ base_संदेश_मोजणी = 0;
स्थिर दीर्घ retained_मोजणी = 0;
स्थिर दीर्घ client_मोजणी = 0;
स्थिर दीर्घ subscription_मोजणी = 0;
स्थिर दीर्घ client_संदेश_मोजणी = 0;

स्थिर पूर्णांक persist__restore_sub(स्थिरांक संरचना mosquitto_subscription *sub);

स्थिर संरचना mosquitto *persist__find_or_add_context(स्थिरांक अक्षर *clientid, uint16_t last_mid)
{
	संरचना mosquitto *context;

	जर(!clientid){
		परत रिक्त;
	}

	context = रिक्त;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, म_माप(clientid), context);
	जर(!context){
		context = context__init();
		जर(!context){
			परत रिक्त;
		}
		context->id = mosquitto_strdup(clientid);
		जर(!context->id){
			mosquitto_FREE(context);
			परत रिक्त;
		}

		context->clean_start = false;

		context__add_to_by_id(context);
	}
	जर(last_mid){
		context->last_mid = last_mid;
	}
	परत context;
}


पूर्णांक persist__read_string_len(खाता *db_fptr, अक्षर **str, uint16_t len)
{
	अक्षर *s = रिक्त;

	जर(len){
		s = mosquitto_malloc(len+1U);
		जर(!s){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
			परत MOSQ_ERR_NOMEM;
		}
		जर(ख_पढ़ो(s, 1, len, db_fptr) != len){
			mosquitto_FREE(s);
			परत MOSQ_ERR_NOMEM;
		}
		s[len] = '\0';
	}

	*str = s;
	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक persist__read_string(खाता *db_fptr, अक्षर **str)
{
	uint16_t i16temp;
	uint16_t slen;

	जर(ख_पढ़ो(&i16temp, 1, आकार(uint16_t), db_fptr) != आकार(uint16_t)){
		परत MOSQ_ERR_INVAL;
	}

	slen = ntohs(i16temp);
	परत persist__read_string_len(db_fptr, str, slen);
}


स्थिर पूर्णांक persist_client_संदेश_restore(संरचना P_client_संदेश *chunk)
{
	संरचना mosquitto_client_संदेश *cmsg;
	संरचना mosquitto_base_संदेश *संदेश;
	संरचना mosquitto *context;
	संरचना mosquitto_संदेश_data *संदेश_data;

	HASH_FIND(hh, db.संदेश_store, &chunk->F.store_id, आकार(chunk->F.store_id), संदेश);
	जर(!संदेश){
		/* Can't find message - probably expired */
		परत MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk->clientid, 0);
	जर(!context){
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Persistence file contains client message with no matching client. File may be corrupt.");
		परत 0;
	}

	cmsg = mosquitto_calloc(1, आकार(संरचना mosquitto_client_संदेश));
	जर(!cmsg){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		परत MOSQ_ERR_NOMEM;
	}

	cmsg->next = रिक्त;
	cmsg->base_संदेश = रिक्त;
	cmsg->data.cmsg_id = ++context->last_cmsg_id;
	cmsg->data.mid = chunk->F.mid;
	cmsg->data.qos = chunk->F.qos;
	cmsg->data.retain = (chunk->F.retain_dup&0xF0)>>4;
	cmsg->data.direction = chunk->F.direction;
	cmsg->data.state = chunk->F.state;
	cmsg->data.dup = chunk->F.retain_dup&0x0F;
	cmsg->data.subscription_identifier = chunk->subscription_identifier;

	cmsg->base_संदेश = संदेश;
	db_संदेश_store_ref_inc(cmsg->base_संदेश);

	जर(cmsg->data.direction == mosq_md_out){
		संदेश_data = &context->msgs_out;
	}अन्यथा{
		संदेश_data = &context->msgs_in;
	}

	जर(chunk->F.state == mosq_ms_queued || (chunk->F.qos > 0 && संदेश_data->inflight_quota == 0)){
		DL_APPEND(संदेश_data->queued, cmsg);
		db_संदेश_add_to_queued_stats(संदेश_data, cmsg);
	}अन्यथा{
		DL_APPEND(संदेश_data->inflight, cmsg);
		जर(chunk->F.qos > 0 && संदेश_data->inflight_quota > 0){
			संदेश_data->inflight_quota--;
		}
		db_संदेश_add_to_inflight_stats(संदेश_data, cmsg);
	}

	परत MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णांक persist__client_chunk_restore(खाता *db_fptr)
{
	पूर्णांक rc = 0;
	संरचना mosquitto *context;
	संरचना P_client chunk;

	स_रखो(&chunk, 0, आकार(संरचना P_client));

	जर(db_version == 6 || db_version == 5){
		rc = persist__chunk_client_read_v56(db_fptr, &chunk, db_version);
	}अन्यथा{
		rc = persist__chunk_client_read_v234(db_fptr, &chunk, db_version);
	}
	जर(rc > 0){
		परत rc;
	}अन्यथा जर(rc < 0){
		/* Client not loaded, but otherwise not an error */
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Empty client entry found in persistence database, it may be corrupt.");
		परत MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk.clientid, chunk.F.last_mid);
	जर(context){
		context->session_expiry_time = chunk.F.session_expiry_time;
		context->session_expiry_interval = chunk.F.session_expiry_interval;
		जर(chunk.username && !context->username){
			/* username is not freed here, it is now owned by context */
			context->username = chunk.username;
			chunk.username = रिक्त;
		}
		/* in per_listener_settings mode, try to find the listener by persisted port */
		जर(db.config->per_listener_settings && !context->listener && chunk.F.listener_port > 0){
			साठी(पूर्णांक i=0; i < db.config->listener_मोजणी; i++){
				जर(db.config->listeners[i].port == chunk.F.listener_port){
					context->listener = &db.config->listeners[i];
					थांबा;
				}
			}
		}
		session_expiry__add_from_persistence(context, chunk.F.session_expiry_time);
	}अन्यथा{
		rc = 1;
	}

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.username);
	जर(rc == 0){
		client_मोजणी++;
	}
	परत rc;
}


स्थिर पूर्णांक persist_client_संदेश_chunk_restore(खाता *db_fptr, uint32_t length)
{
	संरचना P_client_संदेश chunk;
	पूर्णांक rc;

	स_रखो(&chunk, 0, आकार(संरचना P_client_संदेश));

	जर(db_version == 6 || db_version == 5){
		rc = persist_chunk_client_संदेश_read_v56(db_fptr, &chunk, length);
	}अन्यथा{
		rc = persist_chunk_client_संदेश_read_v234(db_fptr, &chunk);
	}
	जर(rc){
		परत rc;
	}

	rc = persist_client_संदेश_restore(&chunk);
	mosquitto_FREE(chunk.clientid);

	जर(rc == 0){
		client_संदेश_मोजणी++;
	}
	परत rc;
}


स्थिर पूर्णांक persist_base_संदेश_chunk_restore(खाता *db_fptr, uint32_t length)
{
	संरचना P_base_संदेश chunk;
	संरचना mosquitto_base_संदेश *base_संदेश = रिक्त;
	int64_t message_expiry_interval64;
	uint32_t message_expiry_interval;
	uint32_t *p_message_expiry_interval = रिक्त;
	पूर्णांक rc = 0;

	स_रखो(&chunk, 0, आकार(संरचना P_base_संदेश));

	जर(db_version == 6 || db_version == 5){
		rc = persist_chunk_base_संदेश_read_v56(db_fptr, &chunk, length);
	}अन्यथा{
		rc = persist_chunk_base_संदेश_read_v234(db_fptr, &chunk, db_version);
	}
	जर(rc){
		परत rc;
	}
	जर(chunk.F.topic_len == 0){
		rc = MOSQ_ERR_INVAL;
		जा cleanup;
	}

	जर(chunk.F.source_port){
		साठी(पूर्णांक i=0; i<db.config->listener_मोजणी; i++){
			जर(db.config->listeners[i].port == chunk.F.source_port){
				chunk.source.listener = &db.config->listeners[i];
				थांबा;
			}
		}
	}

	जर(chunk.F.expiry_time > 0){
		message_expiry_interval64 = chunk.F.expiry_time - समय(रिक्त);
		जर(message_expiry_interval64 < 0 || message_expiry_interval64 > UINT32_MAX){
			/* Expired message */
			rc = MOSQ_ERR_SUCCESS;
			जा cleanup;
		}अन्यथा{
			message_expiry_interval = (uint32_t)message_expiry_interval64;
		}
		p_message_expiry_interval = &message_expiry_interval;
	}

	base_संदेश = mosquitto_calloc(1, आकार(संरचना mosquitto_base_संदेश));
	जर(base_संदेश == रिक्त){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		rc = MOSQ_ERR_NOMEM;
		जा cleanup;
	}

	base_संदेश->data.store_id = chunk.F.store_id;
	base_संदेश->data.source_mid = chunk.F.source_mid;
	base_संदेश->data.topic = chunk.topic;
	base_संदेश->data.qos = chunk.F.qos;
	base_संदेश->data.payloadlen = chunk.F.payloadlen;
	base_संदेश->data.retain = chunk.F.retain;
	base_संदेश->data.properties = chunk.properties;
	base_संदेश->data.payload = chunk.payload;
	base_संदेश->source_listener = chunk.source.listener;

	rc = db__message_store(&chunk.source, base_संदेश, p_message_expiry_interval,
			mosq_mo_client);

	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);

	जर(rc == MOSQ_ERR_SUCCESS){
		base_संदेश_मोजणी++;
		परत MOSQ_ERR_SUCCESS;
	}अन्यथा{
		परत rc;
	}
cleanup:
	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);
	mosquitto_FREE(chunk.topic);
	mosquitto_FREE(chunk.payload);
	परत rc;
}


स्थिर पूर्णांक persist__retain_chunk_restore(खाता *db_fptr)
{
	संरचना mosquitto_base_संदेश *base_संदेश;
	संरचना P_retain chunk;
	पूर्णांक rc;
	अक्षर **split_topics;
	अक्षर *local_topic;

	स_रखो(&chunk, 0, आकार(संरचना P_retain));

	जर(db_version == 6 || db_version == 5){
		rc = persist__chunk_retain_read_v56(db_fptr, &chunk);
	}अन्यथा{
		rc = persist__chunk_retain_read_v234(db_fptr, &chunk);
	}
	जर(rc){
		परत rc;
	}

	HASH_FIND(hh, db.संदेश_store, &chunk.F.store_id, आकार(chunk.F.store_id), base_संदेश);
	जर(base_संदेश){
		rc = sub__topic_tokenise(base_संदेश->data.topic, &local_topic, &split_topics, रिक्त);
		जर(rc){
			परत rc;
		}
		retain__store(base_संदेश->data.topic, base_संदेश, split_topics, true);
		mosquitto_FREE(local_topic);
		mosquitto_FREE(split_topics);
		retained_मोजणी++;
	}अन्यथा{
		/* Can't find the message - probably expired */
	}
	परत MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णांक persist__sub_chunk_restore(खाता *db_fptr)
{
	संरचना P_sub chunk;
	पूर्णांक rc;
	संरचना mosquitto_subscription sub;

	स_रखो(&chunk, 0, आकार(संरचना P_sub));

	जर(db_version == 6 || db_version == 5){
		rc = persist__chunk_sub_read_v56(db_fptr, &chunk);
	}अन्यथा{
		rc = persist__chunk_sub_read_v234(db_fptr, &chunk);
	}
	जर(rc){
		परत rc;
	}

	sub.clientid = chunk.clientid;
	sub.topic_filter = chunk.topic;
	sub.options = chunk.F.qos | chunk.F.options;
	sub.identifier = chunk.F.identifier;
	rc = persist__restore_sub(&sub);

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.topic);
	जर(rc == 0){
		subscription_मोजणी++;
	}

	परत rc;
}


पूर्णांक persist__chunk_header_read(खाता *db_fptr, uint32_t *chunk, uint32_t *length)
{
	जर(db_version == 6 || db_version == 5){
		परत persist__chunk_header_read_v56(db_fptr, chunk, length);
	}अन्यथा{
		परत persist__chunk_header_read_v234(db_fptr, chunk, length);
	}
}


पूर्णांक persist__restore(शून्य)
{
	खाता *fptr;
	अक्षर header[15];
	पूर्णांक rc = 0;
	uint32_t crc;
	uint32_t i32temp;
	uint32_t chunk, length;
	माप_प्रकार rlen;
	अक्षर *err;
	संरचना PF_cfg cfg_chunk;

	प्रतिज्ञा(db.config);

	जर(!db.config->persistence || db.config->persistence_filepath == रिक्त){
		परत MOSQ_ERR_SUCCESS;
	}

	db.संदेश_store = रिक्त;
	base_संदेश_मोजणी = 0;
	retained_मोजणी = 0;
	client_मोजणी = 0;
	subscription_मोजणी = 0;
	client_संदेश_मोजणी = 0;

	fptr = mosquitto_fopen(db.config->persistence_filepath, "rb", true);
	जर(fptr == रिक्त){
		परत MOSQ_ERR_SUCCESS;
	}
	rlen = ख_पढ़ो(&header, 1, 15, fptr);
	जर(rlen == 0){
		ख_बंद(fptr);
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Persistence file is empty.");
		परत 0;
	}अन्यथा जर(rlen != 15){
		जा error;
	}
	जर(!स_भेद(header, magic, 15)){
		/* Restore DB as normal */
		read_e(fptr, &crc, आकार(uint32_t));
		read_e(fptr, &i32temp, आकार(uint32_t));
		db_version = ntohl(i32temp);
		/* IMPORTANT - this is where compatibility checks are made.
		 * Is your DB change still compatible with previous versions?
		 */
		जर(db_version != MOSQ_DB_VERSION){
			जर(db_version == 5){
				/* Addition of username and listener_port to client chunk in v6 */
			}अन्यथा जर(db_version == 4){
			}अन्यथा जर(db_version == 3){
				/* Addition of source_username and source_port to msg_store chunk in v4, v1.5.6 */
			}अन्यथा जर(db_version == 2){
				/* Addition of disconnect_t to client chunk in v3. */
			}अन्यथा{
				ख_बंद(fptr);
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unsupported persistent database format version %d (need version %d).", db_version, MOSQ_DB_VERSION);
				परत MOSQ_ERR_INVAL;
			}
		}

		पर्यंत(persist__chunk_header_read(fptr, &chunk, &length) == MOSQ_ERR_SUCCESS){
			निवड(chunk){
				स्थिती DB_CHUNK_CFG:
					जर(db_version == 6 || db_version == 5){
						rc = persist__chunk_cfg_read_v56(fptr, &cfg_chunk);
						जर(rc){
							ख_बंद(fptr);
							परत rc;
						}
					}अन्यथा{
						rc = persist__chunk_cfg_read_v234(fptr, &cfg_chunk);
						जर(rc){
							ख_बंद(fptr);
							परत rc;
						}
					}
					जर(cfg_chunk.dbid_size != आकार(dbid_t)){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Incompatible database configuration (dbid size is %d bytes, expected %lu)",
								cfg_chunk.dbid_size, (अचिन्हित दीर्घ)आकार(dbid_t));
						ख_बंद(fptr);
						परत MOSQ_ERR_INVAL;
					}
					db.last_db_id = cfg_chunk.last_db_id;
					थांबा;

				स्थिती DB_CHUNK_BASE_संदेश:
					rc = persist_base_संदेश_chunk_restore(fptr, length);
					जर(rc){
						ख_बंद(fptr);
						परत rc;
					}
					थांबा;

				स्थिती DB_CHUNK_CLIENT_संदेश:
					rc = persist_client_संदेश_chunk_restore(fptr, length);
					जर(rc){
						ख_बंद(fptr);
						परत rc;
					}
					थांबा;

				स्थिती DB_CHUNK_RETAIN:
					rc = persist__retain_chunk_restore(fptr);
					जर(rc){
						ख_बंद(fptr);
						परत rc;
					}
					थांबा;

				स्थिती DB_CHUNK_SUB:
					rc = persist__sub_chunk_restore(fptr);
					जर(rc){
						ख_बंद(fptr);
						परत rc;
					}
					थांबा;

				स्थिती DB_CHUNK_CLIENT:
					rc = persist__client_chunk_restore(fptr);
					जर(rc){
						ख_बंद(fptr);
						परत rc;
					}
					थांबा;

				पूर्वनिर्धारित:
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Unsupported chunk \"%d\" in persistent database file. Ignoring.", chunk);
					जर(ख_जाओ(fptr, length, प्रस्तुत_से) < 0){
						ख_बंद(fptr);
						परत MOSQ_ERR_INVAL;
					}
					थांबा;
			}
		}
	}अन्यथा{
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to restore persistent database. Unrecognised file format.");
		rc = 1;
	}

	ख_बंद(fptr);

	log__printf(रिक्त, MOSQ_LOG_INFO, "Restored %ld base messages", base_संदेश_मोजणी);
	log__printf(रिक्त, MOSQ_LOG_INFO, "Restored %ld retained messages", retained_मोजणी);
	log__printf(रिक्त, MOSQ_LOG_INFO, "Restored %ld clients", client_मोजणी);
	log__printf(रिक्त, MOSQ_LOG_INFO, "Restored %ld subscriptions", subscription_मोजणी);
	log__printf(रिक्त, MOSQ_LOG_INFO, "Restored %ld client messages", client_संदेश_मोजणी);

	परत rc;
error:
	err = म_त्रुटि(त्रुटि_सं);
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s.", err);
	जर(fptr){
		ख_बंद(fptr);
	}
	परत MOSQ_ERR_ERRNO;
}


स्थिर पूर्णांक persist__restore_sub(स्थिरांक संरचना mosquitto_subscription *sub)
{
	संरचना mosquitto *context;

	जर(!sub->clientid){
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no client id, ignoring.");
		परत MOSQ_ERR_SUCCESS;
	}
	जर(!sub->topic_filter){
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no topic filter, ignoring.");
		परत MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(sub->clientid, 0);
	जर(!context){
		परत MOSQ_ERR_INVAL;
	}
	परत sub__add(context, sub);
}

#endif
