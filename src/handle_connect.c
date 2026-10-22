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

#include "config.h"

#include <stdio.h>
#include <string.h>
#include <utlist.h>

#include "mosquitto_broker_internal.h"
#include "mosquitto/mqtt_protocol.h"
#include "packet_mosq.h"
#include "property_mosq.h"
#include "send_mosq.h"
#include "sys_tree.h"
#include "tls_mosq.h"
#include "util_mosq.h"
#include "will_mosq.h"

#အကယ်၍ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif


static var_na nibble_to_hex(uint8_t value)
{
	အကယ်၍(value < 0x0A){
		ပြန်ပေး (var_na)('0'+value);
	}မဟုတ်ပါက{
		ပြန်ပေး (var_na)(65 /*'A'*/ +value-10);
	}
}


static var_na *clientid_gen(uint16_t *idlen, const var_na *auto_id_prefix, uint16_t auto_id_prefix_len)
{
	var_na *clientid;
	uint8_t rnd[16];
	ကိန်းပြည့် pos;

	အကယ်၍(mosquitto_getrandom(rnd, 16)){
		ပြန်ပေး NULL;
	}

	*idlen = (uint16_t)(auto_id_prefix_len + 36);

	clientid = (var_na *)mosquitto_calloc((size_t)(*idlen) + 1, parimaa_na(var_na));
	အကယ်၍(!clientid){
		ပြန်ပေး NULL;
	}
	အကယ်၍(auto_id_prefix){
		memcpy(clientid, auto_id_prefix, auto_id_prefix_len);
	}

	pos = 0;
	အတွက်(ကိန်းပြည့် i=0; i<16; i++){
		clientid[auto_id_prefix_len + pos + 0] = nibble_to_hex(rnd[i] & 0x0F);
		clientid[auto_id_prefix_len + pos + 1] = nibble_to_hex((rnd[i] >> 4) & 0x0F);
		pos += 2;
		အကယ်၍(pos == 8 || pos == 13 || pos == 18 || pos == 23){
			clientid[auto_id_prefix_len + pos] = '-';
			pos++;
		}
	}

	ပြန်ပေး clientid;
}


