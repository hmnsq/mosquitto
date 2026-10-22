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

#ಅನ್ತರ್ಭಾವ "config.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_EPOLL

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
#  define GNU_ಮೂಲ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
#  include <sys/epoll.h>
#  define MAX_EVENTS 1000
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ <signal.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mux.h"
#ಅನ್ತರ್ಭಾವ "packet_mosq.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"

ಸ್ಥಿರ ಶೂನ್ಯ loop_handle_reads_writes(ರಚನೆ mosquitto *context, uint32_t events);

ಸ್ಥಿರ ರಚನೆ epoll_event ep_events[MAX_EVENTS];


ಪೂರ್ಣಾಂಕ mux_epoll__init(ಶೂನ್ಯ)
{
	memset(&ep_events, 0, ಗಾತ್ರ(ರಚನೆ epoll_event)*MAX_EVENTS);

	db.epollfd = 0;
	ಆದರೆ((db.epollfd = epoll_create(MAX_EVENTS)) == -1){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error in epoll creating: %s", strerror(errno));
		ಹಿಂತಿರುಗಿ MOSQ_ERR_UNKNOWN;
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ mux_epoll_ಕೂಡಿಸು_listeners(ರಚನೆ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಂಕ listensock_ಎಣಿಕೆ)
{
	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<listensock_ಎಣಿಕೆ; i++){
		ರಚನೆ epoll_event ev;

		memset(&ev, 0, ಗಾತ್ರ(ರಚನೆ epoll_event));
		ev.ದತ್ತಾಂಶ.ptr = &listensock[i];
		ev.events = EPOLLIN;
		ಆದರೆ(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, listensock[i].sock, &ev) == -1){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error in epoll initial registering: %s", strerror(errno));
			ಹಿಂತಿರುಗಿ MOSQ_ERR_UNKNOWN;
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ mux_epoll__delete_listeners(ರಚನೆ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಂಕ listensock_ಎಣಿಕೆ)
{
	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<listensock_ಎಣಿಕೆ; i++){
		ಆದರೆ(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, listensock[i].sock, ಖಾಲಿ) == -1){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_UNKNOWN;
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ mux_epoll_ಕೂಡಿಸು_ಹೊರಹೋಗು(ರಚನೆ mosquitto *context)
{
	ಆದರೆ(!(context->events & EPOLLOUT)){
		ರಚನೆ epoll_event ev;

		memset(&ev, 0, ಗಾತ್ರ(ರಚನೆ epoll_event));
		ev.ದತ್ತಾಂಶ.ptr = context;
		ev.events = EPOLLIN | EPOLLOUT;
		ಆದರೆ(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			ಆದರೆ((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(ಖಾಲಿ, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLOUT: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN | EPOLLOUT;
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ mux_epoll_remove_ಹೊರಹೋಗು(ರಚನೆ mosquitto *context)
{
	ಆದರೆ(context->events & EPOLLOUT){
		ರಚನೆ epoll_event ev;

		memset(&ev, 0, ಗಾತ್ರ(ರಚನೆ epoll_event));
		ev.ದತ್ತಾಂಶ.ptr = context;
		ev.events = EPOLLIN;
		ಆದರೆ(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			ಆದರೆ((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(ಖಾಲಿ, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLIN: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN;
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ mux_epoll__new(ರಚನೆ mosquitto *context)
{
	ರಚನೆ epoll_event ev;

	memset(&ev, 0, ಗಾತ್ರ(ರಚನೆ epoll_event));
	ev.events = EPOLLIN;
	ev.ದತ್ತಾಂಶ.ptr = context;
	ಆದರೆ(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1){
		ಆದರೆ(errno != EEXIST){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error in epoll accepting: %s", strerror(errno));
		}
	}
	context->events = EPOLLIN;
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ mux_epoll__delete(ರಚನೆ mosquitto *context)
{
	ಆದರೆ(context->sock != INVALID_SOCKET){
		ಆದರೆ(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, context->sock, ಖಾಲಿ) == -1){
			ಹಿಂತಿರುಗಿ 1;
		}
	}
	ಹಿಂತಿರುಗಿ 0;
}


ಪೂರ್ಣಾಂಕ mux_epoll__handle(ಶೂನ್ಯ)
{
	ರಚನೆ epoll_event ev;
	ರಚನೆ mosquitto *context;
	ರಚನೆ mosquitto__listener_sock *listensock;
	ಪೂರ್ಣಾಂಕ event_ಎಣಿಕೆ;

	memset(&ev, 0, ಗಾತ್ರ(ರಚನೆ epoll_event));
#ಆದರೆ defined(WITH_WEBSOCKETS)
	event_ಎಣಿಕೆ = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, 100);
#ಇಲ್ಲದಿದ್ದರೆ
	event_ಎಣಿಕೆ = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, db.ಮುಂದಿನ_event_ms);
#ಆದರೆ_ಅಂತ್ಯ

	db.now_s = mosquitto_ಸಮಯ();
	db.now_real_s = ಸಮಯ(ಖಾಲಿ);

	ಆಯ್ಕೆ(event_ಎಣಿಕೆ){
		ಸ್ಥಿತಿ -1:
			ಆದರೆ(errno != EINTR){
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error in epoll waiting: %s.", strerror(errno));
			}
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 0:
			ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
			ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<event_ಎಣಿಕೆ; i++){
				context = ep_events[i].ದತ್ತಾಂಶ.ptr;
				ಆದರೆ(context->ident == id_client){
					loop_handle_reads_writes(context, ep_events[i].events);
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(context->ident == id_listener){
					listensock = ep_events[i].ದತ್ತಾಂಶ.ptr;

					ಆದರೆ(ep_events[i].events & (EPOLLIN | EPOLLPRI)){
						ಆಗ((context = net__socket_accept(listensock)) != ಖಾಲಿ){
						}
					}
#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#ಆದರೆ_ಅಂತ್ಯ
				}
			}
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ mux_epoll__cleanup(ಶೂನ್ಯ)
{
	(ಶೂನ್ಯ)close(db.epollfd);
	db.epollfd = 0;
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಶೂನ್ಯ loop_handle_reads_writes(ರಚನೆ mosquitto *context, uint32_t events)
{
	ಪೂರ್ಣಾಂಕ err;
	socklen_t len;
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ(context->sock == INVALID_SOCKET){
		ಹಿಂತಿರುಗಿ;
	}

#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ಆದರೆ(context->wsi){
		ರಚನೆ lws_pollfd wspoll;
		wspoll.fd = context->sock;
		wspoll.events = (int16_t)context->events;
		wspoll.revents = (int16_t)events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		ಹಿಂತಿರುಗಿ;
	}
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ(events & EPOLLOUT
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
			|| context->want_write
			|| (context->ssl && context->state == mosq_cs_new)
#ಆದರೆ_ಅಂತ್ಯ
			){

		ಆದರೆ(context->state == mosq_cs_connect_pending){
			len = ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ);
			ಆದರೆ(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (ಅಕ್ಷರ *)&err, &len)){
				ಆದರೆ(err == 0){
					mosquitto_ಗಣ_state(context, mosq_cs_new);
#ಆದರೆ defined(WITH_ADNS) && defined(WITH_BRIDGE)
					ಆದರೆ(context->bridge){
						bridge__connect_step3(context);
					}
#ಆದರೆ_ಅಂತ್ಯ
				}
			}ಇಲ್ಲದಿದ್ದರೆ{
				do_disconnect(context, MOSQ_ERR_CONN_ಕಳೆದುಹೋದ);
				ಹಿಂತಿರುಗಿ;
			}
		}
		rc = packet__write(context);
		ಆದರೆ(rc){
			do_disconnect(context, rc);
			ಹಿಂತಿರುಗಿ;
		}
	}

	ಆದರೆ(events & EPOLLIN
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
			|| (context->ssl && context->state == mosq_cs_new)
#ಆದರೆ_ಅಂತ್ಯ
			){

		ಮಾಡು{
			ಆಯ್ಕೆ(context->transport){
				ಸ್ಥಿತಿ mosq_t_tcp:
				ಸ್ಥಿತಿ mosq_t_ws:
					rc = packet__read(context);
					ನಿಲ್ಲಿಸು;
#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
				ಸ್ಥಿತಿ mosq_t_http:
					rc = http__read(context);
					ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
				/* Not supported with LWS */
				ಸ್ಥಿತಿ mosq_t_proxy_v2:
					rc = proxy_v2__read(context);
					ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ mosq_t_proxy_v1:
					rc = proxy_v1__read(context);
					ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
				ಪೂರ್ವನಿಯೋಜಿತ:
					rc = MOSQ_ERR_INVAL;
					ನಿಲ್ಲಿಸು;
			}
			ಆದರೆ(rc){
				do_disconnect(context, rc);
				ಹಿಂತಿರುಗಿ;
			}
		}ಆಗ(SSL_ದತ್ತಾಂಶ_PENDING(context));
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಆದರೆ(events & (EPOLLERR | EPOLLHUP)){
			do_disconnect(context, MOSQ_ERR_CONN_ಕಳೆದುಹೋದ);
			ಹಿಂತಿರುಗಿ;
		}
	}
}
#ಆದರೆ_ಅಂತ್ಯ
