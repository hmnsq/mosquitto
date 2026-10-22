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

#ضمن "config.h"

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#ifndef WIN32
#ضمن <unistd.h>
#وإلا
#ضمن <process.h>
#ضمن <winsock2.h>
#ضمن <ws2tcpip.h>
#endif

#ضمن <errno.h>
#ضمن <signal.h>
#ضمن <stdio.h>
#ضمن <string.h>
#ifndef WIN32
#  include <sys/socket.h>
#endif
#ضمن <time.h>
#ضمن <utlist.h>

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "packet_mosq.h"
#ضمن "property_common.h"
#ضمن "send_mosq.h"
#ضمن "sys_tree.h"
#ضمن "util_mosq.h"

خارجي صحيح g_run;

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_ژمار == 3002000


فراغ lws__sul_callback(بنية lws_sorted_usec_list *l)
{
}

ساكن بنية lws_sorted_usec_list sul;
#endif


ساكن صحيح single_publish(بنية mosquitto *context, بنية mosquitto__message_v5 *pub_پەیام, uint32_t message_expiry)
{
	بنية mosquitto_base_پەیام *base_پەیام;
	uint16_t mid;

	base_پەیام = mosquitto_calloc(1, حجم(بنية mosquitto_base_پەیام));
	إذا(base_پەیام == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}

	base_پەیام->data.topic = pub_پەیام->topic;
	pub_پەیام->topic = عدم;
	base_پەیام->data.retain = 0;
	base_پەیام->data.payloadlen = (uint32_t)pub_پەیام->payloadlen;
	base_پەیام->data.payload = mosquitto_malloc(base_پەیام->data.payloadlen+1);
	إذا(base_پەیام->data.payload == عدم){
		db_پەیام_store_free(base_پەیام);
		أرجع MOSQ_ERR_NOMEM;
	}
	/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
	((uint8_t *)base_پەیام->data.payload)[base_پەیام->data.payloadlen] = 0;
	memcpy(base_پەیام->data.payload, pub_پەیام->payload, base_پەیام->data.payloadlen);

	إذا(pub_پەیام->properties){
		base_پەیام->data.properties = pub_پەیام->properties;
		pub_پەیام->properties = عدم;
	}

	إذا(db__message_store(context, base_پەیام, &message_expiry, mosq_mo_broker)){
		أرجع 1;
	}

	إذا(pub_پەیام->qos){
		mid = mosquitto__mid_generate(context);
	}وإلا{
		mid = 0;
	}
	أرجع db_message_زیادکردن_outgoing(context, 0, mid, (uint8_t)pub_پەیام->qos, 0, base_پەیام, 0, true, true);
}


ساكن فراغ read_message_expiry_interval(mosquitto_property **proplist, uint32_t *message_expiry)
{
	mosquitto_property *p, *previous = عدم;

	*message_expiry = پەیام_EXPIRY_INFINITE;

	إذا(!proplist){
		أرجع;
	}

	p = *proplist;
	طالما(p){
		إذا(mosquitto_property_identifier(p) == MQTT_PROP_MESSAGE_EXPIRY_INTERVAL){
			*message_expiry = mosquitto_property_int32_بەها(p);
			إذا(p == *proplist){
				*proplist = mosquitto_property_next(p);
			}وإلا{
				previous->next = mosquitto_property_next(p);
			}
			mosquitto_property_free(&p);
			أرجع;

		}
		previous = p;
		p = mosquitto_property_next(p);
	}
}


ساكن فراغ queue_plugin_msgs(فراغ)
{
	بنية mosquitto__message_v5 *پەیام, *tmp;
	بنية mosquitto *context;
	uint32_t message_expiry;

	DL_FOREACH_SAFE(db.plugin_msgs, پەیام, tmp){
		DL_DELETE(db.plugin_msgs, پەیام);

		read_message_expiry_interval(&پەیام->properties, &message_expiry);

		إذا(پەیام->clientid){
			HASH_FIND(hh_id, db.contexts_by_id, پەیام->clientid, strlen(پەیام->clientid), context);
			إذا(context){
				single_publish(context, پەیام, message_expiry);
			}
		}وإلا{
			db__messages_easy_queue(عدم, پەیام->topic, (uint8_t)پەیام->qos, (uint32_t)پەیام->payloadlen, پەیام->payload, پەیام->retain, message_expiry, &پەیام->properties);
		}
		mosquitto_FREE(پەیام->topic);
		mosquitto_FREE(پەیام->payload);
		mosquitto_property_free_all(&پەیام->properties);
		mosquitto_FREE(پەیام->clientid);
		mosquitto_FREE(پەیام);
	}
}


