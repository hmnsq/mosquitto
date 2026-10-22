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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <utlist.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "packet_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "property_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "send_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sys_tree.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "tls_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "will_mosq.h"

#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#ਜੇ_ਅੰਤ


ਸਥਿਰ ਅੱਖਰ nibble_ਮੰਜ਼ਿਲ_hex(uint8_t ਮੁੱਲ)
{
	ਜੇ(ਮੁੱਲ < 0x0A){
		ਵਾਪਸ (ਅੱਖਰ)('0'+ਮੁੱਲ);
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ (ਅੱਖਰ)(65 /*'A'*/ +ਮੁੱਲ-10);
	}
}


ਸਥਿਰ ਅੱਖਰ *clientid_gen(uint16_t *idlen, ਅਚਲ ਅੱਖਰ *auto_id_ਅਗੇਤਰ, uint16_t auto_id_ਅਗੇਤਰ_len)
{
	ਅੱਖਰ *clientid;
	uint8_t rnd[16];
	ਪੂਰਨ pos;

	ਜੇ(mosquitto_getrandom(rnd, 16)){
		ਵਾਪਸ ਨਿਰਾ;
	}

	*idlen = (uint16_t)(auto_id_ਅਗੇਤਰ_len + 36);

	clientid = (ਅੱਖਰ *)mosquitto_calloc((size_t)(*idlen) + 1, ਆਕਾਰ(ਅੱਖਰ));
	ਜੇ(!clientid){
		ਵਾਪਸ ਨਿਰਾ;
	}
	ਜੇ(auto_id_ਅਗੇਤਰ){
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(clientid, auto_id_ਅਗੇਤਰ, auto_id_ਅਗੇਤਰ_len);
	}

	pos = 0;
	ਲਈ(ਪੂਰਨ i=0; i<16; i++){
		clientid[auto_id_ਅਗੇਤਰ_len + pos + 0] = nibble_ਮੰਜ਼ਿਲ_hex(rnd[i] & 0x0F);
		clientid[auto_id_ਅਗੇਤਰ_len + pos + 1] = nibble_ਮੰਜ਼ਿਲ_hex((rnd[i] >> 4) & 0x0F);
		pos += 2;
		ਜੇ(pos == 8 || pos == 13 || pos == 18 || pos == 23){
			clientid[auto_id_ਅਗੇਤਰ_len + pos] = '-';
			pos++;
		}
	}

	ਵਾਪਸ clientid;
}


ਪੂਰਨ connect__on_authorised(ਬਣਤਰ mosquitto *context, ਖਾਲੀ *auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ, uint16_t auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ_len)
{
	ਬਣਤਰ mosquitto *ਮਿਲਿਆ_context;
	ਬਣਤਰ mosquitto__subleaf *ਪੱਤੀ;
	mosquitto_property *connack_props = ਨਿਰਾ;
	uint8_t connect_ack = 0;
	ਪੂਰਨ rc;
	ਪੂਰਨ ਵਿੱਚ_quota, ਬਾਹਰਲਾ_quota;
	uint16_t ਵਿੱਚ_ਵੱਧ_ਤੋਂ_ਵੱਧ, ਬਾਹਰਲਾ_ਵੱਧ_ਤੋਂ_ਵੱਧ;

	/* Find if this client already has an entry. This must be done *after* any security checks. */
	HASH_ਲੱਭੋ(hh_id, db.contexts_by_id, context->id, ਲੜੀ_ਲੰਬਾਈ(context->id), ਮਿਲਿਆ_context);
	ਜੇ(ਮਿਲਿਆ_context){
		/* Found a matching client */
		ਜੇ(!net__is_connected(ਮਿਲਿਆ_context)){
			/* Client is reconnecting after a disconnect */
			/* FIXME - does anything need to be done here? */
		}ਨਹੀਂਤਾਂ{
			/* Client is already connected, disconnect old version. This is
			 * done in context__cleanup() below. */
		}

		ਜੇ(context->clean_ਸ਼ੁਰੂ == true){
			sub__clean_session(ਮਿਲਿਆ_context);
			ਮਿਲਿਆ_context->session_expiry_interval = MQTT_SESSION_EXPIRY_ਤੁਰੰਤ;
			plugin_persist__handle_client_delete(ਮਿਲਿਆ_context);
		}
		context->is_persisted = ਮਿਲਿਆ_context->is_persisted;
		ਮਿਲਿਆ_context->is_persisted = false; /* stops persistence for context being removed */

		ਜੇ(context->clean_ਸ਼ੁਰੂ == false && ਮਿਲਿਆ_context->session_expiry_interval != MQTT_SESSION_EXPIRY_ਤੁਰੰਤ){
			ਜੇ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
				connect_ack |= 0x01;
			}

			ਜੇ(ਮਿਲਿਆ_context->msgs_ਵਿੱਚ.inflight || ਮਿਲਿਆ_context->msgs_ਵਿੱਚ.queued
					|| ਮਿਲਿਆ_context->msgs_ਬਾਹਰਲਾ.inflight || ਮਿਲਿਆ_context->msgs_ਬਾਹਰਲਾ.queued){

				ਵਿੱਚ_quota = context->msgs_ਵਿੱਚ.inflight_quota;
				ਬਾਹਰਲਾ_quota = context->msgs_ਬਾਹਰਲਾ.inflight_quota;
				ਵਿੱਚ_ਵੱਧ_ਤੋਂ_ਵੱਧ = context->msgs_ਵਿੱਚ.inflight_ਵੱਧ_ਤੋਂ_ਵੱਧ;
				ਬਾਹਰਲਾ_ਵੱਧ_ਤੋਂ_ਵੱਧ = context->msgs_ਬਾਹਰਲਾ.inflight_ਵੱਧ_ਤੋਂ_ਵੱਧ;

				ਸਮ੍ਰਿਤੀ_ਨਕਲ(&context->msgs_ਵਿੱਚ, &ਮਿਲਿਆ_context->msgs_ਵਿੱਚ, ਆਕਾਰ(ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_ਅੰਕੜੇ));
				ਸਮ੍ਰਿਤੀ_ਨਕਲ(&context->msgs_ਬਾਹਰਲਾ, &ਮਿਲਿਆ_context->msgs_ਬਾਹਰਲਾ, ਆਕਾਰ(ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_ਅੰਕੜੇ));
				context->ਆਖਰੀ_cmsg_id = ਮਿਲਿਆ_context->ਆਖਰੀ_cmsg_id;

				memset(&ਮਿਲਿਆ_context->msgs_ਵਿੱਚ, 0, ਆਕਾਰ(ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_ਅੰਕੜੇ));
				memset(&ਮਿਲਿਆ_context->msgs_ਬਾਹਰਲਾ, 0, ਆਕਾਰ(ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_ਅੰਕੜੇ));

				context->msgs_ਵਿੱਚ.inflight_quota = ਵਿੱਚ_quota;
				context->msgs_ਬਾਹਰਲਾ.inflight_quota = ਬਾਹਰਲਾ_quota;
				context->msgs_ਵਿੱਚ.inflight_ਵੱਧ_ਤੋਂ_ਵੱਧ = ਵਿੱਚ_ਵੱਧ_ਤੋਂ_ਵੱਧ;
				context->msgs_ਬਾਹਰਲਾ.inflight_ਵੱਧ_ਤੋਂ_ਵੱਧ = ਬਾਹਰਲਾ_ਵੱਧ_ਤੋਂ_ਵੱਧ;

				db_ਸੁਨੇਹਾ_reconnect_reset(context);
			}
			context->subs = ਮਿਲਿਆ_context->subs;
			ਮਿਲਿਆ_context->subs = ਨਿਰਾ;
			context->subs_ਸਮਰੱਥਾ = ਮਿਲਿਆ_context->subs_ਸਮਰੱਥਾ;
			context->subs_ਗਿਣਤੀ = ਮਿਲਿਆ_context->subs_ਗਿਣਤੀ;
			ਮਿਲਿਆ_context->subs_ਸਮਰੱਥਾ = 0;
			ਮਿਲਿਆ_context->subs_ਗਿਣਤੀ = 0;
			context->ਆਖਰੀ_mid = ਮਿਲਿਆ_context->ਆਖਰੀ_mid;

			ਲਈ(ਪੂਰਨ i=0; i<context->subs_ਸਮਰੱਥਾ; i++){
				ਜੇ(context->subs[i]){
					ਪੱਤੀ = context->subs[i]->hier->subs;
					ਜਦੋਂ(ਪੱਤੀ){
						ਜੇ(ਪੱਤੀ->context == ਮਿਲਿਆ_context){
							ਪੱਤੀ->context = context;
						}
						ਪੱਤੀ = ਪੱਤੀ->ਅਗਲਾ;
					}

					ਜੇ(context->subs[i]->shared){
						ਪੱਤੀ = context->subs[i]->shared->subs;
						ਜਦੋਂ(ਪੱਤੀ){
							ਜੇ(ਪੱਤੀ->context == ਮਿਲਿਆ_context){
								ਪੱਤੀ->context = context;
							}
							ਪੱਤੀ = ਪੱਤੀ->ਅਗਲਾ;
						}
					}
				}
			}
		}

		ਜੇ((ਮਿਲਿਆ_context->protocol == mosq_p_mqtt5 && ਮਿਲਿਆ_context->session_expiry_interval == MQTT_SESSION_EXPIRY_ਤੁਰੰਤ)
				|| (ਮਿਲਿਆ_context->protocol != mosq_p_mqtt5 && ਮਿਲਿਆ_context->clean_ਸ਼ੁਰੂ == true)
				|| (context->clean_ਸ਼ੁਰੂ == true)
				){

			context__send_will(ਮਿਲਿਆ_context);
		}

		session_expiry__remove(ਮਿਲਿਆ_context);
		will_delay__remove(ਮਿਲਿਆ_context);
		will__clear(ਮਿਲਿਆ_context);

		ਮਿਲਿਆ_context->clean_ਸ਼ੁਰੂ = true;
		ਮਿਲਿਆ_context->session_expiry_interval = MQTT_SESSION_EXPIRY_ਤੁਰੰਤ;
		mosquitto_ਸੈੱਟ_state(ਮਿਲਿਆ_context, mosq_cs_duplicate);
		ਜੇ(ਮਿਲਿਆ_context->protocol == mosq_p_mqtt5){
			send__disconnect(ਮਿਲਿਆ_context, MQTT_RC_SESSION_ਲਿਆ_OVER, ਨਿਰਾ);
		}
		do_disconnect(ਮਿਲਿਆ_context, MOSQ_ERR_SESSION_ਲਿਆ_OVER);
	}

	ਜੇ(db.config->global_max_clients > 0 && HASH_CNT(hh_id, db.contexts_by_id) >= (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)db.config->global_max_clients){
		ਜੇ(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_SERVER_BUSY, ਨਿਰਾ);
		}
		rc = MOSQ_ERR_INVAL;
		ਜਾਓ error;
	}

	ਜੇ(db.config->connection_messages == true){
		ਜੇ(context->is_bridge){
			ਜੇ(context->username){
				log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->ਪਤਾ, context->remote_ਪੋਰਟ, context->id, context->protocol, context->clean_ਸ਼ੁਰੂ, context->keepalive, context->username);
			}ਨਹੀਂਤਾਂ{
				log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d).",
						context->ਪਤਾ, context->remote_ਪੋਰਟ, context->id, context->protocol, context->clean_ਸ਼ੁਰੂ, context->keepalive);
			}
		}ਨਹੀਂਤਾਂ{
			ਜੇ(context->username){
				log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->ਪਤਾ, context->remote_ਪੋਰਟ, context->id, context->protocol, context->clean_ਸ਼ੁਰੂ, context->keepalive, context->username);
			}ਨਹੀਂਤਾਂ{
				log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d).",
						context->ਪਤਾ, context->remote_ਪੋਰਟ, context->id, context->protocol, context->clean_ਸ਼ੁਰੂ, context->keepalive);
			}
		}

		ਜੇ(context->will){
			log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG, "Will message specified (%ld bytes) (r%d, q%d).",
					(ਲੰਮਾ)context->will->ਸੁਨੇਹਾ.payloadlen,
					context->will->ਸੁਨੇਹਾ.retain,
					context->will->ਸੁਨੇਹਾ.qos);

			log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG, "\t%s", context->will->ਸੁਨੇਹਾ.topic);
		}ਨਹੀਂਤਾਂ{
			log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG, "No will message specified.");
		}
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
	ਜੇ(context->ssl){
		log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s negotiated %s cipher %s",
				context->id,
				SSL_get_cipher_ਸੰਸਕਰਣ(context->ssl),
				SSL_get_cipher_name(context->ssl));
	}
