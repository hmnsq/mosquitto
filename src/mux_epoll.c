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
   Tatsuzo Osawa - Add epoll.
*/

#అన్తర్భావ "config.h"

#నిర్వచితమైతే WITH_EPOLL

#నిర్వచితం_కాకపోతే WIN32
#  define GNU_ఆధారం
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే WIN32
#  include <sys/epoll.h>
#  define MAX_EVENTS 1000
#అయితే_ముగింపు

#అన్తర్భావ <errno.h>
#అన్తర్భావ <signal.h>

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mux.h"
#అన్తర్భావ "packet_mosq.h"
#అన్తర్భావ "util_mosq.h"

స్థిర శూన్య loop_handle_reads_writes(నిర్మాణం mosquitto *context, uint32_t events);

స్థిర నిర్మాణం epoll_event ep_events[MAX_EVENTS];


పూర్ణాంకం mux_epoll__init(శూన్య)
{
	memset(&ep_events, 0, పరిమాణం(నిర్మాణం epoll_event)*MAX_EVENTS);

	db.epollfd = 0;
	ఒకవేళ((db.epollfd = epoll_create(MAX_EVENTS)) == -1){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error in epoll creating: %s", strerror(errno));
		తిరిగి MOSQ_ERR_UNKNOWN;
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం mux_epoll_కూడు_listeners(నిర్మాణం mosquitto__listener_sock *listensock, పూర్ణాంకం listensock_లెక్క)
{
	కొరకు(పూర్ణాంకం i=0; i<listensock_లెక్క; i++){
		నిర్మాణం epoll_event ev;

		memset(&ev, 0, పరిమాణం(నిర్మాణం epoll_event));
		ev.దత్తాంశం.ptr = &listensock[i];
		ev.events = EPOLLIN;
		ఒకవేళ(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, listensock[i].sock, &ev) == -1){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error in epoll initial registering: %s", strerror(errno));
			తిరిగి MOSQ_ERR_UNKNOWN;
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం mux_epoll__delete_listeners(నిర్మాణం mosquitto__listener_sock *listensock, పూర్ణాంకం listensock_లెక్క)
{
	కొరకు(పూర్ణాంకం i=0; i<listensock_లెక్క; i++){
		ఒకవేళ(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, listensock[i].sock, ఖాళీ) == -1){
			తిరిగి MOSQ_ERR_UNKNOWN;
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం mux_epoll_కూడు_బహిర్గతం(నిర్మాణం mosquitto *context)
{
	ఒకవేళ(!(context->events & EPOLLOUT)){
		నిర్మాణం epoll_event ev;

		memset(&ev, 0, పరిమాణం(నిర్మాణం epoll_event));
		ev.దత్తాంశం.ptr = context;
		ev.events = EPOLLIN | EPOLLOUT;
		ఒకవేళ(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			ఒకవేళ((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(ఖాళీ, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLOUT: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN | EPOLLOUT;
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం mux_epoll_remove_బహిర్గతం(నిర్మాణం mosquitto *context)
{
	ఒకవేళ(context->events & EPOLLOUT){
		నిర్మాణం epoll_event ev;

		memset(&ev, 0, పరిమాణం(నిర్మాణం epoll_event));
		ev.దత్తాంశం.ptr = context;
		ev.events = EPOLLIN;
		ఒకవేళ(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			ఒకవేళ((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(ఖాళీ, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLIN: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN;
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం mux_epoll__new(నిర్మాణం mosquitto *context)
{
	నిర్మాణం epoll_event ev;

	memset(&ev, 0, పరిమాణం(నిర్మాణం epoll_event));
	ev.events = EPOLLIN;
	ev.దత్తాంశం.ptr = context;
	ఒకవేళ(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1){
		ఒకవేళ(errno != EEXIST){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error in epoll accepting: %s", strerror(errno));
		}
	}
	context->events = EPOLLIN;
	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం mux_epoll__delete(నిర్మాణం mosquitto *context)
{
	ఒకవేళ(context->sock != INVALID_SOCKET){
		ఒకవేళ(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, context->sock, ఖాళీ) == -1){
			తిరిగి 1;
		}
	}
	తిరిగి 0;
}


పూర్ణాంకం mux_epoll__handle(శూన్య)
{
	నిర్మాణం epoll_event ev;
	నిర్మాణం mosquitto *context;
	నిర్మాణం mosquitto__listener_sock *listensock;
	పూర్ణాంకం event_లెక్క;

	memset(&ev, 0, పరిమాణం(నిర్మాణం epoll_event));
#ఒకవేళ defined(WITH_WEBSOCKETS)
	event_లెక్క = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, 100);
#లేకపోతే
	event_లెక్క = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, db.తదుపరి_event_ms);
#అయితే_ముగింపు

	db.now_s = mosquitto_సమయం();
	db.now_real_s = సమయం(ఖాళీ);

	ఎంపిక(event_లెక్క){
		స్థితి -1:
			ఒకవేళ(errno != EINTR){
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error in epoll waiting: %s.", strerror(errno));
			}
			ఆపు;
		స్థితి 0:
			ఆపు;
		అప్రమేయ:
			కొరకు(పూర్ణాంకం i=0; i<event_లెక్క; i++){
				context = ep_events[i].దత్తాంశం.ptr;
				ఒకవేళ(context->ident == id_client){
					loop_handle_reads_writes(context, ep_events[i].events);
				}లేకపోతే ఒకవేళ(context->ident == id_listener){
					listensock = ep_events[i].దత్తాంశం.ptr;

					ఒకవేళ(ep_events[i].events & (EPOLLIN | EPOLLPRI)){
						అయితే((context = net__socket_accept(listensock)) != ఖాళీ){
						}
					}
#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				}లేకపోతే ఒకవేళ(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#అయితే_ముగింపు
				}
			}
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం mux_epoll__cleanup(శూన్య)
{
	(శూన్య)close(db.epollfd);
	db.epollfd = 0;
	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర శూన్య loop_handle_reads_writes(నిర్మాణం mosquitto *context, uint32_t events)
{
	పూర్ణాంకం err;
	socklen_t len;
	పూర్ణాంకం rc;

	ఒకవేళ(context->sock == INVALID_SOCKET){
		తిరిగి;
	}

#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ఒకవేళ(context->wsi){
		నిర్మాణం lws_pollfd wspoll;
		wspoll.fd = context->sock;
		wspoll.events = (int16_t)context->events;
		wspoll.revents = (int16_t)events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		తిరిగి;
	}
#అయితే_ముగింపు

	ఒకవేళ(events & EPOLLOUT
#నిర్వచితమైతే WITH_TLS
			|| context->want_write
			|| (context->ssl && context->state == mosq_cs_new)
#అయితే_ముగింపు
			){

		ఒకవేళ(context->state == mosq_cs_connect_pending){
			len = పరిమాణం(పూర్ణాంకం);
			ఒకవేళ(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (అక్షరం *)&err, &len)){
				ఒకవేళ(err == 0){
					mosquitto_సమితి_state(context, mosq_cs_new);
#ఒకవేళ defined(WITH_ADNS) && defined(WITH_BRIDGE)
					ఒకవేళ(context->bridge){
						bridge__connect_step3(context);
					}
#అయితే_ముగింపు
				}
			}లేకపోతే{
				do_disconnect(context, MOSQ_ERR_CONN_కోల్పోయిన);
				తిరిగి;
			}
		}
		rc = packet__write(context);
		ఒకవేళ(rc){
			do_disconnect(context, rc);
			తిరిగి;
		}
	}

	ఒకవేళ(events & EPOLLIN
#నిర్వచితమైతే WITH_TLS
			|| (context->ssl && context->state == mosq_cs_new)
#అయితే_ముగింపు
			){

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
				తిరిగి;
			}
		}అయితే(SSL_దత్తాంశం_PENDING(context));
	}లేకపోతే{
		ఒకవేళ(events & (EPOLLERR | EPOLLHUP)){
			do_disconnect(context, MOSQ_ERR_CONN_కోల్పోయిన);
			తిరిగి;
		}
	}
}
#అయితే_ముగింపు
