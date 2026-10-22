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

#অন্তর্ভাৱ "config.h"

#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif
#অন্তর্ভাৱ "mosquitto_internal.h"
#অন্তর্ভাৱ "mosquitto_broker_internal.h"
#অন্তর্ভাৱ "mosquitto/mqtt_protocol.h"
#অন্তর্ভাৱ "packet_mosq.h"
#অন্তর্ভাৱ "sys_tree.h"
#অন্তর্ভাৱ "util_mosq.h"

#অন্তর্ভাৱ <stdlib.h>
#অন্তর্ভাৱ <errno.h>
#অন্তর্ভাৱ <sys/stat.h>

#ifndef WIN32
#  include <sys/socket.h>
#endif

#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
/* Be careful if changing these, if TX is not bigger than SERV then there can
 * be very large write performance penalties.
 */
#define WS_SERV_BUF_SIZE 4096
#define WS_TX_BUF_SIZE (WS_SERV_BUF_SIZE*2)

স্থির পূর্ণাঙ্ক callback_mqtt(
		সংরচনা lws *wsi,
		গণনা lws_callback_reasons reason,
		শূন্য *user,
		শূন্য *in,
		size_t len);

স্থির পূর্ণাঙ্ক callback_http(
		সংরচনা lws *wsi,
		গণনা lws_callback_reasons reason,
		শূন্য *user,
		শূন্য *in,
		size_t len);

গণনা mosq_ws_protocols {
	PROTOCOL_HTTP = 0,
	PROTOCOL_MQTT,
	DEMO_PROTOCOL_গুনতি,
};

সংরচনা libws_http_data {
	FILE *fptr;
};

স্থির সংরচনা lws_protocols protocols[] = {
	/* first protocol must always be HTTP handler */
	{
		"http-only",                        /* name */
		callback_http,                      /* lws_callback_function */
		পরিমাণ (সংরচনা libws_http_data),    /* per_session_data_size */
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		রিক্ত,                               /* user v1.4 on */
		WS_TX_BUF_SIZE                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqtt",
		callback_mqtt,
		পরিমাণ(সংরচনা libws_mqtt_data),
		0,                                  /* rx_buffer_size */
		1,                                  /* id */
		রিক্ত,                               /* user v1.4 on */
		WS_TX_BUF_SIZE                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqttv3.1",
		callback_mqtt,
		পরিমাণ(সংরচনা libws_mqtt_data),
		0,                                  /* rx_buffer_size */
		2,                                  /* id */
		রিক্ত,                               /* user v1.4 on */
		WS_TX_BUF_SIZE                      /* tx_packet_size v2.3.0 */
	},
	{
		রিক্ত,
		রিক্ত,
		0,
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		রিক্ত,                               /* user v1.4 on */
		0                                   /* tx_packet_size v2.3.0 */
	}
};


স্থির শূন্য easy_address(পূর্ণাঙ্ক sock, সংরচনা mosquitto *mosq)
{
	ৱর্ণ address[1024];

	যদি(!net__socket_get_address(sock, address, 1024, &mosq->remote_port)){
		mosq->address = mosquitto_strdup(address);
	}
}


