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
#پایان_اگر


ساكن حرف nibble_مقصد_hex(uint8_t مقدار)
{
	إذا(مقدار < 0x0A){
		أرجع (حرف)('0'+مقدار);
	}وإلا{
		أرجع (حرف)(65 /*'A'*/ +مقدار-10);
	}
}


ساكن حرف *clientid_gen(uint16_t *idlen, ثابت حرف *auto_id_پیشوند, uint16_t auto_id_پیشوند_len)
{
	حرف *clientid;
	uint8_t rnd[16];
	صحيح pos;

	إذا(mosquitto_getrandom(rnd, 16)){
		أرجع عدم;
	}

	*idlen = (uint16_t)(auto_id_پیشوند_len + 36);

	clientid = (حرف *)mosquitto_calloc((size_t)(*idlen) + 1, حجم(حرف));
	إذا(!clientid){
		أرجع عدم;
	}
	إذا(auto_id_پیشوند){
		کپی_حافظه(clientid, auto_id_پیشوند, auto_id_پیشوند_len);
	}

	pos = 0;
	لكل(صحيح i=0; i<16; i++){
		clientid[auto_id_پیشوند_len + pos + 0] = nibble_مقصد_hex(rnd[i] & 0x0F);
		clientid[auto_id_پیشوند_len + pos + 1] = nibble_مقصد_hex((rnd[i] >> 4) & 0x0F);
		pos += 2;
		إذا(pos == 8 || pos == 13 || pos == 18 || pos == 23){
			clientid[auto_id_پیشوند_len + pos] = '-';
			pos++;
		}
	}

	أرجع clientid;
}


صحيح connect__on_authorised(بنية mosquitto *context, فراغ *auth_داده_بیرون, uint16_t auth_داده_بیرون_len)
{
	بنية mosquitto *یافت_context;
	بنية mosquitto__subleaf *برگ;
	mosquitto_property *connack_props = عدم;
	uint8_t connect_ack = 0;
	صحيح rc;
	صحيح درون_quota, بیرون_quota;
	uint16_t درون_بیشینه, بیرون_بیشینه;

	/* Find if this client already has an entry. This must be done *after* any security checks. */
	HASH_پیدا_کن(hh_id, db.contexts_by_id, context->id, طول_رشته(context->id), یافت_context);
	إذا(یافت_context){
		/* Found a matching client */
		إذا(!net__is_connected(یافت_context)){
			/* Client is reconnecting after a disconnect */
			/* FIXME - does anything need to be done here? */
		}وإلا{
			/* Client is already connected, disconnect old version. This is
			 * done in context__cleanup() below. */
		}

		إذا(context->clean_شروع == true){
			sub__clean_session(یافت_context);
			یافت_context->session_expiry_interval = MQTT_SESSION_EXPIRY_فوری;
			plugin_persist__handle_client_delete(یافت_context);
		}
		context->is_persisted = یافت_context->is_persisted;
		یافت_context->is_persisted = false; /* stops persistence for context being removed */

		إذا(context->clean_شروع == false && یافت_context->session_expiry_interval != MQTT_SESSION_EXPIRY_فوری){
			إذا(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
				connect_ack |= 0x01;
			}

			إذا(یافت_context->msgs_درون.inflight || یافت_context->msgs_درون.queued
					|| یافت_context->msgs_بیرون.inflight || یافت_context->msgs_بیرون.queued){

				درون_quota = context->msgs_درون.inflight_quota;
				بیرون_quota = context->msgs_بیرون.inflight_quota;
				درون_بیشینه = context->msgs_درون.inflight_بیشینه;
				بیرون_بیشینه = context->msgs_بیرون.inflight_بیشینه;

				کپی_حافظه(&context->msgs_درون, &یافت_context->msgs_درون, حجم(بنية mosquitto_پیام_داده));
				کپی_حافظه(&context->msgs_بیرون, &یافت_context->msgs_بیرون, حجم(بنية mosquitto_پیام_داده));
				context->آخرین_cmsg_id = یافت_context->آخرین_cmsg_id;

				memset(&یافت_context->msgs_درون, 0, حجم(بنية mosquitto_پیام_داده));
				memset(&یافت_context->msgs_بیرون, 0, حجم(بنية mosquitto_پیام_داده));

				context->msgs_درون.inflight_quota = درون_quota;
				context->msgs_بیرون.inflight_quota = بیرون_quota;
				context->msgs_درون.inflight_بیشینه = درون_بیشینه;
				context->msgs_بیرون.inflight_بیشینه = بیرون_بیشینه;

				db_پیام_reconnect_reset(context);
			}
			context->subs = یافت_context->subs;
			یافت_context->subs = عدم;
			context->subs_ظرفیت = یافت_context->subs_ظرفیت;
			context->subs_شمار = یافت_context->subs_شمار;
			یافت_context->subs_ظرفیت = 0;
			یافت_context->subs_شمار = 0;
			context->آخرین_mid = یافت_context->آخرین_mid;

			لكل(صحيح i=0; i<context->subs_ظرفیت; i++){
				إذا(context->subs[i]){
					برگ = context->subs[i]->hier->subs;
					طالما(برگ){
						إذا(برگ->context == یافت_context){
							برگ->context = context;
						}
						برگ = برگ->بعدی;
					}

					إذا(context->subs[i]->shared){
						برگ = context->subs[i]->shared->subs;
						طالما(برگ){
							إذا(برگ->context == یافت_context){
								برگ->context = context;
							}
							برگ = برگ->بعدی;
						}
					}
				}
			}
		}

		إذا((یافت_context->protocol == mosq_p_mqtt5 && یافت_context->session_expiry_interval == MQTT_SESSION_EXPIRY_فوری)
				|| (یافت_context->protocol != mosq_p_mqtt5 && یافت_context->clean_شروع == true)
				|| (context->clean_شروع == true)
				){

			context__send_will(یافت_context);
		}

		session_expiry__remove(یافت_context);
		will_delay__remove(یافت_context);
		will__clear(یافت_context);

		یافت_context->clean_شروع = true;
		یافت_context->session_expiry_interval = MQTT_SESSION_EXPIRY_فوری;
		mosquitto_مجموعه_state(یافت_context, mosq_cs_duplicate);
		إذا(یافت_context->protocol == mosq_p_mqtt5){
			send__disconnect(یافت_context, MQTT_RC_SESSION_گرفته_OVER, عدم);
		}
		do_disconnect(یافت_context, MOSQ_ERR_SESSION_گرفته_OVER);
	}

	إذا(db.config->global_max_clients > 0 && HASH_CNT(hh_id, db.contexts_by_id) >= (بیعلامت صحيح)db.config->global_max_clients){
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
						context->آدرس, context->remote_پورت, context->id, context->protocol, context->clean_شروع, context->keepalive, context->username);
			}وإلا{
				log__printf(عدم, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d).",
						context->آدرس, context->remote_پورت, context->id, context->protocol, context->clean_شروع, context->keepalive);
			}
		}وإلا{
			إذا(context->username){
				log__printf(عدم, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->آدرس, context->remote_پورت, context->id, context->protocol, context->clean_شروع, context->keepalive, context->username);
			}وإلا{
				log__printf(عدم, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d).",
						context->آدرس, context->remote_پورت, context->id, context->protocol, context->clean_شروع, context->keepalive);
			}
		}

		إذا(context->will){
			log__printf(عدم, MOSQ_LOG_DEBUG, "Will message specified (%ld bytes) (r%d, q%d).",
					(دراز)context->will->پیام.payloadlen,
					context->will->پیام.retain,
					context->will->پیام.qos);

			log__printf(عدم, MOSQ_LOG_DEBUG, "\t%s", context->will->پیام.topic);
		}وإلا{
			log__printf(عدم, MOSQ_LOG_DEBUG, "No will message specified.");
		}
	}
