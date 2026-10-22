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

#اگر_غیر_تعریف_شدہ WIN32
#  define GNU_ماخذ
#اگر_ختم

#شامل <assert.h>
#اگر_غیر_تعریف_شدہ WIN32
#شامل <poll.h>
#شامل <unistd.h>
#ورنہ
#شامل <process.h>
#شامل <winsock2.h>
#شامل <ws2tcpip.h>
#اگر_ختم

#شامل <errno.h>
#شامل <signal.h>
#شامل <stdio.h>
#شامل <string.h>
#اگر_غیر_تعریف_شدہ WIN32
#  include <sys/socket.h>
#اگر_ختم
#شامل <time.h>

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#اگر_ختم

#شامل "mosquitto_broker_internal.h"
#شامل "packet_mosq.h"
#شامل "send_mosq.h"
#شامل "sys_tree.h"
#شامل "util_mosq.h"
#شامل "mux.h"

ساکن خالی loop_handle_reads_writes(خالی);

ساکن ساخت pollfd *pollfds = تہی;
ساکن size_t pollfd_max, pollfd_current_max = 0;


صحیح mux_poll__init(خالی)
{
#اگر_تعریف_شدہ WIN32
	pollfd_max = (size_t)_getmaxstdio();
#ورنہ
	pollfd_max = (size_t)sysconf(_SC_OPEN_MAX);
#اگر_ختم

	pollfds = mosquitto_calloc(pollfd_max, حجم(ساخت pollfd));
	اگر(!pollfds){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
		واپس MOSQ_ERR_NOMEM;
	}
	memset(pollfds, 0, حجم(ساخت pollfd)*pollfd_max);
	برائے(size_t i=0; i<pollfd_max; i++){
		pollfds[i].fd = INVALID_SOCKET;
	}

	واپس MOSQ_ERR_SUCCESS;
}


صحیح mux_poll_جوڑو_listeners(ساخت mosquitto__listener_sock *listensock, صحیح listensock_گنتی)
{
	size_t pollfd_اشاریہ = 0;

	برائے(size_t i=0; i<(size_t )listensock_گنتی; i++){
		pollfds[pollfd_اشاریہ].fd = listensock[i].sock;
		pollfds[pollfd_اشاریہ].events = POLLIN;
		pollfds[pollfd_اشاریہ].revents = 0;
		pollfd_اشاریہ++;
	}

	اگر(pollfd_current_max == 0){
		pollfd_current_max = pollfd_اشاریہ-1;
	}
	واپس MOSQ_ERR_SUCCESS;
}


صحیح mux_poll__delete_listeners(ساخت mosquitto__listener_sock *listensock, صحیح listensock_گنتی)
{
	size_t pollfd_اشاریہ = 0;

	غیر_مستعمل(listensock);

	برائے(size_t i=0; i<(size_t )listensock_گنتی; i++){
		pollfds[pollfd_اشاریہ].fd = INVALID_SOCKET;
		pollfds[pollfd_اشاریہ].events = 0;
		pollfds[pollfd_اشاریہ].revents = 0;
		pollfd_اشاریہ++;
	}

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح mux_poll_جوڑو(ساخت mosquitto *context, uint16_t evt)
{
	اگر(context->events == evt){
		واپس MOSQ_ERR_SUCCESS;
	}

	اگر(context->pollfd_اشاریہ != -1){
		pollfds[context->pollfd_اشاریہ].fd = context->sock;
		pollfds[context->pollfd_اشاریہ].events = (مختصر صحیح)evt;
		pollfds[context->pollfd_اشاریہ].revents = 0;
	}ورنہ{
		برائے(size_t i=0; i<pollfd_max; i++){
			اگر(pollfds[i].fd == INVALID_SOCKET){
				pollfds[i].fd = context->sock;
				pollfds[i].events = POLLIN;
				pollfds[i].revents = 0;
				context->pollfd_اشاریہ = (صحیح)i;
				اگر(i > pollfd_current_max){
					pollfd_current_max = i;
				}
				توڑو;
			}
		}
	}
	context->events = evt;

	واپس MOSQ_ERR_SUCCESS;
}


صحیح mux_poll_جوڑو_برآمد(ساخت mosquitto *context)
{
	واپس mux_poll_جوڑو(context, POLLIN | POLLOUT);
}


صحیح mux_poll_remove_برآمد(ساخت mosquitto *context)
{
	اگر(context->events & POLLOUT){
		واپس mux_poll__new(context);
	}ورنہ{
		واپس MOSQ_ERR_SUCCESS;
	}
}


صحیح mux_poll__new(ساخت mosquitto *context)
{
	واپس mux_poll_جوڑو(context, POLLIN);
}


صحیح mux_poll__delete(ساخت mosquitto *context)
{
	size_t pollfd_اشاریہ;

	اگر(context->pollfd_اشاریہ != -1){
		pollfds[context->pollfd_اشاریہ].fd = INVALID_SOCKET;
		pollfds[context->pollfd_اشاریہ].events = 0;
		pollfds[context->pollfd_اشاریہ].revents = 0;
		pollfd_اشاریہ = (size_t )context->pollfd_اشاریہ;
		context->pollfd_اشاریہ = -1;

		/* If this is the highest index, reduce the current max until we find
		 * the next highest in use index. */
		جب(pollfd_اشاریہ == pollfd_current_max
				&& pollfd_اشاریہ > 0
				&& pollfds[pollfd_اشاریہ].fd == INVALID_SOCKET){

			pollfd_اشاریہ--;
			pollfd_current_max--;
		}
	}

	واپس MOSQ_ERR_SUCCESS;
}


صحیح mux_poll__handle(ساخت mosquitto__listener_sock *listensock, صحیح listensock_گنتی)
{
	ساخت mosquitto *context;
	صحیح fdcount;
	صحیح timeout;

#اگر_تعریف_شدہ WITH_WEBSOCKETS
	timeout = 100;
#ورنہ
	timeout = db.اگلا_event_ms;
#اگر_ختم

#اگر_غیر_تعریف_شدہ WIN32
	fdcount = poll(pollfds, pollfd_current_max+1, timeout);
#ورنہ
	fdcount = WSAPoll(pollfds, pollfd_current_max+1, timeout);
#اگر_ختم

	db.now_s = mosquitto_وقت();
	db.now_real_s = وقت(تہی);

	اگر(fdcount == -1){
#  اگر defined(WIN32) && defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		اگر(WSAGet_آخری_Error() == WSAEINVAL){
			/* WSAPoll() immediately returns an error if it is not given
			 * any sockets to wait on. This can happen if we only have
			 * websockets listeners. Sleep a little to prevent a busy loop.
			 */
			Sleep(10);
		}ورنہ
#  endif
		{
			log__printf(تہی, MOSQ_LOG_ERR, "Error in poll: %s.", strerror(errno));
		}
	}ورنہ{
		loop_handle_reads_writes();

		برائے(صحیح i=0; i<listensock_گنتی; i++){
			اگر(pollfds[i].revents & POLLIN){
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				اگر(listensock[i].listener->ws_context){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
				}ورنہ
#اگر_ختم
				{
					جب((context = net__socket_accept(&listensock[i])) != تہی){
					}
				}
			}
		}
	}
	واپس MOSQ_ERR_SUCCESS;
}