স্থির পূর্ণাঙ্ক callback_mqtt(
		সংরচনা lws *wsi,
		গণনা lws_callback_reasons reason,
		শূন্য *user,
		শূন্য *in,
		size_t len)
{
	সংরচনা mosquitto *mosq = রিক্ত;
	সংরচনা mosquitto__packet *packet;
	পূর্ণাঙ্ক গুনতি;
	unsigned পূর্ণাঙ্ক ucount;
	ধ্রুৱ সংরচনা lws_protocols *p;
	সংরচনা libws_mqtt_data *u = (সংরচনা libws_mqtt_data *)user;
	size_t pos;
	uint8_t *buf;
	পূর্ণাঙ্ক rc;
	uint8_t byte;
	ৱর্ণ ip_addr_buff[1024];

	ৱিকল্প(reason){
		পক্ষ LWS_CALLBACK_ESTABLISHED:
			mosq = context__init();
			যদি(mosq){
				p = lws_get_protocol(wsi);
				mosq->listener = p->user;
				যদি(!mosq->listener){
					mosquitto_FREE(mosq);
					ঘূৰাই -1;
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
				ঘূৰাই -1;
			}

			easy_address(lws_get_socket_fd(wsi), mosq);
			যদি(!mosq->address){
				/* getpeername and inet_ntop failed and not a bridge */
				mosquitto_FREE(mosq);
				u->mosq = রিক্ত;
				ঘূৰাই -1;
			}
			mosq->listener->client_গুনতি++;
			যদি((mosq->listener->max_connections > 0 && mosq->listener->client_গুনতি > mosq->listener->max_connections)
					|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (unsigned পূর্ণাঙ্ক)db.config->global_max_connections)){

				যদি(db.config->connection_messages == true){
					log__printf(রিক্ত, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", mosq->address);
				}
				mosq->listener->client_গুনতি--;
				mosquitto_FREE(mosq->address);
				mosquitto_FREE(mosq);
				u->mosq = রিক্ত;
				ঘূৰাই -1;
			}
			mosq->sock = lws_get_socket_fd(wsi);
			HASH_ADD(hh_sock, db.contexts_by_sock, sock, পরিমাণ(mosq->sock), mosq);
			mux__new(mosq);
			ভঙ্গ;

		পক্ষ LWS_CALLBACK_CLOSED:
			যদি(!u){
				ঘূৰাই -1;
			}
			mosq = u->mosq;
			যদি(mosq){
				যদি(mosq->sock != INVALID_SOCKET){
					HASH_DELETE(hh_sock, db.contexts_by_sock, mosq);
					mosq->sock = INVALID_SOCKET;
					mux__delete(mosq);
				}
				mosq->wsi = রিক্ত;
#ifdef WITH_TLS
				mosq->ssl = রিক্ত;
#endif
				do_disconnect(mosq, MOSQ_ERR_CONN_LOST);
			}
			ভঙ্গ;

		পক্ষ LWS_CALLBACK_SERVER_WRITEABLE:
			যদি(!u){
				ঘূৰাই -1;
			}
			mosq = u->mosq;
			যদি(!mosq){
				ঘূৰাই -1;
			}

			rc = db__message_write_inflight_out_latest(mosq);
			যদি(rc){
				ঘূৰাই -1;
			}
			rc = db__message_write_queued_out(mosq);
			যদি(rc){
				ঘূৰাই -1;
			}

			যেতিয়ালৈ(mosq->out_packet && !lws_send_pipe_choked(mosq->wsi)){
				packet = mosq->out_packet;
				গুনতি = lws_write(wsi, &packet->payload[packet->pos], packet->to_process, LWS_WRITE_দ্বিমিক);
				যদি(গুনতি < 0){
					যদি(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						ঘূৰাই -1;
					}
					ঘূৰাই 0;
				}
				ucount = (unsigned পূর্ণাঙ্ক)গুনতি;
#ifdef WITH_SYS_বৃক্ষ
				metrics__int_inc(mosq_counter_bytes_sent, ucount);
#endif
				packet->to_process -= ucount;
				packet->pos += ucount;
				যদি(packet->to_process > 0){
					যদি(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						ঘূৰাই -1;
					}
					ভঙ্গ;
				}

#ifdef WITH_SYS_বৃক্ষ
				metrics__int_inc(mosq_counter_messages_sent, 1);
				যদি(((packet->command)&0xF0) == CMD_PUBLISH){
					metrics__int_inc(mosq_counter_mqtt_publish_sent, 1);
				}
#endif

				packet__get_next_out(mosq);
				mosquitto_FREE(packet);

				mosq->next_বাৰ্তা_out = db.now_s + mosq->keepalive;
			}
			যদি(mosq->state == mosq_cs_disconnect_ws
					|| mosq->state == mosq_cs_disconnecting
					|| mosq->state == mosq_cs_disused){

				ঘূৰাই -1;
			}
			যদি(mosq->out_packet){
				lws_callback_on_writable(mosq->wsi);
			}
			ভঙ্গ;

		পক্ষ LWS_CALLBACK_RECEIVE:
			যদি(!u || !u->mosq){
				ঘূৰাই -1;
			}
			mosq = u->mosq;
			pos = 0;
			buf = (uint8_t *)in;
			metrics__int_inc(mosq_counter_bytes_received, (int64_t)len);
			যেতিয়ালৈ(pos < len){
				যদি(!mosq->in_packet.command){
					mosq->in_packet.command = buf[pos];
					pos++;
					/* Clients must send CONNECT as their first command. */
					যদি(mosq->state == mosq_cs_new && (mosq->in_packet.command&0xF0) != CMD_CONNECT){
						ঘূৰাই -1;
					}
				}
				যদি(mosq->in_packet.remaining_গুনতি <= 0){
					কৰা{
						যদি(pos == len){
							ঘূৰাই 0;
						}
						byte = buf[pos];
						pos++;

						mosq->in_packet.remaining_গুনতি--;
						/* Max 4 bytes length for remaining length as defined by protocol.
						* Anything more likely means a broken/malicious client.
						*/
						যদি(mosq->in_packet.remaining_গুনতি < -4){
							ঘূৰাই -1;
						}

						mosq->in_packet.remaining_length += (byte & 127) * mosq->in_packet.remaining_mult;
						mosq->in_packet.remaining_mult *= 128;
					}যেতিয়ালৈ((byte & 128) != 0);
					mosq->in_packet.remaining_গুনতি = (int8_t)(mosq->in_packet.remaining_গুনতি * -1);

					যদি(mosq->in_packet.remaining_length > 0){
						mosq->in_packet.payload = mosquitto_malloc(mosq->in_packet.remaining_length*পরিমাণ(uint8_t));
						যদি(!mosq->in_packet.payload){
							ঘূৰাই -1;
						}
						mosq->in_packet.to_process = mosq->in_packet.remaining_length;
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
						ঘূৰাই 0;
					}
				}
				/* All data for this packet is read. */
				mosq->in_packet.pos = 0;

#ifdef WITH_SYS_বৃক্ষ
				metrics__int_inc(mosq_counter_messages_received, 1);
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
					ঘূৰাই -1;
				}
			}
			ভঙ্গ;

		স্ৱভাৱ:
			ভঙ্গ;
	}

	ঘূৰাই 0;
}


