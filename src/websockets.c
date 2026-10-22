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

#அந்தர்பாவ "config.h"

#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif
#அந்தர்பாவ "mosquitto_internal.h"
#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto/mqtt_protocol.h"
#அந்தர்பாவ "packet_mosq.h"
#அந்தர்பாவ "sys_tree.h"
#அந்தர்பாவ "util_mosq.h"

#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <errno.h>
#அந்தர்பாவ <sys/stat.h>

#ifndef WIN32
#  include <sys/socket.h>
#endif

#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
/* Be careful if changing these, if TX is not bigger than SERV then there can
 * be very large write performance penalties.
 */
#define WS_SERV_BUF_SIZE 4096
#define WS_TX_BUF_SIZE (WS_SERV_BUF_SIZE*2)

நிலையான முழு callback_mqtt(
		கட்டமைப்பு lws *wsi,
		எண்ணிக்கை lws_callback_reasons reason,
		வெறுமை *user,
		வெறுமை *in,
		size_t len);

நிலையான முழு callback_http(
		கட்டமைப்பு lws *wsi,
		எண்ணிக்கை lws_callback_reasons reason,
		வெறுமை *user,
		வெறுமை *in,
		size_t len);

எண்ணிக்கை mosq_ws_protocols {
	PROTOCOL_HTTP = 0,
	PROTOCOL_MQTT,
	DEMO_PROTOCOL_மொத்தம்,
};

கட்டமைப்பு libws_http_data {
	FILE *fptr;
};

நிலையான கட்டமைப்பு lws_protocols protocols[] = {
	/* first protocol must always be HTTP handler */
	{
		"http-only",                        /* name */
		callback_http,                      /* lws_callback_function */
		அளவு (கட்டமைப்பு libws_http_data),    /* per_session_data_size */
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		வெற்று,                               /* user v1.4 on */
		WS_TX_BUF_SIZE                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqtt",
		callback_mqtt,
		அளவு(கட்டமைப்பு libws_mqtt_data),
		0,                                  /* rx_buffer_size */
		1,                                  /* id */
		வெற்று,                               /* user v1.4 on */
		WS_TX_BUF_SIZE                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqttv3.1",
		callback_mqtt,
		அளவு(கட்டமைப்பு libws_mqtt_data),
		0,                                  /* rx_buffer_size */
		2,                                  /* id */
		வெற்று,                               /* user v1.4 on */
		WS_TX_BUF_SIZE                      /* tx_packet_size v2.3.0 */
	},
	{
		வெற்று,
		வெற்று,
		0,
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		வெற்று,                               /* user v1.4 on */
		0                                   /* tx_packet_size v2.3.0 */
	}
};


நிலையான வெறுமை easy_address(முழு sock, கட்டமைப்பு mosquitto *mosq)
{
	எழுத்து address[1024];

	எனில்(!net__socket_get_address(sock, address, 1024, &mosq->remote_port)){
		mosq->address = mosquitto_strdup(address);
	}
}


