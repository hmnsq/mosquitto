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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ifdef WITH_KQUEUE

#define MAX_EVENTS 1000

#ଅନ୍ତର୍ଭାଵ <errno.h>
#ଅନ୍ତର୍ଭାଵ <signal.h>
#ଅନ୍ତର୍ଭାଵ <sys/event.h>
#ଅନ୍ତର୍ଭାଵ <sys/socket.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "packet_mosq.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"

ସ୍ଥିର ଶୂନ୍ୟ loop_handle_reads_writes(ସଂରଚନା mosquitto *context, short events);

ସ୍ଥିର ସଂରଚନା kevent event_list[MAX_EVENTS];


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_kqueue__init(ଶୂନ୍ୟ)
{
	memset(&event_list, 0, ଆକାର(ସଂରଚନା kevent)*MAX_EVENTS);

	db.kqueuefd = 0;
	ଯଦି((db.kqueuefd = kqueue()) == -1){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error in kqueue creating: %s", strerror(errno));
		ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_kqueue__add_listeners(ସଂରଚନା mosquitto__listener_sock *listensock, ପୂର୍ଣ୍ଣସଂଖ୍ୟା listensock_ଗଣନା)
{
	ସଂରଚନା kevent ev;

	memset(&ev, 0, ଆକାର(ସଂରଚନା kevent));

	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<listensock_ଗଣନା; i++){
		EV_SET(&ev, listensock[i].sock, EVFILT_READ, EV_ADD, 0, 0, &listensock[i]);
		ଯଦି(kevent(db.kqueuefd, &ev, 1, ଖାଲି, 0, ଖାଲି) == -1){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error in kqueue initial registering: %s", strerror(errno));
			ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_kqueue__delete_listeners(ସଂରଚନା mosquitto__listener_sock *listensock, ପୂର୍ଣ୍ଣସଂଖ୍ୟା listensock_ଗଣନା)
{
	ସଂରଚନା kevent ev;

	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<listensock_ଗଣନା; i++){
		EV_SET(&ev, listensock[i].sock, EVFILT_READ, EV_DELETE, 0, 0, &listensock[i]);
		ଯଦି(kevent(db.kqueuefd, &ev, 1, ଖାଲି, 0, ଖାଲି) == -1){
			ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_kqueue__loop_setup(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_kqueue__add_out(ସଂରଚନା mosquitto *context)
{
	ସଂରଚନା kevent ev;

	ଯଦି(context->events != EVFILT_WRITE){
		EV_SET(&ev, context->sock, EVFILT_WRITE, EV_ADD, 0, 0, context);
		ଯଦି(kevent(db.kqueuefd, &ev, 1, ଖାଲି, 0, ଖାଲି) == -1){
			log__printf(ଖାଲି, MOSQ_LOG_DEBUG, "Error in kqueue re-registering to EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = EVFILT_WRITE;
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_kqueue__remove_out(ସଂରଚନା mosquitto *context)
{
	ସଂରଚନା kevent ev;

	ଯଦି(context->events == EVFILT_WRITE){
		EV_SET(&ev, context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		ଯଦି(kevent(db.kqueuefd, &ev, 1, ଖାଲି, 0, ଖାଲି) == -1){
			log__printf(ଖାଲି, MOSQ_LOG_DEBUG, "Error in kqueue removing EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = 0;
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_kqueue__new(ସଂରଚନା mosquitto *context)
{
	ସଂରଚନା kevent ev;

	EV_SET(&ev, context->sock, EVFILT_READ, EV_ADD, 0, 0, context);
	ଯଦି(kevent(db.kqueuefd, &ev, 1, ଖାଲି, 0, ଖାଲି) == -1){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error in kqueue accepting: %s", strerror(errno));
	}

	context->events = 0;
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_kqueue__delete(ସଂରଚନା mosquitto *context)
{
	ସଂରଚନା kevent ev[2];

	ଯଦି(context->sock != INVALID_SOCKET){
		EV_SET(&ev[0], context->sock, EVFILT_READ, EV_DELETE, 0, 0, context);
		EV_SET(&ev[1], context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		ଯଦି(kevent(db.kqueuefd, ev, 2, ଖାଲି, 0, ଖାଲି) == -1){
			ଫେରନ୍ତୁ 1;
		}
	}
	ଫେରନ୍ତୁ 0;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_kqueue__handle(ଶୂନ୍ୟ)
{
	ସଂରଚନା mosquitto *context;
	ସଂରଚନା mosquitto__listener_sock *listensock;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା event_ଗଣନା;
	ସଂରଚନା timespec timeout;

#ifdef WITH_WEBSOCKETS
	timeout.tv_sec = 0;
	timeout.tv_nsec = 100000000; /* 100 ms */
#ଅନ୍ୟଥା
	timeout.tv_sec = db.next_event_ms/1000;
	timeout.tv_nsec = (db.next_event_ms - timeout.tv_sec*1000) * 1000000;
#endif

	event_ଗଣନା = kevent(db.kqueuefd,
			ଖାଲି, 0,
			event_list, MAX_EVENTS,
			&timeout);

	db.now_s = mosquitto_time();
	db.now_real_s = time(ଖାଲି);

	ବାଛ(event_ଗଣନା){
		ସ୍ଥିତି -1:
			ଯଦି(errno != EINTR){
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error in kqueue waiting: %s.", strerror(errno));
			}
			ଭାଙ୍ଗ;
		ସ୍ଥିତି 0:
			ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<event_ଗଣନା; i++){
				context = event_list[i].udata;
				ଯଦି(context->ident == id_client){
					loop_handle_reads_writes(context, event_list[i].filter);
					ଯଦି(event_list[i].flags & (EV_EOF | EV_ERROR)){
						do_disconnect(context, MOSQ_ERR_CONN_LOST);
					}
				}ଅନ୍ୟଥା ଯଦି(context->ident == id_listener){
					listensock = event_list[i].udata;

					ଯଦି(event_list[i].filter == EVFILT_READ){
						ଯେତେବେଳେ((context = net__socket_accept(listensock)) != ଖାଲି){
						}
					}
#ifdef WITH_WEBSOCKETS
				}ଅନ୍ୟଥା ଯଦି(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#endif
				}
			}
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_kqueue__cleanup(ଶୂନ୍ୟ)
{
	(ଶୂନ୍ୟ)close(db.kqueuefd);
	db.kqueuefd = 0;
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ଶୂନ୍ୟ loop_handle_reads_writes(ସଂରଚନା mosquitto *context, short event)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;
	socklen_t len;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି(context->sock == INVALID_SOCKET){
		ଫେରନ୍ତୁ;
	}

#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ଯଦି(context->wsi){
		ସଂରଚନା lws_pollfd wspoll;
		wspoll.fd = context->sock;
		int16_t lws_events;
		ବାଛ(event){
			ସ୍ଥିତି EVFILT_READ:
				lws_events = LWS_POLLIN;
				ଭାଙ୍ଗ;
			ସ୍ଥିତି EVFILT_WRITE:
				lws_events = LWS_POLLOUT;
				ଭାଙ୍ଗ;
			ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
				lws_events = LWS_POLLHUP;
				ଭାଙ୍ଗ;
		}
		wspoll.events = lws_events;
		wspoll.revents = lws_events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		ଫେରନ୍ତୁ;
	}
#endif

	ଯଦି(event == EVFILT_WRITE
#ifdef WITH_TLS
			|| context->want_write
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		ଯଦି(context->state == mosq_cs_connect_pending){
			len = ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
			ଯଦି(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (ଅକ୍ଷର *)&err, &len)){
				ଯଦି(err == 0){
					mosquitto__set_state(context, mosq_cs_new);
#ଯଦି defined(WITH_ADNS) && defined(WITH_BRIDGE)
					ଯଦି(context->bridge){
						bridge__connect_step3(context);
					}
#endif
				}
			}ଅନ୍ୟଥା{
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				ଫେରନ୍ତୁ;
			}
		}
		rc = packet__write(context);
		ଯଦି(rc){
			do_disconnect(context, rc);
			ଫେରନ୍ତୁ;
		}
	}

	ଯଦି(event == EVFILT_READ
#ifdef WITH_TLS
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		କର{
			ବାଛ(context->transport){
				ସ୍ଥିତି mosq_t_tcp:
				ସ୍ଥିତି mosq_t_ws:
					rc = packet__read(context);
					ଭାଙ୍ଗ;
#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
				ସ୍ଥିତି mosq_t_http:
					rc = http__read(context);
					ଭାଙ୍ଗ;
#endif
#ଯଦି !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
				/* Not supported with LWS */
				ସ୍ଥିତି mosq_t_proxy_v2:
					rc = proxy_v2__read(context);
					ଭାଙ୍ଗ;
				ସ୍ଥିତି mosq_t_proxy_v1:
					rc = proxy_v1__read(context);
					ଭାଙ୍ଗ;
#endif
				ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
					rc = MOSQ_ERR_INVAL;
					ଭାଙ୍ଗ;
			}
			ଯଦି(rc){
				do_disconnect(context, rc);
				ଫେରନ୍ତୁ;
			}
		}ଯେତେବେଳେ(SSL_DATA_PENDING(context));
	}
}
#endif
