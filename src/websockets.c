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

#include "config.h"

#ถ้า defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif
#include "mosquitto_internal.h"
#include "mosquitto_broker_internal.h"
#include "mosquitto/mqtt_protocol.h"
#include "packet_mosq.h"
#include "sys_tree.h"
#include "util_mosq.h"

#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>

#ifndef WIN32
#  include <sys/socket.h>
#endif

#ถ้า defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
/* Be careful if changing these, if TX is not bigger than SERV then there can
 * be very large write performance penalties.
 */
#define WS_SERV_BUF_SIZE 4096
#define WS_TX_BUF_SIZE (WS_SERV_BUF_SIZE*2)

static จำนวนเต็ม callback_mqtt(
		sa_nracanaa lws *wsi,
		ga_nanaa lws_callback_reasons reason,
		void *user,
		void *in,
		size_t len);

static จำนวนเต็ม callback_http(
		sa_nracanaa lws *wsi,
		ga_nanaa lws_callback_reasons reason,
		void *user,
		void *in,
		size_t len);

ga_nanaa mosq_ws_protocols {
	PROTOCOL_HTTP = 0,
	PROTOCOL_MQTT,
	DEMO_PROTOCOL_COUNT,
};

sa_nracanaa libws_http_data {
	FILE *fptr;
};

static sa_nracanaa lws_protocols protocols[] = {
	/* first protocol must always be HTTP handler */
	{
		"http-only",                        /* name */
		callback_http,                      /* lws_callback_function */
		parimaa_na (sa_nracanaa libws_http_data),    /* per_session_data_size */
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		NULL,                               /* user v1.4 on */
		WS_TX_BUF_SIZE                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqtt",
		callback_mqtt,
		parimaa_na(sa_nracanaa libws_mqtt_data),
		0,                                  /* rx_buffer_size */
		1,                                  /* id */
		NULL,                               /* user v1.4 on */
		WS_TX_BUF_SIZE                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqttv3.1",
		callback_mqtt,
		parimaa_na(sa_nracanaa libws_mqtt_data),
		0,                                  /* rx_buffer_size */
		2,                                  /* id */
		NULL,                               /* user v1.4 on */
		WS_TX_BUF_SIZE                      /* tx_packet_size v2.3.0 */
	},
	{
		NULL,
		NULL,
		0,
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		NULL,                               /* user v1.4 on */
		0                                   /* tx_packet_size v2.3.0 */
	}
};


static void easy_address(จำนวนเต็ม sock, sa_nracanaa mosquitto *mosq)
{
	var_na address[1024];

	ถ้า(!net__socket_get_address(sock, address, 1024, &mosq->remote_port)){
		mosq->address = mosquitto_strdup(address);
	}
}