صحیح mux_poll__cleanup(خالی)
{
	mosquitto_FREE(pollfds);

	واپس MOSQ_ERR_SUCCESS;
}


ساکن خالی loop_handle_reads_writes(خالی)
{
	ساخت mosquitto *context, *ctxt_tmp;
	صحیح err;
	socklen_t len;
	صحیح rc;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		اگر(context->pollfd_اشاریہ < 0){
			جاری;
		}

		اگر(pollfds[context->pollfd_اشاریہ].fd == INVALID_SOCKET){
			جاری;
		}

		أكد(pollfds[context->pollfd_اشاریہ].fd == context->sock);

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		اگر(context->wsi){
			ساخت lws_pollfd wspoll;
			wspoll.fd = pollfds[context->pollfd_اشاریہ].fd;
			wspoll.events = pollfds[context->pollfd_اشاریہ].events;
			wspoll.revents = pollfds[context->pollfd_اشاریہ].revents;
			lws_service_fd(lws_get_context(context->wsi), &wspoll);
			جاری;
		}
#اگر_ختم

#اگر_تعریف_شدہ WITH_TLS
		اگر(pollfds[context->pollfd_اشاریہ].revents & POLLOUT ||
				context->want_write ||
				(context->ssl && context->state == mosq_cs_new)){
#ورنہ
		اگر(pollfds[context->pollfd_اشاریہ].revents & POLLOUT){
#اگر_ختم
			اگر(context->state == mosq_cs_connect_pending){
				len = حجم(صحیح);
				اگر(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (حرف *)&err, &len)){
					اگر(err == 0){
						mosquitto_سیٹ_state(context, mosq_cs_new);
#اگر defined(WITH_ADNS) && defined(WITH_BRIDGE)
						اگر(context->bridge){
							bridge__connect_step3(context);
							جاری;
						}
#اگر_ختم
					}
				}ورنہ{
					do_disconnect(context, MOSQ_ERR_CONN_گمشدہ);
					جاری;
				}
			}
			rc = packet__write(context);
			اگر(rc){
				do_disconnect(context, rc);
				جاری;
			}
		}
	}

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		اگر(context->pollfd_اشاریہ < 0){
			جاری;
		}
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		اگر(context->wsi){
			// Websocket are already handled above
			جاری;
		}
#اگر_ختم

#اگر_تعریف_شدہ WITH_TLS
		اگر(pollfds[context->pollfd_اشاریہ].revents & POLLIN ||
				(context->ssl && context->state == mosq_cs_new)){
#ورنہ
		اگر(pollfds[context->pollfd_اشاریہ].revents & POLLIN){
#اگر_ختم
			کرو{
				انتخاب(context->transport){
					حالت mosq_t_tcp:
					حالت mosq_t_ws:
						rc = packet__read(context);
						توڑو;
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
					حالت mosq_t_http:
						rc = http__read(context);
						توڑو;
#اگر_ختم
#اگر !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
					/* Not supported with LWS */
					حالت mosq_t_proxy_v2:
						rc = proxy_v2__read(context);
						توڑو;
					حالت mosq_t_proxy_v1:
						rc = proxy_v1__read(context);
						توڑو;
#اگر_ختم
					طےشدہ:
						rc = MOSQ_ERR_INVAL;
						توڑو;
				}
				اگر(rc){
					do_disconnect(context, rc);
					جاری;
				}
			}جب(SSL_ڈیٹا_PENDING(context));
		}ورنہ{
			اگر(context->pollfd_اشاریہ >= 0 && pollfds[context->pollfd_اشاریہ].revents & (POLLERR | POLLNVAL | POLLHUP)){
				do_disconnect(context, MOSQ_ERR_CONN_گمشدہ);
				جاری;
			}
		}
	}
}


#اگر_ختم
