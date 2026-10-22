/*
Copyright (c) 2014-2021 Roger Light <roger@atchoo.org>

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

#अगर_घोषित WITH_WEBSOCKETS

#समावेश "config.h"

#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#पूर्ण_अगर
#समावेश "mosquitto_internal.h"
#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/mqtt_protocol.h"
#समावेश "packet_mosq.h"
#समावेश "sys_tree.h"
#समावेश "util_mosq.h"

#समावेश <stdlib.h>
#समावेश <errno.h>
#समावेश <sys/stat.h>

#अगर_अघोषित WIN32
#  include <sys/socket.h>
#पूर्ण_अगर

#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
/* Be careful if changing these, if TX is not bigger than SERV then there can
 * be very large write performance penalties.
 */
#घोषणा WS_SERV_BUF_आकार 4096
#घोषणा WS_TX_BUF_आकार (WS_SERV_BUF_आकार*2)

अटल पूर्णांक callback_mqtt(
		काष्ठा lws *wsi,
		क्रमागत lws_callback_reasons reason,
		व्योम *user,
		व्योम *in,
		माप_प्रकार len);

अटल पूर्णांक callback_http(
		काष्ठा lws *wsi,
		क्रमागत lws_callback_reasons reason,
		व्योम *user,
		व्योम *in,
		माप_प्रकार len);

क्रमागत mosq_ws_protocols {
	PROTOCOL_HTTP = 0,
	PROTOCOL_MQTT,
	DEMO_PROTOCOL_गिनती,
};

काष्ठा libws_http_तथ्य {
	खाता *fptr;
};

अटल काष्ठा lws_protocols protocols[] = {
	/* first protocol must always be HTTP handler */
	{
		"http-only",                        /* name */
		callback_http,                      /* lws_callback_function */
		माप (काष्ठा libws_http_तथ्य),    /* per_session_data_size */
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		शून्य,                               /* user v1.4 on */
		WS_TX_BUF_आकार                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqtt",
		callback_mqtt,
		माप(काष्ठा libws_mqtt_तथ्य),
		0,                                  /* rx_buffer_size */
		1,                                  /* id */
		शून्य,                               /* user v1.4 on */
		WS_TX_BUF_आकार                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqttv3.1",
		callback_mqtt,
		माप(काष्ठा libws_mqtt_तथ्य),
		0,                                  /* rx_buffer_size */
		2,                                  /* id */
		शून्य,                               /* user v1.4 on */
		WS_TX_BUF_आकार                      /* tx_packet_size v2.3.0 */
	},
	{
		शून्य,
		शून्य,
		0,
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		शून्य,                               /* user v1.4 on */
		0                                   /* tx_packet_size v2.3.0 */
	}
};


अटल व्योम easy_address(पूर्णांक sock, काष्ठा mosquitto *mosq)
{
	अक्षर address[1024];

	अगर(!net__socket_get_address(sock, address, 1024, &mosq->remote_port)){
		mosq->address = mosquitto_strdup(address);
	}
}


