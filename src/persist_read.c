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
#అన్తర్భావ <utlist.h>

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "persist.h"
#అన్తర్భావ "util_mosq.h"

uint32_t db_సంస్కరణ;

స్థిరరాశి గుర్తులేని అక్షరం magic[15] = {0x00, 0xB5, 0x00, 'm', 'o', 's', 'q', 'u', 'i', 't', 't', 'o', ' ', 'd', 'b'};
స్థిర దీర్ఘ పునాది_సందేశం_లెక్క = 0;
స్థిర దీర్ఘ retained_లెక్క = 0;
స్థిర దీర్ఘ client_లెక్క = 0;
స్థిర దీర్ఘ subscription_లెక్క = 0;
స్థిర దీర్ఘ client_సందేశం_లెక్క = 0;

స్థిర పూర్ణాంకం persist__restore_sub(స్థిరరాశి నిర్మాణం mosquitto_subscription *sub);

స్థిర నిర్మాణం mosquitto *persist_కనుగొను_or_కూడు_context(స్థిరరాశి అక్షరం *clientid, uint16_t చివరి_mid)
{
	నిర్మాణం mosquitto *context;

	ఒకవేళ(!clientid){
		తిరిగి ఖాళీ;
	}

	context = ఖాళీ;
	HASH_కనుగొను(hh_id, db.contexts_by_id, clientid, అక్షరశ్రేణి_పొడవు(clientid), context);
	ఒకవేళ(!context){
		context = context__init();
		ఒకవేళ(!context){
			తిరిగి ఖాళీ;
		}
		context->id = mosquitto_strdup(clientid);
		ఒకవేళ(!context->id){
			mosquitto_FREE(context);
			తిరిగి ఖాళీ;
		}

		context->clean_ప్రారంభం = false;

		context_కూడు_గమ్యం_by_id(context);
	}
	ఒకవేళ(చివరి_mid){
		context->చివరి_mid = చివరి_mid;
	}
	తిరిగి context;
}