#ਜੇ_ਅੰਤ

	context->ping_t = 0;
	context->is_dropping = false;

	/* Remove any queued messages that are no longer allowed through ACL,
	 * assuming a possible change of username. */
	db_check_acl_ਦਾ_all_messages(context);
	context_ਜੋੜੋ_ਮੰਜ਼ਿਲ_by_id(context);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_PERSISTENCE
	ਜੇ(!context->clean_ਸ਼ੁਰੂ){
		db.persistence_changes++;
	}
#ਜੇ_ਅੰਤ
	context->max_qos = context->listener->max_qos;

	ਜੇ(db.config->max_keepalive &&
			(context->keepalive > db.config->max_keepalive || context->keepalive == 0)){

		keepalive__remove(context);
		context->keepalive = db.config->max_keepalive;
		keepalive_ਜੋੜੋ(context);
		ਜੇ(context->protocol == mosq_p_mqtt5){
			ਜੇ(mosquitto_property_ਜੋੜੋ_int16(&connack_props, MQTT_PROP_SERVER_KEEP_ALIVE, context->keepalive)){
				rc = MOSQ_ERR_NOMEM;
				ਜਾਓ error;
			}
		}ਨਹੀਂਤਾਂ{
			send__connack(context, connect_ack, CONNACK_REFUSED_IDENTIFIER_REJECTED, ਨਿਰਾ);
			rc = MOSQ_ERR_INVAL;
			ਜਾਓ error;
		}
	}


	ਜੇ(context->protocol == mosq_p_mqtt5){
		ਜੇ(context->listener->max_topic_alias > 0){
			ਜੇ(mosquitto_property_ਜੋੜੋ_int16(&connack_props, MQTT_PROP_TOPIC_ALIAS_ਵੱਧ_ਤੋਂ_ਵੱਧ, context->listener->max_topic_alias)){
				rc = MOSQ_ERR_NOMEM;
				ਜਾਓ error;
			}
		}
		ਜੇ(context->assigned_id){
			ਜੇ(mosquitto_property_ਜੋੜੋ_string(&connack_props, MQTT_PROP_ASSIGNED_CLIENT_IDENTIFIER, context->id)){
				rc = MOSQ_ERR_NOMEM;
				ਜਾਓ error;
			}
		}
		ਜੇ(context->auth_ਵਿਧੀ){
			ਜੇ(mosquitto_property_ਜੋੜੋ_string(&connack_props, MQTT_PROP_AUTHENTICATION_ਵਿਧੀ, context->auth_ਵਿਧੀ)){
				rc = MOSQ_ERR_NOMEM;
				ਜਾਓ error;
			}

			ਜੇ(auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ && auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ_len > 0){
				ਜੇ(mosquitto_property_ਜੋੜੋ_ਦੁਆਧਾਰੀ(&connack_props, MQTT_PROP_AUTHENTICATION_ਅੰਕੜੇ, auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ, auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ_len)){
					rc = MOSQ_ERR_NOMEM;
					ਜਾਓ error;
				}
			}
		}
	}
	ਸੁਰੱਖਿਅਤ_FREE(auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ);

	mosquitto_ਸੈੱਟ_state(context, mosq_cs_ਸਰਗਰਮ);
	rc = send__connack(context, connect_ack, CONNACK_ACCEPTED, connack_props);
	mosquitto_property_free_all(&connack_props);
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}
	db__expire_all_messages(context);
	rc = db_ਸੁਨੇਹਾ_write_queued_ਬਾਹਰਲਾ(context);
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}
	rc = db_ਸੁਨੇਹਾ_write_inflight_ਬਾਹਰਲਾ_all(context);

	ਜੇ(rc == MOSQ_ERR_SUCCESS){
		plugin__handle_connect(context);

		ਜੇ(context->session_expiry_interval != MQTT_SESSION_EXPIRY_ਤੁਰੰਤ){
			plugin_persist_handle_client_ਜੋੜੋ(context);
		}ਨਹੀਂਤਾਂ ਜੇ(context->will){
			plugin_persist_handle_will_ਜੋੜੋ(context);
		}
	}
	ਵਾਪਸ rc;
