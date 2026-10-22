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

#समावेश "config.h"

#समावेश <stdio.h>
#समावेश <string.h>
#समावेश <utlist.h>

#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/mqtt_protocol.h"
#समावेश "packet_mosq.h"
#समावेश "property_mosq.h"
#समावेश "send_mosq.h"
#समावेश "sys_tree.h"
#समावेश "tls_mosq.h"
#समावेश "util_mosq.h"
#समावेश "will_mosq.h"

#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#पूर्ण_अगर


अटल अक्षर nibble_to_hex(uint8_t मूल्य)
{
	अगर(मूल्य < 0x0A){
		वापस (अक्षर)('0'+मूल्य);
	}अन्यथा{
		वापस (अक्षर)(65 /*'A'*/ +मूल्य-10);
	}
}


अटल अक्षर *clientid_gen(uint16_t *idlen, स्थिर अक्षर *auto_id_prefix, uint16_t auto_id_prefix_len)
{
	अक्षर *clientid;
	uint8_t rnd[16];
	पूर्णांक pos;

	अगर(mosquitto_getrandom(rnd, 16)){
		वापस शून्य;
	}

	*idlen = (uint16_t)(auto_id_prefix_len + 36);

	clientid = (अक्षर *)mosquitto_calloc((माप_प्रकार)(*idlen) + 1, माप(अक्षर));
	अगर(!clientid){
		वापस शून्य;
	}
	अगर(auto_id_prefix){
		स_नकल(clientid, auto_id_prefix, auto_id_prefix_len);
	}

	pos = 0;
	क्रम(पूर्णांक i=0; i<16; i++){
		clientid[auto_id_prefix_len + pos + 0] = nibble_to_hex(rnd[i] & 0x0F);
		clientid[auto_id_prefix_len + pos + 1] = nibble_to_hex((rnd[i] >> 4) & 0x0F);
		pos += 2;
		अगर(pos == 8 || pos == 13 || pos == 18 || pos == 23){
			clientid[auto_id_prefix_len + pos] = '-';
			pos++;
		}
	}

	वापस clientid;
}


पूर्णांक connect__on_authorised(काष्ठा mosquitto *context, व्योम *auth_तथ्य_out, uint16_t auth_तथ्य_out_len)
{
	काष्ठा mosquitto *found_context;
	काष्ठा mosquitto__subleaf *leaf;
	mosquitto_property *connack_props = शून्य;
	uint8_t connect_ack = 0;
	पूर्णांक rc;
	पूर्णांक in_quota, out_quota;
	uint16_t in_maximum, out_maximum;

	/* Find if this client already has an entry. This must be done *after* any security checks. */
	HASH_FIND(hh_id, db.contexts_by_id, context->id, म_माप(context->id), found_context);
	अगर(found_context){
		/* Found a matching client */
		अगर(!net__is_connected(found_context)){
			/* Client is reconnecting after a disconnect */
			/* FIXME - does anything need to be done here? */
		}अन्यथा{
			/* Client is already connected, disconnect old version. This is
			 * done in context__cleanup() below. */
		}

		अगर(context->clean_start == true){
			sub__clean_session(found_context);
			found_context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			plugin_persist__handle_client_delete(found_context);
		}
		context->is_persisted = found_context->is_persisted;
		found_context->is_persisted = false; /* stops persistence for context being removed */

		अगर(context->clean_start == false && found_context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
			अगर(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
				connect_ack |= 0x01;
			}

			अगर(found_context->msgs_in.inflight || found_context->msgs_in.queued
					|| found_context->msgs_out.inflight || found_context->msgs_out.queued){

				in_quota = context->msgs_in.inflight_quota;
				out_quota = context->msgs_out.inflight_quota;
				in_maximum = context->msgs_in.inflight_maximum;
				out_maximum = context->msgs_out.inflight_maximum;

				स_नकल(&context->msgs_in, &found_context->msgs_in, माप(काष्ठा mosquitto_संदेश_तथ्य));
				स_नकल(&context->msgs_out, &found_context->msgs_out, माप(काष्ठा mosquitto_संदेश_तथ्य));
				context->last_cmsg_id = found_context->last_cmsg_id;

				स_रखो(&found_context->msgs_in, 0, माप(काष्ठा mosquitto_संदेश_तथ्य));
				स_रखो(&found_context->msgs_out, 0, माप(काष्ठा mosquitto_संदेश_तथ्य));

				context->msgs_in.inflight_quota = in_quota;
				context->msgs_out.inflight_quota = out_quota;
				context->msgs_in.inflight_maximum = in_maximum;
				context->msgs_out.inflight_maximum = out_maximum;

				db__message_reconnect_reset(context);
			}
			context->subs = found_context->subs;
			found_context->subs = शून्य;
			context->subs_capacity = found_context->subs_capacity;
			context->subs_गिनती = found_context->subs_गिनती;
			found_context->subs_capacity = 0;
			found_context->subs_गिनती = 0;
			context->last_mid = found_context->last_mid;

			क्रम(पूर्णांक i=0; i<context->subs_capacity; i++){
				अगर(context->subs[i]){
					leaf = context->subs[i]->hier->subs;
					जबतक(leaf){
						अगर(leaf->context == found_context){
							leaf->context = context;
						}
						leaf = leaf->अग्र;
					}

					अगर(context->subs[i]->shared){
						leaf = context->subs[i]->shared->subs;
						जबतक(leaf){
							अगर(leaf->context == found_context){
								leaf->context = context;
							}
							leaf = leaf->अग्र;
						}
					}
				}
			}
		}

		अगर((found_context->protocol == mosq_p_mqtt5 && found_context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE)
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
		अगर(found_context->protocol == mosq_p_mqtt5){
			send__disconnect(found_context, MQTT_RC_SESSION_TAKEN_OVER, शून्य);
		}
		do_disconnect(found_context, MOSQ_ERR_SESSION_TAKEN_OVER);
	}

	अगर(db.config->global_max_clients > 0 && HASH_CNT(hh_id, db.contexts_by_id) >= (अचिन्हित पूर्णांक)db.config->global_max_clients){
		अगर(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_SERVER_BUSY, शून्य);
		}
		rc = MOSQ_ERR_INVAL;
		जाओ error;
	}

	अगर(db.config->connection_messages == true){
		अगर(context->is_bridge){
			अगर(context->username){
				log__printf(शून्य, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive, context->username);
			}अन्यथा{
				log__printf(शून्य, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d).",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive);
			}
		}अन्यथा{
			अगर(context->username){
				log__printf(शून्य, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive, context->username);
			}अन्यथा{
				log__printf(शून्य, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d).",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive);
			}
		}

		अगर(context->will){
			log__printf(शून्य, MOSQ_LOG_DEBUG, "Will message specified (%ld bytes) (r%d, q%d).",
					(दीर्घ)context->will->संदेश.payloadlen,
					context->will->संदेश.retain,
					context->will->संदेश.qos);

			log__printf(शून्य, MOSQ_LOG_DEBUG, "\t%s", context->will->संदेश.topic);
		}अन्यथा{
			log__printf(शून्य, MOSQ_LOG_DEBUG, "No will message specified.");
		}
	}
#अगर_घोषित WITH_TLS
	अगर(context->ssl){
		log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s negotiated %s cipher %s",
				context->id,
				SSL_get_cipher_version(context->ssl),
				SSL_get_cipher_name(context->ssl));
	}
