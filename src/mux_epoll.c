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
	إذا((db.epollfd = epoll_create(MAX_EVENTS)) == -1){
		log__printf(عدم, MOSQ_LOG_ERR, "Error in epoll creating: %s", strerror(errno));
		أرجع MOSQ_ERR_UNKNOWN;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح mux_epoll__add_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_گنتی)
{
	لكل(صحيح i=0; i<listensock_گنتی; i++){
		بنية epoll_event ev;

		memset(&ev, 0, حجم(بنية epoll_event));
		ev.data.ptr = &listensock[i];
		ev.events = EPOLLIN;
		إذا(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, listensock[i].sock, &ev) == -1){
			log__printf(عدم, MOSQ_LOG_ERR, "Error in epoll initial registering: %s", strerror(errno));
			أرجع MOSQ_ERR_UNKNOWN;
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح mux_epoll__delete_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_گنتی)
{
	لكل(صحيح i=0; i<listensock_گنتی; i++){
		إذا(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, listensock[i].sock, عدم) == -1){
			أرجع MOSQ_ERR_UNKNOWN;
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح mux_epoll__add_out(بنية mosquitto *context)
{
	إذا(!(context->events & EPOLLOUT)){
		بنية epoll_event ev;

		memset(&ev, 0, حجم(بنية epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN | EPOLLOUT;
		إذا(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			إذا((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(عدم, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLOUT: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN | EPOLLOUT;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح mux_epoll__remove_out(بنية mosquitto *context)
{
	إذا(context->events & EPOLLOUT){
		بنية epoll_event ev;

		memset(&ev, 0, حجم(بنية epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN;
		إذا(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			إذا((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(عدم, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLIN: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح mux_epoll__new(بنية mosquitto *context)
{
	بنية epoll_event ev;

	memset(&ev, 0, حجم(بنية epoll_event));
	ev.events = EPOLLIN;
	ev.data.ptr = context;
	إذا(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1){
		إذا(errno != EEXIST){
			log__printf(عدم, MOSQ_LOG_ERR, "Error in epoll accepting: %s", strerror(errno));
		}
	}
	context->events = EPOLLIN;
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح mux_epoll__delete(بنية mosquitto *context)
{
	إذا(context->sock != INVALID_SOCKET){
		إذا(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, context->sock, عدم) == -1){
			أرجع 1;
		}
	}
	أرجع 0;
}


صحيح mux_epoll__handle(فراغ)
{
	بنية epoll_event ev;
	بنية mosquitto *context;
	بنية mosquitto__listener_sock *listensock;
	صحيح event_گنتی;

	memset(&ev, 0, حجم(بنية epoll_event));
#إذا defined(WITH_WEBSOCKETS)
	event_گنتی = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, 100);
#وإلا
	event_گنتی = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, db.next_event_ms);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(عدم);

	بدل(event_گنتی){
		حالة -1:
			إذا(errno != EINTR){
				log__printf(عدم, MOSQ_LOG_ERR, "Error in epoll waiting: %s.", strerror(errno));
			}
			اكسر;
		حالة 0:
			اكسر;
		افتراضي:
			لكل(صحيح i=0; i<event_گنتی; i++){
				context = ep_events[i].data.ptr;
				إذا(context->ident == id_client){
					loop_handle_reads_writes(context, ep_events[i].events);
				}وإلا إذا(context->ident == id_listener){
					listensock = ep_events[i].data.ptr;

					إذا(ep_events[i].events & (EPOLLIN | EPOLLPRI)){
						طالما((context = net__socket_accept(listensock)) != عدم){
						}
					}
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				}وإلا إذا(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#endif
				}
			}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح mux_epoll__cleanup(فراغ)
{
	(فراغ)close(db.epollfd);
	db.epollfd = 0;
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن فراغ loop_handle_reads_writes(بنية mosquitto *context, uint32_t events)
{
	صحيح err;
	socklen_t len;
	صحيح rc;

	إذا(context->sock == INVALID_SOCKET){
		أرجع;
	}

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	إذا(context->wsi){
		بنية lws_pollfd wspoll;
		wspoll.fd = context->sock;
		wspoll.events = (int16_t)context->events;
		wspoll.revents = (int16_t)events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		أرجع;
	}
#endif

	إذا(events & EPOLLOUT
#ifdef WITH_TLS
			|| context->want_write
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		إذا(context->state == mosq_cs_connect_pending){
			len = حجم(صحيح);
			إذا(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (حرف *)&err, &len)){
				إذا(err == 0){
					mosquitto__set_state(context, mosq_cs_new);
#إذا defined(WITH_ADNS) && defined(WITH_BRIDGE)
					إذا(context->bridge){
						bridge__connect_step3(context);
					}
#endif
				}
			}وإلا{
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				أرجع;
			}
		}
		rc = packet__write(context);
		إذا(rc){
			do_disconnect(context, rc);
			أرجع;
		}
	}

	إذا(events & EPOLLIN
#ifdef WITH_TLS
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		افعل{
			بدل(context->transport){
				حالة mosq_t_tcp:
				حالة mosq_t_ws:
					rc = packet__read(context);
					اكسر;
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
				حالة mosq_t_http:
					rc = http__read(context);
					اكسر;
#endif
#إذا !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
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
			إذا(rc){
				do_disconnect(context, rc);
				أرجع;
			}
		}طالما(SSL_DATA_PENDING(context));
	}وإلا{
		إذا(events & (EPOLLERR | EPOLLHUP)){
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			أرجع;
		}
	}
}
#endif
