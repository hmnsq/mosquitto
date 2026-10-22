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

#अन्तर्भाव "config.h"

#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN

#अन्तर्भाव <assert.h>
#अन्तर्भाव <errno.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "net_mosq.h"
#अन्तर्भाव "packet_mosq.h"
#अन्तर्भाव "read_handle.h"
#अन्तर्भाव "sys_tree.h"
#अन्तर्भाव "util_mosq.h"
#अन्तर्भाव "picohttpparser.h"


पूर्णाङ्क http__context_init(संरचना mosquitto *context)
{
	context->transport = mosq_t_http;

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क http__context_cleanup(संरचना mosquitto *context)
{
	अप्रयुक्तम्(context);
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क http__read(संरचना mosquitto *mosq)
{
	ssize_t read_दीर्घता;
	ssize_t शीर्षकम्_दीर्घता;
	गणना mosquitto_client_state state;
	size_t hlen;
	ध्रुव वर्ण *http_विधानम्, *http_मार्गः;
	size_t http_विधानम्_len, http_मार्गः_len;
	पूर्णाङ्क http_minor_संस्करणम्;
	size_t http_शीर्षकम्_गणनम् = 100;
	संरचना phr_शीर्षकम् http_शीर्षकाणि[100];
	ध्रुव वर्ण *client_कुञ्चिका = रिक्त;
	size_t client_कुञ्चिका_len = 0;
	वर्ण *accept_कुञ्चिका;
	bool शीर्षकम्_have_upgrade;
	bool शीर्षकम्_have_connection;
	संरचना mosquitto__packet *packet;
	पूर्णाङ्क rc;
	ध्रुव वर्ण *subprotocol = रिक्त;
	पूर्णाङ्क subprotocol_len = 0;

	यदि(!mosq){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि(mosq->sock == INVALID_SOCKET){
		प्रत्यागम MOSQ_ERR_NO_CONN;
	}

	state = mosquitto__get_state(mosq);
	यदि(state == mosq_cs_connect_pending){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	hlen = वाक्यदैर्घ्यम्((वर्ण *)mosq->अन्तर्_packet.packet_बफर);
	read_दीर्घता = net__read(mosq, &mosq->अन्तर्_packet.packet_बफर[hlen], mosq->अन्तर्_packet.packet_बफर_आकार-hlen);
	यदि(read_दीर्घता <= 0){
		यदि(read_दीर्घता == 0){
			प्रत्यागम MOSQ_ERR_CONN_नष्टम्; /* EOF */
		}
#यदि_परिभाषितम् WIN32
		errno = WSAGet_चरमः_Error();
#यदि_अन्तः
		यदि(errno == EAGAIN || errno == COMPAT_EWOULDBLOCK){
			प्रत्यागम MOSQ_ERR_SUCCESS;
		}अन्यथा{
			विकल्प(errno){
				पक्ष COMPAT_ECONNRESET:
					प्रत्यागम MOSQ_ERR_CONN_नष्टम्;
				पक्ष COMPAT_EINTR:
					प्रत्यागम MOSQ_ERR_SUCCESS;
				स्वभाव:
					प्रत्यागम MOSQ_ERR_ERRNO;
			}
		}
	}

	mosq->अन्तर्_packet.packet_बफर[mosq->अन्तर्_packet.packet_बफर_आकार-1] = '\0'; /* Always 0 terminate */
	शीर्षकम्_दीर्घता = phr_parse_request((वर्ण *)mosq->अन्तर्_packet.packet_बफर, वाक्यदैर्घ्यम्((वर्ण *)mosq->अन्तर्_packet.packet_बफर),
			&http_विधानम्, &http_विधानम्_len,
			&http_मार्गः, &http_मार्गः_len,
			&http_minor_संस्करणम्,
			http_शीर्षकाणि, &http_शीर्षकम्_गणनम्,
			0);
	// FIXME - deal with partial read !
	यदि(शीर्षकम्_दीर्घता == -2){
		// Partial read
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}अन्यथा यदि(शीर्षकम्_दीर्घता == -1){
		// Error
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}अन्यथा यदि(शीर्षकम्_दीर्घता < read_दीर्घता){
		/* Excess data which can't be handled because the client doesn't have a key yet */
		प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
	}

	यदि(वाक्ये_तुलय(http_विधानम्, "GET", http_विधानम्_len) && वाक्ये_तुलय(http_विधानम्, "HEAD", http_विधानम्_len)){
		/* FIXME Not supported - send 501 response */
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}

	शीर्षकम्_have_upgrade = false;
	शीर्षकम्_have_connection = false;
	subprotocol = रिक्त;

	प्रति(size_t i=0; i<http_शीर्षकम्_गणनम्; i++){
		यदि(!strncasecmp(http_शीर्षकाणि[i].name, "Upgrade", http_शीर्षकाणि[i].name_len)){
			यदि(!strncasecmp(http_शीर्षकाणि[i].मानम्, "websocket", http_शीर्षकाणि[i].मानम्_len)){
				शीर्षकम्_have_upgrade = true;
			}
		}अन्यथा यदि(!strncasecmp(http_शीर्षकाणि[i].name, "Connection", http_शीर्षकाणि[i].name_len)){
			/* Check for "upgrade" */
			ध्रुव वर्ण *str = http_शीर्षकाणि[i].मानम्;
			size_t start = 0;
			size_t j;
			प्रति(j=0; j<http_शीर्षकाणि[i].मानम्_len; j++){
				यदि(str[j] == ','){
					यदि(!strncasecmp(&str[start], "upgrade", http_शीर्षकाणि[i].मानम्_len-j)){
						शीर्षकम्_have_connection = true;
						भङ्ग;
					}अन्यथा{
						start = j+1;
					}
				}अन्यथा यदि(str[j] == ' '){
					start = j+1;
				}
			}
			यदि(!strncasecmp(&str[start], "upgrade", http_शीर्षकाणि[i].मानम्_len-j)){
				शीर्षकम्_have_connection = true;
			}
		}अन्यथा यदि(!strncasecmp(http_शीर्षकाणि[i].name, "Sec-WebSocket-Key", http_शीर्षकाणि[i].name_len)){
			client_कुञ्चिका = http_शीर्षकाणि[i].मानम्;
			client_कुञ्चिका_len = http_शीर्षकाणि[i].मानम्_len;
		}अन्यथा यदि(!strncasecmp(http_शीर्षकाणि[i].name, "Sec-WebSocket-Version", http_शीर्षकाणि[i].name_len)){
			/* Check for "13" */
			यदि(http_शीर्षकाणि[i].मानम्_len != 2
					|| http_शीर्षकाणि[i].मानम्[0] != '1'
					|| http_शीर्षकाणि[i].मानम्[1] != '3'
					){

				/* FIXME - not supported */
				प्रत्यागम MOSQ_ERR_NOT_SUPPORTED;
			}
		}अन्यथा यदि(!strncasecmp(http_शीर्षकाणि[i].name, "Sec-WebSocket-Protocol", http_शीर्षकाणि[i].name_len)){
			/* Check for "mqtt" */
			यदि(!वाक्ये_तुलय(http_शीर्षकाणि[i].मानम्, "mqtt", http_शीर्षकाणि[i].मानम्_len)
					|| !वाक्ये_तुलय(http_शीर्षकाणि[i].मानम्, "mqttv3.1", http_शीर्षकाणि[i].मानम्_len)){

				subprotocol = http_शीर्षकाणि[i].मानम्;
				subprotocol_len = (पूर्णाङ्क)http_शीर्षकाणि[i].मानम्_len;
			}
		}अन्यथा यदि(!strncasecmp(http_शीर्षकाणि[i].name, "X-Forwarded-For", http_शीर्षकाणि[i].name_len)){
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
		}अन्यथा यदि(!strncasecmp(http_शीर्षकाणि[i].name, "Origin", http_शीर्षकाणि[i].name_len)){
			यदि(mosq->listener){
				bool have_मेलनम् = false;
				प्रति(पूर्णाङ्क j=0; j<mosq->listener->ws_origin_गणनम्; j++){
					यदि(!वाक्ये_तुलय(mosq->listener->ws_origins[j], http_शीर्षकाणि[i].मानम्, http_शीर्षकाणि[i].मानम्_len)){
						have_मेलनम् = true;
						भङ्ग;
					}
				}
				यदि(!have_मेलनम् && mosq->listener->ws_origin_गणनम् > 0){
					प्रत्यागम MOSQ_ERR_HTTP_BAD_ORIGIN;
				}
			}
		}अन्यथा{
			/* Unknown header */
		}
	}

	यदि(subprotocol == रिक्त){
		// FIXME ?
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}

	यदि(शीर्षकम्_have_upgrade == false || शीर्षकम्_have_connection == false
			|| client_कुञ्चिका == रिक्त || client_कुञ्चिका_len == 0){

		// FIXME - 404
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}

	यदि(ws_create_accept_कुञ्चिका(client_कुञ्चिका, client_कुञ्चिका_len, &accept_कुञ्चिका)){
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}

	packet = mosquitto_calloc(1, परिमाण(संरचना mosquitto__packet) + 1024 + WS_PACKET_विस्थापनम्);
	यदि(!packet){
		SAFE_FREE(accept_कुञ्चिका);
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	packet->packet_दीर्घता = (uint32_t )snprintf((वर्ण *)&packet->payload[WS_PACKET_विस्थापनम्], 1024,
			"HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: WebSocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: %s\r\n"
			"Sec-WebSocket-Protocol: %.*s\r\n"
			"\r\n", accept_कुञ्चिका, subprotocol_len, subprotocol) + WS_PACKET_विस्थापनम्;
	SAFE_FREE(accept_कुञ्चिका);
	packet->गन्तव्यम्_process = packet->packet_दीर्घता;

	स्मृतिं_पूरय(mosq->अन्तर्_packet.packet_बफर, 0, db.config->packet_बफर_आकार);
	rc = packet__queue(mosq, packet);
	http__context_cleanup(mosq);
	ws__context_init(mosq);
	प्रत्यागम rc;
}
#यदि_अन्तः
