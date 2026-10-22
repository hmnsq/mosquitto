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

#அந்தர்பாவ "config.h"

#ifdef WITH_EPOLL

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#ifndef WIN32
#  include <sys/epoll.h>
#  define MAX_EVENTS 1000
#endif

#அந்தர்பாவ <errno.h>
#அந்தர்பாவ <signal.h>

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mux.h"
#அந்தர்பாவ "packet_mosq.h"
#அந்தர்பாவ "util_mosq.h"

நிலையான வெறுமை loop_handle_reads_writes(கட்டமைப்பு mosquitto *context, uint32_t events);

நிலையான கட்டமைப்பு epoll_event ep_events[MAX_EVENTS];


முழு mux_epoll__init(வெறுமை)
{
	memset(&ep_events, 0, அளவு(கட்டமைப்பு epoll_event)*MAX_EVENTS);

	db.epollfd = 0;
	எனில்((db.epollfd = epoll_create(MAX_EVENTS)) == -1){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error in epoll creating: %s", strerror(errno));
		திரும்பு MOSQ_ERR_UNKNOWN;
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு mux_epoll__add_listeners(கட்டமைப்பு mosquitto__listener_sock *listensock, முழு listensock_மொத்தம்)
{
	க்கு(முழு i=0; i<listensock_மொத்தம்; i++){
		கட்டமைப்பு epoll_event ev;

		memset(&ev, 0, அளவு(கட்டமைப்பு epoll_event));
		ev.data.ptr = &listensock[i];
		ev.events = EPOLLIN;
		எனில்(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, listensock[i].sock, &ev) == -1){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error in epoll initial registering: %s", strerror(errno));
			திரும்பு MOSQ_ERR_UNKNOWN;
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு mux_epoll__delete_listeners(கட்டமைப்பு mosquitto__listener_sock *listensock, முழு listensock_மொத்தம்)
{
	க்கு(முழு i=0; i<listensock_மொத்தம்; i++){
		எனில்(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, listensock[i].sock, வெற்று) == -1){
			திரும்பு MOSQ_ERR_UNKNOWN;
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு mux_epoll__add_out(கட்டமைப்பு mosquitto *context)
{
	எனில்(!(context->events & EPOLLOUT)){
		கட்டமைப்பு epoll_event ev;

		memset(&ev, 0, அளவு(கட்டமைப்பு epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN | EPOLLOUT;
		எனில்(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			எனில்((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(வெற்று, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLOUT: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN | EPOLLOUT;
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு mux_epoll__remove_out(கட்டமைப்பு mosquitto *context)
{
	எனில்(context->events & EPOLLOUT){
		கட்டமைப்பு epoll_event ev;

		memset(&ev, 0, அளவு(கட்டமைப்பு epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN;
		எனில்(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			எனில்((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(வெற்று, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLIN: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN;
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு mux_epoll__new(கட்டமைப்பு mosquitto *context)
{
	கட்டமைப்பு epoll_event ev;

	memset(&ev, 0, அளவு(கட்டமைப்பு epoll_event));
	ev.events = EPOLLIN;
	ev.data.ptr = context;
	எனில்(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1){
		எனில்(errno != EEXIST){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error in epoll accepting: %s", strerror(errno));
		}
	}
	context->events = EPOLLIN;
	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு mux_epoll__delete(கட்டமைப்பு mosquitto *context)
{
	எனில்(context->sock != INVALID_SOCKET){
		எனில்(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, context->sock, வெற்று) == -1){
			திரும்பு 1;
		}
	}
	திரும்பு 0;
}


முழு mux_epoll__handle(வெறுமை)
{
	கட்டமைப்பு epoll_event ev;
	கட்டமைப்பு mosquitto *context;
	கட்டமைப்பு mosquitto__listener_sock *listensock;
	முழு event_மொத்தம்;

	memset(&ev, 0, அளவு(கட்டமைப்பு epoll_event));
#எனில் defined(WITH_WEBSOCKETS)
	event_மொத்தம் = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, 100);
#இல்லையெனில்
	event_மொத்தம் = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, db.next_event_ms);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(வெற்று);

	தேர்வு(event_மொத்தம்){
		நிலை -1:
			எனில்(errno != EINTR){
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error in epoll waiting: %s.", strerror(errno));
			}
			நிறுத்து;
		நிலை 0:
			நிறுத்து;
		இயல்பு:
			க்கு(முழு i=0; i<event_மொத்தம்; i++){
				context = ep_events[i].data.ptr;
				எனில்(context->ident == id_client){
					loop_handle_reads_writes(context, ep_events[i].events);
				}இல்லையெனில் எனில்(context->ident == id_listener){
					listensock = ep_events[i].data.ptr;

					எனில்(ep_events[i].events & (EPOLLIN | EPOLLPRI)){
						போது((context = net__socket_accept(listensock)) != வெற்று){
						}
					}
#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				}இல்லையெனில் எனில்(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#endif
				}
			}
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு mux_epoll__cleanup(வெறுமை)
{
	(வெறுமை)close(db.epollfd);
	db.epollfd = 0;
	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான வெறுமை loop_handle_reads_writes(கட்டமைப்பு mosquitto *context, uint32_t events)
{
	முழு err;
	socklen_t len;
	முழு rc;

	எனில்(context->sock == INVALID_SOCKET){
		திரும்பு;
	}

#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	எனில்(context->wsi){
		கட்டமைப்பு lws_pollfd wspoll;
		wspoll.fd = context->sock;
		wspoll.events = (int16_t)context->events;
		wspoll.revents = (int16_t)events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		திரும்பு;
	}
#endif

	எனில்(events & EPOLLOUT
#ifdef WITH_TLS
			|| context->want_write
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		எனில்(context->state == mosq_cs_connect_pending){
			len = அளவு(முழு);
			எனில்(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (எழுத்து *)&err, &len)){
				எனில்(err == 0){
					mosquitto__set_state(context, mosq_cs_new);
#எனில் defined(WITH_ADNS) && defined(WITH_BRIDGE)
					எனில்(context->bridge){
						bridge__connect_step3(context);
					}
#endif
				}
			}இல்லையெனில்{
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				திரும்பு;
			}
		}
		rc = packet__write(context);
		எனில்(rc){
			do_disconnect(context, rc);
			திரும்பு;
		}
	}

	எனில்(events & EPOLLIN
#ifdef WITH_TLS
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		செய்{
			தேர்வு(context->transport){
				நிலை mosq_t_tcp:
				நிலை mosq_t_ws:
					rc = packet__read(context);
					நிறுத்து;
#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
				நிலை mosq_t_http:
					rc = http__read(context);
					நிறுத்து;
#endif
#எனில் !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
				/* Not supported with LWS */
				நிலை mosq_t_proxy_v2:
					rc = proxy_v2__read(context);
					நிறுத்து;
				நிலை mosq_t_proxy_v1:
					rc = proxy_v1__read(context);
					நிறுத்து;
#endif
				இயல்பு:
					rc = MOSQ_ERR_INVAL;
					நிறுத்து;
			}
			எனில்(rc){
				do_disconnect(context, rc);
				திரும்பு;
			}
		}போது(SSL_DATA_PENDING(context));
	}இல்லையெனில்{
		எனில்(events & (EPOLLERR | EPOLLHUP)){
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			திரும்பு;
		}
	}
}
#endif
