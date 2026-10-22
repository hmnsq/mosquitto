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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_WEBSOCKETS

#ಅನ್ತರ್ಭಾವ "config.h"

#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ "mosquitto_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "packet_mosq.h"
#ಅನ್ತರ್ಭಾವ "sys_tree.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"

#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
#  include <sys/socket.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
/* Be careful if changing these, if TX is not bigger than SERV then there can
 * be very large write performance penalties.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು WS_SERV_BUF_ಗಾತ್ರ 4096
#ವ್ಯಾಖ್ಯಾನಿಸು WS_TX_BUF_ಗಾತ್ರ (WS_SERV_BUF_ಗಾತ್ರ*2)

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ callback_mqtt(
		ರಚನೆ lws *wsi,
		ಎಣಿಕೆ lws_callback_reasons reason,
		ಶೂನ್ಯ *user,
		ಶೂನ್ಯ *ಒಳ,
		size_t len);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ callback_http(
		ರಚನೆ lws *wsi,
		ಎಣಿಕೆ lws_callback_reasons reason,
		ಶೂನ್ಯ *user,
		ಶೂನ್ಯ *ಒಳ,
		size_t len);

ಎಣಿಕೆ mosq_ws_protocols {
	PROTOCOL_HTTP = 0,
	PROTOCOL_MQTT,
	DEMO_PROTOCOL_ಎಣಿಕೆ,
};

ರಚನೆ libws_http_ದತ್ತಾಂಶ {
	FILE *fptr;
};

ಸ್ಥಿರ ರಚನೆ lws_protocols protocols[] = {
	/* first protocol must always be HTTP handler */
	{
		"http-only",                        /* name */
		callback_http,                      /* lws_callback_function */
		ಗಾತ್ರ (ರಚನೆ libws_http_ದತ್ತಾಂಶ),    /* per_session_data_size */
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		ಖಾಲಿ,                               /* user v1.4 on */
		WS_TX_BUF_ಗಾತ್ರ                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqtt",
		callback_mqtt,
		ಗಾತ್ರ(ರಚನೆ libws_mqtt_ದತ್ತಾಂಶ),
		0,                                  /* rx_buffer_size */
		1,                                  /* id */
		ಖಾಲಿ,                               /* user v1.4 on */
		WS_TX_BUF_ಗಾತ್ರ                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqttv3.1",
		callback_mqtt,
		ಗಾತ್ರ(ರಚನೆ libws_mqtt_ದತ್ತಾಂಶ),
		0,                                  /* rx_buffer_size */
		2,                                  /* id */
		ಖಾಲಿ,                               /* user v1.4 on */
		WS_TX_BUF_ಗಾತ್ರ                      /* tx_packet_size v2.3.0 */
	},
	{
		ಖಾಲಿ,
		ಖಾಲಿ,
		0,
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		ಖಾಲಿ,                               /* user v1.4 on */
		0                                   /* tx_packet_size v2.3.0 */
	}
};


