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

#അന്തര്ഭാവ "config.h"

#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif
#അന്തര്ഭാവ "mosquitto_internal.h"
#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/mqtt_protocol.h"
#അന്തര്ഭാവ "packet_mosq.h"
#അന്തര്ഭാവ "sys_tree.h"
#അന്തര്ഭാവ "util_mosq.h"

#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <errno.h>
#അന്തര്ഭാവ <sys/stat.h>

#ifndef WIN32
#  include <sys/socket.h>
#endif

#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
/* Be careful if changing these, if TX is not bigger than SERV then there can
 * be very large write performance penalties.
 */
#define WS_SERV_BUF_SIZE 4096
#define WS_TX_BUF_SIZE (WS_SERV_BUF_SIZE*2)

സ്ഥിതം പൂർണ്ണസംഖ്യ callback_mqtt(
		ഘടന lws *wsi,
		എണ്ണൽ lws_callback_reasons reason,
		ശൂന്യം *user,
		ശൂന്യം *in,
		size_t len);

സ്ഥിതം പൂർണ്ണസംഖ്യ callback_http(
		ഘടന lws *wsi,
		എണ്ണൽ lws_callback_reasons reason,
		ശൂന്യം *user,
		ശൂന്യം *in,
		size_t len);

എണ്ണൽ mosq_ws_protocols {
	PROTOCOL_HTTP = 0,
	PROTOCOL_MQTT,
	DEMO_PROTOCOL_എണ്ണം,
};

ഘടന libws_http_data {
	FILE *fptr;
};

സ്ഥിതം ഘടന lws_protocols protocols[] = {
	/* first protocol must always be HTTP handler */
	{
		"http-only",                        /* name */
		callback_http,                      /* lws_callback_function */
		വലുപ്പം (ഘടന libws_http_data),    /* per_session_data_size */
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		ശൂന്യ,                               /* user v1.4 on */
		WS_TX_BUF_SIZE                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqtt",
		callback_mqtt,
		വലുപ്പം(ഘടന libws_mqtt_data),
		0,                                  /* rx_buffer_size */
		1,                                  /* id */
		ശൂന്യ,                               /* user v1.4 on */
		WS_TX_BUF_SIZE                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqttv3.1",
		callback_mqtt,
		വലുപ്പം(ഘടന libws_mqtt_data),
		0,                                  /* rx_buffer_size */
		2,                                  /* id */
		ശൂന്യ,                               /* user v1.4 on */
		WS_TX_BUF_SIZE                      /* tx_packet_size v2.3.0 */
	},
	{
		ശൂന്യ,
		ശൂന്യ,
		0,
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		ശൂന്യ,                               /* user v1.4 on */
		0                                   /* tx_packet_size v2.3.0 */
	}
};


സ്ഥിതം ശൂന്യം easy_address(പൂർണ്ണസംഖ്യ sock, ഘടന mosquitto *mosq)
{
	അക്ഷരം address[1024];

	എങ്കിൽ(!net__socket_get_address(sock, address, 1024, &mosq->remote_port)){
		mosq->address = mosquitto_strdup(address);
	}
}


