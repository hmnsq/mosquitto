#నిర్వచితమైతే WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#లేకపోతే
#  include <arpa/inet.h>
#  include <netinet/in.h>
#అయితే_ముగింపు
#అన్తర్భావ <stdint.h>
#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto_internal.h"
#అన్తర్భావ "net_mosq.h"

#ఒకవేళ !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN

#నిర్వచించు PROXY_CMD_LOCAL 0x00
#నిర్వచించు PROXY_CMD_PROXY 0x01

#నిర్వచించు PROXY_TCP_IPV4 0x11
#నిర్వచించు PROXY_TCP_IPV6 0x21
#నిర్వచించు PROXY_TCP_UNIX 0x31

#నిర్వచించు PP2_TYPE_ALPN           0x01
#నిర్వచించు PP2_TYPE_AUTHORITY      0x02
#నిర్వచించు PP2_TYPE_చక్రీయ_తనిఖీ_32_C         0x03
#నిర్వచించు PP2_TYPE_NOOP           0x04
#నిర్వచించు PP2_TYPE_UNIQUE_ID      0x05
#నిర్వచించు PP2_TYPE_SSL            0x20
#నిర్వచించు PP2_SUBTYPE_SSL_సంస్కరణ 0x21
#నిర్వచించు PP2_SUBTYPE_SSL_CN      0x22
#నిర్వచించు PP2_SUBTYPE_SSL_CIPHER  0x23
#నిర్వచించు PP2_SUBTYPE_SSL_SIG_ALG 0x24
#నిర్వచించు PP2_SUBTYPE_SSL_కీలకం_ALG 0x25
#నిర్వచించు PP2_TYPE_NETNS          0x30

#నిర్వచించు PP2_CLIENT_SSL           0x01
#నిర్వచించు PP2_CLIENT_CERT_CONN     0x02
#నిర్వచించు PP2_CLIENT_CERT_SESS     0x04

#నిర్వచించు PROXY_PACKET_హద్దు 500

నిర్మాణం proxy_hdr_v2 {
	uint8_t sig[12]; /* hex 0D 0A 0D 0A 00 0D 0A 51 55 49 54 0A */
	uint8_t ver_cmd; /* protocol version and command */
	uint8_t fam; /* protocol family and address */
	uint16_t len; /* number of following bytes part of the header */
};

సంఘం proxy_addr {
	నిర్మాణం { /* for TCP/UDP over IPv4, len = 12 */
		uint32_t src_addr;
		uint32_t dst_addr;
		uint16_t src_పోర్టు;
		uint16_t dst_పోర్టు;
	} ipv4_addr;
	నిర్మాణం { /* for TCP/UDP over IPv6, len = 36 */
		uint8_t src_addr[16];
		uint8_t dst_addr[16];
		uint16_t src_పోర్టు;
		uint16_t dst_పోర్టు;
	} ipv6_addr;
	నిర్మాణం { /* for AF_UNIX sockets, len = 216 */
		uint8_t src_addr[108];
		uint8_t dst_addr[108];
	} unix_addr;
};

నిర్మాణం pp2_tlv {
	uint8_t type;
	uint8_t నిడివి_h;
	uint8_t నిడివి_l;
};

నిర్మాణం pp2_tlv_ssl {
	uint8_t client;
	uint32_t verify;
};

స్థిరరాశి uint8_t signature[12] = {0x0D, 0x0A, 0x0D, 0x0A, 0x00, 0x0D, 0x0A, 0x51, 0x55, 0x49, 0x54, 0x0A};


స్థిర శూన్య proxy_cleanup(నిర్మాణం mosquitto *context)
{
	mosquitto_FREE(context->proxy.buf);
	mosquitto_FREE(context->proxy.tls_సంస్కరణ);
	mosquitto_FREE(context->proxy.cipher);
}


