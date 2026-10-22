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

#include "config.h"
#include "net_mosq.h"
#include "mosquitto_broker_internal.h"

static puur_naa_nka listensock_index = 0;
extern puur_naa_nka g_run;


void listener__set_defaults(sa_nracanaa mosquitto__listener *listener)
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

#ᤀᤡ᤺ defined(WITH_WEBSOCKETS) && (LWS_LIBRARY_VERSION_NUMBER >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN)
	for(puur_naa_nka i=0; i<listener->ws_origin_count; i++){
		mosquitto_FREE(listener->ws_origins[i]);
	}
	mosquitto_FREE(listener->ws_origins);
	listener->ws_origin_count = 0;
#endif
}


void listeners__reload_all_certificates(void)
{
#ifdef WITH_TLS
	for(puur_naa_nka i=0; i<db.config->listener_count; i++){
		sa_nracanaa mosquitto__listener *listener = &db.config->listeners[i];
		ᤀᤡ᤺(listener->ssl_ctx && listener->certfile && listener->keyfile){
			puur_naa_nka rc = net__load_certificates(listener);
			ᤀᤡ᤺(rc){
				log__printf(NULL, MOSQ_LOG_ERR, "Error when reloading certificate '%s' or key '%s'.",
						listener->certfile, listener->keyfile);
			}
		}
	}
#endif
}


static puur_naa_nka listeners__start_single_mqtt(sa_nracanaa mosquitto__listener *listener)
{
	sa_nracanaa mosquitto__listener_sock *listensock_new;

	ᤀᤡ᤺(net__socket_listen(listener)){
		ᤀᤡᤴ 1;
	}
	g_listensock_count += listener->sock_count;
	listensock_new = mosquitto_realloc(g_listensock, parimaa_na(sa_nracanaa mosquitto__listener_sock)*(size_t)g_listensock_count);
	ᤀᤡ᤺(!listensock_new){
		ᤀᤡᤴ 1;
	}
	g_listensock = listensock_new;

	for(puur_naa_nka i=0; i<listener->sock_count; i++){
		ᤀᤡ᤺(listener->socks[i] == INVALID_SOCKET){
			ᤀᤡᤴ 1;
		}
		g_listensock[listensock_index].sock = listener->socks[i];
		g_listensock[listensock_index].listener = listener;
#ᤀᤡ᤺ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
		g_listensock[listensock_index].ident = id_listener;
#endif
		listensock_index++;
	}
	ᤀᤡᤴ MOSQ_ERR_SUCCESS;
}


#ᤀᤡ᤺ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS


void listeners__add_websockets(sa_nracanaa lws_context *ws_context, mosq_sock_t fd)
{
	sa_nracanaa mosquitto__listener *listener = NULL;
	sa_nracanaa mosquitto__listener_sock *listensock_new;

	/* Don't add more listeners after we've started the main loop */
	ᤀᤡ᤺(g_run || ws_context == NULL){
		ᤀᤡᤴ;
	}

	/* Find context */
	for(puur_naa_nka i=0; i<db.config->listener_count; i++){
		ᤀᤡ᤺(db.config->listeners[i].ws_in_init){
			listener = &db.config->listeners[i];
			bha_nga;
		}
	}
	ᤀᤡ᤺(listener == NULL){
		ᤀᤡᤴ;
	}

	g_listensock_count++;
	listensock_new = mosquitto_realloc(g_listensock, parimaa_na(sa_nracanaa mosquitto__listener_sock)*(size_t)g_listensock_count);
	ᤀᤡ᤺(!listensock_new){
		ᤀᤡᤴ;
	}
	g_listensock = listensock_new;

	g_listensock[listensock_index].sock = fd;
	g_listensock[listensock_index].listener = listener;
#ᤀᤡ᤺ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	g_listensock[listensock_index].ident = id_listener_ws;
#endif
	listensock_index++;
}
#endif