സ്ഥിതം പൂർണ്ണസംഖ്യ callback_mqtt(
		ഘടന lws *wsi,
		എണ്ണൽ lws_callback_reasons reason,
		ശൂന്യം *user,
		ശൂന്യം *in,
		size_t len)
{
	ഘടന mosquitto *mosq = ശൂന്യ;
	ഘടന mosquitto__packet *packet;
	പൂർണ്ണസംഖ്യ എണ്ണം;
	unsigned പൂർണ്ണസംഖ്യ ucount;
	സ്ഥിരം ഘടന lws_protocols *p;
	ഘടന libws_mqtt_data *u = (ഘടന libws_mqtt_data *)user;
	size_t pos;
	uint8_t *buf;
	പൂർണ്ണസംഖ്യ rc;
	uint8_t byte;
	അക്ഷരം ip_addr_buff[1024];

	തിരഞ്ഞെടുപ്പ്(reason){
		അവസ്ഥ LWS_CALLBACK_ESTABLISHED:
			mosq = context__init();
			എങ്കിൽ(mosq){
				p = lws_get_protocol(wsi);
				mosq->listener = p->user;
				എങ്കിൽ(!mosq->listener){
					mosquitto_FREE(mosq);
					തിരികെ -1;
				}
				mosq->wsi = wsi;
#ifdef WITH_TLS
				എങ്കിൽ(in){
					mosq->ssl = (SSL *)in;
					എങ്കിൽ(!mosq->listener->ssl_ctx){
						mosq->listener->ssl_ctx = SSL_get_SSL_CTX(mosq->ssl);
					}
				}
#endif
				u->mosq = mosq;
			}അല്ലെങ്കിൽ{
				തിരികെ -1;
			}

			easy_address(lws_get_socket_fd(wsi), mosq);
			എങ്കിൽ(!mosq->address){
				/* getpeername and inet_ntop failed and not a bridge */
				mosquitto_FREE(mosq);
				u->mosq = ശൂന്യ;
				തിരികെ -1;
			}
			mosq->listener->client_എണ്ണം++;
			എങ്കിൽ((mosq->listener->max_connections > 0 && mosq->listener->client_എണ്ണം > mosq->listener->max_connections)
					|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (unsigned പൂർണ്ണസംഖ്യ)db.config->global_max_connections)){

				എങ്കിൽ(db.config->connection_messages == true){
					log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", mosq->address);
				}
				mosq->listener->client_എണ്ണം--;
				mosquitto_FREE(mosq->address);
				mosquitto_FREE(mosq);
				u->mosq = ശൂന്യ;
				തിരികെ -1;
			}
			mosq->sock = lws_get_socket_fd(wsi);
			HASH_ADD(hh_sock, db.contexts_by_sock, sock, വലുപ്പം(mosq->sock), mosq);
			mux__new(mosq);
			നിർത്തുക;

		അവസ്ഥ LWS_CALLBACK_CLOSED:
			എങ്കിൽ(!u){
				തിരികെ -1;
			}
			mosq = u->mosq;
			എങ്കിൽ(mosq){
				എങ്കിൽ(mosq->sock != INVALID_SOCKET){
					HASH_DELETE(hh_sock, db.contexts_by_sock, mosq);
					mosq->sock = INVALID_SOCKET;
					mux__delete(mosq);
				}
				mosq->wsi = ശൂന്യ;
#ifdef WITH_TLS
				mosq->ssl = ശൂന്യ;
#endif
				do_disconnect(mosq, MOSQ_ERR_CONN_LOST);
			}
			നിർത്തുക;

		അവസ്ഥ LWS_CALLBACK_SERVER_WRITEABLE:
			എങ്കിൽ(!u){
				തിരികെ -1;
			}
			mosq = u->mosq;
			എങ്കിൽ(!mosq){
				തിരികെ -1;
			}

			rc = db__message_write_inflight_out_latest(mosq);
			എങ്കിൽ(rc){
				തിരികെ -1;
			}
			rc = db__message_write_queued_out(mosq);
			എങ്കിൽ(rc){
				തിരികെ -1;
			}

			ആകുമ്പോൾ(mosq->out_packet && !lws_send_pipe_choked(mosq->wsi)){
				packet = mosq->out_packet;
				എണ്ണം = lws_write(wsi, &packet->payload[packet->pos], packet->to_process, LWS_WRITE_ദ്വിമാന);
				എങ്കിൽ(എണ്ണം < 0){
					എങ്കിൽ(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						തിരികെ -1;
					}
					തിരികെ 0;
				}
				ucount = (unsigned പൂർണ്ണസംഖ്യ)എണ്ണം;
#ifdef WITH_SYS_വൃക്ഷം
				metrics__int_inc(mosq_counter_bytes_sent, ucount);
#endif
				packet->to_process -= ucount;
				packet->pos += ucount;
				എങ്കിൽ(packet->to_process > 0){
					എങ്കിൽ(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						തിരികെ -1;
					}
					നിർത്തുക;
				}

#ifdef WITH_SYS_വൃക്ഷം
				metrics__int_inc(mosq_counter_messages_sent, 1);
				എങ്കിൽ(((packet->command)&0xF0) == CMD_PUBLISH){
					metrics__int_inc(mosq_counter_mqtt_publish_sent, 1);
				}
#endif

				packet__get_next_out(mosq);
				mosquitto_FREE(packet);

				mosq->next_സന്ദേശം_out = db.now_s + mosq->keepalive;
			}
			എങ്കിൽ(mosq->state == mosq_cs_disconnect_ws
					|| mosq->state == mosq_cs_disconnecting
					|| mosq->state == mosq_cs_disused){

				തിരികെ -1;
			}
			എങ്കിൽ(mosq->out_packet){
				lws_callback_on_writable(mosq->wsi);
			}
			നിർത്തുക;

		അവസ്ഥ LWS_CALLBACK_RECEIVE:
			എങ്കിൽ(!u || !u->mosq){
				തിരികെ -1;
			}
			mosq = u->mosq;
			pos = 0;
			buf = (uint8_t *)in;
			metrics__int_inc(mosq_counter_bytes_received, (int64_t)len);
			ആകുമ്പോൾ(pos < len){
				എങ്കിൽ(!mosq->in_packet.command){
					mosq->in_packet.command = buf[pos];
					pos++;
					/* Clients must send CONNECT as their first command. */
					എങ്കിൽ(mosq->state == mosq_cs_new && (mosq->in_packet.command&0xF0) != CMD_CONNECT){
						തിരികെ -1;
					}
				}
				എങ്കിൽ(mosq->in_packet.remaining_എണ്ണം <= 0){
					ചെയ്യുക{
						എങ്കിൽ(pos == len){
							തിരികെ 0;
						}
						byte = buf[pos];
						pos++;

						mosq->in_packet.remaining_എണ്ണം--;
						/* Max 4 bytes length for remaining length as defined by protocol.
						* Anything more likely means a broken/malicious client.
						*/
						എങ്കിൽ(mosq->in_packet.remaining_എണ്ണം < -4){
							തിരികെ -1;
						}

						mosq->in_packet.remaining_length += (byte & 127) * mosq->in_packet.remaining_mult;
						mosq->in_packet.remaining_mult *= 128;
					}ആകുമ്പോൾ((byte & 128) != 0);
					mosq->in_packet.remaining_എണ്ണം = (int8_t)(mosq->in_packet.remaining_എണ്ണം * -1);

					എങ്കിൽ(mosq->in_packet.remaining_length > 0){
						mosq->in_packet.payload = mosquitto_malloc(mosq->in_packet.remaining_length*വലുപ്പം(uint8_t));
						എങ്കിൽ(!mosq->in_packet.payload){
							തിരികെ -1;
						}
						mosq->in_packet.to_process = mosq->in_packet.remaining_length;
					}
				}
				എങ്കിൽ(mosq->in_packet.to_process>0){
					എങ്കിൽ((uint32_t)len - pos >= mosq->in_packet.to_process){
						memcpy(&mosq->in_packet.payload[mosq->in_packet.pos], &buf[pos], mosq->in_packet.to_process);
						mosq->in_packet.pos += mosq->in_packet.to_process;
						pos += mosq->in_packet.to_process;
						mosq->in_packet.to_process = 0;
					}അല്ലെങ്കിൽ{
						memcpy(&mosq->in_packet.payload[mosq->in_packet.pos], &buf[pos], len-pos);
						mosq->in_packet.pos += (uint32_t)(len-pos);
						mosq->in_packet.to_process -= (uint32_t)(len-pos);
						തിരികെ 0;
					}
				}
				/* All data for this packet is read. */
				mosq->in_packet.pos = 0;

#ifdef WITH_SYS_വൃക്ഷം
				metrics__int_inc(mosq_counter_messages_received, 1);
#endif
				rc = handle__packet(mosq);

				/* Free data and reset values */
				packet__cleanup(&mosq->in_packet);

				keepalive__update(mosq);

				എങ്കിൽ(rc && mosq->out_packet){
					എങ്കിൽ(mosq->state != mosq_cs_disconnecting){
						mosquitto__set_state(mosq, mosq_cs_disconnect_ws);
					}
					lws_callback_on_writable(mosq->wsi);
				}അല്ലെങ്കിൽ എങ്കിൽ(rc){
					do_disconnect(mosq, MOSQ_ERR_CONN_LOST);
					തിരികെ -1;
				}
			}
			നിർത്തുക;

		സ്ഥിരസ്ഥിതി:
			നിർത്തുക;
	}

	തിരികെ 0;
}


