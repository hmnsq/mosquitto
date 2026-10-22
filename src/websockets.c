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

#اگر_تعریف_شدہ WITH_WEBSOCKETS

#شامل "config.h"

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#اگر_ختم
#شامل "mosquitto_internal.h"
#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "packet_mosq.h"
#شامل "sys_tree.h"
#شامل "util_mosq.h"

#شامل <stdlib.h>
#شامل <errno.h>
#شامل <sys/stat.h>

#اگر_غیر_تعریف_شدہ WIN32
#  include <sys/socket.h>
#اگر_ختم

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
/* Be careful if changing these, if TX is not bigger than SERV then there can
 * be very large write performance penalties.
 */
#تعریف_کرو WS_SERV_BUF_سائز 4096
#تعریف_کرو WS_TX_BUF_سائز (WS_SERV_BUF_سائز*2)

ساکن صحیح callback_mqtt(
		ساخت lws *wsi,
		شمار lws_callback_reasons reason,
		خالی *user,
		خالی *اندر,
		size_t len);

ساکن صحیح callback_http(
		ساخت lws *wsi,
		شمار lws_callback_reasons reason,
		خالی *user,
		خالی *اندر,
		size_t len);

شمار mosq_ws_protocols {
	PROTOCOL_HTTP = 0,
	PROTOCOL_MQTT,
	DEMO_PROTOCOL_گنتی,
};

ساخت libws_http_ڈیٹا {
	FILE *fptr;
};

ساکن ساخت lws_protocols protocols[] = {
	/* first protocol must always be HTTP handler */
	{
		"http-only",                        /* name */
		callback_http,                      /* lws_callback_function */
		حجم (ساخت libws_http_ڈیٹا),    /* per_session_data_size */
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		تہی,                               /* user v1.4 on */
		WS_TX_BUF_سائز                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqtt",
		callback_mqtt,
		حجم(ساخت libws_mqtt_ڈیٹا),
		0,                                  /* rx_buffer_size */
		1,                                  /* id */
		تہی,                               /* user v1.4 on */
		WS_TX_BUF_سائز                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqttv3.1",
		callback_mqtt,
		حجم(ساخت libws_mqtt_ڈیٹا),
		0,                                  /* rx_buffer_size */
		2,                                  /* id */
		تہی,                               /* user v1.4 on */
		WS_TX_BUF_سائز                      /* tx_packet_size v2.3.0 */
	},
	{
		تہی,
		تہی,
		0,
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		تہی,                               /* user v1.4 on */
		0                                   /* tx_packet_size v2.3.0 */
	}
};


ساکن خالی easy_پتہ(صحیح sock, ساخت mosquitto *mosq)
{
	حرف پتہ[1024];

	اگر(!net_socket_get_پتہ(sock, پتہ, 1024, &mosq->remote_پورٹ)){
		mosq->پتہ = mosquitto_strdup(پتہ);
	}
}