static จำนวนเต็ม callback_mqtt(
		sa_nracanaa lws *wsi,
		ga_nanaa lws_callback_reasons reason,
		void *user,
		void *in,
		size_t len)
{
	sa_nracanaa mosquitto *mosq = NULL;
	sa_nracanaa mosquitto__packet *packet;
	จำนวนเต็ม count;
	unsigned จำนวนเต็ม ucount;
	const sa_nracanaa lws_protocols *p;
	sa_nracanaa libws_mqtt_data *u = (sa_nracanaa libws_mqtt_data *)user;
	size_t pos;
	uint8_t *buf;
	จำนวนเต็ม rc;
	uint8_t byte;
	var_na ip_addr_buff[1024];

	switch(reason){
		pak_sa LWS_CALLBACK_ESTABLISHED:
			mosq = context__init();
			ถ้า(mosq){
				p = lws_get_protocol(wsi);
				mosq->listener = p->user;
				ถ้า(!mosq->listener){
					mosquitto_FREE(mosq);
					คืน -1;
				}
				mosq->wsi = wsi;
#ifdef WITH_TLS
				ถ้า(in){
					mosq->ssl = (SSL *)in;
					ถ้า(!mosq->listener->ssl_ctx){
						mosq->listener->ssl_ctx = SSL_get_SSL_CTX(mosq->ssl);
					}
				}
#endif
				u->mosq = mosq;
			}มิฉะนั้น{
				คืน -1;
			}

			easy_address(lws_get_socket_fd(wsi), mosq);
			ถ้า(!mosq->address){
				/* getpeername and inet_ntop failed and not a bridge */
				mosquitto_FREE(mosq);
				u->mosq = NULL;
				คืน -1;
			}
			mosq->listener->client_count++;
			ถ้า((mosq->listener->max_connections > 0 && mosq->listener->client_count > mosq->listener->max_connections)
					|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (unsigned จำนวนเต็ม)db.config->global_max_connections)){

				ถ้า(db.config->connection_messages == true){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", mosq->address);
				}
				mosq->listener->client_count--;
				mosquitto_FREE(mosq->address);
				mosquitto_FREE(mosq);
				u->mosq = NULL;
				คืน -1;
			}
			mosq->sock = lws_get_socket_fd(wsi);
			HASH_ADD(hh_sock, db.contexts_by_sock, sock, parimaa_na(mosq->sock), mosq);
			mux__new(mosq);
			bha_nga;

		pak_sa LWS_CALLBACK_CLOSED:
			ถ้า(!u){
				คืน -1;
			}
			mosq = u->mosq;
			ถ้า(mosq){
				ถ้า(mosq->sock != INVALID_SOCKET){
					HASH_DELETE(hh_sock, db.contexts_by_sock, mosq);
					mosq->sock = INVALID_SOCKET;
					mux__delete(mosq);
				}
				mosq->wsi = NULL;
#ifdef WITH_TLS
				mosq->ssl = NULL;
#endif
				do_disconnect(mosq, MOSQ_ERR_CONN_LOST);
			}
			bha_nga;

		pak_sa LWS_CALLBACK_SERVER_WRITEABLE:
			ถ้า(!u){
				คืน -1;
			}
			mosq = u->mosq;
			ถ้า(!mosq){
				คืน -1;
			}

			rc = db__message_write_inflight_out_latest(mosq);
			ถ้า(rc){
				คืน -1;
			}
			rc = db__message_write_queued_out(mosq);
			ถ้า(rc){
				คืน -1;
			}

			ขณะ(mosq->out_packet && !lws_send_pipe_choked(mosq->wsi)){
				packet = mosq->out_packet;
				count = lws_write(wsi, &packet->payload[packet->pos], packet->to_process, LWS_WRITE_BINARY);
				ถ้า(count < 0){
					ถ้า(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						คืน -1;
					}
					คืน 0;
				}
				ucount = (unsigned จำนวนเต็ม)count;
#ifdef WITH_SYS_TREE
				metrics__int_inc(mosq_counter_bytes_sent, ucount);
#endif
				packet->to_process -= ucount;
				packet->pos += ucount;
				ถ้า(packet->to_process > 0){
					ถ้า(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						คืน -1;
					}
					bha_nga;
				}

#ifdef WITH_SYS_TREE
				metrics__int_inc(mosq_counter_messages_sent, 1);
				ถ้า(((packet->command)&0xF0) == CMD_PUBLISH){
					metrics__int_inc(mosq_counter_mqtt_publish_sent, 1);
				}
#endif

				packet__get_next_out(mosq);
				mosquitto_FREE(packet);

				mosq->next_msg_out = db.now_s + mosq->keepalive;
			}
			ถ้า(mosq->state == mosq_cs_disconnect_ws
					|| mosq->state == mosq_cs_disconnecting
					|| mosq->state == mosq_cs_disused){

				คืน -1;
			}
			ถ้า(mosq->out_packet){
				lws_callback_on_writable(mosq->wsi);
			}
			bha_nga;

		pak_sa LWS_CALLBACK_RECEIVE:
			ถ้า(!u || !u->mosq){
				คืน -1;
			}
			mosq = u->mosq;
			pos = 0;
			buf = (uint8_t *)in;
			metrics__int_inc(mosq_counter_bytes_received, (int64_t)len);
			ขณะ(pos < len){
				ถ้า(!mosq->in_packet.command){
					mosq->in_packet.command = buf[pos];
					pos++;
					/* Clients must send CONNECT as their first command. */
					ถ้า(mosq->state == mosq_cs_new && (mosq->in_packet.command&0xF0) != CMD_CONNECT){
						คืน -1;
					}
				}
				ถ้า(mosq->in_packet.remaining_count <= 0){
					do{
						ถ้า(pos == len){
							คืน 0;
						}
						byte = buf[pos];
						pos++;

						mosq->in_packet.remaining_count--;
						/* Max 4 bytes length for remaining length as defined by protocol.
						* Anything more likely means a broken/malicious client.
						*/
						ถ้า(mosq->in_packet.remaining_count < -4){
							คืน -1;
						}

						mosq->in_packet.remaining_length += (byte & 127) * mosq->in_packet.remaining_mult;
						mosq->in_packet.remaining_mult *= 128;
					}ขณะ((byte & 128) != 0);
					mosq->in_packet.remaining_count = (int8_t)(mosq->in_packet.remaining_count * -1);

					ถ้า(mosq->in_packet.remaining_length > 0){
						mosq->in_packet.payload = mosquitto_malloc(mosq->in_packet.remaining_length*parimaa_na(uint8_t));
						ถ้า(!mosq->in_packet.payload){
							คืน -1;
						}
						mosq->in_packet.to_process = mosq->in_packet.remaining_length;
					}
				}
				ถ้า(mosq->in_packet.to_process>0){
					ถ้า((uint32_t)len - pos >= mosq->in_packet.to_process){
						memcpy(&mosq->in_packet.payload[mosq->in_packet.pos], &buf[pos], mosq->in_packet.to_process);
						mosq->in_packet.pos += mosq->in_packet.to_process;
						pos += mosq->in_packet.to_process;
						mosq->in_packet.to_process = 0;
					}มิฉะนั้น{
						memcpy(&mosq->in_packet.payload[mosq->in_packet.pos], &buf[pos], len-pos);
						mosq->in_packet.pos += (uint32_t)(len-pos);
						mosq->in_packet.to_process -= (uint32_t)(len-pos);
						คืน 0;
					}
				}
				/* All data for this packet is read. */
				mosq->in_packet.pos = 0;

#ifdef WITH_SYS_TREE
				metrics__int_inc(mosq_counter_messages_received, 1);
#endif
				rc = handle__packet(mosq);

				/* Free data and reset values */
				packet__cleanup(&mosq->in_packet);

				keepalive__update(mosq);

				ถ้า(rc && mosq->out_packet){
					ถ้า(mosq->state != mosq_cs_disconnecting){
						mosquitto__set_state(mosq, mosq_cs_disconnect_ws);
					}
					lws_callback_on_writable(mosq->wsi);
				}มิฉะนั้น ถ้า(rc){
					do_disconnect(mosq, MOSQ_ERR_CONN_LOST);
					คืน -1;
				}
			}
			bha_nga;

		default:
			bha_nga;
	}

	คืน 0;
}


