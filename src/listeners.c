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

#అన్తర్భావ "config.h"
#అన్తర్భావ "net_mosq.h"
#అన్తర్భావ "mosquitto_broker_internal.h"

స్థిర పూర్ణాంకం listensock_సూచీ = 0;
బాహ్య పూర్ణాంకం g_నిరంతరం;


శూన్య listener_సమితి_defaults(నిర్మాణం mosquitto__listener *listener)
{
	listener->disable_protocol_v3 = false;
	listener->disable_protocol_v4 = false;
	listener->disable_protocol_v5 = false;
	listener->max_connections = -1;
	listener->max_qos = 2;
	listener->max_topic_alias = 10;
	listener->max_topic_alias_broker = 10;
	listener->protocol = mp_mqtt;
	mosquitto_FREE(listener->mount_బిందువు);

	mosquitto_FREE(listener->security_options->acl_దత్తాంశం.acl_file);
	mosquitto_FREE(listener->security_options->password_దత్తాంశం.password_file);
	mosquitto_FREE(listener->security_options->psk_file);
	listener->security_options->allow_anonymous = -1;
	listener->security_options->allow_zero_నిడివి_clientid = true;
	mosquitto_FREE(listener->security_options->auto_id_ఉపసర్గ);
	listener->security_options->auto_id_ఉపసర్గ_len = 0;
#నిర్వచితమైతే WITH_TLS
	listener->require_certificate = false;
	listener->use_తటస్థం_as_username = false;
	listener->use_subject_as_username = false;
	listener->use_username_as_clientid = false;
	listener->disable_client_cert_date_checks = false;
#అయితే_ముగింపు

#ఒకవేళ defined(WITH_WEBSOCKETS) && (LWS_LIBRARY_సంస్కరణ_సంఖ్య >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN)
	కొరకు(పూర్ణాంకం i=0; i<listener->ws_origin_లెక్క; i++){
		mosquitto_FREE(listener->ws_origins[i]);
	}
	mosquitto_FREE(listener->ws_origins);
	listener->ws_origin_లెక్క = 0;
#అయితే_ముగింపు
}


శూన్య listeners__reload_all_certificates(శూన్య)
{
#నిర్వచితమైతే WITH_TLS
	కొరకు(పూర్ణాంకం i=0; i<db.config->listener_లెక్క; i++){
		నిర్మాణం mosquitto__listener *listener = &db.config->listeners[i];
		ఒకవేళ(listener->ssl_ctx && listener->certfile && listener->keyfile){
			పూర్ణాంకం rc = net_ఎక్కించు_certificates(listener);
			ఒకవేళ(rc){
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error when reloading certificate '%s' or key '%s'.",
						listener->certfile, listener->keyfile);
			}
		}
	}
#అయితే_ముగింపు
}


