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

#અન્તર્ભાવ "config.h"

#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif
#અન્તર્ભાવ "mosquitto_internal.h"
#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"
#અન્તર્ભાવ "packet_mosq.h"
#અન્તર્ભાવ "sys_tree.h"
#અન્તર્ભાવ "util_mosq.h"

#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <errno.h>
#અન્તર્ભાવ <sys/stat.h>

#ifndef WIN32
#  include <sys/socket.h>
#endif

#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
/* Be careful if changing these, if TX is not bigger than SERV then there can
 * be very large write performance penalties.
 */
#define WS_SERV_BUF_SIZE 4096
#define WS_TX_BUF_SIZE (WS_SERV_BUF_SIZE*2)

સ્થિર પૂર્ણાંક callback_mqtt(
		સંરચના lws *wsi,
		ગણના lws_callback_reasons reason,
		શૂન્ય *user,
		શૂન્ય *in,
		size_t len);

સ્થિર પૂર્ણાંક callback_http(
		સંરચના lws *wsi,
		ગણના lws_callback_reasons reason,
		શૂન્ય *user,
		શૂન્ય *in,
		size_t len);

ગણના mosq_ws_protocols {
	PROTOCOL_HTTP = 0,
	PROTOCOL_MQTT,
	DEMO_PROTOCOL_ગણતરી,
};

સંરચના libws_http_data {
	FILE *fptr;
};

સ્થિર સંરચના lws_protocols protocols[] = {
	/* first protocol must always be HTTP handler */
	{
		"http-only",                        /* name */
		callback_http,                      /* lws_callback_function */
		કદ (સંરચના libws_http_data),    /* per_session_data_size */
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		ખાલી,                               /* user v1.4 on */
		WS_TX_BUF_SIZE                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqtt",
		callback_mqtt,
		કદ(સંરચના libws_mqtt_data),
		0,                                  /* rx_buffer_size */
		1,                                  /* id */
		ખાલી,                               /* user v1.4 on */
		WS_TX_BUF_SIZE                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqttv3.1",
		callback_mqtt,
		કદ(સંરચના libws_mqtt_data),
		0,                                  /* rx_buffer_size */
		2,                                  /* id */
		ખાલી,                               /* user v1.4 on */
		WS_TX_BUF_SIZE                      /* tx_packet_size v2.3.0 */
	},
	{
		ખાલી,
		ખાલી,
		0,
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		ખાલી,                               /* user v1.4 on */
		0                                   /* tx_packet_size v2.3.0 */
	}
};


સ્થિર શૂન્ય easy_address(પૂર્ણાંક sock, સંરચના mosquitto *mosq)
{
	અક્ષર address[1024];

	જો(!net__socket_get_address(sock, address, 1024, &mosq->remote_port)){
		mosq->address = mosquitto_strdup(address);
	}
}


