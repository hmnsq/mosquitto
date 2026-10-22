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

#அந்தர்பாவ "config.h"

#எனில் !defined(WITH_EPOLL) && !defined(WITH_KQUEUE)

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#அந்தர்பாவ <assert.h>
#ifndef WIN32
#அந்தர்பாவ <poll.h>
#அந்தர்பாவ <unistd.h>
#இல்லையெனில்
#அந்தர்பாவ <process.h>
#அந்தர்பாவ <winsock2.h>
#அந்தர்பாவ <ws2tcpip.h>
#endif

#அந்தர்பாவ <errno.h>
#அந்தர்பாவ <signal.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>
#ifndef WIN32
#  include <sys/socket.h>
#endif
#அந்தர்பாவ <time.h>

#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "packet_mosq.h"
#அந்தர்பாவ "send_mosq.h"
#அந்தர்பாவ "sys_tree.h"
#அந்தர்பாவ "util_mosq.h"
#அந்தர்பாவ "mux.h"

நிலையான வெறுமை loop_handle_reads_writes(வெறுமை);

நிலையான கட்டமைப்பு pollfd *pollfds = வெற்று;
நிலையான size_t pollfd_max, pollfd_current_max = 0;


முழு mux_poll__init(வெறுமை)
{
#ifdef WIN32
	pollfd_max = (size_t)_getmaxstdio();
#இல்லையெனில்
	pollfd_max = (size_t)sysconf(_SC_OPEN_MAX);
#endif

	pollfds = mosquitto_calloc(pollfd_max, அளவு(கட்டமைப்பு pollfd));
	எனில்(!pollfds){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
		திரும்பு MOSQ_ERR_NOMEM;
	}
	memset(pollfds, 0, அளவு(கட்டமைப்பு pollfd)*pollfd_max);
	க்கு(size_t i=0; i<pollfd_max; i++){
		pollfds[i].fd = INVALID_SOCKET;
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு mux_poll__add_listeners(கட்டமைப்பு mosquitto__listener_sock *listensock, முழு listensock_மொத்தம்)
{
	size_t pollfd_index = 0;

	க்கு(size_t i=0; i<(size_t )listensock_மொத்தம்; i++){
		pollfds[pollfd_index].fd = listensock[i].sock;
		pollfds[pollfd_index].events = POLLIN;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	எனில்(pollfd_current_max == 0){
		pollfd_current_max = pollfd_index-1;
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு mux_poll__delete_listeners(கட்டமைப்பு mosquitto__listener_sock *listensock, முழு listensock_மொத்தம்)
{
	size_t pollfd_index = 0;

	UNUSED(listensock);

	க்கு(size_t i=0; i<(size_t )listensock_மொத்தம்; i++){
		pollfds[pollfd_index].fd = INVALID_SOCKET;
		pollfds[pollfd_index].events = 0;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு mux_poll__add(கட்டமைப்பு mosquitto *context, uint16_t evt)
{
	எனில்(context->events == evt){
		திரும்பு MOSQ_ERR_SUCCESS;
	}

	எனில்(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = context->sock;
		pollfds[context->pollfd_index].events = (short முழு)evt;
		pollfds[context->pollfd_index].revents = 0;
	}இல்லையெனில்{
		க்கு(size_t i=0; i<pollfd_max; i++){
			எனில்(pollfds[i].fd == INVALID_SOCKET){
				pollfds[i].fd = context->sock;
				pollfds[i].events = POLLIN;
				pollfds[i].revents = 0;
				context->pollfd_index = (முழு)i;
				எனில்(i > pollfd_current_max){
					pollfd_current_max = i;
				}
				நிறுத்து;
			}
		}
	}
	context->events = evt;

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு mux_poll__add_out(கட்டமைப்பு mosquitto *context)
{
	திரும்பு mux_poll__add(context, POLLIN | POLLOUT);
}


முழு mux_poll__remove_out(கட்டமைப்பு mosquitto *context)
{
	எனில்(context->events & POLLOUT){
		திரும்பு mux_poll__new(context);
	}இல்லையெனில்{
		திரும்பு MOSQ_ERR_SUCCESS;
	}
}


முழு mux_poll__new(கட்டமைப்பு mosquitto *context)
{
	திரும்பு mux_poll__add(context, POLLIN);
}


முழு mux_poll__delete(கட்டமைப்பு mosquitto *context)
{
	size_t pollfd_index;

	எனில்(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = INVALID_SOCKET;
		pollfds[context->pollfd_index].events = 0;
		pollfds[context->pollfd_index].revents = 0;
		pollfd_index = (size_t )context->pollfd_index;
		context->pollfd_index = -1;

		/* If this is the highest index, reduce the current max until we find
		 * the next highest in use index. */
		போது(pollfd_index == pollfd_current_max
				&& pollfd_index > 0
				&& pollfds[pollfd_index].fd == INVALID_SOCKET){

			pollfd_index--;
			pollfd_current_max--;
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு mux_poll__handle(கட்டமைப்பு mosquitto__listener_sock *listensock, முழு listensock_மொத்தம்)
{
	கட்டமைப்பு mosquitto *context;
	முழு fdcount;
	முழு timeout;

#ifdef WITH_WEBSOCKETS
	timeout = 100;
#இல்லையெனில்
	timeout = db.next_event_ms;
#endif

#ifndef WIN32
	fdcount = poll(pollfds, pollfd_current_max+1, timeout);
#இல்லையெனில்
	fdcount = WSAPoll(pollfds, pollfd_current_max+1, timeout);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(வெற்று);

	எனில்(fdcount == -1){
#  எனில் defined(WIN32) && defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		எனில்(WSAGetLastError() == WSAEINVAL){
			/* WSAPoll() immediately returns an error if it is not given
			 * any sockets to wait on. This can happen if we only have
			 * websockets listeners. Sleep a little to prevent a busy loop.
			 */
			Sleep(10);
		}இல்லையெனில்
#  endif
		{
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error in poll: %s.", strerror(errno));
		}
	}இல்லையெனில்{
		loop_handle_reads_writes();

		க்கு(முழு i=0; i<listensock_மொத்தம்; i++){
			எனில்(pollfds[i].revents & POLLIN){
#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				எனில்(listensock[i].listener->ws_context){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
				}இல்லையெனில்
#endif
				{
					போது((context = net__socket_accept(&listensock[i])) != வெற்று){
					}
				}
			}
		}
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு mux_poll__cleanup(வெறுமை)
{
	mosquitto_FREE(pollfds);

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான வெறுமை loop_handle_reads_writes(வெறுமை)
{
	கட்டமைப்பு mosquitto *context, *ctxt_tmp;
	முழு err;
	socklen_t len;
	முழு rc;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		எனில்(context->pollfd_index < 0){
			தொடர்;
		}

		எனில்(pollfds[context->pollfd_index].fd == INVALID_SOCKET){
			தொடர்;
		}

		ப்ரதிஜ்ஞா(pollfds[context->pollfd_index].fd == context->sock);

#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		எனில்(context->wsi){
			கட்டமைப்பு lws_pollfd wspoll;
			wspoll.fd = pollfds[context->pollfd_index].fd;
			wspoll.events = pollfds[context->pollfd_index].events;
			wspoll.revents = pollfds[context->pollfd_index].revents;
			lws_service_fd(lws_get_context(context->wsi), &wspoll);
			தொடர்;
		}
#endif

#ifdef WITH_TLS
		எனில்(pollfds[context->pollfd_index].revents & POLLOUT ||
				context->want_write ||
				(context->ssl && context->state == mosq_cs_new)){
#இல்லையெனில்
		எனில்(pollfds[context->pollfd_index].revents & POLLOUT){
#endif
			எனில்(context->state == mosq_cs_connect_pending){
				len = அளவு(முழு);
				எனில்(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (எழுத்து *)&err, &len)){
					எனில்(err == 0){
						mosquitto__set_state(context, mosq_cs_new);
#எனில் defined(WITH_ADNS) && defined(WITH_BRIDGE)
						எனில்(context->bridge){
							bridge__connect_step3(context);
							தொடர்;
						}
#endif
					}
				}இல்லையெனில்{
					do_disconnect(context, MOSQ_ERR_CONN_LOST);
					தொடர்;
				}
			}
			rc = packet__write(context);
			எனில்(rc){
				do_disconnect(context, rc);
				தொடர்;
			}
		}
	}

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		எனில்(context->pollfd_index < 0){
			தொடர்;
		}
#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		எனில்(context->wsi){
			// Websocket are already handled above
			தொடர்;
		}
#endif

#ifdef WITH_TLS
		எனில்(pollfds[context->pollfd_index].revents & POLLIN ||
				(context->ssl && context->state == mosq_cs_new)){
#இல்லையெனில்
		எனில்(pollfds[context->pollfd_index].revents & POLLIN){
#endif
			செய்{
				தேர்வு(context->transport){
					நிலை mosq_t_tcp:
					நிலை mosq_t_ws:
						rc = packet__read(context);
						நிறுத்து;
#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
					நிலை mosq_t_http:
						rc = http__read(context);
						நிறுத்து;
#endif
#எனில் !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
					/* Not supported with LWS */
					நிலை mosq_t_proxy_v2:
						rc = proxy_v2__read(context);
						நிறுத்து;
					நிலை mosq_t_proxy_v1:
						rc = proxy_v1__read(context);
						நிறுத்து;
#endif
					இயல்பு:
						rc = MOSQ_ERR_INVAL;
						நிறுத்து;
				}
				எனில்(rc){
					do_disconnect(context, rc);
					தொடர்;
				}
			}போது(SSL_DATA_PENDING(context));
		}இல்லையெனில்{
			எனில்(context->pollfd_index >= 0 && pollfds[context->pollfd_index].revents & (POLLERR | POLLNVAL | POLLHUP)){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				தொடர்;
			}
		}
	}
}


#endif
