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

#यदि_परिभाषितम् WITH_WEBSOCKETS

#अन्तर्भाव "config.h"

#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#यदि_अन्तः
#अन्तर्भाव "mosquitto_internal.h"
#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "packet_mosq.h"
#अन्तर्भाव "sys_tree.h"
#अन्तर्भाव "util_mosq.h"

#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <errno.h>
#अन्तर्भाव <sys/stat.h>

#यदि_अपरिभाषितम् WIN32
#  include <sys/socket.h>
#यदि_अन्तः

#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
/* Be careful if changing these, if TX is not bigger than SERV then there can
 * be very large write performance penalties.
 */
#परिभाषय WS_SERV_BUF_आकार 4096
#परिभाषय WS_TX_BUF_आकार (WS_SERV_BUF_आकार*2)

स्थिर पूर्णाङ्क callback_mqtt(
		संरचना lws *wsi,
		गणना lws_callback_reasons reason,
		शून्य *user,
		शून्य *अन्तर्,
		size_t len);

स्थिर पूर्णाङ्क callback_http(
		संरचना lws *wsi,
		गणना lws_callback_reasons reason,
		शून्य *user,
		शून्य *अन्तर्,
		size_t len);

गणना mosq_ws_protocols {
	PROTOCOL_HTTP = 0,
	PROTOCOL_MQTT,
	DEMO_PROTOCOL_गणनम्,
};

संरचना libws_http_दत्त {
	FILE *fptr;
};

स्थिर संरचना lws_protocols protocols[] = {
	/* first protocol must always be HTTP handler */
	{
		"http-only",                        /* name */
		callback_http,                      /* lws_callback_function */
		परिमाण (संरचना libws_http_दत्त),    /* per_session_data_size */
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		रिक्त,                               /* user v1.4 on */
		WS_TX_BUF_आकार                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqtt",
		callback_mqtt,
		परिमाण(संरचना libws_mqtt_दत्त),
		0,                                  /* rx_buffer_size */
		1,                                  /* id */
		रिक्त,                               /* user v1.4 on */
		WS_TX_BUF_आकार                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqttv3.1",
		callback_mqtt,
		परिमाण(संरचना libws_mqtt_दत्त),
		0,                                  /* rx_buffer_size */
		2,                                  /* id */
		रिक्त,                               /* user v1.4 on */
		WS_TX_BUF_आकार                      /* tx_packet_size v2.3.0 */
	},
	{
		रिक्त,
		रिक्त,
		0,
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		रिक्त,                               /* user v1.4 on */
		0                                   /* tx_packet_size v2.3.0 */
	}
};


स्थिर शून्य easy_स्थानाङ्कः(पूर्णाङ्क sock, संरचना mosquitto *mosq)
{
	वर्ण स्थानाङ्कः[1024];

	यदि(!net_socket_get_स्थानाङ्कः(sock, स्थानाङ्कः, 1024, &mosq->remote_पत्तनम्)){
		mosq->स्थानाङ्कः = mosquitto_strdup(स्थानाङ्कः);
	}
}


