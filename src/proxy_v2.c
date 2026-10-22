#ifdef WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#иначе
#  include <arpa/inet.h>
#  include <netinet/in.h>
#endif
#include <stdint.h>
#include "mosquitto_broker_internal.h"
#include "mosquitto_internal.h"
#include "net_mosq.h"

#ако !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN

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
#define PP2_SUBTYPE_SSL_KEY_ALG 0x25
#define PP2_TYPE_NETNS          0x30

#define PP2_CLIENT_SSL           0x01
#define PP2_CLIENT_CERT_CONN     0x02
#define PP2_CLIENT_CERT_SESS     0x04

#define PROXY_PACKET_LIMIT 500

struct proxy_hdr_v2 {
	uint8_t sig[12]; /* hex 0D 0A 0D 0A 00 0D 0A 51 55 49 54 0A */
	uint8_t ver_cmd; /* protocol version and command */
	uint8_t fam; /* protocol family and address */
	uint16_t len; /* number of following bytes part of the header */
};

union proxy_addr {
	struct { /* for TCP/UDP over IPv4, len = 12 */
		uint32_t src_addr;
		uint32_t dst_addr;
		uint16_t src_port;
		uint16_t dst_port;
	} ipv4_addr;
	struct { /* for TCP/UDP over IPv6, len = 36 */
		uint8_t src_addr[16];
		uint8_t dst_addr[16];
		uint16_t src_port;
		uint16_t dst_port;
	} ipv6_addr;
	struct { /* for AF_UNIX sockets, len = 216 */
		uint8_t src_addr[108];
		uint8_t dst_addr[108];
	} unix_addr;
};

struct pp2_tlv {
	uint8_t type;
	uint8_t length_h;
	uint8_t length_l;
};

struct pp2_tlv_ssl {
	uint8_t client;
	uint32_t verify;
};

const uint8_t signature[12] = {0x0D, 0x0A, 0x0D, 0x0A, 0x00, 0x0D, 0x0A, 0x51, 0x55, 0x49, 0x54, 0x0A};


static void proxy_cleanup(struct mosquitto *context)
{
	mosquitto_FREE(context->proxy.buf);
	mosquitto_FREE(context->proxy.tls_version);
	mosquitto_FREE(context->proxy.cipher);
}


static цяло read_tlv_ssl(struct mosquitto *context, uint16_t len, bool *have_certificate)
{
	struct pp2_tlv_ssl ssl = {0};

	ако(len < sizeof(uint8_t) + sizeof(uint32_t)){
		върни MOSQ_ERR_INVAL;
	}

	ssl.client = context->proxy.buf[context->proxy.pos];
	ssl.verify = ntohl(*(uint32_t *)(&context->proxy.buf[context->proxy.pos + sizeof(uint8_t)]));
	context->proxy.pos = (uint16_t)(context->proxy.pos + sizeof(uint8_t) + sizeof(uint32_t));
	len = (uint16_t)(len - (sizeof(uint8_t) + sizeof(uint32_t)));

	ако(ssl.client & PP2_CLIENT_SSL && ssl.client & PP2_CLIENT_CERT_SESS && !ssl.verify){
		*have_certificate = true;
	}

	докато(len > 0){
		ако(context->proxy.len - context->proxy.pos < (цяло)sizeof(struct pp2_tlv)){
			върни MOSQ_ERR_INVAL;
		}
		struct pp2_tlv *tlv = (struct pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->length_h<<8) + tlv->length_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + sizeof(struct pp2_tlv));

		ако(tlv_len > context->proxy.len - context->proxy.pos){
			върни MOSQ_ERR_INVAL;
		}

		switch(tlv->type){
			case PP2_SUBTYPE_SSL_VERSION:
#ifdef WITH_TLS
				mosquitto_free(context->proxy.tls_version);
				context->proxy.tls_version = mosquitto_strndup((const char *)&context->proxy.buf[context->proxy.pos], tlv_len);
#иначе
				върни MOSQ_ERR_NOT_SUPPORTED;
#endif
				break;

			case PP2_SUBTYPE_SSL_CIPHER:
#ifdef WITH_TLS
				mosquitto_free(context->proxy.cipher);
				context->proxy.cipher = mosquitto_strndup((const char *)&context->proxy.buf[context->proxy.pos], tlv_len);
#иначе
				върни MOSQ_ERR_NOT_SUPPORTED;
#endif
				break;

			case PP2_SUBTYPE_SSL_CN:
#ifdef WITH_TLS
				ако(context->listener->use_identity_as_username){
					mosquitto_free(context->username);
					context->username = mosquitto_strndup((const char *)&context->proxy.buf[context->proxy.pos], tlv_len);
					ако(!context->username){
						върни MOSQ_ERR_NOMEM;
					}
				}
#иначе
				върни MOSQ_ERR_NOT_SUPPORTED;
#endif
				break;
		}
		len = (uint16_t)(len - (sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t) + tlv_len));
		context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
	}
	context->proxy.have_tls = true;

	върни MOSQ_ERR_SUCCESS;
}


