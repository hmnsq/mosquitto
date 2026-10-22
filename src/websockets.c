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

#شامل "config.h"

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif
#شامل "mosquitto_internal.h"
#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "packet_mosq.h"
#شامل "sys_tree.h"
#شامل "util_mosq.h"

#شامل <stdlib.h>
#شامل <errno.h>
#شامل <sys/stat.h>

#ifndef WIN32
#  include <sys/socket.h>
#endif

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
/* Be careful if changing these, if TX is not bigger than SERV then there can
 * be very large write performance penalties.
 */
#define WS_SERV_BUF_SIZE 4096
#define WS_TX_BUF_SIZE (WS_SERV_BUF_SIZE*2)

ایستا صحیح callback_mqtt(
		ساختار lws *wsi,
		شمارش lws_callback_reasons reason,
		تهی *user,
		تهی *in,
		size_t len);

ایستا صحیح callback_http(
		ساختار lws *wsi,
		شمارش lws_callback_reasons reason,
		تهی *user,
		تهی *in,
		size_t len);

شمارش mosq_ws_protocols {
	PROTOCOL_HTTP = 0,
	PROTOCOL_MQTT,
	DEMO_PROTOCOL_شمار,
};

ساختار libws_http_data {
	FILE *fptr;
};

ایستا ساختار lws_protocols protocols[] = {
	/* first protocol must always be HTTP handler */
	{
		"http-only",                        /* name */
		callback_http,                      /* lws_callback_function */
		اندازه (ساختار libws_http_data),    /* per_session_data_size */
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		پوچ,                               /* user v1.4 on */
		WS_TX_BUF_SIZE                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqtt",
		callback_mqtt,
		اندازه(ساختار libws_mqtt_data),
		0,                                  /* rx_buffer_size */
		1,                                  /* id */
		پوچ,                               /* user v1.4 on */
		WS_TX_BUF_SIZE                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqttv3.1",
		callback_mqtt,
		اندازه(ساختار libws_mqtt_data),
		0,                                  /* rx_buffer_size */
		2,                                  /* id */
		پوچ,                               /* user v1.4 on */
		WS_TX_BUF_SIZE                      /* tx_packet_size v2.3.0 */
	},
	{
		پوچ,
		پوچ,
		0,
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		پوچ,                               /* user v1.4 on */
		0                                   /* tx_packet_size v2.3.0 */
	}
};


ایستا تهی easy_address(صحیح sock, ساختار mosquitto *mosq)
{
	نویسه address[1024];

	اگر(!net__socket_get_address(sock, address, 1024, &mosq->remote_port)){
		mosq->address = mosquitto_strdup(address);
	}
}


