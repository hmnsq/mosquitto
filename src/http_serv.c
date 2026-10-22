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

#অন্তর্ভাব "config.h"

#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN

#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <errno.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto/mqtt_protocol.h"
#অন্তর্ভাব "net_mosq.h"
#অন্তর্ভাব "packet_mosq.h"
#অন্তর্ভাব "read_handle.h"
#অন্তর্ভাব "sys_tree.h"
#অন্তর্ভাব "util_mosq.h"
#অন্তর্ভাব "picohttpparser.h"


পূর্ণাঙ্ক http__context_init(সংরচনা mosquitto *context)
{
	context->transport = mosq_t_http;

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক http__context_cleanup(সংরচনা mosquitto *context)
{
	UNUSED(context);
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক http__read(সংরচনা mosquitto *mosq)
{
	ssize_t read_length;
	ssize_t header_length;
	গণনা mosquitto_client_state state;
	size_t hlen;
	ধ্রুব বর্ণ *http_method, *http_path;
	size_t http_method_len, http_path_len;
	পূর্ণাঙ্ক http_minor_version;
	size_t http_header_গুনতি = 100;
	সংরচনা phr_header http_headers[100];
	ধ্রুব বর্ণ *client_চাবি = রিক্ত;
	size_t client_চাবি_len = 0;
	বর্ণ *accept_চাবি;
	bool header_have_upgrade;
	bool header_have_connection;
	সংরচনা mosquitto__packet *packet;
	পূর্ণাঙ্ক rc;
	ধ্রুব বর্ণ *subprotocol = রিক্ত;
	পূর্ণাঙ্ক subprotocol_len = 0;

	যদি(!mosq){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}
	যদি(mosq->sock == INVALID_SOCKET){
		প্রত্যাগম MOSQ_ERR_NO_CONN;
	}

	state = mosquitto__get_state(mosq);
	যদি(state == mosq_cs_connect_pending){
		প্রত্যাগম MOSQ_ERR_SUCCESS;
	}

	hlen = strlen((বর্ণ *)mosq->in_packet.packet_buffer);
	read_length = net__read(mosq, &mosq->in_packet.packet_buffer[hlen], mosq->in_packet.packet_buffer_size-hlen);
	যদি(read_length <= 0){
		যদি(read_length == 0){
			প্রত্যাগম MOSQ_ERR_CONN_LOST; /* EOF */
		}
#ifdef WIN32
		errno = WSAGetLastError();
#endif
		যদি(errno == EAGAIN || errno == COMPAT_EWOULDBLOCK){
			প্রত্যাগম MOSQ_ERR_SUCCESS;
		}অন্যথা{
			বিকল্প(errno){
				পক্ষ COMPAT_ECONNRESET:
					প্রত্যাগম MOSQ_ERR_CONN_LOST;
				পক্ষ COMPAT_EINTR:
					প্রত্যাগম MOSQ_ERR_SUCCESS;
				স্বভাব:
					প্রত্যাগম MOSQ_ERR_ERRNO;
			}
		}
	}

	mosq->in_packet.packet_buffer[mosq->in_packet.packet_buffer_size-1] = '\0'; /* Always 0 terminate */
	header_length = phr_parse_request((বর্ণ *)mosq->in_packet.packet_buffer, strlen((বর্ণ *)mosq->in_packet.packet_buffer),
			&http_method, &http_method_len,
			&http_path, &http_path_len,
			&http_minor_version,
			http_headers, &http_header_গুনতি,
			0);
	// FIXME - deal with partial read !
	যদি(header_length == -2){
		// Partial read
		প্রত্যাগম MOSQ_ERR_SUCCESS;
	}অন্যথা যদি(header_length == -1){
		// Error
		প্রত্যাগম MOSQ_ERR_UNKNOWN;
	}অন্যথা যদি(header_length < read_length){
		/* Excess data which can't be handled because the client doesn't have a key yet */
		প্রত্যাগম MOSQ_ERR_MALFORMED_PACKET;
	}

	যদি(strncmp(http_method, "GET", http_method_len) && strncmp(http_method, "HEAD", http_method_len)){
		/* FIXME Not supported - send 501 response */
		প্রত্যাগম MOSQ_ERR_UNKNOWN;
	}

	header_have_upgrade = false;
	header_have_connection = false;
	subprotocol = রিক্ত;

	প্রতি(size_t i=0; i<http_header_গুনতি; i++){
		যদি(!strncasecmp(http_headers[i].name, "Upgrade", http_headers[i].name_len)){
			যদি(!strncasecmp(http_headers[i].মান, "websocket", http_headers[i].মান_len)){
				header_have_upgrade = true;
			}
		}অন্যথা যদি(!strncasecmp(http_headers[i].name, "Connection", http_headers[i].name_len)){
			/* Check for "upgrade" */
			ধ্রুব বর্ণ *str = http_headers[i].মান;
			size_t start = 0;
			size_t j;
			প্রতি(j=0; j<http_headers[i].মান_len; j++){
				যদি(str[j] == ','){
					যদি(!strncasecmp(&str[start], "upgrade", http_headers[i].মান_len-j)){
						header_have_connection = true;
						ভঙ্গ;
					}অন্যথা{
						start = j+1;
					}
				}অন্যথা যদি(str[j] == ' '){
					start = j+1;
				}
			}
			যদি(!strncasecmp(&str[start], "upgrade", http_headers[i].মান_len-j)){
				header_have_connection = true;
			}
		}অন্যথা যদি(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Key", http_headers[i].name_len)){
			client_চাবি = http_headers[i].মান;
			client_চাবি_len = http_headers[i].মান_len;
		}অন্যথা যদি(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Version", http_headers[i].name_len)){
			/* Check for "13" */
			যদি(http_headers[i].মান_len != 2
					|| http_headers[i].মান[0] != '1'
					|| http_headers[i].মান[1] != '3'
					){

				/* FIXME - not supported */
				প্রত্যাগম MOSQ_ERR_NOT_SUPPORTED;
			}
		}অন্যথা যদি(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Protocol", http_headers[i].name_len)){
			/* Check for "mqtt" */
			যদি(!strncmp(http_headers[i].মান, "mqtt", http_headers[i].মান_len)
					|| !strncmp(http_headers[i].মান, "mqttv3.1", http_headers[i].মান_len)){

				subprotocol = http_headers[i].মান;
				subprotocol_len = (পূর্ণাঙ্ক)http_headers[i].মান_len;
			}
		}অন্যথা যদি(!strncasecmp(http_headers[i].name, "X-Forwarded-For", http_headers[i].name_len)){
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
		}অন্যথা যদি(!strncasecmp(http_headers[i].name, "Origin", http_headers[i].name_len)){
			যদি(mosq->listener){
				bool have_match = false;
				প্রতি(পূর্ণাঙ্ক j=0; j<mosq->listener->ws_origin_গুনতি; j++){
					যদি(!strncmp(mosq->listener->ws_origins[j], http_headers[i].মান, http_headers[i].মান_len)){
						have_match = true;
						ভঙ্গ;
					}
				}
				যদি(!have_match && mosq->listener->ws_origin_গুনতি > 0){
					প্রত্যাগম MOSQ_ERR_HTTP_BAD_ORIGIN;
				}
			}
		}অন্যথা{
			/* Unknown header */
		}
	}

	যদি(subprotocol == রিক্ত){
		// FIXME ?
		প্রত্যাগম MOSQ_ERR_UNKNOWN;
	}

	যদি(header_have_upgrade == false || header_have_connection == false
			|| client_চাবি == রিক্ত || client_চাবি_len == 0){

		// FIXME - 404
		প্রত্যাগম MOSQ_ERR_UNKNOWN;
	}

	যদি(ws_create_accept_চাবি(client_চাবি, client_চাবি_len, &accept_চাবি)){
		প্রত্যাগম MOSQ_ERR_UNKNOWN;
	}

	packet = mosquitto_calloc(1, পরিমাণ(সংরচনা mosquitto__packet) + 1024 + WS_PACKET_OFFSET);
	যদি(!packet){
		SAFE_FREE(accept_চাবি);
		প্রত্যাগম MOSQ_ERR_NOMEM;
	}
	packet->packet_length = (uint32_t )snprintf((বর্ণ *)&packet->payload[WS_PACKET_OFFSET], 1024,
			"HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: WebSocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: %s\r\n"
			"Sec-WebSocket-Protocol: %.*s\r\n"
			"\r\n", accept_চাবি, subprotocol_len, subprotocol) + WS_PACKET_OFFSET;
	SAFE_FREE(accept_চাবি);
	packet->to_process = packet->packet_length;

	memset(mosq->in_packet.packet_buffer, 0, db.config->packet_buffer_size);
	rc = packet__queue(mosq, packet);
	http__context_cleanup(mosq);
	ws__context_init(mosq);
	প্রত্যাগম rc;
}
#endif
