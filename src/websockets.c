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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_WEBSOCKETS

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "packet_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sys_tree.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
#  include <sys/socket.h>
#ਜੇ_ਅੰਤ

#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
/* Be careful if changing these, if TX is not bigger than SERV then there can
 * be very large write performance penalties.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WS_SERV_BUF_ਮਾਪ 4096
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WS_TX_BUF_ਮਾਪ (WS_SERV_BUF_ਮਾਪ*2)

ਸਥਿਰ ਪੂਰਨ callback_mqtt(
		ਬਣਤਰ lws *wsi,
		ਗਿਣਤੀ lws_callback_reasons reason,
		ਖਾਲੀ *user,
		ਖਾਲੀ *ਵਿੱਚ,
		size_t len);

ਸਥਿਰ ਪੂਰਨ callback_http(
		ਬਣਤਰ lws *wsi,
		ਗਿਣਤੀ lws_callback_reasons reason,
		ਖਾਲੀ *user,
		ਖਾਲੀ *ਵਿੱਚ,
		size_t len);

ਗਿਣਤੀ mosq_ws_protocols {
	PROTOCOL_HTTP = 0,
	PROTOCOL_MQTT,
	DEMO_PROTOCOL_ਗਿਣਤੀ,
};

ਬਣਤਰ libws_http_ਅੰਕੜੇ {
	FILE *fptr;
};

ਸਥਿਰ ਬਣਤਰ lws_protocols protocols[] = {
	/* first protocol must always be HTTP handler */
	{
		"http-only",                        /* name */
		callback_http,                      /* lws_callback_function */
		ਆਕਾਰ (ਬਣਤਰ libws_http_ਅੰਕੜੇ),    /* per_session_data_size */
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		ਨਿਰਾ,                               /* user v1.4 on */
		WS_TX_BUF_ਮਾਪ                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqtt",
		callback_mqtt,
		ਆਕਾਰ(ਬਣਤਰ libws_mqtt_ਅੰਕੜੇ),
		0,                                  /* rx_buffer_size */
		1,                                  /* id */
		ਨਿਰਾ,                               /* user v1.4 on */
		WS_TX_BUF_ਮਾਪ                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqttv3.1",
		callback_mqtt,
		ਆਕਾਰ(ਬਣਤਰ libws_mqtt_ਅੰਕੜੇ),
		0,                                  /* rx_buffer_size */
		2,                                  /* id */
		ਨਿਰਾ,                               /* user v1.4 on */
		WS_TX_BUF_ਮਾਪ                      /* tx_packet_size v2.3.0 */
	},
	{
		ਨਿਰਾ,
		ਨਿਰਾ,
		0,
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		ਨਿਰਾ,                               /* user v1.4 on */
		0                                   /* tx_packet_size v2.3.0 */
	}
};


ਸਥਿਰ ਖਾਲੀ easy_ਪਤਾ(ਪੂਰਨ sock, ਬਣਤਰ mosquitto *mosq)
{
	ਅੱਖਰ ਪਤਾ[1024];

	ਜੇ(!net_socket_get_ਪਤਾ(sock, ਪਤਾ, 1024, &mosq->remote_ਪੋਰਟ)){
		mosq->ਪਤਾ = mosquitto_strdup(ਪਤਾ);
	}
}


