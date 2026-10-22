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

#شامل "config.h"

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#ifndef WIN32
#شامل <unistd.h>
#وگرنه
#شامل <process.h>
#شامل <winsock2.h>
#شامل <ws2tcpip.h>
#endif

#شامل <errno.h>
#شامل <signal.h>
#شامل <stdio.h>
#شامل <string.h>
#ifndef WIN32
#  include <sys/socket.h>
#endif
#شامل <time.h>
#شامل <utlist.h>

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "packet_mosq.h"
#شامل "property_common.h"
#شامل "send_mosq.h"
#شامل "sys_tree.h"
#شامل "util_mosq.h"

بیرونی صحیح g_run;

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_عدد == 3002000


تهی lws__sul_callback(ساختار lws_sorted_usec_list *l)
{
}

ایستا ساختار lws_sorted_usec_list sul;
#endif


ایستا صحیح single_publish(ساختار mosquitto *context, ساختار mosquitto__message_v5 *pub_پیام, uint32_t message_expiry)
{
	ساختار mosquitto_base_پیام *base_پیام;
	uint16_t mid;

	base_پیام = mosquitto_calloc(1, اندازه(ساختار mosquitto_base_پیام));
	اگر(base_پیام == پوچ){
		بازگشت MOSQ_ERR_NOMEM;
	}

	base_پیام->data.topic = pub_پیام->topic;
	pub_پیام->topic = پوچ;
	base_پیام->data.retain = 0;
	base_پیام->data.payloadlen = (uint32_t)pub_پیام->payloadlen;
	base_پیام->data.payload = mosquitto_malloc(base_پیام->data.payloadlen+1);
	اگر(base_پیام->data.payload == پوچ){
		db_پیام_store_free(base_پیام);
		بازگشت MOSQ_ERR_NOMEM;
	}
	/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
	((uint8_t *)base_پیام->data.payload)[base_پیام->data.payloadlen] = 0;
	memcpy(base_پیام->data.payload, pub_پیام->payload, base_پیام->data.payloadlen);

	اگر(pub_پیام->properties){
		base_پیام->data.properties = pub_پیام->properties;
		pub_پیام->properties = پوچ;
	}

	اگر(db__message_store(context, base_پیام, &message_expiry, mosq_mo_broker)){
		بازگشت 1;
	}

	اگر(pub_پیام->qos){
		mid = mosquitto__mid_generate(context);
	}وگرنه{
		mid = 0;
	}
	بازگشت db_message_درج_outgoing(context, 0, mid, (uint8_t)pub_پیام->qos, 0, base_پیام, 0, true, true);
}


ایستا تهی read_message_expiry_interval(mosquitto_property **proplist, uint32_t *message_expiry)
{
	mosquitto_property *p, *previous = پوچ;

	*message_expiry = پیام_EXPIRY_INFINITE;

	اگر(!proplist){
		بازگشت;
	}

	p = *proplist;
	وقتی(p){
		اگر(mosquitto_property_identifier(p) == MQTT_PROP_MESSAGE_EXPIRY_INTERVAL){
			*message_expiry = mosquitto_property_int32_مقدار(p);
			اگر(p == *proplist){
				*proplist = mosquitto_property_next(p);
			}وگرنه{
				previous->next = mosquitto_property_next(p);
			}
			mosquitto_property_free(&p);
			بازگشت;

		}
		previous = p;
		p = mosquitto_property_next(p);
	}
}


ایستا تهی queue_plugin_msgs(تهی)
{
	ساختار mosquitto__message_v5 *پیام, *tmp;
	ساختار mosquitto *context;
	uint32_t message_expiry;

	DL_FOREACH_SAFE(db.plugin_msgs, پیام, tmp){
		DL_DELETE(db.plugin_msgs, پیام);

		read_message_expiry_interval(&پیام->properties, &message_expiry);

		اگر(پیام->clientid){
			HASH_FIND(hh_id, db.contexts_by_id, پیام->clientid, strlen(پیام->clientid), context);
			اگر(context){
				single_publish(context, پیام, message_expiry);
			}
		}وگرنه{
			db__messages_easy_queue(پوچ, پیام->topic, (uint8_t)پیام->qos, (uint32_t)پیام->payloadlen, پیام->payload, پیام->retain, message_expiry, &پیام->properties);
		}
		mosquitto_FREE(پیام->topic);
		mosquitto_FREE(پیام->payload);
		mosquitto_property_free_all(&پیام->properties);
		mosquitto_FREE(پیام->clientid);
		mosquitto_FREE(پیام);
	}
}


