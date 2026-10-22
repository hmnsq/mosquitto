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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif
#ಅನ್ತರ್ಭಾವ "mosquitto_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "packet_mosq.h"
#ಅನ್ತರ್ಭಾವ "sys_tree.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"

#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>

#ifndef WIN32
#  include <sys/socket.h>
#endif

#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
/* Be careful if changing these, if TX is not bigger than SERV then there can
 * be very large write performance penalties.
 */
#define WS_SERV_BUF_SIZE 4096
#define WS_TX_BUF_SIZE (WS_SERV_BUF_SIZE*2)

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ callback_mqtt(
		ಸಂರಚನಾ lws *wsi,
		ಗಣನಾ lws_callback_reasons reason,
		ಶೂನ್ಯ *user,
		ಶೂನ್ಯ *in,
		size_t len);

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ callback_http(
		ಸಂರಚನಾ lws *wsi,
		ಗಣನಾ lws_callback_reasons reason,
		ಶೂನ್ಯ *user,
		ಶೂನ್ಯ *in,
		size_t len);

ಗಣನಾ mosq_ws_protocols {
	PROTOCOL_HTTP = 0,
	PROTOCOL_MQTT,
	DEMO_PROTOCOL_ಎಣಿಕೆ,
};

ಸಂರಚನಾ libws_http_data {
	FILE *fptr;
};

ಸ್ಥಿರ ಸಂರಚನಾ lws_protocols protocols[] = {
	/* first protocol must always be HTTP handler */
	{
		"http-only",                        /* name */
		callback_http,                      /* lws_callback_function */
		ಪರಿಮಾಣ (ಸಂರಚನಾ libws_http_data),    /* per_session_data_size */
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		ರಿಕ್ತ,                               /* user v1.4 on */
		WS_TX_BUF_SIZE                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqtt",
		callback_mqtt,
		ಪರಿಮಾಣ(ಸಂರಚನಾ libws_mqtt_data),
		0,                                  /* rx_buffer_size */
		1,                                  /* id */
		ರಿಕ್ತ,                               /* user v1.4 on */
		WS_TX_BUF_SIZE                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqttv3.1",
		callback_mqtt,
		ಪರಿಮಾಣ(ಸಂರಚನಾ libws_mqtt_data),
		0,                                  /* rx_buffer_size */
		2,                                  /* id */
		ರಿಕ್ತ,                               /* user v1.4 on */
		WS_TX_BUF_SIZE                      /* tx_packet_size v2.3.0 */
	},
	{
		ರಿಕ್ತ,
		ರಿಕ್ತ,
		0,
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		ರಿಕ್ತ,                               /* user v1.4 on */
		0                                   /* tx_packet_size v2.3.0 */
	}
};


