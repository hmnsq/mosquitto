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

#ಅನ್ತರ್ಭಾವ "config.h"
#ಅನ್ತರ್ಭಾವ "net_mosq.h"
#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ listensock_index = 0;
ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ g_run;


ಶೂನ್ಯ listener__set_defaults(ಸಂರಚನಾ mosquitto__listener *listener)
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

#ಯದಿ defined(WITH_WEBSOCKETS) && (LWS_LIBRARY_VERSION_ಸಂಖ್ಯೆ >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN)
	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<listener->ws_origin_ಎಣಿಕೆ; i++){
		mosquitto_FREE(listener->ws_origins[i]);
	}
	mosquitto_FREE(listener->ws_origins);
	listener->ws_origin_ಎಣಿಕೆ = 0;
#endif
}


ಶೂನ್ಯ listeners__reload_all_certificates(ಶೂನ್ಯ)
{
#ifdef WITH_TLS
	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
		ಸಂರಚನಾ mosquitto__listener *listener = &db.config->listeners[i];
		ಯದಿ(listener->ssl_ctx && listener->certfile && listener->keyfile){
			ಪೂರ್ಣಾಙ್ಕ rc = net__load_certificates(listener);
			ಯದಿ(rc){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error when reloading certificate '%s' or key '%s'.",
						listener->certfile, listener->keyfile);
			}
		}
	}
#endif
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ listeners__start_single_mqtt(ಸಂರಚನಾ mosquitto__listener *listener)
{
	ಸಂರಚನಾ mosquitto__listener_sock *listensock_new;

	ಯದಿ(net__socket_listen(listener)){
		ಪ್ರತ್ಯಾಗಮ 1;
	}
	g_listensock_ಎಣಿಕೆ += listener->sock_ಎಣಿಕೆ;
	listensock_new = mosquitto_realloc(g_listensock, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__listener_sock)*(size_t)g_listensock_ಎಣಿಕೆ);
	ಯದಿ(!listensock_new){
		ಪ್ರತ್ಯಾಗಮ 1;
	}
	g_listensock = listensock_new;

	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<listener->sock_ಎಣಿಕೆ; i++){
		ಯದಿ(listener->socks[i] == INVALID_SOCKET){
			ಪ್ರತ್ಯಾಗಮ 1;
		}
		g_listensock[listensock_index].sock = listener->socks[i];
		g_listensock[listensock_index].listener = listener;
#ಯದಿ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
		g_listensock[listensock_index].ident = id_listener;
#endif
		listensock_index++;
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS


ಶೂನ್ಯ listeners__add_websockets(ಸಂರಚನಾ lws_context *ws_context, mosq_sock_t fd)
{
	ಸಂರಚನಾ mosquitto__listener *listener = ರಿಕ್ತ;
	ಸಂರಚನಾ mosquitto__listener_sock *listensock_new;

	/* Don't add more listeners after we've started the main loop */
	ಯದಿ(g_run || ws_context == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ;
	}

	/* Find context */
	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
		ಯದಿ(db.config->listeners[i].ws_in_init){
			listener = &db.config->listeners[i];
			ಭಙ್ಗ;
		}
	}
	ಯದಿ(listener == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ;
	}

	g_listensock_ಎಣಿಕೆ++;
	listensock_new = mosquitto_realloc(g_listensock, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__listener_sock)*(size_t)g_listensock_ಎಣಿಕೆ);
	ಯದಿ(!listensock_new){
		ಪ್ರತ್ಯಾಗಮ;
	}
	g_listensock = listensock_new;

	g_listensock[listensock_index].sock = fd;
	g_listensock[listensock_index].listener = listener;
