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

#અન્તર્ભાવ "config.h"

#જો !defined(WITH_EPOLL) && !defined(WITH_KQUEUE)

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#અન્તર્ભાવ <assert.h>
#ifndef WIN32
#અન્તર્ભાવ <poll.h>
#અન્તર્ભાવ <unistd.h>
#નહીંતર
#અન્તર્ભાવ <process.h>
#અન્તર્ભાવ <winsock2.h>
#અન્તર્ભાવ <ws2tcpip.h>
#endif

#અન્તર્ભાવ <errno.h>
#અન્તર્ભાવ <signal.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>
#ifndef WIN32
#  include <sys/socket.h>
#endif
#અન્તર્ભાવ <time.h>

#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "packet_mosq.h"
#અન્તર્ભાવ "send_mosq.h"
#અન્તર્ભાવ "sys_tree.h"
#અન્તર્ભાવ "util_mosq.h"
#અન્તર્ભાવ "mux.h"

સ્થિર શૂન્ય loop_handle_reads_writes(શૂન્ય);

સ્થિર સંરચના pollfd *pollfds = ખાલી;
સ્થિર size_t pollfd_max, pollfd_current_max = 0;


પૂર્ણાંક mux_poll__init(શૂન્ય)
{
#ifdef WIN32
	pollfd_max = (size_t)_getmaxstdio();
#નહીંતર
	pollfd_max = (size_t)sysconf(_SC_OPEN_MAX);
#endif

	pollfds = mosquitto_calloc(pollfd_max, કદ(સંરચના pollfd));
	જો(!pollfds){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
		પાછા MOSQ_ERR_NOMEM;
	}
	memset(pollfds, 0, કદ(સંરચના pollfd)*pollfd_max);
	માટે(size_t i=0; i<pollfd_max; i++){
		pollfds[i].fd = INVALID_SOCKET;
	}

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક mux_poll__add_listeners(સંરચના mosquitto__listener_sock *listensock, પૂર્ણાંક listensock_ગણતરી)
{
	size_t pollfd_index = 0;

	માટે(size_t i=0; i<(size_t )listensock_ગણતરી; i++){
		pollfds[pollfd_index].fd = listensock[i].sock;
		pollfds[pollfd_index].events = POLLIN;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	જો(pollfd_current_max == 0){
		pollfd_current_max = pollfd_index-1;
	}
	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક mux_poll__delete_listeners(સંરચના mosquitto__listener_sock *listensock, પૂર્ણાંક listensock_ગણતરી)
{
	size_t pollfd_index = 0;

	UNUSED(listensock);

	માટે(size_t i=0; i<(size_t )listensock_ગણતરી; i++){
		pollfds[pollfd_index].fd = INVALID_SOCKET;
		pollfds[pollfd_index].events = 0;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક mux_poll__add(સંરચના mosquitto *context, uint16_t evt)
{
	જો(context->events == evt){
		પાછા MOSQ_ERR_SUCCESS;
	}

	જો(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = context->sock;
		pollfds[context->pollfd_index].events = (short પૂર્ણાંક)evt;
		pollfds[context->pollfd_index].revents = 0;
	}નહીંતર{
		માટે(size_t i=0; i<pollfd_max; i++){
			જો(pollfds[i].fd == INVALID_SOCKET){
				pollfds[i].fd = context->sock;
				pollfds[i].events = POLLIN;
				pollfds[i].revents = 0;
				context->pollfd_index = (પૂર્ણાંક)i;
				જો(i > pollfd_current_max){
					pollfd_current_max = i;
				}
				તોડો;
			}
		}
	}
	context->events = evt;

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક mux_poll__add_out(સંરચના mosquitto *context)
{
	પાછા mux_poll__add(context, POLLIN | POLLOUT);
}


પૂર્ણાંક mux_poll__remove_out(સંરચના mosquitto *context)
{
	જો(context->events & POLLOUT){
		પાછા mux_poll__new(context);
	}નહીંતર{
		પાછા MOSQ_ERR_SUCCESS;
	}
}


પૂર્ણાંક mux_poll__new(સંરચના mosquitto *context)
{
	પાછા mux_poll__add(context, POLLIN);
}


પૂર્ણાંક mux_poll__delete(સંરચના mosquitto *context)
{
	size_t pollfd_index;

	જો(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = INVALID_SOCKET;
		pollfds[context->pollfd_index].events = 0;
		pollfds[context->pollfd_index].revents = 0;
		pollfd_index = (size_t )context->pollfd_index;
		context->pollfd_index = -1;

		/* If this is the highest index, reduce the current max until we find
		 * the next highest in use index. */
		જ્યારે(pollfd_index == pollfd_current_max
				&& pollfd_index > 0
				&& pollfds[pollfd_index].fd == INVALID_SOCKET){

			pollfd_index--;
			pollfd_current_max--;
		}
	}

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક mux_poll__handle(સંરચના mosquitto__listener_sock *listensock, પૂર્ણાંક listensock_ગણતરી)
{
	સંરચના mosquitto *context;
	પૂર્ણાંક fdcount;
	પૂર્ણાંક timeout;

#ifdef WITH_WEBSOCKETS
	timeout = 100;
#નહીંતર
	timeout = db.next_event_ms;
#endif

#ifndef WIN32
	fdcount = poll(pollfds, pollfd_current_max+1, timeout);
#નહીંતર
	fdcount = WSAPoll(pollfds, pollfd_current_max+1, timeout);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(ખાલી);

	જો(fdcount == -1){
#  જો defined(WIN32) && defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		જો(WSAGetLastError() == WSAEINVAL){
			/* WSAPoll() immediately returns an error if it is not given
			 * any sockets to wait on. This can happen if we only have
			 * websockets listeners. Sleep a little to prevent a busy loop.
			 */
			Sleep(10);
		}નહીંતર
#  endif
		{
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error in poll: %s.", strerror(errno));
		}
	}નહીંતર{
		loop_handle_reads_writes();

		માટે(પૂર્ણાંક i=0; i<listensock_ગણતરી; i++){
			જો(pollfds[i].revents & POLLIN){
#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				જો(listensock[i].listener->ws_context){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
				}નહીંતર
#endif
				{
					જ્યારે((context = net__socket_accept(&listensock[i])) != ખાલી){
					}
				}
			}
		}
	}
	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક mux_poll__cleanup(શૂન્ય)
{
	mosquitto_FREE(pollfds);

	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર શૂન્ય loop_handle_reads_writes(શૂન્ય)
{
	સંરચના mosquitto *context, *ctxt_tmp;
	પૂર્ણાંક err;
	socklen_t len;
	પૂર્ણાંક rc;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		જો(context->pollfd_index < 0){
			ચાલુ;
		}

		જો(pollfds[context->pollfd_index].fd == INVALID_SOCKET){
			ચાલુ;
		}

		પ્રતિજ્ઞા(pollfds[context->pollfd_index].fd == context->sock);

#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		જો(context->wsi){
			સંરચના lws_pollfd wspoll;
			wspoll.fd = pollfds[context->pollfd_index].fd;
			wspoll.events = pollfds[context->pollfd_index].events;
			wspoll.revents = pollfds[context->pollfd_index].revents;
			lws_service_fd(lws_get_context(context->wsi), &wspoll);
			ચાલુ;
		}
#endif

#ifdef WITH_TLS
		જો(pollfds[context->pollfd_index].revents & POLLOUT ||
				context->want_write ||
				(context->ssl && context->state == mosq_cs_new)){
#નહીંતર
		જો(pollfds[context->pollfd_index].revents & POLLOUT){
#endif
			જો(context->state == mosq_cs_connect_pending){
				len = કદ(પૂર્ણાંક);
				જો(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (અક્ષર *)&err, &len)){
					જો(err == 0){
						mosquitto__set_state(context, mosq_cs_new);
#જો defined(WITH_ADNS) && defined(WITH_BRIDGE)
						જો(context->bridge){
							bridge__connect_step3(context);
							ચાલુ;
						}
#endif
					}
				}નહીંતર{
					do_disconnect(context, MOSQ_ERR_CONN_LOST);
					ચાલુ;
				}
			}
			rc = packet__write(context);
			જો(rc){
				do_disconnect(context, rc);
				ચાલુ;
			}
		}
	}

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		જો(context->pollfd_index < 0){
			ચાલુ;
		}
#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		જો(context->wsi){
			// Websocket are already handled above
			ચાલુ;
		}
#endif

#ifdef WITH_TLS
		જો(pollfds[context->pollfd_index].revents & POLLIN ||
				(context->ssl && context->state == mosq_cs_new)){
#નહીંતર
		જો(pollfds[context->pollfd_index].revents & POLLIN){
#endif
			કરો{
				પસંદગી(context->transport){
					સ્થિતિ mosq_t_tcp:
					સ્થિતિ mosq_t_ws:
						rc = packet__read(context);
						તોડો;
#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
					સ્થિતિ mosq_t_http:
						rc = http__read(context);
						તોડો;
#endif
#જો !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
					/* Not supported with LWS */
					સ્થિતિ mosq_t_proxy_v2:
						rc = proxy_v2__read(context);
						તોડો;
					સ્થિતિ mosq_t_proxy_v1:
						rc = proxy_v1__read(context);
						તોડો;
#endif
					મૂળભૂત:
						rc = MOSQ_ERR_INVAL;
						તોડો;
				}
				જો(rc){
					do_disconnect(context, rc);
					ચાલુ;
				}
			}જ્યારે(SSL_DATA_PENDING(context));
		}નહીંતર{
			જો(context->pollfd_index >= 0 && pollfds[context->pollfd_index].revents & (POLLERR | POLLNVAL | POLLHUP)){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				ચાલુ;
			}
		}
	}
}


#endif
