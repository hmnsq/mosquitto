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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <utlist.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "packet_mosq.h"
#ಅನ್ತರ್ಭಾವ "property_mosq.h"
#ಅನ್ತರ್ಭಾವ "send_mosq.h"
#ಅನ್ತರ್ಭಾವ "sys_tree.h"
#ಅನ್ತರ್ಭಾವ "tls_mosq.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"
#ಅನ್ತರ್ಭಾವ "will_mosq.h"

#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#ಆದರೆ_ಅಂತ್ಯ


ಸ್ಥಿರ ಅಕ್ಷರ nibble_ಗಮ್ಯ_hex(uint8_t ಮೌಲ್ಯ)
{
	ಆದರೆ(ಮೌಲ್ಯ < 0x0A){
		ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ)('0'+ಮೌಲ್ಯ);
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ)(65 /*'A'*/ +ಮೌಲ್ಯ-10);
	}
}


ಸ್ಥಿರ ಅಕ್ಷರ *clientid_gen(uint16_t *idlen, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ, uint16_t auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ_len)
{
	ಅಕ್ಷರ *clientid;
	uint8_t rnd[16];
	ಪೂರ್ಣಾಂಕ pos;

	ಆದರೆ(mosquitto_getrandom(rnd, 16)){
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	*idlen = (uint16_t)(auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ_len + 36);

	clientid = (ಅಕ್ಷರ *)mosquitto_calloc((size_t)(*idlen) + 1, ಗಾತ್ರ(ಅಕ್ಷರ));
	ಆದರೆ(!clientid){
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	ಆದರೆ(auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ){
		ಸ್ಮರಣೆ_ನಕಲು(clientid, auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ, auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ_len);
	}

	pos = 0;
	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<16; i++){
		clientid[auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ_len + pos + 0] = nibble_ಗಮ್ಯ_hex(rnd[i] & 0x0F);
		clientid[auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ_len + pos + 1] = nibble_ಗಮ್ಯ_hex((rnd[i] >> 4) & 0x0F);
		pos += 2;
		ಆದರೆ(pos == 8 || pos == 13 || pos == 18 || pos == 23){
			clientid[auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ_len + pos] = '-';
			pos++;
		}
	}

	ಹಿಂತಿರುಗಿ clientid;
}


ಪೂರ್ಣಾಂಕ connect__on_authorised(ರಚನೆ mosquitto *context, ಶೂನ್ಯ *auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು, uint16_t auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len)
{
	ರಚನೆ mosquitto *ಸಿಕ್ಕಿತು_context;
	ರಚನೆ mosquitto__subleaf *ಎಲೆ;
	mosquitto_property *connack_props = ಖಾಲಿ;
	uint8_t connect_ack = 0;
	ಪೂರ್ಣಾಂಕ rc;
	ಪೂರ್ಣಾಂಕ ಒಳ_quota, ಹೊರಹೋಗು_quota;
	uint16_t ಒಳ_ಗರಿಷ್ಠ, ಹೊರಹೋಗು_ಗರಿಷ್ಠ;

	/* Find if this client already has an entry. This must be done *after* any security checks. */
	HASH_ಕಂಡುಹಿಡಿ(hh_id, db.contexts_by_id, context->id, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(context->id), ಸಿಕ್ಕಿತು_context);
	ಆದರೆ(ಸಿಕ್ಕಿತು_context){
		/* Found a matching client */
		ಆದರೆ(!net__is_connected(ಸಿಕ್ಕಿತು_context)){
			/* Client is reconnecting after a disconnect */
			/* FIXME - does anything need to be done here? */
		}ಇಲ್ಲದಿದ್ದರೆ{
			/* Client is already connected, disconnect old version. This is
			 * done in context__cleanup() below. */
		}

		ಆದರೆ(context->clean_ಆರಂಭ == true){
			sub__clean_session(ಸಿಕ್ಕಿತು_context);
			ಸಿಕ್ಕಿತು_context->session_expiry_interval = MQTT_SESSION_EXPIRY_ತಕ್ಷಣ;
			plugin_persist__handle_client_delete(ಸಿಕ್ಕಿತು_context);
		}
		context->is_persisted = ಸಿಕ್ಕಿತು_context->is_persisted;
		ಸಿಕ್ಕಿತು_context->is_persisted = false; /* stops persistence for context being removed */

		ಆದರೆ(context->clean_ಆರಂಭ == false && ಸಿಕ್ಕಿತು_context->session_expiry_interval != MQTT_SESSION_EXPIRY_ತಕ್ಷಣ){
			ಆದರೆ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
				connect_ack |= 0x01;
			}

			ಆದರೆ(ಸಿಕ್ಕಿತು_context->msgs_ಒಳ.inflight || ಸಿಕ್ಕಿತು_context->msgs_ಒಳ.queued
					|| ಸಿಕ್ಕಿತು_context->msgs_ಹೊರಹೋಗು.inflight || ಸಿಕ್ಕಿತು_context->msgs_ಹೊರಹೋಗು.queued){

				ಒಳ_quota = context->msgs_ಒಳ.inflight_quota;
				ಹೊರಹೋಗು_quota = context->msgs_ಹೊರಹೋಗು.inflight_quota;
				ಒಳ_ಗರಿಷ್ಠ = context->msgs_ಒಳ.inflight_ಗರಿಷ್ಠ;
				ಹೊರಹೋಗು_ಗರಿಷ್ಠ = context->msgs_ಹೊರಹೋಗು.inflight_ಗರಿಷ್ಠ;

				ಸ್ಮರಣೆ_ನಕಲು(&context->msgs_ಒಳ, &ಸಿಕ್ಕಿತು_context->msgs_ಒಳ, ಗಾತ್ರ(ರಚನೆ mosquitto_ಸಂದೇಶ_ದತ್ತಾಂಶ));
				ಸ್ಮರಣೆ_ನಕಲು(&context->msgs_ಹೊರಹೋಗು, &ಸಿಕ್ಕಿತು_context->msgs_ಹೊರಹೋಗು, ಗಾತ್ರ(ರಚನೆ mosquitto_ಸಂದೇಶ_ದತ್ತಾಂಶ));
				context->ಕೊನೆಯ_cmsg_id = ಸಿಕ್ಕಿತು_context->ಕೊನೆಯ_cmsg_id;

				memset(&ಸಿಕ್ಕಿತು_context->msgs_ಒಳ, 0, ಗಾತ್ರ(ರಚನೆ mosquitto_ಸಂದೇಶ_ದತ್ತಾಂಶ));
				memset(&ಸಿಕ್ಕಿತು_context->msgs_ಹೊರಹೋಗು, 0, ಗಾತ್ರ(ರಚನೆ mosquitto_ಸಂದೇಶ_ದತ್ತಾಂಶ));

				context->msgs_ಒಳ.inflight_quota = ಒಳ_quota;
				context->msgs_ಹೊರಹೋಗು.inflight_quota = ಹೊರಹೋಗು_quota;
				context->msgs_ಒಳ.inflight_ಗರಿಷ್ಠ = ಒಳ_ಗರಿಷ್ಠ;
				context->msgs_ಹೊರಹೋಗು.inflight_ಗರಿಷ್ಠ = ಹೊರಹೋಗು_ಗರಿಷ್ಠ;

				db_ಸಂದೇಶ_reconnect_reset(context);
			}
			context->subs = ಸಿಕ್ಕಿತು_context->subs;
			ಸಿಕ್ಕಿತು_context->subs = ಖಾಲಿ;
			context->subs_ಸಾಮರ್ಥ್ಯ = ಸಿಕ್ಕಿತು_context->subs_ಸಾಮರ್ಥ್ಯ;
			context->subs_ಎಣಿಕೆ = ಸಿಕ್ಕಿತು_context->subs_ಎಣಿಕೆ;
			ಸಿಕ್ಕಿತು_context->subs_ಸಾಮರ್ಥ್ಯ = 0;
			ಸಿಕ್ಕಿತು_context->subs_ಎಣಿಕೆ = 0;
			context->ಕೊನೆಯ_mid = ಸಿಕ್ಕಿತು_context->ಕೊನೆಯ_mid;

			ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<context->subs_ಸಾಮರ್ಥ್ಯ; i++){
				ಆದರೆ(context->subs[i]){
					ಎಲೆ = context->subs[i]->hier->subs;
					ಆಗ(ಎಲೆ){
						ಆದರೆ(ಎಲೆ->context == ಸಿಕ್ಕಿತು_context){
							ಎಲೆ->context = context;
						}
						ಎಲೆ = ಎಲೆ->ಮುಂದಿನ;
					}

					ಆದರೆ(context->subs[i]->shared){
						ಎಲೆ = context->subs[i]->shared->subs;
						ಆಗ(ಎಲೆ){
							ಆದರೆ(ಎಲೆ->context == ಸಿಕ್ಕಿತು_context){
								ಎಲೆ->context = context;
							}
							ಎಲೆ = ಎಲೆ->ಮುಂದಿನ;
						}
					}
				}
			}
		}

		ಆದರೆ((ಸಿಕ್ಕಿತು_context->protocol == mosq_p_mqtt5 && ಸಿಕ್ಕಿತು_context->session_expiry_interval == MQTT_SESSION_EXPIRY_ತಕ್ಷಣ)
				|| (ಸಿಕ್ಕಿತು_context->protocol != mosq_p_mqtt5 && ಸಿಕ್ಕಿತು_context->clean_ಆರಂಭ == true)
				|| (context->clean_ಆರಂಭ == true)
				){

			context__send_will(ಸಿಕ್ಕಿತು_context);
		}

		session_expiry__remove(ಸಿಕ್ಕಿತು_context);
		will_delay__remove(ಸಿಕ್ಕಿತು_context);
		will__clear(ಸಿಕ್ಕಿತು_context);

		ಸಿಕ್ಕಿತು_context->clean_ಆರಂಭ = true;
		ಸಿಕ್ಕಿತು_context->session_expiry_interval = MQTT_SESSION_EXPIRY_ತಕ್ಷಣ;
		mosquitto_ಗಣ_state(ಸಿಕ್ಕಿತು_context, mosq_cs_duplicate);
		ಆದರೆ(ಸಿಕ್ಕಿತು_context->protocol == mosq_p_mqtt5){
			send__disconnect(ಸಿಕ್ಕಿತು_context, MQTT_RC_SESSION_ತೆಗೆದುಕೊಂಡ_OVER, ಖಾಲಿ);
		}
		do_disconnect(ಸಿಕ್ಕಿತು_context, MOSQ_ERR_SESSION_ತೆಗೆದುಕೊಂಡ_OVER);
	}

	ಆದರೆ(db.config->global_max_clients > 0 && HASH_CNT(hh_id, db.contexts_by_id) >= (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)db.config->global_max_clients){
		ಆದರೆ(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_SERVER_BUSY, ಖಾಲಿ);
		}
		rc = MOSQ_ERR_INVAL;
		ಹೋಗು error;
	}

	ಆದರೆ(db.config->connection_messages == true){
		ಆದರೆ(context->is_bridge){
			ಆದರೆ(context->username){
				log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್, context->id, context->protocol, context->clean_ಆರಂಭ, context->keepalive, context->username);
			}ಇಲ್ಲದಿದ್ದರೆ{
				log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d).",
						context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್, context->id, context->protocol, context->clean_ಆರಂಭ, context->keepalive);
			}
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಆದರೆ(context->username){
				log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್, context->id, context->protocol, context->clean_ಆರಂಭ, context->keepalive, context->username);
			}ಇಲ್ಲದಿದ್ದರೆ{
				log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d).",
						context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್, context->id, context->protocol, context->clean_ಆರಂಭ, context->keepalive);
			}
		}

		ಆದರೆ(context->will){
			log__printf(ಖಾಲಿ, MOSQ_LOG_DEBUG, "Will message specified (%ld bytes) (r%d, q%d).",
					(ದೀರ್ಘ)context->will->ಸಂದೇಶ.payloadlen,
					context->will->ಸಂದೇಶ.retain,
					context->will->ಸಂದೇಶ.qos);

			log__printf(ಖಾಲಿ, MOSQ_LOG_DEBUG, "\t%s", context->will->ಸಂದೇಶ.topic);
		}ಇಲ್ಲದಿದ್ದರೆ{
			log__printf(ಖಾಲಿ, MOSQ_LOG_DEBUG, "No will message specified.");
		}
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
	ಆದರೆ(context->ssl){
		log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s negotiated %s cipher %s",
				context->id,
				SSL_get_cipher_ಆವೃತ್ತಿ(context->ssl),
				SSL_get_cipher_name(context->ssl));
	}
