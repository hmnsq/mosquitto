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

#അന്തര്ഭാവ "config.h"

#എങ്കിൽ !defined(WITH_EPOLL) && !defined(WITH_KQUEUE)

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#അന്തര്ഭാവ <assert.h>
#ifndef WIN32
#അന്തര്ഭാവ <poll.h>
#അന്തര്ഭാവ <unistd.h>
#അല്ലെങ്കിൽ
#അന്തര്ഭാവ <process.h>
#അന്തര്ഭാവ <winsock2.h>
#അന്തര്ഭാവ <ws2tcpip.h>
#endif

#അന്തര്ഭാവ <errno.h>
#അന്തര്ഭാവ <signal.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>
#ifndef WIN32
#  include <sys/socket.h>
#endif
#അന്തര്ഭാവ <time.h>

#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "packet_mosq.h"
#അന്തര്ഭാവ "send_mosq.h"
#അന്തര്ഭാവ "sys_tree.h"
#അന്തര്ഭാവ "util_mosq.h"
#അന്തര്ഭാവ "mux.h"

സ്ഥിതം ശൂന്യം loop_handle_reads_writes(ശൂന്യം);

സ്ഥിതം ഘടന pollfd *pollfds = ശൂന്യ;
സ്ഥിതം size_t pollfd_max, pollfd_current_max = 0;


