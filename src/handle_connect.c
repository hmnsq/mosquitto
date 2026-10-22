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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <utlist.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"
#ଅନ୍ତର୍ଭାଵ "packet_mosq.h"
#ଅନ୍ତର୍ଭାଵ "property_mosq.h"
#ଅନ୍ତର୍ଭାଵ "send_mosq.h"
#ଅନ୍ତର୍ଭାଵ "sys_tree.h"
#ଅନ୍ତର୍ଭାଵ "tls_mosq.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"
#ଅନ୍ତର୍ଭାଵ "will_mosq.h"

#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif


ସ୍ଥିର ଅକ୍ଷର nibble_to_hex(uint8_t ମୂଲ୍ୟ)
{
	ଯଦି(ମୂଲ୍ୟ < 0x0A){
		ଫେରନ୍ତୁ (ଅକ୍ଷର)('0'+ମୂଲ୍ୟ);
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ (ଅକ୍ଷର)(65 /*'A'*/ +ମୂଲ୍ୟ-10);
	}
}


ସ୍ଥିର ଅକ୍ଷର *clientid_gen(uint16_t *idlen, ଅଚଳ ଅକ୍ଷର *auto_id_prefix, uint16_t auto_id_prefix_len)
{
	ଅକ୍ଷର *clientid;
	uint8_t rnd[16];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pos;

	ଯଦି(mosquitto_getrandom(rnd, 16)){
		ଫେରନ୍ତୁ ଖାଲି;
	}

	*idlen = (uint16_t)(auto_id_prefix_len + 36);

	clientid = (ଅକ୍ଷର *)mosquitto_calloc((size_t)(*idlen) + 1, ଆକାର(ଅକ୍ଷର));
	ଯଦି(!clientid){
		ଫେରନ୍ତୁ ଖାଲି;
	}
	ଯଦି(auto_id_prefix){
		memcpy(clientid, auto_id_prefix, auto_id_prefix_len);
	}

	pos = 0;
	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<16; i++){
		clientid[auto_id_prefix_len + pos + 0] = nibble_to_hex(rnd[i] & 0x0F);
		clientid[auto_id_prefix_len + pos + 1] = nibble_to_hex((rnd[i] >> 4) & 0x0F);
		pos += 2;
		ଯଦି(pos == 8 || pos == 13 || pos == 18 || pos == 23){
			clientid[auto_id_prefix_len + pos] = '-';
			pos++;
		}
	}

	ଫେରନ୍ତୁ clientid;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା connect__on_authorised(ସଂରଚନା mosquitto *context, ଶୂନ୍ୟ *auth_data_out, uint16_t auth_data_out_len)
{
	ସଂରଚନା mosquitto *found_context;
	ସଂରଚନା mosquitto__subleaf *leaf;
	mosquitto_property *connack_props = ଖାଲି;
	uint8_t connect_ack = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_quota, out_quota;
	uint16_t in_maximum, out_maximum;

	/* Find if this client already has an entry. This must be done *after* any security checks. */
	HASH_FIND(hh_id, db.contexts_by_id, context->id, strlen(context->id), found_context);
	ଯଦି(found_context){
		/* Found a matching client */
		ଯଦି(!net__is_connected(found_context)){
			/* Client is reconnecting after a disconnect */
			/* FIXME - does anything need to be done here? */
		}ଅନ୍ୟଥା{
			/* Client is already connected, disconnect old version. This is
			 * done in context__cleanup() below. */
		}

		ଯଦି(context->clean_start == true){
			sub__clean_session(found_context);
			found_context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			plugin_persist__handle_client_delete(found_context);
		}
		context->is_persisted = found_context->is_persisted;
		found_context->is_persisted = false; /* stops persistence for context being removed */

		ଯଦି(context->clean_start == false && found_context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
			ଯଦି(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
				connect_ack |= 0x01;
			}

			ଯଦି(found_context->msgs_in.inflight || found_context->msgs_in.queued
					|| found_context->msgs_out.inflight || found_context->msgs_out.queued){

				in_quota = context->msgs_in.inflight_quota;
				out_quota = context->msgs_out.inflight_quota;
				in_maximum = context->msgs_in.inflight_maximum;
				out_maximum = context->msgs_out.inflight_maximum;

				memcpy(&context->msgs_in, &found_context->msgs_in, ଆକାର(ସଂରଚନା mosquitto_ସନ୍ଦେଶ_data));
				memcpy(&context->msgs_out, &found_context->msgs_out, ଆକାର(ସଂରଚନା mosquitto_ସନ୍ଦେଶ_data));
				context->last_cmsg_id = found_context->last_cmsg_id;

				memset(&found_context->msgs_in, 0, ଆକାର(ସଂରଚନା mosquitto_ସନ୍ଦେଶ_data));
				memset(&found_context->msgs_out, 0, ଆକାର(ସଂରଚନା mosquitto_ସନ୍ଦେଶ_data));

				context->msgs_in.inflight_quota = in_quota;
				context->msgs_out.inflight_quota = out_quota;
				context->msgs_in.inflight_maximum = in_maximum;
				context->msgs_out.inflight_maximum = out_maximum;

				db__message_reconnect_reset(context);
			}
			context->subs = found_context->subs;
			found_context->subs = ଖାଲି;
			context->subs_capacity = found_context->subs_capacity;
			context->subs_ଗଣନା = found_context->subs_ଗଣନା;
			found_context->subs_capacity = 0;
			found_context->subs_ଗଣନା = 0;
			context->last_mid = found_context->last_mid;

			ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<context->subs_capacity; i++){
				ଯଦି(context->subs[i]){
					leaf = context->subs[i]->hier->subs;
					ଯେତେବେଳେ(leaf){
						ଯଦି(leaf->context == found_context){
							leaf->context = context;
						}
						leaf = leaf->next;
					}

					ଯଦି(context->subs[i]->shared){
						leaf = context->subs[i]->shared->subs;
						ଯେତେବେଳେ(leaf){
							ଯଦି(leaf->context == found_context){
								leaf->context = context;
							}
							leaf = leaf->next;
						}
					}
				}
			}
		}

		ଯଦି((found_context->protocol == mosq_p_mqtt5 && found_context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE)
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
		ଯଦି(found_context->protocol == mosq_p_mqtt5){
			send__disconnect(found_context, MQTT_RC_SESSION_TAKEN_OVER, ଖାଲି);
		}
		do_disconnect(found_context, MOSQ_ERR_SESSION_TAKEN_OVER);
	}

	ଯଦି(db.config->global_max_clients > 0 && HASH_CNT(hh_id, db.contexts_by_id) >= (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)db.config->global_max_clients){
		ଯଦି(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_SERVER_BUSY, ଖାଲି);
		}
		rc = MOSQ_ERR_INVAL;
		ଯାଅ error;
	}

	ଯଦି(db.config->connection_messages == true){
		ଯଦି(context->is_bridge){
			ଯଦି(context->username){
				log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive, context->username);
			}ଅନ୍ୟଥା{
				log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "New bridge connected from %s:%d as %s (p%d, c%d, k%d).",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive);
			}
		}ଅନ୍ୟଥା{
			ଯଦି(context->username){
				log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d, u'%s').",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive, context->username);
			}ଅନ୍ୟଥା{
				log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "New client connected from %s:%d as %s (p%d, c%d, k%d).",
						context->address, context->remote_port, context->id, context->protocol, context->clean_start, context->keepalive);
			}
		}

		ଯଦି(context->will){
			log__printf(ଖାଲି, MOSQ_LOG_DEBUG, "Will message specified (%ld bytes) (r%d, q%d).",
					(long)context->will->ସନ୍ଦେଶ.payloadlen,
					context->will->ସନ୍ଦେଶ.retain,
					context->will->ସନ୍ଦେଶ.qos);

			log__printf(ଖାଲି, MOSQ_LOG_DEBUG, "\t%s", context->will->ସନ୍ଦେଶ.topic);
		}ଅନ୍ୟଥା{
			log__printf(ଖାଲି, MOSQ_LOG_DEBUG, "No will message specified.");
		}
	}
