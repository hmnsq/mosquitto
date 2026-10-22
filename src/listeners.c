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

#ضمن "config.h"
#ضمن "net_mosq.h"
#ضمن "mosquitto_broker_internal.h"

ساكن صحيح listensock_index = 0;
خارجي صحيح g_run;


فراغ listener__set_defaults(بنية mosquitto__listener *listener)
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

#إذا defined(WITH_WEBSOCKETS) && (LWS_LIBRARY_VERSION_العدد >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN)
	لكل(صحيح i=0; i<listener->ws_origin_تعداد; i++){
		mosquitto_FREE(listener->ws_origins[i]);
	}
	mosquitto_FREE(listener->ws_origins);
	listener->ws_origin_تعداد = 0;
#endif
}


فراغ listeners__reload_all_certificates(فراغ)
{
#ifdef WITH_TLS
	لكل(صحيح i=0; i<db.config->listener_تعداد; i++){
		بنية mosquitto__listener *listener = &db.config->listeners[i];
		إذا(listener->ssl_ctx && listener->certfile && listener->keyfile){
			صحيح rc = net__load_certificates(listener);
			إذا(rc){
				log__printf(عدم, MOSQ_LOG_ERR, "Error when reloading certificate '%s' or key '%s'.",
						listener->certfile, listener->keyfile);
			}
		}
	}
#endif
}


ساكن صحيح listeners__start_single_mqtt(بنية mosquitto__listener *listener)
{
	بنية mosquitto__listener_sock *listensock_new;

	إذا(net__socket_listen(listener)){
		إرجاع 1;
	}
	g_listensock_تعداد += listener->sock_تعداد;
	listensock_new = mosquitto_realloc(g_listensock, حجم(بنية mosquitto__listener_sock)*(size_t)g_listensock_تعداد);
	إذا(!listensock_new){
		إرجاع 1;
	}
	g_listensock = listensock_new;

	لكل(صحيح i=0; i<listener->sock_تعداد; i++){
		إذا(listener->socks[i] == INVALID_SOCKET){
			إرجاع 1;
		}
		g_listensock[listensock_index].sock = listener->socks[i];
		g_listensock[listensock_index].listener = listener;
#إذا defined(WITH_EPOLL) || defined(WITH_KQUEUE)
		g_listensock[listensock_index].ident = id_listener;
#endif
		listensock_index++;
	}
	إرجاع MOSQ_ERR_SUCCESS;
}


#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS


فراغ listeners__add_websockets(بنية lws_context *ws_context, mosq_sock_t fd)
{
	بنية mosquitto__listener *listener = عدم;
	بنية mosquitto__listener_sock *listensock_new;

	/* Don't add more listeners after we've started the main loop */
	إذا(g_run || ws_context == عدم){
		إرجاع;
	}

	/* Find context */
	لكل(صحيح i=0; i<db.config->listener_تعداد; i++){
		إذا(db.config->listeners[i].ws_in_init){
			listener = &db.config->listeners[i];
			اكسر;
		}
	}
	إذا(listener == عدم){
		إرجاع;
	}

	g_listensock_تعداد++;
	listensock_new = mosquitto_realloc(g_listensock, حجم(بنية mosquitto__listener_sock)*(size_t)g_listensock_تعداد);
	إذا(!listensock_new){
		إرجاع;
	}
	g_listensock = listensock_new;

	g_listensock[listensock_index].sock = fd;
	g_listensock[listensock_index].listener = listener;
#إذا defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	g_listensock[listensock_index].ident = id_listener_ws;
#endif
	listensock_index++;
}
#endif


ساكن صحيح listeners__add_local(ثابت حرف *host, uint16_t port)
{
	بنية mosquitto__listener *listeners;
	bool allow_anonymous;

	listeners = db.config->listeners;
	إذا(db.config->security_options.allow_anonymous == -1){
		allow_anonymous = true;
	}وإلا{
		allow_anonymous = db.config->security_options.allow_anonymous;
	}

	listeners[db.config->listener_تعداد].security_options = mosquitto_calloc(1, حجم(بنية mosquitto__security_options));
	إذا(listeners[db.config->listener_تعداد].security_options == عدم){
		إرجاع MOSQ_ERR_NOMEM;
	}

	listener__set_defaults(&listeners[db.config->listener_تعداد]);
	listeners[db.config->listener_تعداد].security_options->allow_anonymous = allow_anonymous;
	listeners[db.config->listener_تعداد].security_options->auto_id_prefix = mosquitto_strdup("auto-");
	إذا(listeners[db.config->listener_تعداد].security_options->auto_id_prefix == عدم){
		mosquitto_FREE(listeners[db.config->listener_تعداد].security_options);
		إرجاع MOSQ_ERR_NOMEM;
	}
	listeners[db.config->listener_تعداد].security_options->auto_id_prefix_len = (صحيح)strlen("auto-");
	listeners[db.config->listener_تعداد].port = port;
	listeners[db.config->listener_تعداد].host = mosquitto_strdup(host);
	إذا(listeners[db.config->listener_تعداد].host == عدم){
		mosquitto_FREE(listeners[db.config->listener_تعداد].security_options->auto_id_prefix);
		mosquitto_FREE(listeners[db.config->listener_تعداد].security_options);
		إرجاع MOSQ_ERR_NOMEM;
	}
	إذا(listeners__start_single_mqtt(&listeners[db.config->listener_تعداد])){
		mosquitto_FREE(listeners[db.config->listener_تعداد].security_options->auto_id_prefix);
		mosquitto_FREE(listeners[db.config->listener_تعداد].security_options);
		mosquitto_FREE(listeners[db.config->listener_تعداد].host);
		إرجاع MOSQ_ERR_UNKNOWN;
	}
	db.config->listener_تعداد++;
	إرجاع MOSQ_ERR_SUCCESS;
}


