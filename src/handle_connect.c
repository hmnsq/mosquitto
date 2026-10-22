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

#شامل "config.h"

#شامل <stdio.h>
#شامل <string.h>
#شامل <utlist.h>

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "packet_mosq.h"
#شامل "property_mosq.h"
#شامل "send_mosq.h"
#شامل "sys_tree.h"
#شامل "tls_mosq.h"
#شامل "util_mosq.h"
#شامل "will_mosq.h"

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#اگر_ختم


ساکن حرف nibble_مقصد_hex(uint8_t قدر)
{
	اگر(قدر < 0x0A){
		واپس (حرف)('0'+قدر);
	}ورنہ{
		واپس (حرف)(65 /*'A'*/ +قدر-10);
	}
}


ساکن حرف *clientid_gen(uint16_t *idlen, ثابت حرف *auto_id_سابقہ, uint16_t auto_id_سابقہ_len)
{
	حرف *clientid;
	uint8_t rnd[16];
	صحیح pos;

	اگر(mosquitto_getrandom(rnd, 16)){
		واپس تہی;
	}

	*idlen = (uint16_t)(auto_id_سابقہ_len + 36);

	clientid = (حرف *)mosquitto_calloc((size_t)(*idlen) + 1, حجم(حرف));
	اگر(!clientid){
		واپس تہی;
	}
	اگر(auto_id_سابقہ){
		حافظہ_نقل(clientid, auto_id_سابقہ, auto_id_سابقہ_len);
	}

	pos = 0;
	برائے(صحیح i=0; i<16; i++){
		clientid[auto_id_سابقہ_len + pos + 0] = nibble_مقصد_hex(rnd[i] & 0x0F);
		clientid[auto_id_سابقہ_len + pos + 1] = nibble_مقصد_hex((rnd[i] >> 4) & 0x0F);
		pos += 2;
		اگر(pos == 8 || pos == 13 || pos == 18 || pos == 23){
			clientid[auto_id_سابقہ_len + pos] = '-';
			pos++;
		}
	}

	واپس clientid;
}


صحیح connect__on_authorised(ساخت mosquitto *context, خالی *auth_ڈیٹا_برآمد, uint16_t auth_ڈیٹا_برآمد_len)
{
	ساخت mosquitto *ملا_context;
	ساخت mosquitto__subleaf *برگ;
	mosquitto_property *connack_props = تہی;
	uint8_t connect_ack = 0;
	صحیح rc;
	صحیح اندر_quota, برآمد_quota;
	uint16_t اندر_زیادہ_سے_زیادہ, برآمد_زیادہ_سے_زیادہ;

	/* Find if this client already has an entry. This must be done *after* any security checks. */
	HASH_ڈھونڈو(hh_id, db.contexts_by_id, context->id, لڑی_لمبائی(context->id), ملا_context);
	اگر(ملا_context){
		/* Found a matching client */
		اگر(!net__is_connected(ملا_context)){
			/* Client is reconnecting after a disconnect */
			/* FIXME - does anything need to be done here? */
		}ورنہ{
			/* Client is already connected, disconnect old version. This is
			 * done in context__cleanup() below. */
		}

		اگر(context->clean_آغاز == true){
			sub__clean_session(ملا_context);
			ملا_context->session_expiry_interval = MQTT_SESSION_EXPIRY_فوری;
			plugin_persist__handle_client_delete(ملا_context);
		}
		context->is_persisted = ملا_context->is_persisted;
		ملا_context->is_persisted = false; /* stops persistence for context being removed */

		اگر(context->clean_آغاز == false && ملا_context->session_expiry_interval != MQTT_SESSION_EXPIRY_فوری){
			اگر(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
				connect_ack |= 0x01;
			}

			اگر(ملا_context->msgs_اندر.inflight || ملا_context->msgs_اندر.queued
					|| ملا_context->msgs_برآمد.inflight || ملا_context->msgs_برآمد.queued){

				اندر_quota = context->msgs_اندر.inflight_quota;
				برآمد_quota = context->msgs_برآمد.inflight_quota;
				اندر_زیادہ_سے_زیادہ = context->msgs_اندر.inflight_زیادہ_سے_زیادہ;
				برآمد_زیادہ_سے_زیادہ = context->msgs_برآمد.inflight_زیادہ_سے_زیادہ;

				حافظہ_نقل(&context->msgs_اندر, &ملا_context->msgs_اندر, حجم(ساخت mosquitto_پیغام_ڈیٹا));
				حافظہ_نقل(&context->msgs_برآمد, &ملا_context->msgs_برآمد, حجم(ساخت mosquitto_پیغام_ڈیٹا));
				context->آخری_cmsg_id = ملا_context->آخری_cmsg_id;

				memset(&ملا_context->msgs_اندر, 0, حجم(ساخت mosquitto_پیغام_ڈیٹا));
				memset(&ملا_context->msgs_برآمد, 0, حجم(ساخت mosquitto_پیغام_ڈیٹا));

				context->msgs_اندر.inflight_quota = اندر_quota;
				context->msgs_برآمد.inflight_quota = برآمد_quota;
				context->msgs_اندر.inflight_زیادہ_سے_زیادہ = اندر_زیادہ_سے_زیادہ;
				context->msgs_برآمد.inflight_زیادہ_سے_زیادہ = برآمد_زیادہ_سے_زیادہ;

				db_پیغام_reconnect_reset(context);
			}
			context->subs = ملا_context->subs;
			ملا_context->subs = تہی;
			context->subs_استعداد = ملا_context->subs_استعداد;
			context->subs_گنتی = ملا_context->subs_گنتی;
			ملا_context->subs_استعداد = 0;
			ملا_context->subs_گنتی = 0;
			context->آخری_mid = ملا_context->آخری_mid;

			برائے(صحیح i=0; i<context->subs_استعداد; i++){
				اگر(context->subs[i]){
					برگ = context->subs[i]->hier->subs;
					جب(برگ){
						اگر(برگ->context == ملا_context){
							برگ->context = context;
						}
						برگ = برگ->اگلا;
					}

					اگر(context->subs[i]->shared){
						برگ = context->subs[i]->shared->subs;
						جب(برگ){
							اگر(برگ->context == ملا_context){
								برگ->context = context;
							}
							برگ = برگ->اگلا;
						}
					}
				}
			}
		}

		اگر((ملا_context->protocol == mosq_p_mqtt5 && ملا_context->session_expiry_interval == MQTT_SESSION_EXPIRY_فوری)
				|| (ملا_context->protocol != mosq_p_mqtt5 && ملا_context->clean_آغاز == true)
				|| (context->clean_آغاز == true)
				){

			context__send_will(ملا_context);
		}

		session_expiry__remove(ملا_context);
		will_delay__remove(ملا_context);
		will__clear(ملا_context);

		ملا_context->clean_آغاز = true;
		ملا_context->session_expiry_interval = MQTT_SESSION_EXPIRY_فوری;
		mosquitto_سیٹ_state(ملا_context, mosq_cs_duplicate);
		اگر(ملا_context->protocol == mosq_p_mqtt5){
			send__disconnect(ملا_context, MQTT_RC_SESSION_لیا_گیا_OVER, تہی);
		}
		do_disconnect(ملا_context, MOSQ_ERR_SESSION_لیا_گیا_OVER);
	}

	اگر(db.config->global_max_clients > 0 && HASH_CNT(hh_id, db.contexts_by_id) >= (غیرعلامتی صحیح)db.config->global_max_clients){
		اگر(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_SERVER_BUSY, تہی);
		}
		rc = MOSQ_ERR_INVAL;
		جاؤ error;
	}

	اگر(db.config->connection_messages == true){
		اگر(context->is_bridge){
			اگر(context->username){
				log__printf(تہی, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->پتہ, context->remote_پورٹ, context->id, context->protocol, context->clean_آغاز, context->keepalive, context->username);
			}ورنہ{
				log__printf(تہی, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d).",
						context->پتہ, context->remote_پورٹ, context->id, context->protocol, context->clean_آغاز, context->keepalive);
			}
		}ورنہ{
			اگر(context->username){
				log__printf(تہی, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->پتہ, context->remote_پورٹ, context->id, context->protocol, context->clean_آغاز, context->keepalive, context->username);
			}ورنہ{
				log__printf(تہی, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d).",
						context->پتہ, context->remote_پورٹ, context->id, context->protocol, context->clean_آغاز, context->keepalive);
			}
		}

		اگر(context->will){
			log__printf(تہی, MOSQ_LOG_DEBUG, "Will message specified (%ld bytes) (r%d, q%d).",
					(طویل)context->will->پیغام.payloadlen,
					context->will->پیغام.retain,
					context->will->پیغام.qos);

			log__printf(تہی, MOSQ_LOG_DEBUG, "\t%s", context->will->پیغام.topic);
		}ورنہ{
			log__printf(تہی, MOSQ_LOG_DEBUG, "No will message specified.");
		}
	}
