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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "net_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "packet_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "read_handle.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sys_tree.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "picohttpparser.h"


ਪੂਰਨ http__context_init(ਬਣਤਰ mosquitto *context)
{
	context->transport = mosq_t_http;

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ http__context_cleanup(ਬਣਤਰ mosquitto *context)
{
	ਅਣਵਰਤਿਆ(context);
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ http__read(ਬਣਤਰ mosquitto *mosq)
{
	ssize_t read_ਲੰਮਾਈ;
	ssize_t ਸਿਰਲੇਖ_ਲੰਮਾਈ;
	ਗਿਣਤੀ mosquitto_client_state state;
	size_t hlen;
	ਅਚਲ ਅੱਖਰ *http_ਵਿਧੀ, *http_ਰਸਤਾ;
	size_t http_ਵਿਧੀ_len, http_ਰਸਤਾ_len;
	ਪੂਰਨ http_minor_ਸੰਸਕਰਣ;
	size_t http_ਸਿਰਲੇਖ_ਗਿਣਤੀ = 100;
	ਬਣਤਰ phr_ਸਿਰਲੇਖ http_ਸਿਰਲੇਖਾਂ[100];
	ਅਚਲ ਅੱਖਰ *client_ਕੁੰਜੀ = ਨਿਰਾ;
	size_t client_ਕੁੰਜੀ_len = 0;
	ਅੱਖਰ *accept_ਕੁੰਜੀ;
	bool ਸਿਰਲੇਖ_have_upgrade;
	bool ਸਿਰਲੇਖ_have_connection;
	ਬਣਤਰ mosquitto__packet *packet;
	ਪੂਰਨ rc;
	ਅਚਲ ਅੱਖਰ *subprotocol = ਨਿਰਾ;
	ਪੂਰਨ subprotocol_len = 0;

	ਜੇ(!mosq){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}
	ਜੇ(mosq->sock == INVALID_SOCKET){
		ਵਾਪਸ MOSQ_ERR_NO_CONN;
	}

	state = mosquitto__get_state(mosq);
	ਜੇ(state == mosq_cs_connect_pending){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

	hlen = ਲੜੀ_ਲੰਬਾਈ((ਅੱਖਰ *)mosq->ਵਿੱਚ_packet.packet_ਬਫਰ);
	read_ਲੰਮਾਈ = net__read(mosq, &mosq->ਵਿੱਚ_packet.packet_ਬਫਰ[hlen], mosq->ਵਿੱਚ_packet.packet_ਬਫਰ_ਮਾਪ-hlen);
	ਜੇ(read_ਲੰਮਾਈ <= 0){
		ਜੇ(read_ਲੰਮਾਈ == 0){
			ਵਾਪਸ MOSQ_ERR_CONN_ਗੁਆਚਾ; /* EOF */
		}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
		errno = WSAGet_ਆਖਰੀ_Error();
#ਜੇ_ਅੰਤ
		ਜੇ(errno == EAGAIN || errno == COMPAT_EWOULDBLOCK){
			ਵਾਪਸ MOSQ_ERR_SUCCESS;
		}ਨਹੀਂਤਾਂ{
			ਚੋਣ(errno){
				ਹਾਲਤ COMPAT_ECONNRESET:
					ਵਾਪਸ MOSQ_ERR_CONN_ਗੁਆਚਾ;
				ਹਾਲਤ COMPAT_EINTR:
					ਵਾਪਸ MOSQ_ERR_SUCCESS;
				ਮੂਲ:
					ਵਾਪਸ MOSQ_ERR_ERRNO;
			}
		}
	}

	mosq->ਵਿੱਚ_packet.packet_ਬਫਰ[mosq->ਵਿੱਚ_packet.packet_ਬਫਰ_ਮਾਪ-1] = '\0'; /* Always 0 terminate */
	ਸਿਰਲੇਖ_ਲੰਮਾਈ = phr_parse_request((ਅੱਖਰ *)mosq->ਵਿੱਚ_packet.packet_ਬਫਰ, ਲੜੀ_ਲੰਬਾਈ((ਅੱਖਰ *)mosq->ਵਿੱਚ_packet.packet_ਬਫਰ),
			&http_ਵਿਧੀ, &http_ਵਿਧੀ_len,
			&http_ਰਸਤਾ, &http_ਰਸਤਾ_len,
			&http_minor_ਸੰਸਕਰਣ,
			http_ਸਿਰਲੇਖਾਂ, &http_ਸਿਰਲੇਖ_ਗਿਣਤੀ,
			0);
	// FIXME - deal with partial read !
	ਜੇ(ਸਿਰਲੇਖ_ਲੰਮਾਈ == -2){
		// Partial read
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}ਨਹੀਂਤਾਂ ਜੇ(ਸਿਰਲੇਖ_ਲੰਮਾਈ == -1){
		// Error
		ਵਾਪਸ MOSQ_ERR_UNKNOWN;
	}ਨਹੀਂਤਾਂ ਜੇ(ਸਿਰਲੇਖ_ਲੰਮਾਈ < read_ਲੰਮਾਈ){
		/* Excess data which can't be handled because the client doesn't have a key yet */
		ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
	}

	ਜੇ(ਲੜੀ_ਤੁਲਨਾ(http_ਵਿਧੀ, "GET", http_ਵਿਧੀ_len) && ਲੜੀ_ਤੁਲਨਾ(http_ਵਿਧੀ, "HEAD", http_ਵਿਧੀ_len)){
		/* FIXME Not supported - send 501 response */
		ਵਾਪਸ MOSQ_ERR_UNKNOWN;
	}

	ਸਿਰਲੇਖ_have_upgrade = false;
	ਸਿਰਲੇਖ_have_connection = false;
	subprotocol = ਨਿਰਾ;

	ਲਈ(size_t i=0; i<http_ਸਿਰਲੇਖ_ਗਿਣਤੀ; i++){
		ਜੇ(!strncasecmp(http_ਸਿਰਲੇਖਾਂ[i].name, "Upgrade", http_ਸਿਰਲੇਖਾਂ[i].name_len)){
			ਜੇ(!strncasecmp(http_ਸਿਰਲੇਖਾਂ[i].ਮੁੱਲ, "websocket", http_ਸਿਰਲੇਖਾਂ[i].ਮੁੱਲ_len)){
				ਸਿਰਲੇਖ_have_upgrade = true;
			}
		}ਨਹੀਂਤਾਂ ਜੇ(!strncasecmp(http_ਸਿਰਲੇਖਾਂ[i].name, "Connection", http_ਸਿਰਲੇਖਾਂ[i].name_len)){
			/* Check for "upgrade" */
			ਅਚਲ ਅੱਖਰ *str = http_ਸਿਰਲੇਖਾਂ[i].ਮੁੱਲ;
			size_t ਸ਼ੁਰੂ = 0;
			size_t j;
			ਲਈ(j=0; j<http_ਸਿਰਲੇਖਾਂ[i].ਮੁੱਲ_len; j++){
				ਜੇ(str[j] == ','){
					ਜੇ(!strncasecmp(&str[ਸ਼ੁਰੂ], "upgrade", http_ਸਿਰਲੇਖਾਂ[i].ਮੁੱਲ_len-j)){
						ਸਿਰਲੇਖ_have_connection = true;
						ਰੋਕੋ;
					}ਨਹੀਂਤਾਂ{
						ਸ਼ੁਰੂ = j+1;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(str[j] == ' '){
					ਸ਼ੁਰੂ = j+1;
				}
			}
			ਜੇ(!strncasecmp(&str[ਸ਼ੁਰੂ], "upgrade", http_ਸਿਰਲੇਖਾਂ[i].ਮੁੱਲ_len-j)){
				ਸਿਰਲੇਖ_have_connection = true;
			}
		}ਨਹੀਂਤਾਂ ਜੇ(!strncasecmp(http_ਸਿਰਲੇਖਾਂ[i].name, "Sec-WebSocket-Key", http_ਸਿਰਲੇਖਾਂ[i].name_len)){
			client_ਕੁੰਜੀ = http_ਸਿਰਲੇਖਾਂ[i].ਮੁੱਲ;
			client_ਕੁੰਜੀ_len = http_ਸਿਰਲੇਖਾਂ[i].ਮੁੱਲ_len;
		}ਨਹੀਂਤਾਂ ਜੇ(!strncasecmp(http_ਸਿਰਲੇਖਾਂ[i].name, "Sec-WebSocket-Version", http_ਸਿਰਲੇਖਾਂ[i].name_len)){
			/* Check for "13" */
			ਜੇ(http_ਸਿਰਲੇਖਾਂ[i].ਮੁੱਲ_len != 2
					|| http_ਸਿਰਲੇਖਾਂ[i].ਮੁੱਲ[0] != '1'
					|| http_ਸਿਰਲੇਖਾਂ[i].ਮੁੱਲ[1] != '3'
					){

				/* FIXME - not supported */
				ਵਾਪਸ MOSQ_ERR_NOT_SUPPORTED;
			}
		}ਨਹੀਂਤਾਂ ਜੇ(!strncasecmp(http_ਸਿਰਲੇਖਾਂ[i].name, "Sec-WebSocket-Protocol", http_ਸਿਰਲੇਖਾਂ[i].name_len)){
			/* Check for "mqtt" */
			ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(http_ਸਿਰਲੇਖਾਂ[i].ਮੁੱਲ, "mqtt", http_ਸਿਰਲੇਖਾਂ[i].ਮੁੱਲ_len)
					|| !ਲੜੀ_ਤੁਲਨਾ(http_ਸਿਰਲੇਖਾਂ[i].ਮੁੱਲ, "mqttv3.1", http_ਸਿਰਲੇਖਾਂ[i].ਮੁੱਲ_len)){

				subprotocol = http_ਸਿਰਲੇਖਾਂ[i].ਮੁੱਲ;
				subprotocol_len = (ਪੂਰਨ)http_ਸਿਰਲੇਖਾਂ[i].ਮੁੱਲ_len;
			}
		}ਨਹੀਂਤਾਂ ਜੇ(!strncasecmp(http_ਸਿਰਲੇਖਾਂ[i].name, "X-Forwarded-For", http_ਸਿਰਲੇਖਾਂ[i].name_len)){
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
		}ਨਹੀਂਤਾਂ ਜੇ(!strncasecmp(http_ਸਿਰਲੇਖਾਂ[i].name, "Origin", http_ਸਿਰਲੇਖਾਂ[i].name_len)){
			ਜੇ(mosq->listener){
				bool have_ਮੇਲ = false;
				ਲਈ(ਪੂਰਨ j=0; j<mosq->listener->ws_origin_ਗਿਣਤੀ; j++){
					ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(mosq->listener->ws_origins[j], http_ਸਿਰਲੇਖਾਂ[i].ਮੁੱਲ, http_ਸਿਰਲੇਖਾਂ[i].ਮੁੱਲ_len)){
						have_ਮੇਲ = true;
						ਰੋਕੋ;
					}
				}
				ਜੇ(!have_ਮੇਲ && mosq->listener->ws_origin_ਗਿਣਤੀ > 0){
					ਵਾਪਸ MOSQ_ERR_HTTP_BAD_ORIGIN;
				}
			}
		}ਨਹੀਂਤਾਂ{
			/* Unknown header */
		}
	}

	ਜੇ(subprotocol == ਨਿਰਾ){
		// FIXME ?
		ਵਾਪਸ MOSQ_ERR_UNKNOWN;
	}

	ਜੇ(ਸਿਰਲੇਖ_have_upgrade == false || ਸਿਰਲੇਖ_have_connection == false
			|| client_ਕੁੰਜੀ == ਨਿਰਾ || client_ਕੁੰਜੀ_len == 0){

		// FIXME - 404
		ਵਾਪਸ MOSQ_ERR_UNKNOWN;
	}

	ਜੇ(ws_create_accept_ਕੁੰਜੀ(client_ਕੁੰਜੀ, client_ਕੁੰਜੀ_len, &accept_ਕੁੰਜੀ)){
		ਵਾਪਸ MOSQ_ERR_UNKNOWN;
	}

	packet = mosquitto_calloc(1, ਆਕਾਰ(ਬਣਤਰ mosquitto__packet) + 1024 + WS_PACKET_ਵਿੱਥ);
	ਜੇ(!packet){
		ਸੁਰੱਖਿਅਤ_FREE(accept_ਕੁੰਜੀ);
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}
	packet->packet_ਲੰਮਾਈ = (uint32_t )snprintf((ਅੱਖਰ *)&packet->payload[WS_PACKET_ਵਿੱਥ], 1024,
			"HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: WebSocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: %s\r\n"
			"Sec-WebSocket-Protocol: %.*s\r\n"
			"\r\n", accept_ਕੁੰਜੀ, subprotocol_len, subprotocol) + WS_PACKET_ਵਿੱਥ;
	ਸੁਰੱਖਿਅਤ_FREE(accept_ਕੁੰਜੀ);
	packet->ਮੰਜ਼ਿਲ_process = packet->packet_ਲੰਮਾਈ;

	memset(mosq->ਵਿੱਚ_packet.packet_ਬਫਰ, 0, db.config->packet_ਬਫਰ_ਮਾਪ);
	rc = packet_ਕਤਾਰ(mosq, packet);
	http__context_cleanup(mosq);
	ws__context_init(mosq);
	ਵਾਪਸ rc;
}
#ਜੇ_ਅੰਤ
