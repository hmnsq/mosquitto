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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <limits.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <errno.h>

#ifdef WIN32
#ಅನ್ಯಥಾ
#  include <dirent.h>
#  include <strings.h>
#endif

#ifndef WIN32
#  include <netdb.h>
#  include <sys/socket.h>
#ಅನ್ಯಥಾ
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#ಯದಿ !defined(WIN32) && !defined(__CYGWIN__)
#  include <syslog.h>
#endif

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "tls_mosq.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"

#ಅನ್ತರ್ಭಾವ "utlist.h"

ಸಂರಚನಾ config_recurse {
	unsigned ಪೂರ್ಣಾಙ್ಕ log_dest;
	ಪೂರ್ಣಾಙ್ಕ log_dest_set;
	unsigned ಪೂರ್ಣಾಙ್ಕ log_type;
	ಪೂರ್ಣಾಙ್ಕ log_type_set;
};

#ಯದಿ defined(WIN32) || defined(__CYGWIN__)
#ಅನ್ತರ್ಭಾವ <windows.h>
ಬಾಹ್ಯ SERVICE_STATUS_HANDLE service_handle;
#endif


#define REQUIRE_LISTENER(A) \
		ಕುರು{ \
			ಯದಿ(cur_listener == ರಿಕ್ತ){ \
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL; \
			} \
		}ಯಾವತ್(0)

#define REQUIRE_LISTENER_OR_DEFAULT_LISTENER(A) \
		ಕುರು{ \
			ಯದಿ(cur_listener == ರಿಕ್ತ){ \
				ಯದಿ(config__create_default_listener(config, (A))){ \
					ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM; \
				} \
				cur_listener = config->default_listener; \
			} \
		}ಯಾವತ್(0)

#define REQUIRE_LISTENER_IF_PER_LISTENER(A) \
		ಕುರು{ \
			ಯದಿ(config->per_listener_settings == true && cur_listener == ರಿಕ್ತ){ \
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL; \
			} \
		}ಯಾವತ್(0)

#define REQUIRE_NON_DEFAULT_LISTENER(A) \
		ಕುರು{ \
			ಯದಿ(cur_listener == config->default_listener || cur_listener == ರಿಕ್ತ){ \
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL; \
			} \
		}ಯಾವತ್(0)

#define REQUIRE_BRIDGE(A) \
		ಕುರು{ \
			ಯದಿ(cur_bridge == ರಿಕ್ತ){ \
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: The '%s' option requires a bridge to be defined first.", (A)); \
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL; \
			} \
		}ಯಾವತ್(0)

#define REQUIRE_PLUGIN(A) \
		ಕುರು{ \
			ಯದಿ(cur_plugin == ರಿಕ್ತ){ \
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: The '%s' option requires plugin/global_plugin/plugin_load to be defined first.", (A)); \
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL; \
			} \
		}ಯಾವತ್(0)

#define OPTION_DEPRECATED(A, B) \
		log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "The '%s' option is now deprecated and will be removed in version 3.0. %s", (A), (B))

#define OPTION_UNAVAILABLE(A) \
		log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: The '%s' option is no longer available.", (A));

#define REQUIRE_NON_EMPTY_OPTION(A, B) \
		ಕುರು{ \
			ಯದಿ(!(A)){ \
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", (B)); \
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL; \
			} \
		}ಯಾವತ್(0)

#define PER_LISTENER_ALTERNATIVE(A, B) \
		ಯದಿ(config->per_listener_settings){ \
			log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "You are using the '%s' option with 'per_listener_settings true'. Please replace this with '%s'.", A, B); \
		} \


#ifdef FINAL_WITH_TLS_PSK
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A) \
		ಕುರು{ \
			ಯದಿ(cur_bridge->tls_psk_identity || cur_bridge->tls_psk){ \
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and psk encryption in a single bridge.", (A)); \
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL; \
			} \
		}ಯಾವತ್(0)

#  define REQUIRE_BRIDGE_NO_X509(A) \
		ಕುರು{ \
			ಯದಿ(cur_bridge->tls_cafile || cur_bridge->tls_capath || cur_bridge->tls_certfile || cur_bridge->tls_keyfile){ \
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and identity encryption in a single bridge.", (A)); \
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL; \
			} \
		}ಯಾವತ್(0)

#ಅನ್ಯಥಾ
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A)
#  define REQUIRE_BRIDGE_NO_X509(A)
#endif

ಸ್ಥಿರ ಸಂರಚನಾ mosquitto__security_options *cur_security_options = ರಿಕ್ತ;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ conf__parse_bool(ವರ್ಣ **token, ಧ್ರುವ ವರ್ಣ *name, bool *ಮೌಲ್ಯ, ವರ್ಣ **saveptr);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ conf__parse_int(ವರ್ಣ **token, ಧ್ರುವ ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ *ಮೌಲ್ಯ, ವರ್ಣ **saveptr);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ conf__parse_ssize_t(ವರ್ಣ **token, ಧ್ರುವ ವರ್ಣ *name, ssize_t *ಮೌಲ್ಯ, ವರ್ಣ **saveptr);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ conf__parse_string(ವರ್ಣ **token, ಧ್ರುವ ವರ್ಣ *name, ವರ್ಣ **ಮೌಲ್ಯ, ವರ್ಣ **saveptr);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ config__read_file(ಸಂರಚನಾ mosquitto__config *config, bool reload, ಧ್ರುವ ವರ್ಣ *file, ಸಂರಚನಾ config_recurse *config_tmp, ಪೂರ್ಣಾಙ್ಕ level, ಪೂರ್ಣಾಙ್ಕ *lineno);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ config__check(ಸಂರಚನಾ mosquitto__config *config);
ಸ್ಥಿರ ಶೂನ್ಯ config__cleanup_plugins(ಶೂನ್ಯ);
#ifdef WITH_BRIDGE
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ config__check_bridges(ಸಂರಚನಾ mosquitto__config *config);
#endif


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ config__add_listener(ಸಂರಚನಾ mosquitto__config *config)
{
	ಸಂರಚನಾ mosquitto__listener *listener;
	ಸಂರಚನಾ mosquitto__listener *new_listeners;
	ಪೂರ್ಣಾಙ್ಕ def_listener = -1;

	ಯದಿ(config->default_listener){
		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<config->listener_ಎಣಿಕೆ; i++){
			ಯದಿ(&config->listeners[i] == config->default_listener){
				def_listener = i;
				ಭಙ್ಗ;
			}
		}
	}
	new_listeners = mosquitto_realloc(config->listeners, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__listener)*(size_t)(config->listener_ಎಣಿಕೆ+1));
	ಯದಿ(!new_listeners){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}
	config->listeners = new_listeners;
	listener = &config->listeners[config->listener_ಎಣಿಕೆ];
	memset(listener, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__listener));
	listener->security_options = mosquitto_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__security_options));
	ಯದಿ(!listener->security_options){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}
	ಯದಿ(def_listener != -1){
		config->default_listener = &config->listeners[def_listener];
	}
	config->listener_ಎಣಿಕೆ++;

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ config__create_default_listener(ಸಂರಚನಾ mosquitto__config *config, ಧ್ರುವ ವರ್ಣ *option_name)
{
	ಯದಿ(config->default_listener){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}
	log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Creating default listener due to '%s' option.", option_name);
	log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "It is best practice to define a 'listener' first. Using the '%s' option without a listener will be disabled in the future.", option_name);
	ಯದಿ(config__add_listener(config)){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}

	config->default_listener = &config->listeners[config->listener_ಎಣಿಕೆ-1];
	listener__set_defaults(config->default_listener);
	config->default_listener->port = 1883;

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಶೂನ್ಯ conf__set_cur_security_options(ಸಂರಚನಾ mosquitto__config *config, ಸಂರಚನಾ mosquitto__listener **cur_listener, ಸಂರಚನಾ mosquitto__security_options **security_options, ಧ್ರುವ ವರ್ಣ *option_name)
{
	ಯದಿ(config->per_listener_settings){
		ಯದಿ(*cur_listener == ರಿಕ್ತ){
			ಯದಿ(config__create_default_listener(config, option_name)){
				ಪ್ರತ್ಯಾಗಮ;
			}
			*cur_listener = config->default_listener;
		}
		(*security_options) = (*cur_listener)->security_options;
	}ಅನ್ಯಥಾ{
		(*security_options) = &config->security_options;
	}
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ conf__attempt_resolve(ಧ್ರುವ ವರ್ಣ *host, ಧ್ರುವ ವರ್ಣ *text, unsigned ಪೂರ್ಣಾಙ್ಕ log, ಧ್ರುವ ವರ್ಣ *ಸಂದೇಶ)
{
	ಸಂರಚನಾ addrinfo gai_hints;
	ಸಂರಚನಾ addrinfo *gai_res;
	ಪೂರ್ಣಾಙ್ಕ rc;

	memset(&gai_hints, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ addrinfo));
	gai_hints.ai_family = AF_UNSPEC;
	gai_hints.ai_socktype = SOCK_STREAM;
	gai_res = ರಿಕ್ತ;
	rc = getaddrinfo(host, ರಿಕ್ತ, &gai_hints, &gai_res);
	ಯದಿ(gai_res){
		freeaddrinfo(gai_res);
	}
	ಯದಿ(rc != 0){
#ifndef WIN32
		ಯದಿ(rc == EAI_SYSTEM){
			ಯದಿ(errno == ENOENT){
				log__printf(ರಿಕ್ತ, log, "%s: Unable to resolve %s %s.", ಸಂದೇಶ, text, host);
			}ಅನ್ಯಥಾ{
				log__printf(ರಿಕ್ತ, log, "%s: Error resolving %s: %s.", ಸಂದೇಶ, text, strerror(errno));
			}
		}ಅನ್ಯಥಾ{
			log__printf(ರಿಕ್ತ, log, "%s: Error resolving %s: %s.", ಸಂದೇಶ, text, gai_strerror(rc));
		}
#ಅನ್ಯಥಾ
		ಯದಿ(rc == WSAHOST_NOT_FOUND){
			log__printf(ರಿಕ್ತ, log, "%s: Error resolving %s.", ಸಂದೇಶ, text);
		}
#endif
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಶೂನ್ಯ config__init_reload(ಸಂರಚನಾ mosquitto__config *config)
{
	/* Set defaults */
	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<config->listener_ಎಣಿಕೆ; i++){
		listener__set_defaults(&config->listeners[i]);
	}

	config->local_only = true;
	config->allow_duplicate_messages = true;

	mosquitto_FREE(config->security_options.acl_data.acl_file);
	mosquitto_FREE(config->security_options.password_data.password_file);
	mosquitto_FREE(config->security_options.psk_file);

	config->security_options.allow_anonymous = -1;
	config->security_options.allow_zero_length_clientid = true;
	config->security_options.auto_id_prefix = ರಿಕ್ತ;
	config->security_options.auto_id_prefix_len = 0;

	config->autosave_interval = 1800;
	config->autosave_on_changes = false;

	mosquitto_FREE(config->clientid_prefixes);

	config->connection_messages = true;
	config->clientid_prefixes = ರಿಕ್ತ;
	config->per_listener_settings = false;
	ಯದಿ(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = ರಿಕ್ತ;
	}
	mosquitto_FREE(config->log_file);

#ಯದಿ defined(WIN32) || defined(__CYGWIN__)
	ಯದಿ(service_handle){
		/* This is running as a Windows service. Default to no logging. Using
		 * stdout/stderr is forbidden because the first clients to connect will
		 * get log information sent to them for some reason. */
		config->log_dest = MQTT3_LOG_NONE;
	}ಅನ್ಯಥಾ{
		config->log_dest = MQTT3_LOG_STDERR;
	}
#ಅನ್ಯಥಾ
	config->log_facility = LOG_DAEMON;
	config->log_dest = MQTT3_LOG_STDERR | MQTT3_LOG_DLT;
	ಯದಿ(db.quiet){
		config->log_type = 0;
	}ಅನ್ಯಥಾ ಯದಿ(db.verbose){
		config->log_type = UINT_MAX;
	}ಅನ್ಯಥಾ{
		config->log_type = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;
	}
#endif
	config->log_timestamp = true;
	mosquitto_FREE(config->log_timestamp_format);
	config->global_max_clients = -1;
	config->global_max_connections = -1;
	config->log_timestamp_format = ರಿಕ್ತ;
	config->max_keepalive = 0;
	config->max_packet_size = 2000000;
	config->max_inflight_messages = 20;
	config->max_queued_messages = 1000;
	config->max_inflight_bytes = 0;
	config->max_queued_bytes = 0;
	config->persistence = false;
	mosquitto_FREE(config->persistence_location);
	mosquitto_FREE(config->persistence_file);
	config->persistent_client_expiration = 0;
	config->queue_qos0_messages = false;
	config->retain_available = true;
	config->retain_expiry_interval = 0;
	config->set_tcp_nodelay = false;
	config->sys_interval = 10;
	config->upgrade_outgoing_qos = false;
	config->packet_buffer_size = 4096;

	config->packet_max_auth = 100000;
	config->packet_max_connect = 100000;
	config->packet_max_sub = 100000;
	config->packet_max_simple = 10000;
}


ಸ್ಥಿರ ಶೂನ್ಯ config__cleanup_plugin_config(mosquitto_plugin_id_t *plugin)
{
	mosquitto_FREE(plugin->config.path);
	mosquitto_FREE(plugin->config.name);

	ಯದಿ(plugin->config.options){
		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ j=0; j<plugin->config.option_ಎಣಿಕೆ; j++){
			mosquitto_FREE(plugin->config.options[j].ಕೀಲಿ);
			mosquitto_FREE(plugin->config.options[j].ಮೌಲ್ಯ);
		}
		mosquitto_FREE(plugin->config.options);
		plugin->config.option_ಎಣಿಕೆ = 0;
	}
	mosquitto_FREE(plugin->config.security_options);
	mosquitto_FREE(plugin);
}


ಸ್ಥಿರ ಶೂನ್ಯ config__cleanup_plugins(ಶೂನ್ಯ)
{
	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<db.plugin_ಎಣಿಕೆ; i++){
		config__cleanup_plugin_config(db.plugins[i]);
	}
	mosquitto_FREE(db.plugins);
}


ಶೂನ್ಯ config__init(ಸಂರಚನಾ mosquitto__config *config)
{
	memset(config, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__config));
	config__init_reload(config);

	config->daemon = false;
}


