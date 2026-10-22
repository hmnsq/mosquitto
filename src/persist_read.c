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

#समावेश "config.h"

#अगर_घोषित WITH_PERSISTENCE

#अगर_अघोषित WIN32
#समावेश <arpa/inet.h>
#पूर्ण_अगर
#समावेश <assert.h>
#समावेश <errno.h>
#समावेश <fcntl.h>
#समावेश <stdio.h>
#समावेश <string.h>
#समावेश <sys/stat.h>
#समावेश <time.h>
#समावेश <utlist.h>

#समावेश "mosquitto_broker_internal.h"
#समावेश "persist.h"
#समावेश "util_mosq.h"

uint32_t db_version;

स्थिर अचिन्हित अक्षर magic[15] = {0x00, 0xB5, 0x00, 'm', 'o', 's', 'q', 'u', 'i', 't', 't', 'o', ' ', 'd', 'b'};
अटल दीर्घ base_संदेश_गिनती = 0;
अटल दीर्घ retained_गिनती = 0;
अटल दीर्घ client_गिनती = 0;
अटल दीर्घ subscription_गिनती = 0;
अटल दीर्घ client_संदेश_गिनती = 0;

अटल पूर्णांक persist__restore_sub(स्थिर काष्ठा mosquitto_subscription *sub);

अटल काष्ठा mosquitto *persist__find_or_add_context(स्थिर अक्षर *clientid, uint16_t last_mid)
{
	काष्ठा mosquitto *context;

	अगर(!clientid){
		वापस शून्य;
	}

	context = शून्य;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, म_माप(clientid), context);
	अगर(!context){
		context = context__init();
		अगर(!context){
			वापस शून्य;
		}
		context->id = mosquitto_strdup(clientid);
		अगर(!context->id){
			mosquitto_FREE(context);
			वापस शून्य;
		}

		context->clean_start = false;

		context__add_to_by_id(context);
	}
	अगर(last_mid){
		context->last_mid = last_mid;
	}
	वापस context;
}