સ્થિર પૂર્ણાંક callback_mqtt(
		સંરચના lws *wsi,
		ગણના lws_callback_reasons reason,
		શૂન્ય *user,
		શૂન્ય *in,
		size_t len)
{
	સંરચના mosquitto *mosq = ખાલી;
	સંરચના mosquitto__packet *packet;
	પૂર્ણાંક ગણતરી;
	unsigned પૂર્ણાંક ucount;
	અચલ સંરચના lws_protocols *p;
	સંરચના libws_mqtt_data *u = (સંરચના libws_mqtt_data *)user;
	size_t pos;
	uint8_t *buf;
	પૂર્ણાંક rc;
	uint8_t byte;
	અક્ષર ip_addr_buff[1024];

	પસંદગી(reason){
		સ્થિતિ LWS_CALLBACK_ESTABLISHED:
			mosq = context__init();
			જો(mosq){
				p = lws_get_protocol(wsi);
				mosq->listener = p->user;
				જો(!mosq->listener){
					mosquitto_FREE(mosq);
					પાછા -1;
				}
				mosq->wsi = wsi;
#ifdef WITH_TLS
				જો(in){
					mosq->ssl = (SSL *)in;
					જો(!mosq->listener->ssl_ctx){
						mosq->listener->ssl_ctx = SSL_get_SSL_CTX(mosq->ssl);
					}
				}
#endif
				u->mosq = mosq;
			}નહીંતર{
				પાછા -1;
			}

			easy_address(lws_get_socket_fd(wsi), mosq);
			જો(!mosq->address){
				/* getpeername and inet_ntop failed and not a bridge */
				mosquitto_FREE(mosq);
				u->mosq = ખાલી;
				પાછા -1;
			}
			mosq->listener->client_ગણતરી++;
			જો((mosq->listener->max_connections > 0 && mosq->listener->client_ગણતરી > mosq->listener->max_connections)
					|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (unsigned પૂર્ણાંક)db.config->global_max_connections)){

				જો(db.config->connection_messages == true){
					log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", mosq->address);
				}
				mosq->listener->client_ગણતરી--;
				mosquitto_FREE(mosq->address);
				mosquitto_FREE(mosq);
				u->mosq = ખાલી;
				પાછા -1;
			}
			mosq->sock = lws_get_socket_fd(wsi);
			HASH_ADD(hh_sock, db.contexts_by_sock, sock, કદ(mosq->sock), mosq);
			mux__new(mosq);
			તોડો;

		સ્થિતિ LWS_CALLBACK_CLOSED:
			જો(!u){
				પાછા -1;
			}
			mosq = u->mosq;
			જો(mosq){
				જો(mosq->sock != INVALID_SOCKET){
					HASH_DELETE(hh_sock, db.contexts_by_sock, mosq);
					mosq->sock = INVALID_SOCKET;
					mux__delete(mosq);
				}
				mosq->wsi = ખાલી;
#ifdef WITH_TLS
				mosq->ssl = ખાલી;
#endif
				do_disconnect(mosq, MOSQ_ERR_CONN_LOST);
			}
			તોડો;

		સ્થિતિ LWS_CALLBACK_SERVER_WRITEABLE:
			જો(!u){
				પાછા -1;
			}
			mosq = u->mosq;
			જો(!mosq){
				પાછા -1;
			}

			rc = db__message_write_inflight_out_latest(mosq);
			જો(rc){
				પાછા -1;
			}
			rc = db__message_write_queued_out(mosq);
			જો(rc){
				પાછા -1;
			}

			જ્યારે(mosq->out_packet && !lws_send_pipe_choked(mosq->wsi)){
				packet = mosq->out_packet;
				ગણતરી = lws_write(wsi, &packet->payload[packet->pos], packet->to_process, LWS_WRITE_દ્વિઅંકી);
				જો(ગણતરી < 0){
					જો(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						પાછા -1;
					}
					પાછા 0;
				}
				ucount = (unsigned પૂર્ણાંક)ગણતરી;
#ifdef WITH_SYS_વૃક્ષ
				metrics__int_inc(mosq_counter_bytes_sent, ucount);
#endif
				packet->to_process -= ucount;
				packet->pos += ucount;
				જો(packet->to_process > 0){
					જો(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						પાછા -1;
					}
					તોડો;
				}

#ifdef WITH_SYS_વૃક્ષ
				metrics__int_inc(mosq_counter_messages_sent, 1);
				જો(((packet->command)&0xF0) == CMD_PUBLISH){
					metrics__int_inc(mosq_counter_mqtt_publish_sent, 1);
				}
#endif

				packet__get_next_out(mosq);
				mosquitto_FREE(packet);

				mosq->next_સંદેશ_out = db.now_s + mosq->keepalive;
			}
			જો(mosq->state == mosq_cs_disconnect_ws
					|| mosq->state == mosq_cs_disconnecting
					|| mosq->state == mosq_cs_disused){

				પાછા -1;
			}
			જો(mosq->out_packet){
				lws_callback_on_writable(mosq->wsi);
			}
			તોડો;

		સ્થિતિ LWS_CALLBACK_RECEIVE:
			જો(!u || !u->mosq){
				પાછા -1;
			}
			mosq = u->mosq;
			pos = 0;
			buf = (uint8_t *)in;
			metrics__int_inc(mosq_counter_bytes_received, (int64_t)len);
			જ્યારે(pos < len){
				જો(!mosq->in_packet.command){
					mosq->in_packet.command = buf[pos];
					pos++;
					/* Clients must send CONNECT as their first command. */
					જો(mosq->state == mosq_cs_new && (mosq->in_packet.command&0xF0) != CMD_CONNECT){
						પાછા -1;
					}
				}
				જો(mosq->in_packet.remaining_ગણતરી <= 0){
					કરો{
						જો(pos == len){
							પાછા 0;
						}
						byte = buf[pos];
						pos++;

						mosq->in_packet.remaining_ગણતરી--;
						/* Max 4 bytes length for remaining length as defined by protocol.
						* Anything more likely means a broken/malicious client.
						*/
						જો(mosq->in_packet.remaining_ગણતરી < -4){
							પાછા -1;
						}

						mosq->in_packet.remaining_length += (byte & 127) * mosq->in_packet.remaining_mult;
						mosq->in_packet.remaining_mult *= 128;
					}જ્યારે((byte & 128) != 0);
					mosq->in_packet.remaining_ગણતરી = (int8_t)(mosq->in_packet.remaining_ગણતરી * -1);

					જો(mosq->in_packet.remaining_length > 0){
						mosq->in_packet.payload = mosquitto_malloc(mosq->in_packet.remaining_length*કદ(uint8_t));
						જો(!mosq->in_packet.payload){
							પાછા -1;
						}
						mosq->in_packet.to_process = mosq->in_packet.remaining_length;
					}
				}
				જો(mosq->in_packet.to_process>0){
					જો((uint32_t)len - pos >= mosq->in_packet.to_process){
						memcpy(&mosq->in_packet.payload[mosq->in_packet.pos], &buf[pos], mosq->in_packet.to_process);
						mosq->in_packet.pos += mosq->in_packet.to_process;
						pos += mosq->in_packet.to_process;
						mosq->in_packet.to_process = 0;
					}નહીંતર{
						memcpy(&mosq->in_packet.payload[mosq->in_packet.pos], &buf[pos], len-pos);
						mosq->in_packet.pos += (uint32_t)(len-pos);
						mosq->in_packet.to_process -= (uint32_t)(len-pos);
						પાછા 0;
					}
				}
				/* All data for this packet is read. */
				mosq->in_packet.pos = 0;

#ifdef WITH_SYS_વૃક્ષ
				metrics__int_inc(mosq_counter_messages_received, 1);
#endif
				rc = handle__packet(mosq);

				/* Free data and reset values */
				packet__cleanup(&mosq->in_packet);

				keepalive__update(mosq);

				જો(rc && mosq->out_packet){
					જો(mosq->state != mosq_cs_disconnecting){
						mosquitto__set_state(mosq, mosq_cs_disconnect_ws);
					}
					lws_callback_on_writable(mosq->wsi);
				}નહીંતર જો(rc){
					do_disconnect(mosq, MOSQ_ERR_CONN_LOST);
					પાછા -1;
				}
			}
			તોડો;

		મૂળભૂત:
			તોડો;
	}

	પાછા 0;
}


