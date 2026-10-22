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

#समावेश <time.h>
#अगर defined(__APPLE__) || defined(_AIX)
#समावेश <sys/socket.h>
#पूर्ण_अगर

#समावेश "mosquitto_broker_internal.h"
#समावेश "alias_mosq.h"
#समावेश "packet_mosq.h"
#समावेश "property_mosq.h"
#समावेश "sys_tree.h"
#समावेश "util_mosq.h"
#समावेश "will_mosq.h"

#समावेश "uthash.h"


पूर्णांक context__init_sock(काष्ठा mosquitto *context, mosq_sock_t sock, bool get_address)
{
	context->sock = sock;

	अगर((पूर्णांक)context->sock >= 0){
		अगर(get_address){
			अक्षर address[1024];

			अगर(!net__socket_get_address(context->sock,
					address, माप(address),
					&context->remote_port)){

				context->address = mosquitto_strdup(address);
			}
			अगर(!context->address){
				/* getpeername and inet_ntop failed and not a bridge */
				वापस MOSQ_ERR_NOMEM;
			}
		}
		HASH_ADD(hh_sock, db.contexts_by_sock, sock, माप(context->sock), context);
	}
	वापस MOSQ_ERR_SUCCESS;
}

काष्ठा mosquitto *context__init(व्योम)
{
	काष्ठा mosquitto *context;

	context = mosquitto_calloc(1, माप(काष्ठा mosquitto));
	अगर(!context){
		वापस शून्य;
	}

	context->in_packet.packet_बफर_आकार = db.config->packet_बफर_आकार;
	context->in_packet.packet_बफर = mosquitto_calloc(1, context->in_packet.packet_बफर_आकार);
	अगर(!context->in_packet.packet_बफर){
		mosquitto_FREE(context);
		वापस शून्य;
	}

#अगर defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	context->ident = id_client;
#अन्यथा
	context->pollfd_सूचकांक = -1;
#पूर्ण_अगर
	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->last_संदेश_in = db.now_s;
	context->अग्र_संदेश_out = db.now_s + 20;
	context->keepalive = 20; /* Default to 20s */
	context->clean_start = true;
	context->id = शून्य;
	context->last_mid = 0;
	context->will = शून्य;
	context->username = शून्य;
	context->password = शून्य;
	context->listener = शून्य;
	context->acl_list = शून्य;
	context->retain_available = true;
#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	स_रखो(&context->wsd, 0, माप(context->wsd));
	context->wsd.opcode = UINT8_MAX;
	context->wsd.mask = UINT8_MAX;
	context->wsd.disconnect_reason = 0xE8;
#पूर्ण_अगर

	/* is_bridge records whether this client is a bridge or not. This could be
	 * done by looking at context->bridge for bridges that we create ourself,
	 * but incoming bridges need some other way of being recorded. */
	context->is_bridge = false;

	context->in_packet.payload = शून्य;
	packet__cleanup(&context->in_packet);
	context->out_packet = शून्य;
	context->out_packet_गिनती = 0;
	context->out_packet_bytes = 0;

	context->address = शून्य;
	context->bridge = शून्य;
	context->msgs_in.inflight_maximum = db.config->max_inflight_messages;
	context->msgs_in.inflight_quota = db.config->max_inflight_messages;
	context->msgs_out.inflight_maximum = db.config->max_inflight_messages;
	context->msgs_out.inflight_quota = db.config->max_inflight_messages;
	context->max_qos = 2;
#अगर_घोषित WITH_TLS
	context->ssl = शून्य;
#पूर्ण_अगर

	वापस context;
}


अटल व्योम context__cleanup_out_packets(काष्ठा mosquitto *context)
{
	काष्ठा mosquitto__packet *packet;

	अगर(!context){
		वापस;
	}

	जबतक(context->out_packet){
		packet = context->out_packet;
		context->out_packet = context->out_packet->अग्र;
		mosquitto_free(packet);
	}
	metrics__int_dec(mosq_gauge_out_packets, context->out_packet_गिनती);
	metrics__int_dec(mosq_gauge_out_packet_bytes, context->out_packet_bytes);
	context->out_packet_गिनती = 0;
	context->out_packet_bytes = 0;
}


/*
 * This will result in any outgoing packets going unsent. If we're disconnected
 * forcefully then it is usually an error condition and shouldn't be a problem,
 * but it will mean that CONNACK messages will never get sent for bad protocol
 * versions for example.
 */
व्योम context__cleanup(काष्ठा mosquitto *context, bool force_free)
{
	अगर(!context){
		वापस;
	}

	अगर(force_free){
		context->clean_start = true;
	}

#अगर_घोषित WITH_BRIDGE
	अगर(context->bridge){
		bridge__cleanup(context);
	}
#पूर्ण_अगर
	mosquitto_FREE(context->in_packet.packet_बफर);
	context->in_packet.packet_बफर_आकार = 0;

	alias__free_all(context);
	keepalive__remove(context);
	context__cleanup_out_packets(context);

	mosquitto_FREE(context->auth_method);
	mosquitto_FREE(context->username);
	mosquitto_FREE(context->password);

	net__socket_close(context);
	अगर(force_free){
		sub__clean_session(context);
	}
	db__messages_delete(context, force_free);

	mosquitto_FREE(context->address);

	context__send_will(context);

	अगर(context->id){
		context__remove_from_by_id(context);
		mosquitto_FREE(context->id);
	}
	packet__cleanup(&(context->in_packet));
	context__cleanup_out_packets(context);
#अगर defined(WITH_BROKER) && defined(__GLIBC__) && defined(WITH_ADNS)
	अगर(context->adns){
		gai_cancel(context->adns);
		काष्ठा addrinfo *ar_request = (काष्ठा addrinfo *)context->adns->ar_request;
		mosquitto_FREE(ar_request);
		mosquitto_FREE(context->adns);
	}
#पूर्ण_अगर

#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	mosquitto_FREE(context->http_request);
#पूर्ण_अगर
	mosquitto_FREE(context->proxy.buf);
	अगर(force_free){
		mosquitto_FREE(context);
	}
}