স্থির ৱর্ণ *http__canonical_filename(
		সংরচনা lws *wsi,
		ধ্রুৱ ৱর্ণ *in,
		ধ্রুৱ ৱর্ণ *http_dir)
{
	size_t inlen, slen;
	ৱর্ণ *filename, *filename_canonical;

	inlen = strlen(in);
	যদি(in[inlen-1] == '/'){
		slen = strlen(http_dir) + inlen + strlen("/index.html") + 2;
	}অন্যথা{
		slen = strlen(http_dir) + inlen + 2;
	}
	filename = mosquitto_malloc(slen);
	যদি(!filename){
		lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, রিক্ত);
		ঘূৰাই রিক্ত;
	}
	যদি(((ৱর্ণ *)in)[inlen-1] == '/'){
		snprintf(filename, slen, "%s%sindex.html", http_dir, (ৱর্ণ *)in);
	}অন্যথা{
		snprintf(filename, slen, "%s%s", http_dir, (ৱর্ণ *)in);
	}


	/* Get canonical path and check it is within our http_dir */
#ifdef WIN32
	filename_canonical = _fullpath(রিক্ত, filename, 0);
	mosquitto_FREE(filename);
	যদি(!filename_canonical){
		lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, রিক্ত);
		ঘূৰাই রিক্ত;
	}
