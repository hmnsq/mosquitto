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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <utlist.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "packet_mosq.h"
#ಅನ್ತರ್ಭಾವ "property_mosq.h"
#ಅನ್ತರ್ಭಾವ "send_mosq.h"
#ಅನ್ತರ್ಭಾವ "sys_tree.h"
#ಅನ್ತರ್ಭಾವ "tls_mosq.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"
#ಅನ್ತರ್ಭಾವ "will_mosq.h"

#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif


ಸ್ಥಿರ ವರ್ಣ nibble_to_hex(uint8_t ಮೌಲ್ಯ)
{
	ಯದಿ(ಮೌಲ್ಯ < 0x0A){
		ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ)('0'+ಮೌಲ್ಯ);
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ)(65 /*'A'*/ +ಮೌಲ್ಯ-10);
	}
}


ಸ್ಥಿರ ವರ್ಣ *clientid_gen(uint16_t *idlen, ಧ್ರುವ ವರ್ಣ *auto_id_prefix, uint16_t auto_id_prefix_len)
{
	ವರ್ಣ *clientid;
	uint8_t rnd[16];
	ಪೂರ್ಣಾಙ್ಕ pos;

	ಯದಿ(mosquitto_getrandom(rnd, 16)){
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	*idlen = (uint16_t)(auto_id_prefix_len + 36);

	clientid = (ವರ್ಣ *)mosquitto_calloc((size_t)(*idlen) + 1, ಪರಿಮಾಣ(ವರ್ಣ));
	ಯದಿ(!clientid){
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	ಯದಿ(auto_id_prefix){
		memcpy(clientid, auto_id_prefix, auto_id_prefix_len);
	}

	pos = 0;
	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<16; i++){
		clientid[auto_id_prefix_len + pos + 0] = nibble_to_hex(rnd[i] & 0x0F);
		clientid[auto_id_prefix_len + pos + 1] = nibble_to_hex((rnd[i] >> 4) & 0x0F);
		pos += 2;
		ಯದಿ(pos == 8 || pos == 13 || pos == 18 || pos == 23){
			clientid[auto_id_prefix_len + pos] = '-';
			pos++;
		}
	}

	ಪ್ರತ್ಯಾಗಮ clientid;
}


ಪೂರ್ಣಾಙ್ಕ connect__on_authorised(ಸಂರಚನಾ mosquitto *context, ಶೂನ್ಯ *auth_data_out, uint16_t auth_data_out_len)
{
	ಸಂರಚನಾ mosquitto *found_context;
	ಸಂರಚನಾ mosquitto__subleaf *leaf;
	mosquitto_property *connack_props = ರಿಕ್ತ;
	uint8_t connect_ack = 0;
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಪೂರ್ಣಾಙ್ಕ in_quota, out_quota;
	uint16_t in_maximum, out_maximum;

	/* Find if this client already has an entry. This must be done *after* any security checks. */
	HASH_FIND(hh_id, db.contexts_by_id, context->id, strlen(context->id), found_context);
	ಯದಿ(found_context){
		/* Found a matching client */
		ಯದಿ(!net__is_connected(found_context)){
			/* Client is reconnecting after a disconnect */
			/* FIXME - does anything need to be done here? */
		}ಅನ್ಯಥಾ{
			/* Client is already connected, disconnect old version. This is
			 * done in context__cleanup() below. */
		}

		ಯದಿ(context->clean_start == true){
			sub__clean_session(found_context);
			found_context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			plugin_persist__handle_client_delete(found_context);
		}
		context->is_persisted = found_context->is_persisted;
		found_context->is_persisted = false; /* stops persistence for context being removed */

		ಯದಿ(context->clean_start == false && found_context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
			ಯದಿ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
				connect_ack |= 0x01;
			}

			ಯದಿ(found_context->msgs_in.inflight || found_context->msgs_in.queued
					|| found_context->msgs_out.inflight || found_context->msgs_out.queued){

				in_quota = context->msgs_in.inflight_quota;
				out_quota = context->msgs_out.inflight_quota;
				in_maximum = context->msgs_in.inflight_maximum;
				out_maximum = context->msgs_out.inflight_maximum;

				memcpy(&context->msgs_in, &found_context->msgs_in, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto_ಸಂದೇಶ_data));
				memcpy(&context->msgs_out, &found_context->msgs_out, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto_ಸಂದೇಶ_data));
				context->last_cmsg_id = found_context->last_cmsg_id;

				memset(&found_context->msgs_in, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto_ಸಂದೇಶ_data));
				memset(&found_context->msgs_out, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto_ಸಂದೇಶ_data));

				context->msgs_in.inflight_quota = in_quota;
				context->msgs_out.inflight_quota = out_quota;
				context->msgs_in.inflight_maximum = in_maximum;
				context->msgs_out.inflight_maximum = out_maximum;

				db__message_reconnect_reset(context);
			}
			context->subs = found_context->subs;
			found_context->subs = ರಿಕ್ತ;
			context->subs_capacity = found_context->subs_capacity;
			context->subs_ಎಣಿಕೆ = found_context->subs_ಎಣಿಕೆ;
			found_context->subs_capacity = 0;
			found_context->subs_ಎಣಿಕೆ = 0;
			context->last_mid = found_context->last_mid;

			ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<context->subs_capacity; i++){
				ಯದಿ(context->subs[i]){
					leaf = context->subs[i]->hier->subs;
					ಯಾವತ್(leaf){
						ಯದಿ(leaf->context == found_context){
							leaf->context = context;
						}
						leaf = leaf->next;
					}

					ಯದಿ(context->subs[i]->shared){
						leaf = context->subs[i]->shared->subs;
						ಯಾವತ್(leaf){
							ಯದಿ(leaf->context == found_context){
								leaf->context = context;
							}
							leaf = leaf->next;
						}
					}
				}
			}
		}

		ಯದಿ((found_context->protocol == mosq_p_mqtt5 && found_context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE)
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
		ಯದಿ(found_context->protocol == mosq_p_mqtt5){
			send__disconnect(found_context, MQTT_RC_SESSION_TAKEN_OVER, ರಿಕ್ತ);
		}
		do_disconnect(found_context, MOSQ_ERR_SESSION_TAKEN_OVER);
	}

	ಯದಿ(db.config->global_max_clients > 0 && HASH_CNT(hh_id, db.contexts_by_id) >= (unsigned ಪೂರ್ಣಾಙ್ಕ)db.config->global_max_clients){
		ಯದಿ(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_SERVER_BUSY, ರಿಕ್ತ);
		}
		rc = MOSQ_ERR_INVAL;
		ಗಚ್ಛ error;
	}

	ಯದಿ(db.config->connection_messages == true){
		ಯದಿ(context->is_bridge){
			ಯದಿ(context->username){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive, context->username);
			}ಅನ್ಯಥಾ{
				log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d).",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive);
			}
		}ಅನ್ಯಥಾ{
			ಯದಿ(context->username){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive, context->username);
			}ಅನ್ಯಥಾ{
				log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d).",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive);
			}
		}

		ಯದಿ(context->will){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG, "Will message specified (%ld bytes) (r%d, q%d).",
					(long)context->will->ಸಂದೇಶ.payloadlen,
					context->will->ಸಂದೇಶ.retain,
					context->will->ಸಂದೇಶ.qos);

			log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG, "\t%s", context->will->ಸಂದೇಶ.topic);
		}ಅನ್ಯಥಾ{
			log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG, "No will message specified.");
		}
	}
