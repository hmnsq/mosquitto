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

স্থির শূন্য loop_handle_reads_writes(সংরচনা mosquitto *context, uint32_t events);

স্থির সংরচনা epoll_event ep_events[MAX_EVENTS];


পূর্ণাঙ্ক mux_epoll__init(শূন্য)
{
	memset(&ep_events, 0, পরিমাণ(সংরচনা epoll_event)*MAX_EVENTS);

	db.epollfd = 0;
	যদি((db.epollfd = epoll_create(MAX_EVENTS)) == -1){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error in epoll creating: %s", strerror(errno));
		প্রত্যাগম MOSQ_ERR_UNKNOWN;
	}

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক mux_epoll__add_listeners(সংরচনা mosquitto__listener_sock *listensock, পূর্ণাঙ্ক listensock_গুনতি)
{
	প্রতি(পূর্ণাঙ্ক i=0; i<listensock_গুনতি; i++){
		সংরচনা epoll_event ev;

		memset(&ev, 0, পরিমাণ(সংরচনা epoll_event));
		ev.data.ptr = &listensock[i];
		ev.events = EPOLLIN;
		যদি(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, listensock[i].sock, &ev) == -1){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error in epoll initial registering: %s", strerror(errno));
			প্রত্যাগম MOSQ_ERR_UNKNOWN;
		}
	}

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক mux_epoll__delete_listeners(সংরচনা mosquitto__listener_sock *listensock, পূর্ণাঙ্ক listensock_গুনতি)
{
	প্রতি(পূর্ণাঙ্ক i=0; i<listensock_গুনতি; i++){
		যদি(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, listensock[i].sock, রিক্ত) == -1){
			প্রত্যাগম MOSQ_ERR_UNKNOWN;
		}
	}

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক mux_epoll__add_out(সংরচনা mosquitto *context)
{
	যদি(!(context->events & EPOLLOUT)){
		সংরচনা epoll_event ev;

		memset(&ev, 0, পরিমাণ(সংরচনা epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN | EPOLLOUT;
		যদি(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			যদি((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(রিক্ত, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLOUT: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN | EPOLLOUT;
	}
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক mux_epoll__remove_out(সংরচনা mosquitto *context)
{
	যদি(context->events & EPOLLOUT){
		সংরচনা epoll_event ev;

		memset(&ev, 0, পরিমাণ(সংরচনা epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN;
		যদি(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			যদি((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(রিক্ত, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLIN: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN;
	}
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক mux_epoll__new(সংরচনা mosquitto *context)
{
	সংরচনা epoll_event ev;

	memset(&ev, 0, পরিমাণ(সংরচনা epoll_event));
	ev.events = EPOLLIN;
	ev.data.ptr = context;
	যদি(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1){
		যদি(errno != EEXIST){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error in epoll accepting: %s", strerror(errno));
		}
	}
	context->events = EPOLLIN;
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক mux_epoll__delete(সংরচনা mosquitto *context)
{
	যদি(context->sock != INVALID_SOCKET){
		যদি(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, context->sock, রিক্ত) == -1){
			প্রত্যাগম 1;
		}
	}
	প্রত্যাগম 0;
}


পূর্ণাঙ্ক mux_epoll__handle(শূন্য)
{
	সংরচনা epoll_event ev;
	সংরচনা mosquitto *context;
	সংরচনা mosquitto__listener_sock *listensock;
	পূর্ণাঙ্ক event_গুনতি;

	memset(&ev, 0, পরিমাণ(সংরচনা epoll_event));
#যদি defined(WITH_WEBSOCKETS)
	event_গুনতি = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, 100);
#অন্যথা
	event_গুনতি = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, db.next_event_ms);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(রিক্ত);

	বিকল্প(event_গুনতি){
		পক্ষ -1:
			যদি(errno != EINTR){
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error in epoll waiting: %s.", strerror(errno));
			}
			ভঙ্গ;
		পক্ষ 0:
			ভঙ্গ;
		স্বভাব:
			প্রতি(পূর্ণাঙ্ক i=0; i<event_গুনতি; i++){
				context = ep_events[i].data.ptr;
				যদি(context->ident == id_client){
					loop_handle_reads_writes(context, ep_events[i].events);
				}অন্যথা যদি(context->ident == id_listener){
					listensock = ep_events[i].data.ptr;

					যদি(ep_events[i].events & (EPOLLIN | EPOLLPRI)){
						যাবত্((context = net__socket_accept(listensock)) != রিক্ত){
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
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক mux_epoll__cleanup(শূন্য)
{
	(শূন্য)close(db.epollfd);
	db.epollfd = 0;
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


স্থির শূন্য loop_handle_reads_writes(সংরচনা mosquitto *context, uint32_t events)
{
	পূর্ণাঙ্ক err;
	socklen_t len;
	পূর্ণাঙ্ক rc;

	যদি(context->sock == INVALID_SOCKET){
		প্রত্যাগম;
	}

#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	যদি(context->wsi){
		সংরচনা lws_pollfd wspoll;
		wspoll.fd = context->sock;
		wspoll.events = (int16_t)context->events;
		wspoll.revents = (int16_t)events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		প্রত্যাগম;
	}
#endif

	যদি(events & EPOLLOUT
#ifdef WITH_TLS
			|| context->want_write
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		যদি(context->state == mosq_cs_connect_pending){
			len = পরিমাণ(পূর্ণাঙ্ক);
			যদি(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (বর্ণ *)&err, &len)){
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
				প্রত্যাগম;
			}
		}
		rc = packet__write(context);
		যদি(rc){
			do_disconnect(context, rc);
			প্রত্যাগম;
		}
	}

	যদি(events & EPOLLIN
#ifdef WITH_TLS
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		কুরু{
			বিকল্প(context->transport){
				পক্ষ mosq_t_tcp:
				পক্ষ mosq_t_ws:
					rc = packet__read(context);
					ভঙ্গ;
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
				পক্ষ mosq_t_http:
					rc = http__read(context);
					ভঙ্গ;
#endif
#যদি !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
				/* Not supported with LWS */
				পক্ষ mosq_t_proxy_v2:
					rc = proxy_v2__read(context);
					ভঙ্গ;
				পক্ষ mosq_t_proxy_v1:
					rc = proxy_v1__read(context);
					ভঙ্গ;
#endif
				স্বভাব:
					rc = MOSQ_ERR_INVAL;
					ভঙ্গ;
			}
			যদি(rc){
				do_disconnect(context, rc);
				প্রত্যাগম;
			}
		}যাবত্(SSL_DATA_PENDING(context));
	}অন্যথা{
		যদি(events & (EPOLLERR | EPOLLHUP)){
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			প্রত্যাগম;
		}
	}
}
#endif
