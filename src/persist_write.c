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

#അന്തര്ഭാവ "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#അന്തര്ഭാവ <arpa/inet.h>
#endif
#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <errno.h>
#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <sys/stat.h>
#അന്തര്ഭാവ <time.h>

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "persist.h"
#അന്തര്ഭാവ "util_mosq.h"


സ്ഥിതം പൂർണ്ണസംഖ്യ persist__client_messages_save(FILE *db_fptr, ഘടന mosquitto *context, ഘടന mosquitto_client_സന്ദേശം *queue)
{
	ഘടന P_client_സന്ദേശം chunk;
	ഘടന mosquitto_client_സന്ദേശം *cmsg;
	പൂർണ്ണസംഖ്യ rc;

	പ്രതിജ്ഞാ(db_fptr);
	പ്രതിജ്ഞാ(context);

	cmsg = queue;
	ആകുമ്പോൾ(cmsg){
		എങ്കിൽ(!strncmp(cmsg->base_സന്ദേശം->data.topic, "$SYS", 4)
				&& cmsg->base_സന്ദേശം->ref_എണ്ണം <= 1
				&& cmsg->base_സന്ദേശം->dest_id_എണ്ണം == 0){

			/* This $SYS message won't have been persisted, so we can't persist
			 * this client message. */
			cmsg = cmsg->next;
			തുടരുക;
		}

		memset(&chunk, 0, വലുപ്പം(ഘടന P_client_സന്ദേശം));

		chunk.F.store_id = cmsg->base_സന്ദേശം->data.store_id;
		chunk.F.mid = cmsg->data.mid;
		chunk.F.id_len = (uint16_t)strlen(context->id);
		chunk.F.qos = cmsg->data.qos;
		chunk.F.retain_dup = (uint8_t)((cmsg->data.retain&0x0F)<<4 | (cmsg->data.dup&0x0F));
		chunk.F.direction = (uint8_t)cmsg->data.direction;
		chunk.F.state = (uint8_t)cmsg->data.state;
		chunk.clientid = context->id;
		chunk.subscription_identifier = cmsg->data.subscription_identifier;

		rc = persist_chunk_client_സന്ദേശം_write_v6(db_fptr, &chunk);
		എങ്കിൽ(rc){
			തിരികെ rc;
		}

		cmsg = cmsg->next;
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ persist__message_store_save(FILE *db_fptr)
{
	ഘടന P_base_സന്ദേശം chunk;
	ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം, *base_സന്ദേശം_tmp;
	പൂർണ്ണസംഖ്യ rc;

	പ്രതിജ്ഞാ(db_fptr);

	base_സന്ദേശം = db.സന്ദേശം_store;
	HASH_ITER(hh, db.സന്ദേശം_store, base_സന്ദേശം, base_സന്ദേശം_tmp){
		എങ്കിൽ(base_സന്ദേശം->ref_എണ്ണം < 1 || base_സന്ദേശം->data.topic == ശൂന്യ){
			തുടരുക;
		}

		memset(&chunk, 0, വലുപ്പം(ഘടന P_base_സന്ദേശം));

		എങ്കിൽ(!strncmp(base_സന്ദേശം->data.topic, "$SYS", 4)){
			എങ്കിൽ(base_സന്ദേശം->ref_എണ്ണം <= 1 && base_സന്ദേശം->dest_id_എണ്ണം == 0){
				/* $SYS messages that are only retained shouldn't be persisted. */
				തുടരുക;
			}
			/* Don't save $SYS messages as retained otherwise they can give
			 * misleading information when reloaded. They should still be saved
			 * because a disconnected durable client may have them in their
			 * queue. */
			chunk.F.retain = 0;
		}അല്ലെങ്കിൽ{
			chunk.F.retain = (uint8_t)base_സന്ദേശം->data.retain;
		}

		chunk.F.store_id = base_സന്ദേശം->data.store_id;
		chunk.F.expiry_time = base_സന്ദേശം->data.expiry_time;
		chunk.F.payloadlen = base_സന്ദേശം->data.payloadlen;
		chunk.F.source_mid = base_സന്ദേശം->data.source_mid;
		എങ്കിൽ(base_സന്ദേശം->data.source_id){
			chunk.F.source_id_len = (uint16_t)strlen(base_സന്ദേശം->data.source_id);
			chunk.source.id = base_സന്ദേശം->data.source_id;
		}അല്ലെങ്കിൽ{
			chunk.F.source_id_len = 0;
			chunk.source.id = ശൂന്യ;
		}
		എങ്കിൽ(base_സന്ദേശം->data.source_username){
			chunk.F.source_username_len = (uint16_t)strlen(base_സന്ദേശം->data.source_username);
			chunk.source.username = base_സന്ദേശം->data.source_username;
		}അല്ലെങ്കിൽ{
			chunk.F.source_username_len = 0;
			chunk.source.username = ശൂന്യ;
		}

		chunk.F.topic_len = (uint16_t)strlen(base_സന്ദേശം->data.topic);
		chunk.topic = base_സന്ദേശം->data.topic;

		എങ്കിൽ(base_സന്ദേശം->source_listener){
			chunk.F.source_port = base_സന്ദേശം->source_listener->port;
		}അല്ലെങ്കിൽ{
			chunk.F.source_port = 0;
		}
		chunk.F.qos = base_സന്ദേശം->data.qos;
		chunk.payload = base_സന്ദേശം->data.payload;
		chunk.properties = base_സന്ദേശം->data.properties;

		rc = persist__chunk_message_store_write_v6(db_fptr, &chunk);
		എങ്കിൽ(rc){
			തിരികെ rc;
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ persist__client_save(FILE *db_fptr)
{
	ഘടന mosquitto *context, *ctxt_tmp;
	ഘടന P_client chunk;
	പൂർണ്ണസംഖ്യ rc;

	പ്രതിജ്ഞാ(db_fptr);

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		memset(&chunk, 0, വലുപ്പം(ഘടന P_client));

		എങ്കിൽ(context &&
				context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE &&
#ifdef WITH_BRIDGE
				((!context->bridge && context->clean_start == false)
				|| (context->bridge && context->bridge->clean_start_local == false))
#അല്ലെങ്കിൽ
				context->clean_start == false
#endif
				){
			chunk.F.session_expiry_time = context->session_expiry_time;
			എങ്കിൽ(context->session_expiry_interval != MQTT_SESSION_EXPIRY_NEVER
					&& context->session_expiry_time == 0){

				chunk.F.session_expiry_time = context->session_expiry_interval + db.now_real_s;
			}അല്ലെങ്കിൽ{
				chunk.F.session_expiry_time = context->session_expiry_time;
			}
			chunk.F.session_expiry_interval = context->session_expiry_interval;
			chunk.F.last_mid = context->last_mid;
			chunk.F.id_len = (uint16_t)strlen(context->id);
			chunk.clientid = context->id;
			എങ്കിൽ(context->username){
				chunk.F.username_len = (uint16_t)strlen(context->username);
				chunk.username = context->username;
			}
			എങ്കിൽ(context->listener){
				chunk.F.listener_port = context->listener->port;
			}

			എങ്കിൽ(chunk.F.id_len == 0){
				/* This should never happen, but in case we have a client with
				 * zero length ID, don't persist them. */
				തുടരുക;
			}

			rc = persist__chunk_client_write_v6(db_fptr, &chunk);
			എങ്കിൽ(rc){
				തിരികെ rc;
			}

			എങ്കിൽ(persist__client_messages_save(db_fptr, context, context->msgs_in.inflight)){
				തിരികെ 1;
			}
			എങ്കിൽ(persist__client_messages_save(db_fptr, context, context->msgs_in.queued)){
				തിരികെ 1;
			}
			എങ്കിൽ(persist__client_messages_save(db_fptr, context, context->msgs_out.inflight)){
				തിരികെ 1;
			}
			എങ്കിൽ(persist__client_messages_save(db_fptr, context, context->msgs_out.queued)){
				തിരികെ 1;
			}
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ persist__subs_save(FILE *db_fptr, ഘടന mosquitto__subhier *കണ്ണി, സ്ഥിരം അക്ഷരം *topic, പൂർണ്ണസംഖ്യ level)
{
	ഘടന mosquitto__subhier *subhier, *subhier_tmp;
	ഘടന mosquitto__subleaf *sub;
	ഘടന P_sub sub_chunk;
	അക്ഷരം *thistopic;
	size_t slen;
	പൂർണ്ണസംഖ്യ rc;

	slen = strlen(topic) + കണ്ണി->topic_len + 2;
	thistopic = mosquitto_malloc(വലുപ്പം(അക്ഷരം)*slen);
	എങ്കിൽ(!thistopic){
		തിരികെ MOSQ_ERR_NOMEM;
	}
	എങ്കിൽ(level > 1 || strlen(topic)){
		snprintf(thistopic, slen, "%s/%s", topic, കണ്ണി->topic);
	}അല്ലെങ്കിൽ{
		snprintf(thistopic, slen, "%s", കണ്ണി->topic);
	}

	sub = കണ്ണി->subs;
	ആകുമ്പോൾ(sub){
		എങ്കിൽ(sub->context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE
				&& sub->context->clean_start == false
				&& sub->context->id){

			memset(&sub_chunk, 0, വലുപ്പം(ഘടന P_sub));

			sub_chunk.F.identifier = sub->identifier;
			sub_chunk.F.id_len = (uint16_t)strlen(sub->context->id);
			sub_chunk.F.topic_len = (uint16_t)strlen(thistopic);
			sub_chunk.F.qos = MQTT_SUB_OPT_GET_QOS(sub->subscription_options);
			sub_chunk.F.options = sub->subscription_options & 0xFC;
			sub_chunk.clientid = sub->context->id;
			sub_chunk.topic = thistopic;

			rc = persist__chunk_sub_write_v6(db_fptr, &sub_chunk);
			എങ്കിൽ(rc){
				mosquitto_FREE(thistopic);
				തിരികെ rc;
			}
		}
		sub = sub->next;
	}

	HASH_ITER(hh, കണ്ണി->children, subhier, subhier_tmp){
		persist__subs_save(db_fptr, subhier, thistopic, level+1);
	}
	mosquitto_FREE(thistopic);
	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ persist__subs_save_all(FILE *db_fptr)
{
	ഘടന mosquitto__subhier *subhier, *subhier_tmp;

	HASH_ITER(hh, db.normal_subs, subhier, subhier_tmp){
		എങ്കിൽ(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	HASH_ITER(hh, db.shared_subs, subhier, subhier_tmp){
		എങ്കിൽ(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ persist__retain_save(FILE *db_fptr, ഘടന mosquitto__retainhier *കണ്ണി, പൂർണ്ണസംഖ്യ level)
{
	ഘടന mosquitto__retainhier *retainhier, *retainhier_tmp;
	ഘടന P_retain retain_chunk;
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ(കണ്ണി->retained && strncmp(കണ്ണി->retained->data.topic, "$SYS", 4)){
		memset(&retain_chunk, 0, വലുപ്പം(ഘടന P_retain));

		/* Don't save $SYS messages. */
		retain_chunk.F.store_id = കണ്ണി->retained->data.store_id;
		rc = persist__chunk_retain_write_v6(db_fptr, &retain_chunk);
		എങ്കിൽ(rc){
			തിരികെ rc;
		}
	}

	HASH_ITER(hh, കണ്ണി->children, retainhier, retainhier_tmp){
		persist__retain_save(db_fptr, retainhier, level+1);
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ persist__retain_save_all(FILE *db_fptr)
{
	ഘടന mosquitto__retainhier *retainhier, *retainhier_tmp;

	HASH_ITER(hh, db.retains, retainhier, retainhier_tmp){
		എങ്കിൽ(retainhier->children){
			persist__retain_save(db_fptr, retainhier->children, 0);
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ persist__write_data(FILE *db_fptr, ശൂന്യം *user_data);


സ്ഥിതം ശൂന്യം persist__log_write_error(സ്ഥിരം അക്ഷരം *സന്ദേശം)
{
	log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error saving in-memory database, %s", സന്ദേശം);
}


പൂർണ്ണസംഖ്യ persist__backup(bool shutdown)
{
	എങ്കിൽ(db.config == ശൂന്യ){
		തിരികെ MOSQ_ERR_INVAL;
	}
	എങ്കിൽ(db.config->persistence == false){
		തിരികെ MOSQ_ERR_SUCCESS;
	}
	എങ്കിൽ(db.config->persistence_filepath == ശൂന്യ){
		തിരികെ MOSQ_ERR_INVAL;
	}

	log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Saving in-memory database to %s.", db.config->persistence_filepath);

	തിരികെ mosquitto_write_file(db.config->persistence_filepath, true, &persist__write_data, &shutdown, &persist__log_write_error);
}


സ്ഥിതം പൂർണ്ണസംഖ്യ persist__write_data(FILE *db_fptr, ശൂന്യം *user_data)
{
	bool shutdown = *(bool *)(user_data);
	uint32_t db_version_w = htonl(MOSQ_DB_VERSION);
	uint32_t crc = 0;
	സ്ഥിരം അക്ഷരം *err;
	ഘടന PF_cfg cfg_chunk;
	പൂർണ്ണസംഖ്യ rc = MOSQ_ERR_UNKNOWN;

	/* Header */
	write_e(db_fptr, magic, 15);
	write_e(db_fptr, &crc, വലുപ്പം(uint32_t));
	write_e(db_fptr, &db_version_w, വലുപ്പം(uint32_t));

	memset(&cfg_chunk, 0, വലുപ്പം(ഘടന PF_cfg));
	cfg_chunk.last_db_id = db.last_db_id;
	cfg_chunk.shutdown = shutdown;
	cfg_chunk.dbid_size = വലുപ്പം(dbid_t);
	എങ്കിൽ(persist__chunk_cfg_write_v6(db_fptr, &cfg_chunk)){
		പോകുക error;
	}

	എങ്കിൽ(persist__message_store_save(db_fptr)){
		പോകുക error;
	}

	എങ്കിൽ(persist__client_save(db_fptr)
			|| persist__subs_save_all(db_fptr)
			|| persist__retain_save_all(db_fptr)){
		പോകുക error;
	}
	തിരികെ MOSQ_ERR_SUCCESS;

error:
	err = strerror(errno);
	log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error during saving in-memory database %s: %s.", db.config->persistence_filepath, err);
	എങ്കിൽ(db_fptr){
		fclose(db_fptr);
	}
	തിരികെ rc;
}


#endif