#पूर्ण_अगर

	context->ping_t = 0;
	context->is_dropping = false;

	/* Remove any queued messages that are no longer allowed through ACL,
	 * assuming a possible change of username. */
	db__check_acl_of_all_messages(context);
	context__add_to_by_id(context);

#अगर_घोषित WITH_PERSISTENCE
	अगर(!context->clean_start){
		db.persistence_changes++;
	}
#पूर्ण_अगर
	context->max_qos = context->listener->max_qos;

	अगर(db.config->max_keepalive &&
			(context->keepalive > db.config->max_keepalive || context->keepalive == 0)){

		keepalive__remove(context);
		context->keepalive = db.config->max_keepalive;
		keepalive__add(context);
		अगर(context->protocol == mosq_p_mqtt5){
			अगर(mosquitto_property_add_int16(&connack_props, MQTT_PROP_SERVER_KEEP_ALIVE, context->keepalive)){
				rc = MOSQ_ERR_NOMEM;
				जाओ error;
			}
		}अन्यथा{
			send__connack(context, connect_ack, CONNACK_REFUSED_IDENTIFIER_REJECTED, शून्य);
			rc = MOSQ_ERR_INVAL;
			जाओ error;
		}
	}


	अगर(context->protocol == mosq_p_mqtt5){
		अगर(context->listener->max_topic_alias > 0){
			अगर(mosquitto_property_add_int16(&connack_props, MQTT_PROP_TOPIC_ALIAS_MAXIMUM, context->listener->max_topic_alias)){
				rc = MOSQ_ERR_NOMEM;
				जाओ error;
			}
		}
		अगर(context->assigned_id){
			अगर(mosquitto_property_add_string(&connack_props, MQTT_PROP_ASSIGNED_CLIENT_IDENTIFIER, context->id)){
				rc = MOSQ_ERR_NOMEM;
				जाओ error;
			}
		}
		अगर(context->auth_method){
			अगर(mosquitto_property_add_string(&connack_props, MQTT_PROP_AUTHENTICATION_METHOD, context->auth_method)){
				rc = MOSQ_ERR_NOMEM;
				जाओ error;
			}

			अगर(auth_तथ्य_out && auth_तथ्य_out_len > 0){
				अगर(mosquitto_property_add_द्विआधारी(&connack_props, MQTT_PROP_AUTHENTICATION_तथ्य, auth_तथ्य_out, auth_तथ्य_out_len)){
					rc = MOSQ_ERR_NOMEM;
					जाओ error;
				}
			}
		}
	}
	SAFE_FREE(auth_तथ्य_out);

	mosquitto__set_state(context, mosq_cs_active);
	rc = send__connack(context, connect_ack, CONNACK_ACCEPTED, connack_props);
	mosquitto_property_free_all(&connack_props);
	अगर(rc){
		वापस rc;
	}
	db__expire_all_messages(context);
	rc = db__message_write_queued_out(context);
	अगर(rc){
		वापस rc;
	}
	rc = db__message_write_inflight_out_all(context);

	अगर(rc == MOSQ_ERR_SUCCESS){
		plugin__handle_connect(context);

		अगर(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
			plugin_persist__handle_client_add(context);
		}अन्यथा अगर(context->will){
			plugin_persist__handle_will_add(context);
		}
	}
	वापस rc;
error:
	SAFE_FREE(auth_तथ्य_out);
	mosquitto_property_free_all(&connack_props);
	वापस rc;
}