ಶೂನ್ಯ config__cleanup(ಸಂರಚನಾ mosquitto__config *config)
{
	mosquitto_FREE(config->clientid_prefixes);
	mosquitto_FREE(config->persistence_location);
	mosquitto_FREE(config->persistence_file);
	mosquitto_FREE(config->persistence_filepath);
	mosquitto_FREE(config->security_options.auto_id_prefix);
	mosquitto_FREE(config->security_options.acl_data.acl_file);
	mosquitto_FREE(config->security_options.password_data.password_file);
	mosquitto_FREE(config->security_options.psk_file);
	mosquitto_FREE(config->security_options.plugins);
	mosquitto_FREE(config->pid_file);
	mosquitto_FREE(config->user);
	mosquitto_FREE(config->log_timestamp_format);
	ಯದಿ(config->listeners){
		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<config->listener_ಎಣಿಕೆ; i++){
			mosquitto_FREE(config->listeners[i].host);
			mosquitto_FREE(config->listeners[i].bind_interface);
			mosquitto_FREE(config->listeners[i].mount_point);
			mosquitto_FREE(config->listeners[i].socks);
			ಯದಿ(config->listeners[i].security_options){
				mosquitto_FREE(config->listeners[i].security_options->auto_id_prefix);
				mosquitto_FREE(config->listeners[i].security_options->acl_data.acl_file);
				mosquitto_FREE(config->listeners[i].security_options->password_data.password_file);
				mosquitto_FREE(config->listeners[i].security_options->psk_file);
				mosquitto_FREE(config->listeners[i].security_options->plugins);
				mosquitto_FREE(config->listeners[i].security_options);
			}
#ifdef WITH_TLS
			mosquitto_FREE(config->listeners[i].cafile);
			mosquitto_FREE(config->listeners[i].capath);
			mosquitto_FREE(config->listeners[i].certfile);
			mosquitto_FREE(config->listeners[i].keyfile);
			mosquitto_FREE(config->listeners[i].ciphers);
			mosquitto_FREE(config->listeners[i].ciphers_tls13);
			mosquitto_FREE(config->listeners[i].psk_hint);
			mosquitto_FREE(config->listeners[i].crlfile);
			mosquitto_FREE(config->listeners[i].tls_version);
			mosquitto_FREE(config->listeners[i].tls_engine);
			mosquitto_FREE(config->listeners[i].tls_engine_kpass_sha1);
#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			ಯದಿ(!config->listeners[i].ws_context) /* libwebsockets frees its own SSL_CTX */
#endif
			{
				SSL_CTX_free(config->listeners[i].ssl_ctx);
				config->listeners[i].ssl_ctx = ರಿಕ್ತ;
			}
#endif
#ಯದಿ defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
			mosquitto_FREE(config->listeners[i].http_dir);
#endif
#ifdef WITH_WEBSOCKETS
			ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ j=0; j<config->listeners[i].ws_origin_ಎಣಿಕೆ; j++){
				mosquitto_FREE(config->listeners[i].ws_origins[j]);
			}
			mosquitto_FREE(config->listeners[i].ws_origins);
#endif
#ifdef WITH_UNIX_SOCKETS
			mosquitto_FREE(config->listeners[i].unix_socket_path);
#endif
		}
		mosquitto_FREE(config->listeners);
	}
#ifdef WITH_BRIDGE
	ಯದಿ(config->bridges){
		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<config->bridge_ಎಣಿಕೆ; i++){
			config__bridge_cleanup(config->bridges[i]);
		}
		mosquitto_FREE(config->bridges);
	}
#endif
	config__cleanup_plugins();

	ಯದಿ(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = ರಿಕ್ತ;
	}
	ಯದಿ(config->log_file){
		mosquitto_FREE(config->log_file);
		config->log_file = ರಿಕ್ತ;
	}
}

#ifdef WITH_BRIDGE


ಶೂನ್ಯ config__bridge_cleanup(ಸಂರಚನಾ mosquitto__bridge *bridge)
{
	ಯದಿ(bridge == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ;
	}

	mosquitto_FREE(bridge->name);
	ಯದಿ(bridge->addresses){
		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<bridge->address_ಎಣಿಕೆ; i++){
			mosquitto_FREE(bridge->addresses[i].address);
		}
		mosquitto_FREE(bridge->addresses);
	}
	mosquitto_FREE(bridge->bind_address);
	mosquitto_FREE(bridge->remote_clientid);
	mosquitto_FREE(bridge->remote_username);
	mosquitto_FREE(bridge->remote_password);
	mosquitto_FREE(bridge->local_clientid);
	mosquitto_FREE(bridge->local_username);
	mosquitto_FREE(bridge->local_password);
	ಯದಿ(bridge->topics){
		ಸಂರಚನಾ mosquitto__bridge_topic *cur_topic, *topic_tmp;

		LL_FOREACH_SAFE(bridge->topics, cur_topic, topic_tmp){
			mosquitto_FREE(cur_topic->topic);
			mosquitto_FREE(cur_topic->local_prefix);
			mosquitto_FREE(cur_topic->remote_prefix);
			mosquitto_FREE(cur_topic->local_topic);
			mosquitto_FREE(cur_topic->remote_topic);
			LL_DELETE(bridge->topics, cur_topic);
			mosquitto_FREE(cur_topic);
		}
		mosquitto_FREE(bridge->topics);
	}
	mosquitto_FREE(bridge->notification_topic);
#ifdef WITH_TLS
	mosquitto_FREE(bridge->tls_certfile);
	mosquitto_FREE(bridge->tls_keyfile);
	mosquitto_FREE(bridge->tls_version);
	mosquitto_FREE(bridge->tls_cafile);
	mosquitto_FREE(bridge->tls_capath);
	mosquitto_FREE(bridge->tls_alpn);
	mosquitto_FREE(bridge->tls_ciphers);
	mosquitto_FREE(bridge->tls_13_ciphers);
#ifdef FINAL_WITH_TLS_PSK
	mosquitto_FREE(bridge->tls_psk_identity);
	mosquitto_FREE(bridge->tls_psk);
#endif
#endif
	mosquitto_FREE(bridge);
}
#endif


ಸ್ಥಿರ ಶೂನ್ಯ print_version(ಶೂನ್ಯ)
{
	printf("mosquitto %s\n", VERSION);
	printf("Copyright © 2025 Roger Light.\n");
	printf("License EPL-2.0 OR BSD-3-Clause.\n");
}


ಸ್ಥಿರ ಶೂನ್ಯ print_usage(ಶೂನ್ಯ)
{
	printf("mosquitto version %s\n\n", VERSION);
	printf("mosquitto is an MQTT v5.0/v3.1.1/v3.1 broker.\n\n");
	printf("Usage: mosquitto [-c config_file] [-d] [-h] [-p port] [-v]\n");
	printf("                 [--tls-keylog file]\n\n");
	printf(" -c : specify the broker config file.\n");
	printf(" -d : put the broker into the background after starting.\n");
	printf(" -h : display this help.\n");
	printf(" -p : start the broker listening on the specified port.\n");
	printf("      Not recommended in conjunction with the -c option.\n");
	printf(" -q : quiet mode - disable all logging types. This overrides\n");
	printf("      any logging options given in the config file, and -v.\n");
	printf(" -v : verbose mode - enable all logging types. This overrides\n");
	printf("      any logging options given in the config file.\n");
	printf(" --test-config : test config file and exit\n");
	printf(" --tls-keylog : log TLS connection information to a file, to allow\n");
	printf("      debugging with e.g. wireshark. Do not use on a production\n");
	printf("      server.\n");
	printf("\nSee https://mosquitto.org/ for more information.\n\n");
}


