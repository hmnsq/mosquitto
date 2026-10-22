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

#అన్తర్భావ "config.h"

#అన్తర్భావ <time.h>
#ఒకవేళ defined(__APPLE__) || defined(_AIX)
#అన్తర్భావ <sys/socket.h>
#అయితే_ముగింపు

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "alias_mosq.h"
#అన్తర్భావ "packet_mosq.h"
#అన్తర్భావ "property_mosq.h"
#అన్తర్భావ "sys_tree.h"
#అన్తర్భావ "util_mosq.h"
#అన్తర్భావ "will_mosq.h"

#అన్తర్భావ "uthash.h"


పూర్ణాంకం context__init_sock(నిర్మాణం mosquitto *context, mosq_sock_t sock, bool get_చిరునామా)
{
	context->sock = sock;

	ఒకవేళ((పూర్ణాంకం)context->sock >= 0){
		ఒకవేళ(get_చిరునామా){
			అక్షరం చిరునామా[1024];

			ఒకవేళ(!net_socket_get_చిరునామా(context->sock,
					చిరునామా, పరిమాణం(చిరునామా),
					&context->remote_పోర్టు)){

				context->చిరునామా = mosquitto_strdup(చిరునామా);
			}
			ఒకవేళ(!context->చిరునామా){
				/* getpeername and inet_ntop failed and not a bridge */
				తిరిగి MOSQ_ERR_NOMEM;
			}
		}
		HASH_కూడు(hh_sock, db.contexts_by_sock, sock, పరిమాణం(context->sock), context);
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}

నిర్మాణం mosquitto *context__init(శూన్య)
{
	నిర్మాణం mosquitto *context;

	context = mosquitto_calloc(1, పరిమాణం(నిర్మాణం mosquitto));
	ఒకవేళ(!context){
		తిరిగి ఖాళీ;
	}

	context->లోపల_packet.packet_బఫరు_కొలత = db.config->packet_బఫరు_కొలత;
	context->లోపల_packet.packet_బఫరు = mosquitto_calloc(1, context->లోపల_packet.packet_బఫరు_కొలత);
	ఒకవేళ(!context->లోపల_packet.packet_బఫరు){
		mosquitto_FREE(context);
		తిరిగి ఖాళీ;
	}

#ఒకవేళ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	context->ident = id_client;
#లేకపోతే
	context->pollfd_సూచీ = -1;
#అయితే_ముగింపు
	mosquitto_సమితి_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->చివరి_సందేశం_లోపల = db.now_s;
	context->తదుపరి_సందేశం_బహిర్గతం = db.now_s + 20;
	context->keepalive = 20; /* Default to 20s */
	context->clean_ప్రారంభం = true;
	context->id = ఖాళీ;
	context->చివరి_mid = 0;
	context->will = ఖాళీ;
	context->username = ఖాళీ;
	context->password = ఖాళీ;
	context->listener = ఖాళీ;
	context->acl_list = ఖాళీ;
	context->retain_అందుబాటు = true;
#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	memset(&context->wsd, 0, పరిమాణం(context->wsd));
	context->wsd.opcode = UINT8_MAX;
	context->wsd.ముసుగు = UINT8_MAX;
	context->wsd.disconnect_reason = 0xE8;
#అయితే_ముగింపు

	/* is_bridge records whether this client is a bridge or not. This could be
	 * done by looking at context->bridge for bridges that we create ourself,
	 * but incoming bridges need some other way of being recorded. */
	context->is_bridge = false;

	context->లోపల_packet.payload = ఖాళీ;
	packet__cleanup(&context->లోపల_packet);
	context->బహిర్గతం_packet = ఖాళీ;
	context->బహిర్గతం_packet_లెక్క = 0;
	context->బహిర్గతం_packet_బైట్లు = 0;

	context->చిరునామా = ఖాళీ;
	context->bridge = ఖాళీ;
	context->msgs_లోపల.inflight_గరిష్ఠ = db.config->max_inflight_messages;
	context->msgs_లోపల.inflight_quota = db.config->max_inflight_messages;
	context->msgs_బహిర్గతం.inflight_గరిష్ఠ = db.config->max_inflight_messages;
	context->msgs_బహిర్గతం.inflight_quota = db.config->max_inflight_messages;
	context->max_qos = 2;
#నిర్వచితమైతే WITH_TLS
	context->ssl = ఖాళీ;
#అయితే_ముగింపు

	తిరిగి context;
}


స్థిర శూన్య context_cleanup_బహిర్గతం_packets(నిర్మాణం mosquitto *context)
{
	నిర్మాణం mosquitto__packet *packet;

	ఒకవేళ(!context){
		తిరిగి;
	}

	అయితే(context->బహిర్గతం_packet){
		packet = context->బహిర్గతం_packet;
		context->బహిర్గతం_packet = context->బహిర్గతం_packet->తదుపరి;
		mosquitto_free(packet);
	}
	metrics__int_dec(mosq_gauge_బహిర్గతం_packets, context->బహిర్గతం_packet_లెక్క);
	metrics__int_dec(mosq_gauge_బహిర్గతం_packet_బైట్లు, context->బహిర్గతం_packet_బైట్లు);
	context->బహిర్గతం_packet_లెక్క = 0;
	context->బహిర్గతం_packet_బైట్లు = 0;
}


/*
 * This will result in any outgoing packets going unsent. If we're disconnected
 * forcefully then it is usually an error condition and shouldn't be a problem,
 * but it will mean that CONNACK messages will never get sent for bad protocol
 * versions for example.
 */
శూన్య context__cleanup(నిర్మాణం mosquitto *context, bool force_free)
{
	ఒకవేళ(!context){
		తిరిగి;
	}

	ఒకవేళ(force_free){
		context->clean_ప్రారంభం = true;
	}

#నిర్వచితమైతే WITH_BRIDGE
	ఒకవేళ(context->bridge){
		bridge__cleanup(context);
	}
#అయితే_ముగింపు
	mosquitto_FREE(context->లోపల_packet.packet_బఫరు);
	context->లోపల_packet.packet_బఫరు_కొలత = 0;

	alias__free_all(context);
	keepalive__remove(context);
	context_cleanup_బహిర్గతం_packets(context);

	mosquitto_FREE(context->auth_పద్ధతి);
	mosquitto_FREE(context->username);
	mosquitto_FREE(context->password);

	net__socket_close(context);
	ఒకవేళ(force_free){
		sub__clean_session(context);
	}
	db__messages_delete(context, force_free);

	mosquitto_FREE(context->చిరునామా);

	context__send_will(context);

	ఒకవేళ(context->id){
		context__remove_from_by_id(context);
		mosquitto_FREE(context->id);
	}
	packet__cleanup(&(context->లోపల_packet));
	context_cleanup_బహిర్గతం_packets(context);
#ఒకవేళ defined(WITH_BROKER) && defined(__GLIBC__) && defined(WITH_ADNS)
	ఒకవేళ(context->adns){
		gai_cancel(context->adns);
		నిర్మాణం addrinfo *ar_request = (నిర్మాణం addrinfo *)context->adns->ar_request;
		mosquitto_FREE(ar_request);
		mosquitto_FREE(context->adns);
	}
#అయితే_ముగింపు

#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	mosquitto_FREE(context->http_request);
#అయితే_ముగింపు
	mosquitto_FREE(context->proxy.buf);
	ఒకవేళ(force_free){
		mosquitto_FREE(context);
	}
}


శూన్య context__send_will(నిర్మాణం mosquitto *ctxt)
{
	ఒకవేళ(ctxt->state != mosq_cs_disconnecting && ctxt->will){
		ఒకవేళ(ctxt->session_expiry_interval > 0 && ctxt->will_delay_interval > 0){
			will_delay_కూడు(ctxt);
			తిరిగి;
		}

		ఒకవేళ(mosquitto_acl_check(ctxt,
				ctxt->will->సందేశం.topic,
				(uint32_t)ctxt->will->సందేశం.payloadlen,
				ctxt->will->సందేశం.payload,
				(uint8_t)ctxt->will->సందేశం.qos,
				ctxt->will->సందేశం.retain,
				ctxt->will->properties,
				MOSQ_ACL_WRITE) == MOSQ_ERR_SUCCESS){

			/* Unexpected disconnect, queue the client will. */
			db_messages_easy_వరుస(ctxt,
					ctxt->will->సందేశం.topic,
					(uint8_t)ctxt->will->సందేశం.qos,
					(uint32_t)ctxt->will->సందేశం.payloadlen,
					ctxt->will->సందేశం.payload,
					ctxt->will->సందేశం.retain,
					ctxt->will->expiry_interval,
					&ctxt->will->properties);
		}
	}
	will__clear(ctxt);
}


శూన్య context__disconnect(నిర్మాణం mosquitto *context, పూర్ణాంకం reason)
{
	ఒకవేళ(mosquitto__get_state(context) == mosq_cs_disconnected){
		తిరిగి;
	}

#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	ఒకవేళ(context->transport == mosq_t_ws){
		uint8_t buf[4] = {0x88, 0x02, 0x03, context->wsd.disconnect_reason};
		/* Send the disconnect reason, but don't care if it fails */
		ఒకవేళ(send(context->sock, buf, 4, 0)){
		}
	}
#అయితే_ముగింపు
	ఒకవేళ(context->id){
		నిర్మాణం mosquitto *context_దొరికింది;
		HASH_కనుగొను(hh_id, db.contexts_by_id_delayed_auth, context->id, అక్షరశ్రేణి_పొడవు(context->id), context_దొరికింది);
		ఒకవేళ(context_దొరికింది == context){
			net__socket_close(context);
			context_కూడు_గమ్యం_disused(context);
			తిరిగి;
		}
	}

	context__send_will(context);

	ఒకవేళ(context->session_expiry_interval == MQTT_SESSION_EXPIRY_తక్షణ){
		plugin__handle_disconnect(context, reason);
	}లేకపోతే{
		plugin__handle_client_offline(context, reason);
	}

	net__socket_close(context);
#నిర్వచితమైతే WITH_BRIDGE
	ఒకవేళ(context->bridge == ఖాళీ)
	/* Outgoing bridge connection never expire */
#అయితే_ముగింపు
	{
		ఒకవేళ(context->session_expiry_interval == MQTT_SESSION_EXPIRY_తక్షణ){
			plugin_persist__handle_client_delete(context);
			/* Client session is due to be expired now */
			ఒకవేళ(context->will_delay_interval == 0){
				/* This will be done later, after the will is published for delay>0. */
				context_కూడు_గమ్యం_disused(context);
			}
		}లేకపోతే{
			session_expiry_కూడు(context);
		}
	}
	keepalive__remove(context);
	mosquitto_సమితి_state(context, mosq_cs_disconnected);
	alias__free_all(context);
	context_cleanup_బహిర్గతం_packets(context);
}


