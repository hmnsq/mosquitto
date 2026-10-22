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

#جيڪڏهن_وصف_نه_ٿيل WIN32
#  define GNU_ذريعو
#جيڪڏهن_پورو

#ضمن <assert.h>
#جيڪڏهن_وصف_نه_ٿيل WIN32
#ضمن <poll.h>
#ضمن <unistd.h>
#وإلا
#ضمن <process.h>
#ضمن <winsock2.h>
#ضمن <ws2tcpip.h>
#جيڪڏهن_پورو

#ضمن <errno.h>
#ضمن <signal.h>
#ضمن <stdio.h>
#ضمن <string.h>
#جيڪڏهن_وصف_نه_ٿيل WIN32
#  include <sys/socket.h>
#جيڪڏهن_پورو
#ضمن <time.h>

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#جيڪڏهن_پورو

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
#جيڪڏهن_وصف_ٿيل WIN32
	pollfd_max = (size_t)_getmaxstdio();
#وإلا
	pollfd_max = (size_t)sysconf(_SC_OPEN_MAX);
#جيڪڏهن_پورو

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


صحيح mux_poll_جوڙيو_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_ڳڻپ)
{
	size_t pollfd_اشاريو = 0;

	لكل(size_t i=0; i<(size_t )listensock_ڳڻپ; i++){
		pollfds[pollfd_اشاريو].fd = listensock[i].sock;
		pollfds[pollfd_اشاريو].events = POLLIN;
		pollfds[pollfd_اشاريو].revents = 0;
		pollfd_اشاريو++;
	}

	إذا(pollfd_current_max == 0){
		pollfd_current_max = pollfd_اشاريو-1;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح mux_poll__delete_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_ڳڻپ)
{
	size_t pollfd_اشاريو = 0;

	اڻ_استعمال_ٿيل(listensock);

	لكل(size_t i=0; i<(size_t )listensock_ڳڻپ; i++){
		pollfds[pollfd_اشاريو].fd = INVALID_SOCKET;
		pollfds[pollfd_اشاريو].events = 0;
		pollfds[pollfd_اشاريو].revents = 0;
		pollfd_اشاريو++;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح mux_poll_جوڙيو(بنية mosquitto *context, uint16_t evt)
{
	إذا(context->events == evt){
		أرجع MOSQ_ERR_SUCCESS;
	}

	إذا(context->pollfd_اشاريو != -1){
		pollfds[context->pollfd_اشاريو].fd = context->sock;
		pollfds[context->pollfd_اشاريو].events = (ننڍو صحيح)evt;
		pollfds[context->pollfd_اشاريو].revents = 0;
	}وإلا{
		لكل(size_t i=0; i<pollfd_max; i++){
			إذا(pollfds[i].fd == INVALID_SOCKET){
				pollfds[i].fd = context->sock;
				pollfds[i].events = POLLIN;
				pollfds[i].revents = 0;
				context->pollfd_اشاريو = (صحيح)i;
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


صحيح mux_poll_جوڙيو_ٻاهريون(بنية mosquitto *context)
{
	أرجع mux_poll_جوڙيو(context, POLLIN | POLLOUT);
}


صحيح mux_poll_remove_ٻاهريون(بنية mosquitto *context)
{
	إذا(context->events & POLLOUT){
		أرجع mux_poll__new(context);
	}وإلا{
		أرجع MOSQ_ERR_SUCCESS;
	}
}


صحيح mux_poll__new(بنية mosquitto *context)
{
	أرجع mux_poll_جوڙيو(context, POLLIN);
}


صحيح mux_poll__delete(بنية mosquitto *context)
{
	size_t pollfd_اشاريو;

	إذا(context->pollfd_اشاريو != -1){
		pollfds[context->pollfd_اشاريو].fd = INVALID_SOCKET;
		pollfds[context->pollfd_اشاريو].events = 0;
		pollfds[context->pollfd_اشاريو].revents = 0;
		pollfd_اشاريو = (size_t )context->pollfd_اشاريو;
		context->pollfd_اشاريو = -1;

		/* If this is the highest index, reduce the current max until we find
		 * the next highest in use index. */
		طالما(pollfd_اشاريو == pollfd_current_max
				&& pollfd_اشاريو > 0
				&& pollfds[pollfd_اشاريو].fd == INVALID_SOCKET){

			pollfd_اشاريو--;
			pollfd_current_max--;
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح mux_poll__handle(بنية mosquitto__listener_sock *listensock, صحيح listensock_ڳڻپ)
{
	بنية mosquitto *context;
	صحيح fdcount;
	صحيح timeout;

#جيڪڏهن_وصف_ٿيل WITH_WEBSOCKETS
	timeout = 100;
#وإلا
	timeout = db.ايندڙ_event_ms;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل WIN32
	fdcount = poll(pollfds, pollfd_current_max+1, timeout);
#وإلا
	fdcount = WSAPoll(pollfds, pollfd_current_max+1, timeout);
#جيڪڏهن_پورو

	db.now_s = mosquitto_وقت();
	db.now_real_s = وقت(عدم);

	إذا(fdcount == -1){
#  إذا defined(WIN32) && defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		إذا(WSAGet_آخري_Error() == WSAEINVAL){
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

		لكل(صحيح i=0; i<listensock_ڳڻپ; i++){
			إذا(pollfds[i].revents & POLLIN){
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				إذا(listensock[i].listener->ws_context){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
				}وإلا
#جيڪڏهن_پورو
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
		إذا(context->pollfd_اشاريو < 0){
			استمر;
		}

		إذا(pollfds[context->pollfd_اشاريو].fd == INVALID_SOCKET){
			استمر;
		}

		أكد(pollfds[context->pollfd_اشاريو].fd == context->sock);

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		إذا(context->wsi){
			بنية lws_pollfd wspoll;
			wspoll.fd = pollfds[context->pollfd_اشاريو].fd;
			wspoll.events = pollfds[context->pollfd_اشاريو].events;
			wspoll.revents = pollfds[context->pollfd_اشاريو].revents;
			lws_service_fd(lws_get_context(context->wsi), &wspoll);
			استمر;
		}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل WITH_TLS
		إذا(pollfds[context->pollfd_اشاريو].revents & POLLOUT ||
				context->want_write ||
				(context->ssl && context->state == mosq_cs_new)){
#وإلا
		إذا(pollfds[context->pollfd_اشاريو].revents & POLLOUT){
#جيڪڏهن_پورو
			إذا(context->state == mosq_cs_connect_pending){
				len = حجم(صحيح);
				إذا(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (حرف *)&err, &len)){
					إذا(err == 0){
						mosquitto_سيٽ_state(context, mosq_cs_new);
#إذا defined(WITH_ADNS) && defined(WITH_BRIDGE)
						إذا(context->bridge){
							bridge__connect_step3(context);
							استمر;
						}
#جيڪڏهن_پورو
					}
				}وإلا{
					do_disconnect(context, MOSQ_ERR_CONN_گم);
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
		إذا(context->pollfd_اشاريو < 0){
			استمر;
		}
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		إذا(context->wsi){
			// Websocket are already handled above
			استمر;
		}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل WITH_TLS
		إذا(pollfds[context->pollfd_اشاريو].revents & POLLIN ||
				(context->ssl && context->state == mosq_cs_new)){
#وإلا
		إذا(pollfds[context->pollfd_اشاريو].revents & POLLIN){
#جيڪڏهن_پورو
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
#جيڪڏهن_پورو
#إذا !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
					/* Not supported with LWS */
					حالة mosq_t_proxy_v2:
						rc = proxy_v2__read(context);
						اكسر;
					حالة mosq_t_proxy_v1:
						rc = proxy_v1__read(context);
						اكسر;
#جيڪڏهن_پورو
					افتراضي:
						rc = MOSQ_ERR_INVAL;
						اكسر;
				}
				إذا(rc){
					do_disconnect(context, rc);
					استمر;
				}
			}طالما(SSL_معلومات_PENDING(context));
		}وإلا{
			إذا(context->pollfd_اشاريو >= 0 && pollfds[context->pollfd_اشاريو].revents & (POLLERR | POLLNVAL | POLLHUP)){
				do_disconnect(context, MOSQ_ERR_CONN_گم);
				استمر;
			}
		}
	}
}


#جيڪڏهن_پورو
