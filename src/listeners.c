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

ساکن صحیح listensock_اشاریہ = 0;
خارجی صحیح g_تسلسل;


خالی listener_سیٹ_defaults(ساخت mosquitto__listener *listener)
{
	listener->disable_protocol_v3 = false;
	listener->disable_protocol_v4 = false;
	listener->disable_protocol_v5 = false;
	listener->max_connections = -1;
	listener->max_qos = 2;
	listener->max_topic_alias = 10;
	listener->max_topic_alias_broker = 10;
	listener->protocol = mp_mqtt;
	mosquitto_FREE(listener->mount_نقطہ);

	mosquitto_FREE(listener->security_options->acl_ڈیٹا.acl_file);
	mosquitto_FREE(listener->security_options->password_ڈیٹا.password_file);
	mosquitto_FREE(listener->security_options->psk_file);
	listener->security_options->allow_anonymous = -1;
	listener->security_options->allow_zero_لمبائی_clientid = true;
	mosquitto_FREE(listener->security_options->auto_id_سابقہ);
	listener->security_options->auto_id_سابقہ_len = 0;
#اگر_تعریف_شدہ WITH_TLS
	listener->require_certificate = false;
	listener->use_شناختی_عنصر_as_username = false;
	listener->use_subject_as_username = false;
	listener->use_username_as_clientid = false;
	listener->disable_client_cert_date_checks = false;
#اگر_ختم

#اگر defined(WITH_WEBSOCKETS) && (LWS_LIBRARY_نسخہ_نمبر >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN)
	برائے(صحیح i=0; i<listener->ws_origin_گنتی; i++){
		mosquitto_FREE(listener->ws_origins[i]);
	}
	mosquitto_FREE(listener->ws_origins);
	listener->ws_origin_گنتی = 0;
#اگر_ختم
}


خالی listeners__reload_all_certificates(خالی)
{
#اگر_تعریف_شدہ WITH_TLS
	برائے(صحیح i=0; i<db.config->listener_گنتی; i++){
		ساخت mosquitto__listener *listener = &db.config->listeners[i];
		اگر(listener->ssl_ctx && listener->certfile && listener->keyfile){
			صحیح rc = net_بھرو_certificates(listener);
			اگر(rc){
				log__printf(تہی, MOSQ_LOG_ERR, "Error when reloading certificate '%s' or key '%s'.",
						listener->certfile, listener->keyfile);
			}
		}
	}
#اگر_ختم
}


ساکن صحیح listeners_آغاز_single_mqtt(ساخت mosquitto__listener *listener)
{
	ساخت mosquitto__listener_sock *listensock_new;

	اگر(net__socket_listen(listener)){
		واپس 1;
	}
	g_listensock_گنتی += listener->sock_گنتی;
	listensock_new = mosquitto_realloc(g_listensock, حجم(ساخت mosquitto__listener_sock)*(size_t)g_listensock_گنتی);
	اگر(!listensock_new){
		واپس 1;
	}
	g_listensock = listensock_new;

	برائے(صحیح i=0; i<listener->sock_گنتی; i++){
		اگر(listener->socks[i] == INVALID_SOCKET){
			واپس 1;
		}
		g_listensock[listensock_اشاریہ].sock = listener->socks[i];
		g_listensock[listensock_اشاریہ].listener = listener;
#اگر defined(WITH_EPOLL) || defined(WITH_KQUEUE)
		g_listensock[listensock_اشاریہ].ident = id_listener;
#اگر_ختم
		listensock_اشاریہ++;
	}
	واپس MOSQ_ERR_SUCCESS;
}


#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS


خالی listeners_جوڑو_websockets(ساخت lws_context *ws_context, mosq_sock_t fd)
{
	ساخت mosquitto__listener *listener = تہی;
	ساخت mosquitto__listener_sock *listensock_new;

	/* Don't add more listeners after we've started the main loop */
	اگر(g_تسلسل || ws_context == تہی){
		واپس;
	}

	/* Find context */
	برائے(صحیح i=0; i<db.config->listener_گنتی; i++){
		اگر(db.config->listeners[i].ws_اندر_init){
			listener = &db.config->listeners[i];
			توڑو;
		}
	}
	اگر(listener == تہی){
		واپس;
	}

	g_listensock_گنتی++;
	listensock_new = mosquitto_realloc(g_listensock, حجم(ساخت mosquitto__listener_sock)*(size_t)g_listensock_گنتی);
	اگر(!listensock_new){
		واپس;
	}
	g_listensock = listensock_new;

	g_listensock[listensock_اشاریہ].sock = fd;
	g_listensock[listensock_اشاریہ].listener = listener;
#اگر defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	g_listensock[listensock_اشاریہ].ident = id_listener_ws;
#اگر_ختم
	listensock_اشاریہ++;
}
#اگر_ختم


