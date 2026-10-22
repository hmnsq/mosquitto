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

#nếu !defined(WITH_EPOLL) && !defined(WITH_KQUEUE)

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#include <assert.h>
#ifndef WIN32
#include <poll.h>
#include <unistd.h>
#không_thì
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

#nếu defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif

#include "mosquitto_broker_internal.h"
#include "packet_mosq.h"
#include "send_mosq.h"
#include "sys_tree.h"
#include "util_mosq.h"
#include "mux.h"

static void loop_handle_reads_writes(void);

static struct pollfd *pollfds = NULL;
static size_t pollfd_max, pollfd_current_max = 0;


số_nguyên mux_poll__init(void)
{
#ifdef WIN32
	pollfd_max = (size_t)_getmaxstdio();
#không_thì
	pollfd_max = (size_t)sysconf(_SC_OPEN_MAX);
#endif

	pollfds = mosquitto_calloc(pollfd_max, sizeof(struct pollfd));
	nếu(!pollfds){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		trả_về MOSQ_ERR_NOMEM;
	}
	memset(pollfds, 0, sizeof(struct pollfd)*pollfd_max);
	cho(size_t i=0; i<pollfd_max; i++){
		pollfds[i].fd = INVALID_SOCKET;
	}

	trả_về MOSQ_ERR_SUCCESS;
}


số_nguyên mux_poll__add_listeners(struct mosquitto__listener_sock *listensock, số_nguyên listensock_count)
{
	size_t pollfd_index = 0;

	cho(size_t i=0; i<(size_t )listensock_count; i++){
		pollfds[pollfd_index].fd = listensock[i].sock;
		pollfds[pollfd_index].events = POLLIN;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	nếu(pollfd_current_max == 0){
		pollfd_current_max = pollfd_index-1;
	}
	trả_về MOSQ_ERR_SUCCESS;
}


số_nguyên mux_poll__delete_listeners(struct mosquitto__listener_sock *listensock, số_nguyên listensock_count)
{
	size_t pollfd_index = 0;

	UNUSED(listensock);

	cho(size_t i=0; i<(size_t )listensock_count; i++){
		pollfds[pollfd_index].fd = INVALID_SOCKET;
		pollfds[pollfd_index].events = 0;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	trả_về MOSQ_ERR_SUCCESS;
}


static số_nguyên mux_poll__add(struct mosquitto *context, uint16_t evt)
{
	nếu(context->events == evt){
		trả_về MOSQ_ERR_SUCCESS;
	}

	nếu(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = context->sock;
		pollfds[context->pollfd_index].events = (short số_nguyên)evt;
		pollfds[context->pollfd_index].revents = 0;
	}không_thì{
		cho(size_t i=0; i<pollfd_max; i++){
			nếu(pollfds[i].fd == INVALID_SOCKET){
				pollfds[i].fd = context->sock;
				pollfds[i].events = POLLIN;
				pollfds[i].revents = 0;
				context->pollfd_index = (số_nguyên)i;
				nếu(i > pollfd_current_max){
					pollfd_current_max = i;
				}
				break;
			}
		}
	}
	context->events = evt;

	trả_về MOSQ_ERR_SUCCESS;
}


số_nguyên mux_poll__add_out(struct mosquitto *context)
{
	trả_về mux_poll__add(context, POLLIN | POLLOUT);
}


số_nguyên mux_poll__remove_out(struct mosquitto *context)
{
	nếu(context->events & POLLOUT){
		trả_về mux_poll__new(context);
	}không_thì{
		trả_về MOSQ_ERR_SUCCESS;
	}
}


số_nguyên mux_poll__new(struct mosquitto *context)
{
	trả_về mux_poll__add(context, POLLIN);
}


số_nguyên mux_poll__delete(struct mosquitto *context)
{
	size_t pollfd_index;

	nếu(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = INVALID_SOCKET;
		pollfds[context->pollfd_index].events = 0;
		pollfds[context->pollfd_index].revents = 0;
		pollfd_index = (size_t )context->pollfd_index;
		context->pollfd_index = -1;

		/* If this is the highest index, reduce the current max until we find
		 * the next highest in use index. */
		trong_khi(pollfd_index == pollfd_current_max
				&& pollfd_index > 0
				&& pollfds[pollfd_index].fd == INVALID_SOCKET){

			pollfd_index--;
			pollfd_current_max--;
		}
	}

	trả_về MOSQ_ERR_SUCCESS;
}


số_nguyên mux_poll__handle(struct mosquitto__listener_sock *listensock, số_nguyên listensock_count)
{
	struct mosquitto *context;
	số_nguyên fdcount;
	số_nguyên timeout;

#ifdef WITH_WEBSOCKETS
	timeout = 100;
#không_thì
	timeout = db.next_event_ms;
#endif

#ifndef WIN32
	fdcount = poll(pollfds, pollfd_current_max+1, timeout);
#không_thì
	fdcount = WSAPoll(pollfds, pollfd_current_max+1, timeout);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(NULL);

	nếu(fdcount == -1){
#  nếu defined(WIN32) && defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		nếu(WSAGetLastError() == WSAEINVAL){
			/* WSAPoll() immediately returns an error if it is not given
			 * any sockets to wait on. This can happen if we only have
			 * websockets listeners. Sleep a little to prevent a busy loop.
			 */
			Sleep(10);
		}không_thì
#  endif
		{
			log__printf(NULL, MOSQ_LOG_ERR, "Error in poll: %s.", strerror(errno));
		}
	}không_thì{
		loop_handle_reads_writes();

		cho(số_nguyên i=0; i<listensock_count; i++){
			nếu(pollfds[i].revents & POLLIN){
#nếu defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				nếu(listensock[i].listener->ws_context){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
				}không_thì
#endif
				{
					trong_khi((context = net__socket_accept(&listensock[i])) != NULL){
					}
				}
			}
		}
	}
	trả_về MOSQ_ERR_SUCCESS;
}


