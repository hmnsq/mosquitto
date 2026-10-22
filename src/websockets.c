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

#నిర్వచితమైతే WITH_WEBSOCKETS

#అన్తర్భావ "config.h"

#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#అయితే_ముగింపు
#అన్తర్భావ "mosquitto_internal.h"
#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto/mqtt_protocol.h"
#అన్తర్భావ "packet_mosq.h"
#అన్తర్భావ "sys_tree.h"
#అన్తర్భావ "util_mosq.h"

#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <errno.h>
#అన్తర్భావ <sys/stat.h>

#నిర్వచితం_కాకపోతే WIN32
#  include <sys/socket.h>
#అయితే_ముగింపు

#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
/* Be careful if changing these, if TX is not bigger than SERV then there can
 * be very large write performance penalties.
 */
#నిర్వచించు WS_SERV_BUF_కొలత 4096
#నిర్వచించు WS_TX_BUF_కొలత (WS_SERV_BUF_కొలత*2)

స్థిర పూర్ణాంకం callback_mqtt(
		నిర్మాణం lws *wsi,
		గణన lws_callback_reasons reason,
		శూన్య *user,
		శూన్య *లోపల,
		size_t len);

స్థిర పూర్ణాంకం callback_http(
		నిర్మాణం lws *wsi,
		గణన lws_callback_reasons reason,
		శూన్య *user,
		శూన్య *లోపల,
		size_t len);

గణన mosq_ws_protocols {
	PROTOCOL_HTTP = 0,
	PROTOCOL_MQTT,
	DEMO_PROTOCOL_లెక్క,
};

నిర్మాణం libws_http_దత్తాంశం {
	FILE *fptr;
};

స్థిర నిర్మాణం lws_protocols protocols[] = {
	/* first protocol must always be HTTP handler */
	{
		"http-only",                        /* name */
		callback_http,                      /* lws_callback_function */
		పరిమాణం (నిర్మాణం libws_http_దత్తాంశం),    /* per_session_data_size */
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		ఖాళీ,                               /* user v1.4 on */
		WS_TX_BUF_కొలత                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqtt",
		callback_mqtt,
		పరిమాణం(నిర్మాణం libws_mqtt_దత్తాంశం),
		0,                                  /* rx_buffer_size */
		1,                                  /* id */
		ఖాళీ,                               /* user v1.4 on */
		WS_TX_BUF_కొలత                      /* tx_packet_size v2.3.0 */
	},
	{
		"mqttv3.1",
		callback_mqtt,
		పరిమాణం(నిర్మాణం libws_mqtt_దత్తాంశం),
		0,                                  /* rx_buffer_size */
		2,                                  /* id */
		ఖాళీ,                               /* user v1.4 on */
		WS_TX_BUF_కొలత                      /* tx_packet_size v2.3.0 */
	},
	{
		ఖాళీ,
		ఖాళీ,
		0,
		0,                                  /* rx_buffer_size */
		0,                                  /* id */
		ఖాళీ,                               /* user v1.4 on */
		0                                   /* tx_packet_size v2.3.0 */
	}
};


స్థిర శూన్య easy_చిరునామా(పూర్ణాంకం sock, నిర్మాణం mosquitto *mosq)
{
	అక్షరం చిరునామా[1024];

	ఒకవేళ(!net_socket_get_చిరునామా(sock, చిరునామా, 1024, &mosq->remote_పోర్టు)){
		mosq->చిరునామా = mosquitto_strdup(చిరునామా);
	}
}


