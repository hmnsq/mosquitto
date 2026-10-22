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

#𐡄𐡍 defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif


ساكن حرف nibble_to_hex(uint8_t value)
{
	𐡄𐡍(value < 0x0A){
		أرجع (حرف)('0'+value);
	}وإلا{
		أرجع (حرف)(65 /*'A'*/ +value-10);
	}
}


ساكن حرف *clientid_gen(uint16_t *idlen, ثابت حرف *auto_id_prefix, uint16_t auto_id_prefix_len)
{
	حرف *clientid;
	uint8_t rnd[16];
	صحيح pos;

	𐡄𐡍(mosquitto_getrandom(rnd, 16)){
		أرجع عدم;
	}

	*idlen = (uint16_t)(auto_id_prefix_len + 36);

	clientid = (حرف *)mosquitto_calloc((size_t)(*idlen) + 1, حجم(حرف));
	𐡄𐡍(!clientid){
		أرجع عدم;
	}
	𐡄𐡍(auto_id_prefix){
		memcpy(clientid, auto_id_prefix, auto_id_prefix_len);
	}

	pos = 0;
	لكل(صحيح i=0; i<16; i++){
		clientid[auto_id_prefix_len + pos + 0] = nibble_to_hex(rnd[i] & 0x0F);
		clientid[auto_id_prefix_len + pos + 1] = nibble_to_hex((rnd[i] >> 4) & 0x0F);
		pos += 2;
		𐡄𐡍(pos == 8 || pos == 13 || pos == 18 || pos == 23){
			clientid[auto_id_prefix_len + pos] = '-';
			pos++;
		}
	}

	أرجع clientid;
}