ಸ್ಥಿರ ಶೂನ್ಯ easy_address(ಪೂರ್ಣಾಙ್ಕ sock, ಸಂರಚನಾ mosquitto *mosq)
{
	ವರ್ಣ address[1024];

	ಯದಿ(!net__socket_get_address(sock, address, 1024, &mosq->remote_port)){
		mosq->address = mosquitto_strdup(address);
	}
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ callback_mqtt(
		ಸಂರಚನಾ lws *wsi,
		ಗಣನಾ lws_callback_reasons reason,
		ಶೂನ್ಯ *user,
		ಶೂನ್ಯ *in,
		size_t len)
{
	ಸಂರಚನಾ mosquitto *mosq = ರಿಕ್ತ;
	ಸಂರಚನಾ mosquitto__packet *packet;
	ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ;
	unsigned ಪೂರ್ಣಾಙ್ಕ ucount;
	ಧ್ರುವ ಸಂರಚನಾ lws_protocols *p;
	ಸಂರಚನಾ libws_mqtt_data *u = (ಸಂರಚನಾ libws_mqtt_data *)user;
	size_t pos;
	uint8_t *buf;
	ಪೂರ್ಣಾಙ್ಕ rc;
	uint8_t byte;
	ವರ್ಣ ip_addr_buff[1024];

	ವಿಕಲ್ಪ(reason){
		ಪಕ್ಷ LWS_CALLBACK_ESTABLISHED:
			mosq = context__init();
			ಯದಿ(mosq){
				p = lws_get_protocol(wsi);
				mosq->listener = p->user;
				ಯದಿ(!mosq->listener){
					mosquitto_FREE(mosq);
					ಪ್ರತ್ಯಾಗಮ -1;
				}
				mosq->wsi = wsi;
#ifdef WITH_TLS
				ಯದಿ(in){
					mosq->ssl = (SSL *)in;
					ಯದಿ(!mosq->listener->ssl_ctx){
						mosq->listener->ssl_ctx = SSL_get_SSL_CTX(mosq->ssl);
					}
				}
#endif
				u->mosq = mosq;
			}ಅನ್ಯಥಾ{
				ಪ್ರತ್ಯಾಗಮ -1;
			}

			easy_address(lws_get_socket_fd(wsi), mosq);
			ಯದಿ(!mosq->address){
				/* getpeername and inet_ntop failed and not a bridge */
				mosquitto_FREE(mosq);
				u->mosq = ರಿಕ್ತ;
				ಪ್ರತ್ಯಾಗಮ -1;
			}
			mosq->listener->client_ಎಣಿಕೆ++;
			ಯದಿ((mosq->listener->max_connections > 0 && mosq->listener->client_ಎಣಿಕೆ > mosq->listener->max_connections)
					|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (unsigned ಪೂರ್ಣಾಙ್ಕ)db.config->global_max_connections)){

				ಯದಿ(db.config->connection_messages == true){
					log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", mosq->address);
				}
				mosq->listener->client_ಎಣಿಕೆ--;
				mosquitto_FREE(mosq->address);
				mosquitto_FREE(mosq);
				u->mosq = ರಿಕ್ತ;
				ಪ್ರತ್ಯಾಗಮ -1;
			}
			mosq->sock = lws_get_socket_fd(wsi);
			HASH_ADD(hh_sock, db.contexts_by_sock, sock, ಪರಿಮಾಣ(mosq->sock), mosq);
			mux__new(mosq);
			ಭಙ್ಗ;

		ಪಕ್ಷ LWS_CALLBACK_CLOSED:
			ಯದಿ(!u){
				ಪ್ರತ್ಯಾಗಮ -1;
			}
			mosq = u->mosq;
			ಯದಿ(mosq){
				ಯದಿ(mosq->sock != INVALID_SOCKET){
					HASH_DELETE(hh_sock, db.contexts_by_sock, mosq);
					mosq->sock = INVALID_SOCKET;
					mux__delete(mosq);
				}
				mosq->wsi = ರಿಕ್ತ;
#ifdef WITH_TLS
				mosq->ssl = ರಿಕ್ತ;
#endif
				do_disconnect(mosq, MOSQ_ERR_CONN_LOST);
			}
			ಭಙ್ಗ;

		ಪಕ್ಷ LWS_CALLBACK_SERVER_WRITEABLE:
			ಯದಿ(!u){
				ಪ್ರತ್ಯಾಗಮ -1;
			}
			mosq = u->mosq;
			ಯದಿ(!mosq){
				ಪ್ರತ್ಯಾಗಮ -1;
			}

			rc = db__message_write_inflight_out_latest(mosq);
			ಯದಿ(rc){
				ಪ್ರತ್ಯಾಗಮ -1;
			}
			rc = db__message_write_queued_out(mosq);
			ಯದಿ(rc){
				ಪ್ರತ್ಯಾಗಮ -1;
			}

			ಯಾವತ್(mosq->out_packet && !lws_send_pipe_choked(mosq->wsi)){
				packet = mosq->out_packet;
				ಎಣಿಕೆ = lws_write(wsi, &packet->payload[packet->pos], packet->to_process, LWS_WRITE_ದ್ವಿಮಾನ);
				ಯದಿ(ಎಣಿಕೆ < 0){
					ಯದಿ(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						ಪ್ರತ್ಯಾಗಮ -1;
					}
					ಪ್ರತ್ಯಾಗಮ 0;
				}
				ucount = (unsigned ಪೂರ್ಣಾಙ್ಕ)ಎಣಿಕೆ;
#ifdef WITH_SYS_ವೃಕ್ಷ
				metrics__int_inc(mosq_counter_bytes_sent, ucount);
#endif
				packet->to_process -= ucount;
				packet->pos += ucount;
				ಯದಿ(packet->to_process > 0){
					ಯದಿ(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						ಪ್ರತ್ಯಾಗಮ -1;
					}
					ಭಙ್ಗ;
				}

#ifdef WITH_SYS_ವೃಕ್ಷ
				metrics__int_inc(mosq_counter_messages_sent, 1);
				ಯದಿ(((packet->command)&0xF0) == CMD_PUBLISH){
					metrics__int_inc(mosq_counter_mqtt_publish_sent, 1);
				}
#endif

				packet__get_next_out(mosq);
				mosquitto_FREE(packet);

				mosq->next_ಸಂದೇಶ_out = db.now_s + mosq->keepalive;
			}
			ಯದಿ(mosq->state == mosq_cs_disconnect_ws
					|| mosq->state == mosq_cs_disconnecting
					|| mosq->state == mosq_cs_disused){

				ಪ್ರತ್ಯಾಗಮ -1;
			}
			ಯದಿ(mosq->out_packet){
				lws_callback_on_writable(mosq->wsi);
			}
			ಭಙ್ಗ;

		ಪಕ್ಷ LWS_CALLBACK_RECEIVE:
			ಯದಿ(!u || !u->mosq){
				ಪ್ರತ್ಯಾಗಮ -1;
			}
			mosq = u->mosq;
			pos = 0;
			buf = (uint8_t *)in;
			metrics__int_inc(mosq_counter_bytes_received, (int64_t)len);
			ಯಾವತ್(pos < len){
				ಯದಿ(!mosq->in_packet.command){
					mosq->in_packet.command = buf[pos];
					pos++;
					/* Clients must send CONNECT as their first command. */
					ಯದಿ(mosq->state == mosq_cs_new && (mosq->in_packet.command&0xF0) != CMD_CONNECT){
						ಪ್ರತ್ಯಾಗಮ -1;
					}
				}
				ಯದಿ(mosq->in_packet.remaining_ಎಣಿಕೆ <= 0){
					ಕುರು{
						ಯದಿ(pos == len){
							ಪ್ರತ್ಯಾಗಮ 0;
						}
						byte = buf[pos];
						pos++;

						mosq->in_packet.remaining_ಎಣಿಕೆ--;
						/* Max 4 bytes length for remaining length as defined by protocol.
						* Anything more likely means a broken/malicious client.
						*/
						ಯದಿ(mosq->in_packet.remaining_ಎಣಿಕೆ < -4){
							ಪ್ರತ್ಯಾಗಮ -1;
						}

						mosq->in_packet.remaining_length += (byte & 127) * mosq->in_packet.remaining_mult;
						mosq->in_packet.remaining_mult *= 128;
					}ಯಾವತ್((byte & 128) != 0);
					mosq->in_packet.remaining_ಎಣಿಕೆ = (int8_t)(mosq->in_packet.remaining_ಎಣಿಕೆ * -1);

					ಯದಿ(mosq->in_packet.remaining_length > 0){
						mosq->in_packet.payload = mosquitto_malloc(mosq->in_packet.remaining_length*ಪರಿಮಾಣ(uint8_t));
						ಯದಿ(!mosq->in_packet.payload){
							ಪ್ರತ್ಯಾಗಮ -1;
						}
						mosq->in_packet.to_process = mosq->in_packet.remaining_length;
					}
				}
				ಯದಿ(mosq->in_packet.to_process>0){
					ಯದಿ((uint32_t)len - pos >= mosq->in_packet.to_process){
						memcpy(&mosq->in_packet.payload[mosq->in_packet.pos], &buf[pos], mosq->in_packet.to_process);
						mosq->in_packet.pos += mosq->in_packet.to_process;
						pos += mosq->in_packet.to_process;
						mosq->in_packet.to_process = 0;
					}ಅನ್ಯಥಾ{
						memcpy(&mosq->in_packet.payload[mosq->in_packet.pos], &buf[pos], len-pos);
						mosq->in_packet.pos += (uint32_t)(len-pos);
						mosq->in_packet.to_process -= (uint32_t)(len-pos);
						ಪ್ರತ್ಯಾಗಮ 0;
					}
				}
				/* All data for this packet is read. */
				mosq->in_packet.pos = 0;

#ifdef WITH_SYS_ವೃಕ್ಷ
				metrics__int_inc(mosq_counter_messages_received, 1);
#endif
				rc = handle__packet(mosq);

				/* Free data and reset values */
				packet__cleanup(&mosq->in_packet);

				keepalive__update(mosq);

				ಯದಿ(rc && mosq->out_packet){
					ಯದಿ(mosq->state != mosq_cs_disconnecting){
						mosquitto__set_state(mosq, mosq_cs_disconnect_ws);
					}
					lws_callback_on_writable(mosq->wsi);
				}ಅನ್ಯಥಾ ಯದಿ(rc){
					do_disconnect(mosq, MOSQ_ERR_CONN_LOST);
					ಪ್ರತ್ಯಾಗಮ -1;
				}
			}
			ಭಙ್ಗ;

		ಸ್ವಭಾವ:
			ಭಙ್ಗ;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}


ಸ್ಥಿರ ವರ್ಣ *http__canonical_filename(
		ಸಂರಚನಾ lws *wsi,
		ಧ್ರುವ ವರ್ಣ *in,
		ಧ್ರುವ ವರ್ಣ *http_dir)
{
	size_t inlen, slen;
	ವರ್ಣ *filename, *filename_canonical;

	inlen = strlen(in);
	ಯದಿ(in[inlen-1] == '/'){
		slen = strlen(http_dir) + inlen + strlen("/index.html") + 2;
	}ಅನ್ಯಥಾ{
		slen = strlen(http_dir) + inlen + 2;
	}
	filename = mosquitto_malloc(slen);
	ಯದಿ(!filename){
		lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, ರಿಕ್ತ);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	ಯದಿ(((ವರ್ಣ *)in)[inlen-1] == '/'){
		snprintf(filename, slen, "%s%sindex.html", http_dir, (ವರ್ಣ *)in);
	}ಅನ್ಯಥಾ{
		snprintf(filename, slen, "%s%s", http_dir, (ವರ್ಣ *)in);
	}


	/* Get canonical path and check it is within our http_dir */
#ifdef WIN32
	filename_canonical = _fullpath(ರಿಕ್ತ, filename, 0);
	mosquitto_FREE(filename);
	ಯದಿ(!filename_canonical){
		lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, ರಿಕ್ತ);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
#ಅನ್ಯಥಾ
	filename_canonical = realpath(filename, ರಿಕ್ತ);
	mosquitto_FREE(filename);
	ಯದಿ(!filename_canonical){
		ಯದಿ(errno == EACCES){
			lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, ರಿಕ್ತ);
		}ಅನ್ಯಥಾ ಯದಿ(errno == EINVAL || errno == EIO || errno == ELOOP){
			lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, ರಿಕ್ತ);
		}ಅನ್ಯಥಾ ಯದಿ(errno == ENAMETOOLONG){
			lws_return_http_status(wsi, HTTP_STATUS_REQ_URI_TOO_LONG, ರಿಕ್ತ);
		}ಅನ್ಯಥಾ ಯದಿ(errno == ENOENT || errno == ENOTDIR){
			lws_return_http_status(wsi, HTTP_STATUS_NOT_FOUND, ರಿಕ್ತ);
		}
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
#endif
	ಯದಿ(strncmp(http_dir, filename_canonical, strlen(http_dir))){
		/* Requested file isn't within http_dir, deny access. */
		SAFE_FREE(filename_canonical);
		lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, ರಿಕ್ತ);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	ಪ್ರತ್ಯಾಗಮ filename_canonical;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ callback_http(
		ಸಂರಚನಾ lws *wsi,
		ಗಣನಾ lws_callback_reasons reason,
		ಶೂನ್ಯ *user,
		ಶೂನ್ಯ *in,
		size_t len)
{
	ಸಂರಚನಾ libws_http_data *u = (ಸಂರಚನಾ libws_http_data *)user;
	ಸಂರಚನಾ libws_mqtt_hack *hack;
	ವರ್ಣ *http_dir;
	size_t buflen;
	size_t wlen;
	ಪೂರ್ಣಾಙ್ಕ rc;
	ವರ್ಣ *filename_canonical;
	unsigned ವರ್ಣ buf[4096];
	ಸಂರಚನಾ stat filestat;
	ಸಂರಚನಾ mosquitto *mosq;
	ಸಂರಚನಾ lws_pollargs *pollargs = (ಸಂರಚನಾ lws_pollargs *)in;
	ಪೂರ್ಣಾಙ್ಕ hlen;

	/* FIXME - ssl cert verification is done here. */

	ವಿಕಲ್ಪ(reason){
		ಪಕ್ಷ LWS_CALLBACK_HTTP:
			ಯದಿ(!u){
				ಪ್ರತ್ಯಾಗಮ -1;
			}

			hack = (ಸಂರಚನಾ libws_mqtt_hack *)lws_context_user(lws_get_context(wsi));
			ಯದಿ(!hack){
				ಪ್ರತ್ಯಾಗಮ -1;
			}
			http_dir = hack->http_dir;

			ಯದಿ(!http_dir){
				/* http disabled */
				ಪ್ರತ್ಯಾಗಮ -1;
			}

			/* Forbid POST */
			ಯದಿ(lws_hdr_total_length(wsi, WSI_TOKEN_POST_URI)){
				lws_return_http_status(wsi, HTTP_STATUS_METHOD_NOT_ALLOWED, ರಿಕ್ತ);
				ಪ್ರತ್ಯಾಗಮ -1;
			}

			filename_canonical = http__canonical_filename(wsi, (ವರ್ಣ *)in, http_dir);
			ಯದಿ(!filename_canonical){
				ಪ್ರತ್ಯಾಗಮ -1;
			}

			u->fptr = fopen(filename_canonical, "rb");
			ಯದಿ(!u->fptr){
				SAFE_FREE(filename_canonical);
				lws_return_http_status(wsi, HTTP_STATUS_NOT_FOUND, ರಿಕ್ತ);
				ಪ್ರತ್ಯಾಗಮ -1;
			}
			ಯದಿ(fstat(fileno(u->fptr), &filestat) < 0){
				SAFE_FREE(filename_canonical);
				lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, ರಿಕ್ತ);
				fclose(u->fptr);
				u->fptr = ರಿಕ್ತ;
				ಪ್ರತ್ಯಾಗಮ -1;
			}


			ಯದಿ((filestat.st_mode & S_IFDIR) == S_IFDIR){
				fclose(u->fptr);
				u->fptr = ರಿಕ್ತ;
				SAFE_FREE(filename_canonical);

				/* FIXME - use header functions from lws 2.x */
				buflen = (size_t)snprintf((ವರ್ಣ *)buf, 4096, "HTTP/1.0 302 OK\r\n"
						"Location: %s/\r\n\r\n",
						(ವರ್ಣ *)in);
				ಪ್ರತ್ಯಾಗಮ lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
			}

			ಯದಿ((filestat.st_mode & S_IFREG) != S_IFREG){
				lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, ರಿಕ್ತ);
				fclose(u->fptr);
				u->fptr = ರಿಕ್ತ;
				SAFE_FREE(filename_canonical);
				ಪ್ರತ್ಯಾಗಮ -1;
			}

			log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG, "http serving file \"%s\".", filename_canonical);
			SAFE_FREE(filename_canonical);
			/* FIXME - use header functions from lws 2.x */
			buflen = (size_t)snprintf((ವರ್ಣ *)buf, 4096, "HTTP/1.0 200 OK\r\n"
					"Server: mosquitto\r\n"
					"Content-Length: %u\r\n\r\n",
					(unsigned ಪೂರ್ಣಾಙ್ಕ)filestat.st_size);
			ಯದಿ(lws_write(wsi, buf, buflen, LWS_WRITE_HTTP) < 0){
				fclose(u->fptr);
				u->fptr = ರಿಕ್ತ;
				ಪ್ರತ್ಯಾಗಮ -1;
			}
			lws_callback_on_writable(wsi);
			ಭಙ್ಗ;

		ಪಕ್ಷ LWS_CALLBACK_HTTP_BODY:
			/* For extra POST data? */
			ಪ್ರತ್ಯಾಗಮ -1;

		ಪಕ್ಷ LWS_CALLBACK_HTTP_BODY_COMPLETION:
			/* For end of extra POST data? */
			ಪ್ರತ್ಯಾಗಮ -1;

		ಪಕ್ಷ LWS_CALLBACK_FILTER_HTTP_CONNECTION:
			/* Access control here */
			ಪ್ರತ್ಯಾಗಮ 0;

