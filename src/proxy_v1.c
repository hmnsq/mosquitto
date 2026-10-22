#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#ਨਹੀਂਤਾਂ
#  include <arpa/inet.h>
#  include <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "net_mosq.h"

#ਜੇ !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROXY_V1_PACKET_ਹੱਦਬੰਦੀ 108

ਅਚਲ uint8_t signature4[11] = {'P', 'R', 'O', 'X', 'Y', ' ', 'T', 'C', 'P', '4', ' '};
ਅਚਲ uint8_t signature6[11] = {'P', 'R', 'O', 'X', 'Y', ' ', 'T', 'C', 'P', '6', ' '};
ਅਚਲ uint8_t signatureU[14] = {'P', 'R', 'O', 'X', 'Y', ' ', 'U', 'N', 'K', 'N', 'O', 'W', 'N', ' '};


ਸਥਿਰ ਖਾਲੀ proxy_cleanup(ਬਣਤਰ mosquitto *context)
{
	mosquitto_FREE(context->proxy.buf);
}


ਸਥਿਰ ਪੂਰਨ update_transport(ਬਣਤਰ mosquitto *context)
{
#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	ਜੇ(context->listener->protocol == mp_websockets){
		ਵਾਪਸ http__context_init(context);
	}ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	{
		context->transport = mosq_t_tcp;
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ get_ਪਤਾ_for_unknown(ਬਣਤਰ mosquitto *context)
{
	ਅੱਖਰ ਪਤਾ[1024];

	proxy_cleanup(context);

	ਜੇ(!net_socket_get_ਪਤਾ(context->sock, ਪਤਾ, ਆਕਾਰ(ਪਤਾ), &context->remote_ਪੋਰਟ)){
		context->ਪਤਾ = mosquitto_strdup(ਪਤਾ);
	}
	ਜੇ(!context->ਪਤਾ){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}
	ਵਾਪਸ update_transport(context);
}


ਸਥਿਰ ਪੂਰਨ proxy_v1__decode(ਬਣਤਰ mosquitto *context)
{
	ਅੱਖਰ *saddr_s, *daddr_s, *sport_s, *dport_s;
	ਅੱਖਰ *saveptr = ਨਿਰਾ;
	ਪੂਰਨ sport, dport;
	ਬਣਤਰ in6_addr addr;

	ਜੇ(context->proxy.pos >= ਆਕਾਰ(signatureU) && !memcmp(context->proxy.buf, signatureU, ਆਕਾਰ(signatureU))){
		ਵਾਪਸ get_ਪਤਾ_for_unknown(context);
	}ਨਹੀਂਤਾਂ ਜੇ(context->proxy.pos >= ਆਕਾਰ(signature4) && !memcmp(context->proxy.buf, signature4, ਆਕਾਰ(signature4))){
		context->proxy.fam = AF_INET;
	}ਨਹੀਂਤਾਂ ਜੇ(context->proxy.pos >= ਆਕਾਰ(signature6) && !memcmp(context->proxy.buf, signature6, ਆਕਾਰ(signature6))){
		context->proxy.fam = AF_INET6;
	}ਨਹੀਂਤਾਂ{
		log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	context->proxy.buf[context->proxy.pos-1] = '\0';
	context->proxy.buf[context->proxy.pos-2] = '\0';
	saddr_s = strtok_r((ਅੱਖਰ *)&context->proxy.buf[ਆਕਾਰ(signature4)], " ", &saveptr);
	daddr_s = strtok_r(ਨਿਰਾ, " ", &saveptr);
	sport_s = strtok_r(ਨਿਰਾ, " ", &saveptr);
	dport_s = strtok_r(ਨਿਰਾ, " ", &saveptr);


	ਜੇ(!saddr_s || !daddr_s || !sport_s || !dport_s || (saveptr && ਲੜੀ_ਲੰਬਾਈ(saveptr) > 0)){
		log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	/* Verify ports */
	sport = atoi(sport_s);
	dport = atoi(dport_s);
	ਜੇ(sport < 1 || sport > 65535 || dport < 1 || dport > 65535){
		log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	/* Verify addresses */
	ਜੇ(context->proxy.fam == AF_INET){
		ਜੇ(inet_pton(AF_INET, saddr_s, &addr) != 1
				|| inet_pton(AF_INET, daddr_s, &addr) != 1){

			log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
			proxy_cleanup(context);
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
	}ਨਹੀਂਤਾਂ ਜੇ(context->proxy.fam == AF_INET6){
		ਜੇ(inet_pton(AF_INET6, saddr_s, &addr) != 1
				|| inet_pton(AF_INET6, daddr_s, &addr) != 1){

			log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
			proxy_cleanup(context);
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
	}

	context->ਪਤਾ = mosquitto_strdup(saddr_s);
	ਜੇ(!context->ਪਤਾ){
		proxy_cleanup(context);
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}
	context->remote_ਪੋਰਟ = (uint16_t )sport;
	proxy_cleanup(context);
	ਵਾਪਸ update_transport(context);
}


ਪੂਰਨ proxy_v1__read(ਬਣਤਰ mosquitto *context)
{
	ਜੇ(context->proxy.buf == ਨਿਰਾ){
		context->proxy.buf = mosquitto_calloc(1, PROXY_V1_PACKET_ਹੱਦਬੰਦੀ);
		ਜੇ(!context->proxy.buf){
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}
		context->proxy.pos = 0;
	}

	ਜਦੋਂ(context->proxy.pos < PROXY_V1_PACKET_ਹੱਦਬੰਦੀ){
		ਜੇ(net__read(context, &(context->proxy.buf[context->proxy.pos]), 1) != 1){
			proxy_cleanup(context);
			ਵਾਪਸ MOSQ_ERR_CONN_ਗੁਆਚਾ;
		}
		context->proxy.pos++;
		ਜੇ(context->proxy.pos > 2){ /* FIXME: Figure out better limit */
			ਜੇ(context->proxy.buf[context->proxy.pos-1] == '\n'
					&& context->proxy.buf[context->proxy.pos-2] == '\r'){

				/* Line received, now decode */
				ਵਾਪਸ proxy_v1__decode(context);
			}
		}
	}
	ਜੇ(context->proxy.pos == PROXY_V1_PACKET_ਹੱਦਬੰਦੀ){
		log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}
#ਜੇ_ਅੰਤ
