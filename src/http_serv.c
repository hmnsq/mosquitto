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

#شامل "config.h"

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN

#شامل <assert.h>
#شامل <errno.h>
#شامل <string.h>

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "net_mosq.h"
#شامل "packet_mosq.h"
#شامل "read_handle.h"
#شامل "sys_tree.h"
#شامل "util_mosq.h"
#شامل "picohttpparser.h"


صحیح http__context_init(ساختار mosquitto *context)
{
	context->transport = mosq_t_http;

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح http__context_cleanup(ساختار mosquitto *context)
{
	UNUSED(context);
	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح http__read(ساختار mosquitto *mosq)
{
	ssize_t read_length;
	ssize_t header_length;
	شمارش mosquitto_client_state state;
	size_t hlen;
	ثابت نویسه *http_method, *http_path;
	size_t http_method_len, http_path_len;
	صحیح http_minor_version;
	size_t http_header_شمار = 100;
	ساختار phr_header http_headers[100];
	ثابت نویسه *client_کلید = پوچ;
	size_t client_کلید_len = 0;
	نویسه *accept_کلید;
	bool header_have_upgrade;
	bool header_have_connection;
	ساختار mosquitto__packet *packet;
	صحیح rc;
	ثابت نویسه *subprotocol = پوچ;
	صحیح subprotocol_len = 0;

	اگر(!mosq){
		بازگشت MOSQ_ERR_INVAL;
	}
	اگر(mosq->sock == INVALID_SOCKET){
		بازگشت MOSQ_ERR_NO_CONN;
	}

	state = mosquitto__get_state(mosq);
	اگر(state == mosq_cs_connect_pending){
		بازگشت MOSQ_ERR_SUCCESS;
	}

	hlen = strlen((نویسه *)mosq->in_packet.packet_buffer);
	read_length = net__read(mosq, &mosq->in_packet.packet_buffer[hlen], mosq->in_packet.packet_buffer_size-hlen);
	اگر(read_length <= 0){
		اگر(read_length == 0){
			بازگشت MOSQ_ERR_CONN_LOST; /* EOF */
		}
#ifdef WIN32
		errno = WSAGetLastError();
#endif
		اگر(errno == EAGAIN || errno == COMPAT_EWOULDBLOCK){
			بازگشت MOSQ_ERR_SUCCESS;
		}وگرنه{
			انتخاب(errno){
				حالت COMPAT_ECONNRESET:
					بازگشت MOSQ_ERR_CONN_LOST;
				حالت COMPAT_EINTR:
					بازگشت MOSQ_ERR_SUCCESS;
				پیشفرض:
					بازگشت MOSQ_ERR_ERRNO;
			}
		}
	}

	mosq->in_packet.packet_buffer[mosq->in_packet.packet_buffer_size-1] = '\0'; /* Always 0 terminate */
	header_length = phr_parse_request((نویسه *)mosq->in_packet.packet_buffer, strlen((نویسه *)mosq->in_packet.packet_buffer),
			&http_method, &http_method_len,
			&http_path, &http_path_len,
			&http_minor_version,
			http_headers, &http_header_شمار,
			0);
	// FIXME - deal with partial read !
	اگر(header_length == -2){
		// Partial read
		بازگشت MOSQ_ERR_SUCCESS;
	}وگرنه اگر(header_length == -1){
		// Error
		بازگشت MOSQ_ERR_UNKNOWN;
	}وگرنه اگر(header_length < read_length){
		/* Excess data which can't be handled because the client doesn't have a key yet */
		بازگشت MOSQ_ERR_MALFORMED_PACKET;
	}

	اگر(strncmp(http_method, "GET", http_method_len) && strncmp(http_method, "HEAD", http_method_len)){
		/* FIXME Not supported - send 501 response */
		بازگشت MOSQ_ERR_UNKNOWN;
	}

	header_have_upgrade = false;
	header_have_connection = false;
	subprotocol = پوچ;

	برای(size_t i=0; i<http_header_شمار; i++){
		اگر(!strncasecmp(http_headers[i].name, "Upgrade", http_headers[i].name_len)){
			اگر(!strncasecmp(http_headers[i].مقدار, "websocket", http_headers[i].مقدار_len)){
				header_have_upgrade = true;
			}
		}وگرنه اگر(!strncasecmp(http_headers[i].name, "Connection", http_headers[i].name_len)){
			/* Check for "upgrade" */
			ثابت نویسه *str = http_headers[i].مقدار;
			size_t start = 0;
			size_t j;
			برای(j=0; j<http_headers[i].مقدار_len; j++){
				اگر(str[j] == ','){
					اگر(!strncasecmp(&str[start], "upgrade", http_headers[i].مقدار_len-j)){
						header_have_connection = true;
						بشکن;
					}وگرنه{
						start = j+1;
					}
				}وگرنه اگر(str[j] == ' '){
					start = j+1;
				}
			}
			اگر(!strncasecmp(&str[start], "upgrade", http_headers[i].مقدار_len-j)){
				header_have_connection = true;
			}
		}وگرنه اگر(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Key", http_headers[i].name_len)){
			client_کلید = http_headers[i].مقدار;
			client_کلید_len = http_headers[i].مقدار_len;
		}وگرنه اگر(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Version", http_headers[i].name_len)){
			/* Check for "13" */
			اگر(http_headers[i].مقدار_len != 2
					|| http_headers[i].مقدار[0] != '1'
					|| http_headers[i].مقدار[1] != '3'
					){

				/* FIXME - not supported */
				بازگشت MOSQ_ERR_NOT_SUPPORTED;
			}
		}وگرنه اگر(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Protocol", http_headers[i].name_len)){
			/* Check for "mqtt" */
			اگر(!strncmp(http_headers[i].مقدار, "mqtt", http_headers[i].مقدار_len)
					|| !strncmp(http_headers[i].مقدار, "mqttv3.1", http_headers[i].مقدار_len)){

				subprotocol = http_headers[i].مقدار;
				subprotocol_len = (صحیح)http_headers[i].مقدار_len;
			}
		}وگرنه اگر(!strncasecmp(http_headers[i].name, "X-Forwarded-For", http_headers[i].name_len)){
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
		}وگرنه اگر(!strncasecmp(http_headers[i].name, "Origin", http_headers[i].name_len)){
			اگر(mosq->listener){
				bool have_match = false;
				برای(صحیح j=0; j<mosq->listener->ws_origin_شمار; j++){
					اگر(!strncmp(mosq->listener->ws_origins[j], http_headers[i].مقدار, http_headers[i].مقدار_len)){
						have_match = true;
						بشکن;
					}
				}
				اگر(!have_match && mosq->listener->ws_origin_شمار > 0){
					بازگشت MOSQ_ERR_HTTP_BAD_ORIGIN;
				}
			}
		}وگرنه{
			/* Unknown header */
		}
	}

	اگر(subprotocol == پوچ){
		// FIXME ?
		بازگشت MOSQ_ERR_UNKNOWN;
	}

	اگر(header_have_upgrade == false || header_have_connection == false
			|| client_کلید == پوچ || client_کلید_len == 0){

		// FIXME - 404
		بازگشت MOSQ_ERR_UNKNOWN;
	}

	اگر(ws_create_accept_کلید(client_کلید, client_کلید_len, &accept_کلید)){
		بازگشت MOSQ_ERR_UNKNOWN;
	}

	packet = mosquitto_calloc(1, اندازه(ساختار mosquitto__packet) + 1024 + WS_PACKET_OFFSET);
	اگر(!packet){
		SAFE_FREE(accept_کلید);
		بازگشت MOSQ_ERR_NOMEM;
	}
	packet->packet_length = (uint32_t )snprintf((نویسه *)&packet->payload[WS_PACKET_OFFSET], 1024,
			"HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: WebSocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: %s\r\n"
			"Sec-WebSocket-Protocol: %.*s\r\n"
			"\r\n", accept_کلید, subprotocol_len, subprotocol) + WS_PACKET_OFFSET;
	SAFE_FREE(accept_کلید);
	packet->to_process = packet->packet_length;

	memset(mosq->in_packet.packet_buffer, 0, db.config->packet_buffer_size);
	rc = packet__queue(mosq, packet);
	http__context_cleanup(mosq);
	ws__context_init(mosq);
	بازگشت rc;
}
#endif