ಸ್ಥಿರ ಶೂನ್ಯ easy_ವಿಳಾಸ(ಪೂರ್ಣಾಂಕ sock, ರಚನೆ mosquitto *mosq)
{
	ಅಕ್ಷರ ವಿಳಾಸ[1024];

	ಆದರೆ(!net_socket_get_ವಿಳಾಸ(sock, ವಿಳಾಸ, 1024, &mosq->remote_ಪೋರ್ಟ್)){
		mosq->ವಿಳಾಸ = mosquitto_strdup(ವಿಳಾಸ);
	}
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ callback_mqtt(
		ರಚನೆ lws *wsi,
		ಎಣಿಕೆ lws_callback_reasons reason,
		ಶೂನ್ಯ *user,
		ಶೂನ್ಯ *ಒಳ,
		size_t len)
{
	ರಚನೆ mosquitto *mosq = ಖಾಲಿ;
	ರಚನೆ mosquitto__packet *packet;
	ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ucount;
	ಸ್ಥಿರಾಂಕ ರಚನೆ lws_protocols *p;
	ರಚನೆ libws_mqtt_ದತ್ತಾಂಶ *u = (ರಚನೆ libws_mqtt_ದತ್ತಾಂಶ *)user;
	size_t pos;
	uint8_t *buf;
	ಪೂರ್ಣಾಂಕ rc;
	uint8_t byte;
	ಅಕ್ಷರ ip_addr_buff[1024];

	ಆಯ್ಕೆ(reason){
		ಸ್ಥಿತಿ LWS_CALLBACK_ESTABLISHED:
			mosq = context__init();
			ಆದರೆ(mosq){
				p = lws_get_protocol(wsi);
				mosq->listener = p->user;
				ಆದರೆ(!mosq->listener){
					mosquitto_FREE(mosq);
					ಹಿಂತಿರುಗಿ -1;
				}
				mosq->wsi = wsi;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
				ಆದರೆ(ಒಳ){
					mosq->ssl = (SSL *)ಒಳ;
					ಆದರೆ(!mosq->listener->ssl_ctx){
						mosq->listener->ssl_ctx = SSL_get_SSL_CTX(mosq->ssl);
					}
				}
#ಆದರೆ_ಅಂತ್ಯ
				u->mosq = mosq;
			}ಇಲ್ಲದಿದ್ದರೆ{
				ಹಿಂತಿರುಗಿ -1;
			}

			easy_ವಿಳಾಸ(lws_get_socket_fd(wsi), mosq);
			ಆದರೆ(!mosq->ವಿಳಾಸ){
				/* getpeername and inet_ntop failed and not a bridge */
				mosquitto_FREE(mosq);
				u->mosq = ಖಾಲಿ;
				ಹಿಂತಿರುಗಿ -1;
			}
			mosq->listener->client_ಎಣಿಕೆ++;
			ಆದರೆ((mosq->listener->max_connections > 0 && mosq->listener->client_ಎಣಿಕೆ > mosq->listener->max_connections)
					|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)db.config->global_max_connections)){

				ಆದರೆ(db.config->connection_messages == true){
					log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", mosq->ವಿಳಾಸ);
				}
				mosq->listener->client_ಎಣಿಕೆ--;
				mosquitto_FREE(mosq->ವಿಳಾಸ);
				mosquitto_FREE(mosq);
				u->mosq = ಖಾಲಿ;
				ಹಿಂತಿರುಗಿ -1;
			}
			mosq->sock = lws_get_socket_fd(wsi);
			HASH_ಕೂಡಿಸು(hh_sock, db.contexts_by_sock, sock, ಗಾತ್ರ(mosq->sock), mosq);
			mux__new(mosq);
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ LWS_CALLBACK_CLOSED:
			ಆದರೆ(!u){
				ಹಿಂತಿರುಗಿ -1;
			}
			mosq = u->mosq;
			ಆದರೆ(mosq){
				ಆದರೆ(mosq->sock != INVALID_SOCKET){
					HASH_DELETE(hh_sock, db.contexts_by_sock, mosq);
					mosq->sock = INVALID_SOCKET;
					mux__delete(mosq);
				}
				mosq->wsi = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
				mosq->ssl = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
				do_disconnect(mosq, MOSQ_ERR_CONN_ಕಳೆದುಹೋದ);
			}
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ LWS_CALLBACK_SERVER_WRITEABLE:
			ಆದರೆ(!u){
				ಹಿಂತಿರುಗಿ -1;
			}
			mosq = u->mosq;
			ಆದರೆ(!mosq){
				ಹಿಂತಿರುಗಿ -1;
			}

			rc = db_ಸಂದೇಶ_write_inflight_ಹೊರಹೋಗು_latest(mosq);
			ಆದರೆ(rc){
				ಹಿಂತಿರುಗಿ -1;
			}
			rc = db_ಸಂದೇಶ_write_queued_ಹೊರಹೋಗು(mosq);
			ಆದರೆ(rc){
				ಹಿಂತಿರುಗಿ -1;
			}

			ಆಗ(mosq->ಹೊರಹೋಗು_packet && !lws_send_pipe_choked(mosq->wsi)){
				packet = mosq->ಹೊರಹೋಗು_packet;
				ಎಣಿಕೆ_2 = lws_write(wsi, &packet->payload[packet->pos], packet->ಗಮ್ಯ_process, LWS_WRITE_ದ್ವಿಮಾನ);
				ಆದರೆ(ಎಣಿಕೆ_2 < 0){
					ಆದರೆ(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						ಹಿಂತಿರುಗಿ -1;
					}
					ಹಿಂತಿರುಗಿ 0;
				}
				ucount = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)ಎಣಿಕೆ_2;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_SYS_ವೃಕ್ಷ
				metrics__int_inc(mosq_counter_ಬೈಟುಗಳು_ಕಳುಹಿಸಿದ, ucount);
#ಆದರೆ_ಅಂತ್ಯ
				packet->ಗಮ್ಯ_process -= ucount;
				packet->pos += ucount;
				ಆದರೆ(packet->ಗಮ್ಯ_process > 0){
					ಆದರೆ(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						ಹಿಂತಿರುಗಿ -1;
					}
					ನಿಲ್ಲಿಸು;
				}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_SYS_ವೃಕ್ಷ
				metrics__int_inc(mosq_counter_messages_ಕಳುಹಿಸಿದ, 1);
				ಆದರೆ(((packet->command)&0xF0) == CMD_PUBLISH){
					metrics__int_inc(mosq_counter_mqtt_publish_ಕಳುಹಿಸಿದ, 1);
				}
#ಆದರೆ_ಅಂತ್ಯ

				packet_get_ಮುಂದಿನ_ಹೊರಹೋಗು(mosq);
				mosquitto_FREE(packet);

				mosq->ಮುಂದಿನ_ಸಂದೇಶ_ಹೊರಹೋಗು = db.now_s + mosq->keepalive;
			}
			ಆದರೆ(mosq->state == mosq_cs_disconnect_ws
					|| mosq->state == mosq_cs_disconnecting
					|| mosq->state == mosq_cs_disused){

				ಹಿಂತಿರುಗಿ -1;
			}
			ಆದರೆ(mosq->ಹೊರಹೋಗು_packet){
				lws_callback_on_writable(mosq->wsi);
			}
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ LWS_CALLBACK_RECEIVE:
			ಆದರೆ(!u || !u->mosq){
				ಹಿಂತಿರುಗಿ -1;
			}
			mosq = u->mosq;
			pos = 0;
			buf = (uint8_t *)ಒಳ;
			metrics__int_inc(mosq_counter_ಬೈಟುಗಳು_received, (int64_t)len);
			ಆಗ(pos < len){
				ಆದರೆ(!mosq->ಒಳ_packet.command){
					mosq->ಒಳ_packet.command = buf[pos];
					pos++;
					/* Clients must send CONNECT as their first command. */
					ಆದರೆ(mosq->state == mosq_cs_new && (mosq->ಒಳ_packet.command&0xF0) != CMD_CONNECT){
						ಹಿಂತಿರುಗಿ -1;
					}
				}
				ಆದರೆ(mosq->ಒಳ_packet.ಉಳಿದ_ಎಣಿಕೆ <= 0){
					ಮಾಡು{
						ಆದರೆ(pos == len){
							ಹಿಂತಿರುಗಿ 0;
						}
						byte = buf[pos];
						pos++;

						mosq->ಒಳ_packet.ಉಳಿದ_ಎಣಿಕೆ--;
						/* Max 4 bytes length for remaining length as defined by protocol.
						* Anything more likely means a broken/malicious client.
						*/
						ಆದರೆ(mosq->ಒಳ_packet.ಉಳಿದ_ಎಣಿಕೆ < -4){
							ಹಿಂತಿರುಗಿ -1;
						}

						mosq->ಒಳ_packet.ಉಳಿದ_ಉದ್ದಳತೆ += (byte & 127) * mosq->ಒಳ_packet.ಉಳಿದ_mult;
						mosq->ಒಳ_packet.ಉಳಿದ_mult *= 128;
					}ಆಗ((byte & 128) != 0);
					mosq->ಒಳ_packet.ಉಳಿದ_ಎಣಿಕೆ = (int8_t)(mosq->ಒಳ_packet.ಉಳಿದ_ಎಣಿಕೆ * -1);

					ಆದರೆ(mosq->ಒಳ_packet.ಉಳಿದ_ಉದ್ದಳತೆ > 0){
						mosq->ಒಳ_packet.payload = mosquitto_malloc(mosq->ಒಳ_packet.ಉಳಿದ_ಉದ್ದಳತೆ*ಗಾತ್ರ(uint8_t));
						ಆದರೆ(!mosq->ಒಳ_packet.payload){
							ಹಿಂತಿರುಗಿ -1;
						}
						mosq->ಒಳ_packet.ಗಮ್ಯ_process = mosq->ಒಳ_packet.ಉಳಿದ_ಉದ್ದಳತೆ;
					}
				}
				ಆದರೆ(mosq->ಒಳ_packet.ಗಮ್ಯ_process>0){
					ಆದರೆ((uint32_t)len - pos >= mosq->ಒಳ_packet.ಗಮ್ಯ_process){
						ಸ್ಮರಣೆ_ನಕಲು(&mosq->ಒಳ_packet.payload[mosq->ಒಳ_packet.pos], &buf[pos], mosq->ಒಳ_packet.ಗಮ್ಯ_process);
						mosq->ಒಳ_packet.pos += mosq->ಒಳ_packet.ಗಮ್ಯ_process;
						pos += mosq->ಒಳ_packet.ಗಮ್ಯ_process;
						mosq->ಒಳ_packet.ಗಮ್ಯ_process = 0;
					}ಇಲ್ಲದಿದ್ದರೆ{
						ಸ್ಮರಣೆ_ನಕಲು(&mosq->ಒಳ_packet.payload[mosq->ಒಳ_packet.pos], &buf[pos], len-pos);
						mosq->ಒಳ_packet.pos += (uint32_t)(len-pos);
						mosq->ಒಳ_packet.ಗಮ್ಯ_process -= (uint32_t)(len-pos);
						ಹಿಂತಿರುಗಿ 0;
					}
				}
				/* All data for this packet is read. */
				mosq->ಒಳ_packet.pos = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_SYS_ವೃಕ್ಷ
				metrics__int_inc(mosq_counter_messages_received, 1);
#ಆದರೆ_ಅಂತ್ಯ
				rc = handle__packet(mosq);

				/* Free data and reset values */
				packet__cleanup(&mosq->ಒಳ_packet);

				keepalive__update(mosq);

				ಆದರೆ(rc && mosq->ಹೊರಹೋಗು_packet){
					ಆದರೆ(mosq->state != mosq_cs_disconnecting){
						mosquitto_ಗಣ_state(mosq, mosq_cs_disconnect_ws);
					}
					lws_callback_on_writable(mosq->wsi);
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc){
					do_disconnect(mosq, MOSQ_ERR_CONN_ಕಳೆದುಹೋದ);
					ಹಿಂತಿರುಗಿ -1;
				}
			}
			ನಿಲ್ಲಿಸು;

		ಪೂರ್ವನಿಯೋಜಿತ:
			ನಿಲ್ಲಿಸು;
	}

	ಹಿಂತಿರುಗಿ 0;
}