static цяло read_tlv(struct mosquitto *context, bool *have_certificate)
{
	докато(context->proxy.pos < context->proxy.len){
		ако(context->proxy.len - context->proxy.pos < (цяло)sizeof(struct pp2_tlv)){
			върни MOSQ_ERR_INVAL;
		}
		struct pp2_tlv *tlv = (struct pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->length_h<<8) + tlv->length_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + sizeof(struct pp2_tlv));

		ако(tlv_len > context->proxy.len - context->proxy.pos){
			върни MOSQ_ERR_INVAL;
		}

		switch(tlv->type){
			case PP2_TYPE_SSL:
				{
					цяло rc = read_tlv_ssl(context, tlv_len, have_certificate);
					ако(rc){
						върни rc;
					}
				}
				break;
			default:
				context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
				break;
		}
	}

	върни MOSQ_ERR_SUCCESS;
}


цяло proxy_v2__read(struct mosquitto *context)
{
	struct proxy_hdr_v2 hdr;

	ако(context->proxy.cmd == -1){
		context->proxy.buf = NULL;
		ако(net__read(context, &hdr, sizeof(hdr)) != sizeof(hdr)){
			върни MOSQ_ERR_CONN_LOST;
		}
		ако(memcmp(hdr.sig, signature, sizeof(signature))){
			върни MOSQ_ERR_INVAL;
		}
		ако((hdr.ver_cmd & 0xF0) != 0x20){
			върни MOSQ_ERR_INVAL;
		}
		context->proxy.cmd = hdr.ver_cmd & 0x0F;
		ако(context->proxy.cmd != 0x00 && context->proxy.cmd != 0x01){
			върни MOSQ_ERR_INVAL;
		}
		context->proxy.fam = hdr.fam;
		ако((hdr.fam != 0x00 || context->proxy.cmd != PROXY_CMD_LOCAL)
				&& hdr.fam != PROXY_TCP_IPV4
				&& hdr.fam != PROXY_TCP_IPV6
				&& hdr.fam != PROXY_TCP_UNIX){

			върни 1;
		}
		context->proxy.pos = 0;
		context->proxy.len = ntohs(hdr.len);
		ако(context->proxy.len > 0){
			/* PROXY_PACKET_LIMIT=500 bytes, arbitrary upper limit */
			switch(context->proxy.fam){
				case PROXY_TCP_IPV4:
					ако(context->proxy.len < 12 || context->proxy.len > PROXY_PACKET_LIMIT){
						върни MOSQ_ERR_INVAL;
					}
					break;
				case PROXY_TCP_IPV6:
					ако(context->proxy.len < 36 || context->proxy.len > PROXY_PACKET_LIMIT){
						върни MOSQ_ERR_INVAL;
					}
					break;
				case PROXY_TCP_UNIX:
					ако(context->proxy.len > PROXY_PACKET_LIMIT){
						върни MOSQ_ERR_INVAL;
					}
					break;
			}
			context->proxy.buf = mosquitto_calloc(1, (size_t)(context->proxy.len+1));
			ако(!context->proxy.buf){
				върни MOSQ_ERR_NOMEM;
			}
		}иначе{
			ако(context->proxy.cmd != PROXY_CMD_LOCAL || context->proxy.fam != 0x00){
				върни MOSQ_ERR_PROTOCOL;
			}
		}

	}

	ако(context->proxy.pos < context->proxy.len){
		ssize_t rc = net__read(context, context->proxy.buf, (size_t)(context->proxy.len - context->proxy.pos));
		ако(rc > 0){
			context->proxy.pos = (uint16_t)(context->proxy.pos + rc);
		}иначе{
			proxy_cleanup(context);
			върни MOSQ_ERR_CONN_LOST;
		}
	}
	ако(context->proxy.pos == context->proxy.len){
		ако(context->proxy.fam == PROXY_TCP_IPV4){
			char address[100];
			union proxy_addr *addr = (union proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET, &addr->ipv4_addr.src_addr, address, sizeof(address));
			context->address = mosquitto_strdup(address);
			context->remote_port = ntohs(addr->ipv4_addr.src_port);
			context->proxy.pos = 4+4+2+2;
		}иначе ако(context->proxy.fam == PROXY_TCP_IPV6){
			char address[100];
			union proxy_addr *addr = (union proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET6, addr->ipv6_addr.src_addr, address, sizeof(address));
			context->address = mosquitto_strdup(address);
			context->remote_port = ntohs(addr->ipv6_addr.src_port);
			context->proxy.pos = 16+16+2+2;
		}иначе ако(context->proxy.fam == PROXY_TCP_UNIX){
			union proxy_addr *addr = (union proxy_addr *)context->proxy.buf;
			context->address = mosquitto_strndup((char *)addr->unix_addr.src_addr, sizeof(addr->unix_addr.src_addr));
			context->remote_port = 0;
			context->proxy.pos = (uint16_t)(sizeof(addr->unix_addr.src_addr) + 1);
		}иначе{
			/* Must be LOCAL */
			/* Ignore address */
			context->address = NULL;
			context->remote_port = 0;
			proxy_cleanup(context);
			върни MOSQ_ERR_PROXY;
		}
		ако(!context->address){
			proxy_cleanup(context);
			върни MOSQ_ERR_NOMEM;
		}

		bool have_certificate = false;

		цяло rc = read_tlv(context, &have_certificate);
		ако(rc){
			log__printf(NULL, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, corrupt PROXY header.",
					context->address, context->remote_port);
			proxy_cleanup(context);
			върни MOSQ_ERR_PROXY;
		}
		mosquitto_FREE(context->proxy.buf);

		ако(context->listener->proxy_protocol_v2_require_tls && !context->proxy.have_tls){
			log__printf(NULL, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not connect using TLS.",
					context->address, context->remote_port);
			proxy_cleanup(context);
			върни MOSQ_ERR_PROXY;
		}

#ifdef WITH_TLS
		ако(context->listener->require_certificate){
			ако(!have_certificate){
				log__printf(NULL, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not provide a certificate.",
						context->address, context->remote_port);
				proxy_cleanup(context);
				върни MOSQ_ERR_PROXY;
			}
		}

		ако(context->proxy.tls_version && context->proxy.cipher){
			log__printf(NULL, MOSQ_LOG_NOTICE, "Connection from %s:%d negotiated %s cipher %s",
					context->address, context->remote_port, context->proxy.tls_version, context->proxy.cipher);
		}
#endif
		proxy_cleanup(context);

#ако defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
		ако(context->listener->protocol == mp_websockets){
			върни http__context_init(context);
		}иначе
#endif
		{
			context->transport = mosq_t_tcp;
		}
	}

	върни MOSQ_ERR_SUCCESS;
}
#endif
