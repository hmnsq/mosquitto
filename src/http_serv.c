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

#include "config.h"

#もし defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "mosquitto_broker_internal.h"
#include "mosquitto/mqtt_protocol.h"
#include "net_mosq.h"
#include "packet_mosq.h"
#include "read_handle.h"
#include "sys_tree.h"
#include "util_mosq.h"
#include "picohttpparser.h"


整数 http__context_init(struct mosquitto *context)
{
	context->transport = mosq_t_http;

	戻る MOSQ_ERR_SUCCESS;
}


整数 http__context_cleanup(struct mosquitto *context)
{
	UNUSED(context);
	戻る MOSQ_ERR_SUCCESS;
}


整数 http__read(struct mosquitto *mosq)
{
	ssize_t read_length;
	ssize_t header_length;
	enum mosquitto_client_state state;
	size_t hlen;
	const char *http_method, *http_path;
	size_t http_method_len, http_path_len;
	整数 http_minor_version;
	size_t http_header_count = 100;
	struct phr_header http_headers[100];
	const char *client_key = NULL;
	size_t client_key_len = 0;
	char *accept_key;
	bool header_have_upgrade;
	bool header_have_connection;
	struct mosquitto__packet *packet;
	整数 rc;
	const char *subprotocol = NULL;
	整数 subprotocol_len = 0;

	もし(!mosq){
		戻る MOSQ_ERR_INVAL;
	}
	もし(mosq->sock == INVALID_SOCKET){
		戻る MOSQ_ERR_NO_CONN;
	}

	state = mosquitto__get_state(mosq);
	もし(state == mosq_cs_connect_pending){
		戻る MOSQ_ERR_SUCCESS;
	}

	hlen = strlen((char *)mosq->in_packet.packet_buffer);
	read_length = net__read(mosq, &mosq->in_packet.packet_buffer[hlen], mosq->in_packet.packet_buffer_size-hlen);
	もし(read_length <= 0){
		もし(read_length == 0){
			戻る MOSQ_ERR_CONN_LOST; /* EOF */
		}
#ifdef WIN32
		errno = WSAGetLastError();
#endif
		もし(errno == EAGAIN || errno == COMPAT_EWOULDBLOCK){
			戻る MOSQ_ERR_SUCCESS;
		}否则{
			switch(errno){
				case COMPAT_ECONNRESET:
					戻る MOSQ_ERR_CONN_LOST;
				case COMPAT_EINTR:
					戻る MOSQ_ERR_SUCCESS;
				default:
					戻る MOSQ_ERR_ERRNO;
			}
		}
	}

	mosq->in_packet.packet_buffer[mosq->in_packet.packet_buffer_size-1] = '\0'; /* Always 0 terminate */
	header_length = phr_parse_request((char *)mosq->in_packet.packet_buffer, strlen((char *)mosq->in_packet.packet_buffer),
			&http_method, &http_method_len,
			&http_path, &http_path_len,
			&http_minor_version,
			http_headers, &http_header_count,
			0);
	// FIXME - deal with partial read !
	もし(header_length == -2){
		// Partial read
		戻る MOSQ_ERR_SUCCESS;
	}否则 もし(header_length == -1){
		// Error
		戻る MOSQ_ERR_UNKNOWN;
	}否则 もし(header_length < read_length){
		/* Excess data which can't be handled because the client doesn't have a key yet */
		戻る MOSQ_ERR_MALFORMED_PACKET;
	}

	もし(strncmp(http_method, "GET", http_method_len) && strncmp(http_method, "HEAD", http_method_len)){
		/* FIXME Not supported - send 501 response */
		戻る MOSQ_ERR_UNKNOWN;
	}

	header_have_upgrade = false;
	header_have_connection = false;
	subprotocol = NULL;

	ために(size_t i=0; i<http_header_count; i++){
		もし(!strncasecmp(http_headers[i].name, "Upgrade", http_headers[i].name_len)){
			もし(!strncasecmp(http_headers[i].value, "websocket", http_headers[i].value_len)){
				header_have_upgrade = true;
			}
		}否则 もし(!strncasecmp(http_headers[i].name, "Connection", http_headers[i].name_len)){
			/* Check for "upgrade" */
			const char *str = http_headers[i].value;
			size_t start = 0;
			size_t j;
			ために(j=0; j<http_headers[i].value_len; j++){
				もし(str[j] == ','){
					もし(!strncasecmp(&str[start], "upgrade", http_headers[i].value_len-j)){
						header_have_connection = true;
						break;
					}否则{
						start = j+1;
					}
				}否则 もし(str[j] == ' '){
					start = j+1;
				}
			}
			もし(!strncasecmp(&str[start], "upgrade", http_headers[i].value_len-j)){
				header_have_connection = true;
			}
		}否则 もし(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Key", http_headers[i].name_len)){
			client_key = http_headers[i].value;
			client_key_len = http_headers[i].value_len;
		}否则 もし(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Version", http_headers[i].name_len)){
			/* Check for "13" */
			もし(http_headers[i].value_len != 2
					|| http_headers[i].value[0] != '1'
					|| http_headers[i].value[1] != '3'
					){

				/* FIXME - not supported */
				戻る MOSQ_ERR_NOT_SUPPORTED;
			}
		}否则 もし(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Protocol", http_headers[i].name_len)){
			/* Check for "mqtt" */
			もし(!strncmp(http_headers[i].value, "mqtt", http_headers[i].value_len)
					|| !strncmp(http_headers[i].value, "mqttv3.1", http_headers[i].value_len)){

				subprotocol = http_headers[i].value;
				subprotocol_len = (整数)http_headers[i].value_len;
			}
		}否则 もし(!strncasecmp(http_headers[i].name, "X-Forwarded-For", http_headers[i].name_len)){
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
		}否则 もし(!strncasecmp(http_headers[i].name, "Origin", http_headers[i].name_len)){
			もし(mosq->listener){
				bool have_match = false;
				ために(整数 j=0; j<mosq->listener->ws_origin_count; j++){
					もし(!strncmp(mosq->listener->ws_origins[j], http_headers[i].value, http_headers[i].value_len)){
						have_match = true;
						break;
					}
				}
				もし(!have_match && mosq->listener->ws_origin_count > 0){
					戻る MOSQ_ERR_HTTP_BAD_ORIGIN;
				}
			}
		}否则{
			/* Unknown header */
		}
	}

	もし(subprotocol == NULL){
		// FIXME ?
		戻る MOSQ_ERR_UNKNOWN;
	}

	もし(header_have_upgrade == false || header_have_connection == false
			|| client_key == NULL || client_key_len == 0){

		// FIXME - 404
		戻る MOSQ_ERR_UNKNOWN;
	}

	もし(ws__create_accept_key(client_key, client_key_len, &accept_key)){
		戻る MOSQ_ERR_UNKNOWN;
	}

	packet = mosquitto_calloc(1, sizeof(struct mosquitto__packet) + 1024 + WS_PACKET_OFFSET);
	もし(!packet){
		SAFE_FREE(accept_key);
		戻る MOSQ_ERR_NOMEM;
	}
	packet->packet_length = (uint32_t )snprintf((char *)&packet->payload[WS_PACKET_OFFSET], 1024,
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
	戻る rc;
}
#endif