पूर्णांक persist__read_string_len(खाता *db_fptr, अक्षर **str, uint16_t len)
{
	अक्षर *s = शून्य;

	अगर(len){
		s = mosquitto_malloc(len+1U);
		अगर(!s){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
			वापस MOSQ_ERR_NOMEM;
		}
		अगर(ख_पढ़ो(s, 1, len, db_fptr) != len){
			mosquitto_FREE(s);
			वापस MOSQ_ERR_NOMEM;
		}
		s[len] = '\0';
	}

	*str = s;
	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक persist__read_string(खाता *db_fptr, अक्षर **str)
{
	uint16_t i16temp;
	uint16_t slen;

	अगर(ख_पढ़ो(&i16temp, 1, माप(uint16_t), db_fptr) != माप(uint16_t)){
		वापस MOSQ_ERR_INVAL;
	}

	slen = ntohs(i16temp);
	वापस persist__read_string_len(db_fptr, str, slen);
}


अटल पूर्णांक persist_client_संदेश_restore(काष्ठा P_client_संदेश *chunk)
{
	काष्ठा mosquitto_client_संदेश *cmsg;
	काष्ठा mosquitto_base_संदेश *संदेश;
	काष्ठा mosquitto *context;
	काष्ठा mosquitto_संदेश_तथ्य *संदेश_तथ्य;

	HASH_FIND(hh, db.संदेश_store, &chunk->F.store_id, माप(chunk->F.store_id), संदेश);
	अगर(!संदेश){
		/* Can't find message - probably expired */
		वापस MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk->clientid, 0);
	अगर(!context){
		log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Persistence file contains client message with no matching client. File may be corrupt.");
		वापस 0;
	}

	cmsg = mosquitto_calloc(1, माप(काष्ठा mosquitto_client_संदेश));
	अगर(!cmsg){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
		वापस MOSQ_ERR_NOMEM;
	}

	cmsg->अग्र = शून्य;
	cmsg->base_संदेश = शून्य;
	cmsg->तथ्य.cmsg_id = ++context->last_cmsg_id;
	cmsg->तथ्य.mid = chunk->F.mid;
	cmsg->तथ्य.qos = chunk->F.qos;
	cmsg->तथ्य.retain = (chunk->F.retain_dup&0xF0)>>4;
	cmsg->तथ्य.direction = chunk->F.direction;
	cmsg->तथ्य.state = chunk->F.state;
	cmsg->तथ्य.dup = chunk->F.retain_dup&0x0F;
	cmsg->तथ्य.subscription_identifier = chunk->subscription_identifier;

	cmsg->base_संदेश = संदेश;
	db_संदेश_store_ref_inc(cmsg->base_संदेश);

	अगर(cmsg->तथ्य.direction == mosq_md_out){
		संदेश_तथ्य = &context->msgs_out;
	}अन्यथा{
		संदेश_तथ्य = &context->msgs_in;
	}

	अगर(chunk->F.state == mosq_ms_queued || (chunk->F.qos > 0 && संदेश_तथ्य->inflight_quota == 0)){
		DL_APPEND(संदेश_तथ्य->queued, cmsg);
		db_संदेश_add_to_queued_stats(संदेश_तथ्य, cmsg);
	}अन्यथा{
		DL_APPEND(संदेश_तथ्य->inflight, cmsg);
		अगर(chunk->F.qos > 0 && संदेश_तथ्य->inflight_quota > 0){
			संदेश_तथ्य->inflight_quota--;
		}
		db_संदेश_add_to_inflight_stats(संदेश_तथ्य, cmsg);
	}

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक persist__client_chunk_restore(खाता *db_fptr)
{
	पूर्णांक rc = 0;
	काष्ठा mosquitto *context;
	काष्ठा P_client chunk;

	स_रखो(&chunk, 0, माप(काष्ठा P_client));

	अगर(db_version == 6 || db_version == 5){
		rc = persist__chunk_client_read_v56(db_fptr, &chunk, db_version);
	}अन्यथा{
		rc = persist__chunk_client_read_v234(db_fptr, &chunk, db_version);
	}
	अगर(rc > 0){
		वापस rc;
	}अन्यथा अगर(rc < 0){
		/* Client not loaded, but otherwise not an error */
		log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Empty client entry found in persistence database, it may be corrupt.");
		वापस MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk.clientid, chunk.F.last_mid);
	अगर(context){
		context->session_expiry_time = chunk.F.session_expiry_time;
		context->session_expiry_interval = chunk.F.session_expiry_interval;
		अगर(chunk.username && !context->username){
			/* username is not freed here, it is now owned by context */
			context->username = chunk.username;
			chunk.username = शून्य;
		}
		/* in per_listener_settings mode, try to find the listener by persisted port */
		अगर(db.config->per_listener_settings && !context->listener && chunk.F.listener_port > 0){
			क्रम(पूर्णांक i=0; i < db.config->listener_गिनती; i++){
				अगर(db.config->listeners[i].port == chunk.F.listener_port){
					context->listener = &db.config->listeners[i];
					अवरोध;
				}
			}
		}
		session_expiry__add_from_persistence(context, chunk.F.session_expiry_time);
	}अन्यथा{
		rc = 1;
	}

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.username);
	अगर(rc == 0){
		client_गिनती++;
	}
	वापस rc;
}


अटल पूर्णांक persist_client_संदेश_chunk_restore(खाता *db_fptr, uint32_t लंबाई)
{
	काष्ठा P_client_संदेश chunk;
	पूर्णांक rc;

	स_रखो(&chunk, 0, माप(काष्ठा P_client_संदेश));

	अगर(db_version == 6 || db_version == 5){
		rc = persist_chunk_client_संदेश_read_v56(db_fptr, &chunk, लंबाई);
	}अन्यथा{
		rc = persist_chunk_client_संदेश_read_v234(db_fptr, &chunk);
	}
	अगर(rc){
		वापस rc;
	}

	rc = persist_client_संदेश_restore(&chunk);
	mosquitto_FREE(chunk.clientid);

	अगर(rc == 0){
		client_संदेश_गिनती++;
	}
	वापस rc;
}


अटल पूर्णांक persist_base_संदेश_chunk_restore(खाता *db_fptr, uint32_t लंबाई)
{
	काष्ठा P_base_संदेश chunk;
	काष्ठा mosquitto_base_संदेश *base_संदेश = शून्य;
	int64_t message_expiry_interval64;
	uint32_t message_expiry_interval;
	uint32_t *p_message_expiry_interval = शून्य;
	पूर्णांक rc = 0;

	स_रखो(&chunk, 0, माप(काष्ठा P_base_संदेश));

	अगर(db_version == 6 || db_version == 5){
		rc = persist_chunk_base_संदेश_read_v56(db_fptr, &chunk, लंबाई);
	}अन्यथा{
		rc = persist_chunk_base_संदेश_read_v234(db_fptr, &chunk, db_version);
	}
	अगर(rc){
		वापस rc;
	}
	अगर(chunk.F.topic_len == 0){
		rc = MOSQ_ERR_INVAL;
		जाओ cleanup;
	}

	अगर(chunk.F.source_port){
		क्रम(पूर्णांक i=0; i<db.config->listener_गिनती; i++){
			अगर(db.config->listeners[i].port == chunk.F.source_port){
				chunk.source.listener = &db.config->listeners[i];
				अवरोध;
			}
		}
	}

	अगर(chunk.F.expiry_time > 0){
		message_expiry_interval64 = chunk.F.expiry_time - समय(शून्य);
		अगर(message_expiry_interval64 < 0 || message_expiry_interval64 > UINT32_MAX){
			/* Expired message */
			rc = MOSQ_ERR_SUCCESS;
			जाओ cleanup;
		}अन्यथा{
			message_expiry_interval = (uint32_t)message_expiry_interval64;
		}
		p_message_expiry_interval = &message_expiry_interval;
	}

	base_संदेश = mosquitto_calloc(1, माप(काष्ठा mosquitto_base_संदेश));
	अगर(base_संदेश == शून्य){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
		rc = MOSQ_ERR_NOMEM;
		जाओ cleanup;
	}

	base_संदेश->तथ्य.store_id = chunk.F.store_id;
	base_संदेश->तथ्य.source_mid = chunk.F.source_mid;
	base_संदेश->तथ्य.topic = chunk.topic;
	base_संदेश->तथ्य.qos = chunk.F.qos;
	base_संदेश->तथ्य.payloadlen = chunk.F.payloadlen;
	base_संदेश->तथ्य.retain = chunk.F.retain;
	base_संदेश->तथ्य.properties = chunk.properties;
	base_संदेश->तथ्य.payload = chunk.payload;
	base_संदेश->source_listener = chunk.source.listener;

	rc = db__message_store(&chunk.source, base_संदेश, p_message_expiry_interval,
			mosq_mo_client);

	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);

	अगर(rc == MOSQ_ERR_SUCCESS){
		base_संदेश_गिनती++;
		वापस MOSQ_ERR_SUCCESS;
	}अन्यथा{
		वापस rc;
	}
cleanup:
	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);
	mosquitto_FREE(chunk.topic);
	mosquitto_FREE(chunk.payload);
	वापस rc;
}