ایستا صحیح callback_mqtt(
		ساختار lws *wsi,
		شمارش lws_callback_reasons reason,
		تهی *user,
		تهی *in,
		size_t len)
{
	ساختار mosquitto *mosq = پوچ;
	ساختار mosquitto__packet *packet;
	صحیح شمار;
	unsigned صحیح ucount;
	ثابت ساختار lws_protocols *p;
	ساختار libws_mqtt_data *u = (ساختار libws_mqtt_data *)user;
	size_t pos;
	uint8_t *buf;
	صحیح rc;
	uint8_t byte;
	نویسه ip_addr_buff[1024];

	انتخاب(reason){
		حالت LWS_CALLBACK_ESTABLISHED:
			mosq = context__init();
			اگر(mosq){
				p = lws_get_protocol(wsi);
				mosq->listener = p->user;
				اگر(!mosq->listener){
					mosquitto_FREE(mosq);
					بازگشت -1;
				}
				mosq->wsi = wsi;
#ifdef WITH_TLS
				اگر(in){
					mosq->ssl = (SSL *)in;
					اگر(!mosq->listener->ssl_ctx){
						mosq->listener->ssl_ctx = SSL_get_SSL_CTX(mosq->ssl);
					}
				}
#endif
				u->mosq = mosq;
			}وگرنه{
				بازگشت -1;
			}

			easy_address(lws_get_socket_fd(wsi), mosq);
			اگر(!mosq->address){
				/* getpeername and inet_ntop failed and not a bridge */
				mosquitto_FREE(mosq);
				u->mosq = پوچ;
				بازگشت -1;
			}
			mosq->listener->client_شمار++;
			اگر((mosq->listener->max_connections > 0 && mosq->listener->client_شمار > mosq->listener->max_connections)
					|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (unsigned صحیح)db.config->global_max_connections)){

				اگر(db.config->connection_messages == true){
					log__printf(پوچ, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", mosq->address);
				}
				mosq->listener->client_شمار--;
				mosquitto_FREE(mosq->address);
				mosquitto_FREE(mosq);
				u->mosq = پوچ;
				بازگشت -1;
			}
			mosq->sock = lws_get_socket_fd(wsi);
			HASH_ADD(hh_sock, db.contexts_by_sock, sock, اندازه(mosq->sock), mosq);
			mux__new(mosq);
			بشکن;

		حالت LWS_CALLBACK_CLOSED:
			اگر(!u){
				بازگشت -1;
			}
			mosq = u->mosq;
			اگر(mosq){
				اگر(mosq->sock != INVALID_SOCKET){
					HASH_DELETE(hh_sock, db.contexts_by_sock, mosq);
					mosq->sock = INVALID_SOCKET;
					mux__delete(mosq);
				}
				mosq->wsi = پوچ;
#ifdef WITH_TLS
				mosq->ssl = پوچ;
#endif
				do_disconnect(mosq, MOSQ_ERR_CONN_LOST);
			}
			بشکن;

		حالت LWS_CALLBACK_SERVER_WRITEABLE:
			اگر(!u){
				بازگشت -1;
			}
			mosq = u->mosq;
			اگر(!mosq){
				بازگشت -1;
			}

			rc = db__message_write_inflight_out_latest(mosq);
			اگر(rc){
				بازگشت -1;
			}
			rc = db__message_write_queued_out(mosq);
			اگر(rc){
				بازگشت -1;
			}

			وقتی(mosq->out_packet && !lws_send_pipe_choked(mosq->wsi)){
				packet = mosq->out_packet;
				شمار = lws_write(wsi, &packet->payload[packet->pos], packet->to_process, LWS_WRITE_دودویی);
				اگر(شمار < 0){
					اگر(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						بازگشت -1;
					}
					بازگشت 0;
				}
				ucount = (unsigned صحیح)شمار;
#ifdef WITH_SYS_درخت
				metrics__int_inc(mosq_counter_bytes_sent, ucount);
#endif
				packet->to_process -= ucount;
				packet->pos += ucount;
				اگر(packet->to_process > 0){
					اگر(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						بازگشت -1;
					}
					بشکن;
				}

#ifdef WITH_SYS_درخت
				metrics__int_inc(mosq_counter_messages_sent, 1);
				اگر(((packet->command)&0xF0) == CMD_PUBLISH){
					metrics__int_inc(mosq_counter_mqtt_publish_sent, 1);
				}
#endif

				packet__get_next_out(mosq);
				mosquitto_FREE(packet);

				mosq->next_پیام_out = db.now_s + mosq->keepalive;
			}
			اگر(mosq->state == mosq_cs_disconnect_ws
					|| mosq->state == mosq_cs_disconnecting
					|| mosq->state == mosq_cs_disused){

				بازگشت -1;
			}
			اگر(mosq->out_packet){
				lws_callback_on_writable(mosq->wsi);
			}
			بشکن;

		حالت LWS_CALLBACK_RECEIVE:
			اگر(!u || !u->mosq){
				بازگشت -1;
			}
			mosq = u->mosq;
			pos = 0;
			buf = (uint8_t *)in;
			metrics__int_inc(mosq_counter_bytes_received, (int64_t)len);
			وقتی(pos < len){
				اگر(!mosq->in_packet.command){
					mosq->in_packet.command = buf[pos];
					pos++;
					/* Clients must send CONNECT as their first command. */
					اگر(mosq->state == mosq_cs_new && (mosq->in_packet.command&0xF0) != CMD_CONNECT){
						بازگشت -1;
					}
				}
				اگر(mosq->in_packet.remaining_شمار <= 0){
					انجامبده{
						اگر(pos == len){
							بازگشت 0;
						}
						byte = buf[pos];
						pos++;

						mosq->in_packet.remaining_شمار--;
						/* Max 4 bytes length for remaining length as defined by protocol.
						* Anything more likely means a broken/malicious client.
						*/
						اگر(mosq->in_packet.remaining_شمار < -4){
							بازگشت -1;
						}

						mosq->in_packet.remaining_length += (byte & 127) * mosq->in_packet.remaining_mult;
						mosq->in_packet.remaining_mult *= 128;
					}وقتی((byte & 128) != 0);
					mosq->in_packet.remaining_شمار = (int8_t)(mosq->in_packet.remaining_شمار * -1);

					اگر(mosq->in_packet.remaining_length > 0){
						mosq->in_packet.payload = mosquitto_malloc(mosq->in_packet.remaining_length*اندازه(uint8_t));
						اگر(!mosq->in_packet.payload){
							بازگشت -1;
						}
						mosq->in_packet.to_process = mosq->in_packet.remaining_length;
					}
				}
				اگر(mosq->in_packet.to_process>0){
					اگر((uint32_t)len - pos >= mosq->in_packet.to_process){
						memcpy(&mosq->in_packet.payload[mosq->in_packet.pos], &buf[pos], mosq->in_packet.to_process);
						mosq->in_packet.pos += mosq->in_packet.to_process;
						pos += mosq->in_packet.to_process;
						mosq->in_packet.to_process = 0;
					}وگرنه{
						memcpy(&mosq->in_packet.payload[mosq->in_packet.pos], &buf[pos], len-pos);
						mosq->in_packet.pos += (uint32_t)(len-pos);
						mosq->in_packet.to_process -= (uint32_t)(len-pos);
						بازگشت 0;
					}
				}
				/* All data for this packet is read. */
				mosq->in_packet.pos = 0;

#ifdef WITH_SYS_درخت
				metrics__int_inc(mosq_counter_messages_received, 1);
#endif
				rc = handle__packet(mosq);

				/* Free data and reset values */
				packet__cleanup(&mosq->in_packet);

				keepalive__update(mosq);

				اگر(rc && mosq->out_packet){
					اگر(mosq->state != mosq_cs_disconnecting){
						mosquitto__set_state(mosq, mosq_cs_disconnect_ws);
					}
					lws_callback_on_writable(mosq->wsi);
				}وگرنه اگر(rc){
					do_disconnect(mosq, MOSQ_ERR_CONN_LOST);
					بازگشت -1;
				}
			}
			بشکن;

		پیشفرض:
			بشکن;
	}

	بازگشت 0;
}