تهی loop__update_next_event(time_t new_ms)
{
	اگر(new_ms > 0 && new_ms < db.next_event_ms){
		db.next_event_ms = new_ms;
	}
}


صحیح mosquitto_main_loop(ساختار mosquitto__listener_sock *listensock, صحیح listensock_شمار)
{
#ifdef WITH_PERSISTENCE
	time_t last_backup = mosquitto_time();
#endif
	صحیح rc;


	watchdog__init();
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_عدد == 3002000
	memset(&sul, 0, اندازه(ساختار lws_sorted_usec_list));
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(پوچ);

#ifdef WITH_BRIDGE
	rc = bridge__register_local_connections();
	اگر(rc){
		بازگشت rc;
	}
#endif

	وقتی(g_run){
		retain__expiry_check();
		queue_plugin_msgs();
		context__free_disused();

		db.next_event_ms = 86400000;
#ifdef WITH_SYS_درخت
		اگر(db.config->sys_interval > 0){
			sys_درخت_update(false);
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

		rc = mux__handle(listensock, listensock_شمار);
		اگر(rc){
			بازگشت rc;
		}

#ifdef WITH_PERSISTENCE
		اگر(db.config->persistence && db.config->autosave_interval){
			اگر(db.config->autosave_on_changes){
				اگر(db.persistence_changes >= db.config->autosave_interval){
					persist__backup(false);
					db.persistence_changes = 0;
				}
			}وگرنه{
				اگر(last_backup + db.config->autosave_interval < db.now_s){
					persist__backup(false);
					last_backup = db.now_s;
				}
			}
		}
#endif

		rc = signal__flag_check();
		اگر(rc){
			بازگشت rc;
		}

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		برای(صحیح i=0; i<db.config->listener_شمار; i++){
			/* Extremely hacky, should be using the lws provided external poll
			 * interface, but their interface has changed recently and ours
			 * will soon, so for now websockets clients are second class
			 * citizens. */
			اگر(db.config->listeners[i].ws_context){
#اگر LWS_LIBRARY_VERSION_عدد > 3002000
				lws_service(db.config->listeners[i].ws_context, -1);
#وگرنهاگر LWS_LIBRARY_VERSION_عدد == 3002000
				lws_sul_schedule(db.config->listeners[i].ws_context, 0, &sul, lws__sul_callback, 10);
				lws_service(db.config->listeners[i].ws_context, 0);
#وگرنه
				lws_service(db.config->listeners[i].ws_context, 0);
#endif

			}
		}
#endif
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


تهی do_disconnect(ساختار mosquitto *context, صحیح reason)
{
	ثابت نویسه *id;
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	bool is_duplicate = false;
#endif

	اگر(context->state == mosq_cs_disconnected){
		بازگشت;
	}
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	اگر(context->wsi){
		اگر(context->state == mosq_cs_duplicate){
			is_duplicate = true;
		}

		اگر(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
			mosquitto__set_state(context, mosq_cs_disconnect_ws);
		}
		اگر(context->wsi){
			lws_callback_on_writable(context->wsi);
		}
		اگر(context->sock != INVALID_SOCKET){
			HASH_DELETE(hh_sock, db.contexts_by_sock, context);
			mux__delete(context);
			context->sock = INVALID_SOCKET;
		}
		اگر(is_duplicate){
			/* This occurs if another client is taking over the same client id.
			 * It is important to remove this from the by_id hash here, so it
			 * doesn't leave us with multiple clients in the hash with the same
			 * id. Websockets doesn't actually close the connection here,
			 * unlike for normal clients, which means there is extra time when
			 * there could be two clients with the same id in the hash. */
			context__remove_from_by_id(context);
		}
	}وگرنه
#endif
	{
		اگر(db.config->connection_messages == true){
			اگر(context->id){
				id = context->id;
			}وگرنه{
				id = context->address;
			}
			اگر(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
				انتخاب(reason){
					حالت MOSQ_ERR_SUCCESS:
						بشکن;
					حالت MOSQ_ERR_MALFORMED_PACKET:
						log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: malformed packet.", id, context->address, context->remote_port);
						بشکن;
					حالت MOSQ_ERR_PROTOCOL:
						log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: protocol error.", id, context->address, context->remote_port);
						بشکن;
					حالت MOSQ_ERR_CONN_LOST:
						log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection closed by client.", id, context->address, context->remote_port);
						بشکن;
					حالت MOSQ_ERR_AUTH:
						log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: not authorised.", id, context->address, context->remote_port);
						بشکن;
					حالت MOSQ_ERR_KEEPALIVE:
						log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: exceeded timeout.", id, context->address, context->remote_port);
						بشکن;
					حالت MOSQ_ERR_OVERSIZE_PACKET:
						log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize packet.", id, context->address, context->remote_port);
						بشکن;
					حالت MOSQ_ERR_PAYLOAD_SIZE:
						log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize payload.", id, context->address, context->remote_port);
						بشکن;
					حالت MOSQ_ERR_NOMEM:
						log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: out of memory.", id, context->address, context->remote_port);
						بشکن;
					حالت MOSQ_ERR_NOT_SUPPORTED:
						log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: used disallowed feature (QoS too high, retain not supported, or bad AUTH method).", id, context->address, context->remote_port);
						بشکن;
					حالت MOSQ_ERR_ADMINISTRATIVE_ACTION:
						log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->address, context->remote_port);
						بشکن;
					حالت MOSQ_ERR_ERRNO:
						log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: %s.", id, context->address, context->remote_port, strerror(errno));
						بشکن;
					حالت MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED:
						log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: receive maximum exceeded.", id, context->address, context->remote_port);
						بشکن;
					حالت MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
						log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: implementation specific error.", id, context->address, context->remote_port);
						بشکن;
					حالت MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
						log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client identifier not valid.", id, context->address, context->remote_port);
						بشکن;
					حالت MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
						log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad username or password.", id, context->address, context->remote_port);
						بشکن;
					حالت MOSQ_ERR_SERVER_UNAVAILABLE:
						log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server unavailable.", id, context->address, context->remote_port);
						بشکن;
					حالت MOSQ_ERR_SERVER_BUSY:
						log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server busy.", id, context->address, context->remote_port);
						بشکن;
					حالت MOSQ_ERR_BANNED:
						log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client banned.", id, context->address, context->remote_port);
						بشکن;
					حالت MOSQ_ERR_BAD_AUTHENTICATION_METHOD:
						log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad authentication method.", id, context->address, context->remote_port);
						بشکن;
					حالت MOSQ_ERR_QUOTA_EXCEEDED:
						log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: quota exceeded.", id, context->address, context->remote_port);
						بشکن;
					حالت MOSQ_ERR_CONNECTION_RATE_EXCEEDED:
						log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection rate exceeded.", id, context->address, context->remote_port);
						بشکن;
					حالت MOSQ_ERR_SESSION_TAKEN_OVER:
						log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: session taken over.", id, context->address, context->remote_port);
						بشکن;
					حالت MOSQ_ERR_TOPIC_ALIAS_INVALID:
						log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: topic alias invalid.", id, context->address, context->remote_port);
						بشکن;
					حالت MOSQ_ERR_HTTP_BAD_ORIGIN:
						log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: non-matching http origin.", id, context->address, context->remote_port);
						بشکن;
					حالت MOSQ_ERR_PROXY:
						/* This was a proxy v2 health check connection, so don't report */
						بشکن;
					پیشفرض:
						log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad socket read/write: %s", id, context->address, context->remote_port, mosquitto_strerror(reason));
						بشکن;
				}
			}وگرنه{
				اگر(reason == MOSQ_ERR_ADMINISTRATIVE_ACTION){
					log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->address, context->remote_port);
				}وگرنه{
					log__printf(پوچ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected.", id, context->address, context->remote_port);
				}
			}
		}
		mux__delete(context);
		context__disconnect(context, reason);
	}
}
