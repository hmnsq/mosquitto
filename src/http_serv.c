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
	اڻ_استعمال_ٿيل(context);
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح http__read(بنية mosquitto *mosq)
{
	ssize_t read_ڊيگهائي;
	ssize_t سرو_ڊيگهائي;
	تعداد mosquitto_client_state state;
	size_t hlen;
	ثابت حرف *http_طريقو, *http_رستو;
	size_t http_طريقو_len, http_رستو_len;
	صحيح http_minor_نسخو;
	size_t http_سرو_ڳڻپ = 100;
	بنية phr_سرو http_سرا[100];
	ثابت حرف *client_ڪنجي = عدم;
	size_t client_ڪنجي_len = 0;
	حرف *accept_ڪنجي;
	bool سرو_have_upgrade;
	bool سرو_have_connection;
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

	hlen = لڙي_ڊيگهه((حرف *)mosq->اندريون_packet.packet_بفر);
	read_ڊيگهائي = net__read(mosq, &mosq->اندريون_packet.packet_بفر[hlen], mosq->اندريون_packet.packet_بفر_سائيز-hlen);
	إذا(read_ڊيگهائي <= 0){
		إذا(read_ڊيگهائي == 0){
			أرجع MOSQ_ERR_CONN_گم; /* EOF */
		}
#جيڪڏهن_وصف_ٿيل WIN32
		errno = WSAGet_آخري_Error();
#جيڪڏهن_پورو
		إذا(errno == EAGAIN || errno == COMPAT_EWOULDBLOCK){
			أرجع MOSQ_ERR_SUCCESS;
		}وإلا{
			بدل(errno){
				حالة COMPAT_ECONNRESET:
					أرجع MOSQ_ERR_CONN_گم;
				حالة COMPAT_EINTR:
					أرجع MOSQ_ERR_SUCCESS;
				افتراضي:
					أرجع MOSQ_ERR_ERRNO;
			}
		}
	}

	mosq->اندريون_packet.packet_بفر[mosq->اندريون_packet.packet_بفر_سائيز-1] = '\0'; /* Always 0 terminate */
	سرو_ڊيگهائي = phr_parse_request((حرف *)mosq->اندريون_packet.packet_بفر, لڙي_ڊيگهه((حرف *)mosq->اندريون_packet.packet_بفر),
			&http_طريقو, &http_طريقو_len,
			&http_رستو, &http_رستو_len,
			&http_minor_نسخو,
			http_سرا, &http_سرو_ڳڻپ,
			0);
	// FIXME - deal with partial read !
	إذا(سرو_ڊيگهائي == -2){
		// Partial read
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا إذا(سرو_ڊيگهائي == -1){
		// Error
		أرجع MOSQ_ERR_UNKNOWN;
	}وإلا إذا(سرو_ڊيگهائي < read_ڊيگهائي){
		/* Excess data which can't be handled because the client doesn't have a key yet */
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}

	إذا(لڙي_ڀيٽ(http_طريقو, "GET", http_طريقو_len) && لڙي_ڀيٽ(http_طريقو, "HEAD", http_طريقو_len)){
		/* FIXME Not supported - send 501 response */
		أرجع MOSQ_ERR_UNKNOWN;
	}

	سرو_have_upgrade = false;
	سرو_have_connection = false;
	subprotocol = عدم;

	لكل(size_t i=0; i<http_سرو_ڳڻپ; i++){
		إذا(!strncasecmp(http_سرا[i].name, "Upgrade", http_سرا[i].name_len)){
			إذا(!strncasecmp(http_سرا[i].قدر, "websocket", http_سرا[i].قدر_len)){
				سرو_have_upgrade = true;
			}
		}وإلا إذا(!strncasecmp(http_سرا[i].name, "Connection", http_سرا[i].name_len)){
			/* Check for "upgrade" */
			ثابت حرف *str = http_سرا[i].قدر;
			size_t شروعات = 0;
			size_t j;
			لكل(j=0; j<http_سرا[i].قدر_len; j++){
				إذا(str[j] == ','){
					إذا(!strncasecmp(&str[شروعات], "upgrade", http_سرا[i].قدر_len-j)){
						سرو_have_connection = true;
						اكسر;
					}وإلا{
						شروعات = j+1;
					}
				}وإلا إذا(str[j] == ' '){
					شروعات = j+1;
				}
			}
			إذا(!strncasecmp(&str[شروعات], "upgrade", http_سرا[i].قدر_len-j)){
				سرو_have_connection = true;
			}
		}وإلا إذا(!strncasecmp(http_سرا[i].name, "Sec-WebSocket-Key", http_سرا[i].name_len)){
			client_ڪنجي = http_سرا[i].قدر;
			client_ڪنجي_len = http_سرا[i].قدر_len;
		}وإلا إذا(!strncasecmp(http_سرا[i].name, "Sec-WebSocket-Version", http_سرا[i].name_len)){
			/* Check for "13" */
			إذا(http_سرا[i].قدر_len != 2
					|| http_سرا[i].قدر[0] != '1'
					|| http_سرا[i].قدر[1] != '3'
					){

				/* FIXME - not supported */
				أرجع MOSQ_ERR_NOT_SUPPORTED;
			}
		}وإلا إذا(!strncasecmp(http_سرا[i].name, "Sec-WebSocket-Protocol", http_سرا[i].name_len)){
			/* Check for "mqtt" */
			إذا(!لڙي_ڀيٽ(http_سرا[i].قدر, "mqtt", http_سرا[i].قدر_len)
					|| !لڙي_ڀيٽ(http_سرا[i].قدر, "mqttv3.1", http_سرا[i].قدر_len)){

				subprotocol = http_سرا[i].قدر;
				subprotocol_len = (صحيح)http_سرا[i].قدر_len;
			}
		}وإلا إذا(!strncasecmp(http_سرا[i].name, "X-Forwarded-For", http_سرا[i].name_len)){
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
		}وإلا إذا(!strncasecmp(http_سرا[i].name, "Origin", http_سرا[i].name_len)){
			إذا(mosq->listener){
				bool have_ميلاپ = false;
				لكل(صحيح j=0; j<mosq->listener->ws_origin_ڳڻپ; j++){
					إذا(!لڙي_ڀيٽ(mosq->listener->ws_origins[j], http_سرا[i].قدر, http_سرا[i].قدر_len)){
						have_ميلاپ = true;
						اكسر;
					}
				}
				إذا(!have_ميلاپ && mosq->listener->ws_origin_ڳڻپ > 0){
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

	إذا(سرو_have_upgrade == false || سرو_have_connection == false
			|| client_ڪنجي == عدم || client_ڪنجي_len == 0){

		// FIXME - 404
		أرجع MOSQ_ERR_UNKNOWN;
	}

	إذا(ws_create_accept_ڪنجي(client_ڪنجي, client_ڪنجي_len, &accept_ڪنجي)){
		أرجع MOSQ_ERR_UNKNOWN;
	}

	packet = mosquitto_calloc(1, حجم(بنية mosquitto__packet) + 1024 + WS_PACKET_وٿي);
	إذا(!packet){
		محفوظ_FREE(accept_ڪنجي);
		أرجع MOSQ_ERR_NOMEM;
	}
	packet->packet_ڊيگهائي = (uint32_t )snprintf((حرف *)&packet->payload[WS_PACKET_وٿي], 1024,
			"HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: WebSocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: %s\r\n"
			"Sec-WebSocket-Protocol: %.*s\r\n"
			"\r\n", accept_ڪنجي, subprotocol_len, subprotocol) + WS_PACKET_وٿي;
	محفوظ_FREE(accept_ڪنجي);
	packet->منزل_process = packet->packet_ڊيگهائي;

	memset(mosq->اندريون_packet.packet_بفر, 0, db.config->packet_بفر_سائيز);
	rc = packet_قطار(mosq, packet);
	http__context_cleanup(mosq);
	ws__context_init(mosq);
	أرجع rc;
}
#جيڪڏهن_پورو