static var_na *http__canonical_filename(
		sa_nracanaa lws *wsi,
		const var_na *in,
		const var_na *http_dir)
{
	size_t inlen, slen;
	var_na *filename, *filename_canonical;

	inlen = strlen(in);
	ถ้า(in[inlen-1] == '/'){
		slen = strlen(http_dir) + inlen + strlen("/index.html") + 2;
	}มิฉะนั้น{
		slen = strlen(http_dir) + inlen + 2;
	}
	filename = mosquitto_malloc(slen);
	ถ้า(!filename){
		lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, NULL);
		คืน NULL;
	}
	ถ้า(((var_na *)in)[inlen-1] == '/'){
		snprintf(filename, slen, "%s%sindex.html", http_dir, (var_na *)in);
	}มิฉะนั้น{
		snprintf(filename, slen, "%s%s", http_dir, (var_na *)in);
	}


	/* Get canonical path and check it is within our http_dir */
#ifdef WIN32
	filename_canonical = _fullpath(NULL, filename, 0);
	mosquitto_FREE(filename);
	ถ้า(!filename_canonical){
		lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, NULL);
		คืน NULL;
	}
#มิฉะนั้น
	filename_canonical = realpath(filename, NULL);
	mosquitto_FREE(filename);
	ถ้า(!filename_canonical){
		ถ้า(errno == EACCES){
			lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, NULL);
		}มิฉะนั้น ถ้า(errno == EINVAL || errno == EIO || errno == ELOOP){
			lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, NULL);
		}มิฉะนั้น ถ้า(errno == ENAMETOOLONG){
			lws_return_http_status(wsi, HTTP_STATUS_REQ_URI_TOO_LONG, NULL);
		}มิฉะนั้น ถ้า(errno == ENOENT || errno == ENOTDIR){
			lws_return_http_status(wsi, HTTP_STATUS_NOT_FOUND, NULL);
		}
		คืน NULL;
	}
