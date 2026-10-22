#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#ಇಲ್ಲದಿದ್ದರೆ
#  include <arpa/inet.h>
#  include <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto_internal.h"
#ಅನ್ತರ್ಭಾವ "net_mosq.h"

#ಆದರೆ !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN

#ವ್ಯಾಖ್ಯಾನಿಸು PROXY_CMD_LOCAL 0x00
#ವ್ಯಾಖ್ಯಾನಿಸು PROXY_CMD_PROXY 0x01

#ವ್ಯಾಖ್ಯಾನಿಸು PROXY_TCP_IPV4 0x11
#ವ್ಯಾಖ್ಯಾನಿಸು PROXY_TCP_IPV6 0x21
#ವ್ಯಾಖ್ಯಾನಿಸು PROXY_TCP_UNIX 0x31

#ವ್ಯಾಖ್ಯಾನಿಸು PP2_TYPE_ALPN           0x01
#ವ್ಯಾಖ್ಯಾನಿಸು PP2_TYPE_AUTHORITY      0x02
#ವ್ಯಾಖ್ಯಾನಿಸು PP2_TYPE_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_32_C         0x03
#ವ್ಯಾಖ್ಯಾನಿಸು PP2_TYPE_NOOP           0x04
#ವ್ಯಾಖ್ಯಾನಿಸು PP2_TYPE_UNIQUE_ID      0x05
#ವ್ಯಾಖ್ಯಾನಿಸು PP2_TYPE_SSL            0x20
#ವ್ಯಾಖ್ಯಾನಿಸು PP2_SUBTYPE_SSL_ಆವೃತ್ತಿ 0x21
#ವ್ಯಾಖ್ಯಾನಿಸು PP2_SUBTYPE_SSL_CN      0x22
#ವ್ಯಾಖ್ಯಾನಿಸು PP2_SUBTYPE_SSL_CIPHER  0x23
#ವ್ಯಾಖ್ಯಾನಿಸು PP2_SUBTYPE_SSL_SIG_ALG 0x24
#ವ್ಯಾಖ್ಯಾನಿಸು PP2_SUBTYPE_SSL_ಕೀಲಿ_ALG 0x25
#ವ್ಯಾಖ್ಯಾನಿಸು PP2_TYPE_NETNS          0x30

#ವ್ಯಾಖ್ಯಾನಿಸು PP2_CLIENT_SSL           0x01
#ವ್ಯಾಖ್ಯಾನಿಸು PP2_CLIENT_CERT_CONN     0x02
#ವ್ಯಾಖ್ಯಾನಿಸು PP2_CLIENT_CERT_SESS     0x04

#ವ್ಯಾಖ್ಯಾನಿಸು PROXY_PACKET_ಎಲ್ಲೆ 500

ರಚನೆ proxy_hdr_v2 {
	uint8_t sig[12]; /* hex 0D 0A 0D 0A 00 0D 0A 51 55 49 54 0A */
	uint8_t ver_cmd; /* protocol version and command */
	uint8_t fam; /* protocol family and address */
	uint16_t len; /* number of following bytes part of the header */
};

ಒಕ್ಕೂಟ proxy_addr {
	ರಚನೆ { /* for TCP/UDP over IPv4, len = 12 */
		uint32_t src_addr;
		uint32_t dst_addr;
		uint16_t src_ಪೋರ್ಟ್;
		uint16_t dst_ಪೋರ್ಟ್;
	} ipv4_addr;
	ರಚನೆ { /* for TCP/UDP over IPv6, len = 36 */
		uint8_t src_addr[16];
		uint8_t dst_addr[16];
		uint16_t src_ಪೋರ್ಟ್;
		uint16_t dst_ಪೋರ್ಟ್;
	} ipv6_addr;
	ರಚನೆ { /* for AF_UNIX sockets, len = 216 */
		uint8_t src_addr[108];
		uint8_t dst_addr[108];
	} unix_addr;
};

ರಚನೆ pp2_tlv {
	uint8_t type;
	uint8_t ಉದ್ದಳತೆ_h;
	uint8_t ಉದ್ದಳತೆ_l;
};

