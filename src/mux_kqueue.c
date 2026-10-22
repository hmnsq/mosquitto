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

#اگر_تعریف_شدہ WITH_KQUEUE

#تعریف_کرو MAX_EVENTS 1000

#شامل <errno.h>
#شامل <signal.h>
#شامل <sys/event.h>
#شامل <sys/socket.h>

#شامل "mosquitto_broker_internal.h"
#شامل "packet_mosq.h"
#شامل "util_mosq.h"

ساکن خالی loop_handle_reads_writes(ساخت mosquitto *context, مختصر events);

ساکن ساخت kevent event_list[MAX_EVENTS];


صحیح mux_kqueue__init(خالی)
{
	memset(&event_list, 0, حجم(ساخت kevent)*MAX_EVENTS);

	db.kqueuefd = 0;
	اگر((db.kqueuefd = kqueue()) == -1){
		log__printf(تہی, MOSQ_LOG_ERR, "Error in kqueue creating: %s", strerror(errno));
		واپس MOSQ_ERR_UNKNOWN;
	}

	واپس MOSQ_ERR_SUCCESS;
}


صحیح mux_kqueue_جوڑو_listeners(ساخت mosquitto__listener_sock *listensock, صحیح listensock_گنتی)
{
	ساخت kevent ev;

	memset(&ev, 0, حجم(ساخت kevent));

	برائے(صحیح i=0; i<listensock_گنتی; i++){
		EV_سیٹ(&ev, listensock[i].sock, EVFILT_READ, EV_جوڑو, 0, 0, &listensock[i]);
		اگر(kevent(db.kqueuefd, &ev, 1, تہی, 0, تہی) == -1){
			log__printf(تہی, MOSQ_LOG_ERR, "Error in kqueue initial registering: %s", strerror(errno));
			واپس MOSQ_ERR_UNKNOWN;
		}
	}

	واپس MOSQ_ERR_SUCCESS;
}


صحیح mux_kqueue__delete_listeners(ساخت mosquitto__listener_sock *listensock, صحیح listensock_گنتی)
{
	ساخت kevent ev;

	برائے(صحیح i=0; i<listensock_گنتی; i++){
		EV_سیٹ(&ev, listensock[i].sock, EVFILT_READ, EV_DELETE, 0, 0, &listensock[i]);
		اگر(kevent(db.kqueuefd, &ev, 1, تہی, 0, تہی) == -1){
			واپس MOSQ_ERR_UNKNOWN;
		}
	}

	واپس MOSQ_ERR_SUCCESS;
}


صحیح mux_kqueue_loop_تیاری(خالی)
{
	واپس MOSQ_ERR_SUCCESS;
}