ਸਥਿਰ ਪੂਰਨ callback_mqtt(
		ਬਣਤਰ lws *wsi,
		ਗਿਣਤੀ lws_callback_reasons reason,
		ਖਾਲੀ *user,
		ਖਾਲੀ *ਵਿੱਚ,
		size_t len)
{
	ਬਣਤਰ mosquitto *mosq = ਨਿਰਾ;
	ਬਣਤਰ mosquitto__packet *packet;
	ਪੂਰਨ ਗਿਣਤੀ_2;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ucount;
	ਅਚਲ ਬਣਤਰ lws_protocols *p;
	ਬਣਤਰ libws_mqtt_ਅੰਕੜੇ *u = (ਬਣਤਰ libws_mqtt_ਅੰਕੜੇ *)user;
	size_t pos;
	uint8_t *buf;
	ਪੂਰਨ rc;
	uint8_t byte;
	ਅੱਖਰ ip_addr_buff[1024];

	ਚੋਣ(reason){
		ਹਾਲਤ LWS_CALLBACK_ESTABLISHED:
			mosq = context__init();
			ਜੇ(mosq){
				p = lws_get_protocol(wsi);
				mosq->listener = p->user;
				ਜੇ(!mosq->listener){
					mosquitto_FREE(mosq);
					ਵਾਪਸ -1;
				}
				mosq->wsi = wsi;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
				ਜੇ(ਵਿੱਚ){
					mosq->ssl = (SSL *)ਵਿੱਚ;
					ਜੇ(!mosq->listener->ssl_ctx){
						mosq->listener->ssl_ctx = SSL_get_SSL_CTX(mosq->ssl);
					}
				}
#ਜੇ_ਅੰਤ
				u->mosq = mosq;
			}ਨਹੀਂਤਾਂ{
				ਵਾਪਸ -1;
			}

			easy_ਪਤਾ(lws_get_socket_fd(wsi), mosq);
			ਜੇ(!mosq->ਪਤਾ){
				/* getpeername and inet_ntop failed and not a bridge */
				mosquitto_FREE(mosq);
				u->mosq = ਨਿਰਾ;
				ਵਾਪਸ -1;
			}
			mosq->listener->client_ਗਿਣਤੀ++;
			ਜੇ((mosq->listener->max_connections > 0 && mosq->listener->client_ਗਿਣਤੀ > mosq->listener->max_connections)
					|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)db.config->global_max_connections)){

				ਜੇ(db.config->connection_messages == true){
					log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", mosq->ਪਤਾ);
				}
				mosq->listener->client_ਗਿਣਤੀ--;
				mosquitto_FREE(mosq->ਪਤਾ);
				mosquitto_FREE(mosq);
				u->mosq = ਨਿਰਾ;
				ਵਾਪਸ -1;
			}
			mosq->sock = lws_get_socket_fd(wsi);
			HASH_ਜੋੜੋ(hh_sock, db.contexts_by_sock, sock, ਆਕਾਰ(mosq->sock), mosq);
			mux__new(mosq);
			ਰੋਕੋ;

		ਹਾਲਤ LWS_CALLBACK_CLOSED:
			ਜੇ(!u){
				ਵਾਪਸ -1;
			}
			mosq = u->mosq;
			ਜੇ(mosq){
				ਜੇ(mosq->sock != INVALID_SOCKET){
					HASH_DELETE(hh_sock, db.contexts_by_sock, mosq);
					mosq->sock = INVALID_SOCKET;
					mux__delete(mosq);
				}
				mosq->wsi = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
				mosq->ssl = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
				do_disconnect(mosq, MOSQ_ERR_CONN_ਗੁਆਚਾ);
			}
			ਰੋਕੋ;

		ਹਾਲਤ LWS_CALLBACK_SERVER_WRITEABLE:
			ਜੇ(!u){
				ਵਾਪਸ -1;
			}
			mosq = u->mosq;
			ਜੇ(!mosq){
				ਵਾਪਸ -1;
			}

			rc = db_ਸੁਨੇਹਾ_write_inflight_ਬਾਹਰਲਾ_latest(mosq);
			ਜੇ(rc){
				ਵਾਪਸ -1;
			}
			rc = db_ਸੁਨੇਹਾ_write_queued_ਬਾਹਰਲਾ(mosq);
			ਜੇ(rc){
				ਵਾਪਸ -1;
			}

			ਜਦੋਂ(mosq->ਬਾਹਰਲਾ_packet && !lws_send_pipe_choked(mosq->wsi)){
				packet = mosq->ਬਾਹਰਲਾ_packet;
				ਗਿਣਤੀ_2 = lws_write(wsi, &packet->payload[packet->pos], packet->ਮੰਜ਼ਿਲ_process, LWS_WRITE_ਦੁਆਧਾਰੀ);
				ਜੇ(ਗਿਣਤੀ_2 < 0){
					ਜੇ(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						ਵਾਪਸ -1;
					}
					ਵਾਪਸ 0;
				}
				ucount = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)ਗਿਣਤੀ_2;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_SYS_ਰੁੱਖ
				metrics__int_inc(mosq_counter_ਬਾਈਟਾਂ_ਭੇਜਿਆ, ucount);
#ਜੇ_ਅੰਤ
				packet->ਮੰਜ਼ਿਲ_process -= ucount;
				packet->pos += ucount;
				ਜੇ(packet->ਮੰਜ਼ਿਲ_process > 0){
					ਜੇ(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						ਵਾਪਸ -1;
					}
					ਰੋਕੋ;
				}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_SYS_ਰੁੱਖ
				metrics__int_inc(mosq_counter_messages_ਭੇਜਿਆ, 1);
				ਜੇ(((packet->command)&0xF0) == CMD_PUBLISH){
					metrics__int_inc(mosq_counter_mqtt_publish_ਭੇਜਿਆ, 1);
				}
