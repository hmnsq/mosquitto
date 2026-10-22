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

#ပေႃး defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN

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


puur_naa_nka http__context_init(sa_nracanaa mosquitto *context)
{
	context->transport = mosq_t_http;

	ပွၵ်ႈ MOSQ_ERR_SUCCESS;
}


puur_naa_nka http__context_cleanup(sa_nracanaa mosquitto *context)
{
	UNUSED(context);
	ပွၵ်ႈ MOSQ_ERR_SUCCESS;
}


puur_naa_nka http__read(sa_nracanaa mosquitto *mosq)
{
	ssize_t read_length;
	ssize_t header_length;
	ga_nanaa mosquitto_client_state state;
	size_t hlen;
	const var_na *http_method, *http_path;
	size_t http_method_len, http_path_len;
	puur_naa_nka http_minor_version;
	size_t http_header_count = 100;
	sa_nracanaa phr_header http_headers[100];
	const var_na *client_key = NULL;
	size_t client_key_len = 0;
	var_na *accept_key;
	bool header_have_upgrade;
	bool header_have_connection;
	sa_nracanaa mosquitto__packet *packet;
	puur_naa_nka rc;
	const var_na *subprotocol = NULL;
	puur_naa_nka subprotocol_len = 0;

	ပေႃး(!mosq){
		ပွၵ်ႈ MOSQ_ERR_INVAL;
	}
	ပေႃး(mosq->sock == INVALID_SOCKET){
		ပွၵ်ႈ MOSQ_ERR_NO_CONN;
	}

	state = mosquitto__get_state(mosq);
	ပေႃး(state == mosq_cs_connect_pending){
		ပွၵ်ႈ MOSQ_ERR_SUCCESS;
	}

	hlen = strlen((var_na *)mosq->in_packet.packet_buffer);
	read_length = net__read(mosq, &mosq->in_packet.packet_buffer[hlen], mosq->in_packet.packet_buffer_size-hlen);
	ပေႃး(read_length <= 0){
		ပေႃး(read_length == 0){
			ပွၵ်ႈ MOSQ_ERR_CONN_LOST; /* EOF */
		}
#ifdef WIN32
		errno = WSAGetLastError();
#endif
		ပေႃး(errno == EAGAIN || errno == COMPAT_EWOULDBLOCK){
			ပွၵ်ႈ MOSQ_ERR_SUCCESS;
		}ပေႃးဢမ်ႇ{
			switch(errno){
				pak_sa COMPAT_ECONNRESET:
					ပွၵ်ႈ MOSQ_ERR_CONN_LOST;
				pak_sa COMPAT_EINTR:
					ပွၵ်ႈ MOSQ_ERR_SUCCESS;
				default:
					ပွၵ်ႈ MOSQ_ERR_ERRNO;
			}
		}
	}

	mosq->in_packet.packet_buffer[mosq->in_packet.packet_buffer_size-1] = '\0'; /* Always 0 terminate */
	header_length = phr_parse_request((var_na *)mosq->in_packet.packet_buffer, strlen((var_na *)mosq->in_packet.packet_buffer),
			&http_method, &http_method_len,
			&http_path, &http_path_len,
			&http_minor_version,
			http_headers, &http_header_count,
			0);
	// FIXME - deal with partial read !
	ပေႃး(header_length == -2){
		// Partial read
		ပွၵ်ႈ MOSQ_ERR_SUCCESS;
	}ပေႃးဢမ်ႇ ပေႃး(header_length == -1){
		// Error
		ပွၵ်ႈ MOSQ_ERR_UNKNOWN;
	}ပေႃးဢမ်ႇ ပေႃး(header_length < read_length){
		/* Excess data which can't be handled because the client doesn't have a key yet */
		ပွၵ်ႈ MOSQ_ERR_MALFORMED_PACKET;
	}

	ပေႃး(strncmp(http_method, "GET", http_method_len) && strncmp(http_method, "HEAD", http_method_len)){
		/* FIXME Not supported - send 501 response */
		ပွၵ်ႈ MOSQ_ERR_UNKNOWN;
	}

	header_have_upgrade = false;
	header_have_connection = false;
	subprotocol = NULL;

	for(size_t i=0; i<http_header_count; i++){
		ပေႃး(!strncasecmp(http_headers[i].name, "Upgrade", http_headers[i].name_len)){
			ပေႃး(!strncasecmp(http_headers[i].value, "websocket", http_headers[i].value_len)){
				header_have_upgrade = true;
			}
		}ပေႃးဢမ်ႇ ပေႃး(!strncasecmp(http_headers[i].name, "Connection", http_headers[i].name_len)){
			/* Check for "upgrade" */
			const var_na *str = http_headers[i].value;
			size_t start = 0;
			size_t j;
			for(j=0; j<http_headers[i].value_len; j++){
				ပေႃး(str[j] == ','){
					ပေႃး(!strncasecmp(&str[start], "upgrade", http_headers[i].value_len-j)){
						header_have_connection = true;
						bha_nga;
					}ပေႃးဢမ်ႇ{
						start = j+1;
					}
				}ပေႃးဢမ်ႇ ပေႃး(str[j] == ' '){
					start = j+1;
				}
			}
			ပေႃး(!strncasecmp(&str[start], "upgrade", http_headers[i].value_len-j)){
				header_have_connection = true;
			}
		}ပေႃးဢမ်ႇ ပေႃး(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Key", http_headers[i].name_len)){
			client_key = http_headers[i].value;
			client_key_len = http_headers[i].value_len;
		}ပေႃးဢမ်ႇ ပေႃး(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Version", http_headers[i].name_len)){
			/* Check for "13" */
			ပေႃး(http_headers[i].value_len != 2
					|| http_headers[i].value[0] != '1'
					|| http_headers[i].value[1] != '3'
					){

				/* FIXME - not supported */
				ပွၵ်ႈ MOSQ_ERR_NOT_SUPPORTED;
			}
		}ပေႃးဢမ်ႇ ပေႃး(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Protocol", http_headers[i].name_len)){
			/* Check for "mqtt" */
			ပေႃး(!strncmp(http_headers[i].value, "mqtt", http_headers[i].value_len)
					|| !strncmp(http_headers[i].value, "mqttv3.1", http_headers[i].value_len)){

				subprotocol = http_headers[i].value;
				subprotocol_len = (puur_naa_nka)http_headers[i].value_len;
			}
		}ပေႃးဢမ်ႇ ပေႃး(!strncasecmp(http_headers[i].name, "X-Forwarded-For", http_headers[i].name_len)){
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
		}ပေႃးဢမ်ႇ ပေႃး(!strncasecmp(http_headers[i].name, "Origin", http_headers[i].name_len)){
			ပေႃး(mosq->listener){
				bool have_match = false;
				for(puur_naa_nka j=0; j<mosq->listener->ws_origin_count; j++){
					ပေႃး(!strncmp(mosq->listener->ws_origins[j], http_headers[i].value, http_headers[i].value_len)){
						have_match = true;
						bha_nga;
					}
				}
				ပေႃး(!have_match && mosq->listener->ws_origin_count > 0){
					ပွၵ်ႈ MOSQ_ERR_HTTP_BAD_ORIGIN;
				}
			}
		}ပေႃးဢမ်ႇ{
			/* Unknown header */
		}
	}

	ပေႃး(subprotocol == NULL){
		// FIXME ?
		ပွၵ်ႈ MOSQ_ERR_UNKNOWN;
	}

	ပေႃး(header_have_upgrade == false || header_have_connection == false
			|| client_key == NULL || client_key_len == 0){

		// FIXME - 404
		ပွၵ်ႈ MOSQ_ERR_UNKNOWN;
	}

	ပေႃး(ws__create_accept_key(client_key, client_key_len, &accept_key)){
		ပွၵ်ႈ MOSQ_ERR_UNKNOWN;
	}

	packet = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__packet) + 1024 + WS_PACKET_OFFSET);
	ပေႃး(!packet){
		SAFE_FREE(accept_key);
		ပွၵ်ႈ MOSQ_ERR_NOMEM;
	}
	packet->packet_length = (uint32_t )snprintf((var_na *)&packet->payload[WS_PACKET_OFFSET], 1024,
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
	ပွၵ်ႈ rc;
}
#endif