#অন্যথা
	filename_canonical = realpath(filename, রিক্ত);
	mosquitto_FREE(filename);
	যদি(!filename_canonical){
		যদি(errno == EACCES){
			lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, রিক্ত);
		}অন্যথা যদি(errno == EINVAL || errno == EIO || errno == ELOOP){
			lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, রিক্ত);
		}অন্যথা যদি(errno == ENAMETOOLONG){
			lws_return_http_status(wsi, HTTP_STATUS_REQ_URI_TOO_LONG, রিক্ত);
		}অন্যথা যদি(errno == ENOENT || errno == ENOTDIR){
			lws_return_http_status(wsi, HTTP_STATUS_NOT_FOUND, রিক্ত);
		}
		ঘূৰাই রিক্ত;
	}
#endif
	যদি(strncmp(http_dir, filename_canonical, strlen(http_dir))){
		/* Requested file isn't within http_dir, deny access. */
		SAFE_FREE(filename_canonical);
		lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, রিক্ত);
		ঘূৰাই রিক্ত;
	}

	ঘূৰাই filename_canonical;
}


স্থির পূর্ণাঙ্ক callback_http(
		সংরচনা lws *wsi,
		গণনা lws_callback_reasons reason,
		শূন্য *user,
		শূন্য *in,
		size_t len)
{
	সংরচনা libws_http_data *u = (সংরচনা libws_http_data *)user;
	সংরচনা libws_mqtt_hack *hack;
	ৱর্ণ *http_dir;
	size_t buflen;
	size_t wlen;
	পূর্ণাঙ্ক rc;
	ৱর্ণ *filename_canonical;
	unsigned ৱর্ণ buf[4096];
	সংরচনা stat filestat;
	সংরচনা mosquitto *mosq;
	সংরচনা lws_pollargs *pollargs = (সংরচনা lws_pollargs *)in;
	পূর্ণাঙ্ক hlen;

	/* FIXME - ssl cert verification is done here. */

	ৱিকল্প(reason){
		পক্ষ LWS_CALLBACK_HTTP:
			যদি(!u){
				ঘূৰাই -1;
			}

			hack = (সংরচনা libws_mqtt_hack *)lws_context_user(lws_get_context(wsi));
			যদি(!hack){
				ঘূৰাই -1;
			}
			http_dir = hack->http_dir;

			যদি(!http_dir){
				/* http disabled */
				ঘূৰাই -1;
			}

			/* Forbid POST */
			যদি(lws_hdr_total_length(wsi, WSI_TOKEN_POST_URI)){
				lws_return_http_status(wsi, HTTP_STATUS_METHOD_NOT_ALLOWED, রিক্ত);
				ঘূৰাই -1;
			}

			filename_canonical = http__canonical_filename(wsi, (ৱর্ণ *)in, http_dir);
			যদি(!filename_canonical){
				ঘূৰাই -1;
			}

			u->fptr = fopen(filename_canonical, "rb");
			যদি(!u->fptr){
				SAFE_FREE(filename_canonical);
				lws_return_http_status(wsi, HTTP_STATUS_NOT_FOUND, রিক্ত);
				ঘূৰাই -1;
			}
			যদি(fstat(fileno(u->fptr), &filestat) < 0){
				SAFE_FREE(filename_canonical);
				lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, রিক্ত);
				fclose(u->fptr);
				u->fptr = রিক্ত;
				ঘূৰাই -1;
			}


			যদি((filestat.st_mode & S_IFDIR) == S_IFDIR){
				fclose(u->fptr);
				u->fptr = রিক্ত;
				SAFE_FREE(filename_canonical);

				/* FIXME - use header functions from lws 2.x */
				buflen = (size_t)snprintf((ৱর্ণ *)buf, 4096, "HTTP/1.0 302 OK\r\n"
						"Location: %s/\r\n\r\n",
						(ৱর্ণ *)in);
				ঘূৰাই lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
			}

			যদি((filestat.st_mode & S_IFREG) != S_IFREG){
				lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, রিক্ত);
				fclose(u->fptr);
				u->fptr = রিক্ত;
				SAFE_FREE(filename_canonical);
				ঘূৰাই -1;
			}

			log__printf(রিক্ত, MOSQ_LOG_DEBUG, "http serving file \"%s\".", filename_canonical);
			SAFE_FREE(filename_canonical);
			/* FIXME - use header functions from lws 2.x */
			buflen = (size_t)snprintf((ৱর্ণ *)buf, 4096, "HTTP/1.0 200 OK\r\n"
					"Server: mosquitto\r\n"
					"Content-Length: %u\r\n\r\n",
					(unsigned পূর্ণাঙ্ক)filestat.st_size);
			যদি(lws_write(wsi, buf, buflen, LWS_WRITE_HTTP) < 0){
				fclose(u->fptr);
				u->fptr = রিক্ত;
				ঘূৰাই -1;
			}
			lws_callback_on_writable(wsi);
			ভঙ্গ;

		পক্ষ LWS_CALLBACK_HTTP_BODY:
			/* For extra POST data? */
			ঘূৰাই -1;

		পক্ষ LWS_CALLBACK_HTTP_BODY_COMPLETION:
			/* For end of extra POST data? */
			ঘূৰাই -1;

		পক্ষ LWS_CALLBACK_FILTER_HTTP_CONNECTION:
			/* Access control here */
			ঘূৰাই 0;

