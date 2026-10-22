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

#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN

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


ಪೂರ್ಣಾಂಕ http__context_init(ರಚನೆ mosquitto *context)
{
	context->transport = mosq_t_http;

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ http__context_cleanup(ರಚನೆ mosquitto *context)
{
	ಬಳಸದ(context);
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ http__read(ರಚನೆ mosquitto *mosq)
{
	ssize_t read_ಉದ್ದಳತೆ;
	ssize_t ಶೀರ್ಷಿಕೆ_ಉದ್ದಳತೆ;
	ಎಣಿಕೆ mosquitto_client_state state;
	size_t hlen;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *http_ವಿಧಾನ, *http_ಮಾರ್ಗ;
	size_t http_ವಿಧಾನ_len, http_ಮಾರ್ಗ_len;
	ಪೂರ್ಣಾಂಕ http_minor_ಆವೃತ್ತಿ;
	size_t http_ಶೀರ್ಷಿಕೆ_ಎಣಿಕೆ = 100;
	ರಚನೆ phr_ಶೀರ್ಷಿಕೆ http_ಶೀರ್ಷಿಕೆಗಳು[100];
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *client_ಕೀಲಿ = ಖಾಲಿ;
	size_t client_ಕೀಲಿ_len = 0;
	ಅಕ್ಷರ *accept_ಕೀಲಿ;
	bool ಶೀರ್ಷಿಕೆ_have_upgrade;
	bool ಶೀರ್ಷಿಕೆ_have_connection;
	ರಚನೆ mosquitto__packet *packet;
	ಪೂರ್ಣಾಂಕ rc;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *subprotocol = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ subprotocol_len = 0;

	ಆದರೆ(!mosq){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}
	ಆದರೆ(mosq->sock == INVALID_SOCKET){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NO_CONN;
	}

	state = mosquitto__get_state(mosq);
	ಆದರೆ(state == mosq_cs_connect_pending){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}

	hlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ((ಅಕ್ಷರ *)mosq->ಒಳ_packet.packet_ಬಫರ್);
	read_ಉದ್ದಳತೆ = net__read(mosq, &mosq->ಒಳ_packet.packet_ಬಫರ್[hlen], mosq->ಒಳ_packet.packet_ಬಫರ್_ಗಾತ್ರ-hlen);
	ಆದರೆ(read_ಉದ್ದಳತೆ <= 0){
		ಆದರೆ(read_ಉದ್ದಳತೆ == 0){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_CONN_ಕಳೆದುಹೋದ; /* EOF */
		}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
		errno = WSAGet_ಕೊನೆಯ_Error();
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ(errno == EAGAIN || errno == COMPAT_EWOULDBLOCK){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಆಯ್ಕೆ(errno){
				ಸ್ಥಿತಿ COMPAT_ECONNRESET:
					ಹಿಂತಿರುಗಿ MOSQ_ERR_CONN_ಕಳೆದುಹೋದ;
				ಸ್ಥಿತಿ COMPAT_EINTR:
					ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
				ಪೂರ್ವನಿಯೋಜಿತ:
					ಹಿಂತಿರುಗಿ MOSQ_ERR_ERRNO;
			}
		}
	}

	mosq->ಒಳ_packet.packet_ಬಫರ್[mosq->ಒಳ_packet.packet_ಬಫರ್_ಗಾತ್ರ-1] = '\0'; /* Always 0 terminate */
	ಶೀರ್ಷಿಕೆ_ಉದ್ದಳತೆ = phr_parse_request((ಅಕ್ಷರ *)mosq->ಒಳ_packet.packet_ಬಫರ್, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ((ಅಕ್ಷರ *)mosq->ಒಳ_packet.packet_ಬಫರ್),
			&http_ವಿಧಾನ, &http_ವಿಧಾನ_len,
			&http_ಮಾರ್ಗ, &http_ಮಾರ್ಗ_len,
			&http_minor_ಆವೃತ್ತಿ,
			http_ಶೀರ್ಷಿಕೆಗಳು, &http_ಶೀರ್ಷಿಕೆ_ಎಣಿಕೆ,
			0);
	// FIXME - deal with partial read !
	ಆದರೆ(ಶೀರ್ಷಿಕೆ_ಉದ್ದಳತೆ == -2){
		// Partial read
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಶೀರ್ಷಿಕೆ_ಉದ್ದಳತೆ == -1){
		// Error
		ಹಿಂತಿರುಗಿ MOSQ_ERR_UNKNOWN;
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಶೀರ್ಷಿಕೆ_ಉದ್ದಳತೆ < read_ಉದ್ದಳತೆ){
		/* Excess data which can't be handled because the client doesn't have a key yet */
		ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
	}

	ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(http_ವಿಧಾನ, "GET", http_ವಿಧಾನ_len) && ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(http_ವಿಧಾನ, "HEAD", http_ವಿಧಾನ_len)){
		/* FIXME Not supported - send 501 response */
		ಹಿಂತಿರುಗಿ MOSQ_ERR_UNKNOWN;
	}

	ಶೀರ್ಷಿಕೆ_have_upgrade = false;
	ಶೀರ್ಷಿಕೆ_have_connection = false;
	subprotocol = ಖಾಲಿ;

	ಗಾಗಿ(size_t i=0; i<http_ಶೀರ್ಷಿಕೆ_ಎಣಿಕೆ; i++){
		ಆದರೆ(!strncasecmp(http_ಶೀರ್ಷಿಕೆಗಳು[i].name, "Upgrade", http_ಶೀರ್ಷಿಕೆಗಳು[i].name_len)){
			ಆದರೆ(!strncasecmp(http_ಶೀರ್ಷಿಕೆಗಳು[i].ಮೌಲ್ಯ, "websocket", http_ಶೀರ್ಷಿಕೆಗಳು[i].ಮೌಲ್ಯ_len)){
				ಶೀರ್ಷಿಕೆ_have_upgrade = true;
			}
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strncasecmp(http_ಶೀರ್ಷಿಕೆಗಳು[i].name, "Connection", http_ಶೀರ್ಷಿಕೆಗಳು[i].name_len)){
			/* Check for "upgrade" */
			ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str = http_ಶೀರ್ಷಿಕೆಗಳು[i].ಮೌಲ್ಯ;
			size_t ಆರಂಭ = 0;
			size_t j;
			ಗಾಗಿ(j=0; j<http_ಶೀರ್ಷಿಕೆಗಳು[i].ಮೌಲ್ಯ_len; j++){
				ಆದರೆ(str[j] == ','){
					ಆದರೆ(!strncasecmp(&str[ಆರಂಭ], "upgrade", http_ಶೀರ್ಷಿಕೆಗಳು[i].ಮೌಲ್ಯ_len-j)){
						ಶೀರ್ಷಿಕೆ_have_connection = true;
						ನಿಲ್ಲಿಸು;
					}ಇಲ್ಲದಿದ್ದರೆ{
						ಆರಂಭ = j+1;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(str[j] == ' '){
					ಆರಂಭ = j+1;
				}
			}
			ಆದರೆ(!strncasecmp(&str[ಆರಂಭ], "upgrade", http_ಶೀರ್ಷಿಕೆಗಳು[i].ಮೌಲ್ಯ_len-j)){
				ಶೀರ್ಷಿಕೆ_have_connection = true;
			}
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strncasecmp(http_ಶೀರ್ಷಿಕೆಗಳು[i].name, "Sec-WebSocket-Key", http_ಶೀರ್ಷಿಕೆಗಳು[i].name_len)){
			client_ಕೀಲಿ = http_ಶೀರ್ಷಿಕೆಗಳು[i].ಮೌಲ್ಯ;
			client_ಕೀಲಿ_len = http_ಶೀರ್ಷಿಕೆಗಳು[i].ಮೌಲ್ಯ_len;
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strncasecmp(http_ಶೀರ್ಷಿಕೆಗಳು[i].name, "Sec-WebSocket-Version", http_ಶೀರ್ಷಿಕೆಗಳು[i].name_len)){
			/* Check for "13" */
			ಆದರೆ(http_ಶೀರ್ಷಿಕೆಗಳು[i].ಮೌಲ್ಯ_len != 2
					|| http_ಶೀರ್ಷಿಕೆಗಳು[i].ಮೌಲ್ಯ[0] != '1'
					|| http_ಶೀರ್ಷಿಕೆಗಳು[i].ಮೌಲ್ಯ[1] != '3'
					){

				/* FIXME - not supported */
				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_SUPPORTED;
			}
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strncasecmp(http_ಶೀರ್ಷಿಕೆಗಳು[i].name, "Sec-WebSocket-Protocol", http_ಶೀರ್ಷಿಕೆಗಳು[i].name_len)){
			/* Check for "mqtt" */
			ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(http_ಶೀರ್ಷಿಕೆಗಳು[i].ಮೌಲ್ಯ, "mqtt", http_ಶೀರ್ಷಿಕೆಗಳು[i].ಮೌಲ್ಯ_len)
					|| !ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(http_ಶೀರ್ಷಿಕೆಗಳು[i].ಮೌಲ್ಯ, "mqttv3.1", http_ಶೀರ್ಷಿಕೆಗಳು[i].ಮೌಲ್ಯ_len)){

				subprotocol = http_ಶೀರ್ಷಿಕೆಗಳು[i].ಮೌಲ್ಯ;
				subprotocol_len = (ಪೂರ್ಣಾಂಕ)http_ಶೀರ್ಷಿಕೆಗಳು[i].ಮೌಲ್ಯ_len;
			}
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strncasecmp(http_ಶೀರ್ಷಿಕೆಗಳು[i].name, "X-Forwarded-For", http_ಶೀರ್ಷಿಕೆಗಳು[i].name_len)){
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
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strncasecmp(http_ಶೀರ್ಷಿಕೆಗಳು[i].name, "Origin", http_ಶೀರ್ಷಿಕೆಗಳು[i].name_len)){
			ಆದರೆ(mosq->listener){
				bool have_ಹೊಂದಿಕೆ = false;
				ಗಾಗಿ(ಪೂರ್ಣಾಂಕ j=0; j<mosq->listener->ws_origin_ಎಣಿಕೆ; j++){
					ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(mosq->listener->ws_origins[j], http_ಶೀರ್ಷಿಕೆಗಳು[i].ಮೌಲ್ಯ, http_ಶೀರ್ಷಿಕೆಗಳು[i].ಮೌಲ್ಯ_len)){
						have_ಹೊಂದಿಕೆ = true;
						ನಿಲ್ಲಿಸು;
					}
				}
				ಆದರೆ(!have_ಹೊಂದಿಕೆ && mosq->listener->ws_origin_ಎಣಿಕೆ > 0){
					ಹಿಂತಿರುಗಿ MOSQ_ERR_HTTP_BAD_ORIGIN;
				}
			}
		}ಇಲ್ಲದಿದ್ದರೆ{
			/* Unknown header */
		}
	}

	ಆದರೆ(subprotocol == ಖಾಲಿ){
		// FIXME ?
		ಹಿಂತಿರುಗಿ MOSQ_ERR_UNKNOWN;
	}

	ಆದರೆ(ಶೀರ್ಷಿಕೆ_have_upgrade == false || ಶೀರ್ಷಿಕೆ_have_connection == false
			|| client_ಕೀಲಿ == ಖಾಲಿ || client_ಕೀಲಿ_len == 0){

		// FIXME - 404
		ಹಿಂತಿರುಗಿ MOSQ_ERR_UNKNOWN;
	}

	ಆದರೆ(ws_create_accept_ಕೀಲಿ(client_ಕೀಲಿ, client_ಕೀಲಿ_len, &accept_ಕೀಲಿ)){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_UNKNOWN;
	}

	packet = mosquitto_calloc(1, ಗಾತ್ರ(ರಚನೆ mosquitto__packet) + 1024 + WS_PACKET_ಸ್ಥಾನಾಂತರ);
	ಆದರೆ(!packet){
		ಸುರಕ್ಷಿತ_FREE(accept_ಕೀಲಿ);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}
	packet->packet_ಉದ್ದಳತೆ = (uint32_t )snprintf((ಅಕ್ಷರ *)&packet->payload[WS_PACKET_ಸ್ಥಾನಾಂತರ], 1024,
			"HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: WebSocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: %s\r\n"
			"Sec-WebSocket-Protocol: %.*s\r\n"
			"\r\n", accept_ಕೀಲಿ, subprotocol_len, subprotocol) + WS_PACKET_ಸ್ಥಾನಾಂತರ;
	ಸುರಕ್ಷಿತ_FREE(accept_ಕೀಲಿ);
	packet->ಗಮ್ಯ_process = packet->packet_ಉದ್ದಳತೆ;

	memset(mosq->ಒಳ_packet.packet_ಬಫರ್, 0, db.config->packet_ಬಫರ್_ಗಾತ್ರ);
	rc = packet_ಸರತಿ(mosq, packet);
	http__context_cleanup(mosq);
	ws__context_init(mosq);
	ಹಿಂತಿರುಗಿ rc;
}
#ಆದರೆ_ಅಂತ್ಯ
