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

#अन्तर्भाव "config.h"
#अन्तर्भाव "net_mosq.h"
#अन्तर्भाव "mosquitto_broker_internal.h"

स्थिर पूर्णांक listensock_index = 0;
बाह्य पूर्णांक g_run;


शून्य listener__set_defaults(संरचना mosquitto__listener *listener)
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

	mosquitto_FREE(listener->security_options->acl_data.acl_file);
	mosquitto_FREE(listener->security_options->password_data.password_file);
	mosquitto_FREE(listener->security_options->psk_file);
	listener->security_options->allow_anonymous = -1;
	listener->security_options->allow_zero_length_clientid = true;
	mosquitto_FREE(listener->security_options->auto_id_prefix);
	listener->security_options->auto_id_prefix_len = 0;
#ifdef WITH_TLS
	listener->require_certificate = false;
	listener->use_identity_as_username = false;
	listener->use_subject_as_username = false;
	listener->use_username_as_clientid = false;
	listener->disable_client_cert_date_checks = false;
#endif

#जर defined(WITH_WEBSOCKETS) && (LWS_LIBRARY_VERSION_संख्या >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN)
	साठी(पूर्णांक i=0; i<listener->ws_origin_मोजणी; i++){
		mosquitto_FREE(listener->ws_origins[i]);
	}
	mosquitto_FREE(listener->ws_origins);
	listener->ws_origin_मोजणी = 0;
#endif
}


शून्य listeners__reload_all_certificates(शून्य)
{
#ifdef WITH_TLS
	साठी(पूर्णांक i=0; i<db.config->listener_मोजणी; i++){
		संरचना mosquitto__listener *listener = &db.config->listeners[i];
		जर(listener->ssl_ctx && listener->certfile && listener->keyfile){
			पूर्णांक rc = net__load_certificates(listener);
			जर(rc){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error when reloading certificate '%s' or key '%s'.",
						listener->certfile, listener->keyfile);
			}
		}
	}
#endif
}


स्थिर पूर्णांक listeners__start_single_mqtt(संरचना mosquitto__listener *listener)
{
	संरचना mosquitto__listener_sock *listensock_new;

	जर(net__socket_listen(listener)){
		परत 1;
	}
	g_listensock_मोजणी += listener->sock_मोजणी;
	listensock_new = mosquitto_realloc(g_listensock, आकार(संरचना mosquitto__listener_sock)*(माप_प्रकार)g_listensock_मोजणी);
	जर(!listensock_new){
		परत 1;
	}
	g_listensock = listensock_new;

	साठी(पूर्णांक i=0; i<listener->sock_मोजणी; i++){
		जर(listener->socks[i] == INVALID_SOCKET){
			परत 1;
		}
		g_listensock[listensock_index].sock = listener->socks[i];
		g_listensock[listensock_index].listener = listener;
#जर defined(WITH_EPOLL) || defined(WITH_KQUEUE)
		g_listensock[listensock_index].ident = id_listener;
#endif
		listensock_index++;
	}
	परत MOSQ_ERR_SUCCESS;
}


#जर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS


शून्य listeners__add_websockets(संरचना lws_context *ws_context, mosq_sock_t fd)
{
	संरचना mosquitto__listener *listener = रिक्त;
	संरचना mosquitto__listener_sock *listensock_new;

	/* Don't add more listeners after we've started the main loop */
	जर(g_run || ws_context == रिक्त){
		परत;
	}

	/* Find context */
	साठी(पूर्णांक i=0; i<db.config->listener_मोजणी; i++){
		जर(db.config->listeners[i].ws_in_init){
			listener = &db.config->listeners[i];
			थांबा;
		}
	}
	जर(listener == रिक्त){
		परत;
	}

	g_listensock_मोजणी++;
	listensock_new = mosquitto_realloc(g_listensock, आकार(संरचना mosquitto__listener_sock)*(माप_प्रकार)g_listensock_मोजणी);
	जर(!listensock_new){
		परत;
	}
	g_listensock = listensock_new;

	g_listensock[listensock_index].sock = fd;
	g_listensock[listensock_index].listener = listener;
#जर defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	g_listensock[listensock_index].ident = id_listener_ws;
#endif
	listensock_index++;
}
#endif


