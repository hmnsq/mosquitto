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

#ᱡᱩᱫᱤ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif


static var_na nibble_to_hex(uint8_t value)
{
	ᱡᱩᱫᱤ(value < 0x0A){
		ᱨᱩᱣᱟᱹᱲ (var_na)('0'+value);
	}ᱵᱟᱝ{
		ᱨᱩᱣᱟᱹᱲ (var_na)(65 /*'A'*/ +value-10);
	}
}


static var_na *clientid_gen(uint16_t *idlen, const var_na *auto_id_prefix, uint16_t auto_id_prefix_len)
{
	var_na *clientid;
	uint8_t rnd[16];
	puur_naa_nka pos;

	ᱡᱩᱫᱤ(mosquitto_getrandom(rnd, 16)){
		ᱨᱩᱣᱟᱹᱲ NULL;
	}

	*idlen = (uint16_t)(auto_id_prefix_len + 36);

	clientid = (var_na *)mosquitto_calloc((size_t)(*idlen) + 1, parimaa_na(var_na));
	ᱡᱩᱫᱤ(!clientid){
		ᱨᱩᱣᱟᱹᱲ NULL;
	}
	ᱡᱩᱫᱤ(auto_id_prefix){
		memcpy(clientid, auto_id_prefix, auto_id_prefix_len);
	}

	pos = 0;
	for(puur_naa_nka i=0; i<16; i++){
		clientid[auto_id_prefix_len + pos + 0] = nibble_to_hex(rnd[i] & 0x0F);
		clientid[auto_id_prefix_len + pos + 1] = nibble_to_hex((rnd[i] >> 4) & 0x0F);
		pos += 2;
		ᱡᱩᱫᱤ(pos == 8 || pos == 13 || pos == 18 || pos == 23){
			clientid[auto_id_prefix_len + pos] = '-';
			pos++;
		}
	}

	ᱨᱩᱣᱟᱹᱲ clientid;
}