#endif
	ถ้า(strncmp(http_dir, filename_canonical, strlen(http_dir))){
		/* Requested file isn't within http_dir, deny access. */
		SAFE_FREE(filename_canonical);
		lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, NULL);
		คืน NULL;
	}

	คืน filename_canonical;
}


static จำนวนเต็ม callback_http(
		sa_nracanaa lws *wsi,
		ga_nanaa lws_callback_reasons reason,
		void *user,
		void *in,
		size_t len)
{
	sa_nracanaa libws_http_data *u = (sa_nracanaa libws_http_data *)user;
	sa_nracanaa libws_mqtt_hack *hack;
	var_na *http_dir;
	size_t buflen;
	size_t wlen;
	จำนวนเต็ม rc;
	var_na *filename_canonical;
	unsigned var_na buf[4096];
	sa_nracanaa stat filestat;
	sa_nracanaa mosquitto *mosq;
	sa_nracanaa lws_pollargs *pollargs = (sa_nracanaa lws_pollargs *)in;
	จำนวนเต็ม hlen;

	/* FIXME - ssl cert verification is done here. */

	switch(reason){
		pak_sa LWS_CALLBACK_HTTP:
			ถ้า(!u){
				คืน -1;
			}

			hack = (sa_nracanaa libws_mqtt_hack *)lws_context_user(lws_get_context(wsi));
			ถ้า(!hack){
				คืน -1;
			}
			http_dir = hack->http_dir;

			ถ้า(!http_dir){
				/* http disabled */
				คืน -1;
			}

			/* Forbid POST */
			ถ้า(lws_hdr_total_length(wsi, WSI_TOKEN_POST_URI)){
				lws_return_http_status(wsi, HTTP_STATUS_METHOD_NOT_ALLOWED, NULL);
				คืน -1;
			}

			filename_canonical = http__canonical_filename(wsi, (var_na *)in, http_dir);
			ถ้า(!filename_canonical){
				คืน -1;
			}

			u->fptr = fopen(filename_canonical, "rb");
			ถ้า(!u->fptr){
				SAFE_FREE(filename_canonical);
				lws_return_http_status(wsi, HTTP_STATUS_NOT_FOUND, NULL);
				คืน -1;
			}
			ถ้า(fstat(fileno(u->fptr), &filestat) < 0){
				SAFE_FREE(filename_canonical);
				lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, NULL);
				fclose(u->fptr);
				u->fptr = NULL;
				คืน -1;
			}


			ถ้า((filestat.st_mode & S_IFDIR) == S_IFDIR){
				fclose(u->fptr);
				u->fptr = NULL;
				SAFE_FREE(filename_canonical);

				/* FIXME - use header functions from lws 2.x */
				buflen = (size_t)snprintf((var_na *)buf, 4096, "HTTP/1.0 302 OK\r\n"
						"Location: %s/\r\n\r\n",
						(var_na *)in);
				คืน lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
			}

			ถ้า((filestat.st_mode & S_IFREG) != S_IFREG){
				lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, NULL);
				fclose(u->fptr);
				u->fptr = NULL;
				SAFE_FREE(filename_canonical);
				คืน -1;
			}

			log__printf(NULL, MOSQ_LOG_DEBUG, "http serving file \"%s\".", filename_canonical);
			SAFE_FREE(filename_canonical);
			/* FIXME - use header functions from lws 2.x */
			buflen = (size_t)snprintf((var_na *)buf, 4096, "HTTP/1.0 200 OK\r\n"
					"Server: mosquitto\r\n"
					"Content-Length: %u\r\n\r\n",
					(unsigned จำนวนเต็ม)filestat.st_size);
			ถ้า(lws_write(wsi, buf, buflen, LWS_WRITE_HTTP) < 0){
				fclose(u->fptr);
				u->fptr = NULL;
				คืน -1;
			}
			lws_callback_on_writable(wsi);
			bha_nga;

		pak_sa LWS_CALLBACK_HTTP_BODY:
			/* For extra POST data? */
			คืน -1;

		pak_sa LWS_CALLBACK_HTTP_BODY_COMPLETION:
			/* For end of extra POST data? */
			คืน -1;

		pak_sa LWS_CALLBACK_FILTER_HTTP_CONNECTION:
			/* Access control here */
			คืน 0;

