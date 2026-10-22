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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "net_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"

ਸਥਿਰ ਪੂਰਨ listensock_ਸੂਚਕਾਂਕ = 0;
ਬਾਹਰੀ ਪੂਰਨ g_ਲੜੀਵਾਰ;


ਖਾਲੀ listener_ਸੈੱਟ_defaults(ਬਣਤਰ mosquitto__listener *listener)
{
	listener->disable_protocol_v3 = false;
	listener->disable_protocol_v4 = false;
	listener->disable_protocol_v5 = false;
	listener->max_connections = -1;
	listener->max_qos = 2;
	listener->max_topic_alias = 10;
	listener->max_topic_alias_broker = 10;
	listener->protocol = mp_mqtt;
	mosquitto_FREE(listener->mount_ਬਿੰਦੂ);

	mosquitto_FREE(listener->security_options->acl_ਅੰਕੜੇ.acl_file);
	mosquitto_FREE(listener->security_options->password_ਅੰਕੜੇ.password_file);
	mosquitto_FREE(listener->security_options->psk_file);
	listener->security_options->allow_anonymous = -1;
	listener->security_options->allow_zero_ਲੰਮਾਈ_clientid = true;
	mosquitto_FREE(listener->security_options->auto_id_ਅਗੇਤਰ);
	listener->security_options->auto_id_ਅਗੇਤਰ_len = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
	listener->require_certificate = false;
	listener->use_ਪਛਾਣ_ਤੱਤ_as_username = false;
	listener->use_subject_as_username = false;
	listener->use_username_as_clientid = false;
	listener->disable_client_cert_date_checks = false;
#ਜੇ_ਅੰਤ

#ਜੇ defined(WITH_WEBSOCKETS) && (LWS_LIBRARY_ਸੰਸਕਰਣ_ਸੰਖਿਆ >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN)
	ਲਈ(ਪੂਰਨ i=0; i<listener->ws_origin_ਗਿਣਤੀ; i++){
		mosquitto_FREE(listener->ws_origins[i]);
	}
	mosquitto_FREE(listener->ws_origins);
	listener->ws_origin_ਗਿਣਤੀ = 0;
#ਜੇ_ਅੰਤ
}


ਖਾਲੀ listeners__reload_all_certificates(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
	ਲਈ(ਪੂਰਨ i=0; i<db.config->listener_ਗਿਣਤੀ; i++){
		ਬਣਤਰ mosquitto__listener *listener = &db.config->listeners[i];
		ਜੇ(listener->ssl_ctx && listener->certfile && listener->keyfile){
			ਪੂਰਨ rc = net_ਲੱਦੋ_certificates(listener);
			ਜੇ(rc){
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error when reloading certificate '%s' or key '%s'.",
						listener->certfile, listener->keyfile);
			}
		}
	}
#ਜੇ_ਅੰਤ
}


