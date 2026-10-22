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

#समावेश "config.h"

#अगर_घोषित WITH_KQUEUE

#घोषणा MAX_EVENTS 1000

#समावेश <errno.h>
#समावेश <signal.h>
#समावेश <sys/event.h>
#समावेश <sys/socket.h>

#समावेश "mosquitto_broker_internal.h"
#समावेश "packet_mosq.h"
#समावेश "util_mosq.h"

अटल व्योम loop_handle_reads_writes(काष्ठा mosquitto *context, लघु events);

अटल काष्ठा kevent event_list[MAX_EVENTS];


पूर्णांक mux_kqueue__init(व्योम)
{
	स_रखो(&event_list, 0, माप(काष्ठा kevent)*MAX_EVENTS);

	db.kqueuefd = 0;
	अगर((db.kqueuefd = kqueue()) == -1){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error in kqueue creating: %s", म_त्रुटि(त्रुटि_सं));
		वापस MOSQ_ERR_UNKNOWN;
	}

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_kqueue__add_listeners(काष्ठा mosquitto__listener_sock *listensock, पूर्णांक listensock_गिनती)
{
	काष्ठा kevent ev;

	स_रखो(&ev, 0, माप(काष्ठा kevent));

	क्रम(पूर्णांक i=0; i<listensock_गिनती; i++){
		EV_SET(&ev, listensock[i].sock, EVFILT_READ, EV_ADD, 0, 0, &listensock[i]);
		अगर(kevent(db.kqueuefd, &ev, 1, शून्य, 0, शून्य) == -1){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error in kqueue initial registering: %s", म_त्रुटि(त्रुटि_सं));
			वापस MOSQ_ERR_UNKNOWN;
		}
	}

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_kqueue__delete_listeners(काष्ठा mosquitto__listener_sock *listensock, पूर्णांक listensock_गिनती)
{
	काष्ठा kevent ev;

	क्रम(पूर्णांक i=0; i<listensock_गिनती; i++){
		EV_SET(&ev, listensock[i].sock, EVFILT_READ, EV_DELETE, 0, 0, &listensock[i]);
		अगर(kevent(db.kqueuefd, &ev, 1, शून्य, 0, शून्य) == -1){
			वापस MOSQ_ERR_UNKNOWN;
		}
	}

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_kqueue__loop_setup(व्योम)
{
	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_kqueue__add_out(काष्ठा mosquitto *context)
{
	काष्ठा kevent ev;

	अगर(context->events != EVFILT_WRITE){
		EV_SET(&ev, context->sock, EVFILT_WRITE, EV_ADD, 0, 0, context);
		अगर(kevent(db.kqueuefd, &ev, 1, शून्य, 0, शून्य) == -1){
			log__printf(शून्य, MOSQ_LOG_DEBUG, "Error in kqueue re-registering to EVFILT_WRITE: %s", म_त्रुटि(त्रुटि_सं));
		}

		context->events = EVFILT_WRITE;
	}
	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_kqueue__remove_out(काष्ठा mosquitto *context)
{
	काष्ठा kevent ev;

	अगर(context->events == EVFILT_WRITE){
		EV_SET(&ev, context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		अगर(kevent(db.kqueuefd, &ev, 1, शून्य, 0, शून्य) == -1){
			log__printf(शून्य, MOSQ_LOG_DEBUG, "Error in kqueue removing EVFILT_WRITE: %s", म_त्रुटि(त्रुटि_सं));
		}

		context->events = 0;
	}
	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_kqueue__new(काष्ठा mosquitto *context)
{
	काष्ठा kevent ev;

	EV_SET(&ev, context->sock, EVFILT_READ, EV_ADD, 0, 0, context);
	अगर(kevent(db.kqueuefd, &ev, 1, शून्य, 0, शून्य) == -1){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error in kqueue accepting: %s", म_त्रुटि(त्रुटि_सं));
	}

	context->events = 0;
	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_kqueue__delete(काष्ठा mosquitto *context)
{
	काष्ठा kevent ev[2];

	अगर(context->sock != INVALID_SOCKET){
		EV_SET(&ev[0], context->sock, EVFILT_READ, EV_DELETE, 0, 0, context);
		EV_SET(&ev[1], context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		अगर(kevent(db.kqueuefd, ev, 2, शून्य, 0, शून्य) == -1){
			वापस 1;
		}
	}
	वापस 0;
}


पूर्णांक mux_kqueue__handle(व्योम)
{
	काष्ठा mosquitto *context;
	काष्ठा mosquitto__listener_sock *listensock;
	पूर्णांक event_गिनती;
	काष्ठा timespec timeout;

#अगर_घोषित WITH_WEBSOCKETS
	timeout.tv_sec = 0;
	timeout.tv_nsec = 100000000; /* 100 ms */
#अन्यथा
	timeout.tv_sec = db.अग्र_event_ms/1000;
	timeout.tv_nsec = (db.अग्र_event_ms - timeout.tv_sec*1000) * 1000000;
#पूर्ण_अगर

	event_गिनती = kevent(db.kqueuefd,
			शून्य, 0,
			event_list, MAX_EVENTS,
			&timeout);

	db.now_s = mosquitto_time();
	db.now_real_s = समय(शून्य);

	चयन(event_गिनती){
		हाल -1:
			अगर(त्रुटि_सं != EINTR){
				log__printf(शून्य, MOSQ_LOG_ERR, "Error in kqueue waiting: %s.", म_त्रुटि(त्रुटि_सं));
			}
			अवरोध;
		हाल 0:
			अवरोध;
		शेष:
			क्रम(पूर्णांक i=0; i<event_गिनती; i++){
				context = event_list[i].udata;
				अगर(context->ident == id_client){
					loop_handle_reads_writes(context, event_list[i].filter);
					अगर(event_list[i].flags & (EV_EOF | EV_ERROR)){
						do_disconnect(context, MOSQ_ERR_CONN_LOST);
					}
				}अन्यथा अगर(context->ident == id_listener){
					listensock = event_list[i].udata;

					अगर(event_list[i].filter == EVFILT_READ){
						जबतक((context = net__socket_accept(listensock)) != शून्य){
						}
					}
#अगर_घोषित WITH_WEBSOCKETS
				}अन्यथा अगर(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#पूर्ण_अगर
				}
			}
	}
	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_kqueue__cleanup(व्योम)
{
	(व्योम)बंद(db.kqueuefd);
	db.kqueuefd = 0;
	वापस MOSQ_ERR_SUCCESS;
}


अटल व्योम loop_handle_reads_writes(काष्ठा mosquitto *context, लघु event)
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
		int16_t lws_events;
		चयन(event){
			हाल EVFILT_READ:
				lws_events = LWS_POLLIN;
				अवरोध;
			हाल EVFILT_WRITE:
				lws_events = LWS_POLLOUT;
				अवरोध;
			शेष:
				lws_events = LWS_POLLHUP;
				अवरोध;
		}
		wspoll.events = lws_events;
		wspoll.revents = lws_events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		वापस;
	}
#पूर्ण_अगर

	अगर(event == EVFILT_WRITE
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

	अगर(event == EVFILT_READ
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
	}
}
#पूर्ण_अगर
