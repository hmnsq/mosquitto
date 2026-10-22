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

#अन्तर्भाव "config.h"

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <utlist.h>

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "packet_mosq.h"
#अन्तर्भाव "property_mosq.h"
#अन्तर्भाव "send_mosq.h"
#अन्तर्भाव "sys_tree.h"
#अन्तर्भाव "tls_mosq.h"
#अन्तर्भाव "util_mosq.h"
#अन्तर्भाव "will_mosq.h"

#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif


स्थिर वर्ण nibble_to_hex(uint8_t मूल्य)
{
	यदि(मूल्य < 0x0A){
		प्रत्यागम (वर्ण)('0'+मूल्य);
	}अन्यथा{
		प्रत्यागम (वर्ण)(65 /*'A'*/ +मूल्य-10);
	}
}


स्थिर वर्ण *clientid_gen(uint16_t *idlen, ध्रुव वर्ण *auto_id_prefix, uint16_t auto_id_prefix_len)
{
	वर्ण *clientid;
	uint8_t rnd[16];
	पूर्णाङ्क pos;

	यदि(mosquitto_getrandom(rnd, 16)){
		प्रत्यागम रिक्त;
	}

	*idlen = (uint16_t)(auto_id_prefix_len + 36);

	clientid = (वर्ण *)mosquitto_calloc((size_t)(*idlen) + 1, परिमाण(वर्ण));
	यदि(!clientid){
		प्रत्यागम रिक्त;
	}
	यदि(auto_id_prefix){
		memcpy(clientid, auto_id_prefix, auto_id_prefix_len);
	}

	pos = 0;
	प्रति(पूर्णाङ्क i=0; i<16; i++){
		clientid[auto_id_prefix_len + pos + 0] = nibble_to_hex(rnd[i] & 0x0F);
		clientid[auto_id_prefix_len + pos + 1] = nibble_to_hex((rnd[i] >> 4) & 0x0F);
		pos += 2;
		यदि(pos == 8 || pos == 13 || pos == 18 || pos == 23){
			clientid[auto_id_prefix_len + pos] = '-';
			pos++;
		}
	}

	प्रत्यागम clientid;
}