நிலையான முழு callback_mqtt(
		கட்டமைப்பு lws *wsi,
		எண்ணிக்கை lws_callback_reasons reason,
		வெறுமை *user,
		வெறுமை *in,
		size_t len)
{
	கட்டமைப்பு mosquitto *mosq = வெற்று;
	கட்டமைப்பு mosquitto__packet *packet;
	முழு மொத்தம்;
	unsigned முழு ucount;
	மாறிலி கட்டமைப்பு lws_protocols *p;
	கட்டமைப்பு libws_mqtt_data *u = (கட்டமைப்பு libws_mqtt_data *)user;
	size_t pos;
	uint8_t *buf;
	முழு rc;
	uint8_t byte;
	எழுத்து ip_addr_buff[1024];

	தேர்வு(reason){
		நிலை LWS_CALLBACK_ESTABLISHED:
			mosq = context__init();
			எனில்(mosq){
				p = lws_get_protocol(wsi);
				mosq->listener = p->user;
				எனில்(!mosq->listener){
					mosquitto_FREE(mosq);
					திரும்பு -1;
				}
				mosq->wsi = wsi;
#ifdef WITH_TLS
				எனில்(in){
					mosq->ssl = (SSL *)in;
					எனில்(!mosq->listener->ssl_ctx){
						mosq->listener->ssl_ctx = SSL_get_SSL_CTX(mosq->ssl);
					}
				}
#endif
				u->mosq = mosq;
			}இல்லையெனில்{
				திரும்பு -1;
			}

			easy_address(lws_get_socket_fd(wsi), mosq);
			எனில்(!mosq->address){
				/* getpeername and inet_ntop failed and not a bridge */
				mosquitto_FREE(mosq);
				u->mosq = வெற்று;
				திரும்பு -1;
			}
			mosq->listener->client_மொத்தம்++;
			எனில்((mosq->listener->max_connections > 0 && mosq->listener->client_மொத்தம் > mosq->listener->max_connections)
					|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (unsigned முழு)db.config->global_max_connections)){

				எனில்(db.config->connection_messages == true){
					log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", mosq->address);
				}
				mosq->listener->client_மொத்தம்--;
				mosquitto_FREE(mosq->address);
				mosquitto_FREE(mosq);
				u->mosq = வெற்று;
				திரும்பு -1;
			}
			mosq->sock = lws_get_socket_fd(wsi);
			HASH_ADD(hh_sock, db.contexts_by_sock, sock, அளவு(mosq->sock), mosq);
			mux__new(mosq);
			நிறுத்து;

		நிலை LWS_CALLBACK_CLOSED:
			எனில்(!u){
				திரும்பு -1;
			}
			mosq = u->mosq;
			எனில்(mosq){
				எனில்(mosq->sock != INVALID_SOCKET){
					HASH_DELETE(hh_sock, db.contexts_by_sock, mosq);
					mosq->sock = INVALID_SOCKET;
					mux__delete(mosq);
				}
				mosq->wsi = வெற்று;
#ifdef WITH_TLS
				mosq->ssl = வெற்று;
#endif
				do_disconnect(mosq, MOSQ_ERR_CONN_LOST);
			}
			நிறுத்து;

		நிலை LWS_CALLBACK_SERVER_WRITEABLE:
			எனில்(!u){
				திரும்பு -1;
			}
			mosq = u->mosq;
			எனில்(!mosq){
				திரும்பு -1;
			}

			rc = db__message_write_inflight_out_latest(mosq);
			எனில்(rc){
				திரும்பு -1;
			}
			rc = db__message_write_queued_out(mosq);
			எனில்(rc){
				திரும்பு -1;
			}

			போது(mosq->out_packet && !lws_send_pipe_choked(mosq->wsi)){
				packet = mosq->out_packet;
				மொத்தம் = lws_write(wsi, &packet->payload[packet->pos], packet->to_process, LWS_WRITE_இருமம்);
				எனில்(மொத்தம் < 0){
					எனில்(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						திரும்பு -1;
					}
					திரும்பு 0;
				}
				ucount = (unsigned முழு)மொத்தம்;
#ifdef WITH_SYS_மரம்
				metrics__int_inc(mosq_counter_bytes_sent, ucount);
#endif
				packet->to_process -= ucount;
				packet->pos += ucount;
				எனில்(packet->to_process > 0){
					எனில்(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						திரும்பு -1;
					}
					நிறுத்து;
				}

#ifdef WITH_SYS_மரம்
				metrics__int_inc(mosq_counter_messages_sent, 1);
				எனில்(((packet->command)&0xF0) == CMD_PUBLISH){
					metrics__int_inc(mosq_counter_mqtt_publish_sent, 1);
				}
#endif

				packet__get_next_out(mosq);
				mosquitto_FREE(packet);

				mosq->next_செய்தி_out = db.now_s + mosq->keepalive;
			}
			எனில்(mosq->state == mosq_cs_disconnect_ws
					|| mosq->state == mosq_cs_disconnecting
					|| mosq->state == mosq_cs_disused){

				திரும்பு -1;
			}
			எனில்(mosq->out_packet){
				lws_callback_on_writable(mosq->wsi);
			}
			நிறுத்து;

		நிலை LWS_CALLBACK_RECEIVE:
			எனில்(!u || !u->mosq){
				திரும்பு -1;
			}
			mosq = u->mosq;
			pos = 0;
			buf = (uint8_t *)in;
			metrics__int_inc(mosq_counter_bytes_received, (int64_t)len);
			போது(pos < len){
				எனில்(!mosq->in_packet.command){
					mosq->in_packet.command = buf[pos];
					pos++;
					/* Clients must send CONNECT as their first command. */
					எனில்(mosq->state == mosq_cs_new && (mosq->in_packet.command&0xF0) != CMD_CONNECT){
						திரும்பு -1;
					}
				}
				எனில்(mosq->in_packet.remaining_மொத்தம் <= 0){
					செய்{
						எனில்(pos == len){
							திரும்பு 0;
						}
						byte = buf[pos];
						pos++;

						mosq->in_packet.remaining_மொத்தம்--;
						/* Max 4 bytes length for remaining length as defined by protocol.
						* Anything more likely means a broken/malicious client.
						*/
						எனில்(mosq->in_packet.remaining_மொத்தம் < -4){
							திரும்பு -1;
						}

						mosq->in_packet.remaining_length += (byte & 127) * mosq->in_packet.remaining_mult;
						mosq->in_packet.remaining_mult *= 128;
					}போது((byte & 128) != 0);
					mosq->in_packet.remaining_மொத்தம் = (int8_t)(mosq->in_packet.remaining_மொத்தம் * -1);

					எனில்(mosq->in_packet.remaining_length > 0){
						mosq->in_packet.payload = mosquitto_malloc(mosq->in_packet.remaining_length*அளவு(uint8_t));
						எனில்(!mosq->in_packet.payload){
							திரும்பு -1;
						}
						mosq->in_packet.to_process = mosq->in_packet.remaining_length;
					}
				}
				எனில்(mosq->in_packet.to_process>0){
					எனில்((uint32_t)len - pos >= mosq->in_packet.to_process){
						memcpy(&mosq->in_packet.payload[mosq->in_packet.pos], &buf[pos], mosq->in_packet.to_process);
						mosq->in_packet.pos += mosq->in_packet.to_process;
						pos += mosq->in_packet.to_process;
						mosq->in_packet.to_process = 0;
					}இல்லையெனில்{
						memcpy(&mosq->in_packet.payload[mosq->in_packet.pos], &buf[pos], len-pos);
						mosq->in_packet.pos += (uint32_t)(len-pos);
						mosq->in_packet.to_process -= (uint32_t)(len-pos);
						திரும்பு 0;
					}
				}
				/* All data for this packet is read. */
				mosq->in_packet.pos = 0;

#ifdef WITH_SYS_மரம்
				metrics__int_inc(mosq_counter_messages_received, 1);
#endif
				rc = handle__packet(mosq);

				/* Free data and reset values */
				packet__cleanup(&mosq->in_packet);

				keepalive__update(mosq);

				எனில்(rc && mosq->out_packet){
					எனில்(mosq->state != mosq_cs_disconnecting){
						mosquitto__set_state(mosq, mosq_cs_disconnect_ws);
					}
					lws_callback_on_writable(mosq->wsi);
				}இல்லையெனில் எனில்(rc){
					do_disconnect(mosq, MOSQ_ERR_CONN_LOST);
					திரும்பு -1;
				}
			}
			நிறுத்து;

		இயல்பு:
			நிறுத்து;
	}

	திரும்பு 0;
}