#ಯದಿ LWS_LIBRARY_VERSION_ಸಂಖ್ಯೆ >= 3001000
		ಪಕ್ಷ LWS_CALLBACK_HTTP_CONFIRM_UPGRADE:
			hack = lws_context_user(lws_get_context(wsi));
			ಯದಿ(hack->listener->ws_origin_ಎಣಿಕೆ){
				hlen = lws_hdr_total_length(wsi, WSI_TOKEN_ORIGIN);
				ಯದಿ(hlen <= 0 || hlen >= (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(buf)){
					ಪ್ರತ್ಯಾಗಮ -1;
				}
				ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<hack->listener->ws_origin_ಎಣಿಕೆ; i++){
					lws_hdr_copy(wsi, (ವರ್ಣ *)buf, ಪರಿಮಾಣ(buf), WSI_TOKEN_ORIGIN);
					ಯದಿ((!strcmp(hack->listener->ws_origins[i], (ವರ್ಣ *)buf))){
						ಪ್ರತ್ಯಾಗಮ 0;
					}
				}
				ಪ್ರತ್ಯಾಗಮ -1;
			}ಅನ್ಯಥಾ{
				ಪ್ರತ್ಯಾಗಮ 0;
			}
#endif

		ಪಕ್ಷ LWS_CALLBACK_HTTP_WRITEABLE:
			/* Send our data here */
			ಯದಿ(u && u->fptr){
				ಕುರು{
					buflen = fread(buf, 1, ಪರಿಮಾಣ(buf), u->fptr);
					ಯದಿ(buflen < 1){
						fclose(u->fptr);
						u->fptr = ರಿಕ್ತ;
						ಪ್ರತ್ಯಾಗಮ -1;
					}
					rc = lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
					ಯದಿ(rc < 0){
						ಪ್ರತ್ಯಾಗಮ -1;
					}
					wlen = (size_t)rc;
					/* while still active, extend timeout */
					ಯದಿ(wlen){
						lws_set_timeout(wsi, PENDING_TIMEOUT_HTTP_CONTENT, 10);
					}
					ಯದಿ(wlen < buflen){
						ಯದಿ(fseek(u->fptr, (long)(buflen-wlen), SEEK_CUR) < 0){
							fclose(u->fptr);
							u->fptr = ರಿಕ್ತ;
							ಪ್ರತ್ಯಾಗಮ -1;
						}
					}ಅನ್ಯಥಾ{
						ಯದಿ(buflen < ಪರಿಮಾಣ(buf)){
							fclose(u->fptr);
							u->fptr = ರಿಕ್ತ;
						}
					}
				}ಯಾವತ್(u->fptr && !lws_send_pipe_choked(wsi));
				lws_callback_on_writable(wsi);
			}ಅನ್ಯಥಾ{
				ಪ್ರತ್ಯಾಗಮ -1;
			}
			ಭಙ್ಗ;

		ಪಕ್ಷ LWS_CALLBACK_CLOSED:
		ಪಕ್ಷ LWS_CALLBACK_CLOSED_HTTP:
		ಪಕ್ಷ LWS_CALLBACK_HTTP_FILE_COMPLETION:
			ಯದಿ(u && u->fptr){
				fclose(u->fptr);
				u->fptr = ರಿಕ್ತ;
			}
			ಭಙ್ಗ;

		ಪಕ್ಷ LWS_CALLBACK_ADD_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, ಪರಿಮಾಣ(pollargs->fd), mosq);
			ಯದಿ(mosq){
				ಯದಿ(pollargs->events & LWS_POLLOUT){
					mux__add_out(mosq);
				}ಅನ್ಯಥಾ{
					mux__remove_out(mosq);
				}
			}ಅನ್ಯಥಾ{
				ಯದಿ(pollargs->events & POLLIN){
					/* Assume this is a new listener */
					listeners__add_websockets(lws_get_context(wsi), pollargs->fd);
				}
			}
			ಭಙ್ಗ;

		ಪಕ್ಷ LWS_CALLBACK_DEL_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, ಪರಿಮಾಣ(pollargs->fd), mosq);
			ಯದಿ(mosq){
				mux__delete(mosq);
			}
			ಭಙ್ಗ;

		ಪಕ್ಷ LWS_CALLBACK_CHANGE_MODE_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, ಪರಿಮಾಣ(pollargs->fd), mosq);
			ಯದಿ(mosq){
				ಯದಿ(pollargs->events & LWS_POLLHUP){
					ಪ್ರತ್ಯಾಗಮ 1;
				}ಅನ್ಯಥಾ ಯದಿ(pollargs->events & LWS_POLLOUT){
					mux__add_out(mosq);
				}ಅನ್ಯಥಾ{
					mux__remove_out(mosq);
				}
			}
			ಭಙ್ಗ;

#ifdef WITH_TLS
		ಪಕ್ಷ LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION:
			ಯದಿ(!len || (SSL_get_verify_result((SSL *)in) != X509_V_OK)){
				ಪ್ರತ್ಯಾಗಮ 1;
			}
			ಭಙ್ಗ;
#endif

		ಸ್ವಭಾವ:
			ಪ್ರತ್ಯಾಗಮ 0;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}


ಸ್ಥಿರ ಶೂನ್ಯ log_wrap(ಪೂರ್ಣಾಙ್ಕ level, ಧ್ರುವ ವರ್ಣ *ಸಾಲು)
{
	ವರ್ಣ *l = (ವರ್ಣ *)ಸಾಲು;
	UNUSED(level);
	l[strlen(ಸಾಲು)-1] = '\0'; /* Remove \n */
	log__printf(ರಿಕ್ತ, MOSQ_LOG_WEBSOCKETS, "%s", l);
}


ಶೂನ್ಯ mosq_websockets_init(ಸಂರಚನಾ mosquitto__listener *listener, ಧ್ರುವ ಸಂರಚನಾ mosquitto__config *conf)
{
	ಸಂರಚನಾ lws_context_creation_info info;
	ಸಂರಚನಾ lws_protocols *p;
	size_t protocol_ಎಣಿಕೆ;
	ಪೂರ್ಣಾಙ್ಕ i;
	ಸಂರಚನಾ libws_mqtt_hack *user;

	/* Count valid protocols */
	ಪ್ರತಿ(protocol_ಎಣಿಕೆ=0; protocols[protocol_ಎಣಿಕೆ].name; protocol_ಎಣಿಕೆ++){
		;
	}

	p = mosquitto_calloc(protocol_ಎಣಿಕೆ+1, ಪರಿಮಾಣ(ಸಂರಚನಾ lws_protocols));
	ಯದಿ(!p){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Out of memory.");
		ಪ್ರತ್ಯಾಗಮ;
	}
	ಪ್ರತಿ(i=0; protocols[i].name; i++){
		p[i].name = protocols[i].name;
		p[i].callback = protocols[i].callback;
		p[i].per_session_data_size = protocols[i].per_session_data_size;
		p[i].rx_buffer_size = protocols[i].rx_buffer_size;
		p[i].user = listener;
	}

	memset(&info, 0, ಪರಿಮಾಣ(info));
	info.iface = listener->host;
	info.port = listener->port;
	info.protocols = p;
	info.gid = -1;
	info.uid = -1;
#ifdef WITH_TLS
	ಯದಿ(listener->cafile){
		info.ssl_ca_filepath = listener->cafile;
	}ಅನ್ಯಥಾ ಯದಿ(listener->capath){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: CA path option is not supported for websockets");
	}
	info.ssl_cert_filepath = listener->certfile;
	info.ssl_private_ಕೀಲಿ_filepath = listener->keyfile;
	info.ssl_cipher_list = listener->ciphers;
	/* HTTP 1 only, due to HTTP 2 issues in Firefox:
	   https://github.com/eclipse-mosquitto/mosquitto/issues/1211
	*/
	info.alpn = "h1";
#ಯದಿ defined(WITH_WEBSOCKETS) && LWS_LIBRARY_VERSION_ಸಂಖ್ಯೆ>=3001000
	info.tls1_3_plus_cipher_list = listener->ciphers_tls13;
#endif
	ಯದಿ(listener->require_certificate){
		info.options |= LWS_SERVER_OPTION_REQUIRE_VALID_OPENSSL_CLIENT_CERT;
	}
#endif

	info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
	ಯದಿ(listener->socket_domain == AF_INET){
		info.options |= LWS_SERVER_OPTION_DISABLE_IPV6;
	}
	info.max_http_header_data = conf->packet_buffer_size;

	user = mosquitto_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ libws_mqtt_hack));
	ಯದಿ(!user){
		mosquitto_FREE(p);
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Out of memory.");
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ(listener->http_dir){
#ifdef WIN32
		user->http_dir = _fullpath(ರಿಕ್ತ, listener->http_dir, 0);
#ಅನ್ಯಥಾ
		user->http_dir = realpath(listener->http_dir, ರಿಕ್ತ);
#endif
		ಯದಿ(!user->http_dir){
			mosquitto_FREE(user);
			mosquitto_FREE(p);
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to open http dir \"%s\".", listener->http_dir);
			ಪ್ರತ್ಯಾಗಮ;
		}
	}
	user->listener = listener;

	info.user = user;
	info.pt_serv_buf_size = WS_SERV_BUF_SIZE;
	listener->ws_protocol = p;

	lws_set_log_level(conf->websockets_log_level, log_wrap);

	log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Opening websockets listen socket on port %d.", listener->port);
	listener->ws_in_init = true;
	listener->ws_context = lws_create_context(&info);
	listener->ws_in_init = false;
}

#endif
#endif