స్థిర పూర్ణాంకం listeners_ప్రారంభం_single_mqtt(నిర్మాణం mosquitto__listener *listener)
{
	నిర్మాణం mosquitto__listener_sock *listensock_new;

	ఒకవేళ(net__socket_listen(listener)){
		తిరిగి 1;
	}
	g_listensock_లెక్క += listener->sock_లెక్క;
	listensock_new = mosquitto_realloc(g_listensock, పరిమాణం(నిర్మాణం mosquitto__listener_sock)*(size_t)g_listensock_లెక్క);
	ఒకవేళ(!listensock_new){
		తిరిగి 1;
	}
	g_listensock = listensock_new;

	కొరకు(పూర్ణాంకం i=0; i<listener->sock_లెక్క; i++){
		ఒకవేళ(listener->socks[i] == INVALID_SOCKET){
			తిరిగి 1;
		}
		g_listensock[listensock_సూచీ].sock = listener->socks[i];
		g_listensock[listensock_సూచీ].listener = listener;
#ఒకవేళ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
		g_listensock[listensock_సూచీ].ident = id_listener;
#అయితే_ముగింపు
		listensock_సూచీ++;
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS


శూన్య listeners_కూడు_websockets(నిర్మాణం lws_context *ws_context, mosq_sock_t fd)
{
	నిర్మాణం mosquitto__listener *listener = ఖాళీ;
	నిర్మాణం mosquitto__listener_sock *listensock_new;

	/* Don't add more listeners after we've started the main loop */
	ఒకవేళ(g_నిరంతరం || ws_context == ఖాళీ){
		తిరిగి;
	}

	/* Find context */
	కొరకు(పూర్ణాంకం i=0; i<db.config->listener_లెక్క; i++){
		ఒకవేళ(db.config->listeners[i].ws_లోపల_init){
			listener = &db.config->listeners[i];
			ఆపు;
		}
	}
	ఒకవేళ(listener == ఖాళీ){
		తిరిగి;
	}

	g_listensock_లెక్క++;
	listensock_new = mosquitto_realloc(g_listensock, పరిమాణం(నిర్మాణం mosquitto__listener_sock)*(size_t)g_listensock_లెక్క);
	ఒకవేళ(!listensock_new){
		తిరిగి;
	}
	g_listensock = listensock_new;

	g_listensock[listensock_సూచీ].sock = fd;
	g_listensock[listensock_సూచీ].listener = listener;
#ఒకవేళ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	g_listensock[listensock_సూచీ].ident = id_listener_ws;
#అయితే_ముగింపు
	listensock_సూచీ++;
}
#అయితే_ముగింపు


స్థిర పూర్ణాంకం listeners_కూడు_local(స్థిరరాశి అక్షరం *host, uint16_t పోర్టు)
{
	నిర్మాణం mosquitto__listener *listeners;
	bool allow_anonymous;

	listeners = db.config->listeners;
	ఒకవేళ(db.config->security_options.allow_anonymous == -1){
		allow_anonymous = true;
	}లేకపోతే{
		allow_anonymous = db.config->security_options.allow_anonymous;
	}

	listeners[db.config->listener_లెక్క].security_options = mosquitto_calloc(1, పరిమాణం(నిర్మాణం mosquitto__security_options));
	ఒకవేళ(listeners[db.config->listener_లెక్క].security_options == ఖాళీ){
		తిరిగి MOSQ_ERR_NOMEM;
	}

	listener_సమితి_defaults(&listeners[db.config->listener_లెక్క]);
	listeners[db.config->listener_లెక్క].security_options->allow_anonymous = allow_anonymous;
	listeners[db.config->listener_లెక్క].security_options->auto_id_ఉపసర్గ = mosquitto_strdup("auto-");
	ఒకవేళ(listeners[db.config->listener_లెక్క].security_options->auto_id_ఉపసర్గ == ఖాళీ){
		mosquitto_FREE(listeners[db.config->listener_లెక్క].security_options);
		తిరిగి MOSQ_ERR_NOMEM;
	}
	listeners[db.config->listener_లెక్క].security_options->auto_id_ఉపసర్గ_len = (పూర్ణాంకం)అక్షరశ్రేణి_పొడవు("auto-");
	listeners[db.config->listener_లెక్క].పోర్టు = పోర్టు;
	listeners[db.config->listener_లెక్క].host = mosquitto_strdup(host);
	ఒకవేళ(listeners[db.config->listener_లెక్క].host == ఖాళీ){
		mosquitto_FREE(listeners[db.config->listener_లెక్క].security_options->auto_id_ఉపసర్గ);
		mosquitto_FREE(listeners[db.config->listener_లెక్క].security_options);
		తిరిగి MOSQ_ERR_NOMEM;
	}
	ఒకవేళ(listeners_ప్రారంభం_single_mqtt(&listeners[db.config->listener_లెక్క])){
		mosquitto_FREE(listeners[db.config->listener_లెక్క].security_options->auto_id_ఉపసర్గ);
		mosquitto_FREE(listeners[db.config->listener_లెక్క].security_options);
		mosquitto_FREE(listeners[db.config->listener_లెక్క].host);
		తిరిగి MOSQ_ERR_UNKNOWN;
	}
	db.config->listener_లెక్క++;
	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం listeners_ప్రారంభం_local_only(శూన్య)
{
	/* Attempt to open listeners bound to 127.0.0.1 and ::1 only */
	పూర్ణాంకం rc;
	నిర్మాణం mosquitto__listener *listeners;
	size_t లెక్క;

	ఒకవేళ(db.config->cmd_పోర్టు_లెక్క == 0){
		లెక్క = 2;
	}లేకపోతే{
		లెక్క = (size_t)(db.config->cmd_పోర్టు_లెక్క*2);
	}

#నిర్వచితమైతే WITH_HTTP_API
	లెక్క++;
#అయితే_ముగింపు

	listeners = mosquitto_realloc(db.config->listeners, లెక్క*పరిమాణం(నిర్మాణం mosquitto__listener));
	ఒకవేళ(listeners == ఖాళీ){
		తిరిగి MOSQ_ERR_NOMEM;
	}
	memset(listeners, 0, లెక్క*పరిమాణం(నిర్మాణం mosquitto__listener));
	db.config->listener_లెక్క = 0;
	db.config->listeners = listeners;

	log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Starting in local only mode. Connections will only be possible from clients running on this machine.");
	log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Create a configuration file which defines a listener to allow remote access.");
	log__printf(ఖాళీ, MOSQ_LOG_WARNING, "For more details see https://mosquitto.org/documentation/authentication-methods/");
	ఒకవేళ(db.config->cmd_పోర్టు_లెక్క == 0){
		rc = listeners_కూడు_local("127.0.0.1", 1883);
		ఒకవేళ(rc == MOSQ_ERR_NOMEM){
			తిరిగి MOSQ_ERR_NOMEM;
		}
		rc = listeners_కూడు_local("::1", 1883);
		ఒకవేళ(rc == MOSQ_ERR_NOMEM){
			తిరిగి MOSQ_ERR_NOMEM;
		}
	}లేకపోతే{
		కొరకు(పూర్ణాంకం i=0; i<db.config->cmd_పోర్టు_లెక్క; i++){
			rc = listeners_కూడు_local("127.0.0.1", db.config->cmd_పోర్టు[i]);
			ఒకవేళ(rc == MOSQ_ERR_NOMEM){
				తిరిగి MOSQ_ERR_NOMEM;
			}
			rc = listeners_కూడు_local("::1", db.config->cmd_పోర్టు[i]);
			ఒకవేళ(rc == MOSQ_ERR_NOMEM){
				తిరిగి MOSQ_ERR_NOMEM;
			}
		}
	}

	ఒకవేళ(db.config->listener_లెక్క > 0){
#నిర్వచితమైతే WITH_HTTP_API
		db.config->listener_లెక్క++;
		http_api_ప్రారంభం_local(&db.config->listeners[db.config->listener_లెక్క-1]);
#అయితే_ముగింపు
		తిరిగి MOSQ_ERR_SUCCESS;
	}లేకపోతే{
		తిరిగి MOSQ_ERR_UNKNOWN;
	}
}


పూర్ణాంకం listeners_ప్రారంభం(శూన్య)
{
	g_listensock_లెక్క = 0;

	ఒకవేళ(db.config->local_only){
		ఒకవేళ(listeners_ప్రారంభం_local_only()){
			db__close();
			ఒకవేళ(db.config->pid_file){
				(శూన్య)remove(db.config->pid_file);
			}
			తిరిగి 1;
		}
		mux_కూడు_listeners(g_listensock, g_listensock_లెక్క);
		తిరిగి MOSQ_ERR_SUCCESS;
	}

	కొరకు(పూర్ణాంకం i=0; i<db.config->listener_లెక్క; i++){
		ఒకవేళ(db.config->listeners[i].protocol == mp_mqtt){
			ఒకవేళ(listeners_ప్రారంభం_single_mqtt(&db.config->listeners[i])){
				db__close();
				ఒకవేళ(db.config->pid_file){
					(శూన్య)remove(db.config->pid_file);
				}
				తిరిగి 1;
			}
		}లేకపోతే ఒకవేళ(db.config->listeners[i].protocol == mp_websockets){
#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			mosq_websockets_init(&db.config->listeners[i], db.config);
			ఒకవేళ(!db.config->listeners[i].ws_context){
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to create websockets listener on port %d.", db.config->listeners[i].పోర్టు);
				తిరిగి 1;
			}
#లేదావేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			ఒకవేళ(listeners_ప్రారంభం_single_mqtt(&db.config->listeners[i])){
				db__close();
				ఒకవేళ(db.config->pid_file){
					(శూన్య)remove(db.config->pid_file);
				}
				తిరిగి 1;
			}
#అయితే_ముగింపు
#నిర్వచితమైతే WITH_HTTP_API
		}లేకపోతే ఒకవేళ(db.config->listeners[i].protocol == mp_http_api){
			http_api_ప్రారంభం(&db.config->listeners[i]);
#అయితే_ముగింపు
		}
	}
	ఒకవేళ(g_listensock == ఖాళీ){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to start any listening sockets, exiting.");
		తిరిగి 1;
	}

	mux_కూడు_listeners(g_listensock, g_listensock_లెక్క);
	తిరిగి MOSQ_ERR_SUCCESS;
}


శూన్య listeners__stop(శూన్య)
{
	mux__delete_listeners(g_listensock, g_listensock_లెక్క);

	కొరకు(పూర్ణాంకం i=0; i<db.config->listener_లెక్క; i++){
#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ఒకవేళ(db.config->listeners[i].ws_context){
			lws_context_destroy(db.config->listeners[i].ws_context);
		}
		mosquitto_FREE(db.config->listeners[i].ws_protocol);
#అయితే_ముగింపు
#నిర్వచితమైతే WITH_UNIX_SOCKETS
		ఒకవేళ(db.config->listeners[i].unix_socket_మార్గం != ఖాళీ){
			unlink(db.config->listeners[i].unix_socket_మార్గం);
		}
#అయితే_ముగింపు
#నిర్వచితమైతే WITH_HTTP_API
		ఒకవేళ(db.config->listeners[i].mhd){
			http_api__stop(&db.config->listeners[i]);
		}
#అయితే_ముగింపు
	}

	కొరకు(పూర్ణాంకం i=0; i<g_listensock_లెక్క; i++){
		ఒకవేళ(g_listensock[i].sock != INVALID_SOCKET){
			COMPAT_CLOSE(g_listensock[i].sock);
		}
	}
	mosquitto_FREE(g_listensock);
	g_listensock_లెక్క = 0;
	listensock_సూచీ = 0;
}