நிலையான எழுத்து *http__canonical_filename(
		கட்டமைப்பு lws *wsi,
		மாறிலி எழுத்து *in,
		மாறிலி எழுத்து *http_dir)
{
	size_t inlen, slen;
	எழுத்து *filename, *filename_canonical;

	inlen = strlen(in);
	எனில்(in[inlen-1] == '/'){
		slen = strlen(http_dir) + inlen + strlen("/index.html") + 2;
	}இல்லையெனில்{
		slen = strlen(http_dir) + inlen + 2;
	}
	filename = mosquitto_malloc(slen);
	எனில்(!filename){
		lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, வெற்று);
		திரும்பு வெற்று;
	}
	எனில்(((எழுத்து *)in)[inlen-1] == '/'){
		snprintf(filename, slen, "%s%sindex.html", http_dir, (எழுத்து *)in);
	}இல்லையெனில்{
		snprintf(filename, slen, "%s%s", http_dir, (எழுத்து *)in);
	}


	/* Get canonical path and check it is within our http_dir */
#ifdef WIN32
	filename_canonical = _fullpath(வெற்று, filename, 0);
	mosquitto_FREE(filename);
	எனில்(!filename_canonical){
		lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, வெற்று);
		திரும்பு வெற்று;
	}
#இல்லையெனில்
	filename_canonical = realpath(filename, வெற்று);
	mosquitto_FREE(filename);
	எனில்(!filename_canonical){
		எனில்(errno == EACCES){
			lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, வெற்று);
		}இல்லையெனில் எனில்(errno == EINVAL || errno == EIO || errno == ELOOP){
			lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, வெற்று);
		}இல்லையெனில் எனில்(errno == ENAMETOOLONG){
			lws_return_http_status(wsi, HTTP_STATUS_REQ_URI_TOO_LONG, வெற்று);
		}இல்லையெனில் எனில்(errno == ENOENT || errno == ENOTDIR){
			lws_return_http_status(wsi, HTTP_STATUS_NOT_FOUND, வெற்று);
		}
		திரும்பு வெற்று;
	}