स्थिर पूर्णाङ्क callback_mqtt(
		संरचना lws *wsi,
		गणना lws_callback_reasons reason,
		शून्य *user,
		शून्य *अन्तर्,
		size_t len)
{
	संरचना mosquitto *mosq = रिक्त;
	संरचना mosquitto__packet *packet;
	पूर्णाङ्क गणनम्;
	अचिह्नित पूर्णाङ्क ucount;
	ध्रुव संरचना lws_protocols *p;
	संरचना libws_mqtt_दत्त *u = (संरचना libws_mqtt_दत्त *)user;
	size_t pos;
	uint8_t *buf;
	पूर्णाङ्क rc;
	uint8_t byte;
	वर्ण ip_addr_buff[1024];

	विकल्प(reason){
		पक्ष LWS_CALLBACK_ESTABLISHED:
			mosq = context__init();
			यदि(mosq){
				p = lws_get_protocol(wsi);
				mosq->listener = p->user;
				यदि(!mosq->listener){
					mosquitto_FREE(mosq);
					प्रत्यागम -1;
				}
				mosq->wsi = wsi;
#यदि_परिभाषितम् WITH_TLS
				यदि(अन्तर्){
					mosq->ssl = (SSL *)अन्तर्;
					यदि(!mosq->listener->ssl_ctx){
						mosq->listener->ssl_ctx = SSL_get_SSL_CTX(mosq->ssl);
					}
				}
#यदि_अन्तः
				u->mosq = mosq;
			}अन्यथा{
				प्रत्यागम -1;
			}

			easy_स्थानाङ्कः(lws_get_socket_fd(wsi), mosq);
			यदि(!mosq->स्थानाङ्कः){
				/* getpeername and inet_ntop failed and not a bridge */
				mosquitto_FREE(mosq);
				u->mosq = रिक्त;
				प्रत्यागम -1;
			}
			mosq->listener->client_गणनम्++;
			यदि((mosq->listener->max_connections > 0 && mosq->listener->client_गणनम् > mosq->listener->max_connections)
					|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (अचिह्नित पूर्णाङ्क)db.config->global_max_connections)){

				यदि(db.config->connection_messages == true){
					log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", mosq->स्थानाङ्कः);
				}
				mosq->listener->client_गणनम्--;
				mosquitto_FREE(mosq->स्थानाङ्कः);
				mosquitto_FREE(mosq);
				u->mosq = रिक्त;
				प्रत्यागम -1;
			}
			mosq->sock = lws_get_socket_fd(wsi);
			HASH_सङ्कलय(hh_sock, db.contexts_by_sock, sock, परिमाण(mosq->sock), mosq);
			mux__new(mosq);
			भङ्ग;

		पक्ष LWS_CALLBACK_CLOSED:
			यदि(!u){
				प्रत्यागम -1;
			}
			mosq = u->mosq;
			यदि(mosq){
				यदि(mosq->sock != INVALID_SOCKET){
					HASH_DELETE(hh_sock, db.contexts_by_sock, mosq);
					mosq->sock = INVALID_SOCKET;
					mux__delete(mosq);
				}
				mosq->wsi = रिक्त;
#यदि_परिभाषितम् WITH_TLS
				mosq->ssl = रिक्त;
#यदि_अन्तः
				do_disconnect(mosq, MOSQ_ERR_CONN_नष्टम्);
			}
			भङ्ग;

		पक्ष LWS_CALLBACK_SERVER_WRITEABLE:
			यदि(!u){
				प्रत्यागम -1;
			}
			mosq = u->mosq;
			यदि(!mosq){
				प्रत्यागम -1;
			}

			rc = db_सन्देशः_write_inflight_बहिर्गमनम्_latest(mosq);
			यदि(rc){
				प्रत्यागम -1;
			}
			rc = db_सन्देशः_write_queued_बहिर्गमनम्(mosq);
			यदि(rc){
				प्रत्यागम -1;
			}

			यावत्(mosq->बहिर्गमनम्_packet && !lws_send_pipe_choked(mosq->wsi)){
				packet = mosq->बहिर्गमनम्_packet;
				गणनम् = lws_write(wsi, &packet->payload[packet->pos], packet->गन्तव्यम्_process, LWS_WRITE_द्विमान);
				यदि(गणनम् < 0){
					यदि(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						प्रत्यागम -1;
					}
					प्रत्यागम 0;
				}
				ucount = (अचिह्नित पूर्णाङ्क)गणनम्;
#यदि_परिभाषितम् WITH_SYS_वृक्षः
				metrics__int_inc(mosq_गणक_अष्टकानि_प्रेषितम्, ucount);
#यदि_अन्तः
				packet->गन्तव्यम्_process -= ucount;
				packet->pos += ucount;
				यदि(packet->गन्तव्यम्_process > 0){
					यदि(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						प्रत्यागम -1;
					}
					भङ्ग;
				}

#यदि_परिभाषितम् WITH_SYS_वृक्षः
				metrics__int_inc(mosq_गणक_messages_प्रेषितम्, 1);
				यदि(((packet->command)&0xF0) == CMD_PUBLISH){
					metrics__int_inc(mosq_गणक_mqtt_publish_प्रेषितम्, 1);
				}
#यदि_अन्तः

				packet_get_अग्र_बहिर्गमनम्(mosq);
				mosquitto_FREE(packet);

				mosq->अग्र_सन्देशः_बहिर्गमनम् = db.now_s + mosq->keepalive;
			}
			यदि(mosq->state == mosq_cs_disconnect_ws
					|| mosq->state == mosq_cs_disconnecting
					|| mosq->state == mosq_cs_disused){

				प्रत्यागम -1;
			}
			यदि(mosq->बहिर्गमनम्_packet){
				lws_callback_on_writable(mosq->wsi);
			}
			भङ्ग;

		पक्ष LWS_CALLBACK_RECEIVE:
			यदि(!u || !u->mosq){
				प्रत्यागम -1;
			}
			mosq = u->mosq;
			pos = 0;
			buf = (uint8_t *)अन्तर्;
			metrics__int_inc(mosq_गणक_अष्टकानि_received, (int64_t)len);
			यावत्(pos < len){
				यदि(!mosq->अन्तर्_packet.command){
					mosq->अन्तर्_packet.command = buf[pos];
					pos++;
					/* Clients must send CONNECT as their first command. */
					यदि(mosq->state == mosq_cs_new && (mosq->अन्तर्_packet.command&0xF0) != CMD_CONNECT){
						प्रत्यागम -1;
					}
				}
				यदि(mosq->अन्तर्_packet.remaining_गणनम् <= 0){
					कुरु{
						यदि(pos == len){
							प्रत्यागम 0;
						}
						byte = buf[pos];
						pos++;

						mosq->अन्तर्_packet.remaining_गणनम्--;
						/* Max 4 bytes length for remaining length as defined by protocol.
						* Anything more likely means a broken/malicious client.
						*/
						यदि(mosq->अन्तर्_packet.remaining_गणनम् < -4){
							प्रत्यागम -1;
						}

						mosq->अन्तर्_packet.remaining_दीर्घता += (byte & 127) * mosq->अन्तर्_packet.remaining_mult;
						mosq->अन्तर्_packet.remaining_mult *= 128;
					}यावत्((byte & 128) != 0);
					mosq->अन्तर्_packet.remaining_गणनम् = (int8_t)(mosq->अन्तर्_packet.remaining_गणनम् * -1);

					यदि(mosq->अन्तर्_packet.remaining_दीर्घता > 0){
						mosq->अन्तर्_packet.payload = mosquitto_malloc(mosq->अन्तर्_packet.remaining_दीर्घता*परिमाण(uint8_t));
						यदि(!mosq->अन्तर्_packet.payload){
							प्रत्यागम -1;
						}
						mosq->अन्तर्_packet.गन्तव्यम्_process = mosq->अन्तर्_packet.remaining_दीर्घता;
					}
				}
				यदि(mosq->अन्तर्_packet.गन्तव्यम्_process>0){
					यदि((uint32_t)len - pos >= mosq->अन्तर्_packet.गन्तव्यम्_process){
						स्मृतिं_प्रतिलिख(&mosq->अन्तर्_packet.payload[mosq->अन्तर्_packet.pos], &buf[pos], mosq->अन्तर्_packet.गन्तव्यम्_process);
						mosq->अन्तर्_packet.pos += mosq->अन्तर्_packet.गन्तव्यम्_process;
						pos += mosq->अन्तर्_packet.गन्तव्यम्_process;
						mosq->अन्तर्_packet.गन्तव्यम्_process = 0;
					}अन्यथा{
						स्मृतिं_प्रतिलिख(&mosq->अन्तर्_packet.payload[mosq->अन्तर्_packet.pos], &buf[pos], len-pos);
						mosq->अन्तर्_packet.pos += (uint32_t)(len-pos);
						mosq->अन्तर्_packet.गन्तव्यम्_process -= (uint32_t)(len-pos);
						प्रत्यागम 0;
					}
				}
				/* All data for this packet is read. */
				mosq->अन्तर्_packet.pos = 0;

#यदि_परिभाषितम् WITH_SYS_वृक्षः
				metrics__int_inc(mosq_गणक_messages_received, 1);
#यदि_अन्तः
				rc = handle__packet(mosq);

				/* Free data and reset values */
				packet__cleanup(&mosq->अन्तर्_packet);

				keepalive__update(mosq);

				यदि(rc && mosq->बहिर्गमनम्_packet){
					यदि(mosq->state != mosq_cs_disconnecting){
						mosquitto_समुच्चयः_state(mosq, mosq_cs_disconnect_ws);
					}
					lws_callback_on_writable(mosq->wsi);
				}अन्यथा यदि(rc){
					do_disconnect(mosq, MOSQ_ERR_CONN_नष्टम्);
					प्रत्यागम -1;
				}
			}
			भङ्ग;

		स्वभाव:
			भङ्ग;
	}

	प्रत्यागम 0;
}