अटल पूर्णांक persist__retain_chunk_restore(खाता *db_fptr)
{
	काष्ठा mosquitto_base_संदेश *base_संदेश;
	काष्ठा P_retain chunk;
	पूर्णांक rc;
	अक्षर **split_topics;
	अक्षर *local_topic;

	स_रखो(&chunk, 0, माप(काष्ठा P_retain));

	अगर(db_version == 6 || db_version == 5){
		rc = persist__chunk_retain_read_v56(db_fptr, &chunk);
	}अन्यथा{
		rc = persist__chunk_retain_read_v234(db_fptr, &chunk);
	}
	अगर(rc){
		वापस rc;
	}

	HASH_FIND(hh, db.संदेश_store, &chunk.F.store_id, माप(chunk.F.store_id), base_संदेश);
	अगर(base_संदेश){
		rc = sub__topic_tokenise(base_संदेश->तथ्य.topic, &local_topic, &split_topics, शून्य);
		अगर(rc){
			वापस rc;
		}
		retain__store(base_संदेश->तथ्य.topic, base_संदेश, split_topics, true);
		mosquitto_FREE(local_topic);
		mosquitto_FREE(split_topics);
		retained_गिनती++;
	}अन्यथा{
		/* Can't find the message - probably expired */
	}
	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक persist__sub_chunk_restore(खाता *db_fptr)
{
	काष्ठा P_sub chunk;
	पूर्णांक rc;
	काष्ठा mosquitto_subscription sub;

	स_रखो(&chunk, 0, माप(काष्ठा P_sub));

	अगर(db_version == 6 || db_version == 5){
		rc = persist__chunk_sub_read_v56(db_fptr, &chunk);
	}अन्यथा{
		rc = persist__chunk_sub_read_v234(db_fptr, &chunk);
	}
	अगर(rc){
		वापस rc;
	}

	sub.clientid = chunk.clientid;
	sub.topic_filter = chunk.topic;
	sub.options = chunk.F.qos | chunk.F.options;
	sub.identifier = chunk.F.identifier;
	rc = persist__restore_sub(&sub);

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.topic);
	अगर(rc == 0){
		subscription_गिनती++;
	}

	वापस rc;
}


