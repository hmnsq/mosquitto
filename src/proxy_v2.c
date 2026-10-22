#ifdef WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#இல்லையெனில்
#  include <arpa/inet.h>
#  include <netinet/in.h>
#endif
#அந்தர்பாவ <stdint.h>
#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto_internal.h"
#அந்தர்பாவ "net_mosq.h"

#எனில் !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN

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
#define PP2_SUBTYPE_SSL_சாவி_ALG 0x25
#define PP2_TYPE_NETNS          0x30

#define PP2_CLIENT_SSL           0x01
#define PP2_CLIENT_CERT_CONN     0x02
#define PP2_CLIENT_CERT_SESS     0x04

#define PROXY_PACKET_LIMIT 500

கட்டமைப்பு proxy_hdr_v2 {
	uint8_t sig[12]; /* hex 0D 0A 0D 0A 00 0D 0A 51 55 49 54 0A */
	uint8_t ver_cmd; /* protocol version and command */
	uint8_t fam; /* protocol family and address */
	uint16_t len; /* number of following bytes part of the header */
};

ஒன்றிப்பு proxy_addr {
	கட்டமைப்பு { /* for TCP/UDP over IPv4, len = 12 */
		uint32_t src_addr;
		uint32_t dst_addr;
		uint16_t src_port;
		uint16_t dst_port;
	} ipv4_addr;
	கட்டமைப்பு { /* for TCP/UDP over IPv6, len = 36 */
		uint8_t src_addr[16];
		uint8_t dst_addr[16];
		uint16_t src_port;
		uint16_t dst_port;
	} ipv6_addr;
	கட்டமைப்பு { /* for AF_UNIX sockets, len = 216 */
		uint8_t src_addr[108];
		uint8_t dst_addr[108];
	} unix_addr;
};

கட்டமைப்பு pp2_tlv {
	uint8_t type;
	uint8_t length_h;
	uint8_t length_l;
};

கட்டமைப்பு pp2_tlv_ssl {
	uint8_t client;
	uint32_t verify;
};

மாறிலி uint8_t signature[12] = {0x0D, 0x0A, 0x0D, 0x0A, 0x00, 0x0D, 0x0A, 0x51, 0x55, 0x49, 0x54, 0x0A};


நிலையான வெறுமை proxy_cleanup(கட்டமைப்பு mosquitto *context)
{
	mosquitto_FREE(context->proxy.buf);
	mosquitto_FREE(context->proxy.tls_version);
	mosquitto_FREE(context->proxy.cipher);
}


