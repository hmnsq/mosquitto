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

स्थिर पूर्णाङ्क listensock_index = 0;
बाह्य पूर्णाङ्क g_run;


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

#जइ defined(WITH_WEBSOCKETS) && (LWS_LIBRARY_VERSION_सङ्ख्या >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN)
	प्रति(पूर्णाङ्क i=0; i<listener->ws_origin_गणनम्; i++){
		mosquitto_FREE(listener->ws_origins[i]);
	}
	mosquitto_FREE(listener->ws_origins);
	listener->ws_origin_गणनम् = 0;
#endif
}


शून्य listeners__reload_all_certificates(शून्य)
{
#ifdef WITH_TLS
	प्रति(पूर्णाङ्क i=0; i<db.config->listener_गणनम्; i++){
		संरचना mosquitto__listener *listener = &db.config->listeners[i];
		जइ(listener->ssl_ctx && listener->certfile && listener->keyfile){
			पूर्णाङ्क rc = net__load_certificates(listener);
			जइ(rc){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error when reloading certificate '%s' or key '%s'.",
						listener->certfile, listener->keyfile);
			}
		}
	}
#endif
}


स्थिर पूर्णाङ्क listeners__start_single_mqtt(संरचना mosquitto__listener *listener)
{
	संरचना mosquitto__listener_sock *listensock_new;

	जइ(net__socket_listen(listener)){
		प्रत्यागम 1;
	}
	g_listensock_गणनम् += listener->sock_गणनम्;
	listensock_new = mosquitto_realloc(g_listensock, परिमाण(संरचना mosquitto__listener_sock)*(size_t)g_listensock_गणनम्);
	जइ(!listensock_new){
		प्रत्यागम 1;
	}
	g_listensock = listensock_new;

	प्रति(पूर्णाङ्क i=0; i<listener->sock_गणनम्; i++){
		जइ(listener->socks[i] == INVALID_SOCKET){
			प्रत्यागम 1;
		}
		g_listensock[listensock_index].sock = listener->socks[i];
		g_listensock[listensock_index].listener = listener;
#जइ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
		g_listensock[listensock_index].ident = id_listener;
#endif
		listensock_index++;
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


#जइ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS


शून्य listeners__add_websockets(संरचना lws_context *ws_context, mosq_sock_t fd)
{
	संरचना mosquitto__listener *listener = रिक्त;
	संरचना mosquitto__listener_sock *listensock_new;

	/* Don't add more listeners after we've started the main loop */
	जइ(g_run || ws_context == रिक्त){
		प्रत्यागम;
	}

	/* Find context */
	प्रति(पूर्णाङ्क i=0; i<db.config->listener_गणनम्; i++){
		जइ(db.config->listeners[i].ws_in_init){
			listener = &db.config->listeners[i];
			भङ्ग;
		}
	}
	जइ(listener == रिक्त){
		प्रत्यागम;
	}

	g_listensock_गणनम्++;
	listensock_new = mosquitto_realloc(g_listensock, परिमाण(संरचना mosquitto__listener_sock)*(size_t)g_listensock_गणनम्);
	जइ(!listensock_new){
		प्रत्यागम;
	}
	g_listensock = listensock_new;

	g_listensock[listensock_index].sock = fd;
	g_listensock[listensock_index].listener = listener;
#जइ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	g_listensock[listensock_index].ident = id_listener_ws;
#endif
	listensock_index++;
}
#endif


स्थिर पूर्णाङ्क listeners__add_local(ध्रुव वर्ण *host, uint16_t port)
{
	संरचना mosquitto__listener *listeners;
	bool allow_anonymous;

	listeners = db.config->listeners;
	जइ(db.config->security_options.allow_anonymous == -1){
		allow_anonymous = true;
	}अण्णहा{
		allow_anonymous = db.config->security_options.allow_anonymous;
	}

	listeners[db.config->listener_गणनम्].security_options = mosquitto_calloc(1, परिमाण(संरचना mosquitto__security_options));
	जइ(listeners[db.config->listener_गणनम्].security_options == रिक्त){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	listener__set_defaults(&listeners[db.config->listener_गणनम्]);
	listeners[db.config->listener_गणनम्].security_options->allow_anonymous = allow_anonymous;
	listeners[db.config->listener_गणनम्].security_options->auto_id_prefix = mosquitto_strdup("auto-");
	जइ(listeners[db.config->listener_गणनम्].security_options->auto_id_prefix == रिक्त){
		mosquitto_FREE(listeners[db.config->listener_गणनम्].security_options);
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	listeners[db.config->listener_गणनम्].security_options->auto_id_prefix_len = (पूर्णाङ्क)strlen("auto-");
	listeners[db.config->listener_गणनम्].port = port;
	listeners[db.config->listener_गणनम्].host = mosquitto_strdup(host);
	जइ(listeners[db.config->listener_गणनम्].host == रिक्त){
		mosquitto_FREE(listeners[db.config->listener_गणनम्].security_options->auto_id_prefix);
		mosquitto_FREE(listeners[db.config->listener_गणनम्].security_options);
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	जइ(listeners__start_single_mqtt(&listeners[db.config->listener_गणनम्])){
		mosquitto_FREE(listeners[db.config->listener_गणनम्].security_options->auto_id_prefix);
		mosquitto_FREE(listeners[db.config->listener_गणनम्].security_options);
		mosquitto_FREE(listeners[db.config->listener_गणनम्].host);
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}
	db.config->listener_गणनम्++;
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क listeners__start_local_only(शून्य)
{
	/* Attempt to open listeners bound to 127.0.0.1 and ::1 only */
	पूर्णाङ्क rc;
	संरचना mosquitto__listener *listeners;
	size_t गणनम्;

	जइ(db.config->cmd_port_गणनम् == 0){
		गणनम् = 2;
	}अण्णहा{
		गणनम् = (size_t)(db.config->cmd_port_गणनम्*2);
	}

#ifdef WITH_HTTP_API
	गणनम्++;
#endif

	listeners = mosquitto_realloc(db.config->listeners, गणनम्*परिमाण(संरचना mosquitto__listener));
	जइ(listeners == रिक्त){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	memset(listeners, 0, गणनम्*परिमाण(संरचना mosquitto__listener));
	db.config->listener_गणनम् = 0;
	db.config->listeners = listeners;

	log__printf(रिक्त, MOSQ_LOG_WARNING, "Starting in local only mode. Connections will only be possible from clients running on this machine.");
	log__printf(रिक्त, MOSQ_LOG_WARNING, "Create a configuration file which defines a listener to allow remote access.");
	log__printf(रिक्त, MOSQ_LOG_WARNING, "For more details see https://mosquitto.org/documentation/authentication-methods/");
	जइ(db.config->cmd_port_गणनम् == 0){
		rc = listeners__add_local("127.0.0.1", 1883);
		जइ(rc == MOSQ_ERR_NOMEM){
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
		rc = listeners__add_local("::1", 1883);
		जइ(rc == MOSQ_ERR_NOMEM){
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
	}अण्णहा{
		प्रति(पूर्णाङ्क i=0; i<db.config->cmd_port_गणनम्; i++){
			rc = listeners__add_local("127.0.0.1", db.config->cmd_port[i]);
			जइ(rc == MOSQ_ERR_NOMEM){
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
			rc = listeners__add_local("::1", db.config->cmd_port[i]);
			जइ(rc == MOSQ_ERR_NOMEM){
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
		}
	}

	जइ(db.config->listener_गणनम् > 0){
#ifdef WITH_HTTP_API
		db.config->listener_गणनम्++;
		http_api__start_local(&db.config->listeners[db.config->listener_गणनम्-1]);
#endif
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}अण्णहा{
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}
}


पूर्णाङ्क listeners__start(शून्य)
{
	g_listensock_गणनम् = 0;

	जइ(db.config->local_only){
		जइ(listeners__start_local_only()){
			db__close();
			जइ(db.config->pid_file){
				(शून्य)remove(db.config->pid_file);
			}
			प्रत्यागम 1;
		}
		mux__add_listeners(g_listensock, g_listensock_गणनम्);
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	प्रति(पूर्णाङ्क i=0; i<db.config->listener_गणनम्; i++){
		जइ(db.config->listeners[i].protocol == mp_mqtt){
			जइ(listeners__start_single_mqtt(&db.config->listeners[i])){
				db__close();
				जइ(db.config->pid_file){
					(शून्य)remove(db.config->pid_file);
				}
				प्रत्यागम 1;
			}
		}अण्णहा जइ(db.config->listeners[i].protocol == mp_websockets){
#जइ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			mosq_websockets_init(&db.config->listeners[i], db.config);
			जइ(!db.config->listeners[i].ws_context){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to create websockets listener on port %d.", db.config->listeners[i].port);
				प्रत्यागम 1;
			}
#नोचेद्यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			जइ(listeners__start_single_mqtt(&db.config->listeners[i])){
				db__close();
				जइ(db.config->pid_file){
					(शून्य)remove(db.config->pid_file);
				}
				प्रत्यागम 1;
			}
#endif
#ifdef WITH_HTTP_API
		}अण्णहा जइ(db.config->listeners[i].protocol == mp_http_api){
			http_api__start(&db.config->listeners[i]);
#endif
		}
	}
	जइ(g_listensock == रिक्त){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to start any listening sockets, exiting.");
		प्रत्यागम 1;
	}

	mux__add_listeners(g_listensock, g_listensock_गणनम्);
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


शून्य listeners__stop(शून्य)
{
	mux__delete_listeners(g_listensock, g_listensock_गणनम्);

	प्रति(पूर्णाङ्क i=0; i<db.config->listener_गणनम्; i++){
#जइ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		जइ(db.config->listeners[i].ws_context){
			lws_context_destroy(db.config->listeners[i].ws_context);
		}
		mosquitto_FREE(db.config->listeners[i].ws_protocol);
#endif
#ifdef WITH_UNIX_SOCKETS
		जइ(db.config->listeners[i].unix_socket_path != रिक्त){
			unlink(db.config->listeners[i].unix_socket_path);
		}
#endif
#ifdef WITH_HTTP_API
		जइ(db.config->listeners[i].mhd){
			http_api__stop(&db.config->listeners[i]);
		}
#endif
	}

	प्रति(पूर्णाङ्क i=0; i<g_listensock_गणनम्; i++){
		जइ(g_listensock[i].sock != INVALID_SOCKET){
			COMPAT_CLOSE(g_listensock[i].sock);
		}
	}
	mosquitto_FREE(g_listensock);
	g_listensock_गणनम् = 0;
	listensock_index = 0;
}