صحیح mux_kqueue_جوڑو_برآمد(ساخت mosquitto *context)
{
	ساخت kevent ev;

	اگر(context->events != EVFILT_WRITE){
		EV_سیٹ(&ev, context->sock, EVFILT_WRITE, EV_جوڑو, 0, 0, context);
		اگر(kevent(db.kqueuefd, &ev, 1, تہی, 0, تہی) == -1){
			log__printf(تہی, MOSQ_LOG_DEBUG, "Error in kqueue re-registering to EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = EVFILT_WRITE;
	}
	واپس MOSQ_ERR_SUCCESS;
}


صحیح mux_kqueue_remove_برآمد(ساخت mosquitto *context)
{
	ساخت kevent ev;

	اگر(context->events == EVFILT_WRITE){
		EV_سیٹ(&ev, context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		اگر(kevent(db.kqueuefd, &ev, 1, تہی, 0, تہی) == -1){
			log__printf(تہی, MOSQ_LOG_DEBUG, "Error in kqueue removing EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = 0;
	}
	واپس MOSQ_ERR_SUCCESS;
}


صحیح mux_kqueue__new(ساخت mosquitto *context)
{
	ساخت kevent ev;

	EV_سیٹ(&ev, context->sock, EVFILT_READ, EV_جوڑو, 0, 0, context);
	اگر(kevent(db.kqueuefd, &ev, 1, تہی, 0, تہی) == -1){
		log__printf(تہی, MOSQ_LOG_ERR, "Error in kqueue accepting: %s", strerror(errno));
	}

	context->events = 0;
	واپس MOSQ_ERR_SUCCESS;
}


صحیح mux_kqueue__delete(ساخت mosquitto *context)
{
	ساخت kevent ev[2];

	اگر(context->sock != INVALID_SOCKET){
		EV_سیٹ(&ev[0], context->sock, EVFILT_READ, EV_DELETE, 0, 0, context);
		EV_سیٹ(&ev[1], context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		اگر(kevent(db.kqueuefd, ev, 2, تہی, 0, تہی) == -1){
			واپس 1;
		}
	}
	واپس 0;
}


صحیح mux_kqueue__handle(خالی)
{
	ساخت mosquitto *context;
	ساخت mosquitto__listener_sock *listensock;
	صحیح event_گنتی;
	ساخت timespec timeout;

#اگر_تعریف_شدہ WITH_WEBSOCKETS
	timeout.tv_sec = 0;
	timeout.tv_nsec = 100000000; /* 100 ms */
#ورنہ
	timeout.tv_sec = db.اگلا_event_ms/1000;
	timeout.tv_nsec = (db.اگلا_event_ms - timeout.tv_sec*1000) * 1000000;
#اگر_ختم

	event_گنتی = kevent(db.kqueuefd,
			تہی, 0,
			event_list, MAX_EVENTS,
			&timeout);

	db.now_s = mosquitto_وقت();
	db.now_real_s = وقت(تہی);

	انتخاب(event_گنتی){
		حالت -1:
			اگر(errno != EINTR){
				log__printf(تہی, MOSQ_LOG_ERR, "Error in kqueue waiting: %s.", strerror(errno));
			}
			توڑو;
		حالت 0:
			توڑو;
		طےشدہ:
			برائے(صحیح i=0; i<event_گنتی; i++){
				context = event_list[i].udata;
				اگر(context->ident == id_client){
					loop_handle_reads_writes(context, event_list[i].filter);
					اگر(event_list[i].flags & (EV_EOF | EV_ERROR)){
						do_disconnect(context, MOSQ_ERR_CONN_گمشدہ);
					}
				}ورنہ اگر(context->ident == id_listener){
					listensock = event_list[i].udata;

					اگر(event_list[i].filter == EVFILT_READ){
						جب((context = net__socket_accept(listensock)) != تہی){
						}
					}
#اگر_تعریف_شدہ WITH_WEBSOCKETS
				}ورنہ اگر(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#اگر_ختم
				}
			}
	}
	واپس MOSQ_ERR_SUCCESS;
}


صحیح mux_kqueue__cleanup(خالی)
{
	(خالی)close(db.kqueuefd);
	db.kqueuefd = 0;
	واپس MOSQ_ERR_SUCCESS;
}


ساکن خالی loop_handle_reads_writes(ساخت mosquitto *context, مختصر event)
{
	صحیح err;
	socklen_t len;
	صحیح rc;

	اگر(context->sock == INVALID_SOCKET){
		واپس;
	}

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	اگر(context->wsi){
		ساخت lws_pollfd wspoll;
		wspoll.fd = context->sock;
		int16_t lws_events;
		انتخاب(event){
			حالت EVFILT_READ:
				lws_events = LWS_POLLIN;
				توڑو;
			حالت EVFILT_WRITE:
				lws_events = LWS_POLLOUT;
				توڑو;
			طےشدہ:
				lws_events = LWS_POLLHUP;
				توڑو;
		}
		wspoll.events = lws_events;
		wspoll.revents = lws_events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		واپس;
	}
#اگر_ختم

	اگر(event == EVFILT_WRITE
#اگر_تعریف_شدہ WITH_TLS
			|| context->want_write
			|| (context->ssl && context->state == mosq_cs_new)
#اگر_ختم
			){

		اگر(context->state == mosq_cs_connect_pending){
			len = حجم(صحیح);
			اگر(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (حرف *)&err, &len)){
				اگر(err == 0){
					mosquitto_سیٹ_state(context, mosq_cs_new);
#اگر defined(WITH_ADNS) && defined(WITH_BRIDGE)
					اگر(context->bridge){
						bridge__connect_step3(context);
					}
#اگر_ختم
				}
			}ورنہ{
				do_disconnect(context, MOSQ_ERR_CONN_گمشدہ);
				واپس;
			}
		}
		rc = packet__write(context);
		اگر(rc){
			do_disconnect(context, rc);
			واپس;
		}
	}

	اگر(event == EVFILT_READ
#اگر_تعریف_شدہ WITH_TLS
			|| (context->ssl && context->state == mosq_cs_new)
#اگر_ختم
			){

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
				واپس;
			}
		}جب(SSL_ڈیٹا_PENDING(context));
	}
}
#اگر_ختم
