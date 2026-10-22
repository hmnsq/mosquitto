#اگر_تعریف_شدہ WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#ورنہ
#  include <arpa/inet.h>
#  include <netinet/in.h>
#اگر_ختم
#شامل <stdint.h>
#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto_internal.h"
#شامل "net_mosq.h"

#اگر !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN

#تعریف_کرو PROXY_CMD_LOCAL 0x00
#تعریف_کرو PROXY_CMD_PROXY 0x01

#تعریف_کرو PROXY_TCP_IPV4 0x11
#تعریف_کرو PROXY_TCP_IPV6 0x21
#تعریف_کرو PROXY_TCP_UNIX 0x31

#تعریف_کرو PP2_TYPE_ALPN           0x01
#تعریف_کرو PP2_TYPE_AUTHORITY      0x02
#تعریف_کرو PP2_TYPE_چکری_جانچ_32_C         0x03
#تعریف_کرو PP2_TYPE_NOOP           0x04
#تعریف_کرو PP2_TYPE_UNIQUE_ID      0x05
#تعریف_کرو PP2_TYPE_SSL            0x20
#تعریف_کرو PP2_SUBTYPE_SSL_نسخہ 0x21
#تعریف_کرو PP2_SUBTYPE_SSL_CN      0x22
#تعریف_کرو PP2_SUBTYPE_SSL_CIPHER  0x23
#تعریف_کرو PP2_SUBTYPE_SSL_SIG_ALG 0x24
#تعریف_کرو PP2_SUBTYPE_SSL_کلید_ALG 0x25
#تعریف_کرو PP2_TYPE_NETNS          0x30

#تعریف_کرو PP2_CLIENT_SSL           0x01
#تعریف_کرو PP2_CLIENT_CERT_CONN     0x02
#تعریف_کرو PP2_CLIENT_CERT_SESS     0x04

#تعریف_کرو PROXY_PACKET_حد_بندی 500

ساخت proxy_hdr_v2 {
	uint8_t sig[12]; /* hex 0D 0A 0D 0A 00 0D 0A 51 55 49 54 0A */
	uint8_t ver_cmd; /* protocol version and command */
	uint8_t fam; /* protocol family and address */
	uint16_t len; /* number of following bytes part of the header */
};

اتحاد proxy_addr {
	ساخت { /* for TCP/UDP over IPv4, len = 12 */
		uint32_t src_addr;
		uint32_t dst_addr;
		uint16_t src_پورٹ;
		uint16_t dst_پورٹ;
	} ipv4_addr;
	ساخت { /* for TCP/UDP over IPv6, len = 36 */
		uint8_t src_addr[16];
		uint8_t dst_addr[16];
		uint16_t src_پورٹ;
		uint16_t dst_پورٹ;
	} ipv6_addr;
	ساخت { /* for AF_UNIX sockets, len = 216 */
		uint8_t src_addr[108];
		uint8_t dst_addr[108];
	} unix_addr;
};

ساخت pp2_tlv {
	uint8_t type;
	uint8_t لمبائی_h;
	uint8_t لمبائی_l;
};

ساخت pp2_tlv_ssl {
	uint8_t client;
	uint32_t verify;
};

ثابت uint8_t signature[12] = {0x0D, 0x0A, 0x0D, 0x0A, 0x00, 0x0D, 0x0A, 0x51, 0x55, 0x49, 0x54, 0x0A};


ساکن خالی proxy_cleanup(ساخت mosquitto *context)
{
	mosquitto_FREE(context->proxy.buf);
	mosquitto_FREE(context->proxy.tls_نسخہ);
	mosquitto_FREE(context->proxy.cipher);
}


