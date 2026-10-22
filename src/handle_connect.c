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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <utlist.h>

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto/mqtt_protocol.h"
#அந்தர்பாவ "packet_mosq.h"
#அந்தர்பாவ "property_mosq.h"
#அந்தர்பாவ "send_mosq.h"
#அந்தர்பாவ "sys_tree.h"
#அந்தர்பாவ "tls_mosq.h"
#அந்தர்பாவ "util_mosq.h"
#அந்தர்பாவ "will_mosq.h"

#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif


நிலையான எழுத்து nibble_to_hex(uint8_t மதிப்பு)
{
	எனில்(மதிப்பு < 0x0A){
		திரும்பு (எழுத்து)('0'+மதிப்பு);
	}இல்லையெனில்{
		திரும்பு (எழுத்து)(65 /*'A'*/ +மதிப்பு-10);
	}
}


நிலையான எழுத்து *clientid_gen(uint16_t *idlen, மாறிலி எழுத்து *auto_id_prefix, uint16_t auto_id_prefix_len)
{
	எழுத்து *clientid;
	uint8_t rnd[16];
	முழு pos;

	எனில்(mosquitto_getrandom(rnd, 16)){
		திரும்பு வெற்று;
	}

	*idlen = (uint16_t)(auto_id_prefix_len + 36);

	clientid = (எழுத்து *)mosquitto_calloc((size_t)(*idlen) + 1, அளவு(எழுத்து));
	எனில்(!clientid){
		திரும்பு வெற்று;
	}
	எனில்(auto_id_prefix){
		memcpy(clientid, auto_id_prefix, auto_id_prefix_len);
	}

	pos = 0;
	க்கு(முழு i=0; i<16; i++){
		clientid[auto_id_prefix_len + pos + 0] = nibble_to_hex(rnd[i] & 0x0F);
		clientid[auto_id_prefix_len + pos + 1] = nibble_to_hex((rnd[i] >> 4) & 0x0F);
		pos += 2;
		எனில்(pos == 8 || pos == 13 || pos == 18 || pos == 23){
			clientid[auto_id_prefix_len + pos] = '-';
			pos++;
		}
	}

	திரும்பு clientid;
}