error:
	ਸੁਰੱਖਿਅਤ_FREE(auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ);
	mosquitto_property_free_all(&connack_props);
	ਵਾਪਸ rc;
}


ਸਥਿਰ ਪੂਰਨ will__read(ਬਣਤਰ mosquitto *context, ਅਚਲ ਅੱਖਰ *clientid, ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_all **will, uint8_t will_qos, ਪੂਰਨ will_retain)
{
	ਪੂਰਨ rc = MOSQ_ERR_SUCCESS;
	size_t slen;
	uint16_t tlen;
	ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_all *will_struct = ਨਿਰਾ;
	ਅੱਖਰ *will_topic_mount = ਨਿਰਾ;
	uint16_t payloadlen;
	mosquitto_property *properties = ਨਿਰਾ;

	will_struct = mosquitto_calloc(1, ਆਕਾਰ(ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_all));
	ਜੇ(!will_struct){
		rc = MOSQ_ERR_NOMEM;
		ਜਾਓ error_cleanup;
	}
	ਜੇ(context->protocol == PROTOCOL_ਸੰਸਕਰਣ_v5){
		rc = property__read_all(CMD_WILL, &context->ਵਿੱਚ_packet, &properties);
		ਜੇ(rc){
			ਜਾਓ error_cleanup;
		}

		rc = property__process_will(context, will_struct, &properties);
		mosquitto_property_free_all(&properties);
		ਜੇ(rc){
			ਜਾਓ error_cleanup;
		}
	}
	rc = packet__read_string(&context->ਵਿੱਚ_packet, &will_struct->ਸੁਨੇਹਾ.topic, &tlen);
	ਜੇ(rc){
		ਜਾਓ error_cleanup;
	}
	ਜੇ(!tlen){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: Will with empty topic.",
				context->id);
		rc = MOSQ_ERR_PROTOCOL;
		ਜਾਓ error_cleanup;
	}

	ਜੇ(context->listener->mount_ਬਿੰਦੂ){
		slen = ਲੜੀ_ਲੰਬਾਈ(context->listener->mount_ਬਿੰਦੂ) + ਲੜੀ_ਲੰਬਾਈ(will_struct->ਸੁਨੇਹਾ.topic) + 1;
		will_topic_mount = mosquitto_malloc(slen+1);
		ਜੇ(!will_topic_mount){
			rc = MOSQ_ERR_NOMEM;
			ਜਾਓ error_cleanup;
		}

		snprintf(will_topic_mount, slen, "%s%s", context->listener->mount_ਬਿੰਦੂ, will_struct->ਸੁਨੇਹਾ.topic);
		will_topic_mount[slen] = '\0';

		mosquitto_FREE(will_struct->ਸੁਨੇਹਾ.topic);
		will_struct->ਸੁਨੇਹਾ.topic = will_topic_mount;
	}

	ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(will_struct->ਸੁਨੇਹਾ.topic, "$CONTROL/", ਲੜੀ_ਲੰਬਾਈ("$CONTROL/"))){
		rc = MOSQ_ERR_ACL_DENIED;
		ਜਾਓ error_cleanup;
	}
	rc = mosquitto_pub_topic_check(will_struct->ਸੁਨੇਹਾ.topic);
	ਜੇ(rc){
		ਜਾਓ error_cleanup;
	}

	rc = packet__read_uint16(&context->ਵਿੱਚ_packet, &payloadlen);
	ਜੇ(rc){
		ਜਾਓ error_cleanup;
	}

	will_struct->ਸੁਨੇਹਾ.payloadlen = payloadlen;
	ਜੇ(will_struct->ਸੁਨੇਹਾ.payloadlen > 0){
		ਜੇ(db.config->ਸੁਨੇਹਾ_ਮਾਪ_ਹੱਦਬੰਦੀ && will_struct->ਸੁਨੇਹਾ.payloadlen > (ਪੂਰਨ)db.config->ਸੁਨੇਹਾ_ਮਾਪ_ਹੱਦਬੰਦੀ){
			log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG, "Client %s connected with too large Will payload", clientid);
			ਜੇ(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_PACKET_TOO_LARGE, ਨਿਰਾ);
			}ਨਹੀਂਤਾਂ{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ਨਿਰਾ);
			}
			rc = MOSQ_ERR_PAYLOAD_ਮਾਪ;
			ਜਾਓ error_cleanup;
		}
		will_struct->ਸੁਨੇਹਾ.payload = mosquitto_malloc((size_t)will_struct->ਸੁਨੇਹਾ.payloadlen);
		ਜੇ(!will_struct->ਸੁਨੇਹਾ.payload){
			rc = MOSQ_ERR_NOMEM;
			ਜਾਓ error_cleanup;
		}

		rc = packet_read_ਬਾਈਟਾਂ(&context->ਵਿੱਚ_packet, will_struct->ਸੁਨੇਹਾ.payload, (uint32_t)will_struct->ਸੁਨੇਹਾ.payloadlen);
		ਜੇ(rc){
			ਜਾਓ error_cleanup;
		}
	}

	will_struct->ਸੁਨੇਹਾ.qos = will_qos;
	will_struct->ਸੁਨੇਹਾ.retain = will_retain;

	*will = will_struct;
	ਵਾਪਸ MOSQ_ERR_SUCCESS;

error_cleanup:
	ਜੇ(will_struct){
		mosquitto_FREE(will_struct->ਸੁਨੇਹਾ.topic);
		mosquitto_FREE(will_struct->ਸੁਨੇਹਾ.payload);
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct);
	}
	ਵਾਪਸ rc;
}


ਸਥਿਰ ਪੂਰਨ check_protocol_ਸੰਸਕਰਣ(ਬਣਤਰ mosquitto__listener *listener, ਪੂਰਨ protocol_ਸੰਸਕਰਣ)
{
	/* Allow bridge protocol as well. */
	protocol_ਸੰਸਕਰਣ &= 0x7F;

	ਜੇ((protocol_ਸੰਸਕਰਣ == 3 && listener->disable_protocol_v3 == false)
			|| (protocol_ਸੰਸਕਰਣ == 4 && listener->disable_protocol_v4 == false)
			|| (protocol_ਸੰਸਕਰਣ == 5 && listener->disable_protocol_v5 == false)
			){

		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ MOSQ_ERR_NOT_SUPPORTED;
	}
}


