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

#include "config.h"

#ifdef WITH_EPOLL

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#ifndef WIN32
#  include <sys/epoll.h>
#  define MAX_EVENTS 1000
#endif

#include <errno.h>
#include <signal.h>

#include "mosquitto_broker_internal.h"
#include "mux.h"
#include "packet_mosq.h"
#include "util_mosq.h"

static void loop_handle_reads_writes(struct mosquitto *context, uint32_t events);

static struct epoll_event ep_events[MAX_EVENTS];


정수 mux_epoll__init(void)
{
	memset(&ep_events, 0, sizeof(struct epoll_event)*MAX_EVENTS);

	db.epollfd = 0;
	만약((db.epollfd = epoll_create(MAX_EVENTS)) == -1){
		log__printf(NULL, MOSQ_LOG_ERR, "Error in epoll creating: %s", strerror(errno));
		반환 MOSQ_ERR_UNKNOWN;
	}

	반환 MOSQ_ERR_SUCCESS;
}


정수 mux_epoll__add_listeners(struct mosquitto__listener_sock *listensock, 정수 listensock_count)
{
	위해(정수 i=0; i<listensock_count; i++){
		struct epoll_event ev;

		memset(&ev, 0, sizeof(struct epoll_event));
		ev.data.ptr = &listensock[i];
		ev.events = EPOLLIN;
		만약(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, listensock[i].sock, &ev) == -1){
			log__printf(NULL, MOSQ_LOG_ERR, "Error in epoll initial registering: %s", strerror(errno));
			반환 MOSQ_ERR_UNKNOWN;
		}
	}

	반환 MOSQ_ERR_SUCCESS;
}


정수 mux_epoll__delete_listeners(struct mosquitto__listener_sock *listensock, 정수 listensock_count)
{
	위해(정수 i=0; i<listensock_count; i++){
		만약(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, listensock[i].sock, NULL) == -1){
			반환 MOSQ_ERR_UNKNOWN;
		}
	}

	반환 MOSQ_ERR_SUCCESS;
}


정수 mux_epoll__add_out(struct mosquitto *context)
{
	만약(!(context->events & EPOLLOUT)){
		struct epoll_event ev;

		memset(&ev, 0, sizeof(struct epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN | EPOLLOUT;
		만약(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			만약((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(NULL, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLOUT: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN | EPOLLOUT;
	}
	반환 MOSQ_ERR_SUCCESS;
}


정수 mux_epoll__remove_out(struct mosquitto *context)
{
	만약(context->events & EPOLLOUT){
		struct epoll_event ev;

		memset(&ev, 0, sizeof(struct epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN;
		만약(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			만약((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(NULL, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLIN: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN;
	}
	반환 MOSQ_ERR_SUCCESS;
}


정수 mux_epoll__new(struct mosquitto *context)
{
	struct epoll_event ev;

	memset(&ev, 0, sizeof(struct epoll_event));
	ev.events = EPOLLIN;
	ev.data.ptr = context;
	만약(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1){
		만약(errno != EEXIST){
			log__printf(NULL, MOSQ_LOG_ERR, "Error in epoll accepting: %s", strerror(errno));
		}
	}
	context->events = EPOLLIN;
	반환 MOSQ_ERR_SUCCESS;
}


정수 mux_epoll__delete(struct mosquitto *context)
{
	만약(context->sock != INVALID_SOCKET){
		만약(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, context->sock, NULL) == -1){
			반환 1;
		}
	}
	반환 0;
}


정수 mux_epoll__handle(void)
{
	struct epoll_event ev;
	struct mosquitto *context;
	struct mosquitto__listener_sock *listensock;
	정수 event_count;

	memset(&ev, 0, sizeof(struct epoll_event));
#만약 defined(WITH_WEBSOCKETS)
	event_count = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, 100);
#아니면
	event_count = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, db.next_event_ms);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(NULL);

	switch(event_count){
		case -1:
			만약(errno != EINTR){
				log__printf(NULL, MOSQ_LOG_ERR, "Error in epoll waiting: %s.", strerror(errno));
			}
			break;
		case 0:
			break;
		default:
			위해(정수 i=0; i<event_count; i++){
				context = ep_events[i].data.ptr;
				만약(context->ident == id_client){
					loop_handle_reads_writes(context, ep_events[i].events);
				}아니면 만약(context->ident == id_listener){
					listensock = ep_events[i].data.ptr;

					만약(ep_events[i].events & (EPOLLIN | EPOLLPRI)){
						동안((context = net__socket_accept(listensock)) != NULL){
						}
					}
#만약 defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				}아니면 만약(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#endif
				}
			}
	}
	반환 MOSQ_ERR_SUCCESS;
}


정수 mux_epoll__cleanup(void)
{
	(void)close(db.epollfd);
	db.epollfd = 0;
	반환 MOSQ_ERR_SUCCESS;
}


static void loop_handle_reads_writes(struct mosquitto *context, uint32_t events)
{
	정수 err;
	socklen_t len;
	정수 rc;

	만약(context->sock == INVALID_SOCKET){
		반환;
	}

#만약 defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	만약(context->wsi){
		struct lws_pollfd wspoll;
		wspoll.fd = context->sock;
		wspoll.events = (int16_t)context->events;
		wspoll.revents = (int16_t)events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		반환;
	}
#endif

	만약(events & EPOLLOUT
#ifdef WITH_TLS
			|| context->want_write
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		만약(context->state == mosq_cs_connect_pending){
			len = sizeof(정수);
			만약(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (char *)&err, &len)){
				만약(err == 0){
					mosquitto__set_state(context, mosq_cs_new);
#만약 defined(WITH_ADNS) && defined(WITH_BRIDGE)
					만약(context->bridge){
						bridge__connect_step3(context);
					}
#endif
				}
			}아니면{
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				반환;
			}
		}
		rc = packet__write(context);
		만약(rc){
			do_disconnect(context, rc);
			반환;
		}
	}

	만약(events & EPOLLIN
#ifdef WITH_TLS
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		do{
			switch(context->transport){
				case mosq_t_tcp:
				case mosq_t_ws:
					rc = packet__read(context);
					break;
#만약 defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
				case mosq_t_http:
					rc = http__read(context);
					break;
#endif
#만약 !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
				/* Not supported with LWS */
				case mosq_t_proxy_v2:
					rc = proxy_v2__read(context);
					break;
				case mosq_t_proxy_v1:
					rc = proxy_v1__read(context);
					break;
#endif
				default:
					rc = MOSQ_ERR_INVAL;
					break;
			}
			만약(rc){
				do_disconnect(context, rc);
				반환;
			}
		}동안(SSL_DATA_PENDING(context));
	}아니면{
		만약(events & (EPOLLERR | EPOLLHUP)){
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			반환;
		}
	}
}
#endif
