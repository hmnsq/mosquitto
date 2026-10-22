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

#అన్తర్భావ <limits.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <errno.h>

#నిర్వచితమైతే WIN32
#లేకపోతే
#  include <dirent.h>
#  include <strings.h>
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే WIN32
#  include <netdb.h>
#  include <sys/socket.h>
#లేకపోతే
#  include <winsock2.h>
#  include <ws2tcpip.h>
#అయితే_ముగింపు

#ఒకవేళ !defined(WIN32) && !defined(__CYGWIN__)
#  include <syslog.h>
#అయితే_ముగింపు

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "tls_mosq.h"
#అన్తర్భావ "util_mosq.h"
#అన్తర్భావ "mosquitto/mqtt_protocol.h"

#అన్తర్భావ "utlist.h"

నిర్మాణం config_recurse {
	గుర్తులేని పూర్ణాంకం log_dest;
	పూర్ణాంకం log_dest_సమితి;
	గుర్తులేని పూర్ణాంకం log_type;
	పూర్ణాంకం log_type_సమితి;
};

#ఒకవేళ defined(WIN32) || defined(__CYGWIN__)
#అన్తర్భావ <windows.h>
బాహ్య SERVICE_స్థితి_HANDLE service_handle;
#అయితే_ముగింపు


#నిర్వచించు REQUIRE_LISTENER(A) \
		చేయి{ \
			ఒకవేళ(cur_listener == ఖాళీ){ \
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				తిరిగి MOSQ_ERR_INVAL; \
			} \
		}అయితే(0)

#నిర్వచించు REQUIRE_LISTENER_OR_DEFAULT_LISTENER(A) \
		చేయి{ \
			ఒకవేళ(cur_listener == ఖాళీ){ \
				ఒకవేళ(config__create_default_listener(config, (A))){ \
					తిరిగి MOSQ_ERR_NOMEM; \
				} \
				cur_listener = config->default_listener; \
			} \
		}అయితే(0)

#నిర్వచించు REQUIRE_LISTENER_IF_చొప్పున_LISTENER(A) \
		చేయి{ \
			ఒకవేళ(config->చొప్పున_listener_settings == true && cur_listener == ఖాళీ){ \
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				తిరిగి MOSQ_ERR_INVAL; \
			} \
		}అయితే(0)

#నిర్వచించు REQUIRE_NON_DEFAULT_LISTENER(A) \
		చేయి{ \
			ఒకవేళ(cur_listener == config->default_listener || cur_listener == ఖాళీ){ \
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				తిరిగి MOSQ_ERR_INVAL; \
			} \
		}అయితే(0)