पूर्णाङ्क connect__on_authorised(संरचना mosquitto *context, शून्य *auth_data_out, uint16_t auth_data_out_len)
{
	संरचना mosquitto *found_context;
	संरचना mosquitto__subleaf *leaf;
	mosquitto_property *connack_props = रिक्त;
	uint8_t connect_ack = 0;
	पूर्णाङ्क rc;
	पूर्णाङ्क in_quota, out_quota;
	uint16_t in_maximum, out_maximum;

	/* Find if this client already has an entry. This must be done *after* any security checks. */
	HASH_FIND(hh_id, db.contexts_by_id, context->id, strlen(context->id), found_context);
	यदि(found_context){
		/* Found a matching client */
		यदि(!net__is_connected(found_context)){
			/* Client is reconnecting after a disconnect */
			/* FIXME - does anything need to be done here? */
		}अन्यथा{
			/* Client is already connected, disconnect old version. This is
			 * done in context__cleanup() below. */
		}

		यदि(context->clean_start == true){
			sub__clean_session(found_context);
			found_context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			plugin_persist__handle_client_delete(found_context);
		}
		context->is_persisted = found_context->is_persisted;
		found_context->is_persisted = false; /* stops persistence for context being removed */

		यदि(context->clean_start == false && found_context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
			यदि(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
				connect_ack |= 0x01;
			}

			यदि(found_context->msgs_in.inflight || found_context->msgs_in.queued
					|| found_context->msgs_out.inflight || found_context->msgs_out.queued){

				in_quota = context->msgs_in.inflight_quota;
				out_quota = context->msgs_out.inflight_quota;
				in_maximum = context->msgs_in.inflight_maximum;
				out_maximum = context->msgs_out.inflight_maximum;

				memcpy(&context->msgs_in, &found_context->msgs_in, परिमाण(संरचना mosquitto_संदेश_data));
				memcpy(&context->msgs_out, &found_context->msgs_out, परिमाण(संरचना mosquitto_संदेश_data));
				context->last_cmsg_id = found_context->last_cmsg_id;

				memset(&found_context->msgs_in, 0, परिमाण(संरचना mosquitto_संदेश_data));
				memset(&found_context->msgs_out, 0, परिमाण(संरचना mosquitto_संदेश_data));

				context->msgs_in.inflight_quota = in_quota;
				context->msgs_out.inflight_quota = out_quota;
				context->msgs_in.inflight_maximum = in_maximum;
				context->msgs_out.inflight_maximum = out_maximum;

				db__message_reconnect_reset(context);
			}
			context->subs = found_context->subs;
			found_context->subs = रिक्त;
			context->subs_capacity = found_context->subs_capacity;
			context->subs_मोजणी = found_context->subs_मोजणी;
			found_context->subs_capacity = 0;
			found_context->subs_मोजणी = 0;
			context->last_mid = found_context->last_mid;

			प्रति(पूर्णाङ्क i=0; i<context->subs_capacity; i++){
				यदि(context->subs[i]){
					leaf = context->subs[i]->hier->subs;
					यावत्(leaf){
						यदि(leaf->context == found_context){
							leaf->context = context;
						}
						leaf = leaf->next;
					}

					यदि(context->subs[i]->shared){
						leaf = context->subs[i]->shared->subs;
						यावत्(leaf){
							यदि(leaf->context == found_context){
								leaf->context = context;
							}
							leaf = leaf->next;
						}
					}
				}
			}
		}

		यदि((found_context->protocol == mosq_p_mqtt5 && found_context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE)
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
		यदि(found_context->protocol == mosq_p_mqtt5){
			send__disconnect(found_context, MQTT_RC_SESSION_TAKEN_OVER, रिक्त);
		}
		do_disconnect(found_context, MOSQ_ERR_SESSION_TAKEN_OVER);
	}

	यदि(db.config->global_max_clients > 0 && HASH_CNT(hh_id, db.contexts_by_id) >= (unsigned पूर्णाङ्क)db.config->global_max_clients){
		यदि(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_SERVER_BUSY, रिक्त);
		}
		rc = MOSQ_ERR_INVAL;
		गच्छ error;
	}

	यदि(db.config->connection_messages == true){
		यदि(context->is_bridge){
			यदि(context->username){
				log__printf(रिक्त, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive, context->username);
			}अन्यथा{
				log__printf(रिक्त, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d).",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive);
			}
		}अन्यथा{
			यदि(context->username){
				log__printf(रिक्त, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive, context->username);
			}अन्यथा{
				log__printf(रिक्त, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d).",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive);
			}
		}

		यदि(context->will){
			log__printf(रिक्त, MOSQ_LOG_DEBUG, "Will message specified (%ld bytes) (r%d, q%d).",
					(long)context->will->संदेश.payloadlen,
					context->will->संदेश.retain,
					context->will->संदेश.qos);

			log__printf(रिक्त, MOSQ_LOG_DEBUG, "\t%s", context->will->संदेश.topic);
		}अन्यथा{
			log__printf(रिक्त, MOSQ_LOG_DEBUG, "No will message specified.");
		}
	}
#ifdef WITH_TLS
	यदि(context->ssl){
		log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s negotiated %s cipher %s",
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
	यदि(!context->clean_start){
		db.persistence_changes++;
	}
#endif
	context->max_qos = context->listener->max_qos;

	यदि(db.config->max_keepalive &&
			(context->keepalive > db.config->max_keepalive || context->keepalive == 0)){

		keepalive__remove(context);
		context->keepalive = db.config->max_keepalive;
		keepalive__add(context);
		यदि(context->protocol == mosq_p_mqtt5){
			यदि(mosquitto_property_add_int16(&connack_props, MQTT_PROP_SERVER_KEEP_ALIVE, context->keepalive)){
				rc = MOSQ_ERR_NOMEM;
				गच्छ error;
			}
		}अन्यथा{
			send__connack(context, connect_ack, CONNACK_REFUSED_IDENTIFIER_REJECTED, रिक्त);
			rc = MOSQ_ERR_INVAL;
			गच्छ error;
		}
	}


	यदि(context->protocol == mosq_p_mqtt5){
		यदि(context->listener->max_topic_alias > 0){
			यदि(mosquitto_property_add_int16(&connack_props, MQTT_PROP_TOPIC_ALIAS_MAXIMUM, context->listener->max_topic_alias)){
				rc = MOSQ_ERR_NOMEM;
				गच्छ error;
			}
		}
		यदि(context->assigned_id){
			यदि(mosquitto_property_add_string(&connack_props, MQTT_PROP_ASSIGNED_CLIENT_IDENTIFIER, context->id)){
				rc = MOSQ_ERR_NOMEM;
				गच्छ error;
			}
		}
		यदि(context->auth_method){
			यदि(mosquitto_property_add_string(&connack_props, MQTT_PROP_AUTHENTICATION_METHOD, context->auth_method)){
				rc = MOSQ_ERR_NOMEM;
				गच्छ error;
			}

			यदि(auth_data_out && auth_data_out_len > 0){
				यदि(mosquitto_property_add_द्विमान(&connack_props, MQTT_PROP_AUTHENTICATION_DATA, auth_data_out, auth_data_out_len)){
					rc = MOSQ_ERR_NOMEM;
					गच्छ error;
				}
			}
		}
	}
	SAFE_FREE(auth_data_out);

	mosquitto__set_state(context, mosq_cs_active);
	rc = send__connack(context, connect_ack, CONNACK_ACCEPTED, connack_props);
	mosquitto_property_free_all(&connack_props);
	यदि(rc){
		प्रत्यागम rc;
	}
	db__expire_all_messages(context);
	rc = db__message_write_queued_out(context);
	यदि(rc){
		प्रत्यागम rc;
	}
	rc = db__message_write_inflight_out_all(context);

	यदि(rc == MOSQ_ERR_SUCCESS){
		plugin__handle_connect(context);

		यदि(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
			plugin_persist__handle_client_add(context);
		}अन्यथा यदि(context->will){
			plugin_persist__handle_will_add(context);
		}
	}
	प्रत्यागम rc;
error:
	SAFE_FREE(auth_data_out);
	mosquitto_property_free_all(&connack_props);
	प्रत्यागम rc;
}


स्थिर पूर्णाङ्क will__read(संरचना mosquitto *context, ध्रुव वर्ण *clientid, संरचना mosquitto_message_all **will, uint8_t will_qos, पूर्णाङ्क will_retain)
{
	पूर्णाङ्क rc = MOSQ_ERR_SUCCESS;
	size_t slen;
	uint16_t tlen;
	संरचना mosquitto_message_all *will_struct = रिक्त;
	वर्ण *will_topic_mount = रिक्त;
	uint16_t payloadlen;
	mosquitto_property *properties = रिक्त;

	will_struct = mosquitto_calloc(1, परिमाण(संरचना mosquitto_message_all));
	यदि(!will_struct){
		rc = MOSQ_ERR_NOMEM;
		गच्छ error_cleanup;
	}
	यदि(context->protocol == PROTOCOL_VERSION_v5){
		rc = property__read_all(CMD_WILL, &context->in_packet, &properties);
		यदि(rc){
			गच्छ error_cleanup;
		}

		rc = property__process_will(context, will_struct, &properties);
		mosquitto_property_free_all(&properties);
		यदि(rc){
			गच्छ error_cleanup;
		}
	}
	rc = packet__read_string(&context->in_packet, &will_struct->संदेश.topic, &tlen);
	यदि(rc){
		गच्छ error_cleanup;
	}
	यदि(!tlen){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: Will with empty topic.",
				context->id);
		rc = MOSQ_ERR_PROTOCOL;
		गच्छ error_cleanup;
	}

	यदि(context->listener->mount_point){
		slen = strlen(context->listener->mount_point) + strlen(will_struct->संदेश.topic) + 1;
		will_topic_mount = mosquitto_malloc(slen+1);
		यदि(!will_topic_mount){
			rc = MOSQ_ERR_NOMEM;
			गच्छ error_cleanup;
		}

		snprintf(will_topic_mount, slen, "%s%s", context->listener->mount_point, will_struct->संदेश.topic);
		will_topic_mount[slen] = '\0';

		mosquitto_FREE(will_struct->संदेश.topic);
		will_struct->संदेश.topic = will_topic_mount;
	}

	यदि(!strncmp(will_struct->संदेश.topic, "$CONTROL/", strlen("$CONTROL/"))){
		rc = MOSQ_ERR_ACL_DENIED;
		गच्छ error_cleanup;
	}
	rc = mosquitto_pub_topic_check(will_struct->संदेश.topic);
	यदि(rc){
		गच्छ error_cleanup;
	}

	rc = packet__read_uint16(&context->in_packet, &payloadlen);
	यदि(rc){
		गच्छ error_cleanup;
	}

	will_struct->संदेश.payloadlen = payloadlen;
	यदि(will_struct->संदेश.payloadlen > 0){
		यदि(db.config->message_size_limit && will_struct->संदेश.payloadlen > (पूर्णाङ्क)db.config->message_size_limit){
			log__printf(रिक्त, MOSQ_LOG_DEBUG, "Client %s connected with too large Will payload", clientid);
			यदि(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_PACKET_TOO_LARGE, रिक्त);
			}अन्यथा{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, रिक्त);
			}
			rc = MOSQ_ERR_PAYLOAD_SIZE;
			गच्छ error_cleanup;
		}
		will_struct->संदेश.payload = mosquitto_malloc((size_t)will_struct->संदेश.payloadlen);
		यदि(!will_struct->संदेश.payload){
			rc = MOSQ_ERR_NOMEM;
			गच्छ error_cleanup;
		}

		rc = packet__read_bytes(&context->in_packet, will_struct->संदेश.payload, (uint32_t)will_struct->संदेश.payloadlen);
		यदि(rc){
			गच्छ error_cleanup;
		}
	}

	will_struct->संदेश.qos = will_qos;
	will_struct->संदेश.retain = will_retain;

	*will = will_struct;
	प्रत्यागम MOSQ_ERR_SUCCESS;

