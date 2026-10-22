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

#அந்தர்பாவ "config.h"

#ifdef WITH_KQUEUE

#define MAX_EVENTS 1000

#அந்தர்பாவ <errno.h>
#அந்தர்பாவ <signal.h>
#அந்தர்பாவ <sys/event.h>
#அந்தர்பாவ <sys/socket.h>

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "packet_mosq.h"
#அந்தர்பாவ "util_mosq.h"

நிலையான வெறுமை loop_handle_reads_writes(கட்டமைப்பு mosquitto *context, short events);

நிலையான கட்டமைப்பு kevent event_list[MAX_EVENTS];


முழு mux_kqueue__init(வெறுமை)
{
	memset(&event_list, 0, அளவு(கட்டமைப்பு kevent)*MAX_EVENTS);

	db.kqueuefd = 0;
	எனில்((db.kqueuefd = kqueue()) == -1){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error in kqueue creating: %s", strerror(errno));
		திரும்பு MOSQ_ERR_UNKNOWN;
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு mux_kqueue__add_listeners(கட்டமைப்பு mosquitto__listener_sock *listensock, முழு listensock_மொத்தம்)
{
	கட்டமைப்பு kevent ev;

	memset(&ev, 0, அளவு(கட்டமைப்பு kevent));

	க்கு(முழு i=0; i<listensock_மொத்தம்; i++){
		EV_SET(&ev, listensock[i].sock, EVFILT_READ, EV_ADD, 0, 0, &listensock[i]);
		எனில்(kevent(db.kqueuefd, &ev, 1, வெற்று, 0, வெற்று) == -1){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error in kqueue initial registering: %s", strerror(errno));
			திரும்பு MOSQ_ERR_UNKNOWN;
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு mux_kqueue__delete_listeners(கட்டமைப்பு mosquitto__listener_sock *listensock, முழு listensock_மொத்தம்)
{
	கட்டமைப்பு kevent ev;

	க்கு(முழு i=0; i<listensock_மொத்தம்; i++){
		EV_SET(&ev, listensock[i].sock, EVFILT_READ, EV_DELETE, 0, 0, &listensock[i]);
		எனில்(kevent(db.kqueuefd, &ev, 1, வெற்று, 0, வெற்று) == -1){
			திரும்பு MOSQ_ERR_UNKNOWN;
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு mux_kqueue__loop_setup(வெறுமை)
{
	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு mux_kqueue__add_out(கட்டமைப்பு mosquitto *context)
{
	கட்டமைப்பு kevent ev;

	எனில்(context->events != EVFILT_WRITE){
		EV_SET(&ev, context->sock, EVFILT_WRITE, EV_ADD, 0, 0, context);
		எனில்(kevent(db.kqueuefd, &ev, 1, வெற்று, 0, வெற்று) == -1){
			log__printf(வெற்று, MOSQ_LOG_DEBUG, "Error in kqueue re-registering to EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = EVFILT_WRITE;
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு mux_kqueue__remove_out(கட்டமைப்பு mosquitto *context)
{
	கட்டமைப்பு kevent ev;

	எனில்(context->events == EVFILT_WRITE){
		EV_SET(&ev, context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		எனில்(kevent(db.kqueuefd, &ev, 1, வெற்று, 0, வெற்று) == -1){
			log__printf(வெற்று, MOSQ_LOG_DEBUG, "Error in kqueue removing EVFILT_WRITE: %s", strerror(errno));
		}

		context->events = 0;
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு mux_kqueue__new(கட்டமைப்பு mosquitto *context)
{
	கட்டமைப்பு kevent ev;

	EV_SET(&ev, context->sock, EVFILT_READ, EV_ADD, 0, 0, context);
	எனில்(kevent(db.kqueuefd, &ev, 1, வெற்று, 0, வெற்று) == -1){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error in kqueue accepting: %s", strerror(errno));
	}

	context->events = 0;
	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு mux_kqueue__delete(கட்டமைப்பு mosquitto *context)
{
	கட்டமைப்பு kevent ev[2];

	எனில்(context->sock != INVALID_SOCKET){
		EV_SET(&ev[0], context->sock, EVFILT_READ, EV_DELETE, 0, 0, context);
		EV_SET(&ev[1], context->sock, EVFILT_WRITE, EV_DELETE, 0, 0, context);
		எனில்(kevent(db.kqueuefd, ev, 2, வெற்று, 0, வெற்று) == -1){
			திரும்பு 1;
		}
	}
	திரும்பு 0;
}


முழு mux_kqueue__handle(வெறுமை)
{
	கட்டமைப்பு mosquitto *context;
	கட்டமைப்பு mosquitto__listener_sock *listensock;
	முழு event_மொத்தம்;
	கட்டமைப்பு timespec timeout;

#ifdef WITH_WEBSOCKETS
	timeout.tv_sec = 0;
	timeout.tv_nsec = 100000000; /* 100 ms */
#இல்லையெனில்
	timeout.tv_sec = db.next_event_ms/1000;
	timeout.tv_nsec = (db.next_event_ms - timeout.tv_sec*1000) * 1000000;
#endif

	event_மொத்தம் = kevent(db.kqueuefd,
			வெற்று, 0,
			event_list, MAX_EVENTS,
			&timeout);

	db.now_s = mosquitto_time();
	db.now_real_s = time(வெற்று);

	தேர்வு(event_மொத்தம்){
		நிலை -1:
			எனில்(errno != EINTR){
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error in kqueue waiting: %s.", strerror(errno));
			}
			நிறுத்து;
		நிலை 0:
			நிறுத்து;
		இயல்பு:
			க்கு(முழு i=0; i<event_மொத்தம்; i++){
				context = event_list[i].udata;
				எனில்(context->ident == id_client){
					loop_handle_reads_writes(context, event_list[i].filter);
					எனில்(event_list[i].flags & (EV_EOF | EV_ERROR)){
						do_disconnect(context, MOSQ_ERR_CONN_LOST);
					}
				}இல்லையெனில் எனில்(context->ident == id_listener){
					listensock = event_list[i].udata;

					எனில்(event_list[i].filter == EVFILT_READ){
						போது((context = net__socket_accept(listensock)) != வெற்று){
						}
					}
#ifdef WITH_WEBSOCKETS
				}இல்லையெனில் எனில்(context->ident == id_listener_ws){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
#endif
				}
			}
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு mux_kqueue__cleanup(வெறுமை)
{
	(வெறுமை)close(db.kqueuefd);
	db.kqueuefd = 0;
	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான வெறுமை loop_handle_reads_writes(கட்டமைப்பு mosquitto *context, short event)
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
		int16_t lws_events;
		தேர்வு(event){
			நிலை EVFILT_READ:
				lws_events = LWS_POLLIN;
				நிறுத்து;
			நிலை EVFILT_WRITE:
				lws_events = LWS_POLLOUT;
				நிறுத்து;
			இயல்பு:
				lws_events = LWS_POLLHUP;
				நிறுத்து;
		}
		wspoll.events = lws_events;
		wspoll.revents = lws_events;
		lws_service_fd(lws_get_context(context->wsi), &wspoll);
		திரும்பு;
	}
#endif

	எனில்(event == EVFILT_WRITE
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

	எனில்(event == EVFILT_READ
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
	}
}
#endif
