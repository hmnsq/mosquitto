#ifdef WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#ଅନ୍ୟଥା
#  include <arpa/inet.h>
#  include <netinet/in.h>
#endif
#ଅନ୍ତର୍ଭାଵ <stdint.h>
#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto_internal.h"
#ଅନ୍ତର୍ଭାଵ "net_mosq.h"

#ଯଦି !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN

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
#define PP2_SUBTYPE_SSL_ଚାବି_ALG 0x25
#define PP2_TYPE_NETNS          0x30

#define PP2_CLIENT_SSL           0x01
#define PP2_CLIENT_CERT_CONN     0x02
#define PP2_CLIENT_CERT_SESS     0x04

#define PROXY_PACKET_LIMIT 500

ସଂରଚନା proxy_hdr_v2 {
	uint8_t sig[12]; /* hex 0D 0A 0D 0A 00 0D 0A 51 55 49 54 0A */
	uint8_t ver_cmd; /* protocol version and command */
	uint8_t fam; /* protocol family and address */
	uint16_t len; /* number of following bytes part of the header */
};

ସଂଘ proxy_addr {
	ସଂରଚନା { /* for TCP/UDP over IPv4, len = 12 */
		uint32_t src_addr;
		uint32_t dst_addr;
		uint16_t src_port;
		uint16_t dst_port;
	} ipv4_addr;
	ସଂରଚନା { /* for TCP/UDP over IPv6, len = 36 */
		uint8_t src_addr[16];
		uint8_t dst_addr[16];
		uint16_t src_port;
		uint16_t dst_port;
	} ipv6_addr;
	ସଂରଚନା { /* for AF_UNIX sockets, len = 216 */
		uint8_t src_addr[108];
		uint8_t dst_addr[108];
	} unix_addr;
};

ସଂରଚନା pp2_tlv {
	uint8_t type;
	uint8_t length_h;
	uint8_t length_l;
};

ସଂରଚନା pp2_tlv_ssl {
	uint8_t client;
	uint32_t verify;
};

ଅଚଳ uint8_t signature[12] = {0x0D, 0x0A, 0x0D, 0x0A, 0x00, 0x0D, 0x0A, 0x51, 0x55, 0x49, 0x54, 0x0A};


