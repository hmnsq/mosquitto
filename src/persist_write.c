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

#অন্তর্ভাব "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#অন্তর্ভাব <arpa/inet.h>
#endif
#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <errno.h>
#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <sys/stat.h>
#অন্তর্ভাব <time.h>

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "persist.h"
#অন্তর্ভাব "util_mosq.h"


স্থির পূর্ণসংখ্যা persist__client_messages_save(FILE *db_fptr, কাঠামো mosquitto *context, কাঠামো mosquitto_client_বার্তা *queue)
{
	কাঠামো P_client_বার্তা chunk;
	কাঠামো mosquitto_client_বার্তা *cmsg;
	পূর্ণসংখ্যা rc;

	প্রতিজ্ঞা(db_fptr);
	প্রতিজ্ঞা(context);

	cmsg = queue;
	যখন(cmsg){
		যদি(!strncmp(cmsg->base_বার্তা->তথ্য.topic, "$SYS", 4)
				&& cmsg->base_বার্তা->ref_গুনতি <= 1
				&& cmsg->base_বার্তা->dest_id_গুনতি == 0){

			/* This $SYS message won't have been persisted, so we can't persist
			 * this client message. */
			cmsg = cmsg->পরবর্তী;
			চালু;
		}

		memset(&chunk, 0, আকার(কাঠামো P_client_বার্তা));

		chunk.F.store_id = cmsg->base_বার্তা->তথ্য.store_id;
		chunk.F.mid = cmsg->তথ্য.mid;
		chunk.F.id_len = (uint16_t)strlen(context->id);
		chunk.F.qos = cmsg->তথ্য.qos;
		chunk.F.retain_dup = (uint8_t)((cmsg->তথ্য.retain&0x0F)<<4 | (cmsg->তথ্য.dup&0x0F));
		chunk.F.direction = (uint8_t)cmsg->তথ্য.direction;
		chunk.F.state = (uint8_t)cmsg->তথ্য.state;
		chunk.clientid = context->id;
		chunk.subscription_identifier = cmsg->তথ্য.subscription_identifier;

		rc = persist_chunk_client_বার্তা_write_v6(db_fptr, &chunk);
		যদি(rc){
			ফেরত rc;
		}

		cmsg = cmsg->পরবর্তী;
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা persist__message_store_save(FILE *db_fptr)
{
	কাঠামো P_base_বার্তা chunk;
	কাঠামো mosquitto_base_বার্তা *base_বার্তা, *base_বার্তা_tmp;
	পূর্ণসংখ্যা rc;

	প্রতিজ্ঞা(db_fptr);

	base_বার্তা = db.বার্তা_store;
	HASH_ITER(hh, db.বার্তা_store, base_বার্তা, base_বার্তা_tmp){
		যদি(base_বার্তা->ref_গুনতি < 1 || base_বার্তা->তথ্য.topic == ফাঁকা){
			চালু;
		}

		memset(&chunk, 0, আকার(কাঠামো P_base_বার্তা));

		যদি(!strncmp(base_বার্তা->তথ্য.topic, "$SYS", 4)){
			যদি(base_বার্তা->ref_গুনতি <= 1 && base_বার্তা->dest_id_গুনতি == 0){
				/* $SYS messages that are only retained shouldn't be persisted. */
				চালু;
			}
			/* Don't save $SYS messages as retained otherwise they can give
			 * misleading information when reloaded. They should still be saved
			 * because a disconnected durable client may have them in their
			 * queue. */
			chunk.F.retain = 0;
		}অন্যথা{
			chunk.F.retain = (uint8_t)base_বার্তা->তথ্য.retain;
		}

		chunk.F.store_id = base_বার্তা->তথ্য.store_id;
		chunk.F.expiry_time = base_বার্তা->তথ্য.expiry_time;
		chunk.F.payloadlen = base_বার্তা->তথ্য.payloadlen;
		chunk.F.source_mid = base_বার্তা->তথ্য.source_mid;
		যদি(base_বার্তা->তথ্য.source_id){
			chunk.F.source_id_len = (uint16_t)strlen(base_বার্তা->তথ্য.source_id);
			chunk.source.id = base_বার্তা->তথ্য.source_id;
		}অন্যথা{
			chunk.F.source_id_len = 0;
			chunk.source.id = ফাঁকা;
		}
		যদি(base_বার্তা->তথ্য.source_username){
			chunk.F.source_username_len = (uint16_t)strlen(base_বার্তা->তথ্য.source_username);
			chunk.source.username = base_বার্তা->তথ্য.source_username;
		}অন্যথা{
			chunk.F.source_username_len = 0;
			chunk.source.username = ফাঁকা;
		}

		chunk.F.topic_len = (uint16_t)strlen(base_বার্তা->তথ্য.topic);
		chunk.topic = base_বার্তা->তথ্য.topic;

		যদি(base_বার্তা->source_listener){
			chunk.F.source_port = base_বার্তা->source_listener->port;
		}অন্যথা{
			chunk.F.source_port = 0;
		}
		chunk.F.qos = base_বার্তা->তথ্য.qos;
		chunk.payload = base_বার্তা->তথ্য.payload;
		chunk.properties = base_বার্তা->তথ্য.properties;

		rc = persist__chunk_message_store_write_v6(db_fptr, &chunk);
		যদি(rc){
			ফেরত rc;
		}
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা persist__client_save(FILE *db_fptr)
{
	কাঠামো mosquitto *context, *ctxt_tmp;
	কাঠামো P_client chunk;
	পূর্ণসংখ্যা rc;

	প্রতিজ্ঞা(db_fptr);

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		memset(&chunk, 0, আকার(কাঠামো P_client));

		যদি(context &&
				context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE &&
#ifdef WITH_BRIDGE
				((!context->bridge && context->clean_start == false)
				|| (context->bridge && context->bridge->clean_start_local == false))
#অন্যথা
				context->clean_start == false
#endif
				){
			chunk.F.session_expiry_time = context->session_expiry_time;
			যদি(context->session_expiry_interval != MQTT_SESSION_EXPIRY_NEVER
					&& context->session_expiry_time == 0){

				chunk.F.session_expiry_time = context->session_expiry_interval + db.now_real_s;
			}অন্যথা{
				chunk.F.session_expiry_time = context->session_expiry_time;
			}
			chunk.F.session_expiry_interval = context->session_expiry_interval;
			chunk.F.last_mid = context->last_mid;
			chunk.F.id_len = (uint16_t)strlen(context->id);
			chunk.clientid = context->id;
			যদি(context->username){
				chunk.F.username_len = (uint16_t)strlen(context->username);
				chunk.username = context->username;
			}
			যদি(context->listener){
				chunk.F.listener_port = context->listener->port;
			}

			যদি(chunk.F.id_len == 0){
				/* This should never happen, but in case we have a client with
				 * zero length ID, don't persist them. */
				চালু;
			}

			rc = persist__chunk_client_write_v6(db_fptr, &chunk);
			যদি(rc){
				ফেরত rc;
			}

			যদি(persist__client_messages_save(db_fptr, context, context->msgs_in.inflight)){
				ফেরত 1;
			}
			যদি(persist__client_messages_save(db_fptr, context, context->msgs_in.queued)){
				ফেরত 1;
			}
			যদি(persist__client_messages_save(db_fptr, context, context->msgs_out.inflight)){
				ফেরত 1;
			}
			যদি(persist__client_messages_save(db_fptr, context, context->msgs_out.queued)){
				ফেরত 1;
			}
		}
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা persist__subs_save(FILE *db_fptr, কাঠামো mosquitto__subhier *গ্রন্থি, ধ্রুবক অক্ষর *topic, পূর্ণসংখ্যা level)
{
	কাঠামো mosquitto__subhier *subhier, *subhier_tmp;
	কাঠামো mosquitto__subleaf *sub;
	কাঠামো P_sub sub_chunk;
	অক্ষর *thistopic;
	size_t slen;
	পূর্ণসংখ্যা rc;

	slen = strlen(topic) + গ্রন্থি->topic_len + 2;
	thistopic = mosquitto_malloc(আকার(অক্ষর)*slen);
	যদি(!thistopic){
		ফেরত MOSQ_ERR_NOMEM;
	}
	যদি(level > 1 || strlen(topic)){
		snprintf(thistopic, slen, "%s/%s", topic, গ্রন্থি->topic);
	}অন্যথা{
		snprintf(thistopic, slen, "%s", গ্রন্থি->topic);
	}

	sub = গ্রন্থি->subs;
	যখন(sub){
		যদি(sub->context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE
				&& sub->context->clean_start == false
				&& sub->context->id){

			memset(&sub_chunk, 0, আকার(কাঠামো P_sub));

			sub_chunk.F.identifier = sub->identifier;
			sub_chunk.F.id_len = (uint16_t)strlen(sub->context->id);
			sub_chunk.F.topic_len = (uint16_t)strlen(thistopic);
			sub_chunk.F.qos = MQTT_SUB_OPT_GET_QOS(sub->subscription_options);
			sub_chunk.F.options = sub->subscription_options & 0xFC;
			sub_chunk.clientid = sub->context->id;
			sub_chunk.topic = thistopic;

			rc = persist__chunk_sub_write_v6(db_fptr, &sub_chunk);
			যদি(rc){
				mosquitto_FREE(thistopic);
				ফেরত rc;
			}
		}
		sub = sub->পরবর্তী;
	}

	HASH_ITER(hh, গ্রন্থি->children, subhier, subhier_tmp){
		persist__subs_save(db_fptr, subhier, thistopic, level+1);
	}
	mosquitto_FREE(thistopic);
	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা persist__subs_save_all(FILE *db_fptr)
{
	কাঠামো mosquitto__subhier *subhier, *subhier_tmp;

	HASH_ITER(hh, db.normal_subs, subhier, subhier_tmp){
		যদি(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	HASH_ITER(hh, db.shared_subs, subhier, subhier_tmp){
		যদি(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা persist__retain_save(FILE *db_fptr, কাঠামো mosquitto__retainhier *গ্রন্থি, পূর্ণসংখ্যা level)
{
	কাঠামো mosquitto__retainhier *retainhier, *retainhier_tmp;
	কাঠামো P_retain retain_chunk;
	পূর্ণসংখ্যা rc;

	যদি(গ্রন্থি->retained && strncmp(গ্রন্থি->retained->তথ্য.topic, "$SYS", 4)){
		memset(&retain_chunk, 0, আকার(কাঠামো P_retain));

		/* Don't save $SYS messages. */
		retain_chunk.F.store_id = গ্রন্থি->retained->তথ্য.store_id;
		rc = persist__chunk_retain_write_v6(db_fptr, &retain_chunk);
		যদি(rc){
			ফেরত rc;
		}
	}

	HASH_ITER(hh, গ্রন্থি->children, retainhier, retainhier_tmp){
		persist__retain_save(db_fptr, retainhier, level+1);
	}
	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা persist__retain_save_all(FILE *db_fptr)
{
	কাঠামো mosquitto__retainhier *retainhier, *retainhier_tmp;

	HASH_ITER(hh, db.retains, retainhier, retainhier_tmp){
		যদি(retainhier->children){
			persist__retain_save(db_fptr, retainhier->children, 0);
		}
	}

	ফেরত MOSQ_ERR_SUCCESS;
}

স্থির পূর্ণসংখ্যা persist_write_তথ্য(FILE *db_fptr, শূন্য *user_তথ্য);


স্থির শূন্য persist__log_write_error(ধ্রুবক অক্ষর *বার্তা)
{
	log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error saving in-memory database, %s", বার্তা);
}


পূর্ণসংখ্যা persist__backup(bool shutdown)
{
	যদি(db.config == ফাঁকা){
		ফেরত MOSQ_ERR_INVAL;
	}
	যদি(db.config->persistence == false){
		ফেরত MOSQ_ERR_SUCCESS;
	}
	যদি(db.config->persistence_filepath == ফাঁকা){
		ফেরত MOSQ_ERR_INVAL;
	}

	log__printf(ফাঁকা, MOSQ_LOG_INFO, "Saving in-memory database to %s.", db.config->persistence_filepath);

	ফেরত mosquitto_write_file(db.config->persistence_filepath, true, &persist_write_তথ্য, &shutdown, &persist__log_write_error);
}


স্থির পূর্ণসংখ্যা persist_write_তথ্য(FILE *db_fptr, শূন্য *user_তথ্য)
{
	bool shutdown = *(bool *)(user_তথ্য);
	uint32_t db_version_w = htonl(MOSQ_DB_VERSION);
	uint32_t crc = 0;
	ধ্রুবক অক্ষর *err;
	কাঠামো PF_cfg cfg_chunk;
	পূর্ণসংখ্যা rc = MOSQ_ERR_UNKNOWN;

	/* Header */
	write_e(db_fptr, magic, 15);
	write_e(db_fptr, &crc, আকার(uint32_t));
	write_e(db_fptr, &db_version_w, আকার(uint32_t));

	memset(&cfg_chunk, 0, আকার(কাঠামো PF_cfg));
	cfg_chunk.last_db_id = db.last_db_id;
	cfg_chunk.shutdown = shutdown;
	cfg_chunk.dbid_আকার = আকার(dbid_t);
	যদি(persist__chunk_cfg_write_v6(db_fptr, &cfg_chunk)){
		যাও error;
	}

	যদি(persist__message_store_save(db_fptr)){
		যাও error;
	}

	যদি(persist__client_save(db_fptr)
			|| persist__subs_save_all(db_fptr)
			|| persist__retain_save_all(db_fptr)){
		যাও error;
	}
	ফেরত MOSQ_ERR_SUCCESS;

error:
	err = strerror(errno);
	log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error during saving in-memory database %s: %s.", db.config->persistence_filepath, err);
	যদি(db_fptr){
		fclose(db_fptr);
	}
	ফেরত rc;
}


#endif
