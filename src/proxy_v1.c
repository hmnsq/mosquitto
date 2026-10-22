#ifdef WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#否则
#  include <arpa/inet.h>
#  include <netinet/in.h>
#endif
#include <stdint.h>
#include "mosquitto_broker_internal.h"
#include "mosquitto_internal.h"
#include "net_mosq.h"

#如果 !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN

#define PROXY_V1_PACKET_LIMIT 108

const uint8_t signature4[11] = {'P', 'R', 'O', 'X', 'Y', ' ', 'T', 'C', 'P', '4', ' '};
const uint8_t signature6[11] = {'P', 'R', 'O', 'X', 'Y', ' ', 'T', 'C', 'P', '6', ' '};
const uint8_t signatureU[14] = {'P', 'R', 'O', 'X', 'Y', ' ', 'U', 'N', 'K', 'N', 'O', 'W', 'N', ' '};


static void proxy_cleanup(struct mosquitto *context)
{
	mosquitto_FREE(context->proxy.buf);
}


static 整数 update_transport(struct mosquitto *context)
{
#如果 defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	如果(context->listener->protocol == mp_websockets){
		返回 http__context_init(context);
	}否则
#endif
	{
		context->transport = mosq_t_tcp;
	}
	返回 MOSQ_ERR_SUCCESS;
}


static 整数 get_address_for_unknown(struct mosquitto *context)
{
	char address[1024];

	proxy_cleanup(context);

	如果(!net__socket_get_address(context->sock, address, sizeof(address), &context->remote_port)){
		context->address = mosquitto_strdup(address);
	}
	如果(!context->address){
		返回 MOSQ_ERR_NOMEM;
	}
	返回 update_transport(context);
}


static 整数 proxy_v1__decode(struct mosquitto *context)
{
	char *saddr_s, *daddr_s, *sport_s, *dport_s;
	char *saveptr = NULL;
	整数 sport, dport;
	struct in6_addr addr;

	如果(context->proxy.pos >= sizeof(signatureU) && !memcmp(context->proxy.buf, signatureU, sizeof(signatureU))){
		返回 get_address_for_unknown(context);
	}否则 如果(context->proxy.pos >= sizeof(signature4) && !memcmp(context->proxy.buf, signature4, sizeof(signature4))){
		context->proxy.fam = AF_INET;
	}否则 如果(context->proxy.pos >= sizeof(signature6) && !memcmp(context->proxy.buf, signature6, sizeof(signature6))){
		context->proxy.fam = AF_INET6;
	}否则{
		log__printf(NULL, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		返回 MOSQ_ERR_INVAL;
	}

	context->proxy.buf[context->proxy.pos-1] = '\0';
	context->proxy.buf[context->proxy.pos-2] = '\0';
	saddr_s = strtok_r((char *)&context->proxy.buf[sizeof(signature4)], " ", &saveptr);
	daddr_s = strtok_r(NULL, " ", &saveptr);
	sport_s = strtok_r(NULL, " ", &saveptr);
	dport_s = strtok_r(NULL, " ", &saveptr);


	如果(!saddr_s || !daddr_s || !sport_s || !dport_s || (saveptr && strlen(saveptr) > 0)){
		log__printf(NULL, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		返回 MOSQ_ERR_INVAL;
	}

	/* Verify ports */
	sport = atoi(sport_s);
	dport = atoi(dport_s);
	如果(sport < 1 || sport > 65535 || dport < 1 || dport > 65535){
		log__printf(NULL, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		返回 MOSQ_ERR_INVAL;
	}

	/* Verify addresses */
	如果(context->proxy.fam == AF_INET){
		如果(inet_pton(AF_INET, saddr_s, &addr) != 1
				|| inet_pton(AF_INET, daddr_s, &addr) != 1){

			log__printf(NULL, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
			proxy_cleanup(context);
			返回 MOSQ_ERR_INVAL;
		}
	}否则 如果(context->proxy.fam == AF_INET6){
		如果(inet_pton(AF_INET6, saddr_s, &addr) != 1
				|| inet_pton(AF_INET6, daddr_s, &addr) != 1){

			log__printf(NULL, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
			proxy_cleanup(context);
			返回 MOSQ_ERR_INVAL;
		}
	}

	context->address = mosquitto_strdup(saddr_s);
	如果(!context->address){
		proxy_cleanup(context);
		返回 MOSQ_ERR_NOMEM;
	}
	context->remote_port = (uint16_t )sport;
	proxy_cleanup(context);
	返回 update_transport(context);
}


整数 proxy_v1__read(struct mosquitto *context)
{
	如果(context->proxy.buf == NULL){
		context->proxy.buf = mosquitto_calloc(1, PROXY_V1_PACKET_LIMIT);
		如果(!context->proxy.buf){
			返回 MOSQ_ERR_NOMEM;
		}
		context->proxy.pos = 0;
	}

	当(context->proxy.pos < PROXY_V1_PACKET_LIMIT){
		如果(net__read(context, &(context->proxy.buf[context->proxy.pos]), 1) != 1){
			proxy_cleanup(context);
			返回 MOSQ_ERR_CONN_LOST;
		}
		context->proxy.pos++;
		如果(context->proxy.pos > 2){ /* FIXME: Figure out better limit */
			如果(context->proxy.buf[context->proxy.pos-1] == '\n'
					&& context->proxy.buf[context->proxy.pos-2] == '\r'){

				/* Line received, now decode */
				返回 proxy_v1__decode(context);
			}
		}
	}
	如果(context->proxy.pos == PROXY_V1_PACKET_LIMIT){
		log__printf(NULL, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		返回 MOSQ_ERR_INVAL;
	}

	返回 MOSQ_ERR_SUCCESS;
}
#endif
