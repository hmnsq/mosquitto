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
	ناکارول_شوی(context);
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح http__read(بنية mosquitto *mosq)
{
	ssize_t read_اوږدتیا;
	ssize_t سرلیک_اوږدتیا;
	تعداد mosquitto_client_state state;
	size_t hlen;
	ثابت حرف *http_لاره, *http_لار;
	size_t http_لاره_len, http_لار_len;
	صحيح http_minor_نسخه;
	size_t http_سرلیک_شمېر = 100;
	بنية phr_سرلیک http_سرلیکونه[100];
	ثابت حرف *client_کلی = عدم;
	size_t client_کلی_len = 0;
	حرف *accept_کلی;
	bool سرلیک_have_upgrade;
	bool سرلیک_have_connection;
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

	hlen = مزي_اوږدوالی((حرف *)mosq->دننه_packet.packet_بفر);
	read_اوږدتیا = net__read(mosq, &mosq->دننه_packet.packet_بفر[hlen], mosq->دننه_packet.packet_بفر_اندازه-hlen);
	إذا(read_اوږدتیا <= 0){
		إذا(read_اوږدتیا == 0){
			أرجع MOSQ_ERR_CONN_ورک; /* EOF */
		}
#که_تعریف_شوی WIN32
		errno = WSAGet_وروستی_Error();
#د_که_پای
		إذا(errno == EAGAIN || errno == COMPAT_EWOULDBLOCK){
			أرجع MOSQ_ERR_SUCCESS;
		}وإلا{
			بدل(errno){
				حالة COMPAT_ECONNRESET:
					أرجع MOSQ_ERR_CONN_ورک;
				حالة COMPAT_EINTR:
					أرجع MOSQ_ERR_SUCCESS;
				افتراضي:
					أرجع MOSQ_ERR_ERRNO;
			}
		}
	}

	mosq->دننه_packet.packet_بفر[mosq->دننه_packet.packet_بفر_اندازه-1] = '\0'; /* Always 0 terminate */
	سرلیک_اوږدتیا = phr_parse_request((حرف *)mosq->دننه_packet.packet_بفر, مزي_اوږدوالی((حرف *)mosq->دننه_packet.packet_بفر),
			&http_لاره, &http_لاره_len,
			&http_لار, &http_لار_len,
			&http_minor_نسخه,
			http_سرلیکونه, &http_سرلیک_شمېر,
			0);
	// FIXME - deal with partial read !
	إذا(سرلیک_اوږدتیا == -2){
		// Partial read
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا إذا(سرلیک_اوږدتیا == -1){
		// Error
		أرجع MOSQ_ERR_UNKNOWN;
	}وإلا إذا(سرلیک_اوږدتیا < read_اوږدتیا){
		/* Excess data which can't be handled because the client doesn't have a key yet */
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}

	إذا(مزي_پرتله(http_لاره, "GET", http_لاره_len) && مزي_پرتله(http_لاره, "HEAD", http_لاره_len)){
		/* FIXME Not supported - send 501 response */
		أرجع MOSQ_ERR_UNKNOWN;
	}

	سرلیک_have_upgrade = false;
	سرلیک_have_connection = false;
	subprotocol = عدم;

	لكل(size_t i=0; i<http_سرلیک_شمېر; i++){
		إذا(!strncasecmp(http_سرلیکونه[i].name, "Upgrade", http_سرلیکونه[i].name_len)){
			إذا(!strncasecmp(http_سرلیکونه[i].ارزښت, "websocket", http_سرلیکونه[i].ارزښت_len)){
				سرلیک_have_upgrade = true;
			}
		}وإلا إذا(!strncasecmp(http_سرلیکونه[i].name, "Connection", http_سرلیکونه[i].name_len)){
			/* Check for "upgrade" */
			ثابت حرف *str = http_سرلیکونه[i].ارزښت;
			size_t پیل = 0;
			size_t j;
			لكل(j=0; j<http_سرلیکونه[i].ارزښت_len; j++){
				إذا(str[j] == ','){
					إذا(!strncasecmp(&str[پیل], "upgrade", http_سرلیکونه[i].ارزښت_len-j)){
						سرلیک_have_connection = true;
						اكسر;
					}وإلا{
						پیل = j+1;
					}
				}وإلا إذا(str[j] == ' '){
					پیل = j+1;
				}
			}
			إذا(!strncasecmp(&str[پیل], "upgrade", http_سرلیکونه[i].ارزښت_len-j)){
				سرلیک_have_connection = true;
			}
		}وإلا إذا(!strncasecmp(http_سرلیکونه[i].name, "Sec-WebSocket-Key", http_سرلیکونه[i].name_len)){
			client_کلی = http_سرلیکونه[i].ارزښت;
			client_کلی_len = http_سرلیکونه[i].ارزښت_len;
		}وإلا إذا(!strncasecmp(http_سرلیکونه[i].name, "Sec-WebSocket-Version", http_سرلیکونه[i].name_len)){
			/* Check for "13" */
			إذا(http_سرلیکونه[i].ارزښت_len != 2
					|| http_سرلیکونه[i].ارزښت[0] != '1'
					|| http_سرلیکونه[i].ارزښت[1] != '3'
					){

				/* FIXME - not supported */
				أرجع MOSQ_ERR_NOT_SUPPORTED;
			}
		}وإلا إذا(!strncasecmp(http_سرلیکونه[i].name, "Sec-WebSocket-Protocol", http_سرلیکونه[i].name_len)){
			/* Check for "mqtt" */
			إذا(!مزي_پرتله(http_سرلیکونه[i].ارزښت, "mqtt", http_سرلیکونه[i].ارزښت_len)
					|| !مزي_پرتله(http_سرلیکونه[i].ارزښت, "mqttv3.1", http_سرلیکونه[i].ارزښت_len)){

				subprotocol = http_سرلیکونه[i].ارزښت;
				subprotocol_len = (صحيح)http_سرلیکونه[i].ارزښت_len;
			}
		}وإلا إذا(!strncasecmp(http_سرلیکونه[i].name, "X-Forwarded-For", http_سرلیکونه[i].name_len)){
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
		}وإلا إذا(!strncasecmp(http_سرلیکونه[i].name, "Origin", http_سرلیکونه[i].name_len)){
			إذا(mosq->listener){
				bool have_سمون = false;
				لكل(صحيح j=0; j<mosq->listener->ws_origin_شمېر; j++){
					إذا(!مزي_پرتله(mosq->listener->ws_origins[j], http_سرلیکونه[i].ارزښت, http_سرلیکونه[i].ارزښت_len)){
						have_سمون = true;
						اكسر;
					}
				}
				إذا(!have_سمون && mosq->listener->ws_origin_شمېر > 0){
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

	إذا(سرلیک_have_upgrade == false || سرلیک_have_connection == false
			|| client_کلی == عدم || client_کلی_len == 0){

		// FIXME - 404
		أرجع MOSQ_ERR_UNKNOWN;
	}

	إذا(ws_create_accept_کلی(client_کلی, client_کلی_len, &accept_کلی)){
		أرجع MOSQ_ERR_UNKNOWN;
	}

	packet = mosquitto_calloc(1, حجم(بنية mosquitto__packet) + 1024 + WS_PACKET_دننه_واټن);
	إذا(!packet){
		خوندي_FREE(accept_کلی);
		أرجع MOSQ_ERR_NOMEM;
	}
	packet->packet_اوږدتیا = (uint32_t )snprintf((حرف *)&packet->payload[WS_PACKET_دننه_واټن], 1024,
			"HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: WebSocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: %s\r\n"
			"Sec-WebSocket-Protocol: %.*s\r\n"
			"\r\n", accept_کلی, subprotocol_len, subprotocol) + WS_PACKET_دننه_واټن;
	خوندي_FREE(accept_کلی);
	packet->منزل_process = packet->packet_اوږدتیا;

	memset(mosq->دننه_packet.packet_بفر, 0, db.config->packet_بفر_اندازه);
	rc = packet_کتار(mosq, packet);
	http__context_cleanup(mosq);
	ws__context_init(mosq);
	أرجع rc;
}
#د_که_پای
