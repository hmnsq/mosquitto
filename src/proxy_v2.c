#ifdef WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#ಅನ್ಯಥಾ
#  include <arpa/inet.h>
#  include <netinet/in.h>
#endif
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto_internal.h"
#ಅನ್ತರ್ಭಾವ "net_mosq.h"

#ಯದಿ !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN

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
#define PP2_SUBTYPE_SSL_ಕೀಲಿ_ALG 0x25
#define PP2_TYPE_NETNS          0x30

#define PP2_CLIENT_SSL           0x01
#define PP2_CLIENT_CERT_CONN     0x02
#define PP2_CLIENT_CERT_SESS     0x04

#define PROXY_PACKET_LIMIT 500

ಸಂರಚನಾ proxy_hdr_v2 {
	uint8_t sig[12]; /* hex 0D 0A 0D 0A 00 0D 0A 51 55 49 54 0A */
	uint8_t ver_cmd; /* protocol version and command */
	uint8_t fam; /* protocol family and address */
	uint16_t len; /* number of following bytes part of the header */
};

ಸಙ್ಘ proxy_addr {
	ಸಂರಚನಾ { /* for TCP/UDP over IPv4, len = 12 */
		uint32_t src_addr;
		uint32_t dst_addr;
		uint16_t src_port;
		uint16_t dst_port;
	} ipv4_addr;
	ಸಂರಚನಾ { /* for TCP/UDP over IPv6, len = 36 */
		uint8_t src_addr[16];
		uint8_t dst_addr[16];
		uint16_t src_port;
		uint16_t dst_port;
	} ipv6_addr;
	ಸಂರಚನಾ { /* for AF_UNIX sockets, len = 216 */
		uint8_t src_addr[108];
		uint8_t dst_addr[108];
	} unix_addr;
};

ಸಂರಚನಾ pp2_tlv {
	uint8_t type;
	uint8_t length_h;
	uint8_t length_l;
};

ಸಂರಚನಾ pp2_tlv_ssl {
	uint8_t client;
	uint32_t verify;
};

ಧ್ರುವ uint8_t signature[12] = {0x0D, 0x0A, 0x0D, 0x0A, 0x00, 0x0D, 0x0A, 0x51, 0x55, 0x49, 0x54, 0x0A};