صحيح connect__on_authorised(بنية mosquitto *context, فراغ *auth_data_out, uint16_t auth_data_out_len)
{
	بنية mosquitto *found_context;
	بنية mosquitto__subleaf *leaf;
	mosquitto_property *connack_props = عدم;
	uint8_t connect_ack = 0;
	صحيح rc;
	صحيح in_quota, out_quota;
	uint16_t in_maximum, out_maximum;

	/* Find if this client already has an entry. This must be done *after* any security checks. */
	HASH_FIND(hh_id, db.contexts_by_id, context->id, strlen(context->id), found_context);
	𐡄𐡍(found_context){
		/* Found a matching client */
		𐡄𐡍(!net__is_connected(found_context)){
			/* Client is reconnecting after a disconnect */
			/* FIXME - does anything need to be done here? */
		}وإلا{
			/* Client is already connected, disconnect old version. This is
			 * done in context__cleanup() below. */
		}

		𐡄𐡍(context->clean_start == true){
			sub__clean_session(found_context);
			found_context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			plugin_persist__handle_client_delete(found_context);
		}
		context->is_persisted = found_context->is_persisted;
		found_context->is_persisted = false; /* stops persistence for context being removed */

		𐡄𐡍(context->clean_start == false && found_context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
			𐡄𐡍(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
				connect_ack |= 0x01;
			}

			𐡄𐡍(found_context->msgs_in.inflight || found_context->msgs_in.queued
					|| found_context->msgs_out.inflight || found_context->msgs_out.queued){

				in_quota = context->msgs_in.inflight_quota;
				out_quota = context->msgs_out.inflight_quota;
				in_maximum = context->msgs_in.inflight_maximum;
				out_maximum = context->msgs_out.inflight_maximum;

				memcpy(&context->msgs_in, &found_context->msgs_in, حجم(بنية mosquitto_msg_data));
				memcpy(&context->msgs_out, &found_context->msgs_out, حجم(بنية mosquitto_msg_data));
				context->last_cmsg_id = found_context->last_cmsg_id;

				memset(&found_context->msgs_in, 0, حجم(بنية mosquitto_msg_data));
				memset(&found_context->msgs_out, 0, حجم(بنية mosquitto_msg_data));

				context->msgs_in.inflight_quota = in_quota;
				context->msgs_out.inflight_quota = out_quota;
				context->msgs_in.inflight_maximum = in_maximum;
				context->msgs_out.inflight_maximum = out_maximum;

				db__message_reconnect_reset(context);
			}
			context->subs = found_context->subs;
			found_context->subs = عدم;
			context->subs_capacity = found_context->subs_capacity;
			context->subs_count = found_context->subs_count;
			found_context->subs_capacity = 0;
			found_context->subs_count = 0;
			context->last_mid = found_context->last_mid;

			لكل(صحيح i=0; i<context->subs_capacity; i++){
				𐡄𐡍(context->subs[i]){
					leaf = context->subs[i]->hier->subs;
					طالما(leaf){
						𐡄𐡍(leaf->context == found_context){
							leaf->context = context;
						}
						leaf = leaf->next;
					}

					𐡄𐡍(context->subs[i]->shared){
						leaf = context->subs[i]->shared->subs;
						طالما(leaf){
							𐡄𐡍(leaf->context == found_context){
								leaf->context = context;
							}
							leaf = leaf->next;
						}
					}
				}
			}
		}

		𐡄𐡍((found_context->protocol == mosq_p_mqtt5 && found_context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE)
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
		𐡄𐡍(found_context->protocol == mosq_p_mqtt5){
			send__disconnect(found_context, MQTT_RC_SESSION_TAKEN_OVER, عدم);
		}
		do_disconnect(found_context, MOSQ_ERR_SESSION_TAKEN_OVER);
	}

	𐡄𐡍(db.config->global_max_clients > 0 && HASH_CNT(hh_id, db.contexts_by_id) >= (unsigned صحيح)db.config->global_max_clients){
		𐡄𐡍(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_SERVER_BUSY, عدم);
		}
		rc = MOSQ_ERR_INVAL;
		اذهب error;
	}

	𐡄𐡍(db.config->connection_messages == true){
		𐡄𐡍(context->is_bridge){
			𐡄𐡍(context->username){
				log__printf(عدم, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive, context->username);
			}وإلا{
				log__printf(عدم, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d).",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive);
			}
		}وإلا{
			𐡄𐡍(context->username){
				log__printf(عدم, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive, context->username);
			}وإلا{
				log__printf(عدم, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d).",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive);
			}
		}

		𐡄𐡍(context->will){
			log__printf(عدم, MOSQ_LOG_DEBUG, "Will message specified (%ld bytes) (r%d, q%d).",
					(long)context->will->msg.payloadlen,
					context->will->msg.retain,
					context->will->msg.qos);

			log__printf(عدم, MOSQ_LOG_DEBUG, "\t%s", context->will->msg.topic);
		}وإلا{
			log__printf(عدم, MOSQ_LOG_DEBUG, "No will message specified.");
		}
	}
#ifdef WITH_TLS
	𐡄𐡍(context->ssl){
		log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s negotiated %s cipher %s",
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
	𐡄𐡍(!context->clean_start){
		db.persistence_changes++;
	}
#endif
	context->max_qos = context->listener->max_qos;

	𐡄𐡍(db.config->max_keepalive &&
			(context->keepalive > db.config->max_keepalive || context->keepalive == 0)){

		keepalive__remove(context);
		context->keepalive = db.config->max_keepalive;
		keepalive__add(context);
		𐡄𐡍(context->protocol == mosq_p_mqtt5){
			𐡄𐡍(mosquitto_property_add_int16(&connack_props, MQTT_PROP_SERVER_KEEP_ALIVE, context->keepalive)){
				rc = MOSQ_ERR_NOMEM;
				اذهب error;
			}
		}وإلا{
			send__connack(context, connect_ack, CONNACK_REFUSED_IDENTIFIER_REJECTED, عدم);
			rc = MOSQ_ERR_INVAL;
			اذهب error;
		}
	}


	𐡄𐡍(context->protocol == mosq_p_mqtt5){
		𐡄𐡍(context->listener->max_topic_alias > 0){
			𐡄𐡍(mosquitto_property_add_int16(&connack_props, MQTT_PROP_TOPIC_ALIAS_MAXIMUM, context->listener->max_topic_alias)){
				rc = MOSQ_ERR_NOMEM;
				اذهب error;
			}
		}
		𐡄𐡍(context->assigned_id){
			𐡄𐡍(mosquitto_property_add_string(&connack_props, MQTT_PROP_ASSIGNED_CLIENT_IDENTIFIER, context->id)){
				rc = MOSQ_ERR_NOMEM;
				اذهب error;
			}
		}
		𐡄𐡍(context->auth_method){
			𐡄𐡍(mosquitto_property_add_string(&connack_props, MQTT_PROP_AUTHENTICATION_METHOD, context->auth_method)){
				rc = MOSQ_ERR_NOMEM;
				اذهب error;
			}

			𐡄𐡍(auth_data_out && auth_data_out_len > 0){
				𐡄𐡍(mosquitto_property_add_binary(&connack_props, MQTT_PROP_AUTHENTICATION_DATA, auth_data_out, auth_data_out_len)){
					rc = MOSQ_ERR_NOMEM;
					اذهب error;
				}
			}
		}
	}
	SAFE_FREE(auth_data_out);

	mosquitto__set_state(context, mosq_cs_active);
	rc = send__connack(context, connect_ack, CONNACK_ACCEPTED, connack_props);
	mosquitto_property_free_all(&connack_props);
	𐡄𐡍(rc){
		أرجع rc;
	}
	db__expire_all_messages(context);
	rc = db__message_write_queued_out(context);
	𐡄𐡍(rc){
		أرجع rc;
	}
	rc = db__message_write_inflight_out_all(context);

	𐡄𐡍(rc == MOSQ_ERR_SUCCESS){
		plugin__handle_connect(context);

		𐡄𐡍(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
			plugin_persist__handle_client_add(context);
		}وإلا 𐡄𐡍(context->will){
			plugin_persist__handle_will_add(context);
		}
	}
	أرجع rc;
error:
	SAFE_FREE(auth_data_out);
	mosquitto_property_free_all(&connack_props);
	أرجع rc;
}


ساكن صحيح will__read(بنية mosquitto *context, ثابت حرف *clientid, بنية mosquitto_message_all **will, uint8_t will_qos, صحيح will_retain)
{
	صحيح rc = MOSQ_ERR_SUCCESS;
	size_t slen;
	uint16_t tlen;
	بنية mosquitto_message_all *will_struct = عدم;
	حرف *will_topic_mount = عدم;
	uint16_t payloadlen;
	mosquitto_property *properties = عدم;

	will_struct = mosquitto_calloc(1, حجم(بنية mosquitto_message_all));
	𐡄𐡍(!will_struct){
		rc = MOSQ_ERR_NOMEM;
		اذهب error_cleanup;
	}
	𐡄𐡍(context->protocol == PROTOCOL_VERSION_v5){
		rc = property__read_all(CMD_WILL, &context->in_packet, &properties);
		𐡄𐡍(rc){
			اذهب error_cleanup;
		}

		rc = property__process_will(context, will_struct, &properties);
		mosquitto_property_free_all(&properties);
		𐡄𐡍(rc){
			اذهب error_cleanup;
		}
	}
	rc = packet__read_string(&context->in_packet, &will_struct->msg.topic, &tlen);
	𐡄𐡍(rc){
		اذهب error_cleanup;
	}
	𐡄𐡍(!tlen){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Will with empty topic.",
				context->id);
		rc = MOSQ_ERR_PROTOCOL;
		اذهب error_cleanup;
	}

	𐡄𐡍(context->listener->mount_point){
		slen = strlen(context->listener->mount_point) + strlen(will_struct->msg.topic) + 1;
		will_topic_mount = mosquitto_malloc(slen+1);
		𐡄𐡍(!will_topic_mount){
			rc = MOSQ_ERR_NOMEM;
			اذهب error_cleanup;
		}

		snprintf(will_topic_mount, slen, "%s%s", context->listener->mount_point, will_struct->msg.topic);
		will_topic_mount[slen] = '\0';

		mosquitto_FREE(will_struct->msg.topic);
		will_struct->msg.topic = will_topic_mount;
	}

	𐡄𐡍(!strncmp(will_struct->msg.topic, "$CONTROL/", strlen("$CONTROL/"))){
		rc = MOSQ_ERR_ACL_DENIED;
		اذهب error_cleanup;
	}
	rc = mosquitto_pub_topic_check(will_struct->msg.topic);
	𐡄𐡍(rc){
		اذهب error_cleanup;
	}

	rc = packet__read_uint16(&context->in_packet, &payloadlen);
	𐡄𐡍(rc){
		اذهب error_cleanup;
	}

	will_struct->msg.payloadlen = payloadlen;
	𐡄𐡍(will_struct->msg.payloadlen > 0){
		𐡄𐡍(db.config->message_size_limit && will_struct->msg.payloadlen > (صحيح)db.config->message_size_limit){
			log__printf(عدم, MOSQ_LOG_DEBUG, "Client %s connected with too large Will payload", clientid);
			𐡄𐡍(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_PACKET_TOO_LARGE, عدم);
			}وإلا{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, عدم);
			}
			rc = MOSQ_ERR_PAYLOAD_SIZE;
			اذهب error_cleanup;
		}
		will_struct->msg.payload = mosquitto_malloc((size_t)will_struct->msg.payloadlen);
		𐡄𐡍(!will_struct->msg.payload){
			rc = MOSQ_ERR_NOMEM;
			اذهب error_cleanup;
		}

		rc = packet__read_bytes(&context->in_packet, will_struct->msg.payload, (uint32_t)will_struct->msg.payloadlen);
		𐡄𐡍(rc){
			اذهب error_cleanup;
		}
	}

	will_struct->msg.qos = will_qos;
	will_struct->msg.retain = will_retain;

	*will = will_struct;
	أرجع MOSQ_ERR_SUCCESS;

