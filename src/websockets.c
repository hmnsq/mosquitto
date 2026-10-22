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

#অন্তর্ভাব "config.h"

#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif
#অন্তর্ভাব "mosquitto_internal.h"
#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto/mqtt_protocol.h"
#অন্তর্ভাব "packet_mosq.h"
#অন্তর্ভাব "sys_tree.h"
#অন্তর্ভাব "util_mosq.h"

#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <errno.h>
#অন্তর্ভাব <sys/stat.h>

#ifndef WIN32
#  include <sys/socket.h>
#endif

#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
/* Be careful if changing these, if TX is not bigger than SERV then there can
 * be very large write performance penalties.
 */
#define WS_SERV_BUF_আকার 4096
#define WS_TX_BUF_আকার (WS_SERV_BUF_আকার*2)

স্থির পূর্ণসংখ্যা callback_mqtt(
		কাঠামো lws *wsi,
		গণনা lws_callback_reasons reason,
		শূন্য *user,
		শূন্য *in,
		size_t len);

স্থির পূর্ণসংখ্যা callback_http(
		কাঠামো lws *wsi,
		গণনা lws_callback_reasons reason,
		শূন্য *user,
		শূন্য *in,
		size_t len);

গণনা mosq_ws_protocols {
	PROTOCOL_HTTP = 0,
	PROTOCOL_MQTT,
	DEMO_PROTOCOL_গুনতি,
};

কাঠামো libws_http_তথ্য {
	FILE *fptr;
};

স্থির কাঠামো lws_protocols protocols[] = {
	/* first protocol must always be HTTP handler */
	{
		"http-only",                        /* name */
		callback_http,                      /* lws_callback_function */
		আকার (কাঠামো libws_http_তথ্য),    /* per_session_data_size */
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		ফাঁকা,                               /* user v1.4 on */
		WS_TX_BUF_আকার                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqtt",
		callback_mqtt,
		আকার(কাঠামো libws_mqtt_তথ্য),
		0,                                  /* rx_buffer_size */
		1,                                  /* id */
		ফাঁকা,                               /* user v1.4 on */
		WS_TX_BUF_আকার                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqttv3.1",
		callback_mqtt,
		আকার(কাঠামো libws_mqtt_তথ্য),
		0,                                  /* rx_buffer_size */
		2,                                  /* id */
		ফাঁকা,                               /* user v1.4 on */
		WS_TX_BUF_আকার                      /* tx_packet_size v2.3.0 */
	},
	{
		ফাঁকা,
		ফাঁকা,
		0,
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		ফাঁকা,                               /* user v1.4 on */
		0                                   /* tx_packet_size v2.3.0 */
	}
};


স্থির শূন্য easy_address(পূর্ণসংখ্যা sock, কাঠামো mosquitto *mosq)
{
	অক্ষর address[1024];

	যদি(!net__socket_get_address(sock, address, 1024, &mosq->remote_port)){
		mosq->address = mosquitto_strdup(address);
	}
}


