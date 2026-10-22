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
	بن_ورتیا(context);
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح http__read(بنية mosquitto *mosq)
{
	ssize_t read_لمائی;
	ssize_t سرخی_لمائی;
	تعداد mosquitto_client_state state;
	size_t hlen;
	ثابت حرف *http_طریقہ, *http_رستہ;
	size_t http_طریقہ_len, http_رستہ_len;
	صحيح http_minor_نسخہ;
	size_t http_سرخی_گنتی = 100;
	بنية phr_سرخی http_سرخیاں[100];
	ثابت حرف *client_کلید = عدم;
	size_t client_کلید_len = 0;
	حرف *accept_کلید;
	bool سرخی_have_upgrade;
	bool سرخی_have_connection;
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

	hlen = لڑی_لمبائی((حرف *)mosq->وچ_packet.packet_بفر);
	read_لمائی = net__read(mosq, &mosq->وچ_packet.packet_بفر[hlen], mosq->وچ_packet.packet_بفر_ناپ-hlen);
	إذا(read_لمائی <= 0){
		إذا(read_لمائی == 0){
			أرجع MOSQ_ERR_CONN_گواچا; /* EOF */
		}
#جے_تعریف_شدہ WIN32
		errno = WSAGet_آخری_Error();
#جے_انت
		إذا(errno == EAGAIN || errno == COMPAT_EWOULDBLOCK){
			أرجع MOSQ_ERR_SUCCESS;
		}وإلا{
			بدل(errno){
				حالة COMPAT_ECONNRESET:
					أرجع MOSQ_ERR_CONN_گواچا;
				حالة COMPAT_EINTR:
					أرجع MOSQ_ERR_SUCCESS;
				افتراضي:
					أرجع MOSQ_ERR_ERRNO;
			}
		}
	}

	mosq->وچ_packet.packet_بفر[mosq->وچ_packet.packet_بفر_ناپ-1] = '\0'; /* Always 0 terminate */
	سرخی_لمائی = phr_parse_request((حرف *)mosq->وچ_packet.packet_بفر, لڑی_لمبائی((حرف *)mosq->وچ_packet.packet_بفر),
			&http_طریقہ, &http_طریقہ_len,
			&http_رستہ, &http_رستہ_len,
			&http_minor_نسخہ,
			http_سرخیاں, &http_سرخی_گنتی,
			0);
	// FIXME - deal with partial read !
	إذا(سرخی_لمائی == -2){
		// Partial read
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا إذا(سرخی_لمائی == -1){
		// Error
		أرجع MOSQ_ERR_UNKNOWN;
	}وإلا إذا(سرخی_لمائی < read_لمائی){
		/* Excess data which can't be handled because the client doesn't have a key yet */
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}

	إذا(لڑی_مقابلہ(http_طریقہ, "GET", http_طریقہ_len) && لڑی_مقابلہ(http_طریقہ, "HEAD", http_طریقہ_len)){
		/* FIXME Not supported - send 501 response */
		أرجع MOSQ_ERR_UNKNOWN;
	}

	سرخی_have_upgrade = false;
	سرخی_have_connection = false;
	subprotocol = عدم;

	لكل(size_t i=0; i<http_سرخی_گنتی; i++){
		إذا(!strncasecmp(http_سرخیاں[i].name, "Upgrade", http_سرخیاں[i].name_len)){
			إذا(!strncasecmp(http_سرخیاں[i].قدر, "websocket", http_سرخیاں[i].قدر_len)){
				سرخی_have_upgrade = true;
			}
		}وإلا إذا(!strncasecmp(http_سرخیاں[i].name, "Connection", http_سرخیاں[i].name_len)){
			/* Check for "upgrade" */
			ثابت حرف *str = http_سرخیاں[i].قدر;
			size_t شروع = 0;
			size_t j;
			لكل(j=0; j<http_سرخیاں[i].قدر_len; j++){
				إذا(str[j] == ','){
					إذا(!strncasecmp(&str[شروع], "upgrade", http_سرخیاں[i].قدر_len-j)){
						سرخی_have_connection = true;
						اكسر;
					}وإلا{
						شروع = j+1;
					}
				}وإلا إذا(str[j] == ' '){
					شروع = j+1;
				}
			}
			إذا(!strncasecmp(&str[شروع], "upgrade", http_سرخیاں[i].قدر_len-j)){
				سرخی_have_connection = true;
			}
		}وإلا إذا(!strncasecmp(http_سرخیاں[i].name, "Sec-WebSocket-Key", http_سرخیاں[i].name_len)){
			client_کلید = http_سرخیاں[i].قدر;
			client_کلید_len = http_سرخیاں[i].قدر_len;
		}وإلا إذا(!strncasecmp(http_سرخیاں[i].name, "Sec-WebSocket-Version", http_سرخیاں[i].name_len)){
			/* Check for "13" */
			إذا(http_سرخیاں[i].قدر_len != 2
					|| http_سرخیاں[i].قدر[0] != '1'
					|| http_سرخیاں[i].قدر[1] != '3'
					){

				/* FIXME - not supported */
				أرجع MOSQ_ERR_NOT_SUPPORTED;
			}
		}وإلا إذا(!strncasecmp(http_سرخیاں[i].name, "Sec-WebSocket-Protocol", http_سرخیاں[i].name_len)){
			/* Check for "mqtt" */
			إذا(!لڑی_مقابلہ(http_سرخیاں[i].قدر, "mqtt", http_سرخیاں[i].قدر_len)
					|| !لڑی_مقابلہ(http_سرخیاں[i].قدر, "mqttv3.1", http_سرخیاں[i].قدر_len)){

				subprotocol = http_سرخیاں[i].قدر;
				subprotocol_len = (صحيح)http_سرخیاں[i].قدر_len;
			}
		}وإلا إذا(!strncasecmp(http_سرخیاں[i].name, "X-Forwarded-For", http_سرخیاں[i].name_len)){
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
		}وإلا إذا(!strncasecmp(http_سرخیاں[i].name, "Origin", http_سرخیاں[i].name_len)){
			إذا(mosq->listener){
				bool have_میل = false;
				لكل(صحيح j=0; j<mosq->listener->ws_origin_گنتی; j++){
					إذا(!لڑی_مقابلہ(mosq->listener->ws_origins[j], http_سرخیاں[i].قدر, http_سرخیاں[i].قدر_len)){
						have_میل = true;
						اكسر;
					}
				}
				إذا(!have_میل && mosq->listener->ws_origin_گنتی > 0){
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

	إذا(سرخی_have_upgrade == false || سرخی_have_connection == false
			|| client_کلید == عدم || client_کلید_len == 0){

		// FIXME - 404
		أرجع MOSQ_ERR_UNKNOWN;
	}

	إذا(ws_create_accept_کلید(client_کلید, client_کلید_len, &accept_کلید)){
		أرجع MOSQ_ERR_UNKNOWN;
	}

	packet = mosquitto_calloc(1, حجم(بنية mosquitto__packet) + 1024 + WS_PACKET_وتھ);
	إذا(!packet){
		محفوظ_FREE(accept_کلید);
		أرجع MOSQ_ERR_NOMEM;
	}
	packet->packet_لمائی = (uint32_t )snprintf((حرف *)&packet->payload[WS_PACKET_وتھ], 1024,
			"HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: WebSocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: %s\r\n"
			"Sec-WebSocket-Protocol: %.*s\r\n"
			"\r\n", accept_کلید, subprotocol_len, subprotocol) + WS_PACKET_وتھ;
	محفوظ_FREE(accept_کلید);
	packet->منزل_process = packet->packet_لمائی;

	memset(mosq->وچ_packet.packet_بفر, 0, db.config->packet_بفر_ناپ);
	rc = packet_قطار(mosq, packet);
	http__context_cleanup(mosq);
	ws__context_init(mosq);
	أرجع rc;
}
#جے_انت
