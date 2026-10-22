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

#ifdef WITH_WEBSOCKETS

#अन्तर्भाव "config.h"

#सचे defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif
#अन्तर्भाव "mosquitto_internal.h"
#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "packet_mosq.h"
#अन्तर्भाव "sys_tree.h"
#अन्तर्भाव "util_mosq.h"

#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <errno.h>
#अन्तर्भाव <sys/stat.h>

#ifndef WIN32
#  include <sys/socket.h>
#endif

#सचे defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
/* Be careful if changing these, if TX is not bigger than SERV then there can
 * be very large write performance penalties.
 */
#define WS_SERV_BUF_SIZE 4096
#define WS_TX_BUF_SIZE (WS_SERV_BUF_SIZE*2)

स्थिर पूर्णाङ्क callback_mqtt(
		संरचना lws *wsi,
		गणना lws_callback_reasons reason,
		शून्य *user,
		शून्य *in,
		size_t len);

स्थिर पूर्णाङ्क callback_http(
		संरचना lws *wsi,
		गणना lws_callback_reasons reason,
		शून्य *user,
		शून्य *in,
		size_t len);

गणना mosq_ws_protocols {
	PROTOCOL_HTTP = 0,
	PROTOCOL_MQTT,
	DEMO_PROTOCOL_गणनम्,
};

संरचना libws_http_data {
	FILE *fptr;
};

स्थिर संरचना lws_protocols protocols[] = {
	/* first protocol must always be HTTP handler */
	{
		"http-only",                        /* name */
		callback_http,                      /* lws_callback_function */
		परिमाण (संरचना libws_http_data),    /* per_session_data_size */
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		रिक्त,                               /* user v1.4 on */
		WS_TX_BUF_SIZE                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqtt",
		callback_mqtt,
		परिमाण(संरचना libws_mqtt_data),
		0,                                  /* rx_buffer_size */
		1,                                  /* id */
		रिक्त,                               /* user v1.4 on */
		WS_TX_BUF_SIZE                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqttv3.1",
		callback_mqtt,
		परिमाण(संरचना libws_mqtt_data),
		0,                                  /* rx_buffer_size */
		2,                                  /* id */
		रिक्त,                               /* user v1.4 on */
		WS_TX_BUF_SIZE                      /* tx_packet_size v2.3.0 */
	},
	{
		रिक्त,
		रिक्त,
		0,
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		रिक्त,                               /* user v1.4 on */
		0                                   /* tx_packet_size v2.3.0 */
	}
};


स्थिर शून्य easy_address(पूर्णाङ्क sock, संरचना mosquitto *mosq)
{
	वर्ण address[1024];

	सचे(!net__socket_get_address(sock, address, 1024, &mosq->remote_port)){
		mosq->address = mosquitto_strdup(address);
	}
}


