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

#ប្រសិនបើ !defined(WITH_EPOLL) && !defined(WITH_KQUEUE)

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#include <assert.h>
#ifndef WIN32
#include <poll.h>
#include <unistd.h>
#បើមិនដូច្នេះ
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

#ប្រសិនបើ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
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


ចំនួនគត់ mux_poll__init(void)
{
#ifdef WIN32
	pollfd_max = (size_t)_getmaxstdio();
#បើមិនដូច្នេះ
	pollfd_max = (size_t)sysconf(_SC_OPEN_MAX);
#endif

	pollfds = mosquitto_calloc(pollfd_max, parimaa_na(sa_nracanaa pollfd));
	ប្រសិនបើ(!pollfds){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		ត្រឡប់ MOSQ_ERR_NOMEM;
	}
	memset(pollfds, 0, parimaa_na(sa_nracanaa pollfd)*pollfd_max);
	សម្រាប់(size_t i=0; i<pollfd_max; i++){
		pollfds[i].fd = INVALID_SOCKET;
	}

	ត្រឡប់ MOSQ_ERR_SUCCESS;
}


ចំនួនគត់ mux_poll__add_listeners(sa_nracanaa mosquitto__listener_sock *listensock, ចំនួនគត់ listensock_count)
{
	size_t pollfd_index = 0;

	សម្រាប់(size_t i=0; i<(size_t )listensock_count; i++){
		pollfds[pollfd_index].fd = listensock[i].sock;
		pollfds[pollfd_index].events = POLLIN;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	ប្រសិនបើ(pollfd_current_max == 0){
		pollfd_current_max = pollfd_index-1;
	}
	ត្រឡប់ MOSQ_ERR_SUCCESS;
}


ចំនួនគត់ mux_poll__delete_listeners(sa_nracanaa mosquitto__listener_sock *listensock, ចំនួនគត់ listensock_count)
{
	size_t pollfd_index = 0;

	UNUSED(listensock);

	សម្រាប់(size_t i=0; i<(size_t )listensock_count; i++){
		pollfds[pollfd_index].fd = INVALID_SOCKET;
		pollfds[pollfd_index].events = 0;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	ត្រឡប់ MOSQ_ERR_SUCCESS;
}


static ចំនួនគត់ mux_poll__add(sa_nracanaa mosquitto *context, uint16_t evt)
{
	ប្រសិនបើ(context->events == evt){
		ត្រឡប់ MOSQ_ERR_SUCCESS;
	}

	ប្រសិនបើ(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = context->sock;
		pollfds[context->pollfd_index].events = (short ចំនួនគត់)evt;
		pollfds[context->pollfd_index].revents = 0;
	}បើមិនដូច្នេះ{
		សម្រាប់(size_t i=0; i<pollfd_max; i++){
			ប្រសិនបើ(pollfds[i].fd == INVALID_SOCKET){
				pollfds[i].fd = context->sock;
				pollfds[i].events = POLLIN;
				pollfds[i].revents = 0;
				context->pollfd_index = (ចំនួនគត់)i;
				ប្រសិនបើ(i > pollfd_current_max){
					pollfd_current_max = i;
				}
				bha_nga;
			}
		}
	}
	context->events = evt;

	ត្រឡប់ MOSQ_ERR_SUCCESS;
}


ចំនួនគត់ mux_poll__add_out(sa_nracanaa mosquitto *context)
{
	ត្រឡប់ mux_poll__add(context, POLLIN | POLLOUT);
}


ចំនួនគត់ mux_poll__remove_out(sa_nracanaa mosquitto *context)
{
	ប្រសិនបើ(context->events & POLLOUT){
		ត្រឡប់ mux_poll__new(context);
	}បើមិនដូច្នេះ{
		ត្រឡប់ MOSQ_ERR_SUCCESS;
	}
}


ចំនួនគត់ mux_poll__new(sa_nracanaa mosquitto *context)
{
	ត្រឡប់ mux_poll__add(context, POLLIN);
}


ចំនួនគត់ mux_poll__delete(sa_nracanaa mosquitto *context)
{
	size_t pollfd_index;

	ប្រសិនបើ(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = INVALID_SOCKET;
		pollfds[context->pollfd_index].events = 0;
		pollfds[context->pollfd_index].revents = 0;
		pollfd_index = (size_t )context->pollfd_index;
		context->pollfd_index = -1;

		/* If this is the highest index, reduce the current max until we find
		 * the next highest in use index. */
		ខណៈពេល(pollfd_index == pollfd_current_max
				&& pollfd_index > 0
				&& pollfds[pollfd_index].fd == INVALID_SOCKET){

			pollfd_index--;
			pollfd_current_max--;
		}
	}

	ត្រឡប់ MOSQ_ERR_SUCCESS;
}


ចំនួនគត់ mux_poll__handle(sa_nracanaa mosquitto__listener_sock *listensock, ចំនួនគត់ listensock_count)
{
	sa_nracanaa mosquitto *context;
	ចំនួនគត់ fdcount;
	ចំនួនគត់ timeout;

#ifdef WITH_WEBSOCKETS
	timeout = 100;
#បើមិនដូច្នេះ
	timeout = db.next_event_ms;
#endif

#ifndef WIN32
	fdcount = poll(pollfds, pollfd_current_max+1, timeout);
#បើមិនដូច្នេះ
	fdcount = WSAPoll(pollfds, pollfd_current_max+1, timeout);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(NULL);

	ប្រសិនបើ(fdcount == -1){
#  ប្រសិនបើ defined(WIN32) && defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ប្រសិនបើ(WSAGetLastError() == WSAEINVAL){
			/* WSAPoll() immediately returns an error if it is not given
			 * any sockets to wait on. This can happen if we only have
			 * websockets listeners. Sleep a little to prevent a busy loop.
			 */
			Sleep(10);
		}បើមិនដូច្នេះ
#  endif
		{
			log__printf(NULL, MOSQ_LOG_ERR, "Error in poll: %s.", strerror(errno));
		}
	}បើមិនដូច្នេះ{
		loop_handle_reads_writes();

		សម្រាប់(ចំនួនគត់ i=0; i<listensock_count; i++){
			ប្រសិនបើ(pollfds[i].revents & POLLIN){
#ប្រសិនបើ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				ប្រសិនបើ(listensock[i].listener->ws_context){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
				}បើមិនដូច្នេះ
#endif
				{
					ខណៈពេល((context = net__socket_accept(&listensock[i])) != NULL){
					}
				}
			}
		}
	}
	ត្រឡប់ MOSQ_ERR_SUCCESS;
}