ကိန်းပြည့် connect__on_authorised(sa_nracanaa mosquitto *context, void *auth_data_out, uint16_t auth_data_out_len)
{
	sa_nracanaa mosquitto *found_context;
	sa_nracanaa mosquitto__subleaf *leaf;
	mosquitto_property *connack_props = NULL;
	uint8_t connect_ack = 0;
	ကိန်းပြည့် rc;
	ကိန်းပြည့် in_quota, out_quota;
	uint16_t in_maximum, out_maximum;

	/* Find if this client already has an entry. This must be done *after* any security checks. */
	HASH_FIND(hh_id, db.contexts_by_id, context->id, strlen(context->id), found_context);
	အကယ်၍(found_context){
		/* Found a matching client */
		အကယ်၍(!net__is_connected(found_context)){
			/* Client is reconnecting after a disconnect */
			/* FIXME - does anything need to be done here? */
		}မဟုတ်ပါက{
			/* Client is already connected, disconnect old version. This is
			 * done in context__cleanup() below. */
		}

		အကယ်၍(context->clean_start == true){
			sub__clean_session(found_context);
			found_context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			plugin_persist__handle_client_delete(found_context);
		}
		context->is_persisted = found_context->is_persisted;
		found_context->is_persisted = false; /* stops persistence for context being removed */

		အကယ်၍(context->clean_start == false && found_context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
			အကယ်၍(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
				connect_ack |= 0x01;
			}

			အကယ်၍(found_context->msgs_in.inflight || found_context->msgs_in.queued
					|| found_context->msgs_out.inflight || found_context->msgs_out.queued){

				in_quota = context->msgs_in.inflight_quota;
				out_quota = context->msgs_out.inflight_quota;
				in_maximum = context->msgs_in.inflight_maximum;
				out_maximum = context->msgs_out.inflight_maximum;

				memcpy(&context->msgs_in, &found_context->msgs_in, parimaa_na(sa_nracanaa mosquitto_msg_data));
				memcpy(&context->msgs_out, &found_context->msgs_out, parimaa_na(sa_nracanaa mosquitto_msg_data));
				context->last_cmsg_id = found_context->last_cmsg_id;

				memset(&found_context->msgs_in, 0, parimaa_na(sa_nracanaa mosquitto_msg_data));
				memset(&found_context->msgs_out, 0, parimaa_na(sa_nracanaa mosquitto_msg_data));

				context->msgs_in.inflight_quota = in_quota;
				context->msgs_out.inflight_quota = out_quota;
				context->msgs_in.inflight_maximum = in_maximum;
				context->msgs_out.inflight_maximum = out_maximum;

				db__message_reconnect_reset(context);
			}
			context->subs = found_context->subs;
			found_context->subs = NULL;
			context->subs_capacity = found_context->subs_capacity;
			context->subs_count = found_context->subs_count;
			found_context->subs_capacity = 0;
			found_context->subs_count = 0;
			context->last_mid = found_context->last_mid;

			အတွက်(ကိန်းပြည့် i=0; i<context->subs_capacity; i++){
				အကယ်၍(context->subs[i]){
					leaf = context->subs[i]->hier->subs;
					နေစဉ်(leaf){
						အကယ်၍(leaf->context == found_context){
							leaf->context = context;
						}
						leaf = leaf->next;
					}

					အကယ်၍(context->subs[i]->shared){
						leaf = context->subs[i]->shared->subs;
						နေစဉ်(leaf){
							အကယ်၍(leaf->context == found_context){
								leaf->context = context;
							}
							leaf = leaf->next;
						}
					}
				}
			}
		}

		အကယ်၍((found_context->protocol == mosq_p_mqtt5 && found_context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE)
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
		အကယ်၍(found_context->protocol == mosq_p_mqtt5){
			send__disconnect(found_context, MQTT_RC_SESSION_TAKEN_OVER, NULL);
		}
		do_disconnect(found_context, MOSQ_ERR_SESSION_TAKEN_OVER);
	}

	အကယ်၍(db.config->global_max_clients > 0 && HASH_CNT(hh_id, db.contexts_by_id) >= (unsigned ကိန်းပြည့်)db.config->global_max_clients){
		အကယ်၍(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_SERVER_BUSY, NULL);
		}
		rc = MOSQ_ERR_INVAL;
		goto error;
	}

	အကယ်၍(db.config->connection_messages == true){
		အကယ်၍(context->is_bridge){
			အကယ်၍(context->username){
				log__printf(NULL, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive, context->username);
			}မဟုတ်ပါက{
				log__printf(NULL, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d).",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive);
			}
		}မဟုတ်ပါက{
			အကယ်၍(context->username){
				log__printf(NULL, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive, context->username);
			}မဟုတ်ပါက{
				log__printf(NULL, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d).",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive);
			}
		}

		အကယ်၍(context->will){
			log__printf(NULL, MOSQ_LOG_DEBUG, "Will message specified (%ld bytes) (r%d, q%d).",
					(long)context->will->msg.payloadlen,
					context->will->msg.retain,
					context->will->msg.qos);

			log__printf(NULL, MOSQ_LOG_DEBUG, "\t%s", context->will->msg.topic);
		}မဟုတ်ပါက{
			log__printf(NULL, MOSQ_LOG_DEBUG, "No will message specified.");
		}
	}
#ifdef WITH_TLS
	အကယ်၍(context->ssl){
		log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s negotiated %s cipher %s",
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
	အကယ်၍(!context->clean_start){
		db.persistence_changes++;
	}
#endif
	context->max_qos = context->listener->max_qos;

	အကယ်၍(db.config->max_keepalive &&
			(context->keepalive > db.config->max_keepalive || context->keepalive == 0)){

		keepalive__remove(context);
		context->keepalive = db.config->max_keepalive;
		keepalive__add(context);
		အကယ်၍(context->protocol == mosq_p_mqtt5){
			အကယ်၍(mosquitto_property_add_int16(&connack_props, MQTT_PROP_SERVER_KEEP_ALIVE, context->keepalive)){
				rc = MOSQ_ERR_NOMEM;
				goto error;
			}
		}မဟုတ်ပါက{
			send__connack(context, connect_ack, CONNACK_REFUSED_IDENTIFIER_REJECTED, NULL);
			rc = MOSQ_ERR_INVAL;
			goto error;
		}
	}


	အကယ်၍(context->protocol == mosq_p_mqtt5){
		အကယ်၍(context->listener->max_topic_alias > 0){
			အကယ်၍(mosquitto_property_add_int16(&connack_props, MQTT_PROP_TOPIC_ALIAS_MAXIMUM, context->listener->max_topic_alias)){
				rc = MOSQ_ERR_NOMEM;
				goto error;
			}
		}
		အကယ်၍(context->assigned_id){
			အကယ်၍(mosquitto_property_add_string(&connack_props, MQTT_PROP_ASSIGNED_CLIENT_IDENTIFIER, context->id)){
				rc = MOSQ_ERR_NOMEM;
				goto error;
			}
		}
		အကယ်၍(context->auth_method){
			အကယ်၍(mosquitto_property_add_string(&connack_props, MQTT_PROP_AUTHENTICATION_METHOD, context->auth_method)){
				rc = MOSQ_ERR_NOMEM;
				goto error;
			}

			အကယ်၍(auth_data_out && auth_data_out_len > 0){
				အကယ်၍(mosquitto_property_add_binary(&connack_props, MQTT_PROP_AUTHENTICATION_DATA, auth_data_out, auth_data_out_len)){
					rc = MOSQ_ERR_NOMEM;
					goto error;
				}
			}
		}
	}
	SAFE_FREE(auth_data_out);

	mosquitto__set_state(context, mosq_cs_active);
	rc = send__connack(context, connect_ack, CONNACK_ACCEPTED, connack_props);
	mosquitto_property_free_all(&connack_props);
	အကယ်၍(rc){
		ပြန်ပေး rc;
	}
	db__expire_all_messages(context);
	rc = db__message_write_queued_out(context);
	အကယ်၍(rc){
		ပြန်ပေး rc;
	}
	rc = db__message_write_inflight_out_all(context);

	အကယ်၍(rc == MOSQ_ERR_SUCCESS){
		plugin__handle_connect(context);

		အကယ်၍(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
			plugin_persist__handle_client_add(context);
		}မဟုတ်ပါက အကယ်၍(context->will){
			plugin_persist__handle_will_add(context);
		}
	}
	ပြန်ပေး rc;
error:
	SAFE_FREE(auth_data_out);
	mosquitto_property_free_all(&connack_props);
	ပြန်ပေး rc;
}


static ကိန်းပြည့် will__read(sa_nracanaa mosquitto *context, const var_na *clientid, sa_nracanaa mosquitto_message_all **will, uint8_t will_qos, ကိန်းပြည့် will_retain)
{
	ကိန်းပြည့် rc = MOSQ_ERR_SUCCESS;
	size_t slen;
	uint16_t tlen;
	sa_nracanaa mosquitto_message_all *will_struct = NULL;
	var_na *will_topic_mount = NULL;
	uint16_t payloadlen;
	mosquitto_property *properties = NULL;

	will_struct = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto_message_all));
	အကယ်၍(!will_struct){
		rc = MOSQ_ERR_NOMEM;
		goto error_cleanup;
	}
	အကယ်၍(context->protocol == PROTOCOL_VERSION_v5){
		rc = property__read_all(CMD_WILL, &context->in_packet, &properties);
		အကယ်၍(rc){
			goto error_cleanup;
		}

		rc = property__process_will(context, will_struct, &properties);
		mosquitto_property_free_all(&properties);
		အကယ်၍(rc){
			goto error_cleanup;
		}
	}
	rc = packet__read_string(&context->in_packet, &will_struct->msg.topic, &tlen);
	အကယ်၍(rc){
		goto error_cleanup;
	}
	အကယ်၍(!tlen){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: Will with empty topic.",
				context->id);
		rc = MOSQ_ERR_PROTOCOL;
		goto error_cleanup;
	}

	အကယ်၍(context->listener->mount_point){
		slen = strlen(context->listener->mount_point) + strlen(will_struct->msg.topic) + 1;
		will_topic_mount = mosquitto_malloc(slen+1);
		အကယ်၍(!will_topic_mount){
			rc = MOSQ_ERR_NOMEM;
			goto error_cleanup;
		}

		snprintf(will_topic_mount, slen, "%s%s", context->listener->mount_point, will_struct->msg.topic);
		will_topic_mount[slen] = '\0';

		mosquitto_FREE(will_struct->msg.topic);
		will_struct->msg.topic = will_topic_mount;
	}

	အကယ်၍(!strncmp(will_struct->msg.topic, "$CONTROL/", strlen("$CONTROL/"))){
		rc = MOSQ_ERR_ACL_DENIED;
		goto error_cleanup;
	}
	rc = mosquitto_pub_topic_check(will_struct->msg.topic);
	အကယ်၍(rc){
		goto error_cleanup;
	}

	rc = packet__read_uint16(&context->in_packet, &payloadlen);
	အကယ်၍(rc){
		goto error_cleanup;
	}

	will_struct->msg.payloadlen = payloadlen;
	အကယ်၍(will_struct->msg.payloadlen > 0){
		အကယ်၍(db.config->message_size_limit && will_struct->msg.payloadlen > (ကိန်းပြည့်)db.config->message_size_limit){
			log__printf(NULL, MOSQ_LOG_DEBUG, "Client %s connected with too large Will payload", clientid);
			အကယ်၍(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_PACKET_TOO_LARGE, NULL);
			}မဟုတ်ပါက{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, NULL);
			}
			rc = MOSQ_ERR_PAYLOAD_SIZE;
			goto error_cleanup;
		}
		will_struct->msg.payload = mosquitto_malloc((size_t)will_struct->msg.payloadlen);
		အကယ်၍(!will_struct->msg.payload){
			rc = MOSQ_ERR_NOMEM;
			goto error_cleanup;
		}

		rc = packet__read_bytes(&context->in_packet, will_struct->msg.payload, (uint32_t)will_struct->msg.payloadlen);
		အကယ်၍(rc){
			goto error_cleanup;
		}
	}

	will_struct->msg.qos = will_qos;
	will_struct->msg.retain = will_retain;

	*will = will_struct;
	ပြန်ပေး MOSQ_ERR_SUCCESS;

