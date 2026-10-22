/*
Copyright (c) 2021 Roger Light <roger@atchoo.org>

All rights reserved. This program and the accompanying materials
are made available under the terms of the Eclipse Public License 2.0
and Eclipse Distribution License v1.0 which accompany this distribution.

The Eclipse Public License is available at
   https://www.eclipse.org/legal/epl-2.0/
and the Eclipse Distribution License is available at
  http://www.eclipse.org/org/documents/edl-v10.php.

SPDX-License-Identifier: EPL-2.0 OR EDL-1.0

Contributors:
   Roger Light - initial implementation and documentation.
*/

#అన్తర్భావ "config.h"

#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN

#అన్తర్భావ <assert.h>
#అన్తర్భావ <errno.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto/mqtt_protocol.h"
#అన్తర్భావ "net_mosq.h"
#అన్తర్భావ "packet_mosq.h"
#అన్తర్భావ "read_handle.h"
#అన్తర్భావ "sys_tree.h"
#అన్తర్భావ "util_mosq.h"
#అన్తర్భావ "picohttpparser.h"


పూర్ణాంకం http__context_init(నిర్మాణం mosquitto *context)
{
	context->transport = mosq_t_http;

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం http__context_cleanup(నిర్మాణం mosquitto *context)
{
	వాడని(context);
	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం http__read(నిర్మాణం mosquitto *mosq)
{
	ssize_t read_నిడివి;
	ssize_t శీర్షిక_నిడివి;
	గణన mosquitto_client_state state;
	size_t hlen;
	స్థిరరాశి అక్షరం *http_పద్ధతి, *http_మార్గం;
	size_t http_పద్ధతి_len, http_మార్గం_len;
	పూర్ణాంకం http_minor_సంస్కరణ;
	size_t http_శీర్షిక_లెక్క = 100;
	నిర్మాణం phr_శీర్షిక http_శీర్షికలు[100];
	స్థిరరాశి అక్షరం *client_కీలకం = ఖాళీ;
	size_t client_కీలకం_len = 0;
	అక్షరం *accept_కీలకం;
	bool శీర్షిక_have_upgrade;
	bool శీర్షిక_have_connection;
	నిర్మాణం mosquitto__packet *packet;
	పూర్ణాంకం rc;
	స్థిరరాశి అక్షరం *subprotocol = ఖాళీ;
	పూర్ణాంకం subprotocol_len = 0;

	ఒకవేళ(!mosq){
		తిరిగి MOSQ_ERR_INVAL;
	}
	ఒకవేళ(mosq->sock == INVALID_SOCKET){
		తిరిగి MOSQ_ERR_NO_CONN;
	}

	state = mosquitto__get_state(mosq);
	ఒకవేళ(state == mosq_cs_connect_pending){
		తిరిగి MOSQ_ERR_SUCCESS;
	}

	hlen = అక్షరశ్రేణి_పొడవు((అక్షరం *)mosq->లోపల_packet.packet_బఫరు);
	read_నిడివి = net__read(mosq, &mosq->లోపల_packet.packet_బఫరు[hlen], mosq->లోపల_packet.packet_బఫరు_కొలత-hlen);
	ఒకవేళ(read_నిడివి <= 0){
		ఒకవేళ(read_నిడివి == 0){
			తిరిగి MOSQ_ERR_CONN_కోల్పోయిన; /* EOF */
		}
#నిర్వచితమైతే WIN32
		errno = WSAGet_చివరి_Error();
#అయితే_ముగింపు
		ఒకవేళ(errno == EAGAIN || errno == COMPAT_EWOULDBLOCK){
			తిరిగి MOSQ_ERR_SUCCESS;
		}లేకపోతే{
			ఎంపిక(errno){
				స్థితి COMPAT_ECONNRESET:
					తిరిగి MOSQ_ERR_CONN_కోల్పోయిన;
				స్థితి COMPAT_EINTR:
					తిరిగి MOSQ_ERR_SUCCESS;
				అప్రమేయ:
					తిరిగి MOSQ_ERR_ERRNO;
			}
		}
	}

	mosq->లోపల_packet.packet_బఫరు[mosq->లోపల_packet.packet_బఫరు_కొలత-1] = '\0'; /* Always 0 terminate */
	శీర్షిక_నిడివి = phr_parse_request((అక్షరం *)mosq->లోపల_packet.packet_బఫరు, అక్షరశ్రేణి_పొడవు((అక్షరం *)mosq->లోపల_packet.packet_బఫరు),
			&http_పద్ధతి, &http_పద్ధతి_len,
			&http_మార్గం, &http_మార్గం_len,
			&http_minor_సంస్కరణ,
			http_శీర్షికలు, &http_శీర్షిక_లెక్క,
			0);
	// FIXME - deal with partial read !
	ఒకవేళ(శీర్షిక_నిడివి == -2){
		// Partial read
		తిరిగి MOSQ_ERR_SUCCESS;
	}లేకపోతే ఒకవేళ(శీర్షిక_నిడివి == -1){
		// Error
		తిరిగి MOSQ_ERR_UNKNOWN;
	}లేకపోతే ఒకవేళ(శీర్షిక_నిడివి < read_నిడివి){
		/* Excess data which can't be handled because the client doesn't have a key yet */
		తిరిగి MOSQ_ERR_MALFORMED_PACKET;
	}

	ఒకవేళ(అక్షరశ్రేణి_పోలిక(http_పద్ధతి, "GET", http_పద్ధతి_len) && అక్షరశ్రేణి_పోలిక(http_పద్ధతి, "HEAD", http_పద్ధతి_len)){
		/* FIXME Not supported - send 501 response */
		తిరిగి MOSQ_ERR_UNKNOWN;
	}

	శీర్షిక_have_upgrade = false;
	శీర్షిక_have_connection = false;
	subprotocol = ఖాళీ;

	కొరకు(size_t i=0; i<http_శీర్షిక_లెక్క; i++){
		ఒకవేళ(!strncasecmp(http_శీర్షికలు[i].name, "Upgrade", http_శీర్షికలు[i].name_len)){
			ఒకవేళ(!strncasecmp(http_శీర్షికలు[i].విలువ, "websocket", http_శీర్షికలు[i].విలువ_len)){
				శీర్షిక_have_upgrade = true;
			}
		}లేకపోతే ఒకవేళ(!strncasecmp(http_శీర్షికలు[i].name, "Connection", http_శీర్షికలు[i].name_len)){
			/* Check for "upgrade" */
			స్థిరరాశి అక్షరం *str = http_శీర్షికలు[i].విలువ;
			size_t ప్రారంభం = 0;
			size_t j;
			కొరకు(j=0; j<http_శీర్షికలు[i].విలువ_len; j++){
				ఒకవేళ(str[j] == ','){
					ఒకవేళ(!strncasecmp(&str[ప్రారంభం], "upgrade", http_శీర్షికలు[i].విలువ_len-j)){
						శీర్షిక_have_connection = true;
						ఆపు;
					}లేకపోతే{
						ప్రారంభం = j+1;
					}
				}లేకపోతే ఒకవేళ(str[j] == ' '){
					ప్రారంభం = j+1;
				}
			}
			ఒకవేళ(!strncasecmp(&str[ప్రారంభం], "upgrade", http_శీర్షికలు[i].విలువ_len-j)){
				శీర్షిక_have_connection = true;
			}
		}లేకపోతే ఒకవేళ(!strncasecmp(http_శీర్షికలు[i].name, "Sec-WebSocket-Key", http_శీర్షికలు[i].name_len)){
			client_కీలకం = http_శీర్షికలు[i].విలువ;
			client_కీలకం_len = http_శీర్షికలు[i].విలువ_len;
		}లేకపోతే ఒకవేళ(!strncasecmp(http_శీర్షికలు[i].name, "Sec-WebSocket-Version", http_శీర్షికలు[i].name_len)){
			/* Check for "13" */
			ఒకవేళ(http_శీర్షికలు[i].విలువ_len != 2
					|| http_శీర్షికలు[i].విలువ[0] != '1'
					|| http_శీర్షికలు[i].విలువ[1] != '3'
					){

				/* FIXME - not supported */
				తిరిగి MOSQ_ERR_NOT_SUPPORTED;
			}
		}లేకపోతే ఒకవేళ(!strncasecmp(http_శీర్షికలు[i].name, "Sec-WebSocket-Protocol", http_శీర్షికలు[i].name_len)){
			/* Check for "mqtt" */
			ఒకవేళ(!అక్షరశ్రేణి_పోలిక(http_శీర్షికలు[i].విలువ, "mqtt", http_శీర్షికలు[i].విలువ_len)
					|| !అక్షరశ్రేణి_పోలిక(http_శీర్షికలు[i].విలువ, "mqttv3.1", http_శీర్షికలు[i].విలువ_len)){

				subprotocol = http_శీర్షికలు[i].విలువ;
				subprotocol_len = (పూర్ణాంకం)http_శీర్షికలు[i].విలువ_len;
			}
		}లేకపోతే ఒకవేళ(!strncasecmp(http_శీర్షికలు[i].name, "X-Forwarded-For", http_శీర్షికలు[i].name_len)){
			/* Before implementing this, refer to:
			 * https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/X-Forwarded-For
			 *
			 * At the very least, a trusted proxy count must be used. A trusted
			 * proxy list would ideally be used.
			 *
			 * Problematic for us is that if the listener is directly
			 * connectable, then the use of this header is insecure. We can't
			 * control that, so we have to make it very clear to the end user
			 * that this is the case.
			 */
		}లేకపోతే ఒకవేళ(!strncasecmp(http_శీర్షికలు[i].name, "Origin", http_శీర్షికలు[i].name_len)){
			ఒకవేళ(mosq->listener){
				bool have_సరిపోలిక = false;
				కొరకు(పూర్ణాంకం j=0; j<mosq->listener->ws_origin_లెక్క; j++){
					ఒకవేళ(!అక్షరశ్రేణి_పోలిక(mosq->listener->ws_origins[j], http_శీర్షికలు[i].విలువ, http_శీర్షికలు[i].విలువ_len)){
						have_సరిపోలిక = true;
						ఆపు;
					}
				}
				ఒకవేళ(!have_సరిపోలిక && mosq->listener->ws_origin_లెక్క > 0){
					తిరిగి MOSQ_ERR_HTTP_BAD_ORIGIN;
				}
			}
		}లేకపోతే{
			/* Unknown header */
		}
	}

	ఒకవేళ(subprotocol == ఖాళీ){
		// FIXME ?
		తిరిగి MOSQ_ERR_UNKNOWN;
	}

	ఒకవేళ(శీర్షిక_have_upgrade == false || శీర్షిక_have_connection == false
			|| client_కీలకం == ఖాళీ || client_కీలకం_len == 0){

		// FIXME - 404
		తిరిగి MOSQ_ERR_UNKNOWN;
	}

	ఒకవేళ(ws_create_accept_కీలకం(client_కీలకం, client_కీలకం_len, &accept_కీలకం)){
		తిరిగి MOSQ_ERR_UNKNOWN;
	}

	packet = mosquitto_calloc(1, పరిమాణం(నిర్మాణం mosquitto__packet) + 1024 + WS_PACKET_స్థానభ్రంశం);
	ఒకవేళ(!packet){
		సురక్షిత_FREE(accept_కీలకం);
		తిరిగి MOSQ_ERR_NOMEM;
	}
	packet->packet_నిడివి = (uint32_t )snprintf((అక్షరం *)&packet->payload[WS_PACKET_స్థానభ్రంశం], 1024,
			"HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: WebSocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: %s\r\n"
			"Sec-WebSocket-Protocol: %.*s\r\n"
			"\r\n", accept_కీలకం, subprotocol_len, subprotocol) + WS_PACKET_స్థానభ్రంశం;
	సురక్షిత_FREE(accept_కీలకం);
	packet->గమ్యం_process = packet->packet_నిడివి;

	memset(mosq->లోపల_packet.packet_బఫరు, 0, db.config->packet_బఫరు_కొలత);
	rc = packet_వరుస(mosq, packet);
	http__context_cleanup(mosq);
	ws__context_init(mosq);
	తిరిగి rc;
}
#అయితే_ముగింపు