#اگر_تعریف_شدہ WITH_TLS
	اگر(context->ssl){
		log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s negotiated %s cipher %s",
				context->id,
				SSL_get_cipher_نسخہ(context->ssl),
				SSL_get_cipher_name(context->ssl));
	}
#اگر_ختم

	context->ping_t = 0;
	context->is_dropping = false;

	/* Remove any queued messages that are no longer allowed through ACL,
	 * assuming a possible change of username. */
	db_check_acl_کا_all_messages(context);
	context_جوڑو_مقصد_by_id(context);

#اگر_تعریف_شدہ WITH_PERSISTENCE
	اگر(!context->clean_آغاز){
		db.persistence_changes++;
	}
#اگر_ختم
	context->max_qos = context->listener->max_qos;

	اگر(db.config->max_keepalive &&
			(context->keepalive > db.config->max_keepalive || context->keepalive == 0)){

		keepalive__remove(context);
		context->keepalive = db.config->max_keepalive;
		keepalive_جوڑو(context);
		اگر(context->protocol == mosq_p_mqtt5){
			اگر(mosquitto_property_جوڑو_int16(&connack_props, MQTT_PROP_SERVER_KEEP_ALIVE, context->keepalive)){
				rc = MOSQ_ERR_NOMEM;
				جاؤ error;
			}
		}ورنہ{
			send__connack(context, connect_ack, CONNACK_REFUSED_IDENTIFIER_REJECTED, تہی);
			rc = MOSQ_ERR_INVAL;
			جاؤ error;
		}
	}


	اگر(context->protocol == mosq_p_mqtt5){
		اگر(context->listener->max_topic_alias > 0){
			اگر(mosquitto_property_جوڑو_int16(&connack_props, MQTT_PROP_TOPIC_ALIAS_زیادہ_سے_زیادہ, context->listener->max_topic_alias)){
				rc = MOSQ_ERR_NOMEM;
				جاؤ error;
			}
		}
		اگر(context->assigned_id){
			اگر(mosquitto_property_جوڑو_string(&connack_props, MQTT_PROP_ASSIGNED_CLIENT_IDENTIFIER, context->id)){
				rc = MOSQ_ERR_NOMEM;
				جاؤ error;
			}
		}
		اگر(context->auth_طریقہ){
			اگر(mosquitto_property_جوڑو_string(&connack_props, MQTT_PROP_AUTHENTICATION_طریقہ, context->auth_طریقہ)){
				rc = MOSQ_ERR_NOMEM;
				جاؤ error;
			}

			اگر(auth_ڈیٹا_برآمد && auth_ڈیٹا_برآمد_len > 0){
				اگر(mosquitto_property_جوڑو_ثنائی(&connack_props, MQTT_PROP_AUTHENTICATION_ڈیٹا, auth_ڈیٹا_برآمد, auth_ڈیٹا_برآمد_len)){
					rc = MOSQ_ERR_NOMEM;
					جاؤ error;
				}
			}
		}
	}
	محفوظ_FREE(auth_ڈیٹا_برآمد);

	mosquitto_سیٹ_state(context, mosq_cs_فعال);
	rc = send__connack(context, connect_ack, CONNACK_ACCEPTED, connack_props);
	mosquitto_property_free_all(&connack_props);
	اگر(rc){
		واپس rc;
	}
	db__expire_all_messages(context);
	rc = db_پیغام_write_queued_برآمد(context);
	اگر(rc){
		واپس rc;
	}
	rc = db_پیغام_write_inflight_برآمد_all(context);

	اگر(rc == MOSQ_ERR_SUCCESS){
		plugin__handle_connect(context);

		اگر(context->session_expiry_interval != MQTT_SESSION_EXPIRY_فوری){
			plugin_persist_handle_client_جوڑو(context);
		}ورنہ اگر(context->will){
			plugin_persist_handle_will_جوڑو(context);
		}
	}
	واپس rc;
