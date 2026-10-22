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

ساكن صحيح listensock_شاخص = 0;
خارجي صحيح g_پیاپی;


فراغ listener_مجموعه_defaults(بنية mosquitto__listener *listener)
{
	listener->disable_protocol_v3 = false;
	listener->disable_protocol_v4 = false;
	listener->disable_protocol_v5 = false;
	listener->max_connections = -1;
	listener->max_qos = 2;
	listener->max_topic_alias = 10;
	listener->max_topic_alias_broker = 10;
	listener->protocol = mp_mqtt;
	mosquitto_FREE(listener->mount_نقطه);

	mosquitto_FREE(listener->security_options->acl_داده.acl_file);
	mosquitto_FREE(listener->security_options->password_داده.password_file);
	mosquitto_FREE(listener->security_options->psk_file);
	listener->security_options->allow_anonymous = -1;
	listener->security_options->allow_zero_درازی_clientid = true;
	mosquitto_FREE(listener->security_options->auto_id_پیشوند);
	listener->security_options->auto_id_پیشوند_len = 0;
#اگر_تعریف_شده WITH_TLS
	listener->require_certificate = false;
	listener->use_عنصر_خنثی_as_username = false;
	listener->use_subject_as_username = false;
	listener->use_username_as_clientid = false;
	listener->disable_client_cert_date_checks = false;
#پایان_اگر

#إذا defined(WITH_WEBSOCKETS) && (LWS_LIBRARY_نسخه_عدد >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN)
	لكل(صحيح i=0; i<listener->ws_origin_شمار; i++){
		mosquitto_FREE(listener->ws_origins[i]);
	}
	mosquitto_FREE(listener->ws_origins);
	listener->ws_origin_شمار = 0;
#پایان_اگر
}


فراغ listeners__reload_all_certificates(فراغ)
{
#اگر_تعریف_شده WITH_TLS
	لكل(صحيح i=0; i<db.config->listener_شمار; i++){
		بنية mosquitto__listener *listener = &db.config->listeners[i];
		إذا(listener->ssl_ctx && listener->certfile && listener->keyfile){
			صحيح rc = net_بارگیری_certificates(listener);
			إذا(rc){
				log__printf(عدم, MOSQ_LOG_ERR, "Error when reloading certificate '%s' or key '%s'.",
						listener->certfile, listener->keyfile);
			}
		}
	}
#پایان_اگر
}