ಸ್ಥಿರ ಶೂನ್ಯ proxy_cleanup(ಸಂರಚನಾ mosquitto *context)
{
	mosquitto_FREE(context->proxy.buf);
	mosquitto_FREE(context->proxy.tls_version);
	mosquitto_FREE(context->proxy.cipher);
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ read_tlv_ssl(ಸಂರಚನಾ mosquitto *context, uint16_t len, bool *have_certificate)
{
	ಸಂರಚನಾ pp2_tlv_ssl ssl = {0};

	ಯದಿ(len < ಪರಿಮಾಣ(uint8_t) + ಪರಿಮಾಣ(uint32_t)){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	ssl.client = context->proxy.buf[context->proxy.pos];
	ssl.verify = ntohl(*(uint32_t *)(&context->proxy.buf[context->proxy.pos + ಪರಿಮಾಣ(uint8_t)]));
	context->proxy.pos = (uint16_t)(context->proxy.pos + ಪರಿಮಾಣ(uint8_t) + ಪರಿಮಾಣ(uint32_t));
	len = (uint16_t)(len - (ಪರಿಮಾಣ(uint8_t) + ಪರಿಮಾಣ(uint32_t)));

	ಯದಿ(ssl.client & PP2_CLIENT_SSL && ssl.client & PP2_CLIENT_CERT_SESS && !ssl.verify){
		*have_certificate = true;
	}

	ಯಾವತ್(len > 0){
		ಯದಿ(context->proxy.len - context->proxy.pos < (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(ಸಂರಚನಾ pp2_tlv)){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
		ಸಂರಚನಾ pp2_tlv *tlv = (ಸಂರಚನಾ pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->length_h<<8) + tlv->length_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + ಪರಿಮಾಣ(ಸಂರಚನಾ pp2_tlv));

		ಯದಿ(tlv_len > context->proxy.len - context->proxy.pos){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}

		ವಿಕಲ್ಪ(tlv->type){
			ಪಕ್ಷ PP2_SUBTYPE_SSL_VERSION:
#ifdef WITH_TLS
				mosquitto_free(context->proxy.tls_version);
				context->proxy.tls_version = mosquitto_strndup((ಧ್ರುವ ವರ್ಣ *)&context->proxy.buf[context->proxy.pos], tlv_len);
#ಅನ್ಯಥಾ
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_SUPPORTED;
#endif
				ಭಙ್ಗ;

			ಪಕ್ಷ PP2_SUBTYPE_SSL_CIPHER:
#ifdef WITH_TLS
				mosquitto_free(context->proxy.cipher);
				context->proxy.cipher = mosquitto_strndup((ಧ್ರುವ ವರ್ಣ *)&context->proxy.buf[context->proxy.pos], tlv_len);
#ಅನ್ಯಥಾ
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_SUPPORTED;
#endif
				ಭಙ್ಗ;

			ಪಕ್ಷ PP2_SUBTYPE_SSL_CN:
#ifdef WITH_TLS
				ಯದಿ(context->listener->use_identity_as_username){
					mosquitto_free(context->username);
					context->username = mosquitto_strndup((ಧ್ರುವ ವರ್ಣ *)&context->proxy.buf[context->proxy.pos], tlv_len);
					ಯದಿ(!context->username){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
					}
				}
#ಅನ್ಯಥಾ
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_SUPPORTED;
#endif
				ಭಙ್ಗ;
		}
		len = (uint16_t)(len - (ಪರಿಮಾಣ(uint8_t) + ಪರಿಮಾಣ(uint8_t) + ಪರಿಮಾಣ(uint8_t) + tlv_len));
		context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
	}
	context->proxy.have_tls = true;

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ read_tlv(ಸಂರಚನಾ mosquitto *context, bool *have_certificate)
{
	ಯಾವತ್(context->proxy.pos < context->proxy.len){
		ಯದಿ(context->proxy.len - context->proxy.pos < (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(ಸಂರಚನಾ pp2_tlv)){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
		ಸಂರಚನಾ pp2_tlv *tlv = (ಸಂರಚನಾ pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->length_h<<8) + tlv->length_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + ಪರಿಮಾಣ(ಸಂರಚನಾ pp2_tlv));

		ಯದಿ(tlv_len > context->proxy.len - context->proxy.pos){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}

		ವಿಕಲ್ಪ(tlv->type){
			ಪಕ್ಷ PP2_TYPE_SSL:
				{
					ಪೂರ್ಣಾಙ್ಕ rc = read_tlv_ssl(context, tlv_len, have_certificate);
					ಯದಿ(rc){
						ಪ್ರತ್ಯಾಗಮ rc;
					}
				}
				ಭಙ್ಗ;
			ಸ್ವಭಾವ:
				context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
				ಭಙ್ಗ;
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ proxy_v2__read(ಸಂರಚನಾ mosquitto *context)
{
	ಸಂರಚನಾ proxy_hdr_v2 hdr;

	ಯದಿ(context->proxy.cmd == -1){
		context->proxy.buf = ರಿಕ್ತ;
		ಯದಿ(net__read(context, &hdr, ಪರಿಮಾಣ(hdr)) != ಪರಿಮಾಣ(hdr)){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_CONN_LOST;
		}
		ಯದಿ(memcmp(hdr.sig, signature, ಪರಿಮಾಣ(signature))){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
		ಯದಿ((hdr.ver_cmd & 0xF0) != 0x20){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
		context->proxy.cmd = hdr.ver_cmd & 0x0F;
		ಯದಿ(context->proxy.cmd != 0x00 && context->proxy.cmd != 0x01){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
		context->proxy.fam = hdr.fam;
		ಯದಿ((hdr.fam != 0x00 || context->proxy.cmd != PROXY_CMD_LOCAL)
				&& hdr.fam != PROXY_TCP_IPV4
				&& hdr.fam != PROXY_TCP_IPV6
				&& hdr.fam != PROXY_TCP_UNIX){

			ಪ್ರತ್ಯಾಗಮ 1;
		}
		context->proxy.pos = 0;
		context->proxy.len = ntohs(hdr.len);
		ಯದಿ(context->proxy.len > 0){
			/* PROXY_PACKET_LIMIT=500 bytes, arbitrary upper limit */
			ವಿಕಲ್ಪ(context->proxy.fam){
				ಪಕ್ಷ PROXY_TCP_IPV4:
					ಯದಿ(context->proxy.len < 12 || context->proxy.len > PROXY_PACKET_LIMIT){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಭಙ್ಗ;
				ಪಕ್ಷ PROXY_TCP_IPV6:
					ಯದಿ(context->proxy.len < 36 || context->proxy.len > PROXY_PACKET_LIMIT){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಭಙ್ಗ;
				ಪಕ್ಷ PROXY_TCP_UNIX:
					ಯದಿ(context->proxy.len > PROXY_PACKET_LIMIT){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಭಙ್ಗ;
			}
			context->proxy.buf = mosquitto_calloc(1, (size_t)(context->proxy.len+1));
			ಯದಿ(!context->proxy.buf){
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
			}
		}ಅನ್ಯಥಾ{
			ಯದಿ(context->proxy.cmd != PROXY_CMD_LOCAL || context->proxy.fam != 0x00){
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
			}
		}

	}

	ಯದಿ(context->proxy.pos < context->proxy.len){
		ssize_t rc = net__read(context, context->proxy.buf, (size_t)(context->proxy.len - context->proxy.pos));
		ಯದಿ(rc > 0){
			context->proxy.pos = (uint16_t)(context->proxy.pos + rc);
		}ಅನ್ಯಥಾ{
			proxy_cleanup(context);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_CONN_LOST;
		}
	}
	ಯದಿ(context->proxy.pos == context->proxy.len){
		ಯದಿ(context->proxy.fam == PROXY_TCP_IPV4){
			ವರ್ಣ address[100];
			ಸಙ್ಘ proxy_addr *addr = (ಸಙ್ಘ proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET, &addr->ipv4_addr.src_addr, address, ಪರಿಮಾಣ(address));
			context->address = mosquitto_strdup(address);
			context->remote_port = ntohs(addr->ipv4_addr.src_port);
			context->proxy.pos = 4+4+2+2;
		}ಅನ್ಯಥಾ ಯದಿ(context->proxy.fam == PROXY_TCP_IPV6){
			ವರ್ಣ address[100];
			ಸಙ್ಘ proxy_addr *addr = (ಸಙ್ಘ proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET6, addr->ipv6_addr.src_addr, address, ಪರಿಮಾಣ(address));
			context->address = mosquitto_strdup(address);
			context->remote_port = ntohs(addr->ipv6_addr.src_port);
			context->proxy.pos = 16+16+2+2;
		}ಅನ್ಯಥಾ ಯದಿ(context->proxy.fam == PROXY_TCP_UNIX){
			ಸಙ್ಘ proxy_addr *addr = (ಸಙ್ಘ proxy_addr *)context->proxy.buf;
			context->address = mosquitto_strndup((ವರ್ಣ *)addr->unix_addr.src_addr, ಪರಿಮಾಣ(addr->unix_addr.src_addr));
			context->remote_port = 0;
			context->proxy.pos = (uint16_t)(ಪರಿಮಾಣ(addr->unix_addr.src_addr) + 1);
		}ಅನ್ಯಥಾ{
			/* Must be LOCAL */
			/* Ignore address */
			context->address = ರಿಕ್ತ;
			context->remote_port = 0;
			proxy_cleanup(context);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROXY;
		}
		ಯದಿ(!context->address){
			proxy_cleanup(context);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}

		bool have_certificate = false;

		ಪೂರ್ಣಾಙ್ಕ rc = read_tlv(context, &have_certificate);
		ಯದಿ(rc){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, corrupt PROXY header.",
					context->address, context->remote_port);
			proxy_cleanup(context);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROXY;
		}
		mosquitto_FREE(context->proxy.buf);

		ಯದಿ(context->listener->proxy_protocol_v2_require_tls && !context->proxy.have_tls){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not connect using TLS.",
					context->address, context->remote_port);
			proxy_cleanup(context);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROXY;
		}

#ifdef WITH_TLS
		ಯದಿ(context->listener->require_certificate){
			ಯದಿ(!have_certificate){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not provide a certificate.",
						context->address, context->remote_port);
				proxy_cleanup(context);
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROXY;
			}
		}

		ಯದಿ(context->proxy.tls_version && context->proxy.cipher){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Connection from %s:%d negotiated %s cipher %s",
					context->address, context->remote_port, context->proxy.tls_version, context->proxy.cipher);
		}
#endif
		proxy_cleanup(context);

#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
		ಯದಿ(context->listener->protocol == mp_websockets){
			ಪ್ರತ್ಯಾಗಮ http__context_init(context);
		}ಅನ್ಯಥಾ
#endif
		{
			context->transport = mosq_t_tcp;
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}
#endif
