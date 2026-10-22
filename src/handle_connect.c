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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <utlist.h>

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/mqtt_protocol.h"
#അന്തര്ഭാവ "packet_mosq.h"
#അന്തര്ഭാവ "property_mosq.h"
#അന്തര്ഭാവ "send_mosq.h"
#അന്തര്ഭാവ "sys_tree.h"
#അന്തര്ഭാവ "tls_mosq.h"
#അന്തര്ഭാവ "util_mosq.h"
#അന്തര്ഭാവ "will_mosq.h"

#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif


സ്ഥിതം അക്ഷരം nibble_to_hex(uint8_t മൂല്യം)
{
	എങ്കിൽ(മൂല്യം < 0x0A){
		തിരികെ (അക്ഷരം)('0'+മൂല്യം);
	}അല്ലെങ്കിൽ{
		തിരികെ (അക്ഷരം)(65 /*'A'*/ +മൂല്യം-10);
	}
}


സ്ഥിതം അക്ഷരം *clientid_gen(uint16_t *idlen, സ്ഥിരം അക്ഷരം *auto_id_prefix, uint16_t auto_id_prefix_len)
{
	അക്ഷരം *clientid;
	uint8_t rnd[16];
	പൂർണ്ണസംഖ്യ pos;

	എങ്കിൽ(mosquitto_getrandom(rnd, 16)){
		തിരികെ ശൂന്യ;
	}

	*idlen = (uint16_t)(auto_id_prefix_len + 36);

	clientid = (അക്ഷരം *)mosquitto_calloc((size_t)(*idlen) + 1, വലുപ്പം(അക്ഷരം));
	എങ്കിൽ(!clientid){
		തിരികെ ശൂന്യ;
	}
	എങ്കിൽ(auto_id_prefix){
		memcpy(clientid, auto_id_prefix, auto_id_prefix_len);
	}

	pos = 0;
	വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<16; i++){
		clientid[auto_id_prefix_len + pos + 0] = nibble_to_hex(rnd[i] & 0x0F);
		clientid[auto_id_prefix_len + pos + 1] = nibble_to_hex((rnd[i] >> 4) & 0x0F);
		pos += 2;
		എങ്കിൽ(pos == 8 || pos == 13 || pos == 18 || pos == 23){
			clientid[auto_id_prefix_len + pos] = '-';
			pos++;
		}
	}

	തിരികെ clientid;
}


പൂർണ്ണസംഖ്യ connect__on_authorised(ഘടന mosquitto *context, ശൂന്യം *auth_data_out, uint16_t auth_data_out_len)
{
	ഘടന mosquitto *found_context;
	ഘടന mosquitto__subleaf *leaf;
	mosquitto_property *connack_props = ശൂന്യ;
	uint8_t connect_ack = 0;
	പൂർണ്ണസംഖ്യ rc;
	പൂർണ്ണസംഖ്യ in_quota, out_quota;
	uint16_t in_maximum, out_maximum;

	/* Find if this client already has an entry. This must be done *after* any security checks. */
	HASH_FIND(hh_id, db.contexts_by_id, context->id, strlen(context->id), found_context);
	എങ്കിൽ(found_context){
		/* Found a matching client */
		എങ്കിൽ(!net__is_connected(found_context)){
			/* Client is reconnecting after a disconnect */
			/* FIXME - does anything need to be done here? */
		}അല്ലെങ്കിൽ{
			/* Client is already connected, disconnect old version. This is
			 * done in context__cleanup() below. */
		}

		എങ്കിൽ(context->clean_start == true){
			sub__clean_session(found_context);
			found_context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			plugin_persist__handle_client_delete(found_context);
		}
		context->is_persisted = found_context->is_persisted;
		found_context->is_persisted = false; /* stops persistence for context being removed */

		എങ്കിൽ(context->clean_start == false && found_context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
			എങ്കിൽ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
				connect_ack |= 0x01;
			}

			എങ്കിൽ(found_context->msgs_in.inflight || found_context->msgs_in.queued
					|| found_context->msgs_out.inflight || found_context->msgs_out.queued){

				in_quota = context->msgs_in.inflight_quota;
				out_quota = context->msgs_out.inflight_quota;
				in_maximum = context->msgs_in.inflight_maximum;
				out_maximum = context->msgs_out.inflight_maximum;

				memcpy(&context->msgs_in, &found_context->msgs_in, വലുപ്പം(ഘടന mosquitto_സന്ദേശം_data));
				memcpy(&context->msgs_out, &found_context->msgs_out, വലുപ്പം(ഘടന mosquitto_സന്ദേശം_data));
				context->last_cmsg_id = found_context->last_cmsg_id;

				memset(&found_context->msgs_in, 0, വലുപ്പം(ഘടന mosquitto_സന്ദേശം_data));
				memset(&found_context->msgs_out, 0, വലുപ്പം(ഘടന mosquitto_സന്ദേശം_data));

				context->msgs_in.inflight_quota = in_quota;
				context->msgs_out.inflight_quota = out_quota;
				context->msgs_in.inflight_maximum = in_maximum;
				context->msgs_out.inflight_maximum = out_maximum;

				db__message_reconnect_reset(context);
			}
			context->subs = found_context->subs;
			found_context->subs = ശൂന്യ;
			context->subs_capacity = found_context->subs_capacity;
			context->subs_എണ്ണം = found_context->subs_എണ്ണം;
			found_context->subs_capacity = 0;
			found_context->subs_എണ്ണം = 0;
			context->last_mid = found_context->last_mid;

			വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<context->subs_capacity; i++){
				എങ്കിൽ(context->subs[i]){
					leaf = context->subs[i]->hier->subs;
					ആകുമ്പോൾ(leaf){
						എങ്കിൽ(leaf->context == found_context){
							leaf->context = context;
						}
						leaf = leaf->next;
					}

					എങ്കിൽ(context->subs[i]->shared){
						leaf = context->subs[i]->shared->subs;
						ആകുമ്പോൾ(leaf){
							എങ്കിൽ(leaf->context == found_context){
								leaf->context = context;
							}
							leaf = leaf->next;
						}
					}
				}
			}
		}

		എങ്കിൽ((found_context->protocol == mosq_p_mqtt5 && found_context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE)
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
		എങ്കിൽ(found_context->protocol == mosq_p_mqtt5){
			send__disconnect(found_context, MQTT_RC_SESSION_TAKEN_OVER, ശൂന്യ);
		}
		do_disconnect(found_context, MOSQ_ERR_SESSION_TAKEN_OVER);
	}

	എങ്കിൽ(db.config->global_max_clients > 0 && HASH_CNT(hh_id, db.contexts_by_id) >= (unsigned പൂർണ്ണസംഖ്യ)db.config->global_max_clients){
		എങ്കിൽ(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_SERVER_BUSY, ശൂന്യ);
		}
		rc = MOSQ_ERR_INVAL;
		പോകുക error;
	}

	എങ്കിൽ(db.config->connection_messages == true){
		എങ്കിൽ(context->is_bridge){
			എങ്കിൽ(context->username){
				log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive, context->username);
			}അല്ലെങ്കിൽ{
				log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d).",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive);
			}
		}അല്ലെങ്കിൽ{
			എങ്കിൽ(context->username){
				log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive, context->username);
			}അല്ലെങ്കിൽ{
				log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d).",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive);
			}
		}

		എങ്കിൽ(context->will){
			log__printf(ശൂന്യ, MOSQ_LOG_DEBUG, "Will message specified (%ld bytes) (r%d, q%d).",
					(long)context->will->സന്ദേശം.payloadlen,
					context->will->സന്ദേശം.retain,
					context->will->സന്ദേശം.qos);

			log__printf(ശൂന്യ, MOSQ_LOG_DEBUG, "\t%s", context->will->സന്ദേശം.topic);
		}അല്ലെങ്കിൽ{
			log__printf(ശൂന്യ, MOSQ_LOG_DEBUG, "No will message specified.");
		}
	}
