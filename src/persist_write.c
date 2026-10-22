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


स्थिर पूर्णाङ्क persist__client_messages_save(FILE *db_fptr, संरचना mosquitto *context, संरचना mosquitto_client_सन्देशः *queue)
{
	संरचना P_client_सन्देशः chunk;
	संरचना mosquitto_client_सन्देशः *cmsg;
	पूर्णाङ्क rc;

	प्रतिज्ञा(db_fptr);
	प्रतिज्ञा(context);

	cmsg = queue;
	याव(cmsg){
		सचे(!strncmp(cmsg->base_सन्देशः->data.topic, "$SYS", 4)
				&& cmsg->base_सन्देशः->ref_गणनम् <= 1
				&& cmsg->base_सन्देशः->dest_id_गणनम् == 0){

			/* This $SYS message won't have been persisted, so we can't persist
			 * this client message. */
			cmsg = cmsg->next;
			अनुवर्त;
		}

		memset(&chunk, 0, परिमाण(संरचना P_client_सन्देशः));

		chunk.F.store_id = cmsg->base_सन्देशः->data.store_id;
		chunk.F.mid = cmsg->data.mid;
		chunk.F.id_len = (uint16_t)strlen(context->id);
		chunk.F.qos = cmsg->data.qos;
		chunk.F.retain_dup = (uint8_t)((cmsg->data.retain&0x0F)<<4 | (cmsg->data.dup&0x0F));
		chunk.F.direction = (uint8_t)cmsg->data.direction;
		chunk.F.state = (uint8_t)cmsg->data.state;
		chunk.clientid = context->id;
		chunk.subscription_identifier = cmsg->data.subscription_identifier;

		rc = persist_chunk_client_सन्देशः_write_v6(db_fptr, &chunk);
		सचे(rc){
			पटिनिवत्त rc;
		}

		cmsg = cmsg->next;
	}

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क persist__message_store_save(FILE *db_fptr)
{
	संरचना P_base_सन्देशः chunk;
	संरचना mosquitto_base_सन्देशः *base_सन्देशः, *base_सन्देशः_tmp;
	पूर्णाङ्क rc;

	प्रतिज्ञा(db_fptr);

	base_सन्देशः = db.सन्देशः_store;
	HASH_ITER(hh, db.सन्देशः_store, base_सन्देशः, base_सन्देशः_tmp){
		सचे(base_सन्देशः->ref_गणनम् < 1 || base_सन्देशः->data.topic == रिक्त){
			अनुवर्त;
		}

		memset(&chunk, 0, परिमाण(संरचना P_base_सन्देशः));

		सचे(!strncmp(base_सन्देशः->data.topic, "$SYS", 4)){
			सचे(base_सन्देशः->ref_गणनम् <= 1 && base_सन्देशः->dest_id_गणनम् == 0){
				/* $SYS messages that are only retained shouldn't be persisted. */
				अनुवर्त;
			}
			/* Don't save $SYS messages as retained otherwise they can give
			 * misleading information when reloaded. They should still be saved
			 * because a disconnected durable client may have them in their
			 * queue. */
			chunk.F.retain = 0;
		}अञ्ञथा{
			chunk.F.retain = (uint8_t)base_सन्देशः->data.retain;
		}

		chunk.F.store_id = base_सन्देशः->data.store_id;
		chunk.F.expiry_time = base_सन्देशः->data.expiry_time;
		chunk.F.payloadlen = base_सन्देशः->data.payloadlen;
		chunk.F.source_mid = base_सन्देशः->data.source_mid;
		सचे(base_सन्देशः->data.source_id){
			chunk.F.source_id_len = (uint16_t)strlen(base_सन्देशः->data.source_id);
			chunk.source.id = base_सन्देशः->data.source_id;
		}अञ्ञथा{
			chunk.F.source_id_len = 0;
			chunk.source.id = रिक्त;
		}
		सचे(base_सन्देशः->data.source_username){
			chunk.F.source_username_len = (uint16_t)strlen(base_सन्देशः->data.source_username);
			chunk.source.username = base_सन्देशः->data.source_username;
		}अञ्ञथा{
			chunk.F.source_username_len = 0;
			chunk.source.username = रिक्त;
		}

		chunk.F.topic_len = (uint16_t)strlen(base_सन्देशः->data.topic);
		chunk.topic = base_सन्देशः->data.topic;

		सचे(base_सन्देशः->source_listener){
			chunk.F.source_port = base_सन्देशः->source_listener->port;
		}अञ्ञथा{
			chunk.F.source_port = 0;
		}
		chunk.F.qos = base_सन्देशः->data.qos;
		chunk.payload = base_सन्देशः->data.payload;
		chunk.properties = base_सन्देशः->data.properties;

		rc = persist__chunk_message_store_write_v6(db_fptr, &chunk);
		सचे(rc){
			पटिनिवत्त rc;
		}
	}

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क persist__client_save(FILE *db_fptr)
{
	संरचना mosquitto *context, *ctxt_tmp;
	संरचना P_client chunk;
	पूर्णाङ्क rc;

	प्रतिज्ञा(db_fptr);

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		memset(&chunk, 0, परिमाण(संरचना P_client));

		सचे(context &&
				context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE &&
#ifdef WITH_BRIDGE
				((!context->bridge && context->clean_start == false)
				|| (context->bridge && context->bridge->clean_start_local == false))
#अञ्ञथा
				context->clean_start == false
#endif
				){
			chunk.F.session_expiry_time = context->session_expiry_time;
			सचे(context->session_expiry_interval != MQTT_SESSION_EXPIRY_NEVER
					&& context->session_expiry_time == 0){

				chunk.F.session_expiry_time = context->session_expiry_interval + db.now_real_s;
			}अञ्ञथा{
				chunk.F.session_expiry_time = context->session_expiry_time;
			}
			chunk.F.session_expiry_interval = context->session_expiry_interval;
			chunk.F.last_mid = context->last_mid;
			chunk.F.id_len = (uint16_t)strlen(context->id);
			chunk.clientid = context->id;
			सचे(context->username){
				chunk.F.username_len = (uint16_t)strlen(context->username);
				chunk.username = context->username;
			}
			सचे(context->listener){
				chunk.F.listener_port = context->listener->port;
			}

			सचे(chunk.F.id_len == 0){
				/* This should never happen, but in case we have a client with
				 * zero length ID, don't persist them. */
				अनुवर्त;
			}

			rc = persist__chunk_client_write_v6(db_fptr, &chunk);
			सचे(rc){
				पटिनिवत्त rc;
			}

			सचे(persist__client_messages_save(db_fptr, context, context->msgs_in.inflight)){
				पटिनिवत्त 1;
			}
			सचे(persist__client_messages_save(db_fptr, context, context->msgs_in.queued)){
				पटिनिवत्त 1;
			}
			सचे(persist__client_messages_save(db_fptr, context, context->msgs_out.inflight)){
				पटिनिवत्त 1;
			}
			सचे(persist__client_messages_save(db_fptr, context, context->msgs_out.queued)){
				पटिनिवत्त 1;
			}
		}
	}

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क persist__subs_save(FILE *db_fptr, संरचना mosquitto__subhier *ग्रन्थिः, ध्रुव वर्ण *topic, पूर्णाङ्क level)
{
	संरचना mosquitto__subhier *subhier, *subhier_tmp;
	संरचना mosquitto__subleaf *sub;
	संरचना P_sub sub_chunk;
	वर्ण *thistopic;
	size_t slen;
	पूर्णाङ्क rc;

	slen = strlen(topic) + ग्रन्थिः->topic_len + 2;
	thistopic = mosquitto_malloc(परिमाण(वर्ण)*slen);
	सचे(!thistopic){
		पटिनिवत्त MOSQ_ERR_NOMEM;
	}
	सचे(level > 1 || strlen(topic)){
		snprintf(thistopic, slen, "%s/%s", topic, ग्रन्थिः->topic);
	}अञ्ञथा{
		snprintf(thistopic, slen, "%s", ग्रन्थिः->topic);
	}

	sub = ग्रन्थिः->subs;
	याव(sub){
		सचे(sub->context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE
				&& sub->context->clean_start == false
				&& sub->context->id){

			memset(&sub_chunk, 0, परिमाण(संरचना P_sub));

			sub_chunk.F.identifier = sub->identifier;
			sub_chunk.F.id_len = (uint16_t)strlen(sub->context->id);
			sub_chunk.F.topic_len = (uint16_t)strlen(thistopic);
			sub_chunk.F.qos = MQTT_SUB_OPT_GET_QOS(sub->subscription_options);
			sub_chunk.F.options = sub->subscription_options & 0xFC;
			sub_chunk.clientid = sub->context->id;
			sub_chunk.topic = thistopic;

			rc = persist__chunk_sub_write_v6(db_fptr, &sub_chunk);
			सचे(rc){
				mosquitto_FREE(thistopic);
				पटिनिवत्त rc;
			}
		}
		sub = sub->next;
	}

	HASH_ITER(hh, ग्रन्थिः->children, subhier, subhier_tmp){
		persist__subs_save(db_fptr, subhier, thistopic, level+1);
	}
	mosquitto_FREE(thistopic);
	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क persist__subs_save_all(FILE *db_fptr)
{
	संरचना mosquitto__subhier *subhier, *subhier_tmp;

	HASH_ITER(hh, db.normal_subs, subhier, subhier_tmp){
		सचे(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	HASH_ITER(hh, db.shared_subs, subhier, subhier_tmp){
		सचे(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क persist__retain_save(FILE *db_fptr, संरचना mosquitto__retainhier *ग्रन्थिः, पूर्णाङ्क level)
{
	संरचना mosquitto__retainhier *retainhier, *retainhier_tmp;
	संरचना P_retain retain_chunk;
	पूर्णाङ्क rc;

	सचे(ग्रन्थिः->retained && strncmp(ग्रन्थिः->retained->data.topic, "$SYS", 4)){
		memset(&retain_chunk, 0, परिमाण(संरचना P_retain));

		/* Don't save $SYS messages. */
		retain_chunk.F.store_id = ग्रन्थिः->retained->data.store_id;
		rc = persist__chunk_retain_write_v6(db_fptr, &retain_chunk);
		सचे(rc){
			पटिनिवत्त rc;
		}
	}

	HASH_ITER(hh, ग्रन्थिः->children, retainhier, retainhier_tmp){
		persist__retain_save(db_fptr, retainhier, level+1);
	}
	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क persist__retain_save_all(FILE *db_fptr)
{
	संरचना mosquitto__retainhier *retainhier, *retainhier_tmp;

	HASH_ITER(hh, db.retains, retainhier, retainhier_tmp){
		सचे(retainhier->children){
			persist__retain_save(db_fptr, retainhier->children, 0);
		}
	}

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}

स्थिर पूर्णाङ्क persist__write_data(FILE *db_fptr, शून्य *user_data);


स्थिर शून्य persist__log_write_error(ध्रुव वर्ण *सन्देशः)
{
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error saving in-memory database, %s", सन्देशः);
}


पूर्णाङ्क persist__backup(bool shutdown)
{
	सचे(db.config == रिक्त){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}
	सचे(db.config->persistence == false){
		पटिनिवत्त MOSQ_ERR_SUCCESS;
	}
	सचे(db.config->persistence_filepath == रिक्त){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}

	log__printf(रिक्त, MOSQ_LOG_INFO, "Saving in-memory database to %s.", db.config->persistence_filepath);

	पटिनिवत्त mosquitto_write_file(db.config->persistence_filepath, true, &persist__write_data, &shutdown, &persist__log_write_error);
}


स्थिर पूर्णाङ्क persist__write_data(FILE *db_fptr, शून्य *user_data)
{
	bool shutdown = *(bool *)(user_data);
	uint32_t db_version_w = htonl(MOSQ_DB_VERSION);
	uint32_t crc = 0;
	ध्रुव वर्ण *err;
	संरचना PF_cfg cfg_chunk;
	पूर्णाङ्क rc = MOSQ_ERR_UNKNOWN;

	/* Header */
	write_e(db_fptr, magic, 15);
	write_e(db_fptr, &crc, परिमाण(uint32_t));
	write_e(db_fptr, &db_version_w, परिमाण(uint32_t));

	memset(&cfg_chunk, 0, परिमाण(संरचना PF_cfg));
	cfg_chunk.last_db_id = db.last_db_id;
	cfg_chunk.shutdown = shutdown;
	cfg_chunk.dbid_size = परिमाण(dbid_t);
	सचे(persist__chunk_cfg_write_v6(db_fptr, &cfg_chunk)){
		गच्छ error;
	}

	सचे(persist__message_store_save(db_fptr)){
		गच्छ error;
	}

	सचे(persist__client_save(db_fptr)
			|| persist__subs_save_all(db_fptr)
			|| persist__retain_save_all(db_fptr)){
		गच्छ error;
	}
	पटिनिवत्त MOSQ_ERR_SUCCESS;

error:
	err = strerror(errno);
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error during saving in-memory database %s: %s.", db.config->persistence_filepath, err);
	सचे(db_fptr){
		fclose(db_fptr);
	}
	पटिनिवत्त rc;
}


#endif
