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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN

#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <errno.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"
#ଅନ୍ତର୍ଭାଵ "net_mosq.h"
#ଅନ୍ତର୍ଭାଵ "packet_mosq.h"
#ଅନ୍ତର୍ଭାଵ "read_handle.h"
#ଅନ୍ତର୍ଭାଵ "sys_tree.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"
#ଅନ୍ତର୍ଭାଵ "picohttpparser.h"


ପୂର୍ଣ୍ଣସଂଖ୍ୟା http__context_init(ସଂରଚନା mosquitto *context)
{
	context->transport = mosq_t_http;

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା http__context_cleanup(ସଂରଚନା mosquitto *context)
{
	UNUSED(context);
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା http__read(ସଂରଚନା mosquitto *mosq)
{
	ssize_t read_length;
	ssize_t header_length;
	ଗଣନା mosquitto_client_state state;
	size_t hlen;
	ଅଚଳ ଅକ୍ଷର *http_method, *http_path;
	size_t http_method_len, http_path_len;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା http_minor_version;
	size_t http_header_ଗଣନା = 100;
	ସଂରଚନା phr_header http_headers[100];
	ଅଚଳ ଅକ୍ଷର *client_ଚାବି = ଖାଲି;
	size_t client_ଚାବି_len = 0;
	ଅକ୍ଷର *accept_ଚାବି;
	bool header_have_upgrade;
	bool header_have_connection;
	ସଂରଚନା mosquitto__packet *packet;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ଅଚଳ ଅକ୍ଷର *subprotocol = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା subprotocol_len = 0;

	ଯଦି(!mosq){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}
	ଯଦି(mosq->sock == INVALID_SOCKET){
		ଫେରନ୍ତୁ MOSQ_ERR_NO_CONN;
	}

	state = mosquitto__get_state(mosq);
	ଯଦି(state == mosq_cs_connect_pending){
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

	hlen = strlen((ଅକ୍ଷର *)mosq->in_packet.packet_buffer);
	read_length = net__read(mosq, &mosq->in_packet.packet_buffer[hlen], mosq->in_packet.packet_buffer_size-hlen);
	ଯଦି(read_length <= 0){
		ଯଦି(read_length == 0){
			ଫେରନ୍ତୁ MOSQ_ERR_CONN_LOST; /* EOF */
		}
#ifdef WIN32
		errno = WSAGetLastError();
#endif
		ଯଦି(errno == EAGAIN || errno == COMPAT_EWOULDBLOCK){
			ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
		}ଅନ୍ୟଥା{
			ବାଛ(errno){
				ସ୍ଥିତି COMPAT_ECONNRESET:
					ଫେରନ୍ତୁ MOSQ_ERR_CONN_LOST;
				ସ୍ଥିତି COMPAT_EINTR:
					ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
				ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
					ଫେରନ୍ତୁ MOSQ_ERR_ERRNO;
			}
		}
	}

	mosq->in_packet.packet_buffer[mosq->in_packet.packet_buffer_size-1] = '\0'; /* Always 0 terminate */
	header_length = phr_parse_request((ଅକ୍ଷର *)mosq->in_packet.packet_buffer, strlen((ଅକ୍ଷର *)mosq->in_packet.packet_buffer),
			&http_method, &http_method_len,
			&http_path, &http_path_len,
			&http_minor_version,
			http_headers, &http_header_ଗଣନା,
			0);
	// FIXME - deal with partial read !
	ଯଦି(header_length == -2){
		// Partial read
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}ଅନ୍ୟଥା ଯଦି(header_length == -1){
		// Error
		ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
	}ଅନ୍ୟଥା ଯଦି(header_length < read_length){
		/* Excess data which can't be handled because the client doesn't have a key yet */
		ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
	}

	ଯଦି(strncmp(http_method, "GET", http_method_len) && strncmp(http_method, "HEAD", http_method_len)){
		/* FIXME Not supported - send 501 response */
		ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
	}

	header_have_upgrade = false;
	header_have_connection = false;
	subprotocol = ଖାଲି;

	ପାଇଁ(size_t i=0; i<http_header_ଗଣନା; i++){
		ଯଦି(!strncasecmp(http_headers[i].name, "Upgrade", http_headers[i].name_len)){
			ଯଦି(!strncasecmp(http_headers[i].ମୂଲ୍ୟ, "websocket", http_headers[i].ମୂଲ୍ୟ_len)){
				header_have_upgrade = true;
			}
		}ଅନ୍ୟଥା ଯଦି(!strncasecmp(http_headers[i].name, "Connection", http_headers[i].name_len)){
			/* Check for "upgrade" */
			ଅଚଳ ଅକ୍ଷର *str = http_headers[i].ମୂଲ୍ୟ;
			size_t start = 0;
			size_t j;
			ପାଇଁ(j=0; j<http_headers[i].ମୂଲ୍ୟ_len; j++){
				ଯଦି(str[j] == ','){
					ଯଦି(!strncasecmp(&str[start], "upgrade", http_headers[i].ମୂଲ୍ୟ_len-j)){
						header_have_connection = true;
						ଭାଙ୍ଗ;
					}ଅନ୍ୟଥା{
						start = j+1;
					}
				}ଅନ୍ୟଥା ଯଦି(str[j] == ' '){
					start = j+1;
				}
			}
			ଯଦି(!strncasecmp(&str[start], "upgrade", http_headers[i].ମୂଲ୍ୟ_len-j)){
				header_have_connection = true;
			}
		}ଅନ୍ୟଥା ଯଦି(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Key", http_headers[i].name_len)){
			client_ଚାବି = http_headers[i].ମୂଲ୍ୟ;
			client_ଚାବି_len = http_headers[i].ମୂଲ୍ୟ_len;
		}ଅନ୍ୟଥା ଯଦି(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Version", http_headers[i].name_len)){
			/* Check for "13" */
			ଯଦି(http_headers[i].ମୂଲ୍ୟ_len != 2
					|| http_headers[i].ମୂଲ୍ୟ[0] != '1'
					|| http_headers[i].ମୂଲ୍ୟ[1] != '3'
					){

				/* FIXME - not supported */
				ଫେରନ୍ତୁ MOSQ_ERR_NOT_SUPPORTED;
			}
		}ଅନ୍ୟଥା ଯଦି(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Protocol", http_headers[i].name_len)){
			/* Check for "mqtt" */
			ଯଦି(!strncmp(http_headers[i].ମୂଲ୍ୟ, "mqtt", http_headers[i].ମୂଲ୍ୟ_len)
					|| !strncmp(http_headers[i].ମୂଲ୍ୟ, "mqttv3.1", http_headers[i].ମୂଲ୍ୟ_len)){

				subprotocol = http_headers[i].ମୂଲ୍ୟ;
				subprotocol_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)http_headers[i].ମୂଲ୍ୟ_len;
			}
		}ଅନ୍ୟଥା ଯଦି(!strncasecmp(http_headers[i].name, "X-Forwarded-For", http_headers[i].name_len)){
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
		}ଅନ୍ୟଥା ଯଦି(!strncasecmp(http_headers[i].name, "Origin", http_headers[i].name_len)){
			ଯଦି(mosq->listener){
				bool have_match = false;
				ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା j=0; j<mosq->listener->ws_origin_ଗଣନା; j++){
					ଯଦି(!strncmp(mosq->listener->ws_origins[j], http_headers[i].ମୂଲ୍ୟ, http_headers[i].ମୂଲ୍ୟ_len)){
						have_match = true;
						ଭାଙ୍ଗ;
					}
				}
				ଯଦି(!have_match && mosq->listener->ws_origin_ଗଣନା > 0){
					ଫେରନ୍ତୁ MOSQ_ERR_HTTP_BAD_ORIGIN;
				}
			}
		}ଅନ୍ୟଥା{
			/* Unknown header */
		}
	}

	ଯଦି(subprotocol == ଖାଲି){
		// FIXME ?
		ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
	}

	ଯଦି(header_have_upgrade == false || header_have_connection == false
			|| client_ଚାବି == ଖାଲି || client_ଚାବି_len == 0){

		// FIXME - 404
		ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
	}

	ଯଦି(ws_create_accept_ଚାବି(client_ଚାବି, client_ଚାବି_len, &accept_ଚାବି)){
		ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
	}

	packet = mosquitto_calloc(1, ଆକାର(ସଂରଚନା mosquitto__packet) + 1024 + WS_PACKET_OFFSET);
	ଯଦି(!packet){
		SAFE_FREE(accept_ଚାବି);
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}
	packet->packet_length = (uint32_t )snprintf((ଅକ୍ଷର *)&packet->payload[WS_PACKET_OFFSET], 1024,
			"HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: WebSocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: %s\r\n"
			"Sec-WebSocket-Protocol: %.*s\r\n"
			"\r\n", accept_ଚାବି, subprotocol_len, subprotocol) + WS_PACKET_OFFSET;
	SAFE_FREE(accept_ଚାବି);
	packet->to_process = packet->packet_length;

	memset(mosq->in_packet.packet_buffer, 0, db.config->packet_buffer_size);
	rc = packet__queue(mosq, packet);
	http__context_cleanup(mosq);
	ws__context_init(mosq);
	ଫେରନ୍ତୁ rc;
}
#endif