#ಯದಿ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	g_listensock[listensock_index].ident = id_listener_ws;
#endif
	listensock_index++;
}
#endif


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ listeners__add_local(ಧ್ರುವ ವರ್ಣ *host, uint16_t port)
{
	ಸಂರಚನಾ mosquitto__listener *listeners;
	bool allow_anonymous;

	listeners = db.config->listeners;
	ಯದಿ(db.config->security_options.allow_anonymous == -1){
		allow_anonymous = true;
	}ಅನ್ಯಥಾ{
		allow_anonymous = db.config->security_options.allow_anonymous;
	}

	listeners[db.config->listener_ಎಣಿಕೆ].security_options = mosquitto_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__security_options));
	ಯದಿ(listeners[db.config->listener_ಎಣಿಕೆ].security_options == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}

	listener__set_defaults(&listeners[db.config->listener_ಎಣಿಕೆ]);
	listeners[db.config->listener_ಎಣಿಕೆ].security_options->allow_anonymous = allow_anonymous;
	listeners[db.config->listener_ಎಣಿಕೆ].security_options->auto_id_prefix = mosquitto_strdup("auto-");
	ಯದಿ(listeners[db.config->listener_ಎಣಿಕೆ].security_options->auto_id_prefix == ರಿಕ್ತ){
		mosquitto_FREE(listeners[db.config->listener_ಎಣಿಕೆ].security_options);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}
	listeners[db.config->listener_ಎಣಿಕೆ].security_options->auto_id_prefix_len = (ಪೂರ್ಣಾಙ್ಕ)strlen("auto-");
	listeners[db.config->listener_ಎಣಿಕೆ].port = port;
	listeners[db.config->listener_ಎಣಿಕೆ].host = mosquitto_strdup(host);
	ಯದಿ(listeners[db.config->listener_ಎಣಿಕೆ].host == ರಿಕ್ತ){
		mosquitto_FREE(listeners[db.config->listener_ಎಣಿಕೆ].security_options->auto_id_prefix);
		mosquitto_FREE(listeners[db.config->listener_ಎಣಿಕೆ].security_options);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}
	ಯದಿ(listeners__start_single_mqtt(&listeners[db.config->listener_ಎಣಿಕೆ])){
		mosquitto_FREE(listeners[db.config->listener_ಎಣಿಕೆ].security_options->auto_id_prefix);
		mosquitto_FREE(listeners[db.config->listener_ಎಣಿಕೆ].security_options);
		mosquitto_FREE(listeners[db.config->listener_ಎಣಿಕೆ].host);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
	}
	db.config->listener_ಎಣಿಕೆ++;
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ listeners__start_local_only(ಶೂನ್ಯ)
{
	/* Attempt to open listeners bound to 127.0.0.1 and ::1 only */
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ mosquitto__listener *listeners;
	size_t ಎಣಿಕೆ;

	ಯದಿ(db.config->cmd_port_ಎಣಿಕೆ == 0){
		ಎಣಿಕೆ = 2;
	}ಅನ್ಯಥಾ{
		ಎಣಿಕೆ = (size_t)(db.config->cmd_port_ಎಣಿಕೆ*2);
	}

#ifdef WITH_HTTP_API
	ಎಣಿಕೆ++;
#endif

	listeners = mosquitto_realloc(db.config->listeners, ಎಣಿಕೆ*ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__listener));
	ಯದಿ(listeners == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}
	memset(listeners, 0, ಎಣಿಕೆ*ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__listener));
	db.config->listener_ಎಣಿಕೆ = 0;
	db.config->listeners = listeners;

	log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Starting in local only mode. Connections will only be possible from clients running on this machine.");
	log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Create a configuration file which defines a listener to allow remote access.");
	log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "For more details see https://mosquitto.org/documentation/authentication-methods/");
	ಯದಿ(db.config->cmd_port_ಎಣಿಕೆ == 0){
		rc = listeners__add_local("127.0.0.1", 1883);
		ಯದಿ(rc == MOSQ_ERR_NOMEM){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}
		rc = listeners__add_local("::1", 1883);
		ಯದಿ(rc == MOSQ_ERR_NOMEM){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}
	}ಅನ್ಯಥಾ{
		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<db.config->cmd_port_ಎಣಿಕೆ; i++){
			rc = listeners__add_local("127.0.0.1", db.config->cmd_port[i]);
			ಯದಿ(rc == MOSQ_ERR_NOMEM){
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
			}
			rc = listeners__add_local("::1", db.config->cmd_port[i]);
			ಯದಿ(rc == MOSQ_ERR_NOMEM){
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
			}
		}
	}

	ಯದಿ(db.config->listener_ಎಣಿಕೆ > 0){
#ifdef WITH_HTTP_API
		db.config->listener_ಎಣಿಕೆ++;
		http_api__start_local(&db.config->listeners[db.config->listener_ಎಣಿಕೆ-1]);
#endif
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
	}
}


