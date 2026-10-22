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

#অন্তর্ভাৱ "config.h"

#যদি !defined(WITH_EPOLL) && !defined(WITH_KQUEUE)

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#অন্তর্ভাৱ <assert.h>
#ifndef WIN32
#অন্তর্ভাৱ <poll.h>
#অন্তর্ভাৱ <unistd.h>
#অন্যথা
#অন্তর্ভাৱ <process.h>
#অন্তর্ভাৱ <winsock2.h>
#অন্তর্ভাৱ <ws2tcpip.h>
#endif

#অন্তর্ভাৱ <errno.h>
#অন্তর্ভাৱ <signal.h>
#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <string.h>
#ifndef WIN32
#  include <sys/socket.h>
#endif
#অন্তর্ভাৱ <time.h>

#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif

#অন্তর্ভাৱ "mosquitto_broker_internal.h"
#অন্তর্ভাৱ "packet_mosq.h"
#অন্তর্ভাৱ "send_mosq.h"
#অন্তর্ভাৱ "sys_tree.h"
#অন্তর্ভাৱ "util_mosq.h"
#অন্তর্ভাৱ "mux.h"

স্থির শূন্য loop_handle_reads_writes(শূন্য);

স্থির সংরচনা pollfd *pollfds = রিক্ত;
স্থির size_t pollfd_max, pollfd_current_max = 0;


