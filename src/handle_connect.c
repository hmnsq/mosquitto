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
#جے_انت


ساكن حرف nibble_منزل_hex(uint8_t قدر)
{
	إذا(قدر < 0x0A){
		أرجع (حرف)('0'+قدر);
	}وإلا{
		أرجع (حرف)(65 /*'A'*/ +قدر-10);
	}
}


ساكن حرف *clientid_gen(uint16_t *idlen, ثابت حرف *auto_id_اگیتر, uint16_t auto_id_اگیتر_len)
{
	حرف *clientid;
	uint8_t rnd[16];
	صحيح pos;

	إذا(mosquitto_getrandom(rnd, 16)){
		أرجع عدم;
	}

	*idlen = (uint16_t)(auto_id_اگیتر_len + 36);

	clientid = (حرف *)mosquitto_calloc((size_t)(*idlen) + 1, حجم(حرف));
	إذا(!clientid){
		أرجع عدم;
	}
	إذا(auto_id_اگیتر){
		حافظہ_نقل(clientid, auto_id_اگیتر, auto_id_اگیتر_len);
	}

	pos = 0;
	لكل(صحيح i=0; i<16; i++){
		clientid[auto_id_اگیتر_len + pos + 0] = nibble_منزل_hex(rnd[i] & 0x0F);
		clientid[auto_id_اگیتر_len + pos + 1] = nibble_منزل_hex((rnd[i] >> 4) & 0x0F);
		pos += 2;
		إذا(pos == 8 || pos == 13 || pos == 18 || pos == 23){
			clientid[auto_id_اگیتر_len + pos] = '-';
			pos++;
		}
	}

	أرجع clientid;
}


