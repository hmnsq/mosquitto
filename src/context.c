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

#ضمن "config.h"

#ضمن <time.h>
#إذا defined(__APPLE__) || defined(_AIX)
#ضمن <sys/socket.h>
#پایان_اگر

#ضمن "mosquitto_broker_internal.h"
#ضمن "alias_mosq.h"
#ضمن "packet_mosq.h"
#ضمن "property_mosq.h"
#ضمن "sys_tree.h"
#ضمن "util_mosq.h"
#ضمن "will_mosq.h"

#ضمن "uthash.h"


صحيح context__init_sock(بنية mosquitto *context, mosq_sock_t sock, bool get_آدرس)
{
	context->sock = sock;

	إذا((صحيح)context->sock >= 0){
		إذا(get_آدرس){
			حرف آدرس[1024];

			إذا(!net_socket_get_آدرس(context->sock,
					آدرس, حجم(آدرس),
					&context->remote_پورت)){

				context->آدرس = mosquitto_strdup(آدرس);
			}
			إذا(!context->آدرس){
				/* getpeername and inet_ntop failed and not a bridge */
				أرجع MOSQ_ERR_NOMEM;
			}
		}
		HASH_جمع_کن(hh_sock, db.contexts_by_sock, sock, حجم(context->sock), context);
	}
	أرجع MOSQ_ERR_SUCCESS;
}

بنية mosquitto *context__init(فراغ)
{
	بنية mosquitto *context;

	context = mosquitto_calloc(1, حجم(بنية mosquitto));
	إذا(!context){
		أرجع عدم;
	}

	context->درون_packet.packet_میانگیر_اندازه = db.config->packet_میانگیر_اندازه;
	context->درون_packet.packet_میانگیر = mosquitto_calloc(1, context->درون_packet.packet_میانگیر_اندازه);
	إذا(!context->درون_packet.packet_میانگیر){
		mosquitto_FREE(context);
		أرجع عدم;
	}

#إذا defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	context->ident = id_client;
#وإلا
	context->pollfd_شاخص = -1;
#پایان_اگر
	mosquitto_مجموعه_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->آخرین_پیام_درون = db.now_s;
	context->بعدی_پیام_بیرون = db.now_s + 20;
	context->keepalive = 20; /* Default to 20s */
	context->clean_شروع = true;
	context->id = عدم;
	context->آخرین_mid = 0;
	context->will = عدم;
	context->username = عدم;
	context->password = عدم;
	context->listener = عدم;
	context->acl_list = عدم;
	context->retain_موجود = true;
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	memset(&context->wsd, 0, حجم(context->wsd));
	context->wsd.opcode = UINT8_MAX;
	context->wsd.ماسک = UINT8_MAX;
	context->wsd.disconnect_reason = 0xE8;
#پایان_اگر

	/* is_bridge records whether this client is a bridge or not. This could be
	 * done by looking at context->bridge for bridges that we create ourself,
	 * but incoming bridges need some other way of being recorded. */
	context->is_bridge = false;

	context->درون_packet.payload = عدم;
	packet__cleanup(&context->درون_packet);
	context->بیرون_packet = عدم;
	context->بیرون_packet_شمار = 0;
	context->بیرون_packet_بایتها = 0;

	context->آدرس = عدم;
	context->bridge = عدم;
	context->msgs_درون.inflight_بیشینه = db.config->max_inflight_messages;
	context->msgs_درون.inflight_quota = db.config->max_inflight_messages;
	context->msgs_بیرون.inflight_بیشینه = db.config->max_inflight_messages;
	context->msgs_بیرون.inflight_quota = db.config->max_inflight_messages;
	context->max_qos = 2;
#اگر_تعریف_شده WITH_TLS
	context->ssl = عدم;
#پایان_اگر

	أرجع context;
}


