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

#અન્તર્ભાવ "config.h"

#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN

#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <errno.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"
#અન્તર્ભાવ "net_mosq.h"
#અન્તર્ભાવ "packet_mosq.h"
#અન્તર્ભાવ "read_handle.h"
#અન્તર્ભાવ "sys_tree.h"
#અન્તર્ભાવ "util_mosq.h"
#અન્તર્ભાવ "picohttpparser.h"


પૂર્ણાંક http__context_init(સંરચના mosquitto *context)
{
	context->transport = mosq_t_http;

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક http__context_cleanup(સંરચના mosquitto *context)
{
	UNUSED(context);
	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક http__read(સંરચના mosquitto *mosq)
{
	ssize_t read_length;
	ssize_t header_length;
	ગણના mosquitto_client_state state;
	size_t hlen;
	અચલ અક્ષર *http_method, *http_path;
	size_t http_method_len, http_path_len;
	પૂર્ણાંક http_minor_version;
	size_t http_header_ગણતરી = 100;
	સંરચના phr_header http_headers[100];
	અચલ અક્ષર *client_ચાવી = ખાલી;
	size_t client_ચાવી_len = 0;
	અક્ષર *accept_ચાવી;
	bool header_have_upgrade;
	bool header_have_connection;
	સંરચના mosquitto__packet *packet;
	પૂર્ણાંક rc;
	અચલ અક્ષર *subprotocol = ખાલી;
	પૂર્ણાંક subprotocol_len = 0;

	જો(!mosq){
		પાછા MOSQ_ERR_INVAL;
	}
	જો(mosq->sock == INVALID_SOCKET){
		પાછા MOSQ_ERR_NO_CONN;
	}

	state = mosquitto__get_state(mosq);
	જો(state == mosq_cs_connect_pending){
		પાછા MOSQ_ERR_SUCCESS;
	}

	hlen = strlen((અક્ષર *)mosq->in_packet.packet_buffer);
	read_length = net__read(mosq, &mosq->in_packet.packet_buffer[hlen], mosq->in_packet.packet_buffer_size-hlen);
	જો(read_length <= 0){
		જો(read_length == 0){
			પાછા MOSQ_ERR_CONN_LOST; /* EOF */
		}
#ifdef WIN32
		errno = WSAGetLastError();
#endif
		જો(errno == EAGAIN || errno == COMPAT_EWOULDBLOCK){
			પાછા MOSQ_ERR_SUCCESS;
		}નહીંતર{
			પસંદગી(errno){
				સ્થિતિ COMPAT_ECONNRESET:
					પાછા MOSQ_ERR_CONN_LOST;
				સ્થિતિ COMPAT_EINTR:
					પાછા MOSQ_ERR_SUCCESS;
				મૂળભૂત:
					પાછા MOSQ_ERR_ERRNO;
			}
		}
	}

	mosq->in_packet.packet_buffer[mosq->in_packet.packet_buffer_size-1] = '\0'; /* Always 0 terminate */
	header_length = phr_parse_request((અક્ષર *)mosq->in_packet.packet_buffer, strlen((અક્ષર *)mosq->in_packet.packet_buffer),
			&http_method, &http_method_len,
			&http_path, &http_path_len,
			&http_minor_version,
			http_headers, &http_header_ગણતરી,
			0);
	// FIXME - deal with partial read !
	જો(header_length == -2){
		// Partial read
		પાછા MOSQ_ERR_SUCCESS;
	}નહીંતર જો(header_length == -1){
		// Error
		પાછા MOSQ_ERR_UNKNOWN;
	}નહીંતર જો(header_length < read_length){
		/* Excess data which can't be handled because the client doesn't have a key yet */
		પાછા MOSQ_ERR_MALFORMED_PACKET;
	}

	જો(strncmp(http_method, "GET", http_method_len) && strncmp(http_method, "HEAD", http_method_len)){
		/* FIXME Not supported - send 501 response */
		પાછા MOSQ_ERR_UNKNOWN;
	}

	header_have_upgrade = false;
	header_have_connection = false;
	subprotocol = ખાલી;

	માટે(size_t i=0; i<http_header_ગણતરી; i++){
		જો(!strncasecmp(http_headers[i].name, "Upgrade", http_headers[i].name_len)){
			જો(!strncasecmp(http_headers[i].કિંમત, "websocket", http_headers[i].કિંમત_len)){
				header_have_upgrade = true;
			}
		}નહીંતર જો(!strncasecmp(http_headers[i].name, "Connection", http_headers[i].name_len)){
			/* Check for "upgrade" */
			અચલ અક્ષર *str = http_headers[i].કિંમત;
			size_t start = 0;
			size_t j;
			માટે(j=0; j<http_headers[i].કિંમત_len; j++){
				જો(str[j] == ','){
					જો(!strncasecmp(&str[start], "upgrade", http_headers[i].કિંમત_len-j)){
						header_have_connection = true;
						તોડો;
					}નહીંતર{
						start = j+1;
					}
				}નહીંતર જો(str[j] == ' '){
					start = j+1;
				}
			}
			જો(!strncasecmp(&str[start], "upgrade", http_headers[i].કિંમત_len-j)){
				header_have_connection = true;
			}
		}નહીંતર જો(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Key", http_headers[i].name_len)){
			client_ચાવી = http_headers[i].કિંમત;
			client_ચાવી_len = http_headers[i].કિંમત_len;
		}નહીંતર જો(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Version", http_headers[i].name_len)){
			/* Check for "13" */
			જો(http_headers[i].કિંમત_len != 2
					|| http_headers[i].કિંમત[0] != '1'
					|| http_headers[i].કિંમત[1] != '3'
					){

				/* FIXME - not supported */
				પાછા MOSQ_ERR_NOT_SUPPORTED;
			}
		}નહીંતર જો(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Protocol", http_headers[i].name_len)){
			/* Check for "mqtt" */
			જો(!strncmp(http_headers[i].કિંમત, "mqtt", http_headers[i].કિંમત_len)
					|| !strncmp(http_headers[i].કિંમત, "mqttv3.1", http_headers[i].કિંમત_len)){

				subprotocol = http_headers[i].કિંમત;
				subprotocol_len = (પૂર્ણાંક)http_headers[i].કિંમત_len;
			}
		}નહીંતર જો(!strncasecmp(http_headers[i].name, "X-Forwarded-For", http_headers[i].name_len)){
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
		}નહીંતર જો(!strncasecmp(http_headers[i].name, "Origin", http_headers[i].name_len)){
			જો(mosq->listener){
				bool have_match = false;
				માટે(પૂર્ણાંક j=0; j<mosq->listener->ws_origin_ગણતરી; j++){
					જો(!strncmp(mosq->listener->ws_origins[j], http_headers[i].કિંમત, http_headers[i].કિંમત_len)){
						have_match = true;
						તોડો;
					}
				}
				જો(!have_match && mosq->listener->ws_origin_ગણતરી > 0){
					પાછા MOSQ_ERR_HTTP_BAD_ORIGIN;
				}
			}
		}નહીંતર{
			/* Unknown header */
		}
	}

	જો(subprotocol == ખાલી){
		// FIXME ?
		પાછા MOSQ_ERR_UNKNOWN;
	}

	જો(header_have_upgrade == false || header_have_connection == false
			|| client_ચાવી == ખાલી || client_ચાવી_len == 0){

		// FIXME - 404
		પાછા MOSQ_ERR_UNKNOWN;
	}

	જો(ws_create_accept_ચાવી(client_ચાવી, client_ચાવી_len, &accept_ચાવી)){
		પાછા MOSQ_ERR_UNKNOWN;
	}

	packet = mosquitto_calloc(1, કદ(સંરચના mosquitto__packet) + 1024 + WS_PACKET_OFFSET);
	જો(!packet){
		SAFE_FREE(accept_ચાવી);
		પાછા MOSQ_ERR_NOMEM;
	}
	packet->packet_length = (uint32_t )snprintf((અક્ષર *)&packet->payload[WS_PACKET_OFFSET], 1024,
			"HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: WebSocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: %s\r\n"
			"Sec-WebSocket-Protocol: %.*s\r\n"
			"\r\n", accept_ચાવી, subprotocol_len, subprotocol) + WS_PACKET_OFFSET;
	SAFE_FREE(accept_ચાવી);
	packet->to_process = packet->packet_length;

	memset(mosq->in_packet.packet_buffer, 0, db.config->packet_buffer_size);
	rc = packet__queue(mosq, packet);
	http__context_cleanup(mosq);
	ws__context_init(mosq);
	પાછા rc;
}
#endif
