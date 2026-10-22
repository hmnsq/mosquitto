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

#جے_تعریف_شدہ WITH_WEBSOCKETS

#ضمن "config.h"

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#جے_انت
#ضمن "mosquitto_internal.h"
#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "packet_mosq.h"
#ضمن "sys_tree.h"
#ضمن "util_mosq.h"

#ضمن <stdlib.h>
#ضمن <errno.h>
#ضمن <sys/stat.h>

#جے_بنا_تعریف WIN32
#  include <sys/socket.h>
#جے_انت

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
/* Be careful if changing these, if TX is not bigger than SERV then there can
 * be very large write performance penalties.
 */
#تعریف_کرو WS_SERV_BUF_ناپ 4096
#تعریف_کرو WS_TX_BUF_ناپ (WS_SERV_BUF_ناپ*2)

ساكن صحيح callback_mqtt(
		بنية lws *wsi,
		تعداد lws_callback_reasons reason,
		فراغ *user,
		فراغ *وچ,
		size_t len);

ساكن صحيح callback_http(
		بنية lws *wsi,
		تعداد lws_callback_reasons reason,
		فراغ *user,
		فراغ *وچ,
		size_t len);

تعداد mosq_ws_protocols {
	PROTOCOL_HTTP = 0,
	PROTOCOL_MQTT,
	DEMO_PROTOCOL_گنتی,
};

بنية libws_http_انکڑے {
	FILE *fptr;
};