স্থির পূর্ণসংখ্যা callback_mqtt(
		কাঠামো lws *wsi,
		গণনা lws_callback_reasons reason,
		শূন্য *user,
		শূন্য *in,
		size_t len)
{
	কাঠামো mosquitto *mosq = ফাঁকা;
	কাঠামো mosquitto__packet *packet;
	পূর্ণসংখ্যা গুনতি;
	unsigned পূর্ণসংখ্যা ucount;
	ধ্রুবক কাঠামো lws_protocols *p;
	কাঠামো libws_mqtt_তথ্য *u = (কাঠামো libws_mqtt_তথ্য *)user;
	size_t pos;
	uint8_t *buf;
	পূর্ণসংখ্যা rc;
	uint8_t byte;
	অক্ষর ip_addr_buff[1024];

	নির্বাচন(reason){
		ক্ষেত্র LWS_CALLBACK_ESTABLISHED:
			mosq = context__init();
			যদি(mosq){
				p = lws_get_protocol(wsi);
				mosq->listener = p->user;
				যদি(!mosq->listener){
					mosquitto_FREE(mosq);
					ফেরত -1;
				}
				mosq->wsi = wsi;
#ifdef WITH_TLS
				যদি(in){
					mosq->ssl = (SSL *)in;
					যদি(!mosq->listener->ssl_ctx){
						mosq->listener->ssl_ctx = SSL_get_SSL_CTX(mosq->ssl);
					}
				}
#endif
				u->mosq = mosq;
			}অন্যথা{
				ফেরত -1;
			}

			easy_address(lws_get_socket_fd(wsi), mosq);
			যদি(!mosq->address){
				/* getpeername and inet_ntop failed and not a bridge */
				mosquitto_FREE(mosq);
				u->mosq = ফাঁকা;
				ফেরত -1;
			}
			mosq->listener->client_গুনতি++;
			যদি((mosq->listener->max_connections > 0 && mosq->listener->client_গুনতি > mosq->listener->max_connections)
					|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (unsigned পূর্ণসংখ্যা)db.config->global_max_connections)){

				যদি(db.config->connection_messages == true){
					log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", mosq->address);
				}
				mosq->listener->client_গুনতি--;
				mosquitto_FREE(mosq->address);
				mosquitto_FREE(mosq);
				u->mosq = ফাঁকা;
				ফেরত -1;
			}
			mosq->sock = lws_get_socket_fd(wsi);
			HASH_ADD(hh_sock, db.contexts_by_sock, sock, আকার(mosq->sock), mosq);
			mux__new(mosq);
			বিরতি;

		ক্ষেত্র LWS_CALLBACK_CLOSED:
			যদি(!u){
				ফেরত -1;
			}
			mosq = u->mosq;
			যদি(mosq){
				যদি(mosq->sock != INVALID_SOCKET){
					HASH_DELETE(hh_sock, db.contexts_by_sock, mosq);
					mosq->sock = INVALID_SOCKET;
					mux__delete(mosq);
				}
				mosq->wsi = ফাঁকা;
#ifdef WITH_TLS
				mosq->ssl = ফাঁকা;
#endif
				do_disconnect(mosq, MOSQ_ERR_CONN_LOST);
			}
			বিরতি;

		ক্ষেত্র LWS_CALLBACK_SERVER_WRITEABLE:
			যদি(!u){
				ফেরত -1;
			}
			mosq = u->mosq;
			যদি(!mosq){
				ফেরত -1;
			}

			rc = db__message_write_inflight_out_latest(mosq);
			যদি(rc){
				ফেরত -1;
			}
			rc = db__message_write_queued_out(mosq);
			যদি(rc){
				ফেরত -1;
			}

			যখন(mosq->out_packet && !lws_send_pipe_choked(mosq->wsi)){
				packet = mosq->out_packet;
				গুনতি = lws_write(wsi, &packet->payload[packet->pos], packet->to_process, LWS_WRITE_দ্বিমিক);
				যদি(গুনতি < 0){
					যদি(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						ফেরত -1;
					}
					ফেরত 0;
				}
				ucount = (unsigned পূর্ণসংখ্যা)গুনতি;
#ifdef WITH_SYS_বৃক্ষ
				metrics__int_inc(mosq_গণক_bytes_sent, ucount);
#endif
				packet->to_process -= ucount;
				packet->pos += ucount;
				যদি(packet->to_process > 0){
					যদি(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						ফেরত -1;
					}
					বিরতি;
				}

#ifdef WITH_SYS_বৃক্ষ
				metrics__int_inc(mosq_গণক_messages_sent, 1);
				যদি(((packet->command)&0xF0) == CMD_PUBLISH){
					metrics__int_inc(mosq_গণক_mqtt_publish_sent, 1);
				}
#endif

				packet_get_পরবর্তী_out(mosq);
				mosquitto_FREE(packet);

				mosq->পরবর্তী_বার্তা_out = db.now_s + mosq->keepalive;
			}
			যদি(mosq->state == mosq_cs_disconnect_ws
					|| mosq->state == mosq_cs_disconnecting
					|| mosq->state == mosq_cs_disused){

				ফেরত -1;
			}
			যদি(mosq->out_packet){
				lws_callback_on_writable(mosq->wsi);
			}
			বিরতি;

		ক্ষেত্র LWS_CALLBACK_RECEIVE:
			যদি(!u || !u->mosq){
				ফেরত -1;
			}
			mosq = u->mosq;
			pos = 0;
			buf = (uint8_t *)in;
			metrics__int_inc(mosq_গণক_bytes_received, (int64_t)len);
			যখন(pos < len){
				যদি(!mosq->in_packet.command){
					mosq->in_packet.command = buf[pos];
					pos++;
					/* Clients must send CONNECT as their first command. */
					যদি(mosq->state == mosq_cs_new && (mosq->in_packet.command&0xF0) != CMD_CONNECT){
						ফেরত -1;
					}
				}
				যদি(mosq->in_packet.remaining_গুনতি <= 0){
					করো{
						যদি(pos == len){
							ফেরত 0;
						}
						byte = buf[pos];
						pos++;

						mosq->in_packet.remaining_গুনতি--;
						/* Max 4 bytes length for remaining length as defined by protocol.
						* Anything more likely means a broken/malicious client.
						*/
						যদি(mosq->in_packet.remaining_গুনতি < -4){
							ফেরত -1;
						}

						mosq->in_packet.remaining_দৈর্ঘ্য += (byte & 127) * mosq->in_packet.remaining_mult;
						mosq->in_packet.remaining_mult *= 128;
					}যখন((byte & 128) != 0);
					mosq->in_packet.remaining_গুনতি = (int8_t)(mosq->in_packet.remaining_গুনতি * -1);

					যদি(mosq->in_packet.remaining_দৈর্ঘ্য > 0){
						mosq->in_packet.payload = mosquitto_malloc(mosq->in_packet.remaining_দৈর্ঘ্য*আকার(uint8_t));
						যদি(!mosq->in_packet.payload){
							ফেরত -1;
						}
						mosq->in_packet.to_process = mosq->in_packet.remaining_দৈর্ঘ্য;
					}
				}
				যদি(mosq->in_packet.to_process>0){
					যদি((uint32_t)len - pos >= mosq->in_packet.to_process){
						memcpy(&mosq->in_packet.payload[mosq->in_packet.pos], &buf[pos], mosq->in_packet.to_process);
						mosq->in_packet.pos += mosq->in_packet.to_process;
						pos += mosq->in_packet.to_process;
						mosq->in_packet.to_process = 0;
					}অন্যথা{
						memcpy(&mosq->in_packet.payload[mosq->in_packet.pos], &buf[pos], len-pos);
						mosq->in_packet.pos += (uint32_t)(len-pos);
						mosq->in_packet.to_process -= (uint32_t)(len-pos);
						ফেরত 0;
					}
				}
				/* All data for this packet is read. */
				mosq->in_packet.pos = 0;

#ifdef WITH_SYS_বৃক্ষ
				metrics__int_inc(mosq_গণক_messages_received, 1);
#endif
				rc = handle__packet(mosq);

				/* Free data and reset values */
				packet__cleanup(&mosq->in_packet);

				keepalive__update(mosq);

				যদি(rc && mosq->out_packet){
					যদি(mosq->state != mosq_cs_disconnecting){
						mosquitto__set_state(mosq, mosq_cs_disconnect_ws);
					}
					lws_callback_on_writable(mosq->wsi);
				}অন্যথা যদি(rc){
					do_disconnect(mosq, MOSQ_ERR_CONN_LOST);
					ফেরত -1;
				}
			}
			বিরতি;

		পূর্বনির্ধারিত:
			বিরতি;
	}

	ফেরত 0;
}


