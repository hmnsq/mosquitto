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

#ifdef WITH_EPOLL

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#ifndef WIN32
#  include <sys/epoll.h>
#  define MAX_EVENTS 1000
#endif

#অন্তর্ভাব <errno.h>
#অন্তর্ভাব <signal.h>

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mux.h"
#অন্তর্ভাব "packet_mosq.h"
#অন্তর্ভাব "util_mosq.h"

স্থির শূন্য loop_handle_reads_writes(কাঠামো mosquitto *context, uint32_t events);

স্থির কাঠামো epoll_event ep_events[MAX_EVENTS];


পূর্ণসংখ্যা mux_epoll__init(শূন্য)
{
	memset(&ep_events, 0, আকার(কাঠামো epoll_event)*MAX_EVENTS);

	db.epollfd = 0;
	যদি((db.epollfd = epoll_create(MAX_EVENTS)) == -1){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error in epoll creating: %s", strerror(errno));
		ফেরত MOSQ_ERR_UNKNOWN;
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা mux_epoll__add_listeners(কাঠামো mosquitto__listener_sock *listensock, পূর্ণসংখ্যা listensock_গুনতি)
{
	জন্য(পূর্ণসংখ্যা i=0; i<listensock_গুনতি; i++){
		কাঠামো epoll_event ev;

		memset(&ev, 0, আকার(কাঠামো epoll_event));
		ev.তথ্য.ptr = &listensock[i];
		ev.events = EPOLLIN;
		যদি(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, listensock[i].sock, &ev) == -1){
			log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error in epoll initial registering: %s", strerror(errno));
			ফেরত MOSQ_ERR_UNKNOWN;
		}
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা mux_epoll__delete_listeners(কাঠামো mosquitto__listener_sock *listensock, পূর্ণসংখ্যা listensock_গুনতি)
{
	জন্য(পূর্ণসংখ্যা i=0; i<listensock_গুনতি; i++){
		যদি(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, listensock[i].sock, ফাঁকা) == -1){
			ফেরত MOSQ_ERR_UNKNOWN;
		}
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা mux_epoll__add_out(কাঠামো mosquitto *context)
{
	যদি(!(context->events & EPOLLOUT)){
		কাঠামো epoll_event ev;

		memset(&ev, 0, আকার(কাঠামো epoll_event));
		ev.তথ্য.ptr = context;
		ev.events = EPOLLIN | EPOLLOUT;
		যদি(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			যদি((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(ফাঁকা, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLOUT: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN | EPOLLOUT;
	}
	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা mux_epoll__remove_out(কাঠামো mosquitto *context)
{
	যদি(context->events & EPOLLOUT){
		কাঠামো epoll_event ev;

		memset(&ev, 0, আকার(কাঠামো epoll_event));
		ev.তথ্য.ptr = context;
		ev.events = EPOLLIN;
		যদি(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			যদি((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(ফাঁকা, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLIN: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN;
	}
	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা mux_epoll__new(কাঠামো mosquitto *context)
{
	কাঠামো epoll_event ev;

	memset(&ev, 0, আকার(কাঠামো epoll_event));
	ev.events = EPOLLIN;
	ev.তথ্য.ptr = context;
	যদি(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1){
		যদি(errno != EEXIST){
			log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error in epoll accepting: %s", strerror(errno));
		}
	}
	context->events = EPOLLIN;
	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা mux_epoll__delete(কাঠামো mosquitto *context)
{
	যদি(context->sock != INVALID_SOCKET){
		যদি(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, context->sock, ফাঁকা) == -1){
			ফেরত 1;
		}
	}
	ফেরত 0;
}


পূর্ণসংখ্যা mux_epoll__handle(শূন্য)
{
	কাঠামো epoll_event ev;
	কাঠামো mosquitto *context;
	কাঠামো mosquitto__listener_sock *listensock;
	পূর্ণসংখ্যা event_গুনতি;

	memset(&ev, 0, আকার(কাঠামো epoll_event));
#যদি defined(WITH_WEBSOCKETS)
	event_গুনতি = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, 100);
#অন্যথা
	event_গুনতি = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, db.পরবর্তী_event_ms);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(ফাঁকা);

	নির্বাচন(event_গুনতি){
		ক্ষেত্র -1:
			যদি(errno != EINTR){
				log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error in epoll waiting: %s.", strerror(errno));
			}
			বিরতি;
		ক্ষেত্র 0:
			বিরতি;
		পূর্বনির্ধারিত:
			জন্য(পূর্ণসংখ্যা i=0; i<event_গুনতি; i++){
				context = ep_events[i].তথ্য.ptr;
				যদি(context->ident == id_client){
					loop_handle_reads_writes(context, ep_events[i].events);
				}অন্যথা যদি(context->ident == id_listener){
					listensock = ep_events[i].তথ্য.ptr;

					যদি(ep_events[i].events & (EPOLLIN | EPOLLPRI)){
						যখন((context = net__socket_accept(listensock)) != ফাঁকা){
						}
					}
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				}অন্যথা যদি(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#endif
				}
			}
	}
	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা mux_epoll__cleanup(শূন্য)
{
	(শূন্য)close(db.epollfd);
	db.epollfd = 0;
	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির শূন্য loop_handle_reads_writes(কাঠামো mosquitto *context, uint32_t events)
{
	পূর্ণসংখ্যা err;
	socklen_t len;
	পূর্ণসংখ্যা rc;

	যদি(context->sock == INVALID_SOCKET){
		ফেরত;
	}

#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	যদি(context->wsi){
		কাঠামো lws_pollfd wspoll;
		wspoll.fd = context->sock;
		wspoll.events = (int16_t)context->events;
		wspoll.revents = (int16_t)events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		ফেরত;
	}
#endif

	যদি(events & EPOLLOUT
#ifdef WITH_TLS
			|| context->want_write
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		যদি(context->state == mosq_cs_connect_pending){
			len = আকার(পূর্ণসংখ্যা);
			যদি(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (অক্ষর *)&err, &len)){
				যদি(err == 0){
					mosquitto__set_state(context, mosq_cs_new);
#যদি defined(WITH_ADNS) && defined(WITH_BRIDGE)
					যদি(context->bridge){
						bridge__connect_step3(context);
					}
#endif
				}
			}অন্যথা{
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				ফেরত;
			}
		}
		rc = packet__write(context);
		যদি(rc){
			do_disconnect(context, rc);
			ফেরত;
		}
	}

	যদি(events & EPOLLIN
#ifdef WITH_TLS
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		করো{
			নির্বাচন(context->transport){
				ক্ষেত্র mosq_t_tcp:
				ক্ষেত্র mosq_t_ws:
					rc = packet__read(context);
					বিরতি;
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
				ক্ষেত্র mosq_t_http:
					rc = http__read(context);
					বিরতি;
#endif
#যদি !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
				/* Not supported with LWS */
				ক্ষেত্র mosq_t_proxy_v2:
					rc = proxy_v2__read(context);
					বিরতি;
				ক্ষেত্র mosq_t_proxy_v1:
					rc = proxy_v1__read(context);
					বিরতি;
#endif
				পূর্বনির্ধারিত:
					rc = MOSQ_ERR_INVAL;
					বিরতি;
			}
			যদি(rc){
				do_disconnect(context, rc);
				ফেরত;
			}
		}যখন(SSL_তথ্য_PENDING(context));
	}অন্যথা{
		যদি(events & (EPOLLERR | EPOLLHUP)){
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			ফেরত;
		}
	}
}
#endif
