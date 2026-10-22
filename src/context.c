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
#جيڪڏهن_پورو

#ضمن "mosquitto_broker_internal.h"
#ضمن "alias_mosq.h"
#ضمن "packet_mosq.h"
#ضمن "property_mosq.h"
#ضمن "sys_tree.h"
#ضمن "util_mosq.h"
#ضمن "will_mosq.h"

#ضمن "uthash.h"


صحيح context__init_sock(بنية mosquitto *context, mosq_sock_t sock, bool get_پتو)
{
	context->sock = sock;

	إذا((صحيح)context->sock >= 0){
		إذا(get_پتو){
			حرف پتو[1024];

			إذا(!net_socket_get_پتو(context->sock,
					پتو, حجم(پتو),
					&context->remote_پورٽ)){

				context->پتو = mosquitto_strdup(پتو);
			}
			إذا(!context->پتو){
				/* getpeername and inet_ntop failed and not a bridge */
				أرجع MOSQ_ERR_NOMEM;
			}
		}
		HASH_جوڙيو(hh_sock, db.contexts_by_sock, sock, حجم(context->sock), context);
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

	context->اندريون_packet.packet_بفر_سائيز = db.config->packet_بفر_سائيز;
	context->اندريون_packet.packet_بفر = mosquitto_calloc(1, context->اندريون_packet.packet_بفر_سائيز);
	إذا(!context->اندريون_packet.packet_بفر){
		mosquitto_FREE(context);
		أرجع عدم;
	}

#إذا defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	context->ident = id_client;
#وإلا
	context->pollfd_اشاريو = -1;
#جيڪڏهن_پورو
	mosquitto_سيٽ_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->آخري_نياپو_اندريون = db.now_s;
	context->ايندڙ_نياپو_ٻاهريون = db.now_s + 20;
	context->keepalive = 20; /* Default to 20s */
	context->clean_شروعات = true;
	context->id = عدم;
	context->آخري_mid = 0;
	context->will = عدم;
	context->username = عدم;
	context->password = عدم;
	context->listener = عدم;
	context->acl_list = عدم;
	context->retain_موجود = true;
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	memset(&context->wsd, 0, حجم(context->wsd));
	context->wsd.opcode = UINT8_MAX;
	context->wsd.نقاب = UINT8_MAX;
	context->wsd.disconnect_reason = 0xE8;
#جيڪڏهن_پورو

	/* is_bridge records whether this client is a bridge or not. This could be
	 * done by looking at context->bridge for bridges that we create ourself,
	 * but incoming bridges need some other way of being recorded. */
	context->is_bridge = false;

	context->اندريون_packet.payload = عدم;
	packet__cleanup(&context->اندريون_packet);
	context->ٻاهريون_packet = عدم;
	context->ٻاهريون_packet_ڳڻپ = 0;
	context->ٻاهريون_packet_بائيٽون = 0;

	context->پتو = عدم;
	context->bridge = عدم;
	context->msgs_اندريون.inflight_وڌ_کان_وڌ = db.config->max_inflight_messages;
	context->msgs_اندريون.inflight_quota = db.config->max_inflight_messages;
	context->msgs_ٻاهريون.inflight_وڌ_کان_وڌ = db.config->max_inflight_messages;
	context->msgs_ٻاهريون.inflight_quota = db.config->max_inflight_messages;
	context->max_qos = 2;
#جيڪڏهن_وصف_ٿيل WITH_TLS
	context->ssl = عدم;
#جيڪڏهن_پورو

	أرجع context;
}


ساكن فراغ context_cleanup_ٻاهريون_packets(بنية mosquitto *context)
{
	بنية mosquitto__packet *packet;

	إذا(!context){
		أرجع;
	}

	طالما(context->ٻاهريون_packet){
		packet = context->ٻاهريون_packet;
		context->ٻاهريون_packet = context->ٻاهريون_packet->ايندڙ;
		mosquitto_free(packet);
	}
	metrics__int_dec(mosq_gauge_ٻاهريون_packets, context->ٻاهريون_packet_ڳڻپ);
	metrics__int_dec(mosq_gauge_ٻاهريون_packet_بائيٽون, context->ٻاهريون_packet_بائيٽون);
	context->ٻاهريون_packet_ڳڻپ = 0;
	context->ٻاهريون_packet_بائيٽون = 0;
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
		context->clean_شروعات = true;
	}

#جيڪڏهن_وصف_ٿيل WITH_BRIDGE
	إذا(context->bridge){
		bridge__cleanup(context);
	}
#جيڪڏهن_پورو
	mosquitto_FREE(context->اندريون_packet.packet_بفر);
	context->اندريون_packet.packet_بفر_سائيز = 0;

	alias__free_all(context);
	keepalive__remove(context);
	context_cleanup_ٻاهريون_packets(context);

	mosquitto_FREE(context->auth_طريقو);
	mosquitto_FREE(context->username);
	mosquitto_FREE(context->password);

	net__socket_close(context);
	إذا(force_free){
		sub__clean_session(context);
	}
	db__messages_delete(context, force_free);

	mosquitto_FREE(context->پتو);

	context__send_will(context);

	إذا(context->id){
		context__remove_from_by_id(context);
		mosquitto_FREE(context->id);
	}
	packet__cleanup(&(context->اندريون_packet));
	context_cleanup_ٻاهريون_packets(context);
#إذا defined(WITH_BROKER) && defined(__GLIBC__) && defined(WITH_ADNS)
	إذا(context->adns){
		gai_cancel(context->adns);
		بنية addrinfo *ar_request = (بنية addrinfo *)context->adns->ar_request;
		mosquitto_FREE(ar_request);
		mosquitto_FREE(context->adns);
	}
#جيڪڏهن_پورو

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	mosquitto_FREE(context->http_request);
#جيڪڏهن_پورو
	mosquitto_FREE(context->proxy.buf);
	إذا(force_free){
		mosquitto_FREE(context);
	}
}


