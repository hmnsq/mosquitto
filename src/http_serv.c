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


صحیح http__context_init(ساخت mosquitto *context)
{
	context->transport = mosq_t_http;

	واپس MOSQ_ERR_SUCCESS;
}


صحیح http__context_cleanup(ساخت mosquitto *context)
{
	غیر_مستعمل(context);
	واپس MOSQ_ERR_SUCCESS;
}


صحیح http__read(ساخت mosquitto *mosq)
{
	ssize_t read_لمبائی;
	ssize_t سرخی_لمبائی;
	شمار mosquitto_client_state state;
	size_t hlen;
	ثابت حرف *http_طریقہ, *http_راستہ;
	size_t http_طریقہ_len, http_راستہ_len;
	صحیح http_minor_نسخہ;
	size_t http_سرخی_گنتی = 100;
	ساخت phr_سرخی http_سرخیاں[100];
	ثابت حرف *client_کلید = تہی;
	size_t client_کلید_len = 0;
	حرف *accept_کلید;
	bool سرخی_have_upgrade;
	bool سرخی_have_connection;
	ساخت mosquitto__packet *packet;
	صحیح rc;
	ثابت حرف *subprotocol = تہی;
	صحیح subprotocol_len = 0;

	اگر(!mosq){
		واپس MOSQ_ERR_INVAL;
	}
	اگر(mosq->sock == INVALID_SOCKET){
		واپس MOSQ_ERR_NO_CONN;
	}

	state = mosquitto__get_state(mosq);
	اگر(state == mosq_cs_connect_pending){
		واپس MOSQ_ERR_SUCCESS;
	}

	hlen = لڑی_لمبائی((حرف *)mosq->اندر_packet.packet_بفر);
	read_لمبائی = net__read(mosq, &mosq->اندر_packet.packet_بفر[hlen], mosq->اندر_packet.packet_بفر_سائز-hlen);
	اگر(read_لمبائی <= 0){
		اگر(read_لمبائی == 0){
			واپس MOSQ_ERR_CONN_گمشدہ; /* EOF */
		}
#اگر_تعریف_شدہ WIN32
		errno = WSAGet_آخری_Error();
#اگر_ختم
		اگر(errno == EAGAIN || errno == COMPAT_EWOULDBLOCK){
			واپس MOSQ_ERR_SUCCESS;
		}ورنہ{
			انتخاب(errno){
				حالت COMPAT_ECONNRESET:
					واپس MOSQ_ERR_CONN_گمشدہ;
				حالت COMPAT_EINTR:
					واپس MOSQ_ERR_SUCCESS;
				طےشدہ:
					واپس MOSQ_ERR_ERRNO;
			}
		}
	}

	mosq->اندر_packet.packet_بفر[mosq->اندر_packet.packet_بفر_سائز-1] = '\0'; /* Always 0 terminate */
	سرخی_لمبائی = phr_parse_request((حرف *)mosq->اندر_packet.packet_بفر, لڑی_لمبائی((حرف *)mosq->اندر_packet.packet_بفر),
			&http_طریقہ, &http_طریقہ_len,
			&http_راستہ, &http_راستہ_len,
			&http_minor_نسخہ,
			http_سرخیاں, &http_سرخی_گنتی,
			0);
	// FIXME - deal with partial read !
	اگر(سرخی_لمبائی == -2){
		// Partial read
		واپس MOSQ_ERR_SUCCESS;
	}ورنہ اگر(سرخی_لمبائی == -1){
		// Error
		واپس MOSQ_ERR_UNKNOWN;
	}ورنہ اگر(سرخی_لمبائی < read_لمبائی){
		/* Excess data which can't be handled because the client doesn't have a key yet */
		واپس MOSQ_ERR_MALFORMED_PACKET;
	}

	اگر(لڑی_موازنہ(http_طریقہ, "GET", http_طریقہ_len) && لڑی_موازنہ(http_طریقہ, "HEAD", http_طریقہ_len)){
		/* FIXME Not supported - send 501 response */
		واپس MOSQ_ERR_UNKNOWN;
	}

	سرخی_have_upgrade = false;
	سرخی_have_connection = false;
	subprotocol = تہی;

	برائے(size_t i=0; i<http_سرخی_گنتی; i++){
		اگر(!strncasecmp(http_سرخیاں[i].name, "Upgrade", http_سرخیاں[i].name_len)){
			اگر(!strncasecmp(http_سرخیاں[i].قدر, "websocket", http_سرخیاں[i].قدر_len)){
				سرخی_have_upgrade = true;
			}
		}ورنہ اگر(!strncasecmp(http_سرخیاں[i].name, "Connection", http_سرخیاں[i].name_len)){
			/* Check for "upgrade" */
			ثابت حرف *str = http_سرخیاں[i].قدر;
			size_t آغاز = 0;
			size_t j;
			برائے(j=0; j<http_سرخیاں[i].قدر_len; j++){
				اگر(str[j] == ','){
					اگر(!strncasecmp(&str[آغاز], "upgrade", http_سرخیاں[i].قدر_len-j)){
						سرخی_have_connection = true;
						توڑو;
					}ورنہ{
						آغاز = j+1;
					}
				}ورنہ اگر(str[j] == ' '){
					آغاز = j+1;
				}
			}
			اگر(!strncasecmp(&str[آغاز], "upgrade", http_سرخیاں[i].قدر_len-j)){
				سرخی_have_connection = true;
			}
		}ورنہ اگر(!strncasecmp(http_سرخیاں[i].name, "Sec-WebSocket-Key", http_سرخیاں[i].name_len)){
			client_کلید = http_سرخیاں[i].قدر;
			client_کلید_len = http_سرخیاں[i].قدر_len;
		}ورنہ اگر(!strncasecmp(http_سرخیاں[i].name, "Sec-WebSocket-Version", http_سرخیاں[i].name_len)){
			/* Check for "13" */
			اگر(http_سرخیاں[i].قدر_len != 2
					|| http_سرخیاں[i].قدر[0] != '1'
					|| http_سرخیاں[i].قدر[1] != '3'
					){

				/* FIXME - not supported */
				واپس MOSQ_ERR_NOT_SUPPORTED;
			}
		}ورنہ اگر(!strncasecmp(http_سرخیاں[i].name, "Sec-WebSocket-Protocol", http_سرخیاں[i].name_len)){
			/* Check for "mqtt" */
			اگر(!لڑی_موازنہ(http_سرخیاں[i].قدر, "mqtt", http_سرخیاں[i].قدر_len)
					|| !لڑی_موازنہ(http_سرخیاں[i].قدر, "mqttv3.1", http_سرخیاں[i].قدر_len)){

				subprotocol = http_سرخیاں[i].قدر;
				subprotocol_len = (صحیح)http_سرخیاں[i].قدر_len;
			}
		}ورنہ اگر(!strncasecmp(http_سرخیاں[i].name, "X-Forwarded-For", http_سرخیاں[i].name_len)){
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
		}ورنہ اگر(!strncasecmp(http_سرخیاں[i].name, "Origin", http_سرخیاں[i].name_len)){
			اگر(mosq->listener){
				bool have_مطابقت = false;
				برائے(صحیح j=0; j<mosq->listener->ws_origin_گنتی; j++){
					اگر(!لڑی_موازنہ(mosq->listener->ws_origins[j], http_سرخیاں[i].قدر, http_سرخیاں[i].قدر_len)){
						have_مطابقت = true;
						توڑو;
					}
				}
				اگر(!have_مطابقت && mosq->listener->ws_origin_گنتی > 0){
					واپس MOSQ_ERR_HTTP_BAD_ORIGIN;
				}
			}
		}ورنہ{
			/* Unknown header */
		}
	}

	اگر(subprotocol == تہی){
		// FIXME ?
		واپس MOSQ_ERR_UNKNOWN;
	}

	اگر(سرخی_have_upgrade == false || سرخی_have_connection == false
			|| client_کلید == تہی || client_کلید_len == 0){

		// FIXME - 404
		واپس MOSQ_ERR_UNKNOWN;
	}

	اگر(ws_create_accept_کلید(client_کلید, client_کلید_len, &accept_کلید)){
		واپس MOSQ_ERR_UNKNOWN;
	}

	packet = mosquitto_calloc(1, حجم(ساخت mosquitto__packet) + 1024 + WS_PACKET_انحراف);
	اگر(!packet){
		محفوظ_FREE(accept_کلید);
		واپس MOSQ_ERR_NOMEM;
	}
	packet->packet_لمبائی = (uint32_t )snprintf((حرف *)&packet->payload[WS_PACKET_انحراف], 1024,
			"HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: WebSocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: %s\r\n"
			"Sec-WebSocket-Protocol: %.*s\r\n"
			"\r\n", accept_کلید, subprotocol_len, subprotocol) + WS_PACKET_انحراف;
	محفوظ_FREE(accept_کلید);
	packet->مقصد_process = packet->packet_لمبائی;

	memset(mosq->اندر_packet.packet_بفر, 0, db.config->packet_بفر_سائز);
	rc = packet_قطار(mosq, packet);
	http__context_cleanup(mosq);
	ws__context_init(mosq);
	واپس rc;
}
#اگر_ختم