#ਜੇ_ਅੰਤ

				packet_get_ਅਗਲਾ_ਬਾਹਰਲਾ(mosq);
				mosquitto_FREE(packet);

				mosq->ਅਗਲਾ_ਸੁਨੇਹਾ_ਬਾਹਰਲਾ = db.now_s + mosq->keepalive;
			}
			ਜੇ(mosq->state == mosq_cs_disconnect_ws
					|| mosq->state == mosq_cs_disconnecting
					|| mosq->state == mosq_cs_disused){

				ਵਾਪਸ -1;
			}
			ਜੇ(mosq->ਬਾਹਰਲਾ_packet){
				lws_callback_on_writable(mosq->wsi);
			}
			ਰੋਕੋ;

		ਹਾਲਤ LWS_CALLBACK_RECEIVE:
			ਜੇ(!u || !u->mosq){
				ਵਾਪਸ -1;
			}
			mosq = u->mosq;
			pos = 0;
			buf = (uint8_t *)ਵਿੱਚ;
			metrics__int_inc(mosq_counter_ਬਾਈਟਾਂ_received, (int64_t)len);
			ਜਦੋਂ(pos < len){
				ਜੇ(!mosq->ਵਿੱਚ_packet.command){
					mosq->ਵਿੱਚ_packet.command = buf[pos];
					pos++;
					/* Clients must send CONNECT as their first command. */
					ਜੇ(mosq->state == mosq_cs_new && (mosq->ਵਿੱਚ_packet.command&0xF0) != CMD_CONNECT){
						ਵਾਪਸ -1;
					}
				}
				ਜੇ(mosq->ਵਿੱਚ_packet.ਬਚਿਆ_ਗਿਣਤੀ <= 0){
					ਕਰੋ{
						ਜੇ(pos == len){
							ਵਾਪਸ 0;
						}
						byte = buf[pos];
						pos++;

						mosq->ਵਿੱਚ_packet.ਬਚਿਆ_ਗਿਣਤੀ--;
						/* Max 4 bytes length for remaining length as defined by protocol.
						* Anything more likely means a broken/malicious client.
						*/
						ਜੇ(mosq->ਵਿੱਚ_packet.ਬਚਿਆ_ਗਿਣਤੀ < -4){
							ਵਾਪਸ -1;
						}

						mosq->ਵਿੱਚ_packet.ਬਚਿਆ_ਲੰਮਾਈ += (byte & 127) * mosq->ਵਿੱਚ_packet.ਬਚਿਆ_mult;
						mosq->ਵਿੱਚ_packet.ਬਚਿਆ_mult *= 128;
					}ਜਦੋਂ((byte & 128) != 0);
					mosq->ਵਿੱਚ_packet.ਬਚਿਆ_ਗਿਣਤੀ = (int8_t)(mosq->ਵਿੱਚ_packet.ਬਚਿਆ_ਗਿਣਤੀ * -1);

					ਜੇ(mosq->ਵਿੱਚ_packet.ਬਚਿਆ_ਲੰਮਾਈ > 0){
						mosq->ਵਿੱਚ_packet.payload = mosquitto_malloc(mosq->ਵਿੱਚ_packet.ਬਚਿਆ_ਲੰਮਾਈ*ਆਕਾਰ(uint8_t));
						ਜੇ(!mosq->ਵਿੱਚ_packet.payload){
							ਵਾਪਸ -1;
						}
						mosq->ਵਿੱਚ_packet.ਮੰਜ਼ਿਲ_process = mosq->ਵਿੱਚ_packet.ਬਚਿਆ_ਲੰਮਾਈ;
					}
				}
				ਜੇ(mosq->ਵਿੱਚ_packet.ਮੰਜ਼ਿਲ_process>0){
					ਜੇ((uint32_t)len - pos >= mosq->ਵਿੱਚ_packet.ਮੰਜ਼ਿਲ_process){
						ਸਮ੍ਰਿਤੀ_ਨਕਲ(&mosq->ਵਿੱਚ_packet.payload[mosq->ਵਿੱਚ_packet.pos], &buf[pos], mosq->ਵਿੱਚ_packet.ਮੰਜ਼ਿਲ_process);
						mosq->ਵਿੱਚ_packet.pos += mosq->ਵਿੱਚ_packet.ਮੰਜ਼ਿਲ_process;
						pos += mosq->ਵਿੱਚ_packet.ਮੰਜ਼ਿਲ_process;
						mosq->ਵਿੱਚ_packet.ਮੰਜ਼ਿਲ_process = 0;
					}ਨਹੀਂਤਾਂ{
						ਸਮ੍ਰਿਤੀ_ਨਕਲ(&mosq->ਵਿੱਚ_packet.payload[mosq->ਵਿੱਚ_packet.pos], &buf[pos], len-pos);
						mosq->ਵਿੱਚ_packet.pos += (uint32_t)(len-pos);
						mosq->ਵਿੱਚ_packet.ਮੰਜ਼ਿਲ_process -= (uint32_t)(len-pos);
						ਵਾਪਸ 0;
					}
				}
				/* All data for this packet is read. */
				mosq->ਵਿੱਚ_packet.pos = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_SYS_ਰੁੱਖ
				metrics__int_inc(mosq_counter_messages_received, 1);
#ਜੇ_ਅੰਤ
				rc = handle__packet(mosq);

				/* Free data and reset values */
				packet__cleanup(&mosq->ਵਿੱਚ_packet);

				keepalive__update(mosq);

				ਜੇ(rc && mosq->ਬਾਹਰਲਾ_packet){
					ਜੇ(mosq->state != mosq_cs_disconnecting){
						mosquitto_ਸੈੱਟ_state(mosq, mosq_cs_disconnect_ws);
					}
					lws_callback_on_writable(mosq->wsi);
				}ਨਹੀਂਤਾਂ ਜੇ(rc){
					do_disconnect(mosq, MOSQ_ERR_CONN_ਗੁਆਚਾ);
					ਵਾਪਸ -1;
				}
			}
			ਰੋਕੋ;

		ਮੂਲ:
			ਰੋਕੋ;
	}

	ਵਾਪਸ 0;
}


