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

स्थिर पूर्णाङ्क listensock_सूचकांक = 0;
बाह्य पूर्णाङ्क g_सततम्;


शून्य listener_समुच्चयः_defaults(संरचना mosquitto__listener *listener)
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

	mosquitto_FREE(listener->security_options->acl_दत्त.acl_file);
	mosquitto_FREE(listener->security_options->password_दत्त.password_file);
	mosquitto_FREE(listener->security_options->psk_file);
	listener->security_options->allow_anonymous = -1;
	listener->security_options->allow_zero_दीर्घता_clientid = true;
	mosquitto_FREE(listener->security_options->auto_id_उपसर्गः);
	listener->security_options->auto_id_उपसर्गः_len = 0;
#यदि_परिभाषितम् WITH_TLS
	listener->require_certificate = false;
	listener->use_तटस्थः_as_username = false;
	listener->use_subject_as_username = false;
	listener->use_username_as_clientid = false;
	listener->disable_client_cert_date_checks = false;
#यदि_अन्तः

#यदि defined(WITH_WEBSOCKETS) && (LWS_LIBRARY_संस्करणम्_सङ्ख्या >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN)
	प्रति(पूर्णाङ्क i=0; i<listener->ws_origin_गणनम्; i++){
		mosquitto_FREE(listener->ws_origins[i]);
	}
	mosquitto_FREE(listener->ws_origins);
	listener->ws_origin_गणनम् = 0;
#यदि_अन्तः
}


शून्य listeners__reload_all_certificates(शून्य)
{
#यदि_परिभाषितम् WITH_TLS
	प्रति(पूर्णाङ्क i=0; i<db.config->listener_गणनम्; i++){
		संरचना mosquitto__listener *listener = &db.config->listeners[i];
		यदि(listener->ssl_ctx && listener->certfile && listener->keyfile){
			पूर्णाङ्क rc = net_आरोपय_certificates(listener);
			यदि(rc){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error when reloading certificate '%s' or key '%s'.",
						listener->certfile, listener->keyfile);
			}
		}
	}
#यदि_अन्तः
}