સ્થિર અક્ષર *http__canonical_filename(
		સંરચના lws *wsi,
		અચલ અક્ષર *in,
		અચલ અક્ષર *http_dir)
{
	size_t inlen, slen;
	અક્ષર *filename, *filename_canonical;

	inlen = strlen(in);
	જો(in[inlen-1] == '/'){
		slen = strlen(http_dir) + inlen + strlen("/index.html") + 2;
	}નહીંતર{
		slen = strlen(http_dir) + inlen + 2;
	}
	filename = mosquitto_malloc(slen);
	જો(!filename){
		lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, ખાલી);
		પાછા ખાલી;
	}
	જો(((અક્ષર *)in)[inlen-1] == '/'){
		snprintf(filename, slen, "%s%sindex.html", http_dir, (અક્ષર *)in);
	}નહીંતર{
		snprintf(filename, slen, "%s%s", http_dir, (અક્ષર *)in);
	}


	/* Get canonical path and check it is within our http_dir */
#ifdef WIN32
	filename_canonical = _fullpath(ખાલી, filename, 0);
	mosquitto_FREE(filename);
	જો(!filename_canonical){
		lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, ખાલી);
		પાછા ખાલી;
	}
#નહીંતર
	filename_canonical = realpath(filename, ખાલી);
	mosquitto_FREE(filename);
	જો(!filename_canonical){
		જો(errno == EACCES){
			lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, ખાલી);
		}નહીંતર જો(errno == EINVAL || errno == EIO || errno == ELOOP){
			lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, ખાલી);
		}નહીંતર જો(errno == ENAMETOOLONG){
			lws_return_http_status(wsi, HTTP_STATUS_REQ_URI_TOO_LONG, ખાલી);
		}નહીંતર જો(errno == ENOENT || errno == ENOTDIR){
			lws_return_http_status(wsi, HTTP_STATUS_NOT_FOUND, ખાલી);
		}
		પાછા ખાલી;
	}
