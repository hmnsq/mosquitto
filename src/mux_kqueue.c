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

#شامل "config.h"

#ifdef WITH_KQUEUE

#define MAX_EVENTS 1000

#شامل <errno.h>
#شامل <signal.h>
#شامل <sys/event.h>
#شامل <sys/socket.h>

#شامل "mosquitto_broker_internal.h"
#شامل "packet_mosq.h"
#شامل "util_mosq.h"

ایستا تهی loop_handle_reads_writes(ساختار mosquitto *context, short events);

ایستا ساختار kevent event_list[MAX_EVENTS];


صحیح mux_kqueue__init(تهی)
{
	memset(&event_list, 0, اندازه(ساختار kevent)*MAX_EVENTS);

	db.kqueuefd = 0;
	اگر((db.kqueuefd = kqueue()) == -1){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error in kqueue creating: %s", strerror(errno));
		بازگشت MOSQ_ERR_UNKNOWN;
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح mux_kqueue__add_listeners(ساختار mosquitto__listener_sock *listensock, صحیح listensock_شمار)
{
	ساختار kevent ev;

	memset(&ev, 0, اندازه(ساختار kevent));

	برای(صحیح i=0; i<listensock_شمار; i++){
		EV_SET(&ev, listensock[i].sock, EVFILT_READ, EV_ADD, 0, 0, &listensock[i]);
		اگر(kevent(db.kqueuefd, &ev, 1, پوچ, 0, پوچ) == -1){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error in kqueue initial registering: %s", strerror(errno));
			بازگشت MOSQ_ERR_UNKNOWN;
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح mux_kqueue__delete_listeners(ساختار mosquitto__listener_sock *listensock, صحیح listensock_شمار)
{
	ساختار kevent ev;

	برای(صحیح i=0; i<listensock_شمار; i++){
		EV_SET(&ev, listensock[i].sock, EVFILT_READ, EV_DELETE, 0, 0, &listensock[i]);
		اگر(kevent(db.kqueuefd, &ev, 1, پوچ, 0, پوچ) == -1){
			بازگشت MOSQ_ERR_UNKNOWN;
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح mux_kqueue__loop_setup(تهی)
{
	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح mux_kqueue__add_out(ساختار mosquitto *context)
{
	ساختار kevent ev;

	اگر(context->events != EVFILT_WRITE){
		EV_SET(&ev, context->sock, EVFILT_WRITE, EV_ADD, 0, 0, context);
		اگر(kevent(db.kqueuefd, &ev, 1, پوچ, 0, پوچ) == -1){
			log__printf(پوچ, MOSQ_LOG_DEBUG, "Error in kqueue re-registering to EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = EVFILT_WRITE;
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح mux_kqueue__remove_out(ساختار mosquitto *context)
{
	ساختار kevent ev;

	اگر(context->events == EVFILT_WRITE){
		EV_SET(&ev, context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		اگر(kevent(db.kqueuefd, &ev, 1, پوچ, 0, پوچ) == -1){
			log__printf(پوچ, MOSQ_LOG_DEBUG, "Error in kqueue removing EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = 0;
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح mux_kqueue__new(ساختار mosquitto *context)
{
	ساختار kevent ev;

	EV_SET(&ev, context->sock, EVFILT_READ, EV_ADD, 0, 0, context);
	اگر(kevent(db.kqueuefd, &ev, 1, پوچ, 0, پوچ) == -1){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error in kqueue accepting: %s", strerror(errno));
	}

	context->events = 0;
	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح mux_kqueue__delete(ساختار mosquitto *context)
{
	ساختار kevent ev[2];

	اگر(context->sock != INVALID_SOCKET){
		EV_SET(&ev[0], context->sock, EVFILT_READ, EV_DELETE, 0, 0, context);
		EV_SET(&ev[1], context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		اگر(kevent(db.kqueuefd, ev, 2, پوچ, 0, پوچ) == -1){
			بازگشت 1;
		}
	}
	بازگشت 0;
}


صحیح mux_kqueue__handle(تهی)
{
	ساختار mosquitto *context;
	ساختار mosquitto__listener_sock *listensock;
	صحیح event_شمار;
	ساختار timespec timeout;

#ifdef WITH_WEBSOCKETS
	timeout.tv_sec = 0;
	timeout.tv_nsec = 100000000; /* 100 ms */
#وگرنه
	timeout.tv_sec = db.next_event_ms/1000;
	timeout.tv_nsec = (db.next_event_ms - timeout.tv_sec*1000) * 1000000;
#endif

	event_شمار = kevent(db.kqueuefd,
			پوچ, 0,
			event_list, MAX_EVENTS,
			&timeout);

	db.now_s = mosquitto_time();
	db.now_real_s = time(پوچ);

	انتخاب(event_شمار){
		حالت -1:
			اگر(errno != EINTR){
				log__printf(پوچ, MOSQ_LOG_ERR, "Error in kqueue waiting: %s.", strerror(errno));
			}
			بشکن;
		حالت 0:
			بشکن;
		پیشفرض:
			برای(صحیح i=0; i<event_شمار; i++){
				context = event_list[i].udata;
				اگر(context->ident == id_client){
					loop_handle_reads_writes(context, event_list[i].filter);
					اگر(event_list[i].flags & (EV_EOF | EV_ERROR)){
						do_disconnect(context, MOSQ_ERR_CONN_LOST);
					}
				}وگرنه اگر(context->ident == id_listener){
					listensock = event_list[i].udata;

					اگر(event_list[i].filter == EVFILT_READ){
						وقتی((context = net__socket_accept(listensock)) != پوچ){
						}
					}
#ifdef WITH_WEBSOCKETS
				}وگرنه اگر(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#endif
				}
			}
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح mux_kqueue__cleanup(تهی)
{
	(تهی)close(db.kqueuefd);
	db.kqueuefd = 0;
	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا تهی loop_handle_reads_writes(ساختار mosquitto *context, short event)
{
	صحیح err;
	socklen_t len;
	صحیح rc;

	اگر(context->sock == INVALID_SOCKET){
		بازگشت;
	}

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	اگر(context->wsi){
		ساختار lws_pollfd wspoll;
		wspoll.fd = context->sock;
		int16_t lws_events;
		انتخاب(event){
			حالت EVFILT_READ:
				lws_events = LWS_POLLIN;
				بشکن;
			حالت EVFILT_WRITE:
				lws_events = LWS_POLLOUT;
				بشکن;
			پیشفرض:
				lws_events = LWS_POLLHUP;
				بشکن;
		}
		wspoll.events = lws_events;
		wspoll.revents = lws_events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		بازگشت;
	}
#endif

	اگر(event == EVFILT_WRITE
#ifdef WITH_TLS
			|| context->want_write
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		اگر(context->state == mosq_cs_connect_pending){
			len = اندازه(صحیح);
			اگر(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (نویسه *)&err, &len)){
				اگر(err == 0){
					mosquitto__set_state(context, mosq_cs_new);
#اگر defined(WITH_ADNS) && defined(WITH_BRIDGE)
					اگر(context->bridge){
						bridge__connect_step3(context);
					}
#endif
				}
			}وگرنه{
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				بازگشت;
			}
		}
		rc = packet__write(context);
		اگر(rc){
			do_disconnect(context, rc);
			بازگشت;
		}
	}

	اگر(event == EVFILT_READ
#ifdef WITH_TLS
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

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
				بازگشت;
			}
		}وقتی(SSL_DATA_PENDING(context));
	}
}
#endif
