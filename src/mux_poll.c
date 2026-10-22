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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಯದಿ !defined(WITH_EPOLL) && !defined(WITH_KQUEUE)

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#ಅನ್ತರ್ಭಾವ <assert.h>
#ifndef WIN32
#ಅನ್ತರ್ಭಾವ <poll.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಅನ್ಯಥಾ
#ಅನ್ತರ್ಭಾವ <process.h>
#ಅನ್ತರ್ಭಾವ <winsock2.h>
#ಅನ್ತರ್ಭಾವ <ws2tcpip.h>
#endif

#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ifndef WIN32
#  include <sys/socket.h>
#endif
#ಅನ್ತರ್ಭಾವ <time.h>

#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "packet_mosq.h"
#ಅನ್ತರ್ಭಾವ "send_mosq.h"
#ಅನ್ತರ್ಭಾವ "sys_tree.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"
#ಅನ್ತರ್ಭಾವ "mux.h"

ಸ್ಥಿರ ಶೂನ್ಯ loop_handle_reads_writes(ಶೂನ್ಯ);

ಸ್ಥಿರ ಸಂರಚನಾ pollfd *pollfds = ರಿಕ್ತ;
ಸ್ಥಿರ size_t pollfd_max, pollfd_current_max = 0;


ಪೂರ್ಣಾಙ್ಕ mux_poll__init(ಶೂನ್ಯ)
{
#ifdef WIN32
	pollfd_max = (size_t)_getmaxstdio();
#ಅನ್ಯಥಾ
	pollfd_max = (size_t)sysconf(_SC_OPEN_MAX);
#endif

	pollfds = mosquitto_calloc(pollfd_max, ಪರಿಮಾಣ(ಸಂರಚನಾ pollfd));
	ಯದಿ(!pollfds){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}
	memset(pollfds, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ pollfd)*pollfd_max);
	ಪ್ರತಿ(size_t i=0; i<pollfd_max; i++){
		pollfds[i].fd = INVALID_SOCKET;
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ mux_poll__add_listeners(ಸಂರಚನಾ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಙ್ಕ listensock_ಎಣಿಕೆ)
{
	size_t pollfd_index = 0;

	ಪ್ರತಿ(size_t i=0; i<(size_t )listensock_ಎಣಿಕೆ; i++){
		pollfds[pollfd_index].fd = listensock[i].sock;
		pollfds[pollfd_index].events = POLLIN;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	ಯದಿ(pollfd_current_max == 0){
		pollfd_current_max = pollfd_index-1;
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ mux_poll__delete_listeners(ಸಂರಚನಾ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಙ್ಕ listensock_ಎಣಿಕೆ)
{
	size_t pollfd_index = 0;

	UNUSED(listensock);

	ಪ್ರತಿ(size_t i=0; i<(size_t )listensock_ಎಣಿಕೆ; i++){
		pollfds[pollfd_index].fd = INVALID_SOCKET;
		pollfds[pollfd_index].events = 0;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mux_poll__add(ಸಂರಚನಾ mosquitto *context, uint16_t evt)
{
	ಯದಿ(context->events == evt){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}

	ಯದಿ(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = context->sock;
		pollfds[context->pollfd_index].events = (short ಪೂರ್ಣಾಙ್ಕ)evt;
		pollfds[context->pollfd_index].revents = 0;
	}ಅನ್ಯಥಾ{
		ಪ್ರತಿ(size_t i=0; i<pollfd_max; i++){
			ಯದಿ(pollfds[i].fd == INVALID_SOCKET){
				pollfds[i].fd = context->sock;
				pollfds[i].events = POLLIN;
				pollfds[i].revents = 0;
				context->pollfd_index = (ಪೂರ್ಣಾಙ್ಕ)i;
				ಯದಿ(i > pollfd_current_max){
					pollfd_current_max = i;
				}
				ಭಙ್ಗ;
			}
		}
	}
	context->events = evt;

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ mux_poll__add_out(ಸಂರಚನಾ mosquitto *context)
{
	ಪ್ರತ್ಯಾಗಮ mux_poll__add(context, POLLIN | POLLOUT);
}


ಪೂರ್ಣಾಙ್ಕ mux_poll__remove_out(ಸಂರಚನಾ mosquitto *context)
{
	ಯದಿ(context->events & POLLOUT){
		ಪ್ರತ್ಯಾಗಮ mux_poll__new(context);
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}
}


ಪೂರ್ಣಾಙ್ಕ mux_poll__new(ಸಂರಚನಾ mosquitto *context)
{
	ಪ್ರತ್ಯಾಗಮ mux_poll__add(context, POLLIN);
}


ಪೂರ್ಣಾಙ್ಕ mux_poll__delete(ಸಂರಚನಾ mosquitto *context)
{
	size_t pollfd_index;

	ಯದಿ(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = INVALID_SOCKET;
		pollfds[context->pollfd_index].events = 0;
		pollfds[context->pollfd_index].revents = 0;
		pollfd_index = (size_t )context->pollfd_index;
		context->pollfd_index = -1;

		/* If this is the highest index, reduce the current max until we find
		 * the next highest in use index. */
		ಯಾವತ್(pollfd_index == pollfd_current_max
				&& pollfd_index > 0
				&& pollfds[pollfd_index].fd == INVALID_SOCKET){

			pollfd_index--;
			pollfd_current_max--;
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ mux_poll__handle(ಸಂರಚನಾ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಙ್ಕ listensock_ಎಣಿಕೆ)
{
	ಸಂರಚನಾ mosquitto *context;
	ಪೂರ್ಣಾಙ್ಕ fdcount;
	ಪೂರ್ಣಾಙ್ಕ timeout;

#ifdef WITH_WEBSOCKETS
	timeout = 100;
#ಅನ್ಯಥಾ
	timeout = db.next_event_ms;
#endif

#ifndef WIN32
	fdcount = poll(pollfds, pollfd_current_max+1, timeout);
#ಅನ್ಯಥಾ
	fdcount = WSAPoll(pollfds, pollfd_current_max+1, timeout);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(ರಿಕ್ತ);

	ಯದಿ(fdcount == -1){
#  ಯದಿ defined(WIN32) && defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ಯದಿ(WSAGetLastError() == WSAEINVAL){
			/* WSAPoll() immediately returns an error if it is not given
			 * any sockets to wait on. This can happen if we only have
			 * websockets listeners. Sleep a little to prevent a busy loop.
			 */
			Sleep(10);
		}ಅನ್ಯಥಾ
#  endif
		{
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error in poll: %s.", strerror(errno));
		}
	}ಅನ್ಯಥಾ{
		loop_handle_reads_writes();

		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<listensock_ಎಣಿಕೆ; i++){
			ಯದಿ(pollfds[i].revents & POLLIN){
#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				ಯದಿ(listensock[i].listener->ws_context){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
				}ಅನ್ಯಥಾ
#endif
				{
					ಯಾವತ್((context = net__socket_accept(&listensock[i])) != ರಿಕ್ತ){
					}
				}
			}
		}
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ mux_poll__cleanup(ಶೂನ್ಯ)
{
	mosquitto_FREE(pollfds);

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಶೂನ್ಯ loop_handle_reads_writes(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ mosquitto *context, *ctxt_tmp;
	ಪೂರ್ಣಾಙ್ಕ err;
	socklen_t len;
	ಪೂರ್ಣಾಙ್ಕ rc;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		ಯದಿ(context->pollfd_index < 0){
			ಅನುವರ್ತ;
		}

		ಯದಿ(pollfds[context->pollfd_index].fd == INVALID_SOCKET){
			ಅನುವರ್ತ;
		}

		ಪ್ರತಿಜ್ಞಾ(pollfds[context->pollfd_index].fd == context->sock);

#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ಯದಿ(context->wsi){
			ಸಂರಚನಾ lws_pollfd wspoll;
			wspoll.fd = pollfds[context->pollfd_index].fd;
			wspoll.events = pollfds[context->pollfd_index].events;
			wspoll.revents = pollfds[context->pollfd_index].revents;
			lws_service_fd(lws_get_context(context->wsi), &wspoll);
			ಅನುವರ್ತ;
		}
#endif

#ifdef WITH_TLS
		ಯದಿ(pollfds[context->pollfd_index].revents & POLLOUT ||
				context->want_write ||
				(context->ssl && context->state == mosq_cs_new)){
#ಅನ್ಯಥಾ
		ಯದಿ(pollfds[context->pollfd_index].revents & POLLOUT){
#endif
			ಯದಿ(context->state == mosq_cs_connect_pending){
				len = ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ);
				ಯದಿ(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (ವರ್ಣ *)&err, &len)){
					ಯದಿ(err == 0){
						mosquitto__set_state(context, mosq_cs_new);
#ಯದಿ defined(WITH_ADNS) && defined(WITH_BRIDGE)
						ಯದಿ(context->bridge){
							bridge__connect_step3(context);
							ಅನುವರ್ತ;
						}
#endif
					}
				}ಅನ್ಯಥಾ{
					do_disconnect(context, MOSQ_ERR_CONN_LOST);
					ಅನುವರ್ತ;
				}
			}
			rc = packet__write(context);
			ಯದಿ(rc){
				do_disconnect(context, rc);
				ಅನುವರ್ತ;
			}
		}
	}

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		ಯದಿ(context->pollfd_index < 0){
			ಅನುವರ್ತ;
		}
#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ಯದಿ(context->wsi){
			// Websocket are already handled above
			ಅನುವರ್ತ;
		}
#endif

#ifdef WITH_TLS
		ಯದಿ(pollfds[context->pollfd_index].revents & POLLIN ||
				(context->ssl && context->state == mosq_cs_new)){
#ಅನ್ಯಥಾ
		ಯದಿ(pollfds[context->pollfd_index].revents & POLLIN){
#endif
			ಕುರು{
				ವಿಕಲ್ಪ(context->transport){
					ಪಕ್ಷ mosq_t_tcp:
					ಪಕ್ಷ mosq_t_ws:
						rc = packet__read(context);
						ಭಙ್ಗ;
#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
					ಪಕ್ಷ mosq_t_http:
						rc = http__read(context);
						ಭಙ್ಗ;
#endif
#ಯದಿ !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
					/* Not supported with LWS */
					ಪಕ್ಷ mosq_t_proxy_v2:
						rc = proxy_v2__read(context);
						ಭಙ್ಗ;
					ಪಕ್ಷ mosq_t_proxy_v1:
						rc = proxy_v1__read(context);
						ಭಙ್ಗ;
#endif
					ಸ್ವಭಾವ:
						rc = MOSQ_ERR_INVAL;
						ಭಙ್ಗ;
				}
				ಯದಿ(rc){
					do_disconnect(context, rc);
					ಅನುವರ್ತ;
				}
			}ಯಾವತ್(SSL_DATA_PENDING(context));
		}ಅನ್ಯಥಾ{
			ಯದಿ(context->pollfd_index >= 0 && pollfds[context->pollfd_index].revents & (POLLERR | POLLNVAL | POLLHUP)){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				ಅನುವರ್ತ;
			}
		}
	}
}


#endif
