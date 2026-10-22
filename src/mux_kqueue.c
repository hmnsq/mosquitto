/*
Copyright (c) 2021 Roger Light <roger@atchoo.org>

All rights reserved. This program and the accompanying materials
are made available under the terms of the Eclipse Public License 2.0
and Eclipse Distribution License v1.0 which accompany this distribution.

The Eclipse Public License is available at
   https://www.eclipse.org/legal/epl-2.0/
and the Eclipse Distribution License is available at
  http://www.eclipse.org/org/documents/edl-v10.php.

SPDX-License-Identifier: EPL-2.0 OR EDL-1.0

Contributors:
   Roger Light - initial implementation and documentation.
*/

#అన్తర్భావ "config.h"

#నిర్వచితమైతే WITH_KQUEUE

#నిర్వచించు MAX_EVENTS 1000

#అన్తర్భావ <errno.h>
#అన్తర్భావ <signal.h>
#అన్తర్భావ <sys/event.h>
#అన్తర్భావ <sys/socket.h>

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "packet_mosq.h"
#అన్తర్భావ "util_mosq.h"

స్థిర శూన్య loop_handle_reads_writes(నిర్మాణం mosquitto *context, పొట్టి events);

స్థిర నిర్మాణం kevent event_list[MAX_EVENTS];