സ്ഥിതം അക്ഷരം *http__canonical_filename(
		ഘടന lws *wsi,
		സ്ഥിരം അക്ഷരം *in,
		സ്ഥിരം അക്ഷരം *http_dir)
{
	size_t inlen, slen;
	അക്ഷരം *filename, *filename_canonical;

	inlen = strlen(in);
	എങ്കിൽ(in[inlen-1] == '/'){
		slen = strlen(http_dir) + inlen + strlen("/index.html") + 2;
	}അല്ലെങ്കിൽ{
		slen = strlen(http_dir) + inlen + 2;
	}
	filename = mosquitto_malloc(slen);
	എങ്കിൽ(!filename){
		lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, ശൂന്യ);
		തിരികെ ശൂന്യ;
	}
	എങ്കിൽ(((അക്ഷരം *)in)[inlen-1] == '/'){
		snprintf(filename, slen, "%s%sindex.html", http_dir, (അക്ഷരം *)in);
	}അല്ലെങ്കിൽ{
		snprintf(filename, slen, "%s%s", http_dir, (അക്ഷരം *)in);
	}


	/* Get canonical path and check it is within our http_dir */
#ifdef WIN32
	filename_canonical = _fullpath(ശൂന്യ, filename, 0);
	mosquitto_FREE(filename);
	എങ്കിൽ(!filename_canonical){
		lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, ശൂന്യ);
		തിരികെ ശൂന്യ;
	}
