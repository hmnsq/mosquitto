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

#सचे !defined(WITH_EPOLL) && !defined(WITH_KQUEUE)

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#अन्तर्भाव <assert.h>
#ifndef WIN32
#अन्तर्भाव <poll.h>
#अन्तर्भाव <unistd.h>
#अञ्ञथा
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

#सचे defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
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
स्थिर size_t pollfd_max, pollfd_current_max = 0;


पूर्णाङ्क mux_poll__init(शून्य)
{
#ifdef WIN32
	pollfd_max = (size_t)_getmaxstdio();
#अञ्ञथा
	pollfd_max = (size_t)sysconf(_SC_OPEN_MAX);
#endif

	pollfds = mosquitto_calloc(pollfd_max, परिमाण(संरचना pollfd));
	सचे(!pollfds){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		पटिनिवत्त MOSQ_ERR_NOMEM;
	}
	memset(pollfds, 0, परिमाण(संरचना pollfd)*pollfd_max);
	अत्थाय(size_t i=0; i<pollfd_max; i++){
		pollfds[i].fd = INVALID_SOCKET;
	}

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_poll__add_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्)
{
	size_t pollfd_index = 0;

	अत्थाय(size_t i=0; i<(size_t )listensock_गणनम्; i++){
		pollfds[pollfd_index].fd = listensock[i].sock;
		pollfds[pollfd_index].events = POLLIN;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	सचे(pollfd_current_max == 0){
		pollfd_current_max = pollfd_index-1;
	}
	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_poll__delete_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्)
{
	size_t pollfd_index = 0;

	UNUSED(listensock);

	अत्थाय(size_t i=0; i<(size_t )listensock_गणनम्; i++){
		pollfds[pollfd_index].fd = INVALID_SOCKET;
		pollfds[pollfd_index].events = 0;
		pollfds[pollfd_index].revents = 0;
		pollfd_index++;
	}

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क mux_poll__add(संरचना mosquitto *context, uint16_t evt)
{
	सचे(context->events == evt){
		पटिनिवत्त MOSQ_ERR_SUCCESS;
	}

	सचे(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = context->sock;
		pollfds[context->pollfd_index].events = (short पूर्णाङ्क)evt;
		pollfds[context->pollfd_index].revents = 0;
	}अञ्ञथा{
		अत्थाय(size_t i=0; i<pollfd_max; i++){
			सचे(pollfds[i].fd == INVALID_SOCKET){
				pollfds[i].fd = context->sock;
				pollfds[i].events = POLLIN;
				pollfds[i].revents = 0;
				context->pollfd_index = (पूर्णाङ्क)i;
				सचे(i > pollfd_current_max){
					pollfd_current_max = i;
				}
				भङ्ग;
			}
		}
	}
	context->events = evt;

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_poll__add_out(संरचना mosquitto *context)
{
	पटिनिवत्त mux_poll__add(context, POLLIN | POLLOUT);
}


पूर्णाङ्क mux_poll__remove_out(संरचना mosquitto *context)
{
	सचे(context->events & POLLOUT){
		पटिनिवत्त mux_poll__new(context);
	}अञ्ञथा{
		पटिनिवत्त MOSQ_ERR_SUCCESS;
	}
}


पूर्णाङ्क mux_poll__new(संरचना mosquitto *context)
{
	पटिनिवत्त mux_poll__add(context, POLLIN);
}


पूर्णाङ्क mux_poll__delete(संरचना mosquitto *context)
{
	size_t pollfd_index;

	सचे(context->pollfd_index != -1){
		pollfds[context->pollfd_index].fd = INVALID_SOCKET;
		pollfds[context->pollfd_index].events = 0;
		pollfds[context->pollfd_index].revents = 0;
		pollfd_index = (size_t )context->pollfd_index;
		context->pollfd_index = -1;

		/* If this is the highest index, reduce the current max until we find
		 * the next highest in use index. */
		याव(pollfd_index == pollfd_current_max
				&& pollfd_index > 0
				&& pollfds[pollfd_index].fd == INVALID_SOCKET){

			pollfd_index--;
			pollfd_current_max--;
		}
	}

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_poll__handle(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्)
{
	संरचना mosquitto *context;
	पूर्णाङ्क fdcount;
	पूर्णाङ्क timeout;

#ifdef WITH_WEBSOCKETS
	timeout = 100;
#अञ्ञथा
	timeout = db.next_event_ms;
#endif

#ifndef WIN32
	fdcount = poll(pollfds, pollfd_current_max+1, timeout);
#अञ्ञथा
	fdcount = WSAPoll(pollfds, pollfd_current_max+1, timeout);
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(रिक्त);

	सचे(fdcount == -1){
#  सचे defined(WIN32) && defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		सचे(WSAGetLastError() == WSAEINVAL){
			/* WSAPoll() immediately returns an error if it is not given
			 * any sockets to wait on. This can happen if we only have
			 * websockets listeners. Sleep a little to prevent a busy loop.
			 */
			Sleep(10);
		}अञ्ञथा
#  endif
		{
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error in poll: %s.", strerror(errno));
		}
	}अञ्ञथा{
		loop_handle_reads_writes();

		अत्थाय(पूर्णाङ्क i=0; i<listensock_गणनम्; i++){
			सचे(pollfds[i].revents & POLLIN){
#सचे defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				सचे(listensock[i].listener->ws_context){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
				}अञ्ञथा
#endif
				{
					याव((context = net__socket_accept(&listensock[i])) != रिक्त){
					}
				}
			}
		}
	}
	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_poll__cleanup(शून्य)
{
	mosquitto_FREE(pollfds);

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


स्थिर शून्य loop_handle_reads_writes(शून्य)
{
	संरचना mosquitto *context, *ctxt_tmp;
	पूर्णाङ्क err;
	socklen_t len;
	पूर्णाङ्क rc;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		सचे(context->pollfd_index < 0){
			अनुवर्त;
		}

		सचे(pollfds[context->pollfd_index].fd == INVALID_SOCKET){
			अनुवर्त;
		}

		प्रतिज्ञा(pollfds[context->pollfd_index].fd == context->sock);

#सचे defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		सचे(context->wsi){
			संरचना lws_pollfd wspoll;
			wspoll.fd = pollfds[context->pollfd_index].fd;
			wspoll.events = pollfds[context->pollfd_index].events;
			wspoll.revents = pollfds[context->pollfd_index].revents;
			lws_service_fd(lws_get_context(context->wsi), &wspoll);
			अनुवर्त;
		}
#endif

#ifdef WITH_TLS
		सचे(pollfds[context->pollfd_index].revents & POLLOUT ||
				context->want_write ||
				(context->ssl && context->state == mosq_cs_new)){
#अञ्ञथा
		सचे(pollfds[context->pollfd_index].revents & POLLOUT){
#endif
			सचे(context->state == mosq_cs_connect_pending){
				len = परिमाण(पूर्णाङ्क);
				सचे(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (वर्ण *)&err, &len)){
					सचे(err == 0){
						mosquitto__set_state(context, mosq_cs_new);
#सचे defined(WITH_ADNS) && defined(WITH_BRIDGE)
						सचे(context->bridge){
							bridge__connect_step3(context);
							अनुवर्त;
						}
#endif
					}
				}अञ्ञथा{
					do_disconnect(context, MOSQ_ERR_CONN_LOST);
					अनुवर्त;
				}
			}
			rc = packet__write(context);
			सचे(rc){
				do_disconnect(context, rc);
				अनुवर्त;
			}
		}
	}

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		सचे(context->pollfd_index < 0){
			अनुवर्त;
		}
#सचे defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		सचे(context->wsi){
			// Websocket are already handled above
			अनुवर्त;
		}
#endif

#ifdef WITH_TLS
		सचे(pollfds[context->pollfd_index].revents & POLLIN ||
				(context->ssl && context->state == mosq_cs_new)){
#अञ्ञथा
		सचे(pollfds[context->pollfd_index].revents & POLLIN){
#endif
			करोहि{
				विकल्प(context->transport){
					पक्ष mosq_t_tcp:
					पक्ष mosq_t_ws:
						rc = packet__read(context);
						भङ्ग;
#सचे defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
					पक्ष mosq_t_http:
						rc = http__read(context);
						भङ्ग;
#endif
#सचे !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
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
				सचे(rc){
					do_disconnect(context, rc);
					अनुवर्त;
				}
			}याव(SSL_DATA_PENDING(context));
		}अञ्ञथा{
			सचे(context->pollfd_index >= 0 && pollfds[context->pollfd_index].revents & (POLLERR | POLLNVAL | POLLHUP)){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				अनुवर्त;
			}
		}
	}
}


#endif