ਸਥਿਰ ਅੱਖਰ *http__canonical_filename(
		ਬਣਤਰ lws *wsi,
		ਅਚਲ ਅੱਖਰ *ਵਿੱਚ,
		ਅਚਲ ਅੱਖਰ *http_dir)
{
	size_t inlen, slen;
	ਅੱਖਰ *filename, *filename_canonical;

	inlen = ਲੜੀ_ਲੰਬਾਈ(ਵਿੱਚ);
	ਜੇ(ਵਿੱਚ[inlen-1] == '/'){
		slen = ਲੜੀ_ਲੰਬਾਈ(http_dir) + inlen + ਲੜੀ_ਲੰਬਾਈ("/index.html") + 2;
	}ਨਹੀਂਤਾਂ{
		slen = ਲੜੀ_ਲੰਬਾਈ(http_dir) + inlen + 2;
	}
	filename = mosquitto_malloc(slen);
	ਜੇ(!filename){
		lws_return_http_ਸਥਿਤੀ(wsi, HTTP_ਸਥਿਤੀ_ਅੰਦਰੂਨੀ_SERVER_ERROR, ਨਿਰਾ);
		ਵਾਪਸ ਨਿਰਾ;
	}
	ਜੇ(((ਅੱਖਰ *)ਵਿੱਚ)[inlen-1] == '/'){
		snprintf(filename, slen, "%s%sindex.html", http_dir, (ਅੱਖਰ *)ਵਿੱਚ);
	}ਨਹੀਂਤਾਂ{
		snprintf(filename, slen, "%s%s", http_dir, (ਅੱਖਰ *)ਵਿੱਚ);
	}


	/* Get canonical path and check it is within our http_dir */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
	filename_canonical = _fullpath(ਨਿਰਾ, filename, 0);
	mosquitto_FREE(filename);
	ਜੇ(!filename_canonical){
		lws_return_http_ਸਥਿਤੀ(wsi, HTTP_ਸਥਿਤੀ_ਅੰਦਰੂਨੀ_SERVER_ERROR, ਨਿਰਾ);
		ਵਾਪਸ ਨਿਰਾ;
	}
#ਨਹੀਂਤਾਂ
	filename_canonical = realpath(filename, ਨਿਰਾ);
	mosquitto_FREE(filename);
	ਜੇ(!filename_canonical){
		ਜੇ(errno == EACCES){
			lws_return_http_ਸਥਿਤੀ(wsi, HTTP_ਸਥਿਤੀ_FORBIDDEN, ਨਿਰਾ);
		}ਨਹੀਂਤਾਂ ਜੇ(errno == EINVAL || errno == EIO || errno == ELOOP){
			lws_return_http_ਸਥਿਤੀ(wsi, HTTP_ਸਥਿਤੀ_ਅੰਦਰੂਨੀ_SERVER_ERROR, ਨਿਰਾ);
		}ਨਹੀਂਤਾਂ ਜੇ(errno == ENAMETOOLONG){
			lws_return_http_ਸਥਿਤੀ(wsi, HTTP_ਸਥਿਤੀ_REQ_URI_TOO_LONG, ਨਿਰਾ);
		}ਨਹੀਂਤਾਂ ਜੇ(errno == ENOENT || errno == ENOTDIR){
			lws_return_http_ਸਥਿਤੀ(wsi, HTTP_ਸਥਿਤੀ_NOT_ਮਿਲਿਆ, ਨਿਰਾ);
		}
		ਵਾਪਸ ਨਿਰਾ;
	}
#ਜੇ_ਅੰਤ
	ਜੇ(ਲੜੀ_ਤੁਲਨਾ(http_dir, filename_canonical, ਲੜੀ_ਲੰਬਾਈ(http_dir))){
		/* Requested file isn't within http_dir, deny access. */
		ਸੁਰੱਖਿਅਤ_FREE(filename_canonical);
		lws_return_http_ਸਥਿਤੀ(wsi, HTTP_ਸਥਿਤੀ_FORBIDDEN, ਨਿਰਾ);
		ਵਾਪਸ ਨਿਰਾ;
	}

	ਵਾਪਸ filename_canonical;
}