#ಆದರೆ_ಅಂತ್ಯ

	context->ping_t = 0;
	context->is_dropping = false;

	/* Remove any queued messages that are no longer allowed through ACL,
	 * assuming a possible change of username. */
	db_check_acl_ದ_all_messages(context);
	context_ಕೂಡಿಸು_ಗಮ್ಯ_by_id(context);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_PERSISTENCE
	ಆದರೆ(!context->clean_ಆರಂಭ){
		db.persistence_changes++;
	}
#ಆದರೆ_ಅಂತ್ಯ
	context->max_qos = context->listener->max_qos;

	ಆದರೆ(db.config->max_keepalive &&
			(context->keepalive > db.config->max_keepalive || context->keepalive == 0)){

		keepalive__remove(context);
		context->keepalive = db.config->max_keepalive;
		keepalive_ಕೂಡಿಸು(context);
		ಆದರೆ(context->protocol == mosq_p_mqtt5){
			ಆದರೆ(mosquitto_property_ಕೂಡಿಸು_int16(&connack_props, MQTT_PROP_SERVER_KEEP_ALIVE, context->keepalive)){
				rc = MOSQ_ERR_NOMEM;
				ಹೋಗು error;
			}
		}ಇಲ್ಲದಿದ್ದರೆ{
			send__connack(context, connect_ack, CONNACK_REFUSED_IDENTIFIER_REJECTED, ಖಾಲಿ);
			rc = MOSQ_ERR_INVAL;
			ಹೋಗು error;
		}
	}


	ಆದರೆ(context->protocol == mosq_p_mqtt5){
		ಆದರೆ(context->listener->max_topic_alias > 0){
			ಆದರೆ(mosquitto_property_ಕೂಡಿಸು_int16(&connack_props, MQTT_PROP_TOPIC_ALIAS_ಗರಿಷ್ಠ, context->listener->max_topic_alias)){
				rc = MOSQ_ERR_NOMEM;
				ಹೋಗು error;
			}
		}
		ಆದರೆ(context->assigned_id){
			ಆದರೆ(mosquitto_property_ಕೂಡಿಸು_string(&connack_props, MQTT_PROP_ASSIGNED_CLIENT_IDENTIFIER, context->id)){
				rc = MOSQ_ERR_NOMEM;
				ಹೋಗು error;
			}
		}
		ಆದರೆ(context->auth_ವಿಧಾನ){
			ಆದರೆ(mosquitto_property_ಕೂಡಿಸು_string(&connack_props, MQTT_PROP_AUTHENTICATION_ವಿಧಾನ, context->auth_ವಿಧಾನ)){
				rc = MOSQ_ERR_NOMEM;
				ಹೋಗು error;
			}

			ಆದರೆ(auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು && auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len > 0){
				ಆದರೆ(mosquitto_property_ಕೂಡಿಸು_ದ್ವಿಮಾನ(&connack_props, MQTT_PROP_AUTHENTICATION_ದತ್ತಾಂಶ, auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು, auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len)){
					rc = MOSQ_ERR_NOMEM;
					ಹೋಗು error;
				}
			}
		}
	}
	ಸುರಕ್ಷಿತ_FREE(auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು);

	mosquitto_ಗಣ_state(context, mosq_cs_ಸಕ್ರಿಯ);
	rc = send__connack(context, connect_ack, CONNACK_ACCEPTED, connack_props);
	mosquitto_property_free_all(&connack_props);
	ಆದರೆ(rc){
		ಹಿಂತಿರುಗಿ rc;
	}
	db__expire_all_messages(context);
	rc = db_ಸಂದೇಶ_write_queued_ಹೊರಹೋಗು(context);
	ಆದರೆ(rc){
		ಹಿಂತಿರುಗಿ rc;
	}
	rc = db_ಸಂದೇಶ_write_inflight_ಹೊರಹೋಗು_all(context);

	ಆದರೆ(rc == MOSQ_ERR_SUCCESS){
		plugin__handle_connect(context);

		ಆದರೆ(context->session_expiry_interval != MQTT_SESSION_EXPIRY_ತಕ್ಷಣ){
			plugin_persist_handle_client_ಕೂಡಿಸು(context);
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(context->will){
			plugin_persist_handle_will_ಕೂಡಿಸು(context);
		}
	}
	ಹಿಂತಿರುಗಿ rc;
error:
	ಸುರಕ್ಷಿತ_FREE(auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು);
	mosquitto_property_free_all(&connack_props);
	ಹಿಂತಿರುಗಿ rc;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ will__read(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *clientid, ರಚನೆ mosquitto_ಸಂದೇಶ_all **will, uint8_t will_qos, ಪೂರ್ಣಾಂಕ will_retain)
{
	ಪೂರ್ಣಾಂಕ rc = MOSQ_ERR_SUCCESS;
	size_t slen;
	uint16_t tlen;
	ರಚನೆ mosquitto_ಸಂದೇಶ_all *will_struct = ಖಾಲಿ;
	ಅಕ್ಷರ *will_topic_mount = ಖಾಲಿ;
	uint16_t payloadlen;
	mosquitto_property *properties = ಖಾಲಿ;

	will_struct = mosquitto_calloc(1, ಗಾತ್ರ(ರಚನೆ mosquitto_ಸಂದೇಶ_all));
	ಆದರೆ(!will_struct){
		rc = MOSQ_ERR_NOMEM;
		ಹೋಗು error_cleanup;
	}
	ಆದರೆ(context->protocol == PROTOCOL_ಆವೃತ್ತಿ_v5){
		rc = property__read_all(CMD_WILL, &context->ಒಳ_packet, &properties);
		ಆದರೆ(rc){
			ಹೋಗು error_cleanup;
		}

		rc = property__process_will(context, will_struct, &properties);
		mosquitto_property_free_all(&properties);
		ಆದರೆ(rc){
			ಹೋಗು error_cleanup;
		}
	}
	rc = packet__read_string(&context->ಒಳ_packet, &will_struct->ಸಂದೇಶ.topic, &tlen);
	ಆದರೆ(rc){
		ಹೋಗು error_cleanup;
	}
	ಆದರೆ(!tlen){
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s: Will with empty topic.",
				context->id);
		rc = MOSQ_ERR_PROTOCOL;
		ಹೋಗು error_cleanup;
	}

	ಆದರೆ(context->listener->mount_ಬಿಂದು){
		slen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(context->listener->mount_ಬಿಂದು) + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(will_struct->ಸಂದೇಶ.topic) + 1;
		will_topic_mount = mosquitto_malloc(slen+1);
		ಆದರೆ(!will_topic_mount){
			rc = MOSQ_ERR_NOMEM;
			ಹೋಗು error_cleanup;
		}

		snprintf(will_topic_mount, slen, "%s%s", context->listener->mount_ಬಿಂದು, will_struct->ಸಂದೇಶ.topic);
		will_topic_mount[slen] = '\0';

		mosquitto_FREE(will_struct->ಸಂದೇಶ.topic);
		will_struct->ಸಂದೇಶ.topic = will_topic_mount;
	}

	ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(will_struct->ಸಂದೇಶ.topic, "$CONTROL/", ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("$CONTROL/"))){
		rc = MOSQ_ERR_ACL_DENIED;
		ಹೋಗು error_cleanup;
	}
	rc = mosquitto_pub_topic_check(will_struct->ಸಂದೇಶ.topic);
	ಆದರೆ(rc){
		ಹೋಗು error_cleanup;
	}

	rc = packet__read_uint16(&context->ಒಳ_packet, &payloadlen);
	ಆದರೆ(rc){
		ಹೋಗು error_cleanup;
	}

	will_struct->ಸಂದೇಶ.payloadlen = payloadlen;
	ಆದರೆ(will_struct->ಸಂದೇಶ.payloadlen > 0){
		ಆದರೆ(db.config->ಸಂದೇಶ_ಗಾತ್ರ_ಎಲ್ಲೆ && will_struct->ಸಂದೇಶ.payloadlen > (ಪೂರ್ಣಾಂಕ)db.config->ಸಂದೇಶ_ಗಾತ್ರ_ಎಲ್ಲೆ){
			log__printf(ಖಾಲಿ, MOSQ_LOG_DEBUG, "Client %s connected with too large Will payload", clientid);
			ಆದರೆ(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_PACKET_TOO_LARGE, ಖಾಲಿ);
			}ಇಲ್ಲದಿದ್ದರೆ{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ಖಾಲಿ);
			}
			rc = MOSQ_ERR_PAYLOAD_ಗಾತ್ರ;
			ಹೋಗು error_cleanup;
		}
		will_struct->ಸಂದೇಶ.payload = mosquitto_malloc((size_t)will_struct->ಸಂದೇಶ.payloadlen);
		ಆದರೆ(!will_struct->ಸಂದೇಶ.payload){
			rc = MOSQ_ERR_NOMEM;
			ಹೋಗು error_cleanup;
		}

		rc = packet_read_ಬೈಟುಗಳು(&context->ಒಳ_packet, will_struct->ಸಂದೇಶ.payload, (uint32_t)will_struct->ಸಂದೇಶ.payloadlen);
		ಆದರೆ(rc){
			ಹೋಗು error_cleanup;
		}
	}

	will_struct->ಸಂದೇಶ.qos = will_qos;
	will_struct->ಸಂದೇಶ.retain = will_retain;

	*will = will_struct;
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;

