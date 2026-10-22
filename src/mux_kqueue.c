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

#অন্তর্ভাব "config.h"

#ifdef WITH_KQUEUE

#define MAX_EVENTS 1000

#অন্তর্ভাব <errno.h>
#অন্তর্ভাব <signal.h>
#অন্তর্ভাব <sys/event.h>
#অন্তর্ভাব <sys/socket.h>

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "packet_mosq.h"
#অন্তর্ভাব "util_mosq.h"

স্থির শূন্য loop_handle_reads_writes(সংরচনা mosquitto *context, short events);

স্থির সংরচনা kevent event_list[MAX_EVENTS];


পূর্ণাঙ্ক mux_kqueue__init(শূন্য)
{
	memset(&event_list, 0, পরিমাণ(সংরচনা kevent)*MAX_EVENTS);

	db.kqueuefd = 0;
	যদি((db.kqueuefd = kqueue()) == -1){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error in kqueue creating: %s", strerror(errno));
		প্রত্যাগম MOSQ_ERR_UNKNOWN;
	}

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক mux_kqueue__add_listeners(সংরচনা mosquitto__listener_sock *listensock, পূর্ণাঙ্ক listensock_গুনতি)
{
	সংরচনা kevent ev;

	memset(&ev, 0, পরিমাণ(সংরচনা kevent));

	প্রতি(পূর্ণাঙ্ক i=0; i<listensock_গুনতি; i++){
		EV_SET(&ev, listensock[i].sock, EVFILT_READ, EV_ADD, 0, 0, &listensock[i]);
		যদি(kevent(db.kqueuefd, &ev, 1, রিক্ত, 0, রিক্ত) == -1){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error in kqueue initial registering: %s", strerror(errno));
			প্রত্যাগম MOSQ_ERR_UNKNOWN;
		}
	}

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক mux_kqueue__delete_listeners(সংরচনা mosquitto__listener_sock *listensock, পূর্ণাঙ্ক listensock_গুনতি)
{
	সংরচনা kevent ev;

	প্রতি(পূর্ণাঙ্ক i=0; i<listensock_গুনতি; i++){
		EV_SET(&ev, listensock[i].sock, EVFILT_READ, EV_DELETE, 0, 0, &listensock[i]);
		যদি(kevent(db.kqueuefd, &ev, 1, রিক্ত, 0, রিক্ত) == -1){
			প্রত্যাগম MOSQ_ERR_UNKNOWN;
		}
	}

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক mux_kqueue__loop_setup(শূন্য)
{
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক mux_kqueue__add_out(সংরচনা mosquitto *context)
{
	সংরচনা kevent ev;

	যদি(context->events != EVFILT_WRITE){
		EV_SET(&ev, context->sock, EVFILT_WRITE, EV_ADD, 0, 0, context);
		যদি(kevent(db.kqueuefd, &ev, 1, রিক্ত, 0, রিক্ত) == -1){
			log__printf(রিক্ত, MOSQ_LOG_DEBUG, "Error in kqueue re-registering to EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = EVFILT_WRITE;
	}
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক mux_kqueue__remove_out(সংরচনা mosquitto *context)
{
	সংরচনা kevent ev;

	যদি(context->events == EVFILT_WRITE){
		EV_SET(&ev, context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		যদি(kevent(db.kqueuefd, &ev, 1, রিক্ত, 0, রিক্ত) == -1){
			log__printf(রিক্ত, MOSQ_LOG_DEBUG, "Error in kqueue removing EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = 0;
	}
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক mux_kqueue__new(সংরচনা mosquitto *context)
{
	সংরচনা kevent ev;

	EV_SET(&ev, context->sock, EVFILT_READ, EV_ADD, 0, 0, context);
	যদি(kevent(db.kqueuefd, &ev, 1, রিক্ত, 0, রিক্ত) == -1){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error in kqueue accepting: %s", strerror(errno));
	}

	context->events = 0;
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক mux_kqueue__delete(সংরচনা mosquitto *context)
{
	সংরচনা kevent ev[2];

	যদি(context->sock != INVALID_SOCKET){
		EV_SET(&ev[0], context->sock, EVFILT_READ, EV_DELETE, 0, 0, context);
		EV_SET(&ev[1], context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		যদি(kevent(db.kqueuefd, ev, 2, রিক্ত, 0, রিক্ত) == -1){
			প্রত্যাগম 1;
		}
	}
	প্রত্যাগম 0;
}


পূর্ণাঙ্ক mux_kqueue__handle(শূন্য)
{
	সংরচনা mosquitto *context;
	সংরচনা mosquitto__listener_sock *listensock;
	পূর্ণাঙ্ক event_গুনতি;
	সংরচনা timespec timeout;

#ifdef WITH_WEBSOCKETS
	timeout.tv_sec = 0;
	timeout.tv_nsec = 100000000; /* 100 ms */
#অন্যথা
	timeout.tv_sec = db.next_event_ms/1000;
	timeout.tv_nsec = (db.next_event_ms - timeout.tv_sec*1000) * 1000000;
#endif

	event_গুনতি = kevent(db.kqueuefd,
			রিক্ত, 0,
			event_list, MAX_EVENTS,
			&timeout);

	db.now_s = mosquitto_time();
	db.now_real_s = time(রিক্ত);

	বিকল্প(event_গুনতি){
		পক্ষ -1:
			যদি(errno != EINTR){
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error in kqueue waiting: %s.", strerror(errno));
			}
			ভঙ্গ;
		পক্ষ 0:
			ভঙ্গ;
		স্বভাব:
			প্রতি(পূর্ণাঙ্ক i=0; i<event_গুনতি; i++){
				context = event_list[i].udata;
				যদি(context->ident == id_client){
					loop_handle_reads_writes(context, event_list[i].filter);
					যদি(event_list[i].flags & (EV_EOF | EV_ERROR)){
						do_disconnect(context, MOSQ_ERR_CONN_LOST);
					}
				}অন্যথা যদি(context->ident == id_listener){
					listensock = event_list[i].udata;

					যদি(event_list[i].filter == EVFILT_READ){
						যাবত্((context = net__socket_accept(listensock)) != রিক্ত){
						}
					}
#ifdef WITH_WEBSOCKETS
				}অন্যথা যদি(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#endif
				}
			}
	}
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক mux_kqueue__cleanup(শূন্য)
{
	(শূন্য)close(db.kqueuefd);
	db.kqueuefd = 0;
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


স্থির শূন্য loop_handle_reads_writes(সংরচনা mosquitto *context, short event)
{
	পূর্ণাঙ্ক err;
	socklen_t len;
	পূর্ণাঙ্ক rc;

	যদি(context->sock == INVALID_SOCKET){
		প্রত্যাগম;
	}

#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	যদি(context->wsi){
		সংরচনা lws_pollfd wspoll;
		wspoll.fd = context->sock;
		int16_t lws_events;
		বিকল্প(event){
			পক্ষ EVFILT_READ:
				lws_events = LWS_POLLIN;
				ভঙ্গ;
			পক্ষ EVFILT_WRITE:
				lws_events = LWS_POLLOUT;
				ভঙ্গ;
			স্বভাব:
				lws_events = LWS_POLLHUP;
				ভঙ্গ;
		}
		wspoll.events = lws_events;
		wspoll.revents = lws_events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		প্রত্যাগম;
	}
#endif

	যদি(event == EVFILT_WRITE
#ifdef WITH_TLS
			|| context->want_write
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		যদি(context->state == mosq_cs_connect_pending){
			len = পরিমাণ(পূর্ণাঙ্ক);
			যদি(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (বর্ণ *)&err, &len)){
				যদি(err == 0){
					mosquitto__set_state(context, mosq_cs_new);
#যদি defined(WITH_ADNS) && defined(WITH_BRIDGE)
					যদি(context->bridge){
						bridge__connect_step3(context);
					}
#endif
				}
			}অন্যথা{
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				প্রত্যাগম;
			}
		}
		rc = packet__write(context);
		যদি(rc){
			do_disconnect(context, rc);
			প্রত্যাগম;
		}
	}

	যদি(event == EVFILT_READ
#ifdef WITH_TLS
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		কুরু{
			বিকল্প(context->transport){
				পক্ষ mosq_t_tcp:
				পক্ষ mosq_t_ws:
					rc = packet__read(context);
					ভঙ্গ;
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
				পক্ষ mosq_t_http:
					rc = http__read(context);
					ভঙ্গ;
#endif
#যদি !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
				/* Not supported with LWS */
				পক্ষ mosq_t_proxy_v2:
					rc = proxy_v2__read(context);
					ভঙ্গ;
				পক্ষ mosq_t_proxy_v1:
					rc = proxy_v1__read(context);
					ভঙ্গ;
#endif
				স্বভাব:
					rc = MOSQ_ERR_INVAL;
					ভঙ্গ;
			}
			যদি(rc){
				do_disconnect(context, rc);
				প্রত্যাগম;
			}
		}যাবত্(SSL_DATA_PENDING(context));
	}
}
#endif
