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

#અન્તર્ભાવ "config.h"

#ifdef WITH_EPOLL

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#ifndef WIN32
#  include <sys/epoll.h>
#  define MAX_EVENTS 1000
#endif

#અન્તર્ભાવ <errno.h>
#અન્તર્ભાવ <signal.h>

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mux.h"
#અન્તર્ભાવ "packet_mosq.h"
#અન્તર્ભાવ "util_mosq.h"

સ્થિર શૂન્ય loop_handle_reads_writes(સંરચના mosquitto *context, uint32_t events);

સ્થિર સંરચના epoll_event ep_events[MAX_EVENTS];


પૂર્ણાંક mux_epoll__init(શૂન્ય)
{
	memset(&ep_events, 0, કદ(સંરચના epoll_event)*MAX_EVENTS);

	db.epollfd = 0;
	જો((db.epollfd = epoll_create(MAX_EVENTS)) == -1){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error in epoll creating: %s", strerror(errno));
		પાછા MOSQ_ERR_UNKNOWN;
	}

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક mux_epoll__add_listeners(સંરચના mosquitto__listener_sock *listensock, પૂર્ણાંક listensock_ગણતરી)
{
	માટે(પૂર્ણાંક i=0; i<listensock_ગણતરી; i++){
		સંરચના epoll_event ev;

		memset(&ev, 0, કદ(સંરચના epoll_event));
		ev.data.ptr = &listensock[i];
		ev.events = EPOLLIN;
		જો(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, listensock[i].sock, &ev) == -1){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error in epoll initial registering: %s", strerror(errno));
			પાછા MOSQ_ERR_UNKNOWN;
		}
	}

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક mux_epoll__delete_listeners(સંરચના mosquitto__listener_sock *listensock, પૂર્ણાંક listensock_ગણતરી)
{
	માટે(પૂર્ણાંક i=0; i<listensock_ગણતરી; i++){
		જો(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, listensock[i].sock, ખાલી) == -1){
			પાછા MOSQ_ERR_UNKNOWN;
		}
	}

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક mux_epoll__add_out(સંરચના mosquitto *context)
{
	જો(!(context->events & EPOLLOUT)){
		સંરચના epoll_event ev;

		memset(&ev, 0, કદ(સંરચના epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN | EPOLLOUT;
		જો(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			જો((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(ખાલી, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLOUT: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN | EPOLLOUT;
	}
	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક mux_epoll__remove_out(સંરચના mosquitto *context)
{
	જો(context->events & EPOLLOUT){
		સંરચના epoll_event ev;

		memset(&ev, 0, કદ(સંરચના epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN;
		જો(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			જો((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(ખાલી, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLIN: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN;
	}
	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક mux_epoll__new(સંરચના mosquitto *context)
{
	સંરચના epoll_event ev;

	memset(&ev, 0, કદ(સંરચના epoll_event));
	ev.events = EPOLLIN;
	ev.data.ptr = context;
	જો(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1){
		જો(errno != EEXIST){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error in epoll accepting: %s", strerror(errno));
		}
	}
	context->events = EPOLLIN;
	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક mux_epoll__delete(સંરચના mosquitto *context)
{
	જો(context->sock != INVALID_SOCKET){
		જો(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, context->sock, ખાલી) == -1){
			પાછા 1;
		}
	}
	પાછા 0;
}


પૂર્ણાંક mux_epoll__handle(શૂન્ય)
{
	સંરચના epoll_event ev;
	સંરચના mosquitto *context;
	સંરચના mosquitto__listener_sock *listensock;
	પૂર્ણાંક event_ગણતરી;

	memset(&ev, 0, કદ(સંરચના epoll_event));
#જો defined(WITH_WEBSOCKETS)
	event_ગણતરી = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, 100);
#નહીંતર
	event_ગણતરી = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, db.next_event_ms);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(ખાલી);

	પસંદગી(event_ગણતરી){
		સ્થિતિ -1:
			જો(errno != EINTR){
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error in epoll waiting: %s.", strerror(errno));
			}
			તોડો;
		સ્થિતિ 0:
			તોડો;
		મૂળભૂત:
			માટે(પૂર્ણાંક i=0; i<event_ગણતરી; i++){
				context = ep_events[i].data.ptr;
				જો(context->ident == id_client){
					loop_handle_reads_writes(context, ep_events[i].events);
				}નહીંતર જો(context->ident == id_listener){
					listensock = ep_events[i].data.ptr;

					જો(ep_events[i].events & (EPOLLIN | EPOLLPRI)){
						જ્યારે((context = net__socket_accept(listensock)) != ખાલી){
						}
					}
#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				}નહીંતર જો(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#endif
				}
			}
	}
	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક mux_epoll__cleanup(શૂન્ય)
{
	(શૂન્ય)close(db.epollfd);
	db.epollfd = 0;
	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર શૂન્ય loop_handle_reads_writes(સંરચના mosquitto *context, uint32_t events)
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
		wspoll.events = (int16_t)context->events;
		wspoll.revents = (int16_t)events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		પાછા;
	}
#endif

	જો(events & EPOLLOUT
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

	જો(events & EPOLLIN
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
	}નહીંતર{
		જો(events & (EPOLLERR | EPOLLHUP)){
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			પાછા;
		}
	}
}
#endif
