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

#ضمن "config.h"

#ضمن <stdio.h>
#ضمن <string.h>
#ضمن <utlist.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "packet_mosq.h"
#ضمن "property_mosq.h"
#ضمن "send_mosq.h"
#ضمن "sys_tree.h"
#ضمن "tls_mosq.h"
#ضمن "util_mosq.h"
#ضمن "will_mosq.h"

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#جيڪڏهن_پورو


ساكن حرف nibble_منزل_hex(uint8_t قدر)
{
	إذا(قدر < 0x0A){
		أرجع (حرف)('0'+قدر);
	}وإلا{
		أرجع (حرف)(65 /*'A'*/ +قدر-10);
	}
}


ساكن حرف *clientid_gen(uint16_t *idlen, ثابت حرف *auto_id_اڳياڙي, uint16_t auto_id_اڳياڙي_len)
{
	حرف *clientid;
	uint8_t rnd[16];
	صحيح pos;

	إذا(mosquitto_getrandom(rnd, 16)){
		أرجع عدم;
	}

	*idlen = (uint16_t)(auto_id_اڳياڙي_len + 36);

	clientid = (حرف *)mosquitto_calloc((size_t)(*idlen) + 1, حجم(حرف));
	إذا(!clientid){
		أرجع عدم;
	}
	إذا(auto_id_اڳياڙي){
		يادگيري_نقل(clientid, auto_id_اڳياڙي, auto_id_اڳياڙي_len);
	}

	pos = 0;
	لكل(صحيح i=0; i<16; i++){
		clientid[auto_id_اڳياڙي_len + pos + 0] = nibble_منزل_hex(rnd[i] & 0x0F);
		clientid[auto_id_اڳياڙي_len + pos + 1] = nibble_منزل_hex((rnd[i] >> 4) & 0x0F);
		pos += 2;
		إذا(pos == 8 || pos == 13 || pos == 18 || pos == 23){
			clientid[auto_id_اڳياڙي_len + pos] = '-';
			pos++;
		}
	}

	أرجع clientid;
}


صحيح connect__on_authorised(بنية mosquitto *context, فراغ *auth_معلومات_ٻاهريون, uint16_t auth_معلومات_ٻاهريون_len)
{
	بنية mosquitto *مليو_context;
	بنية mosquitto__subleaf *پن;
	mosquitto_property *connack_props = عدم;
	uint8_t connect_ack = 0;
	صحيح rc;
	صحيح اندريون_quota, ٻاهريون_quota;
	uint16_t اندريون_وڌ_کان_وڌ, ٻاهريون_وڌ_کان_وڌ;

	/* Find if this client already has an entry. This must be done *after* any security checks. */
	HASH_ڳوليو(hh_id, db.contexts_by_id, context->id, لڙي_ڊيگهه(context->id), مليو_context);
	إذا(مليو_context){
		/* Found a matching client */
		إذا(!net__is_connected(مليو_context)){
			/* Client is reconnecting after a disconnect */
			/* FIXME - does anything need to be done here? */
		}وإلا{
			/* Client is already connected, disconnect old version. This is
			 * done in context__cleanup() below. */
		}

		إذا(context->clean_شروعات == true){
			sub__clean_session(مليو_context);
			مليو_context->session_expiry_interval = MQTT_SESSION_EXPIRY_فوري;
			plugin_persist__handle_client_delete(مليو_context);
		}
		context->is_persisted = مليو_context->is_persisted;
		مليو_context->is_persisted = false; /* stops persistence for context being removed */

		إذا(context->clean_شروعات == false && مليو_context->session_expiry_interval != MQTT_SESSION_EXPIRY_فوري){
			إذا(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
				connect_ack |= 0x01;
			}

			إذا(مليو_context->msgs_اندريون.inflight || مليو_context->msgs_اندريون.queued
					|| مليو_context->msgs_ٻاهريون.inflight || مليو_context->msgs_ٻاهريون.queued){

				اندريون_quota = context->msgs_اندريون.inflight_quota;
				ٻاهريون_quota = context->msgs_ٻاهريون.inflight_quota;
				اندريون_وڌ_کان_وڌ = context->msgs_اندريون.inflight_وڌ_کان_وڌ;
				ٻاهريون_وڌ_کان_وڌ = context->msgs_ٻاهريون.inflight_وڌ_کان_وڌ;

				يادگيري_نقل(&context->msgs_اندريون, &مليو_context->msgs_اندريون, حجم(بنية mosquitto_نياپو_معلومات));
				يادگيري_نقل(&context->msgs_ٻاهريون, &مليو_context->msgs_ٻاهريون, حجم(بنية mosquitto_نياپو_معلومات));
				context->آخري_cmsg_id = مليو_context->آخري_cmsg_id;

				memset(&مليو_context->msgs_اندريون, 0, حجم(بنية mosquitto_نياپو_معلومات));
				memset(&مليو_context->msgs_ٻاهريون, 0, حجم(بنية mosquitto_نياپو_معلومات));

				context->msgs_اندريون.inflight_quota = اندريون_quota;
				context->msgs_ٻاهريون.inflight_quota = ٻاهريون_quota;
				context->msgs_اندريون.inflight_وڌ_کان_وڌ = اندريون_وڌ_کان_وڌ;
				context->msgs_ٻاهريون.inflight_وڌ_کان_وڌ = ٻاهريون_وڌ_کان_وڌ;

				db_پيغام_reconnect_reset(context);
			}
			context->subs = مليو_context->subs;
			مليو_context->subs = عدم;
			context->subs_صلاحيت = مليو_context->subs_صلاحيت;
			context->subs_ڳڻپ = مليو_context->subs_ڳڻپ;
			مليو_context->subs_صلاحيت = 0;
			مليو_context->subs_ڳڻپ = 0;
			context->آخري_mid = مليو_context->آخري_mid;

			لكل(صحيح i=0; i<context->subs_صلاحيت; i++){
				إذا(context->subs[i]){
					پن = context->subs[i]->hier->subs;
					طالما(پن){
						إذا(پن->context == مليو_context){
							پن->context = context;
						}
						پن = پن->ايندڙ;
					}

					إذا(context->subs[i]->shared){
						پن = context->subs[i]->shared->subs;
						طالما(پن){
							إذا(پن->context == مليو_context){
								پن->context = context;
							}
							پن = پن->ايندڙ;
						}
					}
				}
			}
		}

		إذا((مليو_context->protocol == mosq_p_mqtt5 && مليو_context->session_expiry_interval == MQTT_SESSION_EXPIRY_فوري)
				|| (مليو_context->protocol != mosq_p_mqtt5 && مليو_context->clean_شروعات == true)
				|| (context->clean_شروعات == true)
				){

			context__send_will(مليو_context);
		}

		session_expiry__remove(مليو_context);
		will_delay__remove(مليو_context);
		will__clear(مليو_context);

		مليو_context->clean_شروعات = true;
		مليو_context->session_expiry_interval = MQTT_SESSION_EXPIRY_فوري;
		mosquitto_سيٽ_state(مليو_context, mosq_cs_duplicate);
		إذا(مليو_context->protocol == mosq_p_mqtt5){
			send__disconnect(مليو_context, MQTT_RC_SESSION_ورتل_OVER, عدم);
		}
		do_disconnect(مليو_context, MOSQ_ERR_SESSION_ورتل_OVER);
	}

	إذا(db.config->global_max_clients > 0 && HASH_CNT(hh_id, db.contexts_by_id) >= (بنانشان صحيح)db.config->global_max_clients){
		إذا(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_SERVER_BUSY, عدم);
		}
		rc = MOSQ_ERR_INVAL;
		اذهب error;
	}

	إذا(db.config->connection_messages == true){
		إذا(context->is_bridge){
			إذا(context->username){
				log__printf(عدم, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->پتو, context->remote_پورٽ, context->id, context->protocol, context->clean_شروعات, context->keepalive, context->username);
			}وإلا{
				log__printf(عدم, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d).",
						context->پتو, context->remote_پورٽ, context->id, context->protocol, context->clean_شروعات, context->keepalive);
			}
		}وإلا{
			إذا(context->username){
				log__printf(عدم, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->پتو, context->remote_پورٽ, context->id, context->protocol, context->clean_شروعات, context->keepalive, context->username);
			}وإلا{
				log__printf(عدم, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d).",
						context->پتو, context->remote_پورٽ, context->id, context->protocol, context->clean_شروعات, context->keepalive);
			}
		}

		إذا(context->will){
			log__printf(عدم, MOSQ_LOG_DEBUG, "Will message specified (%ld bytes) (r%d, q%d).",
					(ڊگهو)context->will->نياپو.payloadlen,
					context->will->نياپو.retain,
					context->will->نياپو.qos);

			log__printf(عدم, MOSQ_LOG_DEBUG, "\t%s", context->will->نياپو.topic);
		}وإلا{
			log__printf(عدم, MOSQ_LOG_DEBUG, "No will message specified.");
		}
	}
