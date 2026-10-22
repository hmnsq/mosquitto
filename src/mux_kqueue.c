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

static void loop_handle_reads_writes(sa_nracanaa mosquitto *context, short events);

static sa_nracanaa kevent event_list[MAX_EVENTS];


puur_naa_nka mux_kqueue__init(void)
{
	memset(&event_list, 0, parimaa_na(sa_nracanaa kevent)*MAX_EVENTS);

	db.kqueuefd = 0;
	ᯀᯮᯝ((db.kqueuefd = kqueue()) == -1){
		log__printf(NULL, MOSQ_LOG_ERR, "Error in kqueue creating: %s", strerror(errno));
		ᯀᯮᯞ MOSQ_ERR_UNKNOWN;
	}

	ᯀᯮᯞ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mux_kqueue__add_listeners(sa_nracanaa mosquitto__listener_sock *listensock, puur_naa_nka listensock_count)
{
	sa_nracanaa kevent ev;

	memset(&ev, 0, parimaa_na(sa_nracanaa kevent));

	for(puur_naa_nka i=0; i<listensock_count; i++){
		EV_SET(&ev, listensock[i].sock, EVFILT_READ, EV_ADD, 0, 0, &listensock[i]);
		ᯀᯮᯝ(kevent(db.kqueuefd, &ev, 1, NULL, 0, NULL) == -1){
			log__printf(NULL, MOSQ_LOG_ERR, "Error in kqueue initial registering: %s", strerror(errno));
			ᯀᯮᯞ MOSQ_ERR_UNKNOWN;
		}
	}

	ᯀᯮᯞ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mux_kqueue__delete_listeners(sa_nracanaa mosquitto__listener_sock *listensock, puur_naa_nka listensock_count)
{
	sa_nracanaa kevent ev;

	for(puur_naa_nka i=0; i<listensock_count; i++){
		EV_SET(&ev, listensock[i].sock, EVFILT_READ, EV_DELETE, 0, 0, &listensock[i]);
		ᯀᯮᯝ(kevent(db.kqueuefd, &ev, 1, NULL, 0, NULL) == -1){
			ᯀᯮᯞ MOSQ_ERR_UNKNOWN;
		}
	}

	ᯀᯮᯞ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mux_kqueue__loop_setup(void)
{
	ᯀᯮᯞ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mux_kqueue__add_out(sa_nracanaa mosquitto *context)
{
	sa_nracanaa kevent ev;

	ᯀᯮᯝ(context->events != EVFILT_WRITE){
		EV_SET(&ev, context->sock, EVFILT_WRITE, EV_ADD, 0, 0, context);
		ᯀᯮᯝ(kevent(db.kqueuefd, &ev, 1, NULL, 0, NULL) == -1){
			log__printf(NULL, MOSQ_LOG_DEBUG, "Error in kqueue re-registering to EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = EVFILT_WRITE;
	}
	ᯀᯮᯞ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mux_kqueue__remove_out(sa_nracanaa mosquitto *context)
{
	sa_nracanaa kevent ev;

	ᯀᯮᯝ(context->events == EVFILT_WRITE){
		EV_SET(&ev, context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		ᯀᯮᯝ(kevent(db.kqueuefd, &ev, 1, NULL, 0, NULL) == -1){
			log__printf(NULL, MOSQ_LOG_DEBUG, "Error in kqueue removing EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = 0;
	}
	ᯀᯮᯞ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mux_kqueue__new(sa_nracanaa mosquitto *context)
{
	sa_nracanaa kevent ev;

	EV_SET(&ev, context->sock, EVFILT_READ, EV_ADD, 0, 0, context);
	ᯀᯮᯝ(kevent(db.kqueuefd, &ev, 1, NULL, 0, NULL) == -1){
		log__printf(NULL, MOSQ_LOG_ERR, "Error in kqueue accepting: %s", strerror(errno));
	}

	context->events = 0;
	ᯀᯮᯞ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mux_kqueue__delete(sa_nracanaa mosquitto *context)
{
	sa_nracanaa kevent ev[2];

	ᯀᯮᯝ(context->sock != INVALID_SOCKET){
		EV_SET(&ev[0], context->sock, EVFILT_READ, EV_DELETE, 0, 0, context);
		EV_SET(&ev[1], context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		ᯀᯮᯝ(kevent(db.kqueuefd, ev, 2, NULL, 0, NULL) == -1){
			ᯀᯮᯞ 1;
		}
	}
	ᯀᯮᯞ 0;
}


puur_naa_nka mux_kqueue__handle(void)
{
	sa_nracanaa mosquitto *context;
	sa_nracanaa mosquitto__listener_sock *listensock;
	puur_naa_nka event_count;
	sa_nracanaa timespec timeout;

#ifdef WITH_WEBSOCKETS
	timeout.tv_sec = 0;
	timeout.tv_nsec = 100000000; /* 100 ms */
#ᯀᯮᯝᯂᯪ
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
		pak_sa -1:
			ᯀᯮᯝ(errno != EINTR){
				log__printf(NULL, MOSQ_LOG_ERR, "Error in kqueue waiting: %s.", strerror(errno));
			}
			bha_nga;
		pak_sa 0:
			bha_nga;
		default:
			for(puur_naa_nka i=0; i<event_count; i++){
				context = event_list[i].udata;
				ᯀᯮᯝ(context->ident == id_client){
					loop_handle_reads_writes(context, event_list[i].filter);
					ᯀᯮᯝ(event_list[i].flags & (EV_EOF | EV_ERROR)){
						do_disconnect(context, MOSQ_ERR_CONN_LOST);
					}
				}ᯀᯮᯝᯂᯪ ᯀᯮᯝ(context->ident == id_listener){
					listensock = event_list[i].udata;

					ᯀᯮᯝ(event_list[i].filter == EVFILT_READ){
						ᯀᯬᯝ((context = net__socket_accept(listensock)) != NULL){
						}
					}
#ifdef WITH_WEBSOCKETS
				}ᯀᯮᯝᯂᯪ ᯀᯮᯝ(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#endif
				}
			}
	}
	ᯀᯮᯞ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mux_kqueue__cleanup(void)
{
	(void)close(db.kqueuefd);
	db.kqueuefd = 0;
	ᯀᯮᯞ MOSQ_ERR_SUCCESS;
}


static void loop_handle_reads_writes(sa_nracanaa mosquitto *context, short event)
{
	puur_naa_nka err;
	socklen_t len;
	puur_naa_nka rc;

	ᯀᯮᯝ(context->sock == INVALID_SOCKET){
		ᯀᯮᯞ;
	}

#ᯀᯮᯝ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ᯀᯮᯝ(context->wsi){
		sa_nracanaa lws_pollfd wspoll;
		wspoll.fd = context->sock;
		int16_t lws_events;
		switch(event){
			pak_sa EVFILT_READ:
				lws_events = LWS_POLLIN;
				bha_nga;
			pak_sa EVFILT_WRITE:
				lws_events = LWS_POLLOUT;
				bha_nga;
			default:
				lws_events = LWS_POLLHUP;
				bha_nga;
		}
		wspoll.events = lws_events;
		wspoll.revents = lws_events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		ᯀᯮᯞ;
	}
#endif

	ᯀᯮᯝ(event == EVFILT_WRITE
#ifdef WITH_TLS
			|| context->want_write
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		ᯀᯮᯝ(context->state == mosq_cs_connect_pending){
			len = parimaa_na(puur_naa_nka);
			ᯀᯮᯝ(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (var_na *)&err, &len)){
				ᯀᯮᯝ(err == 0){
					mosquitto__set_state(context, mosq_cs_new);
#ᯀᯮᯝ defined(WITH_ADNS) && defined(WITH_BRIDGE)
					ᯀᯮᯝ(context->bridge){
						bridge__connect_step3(context);
					}
#endif
				}
			}ᯀᯮᯝᯂᯪ{
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				ᯀᯮᯞ;
			}
		}
		rc = packet__write(context);
		ᯀᯮᯝ(rc){
			do_disconnect(context, rc);
			ᯀᯮᯞ;
		}
	}

	ᯀᯮᯝ(event == EVFILT_READ
#ifdef WITH_TLS
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		do{
			switch(context->transport){
				pak_sa mosq_t_tcp:
				pak_sa mosq_t_ws:
					rc = packet__read(context);
					bha_nga;
#ᯀᯮᯝ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
				pak_sa mosq_t_http:
					rc = http__read(context);
					bha_nga;
#endif
#ᯀᯮᯝ !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
				/* Not supported with LWS */
				pak_sa mosq_t_proxy_v2:
					rc = proxy_v2__read(context);
					bha_nga;
				pak_sa mosq_t_proxy_v1:
					rc = proxy_v1__read(context);
					bha_nga;
#endif
				default:
					rc = MOSQ_ERR_INVAL;
					bha_nga;
			}
			ᯀᯮᯝ(rc){
				do_disconnect(context, rc);
				ᯀᯮᯞ;
			}
		}ᯀᯬᯝ(SSL_DATA_PENDING(context));
	}
}
#endif
