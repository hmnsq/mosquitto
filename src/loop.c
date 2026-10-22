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
   Tatsuzo Osawa - Add epoll.
*/

#అన్తర్భావ "config.h"

#నిర్వచితం_కాకపోతే WIN32
#  define GNU_ఆధారం
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే WIN32
#అన్తర్భావ <unistd.h>
#లేకపోతే
#అన్తర్భావ <process.h>
#అన్తర్భావ <winsock2.h>
#అన్తర్భావ <ws2tcpip.h>
#అయితే_ముగింపు

#అన్తర్భావ <errno.h>
#అన్తర్భావ <signal.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>
#నిర్వచితం_కాకపోతే WIN32
#  include <sys/socket.h>
#అయితే_ముగింపు
#అన్తర్భావ <time.h>
#అన్తర్భావ <utlist.h>

#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#అయితే_ముగింపు

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto/mqtt_protocol.h"
#అన్తర్భావ "packet_mosq.h"
#అన్తర్భావ "property_common.h"
#అన్తర్భావ "send_mosq.h"
#అన్తర్భావ "sys_tree.h"
#అన్తర్భావ "util_mosq.h"

బాహ్య పూర్ణాంకం g_నిరంతరం;

#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_సంస్కరణ_సంఖ్య == 3002000


శూన్య lws__sul_callback(నిర్మాణం lws_sorted_usec_list *l)
{
}

స్థిర నిర్మాణం lws_sorted_usec_list sul;
#అయితే_ముగింపు


స్థిర పూర్ణాంకం single_publish(నిర్మాణం mosquitto *context, నిర్మాణం mosquitto_సందేశం_v5 *pub_సందేశం, uint32_t సందేశం_expiry)
{
	నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం;
	uint16_t mid;

	పునాది_సందేశం = mosquitto_calloc(1, పరిమాణం(నిర్మాణం mosquitto_పునాది_సందేశం));
	ఒకవేళ(పునాది_సందేశం == ఖాళీ){
		తిరిగి MOSQ_ERR_NOMEM;
	}

	పునాది_సందేశం->దత్తాంశం.topic = pub_సందేశం->topic;
	pub_సందేశం->topic = ఖాళీ;
	పునాది_సందేశం->దత్తాంశం.retain = 0;
	పునాది_సందేశం->దత్తాంశం.payloadlen = (uint32_t)pub_సందేశం->payloadlen;
	పునాది_సందేశం->దత్తాంశం.payload = mosquitto_malloc(పునాది_సందేశం->దత్తాంశం.payloadlen+1);
	ఒకవేళ(పునాది_సందేశం->దత్తాంశం.payload == ఖాళీ){
		db_సందేశం_store_free(పునాది_సందేశం);
		తిరిగి MOSQ_ERR_NOMEM;
	}
	/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
	((uint8_t *)పునాది_సందేశం->దత్తాంశం.payload)[పునాది_సందేశం->దత్తాంశం.payloadlen] = 0;
	జ్ఞాపకం_నకలు(పునాది_సందేశం->దత్తాంశం.payload, pub_సందేశం->payload, పునాది_సందేశం->దత్తాంశం.payloadlen);

	ఒకవేళ(pub_సందేశం->properties){
		పునాది_సందేశం->దత్తాంశం.properties = pub_సందేశం->properties;
		pub_సందేశం->properties = ఖాళీ;
	}

	ఒకవేళ(db_సందేశం_store(context, పునాది_సందేశం, &సందేశం_expiry, mosq_mo_broker)){
		తిరిగి 1;
	}

	ఒకవేళ(pub_సందేశం->qos){
		mid = mosquitto__mid_generate(context);
	}లేకపోతే{
		mid = 0;
	}
	తిరిగి db_సందేశం_చొప్పించు_outgoing(context, 0, mid, (uint8_t)pub_సందేశం->qos, 0, పునాది_సందేశం, 0, true, true);
}


