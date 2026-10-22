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

#شامل "config.h"

#شامل <time.h>
#اگر defined(__APPLE__) || defined(_AIX)
#شامل <sys/socket.h>
#اگر_ختم

#شامل "mosquitto_broker_internal.h"
#شامل "alias_mosq.h"
#شامل "packet_mosq.h"
#شامل "property_mosq.h"
#شامل "sys_tree.h"
#شامل "util_mosq.h"
#شامل "will_mosq.h"

#شامل "uthash.h"


صحیح context__init_sock(ساخت mosquitto *context, mosq_sock_t sock, bool get_پتہ)
{
	context->sock = sock;

	اگر((صحیح)context->sock >= 0){
		اگر(get_پتہ){
			حرف پتہ[1024];

			اگر(!net_socket_get_پتہ(context->sock,
					پتہ, حجم(پتہ),
					&context->remote_پورٹ)){

				context->پتہ = mosquitto_strdup(پتہ);
			}
			اگر(!context->پتہ){
				/* getpeername and inet_ntop failed and not a bridge */
				واپس MOSQ_ERR_NOMEM;
			}
		}
		HASH_جوڑو(hh_sock, db.contexts_by_sock, sock, حجم(context->sock), context);
	}
	واپس MOSQ_ERR_SUCCESS;
}

ساخت mosquitto *context__init(خالی)
{
	ساخت mosquitto *context;

	context = mosquitto_calloc(1, حجم(ساخت mosquitto));
	اگر(!context){
		واپس تہی;
	}

	context->اندر_packet.packet_بفر_سائز = db.config->packet_بفر_سائز;
	context->اندر_packet.packet_بفر = mosquitto_calloc(1, context->اندر_packet.packet_بفر_سائز);
	اگر(!context->اندر_packet.packet_بفر){
		mosquitto_FREE(context);
		واپس تہی;
	}

#اگر defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	context->ident = id_client;
#ورنہ
	context->pollfd_اشاریہ = -1;
#اگر_ختم
	mosquitto_سیٹ_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->آخری_پیغام_اندر = db.now_s;
	context->اگلا_پیغام_برآمد = db.now_s + 20;
	context->keepalive = 20; /* Default to 20s */
	context->clean_آغاز = true;
	context->id = تہی;
	context->آخری_mid = 0;
	context->will = تہی;
	context->username = تہی;
	context->password = تہی;
	context->listener = تہی;
	context->acl_list = تہی;
	context->retain_دستیاب = true;
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	memset(&context->wsd, 0, حجم(context->wsd));
	context->wsd.opcode = UINT8_MAX;
	context->wsd.نقاب = UINT8_MAX;
	context->wsd.disconnect_reason = 0xE8;
#اگر_ختم

	/* is_bridge records whether this client is a bridge or not. This could be
	 * done by looking at context->bridge for bridges that we create ourself,
	 * but incoming bridges need some other way of being recorded. */
	context->is_bridge = false;

	context->اندر_packet.payload = تہی;
	packet__cleanup(&context->اندر_packet);
	context->برآمد_packet = تہی;
	context->برآمد_packet_گنتی = 0;
	context->برآمد_packet_بائٹیں = 0;

	context->پتہ = تہی;
	context->bridge = تہی;
	context->msgs_اندر.inflight_زیادہ_سے_زیادہ = db.config->max_inflight_messages;
	context->msgs_اندر.inflight_quota = db.config->max_inflight_messages;
	context->msgs_برآمد.inflight_زیادہ_سے_زیادہ = db.config->max_inflight_messages;
	context->msgs_برآمد.inflight_quota = db.config->max_inflight_messages;
	context->max_qos = 2;
#اگر_تعریف_شدہ WITH_TLS
	context->ssl = تہی;
#اگر_ختم

	واپس context;
}


