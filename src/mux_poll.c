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

#అన్తర్భావ "config.h"

#ఒకవేళ !defined(WITH_EPOLL) && !defined(WITH_KQUEUE)

#నిర్వచితం_కాకపోతే WIN32
#  define GNU_ఆధారం
#అయితే_ముగింపు

#అన్తర్భావ <assert.h>
#నిర్వచితం_కాకపోతే WIN32
#అన్తర్భావ <poll.h>
#అన్తర్భావ <unistd.h>
#లేకపోతే
#అన్తర్భావ <process.h>
#అన్తర్భావ <winsock2.h>
#అన్తర్భావ <ws2tcpip.h>
#అయితే_ముగింపు

#అన్తర్భావ <errno.h>
#అన్తర్భావ <signal.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>
#నిర్వచితం_కాకపోతే WIN32
#  include <sys/socket.h>
#అయితే_ముగింపు
#అన్తర్భావ <time.h>

#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#అయితే_ముగింపు

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "packet_mosq.h"
#అన్తర్భావ "send_mosq.h"
#అన్తర్భావ "sys_tree.h"
#అన్తర్భావ "util_mosq.h"
#అన్తర్భావ "mux.h"

స్థిర శూన్య loop_handle_reads_writes(శూన్య);

స్థిర నిర్మాణం pollfd *pollfds = ఖాళీ;
స్థిర size_t pollfd_max, pollfd_current_max = 0;