ایستا نویسه *http__canonical_filename(
		ساختار lws *wsi,
		ثابت نویسه *in,
		ثابت نویسه *http_dir)
{
	size_t inlen, slen;
	نویسه *filename, *filename_canonical;

	inlen = strlen(in);
	اگر(in[inlen-1] == '/'){
		slen = strlen(http_dir) + inlen + strlen("/index.html") + 2;
	}وگرنه{
		slen = strlen(http_dir) + inlen + 2;
	}
	filename = mosquitto_malloc(slen);
	اگر(!filename){
		lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, پوچ);
		بازگشت پوچ;
	}
	اگر(((نویسه *)in)[inlen-1] == '/'){
		snprintf(filename, slen, "%s%sindex.html", http_dir, (نویسه *)in);
	}وگرنه{
		snprintf(filename, slen, "%s%s", http_dir, (نویسه *)in);
	}


	/* Get canonical path and check it is within our http_dir */
#ifdef WIN32
	filename_canonical = _fullpath(پوچ, filename, 0);
	mosquitto_FREE(filename);
	اگر(!filename_canonical){
		lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, پوچ);
		بازگشت پوچ;
	}
#وگرنه
	filename_canonical = realpath(filename, پوچ);
	mosquitto_FREE(filename);
	اگر(!filename_canonical){
		اگر(errno == EACCES){
			lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, پوچ);
		}وگرنه اگر(errno == EINVAL || errno == EIO || errno == ELOOP){
			lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, پوچ);
		}وگرنه اگر(errno == ENAMETOOLONG){
			lws_return_http_status(wsi, HTTP_STATUS_REQ_URI_TOO_LONG, پوچ);
		}وگرنه اگر(errno == ENOENT || errno == ENOTDIR){
			lws_return_http_status(wsi, HTTP_STATUS_NOT_FOUND, پوچ);
		}
		بازگشت پوچ;
	}
