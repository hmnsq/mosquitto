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

#அந்தர்பாவ "config.h"
#அந்தர்பாவ "net_mosq.h"
#அந்தர்பாவ "mosquitto_broker_internal.h"

நிலையான முழு listensock_index = 0;
வெளி முழு g_run;


வெறுமை listener__set_defaults(கட்டமைப்பு mosquitto__listener *listener)
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

#எனில் defined(WITH_WEBSOCKETS) && (LWS_LIBRARY_VERSION_எண் >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN)
	க்கு(முழு i=0; i<listener->ws_origin_மொத்தம்; i++){
		mosquitto_FREE(listener->ws_origins[i]);
	}
	mosquitto_FREE(listener->ws_origins);
	listener->ws_origin_மொத்தம் = 0;
#endif
}


வெறுமை listeners__reload_all_certificates(வெறுமை)
{
#ifdef WITH_TLS
	க்கு(முழு i=0; i<db.config->listener_மொத்தம்; i++){
		கட்டமைப்பு mosquitto__listener *listener = &db.config->listeners[i];
		எனில்(listener->ssl_ctx && listener->certfile && listener->keyfile){
			முழு rc = net__load_certificates(listener);
			எனில்(rc){
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error when reloading certificate '%s' or key '%s'.",
						listener->certfile, listener->keyfile);
			}
		}
	}
#endif
}