ਸਥਿਰ ਪੂਰਨ callback_http(
		ਬਣਤਰ lws *wsi,
		ਗਿਣਤੀ lws_callback_reasons reason,
		ਖਾਲੀ *user,
		ਖਾਲੀ *ਵਿੱਚ,
		size_t len)
{
	ਬਣਤਰ libws_http_ਅੰਕੜੇ *u = (ਬਣਤਰ libws_http_ਅੰਕੜੇ *)user;
	ਬਣਤਰ libws_mqtt_hack *hack;
	ਅੱਖਰ *http_dir;
	size_t buflen;
	size_t wlen;
	ਪੂਰਨ rc;
	ਅੱਖਰ *filename_canonical;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ buf[4096];
	ਬਣਤਰ stat filestat;
	ਬਣਤਰ mosquitto *mosq;
	ਬਣਤਰ lws_pollargs *pollargs = (ਬਣਤਰ lws_pollargs *)ਵਿੱਚ;
	ਪੂਰਨ hlen;

	/* FIXME - ssl cert verification is done here. */

	ਚੋਣ(reason){
		ਹਾਲਤ LWS_CALLBACK_HTTP:
			ਜੇ(!u){
				ਵਾਪਸ -1;
			}

			hack = (ਬਣਤਰ libws_mqtt_hack *)lws_context_user(lws_get_context(wsi));
			ਜੇ(!hack){
				ਵਾਪਸ -1;
			}
			http_dir = hack->http_dir;

			ਜੇ(!http_dir){
				/* http disabled */
				ਵਾਪਸ -1;
			}

			/* Forbid POST */
			ਜੇ(lws_hdr_ਕੁੱਲ_ਲੰਮਾਈ(wsi, WSI_TOKEN_POST_URI)){
				lws_return_http_ਸਥਿਤੀ(wsi, HTTP_ਸਥਿਤੀ_ਵਿਧੀ_NOT_ALLOWED, ਨਿਰਾ);
				ਵਾਪਸ -1;
			}

			filename_canonical = http__canonical_filename(wsi, (ਅੱਖਰ *)ਵਿੱਚ, http_dir);
			ਜੇ(!filename_canonical){
				ਵਾਪਸ -1;
			}

			u->fptr = fopen(filename_canonical, "rb");
			ਜੇ(!u->fptr){
				ਸੁਰੱਖਿਅਤ_FREE(filename_canonical);
				lws_return_http_ਸਥਿਤੀ(wsi, HTTP_ਸਥਿਤੀ_NOT_ਮਿਲਿਆ, ਨਿਰਾ);
				ਵਾਪਸ -1;
			}
			ਜੇ(fstat(fileno(u->fptr), &filestat) < 0){
				ਸੁਰੱਖਿਅਤ_FREE(filename_canonical);
				lws_return_http_ਸਥਿਤੀ(wsi, HTTP_ਸਥਿਤੀ_ਅੰਦਰੂਨੀ_SERVER_ERROR, ਨਿਰਾ);
				fclose(u->fptr);
				u->fptr = ਨਿਰਾ;
				ਵਾਪਸ -1;
			}


			ਜੇ((filestat.st_ਬਹੁਲਕ & S_IFDIR) == S_IFDIR){
				fclose(u->fptr);
				u->fptr = ਨਿਰਾ;
				ਸੁਰੱਖਿਅਤ_FREE(filename_canonical);

				/* FIXME - use header functions from lws 2.x */
				buflen = (size_t)snprintf((ਅੱਖਰ *)buf, 4096, "HTTP/1.0 302 OK\r\n"
						"Location: %s/\r\n\r\n",
						(ਅੱਖਰ *)ਵਿੱਚ);
				ਵਾਪਸ lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
			}

			ਜੇ((filestat.st_ਬਹੁਲਕ & S_IFREG) != S_IFREG){
				lws_return_http_ਸਥਿਤੀ(wsi, HTTP_ਸਥਿਤੀ_FORBIDDEN, ਨਿਰਾ);
				fclose(u->fptr);
				u->fptr = ਨਿਰਾ;
				ਸੁਰੱਖਿਅਤ_FREE(filename_canonical);
				ਵਾਪਸ -1;
			}

			log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG, "http serving file \"%s\".", filename_canonical);
			ਸੁਰੱਖਿਅਤ_FREE(filename_canonical);
			/* FIXME - use header functions from lws 2.x */
			buflen = (size_t)snprintf((ਅੱਖਰ *)buf, 4096, "HTTP/1.0 200 OK\r\n"
					"Server: mosquitto\r\n"
					"Content-Length: %u\r\n\r\n",
					(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)filestat.st_ਮਾਪ);
			ਜੇ(lws_write(wsi, buf, buflen, LWS_WRITE_HTTP) < 0){
				fclose(u->fptr);
				u->fptr = ਨਿਰਾ;
				ਵਾਪਸ -1;
			}
			lws_callback_on_writable(wsi);
			ਰੋਕੋ;

		ਹਾਲਤ LWS_CALLBACK_HTTP_BODY:
			/* For extra POST data? */
			ਵਾਪਸ -1;

		ਹਾਲਤ LWS_CALLBACK_HTTP_BODY_COMPLETION:
			/* For end of extra POST data? */
			ਵਾਪਸ -1;

		ਹਾਲਤ LWS_CALLBACK_FILTER_HTTP_CONNECTION:
			/* Access control here */
			ਵਾਪਸ 0;

#ਜੇ LWS_LIBRARY_ਸੰਸਕਰਣ_ਸੰਖਿਆ >= 3001000
		ਹਾਲਤ LWS_CALLBACK_HTTP_CONFIRM_UPGRADE:
			hack = lws_context_user(lws_get_context(wsi));
			ਜੇ(hack->listener->ws_origin_ਗਿਣਤੀ){
				hlen = lws_hdr_ਕੁੱਲ_ਲੰਮਾਈ(wsi, WSI_TOKEN_ORIGIN);
				ਜੇ(hlen <= 0 || hlen >= (ਪੂਰਨ)ਆਕਾਰ(buf)){
					ਵਾਪਸ -1;
				}
				ਲਈ(ਪੂਰਨ i=0; i<hack->listener->ws_origin_ਗਿਣਤੀ; i++){
					lws_hdr_copy(wsi, (ਅੱਖਰ *)buf, ਆਕਾਰ(buf), WSI_TOKEN_ORIGIN);
					ਜੇ((!strcmp(hack->listener->ws_origins[i], (ਅੱਖਰ *)buf))){
						ਵਾਪਸ 0;
					}
				}
				ਵਾਪਸ -1;
			}ਨਹੀਂਤਾਂ{
				ਵਾਪਸ 0;
			}
#ਜੇ_ਅੰਤ

		ਹਾਲਤ LWS_CALLBACK_HTTP_WRITEABLE:
			/* Send our data here */
			ਜੇ(u && u->fptr){
				ਕਰੋ{
					buflen = fread(buf, 1, ਆਕਾਰ(buf), u->fptr);
					ਜੇ(buflen < 1){
						fclose(u->fptr);
						u->fptr = ਨਿਰਾ;
						ਵਾਪਸ -1;
					}
					rc = lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
					ਜੇ(rc < 0){
						ਵਾਪਸ -1;
					}
					wlen = (size_t)rc;
					/* while still active, extend timeout */
					ਜੇ(wlen){
						lws_ਸੈੱਟ_timeout(wsi, PENDING_TIMEOUT_HTTP_CONTENT, 10);
					}
					ਜੇ(wlen < buflen){
						ਜੇ(fseek(u->fptr, (ਲੰਮਾ)(buflen-wlen), SEEK_CUR) < 0){
							fclose(u->fptr);
							u->fptr = ਨਿਰਾ;
							ਵਾਪਸ -1;
						}
					}ਨਹੀਂਤਾਂ{
						ਜੇ(buflen < ਆਕਾਰ(buf)){
							fclose(u->fptr);
							u->fptr = ਨਿਰਾ;
						}
					}
				}ਜਦੋਂ(u->fptr && !lws_send_pipe_choked(wsi));
				lws_callback_on_writable(wsi);
			}ਨਹੀਂਤਾਂ{
				ਵਾਪਸ -1;
			}
			ਰੋਕੋ;

		ਹਾਲਤ LWS_CALLBACK_CLOSED:
		ਹਾਲਤ LWS_CALLBACK_CLOSED_HTTP:
		ਹਾਲਤ LWS_CALLBACK_HTTP_FILE_COMPLETION:
			ਜੇ(u && u->fptr){
				fclose(u->fptr);
				u->fptr = ਨਿਰਾ;
			}
			ਰੋਕੋ;

		ਹਾਲਤ LWS_CALLBACK_ਜੋੜੋ_POLL_FD:
			HASH_ਲੱਭੋ(hh_sock, db.contexts_by_sock, &pollargs->fd, ਆਕਾਰ(pollargs->fd), mosq);
			ਜੇ(mosq){
				ਜੇ(pollargs->events & LWS_POLLOUT){
					mux_ਜੋੜੋ_ਬਾਹਰਲਾ(mosq);
				}ਨਹੀਂਤਾਂ{
					mux_remove_ਬਾਹਰਲਾ(mosq);
				}
			}ਨਹੀਂਤਾਂ{
				ਜੇ(pollargs->events & POLLIN){
					/* Assume this is a new listener */
					listeners_ਜੋੜੋ_websockets(lws_get_context(wsi), pollargs->fd);
				}
			}
			ਰੋਕੋ;

		ਹਾਲਤ LWS_CALLBACK_DEL_POLL_FD:
			HASH_ਲੱਭੋ(hh_sock, db.contexts_by_sock, &pollargs->fd, ਆਕਾਰ(pollargs->fd), mosq);
			ਜੇ(mosq){
				mux__delete(mosq);
			}
			ਰੋਕੋ;

		ਹਾਲਤ LWS_CALLBACK_CHANGE_ਬਹੁਲਕ_POLL_FD:
			HASH_ਲੱਭੋ(hh_sock, db.contexts_by_sock, &pollargs->fd, ਆਕਾਰ(pollargs->fd), mosq);
			ਜੇ(mosq){
				ਜੇ(pollargs->events & LWS_POLLHUP){
					ਵਾਪਸ 1;
				}ਨਹੀਂਤਾਂ ਜੇ(pollargs->events & LWS_POLLOUT){
					mux_ਜੋੜੋ_ਬਾਹਰਲਾ(mosq);
				}ਨਹੀਂਤਾਂ{
					mux_remove_ਬਾਹਰਲਾ(mosq);
				}
			}
			ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
		ਹਾਲਤ LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION:
			ਜੇ(!len || (SSL_get_verify_ਨਤੀਜਾ((SSL *)ਵਿੱਚ) != X509_V_OK)){
				ਵਾਪਸ 1;
			}
			ਰੋਕੋ;
#ਜੇ_ਅੰਤ

		ਮੂਲ:
			ਵਾਪਸ 0;
	}

	ਵਾਪਸ 0;
}


