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

#اگر_تعریف_شده WITH_WEBSOCKETS

#ضمن "config.h"

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#پایان_اگر
#ضمن "mosquitto_internal.h"
#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "packet_mosq.h"
#ضمن "sys_tree.h"
#ضمن "util_mosq.h"

#ضمن <stdlib.h>
#ضمن <errno.h>
#ضمن <sys/stat.h>

#اگر_تعریف_نشده WIN32
#  include <sys/socket.h>
#پایان_اگر

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
/* Be careful if changing these, if TX is not bigger than SERV then there can
 * be very large write performance penalties.
 */
#تعریف_کن WS_SERV_BUF_اندازه 4096
#تعریف_کن WS_TX_BUF_اندازه (WS_SERV_BUF_اندازه*2)

ساكن صحيح callback_mqtt(
		بنية lws *wsi,
		تعداد lws_callback_reasons reason,
		فراغ *user,
		فراغ *درون,
		size_t len);

ساكن صحيح callback_http(
		بنية lws *wsi,
		تعداد lws_callback_reasons reason,
		فراغ *user,
		فراغ *درون,
		size_t len);

تعداد mosq_ws_protocols {
	PROTOCOL_HTTP = 0,
	PROTOCOL_MQTT,
	DEMO_PROTOCOL_شمار,
};

بنية libws_http_داده {
	FILE *fptr;
};

ساكن بنية lws_protocols protocols[] = {
	/* first protocol must always be HTTP handler */
	{
		"http-only",                        /* name */
		callback_http,                      /* lws_callback_function */
		حجم (بنية libws_http_داده),    /* per_session_data_size */
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		عدم,                               /* user v1.4 on */
		WS_TX_BUF_اندازه                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqtt",
		callback_mqtt,
		حجم(بنية libws_mqtt_داده),
		0,                                  /* rx_buffer_size */
		1,                                  /* id */
		عدم,                               /* user v1.4 on */
		WS_TX_BUF_اندازه                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqttv3.1",
		callback_mqtt,
		حجم(بنية libws_mqtt_داده),
		0,                                  /* rx_buffer_size */
		2,                                  /* id */
		عدم,                               /* user v1.4 on */
		WS_TX_BUF_اندازه                      /* tx_packet_size v2.3.0 */
	},
	{
		عدم,
		عدم,
		0,
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		عدم,                               /* user v1.4 on */
		0                                   /* tx_packet_size v2.3.0 */
	}
};


ساكن فراغ easy_آدرس(صحيح sock, بنية mosquitto *mosq)
{
	حرف آدرس[1024];

	إذا(!net_socket_get_آدرس(sock, آدرس, 1024, &mosq->remote_پورت)){
		mosq->آدرس = mosquitto_strdup(آدرس);
	}
}