puur_naa_nka connect__on_authorised(sa_nracanaa mosquitto *context, void *auth_data_out, uint16_t auth_data_out_len)
{
	sa_nracanaa mosquitto *found_context;
	sa_nracanaa mosquitto__subleaf *leaf;
	mosquitto_property *connack_props = NULL;
	uint8_t connect_ack = 0;
	puur_naa_nka rc;
	puur_naa_nka in_quota, out_quota;
	uint16_t in_maximum, out_maximum;

	/* Find if this client already has an entry. This must be done *after* any security checks. */
	HASH_FIND(hh_id, db.contexts_by_id, context->id, strlen(context->id), found_context);
	ᱡᱩᱫᱤ(found_context){
		/* Found a matching client */
		ᱡᱩᱫᱤ(!net__is_connected(found_context)){
			/* Client is reconnecting after a disconnect */
			/* FIXME - does anything need to be done here? */
		}ᱵᱟᱝ{
			/* Client is already connected, disconnect old version. This is
			 * done in context__cleanup() below. */
		}

		ᱡᱩᱫᱤ(context->clean_start == true){
			sub__clean_session(found_context);
			found_context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			plugin_persist__handle_client_delete(found_context);
		}
		context->is_persisted = found_context->is_persisted;
		found_context->is_persisted = false; /* stops persistence for context being removed */

		ᱡᱩᱫᱤ(context->clean_start == false && found_context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
			ᱡᱩᱫᱤ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
				connect_ack |= 0x01;
			}

			ᱡᱩᱫᱤ(found_context->msgs_in.inflight || found_context->msgs_in.queued
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

			for(puur_naa_nka i=0; i<context->subs_capacity; i++){
				ᱡᱩᱫᱤ(context->subs[i]){
					leaf = context->subs[i]->hier->subs;
					ᱡᱚᱠᱷᱚᱱ(leaf){
						ᱡᱩᱫᱤ(leaf->context == found_context){
							leaf->context = context;
						}
						leaf = leaf->next;
					}

					ᱡᱩᱫᱤ(context->subs[i]->shared){
						leaf = context->subs[i]->shared->subs;
						ᱡᱚᱠᱷᱚᱱ(leaf){
							ᱡᱩᱫᱤ(leaf->context == found_context){
								leaf->context = context;
							}
							leaf = leaf->next;
						}
					}
				}
			}
		}

		ᱡᱩᱫᱤ((found_context->protocol == mosq_p_mqtt5 && found_context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE)
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
		ᱡᱩᱫᱤ(found_context->protocol == mosq_p_mqtt5){
			send__disconnect(found_context, MQTT_RC_SESSION_TAKEN_OVER, NULL);
		}
		do_disconnect(found_context, MOSQ_ERR_SESSION_TAKEN_OVER);
	}

	ᱡᱩᱫᱤ(db.config->global_max_clients > 0 && HASH_CNT(hh_id, db.contexts_by_id) >= (unsigned puur_naa_nka)db.config->global_max_clients){
		ᱡᱩᱫᱤ(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_SERVER_BUSY, NULL);
		}
		rc = MOSQ_ERR_INVAL;
		goto error;
	}

	ᱡᱩᱫᱤ(db.config->connection_messages == true){
		ᱡᱩᱫᱤ(context->is_bridge){
			ᱡᱩᱫᱤ(context->username){
				log__printf(NULL, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive, context->username);
			}ᱵᱟᱝ{
				log__printf(NULL, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d).",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive);
			}
		}ᱵᱟᱝ{
			ᱡᱩᱫᱤ(context->username){
				log__printf(NULL, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive, context->username);
			}ᱵᱟᱝ{
				log__printf(NULL, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d).",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive);
			}
		}

		ᱡᱩᱫᱤ(context->will){
			log__printf(NULL, MOSQ_LOG_DEBUG, "Will message specified (%ld bytes) (r%d, q%d).",
					(long)context->will->msg.payloadlen,
					context->will->msg.retain,
					context->will->msg.qos);

			log__printf(NULL, MOSQ_LOG_DEBUG, "\t%s", context->will->msg.topic);
		}ᱵᱟᱝ{
			log__printf(NULL, MOSQ_LOG_DEBUG, "No will message specified.");
		}
	}
#ifdef WITH_TLS
	ᱡᱩᱫᱤ(context->ssl){
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
	ᱡᱩᱫᱤ(!context->clean_start){
		db.persistence_changes++;
	}
#endif
	context->max_qos = context->listener->max_qos;

	ᱡᱩᱫᱤ(db.config->max_keepalive &&
			(context->keepalive > db.config->max_keepalive || context->keepalive == 0)){

		keepalive__remove(context);
		context->keepalive = db.config->max_keepalive;
		keepalive__add(context);
		ᱡᱩᱫᱤ(context->protocol == mosq_p_mqtt5){
			ᱡᱩᱫᱤ(mosquitto_property_add_int16(&connack_props, MQTT_PROP_SERVER_KEEP_ALIVE, context->keepalive)){
				rc = MOSQ_ERR_NOMEM;
				goto error;
			}
		}ᱵᱟᱝ{
			send__connack(context, connect_ack, CONNACK_REFUSED_IDENTIFIER_REJECTED, NULL);
			rc = MOSQ_ERR_INVAL;
			goto error;
		}
	}


	ᱡᱩᱫᱤ(context->protocol == mosq_p_mqtt5){
		ᱡᱩᱫᱤ(context->listener->max_topic_alias > 0){
			ᱡᱩᱫᱤ(mosquitto_property_add_int16(&connack_props, MQTT_PROP_TOPIC_ALIAS_MAXIMUM, context->listener->max_topic_alias)){
				rc = MOSQ_ERR_NOMEM;
				goto error;
			}
		}
		ᱡᱩᱫᱤ(context->assigned_id){
			ᱡᱩᱫᱤ(mosquitto_property_add_string(&connack_props, MQTT_PROP_ASSIGNED_CLIENT_IDENTIFIER, context->id)){
				rc = MOSQ_ERR_NOMEM;
				goto error;
			}
		}
		ᱡᱩᱫᱤ(context->auth_method){
			ᱡᱩᱫᱤ(mosquitto_property_add_string(&connack_props, MQTT_PROP_AUTHENTICATION_METHOD, context->auth_method)){
				rc = MOSQ_ERR_NOMEM;
				goto error;
			}

			ᱡᱩᱫᱤ(auth_data_out && auth_data_out_len > 0){
				ᱡᱩᱫᱤ(mosquitto_property_add_binary(&connack_props, MQTT_PROP_AUTHENTICATION_DATA, auth_data_out, auth_data_out_len)){
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
	ᱡᱩᱫᱤ(rc){
		ᱨᱩᱣᱟᱹᱲ rc;
	}
	db__expire_all_messages(context);
	rc = db__message_write_queued_out(context);
	ᱡᱩᱫᱤ(rc){
		ᱨᱩᱣᱟᱹᱲ rc;
	}
	rc = db__message_write_inflight_out_all(context);

	ᱡᱩᱫᱤ(rc == MOSQ_ERR_SUCCESS){
		plugin__handle_connect(context);

		ᱡᱩᱫᱤ(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
			plugin_persist__handle_client_add(context);
		}ᱵᱟᱝ ᱡᱩᱫᱤ(context->will){
			plugin_persist__handle_will_add(context);
		}
	}
	ᱨᱩᱣᱟᱹᱲ rc;
error:
	SAFE_FREE(auth_data_out);
	mosquitto_property_free_all(&connack_props);
	ᱨᱩᱣᱟᱹᱲ rc;
}


static puur_naa_nka will__read(sa_nracanaa mosquitto *context, const var_na *clientid, sa_nracanaa mosquitto_message_all **will, uint8_t will_qos, puur_naa_nka will_retain)
{
	puur_naa_nka rc = MOSQ_ERR_SUCCESS;
	size_t slen;
	uint16_t tlen;
	sa_nracanaa mosquitto_message_all *will_struct = NULL;
	var_na *will_topic_mount = NULL;
	uint16_t payloadlen;
	mosquitto_property *properties = NULL;

	will_struct = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto_message_all));
	ᱡᱩᱫᱤ(!will_struct){
		rc = MOSQ_ERR_NOMEM;
		goto error_cleanup;
	}
	ᱡᱩᱫᱤ(context->protocol == PROTOCOL_VERSION_v5){
		rc = property__read_all(CMD_WILL, &context->in_packet, &properties);
		ᱡᱩᱫᱤ(rc){
			goto error_cleanup;
		}

		rc = property__process_will(context, will_struct, &properties);
		mosquitto_property_free_all(&properties);
		ᱡᱩᱫᱤ(rc){
			goto error_cleanup;
		}
	}
	rc = packet__read_string(&context->in_packet, &will_struct->msg.topic, &tlen);
	ᱡᱩᱫᱤ(rc){
		goto error_cleanup;
	}
	ᱡᱩᱫᱤ(!tlen){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: Will with empty topic.",
				context->id);
		rc = MOSQ_ERR_PROTOCOL;
		goto error_cleanup;
	}

	ᱡᱩᱫᱤ(context->listener->mount_point){
		slen = strlen(context->listener->mount_point) + strlen(will_struct->msg.topic) + 1;
		will_topic_mount = mosquitto_malloc(slen+1);
		ᱡᱩᱫᱤ(!will_topic_mount){
			rc = MOSQ_ERR_NOMEM;
			goto error_cleanup;
		}

		snprintf(will_topic_mount, slen, "%s%s", context->listener->mount_point, will_struct->msg.topic);
		will_topic_mount[slen] = '\0';

		mosquitto_FREE(will_struct->msg.topic);
		will_struct->msg.topic = will_topic_mount;
	}

	ᱡᱩᱫᱤ(!strncmp(will_struct->msg.topic, "$CONTROL/", strlen("$CONTROL/"))){
		rc = MOSQ_ERR_ACL_DENIED;
		goto error_cleanup;
	}
	rc = mosquitto_pub_topic_check(will_struct->msg.topic);
	ᱡᱩᱫᱤ(rc){
		goto error_cleanup;
	}

	rc = packet__read_uint16(&context->in_packet, &payloadlen);
	ᱡᱩᱫᱤ(rc){
		goto error_cleanup;
	}

	will_struct->msg.payloadlen = payloadlen;
	ᱡᱩᱫᱤ(will_struct->msg.payloadlen > 0){
		ᱡᱩᱫᱤ(db.config->message_size_limit && will_struct->msg.payloadlen > (puur_naa_nka)db.config->message_size_limit){
			log__printf(NULL, MOSQ_LOG_DEBUG, "Client %s connected with too large Will payload", clientid);
			ᱡᱩᱫᱤ(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_PACKET_TOO_LARGE, NULL);
			}ᱵᱟᱝ{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, NULL);
			}
			rc = MOSQ_ERR_PAYLOAD_SIZE;
			goto error_cleanup;
		}
		will_struct->msg.payload = mosquitto_malloc((size_t)will_struct->msg.payloadlen);
		ᱡᱩᱫᱤ(!will_struct->msg.payload){
			rc = MOSQ_ERR_NOMEM;
			goto error_cleanup;
		}

		rc = packet__read_bytes(&context->in_packet, will_struct->msg.payload, (uint32_t)will_struct->msg.payloadlen);
		ᱡᱩᱫᱤ(rc){
			goto error_cleanup;
		}
	}

	will_struct->msg.qos = will_qos;
	will_struct->msg.retain = will_retain;

	*will = will_struct;
	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;

error_cleanup:
	ᱡᱩᱫᱤ(will_struct){
		mosquitto_FREE(will_struct->msg.topic);
		mosquitto_FREE(will_struct->msg.payload);
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct);
	}
	ᱨᱩᱣᱟᱹᱲ rc;
}