ಪೂರ್ಣಾಙ್ಕ listeners__start(ಶೂನ್ಯ)
{
	g_listensock_ಎಣಿಕೆ = 0;

	ಯದಿ(db.config->local_only){
		ಯದಿ(listeners__start_local_only()){
			db__close();
			ಯದಿ(db.config->pid_file){
				(ಶೂನ್ಯ)remove(db.config->pid_file);
			}
			ಪ್ರತ್ಯಾಗಮ 1;
		}
		mux__add_listeners(g_listensock, g_listensock_ಎಣಿಕೆ);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}

	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
		ಯದಿ(db.config->listeners[i].protocol == mp_mqtt){
			ಯದಿ(listeners__start_single_mqtt(&db.config->listeners[i])){
				db__close();
				ಯದಿ(db.config->pid_file){
					(ಶೂನ್ಯ)remove(db.config->pid_file);
				}
				ಪ್ರತ್ಯಾಗಮ 1;
			}
		}ಅನ್ಯಥಾ ಯದಿ(db.config->listeners[i].protocol == mp_websockets){
#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			mosq_websockets_init(&db.config->listeners[i], db.config);
			ಯದಿ(!db.config->listeners[i].ws_context){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to create websockets listener on port %d.", db.config->listeners[i].port);
				ಪ್ರತ್ಯಾಗಮ 1;
			}
#ನೋಚೇದ್ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			ಯದಿ(listeners__start_single_mqtt(&db.config->listeners[i])){
				db__close();
				ಯದಿ(db.config->pid_file){
					(ಶೂನ್ಯ)remove(db.config->pid_file);
				}
				ಪ್ರತ್ಯಾಗಮ 1;
			}
#endif
#ifdef WITH_HTTP_API
		}ಅನ್ಯಥಾ ಯದಿ(db.config->listeners[i].protocol == mp_http_api){
			http_api__start(&db.config->listeners[i]);
#endif
		}
	}
	ಯದಿ(g_listensock == ರಿಕ್ತ){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to start any listening sockets, exiting.");
		ಪ್ರತ್ಯಾಗಮ 1;
	}

	mux__add_listeners(g_listensock, g_listensock_ಎಣಿಕೆ);
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಶೂನ್ಯ listeners__stop(ಶೂನ್ಯ)
{
	mux__delete_listeners(g_listensock, g_listensock_ಎಣಿಕೆ);

	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ಯದಿ(db.config->listeners[i].ws_context){
			lws_context_destroy(db.config->listeners[i].ws_context);
		}
		mosquitto_FREE(db.config->listeners[i].ws_protocol);
#endif
#ifdef WITH_UNIX_SOCKETS
		ಯದಿ(db.config->listeners[i].unix_socket_path != ರಿಕ್ತ){
			unlink(db.config->listeners[i].unix_socket_path);
		}
#endif
#ifdef WITH_HTTP_API
		ಯದಿ(db.config->listeners[i].mhd){
			http_api__stop(&db.config->listeners[i]);
		}
#endif
	}

	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<g_listensock_ಎಣಿಕೆ; i++){
		ಯದಿ(g_listensock[i].sock != INVALID_SOCKET){
			COMPAT_CLOSE(g_listensock[i].sock);
		}
	}
	mosquitto_FREE(g_listensock);
	g_listensock_ಎಣಿಕೆ = 0;
	listensock_index = 0;
}