#నిర్వచించు REQUIRE_BRIDGE(A) \
		చేయి{ \
			ఒకవేళ(cur_bridge == ఖాళీ){ \
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: The '%s' option requires a bridge to be defined first.", (A)); \
				తిరిగి MOSQ_ERR_INVAL; \
			} \
		}అయితే(0)

#నిర్వచించు REQUIRE_PLUGIN(A) \
		చేయి{ \
			ఒకవేళ(cur_plugin == ఖాళీ){ \
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: The '%s' option requires plugin/global_plugin/plugin_load to be defined first.", (A)); \
				తిరిగి MOSQ_ERR_INVAL; \
			} \
		}అయితే(0)

#నిర్వచించు OPTION_DEPRECATED(A, B) \
		log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "The '%s' option is now deprecated and will be removed in version 3.0. %s", (A), (B))

#నిర్వచించు OPTION_UNAVAILABLE(A) \
		log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: The '%s' option is no longer available.", (A));

#నిర్వచించు REQUIRE_NON_ఖాళీలు_OPTION(A, B) \
		చేయి{ \
			ఒకవేళ(!(A)){ \
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", (B)); \
				తిరిగి MOSQ_ERR_INVAL; \
			} \
		}అయితే(0)

#నిర్వచించు చొప్పున_LISTENER_ALTERNATIVE(A, B) \
		ఒకవేళ(config->చొప్పున_listener_settings){ \
			log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "You are using the '%s' option with 'per_listener_settings true'. Please replace this with '%s'.", A, B); \
		} \


#నిర్వచితమైతే FINAL_WITH_TLS_PSK
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A) \
		చేయి{ \
			ఒకవేళ(cur_bridge->tls_psk_తటస్థం || cur_bridge->tls_psk){ \
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and psk encryption in a single bridge.", (A)); \
				తిరిగి MOSQ_ERR_INVAL; \
			} \
		}అయితే(0)

#  define REQUIRE_BRIDGE_NO_X509(A) \
		చేయి{ \
			ఒకవేళ(cur_bridge->tls_cafile || cur_bridge->tls_capath || cur_bridge->tls_certfile || cur_bridge->tls_keyfile){ \
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and identity encryption in a single bridge.", (A)); \
				తిరిగి MOSQ_ERR_INVAL; \
			} \
		}అయితే(0)

#లేకపోతే
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A)
#  define REQUIRE_BRIDGE_NO_X509(A)
#అయితే_ముగింపు

స్థిర నిర్మాణం mosquitto__security_options *cur_security_options = ఖాళీ;

స్థిర పూర్ణాంకం conf__parse_bool(అక్షరం **token, స్థిరరాశి అక్షరం *name, bool *విలువ, అక్షరం **saveptr);
స్థిర పూర్ణాంకం conf__parse_int(అక్షరం **token, స్థిరరాశి అక్షరం *name, పూర్ణాంకం *విలువ, అక్షరం **saveptr);
స్థిర పూర్ణాంకం conf__parse_ssize_t(అక్షరం **token, స్థిరరాశి అక్షరం *name, ssize_t *విలువ, అక్షరం **saveptr);
స్థిర పూర్ణాంకం conf__parse_string(అక్షరం **token, స్థిరరాశి అక్షరం *name, అక్షరం **విలువ, అక్షరం **saveptr);
స్థిర పూర్ణాంకం config__read_file(నిర్మాణం mosquitto__config *config, bool reload, స్థిరరాశి అక్షరం *file, నిర్మాణం config_recurse *config_tmp, పూర్ణాంకం level, పూర్ణాంకం *lineno);
స్థిర పూర్ణాంకం config__check(నిర్మాణం mosquitto__config *config);
స్థిర శూన్య config__cleanup_plugins(శూన్య);
#నిర్వచితమైతే WITH_BRIDGE
స్థిర పూర్ణాంకం config__check_bridges(నిర్మాణం mosquitto__config *config);
#అయితే_ముగింపు


స్థిర పూర్ణాంకం config_కూడు_listener(నిర్మాణం mosquitto__config *config)
{
	నిర్మాణం mosquitto__listener *listener;
	నిర్మాణం mosquitto__listener *new_listeners;
	పూర్ణాంకం def_listener = -1;

	ఒకవేళ(config->default_listener){
		కొరకు(పూర్ణాంకం i=0; i<config->listener_లెక్క; i++){
			ఒకవేళ(&config->listeners[i] == config->default_listener){
				def_listener = i;
				ఆపు;
			}
		}
	}
	new_listeners = mosquitto_realloc(config->listeners, పరిమాణం(నిర్మాణం mosquitto__listener)*(size_t)(config->listener_లెక్క+1));
	ఒకవేళ(!new_listeners){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
		తిరిగి MOSQ_ERR_NOMEM;
	}
	config->listeners = new_listeners;
	listener = &config->listeners[config->listener_లెక్క];
	memset(listener, 0, పరిమాణం(నిర్మాణం mosquitto__listener));
	listener->security_options = mosquitto_calloc(1, పరిమాణం(నిర్మాణం mosquitto__security_options));
	ఒకవేళ(!listener->security_options){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
		తిరిగి MOSQ_ERR_NOMEM;
	}
	ఒకవేళ(def_listener != -1){
		config->default_listener = &config->listeners[def_listener];
	}
	config->listener_లెక్క++;

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం config__create_default_listener(నిర్మాణం mosquitto__config *config, స్థిరరాశి అక్షరం *option_name)
{
	ఒకవేళ(config->default_listener){
		తిరిగి MOSQ_ERR_SUCCESS;
	}
	log__printf(ఖాళీ, MOSQ_LOG_INFO, "Creating default listener due to '%s' option.", option_name);
	log__printf(ఖాళీ, MOSQ_LOG_INFO, "It is best practice to define a 'listener' first. Using the '%s' option without a listener will be disabled in the future.", option_name);
	ఒకవేళ(config_కూడు_listener(config)){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
		తిరిగి MOSQ_ERR_NOMEM;
	}

	config->default_listener = &config->listeners[config->listener_లెక్క-1];
	listener_సమితి_defaults(config->default_listener);
	config->default_listener->పోర్టు = 1883;

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర శూన్య conf_సమితి_cur_security_options(నిర్మాణం mosquitto__config *config, నిర్మాణం mosquitto__listener **cur_listener, నిర్మాణం mosquitto__security_options **security_options, స్థిరరాశి అక్షరం *option_name)
{
	ఒకవేళ(config->చొప్పున_listener_settings){
		ఒకవేళ(*cur_listener == ఖాళీ){
			ఒకవేళ(config__create_default_listener(config, option_name)){
				తిరిగి;
			}
			*cur_listener = config->default_listener;
		}
		(*security_options) = (*cur_listener)->security_options;
	}లేకపోతే{
		(*security_options) = &config->security_options;
	}
}


స్థిర పూర్ణాంకం conf__attempt_resolve(స్థిరరాశి అక్షరం *host, స్థిరరాశి అక్షరం *పాఠ్యం, గుర్తులేని పూర్ణాంకం సంవర్గమానం, స్థిరరాశి అక్షరం *సందేశం)
{
	నిర్మాణం addrinfo gai_hints;
	నిర్మాణం addrinfo *gai_res;
	పూర్ణాంకం rc;

	memset(&gai_hints, 0, పరిమాణం(నిర్మాణం addrinfo));
	gai_hints.ai_family = AF_UNSPEC;
	gai_hints.ai_socktype = SOCK_STREAM;
	gai_res = ఖాళీ;
	rc = getaddrinfo(host, ఖాళీ, &gai_hints, &gai_res);
	ఒకవేళ(gai_res){
		freeaddrinfo(gai_res);
	}
	ఒకవేళ(rc != 0){
#నిర్వచితం_కాకపోతే WIN32
		ఒకవేళ(rc == EAI_SYSTEM){
			ఒకవేళ(errno == ENOENT){
				log__printf(ఖాళీ, సంవర్గమానం, "%s: Unable to resolve %s %s.", సందేశం, పాఠ్యం, host);
			}లేకపోతే{
				log__printf(ఖాళీ, సంవర్గమానం, "%s: Error resolving %s: %s.", సందేశం, పాఠ్యం, strerror(errno));
			}
		}లేకపోతే{
			log__printf(ఖాళీ, సంవర్గమానం, "%s: Error resolving %s: %s.", సందేశం, పాఠ్యం, gai_strerror(rc));
		}
#లేకపోతే
		ఒకవేళ(rc == WSAHOST_NOT_దొరికింది){
			log__printf(ఖాళీ, సంవర్గమానం, "%s: Error resolving %s.", సందేశం, పాఠ్యం);
		}
#అయితే_ముగింపు
		తిరిగి MOSQ_ERR_INVAL;
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర శూన్య config__init_reload(నిర్మాణం mosquitto__config *config)
{
	/* Set defaults */
	కొరకు(పూర్ణాంకం i=0; i<config->listener_లెక్క; i++){
		listener_సమితి_defaults(&config->listeners[i]);
	}

	config->local_only = true;
	config->allow_duplicate_messages = true;

	mosquitto_FREE(config->security_options.acl_దత్తాంశం.acl_file);
	mosquitto_FREE(config->security_options.password_దత్తాంశం.password_file);
	mosquitto_FREE(config->security_options.psk_file);

	config->security_options.allow_anonymous = -1;
	config->security_options.allow_zero_నిడివి_clientid = true;
	config->security_options.auto_id_ఉపసర్గ = ఖాళీ;
	config->security_options.auto_id_ఉపసర్గ_len = 0;

	config->autosave_interval = 1800;
	config->autosave_on_changes = false;

	mosquitto_FREE(config->clientid_prefixes);

	config->connection_messages = true;
	config->clientid_prefixes = ఖాళీ;
	config->చొప్పున_listener_settings = false;
	ఒకవేళ(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = ఖాళీ;
	}
	mosquitto_FREE(config->log_file);

#ఒకవేళ defined(WIN32) || defined(__CYGWIN__)
	ఒకవేళ(service_handle){
		/* This is running as a Windows service. Default to no logging. Using
		 * stdout/stderr is forbidden because the first clients to connect will
		 * get log information sent to them for some reason. */
		config->log_dest = MQTT3_LOG_NONE;
	}లేకపోతే{
		config->log_dest = MQTT3_LOG_STDERR;
	}
#లేకపోతే
	config->log_facility = LOG_DAEMON;
	config->log_dest = MQTT3_LOG_STDERR | MQTT3_LOG_DLT;
	ఒకవేళ(db.quiet){
		config->log_type = 0;
	}లేకపోతే ఒకవేళ(db.verbose){
		config->log_type = UINT_MAX;
	}లేకపోతే{
		config->log_type = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;
	}
#అయితే_ముగింపు
	config->log_timestamp = true;
	mosquitto_FREE(config->log_timestamp_format);
	config->global_max_clients = -1;
	config->global_max_connections = -1;
	config->log_timestamp_format = ఖాళీ;
	config->max_keepalive = 0;
	config->max_packet_కొలత = 2000000;
	config->max_inflight_messages = 20;
	config->max_queued_messages = 1000;
	config->max_inflight_బైట్లు = 0;
	config->max_queued_బైట్లు = 0;
	config->persistence = false;
	mosquitto_FREE(config->persistence_location);
	mosquitto_FREE(config->persistence_file);
	config->persistent_client_expiration = 0;
	config->వరుస_qos0_messages = false;
	config->retain_అందుబాటు = true;
	config->retain_expiry_interval = 0;
	config->సమితి_tcp_nodelay = false;
	config->sys_interval = 10;
	config->upgrade_outgoing_qos = false;
	config->packet_బఫరు_కొలత = 4096;

	config->packet_max_auth = 100000;
	config->packet_max_connect = 100000;
	config->packet_max_sub = 100000;
	config->packet_max_simple = 10000;
}


స్థిర శూన్య config__cleanup_plugin_config(mosquitto_plugin_id_t *plugin)
{
	mosquitto_FREE(plugin->config.మార్గం);
	mosquitto_FREE(plugin->config.name);

	ఒకవేళ(plugin->config.options){
		కొరకు(పూర్ణాంకం j=0; j<plugin->config.option_లెక్క; j++){
			mosquitto_FREE(plugin->config.options[j].కీలకం);
			mosquitto_FREE(plugin->config.options[j].విలువ);
		}
		mosquitto_FREE(plugin->config.options);
		plugin->config.option_లెక్క = 0;
	}
	mosquitto_FREE(plugin->config.security_options);
	mosquitto_FREE(plugin);
}


స్థిర శూన్య config__cleanup_plugins(శూన్య)
{
	కొరకు(పూర్ణాంకం i=0; i<db.plugin_లెక్క; i++){
		config__cleanup_plugin_config(db.plugins[i]);
	}
	mosquitto_FREE(db.plugins);
}


శూన్య config__init(నిర్మాణం mosquitto__config *config)
{
	memset(config, 0, పరిమాణం(నిర్మాణం mosquitto__config));
	config__init_reload(config);

	config->daemon = false;
}


శూన్య config__cleanup(నిర్మాణం mosquitto__config *config)
{
	mosquitto_FREE(config->clientid_prefixes);
	mosquitto_FREE(config->persistence_location);
	mosquitto_FREE(config->persistence_file);
	mosquitto_FREE(config->persistence_filepath);
	mosquitto_FREE(config->security_options.auto_id_ఉపసర్గ);
	mosquitto_FREE(config->security_options.acl_దత్తాంశం.acl_file);
	mosquitto_FREE(config->security_options.password_దత్తాంశం.password_file);
	mosquitto_FREE(config->security_options.psk_file);
	mosquitto_FREE(config->security_options.plugins);
	mosquitto_FREE(config->pid_file);
	mosquitto_FREE(config->user);
	mosquitto_FREE(config->log_timestamp_format);
	ఒకవేళ(config->listeners){
		కొరకు(పూర్ణాంకం i=0; i<config->listener_లెక్క; i++){
			mosquitto_FREE(config->listeners[i].host);
			mosquitto_FREE(config->listeners[i].bind_interface);
			mosquitto_FREE(config->listeners[i].mount_బిందువు);
			mosquitto_FREE(config->listeners[i].socks);
			ఒకవేళ(config->listeners[i].security_options){
				mosquitto_FREE(config->listeners[i].security_options->auto_id_ఉపసర్గ);
				mosquitto_FREE(config->listeners[i].security_options->acl_దత్తాంశం.acl_file);
				mosquitto_FREE(config->listeners[i].security_options->password_దత్తాంశం.password_file);
				mosquitto_FREE(config->listeners[i].security_options->psk_file);
				mosquitto_FREE(config->listeners[i].security_options->plugins);
				mosquitto_FREE(config->listeners[i].security_options);
			}
#నిర్వచితమైతే WITH_TLS
			mosquitto_FREE(config->listeners[i].cafile);
			mosquitto_FREE(config->listeners[i].capath);
			mosquitto_FREE(config->listeners[i].certfile);
			mosquitto_FREE(config->listeners[i].keyfile);
			mosquitto_FREE(config->listeners[i].ciphers);
			mosquitto_FREE(config->listeners[i].ciphers_tls13);
			mosquitto_FREE(config->listeners[i].psk_hint);
			mosquitto_FREE(config->listeners[i].crlfile);
			mosquitto_FREE(config->listeners[i].tls_సంస్కరణ);
			mosquitto_FREE(config->listeners[i].tls_engine);
			mosquitto_FREE(config->listeners[i].tls_engine_kpass_sha1);
#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			ఒకవేళ(!config->listeners[i].ws_context) /* libwebsockets frees its own SSL_CTX */
#అయితే_ముగింపు
			{
				SSL_CTX_free(config->listeners[i].ssl_ctx);
				config->listeners[i].ssl_ctx = ఖాళీ;
			}
#అయితే_ముగింపు
#ఒకవేళ defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
			mosquitto_FREE(config->listeners[i].http_dir);
#అయితే_ముగింపు
#నిర్వచితమైతే WITH_WEBSOCKETS
			కొరకు(పూర్ణాంకం j=0; j<config->listeners[i].ws_origin_లెక్క; j++){
				mosquitto_FREE(config->listeners[i].ws_origins[j]);
			}
			mosquitto_FREE(config->listeners[i].ws_origins);
#అయితే_ముగింపు
#నిర్వచితమైతే WITH_UNIX_SOCKETS
			mosquitto_FREE(config->listeners[i].unix_socket_మార్గం);
#అయితే_ముగింపు
		}
		mosquitto_FREE(config->listeners);
	}
#నిర్వచితమైతే WITH_BRIDGE
	ఒకవేళ(config->bridges){
		కొరకు(పూర్ణాంకం i=0; i<config->bridge_లెక్క; i++){
			config__bridge_cleanup(config->bridges[i]);
		}
		mosquitto_FREE(config->bridges);
	}
#అయితే_ముగింపు
	config__cleanup_plugins();

	ఒకవేళ(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = ఖాళీ;
	}
	ఒకవేళ(config->log_file){
		mosquitto_FREE(config->log_file);
		config->log_file = ఖాళీ;
	}
}

#నిర్వచితమైతే WITH_BRIDGE


శూన్య config__bridge_cleanup(నిర్మాణం mosquitto__bridge *bridge)
{
	ఒకవేళ(bridge == ఖాళీ){
		తిరిగి;
	}

	mosquitto_FREE(bridge->name);
	ఒకవేళ(bridge->చిరునామాలు){
		కొరకు(పూర్ణాంకం i=0; i<bridge->చిరునామా_లెక్క; i++){
			mosquitto_FREE(bridge->చిరునామాలు[i].చిరునామా);
		}
		mosquitto_FREE(bridge->చిరునామాలు);
	}
	mosquitto_FREE(bridge->bind_చిరునామా);
	mosquitto_FREE(bridge->remote_clientid);
	mosquitto_FREE(bridge->remote_username);
	mosquitto_FREE(bridge->remote_password);
	mosquitto_FREE(bridge->local_clientid);
	mosquitto_FREE(bridge->local_username);
	mosquitto_FREE(bridge->local_password);
	ఒకవేళ(bridge->topics){
		నిర్మాణం mosquitto__bridge_topic *cur_topic, *topic_tmp;

		LL_FOREACH_సురక్షిత(bridge->topics, cur_topic, topic_tmp){
			mosquitto_FREE(cur_topic->topic);
			mosquitto_FREE(cur_topic->local_ఉపసర్గ);
			mosquitto_FREE(cur_topic->remote_ఉపసర్గ);
			mosquitto_FREE(cur_topic->local_topic);
			mosquitto_FREE(cur_topic->remote_topic);
			LL_DELETE(bridge->topics, cur_topic);
			mosquitto_FREE(cur_topic);
		}
		mosquitto_FREE(bridge->topics);
	}
	mosquitto_FREE(bridge->notification_topic);
#నిర్వచితమైతే WITH_TLS
	mosquitto_FREE(bridge->tls_certfile);
	mosquitto_FREE(bridge->tls_keyfile);
	mosquitto_FREE(bridge->tls_సంస్కరణ);
	mosquitto_FREE(bridge->tls_cafile);
	mosquitto_FREE(bridge->tls_capath);
	mosquitto_FREE(bridge->tls_alpn);
	mosquitto_FREE(bridge->tls_ciphers);
	mosquitto_FREE(bridge->tls_13_ciphers);
#నిర్వచితమైతే FINAL_WITH_TLS_PSK
	mosquitto_FREE(bridge->tls_psk_తటస్థం);
	mosquitto_FREE(bridge->tls_psk);
#అయితే_ముగింపు
#అయితే_ముగింపు
	mosquitto_FREE(bridge);
}
#అయితే_ముగింపు


స్థిర శూన్య print_సంస్కరణ(శూన్య)
{
	రాయి("mosquitto %s\n", సంస్కరణ);
	రాయి("Copyright © 2025 Roger Light.\n");
	రాయి("License EPL-2.0 OR BSD-3-Clause.\n");
}


స్థిర శూన్య print_usage(శూన్య)
{
	రాయి("mosquitto version %s\n\n", సంస్కరణ);
	రాయి("mosquitto is an MQTT v5.0/v3.1.1/v3.1 broker.\n\n");
	రాయి("Usage: mosquitto [-c config_file] [-d] [-h] [-p port] [-v]\n");
	రాయి("                 [--tls-keylog file]\n\n");
	రాయి(" -c : specify the broker config file.\n");
	రాయి(" -d : put the broker into the background after starting.\n");
	రాయి(" -h : display this help.\n");
	రాయి(" -p : start the broker listening on the specified port.\n");
	రాయి("      Not recommended in conjunction with the -c option.\n");
	రాయి(" -q : quiet mode - disable all logging types. This overrides\n");
	రాయి("      any logging options given in the config file, and -v.\n");
	రాయి(" -v : verbose mode - enable all logging types. This overrides\n");
	రాయి("      any logging options given in the config file.\n");
	రాయి(" --test-config : test config file and exit\n");
	రాయి(" --tls-keylog : log TLS connection information to a file, to allow\n");
	రాయి("      debugging with e.g. wireshark. Do not use on a production\n");
	రాయి("      server.\n");
	రాయి("\nSee https://mosquitto.org/ for more information.\n\n");
}


పూర్ణాంకం config_parse_వాదనలు(నిర్మాణం mosquitto__config *config, పూర్ణాంకం argc, అక్షరం *argv[])
{
	పూర్ణాంకం i;
	పూర్ణాంకం పోర్టు_tmp;

	కొరకు(i=1; i<argc; i++){
		ఒకవేళ(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--config-file")){
			ఒకవేళ(i<argc-1){
				db.config_file = argv[i+1];

				ఒకవేళ(config__read(config, false)){
					తిరిగి MOSQ_ERR_INVAL;
				}
			}లేకపోతే{
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: -c argument given, but no config file specified.");
				తిరిగి MOSQ_ERR_INVAL;
			}
			i++;
		}లేకపోతే ఒకవేళ(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--daemon")){
			config->daemon = true;
		}లేకపోతే ఒకవేళ(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
			print_usage();
			తిరిగి MOSQ_ERR_UNKNOWN;
		}లేకపోతే ఒకవేళ(!strcmp(argv[i], "--version")){
			print_సంస్కరణ();
			తిరిగి MOSQ_ERR_UNKNOWN;
		}లేకపోతే ఒకవేళ(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port")){
			ఒకవేళ(i<argc-1){
				పోర్టు_tmp = atoi(argv[i+1]);
				ఒకవేళ(పోర్టు_tmp<1 || పోర్టు_tmp>UINT16_MAX){
					log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid port specified (%d).", పోర్టు_tmp);
					తిరిగి MOSQ_ERR_INVAL;
				}లేకపోతే{
					ఒకవేళ(config->cmd_పోర్టు_లెక్క == CMD_పోర్టు_హద్దు){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Only %d ports can be specified on the command line.", CMD_పోర్టు_హద్దు);
						తిరిగి MOSQ_ERR_INVAL;
					}
					config->cmd_పోర్టు[config->cmd_పోర్టు_లెక్క] = (uint16_t)పోర్టు_tmp;
					config->cmd_పోర్టు_లెక్క++;
				}
			}లేకపోతే{
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: -p argument given, but no port specified.");
				తిరిగి MOSQ_ERR_INVAL;
			}
			i++;
		}లేకపోతే ఒకవేళ(!strcmp(argv[i], "--tls-keylog")){
#నిర్వచితమైతే WITH_TLS
			ఒకవేళ(i<argc-1){
				db.tls_keylog = mosquitto_strdup(argv[i+1]);
				ఒకవేళ(db.tls_keylog == ఖాళీ){
					log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
					తిరిగి MOSQ_ERR_NOMEM;
				}
			}లేకపోతే{
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: --tls-keylog argument given, but no file specified.");
				తిరిగి MOSQ_ERR_INVAL;
			}
			i++;
#లేకపోతే
			fprintf(stderr, "Error: TLS support not available so --tls-keylog is not available.\n");
			తిరిగి MOSQ_ERR_INVAL;
#అయితే_ముగింపు
		}లేకపోతే ఒకవేళ(!strcmp(argv[i], "-q") || !strcmp(argv[i], "--quiet")){
			db.quiet = true;
		}లేకపోతే ఒకవేళ(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")){
			db.verbose = true;
		}లేకపోతే ఒకవేళ(!strcmp(argv[i], "--test-config")){
			config->test_configuration = true;
		}లేకపోతే{
			fprintf(stderr, "Error: Unknown option '%s'.\n", argv[i]);
			print_usage();
			తిరిగి MOSQ_ERR_INVAL;
		}
	}

	/* Default to drop to mosquitto user if we are privileged and no user specified. */
	ఒకవేళ(!config->user){
		config->user = mosquitto_strdup("mosquitto");
		ఒకవేళ(config->user == ఖాళీ){
			తిరిగి MOSQ_ERR_NOMEM;
		}
	}
	ఒకవేళ(db.quiet){
		config->log_type = 0;
	}లేకపోతే ఒకవేళ(db.verbose){
		config->log_type = UINT_MAX;
	}

	ఒకవేళ(getenv("MOSQUITTO_PERSISTENCE_LOCATION")){
		mosquitto_FREE(config->persistence_location);
		config->persistence_location = mosquitto_strdup(getenv("MOSQUITTO_PERSISTENCE_LOCATION"));
		ఒకవేళ(!config->persistence_location){
			తిరిగి MOSQ_ERR_NOMEM;
		}
	}
	తిరిగి config__check(config);
}


స్థిర శూన్య config__copy(నిర్మాణం mosquitto__config *src, నిర్మాణం mosquitto__config *dest)
{
	mosquitto_FREE(dest->security_options.acl_దత్తాంశం.acl_file);
	dest->security_options.acl_దత్తాంశం.acl_file = src->security_options.acl_దత్తాంశం.acl_file;

	acl_file__cleanup(&dest->security_options.acl_దత్తాంశం);
	dest->security_options.acl_దత్తాంశం.acl_users = src->security_options.acl_దత్తాంశం.acl_users;
	dest->security_options.acl_దత్తాంశం.acl_patterns = src->security_options.acl_దత్తాంశం.acl_patterns;
	dest->security_options.acl_దత్తాంశం.acl_anon.username = src->security_options.acl_దత్తాంశం.acl_anon.username;
	dest->security_options.acl_దత్తాంశం.acl_anon.acl = src->security_options.acl_దత్తాంశం.acl_anon.acl;

	dest->security_options.allow_anonymous = src->security_options.allow_anonymous;
	dest->security_options.allow_zero_నిడివి_clientid = src->security_options.allow_zero_నిడివి_clientid;

	mosquitto_FREE(dest->security_options.auto_id_ఉపసర్గ);
	dest->security_options.auto_id_ఉపసర్గ = src->security_options.auto_id_ఉపసర్గ;
	dest->security_options.auto_id_ఉపసర్గ_len = src->security_options.auto_id_ఉపసర్గ_len;

	mosquitto_FREE(dest->security_options.password_దత్తాంశం.password_file);
	dest->security_options.password_దత్తాంశం.password_file = src->security_options.password_దత్తాంశం.password_file;

	password_file__cleanup(&dest->security_options.password_దత్తాంశం);
	dest->security_options.password_దత్తాంశం.unpwd = src->security_options.password_దత్తాంశం.unpwd;

	mosquitto_FREE(dest->security_options.psk_file);
	dest->security_options.psk_file = src->security_options.psk_file;

	mosquitto_FREE(dest->security_options.plugins);
	dest->security_options.plugin_లెక్క = src->security_options.plugin_లెక్క;
	dest->security_options.plugins = src->security_options.plugins;

	dest->allow_duplicate_messages = src->allow_duplicate_messages;


	dest->autosave_interval = src->autosave_interval;
	dest->autosave_on_changes = src->autosave_on_changes;

	mosquitto_FREE(dest->clientid_prefixes);
	dest->clientid_prefixes = src->clientid_prefixes;

	dest->connection_messages = src->connection_messages;
	dest->log_dest = src->log_dest;
	dest->log_facility = src->log_facility;
	dest->log_type = src->log_type;
	dest->log_timestamp = src->log_timestamp;

	mosquitto_FREE(dest->log_timestamp_format);
	dest->log_timestamp_format = src->log_timestamp_format;

	mosquitto_FREE(dest->log_file);
	dest->log_file = src->log_file;

	dest->సందేశం_కొలత_హద్దు = src->సందేశం_కొలత_హద్దు;

	dest->persistence = src->persistence;

	mosquitto_FREE(dest->persistence_location);
	dest->persistence_location = src->persistence_location;

	mosquitto_FREE(dest->persistence_file);
	dest->persistence_file = src->persistence_file;

	mosquitto_FREE(dest->persistence_filepath);
	dest->persistence_filepath = src->persistence_filepath;

	dest->persistent_client_expiration = src->persistent_client_expiration;


	dest->వరుస_qos0_messages = src->వరుస_qos0_messages;
	dest->sys_interval = src->sys_interval;
	dest->upgrade_outgoing_qos = src->upgrade_outgoing_qos;

#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	dest->websockets_log_level = src->websockets_log_level;
#అయితే_ముగింపు

#నిర్వచితమైతే WITH_BRIDGE
	కొరకు(పూర్ణాంకం i=0; i<dest->bridge_లెక్క; i++){
		ఒకవేళ(dest->bridges[i]){
			config__bridge_cleanup(dest->bridges[i]);
		}
	}
	mosquitto_FREE(dest->bridges);
	dest->bridges = src->bridges;
	dest->bridge_లెక్క = src->bridge_లెక్క;
#అయితే_ముగింపు
}


పూర్ణాంకం config__read(నిర్మాణం mosquitto__config *config, bool reload)
{
	పూర్ణాంకం rc = MOSQ_ERR_SUCCESS;
	నిర్మాణం config_recurse cr;
	పూర్ణాంకం lineno = 0;
#నిర్వచితమైతే WITH_PERSISTENCE
	size_t len;
#అయితే_ముగింపు
	నిర్మాణం mosquitto__config config_reload;
	పూర్ణాంకం i;

	ఒకవేళ(reload){
		memset(&config_reload, 0, పరిమాణం(నిర్మాణం mosquitto__config));
	}

	cr.log_dest = MQTT3_LOG_NONE;
	cr.log_dest_సమితి = 0;
	cr.log_type = MOSQ_LOG_NONE;
	cr.log_type_సమితి = 0;

	ఒకవేళ(!db.config_file){
		తిరిగి 0;
	}

	ఒకవేళ(reload){
		/* Re-initialise appropriate config vars to default for reload. */
		config__init_reload(&config_reload);
		config_reload.listeners = config->listeners;
		config_reload.listener_లెక్క = config->listener_లెక్క;
		cur_security_options = ఖాళీ;
		rc = config__read_file(&config_reload, reload, db.config_file, &cr, 0, &lineno);
	}లేకపోతే{
		rc = config__read_file(config, reload, db.config_file, &cr, 0, &lineno);
	}
	ఒకవేళ(rc){
		ఒకవేళ(lineno > 0){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error found at %s:%d.", db.config_file, lineno);
		}
		తిరిగి rc;
	}

	ఒకవేళ(reload){
		config__copy(&config_reload, config);
	}

	/* If auth/access options are set and allow_anonymous not explicitly set, disallow anon. */
	ఒకవేళ(config->local_only == false){
		ఒకవేళ(config->చొప్పున_listener_settings){
			కొరకు(i=0; i<config->listener_లెక్క; i++){
				/* Default option if no security options set */
				ఒకవేళ(config->listeners[i].security_options->allow_anonymous == -1){
					config->listeners[i].security_options->allow_anonymous = false;
				}
			}
		}లేకపోతే{
			ఒకవేళ(config->security_options.allow_anonymous == -1){
				config->security_options.allow_anonymous = false;
			}
		}
	}
#నిర్వచితమైతే WITH_PERSISTENCE
	ఒకవేళ(config->persistence){
		ఒకవేళ(!config->persistence_file){
			config->persistence_file = mosquitto_strdup("mosquitto.db");
			ఒకవేళ(!config->persistence_file){
				తిరిగి MOSQ_ERR_NOMEM;
			}
		}
		mosquitto_FREE(config->persistence_filepath);
		ఒకవేళ(config->persistence_location && అక్షరశ్రేణి_పొడవు(config->persistence_location)){
			len = అక్షరశ్రేణి_పొడవు(config->persistence_location) + అక్షరశ్రేణి_పొడవు(config->persistence_file) + 2;
			config->persistence_filepath = mosquitto_malloc(len);
			ఒకవేళ(!config->persistence_filepath){
				తిరిగి MOSQ_ERR_NOMEM;
			}
#నిర్వచితమైతే WIN32
			snprintf(config->persistence_filepath, len, "%s\\%s", config->persistence_location, config->persistence_file);
#లేకపోతే
			snprintf(config->persistence_filepath, len, "%s/%s", config->persistence_location, config->persistence_file);
#అయితే_ముగింపు
		}లేకపోతే{
			config->persistence_filepath = mosquitto_strdup(config->persistence_file);
			ఒకవేళ(!config->persistence_filepath){
				తిరిగి MOSQ_ERR_NOMEM;
			}
		}
	}
#అయితే_ముగింపు
	/* Default to drop to mosquitto user if no other user specified. This must
	 * remain here even though it is covered in config__parse_args() because this
	 * function may be called on its own. */
	ఒకవేళ(!config->user){
		config->user = mosquitto_strdup("mosquitto");
	}

#నిర్వచితమైతే WITH_BRIDGE
	కొరకు(i=0; i<config->bridge_లెక్క; i++){
		ఒకవేళ(!config->bridges[i]->name){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: bridge name not defined.");
			తిరిగి MOSQ_ERR_INVAL;
		}
		ఒకవేళ(config->bridges[i]->చిరునామాలు  == 0){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no remote addresses defined.");
			తిరిగి MOSQ_ERR_INVAL;
		}
		ఒకవేళ(config->bridges[i]->topic_లెక్క == 0){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no topics defined.");
			తిరిగి MOSQ_ERR_INVAL;
		}
#నిర్వచితమైతే FINAL_WITH_TLS_PSK
		ఒకవేళ(config->bridges[i]->tls_psk && !config->bridges[i]->tls_psk_తటస్థం){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_identity.");
			తిరిగి MOSQ_ERR_INVAL;
		}
		ఒకవేళ(config->bridges[i]->tls_psk_తటస్థం && !config->bridges[i]->tls_psk){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_psk.");
			తిరిగి MOSQ_ERR_INVAL;
		}
#అయితే_ముగింపు
	}
#అయితే_ముగింపు

	ఒకవేళ(cr.log_dest_సమితి){
		config->log_dest = cr.log_dest;
	}
	ఒకవేళ(db.quiet){
		config->log_type = 0;
	}లేకపోతే ఒకవేళ(db.verbose){
		config->log_type = UINT_MAX;
	}లేకపోతే ఒకవేళ(cr.log_type_సమితి){
		config->log_type = cr.log_type;
	}
#నిర్వచితమైతే WITH_BRIDGE
	తిరిగి config__check_bridges(config);
#లేకపోతే
	తిరిగి MOSQ_ERR_SUCCESS;
#అయితే_ముగింపు
}


స్థిర mosquitto_plugin_id_t *config_plugin_కనుగొను(స్థిరరాశి అక్షరం *name)
{
	ఒకవేళ(db.plugins && name){
		కొరకు(పూర్ణాంకం i=0; i<db.plugin_లెక్క; i++){
			ఒకవేళ(db.plugins[i]->config.name && !strcmp(db.plugins[i]->config.name, name)){
				తిరిగి db.plugins[i];
			}
		}
	}
	తిరిగి ఖాళీ;
}


స్థిర mosquitto_plugin_id_t *config_plugin_ఎక్కించు(స్థిరరాశి అక్షరం *name, స్థిరరాశి అక్షరం *మార్గం)
{
	mosquitto_plugin_id_t *plugin = ఖాళీ;
	mosquitto_plugin_id_t **plugins = ఖాళీ;

	ఒకవేళ(name && config_plugin_కనుగొను(name)){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Duplicate plugin name '%s'.", name);
		తిరిగి ఖాళీ;
	}

	ఒకవేళ(!మార్గం || !strcmp(మార్గం, "")){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Missing plugin path for plugin name '%s'.", name);
		తిరిగి ఖాళీ;
	}

	plugin = mosquitto_calloc(1, పరిమాణం(mosquitto_plugin_id_t));
	ఒకవేళ(!plugin){
		వెళ్ళు error;
	}

	ఒకవేళ(name){
		plugin->config.name = mosquitto_strdup(name);
	}
	plugin->config.మార్గం = mosquitto_strdup(మార్గం);
	ఒకవేళ((name && !plugin->config.name) || !plugin->config.మార్గం){
		వెళ్ళు error;
	}
	plugin->config.options = ఖాళీ;
	plugin->config.option_లెక్క = 0;
	plugin->config.deny_special_chars = true;

	/* Add to db list */
	plugins = mosquitto_realloc(db.plugins, (size_t)(db.plugin_లెక్క+1)*పరిమాణం(నిర్మాణం mosquitto__plugin_config *));
	ఒకవేళ(!plugins){
		వెళ్ళు error;
	}

	plugins[db.plugin_లెక్క] = plugin;
	db.plugins = plugins;
	db.plugin_లెక్క++;

	తిరిగి plugin;
error:
	log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
	ఒకవేళ(plugin){
		mosquitto_FREE(plugin->config.name);
		mosquitto_FREE(plugin->config.మార్గం);
	}
	mosquitto_FREE(plugin);
	తిరిగి ఖాళీ;
}


పూర్ణాంకం config_plugin_కూడు_secopt(mosquitto_plugin_id_t *plugin, నిర్మాణం mosquitto__security_options *security_options)
{
	నిర్మాణం mosquitto__security_options **new_options;
	mosquitto_plugin_id_t **new_plugins;

	new_options = mosquitto_realloc(plugin->config.security_options, (size_t)(plugin->config.security_option_లెక్క+1)*పరిమాణం(నిర్మాణం mosquitto__security_options *));
	new_plugins = mosquitto_realloc(security_options->plugins, (size_t)(security_options->plugin_లెక్క+1)*పరిమాణం(mosquitto_plugin_id_t *));

	ఒకవేళ(!new_options || !new_plugins){
		mosquitto_FREE(new_options);
		mosquitto_FREE(new_plugins);
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
		తిరిగి MOSQ_ERR_NOMEM;
	}

	new_options[plugin->config.security_option_లెక్క] = security_options;
	plugin->config.security_options = new_options;
	plugin->config.security_option_లెక్క++;

	new_plugins[security_options->plugin_లెక్క] = plugin;
	security_options->plugins = new_plugins;
	security_options->plugin_లెక్క++;

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం config__read_file_core(నిర్మాణం mosquitto__config *config, bool reload, నిర్మాణం config_recurse *cr, పూర్ణాంకం level, పూర్ణాంకం *lineno, FILE *fptr, అక్షరం **buf, పూర్ణాంకం *buflen)
{
	పూర్ణాంకం rc;
	అక్షరం *token;
	పూర్ణాంకం tmp_int;
	అక్షరం *saveptr = ఖాళీ;
#నిర్వచితమైతే WITH_BRIDGE
	అక్షరం *tmp_char;
	నిర్మాణం mosquitto__bridge *cur_bridge = ఖాళీ;
#అయితే_ముగింపు
	mosquitto_plugin_id_t *cur_plugin = ఖాళీ;

	అక్షరం *కీలకం;
	నిర్మాణం mosquitto__listener *cur_listener = ఖాళీ;
	పూర్ణాంకం i;
	పూర్ణాంకం lineno_ext = 0;
	size_t ఉపసర్గ_len;
	size_t slen;
#నిర్వచితమైతే WITH_WEBSOCKETS
	అక్షరం **ws_origins = ఖాళీ;
#అయితే_ముగింపు

	*lineno = 0;

	అయితే(mosquitto_fgets(buf, buflen, fptr)){
		(*lineno)++;
		ఒకవేళ((*buf)[0] != '#' && (*buf)[0] != 10 && (*buf)[0] != 13){
			slen = అక్షరశ్రేణి_పొడవు(*buf);
			ఒకవేళ(slen == 0){
				కొనసాగు;
			}
			అయితే((*buf)[slen-1] == 10 || (*buf)[slen-1] == 13){
				(*buf)[slen-1] = 0;
				slen = అక్షరశ్రేణి_పొడవు(*buf);
				ఒకవేళ(slen == 0){
					కొనసాగు;
				}
			}
			token = strtok_r((*buf), " ", &saveptr);
			ఒకవేళ(token){
				ఒకవేళ(!strcmp(token, "accept_protocol_versions")){
					REQUIRE_NON_DEFAULT_LISTENER(token);
					cur_listener->disable_protocol_v3 = true;
					cur_listener->disable_protocol_v4 = true;
					cur_listener->disable_protocol_v5 = true;
					ఒకవేళ(saveptr == ఖాళీ){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", "accept_protocol_versions");
						తిరిగి MOSQ_ERR_INVAL;
					}
					token = strtok_r(saveptr, ", \t", &saveptr);
					REQUIRE_NON_ఖాళీలు_OPTION(token, "accept_protocol_versions");

					అయితే(token){
						ఒకవేళ(!strcmp(token, "3")){
							cur_listener->disable_protocol_v3 = false;
						}లేకపోతే ఒకవేళ(!strcmp(token, "4")){
							cur_listener->disable_protocol_v4 = false;
						}లేకపోతే ఒకవేళ(!strcmp(token, "5")){
							cur_listener->disable_protocol_v5 = false;
						}

						token = strtok_r(ఖాళీ, ", \t", &saveptr);
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "acl_file")){
					REQUIRE_LISTENER_IF_చొప్పున_LISTENER(token);

					conf_సమితి_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->acl_దత్తాంశం.acl_file);
					ఒకవేళ(conf__parse_string(&token, "acl_file", &cur_security_options->acl_దత్తాంశం.acl_file, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "address") || !strcmp(token, "addresses")){
#నిర్వచితమైతే WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ఒకవేళ(cur_bridge->చిరునామాలు){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration, 'address' only allowed once.");
						తిరిగి MOSQ_ERR_INVAL;
					}
					అయితే((token = strtok_r(ఖాళీ, " ", &saveptr))){
						ఒకవేళ(token[0] == '#'){
							ఆపు;
						}
						నిర్మాణం bridge_చిరునామా *new_చిరునామాలు = mosquitto_realloc(cur_bridge->చిరునామాలు, పరిమాణం(నిర్మాణం bridge_చిరునామా)*(size_t)(cur_bridge->చిరునామా_లెక్క+1));
						ఒకవేళ(!new_చిరునామాలు){
							log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
							తిరిగి MOSQ_ERR_NOMEM;
						}
						cur_bridge->చిరునామా_లెక్క++;
						cur_bridge->చిరునామాలు = new_చిరునామాలు;
						memset(&cur_bridge->చిరునామాలు[cur_bridge->చిరునామా_లెక్క-1], 0, పరిమాణం(నిర్మాణం bridge_చిరునామా));
						cur_bridge->చిరునామాలు[cur_bridge->చిరునామా_లెక్క-1].చిరునామా = mosquitto_strdup(token);
						ఒకవేళ(!cur_bridge->చిరునామాలు[cur_bridge->చిరునామా_లెక్క-1].చిరునామా){
							log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
							తిరిగి MOSQ_ERR_NOMEM;
						}
					}
					కొరకు(i=0; i<cur_bridge->చిరునామా_లెక్క; i++){
						/* cur_bridge->addresses[i].address is now
						 * "address[:port]". If address is an IPv6 address,
						 * then port is required. We must check for the :
						 * backwards. */
						tmp_char = strrchr(cur_bridge->చిరునామాలు[i].చిరునామా, ':');
						ఒకవేళ(tmp_char){
							/* Remove ':', so cur_bridge->addresses[i].address
							 * now just looks like the address. */
							tmp_char[0] = '\0';

							/* The remainder of the string */
							tmp_int = atoi(&tmp_char[1]);
							ఒకవేళ(tmp_int < 1 || tmp_int > UINT16_MAX){
								log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid bridge port value (%d).", tmp_int);
								తిరిగి MOSQ_ERR_INVAL;
							}
							cur_bridge->చిరునామాలు[i].పోర్టు = (uint16_t)tmp_int;
						}లేకపోతే{
							cur_bridge->చిరునామాలు[i].పోర్టు = 1883;
						}
						conf__attempt_resolve(cur_bridge->చిరునామాలు[i].చిరునామా, "bridge address", MOSQ_LOG_WARNING, "Warning");
					}
					ఒకవేళ(cur_bridge->చిరునామా_లెక్క == 0){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Empty address value in configuration.");
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "allow_anonymous")){
					REQUIRE_LISTENER_IF_చొప్పున_LISTENER(token);
					చొప్పున_LISTENER_ALTERNATIVE(token, "listener_allow_anonymous");
					conf_సమితి_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ఒకవేళ(conf__parse_bool(&token, "allow_anonymous", (bool *)&cur_security_options->allow_anonymous, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "allow_duplicate_messages")){
					OPTION_DEPRECATED(token, "The behaviour will default to true.");
					ఒకవేళ(conf__parse_bool(&token, "allow_duplicate_messages", &config->allow_duplicate_messages, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "allow_zero_length_clientid")){
					REQUIRE_LISTENER_IF_చొప్పున_LISTENER(token);
					conf_సమితి_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ఒకవేళ(conf__parse_bool(&token, "allow_zero_length_clientid", &cur_security_options->allow_zero_నిడివి_clientid, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!అక్షరశ్రేణి_పోలిక(token, "auth_opt_", అక్షరశ్రేణి_పొడవు("auth_opt_")) || !అక్షరశ్రేణి_పోలిక(token, "plugin_opt_", అక్షరశ్రేణి_పొడవు("plugin_opt_"))){
					ఒకవేళ(reload){
						కొనసాగు;        /* Auth plugin not currently valid for reloading. */

					}
					REQUIRE_PLUGIN(token);

					ఒకవేళ(!అక్షరశ్రేణి_పోలిక(token, "auth_opt_", అక్షరశ్రేణి_పొడవు("auth_opt_"))){
						ఉపసర్గ_len = అక్షరశ్రేణి_పొడవు("auth_opt_");
					}లేకపోతే{
						ఉపసర్గ_len = అక్షరశ్రేణి_పొడవు("plugin_opt_");
					}
					ఒకవేళ(అక్షరశ్రేణి_పొడవు(token) < ఉపసర్గ_len + 3){
						/* auth_opt_ == 9, + one digit key == 10, + one space == 11, + one value == 12 */
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid 'plugin_opt_' config option.");
						తిరిగి MOSQ_ERR_INVAL;
					}
					కీలకం = mosquitto_strdup(&token[ఉపసర్గ_len]);
					ఒకవేళ(!కీలకం){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
						తిరిగి MOSQ_ERR_NOMEM;
					}లేకపోతే ఒకవేళ(STREMPTY(కీలకం)){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Empty 'plugin_opt_' config option.");
						mosquitto_FREE(కీలకం);
						తిరిగి MOSQ_ERR_INVAL;
					}
					token = saveptr;
					ఒకవేళ(token && token[0]){
						అయితే(token[0] == ' ' || token[0] == '\t'){
							token++;
						}
						నిర్మాణం mosquitto_opt *new_options = mosquitto_realloc(cur_plugin->config.options, (size_t)(cur_plugin->config.option_లెక్క+1)*పరిమాణం(నిర్మాణం mosquitto_opt));
						ఒకవేళ(!new_options){
							log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
							mosquitto_FREE(కీలకం);
							తిరిగి MOSQ_ERR_NOMEM;
						}
						cur_plugin->config.option_లెక్క++;
						cur_plugin->config.options = new_options;
						cur_plugin->config.options[cur_plugin->config.option_లెక్క-1].కీలకం = కీలకం;
						cur_plugin->config.options[cur_plugin->config.option_లెక్క-1].విలువ = mosquitto_strdup(token);
						ఒకవేళ(!cur_plugin->config.options[cur_plugin->config.option_లెక్క-1].విలువ){
							log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
							తిరిగి MOSQ_ERR_NOMEM;
						}
					}లేకపోతే{
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", కీలకం);
						mosquitto_FREE(కీలకం);
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "auth_plugin") || !strcmp(token, "plugin") || !strcmp(token, "global_plugin")){
					ఒకవేళ(reload){
						కొనసాగు;        /* plugin not currently valid for reloading. */
					}
					ఒకవేళ(!strcmp(token, "global_plugin")){
						cur_security_options = &db.config->security_options;
					}లేకపోతే{
						REQUIRE_LISTENER_IF_చొప్పున_LISTENER(token);
						conf_సమితి_cur_security_options(config, &cur_listener, &cur_security_options, token);
					}

					cur_plugin = config_plugin_ఎక్కించు(ఖాళీ, saveptr);
					ఒకవేళ(cur_plugin == ఖాళీ){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(config_plugin_కూడు_secopt(cur_plugin, cur_security_options)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "auth_plugin_deny_special_chars")){
					ఒకవేళ(reload){
						కొనసాగు;        /* Auth plugin not currently valid for reloading. */
					}
					ఒకవేళ(!cur_plugin){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: An auth_plugin_deny_special_chars option exists in the config file without a plugin.");
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(conf__parse_bool(&token, "auth_plugin_deny_special_chars", &cur_plugin->config.deny_special_chars, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "plugin_load")){
					అక్షరం *name = ఖాళీ;

					ఒకవేళ(reload){
						కొనసాగు;        /* plugin not currently valid for reloading. */

					}
					name = strtok_r(ఖాళీ, " ", &saveptr);
					REQUIRE_NON_ఖాళీలు_OPTION(name, "plugin_load");

					cur_plugin = config_plugin_ఎక్కించు(name, saveptr);
					ఒకవేళ(cur_plugin == ఖాళీ){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "plugin_use")){
					అక్షరం *name = ఖాళీ;

					ఒకవేళ(reload){
						కొనసాగు;        /* plugin not currently valid for reloading. */
					}
					REQUIRE_NON_DEFAULT_LISTENER(token);

					ఒకవేళ(conf__parse_string(&token, "plugin_use", &name, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					cur_plugin = config_plugin_కనుగొను(name);
					ఒకవేళ(!cur_plugin){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Plugin '%s' not previously loaded.", name);
						mosquitto_FREE(name);
						తిరిగి MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(name);
					ఒకవేళ(config_plugin_కూడు_secopt(cur_plugin, cur_listener->security_options)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "auto_id_prefix")){
					REQUIRE_LISTENER_IF_చొప్పున_LISTENER(token);
					OPTION_DEPRECATED(token, "Please use 'listener_auto_id_prefix' instead.");
					conf_సమితి_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ఒకవేళ(conf__parse_string(&token, "auto_id_prefix", &cur_security_options->auto_id_ఉపసర్గ, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(cur_security_options->auto_id_ఉపసర్గ){
						ఒకవేళ(అక్షరశ్రేణి_పొడవు(cur_security_options->auto_id_ఉపసర్గ) > 50){
							log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: auto_id_prefix length must be <= 50.");
							తిరిగి MOSQ_ERR_INVAL;
						}
						cur_security_options->auto_id_ఉపసర్గ_len = (uint16_t)అక్షరశ్రేణి_పొడవు(cur_security_options->auto_id_ఉపసర్గ);
					}లేకపోతే{
						cur_security_options->auto_id_ఉపసర్గ_len = 0;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "autosave_interval")){
					ఒకవేళ(conf__parse_int(&token, "autosave_interval", &config->autosave_interval, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(config->autosave_interval < 0){
						config->autosave_interval = 0;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "autosave_on_changes")){
					ఒకవేళ(conf__parse_bool(&token, "autosave_on_changes", &config->autosave_on_changes, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "bind_address")){
					OPTION_DEPRECATED(token, "");
					config->local_only = false;
					ఒకవేళ(reload){
						కొనసాగు;        /* Rebinding listeners not valid during reloading. */

					}
					ఒకవేళ(config__create_default_listener(config, token)){
						తిరిగి MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					ఒకవేళ(conf__parse_string(&token, "default listener bind_address", &config->default_listener->host, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(conf__attempt_resolve(config->default_listener->host, "bind_address", MOSQ_LOG_ERR, "Error")){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "bind_interface")){
#నిర్వచితమైతే SO_BINDTODEVICE
					ఒకవేళ(reload){
						కొనసాగు;        /* Rebinding listeners not valid during reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ఒకవేళ(conf__parse_string(&token, "bind_interface", &cur_listener->bind_interface, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: bind_interface specified but socket option not available.");
					తిరిగి MOSQ_ERR_INVAL;
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "bridge_attempt_unsubscribe")){
#నిర్వచితమైతే WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_bool(&token, "bridge_attempt_unsubscribe", &cur_bridge->attempt_unsubscribe, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "bridge_cafile")){
#ఒకవేళ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					ఒకవేళ(conf__parse_string(&token, "bridge_cafile", &cur_bridge->tls_cafile, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "bridge_alpn")){
#ఒకవేళ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_string(&token, "bridge_alpn", &cur_bridge->tls_alpn, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "bridge_ciphers")){
#ఒకవేళ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_string(&token, "bridge_ciphers", &cur_bridge->tls_ciphers, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "bridge_ciphers_tls1.3")){
#ఒకవేళ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_string(&token, "bridge_ciphers_tls1.3", &cur_bridge->tls_13_ciphers, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "bridge_bind_address")){
#ఒకవేళ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_string(&token, "bridge_bind_address", &cur_bridge->bind_చిరునామా, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "bridge_capath")){
#ఒకవేళ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					ఒకవేళ(conf__parse_string(&token, "bridge_capath", &cur_bridge->tls_capath, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "bridge_certfile")){
#ఒకవేళ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					ఒకవేళ(conf__parse_string(&token, "bridge_certfile", &cur_bridge->tls_certfile, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "bridge_identity")){
#ఒకవేళ defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					ఒకవేళ(conf__parse_string(&token, "bridge_identity", &cur_bridge->tls_psk_తటస్థం, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "bridge_insecure")){
#ఒకవేళ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_bool(&token, "bridge_insecure", &cur_bridge->tls_insecure, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(cur_bridge->tls_insecure){
						log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge '%s' using insecure mode.", cur_bridge->name);
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "bridge_require_ocsp")){
#ఒకవేళ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_bool(&token, "bridge_require_ocsp", &cur_bridge->tls_ocsp_required, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "bridge_max_packet_size")){
#ఒకవేళ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_int(&token, "bridge_max_packet_size", &tmp_int, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(tmp_int < 0){
						tmp_int = 0;
					}
					cur_bridge->గరిష్ఠ_packet_కొలత = (uint32_t)tmp_int;
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "bridge_max_topic_alias")){
#నిర్వచితమైతే WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_int(&token, "bridge_max_topic_alias", &tmp_int, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}

					ఒకవేళ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: bridge_max_topic_alias must be > 0 and <= 65535.");
						తిరిగి MOSQ_ERR_INVAL;
					}
					cur_bridge->max_topic_alias = (uint16_t)tmp_int;
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "bridge_outgoing_retain")){
#ఒకవేళ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_bool(&token, "bridge_outgoing_retain", &cur_bridge->outgoing_retain, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "bridge_keyfile")){
#ఒకవేళ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					mosquitto_FREE(cur_bridge->tls_keyfile);
					ఒకవేళ(conf__parse_string(&token, "bridge_keyfile", &cur_bridge->tls_keyfile, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "bridge_protocol_version")){
#నిర్వచితమైతే WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(ఖాళీ, "", &saveptr);
					REQUIRE_NON_ఖాళీలు_OPTION(token, "bridge_protocol_version");

					ఒకవేళ(!strcmp(token, "mqttv31")){
						cur_bridge->protocol_సంస్కరణ = mosq_p_mqtt31;
					}లేకపోతే ఒకవేళ(!strcmp(token, "mqttv311")){
						cur_bridge->protocol_సంస్కరణ = mosq_p_mqtt311;
					}లేకపోతే ఒకవేళ(!strcmp(token, "mqttv50")){
						cur_bridge->protocol_సంస్కరణ = mosq_p_mqtt5;
					}లేకపోతే{
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid 'bridge_protocol_version' value (%s).", token);
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "bridge_psk")){
#ఒకవేళ defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					ఒకవేళ(conf__parse_string(&token, "bridge_psk", &cur_bridge->tls_psk, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "bridge_receive_maximum")){
#ఒకవేళ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_int(&token, "bridge_receive_maximum", &tmp_int, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(tmp_int <= 0){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be greater than 0.");
						తిరిగి MOSQ_ERR_INVAL;
					}లేకపోతే ఒకవేళ((uint64_t)tmp_int > (uint64_t)UINT16_MAX){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be lower than %u.", UINT16_MAX);
						తిరిగి MOSQ_ERR_INVAL;
					}
					cur_bridge->receive_గరిష్ఠ = (uint16_t)tmp_int;
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "bridge_reload_type")){
#నిర్వచితమైతే WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(ఖాళీ, " ", &saveptr);
					REQUIRE_NON_ఖాళీలు_OPTION(token, "bridge_reload_type");

					ఒకవేళ(!strcmp(token, "lazy")){
						cur_bridge->reload_type = brt_lazy;
					}లేకపోతే ఒకవేళ(!strcmp(token, "immediate")){
						cur_bridge->reload_type = brt_తక్షణ;
					}లేకపోతే{
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid 'bridge_reload_type' value in configuration (%s).", token);
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "bridge_session_expiry_interval")){
#ఒకవేళ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_int(&token, "bridge_session_expiry_interval", &tmp_int, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(tmp_int < 0){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must not be negative.");
						తిరిగి MOSQ_ERR_INVAL;
					}లేకపోతే ఒకవేళ((uint64_t)tmp_int > (uint64_t)MQTT_SESSION_EXPIRY_NEVER){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must be lower than %u.", MQTT_SESSION_EXPIRY_NEVER);
						తిరిగి MOSQ_ERR_INVAL;
					}
					cur_bridge->session_expiry_interval = (uint32_t)tmp_int;
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "bridge_tcp_keepalive")){
#నిర్వచితమైతే WITH_BRIDGE
					REQUIRE_BRIDGE(token);

					ఒకవేళ(conf__parse_int(&token, "bridge_tcp_keepalive_idle", &tmp_int, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(tmp_int <= 0){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: invalid TCP keepalive idle value.");
						తిరిగి MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_idle = (గుర్తులేని పూర్ణాంకం)tmp_int;

					ఒకవేళ(conf__parse_int(&token, "bridge_tcp_keepalive_interval", &tmp_int, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(tmp_int <= 0){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: invalid TCP keepalive interval value.");
						తిరిగి MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_interval = (గుర్తులేని పూర్ణాంకం)tmp_int;

					ఒకవేళ(conf__parse_int(&token, "bridge_tcp_keepalive_counter", &tmp_int, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(tmp_int <= 0){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: invalid TCP keepalive counter value.");
						తిరిగి MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_counter = (గుర్తులేని పూర్ణాంకం)tmp_int;
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "bridge_tcp_user_timeout")){
#నిర్వచితమైతే WITH_BRIDGE
					REQUIRE_BRIDGE(token);
#నిర్వచితమైతే WITH_TCP_USER_TIMEOUT
					ఒకవేళ(conf__parse_int(&token, "bridge_tcp_user_timeout", &tmp_int, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(tmp_int < 0){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: invalid TCP user timeout value.");
						తిరిగి MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_user_timeout = tmp_int;
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge TCP user timeout support not available.");
#అయితే_ముగింపు
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "bridge_tls_use_os_certs")){
#ఒకవేళ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_bool(&token, "bridge_tls_use_os_certs", &cur_bridge->tls_use_os_certs, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "bridge_tls_version")){
#ఒకవేళ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_string(&token, "bridge_tls_version", &cur_bridge->tls_సంస్కరణ, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "cafile")){
#ఒకవేళ defined(WITH_TLS)
					REQUIRE_LISTENER(token);
					ఒకవేళ(cur_listener->psk_hint){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						తిరిగి MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->cafile);
					ఒకవేళ(conf__parse_string(&token, "cafile", &cur_listener->cafile, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "capath")){
#నిర్వచితమైతే WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->capath);
					ఒకవేళ(conf__parse_string(&token, "capath", &cur_listener->capath, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "certfile")){
#నిర్వచితమైతే WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ఒకవేళ(cur_listener->psk_hint){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						తిరిగి MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->certfile);
					ఒకవేళ(conf__parse_string(&token, "certfile", &cur_listener->certfile, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "check_retain_source")){
					ఒకవేళ(conf__parse_bool(&token, "check_retain_source", &config->check_retain_ఆధారం, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "ciphers")){
#నిర్వచితమైతే WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers);
					ఒకవేళ(conf__parse_string(&token, "ciphers", &cur_listener->ciphers, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "ciphers_tls1.3")){
#ఒకవేళ defined(WITH_TLS) && (!defined(LIBRESSL_సంస్కరణ_సంఖ్య) || LIBRESSL_సంస్కరణ_సంఖ్య > 0x3040000FL)
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers_tls13);
					ఒకవేళ(conf__parse_string(&token, "ciphers_tls1.3", &cur_listener->ciphers_tls13, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: ciphers_tls1.3 support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "clientid") || !strcmp(token, "remote_clientid")){
#నిర్వచితమైతే WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_string(&token, "bridge remote clientid", &cur_bridge->remote_clientid, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "cleansession")){
#నిర్వచితమైతే WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_bool(&token, "cleansession", &cur_bridge->clean_ప్రారంభం, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "local_cleansession")){
#నిర్వచితమైతే WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_bool(&token, "local_cleansession", (bool *)&cur_bridge->clean_ప్రారంభం_local, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "clientid_prefixes")){
					OPTION_DEPRECATED(token, "");
					mosquitto_FREE(config->clientid_prefixes);
					ఒకవేళ(conf__parse_string(&token, "clientid_prefixes", &config->clientid_prefixes, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "connection")){
#నిర్వచితమైతే WITH_BRIDGE
					token = strtok_r(ఖాళీ, " ", &saveptr);
					REQUIRE_NON_ఖాళీలు_OPTION(token, "connection");

					/* Check for existing bridge name. */
					కొరకు(i=0; i<config->bridge_లెక్క; i++){
						ఒకవేళ(!strcmp(config->bridges[i]->name, token)){
							log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Duplicate bridge name '%s'.", token);
							తిరిగి MOSQ_ERR_INVAL;
						}
					}

					నిర్మాణం mosquitto__bridge **bridges_new = mosquitto_realloc(config->bridges, (size_t)(config->bridge_లెక్క+1)*పరిమాణం(నిర్మాణం mosquitto__bridge *));
					ఒకవేళ(!bridges_new){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
						తిరిగి MOSQ_ERR_NOMEM;
					}
					config->bridges = bridges_new;
					cur_bridge = mosquitto_malloc(పరిమాణం(నిర్మాణం mosquitto__bridge));
					ఒకవేళ(!cur_bridge){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
						తిరిగి MOSQ_ERR_NOMEM;
					}

					config->bridge_లెక్క++;
					config->bridges[config->bridge_లెక్క-1] = cur_bridge;

					memset(cur_bridge, 0, పరిమాణం(నిర్మాణం mosquitto__bridge));
					cur_bridge->name = mosquitto_strdup(token);
					ఒకవేళ(!cur_bridge->name){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
						తిరిగి MOSQ_ERR_NOMEM;
					}
					cur_bridge->keepalive = 60;
					cur_bridge->notifications = true;
					cur_bridge->notifications_local_only = false;
					cur_bridge->ప్రారంభం_type = bst_automatic;
					cur_bridge->idle_timeout = 60;
					cur_bridge->restart_timeout = 0;
					cur_bridge->backoff_పునాది = 5 * 1000;
					cur_bridge->backoff_cap = 30 * 1000;
					cur_bridge->stable_connection_period = 0;
					cur_bridge->పరిమితి = 10;
					cur_bridge->try_వ్యక్తిగత = true;
					cur_bridge->attempt_unsubscribe = true;
					cur_bridge->protocol_సంస్కరణ = mosq_p_mqtt311;
					cur_bridge->primary_retry_sock = INVALID_SOCKET;
					cur_bridge->outgoing_retain = true;
					cur_bridge->clean_ప్రారంభం_local = -1;
					cur_bridge->reload_type = brt_lazy;
					cur_bridge->max_topic_alias = 10;
#నిర్వచితమైతే WITH_TCP_USER_TIMEOUT
					cur_bridge->tcp_user_timeout = -1;
#అయితే_ముగింపు
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "connection_messages")){
					ఒకవేళ(conf__parse_bool(&token, token, &config->connection_messages, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "crlfile")){
#నిర్వచితమైతే WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->crlfile);
					ఒకవేళ(conf__parse_string(&token, "crlfile", &cur_listener->crlfile, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "dhparamfile")){
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: dhparamfile is no longer required.");
				}లేకపోతే ఒకవేళ(!strcmp(token, "disable_client_cert_date_checks")){
#నిర్వచితమైతే WITH_TLS
					REQUIRE_LISTENER(token);
					ఒకవేళ(conf__parse_bool(&token, "disable_client_cert_date_checks", &cur_listener->disable_client_cert_date_checks, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "enable_control_api")){
#నిర్వచితమైతే WITH_CONTROL
					ఒకవేళ(conf__parse_bool(&token, "enable_control_api", &config->enable_control_api, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: $CONTROL support not available (enable_control_api).");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "enable_proxy_protocol")){
#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					తిరిగి MOSQ_ERR_INVAL;
#అయితే_ముగింపు
					REQUIRE_LISTENER(token);
					ఒకవేళ(conf__parse_int(&token, "enable_proxy_protocol", &cur_listener->enable_proxy_protocol, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(cur_listener->enable_proxy_protocol < 1 || cur_listener->enable_proxy_protocol > 2){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: enable_proxy_protocol must be 1 or 2.");
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "global_max_clients")){
					ఒకవేళ(conf__parse_int(&token, "global_max_clients", &config->global_max_clients, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "global_max_connections")){
					ఒకవేళ(conf__parse_int(&token, "global_max_connections", &config->global_max_connections, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "http_dir")){
#ఒకవేళ defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
					ఒకవేళ(reload){
						కొనసాగు;        /* Not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					ఒకవేళ(conf__parse_string(&token, "http_dir", &cur_listener->http_dir, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#నిర్వచితమైతే WIN32
					అక్షరం *http_dir_canonical = _fullpath(ఖాళీ, cur_listener->http_dir, 0);
					స్థిరరాశి అక్షరం sep = '\\';
#లేకపోతే
					అక్షరం *http_dir_canonical = realpath(cur_listener->http_dir, ఖాళీ);
					స్థిరరాశి అక్షరం sep = '/';
#అయితే_ముగింపు
					ఒకవేళ(!http_dir_canonical){
						తిరిగి MOSQ_ERR_NOMEM;
					}
					size_t http_dir_len = అక్షరశ్రేణి_పొడవు(http_dir_canonical) + పరిమాణం(sep) + 1;
					అక్షరం *http_dir_canonical_sep = mosquitto_calloc(http_dir_len, పరిమాణం(అక్షరం));
					ఒకవేళ(!http_dir_canonical_sep){
						విడుదల(http_dir_canonical);
						తిరిగి MOSQ_ERR_NOMEM;
					}
					snprintf(http_dir_canonical_sep, http_dir_len, "%s%c", http_dir_canonical, sep);
					విడుదల(http_dir_canonical);
					mosquitto_FREE(cur_listener->http_dir);
					cur_listener->http_dir = http_dir_canonical_sep;
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: http_dir support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "idle_timeout")){
#నిర్వచితమైతే WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_int(&token, "idle_timeout", &cur_bridge->idle_timeout, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(cur_bridge->idle_timeout < 1){
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "idle_timeout interval too low, using 1 second.");
						cur_bridge->idle_timeout = 1;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "include_dir")){
					ఒకవేళ(level == 0){
						అక్షరం **files;
						పూర్ణాంకం file_లెక్క;

						/* Only process include_dir from the main config file. */
						token = strtok_r(ఖాళీ, "", &saveptr);
						REQUIRE_NON_ఖాళీలు_OPTION(token, "include_dir");

						rc = config__get_dir_files(token, &files, &file_లెక్క);
						ఒకవేళ(rc){
							తిరిగి rc;
						}

						కొరకు(i=0; i<file_లెక్క; i++){
							log__printf(ఖాళీ, MOSQ_LOG_INFO, "Loading config file '%s'", files[i]);

							rc = config__read_file(config, reload, files[i], cr, level+1, &lineno_ext);
							ఒకవేళ(rc){
								ఒకవేళ(lineno_ext > 0){
									log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error found at '%s:%d'.", files[i], lineno_ext);
								}
								/* Free happens below */
								ఆపు;
							}
						}
						కొరకు(i=0; i<file_లెక్క; i++){
							mosquitto_FREE(files[i]);
						}
						mosquitto_FREE(files);
						ఒకవేళ(rc){
							తిరిగి rc;    /* This returns if config__read_file() fails above */
						}
					}లేకపోతే{
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: The include_dir option is only valid in the main configuration file.");
						తిరిగి 1;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "keepalive_interval")){
#నిర్వచితమైతే WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_int(&token, "keepalive_interval", &tmp_int, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(tmp_int > UINT16_MAX){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Bridge keepalive value too high.");
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(tmp_int < 5){
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "keepalive interval too low, using 5 seconds.");
						tmp_int = 5;
					}
					cur_bridge->keepalive = (uint16_t)tmp_int;
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "keyfile")){
#నిర్వచితమైతే WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->keyfile);
					ఒకవేళ(conf__parse_string(&token, "keyfile", &cur_listener->keyfile, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "listener")){
					config->local_only = false;

					ఒకవేళ(conf__parse_int(&token, "listener port", &tmp_int, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#నిర్వచితమైతే WITH_UNIX_SOCKETS
					ఒకవేళ(tmp_int < 0 || tmp_int > UINT16_MAX){
#లేకపోతే
					ఒకవేళ(tmp_int < 1 || tmp_int > UINT16_MAX){
#అయితే_ముగింపు
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						తిరిగి MOSQ_ERR_INVAL;
					}

					/* Look for bind address / unix socket path */
					token = strtok_r(ఖాళీ, " ", &saveptr);
					ఒకవేళ(token != ఖాళీ && token[0] == '#'){
						token = ఖాళీ;
					}

					ఒకవేళ(tmp_int == 0 && token == ఖాళీ){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: A listener with port 0 must provide a Unix socket path.");
						తిరిగి MOSQ_ERR_INVAL;
					}

					ఒకవేళ(reload){
						/* We reload listeners settings based on port number/unix socket path.
						 * If the port number/unix path doesn't already exist, exit with a complaint. */
						cur_listener = ఖాళీ;
#నిర్వచితమైతే WITH_UNIX_SOCKETS
						ఒకవేళ(tmp_int == 0){
							కొరకు(i=0; i<config->listener_లెక్క; i++){
								ఒకవేళ(config->listeners[i].unix_socket_మార్గం != ఖాళీ
										&& strcmp(config->listeners[i].unix_socket_మార్గం, token) == 0){

									cur_listener = &config->listeners[i];
									ఆపు;
								}
							}
						}లేకపోతే
#అయితే_ముగింపు
						{
							కొరకు(i=0; i<config->listener_లెక్క; i++){
								ఒకవేళ(config->listeners[i].పోర్టు == tmp_int){
									/* Now check we have a matching bind address, if defined */
									ఒకవేళ(config->listeners[i].host){
										ఒకవేళ(token && !strcmp(config->listeners[i].host, token)){
											/* They both have a bind address, and they match */
											cur_listener = &config->listeners[i];
											ఆపు;
										}
									}లేకపోతే{
										ఒకవేళ(token == ఖాళీ){
											/* Neither this config nor the new config have a bind address,
											 * so they match. */
											cur_listener = &config->listeners[i];
											ఆపు;
										}
									}
								}
							}
						}
						ఒకవేళ(!cur_listener){
							log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: It is not currently possible to add/remove listeners when reloading the config file.");
							తిరిగి MOSQ_ERR_INVAL;
						}
					}లేకపోతే{
						ఒకవేళ(config_కూడు_listener(config)){
							log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
							తిరిగి MOSQ_ERR_NOMEM;
						}
						cur_listener = &config->listeners[config->listener_లెక్క-1];
					}

					listener_సమితి_defaults(cur_listener);
					cur_listener->పోర్టు = (uint16_t)tmp_int;

					mosquitto_FREE(cur_listener->host);

#నిర్వచితమైతే WITH_UNIX_SOCKETS
					mosquitto_FREE(cur_listener->unix_socket_మార్గం);
#అయితే_ముగింపు

					ఒకవేళ(token){
#నిర్వచితమైతే WITH_UNIX_SOCKETS
						ఒకవేళ(cur_listener->పోర్టు == 0){
							cur_listener->unix_socket_మార్గం = mosquitto_strdup(token);
						}లేకపోతే
#అయితే_ముగింపు
						{
							cur_listener->host = mosquitto_strdup(token);
						}
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "listener_allow_anonymous")){
					REQUIRE_LISTENER(token);
					ఒకవేళ(conf__parse_bool(&token, "listener_allow_anonymous", (bool *)&cur_listener->security_options->allow_anonymous, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "listener_auto_id_prefix")){
					REQUIRE_LISTENER(token);
					ఒకవేళ(conf__parse_string(&token, "listener_auto_id_prefix", &cur_listener->security_options->auto_id_ఉపసర్గ, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(cur_listener->security_options->auto_id_ఉపసర్గ){
						ఒకవేళ(అక్షరశ్రేణి_పొడవు(cur_listener->security_options->auto_id_ఉపసర్గ) > 50){
							log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: 'listener_auto_id_prefix' length must be <= 50.");
							తిరిగి MOSQ_ERR_INVAL;
						}
						cur_listener->security_options->auto_id_ఉపసర్గ_len = (uint16_t)అక్షరశ్రేణి_పొడవు(cur_listener->security_options->auto_id_ఉపసర్గ);
					}లేకపోతే{
						cur_listener->security_options->auto_id_ఉపసర్గ_len = 0;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "local_clientid")){
#నిర్వచితమైతే WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_string(&token, "bridge local clientd", &cur_bridge->local_clientid, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "local_password")){
#నిర్వచితమైతే WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_string(&token, "bridge local_password", &cur_bridge->local_password, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "local_username")){
#నిర్వచితమైతే WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_string(&token, "bridge local_username", &cur_bridge->local_username, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "log_dest")){
					token = strtok_r(ఖాళీ, " ", &saveptr);
					REQUIRE_NON_ఖాళీలు_OPTION(token, "log_dest");

					cr->log_dest_సమితి = 1;
					ఒకవేళ(!strcmp(token, "none")){
						cr->log_dest = MQTT3_LOG_NONE;
					}లేకపోతే ఒకవేళ(!strcmp(token, "syslog")){
						cr->log_dest |= MQTT3_LOG_SYSLOG;
					}లేకపోతే ఒకవేళ(!strcmp(token, "stdout")){
						cr->log_dest |= MQTT3_LOG_STDOUT;
					}లేకపోతే ఒకవేళ(!strcmp(token, "stderr")){
						cr->log_dest |= MQTT3_LOG_STDERR;
					}లేకపోతే ఒకవేళ(!strcmp(token, "topic")){
						cr->log_dest |= MQTT3_LOG_TOPIC;
					}లేకపోతే ఒకవేళ(!strcmp(token, "dlt")){
						cr->log_dest |= MQTT3_LOG_DLT;
#నిర్వచితమైతే ANDROID
					}లేకపోతే ఒకవేళ(!strcmp(token, "android")){
						cr->log_dest |= MQTT3_LOG_ANDROID;
#అయితే_ముగింపు
					}లేకపోతే ఒకవేళ(!strcmp(token, "file")){
						ఒకవేళ(config->log_fptr || config->log_file){
							log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Duplicate \"log_dest file\" value.");
							తిరిగి MOSQ_ERR_INVAL;
						}
						/* Get remaining string. */
						token = saveptr;
						ఒకవేళ(token && token[0]){
							అయితే(token[0] == ' ' || token[0] == '\t'){
								token++;
							}
						}
						/* Duplicate "token" check here saves a log__printf() */
						ఒకవేళ(token && token[0]){
							config->log_file = mosquitto_strdup(token);
							ఒకవేళ(!config->log_file){
								log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
								తిరిగి MOSQ_ERR_NOMEM;
							}
							cr->log_dest |= MQTT3_LOG_FILE;
						}లేకపోతే{
							log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Empty \"log_dest file\" value in configuration.");
							తిరిగి MOSQ_ERR_INVAL;
						}
						cr->log_dest |= MQTT3_LOG_FILE;
					}లేకపోతే{
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid 'log_dest' value (%s).", token);
						తిరిగి MOSQ_ERR_INVAL;
					}
#ఒకవేళ defined(WIN32) || defined(__CYGWIN__)
					ఒకవేళ(service_handle){
						ఒకవేళ(cr->log_dest == MQTT3_LOG_STDOUT || cr->log_dest == MQTT3_LOG_STDERR){
							log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Cannot log to stdout/stderr when running as a Windows service.");
							తిరిగి MOSQ_ERR_INVAL;
						}
					}
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "log_facility")){
#ఒకవేళ defined(WIN32) || defined(__CYGWIN__)
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: log_facility not supported on Windows.");
#లేకపోతే
					ఒకవేళ(conf__parse_int(&token, "log_facility", &tmp_int, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఎంపిక(tmp_int){
						స్థితి 0:
							config->log_facility = LOG_LOCAL0;
							ఆపు;
						స్థితి 1:
							config->log_facility = LOG_LOCAL1;
							ఆపు;
						స్థితి 2:
							config->log_facility = LOG_LOCAL2;
							ఆపు;
						స్థితి 3:
							config->log_facility = LOG_LOCAL3;
							ఆపు;
						స్థితి 4:
							config->log_facility = LOG_LOCAL4;
							ఆపు;
						స్థితి 5:
							config->log_facility = LOG_LOCAL5;
							ఆపు;
						స్థితి 6:
							config->log_facility = LOG_LOCAL6;
							ఆపు;
						స్థితి 7:
							config->log_facility = LOG_LOCAL7;
							ఆపు;
						అప్రమేయ:
							log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid 'log_facility' value (%d).", tmp_int);
							తిరిగి MOSQ_ERR_INVAL;
					}
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "log_timestamp")){
					ఒకవేళ(conf__parse_bool(&token, token, &config->log_timestamp, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "log_timestamp_format")){
					ఒకవేళ(conf__parse_string(&token, token, &config->log_timestamp_format, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "log_type")){
					token = strtok_r(ఖాళీ, " ", &saveptr);
					REQUIRE_NON_ఖాళీలు_OPTION(token, "log_type");

					cr->log_type_సమితి = 1;
					ఒకవేళ(!strcmp(token, "none")){
						cr->log_type = MOSQ_LOG_NONE;
					}లేకపోతే ఒకవేళ(!strcmp(token, "information")){
						cr->log_type |= MOSQ_LOG_INFO;
					}లేకపోతే ఒకవేళ(!strcmp(token, "notice")){
						cr->log_type |= MOSQ_LOG_NOTICE;
					}లేకపోతే ఒకవేళ(!strcmp(token, "warning")){
						cr->log_type |= MOSQ_LOG_WARNING;
					}లేకపోతే ఒకవేళ(!strcmp(token, "error")){
						cr->log_type |= MOSQ_LOG_ERR;
					}లేకపోతే ఒకవేళ(!strcmp(token, "debug")){
						cr->log_type |= MOSQ_LOG_DEBUG;
					}లేకపోతే ఒకవేళ(!strcmp(token, "subscribe")){
						cr->log_type |= MOSQ_LOG_SUBSCRIBE;
					}లేకపోతే ఒకవేళ(!strcmp(token, "unsubscribe")){
						cr->log_type |= MOSQ_LOG_UNSUBSCRIBE;
					}లేకపోతే ఒకవేళ(!strcmp(token, "internal")){
						cr->log_type |= MOSQ_LOG_అంతర్గత;
#నిర్వచితమైతే WITH_WEBSOCKETS
					}లేకపోతే ఒకవేళ(!strcmp(token, "websockets")){
						cr->log_type |= MOSQ_LOG_WEBSOCKETS;
#అయితే_ముగింపు
					}లేకపోతే ఒకవేళ(!strcmp(token, "all")){
						cr->log_type = MOSQ_LOG_ALL;
					}లేకపోతే{
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid 'log_type' value (%s).", token);
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "max_connections")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ఒకవేళ(conf__parse_int(&token, token, &cur_listener->max_connections, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(cur_listener->max_connections < 0){
						cur_listener->max_connections = -1;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "maximum_qos") || !strcmp(token, "max_qos")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ఒకవేళ(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(tmp_int < 0 || tmp_int > 2){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: 'max_qos' must be between 0 and 2 inclusive.");
						తిరిగి MOSQ_ERR_INVAL;
					}
					cur_listener->max_qos = (uint8_t)tmp_int;
				}లేకపోతే ఒకవేళ(!strcmp(token, "max_inflight_bytes")){
					ఒకవేళ(conf__parse_int(&token, "max_inflight_bytes", &tmp_int, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_inflight_బైట్లు = (size_t)tmp_int;
				}లేకపోతే ఒకవేళ(!strcmp(token, "max_inflight_messages")){
					ఒకవేళ(conf__parse_int(&token, "max_inflight_messages", &tmp_int, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(tmp_int < 0 || tmp_int == UINT16_MAX){
						tmp_int = 0;
					}లేకపోతే ఒకవేళ(tmp_int > UINT16_MAX){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: 'max_inflight_messages' must be <= 65535.");
						తిరిగి MOSQ_ERR_INVAL;
					}
					config->max_inflight_messages = (uint16_t)tmp_int;
				}లేకపోతే ఒకవేళ(!strcmp(token, "max_keepalive")){
					ఒకవేళ(conf__parse_int(&token, "max_keepalive", &tmp_int, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid 'max_keepalive' value (%d).", tmp_int);
						తిరిగి MOSQ_ERR_INVAL;
					}
					config->max_keepalive = (uint16_t)tmp_int;
				}లేకపోతే ఒకవేళ(!strcmp(token, "max_packet_size")){
					ఒకవేళ(conf__parse_int(&token, "max_packet_size", &tmp_int, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(tmp_int < 20){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: 'max_packet_size' must be >= 20.");
						తిరిగి MOSQ_ERR_INVAL;
					}
					config->max_packet_కొలత = (uint32_t)tmp_int;
				}లేకపోతే ఒకవేళ(!strcmp(token, "max_queued_bytes")){
					ఒకవేళ(conf__parse_int(&token, "max_queued_bytes", &tmp_int, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_బైట్లు = (size_t)tmp_int;
				}లేకపోతే ఒకవేళ(!strcmp(token, "max_queued_messages")){
					ఒకవేళ(conf__parse_int(&token, "max_queued_messages", &tmp_int, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_messages = tmp_int;
				}లేకపోతే ఒకవేళ(!strcmp(token, "memory_limit")){
					ssize_t lim;
					ఒకవేళ(conf__parse_ssize_t(&token, "memory_limit", &lim, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(lim < 0){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid 'memory_limit' value (%ld).", lim);
						తిరిగి MOSQ_ERR_INVAL;
					}
					mosquitto_జ్ఞాపకం_సమితి_హద్దు((size_t)lim);
				}లేకపోతే ఒకవేళ(!strcmp(token, "message_size_limit")){
					log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Note: It is recommended to replace `message_size_limit` with `max_packet_size`.");
					ఒకవేళ(conf__parse_int(&token, "message_size_limit", (పూర్ణాంకం *)&config->సందేశం_కొలత_హద్దు, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(config->సందేశం_కొలత_హద్దు > MQTT_MAX_PAYLOAD){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid 'message_size_limit' value (%u).", config->సందేశం_కొలత_హద్దు);
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "mount_point")){
					REQUIRE_LISTENER(token);
					mosquitto_FREE(cur_listener->mount_బిందువు);
					ఒకవేళ(conf__parse_string(&token, "mount_point", &cur_listener->mount_బిందువు, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(mosquitto_pub_topic_check(cur_listener->mount_బిందువు) != MOSQ_ERR_SUCCESS){
						log__printf(ఖాళీ, MOSQ_LOG_ERR,
								"Error: Invalid 'mount_point' value (%s). Does it contain a wildcard character?",
								cur_listener->mount_బిందువు);
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "notifications")){
#నిర్వచితమైతే WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_bool(&token, "notifications", &cur_bridge->notifications, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "notifications_local_only")){
#నిర్వచితమైతే WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_bool(&token, "notifications_local_only", &cur_bridge->notifications_local_only, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "notification_topic")){
#నిర్వచితమైతే WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_string(&token, "notification_topic", &cur_bridge->notification_topic, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "password") || !strcmp(token, "remote_password")){
#నిర్వచితమైతే WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_string(&token, "bridge remote_password", &cur_bridge->remote_password, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "password_file")){
					REQUIRE_LISTENER_IF_చొప్పున_LISTENER(token);
					conf_సమితి_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->password_దత్తాంశం.password_file);
					ఒకవేళ(conf__parse_string(&token, "password_file", &cur_security_options->password_దత్తాంశం.password_file, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "per_listener_settings")){
					OPTION_DEPRECATED(token, "Please see the documentation for how to achieve the same effect.");
					ఒకవేళ(config->చొప్పున_listener_settings){
						/* Once this is set, don't let it be unset. It should be the first config option ideally. */
						కొనసాగు;
					}
					ఒకవేళ(conf__parse_bool(&token, "per_listener_settings", &config->చొప్పున_listener_settings, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(cur_security_options && config->చొప్పున_listener_settings){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: per_listener_settings must be set before any other security settings.");
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "persistence") || !strcmp(token, "retained_persistence")){
					ఒకవేళ(conf__parse_bool(&token, token, &config->persistence, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "persistence_file")){
					ఒకవేళ(conf__parse_string(&token, "persistence_file", &config->persistence_file, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "persistence_location")){
					ఒకవేళ(conf__parse_string(&token, "persistence_location", &config->persistence_location, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "persistent_client_expiration")){
					token = strtok_r(ఖాళీ, " ", &saveptr);
					REQUIRE_NON_ఖాళీలు_OPTION(token, "persistent_client_expiration");

					time_t expiration_mult;

					ఎంపిక(token[అక్షరశ్రేణి_పొడవు(token)-1]){
						స్థితి 's':
							expiration_mult = 1;
							ఆపు;
						స్థితి 'h':
							expiration_mult = 3600;
							ఆపు;
						స్థితి 'd':
							expiration_mult = 86400;
							ఆపు;
						స్థితి 'w':
							expiration_mult = 86400*7;
							ఆపు;
						స్థితి 'm':
							expiration_mult = 86400*30;
							ఆపు;
						స్థితి 'y':
							expiration_mult = 86400*365;
							ఆపు;
						అప్రమేయ:
							log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
							తిరిగి MOSQ_ERR_INVAL;
					}
					token[అక్షరశ్రేణి_పొడవు(token)-1] = '\0';
					config->persistent_client_expiration = atoi(token)*expiration_mult;
					ఒకవేళ(config->persistent_client_expiration <= 0){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "pid_file")){
					ఒకవేళ(reload){
						కొనసాగు;        /* pid file not valid for reloading. */
					}
					ఒకవేళ(conf__parse_string(&token, "pid_file", &config->pid_file, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "port")){
					OPTION_DEPRECATED(token, "Please use 'listener' instead.");
					config->local_only = false;
					ఒకవేళ(reload){
						కొనసాగు;        /* Listeners not valid for reloading. */

					}
					ఒకవేళ(config__create_default_listener(config, token)){
						తిరిగి MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					ఒకవేళ(config->default_listener->పోర్టు){
						log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Default listener port specified multiple times. Only the latest will be used.");
					}
					ఒకవేళ(conf__parse_int(&token, "port", &tmp_int, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(tmp_int < 1 || tmp_int > UINT16_MAX){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						తిరిగి MOSQ_ERR_INVAL;
					}
					config->default_listener->పోర్టు = (uint16_t)tmp_int;
				}లేకపోతే ఒకవేళ(!strcmp(token, "protocol")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(ఖాళీ, " ", &saveptr);
					REQUIRE_NON_ఖాళీలు_OPTION(token, "protocol");

					ఒకవేళ(!strcmp(token, "mqtt")){
						cur_listener->protocol = mp_mqtt;
						/*
						}else if(!strcmp(token, "mqttsn")){
						    cur_listener->protocol = mp_mqttsn;
						*/
					}లేకపోతే ఒకవేళ(!strcmp(token, "websockets")){
#నిర్వచితమైతే WITH_WEBSOCKETS
						cur_listener->protocol = mp_websockets;
#లేకపోతే
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Websockets support not available.");
						తిరిగి MOSQ_ERR_INVAL;
#అయితే_ముగింపు
					}లేకపోతే ఒకవేళ(!strcmp(token, "http_api")){
#నిర్వచితమైతే WITH_HTTP_API
						cur_listener->protocol = mp_http_api;
#లేకపోతే
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: HTTP API support not available.");
						తిరిగి MOSQ_ERR_INVAL;
#అయితే_ముగింపు
					}లేకపోతే{
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid 'protocol' value (%s).", token);
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "proxy_protocol_v2_require_tls")){
#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					తిరిగి MOSQ_ERR_INVAL;
#అయితే_ముగింపు
					REQUIRE_LISTENER(token);
					ఒకవేళ(conf__parse_bool(&token, "proxy_protocol_v2_require_tls", &cur_listener->proxy_protocol_v2_require_tls, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "psk_file")){
#నిర్వచితమైతే FINAL_WITH_TLS_PSK
					REQUIRE_LISTENER_IF_చొప్పున_LISTENER(token);
					conf_సమితి_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ఒకవేళ(cur_listener && cur_listener->certfile){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						తిరిగి MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_security_options->psk_file);
					ఒకవేళ(conf__parse_string(&token, "psk_file", &cur_security_options->psk_file, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "psk_hint")){
#నిర్వచితమైతే FINAL_WITH_TLS_PSK
					ఒకవేళ(reload){
						కొనసాగు;        /* PSK file not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ఒకవేళ(conf__parse_string(&token, "psk_hint", &cur_listener->psk_hint, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "queue_qos0_messages")){
					ఒకవేళ(conf__parse_bool(&token, token, &config->వరుస_qos0_messages, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "require_certificate")){
#నిర్వచితమైతే WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ఒకవేళ(conf__parse_bool(&token, "require_certificate", &cur_listener->require_certificate, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "restart_timeout")){
#నిర్వచితమైతే WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					cur_bridge->backoff_cap = 0; /* set backoff to constant mode, unless cap is specified further down */
					token = strtok_r(ఖాళీ, " ", &saveptr);
					REQUIRE_NON_ఖాళీలు_OPTION(token, "restart_timeout");

					cur_bridge->restart_timeout = atoi(token);
					cur_bridge->backoff_పునాది = 0;
					cur_bridge->backoff_cap = 0;
					ఒకవేళ(cur_bridge->restart_timeout < 1){
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "restart_timeout interval too low, using 1 second.");
						cur_bridge->restart_timeout = 1;
					}లేకపోతే ఒకవేళ(cur_bridge->restart_timeout > 3600){
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "restart_timeout interval too high, using 3600 seconds.");
						cur_bridge->restart_timeout = 3600;
					}
					token = strtok_r(ఖాళీ, " ", &saveptr);
					ఒకవేళ(token){
						cur_bridge->backoff_పునాది = cur_bridge->restart_timeout;
						cur_bridge->backoff_cap = atoi(token);
						ఒకవేళ(cur_bridge->backoff_cap < cur_bridge->backoff_పునాది){
							log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: backoff cap is lower than the base in restart_timeout.");
							తిరిగి MOSQ_ERR_INVAL;
						}లేకపోతే ఒకవేళ(cur_bridge->backoff_cap > 7200){
							log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: backoff cap too high, using 7200 seconds.");
							cur_bridge->backoff_cap = 7200;
						}

						token = strtok_r(ఖాళీ, " ", &saveptr);
						ఒకవేళ(token){
							cur_bridge->stable_connection_period = atoi(token);
							ఒకవేళ(cur_bridge->stable_connection_period < 0){
								log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: stable connection period cannot be negative.");
								తిరిగి MOSQ_ERR_INVAL;
							}
						}
					}
					cur_bridge->restart_timeout *= 1000; /* backoff is tracked in ms */
					cur_bridge->backoff_పునాది *= 1000;
					cur_bridge->backoff_cap *= 1000;
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "retain_available")){
					ఒకవేళ(conf__parse_bool(&token, token, &config->retain_అందుబాటు, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "retain_expiry_interval")){
					ఒకవేళ(conf__parse_int(&token, token, &config->retain_expiry_interval, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(config->retain_expiry_interval < 1){
						log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Error: retain_expiry_interval must be >= 1.");
						తిరిగి MOSQ_ERR_INVAL;
					}లేకపోతే ఒకవేళ(config->retain_expiry_interval > 10000000){
						log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: retain_expiry_interval being capped at 19 years.");
						config->retain_expiry_interval = 10000000;
					}
					config->retain_expiry_interval *= 60;
				}లేకపోతే ఒకవేళ(!strcmp(token, "retry_interval")){
					OPTION_UNAVAILABLE(token);
				}లేకపోతే ఒకవేళ(!strcmp(token, "round_robin")){
#నిర్వచితమైతే WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_bool(&token, "round_robin", &cur_bridge->విడత_robin, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "set_tcp_nodelay")){
					ఒకవేళ(conf__parse_bool(&token, "set_tcp_nodelay", &config->సమితి_tcp_nodelay, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "start_type")){
#నిర్వచితమైతే WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(ఖాళీ, " ", &saveptr);
					REQUIRE_NON_ఖాళీలు_OPTION(token, "start_type");

					ఒకవేళ(!strcmp(token, "automatic")){
						cur_bridge->ప్రారంభం_type = bst_automatic;
					}లేకపోతే ఒకవేళ(!strcmp(token, "lazy")){
						cur_bridge->ప్రారంభం_type = bst_lazy;
					}లేకపోతే ఒకవేళ(!strcmp(token, "manual")){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Manual start_type not supported.");
						తిరిగి MOSQ_ERR_INVAL;
					}లేకపోతే ఒకవేళ(!strcmp(token, "once")){
						cur_bridge->ప్రారంభం_type = bst_once;
					}లేకపోతే{
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid 'start_type' value in configuration (%s).", token);
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "socket_domain")){
					ఒకవేళ(reload){
						కొనసాగు;        /* socket_domain not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(ఖాళీ, " ", &saveptr);
					REQUIRE_NON_ఖాళీలు_OPTION(token, "start_type");

					ఒకవేళ(!strcmp(token, "ipv4")){
						cur_listener->socket_domain = AF_INET;
					}లేకపోతే ఒకవేళ(!strcmp(token, "ipv6")){
						cur_listener->socket_domain = AF_INET6;
					}లేకపోతే{
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid 'socket_domain' value '%s' in configuration.", token);
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "sys_interval")){
					ఒకవేళ(conf__parse_int(&token, "sys_interval", &config->sys_interval, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(config->sys_interval < 0 || config->sys_interval > 65535){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid 'sys_interval' value (%d).", config->sys_interval);
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "threshold")){
#నిర్వచితమైతే WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_int(&token, "threshold", &cur_bridge->పరిమితి, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(cur_bridge->పరిమితి < 1){
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "threshold too low, using 1 message.");
						cur_bridge->పరిమితి = 1;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "tls_engine")){
#నిర్వచితమైతే WITH_TLS
					ఒకవేళ(reload){
						కొనసాగు;        /* tls_engine not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ఒకవేళ(conf__parse_string(&token, "tls_engine", &cur_listener->tls_engine, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "tls_engine_kpass_sha1")){
#నిర్వచితమైతే WITH_TLS
					ఒకవేళ(reload){
						కొనసాగు;        /* tls_engine not valid for reloading. */

					}
					అక్షరం *kpass_sha = ఖాళీ, *kpass_sha_bin = ఖాళీ;
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ఒకవేళ(conf__parse_string(&token, "tls_engine_kpass_sha1", &kpass_sha, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(mosquitto__hex2bin_sha1(kpass_sha, (గుర్తులేని అక్షరం **)&kpass_sha_bin) != MOSQ_ERR_SUCCESS){
						mosquitto_FREE(kpass_sha);
						తిరిగి MOSQ_ERR_INVAL;
					}
					mosquitto_free(cur_listener->tls_engine_kpass_sha1);
					cur_listener->tls_engine_kpass_sha1 = kpass_sha_bin;
					mosquitto_FREE(kpass_sha);
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "tls_keyform")){
#నిర్వచితమైతే WITH_TLS
					ఒకవేళ(reload){
						కొనసాగు;        /* tls_engine not valid for reloading. */

					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					అక్షరం *keyform = ఖాళీ;
					ఒకవేళ(conf__parse_string(&token, "tls_keyform", &keyform, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					cur_listener->tls_keyform = mosq_k_pem;
					ఒకవేళ(!strcmp(keyform, "engine")){
						cur_listener->tls_keyform = mosq_k_engine;
					}
					mosquitto_FREE(keyform);
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "tls_version")){
#ఒకవేళ defined(WITH_TLS)
					ఒకవేళ(reload){
						కొనసాగు;        /* tls_version not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					ఒకవేళ(conf__parse_string(&token, "tls_version", &cur_listener->tls_సంస్కరణ, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "topic")){
#నిర్వచితమైతే WITH_BRIDGE
					అక్షరం *topic = ఖాళీ;
					గణన mosquitto__bridge_direction direction = bd_బహిర్గతం;
					uint8_t qos = 0;
					అక్షరం *local_ఉపసర్గ = ఖాళీ, *remote_ఉపసర్గ = ఖాళీ;

					REQUIRE_BRIDGE(token);

					token = strtok_r(ఖాళీ, " ", &saveptr);
					REQUIRE_NON_ఖాళీలు_OPTION(token, "topic");

					// Check if the topic is quoted (e.g. for spaces within topic names), but not the
					// special case of ""
					ఒకవేళ(token[0] == '"' && token [1] != '"'){
						ఒకవేళ(strchr(saveptr, '"') == ఖాళీ){
							log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Missing closing quote in topic value (%s).", saveptr);
							తిరిగి MOSQ_ERR_INVAL;
						}

						అక్షరం *topic_లోపల_quotes = strtok_r(ఖాళీ, "\"", &saveptr);
						size_t tlen = 1;
						ఒకవేళ(topic_లోపల_quotes){
							tlen = అక్షరశ్రేణి_పొడవు(topic_లోపల_quotes);
						}

						topic = mosquitto_malloc(అక్షరశ్రేణి_పొడవు(token) + tlen + 1);
						ఒకవేళ(!topic){
							log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
							తిరిగి MOSQ_ERR_NOMEM;
						}

						strcpy(topic, token + 1);
						strcat(topic, " ");
						ఒకవేళ(topic_లోపల_quotes){
							strcat(topic, topic_లోపల_quotes);
						}
					}లేకపోతే{
						topic = mosquitto_strdup(token);
						ఒకవేళ(!topic){
							log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
							తిరిగి MOSQ_ERR_NOMEM;
						}
					}

					token = strtok_r(ఖాళీ, " ", &saveptr);
					ఒకవేళ(token){
						ఒకవేళ(!strcasecmp(token, "out")){
							direction = bd_బహిర్గతం;
						}లేకపోతే ఒకవేళ(!strcasecmp(token, "in")){
							direction = bd_లోపల;
						}లేకపోతే ఒకవేళ(!strcasecmp(token, "both")){
							direction = bd_both;
						}లేకపోతే{
							log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid bridge topic direction '%s'.", token);
							mosquitto_FREE(topic);
							తిరిగి MOSQ_ERR_INVAL;
						}
						token = strtok_r(ఖాళీ, " ", &saveptr);
						ఒకవేళ(token){
							ఒకవేళ(token[0] == '#'){
								(శూన్య)strtok_r(ఖాళీ, "", &saveptr);
							}
							qos = (uint8_t)atoi(token);
							ఒకవేళ(qos > 2){
								log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid bridge QoS level '%s'.", token);
								mosquitto_FREE(topic);
								తిరిగి MOSQ_ERR_INVAL;
							}

							token = strtok_r(ఖాళీ, " ", &saveptr);
							ఒకవేళ(token){
								ఒకవేళ(!strcmp(token, "\"\"") || token[0] == '#'){
									local_ఉపసర్గ = ఖాళీ;
									ఒకవేళ(token[0] == '#'){
										(శూన్య)strtok_r(ఖాళీ, "", &saveptr);
									}
								}లేకపోతే{
									local_ఉపసర్గ = token;
								}

								token = strtok_r(ఖాళీ, " ", &saveptr);
								ఒకవేళ(token){
									ఒకవేళ(!strcmp(token, "\"\"") || token[0] == '#'){
										remote_ఉపసర్గ = ఖాళీ;
									}లేకపోతే{
										remote_ఉపసర్గ = token;
									}
								}
							}
						}
					}

					ఒకవేళ(bridge_కూడు_topic(cur_bridge, topic, direction, qos, local_ఉపసర్గ, remote_ఉపసర్గ)){
						mosquitto_FREE(topic);
						తిరిగి MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(topic);
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "max_topic_alias")){
					REQUIRE_LISTENER(token);
					ఒకవేళ(conf__parse_int(&token, "max_topic_alias", &tmp_int, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias' value in configuration.");
						తిరిగి MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias = (uint16_t)tmp_int;
				}లేకపోతే ఒకవేళ(!strcmp(token, "max_topic_alias_broker")){
					REQUIRE_LISTENER(token);
					ఒకవేళ(conf__parse_int(&token, "max_topic_alias_broker", &tmp_int, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias_broker' value in configuration.");
						తిరిగి MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias_broker = (uint16_t)tmp_int;
				}లేకపోతే ఒకవేళ(!strcmp(token, "try_private")){
#నిర్వచితమైతే WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_bool(&token, "try_private", &cur_bridge->try_వ్యక్తిగత, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "upgrade_outgoing_qos")){
					ఒకవేళ(conf__parse_bool(&token, token, &config->upgrade_outgoing_qos, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "use_identity_as_username")){
#నిర్వచితమైతే WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ఒకవేళ(conf__parse_bool(&token, "use_identity_as_username", &cur_listener->use_తటస్థం_as_username, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "use_subject_as_username")){
#నిర్వచితమైతే WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ఒకవేళ(conf__parse_bool(&token, "use_subject_as_username", &cur_listener->use_subject_as_username, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "user")){
					ఒకవేళ(reload){
						కొనసాగు;        /* Drop privileges user not valid for reloading. */
					}
					mosquitto_FREE(config->user);
					ఒకవేళ(conf__parse_string(&token, "user", &config->user, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "use_username_as_clientid")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ఒకవేళ(conf__parse_bool(&token, "use_username_as_clientid", &cur_listener->use_username_as_clientid, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
				}లేకపోతే ఒకవేళ(!strcmp(token, "username") || !strcmp(token, "remote_username")){
#నిర్వచితమైతే WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ఒకవేళ(conf__parse_string(&token, "bridge remote_username", &cur_bridge->remote_username, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "websockets_log_level")){
#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					ఒకవేళ(conf__parse_int(&token, "websockets_log_level", &config->websockets_log_level, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
#అయితే_ముగింపు
				}లేకపోతే ఒకవేళ(!strcmp(token, "websockets_headers_size") || !strcmp(token, "packet_buffer_size")){
					ఒకవేళ(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					ఒకవేళ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Error: Packet buffer size must be between 0 and 65535 inclusive.");
						తిరిగి MOSQ_ERR_INVAL;
					}
					config->packet_బఫరు_కొలత = (uint16_t)tmp_int;
				}లేకపోతే ఒకవేళ(!strcmp(token, "websockets_origin")){
#నిర్వచితమైతే WITH_WEBSOCKETS
#  ఒకవేళ LWS_LIBRARY_సంస్కరణ_సంఖ్య >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN
					REQUIRE_LISTENER(token);
					ws_origins = mosquitto_realloc(cur_listener->ws_origins, పరిమాణం(అక్షరం *)*(size_t)(cur_listener->ws_origin_లెక్క+1));
					ఒకవేళ(ws_origins == ఖాళీ){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
						తిరిగి MOSQ_ERR_NOMEM;
					}
					ws_origins[cur_listener->ws_origin_లెక్క] = ఖాళీ;
					cur_listener->ws_origins = ws_origins;
					ఒకవేళ(conf__parse_string(&token, "websockets_origin", &ws_origins[cur_listener->ws_origin_లెక్క], &saveptr)){
						తిరిగి MOSQ_ERR_INVAL;
					}
					cur_listener->ws_origin_లెక్క++;
#  లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: websockets_origin support not available, libwebsockets version is too old.");
#  endif
#లేకపోతే
					log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Websockets support not available.");
#అయితే_ముగింపు
				}లేకపోతే{
					log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unknown configuration variable '%s'.", token);
					తిరిగి MOSQ_ERR_INVAL;
				}
			}
		}
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం config__read_file(నిర్మాణం mosquitto__config *config, bool reload, స్థిరరాశి అక్షరం *file, నిర్మాణం config_recurse *cr, పూర్ణాంకం level, పూర్ణాంకం *lineno)
{
	పూర్ణాంకం rc;
	FILE *fptr = ఖాళీ;
	అక్షరం *buf;
	పూర్ణాంకం buflen;
#నిర్వచితం_కాకపోతే WIN32
	DIR *dir;
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే WIN32
	dir = opendir(file);
	ఒకవేళ(dir){
		closedir(dir);
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Config file '%s' is a directory.", file);
		తిరిగి 1;
	}
#అయితే_ముగింపు

	fptr = mosquitto_fopen(file, "rt", false);
	ఒకవేళ(!fptr){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to open config file '%s'.", file);
		తిరిగి 1;
	}

	buflen = 1000;
	buf = mosquitto_malloc((size_t)buflen);
	ఒకవేళ(!buf){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
		fclose(fptr);
		తిరిగి MOSQ_ERR_NOMEM;
	}

	rc = config__read_file_core(config, reload, cr, level, lineno, fptr, &buf, &buflen);
	mosquitto_FREE(buf);
	fclose(fptr);

	తిరిగి rc;
}


స్థిర పూర్ణాంకం config__check_proxy(నిర్మాణం mosquitto__config *config)
{
	కొరకు(పూర్ణాంకం i=0; i<config->listener_లెక్క; i++){
		నిర్మాణం mosquitto__listener *l = &config->listeners[i];

		ఒకవేళ(l->enable_proxy_protocol == 2){
#నిర్వచితమైతే WITH_TLS
			ఒకవేళ(l->use_subject_as_username){
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: use_subject_as_username cannot be used with `enable_proxy_protocol 2`.");
				తిరిగి MOSQ_ERR_INVAL;
			}

			ఒకవేళ(l->certfile || l->keyfile){
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: certfile and keyfile cannot be used with `enable_proxy_protocol 2`.");
				తిరిగి MOSQ_ERR_INVAL;
			}
#అయితే_ముగింపు
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం config__check(నిర్మాణం mosquitto__config *config)
{
	/* Checks that are easy to make after the config has been loaded. */

	స్థిరరాశి అక్షరం *id_ఉపసర్గ;
	పూర్ణాంకం id_ఉపసర్గ_len;
	ఒకవేళ(config->security_options.auto_id_ఉపసర్గ){
		id_ఉపసర్గ = config->security_options.auto_id_ఉపసర్గ;
		id_ఉపసర్గ_len = config->security_options.auto_id_ఉపసర్గ_len;
	}లేకపోతే{
		id_ఉపసర్గ = "auto-";
		id_ఉపసర్గ_len = (పూర్ణాంకం)అక్షరశ్రేణి_పొడవు("auto-");
	}

	/* Default to auto_id_prefix = 'auto-' if none set. */
	కొరకు(పూర్ణాంకం i=0; i<config->listener_లెక్క; i++){
		ఒకవేళ(!config->listeners[i].security_options->auto_id_ఉపసర్గ){
			config->listeners[i].security_options->auto_id_ఉపసర్గ = mosquitto_strdup(id_ఉపసర్గ);
			ఒకవేళ(!config->listeners[i].security_options->auto_id_ఉపసర్గ){
				తిరిగి MOSQ_ERR_NOMEM;
			}
			config->listeners[i].security_options->auto_id_ఉపసర్గ_len = (uint16_t)id_ఉపసర్గ_len;
		}
	}

	తిరిగి config__check_proxy(config);
}

#నిర్వచితమైతే WITH_BRIDGE


స్థిర పూర్ణాంకం config__check_bridges(నిర్మాణం mosquitto__config *config)
{
	నిర్మాణం mosquitto__bridge *bridge1, *bridge2;
	అక్షరం hostname[256];
	size_t len;

	/* Check for bridge duplicate local_clientid, need to generate missing IDs
	 * first. */
	కొరకు(పూర్ణాంకం i=0; i<config->bridge_లెక్క; i++){
		bridge1 = config->bridges[i];

		ఒకవేళ(!bridge1->remote_clientid){
			ఒకవేళ(!gethostname(hostname, 256)){
				len = అక్షరశ్రేణి_పొడవు(hostname) + అక్షరశ్రేణి_పొడవు(bridge1->name) + 2;
				bridge1->remote_clientid = mosquitto_malloc(len);
				ఒకవేళ(!bridge1->remote_clientid){
					తిరిగి MOSQ_ERR_NOMEM;
				}
				snprintf(bridge1->remote_clientid, len, "%s.%s", hostname, bridge1->name);
			}లేకపోతే{
				తిరిగి 1;
			}
		}

		ఒకవేళ(!bridge1->local_clientid){
			len = అక్షరశ్రేణి_పొడవు(bridge1->remote_clientid) + అక్షరశ్రేణి_పొడవు("local.") + 2;
			bridge1->local_clientid = mosquitto_malloc(len);
			ఒకవేళ(!bridge1->local_clientid){
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
				తిరిగి MOSQ_ERR_NOMEM;
			}
			snprintf(bridge1->local_clientid, len, "local.%s", bridge1->remote_clientid);
		}
	}

	కొరకు(పూర్ణాంకం i=0; i<config->bridge_లెక్క; i++){
		bridge1 = config->bridges[i];
		కొరకు(పూర్ణాంకం j=i+1; j<config->bridge_లెక్క; j++){
			bridge2 = config->bridges[j];
			ఒకవేళ(!strcmp(bridge1->local_clientid, bridge2->local_clientid)){
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Bridge local_clientid "
						"'%s' is not unique. Try changing or setting the "
						"local_clientid value for one of the bridges.",
						bridge1->local_clientid);
				తిరిగి MOSQ_ERR_INVAL;
			}
		}
	}

#నిర్వచితమైతే WITH_TLS
	/* Check for missing TLS cafile/capath/certfile/keyfile */
	కొరకు(పూర్ణాంకం i=0; i<config->listener_లెక్క; i++){
		bool cafile = !!config->listeners[i].cafile;
		bool capath = !!config->listeners[i].capath;
		bool certfile = !!config->listeners[i].certfile;
		bool keyfile = !!config->listeners[i].keyfile;

		ఒకవేళ((certfile && !keyfile) || (!certfile && keyfile)){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Both certfile and keyfile must be provided to enable a TLS listener.");
			తిరిగి MOSQ_ERR_INVAL;
		}
		ఒకవేళ(cafile && !certfile){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: cafile specified without certfile and keyfile.");
			తిరిగి MOSQ_ERR_INVAL;
		}
		ఒకవేళ(capath && !certfile){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: capath specified without certfile and keyfile.");
			తిరిగి MOSQ_ERR_INVAL;
		}
	}
#అయితే_ముగింపు
	తిరిగి MOSQ_ERR_SUCCESS;
}
#అయితే_ముగింపు


స్థిర పూర్ణాంకం conf__parse_bool(అక్షరం **token, స్థిరరాశి అక్షరం *name, bool *విలువ, అక్షరం **saveptr)
{
	*token = strtok_r(ఖాళీ, " ", saveptr);
	ఒకవేళ(*token){
		ఒకవేళ(!strcmp(*token, "false") || !strcmp(*token, "0")){
			*విలువ = false;
		}లేకపోతే ఒకవేళ(!strcmp(*token, "true") || !strcmp(*token, "1")){
			*విలువ = true;
		}లేకపోతే{
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Invalid '%s' value (%s).", name, *token);
			తిరిగి MOSQ_ERR_INVAL;
		}
	}లేకపోతే{
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		తిరిగి MOSQ_ERR_INVAL;
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం conf__parse_int(అక్షరం **token, స్థిరరాశి అక్షరం *name, పూర్ణాంకం *విలువ, అక్షరం **saveptr)
{
	*token = strtok_r(ఖాళీ, " ", saveptr);
	ఒకవేళ(*token){
		అక్షరం *endptr = ఖాళీ;
		దీర్ఘ l = strtol(*token, &endptr, 0);
		ఒకవేళ(endptr == *token || endptr[0] != '\0'){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: '%s' value not a number.", name);
			తిరిగి MOSQ_ERR_INVAL;
		}
		ఒకవేళ(l > గరిష్ఠ_పూర్ణాంకం || l < INT_MIN){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: '%s' value out of range.", name);
			తిరిగి MOSQ_ERR_INVAL;
		}
		*విలువ = (పూర్ణాంకం)l;
	}లేకపోతే{
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		తిరిగి MOSQ_ERR_INVAL;
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం conf__parse_ssize_t(అక్షరం **token, స్థిరరాశి అక్షరం *name, ssize_t *విలువ, అక్షరం **saveptr)
{
	*token = strtok_r(ఖాళీ, " ", saveptr);
	ఒకవేళ(*token){
		*విలువ = atol(*token);
	}లేకపోతే{
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		తిరిగి MOSQ_ERR_INVAL;
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం conf__parse_string(అక్షరం **token, స్థిరరాశి అక్షరం *name, అక్షరం **విలువ, అక్షరం **saveptr)
{
	size_t tlen;

	*token = strtok_r(ఖాళీ, "", saveptr);
	ఒకవేళ(*token){
		ఒకవేళ(*విలువ){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Duplicate '%s' value in configuration.", name);
			తిరిగి MOSQ_ERR_INVAL;
		}
		/* Deal with multiple spaces at the beginning of the string. */
		*token = mosquitto_trimblanks(*token);
		ఒకవేళ(అక్షరశ్రేణి_పొడవు(*token) == 0){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
			తిరిగి MOSQ_ERR_INVAL;
		}

		tlen = అక్షరశ్రేణి_పొడవు(*token);
		ఒకవేళ(tlen > UINT16_MAX){
			తిరిగి MOSQ_ERR_INVAL;
		}
		ఒకవేళ(mosquitto_validate_utf8(*token, (uint16_t)tlen)){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Malformed UTF-8 in configuration.");
			తిరిగి MOSQ_ERR_INVAL;
		}
		*విలువ = mosquitto_strdup(*token);
		ఒకవేళ(!*విలువ){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
			తిరిగి MOSQ_ERR_NOMEM;
		}
	}లేకపోతే{
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		తిరిగి MOSQ_ERR_INVAL;
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}