पूर्णांक persist__chunk_header_read(खाता *db_fptr, uint32_t *chunk, uint32_t *लंबाई)
{
	अगर(db_version == 6 || db_version == 5){
		वापस persist__chunk_header_read_v56(db_fptr, chunk, लंबाई);
	}अन्यथा{
		वापस persist__chunk_header_read_v234(db_fptr, chunk, लंबाई);
	}
}


पूर्णांक persist__restore(व्योम)
{
	खाता *fptr;
	अक्षर header[15];
	पूर्णांक rc = 0;
	uint32_t crc;
	uint32_t i32temp;
	uint32_t chunk, लंबाई;
	माप_प्रकार rlen;
	अक्षर *err;
	काष्ठा PF_cfg cfg_chunk;

	निश्चित(db.config);

	अगर(!db.config->persistence || db.config->persistence_filepath == शून्य){
		वापस MOSQ_ERR_SUCCESS;
	}

	db.संदेश_store = शून्य;
	base_संदेश_गिनती = 0;
	retained_गिनती = 0;
	client_गिनती = 0;
	subscription_गिनती = 0;
	client_संदेश_गिनती = 0;

	fptr = mosquitto_fopen(db.config->persistence_filepath, "rb", true);
	अगर(fptr == शून्य){
		वापस MOSQ_ERR_SUCCESS;
	}
	rlen = ख_पढ़ो(&header, 1, 15, fptr);
	अगर(rlen == 0){
		ख_बंद(fptr);
		log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Persistence file is empty.");
		वापस 0;
	}अन्यथा अगर(rlen != 15){
		जाओ error;
	}
	अगर(!स_भेद(header, magic, 15)){
		/* Restore DB as normal */
		read_e(fptr, &crc, माप(uint32_t));
		read_e(fptr, &i32temp, माप(uint32_t));
		db_version = ntohl(i32temp);
		/* IMPORTANT - this is where compatibility checks are made.
		 * Is your DB change still compatible with previous versions?
		 */
		अगर(db_version != MOSQ_DB_VERSION){
			अगर(db_version == 5){
				/* Addition of username and listener_port to client chunk in v6 */
			}अन्यथा अगर(db_version == 4){
			}अन्यथा अगर(db_version == 3){
				/* Addition of source_username and source_port to msg_store chunk in v4, v1.5.6 */
			}अन्यथा अगर(db_version == 2){
				/* Addition of disconnect_t to client chunk in v3. */
			}अन्यथा{
				ख_बंद(fptr);
				log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unsupported persistent database format version %d (need version %d).", db_version, MOSQ_DB_VERSION);
				वापस MOSQ_ERR_INVAL;
			}
		}

		जबतक(persist__chunk_header_read(fptr, &chunk, &लंबाई) == MOSQ_ERR_SUCCESS){
			चयन(chunk){
				हाल DB_CHUNK_CFG:
					अगर(db_version == 6 || db_version == 5){
						rc = persist__chunk_cfg_read_v56(fptr, &cfg_chunk);
						अगर(rc){
							ख_बंद(fptr);
							वापस rc;
						}
					}अन्यथा{
						rc = persist__chunk_cfg_read_v234(fptr, &cfg_chunk);
						अगर(rc){
							ख_बंद(fptr);
							वापस rc;
						}
					}
					अगर(cfg_chunk.dbid_आकार != माप(dbid_t)){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Incompatible database configuration (dbid size is %d bytes, expected %lu)",
								cfg_chunk.dbid_आकार, (अचिन्हित दीर्घ)माप(dbid_t));
						ख_बंद(fptr);
						वापस MOSQ_ERR_INVAL;
					}
					db.last_db_id = cfg_chunk.last_db_id;
					अवरोध;

				हाल DB_CHUNK_BASE_संदेश:
					rc = persist_base_संदेश_chunk_restore(fptr, लंबाई);
					अगर(rc){
						ख_बंद(fptr);
						वापस rc;
					}
					अवरोध;

				हाल DB_CHUNK_CLIENT_संदेश:
					rc = persist_client_संदेश_chunk_restore(fptr, लंबाई);
					अगर(rc){
						ख_बंद(fptr);
						वापस rc;
					}
					अवरोध;

				हाल DB_CHUNK_RETAIN:
					rc = persist__retain_chunk_restore(fptr);
					अगर(rc){
						ख_बंद(fptr);
						वापस rc;
					}
					अवरोध;

				हाल DB_CHUNK_SUB:
					rc = persist__sub_chunk_restore(fptr);
					अगर(rc){
						ख_बंद(fptr);
						वापस rc;
					}
					अवरोध;

				हाल DB_CHUNK_CLIENT:
					rc = persist__client_chunk_restore(fptr);
					अगर(rc){
						ख_बंद(fptr);
						वापस rc;
					}
					अवरोध;

				शेष:
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Unsupported chunk \"%d\" in persistent database file. Ignoring.", chunk);
					अगर(ख_जाओ(fptr, लंबाई, प्रस्तुत_से) < 0){
						ख_बंद(fptr);
						वापस MOSQ_ERR_INVAL;
					}
					अवरोध;
			}
		}
	}अन्यथा{
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to restore persistent database. Unrecognised file format.");
		rc = 1;
	}

	ख_बंद(fptr);

	log__printf(शून्य, MOSQ_LOG_INFO, "Restored %ld base messages", base_संदेश_गिनती);
	log__printf(शून्य, MOSQ_LOG_INFO, "Restored %ld retained messages", retained_गिनती);
	log__printf(शून्य, MOSQ_LOG_INFO, "Restored %ld clients", client_गिनती);
	log__printf(शून्य, MOSQ_LOG_INFO, "Restored %ld subscriptions", subscription_गिनती);
	log__printf(शून्य, MOSQ_LOG_INFO, "Restored %ld client messages", client_संदेश_गिनती);

	वापस rc;
error:
	err = म_त्रुटि(त्रुटि_सं);
	log__printf(शून्य, MOSQ_LOG_ERR, "Error: %s.", err);
	अगर(fptr){
		ख_बंद(fptr);
	}
	वापस MOSQ_ERR_ERRNO;
}


अटल पूर्णांक persist__restore_sub(स्थिर काष्ठा mosquitto_subscription *sub)
{
	काष्ठा mosquitto *context;

	अगर(!sub->clientid){
		log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no client id, ignoring.");
		वापस MOSQ_ERR_SUCCESS;
	}
	अगर(!sub->topic_filter){
		log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no topic filter, ignoring.");
		वापस MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(sub->clientid, 0);
	अगर(!context){
		वापस MOSQ_ERR_INVAL;
	}
	वापस sub__add(context, sub);
}

#पूर्ण_अगर