ساكن صحيح callback_mqtt(
		بنية lws *wsi,
		تعداد lws_callback_reasons reason,
		فراغ *user,
		فراغ *درون,
		size_t len)
{
	بنية mosquitto *mosq = عدم;
	بنية mosquitto__packet *packet;
	صحيح شمار;
	بیعلامت صحيح ucount;
	ثابت بنية lws_protocols *p;
	بنية libws_mqtt_داده *u = (بنية libws_mqtt_داده *)user;
	size_t pos;
	uint8_t *buf;
	صحيح rc;
	uint8_t byte;
	حرف ip_addr_buff[1024];

	بدل(reason){
		حالة LWS_CALLBACK_ESTABLISHED:
			mosq = context__init();
			إذا(mosq){
				p = lws_get_protocol(wsi);
				mosq->listener = p->user;
				إذا(!mosq->listener){
					mosquitto_FREE(mosq);
					أرجع -1;
				}
				mosq->wsi = wsi;
#اگر_تعریف_شده WITH_TLS
				إذا(درون){
					mosq->ssl = (SSL *)درون;
					إذا(!mosq->listener->ssl_ctx){
						mosq->listener->ssl_ctx = SSL_get_SSL_CTX(mosq->ssl);
					}
				}
#پایان_اگر
				u->mosq = mosq;
			}وإلا{
				أرجع -1;
			}

			easy_آدرس(lws_get_socket_fd(wsi), mosq);
			إذا(!mosq->آدرس){
				/* getpeername and inet_ntop failed and not a bridge */
				mosquitto_FREE(mosq);
				u->mosq = عدم;
				أرجع -1;
			}
			mosq->listener->client_شمار++;
			إذا((mosq->listener->max_connections > 0 && mosq->listener->client_شمار > mosq->listener->max_connections)
					|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (بیعلامت صحيح)db.config->global_max_connections)){

				إذا(db.config->connection_messages == true){
					log__printf(عدم, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", mosq->آدرس);
				}
				mosq->listener->client_شمار--;
				mosquitto_FREE(mosq->آدرس);
				mosquitto_FREE(mosq);
				u->mosq = عدم;
				أرجع -1;
			}
			mosq->sock = lws_get_socket_fd(wsi);
			HASH_جمع_کن(hh_sock, db.contexts_by_sock, sock, حجم(mosq->sock), mosq);
			mux__new(mosq);
			اكسر;

		حالة LWS_CALLBACK_CLOSED:
			إذا(!u){
				أرجع -1;
			}
			mosq = u->mosq;
			إذا(mosq){
				إذا(mosq->sock != INVALID_SOCKET){
					HASH_DELETE(hh_sock, db.contexts_by_sock, mosq);
					mosq->sock = INVALID_SOCKET;
					mux__delete(mosq);
				}
				mosq->wsi = عدم;
#اگر_تعریف_شده WITH_TLS
				mosq->ssl = عدم;
#پایان_اگر
				do_disconnect(mosq, MOSQ_ERR_CONN_گمشده);
			}
			اكسر;

		حالة LWS_CALLBACK_SERVER_WRITEABLE:
			إذا(!u){
				أرجع -1;
			}
			mosq = u->mosq;
			إذا(!mosq){
				أرجع -1;
			}

			rc = db_پیام_write_inflight_بیرون_latest(mosq);
			إذا(rc){
				أرجع -1;
			}
			rc = db_پیام_write_queued_بیرون(mosq);
			إذا(rc){
				أرجع -1;
			}

			طالما(mosq->بیرون_packet && !lws_send_pipe_choked(mosq->wsi)){
				packet = mosq->بیرون_packet;
				شمار = lws_write(wsi, &packet->payload[packet->pos], packet->مقصد_process, LWS_WRITE_دودویی);
				إذا(شمار < 0){
					إذا(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						أرجع -1;
					}
					أرجع 0;
				}
				ucount = (بیعلامت صحيح)شمار;
#اگر_تعریف_شده WITH_SYS_درخت
				metrics__int_inc(mosq_counter_بایتها_فرستاده, ucount);
#پایان_اگر
				packet->مقصد_process -= ucount;
				packet->pos += ucount;
				إذا(packet->مقصد_process > 0){
					إذا(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						أرجع -1;
					}
					اكسر;
				}

#اگر_تعریف_شده WITH_SYS_درخت
				metrics__int_inc(mosq_counter_messages_فرستاده, 1);
				إذا(((packet->command)&0xF0) == CMD_PUBLISH){
					metrics__int_inc(mosq_counter_mqtt_publish_فرستاده, 1);
				}
#پایان_اگر

				packet_get_بعدی_بیرون(mosq);
				mosquitto_FREE(packet);

				mosq->بعدی_پیام_بیرون = db.now_s + mosq->keepalive;
			}
			إذا(mosq->state == mosq_cs_disconnect_ws
					|| mosq->state == mosq_cs_disconnecting
					|| mosq->state == mosq_cs_disused){

				أرجع -1;
			}
			إذا(mosq->بیرون_packet){
				lws_callback_on_writable(mosq->wsi);
			}
			اكسر;

		حالة LWS_CALLBACK_RECEIVE:
			إذا(!u || !u->mosq){
				أرجع -1;
			}
			mosq = u->mosq;
			pos = 0;
			buf = (uint8_t *)درون;
			metrics__int_inc(mosq_counter_بایتها_received, (int64_t)len);
			طالما(pos < len){
				إذا(!mosq->درون_packet.command){
					mosq->درون_packet.command = buf[pos];
					pos++;
					/* Clients must send CONNECT as their first command. */
					إذا(mosq->state == mosq_cs_new && (mosq->درون_packet.command&0xF0) != CMD_CONNECT){
						أرجع -1;
					}
				}
				إذا(mosq->درون_packet.مانده_شمار <= 0){
					افعل{
						إذا(pos == len){
							أرجع 0;
						}
						byte = buf[pos];
						pos++;

						mosq->درون_packet.مانده_شمار--;
						/* Max 4 bytes length for remaining length as defined by protocol.
						* Anything more likely means a broken/malicious client.
						*/
						إذا(mosq->درون_packet.مانده_شمار < -4){
							أرجع -1;
						}

						mosq->درون_packet.مانده_درازی += (byte & 127) * mosq->درون_packet.مانده_mult;
						mosq->درون_packet.مانده_mult *= 128;
					}طالما((byte & 128) != 0);
					mosq->درون_packet.مانده_شمار = (int8_t)(mosq->درون_packet.مانده_شمار * -1);

					إذا(mosq->درون_packet.مانده_درازی > 0){
						mosq->درون_packet.payload = mosquitto_malloc(mosq->درون_packet.مانده_درازی*حجم(uint8_t));
						إذا(!mosq->درون_packet.payload){
							أرجع -1;
						}
						mosq->درون_packet.مقصد_process = mosq->درون_packet.مانده_درازی;
					}
				}
				إذا(mosq->درون_packet.مقصد_process>0){
					إذا((uint32_t)len - pos >= mosq->درون_packet.مقصد_process){
						کپی_حافظه(&mosq->درون_packet.payload[mosq->درون_packet.pos], &buf[pos], mosq->درون_packet.مقصد_process);
						mosq->درون_packet.pos += mosq->درون_packet.مقصد_process;
						pos += mosq->درون_packet.مقصد_process;
						mosq->درون_packet.مقصد_process = 0;
					}وإلا{
						کپی_حافظه(&mosq->درون_packet.payload[mosq->درون_packet.pos], &buf[pos], len-pos);
						mosq->درون_packet.pos += (uint32_t)(len-pos);
						mosq->درون_packet.مقصد_process -= (uint32_t)(len-pos);
						أرجع 0;
					}
				}
				/* All data for this packet is read. */
				mosq->درون_packet.pos = 0;

#اگر_تعریف_شده WITH_SYS_درخت
				metrics__int_inc(mosq_counter_messages_received, 1);
#پایان_اگر
				rc = handle__packet(mosq);

				/* Free data and reset values */
				packet__cleanup(&mosq->درون_packet);

				keepalive__update(mosq);

				إذا(rc && mosq->بیرون_packet){
					إذا(mosq->state != mosq_cs_disconnecting){
						mosquitto_مجموعه_state(mosq, mosq_cs_disconnect_ws);
					}
					lws_callback_on_writable(mosq->wsi);
				}وإلا إذا(rc){
					do_disconnect(mosq, MOSQ_ERR_CONN_گمشده);
					أرجع -1;
				}
			}
			اكسر;

		افتراضي:
			اكسر;
	}

	أرجع 0;
}