స్థిర పూర్ణాంకం callback_mqtt(
		నిర్మాణం lws *wsi,
		గణన lws_callback_reasons reason,
		శూన్య *user,
		శూన్య *లోపల,
		size_t len)
{
	నిర్మాణం mosquitto *mosq = ఖాళీ;
	నిర్మాణం mosquitto__packet *packet;
	పూర్ణాంకం లెక్క;
	గుర్తులేని పూర్ణాంకం ucount;
	స్థిరరాశి నిర్మాణం lws_protocols *p;
	నిర్మాణం libws_mqtt_దత్తాంశం *u = (నిర్మాణం libws_mqtt_దత్తాంశం *)user;
	size_t pos;
	uint8_t *buf;
	పూర్ణాంకం rc;
	uint8_t byte;
	అక్షరం ip_addr_buff[1024];

	ఎంపిక(reason){
		స్థితి LWS_CALLBACK_ESTABLISHED:
			mosq = context__init();
			ఒకవేళ(mosq){
				p = lws_get_protocol(wsi);
				mosq->listener = p->user;
				ఒకవేళ(!mosq->listener){
					mosquitto_FREE(mosq);
					తిరిగి -1;
				}
				mosq->wsi = wsi;
#నిర్వచితమైతే WITH_TLS
				ఒకవేళ(లోపల){
					mosq->ssl = (SSL *)లోపల;
					ఒకవేళ(!mosq->listener->ssl_ctx){
						mosq->listener->ssl_ctx = SSL_get_SSL_CTX(mosq->ssl);
					}
				}
#అయితే_ముగింపు
				u->mosq = mosq;
			}లేకపోతే{
				తిరిగి -1;
			}

			easy_చిరునామా(lws_get_socket_fd(wsi), mosq);
			ఒకవేళ(!mosq->చిరునామా){
				/* getpeername and inet_ntop failed and not a bridge */
				mosquitto_FREE(mosq);
				u->mosq = ఖాళీ;
				తిరిగి -1;
			}
			mosq->listener->client_లెక్క++;
			ఒకవేళ((mosq->listener->max_connections > 0 && mosq->listener->client_లెక్క > mosq->listener->max_connections)
					|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (గుర్తులేని పూర్ణాంకం)db.config->global_max_connections)){

				ఒకవేళ(db.config->connection_messages == true){
					log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", mosq->చిరునామా);
				}
				mosq->listener->client_లెక్క--;
				mosquitto_FREE(mosq->చిరునామా);
				mosquitto_FREE(mosq);
				u->mosq = ఖాళీ;
				తిరిగి -1;
			}
			mosq->sock = lws_get_socket_fd(wsi);
			HASH_కూడు(hh_sock, db.contexts_by_sock, sock, పరిమాణం(mosq->sock), mosq);
			mux__new(mosq);
			ఆపు;

		స్థితి LWS_CALLBACK_CLOSED:
			ఒకవేళ(!u){
				తిరిగి -1;
			}
			mosq = u->mosq;
			ఒకవేళ(mosq){
				ఒకవేళ(mosq->sock != INVALID_SOCKET){
					HASH_DELETE(hh_sock, db.contexts_by_sock, mosq);
					mosq->sock = INVALID_SOCKET;
					mux__delete(mosq);
				}
				mosq->wsi = ఖాళీ;
#నిర్వచితమైతే WITH_TLS
				mosq->ssl = ఖాళీ;
#అయితే_ముగింపు
				do_disconnect(mosq, MOSQ_ERR_CONN_కోల్పోయిన);
			}
			ఆపు;

		స్థితి LWS_CALLBACK_SERVER_WRITEABLE:
			ఒకవేళ(!u){
				తిరిగి -1;
			}
			mosq = u->mosq;
			ఒకవేళ(!mosq){
				తిరిగి -1;
			}

			rc = db_సందేశం_write_inflight_బహిర్గతం_latest(mosq);
			ఒకవేళ(rc){
				తిరిగి -1;
			}
			rc = db_సందేశం_write_queued_బహిర్గతం(mosq);
			ఒకవేళ(rc){
				తిరిగి -1;
			}

			అయితే(mosq->బహిర్గతం_packet && !lws_send_pipe_choked(mosq->wsi)){
				packet = mosq->బహిర్గతం_packet;
				లెక్క = lws_write(wsi, &packet->payload[packet->pos], packet->గమ్యం_process, LWS_WRITE_ద్విమాన);
				ఒకవేళ(లెక్క < 0){
					ఒకవేళ(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						తిరిగి -1;
					}
					తిరిగి 0;
				}
				ucount = (గుర్తులేని పూర్ణాంకం)లెక్క;
#నిర్వచితమైతే WITH_SYS_వృక్షం
				metrics__int_inc(mosq_counter_బైట్లు_పంపిన, ucount);
#అయితే_ముగింపు
				packet->గమ్యం_process -= ucount;
				packet->pos += ucount;
				ఒకవేళ(packet->గమ్యం_process > 0){
					ఒకవేళ(mosq->state == mosq_cs_disconnect_ws
							|| mosq->state == mosq_cs_disconnecting
							|| mosq->state == mosq_cs_disused){

						తిరిగి -1;
					}
					ఆపు;
				}

#నిర్వచితమైతే WITH_SYS_వృక్షం
				metrics__int_inc(mosq_counter_messages_పంపిన, 1);
				ఒకవేళ(((packet->command)&0xF0) == CMD_PUBLISH){
					metrics__int_inc(mosq_counter_mqtt_publish_పంపిన, 1);
				}
#అయితే_ముగింపు

				packet_get_తదుపరి_బహిర్గతం(mosq);
				mosquitto_FREE(packet);

				mosq->తదుపరి_సందేశం_బహిర్గతం = db.now_s + mosq->keepalive;
			}
			ఒకవేళ(mosq->state == mosq_cs_disconnect_ws
					|| mosq->state == mosq_cs_disconnecting
					|| mosq->state == mosq_cs_disused){

				తిరిగి -1;
			}
			ఒకవేళ(mosq->బహిర్గతం_packet){
				lws_callback_on_writable(mosq->wsi);
			}
			ఆపు;

		స్థితి LWS_CALLBACK_RECEIVE:
			ఒకవేళ(!u || !u->mosq){
				తిరిగి -1;
			}
			mosq = u->mosq;
			pos = 0;
			buf = (uint8_t *)లోపల;
			metrics__int_inc(mosq_counter_బైట్లు_received, (int64_t)len);
			అయితే(pos < len){
				ఒకవేళ(!mosq->లోపల_packet.command){
					mosq->లోపల_packet.command = buf[pos];
					pos++;
					/* Clients must send CONNECT as their first command. */
					ఒకవేళ(mosq->state == mosq_cs_new && (mosq->లోపల_packet.command&0xF0) != CMD_CONNECT){
						తిరిగి -1;
					}
				}
				ఒకవేళ(mosq->లోపల_packet.మిగిలిన_లెక్క <= 0){
					చేయి{
						ఒకవేళ(pos == len){
							తిరిగి 0;
						}
						byte = buf[pos];
						pos++;

						mosq->లోపల_packet.మిగిలిన_లెక్క--;
						/* Max 4 bytes length for remaining length as defined by protocol.
						* Anything more likely means a broken/malicious client.
						*/
						ఒకవేళ(mosq->లోపల_packet.మిగిలిన_లెక్క < -4){
							తిరిగి -1;
						}

						mosq->లోపల_packet.మిగిలిన_నిడివి += (byte & 127) * mosq->లోపల_packet.మిగిలిన_mult;
						mosq->లోపల_packet.మిగిలిన_mult *= 128;
					}అయితే((byte & 128) != 0);
					mosq->లోపల_packet.మిగిలిన_లెక్క = (int8_t)(mosq->లోపల_packet.మిగిలిన_లెక్క * -1);

					ఒకవేళ(mosq->లోపల_packet.మిగిలిన_నిడివి > 0){
						mosq->లోపల_packet.payload = mosquitto_malloc(mosq->లోపల_packet.మిగిలిన_నిడివి*పరిమాణం(uint8_t));
						ఒకవేళ(!mosq->లోపల_packet.payload){
							తిరిగి -1;
						}
						mosq->లోపల_packet.గమ్యం_process = mosq->లోపల_packet.మిగిలిన_నిడివి;
					}
				}
				ఒకవేళ(mosq->లోపల_packet.గమ్యం_process>0){
					ఒకవేళ((uint32_t)len - pos >= mosq->లోపల_packet.గమ్యం_process){
						జ్ఞాపకం_నకలు(&mosq->లోపల_packet.payload[mosq->లోపల_packet.pos], &buf[pos], mosq->లోపల_packet.గమ్యం_process);
						mosq->లోపల_packet.pos += mosq->లోపల_packet.గమ్యం_process;
						pos += mosq->లోపల_packet.గమ్యం_process;
						mosq->లోపల_packet.గమ్యం_process = 0;
					}లేకపోతే{
						జ్ఞాపకం_నకలు(&mosq->లోపల_packet.payload[mosq->లోపల_packet.pos], &buf[pos], len-pos);
						mosq->లోపల_packet.pos += (uint32_t)(len-pos);
						mosq->లోపల_packet.గమ్యం_process -= (uint32_t)(len-pos);
						తిరిగి 0;
					}
				}
				/* All data for this packet is read. */
				mosq->లోపల_packet.pos = 0;

#నిర్వచితమైతే WITH_SYS_వృక్షం
				metrics__int_inc(mosq_counter_messages_received, 1);
#అయితే_ముగింపు
				rc = handle__packet(mosq);

				/* Free data and reset values */
				packet__cleanup(&mosq->లోపల_packet);

				keepalive__update(mosq);

				ఒకవేళ(rc && mosq->బహిర్గతం_packet){
					ఒకవేళ(mosq->state != mosq_cs_disconnecting){
						mosquitto_సమితి_state(mosq, mosq_cs_disconnect_ws);
					}
					lws_callback_on_writable(mosq->wsi);
				}లేకపోతే ఒకవేళ(rc){
					do_disconnect(mosq, MOSQ_ERR_CONN_కోల్పోయిన);
					తిరిగి -1;
				}
			}
			ఆపు;

		అప్రమేయ:
			ఆపు;
	}

	తిరిగి 0;
}