#endif
	اگر(strncmp(http_dir, filename_canonical, strlen(http_dir))){
		/* Requested file isn't within http_dir, deny access. */
		SAFE_FREE(filename_canonical);
		lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, پوچ);
		بازگشت پوچ;
	}

	بازگشت filename_canonical;
}


ایستا صحیح callback_http(
		ساختار lws *wsi,
		شمارش lws_callback_reasons reason,
		تهی *user,
		تهی *in,
		size_t len)
{
	ساختار libws_http_data *u = (ساختار libws_http_data *)user;
	ساختار libws_mqtt_hack *hack;
	نویسه *http_dir;
	size_t buflen;
	size_t wlen;
	صحیح rc;
	نویسه *filename_canonical;
	unsigned نویسه buf[4096];
	ساختار stat filestat;
	ساختار mosquitto *mosq;
	ساختار lws_pollargs *pollargs = (ساختار lws_pollargs *)in;
	صحیح hlen;

	/* FIXME - ssl cert verification is done here. */

	انتخاب(reason){
		حالت LWS_CALLBACK_HTTP:
			اگر(!u){
				بازگشت -1;
			}

			hack = (ساختار libws_mqtt_hack *)lws_context_user(lws_get_context(wsi));
			اگر(!hack){
				بازگشت -1;
			}
			http_dir = hack->http_dir;

			اگر(!http_dir){
				/* http disabled */
				بازگشت -1;
			}

			/* Forbid POST */
			اگر(lws_hdr_total_length(wsi, WSI_TOKEN_POST_URI)){
				lws_return_http_status(wsi, HTTP_STATUS_METHOD_NOT_ALLOWED, پوچ);
				بازگشت -1;
			}

			filename_canonical = http__canonical_filename(wsi, (نویسه *)in, http_dir);
			اگر(!filename_canonical){
				بازگشت -1;
			}

			u->fptr = fopen(filename_canonical, "rb");
			اگر(!u->fptr){
				SAFE_FREE(filename_canonical);
				lws_return_http_status(wsi, HTTP_STATUS_NOT_FOUND, پوچ);
				بازگشت -1;
			}
			اگر(fstat(fileno(u->fptr), &filestat) < 0){
				SAFE_FREE(filename_canonical);
				lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, پوچ);
				fclose(u->fptr);
				u->fptr = پوچ;
				بازگشت -1;
			}


			اگر((filestat.st_mode & S_IFDIR) == S_IFDIR){
				fclose(u->fptr);
				u->fptr = پوچ;
				SAFE_FREE(filename_canonical);

				/* FIXME - use header functions from lws 2.x */
				buflen = (size_t)snprintf((نویسه *)buf, 4096, "HTTP/1.0 302 OK\r\n"
						"Location: %s/\r\n\r\n",
						(نویسه *)in);
				بازگشت lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
			}

			اگر((filestat.st_mode & S_IFREG) != S_IFREG){
				lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, پوچ);
				fclose(u->fptr);
				u->fptr = پوچ;
				SAFE_FREE(filename_canonical);
				بازگشت -1;
			}

			log__printf(پوچ, MOSQ_LOG_DEBUG, "http serving file \"%s\".", filename_canonical);
			SAFE_FREE(filename_canonical);
			/* FIXME - use header functions from lws 2.x */
			buflen = (size_t)snprintf((نویسه *)buf, 4096, "HTTP/1.0 200 OK\r\n"
					"Server: mosquitto\r\n"
					"Content-Length: %u\r\n\r\n",
					(unsigned صحیح)filestat.st_size);
			اگر(lws_write(wsi, buf, buflen, LWS_WRITE_HTTP) < 0){
				fclose(u->fptr);
				u->fptr = پوچ;
				بازگشت -1;
			}
			lws_callback_on_writable(wsi);
			بشکن;

		حالت LWS_CALLBACK_HTTP_BODY:
			/* For extra POST data? */
			بازگشت -1;

		حالت LWS_CALLBACK_HTTP_BODY_COMPLETION:
			/* For end of extra POST data? */
			بازگشت -1;

		حالت LWS_CALLBACK_FILTER_HTTP_CONNECTION:
			/* Access control here */
			بازگشت 0;