ਸਤਰਅੰਦਰ ਸਥਿਰ ਪੂਰਨ send__connack_error_and_return(ਬਣਤਰ mosquitto *context, uint8_t err_ਕੋਡ, ਪੂਰਨ rc)
{
	send__connack(context, 0, err_ਕੋਡ, ਨਿਰਾ);
	ਵਾਪਸ rc;
}


ਸਤਰਅੰਦਰ ਸਥਿਰ ਪੂਰਨ send__connack_bad_username_or_password_error(ਬਣਤਰ mosquitto *context, ਪੂਰਨ rc)
{
	uint8_t err_ਕੋਡ  = context->protocol == mosq_p_mqtt5
									? (uint8_t)MQTT_RC_BAD_USERNAME_OR_PASSWORD
									: (uint8_t)CONNACK_REFUSED_BAD_USERNAME_PASSWORD;
	ਵਾਪਸ send__connack_error_and_return(context, err_ਕੋਡ, rc);
}


ਸਥਿਰ ਪੂਰਨ read_protocol_name(ਬਣਤਰ mosquitto *context, ਅੱਖਰ protocol_name[7])
{
	/* Read protocol name as length then bytes rather than with read_string
	 * because the length is fixed and we can check that. Removes the need
	 * for another malloc as well. */

	uint16_t slen = 0;

	ਜੇ(packet__read_uint16(&context->ਵਿੱਚ_packet, &slen)){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string length.",
				context->ਪਤਾ, context->remote_ਪੋਰਟ);
		ਵਾਪਸ MOSQ_ERR_PROTOCOL;
	}
	ਜੇ(slen != 4 /* MQTT */ && slen != 6 /* MQIsdp */){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with incorrect protocol string length (%d).",
				context->ਪਤਾ, context->remote_ਪੋਰਟ, slen);
		ਵਾਪਸ MOSQ_ERR_PROTOCOL;
	}
	ਜੇ(packet_read_ਬਾਈਟਾਂ(&context->ਵਿੱਚ_packet, protocol_name, slen)){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string.",
				context->ਪਤਾ, context->remote_ਪੋਰਟ);
		ਵਾਪਸ MOSQ_ERR_PROTOCOL;
	}
	protocol_name[slen] = '\0';

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ read_and_verify_protocol_ਸੰਸਕਰਣ(ਬਣਤਰ mosquitto *context, ਅਚਲ ਅੱਖਰ *protocol_name,
		uint8_t *protocol_ਸੰਸਕਰਣ)
{
	uint8_t tmp_protocol_ਸੰਸਕਰਣ = 0;
	ਜੇ(packet__read_byte(&context->ਵਿੱਚ_packet, &tmp_protocol_ਸੰਸਕਰਣ)){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol version.",
				context->ਪਤਾ, context->remote_ਪੋਰਟ);
		ਵਾਪਸ MOSQ_ERR_PROTOCOL;
	}

	ਜੇ(check_protocol_ਸੰਸਕਰਣ(context->listener, tmp_protocol_ਸੰਸਕਰਣ)){
		ਜੇ(tmp_protocol_ਸੰਸਕਰਣ == 3 || tmp_protocol_ਸੰਸਕਰਣ == 4){
			context->protocol = mosq_p_mqtt311;
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_ਸੰਸਕਰਣ, ਨਿਰਾ);
		}ਨਹੀਂਤਾਂ{
			context->protocol = mosq_p_mqtt5;
			send__connack(context, 0, MQTT_RC_UNSUPPORTED_PROTOCOL_ਸੰਸਕਰਣ, ਨਿਰਾ);
		}
		ਵਾਪਸ MOSQ_ERR_NOT_SUPPORTED;
	}

	ਜੇ(!strcmp(protocol_name, PROTOCOL_NAME_v31)){
		ਜੇ((tmp_protocol_ਸੰਸਕਰਣ&0x7F) != PROTOCOL_ਸੰਸਕਰਣ_v31){
			ਜੇ(db.config->connection_messages == true){
				log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->ਪਤਾ, context->remote_ਪੋਰਟ, tmp_protocol_ਸੰਸਕਰਣ);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_ਸੰਸਕਰਣ, ਨਿਰਾ);
			ਵਾਪਸ MOSQ_ERR_PROTOCOL;
		}
		context->protocol = mosq_p_mqtt31;
		ਜੇ((tmp_protocol_ਸੰਸਕਰਣ&0x80) == 0x80){
			context->is_bridge = true;
		}
	}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(protocol_name, PROTOCOL_NAME)){
		ਜੇ((tmp_protocol_ਸੰਸਕਰਣ&0x7F) == PROTOCOL_ਸੰਸਕਰਣ_v311){
			context->protocol = mosq_p_mqtt311;

			ਜੇ((tmp_protocol_ਸੰਸਕਰਣ&0x80) == 0x80){
				context->is_bridge = true;
			}
		}ਨਹੀਂਤਾਂ ਜੇ((tmp_protocol_ਸੰਸਕਰਣ&0x7F) == PROTOCOL_ਸੰਸਕਰਣ_v5){
			context->protocol = mosq_p_mqtt5;
		}ਨਹੀਂਤਾਂ{
			ਜੇ(db.config->connection_messages == true){
				log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->ਪਤਾ, context->remote_ਪੋਰਟ, tmp_protocol_ਸੰਸਕਰਣ);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_ਸੰਸਕਰਣ, ਨਿਰਾ);
			ਵਾਪਸ MOSQ_ERR_PROTOCOL;
		}
		ਜੇ((context->ਵਿੱਚ_packet.command&0x0F) != 0x00){
			/* Reserved flags not set to 0, must disconnect. */
			log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero reserved flags (%02X).",
					context->ਪਤਾ, context->remote_ਪੋਰਟ, context->ਵਿੱਚ_packet.command);
			ਵਾਪਸ MOSQ_ERR_PROTOCOL;
		}
	}ਨਹੀਂਤਾਂ{
		ਜੇ(db.config->connection_messages == true){
			log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol \"%s\".",
					context->ਪਤਾ, context->remote_ਪੋਰਟ, protocol_name);
		}
		ਵਾਪਸ MOSQ_ERR_PROTOCOL;
	}
	ਜੇ((tmp_protocol_ਸੰਸਕਰਣ&0x7F) != PROTOCOL_ਸੰਸਕਰਣ_v31 && context->ਵਿੱਚ_packet.command != CMD_CONNECT){
		ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
	}

	*protocol_ਸੰਸਕਰਣ = tmp_protocol_ਸੰਸਕਰਣ;
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ read_and_verify_connect_flags(ਬਣਤਰ mosquitto *context, uint8_t *connect_flags)
{
	ਜੇ(packet__read_byte(&context->ਵਿੱਚ_packet, connect_flags)){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing connect flags.",
				context->ਪਤਾ, context->remote_ਪੋਰਟ);
		ਵਾਪਸ MOSQ_ERR_PROTOCOL;
	}
	ਜੇ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		ਜੇ((*connect_flags & 0x01) != 0x00){
			log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero connect reserved flag (%02X).",
					context->ਪਤਾ, context->remote_ਪੋਰਟ, *connect_flags);
			ਵਾਪਸ MOSQ_ERR_PROTOCOL;
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_session_expiry_interval(ਬਣਤਰ mosquitto *context, uint8_t clean_ਸ਼ੁਰੂ, uint8_t protocol_ਸੰਸਕਰਣ)
{
	/* session_expiry_interval will be overridden if the properties are read later */
	ਜੇ(clean_ਸ਼ੁਰੂ == false && protocol_ਸੰਸਕਰਣ != PROTOCOL_ਸੰਸਕਰਣ_v5){
		/* v3* has clean_start == false mean the session never expires */
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
	}ਨਹੀਂਤਾਂ{
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_ਤੁਰੰਤ;
	}
}