error:
	محفوظ_FREE(auth_ڈیٹا_برآمد);
	mosquitto_property_free_all(&connack_props);
	واپس rc;
}


ساکن صحیح will__read(ساخت mosquitto *context, ثابت حرف *clientid, ساخت mosquitto_پیغام_all **will, uint8_t will_qos, صحیح will_retain)
{
	صحیح rc = MOSQ_ERR_SUCCESS;
	size_t slen;
	uint16_t tlen;
	ساخت mosquitto_پیغام_all *will_struct = تہی;
	حرف *will_topic_mount = تہی;
	uint16_t payloadlen;
	mosquitto_property *properties = تہی;

	will_struct = mosquitto_calloc(1, حجم(ساخت mosquitto_پیغام_all));
	اگر(!will_struct){
		rc = MOSQ_ERR_NOMEM;
		جاؤ error_cleanup;
	}
	اگر(context->protocol == PROTOCOL_نسخہ_v5){
		rc = property__read_all(CMD_WILL, &context->اندر_packet, &properties);
		اگر(rc){
			جاؤ error_cleanup;
		}

		rc = property__process_will(context, will_struct, &properties);
		mosquitto_property_free_all(&properties);
		اگر(rc){
			جاؤ error_cleanup;
		}
	}
	rc = packet__read_string(&context->اندر_packet, &will_struct->پیغام.topic, &tlen);
	اگر(rc){
		جاؤ error_cleanup;
	}
	اگر(!tlen){
		log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s: Will with empty topic.",
				context->id);
		rc = MOSQ_ERR_PROTOCOL;
		جاؤ error_cleanup;
	}

	اگر(context->listener->mount_نقطہ){
		slen = لڑی_لمبائی(context->listener->mount_نقطہ) + لڑی_لمبائی(will_struct->پیغام.topic) + 1;
		will_topic_mount = mosquitto_malloc(slen+1);
		اگر(!will_topic_mount){
			rc = MOSQ_ERR_NOMEM;
			جاؤ error_cleanup;
		}

		snprintf(will_topic_mount, slen, "%s%s", context->listener->mount_نقطہ, will_struct->پیغام.topic);
		will_topic_mount[slen] = '\0';

		mosquitto_FREE(will_struct->پیغام.topic);
		will_struct->پیغام.topic = will_topic_mount;
	}

	اگر(!لڑی_موازنہ(will_struct->پیغام.topic, "$CONTROL/", لڑی_لمبائی("$CONTROL/"))){
		rc = MOSQ_ERR_ACL_DENIED;
		جاؤ error_cleanup;
	}
	rc = mosquitto_pub_topic_check(will_struct->پیغام.topic);
	اگر(rc){
		جاؤ error_cleanup;
	}

	rc = packet__read_uint16(&context->اندر_packet, &payloadlen);
	اگر(rc){
		جاؤ error_cleanup;
	}

	will_struct->پیغام.payloadlen = payloadlen;
	اگر(will_struct->پیغام.payloadlen > 0){
		اگر(db.config->پیغام_سائز_حد_بندی && will_struct->پیغام.payloadlen > (صحیح)db.config->پیغام_سائز_حد_بندی){
			log__printf(تہی, MOSQ_LOG_DEBUG, "Client %s connected with too large Will payload", clientid);
			اگر(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_PACKET_TOO_LARGE, تہی);
			}ورنہ{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, تہی);
			}
			rc = MOSQ_ERR_PAYLOAD_سائز;
			جاؤ error_cleanup;
		}
		will_struct->پیغام.payload = mosquitto_malloc((size_t)will_struct->پیغام.payloadlen);
		اگر(!will_struct->پیغام.payload){
			rc = MOSQ_ERR_NOMEM;
			جاؤ error_cleanup;
		}

		rc = packet_read_بائٹیں(&context->اندر_packet, will_struct->پیغام.payload, (uint32_t)will_struct->پیغام.payloadlen);
		اگر(rc){
			جاؤ error_cleanup;
		}
	}

	will_struct->پیغام.qos = will_qos;
	will_struct->پیغام.retain = will_retain;

	*will = will_struct;
	واپس MOSQ_ERR_SUCCESS;

error_cleanup:
	اگر(will_struct){
		mosquitto_FREE(will_struct->پیغام.topic);
		mosquitto_FREE(will_struct->پیغام.payload);
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct);
	}
	واپس rc;
}


ساکن صحیح check_protocol_نسخہ(ساخت mosquitto__listener *listener, صحیح protocol_نسخہ)
{
	/* Allow bridge protocol as well. */
	protocol_نسخہ &= 0x7F;

	اگر((protocol_نسخہ == 3 && listener->disable_protocol_v3 == false)
			|| (protocol_نسخہ == 4 && listener->disable_protocol_v4 == false)
			|| (protocol_نسخہ == 5 && listener->disable_protocol_v5 == false)
			){

		واپس MOSQ_ERR_SUCCESS;
	}ورنہ{
		واپس MOSQ_ERR_NOT_SUPPORTED;
	}
}


اندرونی ساکن صحیح send__connack_error_and_return(ساخت mosquitto *context, uint8_t err_رمز, صحیح rc)
{
	send__connack(context, 0, err_رمز, تہی);
	واپس rc;
}


اندرونی ساکن صحیح send__connack_bad_username_or_password_error(ساخت mosquitto *context, صحیح rc)
{
	uint8_t err_رمز  = context->protocol == mosq_p_mqtt5
									? (uint8_t)MQTT_RC_BAD_USERNAME_OR_PASSWORD
									: (uint8_t)CONNACK_REFUSED_BAD_USERNAME_PASSWORD;
	واپس send__connack_error_and_return(context, err_رمز, rc);
}