#اگر LWS_LIBRARY_VERSION_عدد >= 3001000
		حالت LWS_CALLBACK_HTTP_CONFIRM_UPGRADE:
			hack = lws_context_user(lws_get_context(wsi));
			اگر(hack->listener->ws_origin_شمار){
				hlen = lws_hdr_total_length(wsi, WSI_TOKEN_ORIGIN);
				اگر(hlen <= 0 || hlen >= (صحیح)اندازه(buf)){
					بازگشت -1;
				}
				برای(صحیح i=0; i<hack->listener->ws_origin_شمار; i++){
					lws_hdr_copy(wsi, (نویسه *)buf, اندازه(buf), WSI_TOKEN_ORIGIN);
					اگر((!strcmp(hack->listener->ws_origins[i], (نویسه *)buf))){
						بازگشت 0;
					}
				}
				بازگشت -1;
			}وگرنه{
				بازگشت 0;
			}
#endif

		حالت LWS_CALLBACK_HTTP_WRITEABLE:
			/* Send our data here */
			اگر(u && u->fptr){
				انجامبده{
					buflen = fread(buf, 1, اندازه(buf), u->fptr);
					اگر(buflen < 1){
						fclose(u->fptr);
						u->fptr = پوچ;
						بازگشت -1;
					}
					rc = lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
					اگر(rc < 0){
						بازگشت -1;
					}
					wlen = (size_t)rc;
					/* while still active, extend timeout */
					اگر(wlen){
						lws_set_timeout(wsi, PENDING_TIMEOUT_HTTP_CONTENT, 10);
					}
					اگر(wlen < buflen){
						اگر(fseek(u->fptr, (long)(buflen-wlen), SEEK_CUR) < 0){
							fclose(u->fptr);
							u->fptr = پوچ;
							بازگشت -1;
						}
					}وگرنه{
						اگر(buflen < اندازه(buf)){
							fclose(u->fptr);
							u->fptr = پوچ;
						}
					}
				}وقتی(u->fptr && !lws_send_pipe_choked(wsi));
				lws_callback_on_writable(wsi);
			}وگرنه{
				بازگشت -1;
			}
			بشکن;

		حالت LWS_CALLBACK_CLOSED:
		حالت LWS_CALLBACK_CLOSED_HTTP:
		حالت LWS_CALLBACK_HTTP_FILE_COMPLETION:
			اگر(u && u->fptr){
				fclose(u->fptr);
				u->fptr = پوچ;
			}
			بشکن;

		حالت LWS_CALLBACK_ADD_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, اندازه(pollargs->fd), mosq);
			اگر(mosq){
				اگر(pollargs->events & LWS_POLLOUT){
					mux__add_out(mosq);
				}وگرنه{
					mux__remove_out(mosq);
				}
			}وگرنه{
				اگر(pollargs->events & POLLIN){
					/* Assume this is a new listener */
					listeners__add_websockets(lws_get_context(wsi), pollargs->fd);
				}
			}
			بشکن;

		حالت LWS_CALLBACK_DEL_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, اندازه(pollargs->fd), mosq);
			اگر(mosq){
				mux__delete(mosq);
			}
			بشکن;

		حالت LWS_CALLBACK_CHANGE_MODE_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, اندازه(pollargs->fd), mosq);
			اگر(mosq){
				اگر(pollargs->events & LWS_POLLHUP){
					بازگشت 1;
				}وگرنه اگر(pollargs->events & LWS_POLLOUT){
					mux__add_out(mosq);
				}وگرنه{
					mux__remove_out(mosq);
				}
			}
			بشکن;