#اگر_تعریف_شده WITH_TLS
	إذا(context->ssl){
		log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s negotiated %s cipher %s",
				context->id,
				SSL_get_cipher_نسخه(context->ssl),
				SSL_get_cipher_name(context->ssl));
	}
#پایان_اگر

	context->ping_t = 0;
	context->is_dropping = false;

	/* Remove any queued messages that are no longer allowed through ACL,
	 * assuming a possible change of username. */
	db_check_acl_از_all_messages(context);
	context_جمع_کن_مقصد_by_id(context);

#اگر_تعریف_شده WITH_PERSISTENCE
	إذا(!context->clean_شروع){
		db.persistence_changes++;
	}
#پایان_اگر
	context->max_qos = context->listener->max_qos;

	إذا(db.config->max_keepalive &&
			(context->keepalive > db.config->max_keepalive || context->keepalive == 0)){

		keepalive__remove(context);
		context->keepalive = db.config->max_keepalive;
		keepalive_جمع_کن(context);
		إذا(context->protocol == mosq_p_mqtt5){
			إذا(mosquitto_property_جمع_کن_int16(&connack_props, MQTT_PROP_SERVER_KEEP_ALIVE, context->keepalive)){
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
			إذا(mosquitto_property_جمع_کن_int16(&connack_props, MQTT_PROP_TOPIC_ALIAS_بیشینه, context->listener->max_topic_alias)){
				rc = MOSQ_ERR_NOMEM;
				اذهب error;
			}
		}
		إذا(context->assigned_id){
			إذا(mosquitto_property_جمع_کن_string(&connack_props, MQTT_PROP_ASSIGNED_CLIENT_IDENTIFIER, context->id)){
				rc = MOSQ_ERR_NOMEM;
				اذهب error;
			}
		}
		إذا(context->auth_روش){
			إذا(mosquitto_property_جمع_کن_string(&connack_props, MQTT_PROP_AUTHENTICATION_روش, context->auth_روش)){
				rc = MOSQ_ERR_NOMEM;
				اذهب error;
			}

			إذا(auth_داده_بیرون && auth_داده_بیرون_len > 0){
				إذا(mosquitto_property_جمع_کن_دودویی(&connack_props, MQTT_PROP_AUTHENTICATION_داده, auth_داده_بیرون, auth_داده_بیرون_len)){
					rc = MOSQ_ERR_NOMEM;
					اذهب error;
				}
			}
		}
	}
	امن_FREE(auth_داده_بیرون);

	mosquitto_مجموعه_state(context, mosq_cs_فعال);
	rc = send__connack(context, connect_ack, CONNACK_ACCEPTED, connack_props);
	mosquitto_property_free_all(&connack_props);
	إذا(rc){
		أرجع rc;
	}
	db__expire_all_messages(context);
	rc = db_پیام_write_queued_بیرون(context);
	إذا(rc){
		أرجع rc;
	}
	rc = db_پیام_write_inflight_بیرون_all(context);

	إذا(rc == MOSQ_ERR_SUCCESS){
		plugin__handle_connect(context);

		إذا(context->session_expiry_interval != MQTT_SESSION_EXPIRY_فوری){
			plugin_persist_handle_client_جمع_کن(context);
		}وإلا إذا(context->will){
			plugin_persist_handle_will_جمع_کن(context);
		}
	}
	أرجع rc;
error:
	امن_FREE(auth_داده_بیرون);
	mosquitto_property_free_all(&connack_props);
	أرجع rc;
}