స్థిర అక్షరం *http__canonical_filename(
		నిర్మాణం lws *wsi,
		స్థిరరాశి అక్షరం *లోపల,
		స్థిరరాశి అక్షరం *http_dir)
{
	size_t inlen, slen;
	అక్షరం *filename, *filename_canonical;

	inlen = అక్షరశ్రేణి_పొడవు(లోపల);
	ఒకవేళ(లోపల[inlen-1] == '/'){
		slen = అక్షరశ్రేణి_పొడవు(http_dir) + inlen + అక్షరశ్రేణి_పొడవు("/index.html") + 2;
	}లేకపోతే{
		slen = అక్షరశ్రేణి_పొడవు(http_dir) + inlen + 2;
	}
	filename = mosquitto_malloc(slen);
	ఒకవేళ(!filename){
		lws_return_http_స్థితి(wsi, HTTP_స్థితి_అంతర్గత_SERVER_ERROR, ఖాళీ);
		తిరిగి ఖాళీ;
	}
	ఒకవేళ(((అక్షరం *)లోపల)[inlen-1] == '/'){
		snprintf(filename, slen, "%s%sindex.html", http_dir, (అక్షరం *)లోపల);
	}లేకపోతే{
		snprintf(filename, slen, "%s%s", http_dir, (అక్షరం *)లోపల);
	}


	/* Get canonical path and check it is within our http_dir */
#నిర్వచితమైతే WIN32
	filename_canonical = _fullpath(ఖాళీ, filename, 0);
	mosquitto_FREE(filename);
	ఒకవేళ(!filename_canonical){
		lws_return_http_స్థితి(wsi, HTTP_స్థితి_అంతర్గత_SERVER_ERROR, ఖాళీ);
		తిరిగి ఖాళీ;
	}
#లేకపోతే
	filename_canonical = realpath(filename, ఖాళీ);
	mosquitto_FREE(filename);
	ఒకవేళ(!filename_canonical){
		ఒకవేళ(errno == EACCES){
			lws_return_http_స్థితి(wsi, HTTP_స్థితి_FORBIDDEN, ఖాళీ);
		}లేకపోతే ఒకవేళ(errno == EINVAL || errno == EIO || errno == ELOOP){
			lws_return_http_స్థితి(wsi, HTTP_స్థితి_అంతర్గత_SERVER_ERROR, ఖాళీ);
		}లేకపోతే ఒకవేళ(errno == ENAMETOOLONG){
			lws_return_http_స్థితి(wsi, HTTP_స్థితి_REQ_URI_TOO_LONG, ఖాళీ);
		}లేకపోతే ఒకవేళ(errno == ENOENT || errno == ENOTDIR){
			lws_return_http_స్థితి(wsi, HTTP_స్థితి_NOT_దొరికింది, ఖాళీ);
		}
		తిరిగి ఖాళీ;
	}
#అయితే_ముగింపు
	ఒకవేళ(అక్షరశ్రేణి_పోలిక(http_dir, filename_canonical, అక్షరశ్రేణి_పొడవు(http_dir))){
		/* Requested file isn't within http_dir, deny access. */
		సురక్షిత_FREE(filename_canonical);
		lws_return_http_స్థితి(wsi, HTTP_స్థితి_FORBIDDEN, ఖాళీ);
		తిరిగి ఖాళీ;
	}

	తిరిగి filename_canonical;
}


