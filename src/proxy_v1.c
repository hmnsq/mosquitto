#ifdef WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#אחרת
#  include <arpa/inet.h>
#  include <netinet/in.h>
#endif
#ضمن <stdint.h>
#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto_internal.h"
#ضمن "net_mosq.h"

#אם !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN

#define PROXY_V1_PACKET_LIMIT 108

ثابت uint8_t signature4[11] = {'P', 'R', 'O', 'X', 'Y', ' ', 'T', 'C', 'P', '4', ' '};
ثابت uint8_t signature6[11] = {'P', 'R', 'O', 'X', 'Y', ' ', 'T', 'C', 'P', '6', ' '};
ثابت uint8_t signatureU[14] = {'P', 'R', 'O', 'X', 'Y', ' ', 'U', 'N', 'K', 'N', 'O', 'W', 'N', ' '};


ساكن فراغ proxy_cleanup(بنية mosquitto *context)
{
	mosquitto_FREE(context->proxy.buf);
}


ساكن صحيح update_transport(بنية mosquitto *context)
{
#אם defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	אם(context->listener->protocol == mp_websockets){
		החזר http__context_init(context);
	}אחרת
#endif
	{
		context->transport = mosq_t_tcp;
	}
	החזר MOSQ_ERR_SUCCESS;
}


ساكن صحيح get_address_for_unknown(بنية mosquitto *context)
{
	حرف address[1024];

	proxy_cleanup(context);

	אם(!net__socket_get_address(context->sock, address, حجم(address), &context->remote_port)){
		context->address = mosquitto_strdup(address);
	}
	אם(!context->address){
		החזר MOSQ_ERR_NOMEM;
	}
	החזר update_transport(context);
}


ساكن صحيح proxy_v1__decode(بنية mosquitto *context)
{
	حرف *saddr_s, *daddr_s, *sport_s, *dport_s;
	حرف *saveptr = عدم;
	صحيح sport, dport;
	بنية in6_addr addr;

	אם(context->proxy.pos >= حجم(signatureU) && !memcmp(context->proxy.buf, signatureU, حجم(signatureU))){
		החזר get_address_for_unknown(context);
	}אחרת אם(context->proxy.pos >= حجم(signature4) && !memcmp(context->proxy.buf, signature4, حجم(signature4))){
		context->proxy.fam = AF_INET;
	}אחרת אם(context->proxy.pos >= حجم(signature6) && !memcmp(context->proxy.buf, signature6, حجم(signature6))){
		context->proxy.fam = AF_INET6;
	}אחרת{
		log__printf(عدم, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		החזר MOSQ_ERR_INVAL;
	}

	context->proxy.buf[context->proxy.pos-1] = '\0';
	context->proxy.buf[context->proxy.pos-2] = '\0';
	saddr_s = strtok_r((حرف *)&context->proxy.buf[حجم(signature4)], " ", &saveptr);
	daddr_s = strtok_r(عدم, " ", &saveptr);
	sport_s = strtok_r(عدم, " ", &saveptr);
	dport_s = strtok_r(عدم, " ", &saveptr);


	אם(!saddr_s || !daddr_s || !sport_s || !dport_s || (saveptr && strlen(saveptr) > 0)){
		log__printf(عدم, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		החזר MOSQ_ERR_INVAL;
	}

	/* Verify ports */
	sport = atoi(sport_s);
	dport = atoi(dport_s);
	אם(sport < 1 || sport > 65535 || dport < 1 || dport > 65535){
		log__printf(عدم, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		החזר MOSQ_ERR_INVAL;
	}

	/* Verify addresses */
	אם(context->proxy.fam == AF_INET){
		אם(inet_pton(AF_INET, saddr_s, &addr) != 1
				|| inet_pton(AF_INET, daddr_s, &addr) != 1){

			log__printf(عدم, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
			proxy_cleanup(context);
			החזר MOSQ_ERR_INVAL;
		}
	}אחרת אם(context->proxy.fam == AF_INET6){
		אם(inet_pton(AF_INET6, saddr_s, &addr) != 1
				|| inet_pton(AF_INET6, daddr_s, &addr) != 1){

			log__printf(عدم, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
			proxy_cleanup(context);
			החזר MOSQ_ERR_INVAL;
		}
	}

	context->address = mosquitto_strdup(saddr_s);
	אם(!context->address){
		proxy_cleanup(context);
		החזר MOSQ_ERR_NOMEM;
	}
	context->remote_port = (uint16_t )sport;
	proxy_cleanup(context);
	החזר update_transport(context);
}


صحيح proxy_v1__read(بنية mosquitto *context)
{
	אם(context->proxy.buf == عدم){
		context->proxy.buf = mosquitto_calloc(1, PROXY_V1_PACKET_LIMIT);
		אם(!context->proxy.buf){
			החזר MOSQ_ERR_NOMEM;
		}
		context->proxy.pos = 0;
	}

	כלעוד(context->proxy.pos < PROXY_V1_PACKET_LIMIT){
		אם(net__read(context, &(context->proxy.buf[context->proxy.pos]), 1) != 1){
			proxy_cleanup(context);
			החזר MOSQ_ERR_CONN_LOST;
		}
		context->proxy.pos++;
		אם(context->proxy.pos > 2){ /* FIXME: Figure out better limit */
			אם(context->proxy.buf[context->proxy.pos-1] == '\n'
					&& context->proxy.buf[context->proxy.pos-2] == '\r'){

				/* Line received, now decode */
				החזר proxy_v1__decode(context);
			}
		}
	}
	אם(context->proxy.pos == PROXY_V1_PACKET_LIMIT){
		log__printf(عدم, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		החזר MOSQ_ERR_INVAL;
	}

	החזר MOSQ_ERR_SUCCESS;
}
#endif