#ifdef WITH_TLS
	ଯଦି(context->ssl){
		log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s negotiated %s cipher %s",
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
	ଯଦି(!context->clean_start){
		db.persistence_changes++;
	}
#endif
	context->max_qos = context->listener->max_qos;

	ଯଦି(db.config->max_keepalive &&
			(context->keepalive > db.config->max_keepalive || context->keepalive == 0)){

		keepalive__remove(context);
		context->keepalive = db.config->max_keepalive;
		keepalive__add(context);
		ଯଦି(context->protocol == mosq_p_mqtt5){
			ଯଦି(mosquitto_property_add_int16(&connack_props, MQTT_PROP_SERVER_KEEP_ALIVE, context->keepalive)){
				rc = MOSQ_ERR_NOMEM;
				ଯାଅ error;
			}
		}ଅନ୍ୟଥା{
			send__connack(context, connect_ack, CONNACK_REFUSED_IDENTIFIER_REJECTED, ଖାଲି);
			rc = MOSQ_ERR_INVAL;
			ଯାଅ error;
		}
	}


	ଯଦି(context->protocol == mosq_p_mqtt5){
		ଯଦି(context->listener->max_topic_alias > 0){
			ଯଦି(mosquitto_property_add_int16(&connack_props, MQTT_PROP_TOPIC_ALIAS_MAXIMUM, context->listener->max_topic_alias)){
				rc = MOSQ_ERR_NOMEM;
				ଯାଅ error;
			}
		}
		ଯଦି(context->assigned_id){
			ଯଦି(mosquitto_property_add_string(&connack_props, MQTT_PROP_ASSIGNED_CLIENT_IDENTIFIER, context->id)){
				rc = MOSQ_ERR_NOMEM;
				ଯାଅ error;
			}
		}
		ଯଦି(context->auth_method){
			ଯଦି(mosquitto_property_add_string(&connack_props, MQTT_PROP_AUTHENTICATION_METHOD, context->auth_method)){
				rc = MOSQ_ERR_NOMEM;
				ଯାଅ error;
			}

			ଯଦି(auth_data_out && auth_data_out_len > 0){
				ଯଦି(mosquitto_property_add_ଦ୍ୱିମିକ(&connack_props, MQTT_PROP_AUTHENTICATION_DATA, auth_data_out, auth_data_out_len)){
					rc = MOSQ_ERR_NOMEM;
					ଯାଅ error;
				}
			}
		}
	}
	SAFE_FREE(auth_data_out);

	mosquitto__set_state(context, mosq_cs_active);
	rc = send__connack(context, connect_ack, CONNACK_ACCEPTED, connack_props);
	mosquitto_property_free_all(&connack_props);
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}
	db__expire_all_messages(context);
	rc = db__message_write_queued_out(context);
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}
	rc = db__message_write_inflight_out_all(context);

	ଯଦି(rc == MOSQ_ERR_SUCCESS){
		plugin__handle_connect(context);

		ଯଦି(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
			plugin_persist__handle_client_add(context);
		}ଅନ୍ୟଥା ଯଦି(context->will){
			plugin_persist__handle_will_add(context);
		}
	}
	ଫେରନ୍ତୁ rc;