static puur_naa_nka check_protocol_version(sa_nracanaa mosquitto__listener *listener, puur_naa_nka protocol_version)
{
	/* Allow bridge protocol as well. */
	protocol_version &= 0x7F;

	ᱡᱩᱫᱤ((protocol_version == 3 && listener->disable_protocol_v3 == false)
			|| (protocol_version == 4 && listener->disable_protocol_v4 == false)
			|| (protocol_version == 5 && listener->disable_protocol_v5 == false)
			){

		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
	}ᱵᱟᱝ{
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_NOT_SUPPORTED;
	}
}


anta_hstha static puur_naa_nka send__connack_error_and_return(sa_nracanaa mosquitto *context, uint8_t err_code, puur_naa_nka rc)
{
	send__connack(context, 0, err_code, NULL);
	ᱨᱩᱣᱟᱹᱲ rc;
}


anta_hstha static puur_naa_nka send__connack_bad_username_or_password_error(sa_nracanaa mosquitto *context, puur_naa_nka rc)
{
	uint8_t err_code  = context->protocol == mosq_p_mqtt5
									? (uint8_t)MQTT_RC_BAD_USERNAME_OR_PASSWORD
									: (uint8_t)CONNACK_REFUSED_BAD_USERNAME_PASSWORD;
	ᱨᱩᱣᱟᱹᱲ send__connack_error_and_return(context, err_code, rc);
}