#ifdef WITH_TLS
	എങ്കിൽ(context->ssl){
		log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s negotiated %s cipher %s",
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
	എങ്കിൽ(!context->clean_start){
		db.persistence_changes++;
	}
#endif
	context->max_qos = context->listener->max_qos;

	എങ്കിൽ(db.config->max_keepalive &&
			(context->keepalive > db.config->max_keepalive || context->keepalive == 0)){

		keepalive__remove(context);
		context->keepalive = db.config->max_keepalive;
		keepalive__add(context);
		എങ്കിൽ(context->protocol == mosq_p_mqtt5){
			എങ്കിൽ(mosquitto_property_add_int16(&connack_props, MQTT_PROP_SERVER_KEEP_ALIVE, context->keepalive)){
				rc = MOSQ_ERR_NOMEM;
				പോകുക error;
			}
		}അല്ലെങ്കിൽ{
			send__connack(context, connect_ack, CONNACK_REFUSED_IDENTIFIER_REJECTED, ശൂന്യ);
			rc = MOSQ_ERR_INVAL;
			പോകുക error;
		}
	}


	എങ്കിൽ(context->protocol == mosq_p_mqtt5){
		എങ്കിൽ(context->listener->max_topic_alias > 0){
			എങ്കിൽ(mosquitto_property_add_int16(&connack_props, MQTT_PROP_TOPIC_ALIAS_MAXIMUM, context->listener->max_topic_alias)){
				rc = MOSQ_ERR_NOMEM;
				പോകുക error;
			}
		}
		എങ്കിൽ(context->assigned_id){
			എങ്കിൽ(mosquitto_property_add_string(&connack_props, MQTT_PROP_ASSIGNED_CLIENT_IDENTIFIER, context->id)){
				rc = MOSQ_ERR_NOMEM;
				പോകുക error;
			}
		}
		എങ്കിൽ(context->auth_method){
			എങ്കിൽ(mosquitto_property_add_string(&connack_props, MQTT_PROP_AUTHENTICATION_METHOD, context->auth_method)){
				rc = MOSQ_ERR_NOMEM;
				പോകുക error;
			}

			എങ്കിൽ(auth_data_out && auth_data_out_len > 0){
				എങ്കിൽ(mosquitto_property_add_ദ്വിമാന(&connack_props, MQTT_PROP_AUTHENTICATION_DATA, auth_data_out, auth_data_out_len)){
					rc = MOSQ_ERR_NOMEM;
					പോകുക error;
				}
			}
		}
	}
	SAFE_FREE(auth_data_out);

	mosquitto__set_state(context, mosq_cs_active);
	rc = send__connack(context, connect_ack, CONNACK_ACCEPTED, connack_props);
	mosquitto_property_free_all(&connack_props);
	എങ്കിൽ(rc){
		തിരികെ rc;
	}
	db__expire_all_messages(context);
	rc = db__message_write_queued_out(context);
	എങ്കിൽ(rc){
		തിരികെ rc;
	}
	rc = db__message_write_inflight_out_all(context);

	എങ്കിൽ(rc == MOSQ_ERR_SUCCESS){
		plugin__handle_connect(context);

		എങ്കിൽ(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
			plugin_persist__handle_client_add(context);
		}അല്ലെങ്കിൽ എങ്കിൽ(context->will){
			plugin_persist__handle_will_add(context);
		}
	}
	തിരികെ rc;
error:
	SAFE_FREE(auth_data_out);
	mosquitto_property_free_all(&connack_props);
	തിരികെ rc;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ will__read(ഘടന mosquitto *context, സ്ഥിരം അക്ഷരം *clientid, ഘടന mosquitto_message_all **will, uint8_t will_qos, പൂർണ്ണസംഖ്യ will_retain)
{
	പൂർണ്ണസംഖ്യ rc = MOSQ_ERR_SUCCESS;
	size_t slen;
	uint16_t tlen;
	ഘടന mosquitto_message_all *will_struct = ശൂന്യ;
	അക്ഷരം *will_topic_mount = ശൂന്യ;
	uint16_t payloadlen;
	mosquitto_property *properties = ശൂന്യ;

	will_struct = mosquitto_calloc(1, വലുപ്പം(ഘടന mosquitto_message_all));
	എങ്കിൽ(!will_struct){
		rc = MOSQ_ERR_NOMEM;
		പോകുക error_cleanup;
	}
	എങ്കിൽ(context->protocol == PROTOCOL_VERSION_v5){
		rc = property__read_all(CMD_WILL, &context->in_packet, &properties);
		എങ്കിൽ(rc){
			പോകുക error_cleanup;
		}

		rc = property__process_will(context, will_struct, &properties);
		mosquitto_property_free_all(&properties);
		എങ്കിൽ(rc){
			പോകുക error_cleanup;
		}
	}
	rc = packet__read_string(&context->in_packet, &will_struct->സന്ദേശം.topic, &tlen);
	എങ്കിൽ(rc){
		പോകുക error_cleanup;
	}
	എങ്കിൽ(!tlen){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: Will with empty topic.",
				context->id);
		rc = MOSQ_ERR_PROTOCOL;
		പോകുക error_cleanup;
	}

	എങ്കിൽ(context->listener->mount_point){
		slen = strlen(context->listener->mount_point) + strlen(will_struct->സന്ദേശം.topic) + 1;
		will_topic_mount = mosquitto_malloc(slen+1);
		എങ്കിൽ(!will_topic_mount){
			rc = MOSQ_ERR_NOMEM;
			പോകുക error_cleanup;
		}

		snprintf(will_topic_mount, slen, "%s%s", context->listener->mount_point, will_struct->സന്ദേശം.topic);
		will_topic_mount[slen] = '\0';

		mosquitto_FREE(will_struct->സന്ദേശം.topic);
		will_struct->സന്ദേശം.topic = will_topic_mount;
	}

	എങ്കിൽ(!strncmp(will_struct->സന്ദേശം.topic, "$CONTROL/", strlen("$CONTROL/"))){
		rc = MOSQ_ERR_ACL_DENIED;
		പോകുക error_cleanup;
	}
	rc = mosquitto_pub_topic_check(will_struct->സന്ദേശം.topic);
	എങ്കിൽ(rc){
		പോകുക error_cleanup;
	}

	rc = packet__read_uint16(&context->in_packet, &payloadlen);
	എങ്കിൽ(rc){
		പോകുക error_cleanup;
	}

	will_struct->സന്ദേശം.payloadlen = payloadlen;
	എങ്കിൽ(will_struct->സന്ദേശം.payloadlen > 0){
		എങ്കിൽ(db.config->message_size_limit && will_struct->സന്ദേശം.payloadlen > (പൂർണ്ണസംഖ്യ)db.config->message_size_limit){
			log__printf(ശൂന്യ, MOSQ_LOG_DEBUG, "Client %s connected with too large Will payload", clientid);
			എങ്കിൽ(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_PACKET_TOO_LARGE, ശൂന്യ);
			}അല്ലെങ്കിൽ{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ശൂന്യ);
			}
			rc = MOSQ_ERR_PAYLOAD_SIZE;
			പോകുക error_cleanup;
		}
		will_struct->സന്ദേശം.payload = mosquitto_malloc((size_t)will_struct->സന്ദേശം.payloadlen);
		എങ്കിൽ(!will_struct->സന്ദേശം.payload){
			rc = MOSQ_ERR_NOMEM;
			പോകുക error_cleanup;
		}

		rc = packet__read_bytes(&context->in_packet, will_struct->സന്ദേശം.payload, (uint32_t)will_struct->സന്ദേശം.payloadlen);
		എങ്കിൽ(rc){
			പോകുക error_cleanup;
		}
	}

	will_struct->സന്ദേശം.qos = will_qos;
	will_struct->സന്ദേശം.retain = will_retain;

	*will = will_struct;
	തിരികെ MOSQ_ERR_SUCCESS;

