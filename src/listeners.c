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

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ listensock_ಸೂಚ್ಯಂಕ = 0;
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ g_ಸತತ;


ಶೂನ್ಯ listener_ಗಣ_defaults(ರಚನೆ mosquitto__listener *listener)
{
	listener->disable_protocol_v3 = false;
	listener->disable_protocol_v4 = false;
	listener->disable_protocol_v5 = false;
	listener->max_connections = -1;
	listener->max_qos = 2;
	listener->max_topic_alias = 10;
	listener->max_topic_alias_broker = 10;
	listener->protocol = mp_mqtt;
	mosquitto_FREE(listener->mount_ಬಿಂದು);

	mosquitto_FREE(listener->security_options->acl_ದತ್ತಾಂಶ.acl_file);
	mosquitto_FREE(listener->security_options->password_ದತ್ತಾಂಶ.password_file);
	mosquitto_FREE(listener->security_options->psk_file);
	listener->security_options->allow_anonymous = -1;
	listener->security_options->allow_zero_ಉದ್ದಳತೆ_clientid = true;
	mosquitto_FREE(listener->security_options->auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ);
	listener->security_options->auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ_len = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
	listener->require_certificate = false;
	listener->use_ತಟಸ್ಥ_as_username = false;
	listener->use_subject_as_username = false;
	listener->use_username_as_clientid = false;
	listener->disable_client_cert_date_checks = false;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(WITH_WEBSOCKETS) && (LWS_LIBRARY_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN)
	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<listener->ws_origin_ಎಣಿಕೆ; i++){
		mosquitto_FREE(listener->ws_origins[i]);
	}
	mosquitto_FREE(listener->ws_origins);
	listener->ws_origin_ಎಣಿಕೆ = 0;
#ಆದರೆ_ಅಂತ್ಯ
}


ಶೂನ್ಯ listeners__reload_all_certificates(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
		ರಚನೆ mosquitto__listener *listener = &db.config->listeners[i];
		ಆದರೆ(listener->ssl_ctx && listener->certfile && listener->keyfile){
			ಪೂರ್ಣಾಂಕ rc = net_ತುಂಬು_certificates(listener);
			ಆದರೆ(rc){
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error when reloading certificate '%s' or key '%s'.",
						listener->certfile, listener->keyfile);
			}
		}
	}
