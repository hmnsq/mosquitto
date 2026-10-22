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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଯଦି !defined(WITH_EPOLL) && !defined(WITH_KQUEUE)

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#ଅନ୍ତର୍ଭାଵ <assert.h>
#ifndef WIN32
#ଅନ୍ତର୍ଭାଵ <poll.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>
#ଅନ୍ୟଥା
#ଅନ୍ତର୍ଭାଵ <process.h>
#ଅନ୍ତର୍ଭାଵ <winsock2.h>
#ଅନ୍ତର୍ଭାଵ <ws2tcpip.h>
#endif

#ଅନ୍ତର୍ଭାଵ <errno.h>
#ଅନ୍ତର୍ଭାଵ <signal.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ifndef WIN32
#  include <sys/socket.h>
#endif
#ଅନ୍ତର୍ଭାଵ <time.h>

#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "packet_mosq.h"
#ଅନ୍ତର୍ଭାଵ "send_mosq.h"
#ଅନ୍ତର୍ଭାଵ "sys_tree.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"
#ଅନ୍ତର୍ଭାଵ "mux.h"

ସ୍ଥିର ଶୂନ୍ୟ loop_handle_reads_writes(ଶୂନ୍ୟ);

ସ୍ଥିର ସଂରଚନା pollfd *pollfds = ଖାଲି;
ସ୍ଥିର size_t pollfd_max, pollfd_current_max = 0;


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_poll__init(ଶୂନ୍ୟ)
{
#ifdef WIN32
	pollfd_max = (size_t)_getmaxstdio();
#ଅନ୍ୟଥା
	pollfd_max = (size_t)sysconf(_SC_OPEN_MAX);
#endif

	pollfds = mosquitto_calloc(pollfd_max, ଆକାର(ସଂରଚନା pollfd));
	ଯଦି(!pollfds){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}
	memset(pollfds, 0, ଆକାର(ସଂରଚନା pollfd)*pollfd_max);
	ପାଇଁ(size_t i=0; i<pollfd_max; i++){
		pollfds[i].fd = INVALID_SOCKET;
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_poll__add_listeners(ସଂରଚନା mosquitto__listener_sock *listensock, ପୂର୍ଣ୍ଣସଂଖ୍ୟା listensock_ଗଣନା)
{
	size_t pollfd_index = 0;

	ପାଇଁ(size_t i=0; i<(size_t )listensock_ଗଣନା; i++){
		pollfds[pollfd_index].fd = listensock[i].sock;
		pollfds[pollfd_index].events = POLLIN;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	ଯଦି(pollfd_current_max == 0){
		pollfd_current_max = pollfd_index-1;
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_poll__delete_listeners(ସଂରଚନା mosquitto__listener_sock *listensock, ପୂର୍ଣ୍ଣସଂଖ୍ୟା listensock_ଗଣନା)
{
	size_t pollfd_index = 0;

	UNUSED(listensock);

	ପାଇଁ(size_t i=0; i<(size_t )listensock_ଗଣନା; i++){
		pollfds[pollfd_index].fd = INVALID_SOCKET;
		pollfds[pollfd_index].events = 0;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_poll__add(ସଂରଚନା mosquitto *context, uint16_t evt)
{
	ଯଦି(context->events == evt){
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

	ଯଦି(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = context->sock;
		pollfds[context->pollfd_index].events = (short ପୂର୍ଣ୍ଣସଂଖ୍ୟା)evt;
		pollfds[context->pollfd_index].revents = 0;
	}ଅନ୍ୟଥା{
		ପାଇଁ(size_t i=0; i<pollfd_max; i++){
			ଯଦି(pollfds[i].fd == INVALID_SOCKET){
				pollfds[i].fd = context->sock;
				pollfds[i].events = POLLIN;
				pollfds[i].revents = 0;
				context->pollfd_index = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)i;
				ଯଦି(i > pollfd_current_max){
					pollfd_current_max = i;
				}
				ଭାଙ୍ଗ;
			}
		}
	}
	context->events = evt;

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_poll__add_out(ସଂରଚନା mosquitto *context)
{
	ଫେରନ୍ତୁ mux_poll__add(context, POLLIN | POLLOUT);
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_poll__remove_out(ସଂରଚନା mosquitto *context)
{
	ଯଦି(context->events & POLLOUT){
		ଫେରନ୍ତୁ mux_poll__new(context);
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_poll__new(ସଂରଚନା mosquitto *context)
{
	ଫେରନ୍ତୁ mux_poll__add(context, POLLIN);
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_poll__delete(ସଂରଚନା mosquitto *context)
{
	size_t pollfd_index;

	ଯଦି(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = INVALID_SOCKET;
		pollfds[context->pollfd_index].events = 0;
		pollfds[context->pollfd_index].revents = 0;
		pollfd_index = (size_t )context->pollfd_index;
		context->pollfd_index = -1;

		/* If this is the highest index, reduce the current max until we find
		 * the next highest in use index. */
		ଯେତେବେଳେ(pollfd_index == pollfd_current_max
				&& pollfd_index > 0
				&& pollfds[pollfd_index].fd == INVALID_SOCKET){

			pollfd_index--;
			pollfd_current_max--;
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_poll__handle(ସଂରଚନା mosquitto__listener_sock *listensock, ପୂର୍ଣ୍ଣସଂଖ୍ୟା listensock_ଗଣନା)
{
	ସଂରଚନା mosquitto *context;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdcount;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା timeout;

#ifdef WITH_WEBSOCKETS
	timeout = 100;
#ଅନ୍ୟଥା
	timeout = db.next_event_ms;
#endif

#ifndef WIN32
	fdcount = poll(pollfds, pollfd_current_max+1, timeout);
#ଅନ୍ୟଥା
	fdcount = WSAPoll(pollfds, pollfd_current_max+1, timeout);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(ଖାଲି);

	ଯଦି(fdcount == -1){
#  ଯଦି defined(WIN32) && defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ଯଦି(WSAGetLastError() == WSAEINVAL){
			/* WSAPoll() immediately returns an error if it is not given
			 * any sockets to wait on. This can happen if we only have
			 * websockets listeners. Sleep a little to prevent a busy loop.
			 */
			Sleep(10);
		}ଅନ୍ୟଥା
#  endif
		{
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error in poll: %s.", strerror(errno));
		}
	}ଅନ୍ୟଥା{
		loop_handle_reads_writes();

		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<listensock_ଗଣନା; i++){
			ଯଦି(pollfds[i].revents & POLLIN){
#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				ଯଦି(listensock[i].listener->ws_context){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
				}ଅନ୍ୟଥା
#endif
				{
					ଯେତେବେଳେ((context = net__socket_accept(&listensock[i])) != ଖାଲି){
					}
				}
			}
		}
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_poll__cleanup(ଶୂନ୍ୟ)
{
	mosquitto_FREE(pollfds);

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ଶୂନ୍ୟ loop_handle_reads_writes(ଶୂନ୍ୟ)
{
	ସଂରଚନା mosquitto *context, *ctxt_tmp;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;
	socklen_t len;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		ଯଦି(context->pollfd_index < 0){
			ଜାରି;
		}

		ଯଦି(pollfds[context->pollfd_index].fd == INVALID_SOCKET){
			ଜାରି;
		}

		ପ୍ରତିଜ୍ଞା(pollfds[context->pollfd_index].fd == context->sock);

#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ଯଦି(context->wsi){
			ସଂରଚନା lws_pollfd wspoll;
			wspoll.fd = pollfds[context->pollfd_index].fd;
			wspoll.events = pollfds[context->pollfd_index].events;
			wspoll.revents = pollfds[context->pollfd_index].revents;
			lws_service_fd(lws_get_context(context->wsi), &wspoll);
			ଜାରି;
		}
#endif

#ifdef WITH_TLS
		ଯଦି(pollfds[context->pollfd_index].revents & POLLOUT ||
				context->want_write ||
				(context->ssl && context->state == mosq_cs_new)){
#ଅନ୍ୟଥା
		ଯଦି(pollfds[context->pollfd_index].revents & POLLOUT){
#endif
			ଯଦି(context->state == mosq_cs_connect_pending){
				len = ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
				ଯଦି(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (ଅକ୍ଷର *)&err, &len)){
					ଯଦି(err == 0){
						mosquitto__set_state(context, mosq_cs_new);
#ଯଦି defined(WITH_ADNS) && defined(WITH_BRIDGE)
						ଯଦି(context->bridge){
							bridge__connect_step3(context);
							ଜାରି;
						}
#endif
					}
				}ଅନ୍ୟଥା{
					do_disconnect(context, MOSQ_ERR_CONN_LOST);
					ଜାରି;
				}
			}
			rc = packet__write(context);
			ଯଦି(rc){
				do_disconnect(context, rc);
				ଜାରି;
			}
		}
	}

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		ଯଦି(context->pollfd_index < 0){
			ଜାରି;
		}
#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ଯଦି(context->wsi){
			// Websocket are already handled above
			ଜାରି;
		}
#endif

#ifdef WITH_TLS
		ଯଦି(pollfds[context->pollfd_index].revents & POLLIN ||
				(context->ssl && context->state == mosq_cs_new)){
#ଅନ୍ୟଥା
		ଯଦି(pollfds[context->pollfd_index].revents & POLLIN){
#endif
			କର{
				ବାଛ(context->transport){
					ସ୍ଥିତି mosq_t_tcp:
					ସ୍ଥିତି mosq_t_ws:
						rc = packet__read(context);
						ଭାଙ୍ଗ;
#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
					ସ୍ଥିତି mosq_t_http:
						rc = http__read(context);
						ଭାଙ୍ଗ;
#endif
#ଯଦି !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
					/* Not supported with LWS */
					ସ୍ଥିତି mosq_t_proxy_v2:
						rc = proxy_v2__read(context);
						ଭାଙ୍ଗ;
					ସ୍ଥିତି mosq_t_proxy_v1:
						rc = proxy_v1__read(context);
						ଭାଙ୍ଗ;
#endif
					ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
						rc = MOSQ_ERR_INVAL;
						ଭାଙ୍ଗ;
				}
				ଯଦି(rc){
					do_disconnect(context, rc);
					ଜାରି;
				}
			}ଯେତେବେଳେ(SSL_DATA_PENDING(context));
		}ଅନ୍ୟଥା{
			ଯଦି(context->pollfd_index >= 0 && pollfds[context->pollfd_index].revents & (POLLERR | POLLNVAL | POLLHUP)){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				ଜାରି;
			}
		}
	}
}


#endif