శూన్య context_కూడు_గమ్యం_disused(నిర్మాణం mosquitto *context)
{
	ఒకవేళ(context->state == mosq_cs_disused){
		తిరిగి;
	}

	mosquitto_సమితి_state(context, mosq_cs_disused);

	context__remove_from_by_id(context);

	context->for_free_తదుపరి = db.ll_for_free;
	db.ll_for_free = context;
}


శూన్య context__free_disused(శూన్య)
{
	నిర్మాణం mosquitto *context, *తదుపరి;
#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	నిర్మాణం mosquitto *చివరి = ఖాళీ;
#అయితే_ముగింపు

	context = db.ll_for_free;
	db.ll_for_free = ఖాళీ;
	అయితే(context){
#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ఒకవేళ(context->wsi){
			/* Don't delete yet, lws hasn't finished with it */
			ఒకవేళ(చివరి){
				చివరి->for_free_తదుపరి = context;
			}లేకపోతే{
				db.ll_for_free = context;
			}
			తదుపరి = context->for_free_తదుపరి;
			context->for_free_తదుపరి = ఖాళీ;
			చివరి = context;
			context = తదుపరి;
		}లేకపోతే
#అయితే_ముగింపు
		{
			తదుపరి = context->for_free_తదుపరి;
			context__cleanup(context, true);
			context = తదుపరి;
		}
	}
}