अटल पूर्णांक callback_mqtt(
		काष्ठा lws *wsi,
		क्रमागत lws_callback_reasons reason,
		व्योम *user,
		व्योम *in,
		माप_प्रकार len)
{
	काष्ठा mosquitto *mosq = शून्य;
	काष्ठा mosquitto__packet *packet;
	पूर्णांक गिनती;
	अचिन्हित पूर्णांक ucount;
	स्थिर काष्ठा lws_protocols *p;
	काष्ठा libws_mqtt_तथ्य *u = (काष्ठा libws_mqtt_तथ्य *)user;
	माप_प्रकार pos;
	uint8_t *buf;
	पूर्णांक rc;
	uint8_t byte;
	अक्षर ip_addr_buff[1024];

	चयन(reason){
		हाल LWS_CALLBACK_ESTABLISHED:
			mosq = context__init();
			अगर(mosq){
				p = lws_get_protocol(wsi);
				mosq->listener = p->user;
				अगर(!mosq->listener){
					mosquitto_FREE(mosq);
					वापस -1;
				}
				mosq->wsi = wsi;
#अगर_घोषित WITH_TLS
				अगर(in){
					mosq->ssl = (SSL *)in;
					अगर(!mosq->listener->ssl_ctx){
						mosq->listener->ssl_ctx = SSL_get_SSL_CTX(mosq->ssl);
					}
				}
#पूर्ण_अगर
				u->mosq = mosq;
			}अन्यथा{
				वापस -1;
			}

			easy_address(lws_get_socket_fd(wsi), mosq);
			अगर(!mosq->address){
				/* getpeername and inet_ntop failed and not a bridge */
				mosquitto_FREE(mosq);
				u->mosq = शून्य;
				वापस -1;
			}
			mosq->listener->client_गिनती++;
			अगर((mosq->listener->max_connections > 0 && mosq->listener->client_गिनती > mosq->listener->max_connections)
					|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (अचिन्हित पूर्णांक)db.config->global_max_connections)){

				अगर(db.config->connection_messages == true){
					log__printf(शून्य, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", mosq->address);
				}
				mosq->listener->client_गिनती--;
				mosquitto_FREE(mosq->address);
				mosquitto_FREE(mosq);
				u->mosq = शून्य;
				वापस -1;
			}
			mosq->sock = lws_get_socket_fd(wsi);
			HASH_ADD(hh_sock, db.contexts_by_sock, sock, माप(mosq->sock), mosq);
			mux__new(mosq);
			अवरोध;

		हाल LWS_CALLBACK_CLOSED:
			अगर(!u){
				वापस -1;
			}
			mosq = u->mosq;
			अगर(mosq){
				अगर(mosq->sock != INVALID_SOCKET){
					HASH_DELETE(hh_sock, db.contexts_by_sock, mosq);
					mosq->sock = INVALID_SOCKET;
					mux__delete(mosq);
				}
				mosq->wsi = शून्य;
#अगर_घोषित WITH_TLS
				mosq->ssl = शून्य;
#पूर्ण_अगर
				do_disconnect(mosq, MOSQ_ERR_CONN_LOST);
			}
			अवरोध;

		हाल LWS_CALLBACK_SERVER_WRITEABLE:
			अगर(!u){
				वापस -1;
			}
			mosq = u->mosq;
			अगर(!mosq){
				वापस -1;
			}

			rc = db__message_write_inflight_out_latest(mosq);
			अगर(rc){
				वापस -1;
			}
			rc = db__message_write_queued_out(mosq);
			अगर(rc){
				वापस -1;
			}

			जबतक(mosq->out_packet && !lws_send_pipe_choked(mosq->wsi)){
				packet = mosq->out_packet;
				गिनती = lws_write(wsi, &packet->payload[packet->pos], packet->to_process, LWS_WRITE_द्विआधारी);
				अगर(गिनती < 0){
					अगर(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						वापस -1;
					}
					वापस 0;
				}
				ucount = (अचिन्हित पूर्णांक)गिनती;
#अगर_घोषित WITH_SYS_वृक्ष
				metrics__int_inc(mosq_गणक_bytes_sent, ucount);
#पूर्ण_अगर
				packet->to_process -= ucount;
				packet->pos += ucount;
				अगर(packet->to_process > 0){
					अगर(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						वापस -1;
					}
					अवरोध;
				}

#अगर_घोषित WITH_SYS_वृक्ष
				metrics__int_inc(mosq_गणक_messages_sent, 1);
				अगर(((packet->command)&0xF0) == CMD_PUBLISH){
					metrics__int_inc(mosq_गणक_mqtt_publish_sent, 1);
				}
#पूर्ण_अगर

				packet_get_अग्र_out(mosq);
				mosquitto_FREE(packet);

				mosq->अग्र_संदेश_out = db.now_s + mosq->keepalive;
			}
			अगर(mosq->state == mosq_cs_disconnect_ws
					|| mosq->state == mosq_cs_disconnecting
					|| mosq->state == mosq_cs_disused){

				वापस -1;
			}
			अगर(mosq->out_packet){
				lws_callback_on_writable(mosq->wsi);
			}
			अवरोध;

		हाल LWS_CALLBACK_RECEIVE:
			अगर(!u || !u->mosq){
				वापस -1;
			}
			mosq = u->mosq;
			pos = 0;
			buf = (uint8_t *)in;
			metrics__int_inc(mosq_गणक_bytes_received, (int64_t)len);
			जबतक(pos < len){
				अगर(!mosq->in_packet.command){
					mosq->in_packet.command = buf[pos];
					pos++;
					/* Clients must send CONNECT as their first command. */
					अगर(mosq->state == mosq_cs_new && (mosq->in_packet.command&0xF0) != CMD_CONNECT){
						वापस -1;
					}
				}
				अगर(mosq->in_packet.remaining_गिनती <= 0){
					करो{
						अगर(pos == len){
							वापस 0;
						}
						byte = buf[pos];
						pos++;

						mosq->in_packet.remaining_गिनती--;
						/* Max 4 bytes length for remaining length as defined by protocol.
						* Anything more likely means a broken/malicious client.
						*/
						अगर(mosq->in_packet.remaining_गिनती < -4){
							वापस -1;
						}

						mosq->in_packet.remaining_लंबाई += (byte & 127) * mosq->in_packet.remaining_mult;
						mosq->in_packet.remaining_mult *= 128;
					}जबतक((byte & 128) != 0);
					mosq->in_packet.remaining_गिनती = (int8_t)(mosq->in_packet.remaining_गिनती * -1);

					अगर(mosq->in_packet.remaining_लंबाई > 0){
						mosq->in_packet.payload = mosquitto_malloc(mosq->in_packet.remaining_लंबाई*माप(uint8_t));
						अगर(!mosq->in_packet.payload){
							वापस -1;
						}
						mosq->in_packet.to_process = mosq->in_packet.remaining_लंबाई;
					}
				}
				अगर(mosq->in_packet.to_process>0){
					अगर((uint32_t)len - pos >= mosq->in_packet.to_process){
						स_नकल(&mosq->in_packet.payload[mosq->in_packet.pos], &buf[pos], mosq->in_packet.to_process);
						mosq->in_packet.pos += mosq->in_packet.to_process;
						pos += mosq->in_packet.to_process;
						mosq->in_packet.to_process = 0;
					}अन्यथा{
						स_नकल(&mosq->in_packet.payload[mosq->in_packet.pos], &buf[pos], len-pos);
						mosq->in_packet.pos += (uint32_t)(len-pos);
						mosq->in_packet.to_process -= (uint32_t)(len-pos);
						वापस 0;
					}
				}
				/* All data for this packet is read. */
				mosq->in_packet.pos = 0;

#अगर_घोषित WITH_SYS_वृक्ष
				metrics__int_inc(mosq_गणक_messages_received, 1);
#पूर्ण_अगर
				rc = handle__packet(mosq);

				/* Free data and reset values */
				packet__cleanup(&mosq->in_packet);

				keepalive__update(mosq);

				अगर(rc && mosq->out_packet){
					अगर(mosq->state != mosq_cs_disconnecting){
						mosquitto__set_state(mosq, mosq_cs_disconnect_ws);
					}
					lws_callback_on_writable(mosq->wsi);
				}अन्यथा अगर(rc){
					do_disconnect(mosq, MOSQ_ERR_CONN_LOST);
					वापस -1;
				}
			}
			अवरोध;

		शेष:
			अवरोध;
	}

	वापस 0;
}


