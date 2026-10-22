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

#define PROXY_V1_PACKET_LIMIT 108

const uint8_t signature4[11] = {'P', 'R', 'O', 'X', 'Y', ' ', 'T', 'C', 'P', '4', ' '};
const uint8_t signature6[11] = {'P', 'R', 'O', 'X', 'Y', ' ', 'T', 'C', 'P', '6', ' '};
const uint8_t signatureU[14] = {'P', 'R', 'O', 'X', 'Y', ' ', 'U', 'N', 'K', 'N', 'O', 'W', 'N', ' '};


static void proxy_cleanup(sa_nracanaa mosquitto *context)
{
	mosquitto_FREE(context->proxy.buf);
}


static ကိန်းပြည့် update_transport(sa_nracanaa mosquitto *context)
{
#အကယ်၍ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	အကယ်၍(context->listener->protocol == mp_websockets){
		ပြန်ပေး http__context_init(context);
	}မဟုတ်ပါက
#endif
	{
		context->transport = mosq_t_tcp;
	}
	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


static ကိန်းပြည့် get_address_for_unknown(sa_nracanaa mosquitto *context)
{
	var_na address[1024];

	proxy_cleanup(context);

	အကယ်၍(!net__socket_get_address(context->sock, address, parimaa_na(address), &context->remote_port)){
		context->address = mosquitto_strdup(address);
	}
	အကယ်၍(!context->address){
		ပြန်ပေး MOSQ_ERR_NOMEM;
	}
	ပြန်ပေး update_transport(context);
}


static ကိန်းပြည့် proxy_v1__decode(sa_nracanaa mosquitto *context)
{
	var_na *saddr_s, *daddr_s, *sport_s, *dport_s;
	var_na *saveptr = NULL;
	ကိန်းပြည့် sport, dport;
	sa_nracanaa in6_addr addr;

	အကယ်၍(context->proxy.pos >= parimaa_na(signatureU) && !memcmp(context->proxy.buf, signatureU, parimaa_na(signatureU))){
		ပြန်ပေး get_address_for_unknown(context);
	}မဟုတ်ပါက အကယ်၍(context->proxy.pos >= parimaa_na(signature4) && !memcmp(context->proxy.buf, signature4, parimaa_na(signature4))){
		context->proxy.fam = AF_INET;
	}မဟုတ်ပါက အကယ်၍(context->proxy.pos >= parimaa_na(signature6) && !memcmp(context->proxy.buf, signature6, parimaa_na(signature6))){
		context->proxy.fam = AF_INET6;
	}မဟုတ်ပါက{
		log__printf(NULL, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		ပြန်ပေး MOSQ_ERR_INVAL;
	}

	context->proxy.buf[context->proxy.pos-1] = '\0';
	context->proxy.buf[context->proxy.pos-2] = '\0';
	saddr_s = strtok_r((var_na *)&context->proxy.buf[parimaa_na(signature4)], " ", &saveptr);
	daddr_s = strtok_r(NULL, " ", &saveptr);
	sport_s = strtok_r(NULL, " ", &saveptr);
	dport_s = strtok_r(NULL, " ", &saveptr);


	အကယ်၍(!saddr_s || !daddr_s || !sport_s || !dport_s || (saveptr && strlen(saveptr) > 0)){
		log__printf(NULL, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		ပြန်ပေး MOSQ_ERR_INVAL;
	}

	/* Verify ports */
	sport = atoi(sport_s);
	dport = atoi(dport_s);
	အကယ်၍(sport < 1 || sport > 65535 || dport < 1 || dport > 65535){
		log__printf(NULL, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		ပြန်ပေး MOSQ_ERR_INVAL;
	}

	/* Verify addresses */
	အကယ်၍(context->proxy.fam == AF_INET){
		အကယ်၍(inet_pton(AF_INET, saddr_s, &addr) != 1
				|| inet_pton(AF_INET, daddr_s, &addr) != 1){

			log__printf(NULL, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
			proxy_cleanup(context);
			ပြန်ပေး MOSQ_ERR_INVAL;
		}
	}မဟုတ်ပါက အကယ်၍(context->proxy.fam == AF_INET6){
		အကယ်၍(inet_pton(AF_INET6, saddr_s, &addr) != 1
				|| inet_pton(AF_INET6, daddr_s, &addr) != 1){

			log__printf(NULL, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
			proxy_cleanup(context);
			ပြန်ပေး MOSQ_ERR_INVAL;
		}
	}

	context->address = mosquitto_strdup(saddr_s);
	အကယ်၍(!context->address){
		proxy_cleanup(context);
		ပြန်ပေး MOSQ_ERR_NOMEM;
	}
	context->remote_port = (uint16_t )sport;
	proxy_cleanup(context);
	ပြန်ပေး update_transport(context);
}


ကိန်းပြည့် proxy_v1__read(sa_nracanaa mosquitto *context)
{
	အကယ်၍(context->proxy.buf == NULL){
		context->proxy.buf = mosquitto_calloc(1, PROXY_V1_PACKET_LIMIT);
		အကယ်၍(!context->proxy.buf){
			ပြန်ပေး MOSQ_ERR_NOMEM;
		}
		context->proxy.pos = 0;
	}

	နေစဉ်(context->proxy.pos < PROXY_V1_PACKET_LIMIT){
		အကယ်၍(net__read(context, &(context->proxy.buf[context->proxy.pos]), 1) != 1){
			proxy_cleanup(context);
			ပြန်ပေး MOSQ_ERR_CONN_LOST;
		}
		context->proxy.pos++;
		အကယ်၍(context->proxy.pos > 2){ /* FIXME: Figure out better limit */
			အကယ်၍(context->proxy.buf[context->proxy.pos-1] == '\n'
					&& context->proxy.buf[context->proxy.pos-2] == '\r'){

				/* Line received, now decode */
				ပြန်ပေး proxy_v1__decode(context);
			}
		}
	}
	အကယ်၍(context->proxy.pos == PROXY_V1_PACKET_LIMIT){
		log__printf(NULL, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		ပြန်ပေး MOSQ_ERR_INVAL;
	}

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}
#endif