#ifdef WITH_TLS
		حالت LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION:
			اگر(!len || (SSL_get_verify_result((SSL *)in) != X509_V_OK)){
				بازگشت 1;
			}
			بشکن;
#endif

		پیشفرض:
			بازگشت 0;
	}

	بازگشت 0;
}


ایستا تهی log_wrap(صحیح level, ثابت نویسه *سطر)
{
	نویسه *l = (نویسه *)سطر;
	UNUSED(level);
	l[strlen(سطر)-1] = '\0'; /* Remove \n */
	log__printf(پوچ, MOSQ_LOG_WEBSOCKETS, "%s", l);
}


تهی mosq_websockets_init(ساختار mosquitto__listener *listener, ثابت ساختار mosquitto__config *conf)
{
	ساختار lws_context_creation_info info;
	ساختار lws_protocols *p;
	size_t protocol_شمار;
	صحیح i;
	ساختار libws_mqtt_hack *user;

	/* Count valid protocols */
	برای(protocol_شمار=0; protocols[protocol_شمار].name; protocol_شمار++){
		;
	}

	p = mosquitto_calloc(protocol_شمار+1, اندازه(ساختار lws_protocols));
	اگر(!p){
		log__printf(پوچ, MOSQ_LOG_ERR, "Out of memory.");
		بازگشت;
	}
	برای(i=0; protocols[i].name; i++){
		p[i].name = protocols[i].name;
		p[i].callback = protocols[i].callback;
		p[i].per_session_data_size = protocols[i].per_session_data_size;
		p[i].rx_buffer_size = protocols[i].rx_buffer_size;
		p[i].user = listener;
	}

	memset(&info, 0, اندازه(info));
	info.iface = listener->host;
	info.port = listener->port;
	info.protocols = p;
	info.gid = -1;
	info.uid = -1;
#ifdef WITH_TLS
	اگر(listener->cafile){
		info.ssl_ca_filepath = listener->cafile;
	}وگرنه اگر(listener->capath){
		log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: CA path option is not supported for websockets");
	}
	info.ssl_cert_filepath = listener->certfile;
	info.ssl_private_کلید_filepath = listener->keyfile;
	info.ssl_cipher_list = listener->ciphers;
	/* HTTP 1 only, due to HTTP 2 issues in Firefox:
	   https://github.com/eclipse-mosquitto/mosquitto/issues/1211
	*/
	info.alpn = "h1";
#اگر defined(WITH_WEBSOCKETS) && LWS_LIBRARY_VERSION_عدد>=3001000
	info.tls1_3_plus_cipher_list = listener->ciphers_tls13;
#endif
	اگر(listener->require_certificate){
		info.options |= LWS_SERVER_OPTION_REQUIRE_VALID_OPENSSL_CLIENT_CERT;
	}
#endif

	info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
	اگر(listener->socket_domain == AF_INET){
		info.options |= LWS_SERVER_OPTION_DISABLE_IPV6;
	}
	info.max_http_header_data = conf->packet_buffer_size;

	user = mosquitto_calloc(1, اندازه(ساختار libws_mqtt_hack));
	اگر(!user){
		mosquitto_FREE(p);
		log__printf(پوچ, MOSQ_LOG_ERR, "Out of memory.");
		بازگشت;
	}

	اگر(listener->http_dir){
#ifdef WIN32
		user->http_dir = _fullpath(پوچ, listener->http_dir, 0);
#وگرنه
		user->http_dir = realpath(listener->http_dir, پوچ);
#endif
		اگر(!user->http_dir){
			mosquitto_FREE(user);
			mosquitto_FREE(p);
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to open http dir \"%s\".", listener->http_dir);
			بازگشت;
		}
	}
	user->listener = listener;

	info.user = user;
	info.pt_serv_buf_size = WS_SERV_BUF_SIZE;
	listener->ws_protocol = p;

	lws_set_log_level(conf->websockets_log_level, log_wrap);

	log__printf(پوچ, MOSQ_LOG_INFO, "Opening websockets listen socket on port %d.", listener->port);
	listener->ws_in_init = true;
	listener->ws_context = lws_create_context(&info);
	listener->ws_in_init = false;
}

#endif
#endif