#അല്ലെങ്കിൽ
	filename_canonical = realpath(filename, ശൂന്യ);
	mosquitto_FREE(filename);
	എങ്കിൽ(!filename_canonical){
		എങ്കിൽ(errno == EACCES){
			lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, ശൂന്യ);
		}അല്ലെങ്കിൽ എങ്കിൽ(errno == EINVAL || errno == EIO || errno == ELOOP){
			lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, ശൂന്യ);
		}അല്ലെങ്കിൽ എങ്കിൽ(errno == ENAMETOOLONG){
			lws_return_http_status(wsi, HTTP_STATUS_REQ_URI_TOO_LONG, ശൂന്യ);
		}അല്ലെങ്കിൽ എങ്കിൽ(errno == ENOENT || errno == ENOTDIR){
			lws_return_http_status(wsi, HTTP_STATUS_NOT_FOUND, ശൂന്യ);
		}
		തിരികെ ശൂന്യ;
	}
#endif
	എങ്കിൽ(strncmp(http_dir, filename_canonical, strlen(http_dir))){
		/* Requested file isn't within http_dir, deny access. */
		SAFE_FREE(filename_canonical);
		lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, ശൂന്യ);
		തിരികെ ശൂന്യ;
	}

	തിരികെ filename_canonical;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ callback_http(
		ഘടന lws *wsi,
		എണ്ണൽ lws_callback_reasons reason,
		ശൂന്യം *user,
		ശൂന്യം *in,
		size_t len)
{
	ഘടന libws_http_data *u = (ഘടന libws_http_data *)user;
	ഘടന libws_mqtt_hack *hack;
	അക്ഷരം *http_dir;
	size_t buflen;
	size_t wlen;
	പൂർണ്ണസംഖ്യ rc;
	അക്ഷരം *filename_canonical;
	unsigned അക്ഷരം buf[4096];
	ഘടന stat filestat;
	ഘടന mosquitto *mosq;
	ഘടന lws_pollargs *pollargs = (ഘടന lws_pollargs *)in;
	പൂർണ്ണസംഖ്യ hlen;

	/* FIXME - ssl cert verification is done here. */

	തിരഞ്ഞെടുപ്പ്(reason){
		അവസ്ഥ LWS_CALLBACK_HTTP:
			എങ്കിൽ(!u){
				തിരികെ -1;
			}

			hack = (ഘടന libws_mqtt_hack *)lws_context_user(lws_get_context(wsi));
			എങ്കിൽ(!hack){
				തിരികെ -1;
			}
			http_dir = hack->http_dir;

			എങ്കിൽ(!http_dir){
				/* http disabled */
				തിരികെ -1;
			}

			/* Forbid POST */
			എങ്കിൽ(lws_hdr_total_length(wsi, WSI_TOKEN_POST_URI)){
				lws_return_http_status(wsi, HTTP_STATUS_METHOD_NOT_ALLOWED, ശൂന്യ);
				തിരികെ -1;
			}

			filename_canonical = http__canonical_filename(wsi, (അക്ഷരം *)in, http_dir);
			എങ്കിൽ(!filename_canonical){
				തിരികെ -1;
			}

			u->fptr = fopen(filename_canonical, "rb");
			എങ്കിൽ(!u->fptr){
				SAFE_FREE(filename_canonical);
				lws_return_http_status(wsi, HTTP_STATUS_NOT_FOUND, ശൂന്യ);
				തിരികെ -1;
			}
			എങ്കിൽ(fstat(fileno(u->fptr), &filestat) < 0){
				SAFE_FREE(filename_canonical);
				lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, ശൂന്യ);
				fclose(u->fptr);
				u->fptr = ശൂന്യ;
				തിരികെ -1;
			}


			എങ്കിൽ((filestat.st_mode & S_IFDIR) == S_IFDIR){
				fclose(u->fptr);
				u->fptr = ശൂന്യ;
				SAFE_FREE(filename_canonical);

				/* FIXME - use header functions from lws 2.x */
				buflen = (size_t)snprintf((അക്ഷരം *)buf, 4096, "HTTP/1.0 302 OK\r\n"
						"Location: %s/\r\n\r\n",
						(അക്ഷരം *)in);
				തിരികെ lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
			}

			എങ്കിൽ((filestat.st_mode & S_IFREG) != S_IFREG){
				lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, ശൂന്യ);
				fclose(u->fptr);
				u->fptr = ശൂന്യ;
				SAFE_FREE(filename_canonical);
				തിരികെ -1;
			}

			log__printf(ശൂന്യ, MOSQ_LOG_DEBUG, "http serving file \"%s\".", filename_canonical);
			SAFE_FREE(filename_canonical);
			/* FIXME - use header functions from lws 2.x */
			buflen = (size_t)snprintf((അക്ഷരം *)buf, 4096, "HTTP/1.0 200 OK\r\n"
					"Server: mosquitto\r\n"
					"Content-Length: %u\r\n\r\n",
					(unsigned പൂർണ്ണസംഖ്യ)filestat.st_size);
			എങ്കിൽ(lws_write(wsi, buf, buflen, LWS_WRITE_HTTP) < 0){
				fclose(u->fptr);
				u->fptr = ശൂന്യ;
				തിരികെ -1;
			}
			lws_callback_on_writable(wsi);
			നിർത്തുക;

		അവസ്ഥ LWS_CALLBACK_HTTP_BODY:
			/* For extra POST data? */
			തിരികെ -1;

		അവസ്ഥ LWS_CALLBACK_HTTP_BODY_COMPLETION:
			/* For end of extra POST data? */
			തിരികെ -1;

		അവസ്ഥ LWS_CALLBACK_FILTER_HTTP_CONNECTION:
			/* Access control here */
			തിരികെ 0;

