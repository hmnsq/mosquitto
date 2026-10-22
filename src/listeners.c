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

static 整数 listensock_index = 0;
extern 整数 g_run;


void listener__set_defaults(struct mosquitto__listener *listener)
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

#如果 defined(WITH_WEBSOCKETS) && (LWS_LIBRARY_VERSION_NUMBER >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN)
	为(整数 i=0; i<listener->ws_origin_count; i++){
		mosquitto_FREE(listener->ws_origins[i]);
	}
	mosquitto_FREE(listener->ws_origins);
	listener->ws_origin_count = 0;
#endif
}


void listeners__reload_all_certificates(void)
{
#ifdef WITH_TLS
	为(整数 i=0; i<db.config->listener_count; i++){
		struct mosquitto__listener *listener = &db.config->listeners[i];
		如果(listener->ssl_ctx && listener->certfile && listener->keyfile){
			整数 rc = net__load_certificates(listener);
			如果(rc){
				log__printf(NULL, MOSQ_LOG_ERR, "Error when reloading certificate '%s' or key '%s'.",
						listener->certfile, listener->keyfile);
			}
		}
	}
#endif
}


static 整数 listeners__start_single_mqtt(struct mosquitto__listener *listener)
{
	struct mosquitto__listener_sock *listensock_new;

	如果(net__socket_listen(listener)){
		返回 1;
	}
	g_listensock_count += listener->sock_count;
	listensock_new = mosquitto_realloc(g_listensock, sizeof(struct mosquitto__listener_sock)*(size_t)g_listensock_count);
	如果(!listensock_new){
		返回 1;
	}
	g_listensock = listensock_new;

	为(整数 i=0; i<listener->sock_count; i++){
		如果(listener->socks[i] == INVALID_SOCKET){
			返回 1;
		}
		g_listensock[listensock_index].sock = listener->socks[i];
		g_listensock[listensock_index].listener = listener;
#如果 defined(WITH_EPOLL) || defined(WITH_KQUEUE)
		g_listensock[listensock_index].ident = id_listener;
#endif
		listensock_index++;
	}
	返回 MOSQ_ERR_SUCCESS;
}


#如果 defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS


void listeners__add_websockets(struct lws_context *ws_context, mosq_sock_t fd)
{
	struct mosquitto__listener *listener = NULL;
	struct mosquitto__listener_sock *listensock_new;

	/* Don't add more listeners after we've started the main loop */
	如果(g_run || ws_context == NULL){
		返回;
	}

	/* Find context */
	为(整数 i=0; i<db.config->listener_count; i++){
		如果(db.config->listeners[i].ws_in_init){
			listener = &db.config->listeners[i];
			break;
		}
	}
	如果(listener == NULL){
		返回;
	}

	g_listensock_count++;
	listensock_new = mosquitto_realloc(g_listensock, sizeof(struct mosquitto__listener_sock)*(size_t)g_listensock_count);
	如果(!listensock_new){
		返回;
	}
	g_listensock = listensock_new;

	g_listensock[listensock_index].sock = fd;
	g_listensock[listensock_index].listener = listener;
#如果 defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	g_listensock[listensock_index].ident = id_listener_ws;
#endif
	listensock_index++;
}
#endif


static 整数 listeners__add_local(const char *host, uint16_t port)
{
	struct mosquitto__listener *listeners;
	bool allow_anonymous;

	listeners = db.config->listeners;
	如果(db.config->security_options.allow_anonymous == -1){
		allow_anonymous = true;
	}否则{
		allow_anonymous = db.config->security_options.allow_anonymous;
	}

	listeners[db.config->listener_count].security_options = mosquitto_calloc(1, sizeof(struct mosquitto__security_options));
	如果(listeners[db.config->listener_count].security_options == NULL){
		返回 MOSQ_ERR_NOMEM;
	}

	listener__set_defaults(&listeners[db.config->listener_count]);
	listeners[db.config->listener_count].security_options->allow_anonymous = allow_anonymous;
	listeners[db.config->listener_count].security_options->auto_id_prefix = mosquitto_strdup("auto-");
	如果(listeners[db.config->listener_count].security_options->auto_id_prefix == NULL){
		mosquitto_FREE(listeners[db.config->listener_count].security_options);
		返回 MOSQ_ERR_NOMEM;
	}
	listeners[db.config->listener_count].security_options->auto_id_prefix_len = (整数)strlen("auto-");
	listeners[db.config->listener_count].port = port;
	listeners[db.config->listener_count].host = mosquitto_strdup(host);
	如果(listeners[db.config->listener_count].host == NULL){
		mosquitto_FREE(listeners[db.config->listener_count].security_options->auto_id_prefix);
		mosquitto_FREE(listeners[db.config->listener_count].security_options);
		返回 MOSQ_ERR_NOMEM;
	}
	如果(listeners__start_single_mqtt(&listeners[db.config->listener_count])){
		mosquitto_FREE(listeners[db.config->listener_count].security_options->auto_id_prefix);
		mosquitto_FREE(listeners[db.config->listener_count].security_options);
		mosquitto_FREE(listeners[db.config->listener_count].host);
		返回 MOSQ_ERR_UNKNOWN;
	}
	db.config->listener_count++;
	返回 MOSQ_ERR_SUCCESS;
}


