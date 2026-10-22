#अगर_घोषित WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#अन्यथा
#  include <arpa/inet.h>
#  include <netinet/in.h>
#पूर्ण_अगर
#समावेश <stdint.h>
#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto_internal.h"
#समावेश "net_mosq.h"

#अगर !defined(WITH_WEBSOCKETS) || WITH_WEBSOCKETS == WS_IS_BUILTIN

#घोषणा PROXY_V1_PACKET_LIMIT 108

स्थिर uint8_t signature4[11] = {'P', 'R', 'O', 'X', 'Y', ' ', 'T', 'C', 'P', '4', ' '};
स्थिर uint8_t signature6[11] = {'P', 'R', 'O', 'X', 'Y', ' ', 'T', 'C', 'P', '6', ' '};
स्थिर uint8_t signatureU[14] = {'P', 'R', 'O', 'X', 'Y', ' ', 'U', 'N', 'K', 'N', 'O', 'W', 'N', ' '};


अटल व्योम proxy_cleanup(काष्ठा mosquitto *context)
{
	mosquitto_FREE(context->proxy.buf);
}


अटल पूर्णांक update_transport(काष्ठा mosquitto *context)
{
#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	अगर(context->listener->protocol == mp_websockets){
		वापस http__context_init(context);
	}अन्यथा
#पूर्ण_अगर
	{
		context->transport = mosq_t_tcp;
	}
	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक get_address_for_unknown(काष्ठा mosquitto *context)
{
	अक्षर address[1024];

	proxy_cleanup(context);

	अगर(!net__socket_get_address(context->sock, address, माप(address), &context->remote_port)){
		context->address = mosquitto_strdup(address);
	}
	अगर(!context->address){
		वापस MOSQ_ERR_NOMEM;
	}
	वापस update_transport(context);
}


अटल पूर्णांक proxy_v1__decode(काष्ठा mosquitto *context)
{
	अक्षर *saddr_s, *daddr_s, *sport_s, *dport_s;
	अक्षर *saveptr = शून्य;
	पूर्णांक sport, dport;
	काष्ठा in6_addr addr;

	अगर(context->proxy.pos >= माप(signatureU) && !स_भेद(context->proxy.buf, signatureU, माप(signatureU))){
		वापस get_address_for_unknown(context);
	}अन्यथा अगर(context->proxy.pos >= माप(signature4) && !स_भेद(context->proxy.buf, signature4, माप(signature4))){
		context->proxy.fam = AF_INET;
	}अन्यथा अगर(context->proxy.pos >= माप(signature6) && !स_भेद(context->proxy.buf, signature6, माप(signature6))){
		context->proxy.fam = AF_INET6;
	}अन्यथा{
		log__printf(शून्य, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		वापस MOSQ_ERR_INVAL;
	}

	context->proxy.buf[context->proxy.pos-1] = '\0';
	context->proxy.buf[context->proxy.pos-2] = '\0';
	saddr_s = strtok_r((अक्षर *)&context->proxy.buf[माप(signature4)], " ", &saveptr);
	daddr_s = strtok_r(शून्य, " ", &saveptr);
	sport_s = strtok_r(शून्य, " ", &saveptr);
	dport_s = strtok_r(शून्य, " ", &saveptr);


	अगर(!saddr_s || !daddr_s || !sport_s || !dport_s || (saveptr && म_माप(saveptr) > 0)){
		log__printf(शून्य, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		वापस MOSQ_ERR_INVAL;
	}

	/* Verify ports */
	sport = म_से_प(sport_s);
	dport = म_से_प(dport_s);
	अगर(sport < 1 || sport > 65535 || dport < 1 || dport > 65535){
		log__printf(शून्य, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		वापस MOSQ_ERR_INVAL;
	}

	/* Verify addresses */
	अगर(context->proxy.fam == AF_INET){
		अगर(inet_pton(AF_INET, saddr_s, &addr) != 1
				|| inet_pton(AF_INET, daddr_s, &addr) != 1){

			log__printf(शून्य, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
			proxy_cleanup(context);
			वापस MOSQ_ERR_INVAL;
		}
	}अन्यथा अगर(context->proxy.fam == AF_INET6){
		अगर(inet_pton(AF_INET6, saddr_s, &addr) != 1
				|| inet_pton(AF_INET6, daddr_s, &addr) != 1){

			log__printf(शून्य, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
			proxy_cleanup(context);
			वापस MOSQ_ERR_INVAL;
		}
	}

	context->address = mosquitto_strdup(saddr_s);
	अगर(!context->address){
		proxy_cleanup(context);
		वापस MOSQ_ERR_NOMEM;
	}
	context->remote_port = (uint16_t )sport;
	proxy_cleanup(context);
	वापस update_transport(context);
}


पूर्णांक proxy_v1__read(काष्ठा mosquitto *context)
{
	अगर(context->proxy.buf == शून्य){
		context->proxy.buf = mosquitto_calloc(1, PROXY_V1_PACKET_LIMIT);
		अगर(!context->proxy.buf){
			वापस MOSQ_ERR_NOMEM;
		}
		context->proxy.pos = 0;
	}

	जबतक(context->proxy.pos < PROXY_V1_PACKET_LIMIT){
		अगर(net__read(context, &(context->proxy.buf[context->proxy.pos]), 1) != 1){
			proxy_cleanup(context);
			वापस MOSQ_ERR_CONN_LOST;
		}
		context->proxy.pos++;
		अगर(context->proxy.pos > 2){ /* FIXME: Figure out better limit */
			अगर(context->proxy.buf[context->proxy.pos-1] == '\n'
					&& context->proxy.buf[context->proxy.pos-2] == '\r'){

				/* Line received, now decode */
				वापस proxy_v1__decode(context);
			}
		}
	}
	अगर(context->proxy.pos == PROXY_V1_PACKET_LIMIT){
		log__printf(शून्य, MOSQ_LOG_NOTICE, "Connection rejected, corrupt PROXY header.");
		proxy_cleanup(context);
		वापस MOSQ_ERR_INVAL;
	}

	वापस MOSQ_ERR_SUCCESS;
}
#पूर्ण_अगर
