#ifdef WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#નહીંતર
#  include <arpa/inet.h>
#  include <netinet/in.h>
#endif
#અન્તર્ભાવ <stdint.h>
#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto_internal.h"
#અન્તર્ભાવ "net_mosq.h"

#જો !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN

#define PROXY_V1_PACKET_LIMIT 108

અચલ uint8_t signature4[11] = {'P', 'R', 'O', 'X', 'Y', ' ', 'T', 'C', 'P', '4', ' '};
અચલ uint8_t signature6[11] = {'P', 'R', 'O', 'X', 'Y', ' ', 'T', 'C', 'P', '6', ' '};
અચલ uint8_t signatureU[14] = {'P', 'R', 'O', 'X', 'Y', ' ', 'U', 'N', 'K', 'N', 'O', 'W', 'N', ' '};


સ્થિર શૂન્ય proxy_cleanup(સંરચના mosquitto *context)
{
	mosquitto_FREE(context->proxy.buf);
}


સ્થિર પૂર્ણાંક update_transport(સંરચના mosquitto *context)
{
#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	જો(context->listener->protocol == mp_websockets){
		પાછા http__context_init(context);
	}નહીંતર
#endif
	{
		context->transport = mosq_t_tcp;
	}
	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક get_address_for_unknown(સંરચના mosquitto *context)
{
	અક્ષર address[1024];

	proxy_cleanup(context);

	જો(!net__socket_get_address(context->sock, address, કદ(address), &context->remote_port)){
		context->address = mosquitto_strdup(address);
	}
	જો(!context->address){
		પાછા MOSQ_ERR_NOMEM;
	}
	પાછા update_transport(context);
}


સ્થિર પૂર્ણાંક proxy_v1__decode(સંરચના mosquitto *context)
{
	અક્ષર *saddr_s, *daddr_s, *sport_s, *dport_s;
	અક્ષર *saveptr = ખાલી;
	પૂર્ણાંક sport, dport;
	સંરચના in6_addr addr;

	જો(context->proxy.pos >= કદ(signatureU) && !memcmp(context->proxy.buf, signatureU, કદ(signatureU))){
		પાછા get_address_for_unknown(context);
	}નહીંતર જો(context->proxy.pos >= કદ(signature4) && !memcmp(context->proxy.buf, signature4, કદ(signature4))){
		context->proxy.fam = AF_INET;
	}નહીંતર જો(context->proxy.pos >= કદ(signature6) && !memcmp(context->proxy.buf, signature6, કદ(signature6))){
		context->proxy.fam = AF_INET6;
	}નહીંતર{
		log__printf(ખાલી, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		પાછા MOSQ_ERR_INVAL;
	}

	context->proxy.buf[context->proxy.pos-1] = '\0';
	context->proxy.buf[context->proxy.pos-2] = '\0';
	saddr_s = strtok_r((અક્ષર *)&context->proxy.buf[કદ(signature4)], " ", &saveptr);
	daddr_s = strtok_r(ખાલી, " ", &saveptr);
	sport_s = strtok_r(ખાલી, " ", &saveptr);
	dport_s = strtok_r(ખાલી, " ", &saveptr);


	જો(!saddr_s || !daddr_s || !sport_s || !dport_s || (saveptr && strlen(saveptr) > 0)){
		log__printf(ખાલી, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		પાછા MOSQ_ERR_INVAL;
	}

	/* Verify ports */
	sport = atoi(sport_s);
	dport = atoi(dport_s);
	જો(sport < 1 || sport > 65535 || dport < 1 || dport > 65535){
		log__printf(ખાલી, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		પાછા MOSQ_ERR_INVAL;
	}

	/* Verify addresses */
	જો(context->proxy.fam == AF_INET){
		જો(inet_pton(AF_INET, saddr_s, &addr) != 1
				|| inet_pton(AF_INET, daddr_s, &addr) != 1){

			log__printf(ખાલી, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
			proxy_cleanup(context);
			પાછા MOSQ_ERR_INVAL;
		}
	}નહીંતર જો(context->proxy.fam == AF_INET6){
		જો(inet_pton(AF_INET6, saddr_s, &addr) != 1
				|| inet_pton(AF_INET6, daddr_s, &addr) != 1){

			log__printf(ખાલી, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
			proxy_cleanup(context);
			પાછા MOSQ_ERR_INVAL;
		}
	}

	context->address = mosquitto_strdup(saddr_s);
	જો(!context->address){
		proxy_cleanup(context);
		પાછા MOSQ_ERR_NOMEM;
	}
	context->remote_port = (uint16_t )sport;
	proxy_cleanup(context);
	પાછા update_transport(context);
}


પૂર્ણાંક proxy_v1__read(સંરચના mosquitto *context)
{
	જો(context->proxy.buf == ખાલી){
		context->proxy.buf = mosquitto_calloc(1, PROXY_V1_PACKET_LIMIT);
		જો(!context->proxy.buf){
			પાછા MOSQ_ERR_NOMEM;
		}
		context->proxy.pos = 0;
	}

	જ્યારે(context->proxy.pos < PROXY_V1_PACKET_LIMIT){
		જો(net__read(context, &(context->proxy.buf[context->proxy.pos]), 1) != 1){
			proxy_cleanup(context);
			પાછા MOSQ_ERR_CONN_LOST;
		}
		context->proxy.pos++;
		જો(context->proxy.pos > 2){ /* FIXME: Figure out better limit */
			જો(context->proxy.buf[context->proxy.pos-1] == '\n'
					&& context->proxy.buf[context->proxy.pos-2] == '\r'){

				/* Line received, now decode */
				પાછા proxy_v1__decode(context);
			}
		}
	}
	જો(context->proxy.pos == PROXY_V1_PACKET_LIMIT){
		log__printf(ખાલી, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		પાછા MOSQ_ERR_INVAL;
	}

	પાછા MOSQ_ERR_SUCCESS;
}
#endif
