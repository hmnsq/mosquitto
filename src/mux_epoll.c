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

#اگر_تعریف_شدہ WITH_EPOLL

#اگر_غیر_تعریف_شدہ WIN32
#  define GNU_ماخذ
#اگر_ختم

#اگر_غیر_تعریف_شدہ WIN32
#  include <sys/epoll.h>
#  define MAX_EVENTS 1000
#اگر_ختم

#شامل <errno.h>
#شامل <signal.h>

#شامل "mosquitto_broker_internal.h"
#شامل "mux.h"
#شامل "packet_mosq.h"
#شامل "util_mosq.h"

ساکن خالی loop_handle_reads_writes(ساخت mosquitto *context, uint32_t events);

ساکن ساخت epoll_event ep_events[MAX_EVENTS];


صحیح mux_epoll__init(خالی)
{
	memset(&ep_events, 0, حجم(ساخت epoll_event)*MAX_EVENTS);

	db.epollfd = 0;
	اگر((db.epollfd = epoll_create(MAX_EVENTS)) == -1){
		log__printf(تہی, MOSQ_LOG_ERR, "Error in epoll creating: %s", strerror(errno));
		واپس MOSQ_ERR_UNKNOWN;
	}

	واپس MOSQ_ERR_SUCCESS;
}


صحیح mux_epoll_جوڑو_listeners(ساخت mosquitto__listener_sock *listensock, صحیح listensock_گنتی)
{
	برائے(صحیح i=0; i<listensock_گنتی; i++){
		ساخت epoll_event ev;

		memset(&ev, 0, حجم(ساخت epoll_event));
		ev.ڈیٹا.ptr = &listensock[i];
		ev.events = EPOLLIN;
		اگر(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, listensock[i].sock, &ev) == -1){
			log__printf(تہی, MOSQ_LOG_ERR, "Error in epoll initial registering: %s", strerror(errno));
			واپس MOSQ_ERR_UNKNOWN;
		}
	}

	واپس MOSQ_ERR_SUCCESS;
}


صحیح mux_epoll__delete_listeners(ساخت mosquitto__listener_sock *listensock, صحیح listensock_گنتی)
{
	برائے(صحیح i=0; i<listensock_گنتی; i++){
		اگر(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, listensock[i].sock, تہی) == -1){
			واپس MOSQ_ERR_UNKNOWN;
		}
	}

	واپس MOSQ_ERR_SUCCESS;
}


