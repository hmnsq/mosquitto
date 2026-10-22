#ifdef WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#অন্যথা
#  include <arpa/inet.h>
#  include <netinet/in.h>
#endif
#অন্তর্ভাৱ <stdint.h>
#অন্তর্ভাৱ "mosquitto_broker_internal.h"
#অন্তর্ভাৱ "mosquitto_internal.h"
#অন্তর্ভাৱ "net_mosq.h"

#যদি !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN

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
#define PP2_SUBTYPE_SSL_চাবি_ALG 0x25
#define PP2_TYPE_NETNS          0x30

#define PP2_CLIENT_SSL           0x01
#define PP2_CLIENT_CERT_CONN     0x02
#define PP2_CLIENT_CERT_SESS     0x04

#define PROXY_PACKET_LIMIT 500

সংরচনা proxy_hdr_v2 {
	uint8_t sig[12]; /* hex 0D 0A 0D 0A 00 0D 0A 51 55 49 54 0A */
	uint8_t ver_cmd; /* protocol version and command */
	uint8_t fam; /* protocol family and address */
	uint16_t len; /* number of following bytes part of the header */
};

সঙ্ঘ proxy_addr {
	সংরচনা { /* for TCP/UDP over IPv4, len = 12 */
		uint32_t src_addr;
		uint32_t dst_addr;
		uint16_t src_port;
		uint16_t dst_port;
	} ipv4_addr;
	সংরচনা { /* for TCP/UDP over IPv6, len = 36 */
		uint8_t src_addr[16];
		uint8_t dst_addr[16];
		uint16_t src_port;
		uint16_t dst_port;
	} ipv6_addr;
	সংরচনা { /* for AF_UNIX sockets, len = 216 */
		uint8_t src_addr[108];
		uint8_t dst_addr[108];
	} unix_addr;
};

সংরচনা pp2_tlv {
	uint8_t type;
	uint8_t length_h;
	uint8_t length_l;
};

সংরচনা pp2_tlv_ssl {
	uint8_t client;
	uint32_t verify;
};

ধ্রুৱ uint8_t signature[12] = {0x0D, 0x0A, 0x0D, 0x0A, 0x00, 0x0D, 0x0A, 0x51, 0x55, 0x49, 0x54, 0x0A};


স্থির শূন্য proxy_cleanup(সংরচনা mosquitto *context)
{
	mosquitto_FREE(context->proxy.buf);
	mosquitto_FREE(context->proxy.tls_version);
	mosquitto_FREE(context->proxy.cipher);
}