error_cleanup:
	𐡄𐡍(will_struct){
		mosquitto_FREE(will_struct->msg.topic);
		mosquitto_FREE(will_struct->msg.payload);
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct);
	}
	أرجع rc;
}


ساكن صحيح check_protocol_version(بنية mosquitto__listener *listener, صحيح protocol_version)
{
	/* Allow bridge protocol as well. */
	protocol_version &= 0x7F;

	𐡄𐡍((protocol_version == 3 && listener->disable_protocol_v3 == false)
			|| (protocol_version == 4 && listener->disable_protocol_v4 == false)
			|| (protocol_version == 5 && listener->disable_protocol_v5 == false)
			){

		أرجع MOSQ_ERR_SUCCESS;
	}وإلا{
		أرجع MOSQ_ERR_NOT_SUPPORTED;
	}
}


مضمن ساكن صحيح send__connack_error_and_return(بنية mosquitto *context, uint8_t err_code, صحيح rc)
{
	send__connack(context, 0, err_code, عدم);
	أرجع rc;
}


مضمن ساكن صحيح send__connack_bad_username_or_password_error(بنية mosquitto *context, صحيح rc)
{
	uint8_t err_code  = context->protocol == mosq_p_mqtt5
									? (uint8_t)MQTT_RC_BAD_USERNAME_OR_PASSWORD
									: (uint8_t)CONNACK_REFUSED_BAD_USERNAME_PASSWORD;
	أرجع send__connack_error_and_return(context, err_code, rc);
}