स्थिर पूर्णाङ्क callback_mqtt(
		संरचना lws *wsi,
		गणना lws_callback_reasons reason,
		शून्य *user,
		शून्य *in,
		size_t len)
{
	संरचना mosquitto *mosq = रिक्त;
	संरचना mosquitto__packet *packet;
	पूर्णाङ्क गणनम्;
	unsigned पूर्णाङ्क ucount;
	ध्रुव संरचना lws_protocols *p;
	संरचना libws_mqtt_data *u = (संरचना libws_mqtt_data *)user;
	size_t pos;
	uint8_t *buf;
	पूर्णाङ्क rc;
	uint8_t byte;
	वर्ण ip_addr_buff[1024];

	विकल्प(reason){
		पक्ष LWS_CALLBACK_ESTABLISHED:
			mosq = context__init();
			सचे(mosq){
				p = lws_get_protocol(wsi);
				mosq->listener = p->user;
				सचे(!mosq->listener){
					mosquitto_FREE(mosq);
					पटिनिवत्त -1;
				}
				mosq->wsi = wsi;
#ifdef WITH_TLS
				सचे(in){
					mosq->ssl = (SSL *)in;
					सचे(!mosq->listener->ssl_ctx){
						mosq->listener->ssl_ctx = SSL_get_SSL_CTX(mosq->ssl);
					}
				}
#endif
				u->mosq = mosq;
			}अञ्ञथा{
				पटिनिवत्त -1;
			}

			easy_address(lws_get_socket_fd(wsi), mosq);
			सचे(!mosq->address){
				/* getpeername and inet_ntop failed and not a bridge */
				mosquitto_FREE(mosq);
				u->mosq = रिक्त;
				पटिनिवत्त -1;
			}
			mosq->listener->client_गणनम्++;
			सचे((mosq->listener->max_connections > 0 && mosq->listener->client_गणनम् > mosq->listener->max_connections)
					|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (unsigned पूर्णाङ्क)db.config->global_max_connections)){

				सचे(db.config->connection_messages == true){
					log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", mosq->address);
				}
				mosq->listener->client_गणनम्--;
				mosquitto_FREE(mosq->address);
				mosquitto_FREE(mosq);
				u->mosq = रिक्त;
				पटिनिवत्त -1;
			}
			mosq->sock = lws_get_socket_fd(wsi);
			HASH_ADD(hh_sock, db.contexts_by_sock, sock, परिमाण(mosq->sock), mosq);
			mux__new(mosq);
			भङ्ग;

		पक्ष LWS_CALLBACK_CLOSED:
			सचे(!u){
				पटिनिवत्त -1;
			}
			mosq = u->mosq;
			सचे(mosq){
				सचे(mosq->sock != INVALID_SOCKET){
					HASH_DELETE(hh_sock, db.contexts_by_sock, mosq);
					mosq->sock = INVALID_SOCKET;
					mux__delete(mosq);
				}
				mosq->wsi = रिक्त;
#ifdef WITH_TLS
				mosq->ssl = रिक्त;
#endif
				do_disconnect(mosq, MOSQ_ERR_CONN_LOST);
			}
			भङ्ग;

		पक्ष LWS_CALLBACK_SERVER_WRITEABLE:
			सचे(!u){
				पटिनिवत्त -1;
			}
			mosq = u->mosq;
			सचे(!mosq){
				पटिनिवत्त -1;
			}

			rc = db__message_write_inflight_out_latest(mosq);
			सचे(rc){
				पटिनिवत्त -1;
			}
			rc = db__message_write_queued_out(mosq);
			सचे(rc){
				पटिनिवत्त -1;
			}

			याव(mosq->out_packet && !lws_send_pipe_choked(mosq->wsi)){
				packet = mosq->out_packet;
				गणनम् = lws_write(wsi, &packet->payload[packet->pos], packet->to_process, LWS_WRITE_द्विमान);
				सचे(गणनम् < 0){
					सचे(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						पटिनिवत्त -1;
					}
					पटिनिवत्त 0;
				}
				ucount = (unsigned पूर्णाङ्क)गणनम्;
#ifdef WITH_SYS_वृक्षः
				metrics__int_inc(mosq_counter_bytes_sent, ucount);
#endif
				packet->to_process -= ucount;
				packet->pos += ucount;
				सचे(packet->to_process > 0){
					सचे(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						पटिनिवत्त -1;
					}
					भङ्ग;
				}

#ifdef WITH_SYS_वृक्षः
				metrics__int_inc(mosq_counter_messages_sent, 1);
				सचे(((packet->command)&0xF0) == CMD_PUBLISH){
					metrics__int_inc(mosq_counter_mqtt_publish_sent, 1);
				}
#endif

				packet__get_next_out(mosq);
				mosquitto_FREE(packet);

				mosq->next_सन्देशः_out = db.now_s + mosq->keepalive;
			}
			सचे(mosq->state == mosq_cs_disconnect_ws
					|| mosq->state == mosq_cs_disconnecting
					|| mosq->state == mosq_cs_disused){

				पटिनिवत्त -1;
			}
			सचे(mosq->out_packet){
				lws_callback_on_writable(mosq->wsi);
			}
			भङ्ग;

		पक्ष LWS_CALLBACK_RECEIVE:
			सचे(!u || !u->mosq){
				पटिनिवत्त -1;
			}
			mosq = u->mosq;
			pos = 0;
			buf = (uint8_t *)in;
			metrics__int_inc(mosq_counter_bytes_received, (int64_t)len);
			याव(pos < len){
				सचे(!mosq->in_packet.command){
					mosq->in_packet.command = buf[pos];
					pos++;
					/* Clients must send CONNECT as their first command. */
					सचे(mosq->state == mosq_cs_new && (mosq->in_packet.command&0xF0) != CMD_CONNECT){
						पटिनिवत्त -1;
					}
				}
				सचे(mosq->in_packet.remaining_गणनम् <= 0){
					करोहि{
						सचे(pos == len){
							पटिनिवत्त 0;
						}
						byte = buf[pos];
						pos++;

						mosq->in_packet.remaining_गणनम्--;
						/* Max 4 bytes length for remaining length as defined by protocol.
						* Anything more likely means a broken/malicious client.
						*/
						सचे(mosq->in_packet.remaining_गणनम् < -4){
							पटिनिवत्त -1;
						}

						mosq->in_packet.remaining_length += (byte & 127) * mosq->in_packet.remaining_mult;
						mosq->in_packet.remaining_mult *= 128;
					}याव((byte & 128) != 0);
					mosq->in_packet.remaining_गणनम् = (int8_t)(mosq->in_packet.remaining_गणनम् * -1);

					सचे(mosq->in_packet.remaining_length > 0){
						mosq->in_packet.payload = mosquitto_malloc(mosq->in_packet.remaining_length*परिमाण(uint8_t));
						सचे(!mosq->in_packet.payload){
							पटिनिवत्त -1;
						}
						mosq->in_packet.to_process = mosq->in_packet.remaining_length;
					}
				}
				सचे(mosq->in_packet.to_process>0){
					सचे((uint32_t)len - pos >= mosq->in_packet.to_process){
						memcpy(&mosq->in_packet.payload[mosq->in_packet.pos], &buf[pos], mosq->in_packet.to_process);
						mosq->in_packet.pos += mosq->in_packet.to_process;
						pos += mosq->in_packet.to_process;
						mosq->in_packet.to_process = 0;
					}अञ्ञथा{
						memcpy(&mosq->in_packet.payload[mosq->in_packet.pos], &buf[pos], len-pos);
						mosq->in_packet.pos += (uint32_t)(len-pos);
						mosq->in_packet.to_process -= (uint32_t)(len-pos);
						पटिनिवत्त 0;
					}
				}
				/* All data for this packet is read. */
				mosq->in_packet.pos = 0;

#ifdef WITH_SYS_वृक्षः
				metrics__int_inc(mosq_counter_messages_received, 1);
#endif
				rc = handle__packet(mosq);

				/* Free data and reset values */
				packet__cleanup(&mosq->in_packet);

				keepalive__update(mosq);

				सचे(rc && mosq->out_packet){
					सचे(mosq->state != mosq_cs_disconnecting){
						mosquitto__set_state(mosq, mosq_cs_disconnect_ws);
					}
					lws_callback_on_writable(mosq->wsi);
				}अञ्ञथा सचे(rc){
					do_disconnect(mosq, MOSQ_ERR_CONN_LOST);
					पटिनिवत्त -1;
				}
			}
			भङ्ग;

		स्वभाव:
			भङ्ग;
	}

	पटिनिवत्त 0;
}