#جيڪڏهن_وصف_ٿيل WITH_TLS
	إذا(context->ssl){
		log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s negotiated %s cipher %s",
				context->id,
				SSL_get_cipher_نسخو(context->ssl),
				SSL_get_cipher_name(context->ssl));
	}
#جيڪڏهن_پورو

	context->ping_t = 0;
	context->is_dropping = false;

	/* Remove any queued messages that are no longer allowed through ACL,
	 * assuming a possible change of username. */
	db_check_acl_جو_all_messages(context);
	context_جوڙيو_منزل_by_id(context);

#جيڪڏهن_وصف_ٿيل WITH_PERSISTENCE
	إذا(!context->clean_شروعات){
		db.persistence_changes++;
	}
#جيڪڏهن_پورو
	context->max_qos = context->listener->max_qos;

	إذا(db.config->max_keepalive &&
			(context->keepalive > db.config->max_keepalive || context->keepalive == 0)){

		keepalive__remove(context);
		context->keepalive = db.config->max_keepalive;
		keepalive_جوڙيو(context);
		إذا(context->protocol == mosq_p_mqtt5){
			إذا(mosquitto_property_جوڙيو_int16(&connack_props, MQTT_PROP_SERVER_KEEP_ALIVE, context->keepalive)){
				rc = MOSQ_ERR_NOMEM;
				اذهب error;
			}
		}وإلا{
			send__connack(context, connect_ack, CONNACK_REFUSED_IDENTIFIER_REJECTED, عدم);
			rc = MOSQ_ERR_INVAL;
			اذهب error;
		}
	}


	إذا(context->protocol == mosq_p_mqtt5){
		إذا(context->listener->max_topic_alias > 0){
			إذا(mosquitto_property_جوڙيو_int16(&connack_props, MQTT_PROP_TOPIC_ALIAS_وڌ_کان_وڌ, context->listener->max_topic_alias)){
				rc = MOSQ_ERR_NOMEM;
				اذهب error;
			}
		}
		إذا(context->assigned_id){
			إذا(mosquitto_property_جوڙيو_string(&connack_props, MQTT_PROP_ASSIGNED_CLIENT_IDENTIFIER, context->id)){
				rc = MOSQ_ERR_NOMEM;
				اذهب error;
			}
		}
		إذا(context->auth_طريقو){
			إذا(mosquitto_property_جوڙيو_string(&connack_props, MQTT_PROP_AUTHENTICATION_طريقو, context->auth_طريقو)){
				rc = MOSQ_ERR_NOMEM;
				اذهب error;
			}

			إذا(auth_معلومات_ٻاهريون && auth_معلومات_ٻاهريون_len > 0){
				إذا(mosquitto_property_جوڙيو_ٻٽو(&connack_props, MQTT_PROP_AUTHENTICATION_معلومات, auth_معلومات_ٻاهريون, auth_معلومات_ٻاهريون_len)){
					rc = MOSQ_ERR_NOMEM;
					اذهب error;
				}
			}
		}
	}
	محفوظ_FREE(auth_معلومات_ٻاهريون);

	mosquitto_سيٽ_state(context, mosq_cs_سرگرم);
	rc = send__connack(context, connect_ack, CONNACK_ACCEPTED, connack_props);
	mosquitto_property_free_all(&connack_props);
	إذا(rc){
		أرجع rc;
	}
	db__expire_all_messages(context);
	rc = db_پيغام_write_queued_ٻاهريون(context);
	إذا(rc){
		أرجع rc;
	}
	rc = db_پيغام_write_inflight_ٻاهريون_all(context);

	إذا(rc == MOSQ_ERR_SUCCESS){
		plugin__handle_connect(context);

		إذا(context->session_expiry_interval != MQTT_SESSION_EXPIRY_فوري){
			plugin_persist_handle_client_جوڙيو(context);
		}وإلا إذا(context->will){
			plugin_persist_handle_will_جوڙيو(context);
		}
	}
	أرجع rc;