ساكن حرف *http__canonical_filename(
		بنية lws *wsi,
		ثابت حرف *درون,
		ثابت حرف *http_dir)
{
	size_t inlen, slen;
	حرف *filename, *filename_canonical;

	inlen = طول_رشته(درون);
	إذا(درون[inlen-1] == '/'){
		slen = طول_رشته(http_dir) + inlen + طول_رشته("/index.html") + 2;
	}وإلا{
		slen = طول_رشته(http_dir) + inlen + 2;
	}
	filename = mosquitto_malloc(slen);
	إذا(!filename){
		lws_return_http_وضعیت(wsi, HTTP_وضعیت_داخلی_SERVER_ERROR, عدم);
		أرجع عدم;
	}
	إذا(((حرف *)درون)[inlen-1] == '/'){
		snprintf(filename, slen, "%s%sindex.html", http_dir, (حرف *)درون);
	}وإلا{
		snprintf(filename, slen, "%s%s", http_dir, (حرف *)درون);
	}


	/* Get canonical path and check it is within our http_dir */
#اگر_تعریف_شده WIN32
	filename_canonical = _fullpath(عدم, filename, 0);
	mosquitto_FREE(filename);
	إذا(!filename_canonical){
		lws_return_http_وضعیت(wsi, HTTP_وضعیت_داخلی_SERVER_ERROR, عدم);
		أرجع عدم;
	}
#وإلا
	filename_canonical = realpath(filename, عدم);
	mosquitto_FREE(filename);
	إذا(!filename_canonical){
		إذا(errno == EACCES){
			lws_return_http_وضعیت(wsi, HTTP_وضعیت_FORBIDDEN, عدم);
		}وإلا إذا(errno == EINVAL || errno == EIO || errno == ELOOP){
			lws_return_http_وضعیت(wsi, HTTP_وضعیت_داخلی_SERVER_ERROR, عدم);
		}وإلا إذا(errno == ENAMETOOLONG){
			lws_return_http_وضعیت(wsi, HTTP_وضعیت_REQ_URI_TOO_LONG, عدم);
		}وإلا إذا(errno == ENOENT || errno == ENOTDIR){
			lws_return_http_وضعیت(wsi, HTTP_وضعیت_NOT_یافت, عدم);
		}
		أرجع عدم;
	}
#پایان_اگر
	إذا(مقایسه_رشته(http_dir, filename_canonical, طول_رشته(http_dir))){
		/* Requested file isn't within http_dir, deny access. */
		امن_FREE(filename_canonical);
		lws_return_http_وضعیت(wsi, HTTP_وضعیت_FORBIDDEN, عدم);
		أرجع عدم;
	}

	أرجع filename_canonical;
}