error:
	SAFE_FREE(auth_data_out);
	mosquitto_property_free_all(&connack_props);
	ଫେରନ୍ତୁ rc;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା will__read(ସଂରଚନା mosquitto *context, ଅଚଳ ଅକ୍ଷର *clientid, ସଂରଚନା mosquitto_message_all **will, uint8_t will_qos, ପୂର୍ଣ୍ଣସଂଖ୍ୟା will_retain)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = MOSQ_ERR_SUCCESS;
	size_t slen;
	uint16_t tlen;
	ସଂରଚନା mosquitto_message_all *will_struct = ଖାଲି;
	ଅକ୍ଷର *will_topic_mount = ଖାଲି;
	uint16_t payloadlen;
	mosquitto_property *properties = ଖାଲି;

	will_struct = mosquitto_calloc(1, ଆକାର(ସଂରଚନା mosquitto_message_all));
	ଯଦି(!will_struct){
		rc = MOSQ_ERR_NOMEM;
		ଯାଅ error_cleanup;
	}
	ଯଦି(context->protocol == PROTOCOL_VERSION_v5){
		rc = property__read_all(CMD_WILL, &context->in_packet, &properties);
		ଯଦି(rc){
			ଯାଅ error_cleanup;
		}

		rc = property__process_will(context, will_struct, &properties);
		mosquitto_property_free_all(&properties);
		ଯଦି(rc){
			ଯାଅ error_cleanup;
		}
	}
	rc = packet__read_string(&context->in_packet, &will_struct->ସନ୍ଦେଶ.topic, &tlen);
	ଯଦି(rc){
		ଯାଅ error_cleanup;
	}
	ଯଦି(!tlen){
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s: Will with empty topic.",
				context->id);
		rc = MOSQ_ERR_PROTOCOL;
		ଯାଅ error_cleanup;
	}

	ଯଦି(context->listener->mount_point){
		slen = strlen(context->listener->mount_point) + strlen(will_struct->ସନ୍ଦେଶ.topic) + 1;
		will_topic_mount = mosquitto_malloc(slen+1);
		ଯଦି(!will_topic_mount){
			rc = MOSQ_ERR_NOMEM;
			ଯାଅ error_cleanup;
		}

		snprintf(will_topic_mount, slen, "%s%s", context->listener->mount_point, will_struct->ସନ୍ଦେଶ.topic);
		will_topic_mount[slen] = '\0';

		mosquitto_FREE(will_struct->ସନ୍ଦେଶ.topic);
		will_struct->ସନ୍ଦେଶ.topic = will_topic_mount;
	}

	ଯଦି(!strncmp(will_struct->ସନ୍ଦେଶ.topic, "$CONTROL/", strlen("$CONTROL/"))){
		rc = MOSQ_ERR_ACL_DENIED;
		ଯାଅ error_cleanup;
	}
	rc = mosquitto_pub_topic_check(will_struct->ସନ୍ଦେଶ.topic);
	ଯଦି(rc){
		ଯାଅ error_cleanup;
	}

	rc = packet__read_uint16(&context->in_packet, &payloadlen);
	ଯଦି(rc){
		ଯାଅ error_cleanup;
	}

	will_struct->ସନ୍ଦେଶ.payloadlen = payloadlen;
	ଯଦି(will_struct->ସନ୍ଦେଶ.payloadlen > 0){
		ଯଦି(db.config->message_size_limit && will_struct->ସନ୍ଦେଶ.payloadlen > (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)db.config->message_size_limit){
			log__printf(ଖାଲି, MOSQ_LOG_DEBUG, "Client %s connected with too large Will payload", clientid);
			ଯଦି(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_PACKET_TOO_LARGE, ଖାଲି);
			}ଅନ୍ୟଥା{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ଖାଲି);
			}
			rc = MOSQ_ERR_PAYLOAD_SIZE;
			ଯାଅ error_cleanup;
		}
		will_struct->ସନ୍ଦେଶ.payload = mosquitto_malloc((size_t)will_struct->ସନ୍ଦେଶ.payloadlen);
		ଯଦି(!will_struct->ସନ୍ଦେଶ.payload){
			rc = MOSQ_ERR_NOMEM;
			ଯାଅ error_cleanup;
		}

		rc = packet__read_bytes(&context->in_packet, will_struct->ସନ୍ଦେଶ.payload, (uint32_t)will_struct->ସନ୍ଦେଶ.payloadlen);
		ଯଦି(rc){
			ଯାଅ error_cleanup;
		}
	}

	will_struct->ସନ୍ଦେଶ.qos = will_qos;
	will_struct->ସନ୍ଦେଶ.retain = will_retain;

	*will = will_struct;
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;

error_cleanup:
	ଯଦି(will_struct){
		mosquitto_FREE(will_struct->ସନ୍ଦେଶ.topic);
		mosquitto_FREE(will_struct->ସନ୍ଦେଶ.payload);
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct);
	}
	ଫେରନ୍ତୁ rc;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_protocol_version(ସଂରଚନା mosquitto__listener *listener, ପୂର୍ଣ୍ଣସଂଖ୍ୟା protocol_version)
{
	/* Allow bridge protocol as well. */
	protocol_version &= 0x7F;

	ଯଦି((protocol_version == 3 && listener->disable_protocol_v3 == false)
			|| (protocol_version == 4 && listener->disable_protocol_v4 == false)
			|| (protocol_version == 5 && listener->disable_protocol_v5 == false)
			){

		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ MOSQ_ERR_NOT_SUPPORTED;
	}
}


ଧାଡ଼ିଭିତରେ ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା send__connack_error_and_return(ସଂରଚନା mosquitto *context, uint8_t err_code, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc)
{
	send__connack(context, 0, err_code, ଖାଲି);
	ଫେରନ୍ତୁ rc;
}


