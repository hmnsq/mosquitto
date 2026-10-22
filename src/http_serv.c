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

#सचे defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN

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

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क http__context_cleanup(संरचना mosquitto *context)
{
	UNUSED(context);
	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क http__read(संरचना mosquitto *mosq)
{
	ssize_t read_length;
	ssize_t header_length;
	गणना mosquitto_client_state state;
	size_t hlen;
	ध्रुव वर्ण *http_method, *http_path;
	size_t http_method_len, http_path_len;
	पूर्णाङ्क http_minor_version;
	size_t http_header_गणनम् = 100;
	संरचना phr_header http_headers[100];
	ध्रुव वर्ण *client_कुञ्चिका = रिक्त;
	size_t client_कुञ्चिका_len = 0;
	वर्ण *accept_कुञ्चिका;
	bool header_have_upgrade;
	bool header_have_connection;
	संरचना mosquitto__packet *packet;
	पूर्णाङ्क rc;
	ध्रुव वर्ण *subprotocol = रिक्त;
	पूर्णाङ्क subprotocol_len = 0;

	सचे(!mosq){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}
	सचे(mosq->sock == INVALID_SOCKET){
		पटिनिवत्त MOSQ_ERR_NO_CONN;
	}

	state = mosquitto__get_state(mosq);
	सचे(state == mosq_cs_connect_pending){
		पटिनिवत्त MOSQ_ERR_SUCCESS;
	}

	hlen = strlen((वर्ण *)mosq->in_packet.packet_buffer);
	read_length = net__read(mosq, &mosq->in_packet.packet_buffer[hlen], mosq->in_packet.packet_buffer_size-hlen);
	सचे(read_length <= 0){
		सचे(read_length == 0){
			पटिनिवत्त MOSQ_ERR_CONN_LOST; /* EOF */
		}
#ifdef WIN32
		errno = WSAGetLastError();
#endif
		सचे(errno == EAGAIN || errno == COMPAT_EWOULDBLOCK){
			पटिनिवत्त MOSQ_ERR_SUCCESS;
		}अञ्ञथा{
			विकल्प(errno){
				पक्ष COMPAT_ECONNRESET:
					पटिनिवत्त MOSQ_ERR_CONN_LOST;
				पक्ष COMPAT_EINTR:
					पटिनिवत्त MOSQ_ERR_SUCCESS;
				स्वभाव:
					पटिनिवत्त MOSQ_ERR_ERRNO;
			}
		}
	}

	mosq->in_packet.packet_buffer[mosq->in_packet.packet_buffer_size-1] = '\0'; /* Always 0 terminate */
	header_length = phr_parse_request((वर्ण *)mosq->in_packet.packet_buffer, strlen((वर्ण *)mosq->in_packet.packet_buffer),
			&http_method, &http_method_len,
			&http_path, &http_path_len,
			&http_minor_version,
			http_headers, &http_header_गणनम्,
			0);
	// FIXME - deal with partial read !
	सचे(header_length == -2){
		// Partial read
		पटिनिवत्त MOSQ_ERR_SUCCESS;
	}अञ्ञथा सचे(header_length == -1){
		// Error
		पटिनिवत्त MOSQ_ERR_UNKNOWN;
	}अञ्ञथा सचे(header_length < read_length){
		/* Excess data which can't be handled because the client doesn't have a key yet */
		पटिनिवत्त MOSQ_ERR_MALFORMED_PACKET;
	}

	सचे(strncmp(http_method, "GET", http_method_len) && strncmp(http_method, "HEAD", http_method_len)){
		/* FIXME Not supported - send 501 response */
		पटिनिवत्त MOSQ_ERR_UNKNOWN;
	}

	header_have_upgrade = false;
	header_have_connection = false;
	subprotocol = रिक्त;

	अत्थाय(size_t i=0; i<http_header_गणनम्; i++){
		सचे(!strncasecmp(http_headers[i].name, "Upgrade", http_headers[i].name_len)){
			सचे(!strncasecmp(http_headers[i].मानम्, "websocket", http_headers[i].मानम्_len)){
				header_have_upgrade = true;
			}
		}अञ्ञथा सचे(!strncasecmp(http_headers[i].name, "Connection", http_headers[i].name_len)){
			/* Check for "upgrade" */
			ध्रुव वर्ण *str = http_headers[i].मानम्;
			size_t start = 0;
			size_t j;
			अत्थाय(j=0; j<http_headers[i].मानम्_len; j++){
				सचे(str[j] == ','){
					सचे(!strncasecmp(&str[start], "upgrade", http_headers[i].मानम्_len-j)){
						header_have_connection = true;
						भङ्ग;
					}अञ्ञथा{
						start = j+1;
					}
				}अञ्ञथा सचे(str[j] == ' '){
					start = j+1;
				}
			}
			सचे(!strncasecmp(&str[start], "upgrade", http_headers[i].मानम्_len-j)){
				header_have_connection = true;
			}
		}अञ्ञथा सचे(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Key", http_headers[i].name_len)){
			client_कुञ्चिका = http_headers[i].मानम्;
			client_कुञ्चिका_len = http_headers[i].मानम्_len;
		}अञ्ञथा सचे(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Version", http_headers[i].name_len)){
			/* Check for "13" */
			सचे(http_headers[i].मानम्_len != 2
					|| http_headers[i].मानम्[0] != '1'
					|| http_headers[i].मानम्[1] != '3'
					){

				/* FIXME - not supported */
				पटिनिवत्त MOSQ_ERR_NOT_SUPPORTED;
			}
		}अञ्ञथा सचे(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Protocol", http_headers[i].name_len)){
			/* Check for "mqtt" */
			सचे(!strncmp(http_headers[i].मानम्, "mqtt", http_headers[i].मानम्_len)
					|| !strncmp(http_headers[i].मानम्, "mqttv3.1", http_headers[i].मानम्_len)){

				subprotocol = http_headers[i].मानम्;
				subprotocol_len = (पूर्णाङ्क)http_headers[i].मानम्_len;
			}
		}अञ्ञथा सचे(!strncasecmp(http_headers[i].name, "X-Forwarded-For", http_headers[i].name_len)){
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
		}अञ्ञथा सचे(!strncasecmp(http_headers[i].name, "Origin", http_headers[i].name_len)){
			सचे(mosq->listener){
				bool have_match = false;
				अत्थाय(पूर्णाङ्क j=0; j<mosq->listener->ws_origin_गणनम्; j++){
					सचे(!strncmp(mosq->listener->ws_origins[j], http_headers[i].मानम्, http_headers[i].मानम्_len)){
						have_match = true;
						भङ्ग;
					}
				}
				सचे(!have_match && mosq->listener->ws_origin_गणनम् > 0){
					पटिनिवत्त MOSQ_ERR_HTTP_BAD_ORIGIN;
				}
			}
		}अञ्ञथा{
			/* Unknown header */
		}
	}

	सचे(subprotocol == रिक्त){
		// FIXME ?
		पटिनिवत्त MOSQ_ERR_UNKNOWN;
	}

	सचे(header_have_upgrade == false || header_have_connection == false
			|| client_कुञ्चिका == रिक्त || client_कुञ्चिका_len == 0){

		// FIXME - 404
		पटिनिवत्त MOSQ_ERR_UNKNOWN;
	}

	सचे(ws_create_accept_कुञ्चिका(client_कुञ्चिका, client_कुञ्चिका_len, &accept_कुञ्चिका)){
		पटिनिवत्त MOSQ_ERR_UNKNOWN;
	}

	packet = mosquitto_calloc(1, परिमाण(संरचना mosquitto__packet) + 1024 + WS_PACKET_OFFSET);
	सचे(!packet){
		SAFE_FREE(accept_कुञ्चिका);
		पटिनिवत्त MOSQ_ERR_NOMEM;
	}
	packet->packet_length = (uint32_t )snprintf((वर्ण *)&packet->payload[WS_PACKET_OFFSET], 1024,
			"HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: WebSocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: %s\r\n"
			"Sec-WebSocket-Protocol: %.*s\r\n"
			"\r\n", accept_कुञ्चिका, subprotocol_len, subprotocol) + WS_PACKET_OFFSET;
	SAFE_FREE(accept_कुञ्चिका);
	packet->to_process = packet->packet_length;

	memset(mosq->in_packet.packet_buffer, 0, db.config->packet_buffer_size);
	rc = packet__queue(mosq, packet);
	http__context_cleanup(mosq);
	ws__context_init(mosq);
	पटिनिवत्त rc;
}
#endif
