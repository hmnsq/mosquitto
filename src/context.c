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
#د_که_پای

#ضمن "mosquitto_broker_internal.h"
#ضمن "alias_mosq.h"
#ضمن "packet_mosq.h"
#ضمن "property_mosq.h"
#ضمن "sys_tree.h"
#ضمن "util_mosq.h"
#ضمن "will_mosq.h"

#ضمن "uthash.h"


صحيح context__init_sock(بنية mosquitto *context, mosq_sock_t sock, bool get_پته)
{
	context->sock = sock;

	إذا((صحيح)context->sock >= 0){
		إذا(get_پته){
			حرف پته[1024];

			إذا(!net_socket_get_پته(context->sock,
					پته, حجم(پته),
					&context->remote_پورټ)){

				context->پته = mosquitto_strdup(پته);
			}
			إذا(!context->پته){
				/* getpeername and inet_ntop failed and not a bridge */
				أرجع MOSQ_ERR_NOMEM;
			}
		}
		HASH_جمع_کړه(hh_sock, db.contexts_by_sock, sock, حجم(context->sock), context);
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

	context->دننه_packet.packet_بفر_اندازه = db.config->packet_بفر_اندازه;
	context->دننه_packet.packet_بفر = mosquitto_calloc(1, context->دننه_packet.packet_بفر_اندازه);
	إذا(!context->دننه_packet.packet_بفر){
		mosquitto_FREE(context);
		أرجع عدم;
	}

#إذا defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	context->ident = id_client;
#وإلا
	context->pollfd_شاخص = -1;
#د_که_پای
	mosquitto_سېټ_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->وروستی_پیغام_دننه = db.now_s;
	context->بل_پیغام_بهر = db.now_s + 20;
	context->keepalive = 20; /* Default to 20s */
	context->clean_پیل = true;
	context->id = عدم;
	context->وروستی_mid = 0;
	context->will = عدم;
	context->username = عدم;
	context->password = عدم;
	context->listener = عدم;
	context->acl_list = عدم;
	context->retain_شته = true;
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	memset(&context->wsd, 0, حجم(context->wsd));
	context->wsd.opcode = UINT8_MAX;
	context->wsd.ماسک = UINT8_MAX;
	context->wsd.disconnect_reason = 0xE8;
#د_که_پای

	/* is_bridge records whether this client is a bridge or not. This could be
	 * done by looking at context->bridge for bridges that we create ourself,
	 * but incoming bridges need some other way of being recorded. */
	context->is_bridge = false;

	context->دننه_packet.payload = عدم;
	packet__cleanup(&context->دننه_packet);
	context->بهر_packet = عدم;
	context->بهر_packet_شمېر = 0;
	context->بهر_packet_بایټونه = 0;

	context->پته = عدم;
	context->bridge = عدم;
	context->msgs_دننه.inflight_اعظمي = db.config->max_inflight_messages;
	context->msgs_دننه.inflight_quota = db.config->max_inflight_messages;
	context->msgs_بهر.inflight_اعظمي = db.config->max_inflight_messages;
	context->msgs_بهر.inflight_quota = db.config->max_inflight_messages;
	context->max_qos = 2;
#که_تعریف_شوی WITH_TLS
	context->ssl = عدم;
#د_که_پای

	أرجع context;
}


ساكن فراغ context_cleanup_بهر_packets(بنية mosquitto *context)
{
	بنية mosquitto__packet *packet;

	إذا(!context){
		أرجع;
	}

	طالما(context->بهر_packet){
		packet = context->بهر_packet;
		context->بهر_packet = context->بهر_packet->بل;
		mosquitto_free(packet);
	}
	metrics__int_dec(mosq_gauge_بهر_packets, context->بهر_packet_شمېر);
	metrics__int_dec(mosq_gauge_بهر_packet_بایټونه, context->بهر_packet_بایټونه);
	context->بهر_packet_شمېر = 0;
	context->بهر_packet_بایټونه = 0;
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
		context->clean_پیل = true;
	}

#که_تعریف_شوی WITH_BRIDGE
	إذا(context->bridge){
		bridge__cleanup(context);
	}
#د_که_پای
	mosquitto_FREE(context->دننه_packet.packet_بفر);
	context->دننه_packet.packet_بفر_اندازه = 0;

	alias__free_all(context);
	keepalive__remove(context);
	context_cleanup_بهر_packets(context);

	mosquitto_FREE(context->auth_لاره);
	mosquitto_FREE(context->username);
	mosquitto_FREE(context->password);

	net__socket_close(context);
	إذا(force_free){
		sub__clean_session(context);
	}
	db__messages_delete(context, force_free);

	mosquitto_FREE(context->پته);

	context__send_will(context);

	إذا(context->id){
		context__remove_from_by_id(context);
		mosquitto_FREE(context->id);
	}
	packet__cleanup(&(context->دننه_packet));
	context_cleanup_بهر_packets(context);
#إذا defined(WITH_BROKER) && defined(__GLIBC__) && defined(WITH_ADNS)
	إذا(context->adns){
		gai_cancel(context->adns);
		بنية addrinfo *ar_request = (بنية addrinfo *)context->adns->ar_request;
		mosquitto_FREE(ar_request);
		mosquitto_FREE(context->adns);
	}
#د_که_پای

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	mosquitto_FREE(context->http_request);
#د_که_پای
	mosquitto_FREE(context->proxy.buf);
	إذا(force_free){
		mosquitto_FREE(context);
	}
}


