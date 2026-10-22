#ifdef WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#မဟုတ်ပါက
#  include <arpa/inet.h>
#  include <netinet/in.h>
#endif
#include <stdint.h>
#include "mosquitto_broker_internal.h"
#include "mosquitto_internal.h"
#include "net_mosq.h"

#အကယ်၍ !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN

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
#define PP2_SUBTYPE_SSL_KEY_ALG 0x25
#define PP2_TYPE_NETNS          0x30

#define PP2_CLIENT_SSL           0x01
#define PP2_CLIENT_CERT_CONN     0x02
#define PP2_CLIENT_CERT_SESS     0x04

#define PROXY_PACKET_LIMIT 500

sa_nracanaa proxy_hdr_v2 {
	uint8_t sig[12]; /* hex 0D 0A 0D 0A 00 0D 0A 51 55 49 54 0A */
	uint8_t ver_cmd; /* protocol version and command */
	uint8_t fam; /* protocol family and address */
	uint16_t len; /* number of following bytes part of the header */
};

sa_ngha proxy_addr {
	sa_nracanaa { /* for TCP/UDP over IPv4, len = 12 */
		uint32_t src_addr;
		uint32_t dst_addr;
		uint16_t src_port;
		uint16_t dst_port;
	} ipv4_addr;
	sa_nracanaa { /* for TCP/UDP over IPv6, len = 36 */
		uint8_t src_addr[16];
		uint8_t dst_addr[16];
		uint16_t src_port;
		uint16_t dst_port;
	} ipv6_addr;
	sa_nracanaa { /* for AF_UNIX sockets, len = 216 */
		uint8_t src_addr[108];
		uint8_t dst_addr[108];
	} unix_addr;
};

sa_nracanaa pp2_tlv {
	uint8_t type;
	uint8_t length_h;
	uint8_t length_l;
};

sa_nracanaa pp2_tlv_ssl {
	uint8_t client;
	uint32_t verify;
};

const uint8_t signature[12] = {0x0D, 0x0A, 0x0D, 0x0A, 0x00, 0x0D, 0x0A, 0x51, 0x55, 0x49, 0x54, 0x0A};


static void proxy_cleanup(sa_nracanaa mosquitto *context)
{
	mosquitto_FREE(context->proxy.buf);
	mosquitto_FREE(context->proxy.tls_version);
	mosquitto_FREE(context->proxy.cipher);
}


static ကိန်းပြည့် read_tlv_ssl(sa_nracanaa mosquitto *context, uint16_t len, bool *have_certificate)
{
	sa_nracanaa pp2_tlv_ssl ssl = {0};

	အကယ်၍(len < parimaa_na(uint8_t) + parimaa_na(uint32_t)){
		ပြန်ပေး MOSQ_ERR_INVAL;
	}

	ssl.client = context->proxy.buf[context->proxy.pos];
	ssl.verify = ntohl(*(uint32_t *)(&context->proxy.buf[context->proxy.pos + parimaa_na(uint8_t)]));
	context->proxy.pos = (uint16_t)(context->proxy.pos + parimaa_na(uint8_t) + parimaa_na(uint32_t));
	len = (uint16_t)(len - (parimaa_na(uint8_t) + parimaa_na(uint32_t)));

	အကယ်၍(ssl.client & PP2_CLIENT_SSL && ssl.client & PP2_CLIENT_CERT_SESS && !ssl.verify){
		*have_certificate = true;
	}

	နေစဉ်(len > 0){
		အကယ်၍(context->proxy.len - context->proxy.pos < (ကိန်းပြည့်)parimaa_na(sa_nracanaa pp2_tlv)){
			ပြန်ပေး MOSQ_ERR_INVAL;
		}
		sa_nracanaa pp2_tlv *tlv = (sa_nracanaa pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->length_h<<8) + tlv->length_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + parimaa_na(sa_nracanaa pp2_tlv));

		အကယ်၍(tlv_len > context->proxy.len - context->proxy.pos){
			ပြန်ပေး MOSQ_ERR_INVAL;
		}

		switch(tlv->type){
			pak_sa PP2_SUBTYPE_SSL_VERSION:
#ifdef WITH_TLS
				mosquitto_free(context->proxy.tls_version);
				context->proxy.tls_version = mosquitto_strndup((const var_na *)&context->proxy.buf[context->proxy.pos], tlv_len);
#မဟုတ်ပါက
				ပြန်ပေး MOSQ_ERR_NOT_SUPPORTED;
#endif
				bha_nga;

			pak_sa PP2_SUBTYPE_SSL_CIPHER:
#ifdef WITH_TLS
				mosquitto_free(context->proxy.cipher);
				context->proxy.cipher = mosquitto_strndup((const var_na *)&context->proxy.buf[context->proxy.pos], tlv_len);
#မဟုတ်ပါက
				ပြန်ပေး MOSQ_ERR_NOT_SUPPORTED;
#endif
				bha_nga;

			pak_sa PP2_SUBTYPE_SSL_CN:
#ifdef WITH_TLS
				အကယ်၍(context->listener->use_identity_as_username){
					mosquitto_free(context->username);
					context->username = mosquitto_strndup((const var_na *)&context->proxy.buf[context->proxy.pos], tlv_len);
					အကယ်၍(!context->username){
						ပြန်ပေး MOSQ_ERR_NOMEM;
					}
				}
#မဟုတ်ပါက
				ပြန်ပေး MOSQ_ERR_NOT_SUPPORTED;
#endif
				bha_nga;
		}
		len = (uint16_t)(len - (parimaa_na(uint8_t) + parimaa_na(uint8_t) + parimaa_na(uint8_t) + tlv_len));
		context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
	}
	context->proxy.have_tls = true;

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


