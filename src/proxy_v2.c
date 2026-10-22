#که_تعریف_شوی WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#وإلا
#  include <arpa/inet.h>
#  include <netinet/in.h>
#د_که_پای
#ضمن <stdint.h>
#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto_internal.h"
#ضمن "net_mosq.h"

#إذا !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN

#تعریف_کړه PROXY_CMD_LOCAL 0x00
#تعریف_کړه PROXY_CMD_PROXY 0x01

#تعریف_کړه PROXY_TCP_IPV4 0x11
#تعریف_کړه PROXY_TCP_IPV6 0x21
#تعریف_کړه PROXY_TCP_UNIX 0x31

#تعریف_کړه PP2_TYPE_ALPN           0x01
#تعریف_کړه PP2_TYPE_AUTHORITY      0x02
#تعریف_کړه PP2_TYPE_دوراني_ازموینه_32_C         0x03
#تعریف_کړه PP2_TYPE_NOOP           0x04
#تعریف_کړه PP2_TYPE_UNIQUE_ID      0x05
#تعریف_کړه PP2_TYPE_SSL            0x20
#تعریف_کړه PP2_SUBTYPE_SSL_نسخه 0x21
#تعریف_کړه PP2_SUBTYPE_SSL_CN      0x22
#تعریف_کړه PP2_SUBTYPE_SSL_CIPHER  0x23
#تعریف_کړه PP2_SUBTYPE_SSL_SIG_ALG 0x24
#تعریف_کړه PP2_SUBTYPE_SSL_کلی_ALG 0x25
#تعریف_کړه PP2_TYPE_NETNS          0x30

#تعریف_کړه PP2_CLIENT_SSL           0x01
#تعریف_کړه PP2_CLIENT_CERT_CONN     0x02
#تعریف_کړه PP2_CLIENT_CERT_SESS     0x04

#تعریف_کړه PROXY_PACKET_حد 500

بنية proxy_hdr_v2 {
	uint8_t sig[12]; /* hex 0D 0A 0D 0A 00 0D 0A 51 55 49 54 0A */
	uint8_t ver_cmd; /* protocol version and command */
	uint8_t fam; /* protocol family and address */
	uint16_t len; /* number of following bytes part of the header */
};

اتحاد proxy_addr {
	بنية { /* for TCP/UDP over IPv4, len = 12 */
		uint32_t src_addr;
		uint32_t dst_addr;
		uint16_t src_پورټ;
		uint16_t dst_پورټ;
	} ipv4_addr;
	بنية { /* for TCP/UDP over IPv6, len = 36 */
		uint8_t src_addr[16];
		uint8_t dst_addr[16];
		uint16_t src_پورټ;
		uint16_t dst_پورټ;
	} ipv6_addr;
	بنية { /* for AF_UNIX sockets, len = 216 */
		uint8_t src_addr[108];
		uint8_t dst_addr[108];
	} unix_addr;
};

بنية pp2_tlv {
	uint8_t type;
	uint8_t اوږدتیا_h;
	uint8_t اوږدتیا_l;
};

بنية pp2_tlv_ssl {
	uint8_t client;
	uint32_t verify;
};

ثابت uint8_t signature[12] = {0x0D, 0x0A, 0x0D, 0x0A, 0x00, 0x0D, 0x0A, 0x51, 0x55, 0x49, 0x54, 0x0A};


ساكن فراغ proxy_cleanup(بنية mosquitto *context)
{
	mosquitto_FREE(context->proxy.buf);
	mosquitto_FREE(context->proxy.tls_نسخه);
	mosquitto_FREE(context->proxy.cipher);
}


ساكن صحيح read_tlv_ssl(بنية mosquitto *context, uint16_t len, bool *have_certificate)
{
	بنية pp2_tlv_ssl ssl = {0};

	إذا(len < حجم(uint8_t) + حجم(uint32_t)){
		أرجع MOSQ_ERR_INVAL;
	}

	ssl.client = context->proxy.buf[context->proxy.pos];
	ssl.verify = ntohl(*(uint32_t *)(&context->proxy.buf[context->proxy.pos + حجم(uint8_t)]));
	context->proxy.pos = (uint16_t)(context->proxy.pos + حجم(uint8_t) + حجم(uint32_t));
	len = (uint16_t)(len - (حجم(uint8_t) + حجم(uint32_t)));

	إذا(ssl.client & PP2_CLIENT_SSL && ssl.client & PP2_CLIENT_CERT_SESS && !ssl.verify){
		*have_certificate = true;
	}

	طالما(len > 0){
		إذا(context->proxy.len - context->proxy.pos < (صحيح)حجم(بنية pp2_tlv)){
			أرجع MOSQ_ERR_INVAL;
		}
		بنية pp2_tlv *tlv = (بنية pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->اوږدتیا_h<<8) + tlv->اوږدتیا_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + حجم(بنية pp2_tlv));

		إذا(tlv_len > context->proxy.len - context->proxy.pos){
			أرجع MOSQ_ERR_INVAL;
		}

		بدل(tlv->type){
			حالة PP2_SUBTYPE_SSL_نسخه:
#که_تعریف_شوی WITH_TLS
				mosquitto_free(context->proxy.tls_نسخه);
				context->proxy.tls_نسخه = mosquitto_strndup((ثابت حرف *)&context->proxy.buf[context->proxy.pos], tlv_len);
#وإلا
				أرجع MOSQ_ERR_NOT_SUPPORTED;
#د_که_پای
				اكسر;

			حالة PP2_SUBTYPE_SSL_CIPHER:
#که_تعریف_شوی WITH_TLS
				mosquitto_free(context->proxy.cipher);
				context->proxy.cipher = mosquitto_strndup((ثابت حرف *)&context->proxy.buf[context->proxy.pos], tlv_len);
#وإلا
				أرجع MOSQ_ERR_NOT_SUPPORTED;
#د_که_پای
				اكسر;

			حالة PP2_SUBTYPE_SSL_CN:
#که_تعریف_شوی WITH_TLS
				إذا(context->listener->use_بې_اغېزه_عنصر_as_username){
					mosquitto_free(context->username);
					context->username = mosquitto_strndup((ثابت حرف *)&context->proxy.buf[context->proxy.pos], tlv_len);
					إذا(!context->username){
						أرجع MOSQ_ERR_NOMEM;
					}
				}
#وإلا
				أرجع MOSQ_ERR_NOT_SUPPORTED;
#د_که_پای
				اكسر;
		}
		len = (uint16_t)(len - (حجم(uint8_t) + حجم(uint8_t) + حجم(uint8_t) + tlv_len));
		context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
	}
	context->proxy.have_tls = true;

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح read_tlv(بنية mosquitto *context, bool *have_certificate)
{
	طالما(context->proxy.pos < context->proxy.len){
		إذا(context->proxy.len - context->proxy.pos < (صحيح)حجم(بنية pp2_tlv)){
			أرجع MOSQ_ERR_INVAL;
		}
		بنية pp2_tlv *tlv = (بنية pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->اوږدتیا_h<<8) + tlv->اوږدتیا_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + حجم(بنية pp2_tlv));

		إذا(tlv_len > context->proxy.len - context->proxy.pos){
			أرجع MOSQ_ERR_INVAL;
		}

		بدل(tlv->type){
			حالة PP2_TYPE_SSL:
				{
					صحيح rc = read_tlv_ssl(context, tlv_len, have_certificate);
					إذا(rc){
						أرجع rc;
					}
				}
				اكسر;
			افتراضي:
				context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
				اكسر;
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح proxy_v2__read(بنية mosquitto *context)
{
	بنية proxy_hdr_v2 hdr;

	إذا(context->proxy.cmd == -1){
		context->proxy.buf = عدم;
		إذا(net__read(context, &hdr, حجم(hdr)) != حجم(hdr)){
			أرجع MOSQ_ERR_CONN_ورک;
		}
		إذا(memcmp(hdr.sig, signature, حجم(signature))){
			أرجع MOSQ_ERR_INVAL;
		}
		إذا((hdr.ver_cmd & 0xF0) != 0x20){
			أرجع MOSQ_ERR_INVAL;
		}
		context->proxy.cmd = hdr.ver_cmd & 0x0F;
		إذا(context->proxy.cmd != 0x00 && context->proxy.cmd != 0x01){
			أرجع MOSQ_ERR_INVAL;
		}
		context->proxy.fam = hdr.fam;
		إذا((hdr.fam != 0x00 || context->proxy.cmd != PROXY_CMD_LOCAL)
				&& hdr.fam != PROXY_TCP_IPV4
				&& hdr.fam != PROXY_TCP_IPV6
				&& hdr.fam != PROXY_TCP_UNIX){

			أرجع 1;
		}
		context->proxy.pos = 0;
		context->proxy.len = ntohs(hdr.len);
		إذا(context->proxy.len > 0){
			/* PROXY_PACKET_LIMIT=500 bytes, arbitrary upper limit */
			بدل(context->proxy.fam){
				حالة PROXY_TCP_IPV4:
					إذا(context->proxy.len < 12 || context->proxy.len > PROXY_PACKET_حد){
						أرجع MOSQ_ERR_INVAL;
					}
					اكسر;
				حالة PROXY_TCP_IPV6:
					إذا(context->proxy.len < 36 || context->proxy.len > PROXY_PACKET_حد){
						أرجع MOSQ_ERR_INVAL;
					}
					اكسر;
				حالة PROXY_TCP_UNIX:
					إذا(context->proxy.len > PROXY_PACKET_حد){
						أرجع MOSQ_ERR_INVAL;
					}
					اكسر;
			}
			context->proxy.buf = mosquitto_calloc(1, (size_t)(context->proxy.len+1));
			إذا(!context->proxy.buf){
				أرجع MOSQ_ERR_NOMEM;
			}
		}وإلا{
			إذا(context->proxy.cmd != PROXY_CMD_LOCAL || context->proxy.fam != 0x00){
				أرجع MOSQ_ERR_PROTOCOL;
			}
		}

	}

	إذا(context->proxy.pos < context->proxy.len){
		ssize_t rc = net__read(context, context->proxy.buf, (size_t)(context->proxy.len - context->proxy.pos));
		إذا(rc > 0){
			context->proxy.pos = (uint16_t)(context->proxy.pos + rc);
		}وإلا{
			proxy_cleanup(context);
			أرجع MOSQ_ERR_CONN_ورک;
		}
	}
	إذا(context->proxy.pos == context->proxy.len){
		إذا(context->proxy.fam == PROXY_TCP_IPV4){
			حرف پته[100];
			اتحاد proxy_addr *addr = (اتحاد proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET, &addr->ipv4_addr.src_addr, پته, حجم(پته));
			context->پته = mosquitto_strdup(پته);
			context->remote_پورټ = ntohs(addr->ipv4_addr.src_پورټ);
			context->proxy.pos = 4+4+2+2;
		}وإلا إذا(context->proxy.fam == PROXY_TCP_IPV6){
			حرف پته[100];
			اتحاد proxy_addr *addr = (اتحاد proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET6, addr->ipv6_addr.src_addr, پته, حجم(پته));
			context->پته = mosquitto_strdup(پته);
			context->remote_پورټ = ntohs(addr->ipv6_addr.src_پورټ);
			context->proxy.pos = 16+16+2+2;
		}وإلا إذا(context->proxy.fam == PROXY_TCP_UNIX){
			اتحاد proxy_addr *addr = (اتحاد proxy_addr *)context->proxy.buf;
			context->پته = mosquitto_strndup((حرف *)addr->unix_addr.src_addr, حجم(addr->unix_addr.src_addr));
			context->remote_پورټ = 0;
			context->proxy.pos = (uint16_t)(حجم(addr->unix_addr.src_addr) + 1);
		}وإلا{
			/* Must be LOCAL */
			/* Ignore address */
			context->پته = عدم;
			context->remote_پورټ = 0;
			proxy_cleanup(context);
			أرجع MOSQ_ERR_PROXY;
		}
		إذا(!context->پته){
			proxy_cleanup(context);
			أرجع MOSQ_ERR_NOMEM;
		}

		bool have_certificate = false;

		صحيح rc = read_tlv(context, &have_certificate);
		إذا(rc){
			log__printf(عدم, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, corrupt PROXY header.",
					context->پته, context->remote_پورټ);
			proxy_cleanup(context);
			أرجع MOSQ_ERR_PROXY;
		}
		mosquitto_FREE(context->proxy.buf);

		إذا(context->listener->proxy_protocol_v2_require_tls && !context->proxy.have_tls){
			log__printf(عدم, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not connect using TLS.",
					context->پته, context->remote_پورټ);
			proxy_cleanup(context);
			أرجع MOSQ_ERR_PROXY;
		}

#که_تعریف_شوی WITH_TLS
		إذا(context->listener->require_certificate){
			إذا(!have_certificate){
				log__printf(عدم, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not provide a certificate.",
						context->پته, context->remote_پورټ);
				proxy_cleanup(context);
				أرجع MOSQ_ERR_PROXY;
			}
		}

		إذا(context->proxy.tls_نسخه && context->proxy.cipher){
			log__printf(عدم, MOSQ_LOG_NOTICE, "Connection from %s:%d negotiated %s cipher %s",
					context->پته, context->remote_پورټ, context->proxy.tls_نسخه, context->proxy.cipher);
		}
#د_که_پای
		proxy_cleanup(context);

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
		إذا(context->listener->protocol == mp_websockets){
			أرجع http__context_init(context);
		}وإلا
#د_که_پای
		{
			context->transport = mosq_t_tcp;
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}
#د_که_پای