#endif
	எனில்(strncmp(http_dir, filename_canonical, strlen(http_dir))){
		/* Requested file isn't within http_dir, deny access. */
		SAFE_FREE(filename_canonical);
		lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, வெற்று);
		திரும்பு வெற்று;
	}

	திரும்பு filename_canonical;
}


நிலையான முழு callback_http(
		கட்டமைப்பு lws *wsi,
		எண்ணிக்கை lws_callback_reasons reason,
		வெறுமை *user,
		வெறுமை *in,
		size_t len)
{
	கட்டமைப்பு libws_http_data *u = (கட்டமைப்பு libws_http_data *)user;
	கட்டமைப்பு libws_mqtt_hack *hack;
	எழுத்து *http_dir;
	size_t buflen;
	size_t wlen;
	முழு rc;
	எழுத்து *filename_canonical;
	unsigned எழுத்து buf[4096];
	கட்டமைப்பு stat filestat;
	கட்டமைப்பு mosquitto *mosq;
	கட்டமைப்பு lws_pollargs *pollargs = (கட்டமைப்பு lws_pollargs *)in;
	முழு hlen;

	/* FIXME - ssl cert verification is done here. */

	தேர்வு(reason){
		நிலை LWS_CALLBACK_HTTP:
			எனில்(!u){
				திரும்பு -1;
			}

			hack = (கட்டமைப்பு libws_mqtt_hack *)lws_context_user(lws_get_context(wsi));
			எனில்(!hack){
				திரும்பு -1;
			}
			http_dir = hack->http_dir;

			எனில்(!http_dir){
				/* http disabled */
				திரும்பு -1;
			}

			/* Forbid POST */
			எனில்(lws_hdr_total_length(wsi, WSI_TOKEN_POST_URI)){
				lws_return_http_status(wsi, HTTP_STATUS_METHOD_NOT_ALLOWED, வெற்று);
				திரும்பு -1;
			}

			filename_canonical = http__canonical_filename(wsi, (எழுத்து *)in, http_dir);
			எனில்(!filename_canonical){
				திரும்பு -1;
			}

			u->fptr = fopen(filename_canonical, "rb");
			எனில்(!u->fptr){
				SAFE_FREE(filename_canonical);
				lws_return_http_status(wsi, HTTP_STATUS_NOT_FOUND, வெற்று);
				திரும்பு -1;
			}
			எனில்(fstat(fileno(u->fptr), &filestat) < 0){
				SAFE_FREE(filename_canonical);
				lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, வெற்று);
				fclose(u->fptr);
				u->fptr = வெற்று;
				திரும்பு -1;
			}


			எனில்((filestat.st_mode & S_IFDIR) == S_IFDIR){
				fclose(u->fptr);
				u->fptr = வெற்று;
				SAFE_FREE(filename_canonical);

				/* FIXME - use header functions from lws 2.x */
				buflen = (size_t)snprintf((எழுத்து *)buf, 4096, "HTTP/1.0 302 OK\r\n"
						"Location: %s/\r\n\r\n",
						(எழுத்து *)in);
				திரும்பு lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
			}

			எனில்((filestat.st_mode & S_IFREG) != S_IFREG){
				lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, வெற்று);
				fclose(u->fptr);
				u->fptr = வெற்று;
				SAFE_FREE(filename_canonical);
				திரும்பு -1;
			}

			log__printf(வெற்று, MOSQ_LOG_DEBUG, "http serving file \"%s\".", filename_canonical);
			SAFE_FREE(filename_canonical);
			/* FIXME - use header functions from lws 2.x */
			buflen = (size_t)snprintf((எழுத்து *)buf, 4096, "HTTP/1.0 200 OK\r\n"
					"Server: mosquitto\r\n"
					"Content-Length: %u\r\n\r\n",
					(unsigned முழு)filestat.st_size);
			எனில்(lws_write(wsi, buf, buflen, LWS_WRITE_HTTP) < 0){
				fclose(u->fptr);
				u->fptr = வெற்று;
				திரும்பு -1;
			}
			lws_callback_on_writable(wsi);
			நிறுத்து;

		நிலை LWS_CALLBACK_HTTP_BODY:
			/* For extra POST data? */
			திரும்பு -1;

		நிலை LWS_CALLBACK_HTTP_BODY_COMPLETION:
			/* For end of extra POST data? */
			திரும்பு -1;

		நிலை LWS_CALLBACK_FILTER_HTTP_CONNECTION:
			/* Access control here */
			திரும்பு 0;

