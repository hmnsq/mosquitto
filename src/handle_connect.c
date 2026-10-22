/*
Copyright (c) 2009-2021 Roger Light <roger@atchoo.org>

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

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <utlist.h>

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto/mqtt_protocol.h"
#అన్తర్భావ "packet_mosq.h"
#అన్తర్భావ "property_mosq.h"
#అన్తర్భావ "send_mosq.h"
#అన్తర్భావ "sys_tree.h"
#అన్తర్భావ "tls_mosq.h"
#అన్తర్భావ "util_mosq.h"
#అన్తర్భావ "will_mosq.h"

#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#అయితే_ముగింపు


స్థిర అక్షరం nibble_గమ్యం_hex(uint8_t విలువ)
{
	ఒకవేళ(విలువ < 0x0A){
		తిరిగి (అక్షరం)('0'+విలువ);
	}లేకపోతే{
		తిరిగి (అక్షరం)(65 /*'A'*/ +విలువ-10);
	}
}


స్థిర అక్షరం *clientid_gen(uint16_t *idlen, స్థిరరాశి అక్షరం *auto_id_ఉపసర్గ, uint16_t auto_id_ఉపసర్గ_len)
{
	అక్షరం *clientid;
	uint8_t rnd[16];
	పూర్ణాంకం pos;

	ఒకవేళ(mosquitto_getrandom(rnd, 16)){
		తిరిగి ఖాళీ;
	}

	*idlen = (uint16_t)(auto_id_ఉపసర్గ_len + 36);

	clientid = (అక్షరం *)mosquitto_calloc((size_t)(*idlen) + 1, పరిమాణం(అక్షరం));
	ఒకవేళ(!clientid){
		తిరిగి ఖాళీ;
	}
	ఒకవేళ(auto_id_ఉపసర్గ){
		జ్ఞాపకం_నకలు(clientid, auto_id_ఉపసర్గ, auto_id_ఉపసర్గ_len);
	}

	pos = 0;
	కొరకు(పూర్ణాంకం i=0; i<16; i++){
		clientid[auto_id_ఉపసర్గ_len + pos + 0] = nibble_గమ్యం_hex(rnd[i] & 0x0F);
		clientid[auto_id_ఉపసర్గ_len + pos + 1] = nibble_గమ్యం_hex((rnd[i] >> 4) & 0x0F);
		pos += 2;
		ఒకవేళ(pos == 8 || pos == 13 || pos == 18 || pos == 23){
			clientid[auto_id_ఉపసర్గ_len + pos] = '-';
			pos++;
		}
	}

	తిరిగి clientid;
}


పూర్ణాంకం connect__on_authorised(నిర్మాణం mosquitto *context, శూన్య *auth_దత్తాంశం_బహిర్గతం, uint16_t auth_దత్తాంశం_బహిర్గతం_len)
{
	నిర్మాణం mosquitto *దొరికింది_context;
	నిర్మాణం mosquitto__subleaf *ఆకు;
	mosquitto_property *connack_props = ఖాళీ;
	uint8_t connect_ack = 0;
	పూర్ణాంకం rc;
	పూర్ణాంకం లోపల_quota, బహిర్గతం_quota;
	uint16_t లోపల_గరిష్ఠ, బహిర్గతం_గరిష్ఠ;

	/* Find if this client already has an entry. This must be done *after* any security checks. */
	HASH_కనుగొను(hh_id, db.contexts_by_id, context->id, అక్షరశ్రేణి_పొడవు(context->id), దొరికింది_context);
	ఒకవేళ(దొరికింది_context){
		/* Found a matching client */
		ఒకవేళ(!net__is_connected(దొరికింది_context)){
			/* Client is reconnecting after a disconnect */
			/* FIXME - does anything need to be done here? */
		}లేకపోతే{
			/* Client is already connected, disconnect old version. This is
			 * done in context__cleanup() below. */
		}

		ఒకవేళ(context->clean_ప్రారంభం == true){
			sub__clean_session(దొరికింది_context);
			దొరికింది_context->session_expiry_interval = MQTT_SESSION_EXPIRY_తక్షణ;
			plugin_persist__handle_client_delete(దొరికింది_context);
		}
		context->is_persisted = దొరికింది_context->is_persisted;
		దొరికింది_context->is_persisted = false; /* stops persistence for context being removed */

		ఒకవేళ(context->clean_ప్రారంభం == false && దొరికింది_context->session_expiry_interval != MQTT_SESSION_EXPIRY_తక్షణ){
			ఒకవేళ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
				connect_ack |= 0x01;
			}

			ఒకవేళ(దొరికింది_context->msgs_లోపల.inflight || దొరికింది_context->msgs_లోపల.queued
					|| దొరికింది_context->msgs_బహిర్గతం.inflight || దొరికింది_context->msgs_బహిర్గతం.queued){

				లోపల_quota = context->msgs_లోపల.inflight_quota;
				బహిర్గతం_quota = context->msgs_బహిర్గతం.inflight_quota;
				లోపల_గరిష్ఠ = context->msgs_లోపల.inflight_గరిష్ఠ;
				బహిర్గతం_గరిష్ఠ = context->msgs_బహిర్గతం.inflight_గరిష్ఠ;

				జ్ఞాపకం_నకలు(&context->msgs_లోపల, &దొరికింది_context->msgs_లోపల, పరిమాణం(నిర్మాణం mosquitto_సందేశం_దత్తాంశం));
				జ్ఞాపకం_నకలు(&context->msgs_బహిర్గతం, &దొరికింది_context->msgs_బహిర్గతం, పరిమాణం(నిర్మాణం mosquitto_సందేశం_దత్తాంశం));
				context->చివరి_cmsg_id = దొరికింది_context->చివరి_cmsg_id;

				memset(&దొరికింది_context->msgs_లోపల, 0, పరిమాణం(నిర్మాణం mosquitto_సందేశం_దత్తాంశం));
				memset(&దొరికింది_context->msgs_బహిర్గతం, 0, పరిమాణం(నిర్మాణం mosquitto_సందేశం_దత్తాంశం));

				context->msgs_లోపల.inflight_quota = లోపల_quota;
				context->msgs_బహిర్గతం.inflight_quota = బహిర్గతం_quota;
				context->msgs_లోపల.inflight_గరిష్ఠ = లోపల_గరిష్ఠ;
				context->msgs_బహిర్గతం.inflight_గరిష్ఠ = బహిర్గతం_గరిష్ఠ;

				db_సందేశం_reconnect_reset(context);
			}
			context->subs = దొరికింది_context->subs;
			దొరికింది_context->subs = ఖాళీ;
			context->subs_సామర్థ్యం = దొరికింది_context->subs_సామర్థ్యం;
			context->subs_లెక్క = దొరికింది_context->subs_లెక్క;
			దొరికింది_context->subs_సామర్థ్యం = 0;
			దొరికింది_context->subs_లెక్క = 0;
			context->చివరి_mid = దొరికింది_context->చివరి_mid;

			కొరకు(పూర్ణాంకం i=0; i<context->subs_సామర్థ్యం; i++){
				ఒకవేళ(context->subs[i]){
					ఆకు = context->subs[i]->hier->subs;
					అయితే(ఆకు){
						ఒకవేళ(ఆకు->context == దొరికింది_context){
							ఆకు->context = context;
						}
						ఆకు = ఆకు->తదుపరి;
					}

					ఒకవేళ(context->subs[i]->shared){
						ఆకు = context->subs[i]->shared->subs;
						అయితే(ఆకు){
							ఒకవేళ(ఆకు->context == దొరికింది_context){
								ఆకు->context = context;
							}
							ఆకు = ఆకు->తదుపరి;
						}
					}
				}
			}
		}

		ఒకవేళ((దొరికింది_context->protocol == mosq_p_mqtt5 && దొరికింది_context->session_expiry_interval == MQTT_SESSION_EXPIRY_తక్షణ)
				|| (దొరికింది_context->protocol != mosq_p_mqtt5 && దొరికింది_context->clean_ప్రారంభం == true)
				|| (context->clean_ప్రారంభం == true)
				){

			context__send_will(దొరికింది_context);
		}

		session_expiry__remove(దొరికింది_context);
		will_delay__remove(దొరికింది_context);
		will__clear(దొరికింది_context);

		దొరికింది_context->clean_ప్రారంభం = true;
		దొరికింది_context->session_expiry_interval = MQTT_SESSION_EXPIRY_తక్షణ;
		mosquitto_సమితి_state(దొరికింది_context, mosq_cs_duplicate);
		ఒకవేళ(దొరికింది_context->protocol == mosq_p_mqtt5){
			send__disconnect(దొరికింది_context, MQTT_RC_SESSION_తీసుకున్న_OVER, ఖాళీ);
		}
		do_disconnect(దొరికింది_context, MOSQ_ERR_SESSION_తీసుకున్న_OVER);
	}

	ఒకవేళ(db.config->global_max_clients > 0 && HASH_CNT(hh_id, db.contexts_by_id) >= (గుర్తులేని పూర్ణాంకం)db.config->global_max_clients){
		ఒకవేళ(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_SERVER_BUSY, ఖాళీ);
		}
		rc = MOSQ_ERR_INVAL;
		వెళ్ళు error;
	}

	ఒకవేళ(db.config->connection_messages == true){
		ఒకవేళ(context->is_bridge){
			ఒకవేళ(context->username){
				log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->చిరునామా, context->remote_పోర్టు, context->id, context->protocol, context->clean_ప్రారంభం, context->keepalive, context->username);
			}లేకపోతే{
				log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d).",
						context->చిరునామా, context->remote_పోర్టు, context->id, context->protocol, context->clean_ప్రారంభం, context->keepalive);
			}
		}లేకపోతే{
			ఒకవేళ(context->username){
				log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->చిరునామా, context->remote_పోర్టు, context->id, context->protocol, context->clean_ప్రారంభం, context->keepalive, context->username);
			}లేకపోతే{
				log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d).",
						context->చిరునామా, context->remote_పోర్టు, context->id, context->protocol, context->clean_ప్రారంభం, context->keepalive);
			}
		}

		ఒకవేళ(context->will){
			log__printf(ఖాళీ, MOSQ_LOG_DEBUG, "Will message specified (%ld bytes) (r%d, q%d).",
					(దీర్ఘ)context->will->సందేశం.payloadlen,
					context->will->సందేశం.retain,
					context->will->సందేశం.qos);

			log__printf(ఖాళీ, MOSQ_LOG_DEBUG, "\t%s", context->will->సందేశం.topic);
		}లేకపోతే{
			log__printf(ఖాళీ, MOSQ_LOG_DEBUG, "No will message specified.");
		}
	}