స్థిర శూన్య read_సందేశం_expiry_interval(mosquitto_property **proplist, uint32_t *సందేశం_expiry)
{
	mosquitto_property *p, *మునుపటి = ఖాళీ;

	*సందేశం_expiry = సందేశం_EXPIRY_INFINITE;

	ఒకవేళ(!proplist){
		తిరిగి;
	}

	p = *proplist;
	అయితే(p){
		ఒకవేళ(mosquitto_property_identifier(p) == MQTT_PROP_సందేశం_EXPIRY_INTERVAL){
			*సందేశం_expiry = mosquitto_property_int32_విలువ(p);
			ఒకవేళ(p == *proplist){
				*proplist = mosquitto_property_తదుపరి(p);
			}లేకపోతే{
				మునుపటి->తదుపరి = mosquitto_property_తదుపరి(p);
			}
			mosquitto_property_free(&p);
			తిరిగి;

		}
		మునుపటి = p;
		p = mosquitto_property_తదుపరి(p);
	}
}


స్థిర శూన్య వరుస_plugin_msgs(శూన్య)
{
	నిర్మాణం mosquitto_సందేశం_v5 *సందేశం, *tmp;
	నిర్మాణం mosquitto *context;
	uint32_t సందేశం_expiry;

	DL_FOREACH_సురక్షిత(db.plugin_msgs, సందేశం, tmp){
		DL_DELETE(db.plugin_msgs, సందేశం);

		read_సందేశం_expiry_interval(&సందేశం->properties, &సందేశం_expiry);

		ఒకవేళ(సందేశం->clientid){
			HASH_కనుగొను(hh_id, db.contexts_by_id, సందేశం->clientid, అక్షరశ్రేణి_పొడవు(సందేశం->clientid), context);
			ఒకవేళ(context){
				single_publish(context, సందేశం, సందేశం_expiry);
			}
		}లేకపోతే{
			db_messages_easy_వరుస(ఖాళీ, సందేశం->topic, (uint8_t)సందేశం->qos, (uint32_t)సందేశం->payloadlen, సందేశం->payload, సందేశం->retain, సందేశం_expiry, &సందేశం->properties);
		}
		mosquitto_FREE(సందేశం->topic);
		mosquitto_FREE(సందేశం->payload);
		mosquitto_property_free_all(&సందేశం->properties);
		mosquitto_FREE(సందేశం->clientid);
		mosquitto_FREE(సందేశం);
	}
}


శూన్య loop_update_తదుపరి_event(time_t new_ms)
{
	ఒకవేళ(new_ms > 0 && new_ms < db.తదుపరి_event_ms){
		db.తదుపరి_event_ms = new_ms;
	}
}