স্থির অক্ষর *http__canonical_filename(
		কাঠামো lws *wsi,
		ধ্রুবক অক্ষর *in,
		ধ্রুবক অক্ষর *http_dir)
{
	size_t inlen, slen;
	অক্ষর *filename, *filename_canonical;

	inlen = strlen(in);
	যদি(in[inlen-1] == '/'){
		slen = strlen(http_dir) + inlen + strlen("/index.html") + 2;
	}অন্যথা{
		slen = strlen(http_dir) + inlen + 2;
	}
	filename = mosquitto_malloc(slen);
	যদি(!filename){
		lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, ফাঁকা);
		ফেরত ফাঁকা;
	}
	যদি(((অক্ষর *)in)[inlen-1] == '/'){
		snprintf(filename, slen, "%s%sindex.html", http_dir, (অক্ষর *)in);
	}অন্যথা{
		snprintf(filename, slen, "%s%s", http_dir, (অক্ষর *)in);
	}


	/* Get canonical path and check it is within our http_dir */
#ifdef WIN32
	filename_canonical = _fullpath(ফাঁকা, filename, 0);
	mosquitto_FREE(filename);
	যদি(!filename_canonical){
		lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, ফাঁকা);
		ফেরত ফাঁকা;
	}
#অন্যথা
	filename_canonical = realpath(filename, ফাঁকা);
	mosquitto_FREE(filename);
	যদি(!filename_canonical){
		যদি(errno == EACCES){
			lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, ফাঁকা);
		}অন্যথা যদি(errno == EINVAL || errno == EIO || errno == ELOOP){
			lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, ফাঁকা);
		}অন্যথা যদি(errno == ENAMETOOLONG){
			lws_return_http_status(wsi, HTTP_STATUS_REQ_URI_TOO_LONG, ফাঁকা);
		}অন্যথা যদি(errno == ENOENT || errno == ENOTDIR){
			lws_return_http_status(wsi, HTTP_STATUS_NOT_FOUND, ফাঁকা);
		}
		ফেরত ফাঁকা;
	}