#నిర్వచితమైతే WITH_TLS
	ఒకవేళ(context->ssl){
		log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s negotiated %s cipher %s",
				context->id,
				SSL_get_cipher_సంస్కరణ(context->ssl),
				SSL_get_cipher_name(context->ssl));
	}
#అయితే_ముగింపు

	context->ping_t = 0;
	context->is_dropping = false;

	/* Remove any queued messages that are no longer allowed through ACL,
	 * assuming a possible change of username. */
	db_check_acl_యొక్క_all_messages(context);
	context_కూడు_గమ్యం_by_id(context);

#నిర్వచితమైతే WITH_PERSISTENCE
	ఒకవేళ(!context->clean_ప్రారంభం){
		db.persistence_changes++;
	}
#అయితే_ముగింపు
	context->max_qos = context->listener->max_qos;

	ఒకవేళ(db.config->max_keepalive &&
			(context->keepalive > db.config->max_keepalive || context->keepalive == 0)){

		keepalive__remove(context);
		context->keepalive = db.config->max_keepalive;
		keepalive_కూడు(context);
		ఒకవేళ(context->protocol == mosq_p_mqtt5){
			ఒకవేళ(mosquitto_property_కూడు_int16(&connack_props, MQTT_PROP_SERVER_KEEP_ALIVE, context->keepalive)){
				rc = MOSQ_ERR_NOMEM;
				వెళ్ళు error;
			}
		}లేకపోతే{
			send__connack(context, connect_ack, CONNACK_REFUSED_IDENTIFIER_REJECTED, ఖాళీ);
			rc = MOSQ_ERR_INVAL;
			వెళ్ళు error;
		}
	}


	ఒకవేళ(context->protocol == mosq_p_mqtt5){
		ఒకవేళ(context->listener->max_topic_alias > 0){
			ఒకవేళ(mosquitto_property_కూడు_int16(&connack_props, MQTT_PROP_TOPIC_ALIAS_గరిష్ఠ, context->listener->max_topic_alias)){
				rc = MOSQ_ERR_NOMEM;
				వెళ్ళు error;
			}
		}
		ఒకవేళ(context->assigned_id){
			ఒకవేళ(mosquitto_property_కూడు_string(&connack_props, MQTT_PROP_ASSIGNED_CLIENT_IDENTIFIER, context->id)){
				rc = MOSQ_ERR_NOMEM;
				వెళ్ళు error;
			}
		}
		ఒకవేళ(context->auth_పద్ధతి){
			ఒకవేళ(mosquitto_property_కూడు_string(&connack_props, MQTT_PROP_AUTHENTICATION_పద్ధతి, context->auth_పద్ధతి)){
				rc = MOSQ_ERR_NOMEM;
				వెళ్ళు error;
			}

			ఒకవేళ(auth_దత్తాంశం_బహిర్గతం && auth_దత్తాంశం_బహిర్గతం_len > 0){
				ఒకవేళ(mosquitto_property_కూడు_ద్విమాన(&connack_props, MQTT_PROP_AUTHENTICATION_దత్తాంశం, auth_దత్తాంశం_బహిర్గతం, auth_దత్తాంశం_బహిర్గతం_len)){
					rc = MOSQ_ERR_NOMEM;
					వెళ్ళు error;
				}
			}
		}
	}
	సురక్షిత_FREE(auth_దత్తాంశం_బహిర్గతం);

	mosquitto_సమితి_state(context, mosq_cs_చురుకైన);
	rc = send__connack(context, connect_ack, CONNACK_ACCEPTED, connack_props);
	mosquitto_property_free_all(&connack_props);
	ఒకవేళ(rc){
		తిరిగి rc;
	}
	db__expire_all_messages(context);
	rc = db_సందేశం_write_queued_బహిర్గతం(context);
	ఒకవేళ(rc){
		తిరిగి rc;
	}
	rc = db_సందేశం_write_inflight_బహిర్గతం_all(context);

	ఒకవేళ(rc == MOSQ_ERR_SUCCESS){
		plugin__handle_connect(context);

		ఒకవేళ(context->session_expiry_interval != MQTT_SESSION_EXPIRY_తక్షణ){
			plugin_persist_handle_client_కూడు(context);
		}లేకపోతే ఒకవేళ(context->will){
			plugin_persist_handle_will_కూడు(context);
		}
	}
	తిరిగి rc;
