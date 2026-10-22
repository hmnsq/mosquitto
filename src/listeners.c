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

#شامل "config.h"
#شامل "net_mosq.h"
#شامل "mosquitto_broker_internal.h"

ایستا صحیح listensock_index = 0;
بیرونی صحیح g_run;


تهی listener__set_defaults(ساختار mosquitto__listener *listener)
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

#اگر defined(WITH_WEBSOCKETS) && (LWS_LIBRARY_VERSION_عدد >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN)
	برای(صحیح i=0; i<listener->ws_origin_شمار; i++){
		mosquitto_FREE(listener->ws_origins[i]);
	}
	mosquitto_FREE(listener->ws_origins);
	listener->ws_origin_شمار = 0;
#endif
}


تهی listeners__reload_all_certificates(تهی)
{
#ifdef WITH_TLS
	برای(صحیح i=0; i<db.config->listener_شمار; i++){
		ساختار mosquitto__listener *listener = &db.config->listeners[i];
		اگر(listener->ssl_ctx && listener->certfile && listener->keyfile){
			صحیح rc = net__load_certificates(listener);
			اگر(rc){
				log__printf(پوچ, MOSQ_LOG_ERR, "Error when reloading certificate '%s' or key '%s'.",
						listener->certfile, listener->keyfile);
			}
		}
	}
#endif
}