స్థిర పూర్ణాంకం callback_http(
		నిర్మాణం lws *wsi,
		గణన lws_callback_reasons reason,
		శూన్య *user,
		శూన్య *లోపల,
		size_t len)
{
	నిర్మాణం libws_http_దత్తాంశం *u = (నిర్మాణం libws_http_దత్తాంశం *)user;
	నిర్మాణం libws_mqtt_hack *hack;
	అక్షరం *http_dir;
	size_t buflen;
	size_t wlen;
	పూర్ణాంకం rc;
	అక్షరం *filename_canonical;
	గుర్తులేని అక్షరం buf[4096];
	నిర్మాణం stat filestat;
	నిర్మాణం mosquitto *mosq;
	నిర్మాణం lws_pollargs *pollargs = (నిర్మాణం lws_pollargs *)లోపల;
	పూర్ణాంకం hlen;

	/* FIXME - ssl cert verification is done here. */

	ఎంపిక(reason){
		స్థితి LWS_CALLBACK_HTTP:
			ఒకవేళ(!u){
				తిరిగి -1;
			}

			hack = (నిర్మాణం libws_mqtt_hack *)lws_context_user(lws_get_context(wsi));
			ఒకవేళ(!hack){
				తిరిగి -1;
			}
			http_dir = hack->http_dir;

			ఒకవేళ(!http_dir){
				/* http disabled */
				తిరిగి -1;
			}

			/* Forbid POST */
			ఒకవేళ(lws_hdr_మొత్తం_నిడివి(wsi, WSI_TOKEN_POST_URI)){
				lws_return_http_స్థితి(wsi, HTTP_స్థితి_పద్ధతి_NOT_ALLOWED, ఖాళీ);
				తిరిగి -1;
			}

			filename_canonical = http__canonical_filename(wsi, (అక్షరం *)లోపల, http_dir);
			ఒకవేళ(!filename_canonical){
				తిరిగి -1;
			}

			u->fptr = fopen(filename_canonical, "rb");
			ఒకవేళ(!u->fptr){
				సురక్షిత_FREE(filename_canonical);
				lws_return_http_స్థితి(wsi, HTTP_స్థితి_NOT_దొరికింది, ఖాళీ);
				తిరిగి -1;
			}
			ఒకవేళ(fstat(fileno(u->fptr), &filestat) < 0){
				సురక్షిత_FREE(filename_canonical);
				lws_return_http_స్థితి(wsi, HTTP_స్థితి_అంతర్గత_SERVER_ERROR, ఖాళీ);
				fclose(u->fptr);
				u->fptr = ఖాళీ;
				తిరిగి -1;
			}


			ఒకవేళ((filestat.st_బహుళకం & S_IFDIR) == S_IFDIR){
				fclose(u->fptr);
				u->fptr = ఖాళీ;
				సురక్షిత_FREE(filename_canonical);

				/* FIXME - use header functions from lws 2.x */
				buflen = (size_t)snprintf((అక్షరం *)buf, 4096, "HTTP/1.0 302 OK\r\n"
						"Location: %s/\r\n\r\n",
						(అక్షరం *)లోపల);
				తిరిగి lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
			}

			ఒకవేళ((filestat.st_బహుళకం & S_IFREG) != S_IFREG){
				lws_return_http_స్థితి(wsi, HTTP_స్థితి_FORBIDDEN, ఖాళీ);
				fclose(u->fptr);
				u->fptr = ఖాళీ;
				సురక్షిత_FREE(filename_canonical);
				తిరిగి -1;
			}

			log__printf(ఖాళీ, MOSQ_LOG_DEBUG, "http serving file \"%s\".", filename_canonical);
			సురక్షిత_FREE(filename_canonical);
			/* FIXME - use header functions from lws 2.x */
			buflen = (size_t)snprintf((అక్షరం *)buf, 4096, "HTTP/1.0 200 OK\r\n"
					"Server: mosquitto\r\n"
					"Content-Length: %u\r\n\r\n",
					(గుర్తులేని పూర్ణాంకం)filestat.st_కొలత);
			ఒకవేళ(lws_write(wsi, buf, buflen, LWS_WRITE_HTTP) < 0){
				fclose(u->fptr);
				u->fptr = ఖాళీ;
				తిరిగి -1;
			}
			lws_callback_on_writable(wsi);
			ఆపు;

		స్థితి LWS_CALLBACK_HTTP_BODY:
			/* For extra POST data? */
			తిరిగి -1;

		స్థితి LWS_CALLBACK_HTTP_BODY_COMPLETION:
			/* For end of extra POST data? */
			తిరిగి -1;

		స్థితి LWS_CALLBACK_FILTER_HTTP_CONNECTION:
			/* Access control here */
			తిరిగి 0;

#ఒకవేళ LWS_LIBRARY_సంస్కరణ_సంఖ్య >= 3001000
		స్థితి LWS_CALLBACK_HTTP_CONFIRM_UPGRADE:
			hack = lws_context_user(lws_get_context(wsi));
			ఒకవేళ(hack->listener->ws_origin_లెక్క){
				hlen = lws_hdr_మొత్తం_నిడివి(wsi, WSI_TOKEN_ORIGIN);
				ఒకవేళ(hlen <= 0 || hlen >= (పూర్ణాంకం)పరిమాణం(buf)){
					తిరిగి -1;
				}
				కొరకు(పూర్ణాంకం i=0; i<hack->listener->ws_origin_లెక్క; i++){
					lws_hdr_copy(wsi, (అక్షరం *)buf, పరిమాణం(buf), WSI_TOKEN_ORIGIN);
					ఒకవేళ((!strcmp(hack->listener->ws_origins[i], (అక్షరం *)buf))){
						తిరిగి 0;
					}
				}
				తిరిగి -1;
			}లేకపోతే{
				తిరిగి 0;
			}
#అయితే_ముగింపు

		స్థితి LWS_CALLBACK_HTTP_WRITEABLE:
			/* Send our data here */
			ఒకవేళ(u && u->fptr){
				చేయి{
					buflen = fread(buf, 1, పరిమాణం(buf), u->fptr);
					ఒకవేళ(buflen < 1){
						fclose(u->fptr);
						u->fptr = ఖాళీ;
						తిరిగి -1;
					}
					rc = lws_write(wsi, buf, buflen, LWS_WRITE_HTTP);
					ఒకవేళ(rc < 0){
						తిరిగి -1;
					}
					wlen = (size_t)rc;
					/* while still active, extend timeout */
					ఒకవేళ(wlen){
						lws_సమితి_timeout(wsi, PENDING_TIMEOUT_HTTP_CONTENT, 10);
					}
					ఒకవేళ(wlen < buflen){
						ఒకవేళ(fseek(u->fptr, (దీర్ఘ)(buflen-wlen), SEEK_CUR) < 0){
							fclose(u->fptr);
							u->fptr = ఖాళీ;
							తిరిగి -1;
						}
					}లేకపోతే{
						ఒకవేళ(buflen < పరిమాణం(buf)){
							fclose(u->fptr);
							u->fptr = ఖాళీ;
						}
					}
				}అయితే(u->fptr && !lws_send_pipe_choked(wsi));
				lws_callback_on_writable(wsi);
			}లేకపోతే{
				తిరిగి -1;
			}
			ఆపు;

		స్థితి LWS_CALLBACK_CLOSED:
		స్థితి LWS_CALLBACK_CLOSED_HTTP:
		స్థితి LWS_CALLBACK_HTTP_FILE_COMPLETION:
			ఒకవేళ(u && u->fptr){
				fclose(u->fptr);
				u->fptr = ఖాళీ;
			}
			ఆపు;

		స్థితి LWS_CALLBACK_కూడు_POLL_FD:
			HASH_కనుగొను(hh_sock, db.contexts_by_sock, &pollargs->fd, పరిమాణం(pollargs->fd), mosq);
			ఒకవేళ(mosq){
				ఒకవేళ(pollargs->events & LWS_POLLOUT){
					mux_కూడు_బహిర్గతం(mosq);
				}లేకపోతే{
					mux_remove_బహిర్గతం(mosq);
				}
			}లేకపోతే{
				ఒకవేళ(pollargs->events & POLLIN){
					/* Assume this is a new listener */
					listeners_కూడు_websockets(lws_get_context(wsi), pollargs->fd);
				}
			}
			ఆపు;

		స్థితి LWS_CALLBACK_DEL_POLL_FD:
			HASH_కనుగొను(hh_sock, db.contexts_by_sock, &pollargs->fd, పరిమాణం(pollargs->fd), mosq);
			ఒకవేళ(mosq){
				mux__delete(mosq);
			}
			ఆపు;

		స్థితి LWS_CALLBACK_CHANGE_బహుళకం_POLL_FD:
			HASH_కనుగొను(hh_sock, db.contexts_by_sock, &pollargs->fd, పరిమాణం(pollargs->fd), mosq);
			ఒకవేళ(mosq){
				ఒకవేళ(pollargs->events & LWS_POLLHUP){
					తిరిగి 1;
				}లేకపోతే ఒకవేళ(pollargs->events & LWS_POLLOUT){
					mux_కూడు_బహిర్గతం(mosq);
				}లేకపోతే{
					mux_remove_బహిర్గతం(mosq);
				}
			}
			ఆపు;

#నిర్వచితమైతే WITH_TLS
		స్థితి LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION:
			ఒకవేళ(!len || (SSL_get_verify_ఫలితం((SSL *)లోపల) != X509_V_OK)){
				తిరిగి 1;
			}
			ఆపు;
#అయితే_ముగింపు

		అప్రమేయ:
			తిరిగి 0;
	}

	తిరిగి 0;
}


