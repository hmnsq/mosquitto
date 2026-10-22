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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif
#ଅନ୍ତର୍ଭାଵ "mosquitto_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"
#ଅନ୍ତର୍ଭାଵ "packet_mosq.h"
#ଅନ୍ତର୍ଭାଵ "sys_tree.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"

#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <errno.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>

#ifndef WIN32
#  include <sys/socket.h>
#endif

#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
/* Be careful if changing these, if TX is not bigger than SERV then there can
 * be very large write performance penalties.
 */
#define WS_SERV_BUF_SIZE 4096
#define WS_TX_BUF_SIZE (WS_SERV_BUF_SIZE*2)

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା callback_mqtt(
		ସଂରଚନା lws *wsi,
		ଗଣନା lws_callback_reasons reason,
		ଶୂନ୍ୟ *user,
		ଶୂନ୍ୟ *in,
		size_t len);

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା callback_http(
		ସଂରଚନା lws *wsi,
		ଗଣନା lws_callback_reasons reason,
		ଶୂନ୍ୟ *user,
		ଶୂନ୍ୟ *in,
		size_t len);

ଗଣନା mosq_ws_protocols {
	PROTOCOL_HTTP = 0,
	PROTOCOL_MQTT,
	DEMO_PROTOCOL_ଗଣନା,
};

ସଂରଚନା libws_http_data {
	FILE *fptr;
};

ସ୍ଥିର ସଂରଚନା lws_protocols protocols[] = {
	/* first protocol must always be HTTP handler */
	{
		"http-only",                        /* name */
		callback_http,                      /* lws_callback_function */
		ଆକାର (ସଂରଚନା libws_http_data),    /* per_session_data_size */
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		ଖାଲି,                               /* user v1.4 on */
		WS_TX_BUF_SIZE                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqtt",
		callback_mqtt,
		ଆକାର(ସଂରଚନା libws_mqtt_data),
		0,                                  /* rx_buffer_size */
		1,                                  /* id */
		ଖାଲି,                               /* user v1.4 on */
		WS_TX_BUF_SIZE                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqttv3.1",
		callback_mqtt,
		ଆକାର(ସଂରଚନା libws_mqtt_data),
		0,                                  /* rx_buffer_size */
		2,                                  /* id */
		ଖାଲି,                               /* user v1.4 on */
		WS_TX_BUF_SIZE                      /* tx_packet_size v2.3.0 */
	},
	{
		ଖାଲି,
		ଖାଲି,
		0,
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		ଖାଲି,                               /* user v1.4 on */
		0                                   /* tx_packet_size v2.3.0 */
	}
};


