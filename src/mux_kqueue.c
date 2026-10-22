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


정수 mux_kqueue__init(void)
{
	memset(&event_list, 0, sizeof(struct kevent)*MAX_EVENTS);

	db.kqueuefd = 0;
	만약((db.kqueuefd = kqueue()) == -1){
		log__printf(NULL, MOSQ_LOG_ERR, "Error in kqueue creating: %s", strerror(errno));
		반환 MOSQ_ERR_UNKNOWN;
	}

	반환 MOSQ_ERR_SUCCESS;
}


정수 mux_kqueue__add_listeners(struct mosquitto__listener_sock *listensock, 정수 listensock_count)
{
	struct kevent ev;

	memset(&ev, 0, sizeof(struct kevent));

	위해(정수 i=0; i<listensock_count; i++){
		EV_SET(&ev, listensock[i].sock, EVFILT_READ, EV_ADD, 0, 0, &listensock[i]);
		만약(kevent(db.kqueuefd, &ev, 1, NULL, 0, NULL) == -1){
			log__printf(NULL, MOSQ_LOG_ERR, "Error in kqueue initial registering: %s", strerror(errno));
			반환 MOSQ_ERR_UNKNOWN;
		}
	}

	반환 MOSQ_ERR_SUCCESS;
}


정수 mux_kqueue__delete_listeners(struct mosquitto__listener_sock *listensock, 정수 listensock_count)
{
	struct kevent ev;

	위해(정수 i=0; i<listensock_count; i++){
		EV_SET(&ev, listensock[i].sock, EVFILT_READ, EV_DELETE, 0, 0, &listensock[i]);
		만약(kevent(db.kqueuefd, &ev, 1, NULL, 0, NULL) == -1){
			반환 MOSQ_ERR_UNKNOWN;
		}
	}

	반환 MOSQ_ERR_SUCCESS;
}


정수 mux_kqueue__loop_setup(void)
{
	반환 MOSQ_ERR_SUCCESS;
}


정수 mux_kqueue__add_out(struct mosquitto *context)
{
	struct kevent ev;

	만약(context->events != EVFILT_WRITE){
		EV_SET(&ev, context->sock, EVFILT_WRITE, EV_ADD, 0, 0, context);
		만약(kevent(db.kqueuefd, &ev, 1, NULL, 0, NULL) == -1){
			log__printf(NULL, MOSQ_LOG_DEBUG, "Error in kqueue re-registering to EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = EVFILT_WRITE;
	}
	반환 MOSQ_ERR_SUCCESS;
}


정수 mux_kqueue__remove_out(struct mosquitto *context)
{
	struct kevent ev;

	만약(context->events == EVFILT_WRITE){
		EV_SET(&ev, context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		만약(kevent(db.kqueuefd, &ev, 1, NULL, 0, NULL) == -1){
			log__printf(NULL, MOSQ_LOG_DEBUG, "Error in kqueue removing EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = 0;
	}
	반환 MOSQ_ERR_SUCCESS;
}


정수 mux_kqueue__new(struct mosquitto *context)
{
	struct kevent ev;

	EV_SET(&ev, context->sock, EVFILT_READ, EV_ADD, 0, 0, context);
	만약(kevent(db.kqueuefd, &ev, 1, NULL, 0, NULL) == -1){
		log__printf(NULL, MOSQ_LOG_ERR, "Error in kqueue accepting: %s", strerror(errno));
	}

	context->events = 0;
	반환 MOSQ_ERR_SUCCESS;
}


정수 mux_kqueue__delete(struct mosquitto *context)
{
	struct kevent ev[2];

	만약(context->sock != INVALID_SOCKET){
		EV_SET(&ev[0], context->sock, EVFILT_READ, EV_DELETE, 0, 0, context);
		EV_SET(&ev[1], context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		만약(kevent(db.kqueuefd, ev, 2, NULL, 0, NULL) == -1){
			반환 1;
		}
	}
	반환 0;
}


정수 mux_kqueue__handle(void)
{
	struct mosquitto *context;
	struct mosquitto__listener_sock *listensock;
	정수 event_count;
	struct timespec timeout;

#ifdef WITH_WEBSOCKETS
	timeout.tv_sec = 0;
	timeout.tv_nsec = 100000000; /* 100 ms */
#아니면
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
			만약(errno != EINTR){
				log__printf(NULL, MOSQ_LOG_ERR, "Error in kqueue waiting: %s.", strerror(errno));
			}
			break;
		case 0:
			break;
		default:
			위해(정수 i=0; i<event_count; i++){
				context = event_list[i].udata;
				만약(context->ident == id_client){
					loop_handle_reads_writes(context, event_list[i].filter);
					만약(event_list[i].flags & (EV_EOF | EV_ERROR)){
						do_disconnect(context, MOSQ_ERR_CONN_LOST);
					}
				}아니면 만약(context->ident == id_listener){
					listensock = event_list[i].udata;

					만약(event_list[i].filter == EVFILT_READ){
						동안((context = net__socket_accept(listensock)) != NULL){
						}
					}
#ifdef WITH_WEBSOCKETS
				}아니면 만약(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#endif
				}
			}
	}
	반환 MOSQ_ERR_SUCCESS;
}


정수 mux_kqueue__cleanup(void)
{
	(void)close(db.kqueuefd);
	db.kqueuefd = 0;
	반환 MOSQ_ERR_SUCCESS;
}


static void loop_handle_reads_writes(struct mosquitto *context, short event)
{
	정수 err;
	socklen_t len;
	정수 rc;

	만약(context->sock == INVALID_SOCKET){
		반환;
	}

#만약 defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	만약(context->wsi){
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
		반환;
	}
#endif

	만약(event == EVFILT_WRITE
#ifdef WITH_TLS
			|| context->want_write
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		만약(context->state == mosq_cs_connect_pending){
			len = sizeof(정수);
			만약(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (char *)&err, &len)){
				만약(err == 0){
					mosquitto__set_state(context, mosq_cs_new);
#만약 defined(WITH_ADNS) && defined(WITH_BRIDGE)
					만약(context->bridge){
						bridge__connect_step3(context);
					}
#endif
				}
			}아니면{
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				반환;
			}
		}
		rc = packet__write(context);
		만약(rc){
			do_disconnect(context, rc);
			반환;
		}
	}

	만약(event == EVFILT_READ
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
#만약 defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
				case mosq_t_http:
					rc = http__read(context);
					break;
#endif
#만약 !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
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
			만약(rc){
				do_disconnect(context, rc);
				반환;
			}
		}동안(SSL_DATA_PENDING(context));
	}
}
#endif