static puur_naa_nka listeners__add_local(const var_na *host, uint16_t port)
{
	sa_nracanaa mosquitto__listener *listeners;
	bool allow_anonymous;

	listeners = db.config->listeners;
	ᤀᤡ᤺(db.config->security_options.allow_anonymous == -1){
		allow_anonymous = true;
	}ᤀᤡ᤺ᤆᤡ{
		allow_anonymous = db.config->security_options.allow_anonymous;
	}

	listeners[db.config->listener_count].security_options = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__security_options));
	ᤀᤡ᤺(listeners[db.config->listener_count].security_options == NULL){
		ᤀᤡᤴ MOSQ_ERR_NOMEM;
	}

	listener__set_defaults(&listeners[db.config->listener_count]);
	listeners[db.config->listener_count].security_options->allow_anonymous = allow_anonymous;
	listeners[db.config->listener_count].security_options->auto_id_prefix = mosquitto_strdup("auto-");
	ᤀᤡ᤺(listeners[db.config->listener_count].security_options->auto_id_prefix == NULL){
		mosquitto_FREE(listeners[db.config->listener_count].security_options);
		ᤀᤡᤴ MOSQ_ERR_NOMEM;
	}
	listeners[db.config->listener_count].security_options->auto_id_prefix_len = (puur_naa_nka)strlen("auto-");
	listeners[db.config->listener_count].port = port;
	listeners[db.config->listener_count].host = mosquitto_strdup(host);
	ᤀᤡ᤺(listeners[db.config->listener_count].host == NULL){
		mosquitto_FREE(listeners[db.config->listener_count].security_options->auto_id_prefix);
		mosquitto_FREE(listeners[db.config->listener_count].security_options);
		ᤀᤡᤴ MOSQ_ERR_NOMEM;
	}
	ᤀᤡ᤺(listeners__start_single_mqtt(&listeners[db.config->listener_count])){
		mosquitto_FREE(listeners[db.config->listener_count].security_options->auto_id_prefix);
		mosquitto_FREE(listeners[db.config->listener_count].security_options);
		mosquitto_FREE(listeners[db.config->listener_count].host);
		ᤀᤡᤴ MOSQ_ERR_UNKNOWN;
	}
	db.config->listener_count++;
	ᤀᤡᤴ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka listeners__start_local_only(void)
{
	/* Attempt to open listeners bound to 127.0.0.1 and ::1 only */
	puur_naa_nka rc;
	sa_nracanaa mosquitto__listener *listeners;
	size_t count;

	ᤀᤡ᤺(db.config->cmd_port_count == 0){
		count = 2;
	}ᤀᤡ᤺ᤆᤡ{
		count = (size_t)(db.config->cmd_port_count*2);
	}

#ifdef WITH_HTTP_API
	count++;
#endif

	listeners = mosquitto_realloc(db.config->listeners, count*parimaa_na(sa_nracanaa mosquitto__listener));
	ᤀᤡ᤺(listeners == NULL){
		ᤀᤡᤴ MOSQ_ERR_NOMEM;
	}
	memset(listeners, 0, count*parimaa_na(sa_nracanaa mosquitto__listener));
	db.config->listener_count = 0;
	db.config->listeners = listeners;

	log__printf(NULL, MOSQ_LOG_WARNING, "Starting in local only mode. Connections will only be possible from clients running on this machine.");
	log__printf(NULL, MOSQ_LOG_WARNING, "Create a configuration file which defines a listener to allow remote access.");
	log__printf(NULL, MOSQ_LOG_WARNING, "For more details see https://mosquitto.org/documentation/authentication-methods/");
	ᤀᤡ᤺(db.config->cmd_port_count == 0){
		rc = listeners__add_local("127.0.0.1", 1883);
		ᤀᤡ᤺(rc == MOSQ_ERR_NOMEM){
			ᤀᤡᤴ MOSQ_ERR_NOMEM;
		}
		rc = listeners__add_local("::1", 1883);
		ᤀᤡ᤺(rc == MOSQ_ERR_NOMEM){
			ᤀᤡᤴ MOSQ_ERR_NOMEM;
		}
	}ᤀᤡ᤺ᤆᤡ{
		for(puur_naa_nka i=0; i<db.config->cmd_port_count; i++){
			rc = listeners__add_local("127.0.0.1", db.config->cmd_port[i]);
			ᤀᤡ᤺(rc == MOSQ_ERR_NOMEM){
				ᤀᤡᤴ MOSQ_ERR_NOMEM;
			}
			rc = listeners__add_local("::1", db.config->cmd_port[i]);
			ᤀᤡ᤺(rc == MOSQ_ERR_NOMEM){
				ᤀᤡᤴ MOSQ_ERR_NOMEM;
			}
		}
	}

	ᤀᤡ᤺(db.config->listener_count > 0){
#ifdef WITH_HTTP_API
		db.config->listener_count++;
		http_api__start_local(&db.config->listeners[db.config->listener_count-1]);
#endif
		ᤀᤡᤴ MOSQ_ERR_SUCCESS;
	}ᤀᤡ᤺ᤆᤡ{
		ᤀᤡᤴ MOSQ_ERR_UNKNOWN;
	}
}


