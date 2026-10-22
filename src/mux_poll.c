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

#यदि_अपरिभाषितम् WIN32
#  define _GNU_SOURCE
#यदि_अन्तः

#अन्तर्भाव <assert.h>
#यदि_अपरिभाषितम् WIN32
#अन्तर्भाव <poll.h>
#अन्तर्भाव <unistd.h>
#अन्यथा
#अन्तर्भाव <process.h>
#अन्तर्भाव <winsock2.h>
#अन्तर्भाव <ws2tcpip.h>
#यदि_अन्तः

#अन्तर्भाव <errno.h>
#अन्तर्भाव <signal.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>
#यदि_अपरिभाषितम् WIN32
#  include <sys/socket.h>
#यदि_अन्तः
#अन्तर्भाव <time.h>

#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#यदि_अन्तः

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
#यदि_परिभाषितम् WIN32
	pollfd_max = (size_t)_getmaxstdio();
#अन्यथा
	pollfd_max = (size_t)sysconf(_SC_OPEN_MAX);
#यदि_अन्तः

	pollfds = mosquitto_calloc(pollfd_max, परिमाण(संरचना pollfd));
	यदि(!pollfds){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	स्मृतिं_पूरय(pollfds, 0, परिमाण(संरचना pollfd)*pollfd_max);
	प्रति(size_t i=0; i<pollfd_max; i++){
		pollfds[i].fd = INVALID_SOCKET;
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_poll_सङ्कलय_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्)
{
	size_t pollfd_सूचकांक = 0;

	प्रति(size_t i=0; i<(size_t )listensock_गणनम्; i++){
		pollfds[pollfd_सूचकांक].fd = listensock[i].sock;
		pollfds[pollfd_सूचकांक].events = POLLIN;
		pollfds[pollfd_सूचकांक].revents = 0;
		pollfd_सूचकांक++;
	}

	यदि(pollfd_current_max == 0){
		pollfd_current_max = pollfd_सूचकांक-1;
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_poll__delete_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्)
{
	size_t pollfd_सूचकांक = 0;

	अप्रयुक्तम्(listensock);

	प्रति(size_t i=0; i<(size_t )listensock_गणनम्; i++){
		pollfds[pollfd_सूचकांक].fd = INVALID_SOCKET;
		pollfds[pollfd_सूचकांक].events = 0;
		pollfds[pollfd_सूचकांक].revents = 0;
		pollfd_सूचकांक++;
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क mux_poll_सङ्कलय(संरचना mosquitto *context, uint16_t evt)
{
	यदि(context->events == evt){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	यदि(context->pollfd_सूचकांक != -1){
		pollfds[context->pollfd_सूचकांक].fd = context->sock;
		pollfds[context->pollfd_सूचकांक].events = (ह्रस्व पूर्णाङ्क)evt;
		pollfds[context->pollfd_सूचकांक].revents = 0;
	}अन्यथा{
		प्रति(size_t i=0; i<pollfd_max; i++){
			यदि(pollfds[i].fd == INVALID_SOCKET){
				pollfds[i].fd = context->sock;
				pollfds[i].events = POLLIN;
				pollfds[i].revents = 0;
				context->pollfd_सूचकांक = (पूर्णाङ्क)i;
				यदि(i > pollfd_current_max){
					pollfd_current_max = i;
				}
				भङ्ग;
			}
		}
	}
	context->events = evt;

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_poll_सङ्कलय_बहिर्गमनम्(संरचना mosquitto *context)
{
	प्रत्यागम mux_poll_सङ्कलय(context, POLLIN | POLLOUT);
}


पूर्णाङ्क mux_poll_remove_बहिर्गमनम्(संरचना mosquitto *context)
{
	यदि(context->events & POLLOUT){
		प्रत्यागम mux_poll__new(context);
	}अन्यथा{
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}
}


पूर्णाङ्क mux_poll__new(संरचना mosquitto *context)
{
	प्रत्यागम mux_poll_सङ्कलय(context, POLLIN);
}


पूर्णाङ्क mux_poll__delete(संरचना mosquitto *context)
{
	size_t pollfd_सूचकांक;

	यदि(context->pollfd_सूचकांक != -1){
		pollfds[context->pollfd_सूचकांक].fd = INVALID_SOCKET;
		pollfds[context->pollfd_सूचकांक].events = 0;
		pollfds[context->pollfd_सूचकांक].revents = 0;
		pollfd_सूचकांक = (size_t )context->pollfd_सूचकांक;
		context->pollfd_सूचकांक = -1;

		/* If this is the highest index, reduce the current max until we find
		 * the next highest in use index. */
		यावत्(pollfd_सूचकांक == pollfd_current_max
				&& pollfd_सूचकांक > 0
				&& pollfds[pollfd_सूचकांक].fd == INVALID_SOCKET){

			pollfd_सूचकांक--;
			pollfd_current_max--;
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_poll__handle(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्)
{
	संरचना mosquitto *context;
	पूर्णाङ्क fdcount;
	पूर्णाङ्क timeout;

#यदि_परिभाषितम् WITH_WEBSOCKETS
	timeout = 100;
#अन्यथा
	timeout = db.अग्र_event_ms;
#यदि_अन्तः

#यदि_अपरिभाषितम् WIN32
	fdcount = poll(pollfds, pollfd_current_max+1, timeout);
#अन्यथा
	fdcount = WSAPoll(pollfds, pollfd_current_max+1, timeout);
#यदि_अन्तः

	db.now_s = mosquitto_time();
	db.now_real_s = time(रिक्त);

	यदि(fdcount == -1){
#  यदि defined(WIN32) && defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		यदि(WSAGet_चरमः_Error() == WSAEINVAL){
			/* WSAPoll() immediately returns an error if it is not given
			 * any sockets to wait on. This can happen if we only have
			 * websockets listeners. Sleep a little to prevent a busy loop.
			 */
			Sleep(10);
		}अन्यथा
#  endif
		{
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error in poll: %s.", strerror(errno));
		}
	}अन्यथा{
		loop_handle_reads_writes();

		प्रति(पूर्णाङ्क i=0; i<listensock_गणनम्; i++){
			यदि(pollfds[i].revents & POLLIN){
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
				यदि(listensock[i].listener->ws_context){
					/* Nothing needs to happen here, because we always call lws_service in the loop.
					 * The important point is we've been woken up for this listener. */
				}अन्यथा
#यदि_अन्तः
				{
					यावत्((context = net__socket_accept(&listensock[i])) != रिक्त){
					}
				}
			}
		}
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mux_poll__cleanup(शून्य)
{
	mosquitto_FREE(pollfds);

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर शून्य loop_handle_reads_writes(शून्य)
{
	संरचना mosquitto *context, *ctxt_tmp;
	पूर्णाङ्क err;
	socklen_t len;
	पूर्णाङ्क rc;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		यदि(context->pollfd_सूचकांक < 0){
			अनुवर्त;
		}

		यदि(pollfds[context->pollfd_सूचकांक].fd == INVALID_SOCKET){
			अनुवर्त;
		}

		प्रतिज्ञा(pollfds[context->pollfd_सूचकांक].fd == context->sock);

#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		यदि(context->wsi){
			संरचना lws_pollfd wspoll;
			wspoll.fd = pollfds[context->pollfd_सूचकांक].fd;
			wspoll.events = pollfds[context->pollfd_सूचकांक].events;
			wspoll.revents = pollfds[context->pollfd_सूचकांक].revents;
			lws_service_fd(lws_get_context(context->wsi), &wspoll);
			अनुवर्त;
		}
#यदि_अन्तः

#यदि_परिभाषितम् WITH_TLS
		यदि(pollfds[context->pollfd_सूचकांक].revents & POLLOUT ||
				context->want_write ||
				(context->ssl && context->state == mosq_cs_new)){
#अन्यथा
		यदि(pollfds[context->pollfd_सूचकांक].revents & POLLOUT){
#यदि_अन्तः
			यदि(context->state == mosq_cs_connect_pending){
				len = परिमाण(पूर्णाङ्क);
				यदि(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (वर्ण *)&err, &len)){
					यदि(err == 0){
						mosquitto_समुच्चयः_state(context, mosq_cs_new);
#यदि defined(WITH_ADNS) && defined(WITH_BRIDGE)
						यदि(context->bridge){
							bridge__connect_step3(context);
							अनुवर्त;
						}
#यदि_अन्तः
					}
				}अन्यथा{
					do_disconnect(context, MOSQ_ERR_CONN_नष्टम्);
					अनुवर्त;
				}
			}
			rc = packet__write(context);
			यदि(rc){
				do_disconnect(context, rc);
				अनुवर्त;
			}
		}
	}

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		यदि(context->pollfd_सूचकांक < 0){
			अनुवर्त;
		}
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		यदि(context->wsi){
			// Websocket are already handled above
			अनुवर्त;
		}
#यदि_अन्तः

#यदि_परिभाषितम् WITH_TLS
		यदि(pollfds[context->pollfd_सूचकांक].revents & POLLIN ||
				(context->ssl && context->state == mosq_cs_new)){
#अन्यथा
		यदि(pollfds[context->pollfd_सूचकांक].revents & POLLIN){
#यदि_अन्तः
			कुरु{
				विकल्प(context->transport){
					पक्ष mosq_t_tcp:
					पक्ष mosq_t_ws:
						rc = packet__read(context);
						भङ्ग;
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
					पक्ष mosq_t_http:
						rc = http__read(context);
						भङ्ग;
#यदि_अन्तः
#यदि !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN
					/* Not supported with LWS */
					पक्ष mosq_t_proxy_v2:
						rc = proxy_v2__read(context);
						भङ्ग;
					पक्ष mosq_t_proxy_v1:
						rc = proxy_v1__read(context);
						भङ्ग;
#यदि_अन्तः
					स्वभाव:
						rc = MOSQ_ERR_INVAL;
						भङ्ग;
				}
				यदि(rc){
					do_disconnect(context, rc);
					अनुवर्त;
				}
			}यावत्(SSL_दत्त_PENDING(context));
		}अन्यथा{
			यदि(context->pollfd_सूचकांक >= 0 && pollfds[context->pollfd_सूचकांक].revents & (POLLERR | POLLNVAL | POLLHUP)){
				do_disconnect(context, MOSQ_ERR_CONN_नष्टम्);
				अनुवर्त;
			}
		}
	}
}


#यदि_अन्तः