#endif
	જો(strncmp(http_dir, filename_canonical, strlen(http_dir))){
		/* Requested file isn't within http_dir, deny access. */
		SAFE_FREE(filename_canonical);
		lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, ખાલી);
		પાછા ખાલી;
	}

	પાછા filename_canonical;
}


સ્થિર પૂર્ણાંક callback_http(
		સંરચના lws *wsi,
		ગણના lws_callback_reasons reason,
		શૂન્ય *user,
		શૂન્ય *in,
		size_t len)
{
	સંરચના libws_http_data *u = (સંરચના libws_http_data *)user;
	સંરચના libws_mqtt_hack *hack;
	અક્ષર *http_dir;
	size_t buflen;
	size_t wlen;
	પૂર્ણાંક rc;
	અક્ષર *filename_canonical;
	unsigned અક્ષર buf[4096];
	સંરચના stat filestat;
	સંરચના mosquitto *mosq;
	સંરચના lws_pollargs *pollargs = (સંરચના lws_pollargs *)in;
	પૂર્ણાંક hlen;

	/* FIXME - ssl cert verification is done here. */

	પસંદગી(reason){
		સ્થિતિ LWS_CALLBACK_HTTP:
			જો(!u){
				પાછા -1;
			}

			hack = (સંરચના libws_mqtt_hack *)lws_context_user(lws_get_context(wsi));
			જો(!hack){
				પાછા -1;
			}
			http_dir = hack->http_dir;

			જો(!http_dir){
				/* http disabled */
				પાછા -1;
			}

			/* Forbid POST */
			જો(lws_hdr_total_length(wsi, WSI_TOKEN_POST_URI)){
				lws_return_http_status(wsi, HTTP_STATUS_METHOD_NOT_ALLOWED, ખાલી);
				પાછા -1;
			}

			filename_canonical = http__canonical_filename(wsi, (અક્ષર *)in, http_dir);
			જો(!filename_canonical){
				પાછા -1;
			}

			u->fptr = fopen(filename_canonical, "rb");
			જો(!u->fptr){
				SAFE_FREE(filename_canonical);
				lws_return_http_status(wsi, HTTP_STATUS_NOT_FOUND, ખાલી);
				પાછા -1;
			}
			જો(fstat(fileno(u->fptr), &filestat) < 0){
				SAFE_FREE(filename_canonical);
				lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, ખાલી);
				fclose(u->fptr);
				u->fptr = ખાલી;
				પાછા -1;
			}


			જો((filestat.st_mode & S_IFDIR) == S_IFDIR){
				fclose(u->fptr);
				u->fptr = ખાલી;
				SAFE_FREE(filename_canonical);

				/* FIXME - use header functions from lws 2.x */
				buflen = (size_t)snprintf((અક્ષર *)buf, 4096, "HTTP/1.0 302 OK\r\n"
						"Location: %s/\r\n\r\n",
						(અક્ષર *)in);
				પાછા lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
			}

			જો((filestat.st_mode & S_IFREG) != S_IFREG){
				lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, ખાલી);
				fclose(u->fptr);
				u->fptr = ખાલી;
				SAFE_FREE(filename_canonical);
				પાછા -1;
			}

			log__printf(ખાલી, MOSQ_LOG_DEBUG, "http serving file \"%s\".", filename_canonical);
			SAFE_FREE(filename_canonical);
			/* FIXME - use header functions from lws 2.x */
			buflen = (size_t)snprintf((અક્ષર *)buf, 4096, "HTTP/1.0 200 OK\r\n"
					"Server: mosquitto\r\n"
					"Content-Length: %u\r\n\r\n",
					(unsigned પૂર્ણાંક)filestat.st_size);
			જો(lws_write(wsi, buf, buflen, LWS_WRITE_HTTP) < 0){
				fclose(u->fptr);
				u->fptr = ખાલી;
				પાછા -1;
			}
			lws_callback_on_writable(wsi);
			તોડો;

		સ્થિતિ LWS_CALLBACK_HTTP_BODY:
			/* For extra POST data? */
			પાછા -1;

		સ્થિતિ LWS_CALLBACK_HTTP_BODY_COMPLETION:
			/* For end of extra POST data? */
			પાછા -1;

		સ્થિતિ LWS_CALLBACK_FILTER_HTTP_CONNECTION:
			/* Access control here */
			પાછા 0;

