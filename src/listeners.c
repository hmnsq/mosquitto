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

ساكن صحيح listensock_اشاریہ = 0;
خارجي صحيح g_لگاتار;


فراغ listener_سیٹ_defaults(بنية mosquitto__listener *listener)
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

	mosquitto_FREE(listener->security_options->acl_انکڑے.acl_file);
	mosquitto_FREE(listener->security_options->password_انکڑے.password_file);
	mosquitto_FREE(listener->security_options->psk_file);
	listener->security_options->allow_anonymous = -1;
	listener->security_options->allow_zero_لمائی_clientid = true;
	mosquitto_FREE(listener->security_options->auto_id_اگیتر);
	listener->security_options->auto_id_اگیتر_len = 0;
#جے_تعریف_شدہ WITH_TLS
	listener->require_certificate = false;
	listener->use_پچھان_تت_as_username = false;
	listener->use_subject_as_username = false;
	listener->use_username_as_clientid = false;
	listener->disable_client_cert_date_checks = false;
#جے_انت

#إذا defined(WITH_WEBSOCKETS) && (LWS_LIBRARY_نسخہ_نمبر >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN)
	لكل(صحيح i=0; i<listener->ws_origin_گنتی; i++){
		mosquitto_FREE(listener->ws_origins[i]);
	}
	mosquitto_FREE(listener->ws_origins);
	listener->ws_origin_گنتی = 0;
#جے_انت
}


فراغ listeners__reload_all_certificates(فراغ)
{
#جے_تعریف_شدہ WITH_TLS
	لكل(صحيح i=0; i<db.config->listener_گنتی; i++){
		بنية mosquitto__listener *listener = &db.config->listeners[i];
		إذا(listener->ssl_ctx && listener->certfile && listener->keyfile){
			صحيح rc = net_لدو_certificates(listener);
			إذا(rc){
				log__printf(عدم, MOSQ_LOG_ERR, "Error when reloading certificate '%s' or key '%s'.",
						listener->certfile, listener->keyfile);
			}
		}
	}
#جے_انت
}