error_cleanup:
	အကယ်၍(will_struct){
		mosquitto_FREE(will_struct->msg.topic);
		mosquitto_FREE(will_struct->msg.payload);
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct);
	}
	ပြန်ပေး rc;
}


static ကိန်းပြည့် check_protocol_version(sa_nracanaa mosquitto__listener *listener, ကိန်းပြည့် protocol_version)
{
	/* Allow bridge protocol as well. */
	protocol_version &= 0x7F;

	အကယ်၍((protocol_version == 3 && listener->disable_protocol_v3 == false)
			|| (protocol_version == 4 && listener->disable_protocol_v4 == false)
			|| (protocol_version == 5 && listener->disable_protocol_v5 == false)
			){

		ပြန်ပေး MOSQ_ERR_SUCCESS;
	}မဟုတ်ပါက{
		ပြန်ပေး MOSQ_ERR_NOT_SUPPORTED;
	}
}


anta_hstha static ကိန်းပြည့် send__connack_error_and_return(sa_nracanaa mosquitto *context, uint8_t err_code, ကိန်းပြည့် rc)
{
	send__connack(context, 0, err_code, NULL);
	ပြန်ပေး rc;
}


anta_hstha static ကိန်းပြည့် send__connack_bad_username_or_password_error(sa_nracanaa mosquitto *context, ကိန်းပြည့် rc)
{
	uint8_t err_code  = context->protocol == mosq_p_mqtt5
									? (uint8_t)MQTT_RC_BAD_USERNAME_OR_PASSWORD
									: (uint8_t)CONNACK_REFUSED_BAD_USERNAME_PASSWORD;
	ပြန်ပေး send__connack_error_and_return(context, err_code, rc);
}


static ကိန်းပြည့် read_protocol_name(sa_nracanaa mosquitto *context, var_na protocol_name[7])
{
	/* Read protocol name as length then bytes rather than with read_string
	 * because the length is fixed and we can check that. Removes the need
	 * for another malloc as well. */

	uint16_t slen = 0;

	အကယ်၍(packet__read_uint16(&context->in_packet, &slen)){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string length.",
				context->address, context->remote_port);
		ပြန်ပေး MOSQ_ERR_PROTOCOL;
	}
	အကယ်၍(slen != 4 /* MQTT */ && slen != 6 /* MQIsdp */){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with incorrect protocol string length (%d).",
				context->address, context->remote_port, slen);
		ပြန်ပေး MOSQ_ERR_PROTOCOL;
	}
	အကယ်၍(packet__read_bytes(&context->in_packet, protocol_name, slen)){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string.",
				context->address, context->remote_port);
		ပြန်ပေး MOSQ_ERR_PROTOCOL;
	}
	protocol_name[slen] = '\0';

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


