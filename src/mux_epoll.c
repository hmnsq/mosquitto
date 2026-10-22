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
	memset(&ep_events, 0, परिमाण(संरचना epoll_event)*MAX_EVENTS);

	db.epollfd = 0;
	जइ((db.epollfd = epoll_create(MAX_EVENTS)) == -1){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error in epoll creating: %s", strerror(errno));
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_epoll__add_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्)
{
	प्रति(पूर्णाङ्क i=0; i<listensock_गणनम्; i++){
		संरचना epoll_event ev;

		memset(&ev, 0, परिमाण(संरचना epoll_event));
		ev.data.ptr = &listensock[i];
		ev.events = EPOLLIN;
		जइ(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, listensock[i].sock, &ev) == -1){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error in epoll initial registering: %s", strerror(errno));
			प्रत्यागम MOSQ_ERR_UNKNOWN;
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_epoll__delete_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्)
{
	प्रति(पूर्णाङ्क i=0; i<listensock_गणनम्; i++){
		जइ(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, listensock[i].sock, रिक्त) == -1){
			प्रत्यागम MOSQ_ERR_UNKNOWN;
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_epoll__add_out(संरचना mosquitto *context)
{
	जइ(!(context->events & EPOLLOUT)){
		संरचना epoll_event ev;

		memset(&ev, 0, परिमाण(संरचना epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN | EPOLLOUT;
		जइ(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			जइ((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(रिक्त, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLOUT: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN | EPOLLOUT;
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_epoll__remove_out(संरचना mosquitto *context)
{
	जइ(context->events & EPOLLOUT){
		संरचना epoll_event ev;

		memset(&ev, 0, परिमाण(संरचना epoll_event));
		ev.data.ptr = context;
		ev.events = EPOLLIN;
		जइ(epoll_ctl(db.epollfd, EPOLL_CTL_MOD, context->sock, &ev) == -1){
			जइ((errno != ENOENT)||(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1)){
				log__printf(रिक्त, MOSQ_LOG_DEBUG, "Error in epoll re-registering to EPOLLIN: %s", strerror(errno));
			}
		}
		context->events = EPOLLIN;
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_epoll__new(संरचना mosquitto *context)
{
	संरचना epoll_event ev;

	memset(&ev, 0, परिमाण(संरचना epoll_event));
	ev.events = EPOLLIN;
	ev.data.ptr = context;
	जइ(epoll_ctl(db.epollfd, EPOLL_CTL_ADD, context->sock, &ev) == -1){
		जइ(errno != EEXIST){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error in epoll accepting: %s", strerror(errno));
		}
	}
	context->events = EPOLLIN;
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_epoll__delete(संरचना mosquitto *context)
{
	जइ(context->sock != INVALID_SOCKET){
		जइ(epoll_ctl(db.epollfd, EPOLL_CTL_DEL, context->sock, रिक्त) == -1){
			प्रत्यागम 1;
		}
	}
	प्रत्यागम 0;
}


पूर्णाङ्क mux_epoll__handle(शून्य)
{
	संरचना epoll_event ev;
	संरचना mosquitto *context;
	संरचना mosquitto__listener_sock *listensock;
	पूर्णाङ्क event_गणनम्;

	memset(&ev, 0, परिमाण(संरचना epoll_event));
#जइ defined(WITH_WEBSOCKETS)
	event_गणनम् = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, 100);
#अण्णहा
	event_गणनम् = epoll_wait(db.epollfd, ep_events, MAX_EVENTS, db.next_event_ms);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(रिक्त);

	विकल्प(event_गणनम्){
		पक्ष -1:
			जइ(errno != EINTR){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error in epoll waiting: %s.", strerror(errno));
			}
			भङ्ग;
		पक्ष 0:
			भङ्ग;
		स्वभाव:
			प्रति(पूर्णाङ्क i=0; i<event_गणनम्; i++){
				context = ep_events[i].data.ptr;
				जइ(context->ident == id_client){
					loop_handle_reads_writes(context, ep_events[i].events);
				}अण्णहा जइ(context->ident == id_listener){
					listensock = ep_events[i].data.ptr;

					जइ(ep_events[i].events & (EPOLLIN | EPOLLPRI)){
						जाव((context = net__socket_accept(listensock)) != रिक्त){
						}
					}
#जइ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				}अण्णहा जइ(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#endif
				}
			}
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_epoll__cleanup(शून्य)
{
	(शून्य)close(db.epollfd);
	db.epollfd = 0;
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर शून्य loop_handle_reads_writes(संरचना mosquitto *context, uint32_t events)
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
		wspoll.events = (int16_t)context->events;
		wspoll.revents = (int16_t)events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		प्रत्यागम;
	}
#endif

	जइ(events & EPOLLOUT
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

	जइ(events & EPOLLIN
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
	}अण्णहा{
		जइ(events & (EPOLLERR | EPOLLHUP)){
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			प्रत्यागम;
		}
	}
}
#endif