error:
	సురక్షిత_FREE(auth_దత్తాంశం_బహిర్గతం);
	mosquitto_property_free_all(&connack_props);
	తిరిగి rc;
}


స్థిర పూర్ణాంకం will__read(నిర్మాణం mosquitto *context, స్థిరరాశి అక్షరం *clientid, నిర్మాణం mosquitto_సందేశం_all **will, uint8_t will_qos, పూర్ణాంకం will_retain)
{
	పూర్ణాంకం rc = MOSQ_ERR_SUCCESS;
	size_t slen;
	uint16_t tlen;
	నిర్మాణం mosquitto_సందేశం_all *will_struct = ఖాళీ;
	అక్షరం *will_topic_mount = ఖాళీ;
	uint16_t payloadlen;
	mosquitto_property *properties = ఖాళీ;

	will_struct = mosquitto_calloc(1, పరిమాణం(నిర్మాణం mosquitto_సందేశం_all));
	ఒకవేళ(!will_struct){
		rc = MOSQ_ERR_NOMEM;
		వెళ్ళు error_cleanup;
	}
	ఒకవేళ(context->protocol == PROTOCOL_సంస్కరణ_v5){
		rc = property__read_all(CMD_WILL, &context->లోపల_packet, &properties);
		ఒకవేళ(rc){
			వెళ్ళు error_cleanup;
		}

		rc = property__process_will(context, will_struct, &properties);
		mosquitto_property_free_all(&properties);
		ఒకవేళ(rc){
			వెళ్ళు error_cleanup;
		}
	}
	rc = packet__read_string(&context->లోపల_packet, &will_struct->సందేశం.topic, &tlen);
	ఒకవేళ(rc){
		వెళ్ళు error_cleanup;
	}
	ఒకవేళ(!tlen){
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: Will with empty topic.",
				context->id);
		rc = MOSQ_ERR_PROTOCOL;
		వెళ్ళు error_cleanup;
	}

	ఒకవేళ(context->listener->mount_బిందువు){
		slen = అక్షరశ్రేణి_పొడవు(context->listener->mount_బిందువు) + అక్షరశ్రేణి_పొడవు(will_struct->సందేశం.topic) + 1;
		will_topic_mount = mosquitto_malloc(slen+1);
		ఒకవేళ(!will_topic_mount){
			rc = MOSQ_ERR_NOMEM;
			వెళ్ళు error_cleanup;
		}

		snprintf(will_topic_mount, slen, "%s%s", context->listener->mount_బిందువు, will_struct->సందేశం.topic);
		will_topic_mount[slen] = '\0';

		mosquitto_FREE(will_struct->సందేశం.topic);
		will_struct->సందేశం.topic = will_topic_mount;
	}

	ఒకవేళ(!అక్షరశ్రేణి_పోలిక(will_struct->సందేశం.topic, "$CONTROL/", అక్షరశ్రేణి_పొడవు("$CONTROL/"))){
		rc = MOSQ_ERR_ACL_DENIED;
		వెళ్ళు error_cleanup;
	}
	rc = mosquitto_pub_topic_check(will_struct->సందేశం.topic);
	ఒకవేళ(rc){
		వెళ్ళు error_cleanup;
	}

	rc = packet__read_uint16(&context->లోపల_packet, &payloadlen);
	ఒకవేళ(rc){
		వెళ్ళు error_cleanup;
	}

	will_struct->సందేశం.payloadlen = payloadlen;
	ఒకవేళ(will_struct->సందేశం.payloadlen > 0){
		ఒకవేళ(db.config->సందేశం_కొలత_హద్దు && will_struct->సందేశం.payloadlen > (పూర్ణాంకం)db.config->సందేశం_కొలత_హద్దు){
			log__printf(ఖాళీ, MOSQ_LOG_DEBUG, "Client %s connected with too large Will payload", clientid);
			ఒకవేళ(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_PACKET_TOO_LARGE, ఖాళీ);
			}లేకపోతే{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ఖాళీ);
			}
			rc = MOSQ_ERR_PAYLOAD_కొలత;
			వెళ్ళు error_cleanup;
		}
		will_struct->సందేశం.payload = mosquitto_malloc((size_t)will_struct->సందేశం.payloadlen);
		ఒకవేళ(!will_struct->సందేశం.payload){
			rc = MOSQ_ERR_NOMEM;
			వెళ్ళు error_cleanup;
		}

		rc = packet_read_బైట్లు(&context->లోపల_packet, will_struct->సందేశం.payload, (uint32_t)will_struct->సందేశం.payloadlen);
		ఒకవేళ(rc){
			వెళ్ళు error_cleanup;
		}
	}

	will_struct->సందేశం.qos = will_qos;
	will_struct->సందేశం.retain = will_retain;

	*will = will_struct;
	తిరిగి MOSQ_ERR_SUCCESS;

error_cleanup:
	ఒకవేళ(will_struct){
		mosquitto_FREE(will_struct->సందేశం.topic);
		mosquitto_FREE(will_struct->సందేశం.payload);
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct);
	}
	తిరిగి rc;
}


స్థిర పూర్ణాంకం check_protocol_సంస్కరణ(నిర్మాణం mosquitto__listener *listener, పూర్ణాంకం protocol_సంస్కరణ)
{
	/* Allow bridge protocol as well. */
	protocol_సంస్కరణ &= 0x7F;

	ఒకవేళ((protocol_సంస్కరణ == 3 && listener->disable_protocol_v3 == false)
			|| (protocol_సంస్కరణ == 4 && listener->disable_protocol_v4 == false)
			|| (protocol_సంస్కరణ == 5 && listener->disable_protocol_v5 == false)
			){

		తిరిగి MOSQ_ERR_SUCCESS;
	}లేకపోతే{
		తిరిగి MOSQ_ERR_NOT_SUPPORTED;
	}
}