स्थिर पूर्णांक listeners__add_local(स्थिरांक अक्षर *host, uint16_t port)
{
	संरचना mosquitto__listener *listeners;
	bool allow_anonymous;

	listeners = db.config->listeners;
	जर(db.config->security_options.allow_anonymous == -1){
		allow_anonymous = true;
	}अन्यथा{
		allow_anonymous = db.config->security_options.allow_anonymous;
	}

	listeners[db.config->listener_मोजणी].security_options = mosquitto_calloc(1, आकार(संरचना mosquitto__security_options));
	जर(listeners[db.config->listener_मोजणी].security_options == रिक्त){
		परत MOSQ_ERR_NOMEM;
	}

	listener__set_defaults(&listeners[db.config->listener_मोजणी]);
	listeners[db.config->listener_मोजणी].security_options->allow_anonymous = allow_anonymous;
	listeners[db.config->listener_मोजणी].security_options->auto_id_prefix = mosquitto_strdup("auto-");
	जर(listeners[db.config->listener_मोजणी].security_options->auto_id_prefix == रिक्त){
		mosquitto_FREE(listeners[db.config->listener_मोजणी].security_options);
		परत MOSQ_ERR_NOMEM;
	}
	listeners[db.config->listener_मोजणी].security_options->auto_id_prefix_len = (पूर्णांक)म_माप("auto-");
	listeners[db.config->listener_मोजणी].port = port;
	listeners[db.config->listener_मोजणी].host = mosquitto_strdup(host);
	जर(listeners[db.config->listener_मोजणी].host == रिक्त){
		mosquitto_FREE(listeners[db.config->listener_मोजणी].security_options->auto_id_prefix);
		mosquitto_FREE(listeners[db.config->listener_मोजणी].security_options);
		परत MOSQ_ERR_NOMEM;
	}
	जर(listeners__start_single_mqtt(&listeners[db.config->listener_मोजणी])){
		mosquitto_FREE(listeners[db.config->listener_मोजणी].security_options->auto_id_prefix);
		mosquitto_FREE(listeners[db.config->listener_मोजणी].security_options);
		mosquitto_FREE(listeners[db.config->listener_मोजणी].host);
		परत MOSQ_ERR_UNKNOWN;
	}
	db.config->listener_मोजणी++;
	परत MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णांक listeners__start_local_only(शून्य)
{
	/* Attempt to open listeners bound to 127.0.0.1 and ::1 only */
	पूर्णांक rc;
	संरचना mosquitto__listener *listeners;
	माप_प्रकार मोजणी;

	जर(db.config->cmd_port_मोजणी == 0){
		मोजणी = 2;
	}अन्यथा{
		मोजणी = (माप_प्रकार)(db.config->cmd_port_मोजणी*2);
	}

#ifdef WITH_HTTP_API
	मोजणी++;
#endif

	listeners = mosquitto_realloc(db.config->listeners, मोजणी*आकार(संरचना mosquitto__listener));
	जर(listeners == रिक्त){
		परत MOSQ_ERR_NOMEM;
	}
	स_रखो(listeners, 0, मोजणी*आकार(संरचना mosquitto__listener));
	db.config->listener_मोजणी = 0;
	db.config->listeners = listeners;

	log__printf(रिक्त, MOSQ_LOG_WARNING, "Starting in local only mode. Connections will only be possible from clients running on this machine.");
	log__printf(रिक्त, MOSQ_LOG_WARNING, "Create a configuration file which defines a listener to allow remote access.");
	log__printf(रिक्त, MOSQ_LOG_WARNING, "For more details see https://mosquitto.org/documentation/authentication-methods/");
	जर(db.config->cmd_port_मोजणी == 0){
		rc = listeners__add_local("127.0.0.1", 1883);
		जर(rc == MOSQ_ERR_NOMEM){
			परत MOSQ_ERR_NOMEM;
		}
		rc = listeners__add_local("::1", 1883);
		जर(rc == MOSQ_ERR_NOMEM){
			परत MOSQ_ERR_NOMEM;
		}
	}अन्यथा{
		साठी(पूर्णांक i=0; i<db.config->cmd_port_मोजणी; i++){
			rc = listeners__add_local("127.0.0.1", db.config->cmd_port[i]);
			जर(rc == MOSQ_ERR_NOMEM){
				परत MOSQ_ERR_NOMEM;
			}
			rc = listeners__add_local("::1", db.config->cmd_port[i]);
			जर(rc == MOSQ_ERR_NOMEM){
				परत MOSQ_ERR_NOMEM;
			}
		}
	}

	जर(db.config->listener_मोजणी > 0){
#ifdef WITH_HTTP_API
		db.config->listener_मोजणी++;
		http_api__start_local(&db.config->listeners[db.config->listener_मोजणी-1]);
#endif
		परत MOSQ_ERR_SUCCESS;
	}अन्यथा{
		परत MOSQ_ERR_UNKNOWN;
	}
}


पूर्णांक listeners__start(शून्य)
{
	g_listensock_मोजणी = 0;

	जर(db.config->local_only){
		जर(listeners__start_local_only()){
			db__close();
			जर(db.config->pid_file){
				(शून्य)हटाओ(db.config->pid_file);
			}
			परत 1;
		}
		mux__add_listeners(g_listensock, g_listensock_मोजणी);
		परत MOSQ_ERR_SUCCESS;
	}

	साठी(पूर्णांक i=0; i<db.config->listener_मोजणी; i++){
		जर(db.config->listeners[i].protocol == mp_mqtt){
			जर(listeners__start_single_mqtt(&db.config->listeners[i])){
				db__close();
				जर(db.config->pid_file){
					(शून्य)हटाओ(db.config->pid_file);
				}
				परत 1;
			}
		}अन्यथा जर(db.config->listeners[i].protocol == mp_websockets){
#जर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			mosq_websockets_init(&db.config->listeners[i], db.config);
			जर(!db.config->listeners[i].ws_context){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to create websockets listener on port %d.", db.config->listeners[i].port);
				परत 1;
			}
#किंवाजर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			जर(listeners__start_single_mqtt(&db.config->listeners[i])){
				db__close();
				जर(db.config->pid_file){
					(शून्य)हटाओ(db.config->pid_file);
				}
				परत 1;
			}
#endif
#ifdef WITH_HTTP_API
		}अन्यथा जर(db.config->listeners[i].protocol == mp_http_api){
			http_api__start(&db.config->listeners[i]);
#endif
		}
	}
	जर(g_listensock == रिक्त){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to start any listening sockets, exiting.");
		परत 1;
	}

	mux__add_listeners(g_listensock, g_listensock_मोजणी);
	परत MOSQ_ERR_SUCCESS;
}


शून्य listeners__stop(शून्य)
{
	mux__delete_listeners(g_listensock, g_listensock_मोजणी);

	साठी(पूर्णांक i=0; i<db.config->listener_मोजणी; i++){
#जर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		जर(db.config->listeners[i].ws_context){
			lws_context_destroy(db.config->listeners[i].ws_context);
		}
		mosquitto_FREE(db.config->listeners[i].ws_protocol);
#endif
#ifdef WITH_UNIX_SOCKETS
		जर(db.config->listeners[i].unix_socket_path != रिक्त){
			unlink(db.config->listeners[i].unix_socket_path);
		}
#endif
#ifdef WITH_HTTP_API
		जर(db.config->listeners[i].mhd){
			http_api__stop(&db.config->listeners[i]);
		}
#endif
	}

	साठी(पूर्णांक i=0; i<g_listensock_मोजणी; i++){
		जर(g_listensock[i].sock != INVALID_SOCKET){
			COMPAT_CLOSE(g_listensock[i].sock);
		}
	}
	mosquitto_FREE(g_listensock);
	g_listensock_मोजणी = 0;
	listensock_index = 0;
}