முழு connect__on_authorised(கட்டமைப்பு mosquitto *context, வெறுமை *auth_data_out, uint16_t auth_data_out_len)
{
	கட்டமைப்பு mosquitto *found_context;
	கட்டமைப்பு mosquitto__subleaf *leaf;
	mosquitto_property *connack_props = வெற்று;
	uint8_t connect_ack = 0;
	முழு rc;
	முழு in_quota, out_quota;
	uint16_t in_maximum, out_maximum;

	/* Find if this client already has an entry. This must be done *after* any security checks. */
	HASH_FIND(hh_id, db.contexts_by_id, context->id, strlen(context->id), found_context);
	எனில்(found_context){
		/* Found a matching client */
		எனில்(!net__is_connected(found_context)){
			/* Client is reconnecting after a disconnect */
			/* FIXME - does anything need to be done here? */
		}இல்லையெனில்{
			/* Client is already connected, disconnect old version. This is
			 * done in context__cleanup() below. */
		}

		எனில்(context->clean_start == true){
			sub__clean_session(found_context);
			found_context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			plugin_persist__handle_client_delete(found_context);
		}
		context->is_persisted = found_context->is_persisted;
		found_context->is_persisted = false; /* stops persistence for context being removed */

		எனில்(context->clean_start == false && found_context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
			எனில்(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
				connect_ack |= 0x01;
			}

			எனில்(found_context->msgs_in.inflight || found_context->msgs_in.queued
					|| found_context->msgs_out.inflight || found_context->msgs_out.queued){

				in_quota = context->msgs_in.inflight_quota;
				out_quota = context->msgs_out.inflight_quota;
				in_maximum = context->msgs_in.inflight_maximum;
				out_maximum = context->msgs_out.inflight_maximum;

				memcpy(&context->msgs_in, &found_context->msgs_in, அளவு(கட்டமைப்பு mosquitto_செய்தி_data));
				memcpy(&context->msgs_out, &found_context->msgs_out, அளவு(கட்டமைப்பு mosquitto_செய்தி_data));
				context->last_cmsg_id = found_context->last_cmsg_id;

				memset(&found_context->msgs_in, 0, அளவு(கட்டமைப்பு mosquitto_செய்தி_data));
				memset(&found_context->msgs_out, 0, அளவு(கட்டமைப்பு mosquitto_செய்தி_data));

				context->msgs_in.inflight_quota = in_quota;
				context->msgs_out.inflight_quota = out_quota;
				context->msgs_in.inflight_maximum = in_maximum;
				context->msgs_out.inflight_maximum = out_maximum;

				db__message_reconnect_reset(context);
			}
			context->subs = found_context->subs;
			found_context->subs = வெற்று;
			context->subs_capacity = found_context->subs_capacity;
			context->subs_மொத்தம் = found_context->subs_மொத்தம்;
			found_context->subs_capacity = 0;
			found_context->subs_மொத்தம் = 0;
			context->last_mid = found_context->last_mid;

			க்கு(முழு i=0; i<context->subs_capacity; i++){
				எனில்(context->subs[i]){
					leaf = context->subs[i]->hier->subs;
					போது(leaf){
						எனில்(leaf->context == found_context){
							leaf->context = context;
						}
						leaf = leaf->next;
					}

					எனில்(context->subs[i]->shared){
						leaf = context->subs[i]->shared->subs;
						போது(leaf){
							எனில்(leaf->context == found_context){
								leaf->context = context;
							}
							leaf = leaf->next;
						}
					}
				}
			}
		}

		எனில்((found_context->protocol == mosq_p_mqtt5 && found_context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE)
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
		எனில்(found_context->protocol == mosq_p_mqtt5){
			send__disconnect(found_context, MQTT_RC_SESSION_TAKEN_OVER, வெற்று);
		}
		do_disconnect(found_context, MOSQ_ERR_SESSION_TAKEN_OVER);
	}

	எனில்(db.config->global_max_clients > 0 && HASH_CNT(hh_id, db.contexts_by_id) >= (unsigned முழு)db.config->global_max_clients){
		எனில்(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_SERVER_BUSY, வெற்று);
		}
		rc = MOSQ_ERR_INVAL;
		செல் error;
	}

	எனில்(db.config->connection_messages == true){
		எனில்(context->is_bridge){
			எனில்(context->username){
				log__printf(வெற்று, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive, context->username);
			}இல்லையெனில்{
				log__printf(வெற்று, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d).",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive);
			}
		}இல்லையெனில்{
			எனில்(context->username){
				log__printf(வெற்று, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive, context->username);
			}இல்லையெனில்{
				log__printf(வெற்று, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d).",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive);
			}
		}

		எனில்(context->will){
			log__printf(வெற்று, MOSQ_LOG_DEBUG, "Will message specified (%ld bytes) (r%d, q%d).",
					(long)context->will->செய்தி.payloadlen,
					context->will->செய்தி.retain,
					context->will->செய்தி.qos);

			log__printf(வெற்று, MOSQ_LOG_DEBUG, "\t%s", context->will->செய்தி.topic);
		}இல்லையெனில்{
			log__printf(வெற்று, MOSQ_LOG_DEBUG, "No will message specified.");
		}
	}
#ifdef WITH_TLS
	எனில்(context->ssl){
		log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s negotiated %s cipher %s",
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
	எனில்(!context->clean_start){
		db.persistence_changes++;
	}
#endif
	context->max_qos = context->listener->max_qos;

	எனில்(db.config->max_keepalive &&
			(context->keepalive > db.config->max_keepalive || context->keepalive == 0)){

		keepalive__remove(context);
		context->keepalive = db.config->max_keepalive;
		keepalive__add(context);
		எனில்(context->protocol == mosq_p_mqtt5){
			எனில்(mosquitto_property_add_int16(&connack_props, MQTT_PROP_SERVER_KEEP_ALIVE, context->keepalive)){
				rc = MOSQ_ERR_NOMEM;
				செல் error;
			}
		}இல்லையெனில்{
			send__connack(context, connect_ack, CONNACK_REFUSED_IDENTIFIER_REJECTED, வெற்று);
			rc = MOSQ_ERR_INVAL;
			செல் error;
		}
	}


	எனில்(context->protocol == mosq_p_mqtt5){
		எனில்(context->listener->max_topic_alias > 0){
			எனில்(mosquitto_property_add_int16(&connack_props, MQTT_PROP_TOPIC_ALIAS_MAXIMUM, context->listener->max_topic_alias)){
				rc = MOSQ_ERR_NOMEM;
				செல் error;
			}
		}
		எனில்(context->assigned_id){
			எனில்(mosquitto_property_add_string(&connack_props, MQTT_PROP_ASSIGNED_CLIENT_IDENTIFIER, context->id)){
				rc = MOSQ_ERR_NOMEM;
				செல் error;
			}
		}
		எனில்(context->auth_method){
			எனில்(mosquitto_property_add_string(&connack_props, MQTT_PROP_AUTHENTICATION_METHOD, context->auth_method)){
				rc = MOSQ_ERR_NOMEM;
				செல் error;
			}

			எனில்(auth_data_out && auth_data_out_len > 0){
				எனில்(mosquitto_property_add_இருமம்(&connack_props, MQTT_PROP_AUTHENTICATION_DATA, auth_data_out, auth_data_out_len)){
					rc = MOSQ_ERR_NOMEM;
					செல் error;
				}
			}
		}
	}
	SAFE_FREE(auth_data_out);

	mosquitto__set_state(context, mosq_cs_active);
	rc = send__connack(context, connect_ack, CONNACK_ACCEPTED, connack_props);
	mosquitto_property_free_all(&connack_props);
	எனில்(rc){
		திரும்பு rc;
	}
	db__expire_all_messages(context);
	rc = db__message_write_queued_out(context);
	எனில்(rc){
		திரும்பு rc;
	}
	rc = db__message_write_inflight_out_all(context);

	எனில்(rc == MOSQ_ERR_SUCCESS){
		plugin__handle_connect(context);

		எனில்(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
			plugin_persist__handle_client_add(context);
		}இல்லையெனில் எனில்(context->will){
			plugin_persist__handle_will_add(context);
		}
	}
	திரும்பு rc;
error:
	SAFE_FREE(auth_data_out);
	mosquitto_property_free_all(&connack_props);
	திரும்பு rc;
}


நிலையான முழு will__read(கட்டமைப்பு mosquitto *context, மாறிலி எழுத்து *clientid, கட்டமைப்பு mosquitto_message_all **will, uint8_t will_qos, முழு will_retain)
{
	முழு rc = MOSQ_ERR_SUCCESS;
	size_t slen;
	uint16_t tlen;
	கட்டமைப்பு mosquitto_message_all *will_struct = வெற்று;
	எழுத்து *will_topic_mount = வெற்று;
	uint16_t payloadlen;
	mosquitto_property *properties = வெற்று;

	will_struct = mosquitto_calloc(1, அளவு(கட்டமைப்பு mosquitto_message_all));
	எனில்(!will_struct){
		rc = MOSQ_ERR_NOMEM;
		செல் error_cleanup;
	}
	எனில்(context->protocol == PROTOCOL_VERSION_v5){
		rc = property__read_all(CMD_WILL, &context->in_packet, &properties);
		எனில்(rc){
			செல் error_cleanup;
		}

		rc = property__process_will(context, will_struct, &properties);
		mosquitto_property_free_all(&properties);
		எனில்(rc){
			செல் error_cleanup;
		}
	}
	rc = packet__read_string(&context->in_packet, &will_struct->செய்தி.topic, &tlen);
	எனில்(rc){
		செல் error_cleanup;
	}
	எனில்(!tlen){
		log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s: Will with empty topic.",
				context->id);
		rc = MOSQ_ERR_PROTOCOL;
		செல் error_cleanup;
	}

	எனில்(context->listener->mount_point){
		slen = strlen(context->listener->mount_point) + strlen(will_struct->செய்தி.topic) + 1;
		will_topic_mount = mosquitto_malloc(slen+1);
		எனில்(!will_topic_mount){
			rc = MOSQ_ERR_NOMEM;
			செல் error_cleanup;
		}

		snprintf(will_topic_mount, slen, "%s%s", context->listener->mount_point, will_struct->செய்தி.topic);
		will_topic_mount[slen] = '\0';

		mosquitto_FREE(will_struct->செய்தி.topic);
		will_struct->செய்தி.topic = will_topic_mount;
	}

	எனில்(!strncmp(will_struct->செய்தி.topic, "$CONTROL/", strlen("$CONTROL/"))){
		rc = MOSQ_ERR_ACL_DENIED;
		செல் error_cleanup;
	}
	rc = mosquitto_pub_topic_check(will_struct->செய்தி.topic);
	எனில்(rc){
		செல் error_cleanup;
	}

	rc = packet__read_uint16(&context->in_packet, &payloadlen);
	எனில்(rc){
		செல் error_cleanup;
	}

	will_struct->செய்தி.payloadlen = payloadlen;
	எனில்(will_struct->செய்தி.payloadlen > 0){
		எனில்(db.config->message_size_limit && will_struct->செய்தி.payloadlen > (முழு)db.config->message_size_limit){
			log__printf(வெற்று, MOSQ_LOG_DEBUG, "Client %s connected with too large Will payload", clientid);
			எனில்(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_PACKET_TOO_LARGE, வெற்று);
			}இல்லையெனில்{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, வெற்று);
			}
			rc = MOSQ_ERR_PAYLOAD_SIZE;
			செல் error_cleanup;
		}
		will_struct->செய்தி.payload = mosquitto_malloc((size_t)will_struct->செய்தி.payloadlen);
		எனில்(!will_struct->செய்தி.payload){
			rc = MOSQ_ERR_NOMEM;
			செல் error_cleanup;
		}

		rc = packet__read_bytes(&context->in_packet, will_struct->செய்தி.payload, (uint32_t)will_struct->செய்தி.payloadlen);
		எனில்(rc){
			செல் error_cleanup;
		}
	}

	will_struct->செய்தி.qos = will_qos;
	will_struct->செய்தி.retain = will_retain;

	*will = will_struct;
	திரும்பு MOSQ_ERR_SUCCESS;