#ifdef WITH_TLS
	ಯದಿ(context->ssl){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s negotiated %s cipher %s",
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
	ಯದಿ(!context->clean_start){
		db.persistence_changes++;
	}
#endif
	context->max_qos = context->listener->max_qos;

	ಯದಿ(db.config->max_keepalive &&
			(context->keepalive > db.config->max_keepalive || context->keepalive == 0)){

		keepalive__remove(context);
		context->keepalive = db.config->max_keepalive;
		keepalive__add(context);
		ಯದಿ(context->protocol == mosq_p_mqtt5){
			ಯದಿ(mosquitto_property_add_int16(&connack_props, MQTT_PROP_SERVER_KEEP_ALIVE, context->keepalive)){
				rc = MOSQ_ERR_NOMEM;
				ಗಚ್ಛ error;
			}
		}ಅನ್ಯಥಾ{
			send__connack(context, connect_ack, CONNACK_REFUSED_IDENTIFIER_REJECTED, ರಿಕ್ತ);
			rc = MOSQ_ERR_INVAL;
			ಗಚ್ಛ error;
		}
	}


	ಯದಿ(context->protocol == mosq_p_mqtt5){
		ಯದಿ(context->listener->max_topic_alias > 0){
			ಯದಿ(mosquitto_property_add_int16(&connack_props, MQTT_PROP_TOPIC_ALIAS_MAXIMUM, context->listener->max_topic_alias)){
				rc = MOSQ_ERR_NOMEM;
				ಗಚ್ಛ error;
			}
		}
		ಯದಿ(context->assigned_id){
			ಯದಿ(mosquitto_property_add_string(&connack_props, MQTT_PROP_ASSIGNED_CLIENT_IDENTIFIER, context->id)){
				rc = MOSQ_ERR_NOMEM;
				ಗಚ್ಛ error;
			}
		}
		ಯದಿ(context->auth_method){
			ಯದಿ(mosquitto_property_add_string(&connack_props, MQTT_PROP_AUTHENTICATION_METHOD, context->auth_method)){
				rc = MOSQ_ERR_NOMEM;
				ಗಚ್ಛ error;
			}

			ಯದಿ(auth_data_out && auth_data_out_len > 0){
				ಯದಿ(mosquitto_property_add_ದ್ವಿಮಾನ(&connack_props, MQTT_PROP_AUTHENTICATION_DATA, auth_data_out, auth_data_out_len)){
					rc = MOSQ_ERR_NOMEM;
					ಗಚ್ಛ error;
				}
			}
		}
	}
	SAFE_FREE(auth_data_out);

	mosquitto__set_state(context, mosq_cs_active);
	rc = send__connack(context, connect_ack, CONNACK_ACCEPTED, connack_props);
	mosquitto_property_free_all(&connack_props);
	ಯದಿ(rc){
		ಪ್ರತ್ಯಾಗಮ rc;
	}
	db__expire_all_messages(context);
	rc = db__message_write_queued_out(context);
	ಯದಿ(rc){
		ಪ್ರತ್ಯಾಗಮ rc;
	}
	rc = db__message_write_inflight_out_all(context);

	ಯದಿ(rc == MOSQ_ERR_SUCCESS){
		plugin__handle_connect(context);

		ಯದಿ(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
			plugin_persist__handle_client_add(context);
		}ಅನ್ಯಥಾ ಯದಿ(context->will){
			plugin_persist__handle_will_add(context);
		}
	}
	ಪ್ರತ್ಯಾಗಮ rc;
error:
	SAFE_FREE(auth_data_out);
	mosquitto_property_free_all(&connack_props);
	ಪ್ರತ್ಯಾಗಮ rc;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ will__read(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ವರ್ಣ *clientid, ಸಂರಚನಾ mosquitto_message_all **will, uint8_t will_qos, ಪೂರ್ಣಾಙ್ಕ will_retain)
{
	ಪೂರ್ಣಾಙ್ಕ rc = MOSQ_ERR_SUCCESS;
	size_t slen;
	uint16_t tlen;
	ಸಂರಚನಾ mosquitto_message_all *will_struct = ರಿಕ್ತ;
	ವರ್ಣ *will_topic_mount = ರಿಕ್ತ;
	uint16_t payloadlen;
	mosquitto_property *properties = ರಿಕ್ತ;

	will_struct = mosquitto_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto_message_all));
	ಯದಿ(!will_struct){
		rc = MOSQ_ERR_NOMEM;
		ಗಚ್ಛ error_cleanup;
	}
	ಯದಿ(context->protocol == PROTOCOL_VERSION_v5){
		rc = property__read_all(CMD_WILL, &context->in_packet, &properties);
		ಯದಿ(rc){
			ಗಚ್ಛ error_cleanup;
		}

		rc = property__process_will(context, will_struct, &properties);
		mosquitto_property_free_all(&properties);
		ಯದಿ(rc){
			ಗಚ್ಛ error_cleanup;
		}
	}
	rc = packet__read_string(&context->in_packet, &will_struct->ಸಂದೇಶ.topic, &tlen);
	ಯದಿ(rc){
		ಗಚ್ಛ error_cleanup;
	}
	ಯದಿ(!tlen){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: Will with empty topic.",
				context->id);
		rc = MOSQ_ERR_PROTOCOL;
		ಗಚ್ಛ error_cleanup;
	}

	ಯದಿ(context->listener->mount_point){
		slen = strlen(context->listener->mount_point) + strlen(will_struct->ಸಂದೇಶ.topic) + 1;
		will_topic_mount = mosquitto_malloc(slen+1);
		ಯದಿ(!will_topic_mount){
			rc = MOSQ_ERR_NOMEM;
			ಗಚ್ಛ error_cleanup;
		}

		snprintf(will_topic_mount, slen, "%s%s", context->listener->mount_point, will_struct->ಸಂದೇಶ.topic);
		will_topic_mount[slen] = '\0';

		mosquitto_FREE(will_struct->ಸಂದೇಶ.topic);
		will_struct->ಸಂದೇಶ.topic = will_topic_mount;
	}

	ಯದಿ(!strncmp(will_struct->ಸಂದೇಶ.topic, "$CONTROL/", strlen("$CONTROL/"))){
		rc = MOSQ_ERR_ACL_DENIED;
		ಗಚ್ಛ error_cleanup;
	}
	rc = mosquitto_pub_topic_check(will_struct->ಸಂದೇಶ.topic);
	ಯದಿ(rc){
		ಗಚ್ಛ error_cleanup;
	}

	rc = packet__read_uint16(&context->in_packet, &payloadlen);
	ಯದಿ(rc){
		ಗಚ್ಛ error_cleanup;
	}

	will_struct->ಸಂದೇಶ.payloadlen = payloadlen;
	ಯದಿ(will_struct->ಸಂದೇಶ.payloadlen > 0){
		ಯದಿ(db.config->message_size_limit && will_struct->ಸಂದೇಶ.payloadlen > (ಪೂರ್ಣಾಙ್ಕ)db.config->message_size_limit){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG, "Client %s connected with too large Will payload", clientid);
			ಯದಿ(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_PACKET_TOO_LARGE, ರಿಕ್ತ);
			}ಅನ್ಯಥಾ{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ರಿಕ್ತ);
			}
			rc = MOSQ_ERR_PAYLOAD_SIZE;
			ಗಚ್ಛ error_cleanup;
		}
		will_struct->ಸಂದೇಶ.payload = mosquitto_malloc((size_t)will_struct->ಸಂದೇಶ.payloadlen);
		ಯದಿ(!will_struct->ಸಂದೇಶ.payload){
			rc = MOSQ_ERR_NOMEM;
			ಗಚ್ಛ error_cleanup;
		}

		rc = packet__read_bytes(&context->in_packet, will_struct->ಸಂದೇಶ.payload, (uint32_t)will_struct->ಸಂದೇಶ.payloadlen);
		ಯದಿ(rc){
			ಗಚ್ಛ error_cleanup;
		}
	}

	will_struct->ಸಂದೇಶ.qos = will_qos;
	will_struct->ಸಂದೇಶ.retain = will_retain;

	*will = will_struct;
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;

