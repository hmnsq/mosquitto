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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <utlist.h>

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto/mqtt_protocol.h"
#অন্তর্ভাব "packet_mosq.h"
#অন্তর্ভাব "property_mosq.h"
#অন্তর্ভাব "send_mosq.h"
#অন্তর্ভাব "sys_tree.h"
#অন্তর্ভাব "tls_mosq.h"
#অন্তর্ভাব "util_mosq.h"
#অন্তর্ভাব "will_mosq.h"

#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif


স্থির অক্ষর nibble_to_hex(uint8_t মান)
{
	যদি(মান < 0x0A){
		ফেরত (অক্ষর)('0'+মান);
	}অন্যথা{
		ফেরত (অক্ষর)(65 /*'A'*/ +মান-10);
	}
}


স্থির অক্ষর *clientid_gen(uint16_t *idlen, ধ্রুবক অক্ষর *auto_id_prefix, uint16_t auto_id_prefix_len)
{
	অক্ষর *clientid;
	uint8_t rnd[16];
	পূর্ণসংখ্যা pos;

	যদি(mosquitto_getrandom(rnd, 16)){
		ফেরত ফাঁকা;
	}

	*idlen = (uint16_t)(auto_id_prefix_len + 36);

	clientid = (অক্ষর *)mosquitto_calloc((size_t)(*idlen) + 1, আকার(অক্ষর));
	যদি(!clientid){
		ফেরত ফাঁকা;
	}
	যদি(auto_id_prefix){
		memcpy(clientid, auto_id_prefix, auto_id_prefix_len);
	}

	pos = 0;
	জন্য(পূর্ণসংখ্যা i=0; i<16; i++){
		clientid[auto_id_prefix_len + pos + 0] = nibble_to_hex(rnd[i] & 0x0F);
		clientid[auto_id_prefix_len + pos + 1] = nibble_to_hex((rnd[i] >> 4) & 0x0F);
		pos += 2;
		যদি(pos == 8 || pos == 13 || pos == 18 || pos == 23){
			clientid[auto_id_prefix_len + pos] = '-';
			pos++;
		}
	}

	ফেরত clientid;
}


পূর্ণসংখ্যা connect__on_authorised(কাঠামো mosquitto *context, শূন্য *auth_তথ্য_out, uint16_t auth_তথ্য_out_len)
{
	কাঠামো mosquitto *found_context;
	কাঠামো mosquitto__subleaf *leaf;
	mosquitto_property *connack_props = ফাঁকা;
	uint8_t connect_ack = 0;
	পূর্ণসংখ্যা rc;
	পূর্ণসংখ্যা in_quota, out_quota;
	uint16_t in_maximum, out_maximum;

	/* Find if this client already has an entry. This must be done *after* any security checks. */
	HASH_FIND(hh_id, db.contexts_by_id, context->id, strlen(context->id), found_context);
	যদি(found_context){
		/* Found a matching client */
		যদি(!net__is_connected(found_context)){
			/* Client is reconnecting after a disconnect */
			/* FIXME - does anything need to be done here? */
		}অন্যথা{
			/* Client is already connected, disconnect old version. This is
			 * done in context__cleanup() below. */
		}

		যদি(context->clean_start == true){
			sub__clean_session(found_context);
			found_context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			plugin_persist__handle_client_delete(found_context);
		}
		context->is_persisted = found_context->is_persisted;
		found_context->is_persisted = false; /* stops persistence for context being removed */

		যদি(context->clean_start == false && found_context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
			যদি(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
				connect_ack |= 0x01;
			}

			যদি(found_context->msgs_in.inflight || found_context->msgs_in.queued
					|| found_context->msgs_out.inflight || found_context->msgs_out.queued){

				in_quota = context->msgs_in.inflight_quota;
				out_quota = context->msgs_out.inflight_quota;
				in_maximum = context->msgs_in.inflight_maximum;
				out_maximum = context->msgs_out.inflight_maximum;

				memcpy(&context->msgs_in, &found_context->msgs_in, আকার(কাঠামো mosquitto_বার্তা_তথ্য));
				memcpy(&context->msgs_out, &found_context->msgs_out, আকার(কাঠামো mosquitto_বার্তা_তথ্য));
				context->last_cmsg_id = found_context->last_cmsg_id;

				memset(&found_context->msgs_in, 0, আকার(কাঠামো mosquitto_বার্তা_তথ্য));
				memset(&found_context->msgs_out, 0, আকার(কাঠামো mosquitto_বার্তা_তথ্য));

				context->msgs_in.inflight_quota = in_quota;
				context->msgs_out.inflight_quota = out_quota;
				context->msgs_in.inflight_maximum = in_maximum;
				context->msgs_out.inflight_maximum = out_maximum;

				db__message_reconnect_reset(context);
			}
			context->subs = found_context->subs;
			found_context->subs = ফাঁকা;
			context->subs_capacity = found_context->subs_capacity;
			context->subs_গুনতি = found_context->subs_গুনতি;
			found_context->subs_capacity = 0;
			found_context->subs_গুনতি = 0;
			context->last_mid = found_context->last_mid;

			জন্য(পূর্ণসংখ্যা i=0; i<context->subs_capacity; i++){
				যদি(context->subs[i]){
					leaf = context->subs[i]->hier->subs;
					যখন(leaf){
						যদি(leaf->context == found_context){
							leaf->context = context;
						}
						leaf = leaf->পরবর্তী;
					}

					যদি(context->subs[i]->shared){
						leaf = context->subs[i]->shared->subs;
						যখন(leaf){
							যদি(leaf->context == found_context){
								leaf->context = context;
							}
							leaf = leaf->পরবর্তী;
						}
					}
				}
			}
		}

		যদি((found_context->protocol == mosq_p_mqtt5 && found_context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE)
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
		যদি(found_context->protocol == mosq_p_mqtt5){
			send__disconnect(found_context, MQTT_RC_SESSION_TAKEN_OVER, ফাঁকা);
		}
		do_disconnect(found_context, MOSQ_ERR_SESSION_TAKEN_OVER);
	}

	যদি(db.config->global_max_clients > 0 && HASH_CNT(hh_id, db.contexts_by_id) >= (unsigned পূর্ণসংখ্যা)db.config->global_max_clients){
		যদি(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_SERVER_BUSY, ফাঁকা);
		}
		rc = MOSQ_ERR_INVAL;
		যাও error;
	}

	যদি(db.config->connection_messages == true){
		যদি(context->is_bridge){
			যদি(context->username){
				log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive, context->username);
			}অন্যথা{
				log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d).",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive);
			}
		}অন্যথা{
			যদি(context->username){
				log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive, context->username);
			}অন্যথা{
				log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d).",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive);
			}
		}

		যদি(context->will){
			log__printf(ফাঁকা, MOSQ_LOG_DEBUG, "Will message specified (%ld bytes) (r%d, q%d).",
					(long)context->will->বার্তা.payloadlen,
					context->will->বার্তা.retain,
					context->will->বার্তা.qos);

			log__printf(ফাঁকা, MOSQ_LOG_DEBUG, "\t%s", context->will->বার্তা.topic);
		}অন্যথা{
			log__printf(ফাঁকা, MOSQ_LOG_DEBUG, "No will message specified.");
		}
	}