स्थिर वर्ण *http__canonical_filename(
		संरचना lws *wsi,
		ध्रुव वर्ण *in,
		ध्रुव वर्ण *http_dir)
{
	size_t inlen, slen;
	वर्ण *filename, *filename_canonical;

	inlen = strlen(in);
	सचे(in[inlen-1] == '/'){
		slen = strlen(http_dir) + inlen + strlen("/index.html") + 2;
	}अञ्ञथा{
		slen = strlen(http_dir) + inlen + 2;
	}
	filename = mosquitto_malloc(slen);
	सचे(!filename){
		lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, रिक्त);
		पटिनिवत्त रिक्त;
	}
	सचे(((वर्ण *)in)[inlen-1] == '/'){
		snprintf(filename, slen, "%s%sindex.html", http_dir, (वर्ण *)in);
	}अञ्ञथा{
		snprintf(filename, slen, "%s%s", http_dir, (वर्ण *)in);
	}


	/* Get canonical path and check it is within our http_dir */
#ifdef WIN32
	filename_canonical = _fullpath(रिक्त, filename, 0);
	mosquitto_FREE(filename);
	सचे(!filename_canonical){
		lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, रिक्त);
		पटिनिवत्त रिक्त;
	}
#अञ्ञथा
	filename_canonical = realpath(filename, रिक्त);
	mosquitto_FREE(filename);
	सचे(!filename_canonical){
		सचे(errno == EACCES){
			lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, रिक्त);
		}अञ्ञथा सचे(errno == EINVAL || errno == EIO || errno == ELOOP){
			lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, रिक्त);
		}अञ्ञथा सचे(errno == ENAMETOOLONG){
			lws_return_http_status(wsi, HTTP_STATUS_REQ_URI_TOO_LONG, रिक्त);
		}अञ्ञथा सचे(errno == ENOENT || errno == ENOTDIR){
			lws_return_http_status(wsi, HTTP_STATUS_NOT_FOUND, रिक्त);
		}
		पटिनिवत्त रिक्त;
	}