நிலையான முழு read_tlv_ssl(கட்டமைப்பு mosquitto *context, uint16_t len, bool *have_certificate)
{
	கட்டமைப்பு pp2_tlv_ssl ssl = {0};

	எனில்(len < அளவு(uint8_t) + அளவு(uint32_t)){
		திரும்பு MOSQ_ERR_INVAL;
	}

	ssl.client = context->proxy.buf[context->proxy.pos];
	ssl.verify = ntohl(*(uint32_t *)(&context->proxy.buf[context->proxy.pos + அளவு(uint8_t)]));
	context->proxy.pos = (uint16_t)(context->proxy.pos + அளவு(uint8_t) + அளவு(uint32_t));
	len = (uint16_t)(len - (அளவு(uint8_t) + அளவு(uint32_t)));

	எனில்(ssl.client & PP2_CLIENT_SSL && ssl.client & PP2_CLIENT_CERT_SESS && !ssl.verify){
		*have_certificate = true;
	}

	போது(len > 0){
		எனில்(context->proxy.len - context->proxy.pos < (முழு)அளவு(கட்டமைப்பு pp2_tlv)){
			திரும்பு MOSQ_ERR_INVAL;
		}
		கட்டமைப்பு pp2_tlv *tlv = (கட்டமைப்பு pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->length_h<<8) + tlv->length_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + அளவு(கட்டமைப்பு pp2_tlv));

		எனில்(tlv_len > context->proxy.len - context->proxy.pos){
			திரும்பு MOSQ_ERR_INVAL;
		}

		தேர்வு(tlv->type){
			நிலை PP2_SUBTYPE_SSL_VERSION:
#ifdef WITH_TLS
				mosquitto_free(context->proxy.tls_version);
				context->proxy.tls_version = mosquitto_strndup((மாறிலி எழுத்து *)&context->proxy.buf[context->proxy.pos], tlv_len);
#இல்லையெனில்
				திரும்பு MOSQ_ERR_NOT_SUPPORTED;
#endif
				நிறுத்து;

			நிலை PP2_SUBTYPE_SSL_CIPHER:
#ifdef WITH_TLS
				mosquitto_free(context->proxy.cipher);
				context->proxy.cipher = mosquitto_strndup((மாறிலி எழுத்து *)&context->proxy.buf[context->proxy.pos], tlv_len);
#இல்லையெனில்
				திரும்பு MOSQ_ERR_NOT_SUPPORTED;
#endif
				நிறுத்து;

			நிலை PP2_SUBTYPE_SSL_CN:
#ifdef WITH_TLS
				எனில்(context->listener->use_identity_as_username){
					mosquitto_free(context->username);
					context->username = mosquitto_strndup((மாறிலி எழுத்து *)&context->proxy.buf[context->proxy.pos], tlv_len);
					எனில்(!context->username){
						திரும்பு MOSQ_ERR_NOMEM;
					}
				}
#இல்லையெனில்
				திரும்பு MOSQ_ERR_NOT_SUPPORTED;
#endif
				நிறுத்து;
		}
		len = (uint16_t)(len - (அளவு(uint8_t) + அளவு(uint8_t) + அளவு(uint8_t) + tlv_len));
		context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
	}
	context->proxy.have_tls = true;

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு read_tlv(கட்டமைப்பு mosquitto *context, bool *have_certificate)
{
	போது(context->proxy.pos < context->proxy.len){
		எனில்(context->proxy.len - context->proxy.pos < (முழு)அளவு(கட்டமைப்பு pp2_tlv)){
			திரும்பு MOSQ_ERR_INVAL;
		}
		கட்டமைப்பு pp2_tlv *tlv = (கட்டமைப்பு pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->length_h<<8) + tlv->length_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + அளவு(கட்டமைப்பு pp2_tlv));

		எனில்(tlv_len > context->proxy.len - context->proxy.pos){
			திரும்பு MOSQ_ERR_INVAL;
		}

		தேர்வு(tlv->type){
			நிலை PP2_TYPE_SSL:
				{
					முழு rc = read_tlv_ssl(context, tlv_len, have_certificate);
					எனில்(rc){
						திரும்பு rc;
					}
				}
				நிறுத்து;
			இயல்பு:
				context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
				நிறுத்து;
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு proxy_v2__read(கட்டமைப்பு mosquitto *context)
{
	கட்டமைப்பு proxy_hdr_v2 hdr;

	எனில்(context->proxy.cmd == -1){
		context->proxy.buf = வெற்று;
		எனில்(net__read(context, &hdr, அளவு(hdr)) != அளவு(hdr)){
			திரும்பு MOSQ_ERR_CONN_LOST;
		}
		எனில்(memcmp(hdr.sig, signature, அளவு(signature))){
			திரும்பு MOSQ_ERR_INVAL;
		}
		எனில்((hdr.ver_cmd & 0xF0) != 0x20){
			திரும்பு MOSQ_ERR_INVAL;
		}
		context->proxy.cmd = hdr.ver_cmd & 0x0F;
		எனில்(context->proxy.cmd != 0x00 && context->proxy.cmd != 0x01){
			திரும்பு MOSQ_ERR_INVAL;
		}
		context->proxy.fam = hdr.fam;
		எனில்((hdr.fam != 0x00 || context->proxy.cmd != PROXY_CMD_LOCAL)
				&& hdr.fam != PROXY_TCP_IPV4
				&& hdr.fam != PROXY_TCP_IPV6
				&& hdr.fam != PROXY_TCP_UNIX){

			திரும்பு 1;
		}
		context->proxy.pos = 0;
		context->proxy.len = ntohs(hdr.len);
		எனில்(context->proxy.len > 0){
			/* PROXY_PACKET_LIMIT=500 bytes, arbitrary upper limit */
			தேர்வு(context->proxy.fam){
				நிலை PROXY_TCP_IPV4:
					எனில்(context->proxy.len < 12 || context->proxy.len > PROXY_PACKET_LIMIT){
						திரும்பு MOSQ_ERR_INVAL;
					}
					நிறுத்து;
				நிலை PROXY_TCP_IPV6:
					எனில்(context->proxy.len < 36 || context->proxy.len > PROXY_PACKET_LIMIT){
						திரும்பு MOSQ_ERR_INVAL;
					}
					நிறுத்து;
				நிலை PROXY_TCP_UNIX:
					எனில்(context->proxy.len > PROXY_PACKET_LIMIT){
						திரும்பு MOSQ_ERR_INVAL;
					}
					நிறுத்து;
			}
			context->proxy.buf = mosquitto_calloc(1, (size_t)(context->proxy.len+1));
			எனில்(!context->proxy.buf){
				திரும்பு MOSQ_ERR_NOMEM;
			}
		}இல்லையெனில்{
			எனில்(context->proxy.cmd != PROXY_CMD_LOCAL || context->proxy.fam != 0x00){
				திரும்பு MOSQ_ERR_PROTOCOL;
			}
		}

	}

	எனில்(context->proxy.pos < context->proxy.len){
		ssize_t rc = net__read(context, context->proxy.buf, (size_t)(context->proxy.len - context->proxy.pos));
		எனில்(rc > 0){
			context->proxy.pos = (uint16_t)(context->proxy.pos + rc);
		}இல்லையெனில்{
			proxy_cleanup(context);
			திரும்பு MOSQ_ERR_CONN_LOST;
		}
	}
	எனில்(context->proxy.pos == context->proxy.len){
		எனில்(context->proxy.fam == PROXY_TCP_IPV4){
			எழுத்து address[100];
			ஒன்றிப்பு proxy_addr *addr = (ஒன்றிப்பு proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET, &addr->ipv4_addr.src_addr, address, அளவு(address));
			context->address = mosquitto_strdup(address);
			context->remote_port = ntohs(addr->ipv4_addr.src_port);
			context->proxy.pos = 4+4+2+2;
		}இல்லையெனில் எனில்(context->proxy.fam == PROXY_TCP_IPV6){
			எழுத்து address[100];
			ஒன்றிப்பு proxy_addr *addr = (ஒன்றிப்பு proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET6, addr->ipv6_addr.src_addr, address, அளவு(address));
			context->address = mosquitto_strdup(address);
			context->remote_port = ntohs(addr->ipv6_addr.src_port);
			context->proxy.pos = 16+16+2+2;
		}இல்லையெனில் எனில்(context->proxy.fam == PROXY_TCP_UNIX){
			ஒன்றிப்பு proxy_addr *addr = (ஒன்றிப்பு proxy_addr *)context->proxy.buf;
			context->address = mosquitto_strndup((எழுத்து *)addr->unix_addr.src_addr, அளவு(addr->unix_addr.src_addr));
			context->remote_port = 0;
			context->proxy.pos = (uint16_t)(அளவு(addr->unix_addr.src_addr) + 1);
		}இல்லையெனில்{
			/* Must be LOCAL */
			/* Ignore address */
			context->address = வெற்று;
			context->remote_port = 0;
			proxy_cleanup(context);
			திரும்பு MOSQ_ERR_PROXY;
		}
		எனில்(!context->address){
			proxy_cleanup(context);
			திரும்பு MOSQ_ERR_NOMEM;
		}

		bool have_certificate = false;

		முழு rc = read_tlv(context, &have_certificate);
		எனில்(rc){
			log__printf(வெற்று, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, corrupt PROXY header.",
					context->address, context->remote_port);
			proxy_cleanup(context);
			திரும்பு MOSQ_ERR_PROXY;
		}
		mosquitto_FREE(context->proxy.buf);

		எனில்(context->listener->proxy_protocol_v2_require_tls && !context->proxy.have_tls){
			log__printf(வெற்று, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not connect using TLS.",
					context->address, context->remote_port);
			proxy_cleanup(context);
			திரும்பு MOSQ_ERR_PROXY;
		}

#ifdef WITH_TLS
		எனில்(context->listener->require_certificate){
			எனில்(!have_certificate){
				log__printf(வெற்று, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not provide a certificate.",
						context->address, context->remote_port);
				proxy_cleanup(context);
				திரும்பு MOSQ_ERR_PROXY;
			}
		}

		எனில்(context->proxy.tls_version && context->proxy.cipher){
			log__printf(வெற்று, MOSQ_LOG_NOTICE, "Connection from %s:%d negotiated %s cipher %s",
					context->address, context->remote_port, context->proxy.tls_version, context->proxy.cipher);
		}
#endif
		proxy_cleanup(context);

#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
		எனில்(context->listener->protocol == mp_websockets){
			திரும்பு http__context_init(context);
		}இல்லையெனில்
#endif
		{
			context->transport = mosq_t_tcp;
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}
#endif
