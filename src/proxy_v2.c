#ifdef WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#وگرنه
#  include <arpa/inet.h>
#  include <netinet/in.h>
#endif
#شامل <stdint.h>
#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto_internal.h"
#شامل "net_mosq.h"

#اگر !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN

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
#define PP2_SUBTYPE_SSL_کلید_ALG 0x25
#define PP2_TYPE_NETNS          0x30

#define PP2_CLIENT_SSL           0x01
#define PP2_CLIENT_CERT_CONN     0x02
#define PP2_CLIENT_CERT_SESS     0x04

#define PROXY_PACKET_LIMIT 500

ساختار proxy_hdr_v2 {
	uint8_t sig[12]; /* hex 0D 0A 0D 0A 00 0D 0A 51 55 49 54 0A */
	uint8_t ver_cmd; /* protocol version and command */
	uint8_t fam; /* protocol family and address */
	uint16_t len; /* number of following bytes part of the header */
};

اجتماع proxy_addr {
	ساختار { /* for TCP/UDP over IPv4, len = 12 */
		uint32_t src_addr;
		uint32_t dst_addr;
		uint16_t src_port;
		uint16_t dst_port;
	} ipv4_addr;
	ساختار { /* for TCP/UDP over IPv6, len = 36 */
		uint8_t src_addr[16];
		uint8_t dst_addr[16];
		uint16_t src_port;
		uint16_t dst_port;
	} ipv6_addr;
	ساختار { /* for AF_UNIX sockets, len = 216 */
		uint8_t src_addr[108];
		uint8_t dst_addr[108];
	} unix_addr;
};

ساختار pp2_tlv {
	uint8_t type;
	uint8_t length_h;
	uint8_t length_l;
};

ساختار pp2_tlv_ssl {
	uint8_t client;
	uint32_t verify;
};

ثابت uint8_t signature[12] = {0x0D, 0x0A, 0x0D, 0x0A, 0x00, 0x0D, 0x0A, 0x51, 0x55, 0x49, 0x54, 0x0A};


ایستا تهی proxy_cleanup(ساختار mosquitto *context)
{
	mosquitto_FREE(context->proxy.buf);
	mosquitto_FREE(context->proxy.tls_version);
	mosquitto_FREE(context->proxy.cipher);
}