ساکن صحیح read_protocol_name(ساخت mosquitto *context, حرف protocol_name[7])
{
	/* Read protocol name as length then bytes rather than with read_string
	 * because the length is fixed and we can check that. Removes the need
	 * for another malloc as well. */

	uint16_t slen = 0;

	اگر(packet__read_uint16(&context->اندر_packet, &slen)){
		log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string length.",
				context->پتہ, context->remote_پورٹ);
		واپس MOSQ_ERR_PROTOCOL;
	}
	اگر(slen != 4 /* MQTT */ && slen != 6 /* MQIsdp */){
		log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with incorrect protocol string length (%d).",
				context->پتہ, context->remote_پورٹ, slen);
		واپس MOSQ_ERR_PROTOCOL;
	}
	اگر(packet_read_بائٹیں(&context->اندر_packet, protocol_name, slen)){
		log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string.",
				context->پتہ, context->remote_پورٹ);
		واپس MOSQ_ERR_PROTOCOL;
	}
	protocol_name[slen] = '\0';

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح read_and_verify_protocol_نسخہ(ساخت mosquitto *context, ثابت حرف *protocol_name,
		uint8_t *protocol_نسخہ)
{
	uint8_t tmp_protocol_نسخہ = 0;
	اگر(packet__read_byte(&context->اندر_packet, &tmp_protocol_نسخہ)){
		log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol version.",
				context->پتہ, context->remote_پورٹ);
		واپس MOSQ_ERR_PROTOCOL;
	}

	اگر(check_protocol_نسخہ(context->listener, tmp_protocol_نسخہ)){
		اگر(tmp_protocol_نسخہ == 3 || tmp_protocol_نسخہ == 4){
			context->protocol = mosq_p_mqtt311;
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_نسخہ, تہی);
		}ورنہ{
			context->protocol = mosq_p_mqtt5;
			send__connack(context, 0, MQTT_RC_UNSUPPORTED_PROTOCOL_نسخہ, تہی);
		}
		واپس MOSQ_ERR_NOT_SUPPORTED;
	}

	اگر(!strcmp(protocol_name, PROTOCOL_NAME_v31)){
		اگر((tmp_protocol_نسخہ&0x7F) != PROTOCOL_نسخہ_v31){
			اگر(db.config->connection_messages == true){
				log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->پتہ, context->remote_پورٹ, tmp_protocol_نسخہ);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_نسخہ, تہی);
			واپس MOSQ_ERR_PROTOCOL;
		}
		context->protocol = mosq_p_mqtt31;
		اگر((tmp_protocol_نسخہ&0x80) == 0x80){
			context->is_bridge = true;
		}
	}ورنہ اگر(!strcmp(protocol_name, PROTOCOL_NAME)){
		اگر((tmp_protocol_نسخہ&0x7F) == PROTOCOL_نسخہ_v311){
			context->protocol = mosq_p_mqtt311;

			اگر((tmp_protocol_نسخہ&0x80) == 0x80){
				context->is_bridge = true;
			}
		}ورنہ اگر((tmp_protocol_نسخہ&0x7F) == PROTOCOL_نسخہ_v5){
			context->protocol = mosq_p_mqtt5;
		}ورنہ{
			اگر(db.config->connection_messages == true){
				log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->پتہ, context->remote_پورٹ, tmp_protocol_نسخہ);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_نسخہ, تہی);
			واپس MOSQ_ERR_PROTOCOL;
		}
		اگر((context->اندر_packet.command&0x0F) != 0x00){
			/* Reserved flags not set to 0, must disconnect. */
			log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero reserved flags (%02X).",
					context->پتہ, context->remote_پورٹ, context->اندر_packet.command);
			واپس MOSQ_ERR_PROTOCOL;
		}
	}ورنہ{
		اگر(db.config->connection_messages == true){
			log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol \"%s\".",
					context->پتہ, context->remote_پورٹ, protocol_name);
		}
		واپس MOSQ_ERR_PROTOCOL;
	}
	اگر((tmp_protocol_نسخہ&0x7F) != PROTOCOL_نسخہ_v31 && context->اندر_packet.command != CMD_CONNECT){
		واپس MOSQ_ERR_MALFORMED_PACKET;
	}

	*protocol_نسخہ = tmp_protocol_نسخہ;
	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح read_and_verify_connect_flags(ساخت mosquitto *context, uint8_t *connect_flags)
{
	اگر(packet__read_byte(&context->اندر_packet, connect_flags)){
		log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing connect flags.",
				context->پتہ, context->remote_پورٹ);
		واپس MOSQ_ERR_PROTOCOL;
	}
	اگر(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		اگر((*connect_flags & 0x01) != 0x00){
			log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero connect reserved flag (%02X).",
					context->پتہ, context->remote_پورٹ, *connect_flags);
			واپس MOSQ_ERR_PROTOCOL;
		}
	}

	واپس MOSQ_ERR_SUCCESS;
}


ساکن خالی سیٹ_session_expiry_interval(ساخت mosquitto *context, uint8_t clean_آغاز, uint8_t protocol_نسخہ)
{
	/* session_expiry_interval will be overridden if the properties are read later */
	اگر(clean_آغاز == false && protocol_نسخہ != PROTOCOL_نسخہ_v5){
		/* v3* has clean_start == false mean the session never expires */
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
	}ورنہ{
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_فوری;
	}
}


