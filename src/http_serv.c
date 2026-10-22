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

#ضمن "config.h"

#אם defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN

#ضمن <assert.h>
#ضمن <errno.h>
#ضمن <string.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "net_mosq.h"
#ضمن "packet_mosq.h"
#ضمن "read_handle.h"
#ضمن "sys_tree.h"
#ضمن "util_mosq.h"
#ضمن "picohttpparser.h"


صحيح http__context_init(بنية mosquitto *context)
{
	context->transport = mosq_t_http;

	החזר MOSQ_ERR_SUCCESS;
}


صحيح http__context_cleanup(بنية mosquitto *context)
{
	UNUSED(context);
	החזר MOSQ_ERR_SUCCESS;
}


صحيح http__read(بنية mosquitto *mosq)
{
	ssize_t read_length;
	ssize_t header_length;
	تعداد mosquitto_client_state state;
	size_t hlen;
	ثابت حرف *http_method, *http_path;
	size_t http_method_len, http_path_len;
	صحيح http_minor_version;
	size_t http_header_count = 100;
	بنية phr_header http_headers[100];
	ثابت حرف *client_key = عدم;
	size_t client_key_len = 0;
	حرف *accept_key;
	bool header_have_upgrade;
	bool header_have_connection;
	بنية mosquitto__packet *packet;
	صحيح rc;
	ثابت حرف *subprotocol = عدم;
	صحيح subprotocol_len = 0;

	אם(!mosq){
		החזר MOSQ_ERR_INVAL;
	}
	אם(mosq->sock == INVALID_SOCKET){
		החזר MOSQ_ERR_NO_CONN;
	}

	state = mosquitto__get_state(mosq);
	אם(state == mosq_cs_connect_pending){
		החזר MOSQ_ERR_SUCCESS;
	}

	hlen = strlen((حرف *)mosq->in_packet.packet_buffer);
	read_length = net__read(mosq, &mosq->in_packet.packet_buffer[hlen], mosq->in_packet.packet_buffer_size-hlen);
	אם(read_length <= 0){
		אם(read_length == 0){
			החזר MOSQ_ERR_CONN_LOST; /* EOF */
		}
#ifdef WIN32
		errno = WSAGetLastError();
#endif
		אם(errno == EAGAIN || errno == COMPAT_EWOULDBLOCK){
			החזר MOSQ_ERR_SUCCESS;
		}אחרת{
			بدل(errno){
				حالة COMPAT_ECONNRESET:
					החזר MOSQ_ERR_CONN_LOST;
				حالة COMPAT_EINTR:
					החזר MOSQ_ERR_SUCCESS;
				افتراضي:
					החזר MOSQ_ERR_ERRNO;
			}
		}
	}

	mosq->in_packet.packet_buffer[mosq->in_packet.packet_buffer_size-1] = '\0'; /* Always 0 terminate */
	header_length = phr_parse_request((حرف *)mosq->in_packet.packet_buffer, strlen((حرف *)mosq->in_packet.packet_buffer),
			&http_method, &http_method_len,
			&http_path, &http_path_len,
			&http_minor_version,
			http_headers, &http_header_count,
			0);
	// FIXME - deal with partial read !
	אם(header_length == -2){
		// Partial read
		החזר MOSQ_ERR_SUCCESS;
	}אחרת אם(header_length == -1){
		// Error
		החזר MOSQ_ERR_UNKNOWN;
	}אחרת אם(header_length < read_length){
		/* Excess data which can't be handled because the client doesn't have a key yet */
		החזר MOSQ_ERR_MALFORMED_PACKET;
	}

	אם(strncmp(http_method, "GET", http_method_len) && strncmp(http_method, "HEAD", http_method_len)){
		/* FIXME Not supported - send 501 response */
		החזר MOSQ_ERR_UNKNOWN;
	}

	header_have_upgrade = false;
	header_have_connection = false;
	subprotocol = عدم;

	עבור(size_t i=0; i<http_header_count; i++){
		אם(!strncasecmp(http_headers[i].name, "Upgrade", http_headers[i].name_len)){
			אם(!strncasecmp(http_headers[i].value, "websocket", http_headers[i].value_len)){
				header_have_upgrade = true;
			}
		}אחרת אם(!strncasecmp(http_headers[i].name, "Connection", http_headers[i].name_len)){
			/* Check for "upgrade" */
			ثابت حرف *str = http_headers[i].value;
			size_t start = 0;
			size_t j;
			עבור(j=0; j<http_headers[i].value_len; j++){
				אם(str[j] == ','){
					אם(!strncasecmp(&str[start], "upgrade", http_headers[i].value_len-j)){
						header_have_connection = true;
						اكسر;
					}אחרת{
						start = j+1;
					}
				}אחרת אם(str[j] == ' '){
					start = j+1;
				}
			}
			אם(!strncasecmp(&str[start], "upgrade", http_headers[i].value_len-j)){
				header_have_connection = true;
			}
		}אחרת אם(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Key", http_headers[i].name_len)){
			client_key = http_headers[i].value;
			client_key_len = http_headers[i].value_len;
		}אחרת אם(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Version", http_headers[i].name_len)){
			/* Check for "13" */
			אם(http_headers[i].value_len != 2
					|| http_headers[i].value[0] != '1'
					|| http_headers[i].value[1] != '3'
					){

				/* FIXME - not supported */
				החזר MOSQ_ERR_NOT_SUPPORTED;
			}
		}אחרת אם(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Protocol", http_headers[i].name_len)){
			/* Check for "mqtt" */
			אם(!strncmp(http_headers[i].value, "mqtt", http_headers[i].value_len)
					|| !strncmp(http_headers[i].value, "mqttv3.1", http_headers[i].value_len)){

				subprotocol = http_headers[i].value;
				subprotocol_len = (صحيح)http_headers[i].value_len;
			}
		}אחרת אם(!strncasecmp(http_headers[i].name, "X-Forwarded-For", http_headers[i].name_len)){
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
		}אחרת אם(!strncasecmp(http_headers[i].name, "Origin", http_headers[i].name_len)){
			אם(mosq->listener){
				bool have_match = false;
				עבור(صحيح j=0; j<mosq->listener->ws_origin_count; j++){
					אם(!strncmp(mosq->listener->ws_origins[j], http_headers[i].value, http_headers[i].value_len)){
						have_match = true;
						اكسر;
					}
				}
				אם(!have_match && mosq->listener->ws_origin_count > 0){
					החזר MOSQ_ERR_HTTP_BAD_ORIGIN;
				}
			}
		}אחרת{
			/* Unknown header */
		}
	}

	אם(subprotocol == عدم){
		// FIXME ?
		החזר MOSQ_ERR_UNKNOWN;
	}

	אם(header_have_upgrade == false || header_have_connection == false
			|| client_key == عدم || client_key_len == 0){

		// FIXME - 404
		החזר MOSQ_ERR_UNKNOWN;
	}

	אם(ws__create_accept_key(client_key, client_key_len, &accept_key)){
		החזר MOSQ_ERR_UNKNOWN;
	}

	packet = mosquitto_calloc(1, حجم(بنية mosquitto__packet) + 1024 + WS_PACKET_OFFSET);
	אם(!packet){
		SAFE_FREE(accept_key);
		החזר MOSQ_ERR_NOMEM;
	}
	packet->packet_length = (uint32_t )snprintf((حرف *)&packet->payload[WS_PACKET_OFFSET], 1024,
			"HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: WebSocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: %s\r\n"
			"Sec-WebSocket-Protocol: %.*s\r\n"
			"\r\n", accept_key, subprotocol_len, subprotocol) + WS_PACKET_OFFSET;
	SAFE_FREE(accept_key);
	packet->to_process = packet->packet_length;

	memset(mosq->in_packet.packet_buffer, 0, db.config->packet_buffer_size);
	rc = packet__queue(mosq, packet);
	http__context_cleanup(mosq);
	ws__context_init(mosq);
	החזר rc;
}
#endif