ਸਥਿਰ ਪੂਰਨ read_and_reset_keepalive(ਬਣਤਰ mosquitto *context)
{
	/* _remove here because net__socket_accept() uses _add and we must have the
	 * correct keepalive value */
	keepalive__remove(context);

	ਜੇ(packet__read_uint16(&context->ਵਿੱਚ_packet, &(context->keepalive))){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "%s sent CONNECT with missing keepalive.",
				context->id);
		ਵਾਪਸ MOSQ_ERR_PROTOCOL;
	}
	keepalive_ਜੋੜੋ(context);

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ read_and_verify_v5_connect_properties(ਬਣਤਰ mosquitto *context, mosquitto_property **properties, uint8_t protocol_ਸੰਸਕਰਣ)
{
	ਪੂਰਨ rc;

	ਜੇ(protocol_ਸੰਸਕਰਣ == PROTOCOL_ਸੰਸਕਰਣ_v5){
		rc = property__read_all(CMD_CONNECT, &context->ਵਿੱਚ_packet, properties);
		ਜੇ(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
			send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, ਨਿਰਾ);
		}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_MALFORMED_PACKET){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, ਨਿਰਾ);
		}
		ਜੇ(rc){
			ਵਾਪਸ rc;
		}
	}
	rc = property__process_connect(context, properties);
	ਜੇ(rc != MOSQ_ERR_SUCCESS){
		ਵਾਪਸ send__connack_error_and_return(context, MQTT_RC_PROTOCOL_ERROR, rc);
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ verify_will_options(ਬਣਤਰ mosquitto *context, uint8_t will, uint8_t will_qos, uint8_t will_retain, uint8_t protocol_ਸੰਸਕਰਣ)
{
	ਜੇ(will_qos == 3){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid Will QoS (%d).",
				context->ਪਤਾ, context->remote_ਪੋਰਟ, will_qos);
		ਵਾਪਸ MOSQ_ERR_PROTOCOL;
	}

	ਜੇ(will && will_retain && db.config->retain_ਉਪਲਬਧ == false){
		ਜੇ(protocol_ਸੰਸਕਰਣ == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_RETAIN_NOT_SUPPORTED, ਨਿਰਾ);
		}
		ਵਾਪਸ MOSQ_ERR_NOT_SUPPORTED;
	}

	ਜੇ(will && will_qos > context->listener->max_qos){
		ਜੇ(protocol_ਸੰਸਕਰਣ == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_QOS_NOT_SUPPORTED, ਨਿਰਾ);
		}
		ਵਾਪਸ MOSQ_ERR_NOT_SUPPORTED;
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ handle_zero_ਲੰਮਾਈ_clientid(ਬਣਤਰ mosquitto *context, ਅੱਖਰ **clientid, bool *allow_zero_ਲੰਮਾਈ_clientid,
		uint8_t clean_ਸ਼ੁਰੂ)
{
	ਜੇ(context->protocol == mosq_p_mqtt31){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s:%d: v3.1 CONNECT with zero length clientid.",
				context->ਪਤਾ, context->remote_ਪੋਰਟ);
		send__connack(context, 0, CONNACK_REFUSED_IDENTIFIER_REJECTED, ਨਿਰਾ);
		ਵਾਪਸ MOSQ_ERR_PROTOCOL;
	}

	/* mqtt311/mqtt5 */
	mosquitto_FREE(*clientid);

	ਜੇ(db.config->ਪ੍ਰਤੀ_listener_settings){
		*allow_zero_ਲੰਮਾਈ_clientid = context->listener->security_options->allow_zero_ਲੰਮਾਈ_clientid;
	}ਨਹੀਂਤਾਂ{
		*allow_zero_ਲੰਮਾਈ_clientid = db.config->security_options.allow_zero_ਲੰਮਾਈ_clientid;
	}

	ਜੇ((context->protocol == mosq_p_mqtt311 && clean_ਸ਼ੁਰੂ == 0) || *allow_zero_ਲੰਮਾਈ_clientid == false){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with zero length clientid when forbidden.",
				context->ਪਤਾ, context->remote_ਪੋਰਟ);
		uint8_t err_ਕੋਡ = context->protocol == mosq_p_mqtt311 ? (uint8_t)CONNACK_REFUSED_IDENTIFIER_REJECTED : (uint8_t)MQTT_RC_UNSPECIFIED;
		ਵਾਪਸ send__connack_error_and_return(context, err_ਕੋਡ, MOSQ_ERR_PROTOCOL);
	}

	*clientid = clientid_gen(&(uint16_t){0}, context->listener->security_options->auto_id_ਅਗੇਤਰ,
			context->listener->security_options->auto_id_ਅਗੇਤਰ_len);
	ਜੇ(*clientid == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}
	context->assigned_id = true;

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ check_clientid_prefixes(ਬਣਤਰ mosquitto *context, ਅਚਲ ਅੱਖਰ *clientid)
{
	ਜੇ(db.config->clientid_prefixes){
		ਜੇ(ਲੜੀ_ਤੁਲਨਾ(db.config->clientid_prefixes, clientid, ਲੜੀ_ਲੰਬਾਈ(db.config->clientid_prefixes))){
			uint8_t err_ਕੋਡ = context->protocol == mosq_p_mqtt5 ? (uint8_t)MQTT_RC_NOT_AUTHORIZED : (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
			ਵਾਪਸ send__connack_error_and_return(context, err_ਕੋਡ, MOSQ_ERR_AUTH);
		}
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ read_and_verify_clientid_from_packet(ਬਣਤਰ mosquitto *context, ਅੱਖਰ **clientid,
		bool *allow_zero_ਲੰਮਾਈ_clientid, uint8_t clean_ਸ਼ੁਰੂ)
{
	ਪੂਰਨ rc;
	uint16_t slen;

	rc = packet__read_string(&context->ਵਿੱਚ_packet, clientid, &slen);
	ਜੇ(rc == MOSQ_ERR_MALFORMED_UTF8){
		ਜੇ(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_CLIENTID_NOT_VALID, ਨਿਰਾ);
		}
		ਵਾਪਸ MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID;
	}ਨਹੀਂਤਾਂ ਜੇ(rc > 0){
		ਵਾਪਸ MOSQ_ERR_PROTOCOL;
	}

	ਜੇ(slen == 0){
		rc = handle_zero_ਲੰਮਾਈ_clientid(context, clientid, allow_zero_ਲੰਮਾਈ_clientid, clean_ਸ਼ੁਰੂ);
		ਜੇ(rc != MOSQ_ERR_SUCCESS){
			ਵਾਪਸ rc;
		}
	}

	rc = check_clientid_prefixes(context, *clientid);
	ਜੇ(rc != MOSQ_ERR_SUCCESS){
		ਵਾਪਸ rc;
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ ਸੈੱਟ_username_from_packet(ਬਣਤਰ mosquitto *context, ਅੱਖਰ **username, ਅਚਲ ਅੱਖਰ *clientid)
{
	ਪੂਰਨ rc;

	rc = packet__read_string(&context->ਵਿੱਚ_packet, username, &(uint16_t){0});
	ਜੇ(rc == MOSQ_ERR_NOMEM){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}
	ਜੇ(rc != MOSQ_ERR_SUCCESS){
		ਜੇ(context->protocol == mosq_p_mqtt31){
			/* Username flag given, but no username. Ignore. */
			/* NOTE: Removed setting of username_flag to zero as it is unused afterwards */
		}ਨਹੀਂਤਾਂ{
			log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with username flag but no username.",
					clientid);
			ਵਾਪਸ MOSQ_ERR_PROTOCOL;
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ ਸੈੱਟ_password_from_packet(ਬਣਤਰ mosquitto *context, ਅੱਖਰ **password, uint16_t *password_len, ਅਚਲ ਅੱਖਰ *clientid)
{
	ਪੂਰਨ rc;

	rc = packet_read_ਦੁਆਧਾਰੀ(&context->ਵਿੱਚ_packet, (uint8_t **)password, password_len);
	ਜੇ(rc == MOSQ_ERR_NOMEM){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	ਜੇ(rc == MOSQ_ERR_MALFORMED_PACKET){
		ਜੇ(context->protocol == mosq_p_mqtt31){
			/* Password flag given, but no password. Ignore. */
		}ਨਹੀਂਤਾਂ{
			log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with password flag but no password.",
					clientid);
			ਵਾਪਸ MOSQ_ERR_PROTOCOL;
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ read_and_verify_client_credentials_from_packet(ਬਣਤਰ mosquitto *context,
		ਅੱਖਰ **username, uint8_t username_flag,
		ਅੱਖਰ **password, uint16_t *password_len, uint8_t password_flag,
		ਅਚਲ ਅੱਖਰ *clientid)
{
	ਪੂਰਨ rc;

	ਜੇ(username_flag){
		rc = ਸੈੱਟ_username_from_packet(context, username, clientid);
		ਜੇ(rc != MOSQ_ERR_SUCCESS){
			ਵਾਪਸ rc;
		}
	}ਨਹੀਂਤਾਂ{
		ਜੇ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
			ਜੇ(password_flag){
				/* username_flag == 0 && password_flag == 1 is forbidden */
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Protocol error from %s: password without username, closing connection.", clientid);
				ਵਾਪਸ MOSQ_ERR_PROTOCOL;
			}
		}
	}
	ਜੇ(password_flag){
		rc = ਸੈੱਟ_password_from_packet(context, password, password_len, clientid);
		ਜੇ(rc != MOSQ_ERR_SUCCESS){
			ਵਾਪਸ rc;
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ check_additional_trailing_ਅੰਕੜੇ(ਬਣਤਰ mosquitto *context, uint8_t protocol_ਸੰਸਕਰਣ)
{
	ਜੇ(context->ਵਿੱਚ_packet.pos != context->ਵਿੱਚ_packet.ਬਚਿਆ_ਲੰਮਾਈ){
		/* Surplus data at end of packet, this must be an error. */
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->ਵਿੱਚ_packet.pos, context->ਵਿੱਚ_packet.ਬਚਿਆ_ਲੰਮਾਈ);
		ਜੇ(protocol_ਸੰਸਕਰਣ == PROTOCOL_ਸੰਸਕਰਣ_v5){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, ਨਿਰਾ);
		}
		ਵਾਪਸ MOSQ_ERR_PROTOCOL;
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS


ਸਤਰਅੰਦਰ ਸਥਿਰ ਪੂਰਨ get_client_cert_and_subject_name(ਬਣਤਰ mosquitto *context, X509 **client_cert, X509_NAME **name)
{
	*client_cert = SSL_get_peer_certificate(context->ssl);
	ਜੇ(*client_cert == ਨਿਰਾ){
		ਵਾਪਸ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	*name = X509_get_subject_name(*client_cert);
	ਜੇ(*name == ਨਿਰਾ){
		X509_free(*client_cert);
		ਵਾਪਸ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਤਰਅੰਦਰ ਸਥਿਰ ਪੂਰਨ free_x509_and_send_connack_error(ਬਣਤਰ mosquitto *context, X509 *client_cert, ਪੂਰਨ rc)
{
	X509_free(client_cert);
	ਵਾਪਸ send__connack_bad_username_or_password_error(context, rc);
}


ਸਤਰਅੰਦਰ ਸਥਿਰ ਪੂਰਨ free_x509_and_BIO_and_send_connack_error(ਬਣਤਰ mosquitto *context, X509 *client_cert,
		BIO *subject_name, ਪੂਰਨ rc)
{
	BIO_free(subject_name);
	ਵਾਪਸ free_x509_and_send_connack_error(context, client_cert, rc);
}


ਸਥਿਰ ਪੂਰਨ ਸੈੱਟ_username_from_cert_ਪਛਾਣ_ਤੱਤ(ਬਣਤਰ mosquitto *context)
{
	X509 *client_cert = ਨਿਰਾ;
	X509_NAME *name = ਨਿਰਾ;

	ਜੇ(get_client_cert_and_subject_name(context, &client_cert, &name)){
		ਵਾਪਸ MOSQ_ERR_AUTH;
	}

	ਪੂਰਨ i = -1;
	X509_NAME_ਇੰਦਰਾਜ *name_ਇੰਦਰਾਜ = ਨਿਰਾ;

	i = X509_NAME_get_ਸੂਚਕਾਂਕ_by_NID(name, NID_commonName, -1);
	ਜੇ(i == -1){
		ਵਾਪਸ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	name_ਇੰਦਰਾਜ = X509_NAME_get_ਇੰਦਰਾਜ(name, i);
	ਜੇ(!name_ਇੰਦਰਾਜ){
		ਜਾਓ success;
	}

	ASN1_STRING *name_asn1 = ਨਿਰਾ;
	name_asn1 = X509_NAME_ਇੰਦਰਾਜ_get_ਅੰਕੜੇ(name_ਇੰਦਰਾਜ);
	ਜੇ(name_asn1 == ਨਿਰਾ){
		ਵਾਪਸ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	ਅਚਲ ਅੱਖਰ *cert_ਪਛਾਣ_ਤੱਤ = (ਅਚਲ ਅੱਖਰ *)ASN1_STRING_get0_ਅੰਕੜੇ(name_asn1);
	ਜੇ(!cert_ਪਛਾਣ_ਤੱਤ || mosquitto_validate_utf8(cert_ਪਛਾਣ_ਤੱਤ, (ਪੂਰਨ)ਲੜੀ_ਲੰਬਾਈ(cert_ਪਛਾਣ_ਤੱਤ))){
		ਵਾਪਸ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	mosquitto_free(context->username);
	context->username = mosquitto_strdup(cert_ਪਛਾਣ_ਤੱਤ);
	ਜੇ(context->username == ਨਿਰਾ){
		ਵਾਪਸ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_NOMEM);
	}
	/* Make sure there isn't an embedded NUL character in the CN */
	ਜੇ((size_t)ASN1_STRING_ਲੰਮਾਈ(name_asn1) != ਲੜੀ_ਲੰਬਾਈ(context->username)){
		ਵਾਪਸ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}

success:
	X509_free(client_cert);
	client_cert = ਨਿਰਾ;
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ ਸੈੱਟ_username_from_cert_subject_name(ਬਣਤਰ mosquitto *context)
{
	X509 *client_cert = ਨਿਰਾ;
	X509_NAME *name = ਨਿਰਾ;

	ਜੇ(get_client_cert_and_subject_name(context, &client_cert, &name)){
		ਵਾਪਸ MOSQ_ERR_AUTH;
	}

	ਅੱਖਰ *subject = ਨਿਰਾ;
	ਅੱਖਰ *ਅੰਕੜੇ_ਸ਼ੁਰੂ = ਨਿਰਾ;
	BIO *subject_bio = ਨਿਰਾ;
	ਲੰਮਾ name_ਲੰਮਾਈ = 0;

	subject_bio = BIO_new(BIO_s_mem());
	X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
	ਅੰਕੜੇ_ਸ਼ੁਰੂ = ਨਿਰਾ;
	name_ਲੰਮਾਈ = BIO_get_mem_ਅੰਕੜੇ(subject_bio, &ਅੰਕੜੇ_ਸ਼ੁਰੂ);
	subject = mosquitto_malloc(ਆਕਾਰ(ਅੱਖਰ)*(size_t)(name_ਲੰਮਾਈ+1));
	ਜੇ(!subject){
		ਵਾਪਸ free_x509_and_BIO_and_send_connack_error(context, client_cert, subject_bio, MOSQ_ERR_NOMEM);
	}
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(subject, ਅੰਕੜੇ_ਸ਼ੁਰੂ, (size_t)name_ਲੰਮਾਈ);
	subject[name_ਲੰਮਾਈ] = '\0';
	BIO_free(subject_bio);

	ਜੇ(mosquitto_validate_utf8(subject, (ਪੂਰਨ)ਲੜੀ_ਲੰਬਾਈ(subject))){
		mosquitto_free(subject);
		ਵਾਪਸ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	context->username = subject;
	ਜੇ(!context->username){
		X509_free(client_cert);
		ਵਾਪਸ MOSQ_ERR_AUTH;
	}

	X509_free(client_cert);
	client_cert = ਨਿਰਾ;
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}
#ਜੇ_ਅੰਤ


ਸਥਿਰ ਪੂਰਨ handle_username_from_cert_options(ਬਣਤਰ mosquitto *context, ਅੱਖਰ **username, ਅੱਖਰ **password, uint16_t password_len)
{
	ਪੂਰਨ rc;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
	ਜੇ(context->listener->ssl_ctx && (context->listener->use_ਪਛਾਣ_ਤੱਤ_as_username || context->listener->use_subject_as_username)){
		/* Don't need the username or password if provided */
		mosquitto_FREE(*username);
		mosquitto_FREE(*password);

		ਜੇ(!context->ssl){
			ਵਾਪਸ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
		}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FINAL_WITH_TLS_PSK
		ਜੇ(context->listener->psk_hint){
			/* Client should have provided an identity to get this far. */
			ਜੇ(!context->username){
				ਵਾਪਸ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ /* FINAL_WITH_TLS_PSK */
		{
			ਜੇ(context->listener->use_ਪਛਾਣ_ਤੱਤ_as_username){
				rc = ਸੈੱਟ_username_from_cert_ਪਛਾਣ_ਤੱਤ(context);
			}ਨਹੀਂਤਾਂ{   /* use_subject_as_username */
				rc = ਸੈੱਟ_username_from_cert_subject_name(context);
			}
			ਜੇ(rc){
				ਵਾਪਸ rc;
			}
		}
	}ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ /* WITH_TLS */
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
		ਜੇ(context->listener->use_ਪਛਾਣ_ਤੱਤ_as_username && context->listener->require_certificate){
			mosquitto_FREE(*username);
			mosquitto_FREE(*password);

			ਜੇ(!context->username){
				ਵਾਪਸ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		{
			/* FIXME - these ensure the mosquitto_clientid() and
			* mosquitto_client_username() functions work, but is hacky */
			context->username = *username;
			context->password = *password;
			context->password_len = password_len;
			*username = ਨਿਰਾ; /* Avoid free() in error: below. */
			*password = ਨਿਰਾ;
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ handle_username_as_clientid_option(ਬਣਤਰ mosquitto *context)
{
	ਜੇ(context->username){
		mosquitto_FREE(context->id);
		context->id = mosquitto_strdup(context->username);
		ਜੇ(!context->id){
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}
	}ਨਹੀਂਤਾਂ{
		uint8_t err_ਕੋਡ = context->protocol == mosq_p_mqtt5
												? (uint8_t)MQTT_RC_NOT_AUTHORIZED
												: (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
		ਵਾਪਸ send__connack_error_and_return(context, err_ਕੋਡ, MOSQ_ERR_AUTH);
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ handle__connect(ਬਣਤਰ mosquitto *context)
{
	ਅੱਖਰ protocol_name[7];
	uint8_t protocol_ਸੰਸਕਰਣ;
	uint8_t connect_flags;
	ਅੱਖਰ *clientid = ਨਿਰਾ;
	ਬਣਤਰ mosquitto *ਮਿਲਿਆ_context;
	ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_all *will_struct = ਨਿਰਾ;
	uint8_t will, will_retain, will_qos, clean_ਸ਼ੁਰੂ;
	uint8_t username_flag, password_flag;
	ਅੱਖਰ *username = ਨਿਰਾ, *password = ਨਿਰਾ;
	uint16_t password_len = 0;
	ਪੂਰਨ rc;
	mosquitto_property *properties = ਨਿਰਾ;
	ਖਾਲੀ *auth_ਅੰਕੜੇ = ਨਿਰਾ;
	uint16_t auth_ਅੰਕੜੇ_len = 0;
	ਖਾਲੀ *auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ = ਨਿਰਾ;
	uint16_t auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ_len = 0;
	bool allow_zero_ਲੰਮਾਈ_clientid;

	ਜੇ(!context->listener){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	/* Don't accept multiple CONNECT commands. */
	ਜੇ(context->state != mosq_cs_new){
		log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Bad client %s:%d sending multiple CONNECT messages.",
				context->ਪਤਾ, context->remote_ਪੋਰਟ);
		rc = MOSQ_ERR_PROTOCOL;
		ਜਾਓ handle_connect_error;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
	ਜੇ(context->ਵਿੱਚ_packet.command == 0x16 && context->listener->ssl_ctx == ਨਿਰਾ){ /* 0x16 is TLS handshake client hello */
		log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client from %s:%d appears to be using TLS to connect to a non-TLS listener.",
				context->ਪਤਾ, context->remote_ਪੋਰਟ);
		rc = MOSQ_ERR_PROTOCOL;
		ਜਾਓ handle_connect_error;
	}
#ਜੇ_ਅੰਤ

	rc = read_protocol_name(context, protocol_name);
	ਜੇ(rc != MOSQ_ERR_SUCCESS){
		ਜਾਓ handle_connect_error;
	}

	rc = read_and_verify_protocol_ਸੰਸਕਰਣ(context, protocol_name, &protocol_ਸੰਸਕਰਣ);
	ਜੇ(rc != MOSQ_ERR_SUCCESS){
		ਜੇ(rc == MOSQ_ERR_MALFORMED_PACKET){
			ਵਾਪਸ rc;
		}
		ਜਾਓ handle_connect_error;
	}

	rc = read_and_verify_connect_flags(context, &connect_flags);
	ਜੇ(rc != MOSQ_ERR_SUCCESS){
		ਜਾਓ handle_connect_error;
	}

	clean_ਸ਼ੁਰੂ = (connect_flags & 0x02) >> 1;
	ਸੈੱਟ_session_expiry_interval(context, clean_ਸ਼ੁਰੂ, protocol_ਸੰਸਕਰਣ);

	rc = read_and_reset_keepalive(context);
	ਜੇ(rc != MOSQ_ERR_SUCCESS){
		ਜਾਓ handle_connect_error;
	}

	rc = read_and_verify_v5_connect_properties(context, &properties, protocol_ਸੰਸਕਰਣ);
	ਜੇ(rc != MOSQ_ERR_SUCCESS){
		ਜਾਓ handle_connect_error;
	}

	will = connect_flags & 0x04;
	will_qos = (connect_flags & 0x18) >> 3;
	will_retain = ((connect_flags & 0x20) == 0x20);
	rc = verify_will_options(context, will, will_qos, will_retain, protocol_ਸੰਸਕਰਣ);
	ਜੇ(rc != MOSQ_ERR_SUCCESS){
		ਜਾਓ handle_connect_error;
	}

	mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_ਵਿਧੀ, &context->auth_ਵਿਧੀ, false);
	mosquitto_property_read_ਦੁਆਧਾਰੀ(properties, MQTT_PROP_AUTHENTICATION_ਅੰਕੜੇ, &auth_ਅੰਕੜੇ, &auth_ਅੰਕੜੇ_len, false);
	mosquitto_property_free_all(&properties);

	ਜੇ(auth_ਅੰਕੜੇ && !context->auth_ਵਿਧੀ){
		send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, ਨਿਰਾ);
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s:%d CONNECT with missing clientid string.",
				context->ਪਤਾ, context->remote_ਪੋਰਟ);
		rc = MOSQ_ERR_PROTOCOL;
		ਜਾਓ handle_connect_error;
	}

	rc = read_and_verify_clientid_from_packet(context, &clientid, &allow_zero_ਲੰਮਾਈ_clientid, clean_ਸ਼ੁਰੂ);
	ਜੇ(rc != MOSQ_ERR_SUCCESS){
		ਜਾਓ handle_connect_error;
	}

	ਜੇ(will){
		rc = will__read(context, clientid, &will_struct, will_qos, will_retain);
		ਜੇ(rc){
			ਜੇ(context->protocol == mosq_p_mqtt5){
				ਜੇ(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
					send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, ਨਿਰਾ);
				}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_MALFORMED_PACKET){
					send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, ਨਿਰਾ);
				}
			}
			ਜਾਓ handle_connect_error;
		}
	}ਨਹੀਂਤਾਂ{
		ਜੇ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
			ਜੇ(will_qos != 0 || will_retain != 0){
				log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT without Will with non-zero QoS (%d) or retain (%d).",
						clientid, will_qos, will_retain);
				rc = MOSQ_ERR_PROTOCOL;
				ਜਾਓ handle_connect_error;
			}
		}
	}

	// Client credentials
	password_flag = connect_flags & 0x40;
	username_flag = connect_flags & 0x80;
	rc = read_and_verify_client_credentials_from_packet(context, &username, username_flag, &password, &password_len, password_flag, clientid);
	ਜੇ(rc != MOSQ_ERR_SUCCESS){
		ਜਾਓ handle_connect_error;
	}

	rc = check_additional_trailing_ਅੰਕੜੇ(context, protocol_ਸੰਸਕਰਣ);
	ਜੇ(rc != MOSQ_ERR_SUCCESS){
		ਜਾਓ handle_connect_error;
	}

	/* Once context->id is set, if we return from this function with an error
	 * we must make sure that context->id is freed and set to NULL, so that the
	 * client isn't erroneously removed from the by_id hash table. */
	context->id = clientid;
	clientid = ਨਿਰਾ;

	/* use_identity_as_username or use_subject_as_username */
	rc = handle_username_from_cert_options(context, &username, &password, password_len);
	ਜੇ(rc != MOSQ_ERR_SUCCESS){
		ਜਾਓ handle_connect_error;
	}

	/* use_username_as_clientid */
	ਜੇ(context->listener->use_username_as_clientid){
		rc = handle_username_as_clientid_option(context);
		ਜੇ(rc != MOSQ_ERR_SUCCESS){
			ਜਾਓ handle_connect_error;
		}
	}

	/* Check for an existing delayed auth check, reject if present */
	HASH_ਲੱਭੋ(hh_id, db.contexts_by_id_delayed_auth, context->id, ਲੜੀ_ਲੰਬਾਈ(context->id), ਮਿਲਿਆ_context);
	ਜੇ(ਮਿਲਿਆ_context){
		rc = MOSQ_ERR_UNKNOWN;
		ਜਾਓ handle_connect_error;
	}

	context->clean_ਸ਼ੁਰੂ = clean_ਸ਼ੁਰੂ;
	context->will = will_struct;
	will_struct = ਨਿਰਾ;

	ਜੇ(context->auth_ਵਿਧੀ){
		rc = mosquitto_security_auth_ਸ਼ੁਰੂ(context, false, auth_ਅੰਕੜੇ, auth_ਅੰਕੜੇ_len, &auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ, &auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ_len);
		mosquitto_FREE(auth_ਅੰਕੜੇ);
		ਜੇ(rc == MOSQ_ERR_SUCCESS){
			ਵਾਪਸ connect__on_authorised(context, auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ, auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ_len);
		}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_AUTH_CONTINUE){
			mosquitto_ਸੈੱਟ_state(context, mosq_cs_authenticating);
			rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ, auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ_len);
			ਸੁਰੱਖਿਅਤ_FREE(auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ);
			ਵਾਪਸ rc;
		}ਨਹੀਂਤਾਂ{
			ਸੁਰੱਖਿਅਤ_FREE(auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ);
			will__clear(context);
			ਜੇ(rc == MOSQ_ERR_AUTH){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ਨਿਰਾ);
				mosquitto_FREE(context->id);
				ਜਾਓ handle_connect_error;
			}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_NOT_SUPPORTED){
				/* Client has requested extended authentication, but we don't support it. */
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_ਵਿਧੀ, ਨਿਰਾ);
				mosquitto_FREE(context->id);
				ਜਾਓ handle_connect_error;
			}ਨਹੀਂਤਾਂ{
				mosquitto_FREE(context->id);
				ਜਾਓ handle_connect_error;
			}
		}
	}ਨਹੀਂਤਾਂ{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
		ਜੇ(context->listener->ssl_ctx && (context->listener->use_ਪਛਾਣ_ਤੱਤ_as_username || context->listener->use_subject_as_username)){
			/* Authentication assumed to be cleared */
		}ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		{
			rc = mosquitto_basic_auth(context);
			ਚੋਣ(rc){
				ਹਾਲਤ MOSQ_ERR_SUCCESS:
					ਰੋਕੋ;
				ਹਾਲਤ MOSQ_ERR_AUTH_DELAYED:
					mosquitto_ਸੈੱਟ_state(context, mosq_cs_delayed_auth);
					HASH_ਜੋੜੋ_KEYPTR(hh_id, db.contexts_by_id_delayed_auth, context->id, ਲੜੀ_ਲੰਬਾਈ(context->id), context);
					ਵਾਪਸ MOSQ_ERR_SUCCESS;
					ਰੋਕੋ;
				ਹਾਲਤ MOSQ_ERR_AUTH:
					ਜੇ(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ਨਿਰਾ);
					}ਨਹੀਂਤਾਂ{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ਨਿਰਾ);
					}
					ਜਾਓ handle_connect_error;
					ਰੋਕੋ;
				ਹਾਲਤ MOSQ_ERR_UNSPECIFIED:
				ਹਾਲਤ MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
				ਹਾਲਤ MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
				ਹਾਲਤ MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
				ਹਾਲਤ MOSQ_ERR_SERVER_UNAVAILABLE:
				ਹਾਲਤ MOSQ_ERR_SERVER_BUSY:
				ਹਾਲਤ MOSQ_ERR_BANNED:
				ਹਾਲਤ MOSQ_ERR_BAD_AUTHENTICATION_ਵਿਧੀ:
				ਹਾਲਤ MOSQ_ERR_CONNECTION_ਦਰ_EXCEEDED:
					ਜੇ(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, (uint8_t)rc, ਨਿਰਾ);
					}ਨਹੀਂਤਾਂ{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ਨਿਰਾ);
					}
					ਜਾਓ handle_connect_error;
					ਰੋਕੋ;
				ਮੂਲ:
					rc = MOSQ_ERR_UNKNOWN;
					ਜਾਓ handle_connect_error;
					ਰੋਕੋ;
			}
		}
		ਵਾਪਸ connect__on_authorised(context, ਨਿਰਾ, 0);
	}


handle_connect_error:
	mosquitto_property_free_all(&properties);
	mosquitto_FREE(auth_ਅੰਕੜੇ);
	mosquitto_FREE(clientid);
	mosquitto_FREE(username);
	mosquitto_FREE(password);
	ਜੇ(will_struct){
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct->ਸੁਨੇਹਾ.payload);
		mosquitto_FREE(will_struct->ਸੁਨੇਹਾ.topic);
		mosquitto_FREE(will_struct);
	}
	will__clear(context);
	/* We return an error here which means the client is freed later on. */
	context->clean_ਸ਼ੁਰੂ = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_ਤੁਰੰਤ;
	context->will_delay_interval = 0;
	ਵਾਪਸ rc;
}
