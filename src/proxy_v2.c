#ifdef WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#નહીંતર
#  include <arpa/inet.h>
#  include <netinet/in.h>
#endif
#અન્તર્ભાવ <stdint.h>
#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto_internal.h"
#અન્તર્ભાવ "net_mosq.h"

#જો !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN

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
#define PP2_SUBTYPE_SSL_ચાવી_ALG 0x25
#define PP2_TYPE_NETNS          0x30

#define PP2_CLIENT_SSL           0x01
#define PP2_CLIENT_CERT_CONN     0x02
#define PP2_CLIENT_CERT_SESS     0x04

#define PROXY_PACKET_LIMIT 500

સંરચના proxy_hdr_v2 {
	uint8_t sig[12]; /* hex 0D 0A 0D 0A 00 0D 0A 51 55 49 54 0A */
	uint8_t ver_cmd; /* protocol version and command */
	uint8_t fam; /* protocol family and address */
	uint16_t len; /* number of following bytes part of the header */
};

સંઘ proxy_addr {
	સંરચના { /* for TCP/UDP over IPv4, len = 12 */
		uint32_t src_addr;
		uint32_t dst_addr;
		uint16_t src_port;
		uint16_t dst_port;
	} ipv4_addr;
	સંરચના { /* for TCP/UDP over IPv6, len = 36 */
		uint8_t src_addr[16];
		uint8_t dst_addr[16];
		uint16_t src_port;
		uint16_t dst_port;
	} ipv6_addr;
	સંરચના { /* for AF_UNIX sockets, len = 216 */
		uint8_t src_addr[108];
		uint8_t dst_addr[108];
	} unix_addr;
};

સંરચના pp2_tlv {
	uint8_t type;
	uint8_t length_h;
	uint8_t length_l;
};

સંરચના pp2_tlv_ssl {
	uint8_t client;
	uint32_t verify;
};

અચલ uint8_t signature[12] = {0x0D, 0x0A, 0x0D, 0x0A, 0x00, 0x0D, 0x0A, 0x51, 0x55, 0x49, 0x54, 0x0A};


સ્થિર શૂન્ય proxy_cleanup(સંરચના mosquitto *context)
{
	mosquitto_FREE(context->proxy.buf);
	mosquitto_FREE(context->proxy.tls_version);
	mosquitto_FREE(context->proxy.cipher);
}


