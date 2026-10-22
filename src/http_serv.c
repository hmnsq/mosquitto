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

#समावेश "config.h"

#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN

#समावेश <assert.h>
#समावेश <errno.h>
#समावेश <string.h>

#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/mqtt_protocol.h"
#समावेश "net_mosq.h"
#समावेश "packet_mosq.h"
#समावेश "read_handle.h"
#समावेश "sys_tree.h"
#समावेश "util_mosq.h"
#समावेश "picohttpparser.h"


पूर्णांक http__context_init(काष्ठा mosquitto *context)
{
	context->transport = mosq_t_http;

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक http__context_cleanup(काष्ठा mosquitto *context)
{
	UNUSED(context);
	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक http__read(काष्ठा mosquitto *mosq)
{
	ssize_t read_लंबाई;
	ssize_t header_लंबाई;
	क्रमागत mosquitto_client_state state;
	माप_प्रकार hlen;
	स्थिर अक्षर *http_method, *http_path;
	माप_प्रकार http_method_len, http_path_len;
	पूर्णांक http_minor_version;
	माप_प्रकार http_header_गिनती = 100;
	काष्ठा phr_header http_headers[100];
	स्थिर अक्षर *client_कुंजी = शून्य;
	माप_प्रकार client_कुंजी_len = 0;
	अक्षर *accept_कुंजी;
	bool header_have_upgrade;
	bool header_have_connection;
	काष्ठा mosquitto__packet *packet;
	पूर्णांक rc;
	स्थिर अक्षर *subprotocol = शून्य;
	पूर्णांक subprotocol_len = 0;

	अगर(!mosq){
		वापस MOSQ_ERR_INVAL;
	}
	अगर(mosq->sock == INVALID_SOCKET){
		वापस MOSQ_ERR_NO_CONN;
	}

	state = mosquitto__get_state(mosq);
	अगर(state == mosq_cs_connect_pending){
		वापस MOSQ_ERR_SUCCESS;
	}

	hlen = म_माप((अक्षर *)mosq->in_packet.packet_बफर);
	read_लंबाई = net__read(mosq, &mosq->in_packet.packet_बफर[hlen], mosq->in_packet.packet_बफर_आकार-hlen);
	अगर(read_लंबाई <= 0){
		अगर(read_लंबाई == 0){
			वापस MOSQ_ERR_CONN_LOST; /* EOF */
		}
#अगर_घोषित WIN32
		त्रुटि_सं = WSAGetLastError();
#पूर्ण_अगर
		अगर(त्रुटि_सं == EAGAIN || त्रुटि_सं == COMPAT_EWOULDBLOCK){
			वापस MOSQ_ERR_SUCCESS;
		}अन्यथा{
			चयन(त्रुटि_सं){
				हाल COMPAT_ECONNRESET:
					वापस MOSQ_ERR_CONN_LOST;
				हाल COMPAT_EINTR:
					वापस MOSQ_ERR_SUCCESS;
				शेष:
					वापस MOSQ_ERR_ERRNO;
			}
		}
	}

	mosq->in_packet.packet_बफर[mosq->in_packet.packet_बफर_आकार-1] = '\0'; /* Always 0 terminate */
	header_लंबाई = phr_parse_request((अक्षर *)mosq->in_packet.packet_बफर, म_माप((अक्षर *)mosq->in_packet.packet_बफर),
			&http_method, &http_method_len,
			&http_path, &http_path_len,
			&http_minor_version,
			http_headers, &http_header_गिनती,
			0);
	// FIXME - deal with partial read !
	अगर(header_लंबाई == -2){
		// Partial read
		वापस MOSQ_ERR_SUCCESS;
	}अन्यथा अगर(header_लंबाई == -1){
		// Error
		वापस MOSQ_ERR_UNKNOWN;
	}अन्यथा अगर(header_लंबाई < read_लंबाई){
		/* Excess data which can't be handled because the client doesn't have a key yet */
		वापस MOSQ_ERR_MALFORMED_PACKET;
	}

	अगर(म_भेदन(http_method, "GET", http_method_len) && म_भेदन(http_method, "HEAD", http_method_len)){
		/* FIXME Not supported - send 501 response */
		वापस MOSQ_ERR_UNKNOWN;
	}

	header_have_upgrade = false;
	header_have_connection = false;
	subprotocol = शून्य;

	क्रम(माप_प्रकार i=0; i<http_header_गिनती; i++){
		अगर(!strncasecmp(http_headers[i].name, "Upgrade", http_headers[i].name_len)){
			अगर(!strncasecmp(http_headers[i].मूल्य, "websocket", http_headers[i].मूल्य_len)){
				header_have_upgrade = true;
			}
		}अन्यथा अगर(!strncasecmp(http_headers[i].name, "Connection", http_headers[i].name_len)){
			/* Check for "upgrade" */
			स्थिर अक्षर *str = http_headers[i].मूल्य;
			माप_प्रकार start = 0;
			माप_प्रकार j;
			क्रम(j=0; j<http_headers[i].मूल्य_len; j++){
				अगर(str[j] == ','){
					अगर(!strncasecmp(&str[start], "upgrade", http_headers[i].मूल्य_len-j)){
						header_have_connection = true;
						अवरोध;
					}अन्यथा{
						start = j+1;
					}
				}अन्यथा अगर(str[j] == ' '){
					start = j+1;
				}
			}
			अगर(!strncasecmp(&str[start], "upgrade", http_headers[i].मूल्य_len-j)){
				header_have_connection = true;
			}
		}अन्यथा अगर(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Key", http_headers[i].name_len)){
			client_कुंजी = http_headers[i].मूल्य;
			client_कुंजी_len = http_headers[i].मूल्य_len;
		}अन्यथा अगर(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Version", http_headers[i].name_len)){
			/* Check for "13" */
			अगर(http_headers[i].मूल्य_len != 2
					|| http_headers[i].मूल्य[0] != '1'
					|| http_headers[i].मूल्य[1] != '3'
					){

				/* FIXME - not supported */
				वापस MOSQ_ERR_NOT_SUPPORTED;
			}
		}अन्यथा अगर(!strncasecmp(http_headers[i].name, "Sec-WebSocket-Protocol", http_headers[i].name_len)){
			/* Check for "mqtt" */
			अगर(!म_भेदन(http_headers[i].मूल्य, "mqtt", http_headers[i].मूल्य_len)
					|| !म_भेदन(http_headers[i].मूल्य, "mqttv3.1", http_headers[i].मूल्य_len)){

				subprotocol = http_headers[i].मूल्य;
				subprotocol_len = (पूर्णांक)http_headers[i].मूल्य_len;
			}
		}अन्यथा अगर(!strncasecmp(http_headers[i].name, "X-Forwarded-For", http_headers[i].name_len)){
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
		}अन्यथा अगर(!strncasecmp(http_headers[i].name, "Origin", http_headers[i].name_len)){
			अगर(mosq->listener){
				bool have_match = false;
				क्रम(पूर्णांक j=0; j<mosq->listener->ws_origin_गिनती; j++){
					अगर(!म_भेदन(mosq->listener->ws_origins[j], http_headers[i].मूल्य, http_headers[i].मूल्य_len)){
						have_match = true;
						अवरोध;
					}
				}
				अगर(!have_match && mosq->listener->ws_origin_गिनती > 0){
					वापस MOSQ_ERR_HTTP_BAD_ORIGIN;
				}
			}
		}अन्यथा{
			/* Unknown header */
		}
	}

	अगर(subprotocol == शून्य){
		// FIXME ?
		वापस MOSQ_ERR_UNKNOWN;
	}

	अगर(header_have_upgrade == false || header_have_connection == false
			|| client_कुंजी == शून्य || client_कुंजी_len == 0){

		// FIXME - 404
		वापस MOSQ_ERR_UNKNOWN;
	}

	अगर(ws_create_accept_कुंजी(client_कुंजी, client_कुंजी_len, &accept_कुंजी)){
		वापस MOSQ_ERR_UNKNOWN;
	}

	packet = mosquitto_calloc(1, माप(काष्ठा mosquitto__packet) + 1024 + WS_PACKET_OFFSET);
	अगर(!packet){
		SAFE_FREE(accept_कुंजी);
		वापस MOSQ_ERR_NOMEM;
	}
	packet->packet_लंबाई = (uint32_t )snprintf((अक्षर *)&packet->payload[WS_PACKET_OFFSET], 1024,
			"HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: WebSocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: %s\r\n"
			"Sec-WebSocket-Protocol: %.*s\r\n"
			"\r\n", accept_कुंजी, subprotocol_len, subprotocol) + WS_PACKET_OFFSET;
	SAFE_FREE(accept_कुंजी);
	packet->to_process = packet->packet_लंबाई;

	स_रखो(mosq->in_packet.packet_बफर, 0, db.config->packet_बफर_आकार);
	rc = packet__queue(mosq, packet);
	http__context_cleanup(mosq);
	ws__context_init(mosq);
	वापस rc;
}
#पूर्ण_अगर
