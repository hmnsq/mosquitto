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

#ضمن "config.h"

#إذا !defined(WITH_EPOLL) && !defined(WITH_KQUEUE)

#اگر_تعریف_نشده WIN32
#  define GNU_منبع
#پایان_اگر

#ضمن <assert.h>
#اگر_تعریف_نشده WIN32
#ضمن <poll.h>
#ضمن <unistd.h>
#وإلا
#ضمن <process.h>
#ضمن <winsock2.h>
#ضمن <ws2tcpip.h>
#پایان_اگر

#ضمن <errno.h>
#ضمن <signal.h>
#ضمن <stdio.h>
#ضمن <string.h>
#اگر_تعریف_نشده WIN32
#  include <sys/socket.h>
#پایان_اگر
#ضمن <time.h>

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#پایان_اگر

#ضمن "mosquitto_broker_internal.h"
#ضمن "packet_mosq.h"
#ضمن "send_mosq.h"
#ضمن "sys_tree.h"
#ضمن "util_mosq.h"
#ضمن "mux.h"

ساكن فراغ loop_handle_reads_writes(فراغ);

ساكن بنية pollfd *pollfds = عدم;
ساكن size_t pollfd_max, pollfd_current_max = 0;


صحيح mux_poll__init(فراغ)
{
#اگر_تعریف_شده WIN32
	pollfd_max = (size_t)_getmaxstdio();
#وإلا
	pollfd_max = (size_t)sysconf(_SC_OPEN_MAX);
#پایان_اگر

	pollfds = mosquitto_calloc(pollfd_max, حجم(بنية pollfd));
	إذا(!pollfds){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		أرجع MOSQ_ERR_NOMEM;
	}
	memset(pollfds, 0, حجم(بنية pollfd)*pollfd_max);
	لكل(size_t i=0; i<pollfd_max; i++){
		pollfds[i].fd = INVALID_SOCKET;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح mux_poll_جمع_کن_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_شمار)
{
	size_t pollfd_شاخص = 0;

	لكل(size_t i=0; i<(size_t )listensock_شمار; i++){
		pollfds[pollfd_شاخص].fd = listensock[i].sock;
		pollfds[pollfd_شاخص].events = POLLIN;
		pollfds[pollfd_شاخص].revents = 0;
		pollfd_شاخص++;
	}

	إذا(pollfd_current_max == 0){
		pollfd_current_max = pollfd_شاخص-1;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح mux_poll__delete_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_شمار)
{
	size_t pollfd_شاخص = 0;

	استفادهنشده(listensock);

	لكل(size_t i=0; i<(size_t )listensock_شمار; i++){
		pollfds[pollfd_شاخص].fd = INVALID_SOCKET;
		pollfds[pollfd_شاخص].events = 0;
		pollfds[pollfd_شاخص].revents = 0;
		pollfd_شاخص++;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح mux_poll_جمع_کن(بنية mosquitto *context, uint16_t evt)
{
	إذا(context->events == evt){
		أرجع MOSQ_ERR_SUCCESS;
	}

	إذا(context->pollfd_شاخص != -1){
		pollfds[context->pollfd_شاخص].fd = context->sock;
		pollfds[context->pollfd_شاخص].events = (کوتاه صحيح)evt;
		pollfds[context->pollfd_شاخص].revents = 0;
	}وإلا{
		لكل(size_t i=0; i<pollfd_max; i++){
			إذا(pollfds[i].fd == INVALID_SOCKET){
				pollfds[i].fd = context->sock;
				pollfds[i].events = POLLIN;
				pollfds[i].revents = 0;
				context->pollfd_شاخص = (صحيح)i;
				إذا(i > pollfd_current_max){
					pollfd_current_max = i;
				}
				اكسر;
			}
		}
	}
	context->events = evt;

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح mux_poll_جمع_کن_بیرون(بنية mosquitto *context)
{
	أرجع mux_poll_جمع_کن(context, POLLIN | POLLOUT);
}


صحيح mux_poll_remove_بیرون(بنية mosquitto *context)
{
	إذا(context->events & POLLOUT){
		أرجع mux_poll__new(context);
	}وإلا{
		أرجع MOSQ_ERR_SUCCESS;
	}
}


صحيح mux_poll__new(بنية mosquitto *context)
{
	أرجع mux_poll_جمع_کن(context, POLLIN);
}


صحيح mux_poll__delete(بنية mosquitto *context)
{
	size_t pollfd_شاخص;

	إذا(context->pollfd_شاخص != -1){
		pollfds[context->pollfd_شاخص].fd = INVALID_SOCKET;
		pollfds[context->pollfd_شاخص].events = 0;
		pollfds[context->pollfd_شاخص].revents = 0;
		pollfd_شاخص = (size_t )context->pollfd_شاخص;
		context->pollfd_شاخص = -1;

		/* If this is the highest index, reduce the current max until we find
		 * the next highest in use index. */
		طالما(pollfd_شاخص == pollfd_current_max
				&& pollfd_شاخص > 0
				&& pollfds[pollfd_شاخص].fd == INVALID_SOCKET){

			pollfd_شاخص--;
			pollfd_current_max--;
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح mux_poll__handle(بنية mosquitto__listener_sock *listensock, صحيح listensock_شمار)
{
	بنية mosquitto *context;
	صحيح fdcount;
	صحيح timeout;

#اگر_تعریف_شده WITH_WEBSOCKETS
	timeout = 100;
#وإلا
	timeout = db.بعدی_event_ms;
#پایان_اگر

#اگر_تعریف_نشده WIN32
	fdcount = poll(pollfds, pollfd_current_max+1, timeout);
#وإلا
	fdcount = WSAPoll(pollfds, pollfd_current_max+1, timeout);
#پایان_اگر

	db.now_s = mosquitto_زمان();
	db.now_real_s = زمان(عدم);

	إذا(fdcount == -1){
#  إذا defined(WIN32) && defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		إذا(WSAGet_آخرین_Error() == WSAEINVAL){
			/* WSAPoll() immediately returns an error if it is not given
			 * any sockets to wait on. This can happen if we only have
			 * websockets listeners. Sleep a little to prevent a busy loop.
			 */
			Sleep(10);
		}وإلا
#  endif
		{
			log__printf(عدم, MOSQ_LOG_ERR, "Error in poll: %s.", strerror(errno));
		}
	}وإلا{
		loop_handle_reads_writes();

		لكل(صحيح i=0; i<listensock_شمار; i++){
			إذا(pollfds[i].revents & POLLIN){
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				إذا(listensock[i].listener->ws_context){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
				}وإلا
#پایان_اگر
				{
					طالما((context = net__socket_accept(&listensock[i])) != عدم){
					}
				}
			}
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح mux_poll__cleanup(فراغ)
{
	mosquitto_FREE(pollfds);

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن فراغ loop_handle_reads_writes(فراغ)
{
	بنية mosquitto *context, *ctxt_tmp;
	صحيح err;
	socklen_t len;
	صحيح rc;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		إذا(context->pollfd_شاخص < 0){
			استمر;
		}

		إذا(pollfds[context->pollfd_شاخص].fd == INVALID_SOCKET){
			استمر;
		}

		أكد(pollfds[context->pollfd_شاخص].fd == context->sock);

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		إذا(context->wsi){
			بنية lws_pollfd wspoll;
			wspoll.fd = pollfds[context->pollfd_شاخص].fd;
			wspoll.events = pollfds[context->pollfd_شاخص].events;
			wspoll.revents = pollfds[context->pollfd_شاخص].revents;
			lws_service_fd(lws_get_context(context->wsi), &wspoll);
			استمر;
		}
#پایان_اگر

#اگر_تعریف_شده WITH_TLS
		إذا(pollfds[context->pollfd_شاخص].revents & POLLOUT ||
				context->want_write ||
				(context->ssl && context->state == mosq_cs_new)){
#وإلا
		إذا(pollfds[context->pollfd_شاخص].revents & POLLOUT){
#پایان_اگر
			إذا(context->state == mosq_cs_connect_pending){
				len = حجم(صحيح);
				إذا(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (حرف *)&err, &len)){
					إذا(err == 0){
						mosquitto_مجموعه_state(context, mosq_cs_new);
#إذا defined(WITH_ADNS) && defined(WITH_BRIDGE)
						إذا(context->bridge){
							bridge__connect_step3(context);
							استمر;
						}
#پایان_اگر
					}
				}وإلا{
					do_disconnect(context, MOSQ_ERR_CONN_گمشده);
					استمر;
				}
			}
			rc = packet__write(context);
			إذا(rc){
				do_disconnect(context, rc);
				استمر;
			}
		}
	}

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		إذا(context->pollfd_شاخص < 0){
			استمر;
		}
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		إذا(context->wsi){
			// Websocket are already handled above
			استمر;
		}
#پایان_اگر

#اگر_تعریف_شده WITH_TLS
		إذا(pollfds[context->pollfd_شاخص].revents & POLLIN ||
				(context->ssl && context->state == mosq_cs_new)){
#وإلا
		إذا(pollfds[context->pollfd_شاخص].revents & POLLIN){
#پایان_اگر
			افعل{
				بدل(context->transport){
					حالة mosq_t_tcp:
					حالة mosq_t_ws:
						rc = packet__read(context);
						اكسر;
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
					حالة mosq_t_http:
						rc = http__read(context);
						اكسر;
#پایان_اگر
#إذا !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
					/* Not supported with LWS */
					حالة mosq_t_proxy_v2:
						rc = proxy_v2__read(context);
						اكسر;
					حالة mosq_t_proxy_v1:
						rc = proxy_v1__read(context);
						اكسر;
#پایان_اگر
					افتراضي:
						rc = MOSQ_ERR_INVAL;
						اكسر;
				}
				إذا(rc){
					do_disconnect(context, rc);
					استمر;
				}
			}طالما(SSL_داده_PENDING(context));
		}وإلا{
			إذا(context->pollfd_شاخص >= 0 && pollfds[context->pollfd_شاخص].revents & (POLLERR | POLLNVAL | POLLHUP)){
				do_disconnect(context, MOSQ_ERR_CONN_گمشده);
				استمر;
			}
		}
	}
}


#پایان_اگر