ساكن صحيح listeners_شروع_single_mqtt(بنية mosquitto__listener *listener)
{
	بنية mosquitto__listener_sock *listensock_new;

	إذا(net__socket_listen(listener)){
		أرجع 1;
	}
	g_listensock_گنتی += listener->sock_گنتی;
	listensock_new = mosquitto_realloc(g_listensock, حجم(بنية mosquitto__listener_sock)*(size_t)g_listensock_گنتی);
	إذا(!listensock_new){
		أرجع 1;
	}
	g_listensock = listensock_new;

	لكل(صحيح i=0; i<listener->sock_گنتی; i++){
		إذا(listener->socks[i] == INVALID_SOCKET){
			أرجع 1;
		}
		g_listensock[listensock_اشاریہ].sock = listener->socks[i];
		g_listensock[listensock_اشاریہ].listener = listener;
#إذا defined(WITH_EPOLL) || defined(WITH_KQUEUE)
		g_listensock[listensock_اشاریہ].ident = id_listener;
#جے_انت
		listensock_اشاریہ++;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS


فراغ listeners_جوڑو_websockets(بنية lws_context *ws_context, mosq_sock_t fd)
{
	بنية mosquitto__listener *listener = عدم;
	بنية mosquitto__listener_sock *listensock_new;

	/* Don't add more listeners after we've started the main loop */
	إذا(g_لگاتار || ws_context == عدم){
		أرجع;
	}

	/* Find context */
	لكل(صحيح i=0; i<db.config->listener_گنتی; i++){
		إذا(db.config->listeners[i].ws_وچ_init){
			listener = &db.config->listeners[i];
			اكسر;
		}
	}
	إذا(listener == عدم){
		أرجع;
	}

	g_listensock_گنتی++;
	listensock_new = mosquitto_realloc(g_listensock, حجم(بنية mosquitto__listener_sock)*(size_t)g_listensock_گنتی);
	إذا(!listensock_new){
		أرجع;
	}
	g_listensock = listensock_new;

	g_listensock[listensock_اشاریہ].sock = fd;
	g_listensock[listensock_اشاریہ].listener = listener;
#إذا defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	g_listensock[listensock_اشاریہ].ident = id_listener_ws;
#جے_انت
	listensock_اشاریہ++;
}
#جے_انت


ساكن صحيح listeners_جوڑو_local(ثابت حرف *host, uint16_t پورٹ)
{
	بنية mosquitto__listener *listeners;
	bool allow_anonymous;

	listeners = db.config->listeners;
	إذا(db.config->security_options.allow_anonymous == -1){
		allow_anonymous = true;
	}وإلا{
		allow_anonymous = db.config->security_options.allow_anonymous;
	}

	listeners[db.config->listener_گنتی].security_options = mosquitto_calloc(1, حجم(بنية mosquitto__security_options));
	إذا(listeners[db.config->listener_گنتی].security_options == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}

	listener_سیٹ_defaults(&listeners[db.config->listener_گنتی]);
	listeners[db.config->listener_گنتی].security_options->allow_anonymous = allow_anonymous;
	listeners[db.config->listener_گنتی].security_options->auto_id_اگیتر = mosquitto_strdup("auto-");
	إذا(listeners[db.config->listener_گنتی].security_options->auto_id_اگیتر == عدم){
		mosquitto_FREE(listeners[db.config->listener_گنتی].security_options);
		أرجع MOSQ_ERR_NOMEM;
	}
	listeners[db.config->listener_گنتی].security_options->auto_id_اگیتر_len = (صحيح)لڑی_لمبائی("auto-");
	listeners[db.config->listener_گنتی].پورٹ = پورٹ;
	listeners[db.config->listener_گنتی].host = mosquitto_strdup(host);
	إذا(listeners[db.config->listener_گنتی].host == عدم){
		mosquitto_FREE(listeners[db.config->listener_گنتی].security_options->auto_id_اگیتر);
		mosquitto_FREE(listeners[db.config->listener_گنتی].security_options);
		أرجع MOSQ_ERR_NOMEM;
	}
	إذا(listeners_شروع_single_mqtt(&listeners[db.config->listener_گنتی])){
		mosquitto_FREE(listeners[db.config->listener_گنتی].security_options->auto_id_اگیتر);
		mosquitto_FREE(listeners[db.config->listener_گنتی].security_options);
		mosquitto_FREE(listeners[db.config->listener_گنتی].host);
		أرجع MOSQ_ERR_UNKNOWN;
	}
	db.config->listener_گنتی++;
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح listeners_شروع_local_only(فراغ)
{
	/* Attempt to open listeners bound to 127.0.0.1 and ::1 only */
	صحيح rc;
	بنية mosquitto__listener *listeners;
	size_t گنتی;

	إذا(db.config->cmd_پورٹ_گنتی == 0){
		گنتی = 2;
	}وإلا{
		گنتی = (size_t)(db.config->cmd_پورٹ_گنتی*2);
	}

#جے_تعریف_شدہ WITH_HTTP_API
	گنتی++;
#جے_انت

	listeners = mosquitto_realloc(db.config->listeners, گنتی*حجم(بنية mosquitto__listener));
	إذا(listeners == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}
	memset(listeners, 0, گنتی*حجم(بنية mosquitto__listener));
	db.config->listener_گنتی = 0;
	db.config->listeners = listeners;

	log__printf(عدم, MOSQ_LOG_WARNING, "Starting in local only mode. Connections will only be possible from clients running on this machine.");
	log__printf(عدم, MOSQ_LOG_WARNING, "Create a configuration file which defines a listener to allow remote access.");
	log__printf(عدم, MOSQ_LOG_WARNING, "For more details see https://mosquitto.org/documentation/authentication-methods/");
	إذا(db.config->cmd_پورٹ_گنتی == 0){
		rc = listeners_جوڑو_local("127.0.0.1", 1883);
		إذا(rc == MOSQ_ERR_NOMEM){
			أرجع MOSQ_ERR_NOMEM;
		}
		rc = listeners_جوڑو_local("::1", 1883);
		إذا(rc == MOSQ_ERR_NOMEM){
			أرجع MOSQ_ERR_NOMEM;
		}
	}وإلا{
		لكل(صحيح i=0; i<db.config->cmd_پورٹ_گنتی; i++){
			rc = listeners_جوڑو_local("127.0.0.1", db.config->cmd_پورٹ[i]);
			إذا(rc == MOSQ_ERR_NOMEM){
				أرجع MOSQ_ERR_NOMEM;
			}
			rc = listeners_جوڑو_local("::1", db.config->cmd_پورٹ[i]);
			إذا(rc == MOSQ_ERR_NOMEM){
				أرجع MOSQ_ERR_NOMEM;
			}
		}
	}

	إذا(db.config->listener_گنتی > 0){
#جے_تعریف_شدہ WITH_HTTP_API
		db.config->listener_گنتی++;
		http_api_شروع_local(&db.config->listeners[db.config->listener_گنتی-1]);
#جے_انت
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا{
		أرجع MOSQ_ERR_UNKNOWN;
	}
}


صحيح listeners_شروع(فراغ)
{
	g_listensock_گنتی = 0;

	إذا(db.config->local_only){
		إذا(listeners_شروع_local_only()){
			db__close();
			إذا(db.config->pid_file){
				(فراغ)remove(db.config->pid_file);
			}
			أرجع 1;
		}
		mux_جوڑو_listeners(g_listensock, g_listensock_گنتی);
		أرجع MOSQ_ERR_SUCCESS;
	}

	لكل(صحيح i=0; i<db.config->listener_گنتی; i++){
		إذا(db.config->listeners[i].protocol == mp_mqtt){
			إذا(listeners_شروع_single_mqtt(&db.config->listeners[i])){
				db__close();
				إذا(db.config->pid_file){
					(فراغ)remove(db.config->pid_file);
				}
				أرجع 1;
			}
		}وإلا إذا(db.config->listeners[i].protocol == mp_websockets){
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			mosq_websockets_init(&db.config->listeners[i], db.config);
			إذا(!db.config->listeners[i].ws_context){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to create websockets listener on port %d.", db.config->listeners[i].پورٹ);
				أرجع 1;
			}
#وإلاإذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			إذا(listeners_شروع_single_mqtt(&db.config->listeners[i])){
				db__close();
				إذا(db.config->pid_file){
					(فراغ)remove(db.config->pid_file);
				}
				أرجع 1;
			}
#جے_انت
#جے_تعریف_شدہ WITH_HTTP_API
		}وإلا إذا(db.config->listeners[i].protocol == mp_http_api){
			http_api_شروع(&db.config->listeners[i]);
#جے_انت
		}
	}
	إذا(g_listensock == عدم){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to start any listening sockets, exiting.");
		أرجع 1;
	}

	mux_جوڑو_listeners(g_listensock, g_listensock_گنتی);
	أرجع MOSQ_ERR_SUCCESS;
}


فراغ listeners__stop(فراغ)
{
	mux__delete_listeners(g_listensock, g_listensock_گنتی);

	لكل(صحيح i=0; i<db.config->listener_گنتی; i++){
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		إذا(db.config->listeners[i].ws_context){
			lws_context_destroy(db.config->listeners[i].ws_context);
		}
		mosquitto_FREE(db.config->listeners[i].ws_protocol);
#جے_انت
#جے_تعریف_شدہ WITH_UNIX_SOCKETS
		إذا(db.config->listeners[i].unix_socket_رستہ != عدم){
			unlink(db.config->listeners[i].unix_socket_رستہ);
		}
#جے_انت
#جے_تعریف_شدہ WITH_HTTP_API
		إذا(db.config->listeners[i].mhd){
			http_api__stop(&db.config->listeners[i]);
		}
#جے_انت
	}

	لكل(صحيح i=0; i<g_listensock_گنتی; i++){
		إذا(g_listensock[i].sock != INVALID_SOCKET){
			COMPAT_CLOSE(g_listensock[i].sock);
		}
	}
	mosquitto_FREE(g_listensock);
	g_listensock_گنتی = 0;
	listensock_اشاریہ = 0;
}
