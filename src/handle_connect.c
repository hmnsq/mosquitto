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
#endif


ایستا نویسه nibble_to_hex(uint8_t مقدار)
{
	اگر(مقدار < 0x0A){
		بازگشت (نویسه)('0'+مقدار);
	}وگرنه{
		بازگشت (نویسه)(65 /*'A'*/ +مقدار-10);
	}
}


ایستا نویسه *clientid_gen(uint16_t *idlen, ثابت نویسه *auto_id_prefix, uint16_t auto_id_prefix_len)
{
	نویسه *clientid;
	uint8_t rnd[16];
	صحیح pos;

	اگر(mosquitto_getrandom(rnd, 16)){
		بازگشت پوچ;
	}

	*idlen = (uint16_t)(auto_id_prefix_len + 36);

	clientid = (نویسه *)mosquitto_calloc((size_t)(*idlen) + 1, اندازه(نویسه));
	اگر(!clientid){
		بازگشت پوچ;
	}
	اگر(auto_id_prefix){
		memcpy(clientid, auto_id_prefix, auto_id_prefix_len);
	}

	pos = 0;
	برای(صحیح i=0; i<16; i++){
		clientid[auto_id_prefix_len + pos + 0] = nibble_to_hex(rnd[i] & 0x0F);
		clientid[auto_id_prefix_len + pos + 1] = nibble_to_hex((rnd[i] >> 4) & 0x0F);
		pos += 2;
		اگر(pos == 8 || pos == 13 || pos == 18 || pos == 23){
			clientid[auto_id_prefix_len + pos] = '-';
			pos++;
		}
	}

	بازگشت clientid;
}


صحیح connect__on_authorised(ساختار mosquitto *context, تهی *auth_data_out, uint16_t auth_data_out_len)
{
	ساختار mosquitto *found_context;
	ساختار mosquitto__subleaf *leaf;
	mosquitto_property *connack_props = پوچ;
	uint8_t connect_ack = 0;
	صحیح rc;
	صحیح in_quota, out_quota;
	uint16_t in_maximum, out_maximum;

	/* Find if this client already has an entry. This must be done *after* any security checks. */
	HASH_FIND(hh_id, db.contexts_by_id, context->id, strlen(context->id), found_context);
	اگر(found_context){
		/* Found a matching client */
		اگر(!net__is_connected(found_context)){
			/* Client is reconnecting after a disconnect */
			/* FIXME - does anything need to be done here? */
		}وگرنه{
			/* Client is already connected, disconnect old version. This is
			 * done in context__cleanup() below. */
		}

		اگر(context->clean_start == true){
			sub__clean_session(found_context);
			found_context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			plugin_persist__handle_client_delete(found_context);
		}
		context->is_persisted = found_context->is_persisted;
		found_context->is_persisted = false; /* stops persistence for context being removed */

		اگر(context->clean_start == false && found_context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
			اگر(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
				connect_ack |= 0x01;
			}

			اگر(found_context->msgs_in.inflight || found_context->msgs_in.queued
					|| found_context->msgs_out.inflight || found_context->msgs_out.queued){

				in_quota = context->msgs_in.inflight_quota;
				out_quota = context->msgs_out.inflight_quota;
				in_maximum = context->msgs_in.inflight_maximum;
				out_maximum = context->msgs_out.inflight_maximum;

				memcpy(&context->msgs_in, &found_context->msgs_in, اندازه(ساختار mosquitto_پیام_data));
				memcpy(&context->msgs_out, &found_context->msgs_out, اندازه(ساختار mosquitto_پیام_data));
				context->last_cmsg_id = found_context->last_cmsg_id;

				memset(&found_context->msgs_in, 0, اندازه(ساختار mosquitto_پیام_data));
				memset(&found_context->msgs_out, 0, اندازه(ساختار mosquitto_پیام_data));

				context->msgs_in.inflight_quota = in_quota;
				context->msgs_out.inflight_quota = out_quota;
				context->msgs_in.inflight_maximum = in_maximum;
				context->msgs_out.inflight_maximum = out_maximum;

				db__message_reconnect_reset(context);
			}
			context->subs = found_context->subs;
			found_context->subs = پوچ;
			context->subs_capacity = found_context->subs_capacity;
			context->subs_شمار = found_context->subs_شمار;
			found_context->subs_capacity = 0;
			found_context->subs_شمار = 0;
			context->last_mid = found_context->last_mid;

			برای(صحیح i=0; i<context->subs_capacity; i++){
				اگر(context->subs[i]){
					leaf = context->subs[i]->hier->subs;
					وقتی(leaf){
						اگر(leaf->context == found_context){
							leaf->context = context;
						}
						leaf = leaf->next;
					}

					اگر(context->subs[i]->shared){
						leaf = context->subs[i]->shared->subs;
						وقتی(leaf){
							اگر(leaf->context == found_context){
								leaf->context = context;
							}
							leaf = leaf->next;
						}
					}
				}
			}
		}

		اگر((found_context->protocol == mosq_p_mqtt5 && found_context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE)
				|| (found_context->protocol != mosq_p_mqtt5 && found_context->clean_start == true)
				|| (context->clean_start == true)
				){

			context__send_will(found_context);
		}

		session_expiry__remove(found_context);
		will_delay__remove(found_context);
		will__clear(found_context);

		found_context->clean_start = true;
		found_context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
		mosquitto__set_state(found_context, mosq_cs_duplicate);
		اگر(found_context->protocol == mosq_p_mqtt5){
			send__disconnect(found_context, MQTT_RC_SESSION_TAKEN_OVER, پوچ);
		}
		do_disconnect(found_context, MOSQ_ERR_SESSION_TAKEN_OVER);
	}

	اگر(db.config->global_max_clients > 0 && HASH_CNT(hh_id, db.contexts_by_id) >= (unsigned صحیح)db.config->global_max_clients){
		اگر(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_SERVER_BUSY, پوچ);
		}
		rc = MOSQ_ERR_INVAL;
		برو error;
	}

	اگر(db.config->connection_messages == true){
		اگر(context->is_bridge){
			اگر(context->username){
				log__printf(پوچ, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive, context->username);
			}وگرنه{
				log__printf(پوچ, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d).",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive);
			}
		}وگرنه{
			اگر(context->username){
				log__printf(پوچ, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive, context->username);
			}وگرنه{
				log__printf(پوچ, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d).",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive);
			}
		}

		اگر(context->will){
			log__printf(پوچ, MOSQ_LOG_DEBUG, "Will message specified (%ld bytes) (r%d, q%d).",
					(long)context->will->پیام.payloadlen,
					context->will->پیام.retain,
					context->will->پیام.qos);

			log__printf(پوچ, MOSQ_LOG_DEBUG, "\t%s", context->will->پیام.topic);
		}وگرنه{
			log__printf(پوچ, MOSQ_LOG_DEBUG, "No will message specified.");
		}
	}