error_cleanup:
	എങ്കിൽ(will_struct){
		mosquitto_FREE(will_struct->സന്ദേശം.topic);
		mosquitto_FREE(will_struct->സന്ദേശം.payload);
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct);
	}
	തിരികെ rc;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ check_protocol_version(ഘടന mosquitto__listener *listener, പൂർണ്ണസംഖ്യ protocol_version)
{
	/* Allow bridge protocol as well. */
	protocol_version &= 0x7F;

	എങ്കിൽ((protocol_version == 3 && listener->disable_protocol_v3 == false)
			|| (protocol_version == 4 && listener->disable_protocol_v4 == false)
			|| (protocol_version == 5 && listener->disable_protocol_v5 == false)
			){

		തിരികെ MOSQ_ERR_SUCCESS;
	}അല്ലെങ്കിൽ{
		തിരികെ MOSQ_ERR_NOT_SUPPORTED;
	}
}


വരിയിൽ സ്ഥിതം പൂർണ്ണസംഖ്യ send__connack_error_and_return(ഘടന mosquitto *context, uint8_t err_code, പൂർണ്ണസംഖ്യ rc)
{
	send__connack(context, 0, err_code, ശൂന്യ);
	തിരികെ rc;
}


വരിയിൽ സ്ഥിതം പൂർണ്ണസംഖ്യ send__connack_bad_username_or_password_error(ഘടന mosquitto *context, പൂർണ്ണസംഖ്യ rc)
{
	uint8_t err_code  = context->protocol == mosq_p_mqtt5
									? (uint8_t)MQTT_RC_BAD_USERNAME_OR_PASSWORD
									: (uint8_t)CONNACK_REFUSED_BAD_USERNAME_PASSWORD;
	തിരികെ send__connack_error_and_return(context, err_code, rc);
}


