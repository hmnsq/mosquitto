/*
Copyright (c) 2009-2021 Roger Light <roger@atchoo.org>

All rights reserved. This program and the accompanying materials
are made available under the terms of the Eclipse Public License 2.0
and Eclipse Distribution License v1.0 which accompany this distribution.

The Eclipse Public License is available at
   https://www.eclipse.org/legal/epl-2.0/
and the Eclipse Distribution License is available at
  http://www.eclipse.org/org/documents/edl-v10.php.

SPDX-License-Identifier: EPL-2.0 OR BSD-3-Clause

Contributors:
   Roger Light - initial implementation and documentation.
*/

#समावेश "config.h"
#समावेश "net_mosq.h"
#समावेश "mosquitto_broker_internal.h"

अटल पूर्णांक listensock_सूचकांक = 0;
बाह्य पूर्णांक g_run;


व्योम listener__set_defaults(काष्ठा mosquitto__listener *listener)
{
	listener->disable_protocol_v3 = false;
	listener->disable_protocol_v4 = false;
	listener->disable_protocol_v5 = false;
	listener->max_connections = -1;
	listener->max_qos = 2;
	listener->max_topic_alias = 10;
	listener->max_topic_alias_broker = 10;
	listener->protocol = mp_mqtt;
	mosquitto_FREE(listener->mount_point);

	mosquitto_FREE(listener->security_options->acl_तथ्य.acl_file);
	mosquitto_FREE(listener->security_options->password_तथ्य.password_file);
	mosquitto_FREE(listener->security_options->psk_file);
	listener->security_options->allow_anonymous = -1;
	listener->security_options->allow_zero_लंबाई_clientid = true;
	mosquitto_FREE(listener->security_options->auto_id_prefix);
	listener->security_options->auto_id_prefix_len = 0;
#अगर_घोषित WITH_TLS
	listener->require_certificate = false;
	listener->use_identity_as_username = false;
	listener->use_subject_as_username = false;
	listener->use_username_as_clientid = false;
	listener->disable_client_cert_date_checks = false;
#पूर्ण_अगर

#अगर defined(WITH_WEBSOCKETS) && (LWS_LIBRARY_VERSION_संख्या >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN)
	क्रम(पूर्णांक i=0; i<listener->ws_origin_गिनती; i++){
		mosquitto_FREE(listener->ws_origins[i]);
	}
	mosquitto_FREE(listener->ws_origins);
	listener->ws_origin_गिनती = 0;
#पूर्ण_अगर
}


व्योम listeners__reload_all_certificates(व्योम)
{
#अगर_घोषित WITH_TLS
	क्रम(पूर्णांक i=0; i<db.config->listener_गिनती; i++){
		काष्ठा mosquitto__listener *listener = &db.config->listeners[i];
		अगर(listener->ssl_ctx && listener->certfile && listener->keyfile){
			पूर्णांक rc = net__load_certificates(listener);
			अगर(rc){
				log__printf(शून्य, MOSQ_LOG_ERR, "Error when reloading certificate '%s' or key '%s'.",
						listener->certfile, listener->keyfile);
			}
		}
	}
#पूर्ण_अगर
}


अटल पूर्णांक listeners__start_single_mqtt(काष्ठा mosquitto__listener *listener)
{
	काष्ठा mosquitto__listener_sock *listensock_new;

	अगर(net__socket_listen(listener)){
		वापस 1;
	}
	g_listensock_गिनती += listener->sock_गिनती;
	listensock_new = mosquitto_realloc(g_listensock, माप(काष्ठा mosquitto__listener_sock)*(माप_प्रकार)g_listensock_गिनती);
	अगर(!listensock_new){
		वापस 1;
	}
	g_listensock = listensock_new;

	क्रम(पूर्णांक i=0; i<listener->sock_गिनती; i++){
		अगर(listener->socks[i] == INVALID_SOCKET){
			वापस 1;
		}
		g_listensock[listensock_सूचकांक].sock = listener->socks[i];
		g_listensock[listensock_सूचकांक].listener = listener;
#अगर defined(WITH_EPOLL) || defined(WITH_KQUEUE)
		g_listensock[listensock_सूचकांक].ident = id_listener;
#पूर्ण_अगर
		listensock_सूचकांक++;
	}
	वापस MOSQ_ERR_SUCCESS;
}


#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS


व्योम listeners__add_websockets(काष्ठा lws_context *ws_context, mosq_sock_t fd)
{
	काष्ठा mosquitto__listener *listener = शून्य;
	काष्ठा mosquitto__listener_sock *listensock_new;

	/* Don't add more listeners after we've started the main loop */
	अगर(g_run || ws_context == शून्य){
		वापस;
	}

	/* Find context */
	क्रम(पूर्णांक i=0; i<db.config->listener_गिनती; i++){
		अगर(db.config->listeners[i].ws_in_init){
			listener = &db.config->listeners[i];
			अवरोध;
		}
	}
	अगर(listener == शून्य){
		वापस;
	}

	g_listensock_गिनती++;
	listensock_new = mosquitto_realloc(g_listensock, माप(काष्ठा mosquitto__listener_sock)*(माप_प्रकार)g_listensock_गिनती);
	अगर(!listensock_new){
		वापस;
	}
	g_listensock = listensock_new;

	g_listensock[listensock_सूचकांक].sock = fd;
	g_listensock[listensock_सूचकांक].listener = listener;
#अगर defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	g_listensock[listensock_सूचकांक].ident = id_listener_ws;
#पूर्ण_अगर
	listensock_सूचकांक++;
}
#पूर्ण_अगर


अटल पूर्णांक listeners__add_local(स्थिर अक्षर *host, uint16_t port)
{
	काष्ठा mosquitto__listener *listeners;
	bool allow_anonymous;

	listeners = db.config->listeners;
	अगर(db.config->security_options.allow_anonymous == -1){
		allow_anonymous = true;
	}अन्यथा{
		allow_anonymous = db.config->security_options.allow_anonymous;
	}

	listeners[db.config->listener_गिनती].security_options = mosquitto_calloc(1, माप(काष्ठा mosquitto__security_options));
	अगर(listeners[db.config->listener_गिनती].security_options == शून्य){
		वापस MOSQ_ERR_NOMEM;
	}

	listener__set_defaults(&listeners[db.config->listener_गिनती]);
	listeners[db.config->listener_गिनती].security_options->allow_anonymous = allow_anonymous;
	listeners[db.config->listener_गिनती].security_options->auto_id_prefix = mosquitto_strdup("auto-");
	अगर(listeners[db.config->listener_गिनती].security_options->auto_id_prefix == शून्य){
		mosquitto_FREE(listeners[db.config->listener_गिनती].security_options);
		वापस MOSQ_ERR_NOMEM;
	}
	listeners[db.config->listener_गिनती].security_options->auto_id_prefix_len = (पूर्णांक)म_माप("auto-");
	listeners[db.config->listener_गिनती].port = port;
	listeners[db.config->listener_गिनती].host = mosquitto_strdup(host);
	अगर(listeners[db.config->listener_गिनती].host == शून्य){
		mosquitto_FREE(listeners[db.config->listener_गिनती].security_options->auto_id_prefix);
		mosquitto_FREE(listeners[db.config->listener_गिनती].security_options);
		वापस MOSQ_ERR_NOMEM;
	}
	अगर(listeners__start_single_mqtt(&listeners[db.config->listener_गिनती])){
		mosquitto_FREE(listeners[db.config->listener_गिनती].security_options->auto_id_prefix);
		mosquitto_FREE(listeners[db.config->listener_गिनती].security_options);
		mosquitto_FREE(listeners[db.config->listener_गिनती].host);
		वापस MOSQ_ERR_UNKNOWN;
	}
	db.config->listener_गिनती++;
	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक listeners__start_local_only(व्योम)
{
	/* Attempt to open listeners bound to 127.0.0.1 and ::1 only */
	पूर्णांक rc;
	काष्ठा mosquitto__listener *listeners;
	माप_प्रकार गिनती;

	अगर(db.config->cmd_port_गिनती == 0){
		गिनती = 2;
	}अन्यथा{
		गिनती = (माप_प्रकार)(db.config->cmd_port_गिनती*2);
	}

#अगर_घोषित WITH_HTTP_API
	गिनती++;
#पूर्ण_अगर

	listeners = mosquitto_realloc(db.config->listeners, गिनती*माप(काष्ठा mosquitto__listener));
	अगर(listeners == शून्य){
		वापस MOSQ_ERR_NOMEM;
	}
	स_रखो(listeners, 0, गिनती*माप(काष्ठा mosquitto__listener));
	db.config->listener_गिनती = 0;
	db.config->listeners = listeners;

	log__printf(शून्य, MOSQ_LOG_WARNING, "Starting in local only mode. Connections will only be possible from clients running on this machine.");
	log__printf(शून्य, MOSQ_LOG_WARNING, "Create a configuration file which defines a listener to allow remote access.");
	log__printf(शून्य, MOSQ_LOG_WARNING, "For more details see https://mosquitto.org/documentation/authentication-methods/");
	अगर(db.config->cmd_port_गिनती == 0){
		rc = listeners__add_local("127.0.0.1", 1883);
		अगर(rc == MOSQ_ERR_NOMEM){
			वापस MOSQ_ERR_NOMEM;
		}
		rc = listeners__add_local("::1", 1883);
		अगर(rc == MOSQ_ERR_NOMEM){
			वापस MOSQ_ERR_NOMEM;
		}
	}अन्यथा{
		क्रम(पूर्णांक i=0; i<db.config->cmd_port_गिनती; i++){
			rc = listeners__add_local("127.0.0.1", db.config->cmd_port[i]);
			अगर(rc == MOSQ_ERR_NOMEM){
				वापस MOSQ_ERR_NOMEM;
			}
			rc = listeners__add_local("::1", db.config->cmd_port[i]);
			अगर(rc == MOSQ_ERR_NOMEM){
				वापस MOSQ_ERR_NOMEM;
			}
		}
	}

	अगर(db.config->listener_गिनती > 0){
#अगर_घोषित WITH_HTTP_API
		db.config->listener_गिनती++;
		http_api__start_local(&db.config->listeners[db.config->listener_गिनती-1]);
#पूर्ण_अगर
		वापस MOSQ_ERR_SUCCESS;
	}अन्यथा{
		वापस MOSQ_ERR_UNKNOWN;
	}
}


पूर्णांक listeners__start(व्योम)
{
	g_listensock_गिनती = 0;

	अगर(db.config->local_only){
		अगर(listeners__start_local_only()){
			db__close();
			अगर(db.config->pid_file){
				(व्योम)हटाओ(db.config->pid_file);
			}
			वापस 1;
		}
		mux__add_listeners(g_listensock, g_listensock_गिनती);
		वापस MOSQ_ERR_SUCCESS;
	}

	क्रम(पूर्णांक i=0; i<db.config->listener_गिनती; i++){
		अगर(db.config->listeners[i].protocol == mp_mqtt){
			अगर(listeners__start_single_mqtt(&db.config->listeners[i])){
				db__close();
				अगर(db.config->pid_file){
					(व्योम)हटाओ(db.config->pid_file);
				}
				वापस 1;
			}
		}अन्यथा अगर(db.config->listeners[i].protocol == mp_websockets){
#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			mosq_websockets_init(&db.config->listeners[i], db.config);
			अगर(!db.config->listeners[i].ws_context){
				log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to create websockets listener on port %d.", db.config->listeners[i].port);
				वापस 1;
			}
#या_अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			अगर(listeners__start_single_mqtt(&db.config->listeners[i])){
				db__close();
				अगर(db.config->pid_file){
					(व्योम)हटाओ(db.config->pid_file);
				}
				वापस 1;
			}
#पूर्ण_अगर
#अगर_घोषित WITH_HTTP_API
		}अन्यथा अगर(db.config->listeners[i].protocol == mp_http_api){
			http_api__start(&db.config->listeners[i]);
#पूर्ण_अगर
		}
	}
	अगर(g_listensock == शून्य){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to start any listening sockets, exiting.");
		वापस 1;
	}

	mux__add_listeners(g_listensock, g_listensock_गिनती);
	वापस MOSQ_ERR_SUCCESS;
}


व्योम listeners__stop(व्योम)
{
	mux__delete_listeners(g_listensock, g_listensock_गिनती);

	क्रम(पूर्णांक i=0; i<db.config->listener_गिनती; i++){
#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		अगर(db.config->listeners[i].ws_context){
			lws_context_destroy(db.config->listeners[i].ws_context);
		}
		mosquitto_FREE(db.config->listeners[i].ws_protocol);
#पूर्ण_अगर
#अगर_घोषित WITH_UNIX_SOCKETS
		अगर(db.config->listeners[i].unix_socket_path != शून्य){
			unlink(db.config->listeners[i].unix_socket_path);
		}
#पूर्ण_अगर
#अगर_घोषित WITH_HTTP_API
		अगर(db.config->listeners[i].mhd){
			http_api__stop(&db.config->listeners[i]);
		}
#पूर्ण_अगर
	}

	क्रम(पूर्णांक i=0; i<g_listensock_गिनती; i++){
		अगर(g_listensock[i].sock != INVALID_SOCKET){
			COMPAT_CLOSE(g_listensock[i].sock);
		}
	}
	mosquitto_FREE(g_listensock);
	g_listensock_गिनती = 0;
	listensock_सूचकांक = 0;
}
