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

#ଅନ୍ତର୍ଭାଵ "config.h"
#ଅନ୍ତର୍ଭାଵ "net_mosq.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା listensock_index = 0;
ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା g_run;


ଶୂନ୍ୟ listener__set_defaults(ସଂରଚନା mosquitto__listener *listener)
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

#ଯଦି defined(WITH_WEBSOCKETS) && (LWS_LIBRARY_VERSION_ସଂଖ୍ୟା >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN)
	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<listener->ws_origin_ଗଣନା; i++){
		mosquitto_FREE(listener->ws_origins[i]);
	}
	mosquitto_FREE(listener->ws_origins);
	listener->ws_origin_ଗଣନା = 0;
#endif
}


ଶୂନ୍ୟ listeners__reload_all_certificates(ଶୂନ୍ୟ)
{
#ifdef WITH_TLS
	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<db.config->listener_ଗଣନା; i++){
		ସଂରଚନା mosquitto__listener *listener = &db.config->listeners[i];
		ଯଦି(listener->ssl_ctx && listener->certfile && listener->keyfile){
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = net__load_certificates(listener);
			ଯଦି(rc){
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error when reloading certificate '%s' or key '%s'.",
						listener->certfile, listener->keyfile);
			}
		}
	}
#endif
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା listeners__start_single_mqtt(ସଂରଚନା mosquitto__listener *listener)
{
	ସଂରଚନା mosquitto__listener_sock *listensock_new;

	ଯଦି(net__socket_listen(listener)){
		ଫେରନ୍ତୁ 1;
	}
	g_listensock_ଗଣନା += listener->sock_ଗଣନା;
	listensock_new = mosquitto_realloc(g_listensock, ଆକାର(ସଂରଚନା mosquitto__listener_sock)*(size_t)g_listensock_ଗଣନା);
	ଯଦି(!listensock_new){
		ଫେରନ୍ତୁ 1;
	}
	g_listensock = listensock_new;

	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<listener->sock_ଗଣନା; i++){
		ଯଦି(listener->socks[i] == INVALID_SOCKET){
			ଫେରନ୍ତୁ 1;
		}
		g_listensock[listensock_index].sock = listener->socks[i];
		g_listensock[listensock_index].listener = listener;
#ଯଦି defined(WITH_EPOLL) || defined(WITH_KQUEUE)
		g_listensock[listensock_index].ident = id_listener;
#endif
		listensock_index++;
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS


ଶୂନ୍ୟ listeners__add_websockets(ସଂରଚନା lws_context *ws_context, mosq_sock_t fd)
{
	ସଂରଚନା mosquitto__listener *listener = ଖାଲି;
	ସଂରଚନା mosquitto__listener_sock *listensock_new;

	/* Don't add more listeners after we've started the main loop */
	ଯଦି(g_run || ws_context == ଖାଲି){
		ଫେରନ୍ତୁ;
	}

	/* Find context */
	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<db.config->listener_ଗଣନା; i++){
		ଯଦି(db.config->listeners[i].ws_in_init){
			listener = &db.config->listeners[i];
			ଭାଙ୍ଗ;
		}
	}
	ଯଦି(listener == ଖାଲି){
		ଫେରନ୍ତୁ;
	}

	g_listensock_ଗଣନା++;
	listensock_new = mosquitto_realloc(g_listensock, ଆକାର(ସଂରଚନା mosquitto__listener_sock)*(size_t)g_listensock_ଗଣନା);
	ଯଦି(!listensock_new){
		ଫେରନ୍ତୁ;
	}
	g_listensock = listensock_new;

	g_listensock[listensock_index].sock = fd;
	g_listensock[listensock_index].listener = listener;
#ଯଦି defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	g_listensock[listensock_index].ident = id_listener_ws;
#endif
	listensock_index++;
}
#endif


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା listeners__add_local(ଅଚଳ ଅକ୍ଷର *host, uint16_t port)
{
	ସଂରଚନା mosquitto__listener *listeners;
	bool allow_anonymous;

	listeners = db.config->listeners;
	ଯଦି(db.config->security_options.allow_anonymous == -1){
		allow_anonymous = true;
	}ଅନ୍ୟଥା{
		allow_anonymous = db.config->security_options.allow_anonymous;
	}

	listeners[db.config->listener_ଗଣନା].security_options = mosquitto_calloc(1, ଆକାର(ସଂରଚନା mosquitto__security_options));
	ଯଦି(listeners[db.config->listener_ଗଣନା].security_options == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	listener__set_defaults(&listeners[db.config->listener_ଗଣନା]);
	listeners[db.config->listener_ଗଣନା].security_options->allow_anonymous = allow_anonymous;
	listeners[db.config->listener_ଗଣନା].security_options->auto_id_prefix = mosquitto_strdup("auto-");
	ଯଦି(listeners[db.config->listener_ଗଣନା].security_options->auto_id_prefix == ଖାଲି){
		mosquitto_FREE(listeners[db.config->listener_ଗଣନା].security_options);
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}
	listeners[db.config->listener_ଗଣନା].security_options->auto_id_prefix_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)strlen("auto-");
	listeners[db.config->listener_ଗଣନା].port = port;
	listeners[db.config->listener_ଗଣନା].host = mosquitto_strdup(host);
	ଯଦି(listeners[db.config->listener_ଗଣନା].host == ଖାଲି){
		mosquitto_FREE(listeners[db.config->listener_ଗଣନା].security_options->auto_id_prefix);
		mosquitto_FREE(listeners[db.config->listener_ଗଣନା].security_options);
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}
	ଯଦି(listeners__start_single_mqtt(&listeners[db.config->listener_ଗଣନା])){
		mosquitto_FREE(listeners[db.config->listener_ଗଣନା].security_options->auto_id_prefix);
		mosquitto_FREE(listeners[db.config->listener_ଗଣନା].security_options);
		mosquitto_FREE(listeners[db.config->listener_ଗଣନା].host);
		ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
	}
	db.config->listener_ଗଣନା++;
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା listeners__start_local_only(ଶୂନ୍ୟ)
{
	/* Attempt to open listeners bound to 127.0.0.1 and ::1 only */
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା mosquitto__listener *listeners;
	size_t ଗଣନା_2;

	ଯଦି(db.config->cmd_port_ଗଣନା == 0){
		ଗଣନା_2 = 2;
	}ଅନ୍ୟଥା{
		ଗଣନା_2 = (size_t)(db.config->cmd_port_ଗଣନା*2);
	}

#ifdef WITH_HTTP_API
	ଗଣନା_2++;
#endif

	listeners = mosquitto_realloc(db.config->listeners, ଗଣନା_2*ଆକାର(ସଂରଚନା mosquitto__listener));
	ଯଦି(listeners == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}
	memset(listeners, 0, ଗଣନା_2*ଆକାର(ସଂରଚନା mosquitto__listener));
	db.config->listener_ଗଣନା = 0;
	db.config->listeners = listeners;

	log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Starting in local only mode. Connections will only be possible from clients running on this machine.");
	log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Create a configuration file which defines a listener to allow remote access.");
	log__printf(ଖାଲି, MOSQ_LOG_WARNING, "For more details see https://mosquitto.org/documentation/authentication-methods/");
	ଯଦି(db.config->cmd_port_ଗଣନା == 0){
		rc = listeners__add_local("127.0.0.1", 1883);
		ଯଦି(rc == MOSQ_ERR_NOMEM){
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}
		rc = listeners__add_local("::1", 1883);
		ଯଦି(rc == MOSQ_ERR_NOMEM){
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}
	}ଅନ୍ୟଥା{
		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<db.config->cmd_port_ଗଣନା; i++){
			rc = listeners__add_local("127.0.0.1", db.config->cmd_port[i]);
			ଯଦି(rc == MOSQ_ERR_NOMEM){
				ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
			}
			rc = listeners__add_local("::1", db.config->cmd_port[i]);
			ଯଦି(rc == MOSQ_ERR_NOMEM){
				ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
			}
		}
	}

	ଯଦି(db.config->listener_ଗଣନା > 0){
#ifdef WITH_HTTP_API
		db.config->listener_ଗଣନା++;
		http_api__start_local(&db.config->listeners[db.config->listener_ଗଣନା-1]);
#endif
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
	}
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା listeners__start(ଶୂନ୍ୟ)
{
	g_listensock_ଗଣନା = 0;

	ଯଦି(db.config->local_only){
		ଯଦି(listeners__start_local_only()){
			db__close();
			ଯଦି(db.config->pid_file){
				(ଶୂନ୍ୟ)remove(db.config->pid_file);
			}
			ଫେରନ୍ତୁ 1;
		}
		mux__add_listeners(g_listensock, g_listensock_ଗଣନା);
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<db.config->listener_ଗଣନା; i++){
		ଯଦି(db.config->listeners[i].protocol == mp_mqtt){
			ଯଦି(listeners__start_single_mqtt(&db.config->listeners[i])){
				db__close();
				ଯଦି(db.config->pid_file){
					(ଶୂନ୍ୟ)remove(db.config->pid_file);
				}
				ଫେରନ୍ତୁ 1;
			}
		}ଅନ୍ୟଥା ଯଦି(db.config->listeners[i].protocol == mp_websockets){
#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			mosq_websockets_init(&db.config->listeners[i], db.config);
			ଯଦି(!db.config->listeners[i].ws_context){
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to create websockets listener on port %d.", db.config->listeners[i].port);
				ଫେରନ୍ତୁ 1;
			}
#ନଚେତଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			ଯଦି(listeners__start_single_mqtt(&db.config->listeners[i])){
				db__close();
				ଯଦି(db.config->pid_file){
					(ଶୂନ୍ୟ)remove(db.config->pid_file);
				}
				ଫେରନ୍ତୁ 1;
			}
#endif
#ifdef WITH_HTTP_API
		}ଅନ୍ୟଥା ଯଦି(db.config->listeners[i].protocol == mp_http_api){
			http_api__start(&db.config->listeners[i]);
#endif
		}
	}
	ଯଦି(g_listensock == ଖାଲି){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to start any listening sockets, exiting.");
		ଫେରନ୍ତୁ 1;
	}

	mux__add_listeners(g_listensock, g_listensock_ଗଣନା);
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ଶୂନ୍ୟ listeners__stop(ଶୂନ୍ୟ)
{
	mux__delete_listeners(g_listensock, g_listensock_ଗଣନା);

	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<db.config->listener_ଗଣନା; i++){
#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ଯଦି(db.config->listeners[i].ws_context){
			lws_context_destroy(db.config->listeners[i].ws_context);
		}
		mosquitto_FREE(db.config->listeners[i].ws_protocol);
#endif
#ifdef WITH_UNIX_SOCKETS
		ଯଦି(db.config->listeners[i].unix_socket_path != ଖାଲି){
			unlink(db.config->listeners[i].unix_socket_path);
		}
#endif
#ifdef WITH_HTTP_API
		ଯଦି(db.config->listeners[i].mhd){
			http_api__stop(&db.config->listeners[i]);
		}
#endif
	}

	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<g_listensock_ଗଣନା; i++){
		ଯଦି(g_listensock[i].sock != INVALID_SOCKET){
			COMPAT_CLOSE(g_listensock[i].sock);
		}
	}
	mosquitto_FREE(g_listensock);
	g_listensock_ଗଣନା = 0;
	listensock_index = 0;
}