పూర్ణాంకం persist__read_string_len(FILE *db_fptr, అక్షరం **str, uint16_t len)
{
	అక్షరం *s = ఖాళీ;

	ఒకవేళ(len){
		s = mosquitto_malloc(len+1U);
		ఒకవేళ(!s){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
			తిరిగి MOSQ_ERR_NOMEM;
		}
		ఒకవేళ(fread(s, 1, len, db_fptr) != len){
			mosquitto_FREE(s);
			తిరిగి MOSQ_ERR_NOMEM;
		}
		s[len] = '\0';
	}

	*str = s;
	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం persist__read_string(FILE *db_fptr, అక్షరం **str)
{
	uint16_t i16temp;
	uint16_t slen;

	ఒకవేళ(fread(&i16temp, 1, పరిమాణం(uint16_t), db_fptr) != పరిమాణం(uint16_t)){
		తిరిగి MOSQ_ERR_INVAL;
	}

	slen = ntohs(i16temp);
	తిరిగి persist__read_string_len(db_fptr, str, slen);
}


స్థిర పూర్ణాంకం persist_client_సందేశం_restore(నిర్మాణం P_client_సందేశం *chunk)
{
	నిర్మాణం mosquitto_client_సందేశం *cmsg;
	నిర్మాణం mosquitto_పునాది_సందేశం *సందేశం;
	నిర్మాణం mosquitto *context;
	నిర్మాణం mosquitto_సందేశం_దత్తాంశం *సందేశం_దత్తాంశం;

	HASH_కనుగొను(hh, db.సందేశం_store, &chunk->F.store_id, పరిమాణం(chunk->F.store_id), సందేశం);
	ఒకవేళ(!సందేశం){
		/* Can't find message - probably expired */
		తిరిగి MOSQ_ERR_SUCCESS;
	}

	context = persist_కనుగొను_or_కూడు_context(chunk->clientid, 0);
	ఒకవేళ(!context){
		log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Persistence file contains client message with no matching client. File may be corrupt.");
		తిరిగి 0;
	}

	cmsg = mosquitto_calloc(1, పరిమాణం(నిర్మాణం mosquitto_client_సందేశం));
	ఒకవేళ(!cmsg){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
		తిరిగి MOSQ_ERR_NOMEM;
	}

	cmsg->తదుపరి = ఖాళీ;
	cmsg->పునాది_సందేశం = ఖాళీ;
	cmsg->దత్తాంశం.cmsg_id = ++context->చివరి_cmsg_id;
	cmsg->దత్తాంశం.mid = chunk->F.mid;
	cmsg->దత్తాంశం.qos = chunk->F.qos;
	cmsg->దత్తాంశం.retain = (chunk->F.retain_dup&0xF0)>>4;
	cmsg->దత్తాంశం.direction = chunk->F.direction;
	cmsg->దత్తాంశం.state = chunk->F.state;
	cmsg->దత్తాంశం.dup = chunk->F.retain_dup&0x0F;
	cmsg->దత్తాంశం.subscription_identifier = chunk->subscription_identifier;

	cmsg->పునాది_సందేశం = సందేశం;
	db_సందేశం_store_ref_inc(cmsg->పునాది_సందేశం);

	ఒకవేళ(cmsg->దత్తాంశం.direction == mosq_md_బహిర్గతం){
		సందేశం_దత్తాంశం = &context->msgs_బహిర్గతం;
	}లేకపోతే{
		సందేశం_దత్తాంశం = &context->msgs_లోపల;
	}

	ఒకవేళ(chunk->F.state == mosq_ms_queued || (chunk->F.qos > 0 && సందేశం_దత్తాంశం->inflight_quota == 0)){
		DL_APPEND(సందేశం_దత్తాంశం->queued, cmsg);
		db_సందేశం_కూడు_గమ్యం_queued_stats(సందేశం_దత్తాంశం, cmsg);
	}లేకపోతే{
		DL_APPEND(సందేశం_దత్తాంశం->inflight, cmsg);
		ఒకవేళ(chunk->F.qos > 0 && సందేశం_దత్తాంశం->inflight_quota > 0){
			సందేశం_దత్తాంశం->inflight_quota--;
		}
		db_సందేశం_కూడు_గమ్యం_inflight_stats(సందేశం_దత్తాంశం, cmsg);
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం persist__client_chunk_restore(FILE *db_fptr)
{
	పూర్ణాంకం rc = 0;
	నిర్మాణం mosquitto *context;
	నిర్మాణం P_client chunk;

	memset(&chunk, 0, పరిమాణం(నిర్మాణం P_client));

	ఒకవేళ(db_సంస్కరణ == 6 || db_సంస్కరణ == 5){
		rc = persist__chunk_client_read_v56(db_fptr, &chunk, db_సంస్కరణ);
	}లేకపోతే{
		rc = persist__chunk_client_read_v234(db_fptr, &chunk, db_సంస్కరణ);
	}
	ఒకవేళ(rc > 0){
		తిరిగి rc;
	}లేకపోతే ఒకవేళ(rc < 0){
		/* Client not loaded, but otherwise not an error */
		log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Empty client entry found in persistence database, it may be corrupt.");
		తిరిగి MOSQ_ERR_SUCCESS;
	}

	context = persist_కనుగొను_or_కూడు_context(chunk.clientid, chunk.F.చివరి_mid);
	ఒకవేళ(context){
		context->session_expiry_సమయం = chunk.F.session_expiry_సమయం;
		context->session_expiry_interval = chunk.F.session_expiry_interval;
		ఒకవేళ(chunk.username && !context->username){
			/* username is not freed here, it is now owned by context */
			context->username = chunk.username;
			chunk.username = ఖాళీ;
		}
		/* in per_listener_settings mode, try to find the listener by persisted port */
		ఒకవేళ(db.config->చొప్పున_listener_settings && !context->listener && chunk.F.listener_పోర్టు > 0){
			కొరకు(పూర్ణాంకం i=0; i < db.config->listener_లెక్క; i++){
				ఒకవేళ(db.config->listeners[i].పోర్టు == chunk.F.listener_పోర్టు){
					context->listener = &db.config->listeners[i];
					ఆపు;
				}
			}
		}
		session_expiry_కూడు_from_persistence(context, chunk.F.session_expiry_సమయం);
	}లేకపోతే{
		rc = 1;
	}

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.username);
	ఒకవేళ(rc == 0){
		client_లెక్క++;
	}
	తిరిగి rc;
}


స్థిర పూర్ణాంకం persist_client_సందేశం_chunk_restore(FILE *db_fptr, uint32_t నిడివి)
{
	నిర్మాణం P_client_సందేశం chunk;
	పూర్ణాంకం rc;

	memset(&chunk, 0, పరిమాణం(నిర్మాణం P_client_సందేశం));

	ఒకవేళ(db_సంస్కరణ == 6 || db_సంస్కరణ == 5){
		rc = persist_chunk_client_సందేశం_read_v56(db_fptr, &chunk, నిడివి);
	}లేకపోతే{
		rc = persist_chunk_client_సందేశం_read_v234(db_fptr, &chunk);
	}
	ఒకవేళ(rc){
		తిరిగి rc;
	}

	rc = persist_client_సందేశం_restore(&chunk);
	mosquitto_FREE(chunk.clientid);

	ఒకవేళ(rc == 0){
		client_సందేశం_లెక్క++;
	}
	తిరిగి rc;
}


స్థిర పూర్ణాంకం persist_పునాది_సందేశం_chunk_restore(FILE *db_fptr, uint32_t నిడివి)
{
	నిర్మాణం P_పునాది_సందేశం chunk;
	నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం = ఖాళీ;
	int64_t సందేశం_expiry_interval64;
	uint32_t సందేశం_expiry_interval;
	uint32_t *p_సందేశం_expiry_interval = ఖాళీ;
	పూర్ణాంకం rc = 0;

	memset(&chunk, 0, పరిమాణం(నిర్మాణం P_పునాది_సందేశం));

	ఒకవేళ(db_సంస్కరణ == 6 || db_సంస్కరణ == 5){
		rc = persist_chunk_పునాది_సందేశం_read_v56(db_fptr, &chunk, నిడివి);
	}లేకపోతే{
		rc = persist_chunk_పునాది_సందేశం_read_v234(db_fptr, &chunk, db_సంస్కరణ);
	}
	ఒకవేళ(rc){
		తిరిగి rc;
	}
	ఒకవేళ(chunk.F.topic_len == 0){
		rc = MOSQ_ERR_INVAL;
		వెళ్ళు cleanup;
	}

	ఒకవేళ(chunk.F.ఆధారం_పోర్టు){
		కొరకు(పూర్ణాంకం i=0; i<db.config->listener_లెక్క; i++){
			ఒకవేళ(db.config->listeners[i].పోర్టు == chunk.F.ఆధారం_పోర్టు){
				chunk.ఆధారం.listener = &db.config->listeners[i];
				ఆపు;
			}
		}
	}

	ఒకవేళ(chunk.F.expiry_సమయం > 0){
		సందేశం_expiry_interval64 = chunk.F.expiry_సమయం - సమయం(ఖాళీ);
		ఒకవేళ(సందేశం_expiry_interval64 < 0 || సందేశం_expiry_interval64 > UINT32_MAX){
			/* Expired message */
			rc = MOSQ_ERR_SUCCESS;
			వెళ్ళు cleanup;
		}లేకపోతే{
			సందేశం_expiry_interval = (uint32_t)సందేశం_expiry_interval64;
		}
		p_సందేశం_expiry_interval = &సందేశం_expiry_interval;
	}

	పునాది_సందేశం = mosquitto_calloc(1, పరిమాణం(నిర్మాణం mosquitto_పునాది_సందేశం));
	ఒకవేళ(పునాది_సందేశం == ఖాళీ){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
		rc = MOSQ_ERR_NOMEM;
		వెళ్ళు cleanup;
	}

	పునాది_సందేశం->దత్తాంశం.store_id = chunk.F.store_id;
	పునాది_సందేశం->దత్తాంశం.ఆధారం_mid = chunk.F.ఆధారం_mid;
	పునాది_సందేశం->దత్తాంశం.topic = chunk.topic;
	పునాది_సందేశం->దత్తాంశం.qos = chunk.F.qos;
	పునాది_సందేశం->దత్తాంశం.payloadlen = chunk.F.payloadlen;
	పునాది_సందేశం->దత్తాంశం.retain = chunk.F.retain;
	పునాది_సందేశం->దత్తాంశం.properties = chunk.properties;
	పునాది_సందేశం->దత్తాంశం.payload = chunk.payload;
	పునాది_సందేశం->ఆధారం_listener = chunk.ఆధారం.listener;

	rc = db_సందేశం_store(&chunk.ఆధారం, పునాది_సందేశం, p_సందేశం_expiry_interval,
			mosq_mo_client);

	mosquitto_FREE(chunk.ఆధారం.id);
	mosquitto_FREE(chunk.ఆధారం.username);

	ఒకవేళ(rc == MOSQ_ERR_SUCCESS){
		పునాది_సందేశం_లెక్క++;
		తిరిగి MOSQ_ERR_SUCCESS;
	}లేకపోతే{
		తిరిగి rc;
	}
cleanup:
	mosquitto_FREE(chunk.ఆధారం.id);
	mosquitto_FREE(chunk.ఆధారం.username);
	mosquitto_FREE(chunk.topic);
	mosquitto_FREE(chunk.payload);
	తిరిగి rc;
}


స్థిర పూర్ణాంకం persist__retain_chunk_restore(FILE *db_fptr)
{
	నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం;
	నిర్మాణం P_retain chunk;
	పూర్ణాంకం rc;
	అక్షరం **విభజించు_topics;
	అక్షరం *local_topic;

	memset(&chunk, 0, పరిమాణం(నిర్మాణం P_retain));

	ఒకవేళ(db_సంస్కరణ == 6 || db_సంస్కరణ == 5){
		rc = persist__chunk_retain_read_v56(db_fptr, &chunk);
	}లేకపోతే{
		rc = persist__chunk_retain_read_v234(db_fptr, &chunk);
	}
	ఒకవేళ(rc){
		తిరిగి rc;
	}

	HASH_కనుగొను(hh, db.సందేశం_store, &chunk.F.store_id, పరిమాణం(chunk.F.store_id), పునాది_సందేశం);
	ఒకవేళ(పునాది_సందేశం){
		rc = sub__topic_tokenise(పునాది_సందేశం->దత్తాంశం.topic, &local_topic, &విభజించు_topics, ఖాళీ);
		ఒకవేళ(rc){
			తిరిగి rc;
		}
		retain__store(పునాది_సందేశం->దత్తాంశం.topic, పునాది_సందేశం, విభజించు_topics, true);
		mosquitto_FREE(local_topic);
		mosquitto_FREE(విభజించు_topics);
		retained_లెక్క++;
	}లేకపోతే{
		/* Can't find the message - probably expired */
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం persist__sub_chunk_restore(FILE *db_fptr)
{
	నిర్మాణం P_sub chunk;
	పూర్ణాంకం rc;
	నిర్మాణం mosquitto_subscription sub;

	memset(&chunk, 0, పరిమాణం(నిర్మాణం P_sub));

	ఒకవేళ(db_సంస్కరణ == 6 || db_సంస్కరణ == 5){
		rc = persist__chunk_sub_read_v56(db_fptr, &chunk);
	}లేకపోతే{
		rc = persist__chunk_sub_read_v234(db_fptr, &chunk);
	}
	ఒకవేళ(rc){
		తిరిగి rc;
	}

	sub.clientid = chunk.clientid;
	sub.topic_filter = chunk.topic;
	sub.options = chunk.F.qos | chunk.F.options;
	sub.identifier = chunk.F.identifier;
	rc = persist__restore_sub(&sub);

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.topic);
	ఒకవేళ(rc == 0){
		subscription_లెక్క++;
	}

	తిరిగి rc;
}


పూర్ణాంకం persist_chunk_శీర్షిక_read(FILE *db_fptr, uint32_t *chunk, uint32_t *నిడివి)
{
	ఒకవేళ(db_సంస్కరణ == 6 || db_సంస్కరణ == 5){
		తిరిగి persist_chunk_శీర్షిక_read_v56(db_fptr, chunk, నిడివి);
	}లేకపోతే{
		తిరిగి persist_chunk_శీర్షిక_read_v234(db_fptr, chunk, నిడివి);
	}
}


పూర్ణాంకం persist__restore(శూన్య)
{
	FILE *fptr;
	అక్షరం శీర్షిక[15];
	పూర్ణాంకం rc = 0;
	uint32_t చక్రీయ_తనిఖీ;
	uint32_t i32temp;
	uint32_t chunk, నిడివి;
	size_t rlen;
	అక్షరం *err;
	నిర్మాణం PF_cfg cfg_chunk;

	ప్రతిజ్ఞా(db.config);

	ఒకవేళ(!db.config->persistence || db.config->persistence_filepath == ఖాళీ){
		తిరిగి MOSQ_ERR_SUCCESS;
	}

	db.సందేశం_store = ఖాళీ;
	పునాది_సందేశం_లెక్క = 0;
	retained_లెక్క = 0;
	client_లెక్క = 0;
	subscription_లెక్క = 0;
	client_సందేశం_లెక్క = 0;

	fptr = mosquitto_fopen(db.config->persistence_filepath, "rb", true);
	ఒకవేళ(fptr == ఖాళీ){
		తిరిగి MOSQ_ERR_SUCCESS;
	}
	rlen = fread(&శీర్షిక, 1, 15, fptr);
	ఒకవేళ(rlen == 0){
		fclose(fptr);
		log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Persistence file is empty.");
		తిరిగి 0;
	}లేకపోతే ఒకవేళ(rlen != 15){
		వెళ్ళు error;
	}
	ఒకవేళ(!memcmp(శీర్షిక, magic, 15)){
		/* Restore DB as normal */
		read_e(fptr, &చక్రీయ_తనిఖీ, పరిమాణం(uint32_t));
		read_e(fptr, &i32temp, పరిమాణం(uint32_t));
		db_సంస్కరణ = ntohl(i32temp);
		/* IMPORTANT - this is where compatibility checks are made.
		 * Is your DB change still compatible with previous versions?
		 */
		ఒకవేళ(db_సంస్కరణ != MOSQ_DB_సంస్కరణ){
			ఒకవేళ(db_సంస్కరణ == 5){
				/* Addition of username and listener_port to client chunk in v6 */
			}లేకపోతే ఒకవేళ(db_సంస్కరణ == 4){
			}లేకపోతే ఒకవేళ(db_సంస్కరణ == 3){
				/* Addition of source_username and source_port to msg_store chunk in v4, v1.5.6 */
			}లేకపోతే ఒకవేళ(db_సంస్కరణ == 2){
				/* Addition of disconnect_t to client chunk in v3. */
			}లేకపోతే{
				fclose(fptr);
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unsupported persistent database format version %d (need version %d).", db_సంస్కరణ, MOSQ_DB_సంస్కరణ);
				తిరిగి MOSQ_ERR_INVAL;
			}
		}

		అయితే(persist_chunk_శీర్షిక_read(fptr, &chunk, &నిడివి) == MOSQ_ERR_SUCCESS){
			ఎంపిక(chunk){
				స్థితి DB_CHUNK_CFG:
					ఒకవేళ(db_సంస్కరణ == 6 || db_సంస్కరణ == 5){
						rc = persist__chunk_cfg_read_v56(fptr, &cfg_chunk);
						ఒకవేళ(rc){
							fclose(fptr);
							తిరిగి rc;
						}
					}లేకపోతే{
						rc = persist__chunk_cfg_read_v234(fptr, &cfg_chunk);
						ఒకవేళ(rc){
							fclose(fptr);
							తిరిగి rc;
						}
					}
					ఒకవేళ(cfg_chunk.dbid_కొలత != పరిమాణం(dbid_t)){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Incompatible database configuration (dbid size is %d bytes, expected %lu)",
								cfg_chunk.dbid_కొలత, (గుర్తులేని దీర్ఘ)పరిమాణం(dbid_t));
						fclose(fptr);
						తిరిగి MOSQ_ERR_INVAL;
					}
					db.చివరి_db_id = cfg_chunk.చివరి_db_id;
					ఆపు;

				స్థితి DB_CHUNK_పునాది_సందేశం:
					rc = persist_పునాది_సందేశం_chunk_restore(fptr, నిడివి);
					ఒకవేళ(rc){
						fclose(fptr);
						తిరిగి rc;
					}
					ఆపు;

				స్థితి DB_CHUNK_CLIENT_సందేశం:
					rc = persist_client_సందేశం_chunk_restore(fptr, నిడివి);
					ఒకవేళ(rc){
						fclose(fptr);
						తిరిగి rc;
					}
					ఆపు;

				స్థితి DB_CHUNK_RETAIN:
					rc = persist__retain_chunk_restore(fptr);
					ఒకవేళ(rc){
						fclose(fptr);
						తిరిగి rc;
					}
					ఆపు;

				స్థితి DB_CHUNK_SUB:
					rc = persist__sub_chunk_restore(fptr);
					ఒకవేళ(rc){
						fclose(fptr);
						తిరిగి rc;
					}
					ఆపు;

				స్థితి DB_CHUNK_CLIENT:
					rc = persist__client_chunk_restore(fptr);
					ఒకవేళ(rc){
						fclose(fptr);
						తిరిగి rc;
					}
					ఆపు;

				అప్రమేయ:
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Unsupported chunk \"%d\" in persistent database file. Ignoring.", chunk);
					ఒకవేళ(fseek(fptr, నిడివి, SEEK_CUR) < 0){
						fclose(fptr);
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఆపు;
			}
		}
	}లేకపోతే{
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to restore persistent database. Unrecognised file format.");
		rc = 1;
	}

	fclose(fptr);

	log__printf(ఖాళీ, MOSQ_LOG_INFO, "Restored %ld base messages", పునాది_సందేశం_లెక్క);
	log__printf(ఖాళీ, MOSQ_LOG_INFO, "Restored %ld retained messages", retained_లెక్క);
	log__printf(ఖాళీ, MOSQ_LOG_INFO, "Restored %ld clients", client_లెక్క);
	log__printf(ఖాళీ, MOSQ_LOG_INFO, "Restored %ld subscriptions", subscription_లెక్క);
	log__printf(ఖాళీ, MOSQ_LOG_INFO, "Restored %ld client messages", client_సందేశం_లెక్క);

	తిరిగి rc;
error:
	err = strerror(errno);
	log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: %s.", err);
	ఒకవేళ(fptr){
		fclose(fptr);
	}
	తిరిగి MOSQ_ERR_ERRNO;
}


స్థిర పూర్ణాంకం persist__restore_sub(స్థిరరాశి నిర్మాణం mosquitto_subscription *sub)
{
	నిర్మాణం mosquitto *context;

	ఒకవేళ(!sub->clientid){
		log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no client id, ignoring.");
		తిరిగి MOSQ_ERR_SUCCESS;
	}
	ఒకవేళ(!sub->topic_filter){
		log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no topic filter, ignoring.");
		తిరిగి MOSQ_ERR_SUCCESS;
	}

	context = persist_కనుగొను_or_కూడు_context(sub->clientid, 0);
	ఒకవేళ(!context){
		తిరిగి MOSQ_ERR_INVAL;
	}
	తిరిగి sub_కూడు(context, sub);
}

#అయితే_ముగింపు