#ifdef WITH_TLS
	যদি(context->ssl){
		log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s negotiated %s cipher %s",
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
	যদি(!context->clean_start){
		db.persistence_changes++;
	}
#endif
	context->max_qos = context->listener->max_qos;

	যদি(db.config->max_keepalive &&
			(context->keepalive > db.config->max_keepalive || context->keepalive == 0)){

		keepalive__remove(context);
		context->keepalive = db.config->max_keepalive;
		keepalive__add(context);
		যদি(context->protocol == mosq_p_mqtt5){
			যদি(mosquitto_property_add_int16(&connack_props, MQTT_PROP_SERVER_KEEP_ALIVE, context->keepalive)){
				rc = MOSQ_ERR_NOMEM;
				যাও error;
			}
		}অন্যথা{
			send__connack(context, connect_ack, CONNACK_REFUSED_IDENTIFIER_REJECTED, ফাঁকা);
			rc = MOSQ_ERR_INVAL;
			যাও error;
		}
	}


	যদি(context->protocol == mosq_p_mqtt5){
		যদি(context->listener->max_topic_alias > 0){
			যদি(mosquitto_property_add_int16(&connack_props, MQTT_PROP_TOPIC_ALIAS_MAXIMUM, context->listener->max_topic_alias)){
				rc = MOSQ_ERR_NOMEM;
				যাও error;
			}
		}
		যদি(context->assigned_id){
			যদি(mosquitto_property_add_string(&connack_props, MQTT_PROP_ASSIGNED_CLIENT_IDENTIFIER, context->id)){
				rc = MOSQ_ERR_NOMEM;
				যাও error;
			}
		}
		যদি(context->auth_method){
			যদি(mosquitto_property_add_string(&connack_props, MQTT_PROP_AUTHENTICATION_METHOD, context->auth_method)){
				rc = MOSQ_ERR_NOMEM;
				যাও error;
			}

			যদি(auth_তথ্য_out && auth_তথ্য_out_len > 0){
				যদি(mosquitto_property_add_দ্বিমিক(&connack_props, MQTT_PROP_AUTHENTICATION_তথ্য, auth_তথ্য_out, auth_তথ্য_out_len)){
					rc = MOSQ_ERR_NOMEM;
					যাও error;
				}
			}
		}
	}
	SAFE_FREE(auth_তথ্য_out);

	mosquitto__set_state(context, mosq_cs_active);
	rc = send__connack(context, connect_ack, CONNACK_ACCEPTED, connack_props);
	mosquitto_property_free_all(&connack_props);
	যদি(rc){
		ফেরত rc;
	}
	db__expire_all_messages(context);
	rc = db__message_write_queued_out(context);
	যদি(rc){
		ফেরত rc;
	}
	rc = db__message_write_inflight_out_all(context);

	যদি(rc == MOSQ_ERR_SUCCESS){
		plugin__handle_connect(context);

		যদি(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
			plugin_persist__handle_client_add(context);
		}অন্যথা যদি(context->will){
			plugin_persist__handle_will_add(context);
		}
	}
	ফেরত rc;
error:
	SAFE_FREE(auth_তথ্য_out);
	mosquitto_property_free_all(&connack_props);
	ফেরত rc;
}