ಸ್ಥಿರ ಅಕ್ಷರ *http__canonical_filename(
		ರಚನೆ lws *wsi,
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳ,
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *http_dir)
{
	size_t inlen, slen;
	ಅಕ್ಷರ *filename, *filename_canonical;

	inlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಒಳ);
	ಆದರೆ(ಒಳ[inlen-1] == '/'){
		slen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(http_dir) + inlen + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("/index.html") + 2;
	}ಇಲ್ಲದಿದ್ದರೆ{
		slen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(http_dir) + inlen + 2;
	}
	filename = mosquitto_malloc(slen);
	ಆದರೆ(!filename){
		lws_return_http_ಸ್ಥಿತಿ(wsi, HTTP_ಸ್ಥಿತಿ_ಆಂತರಿಕ_SERVER_ERROR, ಖಾಲಿ);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	ಆದರೆ(((ಅಕ್ಷರ *)ಒಳ)[inlen-1] == '/'){
		snprintf(filename, slen, "%s%sindex.html", http_dir, (ಅಕ್ಷರ *)ಒಳ);
	}ಇಲ್ಲದಿದ್ದರೆ{
		snprintf(filename, slen, "%s%s", http_dir, (ಅಕ್ಷರ *)ಒಳ);
	}


	/* Get canonical path and check it is within our http_dir */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
	filename_canonical = _fullpath(ಖಾಲಿ, filename, 0);
	mosquitto_FREE(filename);
	ಆದರೆ(!filename_canonical){
		lws_return_http_ಸ್ಥಿತಿ(wsi, HTTP_ಸ್ಥಿತಿ_ಆಂತರಿಕ_SERVER_ERROR, ಖಾಲಿ);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
#ಇಲ್ಲದಿದ್ದರೆ
	filename_canonical = realpath(filename, ಖಾಲಿ);
	mosquitto_FREE(filename);
	ಆದರೆ(!filename_canonical){
		ಆದರೆ(errno == EACCES){
			lws_return_http_ಸ್ಥಿತಿ(wsi, HTTP_ಸ್ಥಿತಿ_FORBIDDEN, ಖಾಲಿ);
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(errno == EINVAL || errno == EIO || errno == ELOOP){
			lws_return_http_ಸ್ಥಿತಿ(wsi, HTTP_ಸ್ಥಿತಿ_ಆಂತರಿಕ_SERVER_ERROR, ಖಾಲಿ);
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(errno == ENAMETOOLONG){
			lws_return_http_ಸ್ಥಿತಿ(wsi, HTTP_ಸ್ಥಿತಿ_REQ_URI_TOO_LONG, ಖಾಲಿ);
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(errno == ENOENT || errno == ENOTDIR){
			lws_return_http_ಸ್ಥಿತಿ(wsi, HTTP_ಸ್ಥಿತಿ_NOT_ಸಿಕ್ಕಿತು, ಖಾಲಿ);
		}
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(http_dir, filename_canonical, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(http_dir))){
		/* Requested file isn't within http_dir, deny access. */
		ಸುರಕ್ಷಿತ_FREE(filename_canonical);
		lws_return_http_ಸ್ಥಿತಿ(wsi, HTTP_ಸ್ಥಿತಿ_FORBIDDEN, ಖಾಲಿ);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	ಹಿಂತಿರುಗಿ filename_canonical;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ callback_http(
		ರಚನೆ lws *wsi,
		ಎಣಿಕೆ lws_callback_reasons reason,
		ಶೂನ್ಯ *user,
		ಶೂನ್ಯ *ಒಳ,
		size_t len)
{
	ರಚನೆ libws_http_ದತ್ತಾಂಶ *u = (ರಚನೆ libws_http_ದತ್ತಾಂಶ *)user;
	ರಚನೆ libws_mqtt_hack *hack;
	ಅಕ್ಷರ *http_dir;
	size_t buflen;
	size_t wlen;
	ಪೂರ್ಣಾಂಕ rc;
	ಅಕ್ಷರ *filename_canonical;
	ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ buf[4096];
	ರಚನೆ stat filestat;
	ರಚನೆ mosquitto *mosq;
	ರಚನೆ lws_pollargs *pollargs = (ರಚನೆ lws_pollargs *)ಒಳ;
	ಪೂರ್ಣಾಂಕ hlen;

	/* FIXME - ssl cert verification is done here. */

	ಆಯ್ಕೆ(reason){
		ಸ್ಥಿತಿ LWS_CALLBACK_HTTP:
			ಆದರೆ(!u){
				ಹಿಂತಿರುಗಿ -1;
			}

			hack = (ರಚನೆ libws_mqtt_hack *)lws_context_user(lws_get_context(wsi));
			ಆದರೆ(!hack){
				ಹಿಂತಿರುಗಿ -1;
			}
			http_dir = hack->http_dir;

			ಆದರೆ(!http_dir){
				/* http disabled */
				ಹಿಂತಿರುಗಿ -1;
			}

			/* Forbid POST */
			ಆದರೆ(lws_hdr_ಒಟ್ಟು_ಉದ್ದಳತೆ(wsi, WSI_TOKEN_POST_URI)){
				lws_return_http_ಸ್ಥಿತಿ(wsi, HTTP_ಸ್ಥಿತಿ_ವಿಧಾನ_NOT_ALLOWED, ಖಾಲಿ);
				ಹಿಂತಿರುಗಿ -1;
			}

			filename_canonical = http__canonical_filename(wsi, (ಅಕ್ಷರ *)ಒಳ, http_dir);
			ಆದರೆ(!filename_canonical){
				ಹಿಂತಿರುಗಿ -1;
			}

			u->fptr = fopen(filename_canonical, "rb");
			ಆದರೆ(!u->fptr){
				ಸುರಕ್ಷಿತ_FREE(filename_canonical);
				lws_return_http_ಸ್ಥಿತಿ(wsi, HTTP_ಸ್ಥಿತಿ_NOT_ಸಿಕ್ಕಿತು, ಖಾಲಿ);
				ಹಿಂತಿರುಗಿ -1;
			}
			ಆದರೆ(fstat(fileno(u->fptr), &filestat) < 0){
				ಸುರಕ್ಷಿತ_FREE(filename_canonical);
				lws_return_http_ಸ್ಥಿತಿ(wsi, HTTP_ಸ್ಥಿತಿ_ಆಂತರಿಕ_SERVER_ERROR, ಖಾಲಿ);
				fclose(u->fptr);
				u->fptr = ಖಾಲಿ;
				ಹಿಂತಿರುಗಿ -1;
			}


			ಆದರೆ((filestat.st_ಬಹುಲಕ & S_IFDIR) == S_IFDIR){
				fclose(u->fptr);
				u->fptr = ಖಾಲಿ;
				ಸುರಕ್ಷಿತ_FREE(filename_canonical);

				/* FIXME - use header functions from lws 2.x */
				buflen = (size_t)snprintf((ಅಕ್ಷರ *)buf, 4096, "HTTP/1.0 302 OK\r\n"
						"Location: %s/\r\n\r\n",
						(ಅಕ್ಷರ *)ಒಳ);
				ಹಿಂತಿರುಗಿ lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
			}

			ಆದರೆ((filestat.st_ಬಹುಲಕ & S_IFREG) != S_IFREG){
				lws_return_http_ಸ್ಥಿತಿ(wsi, HTTP_ಸ್ಥಿತಿ_FORBIDDEN, ಖಾಲಿ);
				fclose(u->fptr);
				u->fptr = ಖಾಲಿ;
				ಸುರಕ್ಷಿತ_FREE(filename_canonical);
				ಹಿಂತಿರುಗಿ -1;
			}

			log__printf(ಖಾಲಿ, MOSQ_LOG_DEBUG, "http serving file \"%s\".", filename_canonical);
			ಸುರಕ್ಷಿತ_FREE(filename_canonical);
			/* FIXME - use header functions from lws 2.x */
			buflen = (size_t)snprintf((ಅಕ್ಷರ *)buf, 4096, "HTTP/1.0 200 OK\r\n"
					"Server: mosquitto\r\n"
					"Content-Length: %u\r\n\r\n",
					(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)filestat.st_ಗಾತ್ರ);
			ಆದರೆ(lws_write(wsi, buf, buflen, LWS_WRITE_HTTP) < 0){
				fclose(u->fptr);
				u->fptr = ಖಾಲಿ;
				ಹಿಂತಿರುಗಿ -1;
			}
			lws_callback_on_writable(wsi);
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ LWS_CALLBACK_HTTP_BODY:
			/* For extra POST data? */
			ಹಿಂತಿರುಗಿ -1;

		ಸ್ಥಿತಿ LWS_CALLBACK_HTTP_BODY_COMPLETION:
			/* For end of extra POST data? */
			ಹಿಂತಿರುಗಿ -1;

		ಸ್ಥಿತಿ LWS_CALLBACK_FILTER_HTTP_CONNECTION:
			/* Access control here */
			ಹಿಂತಿರುಗಿ 0;

#ಆದರೆ LWS_LIBRARY_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ >= 3001000
		ಸ್ಥಿತಿ LWS_CALLBACK_HTTP_CONFIRM_UPGRADE:
			hack = lws_context_user(lws_get_context(wsi));
			ಆದರೆ(hack->listener->ws_origin_ಎಣಿಕೆ){
				hlen = lws_hdr_ಒಟ್ಟು_ಉದ್ದಳತೆ(wsi, WSI_TOKEN_ORIGIN);
				ಆದರೆ(hlen <= 0 || hlen >= (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(buf)){
					ಹಿಂತಿರುಗಿ -1;
				}
				ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<hack->listener->ws_origin_ಎಣಿಕೆ; i++){
					lws_hdr_copy(wsi, (ಅಕ್ಷರ *)buf, ಗಾತ್ರ(buf), WSI_TOKEN_ORIGIN);
					ಆದರೆ((!strcmp(hack->listener->ws_origins[i], (ಅಕ್ಷರ *)buf))){
						ಹಿಂತಿರುಗಿ 0;
					}
				}
				ಹಿಂತಿರುಗಿ -1;
			}ಇಲ್ಲದಿದ್ದರೆ{
				ಹಿಂತಿರುಗಿ 0;
			}
#ಆದರೆ_ಅಂತ್ಯ

		ಸ್ಥಿತಿ LWS_CALLBACK_HTTP_WRITEABLE:
			/* Send our data here */
			ಆದರೆ(u && u->fptr){
				ಮಾಡು{
					buflen = fread(buf, 1, ಗಾತ್ರ(buf), u->fptr);
					ಆದರೆ(buflen < 1){
						fclose(u->fptr);
						u->fptr = ಖಾಲಿ;
						ಹಿಂತಿರುಗಿ -1;
					}
					rc = lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
					ಆದರೆ(rc < 0){
						ಹಿಂತಿರುಗಿ -1;
					}
					wlen = (size_t)rc;
					/* while still active, extend timeout */
					ಆದರೆ(wlen){
						lws_ಗಣ_timeout(wsi, PENDING_TIMEOUT_HTTP_CONTENT, 10);
					}
					ಆದರೆ(wlen < buflen){
						ಆದರೆ(fseek(u->fptr, (ದೀರ್ಘ)(buflen-wlen), SEEK_CUR) < 0){
							fclose(u->fptr);
							u->fptr = ಖಾಲಿ;
							ಹಿಂತಿರುಗಿ -1;
						}
					}ಇಲ್ಲದಿದ್ದರೆ{
						ಆದರೆ(buflen < ಗಾತ್ರ(buf)){
							fclose(u->fptr);
							u->fptr = ಖಾಲಿ;
						}
					}
				}ಆಗ(u->fptr && !lws_send_pipe_choked(wsi));
				lws_callback_on_writable(wsi);
			}ಇಲ್ಲದಿದ್ದರೆ{
				ಹಿಂತಿರುಗಿ -1;
			}
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ LWS_CALLBACK_CLOSED:
		ಸ್ಥಿತಿ LWS_CALLBACK_CLOSED_HTTP:
		ಸ್ಥಿತಿ LWS_CALLBACK_HTTP_FILE_COMPLETION:
			ಆದರೆ(u && u->fptr){
				fclose(u->fptr);
				u->fptr = ಖಾಲಿ;
			}
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ LWS_CALLBACK_ಕೂಡಿಸು_POLL_FD:
			HASH_ಕಂಡುಹಿಡಿ(hh_sock, db.contexts_by_sock, &pollargs->fd, ಗಾತ್ರ(pollargs->fd), mosq);
			ಆದರೆ(mosq){
				ಆದರೆ(pollargs->events & LWS_POLLOUT){
					mux_ಕೂಡಿಸು_ಹೊರಹೋಗು(mosq);
				}ಇಲ್ಲದಿದ್ದರೆ{
					mux_remove_ಹೊರಹೋಗು(mosq);
				}
			}ಇಲ್ಲದಿದ್ದರೆ{
				ಆದರೆ(pollargs->events & POLLIN){
					/* Assume this is a new listener */
					listeners_ಕೂಡಿಸು_websockets(lws_get_context(wsi), pollargs->fd);
				}
			}
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ LWS_CALLBACK_DEL_POLL_FD:
			HASH_ಕಂಡುಹಿಡಿ(hh_sock, db.contexts_by_sock, &pollargs->fd, ಗಾತ್ರ(pollargs->fd), mosq);
			ಆದರೆ(mosq){
				mux__delete(mosq);
			}
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ LWS_CALLBACK_CHANGE_ಬಹುಲಕ_POLL_FD:
			HASH_ಕಂಡುಹಿಡಿ(hh_sock, db.contexts_by_sock, &pollargs->fd, ಗಾತ್ರ(pollargs->fd), mosq);
			ಆದರೆ(mosq){
				ಆದರೆ(pollargs->events & LWS_POLLHUP){
					ಹಿಂತಿರುಗಿ 1;
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(pollargs->events & LWS_POLLOUT){
					mux_ಕೂಡಿಸು_ಹೊರಹೋಗು(mosq);
				}ಇಲ್ಲದಿದ್ದರೆ{
					mux_remove_ಹೊರಹೋಗು(mosq);
				}
			}
			ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
		ಸ್ಥಿತಿ LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION:
			ಆದರೆ(!len || (SSL_get_verify_ಫಲಿತಾಂಶ((SSL *)ಒಳ) != X509_V_OK)){
				ಹಿಂತಿರುಗಿ 1;
			}
			ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

		ಪೂರ್ವನಿಯೋಜಿತ:
			ಹಿಂತಿರುಗಿ 0;
	}

	ಹಿಂತಿರುಗಿ 0;
}


ಸ್ಥಿರ ಶೂನ್ಯ log_wrap(ಪೂರ್ಣಾಂಕ level, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು)
{
	ಅಕ್ಷರ *l = (ಅಕ್ಷರ *)ಸಾಲು;
	ಬಳಸದ(level);
	l[ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಸಾಲು)-1] = '\0'; /* Remove \n */
	log__printf(ಖಾಲಿ, MOSQ_LOG_WEBSOCKETS, "%s", l);
}


ಶೂನ್ಯ mosq_websockets_init(ರಚನೆ mosquitto__listener *listener, ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto__config *conf)
{
	ರಚನೆ lws_context_creation_info info;
	ರಚನೆ lws_protocols *p;
	size_t protocol_ಎಣಿಕೆ;
	ಪೂರ್ಣಾಂಕ i;
	ರಚನೆ libws_mqtt_hack *user;

	/* Count valid protocols */
	ಗಾಗಿ(protocol_ಎಣಿಕೆ=0; protocols[protocol_ಎಣಿಕೆ].name; protocol_ಎಣಿಕೆ++){
		;
	}

	p = mosquitto_calloc(protocol_ಎಣಿಕೆ+1, ಗಾತ್ರ(ರಚನೆ lws_protocols));
	ಆದರೆ(!p){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Out of memory.");
		ಹಿಂತಿರುಗಿ;
	}
	ಗಾಗಿ(i=0; protocols[i].name; i++){
		p[i].name = protocols[i].name;
		p[i].callback = protocols[i].callback;
		p[i].ಪ್ರತಿ_session_ದತ್ತಾಂಶ_ಗಾತ್ರ = protocols[i].ಪ್ರತಿ_session_ದತ್ತಾಂಶ_ಗಾತ್ರ;
		p[i].rx_ಬಫರ್_ಗಾತ್ರ = protocols[i].rx_ಬಫರ್_ಗಾತ್ರ;
		p[i].user = listener;
	}

	memset(&info, 0, ಗಾತ್ರ(info));
	info.iface = listener->host;
	info.ಪೋರ್ಟ್ = listener->ಪೋರ್ಟ್;
	info.protocols = p;
	info.gid = -1;
	info.uid = -1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
	ಆದರೆ(listener->cafile){
		info.ssl_ca_filepath = listener->cafile;
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(listener->capath){
		log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: CA path option is not supported for websockets");
	}
	info.ssl_cert_filepath = listener->certfile;
	info.ssl_ಖಾಸಗಿ_ಕೀಲಿ_filepath = listener->keyfile;
	info.ssl_cipher_list = listener->ciphers;
	/* HTTP 1 only, due to HTTP 2 issues in Firefox:
	   https://github.com/eclipse-mosquitto/mosquitto/issues/1211
	*/
	info.alpn = "h1";
#ಆದರೆ defined(WITH_WEBSOCKETS) && LWS_LIBRARY_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ>=3001000
	info.tls1_3_plus_cipher_list = listener->ciphers_tls13;
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ(listener->require_certificate){
		info.options |= LWS_SERVER_OPTION_REQUIRE_VALID_OPENSSL_CLIENT_CERT;
	}
#ಆದರೆ_ಅಂತ್ಯ

	info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
	ಆದರೆ(listener->socket_domain == AF_INET){
		info.options |= LWS_SERVER_OPTION_DISABLE_IPV6;
	}
	info.max_http_ಶೀರ್ಷಿಕೆ_ದತ್ತಾಂಶ = conf->packet_ಬಫರ್_ಗಾತ್ರ;

	user = mosquitto_calloc(1, ಗಾತ್ರ(ರಚನೆ libws_mqtt_hack));
	ಆದರೆ(!user){
		mosquitto_FREE(p);
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Out of memory.");
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ(listener->http_dir){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
		user->http_dir = _fullpath(ಖಾಲಿ, listener->http_dir, 0);
#ಇಲ್ಲದಿದ್ದರೆ
		user->http_dir = realpath(listener->http_dir, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ(!user->http_dir){
			mosquitto_FREE(user);
			mosquitto_FREE(p);
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to open http dir \"%s\".", listener->http_dir);
			ಹಿಂತಿರುಗಿ;
		}
	}
	user->listener = listener;

	info.user = user;
	info.pt_serv_buf_ಗಾತ್ರ = WS_SERV_BUF_ಗಾತ್ರ;
	listener->ws_protocol = p;

	lws_ಗಣ_log_level(conf->websockets_log_level, log_wrap);

	log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Opening websockets listen socket on port %d.", listener->ಪೋರ್ಟ್);
	listener->ws_ಒಳ_init = true;
	listener->ws_context = lws_create_context(&info);
	listener->ws_ಒಳ_init = false;
}

#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ
