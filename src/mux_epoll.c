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

#अन्तर्भाव "config.h"

#ifdef WITH_EPOLL

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#ifndef WIN32
#  include <sys/epoll.h>
#  define MAX_EVENTS 1000
#endif

#अन्तर्भाव <errno.h>
#अन्तर्भाव <signal.h>

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mux.h"
#अन्तर्भाव "packet_mosq.h"
#अन्तर्भाव "util_mosq.h"

स्थिर शून्य loop_handle_reads_writes(संरचना mosquitto *context, uint32_t events);

स्थिर संरचना epoll_event ep_events[MAX_EVENTS];


पूर्णाङ्क mux_epoll__init(शून्य)
{
	स_रखो(&ep_events, 0, परिमाण(संरचना epoll_event)*MAX_EVENTS);

	db.epollfd = 0;
	यदि((db.epollfd = epoll_create(MAX_EVENTS)) == -1){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error in epoll creating: %s", म_त्रुटि(त्रुटि_सं));
		फर्काऊ MOSQ_ERR_UNKNOWN;
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_epoll__add_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गन्ती)
{
	लागि(पूर्णाङ्क i=0; i<listensock_गन्ती; i++){
		संरचना epoll_event ev;

		स_रखो(&ev, 0, परिमाण(संरचना epoll_event));
		ev.data.ptr = &listensock[i];
		ev.events = EPOLLIN;
		यदि(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, listensock[i].sock, &ev) == -1){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error in epoll initial registering: %s", म_त्रुटि(त्रुटि_सं));
			फर्काऊ MOSQ_ERR_UNKNOWN;
		}
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_epoll__delete_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गन्ती)
{
	लागि(पूर्णाङ्क i=0; i<listensock_गन्ती; i++){
		यदि(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, listensock[i].sock, रिक्त) == -1){
			फर्काऊ MOSQ_ERR_UNKNOWN;
		}
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_epoll__add_out(संरचना mosquitto *context)
{
	यदि(!(context->events & EPOLLOUT)){
		संरचना epoll_event ev;

		स_रखो(&ev, 0, परिमाण(संरचना epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN | EPOLLOUT;
		यदि(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			यदि((त्रुटि_सं != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(रिक्त, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLOUT: %s", म_त्रुटि(त्रुटि_सं));
			}
		}
		context->events = EPOLLIN | EPOLLOUT;
	}
	फर्काऊ MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_epoll__remove_out(संरचना mosquitto *context)
{
	यदि(context->events & EPOLLOUT){
		संरचना epoll_event ev;

		स_रखो(&ev, 0, परिमाण(संरचना epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN;
		यदि(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			यदि((त्रुटि_सं != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(रिक्त, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLIN: %s", म_त्रुटि(त्रुटि_सं));
			}
		}
		context->events = EPOLLIN;
	}
	फर्काऊ MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_epoll__new(संरचना mosquitto *context)
{
	संरचना epoll_event ev;

	स_रखो(&ev, 0, परिमाण(संरचना epoll_event));
	ev.events = EPOLLIN;
	ev.data.ptr = context;
	यदि(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1){
		यदि(त्रुटि_सं != EEXIST){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error in epoll accepting: %s", म_त्रुटि(त्रुटि_सं));
		}
	}
	context->events = EPOLLIN;
	फर्काऊ MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_epoll__delete(संरचना mosquitto *context)
{
	यदि(context->sock != INVALID_SOCKET){
		यदि(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, context->sock, रिक्त) == -1){
			फर्काऊ 1;
		}
	}
	फर्काऊ 0;
}


पूर्णाङ्क mux_epoll__handle(शून्य)
{
	संरचना epoll_event ev;
	संरचना mosquitto *context;
	संरचना mosquitto__listener_sock *listensock;
	पूर्णाङ्क event_गन्ती;

	स_रखो(&ev, 0, परिमाण(संरचना epoll_event));
#यदि defined(WITH_WEBSOCKETS)
	event_गन्ती = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, 100);
#अन्यथा
	event_गन्ती = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, db.next_event_ms);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = समय(रिक्त);

	विकल्प(event_गन्ती){
		पक्ष -1:
			यदि(त्रुटि_सं != EINTR){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error in epoll waiting: %s.", म_त्रुटि(त्रुटि_सं));
			}
			रोक;
		पक्ष 0:
			रोक;
		स्वभाव:
			लागि(पूर्णाङ्क i=0; i<event_गन्ती; i++){
				context = ep_events[i].data.ptr;
				यदि(context->ident == id_client){
					loop_handle_reads_writes(context, ep_events[i].events);
				}अन्यथा यदि(context->ident == id_listener){
					listensock = ep_events[i].data.ptr;

					यदि(ep_events[i].events & (EPOLLIN | EPOLLPRI)){
						जबसम्म((context = net__socket_accept(listensock)) != रिक्त){
						}
					}
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				}अन्यथा यदि(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#endif
				}
			}
	}
	फर्काऊ MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_epoll__cleanup(शून्य)
{
	(शून्य)बंद(db.epollfd);
	db.epollfd = 0;
	फर्काऊ MOSQ_ERR_SUCCESS;
}


स्थिर शून्य loop_handle_reads_writes(संरचना mosquitto *context, uint32_t events)
{
	पूर्णाङ्क err;
	socklen_t len;
	पूर्णाङ्क rc;

	यदि(context->sock == INVALID_SOCKET){
		फर्काऊ;
	}

#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	यदि(context->wsi){
		संरचना lws_pollfd wspoll;
		wspoll.fd = context->sock;
		wspoll.events = (int16_t)context->events;
		wspoll.revents = (int16_t)events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		फर्काऊ;
	}
#endif

	यदि(events & EPOLLOUT
#ifdef WITH_TLS
			|| context->want_write
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		यदि(context->state == mosq_cs_connect_pending){
			len = परिमाण(पूर्णाङ्क);
			यदि(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (वर्ण *)&err, &len)){
				यदि(err == 0){
					mosquitto__set_state(context, mosq_cs_new);
#यदि defined(WITH_ADNS) && defined(WITH_BRIDGE)
					यदि(context->bridge){
						bridge__connect_step3(context);
					}
#endif
				}
			}अन्यथा{
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				फर्काऊ;
			}
		}
		rc = packet__write(context);
		यदि(rc){
			do_disconnect(context, rc);
			फर्काऊ;
		}
	}

	यदि(events & EPOLLIN
#ifdef WITH_TLS
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		गर{
			विकल्प(context->transport){
				पक्ष mosq_t_tcp:
				पक्ष mosq_t_ws:
					rc = packet__read(context);
					रोक;
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
				पक्ष mosq_t_http:
					rc = http__read(context);
					रोक;
#endif
#यदि !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
				/* Not supported with LWS */
				पक्ष mosq_t_proxy_v2:
					rc = proxy_v2__read(context);
					रोक;
				पक्ष mosq_t_proxy_v1:
					rc = proxy_v1__read(context);
					रोक;
#endif
				स्वभाव:
					rc = MOSQ_ERR_INVAL;
					रोक;
			}
			यदि(rc){
				do_disconnect(context, rc);
				फर्काऊ;
			}
		}जबसम्म(SSL_DATA_PENDING(context));
	}अन्यथा{
		यदि(events & (EPOLLERR | EPOLLHUP)){
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			फर्काऊ;
		}
	}
}
#endif