ساكن صحيح callback_http(
		بنية lws *wsi,
		تعداد lws_callback_reasons reason,
		فراغ *user,
		فراغ *درون,
		size_t len)
{
	بنية libws_http_داده *u = (بنية libws_http_داده *)user;
	بنية libws_mqtt_hack *hack;
	حرف *http_dir;
	size_t buflen;
	size_t wlen;
	صحيح rc;
	حرف *filename_canonical;
	بیعلامت حرف buf[4096];
	بنية stat filestat;
	بنية mosquitto *mosq;
	بنية lws_pollargs *pollargs = (بنية lws_pollargs *)درون;
	صحيح hlen;

	/* FIXME - ssl cert verification is done here. */

	بدل(reason){
		حالة LWS_CALLBACK_HTTP:
			إذا(!u){
				أرجع -1;
			}

			hack = (بنية libws_mqtt_hack *)lws_context_user(lws_get_context(wsi));
			إذا(!hack){
				أرجع -1;
			}
			http_dir = hack->http_dir;

			إذا(!http_dir){
				/* http disabled */
				أرجع -1;
			}

			/* Forbid POST */
			إذا(lws_hdr_مجموع_درازی(wsi, WSI_TOKEN_POST_URI)){
				lws_return_http_وضعیت(wsi, HTTP_وضعیت_روش_NOT_ALLOWED, عدم);
				أرجع -1;
			}

			filename_canonical = http__canonical_filename(wsi, (حرف *)درون, http_dir);
			إذا(!filename_canonical){
				أرجع -1;
			}

			u->fptr = fopen(filename_canonical, "rb");
			إذا(!u->fptr){
				امن_FREE(filename_canonical);
				lws_return_http_وضعیت(wsi, HTTP_وضعیت_NOT_یافت, عدم);
				أرجع -1;
			}
			إذا(fstat(fileno(u->fptr), &filestat) < 0){
				امن_FREE(filename_canonical);
				lws_return_http_وضعیت(wsi, HTTP_وضعیت_داخلی_SERVER_ERROR, عدم);
				fclose(u->fptr);
				u->fptr = عدم;
				أرجع -1;
			}


			إذا((filestat.st_نما & S_IFDIR) == S_IFDIR){
				fclose(u->fptr);
				u->fptr = عدم;
				امن_FREE(filename_canonical);

				/* FIXME - use header functions from lws 2.x */
				buflen = (size_t)snprintf((حرف *)buf, 4096, "HTTP/1.0 302 OK\r\n"
						"Location: %s/\r\n\r\n",
						(حرف *)درون);
				أرجع lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
			}

			إذا((filestat.st_نما & S_IFREG) != S_IFREG){
				lws_return_http_وضعیت(wsi, HTTP_وضعیت_FORBIDDEN, عدم);
				fclose(u->fptr);
				u->fptr = عدم;
				امن_FREE(filename_canonical);
				أرجع -1;
			}

			log__printf(عدم, MOSQ_LOG_DEBUG, "http serving file \"%s\".", filename_canonical);
			امن_FREE(filename_canonical);
			/* FIXME - use header functions from lws 2.x */
			buflen = (size_t)snprintf((حرف *)buf, 4096, "HTTP/1.0 200 OK\r\n"
					"Server: mosquitto\r\n"
					"Content-Length: %u\r\n\r\n",
					(بیعلامت صحيح)filestat.st_اندازه);
			إذا(lws_write(wsi, buf, buflen, LWS_WRITE_HTTP) < 0){
				fclose(u->fptr);
				u->fptr = عدم;
				أرجع -1;
			}
			lws_callback_on_writable(wsi);
			اكسر;

		حالة LWS_CALLBACK_HTTP_BODY:
			/* For extra POST data? */
			أرجع -1;

		حالة LWS_CALLBACK_HTTP_BODY_COMPLETION:
			/* For end of extra POST data? */
			أرجع -1;

		حالة LWS_CALLBACK_FILTER_HTTP_CONNECTION:
			/* Access control here */
			أرجع 0;

#إذا LWS_LIBRARY_نسخه_عدد >= 3001000
		حالة LWS_CALLBACK_HTTP_CONFIRM_UPGRADE:
			hack = lws_context_user(lws_get_context(wsi));
			إذا(hack->listener->ws_origin_شمار){
				hlen = lws_hdr_مجموع_درازی(wsi, WSI_TOKEN_ORIGIN);
				إذا(hlen <= 0 || hlen >= (صحيح)حجم(buf)){
					أرجع -1;
				}
				لكل(صحيح i=0; i<hack->listener->ws_origin_شمار; i++){
					lws_hdr_copy(wsi, (حرف *)buf, حجم(buf), WSI_TOKEN_ORIGIN);
					إذا((!strcmp(hack->listener->ws_origins[i], (حرف *)buf))){
						أرجع 0;
					}
				}
				أرجع -1;
			}وإلا{
				أرجع 0;
			}
#پایان_اگر

		حالة LWS_CALLBACK_HTTP_WRITEABLE:
			/* Send our data here */
			إذا(u && u->fptr){
				افعل{
					buflen = fread(buf, 1, حجم(buf), u->fptr);
					إذا(buflen < 1){
						fclose(u->fptr);
						u->fptr = عدم;
						أرجع -1;
					}
					rc = lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
					إذا(rc < 0){
						أرجع -1;
					}
					wlen = (size_t)rc;
					/* while still active, extend timeout */
					إذا(wlen){
						lws_مجموعه_timeout(wsi, PENDING_TIMEOUT_HTTP_CONTENT, 10);
					}
					إذا(wlen < buflen){
						إذا(fseek(u->fptr, (دراز)(buflen-wlen), SEEK_CUR) < 0){
							fclose(u->fptr);
							u->fptr = عدم;
							أرجع -1;
						}
					}وإلا{
						إذا(buflen < حجم(buf)){
							fclose(u->fptr);
							u->fptr = عدم;
						}
					}
				}طالما(u->fptr && !lws_send_pipe_choked(wsi));
				lws_callback_on_writable(wsi);
			}وإلا{
				أرجع -1;
			}
			اكسر;

		حالة LWS_CALLBACK_CLOSED:
		حالة LWS_CALLBACK_CLOSED_HTTP:
		حالة LWS_CALLBACK_HTTP_FILE_COMPLETION:
			إذا(u && u->fptr){
				fclose(u->fptr);
				u->fptr = عدم;
			}
			اكسر;

		حالة LWS_CALLBACK_جمع_کن_POLL_FD:
			HASH_پیدا_کن(hh_sock, db.contexts_by_sock, &pollargs->fd, حجم(pollargs->fd), mosq);
			إذا(mosq){
				إذا(pollargs->events & LWS_POLLOUT){
					mux_جمع_کن_بیرون(mosq);
				}وإلا{
					mux_remove_بیرون(mosq);
				}
			}وإلا{
				إذا(pollargs->events & POLLIN){
					/* Assume this is a new listener */
					listeners_جمع_کن_websockets(lws_get_context(wsi), pollargs->fd);
				}
			}
			اكسر;

		حالة LWS_CALLBACK_DEL_POLL_FD:
			HASH_پیدا_کن(hh_sock, db.contexts_by_sock, &pollargs->fd, حجم(pollargs->fd), mosq);
			إذا(mosq){
				mux__delete(mosq);
			}
			اكسر;

		حالة LWS_CALLBACK_CHANGE_نما_POLL_FD:
			HASH_پیدا_کن(hh_sock, db.contexts_by_sock, &pollargs->fd, حجم(pollargs->fd), mosq);
			إذا(mosq){
				إذا(pollargs->events & LWS_POLLHUP){
					أرجع 1;
				}وإلا إذا(pollargs->events & LWS_POLLOUT){
					mux_جمع_کن_بیرون(mosq);
				}وإلا{
					mux_remove_بیرون(mosq);
				}
			}
			اكسر;

#اگر_تعریف_شده WITH_TLS
		حالة LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION:
			إذا(!len || (SSL_get_verify_نتیجه((SSL *)درون) != X509_V_OK)){
				أرجع 1;
			}
			اكسر;
#پایان_اگر

		افتراضي:
			أرجع 0;
	}

	أرجع 0;
}


