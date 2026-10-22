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

स्थिर शून्य loop_handle_reads_writes(संरचना mosquitto *context, short events);

स्थिर संरचना kevent event_list[MAX_EVENTS];


पूर्णाङ्क mux_kqueue__init(शून्य)
{
	memset(&event_list, 0, परिमाण(संरचना kevent)*MAX_EVENTS);

	db.kqueuefd = 0;
	जइ((db.kqueuefd = kqueue()) == -1){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error in kqueue creating: %s", strerror(errno));
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_kqueue__add_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्)
{
	संरचना kevent ev;

	memset(&ev, 0, परिमाण(संरचना kevent));

	प्रति(पूर्णाङ्क i=0; i<listensock_गणनम्; i++){
		EV_SET(&ev, listensock[i].sock, EVFILT_READ, EV_ADD, 0, 0, &listensock[i]);
		जइ(kevent(db.kqueuefd, &ev, 1, रिक्त, 0, रिक्त) == -1){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error in kqueue initial registering: %s", strerror(errno));
			प्रत्यागम MOSQ_ERR_UNKNOWN;
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_kqueue__delete_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्)
{
	संरचना kevent ev;

	प्रति(पूर्णाङ्क i=0; i<listensock_गणनम्; i++){
		EV_SET(&ev, listensock[i].sock, EVFILT_READ, EV_DELETE, 0, 0, &listensock[i]);
		जइ(kevent(db.kqueuefd, &ev, 1, रिक्त, 0, रिक्त) == -1){
			प्रत्यागम MOSQ_ERR_UNKNOWN;
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_kqueue__loop_setup(शून्य)
{
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_kqueue__add_out(संरचना mosquitto *context)
{
	संरचना kevent ev;

	जइ(context->events != EVFILT_WRITE){
		EV_SET(&ev, context->sock, EVFILT_WRITE, EV_ADD, 0, 0, context);
		जइ(kevent(db.kqueuefd, &ev, 1, रिक्त, 0, रिक्त) == -1){
			log__printf(रिक्त, MOSQ_LOG_DEBUG, "Error in kqueue re-registering to EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = EVFILT_WRITE;
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_kqueue__remove_out(संरचना mosquitto *context)
{
	संरचना kevent ev;

	जइ(context->events == EVFILT_WRITE){
		EV_SET(&ev, context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		जइ(kevent(db.kqueuefd, &ev, 1, रिक्त, 0, रिक्त) == -1){
			log__printf(रिक्त, MOSQ_LOG_DEBUG, "Error in kqueue removing EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = 0;
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_kqueue__new(संरचना mosquitto *context)
{
	संरचना kevent ev;

	EV_SET(&ev, context->sock, EVFILT_READ, EV_ADD, 0, 0, context);
	जइ(kevent(db.kqueuefd, &ev, 1, रिक्त, 0, रिक्त) == -1){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error in kqueue accepting: %s", strerror(errno));
	}

	context->events = 0;
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_kqueue__delete(संरचना mosquitto *context)
{
	संरचना kevent ev[2];

	जइ(context->sock != INVALID_SOCKET){
		EV_SET(&ev[0], context->sock, EVFILT_READ, EV_DELETE, 0, 0, context);
		EV_SET(&ev[1], context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		जइ(kevent(db.kqueuefd, ev, 2, रिक्त, 0, रिक्त) == -1){
			प्रत्यागम 1;
		}
	}
	प्रत्यागम 0;
}


पूर्णाङ्क mux_kqueue__handle(शून्य)
{
	संरचना mosquitto *context;
	संरचना mosquitto__listener_sock *listensock;
	पूर्णाङ्क event_गणनम्;
	संरचना timespec timeout;

#ifdef WITH_WEBSOCKETS
	timeout.tv_sec = 0;
	timeout.tv_nsec = 100000000; /* 100 ms */
#अण्णहा
	timeout.tv_sec = db.next_event_ms/1000;
	timeout.tv_nsec = (db.next_event_ms - timeout.tv_sec*1000) * 1000000;
#endif

	event_गणनम् = kevent(db.kqueuefd,
			रिक्त, 0,
			event_list, MAX_EVENTS,
			&timeout);

	db.now_s = mosquitto_time();
	db.now_real_s = time(रिक्त);

	विकल्प(event_गणनम्){
		पक्ष -1:
			जइ(errno != EINTR){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error in kqueue waiting: %s.", strerror(errno));
			}
			भङ्ग;
		पक्ष 0:
			भङ्ग;
		स्वभाव:
			प्रति(पूर्णाङ्क i=0; i<event_गणनम्; i++){
				context = event_list[i].udata;
				जइ(context->ident == id_client){
					loop_handle_reads_writes(context, event_list[i].filter);
					जइ(event_list[i].flags & (EV_EOF | EV_ERROR)){
						do_disconnect(context, MOSQ_ERR_CONN_LOST);
					}
				}अण्णहा जइ(context->ident == id_listener){
					listensock = event_list[i].udata;

					जइ(event_list[i].filter == EVFILT_READ){
						जाव((context = net__socket_accept(listensock)) != रिक्त){
						}
					}
#ifdef WITH_WEBSOCKETS
				}अण्णहा जइ(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#endif
				}
			}
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_kqueue__cleanup(शून्य)
{
	(शून्य)close(db.kqueuefd);
	db.kqueuefd = 0;
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर शून्य loop_handle_reads_writes(संरचना mosquitto *context, short event)
{
	पूर्णाङ्क err;
	socklen_t len;
	पूर्णाङ्क rc;

	जइ(context->sock == INVALID_SOCKET){
		प्रत्यागम;
	}

#जइ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	जइ(context->wsi){
		संरचना lws_pollfd wspoll;
		wspoll.fd = context->sock;
		int16_t lws_events;
		विकल्प(event){
			पक्ष EVFILT_READ:
				lws_events = LWS_POLLIN;
				भङ्ग;
			पक्ष EVFILT_WRITE:
				lws_events = LWS_POLLOUT;
				भङ्ग;
			स्वभाव:
				lws_events = LWS_POLLHUP;
				भङ्ग;
		}
		wspoll.events = lws_events;
		wspoll.revents = lws_events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		प्रत्यागम;
	}
#endif

	जइ(event == EVFILT_WRITE
#ifdef WITH_TLS
			|| context->want_write
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		जइ(context->state == mosq_cs_connect_pending){
			len = परिमाण(पूर्णाङ्क);
			जइ(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (वर्ण *)&err, &len)){
				जइ(err == 0){
					mosquitto__set_state(context, mosq_cs_new);
#जइ defined(WITH_ADNS) && defined(WITH_BRIDGE)
					जइ(context->bridge){
						bridge__connect_step3(context);
					}
#endif
				}
			}अण्णहा{
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				प्रत्यागम;
			}
		}
		rc = packet__write(context);
		जइ(rc){
			do_disconnect(context, rc);
			प्रत्यागम;
		}
	}

	जइ(event == EVFILT_READ
#ifdef WITH_TLS
			|| (context->ssl && context->state == mosq_cs_new)
#endif
			){

		करेहि{
			विकल्प(context->transport){
				पक्ष mosq_t_tcp:
				पक्ष mosq_t_ws:
					rc = packet__read(context);
					भङ्ग;
#जइ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
				पक्ष mosq_t_http:
					rc = http__read(context);
					भङ्ग;
#endif
#जइ !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
				/* Not supported with LWS */
				पक्ष mosq_t_proxy_v2:
					rc = proxy_v2__read(context);
					भङ्ग;
				पक्ष mosq_t_proxy_v1:
					rc = proxy_v1__read(context);
					भङ्ग;
#endif
				स्वभाव:
					rc = MOSQ_ERR_INVAL;
					भङ्ग;
			}
			जइ(rc){
				do_disconnect(context, rc);
				प्रत्यागम;
			}
		}जाव(SSL_DATA_PENDING(context));
	}
}
#endif