ساكن صحيح listeners_شروع_single_mqtt(بنية mosquitto__listener *listener)
{
	بنية mosquitto__listener_sock *listensock_new;

	إذا(net__socket_listen(listener)){
		أرجع 1;
	}
	g_listensock_شمار += listener->sock_شمار;
	listensock_new = mosquitto_realloc(g_listensock, حجم(بنية mosquitto__listener_sock)*(size_t)g_listensock_شمار);
	إذا(!listensock_new){
		أرجع 1;
	}
	g_listensock = listensock_new;

	لكل(صحيح i=0; i<listener->sock_شمار; i++){
		إذا(listener->socks[i] == INVALID_SOCKET){
			أرجع 1;
		}
		g_listensock[listensock_شاخص].sock = listener->socks[i];
		g_listensock[listensock_شاخص].listener = listener;
#إذا defined(WITH_EPOLL) || defined(WITH_KQUEUE)
		g_listensock[listensock_شاخص].ident = id_listener;
#پایان_اگر
		listensock_شاخص++;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS


فراغ listeners_جمع_کن_websockets(بنية lws_context *ws_context, mosq_sock_t fd)
{
	بنية mosquitto__listener *listener = عدم;
	بنية mosquitto__listener_sock *listensock_new;

	/* Don't add more listeners after we've started the main loop */
	إذا(g_پیاپی || ws_context == عدم){
		أرجع;
	}

	/* Find context */
	لكل(صحيح i=0; i<db.config->listener_شمار; i++){
		إذا(db.config->listeners[i].ws_درون_init){
			listener = &db.config->listeners[i];
			اكسر;
		}
	}
	إذا(listener == عدم){
		أرجع;
	}

	g_listensock_شمار++;
	listensock_new = mosquitto_realloc(g_listensock, حجم(بنية mosquitto__listener_sock)*(size_t)g_listensock_شمار);
	إذا(!listensock_new){
		أرجع;
	}
	g_listensock = listensock_new;

	g_listensock[listensock_شاخص].sock = fd;
	g_listensock[listensock_شاخص].listener = listener;
#إذا defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	g_listensock[listensock_شاخص].ident = id_listener_ws;
#پایان_اگر
	listensock_شاخص++;
}
#پایان_اگر


ساكن صحيح listeners_جمع_کن_local(ثابت حرف *host, uint16_t پورت)
{
	بنية mosquitto__listener *listeners;
	bool allow_anonymous;

	listeners = db.config->listeners;
	إذا(db.config->security_options.allow_anonymous == -1){
		allow_anonymous = true;
	}وإلا{
		allow_anonymous = db.config->security_options.allow_anonymous;
	}

	listeners[db.config->listener_شمار].security_options = mosquitto_calloc(1, حجم(بنية mosquitto__security_options));
	إذا(listeners[db.config->listener_شمار].security_options == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}

	listener_مجموعه_defaults(&listeners[db.config->listener_شمار]);
	listeners[db.config->listener_شمار].security_options->allow_anonymous = allow_anonymous;
	listeners[db.config->listener_شمار].security_options->auto_id_پیشوند = mosquitto_strdup("auto-");
	إذا(listeners[db.config->listener_شمار].security_options->auto_id_پیشوند == عدم){
		mosquitto_FREE(listeners[db.config->listener_شمار].security_options);
		أرجع MOSQ_ERR_NOMEM;
	}
	listeners[db.config->listener_شمار].security_options->auto_id_پیشوند_len = (صحيح)طول_رشته("auto-");
	listeners[db.config->listener_شمار].پورت = پورت;
	listeners[db.config->listener_شمار].host = mosquitto_strdup(host);
	إذا(listeners[db.config->listener_شمار].host == عدم){
		mosquitto_FREE(listeners[db.config->listener_شمار].security_options->auto_id_پیشوند);
		mosquitto_FREE(listeners[db.config->listener_شمار].security_options);
		أرجع MOSQ_ERR_NOMEM;
	}
	إذا(listeners_شروع_single_mqtt(&listeners[db.config->listener_شمار])){
		mosquitto_FREE(listeners[db.config->listener_شمار].security_options->auto_id_پیشوند);
		mosquitto_FREE(listeners[db.config->listener_شمار].security_options);
		mosquitto_FREE(listeners[db.config->listener_شمار].host);
		أرجع MOSQ_ERR_UNKNOWN;
	}
	db.config->listener_شمار++;
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح listeners_شروع_local_only(فراغ)
{
	/* Attempt to open listeners bound to 127.0.0.1 and ::1 only */
	صحيح rc;
	بنية mosquitto__listener *listeners;
	size_t شمار;

	إذا(db.config->cmd_پورت_شمار == 0){
		شمار = 2;
	}وإلا{
		شمار = (size_t)(db.config->cmd_پورت_شمار*2);
	}

#اگر_تعریف_شده WITH_HTTP_API
	شمار++;
#پایان_اگر

	listeners = mosquitto_realloc(db.config->listeners, شمار*حجم(بنية mosquitto__listener));
	إذا(listeners == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}
	memset(listeners, 0, شمار*حجم(بنية mosquitto__listener));
	db.config->listener_شمار = 0;
	db.config->listeners = listeners;

	log__printf(عدم, MOSQ_LOG_WARNING, "Starting in local only mode. Connections will only be possible from clients running on this machine.");
	log__printf(عدم, MOSQ_LOG_WARNING, "Create a configuration file which defines a listener to allow remote access.");
	log__printf(عدم, MOSQ_LOG_WARNING, "For more details see https://mosquitto.org/documentation/authentication-methods/");
	إذا(db.config->cmd_پورت_شمار == 0){
		rc = listeners_جمع_کن_local("127.0.0.1", 1883);
		إذا(rc == MOSQ_ERR_NOMEM){
			أرجع MOSQ_ERR_NOMEM;
		}
		rc = listeners_جمع_کن_local("::1", 1883);
		إذا(rc == MOSQ_ERR_NOMEM){
			أرجع MOSQ_ERR_NOMEM;
		}
	}وإلا{
		لكل(صحيح i=0; i<db.config->cmd_پورت_شمار; i++){
			rc = listeners_جمع_کن_local("127.0.0.1", db.config->cmd_پورت[i]);
			إذا(rc == MOSQ_ERR_NOMEM){
				أرجع MOSQ_ERR_NOMEM;
			}
			rc = listeners_جمع_کن_local("::1", db.config->cmd_پورت[i]);
			إذا(rc == MOSQ_ERR_NOMEM){
				أرجع MOSQ_ERR_NOMEM;
			}
		}
	}

	إذا(db.config->listener_شمار > 0){
#اگر_تعریف_شده WITH_HTTP_API
		db.config->listener_شمار++;
		http_api_شروع_local(&db.config->listeners[db.config->listener_شمار-1]);
#پایان_اگر
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا{
		أرجع MOSQ_ERR_UNKNOWN;
	}
}


صحيح listeners_شروع(فراغ)
{
	g_listensock_شمار = 0;

	إذا(db.config->local_only){
		إذا(listeners_شروع_local_only()){
			db__close();
			إذا(db.config->pid_file){
				(فراغ)remove(db.config->pid_file);
			}
			أرجع 1;
		}
		mux_جمع_کن_listeners(g_listensock, g_listensock_شمار);
		أرجع MOSQ_ERR_SUCCESS;
	}

	لكل(صحيح i=0; i<db.config->listener_شمار; i++){
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
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to create websockets listener on port %d.", db.config->listeners[i].پورت);
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
#پایان_اگر
#اگر_تعریف_شده WITH_HTTP_API
		}وإلا إذا(db.config->listeners[i].protocol == mp_http_api){
			http_api_شروع(&db.config->listeners[i]);
#پایان_اگر
		}
	}
	إذا(g_listensock == عدم){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to start any listening sockets, exiting.");
		أرجع 1;
	}

	mux_جمع_کن_listeners(g_listensock, g_listensock_شمار);
	أرجع MOSQ_ERR_SUCCESS;
}


فراغ listeners__stop(فراغ)
{
	mux__delete_listeners(g_listensock, g_listensock_شمار);

	لكل(صحيح i=0; i<db.config->listener_شمار; i++){
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		إذا(db.config->listeners[i].ws_context){
			lws_context_destroy(db.config->listeners[i].ws_context);
		}
		mosquitto_FREE(db.config->listeners[i].ws_protocol);
#پایان_اگر
#اگر_تعریف_شده WITH_UNIX_SOCKETS
		إذا(db.config->listeners[i].unix_socket_مسیر != عدم){
			unlink(db.config->listeners[i].unix_socket_مسیر);
		}
#پایان_اگر
#اگر_تعریف_شده WITH_HTTP_API
		إذا(db.config->listeners[i].mhd){
			http_api__stop(&db.config->listeners[i]);
		}
#پایان_اگر
	}

	لكل(صحيح i=0; i<g_listensock_شمار; i++){
		إذا(g_listensock[i].sock != INVALID_SOCKET){
			COMPAT_CLOSE(g_listensock[i].sock);
		}
	}
	mosquitto_FREE(g_listensock);
	g_listensock_شمار = 0;
	listensock_شاخص = 0;
}