व्योम context__send_will(काष्ठा mosquitto *ctxt)
{
	अगर(ctxt->state != mosq_cs_disconnecting && ctxt->will){
		अगर(ctxt->session_expiry_interval > 0 && ctxt->will_delay_interval > 0){
			will_delay__add(ctxt);
			वापस;
		}

		अगर(mosquitto_acl_check(ctxt,
				ctxt->will->संदेश.topic,
				(uint32_t)ctxt->will->संदेश.payloadlen,
				ctxt->will->संदेश.payload,
				(uint8_t)ctxt->will->संदेश.qos,
				ctxt->will->संदेश.retain,
				ctxt->will->properties,
				MOSQ_ACL_WRITE) == MOSQ_ERR_SUCCESS){

			/* Unexpected disconnect, queue the client will. */
			db__messages_easy_queue(ctxt,
					ctxt->will->संदेश.topic,
					(uint8_t)ctxt->will->संदेश.qos,
					(uint32_t)ctxt->will->संदेश.payloadlen,
					ctxt->will->संदेश.payload,
					ctxt->will->संदेश.retain,
					ctxt->will->expiry_interval,
					&ctxt->will->properties);
		}
	}
	will__clear(ctxt);
}


व्योम context__disconnect(काष्ठा mosquitto *context, पूर्णांक reason)
{
	अगर(mosquitto__get_state(context) == mosq_cs_disconnected){
		वापस;
	}

#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	अगर(context->transport == mosq_t_ws){
		uint8_t buf[4] = {0x88, 0x02, 0x03, context->wsd.disconnect_reason};
		/* Send the disconnect reason, but don't care if it fails */
		अगर(send(context->sock, buf, 4, 0)){
		}
	}
#पूर्ण_अगर
	अगर(context->id){
		काष्ठा mosquitto *context_found;
		HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, म_माप(context->id), context_found);
		अगर(context_found == context){
			net__socket_close(context);
			context__add_to_disused(context);
			वापस;
		}
	}

	context__send_will(context);

	अगर(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
		plugin__handle_disconnect(context, reason);
	}अन्यथा{
		plugin__handle_client_offline(context, reason);
	}

	net__socket_close(context);
#अगर_घोषित WITH_BRIDGE
	अगर(context->bridge == शून्य)
	/* Outgoing bridge connection never expire */
#पूर्ण_अगर
	{
		अगर(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
			plugin_persist__handle_client_delete(context);
			/* Client session is due to be expired now */
			अगर(context->will_delay_interval == 0){
				/* This will be done later, after the will is published for delay>0. */
				context__add_to_disused(context);
			}
		}अन्यथा{
			session_expiry__add(context);
		}
	}
	keepalive__remove(context);
	mosquitto__set_state(context, mosq_cs_disconnected);
	alias__free_all(context);
	context__cleanup_out_packets(context);
}


व्योम context__add_to_disused(काष्ठा mosquitto *context)
{
	अगर(context->state == mosq_cs_disused){
		वापस;
	}

	mosquitto__set_state(context, mosq_cs_disused);

	context__remove_from_by_id(context);

	context->for_free_अग्र = db.ll_for_free;
	db.ll_for_free = context;
}


व्योम context__free_disused(व्योम)
{
	काष्ठा mosquitto *context, *अग्र;
#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	काष्ठा mosquitto *last = शून्य;
#पूर्ण_अगर

	context = db.ll_for_free;
	db.ll_for_free = शून्य;
	जबतक(context){
#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		अगर(context->wsi){
			/* Don't delete yet, lws hasn't finished with it */
			अगर(last){
				last->for_free_अग्र = context;
			}अन्यथा{
				db.ll_for_free = context;
			}
			अग्र = context->for_free_अग्र;
			context->for_free_अग्र = शून्य;
			last = context;
			context = अग्र;
		}अन्यथा
#पूर्ण_अगर
		{
			अग्र = context->for_free_अग्र;
			context__cleanup(context, true);
			context = अग्र;
		}
	}
}


व्योम context__add_to_by_id(काष्ठा mosquitto *context)
{
	अगर(context->in_by_id == false){
		context->in_by_id = true;
		HASH_मूल्य(context->id, म_माप(context->id), context->id_hashv);
		HASH_ADD_KEYPTR_BYHASHVALUE(hh_id, db.contexts_by_id, context->id, म_माप(context->id), context->id_hashv, context);
	}
}


व्योम context__remove_from_by_id(काष्ठा mosquitto *context)
{
	काष्ठा mosquitto *context_found;

	अगर(!context->id){
		वापस;
	}

	अगर(context->in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, context->id, म_माप(context->id), context_found);
		अगर(context_found == context){
			HASH_DELETE(hh_id, db.contexts_by_id, context_found);
		}
		context->id_hashv = 0;
		context->in_by_id = false;
		वापस;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, म_माप(context->id), context_found);
	अगर(context_found == context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context_found);
	}
}