صحيح connect__on_authorised(بنية mosquitto *context, فراغ *auth_انکڑے_باہرلا, uint16_t auth_انکڑے_باہرلا_len)
{
	بنية mosquitto *لبھیا_context;
	بنية mosquitto__subleaf *پتی;
	mosquitto_property *connack_props = عدم;
	uint8_t connect_ack = 0;
	صحيح rc;
	صحيح وچ_quota, باہرلا_quota;
	uint16_t وچ_ودھ_توں_ودھ, باہرلا_ودھ_توں_ودھ;

	/* Find if this client already has an entry. This must be done *after* any security checks. */
	HASH_لبھو(hh_id, db.contexts_by_id, context->id, لڑی_لمبائی(context->id), لبھیا_context);
	إذا(لبھیا_context){
		/* Found a matching client */
		إذا(!net__is_connected(لبھیا_context)){
			/* Client is reconnecting after a disconnect */
			/* FIXME - does anything need to be done here? */
		}وإلا{
			/* Client is already connected, disconnect old version. This is
			 * done in context__cleanup() below. */
		}

		إذا(context->clean_شروع == true){
			sub__clean_session(لبھیا_context);
			لبھیا_context->session_expiry_interval = MQTT_SESSION_EXPIRY_فوری;
			plugin_persist__handle_client_delete(لبھیا_context);
		}
		context->is_persisted = لبھیا_context->is_persisted;
		لبھیا_context->is_persisted = false; /* stops persistence for context being removed */

		إذا(context->clean_شروع == false && لبھیا_context->session_expiry_interval != MQTT_SESSION_EXPIRY_فوری){
			إذا(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
				connect_ack |= 0x01;
			}

			إذا(لبھیا_context->msgs_وچ.inflight || لبھیا_context->msgs_وچ.queued
					|| لبھیا_context->msgs_باہرلا.inflight || لبھیا_context->msgs_باہرلا.queued){

				وچ_quota = context->msgs_وچ.inflight_quota;
				باہرلا_quota = context->msgs_باہرلا.inflight_quota;
				وچ_ودھ_توں_ودھ = context->msgs_وچ.inflight_ودھ_توں_ودھ;
				باہرلا_ودھ_توں_ودھ = context->msgs_باہرلا.inflight_ودھ_توں_ودھ;

				حافظہ_نقل(&context->msgs_وچ, &لبھیا_context->msgs_وچ, حجم(بنية mosquitto_پیغام_انکڑے));
				حافظہ_نقل(&context->msgs_باہرلا, &لبھیا_context->msgs_باہرلا, حجم(بنية mosquitto_پیغام_انکڑے));
				context->آخری_cmsg_id = لبھیا_context->آخری_cmsg_id;

				memset(&لبھیا_context->msgs_وچ, 0, حجم(بنية mosquitto_پیغام_انکڑے));
				memset(&لبھیا_context->msgs_باہرلا, 0, حجم(بنية mosquitto_پیغام_انکڑے));

				context->msgs_وچ.inflight_quota = وچ_quota;
				context->msgs_باہرلا.inflight_quota = باہرلا_quota;
				context->msgs_وچ.inflight_ودھ_توں_ودھ = وچ_ودھ_توں_ودھ;
				context->msgs_باہرلا.inflight_ودھ_توں_ودھ = باہرلا_ودھ_توں_ودھ;

				db_سنیہا_reconnect_reset(context);
			}
			context->subs = لبھیا_context->subs;
			لبھیا_context->subs = عدم;
			context->subs_سمرتھا = لبھیا_context->subs_سمرتھا;
			context->subs_گنتی = لبھیا_context->subs_گنتی;
			لبھیا_context->subs_سمرتھا = 0;
			لبھیا_context->subs_گنتی = 0;
			context->آخری_mid = لبھیا_context->آخری_mid;

			لكل(صحيح i=0; i<context->subs_سمرتھا; i++){
				إذا(context->subs[i]){
					پتی = context->subs[i]->hier->subs;
					طالما(پتی){
						إذا(پتی->context == لبھیا_context){
							پتی->context = context;
						}
						پتی = پتی->اگلا;
					}

					إذا(context->subs[i]->shared){
						پتی = context->subs[i]->shared->subs;
						طالما(پتی){
							إذا(پتی->context == لبھیا_context){
								پتی->context = context;
							}
							پتی = پتی->اگلا;
						}
					}
				}
			}
		}

		إذا((لبھیا_context->protocol == mosq_p_mqtt5 && لبھیا_context->session_expiry_interval == MQTT_SESSION_EXPIRY_فوری)
				|| (لبھیا_context->protocol != mosq_p_mqtt5 && لبھیا_context->clean_شروع == true)
				|| (context->clean_شروع == true)
				){

			context__send_will(لبھیا_context);
		}

		session_expiry__remove(لبھیا_context);
		will_delay__remove(لبھیا_context);
		will__clear(لبھیا_context);

		لبھیا_context->clean_شروع = true;
		لبھیا_context->session_expiry_interval = MQTT_SESSION_EXPIRY_فوری;
		mosquitto_سیٹ_state(لبھیا_context, mosq_cs_duplicate);
		إذا(لبھیا_context->protocol == mosq_p_mqtt5){
			send__disconnect(لبھیا_context, MQTT_RC_SESSION_لیا_OVER, عدم);
		}
		do_disconnect(لبھیا_context, MOSQ_ERR_SESSION_لیا_OVER);
	}

	إذا(db.config->global_max_clients > 0 && HASH_CNT(hh_id, db.contexts_by_id) >= (بنانشانی صحيح)db.config->global_max_clients){
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
						context->پتہ, context->remote_پورٹ, context->id, context->protocol, context->clean_شروع, context->keepalive, context->username);
			}وإلا{
				log__printf(عدم, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d).",
						context->پتہ, context->remote_پورٹ, context->id, context->protocol, context->clean_شروع, context->keepalive);
			}
		}وإلا{
			إذا(context->username){
				log__printf(عدم, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->پتہ, context->remote_پورٹ, context->id, context->protocol, context->clean_شروع, context->keepalive, context->username);
			}وإلا{
				log__printf(عدم, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d).",
						context->پتہ, context->remote_پورٹ, context->id, context->protocol, context->clean_شروع, context->keepalive);
			}
		}

		إذا(context->will){
			log__printf(عدم, MOSQ_LOG_DEBUG, "Will message specified (%ld bytes) (r%d, q%d).",
					(لما)context->will->پیغام.payloadlen,
					context->will->پیغام.retain,
					context->will->پیغام.qos);

			log__printf(عدم, MOSQ_LOG_DEBUG, "\t%s", context->will->پیغام.topic);
		}وإلا{
			log__printf(عدم, MOSQ_LOG_DEBUG, "No will message specified.");
		}
	}
#جے_تعریف_شدہ WITH_TLS
	إذا(context->ssl){
		log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s negotiated %s cipher %s",
				context->id,
				SSL_get_cipher_نسخہ(context->ssl),
				SSL_get_cipher_name(context->ssl));
	}
#جے_انت

	context->ping_t = 0;
	context->is_dropping = false;

	/* Remove any queued messages that are no longer allowed through ACL,
	 * assuming a possible change of username. */
	db_check_acl_دا_all_messages(context);
	context_جوڑو_منزل_by_id(context);

#جے_تعریف_شدہ WITH_PERSISTENCE
	إذا(!context->clean_شروع){
		db.persistence_changes++;
	}
