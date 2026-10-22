#ifdef WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#ᨀᨘᨁ
#  include <arpa/inet.h>
#  include <netinet/in.h>
#endif
#include <stdint.h>
#include "mosquitto_broker_internal.h"
#include "mosquitto_internal.h"
#include "net_mosq.h"

#ᨀᨘ !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN

#define PROXY_V1_PACKET_LIMIT 108

const uint8_t signature4[11] = {'P', 'R', 'O', 'X', 'Y', ' ', 'T', 'C', 'P', '4', ' '};
const uint8_t signature6[11] = {'P', 'R', 'O', 'X', 'Y', ' ', 'T', 'C', 'P', '6', ' '};
const uint8_t signatureU[14] = {'P', 'R', 'O', 'X', 'Y', ' ', 'U', 'N', 'K', 'N', 'O', 'W', 'N', ' '};


static void proxy_cleanup(sa_nracanaa mosquitto *context)
{
	mosquitto_FREE(context->proxy.buf);
}


static puur_naa_nka update_transport(sa_nracanaa mosquitto *context)
{
#ᨀᨘ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	ᨀᨘ(context->listener->protocol == mp_websockets){
		ᨀᨘᨒ http__context_init(context);
	}ᨀᨘᨁ
#endif
	{
		context->transport = mosq_t_tcp;
	}
	ᨀᨘᨒ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka get_address_for_unknown(sa_nracanaa mosquitto *context)
{
	var_na address[1024];

	proxy_cleanup(context);

	ᨀᨘ(!net__socket_get_address(context->sock, address, parimaa_na(address), &context->remote_port)){
		context->address = mosquitto_strdup(address);
	}
	ᨀᨘ(!context->address){
		ᨀᨘᨒ MOSQ_ERR_NOMEM;
	}
	ᨀᨘᨒ update_transport(context);
}


static puur_naa_nka proxy_v1__decode(sa_nracanaa mosquitto *context)
{
	var_na *saddr_s, *daddr_s, *sport_s, *dport_s;
	var_na *saveptr = NULL;
	puur_naa_nka sport, dport;
	sa_nracanaa in6_addr addr;

	ᨀᨘ(context->proxy.pos >= parimaa_na(signatureU) && !memcmp(context->proxy.buf, signatureU, parimaa_na(signatureU))){
		ᨀᨘᨒ get_address_for_unknown(context);
	}ᨀᨘᨁ ᨀᨘ(context->proxy.pos >= parimaa_na(signature4) && !memcmp(context->proxy.buf, signature4, parimaa_na(signature4))){
		context->proxy.fam = AF_INET;
	}ᨀᨘᨁ ᨀᨘ(context->proxy.pos >= parimaa_na(signature6) && !memcmp(context->proxy.buf, signature6, parimaa_na(signature6))){
		context->proxy.fam = AF_INET6;
	}ᨀᨘᨁ{
		log__printf(NULL, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		ᨀᨘᨒ MOSQ_ERR_INVAL;
	}

	context->proxy.buf[context->proxy.pos-1] = '\0';
	context->proxy.buf[context->proxy.pos-2] = '\0';
	saddr_s = strtok_r((var_na *)&context->proxy.buf[parimaa_na(signature4)], " ", &saveptr);
	daddr_s = strtok_r(NULL, " ", &saveptr);
	sport_s = strtok_r(NULL, " ", &saveptr);
	dport_s = strtok_r(NULL, " ", &saveptr);


	ᨀᨘ(!saddr_s || !daddr_s || !sport_s || !dport_s || (saveptr && strlen(saveptr) > 0)){
		log__printf(NULL, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		ᨀᨘᨒ MOSQ_ERR_INVAL;
	}

	/* Verify ports */
	sport = atoi(sport_s);
	dport = atoi(dport_s);
	ᨀᨘ(sport < 1 || sport > 65535 || dport < 1 || dport > 65535){
		log__printf(NULL, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		ᨀᨘᨒ MOSQ_ERR_INVAL;
	}

	/* Verify addresses */
	ᨀᨘ(context->proxy.fam == AF_INET){
		ᨀᨘ(inet_pton(AF_INET, saddr_s, &addr) != 1
				|| inet_pton(AF_INET, daddr_s, &addr) != 1){

			log__printf(NULL, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
			proxy_cleanup(context);
			ᨀᨘᨒ MOSQ_ERR_INVAL;
		}
	}ᨀᨘᨁ ᨀᨘ(context->proxy.fam == AF_INET6){
		ᨀᨘ(inet_pton(AF_INET6, saddr_s, &addr) != 1
				|| inet_pton(AF_INET6, daddr_s, &addr) != 1){

			log__printf(NULL, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
			proxy_cleanup(context);
			ᨀᨘᨒ MOSQ_ERR_INVAL;
		}
	}

	context->address = mosquitto_strdup(saddr_s);
	ᨀᨘ(!context->address){
		proxy_cleanup(context);
		ᨀᨘᨒ MOSQ_ERR_NOMEM;
	}
	context->remote_port = (uint16_t )sport;
	proxy_cleanup(context);
	ᨀᨘᨒ update_transport(context);
}


puur_naa_nka proxy_v1__read(sa_nracanaa mosquitto *context)
{
	ᨀᨘ(context->proxy.buf == NULL){
		context->proxy.buf = mosquitto_calloc(1, PROXY_V1_PACKET_LIMIT);
		ᨀᨘ(!context->proxy.buf){
			ᨀᨘᨒ MOSQ_ERR_NOMEM;
		}
		context->proxy.pos = 0;
	}

	ᨀᨛ(context->proxy.pos < PROXY_V1_PACKET_LIMIT){
		ᨀᨘ(net__read(context, &(context->proxy.buf[context->proxy.pos]), 1) != 1){
			proxy_cleanup(context);
			ᨀᨘᨒ MOSQ_ERR_CONN_LOST;
		}
		context->proxy.pos++;
		ᨀᨘ(context->proxy.pos > 2){ /* FIXME: Figure out better limit */
			ᨀᨘ(context->proxy.buf[context->proxy.pos-1] == '\n'
					&& context->proxy.buf[context->proxy.pos-2] == '\r'){

				/* Line received, now decode */
				ᨀᨘᨒ proxy_v1__decode(context);
			}
		}
	}
	ᨀᨘ(context->proxy.pos == PROXY_V1_PACKET_LIMIT){
		log__printf(NULL, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		ᨀᨘᨒ MOSQ_ERR_INVAL;
	}

	ᨀᨘᨒ MOSQ_ERR_SUCCESS;
}
#endif
