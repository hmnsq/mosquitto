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

#include "config.h"

#ifdef WITH_KQUEUE

#define MAX_EVENTS 1000

#include <errno.h>
#include <signal.h>
#include <sys/event.h>
#include <sys/socket.h>

#include "mosquitto_broker_internal.h"
#include "packet_mosq.h"
#include "util_mosq.h"

static void loop_handle_reads_writes(struct mosquitto *context, short events);

static struct kevent event_list[MAX_EVENTS];


цео mux_kqueue__init(void)
{
	memset(&event_list, 0, sizeof(struct kevent)*MAX_EVENTS);

	db.kqueuefd = 0;
	ако((db.kqueuefd = kqueue()) == -1){
		log__printf(NULL, MOSQ_LOG_ERR, "Error in kqueue creating: %s", strerror(errno));
		врати MOSQ_ERR_UNKNOWN;
	}

	врати MOSQ_ERR_SUCCESS;
}


цео mux_kqueue__add_listeners(struct mosquitto__listener_sock *listensock, цео listensock_count)
{
	struct kevent ev;

	memset(&ev, 0, sizeof(struct kevent));

	за(цео i=0; i<listensock_count; i++){
		EV_SET(&ev, listensock[i].sock, EVFILT_READ, EV_ADD, 0, 0, &listensock[i]);
		ако(kevent(db.kqueuefd, &ev, 1, NULL, 0, NULL) == -1){
			log__printf(NULL, MOSQ_LOG_ERR, "Error in kqueue initial registering: %s", strerror(errno));
			врати MOSQ_ERR_UNKNOWN;
		}
	}

	врати MOSQ_ERR_SUCCESS;
}


цео mux_kqueue__delete_listeners(struct mosquitto__listener_sock *listensock, цео listensock_count)
{
	struct kevent ev;

	за(цео i=0; i<listensock_count; i++){
		EV_SET(&ev, listensock[i].sock, EVFILT_READ, EV_DELETE, 0, 0, &listensock[i]);
		ако(kevent(db.kqueuefd, &ev, 1, NULL, 0, NULL) == -1){
			врати MOSQ_ERR_UNKNOWN;
		}
	}

	врати MOSQ_ERR_SUCCESS;
}


цео mux_kqueue__loop_setup(void)
{
	врати MOSQ_ERR_SUCCESS;
}


