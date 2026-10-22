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

#ضمن "config.h"

#ܐܢ !defined(WITH_EPOLL) && !defined(WITH_KQUEUE)

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#ضمن <assert.h>
#ifndef WIN32
#ضمن <poll.h>
#ضمن <unistd.h>
#ܐܚܪܢܐ
#ضمن <process.h>
#ضمن <winsock2.h>
#ضمن <ws2tcpip.h>
#endif

#ضمن <errno.h>
#ضمن <signal.h>
#ضمن <stdio.h>
#ضمن <string.h>
#ifndef WIN32
#  include <sys/socket.h>
#endif
#ضمن <time.h>

#ܐܢ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif

#ضمن "mosquitto_broker_internal.h"
#ضمن "packet_mosq.h"
#ضمن "send_mosq.h"
#ضمن "sys_tree.h"
#ضمن "util_mosq.h"
#ضمن "mux.h"

ساكن فراغ loop_handle_reads_writes(فراغ);

ساكن بنية pollfd *pollfds = عدم;
ساكن size_t pollfd_max, pollfd_current_max = 0;


صحيح mux_poll__init(فراغ)
{
#ifdef WIN32
	pollfd_max = (size_t)_getmaxstdio();
#ܐܚܪܢܐ
	pollfd_max = (size_t)sysconf(_SC_OPEN_MAX);
#endif

	pollfds = mosquitto_calloc(pollfd_max, حجم(بنية pollfd));
	ܐܢ(!pollfds){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		ܦܢܐ MOSQ_ERR_NOMEM;
	}
	memset(pollfds, 0, حجم(بنية pollfd)*pollfd_max);
	لكل(size_t i=0; i<pollfd_max; i++){
		pollfds[i].fd = INVALID_SOCKET;
	}

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


صحيح mux_poll__add_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_count)
{
	size_t pollfd_index = 0;

	لكل(size_t i=0; i<(size_t )listensock_count; i++){
		pollfds[pollfd_index].fd = listensock[i].sock;
		pollfds[pollfd_index].events = POLLIN;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	ܐܢ(pollfd_current_max == 0){
		pollfd_current_max = pollfd_index-1;
	}
	ܦܢܐ MOSQ_ERR_SUCCESS;
}


صحيح mux_poll__delete_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_count)
{
	size_t pollfd_index = 0;

	UNUSED(listensock);

	لكل(size_t i=0; i<(size_t )listensock_count; i++){
		pollfds[pollfd_index].fd = INVALID_SOCKET;
		pollfds[pollfd_index].events = 0;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


ساكن صحيح mux_poll__add(بنية mosquitto *context, uint16_t evt)
{
	ܐܢ(context->events == evt){
		ܦܢܐ MOSQ_ERR_SUCCESS;
	}

	ܐܢ(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = context->sock;
		pollfds[context->pollfd_index].events = (short صحيح)evt;
		pollfds[context->pollfd_index].revents = 0;
	}ܐܚܪܢܐ{
		لكل(size_t i=0; i<pollfd_max; i++){
			ܐܢ(pollfds[i].fd == INVALID_SOCKET){
				pollfds[i].fd = context->sock;
				pollfds[i].events = POLLIN;
				pollfds[i].revents = 0;
				context->pollfd_index = (صحيح)i;
				ܐܢ(i > pollfd_current_max){
					pollfd_current_max = i;
				}
				اكسر;
			}
		}
	}
	context->events = evt;

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


صحيح mux_poll__add_out(بنية mosquitto *context)
{
	ܦܢܐ mux_poll__add(context, POLLIN | POLLOUT);
}


صحيح mux_poll__remove_out(بنية mosquitto *context)
{
	ܐܢ(context->events & POLLOUT){
		ܦܢܐ mux_poll__new(context);
	}ܐܚܪܢܐ{
		ܦܢܐ MOSQ_ERR_SUCCESS;
	}
}


صحيح mux_poll__new(بنية mosquitto *context)
{
	ܦܢܐ mux_poll__add(context, POLLIN);
}


صحيح mux_poll__delete(بنية mosquitto *context)
{
	size_t pollfd_index;

	ܐܢ(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = INVALID_SOCKET;
		pollfds[context->pollfd_index].events = 0;
		pollfds[context->pollfd_index].revents = 0;
		pollfd_index = (size_t )context->pollfd_index;
		context->pollfd_index = -1;

		/* If this is the highest index, reduce the current max until we find
		 * the next highest in use index. */
		ܟܡܐ(pollfd_index == pollfd_current_max
				&& pollfd_index > 0
				&& pollfds[pollfd_index].fd == INVALID_SOCKET){

			pollfd_index--;
			pollfd_current_max--;
		}
	}

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


صحيح mux_poll__handle(بنية mosquitto__listener_sock *listensock, صحيح listensock_count)
{
	بنية mosquitto *context;
	صحيح fdcount;
	صحيح timeout;

#ifdef WITH_WEBSOCKETS
	timeout = 100;
#ܐܚܪܢܐ
	timeout = db.next_event_ms;
#endif

#ifndef WIN32
	fdcount = poll(pollfds, pollfd_current_max+1, timeout);
#ܐܚܪܢܐ
	fdcount = WSAPoll(pollfds, pollfd_current_max+1, timeout);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(عدم);

	ܐܢ(fdcount == -1){
#  ܐܢ defined(WIN32) && defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ܐܢ(WSAGetLastError() == WSAEINVAL){
			/* WSAPoll() immediately returns an error if it is not given
			 * any sockets to wait on. This can happen if we only have
			 * websockets listeners. Sleep a little to prevent a busy loop.
			 */
			Sleep(10);
		}ܐܚܪܢܐ
#  endif
		{
			log__printf(عدم, MOSQ_LOG_ERR, "Error in poll: %s.", strerror(errno));
		}
	}ܐܚܪܢܐ{
		loop_handle_reads_writes();

		لكل(صحيح i=0; i<listensock_count; i++){
			ܐܢ(pollfds[i].revents & POLLIN){
#ܐܢ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				ܐܢ(listensock[i].listener->ws_context){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
				}ܐܚܪܢܐ
#endif
				{
					ܟܡܐ((context = net__socket_accept(&listensock[i])) != عدم){
					}
				}
			}
		}
	}
	ܦܢܐ MOSQ_ERR_SUCCESS;
}


صحيح mux_poll__cleanup(فراغ)
{
	mosquitto_FREE(pollfds);

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


ساكن فراغ loop_handle_reads_writes(فراغ)
{
	بنية mosquitto *context, *ctxt_tmp;
	صحيح err;
	socklen_t len;
	صحيح rc;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		ܐܢ(context->pollfd_index < 0){
			استمر;
		}

		ܐܢ(pollfds[context->pollfd_index].fd == INVALID_SOCKET){
			استمر;
		}

		أكد(pollfds[context->pollfd_index].fd == context->sock);

#ܐܢ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ܐܢ(context->wsi){
			بنية lws_pollfd wspoll;
			wspoll.fd = pollfds[context->pollfd_index].fd;
			wspoll.events = pollfds[context->pollfd_index].events;
			wspoll.revents = pollfds[context->pollfd_index].revents;
			lws_service_fd(lws_get_context(context->wsi), &wspoll);
			استمر;
		}
#endif

#ifdef WITH_TLS
		ܐܢ(pollfds[context->pollfd_index].revents & POLLOUT ||
				context->want_write ||
				(context->ssl && context->state == mosq_cs_new)){
#ܐܚܪܢܐ
		ܐܢ(pollfds[context->pollfd_index].revents & POLLOUT){
#endif
			ܐܢ(context->state == mosq_cs_connect_pending){
				len = حجم(صحيح);
				ܐܢ(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (حرف *)&err, &len)){
					ܐܢ(err == 0){
						mosquitto__set_state(context, mosq_cs_new);
#ܐܢ defined(WITH_ADNS) && defined(WITH_BRIDGE)
						ܐܢ(context->bridge){
							bridge__connect_step3(context);
							استمر;
						}
#endif
					}
				}ܐܚܪܢܐ{
					do_disconnect(context, MOSQ_ERR_CONN_LOST);
					استمر;
				}
			}
			rc = packet__write(context);
			ܐܢ(rc){
				do_disconnect(context, rc);
				استمر;
			}
		}
	}

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		ܐܢ(context->pollfd_index < 0){
			استمر;
		}
#ܐܢ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ܐܢ(context->wsi){
			// Websocket are already handled above
			استمر;
		}
#endif

#ifdef WITH_TLS
		ܐܢ(pollfds[context->pollfd_index].revents & POLLIN ||
				(context->ssl && context->state == mosq_cs_new)){
#ܐܚܪܢܐ
		ܐܢ(pollfds[context->pollfd_index].revents & POLLIN){
#endif
			ܥܒܕ{
				بدل(context->transport){
					حالة mosq_t_tcp:
					حالة mosq_t_ws:
						rc = packet__read(context);
						اكسر;
#ܐܢ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
					حالة mosq_t_http:
						rc = http__read(context);
						اكسر;
#endif
#ܐܢ !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
					/* Not supported with LWS */
					حالة mosq_t_proxy_v2:
						rc = proxy_v2__read(context);
						اكسر;
					حالة mosq_t_proxy_v1:
						rc = proxy_v1__read(context);
						اكسر;
#endif
					افتراضي:
						rc = MOSQ_ERR_INVAL;
						اكسر;
				}
				ܐܢ(rc){
					do_disconnect(context, rc);
					استمر;
				}
			}ܟܡܐ(SSL_DATA_PENDING(context));
		}ܐܚܪܢܐ{
			ܐܢ(context->pollfd_index >= 0 && pollfds[context->pollfd_index].revents & (POLLERR | POLLNVAL | POLLHUP)){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				استمر;
			}
		}
	}
}


#endif
