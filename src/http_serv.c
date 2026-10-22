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

#जर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN

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


पूर्णांक http__context_init(संरचना mosquitto *context)
{
	context->transport = mosq_t_http;

	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक http__context_cleanup(संरचना mosquitto *context)
{
	UNUSED(context);
	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक http__read(संरचना mosquitto *mosq)
{
	ssize_t read_length;
	ssize_t header_length;
	गणना mosquitto_client_state state;
	माप_प्रकार hlen;
	स्थिरांक अक्षर *http_method, *http_path;
	माप_प्रकार http_method_len, http_path_len;
	पूर्णांक http_minor_version;
	माप_प्रकार http_header_मोजणी = 100;
	संरचना phr_header http_headers[100];
	स्थिरांक अक्षर *client_किल्ली = रिक्त;
	माप_प्रकार client_किल्ली_len = 0;
	अक्षर *accept_किल्ली;
	bool header_have_upgrade;
	bool header_have_connection;
	संरचना mosquitto__packet *packet;
	पूर्णांक rc;
	स्थिरांक अक्षर *subprotocol = रिक्त;
	पूर्णांक subprotocol_len = 0;

	जर(!mosq){
		परत MOSQ_ERR_INVAL;
	}
	जर(mosq->sock == INVALID_SOCKET){
		परत MOSQ_ERR_NO_CONN;
	}

	state = mosquitto__get_state(mosq);
	जर(state == mosq_cs_connect_pending){
		परत MOSQ_ERR_SUCCESS;
	}

	hlen = म_माप((अक्षर *)mosq->in_packet.packet_buffer);
	read_length = net__read(mosq, &mosq->in_packet.packet_buffer[hlen], mosq->in_packet.packet_buffer_size-hlen);
	जर(read_length <= 0){
		जर(read_length == 0){
			परत MOSQ_ERR_CONN_LOST; /* EOF */
		}
#ifdef WIN32
		त्रुटि_सं = WSAGetLastError();
#endif
		जर(त्रुटि_सं == EAGAIN || त्रुटि_सं == COMPAT_EWOULDBLOCK){
			परत MOSQ_ERR_SUCCESS;
		}अन्यथा{
			निवड(त्रुटि_सं){
				स्थिती COMPAT_ECONNRESET:
					परत MOSQ_ERR_CONN_LOST;
				स्थिती COMPAT_EINTR:
					परत MOSQ_ERR_SUCCESS;
				पूर्वनिर्धारित:
					परत MOSQ_ERR_ERRNO;
			}
		}
	}

	mosq->in_packet.packet_buffer[mosq->in_packet.packet_buffer_size-1] = '\0'; /* Always 0 terminate */
	header_length = phr_parse_request((अक्षर *)mosq->in_packet.packet_buffer, म_माप((अक्षर *)mosq->in_packet.packet_buffer),
			&http_method, &http_method_len,
			&http_path, &http_path_len,
			&http_minor_version,
			http_headers, &http_header_मोजणी,
			0);
	// FIXME - deal with partial read !
	जर(header_length == -2){
		// Partial read
		परत MOSQ_ERR_SUCCESS;
	}अन्यथा जर(header_length == -1){
		// Error
		परत MOSQ_ERR_UNKNOWN;
	}अन्यथा जर(header_length < read_length){
		/* Excess data which can't be handled because the client doesn't have a key yet */
		परत MOSQ_ERR_MALFORMED_PACKET;
	}

	जर(म_भेदन(http_method, "GET", http_method_len) && म_भेदन(http_method, "HEAD", http_method_len)){
		/* FIXME Not supported - send 501 response */
		परत MOSQ_ERR_UNKNOWN;
	}

	header_have_upgrade = false;
	header_have_connection = false;
	subprotocol = रिक्त;

	साठी(माप_प्रकार i=0; i<http_header_मोजणी; i++){
		जर(!strncasecmp(http_headers[i].name, "Upgrade", http_headers[i].name_len)){
			जर(!strncasecmp(http_headers[i].मूल्य, "websocket", http_headers[i].मूल्य_len)){
				header_have_upgrade = true;
			}
		}अन्यथा जर(!strncasecmp(http_headers[i].name, "Connection", http_headers[i].name_len)){
			/* Check for "upgrade" */
			स्थिरांक अक्षर *str = http_headers[i].मूल्य;
			माप_प्रकार start = 0;
			माप_प्रकार j;
			साठी(j=0; j<http_headers[i].मूल्य_len; j++){
				जर(str[j] == ','){
					जर(!strncasecmp(&str[start], "upgrade", http_headers[i].मूल्य_len-j)){
						header_have_connection = true;
						थांबा;
					}अन्यथा{
						start = j+1;
					}
				}अन्यथा जर(str[j] == ' '){
					start = j+1;
				}
			}
			जर(!strncasecmp(&str[start], "upgrade", http_headers[i].मूल्य_len-j)){
				header_have_connection = true;
			}
		}अन्यथा जर(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Key", http_headers[i].name_len)){
			client_किल्ली = http_headers[i].मूल्य;
			client_किल्ली_len = http_headers[i].मूल्य_len;
		}अन्यथा जर(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Version", http_headers[i].name_len)){
			/* Check for "13" */
			जर(http_headers[i].मूल्य_len != 2
					|| http_headers[i].मूल्य[0] != '1'
					|| http_headers[i].मूल्य[1] != '3'
					){

				/* FIXME - not supported */
				परत MOSQ_ERR_NOT_SUPPORTED;
			}
		}अन्यथा जर(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Protocol", http_headers[i].name_len)){
			/* Check for "mqtt" */
			जर(!म_भेदन(http_headers[i].मूल्य, "mqtt", http_headers[i].मूल्य_len)
					|| !म_भेदन(http_headers[i].मूल्य, "mqttv3.1", http_headers[i].मूल्य_len)){

				subprotocol = http_headers[i].मूल्य;
				subprotocol_len = (पूर्णांक)http_headers[i].मूल्य_len;
			}
		}अन्यथा जर(!strncasecmp(http_headers[i].name, "X-Forwarded-For", http_headers[i].name_len)){
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
		}अन्यथा जर(!strncasecmp(http_headers[i].name, "Origin", http_headers[i].name_len)){
			जर(mosq->listener){
				bool have_match = false;
				साठी(पूर्णांक j=0; j<mosq->listener->ws_origin_मोजणी; j++){
					जर(!म_भेदन(mosq->listener->ws_origins[j], http_headers[i].मूल्य, http_headers[i].मूल्य_len)){
						have_match = true;
						थांबा;
					}
				}
				जर(!have_match && mosq->listener->ws_origin_मोजणी > 0){
					परत MOSQ_ERR_HTTP_BAD_ORIGIN;
				}
			}
		}अन्यथा{
			/* Unknown header */
		}
	}

	जर(subprotocol == रिक्त){
		// FIXME ?
		परत MOSQ_ERR_UNKNOWN;
	}

	जर(header_have_upgrade == false || header_have_connection == false
			|| client_किल्ली == रिक्त || client_किल्ली_len == 0){

		// FIXME - 404
		परत MOSQ_ERR_UNKNOWN;
	}

	जर(ws_create_accept_किल्ली(client_किल्ली, client_किल्ली_len, &accept_किल्ली)){
		परत MOSQ_ERR_UNKNOWN;
	}

	packet = mosquitto_calloc(1, आकार(संरचना mosquitto__packet) + 1024 + WS_PACKET_OFFSET);
	जर(!packet){
		SAFE_FREE(accept_किल्ली);
		परत MOSQ_ERR_NOMEM;
	}
	packet->packet_length = (uint32_t )snprintf((अक्षर *)&packet->payload[WS_PACKET_OFFSET], 1024,
			"HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: WebSocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: %s\r\n"
			"Sec-WebSocket-Protocol: %.*s\r\n"
			"\r\n", accept_किल्ली, subprotocol_len, subprotocol) + WS_PACKET_OFFSET;
	SAFE_FREE(accept_किल्ली);
	packet->to_process = packet->packet_length;

	स_रखो(mosq->in_packet.packet_buffer, 0, db.config->packet_buffer_size);
	rc = packet__queue(mosq, packet);
	http__context_cleanup(mosq);
	ws__context_init(mosq);
	परत rc;
}
#endif