পূর্ণাঙ্ক mux_poll__init(শূন্য)
{
#ifdef WIN32
	pollfd_max = (size_t)_getmaxstdio();
#অন্যথা
	pollfd_max = (size_t)sysconf(_SC_OPEN_MAX);
#endif

	pollfds = mosquitto_calloc(pollfd_max, পরিমাণ(সংরচনা pollfd));
	যদি(!pollfds){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
		ঘূৰাই MOSQ_ERR_NOMEM;
	}
	memset(pollfds, 0, পরিমাণ(সংরচনা pollfd)*pollfd_max);
	বাবে(size_t i=0; i<pollfd_max; i++){
		pollfds[i].fd = INVALID_SOCKET;
	}

	ঘূৰাই MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক mux_poll__add_listeners(সংরচনা mosquitto__listener_sock *listensock, পূর্ণাঙ্ক listensock_গুনতি)
{
	size_t pollfd_index = 0;

	বাবে(size_t i=0; i<(size_t )listensock_গুনতি; i++){
		pollfds[pollfd_index].fd = listensock[i].sock;
		pollfds[pollfd_index].events = POLLIN;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	যদি(pollfd_current_max == 0){
		pollfd_current_max = pollfd_index-1;
	}
	ঘূৰাই MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক mux_poll__delete_listeners(সংরচনা mosquitto__listener_sock *listensock, পূর্ণাঙ্ক listensock_গুনতি)
{
	size_t pollfd_index = 0;

	UNUSED(listensock);

	বাবে(size_t i=0; i<(size_t )listensock_গুনতি; i++){
		pollfds[pollfd_index].fd = INVALID_SOCKET;
		pollfds[pollfd_index].events = 0;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	ঘূৰাই MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণাঙ্ক mux_poll__add(সংরচনা mosquitto *context, uint16_t evt)
{
	যদি(context->events == evt){
		ঘূৰাই MOSQ_ERR_SUCCESS;
	}

	যদি(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = context->sock;
		pollfds[context->pollfd_index].events = (short পূর্ণাঙ্ক)evt;
		pollfds[context->pollfd_index].revents = 0;
	}অন্যথা{
		বাবে(size_t i=0; i<pollfd_max; i++){
			যদি(pollfds[i].fd == INVALID_SOCKET){
				pollfds[i].fd = context->sock;
				pollfds[i].events = POLLIN;
				pollfds[i].revents = 0;
				context->pollfd_index = (পূর্ণাঙ্ক)i;
				যদি(i > pollfd_current_max){
					pollfd_current_max = i;
				}
				ভঙ্গ;
			}
		}
	}
	context->events = evt;

	ঘূৰাই MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক mux_poll__add_out(সংরচনা mosquitto *context)
{
	ঘূৰাই mux_poll__add(context, POLLIN | POLLOUT);
}


পূর্ণাঙ্ক mux_poll__remove_out(সংরচনা mosquitto *context)
{
	যদি(context->events & POLLOUT){
		ঘূৰাই mux_poll__new(context);
	}অন্যথা{
		ঘূৰাই MOSQ_ERR_SUCCESS;
	}
}


পূর্ণাঙ্ক mux_poll__new(সংরচনা mosquitto *context)
{
	ঘূৰাই mux_poll__add(context, POLLIN);
}


পূর্ণাঙ্ক mux_poll__delete(সংরচনা mosquitto *context)
{
	size_t pollfd_index;

	যদি(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = INVALID_SOCKET;
		pollfds[context->pollfd_index].events = 0;
		pollfds[context->pollfd_index].revents = 0;
		pollfd_index = (size_t )context->pollfd_index;
		context->pollfd_index = -1;

		/* If this is the highest index, reduce the current max until we find
		 * the next highest in use index. */
		যেতিয়ালৈ(pollfd_index == pollfd_current_max
				&& pollfd_index > 0
				&& pollfds[pollfd_index].fd == INVALID_SOCKET){

			pollfd_index--;
			pollfd_current_max--;
		}
	}

	ঘূৰাই MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক mux_poll__handle(সংরচনা mosquitto__listener_sock *listensock, পূর্ণাঙ্ক listensock_গুনতি)
{
	সংরচনা mosquitto *context;
	পূর্ণাঙ্ক fdcount;
	পূর্ণাঙ্ক timeout;

#ifdef WITH_WEBSOCKETS
	timeout = 100;
#অন্যথা
	timeout = db.next_event_ms;
#endif

#ifndef WIN32
	fdcount = poll(pollfds, pollfd_current_max+1, timeout);
#অন্যথা
	fdcount = WSAPoll(pollfds, pollfd_current_max+1, timeout);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(রিক্ত);

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
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error in poll: %s.", strerror(errno));
		}
	}অন্যথা{
		loop_handle_reads_writes();

		বাবে(পূর্ণাঙ্ক i=0; i<listensock_গুনতি; i++){
			যদি(pollfds[i].revents & POLLIN){
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				যদি(listensock[i].listener->ws_context){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
				}অন্যথা
#endif
				{
					যেতিয়ালৈ((context = net__socket_accept(&listensock[i])) != রিক্ত){
					}
				}
			}
		}
	}
	ঘূৰাই MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক mux_poll__cleanup(শূন্য)
{
	mosquitto_FREE(pollfds);

	ঘূৰাই MOSQ_ERR_SUCCESS;
}


স্থির শূন্য loop_handle_reads_writes(শূন্য)
{
	সংরচনা mosquitto *context, *ctxt_tmp;
	পূর্ণাঙ্ক err;
	socklen_t len;
	পূর্ণাঙ্ক rc;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		যদি(context->pollfd_index < 0){
			অনুৱর্ত;
		}

		যদি(pollfds[context->pollfd_index].fd == INVALID_SOCKET){
			অনুৱর্ত;
		}

		প্রতিজ্ঞা(pollfds[context->pollfd_index].fd == context->sock);

#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		যদি(context->wsi){
			সংরচনা lws_pollfd wspoll;
			wspoll.fd = pollfds[context->pollfd_index].fd;
			wspoll.events = pollfds[context->pollfd_index].events;
			wspoll.revents = pollfds[context->pollfd_index].revents;
			lws_service_fd(lws_get_context(context->wsi), &wspoll);
			অনুৱর্ত;
		}
#endif

#ifdef WITH_TLS
		যদি(pollfds[context->pollfd_index].revents & POLLOUT ||
				context->want_write ||
				(context->ssl && context->state == mosq_cs_new)){
#অন্যথা
		যদি(pollfds[context->pollfd_index].revents & POLLOUT){
#endif
			যদি(context->state == mosq_cs_connect_pending){
				len = পরিমাণ(পূর্ণাঙ্ক);
				যদি(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (ৱর্ণ *)&err, &len)){
					যদি(err == 0){
						mosquitto__set_state(context, mosq_cs_new);
#যদি defined(WITH_ADNS) && defined(WITH_BRIDGE)
						যদি(context->bridge){
							bridge__connect_step3(context);
							অনুৱর্ত;
						}
#endif
					}
				}অন্যথা{
					do_disconnect(context, MOSQ_ERR_CONN_LOST);
					অনুৱর্ত;
				}
			}
			rc = packet__write(context);
			যদি(rc){
				do_disconnect(context, rc);
				অনুৱর্ত;
			}
		}
	}

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		যদি(context->pollfd_index < 0){
			অনুৱর্ত;
		}
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		যদি(context->wsi){
			// Websocket are already handled above
			অনুৱর্ত;
		}
#endif

#ifdef WITH_TLS
		যদি(pollfds[context->pollfd_index].revents & POLLIN ||
				(context->ssl && context->state == mosq_cs_new)){
#অন্যথা
		যদি(pollfds[context->pollfd_index].revents & POLLIN){
#endif
			কৰা{
				ৱিকল্প(context->transport){
					পক্ষ mosq_t_tcp:
					পক্ষ mosq_t_ws:
						rc = packet__read(context);
						ভঙ্গ;
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
					পক্ষ mosq_t_http:
						rc = http__read(context);
						ভঙ্গ;
#endif
#যদি !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
					/* Not supported with LWS */
					পক্ষ mosq_t_proxy_v2:
						rc = proxy_v2__read(context);
						ভঙ্গ;
					পক্ষ mosq_t_proxy_v1:
						rc = proxy_v1__read(context);
						ভঙ্গ;
#endif
					স্ৱভাৱ:
						rc = MOSQ_ERR_INVAL;
						ভঙ্গ;
				}
				যদি(rc){
					do_disconnect(context, rc);
					অনুৱর্ত;
				}
			}যেতিয়ালৈ(SSL_DATA_PENDING(context));
		}অন্যথা{
			যদি(context->pollfd_index >= 0 && pollfds[context->pollfd_index].revents & (POLLERR | POLLNVAL | POLLHUP)){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				অনুৱর্ত;
			}
		}
	}
}


#endif