अटल पूर्णांक will__read(काष्ठा mosquitto *context, स्थिर अक्षर *clientid, काष्ठा mosquitto_message_all **will, uint8_t will_qos, पूर्णांक will_retain)
{
	पूर्णांक rc = MOSQ_ERR_SUCCESS;
	माप_प्रकार slen;
	uint16_t tlen;
	काष्ठा mosquitto_message_all *will_struct = शून्य;
	अक्षर *will_topic_mount = शून्य;
	uint16_t payloadlen;
	mosquitto_property *properties = शून्य;

	will_struct = mosquitto_calloc(1, माप(काष्ठा mosquitto_message_all));
	अगर(!will_struct){
		rc = MOSQ_ERR_NOMEM;
		जाओ error_cleanup;
	}
	अगर(context->protocol == PROTOCOL_VERSION_v5){
		rc = property__read_all(CMD_WILL, &context->in_packet, &properties);
		अगर(rc){
			जाओ error_cleanup;
		}

		rc = property__process_will(context, will_struct, &properties);
		mosquitto_property_free_all(&properties);
		अगर(rc){
			जाओ error_cleanup;
		}
	}
	rc = packet__read_string(&context->in_packet, &will_struct->संदेश.topic, &tlen);
	अगर(rc){
		जाओ error_cleanup;
	}
	अगर(!tlen){
		log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: Will with empty topic.",
				context->id);
		rc = MOSQ_ERR_PROTOCOL;
		जाओ error_cleanup;
	}

	अगर(context->listener->mount_point){
		slen = म_माप(context->listener->mount_point) + म_माप(will_struct->संदेश.topic) + 1;
		will_topic_mount = mosquitto_malloc(slen+1);
		अगर(!will_topic_mount){
			rc = MOSQ_ERR_NOMEM;
			जाओ error_cleanup;
		}

		snprintf(will_topic_mount, slen, "%s%s", context->listener->mount_point, will_struct->संदेश.topic);
		will_topic_mount[slen] = '\0';

		mosquitto_FREE(will_struct->संदेश.topic);
		will_struct->संदेश.topic = will_topic_mount;
	}

	अगर(!म_भेदन(will_struct->संदेश.topic, "$CONTROL/", म_माप("$CONTROL/"))){
		rc = MOSQ_ERR_ACL_DENIED;
		जाओ error_cleanup;
	}
	rc = mosquitto_pub_topic_check(will_struct->संदेश.topic);
	अगर(rc){
		जाओ error_cleanup;
	}

	rc = packet__read_uint16(&context->in_packet, &payloadlen);
	अगर(rc){
		जाओ error_cleanup;
	}

	will_struct->संदेश.payloadlen = payloadlen;
	अगर(will_struct->संदेश.payloadlen > 0){
		अगर(db.config->message_आकार_limit && will_struct->संदेश.payloadlen > (पूर्णांक)db.config->message_आकार_limit){
			log__printf(शून्य, MOSQ_LOG_DEBUG, "Client %s connected with too large Will payload", clientid);
			अगर(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_PACKET_TOO_LARGE, शून्य);
			}अन्यथा{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, शून्य);
			}
			rc = MOSQ_ERR_PAYLOAD_आकार;
			जाओ error_cleanup;
		}
		will_struct->संदेश.payload = mosquitto_malloc((माप_प्रकार)will_struct->संदेश.payloadlen);
		अगर(!will_struct->संदेश.payload){
			rc = MOSQ_ERR_NOMEM;
			जाओ error_cleanup;
		}

		rc = packet__read_bytes(&context->in_packet, will_struct->संदेश.payload, (uint32_t)will_struct->संदेश.payloadlen);
		अगर(rc){
			जाओ error_cleanup;
		}
	}

	will_struct->संदेश.qos = will_qos;
	will_struct->संदेश.retain = will_retain;

	*will = will_struct;
	वापस MOSQ_ERR_SUCCESS;

error_cleanup:
	अगर(will_struct){
		mosquitto_FREE(will_struct->संदेश.topic);
		mosquitto_FREE(will_struct->संदेश.payload);
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct);
	}
	वापस rc;
}


अटल पूर्णांक check_protocol_version(काष्ठा mosquitto__listener *listener, पूर्णांक protocol_version)
{
	/* Allow bridge protocol as well. */
	protocol_version &= 0x7F;

	अगर((protocol_version == 3 && listener->disable_protocol_v3 == false)
			|| (protocol_version == 4 && listener->disable_protocol_v4 == false)
			|| (protocol_version == 5 && listener->disable_protocol_v5 == false)
			){

		वापस MOSQ_ERR_SUCCESS;
	}अन्यथा{
		वापस MOSQ_ERR_NOT_SUPPORTED;
	}
}


अंतरभूत अटल पूर्णांक send__connack_error_and_return(काष्ठा mosquitto *context, uint8_t err_code, पूर्णांक rc)
{
	send__connack(context, 0, err_code, शून्य);
	वापस rc;
}


अंतरभूत अटल पूर्णांक send__connack_bad_username_or_password_error(काष्ठा mosquitto *context, पूर्णांक rc)
{
	uint8_t err_code  = context->protocol == mosq_p_mqtt5
									? (uint8_t)MQTT_RC_BAD_USERNAME_OR_PASSWORD
									: (uint8_t)CONNACK_REFUSED_BAD_USERNAME_PASSWORD;
	वापस send__connack_error_and_return(context, err_code, rc);
}


