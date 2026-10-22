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

#ضمن "config.h"

#جيڪڏهن_وصف_ٿيل WITH_KQUEUE

#وصف_ڪريو MAX_EVENTS 1000

#ضمن <errno.h>
#ضمن <signal.h>
#ضمن <sys/event.h>
#ضمن <sys/socket.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "packet_mosq.h"
#ضمن "util_mosq.h"

ساكن فراغ loop_handle_reads_writes(بنية mosquitto *context, ننڍو events);

ساكن بنية kevent event_list[MAX_EVENTS];


صحيح mux_kqueue__init(فراغ)
{
	memset(&event_list, 0, حجم(بنية kevent)*MAX_EVENTS);

	db.kqueuefd = 0;
	إذا((db.kqueuefd = kqueue()) == -1){
		log__printf(عدم, MOSQ_LOG_ERR, "Error in kqueue creating: %s", strerror(errno));
		أرجع MOSQ_ERR_UNKNOWN;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح mux_kqueue_جوڙيو_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_ڳڻپ)
{
	بنية kevent ev;

	memset(&ev, 0, حجم(بنية kevent));

	لكل(صحيح i=0; i<listensock_ڳڻپ; i++){
		EV_سيٽ(&ev, listensock[i].sock, EVFILT_READ, EV_جوڙيو, 0, 0, &listensock[i]);
		إذا(kevent(db.kqueuefd, &ev, 1, عدم, 0, عدم) == -1){
			log__printf(عدم, MOSQ_LOG_ERR, "Error in kqueue initial registering: %s", strerror(errno));
			أرجع MOSQ_ERR_UNKNOWN;
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح mux_kqueue__delete_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_ڳڻپ)
{
	بنية kevent ev;

	لكل(صحيح i=0; i<listensock_ڳڻپ; i++){
		EV_سيٽ(&ev, listensock[i].sock, EVFILT_READ, EV_DELETE, 0, 0, &listensock[i]);
		إذا(kevent(db.kqueuefd, &ev, 1, عدم, 0, عدم) == -1){
			أرجع MOSQ_ERR_UNKNOWN;
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح mux_kqueue_loop_تياري(فراغ)
{
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح mux_kqueue_جوڙيو_ٻاهريون(بنية mosquitto *context)
{
	بنية kevent ev;

	إذا(context->events != EVFILT_WRITE){
		EV_سيٽ(&ev, context->sock, EVFILT_WRITE, EV_جوڙيو, 0, 0, context);
		إذا(kevent(db.kqueuefd, &ev, 1, عدم, 0, عدم) == -1){
			log__printf(عدم, MOSQ_LOG_DEBUG, "Error in kqueue re-registering to EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = EVFILT_WRITE;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح mux_kqueue_remove_ٻاهريون(بنية mosquitto *context)
{
	بنية kevent ev;

	إذا(context->events == EVFILT_WRITE){
		EV_سيٽ(&ev, context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		إذا(kevent(db.kqueuefd, &ev, 1, عدم, 0, عدم) == -1){
			log__printf(عدم, MOSQ_LOG_DEBUG, "Error in kqueue removing EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = 0;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح mux_kqueue__new(بنية mosquitto *context)
{
	بنية kevent ev;

	EV_سيٽ(&ev, context->sock, EVFILT_READ, EV_جوڙيو, 0, 0, context);
	إذا(kevent(db.kqueuefd, &ev, 1, عدم, 0, عدم) == -1){
		log__printf(عدم, MOSQ_LOG_ERR, "Error in kqueue accepting: %s", strerror(errno));
	}

	context->events = 0;
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح mux_kqueue__delete(بنية mosquitto *context)
{
	بنية kevent ev[2];

	إذا(context->sock != INVALID_SOCKET){
		EV_سيٽ(&ev[0], context->sock, EVFILT_READ, EV_DELETE, 0, 0, context);
		EV_سيٽ(&ev[1], context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		إذا(kevent(db.kqueuefd, ev, 2, عدم, 0, عدم) == -1){
			أرجع 1;
		}
	}
	أرجع 0;
}


صحيح mux_kqueue__handle(فراغ)
{
	بنية mosquitto *context;
	بنية mosquitto__listener_sock *listensock;
	صحيح event_ڳڻپ;
	بنية timespec timeout;

#جيڪڏهن_وصف_ٿيل WITH_WEBSOCKETS
	timeout.tv_sec = 0;
	timeout.tv_nsec = 100000000; /* 100 ms */
#وإلا
	timeout.tv_sec = db.ايندڙ_event_ms/1000;
	timeout.tv_nsec = (db.ايندڙ_event_ms - timeout.tv_sec*1000) * 1000000;
#جيڪڏهن_پورو

	event_ڳڻپ = kevent(db.kqueuefd,
			عدم, 0,
			event_list, MAX_EVENTS,
			&timeout);

	db.now_s = mosquitto_وقت();
	db.now_real_s = وقت(عدم);

	بدل(event_ڳڻپ){
		حالة -1:
			إذا(errno != EINTR){
				log__printf(عدم, MOSQ_LOG_ERR, "Error in kqueue waiting: %s.", strerror(errno));
			}
			اكسر;
		حالة 0:
			اكسر;
		افتراضي:
			لكل(صحيح i=0; i<event_ڳڻپ; i++){
				context = event_list[i].udata;
				إذا(context->ident == id_client){
					loop_handle_reads_writes(context, event_list[i].filter);
					إذا(event_list[i].flags & (EV_EOF | EV_ERROR)){
						do_disconnect(context, MOSQ_ERR_CONN_گم);
					}
				}وإلا إذا(context->ident == id_listener){
					listensock = event_list[i].udata;

					إذا(event_list[i].filter == EVFILT_READ){
						طالما((context = net__socket_accept(listensock)) != عدم){
						}
					}
#جيڪڏهن_وصف_ٿيل WITH_WEBSOCKETS
				}وإلا إذا(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#جيڪڏهن_پورو
				}
			}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح mux_kqueue__cleanup(فراغ)
{
	(فراغ)close(db.kqueuefd);
	db.kqueuefd = 0;
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن فراغ loop_handle_reads_writes(بنية mosquitto *context, ننڍو event)
{
	صحيح err;
	socklen_t len;
	صحيح rc;

	إذا(context->sock == INVALID_SOCKET){
		أرجع;
	}

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	إذا(context->wsi){
		بنية lws_pollfd wspoll;
		wspoll.fd = context->sock;
		int16_t lws_events;
		بدل(event){
			حالة EVFILT_READ:
				lws_events = LWS_POLLIN;
				اكسر;
			حالة EVFILT_WRITE:
				lws_events = LWS_POLLOUT;
				اكسر;
			افتراضي:
				lws_events = LWS_POLLHUP;
				اكسر;
		}
		wspoll.events = lws_events;
		wspoll.revents = lws_events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		أرجع;
	}
#جيڪڏهن_پورو

	إذا(event == EVFILT_WRITE
#جيڪڏهن_وصف_ٿيل WITH_TLS
			|| context->want_write
			|| (context->ssl && context->state == mosq_cs_new)
#جيڪڏهن_پورو
			){

		إذا(context->state == mosq_cs_connect_pending){
			len = حجم(صحيح);
			إذا(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (حرف *)&err, &len)){
				إذا(err == 0){
					mosquitto_سيٽ_state(context, mosq_cs_new);
#إذا defined(WITH_ADNS) && defined(WITH_BRIDGE)
					إذا(context->bridge){
						bridge__connect_step3(context);
					}
#جيڪڏهن_پورو
				}
			}وإلا{
				do_disconnect(context, MOSQ_ERR_CONN_گم);
				أرجع;
			}
		}
		rc = packet__write(context);
		إذا(rc){
			do_disconnect(context, rc);
			أرجع;
		}
	}

	إذا(event == EVFILT_READ
#جيڪڏهن_وصف_ٿيل WITH_TLS
			|| (context->ssl && context->state == mosq_cs_new)
#جيڪڏهن_پورو
			){

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
				أرجع;
			}
		}طالما(SSL_معلومات_PENDING(context));
	}
}
#جيڪڏهن_پورو