ساکن صحیح listeners_جوڑو_local(ثابت حرف *host, uint16_t پورٹ)
{
	ساخت mosquitto__listener *listeners;
	bool allow_anonymous;

	listeners = db.config->listeners;
	اگر(db.config->security_options.allow_anonymous == -1){
		allow_anonymous = true;
	}ورنہ{
		allow_anonymous = db.config->security_options.allow_anonymous;
	}

	listeners[db.config->listener_گنتی].security_options = mosquitto_calloc(1, حجم(ساخت mosquitto__security_options));
	اگر(listeners[db.config->listener_گنتی].security_options == تہی){
		واپس MOSQ_ERR_NOMEM;
	}

	listener_سیٹ_defaults(&listeners[db.config->listener_گنتی]);
	listeners[db.config->listener_گنتی].security_options->allow_anonymous = allow_anonymous;
	listeners[db.config->listener_گنتی].security_options->auto_id_سابقہ = mosquitto_strdup("auto-");
	اگر(listeners[db.config->listener_گنتی].security_options->auto_id_سابقہ == تہی){
		mosquitto_FREE(listeners[db.config->listener_گنتی].security_options);
		واپس MOSQ_ERR_NOMEM;
	}
	listeners[db.config->listener_گنتی].security_options->auto_id_سابقہ_len = (صحیح)لڑی_لمبائی("auto-");
	listeners[db.config->listener_گنتی].پورٹ = پورٹ;
	listeners[db.config->listener_گنتی].host = mosquitto_strdup(host);
	اگر(listeners[db.config->listener_گنتی].host == تہی){
		mosquitto_FREE(listeners[db.config->listener_گنتی].security_options->auto_id_سابقہ);
		mosquitto_FREE(listeners[db.config->listener_گنتی].security_options);
		واپس MOSQ_ERR_NOMEM;
	}
	اگر(listeners_آغاز_single_mqtt(&listeners[db.config->listener_گنتی])){
		mosquitto_FREE(listeners[db.config->listener_گنتی].security_options->auto_id_سابقہ);
		mosquitto_FREE(listeners[db.config->listener_گنتی].security_options);
		mosquitto_FREE(listeners[db.config->listener_گنتی].host);
		واپس MOSQ_ERR_UNKNOWN;
	}
	db.config->listener_گنتی++;
	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح listeners_آغاز_local_only(خالی)
{
	/* Attempt to open listeners bound to 127.0.0.1 and ::1 only */
	صحیح rc;
	ساخت mosquitto__listener *listeners;
	size_t گنتی;

	اگر(db.config->cmd_پورٹ_گنتی == 0){
		گنتی = 2;
	}ورنہ{
		گنتی = (size_t)(db.config->cmd_پورٹ_گنتی*2);
	}

#اگر_تعریف_شدہ WITH_HTTP_API
	گنتی++;
#اگر_ختم

	listeners = mosquitto_realloc(db.config->listeners, گنتی*حجم(ساخت mosquitto__listener));
	اگر(listeners == تہی){
		واپس MOSQ_ERR_NOMEM;
	}
	memset(listeners, 0, گنتی*حجم(ساخت mosquitto__listener));
	db.config->listener_گنتی = 0;
	db.config->listeners = listeners;

	log__printf(تہی, MOSQ_LOG_WARNING, "Starting in local only mode. Connections will only be possible from clients running on this machine.");
	log__printf(تہی, MOSQ_LOG_WARNING, "Create a configuration file which defines a listener to allow remote access.");
	log__printf(تہی, MOSQ_LOG_WARNING, "For more details see https://mosquitto.org/documentation/authentication-methods/");
	اگر(db.config->cmd_پورٹ_گنتی == 0){
		rc = listeners_جوڑو_local("127.0.0.1", 1883);
		اگر(rc == MOSQ_ERR_NOMEM){
			واپس MOSQ_ERR_NOMEM;
		}
		rc = listeners_جوڑو_local("::1", 1883);
		اگر(rc == MOSQ_ERR_NOMEM){
			واپس MOSQ_ERR_NOMEM;
		}
	}ورنہ{
		برائے(صحیح i=0; i<db.config->cmd_پورٹ_گنتی; i++){
			rc = listeners_جوڑو_local("127.0.0.1", db.config->cmd_پورٹ[i]);
			اگر(rc == MOSQ_ERR_NOMEM){
				واپس MOSQ_ERR_NOMEM;
			}
			rc = listeners_جوڑو_local("::1", db.config->cmd_پورٹ[i]);
			اگر(rc == MOSQ_ERR_NOMEM){
				واپس MOSQ_ERR_NOMEM;
			}
		}
	}

	اگر(db.config->listener_گنتی > 0){
#اگر_تعریف_شدہ WITH_HTTP_API
		db.config->listener_گنتی++;
		http_api_آغاز_local(&db.config->listeners[db.config->listener_گنتی-1]);
#اگر_ختم
		واپس MOSQ_ERR_SUCCESS;
	}ورنہ{
		واپس MOSQ_ERR_UNKNOWN;
	}
}


صحیح listeners_آغاز(خالی)
{
	g_listensock_گنتی = 0;

	اگر(db.config->local_only){
		اگر(listeners_آغاز_local_only()){
			db__close();
			اگر(db.config->pid_file){
				(خالی)remove(db.config->pid_file);
			}
			واپس 1;
		}
		mux_جوڑو_listeners(g_listensock, g_listensock_گنتی);
		واپس MOSQ_ERR_SUCCESS;
	}

	برائے(صحیح i=0; i<db.config->listener_گنتی; i++){
		اگر(db.config->listeners[i].protocol == mp_mqtt){
			اگر(listeners_آغاز_single_mqtt(&db.config->listeners[i])){
				db__close();
				اگر(db.config->pid_file){
					(خالی)remove(db.config->pid_file);
				}
				واپس 1;
			}
		}ورنہ اگر(db.config->listeners[i].protocol == mp_websockets){
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			mosq_websockets_init(&db.config->listeners[i], db.config);
			اگر(!db.config->listeners[i].ws_context){
				log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to create websockets listener on port %d.", db.config->listeners[i].پورٹ);
				واپس 1;
			}
#وگرنہ_اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			اگر(listeners_آغاز_single_mqtt(&db.config->listeners[i])){
				db__close();
				اگر(db.config->pid_file){
					(خالی)remove(db.config->pid_file);
				}
				واپس 1;
			}
#اگر_ختم
#اگر_تعریف_شدہ WITH_HTTP_API
		}ورنہ اگر(db.config->listeners[i].protocol == mp_http_api){
			http_api_آغاز(&db.config->listeners[i]);
#اگر_ختم
		}
	}
	اگر(g_listensock == تہی){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to start any listening sockets, exiting.");
		واپس 1;
	}

	mux_جوڑو_listeners(g_listensock, g_listensock_گنتی);
	واپس MOSQ_ERR_SUCCESS;
}


خالی listeners__stop(خالی)
{
	mux__delete_listeners(g_listensock, g_listensock_گنتی);

	برائے(صحیح i=0; i<db.config->listener_گنتی; i++){
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		اگر(db.config->listeners[i].ws_context){
			lws_context_destroy(db.config->listeners[i].ws_context);
		}
		mosquitto_FREE(db.config->listeners[i].ws_protocol);
#اگر_ختم
#اگر_تعریف_شدہ WITH_UNIX_SOCKETS
		اگر(db.config->listeners[i].unix_socket_راستہ != تہی){
			unlink(db.config->listeners[i].unix_socket_راستہ);
		}
#اگر_ختم
#اگر_تعریف_شدہ WITH_HTTP_API
		اگر(db.config->listeners[i].mhd){
			http_api__stop(&db.config->listeners[i]);
		}
#اگر_ختم
	}

	برائے(صحیح i=0; i<g_listensock_گنتی; i++){
		اگر(g_listensock[i].sock != INVALID_SOCKET){
			COMPAT_CLOSE(g_listensock[i].sock);
		}
	}
	mosquitto_FREE(g_listensock);
	g_listensock_گنتی = 0;
	listensock_اشاریہ = 0;
}
