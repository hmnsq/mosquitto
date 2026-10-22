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

#அந்தர்பாவ "config.h"

#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN

#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <errno.h>
#அந்தர்பாவ <string.h>

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto/mqtt_protocol.h"
#அந்தர்பாவ "net_mosq.h"
#அந்தர்பாவ "packet_mosq.h"
#அந்தர்பாவ "read_handle.h"
#அந்தர்பாவ "sys_tree.h"
#அந்தர்பாவ "util_mosq.h"
#அந்தர்பாவ "picohttpparser.h"


முழு http__context_init(கட்டமைப்பு mosquitto *context)
{
	context->transport = mosq_t_http;

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு http__context_cleanup(கட்டமைப்பு mosquitto *context)
{
	UNUSED(context);
	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு http__read(கட்டமைப்பு mosquitto *mosq)
{
	ssize_t read_length;
	ssize_t header_length;
	எண்ணிக்கை mosquitto_client_state state;
	size_t hlen;
	மாறிலி எழுத்து *http_method, *http_path;
	size_t http_method_len, http_path_len;
	முழு http_minor_version;
	size_t http_header_மொத்தம் = 100;
	கட்டமைப்பு phr_header http_headers[100];
	மாறிலி எழுத்து *client_சாவி = வெற்று;
	size_t client_சாவி_len = 0;
	எழுத்து *accept_சாவி;
	bool header_have_upgrade;
	bool header_have_connection;
	கட்டமைப்பு mosquitto__packet *packet;
	முழு rc;
	மாறிலி எழுத்து *subprotocol = வெற்று;
	முழு subprotocol_len = 0;

	எனில்(!mosq){
		திரும்பு MOSQ_ERR_INVAL;
	}
	எனில்(mosq->sock == INVALID_SOCKET){
		திரும்பு MOSQ_ERR_NO_CONN;
	}

	state = mosquitto__get_state(mosq);
	எனில்(state == mosq_cs_connect_pending){
		திரும்பு MOSQ_ERR_SUCCESS;
	}

	hlen = strlen((எழுத்து *)mosq->in_packet.packet_buffer);
	read_length = net__read(mosq, &mosq->in_packet.packet_buffer[hlen], mosq->in_packet.packet_buffer_size-hlen);
	எனில்(read_length <= 0){
		எனில்(read_length == 0){
			திரும்பு MOSQ_ERR_CONN_LOST; /* EOF */
		}
#ifdef WIN32
		errno = WSAGetLastError();
#endif
		எனில்(errno == EAGAIN || errno == COMPAT_EWOULDBLOCK){
			திரும்பு MOSQ_ERR_SUCCESS;
		}இல்லையெனில்{
			தேர்வு(errno){
				நிலை COMPAT_ECONNRESET:
					திரும்பு MOSQ_ERR_CONN_LOST;
				நிலை COMPAT_EINTR:
					திரும்பு MOSQ_ERR_SUCCESS;
				இயல்பு:
					திரும்பு MOSQ_ERR_ERRNO;
			}
		}
	}

	mosq->in_packet.packet_buffer[mosq->in_packet.packet_buffer_size-1] = '\0'; /* Always 0 terminate */
	header_length = phr_parse_request((எழுத்து *)mosq->in_packet.packet_buffer, strlen((எழுத்து *)mosq->in_packet.packet_buffer),
			&http_method, &http_method_len,
			&http_path, &http_path_len,
			&http_minor_version,
			http_headers, &http_header_மொத்தம்,
			0);
	// FIXME - deal with partial read !
	எனில்(header_length == -2){
		// Partial read
		திரும்பு MOSQ_ERR_SUCCESS;
	}இல்லையெனில் எனில்(header_length == -1){
		// Error
		திரும்பு MOSQ_ERR_UNKNOWN;
	}இல்லையெனில் எனில்(header_length < read_length){
		/* Excess data which can't be handled because the client doesn't have a key yet */
		திரும்பு MOSQ_ERR_MALFORMED_PACKET;
	}

	எனில்(strncmp(http_method, "GET", http_method_len) && strncmp(http_method, "HEAD", http_method_len)){
		/* FIXME Not supported - send 501 response */
		திரும்பு MOSQ_ERR_UNKNOWN;
	}

	header_have_upgrade = false;
	header_have_connection = false;
	subprotocol = வெற்று;

	க்கு(size_t i=0; i<http_header_மொத்தம்; i++){
		எனில்(!strncasecmp(http_headers[i].name, "Upgrade", http_headers[i].name_len)){
			எனில்(!strncasecmp(http_headers[i].மதிப்பு, "websocket", http_headers[i].மதிப்பு_len)){
				header_have_upgrade = true;
			}
		}இல்லையெனில் எனில்(!strncasecmp(http_headers[i].name, "Connection", http_headers[i].name_len)){
			/* Check for "upgrade" */
			மாறிலி எழுத்து *str = http_headers[i].மதிப்பு;
			size_t start = 0;
			size_t j;
			க்கு(j=0; j<http_headers[i].மதிப்பு_len; j++){
				எனில்(str[j] == ','){
					எனில்(!strncasecmp(&str[start], "upgrade", http_headers[i].மதிப்பு_len-j)){
						header_have_connection = true;
						நிறுத்து;
					}இல்லையெனில்{
						start = j+1;
					}
				}இல்லையெனில் எனில்(str[j] == ' '){
					start = j+1;
				}
			}
			எனில்(!strncasecmp(&str[start], "upgrade", http_headers[i].மதிப்பு_len-j)){
				header_have_connection = true;
			}
		}இல்லையெனில் எனில்(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Key", http_headers[i].name_len)){
			client_சாவி = http_headers[i].மதிப்பு;
			client_சாவி_len = http_headers[i].மதிப்பு_len;
		}இல்லையெனில் எனில்(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Version", http_headers[i].name_len)){
			/* Check for "13" */
			எனில்(http_headers[i].மதிப்பு_len != 2
					|| http_headers[i].மதிப்பு[0] != '1'
					|| http_headers[i].மதிப்பு[1] != '3'
					){

				/* FIXME - not supported */
				திரும்பு MOSQ_ERR_NOT_SUPPORTED;
			}
		}இல்லையெனில் எனில்(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Protocol", http_headers[i].name_len)){
			/* Check for "mqtt" */
			எனில்(!strncmp(http_headers[i].மதிப்பு, "mqtt", http_headers[i].மதிப்பு_len)
					|| !strncmp(http_headers[i].மதிப்பு, "mqttv3.1", http_headers[i].மதிப்பு_len)){

				subprotocol = http_headers[i].மதிப்பு;
				subprotocol_len = (முழு)http_headers[i].மதிப்பு_len;
			}
		}இல்லையெனில் எனில்(!strncasecmp(http_headers[i].name, "X-Forwarded-For", http_headers[i].name_len)){
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
		}இல்லையெனில் எனில்(!strncasecmp(http_headers[i].name, "Origin", http_headers[i].name_len)){
			எனில்(mosq->listener){
				bool have_match = false;
				க்கு(முழு j=0; j<mosq->listener->ws_origin_மொத்தம்; j++){
					எனில்(!strncmp(mosq->listener->ws_origins[j], http_headers[i].மதிப்பு, http_headers[i].மதிப்பு_len)){
						have_match = true;
						நிறுத்து;
					}
				}
				எனில்(!have_match && mosq->listener->ws_origin_மொத்தம் > 0){
					திரும்பு MOSQ_ERR_HTTP_BAD_ORIGIN;
				}
			}
		}இல்லையெனில்{
			/* Unknown header */
		}
	}

	எனில்(subprotocol == வெற்று){
		// FIXME ?
		திரும்பு MOSQ_ERR_UNKNOWN;
	}

	எனில்(header_have_upgrade == false || header_have_connection == false
			|| client_சாவி == வெற்று || client_சாவி_len == 0){

		// FIXME - 404
		திரும்பு MOSQ_ERR_UNKNOWN;
	}

	எனில்(ws_create_accept_சாவி(client_சாவி, client_சாவி_len, &accept_சாவி)){
		திரும்பு MOSQ_ERR_UNKNOWN;
	}

	packet = mosquitto_calloc(1, அளவு(கட்டமைப்பு mosquitto__packet) + 1024 + WS_PACKET_OFFSET);
	எனில்(!packet){
		SAFE_FREE(accept_சாவி);
		திரும்பு MOSQ_ERR_NOMEM;
	}
	packet->packet_length = (uint32_t )snprintf((எழுத்து *)&packet->payload[WS_PACKET_OFFSET], 1024,
			"HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: WebSocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: %s\r\n"
			"Sec-WebSocket-Protocol: %.*s\r\n"
			"\r\n", accept_சாவி, subprotocol_len, subprotocol) + WS_PACKET_OFFSET;
	SAFE_FREE(accept_சாவி);
	packet->to_process = packet->packet_length;

	memset(mosq->in_packet.packet_buffer, 0, db.config->packet_buffer_size);
	rc = packet__queue(mosq, packet);
	http__context_cleanup(mosq);
	ws__context_init(mosq);
	திரும்பு rc;
}
#endif