ساکن خالی context_cleanup_برآمد_packets(ساخت mosquitto *context)
{
	ساخت mosquitto__packet *packet;

	اگر(!context){
		واپس;
	}

	جب(context->برآمد_packet){
		packet = context->برآمد_packet;
		context->برآمد_packet = context->برآمد_packet->اگلا;
		mosquitto_free(packet);
	}
	metrics__int_dec(mosq_gauge_برآمد_packets, context->برآمد_packet_گنتی);
	metrics__int_dec(mosq_gauge_برآمد_packet_بائٹیں, context->برآمد_packet_بائٹیں);
	context->برآمد_packet_گنتی = 0;
	context->برآمد_packet_بائٹیں = 0;
}


/*
 * This will result in any outgoing packets going unsent. If we're disconnected
 * forcefully then it is usually an error condition and shouldn't be a problem,
 * but it will mean that CONNACK messages will never get sent for bad protocol
 * versions for example.
 */
خالی context__cleanup(ساخت mosquitto *context, bool force_free)
{
	اگر(!context){
		واپس;
	}

	اگر(force_free){
		context->clean_آغاز = true;
	}

#اگر_تعریف_شدہ WITH_BRIDGE
	اگر(context->bridge){
		bridge__cleanup(context);
	}
#اگر_ختم
	mosquitto_FREE(context->اندر_packet.packet_بفر);
	context->اندر_packet.packet_بفر_سائز = 0;

	alias__free_all(context);
	keepalive__remove(context);
	context_cleanup_برآمد_packets(context);

	mosquitto_FREE(context->auth_طریقہ);
	mosquitto_FREE(context->username);
	mosquitto_FREE(context->password);

	net__socket_close(context);
	اگر(force_free){
		sub__clean_session(context);
	}
	db__messages_delete(context, force_free);

	mosquitto_FREE(context->پتہ);

	context__send_will(context);

	اگر(context->id){
		context__remove_from_by_id(context);
		mosquitto_FREE(context->id);
	}
	packet__cleanup(&(context->اندر_packet));
	context_cleanup_برآمد_packets(context);
#اگر defined(WITH_BROKER) && defined(__GLIBC__) && defined(WITH_ADNS)
	اگر(context->adns){
		gai_cancel(context->adns);
		ساخت addrinfo *ar_request = (ساخت addrinfo *)context->adns->ar_request;
		mosquitto_FREE(ar_request);
		mosquitto_FREE(context->adns);
	}
#اگر_ختم

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	mosquitto_FREE(context->http_request);
#اگر_ختم
	mosquitto_FREE(context->proxy.buf);
	اگر(force_free){
		mosquitto_FREE(context);
	}
}


خالی context__send_will(ساخت mosquitto *ctxt)
{
	اگر(ctxt->state != mosq_cs_disconnecting && ctxt->will){
		اگر(ctxt->session_expiry_interval > 0 && ctxt->will_delay_interval > 0){
			will_delay_جوڑو(ctxt);
			واپس;
		}

		اگر(mosquitto_acl_check(ctxt,
				ctxt->will->پیغام.topic,
				(uint32_t)ctxt->will->پیغام.payloadlen,
				ctxt->will->پیغام.payload,
				(uint8_t)ctxt->will->پیغام.qos,
				ctxt->will->پیغام.retain,
				ctxt->will->properties,
				MOSQ_ACL_WRITE) == MOSQ_ERR_SUCCESS){

			/* Unexpected disconnect, queue the client will. */
			db_messages_easy_قطار(ctxt,
					ctxt->will->پیغام.topic,
					(uint8_t)ctxt->will->پیغام.qos,
					(uint32_t)ctxt->will->پیغام.payloadlen,
					ctxt->will->پیغام.payload,
					ctxt->will->پیغام.retain,
					ctxt->will->expiry_interval,
					&ctxt->will->properties);
		}
	}
	will__clear(ctxt);
}


