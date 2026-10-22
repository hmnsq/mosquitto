#यदि_परिभाषितम् WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#अन्यथा
#  include <arpa/inet.h>
#  include <netinet/in.h>
#यदि_अन्तः
#अन्तर्भाव <stdint.h>
#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto_internal.h"
#अन्तर्भाव "net_mosq.h"

#यदि !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN

#परिभाषय PROXY_CMD_LOCAL 0x00
#परिभाषय PROXY_CMD_PROXY 0x01

#परिभाषय PROXY_TCP_IPV4 0x11
#परिभाषय PROXY_TCP_IPV6 0x21
#परिभाषय PROXY_TCP_UNIX 0x31

#परिभाषय PP2_TYPE_ALPN           0x01
#परिभाषय PP2_TYPE_AUTHORITY      0x02
#परिभाषय PP2_TYPE_चक्रीयपरीक्षा_32_C         0x03
#परिभाषय PP2_TYPE_NOOP           0x04
#परिभाषय PP2_TYPE_UNIQUE_ID      0x05
#परिभाषय PP2_TYPE_SSL            0x20
#परिभाषय PP2_SUBTYPE_SSL_संस्करणम् 0x21
#परिभाषय PP2_SUBTYPE_SSL_CN      0x22
#परिभाषय PP2_SUBTYPE_SSL_CIPHER  0x23
#परिभाषय PP2_SUBTYPE_SSL_SIG_ALG 0x24
#परिभाषय PP2_SUBTYPE_SSL_कुञ्चिका_ALG 0x25
#परिभाषय PP2_TYPE_NETNS          0x30

#परिभाषय PP2_CLIENT_SSL           0x01
#परिभाषय PP2_CLIENT_CERT_CONN     0x02
#परिभाषय PP2_CLIENT_CERT_SESS     0x04

#परिभाषय PROXY_PACKET_सीमा 500

संरचना proxy_hdr_v2 {
	uint8_t sig[12]; /* hex 0D 0A 0D 0A 00 0D 0A 51 55 49 54 0A */
	uint8_t ver_cmd; /* protocol version and command */
	uint8_t fam; /* protocol family and address */
	uint16_t len; /* number of following bytes part of the header */
};

सङ्घ proxy_addr {
	संरचना { /* for TCP/UDP over IPv4, len = 12 */
		uint32_t src_addr;
		uint32_t dst_addr;
		uint16_t src_पत्तनम्;
		uint16_t dst_पत्तनम्;
	} ipv4_addr;
	संरचना { /* for TCP/UDP over IPv6, len = 36 */
		uint8_t src_addr[16];
		uint8_t dst_addr[16];
		uint16_t src_पत्तनम्;
		uint16_t dst_पत्तनम्;
	} ipv6_addr;
	संरचना { /* for AF_UNIX sockets, len = 216 */
		uint8_t src_addr[108];
		uint8_t dst_addr[108];
	} unix_addr;
};

संरचना pp2_tlv {
	uint8_t type;
	uint8_t दीर्घता_h;
	uint8_t दीर्घता_l;
};

संरचना pp2_tlv_ssl {
	uint8_t client;
	uint32_t verify;
};

ध्रुव uint8_t signature[12] = {0x0D, 0x0A, 0x0D, 0x0A, 0x00, 0x0D, 0x0A, 0x51, 0x55, 0x49, 0x54, 0x0A};


स्थिर शून्य proxy_cleanup(संरचना mosquitto *context)
{
	mosquitto_FREE(context->proxy.buf);
	mosquitto_FREE(context->proxy.tls_संस्करणम्);
	mosquitto_FREE(context->proxy.cipher);
}