số_nguyên mux_poll__cleanup(void)
{
	mosquitto_FREE(pollfds);

	trả_về MOSQ_ERR_SUCCESS;
}


static void loop_handle_reads_writes(void)
{
	struct mosquitto *context, *ctxt_tmp;
	số_nguyên err;
	socklen_t len;
	số_nguyên rc;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		nếu(context->pollfd_index < 0){
			continue;
		}

		nếu(pollfds[context->pollfd_index].fd == INVALID_SOCKET){
			continue;
		}

		assert(pollfds[context->pollfd_index].fd == context->sock);

#nếu defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		nếu(context->wsi){
			struct lws_pollfd wspoll;
			wspoll.fd = pollfds[context->pollfd_index].fd;
			wspoll.events = pollfds[context->pollfd_index].events;
			wspoll.revents = pollfds[context->pollfd_index].revents;
			lws_service_fd(lws_get_context(context->wsi), &wspoll);
			continue;
		}
#endif

#ifdef WITH_TLS
		nếu(pollfds[context->pollfd_index].revents & POLLOUT ||
				context->want_write ||
				(context->ssl && context->state == mosq_cs_new)){
#không_thì
		nếu(pollfds[context->pollfd_index].revents & POLLOUT){
#endif
			nếu(context->state == mosq_cs_connect_pending){
				len = sizeof(số_nguyên);
				nếu(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (char *)&err, &len)){
					nếu(err == 0){
						mosquitto__set_state(context, mosq_cs_new);
#nếu defined(WITH_ADNS) && defined(WITH_BRIDGE)
						nếu(context->bridge){
							bridge__connect_step3(context);
							continue;
						}
#endif
					}
				}không_thì{
					do_disconnect(context, MOSQ_ERR_CONN_LOST);
					continue;
				}
			}
			rc = packet__write(context);
			nếu(rc){
				do_disconnect(context, rc);
				continue;
			}
		}
	}

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		nếu(context->pollfd_index < 0){
			continue;
		}
#nếu defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		nếu(context->wsi){
			// Websocket are already handled above
			continue;
		}
#endif

#ifdef WITH_TLS
		nếu(pollfds[context->pollfd_index].revents & POLLIN ||
				(context->ssl && context->state == mosq_cs_new)){
#không_thì
		nếu(pollfds[context->pollfd_index].revents & POLLIN){
#endif
			do{
				switch(context->transport){
					case mosq_t_tcp:
					case mosq_t_ws:
						rc = packet__read(context);
						break;
#nếu defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
					case mosq_t_http:
						rc = http__read(context);
						break;
#endif
#nếu !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
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
				nếu(rc){
					do_disconnect(context, rc);
					continue;
				}
			}trong_khi(SSL_DATA_PENDING(context));
		}không_thì{
			nếu(context->pollfd_index >= 0 && pollfds[context->pollfd_index].revents & (POLLERR | POLLNVAL | POLLHUP)){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				continue;
			}
		}
	}
}


#endif