అంతఃవరుస స్థిర పూర్ణాంకం send__connack_error_and_return(నిర్మాణం mosquitto *context, uint8_t err_సంకేతం, పూర్ణాంకం rc)
{
	send__connack(context, 0, err_సంకేతం, ఖాళీ);
	తిరిగి rc;
}


అంతఃవరుస స్థిర పూర్ణాంకం send__connack_bad_username_or_password_error(నిర్మాణం mosquitto *context, పూర్ణాంకం rc)
{
	uint8_t err_సంకేతం  = context->protocol == mosq_p_mqtt5
									? (uint8_t)MQTT_RC_BAD_USERNAME_OR_PASSWORD
									: (uint8_t)CONNACK_REFUSED_BAD_USERNAME_PASSWORD;
	తిరిగి send__connack_error_and_return(context, err_సంకేతం, rc);
}


స్థిర పూర్ణాంకం read_protocol_name(నిర్మాణం mosquitto *context, అక్షరం protocol_name[7])
{
	/* Read protocol name as length then bytes rather than with read_string
	 * because the length is fixed and we can check that. Removes the need
	 * for another malloc as well. */

	uint16_t slen = 0;

	ఒకవేళ(packet__read_uint16(&context->లోపల_packet, &slen)){
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string length.",
				context->చిరునామా, context->remote_పోర్టు);
		తిరిగి MOSQ_ERR_PROTOCOL;
	}
	ఒకవేళ(slen != 4 /* MQTT */ && slen != 6 /* MQIsdp */){
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with incorrect protocol string length (%d).",
				context->చిరునామా, context->remote_పోర్టు, slen);
		తిరిగి MOSQ_ERR_PROTOCOL;
	}
	ఒకవేళ(packet_read_బైట్లు(&context->లోపల_packet, protocol_name, slen)){
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string.",
				context->చిరునామా, context->remote_పోర్టు);
		తిరిగి MOSQ_ERR_PROTOCOL;
	}
	protocol_name[slen] = '\0';

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం read_and_verify_protocol_సంస్కరణ(నిర్మాణం mosquitto *context, స్థిరరాశి అక్షరం *protocol_name,
		uint8_t *protocol_సంస్కరణ)
{
	uint8_t tmp_protocol_సంస్కరణ = 0;
	ఒకవేళ(packet__read_byte(&context->లోపల_packet, &tmp_protocol_సంస్కరణ)){
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol version.",
				context->చిరునామా, context->remote_పోర్టు);
		తిరిగి MOSQ_ERR_PROTOCOL;
	}

	ఒకవేళ(check_protocol_సంస్కరణ(context->listener, tmp_protocol_సంస్కరణ)){
		ఒకవేళ(tmp_protocol_సంస్కరణ == 3 || tmp_protocol_సంస్కరణ == 4){
			context->protocol = mosq_p_mqtt311;
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_సంస్కరణ, ఖాళీ);
		}లేకపోతే{
			context->protocol = mosq_p_mqtt5;
			send__connack(context, 0, MQTT_RC_UNSUPPORTED_PROTOCOL_సంస్కరణ, ఖాళీ);
		}
		తిరిగి MOSQ_ERR_NOT_SUPPORTED;
	}

	ఒకవేళ(!strcmp(protocol_name, PROTOCOL_NAME_v31)){
		ఒకవేళ((tmp_protocol_సంస్కరణ&0x7F) != PROTOCOL_సంస్కరణ_v31){
			ఒకవేళ(db.config->connection_messages == true){
				log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->చిరునామా, context->remote_పోర్టు, tmp_protocol_సంస్కరణ);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_సంస్కరణ, ఖాళీ);
			తిరిగి MOSQ_ERR_PROTOCOL;
		}
		context->protocol = mosq_p_mqtt31;
		ఒకవేళ((tmp_protocol_సంస్కరణ&0x80) == 0x80){
			context->is_bridge = true;
		}
	}లేకపోతే ఒకవేళ(!strcmp(protocol_name, PROTOCOL_NAME)){
		ఒకవేళ((tmp_protocol_సంస్కరణ&0x7F) == PROTOCOL_సంస్కరణ_v311){
			context->protocol = mosq_p_mqtt311;

			ఒకవేళ((tmp_protocol_సంస్కరణ&0x80) == 0x80){
				context->is_bridge = true;
			}
		}లేకపోతే ఒకవేళ((tmp_protocol_సంస్కరణ&0x7F) == PROTOCOL_సంస్కరణ_v5){
			context->protocol = mosq_p_mqtt5;
		}లేకపోతే{
			ఒకవేళ(db.config->connection_messages == true){
				log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->చిరునామా, context->remote_పోర్టు, tmp_protocol_సంస్కరణ);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_సంస్కరణ, ఖాళీ);
			తిరిగి MOSQ_ERR_PROTOCOL;
		}
		ఒకవేళ((context->లోపల_packet.command&0x0F) != 0x00){
			/* Reserved flags not set to 0, must disconnect. */
			log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero reserved flags (%02X).",
					context->చిరునామా, context->remote_పోర్టు, context->లోపల_packet.command);
			తిరిగి MOSQ_ERR_PROTOCOL;
		}
	}లేకపోతే{
		ఒకవేళ(db.config->connection_messages == true){
			log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol \"%s\".",
					context->చిరునామా, context->remote_పోర్టు, protocol_name);
		}
		తిరిగి MOSQ_ERR_PROTOCOL;
	}
	ఒకవేళ((tmp_protocol_సంస్కరణ&0x7F) != PROTOCOL_సంస్కరణ_v31 && context->లోపల_packet.command != CMD_CONNECT){
		తిరిగి MOSQ_ERR_MALFORMED_PACKET;
	}

	*protocol_సంస్కరణ = tmp_protocol_సంస్కరణ;
	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం read_and_verify_connect_flags(నిర్మాణం mosquitto *context, uint8_t *connect_flags)
{
	ఒకవేళ(packet__read_byte(&context->లోపల_packet, connect_flags)){
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing connect flags.",
				context->చిరునామా, context->remote_పోర్టు);
		తిరిగి MOSQ_ERR_PROTOCOL;
	}
	ఒకవేళ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		ఒకవేళ((*connect_flags & 0x01) != 0x00){
			log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero connect reserved flag (%02X).",
					context->చిరునామా, context->remote_పోర్టు, *connect_flags);
			తిరిగి MOSQ_ERR_PROTOCOL;
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర శూన్య సమితి_session_expiry_interval(నిర్మాణం mosquitto *context, uint8_t clean_ప్రారంభం, uint8_t protocol_సంస్కరణ)
{
	/* session_expiry_interval will be overridden if the properties are read later */
	ఒకవేళ(clean_ప్రారంభం == false && protocol_సంస్కరణ != PROTOCOL_సంస్కరణ_v5){
		/* v3* has clean_start == false mean the session never expires */
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
	}లేకపోతే{
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_తక్షణ;
	}
}