#എങ്കിൽ LWS_LIBRARY_VERSION_സംഖ്യ >= 3001000
		അവസ്ഥ LWS_CALLBACK_HTTP_CONFIRM_UPGRADE:
			hack = lws_context_user(lws_get_context(wsi));
			എങ്കിൽ(hack->listener->ws_origin_എണ്ണം){
				hlen = lws_hdr_total_length(wsi, WSI_TOKEN_ORIGIN);
				എങ്കിൽ(hlen <= 0 || hlen >= (പൂർണ്ണസംഖ്യ)വലുപ്പം(buf)){
					തിരികെ -1;
				}
				വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<hack->listener->ws_origin_എണ്ണം; i++){
					lws_hdr_copy(wsi, (അക്ഷരം *)buf, വലുപ്പം(buf), WSI_TOKEN_ORIGIN);
					എങ്കിൽ((!strcmp(hack->listener->ws_origins[i], (അക്ഷരം *)buf))){
						തിരികെ 0;
					}
				}
				തിരികെ -1;
			}അല്ലെങ്കിൽ{
				തിരികെ 0;
			}
#endif

		അവസ്ഥ LWS_CALLBACK_HTTP_WRITEABLE:
			/* Send our data here */
			എങ്കിൽ(u && u->fptr){
				ചെയ്യുക{
					buflen = fread(buf, 1, വലുപ്പം(buf), u->fptr);
					എങ്കിൽ(buflen < 1){
						fclose(u->fptr);
						u->fptr = ശൂന്യ;
						തിരികെ -1;
					}
					rc = lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
					എങ്കിൽ(rc < 0){
						തിരികെ -1;
					}
					wlen = (size_t)rc;
					/* while still active, extend timeout */
					എങ്കിൽ(wlen){
						lws_set_timeout(wsi, PENDING_TIMEOUT_HTTP_CONTENT, 10);
					}
					എങ്കിൽ(wlen < buflen){
						എങ്കിൽ(fseek(u->fptr, (long)(buflen-wlen), SEEK_CUR) < 0){
							fclose(u->fptr);
							u->fptr = ശൂന്യ;
							തിരികെ -1;
						}
					}അല്ലെങ്കിൽ{
						എങ്കിൽ(buflen < വലുപ്പം(buf)){
							fclose(u->fptr);
							u->fptr = ശൂന്യ;
						}
					}
				}ആകുമ്പോൾ(u->fptr && !lws_send_pipe_choked(wsi));
				lws_callback_on_writable(wsi);
			}അല്ലെങ്കിൽ{
				തിരികെ -1;
			}
			നിർത്തുക;

		അവസ്ഥ LWS_CALLBACK_CLOSED:
		അവസ്ഥ LWS_CALLBACK_CLOSED_HTTP:
		അവസ്ഥ LWS_CALLBACK_HTTP_FILE_COMPLETION:
			എങ്കിൽ(u && u->fptr){
				fclose(u->fptr);
				u->fptr = ശൂന്യ;
			}
			നിർത്തുക;

		അവസ്ഥ LWS_CALLBACK_ADD_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, വലുപ്പം(pollargs->fd), mosq);
			എങ്കിൽ(mosq){
				എങ്കിൽ(pollargs->events & LWS_POLLOUT){
					mux__add_out(mosq);
				}അല്ലെങ്കിൽ{
					mux__remove_out(mosq);
				}
			}അല്ലെങ്കിൽ{
				എങ്കിൽ(pollargs->events & POLLIN){
					/* Assume this is a new listener */
					listeners__add_websockets(lws_get_context(wsi), pollargs->fd);
				}
			}
			നിർത്തുക;

		അവസ്ഥ LWS_CALLBACK_DEL_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, വലുപ്പം(pollargs->fd), mosq);
			എങ്കിൽ(mosq){
				mux__delete(mosq);
			}
			നിർത്തുക;

		അവസ്ഥ LWS_CALLBACK_CHANGE_MODE_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, വലുപ്പം(pollargs->fd), mosq);
			എങ്കിൽ(mosq){
				എങ്കിൽ(pollargs->events & LWS_POLLHUP){
					തിരികെ 1;
				}അല്ലെങ്കിൽ എങ്കിൽ(pollargs->events & LWS_POLLOUT){
					mux__add_out(mosq);
				}അല്ലെങ്കിൽ{
					mux__remove_out(mosq);
				}
			}
			നിർത്തുക;