ساکن صحیح read_tlv_ssl(ساخت mosquitto *context, uint16_t len, bool *have_certificate)
{
	ساخت pp2_tlv_ssl ssl = {0};

	اگر(len < حجم(uint8_t) + حجم(uint32_t)){
		واپس MOSQ_ERR_INVAL;
	}

	ssl.client = context->proxy.buf[context->proxy.pos];
	ssl.verify = ntohl(*(uint32_t *)(&context->proxy.buf[context->proxy.pos + حجم(uint8_t)]));
	context->proxy.pos = (uint16_t)(context->proxy.pos + حجم(uint8_t) + حجم(uint32_t));
	len = (uint16_t)(len - (حجم(uint8_t) + حجم(uint32_t)));

	اگر(ssl.client & PP2_CLIENT_SSL && ssl.client & PP2_CLIENT_CERT_SESS && !ssl.verify){
		*have_certificate = true;
	}

	جب(len > 0){
		اگر(context->proxy.len - context->proxy.pos < (صحیح)حجم(ساخت pp2_tlv)){
			واپس MOSQ_ERR_INVAL;
		}
		ساخت pp2_tlv *tlv = (ساخت pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->لمبائی_h<<8) + tlv->لمبائی_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + حجم(ساخت pp2_tlv));

		اگر(tlv_len > context->proxy.len - context->proxy.pos){
			واپس MOSQ_ERR_INVAL;
		}

		انتخاب(tlv->type){
			حالت PP2_SUBTYPE_SSL_نسخہ:
#اگر_تعریف_شدہ WITH_TLS
				mosquitto_free(context->proxy.tls_نسخہ);
				context->proxy.tls_نسخہ = mosquitto_strndup((ثابت حرف *)&context->proxy.buf[context->proxy.pos], tlv_len);
#ورنہ
				واپس MOSQ_ERR_NOT_SUPPORTED;
#اگر_ختم
				توڑو;

			حالت PP2_SUBTYPE_SSL_CIPHER:
#اگر_تعریف_شدہ WITH_TLS
				mosquitto_free(context->proxy.cipher);
				context->proxy.cipher = mosquitto_strndup((ثابت حرف *)&context->proxy.buf[context->proxy.pos], tlv_len);
#ورنہ
				واپس MOSQ_ERR_NOT_SUPPORTED;
#اگر_ختم
				توڑو;

			حالت PP2_SUBTYPE_SSL_CN:
#اگر_تعریف_شدہ WITH_TLS
				اگر(context->listener->use_شناختی_عنصر_as_username){
					mosquitto_free(context->username);
					context->username = mosquitto_strndup((ثابت حرف *)&context->proxy.buf[context->proxy.pos], tlv_len);
					اگر(!context->username){
						واپس MOSQ_ERR_NOMEM;
					}
				}
#ورنہ
				واپس MOSQ_ERR_NOT_SUPPORTED;
#اگر_ختم
				توڑو;
		}
		len = (uint16_t)(len - (حجم(uint8_t) + حجم(uint8_t) + حجم(uint8_t) + tlv_len));
		context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
	}
	context->proxy.have_tls = true;

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح read_tlv(ساخت mosquitto *context, bool *have_certificate)
{
	جب(context->proxy.pos < context->proxy.len){
		اگر(context->proxy.len - context->proxy.pos < (صحیح)حجم(ساخت pp2_tlv)){
			واپس MOSQ_ERR_INVAL;
		}
		ساخت pp2_tlv *tlv = (ساخت pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->لمبائی_h<<8) + tlv->لمبائی_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + حجم(ساخت pp2_tlv));

		اگر(tlv_len > context->proxy.len - context->proxy.pos){
			واپس MOSQ_ERR_INVAL;
		}

		انتخاب(tlv->type){
			حالت PP2_TYPE_SSL:
				{
					صحیح rc = read_tlv_ssl(context, tlv_len, have_certificate);
					اگر(rc){
						واپس rc;
					}
				}
				توڑو;
			طےشدہ:
				context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
				توڑو;
		}
	}

	واپس MOSQ_ERR_SUCCESS;
}


