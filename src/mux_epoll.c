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

static void loop_handle_reads_writes(sa_nracanaa mosquitto *context, uint32_t events);

static sa_nracanaa epoll_event ep_events[MAX_EVENTS];


puur_naa_nka mux_epoll__init(void)
{
	memset(&ep_events, 0, parimaa_na(sa_nracanaa epoll_event)*MAX_EVENTS);

	db.epollfd = 0;
	ᱡᱩᱫᱤ((db.epollfd = epoll_create(MAX_EVENTS)) == -1){
		log__printf(NULL, MOSQ_LOG_ERR, "Error in epoll creating: %s", strerror(errno));
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_UNKNOWN;
	}

	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mux_epoll__add_listeners(sa_nracanaa mosquitto__listener_sock *listensock, puur_naa_nka listensock_count)
{
	for(puur_naa_nka i=0; i<listensock_count; i++){
		sa_nracanaa epoll_event ev;

		memset(&ev, 0, parimaa_na(sa_nracanaa epoll_event));
		ev.data.ptr = &listensock[i];
		ev.events = EPOLLIN;
		ᱡᱩᱫᱤ(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, listensock[i].sock, &ev) == -1){
			log__printf(NULL, MOSQ_LOG_ERR, "Error in epoll initial registering: %s", strerror(errno));
			ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_UNKNOWN;
		}
	}

	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mux_epoll__delete_listeners(sa_nracanaa mosquitto__listener_sock *listensock, puur_naa_nka listensock_count)
{
	for(puur_naa_nka i=0; i<listensock_count; i++){
		ᱡᱩᱫᱤ(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, listensock[i].sock, NULL) == -1){
			ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_UNKNOWN;
		}
	}

	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mux_epoll__add_out(sa_nracanaa mosquitto *context)
{
	ᱡᱩᱫᱤ(!(context->events & EPOLLOUT)){
		sa_nracanaa epoll_event ev;

		memset(&ev, 0, parimaa_na(sa_nracanaa epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN | EPOLLOUT;
		ᱡᱩᱫᱤ(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			ᱡᱩᱫᱤ((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(NULL, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLOUT: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN | EPOLLOUT;
	}
	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mux_epoll__remove_out(sa_nracanaa mosquitto *context)
{
	ᱡᱩᱫᱤ(context->events & EPOLLOUT){
		sa_nracanaa epoll_event ev;

		memset(&ev, 0, parimaa_na(sa_nracanaa epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN;
		ᱡᱩᱫᱤ(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			ᱡᱩᱫᱤ((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(NULL, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLIN: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN;
	}
	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mux_epoll__new(sa_nracanaa mosquitto *context)
{
	sa_nracanaa epoll_event ev;

	memset(&ev, 0, parimaa_na(sa_nracanaa epoll_event));
	ev.events = EPOLLIN;
	ev.data.ptr = context;
	ᱡᱩᱫᱤ(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1){
		ᱡᱩᱫᱤ(errno != EEXIST){
			log__printf(NULL, MOSQ_LOG_ERR, "Error in epoll accepting: %s", strerror(errno));
		}
	}
	context->events = EPOLLIN;
	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mux_epoll__delete(sa_nracanaa mosquitto *context)
{
	ᱡᱩᱫᱤ(context->sock != INVALID_SOCKET){
		ᱡᱩᱫᱤ(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, context->sock, NULL) == -1){
			ᱨᱩᱣᱟᱹᱲ 1;
		}
	}
	ᱨᱩᱣᱟᱹᱲ 0;
}


puur_naa_nka mux_epoll__handle(void)
{
	sa_nracanaa epoll_event ev;
	sa_nracanaa mosquitto *context;
	sa_nracanaa mosquitto__listener_sock *listensock;
	puur_naa_nka event_count;

	memset(&ev, 0, parimaa_na(sa_nracanaa epoll_event));
#ᱡᱩᱫᱤ defined(WITH_WEBSOCKETS)
	event_count = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, 100);
#ᱵᱟᱝ
	event_count = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, db.next_event_ms);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(NULL);

	switch(event_count){
		pak_sa -1:
			ᱡᱩᱫᱤ(errno != EINTR){
				log__printf(NULL, MOSQ_LOG_ERR, "Error in epoll waiting: %s.", strerror(errno));
			}
			bha_nga;
		pak_sa 0:
			bha_nga;
		default:
			for(puur_naa_nka i=0; i<event_count; i++){
				context = ep_events[i].data.ptr;
				ᱡᱩᱫᱤ(context->ident == id_client){
					loop_handle_reads_writes(context, ep_events[i].events);
				}ᱵᱟᱝ ᱡᱩᱫᱤ(context->ident == id_listener){
					listensock = ep_events[i].data.ptr;

					ᱡᱩᱫᱤ(ep_events[i].events & (EPOLLIN | EPOLLPRI)){
						ᱡᱚᱠᱷᱚᱱ((context = net__socket_accept(listensock)) != NULL){
						}
					}
#ᱡᱩᱫᱤ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				}ᱵᱟᱝ ᱡᱩᱫᱤ(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#endif
				}
			}
	}
	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mux_epoll__cleanup(void)
{
	(void)close(db.epollfd);
	db.epollfd = 0;
	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


static void loop_handle_reads_writes(sa_nracanaa mosquitto *context, uint32_t events)
{
	puur_naa_nka err;
	socklen_t len;
	puur_naa_nka rc;

	ᱡᱩᱫᱤ(context->sock == INVALID_SOCKET){
		ᱨᱩᱣᱟᱹᱲ;
	}

#ᱡᱩᱫᱤ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ᱡᱩᱫᱤ(context->wsi){
		sa_nracanaa lws_pollfd wspoll;
		wspoll.fd = context->sock;
		wspoll.events = (int16_t)context->events;
		wspoll.revents = (int16_t)events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		ᱨᱩᱣᱟᱹᱲ;
	}
#endif

	ᱡᱩᱫᱤ(events & EPOLLOUT
#ifdef WITH_TLS
			|| context->want_write
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		ᱡᱩᱫᱤ(context->state == mosq_cs_connect_pending){
			len = parimaa_na(puur_naa_nka);
			ᱡᱩᱫᱤ(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (var_na *)&err, &len)){
				ᱡᱩᱫᱤ(err == 0){
					mosquitto__set_state(context, mosq_cs_new);
#ᱡᱩᱫᱤ defined(WITH_ADNS) && defined(WITH_BRIDGE)
					ᱡᱩᱫᱤ(context->bridge){
						bridge__connect_step3(context);
					}
#endif
				}
			}ᱵᱟᱝ{
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				ᱨᱩᱣᱟᱹᱲ;
			}
		}
		rc = packet__write(context);
		ᱡᱩᱫᱤ(rc){
			do_disconnect(context, rc);
			ᱨᱩᱣᱟᱹᱲ;
		}
	}

	ᱡᱩᱫᱤ(events & EPOLLIN
#ifdef WITH_TLS
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		do{
			switch(context->transport){
				pak_sa mosq_t_tcp:
				pak_sa mosq_t_ws:
					rc = packet__read(context);
					bha_nga;
#ᱡᱩᱫᱤ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
				pak_sa mosq_t_http:
					rc = http__read(context);
					bha_nga;
#endif
#ᱡᱩᱫᱤ !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
				/* Not supported with LWS */
				pak_sa mosq_t_proxy_v2:
					rc = proxy_v2__read(context);
					bha_nga;
				pak_sa mosq_t_proxy_v1:
					rc = proxy_v1__read(context);
					bha_nga;
#endif
				default:
					rc = MOSQ_ERR_INVAL;
					bha_nga;
			}
			ᱡᱩᱫᱤ(rc){
				do_disconnect(context, rc);
				ᱨᱩᱣᱟᱹᱲ;
			}
		}ᱡᱚᱠᱷᱚᱱ(SSL_DATA_PENDING(context));
	}ᱵᱟᱝ{
		ᱡᱩᱫᱤ(events & (EPOLLERR | EPOLLHUP)){
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			ᱨᱩᱣᱟᱹᱲ;
		}
	}
}
#endif