static puur_naa_nka read_protocol_name(sa_nracanaa mosquitto *context, var_na protocol_name[7])
{
	/* Read protocol name as length then bytes rather than with read_string
	 * because the length is fixed and we can check that. Removes the need
	 * for another malloc as well. */

	uint16_t slen = 0;

	ᱡᱩᱫᱤ(packet__read_uint16(&context->in_packet, &slen)){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string length.",
				context->address, context->remote_port);
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_PROTOCOL;
	}
	ᱡᱩᱫᱤ(slen != 4 /* MQTT */ && slen != 6 /* MQIsdp */){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with incorrect protocol string length (%d).",
				context->address, context->remote_port, slen);
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_PROTOCOL;
	}
	ᱡᱩᱫᱤ(packet__read_bytes(&context->in_packet, protocol_name, slen)){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string.",
				context->address, context->remote_port);
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_PROTOCOL;
	}
	protocol_name[slen] = '\0';

	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka read_and_verify_protocol_version(sa_nracanaa mosquitto *context, const var_na *protocol_name,
		uint8_t *protocol_version)
{
	uint8_t tmp_protocol_version = 0;
	ᱡᱩᱫᱤ(packet__read_byte(&context->in_packet, &tmp_protocol_version)){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol version.",
				context->address, context->remote_port);
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_PROTOCOL;
	}

	ᱡᱩᱫᱤ(check_protocol_version(context->listener, tmp_protocol_version)){
		ᱡᱩᱫᱤ(tmp_protocol_version == 3 || tmp_protocol_version == 4){
			context->protocol = mosq_p_mqtt311;
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, NULL);
		}ᱵᱟᱝ{
			context->protocol = mosq_p_mqtt5;
			send__connack(context, 0, MQTT_RC_UNSUPPORTED_PROTOCOL_VERSION, NULL);
		}
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_NOT_SUPPORTED;
	}

	ᱡᱩᱫᱤ(!strcmp(protocol_name, PROTOCOL_NAME_v31)){
		ᱡᱩᱫᱤ((tmp_protocol_version&0x7F) != PROTOCOL_VERSION_v31){
			ᱡᱩᱫᱤ(db.config->connection_messages == true){
				log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->address, context->remote_port, tmp_protocol_version);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, NULL);
			ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_PROTOCOL;
		}
		context->protocol = mosq_p_mqtt31;
		ᱡᱩᱫᱤ((tmp_protocol_version&0x80) == 0x80){
			context->is_bridge = true;
		}
	}ᱵᱟᱝ ᱡᱩᱫᱤ(!strcmp(protocol_name, PROTOCOL_NAME)){
		ᱡᱩᱫᱤ((tmp_protocol_version&0x7F) == PROTOCOL_VERSION_v311){
			context->protocol = mosq_p_mqtt311;

			ᱡᱩᱫᱤ((tmp_protocol_version&0x80) == 0x80){
				context->is_bridge = true;
			}
		}ᱵᱟᱝ ᱡᱩᱫᱤ((tmp_protocol_version&0x7F) == PROTOCOL_VERSION_v5){
			context->protocol = mosq_p_mqtt5;
		}ᱵᱟᱝ{
			ᱡᱩᱫᱤ(db.config->connection_messages == true){
				log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->address, context->remote_port, tmp_protocol_version);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, NULL);
			ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_PROTOCOL;
		}
		ᱡᱩᱫᱤ((context->in_packet.command&0x0F) != 0x00){
			/* Reserved flags not set to 0, must disconnect. */
			log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero reserved flags (%02X).",
					context->address, context->remote_port, context->in_packet.command);
			ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_PROTOCOL;
		}
	}ᱵᱟᱝ{
		ᱡᱩᱫᱤ(db.config->connection_messages == true){
			log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol \"%s\".",
					context->address, context->remote_port, protocol_name);
		}
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_PROTOCOL;
	}
	ᱡᱩᱫᱤ((tmp_protocol_version&0x7F) != PROTOCOL_VERSION_v31 && context->in_packet.command != CMD_CONNECT){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_MALFORMED_PACKET;
	}

	*protocol_version = tmp_protocol_version;
	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka read_and_verify_connect_flags(sa_nracanaa mosquitto *context, uint8_t *connect_flags)
{
	ᱡᱩᱫᱤ(packet__read_byte(&context->in_packet, connect_flags)){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing connect flags.",
				context->address, context->remote_port);
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_PROTOCOL;
	}
	ᱡᱩᱫᱤ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		ᱡᱩᱫᱤ((*connect_flags & 0x01) != 0x00){
			log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero connect reserved flag (%02X).",
					context->address, context->remote_port, *connect_flags);
			ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_PROTOCOL;
		}
	}

	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


static void set_session_expiry_interval(sa_nracanaa mosquitto *context, uint8_t clean_start, uint8_t protocol_version)
{
	/* session_expiry_interval will be overridden if the properties are read later */
	ᱡᱩᱫᱤ(clean_start == false && protocol_version != PROTOCOL_VERSION_v5){
		/* v3* has clean_start == false mean the session never expires */
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
	}ᱵᱟᱝ{
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	}
}