صحیح proxy_v2__read(ساخت mosquitto *context)
{
	ساخت proxy_hdr_v2 hdr;

	اگر(context->proxy.cmd == -1){
		context->proxy.buf = تہی;
		اگر(net__read(context, &hdr, حجم(hdr)) != حجم(hdr)){
			واپس MOSQ_ERR_CONN_گمشدہ;
		}
		اگر(memcmp(hdr.sig, signature, حجم(signature))){
			واپس MOSQ_ERR_INVAL;
		}
		اگر((hdr.ver_cmd & 0xF0) != 0x20){
			واپس MOSQ_ERR_INVAL;
		}
		context->proxy.cmd = hdr.ver_cmd & 0x0F;
		اگر(context->proxy.cmd != 0x00 && context->proxy.cmd != 0x01){
			واپس MOSQ_ERR_INVAL;
		}
		context->proxy.fam = hdr.fam;
		اگر((hdr.fam != 0x00 || context->proxy.cmd != PROXY_CMD_LOCAL)
				&& hdr.fam != PROXY_TCP_IPV4
				&& hdr.fam != PROXY_TCP_IPV6
				&& hdr.fam != PROXY_TCP_UNIX){

			واپس 1;
		}
		context->proxy.pos = 0;
		context->proxy.len = ntohs(hdr.len);
		اگر(context->proxy.len > 0){
			/* PROXY_PACKET_LIMIT=500 bytes, arbitrary upper limit */
			انتخاب(context->proxy.fam){
				حالت PROXY_TCP_IPV4:
					اگر(context->proxy.len < 12 || context->proxy.len > PROXY_PACKET_حد_بندی){
						واپس MOSQ_ERR_INVAL;
					}
					توڑو;
				حالت PROXY_TCP_IPV6:
					اگر(context->proxy.len < 36 || context->proxy.len > PROXY_PACKET_حد_بندی){
						واپس MOSQ_ERR_INVAL;
					}
					توڑو;
				حالت PROXY_TCP_UNIX:
					اگر(context->proxy.len > PROXY_PACKET_حد_بندی){
						واپس MOSQ_ERR_INVAL;
					}
					توڑو;
			}
			context->proxy.buf = mosquitto_calloc(1, (size_t)(context->proxy.len+1));
			اگر(!context->proxy.buf){
				واپس MOSQ_ERR_NOMEM;
			}
		}ورنہ{
			اگر(context->proxy.cmd != PROXY_CMD_LOCAL || context->proxy.fam != 0x00){
				واپس MOSQ_ERR_PROTOCOL;
			}
		}

	}

	اگر(context->proxy.pos < context->proxy.len){
		ssize_t rc = net__read(context, context->proxy.buf, (size_t)(context->proxy.len - context->proxy.pos));
		اگر(rc > 0){
			context->proxy.pos = (uint16_t)(context->proxy.pos + rc);
		}ورنہ{
			proxy_cleanup(context);
			واپس MOSQ_ERR_CONN_گمشدہ;
		}
	}
	اگر(context->proxy.pos == context->proxy.len){
		اگر(context->proxy.fam == PROXY_TCP_IPV4){
			حرف پتہ[100];
			اتحاد proxy_addr *addr = (اتحاد proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET, &addr->ipv4_addr.src_addr, پتہ, حجم(پتہ));
			context->پتہ = mosquitto_strdup(پتہ);
			context->remote_پورٹ = ntohs(addr->ipv4_addr.src_پورٹ);
			context->proxy.pos = 4+4+2+2;
		}ورنہ اگر(context->proxy.fam == PROXY_TCP_IPV6){
			حرف پتہ[100];
			اتحاد proxy_addr *addr = (اتحاد proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET6, addr->ipv6_addr.src_addr, پتہ, حجم(پتہ));
			context->پتہ = mosquitto_strdup(پتہ);
			context->remote_پورٹ = ntohs(addr->ipv6_addr.src_پورٹ);
			context->proxy.pos = 16+16+2+2;
		}ورنہ اگر(context->proxy.fam == PROXY_TCP_UNIX){
			اتحاد proxy_addr *addr = (اتحاد proxy_addr *)context->proxy.buf;
			context->پتہ = mosquitto_strndup((حرف *)addr->unix_addr.src_addr, حجم(addr->unix_addr.src_addr));
			context->remote_پورٹ = 0;
			context->proxy.pos = (uint16_t)(حجم(addr->unix_addr.src_addr) + 1);
		}ورنہ{
			/* Must be LOCAL */
			/* Ignore address */
			context->پتہ = تہی;
			context->remote_پورٹ = 0;
			proxy_cleanup(context);
			واپس MOSQ_ERR_PROXY;
		}
		اگر(!context->پتہ){
			proxy_cleanup(context);
			واپس MOSQ_ERR_NOMEM;
		}

		bool have_certificate = false;

		صحیح rc = read_tlv(context, &have_certificate);
		اگر(rc){
			log__printf(تہی, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, corrupt PROXY header.",
					context->پتہ, context->remote_پورٹ);
			proxy_cleanup(context);
			واپس MOSQ_ERR_PROXY;
		}
		mosquitto_FREE(context->proxy.buf);

		اگر(context->listener->proxy_protocol_v2_require_tls && !context->proxy.have_tls){
			log__printf(تہی, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not connect using TLS.",
					context->پتہ, context->remote_پورٹ);
			proxy_cleanup(context);
			واپس MOSQ_ERR_PROXY;
		}

#اگر_تعریف_شدہ WITH_TLS
		اگر(context->listener->require_certificate){
			اگر(!have_certificate){
				log__printf(تہی, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not provide a certificate.",
						context->پتہ, context->remote_پورٹ);
				proxy_cleanup(context);
				واپس MOSQ_ERR_PROXY;
			}
		}

		اگر(context->proxy.tls_نسخہ && context->proxy.cipher){
			log__printf(تہی, MOSQ_LOG_NOTICE, "Connection from %s:%d negotiated %s cipher %s",
					context->پتہ, context->remote_پورٹ, context->proxy.tls_نسخہ, context->proxy.cipher);
		}
#اگر_ختم
		proxy_cleanup(context);

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
		اگر(context->listener->protocol == mp_websockets){
			واپس http__context_init(context);
		}ورنہ
#اگر_ختم
		{
			context->transport = mosq_t_tcp;
		}
	}

	واپس MOSQ_ERR_SUCCESS;
}
#اگر_ختم
