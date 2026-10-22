/*
Copyright (c) 2009-2021 Roger Light <roger@atchoo.org>

All rights reserved. This program and the accompanying materials
are made available under the terms of the Eclipse Public License 2.0
and Eclipse Distribution License v1.0 which accompany this distribution.

The Eclipse Public License is available at
   https://www.eclipse.org/legal/epl-2.0/
and the Eclipse Distribution License is available at
  http://www.eclipse.org/org/documents/edl-v10.php.

SPDX-License-Identifier: EPL-2.0 OR BSD-3-Clause

Contributors:
   Roger Light - initial implementation and documentation.
   Tatsuzo Osawa - Add epoll.
*/

#അന്തര്ഭാവ "config.h"

#ifdef WITH_EPOLL

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#ifndef WIN32
#  include <sys/epoll.h>
#  define MAX_EVENTS 1000
#endif

#അന്തര്ഭാവ <errno.h>
#അന്തര്ഭാവ <signal.h>

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mux.h"
#അന്തര്ഭാവ "packet_mosq.h"
#അന്തര്ഭാവ "util_mosq.h"

സ്ഥിതം ശൂന്യം loop_handle_reads_writes(ഘടന mosquitto *context, uint32_t events);

സ്ഥിതം ഘടന epoll_event ep_events[MAX_EVENTS];


പൂർണ്ണസംഖ്യ mux_epoll__init(ശൂന്യം)
{
	memset(&ep_events, 0, വലുപ്പം(ഘടന epoll_event)*MAX_EVENTS);

	db.epollfd = 0;
	എങ്കിൽ((db.epollfd = epoll_create(MAX_EVENTS)) == -1){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error in epoll creating: %s", strerror(errno));
		തിരികെ MOSQ_ERR_UNKNOWN;
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ mux_epoll__add_listeners(ഘടന mosquitto__listener_sock *listensock, പൂർണ്ണസംഖ്യ listensock_എണ്ണം)
{
	വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<listensock_എണ്ണം; i++){
		ഘടന epoll_event ev;

		memset(&ev, 0, വലുപ്പം(ഘടന epoll_event));
		ev.data.ptr = &listensock[i];
		ev.events = EPOLLIN;
		എങ്കിൽ(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, listensock[i].sock, &ev) == -1){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error in epoll initial registering: %s", strerror(errno));
			തിരികെ MOSQ_ERR_UNKNOWN;
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ mux_epoll__delete_listeners(ഘടന mosquitto__listener_sock *listensock, പൂർണ്ണസംഖ്യ listensock_എണ്ണം)
{
	വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<listensock_എണ്ണം; i++){
		എങ്കിൽ(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, listensock[i].sock, ശൂന്യ) == -1){
			തിരികെ MOSQ_ERR_UNKNOWN;
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ mux_epoll__add_out(ഘടന mosquitto *context)
{
	എങ്കിൽ(!(context->events & EPOLLOUT)){
		ഘടന epoll_event ev;

		memset(&ev, 0, വലുപ്പം(ഘടന epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN | EPOLLOUT;
		എങ്കിൽ(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			എങ്കിൽ((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(ശൂന്യ, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLOUT: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN | EPOLLOUT;
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ mux_epoll__remove_out(ഘടന mosquitto *context)
{
	എങ്കിൽ(context->events & EPOLLOUT){
		ഘടന epoll_event ev;

		memset(&ev, 0, വലുപ്പം(ഘടന epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN;
		എങ്കിൽ(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			എങ്കിൽ((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(ശൂന്യ, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLIN: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN;
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ mux_epoll__new(ഘടന mosquitto *context)
{
	ഘടന epoll_event ev;

	memset(&ev, 0, വലുപ്പം(ഘടന epoll_event));
	ev.events = EPOLLIN;
	ev.data.ptr = context;
	എങ്കിൽ(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1){
		എങ്കിൽ(errno != EEXIST){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error in epoll accepting: %s", strerror(errno));
		}
	}
	context->events = EPOLLIN;
	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ mux_epoll__delete(ഘടന mosquitto *context)
{
	എങ്കിൽ(context->sock != INVALID_SOCKET){
		എങ്കിൽ(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, context->sock, ശൂന്യ) == -1){
			തിരികെ 1;
		}
	}
	തിരികെ 0;
}


പൂർണ്ണസംഖ്യ mux_epoll__handle(ശൂന്യം)
{
	ഘടന epoll_event ev;
	ഘടന mosquitto *context;
	ഘടന mosquitto__listener_sock *listensock;
	പൂർണ്ണസംഖ്യ event_എണ്ണം;

	memset(&ev, 0, വലുപ്പം(ഘടന epoll_event));
#എങ്കിൽ defined(WITH_WEBSOCKETS)
	event_എണ്ണം = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, 100);
#അല്ലെങ്കിൽ
	event_എണ്ണം = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, db.next_event_ms);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(ശൂന്യ);

	തിരഞ്ഞെടുപ്പ്(event_എണ്ണം){
		അവസ്ഥ -1:
			എങ്കിൽ(errno != EINTR){
				log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error in epoll waiting: %s.", strerror(errno));
			}
			നിർത്തുക;
		അവസ്ഥ 0:
			നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
			വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<event_എണ്ണം; i++){
				context = ep_events[i].data.ptr;
				എങ്കിൽ(context->ident == id_client){
					loop_handle_reads_writes(context, ep_events[i].events);
				}അല്ലെങ്കിൽ എങ്കിൽ(context->ident == id_listener){
					listensock = ep_events[i].data.ptr;

					എങ്കിൽ(ep_events[i].events & (EPOLLIN | EPOLLPRI)){
						ആകുമ്പോൾ((context = net__socket_accept(listensock)) != ശൂന്യ){
						}
					}
#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				}അല്ലെങ്കിൽ എങ്കിൽ(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#endif
				}
			}
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ mux_epoll__cleanup(ശൂന്യം)
{
	(ശൂന്യം)close(db.epollfd);
	db.epollfd = 0;
	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം ശൂന്യം loop_handle_reads_writes(ഘടന mosquitto *context, uint32_t events)
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
		wspoll.events = (int16_t)context->events;
		wspoll.revents = (int16_t)events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		തിരികെ;
	}
#endif

	എങ്കിൽ(events & EPOLLOUT
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

	എങ്കിൽ(events & EPOLLIN
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
	}അല്ലെങ്കിൽ{
		എങ്കിൽ(events & (EPOLLERR | EPOLLHUP)){
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			തിരികെ;
		}
	}
}
#endif