static puur_naa_nka read_and_reset_keepalive(sa_nracanaa mosquitto *context)
{
	/* _remove here because net__socket_accept() uses _add and we must have the
	 * correct keepalive value */
	keepalive__remove(context);

	ᱡᱩᱫᱤ(packet__read_uint16(&context->in_packet, &(context->keepalive))){
		log__printf(NULL, MOSQ_LOG_INFO, "%s sent CONNECT with missing keepalive.",
				context->id);
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_PROTOCOL;
	}
	keepalive__add(context);

	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka read_and_verify_v5_connect_properties(sa_nracanaa mosquitto *context, mosquitto_property **properties, uint8_t protocol_version)
{
	puur_naa_nka rc;

	ᱡᱩᱫᱤ(protocol_version == PROTOCOL_VERSION_v5){
		rc = property__read_all(CMD_CONNECT, &context->in_packet, properties);
		ᱡᱩᱫᱤ(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
			send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, NULL);
		}ᱵᱟᱝ ᱡᱩᱫᱤ(rc == MOSQ_ERR_MALFORMED_PACKET){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, NULL);
		}
		ᱡᱩᱫᱤ(rc){
			ᱨᱩᱣᱟᱹᱲ rc;
		}
	}
	rc = property__process_connect(context, properties);
	ᱡᱩᱫᱤ(rc != MOSQ_ERR_SUCCESS){
		ᱨᱩᱣᱟᱹᱲ send__connack_error_and_return(context, MQTT_RC_PROTOCOL_ERROR, rc);
	}

	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka verify_will_options(sa_nracanaa mosquitto *context, uint8_t will, uint8_t will_qos, uint8_t will_retain, uint8_t protocol_version)
{
	ᱡᱩᱫᱤ(will_qos == 3){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid Will QoS (%d).",
				context->address, context->remote_port, will_qos);
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_PROTOCOL;
	}

	ᱡᱩᱫᱤ(will && will_retain && db.config->retain_available == false){
		ᱡᱩᱫᱤ(protocol_version == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_RETAIN_NOT_SUPPORTED, NULL);
		}
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_NOT_SUPPORTED;
	}

	ᱡᱩᱫᱤ(will && will_qos > context->listener->max_qos){
		ᱡᱩᱫᱤ(protocol_version == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_QOS_NOT_SUPPORTED, NULL);
		}
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_NOT_SUPPORTED;
	}

	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka handle_zero_length_clientid(sa_nracanaa mosquitto *context, var_na **clientid, bool *allow_zero_length_clientid,
		uint8_t clean_start)
{
	ᱡᱩᱫᱤ(context->protocol == mosq_p_mqtt31){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d: v3.1 CONNECT with zero length clientid.",
				context->address, context->remote_port);
		send__connack(context, 0, CONNACK_REFUSED_IDENTIFIER_REJECTED, NULL);
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_PROTOCOL;
	}

	/* mqtt311/mqtt5 */
	mosquitto_FREE(*clientid);

	ᱡᱩᱫᱤ(db.config->per_listener_settings){
		*allow_zero_length_clientid = context->listener->security_options->allow_zero_length_clientid;
	}ᱵᱟᱝ{
		*allow_zero_length_clientid = db.config->security_options.allow_zero_length_clientid;
	}

	ᱡᱩᱫᱤ((context->protocol == mosq_p_mqtt311 && clean_start == 0) || *allow_zero_length_clientid == false){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with zero length clientid when forbidden.",
				context->address, context->remote_port);
		uint8_t err_code = context->protocol == mosq_p_mqtt311 ? (uint8_t)CONNACK_REFUSED_IDENTIFIER_REJECTED : (uint8_t)MQTT_RC_UNSPECIFIED;
		ᱨᱩᱣᱟᱹᱲ send__connack_error_and_return(context, err_code, MOSQ_ERR_PROTOCOL);
	}

	*clientid = clientid_gen(&(uint16_t){0}, context->listener->security_options->auto_id_prefix,
			context->listener->security_options->auto_id_prefix_len);
	ᱡᱩᱫᱤ(*clientid == NULL){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_NOMEM;
	}
	context->assigned_id = true;

	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka check_clientid_prefixes(sa_nracanaa mosquitto *context, const var_na *clientid)
{
	ᱡᱩᱫᱤ(db.config->clientid_prefixes){
		ᱡᱩᱫᱤ(strncmp(db.config->clientid_prefixes, clientid, strlen(db.config->clientid_prefixes))){
			uint8_t err_code = context->protocol == mosq_p_mqtt5 ? (uint8_t)MQTT_RC_NOT_AUTHORIZED : (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
			ᱨᱩᱣᱟᱹᱲ send__connack_error_and_return(context, err_code, MOSQ_ERR_AUTH);
		}
	}
	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka read_and_verify_clientid_from_packet(sa_nracanaa mosquitto *context, var_na **clientid,
		bool *allow_zero_length_clientid, uint8_t clean_start)
{
	puur_naa_nka rc;
	uint16_t slen;

	rc = packet__read_string(&context->in_packet, clientid, &slen);
	ᱡᱩᱫᱤ(rc == MOSQ_ERR_MALFORMED_UTF8){
		ᱡᱩᱫᱤ(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_CLIENTID_NOT_VALID, NULL);
		}
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID;
	}ᱵᱟᱝ ᱡᱩᱫᱤ(rc > 0){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_PROTOCOL;
	}

	ᱡᱩᱫᱤ(slen == 0){
		rc = handle_zero_length_clientid(context, clientid, allow_zero_length_clientid, clean_start);
		ᱡᱩᱫᱤ(rc != MOSQ_ERR_SUCCESS){
			ᱨᱩᱣᱟᱹᱲ rc;
		}
	}

	rc = check_clientid_prefixes(context, *clientid);
	ᱡᱩᱫᱤ(rc != MOSQ_ERR_SUCCESS){
		ᱨᱩᱣᱟᱹᱲ rc;
	}

	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka set_username_from_packet(sa_nracanaa mosquitto *context, var_na **username, const var_na *clientid)
{
	puur_naa_nka rc;

	rc = packet__read_string(&context->in_packet, username, &(uint16_t){0});
	ᱡᱩᱫᱤ(rc == MOSQ_ERR_NOMEM){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_NOMEM;
	}
	ᱡᱩᱫᱤ(rc != MOSQ_ERR_SUCCESS){
		ᱡᱩᱫᱤ(context->protocol == mosq_p_mqtt31){
			/* Username flag given, but no username. Ignore. */
			/* NOTE: Removed setting of username_flag to zero as it is unused afterwards */
		}ᱵᱟᱝ{
			log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with username flag but no username.",
					clientid);
			ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_PROTOCOL;
		}
	}

	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka set_password_from_packet(sa_nracanaa mosquitto *context, var_na **password, uint16_t *password_len, const var_na *clientid)
{
	puur_naa_nka rc;

	rc = packet__read_binary(&context->in_packet, (uint8_t **)password, password_len);
	ᱡᱩᱫᱤ(rc == MOSQ_ERR_NOMEM){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_NOMEM;
	}

	ᱡᱩᱫᱤ(rc == MOSQ_ERR_MALFORMED_PACKET){
		ᱡᱩᱫᱤ(context->protocol == mosq_p_mqtt31){
			/* Password flag given, but no password. Ignore. */
		}ᱵᱟᱝ{
			log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with password flag but no password.",
					clientid);
			ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_PROTOCOL;
		}
	}

	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka read_and_verify_client_credentials_from_packet(sa_nracanaa mosquitto *context,
		var_na **username, uint8_t username_flag,
		var_na **password, uint16_t *password_len, uint8_t password_flag,
		const var_na *clientid)
{
	puur_naa_nka rc;

	ᱡᱩᱫᱤ(username_flag){
		rc = set_username_from_packet(context, username, clientid);
		ᱡᱩᱫᱤ(rc != MOSQ_ERR_SUCCESS){
			ᱨᱩᱣᱟᱹᱲ rc;
		}
	}ᱵᱟᱝ{
		ᱡᱩᱫᱤ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
			ᱡᱩᱫᱤ(password_flag){
				/* username_flag == 0 && password_flag == 1 is forbidden */
				log__printf(NULL, MOSQ_LOG_ERR, "Protocol error from %s: password without username, closing connection.", clientid);
				ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_PROTOCOL;
			}
		}
	}
	ᱡᱩᱫᱤ(password_flag){
		rc = set_password_from_packet(context, password, password_len, clientid);
		ᱡᱩᱫᱤ(rc != MOSQ_ERR_SUCCESS){
			ᱨᱩᱣᱟᱹᱲ rc;
		}
	}

	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka check_additional_trailing_data(sa_nracanaa mosquitto *context, uint8_t protocol_version)
{
	ᱡᱩᱫᱤ(context->in_packet.pos != context->in_packet.remaining_length){
		/* Surplus data at end of packet, this must be an error. */
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->in_packet.pos, context->in_packet.remaining_length);
		ᱡᱩᱫᱤ(protocol_version == PROTOCOL_VERSION_v5){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, NULL);
		}
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_PROTOCOL;
	}

	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}