#endif
	যদি(strncmp(http_dir, filename_canonical, strlen(http_dir))){
		/* Requested file isn't within http_dir, deny access. */
		SAFE_FREE(filename_canonical);
		lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, ফাঁকা);
		ফেরত ফাঁকা;
	}

	ফেরত filename_canonical;
}


স্থির পূর্ণসংখ্যা callback_http(
		কাঠামো lws *wsi,
		গণনা lws_callback_reasons reason,
		শূন্য *user,
		শূন্য *in,
		size_t len)
{
	কাঠামো libws_http_তথ্য *u = (কাঠামো libws_http_তথ্য *)user;
	কাঠামো libws_mqtt_hack *hack;
	অক্ষর *http_dir;
	size_t buflen;
	size_t wlen;
	পূর্ণসংখ্যা rc;
	অক্ষর *filename_canonical;
	unsigned অক্ষর buf[4096];
	কাঠামো stat filestat;
	কাঠামো mosquitto *mosq;
	কাঠামো lws_pollargs *pollargs = (কাঠামো lws_pollargs *)in;
	পূর্ণসংখ্যা hlen;

	/* FIXME - ssl cert verification is done here. */

	নির্বাচন(reason){
		ক্ষেত্র LWS_CALLBACK_HTTP:
			যদি(!u){
				ফেরত -1;
			}

			hack = (কাঠামো libws_mqtt_hack *)lws_context_user(lws_get_context(wsi));
			যদি(!hack){
				ফেরত -1;
			}
			http_dir = hack->http_dir;

			যদি(!http_dir){
				/* http disabled */
				ফেরত -1;
			}

			/* Forbid POST */
			যদি(lws_hdr_যোগ_দৈর্ঘ্য(wsi, WSI_TOKEN_POST_URI)){
				lws_return_http_status(wsi, HTTP_STATUS_METHOD_NOT_ALLOWED, ফাঁকা);
				ফেরত -1;
			}

			filename_canonical = http__canonical_filename(wsi, (অক্ষর *)in, http_dir);
			যদি(!filename_canonical){
				ফেরত -1;
			}

			u->fptr = fopen(filename_canonical, "rb");
			যদি(!u->fptr){
				SAFE_FREE(filename_canonical);
				lws_return_http_status(wsi, HTTP_STATUS_NOT_FOUND, ফাঁকা);
				ফেরত -1;
			}
			যদি(fstat(fileno(u->fptr), &filestat) < 0){
				SAFE_FREE(filename_canonical);
				lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, ফাঁকা);
				fclose(u->fptr);
				u->fptr = ফাঁকা;
				ফেরত -1;
			}


			যদি((filestat.st_mode & S_IFDIR) == S_IFDIR){
				fclose(u->fptr);
				u->fptr = ফাঁকা;
				SAFE_FREE(filename_canonical);

				/* FIXME - use header functions from lws 2.x */
				buflen = (size_t)snprintf((অক্ষর *)buf, 4096, "HTTP/1.0 302 OK\r\n"
						"Location: %s/\r\n\r\n",
						(অক্ষর *)in);
				ফেরত lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
			}

			যদি((filestat.st_mode & S_IFREG) != S_IFREG){
				lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, ফাঁকা);
				fclose(u->fptr);
				u->fptr = ফাঁকা;
				SAFE_FREE(filename_canonical);
				ফেরত -1;
			}

			log__printf(ফাঁকা, MOSQ_LOG_DEBUG, "http serving file \"%s\".", filename_canonical);
			SAFE_FREE(filename_canonical);
			/* FIXME - use header functions from lws 2.x */
			buflen = (size_t)snprintf((অক্ষর *)buf, 4096, "HTTP/1.0 200 OK\r\n"
					"Server: mosquitto\r\n"
					"Content-Length: %u\r\n\r\n",
					(unsigned পূর্ণসংখ্যা)filestat.st_আকার);
			যদি(lws_write(wsi, buf, buflen, LWS_WRITE_HTTP) < 0){
				fclose(u->fptr);
				u->fptr = ফাঁকা;
				ফেরত -1;
			}
			lws_callback_on_writable(wsi);
			বিরতি;

		ক্ষেত্র LWS_CALLBACK_HTTP_BODY:
			/* For extra POST data? */
			ফেরত -1;

		ক্ষেত্র LWS_CALLBACK_HTTP_BODY_COMPLETION:
			/* For end of extra POST data? */
			ফেরত -1;

		ক্ষেত্র LWS_CALLBACK_FILTER_HTTP_CONNECTION:
			/* Access control here */
			ফেরত 0;