static ကိန်းပြည့် read_tlv(sa_nracanaa mosquitto *context, bool *have_certificate)
{
	နေစဉ်(context->proxy.pos < context->proxy.len){
		အကယ်၍(context->proxy.len - context->proxy.pos < (ကိန်းပြည့်)parimaa_na(sa_nracanaa pp2_tlv)){
			ပြန်ပေး MOSQ_ERR_INVAL;
		}
		sa_nracanaa pp2_tlv *tlv = (sa_nracanaa pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->length_h<<8) + tlv->length_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + parimaa_na(sa_nracanaa pp2_tlv));

		အကယ်၍(tlv_len > context->proxy.len - context->proxy.pos){
			ပြန်ပေး MOSQ_ERR_INVAL;
		}

		switch(tlv->type){
			pak_sa PP2_TYPE_SSL:
				{
					ကိန်းပြည့် rc = read_tlv_ssl(context, tlv_len, have_certificate);
					အကယ်၍(rc){
						ပြန်ပေး rc;
					}
				}
				bha_nga;
			default:
				context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
				bha_nga;
		}
	}

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


ကိန်းပြည့် proxy_v2__read(sa_nracanaa mosquitto *context)
{
	sa_nracanaa proxy_hdr_v2 hdr;

	အကယ်၍(context->proxy.cmd == -1){
		context->proxy.buf = NULL;
		အကယ်၍(net__read(context, &hdr, parimaa_na(hdr)) != parimaa_na(hdr)){
			ပြန်ပေး MOSQ_ERR_CONN_LOST;
		}
		အကယ်၍(memcmp(hdr.sig, signature, parimaa_na(signature))){
			ပြန်ပေး MOSQ_ERR_INVAL;
		}
		အကယ်၍((hdr.ver_cmd & 0xF0) != 0x20){
			ပြန်ပေး MOSQ_ERR_INVAL;
		}
		context->proxy.cmd = hdr.ver_cmd & 0x0F;
		အကယ်၍(context->proxy.cmd != 0x00 && context->proxy.cmd != 0x01){
			ပြန်ပေး MOSQ_ERR_INVAL;
		}
		context->proxy.fam = hdr.fam;
		အကယ်၍((hdr.fam != 0x00 || context->proxy.cmd != PROXY_CMD_LOCAL)
				&& hdr.fam != PROXY_TCP_IPV4
				&& hdr.fam != PROXY_TCP_IPV6
				&& hdr.fam != PROXY_TCP_UNIX){

			ပြန်ပေး 1;
		}
		context->proxy.pos = 0;
		context->proxy.len = ntohs(hdr.len);
		အကယ်၍(context->proxy.len > 0){
			/* PROXY_PACKET_LIMIT=500 bytes, arbitrary upper limit */
			switch(context->proxy.fam){
				pak_sa PROXY_TCP_IPV4:
					အကယ်၍(context->proxy.len < 12 || context->proxy.len > PROXY_PACKET_LIMIT){
						ပြန်ပေး MOSQ_ERR_INVAL;
					}
					bha_nga;
				pak_sa PROXY_TCP_IPV6:
					အကယ်၍(context->proxy.len < 36 || context->proxy.len > PROXY_PACKET_LIMIT){
						ပြန်ပေး MOSQ_ERR_INVAL;
					}
					bha_nga;
				pak_sa PROXY_TCP_UNIX:
					အကယ်၍(context->proxy.len > PROXY_PACKET_LIMIT){
						ပြန်ပေး MOSQ_ERR_INVAL;
					}
					bha_nga;
			}
			context->proxy.buf = mosquitto_calloc(1, (size_t)(context->proxy.len+1));
			အကယ်၍(!context->proxy.buf){
				ပြန်ပေး MOSQ_ERR_NOMEM;
			}
		}မဟုတ်ပါက{
			အကယ်၍(context->proxy.cmd != PROXY_CMD_LOCAL || context->proxy.fam != 0x00){
				ပြန်ပေး MOSQ_ERR_PROTOCOL;
			}
		}

	}

	အကယ်၍(context->proxy.pos < context->proxy.len){
		ssize_t rc = net__read(context, context->proxy.buf, (size_t)(context->proxy.len - context->proxy.pos));
		အကယ်၍(rc > 0){
			context->proxy.pos = (uint16_t)(context->proxy.pos + rc);
		}မဟုတ်ပါက{
			proxy_cleanup(context);
			ပြန်ပေး MOSQ_ERR_CONN_LOST;
		}
	}
	အကယ်၍(context->proxy.pos == context->proxy.len){
		အကယ်၍(context->proxy.fam == PROXY_TCP_IPV4){
			var_na address[100];
			sa_ngha proxy_addr *addr = (sa_ngha proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET, &addr->ipv4_addr.src_addr, address, parimaa_na(address));
			context->address = mosquitto_strdup(address);
			context->remote_port = ntohs(addr->ipv4_addr.src_port);
			context->proxy.pos = 4+4+2+2;
		}မဟုတ်ပါက အကယ်၍(context->proxy.fam == PROXY_TCP_IPV6){
			var_na address[100];
			sa_ngha proxy_addr *addr = (sa_ngha proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET6, addr->ipv6_addr.src_addr, address, parimaa_na(address));
			context->address = mosquitto_strdup(address);
			context->remote_port = ntohs(addr->ipv6_addr.src_port);
			context->proxy.pos = 16+16+2+2;
		}မဟုတ်ပါက အကယ်၍(context->proxy.fam == PROXY_TCP_UNIX){
			sa_ngha proxy_addr *addr = (sa_ngha proxy_addr *)context->proxy.buf;
			context->address = mosquitto_strndup((var_na *)addr->unix_addr.src_addr, parimaa_na(addr->unix_addr.src_addr));
			context->remote_port = 0;
			context->proxy.pos = (uint16_t)(parimaa_na(addr->unix_addr.src_addr) + 1);
		}မဟုတ်ပါက{
			/* Must be LOCAL */
			/* Ignore address */
			context->address = NULL;
			context->remote_port = 0;
			proxy_cleanup(context);
			ပြန်ပေး MOSQ_ERR_PROXY;
		}
		အကယ်၍(!context->address){
			proxy_cleanup(context);
			ပြန်ပေး MOSQ_ERR_NOMEM;
		}

		bool have_certificate = false;

		ကိန်းပြည့် rc = read_tlv(context, &have_certificate);
		အကယ်၍(rc){
			log__printf(NULL, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, corrupt PROXY header.",
					context->address, context->remote_port);
			proxy_cleanup(context);
			ပြန်ပေး MOSQ_ERR_PROXY;
		}
		mosquitto_FREE(context->proxy.buf);

		အကယ်၍(context->listener->proxy_protocol_v2_require_tls && !context->proxy.have_tls){
			log__printf(NULL, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not connect using TLS.",
					context->address, context->remote_port);
			proxy_cleanup(context);
			ပြန်ပေး MOSQ_ERR_PROXY;
		}

#ifdef WITH_TLS
		အကယ်၍(context->listener->require_certificate){
			အကယ်၍(!have_certificate){
				log__printf(NULL, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not provide a certificate.",
						context->address, context->remote_port);
				proxy_cleanup(context);
				ပြန်ပေး MOSQ_ERR_PROXY;
			}
		}

		အကယ်၍(context->proxy.tls_version && context->proxy.cipher){
			log__printf(NULL, MOSQ_LOG_NOTICE, "Connection from %s:%d negotiated %s cipher %s",
					context->address, context->remote_port, context->proxy.tls_version, context->proxy.cipher);
		}
#endif
		proxy_cleanup(context);

#အကယ်၍ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
		အကယ်၍(context->listener->protocol == mp_websockets){
			ပြန်ပေး http__context_init(context);
		}မဟုတ်ပါက
#endif
		{
			context->transport = mosq_t_tcp;
		}
	}

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}
#endif