#જો LWS_LIBRARY_VERSION_સંખ્યા >= 3001000
		સ્થિતિ LWS_CALLBACK_HTTP_CONFIRM_UPGRADE:
			hack = lws_context_user(lws_get_context(wsi));
			જો(hack->listener->ws_origin_ગણતરી){
				hlen = lws_hdr_total_length(wsi, WSI_TOKEN_ORIGIN);
				જો(hlen <= 0 || hlen >= (પૂર્ણાંક)કદ(buf)){
					પાછા -1;
				}
				માટે(પૂર્ણાંક i=0; i<hack->listener->ws_origin_ગણતરી; i++){
					lws_hdr_copy(wsi, (અક્ષર *)buf, કદ(buf), WSI_TOKEN_ORIGIN);
					જો((!strcmp(hack->listener->ws_origins[i], (અક્ષર *)buf))){
						પાછા 0;
					}
				}
				પાછા -1;
			}નહીંતર{
				પાછા 0;
			}
#endif

		સ્થિતિ LWS_CALLBACK_HTTP_WRITEABLE:
			/* Send our data here */
			જો(u && u->fptr){
				કરો{
					buflen = fread(buf, 1, કદ(buf), u->fptr);
					જો(buflen < 1){
						fclose(u->fptr);
						u->fptr = ખાલી;
						પાછા -1;
					}
					rc = lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
					જો(rc < 0){
						પાછા -1;
					}
					wlen = (size_t)rc;
					/* while still active, extend timeout */
					જો(wlen){
						lws_set_timeout(wsi, PENDING_TIMEOUT_HTTP_CONTENT, 10);
					}
					જો(wlen < buflen){
						જો(fseek(u->fptr, (long)(buflen-wlen), SEEK_CUR) < 0){
							fclose(u->fptr);
							u->fptr = ખાલી;
							પાછા -1;
						}
					}નહીંતર{
						જો(buflen < કદ(buf)){
							fclose(u->fptr);
							u->fptr = ખાલી;
						}
					}
				}જ્યારે(u->fptr && !lws_send_pipe_choked(wsi));
				lws_callback_on_writable(wsi);
			}નહીંતર{
				પાછા -1;
			}
			તોડો;

		સ્થિતિ LWS_CALLBACK_CLOSED:
		સ્થિતિ LWS_CALLBACK_CLOSED_HTTP:
		સ્થિતિ LWS_CALLBACK_HTTP_FILE_COMPLETION:
			જો(u && u->fptr){
				fclose(u->fptr);
				u->fptr = ખાલી;
			}
			તોડો;

		સ્થિતિ LWS_CALLBACK_ADD_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, કદ(pollargs->fd), mosq);
			જો(mosq){
				જો(pollargs->events & LWS_POLLOUT){
					mux__add_out(mosq);
				}નહીંતર{
					mux__remove_out(mosq);
				}
			}નહીંતર{
				જો(pollargs->events & POLLIN){
					/* Assume this is a new listener */
					listeners__add_websockets(lws_get_context(wsi), pollargs->fd);
				}
			}
			તોડો;

		સ્થિતિ LWS_CALLBACK_DEL_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, કદ(pollargs->fd), mosq);
			જો(mosq){
				mux__delete(mosq);
			}
			તોડો;

		સ્થિતિ LWS_CALLBACK_CHANGE_MODE_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, કદ(pollargs->fd), mosq);
			જો(mosq){
				જો(pollargs->events & LWS_POLLHUP){
					પાછા 1;
				}નહીંતર જો(pollargs->events & LWS_POLLOUT){
					mux__add_out(mosq);
				}નહીંતર{
					mux__remove_out(mosq);
				}
			}
			તોડો;