فراغ context__send_will(بنية mosquitto *ctxt)
{
	إذا(ctxt->state != mosq_cs_disconnecting && ctxt->will){
		إذا(ctxt->session_expiry_interval > 0 && ctxt->will_delay_interval > 0){
			will_delay_جمع_کړه(ctxt);
			أرجع;
		}

		إذا(mosquitto_acl_check(ctxt,
				ctxt->will->پیغام.topic,
				(uint32_t)ctxt->will->پیغام.payloadlen,
				ctxt->will->پیغام.payload,
				(uint8_t)ctxt->will->پیغام.qos,
				ctxt->will->پیغام.retain,
				ctxt->will->properties,
				MOSQ_ACL_WRITE) == MOSQ_ERR_SUCCESS){

			/* Unexpected disconnect, queue the client will. */
			db_messages_easy_کتار(ctxt,
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
#د_که_پای
	إذا(context->id){
		بنية mosquitto *context_وموندل;
		HASH_پیدا_کړه(hh_id, db.contexts_by_id_delayed_auth, context->id, مزي_اوږدوالی(context->id), context_وموندل);
		إذا(context_وموندل == context){
			net__socket_close(context);
			context_جمع_کړه_منزل_disused(context);
			أرجع;
		}
	}

	context__send_will(context);

	إذا(context->session_expiry_interval == MQTT_SESSION_EXPIRY_سمدستي){
		plugin__handle_disconnect(context, reason);
	}وإلا{
		plugin__handle_client_offline(context, reason);
	}

	net__socket_close(context);
#که_تعریف_شوی WITH_BRIDGE
	إذا(context->bridge == عدم)
	/* Outgoing bridge connection never expire */
#د_که_پای
	{
		إذا(context->session_expiry_interval == MQTT_SESSION_EXPIRY_سمدستي){
			plugin_persist__handle_client_delete(context);
			/* Client session is due to be expired now */
			إذا(context->will_delay_interval == 0){
				/* This will be done later, after the will is published for delay>0. */
				context_جمع_کړه_منزل_disused(context);
			}
		}وإلا{
			session_expiry_جمع_کړه(context);
		}
	}
	keepalive__remove(context);
	mosquitto_سېټ_state(context, mosq_cs_disconnected);
	alias__free_all(context);
	context_cleanup_بهر_packets(context);
}


فراغ context_جمع_کړه_منزل_disused(بنية mosquitto *context)
{
	إذا(context->state == mosq_cs_disused){
		أرجع;
	}

	mosquitto_سېټ_state(context, mosq_cs_disused);

	context__remove_from_by_id(context);

	context->for_free_بل = db.ll_for_free;
	db.ll_for_free = context;
}


فراغ context__free_disused(فراغ)
{
	بنية mosquitto *context, *بل;
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	بنية mosquitto *وروستی = عدم;
#د_که_پای

	context = db.ll_for_free;
	db.ll_for_free = عدم;
	طالما(context){
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		إذا(context->wsi){
			/* Don't delete yet, lws hasn't finished with it */
			إذا(وروستی){
				وروستی->for_free_بل = context;
			}وإلا{
				db.ll_for_free = context;
			}
			بل = context->for_free_بل;
			context->for_free_بل = عدم;
			وروستی = context;
			context = بل;
		}وإلا
#د_که_پای
		{
			بل = context->for_free_بل;
			context__cleanup(context, true);
			context = بل;
		}
	}
}


فراغ context_جمع_کړه_منزل_by_id(بنية mosquitto *context)
{
	إذا(context->دننه_by_id == false){
		context->دننه_by_id = true;
		HASH_ارزښت(context->id, مزي_اوږدوالی(context->id), context->id_hashv);
		HASH_جمع_کړه_KEYPTR_BYHASHVALUE(hh_id, db.contexts_by_id, context->id, مزي_اوږدوالی(context->id), context->id_hashv, context);
	}
}


فراغ context__remove_from_by_id(بنية mosquitto *context)
{
	بنية mosquitto *context_وموندل;

	إذا(!context->id){
		أرجع;
	}

	إذا(context->دننه_by_id){
		HASH_پیدا_کړه(hh_id, db.contexts_by_id, context->id, مزي_اوږدوالی(context->id), context_وموندل);
		إذا(context_وموندل == context){
			HASH_DELETE(hh_id, db.contexts_by_id, context_وموندل);
		}
		context->id_hashv = 0;
		context->دننه_by_id = false;
		أرجع;
	}

	HASH_پیدا_کړه(hh_id, db.contexts_by_id_delayed_auth, context->id, مزي_اوږدوالی(context->id), context_وموندل);
	إذا(context_وموندل == context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context_وموندل);
	}
}