error_cleanup:
	यदि(will_struct){
		mosquitto_FREE(will_struct->संदेश.topic);
		mosquitto_FREE(will_struct->संदेश.payload);
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct);
	}
	प्रत्यागम rc;
}


स्थिर पूर्णाङ्क check_protocol_version(संरचना mosquitto__listener *listener, पूर्णाङ्क protocol_version)
{
	/* Allow bridge protocol as well. */
	protocol_version &= 0x7F;

	यदि((protocol_version == 3 && listener->disable_protocol_v3 == false)
			|| (protocol_version == 4 && listener->disable_protocol_v4 == false)
			|| (protocol_version == 5 && listener->disable_protocol_v5 == false)
			){

		प्रत्यागम MOSQ_ERR_SUCCESS;
	}अन्यथा{
		प्रत्यागम MOSQ_ERR_NOT_SUPPORTED;
	}
}


अन्तःस्थ स्थिर पूर्णाङ्क send__connack_error_and_return(संरचना mosquitto *context, uint8_t err_code, पूर्णाङ्क rc)
{
	send__connack(context, 0, err_code, रिक्त);
	प्रत्यागम rc;
}


अन्तःस्थ स्थिर पूर्णाङ्क send__connack_bad_username_or_password_error(संरचना mosquitto *context, पूर्णाङ्क rc)
{
	uint8_t err_code  = context->protocol == mosq_p_mqtt5
									? (uint8_t)MQTT_RC_BAD_USERNAME_OR_PASSWORD
									: (uint8_t)CONNACK_REFUSED_BAD_USERNAME_PASSWORD;
	प्रत्यागम send__connack_error_and_return(context, err_code, rc);
}