স্থির পূর্ণসংখ্যা will__read(কাঠামো mosquitto *context, ধ্রুবক অক্ষর *clientid, কাঠামো mosquitto_message_all **will, uint8_t will_qos, পূর্ণসংখ্যা will_retain)
{
	পূর্ণসংখ্যা rc = MOSQ_ERR_SUCCESS;
	size_t slen;
	uint16_t tlen;
	কাঠামো mosquitto_message_all *will_struct = ফাঁকা;
	অক্ষর *will_topic_mount = ফাঁকা;
	uint16_t payloadlen;
	mosquitto_property *properties = ফাঁকা;

	will_struct = mosquitto_calloc(1, আকার(কাঠামো mosquitto_message_all));
	যদি(!will_struct){
		rc = MOSQ_ERR_NOMEM;
		যাও error_cleanup;
	}
	যদি(context->protocol == PROTOCOL_VERSION_v5){
		rc = property__read_all(CMD_WILL, &context->in_packet, &properties);
		যদি(rc){
			যাও error_cleanup;
		}

		rc = property__process_will(context, will_struct, &properties);
		mosquitto_property_free_all(&properties);
		যদি(rc){
			যাও error_cleanup;
		}
	}
	rc = packet__read_string(&context->in_packet, &will_struct->বার্তা.topic, &tlen);
	যদি(rc){
		যাও error_cleanup;
	}
	যদি(!tlen){
		log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s: Will with empty topic.",
				context->id);
		rc = MOSQ_ERR_PROTOCOL;
		যাও error_cleanup;
	}

	যদি(context->listener->mount_point){
		slen = strlen(context->listener->mount_point) + strlen(will_struct->বার্তা.topic) + 1;
		will_topic_mount = mosquitto_malloc(slen+1);
		যদি(!will_topic_mount){
			rc = MOSQ_ERR_NOMEM;
			যাও error_cleanup;
		}

		snprintf(will_topic_mount, slen, "%s%s", context->listener->mount_point, will_struct->বার্তা.topic);
		will_topic_mount[slen] = '\0';

		mosquitto_FREE(will_struct->বার্তা.topic);
		will_struct->বার্তা.topic = will_topic_mount;
	}

	যদি(!strncmp(will_struct->বার্তা.topic, "$CONTROL/", strlen("$CONTROL/"))){
		rc = MOSQ_ERR_ACL_DENIED;
		যাও error_cleanup;
	}
	rc = mosquitto_pub_topic_check(will_struct->বার্তা.topic);
	যদি(rc){
		যাও error_cleanup;
	}

	rc = packet__read_uint16(&context->in_packet, &payloadlen);
	যদি(rc){
		যাও error_cleanup;
	}

	will_struct->বার্তা.payloadlen = payloadlen;
	যদি(will_struct->বার্তা.payloadlen > 0){
		যদি(db.config->message_আকার_limit && will_struct->বার্তা.payloadlen > (পূর্ণসংখ্যা)db.config->message_আকার_limit){
			log__printf(ফাঁকা, MOSQ_LOG_DEBUG, "Client %s connected with too large Will payload", clientid);
			যদি(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_PACKET_TOO_LARGE, ফাঁকা);
			}অন্যথা{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ফাঁকা);
			}
			rc = MOSQ_ERR_PAYLOAD_আকার;
			যাও error_cleanup;
		}
		will_struct->বার্তা.payload = mosquitto_malloc((size_t)will_struct->বার্তা.payloadlen);
		যদি(!will_struct->বার্তা.payload){
			rc = MOSQ_ERR_NOMEM;
			যাও error_cleanup;
		}

		rc = packet__read_bytes(&context->in_packet, will_struct->বার্তা.payload, (uint32_t)will_struct->বার্তা.payloadlen);
		যদি(rc){
			যাও error_cleanup;
		}
	}

	will_struct->বার্তা.qos = will_qos;
	will_struct->বার্তা.retain = will_retain;

	*will = will_struct;
	ফেরত MOSQ_ERR_SUCCESS;

error_cleanup:
	যদি(will_struct){
		mosquitto_FREE(will_struct->বার্তা.topic);
		mosquitto_FREE(will_struct->বার্তা.payload);
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct);
	}
	ফেরত rc;
}


স্থির পূর্ণসংখ্যা check_protocol_version(কাঠামো mosquitto__listener *listener, পূর্ণসংখ্যা protocol_version)
{
	/* Allow bridge protocol as well. */
	protocol_version &= 0x7F;

	যদি((protocol_version == 3 && listener->disable_protocol_v3 == false)
			|| (protocol_version == 4 && listener->disable_protocol_v4 == false)
			|| (protocol_version == 5 && listener->disable_protocol_v5 == false)
			){

		ফেরত MOSQ_ERR_SUCCESS;
	}অন্যথা{
		ফেরত MOSQ_ERR_NOT_SUPPORTED;
	}
}


অন্তঃরেখ স্থির পূর্ণসংখ্যা send__connack_error_and_return(কাঠামো mosquitto *context, uint8_t err_code, পূর্ণসংখ্যা rc)
{
	send__connack(context, 0, err_code, ফাঁকা);
	ফেরত rc;
}


অন্তঃরেখ স্থির পূর্ণসংখ্যা send__connack_bad_username_or_password_error(কাঠামো mosquitto *context, পূর্ণসংখ্যা rc)
{
	uint8_t err_code  = context->protocol == mosq_p_mqtt5
									? (uint8_t)MQTT_RC_BAD_USERNAME_OR_PASSWORD
									: (uint8_t)CONNACK_REFUSED_BAD_USERNAME_PASSWORD;
	ফেরত send__connack_error_and_return(context, err_code, rc);
}