error:
	محفوظ_FREE(auth_معلومات_ٻاهريون);
	mosquitto_property_free_all(&connack_props);
	أرجع rc;
}


ساكن صحيح will__read(بنية mosquitto *context, ثابت حرف *clientid, بنية mosquitto_پيغام_all **will, uint8_t will_qos, صحيح will_retain)
{
	صحيح rc = MOSQ_ERR_SUCCESS;
	size_t slen;
	uint16_t tlen;
	بنية mosquitto_پيغام_all *will_struct = عدم;
	حرف *will_topic_mount = عدم;
	uint16_t payloadlen;
	mosquitto_property *properties = عدم;

	will_struct = mosquitto_calloc(1, حجم(بنية mosquitto_پيغام_all));
	إذا(!will_struct){
		rc = MOSQ_ERR_NOMEM;
		اذهب error_cleanup;
	}
	إذا(context->protocol == PROTOCOL_نسخو_v5){
		rc = property__read_all(CMD_WILL, &context->اندريون_packet, &properties);
		إذا(rc){
			اذهب error_cleanup;
		}

		rc = property__process_will(context, will_struct, &properties);
		mosquitto_property_free_all(&properties);
		إذا(rc){
			اذهب error_cleanup;
		}
	}
	rc = packet__read_string(&context->اندريون_packet, &will_struct->نياپو.topic, &tlen);
	إذا(rc){
		اذهب error_cleanup;
	}
	إذا(!tlen){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Will with empty topic.",
				context->id);
		rc = MOSQ_ERR_PROTOCOL;
		اذهب error_cleanup;
	}

	إذا(context->listener->mount_نقطو){
		slen = لڙي_ڊيگهه(context->listener->mount_نقطو) + لڙي_ڊيگهه(will_struct->نياپو.topic) + 1;
		will_topic_mount = mosquitto_malloc(slen+1);
		إذا(!will_topic_mount){
			rc = MOSQ_ERR_NOMEM;
			اذهب error_cleanup;
		}

		snprintf(will_topic_mount, slen, "%s%s", context->listener->mount_نقطو, will_struct->نياپو.topic);
		will_topic_mount[slen] = '\0';

		mosquitto_FREE(will_struct->نياپو.topic);
		will_struct->نياپو.topic = will_topic_mount;
	}

	إذا(!لڙي_ڀيٽ(will_struct->نياپو.topic, "$CONTROL/", لڙي_ڊيگهه("$CONTROL/"))){
		rc = MOSQ_ERR_ACL_DENIED;
		اذهب error_cleanup;
	}
	rc = mosquitto_pub_topic_check(will_struct->نياپو.topic);
	إذا(rc){
		اذهب error_cleanup;
	}

	rc = packet__read_uint16(&context->اندريون_packet, &payloadlen);
	إذا(rc){
		اذهب error_cleanup;
	}

	will_struct->نياپو.payloadlen = payloadlen;
	إذا(will_struct->نياپو.payloadlen > 0){
		إذا(db.config->پيغام_سائيز_حد_بندي && will_struct->نياپو.payloadlen > (صحيح)db.config->پيغام_سائيز_حد_بندي){
			log__printf(عدم, MOSQ_LOG_DEBUG, "Client %s connected with too large Will payload", clientid);
			إذا(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_PACKET_TOO_LARGE, عدم);
			}وإلا{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, عدم);
			}
			rc = MOSQ_ERR_PAYLOAD_سائيز;
			اذهب error_cleanup;
		}
		will_struct->نياپو.payload = mosquitto_malloc((size_t)will_struct->نياپو.payloadlen);
		إذا(!will_struct->نياپو.payload){
			rc = MOSQ_ERR_NOMEM;
			اذهب error_cleanup;
		}

		rc = packet_read_بائيٽون(&context->اندريون_packet, will_struct->نياپو.payload, (uint32_t)will_struct->نياپو.payloadlen);
		إذا(rc){
			اذهب error_cleanup;
		}
	}

	will_struct->نياپو.qos = will_qos;
	will_struct->نياپو.retain = will_retain;

	*will = will_struct;
	أرجع MOSQ_ERR_SUCCESS;

error_cleanup:
	إذا(will_struct){
		mosquitto_FREE(will_struct->نياپو.topic);
		mosquitto_FREE(will_struct->نياپو.payload);
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct);
	}
	أرجع rc;
}


ساكن صحيح check_protocol_نسخو(بنية mosquitto__listener *listener, صحيح protocol_نسخو)
{
	/* Allow bridge protocol as well. */
	protocol_نسخو &= 0x7F;

	إذا((protocol_نسخو == 3 && listener->disable_protocol_v3 == false)
			|| (protocol_نسخو == 4 && listener->disable_protocol_v4 == false)
			|| (protocol_نسخو == 5 && listener->disable_protocol_v5 == false)
			){

		أرجع MOSQ_ERR_SUCCESS;
	}وإلا{
		أرجع MOSQ_ERR_NOT_SUPPORTED;
	}
}


مضمن ساكن صحيح send__connack_error_and_return(بنية mosquitto *context, uint8_t err_ڪوڊ, صحيح rc)
{
	send__connack(context, 0, err_ڪوڊ, عدم);
	أرجع rc;
}


مضمن ساكن صحيح send__connack_bad_username_or_password_error(بنية mosquitto *context, صحيح rc)
{
	uint8_t err_ڪوڊ  = context->protocol == mosq_p_mqtt5
									? (uint8_t)MQTT_RC_BAD_USERNAME_OR_PASSWORD
									: (uint8_t)CONNACK_REFUSED_BAD_USERNAME_PASSWORD;
	أرجع send__connack_error_and_return(context, err_ڪوڊ, rc);
}


