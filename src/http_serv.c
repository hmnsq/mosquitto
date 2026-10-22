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

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN

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

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح http__context_cleanup(بنية mosquitto *context)
{
	UNUSED(context);
	أرجع MOSQ_ERR_SUCCESS;
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
	size_t http_header_سانى = 100;
	بنية phr_header http_headers[100];
	ثابت حرف *client_ئاچقۇچ = عدم;
	size_t client_ئاچقۇچ_len = 0;
	حرف *accept_ئاچقۇچ;
	bool header_have_upgrade;
	bool header_have_connection;
	بنية mosquitto__packet *packet;
	صحيح rc;
	ثابت حرف *subprotocol = عدم;
	صحيح subprotocol_len = 0;

	إذا(!mosq){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(mosq->sock == INVALID_SOCKET){
		أرجع MOSQ_ERR_NO_CONN;
	}

	state = mosquitto__get_state(mosq);
	إذا(state == mosq_cs_connect_pending){
		أرجع MOSQ_ERR_SUCCESS;
	}

	hlen = strlen((حرف *)mosq->in_packet.packet_buffer);
	read_length = net__read(mosq, &mosq->in_packet.packet_buffer[hlen], mosq->in_packet.packet_buffer_size-hlen);
	إذا(read_length <= 0){
		إذا(read_length == 0){
			أرجع MOSQ_ERR_CONN_LOST; /* EOF */
		}
#ifdef WIN32
		errno = WSAGetLastError();
#endif
		إذا(errno == EAGAIN || errno == COMPAT_EWOULDBLOCK){
			أرجع MOSQ_ERR_SUCCESS;
		}وإلا{
			بدل(errno){
				حالة COMPAT_ECONNRESET:
					أرجع MOSQ_ERR_CONN_LOST;
				حالة COMPAT_EINTR:
					أرجع MOSQ_ERR_SUCCESS;
				افتراضي:
					أرجع MOSQ_ERR_ERRNO;
			}
		}
	}

	mosq->in_packet.packet_buffer[mosq->in_packet.packet_buffer_size-1] = '\0'; /* Always 0 terminate */
	header_length = phr_parse_request((حرف *)mosq->in_packet.packet_buffer, strlen((حرف *)mosq->in_packet.packet_buffer),
			&http_method, &http_method_len,
			&http_path, &http_path_len,
			&http_minor_version,
			http_headers, &http_header_سانى,
			0);
	// FIXME - deal with partial read !
	إذا(header_length == -2){
		// Partial read
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا إذا(header_length == -1){
		// Error
		أرجع MOSQ_ERR_UNKNOWN;
	}وإلا إذا(header_length < read_length){
		/* Excess data which can't be handled because the client doesn't have a key yet */
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}

	إذا(strncmp(http_method, "GET", http_method_len) && strncmp(http_method, "HEAD", http_method_len)){
		/* FIXME Not supported - send 501 response */
		أرجع MOSQ_ERR_UNKNOWN;
	}

	header_have_upgrade = false;
	header_have_connection = false;
	subprotocol = عدم;

	لكل(size_t i=0; i<http_header_سانى; i++){
		إذا(!strncasecmp(http_headers[i].name, "Upgrade", http_headers[i].name_len)){
			إذا(!strncasecmp(http_headers[i].قىممەت, "websocket", http_headers[i].قىممەت_len)){
				header_have_upgrade = true;
			}
		}وإلا إذا(!strncasecmp(http_headers[i].name, "Connection", http_headers[i].name_len)){
			/* Check for "upgrade" */
			ثابت حرف *str = http_headers[i].قىممەت;
			size_t start = 0;
			size_t j;
			لكل(j=0; j<http_headers[i].قىممەت_len; j++){
				إذا(str[j] == ','){
					إذا(!strncasecmp(&str[start], "upgrade", http_headers[i].قىممەت_len-j)){
						header_have_connection = true;
						اكسر;
					}وإلا{
						start = j+1;
					}
				}وإلا إذا(str[j] == ' '){
					start = j+1;
				}
			}
			إذا(!strncasecmp(&str[start], "upgrade", http_headers[i].قىممەت_len-j)){
				header_have_connection = true;
			}
		}وإلا إذا(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Key", http_headers[i].name_len)){
			client_ئاچقۇچ = http_headers[i].قىممەت;
			client_ئاچقۇچ_len = http_headers[i].قىممەت_len;
		}وإلا إذا(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Version", http_headers[i].name_len)){
			/* Check for "13" */
			إذا(http_headers[i].قىممەت_len != 2
					|| http_headers[i].قىممەت[0] != '1'
					|| http_headers[i].قىممەت[1] != '3'
					){

				/* FIXME - not supported */
				أرجع MOSQ_ERR_NOT_SUPPORTED;
			}
		}وإلا إذا(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Protocol", http_headers[i].name_len)){
			/* Check for "mqtt" */
			إذا(!strncmp(http_headers[i].قىممەت, "mqtt", http_headers[i].قىممەت_len)
					|| !strncmp(http_headers[i].قىممەت, "mqttv3.1", http_headers[i].قىممەت_len)){

				subprotocol = http_headers[i].قىممەت;
				subprotocol_len = (صحيح)http_headers[i].قىممەت_len;
			}
		}وإلا إذا(!strncasecmp(http_headers[i].name, "X-Forwarded-For", http_headers[i].name_len)){
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
		}وإلا إذا(!strncasecmp(http_headers[i].name, "Origin", http_headers[i].name_len)){
			إذا(mosq->listener){
				bool have_match = false;
				لكل(صحيح j=0; j<mosq->listener->ws_origin_سانى; j++){
					إذا(!strncmp(mosq->listener->ws_origins[j], http_headers[i].قىممەت, http_headers[i].قىممەت_len)){
						have_match = true;
						اكسر;
					}
				}
				إذا(!have_match && mosq->listener->ws_origin_سانى > 0){
					أرجع MOSQ_ERR_HTTP_BAD_ORIGIN;
				}
			}
		}وإلا{
			/* Unknown header */
		}
	}

	إذا(subprotocol == عدم){
		// FIXME ?
		أرجع MOSQ_ERR_UNKNOWN;
	}

	إذا(header_have_upgrade == false || header_have_connection == false
			|| client_ئاچقۇچ == عدم || client_ئاچقۇچ_len == 0){

		// FIXME - 404
		أرجع MOSQ_ERR_UNKNOWN;
	}

	إذا(ws_create_accept_ئاچقۇچ(client_ئاچقۇچ, client_ئاچقۇچ_len, &accept_ئاچقۇچ)){
		أرجع MOSQ_ERR_UNKNOWN;
	}

	packet = mosquitto_calloc(1, حجم(بنية mosquitto__packet) + 1024 + WS_PACKET_OFFSET);
	إذا(!packet){
		SAFE_FREE(accept_ئاچقۇچ);
		أرجع MOSQ_ERR_NOMEM;
	}
	packet->packet_length = (uint32_t )snprintf((حرف *)&packet->payload[WS_PACKET_OFFSET], 1024,
			"HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: WebSocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: %s\r\n"
			"Sec-WebSocket-Protocol: %.*s\r\n"
			"\r\n", accept_ئاچقۇچ, subprotocol_len, subprotocol) + WS_PACKET_OFFSET;
	SAFE_FREE(accept_ئاچقۇچ);
	packet->to_process = packet->packet_length;

	memset(mosq->in_packet.packet_buffer, 0, db.config->packet_buffer_size);
	rc = packet__queue(mosq, packet);
	http__context_cleanup(mosq);
	ws__context_init(mosq);
	أرجع rc;
}
#endif
