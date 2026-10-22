#ifdef WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#інакше
#  include <arpa/inet.h>
#  include <netinet/in.h>
#endif
#include <stdint.h>
#include "mosquitto_broker_internal.h"
#include "mosquitto_internal.h"
#include "net_mosq.h"

#якщо !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN

#define PROXY_V1_PACKET_LIMIT 108

const uint8_t signature4[11] = {'P', 'R', 'O', 'X', 'Y', ' ', 'T', 'C', 'P', '4', ' '};
const uint8_t signature6[11] = {'P', 'R', 'O', 'X', 'Y', ' ', 'T', 'C', 'P', '6', ' '};
const uint8_t signatureU[14] = {'P', 'R', 'O', 'X', 'Y', ' ', 'U', 'N', 'K', 'N', 'O', 'W', 'N', ' '};


static void proxy_cleanup(struct mosquitto *context)
{
	mosquitto_FREE(context->proxy.buf);
}


static ціле update_transport(struct mosquitto *context)
{
#якщо defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	якщо(context->listener->protocol == mp_websockets){
		повернути http__context_init(context);
	}інакше
#endif
	{
		context->transport = mosq_t_tcp;
	}
	повернути MOSQ_ERR_SUCCESS;
}


static ціле get_address_for_unknown(struct mosquitto *context)
{
	char address[1024];

	proxy_cleanup(context);

	якщо(!net__socket_get_address(context->sock, address, sizeof(address), &context->remote_port)){
		context->address = mosquitto_strdup(address);
	}
	якщо(!context->address){
		повернути MOSQ_ERR_NOMEM;
	}
	повернути update_transport(context);
}


static ціле proxy_v1__decode(struct mosquitto *context)
{
	char *saddr_s, *daddr_s, *sport_s, *dport_s;
	char *saveptr = NULL;
	ціле sport, dport;
	struct in6_addr addr;

	якщо(context->proxy.pos >= sizeof(signatureU) && !memcmp(context->proxy.buf, signatureU, sizeof(signatureU))){
		повернути get_address_for_unknown(context);
	}інакше якщо(context->proxy.pos >= sizeof(signature4) && !memcmp(context->proxy.buf, signature4, sizeof(signature4))){
		context->proxy.fam = AF_INET;
	}інакше якщо(context->proxy.pos >= sizeof(signature6) && !memcmp(context->proxy.buf, signature6, sizeof(signature6))){
		context->proxy.fam = AF_INET6;
	}інакше{
		log__printf(NULL, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		повернути MOSQ_ERR_INVAL;
	}

	context->proxy.buf[context->proxy.pos-1] = '\0';
	context->proxy.buf[context->proxy.pos-2] = '\0';
	saddr_s = strtok_r((char *)&context->proxy.buf[sizeof(signature4)], " ", &saveptr);
	daddr_s = strtok_r(NULL, " ", &saveptr);
	sport_s = strtok_r(NULL, " ", &saveptr);
	dport_s = strtok_r(NULL, " ", &saveptr);


	якщо(!saddr_s || !daddr_s || !sport_s || !dport_s || (saveptr && strlen(saveptr) > 0)){
		log__printf(NULL, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		повернути MOSQ_ERR_INVAL;
	}

	/* Verify ports */
	sport = atoi(sport_s);
	dport = atoi(dport_s);
	якщо(sport < 1 || sport > 65535 || dport < 1 || dport > 65535){
		log__printf(NULL, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		повернути MOSQ_ERR_INVAL;
	}

	/* Verify addresses */
	якщо(context->proxy.fam == AF_INET){
		якщо(inet_pton(AF_INET, saddr_s, &addr) != 1
				|| inet_pton(AF_INET, daddr_s, &addr) != 1){

			log__printf(NULL, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
			proxy_cleanup(context);
			повернути MOSQ_ERR_INVAL;
		}
	}інакше якщо(context->proxy.fam == AF_INET6){
		якщо(inet_pton(AF_INET6, saddr_s, &addr) != 1
				|| inet_pton(AF_INET6, daddr_s, &addr) != 1){

			log__printf(NULL, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
			proxy_cleanup(context);
			повернути MOSQ_ERR_INVAL;
		}
	}

	context->address = mosquitto_strdup(saddr_s);
	якщо(!context->address){
		proxy_cleanup(context);
		повернути MOSQ_ERR_NOMEM;
	}
	context->remote_port = (uint16_t )sport;
	proxy_cleanup(context);
	повернути update_transport(context);
}


ціле proxy_v1__read(struct mosquitto *context)
{
	якщо(context->proxy.buf == NULL){
		context->proxy.buf = mosquitto_calloc(1, PROXY_V1_PACKET_LIMIT);
		якщо(!context->proxy.buf){
			повернути MOSQ_ERR_NOMEM;
		}
		context->proxy.pos = 0;
	}

	поки(context->proxy.pos < PROXY_V1_PACKET_LIMIT){
		якщо(net__read(context, &(context->proxy.buf[context->proxy.pos]), 1) != 1){
			proxy_cleanup(context);
			повернути MOSQ_ERR_CONN_LOST;
		}
		context->proxy.pos++;
		якщо(context->proxy.pos > 2){ /* FIXME: Figure out better limit */
			якщо(context->proxy.buf[context->proxy.pos-1] == '\n'
					&& context->proxy.buf[context->proxy.pos-2] == '\r'){

				/* Line received, now decode */
				повернути proxy_v1__decode(context);
			}
		}
	}
	якщо(context->proxy.pos == PROXY_V1_PACKET_LIMIT){
		log__printf(NULL, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		повернути MOSQ_ERR_INVAL;
	}

	повернути MOSQ_ERR_SUCCESS;
}
#endif