ساکن صحیح callback_mqtt(
		ساخت lws *wsi,
		شمار lws_callback_reasons reason,
		خالی *user,
		خالی *اندر,
		size_t len)
{
	ساخت mosquitto *mosq = تہی;
	ساخت mosquitto__packet *packet;
	صحیح گنتی;
	غیرعلامتی صحیح ucount;
	ثابت ساخت lws_protocols *p;
	ساخت libws_mqtt_ڈیٹا *u = (ساخت libws_mqtt_ڈیٹا *)user;
	size_t pos;
	uint8_t *buf;
	صحیح rc;
	uint8_t byte;
	حرف ip_addr_buff[1024];

	انتخاب(reason){
		حالت LWS_CALLBACK_ESTABLISHED:
			mosq = context__init();
			اگر(mosq){
				p = lws_get_protocol(wsi);
				mosq->listener = p->user;
				اگر(!mosq->listener){
					mosquitto_FREE(mosq);
					واپس -1;
				}
				mosq->wsi = wsi;
#اگر_تعریف_شدہ WITH_TLS
				اگر(اندر){
					mosq->ssl = (SSL *)اندر;
					اگر(!mosq->listener->ssl_ctx){
						mosq->listener->ssl_ctx = SSL_get_SSL_CTX(mosq->ssl);
					}
				}
#اگر_ختم
				u->mosq = mosq;
			}ورنہ{
				واپس -1;
			}

			easy_پتہ(lws_get_socket_fd(wsi), mosq);
			اگر(!mosq->پتہ){
				/* getpeername and inet_ntop failed and not a bridge */
				mosquitto_FREE(mosq);
				u->mosq = تہی;
				واپس -1;
			}
			mosq->listener->client_گنتی++;
			اگر((mosq->listener->max_connections > 0 && mosq->listener->client_گنتی > mosq->listener->max_connections)
					|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (غیرعلامتی صحیح)db.config->global_max_connections)){

				اگر(db.config->connection_messages == true){
					log__printf(تہی, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", mosq->پتہ);
				}
				mosq->listener->client_گنتی--;
				mosquitto_FREE(mosq->پتہ);
				mosquitto_FREE(mosq);
				u->mosq = تہی;
				واپس -1;
			}
			mosq->sock = lws_get_socket_fd(wsi);
			HASH_جوڑو(hh_sock, db.contexts_by_sock, sock, حجم(mosq->sock), mosq);
			mux__new(mosq);
			توڑو;

		حالت LWS_CALLBACK_CLOSED:
			اگر(!u){
				واپس -1;
			}
			mosq = u->mosq;
			اگر(mosq){
				اگر(mosq->sock != INVALID_SOCKET){
					HASH_DELETE(hh_sock, db.contexts_by_sock, mosq);
					mosq->sock = INVALID_SOCKET;
					mux__delete(mosq);
				}
				mosq->wsi = تہی;
#اگر_تعریف_شدہ WITH_TLS
				mosq->ssl = تہی;
#اگر_ختم
				do_disconnect(mosq, MOSQ_ERR_CONN_گمشدہ);
			}
			توڑو;

		حالت LWS_CALLBACK_SERVER_WRITEABLE:
			اگر(!u){
				واپس -1;
			}
			mosq = u->mosq;
			اگر(!mosq){
				واپس -1;
			}

			rc = db_پیغام_write_inflight_برآمد_latest(mosq);
			اگر(rc){
				واپس -1;
			}
			rc = db_پیغام_write_queued_برآمد(mosq);
			اگر(rc){
				واپس -1;
			}

			جب(mosq->برآمد_packet && !lws_send_pipe_choked(mosq->wsi)){
				packet = mosq->برآمد_packet;
				گنتی = lws_write(wsi, &packet->payload[packet->pos], packet->مقصد_process, LWS_WRITE_ثنائی);
				اگر(گنتی < 0){
					اگر(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						واپس -1;
					}
					واپس 0;
				}
				ucount = (غیرعلامتی صحیح)گنتی;
#اگر_تعریف_شدہ WITH_SYS_درخت
				metrics__int_inc(mosq_شمار_بائٹیں_بھیجا, ucount);
#اگر_ختم
				packet->مقصد_process -= ucount;
				packet->pos += ucount;
				اگر(packet->مقصد_process > 0){
					اگر(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						واپس -1;
					}
					توڑو;
				}

#اگر_تعریف_شدہ WITH_SYS_درخت
				metrics__int_inc(mosq_شمار_messages_بھیجا, 1);
				اگر(((packet->command)&0xF0) == CMD_PUBLISH){
					metrics__int_inc(mosq_شمار_mqtt_publish_بھیجا, 1);
				}
#اگر_ختم

				packet_get_اگلا_برآمد(mosq);
				mosquitto_FREE(packet);

				mosq->اگلا_پیغام_برآمد = db.now_s + mosq->keepalive;
			}
			اگر(mosq->state == mosq_cs_disconnect_ws
					|| mosq->state == mosq_cs_disconnecting
					|| mosq->state == mosq_cs_disused){

				واپس -1;
			}
			اگر(mosq->برآمد_packet){
				lws_callback_on_writable(mosq->wsi);
			}
			توڑو;

		حالت LWS_CALLBACK_RECEIVE:
			اگر(!u || !u->mosq){
				واپس -1;
			}
			mosq = u->mosq;
			pos = 0;
			buf = (uint8_t *)اندر;
			metrics__int_inc(mosq_شمار_بائٹیں_received, (int64_t)len);
			جب(pos < len){
				اگر(!mosq->اندر_packet.command){
					mosq->اندر_packet.command = buf[pos];
					pos++;
					/* Clients must send CONNECT as their first command. */
					اگر(mosq->state == mosq_cs_new && (mosq->اندر_packet.command&0xF0) != CMD_CONNECT){
						واپس -1;
					}
				}
				اگر(mosq->اندر_packet.بقایا_گنتی <= 0){
					کرو{
						اگر(pos == len){
							واپس 0;
						}
						byte = buf[pos];
						pos++;

						mosq->اندر_packet.بقایا_گنتی--;
						/* Max 4 bytes length for remaining length as defined by protocol.
						* Anything more likely means a broken/malicious client.
						*/
						اگر(mosq->اندر_packet.بقایا_گنتی < -4){
							واپس -1;
						}

						mosq->اندر_packet.بقایا_لمبائی += (byte & 127) * mosq->اندر_packet.بقایا_mult;
						mosq->اندر_packet.بقایا_mult *= 128;
					}جب((byte & 128) != 0);
					mosq->اندر_packet.بقایا_گنتی = (int8_t)(mosq->اندر_packet.بقایا_گنتی * -1);

					اگر(mosq->اندر_packet.بقایا_لمبائی > 0){
						mosq->اندر_packet.payload = mosquitto_malloc(mosq->اندر_packet.بقایا_لمبائی*حجم(uint8_t));
						اگر(!mosq->اندر_packet.payload){
							واپس -1;
						}
						mosq->اندر_packet.مقصد_process = mosq->اندر_packet.بقایا_لمبائی;
					}
				}
				اگر(mosq->اندر_packet.مقصد_process>0){
					اگر((uint32_t)len - pos >= mosq->اندر_packet.مقصد_process){
						حافظہ_نقل(&mosq->اندر_packet.payload[mosq->اندر_packet.pos], &buf[pos], mosq->اندر_packet.مقصد_process);
						mosq->اندر_packet.pos += mosq->اندر_packet.مقصد_process;
						pos += mosq->اندر_packet.مقصد_process;
						mosq->اندر_packet.مقصد_process = 0;
					}ورنہ{
						حافظہ_نقل(&mosq->اندر_packet.payload[mosq->اندر_packet.pos], &buf[pos], len-pos);
						mosq->اندر_packet.pos += (uint32_t)(len-pos);
						mosq->اندر_packet.مقصد_process -= (uint32_t)(len-pos);
						واپس 0;
					}
				}
				/* All data for this packet is read. */
				mosq->اندر_packet.pos = 0;

#اگر_تعریف_شدہ WITH_SYS_درخت
				metrics__int_inc(mosq_شمار_messages_received, 1);
#اگر_ختم
				rc = handle__packet(mosq);

				/* Free data and reset values */
				packet__cleanup(&mosq->اندر_packet);

				keepalive__update(mosq);

				اگر(rc && mosq->برآمد_packet){
					اگر(mosq->state != mosq_cs_disconnecting){
						mosquitto_سیٹ_state(mosq, mosq_cs_disconnect_ws);
					}
					lws_callback_on_writable(mosq->wsi);
				}ورنہ اگر(rc){
					do_disconnect(mosq, MOSQ_ERR_CONN_گمشدہ);
					واپس -1;
				}
			}
			توڑو;

		طےشدہ:
			توڑو;
	}

	واپس 0;
}