ساكن صحيح read_protocol_name(بنية mosquitto *context, حرف protocol_name[7])
{
	/* Read protocol name as length then bytes rather than with read_string
	 * because the length is fixed and we can check that. Removes the need
	 * for another malloc as well. */

	uint16_t slen = 0;

	𐡄𐡍(packet__read_uint16(&context->in_packet, &slen)){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string length.",
				context->address, context->remote_port);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	𐡄𐡍(slen != 4 /* MQTT */ && slen != 6 /* MQIsdp */){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with incorrect protocol string length (%d).",
				context->address, context->remote_port, slen);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	𐡄𐡍(packet__read_bytes(&context->in_packet, protocol_name, slen)){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string.",
				context->address, context->remote_port);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	protocol_name[slen] = '\0';

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح read_and_verify_protocol_version(بنية mosquitto *context, ثابت حرف *protocol_name,
		uint8_t *protocol_version)
{
	uint8_t tmp_protocol_version = 0;
	𐡄𐡍(packet__read_byte(&context->in_packet, &tmp_protocol_version)){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol version.",
				context->address, context->remote_port);
		أرجع MOSQ_ERR_PROTOCOL;
	}

	𐡄𐡍(check_protocol_version(context->listener, tmp_protocol_version)){
		𐡄𐡍(tmp_protocol_version == 3 || tmp_protocol_version == 4){
			context->protocol = mosq_p_mqtt311;
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, عدم);
		}وإلا{
			context->protocol = mosq_p_mqtt5;
			send__connack(context, 0, MQTT_RC_UNSUPPORTED_PROTOCOL_VERSION, عدم);
		}
		أرجع MOSQ_ERR_NOT_SUPPORTED;
	}

	𐡄𐡍(!strcmp(protocol_name, PROTOCOL_NAME_v31)){
		𐡄𐡍((tmp_protocol_version&0x7F) != PROTOCOL_VERSION_v31){
			𐡄𐡍(db.config->connection_messages == true){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->address, context->remote_port, tmp_protocol_version);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, عدم);
			أرجع MOSQ_ERR_PROTOCOL;
		}
		context->protocol = mosq_p_mqtt31;
		𐡄𐡍((tmp_protocol_version&0x80) == 0x80){
			context->is_bridge = true;
		}
	}وإلا 𐡄𐡍(!strcmp(protocol_name, PROTOCOL_NAME)){
		𐡄𐡍((tmp_protocol_version&0x7F) == PROTOCOL_VERSION_v311){
			context->protocol = mosq_p_mqtt311;

			𐡄𐡍((tmp_protocol_version&0x80) == 0x80){
				context->is_bridge = true;
			}
		}وإلا 𐡄𐡍((tmp_protocol_version&0x7F) == PROTOCOL_VERSION_v5){
			context->protocol = mosq_p_mqtt5;
		}وإلا{
			𐡄𐡍(db.config->connection_messages == true){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->address, context->remote_port, tmp_protocol_version);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, عدم);
			أرجع MOSQ_ERR_PROTOCOL;
		}
		𐡄𐡍((context->in_packet.command&0x0F) != 0x00){
			/* Reserved flags not set to 0, must disconnect. */
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero reserved flags (%02X).",
					context->address, context->remote_port, context->in_packet.command);
			أرجع MOSQ_ERR_PROTOCOL;
		}
	}وإلا{
		𐡄𐡍(db.config->connection_messages == true){
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol \"%s\".",
					context->address, context->remote_port, protocol_name);
		}
		أرجع MOSQ_ERR_PROTOCOL;
	}
	𐡄𐡍((tmp_protocol_version&0x7F) != PROTOCOL_VERSION_v31 && context->in_packet.command != CMD_CONNECT){
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}

	*protocol_version = tmp_protocol_version;
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح read_and_verify_connect_flags(بنية mosquitto *context, uint8_t *connect_flags)
{
	𐡄𐡍(packet__read_byte(&context->in_packet, connect_flags)){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing connect flags.",
				context->address, context->remote_port);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	𐡄𐡍(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		𐡄𐡍((*connect_flags & 0x01) != 0x00){
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero connect reserved flag (%02X).",
					context->address, context->remote_port, *connect_flags);
			أرجع MOSQ_ERR_PROTOCOL;
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن فراغ set_session_expiry_interval(بنية mosquitto *context, uint8_t clean_start, uint8_t protocol_version)
{
	/* session_expiry_interval will be overridden if the properties are read later */
	𐡄𐡍(clean_start == false && protocol_version != PROTOCOL_VERSION_v5){
		/* v3* has clean_start == false mean the session never expires */
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
	}وإلا{
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	}
}


ساكن صحيح read_and_reset_keepalive(بنية mosquitto *context)
{
	/* _remove here because net__socket_accept() uses _add and we must have the
	 * correct keepalive value */
	keepalive__remove(context);

	𐡄𐡍(packet__read_uint16(&context->in_packet, &(context->keepalive))){
		log__printf(عدم, MOSQ_LOG_INFO, "%s sent CONNECT with missing keepalive.",
				context->id);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	keepalive__add(context);

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح read_and_verify_v5_connect_properties(بنية mosquitto *context, mosquitto_property **properties, uint8_t protocol_version)
{
	صحيح rc;

	𐡄𐡍(protocol_version == PROTOCOL_VERSION_v5){
		rc = property__read_all(CMD_CONNECT, &context->in_packet, properties);
		𐡄𐡍(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
			send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, عدم);
		}وإلا 𐡄𐡍(rc == MOSQ_ERR_MALFORMED_PACKET){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, عدم);
		}
		𐡄𐡍(rc){
			أرجع rc;
		}
	}
	rc = property__process_connect(context, properties);
	𐡄𐡍(rc != MOSQ_ERR_SUCCESS){
		أرجع send__connack_error_and_return(context, MQTT_RC_PROTOCOL_ERROR, rc);
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح verify_will_options(بنية mosquitto *context, uint8_t will, uint8_t will_qos, uint8_t will_retain, uint8_t protocol_version)
{
	𐡄𐡍(will_qos == 3){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid Will QoS (%d).",
				context->address, context->remote_port, will_qos);
		أرجع MOSQ_ERR_PROTOCOL;
	}

	𐡄𐡍(will && will_retain && db.config->retain_available == false){
		𐡄𐡍(protocol_version == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_RETAIN_NOT_SUPPORTED, عدم);
		}
		أرجع MOSQ_ERR_NOT_SUPPORTED;
	}

	𐡄𐡍(will && will_qos > context->listener->max_qos){
		𐡄𐡍(protocol_version == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_QOS_NOT_SUPPORTED, عدم);
		}
		أرجع MOSQ_ERR_NOT_SUPPORTED;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح handle_zero_length_clientid(بنية mosquitto *context, حرف **clientid, bool *allow_zero_length_clientid,
		uint8_t clean_start)
{
	𐡄𐡍(context->protocol == mosq_p_mqtt31){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: v3.1 CONNECT with zero length clientid.",
				context->address, context->remote_port);
		send__connack(context, 0, CONNACK_REFUSED_IDENTIFIER_REJECTED, عدم);
		أرجع MOSQ_ERR_PROTOCOL;
	}

	/* mqtt311/mqtt5 */
	mosquitto_FREE(*clientid);

	𐡄𐡍(db.config->per_listener_settings){
		*allow_zero_length_clientid = context->listener->security_options->allow_zero_length_clientid;
	}وإلا{
		*allow_zero_length_clientid = db.config->security_options.allow_zero_length_clientid;
	}

	𐡄𐡍((context->protocol == mosq_p_mqtt311 && clean_start == 0) || *allow_zero_length_clientid == false){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with zero length clientid when forbidden.",
				context->address, context->remote_port);
		uint8_t err_code = context->protocol == mosq_p_mqtt311 ? (uint8_t)CONNACK_REFUSED_IDENTIFIER_REJECTED : (uint8_t)MQTT_RC_UNSPECIFIED;
		أرجع send__connack_error_and_return(context, err_code, MOSQ_ERR_PROTOCOL);
	}

	*clientid = clientid_gen(&(uint16_t){0}, context->listener->security_options->auto_id_prefix,
			context->listener->security_options->auto_id_prefix_len);
	𐡄𐡍(*clientid == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}
	context->assigned_id = true;

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح check_clientid_prefixes(بنية mosquitto *context, ثابت حرف *clientid)
{
	𐡄𐡍(db.config->clientid_prefixes){
		𐡄𐡍(strncmp(db.config->clientid_prefixes, clientid, strlen(db.config->clientid_prefixes))){
			uint8_t err_code = context->protocol == mosq_p_mqtt5 ? (uint8_t)MQTT_RC_NOT_AUTHORIZED : (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
			أرجع send__connack_error_and_return(context, err_code, MOSQ_ERR_AUTH);
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح read_and_verify_clientid_from_packet(بنية mosquitto *context, حرف **clientid,
		bool *allow_zero_length_clientid, uint8_t clean_start)
{
	صحيح rc;
	uint16_t slen;

	rc = packet__read_string(&context->in_packet, clientid, &slen);
	𐡄𐡍(rc == MOSQ_ERR_MALFORMED_UTF8){
		𐡄𐡍(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_CLIENTID_NOT_VALID, عدم);
		}
		أرجع MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID;
	}وإلا 𐡄𐡍(rc > 0){
		أرجع MOSQ_ERR_PROTOCOL;
	}

	𐡄𐡍(slen == 0){
		rc = handle_zero_length_clientid(context, clientid, allow_zero_length_clientid, clean_start);
		𐡄𐡍(rc != MOSQ_ERR_SUCCESS){
			أرجع rc;
		}
	}

	rc = check_clientid_prefixes(context, *clientid);
	𐡄𐡍(rc != MOSQ_ERR_SUCCESS){
		أرجع rc;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح set_username_from_packet(بنية mosquitto *context, حرف **username, ثابت حرف *clientid)
{
	صحيح rc;

	rc = packet__read_string(&context->in_packet, username, &(uint16_t){0});
	𐡄𐡍(rc == MOSQ_ERR_NOMEM){
		أرجع MOSQ_ERR_NOMEM;
	}
	𐡄𐡍(rc != MOSQ_ERR_SUCCESS){
		𐡄𐡍(context->protocol == mosq_p_mqtt31){
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


ساكن صحيح set_password_from_packet(بنية mosquitto *context, حرف **password, uint16_t *password_len, ثابت حرف *clientid)
{
	صحيح rc;

	rc = packet__read_binary(&context->in_packet, (uint8_t **)password, password_len);
	𐡄𐡍(rc == MOSQ_ERR_NOMEM){
		أرجع MOSQ_ERR_NOMEM;
	}

	𐡄𐡍(rc == MOSQ_ERR_MALFORMED_PACKET){
		𐡄𐡍(context->protocol == mosq_p_mqtt31){
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

	𐡄𐡍(username_flag){
		rc = set_username_from_packet(context, username, clientid);
		𐡄𐡍(rc != MOSQ_ERR_SUCCESS){
			أرجع rc;
		}
	}وإلا{
		𐡄𐡍(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
			𐡄𐡍(password_flag){
				/* username_flag == 0 && password_flag == 1 is forbidden */
				log__printf(عدم, MOSQ_LOG_ERR, "Protocol error from %s: password without username, closing connection.", clientid);
				أرجع MOSQ_ERR_PROTOCOL;
			}
		}
	}
	𐡄𐡍(password_flag){
		rc = set_password_from_packet(context, password, password_len, clientid);
		𐡄𐡍(rc != MOSQ_ERR_SUCCESS){
			أرجع rc;
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح check_additional_trailing_data(بنية mosquitto *context, uint8_t protocol_version)
{
	𐡄𐡍(context->in_packet.pos != context->in_packet.remaining_length){
		/* Surplus data at end of packet, this must be an error. */
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->in_packet.pos, context->in_packet.remaining_length);
		𐡄𐡍(protocol_version == PROTOCOL_VERSION_v5){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, عدم);
		}
		أرجع MOSQ_ERR_PROTOCOL;
	}

	أرجع MOSQ_ERR_SUCCESS;
}

#ifdef WITH_TLS


مضمن ساكن صحيح get_client_cert_and_subject_name(بنية mosquitto *context, X509 **client_cert, X509_NAME **name)
{
	*client_cert = SSL_get_peer_certificate(context->ssl);
	𐡄𐡍(*client_cert == عدم){
		أرجع send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	*name = X509_get_subject_name(*client_cert);
	𐡄𐡍(*name == عدم){
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


ساكن صحيح set_username_from_cert_identity(بنية mosquitto *context)
{
	X509 *client_cert = عدم;
	X509_NAME *name = عدم;

	𐡄𐡍(get_client_cert_and_subject_name(context, &client_cert, &name)){
		أرجع MOSQ_ERR_AUTH;
	}

	صحيح i = -1;
	X509_NAME_ENTRY *name_entry = عدم;

	i = X509_NAME_get_index_by_NID(name, NID_commonName, -1);
	𐡄𐡍(i == -1){
		أرجع free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	name_entry = X509_NAME_get_entry(name, i);
	𐡄𐡍(!name_entry){
		اذهب success;
	}

	ASN1_STRING *name_asn1 = عدم;
	name_asn1 = X509_NAME_ENTRY_get_data(name_entry);
	𐡄𐡍(name_asn1 == عدم){
		أرجع free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	ثابت حرف *cert_identity = (ثابت حرف *)ASN1_STRING_get0_data(name_asn1);
	𐡄𐡍(!cert_identity || mosquitto_validate_utf8(cert_identity, (صحيح)strlen(cert_identity))){
		أرجع free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	mosquitto_free(context->username);
	context->username = mosquitto_strdup(cert_identity);
	𐡄𐡍(context->username == عدم){
		أرجع free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_NOMEM);
	}
	/* Make sure there isn't an embedded NUL character in the CN */
	𐡄𐡍((size_t)ASN1_STRING_length(name_asn1) != strlen(context->username)){
		أرجع free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}

success:
	X509_free(client_cert);
	client_cert = عدم;
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح set_username_from_cert_subject_name(بنية mosquitto *context)
{
	X509 *client_cert = عدم;
	X509_NAME *name = عدم;

	𐡄𐡍(get_client_cert_and_subject_name(context, &client_cert, &name)){
		أرجع MOSQ_ERR_AUTH;
	}

	حرف *subject = عدم;
	حرف *data_start = عدم;
	BIO *subject_bio = عدم;
	long name_length = 0;

	subject_bio = BIO_new(BIO_s_mem());
	X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
	data_start = عدم;
	name_length = BIO_get_mem_data(subject_bio, &data_start);
	subject = mosquitto_malloc(حجم(حرف)*(size_t)(name_length+1));
	𐡄𐡍(!subject){
		أرجع free_x509_and_BIO_and_send_connack_error(context, client_cert, subject_bio, MOSQ_ERR_NOMEM);
	}
	memcpy(subject, data_start, (size_t)name_length);
	subject[name_length] = '\0';
	BIO_free(subject_bio);

	𐡄𐡍(mosquitto_validate_utf8(subject, (صحيح)strlen(subject))){
		mosquitto_free(subject);
		أرجع free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	context->username = subject;
	𐡄𐡍(!context->username){
		X509_free(client_cert);
		أرجع MOSQ_ERR_AUTH;
	}

	X509_free(client_cert);
	client_cert = عدم;
	أرجع MOSQ_ERR_SUCCESS;
}
#endif


ساكن صحيح handle_username_from_cert_options(بنية mosquitto *context, حرف **username, حرف **password, uint16_t password_len)
{
	صحيح rc;

#ifdef WITH_TLS
	𐡄𐡍(context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
		/* Don't need the username or password if provided */
		mosquitto_FREE(*username);
		mosquitto_FREE(*password);

		𐡄𐡍(!context->ssl){
			أرجع send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
		}
#ifdef FINAL_WITH_TLS_PSK
		𐡄𐡍(context->listener->psk_hint){
			/* Client should have provided an identity to get this far. */
			𐡄𐡍(!context->username){
				أرجع send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}وإلا
#endif /* FINAL_WITH_TLS_PSK */
		{
			𐡄𐡍(context->listener->use_identity_as_username){
				rc = set_username_from_cert_identity(context);
			}وإلا{   /* use_subject_as_username */
				rc = set_username_from_cert_subject_name(context);
			}
			𐡄𐡍(rc){
				أرجع rc;
			}
		}
	}وإلا
#endif /* WITH_TLS */
	{
#ifdef WITH_TLS
		𐡄𐡍(context->listener->use_identity_as_username && context->listener->require_certificate){
			mosquitto_FREE(*username);
			mosquitto_FREE(*password);

			𐡄𐡍(!context->username){
				أرجع send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}وإلا
#endif
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
	𐡄𐡍(context->username){
		mosquitto_FREE(context->id);
		context->id = mosquitto_strdup(context->username);
		𐡄𐡍(!context->id){
			أرجع MOSQ_ERR_NOMEM;
		}
	}وإلا{
		uint8_t err_code = context->protocol == mosq_p_mqtt5
												? (uint8_t)MQTT_RC_NOT_AUTHORIZED
												: (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
		أرجع send__connack_error_and_return(context, err_code, MOSQ_ERR_AUTH);
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح handle__connect(بنية mosquitto *context)
{
	حرف protocol_name[7];
	uint8_t protocol_version;
	uint8_t connect_flags;
	حرف *clientid = عدم;
	بنية mosquitto *found_context;
	بنية mosquitto_message_all *will_struct = عدم;
	uint8_t will, will_retain, will_qos, clean_start;
	uint8_t username_flag, password_flag;
	حرف *username = عدم, *password = عدم;
	uint16_t password_len = 0;
	صحيح rc;
	mosquitto_property *properties = عدم;
	فراغ *auth_data = عدم;
	uint16_t auth_data_len = 0;
	فراغ *auth_data_out = عدم;
	uint16_t auth_data_out_len = 0;
	bool allow_zero_length_clientid;

	𐡄𐡍(!context->listener){
		أرجع MOSQ_ERR_INVAL;
	}

	/* Don't accept multiple CONNECT commands. */
	𐡄𐡍(context->state != mosq_cs_new){
		log__printf(عدم, MOSQ_LOG_NOTICE, "Bad client %s:%d sending multiple CONNECT messages.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		اذهب handle_connect_error;
	}

#ifdef WITH_TLS
	𐡄𐡍(context->in_packet.command == 0x16 && context->listener->ssl_ctx == عدم){ /* 0x16 is TLS handshake client hello */
		log__printf(عدم, MOSQ_LOG_NOTICE, "Client from %s:%d appears to be using TLS to connect to a non-TLS listener.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		اذهب handle_connect_error;
	}
#endif

	rc = read_protocol_name(context, protocol_name);
	𐡄𐡍(rc != MOSQ_ERR_SUCCESS){
		اذهب handle_connect_error;
	}

	rc = read_and_verify_protocol_version(context, protocol_name, &protocol_version);
	𐡄𐡍(rc != MOSQ_ERR_SUCCESS){
		𐡄𐡍(rc == MOSQ_ERR_MALFORMED_PACKET){
			أرجع rc;
		}
		اذهب handle_connect_error;
	}

	rc = read_and_verify_connect_flags(context, &connect_flags);
	𐡄𐡍(rc != MOSQ_ERR_SUCCESS){
		اذهب handle_connect_error;
	}

	clean_start = (connect_flags & 0x02) >> 1;
	set_session_expiry_interval(context, clean_start, protocol_version);

	rc = read_and_reset_keepalive(context);
	𐡄𐡍(rc != MOSQ_ERR_SUCCESS){
		اذهب handle_connect_error;
	}

	rc = read_and_verify_v5_connect_properties(context, &properties, protocol_version);
	𐡄𐡍(rc != MOSQ_ERR_SUCCESS){
		اذهب handle_connect_error;
	}

	will = connect_flags & 0x04;
	will_qos = (connect_flags & 0x18) >> 3;
	will_retain = ((connect_flags & 0x20) == 0x20);
	rc = verify_will_options(context, will, will_qos, will_retain, protocol_version);
	𐡄𐡍(rc != MOSQ_ERR_SUCCESS){
		اذهب handle_connect_error;
	}

	mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_METHOD, &context->auth_method, false);
	mosquitto_property_read_binary(properties, MQTT_PROP_AUTHENTICATION_DATA, &auth_data, &auth_data_len, false);
	mosquitto_property_free_all(&properties);

	𐡄𐡍(auth_data && !context->auth_method){
		send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, عدم);
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s:%d CONNECT with missing clientid string.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		اذهب handle_connect_error;
	}

	rc = read_and_verify_clientid_from_packet(context, &clientid, &allow_zero_length_clientid, clean_start);
	𐡄𐡍(rc != MOSQ_ERR_SUCCESS){
		اذهب handle_connect_error;
	}

	𐡄𐡍(will){
		rc = will__read(context, clientid, &will_struct, will_qos, will_retain);
		𐡄𐡍(rc){
			𐡄𐡍(context->protocol == mosq_p_mqtt5){
				𐡄𐡍(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
					send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, عدم);
				}وإلا 𐡄𐡍(rc == MOSQ_ERR_MALFORMED_PACKET){
					send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, عدم);
				}
			}
			اذهب handle_connect_error;
		}
	}وإلا{
		𐡄𐡍(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
			𐡄𐡍(will_qos != 0 || will_retain != 0){
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
	𐡄𐡍(rc != MOSQ_ERR_SUCCESS){
		اذهب handle_connect_error;
	}

	rc = check_additional_trailing_data(context, protocol_version);
	𐡄𐡍(rc != MOSQ_ERR_SUCCESS){
		اذهب handle_connect_error;
	}

	/* Once context->id is set, if we return from this function with an error
	 * we must make sure that context->id is freed and set to NULL, so that the
	 * client isn't erroneously removed from the by_id hash table. */
	context->id = clientid;
	clientid = عدم;

	/* use_identity_as_username or use_subject_as_username */
	rc = handle_username_from_cert_options(context, &username, &password, password_len);
	𐡄𐡍(rc != MOSQ_ERR_SUCCESS){
		اذهب handle_connect_error;
	}

	/* use_username_as_clientid */
	𐡄𐡍(context->listener->use_username_as_clientid){
		rc = handle_username_as_clientid_option(context);
		𐡄𐡍(rc != MOSQ_ERR_SUCCESS){
			اذهب handle_connect_error;
		}
	}

	/* Check for an existing delayed auth check, reject if present */
	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), found_context);
	𐡄𐡍(found_context){
		rc = MOSQ_ERR_UNKNOWN;
		اذهب handle_connect_error;
	}

	context->clean_start = clean_start;
	context->will = will_struct;
	will_struct = عدم;

	𐡄𐡍(context->auth_method){
		rc = mosquitto_security_auth_start(context, false, auth_data, auth_data_len, &auth_data_out, &auth_data_out_len);
		mosquitto_FREE(auth_data);
		𐡄𐡍(rc == MOSQ_ERR_SUCCESS){
			أرجع connect__on_authorised(context, auth_data_out, auth_data_out_len);
		}وإلا 𐡄𐡍(rc == MOSQ_ERR_AUTH_CONTINUE){
			mosquitto__set_state(context, mosq_cs_authenticating);
			rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_data_out, auth_data_out_len);
			SAFE_FREE(auth_data_out);
			أرجع rc;
		}وإلا{
			SAFE_FREE(auth_data_out);
			will__clear(context);
			𐡄𐡍(rc == MOSQ_ERR_AUTH){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, عدم);
				mosquitto_FREE(context->id);
				اذهب handle_connect_error;
			}وإلا 𐡄𐡍(rc == MOSQ_ERR_NOT_SUPPORTED){
				/* Client has requested extended authentication, but we don't support it. */
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_METHOD, عدم);
				mosquitto_FREE(context->id);
				اذهب handle_connect_error;
			}وإلا{
				mosquitto_FREE(context->id);
				اذهب handle_connect_error;
			}
		}
	}وإلا{
#ifdef WITH_TLS
		𐡄𐡍(context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
			/* Authentication assumed to be cleared */
		}وإلا
#endif
		{
			rc = mosquitto_basic_auth(context);
			بدل(rc){
				حالة MOSQ_ERR_SUCCESS:
					اكسر;
				حالة MOSQ_ERR_AUTH_DELAYED:
					mosquitto__set_state(context, mosq_cs_delayed_auth);
					HASH_ADD_KEYPTR(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context);
					أرجع MOSQ_ERR_SUCCESS;
					اكسر;
				حالة MOSQ_ERR_AUTH:
					𐡄𐡍(context->protocol == mosq_p_mqtt5){
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
				حالة MOSQ_ERR_BAD_AUTHENTICATION_METHOD:
				حالة MOSQ_ERR_CONNECTION_RATE_EXCEEDED:
					𐡄𐡍(context->protocol == mosq_p_mqtt5){
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
	mosquitto_FREE(auth_data);
	mosquitto_FREE(clientid);
	mosquitto_FREE(username);
	mosquitto_FREE(password);
	𐡄𐡍(will_struct){
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct->msg.payload);
		mosquitto_FREE(will_struct->msg.topic);
		mosquitto_FREE(will_struct);
	}
	will__clear(context);
	/* We return an error here which means the client is freed later on. */
	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->will_delay_interval = 0;
	أرجع rc;
}