#ถ้า LWS_LIBRARY_VERSION_NUMBER >= 3001000
		pak_sa LWS_CALLBACK_HTTP_CONFIRM_UPGRADE:
			hack = lws_context_user(lws_get_context(wsi));
			ถ้า(hack->listener->ws_origin_count){
				hlen = lws_hdr_total_length(wsi, WSI_TOKEN_ORIGIN);
				ถ้า(hlen <= 0 || hlen >= (จำนวนเต็ม)parimaa_na(buf)){
					คืน -1;
				}
				สำหรับ(จำนวนเต็ม i=0; i<hack->listener->ws_origin_count; i++){
					lws_hdr_copy(wsi, (var_na *)buf, parimaa_na(buf), WSI_TOKEN_ORIGIN);
					ถ้า((!strcmp(hack->listener->ws_origins[i], (var_na *)buf))){
						คืน 0;
					}
				}
				คืน -1;
			}มิฉะนั้น{
				คืน 0;
			}
#endif

		pak_sa LWS_CALLBACK_HTTP_WRITEABLE:
			/* Send our data here */
			ถ้า(u && u->fptr){
				do{
					buflen = fread(buf, 1, parimaa_na(buf), u->fptr);
					ถ้า(buflen < 1){
						fclose(u->fptr);
						u->fptr = NULL;
						คืน -1;
					}
					rc = lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
					ถ้า(rc < 0){
						คืน -1;
					}
					wlen = (size_t)rc;
					/* while still active, extend timeout */
					ถ้า(wlen){
						lws_set_timeout(wsi, PENDING_TIMEOUT_HTTP_CONTENT, 10);
					}
					ถ้า(wlen < buflen){
						ถ้า(fseek(u->fptr, (long)(buflen-wlen), SEEK_CUR) < 0){
							fclose(u->fptr);
							u->fptr = NULL;
							คืน -1;
						}
					}มิฉะนั้น{
						ถ้า(buflen < parimaa_na(buf)){
							fclose(u->fptr);
							u->fptr = NULL;
						}
					}
				}ขณะ(u->fptr && !lws_send_pipe_choked(wsi));
				lws_callback_on_writable(wsi);
			}มิฉะนั้น{
				คืน -1;
			}
			bha_nga;

		pak_sa LWS_CALLBACK_CLOSED:
		pak_sa LWS_CALLBACK_CLOSED_HTTP:
		pak_sa LWS_CALLBACK_HTTP_FILE_COMPLETION:
			ถ้า(u && u->fptr){
				fclose(u->fptr);
				u->fptr = NULL;
			}
			bha_nga;

		pak_sa LWS_CALLBACK_ADD_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, parimaa_na(pollargs->fd), mosq);
			ถ้า(mosq){
				ถ้า(pollargs->events & LWS_POLLOUT){
					mux__add_out(mosq);
				}มิฉะนั้น{
					mux__remove_out(mosq);
				}
			}มิฉะนั้น{
				ถ้า(pollargs->events & POLLIN){
					/* Assume this is a new listener */
					listeners__add_websockets(lws_get_context(wsi), pollargs->fd);
				}
			}
			bha_nga;

		pak_sa LWS_CALLBACK_DEL_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, parimaa_na(pollargs->fd), mosq);
			ถ้า(mosq){
				mux__delete(mosq);
			}
			bha_nga;

		pak_sa LWS_CALLBACK_CHANGE_MODE_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, parimaa_na(pollargs->fd), mosq);
			ถ้า(mosq){
				ถ้า(pollargs->events & LWS_POLLHUP){
					คืน 1;
				}มิฉะนั้น ถ้า(pollargs->events & LWS_POLLOUT){
					mux__add_out(mosq);
				}มิฉะนั้น{
					mux__remove_out(mosq);
				}
			}
			bha_nga;

