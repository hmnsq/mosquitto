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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਜੇ !defined(WITH_EPOLL) && !defined(WITH_KQUEUE)

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
#  define GNU_ਸਰੋਤ
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
#ਅਨ੍ਤਰ੍ਭਾਵ <poll.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਨਹੀਂਤਾਂ
#ਅਨ੍ਤਰ੍ਭਾਵ <process.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <winsock2.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <ws2tcpip.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
#  include <sys/socket.h>
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <time.h>

#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "packet_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "send_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sys_tree.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mux.h"

ਸਥਿਰ ਖਾਲੀ loop_handle_reads_writes(ਖਾਲੀ);

ਸਥਿਰ ਬਣਤਰ pollfd *pollfds = ਨਿਰਾ;
ਸਥਿਰ size_t pollfd_max, pollfd_current_max = 0;


ਪੂਰਨ mux_poll__init(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
	pollfd_max = (size_t)_getmaxstdio();
#ਨਹੀਂਤਾਂ
	pollfd_max = (size_t)sysconf(_SC_OPEN_MAX);
#ਜੇ_ਅੰਤ

	pollfds = mosquitto_calloc(pollfd_max, ਆਕਾਰ(ਬਣਤਰ pollfd));
	ਜੇ(!pollfds){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}
	memset(pollfds, 0, ਆਕਾਰ(ਬਣਤਰ pollfd)*pollfd_max);
	ਲਈ(size_t i=0; i<pollfd_max; i++){
		pollfds[i].fd = INVALID_SOCKET;
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ mux_poll_ਜੋੜੋ_listeners(ਬਣਤਰ mosquitto__listener_sock *listensock, ਪੂਰਨ listensock_ਗਿਣਤੀ)
{
	size_t pollfd_ਸੂਚਕਾਂਕ = 0;

	ਲਈ(size_t i=0; i<(size_t )listensock_ਗਿਣਤੀ; i++){
		pollfds[pollfd_ਸੂਚਕਾਂਕ].fd = listensock[i].sock;
		pollfds[pollfd_ਸੂਚਕਾਂਕ].events = POLLIN;
		pollfds[pollfd_ਸੂਚਕਾਂਕ].revents = 0;
		pollfd_ਸੂਚਕਾਂਕ++;
	}

	ਜੇ(pollfd_current_max == 0){
		pollfd_current_max = pollfd_ਸੂਚਕਾਂਕ-1;
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ mux_poll__delete_listeners(ਬਣਤਰ mosquitto__listener_sock *listensock, ਪੂਰਨ listensock_ਗਿਣਤੀ)
{
	size_t pollfd_ਸੂਚਕਾਂਕ = 0;

	ਅਣਵਰਤਿਆ(listensock);

	ਲਈ(size_t i=0; i<(size_t )listensock_ਗਿਣਤੀ; i++){
		pollfds[pollfd_ਸੂਚਕਾਂਕ].fd = INVALID_SOCKET;
		pollfds[pollfd_ਸੂਚਕਾਂਕ].events = 0;
		pollfds[pollfd_ਸੂਚਕਾਂਕ].revents = 0;
		pollfd_ਸੂਚਕਾਂਕ++;
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ mux_poll_ਜੋੜੋ(ਬਣਤਰ mosquitto *context, uint16_t evt)
{
	ਜੇ(context->events == evt){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

	ਜੇ(context->pollfd_ਸੂਚਕਾਂਕ != -1){
		pollfds[context->pollfd_ਸੂਚਕਾਂਕ].fd = context->sock;
		pollfds[context->pollfd_ਸੂਚਕਾਂਕ].events = (ਛੋਟਾ ਪੂਰਨ)evt;
		pollfds[context->pollfd_ਸੂਚਕਾਂਕ].revents = 0;
	}ਨਹੀਂਤਾਂ{
		ਲਈ(size_t i=0; i<pollfd_max; i++){
			ਜੇ(pollfds[i].fd == INVALID_SOCKET){
				pollfds[i].fd = context->sock;
				pollfds[i].events = POLLIN;
				pollfds[i].revents = 0;
				context->pollfd_ਸੂਚਕਾਂਕ = (ਪੂਰਨ)i;
				ਜੇ(i > pollfd_current_max){
					pollfd_current_max = i;
				}
				ਰੋਕੋ;
			}
		}
	}
	context->events = evt;

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ mux_poll_ਜੋੜੋ_ਬਾਹਰਲਾ(ਬਣਤਰ mosquitto *context)
{
	ਵਾਪਸ mux_poll_ਜੋੜੋ(context, POLLIN | POLLOUT);
}


ਪੂਰਨ mux_poll_remove_ਬਾਹਰਲਾ(ਬਣਤਰ mosquitto *context)
{
	ਜੇ(context->events & POLLOUT){
		ਵਾਪਸ mux_poll__new(context);
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}
}


ਪੂਰਨ mux_poll__new(ਬਣਤਰ mosquitto *context)
{
	ਵਾਪਸ mux_poll_ਜੋੜੋ(context, POLLIN);
}


ਪੂਰਨ mux_poll__delete(ਬਣਤਰ mosquitto *context)
{
	size_t pollfd_ਸੂਚਕਾਂਕ;

	ਜੇ(context->pollfd_ਸੂਚਕਾਂਕ != -1){
		pollfds[context->pollfd_ਸੂਚਕਾਂਕ].fd = INVALID_SOCKET;
		pollfds[context->pollfd_ਸੂਚਕਾਂਕ].events = 0;
		pollfds[context->pollfd_ਸੂਚਕਾਂਕ].revents = 0;
		pollfd_ਸੂਚਕਾਂਕ = (size_t )context->pollfd_ਸੂਚਕਾਂਕ;
		context->pollfd_ਸੂਚਕਾਂਕ = -1;

		/* If this is the highest index, reduce the current max until we find
		 * the next highest in use index. */
		ਜਦੋਂ(pollfd_ਸੂਚਕਾਂਕ == pollfd_current_max
				&& pollfd_ਸੂਚਕਾਂਕ > 0
				&& pollfds[pollfd_ਸੂਚਕਾਂਕ].fd == INVALID_SOCKET){

			pollfd_ਸੂਚਕਾਂਕ--;
			pollfd_current_max--;
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ mux_poll__handle(ਬਣਤਰ mosquitto__listener_sock *listensock, ਪੂਰਨ listensock_ਗਿਣਤੀ)
{
	ਬਣਤਰ mosquitto *context;
	ਪੂਰਨ fdcount;
	ਪੂਰਨ timeout;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_WEBSOCKETS
	timeout = 100;
#ਨਹੀਂਤਾਂ
	timeout = db.ਅਗਲਾ_event_ms;
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
	fdcount = poll(pollfds, pollfd_current_max+1, timeout);
#ਨਹੀਂਤਾਂ
	fdcount = WSAPoll(pollfds, pollfd_current_max+1, timeout);
#ਜੇ_ਅੰਤ

	db.now_s = mosquitto_ਸਮਾਂ();
	db.now_real_s = ਸਮਾਂ(ਨਿਰਾ);

	ਜੇ(fdcount == -1){
#  ਜੇ defined(WIN32) && defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ਜੇ(WSAGet_ਆਖਰੀ_Error() == WSAEINVAL){
			/* WSAPoll() immediately returns an error if it is not given
			 * any sockets to wait on. This can happen if we only have
			 * websockets listeners. Sleep a little to prevent a busy loop.
			 */
			Sleep(10);
		}ਨਹੀਂਤਾਂ
#  endif
		{
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error in poll: %s.", strerror(errno));
		}
	}ਨਹੀਂਤਾਂ{
		loop_handle_reads_writes();

		ਲਈ(ਪੂਰਨ i=0; i<listensock_ਗਿਣਤੀ; i++){
			ਜੇ(pollfds[i].revents & POLLIN){
#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				ਜੇ(listensock[i].listener->ws_context){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
				}ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
				{
					ਜਦੋਂ((context = net__socket_accept(&listensock[i])) != ਨਿਰਾ){
					}
				}
			}
		}
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ mux_poll__cleanup(ਖਾਲੀ)
{
	mosquitto_FREE(pollfds);

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਖਾਲੀ loop_handle_reads_writes(ਖਾਲੀ)
{
	ਬਣਤਰ mosquitto *context, *ctxt_tmp;
	ਪੂਰਨ err;
	socklen_t len;
	ਪੂਰਨ rc;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		ਜੇ(context->pollfd_ਸੂਚਕਾਂਕ < 0){
			ਜਾਰੀ;
		}

		ਜੇ(pollfds[context->pollfd_ਸੂਚਕਾਂਕ].fd == INVALID_SOCKET){
			ਜਾਰੀ;
		}

		ਪ੍ਰਤਿਜ੍ਞਾ(pollfds[context->pollfd_ਸੂਚਕਾਂਕ].fd == context->sock);

#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ਜੇ(context->wsi){
			ਬਣਤਰ lws_pollfd wspoll;
			wspoll.fd = pollfds[context->pollfd_ਸੂਚਕਾਂਕ].fd;
			wspoll.events = pollfds[context->pollfd_ਸੂਚਕਾਂਕ].events;
			wspoll.revents = pollfds[context->pollfd_ਸੂਚਕਾਂਕ].revents;
			lws_service_fd(lws_get_context(context->wsi), &wspoll);
			ਜਾਰੀ;
		}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
		ਜੇ(pollfds[context->pollfd_ਸੂਚਕਾਂਕ].revents & POLLOUT ||
				context->want_write ||
				(context->ssl && context->state == mosq_cs_new)){
#ਨਹੀਂਤਾਂ
		ਜੇ(pollfds[context->pollfd_ਸੂਚਕਾਂਕ].revents & POLLOUT){
#ਜੇ_ਅੰਤ
			ਜੇ(context->state == mosq_cs_connect_pending){
				len = ਆਕਾਰ(ਪੂਰਨ);
				ਜੇ(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (ਅੱਖਰ *)&err, &len)){
					ਜੇ(err == 0){
						mosquitto_ਸੈੱਟ_state(context, mosq_cs_new);
#ਜੇ defined(WITH_ADNS) && defined(WITH_BRIDGE)
						ਜੇ(context->bridge){
							bridge__connect_step3(context);
							ਜਾਰੀ;
						}
#ਜੇ_ਅੰਤ
					}
				}ਨਹੀਂਤਾਂ{
					do_disconnect(context, MOSQ_ERR_CONN_ਗੁਆਚਾ);
					ਜਾਰੀ;
				}
			}
			rc = packet__write(context);
			ਜੇ(rc){
				do_disconnect(context, rc);
				ਜਾਰੀ;
			}
		}
	}

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		ਜੇ(context->pollfd_ਸੂਚਕਾਂਕ < 0){
			ਜਾਰੀ;
		}
#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ਜੇ(context->wsi){
			// Websocket are already handled above
			ਜਾਰੀ;
		}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
		ਜੇ(pollfds[context->pollfd_ਸੂਚਕਾਂਕ].revents & POLLIN ||
				(context->ssl && context->state == mosq_cs_new)){
#ਨਹੀਂਤਾਂ
		ਜੇ(pollfds[context->pollfd_ਸੂਚਕਾਂਕ].revents & POLLIN){
#ਜੇ_ਅੰਤ
			ਕਰੋ{
				ਚੋਣ(context->transport){
					ਹਾਲਤ mosq_t_tcp:
					ਹਾਲਤ mosq_t_ws:
						rc = packet__read(context);
						ਰੋਕੋ;
#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
					ਹਾਲਤ mosq_t_http:
						rc = http__read(context);
						ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
					/* Not supported with LWS */
					ਹਾਲਤ mosq_t_proxy_v2:
						rc = proxy_v2__read(context);
						ਰੋਕੋ;
					ਹਾਲਤ mosq_t_proxy_v1:
						rc = proxy_v1__read(context);
						ਰੋਕੋ;
#ਜੇ_ਅੰਤ
					ਮੂਲ:
						rc = MOSQ_ERR_INVAL;
						ਰੋਕੋ;
				}
				ਜੇ(rc){
					do_disconnect(context, rc);
					ਜਾਰੀ;
				}
			}ਜਦੋਂ(SSL_ਅੰਕੜੇ_PENDING(context));
		}ਨਹੀਂਤਾਂ{
			ਜੇ(context->pollfd_ਸੂਚਕਾਂਕ >= 0 && pollfds[context->pollfd_ਸੂਚਕਾਂਕ].revents & (POLLERR | POLLNVAL | POLLHUP)){
				do_disconnect(context, MOSQ_ERR_CONN_ਗੁਆਚਾ);
				ਜਾਰੀ;
			}
		}
	}
}


#ਜੇ_ਅੰਤ