ساکن صحیح read_and_reset_keepalive(ساخت mosquitto *context)
{
	/* _remove here because net__socket_accept() uses _add and we must have the
	 * correct keepalive value */
	keepalive__remove(context);

	اگر(packet__read_uint16(&context->اندر_packet, &(context->keepalive))){
		log__printf(تہی, MOSQ_LOG_INFO, "%s sent CONNECT with missing keepalive.",
				context->id);
		واپس MOSQ_ERR_PROTOCOL;
	}
	keepalive_جوڑو(context);

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح read_and_verify_v5_connect_properties(ساخت mosquitto *context, mosquitto_property **properties, uint8_t protocol_نسخہ)
{
	صحیح rc;

	اگر(protocol_نسخہ == PROTOCOL_نسخہ_v5){
		rc = property__read_all(CMD_CONNECT, &context->اندر_packet, properties);
		اگر(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
			send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, تہی);
		}ورنہ اگر(rc == MOSQ_ERR_MALFORMED_PACKET){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, تہی);
		}
		اگر(rc){
			واپس rc;
		}
	}
	rc = property__process_connect(context, properties);
	اگر(rc != MOSQ_ERR_SUCCESS){
		واپس send__connack_error_and_return(context, MQTT_RC_PROTOCOL_ERROR, rc);
	}

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح verify_will_options(ساخت mosquitto *context, uint8_t will, uint8_t will_qos, uint8_t will_retain, uint8_t protocol_نسخہ)
{
	اگر(will_qos == 3){
		log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid Will QoS (%d).",
				context->پتہ, context->remote_پورٹ, will_qos);
		واپس MOSQ_ERR_PROTOCOL;
	}

	اگر(will && will_retain && db.config->retain_دستیاب == false){
		اگر(protocol_نسخہ == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_RETAIN_NOT_SUPPORTED, تہی);
		}
		واپس MOSQ_ERR_NOT_SUPPORTED;
	}

	اگر(will && will_qos > context->listener->max_qos){
		اگر(protocol_نسخہ == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_QOS_NOT_SUPPORTED, تہی);
		}
		واپس MOSQ_ERR_NOT_SUPPORTED;
	}

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح handle_zero_لمبائی_clientid(ساخت mosquitto *context, حرف **clientid, bool *allow_zero_لمبائی_clientid,
		uint8_t clean_آغاز)
{
	اگر(context->protocol == mosq_p_mqtt31){
		log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s:%d: v3.1 CONNECT with zero length clientid.",
				context->پتہ, context->remote_پورٹ);
		send__connack(context, 0, CONNACK_REFUSED_IDENTIFIER_REJECTED, تہی);
		واپس MOSQ_ERR_PROTOCOL;
	}

	/* mqtt311/mqtt5 */
	mosquitto_FREE(*clientid);

	اگر(db.config->فی_listener_settings){
		*allow_zero_لمبائی_clientid = context->listener->security_options->allow_zero_لمبائی_clientid;
	}ورنہ{
		*allow_zero_لمبائی_clientid = db.config->security_options.allow_zero_لمبائی_clientid;
	}

	اگر((context->protocol == mosq_p_mqtt311 && clean_آغاز == 0) || *allow_zero_لمبائی_clientid == false){
		log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with zero length clientid when forbidden.",
				context->پتہ, context->remote_پورٹ);
		uint8_t err_رمز = context->protocol == mosq_p_mqtt311 ? (uint8_t)CONNACK_REFUSED_IDENTIFIER_REJECTED : (uint8_t)MQTT_RC_UNSPECIFIED;
		واپس send__connack_error_and_return(context, err_رمز, MOSQ_ERR_PROTOCOL);
	}

	*clientid = clientid_gen(&(uint16_t){0}, context->listener->security_options->auto_id_سابقہ,
			context->listener->security_options->auto_id_سابقہ_len);
	اگر(*clientid == تہی){
		واپس MOSQ_ERR_NOMEM;
	}
	context->assigned_id = true;

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح check_clientid_prefixes(ساخت mosquitto *context, ثابت حرف *clientid)
{
	اگر(db.config->clientid_prefixes){
		اگر(لڑی_موازنہ(db.config->clientid_prefixes, clientid, لڑی_لمبائی(db.config->clientid_prefixes))){
			uint8_t err_رمز = context->protocol == mosq_p_mqtt5 ? (uint8_t)MQTT_RC_NOT_AUTHORIZED : (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
			واپس send__connack_error_and_return(context, err_رمز, MOSQ_ERR_AUTH);
		}
	}
	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح read_and_verify_clientid_from_packet(ساخت mosquitto *context, حرف **clientid,
		bool *allow_zero_لمبائی_clientid, uint8_t clean_آغاز)
{
	صحیح rc;
	uint16_t slen;

	rc = packet__read_string(&context->اندر_packet, clientid, &slen);
	اگر(rc == MOSQ_ERR_MALFORMED_UTF8){
		اگر(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_CLIENTID_NOT_VALID, تہی);
		}
		واپس MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID;
	}ورنہ اگر(rc > 0){
		واپس MOSQ_ERR_PROTOCOL;
	}

	اگر(slen == 0){
		rc = handle_zero_لمبائی_clientid(context, clientid, allow_zero_لمبائی_clientid, clean_آغاز);
		اگر(rc != MOSQ_ERR_SUCCESS){
			واپس rc;
		}
	}

	rc = check_clientid_prefixes(context, *clientid);
	اگر(rc != MOSQ_ERR_SUCCESS){
		واپس rc;
	}

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح سیٹ_username_from_packet(ساخت mosquitto *context, حرف **username, ثابت حرف *clientid)
{
	صحیح rc;

	rc = packet__read_string(&context->اندر_packet, username, &(uint16_t){0});
	اگر(rc == MOSQ_ERR_NOMEM){
		واپس MOSQ_ERR_NOMEM;
	}
	اگر(rc != MOSQ_ERR_SUCCESS){
		اگر(context->protocol == mosq_p_mqtt31){
			/* Username flag given, but no username. Ignore. */
			/* NOTE: Removed setting of username_flag to zero as it is unused afterwards */
		}ورنہ{
			log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with username flag but no username.",
					clientid);
			واپس MOSQ_ERR_PROTOCOL;
		}
	}

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح سیٹ_password_from_packet(ساخت mosquitto *context, حرف **password, uint16_t *password_len, ثابت حرف *clientid)
{
	صحیح rc;

	rc = packet_read_ثنائی(&context->اندر_packet, (uint8_t **)password, password_len);
	اگر(rc == MOSQ_ERR_NOMEM){
		واپس MOSQ_ERR_NOMEM;
	}

	اگر(rc == MOSQ_ERR_MALFORMED_PACKET){
		اگر(context->protocol == mosq_p_mqtt31){
			/* Password flag given, but no password. Ignore. */
		}ورنہ{
			log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with password flag but no password.",
					clientid);
			واپس MOSQ_ERR_PROTOCOL;
		}
	}

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح read_and_verify_client_credentials_from_packet(ساخت mosquitto *context,
		حرف **username, uint8_t username_flag,
		حرف **password, uint16_t *password_len, uint8_t password_flag,
		ثابت حرف *clientid)
{
	صحیح rc;

	اگر(username_flag){
		rc = سیٹ_username_from_packet(context, username, clientid);
		اگر(rc != MOSQ_ERR_SUCCESS){
			واپس rc;
		}
	}ورنہ{
		اگر(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
			اگر(password_flag){
				/* username_flag == 0 && password_flag == 1 is forbidden */
				log__printf(تہی, MOSQ_LOG_ERR, "Protocol error from %s: password without username, closing connection.", clientid);
				واپس MOSQ_ERR_PROTOCOL;
			}
		}
	}
	اگر(password_flag){
		rc = سیٹ_password_from_packet(context, password, password_len, clientid);
		اگر(rc != MOSQ_ERR_SUCCESS){
			واپس rc;
		}
	}

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح check_additional_trailing_ڈیٹا(ساخت mosquitto *context, uint8_t protocol_نسخہ)
{
	اگر(context->اندر_packet.pos != context->اندر_packet.بقایا_لمبائی){
		/* Surplus data at end of packet, this must be an error. */
		log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->اندر_packet.pos, context->اندر_packet.بقایا_لمبائی);
		اگر(protocol_نسخہ == PROTOCOL_نسخہ_v5){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, تہی);
		}
		واپس MOSQ_ERR_PROTOCOL;
	}

	واپس MOSQ_ERR_SUCCESS;
}

#اگر_تعریف_شدہ WITH_TLS


اندرونی ساکن صحیح get_client_cert_and_subject_name(ساخت mosquitto *context, X509 **client_cert, X509_NAME **name)
{
	*client_cert = SSL_get_peer_certificate(context->ssl);
	اگر(*client_cert == تہی){
		واپس send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	*name = X509_get_subject_name(*client_cert);
	اگر(*name == تہی){
		X509_free(*client_cert);
		واپس send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	واپس MOSQ_ERR_SUCCESS;
}


اندرونی ساکن صحیح free_x509_and_send_connack_error(ساخت mosquitto *context, X509 *client_cert, صحیح rc)
{
	X509_free(client_cert);
	واپس send__connack_bad_username_or_password_error(context, rc);
}


اندرونی ساکن صحیح free_x509_and_BIO_and_send_connack_error(ساخت mosquitto *context, X509 *client_cert,
		BIO *subject_name, صحیح rc)
{
	BIO_free(subject_name);
	واپس free_x509_and_send_connack_error(context, client_cert, rc);
}


ساکن صحیح سیٹ_username_from_cert_شناختی_عنصر(ساخت mosquitto *context)
{
	X509 *client_cert = تہی;
	X509_NAME *name = تہی;

	اگر(get_client_cert_and_subject_name(context, &client_cert, &name)){
		واپس MOSQ_ERR_AUTH;
	}

	صحیح i = -1;
	X509_NAME_اندراج *name_اندراج = تہی;

	i = X509_NAME_get_اشاریہ_by_NID(name, NID_commonName, -1);
	اگر(i == -1){
		واپس free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	name_اندراج = X509_NAME_get_اندراج(name, i);
	اگر(!name_اندراج){
		جاؤ success;
	}

	ASN1_STRING *name_asn1 = تہی;
	name_asn1 = X509_NAME_اندراج_get_ڈیٹا(name_اندراج);
	اگر(name_asn1 == تہی){
		واپس free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	ثابت حرف *cert_شناختی_عنصر = (ثابت حرف *)ASN1_STRING_get0_ڈیٹا(name_asn1);
	اگر(!cert_شناختی_عنصر || mosquitto_validate_utf8(cert_شناختی_عنصر, (صحیح)لڑی_لمبائی(cert_شناختی_عنصر))){
		واپس free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	mosquitto_free(context->username);
	context->username = mosquitto_strdup(cert_شناختی_عنصر);
	اگر(context->username == تہی){
		واپس free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_NOMEM);
	}
	/* Make sure there isn't an embedded NUL character in the CN */
	اگر((size_t)ASN1_STRING_لمبائی(name_asn1) != لڑی_لمبائی(context->username)){
		واپس free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}

success:
	X509_free(client_cert);
	client_cert = تہی;
	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح سیٹ_username_from_cert_subject_name(ساخت mosquitto *context)
{
	X509 *client_cert = تہی;
	X509_NAME *name = تہی;

	اگر(get_client_cert_and_subject_name(context, &client_cert, &name)){
		واپس MOSQ_ERR_AUTH;
	}

	حرف *subject = تہی;
	حرف *ڈیٹا_آغاز = تہی;
	BIO *subject_bio = تہی;
	طویل name_لمبائی = 0;

	subject_bio = BIO_new(BIO_s_mem());
	X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
	ڈیٹا_آغاز = تہی;
	name_لمبائی = BIO_get_mem_ڈیٹا(subject_bio, &ڈیٹا_آغاز);
	subject = mosquitto_malloc(حجم(حرف)*(size_t)(name_لمبائی+1));
	اگر(!subject){
		واپس free_x509_and_BIO_and_send_connack_error(context, client_cert, subject_bio, MOSQ_ERR_NOMEM);
	}
	حافظہ_نقل(subject, ڈیٹا_آغاز, (size_t)name_لمبائی);
	subject[name_لمبائی] = '\0';
	BIO_free(subject_bio);

	اگر(mosquitto_validate_utf8(subject, (صحیح)لڑی_لمبائی(subject))){
		mosquitto_free(subject);
		واپس free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	context->username = subject;
	اگر(!context->username){
		X509_free(client_cert);
		واپس MOSQ_ERR_AUTH;
	}

	X509_free(client_cert);
	client_cert = تہی;
	واپس MOSQ_ERR_SUCCESS;
}
#اگر_ختم


ساکن صحیح handle_username_from_cert_options(ساخت mosquitto *context, حرف **username, حرف **password, uint16_t password_len)
{
	صحیح rc;

#اگر_تعریف_شدہ WITH_TLS
	اگر(context->listener->ssl_ctx && (context->listener->use_شناختی_عنصر_as_username || context->listener->use_subject_as_username)){
		/* Don't need the username or password if provided */
		mosquitto_FREE(*username);
		mosquitto_FREE(*password);

		اگر(!context->ssl){
			واپس send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
		}
#اگر_تعریف_شدہ FINAL_WITH_TLS_PSK
		اگر(context->listener->psk_hint){
			/* Client should have provided an identity to get this far. */
			اگر(!context->username){
				واپس send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}ورنہ
#اگر_ختم /* FINAL_WITH_TLS_PSK */
		{
			اگر(context->listener->use_شناختی_عنصر_as_username){
				rc = سیٹ_username_from_cert_شناختی_عنصر(context);
			}ورنہ{   /* use_subject_as_username */
				rc = سیٹ_username_from_cert_subject_name(context);
			}
			اگر(rc){
				واپس rc;
			}
		}
	}ورنہ
#اگر_ختم /* WITH_TLS */
	{
#اگر_تعریف_شدہ WITH_TLS
		اگر(context->listener->use_شناختی_عنصر_as_username && context->listener->require_certificate){
			mosquitto_FREE(*username);
			mosquitto_FREE(*password);

			اگر(!context->username){
				واپس send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}ورنہ
#اگر_ختم
		{
			/* FIXME - these ensure the mosquitto_clientid() and
			* mosquitto_client_username() functions work, but is hacky */
			context->username = *username;
			context->password = *password;
			context->password_len = password_len;
			*username = تہی; /* Avoid free() in error: below. */
			*password = تہی;
		}
	}

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح handle_username_as_clientid_option(ساخت mosquitto *context)
{
	اگر(context->username){
		mosquitto_FREE(context->id);
		context->id = mosquitto_strdup(context->username);
		اگر(!context->id){
			واپس MOSQ_ERR_NOMEM;
		}
	}ورنہ{
		uint8_t err_رمز = context->protocol == mosq_p_mqtt5
												? (uint8_t)MQTT_RC_NOT_AUTHORIZED
												: (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
		واپس send__connack_error_and_return(context, err_رمز, MOSQ_ERR_AUTH);
	}

	واپس MOSQ_ERR_SUCCESS;
}


صحیح handle__connect(ساخت mosquitto *context)
{
	حرف protocol_name[7];
	uint8_t protocol_نسخہ;
	uint8_t connect_flags;
	حرف *clientid = تہی;
	ساخت mosquitto *ملا_context;
	ساخت mosquitto_پیغام_all *will_struct = تہی;
	uint8_t will, will_retain, will_qos, clean_آغاز;
	uint8_t username_flag, password_flag;
	حرف *username = تہی, *password = تہی;
	uint16_t password_len = 0;
	صحیح rc;
	mosquitto_property *properties = تہی;
	خالی *auth_ڈیٹا = تہی;
	uint16_t auth_ڈیٹا_len = 0;
	خالی *auth_ڈیٹا_برآمد = تہی;
	uint16_t auth_ڈیٹا_برآمد_len = 0;
	bool allow_zero_لمبائی_clientid;

	اگر(!context->listener){
		واپس MOSQ_ERR_INVAL;
	}

	/* Don't accept multiple CONNECT commands. */
	اگر(context->state != mosq_cs_new){
		log__printf(تہی, MOSQ_LOG_NOTICE, "Bad client %s:%d sending multiple CONNECT messages.",
				context->پتہ, context->remote_پورٹ);
		rc = MOSQ_ERR_PROTOCOL;
		جاؤ handle_connect_error;
	}

#اگر_تعریف_شدہ WITH_TLS
	اگر(context->اندر_packet.command == 0x16 && context->listener->ssl_ctx == تہی){ /* 0x16 is TLS handshake client hello */
		log__printf(تہی, MOSQ_LOG_NOTICE, "Client from %s:%d appears to be using TLS to connect to a non-TLS listener.",
				context->پتہ, context->remote_پورٹ);
		rc = MOSQ_ERR_PROTOCOL;
		جاؤ handle_connect_error;
	}
#اگر_ختم

	rc = read_protocol_name(context, protocol_name);
	اگر(rc != MOSQ_ERR_SUCCESS){
		جاؤ handle_connect_error;
	}

	rc = read_and_verify_protocol_نسخہ(context, protocol_name, &protocol_نسخہ);
	اگر(rc != MOSQ_ERR_SUCCESS){
		اگر(rc == MOSQ_ERR_MALFORMED_PACKET){
			واپس rc;
		}
		جاؤ handle_connect_error;
	}

	rc = read_and_verify_connect_flags(context, &connect_flags);
	اگر(rc != MOSQ_ERR_SUCCESS){
		جاؤ handle_connect_error;
	}

	clean_آغاز = (connect_flags & 0x02) >> 1;
	سیٹ_session_expiry_interval(context, clean_آغاز, protocol_نسخہ);

	rc = read_and_reset_keepalive(context);
	اگر(rc != MOSQ_ERR_SUCCESS){
		جاؤ handle_connect_error;
	}

	rc = read_and_verify_v5_connect_properties(context, &properties, protocol_نسخہ);
	اگر(rc != MOSQ_ERR_SUCCESS){
		جاؤ handle_connect_error;
	}

	will = connect_flags & 0x04;
	will_qos = (connect_flags & 0x18) >> 3;
	will_retain = ((connect_flags & 0x20) == 0x20);
	rc = verify_will_options(context, will, will_qos, will_retain, protocol_نسخہ);
	اگر(rc != MOSQ_ERR_SUCCESS){
		جاؤ handle_connect_error;
	}

	mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_طریقہ, &context->auth_طریقہ, false);
	mosquitto_property_read_ثنائی(properties, MQTT_PROP_AUTHENTICATION_ڈیٹا, &auth_ڈیٹا, &auth_ڈیٹا_len, false);
	mosquitto_property_free_all(&properties);

	اگر(auth_ڈیٹا && !context->auth_طریقہ){
		send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, تہی);
		log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s:%d CONNECT with missing clientid string.",
				context->پتہ, context->remote_پورٹ);
		rc = MOSQ_ERR_PROTOCOL;
		جاؤ handle_connect_error;
	}

	rc = read_and_verify_clientid_from_packet(context, &clientid, &allow_zero_لمبائی_clientid, clean_آغاز);
	اگر(rc != MOSQ_ERR_SUCCESS){
		جاؤ handle_connect_error;
	}

	اگر(will){
		rc = will__read(context, clientid, &will_struct, will_qos, will_retain);
		اگر(rc){
			اگر(context->protocol == mosq_p_mqtt5){
				اگر(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
					send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, تہی);
				}ورنہ اگر(rc == MOSQ_ERR_MALFORMED_PACKET){
					send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, تہی);
				}
			}
			جاؤ handle_connect_error;
		}
	}ورنہ{
		اگر(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
			اگر(will_qos != 0 || will_retain != 0){
				log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT without Will with non-zero QoS (%d) or retain (%d).",
						clientid, will_qos, will_retain);
				rc = MOSQ_ERR_PROTOCOL;
				جاؤ handle_connect_error;
			}
		}
	}

	// Client credentials
	password_flag = connect_flags & 0x40;
	username_flag = connect_flags & 0x80;
	rc = read_and_verify_client_credentials_from_packet(context, &username, username_flag, &password, &password_len, password_flag, clientid);
	اگر(rc != MOSQ_ERR_SUCCESS){
		جاؤ handle_connect_error;
	}

	rc = check_additional_trailing_ڈیٹا(context, protocol_نسخہ);
	اگر(rc != MOSQ_ERR_SUCCESS){
		جاؤ handle_connect_error;
	}

	/* Once context->id is set, if we return from this function with an error
	 * we must make sure that context->id is freed and set to NULL, so that the
	 * client isn't erroneously removed from the by_id hash table. */
	context->id = clientid;
	clientid = تہی;

	/* use_identity_as_username or use_subject_as_username */
	rc = handle_username_from_cert_options(context, &username, &password, password_len);
	اگر(rc != MOSQ_ERR_SUCCESS){
		جاؤ handle_connect_error;
	}

	/* use_username_as_clientid */
	اگر(context->listener->use_username_as_clientid){
		rc = handle_username_as_clientid_option(context);
		اگر(rc != MOSQ_ERR_SUCCESS){
			جاؤ handle_connect_error;
		}
	}

	/* Check for an existing delayed auth check, reject if present */
	HASH_ڈھونڈو(hh_id, db.contexts_by_id_delayed_auth, context->id, لڑی_لمبائی(context->id), ملا_context);
	اگر(ملا_context){
		rc = MOSQ_ERR_UNKNOWN;
		جاؤ handle_connect_error;
	}

	context->clean_آغاز = clean_آغاز;
	context->will = will_struct;
	will_struct = تہی;

	اگر(context->auth_طریقہ){
		rc = mosquitto_security_auth_آغاز(context, false, auth_ڈیٹا, auth_ڈیٹا_len, &auth_ڈیٹا_برآمد, &auth_ڈیٹا_برآمد_len);
		mosquitto_FREE(auth_ڈیٹا);
		اگر(rc == MOSQ_ERR_SUCCESS){
			واپس connect__on_authorised(context, auth_ڈیٹا_برآمد, auth_ڈیٹا_برآمد_len);
		}ورنہ اگر(rc == MOSQ_ERR_AUTH_CONTINUE){
			mosquitto_سیٹ_state(context, mosq_cs_authenticating);
			rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_ڈیٹا_برآمد, auth_ڈیٹا_برآمد_len);
			محفوظ_FREE(auth_ڈیٹا_برآمد);
			واپس rc;
		}ورنہ{
			محفوظ_FREE(auth_ڈیٹا_برآمد);
			will__clear(context);
			اگر(rc == MOSQ_ERR_AUTH){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, تہی);
				mosquitto_FREE(context->id);
				جاؤ handle_connect_error;
			}ورنہ اگر(rc == MOSQ_ERR_NOT_SUPPORTED){
				/* Client has requested extended authentication, but we don't support it. */
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_طریقہ, تہی);
				mosquitto_FREE(context->id);
				جاؤ handle_connect_error;
			}ورنہ{
				mosquitto_FREE(context->id);
				جاؤ handle_connect_error;
			}
		}
	}ورنہ{
#اگر_تعریف_شدہ WITH_TLS
		اگر(context->listener->ssl_ctx && (context->listener->use_شناختی_عنصر_as_username || context->listener->use_subject_as_username)){
			/* Authentication assumed to be cleared */
		}ورنہ
#اگر_ختم
		{
			rc = mosquitto_basic_auth(context);
			انتخاب(rc){
				حالت MOSQ_ERR_SUCCESS:
					توڑو;
				حالت MOSQ_ERR_AUTH_DELAYED:
					mosquitto_سیٹ_state(context, mosq_cs_delayed_auth);
					HASH_جوڑو_KEYPTR(hh_id, db.contexts_by_id_delayed_auth, context->id, لڑی_لمبائی(context->id), context);
					واپس MOSQ_ERR_SUCCESS;
					توڑو;
				حالت MOSQ_ERR_AUTH:
					اگر(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, تہی);
					}ورنہ{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, تہی);
					}
					جاؤ handle_connect_error;
					توڑو;
				حالت MOSQ_ERR_UNSPECIFIED:
				حالت MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
				حالت MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
				حالت MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
				حالت MOSQ_ERR_SERVER_UNAVAILABLE:
				حالت MOSQ_ERR_SERVER_BUSY:
				حالت MOSQ_ERR_BANNED:
				حالت MOSQ_ERR_BAD_AUTHENTICATION_طریقہ:
				حالت MOSQ_ERR_CONNECTION_شرح_EXCEEDED:
					اگر(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, (uint8_t)rc, تہی);
					}ورنہ{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, تہی);
					}
					جاؤ handle_connect_error;
					توڑو;
				طےشدہ:
					rc = MOSQ_ERR_UNKNOWN;
					جاؤ handle_connect_error;
					توڑو;
			}
		}
		واپس connect__on_authorised(context, تہی, 0);
	}


handle_connect_error:
	mosquitto_property_free_all(&properties);
	mosquitto_FREE(auth_ڈیٹا);
	mosquitto_FREE(clientid);
	mosquitto_FREE(username);
	mosquitto_FREE(password);
	اگر(will_struct){
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct->پیغام.payload);
		mosquitto_FREE(will_struct->پیغام.topic);
		mosquitto_FREE(will_struct);
	}
	will__clear(context);
	/* We return an error here which means the client is freed later on. */
	context->clean_آغاز = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_فوری;
	context->will_delay_interval = 0;
	واپس rc;
}