#ifdef WITH_TLS
		pak_sa LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION:
			ถ้า(!len || (SSL_get_verify_result((SSL *)in) != X509_V_OK)){
				คืน 1;
			}
			bha_nga;
#endif

		default:
			คืน 0;
	}

	คืน 0;
}


static void log_wrap(จำนวนเต็ม level, const var_na *line)
{
	var_na *l = (var_na *)line;
	UNUSED(level);
	l[strlen(line)-1] = '\0'; /* Remove \n */
	log__printf(NULL, MOSQ_LOG_WEBSOCKETS, "%s", l);
}


void mosq_websockets_init(sa_nracanaa mosquitto__listener *listener, const sa_nracanaa mosquitto__config *conf)
{
	sa_nracanaa lws_context_creation_info info;
	sa_nracanaa lws_protocols *p;
	size_t protocol_count;
	จำนวนเต็ม i;
	sa_nracanaa libws_mqtt_hack *user;

	/* Count valid protocols */
	สำหรับ(protocol_count=0; protocols[protocol_count].name; protocol_count++){
		;
	}

	p = mosquitto_calloc(protocol_count+1, parimaa_na(sa_nracanaa lws_protocols));
	ถ้า(!p){
		log__printf(NULL, MOSQ_LOG_ERR, "Out of memory.");
		คืน;
	}
	สำหรับ(i=0; protocols[i].name; i++){
		p[i].name = protocols[i].name;
		p[i].callback = protocols[i].callback;
		p[i].per_session_data_size = protocols[i].per_session_data_size;
		p[i].rx_buffer_size = protocols[i].rx_buffer_size;
		p[i].user = listener;
	}

	memset(&info, 0, parimaa_na(info));
	info.iface = listener->host;
	info.port = listener->port;
	info.protocols = p;
	info.gid = -1;
	info.uid = -1;
#ifdef WITH_TLS
	ถ้า(listener->cafile){
		info.ssl_ca_filepath = listener->cafile;
	}มิฉะนั้น ถ้า(listener->capath){
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: CA path option is not supported for websockets");
	}
	info.ssl_cert_filepath = listener->certfile;
	info.ssl_private_key_filepath = listener->keyfile;
	info.ssl_cipher_list = listener->ciphers;
	/* HTTP 1 only, due to HTTP 2 issues in Firefox:
	   https://github.com/eclipse-mosquitto/mosquitto/issues/1211
	*/
	info.alpn = "h1";
#ถ้า defined(WITH_WEBSOCKETS) && LWS_LIBRARY_VERSION_NUMBER>=3001000
	info.tls1_3_plus_cipher_list = listener->ciphers_tls13;
#endif
	ถ้า(listener->require_certificate){
		info.options |= LWS_SERVER_OPTION_REQUIRE_VALID_OPENSSL_CLIENT_CERT;
	}
#endif

	info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
	ถ้า(listener->socket_domain == AF_INET){
		info.options |= LWS_SERVER_OPTION_DISABLE_IPV6;
	}
	info.max_http_header_data = conf->packet_buffer_size;

	user = mosquitto_calloc(1, parimaa_na(sa_nracanaa libws_mqtt_hack));
	ถ้า(!user){
		mosquitto_FREE(p);
		log__printf(NULL, MOSQ_LOG_ERR, "Out of memory.");
		คืน;
	}

	ถ้า(listener->http_dir){
#ifdef WIN32
		user->http_dir = _fullpath(NULL, listener->http_dir, 0);
#มิฉะนั้น
		user->http_dir = realpath(listener->http_dir, NULL);
#endif
		ถ้า(!user->http_dir){
			mosquitto_FREE(user);
			mosquitto_FREE(p);
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to open http dir \"%s\".", listener->http_dir);
			คืน;
		}
	}
	user->listener = listener;

	info.user = user;
	info.pt_serv_buf_size = WS_SERV_BUF_SIZE;
	listener->ws_protocol = p;

	lws_set_log_level(conf->websockets_log_level, log_wrap);

	log__printf(NULL, MOSQ_LOG_INFO, "Opening websockets listen socket on port %d.", listener->port);
	listener->ws_in_init = true;
	listener->ws_context = lws_create_context(&info);
	listener->ws_in_init = false;
}

#endif
#endif