പൂർണ്ണസംഖ്യ mux_poll__init(ശൂന്യം)
{
#ifdef WIN32
	pollfd_max = (size_t)_getmaxstdio();
#അല്ലെങ്കിൽ
	pollfd_max = (size_t)sysconf(_SC_OPEN_MAX);
#endif

	pollfds = mosquitto_calloc(pollfd_max, വലുപ്പം(ഘടന pollfd));
	എങ്കിൽ(!pollfds){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Out of memory.");
		തിരികെ MOSQ_ERR_NOMEM;
	}
	memset(pollfds, 0, വലുപ്പം(ഘടന pollfd)*pollfd_max);
	വേണ്ടി(size_t i=0; i<pollfd_max; i++){
		pollfds[i].fd = INVALID_SOCKET;
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ mux_poll__add_listeners(ഘടന mosquitto__listener_sock *listensock, പൂർണ്ണസംഖ്യ listensock_എണ്ണം)
{
	size_t pollfd_index = 0;

	വേണ്ടി(size_t i=0; i<(size_t )listensock_എണ്ണം; i++){
		pollfds[pollfd_index].fd = listensock[i].sock;
		pollfds[pollfd_index].events = POLLIN;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	എങ്കിൽ(pollfd_current_max == 0){
		pollfd_current_max = pollfd_index-1;
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ mux_poll__delete_listeners(ഘടന mosquitto__listener_sock *listensock, പൂർണ്ണസംഖ്യ listensock_എണ്ണം)
{
	size_t pollfd_index = 0;

	UNUSED(listensock);

	വേണ്ടി(size_t i=0; i<(size_t )listensock_എണ്ണം; i++){
		pollfds[pollfd_index].fd = INVALID_SOCKET;
		pollfds[pollfd_index].events = 0;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ mux_poll__add(ഘടന mosquitto *context, uint16_t evt)
{
	എങ്കിൽ(context->events == evt){
		തിരികെ MOSQ_ERR_SUCCESS;
	}

	എങ്കിൽ(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = context->sock;
		pollfds[context->pollfd_index].events = (short പൂർണ്ണസംഖ്യ)evt;
		pollfds[context->pollfd_index].revents = 0;
	}അല്ലെങ്കിൽ{
		വേണ്ടി(size_t i=0; i<pollfd_max; i++){
			എങ്കിൽ(pollfds[i].fd == INVALID_SOCKET){
				pollfds[i].fd = context->sock;
				pollfds[i].events = POLLIN;
				pollfds[i].revents = 0;
				context->pollfd_index = (പൂർണ്ണസംഖ്യ)i;
				എങ്കിൽ(i > pollfd_current_max){
					pollfd_current_max = i;
				}
				നിർത്തുക;
			}
		}
	}
	context->events = evt;

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ mux_poll__add_out(ഘടന mosquitto *context)
{
	തിരികെ mux_poll__add(context, POLLIN | POLLOUT);
}


പൂർണ്ണസംഖ്യ mux_poll__remove_out(ഘടന mosquitto *context)
{
	എങ്കിൽ(context->events & POLLOUT){
		തിരികെ mux_poll__new(context);
	}അല്ലെങ്കിൽ{
		തിരികെ MOSQ_ERR_SUCCESS;
	}
}


പൂർണ്ണസംഖ്യ mux_poll__new(ഘടന mosquitto *context)
{
	തിരികെ mux_poll__add(context, POLLIN);
}


പൂർണ്ണസംഖ്യ mux_poll__delete(ഘടന mosquitto *context)
{
	size_t pollfd_index;

	എങ്കിൽ(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = INVALID_SOCKET;
		pollfds[context->pollfd_index].events = 0;
		pollfds[context->pollfd_index].revents = 0;
		pollfd_index = (size_t )context->pollfd_index;
		context->pollfd_index = -1;

		/* If this is the highest index, reduce the current max until we find
		 * the next highest in use index. */
		ആകുമ്പോൾ(pollfd_index == pollfd_current_max
				&& pollfd_index > 0
				&& pollfds[pollfd_index].fd == INVALID_SOCKET){

			pollfd_index--;
			pollfd_current_max--;
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ mux_poll__handle(ഘടന mosquitto__listener_sock *listensock, പൂർണ്ണസംഖ്യ listensock_എണ്ണം)
{
	ഘടന mosquitto *context;
	പൂർണ്ണസംഖ്യ fdcount;
	പൂർണ്ണസംഖ്യ timeout;

#ifdef WITH_WEBSOCKETS
	timeout = 100;
#അല്ലെങ്കിൽ
	timeout = db.next_event_ms;
#endif

#ifndef WIN32
	fdcount = poll(pollfds, pollfd_current_max+1, timeout);
#അല്ലെങ്കിൽ
	fdcount = WSAPoll(pollfds, pollfd_current_max+1, timeout);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(ശൂന്യ);

	എങ്കിൽ(fdcount == -1){
#  എങ്കിൽ defined(WIN32) && defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		എങ്കിൽ(WSAGetLastError() == WSAEINVAL){
			/* WSAPoll() immediately returns an error if it is not given
			 * any sockets to wait on. This can happen if we only have
			 * websockets listeners. Sleep a little to prevent a busy loop.
			 */
			Sleep(10);
		}അല്ലെങ്കിൽ
#  endif
		{
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error in poll: %s.", strerror(errno));
		}
	}അല്ലെങ്കിൽ{
		loop_handle_reads_writes();

		വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<listensock_എണ്ണം; i++){
			എങ്കിൽ(pollfds[i].revents & POLLIN){
#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				എങ്കിൽ(listensock[i].listener->ws_context){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
				}അല്ലെങ്കിൽ
#endif
				{
					ആകുമ്പോൾ((context = net__socket_accept(&listensock[i])) != ശൂന്യ){
					}
				}
			}
		}
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ mux_poll__cleanup(ശൂന്യം)
{
	mosquitto_FREE(pollfds);

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം ശൂന്യം loop_handle_reads_writes(ശൂന്യം)
{
	ഘടന mosquitto *context, *ctxt_tmp;
	പൂർണ്ണസംഖ്യ err;
	socklen_t len;
	പൂർണ്ണസംഖ്യ rc;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		എങ്കിൽ(context->pollfd_index < 0){
			തുടരുക;
		}

		എങ്കിൽ(pollfds[context->pollfd_index].fd == INVALID_SOCKET){
			തുടരുക;
		}

		പ്രതിജ്ഞാ(pollfds[context->pollfd_index].fd == context->sock);

#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		എങ്കിൽ(context->wsi){
			ഘടന lws_pollfd wspoll;
			wspoll.fd = pollfds[context->pollfd_index].fd;
			wspoll.events = pollfds[context->pollfd_index].events;
			wspoll.revents = pollfds[context->pollfd_index].revents;
			lws_service_fd(lws_get_context(context->wsi), &wspoll);
			തുടരുക;
		}
#endif

#ifdef WITH_TLS
		എങ്കിൽ(pollfds[context->pollfd_index].revents & POLLOUT ||
				context->want_write ||
				(context->ssl && context->state == mosq_cs_new)){
#അല്ലെങ്കിൽ
		എങ്കിൽ(pollfds[context->pollfd_index].revents & POLLOUT){
#endif
			എങ്കിൽ(context->state == mosq_cs_connect_pending){
				len = വലുപ്പം(പൂർണ്ണസംഖ്യ);
				എങ്കിൽ(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (അക്ഷരം *)&err, &len)){
					എങ്കിൽ(err == 0){
						mosquitto__set_state(context, mosq_cs_new);
#എങ്കിൽ defined(WITH_ADNS) && defined(WITH_BRIDGE)
						എങ്കിൽ(context->bridge){
							bridge__connect_step3(context);
							തുടരുക;
						}
#endif
					}
				}അല്ലെങ്കിൽ{
					do_disconnect(context, MOSQ_ERR_CONN_LOST);
					തുടരുക;
				}
			}
			rc = packet__write(context);
			എങ്കിൽ(rc){
				do_disconnect(context, rc);
				തുടരുക;
			}
		}
	}

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		എങ്കിൽ(context->pollfd_index < 0){
			തുടരുക;
		}
#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		എങ്കിൽ(context->wsi){
			// Websocket are already handled above
			തുടരുക;
		}
#endif

#ifdef WITH_TLS
		എങ്കിൽ(pollfds[context->pollfd_index].revents & POLLIN ||
				(context->ssl && context->state == mosq_cs_new)){
#അല്ലെങ്കിൽ
		എങ്കിൽ(pollfds[context->pollfd_index].revents & POLLIN){
#endif
			ചെയ്യുക{
				തിരഞ്ഞെടുപ്പ്(context->transport){
					അവസ്ഥ mosq_t_tcp:
					അവസ്ഥ mosq_t_ws:
						rc = packet__read(context);
						നിർത്തുക;
#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
					അവസ്ഥ mosq_t_http:
						rc = http__read(context);
						നിർത്തുക;
#endif
#എങ്കിൽ !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
					/* Not supported with LWS */
					അവസ്ഥ mosq_t_proxy_v2:
						rc = proxy_v2__read(context);
						നിർത്തുക;
					അവസ്ഥ mosq_t_proxy_v1:
						rc = proxy_v1__read(context);
						നിർത്തുക;
#endif
					സ്ഥിരസ്ഥിതി:
						rc = MOSQ_ERR_INVAL;
						നിർത്തുക;
				}
				എങ്കിൽ(rc){
					do_disconnect(context, rc);
					തുടരുക;
				}
			}ആകുമ്പോൾ(SSL_DATA_PENDING(context));
		}അല്ലെങ്കിൽ{
			എങ്കിൽ(context->pollfd_index >= 0 && pollfds[context->pollfd_index].revents & (POLLERR | POLLNVAL | POLLHUP)){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				തുടരുക;
			}
		}
	}
}


#endif