ساكن صحيح will__read(بنية mosquitto *context, ثابت حرف *clientid, بنية mosquitto_پیام_all **will, uint8_t will_qos, صحيح will_retain)
{
	صحيح rc = MOSQ_ERR_SUCCESS;
	size_t slen;
	uint16_t tlen;
	بنية mosquitto_پیام_all *will_struct = عدم;
	حرف *will_topic_mount = عدم;
	uint16_t payloadlen;
	mosquitto_property *properties = عدم;

	will_struct = mosquitto_calloc(1, حجم(بنية mosquitto_پیام_all));
	إذا(!will_struct){
		rc = MOSQ_ERR_NOMEM;
		اذهب error_cleanup;
	}
	إذا(context->protocol == PROTOCOL_نسخه_v5){
		rc = property__read_all(CMD_WILL, &context->درون_packet, &properties);
		إذا(rc){
			اذهب error_cleanup;
		}

		rc = property__process_will(context, will_struct, &properties);
		mosquitto_property_free_all(&properties);
		إذا(rc){
			اذهب error_cleanup;
		}
	}
	rc = packet__read_string(&context->درون_packet, &will_struct->پیام.topic, &tlen);
	إذا(rc){
		اذهب error_cleanup;
	}
	إذا(!tlen){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Will with empty topic.",
				context->id);
		rc = MOSQ_ERR_PROTOCOL;
		اذهب error_cleanup;
	}

	إذا(context->listener->mount_نقطه){
		slen = طول_رشته(context->listener->mount_نقطه) + طول_رشته(will_struct->پیام.topic) + 1;
		will_topic_mount = mosquitto_malloc(slen+1);
		إذا(!will_topic_mount){
			rc = MOSQ_ERR_NOMEM;
			اذهب error_cleanup;
		}

		snprintf(will_topic_mount, slen, "%s%s", context->listener->mount_نقطه, will_struct->پیام.topic);
		will_topic_mount[slen] = '\0';

		mosquitto_FREE(will_struct->پیام.topic);
		will_struct->پیام.topic = will_topic_mount;
	}

	إذا(!مقایسه_رشته(will_struct->پیام.topic, "$CONTROL/", طول_رشته("$CONTROL/"))){
		rc = MOSQ_ERR_ACL_DENIED;
		اذهب error_cleanup;
	}
	rc = mosquitto_pub_topic_check(will_struct->پیام.topic);
	إذا(rc){
		اذهب error_cleanup;
	}

	rc = packet__read_uint16(&context->درون_packet, &payloadlen);
	إذا(rc){
		اذهب error_cleanup;
	}

	will_struct->پیام.payloadlen = payloadlen;
	إذا(will_struct->پیام.payloadlen > 0){
		إذا(db.config->پیام_اندازه_حد && will_struct->پیام.payloadlen > (صحيح)db.config->پیام_اندازه_حد){
			log__printf(عدم, MOSQ_LOG_DEBUG, "Client %s connected with too large Will payload", clientid);
			إذا(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_PACKET_TOO_LARGE, عدم);
			}وإلا{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, عدم);
			}
			rc = MOSQ_ERR_PAYLOAD_اندازه;
			اذهب error_cleanup;
		}
		will_struct->پیام.payload = mosquitto_malloc((size_t)will_struct->پیام.payloadlen);
		إذا(!will_struct->پیام.payload){
			rc = MOSQ_ERR_NOMEM;
			اذهب error_cleanup;
		}

		rc = packet_read_بایتها(&context->درون_packet, will_struct->پیام.payload, (uint32_t)will_struct->پیام.payloadlen);
		إذا(rc){
			اذهب error_cleanup;
		}
	}

	will_struct->پیام.qos = will_qos;
	will_struct->پیام.retain = will_retain;

	*will = will_struct;
	أرجع MOSQ_ERR_SUCCESS;

