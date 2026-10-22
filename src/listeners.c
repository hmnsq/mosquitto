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

#અન્તર્ભાવ "config.h"
#અન્તર્ભાવ "net_mosq.h"
#અન્તર્ભાવ "mosquitto_broker_internal.h"

સ્થિર પૂર્ણાંક listensock_index = 0;
બાહ્ય પૂર્ણાંક g_run;


શૂન્ય listener__set_defaults(સંરચના mosquitto__listener *listener)
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

#જો defined(WITH_WEBSOCKETS) && (LWS_LIBRARY_VERSION_સંખ્યા >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN)
	માટે(પૂર્ણાંક i=0; i<listener->ws_origin_ગણતરી; i++){
		mosquitto_FREE(listener->ws_origins[i]);
	}
	mosquitto_FREE(listener->ws_origins);
	listener->ws_origin_ગણતરી = 0;
#endif
}


શૂન્ય listeners__reload_all_certificates(શૂન્ય)
{
#ifdef WITH_TLS
	માટે(પૂર્ણાંક i=0; i<db.config->listener_ગણતરી; i++){
		સંરચના mosquitto__listener *listener = &db.config->listeners[i];
		જો(listener->ssl_ctx && listener->certfile && listener->keyfile){
			પૂર્ણાંક rc = net__load_certificates(listener);
			જો(rc){
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error when reloading certificate '%s' or key '%s'.",
						listener->certfile, listener->keyfile);
			}
		}
	}
#endif
}


સ્થિર પૂર્ણાંક listeners__start_single_mqtt(સંરચના mosquitto__listener *listener)
{
	સંરચના mosquitto__listener_sock *listensock_new;

	જો(net__socket_listen(listener)){
		પાછા 1;
	}
	g_listensock_ગણતરી += listener->sock_ગણતરી;
	listensock_new = mosquitto_realloc(g_listensock, કદ(સંરચના mosquitto__listener_sock)*(size_t)g_listensock_ગણતરી);
	જો(!listensock_new){
		પાછા 1;
	}
	g_listensock = listensock_new;

	માટે(પૂર્ણાંક i=0; i<listener->sock_ગણતરી; i++){
		જો(listener->socks[i] == INVALID_SOCKET){
			પાછા 1;
		}
		g_listensock[listensock_index].sock = listener->socks[i];
		g_listensock[listensock_index].listener = listener;
#જો defined(WITH_EPOLL) || defined(WITH_KQUEUE)
		g_listensock[listensock_index].ident = id_listener;
#endif
		listensock_index++;
	}
	પાછા MOSQ_ERR_SUCCESS;
}


#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS


શૂન્ય listeners__add_websockets(સંરચના lws_context *ws_context, mosq_sock_t fd)
{
	સંરચના mosquitto__listener *listener = ખાલી;
	સંરચના mosquitto__listener_sock *listensock_new;

	/* Don't add more listeners after we've started the main loop */
	જો(g_run || ws_context == ખાલી){
		પાછા;
	}

	/* Find context */
	માટે(પૂર્ણાંક i=0; i<db.config->listener_ગણતરી; i++){
		જો(db.config->listeners[i].ws_in_init){
			listener = &db.config->listeners[i];
			તોડો;
		}
	}
	જો(listener == ખાલી){
		પાછા;
	}

	g_listensock_ગણતરી++;
	listensock_new = mosquitto_realloc(g_listensock, કદ(સંરચના mosquitto__listener_sock)*(size_t)g_listensock_ગણતરી);
	જો(!listensock_new){
		પાછા;
	}
	g_listensock = listensock_new;

	g_listensock[listensock_index].sock = fd;
	g_listensock[listensock_index].listener = listener;
#જો defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	g_listensock[listensock_index].ident = id_listener_ws;
#endif
	listensock_index++;
}
#endif