ساکن حرف *http__canonical_filename(
		ساخت lws *wsi,
		ثابت حرف *اندر,
		ثابت حرف *http_dir)
{
	size_t inlen, slen;
	حرف *filename, *filename_canonical;

	inlen = لڑی_لمبائی(اندر);
	اگر(اندر[inlen-1] == '/'){
		slen = لڑی_لمبائی(http_dir) + inlen + لڑی_لمبائی("/index.html") + 2;
	}ورنہ{
		slen = لڑی_لمبائی(http_dir) + inlen + 2;
	}
	filename = mosquitto_malloc(slen);
	اگر(!filename){
		lws_return_http_حالت(wsi, HTTP_حالت_اندرونی_SERVER_ERROR, تہی);
		واپس تہی;
	}
	اگر(((حرف *)اندر)[inlen-1] == '/'){
		snprintf(filename, slen, "%s%sindex.html", http_dir, (حرف *)اندر);
	}ورنہ{
		snprintf(filename, slen, "%s%s", http_dir, (حرف *)اندر);
	}


	/* Get canonical path and check it is within our http_dir */
#اگر_تعریف_شدہ WIN32
	filename_canonical = _fullpath(تہی, filename, 0);
	mosquitto_FREE(filename);
	اگر(!filename_canonical){
		lws_return_http_حالت(wsi, HTTP_حالت_اندرونی_SERVER_ERROR, تہی);
		واپس تہی;
	}
#ورنہ
	filename_canonical = realpath(filename, تہی);
	mosquitto_FREE(filename);
	اگر(!filename_canonical){
		اگر(errno == EACCES){
			lws_return_http_حالت(wsi, HTTP_حالت_FORBIDDEN, تہی);
		}ورنہ اگر(errno == EINVAL || errno == EIO || errno == ELOOP){
			lws_return_http_حالت(wsi, HTTP_حالت_اندرونی_SERVER_ERROR, تہی);
		}ورنہ اگر(errno == ENAMETOOLONG){
			lws_return_http_حالت(wsi, HTTP_حالت_REQ_URI_TOO_LONG, تہی);
		}ورنہ اگر(errno == ENOENT || errno == ENOTDIR){
			lws_return_http_حالت(wsi, HTTP_حالت_NOT_ملا, تہی);
		}
		واپس تہی;
	}
#اگر_ختم
	اگر(لڑی_موازنہ(http_dir, filename_canonical, لڑی_لمبائی(http_dir))){
		/* Requested file isn't within http_dir, deny access. */
		محفوظ_FREE(filename_canonical);
		lws_return_http_حالت(wsi, HTTP_حالت_FORBIDDEN, تہی);
		واپس تہی;
	}

	واپس filename_canonical;
}