error_cleanup:
	ಆದರೆ(will_struct){
		mosquitto_FREE(will_struct->ಸಂದೇಶ.topic);
		mosquitto_FREE(will_struct->ಸಂದೇಶ.payload);
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct);
	}
	ಹಿಂತಿರುಗಿ rc;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ check_protocol_ಆವೃತ್ತಿ(ರಚನೆ mosquitto__listener *listener, ಪೂರ್ಣಾಂಕ protocol_ಆವೃತ್ತಿ)
{
	/* Allow bridge protocol as well. */
	protocol_ಆವೃತ್ತಿ &= 0x7F;

	ಆದರೆ((protocol_ಆವೃತ್ತಿ == 3 && listener->disable_protocol_v3 == false)
			|| (protocol_ಆವೃತ್ತಿ == 4 && listener->disable_protocol_v4 == false)
			|| (protocol_ಆವೃತ್ತಿ == 5 && listener->disable_protocol_v5 == false)
			){

		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_SUPPORTED;
	}
}


ಸಾಲೊಳಗೆ ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ send__connack_error_and_return(ರಚನೆ mosquitto *context, uint8_t err_ಸಂಕೇತ, ಪೂರ್ಣಾಂಕ rc)
{
	send__connack(context, 0, err_ಸಂಕೇತ, ಖಾಲಿ);
	ಹಿಂತಿರುಗಿ rc;
}


