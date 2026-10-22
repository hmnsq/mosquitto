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

#અન્તર્ભાવ "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#અન્તર્ભાવ <arpa/inet.h>
#endif
#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <errno.h>
#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <sys/stat.h>
#અન્તર્ભાવ <time.h>

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "persist.h"
#અન્તર્ભાવ "util_mosq.h"


સ્થિર પૂર્ણાંક persist__client_messages_save(FILE *db_fptr, સંરચના mosquitto *context, સંરચના mosquitto_client_સંદેશ *queue)
{
	સંરચના P_client_સંદેશ chunk;
	સંરચના mosquitto_client_સંદેશ *cmsg;
	પૂર્ણાંક rc;

	પ્રતિજ્ઞા(db_fptr);
	પ્રતિજ્ઞા(context);

	cmsg = queue;
	જ્યારે(cmsg){
		જો(!strncmp(cmsg->base_સંદેશ->data.topic, "$SYS", 4)
				&& cmsg->base_સંદેશ->ref_ગણતરી <= 1
				&& cmsg->base_સંદેશ->dest_id_ગણતરી == 0){

			/* This $SYS message won't have been persisted, so we can't persist
			 * this client message. */
			cmsg = cmsg->next;
			ચાલુ;
		}

		memset(&chunk, 0, કદ(સંરચના P_client_સંદેશ));

		chunk.F.store_id = cmsg->base_સંદેશ->data.store_id;
		chunk.F.mid = cmsg->data.mid;
		chunk.F.id_len = (uint16_t)strlen(context->id);
		chunk.F.qos = cmsg->data.qos;
		chunk.F.retain_dup = (uint8_t)((cmsg->data.retain&0x0F)<<4 | (cmsg->data.dup&0x0F));
		chunk.F.direction = (uint8_t)cmsg->data.direction;
		chunk.F.state = (uint8_t)cmsg->data.state;
		chunk.clientid = context->id;
		chunk.subscription_identifier = cmsg->data.subscription_identifier;

		rc = persist_chunk_client_સંદેશ_write_v6(db_fptr, &chunk);
		જો(rc){
			પાછા rc;
		}

		cmsg = cmsg->next;
	}

	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક persist__message_store_save(FILE *db_fptr)
{
	સંરચના P_base_સંદેશ chunk;
	સંરચના mosquitto_base_સંદેશ *base_સંદેશ, *base_સંદેશ_tmp;
	પૂર્ણાંક rc;

	પ્રતિજ્ઞા(db_fptr);

	base_સંદેશ = db.સંદેશ_store;
	HASH_ITER(hh, db.સંદેશ_store, base_સંદેશ, base_સંદેશ_tmp){
		જો(base_સંદેશ->ref_ગણતરી < 1 || base_સંદેશ->data.topic == ખાલી){
			ચાલુ;
		}

		memset(&chunk, 0, કદ(સંરચના P_base_સંદેશ));

		જો(!strncmp(base_સંદેશ->data.topic, "$SYS", 4)){
			જો(base_સંદેશ->ref_ગણતરી <= 1 && base_સંદેશ->dest_id_ગણતરી == 0){
				/* $SYS messages that are only retained shouldn't be persisted. */
				ચાલુ;
			}
			/* Don't save $SYS messages as retained otherwise they can give
			 * misleading information when reloaded. They should still be saved
			 * because a disconnected durable client may have them in their
			 * queue. */
			chunk.F.retain = 0;
		}નહીંતર{
			chunk.F.retain = (uint8_t)base_સંદેશ->data.retain;
		}

		chunk.F.store_id = base_સંદેશ->data.store_id;
		chunk.F.expiry_time = base_સંદેશ->data.expiry_time;
		chunk.F.payloadlen = base_સંદેશ->data.payloadlen;
		chunk.F.source_mid = base_સંદેશ->data.source_mid;
		જો(base_સંદેશ->data.source_id){
			chunk.F.source_id_len = (uint16_t)strlen(base_સંદેશ->data.source_id);
			chunk.source.id = base_સંદેશ->data.source_id;
		}નહીંતર{
			chunk.F.source_id_len = 0;
			chunk.source.id = ખાલી;
		}
		જો(base_સંદેશ->data.source_username){
			chunk.F.source_username_len = (uint16_t)strlen(base_સંદેશ->data.source_username);
			chunk.source.username = base_સંદેશ->data.source_username;
		}નહીંતર{
			chunk.F.source_username_len = 0;
			chunk.source.username = ખાલી;
		}

		chunk.F.topic_len = (uint16_t)strlen(base_સંદેશ->data.topic);
		chunk.topic = base_સંદેશ->data.topic;

		જો(base_સંદેશ->source_listener){
			chunk.F.source_port = base_સંદેશ->source_listener->port;
		}નહીંતર{
			chunk.F.source_port = 0;
		}
		chunk.F.qos = base_સંદેશ->data.qos;
		chunk.payload = base_સંદેશ->data.payload;
		chunk.properties = base_સંદેશ->data.properties;

		rc = persist__chunk_message_store_write_v6(db_fptr, &chunk);
		જો(rc){
			પાછા rc;
		}
	}

	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક persist__client_save(FILE *db_fptr)
{
	સંરચના mosquitto *context, *ctxt_tmp;
	સંરચના P_client chunk;
	પૂર્ણાંક rc;

	પ્રતિજ્ઞા(db_fptr);

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		memset(&chunk, 0, કદ(સંરચના P_client));

		જો(context &&
				context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE &&
#ifdef WITH_BRIDGE
				((!context->bridge && context->clean_start == false)
				|| (context->bridge && context->bridge->clean_start_local == false))
#નહીંતર
				context->clean_start == false
#endif
				){
			chunk.F.session_expiry_time = context->session_expiry_time;
			જો(context->session_expiry_interval != MQTT_SESSION_EXPIRY_NEVER
					&& context->session_expiry_time == 0){

				chunk.F.session_expiry_time = context->session_expiry_interval + db.now_real_s;
			}નહીંતર{
				chunk.F.session_expiry_time = context->session_expiry_time;
			}
			chunk.F.session_expiry_interval = context->session_expiry_interval;
			chunk.F.last_mid = context->last_mid;
			chunk.F.id_len = (uint16_t)strlen(context->id);
			chunk.clientid = context->id;
			જો(context->username){
				chunk.F.username_len = (uint16_t)strlen(context->username);
				chunk.username = context->username;
			}
			જો(context->listener){
				chunk.F.listener_port = context->listener->port;
			}

			જો(chunk.F.id_len == 0){
				/* This should never happen, but in case we have a client with
				 * zero length ID, don't persist them. */
				ચાલુ;
			}

			rc = persist__chunk_client_write_v6(db_fptr, &chunk);
			જો(rc){
				પાછા rc;
			}

			જો(persist__client_messages_save(db_fptr, context, context->msgs_in.inflight)){
				પાછા 1;
			}
			જો(persist__client_messages_save(db_fptr, context, context->msgs_in.queued)){
				પાછા 1;
			}
			જો(persist__client_messages_save(db_fptr, context, context->msgs_out.inflight)){
				પાછા 1;
			}
			જો(persist__client_messages_save(db_fptr, context, context->msgs_out.queued)){
				પાછા 1;
			}
		}
	}

	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક persist__subs_save(FILE *db_fptr, સંરચના mosquitto__subhier *ગાંઠ, અચલ અક્ષર *topic, પૂર્ણાંક level)
{
	સંરચના mosquitto__subhier *subhier, *subhier_tmp;
	સંરચના mosquitto__subleaf *sub;
	સંરચના P_sub sub_chunk;
	અક્ષર *thistopic;
	size_t slen;
	પૂર્ણાંક rc;

	slen = strlen(topic) + ગાંઠ->topic_len + 2;
	thistopic = mosquitto_malloc(કદ(અક્ષર)*slen);
	જો(!thistopic){
		પાછા MOSQ_ERR_NOMEM;
	}
	જો(level > 1 || strlen(topic)){
		snprintf(thistopic, slen, "%s/%s", topic, ગાંઠ->topic);
	}નહીંતર{
		snprintf(thistopic, slen, "%s", ગાંઠ->topic);
	}

	sub = ગાંઠ->subs;
	જ્યારે(sub){
		જો(sub->context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE
				&& sub->context->clean_start == false
				&& sub->context->id){

			memset(&sub_chunk, 0, કદ(સંરચના P_sub));

			sub_chunk.F.identifier = sub->identifier;
			sub_chunk.F.id_len = (uint16_t)strlen(sub->context->id);
			sub_chunk.F.topic_len = (uint16_t)strlen(thistopic);
			sub_chunk.F.qos = MQTT_SUB_OPT_GET_QOS(sub->subscription_options);
			sub_chunk.F.options = sub->subscription_options & 0xFC;
			sub_chunk.clientid = sub->context->id;
			sub_chunk.topic = thistopic;

			rc = persist__chunk_sub_write_v6(db_fptr, &sub_chunk);
			જો(rc){
				mosquitto_FREE(thistopic);
				પાછા rc;
			}
		}
		sub = sub->next;
	}

	HASH_ITER(hh, ગાંઠ->children, subhier, subhier_tmp){
		persist__subs_save(db_fptr, subhier, thistopic, level+1);
	}
	mosquitto_FREE(thistopic);
	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક persist__subs_save_all(FILE *db_fptr)
{
	સંરચના mosquitto__subhier *subhier, *subhier_tmp;

	HASH_ITER(hh, db.normal_subs, subhier, subhier_tmp){
		જો(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	HASH_ITER(hh, db.shared_subs, subhier, subhier_tmp){
		જો(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક persist__retain_save(FILE *db_fptr, સંરચના mosquitto__retainhier *ગાંઠ, પૂર્ણાંક level)
{
	સંરચના mosquitto__retainhier *retainhier, *retainhier_tmp;
	સંરચના P_retain retain_chunk;
	પૂર્ણાંક rc;

	જો(ગાંઠ->retained && strncmp(ગાંઠ->retained->data.topic, "$SYS", 4)){
		memset(&retain_chunk, 0, કદ(સંરચના P_retain));

		/* Don't save $SYS messages. */
		retain_chunk.F.store_id = ગાંઠ->retained->data.store_id;
		rc = persist__chunk_retain_write_v6(db_fptr, &retain_chunk);
		જો(rc){
			પાછા rc;
		}
	}

	HASH_ITER(hh, ગાંઠ->children, retainhier, retainhier_tmp){
		persist__retain_save(db_fptr, retainhier, level+1);
	}
	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક persist__retain_save_all(FILE *db_fptr)
{
	સંરચના mosquitto__retainhier *retainhier, *retainhier_tmp;

	HASH_ITER(hh, db.retains, retainhier, retainhier_tmp){
		જો(retainhier->children){
			persist__retain_save(db_fptr, retainhier->children, 0);
		}
	}

	પાછા MOSQ_ERR_SUCCESS;
}

સ્થિર પૂર્ણાંક persist__write_data(FILE *db_fptr, શૂન્ય *user_data);


સ્થિર શૂન્ય persist__log_write_error(અચલ અક્ષર *સંદેશ)
{
	log__printf(ખાલી, MOSQ_LOG_ERR, "Error saving in-memory database, %s", સંદેશ);
}


પૂર્ણાંક persist__backup(bool shutdown)
{
	જો(db.config == ખાલી){
		પાછા MOSQ_ERR_INVAL;
	}
	જો(db.config->persistence == false){
		પાછા MOSQ_ERR_SUCCESS;
	}
	જો(db.config->persistence_filepath == ખાલી){
		પાછા MOSQ_ERR_INVAL;
	}

	log__printf(ખાલી, MOSQ_LOG_INFO, "Saving in-memory database to %s.", db.config->persistence_filepath);

	પાછા mosquitto_write_file(db.config->persistence_filepath, true, &persist__write_data, &shutdown, &persist__log_write_error);
}


સ્થિર પૂર્ણાંક persist__write_data(FILE *db_fptr, શૂન્ય *user_data)
{
	bool shutdown = *(bool *)(user_data);
	uint32_t db_version_w = htonl(MOSQ_DB_VERSION);
	uint32_t crc = 0;
	અચલ અક્ષર *err;
	સંરચના PF_cfg cfg_chunk;
	પૂર્ણાંક rc = MOSQ_ERR_UNKNOWN;

	/* Header */
	write_e(db_fptr, magic, 15);
	write_e(db_fptr, &crc, કદ(uint32_t));
	write_e(db_fptr, &db_version_w, કદ(uint32_t));

	memset(&cfg_chunk, 0, કદ(સંરચના PF_cfg));
	cfg_chunk.last_db_id = db.last_db_id;
	cfg_chunk.shutdown = shutdown;
	cfg_chunk.dbid_size = કદ(dbid_t);
	જો(persist__chunk_cfg_write_v6(db_fptr, &cfg_chunk)){
		જાઓ error;
	}

	જો(persist__message_store_save(db_fptr)){
		જાઓ error;
	}

	જો(persist__client_save(db_fptr)
			|| persist__subs_save_all(db_fptr)
			|| persist__retain_save_all(db_fptr)){
		જાઓ error;
	}
	પાછા MOSQ_ERR_SUCCESS;

error:
	err = strerror(errno);
	log__printf(ખાલી, MOSQ_LOG_ERR, "Error during saving in-memory database %s: %s.", db.config->persistence_filepath, err);
	જો(db_fptr){
		fclose(db_fptr);
	}
	પાછા rc;
}


#endif