ساكن صحيح listeners__start_local_only(فراغ)
{
	/* Attempt to open listeners bound to 127.0.0.1 and ::1 only */
	صحيح rc;
	بنية mosquitto__listener *listeners;
	size_t تعداد_2;

	إذا(db.config->cmd_port_تعداد == 0){
		تعداد_2 = 2;
	}وإلا{
		تعداد_2 = (size_t)(db.config->cmd_port_تعداد*2);
	}

#ifdef WITH_HTTP_API
	تعداد_2++;
#endif

	listeners = mosquitto_realloc(db.config->listeners, تعداد_2*حجم(بنية mosquitto__listener));
	إذا(listeners == عدم){
		إرجاع MOSQ_ERR_NOMEM;
	}
	memset(listeners, 0, تعداد_2*حجم(بنية mosquitto__listener));
	db.config->listener_تعداد = 0;
	db.config->listeners = listeners;

	log__printf(عدم, MOSQ_LOG_WARNING, "Starting in local only mode. Connections will only be possible from clients running on this machine.");
	log__printf(عدم, MOSQ_LOG_WARNING, "Create a configuration file which defines a listener to allow remote access.");
	log__printf(عدم, MOSQ_LOG_WARNING, "For more details see https://mosquitto.org/documentation/authentication-methods/");
	إذا(db.config->cmd_port_تعداد == 0){
		rc = listeners__add_local("127.0.0.1", 1883);
		إذا(rc == MOSQ_ERR_NOMEM){
			إرجاع MOSQ_ERR_NOMEM;
		}
		rc = listeners__add_local("::1", 1883);
		إذا(rc == MOSQ_ERR_NOMEM){
			إرجاع MOSQ_ERR_NOMEM;
		}
	}وإلا{
		لكل(صحيح i=0; i<db.config->cmd_port_تعداد; i++){
			rc = listeners__add_local("127.0.0.1", db.config->cmd_port[i]);
			إذا(rc == MOSQ_ERR_NOMEM){
				إرجاع MOSQ_ERR_NOMEM;
			}
			rc = listeners__add_local("::1", db.config->cmd_port[i]);
			إذا(rc == MOSQ_ERR_NOMEM){
				إرجاع MOSQ_ERR_NOMEM;
			}
		}
	}

	إذا(db.config->listener_تعداد > 0){
#ifdef WITH_HTTP_API
		db.config->listener_تعداد++;
		http_api__start_local(&db.config->listeners[db.config->listener_تعداد-1]);
#endif
		إرجاع MOSQ_ERR_SUCCESS;
	}وإلا{
		إرجاع MOSQ_ERR_UNKNOWN;
	}
}


صحيح listeners__start(فراغ)
{
	g_listensock_تعداد = 0;

	إذا(db.config->local_only){
		إذا(listeners__start_local_only()){
			db__close();
			إذا(db.config->pid_file){
				(فراغ)remove(db.config->pid_file);
			}
			إرجاع 1;
		}
		mux__add_listeners(g_listensock, g_listensock_تعداد);
		إرجاع MOSQ_ERR_SUCCESS;
	}

	لكل(صحيح i=0; i<db.config->listener_تعداد; i++){
		إذا(db.config->listeners[i].protocol == mp_mqtt){
			إذا(listeners__start_single_mqtt(&db.config->listeners[i])){
				db__close();
				إذا(db.config->pid_file){
					(فراغ)remove(db.config->pid_file);
				}
				إرجاع 1;
			}
		}وإلا إذا(db.config->listeners[i].protocol == mp_websockets){
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			mosq_websockets_init(&db.config->listeners[i], db.config);
			إذا(!db.config->listeners[i].ws_context){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to create websockets listener on port %d.", db.config->listeners[i].port);
				إرجاع 1;
			}
#وإلاإذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			إذا(listeners__start_single_mqtt(&db.config->listeners[i])){
				db__close();
				إذا(db.config->pid_file){
					(فراغ)remove(db.config->pid_file);
				}
				إرجاع 1;
			}
#endif
#ifdef WITH_HTTP_API
		}وإلا إذا(db.config->listeners[i].protocol == mp_http_api){
			http_api__start(&db.config->listeners[i]);
#endif
		}
	}
	إذا(g_listensock == عدم){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to start any listening sockets, exiting.");
		إرجاع 1;
	}

	mux__add_listeners(g_listensock, g_listensock_تعداد);
	إرجاع MOSQ_ERR_SUCCESS;
}


فراغ listeners__stop(فراغ)
{
	mux__delete_listeners(g_listensock, g_listensock_تعداد);

	لكل(صحيح i=0; i<db.config->listener_تعداد; i++){
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		إذا(db.config->listeners[i].ws_context){
			lws_context_destroy(db.config->listeners[i].ws_context);
		}
		mosquitto_FREE(db.config->listeners[i].ws_protocol);
#endif
#ifdef WITH_UNIX_SOCKETS
		إذا(db.config->listeners[i].unix_socket_path != عدم){
			unlink(db.config->listeners[i].unix_socket_path);
		}
#endif
#ifdef WITH_HTTP_API
		إذا(db.config->listeners[i].mhd){
			http_api__stop(&db.config->listeners[i]);
		}
#endif
	}

	لكل(صحيح i=0; i<g_listensock_تعداد; i++){
		إذا(g_listensock[i].sock != INVALID_SOCKET){
			COMPAT_CLOSE(g_listensock[i].sock);
		}
	}
	mosquitto_FREE(g_listensock);
	g_listensock_تعداد = 0;
	listensock_index = 0;
}