अटल अक्षर *http__canonical_filename(
		काष्ठा lws *wsi,
		स्थिर अक्षर *in,
		स्थिर अक्षर *http_dir)
{
	माप_प्रकार inlen, slen;
	अक्षर *filename, *filename_canonical;

	inlen = म_माप(in);
	अगर(in[inlen-1] == '/'){
		slen = म_माप(http_dir) + inlen + म_माप("/index.html") + 2;
	}अन्यथा{
		slen = म_माप(http_dir) + inlen + 2;
	}
	filename = mosquitto_malloc(slen);
	अगर(!filename){
		lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, शून्य);
		वापस शून्य;
	}
	अगर(((अक्षर *)in)[inlen-1] == '/'){
		snprintf(filename, slen, "%s%sindex.html", http_dir, (अक्षर *)in);
	}अन्यथा{
		snprintf(filename, slen, "%s%s", http_dir, (अक्षर *)in);
	}


	/* Get canonical path and check it is within our http_dir */
#अगर_घोषित WIN32
	filename_canonical = _fullpath(शून्य, filename, 0);
	mosquitto_FREE(filename);
	अगर(!filename_canonical){
		lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, शून्य);
		वापस शून्य;
	}
#अन्यथा
	filename_canonical = realpath(filename, शून्य);
	mosquitto_FREE(filename);
	अगर(!filename_canonical){
		अगर(त्रुटि_सं == EACCES){
			lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, शून्य);
		}अन्यथा अगर(त्रुटि_सं == EINVAL || त्रुटि_सं == EIO || त्रुटि_सं == ELOOP){
			lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, शून्य);
		}अन्यथा अगर(त्रुटि_सं == ENAMETOOLONG){
			lws_return_http_status(wsi, HTTP_STATUS_REQ_URI_TOO_LONG, शून्य);
		}अन्यथा अगर(त्रुटि_सं == ENOENT || त्रुटि_सं == ENOTDIR){
			lws_return_http_status(wsi, HTTP_STATUS_NOT_FOUND, शून्य);
		}
		वापस शून्य;
	}