error_cleanup:
	إذا(will_struct){
		mosquitto_FREE(will_struct->پیام.topic);
		mosquitto_FREE(will_struct->پیام.payload);
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct);
	}
	أرجع rc;
}


ساكن صحيح check_protocol_نسخه(بنية mosquitto__listener *listener, صحيح protocol_نسخه)
{
	/* Allow bridge protocol as well. */
	protocol_نسخه &= 0x7F;

	إذا((protocol_نسخه == 3 && listener->disable_protocol_v3 == false)
			|| (protocol_نسخه == 4 && listener->disable_protocol_v4 == false)
			|| (protocol_نسخه == 5 && listener->disable_protocol_v5 == false)
			){

		أرجع MOSQ_ERR_SUCCESS;
	}وإلا{
		أرجع MOSQ_ERR_NOT_SUPPORTED;
	}
}


مضمن ساكن صحيح send__connack_error_and_return(بنية mosquitto *context, uint8_t err_کد, صحيح rc)
{
	send__connack(context, 0, err_کد, عدم);
	أرجع rc;
}


مضمن ساكن صحيح send__connack_bad_username_or_password_error(بنية mosquitto *context, صحيح rc)
{
	uint8_t err_کد  = context->protocol == mosq_p_mqtt5
									? (uint8_t)MQTT_RC_BAD_USERNAME_OR_PASSWORD
									: (uint8_t)CONNACK_REFUSED_BAD_USERNAME_PASSWORD;
	أرجع send__connack_error_and_return(context, err_کد, rc);
}


