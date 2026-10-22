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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ifdef WITH_EPOLL

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#ifndef WIN32
#  include <sys/epoll.h>
#  define MAX_EVENTS 1000
#endif

#ଅନ୍ତର୍ଭାଵ <errno.h>
#ଅନ୍ତର୍ଭାଵ <signal.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mux.h"
#ଅନ୍ତର୍ଭାଵ "packet_mosq.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"

ସ୍ଥିର ଶୂନ୍ୟ loop_handle_reads_writes(ସଂରଚନା mosquitto *context, uint32_t events);

ସ୍ଥିର ସଂରଚନା epoll_event ep_events[MAX_EVENTS];


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_epoll__init(ଶୂନ୍ୟ)
{
	memset(&ep_events, 0, ଆକାର(ସଂରଚନା epoll_event)*MAX_EVENTS);

	db.epollfd = 0;
	ଯଦି((db.epollfd = epoll_create(MAX_EVENTS)) == -1){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error in epoll creating: %s", strerror(errno));
		ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_epoll__add_listeners(ସଂରଚନା mosquitto__listener_sock *listensock, ପୂର୍ଣ୍ଣସଂଖ୍ୟା listensock_ଗଣନା)
{
	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<listensock_ଗଣନା; i++){
		ସଂରଚନା epoll_event ev;

		memset(&ev, 0, ଆକାର(ସଂରଚନା epoll_event));
		ev.data.ptr = &listensock[i];
		ev.events = EPOLLIN;
		ଯଦି(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, listensock[i].sock, &ev) == -1){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error in epoll initial registering: %s", strerror(errno));
			ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_epoll__delete_listeners(ସଂରଚନା mosquitto__listener_sock *listensock, ପୂର୍ଣ୍ଣସଂଖ୍ୟା listensock_ଗଣନା)
{
	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<listensock_ଗଣନା; i++){
		ଯଦି(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, listensock[i].sock, ଖାଲି) == -1){
			ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_epoll__add_out(ସଂରଚନା mosquitto *context)
{
	ଯଦି(!(context->events & EPOLLOUT)){
		ସଂରଚନା epoll_event ev;

		memset(&ev, 0, ଆକାର(ସଂରଚନା epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN | EPOLLOUT;
		ଯଦି(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			ଯଦି((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(ଖାଲି, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLOUT: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN | EPOLLOUT;
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_epoll__remove_out(ସଂରଚନା mosquitto *context)
{
	ଯଦି(context->events & EPOLLOUT){
		ସଂରଚନା epoll_event ev;

		memset(&ev, 0, ଆକାର(ସଂରଚନା epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN;
		ଯଦି(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			ଯଦି((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(ଖାଲି, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLIN: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN;
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_epoll__new(ସଂରଚନା mosquitto *context)
{
	ସଂରଚନା epoll_event ev;

	memset(&ev, 0, ଆକାର(ସଂରଚନା epoll_event));
	ev.events = EPOLLIN;
	ev.data.ptr = context;
	ଯଦି(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1){
		ଯଦି(errno != EEXIST){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error in epoll accepting: %s", strerror(errno));
		}
	}
	context->events = EPOLLIN;
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_epoll__delete(ସଂରଚନା mosquitto *context)
{
	ଯଦି(context->sock != INVALID_SOCKET){
		ଯଦି(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, context->sock, ଖାଲି) == -1){
			ଫେରନ୍ତୁ 1;
		}
	}
	ଫେରନ୍ତୁ 0;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_epoll__handle(ଶୂନ୍ୟ)
{
	ସଂରଚନା epoll_event ev;
	ସଂରଚନା mosquitto *context;
	ସଂରଚନା mosquitto__listener_sock *listensock;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା event_ଗଣନା;

	memset(&ev, 0, ଆକାର(ସଂରଚନା epoll_event));
#ଯଦି defined(WITH_WEBSOCKETS)
	event_ଗଣନା = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, 100);
#ଅନ୍ୟଥା
	event_ଗଣନା = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, db.next_event_ms);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(ଖାଲି);

	ବାଛ(event_ଗଣନା){
		ସ୍ଥିତି -1:
			ଯଦି(errno != EINTR){
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error in epoll waiting: %s.", strerror(errno));
			}
			ଭାଙ୍ଗ;
		ସ୍ଥିତି 0:
			ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<event_ଗଣନା; i++){
				context = ep_events[i].data.ptr;
				ଯଦି(context->ident == id_client){
					loop_handle_reads_writes(context, ep_events[i].events);
				}ଅନ୍ୟଥା ଯଦି(context->ident == id_listener){
					listensock = ep_events[i].data.ptr;

					ଯଦି(ep_events[i].events & (EPOLLIN | EPOLLPRI)){
						ଯେତେବେଳେ((context = net__socket_accept(listensock)) != ଖାଲି){
						}
					}
#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				}ଅନ୍ୟଥା ଯଦି(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#endif
				}
			}
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_epoll__cleanup(ଶୂନ୍ୟ)
{
	(ଶୂନ୍ୟ)close(db.epollfd);
	db.epollfd = 0;
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ଶୂନ୍ୟ loop_handle_reads_writes(ସଂରଚନା mosquitto *context, uint32_t events)
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
		wspoll.events = (int16_t)context->events;
		wspoll.revents = (int16_t)events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		ଫେରନ୍ତୁ;
	}
#endif

	ଯଦି(events & EPOLLOUT
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

	ଯଦି(events & EPOLLIN
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
	}ଅନ୍ୟଥା{
		ଯଦି(events & (EPOLLERR | EPOLLHUP)){
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			ଫେରନ୍ତୁ;
		}
	}
}
#endif