#ifdef WITH_TLS
	اگر(context->ssl){
		log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s negotiated %s cipher %s",
				context->id,
				SSL_get_cipher_version(context->ssl),
				SSL_get_cipher_name(context->ssl));
	}
#endif

	context->ping_t = 0;
	context->is_dropping = false;

	/* Remove any queued messages that are no longer allowed through ACL,
	 * assuming a possible change of username. */
	db__check_acl_of_all_messages(context);
	context__add_to_by_id(context);

#ifdef WITH_PERSISTENCE
	اگر(!context->clean_start){
		db.persistence_changes++;
	}
#endif
	context->max_qos = context->listener->max_qos;

	اگر(db.config->max_keepalive &&
			(context->keepalive > db.config->max_keepalive || context->keepalive == 0)){

		keepalive__remove(context);
		context->keepalive = db.config->max_keepalive;
		keepalive__add(context);
		اگر(context->protocol == mosq_p_mqtt5){
			اگر(mosquitto_property_add_int16(&connack_props, MQTT_PROP_SERVER_KEEP_ALIVE, context->keepalive)){
				rc = MOSQ_ERR_NOMEM;
				برو error;
			}
		}وگرنه{
			send__connack(context, connect_ack, CONNACK_REFUSED_IDENTIFIER_REJECTED, پوچ);
			rc = MOSQ_ERR_INVAL;
			برو error;
		}
	}


	اگر(context->protocol == mosq_p_mqtt5){
		اگر(context->listener->max_topic_alias > 0){
			اگر(mosquitto_property_add_int16(&connack_props, MQTT_PROP_TOPIC_ALIAS_MAXIMUM, context->listener->max_topic_alias)){
				rc = MOSQ_ERR_NOMEM;
				برو error;
			}
		}
		اگر(context->assigned_id){
			اگر(mosquitto_property_add_string(&connack_props, MQTT_PROP_ASSIGNED_CLIENT_IDENTIFIER, context->id)){
				rc = MOSQ_ERR_NOMEM;
				برو error;
			}
		}
		اگر(context->auth_method){
			اگر(mosquitto_property_add_string(&connack_props, MQTT_PROP_AUTHENTICATION_METHOD, context->auth_method)){
				rc = MOSQ_ERR_NOMEM;
				برو error;
			}

			اگر(auth_data_out && auth_data_out_len > 0){
				اگر(mosquitto_property_add_دودویی(&connack_props, MQTT_PROP_AUTHENTICATION_DATA, auth_data_out, auth_data_out_len)){
					rc = MOSQ_ERR_NOMEM;
					برو error;
				}
			}
		}
	}
	SAFE_FREE(auth_data_out);

	mosquitto__set_state(context, mosq_cs_active);
	rc = send__connack(context, connect_ack, CONNACK_ACCEPTED, connack_props);
	mosquitto_property_free_all(&connack_props);
	اگر(rc){
		بازگشت rc;
	}
	db__expire_all_messages(context);
	rc = db__message_write_queued_out(context);
	اگر(rc){
		بازگشت rc;
	}
	rc = db__message_write_inflight_out_all(context);

	اگر(rc == MOSQ_ERR_SUCCESS){
		plugin__handle_connect(context);

		اگر(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
			plugin_persist__handle_client_add(context);
		}وگرنه اگر(context->will){
			plugin_persist__handle_will_add(context);
		}
	}
	بازگشت rc;
error:
	SAFE_FREE(auth_data_out);
	mosquitto_property_free_all(&connack_props);
	بازگشت rc;
}