স্থির পূর্ণাঙ্ক read_tlv_ssl(সংরচনা mosquitto *context, uint16_t len, bool *have_certificate)
{
	সংরচনা pp2_tlv_ssl ssl = {0};

	যদি(len < পরিমাণ(uint8_t) + পরিমাণ(uint32_t)){
		ঘূৰাই MOSQ_ERR_INVAL;
	}

	ssl.client = context->proxy.buf[context->proxy.pos];
	ssl.verify = ntohl(*(uint32_t *)(&context->proxy.buf[context->proxy.pos + পরিমাণ(uint8_t)]));
	context->proxy.pos = (uint16_t)(context->proxy.pos + পরিমাণ(uint8_t) + পরিমাণ(uint32_t));
	len = (uint16_t)(len - (পরিমাণ(uint8_t) + পরিমাণ(uint32_t)));

	যদি(ssl.client & PP2_CLIENT_SSL && ssl.client & PP2_CLIENT_CERT_SESS && !ssl.verify){
		*have_certificate = true;
	}

	যেতিয়ালৈ(len > 0){
		যদি(context->proxy.len - context->proxy.pos < (পূর্ণাঙ্ক)পরিমাণ(সংরচনা pp2_tlv)){
			ঘূৰাই MOSQ_ERR_INVAL;
		}
		সংরচনা pp2_tlv *tlv = (সংরচনা pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->length_h<<8) + tlv->length_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + পরিমাণ(সংরচনা pp2_tlv));

		যদি(tlv_len > context->proxy.len - context->proxy.pos){
			ঘূৰাই MOSQ_ERR_INVAL;
		}

		ৱিকল্প(tlv->type){
			পক্ষ PP2_SUBTYPE_SSL_VERSION:
#ifdef WITH_TLS
				mosquitto_free(context->proxy.tls_version);
				context->proxy.tls_version = mosquitto_strndup((ধ্রুৱ ৱর্ণ *)&context->proxy.buf[context->proxy.pos], tlv_len);
#অন্যথা
				ঘূৰাই MOSQ_ERR_NOT_SUPPORTED;
#endif
				ভঙ্গ;

			পক্ষ PP2_SUBTYPE_SSL_CIPHER:
#ifdef WITH_TLS
				mosquitto_free(context->proxy.cipher);
				context->proxy.cipher = mosquitto_strndup((ধ্রুৱ ৱর্ণ *)&context->proxy.buf[context->proxy.pos], tlv_len);
#অন্যথা
				ঘূৰাই MOSQ_ERR_NOT_SUPPORTED;
#endif
				ভঙ্গ;

			পক্ষ PP2_SUBTYPE_SSL_CN:
#ifdef WITH_TLS
				যদি(context->listener->use_identity_as_username){
					mosquitto_free(context->username);
					context->username = mosquitto_strndup((ধ্রুৱ ৱর্ণ *)&context->proxy.buf[context->proxy.pos], tlv_len);
					যদি(!context->username){
						ঘূৰাই MOSQ_ERR_NOMEM;
					}
				}
#অন্যথা
				ঘূৰাই MOSQ_ERR_NOT_SUPPORTED;
#endif
				ভঙ্গ;
		}
		len = (uint16_t)(len - (পরিমাণ(uint8_t) + পরিমাণ(uint8_t) + পরিমাণ(uint8_t) + tlv_len));
		context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
	}
	context->proxy.have_tls = true;

	ঘূৰাই MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণাঙ্ক read_tlv(সংরচনা mosquitto *context, bool *have_certificate)
{
	যেতিয়ালৈ(context->proxy.pos < context->proxy.len){
		যদি(context->proxy.len - context->proxy.pos < (পূর্ণাঙ্ক)পরিমাণ(সংরচনা pp2_tlv)){
			ঘূৰাই MOSQ_ERR_INVAL;
		}
		সংরচনা pp2_tlv *tlv = (সংরচনা pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->length_h<<8) + tlv->length_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + পরিমাণ(সংরচনা pp2_tlv));

		যদি(tlv_len > context->proxy.len - context->proxy.pos){
			ঘূৰাই MOSQ_ERR_INVAL;
		}

		ৱিকল্প(tlv->type){
			পক্ষ PP2_TYPE_SSL:
				{
					পূর্ণাঙ্ক rc = read_tlv_ssl(context, tlv_len, have_certificate);
					যদি(rc){
						ঘূৰাই rc;
					}
				}
				ভঙ্গ;
			স্ৱভাৱ:
				context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
				ভঙ্গ;
		}
	}

	ঘূৰাই MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক proxy_v2__read(সংরচনা mosquitto *context)
{
	সংরচনা proxy_hdr_v2 hdr;

	যদি(context->proxy.cmd == -1){
		context->proxy.buf = রিক্ত;
		যদি(net__read(context, &hdr, পরিমাণ(hdr)) != পরিমাণ(hdr)){
			ঘূৰাই MOSQ_ERR_CONN_LOST;
		}
		যদি(memcmp(hdr.sig, signature, পরিমাণ(signature))){
			ঘূৰাই MOSQ_ERR_INVAL;
		}
		যদি((hdr.ver_cmd & 0xF0) != 0x20){
			ঘূৰাই MOSQ_ERR_INVAL;
		}
		context->proxy.cmd = hdr.ver_cmd & 0x0F;
		যদি(context->proxy.cmd != 0x00 && context->proxy.cmd != 0x01){
			ঘূৰাই MOSQ_ERR_INVAL;
		}
		context->proxy.fam = hdr.fam;
		যদি((hdr.fam != 0x00 || context->proxy.cmd != PROXY_CMD_LOCAL)
				&& hdr.fam != PROXY_TCP_IPV4
				&& hdr.fam != PROXY_TCP_IPV6
				&& hdr.fam != PROXY_TCP_UNIX){

			ঘূৰাই 1;
		}
		context->proxy.pos = 0;
		context->proxy.len = ntohs(hdr.len);
		যদি(context->proxy.len > 0){
			/* PROXY_PACKET_LIMIT=500 bytes, arbitrary upper limit */
			ৱিকল্প(context->proxy.fam){
				পক্ষ PROXY_TCP_IPV4:
					যদি(context->proxy.len < 12 || context->proxy.len > PROXY_PACKET_LIMIT){
						ঘূৰাই MOSQ_ERR_INVAL;
					}
					ভঙ্গ;
				পক্ষ PROXY_TCP_IPV6:
					যদি(context->proxy.len < 36 || context->proxy.len > PROXY_PACKET_LIMIT){
						ঘূৰাই MOSQ_ERR_INVAL;
					}
					ভঙ্গ;
				পক্ষ PROXY_TCP_UNIX:
					যদি(context->proxy.len > PROXY_PACKET_LIMIT){
						ঘূৰাই MOSQ_ERR_INVAL;
					}
					ভঙ্গ;
			}
			context->proxy.buf = mosquitto_calloc(1, (size_t)(context->proxy.len+1));
			যদি(!context->proxy.buf){
				ঘূৰাই MOSQ_ERR_NOMEM;
			}
		}অন্যথা{
			যদি(context->proxy.cmd != PROXY_CMD_LOCAL || context->proxy.fam != 0x00){
				ঘূৰাই MOSQ_ERR_PROTOCOL;
			}
		}

	}

	যদি(context->proxy.pos < context->proxy.len){
		ssize_t rc = net__read(context, context->proxy.buf, (size_t)(context->proxy.len - context->proxy.pos));
		যদি(rc > 0){
			context->proxy.pos = (uint16_t)(context->proxy.pos + rc);
		}অন্যথা{
			proxy_cleanup(context);
			ঘূৰাই MOSQ_ERR_CONN_LOST;
		}
	}
	যদি(context->proxy.pos == context->proxy.len){
		যদি(context->proxy.fam == PROXY_TCP_IPV4){
			ৱর্ণ address[100];
			সঙ্ঘ proxy_addr *addr = (সঙ্ঘ proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET, &addr->ipv4_addr.src_addr, address, পরিমাণ(address));
			context->address = mosquitto_strdup(address);
			context->remote_port = ntohs(addr->ipv4_addr.src_port);
			context->proxy.pos = 4+4+2+2;
		}অন্যথা যদি(context->proxy.fam == PROXY_TCP_IPV6){
			ৱর্ণ address[100];
			সঙ্ঘ proxy_addr *addr = (সঙ্ঘ proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET6, addr->ipv6_addr.src_addr, address, পরিমাণ(address));
			context->address = mosquitto_strdup(address);
			context->remote_port = ntohs(addr->ipv6_addr.src_port);
			context->proxy.pos = 16+16+2+2;
		}অন্যথা যদি(context->proxy.fam == PROXY_TCP_UNIX){
			সঙ্ঘ proxy_addr *addr = (সঙ্ঘ proxy_addr *)context->proxy.buf;
			context->address = mosquitto_strndup((ৱর্ণ *)addr->unix_addr.src_addr, পরিমাণ(addr->unix_addr.src_addr));
			context->remote_port = 0;
			context->proxy.pos = (uint16_t)(পরিমাণ(addr->unix_addr.src_addr) + 1);
		}অন্যথা{
			/* Must be LOCAL */
			/* Ignore address */
			context->address = রিক্ত;
			context->remote_port = 0;
			proxy_cleanup(context);
			ঘূৰাই MOSQ_ERR_PROXY;
		}
		যদি(!context->address){
			proxy_cleanup(context);
			ঘূৰাই MOSQ_ERR_NOMEM;
		}

		bool have_certificate = false;

		পূর্ণাঙ্ক rc = read_tlv(context, &have_certificate);
		যদি(rc){
			log__printf(রিক্ত, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, corrupt PROXY header.",
					context->address, context->remote_port);
			proxy_cleanup(context);
			ঘূৰাই MOSQ_ERR_PROXY;
		}
		mosquitto_FREE(context->proxy.buf);

		যদি(context->listener->proxy_protocol_v2_require_tls && !context->proxy.have_tls){
			log__printf(রিক্ত, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not connect using TLS.",
					context->address, context->remote_port);
			proxy_cleanup(context);
			ঘূৰাই MOSQ_ERR_PROXY;
		}

#ifdef WITH_TLS
		যদি(context->listener->require_certificate){
			যদি(!have_certificate){
				log__printf(রিক্ত, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not provide a certificate.",
						context->address, context->remote_port);
				proxy_cleanup(context);
				ঘূৰাই MOSQ_ERR_PROXY;
			}
		}

		যদি(context->proxy.tls_version && context->proxy.cipher){
			log__printf(রিক্ত, MOSQ_LOG_NOTICE, "Connection from %s:%d negotiated %s cipher %s",
					context->address, context->remote_port, context->proxy.tls_version, context->proxy.cipher);
		}
#endif
		proxy_cleanup(context);

#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
		যদি(context->listener->protocol == mp_websockets){
			ঘূৰাই http__context_init(context);
		}অন্যথা
#endif
		{
			context->transport = mosq_t_tcp;
		}
	}

	ঘূৰাই MOSQ_ERR_SUCCESS;
}
#endif