#ಆದರೆ_ಅಂತ್ಯ
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ listeners_ಆರಂಭ_single_mqtt(ರಚನೆ mosquitto__listener *listener)
{
	ರಚನೆ mosquitto__listener_sock *listensock_new;

	ಆದರೆ(net__socket_listen(listener)){
		ಹಿಂತಿರುಗಿ 1;
	}
	g_listensock_ಎಣಿಕೆ += listener->sock_ಎಣಿಕೆ;
	listensock_new = mosquitto_realloc(g_listensock, ಗಾತ್ರ(ರಚನೆ mosquitto__listener_sock)*(size_t)g_listensock_ಎಣಿಕೆ);
	ಆದರೆ(!listensock_new){
		ಹಿಂತಿರುಗಿ 1;
	}
	g_listensock = listensock_new;

	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<listener->sock_ಎಣಿಕೆ; i++){
		ಆದರೆ(listener->socks[i] == INVALID_SOCKET){
			ಹಿಂತಿರುಗಿ 1;
		}
		g_listensock[listensock_ಸೂಚ್ಯಂಕ].sock = listener->socks[i];
		g_listensock[listensock_ಸೂಚ್ಯಂಕ].listener = listener;
#ಆದರೆ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
		g_listensock[listensock_ಸೂಚ್ಯಂಕ].ident = id_listener;
#ಆದರೆ_ಅಂತ್ಯ
		listensock_ಸೂಚ್ಯಂಕ++;
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS


ಶೂನ್ಯ listeners_ಕೂಡಿಸು_websockets(ರಚನೆ lws_context *ws_context, mosq_sock_t fd)
{
	ರಚನೆ mosquitto__listener *listener = ಖಾಲಿ;
	ರಚನೆ mosquitto__listener_sock *listensock_new;

	/* Don't add more listeners after we've started the main loop */
	ಆದರೆ(g_ಸತತ || ws_context == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ;
	}

	/* Find context */
	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
		ಆದರೆ(db.config->listeners[i].ws_ಒಳ_init){
			listener = &db.config->listeners[i];
			ನಿಲ್ಲಿಸು;
		}
	}
	ಆದರೆ(listener == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ;
	}

	g_listensock_ಎಣಿಕೆ++;
	listensock_new = mosquitto_realloc(g_listensock, ಗಾತ್ರ(ರಚನೆ mosquitto__listener_sock)*(size_t)g_listensock_ಎಣಿಕೆ);
	ಆದರೆ(!listensock_new){
		ಹಿಂತಿರುಗಿ;
	}
	g_listensock = listensock_new;

	g_listensock[listensock_ಸೂಚ್ಯಂಕ].sock = fd;
	g_listensock[listensock_ಸೂಚ್ಯಂಕ].listener = listener;
#ಆದರೆ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	g_listensock[listensock_ಸೂಚ್ಯಂಕ].ident = id_listener_ws;
#ಆದರೆ_ಅಂತ್ಯ
	listensock_ಸೂಚ್ಯಂಕ++;
}
#ಆದರೆ_ಅಂತ್ಯ


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ listeners_ಕೂಡಿಸು_local(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host, uint16_t ಪೋರ್ಟ್)
{
	ರಚನೆ mosquitto__listener *listeners;
	bool allow_anonymous;

	listeners = db.config->listeners;
	ಆದರೆ(db.config->security_options.allow_anonymous == -1){
		allow_anonymous = true;
	}ಇಲ್ಲದಿದ್ದರೆ{
		allow_anonymous = db.config->security_options.allow_anonymous;
	}

	listeners[db.config->listener_ಎಣಿಕೆ].security_options = mosquitto_calloc(1, ಗಾತ್ರ(ರಚನೆ mosquitto__security_options));
	ಆದರೆ(listeners[db.config->listener_ಎಣಿಕೆ].security_options == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}

	listener_ಗಣ_defaults(&listeners[db.config->listener_ಎಣಿಕೆ]);
	listeners[db.config->listener_ಎಣಿಕೆ].security_options->allow_anonymous = allow_anonymous;
	listeners[db.config->listener_ಎಣಿಕೆ].security_options->auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ = mosquitto_strdup("auto-");
	ಆದರೆ(listeners[db.config->listener_ಎಣಿಕೆ].security_options->auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ == ಖಾಲಿ){
		mosquitto_FREE(listeners[db.config->listener_ಎಣಿಕೆ].security_options);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}
	listeners[db.config->listener_ಎಣಿಕೆ].security_options->auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ_len = (ಪೂರ್ಣಾಂಕ)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("auto-");
	listeners[db.config->listener_ಎಣಿಕೆ].ಪೋರ್ಟ್ = ಪೋರ್ಟ್;
	listeners[db.config->listener_ಎಣಿಕೆ].host = mosquitto_strdup(host);
	ಆದರೆ(listeners[db.config->listener_ಎಣಿಕೆ].host == ಖಾಲಿ){
		mosquitto_FREE(listeners[db.config->listener_ಎಣಿಕೆ].security_options->auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ);
		mosquitto_FREE(listeners[db.config->listener_ಎಣಿಕೆ].security_options);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}
	ಆದರೆ(listeners_ಆರಂಭ_single_mqtt(&listeners[db.config->listener_ಎಣಿಕೆ])){
		mosquitto_FREE(listeners[db.config->listener_ಎಣಿಕೆ].security_options->auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ);
		mosquitto_FREE(listeners[db.config->listener_ಎಣಿಕೆ].security_options);
		mosquitto_FREE(listeners[db.config->listener_ಎಣಿಕೆ].host);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_UNKNOWN;
	}
	db.config->listener_ಎಣಿಕೆ++;
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ listeners_ಆರಂಭ_local_only(ಶೂನ್ಯ)
{
	/* Attempt to open listeners bound to 127.0.0.1 and ::1 only */
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ mosquitto__listener *listeners;
	size_t ಎಣಿಕೆ_2;

	ಆದರೆ(db.config->cmd_ಪೋರ್ಟ್_ಎಣಿಕೆ == 0){
		ಎಣಿಕೆ_2 = 2;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಎಣಿಕೆ_2 = (size_t)(db.config->cmd_ಪೋರ್ಟ್_ಎಣಿಕೆ*2);
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_HTTP_API
	ಎಣಿಕೆ_2++;
#ಆದರೆ_ಅಂತ್ಯ

	listeners = mosquitto_realloc(db.config->listeners, ಎಣಿಕೆ_2*ಗಾತ್ರ(ರಚನೆ mosquitto__listener));
	ಆದರೆ(listeners == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}
	memset(listeners, 0, ಎಣಿಕೆ_2*ಗಾತ್ರ(ರಚನೆ mosquitto__listener));
	db.config->listener_ಎಣಿಕೆ = 0;
	db.config->listeners = listeners;

	log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Starting in local only mode. Connections will only be possible from clients running on this machine.");
	log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Create a configuration file which defines a listener to allow remote access.");
	log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "For more details see https://mosquitto.org/documentation/authentication-methods/");
	ಆದರೆ(db.config->cmd_ಪೋರ್ಟ್_ಎಣಿಕೆ == 0){
		rc = listeners_ಕೂಡಿಸು_local("127.0.0.1", 1883);
		ಆದರೆ(rc == MOSQ_ERR_NOMEM){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}
		rc = listeners_ಕೂಡಿಸು_local("::1", 1883);
		ಆದರೆ(rc == MOSQ_ERR_NOMEM){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<db.config->cmd_ಪೋರ್ಟ್_ಎಣಿಕೆ; i++){
			rc = listeners_ಕೂಡಿಸು_local("127.0.0.1", db.config->cmd_ಪೋರ್ಟ್[i]);
			ಆದರೆ(rc == MOSQ_ERR_NOMEM){
				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
			}
			rc = listeners_ಕೂಡಿಸು_local("::1", db.config->cmd_ಪೋರ್ಟ್[i]);
			ಆದರೆ(rc == MOSQ_ERR_NOMEM){
				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
			}
		}
	}

	ಆದರೆ(db.config->listener_ಎಣಿಕೆ > 0){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_HTTP_API
		db.config->listener_ಎಣಿಕೆ++;
		http_api_ಆರಂಭ_local(&db.config->listeners[db.config->listener_ಎಣಿಕೆ-1]);
#ಆದರೆ_ಅಂತ್ಯ
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ MOSQ_ERR_UNKNOWN;
	}
}


ಪೂರ್ಣಾಂಕ listeners_ಆರಂಭ(ಶೂನ್ಯ)
{
	g_listensock_ಎಣಿಕೆ = 0;

	ಆದರೆ(db.config->local_only){
		ಆದರೆ(listeners_ಆರಂಭ_local_only()){
			db__close();
			ಆದರೆ(db.config->pid_file){
				(ಶೂನ್ಯ)remove(db.config->pid_file);
			}
			ಹಿಂತಿರುಗಿ 1;
		}
		mux_ಕೂಡಿಸು_listeners(g_listensock, g_listensock_ಎಣಿಕೆ);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}

	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
		ಆದರೆ(db.config->listeners[i].protocol == mp_mqtt){
			ಆದರೆ(listeners_ಆರಂಭ_single_mqtt(&db.config->listeners[i])){
				db__close();
				ಆದರೆ(db.config->pid_file){
					(ಶೂನ್ಯ)remove(db.config->pid_file);
				}
				ಹಿಂತಿರುಗಿ 1;
			}
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(db.config->listeners[i].protocol == mp_websockets){
#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			mosq_websockets_init(&db.config->listeners[i], db.config);
			ಆದರೆ(!db.config->listeners[i].ws_context){
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to create websockets listener on port %d.", db.config->listeners[i].ಪೋರ್ಟ್);
				ಹಿಂತಿರುಗಿ 1;
			}
#ಅಥವಾಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			ಆದರೆ(listeners_ಆರಂಭ_single_mqtt(&db.config->listeners[i])){
				db__close();
				ಆದರೆ(db.config->pid_file){
					(ಶೂನ್ಯ)remove(db.config->pid_file);
				}
				ಹಿಂತಿರುಗಿ 1;
			}
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_HTTP_API
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(db.config->listeners[i].protocol == mp_http_api){
			http_api_ಆರಂಭ(&db.config->listeners[i]);
#ಆದರೆ_ಅಂತ್ಯ
		}
	}
	ಆದರೆ(g_listensock == ಖಾಲಿ){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to start any listening sockets, exiting.");
		ಹಿಂತಿರುಗಿ 1;
	}

	mux_ಕೂಡಿಸು_listeners(g_listensock, g_listensock_ಎಣಿಕೆ);
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಶೂನ್ಯ listeners__stop(ಶೂನ್ಯ)
{
	mux__delete_listeners(g_listensock, g_listensock_ಎಣಿಕೆ);

	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ಆದರೆ(db.config->listeners[i].ws_context){
			lws_context_destroy(db.config->listeners[i].ws_context);
		}
		mosquitto_FREE(db.config->listeners[i].ws_protocol);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_UNIX_SOCKETS
		ಆದರೆ(db.config->listeners[i].unix_socket_ಮಾರ್ಗ != ಖಾಲಿ){
			unlink(db.config->listeners[i].unix_socket_ಮಾರ್ಗ);
		}
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_HTTP_API
		ಆದರೆ(db.config->listeners[i].mhd){
			http_api__stop(&db.config->listeners[i]);
		}
#ಆದರೆ_ಅಂತ್ಯ
	}

	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<g_listensock_ಎಣಿಕೆ; i++){
		ಆದರೆ(g_listensock[i].sock != INVALID_SOCKET){
			COMPAT_CLOSE(g_listensock[i].sock);
		}
	}
	mosquitto_FREE(g_listensock);
	g_listensock_ಎಣಿಕೆ = 0;
	listensock_ಸೂಚ್ಯಂಕ = 0;
}