अटल पूर्णांक read_protocol_name(काष्ठा mosquitto *context, अक्षर protocol_name[7])
{
	/* Read protocol name as length then bytes rather than with read_string
	 * because the length is fixed and we can check that. Removes the need
	 * for another malloc as well. */

	uint16_t slen = 0;

	अगर(packet__read_uint16(&context->in_packet, &slen)){
		log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string length.",
				context->address, context->remote_port);
		वापस MOSQ_ERR_PROTOCOL;
	}
	अगर(slen != 4 /* MQTT */ && slen != 6 /* MQIsdp */){
		log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with incorrect protocol string length (%d).",
				context->address, context->remote_port, slen);
		वापस MOSQ_ERR_PROTOCOL;
	}
	अगर(packet__read_bytes(&context->in_packet, protocol_name, slen)){
		log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string.",
				context->address, context->remote_port);
		वापस MOSQ_ERR_PROTOCOL;
	}
	protocol_name[slen] = '\0';

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक read_and_verify_protocol_version(काष्ठा mosquitto *context, स्थिर अक्षर *protocol_name,
		uint8_t *protocol_version)
{
	uint8_t tmp_protocol_version = 0;
	अगर(packet__read_byte(&context->in_packet, &tmp_protocol_version)){
		log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol version.",
				context->address, context->remote_port);
		वापस MOSQ_ERR_PROTOCOL;
	}

	अगर(check_protocol_version(context->listener, tmp_protocol_version)){
		अगर(tmp_protocol_version == 3 || tmp_protocol_version == 4){
			context->protocol = mosq_p_mqtt311;
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, शून्य);
		}अन्यथा{
			context->protocol = mosq_p_mqtt5;
			send__connack(context, 0, MQTT_RC_UNSUPPORTED_PROTOCOL_VERSION, शून्य);
		}
		वापस MOSQ_ERR_NOT_SUPPORTED;
	}

	अगर(!म_भेद(protocol_name, PROTOCOL_NAME_v31)){
		अगर((tmp_protocol_version&0x7F) != PROTOCOL_VERSION_v31){
			अगर(db.config->connection_messages == true){
				log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->address, context->remote_port, tmp_protocol_version);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, शून्य);
			वापस MOSQ_ERR_PROTOCOL;
		}
		context->protocol = mosq_p_mqtt31;
		अगर((tmp_protocol_version&0x80) == 0x80){
			context->is_bridge = true;
		}
	}अन्यथा अगर(!म_भेद(protocol_name, PROTOCOL_NAME)){
		अगर((tmp_protocol_version&0x7F) == PROTOCOL_VERSION_v311){
			context->protocol = mosq_p_mqtt311;

			अगर((tmp_protocol_version&0x80) == 0x80){
				context->is_bridge = true;
			}
		}अन्यथा अगर((tmp_protocol_version&0x7F) == PROTOCOL_VERSION_v5){
			context->protocol = mosq_p_mqtt5;
		}अन्यथा{
			अगर(db.config->connection_messages == true){
				log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->address, context->remote_port, tmp_protocol_version);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, शून्य);
			वापस MOSQ_ERR_PROTOCOL;
		}
		अगर((context->in_packet.command&0x0F) != 0x00){
			/* Reserved flags not set to 0, must disconnect. */
			log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero reserved flags (%02X).",
					context->address, context->remote_port, context->in_packet.command);
			वापस MOSQ_ERR_PROTOCOL;
		}
	}अन्यथा{
		अगर(db.config->connection_messages == true){
			log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol \"%s\".",
					context->address, context->remote_port, protocol_name);
		}
		वापस MOSQ_ERR_PROTOCOL;
	}
	अगर((tmp_protocol_version&0x7F) != PROTOCOL_VERSION_v31 && context->in_packet.command != CMD_CONNECT){
		वापस MOSQ_ERR_MALFORMED_PACKET;
	}

	*protocol_version = tmp_protocol_version;
	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक read_and_verify_connect_flags(काष्ठा mosquitto *context, uint8_t *connect_flags)
{
	अगर(packet__read_byte(&context->in_packet, connect_flags)){
		log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing connect flags.",
				context->address, context->remote_port);
		वापस MOSQ_ERR_PROTOCOL;
	}
	अगर(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		अगर((*connect_flags & 0x01) != 0x00){
			log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero connect reserved flag (%02X).",
					context->address, context->remote_port, *connect_flags);
			वापस MOSQ_ERR_PROTOCOL;
		}
	}

	वापस MOSQ_ERR_SUCCESS;
}


अटल व्योम set_session_expiry_interval(काष्ठा mosquitto *context, uint8_t clean_start, uint8_t protocol_version)
{
	/* session_expiry_interval will be overridden if the properties are read later */
	अगर(clean_start == false && protocol_version != PROTOCOL_VERSION_v5){
		/* v3* has clean_start == false mean the session never expires */
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
	}अन्यथा{
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	}
}