ساكن بنية lws_protocols protocols[] = {
	/* first protocol must always be HTTP handler */
	{
		"http-only",                        /* name */
		callback_http,                      /* lws_callback_function */
		حجم (بنية libws_http_انکڑے),    /* per_session_data_size */
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		عدم,                               /* user v1.4 on */
		WS_TX_BUF_ناپ                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqtt",
		callback_mqtt,
		حجم(بنية libws_mqtt_انکڑے),
		0,                                  /* rx_buffer_size */
		1,                                  /* id */
		عدم,                               /* user v1.4 on */
		WS_TX_BUF_ناپ                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqttv3.1",
		callback_mqtt,
		حجم(بنية libws_mqtt_انکڑے),
		0,                                  /* rx_buffer_size */
		2,                                  /* id */
		عدم,                               /* user v1.4 on */
		WS_TX_BUF_ناپ                      /* tx_packet_size v2.3.0 */
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


ساكن فراغ easy_پتہ(صحيح sock, بنية mosquitto *mosq)
{
	حرف پتہ[1024];

	إذا(!net_socket_get_پتہ(sock, پتہ, 1024, &mosq->remote_پورٹ)){
		mosq->پتہ = mosquitto_strdup(پتہ);
	}
}


ساكن صحيح callback_mqtt(
		بنية lws *wsi,
		تعداد lws_callback_reasons reason,
		فراغ *user,
		فراغ *وچ,
		size_t len)
{
	بنية mosquitto *mosq = عدم;
	بنية mosquitto__packet *packet;
	صحيح گنتی;
	بنانشانی صحيح ucount;
	ثابت بنية lws_protocols *p;
	بنية libws_mqtt_انکڑے *u = (بنية libws_mqtt_انکڑے *)user;
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
#جے_تعریف_شدہ WITH_TLS
				إذا(وچ){
					mosq->ssl = (SSL *)وچ;
					إذا(!mosq->listener->ssl_ctx){
						mosq->listener->ssl_ctx = SSL_get_SSL_CTX(mosq->ssl);
					}
				}
#جے_انت
				u->mosq = mosq;
			}وإلا{
				أرجع -1;
			}

			easy_پتہ(lws_get_socket_fd(wsi), mosq);
			إذا(!mosq->پتہ){
				/* getpeername and inet_ntop failed and not a bridge */
				mosquitto_FREE(mosq);
				u->mosq = عدم;
				أرجع -1;
			}
			mosq->listener->client_گنتی++;
			إذا((mosq->listener->max_connections > 0 && mosq->listener->client_گنتی > mosq->listener->max_connections)
					|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (بنانشانی صحيح)db.config->global_max_connections)){

				إذا(db.config->connection_messages == true){
					log__printf(عدم, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", mosq->پتہ);
				}
				mosq->listener->client_گنتی--;
				mosquitto_FREE(mosq->پتہ);
				mosquitto_FREE(mosq);
				u->mosq = عدم;
				أرجع -1;
			}
			mosq->sock = lws_get_socket_fd(wsi);
			HASH_جوڑو(hh_sock, db.contexts_by_sock, sock, حجم(mosq->sock), mosq);
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
#جے_تعریف_شدہ WITH_TLS
				mosq->ssl = عدم;
#جے_انت
				do_disconnect(mosq, MOSQ_ERR_CONN_گواچا);
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

			rc = db_سنیہا_write_inflight_باہرلا_latest(mosq);
			إذا(rc){
				أرجع -1;
			}
			rc = db_سنیہا_write_queued_باہرلا(mosq);
			إذا(rc){
				أرجع -1;
			}

			طالما(mosq->باہرلا_packet && !lws_send_pipe_choked(mosq->wsi)){
				packet = mosq->باہرلا_packet;
				گنتی = lws_write(wsi, &packet->payload[packet->pos], packet->منزل_process, LWS_WRITE_ثنائی);
				إذا(گنتی < 0){
					إذا(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						أرجع -1;
					}
					أرجع 0;
				}
				ucount = (بنانشانی صحيح)گنتی;
#جے_تعریف_شدہ WITH_SYS_درخت
				metrics__int_inc(mosq_counter_بائٹاں_گھلیا, ucount);
#جے_انت
				packet->منزل_process -= ucount;
				packet->pos += ucount;
				إذا(packet->منزل_process > 0){
					إذا(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						أرجع -1;
					}
					اكسر;
				}

#جے_تعریف_شدہ WITH_SYS_درخت
				metrics__int_inc(mosq_counter_messages_گھلیا, 1);
				إذا(((packet->command)&0xF0) == CMD_PUBLISH){
					metrics__int_inc(mosq_counter_mqtt_publish_گھلیا, 1);
				}
#جے_انت

				packet_get_اگلا_باہرلا(mosq);
				mosquitto_FREE(packet);

				mosq->اگلا_پیغام_باہرلا = db.now_s + mosq->keepalive;
			}
			إذا(mosq->state == mosq_cs_disconnect_ws
					|| mosq->state == mosq_cs_disconnecting
					|| mosq->state == mosq_cs_disused){

				أرجع -1;
			}
			إذا(mosq->باہرلا_packet){
				lws_callback_on_writable(mosq->wsi);
			}
			اكسر;

		حالة LWS_CALLBACK_RECEIVE:
			إذا(!u || !u->mosq){
				أرجع -1;
			}
			mosq = u->mosq;
			pos = 0;
			buf = (uint8_t *)وچ;
			metrics__int_inc(mosq_counter_بائٹاں_received, (int64_t)len);
			طالما(pos < len){
				إذا(!mosq->وچ_packet.command){
					mosq->وچ_packet.command = buf[pos];
					pos++;
					/* Clients must send CONNECT as their first command. */
					إذا(mosq->state == mosq_cs_new && (mosq->وچ_packet.command&0xF0) != CMD_CONNECT){
						أرجع -1;
					}
				}
				إذا(mosq->وچ_packet.بچیا_گنتی <= 0){
					افعل{
						إذا(pos == len){
							أرجع 0;
						}
						byte = buf[pos];
						pos++;

						mosq->وچ_packet.بچیا_گنتی--;
						/* Max 4 bytes length for remaining length as defined by protocol.
						* Anything more likely means a broken/malicious client.
						*/
						إذا(mosq->وچ_packet.بچیا_گنتی < -4){
							أرجع -1;
						}

						mosq->وچ_packet.بچیا_لمائی += (byte & 127) * mosq->وچ_packet.بچیا_mult;
						mosq->وچ_packet.بچیا_mult *= 128;
					}طالما((byte & 128) != 0);
					mosq->وچ_packet.بچیا_گنتی = (int8_t)(mosq->وچ_packet.بچیا_گنتی * -1);

					إذا(mosq->وچ_packet.بچیا_لمائی > 0){
						mosq->وچ_packet.payload = mosquitto_malloc(mosq->وچ_packet.بچیا_لمائی*حجم(uint8_t));
						إذا(!mosq->وچ_packet.payload){
							أرجع -1;
						}
						mosq->وچ_packet.منزل_process = mosq->وچ_packet.بچیا_لمائی;
					}
				}
				إذا(mosq->وچ_packet.منزل_process>0){
					إذا((uint32_t)len - pos >= mosq->وچ_packet.منزل_process){
						حافظہ_نقل(&mosq->وچ_packet.payload[mosq->وچ_packet.pos], &buf[pos], mosq->وچ_packet.منزل_process);
						mosq->وچ_packet.pos += mosq->وچ_packet.منزل_process;
						pos += mosq->وچ_packet.منزل_process;
						mosq->وچ_packet.منزل_process = 0;
					}وإلا{
						حافظہ_نقل(&mosq->وچ_packet.payload[mosq->وچ_packet.pos], &buf[pos], len-pos);
						mosq->وچ_packet.pos += (uint32_t)(len-pos);
						mosq->وچ_packet.منزل_process -= (uint32_t)(len-pos);
						أرجع 0;
					}
				}
				/* All data for this packet is read. */
				mosq->وچ_packet.pos = 0;

#جے_تعریف_شدہ WITH_SYS_درخت
				metrics__int_inc(mosq_counter_messages_received, 1);
#جے_انت
				rc = handle__packet(mosq);

				/* Free data and reset values */
				packet__cleanup(&mosq->وچ_packet);

				keepalive__update(mosq);

				إذا(rc && mosq->باہرلا_packet){
					إذا(mosq->state != mosq_cs_disconnecting){
						mosquitto_سیٹ_state(mosq, mosq_cs_disconnect_ws);
					}
					lws_callback_on_writable(mosq->wsi);
				}وإلا إذا(rc){
					do_disconnect(mosq, MOSQ_ERR_CONN_گواچا);
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
		ثابت حرف *وچ,
		ثابت حرف *http_dir)
{
	size_t inlen, slen;
	حرف *filename, *filename_canonical;

	inlen = لڑی_لمبائی(وچ);
	إذا(وچ[inlen-1] == '/'){
		slen = لڑی_لمبائی(http_dir) + inlen + لڑی_لمبائی("/index.html") + 2;
	}وإلا{
		slen = لڑی_لمبائی(http_dir) + inlen + 2;
	}
	filename = mosquitto_malloc(slen);
	إذا(!filename){
		lws_return_http_حالت(wsi, HTTP_حالت_اندرونی_SERVER_ERROR, عدم);
		أرجع عدم;
	}
	إذا(((حرف *)وچ)[inlen-1] == '/'){
		snprintf(filename, slen, "%s%sindex.html", http_dir, (حرف *)وچ);
	}وإلا{
		snprintf(filename, slen, "%s%s", http_dir, (حرف *)وچ);
	}


	/* Get canonical path and check it is within our http_dir */
#جے_تعریف_شدہ WIN32
	filename_canonical = _fullpath(عدم, filename, 0);
	mosquitto_FREE(filename);
	إذا(!filename_canonical){
		lws_return_http_حالت(wsi, HTTP_حالت_اندرونی_SERVER_ERROR, عدم);
		أرجع عدم;
	}
#وإلا
	filename_canonical = realpath(filename, عدم);
	mosquitto_FREE(filename);
	إذا(!filename_canonical){
		إذا(errno == EACCES){
			lws_return_http_حالت(wsi, HTTP_حالت_FORBIDDEN, عدم);
		}وإلا إذا(errno == EINVAL || errno == EIO || errno == ELOOP){
			lws_return_http_حالت(wsi, HTTP_حالت_اندرونی_SERVER_ERROR, عدم);
		}وإلا إذا(errno == ENAMETOOLONG){
			lws_return_http_حالت(wsi, HTTP_حالت_REQ_URI_TOO_LONG, عدم);
		}وإلا إذا(errno == ENOENT || errno == ENOTDIR){
			lws_return_http_حالت(wsi, HTTP_حالت_NOT_لبھیا, عدم);
		}
		أرجع عدم;
	}
#جے_انت
	إذا(لڑی_مقابلہ(http_dir, filename_canonical, لڑی_لمبائی(http_dir))){
		/* Requested file isn't within http_dir, deny access. */
		محفوظ_FREE(filename_canonical);
		lws_return_http_حالت(wsi, HTTP_حالت_FORBIDDEN, عدم);
		أرجع عدم;
	}

	أرجع filename_canonical;
}


ساكن صحيح callback_http(
		بنية lws *wsi,
		تعداد lws_callback_reasons reason,
		فراغ *user,
		فراغ *وچ,
		size_t len)
{
	بنية libws_http_انکڑے *u = (بنية libws_http_انکڑے *)user;
	بنية libws_mqtt_hack *hack;
	حرف *http_dir;
	size_t buflen;
	size_t wlen;
	صحيح rc;
	حرف *filename_canonical;
	بنانشانی حرف buf[4096];
	بنية stat filestat;
	بنية mosquitto *mosq;
	بنية lws_pollargs *pollargs = (بنية lws_pollargs *)وچ;
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
			إذا(lws_hdr_کل_لمائی(wsi, WSI_TOKEN_POST_URI)){
				lws_return_http_حالت(wsi, HTTP_حالت_طریقہ_NOT_ALLOWED, عدم);
				أرجع -1;
			}

			filename_canonical = http__canonical_filename(wsi, (حرف *)وچ, http_dir);
			إذا(!filename_canonical){
				أرجع -1;
			}

			u->fptr = fopen(filename_canonical, "rb");
			إذا(!u->fptr){
				محفوظ_FREE(filename_canonical);
				lws_return_http_حالت(wsi, HTTP_حالت_NOT_لبھیا, عدم);
				أرجع -1;
			}
			إذا(fstat(fileno(u->fptr), &filestat) < 0){
				محفوظ_FREE(filename_canonical);
				lws_return_http_حالت(wsi, HTTP_حالت_اندرونی_SERVER_ERROR, عدم);
				fclose(u->fptr);
				u->fptr = عدم;
				أرجع -1;
			}


			إذا((filestat.st_منوال & S_IFDIR) == S_IFDIR){
				fclose(u->fptr);
				u->fptr = عدم;
				محفوظ_FREE(filename_canonical);

				/* FIXME - use header functions from lws 2.x */
				buflen = (size_t)snprintf((حرف *)buf, 4096, "HTTP/1.0 302 OK\r\n"
						"Location: %s/\r\n\r\n",
						(حرف *)وچ);
				أرجع lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
			}

			إذا((filestat.st_منوال & S_IFREG) != S_IFREG){
				lws_return_http_حالت(wsi, HTTP_حالت_FORBIDDEN, عدم);
				fclose(u->fptr);
				u->fptr = عدم;
				محفوظ_FREE(filename_canonical);
				أرجع -1;
			}

			log__printf(عدم, MOSQ_LOG_DEBUG, "http serving file \"%s\".", filename_canonical);
			محفوظ_FREE(filename_canonical);
			/* FIXME - use header functions from lws 2.x */
			buflen = (size_t)snprintf((حرف *)buf, 4096, "HTTP/1.0 200 OK\r\n"
					"Server: mosquitto\r\n"
					"Content-Length: %u\r\n\r\n",
					(بنانشانی صحيح)filestat.st_ناپ);
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

#إذا LWS_LIBRARY_نسخہ_نمبر >= 3001000
		حالة LWS_CALLBACK_HTTP_CONFIRM_UPGRADE:
			hack = lws_context_user(lws_get_context(wsi));
			إذا(hack->listener->ws_origin_گنتی){
				hlen = lws_hdr_کل_لمائی(wsi, WSI_TOKEN_ORIGIN);
				إذا(hlen <= 0 || hlen >= (صحيح)حجم(buf)){
					أرجع -1;
				}
				لكل(صحيح i=0; i<hack->listener->ws_origin_گنتی; i++){
					lws_hdr_copy(wsi, (حرف *)buf, حجم(buf), WSI_TOKEN_ORIGIN);
					إذا((!strcmp(hack->listener->ws_origins[i], (حرف *)buf))){
						أرجع 0;
					}
				}
				أرجع -1;
			}وإلا{
				أرجع 0;
			}
#جے_انت

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
						lws_سیٹ_timeout(wsi, PENDING_TIMEOUT_HTTP_CONTENT, 10);
					}
					إذا(wlen < buflen){
						إذا(fseek(u->fptr, (لما)(buflen-wlen), SEEK_CUR) < 0){
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

		حالة LWS_CALLBACK_جوڑو_POLL_FD:
			HASH_لبھو(hh_sock, db.contexts_by_sock, &pollargs->fd, حجم(pollargs->fd), mosq);
			إذا(mosq){
				إذا(pollargs->events & LWS_POLLOUT){
					mux_جوڑو_باہرلا(mosq);
				}وإلا{
					mux_remove_باہرلا(mosq);
				}
			}وإلا{
				إذا(pollargs->events & POLLIN){
					/* Assume this is a new listener */
					listeners_جوڑو_websockets(lws_get_context(wsi), pollargs->fd);
				}
			}
			اكسر;

		حالة LWS_CALLBACK_DEL_POLL_FD:
			HASH_لبھو(hh_sock, db.contexts_by_sock, &pollargs->fd, حجم(pollargs->fd), mosq);
			إذا(mosq){
				mux__delete(mosq);
			}
			اكسر;

		حالة LWS_CALLBACK_CHANGE_منوال_POLL_FD:
			HASH_لبھو(hh_sock, db.contexts_by_sock, &pollargs->fd, حجم(pollargs->fd), mosq);
			إذا(mosq){
				إذا(pollargs->events & LWS_POLLHUP){
					أرجع 1;
				}وإلا إذا(pollargs->events & LWS_POLLOUT){
					mux_جوڑو_باہرلا(mosq);
				}وإلا{
					mux_remove_باہرلا(mosq);
				}
			}
			اكسر;

#جے_تعریف_شدہ WITH_TLS
		حالة LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION:
			إذا(!len || (SSL_get_verify_نتیجہ((SSL *)وچ) != X509_V_OK)){
				أرجع 1;
			}
			اكسر;
#جے_انت

		افتراضي:
			أرجع 0;
	}

	أرجع 0;
}


ساكن فراغ log_wrap(صحيح level, ثابت حرف *سطر)
{
	حرف *l = (حرف *)سطر;
	بن_ورتیا(level);
	l[لڑی_لمبائی(سطر)-1] = '\0'; /* Remove \n */
	log__printf(عدم, MOSQ_LOG_WEBSOCKETS, "%s", l);
}


فراغ mosq_websockets_init(بنية mosquitto__listener *listener, ثابت بنية mosquitto__config *conf)
{
	بنية lws_context_creation_info info;
	بنية lws_protocols *p;
	size_t protocol_گنتی;
	صحيح i;
	بنية libws_mqtt_hack *user;

	/* Count valid protocols */
	لكل(protocol_گنتی=0; protocols[protocol_گنتی].name; protocol_گنتی++){
		;
	}

	p = mosquitto_calloc(protocol_گنتی+1, حجم(بنية lws_protocols));
	إذا(!p){
		log__printf(عدم, MOSQ_LOG_ERR, "Out of memory.");
		أرجع;
	}
	لكل(i=0; protocols[i].name; i++){
		p[i].name = protocols[i].name;
		p[i].callback = protocols[i].callback;
		p[i].فی_session_انکڑے_ناپ = protocols[i].فی_session_انکڑے_ناپ;
		p[i].rx_بفر_ناپ = protocols[i].rx_بفر_ناپ;
		p[i].user = listener;
	}

	memset(&info, 0, حجم(info));
	info.iface = listener->host;
	info.پورٹ = listener->پورٹ;
	info.protocols = p;
	info.gid = -1;
	info.uid = -1;
#جے_تعریف_شدہ WITH_TLS
	إذا(listener->cafile){
		info.ssl_ca_filepath = listener->cafile;
	}وإلا إذا(listener->capath){
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: CA path option is not supported for websockets");
	}
	info.ssl_cert_filepath = listener->certfile;
	info.ssl_نجی_کلید_filepath = listener->keyfile;
	info.ssl_cipher_list = listener->ciphers;
	/* HTTP 1 only, due to HTTP 2 issues in Firefox:
	   https://github.com/eclipse-mosquitto/mosquitto/issues/1211
	*/
	info.alpn = "h1";
#إذا defined(WITH_WEBSOCKETS) && LWS_LIBRARY_نسخہ_نمبر>=3001000
	info.tls1_3_plus_cipher_list = listener->ciphers_tls13;
#جے_انت
	إذا(listener->require_certificate){
		info.options |= LWS_SERVER_OPTION_REQUIRE_VALID_OPENSSL_CLIENT_CERT;
	}
#جے_انت

	info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
	إذا(listener->socket_domain == AF_INET){
		info.options |= LWS_SERVER_OPTION_DISABLE_IPV6;
	}
	info.max_http_سرخی_انکڑے = conf->packet_بفر_ناپ;

	user = mosquitto_calloc(1, حجم(بنية libws_mqtt_hack));
	إذا(!user){
		mosquitto_FREE(p);
		log__printf(عدم, MOSQ_LOG_ERR, "Out of memory.");
		أرجع;
	}

	إذا(listener->http_dir){
#جے_تعریف_شدہ WIN32
		user->http_dir = _fullpath(عدم, listener->http_dir, 0);
#وإلا
		user->http_dir = realpath(listener->http_dir, عدم);
#جے_انت
		إذا(!user->http_dir){
			mosquitto_FREE(user);
			mosquitto_FREE(p);
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to open http dir \"%s\".", listener->http_dir);
			أرجع;
		}
	}
	user->listener = listener;

	info.user = user;
	info.pt_serv_buf_ناپ = WS_SERV_BUF_ناپ;
	listener->ws_protocol = p;

	lws_سیٹ_log_level(conf->websockets_log_level, log_wrap);

	log__printf(عدم, MOSQ_LOG_INFO, "Opening websockets listen socket on port %d.", listener->پورٹ);
	listener->ws_وچ_init = true;
	listener->ws_context = lws_create_context(&info);
	listener->ws_وچ_init = false;
}

#جے_انت
#جے_انت
