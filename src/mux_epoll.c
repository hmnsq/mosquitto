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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_EPOLL

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
#  define GNU_ਸਰੋਤ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
#  include <sys/epoll.h>
#  define MAX_EVENTS 1000
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mux.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "packet_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"

ਸਥਿਰ ਖਾਲੀ loop_handle_reads_writes(ਬਣਤਰ mosquitto *context, uint32_t events);

ਸਥਿਰ ਬਣਤਰ epoll_event ep_events[MAX_EVENTS];


ਪੂਰਨ mux_epoll__init(ਖਾਲੀ)
{
	memset(&ep_events, 0, ਆਕਾਰ(ਬਣਤਰ epoll_event)*MAX_EVENTS);

	db.epollfd = 0;
	ਜੇ((db.epollfd = epoll_create(MAX_EVENTS)) == -1){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error in epoll creating: %s", strerror(errno));
		ਵਾਪਸ MOSQ_ERR_UNKNOWN;
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ mux_epoll_ਜੋੜੋ_listeners(ਬਣਤਰ mosquitto__listener_sock *listensock, ਪੂਰਨ listensock_ਗਿਣਤੀ)
{
	ਲਈ(ਪੂਰਨ i=0; i<listensock_ਗਿਣਤੀ; i++){
		ਬਣਤਰ epoll_event ev;

		memset(&ev, 0, ਆਕਾਰ(ਬਣਤਰ epoll_event));
		ev.ਅੰਕੜੇ.ptr = &listensock[i];
		ev.events = EPOLLIN;
		ਜੇ(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, listensock[i].sock, &ev) == -1){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error in epoll initial registering: %s", strerror(errno));
			ਵਾਪਸ MOSQ_ERR_UNKNOWN;
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ mux_epoll__delete_listeners(ਬਣਤਰ mosquitto__listener_sock *listensock, ਪੂਰਨ listensock_ਗਿਣਤੀ)
{
	ਲਈ(ਪੂਰਨ i=0; i<listensock_ਗਿਣਤੀ; i++){
		ਜੇ(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, listensock[i].sock, ਨਿਰਾ) == -1){
			ਵਾਪਸ MOSQ_ERR_UNKNOWN;
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ mux_epoll_ਜੋੜੋ_ਬਾਹਰਲਾ(ਬਣਤਰ mosquitto *context)
{
	ਜੇ(!(context->events & EPOLLOUT)){
		ਬਣਤਰ epoll_event ev;

		memset(&ev, 0, ਆਕਾਰ(ਬਣਤਰ epoll_event));
		ev.ਅੰਕੜੇ.ptr = context;
		ev.events = EPOLLIN | EPOLLOUT;
		ਜੇ(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			ਜੇ((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLOUT: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN | EPOLLOUT;
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ mux_epoll_remove_ਬਾਹਰਲਾ(ਬਣਤਰ mosquitto *context)
{
	ਜੇ(context->events & EPOLLOUT){
		ਬਣਤਰ epoll_event ev;

		memset(&ev, 0, ਆਕਾਰ(ਬਣਤਰ epoll_event));
		ev.ਅੰਕੜੇ.ptr = context;
		ev.events = EPOLLIN;
		ਜੇ(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			ਜੇ((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLIN: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN;
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ mux_epoll__new(ਬਣਤਰ mosquitto *context)
{
	ਬਣਤਰ epoll_event ev;

	memset(&ev, 0, ਆਕਾਰ(ਬਣਤਰ epoll_event));
	ev.events = EPOLLIN;
	ev.ਅੰਕੜੇ.ptr = context;
	ਜੇ(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1){
		ਜੇ(errno != EEXIST){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error in epoll accepting: %s", strerror(errno));
		}
	}
	context->events = EPOLLIN;
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ mux_epoll__delete(ਬਣਤਰ mosquitto *context)
{
	ਜੇ(context->sock != INVALID_SOCKET){
		ਜੇ(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, context->sock, ਨਿਰਾ) == -1){
			ਵਾਪਸ 1;
		}
	}
	ਵਾਪਸ 0;
}


ਪੂਰਨ mux_epoll__handle(ਖਾਲੀ)
{
	ਬਣਤਰ epoll_event ev;
	ਬਣਤਰ mosquitto *context;
	ਬਣਤਰ mosquitto__listener_sock *listensock;
	ਪੂਰਨ event_ਗਿਣਤੀ;

	memset(&ev, 0, ਆਕਾਰ(ਬਣਤਰ epoll_event));
#ਜੇ defined(WITH_WEBSOCKETS)
	event_ਗਿਣਤੀ = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, 100);
#ਨਹੀਂਤਾਂ
	event_ਗਿਣਤੀ = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, db.ਅਗਲਾ_event_ms);
#ਜੇ_ਅੰਤ

	db.now_s = mosquitto_ਸਮਾਂ();
	db.now_real_s = ਸਮਾਂ(ਨਿਰਾ);

	ਚੋਣ(event_ਗਿਣਤੀ){
		ਹਾਲਤ -1:
			ਜੇ(errno != EINTR){
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error in epoll waiting: %s.", strerror(errno));
			}
			ਰੋਕੋ;
		ਹਾਲਤ 0:
			ਰੋਕੋ;
		ਮੂਲ:
			ਲਈ(ਪੂਰਨ i=0; i<event_ਗਿਣਤੀ; i++){
				context = ep_events[i].ਅੰਕੜੇ.ptr;
				ਜੇ(context->ident == id_client){
					loop_handle_reads_writes(context, ep_events[i].events);
				}ਨਹੀਂਤਾਂ ਜੇ(context->ident == id_listener){
					listensock = ep_events[i].ਅੰਕੜੇ.ptr;

					ਜੇ(ep_events[i].events & (EPOLLIN | EPOLLPRI)){
						ਜਦੋਂ((context = net__socket_accept(listensock)) != ਨਿਰਾ){
						}
					}
#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				}ਨਹੀਂਤਾਂ ਜੇ(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#ਜੇ_ਅੰਤ
				}
			}
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ mux_epoll__cleanup(ਖਾਲੀ)
{
	(ਖਾਲੀ)close(db.epollfd);
	db.epollfd = 0;
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਖਾਲੀ loop_handle_reads_writes(ਬਣਤਰ mosquitto *context, uint32_t events)
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
		wspoll.events = (int16_t)context->events;
		wspoll.revents = (int16_t)events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		ਵਾਪਸ;
	}
#ਜੇ_ਅੰਤ

	ਜੇ(events & EPOLLOUT
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

	ਜੇ(events & EPOLLIN
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
	}ਨਹੀਂਤਾਂ{
		ਜੇ(events & (EPOLLERR | EPOLLHUP)){
			do_disconnect(context, MOSQ_ERR_CONN_ਗੁਆਚਾ);
			ਵਾਪਸ;
		}
	}
}
#ਜੇ_ਅੰਤ
