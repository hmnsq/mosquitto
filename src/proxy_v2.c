#ifdef WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#അല്ലെങ്കിൽ
#  include <arpa/inet.h>
#  include <netinet/in.h>
#endif
#അന്തര്ഭാവ <stdint.h>
#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto_internal.h"
#അന്തര്ഭാവ "net_mosq.h"

#എങ്കിൽ !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN

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
#define PP2_SUBTYPE_SSL_താക്കോൽ_ALG 0x25
#define PP2_TYPE_NETNS          0x30

#define PP2_CLIENT_SSL           0x01
#define PP2_CLIENT_CERT_CONN     0x02
#define PP2_CLIENT_CERT_SESS     0x04

#define PROXY_PACKET_LIMIT 500

ഘടന proxy_hdr_v2 {
	uint8_t sig[12]; /* hex 0D 0A 0D 0A 00 0D 0A 51 55 49 54 0A */
	uint8_t ver_cmd; /* protocol version and command */
	uint8_t fam; /* protocol family and address */
	uint16_t len; /* number of following bytes part of the header */
};

സംഘം proxy_addr {
	ഘടന { /* for TCP/UDP over IPv4, len = 12 */
		uint32_t src_addr;
		uint32_t dst_addr;
		uint16_t src_port;
		uint16_t dst_port;
	} ipv4_addr;
	ഘടന { /* for TCP/UDP over IPv6, len = 36 */
		uint8_t src_addr[16];
		uint8_t dst_addr[16];
		uint16_t src_port;
		uint16_t dst_port;
	} ipv6_addr;
	ഘടന { /* for AF_UNIX sockets, len = 216 */
		uint8_t src_addr[108];
		uint8_t dst_addr[108];
	} unix_addr;
};

ഘടന pp2_tlv {
	uint8_t type;
	uint8_t length_h;
	uint8_t length_l;
};

ഘടന pp2_tlv_ssl {
	uint8_t client;
	uint32_t verify;
};

സ്ഥിരം uint8_t signature[12] = {0x0D, 0x0A, 0x0D, 0x0A, 0x00, 0x0D, 0x0A, 0x51, 0x55, 0x49, 0x54, 0x0A};


സ്ഥിതം ശൂന്യം proxy_cleanup(ഘടന mosquitto *context)
{
	mosquitto_FREE(context->proxy.buf);
	mosquitto_FREE(context->proxy.tls_version);
	mosquitto_FREE(context->proxy.cipher);
}