#যদি LWS_LIBRARY_VERSION_সংখ্যা >= 3001000
		পক্ষ LWS_CALLBACK_HTTP_CONFIRM_UPGRADE:
			hack = lws_context_user(lws_get_context(wsi));
			যদি(hack->listener->ws_origin_গুনতি){
				hlen = lws_hdr_total_length(wsi, WSI_TOKEN_ORIGIN);
				যদি(hlen <= 0 || hlen >= (পূর্ণাঙ্ক)পরিমাণ(buf)){
					ঘূৰাই -1;
				}
				বাবে(পূর্ণাঙ্ক i=0; i<hack->listener->ws_origin_গুনতি; i++){
					lws_hdr_copy(wsi, (ৱর্ণ *)buf, পরিমাণ(buf), WSI_TOKEN_ORIGIN);
					যদি((!strcmp(hack->listener->ws_origins[i], (ৱর্ণ *)buf))){
						ঘূৰাই 0;
					}
				}
				ঘূৰাই -1;
			}অন্যথা{
				ঘূৰাই 0;
			}
#endif

		পক্ষ LWS_CALLBACK_HTTP_WRITEABLE:
			/* Send our data here */
			যদি(u && u->fptr){
				কৰা{
					buflen = fread(buf, 1, পরিমাণ(buf), u->fptr);
					যদি(buflen < 1){
						fclose(u->fptr);
						u->fptr = রিক্ত;
						ঘূৰাই -1;
					}
					rc = lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
					যদি(rc < 0){
						ঘূৰাই -1;
					}
					wlen = (size_t)rc;
					/* while still active, extend timeout */
					যদি(wlen){
						lws_set_timeout(wsi, PENDING_TIMEOUT_HTTP_CONTENT, 10);
					}
					যদি(wlen < buflen){
						যদি(fseek(u->fptr, (long)(buflen-wlen), SEEK_CUR) < 0){
							fclose(u->fptr);
							u->fptr = রিক্ত;
							ঘূৰাই -1;
						}
					}অন্যথা{
						যদি(buflen < পরিমাণ(buf)){
							fclose(u->fptr);
							u->fptr = রিক্ত;
						}
					}
				}যেতিয়ালৈ(u->fptr && !lws_send_pipe_choked(wsi));
				lws_callback_on_writable(wsi);
			}অন্যথা{
				ঘূৰাই -1;
			}
			ভঙ্গ;

		পক্ষ LWS_CALLBACK_CLOSED:
		পক্ষ LWS_CALLBACK_CLOSED_HTTP:
		পক্ষ LWS_CALLBACK_HTTP_FILE_COMPLETION:
			যদি(u && u->fptr){
				fclose(u->fptr);
				u->fptr = রিক্ত;
			}
			ভঙ্গ;

		পক্ষ LWS_CALLBACK_ADD_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, পরিমাণ(pollargs->fd), mosq);
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
			ভঙ্গ;

		পক্ষ LWS_CALLBACK_DEL_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, পরিমাণ(pollargs->fd), mosq);
			যদি(mosq){
				mux__delete(mosq);
			}
			ভঙ্গ;

		পক্ষ LWS_CALLBACK_CHANGE_MODE_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, পরিমাণ(pollargs->fd), mosq);
			যদি(mosq){
				যদি(pollargs->events & LWS_POLLHUP){
					ঘূৰাই 1;
				}অন্যথা যদি(pollargs->events & LWS_POLLOUT){
					mux__add_out(mosq);
				}অন্যথা{
					mux__remove_out(mosq);
				}
			}
			ভঙ্গ;