ساكن صحيح read_protocol_name(بنية mosquitto *context, حرف protocol_name[7])
{
	/* Read protocol name as length then bytes rather than with read_string
	 * because the length is fixed and we can check that. Removes the need
	 * for another malloc as well. */

	uint16_t slen = 0;

	إذا(packet__read_uint16(&context->درون_packet, &slen)){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string length.",
				context->آدرس, context->remote_پورت);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	إذا(slen != 4 /* MQTT */ && slen != 6 /* MQIsdp */){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with incorrect protocol string length (%d).",
				context->آدرس, context->remote_پورت, slen);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	إذا(packet_read_بایتها(&context->درون_packet, protocol_name, slen)){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string.",
				context->آدرس, context->remote_پورت);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	protocol_name[slen] = '\0';

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح read_and_verify_protocol_نسخه(بنية mosquitto *context, ثابت حرف *protocol_name,
		uint8_t *protocol_نسخه)
{
	uint8_t tmp_protocol_نسخه = 0;
	إذا(packet__read_byte(&context->درون_packet, &tmp_protocol_نسخه)){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol version.",
				context->آدرس, context->remote_پورت);
		أرجع MOSQ_ERR_PROTOCOL;
	}

	إذا(check_protocol_نسخه(context->listener, tmp_protocol_نسخه)){
		إذا(tmp_protocol_نسخه == 3 || tmp_protocol_نسخه == 4){
			context->protocol = mosq_p_mqtt311;
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_نسخه, عدم);
		}وإلا{
			context->protocol = mosq_p_mqtt5;
			send__connack(context, 0, MQTT_RC_UNSUPPORTED_PROTOCOL_نسخه, عدم);
		}
		أرجع MOSQ_ERR_NOT_SUPPORTED;
	}

	إذا(!strcmp(protocol_name, PROTOCOL_NAME_v31)){
		إذا((tmp_protocol_نسخه&0x7F) != PROTOCOL_نسخه_v31){
			إذا(db.config->connection_messages == true){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->آدرس, context->remote_پورت, tmp_protocol_نسخه);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_نسخه, عدم);
			أرجع MOSQ_ERR_PROTOCOL;
		}
		context->protocol = mosq_p_mqtt31;
		إذا((tmp_protocol_نسخه&0x80) == 0x80){
			context->is_bridge = true;
		}
	}وإلا إذا(!strcmp(protocol_name, PROTOCOL_NAME)){
		إذا((tmp_protocol_نسخه&0x7F) == PROTOCOL_نسخه_v311){
			context->protocol = mosq_p_mqtt311;

			إذا((tmp_protocol_نسخه&0x80) == 0x80){
				context->is_bridge = true;
			}
		}وإلا إذا((tmp_protocol_نسخه&0x7F) == PROTOCOL_نسخه_v5){
			context->protocol = mosq_p_mqtt5;
		}وإلا{
			إذا(db.config->connection_messages == true){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->آدرس, context->remote_پورت, tmp_protocol_نسخه);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_نسخه, عدم);
			أرجع MOSQ_ERR_PROTOCOL;
		}
		إذا((context->درون_packet.command&0x0F) != 0x00){
			/* Reserved flags not set to 0, must disconnect. */
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero reserved flags (%02X).",
					context->آدرس, context->remote_پورت, context->درون_packet.command);
			أرجع MOSQ_ERR_PROTOCOL;
		}
	}وإلا{
		إذا(db.config->connection_messages == true){
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol \"%s\".",
					context->آدرس, context->remote_پورت, protocol_name);
		}
		أرجع MOSQ_ERR_PROTOCOL;
	}
	إذا((tmp_protocol_نسخه&0x7F) != PROTOCOL_نسخه_v31 && context->درون_packet.command != CMD_CONNECT){
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}

	*protocol_نسخه = tmp_protocol_نسخه;
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح read_and_verify_connect_flags(بنية mosquitto *context, uint8_t *connect_flags)
{
	إذا(packet__read_byte(&context->درون_packet, connect_flags)){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing connect flags.",
				context->آدرس, context->remote_پورت);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	إذا(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		إذا((*connect_flags & 0x01) != 0x00){
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero connect reserved flag (%02X).",
					context->آدرس, context->remote_پورت, *connect_flags);
			أرجع MOSQ_ERR_PROTOCOL;
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن فراغ مجموعه_session_expiry_interval(بنية mosquitto *context, uint8_t clean_شروع, uint8_t protocol_نسخه)
{
	/* session_expiry_interval will be overridden if the properties are read later */
	إذا(clean_شروع == false && protocol_نسخه != PROTOCOL_نسخه_v5){
		/* v3* has clean_start == false mean the session never expires */
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
	}وإلا{
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_فوری;
	}
}


ساكن صحيح read_and_reset_keepalive(بنية mosquitto *context)
{
	/* _remove here because net__socket_accept() uses _add and we must have the
	 * correct keepalive value */
	keepalive__remove(context);

	إذا(packet__read_uint16(&context->درون_packet, &(context->keepalive))){
		log__printf(عدم, MOSQ_LOG_INFO, "%s sent CONNECT with missing keepalive.",
				context->id);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	keepalive_جمع_کن(context);

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح read_and_verify_v5_connect_properties(بنية mosquitto *context, mosquitto_property **properties, uint8_t protocol_نسخه)
{
	صحيح rc;

	إذا(protocol_نسخه == PROTOCOL_نسخه_v5){
		rc = property__read_all(CMD_CONNECT, &context->درون_packet, properties);
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


ساكن صحيح verify_will_options(بنية mosquitto *context, uint8_t will, uint8_t will_qos, uint8_t will_retain, uint8_t protocol_نسخه)
{
	إذا(will_qos == 3){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid Will QoS (%d).",
				context->آدرس, context->remote_پورت, will_qos);
		أرجع MOSQ_ERR_PROTOCOL;
	}

	إذا(will && will_retain && db.config->retain_موجود == false){
		إذا(protocol_نسخه == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_RETAIN_NOT_SUPPORTED, عدم);
		}
		أرجع MOSQ_ERR_NOT_SUPPORTED;
	}

	إذا(will && will_qos > context->listener->max_qos){
		إذا(protocol_نسخه == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_QOS_NOT_SUPPORTED, عدم);
		}
		أرجع MOSQ_ERR_NOT_SUPPORTED;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح handle_zero_درازی_clientid(بنية mosquitto *context, حرف **clientid, bool *allow_zero_درازی_clientid,
		uint8_t clean_شروع)
{
	إذا(context->protocol == mosq_p_mqtt31){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: v3.1 CONNECT with zero length clientid.",
				context->آدرس, context->remote_پورت);
		send__connack(context, 0, CONNACK_REFUSED_IDENTIFIER_REJECTED, عدم);
		أرجع MOSQ_ERR_PROTOCOL;
	}

	/* mqtt311/mqtt5 */
	mosquitto_FREE(*clientid);

	إذا(db.config->در_هر_listener_settings){
		*allow_zero_درازی_clientid = context->listener->security_options->allow_zero_درازی_clientid;
	}وإلا{
		*allow_zero_درازی_clientid = db.config->security_options.allow_zero_درازی_clientid;
	}

	إذا((context->protocol == mosq_p_mqtt311 && clean_شروع == 0) || *allow_zero_درازی_clientid == false){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with zero length clientid when forbidden.",
				context->آدرس, context->remote_پورت);
		uint8_t err_کد = context->protocol == mosq_p_mqtt311 ? (uint8_t)CONNACK_REFUSED_IDENTIFIER_REJECTED : (uint8_t)MQTT_RC_UNSPECIFIED;
		أرجع send__connack_error_and_return(context, err_کد, MOSQ_ERR_PROTOCOL);
	}

	*clientid = clientid_gen(&(uint16_t){0}, context->listener->security_options->auto_id_پیشوند,
			context->listener->security_options->auto_id_پیشوند_len);
	إذا(*clientid == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}
	context->assigned_id = true;

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح check_clientid_prefixes(بنية mosquitto *context, ثابت حرف *clientid)
{
	إذا(db.config->clientid_prefixes){
		إذا(مقایسه_رشته(db.config->clientid_prefixes, clientid, طول_رشته(db.config->clientid_prefixes))){
			uint8_t err_کد = context->protocol == mosq_p_mqtt5 ? (uint8_t)MQTT_RC_NOT_AUTHORIZED : (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
			أرجع send__connack_error_and_return(context, err_کد, MOSQ_ERR_AUTH);
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح read_and_verify_clientid_from_packet(بنية mosquitto *context, حرف **clientid,
		bool *allow_zero_درازی_clientid, uint8_t clean_شروع)
{
	صحيح rc;
	uint16_t slen;

	rc = packet__read_string(&context->درون_packet, clientid, &slen);
	إذا(rc == MOSQ_ERR_MALFORMED_UTF8){
		إذا(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_CLIENTID_NOT_VALID, عدم);
		}
		أرجع MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID;
	}وإلا إذا(rc > 0){
		أرجع MOSQ_ERR_PROTOCOL;
	}

	إذا(slen == 0){
		rc = handle_zero_درازی_clientid(context, clientid, allow_zero_درازی_clientid, clean_شروع);
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


ساكن صحيح مجموعه_username_from_packet(بنية mosquitto *context, حرف **username, ثابت حرف *clientid)
{
	صحيح rc;

	rc = packet__read_string(&context->درون_packet, username, &(uint16_t){0});
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


ساكن صحيح مجموعه_password_from_packet(بنية mosquitto *context, حرف **password, uint16_t *password_len, ثابت حرف *clientid)
{
	صحيح rc;

	rc = packet_read_دودویی(&context->درون_packet, (uint8_t **)password, password_len);
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
		rc = مجموعه_username_from_packet(context, username, clientid);
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
		rc = مجموعه_password_from_packet(context, password, password_len, clientid);
		إذا(rc != MOSQ_ERR_SUCCESS){
			أرجع rc;
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح check_additional_trailing_داده(بنية mosquitto *context, uint8_t protocol_نسخه)
{
	إذا(context->درون_packet.pos != context->درون_packet.مانده_درازی){
		/* Surplus data at end of packet, this must be an error. */
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->درون_packet.pos, context->درون_packet.مانده_درازی);
		إذا(protocol_نسخه == PROTOCOL_نسخه_v5){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, عدم);
		}
		أرجع MOSQ_ERR_PROTOCOL;
	}

	أرجع MOSQ_ERR_SUCCESS;
}

#اگر_تعریف_شده WITH_TLS


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


ساكن صحيح مجموعه_username_from_cert_عنصر_خنثی(بنية mosquitto *context)
{
	X509 *client_cert = عدم;
	X509_NAME *name = عدم;

	إذا(get_client_cert_and_subject_name(context, &client_cert, &name)){
		أرجع MOSQ_ERR_AUTH;
	}

	صحيح i = -1;
	X509_NAME_مدخل *name_مدخل = عدم;

	i = X509_NAME_get_شاخص_by_NID(name, NID_commonName, -1);
	إذا(i == -1){
		أرجع free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	name_مدخل = X509_NAME_get_مدخل(name, i);
	إذا(!name_مدخل){
		اذهب success;
	}

	ASN1_STRING *name_asn1 = عدم;
	name_asn1 = X509_NAME_مدخل_get_داده(name_مدخل);
	إذا(name_asn1 == عدم){
		أرجع free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	ثابت حرف *cert_عنصر_خنثی = (ثابت حرف *)ASN1_STRING_get0_داده(name_asn1);
	إذا(!cert_عنصر_خنثی || mosquitto_validate_utf8(cert_عنصر_خنثی, (صحيح)طول_رشته(cert_عنصر_خنثی))){
		أرجع free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	mosquitto_free(context->username);
	context->username = mosquitto_strdup(cert_عنصر_خنثی);
	إذا(context->username == عدم){
		أرجع free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_NOMEM);
	}
	/* Make sure there isn't an embedded NUL character in the CN */
	إذا((size_t)ASN1_STRING_درازی(name_asn1) != طول_رشته(context->username)){
		أرجع free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}

success:
	X509_free(client_cert);
	client_cert = عدم;
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح مجموعه_username_from_cert_subject_name(بنية mosquitto *context)
{
	X509 *client_cert = عدم;
	X509_NAME *name = عدم;

	إذا(get_client_cert_and_subject_name(context, &client_cert, &name)){
		أرجع MOSQ_ERR_AUTH;
	}

	حرف *subject = عدم;
	حرف *داده_شروع = عدم;
	BIO *subject_bio = عدم;
	دراز name_درازی = 0;

	subject_bio = BIO_new(BIO_s_mem());
	X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
	داده_شروع = عدم;
	name_درازی = BIO_get_mem_داده(subject_bio, &داده_شروع);
	subject = mosquitto_malloc(حجم(حرف)*(size_t)(name_درازی+1));
	إذا(!subject){
		أرجع free_x509_and_BIO_and_send_connack_error(context, client_cert, subject_bio, MOSQ_ERR_NOMEM);
	}
	کپی_حافظه(subject, داده_شروع, (size_t)name_درازی);
	subject[name_درازی] = '\0';
	BIO_free(subject_bio);

	إذا(mosquitto_validate_utf8(subject, (صحيح)طول_رشته(subject))){
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
#پایان_اگر


ساكن صحيح handle_username_from_cert_options(بنية mosquitto *context, حرف **username, حرف **password, uint16_t password_len)
{
	صحيح rc;

#اگر_تعریف_شده WITH_TLS
	إذا(context->listener->ssl_ctx && (context->listener->use_عنصر_خنثی_as_username || context->listener->use_subject_as_username)){
		/* Don't need the username or password if provided */
		mosquitto_FREE(*username);
		mosquitto_FREE(*password);

		إذا(!context->ssl){
			أرجع send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
		}
#اگر_تعریف_شده FINAL_WITH_TLS_PSK
		إذا(context->listener->psk_hint){
			/* Client should have provided an identity to get this far. */
			إذا(!context->username){
				أرجع send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}وإلا
#پایان_اگر /* FINAL_WITH_TLS_PSK */
		{
			إذا(context->listener->use_عنصر_خنثی_as_username){
				rc = مجموعه_username_from_cert_عنصر_خنثی(context);
			}وإلا{   /* use_subject_as_username */
				rc = مجموعه_username_from_cert_subject_name(context);
			}
			إذا(rc){
				أرجع rc;
			}
		}
	}وإلا
#پایان_اگر /* WITH_TLS */
	{
#اگر_تعریف_شده WITH_TLS
		إذا(context->listener->use_عنصر_خنثی_as_username && context->listener->require_certificate){
			mosquitto_FREE(*username);
			mosquitto_FREE(*password);

			إذا(!context->username){
				أرجع send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}وإلا
#پایان_اگر
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
		uint8_t err_کد = context->protocol == mosq_p_mqtt5
												? (uint8_t)MQTT_RC_NOT_AUTHORIZED
												: (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
		أرجع send__connack_error_and_return(context, err_کد, MOSQ_ERR_AUTH);
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح handle__connect(بنية mosquitto *context)
{
	حرف protocol_name[7];
	uint8_t protocol_نسخه;
	uint8_t connect_flags;
	حرف *clientid = عدم;
	بنية mosquitto *یافت_context;
	بنية mosquitto_پیام_all *will_struct = عدم;
	uint8_t will, will_retain, will_qos, clean_شروع;
	uint8_t username_flag, password_flag;
	حرف *username = عدم, *password = عدم;
	uint16_t password_len = 0;
	صحيح rc;
	mosquitto_property *properties = عدم;
	فراغ *auth_داده = عدم;
	uint16_t auth_داده_len = 0;
	فراغ *auth_داده_بیرون = عدم;
	uint16_t auth_داده_بیرون_len = 0;
	bool allow_zero_درازی_clientid;

	إذا(!context->listener){
		أرجع MOSQ_ERR_INVAL;
	}

	/* Don't accept multiple CONNECT commands. */
	إذا(context->state != mosq_cs_new){
		log__printf(عدم, MOSQ_LOG_NOTICE, "Bad client %s:%d sending multiple CONNECT messages.",
				context->آدرس, context->remote_پورت);
		rc = MOSQ_ERR_PROTOCOL;
		اذهب handle_connect_error;
	}

#اگر_تعریف_شده WITH_TLS
	إذا(context->درون_packet.command == 0x16 && context->listener->ssl_ctx == عدم){ /* 0x16 is TLS handshake client hello */
		log__printf(عدم, MOSQ_LOG_NOTICE, "Client from %s:%d appears to be using TLS to connect to a non-TLS listener.",
				context->آدرس, context->remote_پورت);
		rc = MOSQ_ERR_PROTOCOL;
		اذهب handle_connect_error;
	}
#پایان_اگر

	rc = read_protocol_name(context, protocol_name);
	إذا(rc != MOSQ_ERR_SUCCESS){
		اذهب handle_connect_error;
	}

	rc = read_and_verify_protocol_نسخه(context, protocol_name, &protocol_نسخه);
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

	clean_شروع = (connect_flags & 0x02) >> 1;
	مجموعه_session_expiry_interval(context, clean_شروع, protocol_نسخه);

	rc = read_and_reset_keepalive(context);
	إذا(rc != MOSQ_ERR_SUCCESS){
		اذهب handle_connect_error;
	}

	rc = read_and_verify_v5_connect_properties(context, &properties, protocol_نسخه);
	إذا(rc != MOSQ_ERR_SUCCESS){
		اذهب handle_connect_error;
	}

	will = connect_flags & 0x04;
	will_qos = (connect_flags & 0x18) >> 3;
	will_retain = ((connect_flags & 0x20) == 0x20);
	rc = verify_will_options(context, will, will_qos, will_retain, protocol_نسخه);
	إذا(rc != MOSQ_ERR_SUCCESS){
		اذهب handle_connect_error;
	}

	mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_روش, &context->auth_روش, false);
	mosquitto_property_read_دودویی(properties, MQTT_PROP_AUTHENTICATION_داده, &auth_داده, &auth_داده_len, false);
	mosquitto_property_free_all(&properties);

	إذا(auth_داده && !context->auth_روش){
		send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, عدم);
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d CONNECT with missing clientid string.",
				context->آدرس, context->remote_پورت);
		rc = MOSQ_ERR_PROTOCOL;
		اذهب handle_connect_error;
	}

	rc = read_and_verify_clientid_from_packet(context, &clientid, &allow_zero_درازی_clientid, clean_شروع);
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

	rc = check_additional_trailing_داده(context, protocol_نسخه);
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
	HASH_پیدا_کن(hh_id, db.contexts_by_id_delayed_auth, context->id, طول_رشته(context->id), یافت_context);
	إذا(یافت_context){
		rc = MOSQ_ERR_UNKNOWN;
		اذهب handle_connect_error;
	}

	context->clean_شروع = clean_شروع;
	context->will = will_struct;
	will_struct = عدم;

	إذا(context->auth_روش){
		rc = mosquitto_security_auth_شروع(context, false, auth_داده, auth_داده_len, &auth_داده_بیرون, &auth_داده_بیرون_len);
		mosquitto_FREE(auth_داده);
		إذا(rc == MOSQ_ERR_SUCCESS){
			أرجع connect__on_authorised(context, auth_داده_بیرون, auth_داده_بیرون_len);
		}وإلا إذا(rc == MOSQ_ERR_AUTH_CONTINUE){
			mosquitto_مجموعه_state(context, mosq_cs_authenticating);
			rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_داده_بیرون, auth_داده_بیرون_len);
			امن_FREE(auth_داده_بیرون);
			أرجع rc;
		}وإلا{
			امن_FREE(auth_داده_بیرون);
			will__clear(context);
			إذا(rc == MOSQ_ERR_AUTH){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, عدم);
				mosquitto_FREE(context->id);
				اذهب handle_connect_error;
			}وإلا إذا(rc == MOSQ_ERR_NOT_SUPPORTED){
				/* Client has requested extended authentication, but we don't support it. */
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_روش, عدم);
				mosquitto_FREE(context->id);
				اذهب handle_connect_error;
			}وإلا{
				mosquitto_FREE(context->id);
				اذهب handle_connect_error;
			}
		}
	}وإلا{
#اگر_تعریف_شده WITH_TLS
		إذا(context->listener->ssl_ctx && (context->listener->use_عنصر_خنثی_as_username || context->listener->use_subject_as_username)){
			/* Authentication assumed to be cleared */
		}وإلا
#پایان_اگر
		{
			rc = mosquitto_basic_auth(context);
			بدل(rc){
				حالة MOSQ_ERR_SUCCESS:
					اكسر;
				حالة MOSQ_ERR_AUTH_DELAYED:
					mosquitto_مجموعه_state(context, mosq_cs_delayed_auth);
					HASH_جمع_کن_KEYPTR(hh_id, db.contexts_by_id_delayed_auth, context->id, طول_رشته(context->id), context);
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
				حالة MOSQ_ERR_BAD_AUTHENTICATION_روش:
				حالة MOSQ_ERR_CONNECTION_نرخ_EXCEEDED:
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
	mosquitto_FREE(auth_داده);
	mosquitto_FREE(clientid);
	mosquitto_FREE(username);
	mosquitto_FREE(password);
	إذا(will_struct){
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct->پیام.payload);
		mosquitto_FREE(will_struct->پیام.topic);
		mosquitto_FREE(will_struct);
	}
	will__clear(context);
	/* We return an error here which means the client is freed later on. */
	context->clean_شروع = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_فوری;
	context->will_delay_interval = 0;
	أرجع rc;
}