स्थिर पूर्णाङ्क listeners__start_single_mqtt(संरचना mosquitto__listener *listener)
{
	संरचना mosquitto__listener_sock *listensock_new;

	यदि(net__socket_listen(listener)){
		प्रत्यागम 1;
	}
	g_listensock_गणनम् += listener->sock_गणनम्;
	listensock_new = mosquitto_realloc(g_listensock, परिमाण(संरचना mosquitto__listener_sock)*(size_t)g_listensock_गणनम्);
	यदि(!listensock_new){
		प्रत्यागम 1;
	}
	g_listensock = listensock_new;

	प्रति(पूर्णाङ्क i=0; i<listener->sock_गणनम्; i++){
		यदि(listener->socks[i] == INVALID_SOCKET){
			प्रत्यागम 1;
		}
		g_listensock[listensock_सूचकांक].sock = listener->socks[i];
		g_listensock[listensock_सूचकांक].listener = listener;
#यदि defined(WITH_EPOLL) || defined(WITH_KQUEUE)
		g_listensock[listensock_सूचकांक].ident = id_listener;
#यदि_अन्तः
		listensock_सूचकांक++;
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS


शून्य listeners_सङ्कलय_websockets(संरचना lws_context *ws_context, mosq_sock_t fd)
{
	संरचना mosquitto__listener *listener = रिक्त;
	संरचना mosquitto__listener_sock *listensock_new;

	/* Don't add more listeners after we've started the main loop */
	यदि(g_सततम् || ws_context == रिक्त){
		प्रत्यागम;
	}

	/* Find context */
	प्रति(पूर्णाङ्क i=0; i<db.config->listener_गणनम्; i++){
		यदि(db.config->listeners[i].ws_अन्तर्_init){
			listener = &db.config->listeners[i];
			भङ्ग;
		}
	}
	यदि(listener == रिक्त){
		प्रत्यागम;
	}

	g_listensock_गणनम्++;
	listensock_new = mosquitto_realloc(g_listensock, परिमाण(संरचना mosquitto__listener_sock)*(size_t)g_listensock_गणनम्);
	यदि(!listensock_new){
		प्रत्यागम;
	}
	g_listensock = listensock_new;

	g_listensock[listensock_सूचकांक].sock = fd;
	g_listensock[listensock_सूचकांक].listener = listener;
#यदि defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	g_listensock[listensock_सूचकांक].ident = id_listener_ws;
#यदि_अन्तः
	listensock_सूचकांक++;
}
#यदि_अन्तः


स्थिर पूर्णाङ्क listeners_सङ्कलय_local(ध्रुव वर्ण *host, uint16_t पत्तनम्)
{
	संरचना mosquitto__listener *listeners;
	bool allow_anonymous;

	listeners = db.config->listeners;
	यदि(db.config->security_options.allow_anonymous == -1){
		allow_anonymous = true;
	}अन्यथा{
		allow_anonymous = db.config->security_options.allow_anonymous;
	}

	listeners[db.config->listener_गणनम्].security_options = mosquitto_calloc(1, परिमाण(संरचना mosquitto__security_options));
	यदि(listeners[db.config->listener_गणनम्].security_options == रिक्त){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	listener_समुच्चयः_defaults(&listeners[db.config->listener_गणनम्]);
	listeners[db.config->listener_गणनम्].security_options->allow_anonymous = allow_anonymous;
	listeners[db.config->listener_गणनम्].security_options->auto_id_उपसर्गः = mosquitto_strdup("auto-");
	यदि(listeners[db.config->listener_गणनम्].security_options->auto_id_उपसर्गः == रिक्त){
		mosquitto_FREE(listeners[db.config->listener_गणनम्].security_options);
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	listeners[db.config->listener_गणनम्].security_options->auto_id_उपसर्गः_len = (पूर्णाङ्क)वाक्यदैर्घ्यम्("auto-");
	listeners[db.config->listener_गणनम्].पत्तनम् = पत्तनम्;
	listeners[db.config->listener_गणनम्].host = mosquitto_strdup(host);
	यदि(listeners[db.config->listener_गणनम्].host == रिक्त){
		mosquitto_FREE(listeners[db.config->listener_गणनम्].security_options->auto_id_उपसर्गः);
		mosquitto_FREE(listeners[db.config->listener_गणनम्].security_options);
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	यदि(listeners__start_single_mqtt(&listeners[db.config->listener_गणनम्])){
		mosquitto_FREE(listeners[db.config->listener_गणनम्].security_options->auto_id_उपसर्गः);
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

	यदि(db.config->cmd_पत्तनम्_गणनम् == 0){
		गणनम् = 2;
	}अन्यथा{
		गणनम् = (size_t)(db.config->cmd_पत्तनम्_गणनम्*2);
	}

#यदि_परिभाषितम् WITH_HTTP_API
	गणनम्++;
#यदि_अन्तः

	listeners = mosquitto_realloc(db.config->listeners, गणनम्*परिमाण(संरचना mosquitto__listener));
	यदि(listeners == रिक्त){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	स्मृतिं_पूरय(listeners, 0, गणनम्*परिमाण(संरचना mosquitto__listener));
	db.config->listener_गणनम् = 0;
	db.config->listeners = listeners;

	log__printf(रिक्त, MOSQ_LOG_WARNING, "Starting in local only mode. Connections will only be possible from clients running on this machine.");
	log__printf(रिक्त, MOSQ_LOG_WARNING, "Create a configuration file which defines a listener to allow remote access.");
	log__printf(रिक्त, MOSQ_LOG_WARNING, "For more details see https://mosquitto.org/documentation/authentication-methods/");
	यदि(db.config->cmd_पत्तनम्_गणनम् == 0){
		rc = listeners_सङ्कलय_local("127.0.0.1", 1883);
		यदि(rc == MOSQ_ERR_NOMEM){
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
		rc = listeners_सङ्कलय_local("::1", 1883);
		यदि(rc == MOSQ_ERR_NOMEM){
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
	}अन्यथा{
		प्रति(पूर्णाङ्क i=0; i<db.config->cmd_पत्तनम्_गणनम्; i++){
			rc = listeners_सङ्कलय_local("127.0.0.1", db.config->cmd_पत्तनम्[i]);
			यदि(rc == MOSQ_ERR_NOMEM){
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
			rc = listeners_सङ्कलय_local("::1", db.config->cmd_पत्तनम्[i]);
			यदि(rc == MOSQ_ERR_NOMEM){
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
		}
	}

	यदि(db.config->listener_गणनम् > 0){
#यदि_परिभाषितम् WITH_HTTP_API
		db.config->listener_गणनम्++;
		http_api__start_local(&db.config->listeners[db.config->listener_गणनम्-1]);
#यदि_अन्तः
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}अन्यथा{
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}
}


पूर्णाङ्क listeners__start(शून्य)
{
	g_listensock_गणनम् = 0;

	यदि(db.config->local_only){
		यदि(listeners__start_local_only()){
			db__close();
			यदि(db.config->pid_file){
				(शून्य)remove(db.config->pid_file);
			}
			प्रत्यागम 1;
		}
		mux_सङ्कलय_listeners(g_listensock, g_listensock_गणनम्);
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	प्रति(पूर्णाङ्क i=0; i<db.config->listener_गणनम्; i++){
		यदि(db.config->listeners[i].protocol == mp_mqtt){
			यदि(listeners__start_single_mqtt(&db.config->listeners[i])){
				db__close();
				यदि(db.config->pid_file){
					(शून्य)remove(db.config->pid_file);
				}
				प्रत्यागम 1;
			}
		}अन्यथा यदि(db.config->listeners[i].protocol == mp_websockets){
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			mosq_websockets_init(&db.config->listeners[i], db.config);
			यदि(!db.config->listeners[i].ws_context){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to create websockets listener on port %d.", db.config->listeners[i].पत्तनम्);
				प्रत्यागम 1;
			}
#नोचेद्यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			यदि(listeners__start_single_mqtt(&db.config->listeners[i])){
				db__close();
				यदि(db.config->pid_file){
					(शून्य)remove(db.config->pid_file);
				}
				प्रत्यागम 1;
			}
#यदि_अन्तः
#यदि_परिभाषितम् WITH_HTTP_API
		}अन्यथा यदि(db.config->listeners[i].protocol == mp_http_api){
			http_api__start(&db.config->listeners[i]);
#यदि_अन्तः
		}
	}
	यदि(g_listensock == रिक्त){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to start any listening sockets, exiting.");
		प्रत्यागम 1;
	}

	mux_सङ्कलय_listeners(g_listensock, g_listensock_गणनम्);
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


शून्य listeners__stop(शून्य)
{
	mux__delete_listeners(g_listensock, g_listensock_गणनम्);

	प्रति(पूर्णाङ्क i=0; i<db.config->listener_गणनम्; i++){
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		यदि(db.config->listeners[i].ws_context){
			lws_context_destroy(db.config->listeners[i].ws_context);
		}
		mosquitto_FREE(db.config->listeners[i].ws_protocol);
#यदि_अन्तः
#यदि_परिभाषितम् WITH_UNIX_SOCKETS
		यदि(db.config->listeners[i].unix_socket_मार्गः != रिक्त){
			unlink(db.config->listeners[i].unix_socket_मार्गः);
		}
#यदि_अन्तः
#यदि_परिभाषितम् WITH_HTTP_API
		यदि(db.config->listeners[i].mhd){
			http_api__stop(&db.config->listeners[i]);
		}
#यदि_अन्तः
	}

	प्रति(पूर्णाङ्क i=0; i<g_listensock_गणनम्; i++){
		यदि(g_listensock[i].sock != INVALID_SOCKET){
			COMPAT_CLOSE(g_listensock[i].sock);
		}
	}
	mosquitto_FREE(g_listensock);
	g_listensock_गणनम् = 0;
	listensock_सूचकांक = 0;
}
