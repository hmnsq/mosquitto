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

#समावेश "config.h"

#अगर_घोषित WITH_EPOLL

#अगर_अघोषित WIN32
#  define _GNU_SOURCE
#पूर्ण_अगर

#अगर_अघोषित WIN32
#  include <sys/epoll.h>
#  define MAX_EVENTS 1000
#पूर्ण_अगर

#समावेश <errno.h>
#समावेश <signal.h>

#समावेश "mosquitto_broker_internal.h"
#समावेश "mux.h"
#समावेश "packet_mosq.h"
#समावेश "util_mosq.h"

अटल व्योम loop_handle_reads_writes(काष्ठा mosquitto *context, uint32_t events);

अटल काष्ठा epoll_event ep_events[MAX_EVENTS];


पूर्णांक mux_epoll__init(व्योम)
{
	स_रखो(&ep_events, 0, माप(काष्ठा epoll_event)*MAX_EVENTS);

	db.epollfd = 0;
	अगर((db.epollfd = epoll_create(MAX_EVENTS)) == -1){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error in epoll creating: %s", म_त्रुटि(त्रुटि_सं));
		वापस MOSQ_ERR_UNKNOWN;
	}

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_epoll__add_listeners(काष्ठा mosquitto__listener_sock *listensock, पूर्णांक listensock_गिनती)
{
	क्रम(पूर्णांक i=0; i<listensock_गिनती; i++){
		काष्ठा epoll_event ev;

		स_रखो(&ev, 0, माप(काष्ठा epoll_event));
		ev.तथ्य.ptr = &listensock[i];
		ev.events = EPOLLIN;
		अगर(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, listensock[i].sock, &ev) == -1){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error in epoll initial registering: %s", म_त्रुटि(त्रुटि_सं));
			वापस MOSQ_ERR_UNKNOWN;
		}
	}

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_epoll__delete_listeners(काष्ठा mosquitto__listener_sock *listensock, पूर्णांक listensock_गिनती)
{
	क्रम(पूर्णांक i=0; i<listensock_गिनती; i++){
		अगर(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, listensock[i].sock, शून्य) == -1){
			वापस MOSQ_ERR_UNKNOWN;
		}
	}

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_epoll__add_out(काष्ठा mosquitto *context)
{
	अगर(!(context->events & EPOLLOUT)){
		काष्ठा epoll_event ev;

		स_रखो(&ev, 0, माप(काष्ठा epoll_event));
		ev.तथ्य.ptr = context;
		ev.events = EPOLLIN | EPOLLOUT;
		अगर(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			अगर((त्रुटि_सं != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(शून्य, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLOUT: %s", म_त्रुटि(त्रुटि_सं));
			}
		}
		context->events = EPOLLIN | EPOLLOUT;
	}
	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_epoll__remove_out(काष्ठा mosquitto *context)
{
	अगर(context->events & EPOLLOUT){
		काष्ठा epoll_event ev;

		स_रखो(&ev, 0, माप(काष्ठा epoll_event));
		ev.तथ्य.ptr = context;
		ev.events = EPOLLIN;
		अगर(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			अगर((त्रुटि_सं != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(शून्य, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLIN: %s", म_त्रुटि(त्रुटि_सं));
			}
		}
		context->events = EPOLLIN;
	}
	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_epoll__new(काष्ठा mosquitto *context)
{
	काष्ठा epoll_event ev;

	स_रखो(&ev, 0, माप(काष्ठा epoll_event));
	ev.events = EPOLLIN;
	ev.तथ्य.ptr = context;
	अगर(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1){
		अगर(त्रुटि_सं != EEXIST){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error in epoll accepting: %s", म_त्रुटि(त्रुटि_सं));
		}
	}
	context->events = EPOLLIN;
	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_epoll__delete(काष्ठा mosquitto *context)
{
	अगर(context->sock != INVALID_SOCKET){
		अगर(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, context->sock, शून्य) == -1){
			वापस 1;
		}
	}
	वापस 0;
}


पूर्णांक mux_epoll__handle(व्योम)
{
	काष्ठा epoll_event ev;
	काष्ठा mosquitto *context;
	काष्ठा mosquitto__listener_sock *listensock;
	पूर्णांक event_गिनती;

	स_रखो(&ev, 0, माप(काष्ठा epoll_event));
#अगर defined(WITH_WEBSOCKETS)
	event_गिनती = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, 100);
#अन्यथा
	event_गिनती = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, db.अग्र_event_ms);
#पूर्ण_अगर

	db.now_s = mosquitto_time();
	db.now_real_s = समय(शून्य);

	चयन(event_गिनती){
		हाल -1:
			अगर(त्रुटि_सं != EINTR){
				log__printf(शून्य, MOSQ_LOG_ERR, "Error in epoll waiting: %s.", म_त्रुटि(त्रुटि_सं));
			}
			अवरोध;
		हाल 0:
			अवरोध;
		शेष:
			क्रम(पूर्णांक i=0; i<event_गिनती; i++){
				context = ep_events[i].तथ्य.ptr;
				अगर(context->ident == id_client){
					loop_handle_reads_writes(context, ep_events[i].events);
				}अन्यथा अगर(context->ident == id_listener){
					listensock = ep_events[i].तथ्य.ptr;

					अगर(ep_events[i].events & (EPOLLIN | EPOLLPRI)){
						जबतक((context = net__socket_accept(listensock)) != शून्य){
						}
					}
#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				}अन्यथा अगर(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#पूर्ण_अगर
				}
			}
	}
	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_epoll__cleanup(व्योम)
{
	(व्योम)बंद(db.epollfd);
	db.epollfd = 0;
	वापस MOSQ_ERR_SUCCESS;
}


अटल व्योम loop_handle_reads_writes(काष्ठा mosquitto *context, uint32_t events)
{
	पूर्णांक err;
	socklen_t len;
	पूर्णांक rc;

	अगर(context->sock == INVALID_SOCKET){
		वापस;
	}

#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	अगर(context->wsi){
		काष्ठा lws_pollfd wspoll;
		wspoll.fd = context->sock;
		wspoll.events = (int16_t)context->events;
		wspoll.revents = (int16_t)events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		वापस;
	}
#पूर्ण_अगर

	अगर(events & EPOLLOUT
#अगर_घोषित WITH_TLS
			|| context->want_write
			|| (context->ssl && context->state == mosq_cs_new)
#पूर्ण_अगर
			){

		अगर(context->state == mosq_cs_connect_pending){
			len = माप(पूर्णांक);
			अगर(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (अक्षर *)&err, &len)){
				अगर(err == 0){
					mosquitto__set_state(context, mosq_cs_new);
#अगर defined(WITH_ADNS) && defined(WITH_BRIDGE)
					अगर(context->bridge){
						bridge__connect_step3(context);
					}
#पूर्ण_अगर
				}
			}अन्यथा{
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				वापस;
			}
		}
		rc = packet__write(context);
		अगर(rc){
			do_disconnect(context, rc);
			वापस;
		}
	}

	अगर(events & EPOLLIN
#अगर_घोषित WITH_TLS
			|| (context->ssl && context->state == mosq_cs_new)
#पूर्ण_अगर
			){

		करो{
			चयन(context->transport){
				हाल mosq_t_tcp:
				हाल mosq_t_ws:
					rc = packet__read(context);
					अवरोध;
#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
				हाल mosq_t_http:
					rc = http__read(context);
					अवरोध;
#पूर्ण_अगर
#अगर !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
				/* Not supported with LWS */
				हाल mosq_t_proxy_v2:
					rc = proxy_v2__read(context);
					अवरोध;
				हाल mosq_t_proxy_v1:
					rc = proxy_v1__read(context);
					अवरोध;
#पूर्ण_अगर
				शेष:
					rc = MOSQ_ERR_INVAL;
					अवरोध;
			}
			अगर(rc){
				do_disconnect(context, rc);
				वापस;
			}
		}जबतक(SSL_तथ्य_PENDING(context));
	}अन्यथा{
		अगर(events & (EPOLLERR | EPOLLHUP)){
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			वापस;
		}
	}
}
#पूर्ण_अगर