ایستا صحیح will__read(ساختار mosquitto *context, ثابت نویسه *clientid, ساختار mosquitto_message_all **will, uint8_t will_qos, صحیح will_retain)
{
	صحیح rc = MOSQ_ERR_SUCCESS;
	size_t slen;
	uint16_t tlen;
	ساختار mosquitto_message_all *will_struct = پوچ;
	نویسه *will_topic_mount = پوچ;
	uint16_t payloadlen;
	mosquitto_property *properties = پوچ;

	will_struct = mosquitto_calloc(1, اندازه(ساختار mosquitto_message_all));
	اگر(!will_struct){
		rc = MOSQ_ERR_NOMEM;
		برو error_cleanup;
	}
	اگر(context->protocol == PROTOCOL_VERSION_v5){
		rc = property__read_all(CMD_WILL, &context->in_packet, &properties);
		اگر(rc){
			برو error_cleanup;
		}

		rc = property__process_will(context, will_struct, &properties);
		mosquitto_property_free_all(&properties);
		اگر(rc){
			برو error_cleanup;
		}
	}
	rc = packet__read_string(&context->in_packet, &will_struct->پیام.topic, &tlen);
	اگر(rc){
		برو error_cleanup;
	}
	اگر(!tlen){
		log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s: Will with empty topic.",
				context->id);
		rc = MOSQ_ERR_PROTOCOL;
		برو error_cleanup;
	}

	اگر(context->listener->mount_point){
		slen = strlen(context->listener->mount_point) + strlen(will_struct->پیام.topic) + 1;
		will_topic_mount = mosquitto_malloc(slen+1);
		اگر(!will_topic_mount){
			rc = MOSQ_ERR_NOMEM;
			برو error_cleanup;
		}

		snprintf(will_topic_mount, slen, "%s%s", context->listener->mount_point, will_struct->پیام.topic);
		will_topic_mount[slen] = '\0';

		mosquitto_FREE(will_struct->پیام.topic);
		will_struct->پیام.topic = will_topic_mount;
	}

	اگر(!strncmp(will_struct->پیام.topic, "$CONTROL/", strlen("$CONTROL/"))){
		rc = MOSQ_ERR_ACL_DENIED;
		برو error_cleanup;
	}
	rc = mosquitto_pub_topic_check(will_struct->پیام.topic);
	اگر(rc){
		برو error_cleanup;
	}

	rc = packet__read_uint16(&context->in_packet, &payloadlen);
	اگر(rc){
		برو error_cleanup;
	}

	will_struct->پیام.payloadlen = payloadlen;
	اگر(will_struct->پیام.payloadlen > 0){
		اگر(db.config->message_size_limit && will_struct->پیام.payloadlen > (صحیح)db.config->message_size_limit){
			log__printf(پوچ, MOSQ_LOG_DEBUG, "Client %s connected with too large Will payload", clientid);
			اگر(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_PACKET_TOO_LARGE, پوچ);
			}وگرنه{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, پوچ);
			}
			rc = MOSQ_ERR_PAYLOAD_SIZE;
			برو error_cleanup;
		}
		will_struct->پیام.payload = mosquitto_malloc((size_t)will_struct->پیام.payloadlen);
		اگر(!will_struct->پیام.payload){
			rc = MOSQ_ERR_NOMEM;
			برو error_cleanup;
		}

		rc = packet__read_bytes(&context->in_packet, will_struct->پیام.payload, (uint32_t)will_struct->پیام.payloadlen);
		اگر(rc){
			برو error_cleanup;
		}
	}

	will_struct->پیام.qos = will_qos;
	will_struct->پیام.retain = will_retain;

	*will = will_struct;
	بازگشت MOSQ_ERR_SUCCESS;

error_cleanup:
	اگر(will_struct){
		mosquitto_FREE(will_struct->پیام.topic);
		mosquitto_FREE(will_struct->پیام.payload);
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct);
	}
	بازگشت rc;
}


ایستا صحیح check_protocol_version(ساختار mosquitto__listener *listener, صحیح protocol_version)
{
	/* Allow bridge protocol as well. */
	protocol_version &= 0x7F;

	اگر((protocol_version == 3 && listener->disable_protocol_v3 == false)
			|| (protocol_version == 4 && listener->disable_protocol_v4 == false)
			|| (protocol_version == 5 && listener->disable_protocol_v5 == false)
			){

		بازگشت MOSQ_ERR_SUCCESS;
	}وگرنه{
		بازگشت MOSQ_ERR_NOT_SUPPORTED;
	}
}


درونخطی ایستا صحیح send__connack_error_and_return(ساختار mosquitto *context, uint8_t err_code, صحیح rc)
{
	send__connack(context, 0, err_code, پوچ);
	بازگشت rc;
}


درونخطی ایستا صحیح send__connack_bad_username_or_password_error(ساختار mosquitto *context, صحیح rc)
{
	uint8_t err_code  = context->protocol == mosq_p_mqtt5
									? (uint8_t)MQTT_RC_BAD_USERNAME_OR_PASSWORD
									: (uint8_t)CONNACK_REFUSED_BAD_USERNAME_PASSWORD;
	بازگشت send__connack_error_and_return(context, err_code, rc);
}


