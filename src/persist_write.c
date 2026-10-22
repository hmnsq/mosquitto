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

#అన్తర్భావ "config.h"

#నిర్వచితమైతే WITH_PERSISTENCE

#నిర్వచితం_కాకపోతే WIN32
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు
#అన్తర్భావ <assert.h>
#అన్తర్భావ <errno.h>
#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <sys/stat.h>
#అన్తర్భావ <time.h>

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "persist.h"
#అన్తర్భావ "util_mosq.h"


స్థిర పూర్ణాంకం persist__client_messages_save(FILE *db_fptr, నిర్మాణం mosquitto *context, నిర్మాణం mosquitto_client_సందేశం *వరుస)
{
	నిర్మాణం P_client_సందేశం chunk;
	నిర్మాణం mosquitto_client_సందేశం *cmsg;
	పూర్ణాంకం rc;

	ప్రతిజ్ఞా(db_fptr);
	ప్రతిజ్ఞా(context);

	cmsg = వరుస;
	అయితే(cmsg){
		ఒకవేళ(!అక్షరశ్రేణి_పోలిక(cmsg->పునాది_సందేశం->దత్తాంశం.topic, "$SYS", 4)
				&& cmsg->పునాది_సందేశం->ref_లెక్క <= 1
				&& cmsg->పునాది_సందేశం->dest_id_లెక్క == 0){

			/* This $SYS message won't have been persisted, so we can't persist
			 * this client message. */
			cmsg = cmsg->తదుపరి;
			కొనసాగు;
		}

		memset(&chunk, 0, పరిమాణం(నిర్మాణం P_client_సందేశం));

		chunk.F.store_id = cmsg->పునాది_సందేశం->దత్తాంశం.store_id;
		chunk.F.mid = cmsg->దత్తాంశం.mid;
		chunk.F.id_len = (uint16_t)అక్షరశ్రేణి_పొడవు(context->id);
		chunk.F.qos = cmsg->దత్తాంశం.qos;
		chunk.F.retain_dup = (uint8_t)((cmsg->దత్తాంశం.retain&0x0F)<<4 | (cmsg->దత్తాంశం.dup&0x0F));
		chunk.F.direction = (uint8_t)cmsg->దత్తాంశం.direction;
		chunk.F.state = (uint8_t)cmsg->దత్తాంశం.state;
		chunk.clientid = context->id;
		chunk.subscription_identifier = cmsg->దత్తాంశం.subscription_identifier;

		rc = persist_chunk_client_సందేశం_write_v6(db_fptr, &chunk);
		ఒకవేళ(rc){
			తిరిగి rc;
		}

		cmsg = cmsg->తదుపరి;
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం persist_సందేశం_store_save(FILE *db_fptr)
{
	నిర్మాణం P_పునాది_సందేశం chunk;
	నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం, *పునాది_సందేశం_tmp;
	పూర్ణాంకం rc;

	ప్రతిజ్ఞా(db_fptr);

	పునాది_సందేశం = db.సందేశం_store;
	HASH_ITER(hh, db.సందేశం_store, పునాది_సందేశం, పునాది_సందేశం_tmp){
		ఒకవేళ(పునాది_సందేశం->ref_లెక్క < 1 || పునాది_సందేశం->దత్తాంశం.topic == ఖాళీ){
			కొనసాగు;
		}

		memset(&chunk, 0, పరిమాణం(నిర్మాణం P_పునాది_సందేశం));

		ఒకవేళ(!అక్షరశ్రేణి_పోలిక(పునాది_సందేశం->దత్తాంశం.topic, "$SYS", 4)){
			ఒకవేళ(పునాది_సందేశం->ref_లెక్క <= 1 && పునాది_సందేశం->dest_id_లెక్క == 0){
				/* $SYS messages that are only retained shouldn't be persisted. */
				కొనసాగు;
			}
			/* Don't save $SYS messages as retained otherwise they can give
			 * misleading information when reloaded. They should still be saved
			 * because a disconnected durable client may have them in their
			 * queue. */
			chunk.F.retain = 0;
		}లేకపోతే{
			chunk.F.retain = (uint8_t)పునాది_సందేశం->దత్తాంశం.retain;
		}

		chunk.F.store_id = పునాది_సందేశం->దత్తాంశం.store_id;
		chunk.F.expiry_సమయం = పునాది_సందేశం->దత్తాంశం.expiry_సమయం;
		chunk.F.payloadlen = పునాది_సందేశం->దత్తాంశం.payloadlen;
		chunk.F.ఆధారం_mid = పునాది_సందేశం->దత్తాంశం.ఆధారం_mid;
		ఒకవేళ(పునాది_సందేశం->దత్తాంశం.ఆధారం_id){
			chunk.F.ఆధారం_id_len = (uint16_t)అక్షరశ్రేణి_పొడవు(పునాది_సందేశం->దత్తాంశం.ఆధారం_id);
			chunk.ఆధారం.id = పునాది_సందేశం->దత్తాంశం.ఆధారం_id;
		}లేకపోతే{
			chunk.F.ఆధారం_id_len = 0;
			chunk.ఆధారం.id = ఖాళీ;
		}
		ఒకవేళ(పునాది_సందేశం->దత్తాంశం.ఆధారం_username){
			chunk.F.ఆధారం_username_len = (uint16_t)అక్షరశ్రేణి_పొడవు(పునాది_సందేశం->దత్తాంశం.ఆధారం_username);
			chunk.ఆధారం.username = పునాది_సందేశం->దత్తాంశం.ఆధారం_username;
		}లేకపోతే{
			chunk.F.ఆధారం_username_len = 0;
			chunk.ఆధారం.username = ఖాళీ;
		}

		chunk.F.topic_len = (uint16_t)అక్షరశ్రేణి_పొడవు(పునాది_సందేశం->దత్తాంశం.topic);
		chunk.topic = పునాది_సందేశం->దత్తాంశం.topic;

		ఒకవేళ(పునాది_సందేశం->ఆధారం_listener){
			chunk.F.ఆధారం_పోర్టు = పునాది_సందేశం->ఆధారం_listener->పోర్టు;
		}లేకపోతే{
			chunk.F.ఆధారం_పోర్టు = 0;
		}
		chunk.F.qos = పునాది_సందేశం->దత్తాంశం.qos;
		chunk.payload = పునాది_సందేశం->దత్తాంశం.payload;
		chunk.properties = పునాది_సందేశం->దత్తాంశం.properties;

		rc = persist_chunk_సందేశం_store_write_v6(db_fptr, &chunk);
		ఒకవేళ(rc){
			తిరిగి rc;
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం persist__client_save(FILE *db_fptr)
{
	నిర్మాణం mosquitto *context, *ctxt_tmp;
	నిర్మాణం P_client chunk;
	పూర్ణాంకం rc;

	ప్రతిజ్ఞా(db_fptr);

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		memset(&chunk, 0, పరిమాణం(నిర్మాణం P_client));

		ఒకవేళ(context &&
				context->session_expiry_interval != MQTT_SESSION_EXPIRY_తక్షణ &&
#నిర్వచితమైతే WITH_BRIDGE
				((!context->bridge && context->clean_ప్రారంభం == false)
				|| (context->bridge && context->bridge->clean_ప్రారంభం_local == false))
#లేకపోతే
				context->clean_ప్రారంభం == false
#అయితే_ముగింపు
				){
			chunk.F.session_expiry_సమయం = context->session_expiry_సమయం;
			ఒకవేళ(context->session_expiry_interval != MQTT_SESSION_EXPIRY_NEVER
					&& context->session_expiry_సమయం == 0){

				chunk.F.session_expiry_సమయం = context->session_expiry_interval + db.now_real_s;
			}లేకపోతే{
				chunk.F.session_expiry_సమయం = context->session_expiry_సమయం;
			}
			chunk.F.session_expiry_interval = context->session_expiry_interval;
			chunk.F.చివరి_mid = context->చివరి_mid;
			chunk.F.id_len = (uint16_t)అక్షరశ్రేణి_పొడవు(context->id);
			chunk.clientid = context->id;
			ఒకవేళ(context->username){
				chunk.F.username_len = (uint16_t)అక్షరశ్రేణి_పొడవు(context->username);
				chunk.username = context->username;
			}
			ఒకవేళ(context->listener){
				chunk.F.listener_పోర్టు = context->listener->పోర్టు;
			}

			ఒకవేళ(chunk.F.id_len == 0){
				/* This should never happen, but in case we have a client with
				 * zero length ID, don't persist them. */
				కొనసాగు;
			}

			rc = persist__chunk_client_write_v6(db_fptr, &chunk);
			ఒకవేళ(rc){
				తిరిగి rc;
			}

			ఒకవేళ(persist__client_messages_save(db_fptr, context, context->msgs_లోపల.inflight)){
				తిరిగి 1;
			}
			ఒకవేళ(persist__client_messages_save(db_fptr, context, context->msgs_లోపల.queued)){
				తిరిగి 1;
			}
			ఒకవేళ(persist__client_messages_save(db_fptr, context, context->msgs_బహిర్గతం.inflight)){
				తిరిగి 1;
			}
			ఒకవేళ(persist__client_messages_save(db_fptr, context, context->msgs_బహిర్గతం.queued)){
				తిరిగి 1;
			}
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం persist__subs_save(FILE *db_fptr, నిర్మాణం mosquitto__subhier *కణుపు, స్థిరరాశి అక్షరం *topic, పూర్ణాంకం level)
{
	నిర్మాణం mosquitto__subhier *subhier, *subhier_tmp;
	నిర్మాణం mosquitto__subleaf *sub;
	నిర్మాణం P_sub sub_chunk;
	అక్షరం *thistopic;
	size_t slen;
	పూర్ణాంకం rc;

	slen = అక్షరశ్రేణి_పొడవు(topic) + కణుపు->topic_len + 2;
	thistopic = mosquitto_malloc(పరిమాణం(అక్షరం)*slen);
	ఒకవేళ(!thistopic){
		తిరిగి MOSQ_ERR_NOMEM;
	}
	ఒకవేళ(level > 1 || అక్షరశ్రేణి_పొడవు(topic)){
		snprintf(thistopic, slen, "%s/%s", topic, కణుపు->topic);
	}లేకపోతే{
		snprintf(thistopic, slen, "%s", కణుపు->topic);
	}

	sub = కణుపు->subs;
	అయితే(sub){
		ఒకవేళ(sub->context->session_expiry_interval != MQTT_SESSION_EXPIRY_తక్షణ
				&& sub->context->clean_ప్రారంభం == false
				&& sub->context->id){

			memset(&sub_chunk, 0, పరిమాణం(నిర్మాణం P_sub));

			sub_chunk.F.identifier = sub->identifier;
			sub_chunk.F.id_len = (uint16_t)అక్షరశ్రేణి_పొడవు(sub->context->id);
			sub_chunk.F.topic_len = (uint16_t)అక్షరశ్రేణి_పొడవు(thistopic);
			sub_chunk.F.qos = MQTT_SUB_OPT_GET_QOS(sub->subscription_options);
			sub_chunk.F.options = sub->subscription_options & 0xFC;
			sub_chunk.clientid = sub->context->id;
			sub_chunk.topic = thistopic;

			rc = persist__chunk_sub_write_v6(db_fptr, &sub_chunk);
			ఒకవేళ(rc){
				mosquitto_FREE(thistopic);
				తిరిగి rc;
			}
		}
		sub = sub->తదుపరి;
	}

	HASH_ITER(hh, కణుపు->children, subhier, subhier_tmp){
		persist__subs_save(db_fptr, subhier, thistopic, level+1);
	}
	mosquitto_FREE(thistopic);
	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం persist__subs_save_all(FILE *db_fptr)
{
	నిర్మాణం mosquitto__subhier *subhier, *subhier_tmp;

	HASH_ITER(hh, db.సాధారణ_subs, subhier, subhier_tmp){
		ఒకవేళ(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	HASH_ITER(hh, db.shared_subs, subhier, subhier_tmp){
		ఒకవేళ(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం persist__retain_save(FILE *db_fptr, నిర్మాణం mosquitto__retainhier *కణుపు, పూర్ణాంకం level)
{
	నిర్మాణం mosquitto__retainhier *retainhier, *retainhier_tmp;
	నిర్మాణం P_retain retain_chunk;
	పూర్ణాంకం rc;

	ఒకవేళ(కణుపు->retained && అక్షరశ్రేణి_పోలిక(కణుపు->retained->దత్తాంశం.topic, "$SYS", 4)){
		memset(&retain_chunk, 0, పరిమాణం(నిర్మాణం P_retain));

		/* Don't save $SYS messages. */
		retain_chunk.F.store_id = కణుపు->retained->దత్తాంశం.store_id;
		rc = persist__chunk_retain_write_v6(db_fptr, &retain_chunk);
		ఒకవేళ(rc){
			తిరిగి rc;
		}
	}

	HASH_ITER(hh, కణుపు->children, retainhier, retainhier_tmp){
		persist__retain_save(db_fptr, retainhier, level+1);
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం persist__retain_save_all(FILE *db_fptr)
{
	నిర్మాణం mosquitto__retainhier *retainhier, *retainhier_tmp;

	HASH_ITER(hh, db.retains, retainhier, retainhier_tmp){
		ఒకవేళ(retainhier->children){
			persist__retain_save(db_fptr, retainhier->children, 0);
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}

స్థిర పూర్ణాంకం persist_write_దత్తాంశం(FILE *db_fptr, శూన్య *user_దత్తాంశం);


స్థిర శూన్య persist__log_write_error(స్థిరరాశి అక్షరం *సందేశం)
{
	log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error saving in-memory database, %s", సందేశం);
}


పూర్ణాంకం persist__backup(bool shutdown)
{
	ఒకవేళ(db.config == ఖాళీ){
		తిరిగి MOSQ_ERR_INVAL;
	}
	ఒకవేళ(db.config->persistence == false){
		తిరిగి MOSQ_ERR_SUCCESS;
	}
	ఒకవేళ(db.config->persistence_filepath == ఖాళీ){
		తిరిగి MOSQ_ERR_INVAL;
	}

	log__printf(ఖాళీ, MOSQ_LOG_INFO, "Saving in-memory database to %s.", db.config->persistence_filepath);

	తిరిగి mosquitto_write_file(db.config->persistence_filepath, true, &persist_write_దత్తాంశం, &shutdown, &persist__log_write_error);
}


స్థిర పూర్ణాంకం persist_write_దత్తాంశం(FILE *db_fptr, శూన్య *user_దత్తాంశం)
{
	bool shutdown = *(bool *)(user_దత్తాంశం);
	uint32_t db_సంస్కరణ_w = htonl(MOSQ_DB_సంస్కరణ);
	uint32_t చక్రీయ_తనిఖీ = 0;
	స్థిరరాశి అక్షరం *err;
	నిర్మాణం PF_cfg cfg_chunk;
	పూర్ణాంకం rc = MOSQ_ERR_UNKNOWN;

	/*శీర్షిక*/
	write_e(db_fptr, magic, 15);
	write_e(db_fptr, &చక్రీయ_తనిఖీ, పరిమాణం(uint32_t));
	write_e(db_fptr, &db_సంస్కరణ_w, పరిమాణం(uint32_t));

	memset(&cfg_chunk, 0, పరిమాణం(నిర్మాణం PF_cfg));
	cfg_chunk.చివరి_db_id = db.చివరి_db_id;
	cfg_chunk.shutdown = shutdown;
	cfg_chunk.dbid_కొలత = పరిమాణం(dbid_t);
	ఒకవేళ(persist__chunk_cfg_write_v6(db_fptr, &cfg_chunk)){
		వెళ్ళు error;
	}

	ఒకవేళ(persist_సందేశం_store_save(db_fptr)){
		వెళ్ళు error;
	}

	ఒకవేళ(persist__client_save(db_fptr)
			|| persist__subs_save_all(db_fptr)
			|| persist__retain_save_all(db_fptr)){
		వెళ్ళు error;
	}
	తిరిగి MOSQ_ERR_SUCCESS;

error:
	err = strerror(errno);
	log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error during saving in-memory database %s: %s.", db.config->persistence_filepath, err);
	ఒకవేళ(db_fptr){
		fclose(db_fptr);
	}
	తిరిగి rc;
}


#అయితే_ముగింపు
