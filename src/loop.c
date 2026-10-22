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

#অন্তর্ভাব "config.h"

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#ifndef WIN32
#অন্তর্ভাব <unistd.h>
#অন্যথা
#অন্তর্ভাব <process.h>
#অন্তর্ভাব <winsock2.h>
#অন্তর্ভাব <ws2tcpip.h>
#endif

#অন্তর্ভাব <errno.h>
#অন্তর্ভাব <signal.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>
#ifndef WIN32
#  include <sys/socket.h>
#endif
#অন্তর্ভাব <time.h>
#অন্তর্ভাব <utlist.h>

#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto/mqtt_protocol.h"
#অন্তর্ভাব "packet_mosq.h"
#অন্তর্ভাব "property_common.h"
#অন্তর্ভাব "send_mosq.h"
#অন্তর্ভাব "sys_tree.h"
#অন্তর্ভাব "util_mosq.h"

বাহ্যিক পূর্ণসংখ্যা g_run;

#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_সংখ্যা == 3002000


শূন্য lws__sul_callback(কাঠামো lws_sorted_usec_list *l)
{
}

স্থির কাঠামো lws_sorted_usec_list sul;
#endif


স্থির পূর্ণসংখ্যা single_publish(কাঠামো mosquitto *context, কাঠামো mosquitto__message_v5 *pub_বার্তা, uint32_t message_expiry)
{
	কাঠামো mosquitto_base_বার্তা *base_বার্তা;
	uint16_t mid;

	base_বার্তা = mosquitto_calloc(1, আকার(কাঠামো mosquitto_base_বার্তা));
	যদি(base_বার্তা == ফাঁকা){
		ফেরত MOSQ_ERR_NOMEM;
	}

	base_বার্তা->তথ্য.topic = pub_বার্তা->topic;
	pub_বার্তা->topic = ফাঁকা;
	base_বার্তা->তথ্য.retain = 0;
	base_বার্তা->তথ্য.payloadlen = (uint32_t)pub_বার্তা->payloadlen;
	base_বার্তা->তথ্য.payload = mosquitto_malloc(base_বার্তা->তথ্য.payloadlen+1);
	যদি(base_বার্তা->তথ্য.payload == ফাঁকা){
		db_বার্তা_store_free(base_বার্তা);
		ফেরত MOSQ_ERR_NOMEM;
	}
	/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
	((uint8_t *)base_বার্তা->তথ্য.payload)[base_বার্তা->তথ্য.payloadlen] = 0;
	memcpy(base_বার্তা->তথ্য.payload, pub_বার্তা->payload, base_বার্তা->তথ্য.payloadlen);

	যদি(pub_বার্তা->properties){
		base_বার্তা->তথ্য.properties = pub_বার্তা->properties;
		pub_বার্তা->properties = ফাঁকা;
	}

	যদি(db__message_store(context, base_বার্তা, &message_expiry, mosq_mo_broker)){
		ফেরত 1;
	}

	যদি(pub_বার্তা->qos){
		mid = mosquitto__mid_generate(context);
	}অন্যথা{
		mid = 0;
	}
	ফেরত db_message_ঢোকাও_outgoing(context, 0, mid, (uint8_t)pub_বার্তা->qos, 0, base_বার্তা, 0, true, true);
}


স্থির শূন্য read_message_expiry_interval(mosquitto_property **proplist, uint32_t *message_expiry)
{
	mosquitto_property *p, *previous = ফাঁকা;

	*message_expiry = বার্তা_EXPIRY_INFINITE;

	যদি(!proplist){
		ফেরত;
	}

	p = *proplist;
	যখন(p){
		যদি(mosquitto_property_identifier(p) == MQTT_PROP_MESSAGE_EXPIRY_INTERVAL){
			*message_expiry = mosquitto_property_int32_মান(p);
			যদি(p == *proplist){
				*proplist = mosquitto_property_পরবর্তী(p);
			}অন্যথা{
				previous->পরবর্তী = mosquitto_property_পরবর্তী(p);
			}
			mosquitto_property_free(&p);
			ফেরত;

		}
		previous = p;
		p = mosquitto_property_পরবর্তী(p);
	}
}


স্থির শূন্য queue_plugin_msgs(শূন্য)
{
	কাঠামো mosquitto__message_v5 *বার্তা, *tmp;
	কাঠামো mosquitto *context;
	uint32_t message_expiry;

	DL_FOREACH_SAFE(db.plugin_msgs, বার্তা, tmp){
		DL_DELETE(db.plugin_msgs, বার্তা);

		read_message_expiry_interval(&বার্তা->properties, &message_expiry);

		যদি(বার্তা->clientid){
			HASH_FIND(hh_id, db.contexts_by_id, বার্তা->clientid, strlen(বার্তা->clientid), context);
			যদি(context){
				single_publish(context, বার্তা, message_expiry);
			}
		}অন্যথা{
			db__messages_easy_queue(ফাঁকা, বার্তা->topic, (uint8_t)বার্তা->qos, (uint32_t)বার্তা->payloadlen, বার্তা->payload, বার্তা->retain, message_expiry, &বার্তা->properties);
		}
		mosquitto_FREE(বার্তা->topic);
		mosquitto_FREE(বার্তা->payload);
		mosquitto_property_free_all(&বার্তা->properties);
		mosquitto_FREE(বার্তা->clientid);
		mosquitto_FREE(বার্তা);
	}
}


