#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#ਨਹੀਂਤਾਂ
#  include <arpa/inet.h>
#  include <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "net_mosq.h"

#ਜੇ !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROXY_CMD_LOCAL 0x00
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROXY_CMD_PROXY 0x01

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROXY_TCP_IPV4 0x11
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROXY_TCP_IPV6 0x21
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROXY_TCP_UNIX 0x31

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PP2_TYPE_ALPN           0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PP2_TYPE_AUTHORITY      0x02
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PP2_TYPE_ਚੱਕਰੀ_ਜਾਂਚ_32_C         0x03
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PP2_TYPE_NOOP           0x04
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PP2_TYPE_UNIQUE_ID      0x05
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PP2_TYPE_SSL            0x20
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PP2_SUBTYPE_SSL_ਸੰਸਕਰਣ 0x21
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PP2_SUBTYPE_SSL_CN      0x22
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PP2_SUBTYPE_SSL_CIPHER  0x23
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PP2_SUBTYPE_SSL_SIG_ALG 0x24
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PP2_SUBTYPE_SSL_ਕੁੰਜੀ_ALG 0x25
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PP2_TYPE_NETNS          0x30

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PP2_CLIENT_SSL           0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PP2_CLIENT_CERT_CONN     0x02
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PP2_CLIENT_CERT_SESS     0x04

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROXY_PACKET_ਹੱਦਬੰਦੀ 500

ਬਣਤਰ proxy_hdr_v2 {
	uint8_t sig[12]; /* hex 0D 0A 0D 0A 00 0D 0A 51 55 49 54 0A */
	uint8_t ver_cmd; /* protocol version and command */
	uint8_t fam; /* protocol family and address */
	uint16_t len; /* number of following bytes part of the header */
};

ਸੰਘ proxy_addr {
	ਬਣਤਰ { /* for TCP/UDP over IPv4, len = 12 */
		uint32_t src_addr;
		uint32_t dst_addr;
		uint16_t src_ਪੋਰਟ;
		uint16_t dst_ਪੋਰਟ;
	} ipv4_addr;
	ਬਣਤਰ { /* for TCP/UDP over IPv6, len = 36 */
		uint8_t src_addr[16];
		uint8_t dst_addr[16];
		uint16_t src_ਪੋਰਟ;
		uint16_t dst_ਪੋਰਟ;
	} ipv6_addr;
	ਬਣਤਰ { /* for AF_UNIX sockets, len = 216 */
		uint8_t src_addr[108];
		uint8_t dst_addr[108];
	} unix_addr;
};

ਬਣਤਰ pp2_tlv {
	uint8_t type;
	uint8_t ਲੰਮਾਈ_h;
	uint8_t ਲੰਮਾਈ_l;
};

ਬਣਤਰ pp2_tlv_ssl {
	uint8_t client;
	uint32_t verify;
};

ਅਚਲ uint8_t signature[12] = {0x0D, 0x0A, 0x0D, 0x0A, 0x00, 0x0D, 0x0A, 0x51, 0x55, 0x49, 0x54, 0x0A};


ਸਥਿਰ ਖਾਲੀ proxy_cleanup(ਬਣਤਰ mosquitto *context)
{
	mosquitto_FREE(context->proxy.buf);
	mosquitto_FREE(context->proxy.tls_ਸੰਸਕਰਣ);
	mosquitto_FREE(context->proxy.cipher);
}


