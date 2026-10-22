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

#ifdef WITH_EPOLL

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#ifndef WIN32
#  include <sys/epoll.h>
#  define MAX_EVENTS 1000
#endif

#ضمن <errno.h>
#ضمن <signal.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "mux.h"
#ضمن "packet_mosq.h"
#ضمن "util_mosq.h"

ساكن فراغ loop_handle_reads_writes(بنية mosquitto *context, uint32_t events);

ساكن بنية epoll_event ep_events[MAX_EVENTS];


صحيح mux_epoll__init(فراغ)
{
	memset(&ep_events, 0, حجم(بنية epoll_event)*MAX_EVENTS);

	db.epollfd = 0;
	ܐܢ((db.epollfd = epoll_create(MAX_EVENTS)) == -1){
		log__printf(عدم, MOSQ_LOG_ERR, "Error in epoll creating: %s", strerror(errno));
		ܦܢܐ MOSQ_ERR_UNKNOWN;
	}

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


صحيح mux_epoll__add_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_count)
{
	لكل(صحيح i=0; i<listensock_count; i++){
		بنية epoll_event ev;

		memset(&ev, 0, حجم(بنية epoll_event));
		ev.data.ptr = &listensock[i];
		ev.events = EPOLLIN;
		ܐܢ(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, listensock[i].sock, &ev) == -1){
			log__printf(عدم, MOSQ_LOG_ERR, "Error in epoll initial registering: %s", strerror(errno));
			ܦܢܐ MOSQ_ERR_UNKNOWN;
		}
	}

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


صحيح mux_epoll__delete_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_count)
{
	لكل(صحيح i=0; i<listensock_count; i++){
		ܐܢ(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, listensock[i].sock, عدم) == -1){
			ܦܢܐ MOSQ_ERR_UNKNOWN;
		}
	}

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


صحيح mux_epoll__add_out(بنية mosquitto *context)
{
	ܐܢ(!(context->events & EPOLLOUT)){
		بنية epoll_event ev;

		memset(&ev, 0, حجم(بنية epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN | EPOLLOUT;
		ܐܢ(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			ܐܢ((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(عدم, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLOUT: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN | EPOLLOUT;
	}
	ܦܢܐ MOSQ_ERR_SUCCESS;
}


صحيح mux_epoll__remove_out(بنية mosquitto *context)
{
	ܐܢ(context->events & EPOLLOUT){
		بنية epoll_event ev;

		memset(&ev, 0, حجم(بنية epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN;
		ܐܢ(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			ܐܢ((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(عدم, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLIN: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN;
	}
	ܦܢܐ MOSQ_ERR_SUCCESS;
}


صحيح mux_epoll__new(بنية mosquitto *context)
{
	بنية epoll_event ev;

	memset(&ev, 0, حجم(بنية epoll_event));
	ev.events = EPOLLIN;
	ev.data.ptr = context;
	ܐܢ(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1){
		ܐܢ(errno != EEXIST){
			log__printf(عدم, MOSQ_LOG_ERR, "Error in epoll accepting: %s", strerror(errno));
		}
	}
	context->events = EPOLLIN;
	ܦܢܐ MOSQ_ERR_SUCCESS;
}


صحيح mux_epoll__delete(بنية mosquitto *context)
{
	ܐܢ(context->sock != INVALID_SOCKET){
		ܐܢ(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, context->sock, عدم) == -1){
			ܦܢܐ 1;
		}
	}
	ܦܢܐ 0;
}


صحيح mux_epoll__handle(فراغ)
{
	بنية epoll_event ev;
	بنية mosquitto *context;
	بنية mosquitto__listener_sock *listensock;
	صحيح event_count;

	memset(&ev, 0, حجم(بنية epoll_event));
#ܐܢ defined(WITH_WEBSOCKETS)
	event_count = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, 100);
#ܐܚܪܢܐ
	event_count = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, db.next_event_ms);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(عدم);

	بدل(event_count){
		حالة -1:
			ܐܢ(errno != EINTR){
				log__printf(عدم, MOSQ_LOG_ERR, "Error in epoll waiting: %s.", strerror(errno));
			}
			اكسر;
		حالة 0:
			اكسر;
		افتراضي:
			لكل(صحيح i=0; i<event_count; i++){
				context = ep_events[i].data.ptr;
				ܐܢ(context->ident == id_client){
					loop_handle_reads_writes(context, ep_events[i].events);
				}ܐܚܪܢܐ ܐܢ(context->ident == id_listener){
					listensock = ep_events[i].data.ptr;

					ܐܢ(ep_events[i].events & (EPOLLIN | EPOLLPRI)){
						ܟܡܐ((context = net__socket_accept(listensock)) != عدم){
						}
					}
#ܐܢ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				}ܐܚܪܢܐ ܐܢ(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#endif
				}
			}
	}
	ܦܢܐ MOSQ_ERR_SUCCESS;
}


صحيح mux_epoll__cleanup(فراغ)
{
	(فراغ)close(db.epollfd);
	db.epollfd = 0;
	ܦܢܐ MOSQ_ERR_SUCCESS;
}


ساكن فراغ loop_handle_reads_writes(بنية mosquitto *context, uint32_t events)
{
	صحيح err;
	socklen_t len;
	صحيح rc;

	ܐܢ(context->sock == INVALID_SOCKET){
		ܦܢܐ;
	}

#ܐܢ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ܐܢ(context->wsi){
		بنية lws_pollfd wspoll;
		wspoll.fd = context->sock;
		wspoll.events = (int16_t)context->events;
		wspoll.revents = (int16_t)events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		ܦܢܐ;
	}
#endif

	ܐܢ(events & EPOLLOUT
#ifdef WITH_TLS
			|| context->want_write
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		ܐܢ(context->state == mosq_cs_connect_pending){
			len = حجم(صحيح);
			ܐܢ(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (حرف *)&err, &len)){
				ܐܢ(err == 0){
					mosquitto__set_state(context, mosq_cs_new);
#ܐܢ defined(WITH_ADNS) && defined(WITH_BRIDGE)
					ܐܢ(context->bridge){
						bridge__connect_step3(context);
					}
#endif
				}
			}ܐܚܪܢܐ{
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				ܦܢܐ;
			}
		}
		rc = packet__write(context);
		ܐܢ(rc){
			do_disconnect(context, rc);
			ܦܢܐ;
		}
	}

	ܐܢ(events & EPOLLIN
#ifdef WITH_TLS
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		ܥܒܕ{
			بدل(context->transport){
				حالة mosq_t_tcp:
				حالة mosq_t_ws:
					rc = packet__read(context);
					اكسر;
#ܐܢ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
				حالة mosq_t_http:
					rc = http__read(context);
					اكسر;
#endif
#ܐܢ !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
				/* Not supported with LWS */
				حالة mosq_t_proxy_v2:
					rc = proxy_v2__read(context);
					اكسر;
				حالة mosq_t_proxy_v1:
					rc = proxy_v1__read(context);
					اكسر;
#endif
				افتراضي:
					rc = MOSQ_ERR_INVAL;
					اكسر;
			}
			ܐܢ(rc){
				do_disconnect(context, rc);
				ܦܢܐ;
			}
		}ܟܡܐ(SSL_DATA_PENDING(context));
	}ܐܚܪܢܐ{
		ܐܢ(events & (EPOLLERR | EPOLLHUP)){
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			ܦܢܐ;
		}
	}
}
#endif