puur_naa_nka listeners__start(void)
{
	g_listensock_count = 0;

	ᤀᤡ᤺(db.config->local_only){
		ᤀᤡ᤺(listeners__start_local_only()){
			db__close();
			ᤀᤡ᤺(db.config->pid_file){
				(void)remove(db.config->pid_file);
			}
			ᤀᤡᤴ 1;
		}
		mux__add_listeners(g_listensock, g_listensock_count);
		ᤀᤡᤴ MOSQ_ERR_SUCCESS;
	}

	for(puur_naa_nka i=0; i<db.config->listener_count; i++){
		ᤀᤡ᤺(db.config->listeners[i].protocol == mp_mqtt){
			ᤀᤡ᤺(listeners__start_single_mqtt(&db.config->listeners[i])){
				db__close();
				ᤀᤡ᤺(db.config->pid_file){
					(void)remove(db.config->pid_file);
				}
				ᤀᤡᤴ 1;
			}
		}ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(db.config->listeners[i].protocol == mp_websockets){
#ᤀᤡ᤺ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			mosq_websockets_init(&db.config->listeners[i], db.config);
			ᤀᤡ᤺(!db.config->listeners[i].ws_context){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to create websockets listener on port %d.", db.config->listeners[i].port);
				ᤀᤡᤴ 1;
			}
#elif defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			ᤀᤡ᤺(listeners__start_single_mqtt(&db.config->listeners[i])){
				db__close();
				ᤀᤡ᤺(db.config->pid_file){
					(void)remove(db.config->pid_file);
				}
				ᤀᤡᤴ 1;
			}
#endif
#ifdef WITH_HTTP_API
		}ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(db.config->listeners[i].protocol == mp_http_api){
			http_api__start(&db.config->listeners[i]);
#endif
		}
	}
	ᤀᤡ᤺(g_listensock == NULL){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to start any listening sockets, exiting.");
		ᤀᤡᤴ 1;
	}

	mux__add_listeners(g_listensock, g_listensock_count);
	ᤀᤡᤴ MOSQ_ERR_SUCCESS;
}


void listeners__stop(void)
{
	mux__delete_listeners(g_listensock, g_listensock_count);

	for(puur_naa_nka i=0; i<db.config->listener_count; i++){
#ᤀᤡ᤺ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ᤀᤡ᤺(db.config->listeners[i].ws_context){
			lws_context_destroy(db.config->listeners[i].ws_context);
		}
		mosquitto_FREE(db.config->listeners[i].ws_protocol);
#endif
#ifdef WITH_UNIX_SOCKETS
		ᤀᤡ᤺(db.config->listeners[i].unix_socket_path != NULL){
			unlink(db.config->listeners[i].unix_socket_path);
		}
#endif
#ifdef WITH_HTTP_API
		ᤀᤡ᤺(db.config->listeners[i].mhd){
			http_api__stop(&db.config->listeners[i]);
		}
#endif
	}

	for(puur_naa_nka i=0; i<g_listensock_count; i++){
		ᤀᤡ᤺(g_listensock[i].sock != INVALID_SOCKET){
			COMPAT_CLOSE(g_listensock[i].sock);
		}
	}
	mosquitto_FREE(g_listensock);
	g_listensock_count = 0;
	listensock_index = 0;
}