स्थिर वर्ण *http__canonical_filename(
		संरचना lws *wsi,
		ध्रुव वर्ण *अन्तर्,
		ध्रुव वर्ण *http_dir)
{
	size_t inlen, slen;
	वर्ण *filename, *filename_canonical;

	inlen = वाक्यदैर्घ्यम्(अन्तर्);
	यदि(अन्तर्[inlen-1] == '/'){
		slen = वाक्यदैर्घ्यम्(http_dir) + inlen + वाक्यदैर्घ्यम्("/index.html") + 2;
	}अन्यथा{
		slen = वाक्यदैर्घ्यम्(http_dir) + inlen + 2;
	}
	filename = mosquitto_malloc(slen);
	यदि(!filename){
		lws_return_http_स्थितिः(wsi, HTTP_स्थितिः_आन्तरिकम्_SERVER_ERROR, रिक्त);
		प्रत्यागम रिक्त;
	}
	यदि(((वर्ण *)अन्तर्)[inlen-1] == '/'){
		snprintf(filename, slen, "%s%sindex.html", http_dir, (वर्ण *)अन्तर्);
	}अन्यथा{
		snprintf(filename, slen, "%s%s", http_dir, (वर्ण *)अन्तर्);
	}


	/* Get canonical path and check it is within our http_dir */
#यदि_परिभाषितम् WIN32
	filename_canonical = _fullpath(रिक्त, filename, 0);
	mosquitto_FREE(filename);
	यदि(!filename_canonical){
		lws_return_http_स्थितिः(wsi, HTTP_स्थितिः_आन्तरिकम्_SERVER_ERROR, रिक्त);
		प्रत्यागम रिक्त;
	}
#अन्यथा
	filename_canonical = realpath(filename, रिक्त);
	mosquitto_FREE(filename);
	यदि(!filename_canonical){
		यदि(errno == EACCES){
			lws_return_http_स्थितिः(wsi, HTTP_स्थितिः_FORBIDDEN, रिक्त);
		}अन्यथा यदि(errno == EINVAL || errno == EIO || errno == ELOOP){
			lws_return_http_स्थितिः(wsi, HTTP_स्थितिः_आन्तरिकम्_SERVER_ERROR, रिक्त);
		}अन्यथा यदि(errno == ENAMETOOLONG){
			lws_return_http_स्थितिः(wsi, HTTP_स्थितिः_REQ_URI_TOO_LONG, रिक्त);
		}अन्यथा यदि(errno == ENOENT || errno == ENOTDIR){
			lws_return_http_स्थितिः(wsi, HTTP_स्थितिः_NOT_प्राप्तम्, रिक्त);
		}
		प्रत्यागम रिक्त;
	}
#यदि_अन्तः
	यदि(वाक्ये_तुलय(http_dir, filename_canonical, वाक्यदैर्घ्यम्(http_dir))){
		/* Requested file isn't within http_dir, deny access. */
		SAFE_FREE(filename_canonical);
		lws_return_http_स्थितिः(wsi, HTTP_स्थितिः_FORBIDDEN, रिक्त);
		प्रत्यागम रिक्त;
	}

	प्रत्यागम filename_canonical;
}