ਸਥਿਰ ਖਾਲੀ log_wrap(ਪੂਰਨ level, ਅਚਲ ਅੱਖਰ *ਸਤਰ)
{
	ਅੱਖਰ *l = (ਅੱਖਰ *)ਸਤਰ;
	ਅਣਵਰਤਿਆ(level);
	l[ਲੜੀ_ਲੰਬਾਈ(ਸਤਰ)-1] = '\0'; /* Remove \n */
	log__printf(ਨਿਰਾ, MOSQ_LOG_WEBSOCKETS, "%s", l);
}


ਖਾਲੀ mosq_websockets_init(ਬਣਤਰ mosquitto__listener *listener, ਅਚਲ ਬਣਤਰ mosquitto__config *conf)
{
	ਬਣਤਰ lws_context_creation_info info;
	ਬਣਤਰ lws_protocols *p;
	size_t protocol_ਗਿਣਤੀ;
	ਪੂਰਨ i;
	ਬਣਤਰ libws_mqtt_hack *user;

	/* Count valid protocols */
	ਲਈ(protocol_ਗਿਣਤੀ=0; protocols[protocol_ਗਿਣਤੀ].name; protocol_ਗਿਣਤੀ++){
		;
	}

	p = mosquitto_calloc(protocol_ਗਿਣਤੀ+1, ਆਕਾਰ(ਬਣਤਰ lws_protocols));
	ਜੇ(!p){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Out of memory.");
		ਵਾਪਸ;
	}
	ਲਈ(i=0; protocols[i].name; i++){
		p[i].name = protocols[i].name;
		p[i].callback = protocols[i].callback;
		p[i].ਪ੍ਰਤੀ_session_ਅੰਕੜੇ_ਮਾਪ = protocols[i].ਪ੍ਰਤੀ_session_ਅੰਕੜੇ_ਮਾਪ;
		p[i].rx_ਬਫਰ_ਮਾਪ = protocols[i].rx_ਬਫਰ_ਮਾਪ;
		p[i].user = listener;
	}

	memset(&info, 0, ਆਕਾਰ(info));
	info.iface = listener->host;
	info.ਪੋਰਟ = listener->ਪੋਰਟ;
	info.protocols = p;
	info.gid = -1;
	info.uid = -1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
	ਜੇ(listener->cafile){
		info.ssl_ca_filepath = listener->cafile;
	}ਨਹੀਂਤਾਂ ਜੇ(listener->capath){
		log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: CA path option is not supported for websockets");
	}
	info.ssl_cert_filepath = listener->certfile;
	info.ssl_ਨਿੱਜੀ_ਕੁੰਜੀ_filepath = listener->keyfile;
	info.ssl_cipher_list = listener->ciphers;
	/* HTTP 1 only, due to HTTP 2 issues in Firefox:
	   https://github.com/eclipse-mosquitto/mosquitto/issues/1211
	*/
	info.alpn = "h1";
#ਜੇ defined(WITH_WEBSOCKETS) && LWS_LIBRARY_ਸੰਸਕਰਣ_ਸੰਖਿਆ>=3001000
	info.tls1_3_plus_cipher_list = listener->ciphers_tls13;
#ਜੇ_ਅੰਤ
	ਜੇ(listener->require_certificate){
		info.options |= LWS_SERVER_OPTION_REQUIRE_VALID_OPENSSL_CLIENT_CERT;
	}
#ਜੇ_ਅੰਤ

	info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
	ਜੇ(listener->socket_domain == AF_INET){
		info.options |= LWS_SERVER_OPTION_DISABLE_IPV6;
	}
	info.max_http_ਸਿਰਲੇਖ_ਅੰਕੜੇ = conf->packet_ਬਫਰ_ਮਾਪ;

	user = mosquitto_calloc(1, ਆਕਾਰ(ਬਣਤਰ libws_mqtt_hack));
	ਜੇ(!user){
		mosquitto_FREE(p);
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Out of memory.");
		ਵਾਪਸ;
	}

	ਜੇ(listener->http_dir){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
		user->http_dir = _fullpath(ਨਿਰਾ, listener->http_dir, 0);
#ਨਹੀਂਤਾਂ
		user->http_dir = realpath(listener->http_dir, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
		ਜੇ(!user->http_dir){
			mosquitto_FREE(user);
			mosquitto_FREE(p);
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to open http dir \"%s\".", listener->http_dir);
			ਵਾਪਸ;
		}
	}
	user->listener = listener;

	info.user = user;
	info.pt_serv_buf_ਮਾਪ = WS_SERV_BUF_ਮਾਪ;
	listener->ws_protocol = p;

	lws_ਸੈੱਟ_log_level(conf->websockets_log_level, log_wrap);

	log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Opening websockets listen socket on port %d.", listener->ਪੋਰਟ);
	listener->ws_ਵਿੱਚ_init = true;
	listener->ws_context = lws_create_context(&info);
	listener->ws_ਵਿੱਚ_init = false;
}

#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ
