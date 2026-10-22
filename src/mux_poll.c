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

#شامل "config.h"

#اگر !defined(WITH_EPOLL) && !defined(WITH_KQUEUE)

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#شامل <assert.h>
#ifndef WIN32
#شامل <poll.h>
#شامل <unistd.h>
#وگرنه
#شامل <process.h>
#شامل <winsock2.h>
#شامل <ws2tcpip.h>
#endif

#شامل <errno.h>
#شامل <signal.h>
#شامل <stdio.h>
#شامل <string.h>
#ifndef WIN32
#  include <sys/socket.h>
#endif
#شامل <time.h>

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif

#شامل "mosquitto_broker_internal.h"
#شامل "packet_mosq.h"
#شامل "send_mosq.h"
#شامل "sys_tree.h"
#شامل "util_mosq.h"
#شامل "mux.h"

ایستا تهی loop_handle_reads_writes(تهی);

ایستا ساختار pollfd *pollfds = پوچ;
ایستا size_t pollfd_max, pollfd_current_max = 0;


صحیح mux_poll__init(تهی)
{
#ifdef WIN32
	pollfd_max = (size_t)_getmaxstdio();
#وگرنه
	pollfd_max = (size_t)sysconf(_SC_OPEN_MAX);
#endif

	pollfds = mosquitto_calloc(pollfd_max, اندازه(ساختار pollfd));
	اگر(!pollfds){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
		بازگشت MOSQ_ERR_NOMEM;
	}
	memset(pollfds, 0, اندازه(ساختار pollfd)*pollfd_max);
	برای(size_t i=0; i<pollfd_max; i++){
		pollfds[i].fd = INVALID_SOCKET;
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح mux_poll__add_listeners(ساختار mosquitto__listener_sock *listensock, صحیح listensock_شمار)
{
	size_t pollfd_index = 0;

	برای(size_t i=0; i<(size_t )listensock_شمار; i++){
		pollfds[pollfd_index].fd = listensock[i].sock;
		pollfds[pollfd_index].events = POLLIN;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	اگر(pollfd_current_max == 0){
		pollfd_current_max = pollfd_index-1;
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح mux_poll__delete_listeners(ساختار mosquitto__listener_sock *listensock, صحیح listensock_شمار)
{
	size_t pollfd_index = 0;

	UNUSED(listensock);

	برای(size_t i=0; i<(size_t )listensock_شمار; i++){
		pollfds[pollfd_index].fd = INVALID_SOCKET;
		pollfds[pollfd_index].events = 0;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح mux_poll__add(ساختار mosquitto *context, uint16_t evt)
{
	اگر(context->events == evt){
		بازگشت MOSQ_ERR_SUCCESS;
	}

	اگر(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = context->sock;
		pollfds[context->pollfd_index].events = (short صحیح)evt;
		pollfds[context->pollfd_index].revents = 0;
	}وگرنه{
		برای(size_t i=0; i<pollfd_max; i++){
			اگر(pollfds[i].fd == INVALID_SOCKET){
				pollfds[i].fd = context->sock;
				pollfds[i].events = POLLIN;
				pollfds[i].revents = 0;
				context->pollfd_index = (صحیح)i;
				اگر(i > pollfd_current_max){
					pollfd_current_max = i;
				}
				بشکن;
			}
		}
	}
	context->events = evt;

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح mux_poll__add_out(ساختار mosquitto *context)
{
	بازگشت mux_poll__add(context, POLLIN | POLLOUT);
}


صحیح mux_poll__remove_out(ساختار mosquitto *context)
{
	اگر(context->events & POLLOUT){
		بازگشت mux_poll__new(context);
	}وگرنه{
		بازگشت MOSQ_ERR_SUCCESS;
	}
}


صحیح mux_poll__new(ساختار mosquitto *context)
{
	بازگشت mux_poll__add(context, POLLIN);
}


صحیح mux_poll__delete(ساختار mosquitto *context)
{
	size_t pollfd_index;

	اگر(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = INVALID_SOCKET;
		pollfds[context->pollfd_index].events = 0;
		pollfds[context->pollfd_index].revents = 0;
		pollfd_index = (size_t )context->pollfd_index;
		context->pollfd_index = -1;

		/* If this is the highest index, reduce the current max until we find
		 * the next highest in use index. */
		وقتی(pollfd_index == pollfd_current_max
				&& pollfd_index > 0
				&& pollfds[pollfd_index].fd == INVALID_SOCKET){

			pollfd_index--;
			pollfd_current_max--;
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح mux_poll__handle(ساختار mosquitto__listener_sock *listensock, صحیح listensock_شمار)
{
	ساختار mosquitto *context;
	صحیح fdcount;
	صحیح timeout;

#ifdef WITH_WEBSOCKETS
	timeout = 100;
#وگرنه
	timeout = db.next_event_ms;
#endif

#ifndef WIN32
	fdcount = poll(pollfds, pollfd_current_max+1, timeout);
#وگرنه
	fdcount = WSAPoll(pollfds, pollfd_current_max+1, timeout);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(پوچ);

	اگر(fdcount == -1){
#  اگر defined(WIN32) && defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		اگر(WSAGetLastError() == WSAEINVAL){
			/* WSAPoll() immediately returns an error if it is not given
			 * any sockets to wait on. This can happen if we only have
			 * websockets listeners. Sleep a little to prevent a busy loop.
			 */
			Sleep(10);
		}وگرنه
#  endif
		{
			log__printf(پوچ, MOSQ_LOG_ERR, "Error in poll: %s.", strerror(errno));
		}
	}وگرنه{
		loop_handle_reads_writes();

		برای(صحیح i=0; i<listensock_شمار; i++){
			اگر(pollfds[i].revents & POLLIN){
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				اگر(listensock[i].listener->ws_context){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
				}وگرنه
#endif
				{
					وقتی((context = net__socket_accept(&listensock[i])) != پوچ){
					}
				}
			}
		}
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح mux_poll__cleanup(تهی)
{
	mosquitto_FREE(pollfds);

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا تهی loop_handle_reads_writes(تهی)
{
	ساختار mosquitto *context, *ctxt_tmp;
	صحیح err;
	socklen_t len;
	صحیح rc;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		اگر(context->pollfd_index < 0){
			ادامه;
		}

		اگر(pollfds[context->pollfd_index].fd == INVALID_SOCKET){
			ادامه;
		}

		أكد(pollfds[context->pollfd_index].fd == context->sock);

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		اگر(context->wsi){
			ساختار lws_pollfd wspoll;
			wspoll.fd = pollfds[context->pollfd_index].fd;
			wspoll.events = pollfds[context->pollfd_index].events;
			wspoll.revents = pollfds[context->pollfd_index].revents;
			lws_service_fd(lws_get_context(context->wsi), &wspoll);
			ادامه;
		}
#endif

#ifdef WITH_TLS
		اگر(pollfds[context->pollfd_index].revents & POLLOUT ||
				context->want_write ||
				(context->ssl && context->state == mosq_cs_new)){
#وگرنه
		اگر(pollfds[context->pollfd_index].revents & POLLOUT){
#endif
			اگر(context->state == mosq_cs_connect_pending){
				len = اندازه(صحیح);
				اگر(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (نویسه *)&err, &len)){
					اگر(err == 0){
						mosquitto__set_state(context, mosq_cs_new);
#اگر defined(WITH_ADNS) && defined(WITH_BRIDGE)
						اگر(context->bridge){
							bridge__connect_step3(context);
							ادامه;
						}
#endif
					}
				}وگرنه{
					do_disconnect(context, MOSQ_ERR_CONN_LOST);
					ادامه;
				}
			}
			rc = packet__write(context);
			اگر(rc){
				do_disconnect(context, rc);
				ادامه;
			}
		}
	}

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		اگر(context->pollfd_index < 0){
			ادامه;
		}
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		اگر(context->wsi){
			// Websocket are already handled above
			ادامه;
		}
#endif

#ifdef WITH_TLS
		اگر(pollfds[context->pollfd_index].revents & POLLIN ||
				(context->ssl && context->state == mosq_cs_new)){
#وگرنه
		اگر(pollfds[context->pollfd_index].revents & POLLIN){
#endif
			انجامبده{
				انتخاب(context->transport){
					حالت mosq_t_tcp:
					حالت mosq_t_ws:
						rc = packet__read(context);
						بشکن;
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
					حالت mosq_t_http:
						rc = http__read(context);
						بشکن;
#endif
#اگر !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
					/* Not supported with LWS */
					حالت mosq_t_proxy_v2:
						rc = proxy_v2__read(context);
						بشکن;
					حالت mosq_t_proxy_v1:
						rc = proxy_v1__read(context);
						بشکن;
#endif
					پیشفرض:
						rc = MOSQ_ERR_INVAL;
						بشکن;
				}
				اگر(rc){
					do_disconnect(context, rc);
					ادامه;
				}
			}وقتی(SSL_DATA_PENDING(context));
		}وگرنه{
			اگر(context->pollfd_index >= 0 && pollfds[context->pollfd_index].revents & (POLLERR | POLLNVAL | POLLHUP)){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				ادامه;
			}
		}
	}
}


#endif
