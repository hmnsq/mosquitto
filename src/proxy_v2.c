#अगर_घोषित WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#अन्यथा
#  include <arpa/inet.h>
#  include <netinet/in.h>
#पूर्ण_अगर
#समावेश <stdint.h>
#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto_internal.h"
#समावेश "net_mosq.h"

#अगर !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN

#घोषणा PROXY_CMD_LOCAL 0x00
#घोषणा PROXY_CMD_PROXY 0x01

#घोषणा PROXY_TCP_IPV4 0x11
#घोषणा PROXY_TCP_IPV6 0x21
#घोषणा PROXY_TCP_UNIX 0x31

#घोषणा PP2_TYPE_ALPN           0x01
#घोषणा PP2_TYPE_AUTHORITY      0x02
#घोषणा PP2_TYPE_CRC32C         0x03
#घोषणा PP2_TYPE_NOOP           0x04
#घोषणा PP2_TYPE_UNIQUE_ID      0x05
#घोषणा PP2_TYPE_SSL            0x20
#घोषणा PP2_SUBTYPE_SSL_VERSION 0x21
#घोषणा PP2_SUBTYPE_SSL_CN      0x22
#घोषणा PP2_SUBTYPE_SSL_CIPHER  0x23
#घोषणा PP2_SUBTYPE_SSL_SIG_ALG 0x24
#घोषणा PP2_SUBTYPE_SSL_कुंजी_ALG 0x25
#घोषणा PP2_TYPE_NETNS          0x30

#घोषणा PP2_CLIENT_SSL           0x01
#घोषणा PP2_CLIENT_CERT_CONN     0x02
#घोषणा PP2_CLIENT_CERT_SESS     0x04

#घोषणा PROXY_PACKET_LIMIT 500

काष्ठा proxy_hdr_v2 {
	uint8_t sig[12]; /* hex 0D 0A 0D 0A 00 0D 0A 51 55 49 54 0A */
	uint8_t ver_cmd; /* protocol version and command */
	uint8_t fam; /* protocol family and address */
	uint16_t len; /* number of following bytes part of the header */
};

जोड़ proxy_addr {
	काष्ठा { /* for TCP/UDP over IPv4, len = 12 */
		uint32_t src_addr;
		uint32_t dst_addr;
		uint16_t src_port;
		uint16_t dst_port;
	} ipv4_addr;
	काष्ठा { /* for TCP/UDP over IPv6, len = 36 */
		uint8_t src_addr[16];
		uint8_t dst_addr[16];
		uint16_t src_port;
		uint16_t dst_port;
	} ipv6_addr;
	काष्ठा { /* for AF_UNIX sockets, len = 216 */
		uint8_t src_addr[108];
		uint8_t dst_addr[108];
	} unix_addr;
};

काष्ठा pp2_tlv {
	uint8_t type;
	uint8_t लंबाई_h;
	uint8_t लंबाई_l;
};

काष्ठा pp2_tlv_ssl {
	uint8_t client;
	uint32_t verify;
};

स्थिर uint8_t signature[12] = {0x0D, 0x0A, 0x0D, 0x0A, 0x00, 0x0D, 0x0A, 0x51, 0x55, 0x49, 0x54, 0x0A};


अटल व्योम proxy_cleanup(काष्ठा mosquitto *context)
{
	mosquitto_FREE(context->proxy.buf);
	mosquitto_FREE(context->proxy.tls_version);
	mosquitto_FREE(context->proxy.cipher);
}


