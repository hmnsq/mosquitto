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

	फर्काऊ MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क http__context_cleanup(संरचना mosquitto *context)
{
	UNUSED(context);
	फर्काऊ MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क http__read(संरचना mosquitto *mosq)
{
	ssize_t read_length;
	ssize_t header_length;
	गणना mosquitto_client_state state;
	माप_प्रकार hlen;
	अचर वर्ण *http_method, *http_path;
	माप_प्रकार http_method_len, http_path_len;
	पूर्णाङ्क http_minor_version;
	माप_प्रकार http_header_गन्ती = 100;
	संरचना phr_header http_headers[100];
	अचर वर्ण *client_साँचो = रिक्त;
	माप_प्रकार client_साँचो_len = 0;
	वर्ण *accept_साँचो;
	bool header_have_upgrade;
	bool header_have_connection;
	संरचना mosquitto__packet *packet;
	पूर्णाङ्क rc;
	अचर वर्ण *subprotocol = रिक्त;
	पूर्णाङ्क subprotocol_len = 0;

	यदि(!mosq){
		फर्काऊ MOSQ_ERR_INVAL;
	}
	यदि(mosq->sock == INVALID_SOCKET){
		फर्काऊ MOSQ_ERR_NO_CONN;
	}

	state = mosquitto__get_state(mosq);
	यदि(state == mosq_cs_connect_pending){
		फर्काऊ MOSQ_ERR_SUCCESS;
	}

	hlen = म_माप((वर्ण *)mosq->in_packet.packet_buffer);
	read_length = net__read(mosq, &mosq->in_packet.packet_buffer[hlen], mosq->in_packet.packet_buffer_size-hlen);
	यदि(read_length <= 0){
		यदि(read_length == 0){
			फर्काऊ MOSQ_ERR_CONN_LOST; /* EOF */
		}
#ifdef WIN32
		त्रुटि_सं = WSAGetLastError();
#endif
		यदि(त्रुटि_सं == EAGAIN || त्रुटि_सं == COMPAT_EWOULDBLOCK){
			फर्काऊ MOSQ_ERR_SUCCESS;
		}अन्यथा{
			विकल्प(त्रुटि_सं){
				पक्ष COMPAT_ECONNRESET:
					फर्काऊ MOSQ_ERR_CONN_LOST;
				पक्ष COMPAT_EINTR:
					फर्काऊ MOSQ_ERR_SUCCESS;
				स्वभाव:
					फर्काऊ MOSQ_ERR_ERRNO;
			}
		}
	}

	mosq->in_packet.packet_buffer[mosq->in_packet.packet_buffer_size-1] = '\0'; /* Always 0 terminate */
	header_length = phr_parse_request((वर्ण *)mosq->in_packet.packet_buffer, म_माप((वर्ण *)mosq->in_packet.packet_buffer),
			&http_method, &http_method_len,
			&http_path, &http_path_len,
			&http_minor_version,
			http_headers, &http_header_गन्ती,
			0);
	// FIXME - deal with partial read !
	यदि(header_length == -2){
		// Partial read
		फर्काऊ MOSQ_ERR_SUCCESS;
	}अन्यथा यदि(header_length == -1){
		// Error
		फर्काऊ MOSQ_ERR_UNKNOWN;
	}अन्यथा यदि(header_length < read_length){
		/* Excess data which can't be handled because the client doesn't have a key yet */
		फर्काऊ MOSQ_ERR_MALFORMED_PACKET;
	}

	यदि(म_भेदन(http_method, "GET", http_method_len) && म_भेदन(http_method, "HEAD", http_method_len)){
		/* FIXME Not supported - send 501 response */
		फर्काऊ MOSQ_ERR_UNKNOWN;
	}

	header_have_upgrade = false;
	header_have_connection = false;
	subprotocol = रिक्त;

	लागि(माप_प्रकार i=0; i<http_header_गन्ती; i++){
		यदि(!strncasecmp(http_headers[i].name, "Upgrade", http_headers[i].name_len)){
			यदि(!strncasecmp(http_headers[i].मान, "websocket", http_headers[i].मान_len)){
				header_have_upgrade = true;
			}
		}अन्यथा यदि(!strncasecmp(http_headers[i].name, "Connection", http_headers[i].name_len)){
			/* Check for "upgrade" */
			अचर वर्ण *str = http_headers[i].मान;
			माप_प्रकार start = 0;
			माप_प्रकार j;
			लागि(j=0; j<http_headers[i].मान_len; j++){
				यदि(str[j] == ','){
					यदि(!strncasecmp(&str[start], "upgrade", http_headers[i].मान_len-j)){
						header_have_connection = true;
						रोक;
					}अन्यथा{
						start = j+1;
					}
				}अन्यथा यदि(str[j] == ' '){
					start = j+1;
				}
			}
			यदि(!strncasecmp(&str[start], "upgrade", http_headers[i].मान_len-j)){
				header_have_connection = true;
			}
		}अन्यथा यदि(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Key", http_headers[i].name_len)){
			client_साँचो = http_headers[i].मान;
			client_साँचो_len = http_headers[i].मान_len;
		}अन्यथा यदि(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Version", http_headers[i].name_len)){
			/* Check for "13" */
			यदि(http_headers[i].मान_len != 2
					|| http_headers[i].मान[0] != '1'
					|| http_headers[i].मान[1] != '3'
					){

				/* FIXME - not supported */
				फर्काऊ MOSQ_ERR_NOT_SUPPORTED;
			}
		}अन्यथा यदि(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Protocol", http_headers[i].name_len)){
			/* Check for "mqtt" */
			यदि(!म_भेदन(http_headers[i].मान, "mqtt", http_headers[i].मान_len)
					|| !म_भेदन(http_headers[i].मान, "mqttv3.1", http_headers[i].मान_len)){

				subprotocol = http_headers[i].मान;
				subprotocol_len = (पूर्णाङ्क)http_headers[i].मान_len;
			}
		}अन्यथा यदि(!strncasecmp(http_headers[i].name, "X-Forwarded-For", http_headers[i].name_len)){
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
		}अन्यथा यदि(!strncasecmp(http_headers[i].name, "Origin", http_headers[i].name_len)){
			यदि(mosq->listener){
				bool have_match = false;
				लागि(पूर्णाङ्क j=0; j<mosq->listener->ws_origin_गन्ती; j++){
					यदि(!म_भेदन(mosq->listener->ws_origins[j], http_headers[i].मान, http_headers[i].मान_len)){
						have_match = true;
						रोक;
					}
				}
				यदि(!have_match && mosq->listener->ws_origin_गन्ती > 0){
					फर्काऊ MOSQ_ERR_HTTP_BAD_ORIGIN;
				}
			}
		}अन्यथा{
			/* Unknown header */
		}
	}

	यदि(subprotocol == रिक्त){
		// FIXME ?
		फर्काऊ MOSQ_ERR_UNKNOWN;
	}

	यदि(header_have_upgrade == false || header_have_connection == false
			|| client_साँचो == रिक्त || client_साँचो_len == 0){

		// FIXME - 404
		फर्काऊ MOSQ_ERR_UNKNOWN;
	}

	यदि(ws_create_accept_साँचो(client_साँचो, client_साँचो_len, &accept_साँचो)){
		फर्काऊ MOSQ_ERR_UNKNOWN;
	}

	packet = mosquitto_calloc(1, परिमाण(संरचना mosquitto__packet) + 1024 + WS_PACKET_OFFSET);
	यदि(!packet){
		SAFE_FREE(accept_साँचो);
		फर्काऊ MOSQ_ERR_NOMEM;
	}
	packet->packet_length = (uint32_t )snprintf((वर्ण *)&packet->payload[WS_PACKET_OFFSET], 1024,
			"HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: WebSocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: %s\r\n"
			"Sec-WebSocket-Protocol: %.*s\r\n"
			"\r\n", accept_साँचो, subprotocol_len, subprotocol) + WS_PACKET_OFFSET;
	SAFE_FREE(accept_साँचो);
	packet->to_process = packet->packet_length;

	स_रखो(mosq->in_packet.packet_buffer, 0, db.config->packet_buffer_size);
	rc = packet__queue(mosq, packet);
	http__context_cleanup(mosq);
	ws__context_init(mosq);
	फर्काऊ rc;
}
#endif