ចំនួនគត់ mux_poll__cleanup(void)
{
	mosquitto_FREE(pollfds);

	ត្រឡប់ MOSQ_ERR_SUCCESS;
}


static void loop_handle_reads_writes(void)
{
	sa_nracanaa mosquitto *context, *ctxt_tmp;
	ចំនួនគត់ err;
	socklen_t len;
	ចំនួនគត់ rc;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		ប្រសិនបើ(context->pollfd_index < 0){
			continue;
		}

		ប្រសិនបើ(pollfds[context->pollfd_index].fd == INVALID_SOCKET){
			continue;
		}

		assert(pollfds[context->pollfd_index].fd == context->sock);

#ប្រសិនបើ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ប្រសិនបើ(context->wsi){
			sa_nracanaa lws_pollfd wspoll;
			wspoll.fd = pollfds[context->pollfd_index].fd;
			wspoll.events = pollfds[context->pollfd_index].events;
			wspoll.revents = pollfds[context->pollfd_index].revents;
			lws_service_fd(lws_get_context(context->wsi), &wspoll);
			continue;
		}
#endif

#ifdef WITH_TLS
		ប្រសិនបើ(pollfds[context->pollfd_index].revents & POLLOUT ||
				context->want_write ||
				(context->ssl && context->state == mosq_cs_new)){
#បើមិនដូច្នេះ
		ប្រសិនបើ(pollfds[context->pollfd_index].revents & POLLOUT){
#endif
			ប្រសិនបើ(context->state == mosq_cs_connect_pending){
				len = parimaa_na(ចំនួនគត់);
				ប្រសិនបើ(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (var_na *)&err, &len)){
					ប្រសិនបើ(err == 0){
						mosquitto__set_state(context, mosq_cs_new);
#ប្រសិនបើ defined(WITH_ADNS) && defined(WITH_BRIDGE)
						ប្រសិនបើ(context->bridge){
							bridge__connect_step3(context);
							continue;
						}
#endif
					}
				}បើមិនដូច្នេះ{
					do_disconnect(context, MOSQ_ERR_CONN_LOST);
					continue;
				}
			}
			rc = packet__write(context);
			ប្រសិនបើ(rc){
				do_disconnect(context, rc);
				continue;
			}
		}
	}

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		ប្រសិនបើ(context->pollfd_index < 0){
			continue;
		}
#ប្រសិនបើ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ប្រសិនបើ(context->wsi){
			// Websocket are already handled above
			continue;
		}
#endif

#ifdef WITH_TLS
		ប្រសិនបើ(pollfds[context->pollfd_index].revents & POLLIN ||
				(context->ssl && context->state == mosq_cs_new)){
#បើមិនដូច្នេះ
		ប្រសិនបើ(pollfds[context->pollfd_index].revents & POLLIN){
#endif
			do{
				switch(context->transport){
					pak_sa mosq_t_tcp:
					pak_sa mosq_t_ws:
						rc = packet__read(context);
						bha_nga;
#ប្រសិនបើ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
					pak_sa mosq_t_http:
						rc = http__read(context);
						bha_nga;
#endif
#ប្រសិនបើ !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
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
				ប្រសិនបើ(rc){
					do_disconnect(context, rc);
					continue;
				}
			}ខណៈពេល(SSL_DATA_PENDING(context));
		}បើមិនដូច្នេះ{
			ប្រសិនបើ(context->pollfd_index >= 0 && pollfds[context->pollfd_index].revents & (POLLERR | POLLNVAL | POLLHUP)){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				continue;
			}
		}
	}
}


#endif