ساكن فراغ context_cleanup_بیرون_packets(بنية mosquitto *context)
{
	بنية mosquitto__packet *packet;

	إذا(!context){
		أرجع;
	}

	طالما(context->بیرون_packet){
		packet = context->بیرون_packet;
		context->بیرون_packet = context->بیرون_packet->بعدی;
		mosquitto_free(packet);
	}
	metrics__int_dec(mosq_gauge_بیرون_packets, context->بیرون_packet_شمار);
	metrics__int_dec(mosq_gauge_بیرون_packet_بایتها, context->بیرون_packet_بایتها);
	context->بیرون_packet_شمار = 0;
	context->بیرون_packet_بایتها = 0;
}


/*
 * This will result in any outgoing packets going unsent. If we're disconnected
 * forcefully then it is usually an error condition and shouldn't be a problem,
 * but it will mean that CONNACK messages will never get sent for bad protocol
 * versions for example.
 */
فراغ context__cleanup(بنية mosquitto *context, bool force_free)
{
	إذا(!context){
		أرجع;
	}

	إذا(force_free){
		context->clean_شروع = true;
	}

#اگر_تعریف_شده WITH_BRIDGE
	إذا(context->bridge){
		bridge__cleanup(context);
	}
#پایان_اگر
	mosquitto_FREE(context->درون_packet.packet_میانگیر);
	context->درون_packet.packet_میانگیر_اندازه = 0;

	alias__free_all(context);
	keepalive__remove(context);
	context_cleanup_بیرون_packets(context);

	mosquitto_FREE(context->auth_روش);
	mosquitto_FREE(context->username);
	mosquitto_FREE(context->password);

	net__socket_close(context);
	إذا(force_free){
		sub__clean_session(context);
	}
	db__messages_delete(context, force_free);

	mosquitto_FREE(context->آدرس);

	context__send_will(context);

	إذا(context->id){
		context__remove_from_by_id(context);
		mosquitto_FREE(context->id);
	}
	packet__cleanup(&(context->درون_packet));
	context_cleanup_بیرون_packets(context);
#إذا defined(WITH_BROKER) && defined(__GLIBC__) && defined(WITH_ADNS)
	إذا(context->adns){
		gai_cancel(context->adns);
		بنية addrinfo *ar_request = (بنية addrinfo *)context->adns->ar_request;
		mosquitto_FREE(ar_request);
		mosquitto_FREE(context->adns);
	}
#پایان_اگر

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	mosquitto_FREE(context->http_request);
#پایان_اگر
	mosquitto_FREE(context->proxy.buf);
	إذا(force_free){
		mosquitto_FREE(context);
	}
}


فراغ context__send_will(بنية mosquitto *ctxt)
{
	إذا(ctxt->state != mosq_cs_disconnecting && ctxt->will){
		إذا(ctxt->session_expiry_interval > 0 && ctxt->will_delay_interval > 0){
			will_delay_جمع_کن(ctxt);
			أرجع;
		}

		إذا(mosquitto_acl_check(ctxt,
				ctxt->will->پیام.topic,
				(uint32_t)ctxt->will->پیام.payloadlen,
				ctxt->will->پیام.payload,
				(uint8_t)ctxt->will->پیام.qos,
				ctxt->will->پیام.retain,
				ctxt->will->properties,
				MOSQ_ACL_WRITE) == MOSQ_ERR_SUCCESS){

			/* Unexpected disconnect, queue the client will. */
			db_messages_easy_صف(ctxt,
					ctxt->will->پیام.topic,
					(uint8_t)ctxt->will->پیام.qos,
					(uint32_t)ctxt->will->پیام.payloadlen,
					ctxt->will->پیام.payload,
					ctxt->will->پیام.retain,
					ctxt->will->expiry_interval,
					&ctxt->will->properties);
		}
	}
	will__clear(ctxt);
}