സ്ഥിതം പൂർണ്ണസംഖ്യ read_protocol_name(ഘടന mosquitto *context, അക്ഷരം protocol_name[7])
{
	/* Read protocol name as length then bytes rather than with read_string
	 * because the length is fixed and we can check that. Removes the need
	 * for another malloc as well. */

	uint16_t slen = 0;

	എങ്കിൽ(packet__read_uint16(&context->in_packet, &slen)){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string length.",
				context->address, context->remote_port);
		തിരികെ MOSQ_ERR_PROTOCOL;
	}
	എങ്കിൽ(slen != 4 /* MQTT */ && slen != 6 /* MQIsdp */){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with incorrect protocol string length (%d).",
				context->address, context->remote_port, slen);
		തിരികെ MOSQ_ERR_PROTOCOL;
	}
	എങ്കിൽ(packet__read_bytes(&context->in_packet, protocol_name, slen)){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string.",
				context->address, context->remote_port);
		തിരികെ MOSQ_ERR_PROTOCOL;
	}
	protocol_name[slen] = '\0';

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ read_and_verify_protocol_version(ഘടന mosquitto *context, സ്ഥിരം അക്ഷരം *protocol_name,
		uint8_t *protocol_version)
{
	uint8_t tmp_protocol_version = 0;
	എങ്കിൽ(packet__read_byte(&context->in_packet, &tmp_protocol_version)){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol version.",
				context->address, context->remote_port);
		തിരികെ MOSQ_ERR_PROTOCOL;
	}

	എങ്കിൽ(check_protocol_version(context->listener, tmp_protocol_version)){
		എങ്കിൽ(tmp_protocol_version == 3 || tmp_protocol_version == 4){
			context->protocol = mosq_p_mqtt311;
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, ശൂന്യ);
		}അല്ലെങ്കിൽ{
			context->protocol = mosq_p_mqtt5;
			send__connack(context, 0, MQTT_RC_UNSUPPORTED_PROTOCOL_VERSION, ശൂന്യ);
		}
		തിരികെ MOSQ_ERR_NOT_SUPPORTED;
	}

	എങ്കിൽ(!strcmp(protocol_name, PROTOCOL_NAME_v31)){
		എങ്കിൽ((tmp_protocol_version&0x7F) != PROTOCOL_VERSION_v31){
			എങ്കിൽ(db.config->connection_messages == true){
				log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->address, context->remote_port, tmp_protocol_version);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, ശൂന്യ);
			തിരികെ MOSQ_ERR_PROTOCOL;
		}
		context->protocol = mosq_p_mqtt31;
		എങ്കിൽ((tmp_protocol_version&0x80) == 0x80){
			context->is_bridge = true;
		}
	}അല്ലെങ്കിൽ എങ്കിൽ(!strcmp(protocol_name, PROTOCOL_NAME)){
		എങ്കിൽ((tmp_protocol_version&0x7F) == PROTOCOL_VERSION_v311){
			context->protocol = mosq_p_mqtt311;

			എങ്കിൽ((tmp_protocol_version&0x80) == 0x80){
				context->is_bridge = true;
			}
		}അല്ലെങ്കിൽ എങ്കിൽ((tmp_protocol_version&0x7F) == PROTOCOL_VERSION_v5){
			context->protocol = mosq_p_mqtt5;
		}അല്ലെങ്കിൽ{
			എങ്കിൽ(db.config->connection_messages == true){
				log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->address, context->remote_port, tmp_protocol_version);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, ശൂന്യ);
			തിരികെ MOSQ_ERR_PROTOCOL;
		}
		എങ്കിൽ((context->in_packet.command&0x0F) != 0x00){
			/* Reserved flags not set to 0, must disconnect. */
			log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero reserved flags (%02X).",
					context->address, context->remote_port, context->in_packet.command);
			തിരികെ MOSQ_ERR_PROTOCOL;
		}
	}അല്ലെങ്കിൽ{
		എങ്കിൽ(db.config->connection_messages == true){
			log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol \"%s\".",
					context->address, context->remote_port, protocol_name);
		}
		തിരികെ MOSQ_ERR_PROTOCOL;
	}
	എങ്കിൽ((tmp_protocol_version&0x7F) != PROTOCOL_VERSION_v31 && context->in_packet.command != CMD_CONNECT){
		തിരികെ MOSQ_ERR_MALFORMED_PACKET;
	}

	*protocol_version = tmp_protocol_version;
	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ read_and_verify_connect_flags(ഘടന mosquitto *context, uint8_t *connect_flags)
{
	എങ്കിൽ(packet__read_byte(&context->in_packet, connect_flags)){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing connect flags.",
				context->address, context->remote_port);
		തിരികെ MOSQ_ERR_PROTOCOL;
	}
	എങ്കിൽ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		എങ്കിൽ((*connect_flags & 0x01) != 0x00){
			log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero connect reserved flag (%02X).",
					context->address, context->remote_port, *connect_flags);
			തിരികെ MOSQ_ERR_PROTOCOL;
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം ശൂന്യം set_session_expiry_interval(ഘടന mosquitto *context, uint8_t clean_start, uint8_t protocol_version)
{
	/* session_expiry_interval will be overridden if the properties are read later */
	എങ്കിൽ(clean_start == false && protocol_version != PROTOCOL_VERSION_v5){
		/* v3* has clean_start == false mean the session never expires */
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
	}അല്ലെങ്കിൽ{
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	}
}