static ကိန်းပြည့် read_and_verify_protocol_version(sa_nracanaa mosquitto *context, const var_na *protocol_name,
		uint8_t *protocol_version)
{
	uint8_t tmp_protocol_version = 0;
	အကယ်၍(packet__read_byte(&context->in_packet, &tmp_protocol_version)){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol version.",
				context->address, context->remote_port);
		ပြန်ပေး MOSQ_ERR_PROTOCOL;
	}

	အကယ်၍(check_protocol_version(context->listener, tmp_protocol_version)){
		အကယ်၍(tmp_protocol_version == 3 || tmp_protocol_version == 4){
			context->protocol = mosq_p_mqtt311;
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, NULL);
		}မဟုတ်ပါက{
			context->protocol = mosq_p_mqtt5;
			send__connack(context, 0, MQTT_RC_UNSUPPORTED_PROTOCOL_VERSION, NULL);
		}
		ပြန်ပေး MOSQ_ERR_NOT_SUPPORTED;
	}

	အကယ်၍(!strcmp(protocol_name, PROTOCOL_NAME_v31)){
		အကယ်၍((tmp_protocol_version&0x7F) != PROTOCOL_VERSION_v31){
			အကယ်၍(db.config->connection_messages == true){
				log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->address, context->remote_port, tmp_protocol_version);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, NULL);
			ပြန်ပေး MOSQ_ERR_PROTOCOL;
		}
		context->protocol = mosq_p_mqtt31;
		အကယ်၍((tmp_protocol_version&0x80) == 0x80){
			context->is_bridge = true;
		}
	}မဟုတ်ပါက အကယ်၍(!strcmp(protocol_name, PROTOCOL_NAME)){
		အကယ်၍((tmp_protocol_version&0x7F) == PROTOCOL_VERSION_v311){
			context->protocol = mosq_p_mqtt311;

			အကယ်၍((tmp_protocol_version&0x80) == 0x80){
				context->is_bridge = true;
			}
		}မဟုတ်ပါက အကယ်၍((tmp_protocol_version&0x7F) == PROTOCOL_VERSION_v5){
			context->protocol = mosq_p_mqtt5;
		}မဟုတ်ပါက{
			အကယ်၍(db.config->connection_messages == true){
				log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->address, context->remote_port, tmp_protocol_version);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, NULL);
			ပြန်ပေး MOSQ_ERR_PROTOCOL;
		}
		အကယ်၍((context->in_packet.command&0x0F) != 0x00){
			/* Reserved flags not set to 0, must disconnect. */
			log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero reserved flags (%02X).",
					context->address, context->remote_port, context->in_packet.command);
			ပြန်ပေး MOSQ_ERR_PROTOCOL;
		}
	}မဟုတ်ပါက{
		အကယ်၍(db.config->connection_messages == true){
			log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol \"%s\".",
					context->address, context->remote_port, protocol_name);
		}
		ပြန်ပေး MOSQ_ERR_PROTOCOL;
	}
	အကယ်၍((tmp_protocol_version&0x7F) != PROTOCOL_VERSION_v31 && context->in_packet.command != CMD_CONNECT){
		ပြန်ပေး MOSQ_ERR_MALFORMED_PACKET;
	}

	*protocol_version = tmp_protocol_version;
	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


static ကိန်းပြည့် read_and_verify_connect_flags(sa_nracanaa mosquitto *context, uint8_t *connect_flags)
{
	အကယ်၍(packet__read_byte(&context->in_packet, connect_flags)){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing connect flags.",
				context->address, context->remote_port);
		ပြန်ပေး MOSQ_ERR_PROTOCOL;
	}
	အကယ်၍(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		အကယ်၍((*connect_flags & 0x01) != 0x00){
			log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero connect reserved flag (%02X).",
					context->address, context->remote_port, *connect_flags);
			ပြန်ပေး MOSQ_ERR_PROTOCOL;
		}
	}

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


static void set_session_expiry_interval(sa_nracanaa mosquitto *context, uint8_t clean_start, uint8_t protocol_version)
{
	/* session_expiry_interval will be overridden if the properties are read later */
	အကယ်၍(clean_start == false && protocol_version != PROTOCOL_VERSION_v5){
		/* v3* has clean_start == false mean the session never expires */
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
	}မဟုတ်ပါက{
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	}
}


static ကိန်းပြည့် read_and_reset_keepalive(sa_nracanaa mosquitto *context)
{
	/* _remove here because net__socket_accept() uses _add and we must have the
	 * correct keepalive value */
	keepalive__remove(context);

	အကယ်၍(packet__read_uint16(&context->in_packet, &(context->keepalive))){
		log__printf(NULL, MOSQ_LOG_INFO, "%s sent CONNECT with missing keepalive.",
				context->id);
		ပြန်ပေး MOSQ_ERR_PROTOCOL;
	}
	keepalive__add(context);

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


static ကိန်းပြည့် read_and_verify_v5_connect_properties(sa_nracanaa mosquitto *context, mosquitto_property **properties, uint8_t protocol_version)
{
	ကိန်းပြည့် rc;

	အကယ်၍(protocol_version == PROTOCOL_VERSION_v5){
		rc = property__read_all(CMD_CONNECT, &context->in_packet, properties);
		အကယ်၍(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
			send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, NULL);
		}မဟုတ်ပါက အကယ်၍(rc == MOSQ_ERR_MALFORMED_PACKET){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, NULL);
		}
		အကယ်၍(rc){
			ပြန်ပေး rc;
		}
	}
	rc = property__process_connect(context, properties);
	အကယ်၍(rc != MOSQ_ERR_SUCCESS){
		ပြန်ပေး send__connack_error_and_return(context, MQTT_RC_PROTOCOL_ERROR, rc);
	}

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


static ကိန်းပြည့် verify_will_options(sa_nracanaa mosquitto *context, uint8_t will, uint8_t will_qos, uint8_t will_retain, uint8_t protocol_version)
{
	အကယ်၍(will_qos == 3){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid Will QoS (%d).",
				context->address, context->remote_port, will_qos);
		ပြန်ပေး MOSQ_ERR_PROTOCOL;
	}

	အကယ်၍(will && will_retain && db.config->retain_available == false){
		အကယ်၍(protocol_version == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_RETAIN_NOT_SUPPORTED, NULL);
		}
		ပြန်ပေး MOSQ_ERR_NOT_SUPPORTED;
	}

	အကယ်၍(will && will_qos > context->listener->max_qos){
		အကယ်၍(protocol_version == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_QOS_NOT_SUPPORTED, NULL);
		}
		ပြန်ပေး MOSQ_ERR_NOT_SUPPORTED;
	}

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