خالی context__disconnect(ساخت mosquitto *context, صحیح reason)
{
	اگر(mosquitto__get_state(context) == mosq_cs_disconnected){
		واپس;
	}

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	اگر(context->transport == mosq_t_ws){
		uint8_t buf[4] = {0x88, 0x02, 0x03, context->wsd.disconnect_reason};
		/* Send the disconnect reason, but don't care if it fails */
		اگر(send(context->sock, buf, 4, 0)){
		}
	}
#اگر_ختم
	اگر(context->id){
		ساخت mosquitto *context_ملا;
		HASH_ڈھونڈو(hh_id, db.contexts_by_id_delayed_auth, context->id, لڑی_لمبائی(context->id), context_ملا);
		اگر(context_ملا == context){
			net__socket_close(context);
			context_جوڑو_مقصد_disused(context);
			واپس;
		}
	}

	context__send_will(context);

	اگر(context->session_expiry_interval == MQTT_SESSION_EXPIRY_فوری){
		plugin__handle_disconnect(context, reason);
	}ورنہ{
		plugin__handle_client_offline(context, reason);
	}

	net__socket_close(context);
#اگر_تعریف_شدہ WITH_BRIDGE
	اگر(context->bridge == تہی)
	/* Outgoing bridge connection never expire */
#اگر_ختم
	{
		اگر(context->session_expiry_interval == MQTT_SESSION_EXPIRY_فوری){
			plugin_persist__handle_client_delete(context);
			/* Client session is due to be expired now */
			اگر(context->will_delay_interval == 0){
				/* This will be done later, after the will is published for delay>0. */
				context_جوڑو_مقصد_disused(context);
			}
		}ورنہ{
			session_expiry_جوڑو(context);
		}
	}
	keepalive__remove(context);
	mosquitto_سیٹ_state(context, mosq_cs_disconnected);
	alias__free_all(context);
	context_cleanup_برآمد_packets(context);
}


خالی context_جوڑو_مقصد_disused(ساخت mosquitto *context)
{
	اگر(context->state == mosq_cs_disused){
		واپس;
	}

	mosquitto_سیٹ_state(context, mosq_cs_disused);

	context__remove_from_by_id(context);

	context->for_free_اگلا = db.ll_for_free;
	db.ll_for_free = context;
}


خالی context__free_disused(خالی)
{
	ساخت mosquitto *context, *اگلا;
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ساخت mosquitto *آخری = تہی;
#اگر_ختم

	context = db.ll_for_free;
	db.ll_for_free = تہی;
	جب(context){
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		اگر(context->wsi){
			/* Don't delete yet, lws hasn't finished with it */
			اگر(آخری){
				آخری->for_free_اگلا = context;
			}ورنہ{
				db.ll_for_free = context;
			}
			اگلا = context->for_free_اگلا;
			context->for_free_اگلا = تہی;
			آخری = context;
			context = اگلا;
		}ورنہ
#اگر_ختم
		{
			اگلا = context->for_free_اگلا;
			context__cleanup(context, true);
			context = اگلا;
		}
	}
}


خالی context_جوڑو_مقصد_by_id(ساخت mosquitto *context)
{
	اگر(context->اندر_by_id == false){
		context->اندر_by_id = true;
		HASH_قدر(context->id, لڑی_لمبائی(context->id), context->id_hashv);
		HASH_جوڑو_KEYPTR_BYHASHVALUE(hh_id, db.contexts_by_id, context->id, لڑی_لمبائی(context->id), context->id_hashv, context);
	}
}


خالی context__remove_from_by_id(ساخت mosquitto *context)
{
	ساخت mosquitto *context_ملا;

	اگر(!context->id){
		واپس;
	}

	اگر(context->اندر_by_id){
		HASH_ڈھونڈو(hh_id, db.contexts_by_id, context->id, لڑی_لمبائی(context->id), context_ملا);
		اگر(context_ملا == context){
			HASH_DELETE(hh_id, db.contexts_by_id, context_ملا);
		}
		context->id_hashv = 0;
		context->اندر_by_id = false;
		واپس;
	}

	HASH_ڈھونڈو(hh_id, db.contexts_by_id_delayed_auth, context->id, لڑی_لمبائی(context->id), context_ملا);
	اگر(context_ملا == context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context_ملا);
	}
}