error_cleanup:
	எனில்(will_struct){
		mosquitto_FREE(will_struct->செய்தி.topic);
		mosquitto_FREE(will_struct->செய்தி.payload);
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct);
	}
	திரும்பு rc;
}


நிலையான முழு check_protocol_version(கட்டமைப்பு mosquitto__listener *listener, முழு protocol_version)
{
	/* Allow bridge protocol as well. */
	protocol_version &= 0x7F;

	எனில்((protocol_version == 3 && listener->disable_protocol_v3 == false)
			|| (protocol_version == 4 && listener->disable_protocol_v4 == false)
			|| (protocol_version == 5 && listener->disable_protocol_v5 == false)
			){

		திரும்பு MOSQ_ERR_SUCCESS;
	}இல்லையெனில்{
		திரும்பு MOSQ_ERR_NOT_SUPPORTED;
	}
}


உள்வரி நிலையான முழு send__connack_error_and_return(கட்டமைப்பு mosquitto *context, uint8_t err_code, முழு rc)
{
	send__connack(context, 0, err_code, வெற்று);
	திரும்பு rc;
}


உள்வரி நிலையான முழு send__connack_bad_username_or_password_error(கட்டமைப்பு mosquitto *context, முழு rc)
{
	uint8_t err_code  = context->protocol == mosq_p_mqtt5
									? (uint8_t)MQTT_RC_BAD_USERNAME_OR_PASSWORD
									: (uint8_t)CONNACK_REFUSED_BAD_USERNAME_PASSWORD;
	திரும்பு send__connack_error_and_return(context, err_code, rc);
}