ಸಾಲೊಳಗೆ ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ send__connack_bad_username_or_password_error(ರಚನೆ mosquitto *context, ಪೂರ್ಣಾಂಕ rc)
{
	uint8_t err_ಸಂಕೇತ  = context->protocol == mosq_p_mqtt5
									? (uint8_t)MQTT_RC_BAD_USERNAME_OR_PASSWORD
									: (uint8_t)CONNACK_REFUSED_BAD_USERNAME_PASSWORD;
	ಹಿಂತಿರುಗಿ send__connack_error_and_return(context, err_ಸಂಕೇತ, rc);
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ read_protocol_name(ರಚನೆ mosquitto *context, ಅಕ್ಷರ protocol_name[7])
{
	/* Read protocol name as length then bytes rather than with read_string
	 * because the length is fixed and we can check that. Removes the need
	 * for another malloc as well. */

	uint16_t slen = 0;

	ಆದರೆ(packet__read_uint16(&context->ಒಳ_packet, &slen)){
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string length.",
				context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
	}
	ಆದರೆ(slen != 4 /* MQTT */ && slen != 6 /* MQIsdp */){
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with incorrect protocol string length (%d).",
				context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್, slen);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
	}
	ಆದರೆ(packet_read_ಬೈಟುಗಳು(&context->ಒಳ_packet, protocol_name, slen)){
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string.",
				context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
	}
	protocol_name[slen] = '\0';

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ read_and_verify_protocol_ಆವೃತ್ತಿ(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *protocol_name,
		uint8_t *protocol_ಆವೃತ್ತಿ)
{
	uint8_t tmp_protocol_ಆವೃತ್ತಿ = 0;
	ಆದರೆ(packet__read_byte(&context->ಒಳ_packet, &tmp_protocol_ಆವೃತ್ತಿ)){
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol version.",
				context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
	}

	ಆದರೆ(check_protocol_ಆವೃತ್ತಿ(context->listener, tmp_protocol_ಆವೃತ್ತಿ)){
		ಆದರೆ(tmp_protocol_ಆವೃತ್ತಿ == 3 || tmp_protocol_ಆವೃತ್ತಿ == 4){
			context->protocol = mosq_p_mqtt311;
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_ಆವೃತ್ತಿ, ಖಾಲಿ);
		}ಇಲ್ಲದಿದ್ದರೆ{
			context->protocol = mosq_p_mqtt5;
			send__connack(context, 0, MQTT_RC_UNSUPPORTED_PROTOCOL_ಆವೃತ್ತಿ, ಖಾಲಿ);
		}
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_SUPPORTED;
	}

	ಆದರೆ(!strcmp(protocol_name, PROTOCOL_NAME_v31)){
		ಆದರೆ((tmp_protocol_ಆವೃತ್ತಿ&0x7F) != PROTOCOL_ಆವೃತ್ತಿ_v31){
			ಆದರೆ(db.config->connection_messages == true){
				log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್, tmp_protocol_ಆವೃತ್ತಿ);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_ಆವೃತ್ತಿ, ಖಾಲಿ);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
		}
		context->protocol = mosq_p_mqtt31;
		ಆದರೆ((tmp_protocol_ಆವೃತ್ತಿ&0x80) == 0x80){
			context->is_bridge = true;
		}
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(protocol_name, PROTOCOL_NAME)){
		ಆದರೆ((tmp_protocol_ಆವೃತ್ತಿ&0x7F) == PROTOCOL_ಆವೃತ್ತಿ_v311){
			context->protocol = mosq_p_mqtt311;

			ಆದರೆ((tmp_protocol_ಆವೃತ್ತಿ&0x80) == 0x80){
				context->is_bridge = true;
			}
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((tmp_protocol_ಆವೃತ್ತಿ&0x7F) == PROTOCOL_ಆವೃತ್ತಿ_v5){
			context->protocol = mosq_p_mqtt5;
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಆದರೆ(db.config->connection_messages == true){
				log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್, tmp_protocol_ಆವೃತ್ತಿ);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_ಆವೃತ್ತಿ, ಖಾಲಿ);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
		}
		ಆದರೆ((context->ಒಳ_packet.command&0x0F) != 0x00){
			/* Reserved flags not set to 0, must disconnect. */
			log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero reserved flags (%02X).",
					context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್, context->ಒಳ_packet.command);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಆದರೆ(db.config->connection_messages == true){
			log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol \"%s\".",
					context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್, protocol_name);
		}
		ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
	}
	ಆದರೆ((tmp_protocol_ಆವೃತ್ತಿ&0x7F) != PROTOCOL_ಆವೃತ್ತಿ_v31 && context->ಒಳ_packet.command != CMD_CONNECT){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
	}

	*protocol_ಆವೃತ್ತಿ = tmp_protocol_ಆವೃತ್ತಿ;
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ read_and_verify_connect_flags(ರಚನೆ mosquitto *context, uint8_t *connect_flags)
{
	ಆದರೆ(packet__read_byte(&context->ಒಳ_packet, connect_flags)){
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing connect flags.",
				context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
	}
	ಆದರೆ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		ಆದರೆ((*connect_flags & 0x01) != 0x00){
			log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero connect reserved flag (%02X).",
					context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್, *connect_flags);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_session_expiry_interval(ರಚನೆ mosquitto *context, uint8_t clean_ಆರಂಭ, uint8_t protocol_ಆವೃತ್ತಿ)
{
	/* session_expiry_interval will be overridden if the properties are read later */
	ಆದರೆ(clean_ಆರಂಭ == false && protocol_ಆವೃತ್ತಿ != PROTOCOL_ಆವೃತ್ತಿ_v5){
		/* v3* has clean_start == false mean the session never expires */
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
	}ಇಲ್ಲದಿದ್ದರೆ{
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_ತಕ್ಷಣ;
	}
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ read_and_reset_keepalive(ರಚನೆ mosquitto *context)
{
	/* _remove here because net__socket_accept() uses _add and we must have the
	 * correct keepalive value */
	keepalive__remove(context);

	ಆದರೆ(packet__read_uint16(&context->ಒಳ_packet, &(context->keepalive))){
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "%s sent CONNECT with missing keepalive.",
				context->id);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
	}
	keepalive_ಕೂಡಿಸು(context);

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ read_and_verify_v5_connect_properties(ರಚನೆ mosquitto *context, mosquitto_property **properties, uint8_t protocol_ಆವೃತ್ತಿ)
{
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ(protocol_ಆವೃತ್ತಿ == PROTOCOL_ಆವೃತ್ತಿ_v5){
		rc = property__read_all(CMD_CONNECT, &context->ಒಳ_packet, properties);
		ಆದರೆ(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
			send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, ಖಾಲಿ);
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_MALFORMED_PACKET){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, ಖಾಲಿ);
		}
		ಆದರೆ(rc){
			ಹಿಂತಿರುಗಿ rc;
		}
	}
	rc = property__process_connect(context, properties);
	ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
		ಹಿಂತಿರುಗಿ send__connack_error_and_return(context, MQTT_RC_PROTOCOL_ERROR, rc);
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ verify_will_options(ರಚನೆ mosquitto *context, uint8_t will, uint8_t will_qos, uint8_t will_retain, uint8_t protocol_ಆವೃತ್ತಿ)
{
	ಆದರೆ(will_qos == 3){
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid Will QoS (%d).",
				context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್, will_qos);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
	}

	ಆದರೆ(will && will_retain && db.config->retain_ಲಭ್ಯ == false){
		ಆದರೆ(protocol_ಆವೃತ್ತಿ == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_RETAIN_NOT_SUPPORTED, ಖಾಲಿ);
		}
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_SUPPORTED;
	}

	ಆದರೆ(will && will_qos > context->listener->max_qos){
		ಆದರೆ(protocol_ಆವೃತ್ತಿ == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_QOS_NOT_SUPPORTED, ಖಾಲಿ);
		}
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_SUPPORTED;
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ handle_zero_ಉದ್ದಳತೆ_clientid(ರಚನೆ mosquitto *context, ಅಕ್ಷರ **clientid, bool *allow_zero_ಉದ್ದಳತೆ_clientid,
		uint8_t clean_ಆರಂಭ)
{
	ಆದರೆ(context->protocol == mosq_p_mqtt31){
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s:%d: v3.1 CONNECT with zero length clientid.",
				context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
		send__connack(context, 0, CONNACK_REFUSED_IDENTIFIER_REJECTED, ಖಾಲಿ);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
	}

	/* mqtt311/mqtt5 */
	mosquitto_FREE(*clientid);

	ಆದರೆ(db.config->ಪ್ರತಿ_listener_settings){
		*allow_zero_ಉದ್ದಳತೆ_clientid = context->listener->security_options->allow_zero_ಉದ್ದಳತೆ_clientid;
	}ಇಲ್ಲದಿದ್ದರೆ{
		*allow_zero_ಉದ್ದಳತೆ_clientid = db.config->security_options.allow_zero_ಉದ್ದಳತೆ_clientid;
	}

	ಆದರೆ((context->protocol == mosq_p_mqtt311 && clean_ಆರಂಭ == 0) || *allow_zero_ಉದ್ದಳತೆ_clientid == false){
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with zero length clientid when forbidden.",
				context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
		uint8_t err_ಸಂಕೇತ = context->protocol == mosq_p_mqtt311 ? (uint8_t)CONNACK_REFUSED_IDENTIFIER_REJECTED : (uint8_t)MQTT_RC_UNSPECIFIED;
		ಹಿಂತಿರುಗಿ send__connack_error_and_return(context, err_ಸಂಕೇತ, MOSQ_ERR_PROTOCOL);
	}

	*clientid = clientid_gen(&(uint16_t){0}, context->listener->security_options->auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ,
			context->listener->security_options->auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ_len);
	ಆದರೆ(*clientid == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}
	context->assigned_id = true;

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ check_clientid_prefixes(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *clientid)
{
	ಆದರೆ(db.config->clientid_prefixes){
		ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(db.config->clientid_prefixes, clientid, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(db.config->clientid_prefixes))){
			uint8_t err_ಸಂಕೇತ = context->protocol == mosq_p_mqtt5 ? (uint8_t)MQTT_RC_NOT_AUTHORIZED : (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
			ಹಿಂತಿರುಗಿ send__connack_error_and_return(context, err_ಸಂಕೇತ, MOSQ_ERR_AUTH);
		}
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ read_and_verify_clientid_from_packet(ರಚನೆ mosquitto *context, ಅಕ್ಷರ **clientid,
		bool *allow_zero_ಉದ್ದಳತೆ_clientid, uint8_t clean_ಆರಂಭ)
{
	ಪೂರ್ಣಾಂಕ rc;
	uint16_t slen;

	rc = packet__read_string(&context->ಒಳ_packet, clientid, &slen);
	ಆದರೆ(rc == MOSQ_ERR_MALFORMED_UTF8){
		ಆದರೆ(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_CLIENTID_NOT_VALID, ಖಾಲಿ);
		}
		ಹಿಂತಿರುಗಿ MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID;
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc > 0){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
	}

	ಆದರೆ(slen == 0){
		rc = handle_zero_ಉದ್ದಳತೆ_clientid(context, clientid, allow_zero_ಉದ್ದಳತೆ_clientid, clean_ಆರಂಭ);
		ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
			ಹಿಂತಿರುಗಿ rc;
		}
	}

	rc = check_clientid_prefixes(context, *clientid);
	ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
		ಹಿಂತಿರುಗಿ rc;
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಗಣ_username_from_packet(ರಚನೆ mosquitto *context, ಅಕ್ಷರ **username, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *clientid)
{
	ಪೂರ್ಣಾಂಕ rc;

	rc = packet__read_string(&context->ಒಳ_packet, username, &(uint16_t){0});
	ಆದರೆ(rc == MOSQ_ERR_NOMEM){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}
	ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
		ಆದರೆ(context->protocol == mosq_p_mqtt31){
			/* Username flag given, but no username. Ignore. */
			/* NOTE: Removed setting of username_flag to zero as it is unused afterwards */
		}ಇಲ್ಲದಿದ್ದರೆ{
			log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with username flag but no username.",
					clientid);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಗಣ_password_from_packet(ರಚನೆ mosquitto *context, ಅಕ್ಷರ **password, uint16_t *password_len, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *clientid)
{
	ಪೂರ್ಣಾಂಕ rc;

	rc = packet_read_ದ್ವಿಮಾನ(&context->ಒಳ_packet, (uint8_t **)password, password_len);
	ಆದರೆ(rc == MOSQ_ERR_NOMEM){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}

	ಆದರೆ(rc == MOSQ_ERR_MALFORMED_PACKET){
		ಆದರೆ(context->protocol == mosq_p_mqtt31){
			/* Password flag given, but no password. Ignore. */
		}ಇಲ್ಲದಿದ್ದರೆ{
			log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with password flag but no password.",
					clientid);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ read_and_verify_client_credentials_from_packet(ರಚನೆ mosquitto *context,
		ಅಕ್ಷರ **username, uint8_t username_flag,
		ಅಕ್ಷರ **password, uint16_t *password_len, uint8_t password_flag,
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *clientid)
{
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ(username_flag){
		rc = ಗಣ_username_from_packet(context, username, clientid);
		ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
			ಹಿಂತಿರುಗಿ rc;
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಆದರೆ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
			ಆದರೆ(password_flag){
				/* username_flag == 0 && password_flag == 1 is forbidden */
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Protocol error from %s: password without username, closing connection.", clientid);
				ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
			}
		}
	}
	ಆದರೆ(password_flag){
		rc = ಗಣ_password_from_packet(context, password, password_len, clientid);
		ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
			ಹಿಂತಿರುಗಿ rc;
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ check_additional_trailing_ದತ್ತಾಂಶ(ರಚನೆ mosquitto *context, uint8_t protocol_ಆವೃತ್ತಿ)
{
	ಆದರೆ(context->ಒಳ_packet.pos != context->ಒಳ_packet.ಉಳಿದ_ಉದ್ದಳತೆ){
		/* Surplus data at end of packet, this must be an error. */
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->ಒಳ_packet.pos, context->ಒಳ_packet.ಉಳಿದ_ಉದ್ದಳತೆ);
		ಆದರೆ(protocol_ಆವೃತ್ತಿ == PROTOCOL_ಆವೃತ್ತಿ_v5){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, ಖಾಲಿ);
		}
		ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS


ಸಾಲೊಳಗೆ ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ get_client_cert_and_subject_name(ರಚನೆ mosquitto *context, X509 **client_cert, X509_NAME **name)
{
	*client_cert = SSL_get_peer_certificate(context->ssl);
	ಆದರೆ(*client_cert == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	*name = X509_get_subject_name(*client_cert);
	ಆದರೆ(*name == ಖಾಲಿ){
		X509_free(*client_cert);
		ಹಿಂತಿರುಗಿ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸಾಲೊಳಗೆ ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ free_x509_and_send_connack_error(ರಚನೆ mosquitto *context, X509 *client_cert, ಪೂರ್ಣಾಂಕ rc)
{
	X509_free(client_cert);
	ಹಿಂತಿರುಗಿ send__connack_bad_username_or_password_error(context, rc);
}


ಸಾಲೊಳಗೆ ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ free_x509_and_BIO_and_send_connack_error(ರಚನೆ mosquitto *context, X509 *client_cert,
		BIO *subject_name, ಪೂರ್ಣಾಂಕ rc)
{
	BIO_free(subject_name);
	ಹಿಂತಿರುಗಿ free_x509_and_send_connack_error(context, client_cert, rc);
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಗಣ_username_from_cert_ತಟಸ್ಥ(ರಚನೆ mosquitto *context)
{
	X509 *client_cert = ಖಾಲಿ;
	X509_NAME *name = ಖಾಲಿ;

	ಆದರೆ(get_client_cert_and_subject_name(context, &client_cert, &name)){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_AUTH;
	}

	ಪೂರ್ಣಾಂಕ i = -1;
	X509_NAME_ನಮೂದು *name_ನಮೂದು = ಖಾಲಿ;

	i = X509_NAME_get_ಸೂಚ್ಯಂಕ_by_NID(name, NID_commonName, -1);
	ಆದರೆ(i == -1){
		ಹಿಂತಿರುಗಿ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	name_ನಮೂದು = X509_NAME_get_ನಮೂದು(name, i);
	ಆದರೆ(!name_ನಮೂದು){
		ಹೋಗು success;
	}

	ASN1_STRING *name_asn1 = ಖಾಲಿ;
	name_asn1 = X509_NAME_ನಮೂದು_get_ದತ್ತಾಂಶ(name_ನಮೂದು);
	ಆದರೆ(name_asn1 == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cert_ತಟಸ್ಥ = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ASN1_STRING_get0_ದತ್ತಾಂಶ(name_asn1);
	ಆದರೆ(!cert_ತಟಸ್ಥ || mosquitto_validate_utf8(cert_ತಟಸ್ಥ, (ಪೂರ್ಣಾಂಕ)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(cert_ತಟಸ್ಥ))){
		ಹಿಂತಿರುಗಿ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	mosquitto_free(context->username);
	context->username = mosquitto_strdup(cert_ತಟಸ್ಥ);
	ಆದರೆ(context->username == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_NOMEM);
	}
	/* Make sure there isn't an embedded NUL character in the CN */
	ಆದರೆ((size_t)ASN1_STRING_ಉದ್ದಳತೆ(name_asn1) != ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(context->username)){
		ಹಿಂತಿರುಗಿ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}

success:
	X509_free(client_cert);
	client_cert = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಗಣ_username_from_cert_subject_name(ರಚನೆ mosquitto *context)
{
	X509 *client_cert = ಖಾಲಿ;
	X509_NAME *name = ಖಾಲಿ;

	ಆದರೆ(get_client_cert_and_subject_name(context, &client_cert, &name)){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_AUTH;
	}

	ಅಕ್ಷರ *subject = ಖಾಲಿ;
	ಅಕ್ಷರ *ದತ್ತಾಂಶ_ಆರಂಭ = ಖಾಲಿ;
	BIO *subject_bio = ಖಾಲಿ;
	ದೀರ್ಘ name_ಉದ್ದಳತೆ = 0;

	subject_bio = BIO_new(BIO_s_mem());
	X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
	ದತ್ತಾಂಶ_ಆರಂಭ = ಖಾಲಿ;
	name_ಉದ್ದಳತೆ = BIO_get_mem_ದತ್ತಾಂಶ(subject_bio, &ದತ್ತಾಂಶ_ಆರಂಭ);
	subject = mosquitto_malloc(ಗಾತ್ರ(ಅಕ್ಷರ)*(size_t)(name_ಉದ್ದಳತೆ+1));
	ಆದರೆ(!subject){
		ಹಿಂತಿರುಗಿ free_x509_and_BIO_and_send_connack_error(context, client_cert, subject_bio, MOSQ_ERR_NOMEM);
	}
	ಸ್ಮರಣೆ_ನಕಲು(subject, ದತ್ತಾಂಶ_ಆರಂಭ, (size_t)name_ಉದ್ದಳತೆ);
	subject[name_ಉದ್ದಳತೆ] = '\0';
	BIO_free(subject_bio);

	ಆದರೆ(mosquitto_validate_utf8(subject, (ಪೂರ್ಣಾಂಕ)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(subject))){
		mosquitto_free(subject);
		ಹಿಂತಿರುಗಿ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	context->username = subject;
	ಆದರೆ(!context->username){
		X509_free(client_cert);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_AUTH;
	}

	X509_free(client_cert);
	client_cert = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}
#ಆದರೆ_ಅಂತ್ಯ


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ handle_username_from_cert_options(ರಚನೆ mosquitto *context, ಅಕ್ಷರ **username, ಅಕ್ಷರ **password, uint16_t password_len)
{
	ಪೂರ್ಣಾಂಕ rc;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
	ಆದರೆ(context->listener->ssl_ctx && (context->listener->use_ತಟಸ್ಥ_as_username || context->listener->use_subject_as_username)){
		/* Don't need the username or password if provided */
		mosquitto_FREE(*username);
		mosquitto_FREE(*password);

		ಆದರೆ(!context->ssl){
			ಹಿಂತಿರುಗಿ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
		}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FINAL_WITH_TLS_PSK
		ಆದರೆ(context->listener->psk_hint){
			/* Client should have provided an identity to get this far. */
			ಆದರೆ(!context->username){
				ಹಿಂತಿರುಗಿ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ /* FINAL_WITH_TLS_PSK */
		{
			ಆದರೆ(context->listener->use_ತಟಸ್ಥ_as_username){
				rc = ಗಣ_username_from_cert_ತಟಸ್ಥ(context);
			}ಇಲ್ಲದಿದ್ದರೆ{   /* use_subject_as_username */
				rc = ಗಣ_username_from_cert_subject_name(context);
			}
			ಆದರೆ(rc){
				ಹಿಂತಿರುಗಿ rc;
			}
		}
	}ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ /* WITH_TLS */
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
		ಆದರೆ(context->listener->use_ತಟಸ್ಥ_as_username && context->listener->require_certificate){
			mosquitto_FREE(*username);
			mosquitto_FREE(*password);

			ಆದರೆ(!context->username){
				ಹಿಂತಿರುಗಿ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		{
			/* FIXME - these ensure the mosquitto_clientid() and
			* mosquitto_client_username() functions work, but is hacky */
			context->username = *username;
			context->password = *password;
			context->password_len = password_len;
			*username = ಖಾಲಿ; /* Avoid free() in error: below. */
			*password = ಖಾಲಿ;
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ handle_username_as_clientid_option(ರಚನೆ mosquitto *context)
{
	ಆದರೆ(context->username){
		mosquitto_FREE(context->id);
		context->id = mosquitto_strdup(context->username);
		ಆದರೆ(!context->id){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		uint8_t err_ಸಂಕೇತ = context->protocol == mosq_p_mqtt5
												? (uint8_t)MQTT_RC_NOT_AUTHORIZED
												: (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
		ಹಿಂತಿರುಗಿ send__connack_error_and_return(context, err_ಸಂಕೇತ, MOSQ_ERR_AUTH);
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ handle__connect(ರಚನೆ mosquitto *context)
{
	ಅಕ್ಷರ protocol_name[7];
	uint8_t protocol_ಆವೃತ್ತಿ;
	uint8_t connect_flags;
	ಅಕ್ಷರ *clientid = ಖಾಲಿ;
	ರಚನೆ mosquitto *ಸಿಕ್ಕಿತು_context;
	ರಚನೆ mosquitto_ಸಂದೇಶ_all *will_struct = ಖಾಲಿ;
	uint8_t will, will_retain, will_qos, clean_ಆರಂಭ;
	uint8_t username_flag, password_flag;
	ಅಕ್ಷರ *username = ಖಾಲಿ, *password = ಖಾಲಿ;
	uint16_t password_len = 0;
	ಪೂರ್ಣಾಂಕ rc;
	mosquitto_property *properties = ಖಾಲಿ;
	ಶೂನ್ಯ *auth_ದತ್ತಾಂಶ = ಖಾಲಿ;
	uint16_t auth_ದತ್ತಾಂಶ_len = 0;
	ಶೂನ್ಯ *auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು = ಖಾಲಿ;
	uint16_t auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len = 0;
	bool allow_zero_ಉದ್ದಳತೆ_clientid;

	ಆದರೆ(!context->listener){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	/* Don't accept multiple CONNECT commands. */
	ಆದರೆ(context->state != mosq_cs_new){
		log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Bad client %s:%d sending multiple CONNECT messages.",
				context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
		rc = MOSQ_ERR_PROTOCOL;
		ಹೋಗು handle_connect_error;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
	ಆದರೆ(context->ಒಳ_packet.command == 0x16 && context->listener->ssl_ctx == ಖಾಲಿ){ /* 0x16 is TLS handshake client hello */
		log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client from %s:%d appears to be using TLS to connect to a non-TLS listener.",
				context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
		rc = MOSQ_ERR_PROTOCOL;
		ಹೋಗು handle_connect_error;
	}
#ಆದರೆ_ಅಂತ್ಯ

	rc = read_protocol_name(context, protocol_name);
	ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
		ಹೋಗು handle_connect_error;
	}

	rc = read_and_verify_protocol_ಆವೃತ್ತಿ(context, protocol_name, &protocol_ಆವೃತ್ತಿ);
	ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
		ಆದರೆ(rc == MOSQ_ERR_MALFORMED_PACKET){
			ಹಿಂತಿರುಗಿ rc;
		}
		ಹೋಗು handle_connect_error;
	}

	rc = read_and_verify_connect_flags(context, &connect_flags);
	ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
		ಹೋಗು handle_connect_error;
	}

	clean_ಆರಂಭ = (connect_flags & 0x02) >> 1;
	ಗಣ_session_expiry_interval(context, clean_ಆರಂಭ, protocol_ಆವೃತ್ತಿ);

	rc = read_and_reset_keepalive(context);
	ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
		ಹೋಗು handle_connect_error;
	}

	rc = read_and_verify_v5_connect_properties(context, &properties, protocol_ಆವೃತ್ತಿ);
	ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
		ಹೋಗು handle_connect_error;
	}

	will = connect_flags & 0x04;
	will_qos = (connect_flags & 0x18) >> 3;
	will_retain = ((connect_flags & 0x20) == 0x20);
	rc = verify_will_options(context, will, will_qos, will_retain, protocol_ಆವೃತ್ತಿ);
	ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
		ಹೋಗು handle_connect_error;
	}

	mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_ವಿಧಾನ, &context->auth_ವಿಧಾನ, false);
	mosquitto_property_read_ದ್ವಿಮಾನ(properties, MQTT_PROP_AUTHENTICATION_ದತ್ತಾಂಶ, &auth_ದತ್ತಾಂಶ, &auth_ದತ್ತಾಂಶ_len, false);
	mosquitto_property_free_all(&properties);

	ಆದರೆ(auth_ದತ್ತಾಂಶ && !context->auth_ವಿಧಾನ){
		send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, ಖಾಲಿ);
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s:%d CONNECT with missing clientid string.",
				context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
		rc = MOSQ_ERR_PROTOCOL;
		ಹೋಗು handle_connect_error;
	}

	rc = read_and_verify_clientid_from_packet(context, &clientid, &allow_zero_ಉದ್ದಳತೆ_clientid, clean_ಆರಂಭ);
	ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
		ಹೋಗು handle_connect_error;
	}

	ಆದರೆ(will){
		rc = will__read(context, clientid, &will_struct, will_qos, will_retain);
		ಆದರೆ(rc){
			ಆದರೆ(context->protocol == mosq_p_mqtt5){
				ಆದರೆ(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
					send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, ಖಾಲಿ);
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_MALFORMED_PACKET){
					send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, ಖಾಲಿ);
				}
			}
			ಹೋಗು handle_connect_error;
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಆದರೆ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
			ಆದರೆ(will_qos != 0 || will_retain != 0){
				log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT without Will with non-zero QoS (%d) or retain (%d).",
						clientid, will_qos, will_retain);
				rc = MOSQ_ERR_PROTOCOL;
				ಹೋಗು handle_connect_error;
			}
		}
	}

	// Client credentials
	password_flag = connect_flags & 0x40;
	username_flag = connect_flags & 0x80;
	rc = read_and_verify_client_credentials_from_packet(context, &username, username_flag, &password, &password_len, password_flag, clientid);
	ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
		ಹೋಗು handle_connect_error;
	}

	rc = check_additional_trailing_ದತ್ತಾಂಶ(context, protocol_ಆವೃತ್ತಿ);
	ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
		ಹೋಗು handle_connect_error;
	}

	/* Once context->id is set, if we return from this function with an error
	 * we must make sure that context->id is freed and set to NULL, so that the
	 * client isn't erroneously removed from the by_id hash table. */
	context->id = clientid;
	clientid = ಖಾಲಿ;

	/* use_identity_as_username or use_subject_as_username */
	rc = handle_username_from_cert_options(context, &username, &password, password_len);
	ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
		ಹೋಗು handle_connect_error;
	}

	/* use_username_as_clientid */
	ಆದರೆ(context->listener->use_username_as_clientid){
		rc = handle_username_as_clientid_option(context);
		ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
			ಹೋಗು handle_connect_error;
		}
	}

	/* Check for an existing delayed auth check, reject if present */
	HASH_ಕಂಡುಹಿಡಿ(hh_id, db.contexts_by_id_delayed_auth, context->id, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(context->id), ಸಿಕ್ಕಿತು_context);
	ಆದರೆ(ಸಿಕ್ಕಿತು_context){
		rc = MOSQ_ERR_UNKNOWN;
		ಹೋಗು handle_connect_error;
	}

	context->clean_ಆರಂಭ = clean_ಆರಂಭ;
	context->will = will_struct;
	will_struct = ಖಾಲಿ;

	ಆದರೆ(context->auth_ವಿಧಾನ){
		rc = mosquitto_security_auth_ಆರಂಭ(context, false, auth_ದತ್ತಾಂಶ, auth_ದತ್ತಾಂಶ_len, &auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು, &auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len);
		mosquitto_FREE(auth_ದತ್ತಾಂಶ);
		ಆದರೆ(rc == MOSQ_ERR_SUCCESS){
			ಹಿಂತಿರುಗಿ connect__on_authorised(context, auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು, auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len);
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_AUTH_CONTINUE){
			mosquitto_ಗಣ_state(context, mosq_cs_authenticating);
			rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು, auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len);
			ಸುರಕ್ಷಿತ_FREE(auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು);
			ಹಿಂತಿರುಗಿ rc;
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಸುರಕ್ಷಿತ_FREE(auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು);
			will__clear(context);
			ಆದರೆ(rc == MOSQ_ERR_AUTH){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ಖಾಲಿ);
				mosquitto_FREE(context->id);
				ಹೋಗು handle_connect_error;
			}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_NOT_SUPPORTED){
				/* Client has requested extended authentication, but we don't support it. */
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_ವಿಧಾನ, ಖಾಲಿ);
				mosquitto_FREE(context->id);
				ಹೋಗು handle_connect_error;
			}ಇಲ್ಲದಿದ್ದರೆ{
				mosquitto_FREE(context->id);
				ಹೋಗು handle_connect_error;
			}
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
		ಆದರೆ(context->listener->ssl_ctx && (context->listener->use_ತಟಸ್ಥ_as_username || context->listener->use_subject_as_username)){
			/* Authentication assumed to be cleared */
		}ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		{
			rc = mosquitto_basic_auth(context);
			ಆಯ್ಕೆ(rc){
				ಸ್ಥಿತಿ MOSQ_ERR_SUCCESS:
					ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ MOSQ_ERR_AUTH_DELAYED:
					mosquitto_ಗಣ_state(context, mosq_cs_delayed_auth);
					HASH_ಕೂಡಿಸು_KEYPTR(hh_id, db.contexts_by_id_delayed_auth, context->id, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(context->id), context);
					ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
					ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ MOSQ_ERR_AUTH:
					ಆದರೆ(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ಖಾಲಿ);
					}ಇಲ್ಲದಿದ್ದರೆ{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ಖಾಲಿ);
					}
					ಹೋಗು handle_connect_error;
					ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ MOSQ_ERR_UNSPECIFIED:
				ಸ್ಥಿತಿ MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
				ಸ್ಥಿತಿ MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
				ಸ್ಥಿತಿ MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
				ಸ್ಥಿತಿ MOSQ_ERR_SERVER_UNAVAILABLE:
				ಸ್ಥಿತಿ MOSQ_ERR_SERVER_BUSY:
				ಸ್ಥಿತಿ MOSQ_ERR_BANNED:
				ಸ್ಥಿತಿ MOSQ_ERR_BAD_AUTHENTICATION_ವಿಧಾನ:
				ಸ್ಥಿತಿ MOSQ_ERR_CONNECTION_ದರ_EXCEEDED:
					ಆದರೆ(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, (uint8_t)rc, ಖಾಲಿ);
					}ಇಲ್ಲದಿದ್ದರೆ{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ಖಾಲಿ);
					}
					ಹೋಗು handle_connect_error;
					ನಿಲ್ಲಿಸು;
				ಪೂರ್ವನಿಯೋಜಿತ:
					rc = MOSQ_ERR_UNKNOWN;
					ಹೋಗು handle_connect_error;
					ನಿಲ್ಲಿಸು;
			}
		}
		ಹಿಂತಿರುಗಿ connect__on_authorised(context, ಖಾಲಿ, 0);
	}


handle_connect_error:
	mosquitto_property_free_all(&properties);
	mosquitto_FREE(auth_ದತ್ತಾಂಶ);
	mosquitto_FREE(clientid);
	mosquitto_FREE(username);
	mosquitto_FREE(password);
	ಆದರೆ(will_struct){
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct->ಸಂದೇಶ.payload);
		mosquitto_FREE(will_struct->ಸಂದೇಶ.topic);
		mosquitto_FREE(will_struct);
	}
	will__clear(context);
	/* We return an error here which means the client is freed later on. */
	context->clean_ಆರಂಭ = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_ತಕ್ಷಣ;
	context->will_delay_interval = 0;
	ಹಿಂತಿರುಗಿ rc;
}
