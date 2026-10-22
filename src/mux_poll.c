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

#जर !defined(WITH_EPOLL) && !defined(WITH_KQUEUE)

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

#जर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
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


पूर्णांक mux_poll__init(शून्य)
{
#ifdef WIN32
	pollfd_max = (माप_प्रकार)_getmaxstdio();
#अन्यथा
	pollfd_max = (माप_प्रकार)sysconf(_SC_OPEN_MAX);
#endif

	pollfds = mosquitto_calloc(pollfd_max, आकार(संरचना pollfd));
	जर(!pollfds){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		परत MOSQ_ERR_NOMEM;
	}
	स_रखो(pollfds, 0, आकार(संरचना pollfd)*pollfd_max);
	साठी(माप_प्रकार i=0; i<pollfd_max; i++){
		pollfds[i].fd = INVALID_SOCKET;
	}

	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_poll__add_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णांक listensock_मोजणी)
{
	माप_प्रकार pollfd_index = 0;

	साठी(माप_प्रकार i=0; i<(माप_प्रकार )listensock_मोजणी; i++){
		pollfds[pollfd_index].fd = listensock[i].sock;
		pollfds[pollfd_index].events = POLLIN;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	जर(pollfd_current_max == 0){
		pollfd_current_max = pollfd_index-1;
	}
	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_poll__delete_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णांक listensock_मोजणी)
{
	माप_प्रकार pollfd_index = 0;

	UNUSED(listensock);

	साठी(माप_प्रकार i=0; i<(माप_प्रकार )listensock_मोजणी; i++){
		pollfds[pollfd_index].fd = INVALID_SOCKET;
		pollfds[pollfd_index].events = 0;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	परत MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णांक mux_poll__add(संरचना mosquitto *context, uint16_t evt)
{
	जर(context->events == evt){
		परत MOSQ_ERR_SUCCESS;
	}

	जर(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = context->sock;
		pollfds[context->pollfd_index].events = (लघु पूर्णांक)evt;
		pollfds[context->pollfd_index].revents = 0;
	}अन्यथा{
		साठी(माप_प्रकार i=0; i<pollfd_max; i++){
			जर(pollfds[i].fd == INVALID_SOCKET){
				pollfds[i].fd = context->sock;
				pollfds[i].events = POLLIN;
				pollfds[i].revents = 0;
				context->pollfd_index = (पूर्णांक)i;
				जर(i > pollfd_current_max){
					pollfd_current_max = i;
				}
				थांबा;
			}
		}
	}
	context->events = evt;

	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_poll__add_out(संरचना mosquitto *context)
{
	परत mux_poll__add(context, POLLIN | POLLOUT);
}


पूर्णांक mux_poll__remove_out(संरचना mosquitto *context)
{
	जर(context->events & POLLOUT){
		परत mux_poll__new(context);
	}अन्यथा{
		परत MOSQ_ERR_SUCCESS;
	}
}


पूर्णांक mux_poll__new(संरचना mosquitto *context)
{
	परत mux_poll__add(context, POLLIN);
}


पूर्णांक mux_poll__delete(संरचना mosquitto *context)
{
	माप_प्रकार pollfd_index;

	जर(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = INVALID_SOCKET;
		pollfds[context->pollfd_index].events = 0;
		pollfds[context->pollfd_index].revents = 0;
		pollfd_index = (माप_प्रकार )context->pollfd_index;
		context->pollfd_index = -1;

		/* If this is the highest index, reduce the current max until we find
		 * the next highest in use index. */
		पर्यंत(pollfd_index == pollfd_current_max
				&& pollfd_index > 0
				&& pollfds[pollfd_index].fd == INVALID_SOCKET){

			pollfd_index--;
			pollfd_current_max--;
		}
	}

	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_poll__handle(संरचना mosquitto__listener_sock *listensock, पूर्णांक listensock_मोजणी)
{
	संरचना mosquitto *context;
	पूर्णांक fdcount;
	पूर्णांक timeout;

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

	जर(fdcount == -1){
#  जर defined(WIN32) && defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		जर(WSAGetLastError() == WSAEINVAL){
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

		साठी(पूर्णांक i=0; i<listensock_मोजणी; i++){
			जर(pollfds[i].revents & POLLIN){
#जर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				जर(listensock[i].listener->ws_context){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
				}अन्यथा
#endif
				{
					पर्यंत((context = net__socket_accept(&listensock[i])) != रिक्त){
					}
				}
			}
		}
	}
	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_poll__cleanup(शून्य)
{
	mosquitto_FREE(pollfds);

	परत MOSQ_ERR_SUCCESS;
}


स्थिर शून्य loop_handle_reads_writes(शून्य)
{
	संरचना mosquitto *context, *ctxt_tmp;
	पूर्णांक err;
	socklen_t len;
	पूर्णांक rc;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		जर(context->pollfd_index < 0){
			सुरूठेवा;
		}

		जर(pollfds[context->pollfd_index].fd == INVALID_SOCKET){
			सुरूठेवा;
		}

		प्रतिज्ञा(pollfds[context->pollfd_index].fd == context->sock);

#जर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		जर(context->wsi){
			संरचना lws_pollfd wspoll;
			wspoll.fd = pollfds[context->pollfd_index].fd;
			wspoll.events = pollfds[context->pollfd_index].events;
			wspoll.revents = pollfds[context->pollfd_index].revents;
			lws_service_fd(lws_get_context(context->wsi), &wspoll);
			सुरूठेवा;
		}
#endif

#ifdef WITH_TLS
		जर(pollfds[context->pollfd_index].revents & POLLOUT ||
				context->want_write ||
				(context->ssl && context->state == mosq_cs_new)){
#अन्यथा
		जर(pollfds[context->pollfd_index].revents & POLLOUT){
#endif
			जर(context->state == mosq_cs_connect_pending){
				len = आकार(पूर्णांक);
				जर(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (अक्षर *)&err, &len)){
					जर(err == 0){
						mosquitto__set_state(context, mosq_cs_new);
#जर defined(WITH_ADNS) && defined(WITH_BRIDGE)
						जर(context->bridge){
							bridge__connect_step3(context);
							सुरूठेवा;
						}
#endif
					}
				}अन्यथा{
					do_disconnect(context, MOSQ_ERR_CONN_LOST);
					सुरूठेवा;
				}
			}
			rc = packet__write(context);
			जर(rc){
				do_disconnect(context, rc);
				सुरूठेवा;
			}
		}
	}

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		जर(context->pollfd_index < 0){
			सुरूठेवा;
		}
#जर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		जर(context->wsi){
			// Websocket are already handled above
			सुरूठेवा;
		}
#endif

#ifdef WITH_TLS
		जर(pollfds[context->pollfd_index].revents & POLLIN ||
				(context->ssl && context->state == mosq_cs_new)){
#अन्यथा
		जर(pollfds[context->pollfd_index].revents & POLLIN){
#endif
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
					सुरूठेवा;
				}
			}पर्यंत(SSL_DATA_PENDING(context));
		}अन्यथा{
			जर(context->pollfd_index >= 0 && pollfds[context->pollfd_index].revents & (POLLERR | POLLNVAL | POLLHUP)){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				सुरूठेवा;
			}
		}
	}
}


#endif