#যদি LWS_LIBRARY_VERSION_সংখ্যা >= 3001000
		ক্ষেত্র LWS_CALLBACK_HTTP_CONFIRM_UPGRADE:
			hack = lws_context_user(lws_get_context(wsi));
			যদি(hack->listener->ws_origin_গুনতি){
				hlen = lws_hdr_যোগ_দৈর্ঘ্য(wsi, WSI_TOKEN_ORIGIN);
				যদি(hlen <= 0 || hlen >= (পূর্ণসংখ্যা)আকার(buf)){
					ফেরত -1;
				}
				জন্য(পূর্ণসংখ্যা i=0; i<hack->listener->ws_origin_গুনতি; i++){
					lws_hdr_copy(wsi, (অক্ষর *)buf, আকার(buf), WSI_TOKEN_ORIGIN);
					যদি((!strcmp(hack->listener->ws_origins[i], (অক্ষর *)buf))){
						ফেরত 0;
					}
				}
				ফেরত -1;
			}অন্যথা{
				ফেরত 0;
			}
#endif

		ক্ষেত্র LWS_CALLBACK_HTTP_WRITEABLE:
			/* Send our data here */
			যদি(u && u->fptr){
				করো{
					buflen = fread(buf, 1, আকার(buf), u->fptr);
					যদি(buflen < 1){
						fclose(u->fptr);
						u->fptr = ফাঁকা;
						ফেরত -1;
					}
					rc = lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
					যদি(rc < 0){
						ফেরত -1;
					}
					wlen = (size_t)rc;
					/* while still active, extend timeout */
					যদি(wlen){
						lws_set_timeout(wsi, PENDING_TIMEOUT_HTTP_CONTENT, 10);
					}
					যদি(wlen < buflen){
						যদি(fseek(u->fptr, (long)(buflen-wlen), SEEK_CUR) < 0){
							fclose(u->fptr);
							u->fptr = ফাঁকা;
							ফেরত -1;
						}
					}অন্যথা{
						যদি(buflen < আকার(buf)){
							fclose(u->fptr);
							u->fptr = ফাঁকা;
						}
					}
				}যখন(u->fptr && !lws_send_pipe_choked(wsi));
				lws_callback_on_writable(wsi);
			}অন্যথা{
				ফেরত -1;
			}
			বিরতি;

		ক্ষেত্র LWS_CALLBACK_CLOSED:
		ক্ষেত্র LWS_CALLBACK_CLOSED_HTTP:
		ক্ষেত্র LWS_CALLBACK_HTTP_FILE_COMPLETION:
			যদি(u && u->fptr){
				fclose(u->fptr);
				u->fptr = ফাঁকা;
			}
			বিরতি;

		ক্ষেত্র LWS_CALLBACK_ADD_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, আকার(pollargs->fd), mosq);
			যদি(mosq){
				যদি(pollargs->events & LWS_POLLOUT){
					mux__add_out(mosq);
				}অন্যথা{
					mux__remove_out(mosq);
				}
			}অন্যথা{
				যদি(pollargs->events & POLLIN){
					/* Assume this is a new listener */
					listeners__add_websockets(lws_get_context(wsi), pollargs->fd);
				}
			}
			বিরতি;

		ক্ষেত্র LWS_CALLBACK_DEL_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, আকার(pollargs->fd), mosq);
			যদি(mosq){
				mux__delete(mosq);
			}
			বিরতি;

		ক্ষেত্র LWS_CALLBACK_CHANGE_MODE_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, আকার(pollargs->fd), mosq);
			যদি(mosq){
				যদি(pollargs->events & LWS_POLLHUP){
					ফেরত 1;
				}অন্যথা যদি(pollargs->events & LWS_POLLOUT){
					mux__add_out(mosq);
				}অন্যথা{
					mux__remove_out(mosq);
				}
			}
			বিরতি;

