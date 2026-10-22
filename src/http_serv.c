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

#ако defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN

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


цяло http__context_init(struct mosquitto *context)
{
	context->transport = mosq_t_http;

	върни MOSQ_ERR_SUCCESS;
}


цяло http__context_cleanup(struct mosquitto *context)
{
	UNUSED(context);
	върни MOSQ_ERR_SUCCESS;
}


цяло http__read(struct mosquitto *mosq)
{
	ssize_t read_length;
	ssize_t header_length;
	enum mosquitto_client_state state;
	size_t hlen;
	const char *http_method, *http_path;
	size_t http_method_len, http_path_len;
	цяло http_minor_version;
	size_t http_header_count = 100;
	struct phr_header http_headers[100];
	const char *client_key = NULL;
	size_t client_key_len = 0;
	char *accept_key;
	bool header_have_upgrade;
	bool header_have_connection;
	struct mosquitto__packet *packet;
	цяло rc;
	const char *subprotocol = NULL;
	цяло subprotocol_len = 0;

	ако(!mosq){
		върни MOSQ_ERR_INVAL;
	}
	ако(mosq->sock == INVALID_SOCKET){
		върни MOSQ_ERR_NO_CONN;
	}

	state = mosquitto__get_state(mosq);
	ако(state == mosq_cs_connect_pending){
		върни MOSQ_ERR_SUCCESS;
	}

	hlen = strlen((char *)mosq->in_packet.packet_buffer);
	read_length = net__read(mosq, &mosq->in_packet.packet_buffer[hlen], mosq->in_packet.packet_buffer_size-hlen);
	ако(read_length <= 0){
		ако(read_length == 0){
			върни MOSQ_ERR_CONN_LOST; /* EOF */
		}
#ifdef WIN32
		errno = WSAGetLastError();
#endif
		ако(errno == EAGAIN || errno == COMPAT_EWOULDBLOCK){
			върни MOSQ_ERR_SUCCESS;
		}иначе{
			switch(errno){
				case COMPAT_ECONNRESET:
					върни MOSQ_ERR_CONN_LOST;
				case COMPAT_EINTR:
					върни MOSQ_ERR_SUCCESS;
				default:
					върни MOSQ_ERR_ERRNO;
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
	ако(header_length == -2){
		// Partial read
		върни MOSQ_ERR_SUCCESS;
	}иначе ако(header_length == -1){
		// Error
		върни MOSQ_ERR_UNKNOWN;
	}иначе ако(header_length < read_length){
		/* Excess data which can't be handled because the client doesn't have a key yet */
		върни MOSQ_ERR_MALFORMED_PACKET;
	}

	ако(strncmp(http_method, "GET", http_method_len) && strncmp(http_method, "HEAD", http_method_len)){
		/* FIXME Not supported - send 501 response */
		върни MOSQ_ERR_UNKNOWN;
	}

	header_have_upgrade = false;
	header_have_connection = false;
	subprotocol = NULL;

	за(size_t i=0; i<http_header_count; i++){
		ако(!strncasecmp(http_headers[i].name, "Upgrade", http_headers[i].name_len)){
			ако(!strncasecmp(http_headers[i].value, "websocket", http_headers[i].value_len)){
				header_have_upgrade = true;
			}
		}иначе ако(!strncasecmp(http_headers[i].name, "Connection", http_headers[i].name_len)){
			/* Check for "upgrade" */
			const char *str = http_headers[i].value;
			size_t start = 0;
			size_t j;
			за(j=0; j<http_headers[i].value_len; j++){
				ако(str[j] == ','){
					ако(!strncasecmp(&str[start], "upgrade", http_headers[i].value_len-j)){
						header_have_connection = true;
						break;
					}иначе{
						start = j+1;
					}
				}иначе ако(str[j] == ' '){
					start = j+1;
				}
			}
			ако(!strncasecmp(&str[start], "upgrade", http_headers[i].value_len-j)){
				header_have_connection = true;
			}
		}иначе ако(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Key", http_headers[i].name_len)){
			client_key = http_headers[i].value;
			client_key_len = http_headers[i].value_len;
		}иначе ако(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Version", http_headers[i].name_len)){
			/* Check for "13" */
			ако(http_headers[i].value_len != 2
					|| http_headers[i].value[0] != '1'
					|| http_headers[i].value[1] != '3'
					){

				/* FIXME - not supported */
				върни MOSQ_ERR_NOT_SUPPORTED;
			}
		}иначе ако(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Protocol", http_headers[i].name_len)){
			/* Check for "mqtt" */
			ако(!strncmp(http_headers[i].value, "mqtt", http_headers[i].value_len)
					|| !strncmp(http_headers[i].value, "mqttv3.1", http_headers[i].value_len)){

				subprotocol = http_headers[i].value;
				subprotocol_len = (цяло)http_headers[i].value_len;
			}
		}иначе ако(!strncasecmp(http_headers[i].name, "X-Forwarded-For", http_headers[i].name_len)){
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
		}иначе ако(!strncasecmp(http_headers[i].name, "Origin", http_headers[i].name_len)){
			ако(mosq->listener){
				bool have_match = false;
				за(цяло j=0; j<mosq->listener->ws_origin_count; j++){
					ако(!strncmp(mosq->listener->ws_origins[j], http_headers[i].value, http_headers[i].value_len)){
						have_match = true;
						break;
					}
				}
				ако(!have_match && mosq->listener->ws_origin_count > 0){
					върни MOSQ_ERR_HTTP_BAD_ORIGIN;
				}
			}
		}иначе{
			/* Unknown header */
		}
	}

	ако(subprotocol == NULL){
		// FIXME ?
		върни MOSQ_ERR_UNKNOWN;
	}

	ако(header_have_upgrade == false || header_have_connection == false
			|| client_key == NULL || client_key_len == 0){

		// FIXME - 404
		върни MOSQ_ERR_UNKNOWN;
	}

	ако(ws__create_accept_key(client_key, client_key_len, &accept_key)){
		върни MOSQ_ERR_UNKNOWN;
	}

	packet = mosquitto_calloc(1, sizeof(struct mosquitto__packet) + 1024 + WS_PACKET_OFFSET);
	ако(!packet){
		SAFE_FREE(accept_key);
		върни MOSQ_ERR_NOMEM;
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
	върни rc;
}
#endif
