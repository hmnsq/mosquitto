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

#समावेश "config.h"

#अगर !defined(WITH_EPOLL) && !defined(WITH_KQUEUE)

#अगर_अघोषित WIN32
#  define _GNU_SOURCE
#पूर्ण_अगर

#समावेश <assert.h>
#अगर_अघोषित WIN32
#समावेश <poll.h>
#समावेश <unistd.h>
#अन्यथा
#समावेश <process.h>
#समावेश <winsock2.h>
#समावेश <ws2tcpip.h>
#पूर्ण_अगर

#समावेश <errno.h>
#समावेश <signal.h>
#समावेश <stdio.h>
#समावेश <string.h>
#अगर_अघोषित WIN32
#  include <sys/socket.h>
#पूर्ण_अगर
#समावेश <time.h>

#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#पूर्ण_अगर

#समावेश "mosquitto_broker_internal.h"
#समावेश "packet_mosq.h"
#समावेश "send_mosq.h"
#समावेश "sys_tree.h"
#समावेश "util_mosq.h"
#समावेश "mux.h"

अटल व्योम loop_handle_reads_writes(व्योम);

अटल काष्ठा pollfd *pollfds = शून्य;
अटल माप_प्रकार pollfd_max, pollfd_current_max = 0;


पूर्णांक mux_poll__init(व्योम)
{
#अगर_घोषित WIN32
	pollfd_max = (माप_प्रकार)_getmaxstdio();
#अन्यथा
	pollfd_max = (माप_प्रकार)sysconf(_SC_OPEN_MAX);
#पूर्ण_अगर

	pollfds = mosquitto_calloc(pollfd_max, माप(काष्ठा pollfd));
	अगर(!pollfds){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
		वापस MOSQ_ERR_NOMEM;
	}
	स_रखो(pollfds, 0, माप(काष्ठा pollfd)*pollfd_max);
	क्रम(माप_प्रकार i=0; i<pollfd_max; i++){
		pollfds[i].fd = INVALID_SOCKET;
	}

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_poll__add_listeners(काष्ठा mosquitto__listener_sock *listensock, पूर्णांक listensock_गिनती)
{
	माप_प्रकार pollfd_सूचकांक = 0;

	क्रम(माप_प्रकार i=0; i<(माप_प्रकार )listensock_गिनती; i++){
		pollfds[pollfd_सूचकांक].fd = listensock[i].sock;
		pollfds[pollfd_सूचकांक].events = POLLIN;
		pollfds[pollfd_सूचकांक].revents = 0;
		pollfd_सूचकांक++;
	}

	अगर(pollfd_current_max == 0){
		pollfd_current_max = pollfd_सूचकांक-1;
	}
	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_poll__delete_listeners(काष्ठा mosquitto__listener_sock *listensock, पूर्णांक listensock_गिनती)
{
	माप_प्रकार pollfd_सूचकांक = 0;

	UNUSED(listensock);

	क्रम(माप_प्रकार i=0; i<(माप_प्रकार )listensock_गिनती; i++){
		pollfds[pollfd_सूचकांक].fd = INVALID_SOCKET;
		pollfds[pollfd_सूचकांक].events = 0;
		pollfds[pollfd_सूचकांक].revents = 0;
		pollfd_सूचकांक++;
	}

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक mux_poll__add(काष्ठा mosquitto *context, uint16_t evt)
{
	अगर(context->events == evt){
		वापस MOSQ_ERR_SUCCESS;
	}

	अगर(context->pollfd_सूचकांक != -1){
		pollfds[context->pollfd_सूचकांक].fd = context->sock;
		pollfds[context->pollfd_सूचकांक].events = (लघु पूर्णांक)evt;
		pollfds[context->pollfd_सूचकांक].revents = 0;
	}अन्यथा{
		क्रम(माप_प्रकार i=0; i<pollfd_max; i++){
			अगर(pollfds[i].fd == INVALID_SOCKET){
				pollfds[i].fd = context->sock;
				pollfds[i].events = POLLIN;
				pollfds[i].revents = 0;
				context->pollfd_सूचकांक = (पूर्णांक)i;
				अगर(i > pollfd_current_max){
					pollfd_current_max = i;
				}
				अवरोध;
			}
		}
	}
	context->events = evt;

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_poll__add_out(काष्ठा mosquitto *context)
{
	वापस mux_poll__add(context, POLLIN | POLLOUT);
}


पूर्णांक mux_poll__remove_out(काष्ठा mosquitto *context)
{
	अगर(context->events & POLLOUT){
		वापस mux_poll__new(context);
	}अन्यथा{
		वापस MOSQ_ERR_SUCCESS;
	}
}


पूर्णांक mux_poll__new(काष्ठा mosquitto *context)
{
	वापस mux_poll__add(context, POLLIN);
}


पूर्णांक mux_poll__delete(काष्ठा mosquitto *context)
{
	माप_प्रकार pollfd_सूचकांक;

	अगर(context->pollfd_सूचकांक != -1){
		pollfds[context->pollfd_सूचकांक].fd = INVALID_SOCKET;
		pollfds[context->pollfd_सूचकांक].events = 0;
		pollfds[context->pollfd_सूचकांक].revents = 0;
		pollfd_सूचकांक = (माप_प्रकार )context->pollfd_सूचकांक;
		context->pollfd_सूचकांक = -1;

		/* If this is the highest index, reduce the current max until we find
		 * the next highest in use index. */
		जबतक(pollfd_सूचकांक == pollfd_current_max
				&& pollfd_सूचकांक > 0
				&& pollfds[pollfd_सूचकांक].fd == INVALID_SOCKET){

			pollfd_सूचकांक--;
			pollfd_current_max--;
		}
	}

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_poll__handle(काष्ठा mosquitto__listener_sock *listensock, पूर्णांक listensock_गिनती)
{
	काष्ठा mosquitto *context;
	पूर्णांक fdcount;
	पूर्णांक timeout;

#अगर_घोषित WITH_WEBSOCKETS
	timeout = 100;
#अन्यथा
	timeout = db.अग्र_event_ms;
#पूर्ण_अगर

#अगर_अघोषित WIN32
	fdcount = poll(pollfds, pollfd_current_max+1, timeout);
#अन्यथा
	fdcount = WSAPoll(pollfds, pollfd_current_max+1, timeout);
#पूर्ण_अगर

	db.now_s = mosquitto_time();
	db.now_real_s = समय(शून्य);

	अगर(fdcount == -1){
#  अगर defined(WIN32) && defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		अगर(WSAGetLastError() == WSAEINVAL){
			/* WSAPoll() immediately returns an error if it is not given
			 * any sockets to wait on. This can happen if we only have
			 * websockets listeners. Sleep a little to prevent a busy loop.
			 */
			Sleep(10);
		}अन्यथा
#  endif
		{
			log__printf(शून्य, MOSQ_LOG_ERR, "Error in poll: %s.", म_त्रुटि(त्रुटि_सं));
		}
	}अन्यथा{
		loop_handle_reads_writes();

		क्रम(पूर्णांक i=0; i<listensock_गिनती; i++){
			अगर(pollfds[i].revents & POLLIN){
#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				अगर(listensock[i].listener->ws_context){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
				}अन्यथा
#पूर्ण_अगर
				{
					जबतक((context = net__socket_accept(&listensock[i])) != शून्य){
					}
				}
			}
		}
	}
	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक mux_poll__cleanup(व्योम)
{
	mosquitto_FREE(pollfds);

	वापस MOSQ_ERR_SUCCESS;
}


अटल व्योम loop_handle_reads_writes(व्योम)
{
	काष्ठा mosquitto *context, *ctxt_tmp;
	पूर्णांक err;
	socklen_t len;
	पूर्णांक rc;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		अगर(context->pollfd_सूचकांक < 0){
			जारी;
		}

		अगर(pollfds[context->pollfd_सूचकांक].fd == INVALID_SOCKET){
			जारी;
		}

		निश्चित(pollfds[context->pollfd_सूचकांक].fd == context->sock);

#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		अगर(context->wsi){
			काष्ठा lws_pollfd wspoll;
			wspoll.fd = pollfds[context->pollfd_सूचकांक].fd;
			wspoll.events = pollfds[context->pollfd_सूचकांक].events;
			wspoll.revents = pollfds[context->pollfd_सूचकांक].revents;
			lws_service_fd(lws_get_context(context->wsi), &wspoll);
			जारी;
		}
#पूर्ण_अगर

#अगर_घोषित WITH_TLS
		अगर(pollfds[context->pollfd_सूचकांक].revents & POLLOUT ||
				context->want_write ||
				(context->ssl && context->state == mosq_cs_new)){
#अन्यथा
		अगर(pollfds[context->pollfd_सूचकांक].revents & POLLOUT){
#पूर्ण_अगर
			अगर(context->state == mosq_cs_connect_pending){
				len = माप(पूर्णांक);
				अगर(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (अक्षर *)&err, &len)){
					अगर(err == 0){
						mosquitto__set_state(context, mosq_cs_new);
#अगर defined(WITH_ADNS) && defined(WITH_BRIDGE)
						अगर(context->bridge){
							bridge__connect_step3(context);
							जारी;
						}
#पूर्ण_अगर
					}
				}अन्यथा{
					do_disconnect(context, MOSQ_ERR_CONN_LOST);
					जारी;
				}
			}
			rc = packet__write(context);
			अगर(rc){
				do_disconnect(context, rc);
				जारी;
			}
		}
	}

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		अगर(context->pollfd_सूचकांक < 0){
			जारी;
		}
#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		अगर(context->wsi){
			// Websocket are already handled above
			जारी;
		}
#पूर्ण_अगर

#अगर_घोषित WITH_TLS
		अगर(pollfds[context->pollfd_सूचकांक].revents & POLLIN ||
				(context->ssl && context->state == mosq_cs_new)){
#अन्यथा
		अगर(pollfds[context->pollfd_सूचकांक].revents & POLLIN){
#पूर्ण_अगर
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
					जारी;
				}
			}जबतक(SSL_तथ्य_PENDING(context));
		}अन्यथा{
			अगर(context->pollfd_सूचकांक >= 0 && pollfds[context->pollfd_सूचकांक].revents & (POLLERR | POLLNVAL | POLLHUP)){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				जारी;
			}
		}
	}
}


#पूर्ण_अगर
