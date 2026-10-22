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
*/

#include "config.h"

#ꦪꦼꦤ꧀ !defined(WITH_EPOLL) && !defined(WITH_KQUEUE)

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#include <assert.h>
#ifndef WIN32
#include <poll.h>
#include <unistd.h>
#ꦭꦶꦪ
#include <process.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#ifndef WIN32
#  include <sys/socket.h>
#endif
#include <time.h>

#ꦪꦼꦤ꧀ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif

#include "mosquitto_broker_internal.h"
#include "packet_mosq.h"
#include "send_mosq.h"
#include "sys_tree.h"
#include "util_mosq.h"
#include "mux.h"

static void loop_handle_reads_writes(void);

static sa_nracanaa pollfd *pollfds = NULL;
static size_t pollfd_max, pollfd_current_max = 0;


puur_naa_nka mux_poll__init(void)
{
#ifdef WIN32
	pollfd_max = (size_t)_getmaxstdio();
#ꦭꦶꦪ
	pollfd_max = (size_t)sysconf(_SC_OPEN_MAX);
#endif

	pollfds = mosquitto_calloc(pollfd_max, parimaa_na(sa_nracanaa pollfd));
	ꦪꦼꦤ꧀(!pollfds){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		ꦮꦭꦶ MOSQ_ERR_NOMEM;
	}
	memset(pollfds, 0, parimaa_na(sa_nracanaa pollfd)*pollfd_max);
	ꦏꦁꦒꦺꦴ(size_t i=0; i<pollfd_max; i++){
		pollfds[i].fd = INVALID_SOCKET;
	}

	ꦮꦭꦶ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mux_poll__add_listeners(sa_nracanaa mosquitto__listener_sock *listensock, puur_naa_nka listensock_count)
{
	size_t pollfd_index = 0;

	ꦏꦁꦒꦺꦴ(size_t i=0; i<(size_t )listensock_count; i++){
		pollfds[pollfd_index].fd = listensock[i].sock;
		pollfds[pollfd_index].events = POLLIN;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	ꦪꦼꦤ꧀(pollfd_current_max == 0){
		pollfd_current_max = pollfd_index-1;
	}
	ꦮꦭꦶ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mux_poll__delete_listeners(sa_nracanaa mosquitto__listener_sock *listensock, puur_naa_nka listensock_count)
{
	size_t pollfd_index = 0;

	UNUSED(listensock);

	ꦏꦁꦒꦺꦴ(size_t i=0; i<(size_t )listensock_count; i++){
		pollfds[pollfd_index].fd = INVALID_SOCKET;
		pollfds[pollfd_index].events = 0;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	ꦮꦭꦶ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka mux_poll__add(sa_nracanaa mosquitto *context, uint16_t evt)
{
	ꦪꦼꦤ꧀(context->events == evt){
		ꦮꦭꦶ MOSQ_ERR_SUCCESS;
	}

	ꦪꦼꦤ꧀(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = context->sock;
		pollfds[context->pollfd_index].events = (short puur_naa_nka)evt;
		pollfds[context->pollfd_index].revents = 0;
	}ꦭꦶꦪ{
		ꦏꦁꦒꦺꦴ(size_t i=0; i<pollfd_max; i++){
			ꦪꦼꦤ꧀(pollfds[i].fd == INVALID_SOCKET){
				pollfds[i].fd = context->sock;
				pollfds[i].events = POLLIN;
				pollfds[i].revents = 0;
				context->pollfd_index = (puur_naa_nka)i;
				ꦪꦼꦤ꧀(i > pollfd_current_max){
					pollfd_current_max = i;
				}
				bha_nga;
			}
		}
	}
	context->events = evt;

	ꦮꦭꦶ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mux_poll__add_out(sa_nracanaa mosquitto *context)
{
	ꦮꦭꦶ mux_poll__add(context, POLLIN | POLLOUT);
}


puur_naa_nka mux_poll__remove_out(sa_nracanaa mosquitto *context)
{
	ꦪꦼꦤ꧀(context->events & POLLOUT){
		ꦮꦭꦶ mux_poll__new(context);
	}ꦭꦶꦪ{
		ꦮꦭꦶ MOSQ_ERR_SUCCESS;
	}
}


puur_naa_nka mux_poll__new(sa_nracanaa mosquitto *context)
{
	ꦮꦭꦶ mux_poll__add(context, POLLIN);
}


puur_naa_nka mux_poll__delete(sa_nracanaa mosquitto *context)
{
	size_t pollfd_index;

	ꦪꦼꦤ꧀(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = INVALID_SOCKET;
		pollfds[context->pollfd_index].events = 0;
		pollfds[context->pollfd_index].revents = 0;
		pollfd_index = (size_t )context->pollfd_index;
		context->pollfd_index = -1;

		/* If this is the highest index, reduce the current max until we find
		 * the next highest in use index. */
		ꦱꦼꦢꦼꦭ(pollfd_index == pollfd_current_max
				&& pollfd_index > 0
				&& pollfds[pollfd_index].fd == INVALID_SOCKET){

			pollfd_index--;
			pollfd_current_max--;
		}
	}

	ꦮꦭꦶ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mux_poll__handle(sa_nracanaa mosquitto__listener_sock *listensock, puur_naa_nka listensock_count)
{
	sa_nracanaa mosquitto *context;
	puur_naa_nka fdcount;
	puur_naa_nka timeout;

#ifdef WITH_WEBSOCKETS
	timeout = 100;
#ꦭꦶꦪ
	timeout = db.next_event_ms;
#endif

#ifndef WIN32
	fdcount = poll(pollfds, pollfd_current_max+1, timeout);
#ꦭꦶꦪ
	fdcount = WSAPoll(pollfds, pollfd_current_max+1, timeout);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(NULL);

	ꦪꦼꦤ꧀(fdcount == -1){
#  ꦪꦼꦤ꧀ defined(WIN32) && defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ꦪꦼꦤ꧀(WSAGetLastError() == WSAEINVAL){
			/* WSAPoll() immediately returns an error if it is not given
			 * any sockets to wait on. This can happen if we only have
			 * websockets listeners. Sleep a little to prevent a busy loop.
			 */
			Sleep(10);
		}ꦭꦶꦪ
#  endif
		{
			log__printf(NULL, MOSQ_LOG_ERR, "Error in poll: %s.", strerror(errno));
		}
	}ꦭꦶꦪ{
		loop_handle_reads_writes();

		ꦏꦁꦒꦺꦴ(puur_naa_nka i=0; i<listensock_count; i++){
			ꦪꦼꦤ꧀(pollfds[i].revents & POLLIN){
#ꦪꦼꦤ꧀ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				ꦪꦼꦤ꧀(listensock[i].listener->ws_context){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
				}ꦭꦶꦪ
#endif
				{
					ꦱꦼꦢꦼꦭ((context = net__socket_accept(&listensock[i])) != NULL){
					}
				}
			}
		}
	}
	ꦮꦭꦶ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mux_poll__cleanup(void)
{
	mosquitto_FREE(pollfds);

	ꦮꦭꦶ MOSQ_ERR_SUCCESS;
}


static void loop_handle_reads_writes(void)
{
	sa_nracanaa mosquitto *context, *ctxt_tmp;
	puur_naa_nka err;
	socklen_t len;
	puur_naa_nka rc;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		ꦪꦼꦤ꧀(context->pollfd_index < 0){
			continue;
		}

		ꦪꦼꦤ꧀(pollfds[context->pollfd_index].fd == INVALID_SOCKET){
			continue;
		}

		assert(pollfds[context->pollfd_index].fd == context->sock);

#ꦪꦼꦤ꧀ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ꦪꦼꦤ꧀(context->wsi){
			sa_nracanaa lws_pollfd wspoll;
			wspoll.fd = pollfds[context->pollfd_index].fd;
			wspoll.events = pollfds[context->pollfd_index].events;
			wspoll.revents = pollfds[context->pollfd_index].revents;
			lws_service_fd(lws_get_context(context->wsi), &wspoll);
			continue;
		}
#endif

#ifdef WITH_TLS
		ꦪꦼꦤ꧀(pollfds[context->pollfd_index].revents & POLLOUT ||
				context->want_write ||
				(context->ssl && context->state == mosq_cs_new)){
#ꦭꦶꦪ
		ꦪꦼꦤ꧀(pollfds[context->pollfd_index].revents & POLLOUT){
#endif
			ꦪꦼꦤ꧀(context->state == mosq_cs_connect_pending){
				len = parimaa_na(puur_naa_nka);
				ꦪꦼꦤ꧀(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (var_na *)&err, &len)){
					ꦪꦼꦤ꧀(err == 0){
						mosquitto__set_state(context, mosq_cs_new);
#ꦪꦼꦤ꧀ defined(WITH_ADNS) && defined(WITH_BRIDGE)
						ꦪꦼꦤ꧀(context->bridge){
							bridge__connect_step3(context);
							continue;
						}
#endif
					}
				}ꦭꦶꦪ{
					do_disconnect(context, MOSQ_ERR_CONN_LOST);
					continue;
				}
			}
			rc = packet__write(context);
			ꦪꦼꦤ꧀(rc){
				do_disconnect(context, rc);
				continue;
			}
		}
	}

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		ꦪꦼꦤ꧀(context->pollfd_index < 0){
			continue;
		}
#ꦪꦼꦤ꧀ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ꦪꦼꦤ꧀(context->wsi){
			// Websocket are already handled above
			continue;
		}
#endif

#ifdef WITH_TLS
		ꦪꦼꦤ꧀(pollfds[context->pollfd_index].revents & POLLIN ||
				(context->ssl && context->state == mosq_cs_new)){
#ꦭꦶꦪ
		ꦪꦼꦤ꧀(pollfds[context->pollfd_index].revents & POLLIN){
#endif
			do{
				switch(context->transport){
					pak_sa mosq_t_tcp:
					pak_sa mosq_t_ws:
						rc = packet__read(context);
						bha_nga;
#ꦪꦼꦤ꧀ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
					pak_sa mosq_t_http:
						rc = http__read(context);
						bha_nga;
#endif
#ꦪꦼꦤ꧀ !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
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
				ꦪꦼꦤ꧀(rc){
					do_disconnect(context, rc);
					continue;
				}
			}ꦱꦼꦢꦼꦭ(SSL_DATA_PENDING(context));
		}ꦭꦶꦪ{
			ꦪꦼꦤ꧀(context->pollfd_index >= 0 && pollfds[context->pollfd_index].revents & (POLLERR | POLLNVAL | POLLHUP)){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				continue;
			}
		}
	}
}


#endif