स्थिर पूर्णाङ्क read_tlv_ssl(संरचना mosquitto *context, uint16_t len, bool *have_certificate)
{
	संरचना pp2_tlv_ssl ssl = {0};

	यदि(len < परिमाण(uint8_t) + परिमाण(uint32_t)){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	ssl.client = context->proxy.buf[context->proxy.pos];
	ssl.verify = ntohl(*(uint32_t *)(&context->proxy.buf[context->proxy.pos + परिमाण(uint8_t)]));
	context->proxy.pos = (uint16_t)(context->proxy.pos + परिमाण(uint8_t) + परिमाण(uint32_t));
	len = (uint16_t)(len - (परिमाण(uint8_t) + परिमाण(uint32_t)));

	यदि(ssl.client & PP2_CLIENT_SSL && ssl.client & PP2_CLIENT_CERT_SESS && !ssl.verify){
		*have_certificate = true;
	}

	यावत्(len > 0){
		यदि(context->proxy.len - context->proxy.pos < (पूर्णाङ्क)परिमाण(संरचना pp2_tlv)){
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		संरचना pp2_tlv *tlv = (संरचना pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->दीर्घता_h<<8) + tlv->दीर्घता_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + परिमाण(संरचना pp2_tlv));

		यदि(tlv_len > context->proxy.len - context->proxy.pos){
			प्रत्यागम MOSQ_ERR_INVAL;
		}

		विकल्प(tlv->type){
			पक्ष PP2_SUBTYPE_SSL_संस्करणम्:
#यदि_परिभाषितम् WITH_TLS
				mosquitto_free(context->proxy.tls_संस्करणम्);
				context->proxy.tls_संस्करणम् = mosquitto_strndup((ध्रुव वर्ण *)&context->proxy.buf[context->proxy.pos], tlv_len);
#अन्यथा
				प्रत्यागम MOSQ_ERR_NOT_SUPPORTED;
#यदि_अन्तः
				भङ्ग;

			पक्ष PP2_SUBTYPE_SSL_CIPHER:
#यदि_परिभाषितम् WITH_TLS
				mosquitto_free(context->proxy.cipher);
				context->proxy.cipher = mosquitto_strndup((ध्रुव वर्ण *)&context->proxy.buf[context->proxy.pos], tlv_len);
#अन्यथा
				प्रत्यागम MOSQ_ERR_NOT_SUPPORTED;
#यदि_अन्तः
				भङ्ग;

			पक्ष PP2_SUBTYPE_SSL_CN:
#यदि_परिभाषितम् WITH_TLS
				यदि(context->listener->use_तटस्थः_as_username){
					mosquitto_free(context->username);
					context->username = mosquitto_strndup((ध्रुव वर्ण *)&context->proxy.buf[context->proxy.pos], tlv_len);
					यदि(!context->username){
						प्रत्यागम MOSQ_ERR_NOMEM;
					}
				}
#अन्यथा
				प्रत्यागम MOSQ_ERR_NOT_SUPPORTED;
#यदि_अन्तः
				भङ्ग;
		}
		len = (uint16_t)(len - (परिमाण(uint8_t) + परिमाण(uint8_t) + परिमाण(uint8_t) + tlv_len));
		context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
	}
	context->proxy.have_tls = true;

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क read_tlv(संरचना mosquitto *context, bool *have_certificate)
{
	यावत्(context->proxy.pos < context->proxy.len){
		यदि(context->proxy.len - context->proxy.pos < (पूर्णाङ्क)परिमाण(संरचना pp2_tlv)){
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		संरचना pp2_tlv *tlv = (संरचना pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->दीर्घता_h<<8) + tlv->दीर्घता_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + परिमाण(संरचना pp2_tlv));

		यदि(tlv_len > context->proxy.len - context->proxy.pos){
			प्रत्यागम MOSQ_ERR_INVAL;
		}

		विकल्प(tlv->type){
			पक्ष PP2_TYPE_SSL:
				{
					पूर्णाङ्क rc = read_tlv_ssl(context, tlv_len, have_certificate);
					यदि(rc){
						प्रत्यागम rc;
					}
				}
				भङ्ग;
			स्वभाव:
				context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
				भङ्ग;
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क proxy_v2__read(संरचना mosquitto *context)
{
	संरचना proxy_hdr_v2 hdr;

	यदि(context->proxy.cmd == -1){
		context->proxy.buf = रिक्त;
		यदि(net__read(context, &hdr, परिमाण(hdr)) != परिमाण(hdr)){
			प्रत्यागम MOSQ_ERR_CONN_नष्टम्;
		}
		यदि(memcmp(hdr.sig, signature, परिमाण(signature))){
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		यदि((hdr.ver_cmd & 0xF0) != 0x20){
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		context->proxy.cmd = hdr.ver_cmd & 0x0F;
		यदि(context->proxy.cmd != 0x00 && context->proxy.cmd != 0x01){
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		context->proxy.fam = hdr.fam;
		यदि((hdr.fam != 0x00 || context->proxy.cmd != PROXY_CMD_LOCAL)
				&& hdr.fam != PROXY_TCP_IPV4
				&& hdr.fam != PROXY_TCP_IPV6
				&& hdr.fam != PROXY_TCP_UNIX){

			प्रत्यागम 1;
		}
		context->proxy.pos = 0;
		context->proxy.len = ntohs(hdr.len);
		यदि(context->proxy.len > 0){
			/* PROXY_PACKET_LIMIT=500 bytes, arbitrary upper limit */
			विकल्प(context->proxy.fam){
				पक्ष PROXY_TCP_IPV4:
					यदि(context->proxy.len < 12 || context->proxy.len > PROXY_PACKET_सीमा){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					भङ्ग;
				पक्ष PROXY_TCP_IPV6:
					यदि(context->proxy.len < 36 || context->proxy.len > PROXY_PACKET_सीमा){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					भङ्ग;
				पक्ष PROXY_TCP_UNIX:
					यदि(context->proxy.len > PROXY_PACKET_सीमा){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					भङ्ग;
			}
			context->proxy.buf = mosquitto_calloc(1, (size_t)(context->proxy.len+1));
			यदि(!context->proxy.buf){
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
		}अन्यथा{
			यदि(context->proxy.cmd != PROXY_CMD_LOCAL || context->proxy.fam != 0x00){
				प्रत्यागम MOSQ_ERR_PROTOCOL;
			}
		}

	}

	यदि(context->proxy.pos < context->proxy.len){
		ssize_t rc = net__read(context, context->proxy.buf, (size_t)(context->proxy.len - context->proxy.pos));
		यदि(rc > 0){
			context->proxy.pos = (uint16_t)(context->proxy.pos + rc);
		}अन्यथा{
			proxy_cleanup(context);
			प्रत्यागम MOSQ_ERR_CONN_नष्टम्;
		}
	}
	यदि(context->proxy.pos == context->proxy.len){
		यदि(context->proxy.fam == PROXY_TCP_IPV4){
			वर्ण स्थानाङ्कः[100];
			सङ्घ proxy_addr *addr = (सङ्घ proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET, &addr->ipv4_addr.src_addr, स्थानाङ्कः, परिमाण(स्थानाङ्कः));
			context->स्थानाङ्कः = mosquitto_strdup(स्थानाङ्कः);
			context->remote_पत्तनम् = ntohs(addr->ipv4_addr.src_पत्तनम्);
			context->proxy.pos = 4+4+2+2;
		}अन्यथा यदि(context->proxy.fam == PROXY_TCP_IPV6){
			वर्ण स्थानाङ्कः[100];
			सङ्घ proxy_addr *addr = (सङ्घ proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET6, addr->ipv6_addr.src_addr, स्थानाङ्कः, परिमाण(स्थानाङ्कः));
			context->स्थानाङ्कः = mosquitto_strdup(स्थानाङ्कः);
			context->remote_पत्तनम् = ntohs(addr->ipv6_addr.src_पत्तनम्);
			context->proxy.pos = 16+16+2+2;
		}अन्यथा यदि(context->proxy.fam == PROXY_TCP_UNIX){
			सङ्घ proxy_addr *addr = (सङ्घ proxy_addr *)context->proxy.buf;
			context->स्थानाङ्कः = mosquitto_strndup((वर्ण *)addr->unix_addr.src_addr, परिमाण(addr->unix_addr.src_addr));
			context->remote_पत्तनम् = 0;
			context->proxy.pos = (uint16_t)(परिमाण(addr->unix_addr.src_addr) + 1);
		}अन्यथा{
			/* Must be LOCAL */
			/* Ignore address */
			context->स्थानाङ्कः = रिक्त;
			context->remote_पत्तनम् = 0;
			proxy_cleanup(context);
			प्रत्यागम MOSQ_ERR_PROXY;
		}
		यदि(!context->स्थानाङ्कः){
			proxy_cleanup(context);
			प्रत्यागम MOSQ_ERR_NOMEM;
		}

		bool have_certificate = false;

		पूर्णाङ्क rc = read_tlv(context, &have_certificate);
		यदि(rc){
			log__printf(रिक्त, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, corrupt PROXY header.",
					context->स्थानाङ्कः, context->remote_पत्तनम्);
			proxy_cleanup(context);
			प्रत्यागम MOSQ_ERR_PROXY;
		}
		mosquitto_FREE(context->proxy.buf);

		यदि(context->listener->proxy_protocol_v2_require_tls && !context->proxy.have_tls){
			log__printf(रिक्त, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not connect using TLS.",
					context->स्थानाङ्कः, context->remote_पत्तनम्);
			proxy_cleanup(context);
			प्रत्यागम MOSQ_ERR_PROXY;
		}

#यदि_परिभाषितम् WITH_TLS
		यदि(context->listener->require_certificate){
			यदि(!have_certificate){
				log__printf(रिक्त, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not provide a certificate.",
						context->स्थानाङ्कः, context->remote_पत्तनम्);
				proxy_cleanup(context);
				प्रत्यागम MOSQ_ERR_PROXY;
			}
		}

		यदि(context->proxy.tls_संस्करणम् && context->proxy.cipher){
			log__printf(रिक्त, MOSQ_LOG_NOTICE, "Connection from %s:%d negotiated %s cipher %s",
					context->स्थानाङ्कः, context->remote_पत्तनम्, context->proxy.tls_संस्करणम्, context->proxy.cipher);
		}
#यदि_अन्तः
		proxy_cleanup(context);

#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
		यदि(context->listener->protocol == mp_websockets){
			प्रत्यागम http__context_init(context);
		}अन्यथा
#यदि_अन्तः
		{
			context->transport = mosq_t_tcp;
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}
#यदि_अन्तः
