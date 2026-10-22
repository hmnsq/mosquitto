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

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "persist.h"
#अन्तर्भाव "util_mosq.h"


स्थिर पूर्णांक persist__client_messages_save(खाता *db_fptr, संरचना mosquitto *context, संरचना mosquitto_client_संदेश *queue)
{
	संरचना P_client_संदेश chunk;
	संरचना mosquitto_client_संदेश *cmsg;
	पूर्णांक rc;

	प्रतिज्ञा(db_fptr);
	प्रतिज्ञा(context);

	cmsg = queue;
	पर्यंत(cmsg){
		जर(!म_भेदन(cmsg->base_संदेश->data.topic, "$SYS", 4)
				&& cmsg->base_संदेश->ref_मोजणी <= 1
				&& cmsg->base_संदेश->dest_id_मोजणी == 0){

			/* This $SYS message won't have been persisted, so we can't persist
			 * this client message. */
			cmsg = cmsg->next;
			सुरूठेवा;
		}

		स_रखो(&chunk, 0, आकार(संरचना P_client_संदेश));

		chunk.F.store_id = cmsg->base_संदेश->data.store_id;
		chunk.F.mid = cmsg->data.mid;
		chunk.F.id_len = (uint16_t)म_माप(context->id);
		chunk.F.qos = cmsg->data.qos;
		chunk.F.retain_dup = (uint8_t)((cmsg->data.retain&0x0F)<<4 | (cmsg->data.dup&0x0F));
		chunk.F.direction = (uint8_t)cmsg->data.direction;
		chunk.F.state = (uint8_t)cmsg->data.state;
		chunk.clientid = context->id;
		chunk.subscription_identifier = cmsg->data.subscription_identifier;

		rc = persist_chunk_client_संदेश_write_v6(db_fptr, &chunk);
		जर(rc){
			परत rc;
		}

		cmsg = cmsg->next;
	}

	परत MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णांक persist__message_store_save(खाता *db_fptr)
{
	संरचना P_base_संदेश chunk;
	संरचना mosquitto_base_संदेश *base_संदेश, *base_संदेश_tmp;
	पूर्णांक rc;

	प्रतिज्ञा(db_fptr);

	base_संदेश = db.संदेश_store;
	HASH_ITER(hh, db.संदेश_store, base_संदेश, base_संदेश_tmp){
		जर(base_संदेश->ref_मोजणी < 1 || base_संदेश->data.topic == रिक्त){
			सुरूठेवा;
		}

		स_रखो(&chunk, 0, आकार(संरचना P_base_संदेश));

		जर(!म_भेदन(base_संदेश->data.topic, "$SYS", 4)){
			जर(base_संदेश->ref_मोजणी <= 1 && base_संदेश->dest_id_मोजणी == 0){
				/* $SYS messages that are only retained shouldn't be persisted. */
				सुरूठेवा;
			}
			/* Don't save $SYS messages as retained otherwise they can give
			 * misleading information when reloaded. They should still be saved
			 * because a disconnected durable client may have them in their
			 * queue. */
			chunk.F.retain = 0;
		}अन्यथा{
			chunk.F.retain = (uint8_t)base_संदेश->data.retain;
		}

		chunk.F.store_id = base_संदेश->data.store_id;
		chunk.F.expiry_time = base_संदेश->data.expiry_time;
		chunk.F.payloadlen = base_संदेश->data.payloadlen;
		chunk.F.source_mid = base_संदेश->data.source_mid;
		जर(base_संदेश->data.source_id){
			chunk.F.source_id_len = (uint16_t)म_माप(base_संदेश->data.source_id);
			chunk.source.id = base_संदेश->data.source_id;
		}अन्यथा{
			chunk.F.source_id_len = 0;
			chunk.source.id = रिक्त;
		}
		जर(base_संदेश->data.source_username){
			chunk.F.source_username_len = (uint16_t)म_माप(base_संदेश->data.source_username);
			chunk.source.username = base_संदेश->data.source_username;
		}अन्यथा{
			chunk.F.source_username_len = 0;
			chunk.source.username = रिक्त;
		}

		chunk.F.topic_len = (uint16_t)म_माप(base_संदेश->data.topic);
		chunk.topic = base_संदेश->data.topic;

		जर(base_संदेश->source_listener){
			chunk.F.source_port = base_संदेश->source_listener->port;
		}अन्यथा{
			chunk.F.source_port = 0;
		}
		chunk.F.qos = base_संदेश->data.qos;
		chunk.payload = base_संदेश->data.payload;
		chunk.properties = base_संदेश->data.properties;

		rc = persist__chunk_message_store_write_v6(db_fptr, &chunk);
		जर(rc){
			परत rc;
		}
	}

	परत MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णांक persist__client_save(खाता *db_fptr)
{
	संरचना mosquitto *context, *ctxt_tmp;
	संरचना P_client chunk;
	पूर्णांक rc;

	प्रतिज्ञा(db_fptr);

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		स_रखो(&chunk, 0, आकार(संरचना P_client));

		जर(context &&
				context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE &&
#ifdef WITH_BRIDGE
				((!context->bridge && context->clean_start == false)
				|| (context->bridge && context->bridge->clean_start_local == false))
#अन्यथा
				context->clean_start == false
#endif
				){
			chunk.F.session_expiry_time = context->session_expiry_time;
			जर(context->session_expiry_interval != MQTT_SESSION_EXPIRY_NEVER
					&& context->session_expiry_time == 0){

				chunk.F.session_expiry_time = context->session_expiry_interval + db.now_real_s;
			}अन्यथा{
				chunk.F.session_expiry_time = context->session_expiry_time;
			}
			chunk.F.session_expiry_interval = context->session_expiry_interval;
			chunk.F.last_mid = context->last_mid;
			chunk.F.id_len = (uint16_t)म_माप(context->id);
			chunk.clientid = context->id;
			जर(context->username){
				chunk.F.username_len = (uint16_t)म_माप(context->username);
				chunk.username = context->username;
			}
			जर(context->listener){
				chunk.F.listener_port = context->listener->port;
			}

			जर(chunk.F.id_len == 0){
				/* This should never happen, but in case we have a client with
				 * zero length ID, don't persist them. */
				सुरूठेवा;
			}

			rc = persist__chunk_client_write_v6(db_fptr, &chunk);
			जर(rc){
				परत rc;
			}

			जर(persist__client_messages_save(db_fptr, context, context->msgs_in.inflight)){
				परत 1;
			}
			जर(persist__client_messages_save(db_fptr, context, context->msgs_in.queued)){
				परत 1;
			}
			जर(persist__client_messages_save(db_fptr, context, context->msgs_out.inflight)){
				परत 1;
			}
			जर(persist__client_messages_save(db_fptr, context, context->msgs_out.queued)){
				परत 1;
			}
		}
	}

	परत MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णांक persist__subs_save(खाता *db_fptr, संरचना mosquitto__subhier *गाठ, स्थिरांक अक्षर *topic, पूर्णांक level)
{
	संरचना mosquitto__subhier *subhier, *subhier_tmp;
	संरचना mosquitto__subleaf *sub;
	संरचना P_sub sub_chunk;
	अक्षर *thistopic;
	माप_प्रकार slen;
	पूर्णांक rc;

	slen = म_माप(topic) + गाठ->topic_len + 2;
	thistopic = mosquitto_malloc(आकार(अक्षर)*slen);
	जर(!thistopic){
		परत MOSQ_ERR_NOMEM;
	}
	जर(level > 1 || म_माप(topic)){
		snprintf(thistopic, slen, "%s/%s", topic, गाठ->topic);
	}अन्यथा{
		snprintf(thistopic, slen, "%s", गाठ->topic);
	}

	sub = गाठ->subs;
	पर्यंत(sub){
		जर(sub->context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE
				&& sub->context->clean_start == false
				&& sub->context->id){

			स_रखो(&sub_chunk, 0, आकार(संरचना P_sub));

			sub_chunk.F.identifier = sub->identifier;
			sub_chunk.F.id_len = (uint16_t)म_माप(sub->context->id);
			sub_chunk.F.topic_len = (uint16_t)म_माप(thistopic);
			sub_chunk.F.qos = MQTT_SUB_OPT_GET_QOS(sub->subscription_options);
			sub_chunk.F.options = sub->subscription_options & 0xFC;
			sub_chunk.clientid = sub->context->id;
			sub_chunk.topic = thistopic;

			rc = persist__chunk_sub_write_v6(db_fptr, &sub_chunk);
			जर(rc){
				mosquitto_FREE(thistopic);
				परत rc;
			}
		}
		sub = sub->next;
	}

	HASH_ITER(hh, गाठ->children, subhier, subhier_tmp){
		persist__subs_save(db_fptr, subhier, thistopic, level+1);
	}
	mosquitto_FREE(thistopic);
	परत MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णांक persist__subs_save_all(खाता *db_fptr)
{
	संरचना mosquitto__subhier *subhier, *subhier_tmp;

	HASH_ITER(hh, db.normal_subs, subhier, subhier_tmp){
		जर(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	HASH_ITER(hh, db.shared_subs, subhier, subhier_tmp){
		जर(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	परत MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णांक persist__retain_save(खाता *db_fptr, संरचना mosquitto__retainhier *गाठ, पूर्णांक level)
{
	संरचना mosquitto__retainhier *retainhier, *retainhier_tmp;
	संरचना P_retain retain_chunk;
	पूर्णांक rc;

	जर(गाठ->retained && म_भेदन(गाठ->retained->data.topic, "$SYS", 4)){
		स_रखो(&retain_chunk, 0, आकार(संरचना P_retain));

		/* Don't save $SYS messages. */
		retain_chunk.F.store_id = गाठ->retained->data.store_id;
		rc = persist__chunk_retain_write_v6(db_fptr, &retain_chunk);
		जर(rc){
			परत rc;
		}
	}

	HASH_ITER(hh, गाठ->children, retainhier, retainhier_tmp){
		persist__retain_save(db_fptr, retainhier, level+1);
	}
	परत MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णांक persist__retain_save_all(खाता *db_fptr)
{
	संरचना mosquitto__retainhier *retainhier, *retainhier_tmp;

	HASH_ITER(hh, db.retains, retainhier, retainhier_tmp){
		जर(retainhier->children){
			persist__retain_save(db_fptr, retainhier->children, 0);
		}
	}

	परत MOSQ_ERR_SUCCESS;
}

स्थिर पूर्णांक persist__write_data(खाता *db_fptr, शून्य *user_data);


स्थिर शून्य persist__log_write_error(स्थिरांक अक्षर *संदेश)
{
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error saving in-memory database, %s", संदेश);
}


पूर्णांक persist__backup(bool shutdown)
{
	जर(db.config == रिक्त){
		परत MOSQ_ERR_INVAL;
	}
	जर(db.config->persistence == false){
		परत MOSQ_ERR_SUCCESS;
	}
	जर(db.config->persistence_filepath == रिक्त){
		परत MOSQ_ERR_INVAL;
	}

	log__printf(रिक्त, MOSQ_LOG_INFO, "Saving in-memory database to %s.", db.config->persistence_filepath);

	परत mosquitto_write_file(db.config->persistence_filepath, true, &persist__write_data, &shutdown, &persist__log_write_error);
}


स्थिर पूर्णांक persist__write_data(खाता *db_fptr, शून्य *user_data)
{
	bool shutdown = *(bool *)(user_data);
	uint32_t db_version_w = htonl(MOSQ_DB_VERSION);
	uint32_t crc = 0;
	स्थिरांक अक्षर *err;
	संरचना PF_cfg cfg_chunk;
	पूर्णांक rc = MOSQ_ERR_UNKNOWN;

	/* Header */
	write_e(db_fptr, magic, 15);
	write_e(db_fptr, &crc, आकार(uint32_t));
	write_e(db_fptr, &db_version_w, आकार(uint32_t));

	स_रखो(&cfg_chunk, 0, आकार(संरचना PF_cfg));
	cfg_chunk.last_db_id = db.last_db_id;
	cfg_chunk.shutdown = shutdown;
	cfg_chunk.dbid_size = आकार(dbid_t);
	जर(persist__chunk_cfg_write_v6(db_fptr, &cfg_chunk)){
		जा error;
	}

	जर(persist__message_store_save(db_fptr)){
		जा error;
	}

	जर(persist__client_save(db_fptr)
			|| persist__subs_save_all(db_fptr)
			|| persist__retain_save_all(db_fptr)){
		जा error;
	}
	परत MOSQ_ERR_SUCCESS;

error:
	err = म_त्रुटि(त्रुटि_सं);
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error during saving in-memory database %s: %s.", db.config->persistence_filepath, err);
	जर(db_fptr){
		ख_बंद(db_fptr);
	}
	परत rc;
}


#endif
