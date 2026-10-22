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

#अन्तर्भाव <time.h>
#यदि defined(__APPLE__) || defined(_AIX)
#अन्तर्भाव <sys/socket.h>
#यदि_अन्तः

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "alias_mosq.h"
#अन्तर्भाव "packet_mosq.h"
#अन्तर्भाव "property_mosq.h"
#अन्तर्भाव "sys_tree.h"
#अन्तर्भाव "util_mosq.h"
#अन्तर्भाव "will_mosq.h"

#अन्तर्भाव "uthash.h"


पूर्णाङ्क context__init_sock(संरचना mosquitto *context, mosq_sock_t sock, bool get_स्थानाङ्कः)
{
	context->sock = sock;

	यदि((पूर्णाङ्क)context->sock >= 0){
		यदि(get_स्थानाङ्कः){
			वर्ण स्थानाङ्कः[1024];

			यदि(!net_socket_get_स्थानाङ्कः(context->sock,
					स्थानाङ्कः, परिमाण(स्थानाङ्कः),
					&context->remote_पत्तनम्)){

				context->स्थानाङ्कः = mosquitto_strdup(स्थानाङ्कः);
			}
			यदि(!context->स्थानाङ्कः){
				/* getpeername and inet_ntop failed and not a bridge */
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
		}
		HASH_सङ्कलय(hh_sock, db.contexts_by_sock, sock, परिमाण(context->sock), context);
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}

संरचना mosquitto *context__init(शून्य)
{
	संरचना mosquitto *context;

	context = mosquitto_calloc(1, परिमाण(संरचना mosquitto));
	यदि(!context){
		प्रत्यागम रिक्त;
	}

	context->अन्तर्_packet.packet_बफर_आकार = db.config->packet_बफर_आकार;
	context->अन्तर्_packet.packet_बफर = mosquitto_calloc(1, context->अन्तर्_packet.packet_बफर_आकार);
	यदि(!context->अन्तर्_packet.packet_बफर){
		mosquitto_FREE(context);
		प्रत्यागम रिक्त;
	}

#यदि defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	context->ident = id_client;
#अन्यथा
	context->pollfd_सूचकांक = -1;
#यदि_अन्तः
	mosquitto_समुच्चयः_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->चरमः_सन्देशः_अन्तर् = db.now_s;
	context->अग्र_सन्देशः_बहिर्गमनम् = db.now_s + 20;
	context->keepalive = 20; /* Default to 20s */
	context->clean_start = true;
	context->id = रिक्त;
	context->चरमः_mid = 0;
	context->will = रिक्त;
	context->username = रिक्त;
	context->password = रिक्त;
	context->listener = रिक्त;
	context->acl_list = रिक्त;
	context->retain_उपलब्धम् = true;
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	स्मृतिं_पूरय(&context->wsd, 0, परिमाण(context->wsd));
	context->wsd.opcode = UINT8_MAX;
	context->wsd.आवरणम् = UINT8_MAX;
	context->wsd.disconnect_reason = 0xE8;
#यदि_अन्तः

	/* is_bridge records whether this client is a bridge or not. This could be
	 * done by looking at context->bridge for bridges that we create ourself,
	 * but incoming bridges need some other way of being recorded. */
	context->is_bridge = false;

	context->अन्तर्_packet.payload = रिक्त;
	packet__cleanup(&context->अन्तर्_packet);
	context->बहिर्गमनम्_packet = रिक्त;
	context->बहिर्गमनम्_packet_गणनम् = 0;
	context->बहिर्गमनम्_packet_अष्टकानि = 0;

	context->स्थानाङ्कः = रिक्त;
	context->bridge = रिक्त;
	context->msgs_अन्तर्.inflight_अधिकतमम् = db.config->max_inflight_messages;
	context->msgs_अन्तर्.inflight_quota = db.config->max_inflight_messages;
	context->msgs_बहिर्गमनम्.inflight_अधिकतमम् = db.config->max_inflight_messages;
	context->msgs_बहिर्गमनम्.inflight_quota = db.config->max_inflight_messages;
	context->max_qos = 2;
#यदि_परिभाषितम् WITH_TLS
	context->ssl = रिक्त;
#यदि_अन्तः

	प्रत्यागम context;
}


स्थिर शून्य context_cleanup_बहिर्गमनम्_packets(संरचना mosquitto *context)
{
	संरचना mosquitto__packet *packet;

	यदि(!context){
		प्रत्यागम;
	}

	यावत्(context->बहिर्गमनम्_packet){
		packet = context->बहिर्गमनम्_packet;
		context->बहिर्गमनम्_packet = context->बहिर्गमनम्_packet->अग्र;
		mosquitto_free(packet);
	}
	metrics__int_dec(mosq_gauge_बहिर्गमनम्_packets, context->बहिर्गमनम्_packet_गणनम्);
	metrics__int_dec(mosq_gauge_बहिर्गमनम्_packet_अष्टकानि, context->बहिर्गमनम्_packet_अष्टकानि);
	context->बहिर्गमनम्_packet_गणनम् = 0;
	context->बहिर्गमनम्_packet_अष्टकानि = 0;
}


/*
 * This will result in any outgoing packets going unsent. If we're disconnected
 * forcefully then it is usually an error condition and shouldn't be a problem,
 * but it will mean that CONNACK messages will never get sent for bad protocol
 * versions for example.
 */
शून्य context__cleanup(संरचना mosquitto *context, bool force_free)
{
	यदि(!context){
		प्रत्यागम;
	}

	यदि(force_free){
		context->clean_start = true;
	}

#यदि_परिभाषितम् WITH_BRIDGE
	यदि(context->bridge){
		bridge__cleanup(context);
	}
#यदि_अन्तः
	mosquitto_FREE(context->अन्तर्_packet.packet_बफर);
	context->अन्तर्_packet.packet_बफर_आकार = 0;

	alias__free_all(context);
	keepalive__remove(context);
	context_cleanup_बहिर्गमनम्_packets(context);

	mosquitto_FREE(context->auth_विधानम्);
	mosquitto_FREE(context->username);
	mosquitto_FREE(context->password);

	net__socket_close(context);
	यदि(force_free){
		sub__clean_session(context);
	}
	db__messages_delete(context, force_free);

	mosquitto_FREE(context->स्थानाङ्कः);

	context__send_will(context);

	यदि(context->id){
		context__remove_from_by_id(context);
		mosquitto_FREE(context->id);
	}
	packet__cleanup(&(context->अन्तर्_packet));
	context_cleanup_बहिर्गमनम्_packets(context);
#यदि defined(WITH_BROKER) && defined(__GLIBC__) && defined(WITH_ADNS)
	यदि(context->adns){
		gai_cancel(context->adns);
		संरचना addrinfo *ar_request = (संरचना addrinfo *)context->adns->ar_request;
		mosquitto_FREE(ar_request);
		mosquitto_FREE(context->adns);
	}
#यदि_अन्तः

#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	mosquitto_FREE(context->http_request);
#यदि_अन्तः
	mosquitto_FREE(context->proxy.buf);
	यदि(force_free){
		mosquitto_FREE(context);
	}
}


शून्य context__send_will(संरचना mosquitto *ctxt)
{
	यदि(ctxt->state != mosq_cs_disconnecting && ctxt->will){
		यदि(ctxt->session_expiry_interval > 0 && ctxt->will_delay_interval > 0){
			will_delay_सङ्कलय(ctxt);
			प्रत्यागम;
		}

		यदि(mosquitto_acl_check(ctxt,
				ctxt->will->सन्देशः.topic,
				(uint32_t)ctxt->will->सन्देशः.payloadlen,
				ctxt->will->सन्देशः.payload,
				(uint8_t)ctxt->will->सन्देशः.qos,
				ctxt->will->सन्देशः.retain,
				ctxt->will->properties,
				MOSQ_ACL_WRITE) == MOSQ_ERR_SUCCESS){

			/* Unexpected disconnect, queue the client will. */
			db__messages_easy_queue(ctxt,
					ctxt->will->सन्देशः.topic,
					(uint8_t)ctxt->will->सन्देशः.qos,
					(uint32_t)ctxt->will->सन्देशः.payloadlen,
					ctxt->will->सन्देशः.payload,
					ctxt->will->सन्देशः.retain,
					ctxt->will->expiry_interval,
					&ctxt->will->properties);
		}
	}
	will__clear(ctxt);
}


शून्य context__disconnect(संरचना mosquitto *context, पूर्णाङ्क reason)
{
	यदि(mosquitto__get_state(context) == mosq_cs_disconnected){
		प्रत्यागम;
	}

#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	यदि(context->transport == mosq_t_ws){
		uint8_t buf[4] = {0x88, 0x02, 0x03, context->wsd.disconnect_reason};
		/* Send the disconnect reason, but don't care if it fails */
		यदि(send(context->sock, buf, 4, 0)){
		}
	}
#यदि_अन्तः
	यदि(context->id){
		संरचना mosquitto *context_प्राप्तम्;
		HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, वाक्यदैर्घ्यम्(context->id), context_प्राप्तम्);
		यदि(context_प्राप्तम् == context){
			net__socket_close(context);
			context_सङ्कलय_गन्तव्यम्_disused(context);
			प्रत्यागम;
		}
	}

	context__send_will(context);

	यदि(context->session_expiry_interval == MQTT_SESSION_EXPIRY_तात्कालिकम्){
		plugin__handle_disconnect(context, reason);
	}अन्यथा{
		plugin__handle_client_offline(context, reason);
	}

	net__socket_close(context);
#यदि_परिभाषितम् WITH_BRIDGE
	यदि(context->bridge == रिक्त)
	/* Outgoing bridge connection never expire */
#यदि_अन्तः
	{
		यदि(context->session_expiry_interval == MQTT_SESSION_EXPIRY_तात्कालिकम्){
			plugin_persist__handle_client_delete(context);
			/* Client session is due to be expired now */
			यदि(context->will_delay_interval == 0){
				/* This will be done later, after the will is published for delay>0. */
				context_सङ्कलय_गन्तव्यम्_disused(context);
			}
		}अन्यथा{
			session_expiry_सङ्कलय(context);
		}
	}
	keepalive__remove(context);
	mosquitto_समुच्चयः_state(context, mosq_cs_disconnected);
	alias__free_all(context);
	context_cleanup_बहिर्गमनम्_packets(context);
}