#எனில் LWS_LIBRARY_VERSION_எண் >= 3001000
		நிலை LWS_CALLBACK_HTTP_CONFIRM_UPGRADE:
			hack = lws_context_user(lws_get_context(wsi));
			எனில்(hack->listener->ws_origin_மொத்தம்){
				hlen = lws_hdr_total_length(wsi, WSI_TOKEN_ORIGIN);
				எனில்(hlen <= 0 || hlen >= (முழு)அளவு(buf)){
					திரும்பு -1;
				}
				க்கு(முழு i=0; i<hack->listener->ws_origin_மொத்தம்; i++){
					lws_hdr_copy(wsi, (எழுத்து *)buf, அளவு(buf), WSI_TOKEN_ORIGIN);
					எனில்((!strcmp(hack->listener->ws_origins[i], (எழுத்து *)buf))){
						திரும்பு 0;
					}
				}
				திரும்பு -1;
			}இல்லையெனில்{
				திரும்பு 0;
			}
#endif

		நிலை LWS_CALLBACK_HTTP_WRITEABLE:
			/* Send our data here */
			எனில்(u && u->fptr){
				செய்{
					buflen = fread(buf, 1, அளவு(buf), u->fptr);
					எனில்(buflen < 1){
						fclose(u->fptr);
						u->fptr = வெற்று;
						திரும்பு -1;
					}
					rc = lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
					எனில்(rc < 0){
						திரும்பு -1;
					}
					wlen = (size_t)rc;
					/* while still active, extend timeout */
					எனில்(wlen){
						lws_set_timeout(wsi, PENDING_TIMEOUT_HTTP_CONTENT, 10);
					}
					எனில்(wlen < buflen){
						எனில்(fseek(u->fptr, (long)(buflen-wlen), SEEK_CUR) < 0){
							fclose(u->fptr);
							u->fptr = வெற்று;
							திரும்பு -1;
						}
					}இல்லையெனில்{
						எனில்(buflen < அளவு(buf)){
							fclose(u->fptr);
							u->fptr = வெற்று;
						}
					}
				}போது(u->fptr && !lws_send_pipe_choked(wsi));
				lws_callback_on_writable(wsi);
			}இல்லையெனில்{
				திரும்பு -1;
			}
			நிறுத்து;

		நிலை LWS_CALLBACK_CLOSED:
		நிலை LWS_CALLBACK_CLOSED_HTTP:
		நிலை LWS_CALLBACK_HTTP_FILE_COMPLETION:
			எனில்(u && u->fptr){
				fclose(u->fptr);
				u->fptr = வெற்று;
			}
			நிறுத்து;

		நிலை LWS_CALLBACK_ADD_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, அளவு(pollargs->fd), mosq);
			எனில்(mosq){
				எனில்(pollargs->events & LWS_POLLOUT){
					mux__add_out(mosq);
				}இல்லையெனில்{
					mux__remove_out(mosq);
				}
			}இல்லையெனில்{
				எனில்(pollargs->events & POLLIN){
					/* Assume this is a new listener */
					listeners__add_websockets(lws_get_context(wsi), pollargs->fd);
				}
			}
			நிறுத்து;

		நிலை LWS_CALLBACK_DEL_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, அளவு(pollargs->fd), mosq);
			எனில்(mosq){
				mux__delete(mosq);
			}
			நிறுத்து;

		நிலை LWS_CALLBACK_CHANGE_MODE_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, அளவு(pollargs->fd), mosq);
			எனில்(mosq){
				எனில்(pollargs->events & LWS_POLLHUP){
					திரும்பு 1;
				}இல்லையெனில் எனில்(pollargs->events & LWS_POLLOUT){
					mux__add_out(mosq);
				}இல்லையெனில்{
					mux__remove_out(mosq);
				}
			}
			நிறுத்து;

