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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif
#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <errno.h>
#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>
#ଅନ୍ତର୍ଭାଵ <time.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "persist.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__client_messages_save(FILE *db_fptr, ସଂରଚନା mosquitto *context, ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *queue)
{
	ସଂରଚନା P_client_ସନ୍ଦେଶ chunk;
	ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *cmsg;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ପ୍ରତିଜ୍ଞା(db_fptr);
	ପ୍ରତିଜ୍ଞା(context);

	cmsg = queue;
	ଯେତେବେଳେ(cmsg){
		ଯଦି(!strncmp(cmsg->base_ସନ୍ଦେଶ->data.topic, "$SYS", 4)
				&& cmsg->base_ସନ୍ଦେଶ->ref_ଗଣନା <= 1
				&& cmsg->base_ସନ୍ଦେଶ->dest_id_ଗଣନା == 0){

			/* This $SYS message won't have been persisted, so we can't persist
			 * this client message. */
			cmsg = cmsg->next;
			ଜାରି;
		}

		memset(&chunk, 0, ଆକାର(ସଂରଚନା P_client_ସନ୍ଦେଶ));

		chunk.F.store_id = cmsg->base_ସନ୍ଦେଶ->data.store_id;
		chunk.F.mid = cmsg->data.mid;
		chunk.F.id_len = (uint16_t)strlen(context->id);
		chunk.F.qos = cmsg->data.qos;
		chunk.F.retain_dup = (uint8_t)((cmsg->data.retain&0x0F)<<4 | (cmsg->data.dup&0x0F));
		chunk.F.direction = (uint8_t)cmsg->data.direction;
		chunk.F.state = (uint8_t)cmsg->data.state;
		chunk.clientid = context->id;
		chunk.subscription_identifier = cmsg->data.subscription_identifier;

		rc = persist_chunk_client_ସନ୍ଦେଶ_write_v6(db_fptr, &chunk);
		ଯଦି(rc){
			ଫେରନ୍ତୁ rc;
		}

		cmsg = cmsg->next;
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__message_store_save(FILE *db_fptr)
{
	ସଂରଚନା P_base_ସନ୍ଦେଶ chunk;
	ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ, *base_ସନ୍ଦେଶ_tmp;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ପ୍ରତିଜ୍ଞା(db_fptr);

	base_ସନ୍ଦେଶ = db.ସନ୍ଦେଶ_store;
	HASH_ITER(hh, db.ସନ୍ଦେଶ_store, base_ସନ୍ଦେଶ, base_ସନ୍ଦେଶ_tmp){
		ଯଦି(base_ସନ୍ଦେଶ->ref_ଗଣନା < 1 || base_ସନ୍ଦେଶ->data.topic == ଖାଲି){
			ଜାରି;
		}

		memset(&chunk, 0, ଆକାର(ସଂରଚନା P_base_ସନ୍ଦେଶ));

		ଯଦି(!strncmp(base_ସନ୍ଦେଶ->data.topic, "$SYS", 4)){
			ଯଦି(base_ସନ୍ଦେଶ->ref_ଗଣନା <= 1 && base_ସନ୍ଦେଶ->dest_id_ଗଣନା == 0){
				/* $SYS messages that are only retained shouldn't be persisted. */
				ଜାରି;
			}
			/* Don't save $SYS messages as retained otherwise they can give
			 * misleading information when reloaded. They should still be saved
			 * because a disconnected durable client may have them in their
			 * queue. */
			chunk.F.retain = 0;
		}ଅନ୍ୟଥା{
			chunk.F.retain = (uint8_t)base_ସନ୍ଦେଶ->data.retain;
		}

		chunk.F.store_id = base_ସନ୍ଦେଶ->data.store_id;
		chunk.F.expiry_time = base_ସନ୍ଦେଶ->data.expiry_time;
		chunk.F.payloadlen = base_ସନ୍ଦେଶ->data.payloadlen;
		chunk.F.source_mid = base_ସନ୍ଦେଶ->data.source_mid;
		ଯଦି(base_ସନ୍ଦେଶ->data.source_id){
			chunk.F.source_id_len = (uint16_t)strlen(base_ସନ୍ଦେଶ->data.source_id);
			chunk.source.id = base_ସନ୍ଦେଶ->data.source_id;
		}ଅନ୍ୟଥା{
			chunk.F.source_id_len = 0;
			chunk.source.id = ଖାଲି;
		}
		ଯଦି(base_ସନ୍ଦେଶ->data.source_username){
			chunk.F.source_username_len = (uint16_t)strlen(base_ସନ୍ଦେଶ->data.source_username);
			chunk.source.username = base_ସନ୍ଦେଶ->data.source_username;
		}ଅନ୍ୟଥା{
			chunk.F.source_username_len = 0;
			chunk.source.username = ଖାଲି;
		}

		chunk.F.topic_len = (uint16_t)strlen(base_ସନ୍ଦେଶ->data.topic);
		chunk.topic = base_ସନ୍ଦେଶ->data.topic;

		ଯଦି(base_ସନ୍ଦେଶ->source_listener){
			chunk.F.source_port = base_ସନ୍ଦେଶ->source_listener->port;
		}ଅନ୍ୟଥା{
			chunk.F.source_port = 0;
		}
		chunk.F.qos = base_ସନ୍ଦେଶ->data.qos;
		chunk.payload = base_ସନ୍ଦେଶ->data.payload;
		chunk.properties = base_ସନ୍ଦେଶ->data.properties;

		rc = persist__chunk_message_store_write_v6(db_fptr, &chunk);
		ଯଦି(rc){
			ଫେରନ୍ତୁ rc;
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__client_save(FILE *db_fptr)
{
	ସଂରଚନା mosquitto *context, *ctxt_tmp;
	ସଂରଚନା P_client chunk;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ପ୍ରତିଜ୍ଞା(db_fptr);

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		memset(&chunk, 0, ଆକାର(ସଂରଚନା P_client));

		ଯଦି(context &&
				context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE &&
#ifdef WITH_BRIDGE
				((!context->bridge && context->clean_start == false)
				|| (context->bridge && context->bridge->clean_start_local == false))
#ଅନ୍ୟଥା
				context->clean_start == false
#endif
				){
			chunk.F.session_expiry_time = context->session_expiry_time;
			ଯଦି(context->session_expiry_interval != MQTT_SESSION_EXPIRY_NEVER
					&& context->session_expiry_time == 0){

				chunk.F.session_expiry_time = context->session_expiry_interval + db.now_real_s;
			}ଅନ୍ୟଥା{
				chunk.F.session_expiry_time = context->session_expiry_time;
			}
			chunk.F.session_expiry_interval = context->session_expiry_interval;
			chunk.F.last_mid = context->last_mid;
			chunk.F.id_len = (uint16_t)strlen(context->id);
			chunk.clientid = context->id;
			ଯଦି(context->username){
				chunk.F.username_len = (uint16_t)strlen(context->username);
				chunk.username = context->username;
			}
			ଯଦି(context->listener){
				chunk.F.listener_port = context->listener->port;
			}

			ଯଦି(chunk.F.id_len == 0){
				/* This should never happen, but in case we have a client with
				 * zero length ID, don't persist them. */
				ଜାରି;
			}

			rc = persist__chunk_client_write_v6(db_fptr, &chunk);
			ଯଦି(rc){
				ଫେରନ୍ତୁ rc;
			}

			ଯଦି(persist__client_messages_save(db_fptr, context, context->msgs_in.inflight)){
				ଫେରନ୍ତୁ 1;
			}
			ଯଦି(persist__client_messages_save(db_fptr, context, context->msgs_in.queued)){
				ଫେରନ୍ତୁ 1;
			}
			ଯଦି(persist__client_messages_save(db_fptr, context, context->msgs_out.inflight)){
				ଫେରନ୍ତୁ 1;
			}
			ଯଦି(persist__client_messages_save(db_fptr, context, context->msgs_out.queued)){
				ଫେରନ୍ତୁ 1;
			}
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__subs_save(FILE *db_fptr, ସଂରଚନା mosquitto__subhier *ଗଣ୍ଠି, ଅଚଳ ଅକ୍ଷର *topic, ପୂର୍ଣ୍ଣସଂଖ୍ୟା level)
{
	ସଂରଚନା mosquitto__subhier *subhier, *subhier_tmp;
	ସଂରଚନା mosquitto__subleaf *sub;
	ସଂରଚନା P_sub sub_chunk;
	ଅକ୍ଷର *thistopic;
	size_t slen;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	slen = strlen(topic) + ଗଣ୍ଠି->topic_len + 2;
	thistopic = mosquitto_malloc(ଆକାର(ଅକ୍ଷର)*slen);
	ଯଦି(!thistopic){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}
	ଯଦି(level > 1 || strlen(topic)){
		snprintf(thistopic, slen, "%s/%s", topic, ଗଣ୍ଠି->topic);
	}ଅନ୍ୟଥା{
		snprintf(thistopic, slen, "%s", ଗଣ୍ଠି->topic);
	}

	sub = ଗଣ୍ଠି->subs;
	ଯେତେବେଳେ(sub){
		ଯଦି(sub->context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE
				&& sub->context->clean_start == false
				&& sub->context->id){

			memset(&sub_chunk, 0, ଆକାର(ସଂରଚନା P_sub));

			sub_chunk.F.identifier = sub->identifier;
			sub_chunk.F.id_len = (uint16_t)strlen(sub->context->id);
			sub_chunk.F.topic_len = (uint16_t)strlen(thistopic);
			sub_chunk.F.qos = MQTT_SUB_OPT_GET_QOS(sub->subscription_options);
			sub_chunk.F.options = sub->subscription_options & 0xFC;
			sub_chunk.clientid = sub->context->id;
			sub_chunk.topic = thistopic;

			rc = persist__chunk_sub_write_v6(db_fptr, &sub_chunk);
			ଯଦି(rc){
				mosquitto_FREE(thistopic);
				ଫେରନ୍ତୁ rc;
			}
		}
		sub = sub->next;
	}

	HASH_ITER(hh, ଗଣ୍ଠି->children, subhier, subhier_tmp){
		persist__subs_save(db_fptr, subhier, thistopic, level+1);
	}
	mosquitto_FREE(thistopic);
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__subs_save_all(FILE *db_fptr)
{
	ସଂରଚନା mosquitto__subhier *subhier, *subhier_tmp;

	HASH_ITER(hh, db.normal_subs, subhier, subhier_tmp){
		ଯଦି(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	HASH_ITER(hh, db.shared_subs, subhier, subhier_tmp){
		ଯଦି(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__retain_save(FILE *db_fptr, ସଂରଚନା mosquitto__retainhier *ଗଣ୍ଠି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା level)
{
	ସଂରଚନା mosquitto__retainhier *retainhier, *retainhier_tmp;
	ସଂରଚନା P_retain retain_chunk;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି(ଗଣ୍ଠି->retained && strncmp(ଗଣ୍ଠି->retained->data.topic, "$SYS", 4)){
		memset(&retain_chunk, 0, ଆକାର(ସଂରଚନା P_retain));

		/* Don't save $SYS messages. */
		retain_chunk.F.store_id = ଗଣ୍ଠି->retained->data.store_id;
		rc = persist__chunk_retain_write_v6(db_fptr, &retain_chunk);
		ଯଦି(rc){
			ଫେରନ୍ତୁ rc;
		}
	}

	HASH_ITER(hh, ଗଣ୍ଠି->children, retainhier, retainhier_tmp){
		persist__retain_save(db_fptr, retainhier, level+1);
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__retain_save_all(FILE *db_fptr)
{
	ସଂରଚନା mosquitto__retainhier *retainhier, *retainhier_tmp;

	HASH_ITER(hh, db.retains, retainhier, retainhier_tmp){
		ଯଦି(retainhier->children){
			persist__retain_save(db_fptr, retainhier->children, 0);
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__write_data(FILE *db_fptr, ଶୂନ୍ୟ *user_data);


ସ୍ଥିର ଶୂନ୍ୟ persist__log_write_error(ଅଚଳ ଅକ୍ଷର *ସନ୍ଦେଶ)
{
	log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error saving in-memory database, %s", ସନ୍ଦେଶ);
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__backup(bool shutdown)
{
	ଯଦି(db.config == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}
	ଯଦି(db.config->persistence == false){
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}
	ଯଦି(db.config->persistence_filepath == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	log__printf(ଖାଲି, MOSQ_LOG_INFO, "Saving in-memory database to %s.", db.config->persistence_filepath);

	ଫେରନ୍ତୁ mosquitto_write_file(db.config->persistence_filepath, true, &persist__write_data, &shutdown, &persist__log_write_error);
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__write_data(FILE *db_fptr, ଶୂନ୍ୟ *user_data)
{
	bool shutdown = *(bool *)(user_data);
	uint32_t db_version_w = htonl(MOSQ_DB_VERSION);
	uint32_t crc = 0;
	ଅଚଳ ଅକ୍ଷର *err;
	ସଂରଚନା PF_cfg cfg_chunk;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = MOSQ_ERR_UNKNOWN;

	/* Header */
	write_e(db_fptr, magic, 15);
	write_e(db_fptr, &crc, ଆକାର(uint32_t));
	write_e(db_fptr, &db_version_w, ଆକାର(uint32_t));

	memset(&cfg_chunk, 0, ଆକାର(ସଂରଚନା PF_cfg));
	cfg_chunk.last_db_id = db.last_db_id;
	cfg_chunk.shutdown = shutdown;
	cfg_chunk.dbid_size = ଆକାର(dbid_t);
	ଯଦି(persist__chunk_cfg_write_v6(db_fptr, &cfg_chunk)){
		ଯାଅ error;
	}

	ଯଦି(persist__message_store_save(db_fptr)){
		ଯାଅ error;
	}

	ଯଦି(persist__client_save(db_fptr)
			|| persist__subs_save_all(db_fptr)
			|| persist__retain_save_all(db_fptr)){
		ଯାଅ error;
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;

error:
	err = strerror(errno);
	log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error during saving in-memory database %s: %s.", db.config->persistence_filepath, err);
	ଯଦି(db_fptr){
		fclose(db_fptr);
	}
	ଫେରନ୍ତୁ rc;
}


#endif
