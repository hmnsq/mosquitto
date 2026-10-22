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
#جے_انت

#ضمن "mosquitto_broker_internal.h"
#ضمن "alias_mosq.h"
#ضمن "packet_mosq.h"
#ضمن "property_mosq.h"
#ضمن "sys_tree.h"
#ضمن "util_mosq.h"
#ضمن "will_mosq.h"

#ضمن "uthash.h"


صحيح context__init_sock(بنية mosquitto *context, mosq_sock_t sock, bool get_پتہ)
{
	context->sock = sock;

	إذا((صحيح)context->sock >= 0){
		إذا(get_پتہ){
			حرف پتہ[1024];

			إذا(!net_socket_get_پتہ(context->sock,
					پتہ, حجم(پتہ),
					&context->remote_پورٹ)){

				context->پتہ = mosquitto_strdup(پتہ);
			}
			إذا(!context->پتہ){
				/* getpeername and inet_ntop failed and not a bridge */
				أرجع MOSQ_ERR_NOMEM;
			}
		}
		HASH_جوڑو(hh_sock, db.contexts_by_sock, sock, حجم(context->sock), context);
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

	context->وچ_packet.packet_بفر_ناپ = db.config->packet_بفر_ناپ;
	context->وچ_packet.packet_بفر = mosquitto_calloc(1, context->وچ_packet.packet_بفر_ناپ);
	إذا(!context->وچ_packet.packet_بفر){
		mosquitto_FREE(context);
		أرجع عدم;
	}

#إذا defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	context->ident = id_client;
#وإلا
	context->pollfd_اشاریہ = -1;
#جے_انت
	mosquitto_سیٹ_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->آخری_پیغام_وچ = db.now_s;
	context->اگلا_پیغام_باہرلا = db.now_s + 20;
	context->keepalive = 20; /* Default to 20s */
	context->clean_شروع = true;
	context->id = عدم;
	context->آخری_mid = 0;
	context->will = عدم;
	context->username = عدم;
	context->password = عدم;
	context->listener = عدم;
	context->acl_list = عدم;
	context->retain_دستیاب = true;
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	memset(&context->wsd, 0, حجم(context->wsd));
	context->wsd.opcode = UINT8_MAX;
	context->wsd.نقاب = UINT8_MAX;
	context->wsd.disconnect_reason = 0xE8;
#جے_انت

	/* is_bridge records whether this client is a bridge or not. This could be
	 * done by looking at context->bridge for bridges that we create ourself,
	 * but incoming bridges need some other way of being recorded. */
	context->is_bridge = false;

	context->وچ_packet.payload = عدم;
	packet__cleanup(&context->وچ_packet);
	context->باہرلا_packet = عدم;
	context->باہرلا_packet_گنتی = 0;
	context->باہرلا_packet_بائٹاں = 0;

	context->پتہ = عدم;
	context->bridge = عدم;
	context->msgs_وچ.inflight_ودھ_توں_ودھ = db.config->max_inflight_messages;
	context->msgs_وچ.inflight_quota = db.config->max_inflight_messages;
	context->msgs_باہرلا.inflight_ودھ_توں_ودھ = db.config->max_inflight_messages;
	context->msgs_باہرلا.inflight_quota = db.config->max_inflight_messages;
	context->max_qos = 2;
#جے_تعریف_شدہ WITH_TLS
	context->ssl = عدم;
#جے_انت

	أرجع context;
}


ساكن فراغ context_cleanup_باہرلا_packets(بنية mosquitto *context)
{
	بنية mosquitto__packet *packet;

	إذا(!context){
		أرجع;
	}

	طالما(context->باہرلا_packet){
		packet = context->باہرلا_packet;
		context->باہرلا_packet = context->باہرلا_packet->اگلا;
		mosquitto_free(packet);
	}
	metrics__int_dec(mosq_gauge_باہرلا_packets, context->باہرلا_packet_گنتی);
	metrics__int_dec(mosq_gauge_باہرلا_packet_بائٹاں, context->باہرلا_packet_بائٹاں);
	context->باہرلا_packet_گنتی = 0;
	context->باہرلا_packet_بائٹاں = 0;
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

#جے_تعریف_شدہ WITH_BRIDGE
	إذا(context->bridge){
		bridge__cleanup(context);
	}
#جے_انت
	mosquitto_FREE(context->وچ_packet.packet_بفر);
	context->وچ_packet.packet_بفر_ناپ = 0;

	alias__free_all(context);
	keepalive__remove(context);
	context_cleanup_باہرلا_packets(context);

	mosquitto_FREE(context->auth_طریقہ);
	mosquitto_FREE(context->username);
	mosquitto_FREE(context->password);

	net__socket_close(context);
	إذا(force_free){
		sub__clean_session(context);
	}
	db__messages_delete(context, force_free);

	mosquitto_FREE(context->پتہ);

	context__send_will(context);

	إذا(context->id){
		context__remove_from_by_id(context);
		mosquitto_FREE(context->id);
	}
	packet__cleanup(&(context->وچ_packet));
	context_cleanup_باہرلا_packets(context);
#إذا defined(WITH_BROKER) && defined(__GLIBC__) && defined(WITH_ADNS)
	إذا(context->adns){
		gai_cancel(context->adns);
		بنية addrinfo *ar_request = (بنية addrinfo *)context->adns->ar_request;
		mosquitto_FREE(ar_request);
		mosquitto_FREE(context->adns);
	}
#جے_انت

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	mosquitto_FREE(context->http_request);
#جے_انت
	mosquitto_FREE(context->proxy.buf);
	إذا(force_free){
		mosquitto_FREE(context);
	}
}


فراغ context__send_will(بنية mosquitto *ctxt)
{
	إذا(ctxt->state != mosq_cs_disconnecting && ctxt->will){
		إذا(ctxt->session_expiry_interval > 0 && ctxt->will_delay_interval > 0){
			will_delay_جوڑو(ctxt);
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
#جے_انت
	إذا(context->id){
		بنية mosquitto *context_لبھیا;
		HASH_لبھو(hh_id, db.contexts_by_id_delayed_auth, context->id, لڑی_لمبائی(context->id), context_لبھیا);
		إذا(context_لبھیا == context){
			net__socket_close(context);
			context_جوڑو_منزل_disused(context);
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
#جے_تعریف_شدہ WITH_BRIDGE
	إذا(context->bridge == عدم)
	/* Outgoing bridge connection never expire */
#جے_انت
	{
		إذا(context->session_expiry_interval == MQTT_SESSION_EXPIRY_فوری){
			plugin_persist__handle_client_delete(context);
			/* Client session is due to be expired now */
			إذا(context->will_delay_interval == 0){
				/* This will be done later, after the will is published for delay>0. */
				context_جوڑو_منزل_disused(context);
			}
		}وإلا{
			session_expiry_جوڑو(context);
		}
	}
	keepalive__remove(context);
	mosquitto_سیٹ_state(context, mosq_cs_disconnected);
	alias__free_all(context);
	context_cleanup_باہرلا_packets(context);
}


فراغ context_جوڑو_منزل_disused(بنية mosquitto *context)
{
	إذا(context->state == mosq_cs_disused){
		أرجع;
	}

	mosquitto_سیٹ_state(context, mosq_cs_disused);

	context__remove_from_by_id(context);

	context->for_free_اگلا = db.ll_for_free;
	db.ll_for_free = context;
}


فراغ context__free_disused(فراغ)
{
	بنية mosquitto *context, *اگلا;
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	بنية mosquitto *آخری = عدم;
#جے_انت

	context = db.ll_for_free;
	db.ll_for_free = عدم;
	طالما(context){
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		إذا(context->wsi){
			/* Don't delete yet, lws hasn't finished with it */
			إذا(آخری){
				آخری->for_free_اگلا = context;
			}وإلا{
				db.ll_for_free = context;
			}
			اگلا = context->for_free_اگلا;
			context->for_free_اگلا = عدم;
			آخری = context;
			context = اگلا;
		}وإلا
#جے_انت
		{
			اگلا = context->for_free_اگلا;
			context__cleanup(context, true);
			context = اگلا;
		}
	}
}


فراغ context_جوڑو_منزل_by_id(بنية mosquitto *context)
{
	إذا(context->وچ_by_id == false){
		context->وچ_by_id = true;
		HASH_قدر(context->id, لڑی_لمبائی(context->id), context->id_hashv);
		HASH_جوڑو_KEYPTR_BYHASHVALUE(hh_id, db.contexts_by_id, context->id, لڑی_لمبائی(context->id), context->id_hashv, context);
	}
}


فراغ context__remove_from_by_id(بنية mosquitto *context)
{
	بنية mosquitto *context_لبھیا;

	إذا(!context->id){
		أرجع;
	}

	إذا(context->وچ_by_id){
		HASH_لبھو(hh_id, db.contexts_by_id, context->id, لڑی_لمبائی(context->id), context_لبھیا);
		إذا(context_لبھیا == context){
			HASH_DELETE(hh_id, db.contexts_by_id, context_لبھیا);
		}
		context->id_hashv = 0;
		context->وچ_by_id = false;
		أرجع;
	}

	HASH_لبھو(hh_id, db.contexts_by_id_delayed_auth, context->id, لڑی_لمبائی(context->id), context_لبھیا);
	إذا(context_لبھیا == context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context_لبھیا);
	}
}