अटल पूर्णांक read_tlv_ssl(काष्ठा mosquitto *context, uint16_t len, bool *have_certificate)
{
	काष्ठा pp2_tlv_ssl ssl = {0};

	अगर(len < माप(uint8_t) + माप(uint32_t)){
		वापस MOSQ_ERR_INVAL;
	}

	ssl.client = context->proxy.buf[context->proxy.pos];
	ssl.verify = ntohl(*(uint32_t *)(&context->proxy.buf[context->proxy.pos + माप(uint8_t)]));
	context->proxy.pos = (uint16_t)(context->proxy.pos + माप(uint8_t) + माप(uint32_t));
	len = (uint16_t)(len - (माप(uint8_t) + माप(uint32_t)));

	अगर(ssl.client & PP2_CLIENT_SSL && ssl.client & PP2_CLIENT_CERT_SESS && !ssl.verify){
		*have_certificate = true;
	}

	जबतक(len > 0){
		अगर(context->proxy.len - context->proxy.pos < (पूर्णांक)माप(काष्ठा pp2_tlv)){
			वापस MOSQ_ERR_INVAL;
		}
		काष्ठा pp2_tlv *tlv = (काष्ठा pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->लंबाई_h<<8) + tlv->लंबाई_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + माप(काष्ठा pp2_tlv));

		अगर(tlv_len > context->proxy.len - context->proxy.pos){
			वापस MOSQ_ERR_INVAL;
		}

		चयन(tlv->type){
			हाल PP2_SUBTYPE_SSL_VERSION:
#अगर_घोषित WITH_TLS
				mosquitto_free(context->proxy.tls_version);
				context->proxy.tls_version = mosquitto_strndup((स्थिर अक्षर *)&context->proxy.buf[context->proxy.pos], tlv_len);
#अन्यथा
				वापस MOSQ_ERR_NOT_SUPPORTED;
#पूर्ण_अगर
				अवरोध;

			हाल PP2_SUBTYPE_SSL_CIPHER:
#अगर_घोषित WITH_TLS
				mosquitto_free(context->proxy.cipher);
				context->proxy.cipher = mosquitto_strndup((स्थिर अक्षर *)&context->proxy.buf[context->proxy.pos], tlv_len);
#अन्यथा
				वापस MOSQ_ERR_NOT_SUPPORTED;
#पूर्ण_अगर
				अवरोध;

			हाल PP2_SUBTYPE_SSL_CN:
#अगर_घोषित WITH_TLS
				अगर(context->listener->use_identity_as_username){
					mosquitto_free(context->username);
					context->username = mosquitto_strndup((स्थिर अक्षर *)&context->proxy.buf[context->proxy.pos], tlv_len);
					अगर(!context->username){
						वापस MOSQ_ERR_NOMEM;
					}
				}
#अन्यथा
				वापस MOSQ_ERR_NOT_SUPPORTED;
#पूर्ण_अगर
				अवरोध;
		}
		len = (uint16_t)(len - (माप(uint8_t) + माप(uint8_t) + माप(uint8_t) + tlv_len));
		context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
	}
	context->proxy.have_tls = true;

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक read_tlv(काष्ठा mosquitto *context, bool *have_certificate)
{
	जबतक(context->proxy.pos < context->proxy.len){
		अगर(context->proxy.len - context->proxy.pos < (पूर्णांक)माप(काष्ठा pp2_tlv)){
			वापस MOSQ_ERR_INVAL;
		}
		काष्ठा pp2_tlv *tlv = (काष्ठा pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->लंबाई_h<<8) + tlv->लंबाई_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + माप(काष्ठा pp2_tlv));

		अगर(tlv_len > context->proxy.len - context->proxy.pos){
			वापस MOSQ_ERR_INVAL;
		}

		चयन(tlv->type){
			हाल PP2_TYPE_SSL:
				{
					पूर्णांक rc = read_tlv_ssl(context, tlv_len, have_certificate);
					अगर(rc){
						वापस rc;
					}
				}
				अवरोध;
			शेष:
				context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
				अवरोध;
		}
	}

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक proxy_v2__read(काष्ठा mosquitto *context)
{
	काष्ठा proxy_hdr_v2 hdr;

	अगर(context->proxy.cmd == -1){
		context->proxy.buf = शून्य;
		अगर(net__read(context, &hdr, माप(hdr)) != माप(hdr)){
			वापस MOSQ_ERR_CONN_LOST;
		}
		अगर(स_भेद(hdr.sig, signature, माप(signature))){
			वापस MOSQ_ERR_INVAL;
		}
		अगर((hdr.ver_cmd & 0xF0) != 0x20){
			वापस MOSQ_ERR_INVAL;
		}
		context->proxy.cmd = hdr.ver_cmd & 0x0F;
		अगर(context->proxy.cmd != 0x00 && context->proxy.cmd != 0x01){
			वापस MOSQ_ERR_INVAL;
		}
		context->proxy.fam = hdr.fam;
		अगर((hdr.fam != 0x00 || context->proxy.cmd != PROXY_CMD_LOCAL)
				&& hdr.fam != PROXY_TCP_IPV4
				&& hdr.fam != PROXY_TCP_IPV6
				&& hdr.fam != PROXY_TCP_UNIX){

			वापस 1;
		}
		context->proxy.pos = 0;
		context->proxy.len = ntohs(hdr.len);
		अगर(context->proxy.len > 0){
			/* PROXY_PACKET_LIMIT=500 bytes, arbitrary upper limit */
			चयन(context->proxy.fam){
				हाल PROXY_TCP_IPV4:
					अगर(context->proxy.len < 12 || context->proxy.len > PROXY_PACKET_LIMIT){
						वापस MOSQ_ERR_INVAL;
					}
					अवरोध;
				हाल PROXY_TCP_IPV6:
					अगर(context->proxy.len < 36 || context->proxy.len > PROXY_PACKET_LIMIT){
						वापस MOSQ_ERR_INVAL;
					}
					अवरोध;
				हाल PROXY_TCP_UNIX:
					अगर(context->proxy.len > PROXY_PACKET_LIMIT){
						वापस MOSQ_ERR_INVAL;
					}
					अवरोध;
			}
			context->proxy.buf = mosquitto_calloc(1, (माप_प्रकार)(context->proxy.len+1));
			अगर(!context->proxy.buf){
				वापस MOSQ_ERR_NOMEM;
			}
		}अन्यथा{
			अगर(context->proxy.cmd != PROXY_CMD_LOCAL || context->proxy.fam != 0x00){
				वापस MOSQ_ERR_PROTOCOL;
			}
		}

	}

	अगर(context->proxy.pos < context->proxy.len){
		ssize_t rc = net__read(context, context->proxy.buf, (माप_प्रकार)(context->proxy.len - context->proxy.pos));
		अगर(rc > 0){
			context->proxy.pos = (uint16_t)(context->proxy.pos + rc);
		}अन्यथा{
			proxy_cleanup(context);
			वापस MOSQ_ERR_CONN_LOST;
		}
	}
	अगर(context->proxy.pos == context->proxy.len){
		अगर(context->proxy.fam == PROXY_TCP_IPV4){
			अक्षर address[100];
			जोड़ proxy_addr *addr = (जोड़ proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET, &addr->ipv4_addr.src_addr, address, माप(address));
			context->address = mosquitto_strdup(address);
			context->remote_port = ntohs(addr->ipv4_addr.src_port);
			context->proxy.pos = 4+4+2+2;
		}अन्यथा अगर(context->proxy.fam == PROXY_TCP_IPV6){
			अक्षर address[100];
			जोड़ proxy_addr *addr = (जोड़ proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET6, addr->ipv6_addr.src_addr, address, माप(address));
			context->address = mosquitto_strdup(address);
			context->remote_port = ntohs(addr->ipv6_addr.src_port);
			context->proxy.pos = 16+16+2+2;
		}अन्यथा अगर(context->proxy.fam == PROXY_TCP_UNIX){
			जोड़ proxy_addr *addr = (जोड़ proxy_addr *)context->proxy.buf;
			context->address = mosquitto_strndup((अक्षर *)addr->unix_addr.src_addr, माप(addr->unix_addr.src_addr));
			context->remote_port = 0;
			context->proxy.pos = (uint16_t)(माप(addr->unix_addr.src_addr) + 1);
		}अन्यथा{
			/* Must be LOCAL */
			/* Ignore address */
			context->address = शून्य;
			context->remote_port = 0;
			proxy_cleanup(context);
			वापस MOSQ_ERR_PROXY;
		}
		अगर(!context->address){
			proxy_cleanup(context);
			वापस MOSQ_ERR_NOMEM;
		}

		bool have_certificate = false;

		पूर्णांक rc = read_tlv(context, &have_certificate);
		अगर(rc){
			log__printf(शून्य, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, corrupt PROXY header.",
					context->address, context->remote_port);
			proxy_cleanup(context);
			वापस MOSQ_ERR_PROXY;
		}
		mosquitto_FREE(context->proxy.buf);

		अगर(context->listener->proxy_protocol_v2_require_tls && !context->proxy.have_tls){
			log__printf(शून्य, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not connect using TLS.",
					context->address, context->remote_port);
			proxy_cleanup(context);
			वापस MOSQ_ERR_PROXY;
		}

#अगर_घोषित WITH_TLS
		अगर(context->listener->require_certificate){
			अगर(!have_certificate){
				log__printf(शून्य, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not provide a certificate.",
						context->address, context->remote_port);
				proxy_cleanup(context);
				वापस MOSQ_ERR_PROXY;
			}
		}

		अगर(context->proxy.tls_version && context->proxy.cipher){
			log__printf(शून्य, MOSQ_LOG_NOTICE, "Connection from %s:%d negotiated %s cipher %s",
					context->address, context->remote_port, context->proxy.tls_version, context->proxy.cipher);
		}
#पूर्ण_अगर
		proxy_cleanup(context);

#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
		अगर(context->listener->protocol == mp_websockets){
			वापस http__context_init(context);
		}अन्यथा
#पूर्ण_अगर
		{
			context->transport = mosq_t_tcp;
		}
	}

	वापस MOSQ_ERR_SUCCESS;
}
#पूर्ण_अगर