#جے_انت
	context->max_qos = context->listener->max_qos;

	إذا(db.config->max_keepalive &&
			(context->keepalive > db.config->max_keepalive || context->keepalive == 0)){

		keepalive__remove(context);
		context->keepalive = db.config->max_keepalive;
		keepalive_جوڑو(context);
		إذا(context->protocol == mosq_p_mqtt5){
			إذا(mosquitto_property_جوڑو_int16(&connack_props, MQTT_PROP_SERVER_KEEP_ALIVE, context->keepalive)){
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
			إذا(mosquitto_property_جوڑو_int16(&connack_props, MQTT_PROP_TOPIC_ALIAS_ودھ_توں_ودھ, context->listener->max_topic_alias)){
				rc = MOSQ_ERR_NOMEM;
				اذهب error;
			}
		}
		إذا(context->assigned_id){
			إذا(mosquitto_property_جوڑو_string(&connack_props, MQTT_PROP_ASSIGNED_CLIENT_IDENTIFIER, context->id)){
				rc = MOSQ_ERR_NOMEM;
				اذهب error;
			}
		}
		إذا(context->auth_طریقہ){
			إذا(mosquitto_property_جوڑو_string(&connack_props, MQTT_PROP_AUTHENTICATION_طریقہ, context->auth_طریقہ)){
				rc = MOSQ_ERR_NOMEM;
				اذهب error;
			}

			إذا(auth_انکڑے_باہرلا && auth_انکڑے_باہرلا_len > 0){
				إذا(mosquitto_property_جوڑو_ثنائی(&connack_props, MQTT_PROP_AUTHENTICATION_انکڑے, auth_انکڑے_باہرلا, auth_انکڑے_باہرلا_len)){
					rc = MOSQ_ERR_NOMEM;
					اذهب error;
				}
			}
		}
	}
	محفوظ_FREE(auth_انکڑے_باہرلا);

	mosquitto_سیٹ_state(context, mosq_cs_سرگرم);
	rc = send__connack(context, connect_ack, CONNACK_ACCEPTED, connack_props);
	mosquitto_property_free_all(&connack_props);
	إذا(rc){
		أرجع rc;
	}
	db__expire_all_messages(context);
	rc = db_سنیہا_write_queued_باہرلا(context);
	إذا(rc){
		أرجع rc;
	}
	rc = db_سنیہا_write_inflight_باہرلا_all(context);

	إذا(rc == MOSQ_ERR_SUCCESS){
		plugin__handle_connect(context);

		إذا(context->session_expiry_interval != MQTT_SESSION_EXPIRY_فوری){
			plugin_persist_handle_client_جوڑو(context);
		}وإلا إذا(context->will){
			plugin_persist_handle_will_جوڑو(context);
		}
	}
	أرجع rc;
error:
	محفوظ_FREE(auth_انکڑے_باہرلا);
	mosquitto_property_free_all(&connack_props);
	أرجع rc;
}


ساكن صحيح will__read(بنية mosquitto *context, ثابت حرف *clientid, بنية mosquitto_سنیہا_all **will, uint8_t will_qos, صحيح will_retain)
{
	صحيح rc = MOSQ_ERR_SUCCESS;
	size_t slen;
	uint16_t tlen;
	بنية mosquitto_سنیہا_all *will_struct = عدم;
	حرف *will_topic_mount = عدم;
	uint16_t payloadlen;
	mosquitto_property *properties = عدم;

	will_struct = mosquitto_calloc(1, حجم(بنية mosquitto_سنیہا_all));
	إذا(!will_struct){
		rc = MOSQ_ERR_NOMEM;
		اذهب error_cleanup;
	}
	إذا(context->protocol == PROTOCOL_نسخہ_v5){
		rc = property__read_all(CMD_WILL, &context->وچ_packet, &properties);
		إذا(rc){
			اذهب error_cleanup;
		}

		rc = property__process_will(context, will_struct, &properties);
		mosquitto_property_free_all(&properties);
		إذا(rc){
			اذهب error_cleanup;
		}
	}
	rc = packet__read_string(&context->وچ_packet, &will_struct->پیغام.topic, &tlen);
	إذا(rc){
		اذهب error_cleanup;
	}
	إذا(!tlen){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Will with empty topic.",
				context->id);
		rc = MOSQ_ERR_PROTOCOL;
		اذهب error_cleanup;
	}

	إذا(context->listener->mount_نقطہ){
		slen = لڑی_لمبائی(context->listener->mount_نقطہ) + لڑی_لمبائی(will_struct->پیغام.topic) + 1;
		will_topic_mount = mosquitto_malloc(slen+1);
		إذا(!will_topic_mount){
			rc = MOSQ_ERR_NOMEM;
			اذهب error_cleanup;
		}

		snprintf(will_topic_mount, slen, "%s%s", context->listener->mount_نقطہ, will_struct->پیغام.topic);
		will_topic_mount[slen] = '\0';

		mosquitto_FREE(will_struct->پیغام.topic);
		will_struct->پیغام.topic = will_topic_mount;
	}

	إذا(!لڑی_مقابلہ(will_struct->پیغام.topic, "$CONTROL/", لڑی_لمبائی("$CONTROL/"))){
		rc = MOSQ_ERR_ACL_DENIED;
		اذهب error_cleanup;
	}
	rc = mosquitto_pub_topic_check(will_struct->پیغام.topic);
	إذا(rc){
		اذهب error_cleanup;
	}

	rc = packet__read_uint16(&context->وچ_packet, &payloadlen);
	إذا(rc){
		اذهب error_cleanup;
	}

	will_struct->پیغام.payloadlen = payloadlen;
	إذا(will_struct->پیغام.payloadlen > 0){
		إذا(db.config->سنیہا_ناپ_حد_بندی && will_struct->پیغام.payloadlen > (صحيح)db.config->سنیہا_ناپ_حد_بندی){
			log__printf(عدم, MOSQ_LOG_DEBUG, "Client %s connected with too large Will payload", clientid);
			إذا(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_PACKET_TOO_LARGE, عدم);
			}وإلا{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, عدم);
			}
			rc = MOSQ_ERR_PAYLOAD_ناپ;
			اذهب error_cleanup;
		}
		will_struct->پیغام.payload = mosquitto_malloc((size_t)will_struct->پیغام.payloadlen);
		إذا(!will_struct->پیغام.payload){
			rc = MOSQ_ERR_NOMEM;
			اذهب error_cleanup;
		}

		rc = packet_read_بائٹاں(&context->وچ_packet, will_struct->پیغام.payload, (uint32_t)will_struct->پیغام.payloadlen);
		إذا(rc){
			اذهب error_cleanup;
		}
	}

	will_struct->پیغام.qos = will_qos;
	will_struct->پیغام.retain = will_retain;

	*will = will_struct;
	أرجع MOSQ_ERR_SUCCESS;