ایستا صحیح read_tlv_ssl(ساختار mosquitto *context, uint16_t len, bool *have_certificate)
{
	ساختار pp2_tlv_ssl ssl = {0};

	اگر(len < اندازه(uint8_t) + اندازه(uint32_t)){
		بازگشت MOSQ_ERR_INVAL;
	}

	ssl.client = context->proxy.buf[context->proxy.pos];
	ssl.verify = ntohl(*(uint32_t *)(&context->proxy.buf[context->proxy.pos + اندازه(uint8_t)]));
	context->proxy.pos = (uint16_t)(context->proxy.pos + اندازه(uint8_t) + اندازه(uint32_t));
	len = (uint16_t)(len - (اندازه(uint8_t) + اندازه(uint32_t)));

	اگر(ssl.client & PP2_CLIENT_SSL && ssl.client & PP2_CLIENT_CERT_SESS && !ssl.verify){
		*have_certificate = true;
	}

	وقتی(len > 0){
		اگر(context->proxy.len - context->proxy.pos < (صحیح)اندازه(ساختار pp2_tlv)){
			بازگشت MOSQ_ERR_INVAL;
		}
		ساختار pp2_tlv *tlv = (ساختار pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->length_h<<8) + tlv->length_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + اندازه(ساختار pp2_tlv));

		اگر(tlv_len > context->proxy.len - context->proxy.pos){
			بازگشت MOSQ_ERR_INVAL;
		}

		انتخاب(tlv->type){
			حالت PP2_SUBTYPE_SSL_VERSION:
#ifdef WITH_TLS
				mosquitto_free(context->proxy.tls_version);
				context->proxy.tls_version = mosquitto_strndup((ثابت نویسه *)&context->proxy.buf[context->proxy.pos], tlv_len);
#وگرنه
				بازگشت MOSQ_ERR_NOT_SUPPORTED;
#endif
				بشکن;

			حالت PP2_SUBTYPE_SSL_CIPHER:
#ifdef WITH_TLS
				mosquitto_free(context->proxy.cipher);
				context->proxy.cipher = mosquitto_strndup((ثابت نویسه *)&context->proxy.buf[context->proxy.pos], tlv_len);
#وگرنه
				بازگشت MOSQ_ERR_NOT_SUPPORTED;
#endif
				بشکن;

			حالت PP2_SUBTYPE_SSL_CN:
#ifdef WITH_TLS
				اگر(context->listener->use_identity_as_username){
					mosquitto_free(context->username);
					context->username = mosquitto_strndup((ثابت نویسه *)&context->proxy.buf[context->proxy.pos], tlv_len);
					اگر(!context->username){
						بازگشت MOSQ_ERR_NOMEM;
					}
				}
#وگرنه
				بازگشت MOSQ_ERR_NOT_SUPPORTED;
#endif
				بشکن;
		}
		len = (uint16_t)(len - (اندازه(uint8_t) + اندازه(uint8_t) + اندازه(uint8_t) + tlv_len));
		context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
	}
	context->proxy.have_tls = true;

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح read_tlv(ساختار mosquitto *context, bool *have_certificate)
{
	وقتی(context->proxy.pos < context->proxy.len){
		اگر(context->proxy.len - context->proxy.pos < (صحیح)اندازه(ساختار pp2_tlv)){
			بازگشت MOSQ_ERR_INVAL;
		}
		ساختار pp2_tlv *tlv = (ساختار pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->length_h<<8) + tlv->length_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + اندازه(ساختار pp2_tlv));

		اگر(tlv_len > context->proxy.len - context->proxy.pos){
			بازگشت MOSQ_ERR_INVAL;
		}

		انتخاب(tlv->type){
			حالت PP2_TYPE_SSL:
				{
					صحیح rc = read_tlv_ssl(context, tlv_len, have_certificate);
					اگر(rc){
						بازگشت rc;
					}
				}
				بشکن;
			پیشفرض:
				context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
				بشکن;
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح proxy_v2__read(ساختار mosquitto *context)
{
	ساختار proxy_hdr_v2 hdr;

	اگر(context->proxy.cmd == -1){
		context->proxy.buf = پوچ;
		اگر(net__read(context, &hdr, اندازه(hdr)) != اندازه(hdr)){
			بازگشت MOSQ_ERR_CONN_LOST;
		}
		اگر(memcmp(hdr.sig, signature, اندازه(signature))){
			بازگشت MOSQ_ERR_INVAL;
		}
		اگر((hdr.ver_cmd & 0xF0) != 0x20){
			بازگشت MOSQ_ERR_INVAL;
		}
		context->proxy.cmd = hdr.ver_cmd & 0x0F;
		اگر(context->proxy.cmd != 0x00 && context->proxy.cmd != 0x01){
			بازگشت MOSQ_ERR_INVAL;
		}
		context->proxy.fam = hdr.fam;
		اگر((hdr.fam != 0x00 || context->proxy.cmd != PROXY_CMD_LOCAL)
				&& hdr.fam != PROXY_TCP_IPV4
				&& hdr.fam != PROXY_TCP_IPV6
				&& hdr.fam != PROXY_TCP_UNIX){

			بازگشت 1;
		}
		context->proxy.pos = 0;
		context->proxy.len = ntohs(hdr.len);
		اگر(context->proxy.len > 0){
			/* PROXY_PACKET_LIMIT=500 bytes, arbitrary upper limit */
			انتخاب(context->proxy.fam){
				حالت PROXY_TCP_IPV4:
					اگر(context->proxy.len < 12 || context->proxy.len > PROXY_PACKET_LIMIT){
						بازگشت MOSQ_ERR_INVAL;
					}
					بشکن;
				حالت PROXY_TCP_IPV6:
					اگر(context->proxy.len < 36 || context->proxy.len > PROXY_PACKET_LIMIT){
						بازگشت MOSQ_ERR_INVAL;
					}
					بشکن;
				حالت PROXY_TCP_UNIX:
					اگر(context->proxy.len > PROXY_PACKET_LIMIT){
						بازگشت MOSQ_ERR_INVAL;
					}
					بشکن;
			}
			context->proxy.buf = mosquitto_calloc(1, (size_t)(context->proxy.len+1));
			اگر(!context->proxy.buf){
				بازگشت MOSQ_ERR_NOMEM;
			}
		}وگرنه{
			اگر(context->proxy.cmd != PROXY_CMD_LOCAL || context->proxy.fam != 0x00){
				بازگشت MOSQ_ERR_PROTOCOL;
			}
		}

	}

	اگر(context->proxy.pos < context->proxy.len){
		ssize_t rc = net__read(context, context->proxy.buf, (size_t)(context->proxy.len - context->proxy.pos));
		اگر(rc > 0){
			context->proxy.pos = (uint16_t)(context->proxy.pos + rc);
		}وگرنه{
			proxy_cleanup(context);
			بازگشت MOSQ_ERR_CONN_LOST;
		}
	}
	اگر(context->proxy.pos == context->proxy.len){
		اگر(context->proxy.fam == PROXY_TCP_IPV4){
			نویسه address[100];
			اجتماع proxy_addr *addr = (اجتماع proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET, &addr->ipv4_addr.src_addr, address, اندازه(address));
			context->address = mosquitto_strdup(address);
			context->remote_port = ntohs(addr->ipv4_addr.src_port);
			context->proxy.pos = 4+4+2+2;
		}وگرنه اگر(context->proxy.fam == PROXY_TCP_IPV6){
			نویسه address[100];
			اجتماع proxy_addr *addr = (اجتماع proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET6, addr->ipv6_addr.src_addr, address, اندازه(address));
			context->address = mosquitto_strdup(address);
			context->remote_port = ntohs(addr->ipv6_addr.src_port);
			context->proxy.pos = 16+16+2+2;
		}وگرنه اگر(context->proxy.fam == PROXY_TCP_UNIX){
			اجتماع proxy_addr *addr = (اجتماع proxy_addr *)context->proxy.buf;
			context->address = mosquitto_strndup((نویسه *)addr->unix_addr.src_addr, اندازه(addr->unix_addr.src_addr));
			context->remote_port = 0;
			context->proxy.pos = (uint16_t)(اندازه(addr->unix_addr.src_addr) + 1);
		}وگرنه{
			/* Must be LOCAL */
			/* Ignore address */
			context->address = پوچ;
			context->remote_port = 0;
			proxy_cleanup(context);
			بازگشت MOSQ_ERR_PROXY;
		}
		اگر(!context->address){
			proxy_cleanup(context);
			بازگشت MOSQ_ERR_NOMEM;
		}

		bool have_certificate = false;

		صحیح rc = read_tlv(context, &have_certificate);
		اگر(rc){
			log__printf(پوچ, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, corrupt PROXY header.",
					context->address, context->remote_port);
			proxy_cleanup(context);
			بازگشت MOSQ_ERR_PROXY;
		}
		mosquitto_FREE(context->proxy.buf);

		اگر(context->listener->proxy_protocol_v2_require_tls && !context->proxy.have_tls){
			log__printf(پوچ, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not connect using TLS.",
					context->address, context->remote_port);
			proxy_cleanup(context);
			بازگشت MOSQ_ERR_PROXY;
		}

#ifdef WITH_TLS
		اگر(context->listener->require_certificate){
			اگر(!have_certificate){
				log__printf(پوچ, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not provide a certificate.",
						context->address, context->remote_port);
				proxy_cleanup(context);
				بازگشت MOSQ_ERR_PROXY;
			}
		}

		اگر(context->proxy.tls_version && context->proxy.cipher){
			log__printf(پوچ, MOSQ_LOG_NOTICE, "Connection from %s:%d negotiated %s cipher %s",
					context->address, context->remote_port, context->proxy.tls_version, context->proxy.cipher);
		}
#endif
		proxy_cleanup(context);

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
		اگر(context->listener->protocol == mp_websockets){
			بازگشت http__context_init(context);
		}وگرنه
#endif
		{
			context->transport = mosq_t_tcp;
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
}
#endif