ایستا صحیح read_protocol_name(ساختار mosquitto *context, نویسه protocol_name[7])
{
	/* Read protocol name as length then bytes rather than with read_string
	 * because the length is fixed and we can check that. Removes the need
	 * for another malloc as well. */

	uint16_t slen = 0;

	اگر(packet__read_uint16(&context->in_packet, &slen)){
		log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string length.",
				context->address, context->remote_port);
		بازگشت MOSQ_ERR_PROTOCOL;
	}
	اگر(slen != 4 /* MQTT */ && slen != 6 /* MQIsdp */){
		log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with incorrect protocol string length (%d).",
				context->address, context->remote_port, slen);
		بازگشت MOSQ_ERR_PROTOCOL;
	}
	اگر(packet__read_bytes(&context->in_packet, protocol_name, slen)){
		log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string.",
				context->address, context->remote_port);
		بازگشت MOSQ_ERR_PROTOCOL;
	}
	protocol_name[slen] = '\0';

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح read_and_verify_protocol_version(ساختار mosquitto *context, ثابت نویسه *protocol_name,
		uint8_t *protocol_version)
{
	uint8_t tmp_protocol_version = 0;
	اگر(packet__read_byte(&context->in_packet, &tmp_protocol_version)){
		log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol version.",
				context->address, context->remote_port);
		بازگشت MOSQ_ERR_PROTOCOL;
	}

	اگر(check_protocol_version(context->listener, tmp_protocol_version)){
		اگر(tmp_protocol_version == 3 || tmp_protocol_version == 4){
			context->protocol = mosq_p_mqtt311;
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, پوچ);
		}وگرنه{
			context->protocol = mosq_p_mqtt5;
			send__connack(context, 0, MQTT_RC_UNSUPPORTED_PROTOCOL_VERSION, پوچ);
		}
		بازگشت MOSQ_ERR_NOT_SUPPORTED;
	}

	اگر(!strcmp(protocol_name, PROTOCOL_NAME_v31)){
		اگر((tmp_protocol_version&0x7F) != PROTOCOL_VERSION_v31){
			اگر(db.config->connection_messages == true){
				log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->address, context->remote_port, tmp_protocol_version);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, پوچ);
			بازگشت MOSQ_ERR_PROTOCOL;
		}
		context->protocol = mosq_p_mqtt31;
		اگر((tmp_protocol_version&0x80) == 0x80){
			context->is_bridge = true;
		}
	}وگرنه اگر(!strcmp(protocol_name, PROTOCOL_NAME)){
		اگر((tmp_protocol_version&0x7F) == PROTOCOL_VERSION_v311){
			context->protocol = mosq_p_mqtt311;

			اگر((tmp_protocol_version&0x80) == 0x80){
				context->is_bridge = true;
			}
		}وگرنه اگر((tmp_protocol_version&0x7F) == PROTOCOL_VERSION_v5){
			context->protocol = mosq_p_mqtt5;
		}وگرنه{
			اگر(db.config->connection_messages == true){
				log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->address, context->remote_port, tmp_protocol_version);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, پوچ);
			بازگشت MOSQ_ERR_PROTOCOL;
		}
		اگر((context->in_packet.command&0x0F) != 0x00){
			/* Reserved flags not set to 0, must disconnect. */
			log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero reserved flags (%02X).",
					context->address, context->remote_port, context->in_packet.command);
			بازگشت MOSQ_ERR_PROTOCOL;
		}
	}وگرنه{
		اگر(db.config->connection_messages == true){
			log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol \"%s\".",
					context->address, context->remote_port, protocol_name);
		}
		بازگشت MOSQ_ERR_PROTOCOL;
	}
	اگر((tmp_protocol_version&0x7F) != PROTOCOL_VERSION_v31 && context->in_packet.command != CMD_CONNECT){
		بازگشت MOSQ_ERR_MALFORMED_PACKET;
	}

	*protocol_version = tmp_protocol_version;
	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح read_and_verify_connect_flags(ساختار mosquitto *context, uint8_t *connect_flags)
{
	اگر(packet__read_byte(&context->in_packet, connect_flags)){
		log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing connect flags.",
				context->address, context->remote_port);
		بازگشت MOSQ_ERR_PROTOCOL;
	}
	اگر(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		اگر((*connect_flags & 0x01) != 0x00){
			log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero connect reserved flag (%02X).",
					context->address, context->remote_port, *connect_flags);
			بازگشت MOSQ_ERR_PROTOCOL;
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا تهی set_session_expiry_interval(ساختار mosquitto *context, uint8_t clean_start, uint8_t protocol_version)
{
	/* session_expiry_interval will be overridden if the properties are read later */
	اگر(clean_start == false && protocol_version != PROTOCOL_VERSION_v5){
		/* v3* has clean_start == false mean the session never expires */
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
	}وگرنه{
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	}
}


ایستا صحیح read_and_reset_keepalive(ساختار mosquitto *context)
{
	/* _remove here because net__socket_accept() uses _add and we must have the
	 * correct keepalive value */
	keepalive__remove(context);

	اگر(packet__read_uint16(&context->in_packet, &(context->keepalive))){
		log__printf(پوچ, MOSQ_LOG_INFO, "%s sent CONNECT with missing keepalive.",
				context->id);
		بازگشت MOSQ_ERR_PROTOCOL;
	}
	keepalive__add(context);

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح read_and_verify_v5_connect_properties(ساختار mosquitto *context, mosquitto_property **properties, uint8_t protocol_version)
{
	صحیح rc;

	اگر(protocol_version == PROTOCOL_VERSION_v5){
		rc = property__read_all(CMD_CONNECT, &context->in_packet, properties);
		اگر(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
			send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, پوچ);
		}وگرنه اگر(rc == MOSQ_ERR_MALFORMED_PACKET){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, پوچ);
		}
		اگر(rc){
			بازگشت rc;
		}
	}
	rc = property__process_connect(context, properties);
	اگر(rc != MOSQ_ERR_SUCCESS){
		بازگشت send__connack_error_and_return(context, MQTT_RC_PROTOCOL_ERROR, rc);
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح verify_will_options(ساختار mosquitto *context, uint8_t will, uint8_t will_qos, uint8_t will_retain, uint8_t protocol_version)
{
	اگر(will_qos == 3){
		log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid Will QoS (%d).",
				context->address, context->remote_port, will_qos);
		بازگشت MOSQ_ERR_PROTOCOL;
	}

	اگر(will && will_retain && db.config->retain_available == false){
		اگر(protocol_version == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_RETAIN_NOT_SUPPORTED, پوچ);
		}
		بازگشت MOSQ_ERR_NOT_SUPPORTED;
	}

	اگر(will && will_qos > context->listener->max_qos){
		اگر(protocol_version == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_QOS_NOT_SUPPORTED, پوچ);
		}
		بازگشت MOSQ_ERR_NOT_SUPPORTED;
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح handle_zero_length_clientid(ساختار mosquitto *context, نویسه **clientid, bool *allow_zero_length_clientid,
		uint8_t clean_start)
{
	اگر(context->protocol == mosq_p_mqtt31){
		log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s:%d: v3.1 CONNECT with zero length clientid.",
				context->address, context->remote_port);
		send__connack(context, 0, CONNACK_REFUSED_IDENTIFIER_REJECTED, پوچ);
		بازگشت MOSQ_ERR_PROTOCOL;
	}

	/* mqtt311/mqtt5 */
	mosquitto_FREE(*clientid);

	اگر(db.config->per_listener_settings){
		*allow_zero_length_clientid = context->listener->security_options->allow_zero_length_clientid;
	}وگرنه{
		*allow_zero_length_clientid = db.config->security_options.allow_zero_length_clientid;
	}

	اگر((context->protocol == mosq_p_mqtt311 && clean_start == 0) || *allow_zero_length_clientid == false){
		log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with zero length clientid when forbidden.",
				context->address, context->remote_port);
		uint8_t err_code = context->protocol == mosq_p_mqtt311 ? (uint8_t)CONNACK_REFUSED_IDENTIFIER_REJECTED : (uint8_t)MQTT_RC_UNSPECIFIED;
		بازگشت send__connack_error_and_return(context, err_code, MOSQ_ERR_PROTOCOL);
	}

	*clientid = clientid_gen(&(uint16_t){0}, context->listener->security_options->auto_id_prefix,
			context->listener->security_options->auto_id_prefix_len);
	اگر(*clientid == پوچ){
		بازگشت MOSQ_ERR_NOMEM;
	}
	context->assigned_id = true;

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح check_clientid_prefixes(ساختار mosquitto *context, ثابت نویسه *clientid)
{
	اگر(db.config->clientid_prefixes){
		اگر(strncmp(db.config->clientid_prefixes, clientid, strlen(db.config->clientid_prefixes))){
			uint8_t err_code = context->protocol == mosq_p_mqtt5 ? (uint8_t)MQTT_RC_NOT_AUTHORIZED : (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
			بازگشت send__connack_error_and_return(context, err_code, MOSQ_ERR_AUTH);
		}
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح read_and_verify_clientid_from_packet(ساختار mosquitto *context, نویسه **clientid,
		bool *allow_zero_length_clientid, uint8_t clean_start)
{
	صحیح rc;
	uint16_t slen;

	rc = packet__read_string(&context->in_packet, clientid, &slen);
	اگر(rc == MOSQ_ERR_MALFORMED_UTF8){
		اگر(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_CLIENTID_NOT_VALID, پوچ);
		}
		بازگشت MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID;
	}وگرنه اگر(rc > 0){
		بازگشت MOSQ_ERR_PROTOCOL;
	}

	اگر(slen == 0){
		rc = handle_zero_length_clientid(context, clientid, allow_zero_length_clientid, clean_start);
		اگر(rc != MOSQ_ERR_SUCCESS){
			بازگشت rc;
		}
	}

	rc = check_clientid_prefixes(context, *clientid);
	اگر(rc != MOSQ_ERR_SUCCESS){
		بازگشت rc;
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح set_username_from_packet(ساختار mosquitto *context, نویسه **username, ثابت نویسه *clientid)
{
	صحیح rc;

	rc = packet__read_string(&context->in_packet, username, &(uint16_t){0});
	اگر(rc == MOSQ_ERR_NOMEM){
		بازگشت MOSQ_ERR_NOMEM;
	}
	اگر(rc != MOSQ_ERR_SUCCESS){
		اگر(context->protocol == mosq_p_mqtt31){
			/* Username flag given, but no username. Ignore. */
			/* NOTE: Removed setting of username_flag to zero as it is unused afterwards */
		}وگرنه{
			log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with username flag but no username.",
					clientid);
			بازگشت MOSQ_ERR_PROTOCOL;
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح set_password_from_packet(ساختار mosquitto *context, نویسه **password, uint16_t *password_len, ثابت نویسه *clientid)
{
	صحیح rc;

	rc = packet_read_دودویی(&context->in_packet, (uint8_t **)password, password_len);
	اگر(rc == MOSQ_ERR_NOMEM){
		بازگشت MOSQ_ERR_NOMEM;
	}

	اگر(rc == MOSQ_ERR_MALFORMED_PACKET){
		اگر(context->protocol == mosq_p_mqtt31){
			/* Password flag given, but no password. Ignore. */
		}وگرنه{
			log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with password flag but no password.",
					clientid);
			بازگشت MOSQ_ERR_PROTOCOL;
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح read_and_verify_client_credentials_from_packet(ساختار mosquitto *context,
		نویسه **username, uint8_t username_flag,
		نویسه **password, uint16_t *password_len, uint8_t password_flag,
		ثابت نویسه *clientid)
{
	صحیح rc;

	اگر(username_flag){
		rc = set_username_from_packet(context, username, clientid);
		اگر(rc != MOSQ_ERR_SUCCESS){
			بازگشت rc;
		}
	}وگرنه{
		اگر(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
			اگر(password_flag){
				/* username_flag == 0 && password_flag == 1 is forbidden */
				log__printf(پوچ, MOSQ_LOG_ERR, "Protocol error from %s: password without username, closing connection.", clientid);
				بازگشت MOSQ_ERR_PROTOCOL;
			}
		}
	}
	اگر(password_flag){
		rc = set_password_from_packet(context, password, password_len, clientid);
		اگر(rc != MOSQ_ERR_SUCCESS){
			بازگشت rc;
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح check_additional_trailing_data(ساختار mosquitto *context, uint8_t protocol_version)
{
	اگر(context->in_packet.pos != context->in_packet.remaining_length){
		/* Surplus data at end of packet, this must be an error. */
		log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->in_packet.pos, context->in_packet.remaining_length);
		اگر(protocol_version == PROTOCOL_VERSION_v5){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, پوچ);
		}
		بازگشت MOSQ_ERR_PROTOCOL;
	}

	بازگشت MOSQ_ERR_SUCCESS;
}

#ifdef WITH_TLS


درونخطی ایستا صحیح get_client_cert_and_subject_name(ساختار mosquitto *context, X509 **client_cert, X509_NAME **name)
{
	*client_cert = SSL_get_peer_certificate(context->ssl);
	اگر(*client_cert == پوچ){
		بازگشت send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	*name = X509_get_subject_name(*client_cert);
	اگر(*name == پوچ){
		X509_free(*client_cert);
		بازگشت send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


درونخطی ایستا صحیح free_x509_and_send_connack_error(ساختار mosquitto *context, X509 *client_cert, صحیح rc)
{
	X509_free(client_cert);
	بازگشت send__connack_bad_username_or_password_error(context, rc);
}


درونخطی ایستا صحیح free_x509_and_BIO_and_send_connack_error(ساختار mosquitto *context, X509 *client_cert,
		BIO *subject_name, صحیح rc)
{
	BIO_free(subject_name);
	بازگشت free_x509_and_send_connack_error(context, client_cert, rc);
}


ایستا صحیح set_username_from_cert_identity(ساختار mosquitto *context)
{
	X509 *client_cert = پوچ;
	X509_NAME *name = پوچ;

	اگر(get_client_cert_and_subject_name(context, &client_cert, &name)){
		بازگشت MOSQ_ERR_AUTH;
	}

	صحیح i = -1;
	X509_NAME_ENTRY *name_entry = پوچ;

	i = X509_NAME_get_index_by_NID(name, NID_commonName, -1);
	اگر(i == -1){
		بازگشت free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	name_entry = X509_NAME_get_entry(name, i);
	اگر(!name_entry){
		برو success;
	}

	ASN1_STRING *name_asn1 = پوچ;
	name_asn1 = X509_NAME_ENTRY_get_data(name_entry);
	اگر(name_asn1 == پوچ){
		بازگشت free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	ثابت نویسه *cert_identity = (ثابت نویسه *)ASN1_STRING_get0_data(name_asn1);
	اگر(!cert_identity || mosquitto_validate_utf8(cert_identity, (صحیح)strlen(cert_identity))){
		بازگشت free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	mosquitto_free(context->username);
	context->username = mosquitto_strdup(cert_identity);
	اگر(context->username == پوچ){
		بازگشت free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_NOMEM);
	}
	/* Make sure there isn't an embedded NUL character in the CN */
	اگر((size_t)ASN1_STRING_length(name_asn1) != strlen(context->username)){
		بازگشت free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}

success:
	X509_free(client_cert);
	client_cert = پوچ;
	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح set_username_from_cert_subject_name(ساختار mosquitto *context)
{
	X509 *client_cert = پوچ;
	X509_NAME *name = پوچ;

	اگر(get_client_cert_and_subject_name(context, &client_cert, &name)){
		بازگشت MOSQ_ERR_AUTH;
	}

	نویسه *subject = پوچ;
	نویسه *data_start = پوچ;
	BIO *subject_bio = پوچ;
	long name_length = 0;

	subject_bio = BIO_new(BIO_s_mem());
	X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
	data_start = پوچ;
	name_length = BIO_get_mem_data(subject_bio, &data_start);
	subject = mosquitto_malloc(اندازه(نویسه)*(size_t)(name_length+1));
	اگر(!subject){
		بازگشت free_x509_and_BIO_and_send_connack_error(context, client_cert, subject_bio, MOSQ_ERR_NOMEM);
	}
	memcpy(subject, data_start, (size_t)name_length);
	subject[name_length] = '\0';
	BIO_free(subject_bio);

	اگر(mosquitto_validate_utf8(subject, (صحیح)strlen(subject))){
		mosquitto_free(subject);
		بازگشت free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	context->username = subject;
	اگر(!context->username){
		X509_free(client_cert);
		بازگشت MOSQ_ERR_AUTH;
	}

	X509_free(client_cert);
	client_cert = پوچ;
	بازگشت MOSQ_ERR_SUCCESS;
}
#endif


ایستا صحیح handle_username_from_cert_options(ساختار mosquitto *context, نویسه **username, نویسه **password, uint16_t password_len)
{
	صحیح rc;

#ifdef WITH_TLS
	اگر(context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
		/* Don't need the username or password if provided */
		mosquitto_FREE(*username);
		mosquitto_FREE(*password);

		اگر(!context->ssl){
			بازگشت send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
		}
#ifdef FINAL_WITH_TLS_PSK
		اگر(context->listener->psk_hint){
			/* Client should have provided an identity to get this far. */
			اگر(!context->username){
				بازگشت send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}وگرنه
#endif /* FINAL_WITH_TLS_PSK */
		{
			اگر(context->listener->use_identity_as_username){
				rc = set_username_from_cert_identity(context);
			}وگرنه{   /* use_subject_as_username */
				rc = set_username_from_cert_subject_name(context);
			}
			اگر(rc){
				بازگشت rc;
			}
		}
	}وگرنه
#endif /* WITH_TLS */
	{
#ifdef WITH_TLS
		اگر(context->listener->use_identity_as_username && context->listener->require_certificate){
			mosquitto_FREE(*username);
			mosquitto_FREE(*password);

			اگر(!context->username){
				بازگشت send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}وگرنه
#endif
		{
			/* FIXME - these ensure the mosquitto_clientid() and
			* mosquitto_client_username() functions work, but is hacky */
			context->username = *username;
			context->password = *password;
			context->password_len = password_len;
			*username = پوچ; /* Avoid free() in error: below. */
			*password = پوچ;
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح handle_username_as_clientid_option(ساختار mosquitto *context)
{
	اگر(context->username){
		mosquitto_FREE(context->id);
		context->id = mosquitto_strdup(context->username);
		اگر(!context->id){
			بازگشت MOSQ_ERR_NOMEM;
		}
	}وگرنه{
		uint8_t err_code = context->protocol == mosq_p_mqtt5
												? (uint8_t)MQTT_RC_NOT_AUTHORIZED
												: (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
		بازگشت send__connack_error_and_return(context, err_code, MOSQ_ERR_AUTH);
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح handle__connect(ساختار mosquitto *context)
{
	نویسه protocol_name[7];
	uint8_t protocol_version;
	uint8_t connect_flags;
	نویسه *clientid = پوچ;
	ساختار mosquitto *found_context;
	ساختار mosquitto_message_all *will_struct = پوچ;
	uint8_t will, will_retain, will_qos, clean_start;
	uint8_t username_flag, password_flag;
	نویسه *username = پوچ, *password = پوچ;
	uint16_t password_len = 0;
	صحیح rc;
	mosquitto_property *properties = پوچ;
	تهی *auth_data = پوچ;
	uint16_t auth_data_len = 0;
	تهی *auth_data_out = پوچ;
	uint16_t auth_data_out_len = 0;
	bool allow_zero_length_clientid;

	اگر(!context->listener){
		بازگشت MOSQ_ERR_INVAL;
	}

	/* Don't accept multiple CONNECT commands. */
	اگر(context->state != mosq_cs_new){
		log__printf(پوچ, MOSQ_LOG_NOTICE, "Bad client %s:%d sending multiple CONNECT messages.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		برو handle_connect_error;
	}

#ifdef WITH_TLS
	اگر(context->in_packet.command == 0x16 && context->listener->ssl_ctx == پوچ){ /* 0x16 is TLS handshake client hello */
		log__printf(پوچ, MOSQ_LOG_NOTICE, "Client from %s:%d appears to be using TLS to connect to a non-TLS listener.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		برو handle_connect_error;
	}
#endif

	rc = read_protocol_name(context, protocol_name);
	اگر(rc != MOSQ_ERR_SUCCESS){
		برو handle_connect_error;
	}

	rc = read_and_verify_protocol_version(context, protocol_name, &protocol_version);
	اگر(rc != MOSQ_ERR_SUCCESS){
		اگر(rc == MOSQ_ERR_MALFORMED_PACKET){
			بازگشت rc;
		}
		برو handle_connect_error;
	}

	rc = read_and_verify_connect_flags(context, &connect_flags);
	اگر(rc != MOSQ_ERR_SUCCESS){
		برو handle_connect_error;
	}

	clean_start = (connect_flags & 0x02) >> 1;
	set_session_expiry_interval(context, clean_start, protocol_version);

	rc = read_and_reset_keepalive(context);
	اگر(rc != MOSQ_ERR_SUCCESS){
		برو handle_connect_error;
	}

	rc = read_and_verify_v5_connect_properties(context, &properties, protocol_version);
	اگر(rc != MOSQ_ERR_SUCCESS){
		برو handle_connect_error;
	}

	will = connect_flags & 0x04;
	will_qos = (connect_flags & 0x18) >> 3;
	will_retain = ((connect_flags & 0x20) == 0x20);
	rc = verify_will_options(context, will, will_qos, will_retain, protocol_version);
	اگر(rc != MOSQ_ERR_SUCCESS){
		برو handle_connect_error;
	}

	mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_METHOD, &context->auth_method, false);
	mosquitto_property_read_دودویی(properties, MQTT_PROP_AUTHENTICATION_DATA, &auth_data, &auth_data_len, false);
	mosquitto_property_free_all(&properties);

	اگر(auth_data && !context->auth_method){
		send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, پوچ);
		log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s:%d CONNECT with missing clientid string.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		برو handle_connect_error;
	}

	rc = read_and_verify_clientid_from_packet(context, &clientid, &allow_zero_length_clientid, clean_start);
	اگر(rc != MOSQ_ERR_SUCCESS){
		برو handle_connect_error;
	}

	اگر(will){
		rc = will__read(context, clientid, &will_struct, will_qos, will_retain);
		اگر(rc){
			اگر(context->protocol == mosq_p_mqtt5){
				اگر(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
					send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, پوچ);
				}وگرنه اگر(rc == MOSQ_ERR_MALFORMED_PACKET){
					send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, پوچ);
				}
			}
			برو handle_connect_error;
		}
	}وگرنه{
		اگر(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
			اگر(will_qos != 0 || will_retain != 0){
				log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT without Will with non-zero QoS (%d) or retain (%d).",
						clientid, will_qos, will_retain);
				rc = MOSQ_ERR_PROTOCOL;
				برو handle_connect_error;
			}
		}
	}

	// Client credentials
	password_flag = connect_flags & 0x40;
	username_flag = connect_flags & 0x80;
	rc = read_and_verify_client_credentials_from_packet(context, &username, username_flag, &password, &password_len, password_flag, clientid);
	اگر(rc != MOSQ_ERR_SUCCESS){
		برو handle_connect_error;
	}

	rc = check_additional_trailing_data(context, protocol_version);
	اگر(rc != MOSQ_ERR_SUCCESS){
		برو handle_connect_error;
	}

	/* Once context->id is set, if we return from this function with an error
	 * we must make sure that context->id is freed and set to NULL, so that the
	 * client isn't erroneously removed from the by_id hash table. */
	context->id = clientid;
	clientid = پوچ;

	/* use_identity_as_username or use_subject_as_username */
	rc = handle_username_from_cert_options(context, &username, &password, password_len);
	اگر(rc != MOSQ_ERR_SUCCESS){
		برو handle_connect_error;
	}

	/* use_username_as_clientid */
	اگر(context->listener->use_username_as_clientid){
		rc = handle_username_as_clientid_option(context);
		اگر(rc != MOSQ_ERR_SUCCESS){
			برو handle_connect_error;
		}
	}

	/* Check for an existing delayed auth check, reject if present */
	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), found_context);
	اگر(found_context){
		rc = MOSQ_ERR_UNKNOWN;
		برو handle_connect_error;
	}

	context->clean_start = clean_start;
	context->will = will_struct;
	will_struct = پوچ;

	اگر(context->auth_method){
		rc = mosquitto_security_auth_start(context, false, auth_data, auth_data_len, &auth_data_out, &auth_data_out_len);
		mosquitto_FREE(auth_data);
		اگر(rc == MOSQ_ERR_SUCCESS){
			بازگشت connect__on_authorised(context, auth_data_out, auth_data_out_len);
		}وگرنه اگر(rc == MOSQ_ERR_AUTH_CONTINUE){
			mosquitto__set_state(context, mosq_cs_authenticating);
			rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_data_out, auth_data_out_len);
			SAFE_FREE(auth_data_out);
			بازگشت rc;
		}وگرنه{
			SAFE_FREE(auth_data_out);
			will__clear(context);
			اگر(rc == MOSQ_ERR_AUTH){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, پوچ);
				mosquitto_FREE(context->id);
				برو handle_connect_error;
			}وگرنه اگر(rc == MOSQ_ERR_NOT_SUPPORTED){
				/* Client has requested extended authentication, but we don't support it. */
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_METHOD, پوچ);
				mosquitto_FREE(context->id);
				برو handle_connect_error;
			}وگرنه{
				mosquitto_FREE(context->id);
				برو handle_connect_error;
			}
		}
	}وگرنه{
#ifdef WITH_TLS
		اگر(context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
			/* Authentication assumed to be cleared */
		}وگرنه
#endif
		{
			rc = mosquitto_basic_auth(context);
			انتخاب(rc){
				حالت MOSQ_ERR_SUCCESS:
					بشکن;
				حالت MOSQ_ERR_AUTH_DELAYED:
					mosquitto__set_state(context, mosq_cs_delayed_auth);
					HASH_ADD_KEYPTR(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context);
					بازگشت MOSQ_ERR_SUCCESS;
					بشکن;
				حالت MOSQ_ERR_AUTH:
					اگر(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, پوچ);
					}وگرنه{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, پوچ);
					}
					برو handle_connect_error;
					بشکن;
				حالت MOSQ_ERR_UNSPECIFIED:
				حالت MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
				حالت MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
				حالت MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
				حالت MOSQ_ERR_SERVER_UNAVAILABLE:
				حالت MOSQ_ERR_SERVER_BUSY:
				حالت MOSQ_ERR_BANNED:
				حالت MOSQ_ERR_BAD_AUTHENTICATION_METHOD:
				حالت MOSQ_ERR_CONNECTION_RATE_EXCEEDED:
					اگر(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, (uint8_t)rc, پوچ);
					}وگرنه{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, پوچ);
					}
					برو handle_connect_error;
					بشکن;
				پیشفرض:
					rc = MOSQ_ERR_UNKNOWN;
					برو handle_connect_error;
					بشکن;
			}
		}
		بازگشت connect__on_authorised(context, پوچ, 0);
	}


handle_connect_error:
	mosquitto_property_free_all(&properties);
	mosquitto_FREE(auth_data);
	mosquitto_FREE(clientid);
	mosquitto_FREE(username);
	mosquitto_FREE(password);
	اگر(will_struct){
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct->پیام.payload);
		mosquitto_FREE(will_struct->پیام.topic);
		mosquitto_FREE(will_struct);
	}
	will__clear(context);
	/* We return an error here which means the client is freed later on. */
	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->will_delay_interval = 0;
	بازگشت rc;
}
