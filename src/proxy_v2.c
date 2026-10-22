#ifdef WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#अन्यथा
#  include <arpa/inet.h>
#  include <netinet/in.h>
#endif
#अन्तर्भाव <stdint.h>
#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto_internal.h"
#अन्तर्भाव "net_mosq.h"

#जर !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN

#define PROXY_CMD_LOCAL 0x00
#define PROXY_CMD_PROXY 0x01

#define PROXY_TCP_IPV4 0x11
#define PROXY_TCP_IPV6 0x21
#define PROXY_TCP_UNIX 0x31

#define PP2_TYPE_ALPN           0x01
#define PP2_TYPE_AUTHORITY      0x02
#define PP2_TYPE_CRC32C         0x03
#define PP2_TYPE_NOOP           0x04
#define PP2_TYPE_UNIQUE_ID      0x05
#define PP2_TYPE_SSL            0x20
#define PP2_SUBTYPE_SSL_VERSION 0x21
#define PP2_SUBTYPE_SSL_CN      0x22
#define PP2_SUBTYPE_SSL_CIPHER  0x23
#define PP2_SUBTYPE_SSL_SIG_ALG 0x24
#define PP2_SUBTYPE_SSL_किल्ली_ALG 0x25
#define PP2_TYPE_NETNS          0x30

#define PP2_CLIENT_SSL           0x01
#define PP2_CLIENT_CERT_CONN     0x02
#define PP2_CLIENT_CERT_SESS     0x04

#define PROXY_PACKET_LIMIT 500

संरचना proxy_hdr_v2 {
	uint8_t sig[12]; /* hex 0D 0A 0D 0A 00 0D 0A 51 55 49 54 0A */
	uint8_t ver_cmd; /* protocol version and command */
	uint8_t fam; /* protocol family and address */
	uint16_t len; /* number of following bytes part of the header */
};

संघ proxy_addr {
	संरचना { /* for TCP/UDP over IPv4, len = 12 */
		uint32_t src_addr;
		uint32_t dst_addr;
		uint16_t src_port;
		uint16_t dst_port;
	} ipv4_addr;
	संरचना { /* for TCP/UDP over IPv6, len = 36 */
		uint8_t src_addr[16];
		uint8_t dst_addr[16];
		uint16_t src_port;
		uint16_t dst_port;
	} ipv6_addr;
	संरचना { /* for AF_UNIX sockets, len = 216 */
		uint8_t src_addr[108];
		uint8_t dst_addr[108];
	} unix_addr;
};

संरचना pp2_tlv {
	uint8_t type;
	uint8_t length_h;
	uint8_t length_l;
};

संरचना pp2_tlv_ssl {
	uint8_t client;
	uint32_t verify;
};

स्थिरांक uint8_t signature[12] = {0x0D, 0x0A, 0x0D, 0x0A, 0x00, 0x0D, 0x0A, 0x51, 0x55, 0x49, 0x54, 0x0A};


स्थिर शून्य proxy_cleanup(संरचना mosquitto *context)
{
	mosquitto_FREE(context->proxy.buf);
	mosquitto_FREE(context->proxy.tls_version);
	mosquitto_FREE(context->proxy.cipher);
}


