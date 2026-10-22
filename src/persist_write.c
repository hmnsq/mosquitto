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

#यदि_परिभाषितम् WITH_PERSISTENCE

#यदि_अपरिभाषितम् WIN32
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः
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
	यावत्(cmsg){
		यदि(!वाक्ये_तुलय(cmsg->आधारः_सन्देशः->दत्त.topic, "$SYS", 4)
				&& cmsg->आधारः_सन्देशः->ref_गणनम् <= 1
				&& cmsg->आधारः_सन्देशः->dest_id_गणनम् == 0){

			/* This $SYS message won't have been persisted, so we can't persist
			 * this client message. */
			cmsg = cmsg->अग्र;
			अनुवर्त;
		}

		स्मृतिं_पूरय(&chunk, 0, परिमाण(संरचना P_client_सन्देशः));

		chunk.F.store_id = cmsg->आधारः_सन्देशः->दत्त.store_id;
		chunk.F.mid = cmsg->दत्त.mid;
		chunk.F.id_len = (uint16_t)वाक्यदैर्घ्यम्(context->id);
		chunk.F.qos = cmsg->दत्त.qos;
		chunk.F.retain_dup = (uint8_t)((cmsg->दत्त.retain&0x0F)<<4 | (cmsg->दत्त.dup&0x0F));
		chunk.F.direction = (uint8_t)cmsg->दत्त.direction;
		chunk.F.state = (uint8_t)cmsg->दत्त.state;
		chunk.clientid = context->id;
		chunk.subscription_identifier = cmsg->दत्त.subscription_identifier;

		rc = persist_chunk_client_सन्देशः_write_v6(db_fptr, &chunk);
		यदि(rc){
			प्रत्यागम rc;
		}

		cmsg = cmsg->अग्र;
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क persist_सन्देशः_store_save(FILE *db_fptr)
{
	संरचना P_आधारः_सन्देशः chunk;
	संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः, *आधारः_सन्देशः_tmp;
	पूर्णाङ्क rc;

	प्रतिज्ञा(db_fptr);

	आधारः_सन्देशः = db.सन्देशः_store;
	HASH_ITER(hh, db.सन्देशः_store, आधारः_सन्देशः, आधारः_सन्देशः_tmp){
		यदि(आधारः_सन्देशः->ref_गणनम् < 1 || आधारः_सन्देशः->दत्त.topic == रिक्त){
			अनुवर्त;
		}

		स्मृतिं_पूरय(&chunk, 0, परिमाण(संरचना P_आधारः_सन्देशः));

		यदि(!वाक्ये_तुलय(आधारः_सन्देशः->दत्त.topic, "$SYS", 4)){
			यदि(आधारः_सन्देशः->ref_गणनम् <= 1 && आधारः_सन्देशः->dest_id_गणनम् == 0){
				/* $SYS messages that are only retained shouldn't be persisted. */
				अनुवर्त;
			}
			/* Don't save $SYS messages as retained otherwise they can give
			 * misleading information when reloaded. They should still be saved
			 * because a disconnected durable client may have them in their
			 * queue. */
			chunk.F.retain = 0;
		}अन्यथा{
			chunk.F.retain = (uint8_t)आधारः_सन्देशः->दत्त.retain;
		}

		chunk.F.store_id = आधारः_सन्देशः->दत्त.store_id;
		chunk.F.expiry_time = आधारः_सन्देशः->दत्त.expiry_time;
		chunk.F.payloadlen = आधारः_सन्देशः->दत्त.payloadlen;
		chunk.F.source_mid = आधारः_सन्देशः->दत्त.source_mid;
		यदि(आधारः_सन्देशः->दत्त.source_id){
			chunk.F.source_id_len = (uint16_t)वाक्यदैर्घ्यम्(आधारः_सन्देशः->दत्त.source_id);
			chunk.source.id = आधारः_सन्देशः->दत्त.source_id;
		}अन्यथा{
			chunk.F.source_id_len = 0;
			chunk.source.id = रिक्त;
		}
		यदि(आधारः_सन्देशः->दत्त.source_username){
			chunk.F.source_username_len = (uint16_t)वाक्यदैर्घ्यम्(आधारः_सन्देशः->दत्त.source_username);
			chunk.source.username = आधारः_सन्देशः->दत्त.source_username;
		}अन्यथा{
			chunk.F.source_username_len = 0;
			chunk.source.username = रिक्त;
		}

		chunk.F.topic_len = (uint16_t)वाक्यदैर्घ्यम्(आधारः_सन्देशः->दत्त.topic);
		chunk.topic = आधारः_सन्देशः->दत्त.topic;

		यदि(आधारः_सन्देशः->source_listener){
			chunk.F.source_पत्तनम् = आधारः_सन्देशः->source_listener->पत्तनम्;
		}अन्यथा{
			chunk.F.source_पत्तनम् = 0;
		}
		chunk.F.qos = आधारः_सन्देशः->दत्त.qos;
		chunk.payload = आधारः_सन्देशः->दत्त.payload;
		chunk.properties = आधारः_सन्देशः->दत्त.properties;

		rc = persist_chunk_सन्देशः_store_write_v6(db_fptr, &chunk);
		यदि(rc){
			प्रत्यागम rc;
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क persist__client_save(FILE *db_fptr)
{
	संरचना mosquitto *context, *ctxt_tmp;
	संरचना P_client chunk;
	पूर्णाङ्क rc;

	प्रतिज्ञा(db_fptr);

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		स्मृतिं_पूरय(&chunk, 0, परिमाण(संरचना P_client));

		यदि(context &&
				context->session_expiry_interval != MQTT_SESSION_EXPIRY_तात्कालिकम् &&
#यदि_परिभाषितम् WITH_BRIDGE
				((!context->bridge && context->clean_start == false)
				|| (context->bridge && context->bridge->clean_start_local == false))
#अन्यथा
				context->clean_start == false
#यदि_अन्तः
				){
			chunk.F.session_expiry_time = context->session_expiry_time;
			यदि(context->session_expiry_interval != MQTT_SESSION_EXPIRY_NEVER
					&& context->session_expiry_time == 0){

				chunk.F.session_expiry_time = context->session_expiry_interval + db.now_real_s;
			}अन्यथा{
				chunk.F.session_expiry_time = context->session_expiry_time;
			}
			chunk.F.session_expiry_interval = context->session_expiry_interval;
			chunk.F.चरमः_mid = context->चरमः_mid;
			chunk.F.id_len = (uint16_t)वाक्यदैर्घ्यम्(context->id);
			chunk.clientid = context->id;
			यदि(context->username){
				chunk.F.username_len = (uint16_t)वाक्यदैर्घ्यम्(context->username);
				chunk.username = context->username;
			}
			यदि(context->listener){
				chunk.F.listener_पत्तनम् = context->listener->पत्तनम्;
			}

			यदि(chunk.F.id_len == 0){
				/* This should never happen, but in case we have a client with
				 * zero length ID, don't persist them. */
				अनुवर्त;
			}

			rc = persist__chunk_client_write_v6(db_fptr, &chunk);
			यदि(rc){
				प्रत्यागम rc;
			}

			यदि(persist__client_messages_save(db_fptr, context, context->msgs_अन्तर्.inflight)){
				प्रत्यागम 1;
			}
			यदि(persist__client_messages_save(db_fptr, context, context->msgs_अन्तर्.queued)){
				प्रत्यागम 1;
			}
			यदि(persist__client_messages_save(db_fptr, context, context->msgs_बहिर्गमनम्.inflight)){
				प्रत्यागम 1;
			}
			यदि(persist__client_messages_save(db_fptr, context, context->msgs_बहिर्गमनम्.queued)){
				प्रत्यागम 1;
			}
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क persist__subs_save(FILE *db_fptr, संरचना mosquitto__subhier *ग्रन्थिः, ध्रुव वर्ण *topic, पूर्णाङ्क level)
{
	संरचना mosquitto__subhier *subhier, *subhier_tmp;
	संरचना mosquitto__subleaf *sub;
	संरचना P_sub sub_chunk;
	वर्ण *thistopic;
	size_t slen;
	पूर्णाङ्क rc;

	slen = वाक्यदैर्घ्यम्(topic) + ग्रन्थिः->topic_len + 2;
	thistopic = mosquitto_malloc(परिमाण(वर्ण)*slen);
	यदि(!thistopic){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	यदि(level > 1 || वाक्यदैर्घ्यम्(topic)){
		snprintf(thistopic, slen, "%s/%s", topic, ग्रन्थिः->topic);
	}अन्यथा{
		snprintf(thistopic, slen, "%s", ग्रन्थिः->topic);
	}

	sub = ग्रन्थिः->subs;
	यावत्(sub){
		यदि(sub->context->session_expiry_interval != MQTT_SESSION_EXPIRY_तात्कालिकम्
				&& sub->context->clean_start == false
				&& sub->context->id){

			स्मृतिं_पूरय(&sub_chunk, 0, परिमाण(संरचना P_sub));

			sub_chunk.F.identifier = sub->identifier;
			sub_chunk.F.id_len = (uint16_t)वाक्यदैर्घ्यम्(sub->context->id);
			sub_chunk.F.topic_len = (uint16_t)वाक्यदैर्घ्यम्(thistopic);
			sub_chunk.F.qos = MQTT_SUB_OPT_GET_QOS(sub->subscription_options);
			sub_chunk.F.options = sub->subscription_options & 0xFC;
			sub_chunk.clientid = sub->context->id;
			sub_chunk.topic = thistopic;

			rc = persist__chunk_sub_write_v6(db_fptr, &sub_chunk);
			यदि(rc){
				mosquitto_FREE(thistopic);
				प्रत्यागम rc;
			}
		}
		sub = sub->अग्र;
	}

	HASH_ITER(hh, ग्रन्थिः->children, subhier, subhier_tmp){
		persist__subs_save(db_fptr, subhier, thistopic, level+1);
	}
	mosquitto_FREE(thistopic);
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क persist__subs_save_all(FILE *db_fptr)
{
	संरचना mosquitto__subhier *subhier, *subhier_tmp;

	HASH_ITER(hh, db.प्रसामान्यम्_subs, subhier, subhier_tmp){
		यदि(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	HASH_ITER(hh, db.shared_subs, subhier, subhier_tmp){
		यदि(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क persist__retain_save(FILE *db_fptr, संरचना mosquitto__retainhier *ग्रन्थिः, पूर्णाङ्क level)
{
	संरचना mosquitto__retainhier *retainhier, *retainhier_tmp;
	संरचना P_retain retain_chunk;
	पूर्णाङ्क rc;

	यदि(ग्रन्थिः->retained && वाक्ये_तुलय(ग्रन्थिः->retained->दत्त.topic, "$SYS", 4)){
		स्मृतिं_पूरय(&retain_chunk, 0, परिमाण(संरचना P_retain));

		/* Don't save $SYS messages. */
		retain_chunk.F.store_id = ग्रन्थिः->retained->दत्त.store_id;
		rc = persist__chunk_retain_write_v6(db_fptr, &retain_chunk);
		यदि(rc){
			प्रत्यागम rc;
		}
	}

	HASH_ITER(hh, ग्रन्थिः->children, retainhier, retainhier_tmp){
		persist__retain_save(db_fptr, retainhier, level+1);
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क persist__retain_save_all(FILE *db_fptr)
{
	संरचना mosquitto__retainhier *retainhier, *retainhier_tmp;

	HASH_ITER(hh, db.retains, retainhier, retainhier_tmp){
		यदि(retainhier->children){
			persist__retain_save(db_fptr, retainhier->children, 0);
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}

स्थिर पूर्णाङ्क persist_write_दत्त(FILE *db_fptr, शून्य *user_दत्त);


स्थिर शून्य persist__log_write_error(ध्रुव वर्ण *सन्देशः)
{
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error saving in-memory database, %s", सन्देशः);
}


पूर्णाङ्क persist__backup(bool shutdown)
{
	यदि(db.config == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि(db.config->persistence == false){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}
	यदि(db.config->persistence_filepath == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	log__printf(रिक्त, MOSQ_LOG_INFO, "Saving in-memory database to %s.", db.config->persistence_filepath);

	प्रत्यागम mosquitto_write_file(db.config->persistence_filepath, true, &persist_write_दत्त, &shutdown, &persist__log_write_error);
}


स्थिर पूर्णाङ्क persist_write_दत्त(FILE *db_fptr, शून्य *user_दत्त)
{
	bool shutdown = *(bool *)(user_दत्त);
	uint32_t db_संस्करणम्_w = htonl(MOSQ_DB_संस्करणम्);
	uint32_t crc = 0;
	ध्रुव वर्ण *err;
	संरचना PF_cfg cfg_chunk;
	पूर्णाङ्क rc = MOSQ_ERR_UNKNOWN;

	/*शीर्षकम्*/
	write_e(db_fptr, magic, 15);
	write_e(db_fptr, &crc, परिमाण(uint32_t));
	write_e(db_fptr, &db_संस्करणम्_w, परिमाण(uint32_t));

	स्मृतिं_पूरय(&cfg_chunk, 0, परिमाण(संरचना PF_cfg));
	cfg_chunk.चरमः_db_id = db.चरमः_db_id;
	cfg_chunk.shutdown = shutdown;
	cfg_chunk.dbid_आकार = परिमाण(dbid_t);
	यदि(persist__chunk_cfg_write_v6(db_fptr, &cfg_chunk)){
		गच्छ error;
	}

	यदि(persist_सन्देशः_store_save(db_fptr)){
		गच्छ error;
	}

	यदि(persist__client_save(db_fptr)
			|| persist__subs_save_all(db_fptr)
			|| persist__retain_save_all(db_fptr)){
		गच्छ error;
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;

error:
	err = strerror(errno);
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error during saving in-memory database %s: %s.", db.config->persistence_filepath, err);
	यदि(db_fptr){
		fclose(db_fptr);
	}
	प्रत्यागम rc;
}


#यदि_अन्तः