فراغ context__send_will(بنية mosquitto *ctxt)
{
	إذا(ctxt->state != mosq_cs_disconnecting && ctxt->will){
		إذا(ctxt->session_expiry_interval > 0 && ctxt->will_delay_interval > 0){
			will_delay_جوڙيو(ctxt);
			أرجع;
		}

		إذا(mosquitto_acl_check(ctxt,
				ctxt->will->نياپو.topic,
				(uint32_t)ctxt->will->نياپو.payloadlen,
				ctxt->will->نياپو.payload,
				(uint8_t)ctxt->will->نياپو.qos,
				ctxt->will->نياپو.retain,
				ctxt->will->properties,
				MOSQ_ACL_WRITE) == MOSQ_ERR_SUCCESS){

			/* Unexpected disconnect, queue the client will. */
			db_messages_easy_قطار(ctxt,
					ctxt->will->نياپو.topic,
					(uint8_t)ctxt->will->نياپو.qos,
					(uint32_t)ctxt->will->نياپو.payloadlen,
					ctxt->will->نياپو.payload,
					ctxt->will->نياپو.retain,
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
#جيڪڏهن_پورو
	إذا(context->id){
		بنية mosquitto *context_مليو;
		HASH_ڳوليو(hh_id, db.contexts_by_id_delayed_auth, context->id, لڙي_ڊيگهه(context->id), context_مليو);
		إذا(context_مليو == context){
			net__socket_close(context);
			context_جوڙيو_منزل_disused(context);
			أرجع;
		}
	}

	context__send_will(context);

	إذا(context->session_expiry_interval == MQTT_SESSION_EXPIRY_فوري){
		plugin__handle_disconnect(context, reason);
	}وإلا{
		plugin__handle_client_offline(context, reason);
	}

	net__socket_close(context);
#جيڪڏهن_وصف_ٿيل WITH_BRIDGE
	إذا(context->bridge == عدم)
	/* Outgoing bridge connection never expire */
#جيڪڏهن_پورو
	{
		إذا(context->session_expiry_interval == MQTT_SESSION_EXPIRY_فوري){
			plugin_persist__handle_client_delete(context);
			/* Client session is due to be expired now */
			إذا(context->will_delay_interval == 0){
				/* This will be done later, after the will is published for delay>0. */
				context_جوڙيو_منزل_disused(context);
			}
		}وإلا{
			session_expiry_جوڙيو(context);
		}
	}
	keepalive__remove(context);
	mosquitto_سيٽ_state(context, mosq_cs_disconnected);
	alias__free_all(context);
	context_cleanup_ٻاهريون_packets(context);
}


فراغ context_جوڙيو_منزل_disused(بنية mosquitto *context)
{
	إذا(context->state == mosq_cs_disused){
		أرجع;
	}

	mosquitto_سيٽ_state(context, mosq_cs_disused);

	context__remove_from_by_id(context);

	context->for_free_ايندڙ = db.ll_for_free;
	db.ll_for_free = context;
}


فراغ context__free_disused(فراغ)
{
	بنية mosquitto *context, *ايندڙ;
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	بنية mosquitto *آخري = عدم;
#جيڪڏهن_پورو

	context = db.ll_for_free;
	db.ll_for_free = عدم;
	طالما(context){
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		إذا(context->wsi){
			/* Don't delete yet, lws hasn't finished with it */
			إذا(آخري){
				آخري->for_free_ايندڙ = context;
			}وإلا{
				db.ll_for_free = context;
			}
			ايندڙ = context->for_free_ايندڙ;
			context->for_free_ايندڙ = عدم;
			آخري = context;
			context = ايندڙ;
		}وإلا
#جيڪڏهن_پورو
		{
			ايندڙ = context->for_free_ايندڙ;
			context__cleanup(context, true);
			context = ايندڙ;
		}
	}
}


فراغ context_جوڙيو_منزل_by_id(بنية mosquitto *context)
{
	إذا(context->اندريون_by_id == false){
		context->اندريون_by_id = true;
		HASH_قدر(context->id, لڙي_ڊيگهه(context->id), context->id_hashv);
		HASH_جوڙيو_KEYPTR_BYHASHVALUE(hh_id, db.contexts_by_id, context->id, لڙي_ڊيگهه(context->id), context->id_hashv, context);
	}
}


فراغ context__remove_from_by_id(بنية mosquitto *context)
{
	بنية mosquitto *context_مليو;

	إذا(!context->id){
		أرجع;
	}

	إذا(context->اندريون_by_id){
		HASH_ڳوليو(hh_id, db.contexts_by_id, context->id, لڙي_ڊيگهه(context->id), context_مليو);
		إذا(context_مليو == context){
			HASH_DELETE(hh_id, db.contexts_by_id, context_مليو);
		}
		context->id_hashv = 0;
		context->اندريون_by_id = false;
		أرجع;
	}

	HASH_ڳوليو(hh_id, db.contexts_by_id_delayed_auth, context->id, لڙي_ڊيگهه(context->id), context_مليو);
	إذا(context_مليو == context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context_مليو);
	}
}