పూర్ణాంకం mosquitto_main_loop(నిర్మాణం mosquitto__listener_sock *listensock, పూర్ణాంకం listensock_లెక్క)
{
#నిర్వచితమైతే WITH_PERSISTENCE
	time_t చివరి_backup = mosquitto_సమయం();
#అయితే_ముగింపు
	పూర్ణాంకం rc;


	watchdog__init();
#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_సంస్కరణ_సంఖ్య == 3002000
	memset(&sul, 0, పరిమాణం(నిర్మాణం lws_sorted_usec_list));
#అయితే_ముగింపు

	db.now_s = mosquitto_సమయం();
	db.now_real_s = సమయం(ఖాళీ);

#నిర్వచితమైతే WITH_BRIDGE
	rc = bridge_రిజిస్టరు_local_connections();
	ఒకవేళ(rc){
		తిరిగి rc;
	}
#అయితే_ముగింపు

	అయితే(g_నిరంతరం){
		retain__expiry_check();
		వరుస_plugin_msgs();
		context__free_disused();

		db.తదుపరి_event_ms = 86400000;
#నిర్వచితమైతే WITH_SYS_వృక్షం
		ఒకవేళ(db.config->sys_interval > 0){
			sys_వృక్షం_update(false);
		}
#అయితే_ముగింపు

		keepalive__check();
		watchdog__check();

#నిర్వచితమైతే WITH_BRIDGE
		bridge_check();
#అయితే_ముగింపు
		plugin__handle_tick();
		session_expiry__check();
		will_delay__check();

		rc = mux__handle(listensock, listensock_లెక్క);
		ఒకవేళ(rc){
			తిరిగి rc;
		}

#నిర్వచితమైతే WITH_PERSISTENCE
		ఒకవేళ(db.config->persistence && db.config->autosave_interval){
			ఒకవేళ(db.config->autosave_on_changes){
				ఒకవేళ(db.persistence_changes >= db.config->autosave_interval){
					persist__backup(false);
					db.persistence_changes = 0;
				}
			}లేకపోతే{
				ఒకవేళ(చివరి_backup + db.config->autosave_interval < db.now_s){
					persist__backup(false);
					చివరి_backup = db.now_s;
				}
			}
		}
#అయితే_ముగింపు

		rc = signal__flag_check();
		ఒకవేళ(rc){
			తిరిగి rc;
		}

#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		కొరకు(పూర్ణాంకం i=0; i<db.config->listener_లెక్క; i++){
			/* Extremely hacky, should be using the lws provided external poll
			 * interface, but their interface has changed recently and ours
			 * will soon, so for now websockets clients are second class
			 * citizens. */
			ఒకవేళ(db.config->listeners[i].ws_context){
#ఒకవేళ LWS_LIBRARY_సంస్కరణ_సంఖ్య > 3002000
				lws_service(db.config->listeners[i].ws_context, -1);
#లేదావేళ LWS_LIBRARY_సంస్కరణ_సంఖ్య == 3002000
				lws_sul_కాలపట్టిక(db.config->listeners[i].ws_context, 0, &sul, lws__sul_callback, 10);
				lws_service(db.config->listeners[i].ws_context, 0);
#లేకపోతే
				lws_service(db.config->listeners[i].ws_context, 0);
#అయితే_ముగింపు

			}
		}
#అయితే_ముగింపు
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


శూన్య do_disconnect(నిర్మాణం mosquitto *context, పూర్ణాంకం reason)
{
	స్థిరరాశి అక్షరం *id;
#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	bool is_duplicate = false;
#అయితే_ముగింపు

	ఒకవేళ(context->state == mosq_cs_disconnected){
		తిరిగి;
	}
#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ఒకవేళ(context->wsi){
		ఒకవేళ(context->state == mosq_cs_duplicate){
			is_duplicate = true;
		}

		ఒకవేళ(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
			mosquitto_సమితి_state(context, mosq_cs_disconnect_ws);
		}
		ఒకవేళ(context->wsi){
			lws_callback_on_writable(context->wsi);
		}
		ఒకవేళ(context->sock != INVALID_SOCKET){
			HASH_DELETE(hh_sock, db.contexts_by_sock, context);
			mux__delete(context);
			context->sock = INVALID_SOCKET;
		}
		ఒకవేళ(is_duplicate){
			/* This occurs if another client is taking over the same client id.
			 * It is important to remove this from the by_id hash here, so it
			 * doesn't leave us with multiple clients in the hash with the same
			 * id. Websockets doesn't actually close the connection here,
			 * unlike for normal clients, which means there is extra time when
			 * there could be two clients with the same id in the hash. */
			context__remove_from_by_id(context);
		}
	}లేకపోతే
#అయితే_ముగింపు
	{
		ఒకవేళ(db.config->connection_messages == true){
			ఒకవేళ(context->id){
				id = context->id;
			}లేకపోతే{
				id = context->చిరునామా;
			}
			ఒకవేళ(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
				ఎంపిక(reason){
					స్థితి MOSQ_ERR_SUCCESS:
						ఆపు;
					స్థితి MOSQ_ERR_MALFORMED_PACKET:
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: malformed packet.", id, context->చిరునామా, context->remote_పోర్టు);
						ఆపు;
					స్థితి MOSQ_ERR_PROTOCOL:
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: protocol error.", id, context->చిరునామా, context->remote_పోర్టు);
						ఆపు;
					స్థితి MOSQ_ERR_CONN_కోల్పోయిన:
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection closed by client.", id, context->చిరునామా, context->remote_పోర్టు);
						ఆపు;
					స్థితి MOSQ_ERR_AUTH:
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: not authorised.", id, context->చిరునామా, context->remote_పోర్టు);
						ఆపు;
					స్థితి MOSQ_ERR_KEEPALIVE:
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: exceeded timeout.", id, context->చిరునామా, context->remote_పోర్టు);
						ఆపు;
					స్థితి MOSQ_ERR_OVERSIZE_PACKET:
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize packet.", id, context->చిరునామా, context->remote_పోర్టు);
						ఆపు;
					స్థితి MOSQ_ERR_PAYLOAD_కొలత:
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize payload.", id, context->చిరునామా, context->remote_పోర్టు);
						ఆపు;
					స్థితి MOSQ_ERR_NOMEM:
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: out of memory.", id, context->చిరునామా, context->remote_పోర్టు);
						ఆపు;
					స్థితి MOSQ_ERR_NOT_SUPPORTED:
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: used disallowed feature (QoS too high, retain not supported, or bad AUTH method).", id, context->చిరునామా, context->remote_పోర్టు);
						ఆపు;
					స్థితి MOSQ_ERR_ADMINISTRATIVE_ACTION:
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->చిరునామా, context->remote_పోర్టు);
						ఆపు;
					స్థితి MOSQ_ERR_ERRNO:
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: %s.", id, context->చిరునామా, context->remote_పోర్టు, strerror(errno));
						ఆపు;
					స్థితి MOSQ_ERR_RECEIVE_గరిష్ఠ_EXCEEDED:
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: receive maximum exceeded.", id, context->చిరునామా, context->remote_పోర్టు);
						ఆపు;
					స్థితి MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: implementation specific error.", id, context->చిరునామా, context->remote_పోర్టు);
						ఆపు;
					స్థితి MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client identifier not valid.", id, context->చిరునామా, context->remote_పోర్టు);
						ఆపు;
					స్థితి MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad username or password.", id, context->చిరునామా, context->remote_పోర్టు);
						ఆపు;
					స్థితి MOSQ_ERR_SERVER_UNAVAILABLE:
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server unavailable.", id, context->చిరునామా, context->remote_పోర్టు);
						ఆపు;
					స్థితి MOSQ_ERR_SERVER_BUSY:
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server busy.", id, context->చిరునామా, context->remote_పోర్టు);
						ఆపు;
					స్థితి MOSQ_ERR_BANNED:
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client banned.", id, context->చిరునామా, context->remote_పోర్టు);
						ఆపు;
					స్థితి MOSQ_ERR_BAD_AUTHENTICATION_పద్ధతి:
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad authentication method.", id, context->చిరునామా, context->remote_పోర్టు);
						ఆపు;
					స్థితి MOSQ_ERR_QUOTA_EXCEEDED:
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: quota exceeded.", id, context->చిరునామా, context->remote_పోర్టు);
						ఆపు;
					స్థితి MOSQ_ERR_CONNECTION_రేటు_EXCEEDED:
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection rate exceeded.", id, context->చిరునామా, context->remote_పోర్టు);
						ఆపు;
					స్థితి MOSQ_ERR_SESSION_తీసుకున్న_OVER:
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: session taken over.", id, context->చిరునామా, context->remote_పోర్టు);
						ఆపు;
					స్థితి MOSQ_ERR_TOPIC_ALIAS_INVALID:
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: topic alias invalid.", id, context->చిరునామా, context->remote_పోర్టు);
						ఆపు;
					స్థితి MOSQ_ERR_HTTP_BAD_ORIGIN:
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: non-matching http origin.", id, context->చిరునామా, context->remote_పోర్టు);
						ఆపు;
					స్థితి MOSQ_ERR_PROXY:
						/* This was a proxy v2 health check connection, so don't report */
						ఆపు;
					అప్రమేయ:
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad socket read/write: %s", id, context->చిరునామా, context->remote_పోర్టు, mosquitto_strerror(reason));
						ఆపు;
				}
			}లేకపోతే{
				ఒకవేళ(reason == MOSQ_ERR_ADMINISTRATIVE_ACTION){
					log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->చిరునామా, context->remote_పోర్టు);
				}లేకపోతే{
					log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected.", id, context->చిరునామా, context->remote_పోర్టు);
				}
			}
		}
		mux__delete(context);
		context__disconnect(context, reason);
	}
}