#पूर्ण_अगर
	अगर(म_भेदन(http_dir, filename_canonical, म_माप(http_dir))){
		/* Requested file isn't within http_dir, deny access. */
		SAFE_FREE(filename_canonical);
		lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, शून्य);
		वापस शून्य;
	}

	वापस filename_canonical;
}


अटल पूर्णांक callback_http(
		काष्ठा lws *wsi,
		क्रमागत lws_callback_reasons reason,
		व्योम *user,
		व्योम *in,
		माप_प्रकार len)
{
	काष्ठा libws_http_तथ्य *u = (काष्ठा libws_http_तथ्य *)user;
	काष्ठा libws_mqtt_hack *hack;
	अक्षर *http_dir;
	माप_प्रकार buflen;
	माप_प्रकार wlen;
	पूर्णांक rc;
	अक्षर *filename_canonical;
	अचिन्हित अक्षर buf[4096];
	काष्ठा stat filestat;
	काष्ठा mosquitto *mosq;
	काष्ठा lws_pollargs *pollargs = (काष्ठा lws_pollargs *)in;
	पूर्णांक hlen;

	/* FIXME - ssl cert verification is done here. */

	चयन(reason){
		हाल LWS_CALLBACK_HTTP:
			अगर(!u){
				वापस -1;
			}

			hack = (काष्ठा libws_mqtt_hack *)lws_context_user(lws_get_context(wsi));
			अगर(!hack){
				वापस -1;
			}
			http_dir = hack->http_dir;

			अगर(!http_dir){
				/* http disabled */
				वापस -1;
			}

			/* Forbid POST */
			अगर(lws_hdr_योग_लंबाई(wsi, WSI_TOKEN_POST_URI)){
				lws_return_http_status(wsi, HTTP_STATUS_METHOD_NOT_ALLOWED, शून्य);
				वापस -1;
			}

			filename_canonical = http__canonical_filename(wsi, (अक्षर *)in, http_dir);
			अगर(!filename_canonical){
				वापस -1;
			}

			u->fptr = ख_खोलो(filename_canonical, "rb");
			अगर(!u->fptr){
				SAFE_FREE(filename_canonical);
				lws_return_http_status(wsi, HTTP_STATUS_NOT_FOUND, शून्य);
				वापस -1;
			}
			अगर(ख_स्थिति(fileno(u->fptr), &filestat) < 0){
				SAFE_FREE(filename_canonical);
				lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, शून्य);
				ख_बंद(u->fptr);
				u->fptr = शून्य;
				वापस -1;
			}


			अगर((filestat.st_mode & S_IFDIR) == S_IFDIR){
				ख_बंद(u->fptr);
				u->fptr = शून्य;
				SAFE_FREE(filename_canonical);

				/* FIXME - use header functions from lws 2.x */
				buflen = (माप_प्रकार)snprintf((अक्षर *)buf, 4096, "HTTP/1.0 302 OK\r\n"
						"Location: %s/\r\n\r\n",
						(अक्षर *)in);
				वापस lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
			}

			अगर((filestat.st_mode & S_IFREG) != S_IFREG){
				lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, शून्य);
				ख_बंद(u->fptr);
				u->fptr = शून्य;
				SAFE_FREE(filename_canonical);
				वापस -1;
			}

			log__printf(शून्य, MOSQ_LOG_DEBUG, "http serving file \"%s\".", filename_canonical);
			SAFE_FREE(filename_canonical);
			/* FIXME - use header functions from lws 2.x */
			buflen = (माप_प्रकार)snprintf((अक्षर *)buf, 4096, "HTTP/1.0 200 OK\r\n"
					"Server: mosquitto\r\n"
					"Content-Length: %u\r\n\r\n",
					(अचिन्हित पूर्णांक)filestat.st_आकार);
			अगर(lws_write(wsi, buf, buflen, LWS_WRITE_HTTP) < 0){
				ख_बंद(u->fptr);
				u->fptr = शून्य;
				वापस -1;
			}
			lws_callback_on_writable(wsi);
			अवरोध;

		हाल LWS_CALLBACK_HTTP_BODY:
			/* For extra POST data? */
			वापस -1;

		हाल LWS_CALLBACK_HTTP_BODY_COMPLETION:
			/* For end of extra POST data? */
			वापस -1;

		हाल LWS_CALLBACK_FILTER_HTTP_CONNECTION:
			/* Access control here */
			वापस 0;