स्थिर पूर्णाङ्क callback_http(
		संरचना lws *wsi,
		गणना lws_callback_reasons reason,
		शून्य *user,
		शून्य *अन्तर्,
		size_t len)
{
	संरचना libws_http_दत्त *u = (संरचना libws_http_दत्त *)user;
	संरचना libws_mqtt_hack *hack;
	वर्ण *http_dir;
	size_t buflen;
	size_t wlen;
	पूर्णाङ्क rc;
	वर्ण *filename_canonical;
	अचिह्नित वर्ण buf[4096];
	संरचना stat filestat;
	संरचना mosquitto *mosq;
	संरचना lws_pollargs *pollargs = (संरचना lws_pollargs *)अन्तर्;
	पूर्णाङ्क hlen;

	/* FIXME - ssl cert verification is done here. */

	विकल्प(reason){
		पक्ष LWS_CALLBACK_HTTP:
			यदि(!u){
				प्रत्यागम -1;
			}

			hack = (संरचना libws_mqtt_hack *)lws_context_user(lws_get_context(wsi));
			यदि(!hack){
				प्रत्यागम -1;
			}
			http_dir = hack->http_dir;

			यदि(!http_dir){
				/* http disabled */
				प्रत्यागम -1;
			}

			/* Forbid POST */
			यदि(lws_hdr_योग_दीर्घता(wsi, WSI_TOKEN_POST_URI)){
				lws_return_http_स्थितिः(wsi, HTTP_स्थितिः_विधानम्_NOT_ALLOWED, रिक्त);
				प्रत्यागम -1;
			}

			filename_canonical = http__canonical_filename(wsi, (वर्ण *)अन्तर्, http_dir);
			यदि(!filename_canonical){
				प्रत्यागम -1;
			}

			u->fptr = fopen(filename_canonical, "rb");
			यदि(!u->fptr){
				SAFE_FREE(filename_canonical);
				lws_return_http_स्थितिः(wsi, HTTP_स्थितिः_NOT_प्राप्तम्, रिक्त);
				प्रत्यागम -1;
			}
			यदि(fstat(fileno(u->fptr), &filestat) < 0){
				SAFE_FREE(filename_canonical);
				lws_return_http_स्थितिः(wsi, HTTP_स्थितिः_आन्तरिकम्_SERVER_ERROR, रिक्त);
				fclose(u->fptr);
				u->fptr = रिक्त;
				प्रत्यागम -1;
			}


			यदि((filestat.st_बहुलकम् & S_IFDIR) == S_IFDIR){
				fclose(u->fptr);
				u->fptr = रिक्त;
				SAFE_FREE(filename_canonical);

				/* FIXME - use header functions from lws 2.x */
				buflen = (size_t)snprintf((वर्ण *)buf, 4096, "HTTP/1.0 302 OK\r\n"
						"Location: %s/\r\n\r\n",
						(वर्ण *)अन्तर्);
				प्रत्यागम lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
			}

			यदि((filestat.st_बहुलकम् & S_IFREG) != S_IFREG){
				lws_return_http_स्थितिः(wsi, HTTP_स्थितिः_FORBIDDEN, रिक्त);
				fclose(u->fptr);
				u->fptr = रिक्त;
				SAFE_FREE(filename_canonical);
				प्रत्यागम -1;
			}

			log__printf(रिक्त, MOSQ_LOG_DEBUG, "http serving file \"%s\".", filename_canonical);
			SAFE_FREE(filename_canonical);
			/* FIXME - use header functions from lws 2.x */
			buflen = (size_t)snprintf((वर्ण *)buf, 4096, "HTTP/1.0 200 OK\r\n"
					"Server: mosquitto\r\n"
					"Content-Length: %u\r\n\r\n",
					(अचिह्नित पूर्णाङ्क)filestat.st_आकार);
			यदि(lws_write(wsi, buf, buflen, LWS_WRITE_HTTP) < 0){
				fclose(u->fptr);
				u->fptr = रिक्त;
				प्रत्यागम -1;
			}
			lws_callback_on_writable(wsi);
			भङ्ग;

		पक्ष LWS_CALLBACK_HTTP_BODY:
			/* For extra POST data? */
			प्रत्यागम -1;

		पक्ष LWS_CALLBACK_HTTP_BODY_COMPLETION:
			/* For end of extra POST data? */
			प्रत्यागम -1;

		पक्ष LWS_CALLBACK_FILTER_HTTP_CONNECTION:
			/* Access control here */
			प्रत्यागम 0;

#यदि LWS_LIBRARY_संस्करणम्_सङ्ख्या >= 3001000
		पक्ष LWS_CALLBACK_HTTP_CONFIRM_UPGRADE:
			hack = lws_context_user(lws_get_context(wsi));
			यदि(hack->listener->ws_origin_गणनम्){
				hlen = lws_hdr_योग_दीर्घता(wsi, WSI_TOKEN_ORIGIN);
				यदि(hlen <= 0 || hlen >= (पूर्णाङ्क)परिमाण(buf)){
					प्रत्यागम -1;
				}
				प्रति(पूर्णाङ्क i=0; i<hack->listener->ws_origin_गणनम्; i++){
					lws_hdr_copy(wsi, (वर्ण *)buf, परिमाण(buf), WSI_TOKEN_ORIGIN);
					यदि((!वाक्यं_तुलय(hack->listener->ws_origins[i], (वर्ण *)buf))){
						प्रत्यागम 0;
					}
				}
				प्रत्यागम -1;
			}अन्यथा{
				प्रत्यागम 0;
			}
#यदि_अन्तः

		पक्ष LWS_CALLBACK_HTTP_WRITEABLE:
			/* Send our data here */
			यदि(u && u->fptr){
				कुरु{
					buflen = fread(buf, 1, परिमाण(buf), u->fptr);
					यदि(buflen < 1){
						fclose(u->fptr);
						u->fptr = रिक्त;
						प्रत्यागम -1;
					}
					rc = lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
					यदि(rc < 0){
						प्रत्यागम -1;
					}
					wlen = (size_t)rc;
					/* while still active, extend timeout */
					यदि(wlen){
						lws_समुच्चयः_timeout(wsi, PENDING_TIMEOUT_HTTP_CONTENT, 10);
					}
					यदि(wlen < buflen){
						यदि(fseek(u->fptr, (दीर्घ)(buflen-wlen), SEEK_CUR) < 0){
							fclose(u->fptr);
							u->fptr = रिक्त;
							प्रत्यागम -1;
						}
					}अन्यथा{
						यदि(buflen < परिमाण(buf)){
							fclose(u->fptr);
							u->fptr = रिक्त;
						}
					}
				}यावत्(u->fptr && !lws_send_pipe_choked(wsi));
				lws_callback_on_writable(wsi);
			}अन्यथा{
				प्रत्यागम -1;
			}
			भङ्ग;

		पक्ष LWS_CALLBACK_CLOSED:
		पक्ष LWS_CALLBACK_CLOSED_HTTP:
		पक्ष LWS_CALLBACK_HTTP_FILE_COMPLETION:
			यदि(u && u->fptr){
				fclose(u->fptr);
				u->fptr = रिक्त;
			}
			भङ्ग;

		पक्ष LWS_CALLBACK_सङ्कलय_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, परिमाण(pollargs->fd), mosq);
			यदि(mosq){
				यदि(pollargs->events & LWS_POLLOUT){
					mux_सङ्कलय_बहिर्गमनम्(mosq);
				}अन्यथा{
					mux_remove_बहिर्गमनम्(mosq);
				}
			}अन्यथा{
				यदि(pollargs->events & POLLIN){
					/* Assume this is a new listener */
					listeners_सङ्कलय_websockets(lws_get_context(wsi), pollargs->fd);
				}
			}
			भङ्ग;

		पक्ष LWS_CALLBACK_DEL_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, परिमाण(pollargs->fd), mosq);
			यदि(mosq){
				mux__delete(mosq);
			}
			भङ्ग;

		पक्ष LWS_CALLBACK_CHANGE_बहुलकम्_POLL_FD:
			HASH_FIND(hh_sock, db.contexts_by_sock, &pollargs->fd, परिमाण(pollargs->fd), mosq);
			यदि(mosq){
				यदि(pollargs->events & LWS_POLLHUP){
					प्रत्यागम 1;
				}अन्यथा यदि(pollargs->events & LWS_POLLOUT){
					mux_सङ्कलय_बहिर्गमनम्(mosq);
				}अन्यथा{
					mux_remove_बहिर्गमनम्(mosq);
				}
			}
			भङ्ग;