સ્થિર પૂર્ણાંક listeners__add_local(અચલ અક્ષર *host, uint16_t port)
{
	સંરચના mosquitto__listener *listeners;
	bool allow_anonymous;

	listeners = db.config->listeners;
	જો(db.config->security_options.allow_anonymous == -1){
		allow_anonymous = true;
	}નહીંતર{
		allow_anonymous = db.config->security_options.allow_anonymous;
	}

	listeners[db.config->listener_ગણતરી].security_options = mosquitto_calloc(1, કદ(સંરચના mosquitto__security_options));
	જો(listeners[db.config->listener_ગણતરી].security_options == ખાલી){
		પાછા MOSQ_ERR_NOMEM;
	}

	listener__set_defaults(&listeners[db.config->listener_ગણતરી]);
	listeners[db.config->listener_ગણતરી].security_options->allow_anonymous = allow_anonymous;
	listeners[db.config->listener_ગણતરી].security_options->auto_id_prefix = mosquitto_strdup("auto-");
	જો(listeners[db.config->listener_ગણતરી].security_options->auto_id_prefix == ખાલી){
		mosquitto_FREE(listeners[db.config->listener_ગણતરી].security_options);
		પાછા MOSQ_ERR_NOMEM;
	}
	listeners[db.config->listener_ગણતરી].security_options->auto_id_prefix_len = (પૂર્ણાંક)strlen("auto-");
	listeners[db.config->listener_ગણતરી].port = port;
	listeners[db.config->listener_ગણતરી].host = mosquitto_strdup(host);
	જો(listeners[db.config->listener_ગણતરી].host == ખાલી){
		mosquitto_FREE(listeners[db.config->listener_ગણતરી].security_options->auto_id_prefix);
		mosquitto_FREE(listeners[db.config->listener_ગણતરી].security_options);
		પાછા MOSQ_ERR_NOMEM;
	}
	જો(listeners__start_single_mqtt(&listeners[db.config->listener_ગણતરી])){
		mosquitto_FREE(listeners[db.config->listener_ગણતરી].security_options->auto_id_prefix);
		mosquitto_FREE(listeners[db.config->listener_ગણતરી].security_options);
		mosquitto_FREE(listeners[db.config->listener_ગણતરી].host);
		પાછા MOSQ_ERR_UNKNOWN;
	}
	db.config->listener_ગણતરી++;
	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક listeners__start_local_only(શૂન્ય)
{
	/* Attempt to open listeners bound to 127.0.0.1 and ::1 only */
	પૂર્ણાંક rc;
	સંરચના mosquitto__listener *listeners;
	size_t ગણતરી;

	જો(db.config->cmd_port_ગણતરી == 0){
		ગણતરી = 2;
	}નહીંતર{
		ગણતરી = (size_t)(db.config->cmd_port_ગણતરી*2);
	}

#ifdef WITH_HTTP_API
	ગણતરી++;
#endif

	listeners = mosquitto_realloc(db.config->listeners, ગણતરી*કદ(સંરચના mosquitto__listener));
	જો(listeners == ખાલી){
		પાછા MOSQ_ERR_NOMEM;
	}
	memset(listeners, 0, ગણતરી*કદ(સંરચના mosquitto__listener));
	db.config->listener_ગણતરી = 0;
	db.config->listeners = listeners;

	log__printf(ખાલી, MOSQ_LOG_WARNING, "Starting in local only mode. Connections will only be possible from clients running on this machine.");
	log__printf(ખાલી, MOSQ_LOG_WARNING, "Create a configuration file which defines a listener to allow remote access.");
	log__printf(ખાલી, MOSQ_LOG_WARNING, "For more details see https://mosquitto.org/documentation/authentication-methods/");
	જો(db.config->cmd_port_ગણતરી == 0){
		rc = listeners__add_local("127.0.0.1", 1883);
		જો(rc == MOSQ_ERR_NOMEM){
			પાછા MOSQ_ERR_NOMEM;
		}
		rc = listeners__add_local("::1", 1883);
		જો(rc == MOSQ_ERR_NOMEM){
			પાછા MOSQ_ERR_NOMEM;
		}
	}નહીંતર{
		માટે(પૂર્ણાંક i=0; i<db.config->cmd_port_ગણતરી; i++){
			rc = listeners__add_local("127.0.0.1", db.config->cmd_port[i]);
			જો(rc == MOSQ_ERR_NOMEM){
				પાછા MOSQ_ERR_NOMEM;
			}
			rc = listeners__add_local("::1", db.config->cmd_port[i]);
			જો(rc == MOSQ_ERR_NOMEM){
				પાછા MOSQ_ERR_NOMEM;
			}
		}
	}

	જો(db.config->listener_ગણતરી > 0){
#ifdef WITH_HTTP_API
		db.config->listener_ગણતરી++;
		http_api__start_local(&db.config->listeners[db.config->listener_ગણતરી-1]);
#endif
		પાછા MOSQ_ERR_SUCCESS;
	}નહીંતર{
		પાછા MOSQ_ERR_UNKNOWN;
	}
}


પૂર્ણાંક listeners__start(શૂન્ય)
{
	g_listensock_ગણતરી = 0;

	જો(db.config->local_only){
		જો(listeners__start_local_only()){
			db__close();
			જો(db.config->pid_file){
				(શૂન્ય)remove(db.config->pid_file);
			}
			પાછા 1;
		}
		mux__add_listeners(g_listensock, g_listensock_ગણતરી);
		પાછા MOSQ_ERR_SUCCESS;
	}

	માટે(પૂર્ણાંક i=0; i<db.config->listener_ગણતરી; i++){
		જો(db.config->listeners[i].protocol == mp_mqtt){
			જો(listeners__start_single_mqtt(&db.config->listeners[i])){
				db__close();
				જો(db.config->pid_file){
					(શૂન્ય)remove(db.config->pid_file);
				}
				પાછા 1;
			}
		}નહીંતર જો(db.config->listeners[i].protocol == mp_websockets){
#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			mosq_websockets_init(&db.config->listeners[i], db.config);
			જો(!db.config->listeners[i].ws_context){
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to create websockets listener on port %d.", db.config->listeners[i].port);
				પાછા 1;
			}
#અથવાજો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			જો(listeners__start_single_mqtt(&db.config->listeners[i])){
				db__close();
				જો(db.config->pid_file){
					(શૂન્ય)remove(db.config->pid_file);
				}
				પાછા 1;
			}
#endif
#ifdef WITH_HTTP_API
		}નહીંતર જો(db.config->listeners[i].protocol == mp_http_api){
			http_api__start(&db.config->listeners[i]);
#endif
		}
	}
	જો(g_listensock == ખાલી){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to start any listening sockets, exiting.");
		પાછા 1;
	}

	mux__add_listeners(g_listensock, g_listensock_ગણતરી);
	પાછા MOSQ_ERR_SUCCESS;
}


શૂન્ય listeners__stop(શૂન્ય)
{
	mux__delete_listeners(g_listensock, g_listensock_ગણતરી);

	માટે(પૂર્ણાંક i=0; i<db.config->listener_ગણતરી; i++){
#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		જો(db.config->listeners[i].ws_context){
			lws_context_destroy(db.config->listeners[i].ws_context);
		}
		mosquitto_FREE(db.config->listeners[i].ws_protocol);
#endif
#ifdef WITH_UNIX_SOCKETS
		જો(db.config->listeners[i].unix_socket_path != ખાલી){
			unlink(db.config->listeners[i].unix_socket_path);
		}
#endif
#ifdef WITH_HTTP_API
		જો(db.config->listeners[i].mhd){
			http_api__stop(&db.config->listeners[i]);
		}
#endif
	}

	માટે(પૂર્ણાંક i=0; i<g_listensock_ગણતરી; i++){
		જો(g_listensock[i].sock != INVALID_SOCKET){
			COMPAT_CLOSE(g_listensock[i].sock);
		}
	}
	mosquitto_FREE(g_listensock);
	g_listensock_ગણતરી = 0;
	listensock_index = 0;
}