цео mux_kqueue__add_out(struct mosquitto *context)
{
	struct kevent ev;

	ако(context->events != EVFILT_WRITE){
		EV_SET(&ev, context->sock, EVFILT_WRITE, EV_ADD, 0, 0, context);
		ако(kevent(db.kqueuefd, &ev, 1, NULL, 0, NULL) == -1){
			log__printf(NULL, MOSQ_LOG_DEBUG, "Error in kqueue re-registering to EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = EVFILT_WRITE;
	}
	врати MOSQ_ERR_SUCCESS;
}


цео mux_kqueue__remove_out(struct mosquitto *context)
{
	struct kevent ev;

	ако(context->events == EVFILT_WRITE){
		EV_SET(&ev, context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		ако(kevent(db.kqueuefd, &ev, 1, NULL, 0, NULL) == -1){
			log__printf(NULL, MOSQ_LOG_DEBUG, "Error in kqueue removing EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = 0;
	}
	врати MOSQ_ERR_SUCCESS;
}


цео mux_kqueue__new(struct mosquitto *context)
{
	struct kevent ev;

	EV_SET(&ev, context->sock, EVFILT_READ, EV_ADD, 0, 0, context);
	ако(kevent(db.kqueuefd, &ev, 1, NULL, 0, NULL) == -1){
		log__printf(NULL, MOSQ_LOG_ERR, "Error in kqueue accepting: %s", strerror(errno));
	}

	context->events = 0;
	врати MOSQ_ERR_SUCCESS;
}


цео mux_kqueue__delete(struct mosquitto *context)
{
	struct kevent ev[2];

	ако(context->sock != INVALID_SOCKET){
		EV_SET(&ev[0], context->sock, EVFILT_READ, EV_DELETE, 0, 0, context);
		EV_SET(&ev[1], context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		ако(kevent(db.kqueuefd, ev, 2, NULL, 0, NULL) == -1){
			врати 1;
		}
	}
	врати 0;
}


цео mux_kqueue__handle(void)
{
	struct mosquitto *context;
	struct mosquitto__listener_sock *listensock;
	цео event_count;
	struct timespec timeout;

#ifdef WITH_WEBSOCKETS
	timeout.tv_sec = 0;
	timeout.tv_nsec = 100000000; /* 100 ms */
#инако
	timeout.tv_sec = db.next_event_ms/1000;
	timeout.tv_nsec = (db.next_event_ms - timeout.tv_sec*1000) * 1000000;
#endif

	event_count = kevent(db.kqueuefd,
			NULL, 0,
			event_list, MAX_EVENTS,
			&timeout);

	db.now_s = mosquitto_time();
	db.now_real_s = time(NULL);

	switch(event_count){
		case -1:
			ако(errno != EINTR){
				log__printf(NULL, MOSQ_LOG_ERR, "Error in kqueue waiting: %s.", strerror(errno));
			}
			break;
		case 0:
			break;
		default:
			за(цео i=0; i<event_count; i++){
				context = event_list[i].udata;
				ако(context->ident == id_client){
					loop_handle_reads_writes(context, event_list[i].filter);
					ако(event_list[i].flags & (EV_EOF | EV_ERROR)){
						do_disconnect(context, MOSQ_ERR_CONN_LOST);
					}
				}инако ако(context->ident == id_listener){
					listensock = event_list[i].udata;

					ако(event_list[i].filter == EVFILT_READ){
						док((context = net__socket_accept(listensock)) != NULL){
						}
					}
#ifdef WITH_WEBSOCKETS
				}инако ако(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#endif
				}
			}
	}
	врати MOSQ_ERR_SUCCESS;
}


цео mux_kqueue__cleanup(void)
{
	(void)close(db.kqueuefd);
	db.kqueuefd = 0;
	врати MOSQ_ERR_SUCCESS;
}


static void loop_handle_reads_writes(struct mosquitto *context, short event)
{
	цео err;
	socklen_t len;
	цео rc;

	ако(context->sock == INVALID_SOCKET){
		врати;
	}

#ако defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ако(context->wsi){
		struct lws_pollfd wspoll;
		wspoll.fd = context->sock;
		int16_t lws_events;
		switch(event){
			case EVFILT_READ:
				lws_events = LWS_POLLIN;
				break;
			case EVFILT_WRITE:
				lws_events = LWS_POLLOUT;
				break;
			default:
				lws_events = LWS_POLLHUP;
				break;
		}
		wspoll.events = lws_events;
		wspoll.revents = lws_events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		врати;
	}
#endif

	ако(event == EVFILT_WRITE
#ifdef WITH_TLS
			|| context->want_write
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		ако(context->state == mosq_cs_connect_pending){
			len = sizeof(цео);
			ако(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (char *)&err, &len)){
				ако(err == 0){
					mosquitto__set_state(context, mosq_cs_new);
#ако defined(WITH_ADNS) && defined(WITH_BRIDGE)
					ако(context->bridge){
						bridge__connect_step3(context);
					}
#endif
				}
			}инако{
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				врати;
			}
		}
		rc = packet__write(context);
		ако(rc){
			do_disconnect(context, rc);
			врати;
		}
	}

	ако(event == EVFILT_READ
#ifdef WITH_TLS
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		do{
			switch(context->transport){
				case mosq_t_tcp:
				case mosq_t_ws:
					rc = packet__read(context);
					break;
#ако defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
				case mosq_t_http:
					rc = http__read(context);
					break;
#endif
#ако !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
				/* Not supported with LWS */
				case mosq_t_proxy_v2:
					rc = proxy_v2__read(context);
					break;
				case mosq_t_proxy_v1:
					rc = proxy_v1__read(context);
					break;
#endif
				default:
					rc = MOSQ_ERR_INVAL;
					break;
			}
			ако(rc){
				do_disconnect(context, rc);
				врати;
			}
		}док(SSL_DATA_PENDING(context));
	}
}
#endif