ਸਥਿਰ ਪੂਰਨ listeners_ਸ਼ੁਰੂ_single_mqtt(ਬਣਤਰ mosquitto__listener *listener)
{
	ਬਣਤਰ mosquitto__listener_sock *listensock_new;

	ਜੇ(net__socket_listen(listener)){
		ਵਾਪਸ 1;
	}
	g_listensock_ਗਿਣਤੀ += listener->sock_ਗਿਣਤੀ;
	listensock_new = mosquitto_realloc(g_listensock, ਆਕਾਰ(ਬਣਤਰ mosquitto__listener_sock)*(size_t)g_listensock_ਗਿਣਤੀ);
	ਜੇ(!listensock_new){
		ਵਾਪਸ 1;
	}
	g_listensock = listensock_new;

	ਲਈ(ਪੂਰਨ i=0; i<listener->sock_ਗਿਣਤੀ; i++){
		ਜੇ(listener->socks[i] == INVALID_SOCKET){
			ਵਾਪਸ 1;
		}
		g_listensock[listensock_ਸੂਚਕਾਂਕ].sock = listener->socks[i];
		g_listensock[listensock_ਸੂਚਕਾਂਕ].listener = listener;
#ਜੇ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
		g_listensock[listensock_ਸੂਚਕਾਂਕ].ident = id_listener;
#ਜੇ_ਅੰਤ
		listensock_ਸੂਚਕਾਂਕ++;
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS


ਖਾਲੀ listeners_ਜੋੜੋ_websockets(ਬਣਤਰ lws_context *ws_context, mosq_sock_t fd)
{
	ਬਣਤਰ mosquitto__listener *listener = ਨਿਰਾ;
	ਬਣਤਰ mosquitto__listener_sock *listensock_new;

	/* Don't add more listeners after we've started the main loop */
	ਜੇ(g_ਲੜੀਵਾਰ || ws_context == ਨਿਰਾ){
		ਵਾਪਸ;
	}

	/* Find context */
	ਲਈ(ਪੂਰਨ i=0; i<db.config->listener_ਗਿਣਤੀ; i++){
		ਜੇ(db.config->listeners[i].ws_ਵਿੱਚ_init){
			listener = &db.config->listeners[i];
			ਰੋਕੋ;
		}
	}
	ਜੇ(listener == ਨਿਰਾ){
		ਵਾਪਸ;
	}

	g_listensock_ਗਿਣਤੀ++;
	listensock_new = mosquitto_realloc(g_listensock, ਆਕਾਰ(ਬਣਤਰ mosquitto__listener_sock)*(size_t)g_listensock_ਗਿਣਤੀ);
	ਜੇ(!listensock_new){
		ਵਾਪਸ;
	}
	g_listensock = listensock_new;

	g_listensock[listensock_ਸੂਚਕਾਂਕ].sock = fd;
	g_listensock[listensock_ਸੂਚਕਾਂਕ].listener = listener;
#ਜੇ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	g_listensock[listensock_ਸੂਚਕਾਂਕ].ident = id_listener_ws;
#ਜੇ_ਅੰਤ
	listensock_ਸੂਚਕਾਂਕ++;
}
#ਜੇ_ਅੰਤ


ਸਥਿਰ ਪੂਰਨ listeners_ਜੋੜੋ_local(ਅਚਲ ਅੱਖਰ *host, uint16_t ਪੋਰਟ)
{
	ਬਣਤਰ mosquitto__listener *listeners;
	bool allow_anonymous;

	listeners = db.config->listeners;
	ਜੇ(db.config->security_options.allow_anonymous == -1){
		allow_anonymous = true;
	}ਨਹੀਂਤਾਂ{
		allow_anonymous = db.config->security_options.allow_anonymous;
	}

	listeners[db.config->listener_ਗਿਣਤੀ].security_options = mosquitto_calloc(1, ਆਕਾਰ(ਬਣਤਰ mosquitto__security_options));
	ਜੇ(listeners[db.config->listener_ਗਿਣਤੀ].security_options == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	listener_ਸੈੱਟ_defaults(&listeners[db.config->listener_ਗਿਣਤੀ]);
	listeners[db.config->listener_ਗਿਣਤੀ].security_options->allow_anonymous = allow_anonymous;
	listeners[db.config->listener_ਗਿਣਤੀ].security_options->auto_id_ਅਗੇਤਰ = mosquitto_strdup("auto-");
	ਜੇ(listeners[db.config->listener_ਗਿਣਤੀ].security_options->auto_id_ਅਗੇਤਰ == ਨਿਰਾ){
		mosquitto_FREE(listeners[db.config->listener_ਗਿਣਤੀ].security_options);
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}
	listeners[db.config->listener_ਗਿਣਤੀ].security_options->auto_id_ਅਗੇਤਰ_len = (ਪੂਰਨ)ਲੜੀ_ਲੰਬਾਈ("auto-");
	listeners[db.config->listener_ਗਿਣਤੀ].ਪੋਰਟ = ਪੋਰਟ;
	listeners[db.config->listener_ਗਿਣਤੀ].host = mosquitto_strdup(host);
	ਜੇ(listeners[db.config->listener_ਗਿਣਤੀ].host == ਨਿਰਾ){
		mosquitto_FREE(listeners[db.config->listener_ਗਿਣਤੀ].security_options->auto_id_ਅਗੇਤਰ);
		mosquitto_FREE(listeners[db.config->listener_ਗਿਣਤੀ].security_options);
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}
	ਜੇ(listeners_ਸ਼ੁਰੂ_single_mqtt(&listeners[db.config->listener_ਗਿਣਤੀ])){
		mosquitto_FREE(listeners[db.config->listener_ਗਿਣਤੀ].security_options->auto_id_ਅਗੇਤਰ);
		mosquitto_FREE(listeners[db.config->listener_ਗਿਣਤੀ].security_options);
		mosquitto_FREE(listeners[db.config->listener_ਗਿਣਤੀ].host);
		ਵਾਪਸ MOSQ_ERR_UNKNOWN;
	}
	db.config->listener_ਗਿਣਤੀ++;
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ listeners_ਸ਼ੁਰੂ_local_only(ਖਾਲੀ)
{
	/* Attempt to open listeners bound to 127.0.0.1 and ::1 only */
	ਪੂਰਨ rc;
	ਬਣਤਰ mosquitto__listener *listeners;
	size_t ਗਿਣਤੀ_2;

	ਜੇ(db.config->cmd_ਪੋਰਟ_ਗਿਣਤੀ == 0){
		ਗਿਣਤੀ_2 = 2;
	}ਨਹੀਂਤਾਂ{
		ਗਿਣਤੀ_2 = (size_t)(db.config->cmd_ਪੋਰਟ_ਗਿਣਤੀ*2);
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_HTTP_API
	ਗਿਣਤੀ_2++;
#ਜੇ_ਅੰਤ

	listeners = mosquitto_realloc(db.config->listeners, ਗਿਣਤੀ_2*ਆਕਾਰ(ਬਣਤਰ mosquitto__listener));
	ਜੇ(listeners == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}
	memset(listeners, 0, ਗਿਣਤੀ_2*ਆਕਾਰ(ਬਣਤਰ mosquitto__listener));
	db.config->listener_ਗਿਣਤੀ = 0;
	db.config->listeners = listeners;

	log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Starting in local only mode. Connections will only be possible from clients running on this machine.");
	log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Create a configuration file which defines a listener to allow remote access.");
	log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "For more details see https://mosquitto.org/documentation/authentication-methods/");
	ਜੇ(db.config->cmd_ਪੋਰਟ_ਗਿਣਤੀ == 0){
		rc = listeners_ਜੋੜੋ_local("127.0.0.1", 1883);
		ਜੇ(rc == MOSQ_ERR_NOMEM){
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}
		rc = listeners_ਜੋੜੋ_local("::1", 1883);
		ਜੇ(rc == MOSQ_ERR_NOMEM){
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}
	}ਨਹੀਂਤਾਂ{
		ਲਈ(ਪੂਰਨ i=0; i<db.config->cmd_ਪੋਰਟ_ਗਿਣਤੀ; i++){
			rc = listeners_ਜੋੜੋ_local("127.0.0.1", db.config->cmd_ਪੋਰਟ[i]);
			ਜੇ(rc == MOSQ_ERR_NOMEM){
				ਵਾਪਸ MOSQ_ERR_NOMEM;
			}
			rc = listeners_ਜੋੜੋ_local("::1", db.config->cmd_ਪੋਰਟ[i]);
			ਜੇ(rc == MOSQ_ERR_NOMEM){
				ਵਾਪਸ MOSQ_ERR_NOMEM;
			}
		}
	}

	ਜੇ(db.config->listener_ਗਿਣਤੀ > 0){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_HTTP_API
		db.config->listener_ਗਿਣਤੀ++;
		http_api_ਸ਼ੁਰੂ_local(&db.config->listeners[db.config->listener_ਗਿਣਤੀ-1]);
#ਜੇ_ਅੰਤ
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ MOSQ_ERR_UNKNOWN;
	}
}


ਪੂਰਨ listeners_ਸ਼ੁਰੂ(ਖਾਲੀ)
{
	g_listensock_ਗਿਣਤੀ = 0;

	ਜੇ(db.config->local_only){
		ਜੇ(listeners_ਸ਼ੁਰੂ_local_only()){
			db__close();
			ਜੇ(db.config->pid_file){
				(ਖਾਲੀ)remove(db.config->pid_file);
			}
			ਵਾਪਸ 1;
		}
		mux_ਜੋੜੋ_listeners(g_listensock, g_listensock_ਗਿਣਤੀ);
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

	ਲਈ(ਪੂਰਨ i=0; i<db.config->listener_ਗਿਣਤੀ; i++){
		ਜੇ(db.config->listeners[i].protocol == mp_mqtt){
			ਜੇ(listeners_ਸ਼ੁਰੂ_single_mqtt(&db.config->listeners[i])){
				db__close();
				ਜੇ(db.config->pid_file){
					(ਖਾਲੀ)remove(db.config->pid_file);
				}
				ਵਾਪਸ 1;
			}
		}ਨਹੀਂਤਾਂ ਜੇ(db.config->listeners[i].protocol == mp_websockets){
#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			mosq_websockets_init(&db.config->listeners[i], db.config);
			ਜੇ(!db.config->listeners[i].ws_context){
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to create websockets listener on port %d.", db.config->listeners[i].ਪੋਰਟ);
				ਵਾਪਸ 1;
			}
#ਜਾਂਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			ਜੇ(listeners_ਸ਼ੁਰੂ_single_mqtt(&db.config->listeners[i])){
				db__close();
				ਜੇ(db.config->pid_file){
					(ਖਾਲੀ)remove(db.config->pid_file);
				}
				ਵਾਪਸ 1;
			}
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_HTTP_API
		}ਨਹੀਂਤਾਂ ਜੇ(db.config->listeners[i].protocol == mp_http_api){
			http_api_ਸ਼ੁਰੂ(&db.config->listeners[i]);
#ਜੇ_ਅੰਤ
		}
	}
	ਜੇ(g_listensock == ਨਿਰਾ){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to start any listening sockets, exiting.");
		ਵਾਪਸ 1;
	}

	mux_ਜੋੜੋ_listeners(g_listensock, g_listensock_ਗਿਣਤੀ);
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਖਾਲੀ listeners__stop(ਖਾਲੀ)
{
	mux__delete_listeners(g_listensock, g_listensock_ਗਿਣਤੀ);

	ਲਈ(ਪੂਰਨ i=0; i<db.config->listener_ਗਿਣਤੀ; i++){
#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ਜੇ(db.config->listeners[i].ws_context){
			lws_context_destroy(db.config->listeners[i].ws_context);
		}
		mosquitto_FREE(db.config->listeners[i].ws_protocol);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_UNIX_SOCKETS
		ਜੇ(db.config->listeners[i].unix_socket_ਰਸਤਾ != ਨਿਰਾ){
			unlink(db.config->listeners[i].unix_socket_ਰਸਤਾ);
		}
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_HTTP_API
		ਜੇ(db.config->listeners[i].mhd){
			http_api__stop(&db.config->listeners[i]);
		}
#ਜੇ_ਅੰਤ
	}

	ਲਈ(ਪੂਰਨ i=0; i<g_listensock_ਗਿਣਤੀ; i++){
		ਜੇ(g_listensock[i].sock != INVALID_SOCKET){
			COMPAT_CLOSE(g_listensock[i].sock);
		}
	}
	mosquitto_FREE(g_listensock);
	g_listensock_ਗਿਣਤੀ = 0;
	listensock_ਸੂਚਕਾਂਕ = 0;
}