పూర్ణాంకం mux_poll__init(శూన్య)
{
#నిర్వచితమైతే WIN32
	pollfd_max = (size_t)_getmaxstdio();
#లేకపోతే
	pollfd_max = (size_t)sysconf(_SC_OPEN_MAX);
#అయితే_ముగింపు

	pollfds = mosquitto_calloc(pollfd_max, పరిమాణం(నిర్మాణం pollfd));
	ఒకవేళ(!pollfds){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
		తిరిగి MOSQ_ERR_NOMEM;
	}
	memset(pollfds, 0, పరిమాణం(నిర్మాణం pollfd)*pollfd_max);
	కొరకు(size_t i=0; i<pollfd_max; i++){
		pollfds[i].fd = INVALID_SOCKET;
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం mux_poll_కూడు_listeners(నిర్మాణం mosquitto__listener_sock *listensock, పూర్ణాంకం listensock_లెక్క)
{
	size_t pollfd_సూచీ = 0;

	కొరకు(size_t i=0; i<(size_t )listensock_లెక్క; i++){
		pollfds[pollfd_సూచీ].fd = listensock[i].sock;
		pollfds[pollfd_సూచీ].events = POLLIN;
		pollfds[pollfd_సూచీ].revents = 0;
		pollfd_సూచీ++;
	}

	ఒకవేళ(pollfd_current_max == 0){
		pollfd_current_max = pollfd_సూచీ-1;
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం mux_poll__delete_listeners(నిర్మాణం mosquitto__listener_sock *listensock, పూర్ణాంకం listensock_లెక్క)
{
	size_t pollfd_సూచీ = 0;

	వాడని(listensock);

	కొరకు(size_t i=0; i<(size_t )listensock_లెక్క; i++){
		pollfds[pollfd_సూచీ].fd = INVALID_SOCKET;
		pollfds[pollfd_సూచీ].events = 0;
		pollfds[pollfd_సూచీ].revents = 0;
		pollfd_సూచీ++;
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం mux_poll_కూడు(నిర్మాణం mosquitto *context, uint16_t evt)
{
	ఒకవేళ(context->events == evt){
		తిరిగి MOSQ_ERR_SUCCESS;
	}

	ఒకవేళ(context->pollfd_సూచీ != -1){
		pollfds[context->pollfd_సూచీ].fd = context->sock;
		pollfds[context->pollfd_సూచీ].events = (పొట్టి పూర్ణాంకం)evt;
		pollfds[context->pollfd_సూచీ].revents = 0;
	}లేకపోతే{
		కొరకు(size_t i=0; i<pollfd_max; i++){
			ఒకవేళ(pollfds[i].fd == INVALID_SOCKET){
				pollfds[i].fd = context->sock;
				pollfds[i].events = POLLIN;
				pollfds[i].revents = 0;
				context->pollfd_సూచీ = (పూర్ణాంకం)i;
				ఒకవేళ(i > pollfd_current_max){
					pollfd_current_max = i;
				}
				ఆపు;
			}
		}
	}
	context->events = evt;

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం mux_poll_కూడు_బహిర్గతం(నిర్మాణం mosquitto *context)
{
	తిరిగి mux_poll_కూడు(context, POLLIN | POLLOUT);
}


పూర్ణాంకం mux_poll_remove_బహిర్గతం(నిర్మాణం mosquitto *context)
{
	ఒకవేళ(context->events & POLLOUT){
		తిరిగి mux_poll__new(context);
	}లేకపోతే{
		తిరిగి MOSQ_ERR_SUCCESS;
	}
}


పూర్ణాంకం mux_poll__new(నిర్మాణం mosquitto *context)
{
	తిరిగి mux_poll_కూడు(context, POLLIN);
}


పూర్ణాంకం mux_poll__delete(నిర్మాణం mosquitto *context)
{
	size_t pollfd_సూచీ;

	ఒకవేళ(context->pollfd_సూచీ != -1){
		pollfds[context->pollfd_సూచీ].fd = INVALID_SOCKET;
		pollfds[context->pollfd_సూచీ].events = 0;
		pollfds[context->pollfd_సూచీ].revents = 0;
		pollfd_సూచీ = (size_t )context->pollfd_సూచీ;
		context->pollfd_సూచీ = -1;

		/* If this is the highest index, reduce the current max until we find
		 * the next highest in use index. */
		అయితే(pollfd_సూచీ == pollfd_current_max
				&& pollfd_సూచీ > 0
				&& pollfds[pollfd_సూచీ].fd == INVALID_SOCKET){

			pollfd_సూచీ--;
			pollfd_current_max--;
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం mux_poll__handle(నిర్మాణం mosquitto__listener_sock *listensock, పూర్ణాంకం listensock_లెక్క)
{
	నిర్మాణం mosquitto *context;
	పూర్ణాంకం fdcount;
	పూర్ణాంకం timeout;

#నిర్వచితమైతే WITH_WEBSOCKETS
	timeout = 100;
#లేకపోతే
	timeout = db.తదుపరి_event_ms;
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే WIN32
	fdcount = poll(pollfds, pollfd_current_max+1, timeout);
#లేకపోతే
	fdcount = WSAPoll(pollfds, pollfd_current_max+1, timeout);
#అయితే_ముగింపు

	db.now_s = mosquitto_సమయం();
	db.now_real_s = సమయం(ఖాళీ);

	ఒకవేళ(fdcount == -1){
#  ఒకవేళ defined(WIN32) && defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ఒకవేళ(WSAGet_చివరి_Error() == WSAEINVAL){
			/* WSAPoll() immediately returns an error if it is not given
			 * any sockets to wait on. This can happen if we only have
			 * websockets listeners. Sleep a little to prevent a busy loop.
			 */
			Sleep(10);
		}లేకపోతే
#  endif
		{
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error in poll: %s.", strerror(errno));
		}
	}లేకపోతే{
		loop_handle_reads_writes();

		కొరకు(పూర్ణాంకం i=0; i<listensock_లెక్క; i++){
			ఒకవేళ(pollfds[i].revents & POLLIN){
#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				ఒకవేళ(listensock[i].listener->ws_context){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
				}లేకపోతే
#అయితే_ముగింపు
				{
					అయితే((context = net__socket_accept(&listensock[i])) != ఖాళీ){
					}
				}
			}
		}
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం mux_poll__cleanup(శూన్య)
{
	mosquitto_FREE(pollfds);

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర శూన్య loop_handle_reads_writes(శూన్య)
{
	నిర్మాణం mosquitto *context, *ctxt_tmp;
	పూర్ణాంకం err;
	socklen_t len;
	పూర్ణాంకం rc;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		ఒకవేళ(context->pollfd_సూచీ < 0){
			కొనసాగు;
		}

		ఒకవేళ(pollfds[context->pollfd_సూచీ].fd == INVALID_SOCKET){
			కొనసాగు;
		}

		ప్రతిజ్ఞా(pollfds[context->pollfd_సూచీ].fd == context->sock);

#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ఒకవేళ(context->wsi){
			నిర్మాణం lws_pollfd wspoll;
			wspoll.fd = pollfds[context->pollfd_సూచీ].fd;
			wspoll.events = pollfds[context->pollfd_సూచీ].events;
			wspoll.revents = pollfds[context->pollfd_సూచీ].revents;
			lws_service_fd(lws_get_context(context->wsi), &wspoll);
			కొనసాగు;
		}
#అయితే_ముగింపు

#నిర్వచితమైతే WITH_TLS
		ఒకవేళ(pollfds[context->pollfd_సూచీ].revents & POLLOUT ||
				context->want_write ||
				(context->ssl && context->state == mosq_cs_new)){
#లేకపోతే
		ఒకవేళ(pollfds[context->pollfd_సూచీ].revents & POLLOUT){
#అయితే_ముగింపు
			ఒకవేళ(context->state == mosq_cs_connect_pending){
				len = పరిమాణం(పూర్ణాంకం);
				ఒకవేళ(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (అక్షరం *)&err, &len)){
					ఒకవేళ(err == 0){
						mosquitto_సమితి_state(context, mosq_cs_new);
#ఒకవేళ defined(WITH_ADNS) && defined(WITH_BRIDGE)
						ఒకవేళ(context->bridge){
							bridge__connect_step3(context);
							కొనసాగు;
						}
#అయితే_ముగింపు
					}
				}లేకపోతే{
					do_disconnect(context, MOSQ_ERR_CONN_కోల్పోయిన);
					కొనసాగు;
				}
			}
			rc = packet__write(context);
			ఒకవేళ(rc){
				do_disconnect(context, rc);
				కొనసాగు;
			}
		}
	}

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		ఒకవేళ(context->pollfd_సూచీ < 0){
			కొనసాగు;
		}
#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ఒకవేళ(context->wsi){
			// Websocket are already handled above
			కొనసాగు;
		}
#అయితే_ముగింపు

#నిర్వచితమైతే WITH_TLS
		ఒకవేళ(pollfds[context->pollfd_సూచీ].revents & POLLIN ||
				(context->ssl && context->state == mosq_cs_new)){
#లేకపోతే
		ఒకవేళ(pollfds[context->pollfd_సూచీ].revents & POLLIN){
#అయితే_ముగింపు
			చేయి{
				ఎంపిక(context->transport){
					స్థితి mosq_t_tcp:
					స్థితి mosq_t_ws:
						rc = packet__read(context);
						ఆపు;
#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
					స్థితి mosq_t_http:
						rc = http__read(context);
						ఆపు;
#అయితే_ముగింపు
#ఒకవేళ !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
					/* Not supported with LWS */
					స్థితి mosq_t_proxy_v2:
						rc = proxy_v2__read(context);
						ఆపు;
					స్థితి mosq_t_proxy_v1:
						rc = proxy_v1__read(context);
						ఆపు;
#అయితే_ముగింపు
					అప్రమేయ:
						rc = MOSQ_ERR_INVAL;
						ఆపు;
				}
				ఒకవేళ(rc){
					do_disconnect(context, rc);
					కొనసాగు;
				}
			}అయితే(SSL_దత్తాంశం_PENDING(context));
		}లేకపోతే{
			ఒకవేళ(context->pollfd_సూచీ >= 0 && pollfds[context->pollfd_సూచీ].revents & (POLLERR | POLLNVAL | POLLHUP)){
				do_disconnect(context, MOSQ_ERR_CONN_కోల్పోయిన);
				కొనసాగు;
			}
		}
	}
}


#అయితే_ముగింపు