স্থির পূর্ণসংখ্যা read_protocol_name(কাঠামো mosquitto *context, অক্ষর protocol_name[7])
{
	/* Read protocol name as length then bytes rather than with read_string
	 * because the length is fixed and we can check that. Removes the need
	 * for another malloc as well. */

	uint16_t slen = 0;

	যদি(packet__read_uint16(&context->in_packet, &slen)){
		log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string length.",
				context->address, context->remote_port);
		ফেরত MOSQ_ERR_PROTOCOL;
	}
	যদি(slen != 4 /* MQTT */ && slen != 6 /* MQIsdp */){
		log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with incorrect protocol string length (%d).",
				context->address, context->remote_port, slen);
		ফেরত MOSQ_ERR_PROTOCOL;
	}
	যদি(packet__read_bytes(&context->in_packet, protocol_name, slen)){
		log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string.",
				context->address, context->remote_port);
		ফেরত MOSQ_ERR_PROTOCOL;
	}
	protocol_name[slen] = '\0';

	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা read_and_verify_protocol_version(কাঠামো mosquitto *context, ধ্রুবক অক্ষর *protocol_name,
		uint8_t *protocol_version)
{
	uint8_t tmp_protocol_version = 0;
	যদি(packet__read_byte(&context->in_packet, &tmp_protocol_version)){
		log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol version.",
				context->address, context->remote_port);
		ফেরত MOSQ_ERR_PROTOCOL;
	}

	যদি(check_protocol_version(context->listener, tmp_protocol_version)){
		যদি(tmp_protocol_version == 3 || tmp_protocol_version == 4){
			context->protocol = mosq_p_mqtt311;
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, ফাঁকা);
		}অন্যথা{
			context->protocol = mosq_p_mqtt5;
			send__connack(context, 0, MQTT_RC_UNSUPPORTED_PROTOCOL_VERSION, ফাঁকা);
		}
		ফেরত MOSQ_ERR_NOT_SUPPORTED;
	}

	যদি(!strcmp(protocol_name, PROTOCOL_NAME_v31)){
		যদি((tmp_protocol_version&0x7F) != PROTOCOL_VERSION_v31){
			যদি(db.config->connection_messages == true){
				log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->address, context->remote_port, tmp_protocol_version);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, ফাঁকা);
			ফেরত MOSQ_ERR_PROTOCOL;
		}
		context->protocol = mosq_p_mqtt31;
		যদি((tmp_protocol_version&0x80) == 0x80){
			context->is_bridge = true;
		}
	}অন্যথা যদি(!strcmp(protocol_name, PROTOCOL_NAME)){
		যদি((tmp_protocol_version&0x7F) == PROTOCOL_VERSION_v311){
			context->protocol = mosq_p_mqtt311;

			যদি((tmp_protocol_version&0x80) == 0x80){
				context->is_bridge = true;
			}
		}অন্যথা যদি((tmp_protocol_version&0x7F) == PROTOCOL_VERSION_v5){
			context->protocol = mosq_p_mqtt5;
		}অন্যথা{
			যদি(db.config->connection_messages == true){
				log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->address, context->remote_port, tmp_protocol_version);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, ফাঁকা);
			ফেরত MOSQ_ERR_PROTOCOL;
		}
		যদি((context->in_packet.command&0x0F) != 0x00){
			/* Reserved flags not set to 0, must disconnect. */
			log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero reserved flags (%02X).",
					context->address, context->remote_port, context->in_packet.command);
			ফেরত MOSQ_ERR_PROTOCOL;
		}
	}অন্যথা{
		যদি(db.config->connection_messages == true){
			log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol \"%s\".",
					context->address, context->remote_port, protocol_name);
		}
		ফেরত MOSQ_ERR_PROTOCOL;
	}
	যদি((tmp_protocol_version&0x7F) != PROTOCOL_VERSION_v31 && context->in_packet.command != CMD_CONNECT){
		ফেরত MOSQ_ERR_MALFORMED_PACKET;
	}

	*protocol_version = tmp_protocol_version;
	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা read_and_verify_connect_flags(কাঠামো mosquitto *context, uint8_t *connect_flags)
{
	যদি(packet__read_byte(&context->in_packet, connect_flags)){
		log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing connect flags.",
				context->address, context->remote_port);
		ফেরত MOSQ_ERR_PROTOCOL;
	}
	যদি(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		যদি((*connect_flags & 0x01) != 0x00){
			log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero connect reserved flag (%02X).",
					context->address, context->remote_port, *connect_flags);
			ফেরত MOSQ_ERR_PROTOCOL;
		}
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির শূন্য set_session_expiry_interval(কাঠামো mosquitto *context, uint8_t clean_start, uint8_t protocol_version)
{
	/* session_expiry_interval will be overridden if the properties are read later */
	যদি(clean_start == false && protocol_version != PROTOCOL_VERSION_v5){
		/* v3* has clean_start == false mean the session never expires */
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
	}অন্যথা{
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	}
}