#अगर LWS_LIBRARY_VERSION_संख्या >= 3001000
		हाल LWS_CALLBACK_HTTP_CONFIRM_UPGRADE:
			hack = lws_context_user(lws_get_context(wsi));
			अगर(hack->listener->ws_origin_गिनती){
				hlen = lws_hdr_योग_लंबाई(wsi, WSI_TOKEN_ORIGIN);
				अगर(hlen <= 0 || hlen >= (पूर्णांक)माप(buf)){
					वापस -1;
				}
				क्रम(पूर्णांक i=0; i<hack->listener->ws_origin_गिनती; i++){
					lws_hdr_copy(wsi, (अक्षर *)buf, माप(buf), WSI_TOKEN_ORIGIN);
					अगर((!म_भेद(hack->listener->ws_origins[i], (अक्षर *)buf))){
						वापस 0;
					}
				}
				वापस -1;
			}अन्यथा{
				वापस 0;
			}
#पूर्ण_अगर

		हाल LWS_CALLBACK_HTTP_WRITEABLE:
			/* Send our data here */
			अगर(u && u->fptr){
				करो{
					buflen = ख_पढ़ो(buf, 1, माप(buf), u->fptr);
					अगर(buflen < 1){
						ख_बंद(u->fptr);
						u->fptr = शून्य;
						वापस -1;
					}
					rc = lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
					अगर(rc < 0){
						वापस -1;
					}
					wlen = (माप_प्रकार)rc;
					/* while still active, extend timeout */
					अगर(wlen){
						lws_set_timeout(wsi, PENDING_TIMEOUT_HTTP_CONTENT, 10);
					}
					अगर(wlen < buflen){
						अगर(ख_जाओ(u->fptr, (दीर्घ)(buflen-wlen), प्रस्तुत_से) < 0){
							ख_बंद(u->fptr);
							u->fptr = शून्य;
							वापस -1;
						}
					}अन्यथा{
						अगर(buflen < माप(buf)){
							ख_बंद(u->fptr);
							u->fptr = शून्य;
						}
					}
				}जबतक(u->fptr && !lws_send_pipe_choked(wsi));
				lws_callback_on_writable(wsi);
			}अन्यथा{
				वापस -1;
			}
			अवरोध;

		हाल LWS_CALLBACK_CLOSED:
		हाल LWS_CALLBACK_CLOSED_HTTP:
		हाल LWS_CALLBACK_HTTP_FILE_COMPLETION:
			अगर(u && u->fptr){
				ख_बंद(u->fptr);
				u->fptr = शून्य;
			}
			अवरोध;

		हाल LWS_CALLBACK_ADD_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, माप(pollargs->fd), mosq);
			अगर(mosq){
				अगर(pollargs->events & LWS_POLLOUT){
					mux__add_out(mosq);
				}अन्यथा{
					mux__remove_out(mosq);
				}
			}अन्यथा{
				अगर(pollargs->events & POLLIN){
					/* Assume this is a new listener */
					listeners__add_websockets(lws_get_context(wsi), pollargs->fd);
				}
			}
			अवरोध;

		हाल LWS_CALLBACK_DEL_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, माप(pollargs->fd), mosq);
			अगर(mosq){
				mux__delete(mosq);
			}
			अवरोध;

		हाल LWS_CALLBACK_CHANGE_MODE_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, माप(pollargs->fd), mosq);
			अगर(mosq){
				अगर(pollargs->events & LWS_POLLHUP){
					वापस 1;
				}अन्यथा अगर(pollargs->events & LWS_POLLOUT){
					mux__add_out(mosq);
				}अन्यथा{
					mux__remove_out(mosq);
				}
			}
			अवरोध;

#अगर_घोषित WITH_TLS
		हाल LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION:
			अगर(!len || (SSL_get_verify_परिणाम((SSL *)in) != X509_V_OK)){
				वापस 1;
			}
			अवरोध;
