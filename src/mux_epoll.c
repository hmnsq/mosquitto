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


पूर्णांक mux_epoll__init(शून्य)
{
	स_रखो(&ep_events, 0, आकार(संरचना epoll_event)*MAX_EVENTS);

	db.epollfd = 0;
	जर((db.epollfd = epoll_create(MAX_EVENTS)) == -1){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error in epoll creating: %s", म_त्रुटि(त्रुटि_सं));
		परत MOSQ_ERR_UNKNOWN;
	}

	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_epoll__add_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णांक listensock_मोजणी)
{
	साठी(पूर्णांक i=0; i<listensock_मोजणी; i++){
		संरचना epoll_event ev;

		स_रखो(&ev, 0, आकार(संरचना epoll_event));
		ev.data.ptr = &listensock[i];
		ev.events = EPOLLIN;
		जर(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, listensock[i].sock, &ev) == -1){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error in epoll initial registering: %s", म_त्रुटि(त्रुटि_सं));
			परत MOSQ_ERR_UNKNOWN;
		}
	}

	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_epoll__delete_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णांक listensock_मोजणी)
{
	साठी(पूर्णांक i=0; i<listensock_मोजणी; i++){
		जर(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, listensock[i].sock, रिक्त) == -1){
			परत MOSQ_ERR_UNKNOWN;
		}
	}

	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_epoll__add_out(संरचना mosquitto *context)
{
	जर(!(context->events & EPOLLOUT)){
		संरचना epoll_event ev;

		स_रखो(&ev, 0, आकार(संरचना epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN | EPOLLOUT;
		जर(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			जर((त्रुटि_सं != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(रिक्त, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLOUT: %s", म_त्रुटि(त्रुटि_सं));
			}
		}
		context->events = EPOLLIN | EPOLLOUT;
	}
	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_epoll__remove_out(संरचना mosquitto *context)
{
	जर(context->events & EPOLLOUT){
		संरचना epoll_event ev;

		स_रखो(&ev, 0, आकार(संरचना epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN;
		जर(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			जर((त्रुटि_सं != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(रिक्त, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLIN: %s", म_त्रुटि(त्रुटि_सं));
			}
		}
		context->events = EPOLLIN;
	}
	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_epoll__new(संरचना mosquitto *context)
{
	संरचना epoll_event ev;

	स_रखो(&ev, 0, आकार(संरचना epoll_event));
	ev.events = EPOLLIN;
	ev.data.ptr = context;
	जर(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1){
		जर(त्रुटि_सं != EEXIST){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error in epoll accepting: %s", म_त्रुटि(त्रुटि_सं));
		}
	}
	context->events = EPOLLIN;
	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_epoll__delete(संरचना mosquitto *context)
{
	जर(context->sock != INVALID_SOCKET){
		जर(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, context->sock, रिक्त) == -1){
			परत 1;
		}
	}
	परत 0;
}


पूर्णांक mux_epoll__handle(शून्य)
{
	संरचना epoll_event ev;
	संरचना mosquitto *context;
	संरचना mosquitto__listener_sock *listensock;
	पूर्णांक event_मोजणी;

	स_रखो(&ev, 0, आकार(संरचना epoll_event));
#जर defined(WITH_WEBSOCKETS)
	event_मोजणी = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, 100);
#अन्यथा
	event_मोजणी = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, db.next_event_ms);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = समय(रिक्त);

	निवड(event_मोजणी){
		स्थिती -1:
			जर(त्रुटि_सं != EINTR){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error in epoll waiting: %s.", म_त्रुटि(त्रुटि_सं));
			}
			थांबा;
		स्थिती 0:
			थांबा;
		पूर्वनिर्धारित:
			साठी(पूर्णांक i=0; i<event_मोजणी; i++){
				context = ep_events[i].data.ptr;
				जर(context->ident == id_client){
					loop_handle_reads_writes(context, ep_events[i].events);
				}अन्यथा जर(context->ident == id_listener){
					listensock = ep_events[i].data.ptr;

					जर(ep_events[i].events & (EPOLLIN | EPOLLPRI)){
						पर्यंत((context = net__socket_accept(listensock)) != रिक्त){
						}
					}
#जर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				}अन्यथा जर(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#endif
				}
			}
	}
	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_epoll__cleanup(शून्य)
{
	(शून्य)बंद(db.epollfd);
	db.epollfd = 0;
	परत MOSQ_ERR_SUCCESS;
}


स्थिर शून्य loop_handle_reads_writes(संरचना mosquitto *context, uint32_t events)
{
	पूर्णांक err;
	socklen_t len;
	पूर्णांक rc;

	जर(context->sock == INVALID_SOCKET){
		परत;
	}

#जर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	जर(context->wsi){
		संरचना lws_pollfd wspoll;
		wspoll.fd = context->sock;
		wspoll.events = (int16_t)context->events;
		wspoll.revents = (int16_t)events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		परत;
	}
#endif

	जर(events & EPOLLOUT
#ifdef WITH_TLS
			|| context->want_write
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		जर(context->state == mosq_cs_connect_pending){
			len = आकार(पूर्णांक);
			जर(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (अक्षर *)&err, &len)){
				जर(err == 0){
					mosquitto__set_state(context, mosq_cs_new);
#जर defined(WITH_ADNS) && defined(WITH_BRIDGE)
					जर(context->bridge){
						bridge__connect_step3(context);
					}
#endif
				}
			}अन्यथा{
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				परत;
			}
		}
		rc = packet__write(context);
		जर(rc){
			do_disconnect(context, rc);
			परत;
		}
	}

	जर(events & EPOLLIN
#ifdef WITH_TLS
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		करा{
			निवड(context->transport){
				स्थिती mosq_t_tcp:
				स्थिती mosq_t_ws:
					rc = packet__read(context);
					थांबा;
#जर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
				स्थिती mosq_t_http:
					rc = http__read(context);
					थांबा;
#endif
#जर !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
				/* Not supported with LWS */
				स्थिती mosq_t_proxy_v2:
					rc = proxy_v2__read(context);
					थांबा;
				स्थिती mosq_t_proxy_v1:
					rc = proxy_v1__read(context);
					थांबा;
#endif
				पूर्वनिर्धारित:
					rc = MOSQ_ERR_INVAL;
					थांबा;
			}
			जर(rc){
				do_disconnect(context, rc);
				परत;
			}
		}पर्यंत(SSL_DATA_PENDING(context));
	}अन्यथा{
		जर(events & (EPOLLERR | EPOLLHUP)){
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			परत;
		}
	}
}
#endif