static ကိန်းပြည့် handle_zero_length_clientid(sa_nracanaa mosquitto *context, var_na **clientid, bool *allow_zero_length_clientid,
		uint8_t clean_start)
{
	အကယ်၍(context->protocol == mosq_p_mqtt31){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d: v3.1 CONNECT with zero length clientid.",
				context->address, context->remote_port);
		send__connack(context, 0, CONNACK_REFUSED_IDENTIFIER_REJECTED, NULL);
		ပြန်ပေး MOSQ_ERR_PROTOCOL;
	}

	/* mqtt311/mqtt5 */
	mosquitto_FREE(*clientid);

	အကယ်၍(db.config->per_listener_settings){
		*allow_zero_length_clientid = context->listener->security_options->allow_zero_length_clientid;
	}မဟုတ်ပါက{
		*allow_zero_length_clientid = db.config->security_options.allow_zero_length_clientid;
	}

	အကယ်၍((context->protocol == mosq_p_mqtt311 && clean_start == 0) || *allow_zero_length_clientid == false){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with zero length clientid when forbidden.",
				context->address, context->remote_port);
		uint8_t err_code = context->protocol == mosq_p_mqtt311 ? (uint8_t)CONNACK_REFUSED_IDENTIFIER_REJECTED : (uint8_t)MQTT_RC_UNSPECIFIED;
		ပြန်ပေး send__connack_error_and_return(context, err_code, MOSQ_ERR_PROTOCOL);
	}

	*clientid = clientid_gen(&(uint16_t){0}, context->listener->security_options->auto_id_prefix,
			context->listener->security_options->auto_id_prefix_len);
	အကယ်၍(*clientid == NULL){
		ပြန်ပေး MOSQ_ERR_NOMEM;
	}
	context->assigned_id = true;

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