ರಚನೆ pp2_tlv_ssl {
	uint8_t client;
	uint32_t verify;
};

ಸ್ಥಿರಾಂಕ uint8_t signature[12] = {0x0D, 0x0A, 0x0D, 0x0A, 0x00, 0x0D, 0x0A, 0x51, 0x55, 0x49, 0x54, 0x0A};


ಸ್ಥಿರ ಶೂನ್ಯ proxy_cleanup(ರಚನೆ mosquitto *context)
{
	mosquitto_FREE(context->proxy.buf);
	mosquitto_FREE(context->proxy.tls_ಆವೃತ್ತಿ);
	mosquitto_FREE(context->proxy.cipher);
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ read_tlv_ssl(ರಚನೆ mosquitto *context, uint16_t len, bool *have_certificate)
{
	ರಚನೆ pp2_tlv_ssl ssl = {0};

	ಆದರೆ(len < ಗಾತ್ರ(uint8_t) + ಗಾತ್ರ(uint32_t)){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	ssl.client = context->proxy.buf[context->proxy.pos];
	ssl.verify = ntohl(*(uint32_t *)(&context->proxy.buf[context->proxy.pos + ಗಾತ್ರ(uint8_t)]));
	context->proxy.pos = (uint16_t)(context->proxy.pos + ಗಾತ್ರ(uint8_t) + ಗಾತ್ರ(uint32_t));
	len = (uint16_t)(len - (ಗಾತ್ರ(uint8_t) + ಗಾತ್ರ(uint32_t)));

	ಆದರೆ(ssl.client & PP2_CLIENT_SSL && ssl.client & PP2_CLIENT_CERT_SESS && !ssl.verify){
		*have_certificate = true;
	}

	ಆಗ(len > 0){
		ಆದರೆ(context->proxy.len - context->proxy.pos < (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(ರಚನೆ pp2_tlv)){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
		ರಚನೆ pp2_tlv *tlv = (ರಚನೆ pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->ಉದ್ದಳತೆ_h<<8) + tlv->ಉದ್ದಳತೆ_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + ಗಾತ್ರ(ರಚನೆ pp2_tlv));

		ಆದರೆ(tlv_len > context->proxy.len - context->proxy.pos){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}

		ಆಯ್ಕೆ(tlv->type){
			ಸ್ಥಿತಿ PP2_SUBTYPE_SSL_ಆವೃತ್ತಿ:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
				mosquitto_free(context->proxy.tls_ಆವೃತ್ತಿ);
				context->proxy.tls_ಆವೃತ್ತಿ = mosquitto_strndup((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)&context->proxy.buf[context->proxy.pos], tlv_len);
#ಇಲ್ಲದಿದ್ದರೆ
				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_SUPPORTED;
#ಆದರೆ_ಅಂತ್ಯ
				ನಿಲ್ಲಿಸು;

			ಸ್ಥಿತಿ PP2_SUBTYPE_SSL_CIPHER:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
				mosquitto_free(context->proxy.cipher);
				context->proxy.cipher = mosquitto_strndup((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)&context->proxy.buf[context->proxy.pos], tlv_len);
#ಇಲ್ಲದಿದ್ದರೆ
				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_SUPPORTED;
#ಆದರೆ_ಅಂತ್ಯ
				ನಿಲ್ಲಿಸು;

			ಸ್ಥಿತಿ PP2_SUBTYPE_SSL_CN:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
				ಆದರೆ(context->listener->use_ತಟಸ್ಥ_as_username){
					mosquitto_free(context->username);
					context->username = mosquitto_strndup((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)&context->proxy.buf[context->proxy.pos], tlv_len);
					ಆದರೆ(!context->username){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
					}
				}
#ಇಲ್ಲದಿದ್ದರೆ
				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_SUPPORTED;
#ಆದರೆ_ಅಂತ್ಯ
				ನಿಲ್ಲಿಸು;
		}
		len = (uint16_t)(len - (ಗಾತ್ರ(uint8_t) + ಗಾತ್ರ(uint8_t) + ಗಾತ್ರ(uint8_t) + tlv_len));
		context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
	}
	context->proxy.have_tls = true;

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ read_tlv(ರಚನೆ mosquitto *context, bool *have_certificate)
{
	ಆಗ(context->proxy.pos < context->proxy.len){
		ಆದರೆ(context->proxy.len - context->proxy.pos < (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(ರಚನೆ pp2_tlv)){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
		ರಚನೆ pp2_tlv *tlv = (ರಚನೆ pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->ಉದ್ದಳತೆ_h<<8) + tlv->ಉದ್ದಳತೆ_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + ಗಾತ್ರ(ರಚನೆ pp2_tlv));

		ಆದರೆ(tlv_len > context->proxy.len - context->proxy.pos){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}

		ಆಯ್ಕೆ(tlv->type){
			ಸ್ಥಿತಿ PP2_TYPE_SSL:
				{
					ಪೂರ್ಣಾಂಕ rc = read_tlv_ssl(context, tlv_len, have_certificate);
					ಆದರೆ(rc){
						ಹಿಂತಿರುಗಿ rc;
					}
				}
				ನಿಲ್ಲಿಸು;
			ಪೂರ್ವನಿಯೋಜಿತ:
				context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
				ನಿಲ್ಲಿಸು;
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ proxy_v2__read(ರಚನೆ mosquitto *context)
{
	ರಚನೆ proxy_hdr_v2 hdr;

	ಆದರೆ(context->proxy.cmd == -1){
		context->proxy.buf = ಖಾಲಿ;
		ಆದರೆ(net__read(context, &hdr, ಗಾತ್ರ(hdr)) != ಗಾತ್ರ(hdr)){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_CONN_ಕಳೆದುಹೋದ;
		}
		ಆದರೆ(memcmp(hdr.sig, signature, ಗಾತ್ರ(signature))){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
		ಆದರೆ((hdr.ver_cmd & 0xF0) != 0x20){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
		context->proxy.cmd = hdr.ver_cmd & 0x0F;
		ಆದರೆ(context->proxy.cmd != 0x00 && context->proxy.cmd != 0x01){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
		context->proxy.fam = hdr.fam;
		ಆದರೆ((hdr.fam != 0x00 || context->proxy.cmd != PROXY_CMD_LOCAL)
				&& hdr.fam != PROXY_TCP_IPV4
				&& hdr.fam != PROXY_TCP_IPV6
				&& hdr.fam != PROXY_TCP_UNIX){

			ಹಿಂತಿರುಗಿ 1;
		}
		context->proxy.pos = 0;
		context->proxy.len = ntohs(hdr.len);
		ಆದರೆ(context->proxy.len > 0){
			/* PROXY_PACKET_LIMIT=500 bytes, arbitrary upper limit */
			ಆಯ್ಕೆ(context->proxy.fam){
				ಸ್ಥಿತಿ PROXY_TCP_IPV4:
					ಆದರೆ(context->proxy.len < 12 || context->proxy.len > PROXY_PACKET_ಎಲ್ಲೆ){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ PROXY_TCP_IPV6:
					ಆದರೆ(context->proxy.len < 36 || context->proxy.len > PROXY_PACKET_ಎಲ್ಲೆ){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ PROXY_TCP_UNIX:
					ಆದರೆ(context->proxy.len > PROXY_PACKET_ಎಲ್ಲೆ){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ನಿಲ್ಲಿಸು;
			}
			context->proxy.buf = mosquitto_calloc(1, (size_t)(context->proxy.len+1));
			ಆದರೆ(!context->proxy.buf){
				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
			}
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಆದರೆ(context->proxy.cmd != PROXY_CMD_LOCAL || context->proxy.fam != 0x00){
				ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
			}
		}

	}

	ಆದರೆ(context->proxy.pos < context->proxy.len){
		ssize_t rc = net__read(context, context->proxy.buf, (size_t)(context->proxy.len - context->proxy.pos));
		ಆದರೆ(rc > 0){
			context->proxy.pos = (uint16_t)(context->proxy.pos + rc);
		}ಇಲ್ಲದಿದ್ದರೆ{
			proxy_cleanup(context);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_CONN_ಕಳೆದುಹೋದ;
		}
	}
	ಆದರೆ(context->proxy.pos == context->proxy.len){
		ಆದರೆ(context->proxy.fam == PROXY_TCP_IPV4){
			ಅಕ್ಷರ ವಿಳಾಸ[100];
			ಒಕ್ಕೂಟ proxy_addr *addr = (ಒಕ್ಕೂಟ proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET, &addr->ipv4_addr.src_addr, ವಿಳಾಸ, ಗಾತ್ರ(ವಿಳಾಸ));
			context->ವಿಳಾಸ = mosquitto_strdup(ವಿಳಾಸ);
			context->remote_ಪೋರ್ಟ್ = ntohs(addr->ipv4_addr.src_ಪೋರ್ಟ್);
			context->proxy.pos = 4+4+2+2;
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(context->proxy.fam == PROXY_TCP_IPV6){
			ಅಕ್ಷರ ವಿಳಾಸ[100];
			ಒಕ್ಕೂಟ proxy_addr *addr = (ಒಕ್ಕೂಟ proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET6, addr->ipv6_addr.src_addr, ವಿಳಾಸ, ಗಾತ್ರ(ವಿಳಾಸ));
			context->ವಿಳಾಸ = mosquitto_strdup(ವಿಳಾಸ);
			context->remote_ಪೋರ್ಟ್ = ntohs(addr->ipv6_addr.src_ಪೋರ್ಟ್);
			context->proxy.pos = 16+16+2+2;
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(context->proxy.fam == PROXY_TCP_UNIX){
			ಒಕ್ಕೂಟ proxy_addr *addr = (ಒಕ್ಕೂಟ proxy_addr *)context->proxy.buf;
			context->ವಿಳಾಸ = mosquitto_strndup((ಅಕ್ಷರ *)addr->unix_addr.src_addr, ಗಾತ್ರ(addr->unix_addr.src_addr));
			context->remote_ಪೋರ್ಟ್ = 0;
			context->proxy.pos = (uint16_t)(ಗಾತ್ರ(addr->unix_addr.src_addr) + 1);
		}ಇಲ್ಲದಿದ್ದರೆ{
			/* Must be LOCAL */
			/* Ignore address */
			context->ವಿಳಾಸ = ಖಾಲಿ;
			context->remote_ಪೋರ್ಟ್ = 0;
			proxy_cleanup(context);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_PROXY;
		}
		ಆದರೆ(!context->ವಿಳಾಸ){
			proxy_cleanup(context);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}

		bool have_certificate = false;

		ಪೂರ್ಣಾಂಕ rc = read_tlv(context, &have_certificate);
		ಆದರೆ(rc){
			log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, corrupt PROXY header.",
					context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
			proxy_cleanup(context);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_PROXY;
		}
		mosquitto_FREE(context->proxy.buf);

		ಆದರೆ(context->listener->proxy_protocol_v2_require_tls && !context->proxy.have_tls){
			log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not connect using TLS.",
					context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
			proxy_cleanup(context);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_PROXY;
		}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
		ಆದರೆ(context->listener->require_certificate){
			ಆದರೆ(!have_certificate){
				log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not provide a certificate.",
						context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
				proxy_cleanup(context);
				ಹಿಂತಿರುಗಿ MOSQ_ERR_PROXY;
			}
		}

		ಆದರೆ(context->proxy.tls_ಆವೃತ್ತಿ && context->proxy.cipher){
			log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Connection from %s:%d negotiated %s cipher %s",
					context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್, context->proxy.tls_ಆವೃತ್ತಿ, context->proxy.cipher);
		}
#ಆದರೆ_ಅಂತ್ಯ
		proxy_cleanup(context);

#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
		ಆದರೆ(context->listener->protocol == mp_websockets){
			ಹಿಂತಿರುಗಿ http__context_init(context);
		}ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		{
			context->transport = mosq_t_tcp;
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}
#ಆದರೆ_ಅಂತ್ಯ