#ifdef WITH_TLS
		நிலை LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION:
			எனில்(!len || (SSL_get_verify_result((SSL *)in) != X509_V_OK)){
				திரும்பு 1;
			}
			நிறுத்து;
#endif

		இயல்பு:
			திரும்பு 0;
	}

	திரும்பு 0;
}


நிலையான வெறுமை log_wrap(முழு level, மாறிலி எழுத்து *வரி)
{
	எழுத்து *l = (எழுத்து *)வரி;
	UNUSED(level);
	l[strlen(வரி)-1] = '\0'; /* Remove \n */
	log__printf(வெற்று, MOSQ_LOG_WEBSOCKETS, "%s", l);
}


வெறுமை mosq_websockets_init(கட்டமைப்பு mosquitto__listener *listener, மாறிலி கட்டமைப்பு mosquitto__config *conf)
{
	கட்டமைப்பு lws_context_creation_info info;
	கட்டமைப்பு lws_protocols *p;
	size_t protocol_மொத்தம்;
	முழு i;
	கட்டமைப்பு libws_mqtt_hack *user;

	/* Count valid protocols */
	க்கு(protocol_மொத்தம்=0; protocols[protocol_மொத்தம்].name; protocol_மொத்தம்++){
		;
	}

	p = mosquitto_calloc(protocol_மொத்தம்+1, அளவு(கட்டமைப்பு lws_protocols));
	எனில்(!p){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Out of memory.");
		திரும்பு;
	}
	க்கு(i=0; protocols[i].name; i++){
		p[i].name = protocols[i].name;
		p[i].callback = protocols[i].callback;
		p[i].per_session_data_size = protocols[i].per_session_data_size;
		p[i].rx_buffer_size = protocols[i].rx_buffer_size;
		p[i].user = listener;
	}

	memset(&info, 0, அளவு(info));
	info.iface = listener->host;
	info.port = listener->port;
	info.protocols = p;
	info.gid = -1;
	info.uid = -1;
#ifdef WITH_TLS
	எனில்(listener->cafile){
		info.ssl_ca_filepath = listener->cafile;
	}இல்லையெனில் எனில்(listener->capath){
		log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: CA path option is not supported for websockets");
	}
	info.ssl_cert_filepath = listener->certfile;
	info.ssl_private_சாவி_filepath = listener->keyfile;
	info.ssl_cipher_list = listener->ciphers;
	/* HTTP 1 only, due to HTTP 2 issues in Firefox:
	   https://github.com/eclipse-mosquitto/mosquitto/issues/1211
	*/
	info.alpn = "h1";
#எனில் defined(WITH_WEBSOCKETS) && LWS_LIBRARY_VERSION_எண்>=3001000
	info.tls1_3_plus_cipher_list = listener->ciphers_tls13;
#endif
	எனில்(listener->require_certificate){
		info.options |= LWS_SERVER_OPTION_REQUIRE_VALID_OPENSSL_CLIENT_CERT;
	}
#endif

	info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
	எனில்(listener->socket_domain == AF_INET){
		info.options |= LWS_SERVER_OPTION_DISABLE_IPV6;
	}
	info.max_http_header_data = conf->packet_buffer_size;

	user = mosquitto_calloc(1, அளவு(கட்டமைப்பு libws_mqtt_hack));
	எனில்(!user){
		mosquitto_FREE(p);
		log__printf(வெற்று, MOSQ_LOG_ERR, "Out of memory.");
		திரும்பு;
	}

	எனில்(listener->http_dir){
#ifdef WIN32
		user->http_dir = _fullpath(வெற்று, listener->http_dir, 0);
#இல்லையெனில்
		user->http_dir = realpath(listener->http_dir, வெற்று);
#endif
		எனில்(!user->http_dir){
			mosquitto_FREE(user);
			mosquitto_FREE(p);
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to open http dir \"%s\".", listener->http_dir);
			திரும்பு;
		}
	}
	user->listener = listener;

	info.user = user;
	info.pt_serv_buf_size = WS_SERV_BUF_SIZE;
	listener->ws_protocol = p;

	lws_set_log_level(conf->websockets_log_level, log_wrap);

	log__printf(வெற்று, MOSQ_LOG_INFO, "Opening websockets listen socket on port %d.", listener->port);
	listener->ws_in_init = true;
	listener->ws_context = lws_create_context(&info);
	listener->ws_in_init = false;
}

#endif
#endif