#endif
	सचे(strncmp(http_dir, filename_canonical, strlen(http_dir))){
		/* Requested file isn't within http_dir, deny access. */
		SAFE_FREE(filename_canonical);
		lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, रिक्त);
		पटिनिवत्त रिक्त;
	}

	पटिनिवत्त filename_canonical;
}


स्थिर पूर्णाङ्क callback_http(
		संरचना lws *wsi,
		गणना lws_callback_reasons reason,
		शून्य *user,
		शून्य *in,
		size_t len)
{
	संरचना libws_http_data *u = (संरचना libws_http_data *)user;
	संरचना libws_mqtt_hack *hack;
	वर्ण *http_dir;
	size_t buflen;
	size_t wlen;
	पूर्णाङ्क rc;
	वर्ण *filename_canonical;
	unsigned वर्ण buf[4096];
	संरचना stat filestat;
	संरचना mosquitto *mosq;
	संरचना lws_pollargs *pollargs = (संरचना lws_pollargs *)in;
	पूर्णाङ्क hlen;

	/* FIXME - ssl cert verification is done here. */

	विकल्प(reason){
		पक्ष LWS_CALLBACK_HTTP:
			सचे(!u){
				पटिनिवत्त -1;
			}

			hack = (संरचना libws_mqtt_hack *)lws_context_user(lws_get_context(wsi));
			सचे(!hack){
				पटिनिवत्त -1;
			}
			http_dir = hack->http_dir;

			सचे(!http_dir){
				/* http disabled */
				पटिनिवत्त -1;
			}

			/* Forbid POST */
			सचे(lws_hdr_total_length(wsi, WSI_TOKEN_POST_URI)){
				lws_return_http_status(wsi, HTTP_STATUS_METHOD_NOT_ALLOWED, रिक्त);
				पटिनिवत्त -1;
			}

			filename_canonical = http__canonical_filename(wsi, (वर्ण *)in, http_dir);
			सचे(!filename_canonical){
				पटिनिवत्त -1;
			}

			u->fptr = fopen(filename_canonical, "rb");
			सचे(!u->fptr){
				SAFE_FREE(filename_canonical);
				lws_return_http_status(wsi, HTTP_STATUS_NOT_FOUND, रिक्त);
				पटिनिवत्त -1;
			}
			सचे(fstat(fileno(u->fptr), &filestat) < 0){
				SAFE_FREE(filename_canonical);
				lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, रिक्त);
				fclose(u->fptr);
				u->fptr = रिक्त;
				पटिनिवत्त -1;
			}


			सचे((filestat.st_mode & S_IFDIR) == S_IFDIR){
				fclose(u->fptr);
				u->fptr = रिक्त;
				SAFE_FREE(filename_canonical);

				/* FIXME - use header functions from lws 2.x */
				buflen = (size_t)snprintf((वर्ण *)buf, 4096, "HTTP/1.0 302 OK\r\n"
						"Location: %s/\r\n\r\n",
						(वर्ण *)in);
				पटिनिवत्त lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
			}

			सचे((filestat.st_mode & S_IFREG) != S_IFREG){
				lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, रिक्त);
				fclose(u->fptr);
				u->fptr = रिक्त;
				SAFE_FREE(filename_canonical);
				पटिनिवत्त -1;
			}

			log__printf(रिक्त, MOSQ_LOG_DEBUG, "http serving file \"%s\".", filename_canonical);
			SAFE_FREE(filename_canonical);
			/* FIXME - use header functions from lws 2.x */
			buflen = (size_t)snprintf((वर्ण *)buf, 4096, "HTTP/1.0 200 OK\r\n"
					"Server: mosquitto\r\n"
					"Content-Length: %u\r\n\r\n",
					(unsigned पूर्णाङ्क)filestat.st_size);
			सचे(lws_write(wsi, buf, buflen, LWS_WRITE_HTTP) < 0){
				fclose(u->fptr);
				u->fptr = रिक्त;
				पटिनिवत्त -1;
			}
			lws_callback_on_writable(wsi);
			भङ्ग;

		पक्ष LWS_CALLBACK_HTTP_BODY:
			/* For extra POST data? */
			पटिनिवत्त -1;

		पक्ष LWS_CALLBACK_HTTP_BODY_COMPLETION:
			/* For end of extra POST data? */
			पटिनिवत्त -1;

		पक्ष LWS_CALLBACK_FILTER_HTTP_CONNECTION:
			/* Access control here */
			पटिनिवत्त 0;