static 整数 listeners__start_local_only(void)
{
	/* Attempt to open listeners bound to 127.0.0.1 and ::1 only */
	整数 rc;
	struct mosquitto__listener *listeners;
	size_t count;

	如果(db.config->cmd_port_count == 0){
		count = 2;
	}否则{
		count = (size_t)(db.config->cmd_port_count*2);
	}

#ifdef WITH_HTTP_API
	count++;
#endif

	listeners = mosquitto_realloc(db.config->listeners, count*sizeof(struct mosquitto__listener));
	如果(listeners == NULL){
		返回 MOSQ_ERR_NOMEM;
	}
	memset(listeners, 0, count*sizeof(struct mosquitto__listener));
	db.config->listener_count = 0;
	db.config->listeners = listeners;

	log__printf(NULL, MOSQ_LOG_WARNING, "Starting in local only mode. Connections will only be possible from clients running on this machine.");
	log__printf(NULL, MOSQ_LOG_WARNING, "Create a configuration file which defines a listener to allow remote access.");
	log__printf(NULL, MOSQ_LOG_WARNING, "For more details see https://mosquitto.org/documentation/authentication-methods/");
	如果(db.config->cmd_port_count == 0){
		rc = listeners__add_local("127.0.0.1", 1883);
		如果(rc == MOSQ_ERR_NOMEM){
			返回 MOSQ_ERR_NOMEM;
		}
		rc = listeners__add_local("::1", 1883);
		如果(rc == MOSQ_ERR_NOMEM){
			返回 MOSQ_ERR_NOMEM;
		}
	}否则{
		为(整数 i=0; i<db.config->cmd_port_count; i++){
			rc = listeners__add_local("127.0.0.1", db.config->cmd_port[i]);
			如果(rc == MOSQ_ERR_NOMEM){
				返回 MOSQ_ERR_NOMEM;
			}
			rc = listeners__add_local("::1", db.config->cmd_port[i]);
			如果(rc == MOSQ_ERR_NOMEM){
				返回 MOSQ_ERR_NOMEM;
			}
		}
	}

	如果(db.config->listener_count > 0){
#ifdef WITH_HTTP_API
		db.config->listener_count++;
		http_api__start_local(&db.config->listeners[db.config->listener_count-1]);
#endif
		返回 MOSQ_ERR_SUCCESS;
	}否则{
		返回 MOSQ_ERR_UNKNOWN;
	}
}


整数 listeners__start(void)
{
	g_listensock_count = 0;

	如果(db.config->local_only){
		如果(listeners__start_local_only()){
			db__close();
			如果(db.config->pid_file){
				(void)remove(db.config->pid_file);
			}
			返回 1;
		}
		mux__add_listeners(g_listensock, g_listensock_count);
		返回 MOSQ_ERR_SUCCESS;
	}

	为(整数 i=0; i<db.config->listener_count; i++){
		如果(db.config->listeners[i].protocol == mp_mqtt){
			如果(listeners__start_single_mqtt(&db.config->listeners[i])){
				db__close();
				如果(db.config->pid_file){
					(void)remove(db.config->pid_file);
				}
				返回 1;
			}
		}否则 如果(db.config->listeners[i].protocol == mp_websockets){
#如果 defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			mosq_websockets_init(&db.config->listeners[i], db.config);
			如果(!db.config->listeners[i].ws_context){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to create websockets listener on port %d.", db.config->listeners[i].port);
				返回 1;
			}
#elif defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			如果(listeners__start_single_mqtt(&db.config->listeners[i])){
				db__close();
				如果(db.config->pid_file){
					(void)remove(db.config->pid_file);
				}
				返回 1;
			}
#endif
#ifdef WITH_HTTP_API
		}否则 如果(db.config->listeners[i].protocol == mp_http_api){
			http_api__start(&db.config->listeners[i]);
#endif
		}
	}
	如果(g_listensock == NULL){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to start any listening sockets, exiting.");
		返回 1;
	}

	mux__add_listeners(g_listensock, g_listensock_count);
	返回 MOSQ_ERR_SUCCESS;
}


void listeners__stop(void)
{
	mux__delete_listeners(g_listensock, g_listensock_count);

	为(整数 i=0; i<db.config->listener_count; i++){
#如果 defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		如果(db.config->listeners[i].ws_context){
			lws_context_destroy(db.config->listeners[i].ws_context);
		}
		mosquitto_FREE(db.config->listeners[i].ws_protocol);
#endif
#ifdef WITH_UNIX_SOCKETS
		如果(db.config->listeners[i].unix_socket_path != NULL){
			unlink(db.config->listeners[i].unix_socket_path);
		}
#endif
#ifdef WITH_HTTP_API
		如果(db.config->listeners[i].mhd){
			http_api__stop(&db.config->listeners[i]);
		}
#endif
	}

	为(整数 i=0; i<g_listensock_count; i++){
		如果(g_listensock[i].sock != INVALID_SOCKET){
			COMPAT_CLOSE(g_listensock[i].sock);
		}
	}
	mosquitto_FREE(g_listensock);
	g_listensock_count = 0;
	listensock_index = 0;
}
