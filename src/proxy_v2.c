#ifdef WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#অন্যথা
#  include <arpa/inet.h>
#  include <netinet/in.h>
#endif
#অন্তর্ভাব <stdint.h>
#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto_internal.h"
#অন্তর্ভাব "net_mosq.h"

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

কাঠামো proxy_hdr_v2 {
	uint8_t sig[12]; /* hex 0D 0A 0D 0A 00 0D 0A 51 55 49 54 0A */
	uint8_t ver_cmd; /* protocol version and command */
	uint8_t fam; /* protocol family and address */
	uint16_t len; /* number of following bytes part of the header */
};

সংঘ proxy_addr {
	কাঠামো { /* for TCP/UDP over IPv4, len = 12 */
		uint32_t src_addr;
		uint32_t dst_addr;
		uint16_t src_port;
		uint16_t dst_port;
	} ipv4_addr;
	কাঠামো { /* for TCP/UDP over IPv6, len = 36 */
		uint8_t src_addr[16];
		uint8_t dst_addr[16];
		uint16_t src_port;
		uint16_t dst_port;
	} ipv6_addr;
	কাঠামো { /* for AF_UNIX sockets, len = 216 */
		uint8_t src_addr[108];
		uint8_t dst_addr[108];
	} unix_addr;
};

কাঠামো pp2_tlv {
	uint8_t type;
	uint8_t দৈর্ঘ্য_h;
	uint8_t দৈর্ঘ্য_l;
};

কাঠামো pp2_tlv_ssl {
	uint8_t client;
	uint32_t verify;
};

ধ্রুবক uint8_t signature[12] = {0x0D, 0x0A, 0x0D, 0x0A, 0x00, 0x0D, 0x0A, 0x51, 0x55, 0x49, 0x54, 0x0A};


স্থির শূন্য proxy_cleanup(কাঠামো mosquitto *context)
{
	mosquitto_FREE(context->proxy.buf);
	mosquitto_FREE(context->proxy.tls_version);
	mosquitto_FREE(context->proxy.cipher);
}