ساكن صحيح read_protocol_name(بنية mosquitto *context, حرف protocol_name[7])
{
	/* Read protocol name as length then bytes rather than with read_string
	 * because the length is fixed and we can check that. Removes the need
	 * for another malloc as well. */

	uint16_t slen = 0;

	إذا(packet__read_uint16(&context->اندريون_packet, &slen)){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string length.",
				context->پتو, context->remote_پورٽ);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	إذا(slen != 4 /* MQTT */ && slen != 6 /* MQIsdp */){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with incorrect protocol string length (%d).",
				context->پتو, context->remote_پورٽ, slen);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	إذا(packet_read_بائيٽون(&context->اندريون_packet, protocol_name, slen)){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string.",
				context->پتو, context->remote_پورٽ);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	protocol_name[slen] = '\0';

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح read_and_verify_protocol_نسخو(بنية mosquitto *context, ثابت حرف *protocol_name,
		uint8_t *protocol_نسخو)
{
	uint8_t tmp_protocol_نسخو = 0;
	إذا(packet__read_byte(&context->اندريون_packet, &tmp_protocol_نسخو)){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol version.",
				context->پتو, context->remote_پورٽ);
		أرجع MOSQ_ERR_PROTOCOL;
	}

	إذا(check_protocol_نسخو(context->listener, tmp_protocol_نسخو)){
		إذا(tmp_protocol_نسخو == 3 || tmp_protocol_نسخو == 4){
			context->protocol = mosq_p_mqtt311;
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_نسخو, عدم);
		}وإلا{
			context->protocol = mosq_p_mqtt5;
			send__connack(context, 0, MQTT_RC_UNSUPPORTED_PROTOCOL_نسخو, عدم);
		}
		أرجع MOSQ_ERR_NOT_SUPPORTED;
	}

	إذا(!strcmp(protocol_name, PROTOCOL_NAME_v31)){
		إذا((tmp_protocol_نسخو&0x7F) != PROTOCOL_نسخو_v31){
			إذا(db.config->connection_messages == true){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->پتو, context->remote_پورٽ, tmp_protocol_نسخو);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_نسخو, عدم);
			أرجع MOSQ_ERR_PROTOCOL;
		}
		context->protocol = mosq_p_mqtt31;
		إذا((tmp_protocol_نسخو&0x80) == 0x80){
			context->is_bridge = true;
		}
	}وإلا إذا(!strcmp(protocol_name, PROTOCOL_NAME)){
		إذا((tmp_protocol_نسخو&0x7F) == PROTOCOL_نسخو_v311){
			context->protocol = mosq_p_mqtt311;

			إذا((tmp_protocol_نسخو&0x80) == 0x80){
				context->is_bridge = true;
			}
		}وإلا إذا((tmp_protocol_نسخو&0x7F) == PROTOCOL_نسخو_v5){
			context->protocol = mosq_p_mqtt5;
		}وإلا{
			إذا(db.config->connection_messages == true){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->پتو, context->remote_پورٽ, tmp_protocol_نسخو);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_نسخو, عدم);
			أرجع MOSQ_ERR_PROTOCOL;
		}
		إذا((context->اندريون_packet.command&0x0F) != 0x00){
			/* Reserved flags not set to 0, must disconnect. */
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero reserved flags (%02X).",
					context->پتو, context->remote_پورٽ, context->اندريون_packet.command);
			أرجع MOSQ_ERR_PROTOCOL;
		}
	}وإلا{
		إذا(db.config->connection_messages == true){
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol \"%s\".",
					context->پتو, context->remote_پورٽ, protocol_name);
		}
		أرجع MOSQ_ERR_PROTOCOL;
	}
	إذا((tmp_protocol_نسخو&0x7F) != PROTOCOL_نسخو_v31 && context->اندريون_packet.command != CMD_CONNECT){
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}

	*protocol_نسخو = tmp_protocol_نسخو;
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح read_and_verify_connect_flags(بنية mosquitto *context, uint8_t *connect_flags)
{
	إذا(packet__read_byte(&context->اندريون_packet, connect_flags)){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing connect flags.",
				context->پتو, context->remote_پورٽ);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	إذا(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		إذا((*connect_flags & 0x01) != 0x00){
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero connect reserved flag (%02X).",
					context->پتو, context->remote_پورٽ, *connect_flags);
			أرجع MOSQ_ERR_PROTOCOL;
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن فراغ سيٽ_session_expiry_interval(بنية mosquitto *context, uint8_t clean_شروعات, uint8_t protocol_نسخو)
{
	/* session_expiry_interval will be overridden if the properties are read later */
	إذا(clean_شروعات == false && protocol_نسخو != PROTOCOL_نسخو_v5){
		/* v3* has clean_start == false mean the session never expires */
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
	}وإلا{
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_فوري;
	}
}


ساكن صحيح read_and_reset_keepalive(بنية mosquitto *context)
{
	/* _remove here because net__socket_accept() uses _add and we must have the
	 * correct keepalive value */
	keepalive__remove(context);

	إذا(packet__read_uint16(&context->اندريون_packet, &(context->keepalive))){
		log__printf(عدم, MOSQ_LOG_INFO, "%s sent CONNECT with missing keepalive.",
				context->id);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	keepalive_جوڙيو(context);

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح read_and_verify_v5_connect_properties(بنية mosquitto *context, mosquitto_property **properties, uint8_t protocol_نسخو)
{
	صحيح rc;

	إذا(protocol_نسخو == PROTOCOL_نسخو_v5){
		rc = property__read_all(CMD_CONNECT, &context->اندريون_packet, properties);
		إذا(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
			send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, عدم);
		}وإلا إذا(rc == MOSQ_ERR_MALFORMED_PACKET){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, عدم);
		}
		إذا(rc){
			أرجع rc;
		}
	}
	rc = property__process_connect(context, properties);
	إذا(rc != MOSQ_ERR_SUCCESS){
		أرجع send__connack_error_and_return(context, MQTT_RC_PROTOCOL_ERROR, rc);
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح verify_will_options(بنية mosquitto *context, uint8_t will, uint8_t will_qos, uint8_t will_retain, uint8_t protocol_نسخو)
{
	إذا(will_qos == 3){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid Will QoS (%d).",
				context->پتو, context->remote_پورٽ, will_qos);
		أرجع MOSQ_ERR_PROTOCOL;
	}

	إذا(will && will_retain && db.config->retain_موجود == false){
		إذا(protocol_نسخو == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_RETAIN_NOT_SUPPORTED, عدم);
		}
		أرجع MOSQ_ERR_NOT_SUPPORTED;
	}

	إذا(will && will_qos > context->listener->max_qos){
		إذا(protocol_نسخو == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_QOS_NOT_SUPPORTED, عدم);
		}
		أرجع MOSQ_ERR_NOT_SUPPORTED;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح handle_zero_ڊيگهائي_clientid(بنية mosquitto *context, حرف **clientid, bool *allow_zero_ڊيگهائي_clientid,
		uint8_t clean_شروعات)
{
	إذا(context->protocol == mosq_p_mqtt31){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: v3.1 CONNECT with zero length clientid.",
				context->پتو, context->remote_پورٽ);
		send__connack(context, 0, CONNACK_REFUSED_IDENTIFIER_REJECTED, عدم);
		أرجع MOSQ_ERR_PROTOCOL;
	}

	/* mqtt311/mqtt5 */
	mosquitto_FREE(*clientid);

	إذا(db.config->في_listener_settings){
		*allow_zero_ڊيگهائي_clientid = context->listener->security_options->allow_zero_ڊيگهائي_clientid;
	}وإلا{
		*allow_zero_ڊيگهائي_clientid = db.config->security_options.allow_zero_ڊيگهائي_clientid;
	}

	إذا((context->protocol == mosq_p_mqtt311 && clean_شروعات == 0) || *allow_zero_ڊيگهائي_clientid == false){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with zero length clientid when forbidden.",
				context->پتو, context->remote_پورٽ);
		uint8_t err_ڪوڊ = context->protocol == mosq_p_mqtt311 ? (uint8_t)CONNACK_REFUSED_IDENTIFIER_REJECTED : (uint8_t)MQTT_RC_UNSPECIFIED;
		أرجع send__connack_error_and_return(context, err_ڪوڊ, MOSQ_ERR_PROTOCOL);
	}

	*clientid = clientid_gen(&(uint16_t){0}, context->listener->security_options->auto_id_اڳياڙي,
			context->listener->security_options->auto_id_اڳياڙي_len);
	إذا(*clientid == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}
	context->assigned_id = true;

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح check_clientid_prefixes(بنية mosquitto *context, ثابت حرف *clientid)
{
	إذا(db.config->clientid_prefixes){
		إذا(لڙي_ڀيٽ(db.config->clientid_prefixes, clientid, لڙي_ڊيگهه(db.config->clientid_prefixes))){
			uint8_t err_ڪوڊ = context->protocol == mosq_p_mqtt5 ? (uint8_t)MQTT_RC_NOT_AUTHORIZED : (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
			أرجع send__connack_error_and_return(context, err_ڪوڊ, MOSQ_ERR_AUTH);
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح read_and_verify_clientid_from_packet(بنية mosquitto *context, حرف **clientid,
		bool *allow_zero_ڊيگهائي_clientid, uint8_t clean_شروعات)
{
	صحيح rc;
	uint16_t slen;

	rc = packet__read_string(&context->اندريون_packet, clientid, &slen);
	إذا(rc == MOSQ_ERR_MALFORMED_UTF8){
		إذا(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_CLIENTID_NOT_VALID, عدم);
		}
		أرجع MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID;
	}وإلا إذا(rc > 0){
		أرجع MOSQ_ERR_PROTOCOL;
	}

	إذا(slen == 0){
		rc = handle_zero_ڊيگهائي_clientid(context, clientid, allow_zero_ڊيگهائي_clientid, clean_شروعات);
		إذا(rc != MOSQ_ERR_SUCCESS){
			أرجع rc;
		}
	}

	rc = check_clientid_prefixes(context, *clientid);
	إذا(rc != MOSQ_ERR_SUCCESS){
		أرجع rc;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح سيٽ_username_from_packet(بنية mosquitto *context, حرف **username, ثابت حرف *clientid)
{
	صحيح rc;

	rc = packet__read_string(&context->اندريون_packet, username, &(uint16_t){0});
	إذا(rc == MOSQ_ERR_NOMEM){
		أرجع MOSQ_ERR_NOMEM;
	}
	إذا(rc != MOSQ_ERR_SUCCESS){
		إذا(context->protocol == mosq_p_mqtt31){
			/* Username flag given, but no username. Ignore. */
			/* NOTE: Removed setting of username_flag to zero as it is unused afterwards */
		}وإلا{
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with username flag but no username.",
					clientid);
			أرجع MOSQ_ERR_PROTOCOL;
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح سيٽ_password_from_packet(بنية mosquitto *context, حرف **password, uint16_t *password_len, ثابت حرف *clientid)
{
	صحيح rc;

	rc = packet_read_ٻٽو(&context->اندريون_packet, (uint8_t **)password, password_len);
	إذا(rc == MOSQ_ERR_NOMEM){
		أرجع MOSQ_ERR_NOMEM;
	}

	إذا(rc == MOSQ_ERR_MALFORMED_PACKET){
		إذا(context->protocol == mosq_p_mqtt31){
			/* Password flag given, but no password. Ignore. */
		}وإلا{
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with password flag but no password.",
					clientid);
			أرجع MOSQ_ERR_PROTOCOL;
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح read_and_verify_client_credentials_from_packet(بنية mosquitto *context,
		حرف **username, uint8_t username_flag,
		حرف **password, uint16_t *password_len, uint8_t password_flag,
		ثابت حرف *clientid)
{
	صحيح rc;

	إذا(username_flag){
		rc = سيٽ_username_from_packet(context, username, clientid);
		إذا(rc != MOSQ_ERR_SUCCESS){
			أرجع rc;
		}
	}وإلا{
		إذا(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
			إذا(password_flag){
				/* username_flag == 0 && password_flag == 1 is forbidden */
				log__printf(عدم, MOSQ_LOG_ERR, "Protocol error from %s: password without username, closing connection.", clientid);
				أرجع MOSQ_ERR_PROTOCOL;
			}
		}
	}
	إذا(password_flag){
		rc = سيٽ_password_from_packet(context, password, password_len, clientid);
		إذا(rc != MOSQ_ERR_SUCCESS){
			أرجع rc;
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح check_additional_trailing_معلومات(بنية mosquitto *context, uint8_t protocol_نسخو)
{
	إذا(context->اندريون_packet.pos != context->اندريون_packet.بچيل_ڊيگهائي){
		/* Surplus data at end of packet, this must be an error. */
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->اندريون_packet.pos, context->اندريون_packet.بچيل_ڊيگهائي);
		إذا(protocol_نسخو == PROTOCOL_نسخو_v5){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, عدم);
		}
		أرجع MOSQ_ERR_PROTOCOL;
	}

	أرجع MOSQ_ERR_SUCCESS;
}

#جيڪڏهن_وصف_ٿيل WITH_TLS


مضمن ساكن صحيح get_client_cert_and_subject_name(بنية mosquitto *context, X509 **client_cert, X509_NAME **name)
{
	*client_cert = SSL_get_peer_certificate(context->ssl);
	إذا(*client_cert == عدم){
		أرجع send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	*name = X509_get_subject_name(*client_cert);
	إذا(*name == عدم){
		X509_free(*client_cert);
		أرجع send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	أرجع MOSQ_ERR_SUCCESS;
}


مضمن ساكن صحيح free_x509_and_send_connack_error(بنية mosquitto *context, X509 *client_cert, صحيح rc)
{
	X509_free(client_cert);
	أرجع send__connack_bad_username_or_password_error(context, rc);
}


مضمن ساكن صحيح free_x509_and_BIO_and_send_connack_error(بنية mosquitto *context, X509 *client_cert,
		BIO *subject_name, صحيح rc)
{
	BIO_free(subject_name);
	أرجع free_x509_and_send_connack_error(context, client_cert, rc);
}


ساكن صحيح سيٽ_username_from_cert_سڃاڻپ_عنصر(بنية mosquitto *context)
{
	X509 *client_cert = عدم;
	X509_NAME *name = عدم;

	إذا(get_client_cert_and_subject_name(context, &client_cert, &name)){
		أرجع MOSQ_ERR_AUTH;
	}

	صحيح i = -1;
	X509_NAME_داخلا *name_داخلا = عدم;

	i = X509_NAME_get_اشاريو_by_NID(name, NID_commonName, -1);
	إذا(i == -1){
		أرجع free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	name_داخلا = X509_NAME_get_داخلا(name, i);
	إذا(!name_داخلا){
		اذهب success;
	}

	ASN1_STRING *name_asn1 = عدم;
	name_asn1 = X509_NAME_داخلا_get_معلومات(name_داخلا);
	إذا(name_asn1 == عدم){
		أرجع free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	ثابت حرف *cert_سڃاڻپ_عنصر = (ثابت حرف *)ASN1_STRING_get0_معلومات(name_asn1);
	إذا(!cert_سڃاڻپ_عنصر || mosquitto_validate_utf8(cert_سڃاڻپ_عنصر, (صحيح)لڙي_ڊيگهه(cert_سڃاڻپ_عنصر))){
		أرجع free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	mosquitto_free(context->username);
	context->username = mosquitto_strdup(cert_سڃاڻپ_عنصر);
	إذا(context->username == عدم){
		أرجع free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_NOMEM);
	}
	/* Make sure there isn't an embedded NUL character in the CN */
	إذا((size_t)ASN1_STRING_ڊيگهائي(name_asn1) != لڙي_ڊيگهه(context->username)){
		أرجع free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}

success:
	X509_free(client_cert);
	client_cert = عدم;
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح سيٽ_username_from_cert_subject_name(بنية mosquitto *context)
{
	X509 *client_cert = عدم;
	X509_NAME *name = عدم;

	إذا(get_client_cert_and_subject_name(context, &client_cert, &name)){
		أرجع MOSQ_ERR_AUTH;
	}

	حرف *subject = عدم;
	حرف *معلومات_شروعات = عدم;
	BIO *subject_bio = عدم;
	ڊگهو name_ڊيگهائي = 0;

	subject_bio = BIO_new(BIO_s_mem());
	X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
	معلومات_شروعات = عدم;
	name_ڊيگهائي = BIO_get_mem_معلومات(subject_bio, &معلومات_شروعات);
	subject = mosquitto_malloc(حجم(حرف)*(size_t)(name_ڊيگهائي+1));
	إذا(!subject){
		أرجع free_x509_and_BIO_and_send_connack_error(context, client_cert, subject_bio, MOSQ_ERR_NOMEM);
	}
	يادگيري_نقل(subject, معلومات_شروعات, (size_t)name_ڊيگهائي);
	subject[name_ڊيگهائي] = '\0';
	BIO_free(subject_bio);

	إذا(mosquitto_validate_utf8(subject, (صحيح)لڙي_ڊيگهه(subject))){
		mosquitto_free(subject);
		أرجع free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	context->username = subject;
	إذا(!context->username){
		X509_free(client_cert);
		أرجع MOSQ_ERR_AUTH;
	}

	X509_free(client_cert);
	client_cert = عدم;
	أرجع MOSQ_ERR_SUCCESS;
}
#جيڪڏهن_پورو


ساكن صحيح handle_username_from_cert_options(بنية mosquitto *context, حرف **username, حرف **password, uint16_t password_len)
{
	صحيح rc;

#جيڪڏهن_وصف_ٿيل WITH_TLS
	إذا(context->listener->ssl_ctx && (context->listener->use_سڃاڻپ_عنصر_as_username || context->listener->use_subject_as_username)){
		/* Don't need the username or password if provided */
		mosquitto_FREE(*username);
		mosquitto_FREE(*password);

		إذا(!context->ssl){
			أرجع send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
		}
#جيڪڏهن_وصف_ٿيل FINAL_WITH_TLS_PSK
		إذا(context->listener->psk_hint){
			/* Client should have provided an identity to get this far. */
			إذا(!context->username){
				أرجع send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}وإلا
#جيڪڏهن_پورو /* FINAL_WITH_TLS_PSK */
		{
			إذا(context->listener->use_سڃاڻپ_عنصر_as_username){
				rc = سيٽ_username_from_cert_سڃاڻپ_عنصر(context);
			}وإلا{   /* use_subject_as_username */
				rc = سيٽ_username_from_cert_subject_name(context);
			}
			إذا(rc){
				أرجع rc;
			}
		}
	}وإلا
#جيڪڏهن_پورو /* WITH_TLS */
	{
#جيڪڏهن_وصف_ٿيل WITH_TLS
		إذا(context->listener->use_سڃاڻپ_عنصر_as_username && context->listener->require_certificate){
			mosquitto_FREE(*username);
			mosquitto_FREE(*password);

			إذا(!context->username){
				أرجع send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}وإلا
#جيڪڏهن_پورو
		{
			/* FIXME - these ensure the mosquitto_clientid() and
			* mosquitto_client_username() functions work, but is hacky */
			context->username = *username;
			context->password = *password;
			context->password_len = password_len;
			*username = عدم; /* Avoid free() in error: below. */
			*password = عدم;
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح handle_username_as_clientid_option(بنية mosquitto *context)
{
	إذا(context->username){
		mosquitto_FREE(context->id);
		context->id = mosquitto_strdup(context->username);
		إذا(!context->id){
			أرجع MOSQ_ERR_NOMEM;
		}
	}وإلا{
		uint8_t err_ڪوڊ = context->protocol == mosq_p_mqtt5
												? (uint8_t)MQTT_RC_NOT_AUTHORIZED
												: (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
		أرجع send__connack_error_and_return(context, err_ڪوڊ, MOSQ_ERR_AUTH);
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح handle__connect(بنية mosquitto *context)
{
	حرف protocol_name[7];
	uint8_t protocol_نسخو;
	uint8_t connect_flags;
	حرف *clientid = عدم;
	بنية mosquitto *مليو_context;
	بنية mosquitto_پيغام_all *will_struct = عدم;
	uint8_t will, will_retain, will_qos, clean_شروعات;
	uint8_t username_flag, password_flag;
	حرف *username = عدم, *password = عدم;
	uint16_t password_len = 0;
	صحيح rc;
	mosquitto_property *properties = عدم;
	فراغ *auth_معلومات = عدم;
	uint16_t auth_معلومات_len = 0;
	فراغ *auth_معلومات_ٻاهريون = عدم;
	uint16_t auth_معلومات_ٻاهريون_len = 0;
	bool allow_zero_ڊيگهائي_clientid;

	إذا(!context->listener){
		أرجع MOSQ_ERR_INVAL;
	}

	/* Don't accept multiple CONNECT commands. */
	إذا(context->state != mosq_cs_new){
		log__printf(عدم, MOSQ_LOG_NOTICE, "Bad client %s:%d sending multiple CONNECT messages.",
				context->پتو, context->remote_پورٽ);
		rc = MOSQ_ERR_PROTOCOL;
		اذهب handle_connect_error;
	}

#جيڪڏهن_وصف_ٿيل WITH_TLS
	إذا(context->اندريون_packet.command == 0x16 && context->listener->ssl_ctx == عدم){ /* 0x16 is TLS handshake client hello */
		log__printf(عدم, MOSQ_LOG_NOTICE, "Client from %s:%d appears to be using TLS to connect to a non-TLS listener.",
				context->پتو, context->remote_پورٽ);
		rc = MOSQ_ERR_PROTOCOL;
		اذهب handle_connect_error;
	}
#جيڪڏهن_پورو

	rc = read_protocol_name(context, protocol_name);
	إذا(rc != MOSQ_ERR_SUCCESS){
		اذهب handle_connect_error;
	}

	rc = read_and_verify_protocol_نسخو(context, protocol_name, &protocol_نسخو);
	إذا(rc != MOSQ_ERR_SUCCESS){
		إذا(rc == MOSQ_ERR_MALFORMED_PACKET){
			أرجع rc;
		}
		اذهب handle_connect_error;
	}

	rc = read_and_verify_connect_flags(context, &connect_flags);
	إذا(rc != MOSQ_ERR_SUCCESS){
		اذهب handle_connect_error;
	}

	clean_شروعات = (connect_flags & 0x02) >> 1;
	سيٽ_session_expiry_interval(context, clean_شروعات, protocol_نسخو);

	rc = read_and_reset_keepalive(context);
	إذا(rc != MOSQ_ERR_SUCCESS){
		اذهب handle_connect_error;
	}

	rc = read_and_verify_v5_connect_properties(context, &properties, protocol_نسخو);
	إذا(rc != MOSQ_ERR_SUCCESS){
		اذهب handle_connect_error;
	}

	will = connect_flags & 0x04;
	will_qos = (connect_flags & 0x18) >> 3;
	will_retain = ((connect_flags & 0x20) == 0x20);
	rc = verify_will_options(context, will, will_qos, will_retain, protocol_نسخو);
	إذا(rc != MOSQ_ERR_SUCCESS){
		اذهب handle_connect_error;
	}

	mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_طريقو, &context->auth_طريقو, false);
	mosquitto_property_read_ٻٽو(properties, MQTT_PROP_AUTHENTICATION_معلومات, &auth_معلومات, &auth_معلومات_len, false);
	mosquitto_property_free_all(&properties);

	إذا(auth_معلومات && !context->auth_طريقو){
		send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, عدم);
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d CONNECT with missing clientid string.",
				context->پتو, context->remote_پورٽ);
		rc = MOSQ_ERR_PROTOCOL;
		اذهب handle_connect_error;
	}

	rc = read_and_verify_clientid_from_packet(context, &clientid, &allow_zero_ڊيگهائي_clientid, clean_شروعات);
	إذا(rc != MOSQ_ERR_SUCCESS){
		اذهب handle_connect_error;
	}

	إذا(will){
		rc = will__read(context, clientid, &will_struct, will_qos, will_retain);
		إذا(rc){
			إذا(context->protocol == mosq_p_mqtt5){
				إذا(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
					send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, عدم);
				}وإلا إذا(rc == MOSQ_ERR_MALFORMED_PACKET){
					send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, عدم);
				}
			}
			اذهب handle_connect_error;
		}
	}وإلا{
		إذا(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
			إذا(will_qos != 0 || will_retain != 0){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT without Will with non-zero QoS (%d) or retain (%d).",
						clientid, will_qos, will_retain);
				rc = MOSQ_ERR_PROTOCOL;
				اذهب handle_connect_error;
			}
		}
	}

	// Client credentials
	password_flag = connect_flags & 0x40;
	username_flag = connect_flags & 0x80;
	rc = read_and_verify_client_credentials_from_packet(context, &username, username_flag, &password, &password_len, password_flag, clientid);
	إذا(rc != MOSQ_ERR_SUCCESS){
		اذهب handle_connect_error;
	}

	rc = check_additional_trailing_معلومات(context, protocol_نسخو);
	إذا(rc != MOSQ_ERR_SUCCESS){
		اذهب handle_connect_error;
	}

	/* Once context->id is set, if we return from this function with an error
	 * we must make sure that context->id is freed and set to NULL, so that the
	 * client isn't erroneously removed from the by_id hash table. */
	context->id = clientid;
	clientid = عدم;

	/* use_identity_as_username or use_subject_as_username */
	rc = handle_username_from_cert_options(context, &username, &password, password_len);
	إذا(rc != MOSQ_ERR_SUCCESS){
		اذهب handle_connect_error;
	}

	/* use_username_as_clientid */
	إذا(context->listener->use_username_as_clientid){
		rc = handle_username_as_clientid_option(context);
		إذا(rc != MOSQ_ERR_SUCCESS){
			اذهب handle_connect_error;
		}
	}

	/* Check for an existing delayed auth check, reject if present */
	HASH_ڳوليو(hh_id, db.contexts_by_id_delayed_auth, context->id, لڙي_ڊيگهه(context->id), مليو_context);
	إذا(مليو_context){
		rc = MOSQ_ERR_UNKNOWN;
		اذهب handle_connect_error;
	}

	context->clean_شروعات = clean_شروعات;
	context->will = will_struct;
	will_struct = عدم;

	إذا(context->auth_طريقو){
		rc = mosquitto_security_auth_شروعات(context, false, auth_معلومات, auth_معلومات_len, &auth_معلومات_ٻاهريون, &auth_معلومات_ٻاهريون_len);
		mosquitto_FREE(auth_معلومات);
		إذا(rc == MOSQ_ERR_SUCCESS){
			أرجع connect__on_authorised(context, auth_معلومات_ٻاهريون, auth_معلومات_ٻاهريون_len);
		}وإلا إذا(rc == MOSQ_ERR_AUTH_CONTINUE){
			mosquitto_سيٽ_state(context, mosq_cs_authenticating);
			rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_معلومات_ٻاهريون, auth_معلومات_ٻاهريون_len);
			محفوظ_FREE(auth_معلومات_ٻاهريون);
			أرجع rc;
		}وإلا{
			محفوظ_FREE(auth_معلومات_ٻاهريون);
			will__clear(context);
			إذا(rc == MOSQ_ERR_AUTH){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, عدم);
				mosquitto_FREE(context->id);
				اذهب handle_connect_error;
			}وإلا إذا(rc == MOSQ_ERR_NOT_SUPPORTED){
				/* Client has requested extended authentication, but we don't support it. */
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_طريقو, عدم);
				mosquitto_FREE(context->id);
				اذهب handle_connect_error;
			}وإلا{
				mosquitto_FREE(context->id);
				اذهب handle_connect_error;
			}
		}
	}وإلا{
#جيڪڏهن_وصف_ٿيل WITH_TLS
		إذا(context->listener->ssl_ctx && (context->listener->use_سڃاڻپ_عنصر_as_username || context->listener->use_subject_as_username)){
			/* Authentication assumed to be cleared */
		}وإلا
#جيڪڏهن_پورو
		{
			rc = mosquitto_basic_auth(context);
			بدل(rc){
				حالة MOSQ_ERR_SUCCESS:
					اكسر;
				حالة MOSQ_ERR_AUTH_DELAYED:
					mosquitto_سيٽ_state(context, mosq_cs_delayed_auth);
					HASH_جوڙيو_KEYPTR(hh_id, db.contexts_by_id_delayed_auth, context->id, لڙي_ڊيگهه(context->id), context);
					أرجع MOSQ_ERR_SUCCESS;
					اكسر;
				حالة MOSQ_ERR_AUTH:
					إذا(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, عدم);
					}وإلا{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, عدم);
					}
					اذهب handle_connect_error;
					اكسر;
				حالة MOSQ_ERR_UNSPECIFIED:
				حالة MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
				حالة MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
				حالة MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
				حالة MOSQ_ERR_SERVER_UNAVAILABLE:
				حالة MOSQ_ERR_SERVER_BUSY:
				حالة MOSQ_ERR_BANNED:
				حالة MOSQ_ERR_BAD_AUTHENTICATION_طريقو:
				حالة MOSQ_ERR_CONNECTION_شرح_EXCEEDED:
					إذا(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, (uint8_t)rc, عدم);
					}وإلا{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, عدم);
					}
					اذهب handle_connect_error;
					اكسر;
				افتراضي:
					rc = MOSQ_ERR_UNKNOWN;
					اذهب handle_connect_error;
					اكسر;
			}
		}
		أرجع connect__on_authorised(context, عدم, 0);
	}


handle_connect_error:
	mosquitto_property_free_all(&properties);
	mosquitto_FREE(auth_معلومات);
	mosquitto_FREE(clientid);
	mosquitto_FREE(username);
	mosquitto_FREE(password);
	إذا(will_struct){
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct->نياپو.payload);
		mosquitto_FREE(will_struct->نياپو.topic);
		mosquitto_FREE(will_struct);
	}
	will__clear(context);
	/* We return an error here which means the client is freed later on. */
	context->clean_شروعات = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_فوري;
	context->will_delay_interval = 0;
	أرجع rc;
}