ساکن صحیح callback_http(
		ساخت lws *wsi,
		شمار lws_callback_reasons reason,
		خالی *user,
		خالی *اندر,
		size_t len)
{
	ساخت libws_http_ڈیٹا *u = (ساخت libws_http_ڈیٹا *)user;
	ساخت libws_mqtt_hack *hack;
	حرف *http_dir;
	size_t buflen;
	size_t wlen;
	صحیح rc;
	حرف *filename_canonical;
	غیرعلامتی حرف buf[4096];
	ساخت stat filestat;
	ساخت mosquitto *mosq;
	ساخت lws_pollargs *pollargs = (ساخت lws_pollargs *)اندر;
	صحیح hlen;

	/* FIXME - ssl cert verification is done here. */

	انتخاب(reason){
		حالت LWS_CALLBACK_HTTP:
			اگر(!u){
				واپس -1;
			}

			hack = (ساخت libws_mqtt_hack *)lws_context_user(lws_get_context(wsi));
			اگر(!hack){
				واپس -1;
			}
			http_dir = hack->http_dir;

			اگر(!http_dir){
				/* http disabled */
				واپس -1;
			}

			/* Forbid POST */
			اگر(lws_hdr_کل_لمبائی(wsi, WSI_TOKEN_POST_URI)){
				lws_return_http_حالت(wsi, HTTP_حالت_طریقہ_NOT_ALLOWED, تہی);
				واپس -1;
			}

			filename_canonical = http__canonical_filename(wsi, (حرف *)اندر, http_dir);
			اگر(!filename_canonical){
				واپس -1;
			}

			u->fptr = fopen(filename_canonical, "rb");
			اگر(!u->fptr){
				محفوظ_FREE(filename_canonical);
				lws_return_http_حالت(wsi, HTTP_حالت_NOT_ملا, تہی);
				واپس -1;
			}
			اگر(fstat(fileno(u->fptr), &filestat) < 0){
				محفوظ_FREE(filename_canonical);
				lws_return_http_حالت(wsi, HTTP_حالت_اندرونی_SERVER_ERROR, تہی);
				fclose(u->fptr);
				u->fptr = تہی;
				واپس -1;
			}


			اگر((filestat.st_منوال & S_IFDIR) == S_IFDIR){
				fclose(u->fptr);
				u->fptr = تہی;
				محفوظ_FREE(filename_canonical);

				/* FIXME - use header functions from lws 2.x */
				buflen = (size_t)snprintf((حرف *)buf, 4096, "HTTP/1.0 302 OK\r\n"
						"Location: %s/\r\n\r\n",
						(حرف *)اندر);
				واپس lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
			}

			اگر((filestat.st_منوال & S_IFREG) != S_IFREG){
				lws_return_http_حالت(wsi, HTTP_حالت_FORBIDDEN, تہی);
				fclose(u->fptr);
				u->fptr = تہی;
				محفوظ_FREE(filename_canonical);
				واپس -1;
			}

			log__printf(تہی, MOSQ_LOG_DEBUG, "http serving file \"%s\".", filename_canonical);
			محفوظ_FREE(filename_canonical);
			/* FIXME - use header functions from lws 2.x */
			buflen = (size_t)snprintf((حرف *)buf, 4096, "HTTP/1.0 200 OK\r\n"
					"Server: mosquitto\r\n"
					"Content-Length: %u\r\n\r\n",
					(غیرعلامتی صحیح)filestat.st_سائز);
			اگر(lws_write(wsi, buf, buflen, LWS_WRITE_HTTP) < 0){
				fclose(u->fptr);
				u->fptr = تہی;
				واپس -1;
			}
			lws_callback_on_writable(wsi);
			توڑو;

		حالت LWS_CALLBACK_HTTP_BODY:
			/* For extra POST data? */
			واپس -1;

		حالت LWS_CALLBACK_HTTP_BODY_COMPLETION:
			/* For end of extra POST data? */
			واپس -1;

		حالت LWS_CALLBACK_FILTER_HTTP_CONNECTION:
			/* Access control here */
			واپس 0;

#اگر LWS_LIBRARY_نسخہ_نمبر >= 3001000
		حالت LWS_CALLBACK_HTTP_CONFIRM_UPGRADE:
			hack = lws_context_user(lws_get_context(wsi));
			اگر(hack->listener->ws_origin_گنتی){
				hlen = lws_hdr_کل_لمبائی(wsi, WSI_TOKEN_ORIGIN);
				اگر(hlen <= 0 || hlen >= (صحیح)حجم(buf)){
					واپس -1;
				}
				برائے(صحیح i=0; i<hack->listener->ws_origin_گنتی; i++){
					lws_hdr_copy(wsi, (حرف *)buf, حجم(buf), WSI_TOKEN_ORIGIN);
					اگر((!strcmp(hack->listener->ws_origins[i], (حرف *)buf))){
						واپس 0;
					}
				}
				واپس -1;
			}ورنہ{
				واپس 0;
			}
#اگر_ختم

		حالت LWS_CALLBACK_HTTP_WRITEABLE:
			/* Send our data here */
			اگر(u && u->fptr){
				کرو{
					buflen = fread(buf, 1, حجم(buf), u->fptr);
					اگر(buflen < 1){
						fclose(u->fptr);
						u->fptr = تہی;
						واپس -1;
					}
					rc = lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
					اگر(rc < 0){
						واپس -1;
					}
					wlen = (size_t)rc;
					/* while still active, extend timeout */
					اگر(wlen){
						lws_سیٹ_timeout(wsi, PENDING_TIMEOUT_HTTP_CONTENT, 10);
					}
					اگر(wlen < buflen){
						اگر(fseek(u->fptr, (طویل)(buflen-wlen), SEEK_CUR) < 0){
							fclose(u->fptr);
							u->fptr = تہی;
							واپس -1;
						}
					}ورنہ{
						اگر(buflen < حجم(buf)){
							fclose(u->fptr);
							u->fptr = تہی;
						}
					}
				}جب(u->fptr && !lws_send_pipe_choked(wsi));
				lws_callback_on_writable(wsi);
			}ورنہ{
				واپس -1;
			}
			توڑو;

		حالت LWS_CALLBACK_CLOSED:
		حالت LWS_CALLBACK_CLOSED_HTTP:
		حالت LWS_CALLBACK_HTTP_FILE_COMPLETION:
			اگر(u && u->fptr){
				fclose(u->fptr);
				u->fptr = تہی;
			}
			توڑو;

		حالت LWS_CALLBACK_جوڑو_POLL_FD:
			HASH_ڈھونڈو(hh_sock, db.contexts_by_sock, &pollargs->fd, حجم(pollargs->fd), mosq);
			اگر(mosq){
				اگر(pollargs->events & LWS_POLLOUT){
					mux_جوڑو_برآمد(mosq);
				}ورنہ{
					mux_remove_برآمد(mosq);
				}
			}ورنہ{
				اگر(pollargs->events & POLLIN){
					/* Assume this is a new listener */
					listeners_جوڑو_websockets(lws_get_context(wsi), pollargs->fd);
				}
			}
			توڑو;

		حالت LWS_CALLBACK_DEL_POLL_FD:
			HASH_ڈھونڈو(hh_sock, db.contexts_by_sock, &pollargs->fd, حجم(pollargs->fd), mosq);
			اگر(mosq){
				mux__delete(mosq);
			}
			توڑو;

		حالت LWS_CALLBACK_CHANGE_منوال_POLL_FD:
			HASH_ڈھونڈو(hh_sock, db.contexts_by_sock, &pollargs->fd, حجم(pollargs->fd), mosq);
			اگر(mosq){
				اگر(pollargs->events & LWS_POLLHUP){
					واپس 1;
				}ورنہ اگر(pollargs->events & LWS_POLLOUT){
					mux_جوڑو_برآمد(mosq);
				}ورنہ{
					mux_remove_برآمد(mosq);
				}
			}
			توڑو;

#اگر_تعریف_شدہ WITH_TLS
		حالت LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION:
			اگر(!len || (SSL_get_verify_نتیجہ((SSL *)اندر) != X509_V_OK)){
				واپس 1;
			}
			توڑو;
#اگر_ختم

		طےشدہ:
			واپس 0;
	}

	واپس 0;
}


