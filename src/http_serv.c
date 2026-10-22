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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "net_mosq.h"
#ಅನ್ತರ್ಭಾವ "packet_mosq.h"
#ಅನ್ತರ್ಭಾವ "read_handle.h"
#ಅನ್ತರ್ಭಾವ "sys_tree.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"
#ಅನ್ತರ್ಭಾವ "picohttpparser.h"


ಪೂರ್ಣಾಙ್ಕ http__context_init(ಸಂರಚನಾ mosquitto *context)
{
	context->transport = mosq_t_http;

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ http__context_cleanup(ಸಂರಚನಾ mosquitto *context)
{
	UNUSED(context);
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ http__read(ಸಂರಚನಾ mosquitto *mosq)
{
	ssize_t read_length;
	ssize_t header_length;
	ಗಣನಾ mosquitto_client_state state;
	size_t hlen;
	ಧ್ರುವ ವರ್ಣ *http_method, *http_path;
	size_t http_method_len, http_path_len;
	ಪೂರ್ಣಾಙ್ಕ http_minor_version;
	size_t http_header_ಎಣಿಕೆ = 100;
	ಸಂರಚನಾ phr_header http_headers[100];
	ಧ್ರುವ ವರ್ಣ *client_ಕೀಲಿ = ರಿಕ್ತ;
	size_t client_ಕೀಲಿ_len = 0;
	ವರ್ಣ *accept_ಕೀಲಿ;
	bool header_have_upgrade;
	bool header_have_connection;
	ಸಂರಚನಾ mosquitto__packet *packet;
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಧ್ರುವ ವರ್ಣ *subprotocol = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ subprotocol_len = 0;

	ಯದಿ(!mosq){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}
	ಯದಿ(mosq->sock == INVALID_SOCKET){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NO_CONN;
	}

	state = mosquitto__get_state(mosq);
	ಯದಿ(state == mosq_cs_connect_pending){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}

	hlen = strlen((ವರ್ಣ *)mosq->in_packet.packet_buffer);
	read_length = net__read(mosq, &mosq->in_packet.packet_buffer[hlen], mosq->in_packet.packet_buffer_size-hlen);
	ಯದಿ(read_length <= 0){
		ಯದಿ(read_length == 0){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_CONN_LOST; /* EOF */
		}
#ifdef WIN32
		errno = WSAGetLastError();
#endif
		ಯದಿ(errno == EAGAIN || errno == COMPAT_EWOULDBLOCK){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
		}ಅನ್ಯಥಾ{
			ವಿಕಲ್ಪ(errno){
				ಪಕ್ಷ COMPAT_ECONNRESET:
					ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_CONN_LOST;
				ಪಕ್ಷ COMPAT_EINTR:
					ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
				ಸ್ವಭಾವ:
					ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_ERRNO;
			}
		}
	}

	mosq->in_packet.packet_buffer[mosq->in_packet.packet_buffer_size-1] = '\0'; /* Always 0 terminate */
	header_length = phr_parse_request((ವರ್ಣ *)mosq->in_packet.packet_buffer, strlen((ವರ್ಣ *)mosq->in_packet.packet_buffer),
			&http_method, &http_method_len,
			&http_path, &http_path_len,
			&http_minor_version,
			http_headers, &http_header_ಎಣಿಕೆ,
			0);
	// FIXME - deal with partial read !
	ಯದಿ(header_length == -2){
		// Partial read
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}ಅನ್ಯಥಾ ಯದಿ(header_length == -1){
		// Error
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
	}ಅನ್ಯಥಾ ಯದಿ(header_length < read_length){
		/* Excess data which can't be handled because the client doesn't have a key yet */
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
	}

	ಯದಿ(strncmp(http_method, "GET", http_method_len) && strncmp(http_method, "HEAD", http_method_len)){
		/* FIXME Not supported - send 501 response */
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
	}

	header_have_upgrade = false;
	header_have_connection = false;
	subprotocol = ರಿಕ್ತ;

	ಪ್ರತಿ(size_t i=0; i<http_header_ಎಣಿಕೆ; i++){
		ಯದಿ(!strncasecmp(http_headers[i].name, "Upgrade", http_headers[i].name_len)){
			ಯದಿ(!strncasecmp(http_headers[i].ಮೌಲ್ಯ, "websocket", http_headers[i].ಮೌಲ್ಯ_len)){
				header_have_upgrade = true;
			}
		}ಅನ್ಯಥಾ ಯದಿ(!strncasecmp(http_headers[i].name, "Connection", http_headers[i].name_len)){
			/* Check for "upgrade" */
			ಧ್ರುವ ವರ್ಣ *str = http_headers[i].ಮೌಲ್ಯ;
			size_t start = 0;
			size_t j;
			ಪ್ರತಿ(j=0; j<http_headers[i].ಮೌಲ್ಯ_len; j++){
				ಯದಿ(str[j] == ','){
					ಯದಿ(!strncasecmp(&str[start], "upgrade", http_headers[i].ಮೌಲ್ಯ_len-j)){
						header_have_connection = true;
						ಭಙ್ಗ;
					}ಅನ್ಯಥಾ{
						start = j+1;
					}
				}ಅನ್ಯಥಾ ಯದಿ(str[j] == ' '){
					start = j+1;
				}
			}
			ಯದಿ(!strncasecmp(&str[start], "upgrade", http_headers[i].ಮೌಲ್ಯ_len-j)){
				header_have_connection = true;
			}
		}ಅನ್ಯಥಾ ಯದಿ(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Key", http_headers[i].name_len)){
			client_ಕೀಲಿ = http_headers[i].ಮೌಲ್ಯ;
			client_ಕೀಲಿ_len = http_headers[i].ಮೌಲ್ಯ_len;
		}ಅನ್ಯಥಾ ಯದಿ(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Version", http_headers[i].name_len)){
			/* Check for "13" */
			ಯದಿ(http_headers[i].ಮೌಲ್ಯ_len != 2
					|| http_headers[i].ಮೌಲ್ಯ[0] != '1'
					|| http_headers[i].ಮೌಲ್ಯ[1] != '3'
					){

				/* FIXME - not supported */
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_SUPPORTED;
			}
		}ಅನ್ಯಥಾ ಯದಿ(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Protocol", http_headers[i].name_len)){
			/* Check for "mqtt" */
			ಯದಿ(!strncmp(http_headers[i].ಮೌಲ್ಯ, "mqtt", http_headers[i].ಮೌಲ್ಯ_len)
					|| !strncmp(http_headers[i].ಮೌಲ್ಯ, "mqttv3.1", http_headers[i].ಮೌಲ್ಯ_len)){

				subprotocol = http_headers[i].ಮೌಲ್ಯ;
				subprotocol_len = (ಪೂರ್ಣಾಙ್ಕ)http_headers[i].ಮೌಲ್ಯ_len;
			}
		}ಅನ್ಯಥಾ ಯದಿ(!strncasecmp(http_headers[i].name, "X-Forwarded-For", http_headers[i].name_len)){
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
		}ಅನ್ಯಥಾ ಯದಿ(!strncasecmp(http_headers[i].name, "Origin", http_headers[i].name_len)){
			ಯದಿ(mosq->listener){
				bool have_match = false;
				ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ j=0; j<mosq->listener->ws_origin_ಎಣಿಕೆ; j++){
					ಯದಿ(!strncmp(mosq->listener->ws_origins[j], http_headers[i].ಮೌಲ್ಯ, http_headers[i].ಮೌಲ್ಯ_len)){
						have_match = true;
						ಭಙ್ಗ;
					}
				}
				ಯದಿ(!have_match && mosq->listener->ws_origin_ಎಣಿಕೆ > 0){
					ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_HTTP_BAD_ORIGIN;
				}
			}
		}ಅನ್ಯಥಾ{
			/* Unknown header */
		}
	}

	ಯದಿ(subprotocol == ರಿಕ್ತ){
		// FIXME ?
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
	}

	ಯದಿ(header_have_upgrade == false || header_have_connection == false
			|| client_ಕೀಲಿ == ರಿಕ್ತ || client_ಕೀಲಿ_len == 0){

		// FIXME - 404
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
	}

	ಯದಿ(ws_create_accept_ಕೀಲಿ(client_ಕೀಲಿ, client_ಕೀಲಿ_len, &accept_ಕೀಲಿ)){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
	}

	packet = mosquitto_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__packet) + 1024 + WS_PACKET_OFFSET);
	ಯದಿ(!packet){
		SAFE_FREE(accept_ಕೀಲಿ);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}
	packet->packet_length = (uint32_t )snprintf((ವರ್ಣ *)&packet->payload[WS_PACKET_OFFSET], 1024,
			"HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: WebSocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: %s\r\n"
			"Sec-WebSocket-Protocol: %.*s\r\n"
			"\r\n", accept_ಕೀಲಿ, subprotocol_len, subprotocol) + WS_PACKET_OFFSET;
	SAFE_FREE(accept_ಕೀಲಿ);
	packet->to_process = packet->packet_length;

	memset(mosq->in_packet.packet_buffer, 0, db.config->packet_buffer_size);
	rc = packet__queue(mosq, packet);
	http__context_cleanup(mosq);
	ws__context_init(mosq);
	ಪ್ರತ್ಯಾಗಮ rc;
}
#endif