స్థిర శూన్య log_wrap(పూర్ణాంకం level, స్థిరరాశి అక్షరం *పంక్తి)
{
	అక్షరం *l = (అక్షరం *)పంక్తి;
	వాడని(level);
	l[అక్షరశ్రేణి_పొడవు(పంక్తి)-1] = '\0'; /* Remove \n */
	log__printf(ఖాళీ, MOSQ_LOG_WEBSOCKETS, "%s", l);
}


శూన్య mosq_websockets_init(నిర్మాణం mosquitto__listener *listener, స్థిరరాశి నిర్మాణం mosquitto__config *conf)
{
	నిర్మాణం lws_context_creation_info info;
	నిర్మాణం lws_protocols *p;
	size_t protocol_లెక్క;
	పూర్ణాంకం i;
	నిర్మాణం libws_mqtt_hack *user;

	/* Count valid protocols */
	కొరకు(protocol_లెక్క=0; protocols[protocol_లెక్క].name; protocol_లెక్క++){
		;
	}

	p = mosquitto_calloc(protocol_లెక్క+1, పరిమాణం(నిర్మాణం lws_protocols));
	ఒకవేళ(!p){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Out of memory.");
		తిరిగి;
	}
	కొరకు(i=0; protocols[i].name; i++){
		p[i].name = protocols[i].name;
		p[i].callback = protocols[i].callback;
		p[i].చొప్పున_session_దత్తాంశం_కొలత = protocols[i].చొప్పున_session_దత్తాంశం_కొలత;
		p[i].rx_బఫరు_కొలత = protocols[i].rx_బఫరు_కొలత;
		p[i].user = listener;
	}

	memset(&info, 0, పరిమాణం(info));
	info.iface = listener->host;
	info.పోర్టు = listener->పోర్టు;
	info.protocols = p;
	info.gid = -1;
	info.uid = -1;
#నిర్వచితమైతే WITH_TLS
	ఒకవేళ(listener->cafile){
		info.ssl_ca_filepath = listener->cafile;
	}లేకపోతే ఒకవేళ(listener->capath){
		log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: CA path option is not supported for websockets");
	}
	info.ssl_cert_filepath = listener->certfile;
	info.ssl_వ్యక్తిగత_కీలకం_filepath = listener->keyfile;
	info.ssl_cipher_list = listener->ciphers;
	/* HTTP 1 only, due to HTTP 2 issues in Firefox:
	   https://github.com/eclipse-mosquitto/mosquitto/issues/1211
	*/
	info.alpn = "h1";
#ఒకవేళ defined(WITH_WEBSOCKETS) && LWS_LIBRARY_సంస్కరణ_సంఖ్య>=3001000
	info.tls1_3_plus_cipher_list = listener->ciphers_tls13;
#అయితే_ముగింపు
	ఒకవేళ(listener->require_certificate){
		info.options |= LWS_SERVER_OPTION_REQUIRE_VALID_OPENSSL_CLIENT_CERT;
	}
#అయితే_ముగింపు

	info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
	ఒకవేళ(listener->socket_domain == AF_INET){
		info.options |= LWS_SERVER_OPTION_DISABLE_IPV6;
	}
	info.max_http_శీర్షిక_దత్తాంశం = conf->packet_బఫరు_కొలత;

	user = mosquitto_calloc(1, పరిమాణం(నిర్మాణం libws_mqtt_hack));
	ఒకవేళ(!user){
		mosquitto_FREE(p);
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Out of memory.");
		తిరిగి;
	}

	ఒకవేళ(listener->http_dir){
#నిర్వచితమైతే WIN32
		user->http_dir = _fullpath(ఖాళీ, listener->http_dir, 0);
#లేకపోతే
		user->http_dir = realpath(listener->http_dir, ఖాళీ);
#అయితే_ముగింపు
		ఒకవేళ(!user->http_dir){
			mosquitto_FREE(user);
			mosquitto_FREE(p);
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to open http dir \"%s\".", listener->http_dir);
			తిరిగి;
		}
	}
	user->listener = listener;

	info.user = user;
	info.pt_serv_buf_కొలత = WS_SERV_BUF_కొలత;
	listener->ws_protocol = p;

	lws_సమితి_log_level(conf->websockets_log_level, log_wrap);

	log__printf(ఖాళీ, MOSQ_LOG_INFO, "Opening websockets listen socket on port %d.", listener->పోర్టు);
	listener->ws_లోపల_init = true;
	listener->ws_context = lws_create_context(&info);
	listener->ws_లోపల_init = false;
}

#అయితే_ముగింపు
#అయితే_ముగింపు