ساکن خالی log_wrap(صحیح level, ثابت حرف *سطر)
{
	حرف *l = (حرف *)سطر;
	غیر_مستعمل(level);
	l[لڑی_لمبائی(سطر)-1] = '\0'; /* Remove \n */
	log__printf(تہی, MOSQ_LOG_WEBSOCKETS, "%s", l);
}


خالی mosq_websockets_init(ساخت mosquitto__listener *listener, ثابت ساخت mosquitto__config *conf)
{
	ساخت lws_context_creation_info info;
	ساخت lws_protocols *p;
	size_t protocol_گنتی;
	صحیح i;
	ساخت libws_mqtt_hack *user;

	/* Count valid protocols */
	برائے(protocol_گنتی=0; protocols[protocol_گنتی].name; protocol_گنتی++){
		;
	}

	p = mosquitto_calloc(protocol_گنتی+1, حجم(ساخت lws_protocols));
	اگر(!p){
		log__printf(تہی, MOSQ_LOG_ERR, "Out of memory.");
		واپس;
	}
	برائے(i=0; protocols[i].name; i++){
		p[i].name = protocols[i].name;
		p[i].callback = protocols[i].callback;
		p[i].فی_session_ڈیٹا_سائز = protocols[i].فی_session_ڈیٹا_سائز;
		p[i].rx_بفر_سائز = protocols[i].rx_بفر_سائز;
		p[i].user = listener;
	}

	memset(&info, 0, حجم(info));
	info.iface = listener->host;
	info.پورٹ = listener->پورٹ;
	info.protocols = p;
	info.gid = -1;
	info.uid = -1;
#اگر_تعریف_شدہ WITH_TLS
	اگر(listener->cafile){
		info.ssl_ca_filepath = listener->cafile;
	}ورنہ اگر(listener->capath){
		log__printf(تہی, MOSQ_LOG_WARNING, "Warning: CA path option is not supported for websockets");
	}
	info.ssl_cert_filepath = listener->certfile;
	info.ssl_نجی_کلید_filepath = listener->keyfile;
	info.ssl_cipher_list = listener->ciphers;
	/* HTTP 1 only, due to HTTP 2 issues in Firefox:
	   https://github.com/eclipse-mosquitto/mosquitto/issues/1211
	*/
	info.alpn = "h1";
#اگر defined(WITH_WEBSOCKETS) && LWS_LIBRARY_نسخہ_نمبر>=3001000
	info.tls1_3_plus_cipher_list = listener->ciphers_tls13;
#اگر_ختم
	اگر(listener->require_certificate){
		info.options |= LWS_SERVER_OPTION_REQUIRE_VALID_OPENSSL_CLIENT_CERT;
	}
#اگر_ختم

	info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
	اگر(listener->socket_domain == AF_INET){
		info.options |= LWS_SERVER_OPTION_DISABLE_IPV6;
	}
	info.max_http_سرخی_ڈیٹا = conf->packet_بفر_سائز;

	user = mosquitto_calloc(1, حجم(ساخت libws_mqtt_hack));
	اگر(!user){
		mosquitto_FREE(p);
		log__printf(تہی, MOSQ_LOG_ERR, "Out of memory.");
		واپس;
	}

	اگر(listener->http_dir){
#اگر_تعریف_شدہ WIN32
		user->http_dir = _fullpath(تہی, listener->http_dir, 0);
#ورنہ
		user->http_dir = realpath(listener->http_dir, تہی);
#اگر_ختم
		اگر(!user->http_dir){
			mosquitto_FREE(user);
			mosquitto_FREE(p);
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to open http dir \"%s\".", listener->http_dir);
			واپس;
		}
	}
	user->listener = listener;

	info.user = user;
	info.pt_serv_buf_سائز = WS_SERV_BUF_سائز;
	listener->ws_protocol = p;

	lws_سیٹ_log_level(conf->websockets_log_level, log_wrap);

	log__printf(تہی, MOSQ_LOG_INFO, "Opening websockets listen socket on port %d.", listener->پورٹ);
	listener->ws_اندر_init = true;
	listener->ws_context = lws_create_context(&info);
	listener->ws_اندر_init = false;
}

#اگر_ختم
#اگر_ختم