ସ୍ଥିର ଶୂନ୍ୟ easy_address(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sock, ସଂରଚନା mosquitto *mosq)
{
	ଅକ୍ଷର address[1024];

	ଯଦି(!net__socket_get_address(sock, address, 1024, &mosq->remote_port)){
		mosq->address = mosquitto_strdup(address);
	}
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା callback_mqtt(
		ସଂରଚନା lws *wsi,
		ଗଣନା lws_callback_reasons reason,
		ଶୂନ୍ୟ *user,
		ଶୂନ୍ୟ *in,
		size_t len)
{
	ସଂରଚନା mosquitto *mosq = ଖାଲି;
	ସଂରଚନା mosquitto__packet *packet;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ucount;
	ଅଚଳ ସଂରଚନା lws_protocols *p;
	ସଂରଚନା libws_mqtt_data *u = (ସଂରଚନା libws_mqtt_data *)user;
	size_t pos;
	uint8_t *buf;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	uint8_t byte;
	ଅକ୍ଷର ip_addr_buff[1024];

	ବାଛ(reason){
		ସ୍ଥିତି LWS_CALLBACK_ESTABLISHED:
			mosq = context__init();
			ଯଦି(mosq){
				p = lws_get_protocol(wsi);
				mosq->listener = p->user;
				ଯଦି(!mosq->listener){
					mosquitto_FREE(mosq);
					ଫେରନ୍ତୁ -1;
				}
				mosq->wsi = wsi;
#ifdef WITH_TLS
				ଯଦି(in){
					mosq->ssl = (SSL *)in;
					ଯଦି(!mosq->listener->ssl_ctx){
						mosq->listener->ssl_ctx = SSL_get_SSL_CTX(mosq->ssl);
					}
				}
#endif
				u->mosq = mosq;
			}ଅନ୍ୟଥା{
				ଫେରନ୍ତୁ -1;
			}

			easy_address(lws_get_socket_fd(wsi), mosq);
			ଯଦି(!mosq->address){
				/* getpeername and inet_ntop failed and not a bridge */
				mosquitto_FREE(mosq);
				u->mosq = ଖାଲି;
				ଫେରନ୍ତୁ -1;
			}
			mosq->listener->client_ଗଣନା++;
			ଯଦି((mosq->listener->max_connections > 0 && mosq->listener->client_ଗଣନା > mosq->listener->max_connections)
					|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)db.config->global_max_connections)){

				ଯଦି(db.config->connection_messages == true){
					log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", mosq->address);
				}
				mosq->listener->client_ଗଣନା--;
				mosquitto_FREE(mosq->address);
				mosquitto_FREE(mosq);
				u->mosq = ଖାଲି;
				ଫେରନ୍ତୁ -1;
			}
			mosq->sock = lws_get_socket_fd(wsi);
			HASH_ADD(hh_sock, db.contexts_by_sock, sock, ଆକାର(mosq->sock), mosq);
			mux__new(mosq);
			ଭାଙ୍ଗ;

		ସ୍ଥିତି LWS_CALLBACK_CLOSED:
			ଯଦି(!u){
				ଫେରନ୍ତୁ -1;
			}
			mosq = u->mosq;
			ଯଦି(mosq){
				ଯଦି(mosq->sock != INVALID_SOCKET){
					HASH_DELETE(hh_sock, db.contexts_by_sock, mosq);
					mosq->sock = INVALID_SOCKET;
					mux__delete(mosq);
				}
				mosq->wsi = ଖାଲି;
#ifdef WITH_TLS
				mosq->ssl = ଖାଲି;
#endif
				do_disconnect(mosq, MOSQ_ERR_CONN_LOST);
			}
			ଭାଙ୍ଗ;

		ସ୍ଥିତି LWS_CALLBACK_SERVER_WRITEABLE:
			ଯଦି(!u){
				ଫେରନ୍ତୁ -1;
			}
			mosq = u->mosq;
			ଯଦି(!mosq){
				ଫେରନ୍ତୁ -1;
			}

			rc = db__message_write_inflight_out_latest(mosq);
			ଯଦି(rc){
				ଫେରନ୍ତୁ -1;
			}
			rc = db__message_write_queued_out(mosq);
			ଯଦି(rc){
				ଫେରନ୍ତୁ -1;
			}

			ଯେତେବେଳେ(mosq->out_packet && !lws_send_pipe_choked(mosq->wsi)){
				packet = mosq->out_packet;
				ଗଣନା_2 = lws_write(wsi, &packet->payload[packet->pos], packet->to_process, LWS_WRITE_ଦ୍ୱିମିକ);
				ଯଦି(ଗଣନା_2 < 0){
					ଯଦି(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						ଫେରନ୍ତୁ -1;
					}
					ଫେରନ୍ତୁ 0;
				}
				ucount = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଗଣନା_2;
#ifdef WITH_SYS_ବୃକ୍ଷ
				metrics__int_inc(mosq_counter_bytes_sent, ucount);
#endif
				packet->to_process -= ucount;
				packet->pos += ucount;
				ଯଦି(packet->to_process > 0){
					ଯଦି(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						ଫେରନ୍ତୁ -1;
					}
					ଭାଙ୍ଗ;
				}

#ifdef WITH_SYS_ବୃକ୍ଷ
				metrics__int_inc(mosq_counter_messages_sent, 1);
				ଯଦି(((packet->command)&0xF0) == CMD_PUBLISH){
					metrics__int_inc(mosq_counter_mqtt_publish_sent, 1);
				}
#endif

				packet__get_next_out(mosq);
				mosquitto_FREE(packet);

				mosq->next_ସନ୍ଦେଶ_out = db.now_s + mosq->keepalive;
			}
			ଯଦି(mosq->state == mosq_cs_disconnect_ws
					|| mosq->state == mosq_cs_disconnecting
					|| mosq->state == mosq_cs_disused){

				ଫେରନ୍ତୁ -1;
			}
			ଯଦି(mosq->out_packet){
				lws_callback_on_writable(mosq->wsi);
			}
			ଭାଙ୍ଗ;

		ସ୍ଥିତି LWS_CALLBACK_RECEIVE:
			ଯଦି(!u || !u->mosq){
				ଫେରନ୍ତୁ -1;
			}
			mosq = u->mosq;
			pos = 0;
			buf = (uint8_t *)in;
			metrics__int_inc(mosq_counter_bytes_received, (int64_t)len);
			ଯେତେବେଳେ(pos < len){
				ଯଦି(!mosq->in_packet.command){
					mosq->in_packet.command = buf[pos];
					pos++;
					/* Clients must send CONNECT as their first command. */
					ଯଦି(mosq->state == mosq_cs_new && (mosq->in_packet.command&0xF0) != CMD_CONNECT){
						ଫେରନ୍ତୁ -1;
					}
				}
				ଯଦି(mosq->in_packet.remaining_ଗଣନା <= 0){
					କର{
						ଯଦି(pos == len){
							ଫେରନ୍ତୁ 0;
						}
						byte = buf[pos];
						pos++;

						mosq->in_packet.remaining_ଗଣନା--;
						/* Max 4 bytes length for remaining length as defined by protocol.
						* Anything more likely means a broken/malicious client.
						*/
						ଯଦି(mosq->in_packet.remaining_ଗଣନା < -4){
							ଫେରନ୍ତୁ -1;
						}

						mosq->in_packet.remaining_length += (byte & 127) * mosq->in_packet.remaining_mult;
						mosq->in_packet.remaining_mult *= 128;
					}ଯେତେବେଳେ((byte & 128) != 0);
					mosq->in_packet.remaining_ଗଣନା = (int8_t)(mosq->in_packet.remaining_ଗଣନା * -1);

					ଯଦି(mosq->in_packet.remaining_length > 0){
						mosq->in_packet.payload = mosquitto_malloc(mosq->in_packet.remaining_length*ଆକାର(uint8_t));
						ଯଦି(!mosq->in_packet.payload){
							ଫେରନ୍ତୁ -1;
						}
						mosq->in_packet.to_process = mosq->in_packet.remaining_length;
					}
				}
				ଯଦି(mosq->in_packet.to_process>0){
					ଯଦି((uint32_t)len - pos >= mosq->in_packet.to_process){
						memcpy(&mosq->in_packet.payload[mosq->in_packet.pos], &buf[pos], mosq->in_packet.to_process);
						mosq->in_packet.pos += mosq->in_packet.to_process;
						pos += mosq->in_packet.to_process;
						mosq->in_packet.to_process = 0;
					}ଅନ୍ୟଥା{
						memcpy(&mosq->in_packet.payload[mosq->in_packet.pos], &buf[pos], len-pos);
						mosq->in_packet.pos += (uint32_t)(len-pos);
						mosq->in_packet.to_process -= (uint32_t)(len-pos);
						ଫେରନ୍ତୁ 0;
					}
				}
				/* All data for this packet is read. */
				mosq->in_packet.pos = 0;

#ifdef WITH_SYS_ବୃକ୍ଷ
				metrics__int_inc(mosq_counter_messages_received, 1);
#endif
				rc = handle__packet(mosq);

				/* Free data and reset values */
				packet__cleanup(&mosq->in_packet);

				keepalive__update(mosq);

				ଯଦି(rc && mosq->out_packet){
					ଯଦି(mosq->state != mosq_cs_disconnecting){
						mosquitto__set_state(mosq, mosq_cs_disconnect_ws);
					}
					lws_callback_on_writable(mosq->wsi);
				}ଅନ୍ୟଥା ଯଦି(rc){
					do_disconnect(mosq, MOSQ_ERR_CONN_LOST);
					ଫେରନ୍ତୁ -1;
				}
			}
			ଭାଙ୍ଗ;

		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ଭାଙ୍ଗ;
	}

	ଫେରନ୍ତୁ 0;
}


ସ୍ଥିର ଅକ୍ଷର *http__canonical_filename(
		ସଂରଚନା lws *wsi,
		ଅଚଳ ଅକ୍ଷର *in,
		ଅଚଳ ଅକ୍ଷର *http_dir)
{
	size_t inlen, slen;
	ଅକ୍ଷର *filename, *filename_canonical;

	inlen = strlen(in);
	ଯଦି(in[inlen-1] == '/'){
		slen = strlen(http_dir) + inlen + strlen("/index.html") + 2;
	}ଅନ୍ୟଥା{
		slen = strlen(http_dir) + inlen + 2;
	}
	filename = mosquitto_malloc(slen);
	ଯଦି(!filename){
		lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, ଖାଲି);
		ଫେରନ୍ତୁ ଖାଲି;
	}
	ଯଦି(((ଅକ୍ଷର *)in)[inlen-1] == '/'){
		snprintf(filename, slen, "%s%sindex.html", http_dir, (ଅକ୍ଷର *)in);
	}ଅନ୍ୟଥା{
		snprintf(filename, slen, "%s%s", http_dir, (ଅକ୍ଷର *)in);
	}


	/* Get canonical path and check it is within our http_dir */
#ifdef WIN32
	filename_canonical = _fullpath(ଖାଲି, filename, 0);
	mosquitto_FREE(filename);
	ଯଦି(!filename_canonical){
		lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, ଖାଲି);
		ଫେରନ୍ତୁ ଖାଲି;
	}
#ଅନ୍ୟଥା
	filename_canonical = realpath(filename, ଖାଲି);
	mosquitto_FREE(filename);
	ଯଦି(!filename_canonical){
		ଯଦି(errno == EACCES){
			lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, ଖାଲି);
		}ଅନ୍ୟଥା ଯଦି(errno == EINVAL || errno == EIO || errno == ELOOP){
			lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, ଖାଲି);
		}ଅନ୍ୟଥା ଯଦି(errno == ENAMETOOLONG){
			lws_return_http_status(wsi, HTTP_STATUS_REQ_URI_TOO_LONG, ଖାଲି);
		}ଅନ୍ୟଥା ଯଦି(errno == ENOENT || errno == ENOTDIR){
			lws_return_http_status(wsi, HTTP_STATUS_NOT_FOUND, ଖାଲି);
		}
		ଫେରନ୍ତୁ ଖାଲି;
	}
#endif
	ଯଦି(strncmp(http_dir, filename_canonical, strlen(http_dir))){
		/* Requested file isn't within http_dir, deny access. */
		SAFE_FREE(filename_canonical);
		lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, ଖାଲି);
		ଫେରନ୍ତୁ ଖାଲି;
	}

	ଫେରନ୍ତୁ filename_canonical;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା callback_http(
		ସଂରଚନା lws *wsi,
		ଗଣନା lws_callback_reasons reason,
		ଶୂନ୍ୟ *user,
		ଶୂନ୍ୟ *in,
		size_t len)
{
	ସଂରଚନା libws_http_data *u = (ସଂରଚନା libws_http_data *)user;
	ସଂରଚନା libws_mqtt_hack *hack;
	ଅକ୍ଷର *http_dir;
	size_t buflen;
	size_t wlen;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ଅକ୍ଷର *filename_canonical;
	unsigned ଅକ୍ଷର buf[4096];
	ସଂରଚନା stat filestat;
	ସଂରଚନା mosquitto *mosq;
	ସଂରଚନା lws_pollargs *pollargs = (ସଂରଚନା lws_pollargs *)in;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା hlen;

	/* FIXME - ssl cert verification is done here. */

	ବାଛ(reason){
		ସ୍ଥିତି LWS_CALLBACK_HTTP:
			ଯଦି(!u){
				ଫେରନ୍ତୁ -1;
			}

			hack = (ସଂରଚନା libws_mqtt_hack *)lws_context_user(lws_get_context(wsi));
			ଯଦି(!hack){
				ଫେରନ୍ତୁ -1;
			}
			http_dir = hack->http_dir;

			ଯଦି(!http_dir){
				/* http disabled */
				ଫେରନ୍ତୁ -1;
			}

			/* Forbid POST */
			ଯଦି(lws_hdr_total_length(wsi, WSI_TOKEN_POST_URI)){
				lws_return_http_status(wsi, HTTP_STATUS_METHOD_NOT_ALLOWED, ଖାଲି);
				ଫେରନ୍ତୁ -1;
			}

			filename_canonical = http__canonical_filename(wsi, (ଅକ୍ଷର *)in, http_dir);
			ଯଦି(!filename_canonical){
				ଫେରନ୍ତୁ -1;
			}

			u->fptr = fopen(filename_canonical, "rb");
			ଯଦି(!u->fptr){
				SAFE_FREE(filename_canonical);
				lws_return_http_status(wsi, HTTP_STATUS_NOT_FOUND, ଖାଲି);
				ଫେରନ୍ତୁ -1;
			}
			ଯଦି(fstat(fileno(u->fptr), &filestat) < 0){
				SAFE_FREE(filename_canonical);
				lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, ଖାଲି);
				fclose(u->fptr);
				u->fptr = ଖାଲି;
				ଫେରନ୍ତୁ -1;
			}


			ଯଦି((filestat.st_mode & S_IFDIR) == S_IFDIR){
				fclose(u->fptr);
				u->fptr = ଖାଲି;
				SAFE_FREE(filename_canonical);

				/* FIXME - use header functions from lws 2.x */
				buflen = (size_t)snprintf((ଅକ୍ଷର *)buf, 4096, "HTTP/1.0 302 OK\r\n"
						"Location: %s/\r\n\r\n",
						(ଅକ୍ଷର *)in);
				ଫେରନ୍ତୁ lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
			}

			ଯଦି((filestat.st_mode & S_IFREG) != S_IFREG){
				lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, ଖାଲି);
				fclose(u->fptr);
				u->fptr = ଖାଲି;
				SAFE_FREE(filename_canonical);
				ଫେରନ୍ତୁ -1;
			}

			log__printf(ଖାଲି, MOSQ_LOG_DEBUG, "http serving file \"%s\".", filename_canonical);
			SAFE_FREE(filename_canonical);
			/* FIXME - use header functions from lws 2.x */
			buflen = (size_t)snprintf((ଅକ୍ଷର *)buf, 4096, "HTTP/1.0 200 OK\r\n"
					"Server: mosquitto\r\n"
					"Content-Length: %u\r\n\r\n",
					(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)filestat.st_size);
			ଯଦି(lws_write(wsi, buf, buflen, LWS_WRITE_HTTP) < 0){
				fclose(u->fptr);
				u->fptr = ଖାଲି;
				ଫେରନ୍ତୁ -1;
			}
			lws_callback_on_writable(wsi);
			ଭାଙ୍ଗ;

		ସ୍ଥିତି LWS_CALLBACK_HTTP_BODY:
			/* For extra POST data? */
			ଫେରନ୍ତୁ -1;

		ସ୍ଥିତି LWS_CALLBACK_HTTP_BODY_COMPLETION:
			/* For end of extra POST data? */
			ଫେରନ୍ତୁ -1;

		ସ୍ଥିତି LWS_CALLBACK_FILTER_HTTP_CONNECTION:
			/* Access control here */
			ଫେରନ୍ତୁ 0;

#ଯଦି LWS_LIBRARY_VERSION_ସଂଖ୍ୟା >= 3001000
		ସ୍ଥିତି LWS_CALLBACK_HTTP_CONFIRM_UPGRADE:
			hack = lws_context_user(lws_get_context(wsi));
			ଯଦି(hack->listener->ws_origin_ଗଣନା){
				hlen = lws_hdr_total_length(wsi, WSI_TOKEN_ORIGIN);
				ଯଦି(hlen <= 0 || hlen >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଆକାର(buf)){
					ଫେରନ୍ତୁ -1;
				}
				ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<hack->listener->ws_origin_ଗଣନା; i++){
					lws_hdr_copy(wsi, (ଅକ୍ଷର *)buf, ଆକାର(buf), WSI_TOKEN_ORIGIN);
					ଯଦି((!strcmp(hack->listener->ws_origins[i], (ଅକ୍ଷର *)buf))){
						ଫେରନ୍ତୁ 0;
					}
				}
				ଫେରନ୍ତୁ -1;
			}ଅନ୍ୟଥା{
				ଫେରନ୍ତୁ 0;
			}
#endif

		ସ୍ଥିତି LWS_CALLBACK_HTTP_WRITEABLE:
			/* Send our data here */
			ଯଦି(u && u->fptr){
				କର{
					buflen = fread(buf, 1, ଆକାର(buf), u->fptr);
					ଯଦି(buflen < 1){
						fclose(u->fptr);
						u->fptr = ଖାଲି;
						ଫେରନ୍ତୁ -1;
					}
					rc = lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
					ଯଦି(rc < 0){
						ଫେରନ୍ତୁ -1;
					}
					wlen = (size_t)rc;
					/* while still active, extend timeout */
					ଯଦି(wlen){
						lws_set_timeout(wsi, PENDING_TIMEOUT_HTTP_CONTENT, 10);
					}
					ଯଦି(wlen < buflen){
						ଯଦି(fseek(u->fptr, (long)(buflen-wlen), SEEK_CUR) < 0){
							fclose(u->fptr);
							u->fptr = ଖାଲି;
							ଫେରନ୍ତୁ -1;
						}
					}ଅନ୍ୟଥା{
						ଯଦି(buflen < ଆକାର(buf)){
							fclose(u->fptr);
							u->fptr = ଖାଲି;
						}
					}
				}ଯେତେବେଳେ(u->fptr && !lws_send_pipe_choked(wsi));
				lws_callback_on_writable(wsi);
			}ଅନ୍ୟଥା{
				ଫେରନ୍ତୁ -1;
			}
			ଭାଙ୍ଗ;

		ସ୍ଥିତି LWS_CALLBACK_CLOSED:
		ସ୍ଥିତି LWS_CALLBACK_CLOSED_HTTP:
		ସ୍ଥିତି LWS_CALLBACK_HTTP_FILE_COMPLETION:
			ଯଦି(u && u->fptr){
				fclose(u->fptr);
				u->fptr = ଖାଲି;
			}
			ଭାଙ୍ଗ;

		ସ୍ଥିତି LWS_CALLBACK_ADD_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, ଆକାର(pollargs->fd), mosq);
			ଯଦି(mosq){
				ଯଦି(pollargs->events & LWS_POLLOUT){
					mux__add_out(mosq);
				}ଅନ୍ୟଥା{
					mux__remove_out(mosq);
				}
			}ଅନ୍ୟଥା{
				ଯଦି(pollargs->events & POLLIN){
					/* Assume this is a new listener */
					listeners__add_websockets(lws_get_context(wsi), pollargs->fd);
				}
			}
			ଭାଙ୍ଗ;

		ସ୍ଥିତି LWS_CALLBACK_DEL_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, ଆକାର(pollargs->fd), mosq);
			ଯଦି(mosq){
				mux__delete(mosq);
			}
			ଭାଙ୍ଗ;

		ସ୍ଥିତି LWS_CALLBACK_CHANGE_MODE_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, ଆକାର(pollargs->fd), mosq);
			ଯଦି(mosq){
				ଯଦି(pollargs->events & LWS_POLLHUP){
					ଫେରନ୍ତୁ 1;
				}ଅନ୍ୟଥା ଯଦି(pollargs->events & LWS_POLLOUT){
					mux__add_out(mosq);
				}ଅନ୍ୟଥା{
					mux__remove_out(mosq);
				}
			}
			ଭାଙ୍ଗ;

#ifdef WITH_TLS
		ସ୍ଥିତି LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION:
			ଯଦି(!len || (SSL_get_verify_result((SSL *)in) != X509_V_OK)){
				ଫେରନ୍ତୁ 1;
			}
			ଭାଙ୍ଗ;
#endif

		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ଫେରନ୍ତୁ 0;
	}

	ଫେରନ୍ତୁ 0;
}


ସ୍ଥିର ଶୂନ୍ୟ log_wrap(ପୂର୍ଣ୍ଣସଂଖ୍ୟା level, ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି)
{
	ଅକ୍ଷର *l = (ଅକ୍ଷର *)ଧାଡ଼ି;
	UNUSED(level);
	l[strlen(ଧାଡ଼ି)-1] = '\0'; /* Remove \n */
	log__printf(ଖାଲି, MOSQ_LOG_WEBSOCKETS, "%s", l);
}


ଶୂନ୍ୟ mosq_websockets_init(ସଂରଚନା mosquitto__listener *listener, ଅଚଳ ସଂରଚନା mosquitto__config *conf)
{
	ସଂରଚନା lws_context_creation_info info;
	ସଂରଚନା lws_protocols *p;
	size_t protocol_ଗଣନା;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ସଂରଚନା libws_mqtt_hack *user;

	/* Count valid protocols */
	ପାଇଁ(protocol_ଗଣନା=0; protocols[protocol_ଗଣନା].name; protocol_ଗଣନା++){
		;
	}

	p = mosquitto_calloc(protocol_ଗଣନା+1, ଆକାର(ସଂରଚନା lws_protocols));
	ଯଦି(!p){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Out of memory.");
		ଫେରନ୍ତୁ;
	}
	ପାଇଁ(i=0; protocols[i].name; i++){
		p[i].name = protocols[i].name;
		p[i].callback = protocols[i].callback;
		p[i].per_session_data_size = protocols[i].per_session_data_size;
		p[i].rx_buffer_size = protocols[i].rx_buffer_size;
		p[i].user = listener;
	}

	memset(&info, 0, ଆକାର(info));
	info.iface = listener->host;
	info.port = listener->port;
	info.protocols = p;
	info.gid = -1;
	info.uid = -1;
#ifdef WITH_TLS
	ଯଦି(listener->cafile){
		info.ssl_ca_filepath = listener->cafile;
	}ଅନ୍ୟଥା ଯଦି(listener->capath){
		log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: CA path option is not supported for websockets");
	}
	info.ssl_cert_filepath = listener->certfile;
	info.ssl_private_ଚାବି_filepath = listener->keyfile;
	info.ssl_cipher_list = listener->ciphers;
	/* HTTP 1 only, due to HTTP 2 issues in Firefox:
	   https://github.com/eclipse-mosquitto/mosquitto/issues/1211
	*/
	info.alpn = "h1";
#ଯଦି defined(WITH_WEBSOCKETS) && LWS_LIBRARY_VERSION_ସଂଖ୍ୟା>=3001000
	info.tls1_3_plus_cipher_list = listener->ciphers_tls13;
#endif
	ଯଦି(listener->require_certificate){
		info.options |= LWS_SERVER_OPTION_REQUIRE_VALID_OPENSSL_CLIENT_CERT;
	}
#endif

	info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
	ଯଦି(listener->socket_domain == AF_INET){
		info.options |= LWS_SERVER_OPTION_DISABLE_IPV6;
	}
	info.max_http_header_data = conf->packet_buffer_size;

	user = mosquitto_calloc(1, ଆକାର(ସଂରଚନା libws_mqtt_hack));
	ଯଦି(!user){
		mosquitto_FREE(p);
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Out of memory.");
		ଫେରନ୍ତୁ;
	}

	ଯଦି(listener->http_dir){
#ifdef WIN32
		user->http_dir = _fullpath(ଖାଲି, listener->http_dir, 0);
#ଅନ୍ୟଥା
		user->http_dir = realpath(listener->http_dir, ଖାଲି);
#endif
		ଯଦି(!user->http_dir){
			mosquitto_FREE(user);
			mosquitto_FREE(p);
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to open http dir \"%s\".", listener->http_dir);
			ଫେରନ୍ତୁ;
		}
	}
	user->listener = listener;

	info.user = user;
	info.pt_serv_buf_size = WS_SERV_BUF_SIZE;
	listener->ws_protocol = p;

	lws_set_log_level(conf->websockets_log_level, log_wrap);

	log__printf(ଖାଲି, MOSQ_LOG_INFO, "Opening websockets listen socket on port %d.", listener->port);
	listener->ws_in_init = true;
	listener->ws_context = lws_create_context(&info);
	listener->ws_in_init = false;
}

#endif
#endif
