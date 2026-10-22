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

#ifdef WITH_EPOLL

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#ifndef WIN32
#  include <sys/epoll.h>
#  define MAX_EVENTS 1000
#endif

#شامل <errno.h>
#شامل <signal.h>

#شامل "mosquitto_broker_internal.h"
#شامل "mux.h"
#شامل "packet_mosq.h"
#شامل "util_mosq.h"

ایستا تهی loop_handle_reads_writes(ساختار mosquitto *context, uint32_t events);

ایستا ساختار epoll_event ep_events[MAX_EVENTS];


صحیح mux_epoll__init(تهی)
{
	memset(&ep_events, 0, اندازه(ساختار epoll_event)*MAX_EVENTS);

	db.epollfd = 0;
	اگر((db.epollfd = epoll_create(MAX_EVENTS)) == -1){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error in epoll creating: %s", strerror(errno));
		بازگشت MOSQ_ERR_UNKNOWN;
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح mux_epoll__add_listeners(ساختار mosquitto__listener_sock *listensock, صحیح listensock_شمار)
{
	برای(صحیح i=0; i<listensock_شمار; i++){
		ساختار epoll_event ev;

		memset(&ev, 0, اندازه(ساختار epoll_event));
		ev.data.ptr = &listensock[i];
		ev.events = EPOLLIN;
		اگر(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, listensock[i].sock, &ev) == -1){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error in epoll initial registering: %s", strerror(errno));
			بازگشت MOSQ_ERR_UNKNOWN;
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح mux_epoll__delete_listeners(ساختار mosquitto__listener_sock *listensock, صحیح listensock_شمار)
{
	برای(صحیح i=0; i<listensock_شمار; i++){
		اگر(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, listensock[i].sock, پوچ) == -1){
			بازگشت MOSQ_ERR_UNKNOWN;
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح mux_epoll__add_out(ساختار mosquitto *context)
{
	اگر(!(context->events & EPOLLOUT)){
		ساختار epoll_event ev;

		memset(&ev, 0, اندازه(ساختار epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN | EPOLLOUT;
		اگر(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			اگر((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(پوچ, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLOUT: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN | EPOLLOUT;
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح mux_epoll__remove_out(ساختار mosquitto *context)
{
	اگر(context->events & EPOLLOUT){
		ساختار epoll_event ev;

		memset(&ev, 0, اندازه(ساختار epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN;
		اگر(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			اگر((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(پوچ, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLIN: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN;
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح mux_epoll__new(ساختار mosquitto *context)
{
	ساختار epoll_event ev;

	memset(&ev, 0, اندازه(ساختار epoll_event));
	ev.events = EPOLLIN;
	ev.data.ptr = context;
	اگر(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1){
		اگر(errno != EEXIST){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error in epoll accepting: %s", strerror(errno));
		}
	}
	context->events = EPOLLIN;
	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح mux_epoll__delete(ساختار mosquitto *context)
{
	اگر(context->sock != INVALID_SOCKET){
		اگر(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, context->sock, پوچ) == -1){
			بازگشت 1;
		}
	}
	بازگشت 0;
}


صحیح mux_epoll__handle(تهی)
{
	ساختار epoll_event ev;
	ساختار mosquitto *context;
	ساختار mosquitto__listener_sock *listensock;
	صحیح event_شمار;

	memset(&ev, 0, اندازه(ساختار epoll_event));
#اگر defined(WITH_WEBSOCKETS)
	event_شمار = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, 100);
#وگرنه
	event_شمار = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, db.next_event_ms);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(پوچ);

	انتخاب(event_شمار){
		حالت -1:
			اگر(errno != EINTR){
				log__printf(پوچ, MOSQ_LOG_ERR, "Error in epoll waiting: %s.", strerror(errno));
			}
			بشکن;
		حالت 0:
			بشکن;
		پیشفرض:
			برای(صحیح i=0; i<event_شمار; i++){
				context = ep_events[i].data.ptr;
				اگر(context->ident == id_client){
					loop_handle_reads_writes(context, ep_events[i].events);
				}وگرنه اگر(context->ident == id_listener){
					listensock = ep_events[i].data.ptr;

					اگر(ep_events[i].events & (EPOLLIN | EPOLLPRI)){
						وقتی((context = net__socket_accept(listensock)) != پوچ){
						}
					}
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				}وگرنه اگر(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#endif
				}
			}
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح mux_epoll__cleanup(تهی)
{
	(تهی)close(db.epollfd);
	db.epollfd = 0;
	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا تهی loop_handle_reads_writes(ساختار mosquitto *context, uint32_t events)
{
	صحیح err;
	socklen_t len;
	صحیح rc;

	اگر(context->sock == INVALID_SOCKET){
		بازگشت;
	}

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	اگر(context->wsi){
		ساختار lws_pollfd wspoll;
		wspoll.fd = context->sock;
		wspoll.events = (int16_t)context->events;
		wspoll.revents = (int16_t)events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		بازگشت;
	}
#endif

	اگر(events & EPOLLOUT
#ifdef WITH_TLS
			|| context->want_write
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		اگر(context->state == mosq_cs_connect_pending){
			len = اندازه(صحیح);
			اگر(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (نویسه *)&err, &len)){
				اگر(err == 0){
					mosquitto__set_state(context, mosq_cs_new);
#اگر defined(WITH_ADNS) && defined(WITH_BRIDGE)
					اگر(context->bridge){
						bridge__connect_step3(context);
					}
#endif
				}
			}وگرنه{
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				بازگشت;
			}
		}
		rc = packet__write(context);
		اگر(rc){
			do_disconnect(context, rc);
			بازگشت;
		}
	}

	اگر(events & EPOLLIN
#ifdef WITH_TLS
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		انجامبده{
			انتخاب(context->transport){
				حالت mosq_t_tcp:
				حالت mosq_t_ws:
					rc = packet__read(context);
					بشکن;
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
				حالت mosq_t_http:
					rc = http__read(context);
					بشکن;
#endif
#اگر !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
				/* Not supported with LWS */
				حالت mosq_t_proxy_v2:
					rc = proxy_v2__read(context);
					بشکن;
				حالت mosq_t_proxy_v1:
					rc = proxy_v1__read(context);
					بشکن;
#endif
				پیشفرض:
					rc = MOSQ_ERR_INVAL;
					بشکن;
			}
			اگر(rc){
				do_disconnect(context, rc);
				بازگشت;
			}
		}وقتی(SSL_DATA_PENDING(context));
	}وگرنه{
		اگر(events & (EPOLLERR | EPOLLHUP)){
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			بازگشت;
		}
	}
}
#endif