સ્થિર પૂર્ણાંક read_tlv_ssl(સંરચના mosquitto *context, uint16_t len, bool *have_certificate)
{
	સંરચના pp2_tlv_ssl ssl = {0};

	જો(len < કદ(uint8_t) + કદ(uint32_t)){
		પાછા MOSQ_ERR_INVAL;
	}

	ssl.client = context->proxy.buf[context->proxy.pos];
	ssl.verify = ntohl(*(uint32_t *)(&context->proxy.buf[context->proxy.pos + કદ(uint8_t)]));
	context->proxy.pos = (uint16_t)(context->proxy.pos + કદ(uint8_t) + કદ(uint32_t));
	len = (uint16_t)(len - (કદ(uint8_t) + કદ(uint32_t)));

	જો(ssl.client & PP2_CLIENT_SSL && ssl.client & PP2_CLIENT_CERT_SESS && !ssl.verify){
		*have_certificate = true;
	}

	જ્યારે(len > 0){
		જો(context->proxy.len - context->proxy.pos < (પૂર્ણાંક)કદ(સંરચના pp2_tlv)){
			પાછા MOSQ_ERR_INVAL;
		}
		સંરચના pp2_tlv *tlv = (સંરચના pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->length_h<<8) + tlv->length_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + કદ(સંરચના pp2_tlv));

		જો(tlv_len > context->proxy.len - context->proxy.pos){
			પાછા MOSQ_ERR_INVAL;
		}

		પસંદગી(tlv->type){
			સ્થિતિ PP2_SUBTYPE_SSL_VERSION:
#ifdef WITH_TLS
				mosquitto_free(context->proxy.tls_version);
				context->proxy.tls_version = mosquitto_strndup((અચલ અક્ષર *)&context->proxy.buf[context->proxy.pos], tlv_len);
#નહીંતર
				પાછા MOSQ_ERR_NOT_SUPPORTED;
#endif
				તોડો;

			સ્થિતિ PP2_SUBTYPE_SSL_CIPHER:
#ifdef WITH_TLS
				mosquitto_free(context->proxy.cipher);
				context->proxy.cipher = mosquitto_strndup((અચલ અક્ષર *)&context->proxy.buf[context->proxy.pos], tlv_len);
#નહીંતર
				પાછા MOSQ_ERR_NOT_SUPPORTED;
#endif
				તોડો;

			સ્થિતિ PP2_SUBTYPE_SSL_CN:
#ifdef WITH_TLS
				જો(context->listener->use_identity_as_username){
					mosquitto_free(context->username);
					context->username = mosquitto_strndup((અચલ અક્ષર *)&context->proxy.buf[context->proxy.pos], tlv_len);
					જો(!context->username){
						પાછા MOSQ_ERR_NOMEM;
					}
				}
#નહીંતર
				પાછા MOSQ_ERR_NOT_SUPPORTED;
#endif
				તોડો;
		}
		len = (uint16_t)(len - (કદ(uint8_t) + કદ(uint8_t) + કદ(uint8_t) + tlv_len));
		context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
	}
	context->proxy.have_tls = true;

	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક read_tlv(સંરચના mosquitto *context, bool *have_certificate)
{
	જ્યારે(context->proxy.pos < context->proxy.len){
		જો(context->proxy.len - context->proxy.pos < (પૂર્ણાંક)કદ(સંરચના pp2_tlv)){
			પાછા MOSQ_ERR_INVAL;
		}
		સંરચના pp2_tlv *tlv = (સંરચના pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->length_h<<8) + tlv->length_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + કદ(સંરચના pp2_tlv));

		જો(tlv_len > context->proxy.len - context->proxy.pos){
			પાછા MOSQ_ERR_INVAL;
		}

		પસંદગી(tlv->type){
			સ્થિતિ PP2_TYPE_SSL:
				{
					પૂર્ણાંક rc = read_tlv_ssl(context, tlv_len, have_certificate);
					જો(rc){
						પાછા rc;
					}
				}
				તોડો;
			મૂળભૂત:
				context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
				તોડો;
		}
	}

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક proxy_v2__read(સંરચના mosquitto *context)
{
	સંરચના proxy_hdr_v2 hdr;

	જો(context->proxy.cmd == -1){
		context->proxy.buf = ખાલી;
		જો(net__read(context, &hdr, કદ(hdr)) != કદ(hdr)){
			પાછા MOSQ_ERR_CONN_LOST;
		}
		જો(memcmp(hdr.sig, signature, કદ(signature))){
			પાછા MOSQ_ERR_INVAL;
		}
		જો((hdr.ver_cmd & 0xF0) != 0x20){
			પાછા MOSQ_ERR_INVAL;
		}
		context->proxy.cmd = hdr.ver_cmd & 0x0F;
		જો(context->proxy.cmd != 0x00 && context->proxy.cmd != 0x01){
			પાછા MOSQ_ERR_INVAL;
		}
		context->proxy.fam = hdr.fam;
		જો((hdr.fam != 0x00 || context->proxy.cmd != PROXY_CMD_LOCAL)
				&& hdr.fam != PROXY_TCP_IPV4
				&& hdr.fam != PROXY_TCP_IPV6
				&& hdr.fam != PROXY_TCP_UNIX){

			પાછા 1;
		}
		context->proxy.pos = 0;
		context->proxy.len = ntohs(hdr.len);
		જો(context->proxy.len > 0){
			/* PROXY_PACKET_LIMIT=500 bytes, arbitrary upper limit */
			પસંદગી(context->proxy.fam){
				સ્થિતિ PROXY_TCP_IPV4:
					જો(context->proxy.len < 12 || context->proxy.len > PROXY_PACKET_LIMIT){
						પાછા MOSQ_ERR_INVAL;
					}
					તોડો;
				સ્થિતિ PROXY_TCP_IPV6:
					જો(context->proxy.len < 36 || context->proxy.len > PROXY_PACKET_LIMIT){
						પાછા MOSQ_ERR_INVAL;
					}
					તોડો;
				સ્થિતિ PROXY_TCP_UNIX:
					જો(context->proxy.len > PROXY_PACKET_LIMIT){
						પાછા MOSQ_ERR_INVAL;
					}
					તોડો;
			}
			context->proxy.buf = mosquitto_calloc(1, (size_t)(context->proxy.len+1));
			જો(!context->proxy.buf){
				પાછા MOSQ_ERR_NOMEM;
			}
		}નહીંતર{
			જો(context->proxy.cmd != PROXY_CMD_LOCAL || context->proxy.fam != 0x00){
				પાછા MOSQ_ERR_PROTOCOL;
			}
		}

	}

	જો(context->proxy.pos < context->proxy.len){
		ssize_t rc = net__read(context, context->proxy.buf, (size_t)(context->proxy.len - context->proxy.pos));
		જો(rc > 0){
			context->proxy.pos = (uint16_t)(context->proxy.pos + rc);
		}નહીંતર{
			proxy_cleanup(context);
			પાછા MOSQ_ERR_CONN_LOST;
		}
	}
	જો(context->proxy.pos == context->proxy.len){
		જો(context->proxy.fam == PROXY_TCP_IPV4){
			અક્ષર address[100];
			સંઘ proxy_addr *addr = (સંઘ proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET, &addr->ipv4_addr.src_addr, address, કદ(address));
			context->address = mosquitto_strdup(address);
			context->remote_port = ntohs(addr->ipv4_addr.src_port);
			context->proxy.pos = 4+4+2+2;
		}નહીંતર જો(context->proxy.fam == PROXY_TCP_IPV6){
			અક્ષર address[100];
			સંઘ proxy_addr *addr = (સંઘ proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET6, addr->ipv6_addr.src_addr, address, કદ(address));
			context->address = mosquitto_strdup(address);
			context->remote_port = ntohs(addr->ipv6_addr.src_port);
			context->proxy.pos = 16+16+2+2;
		}નહીંતર જો(context->proxy.fam == PROXY_TCP_UNIX){
			સંઘ proxy_addr *addr = (સંઘ proxy_addr *)context->proxy.buf;
			context->address = mosquitto_strndup((અક્ષર *)addr->unix_addr.src_addr, કદ(addr->unix_addr.src_addr));
			context->remote_port = 0;
			context->proxy.pos = (uint16_t)(કદ(addr->unix_addr.src_addr) + 1);
		}નહીંતર{
			/* Must be LOCAL */
			/* Ignore address */
			context->address = ખાલી;
			context->remote_port = 0;
			proxy_cleanup(context);
			પાછા MOSQ_ERR_PROXY;
		}
		જો(!context->address){
			proxy_cleanup(context);
			પાછા MOSQ_ERR_NOMEM;
		}

		bool have_certificate = false;

		પૂર્ણાંક rc = read_tlv(context, &have_certificate);
		જો(rc){
			log__printf(ખાલી, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, corrupt PROXY header.",
					context->address, context->remote_port);
			proxy_cleanup(context);
			પાછા MOSQ_ERR_PROXY;
		}
		mosquitto_FREE(context->proxy.buf);

		જો(context->listener->proxy_protocol_v2_require_tls && !context->proxy.have_tls){
			log__printf(ખાલી, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not connect using TLS.",
					context->address, context->remote_port);
			proxy_cleanup(context);
			પાછા MOSQ_ERR_PROXY;
		}

#ifdef WITH_TLS
		જો(context->listener->require_certificate){
			જો(!have_certificate){
				log__printf(ખાલી, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not provide a certificate.",
						context->address, context->remote_port);
				proxy_cleanup(context);
				પાછા MOSQ_ERR_PROXY;
			}
		}

		જો(context->proxy.tls_version && context->proxy.cipher){
			log__printf(ખાલી, MOSQ_LOG_NOTICE, "Connection from %s:%d negotiated %s cipher %s",
					context->address, context->remote_port, context->proxy.tls_version, context->proxy.cipher);
		}
#endif
		proxy_cleanup(context);

#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
		જો(context->listener->protocol == mp_websockets){
			પાછા http__context_init(context);
		}નહીંતર
#endif
		{
			context->transport = mosq_t_tcp;
		}
	}

	પાછા MOSQ_ERR_SUCCESS;
}
#endif