#ifdef WITH_TLS
		পক্ষ LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION:
			যদি(!len || (SSL_get_verify_result((SSL *)in) != X509_V_OK)){
				ঘূৰাই 1;
			}
			ভঙ্গ;
#endif

		স্ৱভাৱ:
			ঘূৰাই 0;
	}

	ঘূৰাই 0;
}


স্থির শূন্য log_wrap(পূর্ণাঙ্ক level, ধ্রুৱ ৱর্ণ *সাৰি)
{
	ৱর্ণ *l = (ৱর্ণ *)সাৰি;
	UNUSED(level);
	l[strlen(সাৰি)-1] = '\0'; /* Remove \n */
	log__printf(রিক্ত, MOSQ_LOG_WEBSOCKETS, "%s", l);
}


শূন্য mosq_websockets_init(সংরচনা mosquitto__listener *listener, ধ্রুৱ সংরচনা mosquitto__config *conf)
{
	সংরচনা lws_context_creation_info info;
	সংরচনা lws_protocols *p;
	size_t protocol_গুনতি;
	পূর্ণাঙ্ক i;
	সংরচনা libws_mqtt_hack *user;

	/* Count valid protocols */
	বাবে(protocol_গুনতি=0; protocols[protocol_গুনতি].name; protocol_গুনতি++){
		;
	}

	p = mosquitto_calloc(protocol_গুনতি+1, পরিমাণ(সংরচনা lws_protocols));
	যদি(!p){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Out of memory.");
		ঘূৰাই;
	}
	বাবে(i=0; protocols[i].name; i++){
		p[i].name = protocols[i].name;
		p[i].callback = protocols[i].callback;
		p[i].per_session_data_size = protocols[i].per_session_data_size;
		p[i].rx_buffer_size = protocols[i].rx_buffer_size;
		p[i].user = listener;
	}

	memset(&info, 0, পরিমাণ(info));
	info.iface = listener->host;
	info.port = listener->port;
	info.protocols = p;
	info.gid = -1;
	info.uid = -1;
#ifdef WITH_TLS
	যদি(listener->cafile){
		info.ssl_ca_filepath = listener->cafile;
	}অন্যথা যদি(listener->capath){
		log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: CA path option is not supported for websockets");
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
	info.max_http_header_data = conf->packet_buffer_size;

	user = mosquitto_calloc(1, পরিমাণ(সংরচনা libws_mqtt_hack));
	যদি(!user){
		mosquitto_FREE(p);
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Out of memory.");
		ঘূৰাই;
	}

	যদি(listener->http_dir){
#ifdef WIN32
		user->http_dir = _fullpath(রিক্ত, listener->http_dir, 0);
#অন্যথা
		user->http_dir = realpath(listener->http_dir, রিক্ত);
#endif
		যদি(!user->http_dir){
			mosquitto_FREE(user);
			mosquitto_FREE(p);
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to open http dir \"%s\".", listener->http_dir);
			ঘূৰাই;
		}
	}
	user->listener = listener;

	info.user = user;
	info.pt_serv_buf_size = WS_SERV_BUF_SIZE;
	listener->ws_protocol = p;

	lws_set_log_level(conf->websockets_log_level, log_wrap);

	log__printf(রিক্ত, MOSQ_LOG_INFO, "Opening websockets listen socket on port %d.", listener->port);
	listener->ws_in_init = true;
	listener->ws_context = lws_create_context(&info);
	listener->ws_in_init = false;
}

#endif
#endif