#ifdef WITH_TLS
		അവസ്ഥ LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION:
			എങ്കിൽ(!len || (SSL_get_verify_result((SSL *)in) != X509_V_OK)){
				തിരികെ 1;
			}
			നിർത്തുക;
#endif

		സ്ഥിരസ്ഥിതി:
			തിരികെ 0;
	}

	തിരികെ 0;
}


സ്ഥിതം ശൂന്യം log_wrap(പൂർണ്ണസംഖ്യ level, സ്ഥിരം അക്ഷരം *വരി)
{
	അക്ഷരം *l = (അക്ഷരം *)വരി;
	UNUSED(level);
	l[strlen(വരി)-1] = '\0'; /* Remove \n */
	log__printf(ശൂന്യ, MOSQ_LOG_WEBSOCKETS, "%s", l);
}


ശൂന്യം mosq_websockets_init(ഘടന mosquitto__listener *listener, സ്ഥിരം ഘടന mosquitto__config *conf)
{
	ഘടന lws_context_creation_info info;
	ഘടന lws_protocols *p;
	size_t protocol_എണ്ണം;
	പൂർണ്ണസംഖ്യ i;
	ഘടന libws_mqtt_hack *user;

	/* Count valid protocols */
	വേണ്ടി(protocol_എണ്ണം=0; protocols[protocol_എണ്ണം].name; protocol_എണ്ണം++){
		;
	}

	p = mosquitto_calloc(protocol_എണ്ണം+1, വലുപ്പം(ഘടന lws_protocols));
	എങ്കിൽ(!p){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Out of memory.");
		തിരികെ;
	}
	വേണ്ടി(i=0; protocols[i].name; i++){
		p[i].name = protocols[i].name;
		p[i].callback = protocols[i].callback;
		p[i].per_session_data_size = protocols[i].per_session_data_size;
		p[i].rx_buffer_size = protocols[i].rx_buffer_size;
		p[i].user = listener;
	}

	memset(&info, 0, വലുപ്പം(info));
	info.iface = listener->host;
	info.port = listener->port;
	info.protocols = p;
	info.gid = -1;
	info.uid = -1;
#ifdef WITH_TLS
	എങ്കിൽ(listener->cafile){
		info.ssl_ca_filepath = listener->cafile;
	}അല്ലെങ്കിൽ എങ്കിൽ(listener->capath){
		log__printf(ശൂന്യ, MOSQ_LOG_WARNING, "Warning: CA path option is not supported for websockets");
	}
	info.ssl_cert_filepath = listener->certfile;
	info.ssl_private_താക്കോൽ_filepath = listener->keyfile;
	info.ssl_cipher_list = listener->ciphers;
	/* HTTP 1 only, due to HTTP 2 issues in Firefox:
	   https://github.com/eclipse-mosquitto/mosquitto/issues/1211
	*/
	info.alpn = "h1";
#എങ്കിൽ defined(WITH_WEBSOCKETS) && LWS_LIBRARY_VERSION_സംഖ്യ>=3001000
	info.tls1_3_plus_cipher_list = listener->ciphers_tls13;
#endif
	എങ്കിൽ(listener->require_certificate){
		info.options |= LWS_SERVER_OPTION_REQUIRE_VALID_OPENSSL_CLIENT_CERT;
	}
#endif

	info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
	എങ്കിൽ(listener->socket_domain == AF_INET){
		info.options |= LWS_SERVER_OPTION_DISABLE_IPV6;
	}
	info.max_http_header_data = conf->packet_buffer_size;

	user = mosquitto_calloc(1, വലുപ്പം(ഘടന libws_mqtt_hack));
	എങ്കിൽ(!user){
		mosquitto_FREE(p);
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Out of memory.");
		തിരികെ;
	}

	എങ്കിൽ(listener->http_dir){
#ifdef WIN32
		user->http_dir = _fullpath(ശൂന്യ, listener->http_dir, 0);
#അല്ലെങ്കിൽ
		user->http_dir = realpath(listener->http_dir, ശൂന്യ);
#endif
		എങ്കിൽ(!user->http_dir){
			mosquitto_FREE(user);
			mosquitto_FREE(p);
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to open http dir \"%s\".", listener->http_dir);
			തിരികെ;
		}
	}
	user->listener = listener;

	info.user = user;
	info.pt_serv_buf_size = WS_SERV_BUF_SIZE;
	listener->ws_protocol = p;

	lws_set_log_level(conf->websockets_log_level, log_wrap);

	log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Opening websockets listen socket on port %d.", listener->port);
	listener->ws_in_init = true;
	listener->ws_context = lws_create_context(&info);
	listener->ws_in_init = false;
}

#endif
#endif
