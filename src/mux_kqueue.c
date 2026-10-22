/*
Copyright (c) 2021 Roger Light <roger@atchoo.org>

All rights reserved. This program and the accompanying materials
are made available under the terms of the Eclipse Public License 2.0
and Eclipse Distribution License v1.0 which accompany this distribution.

The Eclipse Public License is available at
   https://www.eclipse.org/legal/epl-2.0/
and the Eclipse Distribution License is available at
  http://www.eclipse.org/org/documents/edl-v10.php.

SPDX-License-Identifier: EPL-2.0 OR EDL-1.0

Contributors:
   Roger Light - initial implementation and documentation.
*/

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_KQUEUE

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_EVENTS 1000

#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/event.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/socket.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "packet_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"

ਸਥਿਰ ਖਾਲੀ loop_handle_reads_writes(ਬਣਤਰ mosquitto *context, ਛੋਟਾ events);

ਸਥਿਰ ਬਣਤਰ kevent event_list[MAX_EVENTS];


ਪੂਰਨ mux_kqueue__init(ਖਾਲੀ)
{
	memset(&event_list, 0, ਆਕਾਰ(ਬਣਤਰ kevent)*MAX_EVENTS);

	db.kqueuefd = 0;
	ਜੇ((db.kqueuefd = kqueue()) == -1){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error in kqueue creating: %s", strerror(errno));
		ਵਾਪਸ MOSQ_ERR_UNKNOWN;
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ mux_kqueue_ਜੋੜੋ_listeners(ਬਣਤਰ mosquitto__listener_sock *listensock, ਪੂਰਨ listensock_ਗਿਣਤੀ)
{
	ਬਣਤਰ kevent ev;

	memset(&ev, 0, ਆਕਾਰ(ਬਣਤਰ kevent));

	ਲਈ(ਪੂਰਨ i=0; i<listensock_ਗਿਣਤੀ; i++){
		EV_ਸੈੱਟ(&ev, listensock[i].sock, EVFILT_READ, EV_ਜੋੜੋ, 0, 0, &listensock[i]);
		ਜੇ(kevent(db.kqueuefd, &ev, 1, ਨਿਰਾ, 0, ਨਿਰਾ) == -1){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error in kqueue initial registering: %s", strerror(errno));
			ਵਾਪਸ MOSQ_ERR_UNKNOWN;
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ mux_kqueue__delete_listeners(ਬਣਤਰ mosquitto__listener_sock *listensock, ਪੂਰਨ listensock_ਗਿਣਤੀ)
{
	ਬਣਤਰ kevent ev;

	ਲਈ(ਪੂਰਨ i=0; i<listensock_ਗਿਣਤੀ; i++){
		EV_ਸੈੱਟ(&ev, listensock[i].sock, EVFILT_READ, EV_DELETE, 0, 0, &listensock[i]);
		ਜੇ(kevent(db.kqueuefd, &ev, 1, ਨਿਰਾ, 0, ਨਿਰਾ) == -1){
			ਵਾਪਸ MOSQ_ERR_UNKNOWN;
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ mux_kqueue_loop_ਤਿਆਰੀ(ਖਾਲੀ)
{
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ mux_kqueue_ਜੋੜੋ_ਬਾਹਰਲਾ(ਬਣਤਰ mosquitto *context)
{
	ਬਣਤਰ kevent ev;

	ਜੇ(context->events != EVFILT_WRITE){
		EV_ਸੈੱਟ(&ev, context->sock, EVFILT_WRITE, EV_ਜੋੜੋ, 0, 0, context);
		ਜੇ(kevent(db.kqueuefd, &ev, 1, ਨਿਰਾ, 0, ਨਿਰਾ) == -1){
			log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG, "Error in kqueue re-registering to EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = EVFILT_WRITE;
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ mux_kqueue_remove_ਬਾਹਰਲਾ(ਬਣਤਰ mosquitto *context)
{
	ਬਣਤਰ kevent ev;

	ਜੇ(context->events == EVFILT_WRITE){
		EV_ਸੈੱਟ(&ev, context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		ਜੇ(kevent(db.kqueuefd, &ev, 1, ਨਿਰਾ, 0, ਨਿਰਾ) == -1){
			log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG, "Error in kqueue removing EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = 0;
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ mux_kqueue__new(ਬਣਤਰ mosquitto *context)
{
	ਬਣਤਰ kevent ev;

	EV_ਸੈੱਟ(&ev, context->sock, EVFILT_READ, EV_ਜੋੜੋ, 0, 0, context);
	ਜੇ(kevent(db.kqueuefd, &ev, 1, ਨਿਰਾ, 0, ਨਿਰਾ) == -1){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error in kqueue accepting: %s", strerror(errno));
	}

	context->events = 0;
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ mux_kqueue__delete(ਬਣਤਰ mosquitto *context)
{
	ਬਣਤਰ kevent ev[2];

	ਜੇ(context->sock != INVALID_SOCKET){
		EV_ਸੈੱਟ(&ev[0], context->sock, EVFILT_READ, EV_DELETE, 0, 0, context);
		EV_ਸੈੱਟ(&ev[1], context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		ਜੇ(kevent(db.kqueuefd, ev, 2, ਨਿਰਾ, 0, ਨਿਰਾ) == -1){
			ਵਾਪਸ 1;
		}
	}
	ਵਾਪਸ 0;
}


ਪੂਰਨ mux_kqueue__handle(ਖਾਲੀ)
{
	ਬਣਤਰ mosquitto *context;
	ਬਣਤਰ mosquitto__listener_sock *listensock;
	ਪੂਰਨ event_ਗਿਣਤੀ;
	ਬਣਤਰ timespec timeout;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_WEBSOCKETS
	timeout.tv_sec = 0;
	timeout.tv_nsec = 100000000; /* 100 ms */
#ਨਹੀਂਤਾਂ
	timeout.tv_sec = db.ਅਗਲਾ_event_ms/1000;
	timeout.tv_nsec = (db.ਅਗਲਾ_event_ms - timeout.tv_sec*1000) * 1000000;
#ਜੇ_ਅੰਤ

	event_ਗਿਣਤੀ = kevent(db.kqueuefd,
			ਨਿਰਾ, 0,
			event_list, MAX_EVENTS,
			&timeout);

	db.now_s = mosquitto_ਸਮਾਂ();
	db.now_real_s = ਸਮਾਂ(ਨਿਰਾ);

	ਚੋਣ(event_ਗਿਣਤੀ){
		ਹਾਲਤ -1:
			ਜੇ(errno != EINTR){
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error in kqueue waiting: %s.", strerror(errno));
			}
			ਰੋਕੋ;
		ਹਾਲਤ 0:
			ਰੋਕੋ;
		ਮੂਲ:
			ਲਈ(ਪੂਰਨ i=0; i<event_ਗਿਣਤੀ; i++){
				context = event_list[i].udata;
				ਜੇ(context->ident == id_client){
					loop_handle_reads_writes(context, event_list[i].filter);
					ਜੇ(event_list[i].flags & (EV_EOF | EV_ERROR)){
						do_disconnect(context, MOSQ_ERR_CONN_ਗੁਆਚਾ);
					}
				}ਨਹੀਂਤਾਂ ਜੇ(context->ident == id_listener){
					listensock = event_list[i].udata;

					ਜੇ(event_list[i].filter == EVFILT_READ){
						ਜਦੋਂ((context = net__socket_accept(listensock)) != ਨਿਰਾ){
						}
					}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_WEBSOCKETS
				}ਨਹੀਂਤਾਂ ਜੇ(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#ਜੇ_ਅੰਤ
				}
			}
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ mux_kqueue__cleanup(ਖਾਲੀ)
{
	(ਖਾਲੀ)close(db.kqueuefd);
	db.kqueuefd = 0;
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਖਾਲੀ loop_handle_reads_writes(ਬਣਤਰ mosquitto *context, ਛੋਟਾ event)
{
	ਪੂਰਨ err;
	socklen_t len;
	ਪੂਰਨ rc;

	ਜੇ(context->sock == INVALID_SOCKET){
		ਵਾਪਸ;
	}

#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ਜੇ(context->wsi){
		ਬਣਤਰ lws_pollfd wspoll;
		wspoll.fd = context->sock;
		int16_t lws_events;
		ਚੋਣ(event){
			ਹਾਲਤ EVFILT_READ:
				lws_events = LWS_POLLIN;
				ਰੋਕੋ;
			ਹਾਲਤ EVFILT_WRITE:
				lws_events = LWS_POLLOUT;
				ਰੋਕੋ;
			ਮੂਲ:
				lws_events = LWS_POLLHUP;
				ਰੋਕੋ;
		}
		wspoll.events = lws_events;
		wspoll.revents = lws_events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		ਵਾਪਸ;
	}
#ਜੇ_ਅੰਤ

	ਜੇ(event == EVFILT_WRITE
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
			|| context->want_write
			|| (context->ssl && context->state == mosq_cs_new)
#ਜੇ_ਅੰਤ
			){

		ਜੇ(context->state == mosq_cs_connect_pending){
			len = ਆਕਾਰ(ਪੂਰਨ);
			ਜੇ(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (ਅੱਖਰ *)&err, &len)){
				ਜੇ(err == 0){
					mosquitto_ਸੈੱਟ_state(context, mosq_cs_new);
#ਜੇ defined(WITH_ADNS) && defined(WITH_BRIDGE)
					ਜੇ(context->bridge){
						bridge__connect_step3(context);
					}
#ਜੇ_ਅੰਤ
				}
			}ਨਹੀਂਤਾਂ{
				do_disconnect(context, MOSQ_ERR_CONN_ਗੁਆਚਾ);
				ਵਾਪਸ;
			}
		}
		rc = packet__write(context);
		ਜੇ(rc){
			do_disconnect(context, rc);
			ਵਾਪਸ;
		}
	}

	ਜੇ(event == EVFILT_READ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
			|| (context->ssl && context->state == mosq_cs_new)
#ਜੇ_ਅੰਤ
			){

		ਕਰੋ{
			ਚੋਣ(context->transport){
				ਹਾਲਤ mosq_t_tcp:
				ਹਾਲਤ mosq_t_ws:
					rc = packet__read(context);
					ਰੋਕੋ;
#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
				ਹਾਲਤ mosq_t_http:
					rc = http__read(context);
					ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
				/* Not supported with LWS */
				ਹਾਲਤ mosq_t_proxy_v2:
					rc = proxy_v2__read(context);
					ਰੋਕੋ;
				ਹਾਲਤ mosq_t_proxy_v1:
					rc = proxy_v1__read(context);
					ਰੋਕੋ;
#ਜੇ_ਅੰਤ
				ਮੂਲ:
					rc = MOSQ_ERR_INVAL;
					ਰੋਕੋ;
			}
			ਜੇ(rc){
				do_disconnect(context, rc);
				ਵਾਪਸ;
			}
		}ਜਦੋਂ(SSL_ਅੰਕੜੇ_PENDING(context));
	}
}
#ਜੇ_ਅੰਤ