#ifdef WITH_TLS
		ক্ষেত্র LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION:
			যদি(!len || (SSL_get_verify_ফলাফল((SSL *)in) != X509_V_OK)){
				ফেরত 1;
			}
			বিরতি;
#endif

		পূর্বনির্ধারিত:
			ফেরত 0;
	}

	ফেরত 0;
}


স্থির শূন্য log_wrap(পূর্ণসংখ্যা level, ধ্রুবক অক্ষর *সারি)
{
	অক্ষর *l = (অক্ষর *)সারি;
	UNUSED(level);
	l[strlen(সারি)-1] = '\0'; /* Remove \n */
	log__printf(ফাঁকা, MOSQ_LOG_WEBSOCKETS, "%s", l);
}


শূন্য mosq_websockets_init(কাঠামো mosquitto__listener *listener, ধ্রুবক কাঠামো mosquitto__config *conf)
{
	কাঠামো lws_context_creation_info info;
	কাঠামো lws_protocols *p;
	size_t protocol_গুনতি;
	পূর্ণসংখ্যা i;
	কাঠামো libws_mqtt_hack *user;

	/* Count valid protocols */
	জন্য(protocol_গুনতি=0; protocols[protocol_গুনতি].name; protocol_গুনতি++){
		;
	}

	p = mosquitto_calloc(protocol_গুনতি+1, আকার(কাঠামো lws_protocols));
	যদি(!p){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Out of memory.");
		ফেরত;
	}
	জন্য(i=0; protocols[i].name; i++){
		p[i].name = protocols[i].name;
		p[i].callback = protocols[i].callback;
		p[i].per_session_তথ্য_আকার = protocols[i].per_session_তথ্য_আকার;
		p[i].rx_বাফার_আকার = protocols[i].rx_বাফার_আকার;
		p[i].user = listener;
	}

	memset(&info, 0, আকার(info));
	info.iface = listener->host;
	info.port = listener->port;
	info.protocols = p;
	info.gid = -1;
	info.uid = -1;
#ifdef WITH_TLS
	যদি(listener->cafile){
		info.ssl_ca_filepath = listener->cafile;
	}অন্যথা যদি(listener->capath){
		log__printf(ফাঁকা, MOSQ_LOG_WARNING, "Warning: CA path option is not supported for websockets");
	}
	info.ssl_cert_filepath = listener->certfile;
	info.ssl_private_চাবি_filepath = listener->keyfile;
	info.ssl_cipher_list = listener->ciphers;
	/* HTTP 1 only, due to HTTP 2 issues in Firefox:
	   https://github.com/eclipse-mosquitto/mosquitto/issues/1211
	*/
	info.alpn = "h1";
#যদি defined(WITH_WEBSOCKETS) && LWS_LIBRARY_VERSION_সংখ্যা>=3001000
	info.tls1_3_plus_cipher_list = listener->ciphers_tls13;
#endif
	যদি(listener->require_certificate){
		info.options |= LWS_SERVER_OPTION_REQUIRE_VALID_OPENSSL_CLIENT_CERT;
	}
#endif

	info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
	যদি(listener->socket_domain == AF_INET){
		info.options |= LWS_SERVER_OPTION_DISABLE_IPV6;
	}
	info.max_http_header_তথ্য = conf->packet_বাফার_আকার;

	user = mosquitto_calloc(1, আকার(কাঠামো libws_mqtt_hack));
	যদি(!user){
		mosquitto_FREE(p);
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Out of memory.");
		ফেরত;
	}

	যদি(listener->http_dir){
#ifdef WIN32
		user->http_dir = _fullpath(ফাঁকা, listener->http_dir, 0);
#অন্যথা
		user->http_dir = realpath(listener->http_dir, ফাঁকা);
#endif
		যদি(!user->http_dir){
			mosquitto_FREE(user);
			mosquitto_FREE(p);
			log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to open http dir \"%s\".", listener->http_dir);
			ফেরত;
		}
	}
	user->listener = listener;

	info.user = user;
	info.pt_serv_buf_আকার = WS_SERV_BUF_আকার;
	listener->ws_protocol = p;

	lws_set_log_level(conf->websockets_log_level, log_wrap);

	log__printf(ফাঁকা, MOSQ_LOG_INFO, "Opening websockets listen socket on port %d.", listener->port);
	listener->ws_in_init = true;
	listener->ws_context = lws_create_context(&info);
	listener->ws_in_init = false;
}

#endif
#endif