शून्य context_सङ्कलय_गन्तव्यम्_disused(संरचना mosquitto *context)
{
	यदि(context->state == mosq_cs_disused){
		प्रत्यागम;
	}

	mosquitto_समुच्चयः_state(context, mosq_cs_disused);

	context__remove_from_by_id(context);

	context->for_free_अग्र = db.ll_for_free;
	db.ll_for_free = context;
}


शून्य context__free_disused(शून्य)
{
	संरचना mosquitto *context, *अग्र;
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	संरचना mosquitto *चरमः = रिक्त;
#यदि_अन्तः

	context = db.ll_for_free;
	db.ll_for_free = रिक्त;
	यावत्(context){
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		यदि(context->wsi){
			/* Don't delete yet, lws hasn't finished with it */
			यदि(चरमः){
				चरमः->for_free_अग्र = context;
			}अन्यथा{
				db.ll_for_free = context;
			}
			अग्र = context->for_free_अग्र;
			context->for_free_अग्र = रिक्त;
			चरमः = context;
			context = अग्र;
		}अन्यथा
#यदि_अन्तः
		{
			अग्र = context->for_free_अग्र;
			context__cleanup(context, true);
			context = अग्र;
		}
	}
}


शून्य context_सङ्कलय_गन्तव्यम्_by_id(संरचना mosquitto *context)
{
	यदि(context->अन्तर्_by_id == false){
		context->अन्तर्_by_id = true;
		HASH_मानम्(context->id, वाक्यदैर्घ्यम्(context->id), context->id_hashv);
		HASH_सङ्कलय_KEYPTR_BYHASHVALUE(hh_id, db.contexts_by_id, context->id, वाक्यदैर्घ्यम्(context->id), context->id_hashv, context);
	}
}


शून्य context__remove_from_by_id(संरचना mosquitto *context)
{
	संरचना mosquitto *context_प्राप्तम्;

	यदि(!context->id){
		प्रत्यागम;
	}

	यदि(context->अन्तर्_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, context->id, वाक्यदैर्घ्यम्(context->id), context_प्राप्तम्);
		यदि(context_प्राप्तम् == context){
			HASH_DELETE(hh_id, db.contexts_by_id, context_प्राप्तम्);
		}
		context->id_hashv = 0;
		context->अन्तर्_by_id = false;
		प्रत्यागम;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, वाक्यदैर्घ्यम्(context->id), context_प्राप्तम्);
	यदि(context_प्राप्तम् == context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context_प्राप्तम्);
	}
}
