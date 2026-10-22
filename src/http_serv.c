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

#അന്തര്ഭാവ "config.h"

#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN

#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <errno.h>
#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/mqtt_protocol.h"
#അന്തര്ഭാവ "net_mosq.h"
#അന്തര്ഭാവ "packet_mosq.h"
#അന്തര്ഭാവ "read_handle.h"
#അന്തര്ഭാവ "sys_tree.h"
#അന്തര്ഭാവ "util_mosq.h"
#അന്തര്ഭാവ "picohttpparser.h"


പൂർണ്ണസംഖ്യ http__context_init(ഘടന mosquitto *context)
{
	context->transport = mosq_t_http;

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ http__context_cleanup(ഘടന mosquitto *context)
{
	UNUSED(context);
	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ http__read(ഘടന mosquitto *mosq)
{
	ssize_t read_length;
	ssize_t header_length;
	എണ്ണൽ mosquitto_client_state state;
	size_t hlen;
	സ്ഥിരം അക്ഷരം *http_method, *http_path;
	size_t http_method_len, http_path_len;
	പൂർണ്ണസംഖ്യ http_minor_version;
	size_t http_header_എണ്ണം = 100;
	ഘടന phr_header http_headers[100];
	സ്ഥിരം അക്ഷരം *client_താക്കോൽ = ശൂന്യ;
	size_t client_താക്കോൽ_len = 0;
	അക്ഷരം *accept_താക്കോൽ;
	bool header_have_upgrade;
	bool header_have_connection;
	ഘടന mosquitto__packet *packet;
	പൂർണ്ണസംഖ്യ rc;
	സ്ഥിരം അക്ഷരം *subprotocol = ശൂന്യ;
	പൂർണ്ണസംഖ്യ subprotocol_len = 0;

	എങ്കിൽ(!mosq){
		തിരികെ MOSQ_ERR_INVAL;
	}
	എങ്കിൽ(mosq->sock == INVALID_SOCKET){
		തിരികെ MOSQ_ERR_NO_CONN;
	}

	state = mosquitto__get_state(mosq);
	എങ്കിൽ(state == mosq_cs_connect_pending){
		തിരികെ MOSQ_ERR_SUCCESS;
	}

	hlen = strlen((അക്ഷരം *)mosq->in_packet.packet_buffer);
	read_length = net__read(mosq, &mosq->in_packet.packet_buffer[hlen], mosq->in_packet.packet_buffer_size-hlen);
	എങ്കിൽ(read_length <= 0){
		എങ്കിൽ(read_length == 0){
			തിരികെ MOSQ_ERR_CONN_LOST; /* EOF */
		}
#ifdef WIN32
		errno = WSAGetLastError();
#endif
		എങ്കിൽ(errno == EAGAIN || errno == COMPAT_EWOULDBLOCK){
			തിരികെ MOSQ_ERR_SUCCESS;
		}അല്ലെങ്കിൽ{
			തിരഞ്ഞെടുപ്പ്(errno){
				അവസ്ഥ COMPAT_ECONNRESET:
					തിരികെ MOSQ_ERR_CONN_LOST;
				അവസ്ഥ COMPAT_EINTR:
					തിരികെ MOSQ_ERR_SUCCESS;
				സ്ഥിരസ്ഥിതി:
					തിരികെ MOSQ_ERR_ERRNO;
			}
		}
	}

	mosq->in_packet.packet_buffer[mosq->in_packet.packet_buffer_size-1] = '\0'; /* Always 0 terminate */
	header_length = phr_parse_request((അക്ഷരം *)mosq->in_packet.packet_buffer, strlen((അക്ഷരം *)mosq->in_packet.packet_buffer),
			&http_method, &http_method_len,
			&http_path, &http_path_len,
			&http_minor_version,
			http_headers, &http_header_എണ്ണം,
			0);
	// FIXME - deal with partial read !
	എങ്കിൽ(header_length == -2){
		// Partial read
		തിരികെ MOSQ_ERR_SUCCESS;
	}അല്ലെങ്കിൽ എങ്കിൽ(header_length == -1){
		// Error
		തിരികെ MOSQ_ERR_UNKNOWN;
	}അല്ലെങ്കിൽ എങ്കിൽ(header_length < read_length){
		/* Excess data which can't be handled because the client doesn't have a key yet */
		തിരികെ MOSQ_ERR_MALFORMED_PACKET;
	}

	എങ്കിൽ(strncmp(http_method, "GET", http_method_len) && strncmp(http_method, "HEAD", http_method_len)){
		/* FIXME Not supported - send 501 response */
		തിരികെ MOSQ_ERR_UNKNOWN;
	}

	header_have_upgrade = false;
	header_have_connection = false;
	subprotocol = ശൂന്യ;

	വേണ്ടി(size_t i=0; i<http_header_എണ്ണം; i++){
		എങ്കിൽ(!strncasecmp(http_headers[i].name, "Upgrade", http_headers[i].name_len)){
			എങ്കിൽ(!strncasecmp(http_headers[i].മൂല്യം, "websocket", http_headers[i].മൂല്യം_len)){
				header_have_upgrade = true;
			}
		}അല്ലെങ്കിൽ എങ്കിൽ(!strncasecmp(http_headers[i].name, "Connection", http_headers[i].name_len)){
			/* Check for "upgrade" */
			സ്ഥിരം അക്ഷരം *str = http_headers[i].മൂല്യം;
			size_t start = 0;
			size_t j;
			വേണ്ടി(j=0; j<http_headers[i].മൂല്യം_len; j++){
				എങ്കിൽ(str[j] == ','){
					എങ്കിൽ(!strncasecmp(&str[start], "upgrade", http_headers[i].മൂല്യം_len-j)){
						header_have_connection = true;
						നിർത്തുക;
					}അല്ലെങ്കിൽ{
						start = j+1;
					}
				}അല്ലെങ്കിൽ എങ്കിൽ(str[j] == ' '){
					start = j+1;
				}
			}
			എങ്കിൽ(!strncasecmp(&str[start], "upgrade", http_headers[i].മൂല്യം_len-j)){
				header_have_connection = true;
			}
		}അല്ലെങ്കിൽ എങ്കിൽ(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Key", http_headers[i].name_len)){
			client_താക്കോൽ = http_headers[i].മൂല്യം;
			client_താക്കോൽ_len = http_headers[i].മൂല്യം_len;
		}അല്ലെങ്കിൽ എങ്കിൽ(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Version", http_headers[i].name_len)){
			/* Check for "13" */
			എങ്കിൽ(http_headers[i].മൂല്യം_len != 2
					|| http_headers[i].മൂല്യം[0] != '1'
					|| http_headers[i].മൂല്യം[1] != '3'
					){

				/* FIXME - not supported */
				തിരികെ MOSQ_ERR_NOT_SUPPORTED;
			}
		}അല്ലെങ്കിൽ എങ്കിൽ(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Protocol", http_headers[i].name_len)){
			/* Check for "mqtt" */
			എങ്കിൽ(!strncmp(http_headers[i].മൂല്യം, "mqtt", http_headers[i].മൂല്യം_len)
					|| !strncmp(http_headers[i].മൂല്യം, "mqttv3.1", http_headers[i].മൂല്യം_len)){

				subprotocol = http_headers[i].മൂല്യം;
				subprotocol_len = (പൂർണ്ണസംഖ്യ)http_headers[i].മൂല്യം_len;
			}
		}അല്ലെങ്കിൽ എങ്കിൽ(!strncasecmp(http_headers[i].name, "X-Forwarded-For", http_headers[i].name_len)){
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
		}അല്ലെങ്കിൽ എങ്കിൽ(!strncasecmp(http_headers[i].name, "Origin", http_headers[i].name_len)){
			എങ്കിൽ(mosq->listener){
				bool have_match = false;
				വേണ്ടി(പൂർണ്ണസംഖ്യ j=0; j<mosq->listener->ws_origin_എണ്ണം; j++){
					എങ്കിൽ(!strncmp(mosq->listener->ws_origins[j], http_headers[i].മൂല്യം, http_headers[i].മൂല്യം_len)){
						have_match = true;
						നിർത്തുക;
					}
				}
				എങ്കിൽ(!have_match && mosq->listener->ws_origin_എണ്ണം > 0){
					തിരികെ MOSQ_ERR_HTTP_BAD_ORIGIN;
				}
			}
		}അല്ലെങ്കിൽ{
			/* Unknown header */
		}
	}

	എങ്കിൽ(subprotocol == ശൂന്യ){
		// FIXME ?
		തിരികെ MOSQ_ERR_UNKNOWN;
	}

	എങ്കിൽ(header_have_upgrade == false || header_have_connection == false
			|| client_താക്കോൽ == ശൂന്യ || client_താക്കോൽ_len == 0){

		// FIXME - 404
		തിരികെ MOSQ_ERR_UNKNOWN;
	}

	എങ്കിൽ(ws_create_accept_താക്കോൽ(client_താക്കോൽ, client_താക്കോൽ_len, &accept_താക്കോൽ)){
		തിരികെ MOSQ_ERR_UNKNOWN;
	}

	packet = mosquitto_calloc(1, വലുപ്പം(ഘടന mosquitto__packet) + 1024 + WS_PACKET_OFFSET);
	എങ്കിൽ(!packet){
		SAFE_FREE(accept_താക്കോൽ);
		തിരികെ MOSQ_ERR_NOMEM;
	}
	packet->packet_length = (uint32_t )snprintf((അക്ഷരം *)&packet->payload[WS_PACKET_OFFSET], 1024,
			"HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: WebSocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: %s\r\n"
			"Sec-WebSocket-Protocol: %.*s\r\n"
			"\r\n", accept_താക്കോൽ, subprotocol_len, subprotocol) + WS_PACKET_OFFSET;
	SAFE_FREE(accept_താക്കോൽ);
	packet->to_process = packet->packet_length;

	memset(mosq->in_packet.packet_buffer, 0, db.config->packet_buffer_size);
	rc = packet__queue(mosq, packet);
	http__context_cleanup(mosq);
	ws__context_init(mosq);
	തിരികെ rc;
}
#endif