శూన్య context_కూడు_గమ్యం_by_id(నిర్మాణం mosquitto *context)
{
	ఒకవేళ(context->లోపల_by_id == false){
		context->లోపల_by_id = true;
		HASH_విలువ(context->id, అక్షరశ్రేణి_పొడవు(context->id), context->id_hashv);
		HASH_కూడు_KEYPTR_BYHASHVALUE(hh_id, db.contexts_by_id, context->id, అక్షరశ్రేణి_పొడవు(context->id), context->id_hashv, context);
	}
}


శూన్య context__remove_from_by_id(నిర్మాణం mosquitto *context)
{
	నిర్మాణం mosquitto *context_దొరికింది;

	ఒకవేళ(!context->id){
		తిరిగి;
	}

	ఒకవేళ(context->లోపల_by_id){
		HASH_కనుగొను(hh_id, db.contexts_by_id, context->id, అక్షరశ్రేణి_పొడవు(context->id), context_దొరికింది);
		ఒకవేళ(context_దొరికింది == context){
			HASH_DELETE(hh_id, db.contexts_by_id, context_దొరికింది);
		}
		context->id_hashv = 0;
		context->లోపల_by_id = false;
		తిరిగి;
	}

	HASH_కనుగొను(hh_id, db.contexts_by_id_delayed_auth, context->id, అక్షరశ్రేణి_పొడవు(context->id), context_దొరికింది);
	ఒకవేళ(context_దొరికింది == context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context_దొరికింది);
	}
}
