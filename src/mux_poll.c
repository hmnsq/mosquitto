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
*/

#अन्तर्भाव "config.h"

#यदि !defined(WITH_EPOLL) && !defined(WITH_KQUEUE)

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#अन्तर्भाव <assert.h>
#ifndef WIN32
#अन्तर्भाव <poll.h>
#अन्तर्भाव <unistd.h>
#अन्यथा
#अन्तर्भाव <process.h>
#अन्तर्भाव <winsock2.h>
#अन्तर्भाव <ws2tcpip.h>
#endif

#अन्तर्भाव <errno.h>
#अन्तर्भाव <signal.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>
#ifndef WIN32
#  include <sys/socket.h>
#endif
#अन्तर्भाव <time.h>

#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "packet_mosq.h"
#अन्तर्भाव "send_mosq.h"
#अन्तर्भाव "sys_tree.h"
#अन्तर्भाव "util_mosq.h"
#अन्तर्भाव "mux.h"

स्थिर शून्य loop_handle_reads_writes(शून्य);

स्थिर संरचना pollfd *pollfds = रिक्त;
स्थिर माप_प्रकार pollfd_max, pollfd_current_max = 0;


पूर्णाङ्क mux_poll__init(शून्य)
{
#ifdef WIN32
	pollfd_max = (माप_प्रकार)_getmaxstdio();
#अन्यथा
	pollfd_max = (माप_प्रकार)sysconf(_SC_OPEN_MAX);
#endif

	pollfds = mosquitto_calloc(pollfd_max, परिमाण(संरचना pollfd));
	यदि(!pollfds){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		फर्काऊ MOSQ_ERR_NOMEM;
	}
	स_रखो(pollfds, 0, परिमाण(संरचना pollfd)*pollfd_max);
	लागि(माप_प्रकार i=0; i<pollfd_max; i++){
		pollfds[i].fd = INVALID_SOCKET;
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_poll__add_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गन्ती)
{
	माप_प्रकार pollfd_index = 0;

	लागि(माप_प्रकार i=0; i<(माप_प्रकार )listensock_गन्ती; i++){
		pollfds[pollfd_index].fd = listensock[i].sock;
		pollfds[pollfd_index].events = POLLIN;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	यदि(pollfd_current_max == 0){
		pollfd_current_max = pollfd_index-1;
	}
	फर्काऊ MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_poll__delete_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गन्ती)
{
	माप_प्रकार pollfd_index = 0;

	UNUSED(listensock);

	लागि(माप_प्रकार i=0; i<(माप_प्रकार )listensock_गन्ती; i++){
		pollfds[pollfd_index].fd = INVALID_SOCKET;
		pollfds[pollfd_index].events = 0;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क mux_poll__add(संरचना mosquitto *context, uint16_t evt)
{
	यदि(context->events == evt){
		फर्काऊ MOSQ_ERR_SUCCESS;
	}

	यदि(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = context->sock;
		pollfds[context->pollfd_index].events = (लघु पूर्णाङ्क)evt;
		pollfds[context->pollfd_index].revents = 0;
	}अन्यथा{
		लागि(माप_प्रकार i=0; i<pollfd_max; i++){
			यदि(pollfds[i].fd == INVALID_SOCKET){
				pollfds[i].fd = context->sock;
				pollfds[i].events = POLLIN;
				pollfds[i].revents = 0;
				context->pollfd_index = (पूर्णाङ्क)i;
				यदि(i > pollfd_current_max){
					pollfd_current_max = i;
				}
				रोक;
			}
		}
	}
	context->events = evt;

	फर्काऊ MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_poll__add_out(संरचना mosquitto *context)
{
	फर्काऊ mux_poll__add(context, POLLIN | POLLOUT);
}


पूर्णाङ्क mux_poll__remove_out(संरचना mosquitto *context)
{
	यदि(context->events & POLLOUT){
		फर्काऊ mux_poll__new(context);
	}अन्यथा{
		फर्काऊ MOSQ_ERR_SUCCESS;
	}
}


पूर्णाङ्क mux_poll__new(संरचना mosquitto *context)
{
	फर्काऊ mux_poll__add(context, POLLIN);
}


पूर्णाङ्क mux_poll__delete(संरचना mosquitto *context)
{
	माप_प्रकार pollfd_index;

	यदि(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = INVALID_SOCKET;
		pollfds[context->pollfd_index].events = 0;
		pollfds[context->pollfd_index].revents = 0;
		pollfd_index = (माप_प्रकार )context->pollfd_index;
		context->pollfd_index = -1;

		/* If this is the highest index, reduce the current max until we find
		 * the next highest in use index. */
		जबसम्म(pollfd_index == pollfd_current_max
				&& pollfd_index > 0
				&& pollfds[pollfd_index].fd == INVALID_SOCKET){

			pollfd_index--;
			pollfd_current_max--;
		}
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_poll__handle(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गन्ती)
{
	संरचना mosquitto *context;
	पूर्णाङ्क fdcount;
	पूर्णाङ्क timeout;

#ifdef WITH_WEBSOCKETS
	timeout = 100;
#अन्यथा
	timeout = db.next_event_ms;
#endif

#ifndef WIN32
	fdcount = poll(pollfds, pollfd_current_max+1, timeout);
#अन्यथा
	fdcount = WSAPoll(pollfds, pollfd_current_max+1, timeout);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = समय(रिक्त);

	यदि(fdcount == -1){
#  यदि defined(WIN32) && defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		यदि(WSAGetLastError() == WSAEINVAL){
			/* WSAPoll() immediately returns an error if it is not given
			 * any sockets to wait on. This can happen if we only have
			 * websockets listeners. Sleep a little to prevent a busy loop.
			 */
			Sleep(10);
		}अन्यथा
#  endif
		{
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error in poll: %s.", म_त्रुटि(त्रुटि_सं));
		}
	}अन्यथा{
		loop_handle_reads_writes();

		लागि(पूर्णाङ्क i=0; i<listensock_गन्ती; i++){
			यदि(pollfds[i].revents & POLLIN){
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				यदि(listensock[i].listener->ws_context){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
				}अन्यथा
#endif
				{
					जबसम्म((context = net__socket_accept(&listensock[i])) != रिक्त){
					}
				}
			}
		}
	}
	फर्काऊ MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_poll__cleanup(शून्य)
{
	mosquitto_FREE(pollfds);

	फर्काऊ MOSQ_ERR_SUCCESS;
}


स्थिर शून्य loop_handle_reads_writes(शून्य)
{
	संरचना mosquitto *context, *ctxt_tmp;
	पूर्णाङ्क err;
	socklen_t len;
	पूर्णाङ्क rc;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		यदि(context->pollfd_index < 0){
			जारी;
		}

		यदि(pollfds[context->pollfd_index].fd == INVALID_SOCKET){
			जारी;
		}

		प्रतिज्ञा(pollfds[context->pollfd_index].fd == context->sock);

#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		यदि(context->wsi){
			संरचना lws_pollfd wspoll;
			wspoll.fd = pollfds[context->pollfd_index].fd;
			wspoll.events = pollfds[context->pollfd_index].events;
			wspoll.revents = pollfds[context->pollfd_index].revents;
			lws_service_fd(lws_get_context(context->wsi), &wspoll);
			जारी;
		}
#endif

#ifdef WITH_TLS
		यदि(pollfds[context->pollfd_index].revents & POLLOUT ||
				context->want_write ||
				(context->ssl && context->state == mosq_cs_new)){
#अन्यथा
		यदि(pollfds[context->pollfd_index].revents & POLLOUT){
#endif
			यदि(context->state == mosq_cs_connect_pending){
				len = परिमाण(पूर्णाङ्क);
				यदि(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (वर्ण *)&err, &len)){
					यदि(err == 0){
						mosquitto__set_state(context, mosq_cs_new);
#यदि defined(WITH_ADNS) && defined(WITH_BRIDGE)
						यदि(context->bridge){
							bridge__connect_step3(context);
							जारी;
						}
#endif
					}
				}अन्यथा{
					do_disconnect(context, MOSQ_ERR_CONN_LOST);
					जारी;
				}
			}
			rc = packet__write(context);
			यदि(rc){
				do_disconnect(context, rc);
				जारी;
			}
		}
	}

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		यदि(context->pollfd_index < 0){
			जारी;
		}
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		यदि(context->wsi){
			// Websocket are already handled above
			जारी;
		}
#endif

#ifdef WITH_TLS
		यदि(pollfds[context->pollfd_index].revents & POLLIN ||
				(context->ssl && context->state == mosq_cs_new)){
#अन्यथा
		यदि(pollfds[context->pollfd_index].revents & POLLIN){
#endif
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
					जारी;
				}
			}जबसम्म(SSL_DATA_PENDING(context));
		}अन्यथा{
			यदि(context->pollfd_index >= 0 && pollfds[context->pollfd_index].revents & (POLLERR | POLLNVAL | POLLHUP)){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				जारी;
			}
		}
	}
}


#endif