error_cleanup:
	ಯದಿ(will_struct){
		mosquitto_FREE(will_struct->ಸಂದೇಶ.topic);
		mosquitto_FREE(will_struct->ಸಂದೇಶ.payload);
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct);
	}
	ಪ್ರತ್ಯಾಗಮ rc;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ check_protocol_version(ಸಂರಚನಾ mosquitto__listener *listener, ಪೂರ್ಣಾಙ್ಕ protocol_version)
{
	/* Allow bridge protocol as well. */
	protocol_version &= 0x7F;

	ಯದಿ((protocol_version == 3 && listener->disable_protocol_v3 == false)
			|| (protocol_version == 4 && listener->disable_protocol_v4 == false)
			|| (protocol_version == 5 && listener->disable_protocol_v5 == false)
			){

		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_SUPPORTED;
	}
}


ಅನ್ತಃಸ್ಥ ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ send__connack_error_and_return(ಸಂರಚನಾ mosquitto *context, uint8_t err_code, ಪೂರ್ಣಾಙ್ಕ rc)
{
	send__connack(context, 0, err_code, ರಿಕ್ತ);
	ಪ್ರತ್ಯಾಗಮ rc;
}


ಅನ್ತಃಸ್ಥ ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ send__connack_bad_username_or_password_error(ಸಂರಚನಾ mosquitto *context, ಪೂರ್ಣಾಙ್ಕ rc)
{
	uint8_t err_code  = context->protocol == mosq_p_mqtt5
									? (uint8_t)MQTT_RC_BAD_USERNAME_OR_PASSWORD
									: (uint8_t)CONNACK_REFUSED_BAD_USERNAME_PASSWORD;
	ಪ್ರತ್ಯಾಗಮ send__connack_error_and_return(context, err_code, rc);
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ read_protocol_name(ಸಂರಚನಾ mosquitto *context, ವರ್ಣ protocol_name[7])
{
	/* Read protocol name as length then bytes rather than with read_string
	 * because the length is fixed and we can check that. Removes the need
	 * for another malloc as well. */

	uint16_t slen = 0;

	ಯದಿ(packet__read_uint16(&context->in_packet, &slen)){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string length.",
				context->address, context->remote_port);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
	}
	ಯದಿ(slen != 4 /* MQTT */ && slen != 6 /* MQIsdp */){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with incorrect protocol string length (%d).",
				context->address, context->remote_port, slen);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
	}
	ಯದಿ(packet__read_bytes(&context->in_packet, protocol_name, slen)){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string.",
				context->address, context->remote_port);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
	}
	protocol_name[slen] = '\0';

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ read_and_verify_protocol_version(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ವರ್ಣ *protocol_name,
		uint8_t *protocol_version)
{
	uint8_t tmp_protocol_version = 0;
	ಯದಿ(packet__read_byte(&context->in_packet, &tmp_protocol_version)){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol version.",
				context->address, context->remote_port);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
	}

	ಯದಿ(check_protocol_version(context->listener, tmp_protocol_version)){
		ಯದಿ(tmp_protocol_version == 3 || tmp_protocol_version == 4){
			context->protocol = mosq_p_mqtt311;
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, ರಿಕ್ತ);
		}ಅನ್ಯಥಾ{
			context->protocol = mosq_p_mqtt5;
			send__connack(context, 0, MQTT_RC_UNSUPPORTED_PROTOCOL_VERSION, ರಿಕ್ತ);
		}
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_SUPPORTED;
	}

	ಯದಿ(!strcmp(protocol_name, PROTOCOL_NAME_v31)){
		ಯದಿ((tmp_protocol_version&0x7F) != PROTOCOL_VERSION_v31){
			ಯದಿ(db.config->connection_messages == true){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->address, context->remote_port, tmp_protocol_version);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, ರಿಕ್ತ);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
		}
		context->protocol = mosq_p_mqtt31;
		ಯದಿ((tmp_protocol_version&0x80) == 0x80){
			context->is_bridge = true;
		}
	}ಅನ್ಯಥಾ ಯದಿ(!strcmp(protocol_name, PROTOCOL_NAME)){
		ಯದಿ((tmp_protocol_version&0x7F) == PROTOCOL_VERSION_v311){
			context->protocol = mosq_p_mqtt311;

			ಯದಿ((tmp_protocol_version&0x80) == 0x80){
				context->is_bridge = true;
			}
		}ಅನ್ಯಥಾ ಯದಿ((tmp_protocol_version&0x7F) == PROTOCOL_VERSION_v5){
			context->protocol = mosq_p_mqtt5;
		}ಅನ್ಯಥಾ{
			ಯದಿ(db.config->connection_messages == true){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->address, context->remote_port, tmp_protocol_version);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, ರಿಕ್ತ);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
		}
		ಯದಿ((context->in_packet.command&0x0F) != 0x00){
			/* Reserved flags not set to 0, must disconnect. */
			log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero reserved flags (%02X).",
					context->address, context->remote_port, context->in_packet.command);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
		}
	}ಅನ್ಯಥಾ{
		ಯದಿ(db.config->connection_messages == true){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol \"%s\".",
					context->address, context->remote_port, protocol_name);
		}
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
	}
	ಯದಿ((tmp_protocol_version&0x7F) != PROTOCOL_VERSION_v31 && context->in_packet.command != CMD_CONNECT){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
	}

	*protocol_version = tmp_protocol_version;
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ read_and_verify_connect_flags(ಸಂರಚನಾ mosquitto *context, uint8_t *connect_flags)
{
	ಯದಿ(packet__read_byte(&context->in_packet, connect_flags)){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing connect flags.",
				context->address, context->remote_port);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
	}
	ಯದಿ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		ಯದಿ((*connect_flags & 0x01) != 0x00){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero connect reserved flag (%02X).",
					context->address, context->remote_port, *connect_flags);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಶೂನ್ಯ set_session_expiry_interval(ಸಂರಚನಾ mosquitto *context, uint8_t clean_start, uint8_t protocol_version)
{
	/* session_expiry_interval will be overridden if the properties are read later */
	ಯದಿ(clean_start == false && protocol_version != PROTOCOL_VERSION_v5){
		/* v3* has clean_start == false mean the session never expires */
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
	}ಅನ್ಯಥಾ{
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	}
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ read_and_reset_keepalive(ಸಂರಚನಾ mosquitto *context)
{
	/* _remove here because net__socket_accept() uses _add and we must have the
	 * correct keepalive value */
	keepalive__remove(context);

	ಯದಿ(packet__read_uint16(&context->in_packet, &(context->keepalive))){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "%s sent CONNECT with missing keepalive.",
				context->id);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
	}
	keepalive__add(context);

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ read_and_verify_v5_connect_properties(ಸಂರಚನಾ mosquitto *context, mosquitto_property **properties, uint8_t protocol_version)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ(protocol_version == PROTOCOL_VERSION_v5){
		rc = property__read_all(CMD_CONNECT, &context->in_packet, properties);
		ಯದಿ(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
			send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, ರಿಕ್ತ);
		}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_MALFORMED_PACKET){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, ರಿಕ್ತ);
		}
		ಯದಿ(rc){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}
	rc = property__process_connect(context, properties);
	ಯದಿ(rc != MOSQ_ERR_SUCCESS){
		ಪ್ರತ್ಯಾಗಮ send__connack_error_and_return(context, MQTT_RC_PROTOCOL_ERROR, rc);
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ verify_will_options(ಸಂರಚನಾ mosquitto *context, uint8_t will, uint8_t will_qos, uint8_t will_retain, uint8_t protocol_version)
{
	ಯದಿ(will_qos == 3){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid Will QoS (%d).",
				context->address, context->remote_port, will_qos);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
	}

	ಯದಿ(will && will_retain && db.config->retain_available == false){
		ಯದಿ(protocol_version == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_RETAIN_NOT_SUPPORTED, ರಿಕ್ತ);
		}
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_SUPPORTED;
	}

	ಯದಿ(will && will_qos > context->listener->max_qos){
		ಯದಿ(protocol_version == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_QOS_NOT_SUPPORTED, ರಿಕ್ತ);
		}
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_SUPPORTED;
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ handle_zero_length_clientid(ಸಂರಚನಾ mosquitto *context, ವರ್ಣ **clientid, bool *allow_zero_length_clientid,
		uint8_t clean_start)
{
	ಯದಿ(context->protocol == mosq_p_mqtt31){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s:%d: v3.1 CONNECT with zero length clientid.",
				context->address, context->remote_port);
		send__connack(context, 0, CONNACK_REFUSED_IDENTIFIER_REJECTED, ರಿಕ್ತ);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
	}

	/* mqtt311/mqtt5 */
	mosquitto_FREE(*clientid);

	ಯದಿ(db.config->per_listener_settings){
		*allow_zero_length_clientid = context->listener->security_options->allow_zero_length_clientid;
	}ಅನ್ಯಥಾ{
		*allow_zero_length_clientid = db.config->security_options.allow_zero_length_clientid;
	}

	ಯದಿ((context->protocol == mosq_p_mqtt311 && clean_start == 0) || *allow_zero_length_clientid == false){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with zero length clientid when forbidden.",
				context->address, context->remote_port);
		uint8_t err_code = context->protocol == mosq_p_mqtt311 ? (uint8_t)CONNACK_REFUSED_IDENTIFIER_REJECTED : (uint8_t)MQTT_RC_UNSPECIFIED;
		ಪ್ರತ್ಯಾಗಮ send__connack_error_and_return(context, err_code, MOSQ_ERR_PROTOCOL);
	}

	*clientid = clientid_gen(&(uint16_t){0}, context->listener->security_options->auto_id_prefix,
			context->listener->security_options->auto_id_prefix_len);
	ಯದಿ(*clientid == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}
	context->assigned_id = true;

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ check_clientid_prefixes(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ವರ್ಣ *clientid)
{
	ಯದಿ(db.config->clientid_prefixes){
		ಯದಿ(strncmp(db.config->clientid_prefixes, clientid, strlen(db.config->clientid_prefixes))){
			uint8_t err_code = context->protocol == mosq_p_mqtt5 ? (uint8_t)MQTT_RC_NOT_AUTHORIZED : (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
			ಪ್ರತ್ಯಾಗಮ send__connack_error_and_return(context, err_code, MOSQ_ERR_AUTH);
		}
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ read_and_verify_clientid_from_packet(ಸಂರಚನಾ mosquitto *context, ವರ್ಣ **clientid,
		bool *allow_zero_length_clientid, uint8_t clean_start)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	uint16_t slen;

	rc = packet__read_string(&context->in_packet, clientid, &slen);
	ಯದಿ(rc == MOSQ_ERR_MALFORMED_UTF8){
		ಯದಿ(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_CLIENTID_NOT_VALID, ರಿಕ್ತ);
		}
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID;
	}ಅನ್ಯಥಾ ಯದಿ(rc > 0){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
	}

	ಯದಿ(slen == 0){
		rc = handle_zero_length_clientid(context, clientid, allow_zero_length_clientid, clean_start);
		ಯದಿ(rc != MOSQ_ERR_SUCCESS){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}

	rc = check_clientid_prefixes(context, *clientid);
	ಯದಿ(rc != MOSQ_ERR_SUCCESS){
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ set_username_from_packet(ಸಂರಚನಾ mosquitto *context, ವರ್ಣ **username, ಧ್ರುವ ವರ್ಣ *clientid)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

	rc = packet__read_string(&context->in_packet, username, &(uint16_t){0});
	ಯದಿ(rc == MOSQ_ERR_NOMEM){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}
	ಯದಿ(rc != MOSQ_ERR_SUCCESS){
		ಯದಿ(context->protocol == mosq_p_mqtt31){
			/* Username flag given, but no username. Ignore. */
			/* NOTE: Removed setting of username_flag to zero as it is unused afterwards */
		}ಅನ್ಯಥಾ{
			log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with username flag but no username.",
					clientid);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ set_password_from_packet(ಸಂರಚನಾ mosquitto *context, ವರ್ಣ **password, uint16_t *password_len, ಧ್ರುವ ವರ್ಣ *clientid)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

	rc = packet_read_ದ್ವಿಮಾನ(&context->in_packet, (uint8_t **)password, password_len);
	ಯದಿ(rc == MOSQ_ERR_NOMEM){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}

	ಯದಿ(rc == MOSQ_ERR_MALFORMED_PACKET){
		ಯದಿ(context->protocol == mosq_p_mqtt31){
			/* Password flag given, but no password. Ignore. */
		}ಅನ್ಯಥಾ{
			log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with password flag but no password.",
					clientid);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ read_and_verify_client_credentials_from_packet(ಸಂರಚನಾ mosquitto *context,
		ವರ್ಣ **username, uint8_t username_flag,
		ವರ್ಣ **password, uint16_t *password_len, uint8_t password_flag,
		ಧ್ರುವ ವರ್ಣ *clientid)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ(username_flag){
		rc = set_username_from_packet(context, username, clientid);
		ಯದಿ(rc != MOSQ_ERR_SUCCESS){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}ಅನ್ಯಥಾ{
		ಯದಿ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
			ಯದಿ(password_flag){
				/* username_flag == 0 && password_flag == 1 is forbidden */
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Protocol error from %s: password without username, closing connection.", clientid);
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
			}
		}
	}
	ಯದಿ(password_flag){
		rc = set_password_from_packet(context, password, password_len, clientid);
		ಯದಿ(rc != MOSQ_ERR_SUCCESS){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ check_additional_trailing_data(ಸಂರಚನಾ mosquitto *context, uint8_t protocol_version)
{
	ಯದಿ(context->in_packet.pos != context->in_packet.remaining_length){
		/* Surplus data at end of packet, this must be an error. */
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->in_packet.pos, context->in_packet.remaining_length);
		ಯದಿ(protocol_version == PROTOCOL_VERSION_v5){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, ರಿಕ್ತ);
		}
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}

#ifdef WITH_TLS


ಅನ್ತಃಸ್ಥ ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ get_client_cert_and_subject_name(ಸಂರಚನಾ mosquitto *context, X509 **client_cert, X509_NAME **name)
{
	*client_cert = SSL_get_peer_certificate(context->ssl);
	ಯದಿ(*client_cert == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	*name = X509_get_subject_name(*client_cert);
	ಯದಿ(*name == ರಿಕ್ತ){
		X509_free(*client_cert);
		ಪ್ರತ್ಯಾಗಮ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಅನ್ತಃಸ್ಥ ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ free_x509_and_send_connack_error(ಸಂರಚನಾ mosquitto *context, X509 *client_cert, ಪೂರ್ಣಾಙ್ಕ rc)
{
	X509_free(client_cert);
	ಪ್ರತ್ಯಾಗಮ send__connack_bad_username_or_password_error(context, rc);
}


ಅನ್ತಃಸ್ಥ ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ free_x509_and_BIO_and_send_connack_error(ಸಂರಚನಾ mosquitto *context, X509 *client_cert,
		BIO *subject_name, ಪೂರ್ಣಾಙ್ಕ rc)
{
	BIO_free(subject_name);
	ಪ್ರತ್ಯಾಗಮ free_x509_and_send_connack_error(context, client_cert, rc);
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ set_username_from_cert_identity(ಸಂರಚನಾ mosquitto *context)
{
	X509 *client_cert = ರಿಕ್ತ;
	X509_NAME *name = ರಿಕ್ತ;

	ಯದಿ(get_client_cert_and_subject_name(context, &client_cert, &name)){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_AUTH;
	}

	ಪೂರ್ಣಾಙ್ಕ i = -1;
	X509_NAME_ENTRY *name_entry = ರಿಕ್ತ;

	i = X509_NAME_get_index_by_NID(name, NID_commonName, -1);
	ಯದಿ(i == -1){
		ಪ್ರತ್ಯಾಗಮ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	name_entry = X509_NAME_get_entry(name, i);
	ಯದಿ(!name_entry){
		ಗಚ್ಛ success;
	}

	ASN1_STRING *name_asn1 = ರಿಕ್ತ;
	name_asn1 = X509_NAME_ENTRY_get_data(name_entry);
	ಯದಿ(name_asn1 == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	ಧ್ರುವ ವರ್ಣ *cert_identity = (ಧ್ರುವ ವರ್ಣ *)ASN1_STRING_get0_data(name_asn1);
	ಯದಿ(!cert_identity || mosquitto_validate_utf8(cert_identity, (ಪೂರ್ಣಾಙ್ಕ)strlen(cert_identity))){
		ಪ್ರತ್ಯಾಗಮ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	mosquitto_free(context->username);
	context->username = mosquitto_strdup(cert_identity);
	ಯದಿ(context->username == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_NOMEM);
	}
	/* Make sure there isn't an embedded NUL character in the CN */
	ಯದಿ((size_t)ASN1_STRING_length(name_asn1) != strlen(context->username)){
		ಪ್ರತ್ಯಾಗಮ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}

success:
	X509_free(client_cert);
	client_cert = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ set_username_from_cert_subject_name(ಸಂರಚನಾ mosquitto *context)
{
	X509 *client_cert = ರಿಕ್ತ;
	X509_NAME *name = ರಿಕ್ತ;

	ಯದಿ(get_client_cert_and_subject_name(context, &client_cert, &name)){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_AUTH;
	}

	ವರ್ಣ *subject = ರಿಕ್ತ;
	ವರ್ಣ *data_start = ರಿಕ್ತ;
	BIO *subject_bio = ರಿಕ್ತ;
	long name_length = 0;

	subject_bio = BIO_new(BIO_s_mem());
	X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
	data_start = ರಿಕ್ತ;
	name_length = BIO_get_mem_data(subject_bio, &data_start);
	subject = mosquitto_malloc(ಪರಿಮಾಣ(ವರ್ಣ)*(size_t)(name_length+1));
	ಯದಿ(!subject){
		ಪ್ರತ್ಯಾಗಮ free_x509_and_BIO_and_send_connack_error(context, client_cert, subject_bio, MOSQ_ERR_NOMEM);
	}
	memcpy(subject, data_start, (size_t)name_length);
	subject[name_length] = '\0';
	BIO_free(subject_bio);

	ಯದಿ(mosquitto_validate_utf8(subject, (ಪೂರ್ಣಾಙ್ಕ)strlen(subject))){
		mosquitto_free(subject);
		ಪ್ರತ್ಯಾಗಮ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	context->username = subject;
	ಯದಿ(!context->username){
		X509_free(client_cert);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_AUTH;
	}

	X509_free(client_cert);
	client_cert = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}
#endif


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ handle_username_from_cert_options(ಸಂರಚನಾ mosquitto *context, ವರ್ಣ **username, ವರ್ಣ **password, uint16_t password_len)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

#ifdef WITH_TLS
	ಯದಿ(context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
		/* Don't need the username or password if provided */
		mosquitto_FREE(*username);
		mosquitto_FREE(*password);

		ಯದಿ(!context->ssl){
			ಪ್ರತ್ಯಾಗಮ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
		}
#ifdef FINAL_WITH_TLS_PSK
		ಯದಿ(context->listener->psk_hint){
			/* Client should have provided an identity to get this far. */
			ಯದಿ(!context->username){
				ಪ್ರತ್ಯಾಗಮ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}ಅನ್ಯಥಾ
#endif /* FINAL_WITH_TLS_PSK */
		{
			ಯದಿ(context->listener->use_identity_as_username){
				rc = set_username_from_cert_identity(context);
			}ಅನ್ಯಥಾ{   /* use_subject_as_username */
				rc = set_username_from_cert_subject_name(context);
			}
			ಯದಿ(rc){
				ಪ್ರತ್ಯಾಗಮ rc;
			}
		}
	}ಅನ್ಯಥಾ
#endif /* WITH_TLS */
	{
#ifdef WITH_TLS
		ಯದಿ(context->listener->use_identity_as_username && context->listener->require_certificate){
			mosquitto_FREE(*username);
			mosquitto_FREE(*password);

			ಯದಿ(!context->username){
				ಪ್ರತ್ಯಾಗಮ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}ಅನ್ಯಥಾ
#endif
		{
			/* FIXME - these ensure the mosquitto_clientid() and
			* mosquitto_client_username() functions work, but is hacky */
			context->username = *username;
			context->password = *password;
			context->password_len = password_len;
			*username = ರಿಕ್ತ; /* Avoid free() in error: below. */
			*password = ರಿಕ್ತ;
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ handle_username_as_clientid_option(ಸಂರಚನಾ mosquitto *context)
{
	ಯದಿ(context->username){
		mosquitto_FREE(context->id);
		context->id = mosquitto_strdup(context->username);
		ಯದಿ(!context->id){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}
	}ಅನ್ಯಥಾ{
		uint8_t err_code = context->protocol == mosq_p_mqtt5
												? (uint8_t)MQTT_RC_NOT_AUTHORIZED
												: (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
		ಪ್ರತ್ಯಾಗಮ send__connack_error_and_return(context, err_code, MOSQ_ERR_AUTH);
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ handle__connect(ಸಂರಚನಾ mosquitto *context)
{
	ವರ್ಣ protocol_name[7];
	uint8_t protocol_version;
	uint8_t connect_flags;
	ವರ್ಣ *clientid = ರಿಕ್ತ;
	ಸಂರಚನಾ mosquitto *found_context;
	ಸಂರಚನಾ mosquitto_message_all *will_struct = ರಿಕ್ತ;
	uint8_t will, will_retain, will_qos, clean_start;
	uint8_t username_flag, password_flag;
	ವರ್ಣ *username = ರಿಕ್ತ, *password = ರಿಕ್ತ;
	uint16_t password_len = 0;
	ಪೂರ್ಣಾಙ್ಕ rc;
	mosquitto_property *properties = ರಿಕ್ತ;
	ಶೂನ್ಯ *auth_data = ರಿಕ್ತ;
	uint16_t auth_data_len = 0;
	ಶೂನ್ಯ *auth_data_out = ರಿಕ್ತ;
	uint16_t auth_data_out_len = 0;
	bool allow_zero_length_clientid;

	ಯದಿ(!context->listener){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	/* Don't accept multiple CONNECT commands. */
	ಯದಿ(context->state != mosq_cs_new){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Bad client %s:%d sending multiple CONNECT messages.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		ಗಚ್ಛ handle_connect_error;
	}

#ifdef WITH_TLS
	ಯದಿ(context->in_packet.command == 0x16 && context->listener->ssl_ctx == ರಿಕ್ತ){ /* 0x16 is TLS handshake client hello */
		log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client from %s:%d appears to be using TLS to connect to a non-TLS listener.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		ಗಚ್ಛ handle_connect_error;
	}
#endif

	rc = read_protocol_name(context, protocol_name);
	ಯದಿ(rc != MOSQ_ERR_SUCCESS){
		ಗಚ್ಛ handle_connect_error;
	}

	rc = read_and_verify_protocol_version(context, protocol_name, &protocol_version);
	ಯದಿ(rc != MOSQ_ERR_SUCCESS){
		ಯದಿ(rc == MOSQ_ERR_MALFORMED_PACKET){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
		ಗಚ್ಛ handle_connect_error;
	}

	rc = read_and_verify_connect_flags(context, &connect_flags);
	ಯದಿ(rc != MOSQ_ERR_SUCCESS){
		ಗಚ್ಛ handle_connect_error;
	}

	clean_start = (connect_flags & 0x02) >> 1;
	set_session_expiry_interval(context, clean_start, protocol_version);

	rc = read_and_reset_keepalive(context);
	ಯದಿ(rc != MOSQ_ERR_SUCCESS){
		ಗಚ್ಛ handle_connect_error;
	}

	rc = read_and_verify_v5_connect_properties(context, &properties, protocol_version);
	ಯದಿ(rc != MOSQ_ERR_SUCCESS){
		ಗಚ್ಛ handle_connect_error;
	}

	will = connect_flags & 0x04;
	will_qos = (connect_flags & 0x18) >> 3;
	will_retain = ((connect_flags & 0x20) == 0x20);
	rc = verify_will_options(context, will, will_qos, will_retain, protocol_version);
	ಯದಿ(rc != MOSQ_ERR_SUCCESS){
		ಗಚ್ಛ handle_connect_error;
	}

	mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_METHOD, &context->auth_method, false);
	mosquitto_property_read_ದ್ವಿಮಾನ(properties, MQTT_PROP_AUTHENTICATION_DATA, &auth_data, &auth_data_len, false);
	mosquitto_property_free_all(&properties);

	ಯದಿ(auth_data && !context->auth_method){
		send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, ರಿಕ್ತ);
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s:%d CONNECT with missing clientid string.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		ಗಚ್ಛ handle_connect_error;
	}

	rc = read_and_verify_clientid_from_packet(context, &clientid, &allow_zero_length_clientid, clean_start);
	ಯದಿ(rc != MOSQ_ERR_SUCCESS){
		ಗಚ್ಛ handle_connect_error;
	}

	ಯದಿ(will){
		rc = will__read(context, clientid, &will_struct, will_qos, will_retain);
		ಯದಿ(rc){
			ಯದಿ(context->protocol == mosq_p_mqtt5){
				ಯದಿ(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
					send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, ರಿಕ್ತ);
				}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_MALFORMED_PACKET){
					send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, ರಿಕ್ತ);
				}
			}
			ಗಚ್ಛ handle_connect_error;
		}
	}ಅನ್ಯಥಾ{
		ಯದಿ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
			ಯದಿ(will_qos != 0 || will_retain != 0){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT without Will with non-zero QoS (%d) or retain (%d).",
						clientid, will_qos, will_retain);
				rc = MOSQ_ERR_PROTOCOL;
				ಗಚ್ಛ handle_connect_error;
			}
		}
	}

	// Client credentials
	password_flag = connect_flags & 0x40;
	username_flag = connect_flags & 0x80;
	rc = read_and_verify_client_credentials_from_packet(context, &username, username_flag, &password, &password_len, password_flag, clientid);
	ಯದಿ(rc != MOSQ_ERR_SUCCESS){
		ಗಚ್ಛ handle_connect_error;
	}

	rc = check_additional_trailing_data(context, protocol_version);
	ಯದಿ(rc != MOSQ_ERR_SUCCESS){
		ಗಚ್ಛ handle_connect_error;
	}

	/* Once context->id is set, if we return from this function with an error
	 * we must make sure that context->id is freed and set to NULL, so that the
	 * client isn't erroneously removed from the by_id hash table. */
	context->id = clientid;
	clientid = ರಿಕ್ತ;

	/* use_identity_as_username or use_subject_as_username */
	rc = handle_username_from_cert_options(context, &username, &password, password_len);
	ಯದಿ(rc != MOSQ_ERR_SUCCESS){
		ಗಚ್ಛ handle_connect_error;
	}

	/* use_username_as_clientid */
	ಯದಿ(context->listener->use_username_as_clientid){
		rc = handle_username_as_clientid_option(context);
		ಯದಿ(rc != MOSQ_ERR_SUCCESS){
			ಗಚ್ಛ handle_connect_error;
		}
	}

	/* Check for an existing delayed auth check, reject if present */
	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), found_context);
	ಯದಿ(found_context){
		rc = MOSQ_ERR_UNKNOWN;
		ಗಚ್ಛ handle_connect_error;
	}

	context->clean_start = clean_start;
	context->will = will_struct;
	will_struct = ರಿಕ್ತ;

	ಯದಿ(context->auth_method){
		rc = mosquitto_security_auth_start(context, false, auth_data, auth_data_len, &auth_data_out, &auth_data_out_len);
		mosquitto_FREE(auth_data);
		ಯದಿ(rc == MOSQ_ERR_SUCCESS){
			ಪ್ರತ್ಯಾಗಮ connect__on_authorised(context, auth_data_out, auth_data_out_len);
		}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_AUTH_CONTINUE){
			mosquitto__set_state(context, mosq_cs_authenticating);
			rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_data_out, auth_data_out_len);
			SAFE_FREE(auth_data_out);
			ಪ್ರತ್ಯಾಗಮ rc;
		}ಅನ್ಯಥಾ{
			SAFE_FREE(auth_data_out);
			will__clear(context);
			ಯದಿ(rc == MOSQ_ERR_AUTH){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ರಿಕ್ತ);
				mosquitto_FREE(context->id);
				ಗಚ್ಛ handle_connect_error;
			}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_NOT_SUPPORTED){
				/* Client has requested extended authentication, but we don't support it. */
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_METHOD, ರಿಕ್ತ);
				mosquitto_FREE(context->id);
				ಗಚ್ಛ handle_connect_error;
			}ಅನ್ಯಥಾ{
				mosquitto_FREE(context->id);
				ಗಚ್ಛ handle_connect_error;
			}
		}
	}ಅನ್ಯಥಾ{
#ifdef WITH_TLS
		ಯದಿ(context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
			/* Authentication assumed to be cleared */
		}ಅನ್ಯಥಾ
#endif
		{
			rc = mosquitto_basic_auth(context);
			ವಿಕಲ್ಪ(rc){
				ಪಕ್ಷ MOSQ_ERR_SUCCESS:
					ಭಙ್ಗ;
				ಪಕ್ಷ MOSQ_ERR_AUTH_DELAYED:
					mosquitto__set_state(context, mosq_cs_delayed_auth);
					HASH_ADD_KEYPTR(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context);
					ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
					ಭಙ್ಗ;
				ಪಕ್ಷ MOSQ_ERR_AUTH:
					ಯದಿ(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ರಿಕ್ತ);
					}ಅನ್ಯಥಾ{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ರಿಕ್ತ);
					}
					ಗಚ್ಛ handle_connect_error;
					ಭಙ್ಗ;
				ಪಕ್ಷ MOSQ_ERR_UNSPECIFIED:
				ಪಕ್ಷ MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
				ಪಕ್ಷ MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
				ಪಕ್ಷ MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
				ಪಕ್ಷ MOSQ_ERR_SERVER_UNAVAILABLE:
				ಪಕ್ಷ MOSQ_ERR_SERVER_BUSY:
				ಪಕ್ಷ MOSQ_ERR_BANNED:
				ಪಕ್ಷ MOSQ_ERR_BAD_AUTHENTICATION_METHOD:
				ಪಕ್ಷ MOSQ_ERR_CONNECTION_RATE_EXCEEDED:
					ಯದಿ(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, (uint8_t)rc, ರಿಕ್ತ);
					}ಅನ್ಯಥಾ{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ರಿಕ್ತ);
					}
					ಗಚ್ಛ handle_connect_error;
					ಭಙ್ಗ;
				ಸ್ವಭಾವ:
					rc = MOSQ_ERR_UNKNOWN;
					ಗಚ್ಛ handle_connect_error;
					ಭಙ್ಗ;
			}
		}
		ಪ್ರತ್ಯಾಗಮ connect__on_authorised(context, ರಿಕ್ತ, 0);
	}


handle_connect_error:
	mosquitto_property_free_all(&properties);
	mosquitto_FREE(auth_data);
	mosquitto_FREE(clientid);
	mosquitto_FREE(username);
	mosquitto_FREE(password);
	ಯದಿ(will_struct){
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct->ಸಂದೇಶ.payload);
		mosquitto_FREE(will_struct->ಸಂದೇಶ.topic);
		mosquitto_FREE(will_struct);
	}
	will__clear(context);
	/* We return an error here which means the client is freed later on. */
	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->will_delay_interval = 0;
	ಪ್ರತ್ಯಾಗಮ rc;
}