فراغ loop__update_next_event(time_t new_ms)
{
	إذا(new_ms > 0 && new_ms < db.next_event_ms){
		db.next_event_ms = new_ms;
	}
}


صحيح mosquitto_main_loop(بنية mosquitto__listener_sock *listensock, صحيح listensock_ژماردن)
{
#ifdef WITH_PERSISTENCE
	time_t last_backup = mosquitto_time();
#endif
	صحيح rc;


	watchdog__init();
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_ژمار == 3002000
	memset(&sul, 0, حجم(بنية lws_sorted_usec_list));
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(عدم);

#ifdef WITH_BRIDGE
	rc = bridge__register_local_connections();
	إذا(rc){
		أرجع rc;
	}
#endif

	طالما(g_run){
		retain__expiry_check();
		queue_plugin_msgs();
		context__free_disused();

		db.next_event_ms = 86400000;
#ifdef WITH_SYS_درەخت
		إذا(db.config->sys_interval > 0){
			sys_درەخت_update(false);
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

		rc = mux__handle(listensock, listensock_ژماردن);
		إذا(rc){
			أرجع rc;
		}

#ifdef WITH_PERSISTENCE
		إذا(db.config->persistence && db.config->autosave_interval){
			إذا(db.config->autosave_on_changes){
				إذا(db.persistence_changes >= db.config->autosave_interval){
					persist__backup(false);
					db.persistence_changes = 0;
				}
			}وإلا{
				إذا(last_backup + db.config->autosave_interval < db.now_s){
					persist__backup(false);
					last_backup = db.now_s;
				}
			}
		}
#endif

		rc = signal__flag_check();
		إذا(rc){
			أرجع rc;
		}

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		لكل(صحيح i=0; i<db.config->listener_ژماردن; i++){
			/* Extremely hacky, should be using the lws provided external poll
			 * interface, but their interface has changed recently and ours
			 * will soon, so for now websockets clients are second class
			 * citizens. */
			إذا(db.config->listeners[i].ws_context){
#إذا LWS_LIBRARY_VERSION_ژمار > 3002000
				lws_service(db.config->listeners[i].ws_context, -1);
#وإلاإذا LWS_LIBRARY_VERSION_ژمار == 3002000
				lws_sul_schedule(db.config->listeners[i].ws_context, 0, &sul, lws__sul_callback, 10);
				lws_service(db.config->listeners[i].ws_context, 0);
#وإلا
				lws_service(db.config->listeners[i].ws_context, 0);
#endif

			}
		}
#endif
	}

	أرجع MOSQ_ERR_SUCCESS;
}


فراغ do_disconnect(بنية mosquitto *context, صحيح reason)
{
	ثابت حرف *id;
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	bool is_duplicate = false;
#endif

	إذا(context->state == mosq_cs_disconnected){
		أرجع;
	}
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	إذا(context->wsi){
		إذا(context->state == mosq_cs_duplicate){
			is_duplicate = true;
		}

		إذا(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
			mosquitto__set_state(context, mosq_cs_disconnect_ws);
		}
		إذا(context->wsi){
			lws_callback_on_writable(context->wsi);
		}
		إذا(context->sock != INVALID_SOCKET){
			HASH_DELETE(hh_sock, db.contexts_by_sock, context);
			mux__delete(context);
			context->sock = INVALID_SOCKET;
		}
		إذا(is_duplicate){
			/* This occurs if another client is taking over the same client id.
			 * It is important to remove this from the by_id hash here, so it
			 * doesn't leave us with multiple clients in the hash with the same
			 * id. Websockets doesn't actually close the connection here,
			 * unlike for normal clients, which means there is extra time when
			 * there could be two clients with the same id in the hash. */
			context__remove_from_by_id(context);
		}
	}وإلا
#endif
	{
		إذا(db.config->connection_messages == true){
			إذا(context->id){
				id = context->id;
			}وإلا{
				id = context->address;
			}
			إذا(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
				بدل(reason){
					حالة MOSQ_ERR_SUCCESS:
						اكسر;
					حالة MOSQ_ERR_MALFORMED_PACKET:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: malformed packet.", id, context->address, context->remote_port);
						اكسر;
					حالة MOSQ_ERR_PROTOCOL:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: protocol error.", id, context->address, context->remote_port);
						اكسر;
					حالة MOSQ_ERR_CONN_LOST:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection closed by client.", id, context->address, context->remote_port);
						اكسر;
					حالة MOSQ_ERR_AUTH:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: not authorised.", id, context->address, context->remote_port);
						اكسر;
					حالة MOSQ_ERR_KEEPALIVE:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: exceeded timeout.", id, context->address, context->remote_port);
						اكسر;
					حالة MOSQ_ERR_OVERSIZE_PACKET:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize packet.", id, context->address, context->remote_port);
						اكسر;
					حالة MOSQ_ERR_PAYLOAD_SIZE:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize payload.", id, context->address, context->remote_port);
						اكسر;
					حالة MOSQ_ERR_NOMEM:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: out of memory.", id, context->address, context->remote_port);
						اكسر;
					حالة MOSQ_ERR_NOT_SUPPORTED:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: used disallowed feature (QoS too high, retain not supported, or bad AUTH method).", id, context->address, context->remote_port);
						اكسر;
					حالة MOSQ_ERR_ADMINISTRATIVE_ACTION:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->address, context->remote_port);
						اكسر;
					حالة MOSQ_ERR_ERRNO:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: %s.", id, context->address, context->remote_port, strerror(errno));
						اكسر;
					حالة MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: receive maximum exceeded.", id, context->address, context->remote_port);
						اكسر;
					حالة MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: implementation specific error.", id, context->address, context->remote_port);
						اكسر;
					حالة MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client identifier not valid.", id, context->address, context->remote_port);
						اكسر;
					حالة MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad username or password.", id, context->address, context->remote_port);
						اكسر;
					حالة MOSQ_ERR_SERVER_UNAVAILABLE:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server unavailable.", id, context->address, context->remote_port);
						اكسر;
					حالة MOSQ_ERR_SERVER_BUSY:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server busy.", id, context->address, context->remote_port);
						اكسر;
					حالة MOSQ_ERR_BANNED:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client banned.", id, context->address, context->remote_port);
						اكسر;
					حالة MOSQ_ERR_BAD_AUTHENTICATION_METHOD:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad authentication method.", id, context->address, context->remote_port);
						اكسر;
					حالة MOSQ_ERR_QUOTA_EXCEEDED:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: quota exceeded.", id, context->address, context->remote_port);
						اكسر;
					حالة MOSQ_ERR_CONNECTION_RATE_EXCEEDED:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection rate exceeded.", id, context->address, context->remote_port);
						اكسر;
					حالة MOSQ_ERR_SESSION_TAKEN_OVER:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: session taken over.", id, context->address, context->remote_port);
						اكسر;
					حالة MOSQ_ERR_TOPIC_ALIAS_INVALID:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: topic alias invalid.", id, context->address, context->remote_port);
						اكسر;
					حالة MOSQ_ERR_HTTP_BAD_ORIGIN:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: non-matching http origin.", id, context->address, context->remote_port);
						اكسر;
					حالة MOSQ_ERR_PROXY:
						/* This was a proxy v2 health check connection, so don't report */
						اكسر;
					افتراضي:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad socket read/write: %s", id, context->address, context->remote_port, mosquitto_strerror(reason));
						اكسر;
				}
			}وإلا{
				إذا(reason == MOSQ_ERR_ADMINISTRATIVE_ACTION){
					log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->address, context->remote_port);
				}وإلا{
					log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected.", id, context->address, context->remote_port);
				}
			}
		}
		mux__delete(context);
		context__disconnect(context, reason);
	}
}