স্থির পূর্ণসংখ্যা read_and_reset_keepalive(কাঠামো mosquitto *context)
{
	/* _remove here because net__socket_accept() uses _add and we must have the
	 * correct keepalive value */
	keepalive__remove(context);

	যদি(packet__read_uint16(&context->in_packet, &(context->keepalive))){
		log__printf(ফাঁকা, MOSQ_LOG_INFO, "%s sent CONNECT with missing keepalive.",
				context->id);
		ফেরত MOSQ_ERR_PROTOCOL;
	}
	keepalive__add(context);

	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা read_and_verify_v5_connect_properties(কাঠামো mosquitto *context, mosquitto_property **properties, uint8_t protocol_version)
{
	পূর্ণসংখ্যা rc;

	যদি(protocol_version == PROTOCOL_VERSION_v5){
		rc = property__read_all(CMD_CONNECT, &context->in_packet, properties);
		যদি(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
			send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, ফাঁকা);
		}অন্যথা যদি(rc == MOSQ_ERR_MALFORMED_PACKET){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, ফাঁকা);
		}
		যদি(rc){
			ফেরত rc;
		}
	}
	rc = property__process_connect(context, properties);
	যদি(rc != MOSQ_ERR_SUCCESS){
		ফেরত send__connack_error_and_return(context, MQTT_RC_PROTOCOL_ERROR, rc);
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা verify_will_options(কাঠামো mosquitto *context, uint8_t will, uint8_t will_qos, uint8_t will_retain, uint8_t protocol_version)
{
	যদি(will_qos == 3){
		log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid Will QoS (%d).",
				context->address, context->remote_port, will_qos);
		ফেরত MOSQ_ERR_PROTOCOL;
	}

	যদি(will && will_retain && db.config->retain_available == false){
		যদি(protocol_version == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_RETAIN_NOT_SUPPORTED, ফাঁকা);
		}
		ফেরত MOSQ_ERR_NOT_SUPPORTED;
	}

	যদি(will && will_qos > context->listener->max_qos){
		যদি(protocol_version == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_QOS_NOT_SUPPORTED, ফাঁকা);
		}
		ফেরত MOSQ_ERR_NOT_SUPPORTED;
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা handle_zero_দৈর্ঘ্য_clientid(কাঠামো mosquitto *context, অক্ষর **clientid, bool *allow_zero_দৈর্ঘ্য_clientid,
		uint8_t clean_start)
{
	যদি(context->protocol == mosq_p_mqtt31){
		log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s:%d: v3.1 CONNECT with zero length clientid.",
				context->address, context->remote_port);
		send__connack(context, 0, CONNACK_REFUSED_IDENTIFIER_REJECTED, ফাঁকা);
		ফেরত MOSQ_ERR_PROTOCOL;
	}

	/* mqtt311/mqtt5 */
	mosquitto_FREE(*clientid);

	যদি(db.config->per_listener_settings){
		*allow_zero_দৈর্ঘ্য_clientid = context->listener->security_options->allow_zero_দৈর্ঘ্য_clientid;
	}অন্যথা{
		*allow_zero_দৈর্ঘ্য_clientid = db.config->security_options.allow_zero_দৈর্ঘ্য_clientid;
	}

	যদি((context->protocol == mosq_p_mqtt311 && clean_start == 0) || *allow_zero_দৈর্ঘ্য_clientid == false){
		log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with zero length clientid when forbidden.",
				context->address, context->remote_port);
		uint8_t err_code = context->protocol == mosq_p_mqtt311 ? (uint8_t)CONNACK_REFUSED_IDENTIFIER_REJECTED : (uint8_t)MQTT_RC_UNSPECIFIED;
		ফেরত send__connack_error_and_return(context, err_code, MOSQ_ERR_PROTOCOL);
	}

	*clientid = clientid_gen(&(uint16_t){0}, context->listener->security_options->auto_id_prefix,
			context->listener->security_options->auto_id_prefix_len);
	যদি(*clientid == ফাঁকা){
		ফেরত MOSQ_ERR_NOMEM;
	}
	context->assigned_id = true;

	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা check_clientid_prefixes(কাঠামো mosquitto *context, ধ্রুবক অক্ষর *clientid)
{
	যদি(db.config->clientid_prefixes){
		যদি(strncmp(db.config->clientid_prefixes, clientid, strlen(db.config->clientid_prefixes))){
			uint8_t err_code = context->protocol == mosq_p_mqtt5 ? (uint8_t)MQTT_RC_NOT_AUTHORIZED : (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
			ফেরত send__connack_error_and_return(context, err_code, MOSQ_ERR_AUTH);
		}
	}
	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা read_and_verify_clientid_from_packet(কাঠামো mosquitto *context, অক্ষর **clientid,
		bool *allow_zero_দৈর্ঘ্য_clientid, uint8_t clean_start)
{
	পূর্ণসংখ্যা rc;
	uint16_t slen;

	rc = packet__read_string(&context->in_packet, clientid, &slen);
	যদি(rc == MOSQ_ERR_MALFORMED_UTF8){
		যদি(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_CLIENTID_NOT_VALID, ফাঁকা);
		}
		ফেরত MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID;
	}অন্যথা যদি(rc > 0){
		ফেরত MOSQ_ERR_PROTOCOL;
	}

	যদি(slen == 0){
		rc = handle_zero_দৈর্ঘ্য_clientid(context, clientid, allow_zero_দৈর্ঘ্য_clientid, clean_start);
		যদি(rc != MOSQ_ERR_SUCCESS){
			ফেরত rc;
		}
	}

	rc = check_clientid_prefixes(context, *clientid);
	যদি(rc != MOSQ_ERR_SUCCESS){
		ফেরত rc;
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা set_username_from_packet(কাঠামো mosquitto *context, অক্ষর **username, ধ্রুবক অক্ষর *clientid)
{
	পূর্ণসংখ্যা rc;

	rc = packet__read_string(&context->in_packet, username, &(uint16_t){0});
	যদি(rc == MOSQ_ERR_NOMEM){
		ফেরত MOSQ_ERR_NOMEM;
	}
	যদি(rc != MOSQ_ERR_SUCCESS){
		যদি(context->protocol == mosq_p_mqtt31){
			/* Username flag given, but no username. Ignore. */
			/* NOTE: Removed setting of username_flag to zero as it is unused afterwards */
		}অন্যথা{
			log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with username flag but no username.",
					clientid);
			ফেরত MOSQ_ERR_PROTOCOL;
		}
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা set_password_from_packet(কাঠামো mosquitto *context, অক্ষর **password, uint16_t *password_len, ধ্রুবক অক্ষর *clientid)
{
	পূর্ণসংখ্যা rc;

	rc = packet_read_দ্বিমিক(&context->in_packet, (uint8_t **)password, password_len);
	যদি(rc == MOSQ_ERR_NOMEM){
		ফেরত MOSQ_ERR_NOMEM;
	}

	যদি(rc == MOSQ_ERR_MALFORMED_PACKET){
		যদি(context->protocol == mosq_p_mqtt31){
			/* Password flag given, but no password. Ignore. */
		}অন্যথা{
			log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with password flag but no password.",
					clientid);
			ফেরত MOSQ_ERR_PROTOCOL;
		}
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা read_and_verify_client_credentials_from_packet(কাঠামো mosquitto *context,
		অক্ষর **username, uint8_t username_flag,
		অক্ষর **password, uint16_t *password_len, uint8_t password_flag,
		ধ্রুবক অক্ষর *clientid)
{
	পূর্ণসংখ্যা rc;

	যদি(username_flag){
		rc = set_username_from_packet(context, username, clientid);
		যদি(rc != MOSQ_ERR_SUCCESS){
			ফেরত rc;
		}
	}অন্যথা{
		যদি(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
			যদি(password_flag){
				/* username_flag == 0 && password_flag == 1 is forbidden */
				log__printf(ফাঁকা, MOSQ_LOG_ERR, "Protocol error from %s: password without username, closing connection.", clientid);
				ফেরত MOSQ_ERR_PROTOCOL;
			}
		}
	}
	যদি(password_flag){
		rc = set_password_from_packet(context, password, password_len, clientid);
		যদি(rc != MOSQ_ERR_SUCCESS){
			ফেরত rc;
		}
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা check_additional_trailing_তথ্য(কাঠামো mosquitto *context, uint8_t protocol_version)
{
	যদি(context->in_packet.pos != context->in_packet.remaining_দৈর্ঘ্য){
		/* Surplus data at end of packet, this must be an error. */
		log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->in_packet.pos, context->in_packet.remaining_দৈর্ঘ্য);
		যদি(protocol_version == PROTOCOL_VERSION_v5){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, ফাঁকা);
		}
		ফেরত MOSQ_ERR_PROTOCOL;
	}

	ফেরত MOSQ_ERR_SUCCESS;
}

#ifdef WITH_TLS


অন্তঃরেখ স্থির পূর্ণসংখ্যা get_client_cert_and_subject_name(কাঠামো mosquitto *context, X509 **client_cert, X509_NAME **name)
{
	*client_cert = SSL_get_peer_certificate(context->ssl);
	যদি(*client_cert == ফাঁকা){
		ফেরত send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	*name = X509_get_subject_name(*client_cert);
	যদি(*name == ফাঁকা){
		X509_free(*client_cert);
		ফেরত send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


অন্তঃরেখ স্থির পূর্ণসংখ্যা free_x509_and_send_connack_error(কাঠামো mosquitto *context, X509 *client_cert, পূর্ণসংখ্যা rc)
{
	X509_free(client_cert);
	ফেরত send__connack_bad_username_or_password_error(context, rc);
}


অন্তঃরেখ স্থির পূর্ণসংখ্যা free_x509_and_BIO_and_send_connack_error(কাঠামো mosquitto *context, X509 *client_cert,
		BIO *subject_name, পূর্ণসংখ্যা rc)
{
	BIO_free(subject_name);
	ফেরত free_x509_and_send_connack_error(context, client_cert, rc);
}


স্থির পূর্ণসংখ্যা set_username_from_cert_identity(কাঠামো mosquitto *context)
{
	X509 *client_cert = ফাঁকা;
	X509_NAME *name = ফাঁকা;

	যদি(get_client_cert_and_subject_name(context, &client_cert, &name)){
		ফেরত MOSQ_ERR_AUTH;
	}

	পূর্ণসংখ্যা i = -1;
	X509_NAME_ENTRY *name_entry = ফাঁকা;

	i = X509_NAME_get_সূচক_by_NID(name, NID_commonName, -1);
	যদি(i == -1){
		ফেরত free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	name_entry = X509_NAME_get_entry(name, i);
	যদি(!name_entry){
		যাও success;
	}

	ASN1_STRING *name_asn1 = ফাঁকা;
	name_asn1 = X509_NAME_ENTRY_get_তথ্য(name_entry);
	যদি(name_asn1 == ফাঁকা){
		ফেরত free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	ধ্রুবক অক্ষর *cert_identity = (ধ্রুবক অক্ষর *)ASN1_STRING_get0_তথ্য(name_asn1);
	যদি(!cert_identity || mosquitto_validate_utf8(cert_identity, (পূর্ণসংখ্যা)strlen(cert_identity))){
		ফেরত free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	mosquitto_free(context->username);
	context->username = mosquitto_strdup(cert_identity);
	যদি(context->username == ফাঁকা){
		ফেরত free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_NOMEM);
	}
	/* Make sure there isn't an embedded NUL character in the CN */
	যদি((size_t)ASN1_STRING_দৈর্ঘ্য(name_asn1) != strlen(context->username)){
		ফেরত free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}

success:
	X509_free(client_cert);
	client_cert = ফাঁকা;
	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা set_username_from_cert_subject_name(কাঠামো mosquitto *context)
{
	X509 *client_cert = ফাঁকা;
	X509_NAME *name = ফাঁকা;

	যদি(get_client_cert_and_subject_name(context, &client_cert, &name)){
		ফেরত MOSQ_ERR_AUTH;
	}

	অক্ষর *subject = ফাঁকা;
	অক্ষর *তথ্য_start = ফাঁকা;
	BIO *subject_bio = ফাঁকা;
	long name_দৈর্ঘ্য = 0;

	subject_bio = BIO_new(BIO_s_mem());
	X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
	তথ্য_start = ফাঁকা;
	name_দৈর্ঘ্য = BIO_get_mem_তথ্য(subject_bio, &তথ্য_start);
	subject = mosquitto_malloc(আকার(অক্ষর)*(size_t)(name_দৈর্ঘ্য+1));
	যদি(!subject){
		ফেরত free_x509_and_BIO_and_send_connack_error(context, client_cert, subject_bio, MOSQ_ERR_NOMEM);
	}
	memcpy(subject, তথ্য_start, (size_t)name_দৈর্ঘ্য);
	subject[name_দৈর্ঘ্য] = '\0';
	BIO_free(subject_bio);

	যদি(mosquitto_validate_utf8(subject, (পূর্ণসংখ্যা)strlen(subject))){
		mosquitto_free(subject);
		ফেরত free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	context->username = subject;
	যদি(!context->username){
		X509_free(client_cert);
		ফেরত MOSQ_ERR_AUTH;
	}

	X509_free(client_cert);
	client_cert = ফাঁকা;
	ফেরত MOSQ_ERR_SUCCESS;
}
#endif


স্থির পূর্ণসংখ্যা handle_username_from_cert_options(কাঠামো mosquitto *context, অক্ষর **username, অক্ষর **password, uint16_t password_len)
{
	পূর্ণসংখ্যা rc;

#ifdef WITH_TLS
	যদি(context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
		/* Don't need the username or password if provided */
		mosquitto_FREE(*username);
		mosquitto_FREE(*password);

		যদি(!context->ssl){
			ফেরত send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
		}
#ifdef FINAL_WITH_TLS_PSK
		যদি(context->listener->psk_hint){
			/* Client should have provided an identity to get this far. */
			যদি(!context->username){
				ফেরত send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}অন্যথা
#endif /* FINAL_WITH_TLS_PSK */
		{
			যদি(context->listener->use_identity_as_username){
				rc = set_username_from_cert_identity(context);
			}অন্যথা{   /* use_subject_as_username */
				rc = set_username_from_cert_subject_name(context);
			}
			যদি(rc){
				ফেরত rc;
			}
		}
	}অন্যথা
#endif /* WITH_TLS */
	{
#ifdef WITH_TLS
		যদি(context->listener->use_identity_as_username && context->listener->require_certificate){
			mosquitto_FREE(*username);
			mosquitto_FREE(*password);

			যদি(!context->username){
				ফেরত send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}অন্যথা
#endif
		{
			/* FIXME - these ensure the mosquitto_clientid() and
			* mosquitto_client_username() functions work, but is hacky */
			context->username = *username;
			context->password = *password;
			context->password_len = password_len;
			*username = ফাঁকা; /* Avoid free() in error: below. */
			*password = ফাঁকা;
		}
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা handle_username_as_clientid_option(কাঠামো mosquitto *context)
{
	যদি(context->username){
		mosquitto_FREE(context->id);
		context->id = mosquitto_strdup(context->username);
		যদি(!context->id){
			ফেরত MOSQ_ERR_NOMEM;
		}
	}অন্যথা{
		uint8_t err_code = context->protocol == mosq_p_mqtt5
												? (uint8_t)MQTT_RC_NOT_AUTHORIZED
												: (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
		ফেরত send__connack_error_and_return(context, err_code, MOSQ_ERR_AUTH);
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা handle__connect(কাঠামো mosquitto *context)
{
	অক্ষর protocol_name[7];
	uint8_t protocol_version;
	uint8_t connect_flags;
	অক্ষর *clientid = ফাঁকা;
	কাঠামো mosquitto *found_context;
	কাঠামো mosquitto_message_all *will_struct = ফাঁকা;
	uint8_t will, will_retain, will_qos, clean_start;
	uint8_t username_flag, password_flag;
	অক্ষর *username = ফাঁকা, *password = ফাঁকা;
	uint16_t password_len = 0;
	পূর্ণসংখ্যা rc;
	mosquitto_property *properties = ফাঁকা;
	শূন্য *auth_তথ্য = ফাঁকা;
	uint16_t auth_তথ্য_len = 0;
	শূন্য *auth_তথ্য_out = ফাঁকা;
	uint16_t auth_তথ্য_out_len = 0;
	bool allow_zero_দৈর্ঘ্য_clientid;

	যদি(!context->listener){
		ফেরত MOSQ_ERR_INVAL;
	}

	/* Don't accept multiple CONNECT commands. */
	যদি(context->state != mosq_cs_new){
		log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Bad client %s:%d sending multiple CONNECT messages.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		যাও handle_connect_error;
	}

#ifdef WITH_TLS
	যদি(context->in_packet.command == 0x16 && context->listener->ssl_ctx == ফাঁকা){ /* 0x16 is TLS handshake client hello */
		log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client from %s:%d appears to be using TLS to connect to a non-TLS listener.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		যাও handle_connect_error;
	}
#endif

	rc = read_protocol_name(context, protocol_name);
	যদি(rc != MOSQ_ERR_SUCCESS){
		যাও handle_connect_error;
	}

	rc = read_and_verify_protocol_version(context, protocol_name, &protocol_version);
	যদি(rc != MOSQ_ERR_SUCCESS){
		যদি(rc == MOSQ_ERR_MALFORMED_PACKET){
			ফেরত rc;
		}
		যাও handle_connect_error;
	}

	rc = read_and_verify_connect_flags(context, &connect_flags);
	যদি(rc != MOSQ_ERR_SUCCESS){
		যাও handle_connect_error;
	}

	clean_start = (connect_flags & 0x02) >> 1;
	set_session_expiry_interval(context, clean_start, protocol_version);

	rc = read_and_reset_keepalive(context);
	যদি(rc != MOSQ_ERR_SUCCESS){
		যাও handle_connect_error;
	}

	rc = read_and_verify_v5_connect_properties(context, &properties, protocol_version);
	যদি(rc != MOSQ_ERR_SUCCESS){
		যাও handle_connect_error;
	}

	will = connect_flags & 0x04;
	will_qos = (connect_flags & 0x18) >> 3;
	will_retain = ((connect_flags & 0x20) == 0x20);
	rc = verify_will_options(context, will, will_qos, will_retain, protocol_version);
	যদি(rc != MOSQ_ERR_SUCCESS){
		যাও handle_connect_error;
	}

	mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_METHOD, &context->auth_method, false);
	mosquitto_property_read_দ্বিমিক(properties, MQTT_PROP_AUTHENTICATION_তথ্য, &auth_তথ্য, &auth_তথ্য_len, false);
	mosquitto_property_free_all(&properties);

	যদি(auth_তথ্য && !context->auth_method){
		send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, ফাঁকা);
		log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s:%d CONNECT with missing clientid string.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		যাও handle_connect_error;
	}

	rc = read_and_verify_clientid_from_packet(context, &clientid, &allow_zero_দৈর্ঘ্য_clientid, clean_start);
	যদি(rc != MOSQ_ERR_SUCCESS){
		যাও handle_connect_error;
	}

	যদি(will){
		rc = will__read(context, clientid, &will_struct, will_qos, will_retain);
		যদি(rc){
			যদি(context->protocol == mosq_p_mqtt5){
				যদি(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
					send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, ফাঁকা);
				}অন্যথা যদি(rc == MOSQ_ERR_MALFORMED_PACKET){
					send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, ফাঁকা);
				}
			}
			যাও handle_connect_error;
		}
	}অন্যথা{
		যদি(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
			যদি(will_qos != 0 || will_retain != 0){
				log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT without Will with non-zero QoS (%d) or retain (%d).",
						clientid, will_qos, will_retain);
				rc = MOSQ_ERR_PROTOCOL;
				যাও handle_connect_error;
			}
		}
	}

	// Client credentials
	password_flag = connect_flags & 0x40;
	username_flag = connect_flags & 0x80;
	rc = read_and_verify_client_credentials_from_packet(context, &username, username_flag, &password, &password_len, password_flag, clientid);
	যদি(rc != MOSQ_ERR_SUCCESS){
		যাও handle_connect_error;
	}

	rc = check_additional_trailing_তথ্য(context, protocol_version);
	যদি(rc != MOSQ_ERR_SUCCESS){
		যাও handle_connect_error;
	}

	/* Once context->id is set, if we return from this function with an error
	 * we must make sure that context->id is freed and set to NULL, so that the
	 * client isn't erroneously removed from the by_id hash table. */
	context->id = clientid;
	clientid = ফাঁকা;

	/* use_identity_as_username or use_subject_as_username */
	rc = handle_username_from_cert_options(context, &username, &password, password_len);
	যদি(rc != MOSQ_ERR_SUCCESS){
		যাও handle_connect_error;
	}

	/* use_username_as_clientid */
	যদি(context->listener->use_username_as_clientid){
		rc = handle_username_as_clientid_option(context);
		যদি(rc != MOSQ_ERR_SUCCESS){
			যাও handle_connect_error;
		}
	}

	/* Check for an existing delayed auth check, reject if present */
	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), found_context);
	যদি(found_context){
		rc = MOSQ_ERR_UNKNOWN;
		যাও handle_connect_error;
	}

	context->clean_start = clean_start;
	context->will = will_struct;
	will_struct = ফাঁকা;

	যদি(context->auth_method){
		rc = mosquitto_security_auth_start(context, false, auth_তথ্য, auth_তথ্য_len, &auth_তথ্য_out, &auth_তথ্য_out_len);
		mosquitto_FREE(auth_তথ্য);
		যদি(rc == MOSQ_ERR_SUCCESS){
			ফেরত connect__on_authorised(context, auth_তথ্য_out, auth_তথ্য_out_len);
		}অন্যথা যদি(rc == MOSQ_ERR_AUTH_CONTINUE){
			mosquitto__set_state(context, mosq_cs_authenticating);
			rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_তথ্য_out, auth_তথ্য_out_len);
			SAFE_FREE(auth_তথ্য_out);
			ফেরত rc;
		}অন্যথা{
			SAFE_FREE(auth_তথ্য_out);
			will__clear(context);
			যদি(rc == MOSQ_ERR_AUTH){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ফাঁকা);
				mosquitto_FREE(context->id);
				যাও handle_connect_error;
			}অন্যথা যদি(rc == MOSQ_ERR_NOT_SUPPORTED){
				/* Client has requested extended authentication, but we don't support it. */
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_METHOD, ফাঁকা);
				mosquitto_FREE(context->id);
				যাও handle_connect_error;
			}অন্যথা{
				mosquitto_FREE(context->id);
				যাও handle_connect_error;
			}
		}
	}অন্যথা{
#ifdef WITH_TLS
		যদি(context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
			/* Authentication assumed to be cleared */
		}অন্যথা
#endif
		{
			rc = mosquitto_basic_auth(context);
			নির্বাচন(rc){
				ক্ষেত্র MOSQ_ERR_SUCCESS:
					বিরতি;
				ক্ষেত্র MOSQ_ERR_AUTH_DELAYED:
					mosquitto__set_state(context, mosq_cs_delayed_auth);
					HASH_ADD_KEYPTR(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context);
					ফেরত MOSQ_ERR_SUCCESS;
					বিরতি;
				ক্ষেত্র MOSQ_ERR_AUTH:
					যদি(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ফাঁকা);
					}অন্যথা{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ফাঁকা);
					}
					যাও handle_connect_error;
					বিরতি;
				ক্ষেত্র MOSQ_ERR_UNSPECIFIED:
				ক্ষেত্র MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
				ক্ষেত্র MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
				ক্ষেত্র MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
				ক্ষেত্র MOSQ_ERR_SERVER_UNAVAILABLE:
				ক্ষেত্র MOSQ_ERR_SERVER_BUSY:
				ক্ষেত্র MOSQ_ERR_BANNED:
				ক্ষেত্র MOSQ_ERR_BAD_AUTHENTICATION_METHOD:
				ক্ষেত্র MOSQ_ERR_CONNECTION_RATE_EXCEEDED:
					যদি(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, (uint8_t)rc, ফাঁকা);
					}অন্যথা{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ফাঁকা);
					}
					যাও handle_connect_error;
					বিরতি;
				পূর্বনির্ধারিত:
					rc = MOSQ_ERR_UNKNOWN;
					যাও handle_connect_error;
					বিরতি;
			}
		}
		ফেরত connect__on_authorised(context, ফাঁকা, 0);
	}


handle_connect_error:
	mosquitto_property_free_all(&properties);
	mosquitto_FREE(auth_তথ্য);
	mosquitto_FREE(clientid);
	mosquitto_FREE(username);
	mosquitto_FREE(password);
	যদি(will_struct){
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct->বার্তা.payload);
		mosquitto_FREE(will_struct->বার্তা.topic);
		mosquitto_FREE(will_struct);
	}
	will__clear(context);
	/* We return an error here which means the client is freed later on. */
	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->will_delay_interval = 0;
	ফেরত rc;
}