#ifdef WITH_TLS
		સ્થિતિ LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION:
			જો(!len || (SSL_get_verify_result((SSL *)in) != X509_V_OK)){
				પાછા 1;
			}
			તોડો;
#endif

		મૂળભૂત:
			પાછા 0;
	}

	પાછા 0;
}


સ્થિર શૂન્ય log_wrap(પૂર્ણાંક level, અચલ અક્ષર *લીટી)
{
	અક્ષર *l = (અક્ષર *)લીટી;
	UNUSED(level);
	l[strlen(લીટી)-1] = '\0'; /* Remove \n */
	log__printf(ખાલી, MOSQ_LOG_WEBSOCKETS, "%s", l);
}


શૂન્ય mosq_websockets_init(સંરચના mosquitto__listener *listener, અચલ સંરચના mosquitto__config *conf)
{
	સંરચના lws_context_creation_info info;
	સંરચના lws_protocols *p;
	size_t protocol_ગણતરી;
	પૂર્ણાંક i;
	સંરચના libws_mqtt_hack *user;

	/* Count valid protocols */
	માટે(protocol_ગણતરી=0; protocols[protocol_ગણતરી].name; protocol_ગણતરી++){
		;
	}

	p = mosquitto_calloc(protocol_ગણતરી+1, કદ(સંરચના lws_protocols));
	જો(!p){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Out of memory.");
		પાછા;
	}
	માટે(i=0; protocols[i].name; i++){
		p[i].name = protocols[i].name;
		p[i].callback = protocols[i].callback;
		p[i].per_session_data_size = protocols[i].per_session_data_size;
		p[i].rx_buffer_size = protocols[i].rx_buffer_size;
		p[i].user = listener;
	}

	memset(&info, 0, કદ(info));
	info.iface = listener->host;
	info.port = listener->port;
	info.protocols = p;
	info.gid = -1;
	info.uid = -1;
#ifdef WITH_TLS
	જો(listener->cafile){
		info.ssl_ca_filepath = listener->cafile;
	}નહીંતર જો(listener->capath){
		log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: CA path option is not supported for websockets");
	}
	info.ssl_cert_filepath = listener->certfile;
	info.ssl_private_ચાવી_filepath = listener->keyfile;
	info.ssl_cipher_list = listener->ciphers;
	/* HTTP 1 only, due to HTTP 2 issues in Firefox:
	   https://github.com/eclipse-mosquitto/mosquitto/issues/1211
	*/
	info.alpn = "h1";
#જો defined(WITH_WEBSOCKETS) && LWS_LIBRARY_VERSION_સંખ્યા>=3001000
	info.tls1_3_plus_cipher_list = listener->ciphers_tls13;
#endif
	જો(listener->require_certificate){
		info.options |= LWS_SERVER_OPTION_REQUIRE_VALID_OPENSSL_CLIENT_CERT;
	}
#endif

	info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
	જો(listener->socket_domain == AF_INET){
		info.options |= LWS_SERVER_OPTION_DISABLE_IPV6;
	}
	info.max_http_header_data = conf->packet_buffer_size;

	user = mosquitto_calloc(1, કદ(સંરચના libws_mqtt_hack));
	જો(!user){
		mosquitto_FREE(p);
		log__printf(ખાલી, MOSQ_LOG_ERR, "Out of memory.");
		પાછા;
	}

	જો(listener->http_dir){
#ifdef WIN32
		user->http_dir = _fullpath(ખાલી, listener->http_dir, 0);
#નહીંતર
		user->http_dir = realpath(listener->http_dir, ખાલી);
#endif
		જો(!user->http_dir){
			mosquitto_FREE(user);
			mosquitto_FREE(p);
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to open http dir \"%s\".", listener->http_dir);
			પાછા;
		}
	}
	user->listener = listener;

	info.user = user;
	info.pt_serv_buf_size = WS_SERV_BUF_SIZE;
	listener->ws_protocol = p;

	lws_set_log_level(conf->websockets_log_level, log_wrap);

	log__printf(ખાલી, MOSQ_LOG_INFO, "Opening websockets listen socket on port %d.", listener->port);
	listener->ws_in_init = true;
	listener->ws_context = lws_create_context(&info);
	listener->ws_in_init = false;
}

#endif
#endif