স্থির পূর্ণসংখ্যা read_tlv_ssl(কাঠামো mosquitto *context, uint16_t len, bool *have_certificate)
{
	কাঠামো pp2_tlv_ssl ssl = {0};

	যদি(len < আকার(uint8_t) + আকার(uint32_t)){
		ফেরত MOSQ_ERR_INVAL;
	}

	ssl.client = context->proxy.buf[context->proxy.pos];
	ssl.verify = ntohl(*(uint32_t *)(&context->proxy.buf[context->proxy.pos + আকার(uint8_t)]));
	context->proxy.pos = (uint16_t)(context->proxy.pos + আকার(uint8_t) + আকার(uint32_t));
	len = (uint16_t)(len - (আকার(uint8_t) + আকার(uint32_t)));

	যদি(ssl.client & PP2_CLIENT_SSL && ssl.client & PP2_CLIENT_CERT_SESS && !ssl.verify){
		*have_certificate = true;
	}

	যখন(len > 0){
		যদি(context->proxy.len - context->proxy.pos < (পূর্ণসংখ্যা)আকার(কাঠামো pp2_tlv)){
			ফেরত MOSQ_ERR_INVAL;
		}
		কাঠামো pp2_tlv *tlv = (কাঠামো pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->দৈর্ঘ্য_h<<8) + tlv->দৈর্ঘ্য_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + আকার(কাঠামো pp2_tlv));

		যদি(tlv_len > context->proxy.len - context->proxy.pos){
			ফেরত MOSQ_ERR_INVAL;
		}

		নির্বাচন(tlv->type){
			ক্ষেত্র PP2_SUBTYPE_SSL_VERSION:
#ifdef WITH_TLS
				mosquitto_free(context->proxy.tls_version);
				context->proxy.tls_version = mosquitto_strndup((ধ্রুবক অক্ষর *)&context->proxy.buf[context->proxy.pos], tlv_len);
#অন্যথা
				ফেরত MOSQ_ERR_NOT_SUPPORTED;
#endif
				বিরতি;

			ক্ষেত্র PP2_SUBTYPE_SSL_CIPHER:
#ifdef WITH_TLS
				mosquitto_free(context->proxy.cipher);
				context->proxy.cipher = mosquitto_strndup((ধ্রুবক অক্ষর *)&context->proxy.buf[context->proxy.pos], tlv_len);
#অন্যথা
				ফেরত MOSQ_ERR_NOT_SUPPORTED;
#endif
				বিরতি;

			ক্ষেত্র PP2_SUBTYPE_SSL_CN:
#ifdef WITH_TLS
				যদি(context->listener->use_identity_as_username){
					mosquitto_free(context->username);
					context->username = mosquitto_strndup((ধ্রুবক অক্ষর *)&context->proxy.buf[context->proxy.pos], tlv_len);
					যদি(!context->username){
						ফেরত MOSQ_ERR_NOMEM;
					}
				}
#অন্যথা
				ফেরত MOSQ_ERR_NOT_SUPPORTED;
#endif
				বিরতি;
		}
		len = (uint16_t)(len - (আকার(uint8_t) + আকার(uint8_t) + আকার(uint8_t) + tlv_len));
		context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
	}
	context->proxy.have_tls = true;

	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা read_tlv(কাঠামো mosquitto *context, bool *have_certificate)
{
	যখন(context->proxy.pos < context->proxy.len){
		যদি(context->proxy.len - context->proxy.pos < (পূর্ণসংখ্যা)আকার(কাঠামো pp2_tlv)){
			ফেরত MOSQ_ERR_INVAL;
		}
		কাঠামো pp2_tlv *tlv = (কাঠামো pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->দৈর্ঘ্য_h<<8) + tlv->দৈর্ঘ্য_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + আকার(কাঠামো pp2_tlv));

		যদি(tlv_len > context->proxy.len - context->proxy.pos){
			ফেরত MOSQ_ERR_INVAL;
		}

		নির্বাচন(tlv->type){
			ক্ষেত্র PP2_TYPE_SSL:
				{
					পূর্ণসংখ্যা rc = read_tlv_ssl(context, tlv_len, have_certificate);
					যদি(rc){
						ফেরত rc;
					}
				}
				বিরতি;
			পূর্বনির্ধারিত:
				context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
				বিরতি;
		}
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা proxy_v2__read(কাঠামো mosquitto *context)
{
	কাঠামো proxy_hdr_v2 hdr;

	যদি(context->proxy.cmd == -1){
		context->proxy.buf = ফাঁকা;
		যদি(net__read(context, &hdr, আকার(hdr)) != আকার(hdr)){
			ফেরত MOSQ_ERR_CONN_LOST;
		}
		যদি(memcmp(hdr.sig, signature, আকার(signature))){
			ফেরত MOSQ_ERR_INVAL;
		}
		যদি((hdr.ver_cmd & 0xF0) != 0x20){
			ফেরত MOSQ_ERR_INVAL;
		}
		context->proxy.cmd = hdr.ver_cmd & 0x0F;
		যদি(context->proxy.cmd != 0x00 && context->proxy.cmd != 0x01){
			ফেরত MOSQ_ERR_INVAL;
		}
		context->proxy.fam = hdr.fam;
		যদি((hdr.fam != 0x00 || context->proxy.cmd != PROXY_CMD_LOCAL)
				&& hdr.fam != PROXY_TCP_IPV4
				&& hdr.fam != PROXY_TCP_IPV6
				&& hdr.fam != PROXY_TCP_UNIX){

			ফেরত 1;
		}
		context->proxy.pos = 0;
		context->proxy.len = ntohs(hdr.len);
		যদি(context->proxy.len > 0){
			/* PROXY_PACKET_LIMIT=500 bytes, arbitrary upper limit */
			নির্বাচন(context->proxy.fam){
				ক্ষেত্র PROXY_TCP_IPV4:
					যদি(context->proxy.len < 12 || context->proxy.len > PROXY_PACKET_LIMIT){
						ফেরত MOSQ_ERR_INVAL;
					}
					বিরতি;
				ক্ষেত্র PROXY_TCP_IPV6:
					যদি(context->proxy.len < 36 || context->proxy.len > PROXY_PACKET_LIMIT){
						ফেরত MOSQ_ERR_INVAL;
					}
					বিরতি;
				ক্ষেত্র PROXY_TCP_UNIX:
					যদি(context->proxy.len > PROXY_PACKET_LIMIT){
						ফেরত MOSQ_ERR_INVAL;
					}
					বিরতি;
			}
			context->proxy.buf = mosquitto_calloc(1, (size_t)(context->proxy.len+1));
			যদি(!context->proxy.buf){
				ফেরত MOSQ_ERR_NOMEM;
			}
		}অন্যথা{
			যদি(context->proxy.cmd != PROXY_CMD_LOCAL || context->proxy.fam != 0x00){
				ফেরত MOSQ_ERR_PROTOCOL;
			}
		}

	}

	যদি(context->proxy.pos < context->proxy.len){
		ssize_t rc = net__read(context, context->proxy.buf, (size_t)(context->proxy.len - context->proxy.pos));
		যদি(rc > 0){
			context->proxy.pos = (uint16_t)(context->proxy.pos + rc);
		}অন্যথা{
			proxy_cleanup(context);
			ফেরত MOSQ_ERR_CONN_LOST;
		}
	}
	যদি(context->proxy.pos == context->proxy.len){
		যদি(context->proxy.fam == PROXY_TCP_IPV4){
			অক্ষর address[100];
			সংঘ proxy_addr *addr = (সংঘ proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET, &addr->ipv4_addr.src_addr, address, আকার(address));
			context->address = mosquitto_strdup(address);
			context->remote_port = ntohs(addr->ipv4_addr.src_port);
			context->proxy.pos = 4+4+2+2;
		}অন্যথা যদি(context->proxy.fam == PROXY_TCP_IPV6){
			অক্ষর address[100];
			সংঘ proxy_addr *addr = (সংঘ proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET6, addr->ipv6_addr.src_addr, address, আকার(address));
			context->address = mosquitto_strdup(address);
			context->remote_port = ntohs(addr->ipv6_addr.src_port);
			context->proxy.pos = 16+16+2+2;
		}অন্যথা যদি(context->proxy.fam == PROXY_TCP_UNIX){
			সংঘ proxy_addr *addr = (সংঘ proxy_addr *)context->proxy.buf;
			context->address = mosquitto_strndup((অক্ষর *)addr->unix_addr.src_addr, আকার(addr->unix_addr.src_addr));
			context->remote_port = 0;
			context->proxy.pos = (uint16_t)(আকার(addr->unix_addr.src_addr) + 1);
		}অন্যথা{
			/* Must be LOCAL */
			/* Ignore address */
			context->address = ফাঁকা;
			context->remote_port = 0;
			proxy_cleanup(context);
			ফেরত MOSQ_ERR_PROXY;
		}
		যদি(!context->address){
			proxy_cleanup(context);
			ফেরত MOSQ_ERR_NOMEM;
		}

		bool have_certificate = false;

		পূর্ণসংখ্যা rc = read_tlv(context, &have_certificate);
		যদি(rc){
			log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, corrupt PROXY header.",
					context->address, context->remote_port);
			proxy_cleanup(context);
			ফেরত MOSQ_ERR_PROXY;
		}
		mosquitto_FREE(context->proxy.buf);

		যদি(context->listener->proxy_protocol_v2_require_tls && !context->proxy.have_tls){
			log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not connect using TLS.",
					context->address, context->remote_port);
			proxy_cleanup(context);
			ফেরত MOSQ_ERR_PROXY;
		}

#ifdef WITH_TLS
		যদি(context->listener->require_certificate){
			যদি(!have_certificate){
				log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not provide a certificate.",
						context->address, context->remote_port);
				proxy_cleanup(context);
				ফেরত MOSQ_ERR_PROXY;
			}
		}

		যদি(context->proxy.tls_version && context->proxy.cipher){
			log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Connection from %s:%d negotiated %s cipher %s",
					context->address, context->remote_port, context->proxy.tls_version, context->proxy.cipher);
		}
#endif
		proxy_cleanup(context);

#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
		যদি(context->listener->protocol == mp_websockets){
			ফেরত http__context_init(context);
		}অন্যথা
#endif
		{
			context->transport = mosq_t_tcp;
		}
	}

	ফেরত MOSQ_ERR_SUCCESS;
}
#endif
