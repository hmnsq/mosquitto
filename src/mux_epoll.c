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

#ifdef WITH_EPOLL

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#ifndef WIN32
#  include <sys/epoll.h>
#  define MAX_EVENTS 1000
#endif

#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ <signal.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mux.h"
#ಅನ್ತರ್ಭಾವ "packet_mosq.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"

ಸ್ಥಿರ ಶೂನ್ಯ loop_handle_reads_writes(ಸಂರಚನಾ mosquitto *context, uint32_t events);

ಸ್ಥಿರ ಸಂರಚನಾ epoll_event ep_events[MAX_EVENTS];


ಪೂರ್ಣಾಙ್ಕ mux_epoll__init(ಶೂನ್ಯ)
{
	memset(&ep_events, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ epoll_event)*MAX_EVENTS);

	db.epollfd = 0;
	ಯದಿ((db.epollfd = epoll_create(MAX_EVENTS)) == -1){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error in epoll creating: %s", strerror(errno));
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ mux_epoll__add_listeners(ಸಂರಚನಾ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಙ್ಕ listensock_ಎಣಿಕೆ)
{
	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<listensock_ಎಣಿಕೆ; i++){
		ಸಂರಚನಾ epoll_event ev;

		memset(&ev, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ epoll_event));
		ev.data.ptr = &listensock[i];
		ev.events = EPOLLIN;
		ಯದಿ(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, listensock[i].sock, &ev) == -1){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error in epoll initial registering: %s", strerror(errno));
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ mux_epoll__delete_listeners(ಸಂರಚನಾ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಙ್ಕ listensock_ಎಣಿಕೆ)
{
	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<listensock_ಎಣಿಕೆ; i++){
		ಯದಿ(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, listensock[i].sock, ರಿಕ್ತ) == -1){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ mux_epoll__add_out(ಸಂರಚನಾ mosquitto *context)
{
	ಯದಿ(!(context->events & EPOLLOUT)){
		ಸಂರಚನಾ epoll_event ev;

		memset(&ev, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN | EPOLLOUT;
		ಯದಿ(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			ಯದಿ((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLOUT: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN | EPOLLOUT;
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ mux_epoll__remove_out(ಸಂರಚನಾ mosquitto *context)
{
	ಯದಿ(context->events & EPOLLOUT){
		ಸಂರಚನಾ epoll_event ev;

		memset(&ev, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN;
		ಯದಿ(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			ಯದಿ((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLIN: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN;
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ mux_epoll__new(ಸಂರಚನಾ mosquitto *context)
{
	ಸಂರಚನಾ epoll_event ev;

	memset(&ev, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ epoll_event));
	ev.events = EPOLLIN;
	ev.data.ptr = context;
	ಯದಿ(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1){
		ಯದಿ(errno != EEXIST){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error in epoll accepting: %s", strerror(errno));
		}
	}
	context->events = EPOLLIN;
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ mux_epoll__delete(ಸಂರಚನಾ mosquitto *context)
{
	ಯದಿ(context->sock != INVALID_SOCKET){
		ಯದಿ(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, context->sock, ರಿಕ್ತ) == -1){
			ಪ್ರತ್ಯಾಗಮ 1;
		}
	}
	ಪ್ರತ್ಯಾಗಮ 0;
}


ಪೂರ್ಣಾಙ್ಕ mux_epoll__handle(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ epoll_event ev;
	ಸಂರಚನಾ mosquitto *context;
	ಸಂರಚನಾ mosquitto__listener_sock *listensock;
	ಪೂರ್ಣಾಙ್ಕ event_ಎಣಿಕೆ;

	memset(&ev, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ epoll_event));
#ಯದಿ defined(WITH_WEBSOCKETS)
	event_ಎಣಿಕೆ = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, 100);
#ಅನ್ಯಥಾ
	event_ಎಣಿಕೆ = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, db.next_event_ms);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(ರಿಕ್ತ);

	ವಿಕಲ್ಪ(event_ಎಣಿಕೆ){
		ಪಕ್ಷ -1:
			ಯದಿ(errno != EINTR){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error in epoll waiting: %s.", strerror(errno));
			}
			ಭಙ್ಗ;
		ಪಕ್ಷ 0:
			ಭಙ್ಗ;
		ಸ್ವಭಾವ:
			ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<event_ಎಣಿಕೆ; i++){
				context = ep_events[i].data.ptr;
				ಯದಿ(context->ident == id_client){
					loop_handle_reads_writes(context, ep_events[i].events);
				}ಅನ್ಯಥಾ ಯದಿ(context->ident == id_listener){
					listensock = ep_events[i].data.ptr;

					ಯದಿ(ep_events[i].events & (EPOLLIN | EPOLLPRI)){
						ಯಾವತ್((context = net__socket_accept(listensock)) != ರಿಕ್ತ){
						}
					}
#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				}ಅನ್ಯಥಾ ಯದಿ(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#endif
				}
			}
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ mux_epoll__cleanup(ಶೂನ್ಯ)
{
	(ಶೂನ್ಯ)close(db.epollfd);
	db.epollfd = 0;
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಶೂನ್ಯ loop_handle_reads_writes(ಸಂರಚನಾ mosquitto *context, uint32_t events)
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
		wspoll.events = (int16_t)context->events;
		wspoll.revents = (int16_t)events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		ಪ್ರತ್ಯಾಗಮ;
	}
#endif

	ಯದಿ(events & EPOLLOUT
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

	ಯದಿ(events & EPOLLIN
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
	}ಅನ್ಯಥಾ{
		ಯದಿ(events & (EPOLLERR | EPOLLHUP)){
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			ಪ್ರತ್ಯಾಗಮ;
		}
	}
}
#endif