நிலையான முழு read_protocol_name(கட்டமைப்பு mosquitto *context, எழுத்து protocol_name[7])
{
	/* Read protocol name as length then bytes rather than with read_string
	 * because the length is fixed and we can check that. Removes the need
	 * for another malloc as well. */

	uint16_t slen = 0;

	எனில்(packet__read_uint16(&context->in_packet, &slen)){
		log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string length.",
				context->address, context->remote_port);
		திரும்பு MOSQ_ERR_PROTOCOL;
	}
	எனில்(slen != 4 /* MQTT */ && slen != 6 /* MQIsdp */){
		log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with incorrect protocol string length (%d).",
				context->address, context->remote_port, slen);
		திரும்பு MOSQ_ERR_PROTOCOL;
	}
	எனில்(packet__read_bytes(&context->in_packet, protocol_name, slen)){
		log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string.",
				context->address, context->remote_port);
		திரும்பு MOSQ_ERR_PROTOCOL;
	}
	protocol_name[slen] = '\0';

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு read_and_verify_protocol_version(கட்டமைப்பு mosquitto *context, மாறிலி எழுத்து *protocol_name,
		uint8_t *protocol_version)
{
	uint8_t tmp_protocol_version = 0;
	எனில்(packet__read_byte(&context->in_packet, &tmp_protocol_version)){
		log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol version.",
				context->address, context->remote_port);
		திரும்பு MOSQ_ERR_PROTOCOL;
	}

	எனில்(check_protocol_version(context->listener, tmp_protocol_version)){
		எனில்(tmp_protocol_version == 3 || tmp_protocol_version == 4){
			context->protocol = mosq_p_mqtt311;
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, வெற்று);
		}இல்லையெனில்{
			context->protocol = mosq_p_mqtt5;
			send__connack(context, 0, MQTT_RC_UNSUPPORTED_PROTOCOL_VERSION, வெற்று);
		}
		திரும்பு MOSQ_ERR_NOT_SUPPORTED;
	}

	எனில்(!strcmp(protocol_name, PROTOCOL_NAME_v31)){
		எனில்((tmp_protocol_version&0x7F) != PROTOCOL_VERSION_v31){
			எனில்(db.config->connection_messages == true){
				log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->address, context->remote_port, tmp_protocol_version);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, வெற்று);
			திரும்பு MOSQ_ERR_PROTOCOL;
		}
		context->protocol = mosq_p_mqtt31;
		எனில்((tmp_protocol_version&0x80) == 0x80){
			context->is_bridge = true;
		}
	}இல்லையெனில் எனில்(!strcmp(protocol_name, PROTOCOL_NAME)){
		எனில்((tmp_protocol_version&0x7F) == PROTOCOL_VERSION_v311){
			context->protocol = mosq_p_mqtt311;

			எனில்((tmp_protocol_version&0x80) == 0x80){
				context->is_bridge = true;
			}
		}இல்லையெனில் எனில்((tmp_protocol_version&0x7F) == PROTOCOL_VERSION_v5){
			context->protocol = mosq_p_mqtt5;
		}இல்லையெனில்{
			எனில்(db.config->connection_messages == true){
				log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->address, context->remote_port, tmp_protocol_version);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, வெற்று);
			திரும்பு MOSQ_ERR_PROTOCOL;
		}
		எனில்((context->in_packet.command&0x0F) != 0x00){
			/* Reserved flags not set to 0, must disconnect. */
			log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero reserved flags (%02X).",
					context->address, context->remote_port, context->in_packet.command);
			திரும்பு MOSQ_ERR_PROTOCOL;
		}
	}இல்லையெனில்{
		எனில்(db.config->connection_messages == true){
			log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol \"%s\".",
					context->address, context->remote_port, protocol_name);
		}
		திரும்பு MOSQ_ERR_PROTOCOL;
	}
	எனில்((tmp_protocol_version&0x7F) != PROTOCOL_VERSION_v31 && context->in_packet.command != CMD_CONNECT){
		திரும்பு MOSQ_ERR_MALFORMED_PACKET;
	}

	*protocol_version = tmp_protocol_version;
	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு read_and_verify_connect_flags(கட்டமைப்பு mosquitto *context, uint8_t *connect_flags)
{
	எனில்(packet__read_byte(&context->in_packet, connect_flags)){
		log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing connect flags.",
				context->address, context->remote_port);
		திரும்பு MOSQ_ERR_PROTOCOL;
	}
	எனில்(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		எனில்((*connect_flags & 0x01) != 0x00){
			log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero connect reserved flag (%02X).",
					context->address, context->remote_port, *connect_flags);
			திரும்பு MOSQ_ERR_PROTOCOL;
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான வெறுமை set_session_expiry_interval(கட்டமைப்பு mosquitto *context, uint8_t clean_start, uint8_t protocol_version)
{
	/* session_expiry_interval will be overridden if the properties are read later */
	எனில்(clean_start == false && protocol_version != PROTOCOL_VERSION_v5){
		/* v3* has clean_start == false mean the session never expires */
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
	}இல்லையெனில்{
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	}
}


நிலையான முழு read_and_reset_keepalive(கட்டமைப்பு mosquitto *context)
{
	/* _remove here because net__socket_accept() uses _add and we must have the
	 * correct keepalive value */
	keepalive__remove(context);

	எனில்(packet__read_uint16(&context->in_packet, &(context->keepalive))){
		log__printf(வெற்று, MOSQ_LOG_INFO, "%s sent CONNECT with missing keepalive.",
				context->id);
		திரும்பு MOSQ_ERR_PROTOCOL;
	}
	keepalive__add(context);

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு read_and_verify_v5_connect_properties(கட்டமைப்பு mosquitto *context, mosquitto_property **properties, uint8_t protocol_version)
{
	முழு rc;

	எனில்(protocol_version == PROTOCOL_VERSION_v5){
		rc = property__read_all(CMD_CONNECT, &context->in_packet, properties);
		எனில்(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
			send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, வெற்று);
		}இல்லையெனில் எனில்(rc == MOSQ_ERR_MALFORMED_PACKET){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, வெற்று);
		}
		எனில்(rc){
			திரும்பு rc;
		}
	}
	rc = property__process_connect(context, properties);
	எனில்(rc != MOSQ_ERR_SUCCESS){
		திரும்பு send__connack_error_and_return(context, MQTT_RC_PROTOCOL_ERROR, rc);
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு verify_will_options(கட்டமைப்பு mosquitto *context, uint8_t will, uint8_t will_qos, uint8_t will_retain, uint8_t protocol_version)
{
	எனில்(will_qos == 3){
		log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid Will QoS (%d).",
				context->address, context->remote_port, will_qos);
		திரும்பு MOSQ_ERR_PROTOCOL;
	}

	எனில்(will && will_retain && db.config->retain_available == false){
		எனில்(protocol_version == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_RETAIN_NOT_SUPPORTED, வெற்று);
		}
		திரும்பு MOSQ_ERR_NOT_SUPPORTED;
	}

	எனில்(will && will_qos > context->listener->max_qos){
		எனில்(protocol_version == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_QOS_NOT_SUPPORTED, வெற்று);
		}
		திரும்பு MOSQ_ERR_NOT_SUPPORTED;
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு handle_zero_length_clientid(கட்டமைப்பு mosquitto *context, எழுத்து **clientid, bool *allow_zero_length_clientid,
		uint8_t clean_start)
{
	எனில்(context->protocol == mosq_p_mqtt31){
		log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s:%d: v3.1 CONNECT with zero length clientid.",
				context->address, context->remote_port);
		send__connack(context, 0, CONNACK_REFUSED_IDENTIFIER_REJECTED, வெற்று);
		திரும்பு MOSQ_ERR_PROTOCOL;
	}

	/* mqtt311/mqtt5 */
	mosquitto_FREE(*clientid);

	எனில்(db.config->per_listener_settings){
		*allow_zero_length_clientid = context->listener->security_options->allow_zero_length_clientid;
	}இல்லையெனில்{
		*allow_zero_length_clientid = db.config->security_options.allow_zero_length_clientid;
	}

	எனில்((context->protocol == mosq_p_mqtt311 && clean_start == 0) || *allow_zero_length_clientid == false){
		log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with zero length clientid when forbidden.",
				context->address, context->remote_port);
		uint8_t err_code = context->protocol == mosq_p_mqtt311 ? (uint8_t)CONNACK_REFUSED_IDENTIFIER_REJECTED : (uint8_t)MQTT_RC_UNSPECIFIED;
		திரும்பு send__connack_error_and_return(context, err_code, MOSQ_ERR_PROTOCOL);
	}

	*clientid = clientid_gen(&(uint16_t){0}, context->listener->security_options->auto_id_prefix,
			context->listener->security_options->auto_id_prefix_len);
	எனில்(*clientid == வெற்று){
		திரும்பு MOSQ_ERR_NOMEM;
	}
	context->assigned_id = true;

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு check_clientid_prefixes(கட்டமைப்பு mosquitto *context, மாறிலி எழுத்து *clientid)
{
	எனில்(db.config->clientid_prefixes){
		எனில்(strncmp(db.config->clientid_prefixes, clientid, strlen(db.config->clientid_prefixes))){
			uint8_t err_code = context->protocol == mosq_p_mqtt5 ? (uint8_t)MQTT_RC_NOT_AUTHORIZED : (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
			திரும்பு send__connack_error_and_return(context, err_code, MOSQ_ERR_AUTH);
		}
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு read_and_verify_clientid_from_packet(கட்டமைப்பு mosquitto *context, எழுத்து **clientid,
		bool *allow_zero_length_clientid, uint8_t clean_start)
{
	முழு rc;
	uint16_t slen;

	rc = packet__read_string(&context->in_packet, clientid, &slen);
	எனில்(rc == MOSQ_ERR_MALFORMED_UTF8){
		எனில்(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_CLIENTID_NOT_VALID, வெற்று);
		}
		திரும்பு MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID;
	}இல்லையெனில் எனில்(rc > 0){
		திரும்பு MOSQ_ERR_PROTOCOL;
	}

	எனில்(slen == 0){
		rc = handle_zero_length_clientid(context, clientid, allow_zero_length_clientid, clean_start);
		எனில்(rc != MOSQ_ERR_SUCCESS){
			திரும்பு rc;
		}
	}

	rc = check_clientid_prefixes(context, *clientid);
	எனில்(rc != MOSQ_ERR_SUCCESS){
		திரும்பு rc;
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு set_username_from_packet(கட்டமைப்பு mosquitto *context, எழுத்து **username, மாறிலி எழுத்து *clientid)
{
	முழு rc;

	rc = packet__read_string(&context->in_packet, username, &(uint16_t){0});
	எனில்(rc == MOSQ_ERR_NOMEM){
		திரும்பு MOSQ_ERR_NOMEM;
	}
	எனில்(rc != MOSQ_ERR_SUCCESS){
		எனில்(context->protocol == mosq_p_mqtt31){
			/* Username flag given, but no username. Ignore. */
			/* NOTE: Removed setting of username_flag to zero as it is unused afterwards */
		}இல்லையெனில்{
			log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with username flag but no username.",
					clientid);
			திரும்பு MOSQ_ERR_PROTOCOL;
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு set_password_from_packet(கட்டமைப்பு mosquitto *context, எழுத்து **password, uint16_t *password_len, மாறிலி எழுத்து *clientid)
{
	முழு rc;

	rc = packet_read_இருமம்(&context->in_packet, (uint8_t **)password, password_len);
	எனில்(rc == MOSQ_ERR_NOMEM){
		திரும்பு MOSQ_ERR_NOMEM;
	}

	எனில்(rc == MOSQ_ERR_MALFORMED_PACKET){
		எனில்(context->protocol == mosq_p_mqtt31){
			/* Password flag given, but no password. Ignore. */
		}இல்லையெனில்{
			log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with password flag but no password.",
					clientid);
			திரும்பு MOSQ_ERR_PROTOCOL;
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு read_and_verify_client_credentials_from_packet(கட்டமைப்பு mosquitto *context,
		எழுத்து **username, uint8_t username_flag,
		எழுத்து **password, uint16_t *password_len, uint8_t password_flag,
		மாறிலி எழுத்து *clientid)
{
	முழு rc;

	எனில்(username_flag){
		rc = set_username_from_packet(context, username, clientid);
		எனில்(rc != MOSQ_ERR_SUCCESS){
			திரும்பு rc;
		}
	}இல்லையெனில்{
		எனில்(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
			எனில்(password_flag){
				/* username_flag == 0 && password_flag == 1 is forbidden */
				log__printf(வெற்று, MOSQ_LOG_ERR, "Protocol error from %s: password without username, closing connection.", clientid);
				திரும்பு MOSQ_ERR_PROTOCOL;
			}
		}
	}
	எனில்(password_flag){
		rc = set_password_from_packet(context, password, password_len, clientid);
		எனில்(rc != MOSQ_ERR_SUCCESS){
			திரும்பு rc;
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு check_additional_trailing_data(கட்டமைப்பு mosquitto *context, uint8_t protocol_version)
{
	எனில்(context->in_packet.pos != context->in_packet.remaining_length){
		/* Surplus data at end of packet, this must be an error. */
		log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->in_packet.pos, context->in_packet.remaining_length);
		எனில்(protocol_version == PROTOCOL_VERSION_v5){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, வெற்று);
		}
		திரும்பு MOSQ_ERR_PROTOCOL;
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}

#ifdef WITH_TLS


உள்வரி நிலையான முழு get_client_cert_and_subject_name(கட்டமைப்பு mosquitto *context, X509 **client_cert, X509_NAME **name)
{
	*client_cert = SSL_get_peer_certificate(context->ssl);
	எனில்(*client_cert == வெற்று){
		திரும்பு send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	*name = X509_get_subject_name(*client_cert);
	எனில்(*name == வெற்று){
		X509_free(*client_cert);
		திரும்பு send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


உள்வரி நிலையான முழு free_x509_and_send_connack_error(கட்டமைப்பு mosquitto *context, X509 *client_cert, முழு rc)
{
	X509_free(client_cert);
	திரும்பு send__connack_bad_username_or_password_error(context, rc);
}


உள்வரி நிலையான முழு free_x509_and_BIO_and_send_connack_error(கட்டமைப்பு mosquitto *context, X509 *client_cert,
		BIO *subject_name, முழு rc)
{
	BIO_free(subject_name);
	திரும்பு free_x509_and_send_connack_error(context, client_cert, rc);
}


நிலையான முழு set_username_from_cert_identity(கட்டமைப்பு mosquitto *context)
{
	X509 *client_cert = வெற்று;
	X509_NAME *name = வெற்று;

	எனில்(get_client_cert_and_subject_name(context, &client_cert, &name)){
		திரும்பு MOSQ_ERR_AUTH;
	}

	முழு i = -1;
	X509_NAME_ENTRY *name_entry = வெற்று;

	i = X509_NAME_get_index_by_NID(name, NID_commonName, -1);
	எனில்(i == -1){
		திரும்பு free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	name_entry = X509_NAME_get_entry(name, i);
	எனில்(!name_entry){
		செல் success;
	}

	ASN1_STRING *name_asn1 = வெற்று;
	name_asn1 = X509_NAME_ENTRY_get_data(name_entry);
	எனில்(name_asn1 == வெற்று){
		திரும்பு free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	மாறிலி எழுத்து *cert_identity = (மாறிலி எழுத்து *)ASN1_STRING_get0_data(name_asn1);
	எனில்(!cert_identity || mosquitto_validate_utf8(cert_identity, (முழு)strlen(cert_identity))){
		திரும்பு free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	mosquitto_free(context->username);
	context->username = mosquitto_strdup(cert_identity);
	எனில்(context->username == வெற்று){
		திரும்பு free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_NOMEM);
	}
	/* Make sure there isn't an embedded NUL character in the CN */
	எனில்((size_t)ASN1_STRING_length(name_asn1) != strlen(context->username)){
		திரும்பு free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}

success:
	X509_free(client_cert);
	client_cert = வெற்று;
	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு set_username_from_cert_subject_name(கட்டமைப்பு mosquitto *context)
{
	X509 *client_cert = வெற்று;
	X509_NAME *name = வெற்று;

	எனில்(get_client_cert_and_subject_name(context, &client_cert, &name)){
		திரும்பு MOSQ_ERR_AUTH;
	}

	எழுத்து *subject = வெற்று;
	எழுத்து *data_start = வெற்று;
	BIO *subject_bio = வெற்று;
	long name_length = 0;

	subject_bio = BIO_new(BIO_s_mem());
	X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
	data_start = வெற்று;
	name_length = BIO_get_mem_data(subject_bio, &data_start);
	subject = mosquitto_malloc(அளவு(எழுத்து)*(size_t)(name_length+1));
	எனில்(!subject){
		திரும்பு free_x509_and_BIO_and_send_connack_error(context, client_cert, subject_bio, MOSQ_ERR_NOMEM);
	}
	memcpy(subject, data_start, (size_t)name_length);
	subject[name_length] = '\0';
	BIO_free(subject_bio);

	எனில்(mosquitto_validate_utf8(subject, (முழு)strlen(subject))){
		mosquitto_free(subject);
		திரும்பு free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	context->username = subject;
	எனில்(!context->username){
		X509_free(client_cert);
		திரும்பு MOSQ_ERR_AUTH;
	}

	X509_free(client_cert);
	client_cert = வெற்று;
	திரும்பு MOSQ_ERR_SUCCESS;
}
#endif


நிலையான முழு handle_username_from_cert_options(கட்டமைப்பு mosquitto *context, எழுத்து **username, எழுத்து **password, uint16_t password_len)
{
	முழு rc;

#ifdef WITH_TLS
	எனில்(context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
		/* Don't need the username or password if provided */
		mosquitto_FREE(*username);
		mosquitto_FREE(*password);

		எனில்(!context->ssl){
			திரும்பு send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
		}
#ifdef FINAL_WITH_TLS_PSK
		எனில்(context->listener->psk_hint){
			/* Client should have provided an identity to get this far. */
			எனில்(!context->username){
				திரும்பு send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}இல்லையெனில்
#endif /* FINAL_WITH_TLS_PSK */
		{
			எனில்(context->listener->use_identity_as_username){
				rc = set_username_from_cert_identity(context);
			}இல்லையெனில்{   /* use_subject_as_username */
				rc = set_username_from_cert_subject_name(context);
			}
			எனில்(rc){
				திரும்பு rc;
			}
		}
	}இல்லையெனில்
#endif /* WITH_TLS */
	{
#ifdef WITH_TLS
		எனில்(context->listener->use_identity_as_username && context->listener->require_certificate){
			mosquitto_FREE(*username);
			mosquitto_FREE(*password);

			எனில்(!context->username){
				திரும்பு send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}இல்லையெனில்
#endif
		{
			/* FIXME - these ensure the mosquitto_clientid() and
			* mosquitto_client_username() functions work, but is hacky */
			context->username = *username;
			context->password = *password;
			context->password_len = password_len;
			*username = வெற்று; /* Avoid free() in error: below. */
			*password = வெற்று;
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு handle_username_as_clientid_option(கட்டமைப்பு mosquitto *context)
{
	எனில்(context->username){
		mosquitto_FREE(context->id);
		context->id = mosquitto_strdup(context->username);
		எனில்(!context->id){
			திரும்பு MOSQ_ERR_NOMEM;
		}
	}இல்லையெனில்{
		uint8_t err_code = context->protocol == mosq_p_mqtt5
												? (uint8_t)MQTT_RC_NOT_AUTHORIZED
												: (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
		திரும்பு send__connack_error_and_return(context, err_code, MOSQ_ERR_AUTH);
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு handle__connect(கட்டமைப்பு mosquitto *context)
{
	எழுத்து protocol_name[7];
	uint8_t protocol_version;
	uint8_t connect_flags;
	எழுத்து *clientid = வெற்று;
	கட்டமைப்பு mosquitto *found_context;
	கட்டமைப்பு mosquitto_message_all *will_struct = வெற்று;
	uint8_t will, will_retain, will_qos, clean_start;
	uint8_t username_flag, password_flag;
	எழுத்து *username = வெற்று, *password = வெற்று;
	uint16_t password_len = 0;
	முழு rc;
	mosquitto_property *properties = வெற்று;
	வெறுமை *auth_data = வெற்று;
	uint16_t auth_data_len = 0;
	வெறுமை *auth_data_out = வெற்று;
	uint16_t auth_data_out_len = 0;
	bool allow_zero_length_clientid;

	எனில்(!context->listener){
		திரும்பு MOSQ_ERR_INVAL;
	}

	/* Don't accept multiple CONNECT commands. */
	எனில்(context->state != mosq_cs_new){
		log__printf(வெற்று, MOSQ_LOG_NOTICE, "Bad client %s:%d sending multiple CONNECT messages.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		செல் handle_connect_error;
	}

#ifdef WITH_TLS
	எனில்(context->in_packet.command == 0x16 && context->listener->ssl_ctx == வெற்று){ /* 0x16 is TLS handshake client hello */
		log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client from %s:%d appears to be using TLS to connect to a non-TLS listener.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		செல் handle_connect_error;
	}
#endif

	rc = read_protocol_name(context, protocol_name);
	எனில்(rc != MOSQ_ERR_SUCCESS){
		செல் handle_connect_error;
	}

	rc = read_and_verify_protocol_version(context, protocol_name, &protocol_version);
	எனில்(rc != MOSQ_ERR_SUCCESS){
		எனில்(rc == MOSQ_ERR_MALFORMED_PACKET){
			திரும்பு rc;
		}
		செல் handle_connect_error;
	}

	rc = read_and_verify_connect_flags(context, &connect_flags);
	எனில்(rc != MOSQ_ERR_SUCCESS){
		செல் handle_connect_error;
	}

	clean_start = (connect_flags & 0x02) >> 1;
	set_session_expiry_interval(context, clean_start, protocol_version);

	rc = read_and_reset_keepalive(context);
	எனில்(rc != MOSQ_ERR_SUCCESS){
		செல் handle_connect_error;
	}

	rc = read_and_verify_v5_connect_properties(context, &properties, protocol_version);
	எனில்(rc != MOSQ_ERR_SUCCESS){
		செல் handle_connect_error;
	}

	will = connect_flags & 0x04;
	will_qos = (connect_flags & 0x18) >> 3;
	will_retain = ((connect_flags & 0x20) == 0x20);
	rc = verify_will_options(context, will, will_qos, will_retain, protocol_version);
	எனில்(rc != MOSQ_ERR_SUCCESS){
		செல் handle_connect_error;
	}

	mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_METHOD, &context->auth_method, false);
	mosquitto_property_read_இருமம்(properties, MQTT_PROP_AUTHENTICATION_DATA, &auth_data, &auth_data_len, false);
	mosquitto_property_free_all(&properties);

	எனில்(auth_data && !context->auth_method){
		send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, வெற்று);
		log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s:%d CONNECT with missing clientid string.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		செல் handle_connect_error;
	}

	rc = read_and_verify_clientid_from_packet(context, &clientid, &allow_zero_length_clientid, clean_start);
	எனில்(rc != MOSQ_ERR_SUCCESS){
		செல் handle_connect_error;
	}

	எனில்(will){
		rc = will__read(context, clientid, &will_struct, will_qos, will_retain);
		எனில்(rc){
			எனில்(context->protocol == mosq_p_mqtt5){
				எனில்(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
					send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, வெற்று);
				}இல்லையெனில் எனில்(rc == MOSQ_ERR_MALFORMED_PACKET){
					send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, வெற்று);
				}
			}
			செல் handle_connect_error;
		}
	}இல்லையெனில்{
		எனில்(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
			எனில்(will_qos != 0 || will_retain != 0){
				log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT without Will with non-zero QoS (%d) or retain (%d).",
						clientid, will_qos, will_retain);
				rc = MOSQ_ERR_PROTOCOL;
				செல் handle_connect_error;
			}
		}
	}

	// Client credentials
	password_flag = connect_flags & 0x40;
	username_flag = connect_flags & 0x80;
	rc = read_and_verify_client_credentials_from_packet(context, &username, username_flag, &password, &password_len, password_flag, clientid);
	எனில்(rc != MOSQ_ERR_SUCCESS){
		செல் handle_connect_error;
	}

	rc = check_additional_trailing_data(context, protocol_version);
	எனில்(rc != MOSQ_ERR_SUCCESS){
		செல் handle_connect_error;
	}

	/* Once context->id is set, if we return from this function with an error
	 * we must make sure that context->id is freed and set to NULL, so that the
	 * client isn't erroneously removed from the by_id hash table. */
	context->id = clientid;
	clientid = வெற்று;

	/* use_identity_as_username or use_subject_as_username */
	rc = handle_username_from_cert_options(context, &username, &password, password_len);
	எனில்(rc != MOSQ_ERR_SUCCESS){
		செல் handle_connect_error;
	}

	/* use_username_as_clientid */
	எனில்(context->listener->use_username_as_clientid){
		rc = handle_username_as_clientid_option(context);
		எனில்(rc != MOSQ_ERR_SUCCESS){
			செல் handle_connect_error;
		}
	}

	/* Check for an existing delayed auth check, reject if present */
	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), found_context);
	எனில்(found_context){
		rc = MOSQ_ERR_UNKNOWN;
		செல் handle_connect_error;
	}

	context->clean_start = clean_start;
	context->will = will_struct;
	will_struct = வெற்று;

	எனில்(context->auth_method){
		rc = mosquitto_security_auth_start(context, false, auth_data, auth_data_len, &auth_data_out, &auth_data_out_len);
		mosquitto_FREE(auth_data);
		எனில்(rc == MOSQ_ERR_SUCCESS){
			திரும்பு connect__on_authorised(context, auth_data_out, auth_data_out_len);
		}இல்லையெனில் எனில்(rc == MOSQ_ERR_AUTH_CONTINUE){
			mosquitto__set_state(context, mosq_cs_authenticating);
			rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_data_out, auth_data_out_len);
			SAFE_FREE(auth_data_out);
			திரும்பு rc;
		}இல்லையெனில்{
			SAFE_FREE(auth_data_out);
			will__clear(context);
			எனில்(rc == MOSQ_ERR_AUTH){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, வெற்று);
				mosquitto_FREE(context->id);
				செல் handle_connect_error;
			}இல்லையெனில் எனில்(rc == MOSQ_ERR_NOT_SUPPORTED){
				/* Client has requested extended authentication, but we don't support it. */
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_METHOD, வெற்று);
				mosquitto_FREE(context->id);
				செல் handle_connect_error;
			}இல்லையெனில்{
				mosquitto_FREE(context->id);
				செல் handle_connect_error;
			}
		}
	}இல்லையெனில்{
#ifdef WITH_TLS
		எனில்(context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
			/* Authentication assumed to be cleared */
		}இல்லையெனில்
#endif
		{
			rc = mosquitto_basic_auth(context);
			தேர்வு(rc){
				நிலை MOSQ_ERR_SUCCESS:
					நிறுத்து;
				நிலை MOSQ_ERR_AUTH_DELAYED:
					mosquitto__set_state(context, mosq_cs_delayed_auth);
					HASH_ADD_KEYPTR(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context);
					திரும்பு MOSQ_ERR_SUCCESS;
					நிறுத்து;
				நிலை MOSQ_ERR_AUTH:
					எனில்(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, வெற்று);
					}இல்லையெனில்{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, வெற்று);
					}
					செல் handle_connect_error;
					நிறுத்து;
				நிலை MOSQ_ERR_UNSPECIFIED:
				நிலை MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
				நிலை MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
				நிலை MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
				நிலை MOSQ_ERR_SERVER_UNAVAILABLE:
				நிலை MOSQ_ERR_SERVER_BUSY:
				நிலை MOSQ_ERR_BANNED:
				நிலை MOSQ_ERR_BAD_AUTHENTICATION_METHOD:
				நிலை MOSQ_ERR_CONNECTION_RATE_EXCEEDED:
					எனில்(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, (uint8_t)rc, வெற்று);
					}இல்லையெனில்{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, வெற்று);
					}
					செல் handle_connect_error;
					நிறுத்து;
				இயல்பு:
					rc = MOSQ_ERR_UNKNOWN;
					செல் handle_connect_error;
					நிறுத்து;
			}
		}
		திரும்பு connect__on_authorised(context, வெற்று, 0);
	}


handle_connect_error:
	mosquitto_property_free_all(&properties);
	mosquitto_FREE(auth_data);
	mosquitto_FREE(clientid);
	mosquitto_FREE(username);
	mosquitto_FREE(password);
	எனில்(will_struct){
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct->செய்தி.payload);
		mosquitto_FREE(will_struct->செய்தி.topic);
		mosquitto_FREE(will_struct);
	}
	will__clear(context);
	/* We return an error here which means the client is freed later on. */
	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->will_delay_interval = 0;
	திரும்பு rc;
}