ਸਥਿਰ ਪੂਰਨ read_tlv_ssl(ਬਣਤਰ mosquitto *context, uint16_t len, bool *have_certificate)
{
	ਬਣਤਰ pp2_tlv_ssl ssl = {0};

	ਜੇ(len < ਆਕਾਰ(uint8_t) + ਆਕਾਰ(uint32_t)){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	ssl.client = context->proxy.buf[context->proxy.pos];
	ssl.verify = ntohl(*(uint32_t *)(&context->proxy.buf[context->proxy.pos + ਆਕਾਰ(uint8_t)]));
	context->proxy.pos = (uint16_t)(context->proxy.pos + ਆਕਾਰ(uint8_t) + ਆਕਾਰ(uint32_t));
	len = (uint16_t)(len - (ਆਕਾਰ(uint8_t) + ਆਕਾਰ(uint32_t)));

	ਜੇ(ssl.client & PP2_CLIENT_SSL && ssl.client & PP2_CLIENT_CERT_SESS && !ssl.verify){
		*have_certificate = true;
	}

	ਜਦੋਂ(len > 0){
		ਜੇ(context->proxy.len - context->proxy.pos < (ਪੂਰਨ)ਆਕਾਰ(ਬਣਤਰ pp2_tlv)){
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
		ਬਣਤਰ pp2_tlv *tlv = (ਬਣਤਰ pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->ਲੰਮਾਈ_h<<8) + tlv->ਲੰਮਾਈ_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + ਆਕਾਰ(ਬਣਤਰ pp2_tlv));

		ਜੇ(tlv_len > context->proxy.len - context->proxy.pos){
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}

		ਚੋਣ(tlv->type){
			ਹਾਲਤ PP2_SUBTYPE_SSL_ਸੰਸਕਰਣ:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
				mosquitto_free(context->proxy.tls_ਸੰਸਕਰਣ);
				context->proxy.tls_ਸੰਸਕਰਣ = mosquitto_strndup((ਅਚਲ ਅੱਖਰ *)&context->proxy.buf[context->proxy.pos], tlv_len);
#ਨਹੀਂਤਾਂ
				ਵਾਪਸ MOSQ_ERR_NOT_SUPPORTED;
#ਜੇ_ਅੰਤ
				ਰੋਕੋ;

			ਹਾਲਤ PP2_SUBTYPE_SSL_CIPHER:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
				mosquitto_free(context->proxy.cipher);
				context->proxy.cipher = mosquitto_strndup((ਅਚਲ ਅੱਖਰ *)&context->proxy.buf[context->proxy.pos], tlv_len);
#ਨਹੀਂਤਾਂ
				ਵਾਪਸ MOSQ_ERR_NOT_SUPPORTED;
#ਜੇ_ਅੰਤ
				ਰੋਕੋ;

			ਹਾਲਤ PP2_SUBTYPE_SSL_CN:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
				ਜੇ(context->listener->use_ਪਛਾਣ_ਤੱਤ_as_username){
					mosquitto_free(context->username);
					context->username = mosquitto_strndup((ਅਚਲ ਅੱਖਰ *)&context->proxy.buf[context->proxy.pos], tlv_len);
					ਜੇ(!context->username){
						ਵਾਪਸ MOSQ_ERR_NOMEM;
					}
				}
#ਨਹੀਂਤਾਂ
				ਵਾਪਸ MOSQ_ERR_NOT_SUPPORTED;
#ਜੇ_ਅੰਤ
				ਰੋਕੋ;
		}
		len = (uint16_t)(len - (ਆਕਾਰ(uint8_t) + ਆਕਾਰ(uint8_t) + ਆਕਾਰ(uint8_t) + tlv_len));
		context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
	}
	context->proxy.have_tls = true;

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ read_tlv(ਬਣਤਰ mosquitto *context, bool *have_certificate)
{
	ਜਦੋਂ(context->proxy.pos < context->proxy.len){
		ਜੇ(context->proxy.len - context->proxy.pos < (ਪੂਰਨ)ਆਕਾਰ(ਬਣਤਰ pp2_tlv)){
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
		ਬਣਤਰ pp2_tlv *tlv = (ਬਣਤਰ pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->ਲੰਮਾਈ_h<<8) + tlv->ਲੰਮਾਈ_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + ਆਕਾਰ(ਬਣਤਰ pp2_tlv));

		ਜੇ(tlv_len > context->proxy.len - context->proxy.pos){
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}

		ਚੋਣ(tlv->type){
			ਹਾਲਤ PP2_TYPE_SSL:
				{
					ਪੂਰਨ rc = read_tlv_ssl(context, tlv_len, have_certificate);
					ਜੇ(rc){
						ਵਾਪਸ rc;
					}
				}
				ਰੋਕੋ;
			ਮੂਲ:
				context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
				ਰੋਕੋ;
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ proxy_v2__read(ਬਣਤਰ mosquitto *context)
{
	ਬਣਤਰ proxy_hdr_v2 hdr;

	ਜੇ(context->proxy.cmd == -1){
		context->proxy.buf = ਨਿਰਾ;
		ਜੇ(net__read(context, &hdr, ਆਕਾਰ(hdr)) != ਆਕਾਰ(hdr)){
			ਵਾਪਸ MOSQ_ERR_CONN_ਗੁਆਚਾ;
		}
		ਜੇ(memcmp(hdr.sig, signature, ਆਕਾਰ(signature))){
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
		ਜੇ((hdr.ver_cmd & 0xF0) != 0x20){
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
		context->proxy.cmd = hdr.ver_cmd & 0x0F;
		ਜੇ(context->proxy.cmd != 0x00 && context->proxy.cmd != 0x01){
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
		context->proxy.fam = hdr.fam;
		ਜੇ((hdr.fam != 0x00 || context->proxy.cmd != PROXY_CMD_LOCAL)
				&& hdr.fam != PROXY_TCP_IPV4
				&& hdr.fam != PROXY_TCP_IPV6
				&& hdr.fam != PROXY_TCP_UNIX){

			ਵਾਪਸ 1;
		}
		context->proxy.pos = 0;
		context->proxy.len = ntohs(hdr.len);
		ਜੇ(context->proxy.len > 0){
			/* PROXY_PACKET_LIMIT=500 bytes, arbitrary upper limit */
			ਚੋਣ(context->proxy.fam){
				ਹਾਲਤ PROXY_TCP_IPV4:
					ਜੇ(context->proxy.len < 12 || context->proxy.len > PROXY_PACKET_ਹੱਦਬੰਦੀ){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਰੋਕੋ;
				ਹਾਲਤ PROXY_TCP_IPV6:
					ਜੇ(context->proxy.len < 36 || context->proxy.len > PROXY_PACKET_ਹੱਦਬੰਦੀ){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਰੋਕੋ;
				ਹਾਲਤ PROXY_TCP_UNIX:
					ਜੇ(context->proxy.len > PROXY_PACKET_ਹੱਦਬੰਦੀ){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਰੋਕੋ;
			}
			context->proxy.buf = mosquitto_calloc(1, (size_t)(context->proxy.len+1));
			ਜੇ(!context->proxy.buf){
				ਵਾਪਸ MOSQ_ERR_NOMEM;
			}
		}ਨਹੀਂਤਾਂ{
			ਜੇ(context->proxy.cmd != PROXY_CMD_LOCAL || context->proxy.fam != 0x00){
				ਵਾਪਸ MOSQ_ERR_PROTOCOL;
			}
		}

	}

	ਜੇ(context->proxy.pos < context->proxy.len){
		ssize_t rc = net__read(context, context->proxy.buf, (size_t)(context->proxy.len - context->proxy.pos));
		ਜੇ(rc > 0){
			context->proxy.pos = (uint16_t)(context->proxy.pos + rc);
		}ਨਹੀਂਤਾਂ{
			proxy_cleanup(context);
			ਵਾਪਸ MOSQ_ERR_CONN_ਗੁਆਚਾ;
		}
	}
	ਜੇ(context->proxy.pos == context->proxy.len){
		ਜੇ(context->proxy.fam == PROXY_TCP_IPV4){
			ਅੱਖਰ ਪਤਾ[100];
			ਸੰਘ proxy_addr *addr = (ਸੰਘ proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET, &addr->ipv4_addr.src_addr, ਪਤਾ, ਆਕਾਰ(ਪਤਾ));
			context->ਪਤਾ = mosquitto_strdup(ਪਤਾ);
			context->remote_ਪੋਰਟ = ntohs(addr->ipv4_addr.src_ਪੋਰਟ);
			context->proxy.pos = 4+4+2+2;
		}ਨਹੀਂਤਾਂ ਜੇ(context->proxy.fam == PROXY_TCP_IPV6){
			ਅੱਖਰ ਪਤਾ[100];
			ਸੰਘ proxy_addr *addr = (ਸੰਘ proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET6, addr->ipv6_addr.src_addr, ਪਤਾ, ਆਕਾਰ(ਪਤਾ));
			context->ਪਤਾ = mosquitto_strdup(ਪਤਾ);
			context->remote_ਪੋਰਟ = ntohs(addr->ipv6_addr.src_ਪੋਰਟ);
			context->proxy.pos = 16+16+2+2;
		}ਨਹੀਂਤਾਂ ਜੇ(context->proxy.fam == PROXY_TCP_UNIX){
			ਸੰਘ proxy_addr *addr = (ਸੰਘ proxy_addr *)context->proxy.buf;
			context->ਪਤਾ = mosquitto_strndup((ਅੱਖਰ *)addr->unix_addr.src_addr, ਆਕਾਰ(addr->unix_addr.src_addr));
			context->remote_ਪੋਰਟ = 0;
			context->proxy.pos = (uint16_t)(ਆਕਾਰ(addr->unix_addr.src_addr) + 1);
		}ਨਹੀਂਤਾਂ{
			/* Must be LOCAL */
			/* Ignore address */
			context->ਪਤਾ = ਨਿਰਾ;
			context->remote_ਪੋਰਟ = 0;
			proxy_cleanup(context);
			ਵਾਪਸ MOSQ_ERR_PROXY;
		}
		ਜੇ(!context->ਪਤਾ){
			proxy_cleanup(context);
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}

		bool have_certificate = false;

		ਪੂਰਨ rc = read_tlv(context, &have_certificate);
		ਜੇ(rc){
			log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, corrupt PROXY header.",
					context->ਪਤਾ, context->remote_ਪੋਰਟ);
			proxy_cleanup(context);
			ਵਾਪਸ MOSQ_ERR_PROXY;
		}
		mosquitto_FREE(context->proxy.buf);

		ਜੇ(context->listener->proxy_protocol_v2_require_tls && !context->proxy.have_tls){
			log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not connect using TLS.",
					context->ਪਤਾ, context->remote_ਪੋਰਟ);
			proxy_cleanup(context);
			ਵਾਪਸ MOSQ_ERR_PROXY;
		}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
		ਜੇ(context->listener->require_certificate){
			ਜੇ(!have_certificate){
				log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not provide a certificate.",
						context->ਪਤਾ, context->remote_ਪੋਰਟ);
				proxy_cleanup(context);
				ਵਾਪਸ MOSQ_ERR_PROXY;
			}
		}

		ਜੇ(context->proxy.tls_ਸੰਸਕਰਣ && context->proxy.cipher){
			log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Connection from %s:%d negotiated %s cipher %s",
					context->ਪਤਾ, context->remote_ਪੋਰਟ, context->proxy.tls_ਸੰਸਕਰਣ, context->proxy.cipher);
		}
#ਜੇ_ਅੰਤ
		proxy_cleanup(context);

#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
		ਜੇ(context->listener->protocol == mp_websockets){
			ਵਾਪਸ http__context_init(context);
		}ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		{
			context->transport = mosq_t_tcp;
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}
#ਜੇ_ਅੰਤ
