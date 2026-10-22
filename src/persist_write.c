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

#समावेश "mosquitto_broker_internal.h"
#समावेश "persist.h"
#समावेश "util_mosq.h"


अटल पूर्णांक persist__client_messages_save(खाता *db_fptr, काष्ठा mosquitto *context, काष्ठा mosquitto_client_संदेश *queue)
{
	काष्ठा P_client_संदेश chunk;
	काष्ठा mosquitto_client_संदेश *cmsg;
	पूर्णांक rc;

	निश्चित(db_fptr);
	निश्चित(context);

	cmsg = queue;
	जबतक(cmsg){
		अगर(!म_भेदन(cmsg->base_संदेश->तथ्य.topic, "$SYS", 4)
				&& cmsg->base_संदेश->ref_गिनती <= 1
				&& cmsg->base_संदेश->dest_id_गिनती == 0){

			/* This $SYS message won't have been persisted, so we can't persist
			 * this client message. */
			cmsg = cmsg->अग्र;
			जारी;
		}

		स_रखो(&chunk, 0, माप(काष्ठा P_client_संदेश));

		chunk.F.store_id = cmsg->base_संदेश->तथ्य.store_id;
		chunk.F.mid = cmsg->तथ्य.mid;
		chunk.F.id_len = (uint16_t)म_माप(context->id);
		chunk.F.qos = cmsg->तथ्य.qos;
		chunk.F.retain_dup = (uint8_t)((cmsg->तथ्य.retain&0x0F)<<4 | (cmsg->तथ्य.dup&0x0F));
		chunk.F.direction = (uint8_t)cmsg->तथ्य.direction;
		chunk.F.state = (uint8_t)cmsg->तथ्य.state;
		chunk.clientid = context->id;
		chunk.subscription_identifier = cmsg->तथ्य.subscription_identifier;

		rc = persist_chunk_client_संदेश_write_v6(db_fptr, &chunk);
		अगर(rc){
			वापस rc;
		}

		cmsg = cmsg->अग्र;
	}

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक persist__message_store_save(खाता *db_fptr)
{
	काष्ठा P_base_संदेश chunk;
	काष्ठा mosquitto_base_संदेश *base_संदेश, *base_संदेश_tmp;
	पूर्णांक rc;

	निश्चित(db_fptr);

	base_संदेश = db.संदेश_store;
	HASH_ITER(hh, db.संदेश_store, base_संदेश, base_संदेश_tmp){
		अगर(base_संदेश->ref_गिनती < 1 || base_संदेश->तथ्य.topic == शून्य){
			जारी;
		}

		स_रखो(&chunk, 0, माप(काष्ठा P_base_संदेश));

		अगर(!म_भेदन(base_संदेश->तथ्य.topic, "$SYS", 4)){
			अगर(base_संदेश->ref_गिनती <= 1 && base_संदेश->dest_id_गिनती == 0){
				/* $SYS messages that are only retained shouldn't be persisted. */
				जारी;
			}
			/* Don't save $SYS messages as retained otherwise they can give
			 * misleading information when reloaded. They should still be saved
			 * because a disconnected durable client may have them in their
			 * queue. */
			chunk.F.retain = 0;
		}अन्यथा{
			chunk.F.retain = (uint8_t)base_संदेश->तथ्य.retain;
		}

		chunk.F.store_id = base_संदेश->तथ्य.store_id;
		chunk.F.expiry_time = base_संदेश->तथ्य.expiry_time;
		chunk.F.payloadlen = base_संदेश->तथ्य.payloadlen;
		chunk.F.source_mid = base_संदेश->तथ्य.source_mid;
		अगर(base_संदेश->तथ्य.source_id){
			chunk.F.source_id_len = (uint16_t)म_माप(base_संदेश->तथ्य.source_id);
			chunk.source.id = base_संदेश->तथ्य.source_id;
		}अन्यथा{
			chunk.F.source_id_len = 0;
			chunk.source.id = शून्य;
		}
		अगर(base_संदेश->तथ्य.source_username){
			chunk.F.source_username_len = (uint16_t)म_माप(base_संदेश->तथ्य.source_username);
			chunk.source.username = base_संदेश->तथ्य.source_username;
		}अन्यथा{
			chunk.F.source_username_len = 0;
			chunk.source.username = शून्य;
		}

		chunk.F.topic_len = (uint16_t)म_माप(base_संदेश->तथ्य.topic);
		chunk.topic = base_संदेश->तथ्य.topic;

		अगर(base_संदेश->source_listener){
			chunk.F.source_port = base_संदेश->source_listener->port;
		}अन्यथा{
			chunk.F.source_port = 0;
		}
		chunk.F.qos = base_संदेश->तथ्य.qos;
		chunk.payload = base_संदेश->तथ्य.payload;
		chunk.properties = base_संदेश->तथ्य.properties;

		rc = persist__chunk_message_store_write_v6(db_fptr, &chunk);
		अगर(rc){
			वापस rc;
		}
	}

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक persist__client_save(खाता *db_fptr)
{
	काष्ठा mosquitto *context, *ctxt_tmp;
	काष्ठा P_client chunk;
	पूर्णांक rc;

	निश्चित(db_fptr);

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		स_रखो(&chunk, 0, माप(काष्ठा P_client));

		अगर(context &&
				context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE &&
#अगर_घोषित WITH_BRIDGE
				((!context->bridge && context->clean_start == false)
				|| (context->bridge && context->bridge->clean_start_local == false))
#अन्यथा
				context->clean_start == false
#पूर्ण_अगर
				){
			chunk.F.session_expiry_time = context->session_expiry_time;
			अगर(context->session_expiry_interval != MQTT_SESSION_EXPIRY_NEVER
					&& context->session_expiry_time == 0){

				chunk.F.session_expiry_time = context->session_expiry_interval + db.now_real_s;
			}अन्यथा{
				chunk.F.session_expiry_time = context->session_expiry_time;
			}
			chunk.F.session_expiry_interval = context->session_expiry_interval;
			chunk.F.last_mid = context->last_mid;
			chunk.F.id_len = (uint16_t)म_माप(context->id);
			chunk.clientid = context->id;
			अगर(context->username){
				chunk.F.username_len = (uint16_t)म_माप(context->username);
				chunk.username = context->username;
			}
			अगर(context->listener){
				chunk.F.listener_port = context->listener->port;
			}

			अगर(chunk.F.id_len == 0){
				/* This should never happen, but in case we have a client with
				 * zero length ID, don't persist them. */
				जारी;
			}

			rc = persist__chunk_client_write_v6(db_fptr, &chunk);
			अगर(rc){
				वापस rc;
			}

			अगर(persist__client_messages_save(db_fptr, context, context->msgs_in.inflight)){
				वापस 1;
			}
			अगर(persist__client_messages_save(db_fptr, context, context->msgs_in.queued)){
				वापस 1;
			}
			अगर(persist__client_messages_save(db_fptr, context, context->msgs_out.inflight)){
				वापस 1;
			}
			अगर(persist__client_messages_save(db_fptr, context, context->msgs_out.queued)){
				वापस 1;
			}
		}
	}

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक persist__subs_save(खाता *db_fptr, काष्ठा mosquitto__subhier *गाँठ, स्थिर अक्षर *topic, पूर्णांक level)
{
	काष्ठा mosquitto__subhier *subhier, *subhier_tmp;
	काष्ठा mosquitto__subleaf *sub;
	काष्ठा P_sub sub_chunk;
	अक्षर *thistopic;
	माप_प्रकार slen;
	पूर्णांक rc;

	slen = म_माप(topic) + गाँठ->topic_len + 2;
	thistopic = mosquitto_malloc(माप(अक्षर)*slen);
	अगर(!thistopic){
		वापस MOSQ_ERR_NOMEM;
	}
	अगर(level > 1 || म_माप(topic)){
		snprintf(thistopic, slen, "%s/%s", topic, गाँठ->topic);
	}अन्यथा{
		snprintf(thistopic, slen, "%s", गाँठ->topic);
	}

	sub = गाँठ->subs;
	जबतक(sub){
		अगर(sub->context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE
				&& sub->context->clean_start == false
				&& sub->context->id){

			स_रखो(&sub_chunk, 0, माप(काष्ठा P_sub));

			sub_chunk.F.identifier = sub->identifier;
			sub_chunk.F.id_len = (uint16_t)म_माप(sub->context->id);
			sub_chunk.F.topic_len = (uint16_t)म_माप(thistopic);
			sub_chunk.F.qos = MQTT_SUB_OPT_GET_QOS(sub->subscription_options);
			sub_chunk.F.options = sub->subscription_options & 0xFC;
			sub_chunk.clientid = sub->context->id;
			sub_chunk.topic = thistopic;

			rc = persist__chunk_sub_write_v6(db_fptr, &sub_chunk);
			अगर(rc){
				mosquitto_FREE(thistopic);
				वापस rc;
			}
		}
		sub = sub->अग्र;
	}

	HASH_ITER(hh, गाँठ->children, subhier, subhier_tmp){
		persist__subs_save(db_fptr, subhier, thistopic, level+1);
	}
	mosquitto_FREE(thistopic);
	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक persist__subs_save_all(खाता *db_fptr)
{
	काष्ठा mosquitto__subhier *subhier, *subhier_tmp;

	HASH_ITER(hh, db.normal_subs, subhier, subhier_tmp){
		अगर(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	HASH_ITER(hh, db.shared_subs, subhier, subhier_tmp){
		अगर(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक persist__retain_save(खाता *db_fptr, काष्ठा mosquitto__retainhier *गाँठ, पूर्णांक level)
{
	काष्ठा mosquitto__retainhier *retainhier, *retainhier_tmp;
	काष्ठा P_retain retain_chunk;
	पूर्णांक rc;

	अगर(गाँठ->retained && म_भेदन(गाँठ->retained->तथ्य.topic, "$SYS", 4)){
		स_रखो(&retain_chunk, 0, माप(काष्ठा P_retain));

		/* Don't save $SYS messages. */
		retain_chunk.F.store_id = गाँठ->retained->तथ्य.store_id;
		rc = persist__chunk_retain_write_v6(db_fptr, &retain_chunk);
		अगर(rc){
			वापस rc;
		}
	}

	HASH_ITER(hh, गाँठ->children, retainhier, retainhier_tmp){
		persist__retain_save(db_fptr, retainhier, level+1);
	}
	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक persist__retain_save_all(खाता *db_fptr)
{
	काष्ठा mosquitto__retainhier *retainhier, *retainhier_tmp;

	HASH_ITER(hh, db.retains, retainhier, retainhier_tmp){
		अगर(retainhier->children){
			persist__retain_save(db_fptr, retainhier->children, 0);
		}
	}

	वापस MOSQ_ERR_SUCCESS;
}

अटल पूर्णांक persist_write_तथ्य(खाता *db_fptr, व्योम *user_तथ्य);


अटल व्योम persist__log_write_error(स्थिर अक्षर *संदेश)
{
	log__printf(शून्य, MOSQ_LOG_ERR, "Error saving in-memory database, %s", संदेश);
}


पूर्णांक persist__backup(bool shutdown)
{
	अगर(db.config == शून्य){
		वापस MOSQ_ERR_INVAL;
	}
	अगर(db.config->persistence == false){
		वापस MOSQ_ERR_SUCCESS;
	}
	अगर(db.config->persistence_filepath == शून्य){
		वापस MOSQ_ERR_INVAL;
	}

	log__printf(शून्य, MOSQ_LOG_INFO, "Saving in-memory database to %s.", db.config->persistence_filepath);

	वापस mosquitto_write_file(db.config->persistence_filepath, true, &persist_write_तथ्य, &shutdown, &persist__log_write_error);
}


अटल पूर्णांक persist_write_तथ्य(खाता *db_fptr, व्योम *user_तथ्य)
{
	bool shutdown = *(bool *)(user_तथ्य);
	uint32_t db_version_w = htonl(MOSQ_DB_VERSION);
	uint32_t crc = 0;
	स्थिर अक्षर *err;
	काष्ठा PF_cfg cfg_chunk;
	पूर्णांक rc = MOSQ_ERR_UNKNOWN;

	/* Header */
	write_e(db_fptr, magic, 15);
	write_e(db_fptr, &crc, माप(uint32_t));
	write_e(db_fptr, &db_version_w, माप(uint32_t));

	स_रखो(&cfg_chunk, 0, माप(काष्ठा PF_cfg));
	cfg_chunk.last_db_id = db.last_db_id;
	cfg_chunk.shutdown = shutdown;
	cfg_chunk.dbid_आकार = माप(dbid_t);
	अगर(persist__chunk_cfg_write_v6(db_fptr, &cfg_chunk)){
		जाओ error;
	}

	अगर(persist__message_store_save(db_fptr)){
		जाओ error;
	}

	अगर(persist__client_save(db_fptr)
			|| persist__subs_save_all(db_fptr)
			|| persist__retain_save_all(db_fptr)){
		जाओ error;
	}
	वापस MOSQ_ERR_SUCCESS;

error:
	err = म_त्रुटि(त्रुटि_सं);
	log__printf(शून्य, MOSQ_LOG_ERR, "Error during saving in-memory database %s: %s.", db.config->persistence_filepath, err);
	अगर(db_fptr){
		ख_बंद(db_fptr);
	}
	वापस rc;
}


#पूर्ण_अगर