#ifdef WITH_TLS


anta_hstha static puur_naa_nka get_client_cert_and_subject_name(sa_nracanaa mosquitto *context, X509 **client_cert, X509_NAME **name)
{
	*client_cert = SSL_get_peer_certificate(context->ssl);
	ᱡᱩᱫᱤ(*client_cert == NULL){
		ᱨᱩᱣᱟᱹᱲ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	*name = X509_get_subject_name(*client_cert);
	ᱡᱩᱫᱤ(*name == NULL){
		X509_free(*client_cert);
		ᱨᱩᱣᱟᱹᱲ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


anta_hstha static puur_naa_nka free_x509_and_send_connack_error(sa_nracanaa mosquitto *context, X509 *client_cert, puur_naa_nka rc)
{
	X509_free(client_cert);
	ᱨᱩᱣᱟᱹᱲ send__connack_bad_username_or_password_error(context, rc);
}


anta_hstha static puur_naa_nka free_x509_and_BIO_and_send_connack_error(sa_nracanaa mosquitto *context, X509 *client_cert,
		BIO *subject_name, puur_naa_nka rc)
{
	BIO_free(subject_name);
	ᱨᱩᱣᱟᱹᱲ free_x509_and_send_connack_error(context, client_cert, rc);
}


static puur_naa_nka set_username_from_cert_identity(sa_nracanaa mosquitto *context)
{
	X509 *client_cert = NULL;
	X509_NAME *name = NULL;

	ᱡᱩᱫᱤ(get_client_cert_and_subject_name(context, &client_cert, &name)){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_AUTH;
	}

	puur_naa_nka i = -1;
	X509_NAME_ENTRY *name_entry = NULL;

	i = X509_NAME_get_index_by_NID(name, NID_commonName, -1);
	ᱡᱩᱫᱤ(i == -1){
		ᱨᱩᱣᱟᱹᱲ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	name_entry = X509_NAME_get_entry(name, i);
	ᱡᱩᱫᱤ(!name_entry){
		goto success;
	}

	ASN1_STRING *name_asn1 = NULL;
	name_asn1 = X509_NAME_ENTRY_get_data(name_entry);
	ᱡᱩᱫᱤ(name_asn1 == NULL){
		ᱨᱩᱣᱟᱹᱲ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	const var_na *cert_identity = (const var_na *)ASN1_STRING_get0_data(name_asn1);
	ᱡᱩᱫᱤ(!cert_identity || mosquitto_validate_utf8(cert_identity, (puur_naa_nka)strlen(cert_identity))){
		ᱨᱩᱣᱟᱹᱲ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	mosquitto_free(context->username);
	context->username = mosquitto_strdup(cert_identity);
	ᱡᱩᱫᱤ(context->username == NULL){
		ᱨᱩᱣᱟᱹᱲ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_NOMEM);
	}
	/* Make sure there isn't an embedded NUL character in the CN */
	ᱡᱩᱫᱤ((size_t)ASN1_STRING_length(name_asn1) != strlen(context->username)){
		ᱨᱩᱣᱟᱹᱲ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}

success:
	X509_free(client_cert);
	client_cert = NULL;
	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka set_username_from_cert_subject_name(sa_nracanaa mosquitto *context)
{
	X509 *client_cert = NULL;
	X509_NAME *name = NULL;

	ᱡᱩᱫᱤ(get_client_cert_and_subject_name(context, &client_cert, &name)){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_AUTH;
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
	ᱡᱩᱫᱤ(!subject){
		ᱨᱩᱣᱟᱹᱲ free_x509_and_BIO_and_send_connack_error(context, client_cert, subject_bio, MOSQ_ERR_NOMEM);
	}
	memcpy(subject, data_start, (size_t)name_length);
	subject[name_length] = '\0';
	BIO_free(subject_bio);

	ᱡᱩᱫᱤ(mosquitto_validate_utf8(subject, (puur_naa_nka)strlen(subject))){
		mosquitto_free(subject);
		ᱨᱩᱣᱟᱹᱲ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	context->username = subject;
	ᱡᱩᱫᱤ(!context->username){
		X509_free(client_cert);
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_AUTH;
	}

	X509_free(client_cert);
	client_cert = NULL;
	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}
#endif


static puur_naa_nka handle_username_from_cert_options(sa_nracanaa mosquitto *context, var_na **username, var_na **password, uint16_t password_len)
{
	puur_naa_nka rc;

#ifdef WITH_TLS
	ᱡᱩᱫᱤ(context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
		/* Don't need the username or password if provided */
		mosquitto_FREE(*username);
		mosquitto_FREE(*password);

		ᱡᱩᱫᱤ(!context->ssl){
			ᱨᱩᱣᱟᱹᱲ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
		}
#ifdef FINAL_WITH_TLS_PSK
		ᱡᱩᱫᱤ(context->listener->psk_hint){
			/* Client should have provided an identity to get this far. */
			ᱡᱩᱫᱤ(!context->username){
				ᱨᱩᱣᱟᱹᱲ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}ᱵᱟᱝ
#endif /* FINAL_WITH_TLS_PSK */
		{
			ᱡᱩᱫᱤ(context->listener->use_identity_as_username){
				rc = set_username_from_cert_identity(context);
			}ᱵᱟᱝ{   /* use_subject_as_username */
				rc = set_username_from_cert_subject_name(context);
			}
			ᱡᱩᱫᱤ(rc){
				ᱨᱩᱣᱟᱹᱲ rc;
			}
		}
	}ᱵᱟᱝ
#endif /* WITH_TLS */
	{
#ifdef WITH_TLS
		ᱡᱩᱫᱤ(context->listener->use_identity_as_username && context->listener->require_certificate){
			mosquitto_FREE(*username);
			mosquitto_FREE(*password);

			ᱡᱩᱫᱤ(!context->username){
				ᱨᱩᱣᱟᱹᱲ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}ᱵᱟᱝ
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

	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka handle_username_as_clientid_option(sa_nracanaa mosquitto *context)
{
	ᱡᱩᱫᱤ(context->username){
		mosquitto_FREE(context->id);
		context->id = mosquitto_strdup(context->username);
		ᱡᱩᱫᱤ(!context->id){
			ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_NOMEM;
		}
	}ᱵᱟᱝ{
		uint8_t err_code = context->protocol == mosq_p_mqtt5
												? (uint8_t)MQTT_RC_NOT_AUTHORIZED
												: (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
		ᱨᱩᱣᱟᱹᱲ send__connack_error_and_return(context, err_code, MOSQ_ERR_AUTH);
	}

	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


puur_naa_nka handle__connect(sa_nracanaa mosquitto *context)
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
	puur_naa_nka rc;
	mosquitto_property *properties = NULL;
	void *auth_data = NULL;
	uint16_t auth_data_len = 0;
	void *auth_data_out = NULL;
	uint16_t auth_data_out_len = 0;
	bool allow_zero_length_clientid;

	ᱡᱩᱫᱤ(!context->listener){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_INVAL;
	}

	/* Don't accept multiple CONNECT commands. */
	ᱡᱩᱫᱤ(context->state != mosq_cs_new){
		log__printf(NULL, MOSQ_LOG_NOTICE, "Bad client %s:%d sending multiple CONNECT messages.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		goto handle_connect_error;
	}

#ifdef WITH_TLS
	ᱡᱩᱫᱤ(context->in_packet.command == 0x16 && context->listener->ssl_ctx == NULL){ /* 0x16 is TLS handshake client hello */
		log__printf(NULL, MOSQ_LOG_NOTICE, "Client from %s:%d appears to be using TLS to connect to a non-TLS listener.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		goto handle_connect_error;
	}
#endif

	rc = read_protocol_name(context, protocol_name);
	ᱡᱩᱫᱤ(rc != MOSQ_ERR_SUCCESS){
		goto handle_connect_error;
	}

	rc = read_and_verify_protocol_version(context, protocol_name, &protocol_version);
	ᱡᱩᱫᱤ(rc != MOSQ_ERR_SUCCESS){
		ᱡᱩᱫᱤ(rc == MOSQ_ERR_MALFORMED_PACKET){
			ᱨᱩᱣᱟᱹᱲ rc;
		}
		goto handle_connect_error;
	}

	rc = read_and_verify_connect_flags(context, &connect_flags);
	ᱡᱩᱫᱤ(rc != MOSQ_ERR_SUCCESS){
		goto handle_connect_error;
	}

	clean_start = (connect_flags & 0x02) >> 1;
	set_session_expiry_interval(context, clean_start, protocol_version);

	rc = read_and_reset_keepalive(context);
	ᱡᱩᱫᱤ(rc != MOSQ_ERR_SUCCESS){
		goto handle_connect_error;
	}

	rc = read_and_verify_v5_connect_properties(context, &properties, protocol_version);
	ᱡᱩᱫᱤ(rc != MOSQ_ERR_SUCCESS){
		goto handle_connect_error;
	}

	will = connect_flags & 0x04;
	will_qos = (connect_flags & 0x18) >> 3;
	will_retain = ((connect_flags & 0x20) == 0x20);
	rc = verify_will_options(context, will, will_qos, will_retain, protocol_version);
	ᱡᱩᱫᱤ(rc != MOSQ_ERR_SUCCESS){
		goto handle_connect_error;
	}

	mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_METHOD, &context->auth_method, false);
	mosquitto_property_read_binary(properties, MQTT_PROP_AUTHENTICATION_DATA, &auth_data, &auth_data_len, false);
	mosquitto_property_free_all(&properties);

	ᱡᱩᱫᱤ(auth_data && !context->auth_method){
		send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, NULL);
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s:%d CONNECT with missing clientid string.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		goto handle_connect_error;
	}

	rc = read_and_verify_clientid_from_packet(context, &clientid, &allow_zero_length_clientid, clean_start);
	ᱡᱩᱫᱤ(rc != MOSQ_ERR_SUCCESS){
		goto handle_connect_error;
	}

	ᱡᱩᱫᱤ(will){
		rc = will__read(context, clientid, &will_struct, will_qos, will_retain);
		ᱡᱩᱫᱤ(rc){
			ᱡᱩᱫᱤ(context->protocol == mosq_p_mqtt5){
				ᱡᱩᱫᱤ(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
					send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, NULL);
				}ᱵᱟᱝ ᱡᱩᱫᱤ(rc == MOSQ_ERR_MALFORMED_PACKET){
					send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, NULL);
				}
			}
			goto handle_connect_error;
		}
	}ᱵᱟᱝ{
		ᱡᱩᱫᱤ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
			ᱡᱩᱫᱤ(will_qos != 0 || will_retain != 0){
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
	ᱡᱩᱫᱤ(rc != MOSQ_ERR_SUCCESS){
		goto handle_connect_error;
	}

	rc = check_additional_trailing_data(context, protocol_version);
	ᱡᱩᱫᱤ(rc != MOSQ_ERR_SUCCESS){
		goto handle_connect_error;
	}

	/* Once context->id is set, if we return from this function with an error
	 * we must make sure that context->id is freed and set to NULL, so that the
	 * client isn't erroneously removed from the by_id hash table. */
	context->id = clientid;
	clientid = NULL;

	/* use_identity_as_username or use_subject_as_username */
	rc = handle_username_from_cert_options(context, &username, &password, password_len);
	ᱡᱩᱫᱤ(rc != MOSQ_ERR_SUCCESS){
		goto handle_connect_error;
	}

	/* use_username_as_clientid */
	ᱡᱩᱫᱤ(context->listener->use_username_as_clientid){
		rc = handle_username_as_clientid_option(context);
		ᱡᱩᱫᱤ(rc != MOSQ_ERR_SUCCESS){
			goto handle_connect_error;
		}
	}

	/* Check for an existing delayed auth check, reject if present */
	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), found_context);
	ᱡᱩᱫᱤ(found_context){
		rc = MOSQ_ERR_UNKNOWN;
		goto handle_connect_error;
	}

	context->clean_start = clean_start;
	context->will = will_struct;
	will_struct = NULL;

	ᱡᱩᱫᱤ(context->auth_method){
		rc = mosquitto_security_auth_start(context, false, auth_data, auth_data_len, &auth_data_out, &auth_data_out_len);
		mosquitto_FREE(auth_data);
		ᱡᱩᱫᱤ(rc == MOSQ_ERR_SUCCESS){
			ᱨᱩᱣᱟᱹᱲ connect__on_authorised(context, auth_data_out, auth_data_out_len);
		}ᱵᱟᱝ ᱡᱩᱫᱤ(rc == MOSQ_ERR_AUTH_CONTINUE){
			mosquitto__set_state(context, mosq_cs_authenticating);
			rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_data_out, auth_data_out_len);
			SAFE_FREE(auth_data_out);
			ᱨᱩᱣᱟᱹᱲ rc;
		}ᱵᱟᱝ{
			SAFE_FREE(auth_data_out);
			will__clear(context);
			ᱡᱩᱫᱤ(rc == MOSQ_ERR_AUTH){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, NULL);
				mosquitto_FREE(context->id);
				goto handle_connect_error;
			}ᱵᱟᱝ ᱡᱩᱫᱤ(rc == MOSQ_ERR_NOT_SUPPORTED){
				/* Client has requested extended authentication, but we don't support it. */
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_METHOD, NULL);
				mosquitto_FREE(context->id);
				goto handle_connect_error;
			}ᱵᱟᱝ{
				mosquitto_FREE(context->id);
				goto handle_connect_error;
			}
		}
	}ᱵᱟᱝ{
#ifdef WITH_TLS
		ᱡᱩᱫᱤ(context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
			/* Authentication assumed to be cleared */
		}ᱵᱟᱝ
#endif
		{
			rc = mosquitto_basic_auth(context);
			switch(rc){
				pak_sa MOSQ_ERR_SUCCESS:
					bha_nga;
				pak_sa MOSQ_ERR_AUTH_DELAYED:
					mosquitto__set_state(context, mosq_cs_delayed_auth);
					HASH_ADD_KEYPTR(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context);
					ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
					bha_nga;
				pak_sa MOSQ_ERR_AUTH:
					ᱡᱩᱫᱤ(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, NULL);
					}ᱵᱟᱝ{
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
					ᱡᱩᱫᱤ(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, (uint8_t)rc, NULL);
					}ᱵᱟᱝ{
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
		ᱨᱩᱣᱟᱹᱲ connect__on_authorised(context, NULL, 0);
	}


handle_connect_error:
	mosquitto_property_free_all(&properties);
	mosquitto_FREE(auth_data);
	mosquitto_FREE(clientid);
	mosquitto_FREE(username);
	mosquitto_FREE(password);
	ᱡᱩᱫᱤ(will_struct){
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
	ᱨᱩᱣᱟᱹᱲ rc;
}