ଧାଡ଼ିଭିତରେ ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା send__connack_bad_username_or_password_error(ସଂରଚନା mosquitto *context, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc)
{
	uint8_t err_code  = context->protocol == mosq_p_mqtt5
									? (uint8_t)MQTT_RC_BAD_USERNAME_OR_PASSWORD
									: (uint8_t)CONNACK_REFUSED_BAD_USERNAME_PASSWORD;
	ଫେରନ୍ତୁ send__connack_error_and_return(context, err_code, rc);
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_protocol_name(ସଂରଚନା mosquitto *context, ଅକ୍ଷର protocol_name[7])
{
	/* Read protocol name as length then bytes rather than with read_string
	 * because the length is fixed and we can check that. Removes the need
	 * for another malloc as well. */

	uint16_t slen = 0;

	ଯଦି(packet__read_uint16(&context->in_packet, &slen)){
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string length.",
				context->address, context->remote_port);
		ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
	}
	ଯଦି(slen != 4 /* MQTT */ && slen != 6 /* MQIsdp */){
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with incorrect protocol string length (%d).",
				context->address, context->remote_port, slen);
		ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
	}
	ଯଦି(packet__read_bytes(&context->in_packet, protocol_name, slen)){
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol string.",
				context->address, context->remote_port);
		ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
	}
	protocol_name[slen] = '\0';

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_and_verify_protocol_version(ସଂରଚନା mosquitto *context, ଅଚଳ ଅକ୍ଷର *protocol_name,
		uint8_t *protocol_version)
{
	uint8_t tmp_protocol_version = 0;
	ଯଦି(packet__read_byte(&context->in_packet, &tmp_protocol_version)){
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing protocol version.",
				context->address, context->remote_port);
		ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
	}

	ଯଦି(check_protocol_version(context->listener, tmp_protocol_version)){
		ଯଦି(tmp_protocol_version == 3 || tmp_protocol_version == 4){
			context->protocol = mosq_p_mqtt311;
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, ଖାଲି);
		}ଅନ୍ୟଥା{
			context->protocol = mosq_p_mqtt5;
			send__connack(context, 0, MQTT_RC_UNSUPPORTED_PROTOCOL_VERSION, ଖାଲି);
		}
		ଫେରନ୍ତୁ MOSQ_ERR_NOT_SUPPORTED;
	}

	ଯଦି(!strcmp(protocol_name, PROTOCOL_NAME_v31)){
		ଯଦି((tmp_protocol_version&0x7F) != PROTOCOL_VERSION_v31){
			ଯଦି(db.config->connection_messages == true){
				log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->address, context->remote_port, tmp_protocol_version);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, ଖାଲି);
			ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
		}
		context->protocol = mosq_p_mqtt31;
		ଯଦି((tmp_protocol_version&0x80) == 0x80){
			context->is_bridge = true;
		}
	}ଅନ୍ୟଥା ଯଦି(!strcmp(protocol_name, PROTOCOL_NAME)){
		ଯଦି((tmp_protocol_version&0x7F) == PROTOCOL_VERSION_v311){
			context->protocol = mosq_p_mqtt311;

			ଯଦି((tmp_protocol_version&0x80) == 0x80){
				context->is_bridge = true;
			}
		}ଅନ୍ୟଥା ଯଦି((tmp_protocol_version&0x7F) == PROTOCOL_VERSION_v5){
			context->protocol = mosq_p_mqtt5;
		}ଅନ୍ୟଥା{
			ଯଦି(db.config->connection_messages == true){
				log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol version (%d).",
						context->address, context->remote_port, tmp_protocol_version);
			}
			send__connack(context, 0, CONNACK_REFUSED_PROTOCOL_VERSION, ଖାଲି);
			ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
		}
		ଯଦି((context->in_packet.command&0x0F) != 0x00){
			/* Reserved flags not set to 0, must disconnect. */
			log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero reserved flags (%02X).",
					context->address, context->remote_port, context->in_packet.command);
			ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
		}
	}ଅନ୍ୟଥା{
		ଯଦି(db.config->connection_messages == true){
			log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid protocol \"%s\".",
					context->address, context->remote_port, protocol_name);
		}
		ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
	}
	ଯଦି((tmp_protocol_version&0x7F) != PROTOCOL_VERSION_v31 && context->in_packet.command != CMD_CONNECT){
		ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
	}

	*protocol_version = tmp_protocol_version;
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_and_verify_connect_flags(ସଂରଚନା mosquitto *context, uint8_t *connect_flags)
{
	ଯଦି(packet__read_byte(&context->in_packet, connect_flags)){
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with missing connect flags.",
				context->address, context->remote_port);
		ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
	}
	ଯଦି(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		ଯଦି((*connect_flags & 0x01) != 0x00){
			log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with non-zero connect reserved flag (%02X).",
					context->address, context->remote_port, *connect_flags);
			ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ଶୂନ୍ୟ set_session_expiry_interval(ସଂରଚନା mosquitto *context, uint8_t clean_start, uint8_t protocol_version)
{
	/* session_expiry_interval will be overridden if the properties are read later */
	ଯଦି(clean_start == false && protocol_version != PROTOCOL_VERSION_v5){
		/* v3* has clean_start == false mean the session never expires */
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
	}ଅନ୍ୟଥା{
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	}
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_and_reset_keepalive(ସଂରଚନା mosquitto *context)
{
	/* _remove here because net__socket_accept() uses _add and we must have the
	 * correct keepalive value */
	keepalive__remove(context);

	ଯଦି(packet__read_uint16(&context->in_packet, &(context->keepalive))){
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "%s sent CONNECT with missing keepalive.",
				context->id);
		ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
	}
	keepalive__add(context);

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_and_verify_v5_connect_properties(ସଂରଚନା mosquitto *context, mosquitto_property **properties, uint8_t protocol_version)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି(protocol_version == PROTOCOL_VERSION_v5){
		rc = property__read_all(CMD_CONNECT, &context->in_packet, properties);
		ଯଦି(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
			send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, ଖାଲି);
		}ଅନ୍ୟଥା ଯଦି(rc == MOSQ_ERR_MALFORMED_PACKET){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, ଖାଲି);
		}
		ଯଦି(rc){
			ଫେରନ୍ତୁ rc;
		}
	}
	rc = property__process_connect(context, properties);
	ଯଦି(rc != MOSQ_ERR_SUCCESS){
		ଫେରନ୍ତୁ send__connack_error_and_return(context, MQTT_RC_PROTOCOL_ERROR, rc);
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା verify_will_options(ସଂରଚନା mosquitto *context, uint8_t will, uint8_t will_qos, uint8_t will_retain, uint8_t protocol_version)
{
	ଯଦି(will_qos == 3){
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with invalid Will QoS (%d).",
				context->address, context->remote_port, will_qos);
		ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
	}

	ଯଦି(will && will_retain && db.config->retain_available == false){
		ଯଦି(protocol_version == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_RETAIN_NOT_SUPPORTED, ଖାଲି);
		}
		ଫେରନ୍ତୁ MOSQ_ERR_NOT_SUPPORTED;
	}

	ଯଦି(will && will_qos > context->listener->max_qos){
		ଯଦି(protocol_version == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_QOS_NOT_SUPPORTED, ଖାଲି);
		}
		ଫେରନ୍ତୁ MOSQ_ERR_NOT_SUPPORTED;
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା handle_zero_length_clientid(ସଂରଚନା mosquitto *context, ଅକ୍ଷର **clientid, bool *allow_zero_length_clientid,
		uint8_t clean_start)
{
	ଯଦି(context->protocol == mosq_p_mqtt31){
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s:%d: v3.1 CONNECT with zero length clientid.",
				context->address, context->remote_port);
		send__connack(context, 0, CONNACK_REFUSED_IDENTIFIER_REJECTED, ଖାଲି);
		ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
	}

	/* mqtt311/mqtt5 */
	mosquitto_FREE(*clientid);

	ଯଦି(db.config->per_listener_settings){
		*allow_zero_length_clientid = context->listener->security_options->allow_zero_length_clientid;
	}ଅନ୍ୟଥା{
		*allow_zero_length_clientid = db.config->security_options.allow_zero_length_clientid;
	}

	ଯଦି((context->protocol == mosq_p_mqtt311 && clean_start == 0) || *allow_zero_length_clientid == false){
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s:%d: CONNECT with zero length clientid when forbidden.",
				context->address, context->remote_port);
		uint8_t err_code = context->protocol == mosq_p_mqtt311 ? (uint8_t)CONNACK_REFUSED_IDENTIFIER_REJECTED : (uint8_t)MQTT_RC_UNSPECIFIED;
		ଫେରନ୍ତୁ send__connack_error_and_return(context, err_code, MOSQ_ERR_PROTOCOL);
	}

	*clientid = clientid_gen(&(uint16_t){0}, context->listener->security_options->auto_id_prefix,
			context->listener->security_options->auto_id_prefix_len);
	ଯଦି(*clientid == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}
	context->assigned_id = true;

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_clientid_prefixes(ସଂରଚନା mosquitto *context, ଅଚଳ ଅକ୍ଷର *clientid)
{
	ଯଦି(db.config->clientid_prefixes){
		ଯଦି(strncmp(db.config->clientid_prefixes, clientid, strlen(db.config->clientid_prefixes))){
			uint8_t err_code = context->protocol == mosq_p_mqtt5 ? (uint8_t)MQTT_RC_NOT_AUTHORIZED : (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
			ଫେରନ୍ତୁ send__connack_error_and_return(context, err_code, MOSQ_ERR_AUTH);
		}
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_and_verify_clientid_from_packet(ସଂରଚନା mosquitto *context, ଅକ୍ଷର **clientid,
		bool *allow_zero_length_clientid, uint8_t clean_start)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	uint16_t slen;

	rc = packet__read_string(&context->in_packet, clientid, &slen);
	ଯଦି(rc == MOSQ_ERR_MALFORMED_UTF8){
		ଯଦି(context->protocol == mosq_p_mqtt5){
			send__connack(context, 0, MQTT_RC_CLIENTID_NOT_VALID, ଖାଲି);
		}
		ଫେରନ୍ତୁ MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID;
	}ଅନ୍ୟଥା ଯଦି(rc > 0){
		ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
	}

	ଯଦି(slen == 0){
		rc = handle_zero_length_clientid(context, clientid, allow_zero_length_clientid, clean_start);
		ଯଦି(rc != MOSQ_ERR_SUCCESS){
			ଫେରନ୍ତୁ rc;
		}
	}

	rc = check_clientid_prefixes(context, *clientid);
	ଯଦି(rc != MOSQ_ERR_SUCCESS){
		ଫେରନ୍ତୁ rc;
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_username_from_packet(ସଂରଚନା mosquitto *context, ଅକ୍ଷର **username, ଅଚଳ ଅକ୍ଷର *clientid)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	rc = packet__read_string(&context->in_packet, username, &(uint16_t){0});
	ଯଦି(rc == MOSQ_ERR_NOMEM){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}
	ଯଦି(rc != MOSQ_ERR_SUCCESS){
		ଯଦି(context->protocol == mosq_p_mqtt31){
			/* Username flag given, but no username. Ignore. */
			/* NOTE: Removed setting of username_flag to zero as it is unused afterwards */
		}ଅନ୍ୟଥା{
			log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with username flag but no username.",
					clientid);
			ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_password_from_packet(ସଂରଚନା mosquitto *context, ଅକ୍ଷର **password, uint16_t *password_len, ଅଚଳ ଅକ୍ଷର *clientid)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	rc = packet_read_ଦ୍ୱିମିକ(&context->in_packet, (uint8_t **)password, password_len);
	ଯଦି(rc == MOSQ_ERR_NOMEM){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	ଯଦି(rc == MOSQ_ERR_MALFORMED_PACKET){
		ଯଦି(context->protocol == mosq_p_mqtt31){
			/* Password flag given, but no password. Ignore. */
		}ଅନ୍ୟଥା{
			log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT with password flag but no password.",
					clientid);
			ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_and_verify_client_credentials_from_packet(ସଂରଚନା mosquitto *context,
		ଅକ୍ଷର **username, uint8_t username_flag,
		ଅକ୍ଷର **password, uint16_t *password_len, uint8_t password_flag,
		ଅଚଳ ଅକ୍ଷର *clientid)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି(username_flag){
		rc = set_username_from_packet(context, username, clientid);
		ଯଦି(rc != MOSQ_ERR_SUCCESS){
			ଫେରନ୍ତୁ rc;
		}
	}ଅନ୍ୟଥା{
		ଯଦି(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
			ଯଦି(password_flag){
				/* username_flag == 0 && password_flag == 1 is forbidden */
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Protocol error from %s: password without username, closing connection.", clientid);
				ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
			}
		}
	}
	ଯଦି(password_flag){
		rc = set_password_from_packet(context, password, password_len, clientid);
		ଯଦି(rc != MOSQ_ERR_SUCCESS){
			ଫେରନ୍ତୁ rc;
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_additional_trailing_data(ସଂରଚନା mosquitto *context, uint8_t protocol_version)
{
	ଯଦି(context->in_packet.pos != context->in_packet.remaining_length){
		/* Surplus data at end of packet, this must be an error. */
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->in_packet.pos, context->in_packet.remaining_length);
		ଯଦି(protocol_version == PROTOCOL_VERSION_v5){
			send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, ଖାଲି);
		}
		ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}

#ifdef WITH_TLS


ଧାଡ଼ିଭିତରେ ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_client_cert_and_subject_name(ସଂରଚନା mosquitto *context, X509 **client_cert, X509_NAME **name)
{
	*client_cert = SSL_get_peer_certificate(context->ssl);
	ଯଦି(*client_cert == ଖାଲି){
		ଫେରନ୍ତୁ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	*name = X509_get_subject_name(*client_cert);
	ଯଦି(*name == ଖାଲି){
		X509_free(*client_cert);
		ଫେରନ୍ତୁ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ଧାଡ଼ିଭିତରେ ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା free_x509_and_send_connack_error(ସଂରଚନା mosquitto *context, X509 *client_cert, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc)
{
	X509_free(client_cert);
	ଫେରନ୍ତୁ send__connack_bad_username_or_password_error(context, rc);
}


ଧାଡ଼ିଭିତରେ ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା free_x509_and_BIO_and_send_connack_error(ସଂରଚନା mosquitto *context, X509 *client_cert,
		BIO *subject_name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc)
{
	BIO_free(subject_name);
	ଫେରନ୍ତୁ free_x509_and_send_connack_error(context, client_cert, rc);
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_username_from_cert_identity(ସଂରଚନା mosquitto *context)
{
	X509 *client_cert = ଖାଲି;
	X509_NAME *name = ଖାଲି;

	ଯଦି(get_client_cert_and_subject_name(context, &client_cert, &name)){
		ଫେରନ୍ତୁ MOSQ_ERR_AUTH;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = -1;
	X509_NAME_ENTRY *name_entry = ଖାଲି;

	i = X509_NAME_get_index_by_NID(name, NID_commonName, -1);
	ଯଦି(i == -1){
		ଫେରନ୍ତୁ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	name_entry = X509_NAME_get_entry(name, i);
	ଯଦି(!name_entry){
		ଯାଅ success;
	}

	ASN1_STRING *name_asn1 = ଖାଲି;
	name_asn1 = X509_NAME_ENTRY_get_data(name_entry);
	ଯଦି(name_asn1 == ଖାଲି){
		ଫେରନ୍ତୁ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	ଅଚଳ ଅକ୍ଷର *cert_identity = (ଅଚଳ ଅକ୍ଷର *)ASN1_STRING_get0_data(name_asn1);
	ଯଦି(!cert_identity || mosquitto_validate_utf8(cert_identity, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)strlen(cert_identity))){
		ଫେରନ୍ତୁ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	mosquitto_free(context->username);
	context->username = mosquitto_strdup(cert_identity);
	ଯଦି(context->username == ଖାଲି){
		ଫେରନ୍ତୁ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_NOMEM);
	}
	/* Make sure there isn't an embedded NUL character in the CN */
	ଯଦି((size_t)ASN1_STRING_length(name_asn1) != strlen(context->username)){
		ଫେରନ୍ତୁ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}

success:
	X509_free(client_cert);
	client_cert = ଖାଲି;
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_username_from_cert_subject_name(ସଂରଚନା mosquitto *context)
{
	X509 *client_cert = ଖାଲି;
	X509_NAME *name = ଖାଲି;

	ଯଦି(get_client_cert_and_subject_name(context, &client_cert, &name)){
		ଫେରନ୍ତୁ MOSQ_ERR_AUTH;
	}

	ଅକ୍ଷର *subject = ଖାଲି;
	ଅକ୍ଷର *data_start = ଖାଲି;
	BIO *subject_bio = ଖାଲି;
	long name_length = 0;

	subject_bio = BIO_new(BIO_s_mem());
	X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
	data_start = ଖାଲି;
	name_length = BIO_get_mem_data(subject_bio, &data_start);
	subject = mosquitto_malloc(ଆକାର(ଅକ୍ଷର)*(size_t)(name_length+1));
	ଯଦି(!subject){
		ଫେରନ୍ତୁ free_x509_and_BIO_and_send_connack_error(context, client_cert, subject_bio, MOSQ_ERR_NOMEM);
	}
	memcpy(subject, data_start, (size_t)name_length);
	subject[name_length] = '\0';
	BIO_free(subject_bio);

	ଯଦି(mosquitto_validate_utf8(subject, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)strlen(subject))){
		mosquitto_free(subject);
		ଫେରନ୍ତୁ free_x509_and_send_connack_error(context, client_cert, MOSQ_ERR_AUTH);
	}
	context->username = subject;
	ଯଦି(!context->username){
		X509_free(client_cert);
		ଫେରନ୍ତୁ MOSQ_ERR_AUTH;
	}

	X509_free(client_cert);
	client_cert = ଖାଲି;
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}
#endif


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା handle_username_from_cert_options(ସଂରଚନା mosquitto *context, ଅକ୍ଷର **username, ଅକ୍ଷର **password, uint16_t password_len)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

#ifdef WITH_TLS
	ଯଦି(context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
		/* Don't need the username or password if provided */
		mosquitto_FREE(*username);
		mosquitto_FREE(*password);

		ଯଦି(!context->ssl){
			ଫେରନ୍ତୁ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
		}
#ifdef FINAL_WITH_TLS_PSK
		ଯଦି(context->listener->psk_hint){
			/* Client should have provided an identity to get this far. */
			ଯଦି(!context->username){
				ଫେରନ୍ତୁ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}ଅନ୍ୟଥା
#endif /* FINAL_WITH_TLS_PSK */
		{
			ଯଦି(context->listener->use_identity_as_username){
				rc = set_username_from_cert_identity(context);
			}ଅନ୍ୟଥା{   /* use_subject_as_username */
				rc = set_username_from_cert_subject_name(context);
			}
			ଯଦି(rc){
				ଫେରନ୍ତୁ rc;
			}
		}
	}ଅନ୍ୟଥା
#endif /* WITH_TLS */
	{
#ifdef WITH_TLS
		ଯଦି(context->listener->use_identity_as_username && context->listener->require_certificate){
			mosquitto_FREE(*username);
			mosquitto_FREE(*password);

			ଯଦି(!context->username){
				ଫେରନ୍ତୁ send__connack_bad_username_or_password_error(context, MOSQ_ERR_AUTH);
			}
		}ଅନ୍ୟଥା
#endif
		{
			/* FIXME - these ensure the mosquitto_clientid() and
			* mosquitto_client_username() functions work, but is hacky */
			context->username = *username;
			context->password = *password;
			context->password_len = password_len;
			*username = ଖାଲି; /* Avoid free() in error: below. */
			*password = ଖାଲି;
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା handle_username_as_clientid_option(ସଂରଚନା mosquitto *context)
{
	ଯଦି(context->username){
		mosquitto_FREE(context->id);
		context->id = mosquitto_strdup(context->username);
		ଯଦି(!context->id){
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}
	}ଅନ୍ୟଥା{
		uint8_t err_code = context->protocol == mosq_p_mqtt5
												? (uint8_t)MQTT_RC_NOT_AUTHORIZED
												: (uint8_t)CONNACK_REFUSED_NOT_AUTHORIZED;
		ଫେରନ୍ତୁ send__connack_error_and_return(context, err_code, MOSQ_ERR_AUTH);
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା handle__connect(ସଂରଚନା mosquitto *context)
{
	ଅକ୍ଷର protocol_name[7];
	uint8_t protocol_version;
	uint8_t connect_flags;
	ଅକ୍ଷର *clientid = ଖାଲି;
	ସଂରଚନା mosquitto *found_context;
	ସଂରଚନା mosquitto_message_all *will_struct = ଖାଲି;
	uint8_t will, will_retain, will_qos, clean_start;
	uint8_t username_flag, password_flag;
	ଅକ୍ଷର *username = ଖାଲି, *password = ଖାଲି;
	uint16_t password_len = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	mosquitto_property *properties = ଖାଲି;
	ଶୂନ୍ୟ *auth_data = ଖାଲି;
	uint16_t auth_data_len = 0;
	ଶୂନ୍ୟ *auth_data_out = ଖାଲି;
	uint16_t auth_data_out_len = 0;
	bool allow_zero_length_clientid;

	ଯଦି(!context->listener){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	/* Don't accept multiple CONNECT commands. */
	ଯଦି(context->state != mosq_cs_new){
		log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Bad client %s:%d sending multiple CONNECT messages.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		ଯାଅ handle_connect_error;
	}

#ifdef WITH_TLS
	ଯଦି(context->in_packet.command == 0x16 && context->listener->ssl_ctx == ଖାଲି){ /* 0x16 is TLS handshake client hello */
		log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client from %s:%d appears to be using TLS to connect to a non-TLS listener.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		ଯାଅ handle_connect_error;
	}
#endif

	rc = read_protocol_name(context, protocol_name);
	ଯଦି(rc != MOSQ_ERR_SUCCESS){
		ଯାଅ handle_connect_error;
	}

	rc = read_and_verify_protocol_version(context, protocol_name, &protocol_version);
	ଯଦି(rc != MOSQ_ERR_SUCCESS){
		ଯଦି(rc == MOSQ_ERR_MALFORMED_PACKET){
			ଫେରନ୍ତୁ rc;
		}
		ଯାଅ handle_connect_error;
	}

	rc = read_and_verify_connect_flags(context, &connect_flags);
	ଯଦି(rc != MOSQ_ERR_SUCCESS){
		ଯାଅ handle_connect_error;
	}

	clean_start = (connect_flags & 0x02) >> 1;
	set_session_expiry_interval(context, clean_start, protocol_version);

	rc = read_and_reset_keepalive(context);
	ଯଦି(rc != MOSQ_ERR_SUCCESS){
		ଯାଅ handle_connect_error;
	}

	rc = read_and_verify_v5_connect_properties(context, &properties, protocol_version);
	ଯଦି(rc != MOSQ_ERR_SUCCESS){
		ଯାଅ handle_connect_error;
	}

	will = connect_flags & 0x04;
	will_qos = (connect_flags & 0x18) >> 3;
	will_retain = ((connect_flags & 0x20) == 0x20);
	rc = verify_will_options(context, will, will_qos, will_retain, protocol_version);
	ଯଦି(rc != MOSQ_ERR_SUCCESS){
		ଯାଅ handle_connect_error;
	}

	mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_METHOD, &context->auth_method, false);
	mosquitto_property_read_ଦ୍ୱିମିକ(properties, MQTT_PROP_AUTHENTICATION_DATA, &auth_data, &auth_data_len, false);
	mosquitto_property_free_all(&properties);

	ଯଦି(auth_data && !context->auth_method){
		send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, ଖାଲି);
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s:%d CONNECT with missing clientid string.",
				context->address, context->remote_port);
		rc = MOSQ_ERR_PROTOCOL;
		ଯାଅ handle_connect_error;
	}

	rc = read_and_verify_clientid_from_packet(context, &clientid, &allow_zero_length_clientid, clean_start);
	ଯଦି(rc != MOSQ_ERR_SUCCESS){
		ଯାଅ handle_connect_error;
	}

	ଯଦି(will){
		rc = will__read(context, clientid, &will_struct, will_qos, will_retain);
		ଯଦି(rc){
			ଯଦି(context->protocol == mosq_p_mqtt5){
				ଯଦି(rc == MOSQ_ERR_DUPLICATE_PROPERTY || rc == MOSQ_ERR_PROTOCOL){
					send__connack(context, 0, MQTT_RC_PROTOCOL_ERROR, ଖାଲି);
				}ଅନ୍ୟଥା ଯଦି(rc == MOSQ_ERR_MALFORMED_PACKET){
					send__connack(context, 0, MQTT_RC_MALFORMED_PACKET, ଖାଲି);
				}
			}
			ଯାଅ handle_connect_error;
		}
	}ଅନ୍ୟଥା{
		ଯଦି(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
			ଯଦି(will_qos != 0 || will_retain != 0){
				log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT without Will with non-zero QoS (%d) or retain (%d).",
						clientid, will_qos, will_retain);
				rc = MOSQ_ERR_PROTOCOL;
				ଯାଅ handle_connect_error;
			}
		}
	}

	// Client credentials
	password_flag = connect_flags & 0x40;
	username_flag = connect_flags & 0x80;
	rc = read_and_verify_client_credentials_from_packet(context, &username, username_flag, &password, &password_len, password_flag, clientid);
	ଯଦି(rc != MOSQ_ERR_SUCCESS){
		ଯାଅ handle_connect_error;
	}

	rc = check_additional_trailing_data(context, protocol_version);
	ଯଦି(rc != MOSQ_ERR_SUCCESS){
		ଯାଅ handle_connect_error;
	}

	/* Once context->id is set, if we return from this function with an error
	 * we must make sure that context->id is freed and set to NULL, so that the
	 * client isn't erroneously removed from the by_id hash table. */
	context->id = clientid;
	clientid = ଖାଲି;

	/* use_identity_as_username or use_subject_as_username */
	rc = handle_username_from_cert_options(context, &username, &password, password_len);
	ଯଦି(rc != MOSQ_ERR_SUCCESS){
		ଯାଅ handle_connect_error;
	}

	/* use_username_as_clientid */
	ଯଦି(context->listener->use_username_as_clientid){
		rc = handle_username_as_clientid_option(context);
		ଯଦି(rc != MOSQ_ERR_SUCCESS){
			ଯାଅ handle_connect_error;
		}
	}

	/* Check for an existing delayed auth check, reject if present */
	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), found_context);
	ଯଦି(found_context){
		rc = MOSQ_ERR_UNKNOWN;
		ଯାଅ handle_connect_error;
	}

	context->clean_start = clean_start;
	context->will = will_struct;
	will_struct = ଖାଲି;

	ଯଦି(context->auth_method){
		rc = mosquitto_security_auth_start(context, false, auth_data, auth_data_len, &auth_data_out, &auth_data_out_len);
		mosquitto_FREE(auth_data);
		ଯଦି(rc == MOSQ_ERR_SUCCESS){
			ଫେରନ୍ତୁ connect__on_authorised(context, auth_data_out, auth_data_out_len);
		}ଅନ୍ୟଥା ଯଦି(rc == MOSQ_ERR_AUTH_CONTINUE){
			mosquitto__set_state(context, mosq_cs_authenticating);
			rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_data_out, auth_data_out_len);
			SAFE_FREE(auth_data_out);
			ଫେରନ୍ତୁ rc;
		}ଅନ୍ୟଥା{
			SAFE_FREE(auth_data_out);
			will__clear(context);
			ଯଦି(rc == MOSQ_ERR_AUTH){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ଖାଲି);
				mosquitto_FREE(context->id);
				ଯାଅ handle_connect_error;
			}ଅନ୍ୟଥା ଯଦି(rc == MOSQ_ERR_NOT_SUPPORTED){
				/* Client has requested extended authentication, but we don't support it. */
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_METHOD, ଖାଲି);
				mosquitto_FREE(context->id);
				ଯାଅ handle_connect_error;
			}ଅନ୍ୟଥା{
				mosquitto_FREE(context->id);
				ଯାଅ handle_connect_error;
			}
		}
	}ଅନ୍ୟଥା{
#ifdef WITH_TLS
		ଯଦି(context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
			/* Authentication assumed to be cleared */
		}ଅନ୍ୟଥା
#endif
		{
			rc = mosquitto_basic_auth(context);
			ବାଛ(rc){
				ସ୍ଥିତି MOSQ_ERR_SUCCESS:
					ଭାଙ୍ଗ;
				ସ୍ଥିତି MOSQ_ERR_AUTH_DELAYED:
					mosquitto__set_state(context, mosq_cs_delayed_auth);
					HASH_ADD_KEYPTR(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context);
					ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
					ଭାଙ୍ଗ;
				ସ୍ଥିତି MOSQ_ERR_AUTH:
					ଯଦି(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ଖାଲି);
					}ଅନ୍ୟଥା{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ଖାଲି);
					}
					ଯାଅ handle_connect_error;
					ଭାଙ୍ଗ;
				ସ୍ଥିତି MOSQ_ERR_UNSPECIFIED:
				ସ୍ଥିତି MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
				ସ୍ଥିତି MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
				ସ୍ଥିତି MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
				ସ୍ଥିତି MOSQ_ERR_SERVER_UNAVAILABLE:
				ସ୍ଥିତି MOSQ_ERR_SERVER_BUSY:
				ସ୍ଥିତି MOSQ_ERR_BANNED:
				ସ୍ଥିତି MOSQ_ERR_BAD_AUTHENTICATION_METHOD:
				ସ୍ଥିତି MOSQ_ERR_CONNECTION_RATE_EXCEEDED:
					ଯଦି(context->protocol == mosq_p_mqtt5){
						send__connack(context, 0, (uint8_t)rc, ଖାଲି);
					}ଅନ୍ୟଥା{
						send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ଖାଲି);
					}
					ଯାଅ handle_connect_error;
					ଭାଙ୍ଗ;
				ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
					rc = MOSQ_ERR_UNKNOWN;
					ଯାଅ handle_connect_error;
					ଭାଙ୍ଗ;
			}
		}
		ଫେରନ୍ତୁ connect__on_authorised(context, ଖାଲି, 0);
	}


handle_connect_error:
	mosquitto_property_free_all(&properties);
	mosquitto_FREE(auth_data);
	mosquitto_FREE(clientid);
	mosquitto_FREE(username);
	mosquitto_FREE(password);
	ଯଦି(will_struct){
		mosquitto_property_free_all(&will_struct->properties);
		mosquitto_FREE(will_struct->ସନ୍ଦେଶ.payload);
		mosquitto_FREE(will_struct->ସନ୍ଦେଶ.topic);
		mosquitto_FREE(will_struct);
	}
	will__clear(context);
	/* We return an error here which means the client is freed later on. */
	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->will_delay_interval = 0;
	ଫେରନ୍ତୁ rc;
}