static ကိန်းပြည့် check_clientid_prefixes(sa_nracanaa mosquitto *context, const var_na *clientid)
{
	အကယ်၍(db.config->clientid_prefixes){
		အကယ်၍(strncmp(db.config->clientid_prefixes, clientid, strlen(db.config->clientid_prefixes))){
			uint8_t err_code = context->protocol == mosq_p_mqtt5 ? (uint8_t)MQTT_RC_NOT_AUTHORIZED : (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
			ပြန်ပေး send__connack_error_and_return(context, err_code, MOSQ_ERR_AUTH);
		}
	}
	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


static ကိန်းပြည့် read_and_verify_clientid_from_packet(sa_nracanaa mosquitto *context, var_na **clientid,
		bool *allow_zero_length_clientid, uint8_t clean_start)
{
	ကိန်းပြည့် rc;
	uint16_t slen;

	rc = packet__read_string(&context->in_packet, clientid, &slen);
	အကယ်၍(rc == MOSQ_ERR_MALFORMED_UTF8){
		အကယ်၍(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_CLIENTID_NOT_VALID, NULL);
		}
		ပြန်ပေး MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID;
	}မဟုတ်ပါက အကယ်၍(rc > 0){
		ပြန်ပေး MOSQ_ERR_PROTOCOL;
	}

	အကယ်၍(slen == 0){
		rc = handle_zero_length_clientid(context, clientid, allow_zero_length_clientid, clean_start);
		အကယ်၍(rc != MOSQ_ERR_SUCCESS){
			ပြန်ပေး rc;
		}
	}

	rc = check_clientid_prefixes(context, *clientid);
	အကယ်၍(rc != MOSQ_ERR_SUCCESS){
		ပြန်ပေး rc;
	}

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


static ကိန်းပြည့် set_username_from_packet(sa_nracanaa mosquitto *context, var_na **username, const var_na *clientid)
{
	ကိန်းပြည့် rc;

	rc = packet__read_string(&context->in_packet, username, &(uint16_t){0});
	အကယ်၍(rc == MOSQ_ERR_NOMEM){
		ပြန်ပေး MOSQ_ERR_NOMEM;
	}
	အကယ်၍(rc != MOSQ_ERR_SUCCESS){
		အကယ်၍(context->protocol == mosq_p_mqtt31){
			/* Username flag given, but no username. Ignore. */
			/* NOTE: Removed setting of username_flag to zero as it is unused afterwards */
		}မဟုတ်ပါက{
			log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with username flag but no username.",
					clientid);
			ပြန်ပေး MOSQ_ERR_PROTOCOL;
		}
	}

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


static ကိန်းပြည့် set_password_from_packet(sa_nracanaa mosquitto *context, var_na **password, uint16_t *password_len, const var_na *clientid)
{
	ကိန်းပြည့် rc;

	rc = packet__read_binary(&context->in_packet, (uint8_t **)password, password_len);
	အကယ်၍(rc == MOSQ_ERR_NOMEM){
		ပြန်ပေး MOSQ_ERR_NOMEM;
	}

	အကယ်၍(rc == MOSQ_ERR_MALFORMED_PACKET){
		အကယ်၍(context->protocol == mosq_p_mqtt31){
			/* Password flag given, but no password. Ignore. */
		}မဟုတ်ပါက{
			log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with password flag but no password.",
					clientid);
			ပြန်ပေး MOSQ_ERR_PROTOCOL;
		}
	}

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


static ကိန်းပြည့် read_and_verify_client_credentials_from_packet(sa_nracanaa mosquitto *context,
		var_na **username, uint8_t username_flag,
		var_na **password, uint16_t *password_len, uint8_t password_flag,
		const var_na *clientid)
{
	ကိန်းပြည့် rc;

	အကယ်၍(username_flag){
		rc = set_username_from_packet(context, username, clientid);
		အကယ်၍(rc != MOSQ_ERR_SUCCESS){
			ပြန်ပေး rc;
		}
	}မဟုတ်ပါက{
		အကယ်၍(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
			အကယ်၍(password_flag){
				/* username_flag == 0 && password_flag == 1 is forbidden */
				log__printf(NULL, MOSQ_LOG_ERR, "Protocol error from %s: password without username, closing connection.", clientid);
				ပြန်ပေး MOSQ_ERR_PROTOCOL;
			}
		}
	}
	အကယ်၍(password_flag){
		rc = set_password_from_packet(context, password, password_len, clientid);
		အကယ်၍(rc != MOSQ_ERR_SUCCESS){
			ပြန်ပေး rc;
		}
	}

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


static ကိန်းပြည့် check_additional_trailing_data(sa_nracanaa mosquitto *context, uint8_t protocol_version)
{
	အကယ်၍(context->in_packet.pos != context->in_packet.remaining_length){
		/* Surplus data at end of packet, this must be an error. */
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->in_packet.pos, context->in_packet.remaining_length);
		အကယ်၍(protocol_version == PROTOCOL_VERSION_v5){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, NULL);
		}
		ပြန်ပေး MOSQ_ERR_PROTOCOL;
	}

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}

#ifdef WITH_TLS


anta_hstha static ကိန်းပြည့် get_client_cert_and_subject_name(sa_nracanaa mosquitto *context, X509 **client_cert, X509_NAME **name)
{
	*client_cert = SSL_get_peer_certificate(context->ssl);
	အကယ်၍(*client_cert == NULL){
		ပြန်ပေး send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	*name = X509_get_subject_name(*client_cert);
	အကယ်၍(*name == NULL){
		X509_free(*client_cert);
		ပြန်ပေး send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


anta_hstha static ကိန်းပြည့် free_x509_and_send_connack_error(sa_nracanaa mosquitto *context, X509 *client_cert, ကိန်းပြည့် rc)
{
	X509_free(client_cert);
	ပြန်ပေး send__connack_bad_username_or_password_error(context, rc);
}


anta_hstha static ကိန်းပြည့် free_x509_and_BIO_and_send_connack_error(sa_nracanaa mosquitto *context, X509 *client_cert,
		BIO *subject_name, ကိန်းပြည့် rc)
{
	BIO_free(subject_name);
	ပြန်ပေး free_x509_and_send_connack_error(context, client_cert, rc);
}


static ကိန်းပြည့် set_username_from_cert_identity(sa_nracanaa mosquitto *context)
{
	X509 *client_cert = NULL;
	X509_NAME *name = NULL;

	အကယ်၍(get_client_cert_and_subject_name(context, &client_cert, &name)){
		ပြန်ပေး MOSQ_ERR_AUTH;
	}

	ကိန်းပြည့် i = -1;
	X509_NAME_ENTRY *name_entry = NULL;

	i = X509_NAME_get_index_by_NID(name, NID_commonName, -1);
	အကယ်၍(i == -1){
		ပြန်ပေး free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	name_entry = X509_NAME_get_entry(name, i);
	အကယ်၍(!name_entry){
		goto success;
	}

	ASN1_STRING *name_asn1 = NULL;
	name_asn1 = X509_NAME_ENTRY_get_data(name_entry);
	အကယ်၍(name_asn1 == NULL){
		ပြန်ပေး free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	const var_na *cert_identity = (const var_na *)ASN1_STRING_get0_data(name_asn1);
	အကယ်၍(!cert_identity || mosquitto_validate_utf8(cert_identity, (ကိန်းပြည့်)strlen(cert_identity))){
		ပြန်ပေး free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	mosquitto_free(context->username);
	context->username = mosquitto_strdup(cert_identity);
	အကယ်၍(context->username == NULL){
		ပြန်ပေး free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_NOMEM);
	}
	/* Make sure there isn't an embedded NUL character in the CN */
	အကယ်၍((size_t)ASN1_STRING_length(name_asn1) != strlen(context->username)){
		ပြန်ပေး free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}

success:
	X509_free(client_cert);
	client_cert = NULL;
	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


static ကိန်းပြည့် set_username_from_cert_subject_name(sa_nracanaa mosquitto *context)
{
	X509 *client_cert = NULL;
	X509_NAME *name = NULL;

	အကယ်၍(get_client_cert_and_subject_name(context, &client_cert, &name)){
		ပြန်ပေး MOSQ_ERR_AUTH;
	}

	var_na *subject = NULL;
	var_na *data_start = NULL;
	BIO *subject_bio = NULL;
	long name_length = 0;

	subject_bio = BIO_new(BIO_s_mem());
	X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
	data_start = NULL;
	name_length = BIO_get_mem_data(subject_bio, &data_start);
	subject = mosquitto_malloc(parimaa_na(var_na)*(size_t)(name_length+1));
	အကယ်၍(!subject){
		ပြန်ပေး free_x509_and_BIO_and_send_connack_error(context, client_cert, subject_bio, MOSQ_ERR_NOMEM);
	}
	memcpy(subject, data_start, (size_t)name_length);
	subject[name_length] = '\0';
	BIO_free(subject_bio);

	အကယ်၍(mosquitto_validate_utf8(subject, (ကိန်းပြည့်)strlen(subject))){
		mosquitto_free(subject);
		ပြန်ပေး free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	context->username = subject;
	အကယ်၍(!context->username){
		X509_free(client_cert);
		ပြန်ပေး MOSQ_ERR_AUTH;
	}

	X509_free(client_cert);
	client_cert = NULL;
	ပြန်ပေး MOSQ_ERR_SUCCESS;
}
#endif


static ကိန်းပြည့် handle_username_from_cert_options(sa_nracanaa mosquitto *context, var_na **username, var_na **password, uint16_t password_len)
{
	ကိန်းပြည့် rc;

#ifdef WITH_TLS
	အကယ်၍(context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
		/* Don't need the username or password if provided */
		mosquitto_FREE(*username);
		mosquitto_FREE(*password);

		အကယ်၍(!context->ssl){
			ပြန်ပေး send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
		}
#ifdef FINAL_WITH_TLS_PSK
		အကယ်၍(context->listener->psk_hint){
			/* Client should have provided an identity to get this far. */
			အကယ်၍(!context->username){
				ပြန်ပေး send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}မဟုတ်ပါက
#endif /* FINAL_WITH_TLS_PSK */
		{
			အကယ်၍(context->listener->use_identity_as_username){
				rc = set_username_from_cert_identity(context);
			}မဟုတ်ပါက{   /* use_subject_as_username */
				rc = set_username_from_cert_subject_name(context);
			}
			အကယ်၍(rc){
				ပြန်ပေး rc;
			}
		}
	}မဟုတ်ပါက
#endif /* WITH_TLS */
	{
#ifdef WITH_TLS
		အကယ်၍(context->listener->use_identity_as_username && context->listener->require_certificate){
			mosquitto_FREE(*username);
			mosquitto_FREE(*password);

			အကယ်၍(!context->username){
				ပြန်ပေး send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}မဟုတ်ပါက
#endif
		{
			/* FIXME - these ensure the mosquitto_clientid() and
			* mosquitto_client_username() functions work, but is hacky */
			context->username = *username;
			context->password = *password;
			context->password_len = password_len;
			*username = NULL; /* Avoid free() in error: below. */
			*password = NULL;
		}
	}

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


static ကိန်းပြည့် handle_username_as_clientid_option(sa_nracanaa mosquitto *context)
{
	အကယ်၍(context->username){
		mosquitto_FREE(context->id);
		context->id = mosquitto_strdup(context->username);
		အကယ်၍(!context->id){
			ပြန်ပေး MOSQ_ERR_NOMEM;
		}
	}မဟုတ်ပါက{
		uint8_t err_code = context->protocol == mosq_p_mqtt5
												? (uint8_t)MQTT_RC_NOT_AUTHORIZED
												: (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
		ပြန်ပေး send__connack_error_and_return(context, err_code, MOSQ_ERR_AUTH);
	}

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


ကိန်းပြည့် handle__connect(sa_nracanaa mosquitto *context)
{
	var_na protocol_name[7];
	uint8_t protocol_version;
	uint8_t connect_flags;
	var_na *clientid = NULL;
	sa_nracanaa mosquitto *found_context;
	sa_nracanaa mosquitto_message_all *will_struct = NULL;
	uint8_t will, will_retain, will_qos, clean_start;
	uint8_t username_flag, password_flag;
	var_na *username = NULL, *password = NULL;
	uint16_t password_len = 0;
	ကိန်းပြည့် rc;
	mosquitto_property *properties = NULL;
	void *auth_data = NULL;
	uint16_t auth_data_len = 0;
	void *auth_data_out = NULL;
	uint16_t auth_data_out_len = 0;
	bool allow_zero_length_clientid;

	အကယ်၍(!context->listener){
		ပြန်ပေး MOSQ_ERR_INVAL;
	}

	/* Don't accept multiple CONNECT commands. */
	အကယ်၍(context->state != mosq_cs_new){
		log__printf(NULL, MOSQ_LOG_NOTICE, "Bad client %s:%d sending multiple CONNECT messages.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		goto handle_connect_error;
	}

#ifdef WITH_TLS
	အကယ်၍(context->in_packet.command == 0x16 && context->listener->ssl_ctx == NULL){ /* 0x16 is TLS handshake client hello */
		log__printf(NULL, MOSQ_LOG_NOTICE, "Client from %s:%d appears to be using TLS to connect to a non-TLS listener.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		goto handle_connect_error;
	}
#endif

	rc = read_protocol_name(context, protocol_name);
	အကယ်၍(rc != MOSQ_ERR_SUCCESS){
		goto handle_connect_error;
	}

	rc = read_and_verify_protocol_version(context, protocol_name, &protocol_version);
	အကယ်၍(rc != MOSQ_ERR_SUCCESS){
		အကယ်၍(rc == MOSQ_ERR_MALFORMED_PACKET){
			ပြန်ပေး rc;
		}
		goto handle_connect_error;
	}

	rc = read_and_verify_connect_flags(context, &connect_flags);
	အကယ်၍(rc != MOSQ_ERR_SUCCESS){
		goto handle_connect_error;
	}

	clean_start = (connect_flags & 0x02) >> 1;
	set_session_expiry_interval(context, clean_start, protocol_version);

	rc = read_and_reset_keepalive(context);
	အကယ်၍(rc != MOSQ_ERR_SUCCESS){
		goto handle_connect_error;
	}

	rc = read_and_verify_v5_connect_properties(context, &properties, protocol_version);
	အကယ်၍(rc != MOSQ_ERR_SUCCESS){
		goto handle_connect_error;
	}

	will = connect_flags & 0x04;
	will_qos = (connect_flags & 0x18) >> 3;
	will_retain = ((connect_flags & 0x20) == 0x20);
	rc = verify_will_options(context, will, will_qos, will_retain, protocol_version);
	အကယ်၍(rc != MOSQ_ERR_SUCCESS){
		goto handle_connect_error;
	}

	mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_METHOD, &context->auth_method, false);
	mosquitto_property_read_binary(properties, MQTT_PROP_AUTHENTICATION_DATA, &auth_data, &auth_data_len, false);
	mosquitto_property_free_all(&properties);

	အကယ်၍(auth_data && !context->auth_method){
		send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, NULL);
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d CONNECT with missing clientid string.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		goto handle_connect_error;
	}

	rc = read_and_verify_clientid_from_packet(context, &clientid, &allow_zero_length_clientid, clean_start);
	အကယ်၍(rc != MOSQ_ERR_SUCCESS){
		goto handle_connect_error;
	}

	အကယ်၍(will){
		rc = will__read(context, clientid, &will_struct, will_qos, will_retain);
		အကယ်၍(rc){
			အကယ်၍(context->protocol == mosq_p_mqtt5){
				အကယ်၍(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
					send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, NULL);
				}မဟုတ်ပါက အကယ်၍(rc == MOSQ_ERR_MALFORMED_PACKET){
					send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, NULL);
				}
			}
			goto handle_connect_error;
		}
	}မဟုတ်ပါက{
		အကယ်၍(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
			အကယ်၍(will_qos != 0 || will_retain != 0){
				log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT without Will with non-zero QoS (%d) or retain (%d).",
						clientid, will_qos, will_retain);
				rc = MOSQ_ERR_PROTOCOL;
				goto handle_connect_error;
			}
		}
	}

	// Client credentials
	password_flag = connect_flags & 0x40;
	username_flag = connect_flags & 0x80;
	rc = read_and_verify_client_credentials_from_packet(context, &username, username_flag, &password, &password_len, password_flag, clientid);
	အကယ်၍(rc != MOSQ_ERR_SUCCESS){
		goto handle_connect_error;
	}

	rc = check_additional_trailing_data(context, protocol_version);
	အကယ်၍(rc != MOSQ_ERR_SUCCESS){
		goto handle_connect_error;
	}

	/* Once context->id is set, if we return from this function with an error
	 * we must make sure that context->id is freed and set to NULL, so that the
	 * client isn't erroneously removed from the by_id hash table. */
	context->id = clientid;
	clientid = NULL;

	/* use_identity_as_username or use_subject_as_username */
	rc = handle_username_from_cert_options(context, &username, &password, password_len);
	အကယ်၍(rc != MOSQ_ERR_SUCCESS){
		goto handle_connect_error;
	}

	/* use_username_as_clientid */
	အကယ်၍(context->listener->use_username_as_clientid){
		rc = handle_username_as_clientid_option(context);
		အကယ်၍(rc != MOSQ_ERR_SUCCESS){
			goto handle_connect_error;
		}
	}

	/* Check for an existing delayed auth check, reject if present */
	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), found_context);
	အကယ်၍(found_context){
		rc = MOSQ_ERR_UNKNOWN;
		goto handle_connect_error;
	}

	context->clean_start = clean_start;
	context->will = will_struct;
	will_struct = NULL;

	အကယ်၍(context->auth_method){
		rc = mosquitto_security_auth_start(context, false, auth_data, auth_data_len, &auth_data_out, &auth_data_out_len);
		mosquitto_FREE(auth_data);
		အကယ်၍(rc == MOSQ_ERR_SUCCESS){
			ပြန်ပေး connect__on_authorised(context, auth_data_out, auth_data_out_len);
		}မဟုတ်ပါက အကယ်၍(rc == MOSQ_ERR_AUTH_CONTINUE){
			mosquitto__set_state(context, mosq_cs_authenticating);
			rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_data_out, auth_data_out_len);
			SAFE_FREE(auth_data_out);
			ပြန်ပေး rc;
		}မဟုတ်ပါက{
			SAFE_FREE(auth_data_out);
			will__clear(context);
			အကယ်၍(rc == MOSQ_ERR_AUTH){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, NULL);
				mosquitto_FREE(context->id);
				goto handle_connect_error;
			}မဟုတ်ပါက အကယ်၍(rc == MOSQ_ERR_NOT_SUPPORTED){
				/* Client has requested extended authentication, but we don't support it. */
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_METHOD, NULL);
				mosquitto_FREE(context->id);
				goto handle_connect_error;
			}မဟုတ်ပါက{
				mosquitto_FREE(context->id);
				goto handle_connect_error;
			}
		}
	}မဟုတ်ပါက{
#ifdef WITH_TLS
		အကယ်၍(context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
			/* Authentication assumed to be cleared */
		}မဟုတ်ပါက
#endif
		{
			rc = mosquitto_basic_auth(context);
			switch(rc){
				pak_sa MOSQ_ERR_SUCCESS:
					bha_nga;
				pak_sa MOSQ_ERR_AUTH_DELAYED:
					mosquitto__set_state(context, mosq_cs_delayed_auth);
					HASH_ADD_KEYPTR(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context);
					ပြန်ပေး MOSQ_ERR_SUCCESS;
					bha_nga;
				pak_sa MOSQ_ERR_AUTH:
					အကယ်၍(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, NULL);
					}မဟုတ်ပါက{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, NULL);
					}
					goto handle_connect_error;
					bha_nga;
				pak_sa MOSQ_ERR_UNSPECIFIED:
				pak_sa MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
				pak_sa MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
				pak_sa MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
				pak_sa MOSQ_ERR_SERVER_UNAVAILABLE:
				pak_sa MOSQ_ERR_SERVER_BUSY:
				pak_sa MOSQ_ERR_BANNED:
				pak_sa MOSQ_ERR_BAD_AUTHENTICATION_METHOD:
				pak_sa MOSQ_ERR_CONNECTION_RATE_EXCEEDED:
					အကယ်၍(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, (uint8_t)rc, NULL);
					}မဟုတ်ပါက{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, NULL);
					}
					goto handle_connect_error;
					bha_nga;
				default:
					rc = MOSQ_ERR_UNKNOWN;
					goto handle_connect_error;
					bha_nga;
			}
		}
		ပြန်ပေး connect__on_authorised(context, NULL, 0);
	}


handle_connect_error:
	mosquitto_property_free_all(&properties);
	mosquitto_FREE(auth_data);
	mosquitto_FREE(clientid);
	mosquitto_FREE(username);
	mosquitto_FREE(password);
	အကယ်၍(will_struct){
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
	ပြန်ပေး rc;
}