فراغ context__disconnect(بنية mosquitto *context, صحيح reason)
{
	إذا(mosquitto__get_state(context) == mosq_cs_disconnected){
		أرجع;
	}

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	إذا(context->transport == mosq_t_ws){
		uint8_t buf[4] = {0x88, 0x02, 0x03, context->wsd.disconnect_reason};
		/* Send the disconnect reason, but don't care if it fails */
		إذا(send(context->sock, buf, 4, 0)){
		}
	}
#پایان_اگر
	إذا(context->id){
		بنية mosquitto *context_یافت;
		HASH_پیدا_کن(hh_id, db.contexts_by_id_delayed_auth, context->id, طول_رشته(context->id), context_یافت);
		إذا(context_یافت == context){
			net__socket_close(context);
			context_جمع_کن_مقصد_disused(context);
			أرجع;
		}
	}

	context__send_will(context);

	إذا(context->session_expiry_interval == MQTT_SESSION_EXPIRY_فوری){
		plugin__handle_disconnect(context, reason);
	}وإلا{
		plugin__handle_client_offline(context, reason);
	}

	net__socket_close(context);
#اگر_تعریف_شده WITH_BRIDGE
	إذا(context->bridge == عدم)
	/* Outgoing bridge connection never expire */
#پایان_اگر
	{
		إذا(context->session_expiry_interval == MQTT_SESSION_EXPIRY_فوری){
			plugin_persist__handle_client_delete(context);
			/* Client session is due to be expired now */
			إذا(context->will_delay_interval == 0){
				/* This will be done later, after the will is published for delay>0. */
				context_جمع_کن_مقصد_disused(context);
			}
		}وإلا{
			session_expiry_جمع_کن(context);
		}
	}
	keepalive__remove(context);
	mosquitto_مجموعه_state(context, mosq_cs_disconnected);
	alias__free_all(context);
	context_cleanup_بیرون_packets(context);
}


فراغ context_جمع_کن_مقصد_disused(بنية mosquitto *context)
{
	إذا(context->state == mosq_cs_disused){
		أرجع;
	}

	mosquitto_مجموعه_state(context, mosq_cs_disused);

	context__remove_from_by_id(context);

	context->for_free_بعدی = db.ll_for_free;
	db.ll_for_free = context;
}


فراغ context__free_disused(فراغ)
{
	بنية mosquitto *context, *بعدی;
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	بنية mosquitto *آخرین = عدم;
#پایان_اگر

	context = db.ll_for_free;
	db.ll_for_free = عدم;
	طالما(context){
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		إذا(context->wsi){
			/* Don't delete yet, lws hasn't finished with it */
			إذا(آخرین){
				آخرین->for_free_بعدی = context;
			}وإلا{
				db.ll_for_free = context;
			}
			بعدی = context->for_free_بعدی;
			context->for_free_بعدی = عدم;
			آخرین = context;
			context = بعدی;
		}وإلا
#پایان_اگر
		{
			بعدی = context->for_free_بعدی;
			context__cleanup(context, true);
			context = بعدی;
		}
	}
}


فراغ context_جمع_کن_مقصد_by_id(بنية mosquitto *context)
{
	إذا(context->درون_by_id == false){
		context->درون_by_id = true;
		HASH_مقدار(context->id, طول_رشته(context->id), context->id_hashv);
		HASH_جمع_کن_KEYPTR_BYHASHVALUE(hh_id, db.contexts_by_id, context->id, طول_رشته(context->id), context->id_hashv, context);
	}
}


فراغ context__remove_from_by_id(بنية mosquitto *context)
{
	بنية mosquitto *context_یافت;

	إذا(!context->id){
		أرجع;
	}

	إذا(context->درون_by_id){
		HASH_پیدا_کن(hh_id, db.contexts_by_id, context->id, طول_رشته(context->id), context_یافت);
		إذا(context_یافت == context){
			HASH_DELETE(hh_id, db.contexts_by_id, context_یافت);
		}
		context->id_hashv = 0;
		context->درون_by_id = false;
		أرجع;
	}

	HASH_پیدا_کن(hh_id, db.contexts_by_id_delayed_auth, context->id, طول_رشته(context->id), context_یافت);
	إذا(context_یافت == context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context_یافت);
	}
}