పూర్ణాంకం mux_kqueue__init(శూన్య)
{
	memset(&event_list, 0, పరిమాణం(నిర్మాణం kevent)*MAX_EVENTS);

	db.kqueuefd = 0;
	ఒకవేళ((db.kqueuefd = kqueue()) == -1){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error in kqueue creating: %s", strerror(errno));
		తిరిగి MOSQ_ERR_UNKNOWN;
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం mux_kqueue_కూడు_listeners(నిర్మాణం mosquitto__listener_sock *listensock, పూర్ణాంకం listensock_లెక్క)
{
	నిర్మాణం kevent ev;

	memset(&ev, 0, పరిమాణం(నిర్మాణం kevent));

	కొరకు(పూర్ణాంకం i=0; i<listensock_లెక్క; i++){
		EV_సమితి(&ev, listensock[i].sock, EVFILT_READ, EV_కూడు, 0, 0, &listensock[i]);
		ఒకవేళ(kevent(db.kqueuefd, &ev, 1, ఖాళీ, 0, ఖాళీ) == -1){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error in kqueue initial registering: %s", strerror(errno));
			తిరిగి MOSQ_ERR_UNKNOWN;
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం mux_kqueue__delete_listeners(నిర్మాణం mosquitto__listener_sock *listensock, పూర్ణాంకం listensock_లెక్క)
{
	నిర్మాణం kevent ev;

	కొరకు(పూర్ణాంకం i=0; i<listensock_లెక్క; i++){
		EV_సమితి(&ev, listensock[i].sock, EVFILT_READ, EV_DELETE, 0, 0, &listensock[i]);
		ఒకవేళ(kevent(db.kqueuefd, &ev, 1, ఖాళీ, 0, ఖాళీ) == -1){
			తిరిగి MOSQ_ERR_UNKNOWN;
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం mux_kqueue_loop_అమరిక(శూన్య)
{
	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం mux_kqueue_కూడు_బహిర్గతం(నిర్మాణం mosquitto *context)
{
	నిర్మాణం kevent ev;

	ఒకవేళ(context->events != EVFILT_WRITE){
		EV_సమితి(&ev, context->sock, EVFILT_WRITE, EV_కూడు, 0, 0, context);
		ఒకవేళ(kevent(db.kqueuefd, &ev, 1, ఖాళీ, 0, ఖాళీ) == -1){
			log__printf(ఖాళీ, MOSQ_LOG_DEBUG, "Error in kqueue re-registering to EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = EVFILT_WRITE;
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం mux_kqueue_remove_బహిర్గతం(నిర్మాణం mosquitto *context)
{
	నిర్మాణం kevent ev;

	ఒకవేళ(context->events == EVFILT_WRITE){
		EV_సమితి(&ev, context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		ఒకవేళ(kevent(db.kqueuefd, &ev, 1, ఖాళీ, 0, ఖాళీ) == -1){
			log__printf(ఖాళీ, MOSQ_LOG_DEBUG, "Error in kqueue removing EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = 0;
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం mux_kqueue__new(నిర్మాణం mosquitto *context)
{
	నిర్మాణం kevent ev;

	EV_సమితి(&ev, context->sock, EVFILT_READ, EV_కూడు, 0, 0, context);
	ఒకవేళ(kevent(db.kqueuefd, &ev, 1, ఖాళీ, 0, ఖాళీ) == -1){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error in kqueue accepting: %s", strerror(errno));
	}

	context->events = 0;
	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం mux_kqueue__delete(నిర్మాణం mosquitto *context)
{
	నిర్మాణం kevent ev[2];

	ఒకవేళ(context->sock != INVALID_SOCKET){
		EV_సమితి(&ev[0], context->sock, EVFILT_READ, EV_DELETE, 0, 0, context);
		EV_సమితి(&ev[1], context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		ఒకవేళ(kevent(db.kqueuefd, ev, 2, ఖాళీ, 0, ఖాళీ) == -1){
			తిరిగి 1;
		}
	}
	తిరిగి 0;
}


పూర్ణాంకం mux_kqueue__handle(శూన్య)
{
	నిర్మాణం mosquitto *context;
	నిర్మాణం mosquitto__listener_sock *listensock;
	పూర్ణాంకం event_లెక్క;
	నిర్మాణం timespec timeout;

#నిర్వచితమైతే WITH_WEBSOCKETS
	timeout.tv_sec = 0;
	timeout.tv_nsec = 100000000; /* 100 ms */
#లేకపోతే
	timeout.tv_sec = db.తదుపరి_event_ms/1000;
	timeout.tv_nsec = (db.తదుపరి_event_ms - timeout.tv_sec*1000) * 1000000;
#అయితే_ముగింపు

	event_లెక్క = kevent(db.kqueuefd,
			ఖాళీ, 0,
			event_list, MAX_EVENTS,
			&timeout);

	db.now_s = mosquitto_సమయం();
	db.now_real_s = సమయం(ఖాళీ);

	ఎంపిక(event_లెక్క){
		స్థితి -1:
			ఒకవేళ(errno != EINTR){
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error in kqueue waiting: %s.", strerror(errno));
			}
			ఆపు;
		స్థితి 0:
			ఆపు;
		అప్రమేయ:
			కొరకు(పూర్ణాంకం i=0; i<event_లెక్క; i++){
				context = event_list[i].udata;
				ఒకవేళ(context->ident == id_client){
					loop_handle_reads_writes(context, event_list[i].filter);
					ఒకవేళ(event_list[i].flags & (EV_EOF | EV_ERROR)){
						do_disconnect(context, MOSQ_ERR_CONN_కోల్పోయిన);
					}
				}లేకపోతే ఒకవేళ(context->ident == id_listener){
					listensock = event_list[i].udata;

					ఒకవేళ(event_list[i].filter == EVFILT_READ){
						అయితే((context = net__socket_accept(listensock)) != ఖాళీ){
						}
					}
#నిర్వచితమైతే WITH_WEBSOCKETS
				}లేకపోతే ఒకవేళ(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#అయితే_ముగింపు
				}
			}
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం mux_kqueue__cleanup(శూన్య)
{
	(శూన్య)close(db.kqueuefd);
	db.kqueuefd = 0;
	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర శూన్య loop_handle_reads_writes(నిర్మాణం mosquitto *context, పొట్టి event)
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
		int16_t lws_events;
		ఎంపిక(event){
			స్థితి EVFILT_READ:
				lws_events = LWS_POLLIN;
				ఆపు;
			స్థితి EVFILT_WRITE:
				lws_events = LWS_POLLOUT;
				ఆపు;
			అప్రమేయ:
				lws_events = LWS_POLLHUP;
				ఆపు;
		}
		wspoll.events = lws_events;
		wspoll.revents = lws_events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		తిరిగి;
	}
#అయితే_ముగింపు

	ఒకవేళ(event == EVFILT_WRITE
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

	ఒకవేళ(event == EVFILT_READ
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
	}
}
#అయితే_ముగింపు
