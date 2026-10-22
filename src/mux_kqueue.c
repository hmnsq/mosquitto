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

#अन्तर्भाव "config.h"

#ifdef WITH_KQUEUE

#define MAX_EVENTS 1000

#अन्तर्भाव <errno.h>
#अन्तर्भाव <signal.h>
#अन्तर्भाव <sys/event.h>
#अन्तर्भाव <sys/socket.h>

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "packet_mosq.h"
#अन्तर्भाव "util_mosq.h"

स्थिर शून्य loop_handle_reads_writes(संरचना mosquitto *context, लघु events);

स्थिर संरचना kevent event_list[MAX_EVENTS];


पूर्णांक mux_kqueue__init(शून्य)
{
	स_रखो(&event_list, 0, आकार(संरचना kevent)*MAX_EVENTS);

	db.kqueuefd = 0;
	जर((db.kqueuefd = kqueue()) == -1){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error in kqueue creating: %s", म_त्रुटि(त्रुटि_सं));
		परत MOSQ_ERR_UNKNOWN;
	}

	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_kqueue__add_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णांक listensock_मोजणी)
{
	संरचना kevent ev;

	स_रखो(&ev, 0, आकार(संरचना kevent));

	साठी(पूर्णांक i=0; i<listensock_मोजणी; i++){
		EV_SET(&ev, listensock[i].sock, EVFILT_READ, EV_ADD, 0, 0, &listensock[i]);
		जर(kevent(db.kqueuefd, &ev, 1, रिक्त, 0, रिक्त) == -1){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error in kqueue initial registering: %s", म_त्रुटि(त्रुटि_सं));
			परत MOSQ_ERR_UNKNOWN;
		}
	}

	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_kqueue__delete_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णांक listensock_मोजणी)
{
	संरचना kevent ev;

	साठी(पूर्णांक i=0; i<listensock_मोजणी; i++){
		EV_SET(&ev, listensock[i].sock, EVFILT_READ, EV_DELETE, 0, 0, &listensock[i]);
		जर(kevent(db.kqueuefd, &ev, 1, रिक्त, 0, रिक्त) == -1){
			परत MOSQ_ERR_UNKNOWN;
		}
	}

	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_kqueue__loop_setup(शून्य)
{
	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_kqueue__add_out(संरचना mosquitto *context)
{
	संरचना kevent ev;

	जर(context->events != EVFILT_WRITE){
		EV_SET(&ev, context->sock, EVFILT_WRITE, EV_ADD, 0, 0, context);
		जर(kevent(db.kqueuefd, &ev, 1, रिक्त, 0, रिक्त) == -1){
			log__printf(रिक्त, MOSQ_LOG_DEBUG, "Error in kqueue re-registering to EVFILT_WRITE: %s", म_त्रुटि(त्रुटि_सं));
		}

		context->events = EVFILT_WRITE;
	}
	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_kqueue__remove_out(संरचना mosquitto *context)
{
	संरचना kevent ev;

	जर(context->events == EVFILT_WRITE){
		EV_SET(&ev, context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		जर(kevent(db.kqueuefd, &ev, 1, रिक्त, 0, रिक्त) == -1){
			log__printf(रिक्त, MOSQ_LOG_DEBUG, "Error in kqueue removing EVFILT_WRITE: %s", म_त्रुटि(त्रुटि_सं));
		}

		context->events = 0;
	}
	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_kqueue__new(संरचना mosquitto *context)
{
	संरचना kevent ev;

	EV_SET(&ev, context->sock, EVFILT_READ, EV_ADD, 0, 0, context);
	जर(kevent(db.kqueuefd, &ev, 1, रिक्त, 0, रिक्त) == -1){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error in kqueue accepting: %s", म_त्रुटि(त्रुटि_सं));
	}

	context->events = 0;
	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_kqueue__delete(संरचना mosquitto *context)
{
	संरचना kevent ev[2];

	जर(context->sock != INVALID_SOCKET){
		EV_SET(&ev[0], context->sock, EVFILT_READ, EV_DELETE, 0, 0, context);
		EV_SET(&ev[1], context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		जर(kevent(db.kqueuefd, ev, 2, रिक्त, 0, रिक्त) == -1){
			परत 1;
		}
	}
	परत 0;
}


पूर्णांक mux_kqueue__handle(शून्य)
{
	संरचना mosquitto *context;
	संरचना mosquitto__listener_sock *listensock;
	पूर्णांक event_मोजणी;
	संरचना timespec timeout;

#ifdef WITH_WEBSOCKETS
	timeout.tv_sec = 0;
	timeout.tv_nsec = 100000000; /* 100 ms */
#अन्यथा
	timeout.tv_sec = db.next_event_ms/1000;
	timeout.tv_nsec = (db.next_event_ms - timeout.tv_sec*1000) * 1000000;
#endif

	event_मोजणी = kevent(db.kqueuefd,
			रिक्त, 0,
			event_list, MAX_EVENTS,
			&timeout);

	db.now_s = mosquitto_time();
	db.now_real_s = समय(रिक्त);

	निवड(event_मोजणी){
		स्थिती -1:
			जर(त्रुटि_सं != EINTR){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error in kqueue waiting: %s.", म_त्रुटि(त्रुटि_सं));
			}
			थांबा;
		स्थिती 0:
			थांबा;
		पूर्वनिर्धारित:
			साठी(पूर्णांक i=0; i<event_मोजणी; i++){
				context = event_list[i].udata;
				जर(context->ident == id_client){
					loop_handle_reads_writes(context, event_list[i].filter);
					जर(event_list[i].flags & (EV_EOF | EV_ERROR)){
						do_disconnect(context, MOSQ_ERR_CONN_LOST);
					}
				}अन्यथा जर(context->ident == id_listener){
					listensock = event_list[i].udata;

					जर(event_list[i].filter == EVFILT_READ){
						पर्यंत((context = net__socket_accept(listensock)) != रिक्त){
						}
					}
#ifdef WITH_WEBSOCKETS
				}अन्यथा जर(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#endif
				}
			}
	}
	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_kqueue__cleanup(शून्य)
{
	(शून्य)बंद(db.kqueuefd);
	db.kqueuefd = 0;
	परत MOSQ_ERR_SUCCESS;
}


स्थिर शून्य loop_handle_reads_writes(संरचना mosquitto *context, लघु event)
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
		int16_t lws_events;
		निवड(event){
			स्थिती EVFILT_READ:
				lws_events = LWS_POLLIN;
				थांबा;
			स्थिती EVFILT_WRITE:
				lws_events = LWS_POLLOUT;
				थांबा;
			पूर्वनिर्धारित:
				lws_events = LWS_POLLHUP;
				थांबा;
		}
		wspoll.events = lws_events;
		wspoll.revents = lws_events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		परत;
	}
#endif

	जर(event == EVFILT_WRITE
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

	जर(event == EVFILT_READ
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
	}
}
#endif
