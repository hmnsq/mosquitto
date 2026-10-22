#ifdef WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#ଅନ୍ୟଥା
#  include <arpa/inet.h>
#  include <netinet/in.h>
#endif
#ଅନ୍ତର୍ଭାଵ <stdint.h>
#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto_internal.h"
#ଅନ୍ତର୍ଭାଵ "net_mosq.h"

#ଯଦି !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN

#define PROXY_V1_PACKET_LIMIT 108

ଅଚଳ uint8_t signature4[11] = {'P', 'R', 'O', 'X', 'Y', ' ', 'T', 'C', 'P', '4', ' '};
ଅଚଳ uint8_t signature6[11] = {'P', 'R', 'O', 'X', 'Y', ' ', 'T', 'C', 'P', '6', ' '};
ଅଚଳ uint8_t signatureU[14] = {'P', 'R', 'O', 'X', 'Y', ' ', 'U', 'N', 'K', 'N', 'O', 'W', 'N', ' '};


ସ୍ଥିର ଶୂନ୍ୟ proxy_cleanup(ସଂରଚନା mosquitto *context)
{
	mosquitto_FREE(context->proxy.buf);
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା update_transport(ସଂରଚନା mosquitto *context)
{
#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	ଯଦି(context->listener->protocol == mp_websockets){
		ଫେରନ୍ତୁ http__context_init(context);
	}ଅନ୍ୟଥା
#endif
	{
		context->transport = mosq_t_tcp;
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_address_for_unknown(ସଂରଚନା mosquitto *context)
{
	ଅକ୍ଷର address[1024];

	proxy_cleanup(context);

	ଯଦି(!net__socket_get_address(context->sock, address, ଆକାର(address), &context->remote_port)){
		context->address = mosquitto_strdup(address);
	}
	ଯଦି(!context->address){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}
	ଫେରନ୍ତୁ update_transport(context);
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା proxy_v1__decode(ସଂରଚନା mosquitto *context)
{
	ଅକ୍ଷର *saddr_s, *daddr_s, *sport_s, *dport_s;
	ଅକ୍ଷର *saveptr = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା sport, dport;
	ସଂରଚନା in6_addr addr;

	ଯଦି(context->proxy.pos >= ଆକାର(signatureU) && !memcmp(context->proxy.buf, signatureU, ଆକାର(signatureU))){
		ଫେରନ୍ତୁ get_address_for_unknown(context);
	}ଅନ୍ୟଥା ଯଦି(context->proxy.pos >= ଆକାର(signature4) && !memcmp(context->proxy.buf, signature4, ଆକାର(signature4))){
		context->proxy.fam = AF_INET;
	}ଅନ୍ୟଥା ଯଦି(context->proxy.pos >= ଆକାର(signature6) && !memcmp(context->proxy.buf, signature6, ଆକାର(signature6))){
		context->proxy.fam = AF_INET6;
	}ଅନ୍ୟଥା{
		log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	context->proxy.buf[context->proxy.pos-1] = '\0';
	context->proxy.buf[context->proxy.pos-2] = '\0';
	saddr_s = strtok_r((ଅକ୍ଷର *)&context->proxy.buf[ଆକାର(signature4)], " ", &saveptr);
	daddr_s = strtok_r(ଖାଲି, " ", &saveptr);
	sport_s = strtok_r(ଖାଲି, " ", &saveptr);
	dport_s = strtok_r(ଖାଲି, " ", &saveptr);


	ଯଦି(!saddr_s || !daddr_s || !sport_s || !dport_s || (saveptr && strlen(saveptr) > 0)){
		log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	/* Verify ports */
	sport = atoi(sport_s);
	dport = atoi(dport_s);
	ଯଦି(sport < 1 || sport > 65535 || dport < 1 || dport > 65535){
		log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	/* Verify addresses */
	ଯଦି(context->proxy.fam == AF_INET){
		ଯଦି(inet_pton(AF_INET, saddr_s, &addr) != 1
				|| inet_pton(AF_INET, daddr_s, &addr) != 1){

			log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
			proxy_cleanup(context);
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
	}ଅନ୍ୟଥା ଯଦି(context->proxy.fam == AF_INET6){
		ଯଦି(inet_pton(AF_INET6, saddr_s, &addr) != 1
				|| inet_pton(AF_INET6, daddr_s, &addr) != 1){

			log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
			proxy_cleanup(context);
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
	}

	context->address = mosquitto_strdup(saddr_s);
	ଯଦି(!context->address){
		proxy_cleanup(context);
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}
	context->remote_port = (uint16_t )sport;
	proxy_cleanup(context);
	ଫେରନ୍ତୁ update_transport(context);
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା proxy_v1__read(ସଂରଚନା mosquitto *context)
{
	ଯଦି(context->proxy.buf == ଖାଲି){
		context->proxy.buf = mosquitto_calloc(1, PROXY_V1_PACKET_LIMIT);
		ଯଦି(!context->proxy.buf){
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}
		context->proxy.pos = 0;
	}

	ଯେତେବେଳେ(context->proxy.pos < PROXY_V1_PACKET_LIMIT){
		ଯଦି(net__read(context, &(context->proxy.buf[context->proxy.pos]), 1) != 1){
			proxy_cleanup(context);
			ଫେରନ୍ତୁ MOSQ_ERR_CONN_LOST;
		}
		context->proxy.pos++;
		ଯଦି(context->proxy.pos > 2){ /* FIXME: Figure out better limit */
			ଯଦି(context->proxy.buf[context->proxy.pos-1] == '\n'
					&& context->proxy.buf[context->proxy.pos-2] == '\r'){

				/* Line received, now decode */
				ଫେରନ୍ତୁ proxy_v1__decode(context);
			}
		}
	}
	ଯଦି(context->proxy.pos == PROXY_V1_PACKET_LIMIT){
		log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}
#endif