صحیح mux_epoll_جوڑو_برآمد(ساخت mosquitto *context)
{
	اگر(!(context->events & EPOLLOUT)){
		ساخت epoll_event ev;

		memset(&ev, 0, حجم(ساخت epoll_event));
		ev.ڈیٹا.ptr = context;
		ev.events = EPOLLIN | EPOLLOUT;
		اگر(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			اگر((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(تہی, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLOUT: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN | EPOLLOUT;
	}
	واپس MOSQ_ERR_SUCCESS;
}


صحیح mux_epoll_remove_برآمد(ساخت mosquitto *context)
{
	اگر(context->events & EPOLLOUT){
		ساخت epoll_event ev;

		memset(&ev, 0, حجم(ساخت epoll_event));
		ev.ڈیٹا.ptr = context;
		ev.events = EPOLLIN;
		اگر(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			اگر((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(تہی, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLIN: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN;
	}
	واپس MOSQ_ERR_SUCCESS;
}


صحیح mux_epoll__new(ساخت mosquitto *context)
{
	ساخت epoll_event ev;

	memset(&ev, 0, حجم(ساخت epoll_event));
	ev.events = EPOLLIN;
	ev.ڈیٹا.ptr = context;
	اگر(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1){
		اگر(errno != EEXIST){
			log__printf(تہی, MOSQ_LOG_ERR, "Error in epoll accepting: %s", strerror(errno));
		}
	}
	context->events = EPOLLIN;
	واپس MOSQ_ERR_SUCCESS;
}


صحیح mux_epoll__delete(ساخت mosquitto *context)
{
	اگر(context->sock != INVALID_SOCKET){
		اگر(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, context->sock, تہی) == -1){
			واپس 1;
		}
	}
	واپس 0;
}


صحیح mux_epoll__handle(خالی)
{
	ساخت epoll_event ev;
	ساخت mosquitto *context;
	ساخت mosquitto__listener_sock *listensock;
	صحیح event_گنتی;

	memset(&ev, 0, حجم(ساخت epoll_event));
#اگر defined(WITH_WEBSOCKETS)
	event_گنتی = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, 100);
#ورنہ
	event_گنتی = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, db.اگلا_event_ms);
#اگر_ختم

	db.now_s = mosquitto_وقت();
	db.now_real_s = وقت(تہی);

	انتخاب(event_گنتی){
		حالت -1:
			اگر(errno != EINTR){
				log__printf(تہی, MOSQ_LOG_ERR, "Error in epoll waiting: %s.", strerror(errno));
			}
			توڑو;
		حالت 0:
			توڑو;
		طےشدہ:
			برائے(صحیح i=0; i<event_گنتی; i++){
				context = ep_events[i].ڈیٹا.ptr;
				اگر(context->ident == id_client){
					loop_handle_reads_writes(context, ep_events[i].events);
				}ورنہ اگر(context->ident == id_listener){
					listensock = ep_events[i].ڈیٹا.ptr;

					اگر(ep_events[i].events & (EPOLLIN | EPOLLPRI)){
						جب((context = net__socket_accept(listensock)) != تہی){
						}
					}
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				}ورنہ اگر(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#اگر_ختم
				}
			}
	}
	واپس MOSQ_ERR_SUCCESS;
}


صحیح mux_epoll__cleanup(خالی)
{
	(خالی)close(db.epollfd);
	db.epollfd = 0;
	واپس MOSQ_ERR_SUCCESS;
}


ساکن خالی loop_handle_reads_writes(ساخت mosquitto *context, uint32_t events)
{
	صحیح err;
	socklen_t len;
	صحیح rc;

	اگر(context->sock == INVALID_SOCKET){
		واپس;
	}

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	اگر(context->wsi){
		ساخت lws_pollfd wspoll;
		wspoll.fd = context->sock;
		wspoll.events = (int16_t)context->events;
		wspoll.revents = (int16_t)events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		واپس;
	}
#اگر_ختم

	اگر(events & EPOLLOUT
#اگر_تعریف_شدہ WITH_TLS
			|| context->want_write
			|| (context->ssl && context->state == mosq_cs_new)
#اگر_ختم
			){

		اگر(context->state == mosq_cs_connect_pending){
			len = حجم(صحیح);
			اگر(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (حرف *)&err, &len)){
				اگر(err == 0){
					mosquitto_سیٹ_state(context, mosq_cs_new);
#اگر defined(WITH_ADNS) && defined(WITH_BRIDGE)
					اگر(context->bridge){
						bridge__connect_step3(context);
					}
#اگر_ختم
				}
			}ورنہ{
				do_disconnect(context, MOSQ_ERR_CONN_گمشدہ);
				واپس;
			}
		}
		rc = packet__write(context);
		اگر(rc){
			do_disconnect(context, rc);
			واپس;
		}
	}

	اگر(events & EPOLLIN
#اگر_تعریف_شدہ WITH_TLS
			|| (context->ssl && context->state == mosq_cs_new)
#اگر_ختم
			){

		کرو{
			انتخاب(context->transport){
				حالت mosq_t_tcp:
				حالت mosq_t_ws:
					rc = packet__read(context);
					توڑو;
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
				حالت mosq_t_http:
					rc = http__read(context);
					توڑو;
#اگر_ختم
#اگر !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
				/* Not supported with LWS */
				حالت mosq_t_proxy_v2:
					rc = proxy_v2__read(context);
					توڑو;
				حالت mosq_t_proxy_v1:
					rc = proxy_v1__read(context);
					توڑو;
#اگر_ختم
				طےشدہ:
					rc = MOSQ_ERR_INVAL;
					توڑو;
			}
			اگر(rc){
				do_disconnect(context, rc);
				واپس;
			}
		}جب(SSL_ڈیٹا_PENDING(context));
	}ورنہ{
		اگر(events & (EPOLLERR | EPOLLHUP)){
			do_disconnect(context, MOSQ_ERR_CONN_گمشدہ);
			واپس;
		}
	}
}
#اگر_ختم