ایستا صحیح listeners__start_single_mqtt(ساختار mosquitto__listener *listener)
{
	ساختار mosquitto__listener_sock *listensock_new;

	اگر(net__socket_listen(listener)){
		بازگشت 1;
	}
	g_listensock_شمار += listener->sock_شمار;
	listensock_new = mosquitto_realloc(g_listensock, اندازه(ساختار mosquitto__listener_sock)*(size_t)g_listensock_شمار);
	اگر(!listensock_new){
		بازگشت 1;
	}
	g_listensock = listensock_new;

	برای(صحیح i=0; i<listener->sock_شمار; i++){
		اگر(listener->socks[i] == INVALID_SOCKET){
			بازگشت 1;
		}
		g_listensock[listensock_index].sock = listener->socks[i];
		g_listensock[listensock_index].listener = listener;
#اگر defined(WITH_EPOLL) || defined(WITH_KQUEUE)
		g_listensock[listensock_index].ident = id_listener;
#endif
		listensock_index++;
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS


تهی listeners__add_websockets(ساختار lws_context *ws_context, mosq_sock_t fd)
{
	ساختار mosquitto__listener *listener = پوچ;
	ساختار mosquitto__listener_sock *listensock_new;

	/* Don't add more listeners after we've started the main loop */
	اگر(g_run || ws_context == پوچ){
		بازگشت;
	}

	/* Find context */
	برای(صحیح i=0; i<db.config->listener_شمار; i++){
		اگر(db.config->listeners[i].ws_in_init){
			listener = &db.config->listeners[i];
			بشکن;
		}
	}
	اگر(listener == پوچ){
		بازگشت;
	}

	g_listensock_شمار++;
	listensock_new = mosquitto_realloc(g_listensock, اندازه(ساختار mosquitto__listener_sock)*(size_t)g_listensock_شمار);
	اگر(!listensock_new){
		بازگشت;
	}
	g_listensock = listensock_new;

	g_listensock[listensock_index].sock = fd;
	g_listensock[listensock_index].listener = listener;
#اگر defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	g_listensock[listensock_index].ident = id_listener_ws;
#endif
	listensock_index++;
}
#endif


ایستا صحیح listeners__add_local(ثابت نویسه *host, uint16_t port)
{
	ساختار mosquitto__listener *listeners;
	bool allow_anonymous;

	listeners = db.config->listeners;
	اگر(db.config->security_options.allow_anonymous == -1){
		allow_anonymous = true;
	}وگرنه{
		allow_anonymous = db.config->security_options.allow_anonymous;
	}

	listeners[db.config->listener_شمار].security_options = mosquitto_calloc(1, اندازه(ساختار mosquitto__security_options));
	اگر(listeners[db.config->listener_شمار].security_options == پوچ){
		بازگشت MOSQ_ERR_NOMEM;
	}

	listener__set_defaults(&listeners[db.config->listener_شمار]);
	listeners[db.config->listener_شمار].security_options->allow_anonymous = allow_anonymous;
	listeners[db.config->listener_شمار].security_options->auto_id_prefix = mosquitto_strdup("auto-");
	اگر(listeners[db.config->listener_شمار].security_options->auto_id_prefix == پوچ){
		mosquitto_FREE(listeners[db.config->listener_شمار].security_options);
		بازگشت MOSQ_ERR_NOMEM;
	}
	listeners[db.config->listener_شمار].security_options->auto_id_prefix_len = (صحیح)strlen("auto-");
	listeners[db.config->listener_شمار].port = port;
	listeners[db.config->listener_شمار].host = mosquitto_strdup(host);
	اگر(listeners[db.config->listener_شمار].host == پوچ){
		mosquitto_FREE(listeners[db.config->listener_شمار].security_options->auto_id_prefix);
		mosquitto_FREE(listeners[db.config->listener_شمار].security_options);
		بازگشت MOSQ_ERR_NOMEM;
	}
	اگر(listeners__start_single_mqtt(&listeners[db.config->listener_شمار])){
		mosquitto_FREE(listeners[db.config->listener_شمار].security_options->auto_id_prefix);
		mosquitto_FREE(listeners[db.config->listener_شمار].security_options);
		mosquitto_FREE(listeners[db.config->listener_شمار].host);
		بازگشت MOSQ_ERR_UNKNOWN;
	}
	db.config->listener_شمار++;
	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح listeners__start_local_only(تهی)
{
	/* Attempt to open listeners bound to 127.0.0.1 and ::1 only */
	صحیح rc;
	ساختار mosquitto__listener *listeners;
	size_t شمار;

	اگر(db.config->cmd_port_شمار == 0){
		شمار = 2;
	}وگرنه{
		شمار = (size_t)(db.config->cmd_port_شمار*2);
	}

#ifdef WITH_HTTP_API
	شمار++;
#endif

	listeners = mosquitto_realloc(db.config->listeners, شمار*اندازه(ساختار mosquitto__listener));
	اگر(listeners == پوچ){
		بازگشت MOSQ_ERR_NOMEM;
	}
	memset(listeners, 0, شمار*اندازه(ساختار mosquitto__listener));
	db.config->listener_شمار = 0;
	db.config->listeners = listeners;

	log__printf(پوچ, MOSQ_LOG_WARNING, "Starting in local only mode. Connections will only be possible from clients running on this machine.");
	log__printf(پوچ, MOSQ_LOG_WARNING, "Create a configuration file which defines a listener to allow remote access.");
	log__printf(پوچ, MOSQ_LOG_WARNING, "For more details see https://mosquitto.org/documentation/authentication-methods/");
	اگر(db.config->cmd_port_شمار == 0){
		rc = listeners__add_local("127.0.0.1", 1883);
		اگر(rc == MOSQ_ERR_NOMEM){
			بازگشت MOSQ_ERR_NOMEM;
		}
		rc = listeners__add_local("::1", 1883);
		اگر(rc == MOSQ_ERR_NOMEM){
			بازگشت MOSQ_ERR_NOMEM;
		}
	}وگرنه{
		برای(صحیح i=0; i<db.config->cmd_port_شمار; i++){
			rc = listeners__add_local("127.0.0.1", db.config->cmd_port[i]);
			اگر(rc == MOSQ_ERR_NOMEM){
				بازگشت MOSQ_ERR_NOMEM;
			}
			rc = listeners__add_local("::1", db.config->cmd_port[i]);
			اگر(rc == MOSQ_ERR_NOMEM){
				بازگشت MOSQ_ERR_NOMEM;
			}
		}
	}

	اگر(db.config->listener_شمار > 0){
#ifdef WITH_HTTP_API
		db.config->listener_شمار++;
		http_api__start_local(&db.config->listeners[db.config->listener_شمار-1]);
#endif
		بازگشت MOSQ_ERR_SUCCESS;
	}وگرنه{
		بازگشت MOSQ_ERR_UNKNOWN;
	}
}


صحیح listeners__start(تهی)
{
	g_listensock_شمار = 0;

	اگر(db.config->local_only){
		اگر(listeners__start_local_only()){
			db__close();
			اگر(db.config->pid_file){
				(تهی)remove(db.config->pid_file);
			}
			بازگشت 1;
		}
		mux__add_listeners(g_listensock, g_listensock_شمار);
		بازگشت MOSQ_ERR_SUCCESS;
	}

	برای(صحیح i=0; i<db.config->listener_شمار; i++){
		اگر(db.config->listeners[i].protocol == mp_mqtt){
			اگر(listeners__start_single_mqtt(&db.config->listeners[i])){
				db__close();
				اگر(db.config->pid_file){
					(تهی)remove(db.config->pid_file);
				}
				بازگشت 1;
			}
		}وگرنه اگر(db.config->listeners[i].protocol == mp_websockets){
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			mosq_websockets_init(&db.config->listeners[i], db.config);
			اگر(!db.config->listeners[i].ws_context){
				log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to create websockets listener on port %d.", db.config->listeners[i].port);
				بازگشت 1;
			}
#وگرنهاگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			اگر(listeners__start_single_mqtt(&db.config->listeners[i])){
				db__close();
				اگر(db.config->pid_file){
					(تهی)remove(db.config->pid_file);
				}
				بازگشت 1;
			}
#endif
#ifdef WITH_HTTP_API
		}وگرنه اگر(db.config->listeners[i].protocol == mp_http_api){
			http_api__start(&db.config->listeners[i]);
#endif
		}
	}
	اگر(g_listensock == پوچ){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to start any listening sockets, exiting.");
		بازگشت 1;
	}

	mux__add_listeners(g_listensock, g_listensock_شمار);
	بازگشت MOSQ_ERR_SUCCESS;
}


تهی listeners__stop(تهی)
{
	mux__delete_listeners(g_listensock, g_listensock_شمار);

	برای(صحیح i=0; i<db.config->listener_شمار; i++){
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		اگر(db.config->listeners[i].ws_context){
			lws_context_destroy(db.config->listeners[i].ws_context);
		}
		mosquitto_FREE(db.config->listeners[i].ws_protocol);
#endif
#ifdef WITH_UNIX_SOCKETS
		اگر(db.config->listeners[i].unix_socket_path != پوچ){
			unlink(db.config->listeners[i].unix_socket_path);
		}
#endif
#ifdef WITH_HTTP_API
		اگر(db.config->listeners[i].mhd){
			http_api__stop(&db.config->listeners[i]);
		}
#endif
	}

	برای(صحیح i=0; i<g_listensock_شمار; i++){
		اگر(g_listensock[i].sock != INVALID_SOCKET){
			COMPAT_CLOSE(g_listensock[i].sock);
		}
	}
	mosquitto_FREE(g_listensock);
	g_listensock_شمار = 0;
	listensock_index = 0;
}