error_cleanup:
	إذا(will_struct){
		mosquitto_FREE(will_struct->پیغام.topic);
		mosquitto_FREE(will_struct->پیغام.payload);
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct);
	}
	أرجع rc;
}


ساكن صحيح check_protocol_نسخہ(بنية mosquitto__listener *listener, صحيح protocol_نسخہ)
{
	/* Allow bridge protocol as well. */
	protocol_نسخہ &= 0x7F;

	إذا((protocol_نسخہ == 3 && listener->disable_protocol_v3 == false)
			|| (protocol_نسخہ == 4 && listener->disable_protocol_v4 == false)
			|| (protocol_نسخہ == 5 && listener->disable_protocol_v5 == false)
			){

		أرجع MOSQ_ERR_SUCCESS;
	}وإلا{
		أرجع MOSQ_ERR_NOT_SUPPORTED;
	}
}


مضمن ساكن صحيح send__connack_error_and_return(بنية mosquitto *context, uint8_t err_کوڈ, صحيح rc)
{
	send__connack(context, 0, err_کوڈ, عدم);
	أرجع rc;
}


مضمن ساكن صحيح send__connack_bad_username_or_password_error(بنية mosquitto *context, صحيح rc)
{
	uint8_t err_کوڈ  = context->protocol == mosq_p_mqtt5
									? (uint8_t)MQTT_RC_BAD_USERNAME_OR_PASSWORD
									: (uint8_t)CONNACK_REFUSED_BAD_USERNAME_PASSWORD;
	أرجع send__connack_error_and_return(context, err_کوڈ, rc);
}