#सचे LWS_LIBRARY_VERSION_सङ्ख्या >= 3001000
		पक्ष LWS_CALLBACK_HTTP_CONFIRM_UPGRADE:
			hack = lws_context_user(lws_get_context(wsi));
			सचे(hack->listener->ws_origin_गणनम्){
				hlen = lws_hdr_total_length(wsi, WSI_TOKEN_ORIGIN);
				सचे(hlen <= 0 || hlen >= (पूर्णाङ्क)परिमाण(buf)){
					पटिनिवत्त -1;
				}
				अत्थाय(पूर्णाङ्क i=0; i<hack->listener->ws_origin_गणनम्; i++){
					lws_hdr_copy(wsi, (वर्ण *)buf, परिमाण(buf), WSI_TOKEN_ORIGIN);
					सचे((!strcmp(hack->listener->ws_origins[i], (वर्ण *)buf))){
						पटिनिवत्त 0;
					}
				}
				पटिनिवत्त -1;
			}अञ्ञथा{
				पटिनिवत्त 0;
			}
#endif

		पक्ष LWS_CALLBACK_HTTP_WRITEABLE:
			/* Send our data here */
			सचे(u && u->fptr){
				करोहि{
					buflen = fread(buf, 1, परिमाण(buf), u->fptr);
					सचे(buflen < 1){
						fclose(u->fptr);
						u->fptr = रिक्त;
						पटिनिवत्त -1;
					}
					rc = lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
					सचे(rc < 0){
						पटिनिवत्त -1;
					}
					wlen = (size_t)rc;
					/* while still active, extend timeout */
					सचे(wlen){
						lws_set_timeout(wsi, PENDING_TIMEOUT_HTTP_CONTENT, 10);
					}
					सचे(wlen < buflen){
						सचे(fseek(u->fptr, (long)(buflen-wlen), SEEK_CUR) < 0){
							fclose(u->fptr);
							u->fptr = रिक्त;
							पटिनिवत्त -1;
						}
					}अञ्ञथा{
						सचे(buflen < परिमाण(buf)){
							fclose(u->fptr);
							u->fptr = रिक्त;
						}
					}
				}याव(u->fptr && !lws_send_pipe_choked(wsi));
				lws_callback_on_writable(wsi);
			}अञ्ञथा{
				पटिनिवत्त -1;
			}
			भङ्ग;

		पक्ष LWS_CALLBACK_CLOSED:
		पक्ष LWS_CALLBACK_CLOSED_HTTP:
		पक्ष LWS_CALLBACK_HTTP_FILE_COMPLETION:
			सचे(u && u->fptr){
				fclose(u->fptr);
				u->fptr = रिक्त;
			}
			भङ्ग;

		पक्ष LWS_CALLBACK_ADD_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, परिमाण(pollargs->fd), mosq);
			सचे(mosq){
				सचे(pollargs->events & LWS_POLLOUT){
					mux__add_out(mosq);
				}अञ्ञथा{
					mux__remove_out(mosq);
				}
			}अञ्ञथा{
				सचे(pollargs->events & POLLIN){
					/* Assume this is a new listener */
					listeners__add_websockets(lws_get_context(wsi), pollargs->fd);
				}
			}
			भङ्ग;

		पक्ष LWS_CALLBACK_DEL_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, परिमाण(pollargs->fd), mosq);
			सचे(mosq){
				mux__delete(mosq);
			}
			भङ्ग;

		पक्ष LWS_CALLBACK_CHANGE_MODE_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, परिमाण(pollargs->fd), mosq);
			सचे(mosq){
				सचे(pollargs->events & LWS_POLLHUP){
					पटिनिवत्त 1;
				}अञ्ञथा सचे(pollargs->events & LWS_POLLOUT){
					mux__add_out(mosq);
				}अञ्ञथा{
					mux__remove_out(mosq);
				}
			}
			भङ्ग;