நிலையான முழு listeners__start_single_mqtt(கட்டமைப்பு mosquitto__listener *listener)
{
	கட்டமைப்பு mosquitto__listener_sock *listensock_new;

	எனில்(net__socket_listen(listener)){
		திரும்பு 1;
	}
	g_listensock_மொத்தம் += listener->sock_மொத்தம்;
	listensock_new = mosquitto_realloc(g_listensock, அளவு(கட்டமைப்பு mosquitto__listener_sock)*(size_t)g_listensock_மொத்தம்);
	எனில்(!listensock_new){
		திரும்பு 1;
	}
	g_listensock = listensock_new;

	க்கு(முழு i=0; i<listener->sock_மொத்தம்; i++){
		எனில்(listener->socks[i] == INVALID_SOCKET){
			திரும்பு 1;
		}
		g_listensock[listensock_index].sock = listener->socks[i];
		g_listensock[listensock_index].listener = listener;
#எனில் defined(WITH_EPOLL) || defined(WITH_KQUEUE)
		g_listensock[listensock_index].ident = id_listener;
#endif
		listensock_index++;
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS


வெறுமை listeners__add_websockets(கட்டமைப்பு lws_context *ws_context, mosq_sock_t fd)
{
	கட்டமைப்பு mosquitto__listener *listener = வெற்று;
	கட்டமைப்பு mosquitto__listener_sock *listensock_new;

	/* Don't add more listeners after we've started the main loop */
	எனில்(g_run || ws_context == வெற்று){
		திரும்பு;
	}

	/* Find context */
	க்கு(முழு i=0; i<db.config->listener_மொத்தம்; i++){
		எனில்(db.config->listeners[i].ws_in_init){
			listener = &db.config->listeners[i];
			நிறுத்து;
		}
	}
	எனில்(listener == வெற்று){
		திரும்பு;
	}

	g_listensock_மொத்தம்++;
	listensock_new = mosquitto_realloc(g_listensock, அளவு(கட்டமைப்பு mosquitto__listener_sock)*(size_t)g_listensock_மொத்தம்);
	எனில்(!listensock_new){
		திரும்பு;
	}
	g_listensock = listensock_new;

	g_listensock[listensock_index].sock = fd;
	g_listensock[listensock_index].listener = listener;
#எனில் defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	g_listensock[listensock_index].ident = id_listener_ws;
#endif
	listensock_index++;
}
#endif


நிலையான முழு listeners__add_local(மாறிலி எழுத்து *host, uint16_t port)
{
	கட்டமைப்பு mosquitto__listener *listeners;
	bool allow_anonymous;

	listeners = db.config->listeners;
	எனில்(db.config->security_options.allow_anonymous == -1){
		allow_anonymous = true;
	}இல்லையெனில்{
		allow_anonymous = db.config->security_options.allow_anonymous;
	}

	listeners[db.config->listener_மொத்தம்].security_options = mosquitto_calloc(1, அளவு(கட்டமைப்பு mosquitto__security_options));
	எனில்(listeners[db.config->listener_மொத்தம்].security_options == வெற்று){
		திரும்பு MOSQ_ERR_NOMEM;
	}

	listener__set_defaults(&listeners[db.config->listener_மொத்தம்]);
	listeners[db.config->listener_மொத்தம்].security_options->allow_anonymous = allow_anonymous;
	listeners[db.config->listener_மொத்தம்].security_options->auto_id_prefix = mosquitto_strdup("auto-");
	எனில்(listeners[db.config->listener_மொத்தம்].security_options->auto_id_prefix == வெற்று){
		mosquitto_FREE(listeners[db.config->listener_மொத்தம்].security_options);
		திரும்பு MOSQ_ERR_NOMEM;
	}
	listeners[db.config->listener_மொத்தம்].security_options->auto_id_prefix_len = (முழு)strlen("auto-");
	listeners[db.config->listener_மொத்தம்].port = port;
	listeners[db.config->listener_மொத்தம்].host = mosquitto_strdup(host);
	எனில்(listeners[db.config->listener_மொத்தம்].host == வெற்று){
		mosquitto_FREE(listeners[db.config->listener_மொத்தம்].security_options->auto_id_prefix);
		mosquitto_FREE(listeners[db.config->listener_மொத்தம்].security_options);
		திரும்பு MOSQ_ERR_NOMEM;
	}
	எனில்(listeners__start_single_mqtt(&listeners[db.config->listener_மொத்தம்])){
		mosquitto_FREE(listeners[db.config->listener_மொத்தம்].security_options->auto_id_prefix);
		mosquitto_FREE(listeners[db.config->listener_மொத்தம்].security_options);
		mosquitto_FREE(listeners[db.config->listener_மொத்தம்].host);
		திரும்பு MOSQ_ERR_UNKNOWN;
	}
	db.config->listener_மொத்தம்++;
	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு listeners__start_local_only(வெறுமை)
{
	/* Attempt to open listeners bound to 127.0.0.1 and ::1 only */
	முழு rc;
	கட்டமைப்பு mosquitto__listener *listeners;
	size_t மொத்தம்;

	எனில்(db.config->cmd_port_மொத்தம் == 0){
		மொத்தம் = 2;
	}இல்லையெனில்{
		மொத்தம் = (size_t)(db.config->cmd_port_மொத்தம்*2);
	}

#ifdef WITH_HTTP_API
	மொத்தம்++;
#endif

	listeners = mosquitto_realloc(db.config->listeners, மொத்தம்*அளவு(கட்டமைப்பு mosquitto__listener));
	எனில்(listeners == வெற்று){
		திரும்பு MOSQ_ERR_NOMEM;
	}
	memset(listeners, 0, மொத்தம்*அளவு(கட்டமைப்பு mosquitto__listener));
	db.config->listener_மொத்தம் = 0;
	db.config->listeners = listeners;

	log__printf(வெற்று, MOSQ_LOG_WARNING, "Starting in local only mode. Connections will only be possible from clients running on this machine.");
	log__printf(வெற்று, MOSQ_LOG_WARNING, "Create a configuration file which defines a listener to allow remote access.");
	log__printf(வெற்று, MOSQ_LOG_WARNING, "For more details see https://mosquitto.org/documentation/authentication-methods/");
	எனில்(db.config->cmd_port_மொத்தம் == 0){
		rc = listeners__add_local("127.0.0.1", 1883);
		எனில்(rc == MOSQ_ERR_NOMEM){
			திரும்பு MOSQ_ERR_NOMEM;
		}
		rc = listeners__add_local("::1", 1883);
		எனில்(rc == MOSQ_ERR_NOMEM){
			திரும்பு MOSQ_ERR_NOMEM;
		}
	}இல்லையெனில்{
		க்கு(முழு i=0; i<db.config->cmd_port_மொத்தம்; i++){
			rc = listeners__add_local("127.0.0.1", db.config->cmd_port[i]);
			எனில்(rc == MOSQ_ERR_NOMEM){
				திரும்பு MOSQ_ERR_NOMEM;
			}
			rc = listeners__add_local("::1", db.config->cmd_port[i]);
			எனில்(rc == MOSQ_ERR_NOMEM){
				திரும்பு MOSQ_ERR_NOMEM;
			}
		}
	}

	எனில்(db.config->listener_மொத்தம் > 0){
#ifdef WITH_HTTP_API
		db.config->listener_மொத்தம்++;
		http_api__start_local(&db.config->listeners[db.config->listener_மொத்தம்-1]);
#endif
		திரும்பு MOSQ_ERR_SUCCESS;
	}இல்லையெனில்{
		திரும்பு MOSQ_ERR_UNKNOWN;
	}
}


முழு listeners__start(வெறுமை)
{
	g_listensock_மொத்தம் = 0;

	எனில்(db.config->local_only){
		எனில்(listeners__start_local_only()){
			db__close();
			எனில்(db.config->pid_file){
				(வெறுமை)remove(db.config->pid_file);
			}
			திரும்பு 1;
		}
		mux__add_listeners(g_listensock, g_listensock_மொத்தம்);
		திரும்பு MOSQ_ERR_SUCCESS;
	}

	க்கு(முழு i=0; i<db.config->listener_மொத்தம்; i++){
		எனில்(db.config->listeners[i].protocol == mp_mqtt){
			எனில்(listeners__start_single_mqtt(&db.config->listeners[i])){
				db__close();
				எனில்(db.config->pid_file){
					(வெறுமை)remove(db.config->pid_file);
				}
				திரும்பு 1;
			}
		}இல்லையெனில் எனில்(db.config->listeners[i].protocol == mp_websockets){
#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			mosq_websockets_init(&db.config->listeners[i], db.config);
			எனில்(!db.config->listeners[i].ws_context){
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to create websockets listener on port %d.", db.config->listeners[i].port);
				திரும்பு 1;
			}
#அல்லதெனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			எனில்(listeners__start_single_mqtt(&db.config->listeners[i])){
				db__close();
				எனில்(db.config->pid_file){
					(வெறுமை)remove(db.config->pid_file);
				}
				திரும்பு 1;
			}
#endif
#ifdef WITH_HTTP_API
		}இல்லையெனில் எனில்(db.config->listeners[i].protocol == mp_http_api){
			http_api__start(&db.config->listeners[i]);
#endif
		}
	}
	எனில்(g_listensock == வெற்று){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to start any listening sockets, exiting.");
		திரும்பு 1;
	}

	mux__add_listeners(g_listensock, g_listensock_மொத்தம்);
	திரும்பு MOSQ_ERR_SUCCESS;
}


வெறுமை listeners__stop(வெறுமை)
{
	mux__delete_listeners(g_listensock, g_listensock_மொத்தம்);

	க்கு(முழு i=0; i<db.config->listener_மொத்தம்; i++){
#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		எனில்(db.config->listeners[i].ws_context){
			lws_context_destroy(db.config->listeners[i].ws_context);
		}
		mosquitto_FREE(db.config->listeners[i].ws_protocol);
#endif
#ifdef WITH_UNIX_SOCKETS
		எனில்(db.config->listeners[i].unix_socket_path != வெற்று){
			unlink(db.config->listeners[i].unix_socket_path);
		}
#endif
#ifdef WITH_HTTP_API
		எனில்(db.config->listeners[i].mhd){
			http_api__stop(&db.config->listeners[i]);
		}
#endif
	}

	க்கு(முழு i=0; i<g_listensock_மொத்தம்; i++){
		எனில்(g_listensock[i].sock != INVALID_SOCKET){
			COMPAT_CLOSE(g_listensock[i].sock);
		}
	}
	mosquitto_FREE(g_listensock);
	g_listensock_மொத்தம் = 0;
	listensock_index = 0;
}