స్థిర పూర్ణాంకం read_tlv_ssl(నిర్మాణం mosquitto *context, uint16_t len, bool *have_certificate)
{
	నిర్మాణం pp2_tlv_ssl ssl = {0};

	ఒకవేళ(len < పరిమాణం(uint8_t) + పరిమాణం(uint32_t)){
		తిరిగి MOSQ_ERR_INVAL;
	}

	ssl.client = context->proxy.buf[context->proxy.pos];
	ssl.verify = ntohl(*(uint32_t *)(&context->proxy.buf[context->proxy.pos + పరిమాణం(uint8_t)]));
	context->proxy.pos = (uint16_t)(context->proxy.pos + పరిమాణం(uint8_t) + పరిమాణం(uint32_t));
	len = (uint16_t)(len - (పరిమాణం(uint8_t) + పరిమాణం(uint32_t)));

	ఒకవేళ(ssl.client & PP2_CLIENT_SSL && ssl.client & PP2_CLIENT_CERT_SESS && !ssl.verify){
		*have_certificate = true;
	}

	అయితే(len > 0){
		ఒకవేళ(context->proxy.len - context->proxy.pos < (పూర్ణాంకం)పరిమాణం(నిర్మాణం pp2_tlv)){
			తిరిగి MOSQ_ERR_INVAL;
		}
		నిర్మాణం pp2_tlv *tlv = (నిర్మాణం pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->నిడివి_h<<8) + tlv->నిడివి_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + పరిమాణం(నిర్మాణం pp2_tlv));

		ఒకవేళ(tlv_len > context->proxy.len - context->proxy.pos){
			తిరిగి MOSQ_ERR_INVAL;
		}

		ఎంపిక(tlv->type){
			స్థితి PP2_SUBTYPE_SSL_సంస్కరణ:
#నిర్వచితమైతే WITH_TLS
				mosquitto_free(context->proxy.tls_సంస్కరణ);
				context->proxy.tls_సంస్కరణ = mosquitto_strndup((స్థిరరాశి అక్షరం *)&context->proxy.buf[context->proxy.pos], tlv_len);
#లేకపోతే
				తిరిగి MOSQ_ERR_NOT_SUPPORTED;
#అయితే_ముగింపు
				ఆపు;

			స్థితి PP2_SUBTYPE_SSL_CIPHER:
#నిర్వచితమైతే WITH_TLS
				mosquitto_free(context->proxy.cipher);
				context->proxy.cipher = mosquitto_strndup((స్థిరరాశి అక్షరం *)&context->proxy.buf[context->proxy.pos], tlv_len);
#లేకపోతే
				తిరిగి MOSQ_ERR_NOT_SUPPORTED;
#అయితే_ముగింపు
				ఆపు;

			స్థితి PP2_SUBTYPE_SSL_CN:
#నిర్వచితమైతే WITH_TLS
				ఒకవేళ(context->listener->use_తటస్థం_as_username){
					mosquitto_free(context->username);
					context->username = mosquitto_strndup((స్థిరరాశి అక్షరం *)&context->proxy.buf[context->proxy.pos], tlv_len);
					ఒకవేళ(!context->username){
						తిరిగి MOSQ_ERR_NOMEM;
					}
				}
#లేకపోతే
				తిరిగి MOSQ_ERR_NOT_SUPPORTED;
#అయితే_ముగింపు
				ఆపు;
		}
		len = (uint16_t)(len - (పరిమాణం(uint8_t) + పరిమాణం(uint8_t) + పరిమాణం(uint8_t) + tlv_len));
		context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
	}
	context->proxy.have_tls = true;

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం read_tlv(నిర్మాణం mosquitto *context, bool *have_certificate)
{
	అయితే(context->proxy.pos < context->proxy.len){
		ఒకవేళ(context->proxy.len - context->proxy.pos < (పూర్ణాంకం)పరిమాణం(నిర్మాణం pp2_tlv)){
			తిరిగి MOSQ_ERR_INVAL;
		}
		నిర్మాణం pp2_tlv *tlv = (నిర్మాణం pp2_tlv *)(&context->proxy.buf[context->proxy.pos]);
		uint16_t tlv_len = (uint16_t)((tlv->నిడివి_h<<8) + tlv->నిడివి_l);
		context->proxy.pos = (uint16_t)(context->proxy.pos + పరిమాణం(నిర్మాణం pp2_tlv));

		ఒకవేళ(tlv_len > context->proxy.len - context->proxy.pos){
			తిరిగి MOSQ_ERR_INVAL;
		}

		ఎంపిక(tlv->type){
			స్థితి PP2_TYPE_SSL:
				{
					పూర్ణాంకం rc = read_tlv_ssl(context, tlv_len, have_certificate);
					ఒకవేళ(rc){
						తిరిగి rc;
					}
				}
				ఆపు;
			అప్రమేయ:
				context->proxy.pos = (uint16_t)(context->proxy.pos + tlv_len);
				ఆపు;
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం proxy_v2__read(నిర్మాణం mosquitto *context)
{
	నిర్మాణం proxy_hdr_v2 hdr;

	ఒకవేళ(context->proxy.cmd == -1){
		context->proxy.buf = ఖాళీ;
		ఒకవేళ(net__read(context, &hdr, పరిమాణం(hdr)) != పరిమాణం(hdr)){
			తిరిగి MOSQ_ERR_CONN_కోల్పోయిన;
		}
		ఒకవేళ(memcmp(hdr.sig, signature, పరిమాణం(signature))){
			తిరిగి MOSQ_ERR_INVAL;
		}
		ఒకవేళ((hdr.ver_cmd & 0xF0) != 0x20){
			తిరిగి MOSQ_ERR_INVAL;
		}
		context->proxy.cmd = hdr.ver_cmd & 0x0F;
		ఒకవేళ(context->proxy.cmd != 0x00 && context->proxy.cmd != 0x01){
			తిరిగి MOSQ_ERR_INVAL;
		}
		context->proxy.fam = hdr.fam;
		ఒకవేళ((hdr.fam != 0x00 || context->proxy.cmd != PROXY_CMD_LOCAL)
				&& hdr.fam != PROXY_TCP_IPV4
				&& hdr.fam != PROXY_TCP_IPV6
				&& hdr.fam != PROXY_TCP_UNIX){

			తిరిగి 1;
		}
		context->proxy.pos = 0;
		context->proxy.len = ntohs(hdr.len);
		ఒకవేళ(context->proxy.len > 0){
			/* PROXY_PACKET_LIMIT=500 bytes, arbitrary upper limit */
			ఎంపిక(context->proxy.fam){
				స్థితి PROXY_TCP_IPV4:
					ఒకవేళ(context->proxy.len < 12 || context->proxy.len > PROXY_PACKET_హద్దు){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఆపు;
				స్థితి PROXY_TCP_IPV6:
					ఒకవేళ(context->proxy.len < 36 || context->proxy.len > PROXY_PACKET_హద్దు){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఆపు;
				స్థితి PROXY_TCP_UNIX:
					ఒకవేళ(context->proxy.len > PROXY_PACKET_హద్దు){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఆపు;
			}
			context->proxy.buf = mosquitto_calloc(1, (size_t)(context->proxy.len+1));
			ఒకవేళ(!context->proxy.buf){
				తిరిగి MOSQ_ERR_NOMEM;
			}
		}లేకపోతే{
			ఒకవేళ(context->proxy.cmd != PROXY_CMD_LOCAL || context->proxy.fam != 0x00){
				తిరిగి MOSQ_ERR_PROTOCOL;
			}
		}

	}

	ఒకవేళ(context->proxy.pos < context->proxy.len){
		ssize_t rc = net__read(context, context->proxy.buf, (size_t)(context->proxy.len - context->proxy.pos));
		ఒకవేళ(rc > 0){
			context->proxy.pos = (uint16_t)(context->proxy.pos + rc);
		}లేకపోతే{
			proxy_cleanup(context);
			తిరిగి MOSQ_ERR_CONN_కోల్పోయిన;
		}
	}
	ఒకవేళ(context->proxy.pos == context->proxy.len){
		ఒకవేళ(context->proxy.fam == PROXY_TCP_IPV4){
			అక్షరం చిరునామా[100];
			సంఘం proxy_addr *addr = (సంఘం proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET, &addr->ipv4_addr.src_addr, చిరునామా, పరిమాణం(చిరునామా));
			context->చిరునామా = mosquitto_strdup(చిరునామా);
			context->remote_పోర్టు = ntohs(addr->ipv4_addr.src_పోర్టు);
			context->proxy.pos = 4+4+2+2;
		}లేకపోతే ఒకవేళ(context->proxy.fam == PROXY_TCP_IPV6){
			అక్షరం చిరునామా[100];
			సంఘం proxy_addr *addr = (సంఘం proxy_addr *)context->proxy.buf;

			inet_ntop(AF_INET6, addr->ipv6_addr.src_addr, చిరునామా, పరిమాణం(చిరునామా));
			context->చిరునామా = mosquitto_strdup(చిరునామా);
			context->remote_పోర్టు = ntohs(addr->ipv6_addr.src_పోర్టు);
			context->proxy.pos = 16+16+2+2;
		}లేకపోతే ఒకవేళ(context->proxy.fam == PROXY_TCP_UNIX){
			సంఘం proxy_addr *addr = (సంఘం proxy_addr *)context->proxy.buf;
			context->చిరునామా = mosquitto_strndup((అక్షరం *)addr->unix_addr.src_addr, పరిమాణం(addr->unix_addr.src_addr));
			context->remote_పోర్టు = 0;
			context->proxy.pos = (uint16_t)(పరిమాణం(addr->unix_addr.src_addr) + 1);
		}లేకపోతే{
			/* Must be LOCAL */
			/* Ignore address */
			context->చిరునామా = ఖాళీ;
			context->remote_పోర్టు = 0;
			proxy_cleanup(context);
			తిరిగి MOSQ_ERR_PROXY;
		}
		ఒకవేళ(!context->చిరునామా){
			proxy_cleanup(context);
			తిరిగి MOSQ_ERR_NOMEM;
		}

		bool have_certificate = false;

		పూర్ణాంకం rc = read_tlv(context, &have_certificate);
		ఒకవేళ(rc){
			log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, corrupt PROXY header.",
					context->చిరునామా, context->remote_పోర్టు);
			proxy_cleanup(context);
			తిరిగి MOSQ_ERR_PROXY;
		}
		mosquitto_FREE(context->proxy.buf);

		ఒకవేళ(context->listener->proxy_protocol_v2_require_tls && !context->proxy.have_tls){
			log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not connect using TLS.",
					context->చిరునామా, context->remote_పోర్టు);
			proxy_cleanup(context);
			తిరిగి MOSQ_ERR_PROXY;
		}

#నిర్వచితమైతే WITH_TLS
		ఒకవేళ(context->listener->require_certificate){
			ఒకవేళ(!have_certificate){
				log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Connection from %s:%d rejected, client did not provide a certificate.",
						context->చిరునామా, context->remote_పోర్టు);
				proxy_cleanup(context);
				తిరిగి MOSQ_ERR_PROXY;
			}
		}

		ఒకవేళ(context->proxy.tls_సంస్కరణ && context->proxy.cipher){
			log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Connection from %s:%d negotiated %s cipher %s",
					context->చిరునామా, context->remote_పోర్టు, context->proxy.tls_సంస్కరణ, context->proxy.cipher);
		}
#అయితే_ముగింపు
		proxy_cleanup(context);

#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
		ఒకవేళ(context->listener->protocol == mp_websockets){
			తిరిగి http__context_init(context);
		}లేకపోతే
#అయితే_ముగింపు
		{
			context->transport = mosq_t_tcp;
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}
#అయితే_ముగింపు