#पूर्ण_अगर

		शेष:
			वापस 0;
	}

	वापस 0;
}


अटल व्योम log_wrap(पूर्णांक level, स्थिर अक्षर *पंक्ति)
{
	अक्षर *l = (अक्षर *)पंक्ति;
	UNUSED(level);
	l[म_माप(पंक्ति)-1] = '\0'; /* Remove \n */
	log__printf(शून्य, MOSQ_LOG_WEBSOCKETS, "%s", l);
}


व्योम mosq_websockets_init(काष्ठा mosquitto__listener *listener, स्थिर काष्ठा mosquitto__config *conf)
{
	काष्ठा lws_context_creation_info info;
	काष्ठा lws_protocols *p;
	माप_प्रकार protocol_गिनती;
	पूर्णांक i;
	काष्ठा libws_mqtt_hack *user;

	/* Count valid protocols */
	क्रम(protocol_गिनती=0; protocols[protocol_गिनती].name; protocol_गिनती++){
		;
	}

	p = mosquitto_calloc(protocol_गिनती+1, माप(काष्ठा lws_protocols));
	अगर(!p){
		log__printf(शून्य, MOSQ_LOG_ERR, "Out of memory.");
		वापस;
	}
	क्रम(i=0; protocols[i].name; i++){
		p[i].name = protocols[i].name;
		p[i].callback = protocols[i].callback;
		p[i].per_session_तथ्य_आकार = protocols[i].per_session_तथ्य_आकार;
		p[i].rx_बफर_आकार = protocols[i].rx_बफर_आकार;
		p[i].user = listener;
	}

	स_रखो(&info, 0, माप(info));
	info.iface = listener->host;
	info.port = listener->port;
	info.protocols = p;
	info.gid = -1;
	info.uid = -1;
#अगर_घोषित WITH_TLS
	अगर(listener->cafile){
		info.ssl_ca_filepath = listener->cafile;
	}अन्यथा अगर(listener->capath){
		log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: CA path option is not supported for websockets");
	}
	info.ssl_cert_filepath = listener->certfile;
	info.ssl_private_कुंजी_filepath = listener->keyfile;
	info.ssl_cipher_list = listener->ciphers;
	/* HTTP 1 only, due to HTTP 2 issues in Firefox:
	   https://github.com/eclipse-mosquitto/mosquitto/issues/1211
	*/
	info.alpn = "h1";
#अगर defined(WITH_WEBSOCKETS) && LWS_LIBRARY_VERSION_संख्या>=3001000
	info.tls1_3_plus_cipher_list = listener->ciphers_tls13;
#पूर्ण_अगर
	अगर(listener->require_certificate){
		info.options |= LWS_SERVER_OPTION_REQUIRE_VALID_OPENSSL_CLIENT_CERT;
	}
#पूर्ण_अगर

	info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
	अगर(listener->socket_domain == AF_INET){
		info.options |= LWS_SERVER_OPTION_DISABLE_IPV6;
	}
	info.max_http_header_तथ्य = conf->packet_बफर_आकार;

	user = mosquitto_calloc(1, माप(काष्ठा libws_mqtt_hack));
	अगर(!user){
		mosquitto_FREE(p);
		log__printf(शून्य, MOSQ_LOG_ERR, "Out of memory.");
		वापस;
	}

	अगर(listener->http_dir){
#अगर_घोषित WIN32
		user->http_dir = _fullpath(शून्य, listener->http_dir, 0);
#अन्यथा
		user->http_dir = realpath(listener->http_dir, शून्य);
#पूर्ण_अगर
		अगर(!user->http_dir){
			mosquitto_FREE(user);
			mosquitto_FREE(p);
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to open http dir \"%s\".", listener->http_dir);
			वापस;
		}
	}
	user->listener = listener;

	info.user = user;
	info.pt_serv_buf_आकार = WS_SERV_BUF_आकार;
	listener->ws_protocol = p;

	lws_set_log_level(conf->websockets_log_level, log_wrap);

	log__printf(शून्य, MOSQ_LOG_INFO, "Opening websockets listen socket on port %d.", listener->port);
	listener->ws_in_init = true;
	listener->ws_context = lws_create_context(&info);
	listener->ws_in_init = false;
}

#पूर्ण_अगर
#पूर्ण_अगर