ସ୍ଥିର ଶୂନ୍ୟ proxy_cleanup(ସଂରଚନା mosquitto *context)
{
	mosquitto_FREE(context->proxy.buf);
	mosquitto_FREE(context->proxy.tls_version);
	mosquitto_FREE(context->proxy.cipher);
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_tlv_ssl(ସଂରଚନା mosquitto *context, uint16_t len, bool *have_certificate)
{
	ସଂରଚନା pp2_tlv_ssl ssl = {0};

	ଯଦି(len < ଆକାର(uint8_t) + ଆକାର(uint32_t)){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	ssl.client = context->proxy.buf[context->proxy.pos];
	ssl.verify = ntohl(*(uint32_t *)(&context->proxy.buf[context->proxy.pos + ଆକାର(uint8_t)]));
	context->proxy.pos = (uint16_t)(context->proxy.pos + ଆକାର(uint8_t) + ଆକାର(uint32_t));
	len = (uint16_t)(len - (ଆକାର(uint8_t) + ଆକାର(uint32_t)));

	ଯଦି(ssl.client & PP2_CLIENT_SSL && ssl.client & PP2_CLIENT_CERT_SESS && !ssl.verify){
		*have_certificate = true;
	}

	ଯେତେବେଳେ(len > 0){
		ଯଦି(context->proxy.len - context->proxy.pos < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଆକାର(ସଂରଚନା pp2_tlv)){
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
		ସଂରଚନା pp2_tlv *tlv = (ସଂରଚନା pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->length_h<<8) + tlv->length_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + ଆକାର(ସଂରଚନା pp2_tlv));

		ଯଦି(tlv_len > context->proxy.len - context->proxy.pos){
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}

		ବାଛ(tlv->type){
			ସ୍ଥିତି PP2_SUBTYPE_SSL_VERSION:
#ifdef WITH_TLS
				mosquitto_free(context->proxy.tls_version);
				context->proxy.tls_version = mosquitto_strndup((ଅଚଳ ଅକ୍ଷର *)&context->proxy.buf[context->proxy.pos], tlv_len);
#ଅନ୍ୟଥା
				ଫେରନ୍ତୁ MOSQ_ERR_NOT_SUPPORTED;
#endif
				ଭାଙ୍ଗ;

			ସ୍ଥିତି PP2_SUBTYPE_SSL_CIPHER:
#ifdef WITH_TLS
				mosquitto_free(context->proxy.cipher);
				context->proxy.cipher = mosquitto_strndup((ଅଚଳ ଅକ୍ଷର *)&context->proxy.buf[context->proxy.pos], tlv_len);
#ଅନ୍ୟଥା
				ଫେରନ୍ତୁ MOSQ_ERR_NOT_SUPPORTED;
#endif
				ଭାଙ୍ଗ;

			ସ୍ଥିତି PP2_SUBTYPE_SSL_CN:
#ifdef WITH_TLS
				ଯଦି(context->listener->use_identity_as_username){
					mosquitto_free(context->username);
					context->username = mosquitto_strndup((ଅଚଳ ଅକ୍ଷର *)&context->proxy.buf[context->proxy.pos], tlv_len);
					ଯଦି(!context->username){
						ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
					}
				}
#ଅନ୍ୟଥା
				ଫେରନ୍ତୁ MOSQ_ERR_NOT_SUPPORTED;
#endif
				ଭାଙ୍ଗ;
		}
		len = (uint16_t)(len - (ଆକାର(uint8_t) + ଆକାର(uint8_t) + ଆକାର(uint8_t) + tlv_len));
		context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
	}
	context->proxy.have_tls = true;

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_tlv(ସଂରଚନା mosquitto *context, bool *have_certificate)
{
	ଯେତେବେଳେ(context->proxy.pos < context->proxy.len){
		ଯଦି(context->proxy.len - context->proxy.pos < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଆକାର(ସଂରଚନା pp2_tlv)){
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
		ସଂରଚନା pp2_tlv *tlv = (ସଂରଚନା pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->length_h<<8) + tlv->length_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + ଆକାର(ସଂରଚନା pp2_tlv));

		ଯଦି(tlv_len > context->proxy.len - context->proxy.pos){
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}

		ବାଛ(tlv->type){
			ସ୍ଥିତି PP2_TYPE_SSL:
				{
					ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = read_tlv_ssl(context, tlv_len, have_certificate);
					ଯଦି(rc){
						ଫେରନ୍ତୁ rc;
					}
				}
				ଭାଙ୍ଗ;
			ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
				context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
				ଭାଙ୍ଗ;
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା proxy_v2__read(ସଂରଚନା mosquitto *context)
{
	ସଂରଚନା proxy_hdr_v2 hdr;

	ଯଦି(context->proxy.cmd == -1){
		context->proxy.buf = ଖାଲି;
		ଯଦି(net__read(context, &hdr, ଆକାର(hdr)) != ଆକାର(hdr)){
			ଫେରନ୍ତୁ MOSQ_ERR_CONN_LOST;
		}
		ଯଦି(memcmp(hdr.sig, signature, ଆକାର(signature))){
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
		ଯଦି((hdr.ver_cmd & 0xF0) != 0x20){
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
		context->proxy.cmd = hdr.ver_cmd & 0x0F;
		ଯଦି(context->proxy.cmd != 0x00 && context->proxy.cmd != 0x01){
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
		context->proxy.fam = hdr.fam;
		ଯଦି((hdr.fam != 0x00 || context->proxy.cmd != PROXY_CMD_LOCAL)
				&& hdr.fam != PROXY_TCP_IPV4
				&& hdr.fam != PROXY_TCP_IPV6
				&& hdr.fam != PROXY_TCP_UNIX){

			ଫେରନ୍ତୁ 1;
		}
		context->proxy.pos = 0;
		context->proxy.len = ntohs(hdr.len);
		ଯଦି(context->proxy.len > 0){
			/* PROXY_PACKET_LIMIT=500 bytes, arbitrary upper limit */
			ବାଛ(context->proxy.fam){
				ସ୍ଥିତି PROXY_TCP_IPV4:
					ଯଦି(context->proxy.len < 12 || context->proxy.len > PROXY_PACKET_LIMIT){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଭାଙ୍ଗ;
				ସ୍ଥିତି PROXY_TCP_IPV6:
					ଯଦି(context->proxy.len < 36 || context->proxy.len > PROXY_PACKET_LIMIT){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଭାଙ୍ଗ;
				ସ୍ଥିତି PROXY_TCP_UNIX:
					ଯଦି(context->proxy.len > PROXY_PACKET_LIMIT){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଭାଙ୍ଗ;
			}
			context->proxy.buf = mosquitto_calloc(1, (size_t)(context->proxy.len+1));
			ଯଦି(!context->proxy.buf){
				ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
			}
		}ଅନ୍ୟଥା{
			ଯଦି(context->proxy.cmd != PROXY_CMD_LOCAL || context->proxy.fam != 0x00){
				ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
			}
		}

	}

	ଯଦି(context->proxy.pos < context->proxy.len){
		ssize_t rc = net__read(context, context->proxy.buf, (size_t)(context->proxy.len - context->proxy.pos));
		ଯଦି(rc > 0){
			context->proxy.pos = (uint16_t)(context->proxy.pos + rc);
		}ଅନ୍ୟଥା{
			proxy_cleanup(context);
			ଫେରନ୍ତୁ MOSQ_ERR_CONN_LOST;
		}
	}
	ଯଦି(context->proxy.pos == context->proxy.len){
		ଯଦି(context->proxy.fam == PROXY_TCP_IPV4){
			ଅକ୍ଷର address[100];
			ସଂଘ proxy_addr *addr = (ସଂଘ proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET, &addr->ipv4_addr.src_addr, address, ଆକାର(address));
			context->address = mosquitto_strdup(address);
			context->remote_port = ntohs(addr->ipv4_addr.src_port);
			context->proxy.pos = 4+4+2+2;
		}ଅନ୍ୟଥା ଯଦି(context->proxy.fam == PROXY_TCP_IPV6){
			ଅକ୍ଷର address[100];
			ସଂଘ proxy_addr *addr = (ସଂଘ proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET6, addr->ipv6_addr.src_addr, address, ଆକାର(address));
			context->address = mosquitto_strdup(address);
			context->remote_port = ntohs(addr->ipv6_addr.src_port);
			context->proxy.pos = 16+16+2+2;
		}ଅନ୍ୟଥା ଯଦି(context->proxy.fam == PROXY_TCP_UNIX){
			ସଂଘ proxy_addr *addr = (ସଂଘ proxy_addr *)context->proxy.buf;
			context->address = mosquitto_strndup((ଅକ୍ଷର *)addr->unix_addr.src_addr, ଆକାର(addr->unix_addr.src_addr));
			context->remote_port = 0;
			context->proxy.pos = (uint16_t)(ଆକାର(addr->unix_addr.src_addr) + 1);
		}ଅନ୍ୟଥା{
			/* Must be LOCAL */
			/* Ignore address */
			context->address = ଖାଲି;
			context->remote_port = 0;
			proxy_cleanup(context);
			ଫେରନ୍ତୁ MOSQ_ERR_PROXY;
		}
		ଯଦି(!context->address){
			proxy_cleanup(context);
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}

		bool have_certificate = false;

		ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = read_tlv(context, &have_certificate);
		ଯଦି(rc){
			log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, corrupt PROXY header.",
					context->address, context->remote_port);
			proxy_cleanup(context);
			ଫେରନ୍ତୁ MOSQ_ERR_PROXY;
		}
		mosquitto_FREE(context->proxy.buf);

		ଯଦି(context->listener->proxy_protocol_v2_require_tls && !context->proxy.have_tls){
			log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not connect using TLS.",
					context->address, context->remote_port);
			proxy_cleanup(context);
			ଫେରନ୍ତୁ MOSQ_ERR_PROXY;
		}

#ifdef WITH_TLS
		ଯଦି(context->listener->require_certificate){
			ଯଦି(!have_certificate){
				log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not provide a certificate.",
						context->address, context->remote_port);
				proxy_cleanup(context);
				ଫେରନ୍ତୁ MOSQ_ERR_PROXY;
			}
		}

		ଯଦି(context->proxy.tls_version && context->proxy.cipher){
			log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Connection from %s:%d negotiated %s cipher %s",
					context->address, context->remote_port, context->proxy.tls_version, context->proxy.cipher);
		}
#endif
		proxy_cleanup(context);

#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
		ଯଦି(context->listener->protocol == mp_websockets){
			ଫେରନ୍ତୁ http__context_init(context);
		}ଅନ୍ୟଥା
#endif
		{
			context->transport = mosq_t_tcp;
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}
#endif
