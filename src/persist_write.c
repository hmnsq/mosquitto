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

#அந்தர்பாவ "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#அந்தர்பாவ <arpa/inet.h>
#endif
#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <errno.h>
#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <sys/stat.h>
#அந்தர்பாவ <time.h>

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "persist.h"
#அந்தர்பாவ "util_mosq.h"


நிலையான முழு persist__client_messages_save(FILE *db_fptr, கட்டமைப்பு mosquitto *context, கட்டமைப்பு mosquitto_client_செய்தி *queue)
{
	கட்டமைப்பு P_client_செய்தி chunk;
	கட்டமைப்பு mosquitto_client_செய்தி *cmsg;
	முழு rc;

	ப்ரதிஜ்ஞா(db_fptr);
	ப்ரதிஜ்ஞா(context);

	cmsg = queue;
	போது(cmsg){
		எனில்(!strncmp(cmsg->base_செய்தி->data.topic, "$SYS", 4)
				&& cmsg->base_செய்தி->ref_மொத்தம் <= 1
				&& cmsg->base_செய்தி->dest_id_மொத்தம் == 0){

			/* This $SYS message won't have been persisted, so we can't persist
			 * this client message. */
			cmsg = cmsg->next;
			தொடர்;
		}

		memset(&chunk, 0, அளவு(கட்டமைப்பு P_client_செய்தி));

		chunk.F.store_id = cmsg->base_செய்தி->data.store_id;
		chunk.F.mid = cmsg->data.mid;
		chunk.F.id_len = (uint16_t)strlen(context->id);
		chunk.F.qos = cmsg->data.qos;
		chunk.F.retain_dup = (uint8_t)((cmsg->data.retain&0x0F)<<4 | (cmsg->data.dup&0x0F));
		chunk.F.direction = (uint8_t)cmsg->data.direction;
		chunk.F.state = (uint8_t)cmsg->data.state;
		chunk.clientid = context->id;
		chunk.subscription_identifier = cmsg->data.subscription_identifier;

		rc = persist_chunk_client_செய்தி_write_v6(db_fptr, &chunk);
		எனில்(rc){
			திரும்பு rc;
		}

		cmsg = cmsg->next;
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு persist__message_store_save(FILE *db_fptr)
{
	கட்டமைப்பு P_base_செய்தி chunk;
	கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி, *base_செய்தி_tmp;
	முழு rc;

	ப்ரதிஜ்ஞா(db_fptr);

	base_செய்தி = db.செய்தி_store;
	HASH_ITER(hh, db.செய்தி_store, base_செய்தி, base_செய்தி_tmp){
		எனில்(base_செய்தி->ref_மொத்தம் < 1 || base_செய்தி->data.topic == வெற்று){
			தொடர்;
		}

		memset(&chunk, 0, அளவு(கட்டமைப்பு P_base_செய்தி));

		எனில்(!strncmp(base_செய்தி->data.topic, "$SYS", 4)){
			எனில்(base_செய்தி->ref_மொத்தம் <= 1 && base_செய்தி->dest_id_மொத்தம் == 0){
				/* $SYS messages that are only retained shouldn't be persisted. */
				தொடர்;
			}
			/* Don't save $SYS messages as retained otherwise they can give
			 * misleading information when reloaded. They should still be saved
			 * because a disconnected durable client may have them in their
			 * queue. */
			chunk.F.retain = 0;
		}இல்லையெனில்{
			chunk.F.retain = (uint8_t)base_செய்தி->data.retain;
		}

		chunk.F.store_id = base_செய்தி->data.store_id;
		chunk.F.expiry_time = base_செய்தி->data.expiry_time;
		chunk.F.payloadlen = base_செய்தி->data.payloadlen;
		chunk.F.source_mid = base_செய்தி->data.source_mid;
		எனில்(base_செய்தி->data.source_id){
			chunk.F.source_id_len = (uint16_t)strlen(base_செய்தி->data.source_id);
			chunk.source.id = base_செய்தி->data.source_id;
		}இல்லையெனில்{
			chunk.F.source_id_len = 0;
			chunk.source.id = வெற்று;
		}
		எனில்(base_செய்தி->data.source_username){
			chunk.F.source_username_len = (uint16_t)strlen(base_செய்தி->data.source_username);
			chunk.source.username = base_செய்தி->data.source_username;
		}இல்லையெனில்{
			chunk.F.source_username_len = 0;
			chunk.source.username = வெற்று;
		}

		chunk.F.topic_len = (uint16_t)strlen(base_செய்தி->data.topic);
		chunk.topic = base_செய்தி->data.topic;

		எனில்(base_செய்தி->source_listener){
			chunk.F.source_port = base_செய்தி->source_listener->port;
		}இல்லையெனில்{
			chunk.F.source_port = 0;
		}
		chunk.F.qos = base_செய்தி->data.qos;
		chunk.payload = base_செய்தி->data.payload;
		chunk.properties = base_செய்தி->data.properties;

		rc = persist__chunk_message_store_write_v6(db_fptr, &chunk);
		எனில்(rc){
			திரும்பு rc;
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு persist__client_save(FILE *db_fptr)
{
	கட்டமைப்பு mosquitto *context, *ctxt_tmp;
	கட்டமைப்பு P_client chunk;
	முழு rc;

	ப்ரதிஜ்ஞா(db_fptr);

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		memset(&chunk, 0, அளவு(கட்டமைப்பு P_client));

		எனில்(context &&
				context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE &&
#ifdef WITH_BRIDGE
				((!context->bridge && context->clean_start == false)
				|| (context->bridge && context->bridge->clean_start_local == false))
#இல்லையெனில்
				context->clean_start == false
#endif
				){
			chunk.F.session_expiry_time = context->session_expiry_time;
			எனில்(context->session_expiry_interval != MQTT_SESSION_EXPIRY_NEVER
					&& context->session_expiry_time == 0){

				chunk.F.session_expiry_time = context->session_expiry_interval + db.now_real_s;
			}இல்லையெனில்{
				chunk.F.session_expiry_time = context->session_expiry_time;
			}
			chunk.F.session_expiry_interval = context->session_expiry_interval;
			chunk.F.last_mid = context->last_mid;
			chunk.F.id_len = (uint16_t)strlen(context->id);
			chunk.clientid = context->id;
			எனில்(context->username){
				chunk.F.username_len = (uint16_t)strlen(context->username);
				chunk.username = context->username;
			}
			எனில்(context->listener){
				chunk.F.listener_port = context->listener->port;
			}

			எனில்(chunk.F.id_len == 0){
				/* This should never happen, but in case we have a client with
				 * zero length ID, don't persist them. */
				தொடர்;
			}

			rc = persist__chunk_client_write_v6(db_fptr, &chunk);
			எனில்(rc){
				திரும்பு rc;
			}

			எனில்(persist__client_messages_save(db_fptr, context, context->msgs_in.inflight)){
				திரும்பு 1;
			}
			எனில்(persist__client_messages_save(db_fptr, context, context->msgs_in.queued)){
				திரும்பு 1;
			}
			எனில்(persist__client_messages_save(db_fptr, context, context->msgs_out.inflight)){
				திரும்பு 1;
			}
			எனில்(persist__client_messages_save(db_fptr, context, context->msgs_out.queued)){
				திரும்பு 1;
			}
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு persist__subs_save(FILE *db_fptr, கட்டமைப்பு mosquitto__subhier *முனை, மாறிலி எழுத்து *topic, முழு level)
{
	கட்டமைப்பு mosquitto__subhier *subhier, *subhier_tmp;
	கட்டமைப்பு mosquitto__subleaf *sub;
	கட்டமைப்பு P_sub sub_chunk;
	எழுத்து *thistopic;
	size_t slen;
	முழு rc;

	slen = strlen(topic) + முனை->topic_len + 2;
	thistopic = mosquitto_malloc(அளவு(எழுத்து)*slen);
	எனில்(!thistopic){
		திரும்பு MOSQ_ERR_NOMEM;
	}
	எனில்(level > 1 || strlen(topic)){
		snprintf(thistopic, slen, "%s/%s", topic, முனை->topic);
	}இல்லையெனில்{
		snprintf(thistopic, slen, "%s", முனை->topic);
	}

	sub = முனை->subs;
	போது(sub){
		எனில்(sub->context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE
				&& sub->context->clean_start == false
				&& sub->context->id){

			memset(&sub_chunk, 0, அளவு(கட்டமைப்பு P_sub));

			sub_chunk.F.identifier = sub->identifier;
			sub_chunk.F.id_len = (uint16_t)strlen(sub->context->id);
			sub_chunk.F.topic_len = (uint16_t)strlen(thistopic);
			sub_chunk.F.qos = MQTT_SUB_OPT_GET_QOS(sub->subscription_options);
			sub_chunk.F.options = sub->subscription_options & 0xFC;
			sub_chunk.clientid = sub->context->id;
			sub_chunk.topic = thistopic;

			rc = persist__chunk_sub_write_v6(db_fptr, &sub_chunk);
			எனில்(rc){
				mosquitto_FREE(thistopic);
				திரும்பு rc;
			}
		}
		sub = sub->next;
	}

	HASH_ITER(hh, முனை->children, subhier, subhier_tmp){
		persist__subs_save(db_fptr, subhier, thistopic, level+1);
	}
	mosquitto_FREE(thistopic);
	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு persist__subs_save_all(FILE *db_fptr)
{
	கட்டமைப்பு mosquitto__subhier *subhier, *subhier_tmp;

	HASH_ITER(hh, db.normal_subs, subhier, subhier_tmp){
		எனில்(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	HASH_ITER(hh, db.shared_subs, subhier, subhier_tmp){
		எனில்(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு persist__retain_save(FILE *db_fptr, கட்டமைப்பு mosquitto__retainhier *முனை, முழு level)
{
	கட்டமைப்பு mosquitto__retainhier *retainhier, *retainhier_tmp;
	கட்டமைப்பு P_retain retain_chunk;
	முழு rc;

	எனில்(முனை->retained && strncmp(முனை->retained->data.topic, "$SYS", 4)){
		memset(&retain_chunk, 0, அளவு(கட்டமைப்பு P_retain));

		/* Don't save $SYS messages. */
		retain_chunk.F.store_id = முனை->retained->data.store_id;
		rc = persist__chunk_retain_write_v6(db_fptr, &retain_chunk);
		எனில்(rc){
			திரும்பு rc;
		}
	}

	HASH_ITER(hh, முனை->children, retainhier, retainhier_tmp){
		persist__retain_save(db_fptr, retainhier, level+1);
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு persist__retain_save_all(FILE *db_fptr)
{
	கட்டமைப்பு mosquitto__retainhier *retainhier, *retainhier_tmp;

	HASH_ITER(hh, db.retains, retainhier, retainhier_tmp){
		எனில்(retainhier->children){
			persist__retain_save(db_fptr, retainhier->children, 0);
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}

நிலையான முழு persist__write_data(FILE *db_fptr, வெறுமை *user_data);


நிலையான வெறுமை persist__log_write_error(மாறிலி எழுத்து *செய்தி)
{
	log__printf(வெற்று, MOSQ_LOG_ERR, "Error saving in-memory database, %s", செய்தி);
}


முழு persist__backup(bool shutdown)
{
	எனில்(db.config == வெற்று){
		திரும்பு MOSQ_ERR_INVAL;
	}
	எனில்(db.config->persistence == false){
		திரும்பு MOSQ_ERR_SUCCESS;
	}
	எனில்(db.config->persistence_filepath == வெற்று){
		திரும்பு MOSQ_ERR_INVAL;
	}

	log__printf(வெற்று, MOSQ_LOG_INFO, "Saving in-memory database to %s.", db.config->persistence_filepath);

	திரும்பு mosquitto_write_file(db.config->persistence_filepath, true, &persist__write_data, &shutdown, &persist__log_write_error);
}


நிலையான முழு persist__write_data(FILE *db_fptr, வெறுமை *user_data)
{
	bool shutdown = *(bool *)(user_data);
	uint32_t db_version_w = htonl(MOSQ_DB_VERSION);
	uint32_t crc = 0;
	மாறிலி எழுத்து *err;
	கட்டமைப்பு PF_cfg cfg_chunk;
	முழு rc = MOSQ_ERR_UNKNOWN;

	/* Header */
	write_e(db_fptr, magic, 15);
	write_e(db_fptr, &crc, அளவு(uint32_t));
	write_e(db_fptr, &db_version_w, அளவு(uint32_t));

	memset(&cfg_chunk, 0, அளவு(கட்டமைப்பு PF_cfg));
	cfg_chunk.last_db_id = db.last_db_id;
	cfg_chunk.shutdown = shutdown;
	cfg_chunk.dbid_size = அளவு(dbid_t);
	எனில்(persist__chunk_cfg_write_v6(db_fptr, &cfg_chunk)){
		செல் error;
	}

	எனில்(persist__message_store_save(db_fptr)){
		செல் error;
	}

	எனில்(persist__client_save(db_fptr)
			|| persist__subs_save_all(db_fptr)
			|| persist__retain_save_all(db_fptr)){
		செல் error;
	}
	திரும்பு MOSQ_ERR_SUCCESS;

error:
	err = strerror(errno);
	log__printf(வெற்று, MOSQ_LOG_ERR, "Error during saving in-memory database %s: %s.", db.config->persistence_filepath, err);
	எனில்(db_fptr){
		fclose(db_fptr);
	}
	திரும்பு rc;
}


#endif
