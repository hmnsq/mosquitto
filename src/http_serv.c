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
	استفادهنشده(context);
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح http__read(بنية mosquitto *mosq)
{
	ssize_t read_درازی;
	ssize_t سرآیند_درازی;
	تعداد mosquitto_client_state state;
	size_t hlen;
	ثابت حرف *http_روش, *http_مسیر;
	size_t http_روش_len, http_مسیر_len;
	صحيح http_minor_نسخه;
	size_t http_سرآیند_شمار = 100;
	بنية phr_سرآیند http_سرآیندها[100];
	ثابت حرف *client_کلید = عدم;
	size_t client_کلید_len = 0;
	حرف *accept_کلید;
	bool سرآیند_have_upgrade;
	bool سرآیند_have_connection;
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

	hlen = طول_رشته((حرف *)mosq->درون_packet.packet_میانگیر);
	read_درازی = net__read(mosq, &mosq->درون_packet.packet_میانگیر[hlen], mosq->درون_packet.packet_میانگیر_اندازه-hlen);
	إذا(read_درازی <= 0){
		إذا(read_درازی == 0){
			أرجع MOSQ_ERR_CONN_گمشده; /* EOF */
		}
#اگر_تعریف_شده WIN32
		errno = WSAGet_آخرین_Error();
#پایان_اگر
		إذا(errno == EAGAIN || errno == COMPAT_EWOULDBLOCK){
			أرجع MOSQ_ERR_SUCCESS;
		}وإلا{
			بدل(errno){
				حالة COMPAT_ECONNRESET:
					أرجع MOSQ_ERR_CONN_گمشده;
				حالة COMPAT_EINTR:
					أرجع MOSQ_ERR_SUCCESS;
				افتراضي:
					أرجع MOSQ_ERR_ERRNO;
			}
		}
	}

	mosq->درون_packet.packet_میانگیر[mosq->درون_packet.packet_میانگیر_اندازه-1] = '\0'; /* Always 0 terminate */
	سرآیند_درازی = phr_parse_request((حرف *)mosq->درون_packet.packet_میانگیر, طول_رشته((حرف *)mosq->درون_packet.packet_میانگیر),
			&http_روش, &http_روش_len,
			&http_مسیر, &http_مسیر_len,
			&http_minor_نسخه,
			http_سرآیندها, &http_سرآیند_شمار,
			0);
	// FIXME - deal with partial read !
	إذا(سرآیند_درازی == -2){
		// Partial read
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا إذا(سرآیند_درازی == -1){
		// Error
		أرجع MOSQ_ERR_UNKNOWN;
	}وإلا إذا(سرآیند_درازی < read_درازی){
		/* Excess data which can't be handled because the client doesn't have a key yet */
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}

	إذا(مقایسه_رشته(http_روش, "GET", http_روش_len) && مقایسه_رشته(http_روش, "HEAD", http_روش_len)){
		/* FIXME Not supported - send 501 response */
		أرجع MOSQ_ERR_UNKNOWN;
	}

	سرآیند_have_upgrade = false;
	سرآیند_have_connection = false;
	subprotocol = عدم;

	لكل(size_t i=0; i<http_سرآیند_شمار; i++){
		إذا(!strncasecmp(http_سرآیندها[i].name, "Upgrade", http_سرآیندها[i].name_len)){
			إذا(!strncasecmp(http_سرآیندها[i].مقدار, "websocket", http_سرآیندها[i].مقدار_len)){
				سرآیند_have_upgrade = true;
			}
		}وإلا إذا(!strncasecmp(http_سرآیندها[i].name, "Connection", http_سرآیندها[i].name_len)){
			/* Check for "upgrade" */
			ثابت حرف *str = http_سرآیندها[i].مقدار;
			size_t شروع = 0;
			size_t j;
			لكل(j=0; j<http_سرآیندها[i].مقدار_len; j++){
				إذا(str[j] == ','){
					إذا(!strncasecmp(&str[شروع], "upgrade", http_سرآیندها[i].مقدار_len-j)){
						سرآیند_have_connection = true;
						اكسر;
					}وإلا{
						شروع = j+1;
					}
				}وإلا إذا(str[j] == ' '){
					شروع = j+1;
				}
			}
			إذا(!strncasecmp(&str[شروع], "upgrade", http_سرآیندها[i].مقدار_len-j)){
				سرآیند_have_connection = true;
			}
		}وإلا إذا(!strncasecmp(http_سرآیندها[i].name, "Sec-WebSocket-Key", http_سرآیندها[i].name_len)){
			client_کلید = http_سرآیندها[i].مقدار;
			client_کلید_len = http_سرآیندها[i].مقدار_len;
		}وإلا إذا(!strncasecmp(http_سرآیندها[i].name, "Sec-WebSocket-Version", http_سرآیندها[i].name_len)){
			/* Check for "13" */
			إذا(http_سرآیندها[i].مقدار_len != 2
					|| http_سرآیندها[i].مقدار[0] != '1'
					|| http_سرآیندها[i].مقدار[1] != '3'
					){

				/* FIXME - not supported */
				أرجع MOSQ_ERR_NOT_SUPPORTED;
			}
		}وإلا إذا(!strncasecmp(http_سرآیندها[i].name, "Sec-WebSocket-Protocol", http_سرآیندها[i].name_len)){
			/* Check for "mqtt" */
			إذا(!مقایسه_رشته(http_سرآیندها[i].مقدار, "mqtt", http_سرآیندها[i].مقدار_len)
					|| !مقایسه_رشته(http_سرآیندها[i].مقدار, "mqttv3.1", http_سرآیندها[i].مقدار_len)){

				subprotocol = http_سرآیندها[i].مقدار;
				subprotocol_len = (صحيح)http_سرآیندها[i].مقدار_len;
			}
		}وإلا إذا(!strncasecmp(http_سرآیندها[i].name, "X-Forwarded-For", http_سرآیندها[i].name_len)){
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
		}وإلا إذا(!strncasecmp(http_سرآیندها[i].name, "Origin", http_سرآیندها[i].name_len)){
			إذا(mosq->listener){
				bool have_تطابق = false;
				لكل(صحيح j=0; j<mosq->listener->ws_origin_شمار; j++){
					إذا(!مقایسه_رشته(mosq->listener->ws_origins[j], http_سرآیندها[i].مقدار, http_سرآیندها[i].مقدار_len)){
						have_تطابق = true;
						اكسر;
					}
				}
				إذا(!have_تطابق && mosq->listener->ws_origin_شمار > 0){
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

	إذا(سرآیند_have_upgrade == false || سرآیند_have_connection == false
			|| client_کلید == عدم || client_کلید_len == 0){

		// FIXME - 404
		أرجع MOSQ_ERR_UNKNOWN;
	}

	إذا(ws_create_accept_کلید(client_کلید, client_کلید_len, &accept_کلید)){
		أرجع MOSQ_ERR_UNKNOWN;
	}

	packet = mosquitto_calloc(1, حجم(بنية mosquitto__packet) + 1024 + WS_PACKET_جابه_جایی);
	إذا(!packet){
		امن_FREE(accept_کلید);
		أرجع MOSQ_ERR_NOMEM;
	}
	packet->packet_درازی = (uint32_t )snprintf((حرف *)&packet->payload[WS_PACKET_جابه_جایی], 1024,
			"HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: WebSocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: %s\r\n"
			"Sec-WebSocket-Protocol: %.*s\r\n"
			"\r\n", accept_کلید, subprotocol_len, subprotocol) + WS_PACKET_جابه_جایی;
	امن_FREE(accept_کلید);
	packet->مقصد_process = packet->packet_درازی;

	memset(mosq->درون_packet.packet_میانگیر, 0, db.config->packet_میانگیر_اندازه);
	rc = packet_صف(mosq, packet);
	http__context_cleanup(mosq);
	ws__context_init(mosq);
	أرجع rc;
}
#پایان_اگر