സ്ഥിതം പൂർണ്ണസംഖ്യ read_tlv_ssl(ഘടന mosquitto *context, uint16_t len, bool *have_certificate)
{
	ഘടന pp2_tlv_ssl ssl = {0};

	എങ്കിൽ(len < വലുപ്പം(uint8_t) + വലുപ്പം(uint32_t)){
		തിരികെ MOSQ_ERR_INVAL;
	}

	ssl.client = context->proxy.buf[context->proxy.pos];
	ssl.verify = ntohl(*(uint32_t *)(&context->proxy.buf[context->proxy.pos + വലുപ്പം(uint8_t)]));
	context->proxy.pos = (uint16_t)(context->proxy.pos + വലുപ്പം(uint8_t) + വലുപ്പം(uint32_t));
	len = (uint16_t)(len - (വലുപ്പം(uint8_t) + വലുപ്പം(uint32_t)));

	എങ്കിൽ(ssl.client & PP2_CLIENT_SSL && ssl.client & PP2_CLIENT_CERT_SESS && !ssl.verify){
		*have_certificate = true;
	}

	ആകുമ്പോൾ(len > 0){
		എങ്കിൽ(context->proxy.len - context->proxy.pos < (പൂർണ്ണസംഖ്യ)വലുപ്പം(ഘടന pp2_tlv)){
			തിരികെ MOSQ_ERR_INVAL;
		}
		ഘടന pp2_tlv *tlv = (ഘടന pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->length_h<<8) + tlv->length_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + വലുപ്പം(ഘടന pp2_tlv));

		എങ്കിൽ(tlv_len > context->proxy.len - context->proxy.pos){
			തിരികെ MOSQ_ERR_INVAL;
		}

		തിരഞ്ഞെടുപ്പ്(tlv->type){
			അവസ്ഥ PP2_SUBTYPE_SSL_VERSION:
#ifdef WITH_TLS
				mosquitto_free(context->proxy.tls_version);
				context->proxy.tls_version = mosquitto_strndup((സ്ഥിരം അക്ഷരം *)&context->proxy.buf[context->proxy.pos], tlv_len);
#അല്ലെങ്കിൽ
				തിരികെ MOSQ_ERR_NOT_SUPPORTED;
#endif
				നിർത്തുക;

			അവസ്ഥ PP2_SUBTYPE_SSL_CIPHER:
#ifdef WITH_TLS
				mosquitto_free(context->proxy.cipher);
				context->proxy.cipher = mosquitto_strndup((സ്ഥിരം അക്ഷരം *)&context->proxy.buf[context->proxy.pos], tlv_len);
#അല്ലെങ്കിൽ
				തിരികെ MOSQ_ERR_NOT_SUPPORTED;
#endif
				നിർത്തുക;

			അവസ്ഥ PP2_SUBTYPE_SSL_CN:
#ifdef WITH_TLS
				എങ്കിൽ(context->listener->use_identity_as_username){
					mosquitto_free(context->username);
					context->username = mosquitto_strndup((സ്ഥിരം അക്ഷരം *)&context->proxy.buf[context->proxy.pos], tlv_len);
					എങ്കിൽ(!context->username){
						തിരികെ MOSQ_ERR_NOMEM;
					}
				}
#അല്ലെങ്കിൽ
				തിരികെ MOSQ_ERR_NOT_SUPPORTED;
#endif
				നിർത്തുക;
		}
		len = (uint16_t)(len - (വലുപ്പം(uint8_t) + വലുപ്പം(uint8_t) + വലുപ്പം(uint8_t) + tlv_len));
		context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
	}
	context->proxy.have_tls = true;

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ read_tlv(ഘടന mosquitto *context, bool *have_certificate)
{
	ആകുമ്പോൾ(context->proxy.pos < context->proxy.len){
		എങ്കിൽ(context->proxy.len - context->proxy.pos < (പൂർണ്ണസംഖ്യ)വലുപ്പം(ഘടന pp2_tlv)){
			തിരികെ MOSQ_ERR_INVAL;
		}
		ഘടന pp2_tlv *tlv = (ഘടന pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->length_h<<8) + tlv->length_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + വലുപ്പം(ഘടന pp2_tlv));

		എങ്കിൽ(tlv_len > context->proxy.len - context->proxy.pos){
			തിരികെ MOSQ_ERR_INVAL;
		}

		തിരഞ്ഞെടുപ്പ്(tlv->type){
			അവസ്ഥ PP2_TYPE_SSL:
				{
					പൂർണ്ണസംഖ്യ rc = read_tlv_ssl(context, tlv_len, have_certificate);
					എങ്കിൽ(rc){
						തിരികെ rc;
					}
				}
				നിർത്തുക;
			സ്ഥിരസ്ഥിതി:
				context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
				നിർത്തുക;
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ proxy_v2__read(ഘടന mosquitto *context)
{
	ഘടന proxy_hdr_v2 hdr;

	എങ്കിൽ(context->proxy.cmd == -1){
		context->proxy.buf = ശൂന്യ;
		എങ്കിൽ(net__read(context, &hdr, വലുപ്പം(hdr)) != വലുപ്പം(hdr)){
			തിരികെ MOSQ_ERR_CONN_LOST;
		}
		എങ്കിൽ(memcmp(hdr.sig, signature, വലുപ്പം(signature))){
			തിരികെ MOSQ_ERR_INVAL;
		}
		എങ്കിൽ((hdr.ver_cmd & 0xF0) != 0x20){
			തിരികെ MOSQ_ERR_INVAL;
		}
		context->proxy.cmd = hdr.ver_cmd & 0x0F;
		എങ്കിൽ(context->proxy.cmd != 0x00 && context->proxy.cmd != 0x01){
			തിരികെ MOSQ_ERR_INVAL;
		}
		context->proxy.fam = hdr.fam;
		എങ്കിൽ((hdr.fam != 0x00 || context->proxy.cmd != PROXY_CMD_LOCAL)
				&& hdr.fam != PROXY_TCP_IPV4
				&& hdr.fam != PROXY_TCP_IPV6
				&& hdr.fam != PROXY_TCP_UNIX){

			തിരികെ 1;
		}
		context->proxy.pos = 0;
		context->proxy.len = ntohs(hdr.len);
		എങ്കിൽ(context->proxy.len > 0){
			/* PROXY_PACKET_LIMIT=500 bytes, arbitrary upper limit */
			തിരഞ്ഞെടുപ്പ്(context->proxy.fam){
				അവസ്ഥ PROXY_TCP_IPV4:
					എങ്കിൽ(context->proxy.len < 12 || context->proxy.len > PROXY_PACKET_LIMIT){
						തിരികെ MOSQ_ERR_INVAL;
					}
					നിർത്തുക;
				അവസ്ഥ PROXY_TCP_IPV6:
					എങ്കിൽ(context->proxy.len < 36 || context->proxy.len > PROXY_PACKET_LIMIT){
						തിരികെ MOSQ_ERR_INVAL;
					}
					നിർത്തുക;
				അവസ്ഥ PROXY_TCP_UNIX:
					എങ്കിൽ(context->proxy.len > PROXY_PACKET_LIMIT){
						തിരികെ MOSQ_ERR_INVAL;
					}
					നിർത്തുക;
			}
			context->proxy.buf = mosquitto_calloc(1, (size_t)(context->proxy.len+1));
			എങ്കിൽ(!context->proxy.buf){
				തിരികെ MOSQ_ERR_NOMEM;
			}
		}അല്ലെങ്കിൽ{
			എങ്കിൽ(context->proxy.cmd != PROXY_CMD_LOCAL || context->proxy.fam != 0x00){
				തിരികെ MOSQ_ERR_PROTOCOL;
			}
		}

	}

	എങ്കിൽ(context->proxy.pos < context->proxy.len){
		ssize_t rc = net__read(context, context->proxy.buf, (size_t)(context->proxy.len - context->proxy.pos));
		എങ്കിൽ(rc > 0){
			context->proxy.pos = (uint16_t)(context->proxy.pos + rc);
		}അല്ലെങ്കിൽ{
			proxy_cleanup(context);
			തിരികെ MOSQ_ERR_CONN_LOST;
		}
	}
	എങ്കിൽ(context->proxy.pos == context->proxy.len){
		എങ്കിൽ(context->proxy.fam == PROXY_TCP_IPV4){
			അക്ഷരം address[100];
			സംഘം proxy_addr *addr = (സംഘം proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET, &addr->ipv4_addr.src_addr, address, വലുപ്പം(address));
			context->address = mosquitto_strdup(address);
			context->remote_port = ntohs(addr->ipv4_addr.src_port);
			context->proxy.pos = 4+4+2+2;
		}അല്ലെങ്കിൽ എങ്കിൽ(context->proxy.fam == PROXY_TCP_IPV6){
			അക്ഷരം address[100];
			സംഘം proxy_addr *addr = (സംഘം proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET6, addr->ipv6_addr.src_addr, address, വലുപ്പം(address));
			context->address = mosquitto_strdup(address);
			context->remote_port = ntohs(addr->ipv6_addr.src_port);
			context->proxy.pos = 16+16+2+2;
		}അല്ലെങ്കിൽ എങ്കിൽ(context->proxy.fam == PROXY_TCP_UNIX){
			സംഘം proxy_addr *addr = (സംഘം proxy_addr *)context->proxy.buf;
			context->address = mosquitto_strndup((അക്ഷരം *)addr->unix_addr.src_addr, വലുപ്പം(addr->unix_addr.src_addr));
			context->remote_port = 0;
			context->proxy.pos = (uint16_t)(വലുപ്പം(addr->unix_addr.src_addr) + 1);
		}അല്ലെങ്കിൽ{
			/* Must be LOCAL */
			/* Ignore address */
			context->address = ശൂന്യ;
			context->remote_port = 0;
			proxy_cleanup(context);
			തിരികെ MOSQ_ERR_PROXY;
		}
		എങ്കിൽ(!context->address){
			proxy_cleanup(context);
			തിരികെ MOSQ_ERR_NOMEM;
		}

		bool have_certificate = false;

		പൂർണ്ണസംഖ്യ rc = read_tlv(context, &have_certificate);
		എങ്കിൽ(rc){
			log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, corrupt PROXY header.",
					context->address, context->remote_port);
			proxy_cleanup(context);
			തിരികെ MOSQ_ERR_PROXY;
		}
		mosquitto_FREE(context->proxy.buf);

		എങ്കിൽ(context->listener->proxy_protocol_v2_require_tls && !context->proxy.have_tls){
			log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not connect using TLS.",
					context->address, context->remote_port);
			proxy_cleanup(context);
			തിരികെ MOSQ_ERR_PROXY;
		}

#ifdef WITH_TLS
		എങ്കിൽ(context->listener->require_certificate){
			എങ്കിൽ(!have_certificate){
				log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not provide a certificate.",
						context->address, context->remote_port);
				proxy_cleanup(context);
				തിരികെ MOSQ_ERR_PROXY;
			}
		}

		എങ്കിൽ(context->proxy.tls_version && context->proxy.cipher){
			log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Connection from %s:%d negotiated %s cipher %s",
					context->address, context->remote_port, context->proxy.tls_version, context->proxy.cipher);
		}
#endif
		proxy_cleanup(context);

#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
		എങ്കിൽ(context->listener->protocol == mp_websockets){
			തിരികെ http__context_init(context);
		}അല്ലെങ്കിൽ
#endif
		{
			context->transport = mosq_t_tcp;
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}
#endif