#ifdef WITH_TLS
		पक्ष LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION:
			सचे(!len || (SSL_get_verify_result((SSL *)in) != X509_V_OK)){
				पटिनिवत्त 1;
			}
			भङ्ग;
#endif

		स्वभाव:
			पटिनिवत्त 0;
	}

	पटिनिवत्त 0;
}


स्थिर शून्य log_wrap(पूर्णाङ्क level, ध्रुव वर्ण *पङ्क्तिः)
{
	वर्ण *l = (वर्ण *)पङ्क्तिः;
	UNUSED(level);
	l[strlen(पङ्क्तिः)-1] = '\0'; /* Remove \n */
	log__printf(रिक्त, MOSQ_LOG_WEBSOCKETS, "%s", l);
}


शून्य mosq_websockets_init(संरचना mosquitto__listener *listener, ध्रुव संरचना mosquitto__config *conf)
{
	संरचना lws_context_creation_info info;
	संरचना lws_protocols *p;
	size_t protocol_गणनम्;
	पूर्णाङ्क i;
	संरचना libws_mqtt_hack *user;

	/* Count valid protocols */
	अत्थाय(protocol_गणनम्=0; protocols[protocol_गणनम्].name; protocol_गणनम्++){
		;
	}

	p = mosquitto_calloc(protocol_गणनम्+1, परिमाण(संरचना lws_protocols));
	सचे(!p){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Out of memory.");
		पटिनिवत्त;
	}
	अत्थाय(i=0; protocols[i].name; i++){
		p[i].name = protocols[i].name;
		p[i].callback = protocols[i].callback;
		p[i].per_session_data_size = protocols[i].per_session_data_size;
		p[i].rx_buffer_size = protocols[i].rx_buffer_size;
		p[i].user = listener;
	}

	memset(&info, 0, परिमाण(info));
	info.iface = listener->host;
	info.port = listener->port;
	info.protocols = p;
	info.gid = -1;
	info.uid = -1;
#ifdef WITH_TLS
	सचे(listener->cafile){
		info.ssl_ca_filepath = listener->cafile;
	}अञ्ञथा सचे(listener->capath){
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: CA path option is not supported for websockets");
	}
	info.ssl_cert_filepath = listener->certfile;
	info.ssl_private_कुञ्चिका_filepath = listener->keyfile;
	info.ssl_cipher_list = listener->ciphers;
	/* HTTP 1 only, due to HTTP 2 issues in Firefox:
	   https://github.com/eclipse-mosquitto/mosquitto/issues/1211
	*/
	info.alpn = "h1";
#सचे defined(WITH_WEBSOCKETS) && LWS_LIBRARY_VERSION_सङ्ख्या>=3001000
	info.tls1_3_plus_cipher_list = listener->ciphers_tls13;
#endif
	सचे(listener->require_certificate){
		info.options |= LWS_SERVER_OPTION_REQUIRE_VALID_OPENSSL_CLIENT_CERT;
	}
#endif

	info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
	सचे(listener->socket_domain == AF_INET){
		info.options |= LWS_SERVER_OPTION_DISABLE_IPV6;
	}
	info.max_http_header_data = conf->packet_buffer_size;

	user = mosquitto_calloc(1, परिमाण(संरचना libws_mqtt_hack));
	सचे(!user){
		mosquitto_FREE(p);
		log__printf(रिक्त, MOSQ_LOG_ERR, "Out of memory.");
		पटिनिवत्त;
	}

	सचे(listener->http_dir){
#ifdef WIN32
		user->http_dir = _fullpath(रिक्त, listener->http_dir, 0);
#अञ्ञथा
		user->http_dir = realpath(listener->http_dir, रिक्त);
#endif
		सचे(!user->http_dir){
			mosquitto_FREE(user);
			mosquitto_FREE(p);
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to open http dir \"%s\".", listener->http_dir);
			पटिनिवत्त;
		}
	}
	user->listener = listener;

	info.user = user;
	info.pt_serv_buf_size = WS_SERV_BUF_SIZE;
	listener->ws_protocol = p;

	lws_set_log_level(conf->websockets_log_level, log_wrap);

	log__printf(रिक्त, MOSQ_LOG_INFO, "Opening websockets listen socket on port %d.", listener->port);
	listener->ws_in_init = true;
	listener->ws_context = lws_create_context(&info);
	listener->ws_in_init = false;
}

#endif
#endif