स्थिर पूर्णांक read_tlv_ssl(संरचना mosquitto *context, uint16_t len, bool *have_certificate)
{
	संरचना pp2_tlv_ssl ssl = {0};

	जर(len < आकार(uint8_t) + आकार(uint32_t)){
		परत MOSQ_ERR_INVAL;
	}

	ssl.client = context->proxy.buf[context->proxy.pos];
	ssl.verify = ntohl(*(uint32_t *)(&context->proxy.buf[context->proxy.pos + आकार(uint8_t)]));
	context->proxy.pos = (uint16_t)(context->proxy.pos + आकार(uint8_t) + आकार(uint32_t));
	len = (uint16_t)(len - (आकार(uint8_t) + आकार(uint32_t)));

	जर(ssl.client & PP2_CLIENT_SSL && ssl.client & PP2_CLIENT_CERT_SESS && !ssl.verify){
		*have_certificate = true;
	}

	पर्यंत(len > 0){
		जर(context->proxy.len - context->proxy.pos < (पूर्णांक)आकार(संरचना pp2_tlv)){
			परत MOSQ_ERR_INVAL;
		}
		संरचना pp2_tlv *tlv = (संरचना pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->length_h<<8) + tlv->length_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + आकार(संरचना pp2_tlv));

		जर(tlv_len > context->proxy.len - context->proxy.pos){
			परत MOSQ_ERR_INVAL;
		}

		निवड(tlv->type){
			स्थिती PP2_SUBTYPE_SSL_VERSION:
#ifdef WITH_TLS
				mosquitto_free(context->proxy.tls_version);
				context->proxy.tls_version = mosquitto_strndup((स्थिरांक अक्षर *)&context->proxy.buf[context->proxy.pos], tlv_len);
#अन्यथा
				परत MOSQ_ERR_NOT_SUPPORTED;
#endif
				थांबा;

			स्थिती PP2_SUBTYPE_SSL_CIPHER:
#ifdef WITH_TLS
				mosquitto_free(context->proxy.cipher);
				context->proxy.cipher = mosquitto_strndup((स्थिरांक अक्षर *)&context->proxy.buf[context->proxy.pos], tlv_len);
#अन्यथा
				परत MOSQ_ERR_NOT_SUPPORTED;
#endif
				थांबा;

			स्थिती PP2_SUBTYPE_SSL_CN:
#ifdef WITH_TLS
				जर(context->listener->use_identity_as_username){
					mosquitto_free(context->username);
					context->username = mosquitto_strndup((स्थिरांक अक्षर *)&context->proxy.buf[context->proxy.pos], tlv_len);
					जर(!context->username){
						परत MOSQ_ERR_NOMEM;
					}
				}
#अन्यथा
				परत MOSQ_ERR_NOT_SUPPORTED;
#endif
				थांबा;
		}
		len = (uint16_t)(len - (आकार(uint8_t) + आकार(uint8_t) + आकार(uint8_t) + tlv_len));
		context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
	}
	context->proxy.have_tls = true;

	परत MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णांक read_tlv(संरचना mosquitto *context, bool *have_certificate)
{
	पर्यंत(context->proxy.pos < context->proxy.len){
		जर(context->proxy.len - context->proxy.pos < (पूर्णांक)आकार(संरचना pp2_tlv)){
			परत MOSQ_ERR_INVAL;
		}
		संरचना pp2_tlv *tlv = (संरचना pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->length_h<<8) + tlv->length_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + आकार(संरचना pp2_tlv));

		जर(tlv_len > context->proxy.len - context->proxy.pos){
			परत MOSQ_ERR_INVAL;
		}

		निवड(tlv->type){
			स्थिती PP2_TYPE_SSL:
				{
					पूर्णांक rc = read_tlv_ssl(context, tlv_len, have_certificate);
					जर(rc){
						परत rc;
					}
				}
				थांबा;
			पूर्वनिर्धारित:
				context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
				थांबा;
		}
	}

	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक proxy_v2__read(संरचना mosquitto *context)
{
	संरचना proxy_hdr_v2 hdr;

	जर(context->proxy.cmd == -1){
		context->proxy.buf = रिक्त;
		जर(net__read(context, &hdr, आकार(hdr)) != आकार(hdr)){
			परत MOSQ_ERR_CONN_LOST;
		}
		जर(स_भेद(hdr.sig, signature, आकार(signature))){
			परत MOSQ_ERR_INVAL;
		}
		जर((hdr.ver_cmd & 0xF0) != 0x20){
			परत MOSQ_ERR_INVAL;
		}
		context->proxy.cmd = hdr.ver_cmd & 0x0F;
		जर(context->proxy.cmd != 0x00 && context->proxy.cmd != 0x01){
			परत MOSQ_ERR_INVAL;
		}
		context->proxy.fam = hdr.fam;
		जर((hdr.fam != 0x00 || context->proxy.cmd != PROXY_CMD_LOCAL)
				&& hdr.fam != PROXY_TCP_IPV4
				&& hdr.fam != PROXY_TCP_IPV6
				&& hdr.fam != PROXY_TCP_UNIX){

			परत 1;
		}
		context->proxy.pos = 0;
		context->proxy.len = ntohs(hdr.len);
		जर(context->proxy.len > 0){
			/* PROXY_PACKET_LIMIT=500 bytes, arbitrary upper limit */
			निवड(context->proxy.fam){
				स्थिती PROXY_TCP_IPV4:
					जर(context->proxy.len < 12 || context->proxy.len > PROXY_PACKET_LIMIT){
						परत MOSQ_ERR_INVAL;
					}
					थांबा;
				स्थिती PROXY_TCP_IPV6:
					जर(context->proxy.len < 36 || context->proxy.len > PROXY_PACKET_LIMIT){
						परत MOSQ_ERR_INVAL;
					}
					थांबा;
				स्थिती PROXY_TCP_UNIX:
					जर(context->proxy.len > PROXY_PACKET_LIMIT){
						परत MOSQ_ERR_INVAL;
					}
					थांबा;
			}
			context->proxy.buf = mosquitto_calloc(1, (माप_प्रकार)(context->proxy.len+1));
			जर(!context->proxy.buf){
				परत MOSQ_ERR_NOMEM;
			}
		}अन्यथा{
			जर(context->proxy.cmd != PROXY_CMD_LOCAL || context->proxy.fam != 0x00){
				परत MOSQ_ERR_PROTOCOL;
			}
		}

	}

	जर(context->proxy.pos < context->proxy.len){
		ssize_t rc = net__read(context, context->proxy.buf, (माप_प्रकार)(context->proxy.len - context->proxy.pos));
		जर(rc > 0){
			context->proxy.pos = (uint16_t)(context->proxy.pos + rc);
		}अन्यथा{
			proxy_cleanup(context);
			परत MOSQ_ERR_CONN_LOST;
		}
	}
	जर(context->proxy.pos == context->proxy.len){
		जर(context->proxy.fam == PROXY_TCP_IPV4){
			अक्षर address[100];
			संघ proxy_addr *addr = (संघ proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET, &addr->ipv4_addr.src_addr, address, आकार(address));
			context->address = mosquitto_strdup(address);
			context->remote_port = ntohs(addr->ipv4_addr.src_port);
			context->proxy.pos = 4+4+2+2;
		}अन्यथा जर(context->proxy.fam == PROXY_TCP_IPV6){
			अक्षर address[100];
			संघ proxy_addr *addr = (संघ proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET6, addr->ipv6_addr.src_addr, address, आकार(address));
			context->address = mosquitto_strdup(address);
			context->remote_port = ntohs(addr->ipv6_addr.src_port);
			context->proxy.pos = 16+16+2+2;
		}अन्यथा जर(context->proxy.fam == PROXY_TCP_UNIX){
			संघ proxy_addr *addr = (संघ proxy_addr *)context->proxy.buf;
			context->address = mosquitto_strndup((अक्षर *)addr->unix_addr.src_addr, आकार(addr->unix_addr.src_addr));
			context->remote_port = 0;
			context->proxy.pos = (uint16_t)(आकार(addr->unix_addr.src_addr) + 1);
		}अन्यथा{
			/* Must be LOCAL */
			/* Ignore address */
			context->address = रिक्त;
			context->remote_port = 0;
			proxy_cleanup(context);
			परत MOSQ_ERR_PROXY;
		}
		जर(!context->address){
			proxy_cleanup(context);
			परत MOSQ_ERR_NOMEM;
		}

		bool have_certificate = false;

		पूर्णांक rc = read_tlv(context, &have_certificate);
		जर(rc){
			log__printf(रिक्त, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, corrupt PROXY header.",
					context->address, context->remote_port);
			proxy_cleanup(context);
			परत MOSQ_ERR_PROXY;
		}
		mosquitto_FREE(context->proxy.buf);

		जर(context->listener->proxy_protocol_v2_require_tls && !context->proxy.have_tls){
			log__printf(रिक्त, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not connect using TLS.",
					context->address, context->remote_port);
			proxy_cleanup(context);
			परत MOSQ_ERR_PROXY;
		}

#ifdef WITH_TLS
		जर(context->listener->require_certificate){
			जर(!have_certificate){
				log__printf(रिक्त, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not provide a certificate.",
						context->address, context->remote_port);
				proxy_cleanup(context);
				परत MOSQ_ERR_PROXY;
			}
		}

		जर(context->proxy.tls_version && context->proxy.cipher){
			log__printf(रिक्त, MOSQ_LOG_NOTICE, "Connection from %s:%d negotiated %s cipher %s",
					context->address, context->remote_port, context->proxy.tls_version, context->proxy.cipher);
		}
#endif
		proxy_cleanup(context);

#जर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
		जर(context->listener->protocol == mp_websockets){
			परत http__context_init(context);
		}अन्यथा
#endif
		{
			context->transport = mosq_t_tcp;
		}
	}

	परत MOSQ_ERR_SUCCESS;
}
#endif