#यदि_परिभाषितम् WITH_TLS
		पक्ष LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION:
			यदि(!len || (SSL_get_verify_परिणाम((SSL *)अन्तर्) != X509_V_OK)){
				प्रत्यागम 1;
			}
			भङ्ग;
#यदि_अन्तः

		स्वभाव:
			प्रत्यागम 0;
	}

	प्रत्यागम 0;
}


स्थिर शून्य log_wrap(पूर्णाङ्क level, ध्रुव वर्ण *पङ्क्तिः)
{
	वर्ण *l = (वर्ण *)पङ्क्तिः;
	अप्रयुक्तम्(level);
	l[वाक्यदैर्घ्यम्(पङ्क्तिः)-1] = '\0'; /* Remove \n */
	log__printf(रिक्त, MOSQ_LOG_WEBSOCKETS, "%s", l);
}


शून्य mosq_websockets_init(संरचना mosquitto__listener *listener, ध्रुव संरचना mosquitto__config *conf)
{
	संरचना lws_context_creation_info info;
	संरचना lws_protocols *p;
	size_t protocol_गणनम्;
	पूर्णाङ्क i;
	संरचना libws_mqtt_hack *user;

	/* Count valid protocols */
	प्रति(protocol_गणनम्=0; protocols[protocol_गणनम्].name; protocol_गणनम्++){
		;
	}

	p = mosquitto_calloc(protocol_गणनम्+1, परिमाण(संरचना lws_protocols));
	यदि(!p){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Out of memory.");
		प्रत्यागम;
	}
	प्रति(i=0; protocols[i].name; i++){
		p[i].name = protocols[i].name;
		p[i].callback = protocols[i].callback;
		p[i].प्रत्येकम्_session_दत्त_आकार = protocols[i].प्रत्येकम्_session_दत्त_आकार;
		p[i].rx_बफर_आकार = protocols[i].rx_बफर_आकार;
		p[i].user = listener;
	}

	स्मृतिं_पूरय(&info, 0, परिमाण(info));
	info.iface = listener->host;
	info.पत्तनम् = listener->पत्तनम्;
	info.protocols = p;
	info.gid = -1;
	info.uid = -1;
#यदि_परिभाषितम् WITH_TLS
	यदि(listener->cafile){
		info.ssl_ca_filepath = listener->cafile;
	}अन्यथा यदि(listener->capath){
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: CA path option is not supported for websockets");
	}
	info.ssl_cert_filepath = listener->certfile;
	info.ssl_वैयक्तिकम्_कुञ्चिका_filepath = listener->keyfile;
	info.ssl_cipher_list = listener->ciphers;
	/* HTTP 1 only, due to HTTP 2 issues in Firefox:
	   https://github.com/eclipse-mosquitto/mosquitto/issues/1211
	*/
	info.alpn = "h1";
#यदि defined(WITH_WEBSOCKETS) && LWS_LIBRARY_संस्करणम्_सङ्ख्या>=3001000
	info.tls1_3_plus_cipher_list = listener->ciphers_tls13;
#यदि_अन्तः
	यदि(listener->require_certificate){
		info.options |= LWS_SERVER_OPTION_REQUIRE_VALID_OPENSSL_CLIENT_CERT;
	}
#यदि_अन्तः

	info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
	यदि(listener->socket_domain == AF_INET){
		info.options |= LWS_SERVER_OPTION_DISABLE_IPV6;
	}
	info.max_http_शीर्षकम्_दत्त = conf->packet_बफर_आकार;

	user = mosquitto_calloc(1, परिमाण(संरचना libws_mqtt_hack));
	यदि(!user){
		mosquitto_FREE(p);
		log__printf(रिक्त, MOSQ_LOG_ERR, "Out of memory.");
		प्रत्यागम;
	}

	यदि(listener->http_dir){
#यदि_परिभाषितम् WIN32
		user->http_dir = _fullpath(रिक्त, listener->http_dir, 0);
#अन्यथा
		user->http_dir = realpath(listener->http_dir, रिक्त);
#यदि_अन्तः
		यदि(!user->http_dir){
			mosquitto_FREE(user);
			mosquitto_FREE(p);
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to open http dir \"%s\".", listener->http_dir);
			प्रत्यागम;
		}
	}
	user->listener = listener;

	info.user = user;
	info.pt_serv_buf_आकार = WS_SERV_BUF_आकार;
	listener->ws_protocol = p;

	lws_समुच्चयः_log_level(conf->websockets_log_level, log_wrap);

	log__printf(रिक्त, MOSQ_LOG_INFO, "Opening websockets listen socket on port %d.", listener->पत्तनम्);
	listener->ws_अन्तर्_init = true;
	listener->ws_context = lws_create_context(&info);
	listener->ws_अन्तर्_init = false;
}

#यदि_अन्तः
#यदि_अन्तः
