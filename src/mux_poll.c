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

#অন্তর্ভাব "config.h"

#যদি !defined(WITH_EPOLL) && !defined(WITH_KQUEUE)

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#অন্তর্ভাব <assert.h>
#ifndef WIN32
#অন্তর্ভাব <poll.h>
#অন্তর্ভাব <unistd.h>
#অন্যথা
#অন্তর্ভাব <process.h>
#অন্তর্ভাব <winsock2.h>
#অন্তর্ভাব <ws2tcpip.h>
#endif

#অন্তর্ভাব <errno.h>
#অন্তর্ভাব <signal.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>
#ifndef WIN32
#  include <sys/socket.h>
#endif
#অন্তর্ভাব <time.h>

#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "packet_mosq.h"
#অন্তর্ভাব "send_mosq.h"
#অন্তর্ভাব "sys_tree.h"
#অন্তর্ভাব "util_mosq.h"
#অন্তর্ভাব "mux.h"

স্থির শূন্য loop_handle_reads_writes(শূন্য);

স্থির কাঠামো pollfd *pollfds = ফাঁকা;
স্থির size_t pollfd_max, pollfd_current_max = 0;


পূর্ণসংখ্যা mux_poll__init(শূন্য)
{
#ifdef WIN32
	pollfd_max = (size_t)_getmaxstdio();
#অন্যথা
	pollfd_max = (size_t)sysconf(_SC_OPEN_MAX);
#endif

	pollfds = mosquitto_calloc(pollfd_max, আকার(কাঠামো pollfd));
	যদি(!pollfds){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Out of memory.");
		ফেরত MOSQ_ERR_NOMEM;
	}
	memset(pollfds, 0, আকার(কাঠামো pollfd)*pollfd_max);
	জন্য(size_t i=0; i<pollfd_max; i++){
		pollfds[i].fd = INVALID_SOCKET;
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা mux_poll__add_listeners(কাঠামো mosquitto__listener_sock *listensock, পূর্ণসংখ্যা listensock_গুনতি)
{
	size_t pollfd_সূচক = 0;

	জন্য(size_t i=0; i<(size_t )listensock_গুনতি; i++){
		pollfds[pollfd_সূচক].fd = listensock[i].sock;
		pollfds[pollfd_সূচক].events = POLLIN;
		pollfds[pollfd_সূচক].revents = 0;
		pollfd_সূচক++;
	}

	যদি(pollfd_current_max == 0){
		pollfd_current_max = pollfd_সূচক-1;
	}
	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা mux_poll__delete_listeners(কাঠামো mosquitto__listener_sock *listensock, পূর্ণসংখ্যা listensock_গুনতি)
{
	size_t pollfd_সূচক = 0;

	UNUSED(listensock);

	জন্য(size_t i=0; i<(size_t )listensock_গুনতি; i++){
		pollfds[pollfd_সূচক].fd = INVALID_SOCKET;
		pollfds[pollfd_সূচক].events = 0;
		pollfds[pollfd_সূচক].revents = 0;
		pollfd_সূচক++;
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা mux_poll__add(কাঠামো mosquitto *context, uint16_t evt)
{
	যদি(context->events == evt){
		ফেরত MOSQ_ERR_SUCCESS;
	}

	যদি(context->pollfd_সূচক != -1){
		pollfds[context->pollfd_সূচক].fd = context->sock;
		pollfds[context->pollfd_সূচক].events = (short পূর্ণসংখ্যা)evt;
		pollfds[context->pollfd_সূচক].revents = 0;
	}অন্যথা{
		জন্য(size_t i=0; i<pollfd_max; i++){
			যদি(pollfds[i].fd == INVALID_SOCKET){
				pollfds[i].fd = context->sock;
				pollfds[i].events = POLLIN;
				pollfds[i].revents = 0;
				context->pollfd_সূচক = (পূর্ণসংখ্যা)i;
				যদি(i > pollfd_current_max){
					pollfd_current_max = i;
				}
				বিরতি;
			}
		}
	}
	context->events = evt;

	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা mux_poll__add_out(কাঠামো mosquitto *context)
{
	ফেরত mux_poll__add(context, POLLIN | POLLOUT);
}


পূর্ণসংখ্যা mux_poll__remove_out(কাঠামো mosquitto *context)
{
	যদি(context->events & POLLOUT){
		ফেরত mux_poll__new(context);
	}অন্যথা{
		ফেরত MOSQ_ERR_SUCCESS;
	}
}


পূর্ণসংখ্যা mux_poll__new(কাঠামো mosquitto *context)
{
	ফেরত mux_poll__add(context, POLLIN);
}


পূর্ণসংখ্যা mux_poll__delete(কাঠামো mosquitto *context)
{
	size_t pollfd_সূচক;

	যদি(context->pollfd_সূচক != -1){
		pollfds[context->pollfd_সূচক].fd = INVALID_SOCKET;
		pollfds[context->pollfd_সূচক].events = 0;
		pollfds[context->pollfd_সূচক].revents = 0;
		pollfd_সূচক = (size_t )context->pollfd_সূচক;
		context->pollfd_সূচক = -1;

		/* If this is the highest index, reduce the current max until we find
		 * the next highest in use index. */
		যখন(pollfd_সূচক == pollfd_current_max
				&& pollfd_সূচক > 0
				&& pollfds[pollfd_সূচক].fd == INVALID_SOCKET){

			pollfd_সূচক--;
			pollfd_current_max--;
		}
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা mux_poll__handle(কাঠামো mosquitto__listener_sock *listensock, পূর্ণসংখ্যা listensock_গুনতি)
{
	কাঠামো mosquitto *context;
	পূর্ণসংখ্যা fdcount;
	পূর্ণসংখ্যা timeout;

#ifdef WITH_WEBSOCKETS
	timeout = 100;
#অন্যথা
	timeout = db.পরবর্তী_event_ms;
#endif

#ifndef WIN32
	fdcount = poll(pollfds, pollfd_current_max+1, timeout);
#অন্যথা
	fdcount = WSAPoll(pollfds, pollfd_current_max+1, timeout);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(ফাঁকা);

	যদি(fdcount == -1){
#  যদি defined(WIN32) && defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		যদি(WSAGetLastError() == WSAEINVAL){
			/* WSAPoll() immediately returns an error if it is not given
			 * any sockets to wait on. This can happen if we only have
			 * websockets listeners. Sleep a little to prevent a busy loop.
			 */
			Sleep(10);
		}অন্যথা
#  endif
		{
			log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error in poll: %s.", strerror(errno));
		}
	}অন্যথা{
		loop_handle_reads_writes();

		জন্য(পূর্ণসংখ্যা i=0; i<listensock_গুনতি; i++){
			যদি(pollfds[i].revents & POLLIN){
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				যদি(listensock[i].listener->ws_context){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
				}অন্যথা
#endif
				{
					যখন((context = net__socket_accept(&listensock[i])) != ফাঁকা){
					}
				}
			}
		}
	}
	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা mux_poll__cleanup(শূন্য)
{
	mosquitto_FREE(pollfds);

	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির শূন্য loop_handle_reads_writes(শূন্য)
{
	কাঠামো mosquitto *context, *ctxt_tmp;
	পূর্ণসংখ্যা err;
	socklen_t len;
	পূর্ণসংখ্যা rc;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		যদি(context->pollfd_সূচক < 0){
			চালু;
		}

		যদি(pollfds[context->pollfd_সূচক].fd == INVALID_SOCKET){
			চালু;
		}

		প্রতিজ্ঞা(pollfds[context->pollfd_সূচক].fd == context->sock);

#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		যদি(context->wsi){
			কাঠামো lws_pollfd wspoll;
			wspoll.fd = pollfds[context->pollfd_সূচক].fd;
			wspoll.events = pollfds[context->pollfd_সূচক].events;
			wspoll.revents = pollfds[context->pollfd_সূচক].revents;
			lws_service_fd(lws_get_context(context->wsi), &wspoll);
			চালু;
		}
#endif

#ifdef WITH_TLS
		যদি(pollfds[context->pollfd_সূচক].revents & POLLOUT ||
				context->want_write ||
				(context->ssl && context->state == mosq_cs_new)){
#অন্যথা
		যদি(pollfds[context->pollfd_সূচক].revents & POLLOUT){
#endif
			যদি(context->state == mosq_cs_connect_pending){
				len = আকার(পূর্ণসংখ্যা);
				যদি(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (অক্ষর *)&err, &len)){
					যদি(err == 0){
						mosquitto__set_state(context, mosq_cs_new);
#যদি defined(WITH_ADNS) && defined(WITH_BRIDGE)
						যদি(context->bridge){
							bridge__connect_step3(context);
							চালু;
						}
#endif
					}
				}অন্যথা{
					do_disconnect(context, MOSQ_ERR_CONN_LOST);
					চালু;
				}
			}
			rc = packet__write(context);
			যদি(rc){
				do_disconnect(context, rc);
				চালু;
			}
		}
	}

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		যদি(context->pollfd_সূচক < 0){
			চালু;
		}
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		যদি(context->wsi){
			// Websocket are already handled above
			চালু;
		}
#endif

#ifdef WITH_TLS
		যদি(pollfds[context->pollfd_সূচক].revents & POLLIN ||
				(context->ssl && context->state == mosq_cs_new)){
#অন্যথা
		যদি(pollfds[context->pollfd_সূচক].revents & POLLIN){
#endif
			করো{
				নির্বাচন(context->transport){
					ক্ষেত্র mosq_t_tcp:
					ক্ষেত্র mosq_t_ws:
						rc = packet__read(context);
						বিরতি;
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
					ক্ষেত্র mosq_t_http:
						rc = http__read(context);
						বিরতি;
#endif
#যদি !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
					/* Not supported with LWS */
					ক্ষেত্র mosq_t_proxy_v2:
						rc = proxy_v2__read(context);
						বিরতি;
					ক্ষেত্র mosq_t_proxy_v1:
						rc = proxy_v1__read(context);
						বিরতি;
#endif
					পূর্বনির্ধারিত:
						rc = MOSQ_ERR_INVAL;
						বিরতি;
				}
				যদি(rc){
					do_disconnect(context, rc);
					চালু;
				}
			}যখন(SSL_তথ্য_PENDING(context));
		}অন্যথা{
			যদি(context->pollfd_সূচক >= 0 && pollfds[context->pollfd_সূচক].revents & (POLLERR | POLLNVAL | POLLHUP)){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				চালু;
			}
		}
	}
}


#endif
