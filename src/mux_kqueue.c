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

#ಅನ್ತರ್ಭಾವ "config.h"

#ifdef WITH_KQUEUE

#define MAX_EVENTS 1000

#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <sys/event.h>
#ಅನ್ತರ್ಭಾವ <sys/socket.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "packet_mosq.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"

ಸ್ಥಿರ ಶೂನ್ಯ loop_handle_reads_writes(ಸಂರಚನಾ mosquitto *context, short events);

ಸ್ಥಿರ ಸಂರಚನಾ kevent event_list[MAX_EVENTS];


ಪೂರ್ಣಾಙ್ಕ mux_kqueue__init(ಶೂನ್ಯ)
{
	memset(&event_list, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ kevent)*MAX_EVENTS);

	db.kqueuefd = 0;
	ಯದಿ((db.kqueuefd = kqueue()) == -1){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error in kqueue creating: %s", strerror(errno));
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ mux_kqueue__add_listeners(ಸಂರಚನಾ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಙ್ಕ listensock_ಎಣಿಕೆ)
{
	ಸಂರಚನಾ kevent ev;

	memset(&ev, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ kevent));

	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<listensock_ಎಣಿಕೆ; i++){
		EV_SET(&ev, listensock[i].sock, EVFILT_READ, EV_ADD, 0, 0, &listensock[i]);
		ಯದಿ(kevent(db.kqueuefd, &ev, 1, ರಿಕ್ತ, 0, ರಿಕ್ತ) == -1){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error in kqueue initial registering: %s", strerror(errno));
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ mux_kqueue__delete_listeners(ಸಂರಚನಾ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಙ್ಕ listensock_ಎಣಿಕೆ)
{
	ಸಂರಚನಾ kevent ev;

	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<listensock_ಎಣಿಕೆ; i++){
		EV_SET(&ev, listensock[i].sock, EVFILT_READ, EV_DELETE, 0, 0, &listensock[i]);
		ಯದಿ(kevent(db.kqueuefd, &ev, 1, ರಿಕ್ತ, 0, ರಿಕ್ತ) == -1){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ mux_kqueue__loop_setup(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ mux_kqueue__add_out(ಸಂರಚನಾ mosquitto *context)
{
	ಸಂರಚನಾ kevent ev;

	ಯದಿ(context->events != EVFILT_WRITE){
		EV_SET(&ev, context->sock, EVFILT_WRITE, EV_ADD, 0, 0, context);
		ಯದಿ(kevent(db.kqueuefd, &ev, 1, ರಿಕ್ತ, 0, ರಿಕ್ತ) == -1){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG, "Error in kqueue re-registering to EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = EVFILT_WRITE;
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ mux_kqueue__remove_out(ಸಂರಚನಾ mosquitto *context)
{
	ಸಂರಚನಾ kevent ev;

	ಯದಿ(context->events == EVFILT_WRITE){
		EV_SET(&ev, context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		ಯದಿ(kevent(db.kqueuefd, &ev, 1, ರಿಕ್ತ, 0, ರಿಕ್ತ) == -1){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG, "Error in kqueue removing EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = 0;
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ mux_kqueue__new(ಸಂರಚನಾ mosquitto *context)
{
	ಸಂರಚನಾ kevent ev;

	EV_SET(&ev, context->sock, EVFILT_READ, EV_ADD, 0, 0, context);
	ಯದಿ(kevent(db.kqueuefd, &ev, 1, ರಿಕ್ತ, 0, ರಿಕ್ತ) == -1){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error in kqueue accepting: %s", strerror(errno));
	}

	context->events = 0;
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ mux_kqueue__delete(ಸಂರಚನಾ mosquitto *context)
{
	ಸಂರಚನಾ kevent ev[2];

	ಯದಿ(context->sock != INVALID_SOCKET){
		EV_SET(&ev[0], context->sock, EVFILT_READ, EV_DELETE, 0, 0, context);
		EV_SET(&ev[1], context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		ಯದಿ(kevent(db.kqueuefd, ev, 2, ರಿಕ್ತ, 0, ರಿಕ್ತ) == -1){
			ಪ್ರತ್ಯಾಗಮ 1;
		}
	}
	ಪ್ರತ್ಯಾಗಮ 0;
}


ಪೂರ್ಣಾಙ್ಕ mux_kqueue__handle(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ mosquitto *context;
	ಸಂರಚನಾ mosquitto__listener_sock *listensock;
	ಪೂರ್ಣಾಙ್ಕ event_ಎಣಿಕೆ;
	ಸಂರಚನಾ timespec timeout;

#ifdef WITH_WEBSOCKETS
	timeout.tv_sec = 0;
	timeout.tv_nsec = 100000000; /* 100 ms */
#ಅನ್ಯಥಾ
	timeout.tv_sec = db.next_event_ms/1000;
	timeout.tv_nsec = (db.next_event_ms - timeout.tv_sec*1000) * 1000000;
#endif

	event_ಎಣಿಕೆ = kevent(db.kqueuefd,
			ರಿಕ್ತ, 0,
			event_list, MAX_EVENTS,
			&timeout);

	db.now_s = mosquitto_time();
	db.now_real_s = time(ರಿಕ್ತ);

	ವಿಕಲ್ಪ(event_ಎಣಿಕೆ){
		ಪಕ್ಷ -1:
			ಯದಿ(errno != EINTR){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error in kqueue waiting: %s.", strerror(errno));
			}
			ಭಙ್ಗ;
		ಪಕ್ಷ 0:
			ಭಙ್ಗ;
		ಸ್ವಭಾವ:
			ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<event_ಎಣಿಕೆ; i++){
				context = event_list[i].udata;
				ಯದಿ(context->ident == id_client){
					loop_handle_reads_writes(context, event_list[i].filter);
					ಯದಿ(event_list[i].flags & (EV_EOF | EV_ERROR)){
						do_disconnect(context, MOSQ_ERR_CONN_LOST);
					}
				}ಅನ್ಯಥಾ ಯದಿ(context->ident == id_listener){
					listensock = event_list[i].udata;

					ಯದಿ(event_list[i].filter == EVFILT_READ){
						ಯಾವತ್((context = net__socket_accept(listensock)) != ರಿಕ್ತ){
						}
					}
#ifdef WITH_WEBSOCKETS
				}ಅನ್ಯಥಾ ಯದಿ(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#endif
				}
			}
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ mux_kqueue__cleanup(ಶೂನ್ಯ)
{
	(ಶೂನ್ಯ)close(db.kqueuefd);
	db.kqueuefd = 0;
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಶೂನ್ಯ loop_handle_reads_writes(ಸಂರಚನಾ mosquitto *context, short event)
{
	ಪೂರ್ಣಾಙ್ಕ err;
	socklen_t len;
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ(context->sock == INVALID_SOCKET){
		ಪ್ರತ್ಯಾಗಮ;
	}

#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ಯದಿ(context->wsi){
		ಸಂರಚನಾ lws_pollfd wspoll;
		wspoll.fd = context->sock;
		int16_t lws_events;
		ವಿಕಲ್ಪ(event){
			ಪಕ್ಷ EVFILT_READ:
				lws_events = LWS_POLLIN;
				ಭಙ್ಗ;
			ಪಕ್ಷ EVFILT_WRITE:
				lws_events = LWS_POLLOUT;
				ಭಙ್ಗ;
			ಸ್ವಭಾವ:
				lws_events = LWS_POLLHUP;
				ಭಙ್ಗ;
		}
		wspoll.events = lws_events;
		wspoll.revents = lws_events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		ಪ್ರತ್ಯಾಗಮ;
	}
#endif

	ಯದಿ(event == EVFILT_WRITE
#ifdef WITH_TLS
			|| context->want_write
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		ಯದಿ(context->state == mosq_cs_connect_pending){
			len = ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ);
			ಯದಿ(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (ವರ್ಣ *)&err, &len)){
				ಯದಿ(err == 0){
					mosquitto__set_state(context, mosq_cs_new);
#ಯದಿ defined(WITH_ADNS) && defined(WITH_BRIDGE)
					ಯದಿ(context->bridge){
						bridge__connect_step3(context);
					}
#endif
				}
			}ಅನ್ಯಥಾ{
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				ಪ್ರತ್ಯಾಗಮ;
			}
		}
		rc = packet__write(context);
		ಯದಿ(rc){
			do_disconnect(context, rc);
			ಪ್ರತ್ಯಾಗಮ;
		}
	}

	ಯದಿ(event == EVFILT_READ
#ifdef WITH_TLS
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		ಕುರು{
			ವಿಕಲ್ಪ(context->transport){
				ಪಕ್ಷ mosq_t_tcp:
				ಪಕ್ಷ mosq_t_ws:
					rc = packet__read(context);
					ಭಙ್ಗ;
#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
				ಪಕ್ಷ mosq_t_http:
					rc = http__read(context);
					ಭಙ್ಗ;
#endif
#ಯದಿ !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
				/* Not supported with LWS */
				ಪಕ್ಷ mosq_t_proxy_v2:
					rc = proxy_v2__read(context);
					ಭಙ್ಗ;
				ಪಕ್ಷ mosq_t_proxy_v1:
					rc = proxy_v1__read(context);
					ಭಙ್ಗ;
#endif
				ಸ್ವಭಾವ:
					rc = MOSQ_ERR_INVAL;
					ಭಙ್ಗ;
			}
			ಯದಿ(rc){
				do_disconnect(context, rc);
				ಪ್ರತ್ಯಾಗಮ;
			}
		}ಯಾವತ್(SSL_DATA_PENDING(context));
	}
}
#endif