শূন্য loop_update_পরবর্তী_event(time_t new_ms)
{
	যদি(new_ms > 0 && new_ms < db.পরবর্তী_event_ms){
		db.পরবর্তী_event_ms = new_ms;
	}
}


পূর্ণসংখ্যা mosquitto_main_loop(কাঠামো mosquitto__listener_sock *listensock, পূর্ণসংখ্যা listensock_গুনতি)
{
#ifdef WITH_PERSISTENCE
	time_t last_backup = mosquitto_time();
#endif
	পূর্ণসংখ্যা rc;


	watchdog__init();
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_সংখ্যা == 3002000
	memset(&sul, 0, আকার(কাঠামো lws_sorted_usec_list));
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(ফাঁকা);

#ifdef WITH_BRIDGE
	rc = bridge__register_local_connections();
	যদি(rc){
		ফেরত rc;
	}
#endif

	যখন(g_run){
		retain__expiry_check();
		queue_plugin_msgs();
		context__free_disused();

		db.পরবর্তী_event_ms = 86400000;
#ifdef WITH_SYS_বৃক্ষ
		যদি(db.config->sys_interval > 0){
			sys_বৃক্ষ_update(false);
		}
#endif

		keepalive__check();
		watchdog__check();

#ifdef WITH_BRIDGE
		bridge_check();
#endif
		plugin__handle_tick();
		session_expiry__check();
		will_delay__check();

		rc = mux__handle(listensock, listensock_গুনতি);
		যদি(rc){
			ফেরত rc;
		}

#ifdef WITH_PERSISTENCE
		যদি(db.config->persistence && db.config->autosave_interval){
			যদি(db.config->autosave_on_changes){
				যদি(db.persistence_changes >= db.config->autosave_interval){
					persist__backup(false);
					db.persistence_changes = 0;
				}
			}অন্যথা{
				যদি(last_backup + db.config->autosave_interval < db.now_s){
					persist__backup(false);
					last_backup = db.now_s;
				}
			}
		}
#endif

		rc = signal__flag_check();
		যদি(rc){
			ফেরত rc;
		}

#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		জন্য(পূর্ণসংখ্যা i=0; i<db.config->listener_গুনতি; i++){
			/* Extremely hacky, should be using the lws provided external poll
			 * interface, but their interface has changed recently and ours
			 * will soon, so for now websockets clients are second class
			 * citizens. */
			যদি(db.config->listeners[i].ws_context){
#যদি LWS_LIBRARY_VERSION_সংখ্যা > 3002000
				lws_service(db.config->listeners[i].ws_context, -1);
#নতুবা_যদি LWS_LIBRARY_VERSION_সংখ্যা == 3002000
				lws_sul_schedule(db.config->listeners[i].ws_context, 0, &sul, lws__sul_callback, 10);
				lws_service(db.config->listeners[i].ws_context, 0);
#অন্যথা
				lws_service(db.config->listeners[i].ws_context, 0);
#endif

			}
		}
#endif
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


শূন্য do_disconnect(কাঠামো mosquitto *context, পূর্ণসংখ্যা reason)
{
	ধ্রুবক অক্ষর *id;
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	bool is_duplicate = false;
#endif

	যদি(context->state == mosq_cs_disconnected){
		ফেরত;
	}
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	যদি(context->wsi){
		যদি(context->state == mosq_cs_duplicate){
			is_duplicate = true;
		}

		যদি(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
			mosquitto__set_state(context, mosq_cs_disconnect_ws);
		}
		যদি(context->wsi){
			lws_callback_on_writable(context->wsi);
		}
		যদি(context->sock != INVALID_SOCKET){
			HASH_DELETE(hh_sock, db.contexts_by_sock, context);
			mux__delete(context);
			context->sock = INVALID_SOCKET;
		}
		যদি(is_duplicate){
			/* This occurs if another client is taking over the same client id.
			 * It is important to remove this from the by_id hash here, so it
			 * doesn't leave us with multiple clients in the hash with the same
			 * id. Websockets doesn't actually close the connection here,
			 * unlike for normal clients, which means there is extra time when
			 * there could be two clients with the same id in the hash. */
			context__remove_from_by_id(context);
		}
	}অন্যথা
#endif
	{
		যদি(db.config->connection_messages == true){
			যদি(context->id){
				id = context->id;
			}অন্যথা{
				id = context->address;
			}
			যদি(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
				নির্বাচন(reason){
					ক্ষেত্র MOSQ_ERR_SUCCESS:
						বিরতি;
					ক্ষেত্র MOSQ_ERR_MALFORMED_PACKET:
						log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: malformed packet.", id, context->address, context->remote_port);
						বিরতি;
					ক্ষেত্র MOSQ_ERR_PROTOCOL:
						log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: protocol error.", id, context->address, context->remote_port);
						বিরতি;
					ক্ষেত্র MOSQ_ERR_CONN_LOST:
						log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection closed by client.", id, context->address, context->remote_port);
						বিরতি;
					ক্ষেত্র MOSQ_ERR_AUTH:
						log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: not authorised.", id, context->address, context->remote_port);
						বিরতি;
					ক্ষেত্র MOSQ_ERR_KEEPALIVE:
						log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: exceeded timeout.", id, context->address, context->remote_port);
						বিরতি;
					ক্ষেত্র MOSQ_ERR_OVERSIZE_PACKET:
						log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize packet.", id, context->address, context->remote_port);
						বিরতি;
					ক্ষেত্র MOSQ_ERR_PAYLOAD_আকার:
						log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize payload.", id, context->address, context->remote_port);
						বিরতি;
					ক্ষেত্র MOSQ_ERR_NOMEM:
						log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: out of memory.", id, context->address, context->remote_port);
						বিরতি;
					ক্ষেত্র MOSQ_ERR_NOT_SUPPORTED:
						log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: used disallowed feature (QoS too high, retain not supported, or bad AUTH method).", id, context->address, context->remote_port);
						বিরতি;
					ক্ষেত্র MOSQ_ERR_ADMINISTRATIVE_ACTION:
						log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->address, context->remote_port);
						বিরতি;
					ক্ষেত্র MOSQ_ERR_ERRNO:
						log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: %s.", id, context->address, context->remote_port, strerror(errno));
						বিরতি;
					ক্ষেত্র MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED:
						log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: receive maximum exceeded.", id, context->address, context->remote_port);
						বিরতি;
					ক্ষেত্র MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
						log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: implementation specific error.", id, context->address, context->remote_port);
						বিরতি;
					ক্ষেত্র MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
						log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client identifier not valid.", id, context->address, context->remote_port);
						বিরতি;
					ক্ষেত্র MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
						log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad username or password.", id, context->address, context->remote_port);
						বিরতি;
					ক্ষেত্র MOSQ_ERR_SERVER_UNAVAILABLE:
						log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server unavailable.", id, context->address, context->remote_port);
						বিরতি;
					ক্ষেত্র MOSQ_ERR_SERVER_BUSY:
						log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server busy.", id, context->address, context->remote_port);
						বিরতি;
					ক্ষেত্র MOSQ_ERR_BANNED:
						log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client banned.", id, context->address, context->remote_port);
						বিরতি;
					ক্ষেত্র MOSQ_ERR_BAD_AUTHENTICATION_METHOD:
						log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad authentication method.", id, context->address, context->remote_port);
						বিরতি;
					ক্ষেত্র MOSQ_ERR_QUOTA_EXCEEDED:
						log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: quota exceeded.", id, context->address, context->remote_port);
						বিরতি;
					ক্ষেত্র MOSQ_ERR_CONNECTION_RATE_EXCEEDED:
						log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection rate exceeded.", id, context->address, context->remote_port);
						বিরতি;
					ক্ষেত্র MOSQ_ERR_SESSION_TAKEN_OVER:
						log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: session taken over.", id, context->address, context->remote_port);
						বিরতি;
					ক্ষেত্র MOSQ_ERR_TOPIC_ALIAS_INVALID:
						log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: topic alias invalid.", id, context->address, context->remote_port);
						বিরতি;
					ক্ষেত্র MOSQ_ERR_HTTP_BAD_ORIGIN:
						log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: non-matching http origin.", id, context->address, context->remote_port);
						বিরতি;
					ক্ষেত্র MOSQ_ERR_PROXY:
						/* This was a proxy v2 health check connection, so don't report */
						বিরতি;
					পূর্বনির্ধারিত:
						log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad socket read/write: %s", id, context->address, context->remote_port, mosquitto_strerror(reason));
						বিরতি;
				}
			}অন্যথা{
				যদি(reason == MOSQ_ERR_ADMINISTRATIVE_ACTION){
					log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->address, context->remote_port);
				}অন্যথা{
					log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected.", id, context->address, context->remote_port);
				}
			}
		}
		mux__delete(context);
		context__disconnect(context, reason);
	}
}