ساكن فراغ log_wrap(صحيح level, ثابت حرف *سطر)
{
	حرف *l = (حرف *)سطر;
	استفادهنشده(level);
	l[طول_رشته(سطر)-1] = '\0'; /* Remove \n */
	log__printf(عدم, MOSQ_LOG_WEBSOCKETS, "%s", l);
}


فراغ mosq_websockets_init(بنية mosquitto__listener *listener, ثابت بنية mosquitto__config *conf)
{
	بنية lws_context_creation_info info;
	بنية lws_protocols *p;
	size_t protocol_شمار;
	صحيح i;
	بنية libws_mqtt_hack *user;

	/* Count valid protocols */
	لكل(protocol_شمار=0; protocols[protocol_شمار].name; protocol_شمار++){
		;
	}

	p = mosquitto_calloc(protocol_شمار+1, حجم(بنية lws_protocols));
	إذا(!p){
		log__printf(عدم, MOSQ_LOG_ERR, "Out of memory.");
		أرجع;
	}
	لكل(i=0; protocols[i].name; i++){
		p[i].name = protocols[i].name;
		p[i].callback = protocols[i].callback;
		p[i].در_هر_session_داده_اندازه = protocols[i].در_هر_session_داده_اندازه;
		p[i].rx_میانگیر_اندازه = protocols[i].rx_میانگیر_اندازه;
		p[i].user = listener;
	}

	memset(&info, 0, حجم(info));
	info.iface = listener->host;
	info.پورت = listener->پورت;
	info.protocols = p;
	info.gid = -1;
	info.uid = -1;
#اگر_تعریف_شده WITH_TLS
	إذا(listener->cafile){
		info.ssl_ca_filepath = listener->cafile;
	}وإلا إذا(listener->capath){
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: CA path option is not supported for websockets");
	}
	info.ssl_cert_filepath = listener->certfile;
	info.ssl_خصوصی_کلید_filepath = listener->keyfile;
	info.ssl_cipher_list = listener->ciphers;
	/* HTTP 1 only, due to HTTP 2 issues in Firefox:
	   https://github.com/eclipse-mosquitto/mosquitto/issues/1211
	*/
	info.alpn = "h1";
#إذا defined(WITH_WEBSOCKETS) && LWS_LIBRARY_نسخه_عدد>=3001000
	info.tls1_3_plus_cipher_list = listener->ciphers_tls13;
#پایان_اگر
	إذا(listener->require_certificate){
		info.options |= LWS_SERVER_OPTION_REQUIRE_VALID_OPENSSL_CLIENT_CERT;
	}
#پایان_اگر

	info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
	إذا(listener->socket_domain == AF_INET){
		info.options |= LWS_SERVER_OPTION_DISABLE_IPV6;
	}
	info.max_http_سرآیند_داده = conf->packet_میانگیر_اندازه;

	user = mosquitto_calloc(1, حجم(بنية libws_mqtt_hack));
	إذا(!user){
		mosquitto_FREE(p);
		log__printf(عدم, MOSQ_LOG_ERR, "Out of memory.");
		أرجع;
	}

	إذا(listener->http_dir){
#اگر_تعریف_شده WIN32
		user->http_dir = _fullpath(عدم, listener->http_dir, 0);
#وإلا
		user->http_dir = realpath(listener->http_dir, عدم);
#پایان_اگر
		إذا(!user->http_dir){
			mosquitto_FREE(user);
			mosquitto_FREE(p);
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to open http dir \"%s\".", listener->http_dir);
			أرجع;
		}
	}
	user->listener = listener;

	info.user = user;
	info.pt_serv_buf_اندازه = WS_SERV_BUF_اندازه;
	listener->ws_protocol = p;

	lws_مجموعه_log_level(conf->websockets_log_level, log_wrap);

	log__printf(عدم, MOSQ_LOG_INFO, "Opening websockets listen socket on port %d.", listener->پورت);
	listener->ws_درون_init = true;
	listener->ws_context = lws_create_context(&info);
	listener->ws_درون_init = false;
}

#پایان_اگر
#پایان_اگر