స్థిర పూర్ణాంకం read_and_reset_keepalive(నిర్మాణం mosquitto *context)
{
	/* _remove here because net__socket_accept() uses _add and we must have the
	 * correct keepalive value */
	keepalive__remove(context);

	ఒకవేళ(packet__read_uint16(&context->లోపల_packet, &(context->keepalive))){
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "%s sent CONNECT with missing keepalive.",
				context->id);
		తిరిగి MOSQ_ERR_PROTOCOL;
	}
	keepalive_కూడు(context);

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం read_and_verify_v5_connect_properties(నిర్మాణం mosquitto *context, mosquitto_property **properties, uint8_t protocol_సంస్కరణ)
{
	పూర్ణాంకం rc;

	ఒకవేళ(protocol_సంస్కరణ == PROTOCOL_సంస్కరణ_v5){
		rc = property__read_all(CMD_CONNECT, &context->లోపల_packet, properties);
		ఒకవేళ(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
			send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, ఖాళీ);
		}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_MALFORMED_PACKET){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, ఖాళీ);
		}
		ఒకవేళ(rc){
			తిరిగి rc;
		}
	}
	rc = property__process_connect(context, properties);
	ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
		తిరిగి send__connack_error_and_return(context, MQTT_RC_PROTOCOL_ERROR, rc);
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం verify_will_options(నిర్మాణం mosquitto *context, uint8_t will, uint8_t will_qos, uint8_t will_retain, uint8_t protocol_సంస్కరణ)
{
	ఒకవేళ(will_qos == 3){
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid Will QoS (%d).",
				context->చిరునామా, context->remote_పోర్టు, will_qos);
		తిరిగి MOSQ_ERR_PROTOCOL;
	}

	ఒకవేళ(will && will_retain && db.config->retain_అందుబాటు == false){
		ఒకవేళ(protocol_సంస్కరణ == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_RETAIN_NOT_SUPPORTED, ఖాళీ);
		}
		తిరిగి MOSQ_ERR_NOT_SUPPORTED;
	}

	ఒకవేళ(will && will_qos > context->listener->max_qos){
		ఒకవేళ(protocol_సంస్కరణ == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_QOS_NOT_SUPPORTED, ఖాళీ);
		}
		తిరిగి MOSQ_ERR_NOT_SUPPORTED;
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం handle_zero_నిడివి_clientid(నిర్మాణం mosquitto *context, అక్షరం **clientid, bool *allow_zero_నిడివి_clientid,
		uint8_t clean_ప్రారంభం)
{
	ఒకవేళ(context->protocol == mosq_p_mqtt31){
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s:%d: v3.1 CONNECT with zero length clientid.",
				context->చిరునామా, context->remote_పోర్టు);
		send__connack(context, 0, CONNACK_REFUSED_IDENTIFIER_REJECTED, ఖాళీ);
		తిరిగి MOSQ_ERR_PROTOCOL;
	}

	/* mqtt311/mqtt5 */
	mosquitto_FREE(*clientid);

	ఒకవేళ(db.config->చొప్పున_listener_settings){
		*allow_zero_నిడివి_clientid = context->listener->security_options->allow_zero_నిడివి_clientid;
	}లేకపోతే{
		*allow_zero_నిడివి_clientid = db.config->security_options.allow_zero_నిడివి_clientid;
	}

	ఒకవేళ((context->protocol == mosq_p_mqtt311 && clean_ప్రారంభం == 0) || *allow_zero_నిడివి_clientid == false){
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with zero length clientid when forbidden.",
				context->చిరునామా, context->remote_పోర్టు);
		uint8_t err_సంకేతం = context->protocol == mosq_p_mqtt311 ? (uint8_t)CONNACK_REFUSED_IDENTIFIER_REJECTED : (uint8_t)MQTT_RC_UNSPECIFIED;
		తిరిగి send__connack_error_and_return(context, err_సంకేతం, MOSQ_ERR_PROTOCOL);
	}

	*clientid = clientid_gen(&(uint16_t){0}, context->listener->security_options->auto_id_ఉపసర్గ,
			context->listener->security_options->auto_id_ఉపసర్గ_len);
	ఒకవేళ(*clientid == ఖాళీ){
		తిరిగి MOSQ_ERR_NOMEM;
	}
	context->assigned_id = true;

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం check_clientid_prefixes(నిర్మాణం mosquitto *context, స్థిరరాశి అక్షరం *clientid)
{
	ఒకవేళ(db.config->clientid_prefixes){
		ఒకవేళ(అక్షరశ్రేణి_పోలిక(db.config->clientid_prefixes, clientid, అక్షరశ్రేణి_పొడవు(db.config->clientid_prefixes))){
			uint8_t err_సంకేతం = context->protocol == mosq_p_mqtt5 ? (uint8_t)MQTT_RC_NOT_AUTHORIZED : (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
			తిరిగి send__connack_error_and_return(context, err_సంకేతం, MOSQ_ERR_AUTH);
		}
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం read_and_verify_clientid_from_packet(నిర్మాణం mosquitto *context, అక్షరం **clientid,
		bool *allow_zero_నిడివి_clientid, uint8_t clean_ప్రారంభం)
{
	పూర్ణాంకం rc;
	uint16_t slen;

	rc = packet__read_string(&context->లోపల_packet, clientid, &slen);
	ఒకవేళ(rc == MOSQ_ERR_MALFORMED_UTF8){
		ఒకవేళ(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_CLIENTID_NOT_VALID, ఖాళీ);
		}
		తిరిగి MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID;
	}లేకపోతే ఒకవేళ(rc > 0){
		తిరిగి MOSQ_ERR_PROTOCOL;
	}

	ఒకవేళ(slen == 0){
		rc = handle_zero_నిడివి_clientid(context, clientid, allow_zero_నిడివి_clientid, clean_ప్రారంభం);
		ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
			తిరిగి rc;
		}
	}

	rc = check_clientid_prefixes(context, *clientid);
	ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
		తిరిగి rc;
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం సమితి_username_from_packet(నిర్మాణం mosquitto *context, అక్షరం **username, స్థిరరాశి అక్షరం *clientid)
{
	పూర్ణాంకం rc;

	rc = packet__read_string(&context->లోపల_packet, username, &(uint16_t){0});
	ఒకవేళ(rc == MOSQ_ERR_NOMEM){
		తిరిగి MOSQ_ERR_NOMEM;
	}
	ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
		ఒకవేళ(context->protocol == mosq_p_mqtt31){
			/* Username flag given, but no username. Ignore. */
			/* NOTE: Removed setting of username_flag to zero as it is unused afterwards */
		}లేకపోతే{
			log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with username flag but no username.",
					clientid);
			తిరిగి MOSQ_ERR_PROTOCOL;
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం సమితి_password_from_packet(నిర్మాణం mosquitto *context, అక్షరం **password, uint16_t *password_len, స్థిరరాశి అక్షరం *clientid)
{
	పూర్ణాంకం rc;

	rc = packet_read_ద్విమాన(&context->లోపల_packet, (uint8_t **)password, password_len);
	ఒకవేళ(rc == MOSQ_ERR_NOMEM){
		తిరిగి MOSQ_ERR_NOMEM;
	}

	ఒకవేళ(rc == MOSQ_ERR_MALFORMED_PACKET){
		ఒకవేళ(context->protocol == mosq_p_mqtt31){
			/* Password flag given, but no password. Ignore. */
		}లేకపోతే{
			log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with password flag but no password.",
					clientid);
			తిరిగి MOSQ_ERR_PROTOCOL;
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం read_and_verify_client_credentials_from_packet(నిర్మాణం mosquitto *context,
		అక్షరం **username, uint8_t username_flag,
		అక్షరం **password, uint16_t *password_len, uint8_t password_flag,
		స్థిరరాశి అక్షరం *clientid)
{
	పూర్ణాంకం rc;

	ఒకవేళ(username_flag){
		rc = సమితి_username_from_packet(context, username, clientid);
		ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
			తిరిగి rc;
		}
	}లేకపోతే{
		ఒకవేళ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
			ఒకవేళ(password_flag){
				/* username_flag == 0 && password_flag == 1 is forbidden */
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Protocol error from %s: password without username, closing connection.", clientid);
				తిరిగి MOSQ_ERR_PROTOCOL;
			}
		}
	}
	ఒకవేళ(password_flag){
		rc = సమితి_password_from_packet(context, password, password_len, clientid);
		ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
			తిరిగి rc;
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం check_additional_trailing_దత్తాంశం(నిర్మాణం mosquitto *context, uint8_t protocol_సంస్కరణ)
{
	ఒకవేళ(context->లోపల_packet.pos != context->లోపల_packet.మిగిలిన_నిడివి){
		/* Surplus data at end of packet, this must be an error. */
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->లోపల_packet.pos, context->లోపల_packet.మిగిలిన_నిడివి);
		ఒకవేళ(protocol_సంస్కరణ == PROTOCOL_సంస్కరణ_v5){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, ఖాళీ);
		}
		తిరిగి MOSQ_ERR_PROTOCOL;
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}

#నిర్వచితమైతే WITH_TLS


అంతఃవరుస స్థిర పూర్ణాంకం get_client_cert_and_subject_name(నిర్మాణం mosquitto *context, X509 **client_cert, X509_NAME **name)
{
	*client_cert = SSL_get_peer_certificate(context->ssl);
	ఒకవేళ(*client_cert == ఖాళీ){
		తిరిగి send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	*name = X509_get_subject_name(*client_cert);
	ఒకవేళ(*name == ఖాళీ){
		X509_free(*client_cert);
		తిరిగి send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


అంతఃవరుస స్థిర పూర్ణాంకం free_x509_and_send_connack_error(నిర్మాణం mosquitto *context, X509 *client_cert, పూర్ణాంకం rc)
{
	X509_free(client_cert);
	తిరిగి send__connack_bad_username_or_password_error(context, rc);
}


అంతఃవరుస స్థిర పూర్ణాంకం free_x509_and_BIO_and_send_connack_error(నిర్మాణం mosquitto *context, X509 *client_cert,
		BIO *subject_name, పూర్ణాంకం rc)
{
	BIO_free(subject_name);
	తిరిగి free_x509_and_send_connack_error(context, client_cert, rc);
}


స్థిర పూర్ణాంకం సమితి_username_from_cert_తటస్థం(నిర్మాణం mosquitto *context)
{
	X509 *client_cert = ఖాళీ;
	X509_NAME *name = ఖాళీ;

	ఒకవేళ(get_client_cert_and_subject_name(context, &client_cert, &name)){
		తిరిగి MOSQ_ERR_AUTH;
	}

	పూర్ణాంకం i = -1;
	X509_NAME_నమోదు *name_నమోదు = ఖాళీ;

	i = X509_NAME_get_సూచీ_by_NID(name, NID_commonName, -1);
	ఒకవేళ(i == -1){
		తిరిగి free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	name_నమోదు = X509_NAME_get_నమోదు(name, i);
	ఒకవేళ(!name_నమోదు){
		వెళ్ళు success;
	}

	ASN1_STRING *name_asn1 = ఖాళీ;
	name_asn1 = X509_NAME_నమోదు_get_దత్తాంశం(name_నమోదు);
	ఒకవేళ(name_asn1 == ఖాళీ){
		తిరిగి free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	స్థిరరాశి అక్షరం *cert_తటస్థం = (స్థిరరాశి అక్షరం *)ASN1_STRING_get0_దత్తాంశం(name_asn1);
	ఒకవేళ(!cert_తటస్థం || mosquitto_validate_utf8(cert_తటస్థం, (పూర్ణాంకం)అక్షరశ్రేణి_పొడవు(cert_తటస్థం))){
		తిరిగి free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	mosquitto_free(context->username);
	context->username = mosquitto_strdup(cert_తటస్థం);
	ఒకవేళ(context->username == ఖాళీ){
		తిరిగి free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_NOMEM);
	}
	/* Make sure there isn't an embedded NUL character in the CN */
	ఒకవేళ((size_t)ASN1_STRING_నిడివి(name_asn1) != అక్షరశ్రేణి_పొడవు(context->username)){
		తిరిగి free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}

success:
	X509_free(client_cert);
	client_cert = ఖాళీ;
	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం సమితి_username_from_cert_subject_name(నిర్మాణం mosquitto *context)
{
	X509 *client_cert = ఖాళీ;
	X509_NAME *name = ఖాళీ;

	ఒకవేళ(get_client_cert_and_subject_name(context, &client_cert, &name)){
		తిరిగి MOSQ_ERR_AUTH;
	}

	అక్షరం *subject = ఖాళీ;
	అక్షరం *దత్తాంశం_ప్రారంభం = ఖాళీ;
	BIO *subject_bio = ఖాళీ;
	దీర్ఘ name_నిడివి = 0;

	subject_bio = BIO_new(BIO_s_mem());
	X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
	దత్తాంశం_ప్రారంభం = ఖాళీ;
	name_నిడివి = BIO_get_mem_దత్తాంశం(subject_bio, &దత్తాంశం_ప్రారంభం);
	subject = mosquitto_malloc(పరిమాణం(అక్షరం)*(size_t)(name_నిడివి+1));
	ఒకవేళ(!subject){
		తిరిగి free_x509_and_BIO_and_send_connack_error(context, client_cert, subject_bio, MOSQ_ERR_NOMEM);
	}
	జ్ఞాపకం_నకలు(subject, దత్తాంశం_ప్రారంభం, (size_t)name_నిడివి);
	subject[name_నిడివి] = '\0';
	BIO_free(subject_bio);

	ఒకవేళ(mosquitto_validate_utf8(subject, (పూర్ణాంకం)అక్షరశ్రేణి_పొడవు(subject))){
		mosquitto_free(subject);
		తిరిగి free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	context->username = subject;
	ఒకవేళ(!context->username){
		X509_free(client_cert);
		తిరిగి MOSQ_ERR_AUTH;
	}

	X509_free(client_cert);
	client_cert = ఖాళీ;
	తిరిగి MOSQ_ERR_SUCCESS;
}
#అయితే_ముగింపు


స్థిర పూర్ణాంకం handle_username_from_cert_options(నిర్మాణం mosquitto *context, అక్షరం **username, అక్షరం **password, uint16_t password_len)
{
	పూర్ణాంకం rc;

#నిర్వచితమైతే WITH_TLS
	ఒకవేళ(context->listener->ssl_ctx && (context->listener->use_తటస్థం_as_username || context->listener->use_subject_as_username)){
		/* Don't need the username or password if provided */
		mosquitto_FREE(*username);
		mosquitto_FREE(*password);

		ఒకవేళ(!context->ssl){
			తిరిగి send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
		}
#నిర్వచితమైతే FINAL_WITH_TLS_PSK
		ఒకవేళ(context->listener->psk_hint){
			/* Client should have provided an identity to get this far. */
			ఒకవేళ(!context->username){
				తిరిగి send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}లేకపోతే
#అయితే_ముగింపు /* FINAL_WITH_TLS_PSK */
		{
			ఒకవేళ(context->listener->use_తటస్థం_as_username){
				rc = సమితి_username_from_cert_తటస్థం(context);
			}లేకపోతే{   /* use_subject_as_username */
				rc = సమితి_username_from_cert_subject_name(context);
			}
			ఒకవేళ(rc){
				తిరిగి rc;
			}
		}
	}లేకపోతే
#అయితే_ముగింపు /* WITH_TLS */
	{
#నిర్వచితమైతే WITH_TLS
		ఒకవేళ(context->listener->use_తటస్థం_as_username && context->listener->require_certificate){
			mosquitto_FREE(*username);
			mosquitto_FREE(*password);

			ఒకవేళ(!context->username){
				తిరిగి send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}లేకపోతే
#అయితే_ముగింపు
		{
			/* FIXME - these ensure the mosquitto_clientid() and
			* mosquitto_client_username() functions work, but is hacky */
			context->username = *username;
			context->password = *password;
			context->password_len = password_len;
			*username = ఖాళీ; /* Avoid free() in error: below. */
			*password = ఖాళీ;
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం handle_username_as_clientid_option(నిర్మాణం mosquitto *context)
{
	ఒకవేళ(context->username){
		mosquitto_FREE(context->id);
		context->id = mosquitto_strdup(context->username);
		ఒకవేళ(!context->id){
			తిరిగి MOSQ_ERR_NOMEM;
		}
	}లేకపోతే{
		uint8_t err_సంకేతం = context->protocol == mosq_p_mqtt5
												? (uint8_t)MQTT_RC_NOT_AUTHORIZED
												: (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
		తిరిగి send__connack_error_and_return(context, err_సంకేతం, MOSQ_ERR_AUTH);
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం handle__connect(నిర్మాణం mosquitto *context)
{
	అక్షరం protocol_name[7];
	uint8_t protocol_సంస్కరణ;
	uint8_t connect_flags;
	అక్షరం *clientid = ఖాళీ;
	నిర్మాణం mosquitto *దొరికింది_context;
	నిర్మాణం mosquitto_సందేశం_all *will_struct = ఖాళీ;
	uint8_t will, will_retain, will_qos, clean_ప్రారంభం;
	uint8_t username_flag, password_flag;
	అక్షరం *username = ఖాళీ, *password = ఖాళీ;
	uint16_t password_len = 0;
	పూర్ణాంకం rc;
	mosquitto_property *properties = ఖాళీ;
	శూన్య *auth_దత్తాంశం = ఖాళీ;
	uint16_t auth_దత్తాంశం_len = 0;
	శూన్య *auth_దత్తాంశం_బహిర్గతం = ఖాళీ;
	uint16_t auth_దత్తాంశం_బహిర్గతం_len = 0;
	bool allow_zero_నిడివి_clientid;

	ఒకవేళ(!context->listener){
		తిరిగి MOSQ_ERR_INVAL;
	}

	/* Don't accept multiple CONNECT commands. */
	ఒకవేళ(context->state != mosq_cs_new){
		log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Bad client %s:%d sending multiple CONNECT messages.",
				context->చిరునామా, context->remote_పోర్టు);
		rc = MOSQ_ERR_PROTOCOL;
		వెళ్ళు handle_connect_error;
	}

#నిర్వచితమైతే WITH_TLS
	ఒకవేళ(context->లోపల_packet.command == 0x16 && context->listener->ssl_ctx == ఖాళీ){ /* 0x16 is TLS handshake client hello */
		log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client from %s:%d appears to be using TLS to connect to a non-TLS listener.",
				context->చిరునామా, context->remote_పోర్టు);
		rc = MOSQ_ERR_PROTOCOL;
		వెళ్ళు handle_connect_error;
	}
#అయితే_ముగింపు

	rc = read_protocol_name(context, protocol_name);
	ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
		వెళ్ళు handle_connect_error;
	}

	rc = read_and_verify_protocol_సంస్కరణ(context, protocol_name, &protocol_సంస్కరణ);
	ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
		ఒకవేళ(rc == MOSQ_ERR_MALFORMED_PACKET){
			తిరిగి rc;
		}
		వెళ్ళు handle_connect_error;
	}

	rc = read_and_verify_connect_flags(context, &connect_flags);
	ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
		వెళ్ళు handle_connect_error;
	}

	clean_ప్రారంభం = (connect_flags & 0x02) >> 1;
	సమితి_session_expiry_interval(context, clean_ప్రారంభం, protocol_సంస్కరణ);

	rc = read_and_reset_keepalive(context);
	ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
		వెళ్ళు handle_connect_error;
	}

	rc = read_and_verify_v5_connect_properties(context, &properties, protocol_సంస్కరణ);
	ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
		వెళ్ళు handle_connect_error;
	}

	will = connect_flags & 0x04;
	will_qos = (connect_flags & 0x18) >> 3;
	will_retain = ((connect_flags & 0x20) == 0x20);
	rc = verify_will_options(context, will, will_qos, will_retain, protocol_సంస్కరణ);
	ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
		వెళ్ళు handle_connect_error;
	}

	mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_పద్ధతి, &context->auth_పద్ధతి, false);
	mosquitto_property_read_ద్విమాన(properties, MQTT_PROP_AUTHENTICATION_దత్తాంశం, &auth_దత్తాంశం, &auth_దత్తాంశం_len, false);
	mosquitto_property_free_all(&properties);

	ఒకవేళ(auth_దత్తాంశం && !context->auth_పద్ధతి){
		send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, ఖాళీ);
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s:%d CONNECT with missing clientid string.",
				context->చిరునామా, context->remote_పోర్టు);
		rc = MOSQ_ERR_PROTOCOL;
		వెళ్ళు handle_connect_error;
	}

	rc = read_and_verify_clientid_from_packet(context, &clientid, &allow_zero_నిడివి_clientid, clean_ప్రారంభం);
	ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
		వెళ్ళు handle_connect_error;
	}

	ఒకవేళ(will){
		rc = will__read(context, clientid, &will_struct, will_qos, will_retain);
		ఒకవేళ(rc){
			ఒకవేళ(context->protocol == mosq_p_mqtt5){
				ఒకవేళ(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
					send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, ఖాళీ);
				}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_MALFORMED_PACKET){
					send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, ఖాళీ);
				}
			}
			వెళ్ళు handle_connect_error;
		}
	}లేకపోతే{
		ఒకవేళ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
			ఒకవేళ(will_qos != 0 || will_retain != 0){
				log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT without Will with non-zero QoS (%d) or retain (%d).",
						clientid, will_qos, will_retain);
				rc = MOSQ_ERR_PROTOCOL;
				వెళ్ళు handle_connect_error;
			}
		}
	}

	// Client credentials
	password_flag = connect_flags & 0x40;
	username_flag = connect_flags & 0x80;
	rc = read_and_verify_client_credentials_from_packet(context, &username, username_flag, &password, &password_len, password_flag, clientid);
	ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
		వెళ్ళు handle_connect_error;
	}

	rc = check_additional_trailing_దత్తాంశం(context, protocol_సంస్కరణ);
	ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
		వెళ్ళు handle_connect_error;
	}

	/* Once context->id is set, if we return from this function with an error
	 * we must make sure that context->id is freed and set to NULL, so that the
	 * client isn't erroneously removed from the by_id hash table. */
	context->id = clientid;
	clientid = ఖాళీ;

	/* use_identity_as_username or use_subject_as_username */
	rc = handle_username_from_cert_options(context, &username, &password, password_len);
	ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
		వెళ్ళు handle_connect_error;
	}

	/* use_username_as_clientid */
	ఒకవేళ(context->listener->use_username_as_clientid){
		rc = handle_username_as_clientid_option(context);
		ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
			వెళ్ళు handle_connect_error;
		}
	}

	/* Check for an existing delayed auth check, reject if present */
	HASH_కనుగొను(hh_id, db.contexts_by_id_delayed_auth, context->id, అక్షరశ్రేణి_పొడవు(context->id), దొరికింది_context);
	ఒకవేళ(దొరికింది_context){
		rc = MOSQ_ERR_UNKNOWN;
		వెళ్ళు handle_connect_error;
	}

	context->clean_ప్రారంభం = clean_ప్రారంభం;
	context->will = will_struct;
	will_struct = ఖాళీ;

	ఒకవేళ(context->auth_పద్ధతి){
		rc = mosquitto_security_auth_ప్రారంభం(context, false, auth_దత్తాంశం, auth_దత్తాంశం_len, &auth_దత్తాంశం_బహిర్గతం, &auth_దత్తాంశం_బహిర్గతం_len);
		mosquitto_FREE(auth_దత్తాంశం);
		ఒకవేళ(rc == MOSQ_ERR_SUCCESS){
			తిరిగి connect__on_authorised(context, auth_దత్తాంశం_బహిర్గతం, auth_దత్తాంశం_బహిర్గతం_len);
		}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_AUTH_CONTINUE){
			mosquitto_సమితి_state(context, mosq_cs_authenticating);
			rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_దత్తాంశం_బహిర్గతం, auth_దత్తాంశం_బహిర్గతం_len);
			సురక్షిత_FREE(auth_దత్తాంశం_బహిర్గతం);
			తిరిగి rc;
		}లేకపోతే{
			సురక్షిత_FREE(auth_దత్తాంశం_బహిర్గతం);
			will__clear(context);
			ఒకవేళ(rc == MOSQ_ERR_AUTH){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ఖాళీ);
				mosquitto_FREE(context->id);
				వెళ్ళు handle_connect_error;
			}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_NOT_SUPPORTED){
				/* Client has requested extended authentication, but we don't support it. */
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_పద్ధతి, ఖాళీ);
				mosquitto_FREE(context->id);
				వెళ్ళు handle_connect_error;
			}లేకపోతే{
				mosquitto_FREE(context->id);
				వెళ్ళు handle_connect_error;
			}
		}
	}లేకపోతే{
#నిర్వచితమైతే WITH_TLS
		ఒకవేళ(context->listener->ssl_ctx && (context->listener->use_తటస్థం_as_username || context->listener->use_subject_as_username)){
			/* Authentication assumed to be cleared */
		}లేకపోతే
#అయితే_ముగింపు
		{
			rc = mosquitto_basic_auth(context);
			ఎంపిక(rc){
				స్థితి MOSQ_ERR_SUCCESS:
					ఆపు;
				స్థితి MOSQ_ERR_AUTH_DELAYED:
					mosquitto_సమితి_state(context, mosq_cs_delayed_auth);
					HASH_కూడు_KEYPTR(hh_id, db.contexts_by_id_delayed_auth, context->id, అక్షరశ్రేణి_పొడవు(context->id), context);
					తిరిగి MOSQ_ERR_SUCCESS;
					ఆపు;
				స్థితి MOSQ_ERR_AUTH:
					ఒకవేళ(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ఖాళీ);
					}లేకపోతే{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ఖాళీ);
					}
					వెళ్ళు handle_connect_error;
					ఆపు;
				స్థితి MOSQ_ERR_UNSPECIFIED:
				స్థితి MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
				స్థితి MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
				స్థితి MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
				స్థితి MOSQ_ERR_SERVER_UNAVAILABLE:
				స్థితి MOSQ_ERR_SERVER_BUSY:
				స్థితి MOSQ_ERR_BANNED:
				స్థితి MOSQ_ERR_BAD_AUTHENTICATION_పద్ధతి:
				స్థితి MOSQ_ERR_CONNECTION_రేటు_EXCEEDED:
					ఒకవేళ(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, (uint8_t)rc, ఖాళీ);
					}లేకపోతే{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ఖాళీ);
					}
					వెళ్ళు handle_connect_error;
					ఆపు;
				అప్రమేయ:
					rc = MOSQ_ERR_UNKNOWN;
					వెళ్ళు handle_connect_error;
					ఆపు;
			}
		}
		తిరిగి connect__on_authorised(context, ఖాళీ, 0);
	}


handle_connect_error:
	mosquitto_property_free_all(&properties);
	mosquitto_FREE(auth_దత్తాంశం);
	mosquitto_FREE(clientid);
	mosquitto_FREE(username);
	mosquitto_FREE(password);
	ఒకవేళ(will_struct){
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct->సందేశం.payload);
		mosquitto_FREE(will_struct->సందేశం.topic);
		mosquitto_FREE(will_struct);
	}
	will__clear(context);
	/* We return an error here which means the client is freed later on. */
	context->clean_ప్రారంభం = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_తక్షణ;
	context->will_delay_interval = 0;
	తిరిగి rc;
}
