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

#അന്തര്ഭാവ "config.h"

#ifdef WITH_KQUEUE

#define MAX_EVENTS 1000

#അന്തര്ഭാവ <errno.h>
#അന്തര്ഭാവ <signal.h>
#അന്തര്ഭാവ <sys/event.h>
#അന്തര്ഭാവ <sys/socket.h>

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "packet_mosq.h"
#അന്തര്ഭാവ "util_mosq.h"

സ്ഥിതം ശൂന്യം loop_handle_reads_writes(ഘടന mosquitto *context, short events);

സ്ഥിതം ഘടന kevent event_list[MAX_EVENTS];


പൂർണ്ണസംഖ്യ mux_kqueue__init(ശൂന്യം)
{
	memset(&event_list, 0, വലുപ്പം(ഘടന kevent)*MAX_EVENTS);

	db.kqueuefd = 0;
	എങ്കിൽ((db.kqueuefd = kqueue()) == -1){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error in kqueue creating: %s", strerror(errno));
		തിരികെ MOSQ_ERR_UNKNOWN;
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ mux_kqueue__add_listeners(ഘടന mosquitto__listener_sock *listensock, പൂർണ്ണസംഖ്യ listensock_എണ്ണം)
{
	ഘടന kevent ev;

	memset(&ev, 0, വലുപ്പം(ഘടന kevent));

	വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<listensock_എണ്ണം; i++){
		EV_SET(&ev, listensock[i].sock, EVFILT_READ, EV_ADD, 0, 0, &listensock[i]);
		എങ്കിൽ(kevent(db.kqueuefd, &ev, 1, ശൂന്യ, 0, ശൂന്യ) == -1){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error in kqueue initial registering: %s", strerror(errno));
			തിരികെ MOSQ_ERR_UNKNOWN;
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ mux_kqueue__delete_listeners(ഘടന mosquitto__listener_sock *listensock, പൂർണ്ണസംഖ്യ listensock_എണ്ണം)
{
	ഘടന kevent ev;

	വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<listensock_എണ്ണം; i++){
		EV_SET(&ev, listensock[i].sock, EVFILT_READ, EV_DELETE, 0, 0, &listensock[i]);
		എങ്കിൽ(kevent(db.kqueuefd, &ev, 1, ശൂന്യ, 0, ശൂന്യ) == -1){
			തിരികെ MOSQ_ERR_UNKNOWN;
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ mux_kqueue__loop_setup(ശൂന്യം)
{
	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ mux_kqueue__add_out(ഘടന mosquitto *context)
{
	ഘടന kevent ev;

	എങ്കിൽ(context->events != EVFILT_WRITE){
		EV_SET(&ev, context->sock, EVFILT_WRITE, EV_ADD, 0, 0, context);
		എങ്കിൽ(kevent(db.kqueuefd, &ev, 1, ശൂന്യ, 0, ശൂന്യ) == -1){
			log__printf(ശൂന്യ, MOSQ_LOG_DEBUG, "Error in kqueue re-registering to EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = EVFILT_WRITE;
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ mux_kqueue__remove_out(ഘടന mosquitto *context)
{
	ഘടന kevent ev;

	എങ്കിൽ(context->events == EVFILT_WRITE){
		EV_SET(&ev, context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		എങ്കിൽ(kevent(db.kqueuefd, &ev, 1, ശൂന്യ, 0, ശൂന്യ) == -1){
			log__printf(ശൂന്യ, MOSQ_LOG_DEBUG, "Error in kqueue removing EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = 0;
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ mux_kqueue__new(ഘടന mosquitto *context)
{
	ഘടന kevent ev;

	EV_SET(&ev, context->sock, EVFILT_READ, EV_ADD, 0, 0, context);
	എങ്കിൽ(kevent(db.kqueuefd, &ev, 1, ശൂന്യ, 0, ശൂന്യ) == -1){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error in kqueue accepting: %s", strerror(errno));
	}

	context->events = 0;
	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ mux_kqueue__delete(ഘടന mosquitto *context)
{
	ഘടന kevent ev[2];

	എങ്കിൽ(context->sock != INVALID_SOCKET){
		EV_SET(&ev[0], context->sock, EVFILT_READ, EV_DELETE, 0, 0, context);
		EV_SET(&ev[1], context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		എങ്കിൽ(kevent(db.kqueuefd, ev, 2, ശൂന്യ, 0, ശൂന്യ) == -1){
			തിരികെ 1;
		}
	}
	തിരികെ 0;
}


പൂർണ്ണസംഖ്യ mux_kqueue__handle(ശൂന്യം)
{
	ഘടന mosquitto *context;
	ഘടന mosquitto__listener_sock *listensock;
	പൂർണ്ണസംഖ്യ event_എണ്ണം;
	ഘടന timespec timeout;

#ifdef WITH_WEBSOCKETS
	timeout.tv_sec = 0;
	timeout.tv_nsec = 100000000; /* 100 ms */
#അല്ലെങ്കിൽ
	timeout.tv_sec = db.next_event_ms/1000;
	timeout.tv_nsec = (db.next_event_ms - timeout.tv_sec*1000) * 1000000;
#endif

	event_എണ്ണം = kevent(db.kqueuefd,
			ശൂന്യ, 0,
			event_list, MAX_EVENTS,
			&timeout);

	db.now_s = mosquitto_time();
	db.now_real_s = time(ശൂന്യ);

	തിരഞ്ഞെടുപ്പ്(event_എണ്ണം){
		അവസ്ഥ -1:
			എങ്കിൽ(errno != EINTR){
				log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error in kqueue waiting: %s.", strerror(errno));
			}
			നിർത്തുക;
		അവസ്ഥ 0:
			നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
			വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<event_എണ്ണം; i++){
				context = event_list[i].udata;
				എങ്കിൽ(context->ident == id_client){
					loop_handle_reads_writes(context, event_list[i].filter);
					എങ്കിൽ(event_list[i].flags & (EV_EOF | EV_ERROR)){
						do_disconnect(context, MOSQ_ERR_CONN_LOST);
					}
				}അല്ലെങ്കിൽ എങ്കിൽ(context->ident == id_listener){
					listensock = event_list[i].udata;

					എങ്കിൽ(event_list[i].filter == EVFILT_READ){
						ആകുമ്പോൾ((context = net__socket_accept(listensock)) != ശൂന്യ){
						}
					}
#ifdef WITH_WEBSOCKETS
				}അല്ലെങ്കിൽ എങ്കിൽ(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#endif
				}
			}
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ mux_kqueue__cleanup(ശൂന്യം)
{
	(ശൂന്യം)close(db.kqueuefd);
	db.kqueuefd = 0;
	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം ശൂന്യം loop_handle_reads_writes(ഘടന mosquitto *context, short event)
{
	പൂർണ്ണസംഖ്യ err;
	socklen_t len;
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ(context->sock == INVALID_SOCKET){
		തിരികെ;
	}

#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	എങ്കിൽ(context->wsi){
		ഘടന lws_pollfd wspoll;
		wspoll.fd = context->sock;
		int16_t lws_events;
		തിരഞ്ഞെടുപ്പ്(event){
			അവസ്ഥ EVFILT_READ:
				lws_events = LWS_POLLIN;
				നിർത്തുക;
			അവസ്ഥ EVFILT_WRITE:
				lws_events = LWS_POLLOUT;
				നിർത്തുക;
			സ്ഥിരസ്ഥിതി:
				lws_events = LWS_POLLHUP;
				നിർത്തുക;
		}
		wspoll.events = lws_events;
		wspoll.revents = lws_events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		തിരികെ;
	}
#endif

	എങ്കിൽ(event == EVFILT_WRITE
#ifdef WITH_TLS
			|| context->want_write
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		എങ്കിൽ(context->state == mosq_cs_connect_pending){
			len = വലുപ്പം(പൂർണ്ണസംഖ്യ);
			എങ്കിൽ(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (അക്ഷരം *)&err, &len)){
				എങ്കിൽ(err == 0){
					mosquitto__set_state(context, mosq_cs_new);
#എങ്കിൽ defined(WITH_ADNS) && defined(WITH_BRIDGE)
					എങ്കിൽ(context->bridge){
						bridge__connect_step3(context);
					}
#endif
				}
			}അല്ലെങ്കിൽ{
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				തിരികെ;
			}
		}
		rc = packet__write(context);
		എങ്കിൽ(rc){
			do_disconnect(context, rc);
			തിരികെ;
		}
	}

	എങ്കിൽ(event == EVFILT_READ
#ifdef WITH_TLS
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		ചെയ്യുക{
			തിരഞ്ഞെടുപ്പ്(context->transport){
				അവസ്ഥ mosq_t_tcp:
				അവസ്ഥ mosq_t_ws:
					rc = packet__read(context);
					നിർത്തുക;
#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
				അവസ്ഥ mosq_t_http:
					rc = http__read(context);
					നിർത്തുക;
#endif
#എങ്കിൽ !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
				/* Not supported with LWS */
				അവസ്ഥ mosq_t_proxy_v2:
					rc = proxy_v2__read(context);
					നിർത്തുക;
				അവസ്ഥ mosq_t_proxy_v1:
					rc = proxy_v1__read(context);
					നിർത്തുക;
#endif
				സ്ഥിരസ്ഥിതി:
					rc = MOSQ_ERR_INVAL;
					നിർത്തുക;
			}
			എങ്കിൽ(rc){
				do_disconnect(context, rc);
				തിരികെ;
			}
		}ആകുമ്പോൾ(SSL_DATA_PENDING(context));
	}
}
#endif
