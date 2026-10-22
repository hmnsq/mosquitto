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

#جے_بنا_تعریف WIN32
#  define GNU_سروت
#جے_انت

#ضمن <assert.h>
#جے_بنا_تعریف WIN32
#ضمن <poll.h>
#ضمن <unistd.h>
#وإلا
#ضمن <process.h>
#ضمن <winsock2.h>
#ضمن <ws2tcpip.h>
#جے_انت

#ضمن <errno.h>
#ضمن <signal.h>
#ضمن <stdio.h>
#ضمن <string.h>
#جے_بنا_تعریف WIN32
#  include <sys/socket.h>
#جے_انت
#ضمن <time.h>

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#جے_انت

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
#جے_تعریف_شدہ WIN32
	pollfd_max = (size_t)_getmaxstdio();
#وإلا
	pollfd_max = (size_t)sysconf(_SC_OPEN_MAX);
#جے_انت

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


صحيح mux_poll_جوڑو_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_گنتی)
{
	size_t pollfd_اشاریہ = 0;

	لكل(size_t i=0; i<(size_t )listensock_گنتی; i++){
		pollfds[pollfd_اشاریہ].fd = listensock[i].sock;
		pollfds[pollfd_اشاریہ].events = POLLIN;
		pollfds[pollfd_اشاریہ].revents = 0;
		pollfd_اشاریہ++;
	}

	إذا(pollfd_current_max == 0){
		pollfd_current_max = pollfd_اشاریہ-1;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح mux_poll__delete_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_گنتی)
{
	size_t pollfd_اشاریہ = 0;

	بن_ورتیا(listensock);

	لكل(size_t i=0; i<(size_t )listensock_گنتی; i++){
		pollfds[pollfd_اشاریہ].fd = INVALID_SOCKET;
		pollfds[pollfd_اشاریہ].events = 0;
		pollfds[pollfd_اشاریہ].revents = 0;
		pollfd_اشاریہ++;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح mux_poll_جوڑو(بنية mosquitto *context, uint16_t evt)
{
	إذا(context->events == evt){
		أرجع MOSQ_ERR_SUCCESS;
	}

	إذا(context->pollfd_اشاریہ != -1){
		pollfds[context->pollfd_اشاریہ].fd = context->sock;
		pollfds[context->pollfd_اشاریہ].events = (چھوٹا صحيح)evt;
		pollfds[context->pollfd_اشاریہ].revents = 0;
	}وإلا{
		لكل(size_t i=0; i<pollfd_max; i++){
			إذا(pollfds[i].fd == INVALID_SOCKET){
				pollfds[i].fd = context->sock;
				pollfds[i].events = POLLIN;
				pollfds[i].revents = 0;
				context->pollfd_اشاریہ = (صحيح)i;
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


صحيح mux_poll_جوڑو_باہرلا(بنية mosquitto *context)
{
	أرجع mux_poll_جوڑو(context, POLLIN | POLLOUT);
}


صحيح mux_poll_remove_باہرلا(بنية mosquitto *context)
{
	إذا(context->events & POLLOUT){
		أرجع mux_poll__new(context);
	}وإلا{
		أرجع MOSQ_ERR_SUCCESS;
	}
}


صحيح mux_poll__new(بنية mosquitto *context)
{
	أرجع mux_poll_جوڑو(context, POLLIN);
}


صحيح mux_poll__delete(بنية mosquitto *context)
{
	size_t pollfd_اشاریہ;

	إذا(context->pollfd_اشاریہ != -1){
		pollfds[context->pollfd_اشاریہ].fd = INVALID_SOCKET;
		pollfds[context->pollfd_اشاریہ].events = 0;
		pollfds[context->pollfd_اشاریہ].revents = 0;
		pollfd_اشاریہ = (size_t )context->pollfd_اشاریہ;
		context->pollfd_اشاریہ = -1;

		/* If this is the highest index, reduce the current max until we find
		 * the next highest in use index. */
		طالما(pollfd_اشاریہ == pollfd_current_max
				&& pollfd_اشاریہ > 0
				&& pollfds[pollfd_اشاریہ].fd == INVALID_SOCKET){

			pollfd_اشاریہ--;
			pollfd_current_max--;
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح mux_poll__handle(بنية mosquitto__listener_sock *listensock, صحيح listensock_گنتی)
{
	بنية mosquitto *context;
	صحيح fdcount;
	صحيح timeout;

#جے_تعریف_شدہ WITH_WEBSOCKETS
	timeout = 100;
#وإلا
	timeout = db.اگلا_event_ms;
#جے_انت

#جے_بنا_تعریف WIN32
	fdcount = poll(pollfds, pollfd_current_max+1, timeout);
#وإلا
	fdcount = WSAPoll(pollfds, pollfd_current_max+1, timeout);
#جے_انت

	db.now_s = mosquitto_ویلا();
	db.now_real_s = ویلا(عدم);

	إذا(fdcount == -1){
#  إذا defined(WIN32) && defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		إذا(WSAGet_آخری_Error() == WSAEINVAL){
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

		لكل(صحيح i=0; i<listensock_گنتی; i++){
			إذا(pollfds[i].revents & POLLIN){
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				إذا(listensock[i].listener->ws_context){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
				}وإلا
#جے_انت
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
		إذا(context->pollfd_اشاریہ < 0){
			استمر;
		}

		إذا(pollfds[context->pollfd_اشاریہ].fd == INVALID_SOCKET){
			استمر;
		}

		أكد(pollfds[context->pollfd_اشاریہ].fd == context->sock);

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		إذا(context->wsi){
			بنية lws_pollfd wspoll;
			wspoll.fd = pollfds[context->pollfd_اشاریہ].fd;
			wspoll.events = pollfds[context->pollfd_اشاریہ].events;
			wspoll.revents = pollfds[context->pollfd_اشاریہ].revents;
			lws_service_fd(lws_get_context(context->wsi), &wspoll);
			استمر;
		}
#جے_انت

#جے_تعریف_شدہ WITH_TLS
		إذا(pollfds[context->pollfd_اشاریہ].revents & POLLOUT ||
				context->want_write ||
				(context->ssl && context->state == mosq_cs_new)){
#وإلا
		إذا(pollfds[context->pollfd_اشاریہ].revents & POLLOUT){
#جے_انت
			إذا(context->state == mosq_cs_connect_pending){
				len = حجم(صحيح);
				إذا(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (حرف *)&err, &len)){
					إذا(err == 0){
						mosquitto_سیٹ_state(context, mosq_cs_new);
#إذا defined(WITH_ADNS) && defined(WITH_BRIDGE)
						إذا(context->bridge){
							bridge__connect_step3(context);
							استمر;
						}
#جے_انت
					}
				}وإلا{
					do_disconnect(context, MOSQ_ERR_CONN_گواچا);
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
		إذا(context->pollfd_اشاریہ < 0){
			استمر;
		}
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		إذا(context->wsi){
			// Websocket are already handled above
			استمر;
		}
#جے_انت

#جے_تعریف_شدہ WITH_TLS
		إذا(pollfds[context->pollfd_اشاریہ].revents & POLLIN ||
				(context->ssl && context->state == mosq_cs_new)){
#وإلا
		إذا(pollfds[context->pollfd_اشاریہ].revents & POLLIN){
#جے_انت
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
#جے_انت
#إذا !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
					/* Not supported with LWS */
					حالة mosq_t_proxy_v2:
						rc = proxy_v2__read(context);
						اكسر;
					حالة mosq_t_proxy_v1:
						rc = proxy_v1__read(context);
						اكسر;
#جے_انت
					افتراضي:
						rc = MOSQ_ERR_INVAL;
						اكسر;
				}
				إذا(rc){
					do_disconnect(context, rc);
					استمر;
				}
			}طالما(SSL_انکڑے_PENDING(context));
		}وإلا{
			إذا(context->pollfd_اشاریہ >= 0 && pollfds[context->pollfd_اشاریہ].revents & (POLLERR | POLLNVAL | POLLHUP)){
				do_disconnect(context, MOSQ_ERR_CONN_گواچا);
				استمر;
			}
		}
	}
}


#جے_انت
