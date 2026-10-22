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

#અન્તર્ભાવ "config.h"

#ifdef WITH_KQUEUE

#define MAX_EVENTS 1000

#અન્તર્ભાવ <errno.h>
#અન્તર્ભાવ <signal.h>
#અન્તર્ભાવ <sys/event.h>
#અન્તર્ભાવ <sys/socket.h>

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "packet_mosq.h"
#અન્તર્ભાવ "util_mosq.h"

સ્થિર શૂન્ય loop_handle_reads_writes(સંરચના mosquitto *context, short events);

સ્થિર સંરચના kevent event_list[MAX_EVENTS];


પૂર્ણાંક mux_kqueue__init(શૂન્ય)
{
	memset(&event_list, 0, કદ(સંરચના kevent)*MAX_EVENTS);

	db.kqueuefd = 0;
	જો((db.kqueuefd = kqueue()) == -1){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error in kqueue creating: %s", strerror(errno));
		પાછા MOSQ_ERR_UNKNOWN;
	}

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક mux_kqueue__add_listeners(સંરચના mosquitto__listener_sock *listensock, પૂર્ણાંક listensock_ગણતરી)
{
	સંરચના kevent ev;

	memset(&ev, 0, કદ(સંરચના kevent));

	માટે(પૂર્ણાંક i=0; i<listensock_ગણતરી; i++){
		EV_SET(&ev, listensock[i].sock, EVFILT_READ, EV_ADD, 0, 0, &listensock[i]);
		જો(kevent(db.kqueuefd, &ev, 1, ખાલી, 0, ખાલી) == -1){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error in kqueue initial registering: %s", strerror(errno));
			પાછા MOSQ_ERR_UNKNOWN;
		}
	}

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક mux_kqueue__delete_listeners(સંરચના mosquitto__listener_sock *listensock, પૂર્ણાંક listensock_ગણતરી)
{
	સંરચના kevent ev;

	માટે(પૂર્ણાંક i=0; i<listensock_ગણતરી; i++){
		EV_SET(&ev, listensock[i].sock, EVFILT_READ, EV_DELETE, 0, 0, &listensock[i]);
		જો(kevent(db.kqueuefd, &ev, 1, ખાલી, 0, ખાલી) == -1){
			પાછા MOSQ_ERR_UNKNOWN;
		}
	}

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક mux_kqueue__loop_setup(શૂન્ય)
{
	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક mux_kqueue__add_out(સંરચના mosquitto *context)
{
	સંરચના kevent ev;

	જો(context->events != EVFILT_WRITE){
		EV_SET(&ev, context->sock, EVFILT_WRITE, EV_ADD, 0, 0, context);
		જો(kevent(db.kqueuefd, &ev, 1, ખાલી, 0, ખાલી) == -1){
			log__printf(ખાલી, MOSQ_LOG_DEBUG, "Error in kqueue re-registering to EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = EVFILT_WRITE;
	}
	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક mux_kqueue__remove_out(સંરચના mosquitto *context)
{
	સંરચના kevent ev;

	જો(context->events == EVFILT_WRITE){
		EV_SET(&ev, context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		જો(kevent(db.kqueuefd, &ev, 1, ખાલી, 0, ખાલી) == -1){
			log__printf(ખાલી, MOSQ_LOG_DEBUG, "Error in kqueue removing EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = 0;
	}
	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક mux_kqueue__new(સંરચના mosquitto *context)
{
	સંરચના kevent ev;

	EV_SET(&ev, context->sock, EVFILT_READ, EV_ADD, 0, 0, context);
	જો(kevent(db.kqueuefd, &ev, 1, ખાલી, 0, ખાલી) == -1){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error in kqueue accepting: %s", strerror(errno));
	}

	context->events = 0;
	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક mux_kqueue__delete(સંરચના mosquitto *context)
{
	સંરચના kevent ev[2];

	જો(context->sock != INVALID_SOCKET){
		EV_SET(&ev[0], context->sock, EVFILT_READ, EV_DELETE, 0, 0, context);
		EV_SET(&ev[1], context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		જો(kevent(db.kqueuefd, ev, 2, ખાલી, 0, ખાલી) == -1){
			પાછા 1;
		}
	}
	પાછા 0;
}


પૂર્ણાંક mux_kqueue__handle(શૂન્ય)
{
	સંરચના mosquitto *context;
	સંરચના mosquitto__listener_sock *listensock;
	પૂર્ણાંક event_ગણતરી;
	સંરચના timespec timeout;

#ifdef WITH_WEBSOCKETS
	timeout.tv_sec = 0;
	timeout.tv_nsec = 100000000; /* 100 ms */
#નહીંતર
	timeout.tv_sec = db.next_event_ms/1000;
	timeout.tv_nsec = (db.next_event_ms - timeout.tv_sec*1000) * 1000000;
#endif

	event_ગણતરી = kevent(db.kqueuefd,
			ખાલી, 0,
			event_list, MAX_EVENTS,
			&timeout);

	db.now_s = mosquitto_time();
	db.now_real_s = time(ખાલી);

	પસંદગી(event_ગણતરી){
		સ્થિતિ -1:
			જો(errno != EINTR){
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error in kqueue waiting: %s.", strerror(errno));
			}
			તોડો;
		સ્થિતિ 0:
			તોડો;
		મૂળભૂત:
			માટે(પૂર્ણાંક i=0; i<event_ગણતરી; i++){
				context = event_list[i].udata;
				જો(context->ident == id_client){
					loop_handle_reads_writes(context, event_list[i].filter);
					જો(event_list[i].flags & (EV_EOF | EV_ERROR)){
						do_disconnect(context, MOSQ_ERR_CONN_LOST);
					}
				}નહીંતર જો(context->ident == id_listener){
					listensock = event_list[i].udata;

					જો(event_list[i].filter == EVFILT_READ){
						જ્યારે((context = net__socket_accept(listensock)) != ખાલી){
						}
					}
#ifdef WITH_WEBSOCKETS
				}નહીંતર જો(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#endif
				}
			}
	}
	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક mux_kqueue__cleanup(શૂન્ય)
{
	(શૂન્ય)close(db.kqueuefd);
	db.kqueuefd = 0;
	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર શૂન્ય loop_handle_reads_writes(સંરચના mosquitto *context, short event)
{
	પૂર્ણાંક err;
	socklen_t len;
	પૂર્ણાંક rc;

	જો(context->sock == INVALID_SOCKET){
		પાછા;
	}

#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	જો(context->wsi){
		સંરચના lws_pollfd wspoll;
		wspoll.fd = context->sock;
		int16_t lws_events;
		પસંદગી(event){
			સ્થિતિ EVFILT_READ:
				lws_events = LWS_POLLIN;
				તોડો;
			સ્થિતિ EVFILT_WRITE:
				lws_events = LWS_POLLOUT;
				તોડો;
			મૂળભૂત:
				lws_events = LWS_POLLHUP;
				તોડો;
		}
		wspoll.events = lws_events;
		wspoll.revents = lws_events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		પાછા;
	}
#endif

	જો(event == EVFILT_WRITE
#ifdef WITH_TLS
			|| context->want_write
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		જો(context->state == mosq_cs_connect_pending){
			len = કદ(પૂર્ણાંક);
			જો(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (અક્ષર *)&err, &len)){
				જો(err == 0){
					mosquitto__set_state(context, mosq_cs_new);
#જો defined(WITH_ADNS) && defined(WITH_BRIDGE)
					જો(context->bridge){
						bridge__connect_step3(context);
					}
#endif
				}
			}નહીંતર{
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				પાછા;
			}
		}
		rc = packet__write(context);
		જો(rc){
			do_disconnect(context, rc);
			પાછા;
		}
	}

	જો(event == EVFILT_READ
#ifdef WITH_TLS
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		કરો{
			પસંદગી(context->transport){
				સ્થિતિ mosq_t_tcp:
				સ્થિતિ mosq_t_ws:
					rc = packet__read(context);
					તોડો;
#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
				સ્થિતિ mosq_t_http:
					rc = http__read(context);
					તોડો;
#endif
#જો !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
				/* Not supported with LWS */
				સ્થિતિ mosq_t_proxy_v2:
					rc = proxy_v2__read(context);
					તોડો;
				સ્થિતિ mosq_t_proxy_v1:
					rc = proxy_v1__read(context);
					તોડો;
#endif
				મૂળભૂત:
					rc = MOSQ_ERR_INVAL;
					તોડો;
			}
			જો(rc){
				do_disconnect(context, rc);
				પાછા;
			}
		}જ્યારે(SSL_DATA_PENDING(context));
	}
}
#endif