ساكن صحيح read_protocol_name(بنية mosquitto *context, حرف protocol_name[7])
{
	/* Read protocol name as length then bytes rather than with read_string
	 * because the length is fixed and we can check that. Removes the need
	 * for another malloc as well. */

	uint16_t slen = 0;

	إذا(packet__read_uint16(&context->وچ_packet, &slen)){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string length.",
				context->پتہ, context->remote_پورٹ);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	إذا(slen != 4 /* MQTT */ && slen != 6 /* MQIsdp */){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with incorrect protocol string length (%d).",
				context->پتہ, context->remote_پورٹ, slen);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	إذا(packet_read_بائٹاں(&context->وچ_packet, protocol_name, slen)){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string.",
				context->پتہ, context->remote_پورٹ);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	protocol_name[slen] = '\0';

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح read_and_verify_protocol_نسخہ(بنية mosquitto *context, ثابت حرف *protocol_name,
		uint8_t *protocol_نسخہ)
{
	uint8_t tmp_protocol_نسخہ = 0;
	إذا(packet__read_byte(&context->وچ_packet, &tmp_protocol_نسخہ)){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol version.",
				context->پتہ, context->remote_پورٹ);
		أرجع MOSQ_ERR_PROTOCOL;
	}

	إذا(check_protocol_نسخہ(context->listener, tmp_protocol_نسخہ)){
		إذا(tmp_protocol_نسخہ == 3 || tmp_protocol_نسخہ == 4){
			context->protocol = mosq_p_mqtt311;
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_نسخہ, عدم);
		}وإلا{
			context->protocol = mosq_p_mqtt5;
			send__connack(context, 0, MQTT_RC_UNSUPPORTED_PROTOCOL_نسخہ, عدم);
		}
		أرجع MOSQ_ERR_NOT_SUPPORTED;
	}

	إذا(!strcmp(protocol_name, PROTOCOL_NAME_v31)){
		إذا((tmp_protocol_نسخہ&0x7F) != PROTOCOL_نسخہ_v31){
			إذا(db.config->connection_messages == true){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->پتہ, context->remote_پورٹ, tmp_protocol_نسخہ);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_نسخہ, عدم);
			أرجع MOSQ_ERR_PROTOCOL;
		}
		context->protocol = mosq_p_mqtt31;
		إذا((tmp_protocol_نسخہ&0x80) == 0x80){
			context->is_bridge = true;
		}
	}وإلا إذا(!strcmp(protocol_name, PROTOCOL_NAME)){
		إذا((tmp_protocol_نسخہ&0x7F) == PROTOCOL_نسخہ_v311){
			context->protocol = mosq_p_mqtt311;

			إذا((tmp_protocol_نسخہ&0x80) == 0x80){
				context->is_bridge = true;
			}
		}وإلا إذا((tmp_protocol_نسخہ&0x7F) == PROTOCOL_نسخہ_v5){
			context->protocol = mosq_p_mqtt5;
		}وإلا{
			إذا(db.config->connection_messages == true){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->پتہ, context->remote_پورٹ, tmp_protocol_نسخہ);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_نسخہ, عدم);
			أرجع MOSQ_ERR_PROTOCOL;
		}
		إذا((context->وچ_packet.command&0x0F) != 0x00){
			/* Reserved flags not set to 0, must disconnect. */
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero reserved flags (%02X).",
					context->پتہ, context->remote_پورٹ, context->وچ_packet.command);
			أرجع MOSQ_ERR_PROTOCOL;
		}
	}وإلا{
		إذا(db.config->connection_messages == true){
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol \"%s\".",
					context->پتہ, context->remote_پورٹ, protocol_name);
		}
		أرجع MOSQ_ERR_PROTOCOL;
	}
	إذا((tmp_protocol_نسخہ&0x7F) != PROTOCOL_نسخہ_v31 && context->وچ_packet.command != CMD_CONNECT){
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}

	*protocol_نسخہ = tmp_protocol_نسخہ;
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح read_and_verify_connect_flags(بنية mosquitto *context, uint8_t *connect_flags)
{
	إذا(packet__read_byte(&context->وچ_packet, connect_flags)){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing connect flags.",
				context->پتہ, context->remote_پورٹ);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	إذا(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		إذا((*connect_flags & 0x01) != 0x00){
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero connect reserved flag (%02X).",
					context->پتہ, context->remote_پورٹ, *connect_flags);
			أرجع MOSQ_ERR_PROTOCOL;
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن فراغ سیٹ_session_expiry_interval(بنية mosquitto *context, uint8_t clean_شروع, uint8_t protocol_نسخہ)
{
	/* session_expiry_interval will be overridden if the properties are read later */
	إذا(clean_شروع == false && protocol_نسخہ != PROTOCOL_نسخہ_v5){
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

	إذا(packet__read_uint16(&context->وچ_packet, &(context->keepalive))){
		log__printf(عدم, MOSQ_LOG_INFO, "%s sent CONNECT with missing keepalive.",
				context->id);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	keepalive_جوڑو(context);

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح read_and_verify_v5_connect_properties(بنية mosquitto *context, mosquitto_property **properties, uint8_t protocol_نسخہ)
{
	صحيح rc;

	إذا(protocol_نسخہ == PROTOCOL_نسخہ_v5){
		rc = property__read_all(CMD_CONNECT, &context->وچ_packet, properties);
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


ساكن صحيح verify_will_options(بنية mosquitto *context, uint8_t will, uint8_t will_qos, uint8_t will_retain, uint8_t protocol_نسخہ)
{
	إذا(will_qos == 3){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid Will QoS (%d).",
				context->پتہ, context->remote_پورٹ, will_qos);
		أرجع MOSQ_ERR_PROTOCOL;
	}

	إذا(will && will_retain && db.config->retain_دستیاب == false){
		إذا(protocol_نسخہ == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_RETAIN_NOT_SUPPORTED, عدم);
		}
		أرجع MOSQ_ERR_NOT_SUPPORTED;
	}

	إذا(will && will_qos > context->listener->max_qos){
		إذا(protocol_نسخہ == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_QOS_NOT_SUPPORTED, عدم);
		}
		أرجع MOSQ_ERR_NOT_SUPPORTED;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح handle_zero_لمائی_clientid(بنية mosquitto *context, حرف **clientid, bool *allow_zero_لمائی_clientid,
		uint8_t clean_شروع)
{
	إذا(context->protocol == mosq_p_mqtt31){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: v3.1 CONNECT with zero length clientid.",
				context->پتہ, context->remote_پورٹ);
		send__connack(context, 0, CONNACK_REFUSED_IDENTIFIER_REJECTED, عدم);
		أرجع MOSQ_ERR_PROTOCOL;
	}

	/* mqtt311/mqtt5 */
	mosquitto_FREE(*clientid);

	إذا(db.config->فی_listener_settings){
		*allow_zero_لمائی_clientid = context->listener->security_options->allow_zero_لمائی_clientid;
	}وإلا{
		*allow_zero_لمائی_clientid = db.config->security_options.allow_zero_لمائی_clientid;
	}

	إذا((context->protocol == mosq_p_mqtt311 && clean_شروع == 0) || *allow_zero_لمائی_clientid == false){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with zero length clientid when forbidden.",
				context->پتہ, context->remote_پورٹ);
		uint8_t err_کوڈ = context->protocol == mosq_p_mqtt311 ? (uint8_t)CONNACK_REFUSED_IDENTIFIER_REJECTED : (uint8_t)MQTT_RC_UNSPECIFIED;
		أرجع send__connack_error_and_return(context, err_کوڈ, MOSQ_ERR_PROTOCOL);
	}

	*clientid = clientid_gen(&(uint16_t){0}, context->listener->security_options->auto_id_اگیتر,
			context->listener->security_options->auto_id_اگیتر_len);
	إذا(*clientid == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}
	context->assigned_id = true;

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح check_clientid_prefixes(بنية mosquitto *context, ثابت حرف *clientid)
{
	إذا(db.config->clientid_prefixes){
		إذا(لڑی_مقابلہ(db.config->clientid_prefixes, clientid, لڑی_لمبائی(db.config->clientid_prefixes))){
			uint8_t err_کوڈ = context->protocol == mosq_p_mqtt5 ? (uint8_t)MQTT_RC_NOT_AUTHORIZED : (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
			أرجع send__connack_error_and_return(context, err_کوڈ, MOSQ_ERR_AUTH);
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح read_and_verify_clientid_from_packet(بنية mosquitto *context, حرف **clientid,
		bool *allow_zero_لمائی_clientid, uint8_t clean_شروع)
{
	صحيح rc;
	uint16_t slen;

	rc = packet__read_string(&context->وچ_packet, clientid, &slen);
	إذا(rc == MOSQ_ERR_MALFORMED_UTF8){
		إذا(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_CLIENTID_NOT_VALID, عدم);
		}
		أرجع MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID;
	}وإلا إذا(rc > 0){
		أرجع MOSQ_ERR_PROTOCOL;
	}

	إذا(slen == 0){
		rc = handle_zero_لمائی_clientid(context, clientid, allow_zero_لمائی_clientid, clean_شروع);
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


ساكن صحيح سیٹ_username_from_packet(بنية mosquitto *context, حرف **username, ثابت حرف *clientid)
{
	صحيح rc;

	rc = packet__read_string(&context->وچ_packet, username, &(uint16_t){0});
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


ساكن صحيح سیٹ_password_from_packet(بنية mosquitto *context, حرف **password, uint16_t *password_len, ثابت حرف *clientid)
{
	صحيح rc;

	rc = packet_read_ثنائی(&context->وچ_packet, (uint8_t **)password, password_len);
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
		rc = سیٹ_username_from_packet(context, username, clientid);
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
		rc = سیٹ_password_from_packet(context, password, password_len, clientid);
		إذا(rc != MOSQ_ERR_SUCCESS){
			أرجع rc;
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح check_additional_trailing_انکڑے(بنية mosquitto *context, uint8_t protocol_نسخہ)
{
	إذا(context->وچ_packet.pos != context->وچ_packet.بچیا_لمائی){
		/* Surplus data at end of packet, this must be an error. */
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->وچ_packet.pos, context->وچ_packet.بچیا_لمائی);
		إذا(protocol_نسخہ == PROTOCOL_نسخہ_v5){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, عدم);
		}
		أرجع MOSQ_ERR_PROTOCOL;
	}

	أرجع MOSQ_ERR_SUCCESS;
}

#جے_تعریف_شدہ WITH_TLS


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


ساكن صحيح سیٹ_username_from_cert_پچھان_تت(بنية mosquitto *context)
{
	X509 *client_cert = عدم;
	X509_NAME *name = عدم;

	إذا(get_client_cert_and_subject_name(context, &client_cert, &name)){
		أرجع MOSQ_ERR_AUTH;
	}

	صحيح i = -1;
	X509_NAME_اندراج *name_اندراج = عدم;

	i = X509_NAME_get_اشاریہ_by_NID(name, NID_commonName, -1);
	إذا(i == -1){
		أرجع free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	name_اندراج = X509_NAME_get_اندراج(name, i);
	إذا(!name_اندراج){
		اذهب success;
	}

	ASN1_STRING *name_asn1 = عدم;
	name_asn1 = X509_NAME_اندراج_get_انکڑے(name_اندراج);
	إذا(name_asn1 == عدم){
		أرجع free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	ثابت حرف *cert_پچھان_تت = (ثابت حرف *)ASN1_STRING_get0_انکڑے(name_asn1);
	إذا(!cert_پچھان_تت || mosquitto_validate_utf8(cert_پچھان_تت, (صحيح)لڑی_لمبائی(cert_پچھان_تت))){
		أرجع free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	mosquitto_free(context->username);
	context->username = mosquitto_strdup(cert_پچھان_تت);
	إذا(context->username == عدم){
		أرجع free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_NOMEM);
	}
	/* Make sure there isn't an embedded NUL character in the CN */
	إذا((size_t)ASN1_STRING_لمائی(name_asn1) != لڑی_لمبائی(context->username)){
		أرجع free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}

success:
	X509_free(client_cert);
	client_cert = عدم;
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح سیٹ_username_from_cert_subject_name(بنية mosquitto *context)
{
	X509 *client_cert = عدم;
	X509_NAME *name = عدم;

	إذا(get_client_cert_and_subject_name(context, &client_cert, &name)){
		أرجع MOSQ_ERR_AUTH;
	}

	حرف *subject = عدم;
	حرف *انکڑے_شروع = عدم;
	BIO *subject_bio = عدم;
	لما name_لمائی = 0;

	subject_bio = BIO_new(BIO_s_mem());
	X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
	انکڑے_شروع = عدم;
	name_لمائی = BIO_get_mem_انکڑے(subject_bio, &انکڑے_شروع);
	subject = mosquitto_malloc(حجم(حرف)*(size_t)(name_لمائی+1));
	إذا(!subject){
		أرجع free_x509_and_BIO_and_send_connack_error(context, client_cert, subject_bio, MOSQ_ERR_NOMEM);
	}
	حافظہ_نقل(subject, انکڑے_شروع, (size_t)name_لمائی);
	subject[name_لمائی] = '\0';
	BIO_free(subject_bio);

	إذا(mosquitto_validate_utf8(subject, (صحيح)لڑی_لمبائی(subject))){
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
#جے_انت


ساكن صحيح handle_username_from_cert_options(بنية mosquitto *context, حرف **username, حرف **password, uint16_t password_len)
{
	صحيح rc;

#جے_تعریف_شدہ WITH_TLS
	إذا(context->listener->ssl_ctx && (context->listener->use_پچھان_تت_as_username || context->listener->use_subject_as_username)){
		/* Don't need the username or password if provided */
		mosquitto_FREE(*username);
		mosquitto_FREE(*password);

		إذا(!context->ssl){
			أرجع send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
		}
#جے_تعریف_شدہ FINAL_WITH_TLS_PSK
		إذا(context->listener->psk_hint){
			/* Client should have provided an identity to get this far. */
			إذا(!context->username){
				أرجع send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}وإلا
#جے_انت /* FINAL_WITH_TLS_PSK */
		{
			إذا(context->listener->use_پچھان_تت_as_username){
				rc = سیٹ_username_from_cert_پچھان_تت(context);
			}وإلا{   /* use_subject_as_username */
				rc = سیٹ_username_from_cert_subject_name(context);
			}
			إذا(rc){
				أرجع rc;
			}
		}
	}وإلا
#جے_انت /* WITH_TLS */
	{
#جے_تعریف_شدہ WITH_TLS
		إذا(context->listener->use_پچھان_تت_as_username && context->listener->require_certificate){
			mosquitto_FREE(*username);
			mosquitto_FREE(*password);

			إذا(!context->username){
				أرجع send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}وإلا
#جے_انت
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
		uint8_t err_کوڈ = context->protocol == mosq_p_mqtt5
												? (uint8_t)MQTT_RC_NOT_AUTHORIZED
												: (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
		أرجع send__connack_error_and_return(context, err_کوڈ, MOSQ_ERR_AUTH);
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح handle__connect(بنية mosquitto *context)
{
	حرف protocol_name[7];
	uint8_t protocol_نسخہ;
	uint8_t connect_flags;
	حرف *clientid = عدم;
	بنية mosquitto *لبھیا_context;
	بنية mosquitto_سنیہا_all *will_struct = عدم;
	uint8_t will, will_retain, will_qos, clean_شروع;
	uint8_t username_flag, password_flag;
	حرف *username = عدم, *password = عدم;
	uint16_t password_len = 0;
	صحيح rc;
	mosquitto_property *properties = عدم;
	فراغ *auth_انکڑے = عدم;
	uint16_t auth_انکڑے_len = 0;
	فراغ *auth_انکڑے_باہرلا = عدم;
	uint16_t auth_انکڑے_باہرلا_len = 0;
	bool allow_zero_لمائی_clientid;

	إذا(!context->listener){
		أرجع MOSQ_ERR_INVAL;
	}

	/* Don't accept multiple CONNECT commands. */
	إذا(context->state != mosq_cs_new){
		log__printf(عدم, MOSQ_LOG_NOTICE, "Bad client %s:%d sending multiple CONNECT messages.",
				context->پتہ, context->remote_پورٹ);
		rc = MOSQ_ERR_PROTOCOL;
		اذهب handle_connect_error;
	}

#جے_تعریف_شدہ WITH_TLS
	إذا(context->وچ_packet.command == 0x16 && context->listener->ssl_ctx == عدم){ /* 0x16 is TLS handshake client hello */
		log__printf(عدم, MOSQ_LOG_NOTICE, "Client from %s:%d appears to be using TLS to connect to a non-TLS listener.",
				context->پتہ, context->remote_پورٹ);
		rc = MOSQ_ERR_PROTOCOL;
		اذهب handle_connect_error;
	}
#جے_انت

	rc = read_protocol_name(context, protocol_name);
	إذا(rc != MOSQ_ERR_SUCCESS){
		اذهب handle_connect_error;
	}

	rc = read_and_verify_protocol_نسخہ(context, protocol_name, &protocol_نسخہ);
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
	سیٹ_session_expiry_interval(context, clean_شروع, protocol_نسخہ);

	rc = read_and_reset_keepalive(context);
	إذا(rc != MOSQ_ERR_SUCCESS){
		اذهب handle_connect_error;
	}

	rc = read_and_verify_v5_connect_properties(context, &properties, protocol_نسخہ);
	إذا(rc != MOSQ_ERR_SUCCESS){
		اذهب handle_connect_error;
	}

	will = connect_flags & 0x04;
	will_qos = (connect_flags & 0x18) >> 3;
	will_retain = ((connect_flags & 0x20) == 0x20);
	rc = verify_will_options(context, will, will_qos, will_retain, protocol_نسخہ);
	إذا(rc != MOSQ_ERR_SUCCESS){
		اذهب handle_connect_error;
	}

	mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_طریقہ, &context->auth_طریقہ, false);
	mosquitto_property_read_ثنائی(properties, MQTT_PROP_AUTHENTICATION_انکڑے, &auth_انکڑے, &auth_انکڑے_len, false);
	mosquitto_property_free_all(&properties);

	إذا(auth_انکڑے && !context->auth_طریقہ){
		send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, عدم);
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d CONNECT with missing clientid string.",
				context->پتہ, context->remote_پورٹ);
		rc = MOSQ_ERR_PROTOCOL;
		اذهب handle_connect_error;
	}

	rc = read_and_verify_clientid_from_packet(context, &clientid, &allow_zero_لمائی_clientid, clean_شروع);
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

	rc = check_additional_trailing_انکڑے(context, protocol_نسخہ);
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
	HASH_لبھو(hh_id, db.contexts_by_id_delayed_auth, context->id, لڑی_لمبائی(context->id), لبھیا_context);
	إذا(لبھیا_context){
		rc = MOSQ_ERR_UNKNOWN;
		اذهب handle_connect_error;
	}

	context->clean_شروع = clean_شروع;
	context->will = will_struct;
	will_struct = عدم;

	إذا(context->auth_طریقہ){
		rc = mosquitto_security_auth_شروع(context, false, auth_انکڑے, auth_انکڑے_len, &auth_انکڑے_باہرلا, &auth_انکڑے_باہرلا_len);
		mosquitto_FREE(auth_انکڑے);
		إذا(rc == MOSQ_ERR_SUCCESS){
			أرجع connect__on_authorised(context, auth_انکڑے_باہرلا, auth_انکڑے_باہرلا_len);
		}وإلا إذا(rc == MOSQ_ERR_AUTH_CONTINUE){
			mosquitto_سیٹ_state(context, mosq_cs_authenticating);
			rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_انکڑے_باہرلا, auth_انکڑے_باہرلا_len);
			محفوظ_FREE(auth_انکڑے_باہرلا);
			أرجع rc;
		}وإلا{
			محفوظ_FREE(auth_انکڑے_باہرلا);
			will__clear(context);
			إذا(rc == MOSQ_ERR_AUTH){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, عدم);
				mosquitto_FREE(context->id);
				اذهب handle_connect_error;
			}وإلا إذا(rc == MOSQ_ERR_NOT_SUPPORTED){
				/* Client has requested extended authentication, but we don't support it. */
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_طریقہ, عدم);
				mosquitto_FREE(context->id);
				اذهب handle_connect_error;
			}وإلا{
				mosquitto_FREE(context->id);
				اذهب handle_connect_error;
			}
		}
	}وإلا{
#جے_تعریف_شدہ WITH_TLS
		إذا(context->listener->ssl_ctx && (context->listener->use_پچھان_تت_as_username || context->listener->use_subject_as_username)){
			/* Authentication assumed to be cleared */
		}وإلا
#جے_انت
		{
			rc = mosquitto_basic_auth(context);
			بدل(rc){
				حالة MOSQ_ERR_SUCCESS:
					اكسر;
				حالة MOSQ_ERR_AUTH_DELAYED:
					mosquitto_سیٹ_state(context, mosq_cs_delayed_auth);
					HASH_جوڑو_KEYPTR(hh_id, db.contexts_by_id_delayed_auth, context->id, لڑی_لمبائی(context->id), context);
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
				حالة MOSQ_ERR_BAD_AUTHENTICATION_طریقہ:
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
	mosquitto_FREE(auth_انکڑے);
	mosquitto_FREE(clientid);
	mosquitto_FREE(username);
	mosquitto_FREE(password);
	إذا(will_struct){
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct->پیغام.payload);
		mosquitto_FREE(will_struct->پیغام.topic);
		mosquitto_FREE(will_struct);
	}
	will__clear(context);
	/* We return an error here which means the client is freed later on. */
	context->clean_شروع = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_فوری;
	context->will_delay_interval = 0;
	أرجع rc;
}