अटल पूर्णांक read_and_reset_keepalive(काष्ठा mosquitto *context)
{
	/* _remove here because net__socket_accept() uses _add and we must have the
	 * correct keepalive value */
	keepalive__remove(context);

	अगर(packet__read_uint16(&context->in_packet, &(context->keepalive))){
		log__printf(शून्य, MOSQ_LOG_INFO, "%s sent CONNECT with missing keepalive.",
				context->id);
		वापस MOSQ_ERR_PROTOCOL;
	}
	keepalive__add(context);

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक read_and_verify_v5_connect_properties(काष्ठा mosquitto *context, mosquitto_property **properties, uint8_t protocol_version)
{
	पूर्णांक rc;

	अगर(protocol_version == PROTOCOL_VERSION_v5){
		rc = property__read_all(CMD_CONNECT, &context->in_packet, properties);
		अगर(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
			send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, शून्य);
		}अन्यथा अगर(rc == MOSQ_ERR_MALFORMED_PACKET){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, शून्य);
		}
		अगर(rc){
			वापस rc;
		}
	}
	rc = property__process_connect(context, properties);
	अगर(rc != MOSQ_ERR_SUCCESS){
		वापस send__connack_error_and_return(context, MQTT_RC_PROTOCOL_ERROR, rc);
	}

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक verify_will_options(काष्ठा mosquitto *context, uint8_t will, uint8_t will_qos, uint8_t will_retain, uint8_t protocol_version)
{
	अगर(will_qos == 3){
		log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid Will QoS (%d).",
				context->address, context->remote_port, will_qos);
		वापस MOSQ_ERR_PROTOCOL;
	}

	अगर(will && will_retain && db.config->retain_available == false){
		अगर(protocol_version == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_RETAIN_NOT_SUPPORTED, शून्य);
		}
		वापस MOSQ_ERR_NOT_SUPPORTED;
	}

	अगर(will && will_qos > context->listener->max_qos){
		अगर(protocol_version == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_QOS_NOT_SUPPORTED, शून्य);
		}
		वापस MOSQ_ERR_NOT_SUPPORTED;
	}

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक handle_zero_लंबाई_clientid(काष्ठा mosquitto *context, अक्षर **clientid, bool *allow_zero_लंबाई_clientid,
		uint8_t clean_start)
{
	अगर(context->protocol == mosq_p_mqtt31){
		log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s:%d: v3.1 CONNECT with zero length clientid.",
				context->address, context->remote_port);
		send__connack(context, 0, CONNACK_REFUSED_IDENTIFIER_REJECTED, शून्य);
		वापस MOSQ_ERR_PROTOCOL;
	}

	/* mqtt311/mqtt5 */
	mosquitto_FREE(*clientid);

	अगर(db.config->per_listener_settings){
		*allow_zero_लंबाई_clientid = context->listener->security_options->allow_zero_लंबाई_clientid;
	}अन्यथा{
		*allow_zero_लंबाई_clientid = db.config->security_options.allow_zero_लंबाई_clientid;
	}

	अगर((context->protocol == mosq_p_mqtt311 && clean_start == 0) || *allow_zero_लंबाई_clientid == false){
		log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with zero length clientid when forbidden.",
				context->address, context->remote_port);
		uint8_t err_code = context->protocol == mosq_p_mqtt311 ? (uint8_t)CONNACK_REFUSED_IDENTIFIER_REJECTED : (uint8_t)MQTT_RC_UNSPECIFIED;
		वापस send__connack_error_and_return(context, err_code, MOSQ_ERR_PROTOCOL);
	}

	*clientid = clientid_gen(&(uint16_t){0}, context->listener->security_options->auto_id_prefix,
			context->listener->security_options->auto_id_prefix_len);
	अगर(*clientid == शून्य){
		वापस MOSQ_ERR_NOMEM;
	}
	context->assigned_id = true;

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक check_clientid_prefixes(काष्ठा mosquitto *context, स्थिर अक्षर *clientid)
{
	अगर(db.config->clientid_prefixes){
		अगर(म_भेदन(db.config->clientid_prefixes, clientid, म_माप(db.config->clientid_prefixes))){
			uint8_t err_code = context->protocol == mosq_p_mqtt5 ? (uint8_t)MQTT_RC_NOT_AUTHORIZED : (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
			वापस send__connack_error_and_return(context, err_code, MOSQ_ERR_AUTH);
		}
	}
	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक read_and_verify_clientid_from_packet(काष्ठा mosquitto *context, अक्षर **clientid,
		bool *allow_zero_लंबाई_clientid, uint8_t clean_start)
{
	पूर्णांक rc;
	uint16_t slen;

	rc = packet__read_string(&context->in_packet, clientid, &slen);
	अगर(rc == MOSQ_ERR_MALFORMED_UTF8){
		अगर(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_CLIENTID_NOT_VALID, शून्य);
		}
		वापस MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID;
	}अन्यथा अगर(rc > 0){
		वापस MOSQ_ERR_PROTOCOL;
	}

	अगर(slen == 0){
		rc = handle_zero_लंबाई_clientid(context, clientid, allow_zero_लंबाई_clientid, clean_start);
		अगर(rc != MOSQ_ERR_SUCCESS){
			वापस rc;
		}
	}

	rc = check_clientid_prefixes(context, *clientid);
	अगर(rc != MOSQ_ERR_SUCCESS){
		वापस rc;
	}

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक set_username_from_packet(काष्ठा mosquitto *context, अक्षर **username, स्थिर अक्षर *clientid)
{
	पूर्णांक rc;

	rc = packet__read_string(&context->in_packet, username, &(uint16_t){0});
	अगर(rc == MOSQ_ERR_NOMEM){
		वापस MOSQ_ERR_NOMEM;
	}
	अगर(rc != MOSQ_ERR_SUCCESS){
		अगर(context->protocol == mosq_p_mqtt31){
			/* Username flag given, but no username. Ignore. */
			/* NOTE: Removed setting of username_flag to zero as it is unused afterwards */
		}अन्यथा{
			log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with username flag but no username.",
					clientid);
			वापस MOSQ_ERR_PROTOCOL;
		}
	}

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक set_password_from_packet(काष्ठा mosquitto *context, अक्षर **password, uint16_t *password_len, स्थिर अक्षर *clientid)
{
	पूर्णांक rc;

	rc = packet_read_द्विआधारी(&context->in_packet, (uint8_t **)password, password_len);
	अगर(rc == MOSQ_ERR_NOMEM){
		वापस MOSQ_ERR_NOMEM;
	}

	अगर(rc == MOSQ_ERR_MALFORMED_PACKET){
		अगर(context->protocol == mosq_p_mqtt31){
			/* Password flag given, but no password. Ignore. */
		}अन्यथा{
			log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with password flag but no password.",
					clientid);
			वापस MOSQ_ERR_PROTOCOL;
		}
	}

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक read_and_verify_client_credentials_from_packet(काष्ठा mosquitto *context,
		अक्षर **username, uint8_t username_flag,
		अक्षर **password, uint16_t *password_len, uint8_t password_flag,
		स्थिर अक्षर *clientid)
{
	पूर्णांक rc;

	अगर(username_flag){
		rc = set_username_from_packet(context, username, clientid);
		अगर(rc != MOSQ_ERR_SUCCESS){
			वापस rc;
		}
	}अन्यथा{
		अगर(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
			अगर(password_flag){
				/* username_flag == 0 && password_flag == 1 is forbidden */
				log__printf(शून्य, MOSQ_LOG_ERR, "Protocol error from %s: password without username, closing connection.", clientid);
				वापस MOSQ_ERR_PROTOCOL;
			}
		}
	}
	अगर(password_flag){
		rc = set_password_from_packet(context, password, password_len, clientid);
		अगर(rc != MOSQ_ERR_SUCCESS){
			वापस rc;
		}
	}

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक check_additional_trailing_तथ्य(काष्ठा mosquitto *context, uint8_t protocol_version)
{
	अगर(context->in_packet.pos != context->in_packet.remaining_लंबाई){
		/* Surplus data at end of packet, this must be an error. */
		log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->in_packet.pos, context->in_packet.remaining_लंबाई);
		अगर(protocol_version == PROTOCOL_VERSION_v5){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, शून्य);
		}
		वापस MOSQ_ERR_PROTOCOL;
	}

	वापस MOSQ_ERR_SUCCESS;
}

#अगर_घोषित WITH_TLS


अंतरभूत अटल पूर्णांक get_client_cert_and_subject_name(काष्ठा mosquitto *context, X509 **client_cert, X509_NAME **name)
{
	*client_cert = SSL_get_peer_certificate(context->ssl);
	अगर(*client_cert == शून्य){
		वापस send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	*name = X509_get_subject_name(*client_cert);
	अगर(*name == शून्य){
		X509_free(*client_cert);
		वापस send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	वापस MOSQ_ERR_SUCCESS;
}


अंतरभूत अटल पूर्णांक free_x509_and_send_connack_error(काष्ठा mosquitto *context, X509 *client_cert, पूर्णांक rc)
{
	X509_free(client_cert);
	वापस send__connack_bad_username_or_password_error(context, rc);
}


अंतरभूत अटल पूर्णांक free_x509_and_BIO_and_send_connack_error(काष्ठा mosquitto *context, X509 *client_cert,
		BIO *subject_name, पूर्णांक rc)
{
	BIO_free(subject_name);
	वापस free_x509_and_send_connack_error(context, client_cert, rc);
}


अटल पूर्णांक set_username_from_cert_identity(काष्ठा mosquitto *context)
{
	X509 *client_cert = शून्य;
	X509_NAME *name = शून्य;

	अगर(get_client_cert_and_subject_name(context, &client_cert, &name)){
		वापस MOSQ_ERR_AUTH;
	}

	पूर्णांक i = -1;
	X509_NAME_ENTRY *name_entry = शून्य;

	i = X509_NAME_get_सूचकांक_by_NID(name, NID_commonName, -1);
	अगर(i == -1){
		वापस free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	name_entry = X509_NAME_get_entry(name, i);
	अगर(!name_entry){
		जाओ success;
	}

	ASN1_STRING *name_asn1 = शून्य;
	name_asn1 = X509_NAME_ENTRY_get_तथ्य(name_entry);
	अगर(name_asn1 == शून्य){
		वापस free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	स्थिर अक्षर *cert_identity = (स्थिर अक्षर *)ASN1_STRING_get0_तथ्य(name_asn1);
	अगर(!cert_identity || mosquitto_validate_utf8(cert_identity, (पूर्णांक)म_माप(cert_identity))){
		वापस free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	mosquitto_free(context->username);
	context->username = mosquitto_strdup(cert_identity);
	अगर(context->username == शून्य){
		वापस free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_NOMEM);
	}
	/* Make sure there isn't an embedded NUL character in the CN */
	अगर((माप_प्रकार)ASN1_STRING_लंबाई(name_asn1) != म_माप(context->username)){
		वापस free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}

success:
	X509_free(client_cert);
	client_cert = शून्य;
	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक set_username_from_cert_subject_name(काष्ठा mosquitto *context)
{
	X509 *client_cert = शून्य;
	X509_NAME *name = शून्य;

	अगर(get_client_cert_and_subject_name(context, &client_cert, &name)){
		वापस MOSQ_ERR_AUTH;
	}

	अक्षर *subject = शून्य;
	अक्षर *तथ्य_start = शून्य;
	BIO *subject_bio = शून्य;
	दीर्घ name_लंबाई = 0;

	subject_bio = BIO_new(BIO_s_mem());
	X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
	तथ्य_start = शून्य;
	name_लंबाई = BIO_get_mem_तथ्य(subject_bio, &तथ्य_start);
	subject = mosquitto_malloc(माप(अक्षर)*(माप_प्रकार)(name_लंबाई+1));
	अगर(!subject){
		वापस free_x509_and_BIO_and_send_connack_error(context, client_cert, subject_bio, MOSQ_ERR_NOMEM);
	}
	स_नकल(subject, तथ्य_start, (माप_प्रकार)name_लंबाई);
	subject[name_लंबाई] = '\0';
	BIO_free(subject_bio);

	अगर(mosquitto_validate_utf8(subject, (पूर्णांक)म_माप(subject))){
		mosquitto_free(subject);
		वापस free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	context->username = subject;
	अगर(!context->username){
		X509_free(client_cert);
		वापस MOSQ_ERR_AUTH;
	}

	X509_free(client_cert);
	client_cert = शून्य;
	वापस MOSQ_ERR_SUCCESS;
}
#पूर्ण_अगर


अटल पूर्णांक handle_username_from_cert_options(काष्ठा mosquitto *context, अक्षर **username, अक्षर **password, uint16_t password_len)
{
	पूर्णांक rc;

#अगर_घोषित WITH_TLS
	अगर(context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
		/* Don't need the username or password if provided */
		mosquitto_FREE(*username);
		mosquitto_FREE(*password);

		अगर(!context->ssl){
			वापस send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
		}
#अगर_घोषित FINAL_WITH_TLS_PSK
		अगर(context->listener->psk_hint){
			/* Client should have provided an identity to get this far. */
			अगर(!context->username){
				वापस send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}अन्यथा
#पूर्ण_अगर /* FINAL_WITH_TLS_PSK */
		{
			अगर(context->listener->use_identity_as_username){
				rc = set_username_from_cert_identity(context);
			}अन्यथा{   /* use_subject_as_username */
				rc = set_username_from_cert_subject_name(context);
			}
			अगर(rc){
				वापस rc;
			}
		}
	}अन्यथा
#पूर्ण_अगर /* WITH_TLS */
	{
#अगर_घोषित WITH_TLS
		अगर(context->listener->use_identity_as_username && context->listener->require_certificate){
			mosquitto_FREE(*username);
			mosquitto_FREE(*password);

			अगर(!context->username){
				वापस send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}अन्यथा
#पूर्ण_अगर
		{
			/* FIXME - these ensure the mosquitto_clientid() and
			* mosquitto_client_username() functions work, but is hacky */
			context->username = *username;
			context->password = *password;
			context->password_len = password_len;
			*username = शून्य; /* Avoid free() in error: below. */
			*password = शून्य;
		}
	}

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक handle_username_as_clientid_option(काष्ठा mosquitto *context)
{
	अगर(context->username){
		mosquitto_FREE(context->id);
		context->id = mosquitto_strdup(context->username);
		अगर(!context->id){
			वापस MOSQ_ERR_NOMEM;
		}
	}अन्यथा{
		uint8_t err_code = context->protocol == mosq_p_mqtt5
												? (uint8_t)MQTT_RC_NOT_AUTHORIZED
												: (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
		वापस send__connack_error_and_return(context, err_code, MOSQ_ERR_AUTH);
	}

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक handle__connect(काष्ठा mosquitto *context)
{
	अक्षर protocol_name[7];
	uint8_t protocol_version;
	uint8_t connect_flags;
	अक्षर *clientid = शून्य;
	काष्ठा mosquitto *found_context;
	काष्ठा mosquitto_message_all *will_struct = शून्य;
	uint8_t will, will_retain, will_qos, clean_start;
	uint8_t username_flag, password_flag;
	अक्षर *username = शून्य, *password = शून्य;
	uint16_t password_len = 0;
	पूर्णांक rc;
	mosquitto_property *properties = शून्य;
	व्योम *auth_तथ्य = शून्य;
	uint16_t auth_तथ्य_len = 0;
	व्योम *auth_तथ्य_out = शून्य;
	uint16_t auth_तथ्य_out_len = 0;
	bool allow_zero_लंबाई_clientid;

	अगर(!context->listener){
		वापस MOSQ_ERR_INVAL;
	}

	/* Don't accept multiple CONNECT commands. */
	अगर(context->state != mosq_cs_new){
		log__printf(शून्य, MOSQ_LOG_NOTICE, "Bad client %s:%d sending multiple CONNECT messages.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		जाओ handle_connect_error;
	}

#अगर_घोषित WITH_TLS
	अगर(context->in_packet.command == 0x16 && context->listener->ssl_ctx == शून्य){ /* 0x16 is TLS handshake client hello */
		log__printf(शून्य, MOSQ_LOG_NOTICE, "Client from %s:%d appears to be using TLS to connect to a non-TLS listener.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		जाओ handle_connect_error;
	}
#पूर्ण_अगर

	rc = read_protocol_name(context, protocol_name);
	अगर(rc != MOSQ_ERR_SUCCESS){
		जाओ handle_connect_error;
	}

	rc = read_and_verify_protocol_version(context, protocol_name, &protocol_version);
	अगर(rc != MOSQ_ERR_SUCCESS){
		अगर(rc == MOSQ_ERR_MALFORMED_PACKET){
			वापस rc;
		}
		जाओ handle_connect_error;
	}

	rc = read_and_verify_connect_flags(context, &connect_flags);
	अगर(rc != MOSQ_ERR_SUCCESS){
		जाओ handle_connect_error;
	}

	clean_start = (connect_flags & 0x02) >> 1;
	set_session_expiry_interval(context, clean_start, protocol_version);

	rc = read_and_reset_keepalive(context);
	अगर(rc != MOSQ_ERR_SUCCESS){
		जाओ handle_connect_error;
	}

	rc = read_and_verify_v5_connect_properties(context, &properties, protocol_version);
	अगर(rc != MOSQ_ERR_SUCCESS){
		जाओ handle_connect_error;
	}

	will = connect_flags & 0x04;
	will_qos = (connect_flags & 0x18) >> 3;
	will_retain = ((connect_flags & 0x20) == 0x20);
	rc = verify_will_options(context, will, will_qos, will_retain, protocol_version);
	अगर(rc != MOSQ_ERR_SUCCESS){
		जाओ handle_connect_error;
	}

	mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_METHOD, &context->auth_method, false);
	mosquitto_property_read_द्विआधारी(properties, MQTT_PROP_AUTHENTICATION_तथ्य, &auth_तथ्य, &auth_तथ्य_len, false);
	mosquitto_property_free_all(&properties);

	अगर(auth_तथ्य && !context->auth_method){
		send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, शून्य);
		log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s:%d CONNECT with missing clientid string.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		जाओ handle_connect_error;
	}

	rc = read_and_verify_clientid_from_packet(context, &clientid, &allow_zero_लंबाई_clientid, clean_start);
	अगर(rc != MOSQ_ERR_SUCCESS){
		जाओ handle_connect_error;
	}

	अगर(will){
		rc = will__read(context, clientid, &will_struct, will_qos, will_retain);
		अगर(rc){
			अगर(context->protocol == mosq_p_mqtt5){
				अगर(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
					send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, शून्य);
				}अन्यथा अगर(rc == MOSQ_ERR_MALFORMED_PACKET){
					send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, शून्य);
				}
			}
			जाओ handle_connect_error;
		}
	}अन्यथा{
		अगर(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
			अगर(will_qos != 0 || will_retain != 0){
				log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT without Will with non-zero QoS (%d) or retain (%d).",
						clientid, will_qos, will_retain);
				rc = MOSQ_ERR_PROTOCOL;
				जाओ handle_connect_error;
			}
		}
	}

	// Client credentials
	password_flag = connect_flags & 0x40;
	username_flag = connect_flags & 0x80;
	rc = read_and_verify_client_credentials_from_packet(context, &username, username_flag, &password, &password_len, password_flag, clientid);
	अगर(rc != MOSQ_ERR_SUCCESS){
		जाओ handle_connect_error;
	}

	rc = check_additional_trailing_तथ्य(context, protocol_version);
	अगर(rc != MOSQ_ERR_SUCCESS){
		जाओ handle_connect_error;
	}

	/* Once context->id is set, if we return from this function with an error
	 * we must make sure that context->id is freed and set to NULL, so that the
	 * client isn't erroneously removed from the by_id hash table. */
	context->id = clientid;
	clientid = शून्य;

	/* use_identity_as_username or use_subject_as_username */
	rc = handle_username_from_cert_options(context, &username, &password, password_len);
	अगर(rc != MOSQ_ERR_SUCCESS){
		जाओ handle_connect_error;
	}

	/* use_username_as_clientid */
	अगर(context->listener->use_username_as_clientid){
		rc = handle_username_as_clientid_option(context);
		अगर(rc != MOSQ_ERR_SUCCESS){
			जाओ handle_connect_error;
		}
	}

	/* Check for an existing delayed auth check, reject if present */
	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, म_माप(context->id), found_context);
	अगर(found_context){
		rc = MOSQ_ERR_UNKNOWN;
		जाओ handle_connect_error;
	}

	context->clean_start = clean_start;
	context->will = will_struct;
	will_struct = शून्य;

	अगर(context->auth_method){
		rc = mosquitto_security_auth_start(context, false, auth_तथ्य, auth_तथ्य_len, &auth_तथ्य_out, &auth_तथ्य_out_len);
		mosquitto_FREE(auth_तथ्य);
		अगर(rc == MOSQ_ERR_SUCCESS){
			वापस connect__on_authorised(context, auth_तथ्य_out, auth_तथ्य_out_len);
		}अन्यथा अगर(rc == MOSQ_ERR_AUTH_CONTINUE){
			mosquitto__set_state(context, mosq_cs_authenticating);
			rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_तथ्य_out, auth_तथ्य_out_len);
			SAFE_FREE(auth_तथ्य_out);
			वापस rc;
		}अन्यथा{
			SAFE_FREE(auth_तथ्य_out);
			will__clear(context);
			अगर(rc == MOSQ_ERR_AUTH){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, शून्य);
				mosquitto_FREE(context->id);
				जाओ handle_connect_error;
			}अन्यथा अगर(rc == MOSQ_ERR_NOT_SUPPORTED){
				/* Client has requested extended authentication, but we don't support it. */
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_METHOD, शून्य);
				mosquitto_FREE(context->id);
				जाओ handle_connect_error;
			}अन्यथा{
				mosquitto_FREE(context->id);
				जाओ handle_connect_error;
			}
		}
	}अन्यथा{
#अगर_घोषित WITH_TLS
		अगर(context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
			/* Authentication assumed to be cleared */
		}अन्यथा
#पूर्ण_अगर
		{
			rc = mosquitto_basic_auth(context);
			चयन(rc){
				हाल MOSQ_ERR_SUCCESS:
					अवरोध;
				हाल MOSQ_ERR_AUTH_DELAYED:
					mosquitto__set_state(context, mosq_cs_delayed_auth);
					HASH_ADD_KEYPTR(hh_id, db.contexts_by_id_delayed_auth, context->id, म_माप(context->id), context);
					वापस MOSQ_ERR_SUCCESS;
					अवरोध;
				हाल MOSQ_ERR_AUTH:
					अगर(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, शून्य);
					}अन्यथा{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, शून्य);
					}
					जाओ handle_connect_error;
					अवरोध;
				हाल MOSQ_ERR_UNSPECIFIED:
				हाल MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
				हाल MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
				हाल MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
				हाल MOSQ_ERR_SERVER_UNAVAILABLE:
				हाल MOSQ_ERR_SERVER_BUSY:
				हाल MOSQ_ERR_BANNED:
				हाल MOSQ_ERR_BAD_AUTHENTICATION_METHOD:
				हाल MOSQ_ERR_CONNECTION_RATE_EXCEEDED:
					अगर(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, (uint8_t)rc, शून्य);
					}अन्यथा{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, शून्य);
					}
					जाओ handle_connect_error;
					अवरोध;
				शेष:
					rc = MOSQ_ERR_UNKNOWN;
					जाओ handle_connect_error;
					अवरोध;
			}
		}
		वापस connect__on_authorised(context, शून्य, 0);
	}


handle_connect_error:
	mosquitto_property_free_all(&properties);
	mosquitto_FREE(auth_तथ्य);
	mosquitto_FREE(clientid);
	mosquitto_FREE(username);
	mosquitto_FREE(password);
	अगर(will_struct){
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct->संदेश.payload);
		mosquitto_FREE(will_struct->संदेश.topic);
		mosquitto_FREE(will_struct);
	}
	will__clear(context);
	/* We return an error here which means the client is freed later on. */
	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->will_delay_interval = 0;
	वापस rc;
}