ಪೂರ್ಣಾಙ್ಕ config__parse_args(ಸಂರಚನಾ mosquitto__config *config, ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ *argv[])
{
	ಪೂರ್ಣಾಙ್ಕ i;
	ಪೂರ್ಣಾಙ್ಕ port_tmp;

	ಪ್ರತಿ(i=1; i<argc; i++){
		ಯದಿ(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--config-file")){
			ಯದಿ(i<argc-1){
				db.config_file = argv[i+1];

				ಯದಿ(config__read(config, false)){
					ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
				}
			}ಅನ್ಯಥಾ{
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: -c argument given, but no config file specified.");
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
			}
			i++;
		}ಅನ್ಯಥಾ ಯದಿ(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--daemon")){
			config->daemon = true;
		}ಅನ್ಯಥಾ ಯದಿ(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
			print_usage();
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
		}ಅನ್ಯಥಾ ಯದಿ(!strcmp(argv[i], "--version")){
			print_version();
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
		}ಅನ್ಯಥಾ ಯದಿ(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port")){
			ಯದಿ(i<argc-1){
				port_tmp = atoi(argv[i+1]);
				ಯದಿ(port_tmp<1 || port_tmp>UINT16_MAX){
					log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid port specified (%d).", port_tmp);
					ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
				}ಅನ್ಯಥಾ{
					ಯದಿ(config->cmd_port_ಎಣಿಕೆ == CMD_PORT_LIMIT){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Only %d ports can be specified on the command line.", CMD_PORT_LIMIT);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					config->cmd_port[config->cmd_port_ಎಣಿಕೆ] = (uint16_t)port_tmp;
					config->cmd_port_ಎಣಿಕೆ++;
				}
			}ಅನ್ಯಥಾ{
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: -p argument given, but no port specified.");
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
			}
			i++;
		}ಅನ್ಯಥಾ ಯದಿ(!strcmp(argv[i], "--tls-keylog")){
#ifdef WITH_TLS
			ಯದಿ(i<argc-1){
				db.tls_keylog = mosquitto_strdup(argv[i+1]);
				ಯದಿ(db.tls_keylog == ರಿಕ್ತ){
					log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
					ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
				}
			}ಅನ್ಯಥಾ{
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: --tls-keylog argument given, but no file specified.");
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
			}
			i++;
#ಅನ್ಯಥಾ
			fprintf(stderr, "Error: TLS support not available so --tls-keylog is not available.\n");
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
#endif
		}ಅನ್ಯಥಾ ಯದಿ(!strcmp(argv[i], "-q") || !strcmp(argv[i], "--quiet")){
			db.quiet = true;
		}ಅನ್ಯಥಾ ಯದಿ(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")){
			db.verbose = true;
		}ಅನ್ಯಥಾ ಯದಿ(!strcmp(argv[i], "--test-config")){
			config->test_configuration = true;
		}ಅನ್ಯಥಾ{
			fprintf(stderr, "Error: Unknown option '%s'.\n", argv[i]);
			print_usage();
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
	}

	/* Default to drop to mosquitto user if we are privileged and no user specified. */
	ಯದಿ(!config->user){
		config->user = mosquitto_strdup("mosquitto");
		ಯದಿ(config->user == ರಿಕ್ತ){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}
	}
	ಯದಿ(db.quiet){
		config->log_type = 0;
	}ಅನ್ಯಥಾ ಯದಿ(db.verbose){
		config->log_type = UINT_MAX;
	}

	ಯದಿ(getenv("MOSQUITTO_PERSISTENCE_LOCATION")){
		mosquitto_FREE(config->persistence_location);
		config->persistence_location = mosquitto_strdup(getenv("MOSQUITTO_PERSISTENCE_LOCATION"));
		ಯದಿ(!config->persistence_location){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}
	}
	ಪ್ರತ್ಯಾಗಮ config__check(config);
}


ಸ್ಥಿರ ಶೂನ್ಯ config__copy(ಸಂರಚನಾ mosquitto__config *src, ಸಂರಚನಾ mosquitto__config *dest)
{
	mosquitto_FREE(dest->security_options.acl_data.acl_file);
	dest->security_options.acl_data.acl_file = src->security_options.acl_data.acl_file;

	acl_file__cleanup(&dest->security_options.acl_data);
	dest->security_options.acl_data.acl_users = src->security_options.acl_data.acl_users;
	dest->security_options.acl_data.acl_patterns = src->security_options.acl_data.acl_patterns;
	dest->security_options.acl_data.acl_anon.username = src->security_options.acl_data.acl_anon.username;
	dest->security_options.acl_data.acl_anon.acl = src->security_options.acl_data.acl_anon.acl;

	dest->security_options.allow_anonymous = src->security_options.allow_anonymous;
	dest->security_options.allow_zero_length_clientid = src->security_options.allow_zero_length_clientid;

	mosquitto_FREE(dest->security_options.auto_id_prefix);
	dest->security_options.auto_id_prefix = src->security_options.auto_id_prefix;
	dest->security_options.auto_id_prefix_len = src->security_options.auto_id_prefix_len;

	mosquitto_FREE(dest->security_options.password_data.password_file);
	dest->security_options.password_data.password_file = src->security_options.password_data.password_file;

	password_file__cleanup(&dest->security_options.password_data);
	dest->security_options.password_data.unpwd = src->security_options.password_data.unpwd;

	mosquitto_FREE(dest->security_options.psk_file);
	dest->security_options.psk_file = src->security_options.psk_file;

	mosquitto_FREE(dest->security_options.plugins);
	dest->security_options.plugin_ಎಣಿಕೆ = src->security_options.plugin_ಎಣಿಕೆ;
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

	dest->message_size_limit = src->message_size_limit;

	dest->persistence = src->persistence;

	mosquitto_FREE(dest->persistence_location);
	dest->persistence_location = src->persistence_location;

	mosquitto_FREE(dest->persistence_file);
	dest->persistence_file = src->persistence_file;

	mosquitto_FREE(dest->persistence_filepath);
	dest->persistence_filepath = src->persistence_filepath;

	dest->persistent_client_expiration = src->persistent_client_expiration;


	dest->queue_qos0_messages = src->queue_qos0_messages;
	dest->sys_interval = src->sys_interval;
	dest->upgrade_outgoing_qos = src->upgrade_outgoing_qos;

#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	dest->websockets_log_level = src->websockets_log_level;
#endif

#ifdef WITH_BRIDGE
	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<dest->bridge_ಎಣಿಕೆ; i++){
		ಯದಿ(dest->bridges[i]){
			config__bridge_cleanup(dest->bridges[i]);
		}
	}
	mosquitto_FREE(dest->bridges);
	dest->bridges = src->bridges;
	dest->bridge_ಎಣಿಕೆ = src->bridge_ಎಣಿಕೆ;
#endif
}


ಪೂರ್ಣಾಙ್ಕ config__read(ಸಂರಚನಾ mosquitto__config *config, bool reload)
{
	ಪೂರ್ಣಾಙ್ಕ rc = MOSQ_ERR_SUCCESS;
	ಸಂರಚನಾ config_recurse cr;
	ಪೂರ್ಣಾಙ್ಕ lineno = 0;
#ifdef WITH_PERSISTENCE
	size_t len;
#endif
	ಸಂರಚನಾ mosquitto__config config_reload;
	ಪೂರ್ಣಾಙ್ಕ i;

	ಯದಿ(reload){
		memset(&config_reload, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__config));
	}

	cr.log_dest = MQTT3_LOG_NONE;
	cr.log_dest_set = 0;
	cr.log_type = MOSQ_LOG_NONE;
	cr.log_type_set = 0;

	ಯದಿ(!db.config_file){
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	ಯದಿ(reload){
		/* Re-initialise appropriate config vars to default for reload. */
		config__init_reload(&config_reload);
		config_reload.listeners = config->listeners;
		config_reload.listener_ಎಣಿಕೆ = config->listener_ಎಣಿಕೆ;
		cur_security_options = ರಿಕ್ತ;
		rc = config__read_file(&config_reload, reload, db.config_file, &cr, 0, &lineno);
	}ಅನ್ಯಥಾ{
		rc = config__read_file(config, reload, db.config_file, &cr, 0, &lineno);
	}
	ಯದಿ(rc){
		ಯದಿ(lineno > 0){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error found at %s:%d.", db.config_file, lineno);
		}
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಯದಿ(reload){
		config__copy(&config_reload, config);
	}

	/* If auth/access options are set and allow_anonymous not explicitly set, disallow anon. */
	ಯದಿ(config->local_only == false){
		ಯದಿ(config->per_listener_settings){
			ಪ್ರತಿ(i=0; i<config->listener_ಎಣಿಕೆ; i++){
				/* Default option if no security options set */
				ಯದಿ(config->listeners[i].security_options->allow_anonymous == -1){
					config->listeners[i].security_options->allow_anonymous = false;
				}
			}
		}ಅನ್ಯಥಾ{
			ಯದಿ(config->security_options.allow_anonymous == -1){
				config->security_options.allow_anonymous = false;
			}
		}
	}
#ifdef WITH_PERSISTENCE
	ಯದಿ(config->persistence){
		ಯದಿ(!config->persistence_file){
			config->persistence_file = mosquitto_strdup("mosquitto.db");
			ಯದಿ(!config->persistence_file){
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
			}
		}
		mosquitto_FREE(config->persistence_filepath);
		ಯದಿ(config->persistence_location && strlen(config->persistence_location)){
			len = strlen(config->persistence_location) + strlen(config->persistence_file) + 2;
			config->persistence_filepath = mosquitto_malloc(len);
			ಯದಿ(!config->persistence_filepath){
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
			}
#ifdef WIN32
			snprintf(config->persistence_filepath, len, "%s\\%s", config->persistence_location, config->persistence_file);
#ಅನ್ಯಥಾ
			snprintf(config->persistence_filepath, len, "%s/%s", config->persistence_location, config->persistence_file);
#endif
		}ಅನ್ಯಥಾ{
			config->persistence_filepath = mosquitto_strdup(config->persistence_file);
			ಯದಿ(!config->persistence_filepath){
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
			}
		}
	}
#endif
	/* Default to drop to mosquitto user if no other user specified. This must
	 * remain here even though it is covered in config__parse_args() because this
	 * function may be called on its own. */
	ಯದಿ(!config->user){
		config->user = mosquitto_strdup("mosquitto");
	}

#ifdef WITH_BRIDGE
	ಪ್ರತಿ(i=0; i<config->bridge_ಎಣಿಕೆ; i++){
		ಯದಿ(!config->bridges[i]->name){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: bridge name not defined.");
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
		ಯದಿ(config->bridges[i]->addresses  == 0){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no remote addresses defined.");
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
		ಯದಿ(config->bridges[i]->topic_ಎಣಿಕೆ == 0){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no topics defined.");
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
#ifdef FINAL_WITH_TLS_PSK
		ಯದಿ(config->bridges[i]->tls_psk && !config->bridges[i]->tls_psk_identity){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_identity.");
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
		ಯದಿ(config->bridges[i]->tls_psk_identity && !config->bridges[i]->tls_psk){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_psk.");
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
#endif
	}
#endif

	ಯದಿ(cr.log_dest_set){
		config->log_dest = cr.log_dest;
	}
	ಯದಿ(db.quiet){
		config->log_type = 0;
	}ಅನ್ಯಥಾ ಯದಿ(db.verbose){
		config->log_type = UINT_MAX;
	}ಅನ್ಯಥಾ ಯದಿ(cr.log_type_set){
		config->log_type = cr.log_type;
	}
#ifdef WITH_BRIDGE
	ಪ್ರತ್ಯಾಗಮ config__check_bridges(config);
#ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
#endif
}


ಸ್ಥಿರ mosquitto_plugin_id_t *config__plugin_find(ಧ್ರುವ ವರ್ಣ *name)
{
	ಯದಿ(db.plugins && name){
		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<db.plugin_ಎಣಿಕೆ; i++){
			ಯದಿ(db.plugins[i]->config.name && !strcmp(db.plugins[i]->config.name, name)){
				ಪ್ರತ್ಯಾಗಮ db.plugins[i];
			}
		}
	}
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}


ಸ್ಥಿರ mosquitto_plugin_id_t *config__plugin_load(ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ ವರ್ಣ *path)
{
	mosquitto_plugin_id_t *plugin = ರಿಕ್ತ;
	mosquitto_plugin_id_t **plugins = ರಿಕ್ತ;

	ಯದಿ(name && config__plugin_find(name)){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Duplicate plugin name '%s'.", name);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	ಯದಿ(!path || !strcmp(path, "")){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Missing plugin path for plugin name '%s'.", name);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	plugin = mosquitto_calloc(1, ಪರಿಮಾಣ(mosquitto_plugin_id_t));
	ಯದಿ(!plugin){
		ಗಚ್ಛ error;
	}

	ಯದಿ(name){
		plugin->config.name = mosquitto_strdup(name);
	}
	plugin->config.path = mosquitto_strdup(path);
	ಯದಿ((name && !plugin->config.name) || !plugin->config.path){
		ಗಚ್ಛ error;
	}
	plugin->config.options = ರಿಕ್ತ;
	plugin->config.option_ಎಣಿಕೆ = 0;
	plugin->config.deny_special_chars = true;

	/* Add to db list */
	plugins = mosquitto_realloc(db.plugins, (size_t)(db.plugin_ಎಣಿಕೆ+1)*ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__plugin_config *));
	ಯದಿ(!plugins){
		ಗಚ್ಛ error;
	}

	plugins[db.plugin_ಎಣಿಕೆ] = plugin;
	db.plugins = plugins;
	db.plugin_ಎಣಿಕೆ++;

	ಪ್ರತ್ಯಾಗಮ plugin;
error:
	log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
	ಯದಿ(plugin){
		mosquitto_FREE(plugin->config.name);
		mosquitto_FREE(plugin->config.path);
	}
	mosquitto_FREE(plugin);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}


ಪೂರ್ಣಾಙ್ಕ config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, ಸಂರಚನಾ mosquitto__security_options *security_options)
{
	ಸಂರಚನಾ mosquitto__security_options **new_options;
	mosquitto_plugin_id_t **new_plugins;

	new_options = mosquitto_realloc(plugin->config.security_options, (size_t)(plugin->config.security_option_ಎಣಿಕೆ+1)*ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__security_options *));
	new_plugins = mosquitto_realloc(security_options->plugins, (size_t)(security_options->plugin_ಎಣಿಕೆ+1)*ಪರಿಮಾಣ(mosquitto_plugin_id_t *));

	ಯದಿ(!new_options || !new_plugins){
		mosquitto_FREE(new_options);
		mosquitto_FREE(new_plugins);
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}

	new_options[plugin->config.security_option_ಎಣಿಕೆ] = security_options;
	plugin->config.security_options = new_options;
	plugin->config.security_option_ಎಣಿಕೆ++;

	new_plugins[security_options->plugin_ಎಣಿಕೆ] = plugin;
	security_options->plugins = new_plugins;
	security_options->plugin_ಎಣಿಕೆ++;

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ config__read_file_core(ಸಂರಚನಾ mosquitto__config *config, bool reload, ಸಂರಚನಾ config_recurse *cr, ಪೂರ್ಣಾಙ್ಕ level, ಪೂರ್ಣಾಙ್ಕ *lineno, FILE *fptr, ವರ್ಣ **buf, ಪೂರ್ಣಾಙ್ಕ *buflen)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ವರ್ಣ *token;
	ಪೂರ್ಣಾಙ್ಕ tmp_int;
	ವರ್ಣ *saveptr = ರಿಕ್ತ;
#ifdef WITH_BRIDGE
	ವರ್ಣ *tmp_char;
	ಸಂರಚನಾ mosquitto__bridge *cur_bridge = ರಿಕ್ತ;
#endif
	mosquitto_plugin_id_t *cur_plugin = ರಿಕ್ತ;

	ವರ್ಣ *ಕೀಲಿ;
	ಸಂರಚನಾ mosquitto__listener *cur_listener = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ i;
	ಪೂರ್ಣಾಙ್ಕ lineno_ext = 0;
	size_t prefix_len;
	size_t slen;
#ifdef WITH_WEBSOCKETS
	ವರ್ಣ **ws_origins = ರಿಕ್ತ;
#endif

	*lineno = 0;

	ಯಾವತ್(mosquitto_fgets(buf, buflen, fptr)){
		(*lineno)++;
		ಯದಿ((*buf)[0] != '#' && (*buf)[0] != 10 && (*buf)[0] != 13){
			slen = strlen(*buf);
			ಯದಿ(slen == 0){
				ಅನುವರ್ತ;
			}
			ಯಾವತ್((*buf)[slen-1] == 10 || (*buf)[slen-1] == 13){
				(*buf)[slen-1] = 0;
				slen = strlen(*buf);
				ಯದಿ(slen == 0){
					ಅನುವರ್ತ;
				}
			}
			token = strtok_r((*buf), " ", &saveptr);
			ಯದಿ(token){
				ಯದಿ(!strcmp(token, "accept_protocol_versions")){
					REQUIRE_NON_DEFAULT_LISTENER(token);
					cur_listener->disable_protocol_v3 = true;
					cur_listener->disable_protocol_v4 = true;
					cur_listener->disable_protocol_v5 = true;
					ಯದಿ(saveptr == ರಿಕ್ತ){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", "accept_protocol_versions");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					token = strtok_r(saveptr, ", \t", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "accept_protocol_versions");

					ಯಾವತ್(token){
						ಯದಿ(!strcmp(token, "3")){
							cur_listener->disable_protocol_v3 = false;
						}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "4")){
							cur_listener->disable_protocol_v4 = false;
						}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "5")){
							cur_listener->disable_protocol_v5 = false;
						}

						token = strtok_r(ರಿಕ್ತ, ", \t", &saveptr);
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "acl_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);

					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->acl_data.acl_file);
					ಯದಿ(conf__parse_string(&token, "acl_file", &cur_security_options->acl_data.acl_file, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "address") || !strcmp(token, "addresses")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಯದಿ(cur_bridge->addresses){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration, 'address' only allowed once.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯಾವತ್((token = strtok_r(ರಿಕ್ತ, " ", &saveptr))){
						ಯದಿ(token[0] == '#'){
							ಭಙ್ಗ;
						}
						ಸಂರಚನಾ bridge_address *new_addresses = mosquitto_realloc(cur_bridge->addresses, ಪರಿಮಾಣ(ಸಂರಚನಾ bridge_address)*(size_t)(cur_bridge->address_ಎಣಿಕೆ+1));
						ಯದಿ(!new_addresses){
							log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
							ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
						}
						cur_bridge->address_ಎಣಿಕೆ++;
						cur_bridge->addresses = new_addresses;
						memset(&cur_bridge->addresses[cur_bridge->address_ಎಣಿಕೆ-1], 0, ಪರಿಮಾಣ(ಸಂರಚನಾ bridge_address));
						cur_bridge->addresses[cur_bridge->address_ಎಣಿಕೆ-1].address = mosquitto_strdup(token);
						ಯದಿ(!cur_bridge->addresses[cur_bridge->address_ಎಣಿಕೆ-1].address){
							log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
							ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
						}
					}
					ಪ್ರತಿ(i=0; i<cur_bridge->address_ಎಣಿಕೆ; i++){
						/* cur_bridge->addresses[i].address is now
						 * "address[:port]". If address is an IPv6 address,
						 * then port is required. We must check for the :
						 * backwards. */
						tmp_char = strrchr(cur_bridge->addresses[i].address, ':');
						ಯದಿ(tmp_char){
							/* Remove ':', so cur_bridge->addresses[i].address
							 * now just looks like the address. */
							tmp_char[0] = '\0';

							/* The remainder of the string */
							tmp_int = atoi(&tmp_char[1]);
							ಯದಿ(tmp_int < 1 || tmp_int > UINT16_MAX){
								log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid bridge port value (%d).", tmp_int);
								ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
							}
							cur_bridge->addresses[i].port = (uint16_t)tmp_int;
						}ಅನ್ಯಥಾ{
							cur_bridge->addresses[i].port = 1883;
						}
						conf__attempt_resolve(cur_bridge->addresses[i].address, "bridge address", MOSQ_LOG_WARNING, "Warning");
					}
					ಯದಿ(cur_bridge->address_ಎಣಿಕೆ == 0){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Empty address value in configuration.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "allow_anonymous")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					PER_LISTENER_ALTERNATIVE(token, "listener_allow_anonymous");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ಯದಿ(conf__parse_bool(&token, "allow_anonymous", (bool *)&cur_security_options->allow_anonymous, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "allow_duplicate_messages")){
					OPTION_DEPRECATED(token, "The behaviour will default to true.");
					ಯದಿ(conf__parse_bool(&token, "allow_duplicate_messages", &config->allow_duplicate_messages, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "allow_zero_length_clientid")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ಯದಿ(conf__parse_bool(&token, "allow_zero_length_clientid", &cur_security_options->allow_zero_length_clientid, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strncmp(token, "auth_opt_", strlen("auth_opt_")) || !strncmp(token, "plugin_opt_", strlen("plugin_opt_"))){
					ಯದಿ(reload){
						ಅನುವರ್ತ;        /* Auth plugin not currently valid for reloading. */

					}
					REQUIRE_PLUGIN(token);

					ಯದಿ(!strncmp(token, "auth_opt_", strlen("auth_opt_"))){
						prefix_len = strlen("auth_opt_");
					}ಅನ್ಯಥಾ{
						prefix_len = strlen("plugin_opt_");
					}
					ಯದಿ(strlen(token) < prefix_len + 3){
						/* auth_opt_ == 9, + one digit key == 10, + one space == 11, + one value == 12 */
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid 'plugin_opt_' config option.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಕೀಲಿ = mosquitto_strdup(&token[prefix_len]);
					ಯದಿ(!ಕೀಲಿ){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
					}ಅನ್ಯಥಾ ಯದಿ(STREMPTY(ಕೀಲಿ)){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Empty 'plugin_opt_' config option.");
						mosquitto_FREE(ಕೀಲಿ);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					token = saveptr;
					ಯದಿ(token && token[0]){
						ಯಾವತ್(token[0] == ' ' || token[0] == '\t'){
							token++;
						}
						ಸಂರಚನಾ mosquitto_opt *new_options = mosquitto_realloc(cur_plugin->config.options, (size_t)(cur_plugin->config.option_ಎಣಿಕೆ+1)*ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto_opt));
						ಯದಿ(!new_options){
							log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
							mosquitto_FREE(ಕೀಲಿ);
							ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
						}
						cur_plugin->config.option_ಎಣಿಕೆ++;
						cur_plugin->config.options = new_options;
						cur_plugin->config.options[cur_plugin->config.option_ಎಣಿಕೆ-1].ಕೀಲಿ = ಕೀಲಿ;
						cur_plugin->config.options[cur_plugin->config.option_ಎಣಿಕೆ-1].ಮೌಲ್ಯ = mosquitto_strdup(token);
						ಯದಿ(!cur_plugin->config.options[cur_plugin->config.option_ಎಣಿಕೆ-1].ಮೌಲ್ಯ){
							log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
							ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
						}
					}ಅನ್ಯಥಾ{
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", ಕೀಲಿ);
						mosquitto_FREE(ಕೀಲಿ);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "auth_plugin") || !strcmp(token, "plugin") || !strcmp(token, "global_plugin")){
					ಯದಿ(reload){
						ಅನುವರ್ತ;        /* plugin not currently valid for reloading. */
					}
					ಯದಿ(!strcmp(token, "global_plugin")){
						cur_security_options = &db.config->security_options;
					}ಅನ್ಯಥಾ{
						REQUIRE_LISTENER_IF_PER_LISTENER(token);
						conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					}

					cur_plugin = config__plugin_load(ರಿಕ್ತ, saveptr);
					ಯದಿ(cur_plugin == ರಿಕ್ತ){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(config__plugin_add_secopt(cur_plugin, cur_security_options)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "auth_plugin_deny_special_chars")){
					ಯದಿ(reload){
						ಅನುವರ್ತ;        /* Auth plugin not currently valid for reloading. */
					}
					ಯದಿ(!cur_plugin){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: An auth_plugin_deny_special_chars option exists in the config file without a plugin.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(conf__parse_bool(&token, "auth_plugin_deny_special_chars", &cur_plugin->config.deny_special_chars, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "plugin_load")){
					ವರ್ಣ *name = ರಿಕ್ತ;

					ಯದಿ(reload){
						ಅನುವರ್ತ;        /* plugin not currently valid for reloading. */

					}
					name = strtok_r(ರಿಕ್ತ, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(name, "plugin_load");

					cur_plugin = config__plugin_load(name, saveptr);
					ಯದಿ(cur_plugin == ರಿಕ್ತ){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "plugin_use")){
					ವರ್ಣ *name = ರಿಕ್ತ;

					ಯದಿ(reload){
						ಅನುವರ್ತ;        /* plugin not currently valid for reloading. */
					}
					REQUIRE_NON_DEFAULT_LISTENER(token);

					ಯದಿ(conf__parse_string(&token, "plugin_use", &name, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					cur_plugin = config__plugin_find(name);
					ಯದಿ(!cur_plugin){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Plugin '%s' not previously loaded.", name);
						mosquitto_FREE(name);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(name);
					ಯದಿ(config__plugin_add_secopt(cur_plugin, cur_listener->security_options)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "auto_id_prefix")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					OPTION_DEPRECATED(token, "Please use 'listener_auto_id_prefix' instead.");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ಯದಿ(conf__parse_string(&token, "auto_id_prefix", &cur_security_options->auto_id_prefix, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(cur_security_options->auto_id_prefix){
						ಯದಿ(strlen(cur_security_options->auto_id_prefix) > 50){
							log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: auto_id_prefix length must be <= 50.");
							ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
						}
						cur_security_options->auto_id_prefix_len = (uint16_t)strlen(cur_security_options->auto_id_prefix);
					}ಅನ್ಯಥಾ{
						cur_security_options->auto_id_prefix_len = 0;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "autosave_interval")){
					ಯದಿ(conf__parse_int(&token, "autosave_interval", &config->autosave_interval, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(config->autosave_interval < 0){
						config->autosave_interval = 0;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "autosave_on_changes")){
					ಯದಿ(conf__parse_bool(&token, "autosave_on_changes", &config->autosave_on_changes, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "bind_address")){
					OPTION_DEPRECATED(token, "");
					config->local_only = false;
					ಯದಿ(reload){
						ಅನುವರ್ತ;        /* Rebinding listeners not valid during reloading. */

					}
					ಯದಿ(config__create_default_listener(config, token)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					ಯದಿ(conf__parse_string(&token, "default listener bind_address", &config->default_listener->host, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(conf__attempt_resolve(config->default_listener->host, "bind_address", MOSQ_LOG_ERR, "Error")){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "bind_interface")){
#ifdef SO_BINDTODEVICE
					ಯದಿ(reload){
						ಅನುವರ್ತ;        /* Rebinding listeners not valid during reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ಯದಿ(conf__parse_string(&token, "bind_interface", &cur_listener->bind_interface, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: bind_interface specified but socket option not available.");
					ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "bridge_attempt_unsubscribe")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_bool(&token, "bridge_attempt_unsubscribe", &cur_bridge->attempt_unsubscribe, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "bridge_cafile")){
#ಯದಿ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					ಯದಿ(conf__parse_string(&token, "bridge_cafile", &cur_bridge->tls_cafile, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "bridge_alpn")){
#ಯದಿ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_string(&token, "bridge_alpn", &cur_bridge->tls_alpn, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "bridge_ciphers")){
#ಯದಿ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_string(&token, "bridge_ciphers", &cur_bridge->tls_ciphers, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "bridge_ciphers_tls1.3")){
#ಯದಿ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_string(&token, "bridge_ciphers_tls1.3", &cur_bridge->tls_13_ciphers, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "bridge_bind_address")){
#ಯದಿ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_string(&token, "bridge_bind_address", &cur_bridge->bind_address, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "bridge_capath")){
#ಯದಿ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					ಯದಿ(conf__parse_string(&token, "bridge_capath", &cur_bridge->tls_capath, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "bridge_certfile")){
#ಯದಿ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					ಯದಿ(conf__parse_string(&token, "bridge_certfile", &cur_bridge->tls_certfile, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "bridge_identity")){
#ಯದಿ defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					ಯದಿ(conf__parse_string(&token, "bridge_identity", &cur_bridge->tls_psk_identity, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "bridge_insecure")){
#ಯದಿ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_bool(&token, "bridge_insecure", &cur_bridge->tls_insecure, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(cur_bridge->tls_insecure){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge '%s' using insecure mode.", cur_bridge->name);
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "bridge_require_ocsp")){
#ಯದಿ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_bool(&token, "bridge_require_ocsp", &cur_bridge->tls_ocsp_required, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "bridge_max_packet_size")){
#ಯದಿ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_int(&token, "bridge_max_packet_size", &tmp_int, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(tmp_int < 0){
						tmp_int = 0;
					}
					cur_bridge->maximum_packet_size = (uint32_t)tmp_int;
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "bridge_max_topic_alias")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_int(&token, "bridge_max_topic_alias", &tmp_int, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}

					ಯದಿ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: bridge_max_topic_alias must be > 0 and <= 65535.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					cur_bridge->max_topic_alias = (uint16_t)tmp_int;
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "bridge_outgoing_retain")){
#ಯದಿ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_bool(&token, "bridge_outgoing_retain", &cur_bridge->outgoing_retain, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "bridge_keyfile")){
#ಯದಿ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					mosquitto_FREE(cur_bridge->tls_keyfile);
					ಯದಿ(conf__parse_string(&token, "bridge_keyfile", &cur_bridge->tls_keyfile, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "bridge_protocol_version")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(ರಿಕ್ತ, "", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_protocol_version");

					ಯದಿ(!strcmp(token, "mqttv31")){
						cur_bridge->protocol_version = mosq_p_mqtt31;
					}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "mqttv311")){
						cur_bridge->protocol_version = mosq_p_mqtt311;
					}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "mqttv50")){
						cur_bridge->protocol_version = mosq_p_mqtt5;
					}ಅನ್ಯಥಾ{
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid 'bridge_protocol_version' value (%s).", token);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "bridge_psk")){
#ಯದಿ defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					ಯದಿ(conf__parse_string(&token, "bridge_psk", &cur_bridge->tls_psk, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "bridge_receive_maximum")){
#ಯದಿ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_int(&token, "bridge_receive_maximum", &tmp_int, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(tmp_int <= 0){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be greater than 0.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}ಅನ್ಯಥಾ ಯದಿ((uint64_t)tmp_int > (uint64_t)UINT16_MAX){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be lower than %u.", UINT16_MAX);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					cur_bridge->receive_maximum = (uint16_t)tmp_int;
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "bridge_reload_type")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(ರಿಕ್ತ, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_reload_type");

					ಯದಿ(!strcmp(token, "lazy")){
						cur_bridge->reload_type = brt_lazy;
					}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "immediate")){
						cur_bridge->reload_type = brt_immediate;
					}ಅನ್ಯಥಾ{
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid 'bridge_reload_type' value in configuration (%s).", token);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "bridge_session_expiry_interval")){
#ಯದಿ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_int(&token, "bridge_session_expiry_interval", &tmp_int, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(tmp_int < 0){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must not be negative.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}ಅನ್ಯಥಾ ಯದಿ((uint64_t)tmp_int > (uint64_t)MQTT_SESSION_EXPIRY_NEVER){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must be lower than %u.", MQTT_SESSION_EXPIRY_NEVER);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					cur_bridge->session_expiry_interval = (uint32_t)tmp_int;
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "bridge_tcp_keepalive")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);

					ಯದಿ(conf__parse_int(&token, "bridge_tcp_keepalive_idle", &tmp_int, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(tmp_int <= 0){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: invalid TCP keepalive idle value.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_idle = (unsigned ಪೂರ್ಣಾಙ್ಕ)tmp_int;

					ಯದಿ(conf__parse_int(&token, "bridge_tcp_keepalive_interval", &tmp_int, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(tmp_int <= 0){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: invalid TCP keepalive interval value.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_interval = (unsigned ಪೂರ್ಣಾಙ್ಕ)tmp_int;

					ಯದಿ(conf__parse_int(&token, "bridge_tcp_keepalive_counter", &tmp_int, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(tmp_int <= 0){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: invalid TCP keepalive counter value.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_counter = (unsigned ಪೂರ್ಣಾಙ್ಕ)tmp_int;
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "bridge_tcp_user_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
#ifdef WITH_TCP_USER_TIMEOUT
					ಯದಿ(conf__parse_int(&token, "bridge_tcp_user_timeout", &tmp_int, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(tmp_int < 0){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: invalid TCP user timeout value.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_user_timeout = tmp_int;
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge TCP user timeout support not available.");
#endif
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "bridge_tls_use_os_certs")){
#ಯದಿ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_bool(&token, "bridge_tls_use_os_certs", &cur_bridge->tls_use_os_certs, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "bridge_tls_version")){
#ಯದಿ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_string(&token, "bridge_tls_version", &cur_bridge->tls_version, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "cafile")){
#ಯದಿ defined(WITH_TLS)
					REQUIRE_LISTENER(token);
					ಯದಿ(cur_listener->psk_hint){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->cafile);
					ಯದಿ(conf__parse_string(&token, "cafile", &cur_listener->cafile, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "capath")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->capath);
					ಯದಿ(conf__parse_string(&token, "capath", &cur_listener->capath, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "certfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ಯದಿ(cur_listener->psk_hint){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->certfile);
					ಯದಿ(conf__parse_string(&token, "certfile", &cur_listener->certfile, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "check_retain_source")){
					ಯದಿ(conf__parse_bool(&token, "check_retain_source", &config->check_retain_source, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "ciphers")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers);
					ಯದಿ(conf__parse_string(&token, "ciphers", &cur_listener->ciphers, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "ciphers_tls1.3")){
#ಯದಿ defined(WITH_TLS) && (!defined(LIBRESSL_VERSION_ಸಂಖ್ಯೆ) || LIBRESSL_VERSION_ಸಂಖ್ಯೆ > 0x3040000FL)
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers_tls13);
					ಯದಿ(conf__parse_string(&token, "ciphers_tls1.3", &cur_listener->ciphers_tls13, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: ciphers_tls1.3 support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "clientid") || !strcmp(token, "remote_clientid")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_string(&token, "bridge remote clientid", &cur_bridge->remote_clientid, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "cleansession")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_bool(&token, "cleansession", &cur_bridge->clean_start, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "local_cleansession")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_bool(&token, "local_cleansession", (bool *)&cur_bridge->clean_start_local, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "clientid_prefixes")){
					OPTION_DEPRECATED(token, "");
					mosquitto_FREE(config->clientid_prefixes);
					ಯದಿ(conf__parse_string(&token, "clientid_prefixes", &config->clientid_prefixes, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "connection")){
#ifdef WITH_BRIDGE
					token = strtok_r(ರಿಕ್ತ, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "connection");

					/* Check for existing bridge name. */
					ಪ್ರತಿ(i=0; i<config->bridge_ಎಣಿಕೆ; i++){
						ಯದಿ(!strcmp(config->bridges[i]->name, token)){
							log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Duplicate bridge name '%s'.", token);
							ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
						}
					}

					ಸಂರಚನಾ mosquitto__bridge **bridges_new = mosquitto_realloc(config->bridges, (size_t)(config->bridge_ಎಣಿಕೆ+1)*ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__bridge *));
					ಯದಿ(!bridges_new){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
					}
					config->bridges = bridges_new;
					cur_bridge = mosquitto_malloc(ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__bridge));
					ಯದಿ(!cur_bridge){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
					}

					config->bridge_ಎಣಿಕೆ++;
					config->bridges[config->bridge_ಎಣಿಕೆ-1] = cur_bridge;

					memset(cur_bridge, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__bridge));
					cur_bridge->name = mosquitto_strdup(token);
					ಯದಿ(!cur_bridge->name){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
					}
					cur_bridge->keepalive = 60;
					cur_bridge->notifications = true;
					cur_bridge->notifications_local_only = false;
					cur_bridge->start_type = bst_automatic;
					cur_bridge->idle_timeout = 60;
					cur_bridge->restart_timeout = 0;
					cur_bridge->backoff_base = 5 * 1000;
					cur_bridge->backoff_cap = 30 * 1000;
					cur_bridge->stable_connection_period = 0;
					cur_bridge->threshold = 10;
					cur_bridge->try_private = true;
					cur_bridge->attempt_unsubscribe = true;
					cur_bridge->protocol_version = mosq_p_mqtt311;
					cur_bridge->primary_retry_sock = INVALID_SOCKET;
					cur_bridge->outgoing_retain = true;
					cur_bridge->clean_start_local = -1;
					cur_bridge->reload_type = brt_lazy;
					cur_bridge->max_topic_alias = 10;
#ifdef WITH_TCP_USER_TIMEOUT
					cur_bridge->tcp_user_timeout = -1;
#endif
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "connection_messages")){
					ಯದಿ(conf__parse_bool(&token, token, &config->connection_messages, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "crlfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->crlfile);
					ಯದಿ(conf__parse_string(&token, "crlfile", &cur_listener->crlfile, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "dhparamfile")){
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: dhparamfile is no longer required.");
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "disable_client_cert_date_checks")){
#ifdef WITH_TLS
					REQUIRE_LISTENER(token);
					ಯದಿ(conf__parse_bool(&token, "disable_client_cert_date_checks", &cur_listener->disable_client_cert_date_checks, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "enable_control_api")){
#ifdef WITH_CONTROL
					ಯದಿ(conf__parse_bool(&token, "enable_control_api", &config->enable_control_api, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: $CONTROL support not available (enable_control_api).");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "enable_proxy_protocol")){
#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
#endif
					REQUIRE_LISTENER(token);
					ಯದಿ(conf__parse_int(&token, "enable_proxy_protocol", &cur_listener->enable_proxy_protocol, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(cur_listener->enable_proxy_protocol < 1 || cur_listener->enable_proxy_protocol > 2){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: enable_proxy_protocol must be 1 or 2.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "global_max_clients")){
					ಯದಿ(conf__parse_int(&token, "global_max_clients", &config->global_max_clients, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "global_max_connections")){
					ಯದಿ(conf__parse_int(&token, "global_max_connections", &config->global_max_connections, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "http_dir")){
#ಯದಿ defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
					ಯದಿ(reload){
						ಅನುವರ್ತ;        /* Not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					ಯದಿ(conf__parse_string(&token, "http_dir", &cur_listener->http_dir, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ifdef WIN32
					ವರ್ಣ *http_dir_canonical = _fullpath(ರಿಕ್ತ, cur_listener->http_dir, 0);
					ಧ್ರುವ ವರ್ಣ sep = '\\';
#ಅನ್ಯಥಾ
					ವರ್ಣ *http_dir_canonical = realpath(cur_listener->http_dir, ರಿಕ್ತ);
					ಧ್ರುವ ವರ್ಣ sep = '/';
#endif
					ಯದಿ(!http_dir_canonical){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
					}
					size_t http_dir_len = strlen(http_dir_canonical) + ಪರಿಮಾಣ(sep) + 1;
					ವರ್ಣ *http_dir_canonical_sep = mosquitto_calloc(http_dir_len, ಪರಿಮಾಣ(ವರ್ಣ));
					ಯದಿ(!http_dir_canonical_sep){
						ಮುಕ್ತ(http_dir_canonical);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
					}
					snprintf(http_dir_canonical_sep, http_dir_len, "%s%c", http_dir_canonical, sep);
					ಮುಕ್ತ(http_dir_canonical);
					mosquitto_FREE(cur_listener->http_dir);
					cur_listener->http_dir = http_dir_canonical_sep;
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: http_dir support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "idle_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_int(&token, "idle_timeout", &cur_bridge->idle_timeout, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(cur_bridge->idle_timeout < 1){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "idle_timeout interval too low, using 1 second.");
						cur_bridge->idle_timeout = 1;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "include_dir")){
					ಯದಿ(level == 0){
						ವರ್ಣ **files;
						ಪೂರ್ಣಾಙ್ಕ file_ಎಣಿಕೆ;

						/* Only process include_dir from the main config file. */
						token = strtok_r(ರಿಕ್ತ, "", &saveptr);
						REQUIRE_NON_EMPTY_OPTION(token, "include_dir");

						rc = config__get_dir_files(token, &files, &file_ಎಣಿಕೆ);
						ಯದಿ(rc){
							ಪ್ರತ್ಯಾಗಮ rc;
						}

						ಪ್ರತಿ(i=0; i<file_ಎಣಿಕೆ; i++){
							log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Loading config file '%s'", files[i]);

							rc = config__read_file(config, reload, files[i], cr, level+1, &lineno_ext);
							ಯದಿ(rc){
								ಯದಿ(lineno_ext > 0){
									log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error found at '%s:%d'.", files[i], lineno_ext);
								}
								/* Free happens below */
								ಭಙ್ಗ;
							}
						}
						ಪ್ರತಿ(i=0; i<file_ಎಣಿಕೆ; i++){
							mosquitto_FREE(files[i]);
						}
						mosquitto_FREE(files);
						ಯದಿ(rc){
							ಪ್ರತ್ಯಾಗಮ rc;    /* This returns if config__read_file() fails above */
						}
					}ಅನ್ಯಥಾ{
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: The include_dir option is only valid in the main configuration file.");
						ಪ್ರತ್ಯಾಗಮ 1;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "keepalive_interval")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_int(&token, "keepalive_interval", &tmp_int, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(tmp_int > UINT16_MAX){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Bridge keepalive value too high.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(tmp_int < 5){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "keepalive interval too low, using 5 seconds.");
						tmp_int = 5;
					}
					cur_bridge->keepalive = (uint16_t)tmp_int;
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "keyfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->keyfile);
					ಯದಿ(conf__parse_string(&token, "keyfile", &cur_listener->keyfile, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "listener")){
					config->local_only = false;

					ಯದಿ(conf__parse_int(&token, "listener port", &tmp_int, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ifdef WITH_UNIX_SOCKETS
					ಯದಿ(tmp_int < 0 || tmp_int > UINT16_MAX){
#ಅನ್ಯಥಾ
					ಯದಿ(tmp_int < 1 || tmp_int > UINT16_MAX){
#endif
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}

					/* Look for bind address / unix socket path */
					token = strtok_r(ರಿಕ್ತ, " ", &saveptr);
					ಯದಿ(token != ರಿಕ್ತ && token[0] == '#'){
						token = ರಿಕ್ತ;
					}

					ಯದಿ(tmp_int == 0 && token == ರಿಕ್ತ){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: A listener with port 0 must provide a Unix socket path.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}

					ಯದಿ(reload){
						/* We reload listeners settings based on port number/unix socket path.
						 * If the port number/unix path doesn't already exist, exit with a complaint. */
						cur_listener = ರಿಕ್ತ;
#ifdef WITH_UNIX_SOCKETS
						ಯದಿ(tmp_int == 0){
							ಪ್ರತಿ(i=0; i<config->listener_ಎಣಿಕೆ; i++){
								ಯದಿ(config->listeners[i].unix_socket_path != ರಿಕ್ತ
										&& strcmp(config->listeners[i].unix_socket_path, token) == 0){

									cur_listener = &config->listeners[i];
									ಭಙ್ಗ;
								}
							}
						}ಅನ್ಯಥಾ
#endif
						{
							ಪ್ರತಿ(i=0; i<config->listener_ಎಣಿಕೆ; i++){
								ಯದಿ(config->listeners[i].port == tmp_int){
									/* Now check we have a matching bind address, if defined */
									ಯದಿ(config->listeners[i].host){
										ಯದಿ(token && !strcmp(config->listeners[i].host, token)){
											/* They both have a bind address, and they match */
											cur_listener = &config->listeners[i];
											ಭಙ್ಗ;
										}
									}ಅನ್ಯಥಾ{
										ಯದಿ(token == ರಿಕ್ತ){
											/* Neither this config nor the new config have a bind address,
											 * so they match. */
											cur_listener = &config->listeners[i];
											ಭಙ್ಗ;
										}
									}
								}
							}
						}
						ಯದಿ(!cur_listener){
							log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: It is not currently possible to add/remove listeners when reloading the config file.");
							ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
						}
					}ಅನ್ಯಥಾ{
						ಯದಿ(config__add_listener(config)){
							log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
							ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
						}
						cur_listener = &config->listeners[config->listener_ಎಣಿಕೆ-1];
					}

					listener__set_defaults(cur_listener);
					cur_listener->port = (uint16_t)tmp_int;

					mosquitto_FREE(cur_listener->host);

#ifdef WITH_UNIX_SOCKETS
					mosquitto_FREE(cur_listener->unix_socket_path);
#endif

					ಯದಿ(token){
#ifdef WITH_UNIX_SOCKETS
						ಯದಿ(cur_listener->port == 0){
							cur_listener->unix_socket_path = mosquitto_strdup(token);
						}ಅನ್ಯಥಾ
#endif
						{
							cur_listener->host = mosquitto_strdup(token);
						}
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "listener_allow_anonymous")){
					REQUIRE_LISTENER(token);
					ಯದಿ(conf__parse_bool(&token, "listener_allow_anonymous", (bool *)&cur_listener->security_options->allow_anonymous, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "listener_auto_id_prefix")){
					REQUIRE_LISTENER(token);
					ಯದಿ(conf__parse_string(&token, "listener_auto_id_prefix", &cur_listener->security_options->auto_id_prefix, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(cur_listener->security_options->auto_id_prefix){
						ಯದಿ(strlen(cur_listener->security_options->auto_id_prefix) > 50){
							log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: 'listener_auto_id_prefix' length must be <= 50.");
							ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
						}
						cur_listener->security_options->auto_id_prefix_len = (uint16_t)strlen(cur_listener->security_options->auto_id_prefix);
					}ಅನ್ಯಥಾ{
						cur_listener->security_options->auto_id_prefix_len = 0;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "local_clientid")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_string(&token, "bridge local clientd", &cur_bridge->local_clientid, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "local_password")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_string(&token, "bridge local_password", &cur_bridge->local_password, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "local_username")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_string(&token, "bridge local_username", &cur_bridge->local_username, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "log_dest")){
					token = strtok_r(ರಿಕ್ತ, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_dest");

					cr->log_dest_set = 1;
					ಯದಿ(!strcmp(token, "none")){
						cr->log_dest = MQTT3_LOG_NONE;
					}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "syslog")){
						cr->log_dest |= MQTT3_LOG_SYSLOG;
					}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "stdout")){
						cr->log_dest |= MQTT3_LOG_STDOUT;
					}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "stderr")){
						cr->log_dest |= MQTT3_LOG_STDERR;
					}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "topic")){
						cr->log_dest |= MQTT3_LOG_TOPIC;
					}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "dlt")){
						cr->log_dest |= MQTT3_LOG_DLT;
#ifdef ANDROID
					}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "android")){
						cr->log_dest |= MQTT3_LOG_ANDROID;
#endif
					}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "file")){
						ಯದಿ(config->log_fptr || config->log_file){
							log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Duplicate \"log_dest file\" value.");
							ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
						}
						/* Get remaining string. */
						token = saveptr;
						ಯದಿ(token && token[0]){
							ಯಾವತ್(token[0] == ' ' || token[0] == '\t'){
								token++;
							}
						}
						/* Duplicate "token" check here saves a log__printf() */
						ಯದಿ(token && token[0]){
							config->log_file = mosquitto_strdup(token);
							ಯದಿ(!config->log_file){
								log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
								ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
							}
							cr->log_dest |= MQTT3_LOG_FILE;
						}ಅನ್ಯಥಾ{
							log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Empty \"log_dest file\" value in configuration.");
							ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
						}
						cr->log_dest |= MQTT3_LOG_FILE;
					}ಅನ್ಯಥಾ{
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid 'log_dest' value (%s).", token);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಯದಿ defined(WIN32) || defined(__CYGWIN__)
					ಯದಿ(service_handle){
						ಯದಿ(cr->log_dest == MQTT3_LOG_STDOUT || cr->log_dest == MQTT3_LOG_STDERR){
							log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Cannot log to stdout/stderr when running as a Windows service.");
							ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
						}
					}
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "log_facility")){
#ಯದಿ defined(WIN32) || defined(__CYGWIN__)
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: log_facility not supported on Windows.");
#ಅನ್ಯಥಾ
					ಯದಿ(conf__parse_int(&token, "log_facility", &tmp_int, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ವಿಕಲ್ಪ(tmp_int){
						ಪಕ್ಷ 0:
							config->log_facility = LOG_LOCAL0;
							ಭಙ್ಗ;
						ಪಕ್ಷ 1:
							config->log_facility = LOG_LOCAL1;
							ಭಙ್ಗ;
						ಪಕ್ಷ 2:
							config->log_facility = LOG_LOCAL2;
							ಭಙ್ಗ;
						ಪಕ್ಷ 3:
							config->log_facility = LOG_LOCAL3;
							ಭಙ್ಗ;
						ಪಕ್ಷ 4:
							config->log_facility = LOG_LOCAL4;
							ಭಙ್ಗ;
						ಪಕ್ಷ 5:
							config->log_facility = LOG_LOCAL5;
							ಭಙ್ಗ;
						ಪಕ್ಷ 6:
							config->log_facility = LOG_LOCAL6;
							ಭಙ್ಗ;
						ಪಕ್ಷ 7:
							config->log_facility = LOG_LOCAL7;
							ಭಙ್ಗ;
						ಸ್ವಭಾವ:
							log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid 'log_facility' value (%d).", tmp_int);
							ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "log_timestamp")){
					ಯದಿ(conf__parse_bool(&token, token, &config->log_timestamp, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "log_timestamp_format")){
					ಯದಿ(conf__parse_string(&token, token, &config->log_timestamp_format, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "log_type")){
					token = strtok_r(ರಿಕ್ತ, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_type");

					cr->log_type_set = 1;
					ಯದಿ(!strcmp(token, "none")){
						cr->log_type = MOSQ_LOG_NONE;
					}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "information")){
						cr->log_type |= MOSQ_LOG_INFO;
					}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "notice")){
						cr->log_type |= MOSQ_LOG_NOTICE;
					}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "warning")){
						cr->log_type |= MOSQ_LOG_WARNING;
					}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "error")){
						cr->log_type |= MOSQ_LOG_ERR;
					}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "debug")){
						cr->log_type |= MOSQ_LOG_DEBUG;
					}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "subscribe")){
						cr->log_type |= MOSQ_LOG_SUBSCRIBE;
					}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "unsubscribe")){
						cr->log_type |= MOSQ_LOG_UNSUBSCRIBE;
					}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "internal")){
						cr->log_type |= MOSQ_LOG_INTERNAL;
#ifdef WITH_WEBSOCKETS
					}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "websockets")){
						cr->log_type |= MOSQ_LOG_WEBSOCKETS;
#endif
					}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "all")){
						cr->log_type = MOSQ_LOG_ALL;
					}ಅನ್ಯಥಾ{
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid 'log_type' value (%s).", token);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "max_connections")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ಯದಿ(conf__parse_int(&token, token, &cur_listener->max_connections, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(cur_listener->max_connections < 0){
						cur_listener->max_connections = -1;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "maximum_qos") || !strcmp(token, "max_qos")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ಯದಿ(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(tmp_int < 0 || tmp_int > 2){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: 'max_qos' must be between 0 and 2 inclusive.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					cur_listener->max_qos = (uint8_t)tmp_int;
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "max_inflight_bytes")){
					ಯದಿ(conf__parse_int(&token, "max_inflight_bytes", &tmp_int, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_inflight_bytes = (size_t)tmp_int;
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "max_inflight_messages")){
					ಯದಿ(conf__parse_int(&token, "max_inflight_messages", &tmp_int, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(tmp_int < 0 || tmp_int == UINT16_MAX){
						tmp_int = 0;
					}ಅನ್ಯಥಾ ಯದಿ(tmp_int > UINT16_MAX){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: 'max_inflight_messages' must be <= 65535.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					config->max_inflight_messages = (uint16_t)tmp_int;
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "max_keepalive")){
					ಯದಿ(conf__parse_int(&token, "max_keepalive", &tmp_int, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid 'max_keepalive' value (%d).", tmp_int);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					config->max_keepalive = (uint16_t)tmp_int;
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "max_packet_size")){
					ಯದಿ(conf__parse_int(&token, "max_packet_size", &tmp_int, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(tmp_int < 20){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: 'max_packet_size' must be >= 20.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					config->max_packet_size = (uint32_t)tmp_int;
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "max_queued_bytes")){
					ಯದಿ(conf__parse_int(&token, "max_queued_bytes", &tmp_int, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_bytes = (size_t)tmp_int;
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "max_queued_messages")){
					ಯದಿ(conf__parse_int(&token, "max_queued_messages", &tmp_int, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_messages = tmp_int;
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "memory_limit")){
					ssize_t lim;
					ಯದಿ(conf__parse_ssize_t(&token, "memory_limit", &lim, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(lim < 0){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid 'memory_limit' value (%ld).", lim);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					mosquitto_memory_set_limit((size_t)lim);
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "message_size_limit")){
					log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Note: It is recommended to replace `message_size_limit` with `max_packet_size`.");
					ಯದಿ(conf__parse_int(&token, "message_size_limit", (ಪೂರ್ಣಾಙ್ಕ *)&config->message_size_limit, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(config->message_size_limit > MQTT_MAX_PAYLOAD){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid 'message_size_limit' value (%u).", config->message_size_limit);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "mount_point")){
					REQUIRE_LISTENER(token);
					mosquitto_FREE(cur_listener->mount_point);
					ಯದಿ(conf__parse_string(&token, "mount_point", &cur_listener->mount_point, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(mosquitto_pub_topic_check(cur_listener->mount_point) != MOSQ_ERR_SUCCESS){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR,
								"Error: Invalid 'mount_point' value (%s). Does it contain a wildcard character?",
								cur_listener->mount_point);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "notifications")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_bool(&token, "notifications", &cur_bridge->notifications, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "notifications_local_only")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_bool(&token, "notifications_local_only", &cur_bridge->notifications_local_only, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "notification_topic")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_string(&token, "notification_topic", &cur_bridge->notification_topic, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "password") || !strcmp(token, "remote_password")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_string(&token, "bridge remote_password", &cur_bridge->remote_password, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "password_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->password_data.password_file);
					ಯದಿ(conf__parse_string(&token, "password_file", &cur_security_options->password_data.password_file, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "per_listener_settings")){
					OPTION_DEPRECATED(token, "Please see the documentation for how to achieve the same effect.");
					ಯದಿ(config->per_listener_settings){
						/* Once this is set, don't let it be unset. It should be the first config option ideally. */
						ಅನುವರ್ತ;
					}
					ಯದಿ(conf__parse_bool(&token, "per_listener_settings", &config->per_listener_settings, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(cur_security_options && config->per_listener_settings){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: per_listener_settings must be set before any other security settings.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "persistence") || !strcmp(token, "retained_persistence")){
					ಯದಿ(conf__parse_bool(&token, token, &config->persistence, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "persistence_file")){
					ಯದಿ(conf__parse_string(&token, "persistence_file", &config->persistence_file, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "persistence_location")){
					ಯದಿ(conf__parse_string(&token, "persistence_location", &config->persistence_location, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "persistent_client_expiration")){
					token = strtok_r(ರಿಕ್ತ, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "persistent_client_expiration");

					time_t expiration_mult;

					ವಿಕಲ್ಪ(token[strlen(token)-1]){
						ಪಕ್ಷ 's':
							expiration_mult = 1;
							ಭಙ್ಗ;
						ಪಕ್ಷ 'h':
							expiration_mult = 3600;
							ಭಙ್ಗ;
						ಪಕ್ಷ 'd':
							expiration_mult = 86400;
							ಭಙ್ಗ;
						ಪಕ್ಷ 'w':
							expiration_mult = 86400*7;
							ಭಙ್ಗ;
						ಪಕ್ಷ 'm':
							expiration_mult = 86400*30;
							ಭಙ್ಗ;
						ಪಕ್ಷ 'y':
							expiration_mult = 86400*365;
							ಭಙ್ಗ;
						ಸ್ವಭಾವ:
							log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
							ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					token[strlen(token)-1] = '\0';
					config->persistent_client_expiration = atoi(token)*expiration_mult;
					ಯದಿ(config->persistent_client_expiration <= 0){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "pid_file")){
					ಯದಿ(reload){
						ಅನುವರ್ತ;        /* pid file not valid for reloading. */
					}
					ಯದಿ(conf__parse_string(&token, "pid_file", &config->pid_file, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "port")){
					OPTION_DEPRECATED(token, "Please use 'listener' instead.");
					config->local_only = false;
					ಯದಿ(reload){
						ಅನುವರ್ತ;        /* Listeners not valid for reloading. */

					}
					ಯದಿ(config__create_default_listener(config, token)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					ಯದಿ(config->default_listener->port){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Default listener port specified multiple times. Only the latest will be used.");
					}
					ಯದಿ(conf__parse_int(&token, "port", &tmp_int, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(tmp_int < 1 || tmp_int > UINT16_MAX){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					config->default_listener->port = (uint16_t)tmp_int;
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "protocol")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(ರಿಕ್ತ, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "protocol");

					ಯದಿ(!strcmp(token, "mqtt")){
						cur_listener->protocol = mp_mqtt;
						/*
						}else if(!strcmp(token, "mqttsn")){
						    cur_listener->protocol = mp_mqttsn;
						*/
					}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "websockets")){
#ifdef WITH_WEBSOCKETS
						cur_listener->protocol = mp_websockets;
#ಅನ್ಯಥಾ
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Websockets support not available.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
#endif
					}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "http_api")){
#ifdef WITH_HTTP_API
						cur_listener->protocol = mp_http_api;
#ಅನ್ಯಥಾ
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: HTTP API support not available.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
#endif
					}ಅನ್ಯಥಾ{
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid 'protocol' value (%s).", token);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "proxy_protocol_v2_require_tls")){
#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
#endif
					REQUIRE_LISTENER(token);
					ಯದಿ(conf__parse_bool(&token, "proxy_protocol_v2_require_tls", &cur_listener->proxy_protocol_v2_require_tls, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "psk_file")){
#ifdef FINAL_WITH_TLS_PSK
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ಯದಿ(cur_listener && cur_listener->certfile){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_security_options->psk_file);
					ಯದಿ(conf__parse_string(&token, "psk_file", &cur_security_options->psk_file, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "psk_hint")){
#ifdef FINAL_WITH_TLS_PSK
					ಯದಿ(reload){
						ಅನುವರ್ತ;        /* PSK file not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ಯದಿ(conf__parse_string(&token, "psk_hint", &cur_listener->psk_hint, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "queue_qos0_messages")){
					ಯದಿ(conf__parse_bool(&token, token, &config->queue_qos0_messages, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "require_certificate")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ಯದಿ(conf__parse_bool(&token, "require_certificate", &cur_listener->require_certificate, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "restart_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					cur_bridge->backoff_cap = 0; /* set backoff to constant mode, unless cap is specified further down */
					token = strtok_r(ರಿಕ್ತ, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "restart_timeout");

					cur_bridge->restart_timeout = atoi(token);
					cur_bridge->backoff_base = 0;
					cur_bridge->backoff_cap = 0;
					ಯದಿ(cur_bridge->restart_timeout < 1){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "restart_timeout interval too low, using 1 second.");
						cur_bridge->restart_timeout = 1;
					}ಅನ್ಯಥಾ ಯದಿ(cur_bridge->restart_timeout > 3600){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "restart_timeout interval too high, using 3600 seconds.");
						cur_bridge->restart_timeout = 3600;
					}
					token = strtok_r(ರಿಕ್ತ, " ", &saveptr);
					ಯದಿ(token){
						cur_bridge->backoff_base = cur_bridge->restart_timeout;
						cur_bridge->backoff_cap = atoi(token);
						ಯದಿ(cur_bridge->backoff_cap < cur_bridge->backoff_base){
							log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: backoff cap is lower than the base in restart_timeout.");
							ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
						}ಅನ್ಯಥಾ ಯದಿ(cur_bridge->backoff_cap > 7200){
							log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: backoff cap too high, using 7200 seconds.");
							cur_bridge->backoff_cap = 7200;
						}

						token = strtok_r(ರಿಕ್ತ, " ", &saveptr);
						ಯದಿ(token){
							cur_bridge->stable_connection_period = atoi(token);
							ಯದಿ(cur_bridge->stable_connection_period < 0){
								log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: stable connection period cannot be negative.");
								ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
							}
						}
					}
					cur_bridge->restart_timeout *= 1000; /* backoff is tracked in ms */
					cur_bridge->backoff_base *= 1000;
					cur_bridge->backoff_cap *= 1000;
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "retain_available")){
					ಯದಿ(conf__parse_bool(&token, token, &config->retain_available, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "retain_expiry_interval")){
					ಯದಿ(conf__parse_int(&token, token, &config->retain_expiry_interval, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(config->retain_expiry_interval < 1){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Error: retain_expiry_interval must be >= 1.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}ಅನ್ಯಥಾ ಯದಿ(config->retain_expiry_interval > 10000000){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: retain_expiry_interval being capped at 19 years.");
						config->retain_expiry_interval = 10000000;
					}
					config->retain_expiry_interval *= 60;
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "retry_interval")){
					OPTION_UNAVAILABLE(token);
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "round_robin")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_bool(&token, "round_robin", &cur_bridge->round_robin, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "set_tcp_nodelay")){
					ಯದಿ(conf__parse_bool(&token, "set_tcp_nodelay", &config->set_tcp_nodelay, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "start_type")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(ರಿಕ್ತ, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					ಯದಿ(!strcmp(token, "automatic")){
						cur_bridge->start_type = bst_automatic;
					}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "lazy")){
						cur_bridge->start_type = bst_lazy;
					}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "manual")){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Manual start_type not supported.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "once")){
						cur_bridge->start_type = bst_once;
					}ಅನ್ಯಥಾ{
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid 'start_type' value in configuration (%s).", token);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "socket_domain")){
					ಯದಿ(reload){
						ಅನುವರ್ತ;        /* socket_domain not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(ರಿಕ್ತ, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					ಯದಿ(!strcmp(token, "ipv4")){
						cur_listener->socket_domain = AF_INET;
					}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "ipv6")){
						cur_listener->socket_domain = AF_INET6;
					}ಅನ್ಯಥಾ{
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid 'socket_domain' value '%s' in configuration.", token);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "sys_interval")){
					ಯದಿ(conf__parse_int(&token, "sys_interval", &config->sys_interval, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(config->sys_interval < 0 || config->sys_interval > 65535){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid 'sys_interval' value (%d).", config->sys_interval);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "threshold")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_int(&token, "threshold", &cur_bridge->threshold, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(cur_bridge->threshold < 1){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "threshold too low, using 1 message.");
						cur_bridge->threshold = 1;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "tls_engine")){
#ifdef WITH_TLS
					ಯದಿ(reload){
						ಅನುವರ್ತ;        /* tls_engine not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ಯದಿ(conf__parse_string(&token, "tls_engine", &cur_listener->tls_engine, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "tls_engine_kpass_sha1")){
#ifdef WITH_TLS
					ಯದಿ(reload){
						ಅನುವರ್ತ;        /* tls_engine not valid for reloading. */

					}
					ವರ್ಣ *kpass_sha = ರಿಕ್ತ, *kpass_sha_bin = ರಿಕ್ತ;
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ಯದಿ(conf__parse_string(&token, "tls_engine_kpass_sha1", &kpass_sha, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(mosquitto__hex2bin_sha1(kpass_sha, (unsigned ವರ್ಣ **)&kpass_sha_bin) != MOSQ_ERR_SUCCESS){
						mosquitto_FREE(kpass_sha);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					mosquitto_free(cur_listener->tls_engine_kpass_sha1);
					cur_listener->tls_engine_kpass_sha1 = kpass_sha_bin;
					mosquitto_FREE(kpass_sha);
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "tls_keyform")){
#ifdef WITH_TLS
					ಯದಿ(reload){
						ಅನುವರ್ತ;        /* tls_engine not valid for reloading. */

					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ವರ್ಣ *keyform = ರಿಕ್ತ;
					ಯದಿ(conf__parse_string(&token, "tls_keyform", &keyform, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					cur_listener->tls_keyform = mosq_k_pem;
					ಯದಿ(!strcmp(keyform, "engine")){
						cur_listener->tls_keyform = mosq_k_engine;
					}
					mosquitto_FREE(keyform);
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "tls_version")){
#ಯದಿ defined(WITH_TLS)
					ಯದಿ(reload){
						ಅನುವರ್ತ;        /* tls_version not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					ಯದಿ(conf__parse_string(&token, "tls_version", &cur_listener->tls_version, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "topic")){
#ifdef WITH_BRIDGE
					ವರ್ಣ *topic = ರಿಕ್ತ;
					ಗಣನಾ mosquitto__bridge_direction direction = bd_out;
					uint8_t qos = 0;
					ವರ್ಣ *local_prefix = ರಿಕ್ತ, *remote_prefix = ರಿಕ್ತ;

					REQUIRE_BRIDGE(token);

					token = strtok_r(ರಿಕ್ತ, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "topic");

					// Check if the topic is quoted (e.g. for spaces within topic names), but not the
					// special case of ""
					ಯದಿ(token[0] == '"' && token [1] != '"'){
						ಯದಿ(strchr(saveptr, '"') == ರಿಕ್ತ){
							log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Missing closing quote in topic value (%s).", saveptr);
							ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
						}

						ವರ್ಣ *topic_in_quotes = strtok_r(ರಿಕ್ತ, "\"", &saveptr);
						size_t tlen = 1;
						ಯದಿ(topic_in_quotes){
							tlen = strlen(topic_in_quotes);
						}

						topic = mosquitto_malloc(strlen(token) + tlen + 1);
						ಯದಿ(!topic){
							log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
							ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
						}

						strcpy(topic, token + 1);
						strcat(topic, " ");
						ಯದಿ(topic_in_quotes){
							strcat(topic, topic_in_quotes);
						}
					}ಅನ್ಯಥಾ{
						topic = mosquitto_strdup(token);
						ಯದಿ(!topic){
							log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
							ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
						}
					}

					token = strtok_r(ರಿಕ್ತ, " ", &saveptr);
					ಯದಿ(token){
						ಯದಿ(!strcasecmp(token, "out")){
							direction = bd_out;
						}ಅನ್ಯಥಾ ಯದಿ(!strcasecmp(token, "in")){
							direction = bd_in;
						}ಅನ್ಯಥಾ ಯದಿ(!strcasecmp(token, "both")){
							direction = bd_both;
						}ಅನ್ಯಥಾ{
							log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid bridge topic direction '%s'.", token);
							mosquitto_FREE(topic);
							ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
						}
						token = strtok_r(ರಿಕ್ತ, " ", &saveptr);
						ಯದಿ(token){
							ಯದಿ(token[0] == '#'){
								(ಶೂನ್ಯ)strtok_r(ರಿಕ್ತ, "", &saveptr);
							}
							qos = (uint8_t)atoi(token);
							ಯದಿ(qos > 2){
								log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid bridge QoS level '%s'.", token);
								mosquitto_FREE(topic);
								ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
							}

							token = strtok_r(ರಿಕ್ತ, " ", &saveptr);
							ಯದಿ(token){
								ಯದಿ(!strcmp(token, "\"\"") || token[0] == '#'){
									local_prefix = ರಿಕ್ತ;
									ಯದಿ(token[0] == '#'){
										(ಶೂನ್ಯ)strtok_r(ರಿಕ್ತ, "", &saveptr);
									}
								}ಅನ್ಯಥಾ{
									local_prefix = token;
								}

								token = strtok_r(ರಿಕ್ತ, " ", &saveptr);
								ಯದಿ(token){
									ಯದಿ(!strcmp(token, "\"\"") || token[0] == '#'){
										remote_prefix = ರಿಕ್ತ;
									}ಅನ್ಯಥಾ{
										remote_prefix = token;
									}
								}
							}
						}
					}

					ಯದಿ(bridge__add_topic(cur_bridge, topic, direction, qos, local_prefix, remote_prefix)){
						mosquitto_FREE(topic);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(topic);
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "max_topic_alias")){
					REQUIRE_LISTENER(token);
					ಯದಿ(conf__parse_int(&token, "max_topic_alias", &tmp_int, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias' value in configuration.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias = (uint16_t)tmp_int;
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "max_topic_alias_broker")){
					REQUIRE_LISTENER(token);
					ಯದಿ(conf__parse_int(&token, "max_topic_alias_broker", &tmp_int, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias_broker' value in configuration.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias_broker = (uint16_t)tmp_int;
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "try_private")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_bool(&token, "try_private", &cur_bridge->try_private, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "upgrade_outgoing_qos")){
					ಯದಿ(conf__parse_bool(&token, token, &config->upgrade_outgoing_qos, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "use_identity_as_username")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ಯದಿ(conf__parse_bool(&token, "use_identity_as_username", &cur_listener->use_identity_as_username, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "use_subject_as_username")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ಯದಿ(conf__parse_bool(&token, "use_subject_as_username", &cur_listener->use_subject_as_username, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "user")){
					ಯದಿ(reload){
						ಅನುವರ್ತ;        /* Drop privileges user not valid for reloading. */
					}
					mosquitto_FREE(config->user);
					ಯದಿ(conf__parse_string(&token, "user", &config->user, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "use_username_as_clientid")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ಯದಿ(conf__parse_bool(&token, "use_username_as_clientid", &cur_listener->use_username_as_clientid, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "username") || !strcmp(token, "remote_username")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಯದಿ(conf__parse_string(&token, "bridge remote_username", &cur_bridge->remote_username, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "websockets_log_level")){
#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					ಯದಿ(conf__parse_int(&token, "websockets_log_level", &config->websockets_log_level, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
#endif
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "websockets_headers_size") || !strcmp(token, "packet_buffer_size")){
					ಯದಿ(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಯದಿ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Error: Packet buffer size must be between 0 and 65535 inclusive.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					config->packet_buffer_size = (uint16_t)tmp_int;
				}ಅನ್ಯಥಾ ಯದಿ(!strcmp(token, "websockets_origin")){
#ifdef WITH_WEBSOCKETS
#  ಯದಿ LWS_LIBRARY_VERSION_ಸಂಖ್ಯೆ >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN
					REQUIRE_LISTENER(token);
					ws_origins = mosquitto_realloc(cur_listener->ws_origins, ಪರಿಮಾಣ(ವರ್ಣ *)*(size_t)(cur_listener->ws_origin_ಎಣಿಕೆ+1));
					ಯದಿ(ws_origins == ರಿಕ್ತ){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
					}
					ws_origins[cur_listener->ws_origin_ಎಣಿಕೆ] = ರಿಕ್ತ;
					cur_listener->ws_origins = ws_origins;
					ಯದಿ(conf__parse_string(&token, "websockets_origin", &ws_origins[cur_listener->ws_origin_ಎಣಿಕೆ], &saveptr)){
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					cur_listener->ws_origin_ಎಣಿಕೆ++;
#  ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: websockets_origin support not available, libwebsockets version is too old.");
#  endif
#ಅನ್ಯಥಾ
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Websockets support not available.");
#endif
				}ಅನ್ಯಥಾ{
					log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unknown configuration variable '%s'.", token);
					ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
				}
			}
		}
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ config__read_file(ಸಂರಚನಾ mosquitto__config *config, bool reload, ಧ್ರುವ ವರ್ಣ *file, ಸಂರಚನಾ config_recurse *cr, ಪೂರ್ಣಾಙ್ಕ level, ಪೂರ್ಣಾಙ್ಕ *lineno)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	FILE *fptr = ರಿಕ್ತ;
	ವರ್ಣ *buf;
	ಪೂರ್ಣಾಙ್ಕ buflen;
#ifndef WIN32
	DIR *dir;
#endif

#ifndef WIN32
	dir = opendir(file);
	ಯದಿ(dir){
		closedir(dir);
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Config file '%s' is a directory.", file);
		ಪ್ರತ್ಯಾಗಮ 1;
	}
#endif

	fptr = mosquitto_fopen(file, "rt", false);
	ಯದಿ(!fptr){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to open config file '%s'.", file);
		ಪ್ರತ್ಯಾಗಮ 1;
	}

	buflen = 1000;
	buf = mosquitto_malloc((size_t)buflen);
	ಯದಿ(!buf){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
		fclose(fptr);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}

	rc = config__read_file_core(config, reload, cr, level, lineno, fptr, &buf, &buflen);
	mosquitto_FREE(buf);
	fclose(fptr);

	ಪ್ರತ್ಯಾಗಮ rc;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ config__check_proxy(ಸಂರಚನಾ mosquitto__config *config)
{
	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<config->listener_ಎಣಿಕೆ; i++){
		ಸಂರಚನಾ mosquitto__listener *l = &config->listeners[i];

		ಯದಿ(l->enable_proxy_protocol == 2){
#ifdef WITH_TLS
			ಯದಿ(l->use_subject_as_username){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: use_subject_as_username cannot be used with `enable_proxy_protocol 2`.");
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
			}

			ಯದಿ(l->certfile || l->keyfile){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: certfile and keyfile cannot be used with `enable_proxy_protocol 2`.");
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
			}
#endif
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ config__check(ಸಂರಚನಾ mosquitto__config *config)
{
	/* Checks that are easy to make after the config has been loaded. */

	ಧ್ರುವ ವರ್ಣ *id_prefix;
	ಪೂರ್ಣಾಙ್ಕ id_prefix_len;
	ಯದಿ(config->security_options.auto_id_prefix){
		id_prefix = config->security_options.auto_id_prefix;
		id_prefix_len = config->security_options.auto_id_prefix_len;
	}ಅನ್ಯಥಾ{
		id_prefix = "auto-";
		id_prefix_len = (ಪೂರ್ಣಾಙ್ಕ)strlen("auto-");
	}

	/* Default to auto_id_prefix = 'auto-' if none set. */
	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<config->listener_ಎಣಿಕೆ; i++){
		ಯದಿ(!config->listeners[i].security_options->auto_id_prefix){
			config->listeners[i].security_options->auto_id_prefix = mosquitto_strdup(id_prefix);
			ಯದಿ(!config->listeners[i].security_options->auto_id_prefix){
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
			}
			config->listeners[i].security_options->auto_id_prefix_len = (uint16_t)id_prefix_len;
		}
	}

	ಪ್ರತ್ಯಾಗಮ config__check_proxy(config);
}

#ifdef WITH_BRIDGE


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ config__check_bridges(ಸಂರಚನಾ mosquitto__config *config)
{
	ಸಂರಚನಾ mosquitto__bridge *bridge1, *bridge2;
	ವರ್ಣ hostname[256];
	size_t len;

	/* Check for bridge duplicate local_clientid, need to generate missing IDs
	 * first. */
	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<config->bridge_ಎಣಿಕೆ; i++){
		bridge1 = config->bridges[i];

		ಯದಿ(!bridge1->remote_clientid){
			ಯದಿ(!gethostname(hostname, 256)){
				len = strlen(hostname) + strlen(bridge1->name) + 2;
				bridge1->remote_clientid = mosquitto_malloc(len);
				ಯದಿ(!bridge1->remote_clientid){
					ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
				}
				snprintf(bridge1->remote_clientid, len, "%s.%s", hostname, bridge1->name);
			}ಅನ್ಯಥಾ{
				ಪ್ರತ್ಯಾಗಮ 1;
			}
		}

		ಯದಿ(!bridge1->local_clientid){
			len = strlen(bridge1->remote_clientid) + strlen("local.") + 2;
			bridge1->local_clientid = mosquitto_malloc(len);
			ಯದಿ(!bridge1->local_clientid){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
			}
			snprintf(bridge1->local_clientid, len, "local.%s", bridge1->remote_clientid);
		}
	}

	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<config->bridge_ಎಣಿಕೆ; i++){
		bridge1 = config->bridges[i];
		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ j=i+1; j<config->bridge_ಎಣಿಕೆ; j++){
			bridge2 = config->bridges[j];
			ಯದಿ(!strcmp(bridge1->local_clientid, bridge2->local_clientid)){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Bridge local_clientid "
						"'%s' is not unique. Try changing or setting the "
						"local_clientid value for one of the bridges.",
						bridge1->local_clientid);
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
			}
		}
	}

#ifdef WITH_TLS
	/* Check for missing TLS cafile/capath/certfile/keyfile */
	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<config->listener_ಎಣಿಕೆ; i++){
		bool cafile = !!config->listeners[i].cafile;
		bool capath = !!config->listeners[i].capath;
		bool certfile = !!config->listeners[i].certfile;
		bool keyfile = !!config->listeners[i].keyfile;

		ಯದಿ((certfile && !keyfile) || (!certfile && keyfile)){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Both certfile and keyfile must be provided to enable a TLS listener.");
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
		ಯದಿ(cafile && !certfile){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: cafile specified without certfile and keyfile.");
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
		ಯದಿ(capath && !certfile){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: capath specified without certfile and keyfile.");
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
	}
#endif
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}
#endif


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ conf__parse_bool(ವರ್ಣ **token, ಧ್ರುವ ವರ್ಣ *name, bool *ಮೌಲ್ಯ, ವರ್ಣ **saveptr)
{
	*token = strtok_r(ರಿಕ್ತ, " ", saveptr);
	ಯದಿ(*token){
		ಯದಿ(!strcmp(*token, "false") || !strcmp(*token, "0")){
			*ಮೌಲ್ಯ = false;
		}ಅನ್ಯಥಾ ಯದಿ(!strcmp(*token, "true") || !strcmp(*token, "1")){
			*ಮೌಲ್ಯ = true;
		}ಅನ್ಯಥಾ{
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Invalid '%s' value (%s).", name, *token);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
	}ಅನ್ಯಥಾ{
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ conf__parse_int(ವರ್ಣ **token, ಧ್ರುವ ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ *ಮೌಲ್ಯ, ವರ್ಣ **saveptr)
{
	*token = strtok_r(ರಿಕ್ತ, " ", saveptr);
	ಯದಿ(*token){
		ವರ್ಣ *endptr = ರಿಕ್ತ;
		long l = strtol(*token, &endptr, 0);
		ಯದಿ(endptr == *token || endptr[0] != '\0'){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: '%s' value not a number.", name);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
		ಯದಿ(l > INT_MAX || l < INT_MIN){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: '%s' value out of range.", name);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
		*ಮೌಲ್ಯ = (ಪೂರ್ಣಾಙ್ಕ)l;
	}ಅನ್ಯಥಾ{
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ conf__parse_ssize_t(ವರ್ಣ **token, ಧ್ರುವ ವರ್ಣ *name, ssize_t *ಮೌಲ್ಯ, ವರ್ಣ **saveptr)
{
	*token = strtok_r(ರಿಕ್ತ, " ", saveptr);
	ಯದಿ(*token){
		*ಮೌಲ್ಯ = atol(*token);
	}ಅನ್ಯಥಾ{
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ conf__parse_string(ವರ್ಣ **token, ಧ್ರುವ ವರ್ಣ *name, ವರ್ಣ **ಮೌಲ್ಯ, ವರ್ಣ **saveptr)
{
	size_t tlen;

	*token = strtok_r(ರಿಕ್ತ, "", saveptr);
	ಯದಿ(*token){
		ಯದಿ(*ಮೌಲ್ಯ){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Duplicate '%s' value in configuration.", name);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
		/* Deal with multiple spaces at the beginning of the string. */
		*token = mosquitto_trimblanks(*token);
		ಯದಿ(strlen(*token) == 0){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}

		tlen = strlen(*token);
		ಯದಿ(tlen > UINT16_MAX){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
		ಯದಿ(mosquitto_validate_utf8(*token, (uint16_t)tlen)){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Malformed UTF-8 in configuration.");
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
		*ಮೌಲ್ಯ = mosquitto_strdup(*token);
		ಯದಿ(!*ಮೌಲ್ಯ){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}
	}ಅನ್ಯಥಾ{
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}
