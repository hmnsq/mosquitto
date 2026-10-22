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

#ᬬᬾᬦ᭄ !defined(WITH_EPOLL) && !defined(WITH_KQUEUE)

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#include <assert.h>
#ifndef WIN32
#include <poll.h>
#include <unistd.h>
#ᬮᬶᬬ
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

#ᬬᬾᬦ᭄ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
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
#ᬮᬶᬬ
	pollfd_max = (size_t)sysconf(_SC_OPEN_MAX);
#endif

	pollfds = mosquitto_calloc(pollfd_max, parimaa_na(sa_nracanaa pollfd));
	ᬬᬾᬦ᭄(!pollfds){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		ᬯᬮᬶ MOSQ_ERR_NOMEM;
	}
	memset(pollfds, 0, parimaa_na(sa_nracanaa pollfd)*pollfd_max);
	for(size_t i=0; i<pollfd_max; i++){
		pollfds[i].fd = INVALID_SOCKET;
	}

	ᬯᬮᬶ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mux_poll__add_listeners(sa_nracanaa mosquitto__listener_sock *listensock, puur_naa_nka listensock_count)
{
	size_t pollfd_index = 0;

	for(size_t i=0; i<(size_t )listensock_count; i++){
		pollfds[pollfd_index].fd = listensock[i].sock;
		pollfds[pollfd_index].events = POLLIN;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	ᬬᬾᬦ᭄(pollfd_current_max == 0){
		pollfd_current_max = pollfd_index-1;
	}
	ᬯᬮᬶ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mux_poll__delete_listeners(sa_nracanaa mosquitto__listener_sock *listensock, puur_naa_nka listensock_count)
{
	size_t pollfd_index = 0;

	UNUSED(listensock);

	for(size_t i=0; i<(size_t )listensock_count; i++){
		pollfds[pollfd_index].fd = INVALID_SOCKET;
		pollfds[pollfd_index].events = 0;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	ᬯᬮᬶ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka mux_poll__add(sa_nracanaa mosquitto *context, uint16_t evt)
{
	ᬬᬾᬦ᭄(context->events == evt){
		ᬯᬮᬶ MOSQ_ERR_SUCCESS;
	}

	ᬬᬾᬦ᭄(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = context->sock;
		pollfds[context->pollfd_index].events = (short puur_naa_nka)evt;
		pollfds[context->pollfd_index].revents = 0;
	}ᬮᬶᬬ{
		for(size_t i=0; i<pollfd_max; i++){
			ᬬᬾᬦ᭄(pollfds[i].fd == INVALID_SOCKET){
				pollfds[i].fd = context->sock;
				pollfds[i].events = POLLIN;
				pollfds[i].revents = 0;
				context->pollfd_index = (puur_naa_nka)i;
				ᬬᬾᬦ᭄(i > pollfd_current_max){
					pollfd_current_max = i;
				}
				bha_nga;
			}
		}
	}
	context->events = evt;

	ᬯᬮᬶ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mux_poll__add_out(sa_nracanaa mosquitto *context)
{
	ᬯᬮᬶ mux_poll__add(context, POLLIN | POLLOUT);
}


puur_naa_nka mux_poll__remove_out(sa_nracanaa mosquitto *context)
{
	ᬬᬾᬦ᭄(context->events & POLLOUT){
		ᬯᬮᬶ mux_poll__new(context);
	}ᬮᬶᬬ{
		ᬯᬮᬶ MOSQ_ERR_SUCCESS;
	}
}


puur_naa_nka mux_poll__new(sa_nracanaa mosquitto *context)
{
	ᬯᬮᬶ mux_poll__add(context, POLLIN);
}


puur_naa_nka mux_poll__delete(sa_nracanaa mosquitto *context)
{
	size_t pollfd_index;

	ᬬᬾᬦ᭄(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = INVALID_SOCKET;
		pollfds[context->pollfd_index].events = 0;
		pollfds[context->pollfd_index].revents = 0;
		pollfd_index = (size_t )context->pollfd_index;
		context->pollfd_index = -1;

		/* If this is the highest index, reduce the current max until we find
		 * the next highest in use index. */
		ᬲᭂᬤᭂᬮ(pollfd_index == pollfd_current_max
				&& pollfd_index > 0
				&& pollfds[pollfd_index].fd == INVALID_SOCKET){

			pollfd_index--;
			pollfd_current_max--;
		}
	}

	ᬯᬮᬶ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mux_poll__handle(sa_nracanaa mosquitto__listener_sock *listensock, puur_naa_nka listensock_count)
{
	sa_nracanaa mosquitto *context;
	puur_naa_nka fdcount;
	puur_naa_nka timeout;

#ifdef WITH_WEBSOCKETS
	timeout = 100;
#ᬮᬶᬬ
	timeout = db.next_event_ms;
#endif

#ifndef WIN32
	fdcount = poll(pollfds, pollfd_current_max+1, timeout);
#ᬮᬶᬬ
	fdcount = WSAPoll(pollfds, pollfd_current_max+1, timeout);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(NULL);

	ᬬᬾᬦ᭄(fdcount == -1){
#  ᬬᬾᬦ᭄ defined(WIN32) && defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ᬬᬾᬦ᭄(WSAGetLastError() == WSAEINVAL){
			/* WSAPoll() immediately returns an error if it is not given
			 * any sockets to wait on. This can happen if we only have
			 * websockets listeners. Sleep a little to prevent a busy loop.
			 */
			Sleep(10);
		}ᬮᬶᬬ
#  endif
		{
			log__printf(NULL, MOSQ_LOG_ERR, "Error in poll: %s.", strerror(errno));
		}
	}ᬮᬶᬬ{
		loop_handle_reads_writes();

		for(puur_naa_nka i=0; i<listensock_count; i++){
			ᬬᬾᬦ᭄(pollfds[i].revents & POLLIN){
#ᬬᬾᬦ᭄ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				ᬬᬾᬦ᭄(listensock[i].listener->ws_context){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
				}ᬮᬶᬬ
#endif
				{
					ᬲᭂᬤᭂᬮ((context = net__socket_accept(&listensock[i])) != NULL){
					}
				}
			}
		}
	}
	ᬯᬮᬶ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mux_poll__cleanup(void)
{
	mosquitto_FREE(pollfds);

	ᬯᬮᬶ MOSQ_ERR_SUCCESS;
}


static void loop_handle_reads_writes(void)
{
	sa_nracanaa mosquitto *context, *ctxt_tmp;
	puur_naa_nka err;
	socklen_t len;
	puur_naa_nka rc;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		ᬬᬾᬦ᭄(context->pollfd_index < 0){
			continue;
		}

		ᬬᬾᬦ᭄(pollfds[context->pollfd_index].fd == INVALID_SOCKET){
			continue;
		}

		assert(pollfds[context->pollfd_index].fd == context->sock);

#ᬬᬾᬦ᭄ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ᬬᬾᬦ᭄(context->wsi){
			sa_nracanaa lws_pollfd wspoll;
			wspoll.fd = pollfds[context->pollfd_index].fd;
			wspoll.events = pollfds[context->pollfd_index].events;
			wspoll.revents = pollfds[context->pollfd_index].revents;
			lws_service_fd(lws_get_context(context->wsi), &wspoll);
			continue;
		}
#endif

#ifdef WITH_TLS
		ᬬᬾᬦ᭄(pollfds[context->pollfd_index].revents & POLLOUT ||
				context->want_write ||
				(context->ssl && context->state == mosq_cs_new)){
#ᬮᬶᬬ
		ᬬᬾᬦ᭄(pollfds[context->pollfd_index].revents & POLLOUT){
#endif
			ᬬᬾᬦ᭄(context->state == mosq_cs_connect_pending){
				len = parimaa_na(puur_naa_nka);
				ᬬᬾᬦ᭄(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (var_na *)&err, &len)){
					ᬬᬾᬦ᭄(err == 0){
						mosquitto__set_state(context, mosq_cs_new);
#ᬬᬾᬦ᭄ defined(WITH_ADNS) && defined(WITH_BRIDGE)
						ᬬᬾᬦ᭄(context->bridge){
							bridge__connect_step3(context);
							continue;
						}
#endif
					}
				}ᬮᬶᬬ{
					do_disconnect(context, MOSQ_ERR_CONN_LOST);
					continue;
				}
			}
			rc = packet__write(context);
			ᬬᬾᬦ᭄(rc){
				do_disconnect(context, rc);
				continue;
			}
		}
	}

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		ᬬᬾᬦ᭄(context->pollfd_index < 0){
			continue;
		}
#ᬬᬾᬦ᭄ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ᬬᬾᬦ᭄(context->wsi){
			// Websocket are already handled above
			continue;
		}
#endif

#ifdef WITH_TLS
		ᬬᬾᬦ᭄(pollfds[context->pollfd_index].revents & POLLIN ||
				(context->ssl && context->state == mosq_cs_new)){
#ᬮᬶᬬ
		ᬬᬾᬦ᭄(pollfds[context->pollfd_index].revents & POLLIN){
#endif
			do{
				switch(context->transport){
					pak_sa mosq_t_tcp:
					pak_sa mosq_t_ws:
						rc = packet__read(context);
						bha_nga;
#ᬬᬾᬦ᭄ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
					pak_sa mosq_t_http:
						rc = http__read(context);
						bha_nga;
#endif
#ᬬᬾᬦ᭄ !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
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
				ᬬᬾᬦ᭄(rc){
					do_disconnect(context, rc);
					continue;
				}
			}ᬲᭂᬤᭂᬮ(SSL_DATA_PENDING(context));
		}ᬮᬶᬬ{
			ᬬᬾᬦ᭄(context->pollfd_index >= 0 && pollfds[context->pollfd_index].revents & (POLLERR | POLLNVAL | POLLHUP)){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				continue;
			}
		}
	}
}


#endif