स्थिर पूर्णाङ्क read_protocol_name(संरचना mosquitto *context, वर्ण protocol_name[7])
{
	/* Read protocol name as length then bytes rather than with read_string
	 * because the length is fixed and we can check that. Removes the need
	 * for another malloc as well. */

	uint16_t slen = 0;

	यदि(packet__read_uint16(&context->in_packet, &slen)){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string length.",
				context->address, context->remote_port);
		प्रत्यागम MOSQ_ERR_PROTOCOL;
	}
	यदि(slen != 4 /* MQTT */ && slen != 6 /* MQIsdp */){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with incorrect protocol string length (%d).",
				context->address, context->remote_port, slen);
		प्रत्यागम MOSQ_ERR_PROTOCOL;
	}
	यदि(packet__read_bytes(&context->in_packet, protocol_name, slen)){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string.",
				context->address, context->remote_port);
		प्रत्यागम MOSQ_ERR_PROTOCOL;
	}
	protocol_name[slen] = '\0';

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क read_and_verify_protocol_version(संरचना mosquitto *context, ध्रुव वर्ण *protocol_name,
		uint8_t *protocol_version)
{
	uint8_t tmp_protocol_version = 0;
	यदि(packet__read_byte(&context->in_packet, &tmp_protocol_version)){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol version.",
				context->address, context->remote_port);
		प्रत्यागम MOSQ_ERR_PROTOCOL;
	}

	यदि(check_protocol_version(context->listener, tmp_protocol_version)){
		यदि(tmp_protocol_version == 3 || tmp_protocol_version == 4){
			context->protocol = mosq_p_mqtt311;
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, रिक्त);
		}अन्यथा{
			context->protocol = mosq_p_mqtt5;
			send__connack(context, 0, MQTT_RC_UNSUPPORTED_PROTOCOL_VERSION, रिक्त);
		}
		प्रत्यागम MOSQ_ERR_NOT_SUPPORTED;
	}

	यदि(!strcmp(protocol_name, PROTOCOL_NAME_v31)){
		यदि((tmp_protocol_version&0x7F) != PROTOCOL_VERSION_v31){
			यदि(db.config->connection_messages == true){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->address, context->remote_port, tmp_protocol_version);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, रिक्त);
			प्रत्यागम MOSQ_ERR_PROTOCOL;
		}
		context->protocol = mosq_p_mqtt31;
		यदि((tmp_protocol_version&0x80) == 0x80){
			context->is_bridge = true;
		}
	}अन्यथा यदि(!strcmp(protocol_name, PROTOCOL_NAME)){
		यदि((tmp_protocol_version&0x7F) == PROTOCOL_VERSION_v311){
			context->protocol = mosq_p_mqtt311;

			यदि((tmp_protocol_version&0x80) == 0x80){
				context->is_bridge = true;
			}
		}अन्यथा यदि((tmp_protocol_version&0x7F) == PROTOCOL_VERSION_v5){
			context->protocol = mosq_p_mqtt5;
		}अन्यथा{
			यदि(db.config->connection_messages == true){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->address, context->remote_port, tmp_protocol_version);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, रिक्त);
			प्रत्यागम MOSQ_ERR_PROTOCOL;
		}
		यदि((context->in_packet.command&0x0F) != 0x00){
			/* Reserved flags not set to 0, must disconnect. */
			log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero reserved flags (%02X).",
					context->address, context->remote_port, context->in_packet.command);
			प्रत्यागम MOSQ_ERR_PROTOCOL;
		}
	}अन्यथा{
		यदि(db.config->connection_messages == true){
			log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol \"%s\".",
					context->address, context->remote_port, protocol_name);
		}
		प्रत्यागम MOSQ_ERR_PROTOCOL;
	}
	यदि((tmp_protocol_version&0x7F) != PROTOCOL_VERSION_v31 && context->in_packet.command != CMD_CONNECT){
		प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
	}

	*protocol_version = tmp_protocol_version;
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क read_and_verify_connect_flags(संरचना mosquitto *context, uint8_t *connect_flags)
{
	यदि(packet__read_byte(&context->in_packet, connect_flags)){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing connect flags.",
				context->address, context->remote_port);
		प्रत्यागम MOSQ_ERR_PROTOCOL;
	}
	यदि(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		यदि((*connect_flags & 0x01) != 0x00){
			log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero connect reserved flag (%02X).",
					context->address, context->remote_port, *connect_flags);
			प्रत्यागम MOSQ_ERR_PROTOCOL;
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर शून्य set_session_expiry_interval(संरचना mosquitto *context, uint8_t clean_start, uint8_t protocol_version)
{
	/* session_expiry_interval will be overridden if the properties are read later */
	यदि(clean_start == false && protocol_version != PROTOCOL_VERSION_v5){
		/* v3* has clean_start == false mean the session never expires */
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
	}अन्यथा{
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	}
}


स्थिर पूर्णाङ्क read_and_reset_keepalive(संरचना mosquitto *context)
{
	/* _remove here because net__socket_accept() uses _add and we must have the
	 * correct keepalive value */
	keepalive__remove(context);

	यदि(packet__read_uint16(&context->in_packet, &(context->keepalive))){
		log__printf(रिक्त, MOSQ_LOG_INFO, "%s sent CONNECT with missing keepalive.",
				context->id);
		प्रत्यागम MOSQ_ERR_PROTOCOL;
	}
	keepalive__add(context);

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क read_and_verify_v5_connect_properties(संरचना mosquitto *context, mosquitto_property **properties, uint8_t protocol_version)
{
	पूर्णाङ्क rc;

	यदि(protocol_version == PROTOCOL_VERSION_v5){
		rc = property__read_all(CMD_CONNECT, &context->in_packet, properties);
		यदि(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
			send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, रिक्त);
		}अन्यथा यदि(rc == MOSQ_ERR_MALFORMED_PACKET){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, रिक्त);
		}
		यदि(rc){
			प्रत्यागम rc;
		}
	}
	rc = property__process_connect(context, properties);
	यदि(rc != MOSQ_ERR_SUCCESS){
		प्रत्यागम send__connack_error_and_return(context, MQTT_RC_PROTOCOL_ERROR, rc);
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क verify_will_options(संरचना mosquitto *context, uint8_t will, uint8_t will_qos, uint8_t will_retain, uint8_t protocol_version)
{
	यदि(will_qos == 3){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid Will QoS (%d).",
				context->address, context->remote_port, will_qos);
		प्रत्यागम MOSQ_ERR_PROTOCOL;
	}

	यदि(will && will_retain && db.config->retain_available == false){
		यदि(protocol_version == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_RETAIN_NOT_SUPPORTED, रिक्त);
		}
		प्रत्यागम MOSQ_ERR_NOT_SUPPORTED;
	}

	यदि(will && will_qos > context->listener->max_qos){
		यदि(protocol_version == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_QOS_NOT_SUPPORTED, रिक्त);
		}
		प्रत्यागम MOSQ_ERR_NOT_SUPPORTED;
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क handle_zero_length_clientid(संरचना mosquitto *context, वर्ण **clientid, bool *allow_zero_length_clientid,
		uint8_t clean_start)
{
	यदि(context->protocol == mosq_p_mqtt31){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s:%d: v3.1 CONNECT with zero length clientid.",
				context->address, context->remote_port);
		send__connack(context, 0, CONNACK_REFUSED_IDENTIFIER_REJECTED, रिक्त);
		प्रत्यागम MOSQ_ERR_PROTOCOL;
	}

	/* mqtt311/mqtt5 */
	mosquitto_FREE(*clientid);

	यदि(db.config->per_listener_settings){
		*allow_zero_length_clientid = context->listener->security_options->allow_zero_length_clientid;
	}अन्यथा{
		*allow_zero_length_clientid = db.config->security_options.allow_zero_length_clientid;
	}

	यदि((context->protocol == mosq_p_mqtt311 && clean_start == 0) || *allow_zero_length_clientid == false){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with zero length clientid when forbidden.",
				context->address, context->remote_port);
		uint8_t err_code = context->protocol == mosq_p_mqtt311 ? (uint8_t)CONNACK_REFUSED_IDENTIFIER_REJECTED : (uint8_t)MQTT_RC_UNSPECIFIED;
		प्रत्यागम send__connack_error_and_return(context, err_code, MOSQ_ERR_PROTOCOL);
	}

	*clientid = clientid_gen(&(uint16_t){0}, context->listener->security_options->auto_id_prefix,
			context->listener->security_options->auto_id_prefix_len);
	यदि(*clientid == रिक्त){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	context->assigned_id = true;

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क check_clientid_prefixes(संरचना mosquitto *context, ध्रुव वर्ण *clientid)
{
	यदि(db.config->clientid_prefixes){
		यदि(strncmp(db.config->clientid_prefixes, clientid, strlen(db.config->clientid_prefixes))){
			uint8_t err_code = context->protocol == mosq_p_mqtt5 ? (uint8_t)MQTT_RC_NOT_AUTHORIZED : (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
			प्रत्यागम send__connack_error_and_return(context, err_code, MOSQ_ERR_AUTH);
		}
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क read_and_verify_clientid_from_packet(संरचना mosquitto *context, वर्ण **clientid,
		bool *allow_zero_length_clientid, uint8_t clean_start)
{
	पूर्णाङ्क rc;
	uint16_t slen;

	rc = packet__read_string(&context->in_packet, clientid, &slen);
	यदि(rc == MOSQ_ERR_MALFORMED_UTF8){
		यदि(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_CLIENTID_NOT_VALID, रिक्त);
		}
		प्रत्यागम MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID;
	}अन्यथा यदि(rc > 0){
		प्रत्यागम MOSQ_ERR_PROTOCOL;
	}

	यदि(slen == 0){
		rc = handle_zero_length_clientid(context, clientid, allow_zero_length_clientid, clean_start);
		यदि(rc != MOSQ_ERR_SUCCESS){
			प्रत्यागम rc;
		}
	}

	rc = check_clientid_prefixes(context, *clientid);
	यदि(rc != MOSQ_ERR_SUCCESS){
		प्रत्यागम rc;
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क set_username_from_packet(संरचना mosquitto *context, वर्ण **username, ध्रुव वर्ण *clientid)
{
	पूर्णाङ्क rc;

	rc = packet__read_string(&context->in_packet, username, &(uint16_t){0});
	यदि(rc == MOSQ_ERR_NOMEM){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	यदि(rc != MOSQ_ERR_SUCCESS){
		यदि(context->protocol == mosq_p_mqtt31){
			/* Username flag given, but no username. Ignore. */
			/* NOTE: Removed setting of username_flag to zero as it is unused afterwards */
		}अन्यथा{
			log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with username flag but no username.",
					clientid);
			प्रत्यागम MOSQ_ERR_PROTOCOL;
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क set_password_from_packet(संरचना mosquitto *context, वर्ण **password, uint16_t *password_len, ध्रुव वर्ण *clientid)
{
	पूर्णाङ्क rc;

	rc = packet_read_द्विमान(&context->in_packet, (uint8_t **)password, password_len);
	यदि(rc == MOSQ_ERR_NOMEM){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	यदि(rc == MOSQ_ERR_MALFORMED_PACKET){
		यदि(context->protocol == mosq_p_mqtt31){
			/* Password flag given, but no password. Ignore. */
		}अन्यथा{
			log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with password flag but no password.",
					clientid);
			प्रत्यागम MOSQ_ERR_PROTOCOL;
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क read_and_verify_client_credentials_from_packet(संरचना mosquitto *context,
		वर्ण **username, uint8_t username_flag,
		वर्ण **password, uint16_t *password_len, uint8_t password_flag,
		ध्रुव वर्ण *clientid)
{
	पूर्णाङ्क rc;

	यदि(username_flag){
		rc = set_username_from_packet(context, username, clientid);
		यदि(rc != MOSQ_ERR_SUCCESS){
			प्रत्यागम rc;
		}
	}अन्यथा{
		यदि(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
			यदि(password_flag){
				/* username_flag == 0 && password_flag == 1 is forbidden */
				log__printf(रिक्त, MOSQ_LOG_ERR, "Protocol error from %s: password without username, closing connection.", clientid);
				प्रत्यागम MOSQ_ERR_PROTOCOL;
			}
		}
	}
	यदि(password_flag){
		rc = set_password_from_packet(context, password, password_len, clientid);
		यदि(rc != MOSQ_ERR_SUCCESS){
			प्रत्यागम rc;
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क check_additional_trailing_data(संरचना mosquitto *context, uint8_t protocol_version)
{
	यदि(context->in_packet.pos != context->in_packet.remaining_length){
		/* Surplus data at end of packet, this must be an error. */
		log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->in_packet.pos, context->in_packet.remaining_length);
		यदि(protocol_version == PROTOCOL_VERSION_v5){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, रिक्त);
		}
		प्रत्यागम MOSQ_ERR_PROTOCOL;
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}

#ifdef WITH_TLS


अन्तःस्थ स्थिर पूर्णाङ्क get_client_cert_and_subject_name(संरचना mosquitto *context, X509 **client_cert, X509_NAME **name)
{
	*client_cert = SSL_get_peer_certificate(context->ssl);
	यदि(*client_cert == रिक्त){
		प्रत्यागम send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	*name = X509_get_subject_name(*client_cert);
	यदि(*name == रिक्त){
		X509_free(*client_cert);
		प्रत्यागम send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


अन्तःस्थ स्थिर पूर्णाङ्क free_x509_and_send_connack_error(संरचना mosquitto *context, X509 *client_cert, पूर्णाङ्क rc)
{
	X509_free(client_cert);
	प्रत्यागम send__connack_bad_username_or_password_error(context, rc);
}


अन्तःस्थ स्थिर पूर्णाङ्क free_x509_and_BIO_and_send_connack_error(संरचना mosquitto *context, X509 *client_cert,
		BIO *subject_name, पूर्णाङ्क rc)
{
	BIO_free(subject_name);
	प्रत्यागम free_x509_and_send_connack_error(context, client_cert, rc);
}


स्थिर पूर्णाङ्क set_username_from_cert_identity(संरचना mosquitto *context)
{
	X509 *client_cert = रिक्त;
	X509_NAME *name = रिक्त;

	यदि(get_client_cert_and_subject_name(context, &client_cert, &name)){
		प्रत्यागम MOSQ_ERR_AUTH;
	}

	पूर्णाङ्क i = -1;
	X509_NAME_ENTRY *name_entry = रिक्त;

	i = X509_NAME_get_index_by_NID(name, NID_commonName, -1);
	यदि(i == -1){
		प्रत्यागम free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	name_entry = X509_NAME_get_entry(name, i);
	यदि(!name_entry){
		गच्छ success;
	}

	ASN1_STRING *name_asn1 = रिक्त;
	name_asn1 = X509_NAME_ENTRY_get_data(name_entry);
	यदि(name_asn1 == रिक्त){
		प्रत्यागम free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	ध्रुव वर्ण *cert_identity = (ध्रुव वर्ण *)ASN1_STRING_get0_data(name_asn1);
	यदि(!cert_identity || mosquitto_validate_utf8(cert_identity, (पूर्णाङ्क)strlen(cert_identity))){
		प्रत्यागम free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	mosquitto_free(context->username);
	context->username = mosquitto_strdup(cert_identity);
	यदि(context->username == रिक्त){
		प्रत्यागम free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_NOMEM);
	}
	/* Make sure there isn't an embedded NUL character in the CN */
	यदि((size_t)ASN1_STRING_length(name_asn1) != strlen(context->username)){
		प्रत्यागम free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}

success:
	X509_free(client_cert);
	client_cert = रिक्त;
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क set_username_from_cert_subject_name(संरचना mosquitto *context)
{
	X509 *client_cert = रिक्त;
	X509_NAME *name = रिक्त;

	यदि(get_client_cert_and_subject_name(context, &client_cert, &name)){
		प्रत्यागम MOSQ_ERR_AUTH;
	}

	वर्ण *subject = रिक्त;
	वर्ण *data_start = रिक्त;
	BIO *subject_bio = रिक्त;
	long name_length = 0;

	subject_bio = BIO_new(BIO_s_mem());
	X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
	data_start = रिक्त;
	name_length = BIO_get_mem_data(subject_bio, &data_start);
	subject = mosquitto_malloc(परिमाण(वर्ण)*(size_t)(name_length+1));
	यदि(!subject){
		प्रत्यागम free_x509_and_BIO_and_send_connack_error(context, client_cert, subject_bio, MOSQ_ERR_NOMEM);
	}
	memcpy(subject, data_start, (size_t)name_length);
	subject[name_length] = '\0';
	BIO_free(subject_bio);

	यदि(mosquitto_validate_utf8(subject, (पूर्णाङ्क)strlen(subject))){
		mosquitto_free(subject);
		प्रत्यागम free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	context->username = subject;
	यदि(!context->username){
		X509_free(client_cert);
		प्रत्यागम MOSQ_ERR_AUTH;
	}

	X509_free(client_cert);
	client_cert = रिक्त;
	प्रत्यागम MOSQ_ERR_SUCCESS;
}
#endif


स्थिर पूर्णाङ्क handle_username_from_cert_options(संरचना mosquitto *context, वर्ण **username, वर्ण **password, uint16_t password_len)
{
	पूर्णाङ्क rc;

#ifdef WITH_TLS
	यदि(context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
		/* Don't need the username or password if provided */
		mosquitto_FREE(*username);
		mosquitto_FREE(*password);

		यदि(!context->ssl){
			प्रत्यागम send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
		}
#ifdef FINAL_WITH_TLS_PSK
		यदि(context->listener->psk_hint){
			/* Client should have provided an identity to get this far. */
			यदि(!context->username){
				प्रत्यागम send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}अन्यथा
#endif /* FINAL_WITH_TLS_PSK */
		{
			यदि(context->listener->use_identity_as_username){
				rc = set_username_from_cert_identity(context);
			}अन्यथा{   /* use_subject_as_username */
				rc = set_username_from_cert_subject_name(context);
			}
			यदि(rc){
				प्रत्यागम rc;
			}
		}
	}अन्यथा
#endif /* WITH_TLS */
	{
#ifdef WITH_TLS
		यदि(context->listener->use_identity_as_username && context->listener->require_certificate){
			mosquitto_FREE(*username);
			mosquitto_FREE(*password);

			यदि(!context->username){
				प्रत्यागम send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}अन्यथा
#endif
		{
			/* FIXME - these ensure the mosquitto_clientid() and
			* mosquitto_client_username() functions work, but is hacky */
			context->username = *username;
			context->password = *password;
			context->password_len = password_len;
			*username = रिक्त; /* Avoid free() in error: below. */
			*password = रिक्त;
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क handle_username_as_clientid_option(संरचना mosquitto *context)
{
	यदि(context->username){
		mosquitto_FREE(context->id);
		context->id = mosquitto_strdup(context->username);
		यदि(!context->id){
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
	}अन्यथा{
		uint8_t err_code = context->protocol == mosq_p_mqtt5
												? (uint8_t)MQTT_RC_NOT_AUTHORIZED
												: (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
		प्रत्यागम send__connack_error_and_return(context, err_code, MOSQ_ERR_AUTH);
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क handle__connect(संरचना mosquitto *context)
{
	वर्ण protocol_name[7];
	uint8_t protocol_version;
	uint8_t connect_flags;
	वर्ण *clientid = रिक्त;
	संरचना mosquitto *found_context;
	संरचना mosquitto_message_all *will_struct = रिक्त;
	uint8_t will, will_retain, will_qos, clean_start;
	uint8_t username_flag, password_flag;
	वर्ण *username = रिक्त, *password = रिक्त;
	uint16_t password_len = 0;
	पूर्णाङ्क rc;
	mosquitto_property *properties = रिक्त;
	शून्य *auth_data = रिक्त;
	uint16_t auth_data_len = 0;
	शून्य *auth_data_out = रिक्त;
	uint16_t auth_data_out_len = 0;
	bool allow_zero_length_clientid;

	यदि(!context->listener){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	/* Don't accept multiple CONNECT commands. */
	यदि(context->state != mosq_cs_new){
		log__printf(रिक्त, MOSQ_LOG_NOTICE, "Bad client %s:%d sending multiple CONNECT messages.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		गच्छ handle_connect_error;
	}

#ifdef WITH_TLS
	यदि(context->in_packet.command == 0x16 && context->listener->ssl_ctx == रिक्त){ /* 0x16 is TLS handshake client hello */
		log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client from %s:%d appears to be using TLS to connect to a non-TLS listener.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		गच्छ handle_connect_error;
	}
#endif

	rc = read_protocol_name(context, protocol_name);
	यदि(rc != MOSQ_ERR_SUCCESS){
		गच्छ handle_connect_error;
	}

	rc = read_and_verify_protocol_version(context, protocol_name, &protocol_version);
	यदि(rc != MOSQ_ERR_SUCCESS){
		यदि(rc == MOSQ_ERR_MALFORMED_PACKET){
			प्रत्यागम rc;
		}
		गच्छ handle_connect_error;
	}

	rc = read_and_verify_connect_flags(context, &connect_flags);
	यदि(rc != MOSQ_ERR_SUCCESS){
		गच्छ handle_connect_error;
	}

	clean_start = (connect_flags & 0x02) >> 1;
	set_session_expiry_interval(context, clean_start, protocol_version);

	rc = read_and_reset_keepalive(context);
	यदि(rc != MOSQ_ERR_SUCCESS){
		गच्छ handle_connect_error;
	}

	rc = read_and_verify_v5_connect_properties(context, &properties, protocol_version);
	यदि(rc != MOSQ_ERR_SUCCESS){
		गच्छ handle_connect_error;
	}

	will = connect_flags & 0x04;
	will_qos = (connect_flags & 0x18) >> 3;
	will_retain = ((connect_flags & 0x20) == 0x20);
	rc = verify_will_options(context, will, will_qos, will_retain, protocol_version);
	यदि(rc != MOSQ_ERR_SUCCESS){
		गच्छ handle_connect_error;
	}

	mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_METHOD, &context->auth_method, false);
	mosquitto_property_read_द्विमान(properties, MQTT_PROP_AUTHENTICATION_DATA, &auth_data, &auth_data_len, false);
	mosquitto_property_free_all(&properties);

	यदि(auth_data && !context->auth_method){
		send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, रिक्त);
		log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s:%d CONNECT with missing clientid string.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		गच्छ handle_connect_error;
	}

	rc = read_and_verify_clientid_from_packet(context, &clientid, &allow_zero_length_clientid, clean_start);
	यदि(rc != MOSQ_ERR_SUCCESS){
		गच्छ handle_connect_error;
	}

	यदि(will){
		rc = will__read(context, clientid, &will_struct, will_qos, will_retain);
		यदि(rc){
			यदि(context->protocol == mosq_p_mqtt5){
				यदि(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
					send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, रिक्त);
				}अन्यथा यदि(rc == MOSQ_ERR_MALFORMED_PACKET){
					send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, रिक्त);
				}
			}
			गच्छ handle_connect_error;
		}
	}अन्यथा{
		यदि(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
			यदि(will_qos != 0 || will_retain != 0){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT without Will with non-zero QoS (%d) or retain (%d).",
						clientid, will_qos, will_retain);
				rc = MOSQ_ERR_PROTOCOL;
				गच्छ handle_connect_error;
			}
		}
	}

	// Client credentials
	password_flag = connect_flags & 0x40;
	username_flag = connect_flags & 0x80;
	rc = read_and_verify_client_credentials_from_packet(context, &username, username_flag, &password, &password_len, password_flag, clientid);
	यदि(rc != MOSQ_ERR_SUCCESS){
		गच्छ handle_connect_error;
	}

	rc = check_additional_trailing_data(context, protocol_version);
	यदि(rc != MOSQ_ERR_SUCCESS){
		गच्छ handle_connect_error;
	}

	/* Once context->id is set, if we return from this function with an error
	 * we must make sure that context->id is freed and set to NULL, so that the
	 * client isn't erroneously removed from the by_id hash table. */
	context->id = clientid;
	clientid = रिक्त;

	/* use_identity_as_username or use_subject_as_username */
	rc = handle_username_from_cert_options(context, &username, &password, password_len);
	यदि(rc != MOSQ_ERR_SUCCESS){
		गच्छ handle_connect_error;
	}

	/* use_username_as_clientid */
	यदि(context->listener->use_username_as_clientid){
		rc = handle_username_as_clientid_option(context);
		यदि(rc != MOSQ_ERR_SUCCESS){
			गच्छ handle_connect_error;
		}
	}

	/* Check for an existing delayed auth check, reject if present */
	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), found_context);
	यदि(found_context){
		rc = MOSQ_ERR_UNKNOWN;
		गच्छ handle_connect_error;
	}

	context->clean_start = clean_start;
	context->will = will_struct;
	will_struct = रिक्त;

	यदि(context->auth_method){
		rc = mosquitto_security_auth_start(context, false, auth_data, auth_data_len, &auth_data_out, &auth_data_out_len);
		mosquitto_FREE(auth_data);
		यदि(rc == MOSQ_ERR_SUCCESS){
			प्रत्यागम connect__on_authorised(context, auth_data_out, auth_data_out_len);
		}अन्यथा यदि(rc == MOSQ_ERR_AUTH_CONTINUE){
			mosquitto__set_state(context, mosq_cs_authenticating);
			rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_data_out, auth_data_out_len);
			SAFE_FREE(auth_data_out);
			प्रत्यागम rc;
		}अन्यथा{
			SAFE_FREE(auth_data_out);
			will__clear(context);
			यदि(rc == MOSQ_ERR_AUTH){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, रिक्त);
				mosquitto_FREE(context->id);
				गच्छ handle_connect_error;
			}अन्यथा यदि(rc == MOSQ_ERR_NOT_SUPPORTED){
				/* Client has requested extended authentication, but we don't support it. */
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_METHOD, रिक्त);
				mosquitto_FREE(context->id);
				गच्छ handle_connect_error;
			}अन्यथा{
				mosquitto_FREE(context->id);
				गच्छ handle_connect_error;
			}
		}
	}अन्यथा{
#ifdef WITH_TLS
		यदि(context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
			/* Authentication assumed to be cleared */
		}अन्यथा
#endif
		{
			rc = mosquitto_basic_auth(context);
			विकल्प(rc){
				पक्ष MOSQ_ERR_SUCCESS:
					भङ्ग;
				पक्ष MOSQ_ERR_AUTH_DELAYED:
					mosquitto__set_state(context, mosq_cs_delayed_auth);
					HASH_ADD_KEYPTR(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context);
					प्रत्यागम MOSQ_ERR_SUCCESS;
					भङ्ग;
				पक्ष MOSQ_ERR_AUTH:
					यदि(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, रिक्त);
					}अन्यथा{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, रिक्त);
					}
					गच्छ handle_connect_error;
					भङ्ग;
				पक्ष MOSQ_ERR_UNSPECIFIED:
				पक्ष MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
				पक्ष MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
				पक्ष MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
				पक्ष MOSQ_ERR_SERVER_UNAVAILABLE:
				पक्ष MOSQ_ERR_SERVER_BUSY:
				पक्ष MOSQ_ERR_BANNED:
				पक्ष MOSQ_ERR_BAD_AUTHENTICATION_METHOD:
				पक्ष MOSQ_ERR_CONNECTION_RATE_EXCEEDED:
					यदि(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, (uint8_t)rc, रिक्त);
					}अन्यथा{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, रिक्त);
					}
					गच्छ handle_connect_error;
					भङ्ग;
				स्वभाव:
					rc = MOSQ_ERR_UNKNOWN;
					गच्छ handle_connect_error;
					भङ्ग;
			}
		}
		प्रत्यागम connect__on_authorised(context, रिक्त, 0);
	}


handle_connect_error:
	mosquitto_property_free_all(&properties);
	mosquitto_FREE(auth_data);
	mosquitto_FREE(clientid);
	mosquitto_FREE(username);
	mosquitto_FREE(password);
	यदि(will_struct){
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
	प्रत्यागम rc;
}