സ്ഥിതം പൂർണ്ണസംഖ്യ read_and_reset_keepalive(ഘടന mosquitto *context)
{
	/* _remove here because net__socket_accept() uses _add and we must have the
	 * correct keepalive value */
	keepalive__remove(context);

	എങ്കിൽ(packet__read_uint16(&context->in_packet, &(context->keepalive))){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "%s sent CONNECT with missing keepalive.",
				context->id);
		തിരികെ MOSQ_ERR_PROTOCOL;
	}
	keepalive__add(context);

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ read_and_verify_v5_connect_properties(ഘടന mosquitto *context, mosquitto_property **properties, uint8_t protocol_version)
{
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ(protocol_version == PROTOCOL_VERSION_v5){
		rc = property__read_all(CMD_CONNECT, &context->in_packet, properties);
		എങ്കിൽ(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
			send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, ശൂന്യ);
		}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_MALFORMED_PACKET){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, ശൂന്യ);
		}
		എങ്കിൽ(rc){
			തിരികെ rc;
		}
	}
	rc = property__process_connect(context, properties);
	എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
		തിരികെ send__connack_error_and_return(context, MQTT_RC_PROTOCOL_ERROR, rc);
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ verify_will_options(ഘടന mosquitto *context, uint8_t will, uint8_t will_qos, uint8_t will_retain, uint8_t protocol_version)
{
	എങ്കിൽ(will_qos == 3){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid Will QoS (%d).",
				context->address, context->remote_port, will_qos);
		തിരികെ MOSQ_ERR_PROTOCOL;
	}

	എങ്കിൽ(will && will_retain && db.config->retain_available == false){
		എങ്കിൽ(protocol_version == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_RETAIN_NOT_SUPPORTED, ശൂന്യ);
		}
		തിരികെ MOSQ_ERR_NOT_SUPPORTED;
	}

	എങ്കിൽ(will && will_qos > context->listener->max_qos){
		എങ്കിൽ(protocol_version == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_QOS_NOT_SUPPORTED, ശൂന്യ);
		}
		തിരികെ MOSQ_ERR_NOT_SUPPORTED;
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ handle_zero_length_clientid(ഘടന mosquitto *context, അക്ഷരം **clientid, bool *allow_zero_length_clientid,
		uint8_t clean_start)
{
	എങ്കിൽ(context->protocol == mosq_p_mqtt31){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s:%d: v3.1 CONNECT with zero length clientid.",
				context->address, context->remote_port);
		send__connack(context, 0, CONNACK_REFUSED_IDENTIFIER_REJECTED, ശൂന്യ);
		തിരികെ MOSQ_ERR_PROTOCOL;
	}

	/* mqtt311/mqtt5 */
	mosquitto_FREE(*clientid);

	എങ്കിൽ(db.config->per_listener_settings){
		*allow_zero_length_clientid = context->listener->security_options->allow_zero_length_clientid;
	}അല്ലെങ്കിൽ{
		*allow_zero_length_clientid = db.config->security_options.allow_zero_length_clientid;
	}

	എങ്കിൽ((context->protocol == mosq_p_mqtt311 && clean_start == 0) || *allow_zero_length_clientid == false){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with zero length clientid when forbidden.",
				context->address, context->remote_port);
		uint8_t err_code = context->protocol == mosq_p_mqtt311 ? (uint8_t)CONNACK_REFUSED_IDENTIFIER_REJECTED : (uint8_t)MQTT_RC_UNSPECIFIED;
		തിരികെ send__connack_error_and_return(context, err_code, MOSQ_ERR_PROTOCOL);
	}

	*clientid = clientid_gen(&(uint16_t){0}, context->listener->security_options->auto_id_prefix,
			context->listener->security_options->auto_id_prefix_len);
	എങ്കിൽ(*clientid == ശൂന്യ){
		തിരികെ MOSQ_ERR_NOMEM;
	}
	context->assigned_id = true;

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ check_clientid_prefixes(ഘടന mosquitto *context, സ്ഥിരം അക്ഷരം *clientid)
{
	എങ്കിൽ(db.config->clientid_prefixes){
		എങ്കിൽ(strncmp(db.config->clientid_prefixes, clientid, strlen(db.config->clientid_prefixes))){
			uint8_t err_code = context->protocol == mosq_p_mqtt5 ? (uint8_t)MQTT_RC_NOT_AUTHORIZED : (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
			തിരികെ send__connack_error_and_return(context, err_code, MOSQ_ERR_AUTH);
		}
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ read_and_verify_clientid_from_packet(ഘടന mosquitto *context, അക്ഷരം **clientid,
		bool *allow_zero_length_clientid, uint8_t clean_start)
{
	പൂർണ്ണസംഖ്യ rc;
	uint16_t slen;

	rc = packet__read_string(&context->in_packet, clientid, &slen);
	എങ്കിൽ(rc == MOSQ_ERR_MALFORMED_UTF8){
		എങ്കിൽ(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_CLIENTID_NOT_VALID, ശൂന്യ);
		}
		തിരികെ MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID;
	}അല്ലെങ്കിൽ എങ്കിൽ(rc > 0){
		തിരികെ MOSQ_ERR_PROTOCOL;
	}

	എങ്കിൽ(slen == 0){
		rc = handle_zero_length_clientid(context, clientid, allow_zero_length_clientid, clean_start);
		എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
			തിരികെ rc;
		}
	}

	rc = check_clientid_prefixes(context, *clientid);
	എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
		തിരികെ rc;
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ set_username_from_packet(ഘടന mosquitto *context, അക്ഷരം **username, സ്ഥിരം അക്ഷരം *clientid)
{
	പൂർണ്ണസംഖ്യ rc;

	rc = packet__read_string(&context->in_packet, username, &(uint16_t){0});
	എങ്കിൽ(rc == MOSQ_ERR_NOMEM){
		തിരികെ MOSQ_ERR_NOMEM;
	}
	എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
		എങ്കിൽ(context->protocol == mosq_p_mqtt31){
			/* Username flag given, but no username. Ignore. */
			/* NOTE: Removed setting of username_flag to zero as it is unused afterwards */
		}അല്ലെങ്കിൽ{
			log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with username flag but no username.",
					clientid);
			തിരികെ MOSQ_ERR_PROTOCOL;
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ set_password_from_packet(ഘടന mosquitto *context, അക്ഷരം **password, uint16_t *password_len, സ്ഥിരം അക്ഷരം *clientid)
{
	പൂർണ്ണസംഖ്യ rc;

	rc = packet_read_ദ്വിമാന(&context->in_packet, (uint8_t **)password, password_len);
	എങ്കിൽ(rc == MOSQ_ERR_NOMEM){
		തിരികെ MOSQ_ERR_NOMEM;
	}

	എങ്കിൽ(rc == MOSQ_ERR_MALFORMED_PACKET){
		എങ്കിൽ(context->protocol == mosq_p_mqtt31){
			/* Password flag given, but no password. Ignore. */
		}അല്ലെങ്കിൽ{
			log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with password flag but no password.",
					clientid);
			തിരികെ MOSQ_ERR_PROTOCOL;
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ read_and_verify_client_credentials_from_packet(ഘടന mosquitto *context,
		അക്ഷരം **username, uint8_t username_flag,
		അക്ഷരം **password, uint16_t *password_len, uint8_t password_flag,
		സ്ഥിരം അക്ഷരം *clientid)
{
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ(username_flag){
		rc = set_username_from_packet(context, username, clientid);
		എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
			തിരികെ rc;
		}
	}അല്ലെങ്കിൽ{
		എങ്കിൽ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
			എങ്കിൽ(password_flag){
				/* username_flag == 0 && password_flag == 1 is forbidden */
				log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Protocol error from %s: password without username, closing connection.", clientid);
				തിരികെ MOSQ_ERR_PROTOCOL;
			}
		}
	}
	എങ്കിൽ(password_flag){
		rc = set_password_from_packet(context, password, password_len, clientid);
		എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
			തിരികെ rc;
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ check_additional_trailing_data(ഘടന mosquitto *context, uint8_t protocol_version)
{
	എങ്കിൽ(context->in_packet.pos != context->in_packet.remaining_length){
		/* Surplus data at end of packet, this must be an error. */
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->in_packet.pos, context->in_packet.remaining_length);
		എങ്കിൽ(protocol_version == PROTOCOL_VERSION_v5){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, ശൂന്യ);
		}
		തിരികെ MOSQ_ERR_PROTOCOL;
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}

#ifdef WITH_TLS


വരിയിൽ സ്ഥിതം പൂർണ്ണസംഖ്യ get_client_cert_and_subject_name(ഘടന mosquitto *context, X509 **client_cert, X509_NAME **name)
{
	*client_cert = SSL_get_peer_certificate(context->ssl);
	എങ്കിൽ(*client_cert == ശൂന്യ){
		തിരികെ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	*name = X509_get_subject_name(*client_cert);
	എങ്കിൽ(*name == ശൂന്യ){
		X509_free(*client_cert);
		തിരികെ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


വരിയിൽ സ്ഥിതം പൂർണ്ണസംഖ്യ free_x509_and_send_connack_error(ഘടന mosquitto *context, X509 *client_cert, പൂർണ്ണസംഖ്യ rc)
{
	X509_free(client_cert);
	തിരികെ send__connack_bad_username_or_password_error(context, rc);
}


വരിയിൽ സ്ഥിതം പൂർണ്ണസംഖ്യ free_x509_and_BIO_and_send_connack_error(ഘടന mosquitto *context, X509 *client_cert,
		BIO *subject_name, പൂർണ്ണസംഖ്യ rc)
{
	BIO_free(subject_name);
	തിരികെ free_x509_and_send_connack_error(context, client_cert, rc);
}


സ്ഥിതം പൂർണ്ണസംഖ്യ set_username_from_cert_identity(ഘടന mosquitto *context)
{
	X509 *client_cert = ശൂന്യ;
	X509_NAME *name = ശൂന്യ;

	എങ്കിൽ(get_client_cert_and_subject_name(context, &client_cert, &name)){
		തിരികെ MOSQ_ERR_AUTH;
	}

	പൂർണ്ണസംഖ്യ i = -1;
	X509_NAME_ENTRY *name_entry = ശൂന്യ;

	i = X509_NAME_get_index_by_NID(name, NID_commonName, -1);
	എങ്കിൽ(i == -1){
		തിരികെ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	name_entry = X509_NAME_get_entry(name, i);
	എങ്കിൽ(!name_entry){
		പോകുക success;
	}

	ASN1_STRING *name_asn1 = ശൂന്യ;
	name_asn1 = X509_NAME_ENTRY_get_data(name_entry);
	എങ്കിൽ(name_asn1 == ശൂന്യ){
		തിരികെ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	സ്ഥിരം അക്ഷരം *cert_identity = (സ്ഥിരം അക്ഷരം *)ASN1_STRING_get0_data(name_asn1);
	എങ്കിൽ(!cert_identity || mosquitto_validate_utf8(cert_identity, (പൂർണ്ണസംഖ്യ)strlen(cert_identity))){
		തിരികെ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	mosquitto_free(context->username);
	context->username = mosquitto_strdup(cert_identity);
	എങ്കിൽ(context->username == ശൂന്യ){
		തിരികെ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_NOMEM);
	}
	/* Make sure there isn't an embedded NUL character in the CN */
	എങ്കിൽ((size_t)ASN1_STRING_length(name_asn1) != strlen(context->username)){
		തിരികെ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}

success:
	X509_free(client_cert);
	client_cert = ശൂന്യ;
	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ set_username_from_cert_subject_name(ഘടന mosquitto *context)
{
	X509 *client_cert = ശൂന്യ;
	X509_NAME *name = ശൂന്യ;

	എങ്കിൽ(get_client_cert_and_subject_name(context, &client_cert, &name)){
		തിരികെ MOSQ_ERR_AUTH;
	}

	അക്ഷരം *subject = ശൂന്യ;
	അക്ഷരം *data_start = ശൂന്യ;
	BIO *subject_bio = ശൂന്യ;
	long name_length = 0;

	subject_bio = BIO_new(BIO_s_mem());
	X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
	data_start = ശൂന്യ;
	name_length = BIO_get_mem_data(subject_bio, &data_start);
	subject = mosquitto_malloc(വലുപ്പം(അക്ഷരം)*(size_t)(name_length+1));
	എങ്കിൽ(!subject){
		തിരികെ free_x509_and_BIO_and_send_connack_error(context, client_cert, subject_bio, MOSQ_ERR_NOMEM);
	}
	memcpy(subject, data_start, (size_t)name_length);
	subject[name_length] = '\0';
	BIO_free(subject_bio);

	എങ്കിൽ(mosquitto_validate_utf8(subject, (പൂർണ്ണസംഖ്യ)strlen(subject))){
		mosquitto_free(subject);
		തിരികെ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	context->username = subject;
	എങ്കിൽ(!context->username){
		X509_free(client_cert);
		തിരികെ MOSQ_ERR_AUTH;
	}

	X509_free(client_cert);
	client_cert = ശൂന്യ;
	തിരികെ MOSQ_ERR_SUCCESS;
}
#endif


സ്ഥിതം പൂർണ്ണസംഖ്യ handle_username_from_cert_options(ഘടന mosquitto *context, അക്ഷരം **username, അക്ഷരം **password, uint16_t password_len)
{
	പൂർണ്ണസംഖ്യ rc;

#ifdef WITH_TLS
	എങ്കിൽ(context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
		/* Don't need the username or password if provided */
		mosquitto_FREE(*username);
		mosquitto_FREE(*password);

		എങ്കിൽ(!context->ssl){
			തിരികെ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
		}
#ifdef FINAL_WITH_TLS_PSK
		എങ്കിൽ(context->listener->psk_hint){
			/* Client should have provided an identity to get this far. */
			എങ്കിൽ(!context->username){
				തിരികെ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}അല്ലെങ്കിൽ
#endif /* FINAL_WITH_TLS_PSK */
		{
			എങ്കിൽ(context->listener->use_identity_as_username){
				rc = set_username_from_cert_identity(context);
			}അല്ലെങ്കിൽ{   /* use_subject_as_username */
				rc = set_username_from_cert_subject_name(context);
			}
			എങ്കിൽ(rc){
				തിരികെ rc;
			}
		}
	}അല്ലെങ്കിൽ
#endif /* WITH_TLS */
	{
#ifdef WITH_TLS
		എങ്കിൽ(context->listener->use_identity_as_username && context->listener->require_certificate){
			mosquitto_FREE(*username);
			mosquitto_FREE(*password);

			എങ്കിൽ(!context->username){
				തിരികെ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}അല്ലെങ്കിൽ
#endif
		{
			/* FIXME - these ensure the mosquitto_clientid() and
			* mosquitto_client_username() functions work, but is hacky */
			context->username = *username;
			context->password = *password;
			context->password_len = password_len;
			*username = ശൂന്യ; /* Avoid free() in error: below. */
			*password = ശൂന്യ;
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ handle_username_as_clientid_option(ഘടന mosquitto *context)
{
	എങ്കിൽ(context->username){
		mosquitto_FREE(context->id);
		context->id = mosquitto_strdup(context->username);
		എങ്കിൽ(!context->id){
			തിരികെ MOSQ_ERR_NOMEM;
		}
	}അല്ലെങ്കിൽ{
		uint8_t err_code = context->protocol == mosq_p_mqtt5
												? (uint8_t)MQTT_RC_NOT_AUTHORIZED
												: (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
		തിരികെ send__connack_error_and_return(context, err_code, MOSQ_ERR_AUTH);
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ handle__connect(ഘടന mosquitto *context)
{
	അക്ഷരം protocol_name[7];
	uint8_t protocol_version;
	uint8_t connect_flags;
	അക്ഷരം *clientid = ശൂന്യ;
	ഘടന mosquitto *found_context;
	ഘടന mosquitto_message_all *will_struct = ശൂന്യ;
	uint8_t will, will_retain, will_qos, clean_start;
	uint8_t username_flag, password_flag;
	അക്ഷരം *username = ശൂന്യ, *password = ശൂന്യ;
	uint16_t password_len = 0;
	പൂർണ്ണസംഖ്യ rc;
	mosquitto_property *properties = ശൂന്യ;
	ശൂന്യം *auth_data = ശൂന്യ;
	uint16_t auth_data_len = 0;
	ശൂന്യം *auth_data_out = ശൂന്യ;
	uint16_t auth_data_out_len = 0;
	bool allow_zero_length_clientid;

	എങ്കിൽ(!context->listener){
		തിരികെ MOSQ_ERR_INVAL;
	}

	/* Don't accept multiple CONNECT commands. */
	എങ്കിൽ(context->state != mosq_cs_new){
		log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Bad client %s:%d sending multiple CONNECT messages.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		പോകുക handle_connect_error;
	}

#ifdef WITH_TLS
	എങ്കിൽ(context->in_packet.command == 0x16 && context->listener->ssl_ctx == ശൂന്യ){ /* 0x16 is TLS handshake client hello */
		log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client from %s:%d appears to be using TLS to connect to a non-TLS listener.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		പോകുക handle_connect_error;
	}
#endif

	rc = read_protocol_name(context, protocol_name);
	എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
		പോകുക handle_connect_error;
	}

	rc = read_and_verify_protocol_version(context, protocol_name, &protocol_version);
	എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
		എങ്കിൽ(rc == MOSQ_ERR_MALFORMED_PACKET){
			തിരികെ rc;
		}
		പോകുക handle_connect_error;
	}

	rc = read_and_verify_connect_flags(context, &connect_flags);
	എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
		പോകുക handle_connect_error;
	}

	clean_start = (connect_flags & 0x02) >> 1;
	set_session_expiry_interval(context, clean_start, protocol_version);

	rc = read_and_reset_keepalive(context);
	എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
		പോകുക handle_connect_error;
	}

	rc = read_and_verify_v5_connect_properties(context, &properties, protocol_version);
	എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
		പോകുക handle_connect_error;
	}

	will = connect_flags & 0x04;
	will_qos = (connect_flags & 0x18) >> 3;
	will_retain = ((connect_flags & 0x20) == 0x20);
	rc = verify_will_options(context, will, will_qos, will_retain, protocol_version);
	എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
		പോകുക handle_connect_error;
	}

	mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_METHOD, &context->auth_method, false);
	mosquitto_property_read_ദ്വിമാന(properties, MQTT_PROP_AUTHENTICATION_DATA, &auth_data, &auth_data_len, false);
	mosquitto_property_free_all(&properties);

	എങ്കിൽ(auth_data && !context->auth_method){
		send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, ശൂന്യ);
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s:%d CONNECT with missing clientid string.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		പോകുക handle_connect_error;
	}

	rc = read_and_verify_clientid_from_packet(context, &clientid, &allow_zero_length_clientid, clean_start);
	എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
		പോകുക handle_connect_error;
	}

	എങ്കിൽ(will){
		rc = will__read(context, clientid, &will_struct, will_qos, will_retain);
		എങ്കിൽ(rc){
			എങ്കിൽ(context->protocol == mosq_p_mqtt5){
				എങ്കിൽ(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
					send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, ശൂന്യ);
				}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_MALFORMED_PACKET){
					send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, ശൂന്യ);
				}
			}
			പോകുക handle_connect_error;
		}
	}അല്ലെങ്കിൽ{
		എങ്കിൽ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
			എങ്കിൽ(will_qos != 0 || will_retain != 0){
				log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT without Will with non-zero QoS (%d) or retain (%d).",
						clientid, will_qos, will_retain);
				rc = MOSQ_ERR_PROTOCOL;
				പോകുക handle_connect_error;
			}
		}
	}

	// Client credentials
	password_flag = connect_flags & 0x40;
	username_flag = connect_flags & 0x80;
	rc = read_and_verify_client_credentials_from_packet(context, &username, username_flag, &password, &password_len, password_flag, clientid);
	എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
		പോകുക handle_connect_error;
	}

	rc = check_additional_trailing_data(context, protocol_version);
	എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
		പോകുക handle_connect_error;
	}

	/* Once context->id is set, if we return from this function with an error
	 * we must make sure that context->id is freed and set to NULL, so that the
	 * client isn't erroneously removed from the by_id hash table. */
	context->id = clientid;
	clientid = ശൂന്യ;

	/* use_identity_as_username or use_subject_as_username */
	rc = handle_username_from_cert_options(context, &username, &password, password_len);
	എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
		പോകുക handle_connect_error;
	}

	/* use_username_as_clientid */
	എങ്കിൽ(context->listener->use_username_as_clientid){
		rc = handle_username_as_clientid_option(context);
		എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
			പോകുക handle_connect_error;
		}
	}

	/* Check for an existing delayed auth check, reject if present */
	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), found_context);
	എങ്കിൽ(found_context){
		rc = MOSQ_ERR_UNKNOWN;
		പോകുക handle_connect_error;
	}

	context->clean_start = clean_start;
	context->will = will_struct;
	will_struct = ശൂന്യ;

	എങ്കിൽ(context->auth_method){
		rc = mosquitto_security_auth_start(context, false, auth_data, auth_data_len, &auth_data_out, &auth_data_out_len);
		mosquitto_FREE(auth_data);
		എങ്കിൽ(rc == MOSQ_ERR_SUCCESS){
			തിരികെ connect__on_authorised(context, auth_data_out, auth_data_out_len);
		}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_AUTH_CONTINUE){
			mosquitto__set_state(context, mosq_cs_authenticating);
			rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_data_out, auth_data_out_len);
			SAFE_FREE(auth_data_out);
			തിരികെ rc;
		}അല്ലെങ്കിൽ{
			SAFE_FREE(auth_data_out);
			will__clear(context);
			എങ്കിൽ(rc == MOSQ_ERR_AUTH){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ശൂന്യ);
				mosquitto_FREE(context->id);
				പോകുക handle_connect_error;
			}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_NOT_SUPPORTED){
				/* Client has requested extended authentication, but we don't support it. */
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_METHOD, ശൂന്യ);
				mosquitto_FREE(context->id);
				പോകുക handle_connect_error;
			}അല്ലെങ്കിൽ{
				mosquitto_FREE(context->id);
				പോകുക handle_connect_error;
			}
		}
	}അല്ലെങ്കിൽ{
#ifdef WITH_TLS
		എങ്കിൽ(context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
			/* Authentication assumed to be cleared */
		}അല്ലെങ്കിൽ
#endif
		{
			rc = mosquitto_basic_auth(context);
			തിരഞ്ഞെടുപ്പ്(rc){
				അവസ്ഥ MOSQ_ERR_SUCCESS:
					നിർത്തുക;
				അവസ്ഥ MOSQ_ERR_AUTH_DELAYED:
					mosquitto__set_state(context, mosq_cs_delayed_auth);
					HASH_ADD_KEYPTR(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context);
					തിരികെ MOSQ_ERR_SUCCESS;
					നിർത്തുക;
				അവസ്ഥ MOSQ_ERR_AUTH:
					എങ്കിൽ(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ശൂന്യ);
					}അല്ലെങ്കിൽ{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ശൂന്യ);
					}
					പോകുക handle_connect_error;
					നിർത്തുക;
				അവസ്ഥ MOSQ_ERR_UNSPECIFIED:
				അവസ്ഥ MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
				അവസ്ഥ MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
				അവസ്ഥ MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
				അവസ്ഥ MOSQ_ERR_SERVER_UNAVAILABLE:
				അവസ്ഥ MOSQ_ERR_SERVER_BUSY:
				അവസ്ഥ MOSQ_ERR_BANNED:
				അവസ്ഥ MOSQ_ERR_BAD_AUTHENTICATION_METHOD:
				അവസ്ഥ MOSQ_ERR_CONNECTION_RATE_EXCEEDED:
					എങ്കിൽ(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, (uint8_t)rc, ശൂന്യ);
					}അല്ലെങ്കിൽ{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ശൂന്യ);
					}
					പോകുക handle_connect_error;
					നിർത്തുക;
				സ്ഥിരസ്ഥിതി:
					rc = MOSQ_ERR_UNKNOWN;
					പോകുക handle_connect_error;
					നിർത്തുക;
			}
		}
		തിരികെ connect__on_authorised(context, ശൂന്യ, 0);
	}


handle_connect_error:
	mosquitto_property_free_all(&properties);
	mosquitto_FREE(auth_data);
	mosquitto_FREE(clientid);
	mosquitto_FREE(username);
	mosquitto_FREE(password);
	എങ്കിൽ(will_struct){
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct->സന്ദേശം.payload);
		mosquitto_FREE(will_struct->സന്ദേശം.topic);
		mosquitto_FREE(will_struct);
	}
	will__clear(context);
	/* We return an error here which means the client is freed later on. */
	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->will_delay_interval = 0;
	തിരികെ rc;
}
