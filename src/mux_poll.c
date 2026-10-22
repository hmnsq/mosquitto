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

#ಆದರೆ !defined(WITH_EPOLL) && !defined(WITH_KQUEUE)

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
#  define GNU_ಮೂಲ
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <assert.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
#ಅನ್ತರ್ಭಾವ <poll.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಇಲ್ಲದಿದ್ದರೆ
#ಅನ್ತರ್ಭಾವ <process.h>
#ಅನ್ತರ್ಭಾವ <winsock2.h>
#ಅನ್ತರ್ಭಾವ <ws2tcpip.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
#  include <sys/socket.h>
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <time.h>

#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "packet_mosq.h"
#ಅನ್ತರ್ಭಾವ "send_mosq.h"
#ಅನ್ತರ್ಭಾವ "sys_tree.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"
#ಅನ್ತರ್ಭಾವ "mux.h"

ಸ್ಥಿರ ಶೂನ್ಯ loop_handle_reads_writes(ಶೂನ್ಯ);

ಸ್ಥಿರ ರಚನೆ pollfd *pollfds = ಖಾಲಿ;
ಸ್ಥಿರ size_t pollfd_max, pollfd_current_max = 0;


ಪೂರ್ಣಾಂಕ mux_poll__init(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
	pollfd_max = (size_t)_getmaxstdio();
#ಇಲ್ಲದಿದ್ದರೆ
	pollfd_max = (size_t)sysconf(_SC_OPEN_MAX);
#ಆದರೆ_ಅಂತ್ಯ

	pollfds = mosquitto_calloc(pollfd_max, ಗಾತ್ರ(ರಚನೆ pollfd));
	ಆದರೆ(!pollfds){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}
	memset(pollfds, 0, ಗಾತ್ರ(ರಚನೆ pollfd)*pollfd_max);
	ಗಾಗಿ(size_t i=0; i<pollfd_max; i++){
		pollfds[i].fd = INVALID_SOCKET;
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ mux_poll_ಕೂಡಿಸು_listeners(ರಚನೆ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಂಕ listensock_ಎಣಿಕೆ)
{
	size_t pollfd_ಸೂಚ್ಯಂಕ = 0;

	ಗಾಗಿ(size_t i=0; i<(size_t )listensock_ಎಣಿಕೆ; i++){
		pollfds[pollfd_ಸೂಚ್ಯಂಕ].fd = listensock[i].sock;
		pollfds[pollfd_ಸೂಚ್ಯಂಕ].events = POLLIN;
		pollfds[pollfd_ಸೂಚ್ಯಂಕ].revents = 0;
		pollfd_ಸೂಚ್ಯಂಕ++;
	}

	ಆದರೆ(pollfd_current_max == 0){
		pollfd_current_max = pollfd_ಸೂಚ್ಯಂಕ-1;
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ mux_poll__delete_listeners(ರಚನೆ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಂಕ listensock_ಎಣಿಕೆ)
{
	size_t pollfd_ಸೂಚ್ಯಂಕ = 0;

	ಬಳಸದ(listensock);

	ಗಾಗಿ(size_t i=0; i<(size_t )listensock_ಎಣಿಕೆ; i++){
		pollfds[pollfd_ಸೂಚ್ಯಂಕ].fd = INVALID_SOCKET;
		pollfds[pollfd_ಸೂಚ್ಯಂಕ].events = 0;
		pollfds[pollfd_ಸೂಚ್ಯಂಕ].revents = 0;
		pollfd_ಸೂಚ್ಯಂಕ++;
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mux_poll_ಕೂಡಿಸು(ರಚನೆ mosquitto *context, uint16_t evt)
{
	ಆದರೆ(context->events == evt){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}

	ಆದರೆ(context->pollfd_ಸೂಚ್ಯಂಕ != -1){
		pollfds[context->pollfd_ಸೂಚ್ಯಂಕ].fd = context->sock;
		pollfds[context->pollfd_ಸೂಚ್ಯಂಕ].events = (ಗಿಡ್ಡ ಪೂರ್ಣಾಂಕ)evt;
		pollfds[context->pollfd_ಸೂಚ್ಯಂಕ].revents = 0;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಗಾಗಿ(size_t i=0; i<pollfd_max; i++){
			ಆದರೆ(pollfds[i].fd == INVALID_SOCKET){
				pollfds[i].fd = context->sock;
				pollfds[i].events = POLLIN;
				pollfds[i].revents = 0;
				context->pollfd_ಸೂಚ್ಯಂಕ = (ಪೂರ್ಣಾಂಕ)i;
				ಆದರೆ(i > pollfd_current_max){
					pollfd_current_max = i;
				}
				ನಿಲ್ಲಿಸು;
			}
		}
	}
	context->events = evt;

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ mux_poll_ಕೂಡಿಸು_ಹೊರಹೋಗು(ರಚನೆ mosquitto *context)
{
	ಹಿಂತಿರುಗಿ mux_poll_ಕೂಡಿಸು(context, POLLIN | POLLOUT);
}


ಪೂರ್ಣಾಂಕ mux_poll_remove_ಹೊರಹೋಗು(ರಚನೆ mosquitto *context)
{
	ಆದರೆ(context->events & POLLOUT){
		ಹಿಂತಿರುಗಿ mux_poll__new(context);
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}
}


ಪೂರ್ಣಾಂಕ mux_poll__new(ರಚನೆ mosquitto *context)
{
	ಹಿಂತಿರುಗಿ mux_poll_ಕೂಡಿಸು(context, POLLIN);
}


ಪೂರ್ಣಾಂಕ mux_poll__delete(ರಚನೆ mosquitto *context)
{
	size_t pollfd_ಸೂಚ್ಯಂಕ;

	ಆದರೆ(context->pollfd_ಸೂಚ್ಯಂಕ != -1){
		pollfds[context->pollfd_ಸೂಚ್ಯಂಕ].fd = INVALID_SOCKET;
		pollfds[context->pollfd_ಸೂಚ್ಯಂಕ].events = 0;
		pollfds[context->pollfd_ಸೂಚ್ಯಂಕ].revents = 0;
		pollfd_ಸೂಚ್ಯಂಕ = (size_t )context->pollfd_ಸೂಚ್ಯಂಕ;
		context->pollfd_ಸೂಚ್ಯಂಕ = -1;

		/* If this is the highest index, reduce the current max until we find
		 * the next highest in use index. */
		ಆಗ(pollfd_ಸೂಚ್ಯಂಕ == pollfd_current_max
				&& pollfd_ಸೂಚ್ಯಂಕ > 0
				&& pollfds[pollfd_ಸೂಚ್ಯಂಕ].fd == INVALID_SOCKET){

			pollfd_ಸೂಚ್ಯಂಕ--;
			pollfd_current_max--;
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ mux_poll__handle(ರಚನೆ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಂಕ listensock_ಎಣಿಕೆ)
{
	ರಚನೆ mosquitto *context;
	ಪೂರ್ಣಾಂಕ fdcount;
	ಪೂರ್ಣಾಂಕ timeout;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_WEBSOCKETS
	timeout = 100;
#ಇಲ್ಲದಿದ್ದರೆ
	timeout = db.ಮುಂದಿನ_event_ms;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
	fdcount = poll(pollfds, pollfd_current_max+1, timeout);
#ಇಲ್ಲದಿದ್ದರೆ
	fdcount = WSAPoll(pollfds, pollfd_current_max+1, timeout);
#ಆದರೆ_ಅಂತ್ಯ

	db.now_s = mosquitto_ಸಮಯ();
	db.now_real_s = ಸಮಯ(ಖಾಲಿ);

	ಆದರೆ(fdcount == -1){
#  ಆದರೆ defined(WIN32) && defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ಆದರೆ(WSAGet_ಕೊನೆಯ_Error() == WSAEINVAL){
			/* WSAPoll() immediately returns an error if it is not given
			 * any sockets to wait on. This can happen if we only have
			 * websockets listeners. Sleep a little to prevent a busy loop.
			 */
			Sleep(10);
		}ಇಲ್ಲದಿದ್ದರೆ
#  endif
		{
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error in poll: %s.", strerror(errno));
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		loop_handle_reads_writes();

		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<listensock_ಎಣಿಕೆ; i++){
			ಆದರೆ(pollfds[i].revents & POLLIN){
#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				ಆದರೆ(listensock[i].listener->ws_context){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
				}ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
				{
					ಆಗ((context = net__socket_accept(&listensock[i])) != ಖಾಲಿ){
					}
				}
			}
		}
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ mux_poll__cleanup(ಶೂನ್ಯ)
{
	mosquitto_FREE(pollfds);

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಶೂನ್ಯ loop_handle_reads_writes(ಶೂನ್ಯ)
{
	ರಚನೆ mosquitto *context, *ctxt_tmp;
	ಪೂರ್ಣಾಂಕ err;
	socklen_t len;
	ಪೂರ್ಣಾಂಕ rc;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		ಆದರೆ(context->pollfd_ಸೂಚ್ಯಂಕ < 0){
			ಮುಂದುವರಿ;
		}

		ಆದರೆ(pollfds[context->pollfd_ಸೂಚ್ಯಂಕ].fd == INVALID_SOCKET){
			ಮುಂದುವರಿ;
		}

		ಪ್ರತಿಜ್ಞಾ(pollfds[context->pollfd_ಸೂಚ್ಯಂಕ].fd == context->sock);

#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ಆದರೆ(context->wsi){
			ರಚನೆ lws_pollfd wspoll;
			wspoll.fd = pollfds[context->pollfd_ಸೂಚ್ಯಂಕ].fd;
			wspoll.events = pollfds[context->pollfd_ಸೂಚ್ಯಂಕ].events;
			wspoll.revents = pollfds[context->pollfd_ಸೂಚ್ಯಂಕ].revents;
			lws_service_fd(lws_get_context(context->wsi), &wspoll);
			ಮುಂದುವರಿ;
		}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
		ಆದರೆ(pollfds[context->pollfd_ಸೂಚ್ಯಂಕ].revents & POLLOUT ||
				context->want_write ||
				(context->ssl && context->state == mosq_cs_new)){
#ಇಲ್ಲದಿದ್ದರೆ
		ಆದರೆ(pollfds[context->pollfd_ಸೂಚ್ಯಂಕ].revents & POLLOUT){
#ಆದರೆ_ಅಂತ್ಯ
			ಆದರೆ(context->state == mosq_cs_connect_pending){
				len = ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ);
				ಆದರೆ(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (ಅಕ್ಷರ *)&err, &len)){
					ಆದರೆ(err == 0){
						mosquitto_ಗಣ_state(context, mosq_cs_new);
#ಆದರೆ defined(WITH_ADNS) && defined(WITH_BRIDGE)
						ಆದರೆ(context->bridge){
							bridge__connect_step3(context);
							ಮುಂದುವರಿ;
						}
#ಆದರೆ_ಅಂತ್ಯ
					}
				}ಇಲ್ಲದಿದ್ದರೆ{
					do_disconnect(context, MOSQ_ERR_CONN_ಕಳೆದುಹೋದ);
					ಮುಂದುವರಿ;
				}
			}
			rc = packet__write(context);
			ಆದರೆ(rc){
				do_disconnect(context, rc);
				ಮುಂದುವರಿ;
			}
		}
	}

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		ಆದರೆ(context->pollfd_ಸೂಚ್ಯಂಕ < 0){
			ಮುಂದುವರಿ;
		}
#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ಆದರೆ(context->wsi){
			// Websocket are already handled above
			ಮುಂದುವರಿ;
		}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
		ಆದರೆ(pollfds[context->pollfd_ಸೂಚ್ಯಂಕ].revents & POLLIN ||
				(context->ssl && context->state == mosq_cs_new)){
#ಇಲ್ಲದಿದ್ದರೆ
		ಆದರೆ(pollfds[context->pollfd_ಸೂಚ್ಯಂಕ].revents & POLLIN){
#ಆದರೆ_ಅಂತ್ಯ
			ಮಾಡು{
				ಆಯ್ಕೆ(context->transport){
					ಸ್ಥಿತಿ mosq_t_tcp:
					ಸ್ಥಿತಿ mosq_t_ws:
						rc = packet__read(context);
						ನಿಲ್ಲಿಸು;
#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
					ಸ್ಥಿತಿ mosq_t_http:
						rc = http__read(context);
						ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
					/* Not supported with LWS */
					ಸ್ಥಿತಿ mosq_t_proxy_v2:
						rc = proxy_v2__read(context);
						ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ mosq_t_proxy_v1:
						rc = proxy_v1__read(context);
						ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
					ಪೂರ್ವನಿಯೋಜಿತ:
						rc = MOSQ_ERR_INVAL;
						ನಿಲ್ಲಿಸು;
				}
				ಆದರೆ(rc){
					do_disconnect(context, rc);
					ಮುಂದುವರಿ;
				}
			}ಆಗ(SSL_ದತ್ತಾಂಶ_PENDING(context));
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಆದರೆ(context->pollfd_ಸೂಚ್ಯಂಕ >= 0 && pollfds[context->pollfd_ಸೂಚ್ಯಂಕ].revents & (POLLERR | POLLNVAL | POLLHUP)){
				do_disconnect(context, MOSQ_ERR_CONN_ಕಳೆದುಹೋದ);
				ಮುಂದುವರಿ;
			}
		}
	}
}


#ಆದರೆ_ಅಂತ್ಯ
