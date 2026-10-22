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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
#ಇಲ್ಲದಿದ್ದರೆ
#  include <dirent.h>
#  include <strings.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
#  include <netdb.h>
#  include <sys/socket.h>
#ಇಲ್ಲದಿದ್ದರೆ
#  include <winsock2.h>
#  include <ws2tcpip.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(WIN32) && !defined(__CYGWIN__)
#  include <syslog.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "tls_mosq.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"

#ಅನ್ತರ್ಭಾವ "utlist.h"

ರಚನೆ config_recurse {
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ log_dest;
	ಪೂರ್ಣಾಂಕ log_dest_ಗಣ;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ log_type;
	ಪೂರ್ಣಾಂಕ log_type_ಗಣ;
};

#ಆದರೆ defined(WIN32) || defined(__CYGWIN__)
#ಅನ್ತರ್ಭಾವ <windows.h>
ಬಾಹ್ಯ SERVICE_ಸ್ಥಿತಿ_HANDLE service_handle;
#ಆದರೆ_ಅಂತ್ಯ


#ವ್ಯಾಖ್ಯಾನಿಸು REQUIRE_LISTENER(A) \
		ಮಾಡು{ \
			ಆದರೆ(cur_listener == ಖಾಲಿ){ \
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL; \
			} \
		}ಆಗ(0)

#ವ್ಯಾಖ್ಯಾನಿಸು REQUIRE_LISTENER_OR_DEFAULT_LISTENER(A) \
		ಮಾಡು{ \
			ಆದರೆ(cur_listener == ಖಾಲಿ){ \
				ಆದರೆ(config__create_default_listener(config, (A))){ \
					ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM; \
				} \
				cur_listener = config->default_listener; \
			} \
		}ಆಗ(0)

#ವ್ಯಾಖ್ಯಾನಿಸು REQUIRE_LISTENER_IF_ಪ್ರತಿ_LISTENER(A) \
		ಮಾಡು{ \
			ಆದರೆ(config->ಪ್ರತಿ_listener_settings == true && cur_listener == ಖಾಲಿ){ \
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL; \
			} \
		}ಆಗ(0)

#ವ್ಯಾಖ್ಯಾನಿಸು REQUIRE_NON_DEFAULT_LISTENER(A) \
		ಮಾಡು{ \
			ಆದರೆ(cur_listener == config->default_listener || cur_listener == ಖಾಲಿ){ \
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL; \
			} \
		}ಆಗ(0)

#ವ್ಯಾಖ್ಯಾನಿಸು REQUIRE_BRIDGE(A) \
		ಮಾಡು{ \
			ಆದರೆ(cur_bridge == ಖಾಲಿ){ \
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: The '%s' option requires a bridge to be defined first.", (A)); \
				ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL; \
			} \
		}ಆಗ(0)

#ವ್ಯಾಖ್ಯಾನಿಸು REQUIRE_PLUGIN(A) \
		ಮಾಡು{ \
			ಆದರೆ(cur_plugin == ಖಾಲಿ){ \
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: The '%s' option requires plugin/global_plugin/plugin_load to be defined first.", (A)); \
				ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL; \
			} \
		}ಆಗ(0)

#ವ್ಯಾಖ್ಯಾನಿಸು OPTION_DEPRECATED(A, B) \
		log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "The '%s' option is now deprecated and will be removed in version 3.0. %s", (A), (B))

#ವ್ಯಾಖ್ಯಾನಿಸು OPTION_UNAVAILABLE(A) \
		log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: The '%s' option is no longer available.", (A));

#ವ್ಯಾಖ್ಯಾನಿಸು REQUIRE_NON_ಖಾಲಿಗಳು_OPTION(A, B) \
		ಮಾಡು{ \
			ಆದರೆ(!(A)){ \
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", (B)); \
				ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL; \
			} \
		}ಆಗ(0)

#ವ್ಯಾಖ್ಯಾನಿಸು ಪ್ರತಿ_LISTENER_ALTERNATIVE(A, B) \
		ಆದರೆ(config->ಪ್ರತಿ_listener_settings){ \
			log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "You are using the '%s' option with 'per_listener_settings true'. Please replace this with '%s'.", A, B); \
		} \


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FINAL_WITH_TLS_PSK
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A) \
		ಮಾಡು{ \
			ಆದರೆ(cur_bridge->tls_psk_ತಟಸ್ಥ || cur_bridge->tls_psk){ \
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and psk encryption in a single bridge.", (A)); \
				ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL; \
			} \
		}ಆಗ(0)

#  define REQUIRE_BRIDGE_NO_X509(A) \
		ಮಾಡು{ \
			ಆದರೆ(cur_bridge->tls_cafile || cur_bridge->tls_capath || cur_bridge->tls_certfile || cur_bridge->tls_keyfile){ \
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and identity encryption in a single bridge.", (A)); \
				ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL; \
			} \
		}ಆಗ(0)

#ಇಲ್ಲದಿದ್ದರೆ
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A)
#  define REQUIRE_BRIDGE_NO_X509(A)
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ರಚನೆ mosquitto__security_options *cur_security_options = ಖಾಲಿ;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ conf__parse_bool(ಅಕ್ಷರ **token, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, bool *ಮೌಲ್ಯ, ಅಕ್ಷರ **saveptr);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ conf__parse_int(ಅಕ್ಷರ **token, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ *ಮೌಲ್ಯ, ಅಕ್ಷರ **saveptr);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ conf__parse_ssize_t(ಅಕ್ಷರ **token, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ssize_t *ಮೌಲ್ಯ, ಅಕ್ಷರ **saveptr);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ conf__parse_string(ಅಕ್ಷರ **token, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಅಕ್ಷರ **ಮೌಲ್ಯ, ಅಕ್ಷರ **saveptr);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ config__read_file(ರಚನೆ mosquitto__config *config, bool reload, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file, ರಚನೆ config_recurse *config_tmp, ಪೂರ್ಣಾಂಕ level, ಪೂರ್ಣಾಂಕ *lineno);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ config__check(ರಚನೆ mosquitto__config *config);
ಸ್ಥಿರ ಶೂನ್ಯ config__cleanup_plugins(ಶೂನ್ಯ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ config__check_bridges(ರಚನೆ mosquitto__config *config);
#ಆದರೆ_ಅಂತ್ಯ


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ config_ಕೂಡಿಸು_listener(ರಚನೆ mosquitto__config *config)
{
	ರಚನೆ mosquitto__listener *listener;
	ರಚನೆ mosquitto__listener *new_listeners;
	ಪೂರ್ಣಾಂಕ def_listener = -1;

	ಆದರೆ(config->default_listener){
		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<config->listener_ಎಣಿಕೆ; i++){
			ಆದರೆ(&config->listeners[i] == config->default_listener){
				def_listener = i;
				ನಿಲ್ಲಿಸು;
			}
		}
	}
	new_listeners = mosquitto_realloc(config->listeners, ಗಾತ್ರ(ರಚನೆ mosquitto__listener)*(size_t)(config->listener_ಎಣಿಕೆ+1));
	ಆದರೆ(!new_listeners){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}
	config->listeners = new_listeners;
	listener = &config->listeners[config->listener_ಎಣಿಕೆ];
	memset(listener, 0, ಗಾತ್ರ(ರಚನೆ mosquitto__listener));
	listener->security_options = mosquitto_calloc(1, ಗಾತ್ರ(ರಚನೆ mosquitto__security_options));
	ಆದರೆ(!listener->security_options){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}
	ಆದರೆ(def_listener != -1){
		config->default_listener = &config->listeners[def_listener];
	}
	config->listener_ಎಣಿಕೆ++;

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ config__create_default_listener(ರಚನೆ mosquitto__config *config, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *option_name)
{
	ಆದರೆ(config->default_listener){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}
	log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Creating default listener due to '%s' option.", option_name);
	log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "It is best practice to define a 'listener' first. Using the '%s' option without a listener will be disabled in the future.", option_name);
	ಆದರೆ(config_ಕೂಡಿಸು_listener(config)){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}

	config->default_listener = &config->listeners[config->listener_ಎಣಿಕೆ-1];
	listener_ಗಣ_defaults(config->default_listener);
	config->default_listener->ಪೋರ್ಟ್ = 1883;

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಶೂನ್ಯ conf_ಗಣ_cur_security_options(ರಚನೆ mosquitto__config *config, ರಚನೆ mosquitto__listener **cur_listener, ರಚನೆ mosquitto__security_options **security_options, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *option_name)
{
	ಆದರೆ(config->ಪ್ರತಿ_listener_settings){
		ಆದರೆ(*cur_listener == ಖಾಲಿ){
			ಆದರೆ(config__create_default_listener(config, option_name)){
				ಹಿಂತಿರುಗಿ;
			}
			*cur_listener = config->default_listener;
		}
		(*security_options) = (*cur_listener)->security_options;
	}ಇಲ್ಲದಿದ್ದರೆ{
		(*security_options) = &config->security_options;
	}
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ conf__attempt_resolve(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಲಘುಗಣಕ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ)
{
	ರಚನೆ addrinfo gai_hints;
	ರಚನೆ addrinfo *gai_res;
	ಪೂರ್ಣಾಂಕ rc;

	memset(&gai_hints, 0, ಗಾತ್ರ(ರಚನೆ addrinfo));
	gai_hints.ai_family = AF_UNSPEC;
	gai_hints.ai_socktype = SOCK_STREAM;
	gai_res = ಖಾಲಿ;
	rc = getaddrinfo(host, ಖಾಲಿ, &gai_hints, &gai_res);
	ಆದರೆ(gai_res){
		freeaddrinfo(gai_res);
	}
	ಆದರೆ(rc != 0){
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
		ಆದರೆ(rc == EAI_SYSTEM){
			ಆದರೆ(errno == ENOENT){
				log__printf(ಖಾಲಿ, ಲಘುಗಣಕ, "%s: Unable to resolve %s %s.", ಸಂದೇಶ, ಪಠ್ಯ, host);
			}ಇಲ್ಲದಿದ್ದರೆ{
				log__printf(ಖಾಲಿ, ಲಘುಗಣಕ, "%s: Error resolving %s: %s.", ಸಂದೇಶ, ಪಠ್ಯ, strerror(errno));
			}
		}ಇಲ್ಲದಿದ್ದರೆ{
			log__printf(ಖಾಲಿ, ಲಘುಗಣಕ, "%s: Error resolving %s: %s.", ಸಂದೇಶ, ಪಠ್ಯ, gai_strerror(rc));
		}
#ಇಲ್ಲದಿದ್ದರೆ
		ಆದರೆ(rc == WSAHOST_NOT_ಸಿಕ್ಕಿತು){
			log__printf(ಖಾಲಿ, ಲಘುಗಣಕ, "%s: Error resolving %s.", ಸಂದೇಶ, ಪಠ್ಯ);
		}
#ಆದರೆ_ಅಂತ್ಯ
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಶೂನ್ಯ config__init_reload(ರಚನೆ mosquitto__config *config)
{
	/* Set defaults */
	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<config->listener_ಎಣಿಕೆ; i++){
		listener_ಗಣ_defaults(&config->listeners[i]);
	}

	config->local_only = true;
	config->allow_duplicate_messages = true;

	mosquitto_FREE(config->security_options.acl_ದತ್ತಾಂಶ.acl_file);
	mosquitto_FREE(config->security_options.password_ದತ್ತಾಂಶ.password_file);
	mosquitto_FREE(config->security_options.psk_file);

	config->security_options.allow_anonymous = -1;
	config->security_options.allow_zero_ಉದ್ದಳತೆ_clientid = true;
	config->security_options.auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ = ಖಾಲಿ;
	config->security_options.auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ_len = 0;

	config->autosave_interval = 1800;
	config->autosave_on_changes = false;

	mosquitto_FREE(config->clientid_prefixes);

	config->connection_messages = true;
	config->clientid_prefixes = ಖಾಲಿ;
	config->ಪ್ರತಿ_listener_settings = false;
	ಆದರೆ(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = ಖಾಲಿ;
	}
	mosquitto_FREE(config->log_file);

#ಆದರೆ defined(WIN32) || defined(__CYGWIN__)
	ಆದರೆ(service_handle){
		/* This is running as a Windows service. Default to no logging. Using
		 * stdout/stderr is forbidden because the first clients to connect will
		 * get log information sent to them for some reason. */
		config->log_dest = MQTT3_LOG_NONE;
	}ಇಲ್ಲದಿದ್ದರೆ{
		config->log_dest = MQTT3_LOG_STDERR;
	}
#ಇಲ್ಲದಿದ್ದರೆ
	config->log_facility = LOG_DAEMON;
	config->log_dest = MQTT3_LOG_STDERR | MQTT3_LOG_DLT;
	ಆದರೆ(db.quiet){
		config->log_type = 0;
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(db.verbose){
		config->log_type = UINT_MAX;
	}ಇಲ್ಲದಿದ್ದರೆ{
		config->log_type = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;
	}
#ಆದರೆ_ಅಂತ್ಯ
	config->log_timestamp = true;
	mosquitto_FREE(config->log_timestamp_format);
	config->global_max_clients = -1;
	config->global_max_connections = -1;
	config->log_timestamp_format = ಖಾಲಿ;
	config->max_keepalive = 0;
	config->max_packet_ಗಾತ್ರ = 2000000;
	config->max_inflight_messages = 20;
	config->max_queued_messages = 1000;
	config->max_inflight_ಬೈಟುಗಳು = 0;
	config->max_queued_ಬೈಟುಗಳು = 0;
	config->persistence = false;
	mosquitto_FREE(config->persistence_location);
	mosquitto_FREE(config->persistence_file);
	config->persistent_client_expiration = 0;
	config->ಸರತಿ_qos0_messages = false;
	config->retain_ಲಭ್ಯ = true;
	config->retain_expiry_interval = 0;
	config->ಗಣ_tcp_nodelay = false;
	config->sys_interval = 10;
	config->upgrade_outgoing_qos = false;
	config->packet_ಬಫರ್_ಗಾತ್ರ = 4096;

	config->packet_max_auth = 100000;
	config->packet_max_connect = 100000;
	config->packet_max_sub = 100000;
	config->packet_max_simple = 10000;
}


ಸ್ಥಿರ ಶೂನ್ಯ config__cleanup_plugin_config(mosquitto_plugin_id_t *plugin)
{
	mosquitto_FREE(plugin->config.ಮಾರ್ಗ);
	mosquitto_FREE(plugin->config.name);

	ಆದರೆ(plugin->config.options){
		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ j=0; j<plugin->config.option_ಎಣಿಕೆ; j++){
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
	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<db.plugin_ಎಣಿಕೆ; i++){
		config__cleanup_plugin_config(db.plugins[i]);
	}
	mosquitto_FREE(db.plugins);
}


ಶೂನ್ಯ config__init(ರಚನೆ mosquitto__config *config)
{
	memset(config, 0, ಗಾತ್ರ(ರಚನೆ mosquitto__config));
	config__init_reload(config);

	config->daemon = false;
}


ಶೂನ್ಯ config__cleanup(ರಚನೆ mosquitto__config *config)
{
	mosquitto_FREE(config->clientid_prefixes);
	mosquitto_FREE(config->persistence_location);
	mosquitto_FREE(config->persistence_file);
	mosquitto_FREE(config->persistence_filepath);
	mosquitto_FREE(config->security_options.auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ);
	mosquitto_FREE(config->security_options.acl_ದತ್ತಾಂಶ.acl_file);
	mosquitto_FREE(config->security_options.password_ದತ್ತಾಂಶ.password_file);
	mosquitto_FREE(config->security_options.psk_file);
	mosquitto_FREE(config->security_options.plugins);
	mosquitto_FREE(config->pid_file);
	mosquitto_FREE(config->user);
	mosquitto_FREE(config->log_timestamp_format);
	ಆದರೆ(config->listeners){
		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<config->listener_ಎಣಿಕೆ; i++){
			mosquitto_FREE(config->listeners[i].host);
			mosquitto_FREE(config->listeners[i].bind_interface);
			mosquitto_FREE(config->listeners[i].mount_ಬಿಂದು);
			mosquitto_FREE(config->listeners[i].socks);
			ಆದರೆ(config->listeners[i].security_options){
				mosquitto_FREE(config->listeners[i].security_options->auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ);
				mosquitto_FREE(config->listeners[i].security_options->acl_ದತ್ತಾಂಶ.acl_file);
				mosquitto_FREE(config->listeners[i].security_options->password_ದತ್ತಾಂಶ.password_file);
				mosquitto_FREE(config->listeners[i].security_options->psk_file);
				mosquitto_FREE(config->listeners[i].security_options->plugins);
				mosquitto_FREE(config->listeners[i].security_options);
			}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
			mosquitto_FREE(config->listeners[i].cafile);
			mosquitto_FREE(config->listeners[i].capath);
			mosquitto_FREE(config->listeners[i].certfile);
			mosquitto_FREE(config->listeners[i].keyfile);
			mosquitto_FREE(config->listeners[i].ciphers);
			mosquitto_FREE(config->listeners[i].ciphers_tls13);
			mosquitto_FREE(config->listeners[i].psk_hint);
			mosquitto_FREE(config->listeners[i].crlfile);
			mosquitto_FREE(config->listeners[i].tls_ಆವೃತ್ತಿ);
			mosquitto_FREE(config->listeners[i].tls_engine);
			mosquitto_FREE(config->listeners[i].tls_engine_kpass_sha1);
#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			ಆದರೆ(!config->listeners[i].ws_context) /* libwebsockets frees its own SSL_CTX */
#ಆದರೆ_ಅಂತ್ಯ
			{
				SSL_CTX_free(config->listeners[i].ssl_ctx);
				config->listeners[i].ssl_ctx = ಖಾಲಿ;
			}
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
			mosquitto_FREE(config->listeners[i].http_dir);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_WEBSOCKETS
			ಗಾಗಿ(ಪೂರ್ಣಾಂಕ j=0; j<config->listeners[i].ws_origin_ಎಣಿಕೆ; j++){
				mosquitto_FREE(config->listeners[i].ws_origins[j]);
			}
			mosquitto_FREE(config->listeners[i].ws_origins);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_UNIX_SOCKETS
			mosquitto_FREE(config->listeners[i].unix_socket_ಮಾರ್ಗ);
#ಆದರೆ_ಅಂತ್ಯ
		}
		mosquitto_FREE(config->listeners);
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
	ಆದರೆ(config->bridges){
		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<config->bridge_ಎಣಿಕೆ; i++){
			config__bridge_cleanup(config->bridges[i]);
		}
		mosquitto_FREE(config->bridges);
	}
#ಆದರೆ_ಅಂತ್ಯ
	config__cleanup_plugins();

	ಆದರೆ(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = ಖಾಲಿ;
	}
	ಆದರೆ(config->log_file){
		mosquitto_FREE(config->log_file);
		config->log_file = ಖಾಲಿ;
	}
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE


ಶೂನ್ಯ config__bridge_cleanup(ರಚನೆ mosquitto__bridge *bridge)
{
	ಆದರೆ(bridge == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ;
	}

	mosquitto_FREE(bridge->name);
	ಆದರೆ(bridge->ವಿಳಾಸಗಳು){
		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<bridge->ವಿಳಾಸ_ಎಣಿಕೆ; i++){
			mosquitto_FREE(bridge->ವಿಳಾಸಗಳು[i].ವಿಳಾಸ);
		}
		mosquitto_FREE(bridge->ವಿಳಾಸಗಳು);
	}
	mosquitto_FREE(bridge->bind_ವಿಳಾಸ);
	mosquitto_FREE(bridge->remote_clientid);
	mosquitto_FREE(bridge->remote_username);
	mosquitto_FREE(bridge->remote_password);
	mosquitto_FREE(bridge->local_clientid);
	mosquitto_FREE(bridge->local_username);
	mosquitto_FREE(bridge->local_password);
	ಆದರೆ(bridge->topics){
		ರಚನೆ mosquitto__bridge_topic *cur_topic, *topic_tmp;

		LL_FOREACH_ಸುರಕ್ಷಿತ(bridge->topics, cur_topic, topic_tmp){
			mosquitto_FREE(cur_topic->topic);
			mosquitto_FREE(cur_topic->local_ಪೂರ್ವಪ್ರತ್ಯಯ);
			mosquitto_FREE(cur_topic->remote_ಪೂರ್ವಪ್ರತ್ಯಯ);
			mosquitto_FREE(cur_topic->local_topic);
			mosquitto_FREE(cur_topic->remote_topic);
			LL_DELETE(bridge->topics, cur_topic);
			mosquitto_FREE(cur_topic);
		}
		mosquitto_FREE(bridge->topics);
	}
	mosquitto_FREE(bridge->notification_topic);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
	mosquitto_FREE(bridge->tls_certfile);
	mosquitto_FREE(bridge->tls_keyfile);
	mosquitto_FREE(bridge->tls_ಆವೃತ್ತಿ);
	mosquitto_FREE(bridge->tls_cafile);
	mosquitto_FREE(bridge->tls_capath);
	mosquitto_FREE(bridge->tls_alpn);
	mosquitto_FREE(bridge->tls_ciphers);
	mosquitto_FREE(bridge->tls_13_ciphers);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FINAL_WITH_TLS_PSK
	mosquitto_FREE(bridge->tls_psk_ತಟಸ್ಥ);
	mosquitto_FREE(bridge->tls_psk);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ
	mosquitto_FREE(bridge);
}
#ಆದರೆ_ಅಂತ್ಯ


ಸ್ಥಿರ ಶೂನ್ಯ print_ಆವೃತ್ತಿ(ಶೂನ್ಯ)
{
	ಬರೆ("mosquitto %s\n", ಆವೃತ್ತಿ);
	ಬರೆ("Copyright © 2025 Roger Light.\n");
	ಬರೆ("License EPL-2.0 OR BSD-3-Clause.\n");
}


ಸ್ಥಿರ ಶೂನ್ಯ print_usage(ಶೂನ್ಯ)
{
	ಬರೆ("mosquitto version %s\n\n", ಆವೃತ್ತಿ);
	ಬರೆ("mosquitto is an MQTT v5.0/v3.1.1/v3.1 broker.\n\n");
	ಬರೆ("Usage: mosquitto [-c config_file] [-d] [-h] [-p port] [-v]\n");
	ಬರೆ("                 [--tls-keylog file]\n\n");
	ಬರೆ(" -c : specify the broker config file.\n");
	ಬರೆ(" -d : put the broker into the background after starting.\n");
	ಬರೆ(" -h : display this help.\n");
	ಬರೆ(" -p : start the broker listening on the specified port.\n");
	ಬರೆ("      Not recommended in conjunction with the -c option.\n");
	ಬರೆ(" -q : quiet mode - disable all logging types. This overrides\n");
	ಬರೆ("      any logging options given in the config file, and -v.\n");
	ಬರೆ(" -v : verbose mode - enable all logging types. This overrides\n");
	ಬರೆ("      any logging options given in the config file.\n");
	ಬರೆ(" --test-config : test config file and exit\n");
	ಬರೆ(" --tls-keylog : log TLS connection information to a file, to allow\n");
	ಬರೆ("      debugging with e.g. wireshark. Do not use on a production\n");
	ಬರೆ("      server.\n");
	ಬರೆ("\nSee https://mosquitto.org/ for more information.\n\n");
}


ಪೂರ್ಣಾಂಕ config_parse_ವಾದಗಳು(ರಚನೆ mosquitto__config *config, ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ *argv[])
{
	ಪೂರ್ಣಾಂಕ i;
	ಪೂರ್ಣಾಂಕ ಪೋರ್ಟ್_tmp;

	ಗಾಗಿ(i=1; i<argc; i++){
		ಆದರೆ(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--config-file")){
			ಆದರೆ(i<argc-1){
				db.config_file = argv[i+1];

				ಆದರೆ(config__read(config, false)){
					ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
				}
			}ಇಲ್ಲದಿದ್ದರೆ{
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: -c argument given, but no config file specified.");
				ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
			}
			i++;
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--daemon")){
			config->daemon = true;
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
			print_usage();
			ಹಿಂತಿರುಗಿ MOSQ_ERR_UNKNOWN;
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(argv[i], "--version")){
			print_ಆವೃತ್ತಿ();
			ಹಿಂತಿರುಗಿ MOSQ_ERR_UNKNOWN;
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port")){
			ಆದರೆ(i<argc-1){
				ಪೋರ್ಟ್_tmp = atoi(argv[i+1]);
				ಆದರೆ(ಪೋರ್ಟ್_tmp<1 || ಪೋರ್ಟ್_tmp>UINT16_MAX){
					log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid port specified (%d).", ಪೋರ್ಟ್_tmp);
					ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
				}ಇಲ್ಲದಿದ್ದರೆ{
					ಆದರೆ(config->cmd_ಪೋರ್ಟ್_ಎಣಿಕೆ == CMD_ಪೋರ್ಟ್_ಎಲ್ಲೆ){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Only %d ports can be specified on the command line.", CMD_ಪೋರ್ಟ್_ಎಲ್ಲೆ);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					config->cmd_ಪೋರ್ಟ್[config->cmd_ಪೋರ್ಟ್_ಎಣಿಕೆ] = (uint16_t)ಪೋರ್ಟ್_tmp;
					config->cmd_ಪೋರ್ಟ್_ಎಣಿಕೆ++;
				}
			}ಇಲ್ಲದಿದ್ದರೆ{
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: -p argument given, but no port specified.");
				ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
			}
			i++;
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(argv[i], "--tls-keylog")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
			ಆದರೆ(i<argc-1){
				db.tls_keylog = mosquitto_strdup(argv[i+1]);
				ಆದರೆ(db.tls_keylog == ಖಾಲಿ){
					log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
					ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
				}
			}ಇಲ್ಲದಿದ್ದರೆ{
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: --tls-keylog argument given, but no file specified.");
				ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
			}
			i++;
#ಇಲ್ಲದಿದ್ದರೆ
			fprintf(stderr, "Error: TLS support not available so --tls-keylog is not available.\n");
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
#ಆದರೆ_ಅಂತ್ಯ
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(argv[i], "-q") || !strcmp(argv[i], "--quiet")){
			db.quiet = true;
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")){
			db.verbose = true;
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(argv[i], "--test-config")){
			config->test_configuration = true;
		}ಇಲ್ಲದಿದ್ದರೆ{
			fprintf(stderr, "Error: Unknown option '%s'.\n", argv[i]);
			print_usage();
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
	}

	/* Default to drop to mosquitto user if we are privileged and no user specified. */
	ಆದರೆ(!config->user){
		config->user = mosquitto_strdup("mosquitto");
		ಆದರೆ(config->user == ಖಾಲಿ){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}
	}
	ಆದರೆ(db.quiet){
		config->log_type = 0;
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(db.verbose){
		config->log_type = UINT_MAX;
	}

	ಆದರೆ(getenv("MOSQUITTO_PERSISTENCE_LOCATION")){
		mosquitto_FREE(config->persistence_location);
		config->persistence_location = mosquitto_strdup(getenv("MOSQUITTO_PERSISTENCE_LOCATION"));
		ಆದರೆ(!config->persistence_location){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}
	}
	ಹಿಂತಿರುಗಿ config__check(config);
}


ಸ್ಥಿರ ಶೂನ್ಯ config__copy(ರಚನೆ mosquitto__config *src, ರಚನೆ mosquitto__config *dest)
{
	mosquitto_FREE(dest->security_options.acl_ದತ್ತಾಂಶ.acl_file);
	dest->security_options.acl_ದತ್ತಾಂಶ.acl_file = src->security_options.acl_ದತ್ತಾಂಶ.acl_file;

	acl_file__cleanup(&dest->security_options.acl_ದತ್ತಾಂಶ);
	dest->security_options.acl_ದತ್ತಾಂಶ.acl_users = src->security_options.acl_ದತ್ತಾಂಶ.acl_users;
	dest->security_options.acl_ದತ್ತಾಂಶ.acl_patterns = src->security_options.acl_ದತ್ತಾಂಶ.acl_patterns;
	dest->security_options.acl_ದತ್ತಾಂಶ.acl_anon.username = src->security_options.acl_ದತ್ತಾಂಶ.acl_anon.username;
	dest->security_options.acl_ದತ್ತಾಂಶ.acl_anon.acl = src->security_options.acl_ದತ್ತಾಂಶ.acl_anon.acl;

	dest->security_options.allow_anonymous = src->security_options.allow_anonymous;
	dest->security_options.allow_zero_ಉದ್ದಳತೆ_clientid = src->security_options.allow_zero_ಉದ್ದಳತೆ_clientid;

	mosquitto_FREE(dest->security_options.auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ);
	dest->security_options.auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ = src->security_options.auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ;
	dest->security_options.auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ_len = src->security_options.auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ_len;

	mosquitto_FREE(dest->security_options.password_ದತ್ತಾಂಶ.password_file);
	dest->security_options.password_ದತ್ತಾಂಶ.password_file = src->security_options.password_ದತ್ತಾಂಶ.password_file;

	password_file__cleanup(&dest->security_options.password_ದತ್ತಾಂಶ);
	dest->security_options.password_ದತ್ತಾಂಶ.unpwd = src->security_options.password_ದತ್ತಾಂಶ.unpwd;

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

	dest->ಸಂದೇಶ_ಗಾತ್ರ_ಎಲ್ಲೆ = src->ಸಂದೇಶ_ಗಾತ್ರ_ಎಲ್ಲೆ;

	dest->persistence = src->persistence;

	mosquitto_FREE(dest->persistence_location);
	dest->persistence_location = src->persistence_location;

	mosquitto_FREE(dest->persistence_file);
	dest->persistence_file = src->persistence_file;

	mosquitto_FREE(dest->persistence_filepath);
	dest->persistence_filepath = src->persistence_filepath;

	dest->persistent_client_expiration = src->persistent_client_expiration;


	dest->ಸರತಿ_qos0_messages = src->ಸರತಿ_qos0_messages;
	dest->sys_interval = src->sys_interval;
	dest->upgrade_outgoing_qos = src->upgrade_outgoing_qos;

#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	dest->websockets_log_level = src->websockets_log_level;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<dest->bridge_ಎಣಿಕೆ; i++){
		ಆದರೆ(dest->bridges[i]){
			config__bridge_cleanup(dest->bridges[i]);
		}
	}
	mosquitto_FREE(dest->bridges);
	dest->bridges = src->bridges;
	dest->bridge_ಎಣಿಕೆ = src->bridge_ಎಣಿಕೆ;
#ಆದರೆ_ಅಂತ್ಯ
}


ಪೂರ್ಣಾಂಕ config__read(ರಚನೆ mosquitto__config *config, bool reload)
{
	ಪೂರ್ಣಾಂಕ rc = MOSQ_ERR_SUCCESS;
	ರಚನೆ config_recurse cr;
	ಪೂರ್ಣಾಂಕ lineno = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_PERSISTENCE
	size_t len;
#ಆದರೆ_ಅಂತ್ಯ
	ರಚನೆ mosquitto__config config_reload;
	ಪೂರ್ಣಾಂಕ i;

	ಆದರೆ(reload){
		memset(&config_reload, 0, ಗಾತ್ರ(ರಚನೆ mosquitto__config));
	}

	cr.log_dest = MQTT3_LOG_NONE;
	cr.log_dest_ಗಣ = 0;
	cr.log_type = MOSQ_LOG_NONE;
	cr.log_type_ಗಣ = 0;

	ಆದರೆ(!db.config_file){
		ಹಿಂತಿರುಗಿ 0;
	}

	ಆದರೆ(reload){
		/* Re-initialise appropriate config vars to default for reload. */
		config__init_reload(&config_reload);
		config_reload.listeners = config->listeners;
		config_reload.listener_ಎಣಿಕೆ = config->listener_ಎಣಿಕೆ;
		cur_security_options = ಖಾಲಿ;
		rc = config__read_file(&config_reload, reload, db.config_file, &cr, 0, &lineno);
	}ಇಲ್ಲದಿದ್ದರೆ{
		rc = config__read_file(config, reload, db.config_file, &cr, 0, &lineno);
	}
	ಆದರೆ(rc){
		ಆದರೆ(lineno > 0){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error found at %s:%d.", db.config_file, lineno);
		}
		ಹಿಂತಿರುಗಿ rc;
	}

	ಆದರೆ(reload){
		config__copy(&config_reload, config);
	}

	/* If auth/access options are set and allow_anonymous not explicitly set, disallow anon. */
	ಆದರೆ(config->local_only == false){
		ಆದರೆ(config->ಪ್ರತಿ_listener_settings){
			ಗಾಗಿ(i=0; i<config->listener_ಎಣಿಕೆ; i++){
				/* Default option if no security options set */
				ಆದರೆ(config->listeners[i].security_options->allow_anonymous == -1){
					config->listeners[i].security_options->allow_anonymous = false;
				}
			}
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಆದರೆ(config->security_options.allow_anonymous == -1){
				config->security_options.allow_anonymous = false;
			}
		}
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_PERSISTENCE
	ಆದರೆ(config->persistence){
		ಆದರೆ(!config->persistence_file){
			config->persistence_file = mosquitto_strdup("mosquitto.db");
			ಆದರೆ(!config->persistence_file){
				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
			}
		}
		mosquitto_FREE(config->persistence_filepath);
		ಆದರೆ(config->persistence_location && ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(config->persistence_location)){
			len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(config->persistence_location) + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(config->persistence_file) + 2;
			config->persistence_filepath = mosquitto_malloc(len);
			ಆದರೆ(!config->persistence_filepath){
				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
			}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
			snprintf(config->persistence_filepath, len, "%s\\%s", config->persistence_location, config->persistence_file);
#ಇಲ್ಲದಿದ್ದರೆ
			snprintf(config->persistence_filepath, len, "%s/%s", config->persistence_location, config->persistence_file);
#ಆದರೆ_ಅಂತ್ಯ
		}ಇಲ್ಲದಿದ್ದರೆ{
			config->persistence_filepath = mosquitto_strdup(config->persistence_file);
			ಆದರೆ(!config->persistence_filepath){
				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
			}
		}
	}
#ಆದರೆ_ಅಂತ್ಯ
	/* Default to drop to mosquitto user if no other user specified. This must
	 * remain here even though it is covered in config__parse_args() because this
	 * function may be called on its own. */
	ಆದರೆ(!config->user){
		config->user = mosquitto_strdup("mosquitto");
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
	ಗಾಗಿ(i=0; i<config->bridge_ಎಣಿಕೆ; i++){
		ಆದರೆ(!config->bridges[i]->name){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: bridge name not defined.");
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
		ಆದರೆ(config->bridges[i]->ವಿಳಾಸಗಳು  == 0){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no remote addresses defined.");
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
		ಆದರೆ(config->bridges[i]->topic_ಎಣಿಕೆ == 0){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no topics defined.");
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FINAL_WITH_TLS_PSK
		ಆದರೆ(config->bridges[i]->tls_psk && !config->bridges[i]->tls_psk_ತಟಸ್ಥ){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_identity.");
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
		ಆದರೆ(config->bridges[i]->tls_psk_ತಟಸ್ಥ && !config->bridges[i]->tls_psk){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_psk.");
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
#ಆದರೆ_ಅಂತ್ಯ
	}
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ(cr.log_dest_ಗಣ){
		config->log_dest = cr.log_dest;
	}
	ಆದರೆ(db.quiet){
		config->log_type = 0;
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(db.verbose){
		config->log_type = UINT_MAX;
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cr.log_type_ಗಣ){
		config->log_type = cr.log_type;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
	ಹಿಂತಿರುಗಿ config__check_bridges(config);
#ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
#ಆದರೆ_ಅಂತ್ಯ
}


ಸ್ಥಿರ mosquitto_plugin_id_t *config_plugin_ಕಂಡುಹಿಡಿ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
	ಆದರೆ(db.plugins && name){
		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<db.plugin_ಎಣಿಕೆ; i++){
			ಆದರೆ(db.plugins[i]->config.name && !strcmp(db.plugins[i]->config.name, name)){
				ಹಿಂತಿರುಗಿ db.plugins[i];
			}
		}
	}
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}


ಸ್ಥಿರ mosquitto_plugin_id_t *config_plugin_ತುಂಬು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ)
{
	mosquitto_plugin_id_t *plugin = ಖಾಲಿ;
	mosquitto_plugin_id_t **plugins = ಖಾಲಿ;

	ಆದರೆ(name && config_plugin_ಕಂಡುಹಿಡಿ(name)){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Duplicate plugin name '%s'.", name);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	ಆದರೆ(!ಮಾರ್ಗ || !strcmp(ಮಾರ್ಗ, "")){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Missing plugin path for plugin name '%s'.", name);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	plugin = mosquitto_calloc(1, ಗಾತ್ರ(mosquitto_plugin_id_t));
	ಆದರೆ(!plugin){
		ಹೋಗು error;
	}

	ಆದರೆ(name){
		plugin->config.name = mosquitto_strdup(name);
	}
	plugin->config.ಮಾರ್ಗ = mosquitto_strdup(ಮಾರ್ಗ);
	ಆದರೆ((name && !plugin->config.name) || !plugin->config.ಮಾರ್ಗ){
		ಹೋಗು error;
	}
	plugin->config.options = ಖಾಲಿ;
	plugin->config.option_ಎಣಿಕೆ = 0;
	plugin->config.deny_special_chars = true;

	/* Add to db list */
	plugins = mosquitto_realloc(db.plugins, (size_t)(db.plugin_ಎಣಿಕೆ+1)*ಗಾತ್ರ(ರಚನೆ mosquitto__plugin_config *));
	ಆದರೆ(!plugins){
		ಹೋಗು error;
	}

	plugins[db.plugin_ಎಣಿಕೆ] = plugin;
	db.plugins = plugins;
	db.plugin_ಎಣಿಕೆ++;

	ಹಿಂತಿರುಗಿ plugin;
error:
	log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
	ಆದರೆ(plugin){
		mosquitto_FREE(plugin->config.name);
		mosquitto_FREE(plugin->config.ಮಾರ್ಗ);
	}
	mosquitto_FREE(plugin);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}


ಪೂರ್ಣಾಂಕ config_plugin_ಕೂಡಿಸು_secopt(mosquitto_plugin_id_t *plugin, ರಚನೆ mosquitto__security_options *security_options)
{
	ರಚನೆ mosquitto__security_options **new_options;
	mosquitto_plugin_id_t **new_plugins;

	new_options = mosquitto_realloc(plugin->config.security_options, (size_t)(plugin->config.security_option_ಎಣಿಕೆ+1)*ಗಾತ್ರ(ರಚನೆ mosquitto__security_options *));
	new_plugins = mosquitto_realloc(security_options->plugins, (size_t)(security_options->plugin_ಎಣಿಕೆ+1)*ಗಾತ್ರ(mosquitto_plugin_id_t *));

	ಆದರೆ(!new_options || !new_plugins){
		mosquitto_FREE(new_options);
		mosquitto_FREE(new_plugins);
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}

	new_options[plugin->config.security_option_ಎಣಿಕೆ] = security_options;
	plugin->config.security_options = new_options;
	plugin->config.security_option_ಎಣಿಕೆ++;

	new_plugins[security_options->plugin_ಎಣಿಕೆ] = plugin;
	security_options->plugins = new_plugins;
	security_options->plugin_ಎಣಿಕೆ++;

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ config__read_file_core(ರಚನೆ mosquitto__config *config, bool reload, ರಚನೆ config_recurse *cr, ಪೂರ್ಣಾಂಕ level, ಪೂರ್ಣಾಂಕ *lineno, FILE *fptr, ಅಕ್ಷರ **buf, ಪೂರ್ಣಾಂಕ *buflen)
{
	ಪೂರ್ಣಾಂಕ rc;
	ಅಕ್ಷರ *token;
	ಪೂರ್ಣಾಂಕ tmp_int;
	ಅಕ್ಷರ *saveptr = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
	ಅಕ್ಷರ *tmp_char;
	ರಚನೆ mosquitto__bridge *cur_bridge = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
	mosquitto_plugin_id_t *cur_plugin = ಖಾಲಿ;

	ಅಕ್ಷರ *ಕೀಲಿ;
	ರಚನೆ mosquitto__listener *cur_listener = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ i;
	ಪೂರ್ಣಾಂಕ lineno_ext = 0;
	size_t ಪೂರ್ವಪ್ರತ್ಯಯ_len;
	size_t slen;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_WEBSOCKETS
	ಅಕ್ಷರ **ws_origins = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

	*lineno = 0;

	ಆಗ(mosquitto_fgets(buf, buflen, fptr)){
		(*lineno)++;
		ಆದರೆ((*buf)[0] != '#' && (*buf)[0] != 10 && (*buf)[0] != 13){
			slen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(*buf);
			ಆದರೆ(slen == 0){
				ಮುಂದುವರಿ;
			}
			ಆಗ((*buf)[slen-1] == 10 || (*buf)[slen-1] == 13){
				(*buf)[slen-1] = 0;
				slen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(*buf);
				ಆದರೆ(slen == 0){
					ಮುಂದುವರಿ;
				}
			}
			token = strtok_r((*buf), " ", &saveptr);
			ಆದರೆ(token){
				ಆದರೆ(!strcmp(token, "accept_protocol_versions")){
					REQUIRE_NON_DEFAULT_LISTENER(token);
					cur_listener->disable_protocol_v3 = true;
					cur_listener->disable_protocol_v4 = true;
					cur_listener->disable_protocol_v5 = true;
					ಆದರೆ(saveptr == ಖಾಲಿ){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", "accept_protocol_versions");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					token = strtok_r(saveptr, ", \t", &saveptr);
					REQUIRE_NON_ಖಾಲಿಗಳು_OPTION(token, "accept_protocol_versions");

					ಆಗ(token){
						ಆದರೆ(!strcmp(token, "3")){
							cur_listener->disable_protocol_v3 = false;
						}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "4")){
							cur_listener->disable_protocol_v4 = false;
						}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "5")){
							cur_listener->disable_protocol_v5 = false;
						}

						token = strtok_r(ಖಾಲಿ, ", \t", &saveptr);
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "acl_file")){
					REQUIRE_LISTENER_IF_ಪ್ರತಿ_LISTENER(token);

					conf_ಗಣ_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->acl_ದತ್ತಾಂಶ.acl_file);
					ಆದರೆ(conf__parse_string(&token, "acl_file", &cur_security_options->acl_ದತ್ತಾಂಶ.acl_file, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "address") || !strcmp(token, "addresses")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಆದರೆ(cur_bridge->ವಿಳಾಸಗಳು){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration, 'address' only allowed once.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆಗ((token = strtok_r(ಖಾಲಿ, " ", &saveptr))){
						ಆದರೆ(token[0] == '#'){
							ನಿಲ್ಲಿಸು;
						}
						ರಚನೆ bridge_ವಿಳಾಸ *new_ವಿಳಾಸಗಳು = mosquitto_realloc(cur_bridge->ವಿಳಾಸಗಳು, ಗಾತ್ರ(ರಚನೆ bridge_ವಿಳಾಸ)*(size_t)(cur_bridge->ವಿಳಾಸ_ಎಣಿಕೆ+1));
						ಆದರೆ(!new_ವಿಳಾಸಗಳು){
							log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
							ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
						}
						cur_bridge->ವಿಳಾಸ_ಎಣಿಕೆ++;
						cur_bridge->ವಿಳಾಸಗಳು = new_ವಿಳಾಸಗಳು;
						memset(&cur_bridge->ವಿಳಾಸಗಳು[cur_bridge->ವಿಳಾಸ_ಎಣಿಕೆ-1], 0, ಗಾತ್ರ(ರಚನೆ bridge_ವಿಳಾಸ));
						cur_bridge->ವಿಳಾಸಗಳು[cur_bridge->ವಿಳಾಸ_ಎಣಿಕೆ-1].ವಿಳಾಸ = mosquitto_strdup(token);
						ಆದರೆ(!cur_bridge->ವಿಳಾಸಗಳು[cur_bridge->ವಿಳಾಸ_ಎಣಿಕೆ-1].ವಿಳಾಸ){
							log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
							ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
						}
					}
					ಗಾಗಿ(i=0; i<cur_bridge->ವಿಳಾಸ_ಎಣಿಕೆ; i++){
						/* cur_bridge->addresses[i].address is now
						 * "address[:port]". If address is an IPv6 address,
						 * then port is required. We must check for the :
						 * backwards. */
						tmp_char = strrchr(cur_bridge->ವಿಳಾಸಗಳು[i].ವಿಳಾಸ, ':');
						ಆದರೆ(tmp_char){
							/* Remove ':', so cur_bridge->addresses[i].address
							 * now just looks like the address. */
							tmp_char[0] = '\0';

							/* The remainder of the string */
							tmp_int = atoi(&tmp_char[1]);
							ಆದರೆ(tmp_int < 1 || tmp_int > UINT16_MAX){
								log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid bridge port value (%d).", tmp_int);
								ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
							}
							cur_bridge->ವಿಳಾಸಗಳು[i].ಪೋರ್ಟ್ = (uint16_t)tmp_int;
						}ಇಲ್ಲದಿದ್ದರೆ{
							cur_bridge->ವಿಳಾಸಗಳು[i].ಪೋರ್ಟ್ = 1883;
						}
						conf__attempt_resolve(cur_bridge->ವಿಳಾಸಗಳು[i].ವಿಳಾಸ, "bridge address", MOSQ_LOG_WARNING, "Warning");
					}
					ಆದರೆ(cur_bridge->ವಿಳಾಸ_ಎಣಿಕೆ == 0){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Empty address value in configuration.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "allow_anonymous")){
					REQUIRE_LISTENER_IF_ಪ್ರತಿ_LISTENER(token);
					ಪ್ರತಿ_LISTENER_ALTERNATIVE(token, "listener_allow_anonymous");
					conf_ಗಣ_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ಆದರೆ(conf__parse_bool(&token, "allow_anonymous", (bool *)&cur_security_options->allow_anonymous, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "allow_duplicate_messages")){
					OPTION_DEPRECATED(token, "The behaviour will default to true.");
					ಆದರೆ(conf__parse_bool(&token, "allow_duplicate_messages", &config->allow_duplicate_messages, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "allow_zero_length_clientid")){
					REQUIRE_LISTENER_IF_ಪ್ರತಿ_LISTENER(token);
					conf_ಗಣ_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ಆದರೆ(conf__parse_bool(&token, "allow_zero_length_clientid", &cur_security_options->allow_zero_ಉದ್ದಳತೆ_clientid, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(token, "auth_opt_", ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("auth_opt_")) || !ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(token, "plugin_opt_", ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("plugin_opt_"))){
					ಆದರೆ(reload){
						ಮುಂದುವರಿ;        /* Auth plugin not currently valid for reloading. */

					}
					REQUIRE_PLUGIN(token);

					ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(token, "auth_opt_", ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("auth_opt_"))){
						ಪೂರ್ವಪ್ರತ್ಯಯ_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("auth_opt_");
					}ಇಲ್ಲದಿದ್ದರೆ{
						ಪೂರ್ವಪ್ರತ್ಯಯ_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("plugin_opt_");
					}
					ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(token) < ಪೂರ್ವಪ್ರತ್ಯಯ_len + 3){
						/* auth_opt_ == 9, + one digit key == 10, + one space == 11, + one value == 12 */
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid 'plugin_opt_' config option.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಕೀಲಿ = mosquitto_strdup(&token[ಪೂರ್ವಪ್ರತ್ಯಯ_len]);
					ಆದರೆ(!ಕೀಲಿ){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(STREMPTY(ಕೀಲಿ)){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Empty 'plugin_opt_' config option.");
						mosquitto_FREE(ಕೀಲಿ);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					token = saveptr;
					ಆದರೆ(token && token[0]){
						ಆಗ(token[0] == ' ' || token[0] == '\t'){
							token++;
						}
						ರಚನೆ mosquitto_opt *new_options = mosquitto_realloc(cur_plugin->config.options, (size_t)(cur_plugin->config.option_ಎಣಿಕೆ+1)*ಗಾತ್ರ(ರಚನೆ mosquitto_opt));
						ಆದರೆ(!new_options){
							log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
							mosquitto_FREE(ಕೀಲಿ);
							ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
						}
						cur_plugin->config.option_ಎಣಿಕೆ++;
						cur_plugin->config.options = new_options;
						cur_plugin->config.options[cur_plugin->config.option_ಎಣಿಕೆ-1].ಕೀಲಿ = ಕೀಲಿ;
						cur_plugin->config.options[cur_plugin->config.option_ಎಣಿಕೆ-1].ಮೌಲ್ಯ = mosquitto_strdup(token);
						ಆದರೆ(!cur_plugin->config.options[cur_plugin->config.option_ಎಣಿಕೆ-1].ಮೌಲ್ಯ){
							log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
							ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
						}
					}ಇಲ್ಲದಿದ್ದರೆ{
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", ಕೀಲಿ);
						mosquitto_FREE(ಕೀಲಿ);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "auth_plugin") || !strcmp(token, "plugin") || !strcmp(token, "global_plugin")){
					ಆದರೆ(reload){
						ಮುಂದುವರಿ;        /* plugin not currently valid for reloading. */
					}
					ಆದರೆ(!strcmp(token, "global_plugin")){
						cur_security_options = &db.config->security_options;
					}ಇಲ್ಲದಿದ್ದರೆ{
						REQUIRE_LISTENER_IF_ಪ್ರತಿ_LISTENER(token);
						conf_ಗಣ_cur_security_options(config, &cur_listener, &cur_security_options, token);
					}

					cur_plugin = config_plugin_ತುಂಬು(ಖಾಲಿ, saveptr);
					ಆದರೆ(cur_plugin == ಖಾಲಿ){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(config_plugin_ಕೂಡಿಸು_secopt(cur_plugin, cur_security_options)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "auth_plugin_deny_special_chars")){
					ಆದರೆ(reload){
						ಮುಂದುವರಿ;        /* Auth plugin not currently valid for reloading. */
					}
					ಆದರೆ(!cur_plugin){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: An auth_plugin_deny_special_chars option exists in the config file without a plugin.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(conf__parse_bool(&token, "auth_plugin_deny_special_chars", &cur_plugin->config.deny_special_chars, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "plugin_load")){
					ಅಕ್ಷರ *name = ಖಾಲಿ;

					ಆದರೆ(reload){
						ಮುಂದುವರಿ;        /* plugin not currently valid for reloading. */

					}
					name = strtok_r(ಖಾಲಿ, " ", &saveptr);
					REQUIRE_NON_ಖಾಲಿಗಳು_OPTION(name, "plugin_load");

					cur_plugin = config_plugin_ತುಂಬು(name, saveptr);
					ಆದರೆ(cur_plugin == ಖಾಲಿ){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "plugin_use")){
					ಅಕ್ಷರ *name = ಖಾಲಿ;

					ಆದರೆ(reload){
						ಮುಂದುವರಿ;        /* plugin not currently valid for reloading. */
					}
					REQUIRE_NON_DEFAULT_LISTENER(token);

					ಆದರೆ(conf__parse_string(&token, "plugin_use", &name, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					cur_plugin = config_plugin_ಕಂಡುಹಿಡಿ(name);
					ಆದರೆ(!cur_plugin){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Plugin '%s' not previously loaded.", name);
						mosquitto_FREE(name);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(name);
					ಆದರೆ(config_plugin_ಕೂಡಿಸು_secopt(cur_plugin, cur_listener->security_options)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "auto_id_prefix")){
					REQUIRE_LISTENER_IF_ಪ್ರತಿ_LISTENER(token);
					OPTION_DEPRECATED(token, "Please use 'listener_auto_id_prefix' instead.");
					conf_ಗಣ_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ಆದರೆ(conf__parse_string(&token, "auto_id_prefix", &cur_security_options->auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(cur_security_options->auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ){
						ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(cur_security_options->auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ) > 50){
							log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: auto_id_prefix length must be <= 50.");
							ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
						}
						cur_security_options->auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ_len = (uint16_t)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(cur_security_options->auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ);
					}ಇಲ್ಲದಿದ್ದರೆ{
						cur_security_options->auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ_len = 0;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "autosave_interval")){
					ಆದರೆ(conf__parse_int(&token, "autosave_interval", &config->autosave_interval, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(config->autosave_interval < 0){
						config->autosave_interval = 0;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "autosave_on_changes")){
					ಆದರೆ(conf__parse_bool(&token, "autosave_on_changes", &config->autosave_on_changes, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "bind_address")){
					OPTION_DEPRECATED(token, "");
					config->local_only = false;
					ಆದರೆ(reload){
						ಮುಂದುವರಿ;        /* Rebinding listeners not valid during reloading. */

					}
					ಆದರೆ(config__create_default_listener(config, token)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					ಆದರೆ(conf__parse_string(&token, "default listener bind_address", &config->default_listener->host, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(conf__attempt_resolve(config->default_listener->host, "bind_address", MOSQ_LOG_ERR, "Error")){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "bind_interface")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SO_BINDTODEVICE
					ಆದರೆ(reload){
						ಮುಂದುವರಿ;        /* Rebinding listeners not valid during reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ಆದರೆ(conf__parse_string(&token, "bind_interface", &cur_listener->bind_interface, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: bind_interface specified but socket option not available.");
					ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "bridge_attempt_unsubscribe")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_bool(&token, "bridge_attempt_unsubscribe", &cur_bridge->attempt_unsubscribe, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "bridge_cafile")){
#ಆದರೆ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					ಆದರೆ(conf__parse_string(&token, "bridge_cafile", &cur_bridge->tls_cafile, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "bridge_alpn")){
#ಆದರೆ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_string(&token, "bridge_alpn", &cur_bridge->tls_alpn, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "bridge_ciphers")){
#ಆದರೆ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_string(&token, "bridge_ciphers", &cur_bridge->tls_ciphers, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "bridge_ciphers_tls1.3")){
#ಆದರೆ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_string(&token, "bridge_ciphers_tls1.3", &cur_bridge->tls_13_ciphers, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "bridge_bind_address")){
#ಆದರೆ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_string(&token, "bridge_bind_address", &cur_bridge->bind_ವಿಳಾಸ, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "bridge_capath")){
#ಆದರೆ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					ಆದರೆ(conf__parse_string(&token, "bridge_capath", &cur_bridge->tls_capath, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "bridge_certfile")){
#ಆದರೆ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					ಆದರೆ(conf__parse_string(&token, "bridge_certfile", &cur_bridge->tls_certfile, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "bridge_identity")){
#ಆದರೆ defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					ಆದರೆ(conf__parse_string(&token, "bridge_identity", &cur_bridge->tls_psk_ತಟಸ್ಥ, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "bridge_insecure")){
#ಆದರೆ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_bool(&token, "bridge_insecure", &cur_bridge->tls_insecure, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(cur_bridge->tls_insecure){
						log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge '%s' using insecure mode.", cur_bridge->name);
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "bridge_require_ocsp")){
#ಆದರೆ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_bool(&token, "bridge_require_ocsp", &cur_bridge->tls_ocsp_required, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "bridge_max_packet_size")){
#ಆದರೆ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_int(&token, "bridge_max_packet_size", &tmp_int, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(tmp_int < 0){
						tmp_int = 0;
					}
					cur_bridge->ಗರಿಷ್ಠ_packet_ಗಾತ್ರ = (uint32_t)tmp_int;
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "bridge_max_topic_alias")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_int(&token, "bridge_max_topic_alias", &tmp_int, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}

					ಆದರೆ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: bridge_max_topic_alias must be > 0 and <= 65535.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					cur_bridge->max_topic_alias = (uint16_t)tmp_int;
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "bridge_outgoing_retain")){
#ಆದರೆ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_bool(&token, "bridge_outgoing_retain", &cur_bridge->outgoing_retain, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "bridge_keyfile")){
#ಆದರೆ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					mosquitto_FREE(cur_bridge->tls_keyfile);
					ಆದರೆ(conf__parse_string(&token, "bridge_keyfile", &cur_bridge->tls_keyfile, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "bridge_protocol_version")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(ಖಾಲಿ, "", &saveptr);
					REQUIRE_NON_ಖಾಲಿಗಳು_OPTION(token, "bridge_protocol_version");

					ಆದರೆ(!strcmp(token, "mqttv31")){
						cur_bridge->protocol_ಆವೃತ್ತಿ = mosq_p_mqtt31;
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "mqttv311")){
						cur_bridge->protocol_ಆವೃತ್ತಿ = mosq_p_mqtt311;
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "mqttv50")){
						cur_bridge->protocol_ಆವೃತ್ತಿ = mosq_p_mqtt5;
					}ಇಲ್ಲದಿದ್ದರೆ{
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid 'bridge_protocol_version' value (%s).", token);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "bridge_psk")){
#ಆದರೆ defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					ಆದರೆ(conf__parse_string(&token, "bridge_psk", &cur_bridge->tls_psk, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "bridge_receive_maximum")){
#ಆದರೆ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_int(&token, "bridge_receive_maximum", &tmp_int, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(tmp_int <= 0){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be greater than 0.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((uint64_t)tmp_int > (uint64_t)UINT16_MAX){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be lower than %u.", UINT16_MAX);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					cur_bridge->receive_ಗರಿಷ್ಠ = (uint16_t)tmp_int;
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "bridge_reload_type")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(ಖಾಲಿ, " ", &saveptr);
					REQUIRE_NON_ಖಾಲಿಗಳು_OPTION(token, "bridge_reload_type");

					ಆದರೆ(!strcmp(token, "lazy")){
						cur_bridge->reload_type = brt_lazy;
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "immediate")){
						cur_bridge->reload_type = brt_ತಕ್ಷಣ;
					}ಇಲ್ಲದಿದ್ದರೆ{
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid 'bridge_reload_type' value in configuration (%s).", token);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "bridge_session_expiry_interval")){
#ಆದರೆ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_int(&token, "bridge_session_expiry_interval", &tmp_int, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(tmp_int < 0){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must not be negative.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((uint64_t)tmp_int > (uint64_t)MQTT_SESSION_EXPIRY_NEVER){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must be lower than %u.", MQTT_SESSION_EXPIRY_NEVER);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					cur_bridge->session_expiry_interval = (uint32_t)tmp_int;
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "bridge_tcp_keepalive")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
					REQUIRE_BRIDGE(token);

					ಆದರೆ(conf__parse_int(&token, "bridge_tcp_keepalive_idle", &tmp_int, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(tmp_int <= 0){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: invalid TCP keepalive idle value.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_idle = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)tmp_int;

					ಆದರೆ(conf__parse_int(&token, "bridge_tcp_keepalive_interval", &tmp_int, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(tmp_int <= 0){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: invalid TCP keepalive interval value.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_interval = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)tmp_int;

					ಆದರೆ(conf__parse_int(&token, "bridge_tcp_keepalive_counter", &tmp_int, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(tmp_int <= 0){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: invalid TCP keepalive counter value.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_counter = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)tmp_int;
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "bridge_tcp_user_timeout")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TCP_USER_TIMEOUT
					ಆದರೆ(conf__parse_int(&token, "bridge_tcp_user_timeout", &tmp_int, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(tmp_int < 0){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: invalid TCP user timeout value.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_user_timeout = tmp_int;
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge TCP user timeout support not available.");
#ಆದರೆ_ಅಂತ್ಯ
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "bridge_tls_use_os_certs")){
#ಆದರೆ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_bool(&token, "bridge_tls_use_os_certs", &cur_bridge->tls_use_os_certs, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "bridge_tls_version")){
#ಆದರೆ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_string(&token, "bridge_tls_version", &cur_bridge->tls_ಆವೃತ್ತಿ, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "cafile")){
#ಆದರೆ defined(WITH_TLS)
					REQUIRE_LISTENER(token);
					ಆದರೆ(cur_listener->psk_hint){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->cafile);
					ಆದರೆ(conf__parse_string(&token, "cafile", &cur_listener->cafile, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "capath")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->capath);
					ಆದರೆ(conf__parse_string(&token, "capath", &cur_listener->capath, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "certfile")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ಆದರೆ(cur_listener->psk_hint){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->certfile);
					ಆದರೆ(conf__parse_string(&token, "certfile", &cur_listener->certfile, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "check_retain_source")){
					ಆದರೆ(conf__parse_bool(&token, "check_retain_source", &config->check_retain_ಮೂಲ, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "ciphers")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers);
					ಆದರೆ(conf__parse_string(&token, "ciphers", &cur_listener->ciphers, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "ciphers_tls1.3")){
#ಆದರೆ defined(WITH_TLS) && (!defined(LIBRESSL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ) || LIBRESSL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ > 0x3040000FL)
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers_tls13);
					ಆದರೆ(conf__parse_string(&token, "ciphers_tls1.3", &cur_listener->ciphers_tls13, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: ciphers_tls1.3 support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "clientid") || !strcmp(token, "remote_clientid")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_string(&token, "bridge remote clientid", &cur_bridge->remote_clientid, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "cleansession")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_bool(&token, "cleansession", &cur_bridge->clean_ಆರಂಭ, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "local_cleansession")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_bool(&token, "local_cleansession", (bool *)&cur_bridge->clean_ಆರಂಭ_local, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "clientid_prefixes")){
					OPTION_DEPRECATED(token, "");
					mosquitto_FREE(config->clientid_prefixes);
					ಆದರೆ(conf__parse_string(&token, "clientid_prefixes", &config->clientid_prefixes, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "connection")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
					token = strtok_r(ಖಾಲಿ, " ", &saveptr);
					REQUIRE_NON_ಖಾಲಿಗಳು_OPTION(token, "connection");

					/* Check for existing bridge name. */
					ಗಾಗಿ(i=0; i<config->bridge_ಎಣಿಕೆ; i++){
						ಆದರೆ(!strcmp(config->bridges[i]->name, token)){
							log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Duplicate bridge name '%s'.", token);
							ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
						}
					}

					ರಚನೆ mosquitto__bridge **bridges_new = mosquitto_realloc(config->bridges, (size_t)(config->bridge_ಎಣಿಕೆ+1)*ಗಾತ್ರ(ರಚನೆ mosquitto__bridge *));
					ಆದರೆ(!bridges_new){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
					}
					config->bridges = bridges_new;
					cur_bridge = mosquitto_malloc(ಗಾತ್ರ(ರಚನೆ mosquitto__bridge));
					ಆದರೆ(!cur_bridge){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
					}

					config->bridge_ಎಣಿಕೆ++;
					config->bridges[config->bridge_ಎಣಿಕೆ-1] = cur_bridge;

					memset(cur_bridge, 0, ಗಾತ್ರ(ರಚನೆ mosquitto__bridge));
					cur_bridge->name = mosquitto_strdup(token);
					ಆದರೆ(!cur_bridge->name){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
					}
					cur_bridge->keepalive = 60;
					cur_bridge->notifications = true;
					cur_bridge->notifications_local_only = false;
					cur_bridge->ಆರಂಭ_type = bst_automatic;
					cur_bridge->idle_timeout = 60;
					cur_bridge->restart_timeout = 0;
					cur_bridge->backoff_ತಳ = 5 * 1000;
					cur_bridge->backoff_cap = 30 * 1000;
					cur_bridge->stable_connection_period = 0;
					cur_bridge->ಮಿತಿ = 10;
					cur_bridge->try_ಖಾಸಗಿ = true;
					cur_bridge->attempt_unsubscribe = true;
					cur_bridge->protocol_ಆವೃತ್ತಿ = mosq_p_mqtt311;
					cur_bridge->primary_retry_sock = INVALID_SOCKET;
					cur_bridge->outgoing_retain = true;
					cur_bridge->clean_ಆರಂಭ_local = -1;
					cur_bridge->reload_type = brt_lazy;
					cur_bridge->max_topic_alias = 10;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TCP_USER_TIMEOUT
					cur_bridge->tcp_user_timeout = -1;
#ಆದರೆ_ಅಂತ್ಯ
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "connection_messages")){
					ಆದರೆ(conf__parse_bool(&token, token, &config->connection_messages, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "crlfile")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->crlfile);
					ಆದರೆ(conf__parse_string(&token, "crlfile", &cur_listener->crlfile, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "dhparamfile")){
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: dhparamfile is no longer required.");
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "disable_client_cert_date_checks")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
					REQUIRE_LISTENER(token);
					ಆದರೆ(conf__parse_bool(&token, "disable_client_cert_date_checks", &cur_listener->disable_client_cert_date_checks, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "enable_control_api")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_CONTROL
					ಆದರೆ(conf__parse_bool(&token, "enable_control_api", &config->enable_control_api, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: $CONTROL support not available (enable_control_api).");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "enable_proxy_protocol")){
#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
#ಆದರೆ_ಅಂತ್ಯ
					REQUIRE_LISTENER(token);
					ಆದರೆ(conf__parse_int(&token, "enable_proxy_protocol", &cur_listener->enable_proxy_protocol, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(cur_listener->enable_proxy_protocol < 1 || cur_listener->enable_proxy_protocol > 2){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: enable_proxy_protocol must be 1 or 2.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "global_max_clients")){
					ಆದರೆ(conf__parse_int(&token, "global_max_clients", &config->global_max_clients, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "global_max_connections")){
					ಆದರೆ(conf__parse_int(&token, "global_max_connections", &config->global_max_connections, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "http_dir")){
#ಆದರೆ defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
					ಆದರೆ(reload){
						ಮುಂದುವರಿ;        /* Not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					ಆದರೆ(conf__parse_string(&token, "http_dir", &cur_listener->http_dir, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
					ಅಕ್ಷರ *http_dir_canonical = _fullpath(ಖಾಲಿ, cur_listener->http_dir, 0);
					ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ sep = '\\';
#ಇಲ್ಲದಿದ್ದರೆ
					ಅಕ್ಷರ *http_dir_canonical = realpath(cur_listener->http_dir, ಖಾಲಿ);
					ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ sep = '/';
#ಆದರೆ_ಅಂತ್ಯ
					ಆದರೆ(!http_dir_canonical){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
					}
					size_t http_dir_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(http_dir_canonical) + ಗಾತ್ರ(sep) + 1;
					ಅಕ್ಷರ *http_dir_canonical_sep = mosquitto_calloc(http_dir_len, ಗಾತ್ರ(ಅಕ್ಷರ));
					ಆದರೆ(!http_dir_canonical_sep){
						ಬಿಡುಗಡೆ(http_dir_canonical);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
					}
					snprintf(http_dir_canonical_sep, http_dir_len, "%s%c", http_dir_canonical, sep);
					ಬಿಡುಗಡೆ(http_dir_canonical);
					mosquitto_FREE(cur_listener->http_dir);
					cur_listener->http_dir = http_dir_canonical_sep;
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: http_dir support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "idle_timeout")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_int(&token, "idle_timeout", &cur_bridge->idle_timeout, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(cur_bridge->idle_timeout < 1){
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "idle_timeout interval too low, using 1 second.");
						cur_bridge->idle_timeout = 1;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "include_dir")){
					ಆದರೆ(level == 0){
						ಅಕ್ಷರ **files;
						ಪೂರ್ಣಾಂಕ file_ಎಣಿಕೆ;

						/* Only process include_dir from the main config file. */
						token = strtok_r(ಖಾಲಿ, "", &saveptr);
						REQUIRE_NON_ಖಾಲಿಗಳು_OPTION(token, "include_dir");

						rc = config__get_dir_files(token, &files, &file_ಎಣಿಕೆ);
						ಆದರೆ(rc){
							ಹಿಂತಿರುಗಿ rc;
						}

						ಗಾಗಿ(i=0; i<file_ಎಣಿಕೆ; i++){
							log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Loading config file '%s'", files[i]);

							rc = config__read_file(config, reload, files[i], cr, level+1, &lineno_ext);
							ಆದರೆ(rc){
								ಆದರೆ(lineno_ext > 0){
									log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error found at '%s:%d'.", files[i], lineno_ext);
								}
								/* Free happens below */
								ನಿಲ್ಲಿಸು;
							}
						}
						ಗಾಗಿ(i=0; i<file_ಎಣಿಕೆ; i++){
							mosquitto_FREE(files[i]);
						}
						mosquitto_FREE(files);
						ಆದರೆ(rc){
							ಹಿಂತಿರುಗಿ rc;    /* This returns if config__read_file() fails above */
						}
					}ಇಲ್ಲದಿದ್ದರೆ{
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: The include_dir option is only valid in the main configuration file.");
						ಹಿಂತಿರುಗಿ 1;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "keepalive_interval")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_int(&token, "keepalive_interval", &tmp_int, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(tmp_int > UINT16_MAX){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Bridge keepalive value too high.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(tmp_int < 5){
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "keepalive interval too low, using 5 seconds.");
						tmp_int = 5;
					}
					cur_bridge->keepalive = (uint16_t)tmp_int;
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "keyfile")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->keyfile);
					ಆದರೆ(conf__parse_string(&token, "keyfile", &cur_listener->keyfile, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "listener")){
					config->local_only = false;

					ಆದರೆ(conf__parse_int(&token, "listener port", &tmp_int, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_UNIX_SOCKETS
					ಆದರೆ(tmp_int < 0 || tmp_int > UINT16_MAX){
#ಇಲ್ಲದಿದ್ದರೆ
					ಆದರೆ(tmp_int < 1 || tmp_int > UINT16_MAX){
#ಆದರೆ_ಅಂತ್ಯ
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}

					/* Look for bind address / unix socket path */
					token = strtok_r(ಖಾಲಿ, " ", &saveptr);
					ಆದರೆ(token != ಖಾಲಿ && token[0] == '#'){
						token = ಖಾಲಿ;
					}

					ಆದರೆ(tmp_int == 0 && token == ಖಾಲಿ){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: A listener with port 0 must provide a Unix socket path.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}

					ಆದರೆ(reload){
						/* We reload listeners settings based on port number/unix socket path.
						 * If the port number/unix path doesn't already exist, exit with a complaint. */
						cur_listener = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_UNIX_SOCKETS
						ಆದರೆ(tmp_int == 0){
							ಗಾಗಿ(i=0; i<config->listener_ಎಣಿಕೆ; i++){
								ಆದರೆ(config->listeners[i].unix_socket_ಮಾರ್ಗ != ಖಾಲಿ
										&& strcmp(config->listeners[i].unix_socket_ಮಾರ್ಗ, token) == 0){

									cur_listener = &config->listeners[i];
									ನಿಲ್ಲಿಸು;
								}
							}
						}ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
						{
							ಗಾಗಿ(i=0; i<config->listener_ಎಣಿಕೆ; i++){
								ಆದರೆ(config->listeners[i].ಪೋರ್ಟ್ == tmp_int){
									/* Now check we have a matching bind address, if defined */
									ಆದರೆ(config->listeners[i].host){
										ಆದರೆ(token && !strcmp(config->listeners[i].host, token)){
											/* They both have a bind address, and they match */
											cur_listener = &config->listeners[i];
											ನಿಲ್ಲಿಸು;
										}
									}ಇಲ್ಲದಿದ್ದರೆ{
										ಆದರೆ(token == ಖಾಲಿ){
											/* Neither this config nor the new config have a bind address,
											 * so they match. */
											cur_listener = &config->listeners[i];
											ನಿಲ್ಲಿಸು;
										}
									}
								}
							}
						}
						ಆದರೆ(!cur_listener){
							log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: It is not currently possible to add/remove listeners when reloading the config file.");
							ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
						}
					}ಇಲ್ಲದಿದ್ದರೆ{
						ಆದರೆ(config_ಕೂಡಿಸು_listener(config)){
							log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
							ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
						}
						cur_listener = &config->listeners[config->listener_ಎಣಿಕೆ-1];
					}

					listener_ಗಣ_defaults(cur_listener);
					cur_listener->ಪೋರ್ಟ್ = (uint16_t)tmp_int;

					mosquitto_FREE(cur_listener->host);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_UNIX_SOCKETS
					mosquitto_FREE(cur_listener->unix_socket_ಮಾರ್ಗ);
#ಆದರೆ_ಅಂತ್ಯ

					ಆದರೆ(token){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_UNIX_SOCKETS
						ಆದರೆ(cur_listener->ಪೋರ್ಟ್ == 0){
							cur_listener->unix_socket_ಮಾರ್ಗ = mosquitto_strdup(token);
						}ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
						{
							cur_listener->host = mosquitto_strdup(token);
						}
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "listener_allow_anonymous")){
					REQUIRE_LISTENER(token);
					ಆದರೆ(conf__parse_bool(&token, "listener_allow_anonymous", (bool *)&cur_listener->security_options->allow_anonymous, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "listener_auto_id_prefix")){
					REQUIRE_LISTENER(token);
					ಆದರೆ(conf__parse_string(&token, "listener_auto_id_prefix", &cur_listener->security_options->auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(cur_listener->security_options->auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ){
						ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(cur_listener->security_options->auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ) > 50){
							log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: 'listener_auto_id_prefix' length must be <= 50.");
							ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
						}
						cur_listener->security_options->auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ_len = (uint16_t)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(cur_listener->security_options->auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ);
					}ಇಲ್ಲದಿದ್ದರೆ{
						cur_listener->security_options->auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ_len = 0;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "local_clientid")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_string(&token, "bridge local clientd", &cur_bridge->local_clientid, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "local_password")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_string(&token, "bridge local_password", &cur_bridge->local_password, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "local_username")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_string(&token, "bridge local_username", &cur_bridge->local_username, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "log_dest")){
					token = strtok_r(ಖಾಲಿ, " ", &saveptr);
					REQUIRE_NON_ಖಾಲಿಗಳು_OPTION(token, "log_dest");

					cr->log_dest_ಗಣ = 1;
					ಆದರೆ(!strcmp(token, "none")){
						cr->log_dest = MQTT3_LOG_NONE;
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "syslog")){
						cr->log_dest |= MQTT3_LOG_SYSLOG;
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "stdout")){
						cr->log_dest |= MQTT3_LOG_STDOUT;
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "stderr")){
						cr->log_dest |= MQTT3_LOG_STDERR;
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "topic")){
						cr->log_dest |= MQTT3_LOG_TOPIC;
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "dlt")){
						cr->log_dest |= MQTT3_LOG_DLT;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ANDROID
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "android")){
						cr->log_dest |= MQTT3_LOG_ANDROID;
#ಆದರೆ_ಅಂತ್ಯ
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "file")){
						ಆದರೆ(config->log_fptr || config->log_file){
							log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Duplicate \"log_dest file\" value.");
							ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
						}
						/* Get remaining string. */
						token = saveptr;
						ಆದರೆ(token && token[0]){
							ಆಗ(token[0] == ' ' || token[0] == '\t'){
								token++;
							}
						}
						/* Duplicate "token" check here saves a log__printf() */
						ಆದರೆ(token && token[0]){
							config->log_file = mosquitto_strdup(token);
							ಆದರೆ(!config->log_file){
								log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
								ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
							}
							cr->log_dest |= MQTT3_LOG_FILE;
						}ಇಲ್ಲದಿದ್ದರೆ{
							log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Empty \"log_dest file\" value in configuration.");
							ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
						}
						cr->log_dest |= MQTT3_LOG_FILE;
					}ಇಲ್ಲದಿದ್ದರೆ{
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid 'log_dest' value (%s).", token);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಆದರೆ defined(WIN32) || defined(__CYGWIN__)
					ಆದರೆ(service_handle){
						ಆದರೆ(cr->log_dest == MQTT3_LOG_STDOUT || cr->log_dest == MQTT3_LOG_STDERR){
							log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Cannot log to stdout/stderr when running as a Windows service.");
							ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
						}
					}
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "log_facility")){
#ಆದರೆ defined(WIN32) || defined(__CYGWIN__)
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: log_facility not supported on Windows.");
#ಇಲ್ಲದಿದ್ದರೆ
					ಆದರೆ(conf__parse_int(&token, "log_facility", &tmp_int, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆಯ್ಕೆ(tmp_int){
						ಸ್ಥಿತಿ 0:
							config->log_facility = LOG_LOCAL0;
							ನಿಲ್ಲಿಸು;
						ಸ್ಥಿತಿ 1:
							config->log_facility = LOG_LOCAL1;
							ನಿಲ್ಲಿಸು;
						ಸ್ಥಿತಿ 2:
							config->log_facility = LOG_LOCAL2;
							ನಿಲ್ಲಿಸು;
						ಸ್ಥಿತಿ 3:
							config->log_facility = LOG_LOCAL3;
							ನಿಲ್ಲಿಸು;
						ಸ್ಥಿತಿ 4:
							config->log_facility = LOG_LOCAL4;
							ನಿಲ್ಲಿಸು;
						ಸ್ಥಿತಿ 5:
							config->log_facility = LOG_LOCAL5;
							ನಿಲ್ಲಿಸು;
						ಸ್ಥಿತಿ 6:
							config->log_facility = LOG_LOCAL6;
							ನಿಲ್ಲಿಸು;
						ಸ್ಥಿತಿ 7:
							config->log_facility = LOG_LOCAL7;
							ನಿಲ್ಲಿಸು;
						ಪೂರ್ವನಿಯೋಜಿತ:
							log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid 'log_facility' value (%d).", tmp_int);
							ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "log_timestamp")){
					ಆದರೆ(conf__parse_bool(&token, token, &config->log_timestamp, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "log_timestamp_format")){
					ಆದರೆ(conf__parse_string(&token, token, &config->log_timestamp_format, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "log_type")){
					token = strtok_r(ಖಾಲಿ, " ", &saveptr);
					REQUIRE_NON_ಖಾಲಿಗಳು_OPTION(token, "log_type");

					cr->log_type_ಗಣ = 1;
					ಆದರೆ(!strcmp(token, "none")){
						cr->log_type = MOSQ_LOG_NONE;
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "information")){
						cr->log_type |= MOSQ_LOG_INFO;
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "notice")){
						cr->log_type |= MOSQ_LOG_NOTICE;
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "warning")){
						cr->log_type |= MOSQ_LOG_WARNING;
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "error")){
						cr->log_type |= MOSQ_LOG_ERR;
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "debug")){
						cr->log_type |= MOSQ_LOG_DEBUG;
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "subscribe")){
						cr->log_type |= MOSQ_LOG_SUBSCRIBE;
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "unsubscribe")){
						cr->log_type |= MOSQ_LOG_UNSUBSCRIBE;
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "internal")){
						cr->log_type |= MOSQ_LOG_ಆಂತರಿಕ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_WEBSOCKETS
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "websockets")){
						cr->log_type |= MOSQ_LOG_WEBSOCKETS;
#ಆದರೆ_ಅಂತ್ಯ
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "all")){
						cr->log_type = MOSQ_LOG_ALL;
					}ಇಲ್ಲದಿದ್ದರೆ{
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid 'log_type' value (%s).", token);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "max_connections")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ಆದರೆ(conf__parse_int(&token, token, &cur_listener->max_connections, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(cur_listener->max_connections < 0){
						cur_listener->max_connections = -1;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "maximum_qos") || !strcmp(token, "max_qos")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ಆದರೆ(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(tmp_int < 0 || tmp_int > 2){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: 'max_qos' must be between 0 and 2 inclusive.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					cur_listener->max_qos = (uint8_t)tmp_int;
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "max_inflight_bytes")){
					ಆದರೆ(conf__parse_int(&token, "max_inflight_bytes", &tmp_int, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_inflight_ಬೈಟುಗಳು = (size_t)tmp_int;
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "max_inflight_messages")){
					ಆದರೆ(conf__parse_int(&token, "max_inflight_messages", &tmp_int, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(tmp_int < 0 || tmp_int == UINT16_MAX){
						tmp_int = 0;
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(tmp_int > UINT16_MAX){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: 'max_inflight_messages' must be <= 65535.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					config->max_inflight_messages = (uint16_t)tmp_int;
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "max_keepalive")){
					ಆದರೆ(conf__parse_int(&token, "max_keepalive", &tmp_int, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid 'max_keepalive' value (%d).", tmp_int);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					config->max_keepalive = (uint16_t)tmp_int;
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "max_packet_size")){
					ಆದರೆ(conf__parse_int(&token, "max_packet_size", &tmp_int, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(tmp_int < 20){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: 'max_packet_size' must be >= 20.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					config->max_packet_ಗಾತ್ರ = (uint32_t)tmp_int;
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "max_queued_bytes")){
					ಆದರೆ(conf__parse_int(&token, "max_queued_bytes", &tmp_int, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_ಬೈಟುಗಳು = (size_t)tmp_int;
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "max_queued_messages")){
					ಆದರೆ(conf__parse_int(&token, "max_queued_messages", &tmp_int, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_messages = tmp_int;
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "memory_limit")){
					ssize_t lim;
					ಆದರೆ(conf__parse_ssize_t(&token, "memory_limit", &lim, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(lim < 0){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid 'memory_limit' value (%ld).", lim);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					mosquitto_ಸ್ಮರಣೆ_ಗಣ_ಎಲ್ಲೆ((size_t)lim);
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "message_size_limit")){
					log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Note: It is recommended to replace `message_size_limit` with `max_packet_size`.");
					ಆದರೆ(conf__parse_int(&token, "message_size_limit", (ಪೂರ್ಣಾಂಕ *)&config->ಸಂದೇಶ_ಗಾತ್ರ_ಎಲ್ಲೆ, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(config->ಸಂದೇಶ_ಗಾತ್ರ_ಎಲ್ಲೆ > MQTT_MAX_PAYLOAD){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid 'message_size_limit' value (%u).", config->ಸಂದೇಶ_ಗಾತ್ರ_ಎಲ್ಲೆ);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "mount_point")){
					REQUIRE_LISTENER(token);
					mosquitto_FREE(cur_listener->mount_ಬಿಂದು);
					ಆದರೆ(conf__parse_string(&token, "mount_point", &cur_listener->mount_ಬಿಂದು, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(mosquitto_pub_topic_check(cur_listener->mount_ಬಿಂದು) != MOSQ_ERR_SUCCESS){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR,
								"Error: Invalid 'mount_point' value (%s). Does it contain a wildcard character?",
								cur_listener->mount_ಬಿಂದು);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "notifications")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_bool(&token, "notifications", &cur_bridge->notifications, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "notifications_local_only")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_bool(&token, "notifications_local_only", &cur_bridge->notifications_local_only, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "notification_topic")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_string(&token, "notification_topic", &cur_bridge->notification_topic, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "password") || !strcmp(token, "remote_password")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_string(&token, "bridge remote_password", &cur_bridge->remote_password, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "password_file")){
					REQUIRE_LISTENER_IF_ಪ್ರತಿ_LISTENER(token);
					conf_ಗಣ_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->password_ದತ್ತಾಂಶ.password_file);
					ಆದರೆ(conf__parse_string(&token, "password_file", &cur_security_options->password_ದತ್ತಾಂಶ.password_file, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "per_listener_settings")){
					OPTION_DEPRECATED(token, "Please see the documentation for how to achieve the same effect.");
					ಆದರೆ(config->ಪ್ರತಿ_listener_settings){
						/* Once this is set, don't let it be unset. It should be the first config option ideally. */
						ಮುಂದುವರಿ;
					}
					ಆದರೆ(conf__parse_bool(&token, "per_listener_settings", &config->ಪ್ರತಿ_listener_settings, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(cur_security_options && config->ಪ್ರತಿ_listener_settings){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: per_listener_settings must be set before any other security settings.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "persistence") || !strcmp(token, "retained_persistence")){
					ಆದರೆ(conf__parse_bool(&token, token, &config->persistence, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "persistence_file")){
					ಆದರೆ(conf__parse_string(&token, "persistence_file", &config->persistence_file, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "persistence_location")){
					ಆದರೆ(conf__parse_string(&token, "persistence_location", &config->persistence_location, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "persistent_client_expiration")){
					token = strtok_r(ಖಾಲಿ, " ", &saveptr);
					REQUIRE_NON_ಖಾಲಿಗಳು_OPTION(token, "persistent_client_expiration");

					time_t expiration_mult;

					ಆಯ್ಕೆ(token[ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(token)-1]){
						ಸ್ಥಿತಿ 's':
							expiration_mult = 1;
							ನಿಲ್ಲಿಸು;
						ಸ್ಥಿತಿ 'h':
							expiration_mult = 3600;
							ನಿಲ್ಲಿಸು;
						ಸ್ಥಿತಿ 'd':
							expiration_mult = 86400;
							ನಿಲ್ಲಿಸು;
						ಸ್ಥಿತಿ 'w':
							expiration_mult = 86400*7;
							ನಿಲ್ಲಿಸು;
						ಸ್ಥಿತಿ 'm':
							expiration_mult = 86400*30;
							ನಿಲ್ಲಿಸು;
						ಸ್ಥಿತಿ 'y':
							expiration_mult = 86400*365;
							ನಿಲ್ಲಿಸು;
						ಪೂರ್ವನಿಯೋಜಿತ:
							log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
							ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					token[ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(token)-1] = '\0';
					config->persistent_client_expiration = atoi(token)*expiration_mult;
					ಆದರೆ(config->persistent_client_expiration <= 0){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "pid_file")){
					ಆದರೆ(reload){
						ಮುಂದುವರಿ;        /* pid file not valid for reloading. */
					}
					ಆದರೆ(conf__parse_string(&token, "pid_file", &config->pid_file, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "port")){
					OPTION_DEPRECATED(token, "Please use 'listener' instead.");
					config->local_only = false;
					ಆದರೆ(reload){
						ಮುಂದುವರಿ;        /* Listeners not valid for reloading. */

					}
					ಆದರೆ(config__create_default_listener(config, token)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					ಆದರೆ(config->default_listener->ಪೋರ್ಟ್){
						log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Default listener port specified multiple times. Only the latest will be used.");
					}
					ಆದರೆ(conf__parse_int(&token, "port", &tmp_int, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(tmp_int < 1 || tmp_int > UINT16_MAX){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					config->default_listener->ಪೋರ್ಟ್ = (uint16_t)tmp_int;
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "protocol")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(ಖಾಲಿ, " ", &saveptr);
					REQUIRE_NON_ಖಾಲಿಗಳು_OPTION(token, "protocol");

					ಆದರೆ(!strcmp(token, "mqtt")){
						cur_listener->protocol = mp_mqtt;
						/*
						}else if(!strcmp(token, "mqttsn")){
						    cur_listener->protocol = mp_mqttsn;
						*/
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "websockets")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_WEBSOCKETS
						cur_listener->protocol = mp_websockets;
#ಇಲ್ಲದಿದ್ದರೆ
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Websockets support not available.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
#ಆದರೆ_ಅಂತ್ಯ
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "http_api")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_HTTP_API
						cur_listener->protocol = mp_http_api;
#ಇಲ್ಲದಿದ್ದರೆ
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: HTTP API support not available.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
#ಆದರೆ_ಅಂತ್ಯ
					}ಇಲ್ಲದಿದ್ದರೆ{
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid 'protocol' value (%s).", token);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "proxy_protocol_v2_require_tls")){
#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
#ಆದರೆ_ಅಂತ್ಯ
					REQUIRE_LISTENER(token);
					ಆದರೆ(conf__parse_bool(&token, "proxy_protocol_v2_require_tls", &cur_listener->proxy_protocol_v2_require_tls, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "psk_file")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FINAL_WITH_TLS_PSK
					REQUIRE_LISTENER_IF_ಪ್ರತಿ_LISTENER(token);
					conf_ಗಣ_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ಆದರೆ(cur_listener && cur_listener->certfile){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_security_options->psk_file);
					ಆದರೆ(conf__parse_string(&token, "psk_file", &cur_security_options->psk_file, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "psk_hint")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FINAL_WITH_TLS_PSK
					ಆದರೆ(reload){
						ಮುಂದುವರಿ;        /* PSK file not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ಆದರೆ(conf__parse_string(&token, "psk_hint", &cur_listener->psk_hint, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "queue_qos0_messages")){
					ಆದರೆ(conf__parse_bool(&token, token, &config->ಸರತಿ_qos0_messages, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "require_certificate")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ಆದರೆ(conf__parse_bool(&token, "require_certificate", &cur_listener->require_certificate, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "restart_timeout")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					cur_bridge->backoff_cap = 0; /* set backoff to constant mode, unless cap is specified further down */
					token = strtok_r(ಖಾಲಿ, " ", &saveptr);
					REQUIRE_NON_ಖಾಲಿಗಳು_OPTION(token, "restart_timeout");

					cur_bridge->restart_timeout = atoi(token);
					cur_bridge->backoff_ತಳ = 0;
					cur_bridge->backoff_cap = 0;
					ಆದರೆ(cur_bridge->restart_timeout < 1){
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "restart_timeout interval too low, using 1 second.");
						cur_bridge->restart_timeout = 1;
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cur_bridge->restart_timeout > 3600){
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "restart_timeout interval too high, using 3600 seconds.");
						cur_bridge->restart_timeout = 3600;
					}
					token = strtok_r(ಖಾಲಿ, " ", &saveptr);
					ಆದರೆ(token){
						cur_bridge->backoff_ತಳ = cur_bridge->restart_timeout;
						cur_bridge->backoff_cap = atoi(token);
						ಆದರೆ(cur_bridge->backoff_cap < cur_bridge->backoff_ತಳ){
							log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: backoff cap is lower than the base in restart_timeout.");
							ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
						}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cur_bridge->backoff_cap > 7200){
							log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: backoff cap too high, using 7200 seconds.");
							cur_bridge->backoff_cap = 7200;
						}

						token = strtok_r(ಖಾಲಿ, " ", &saveptr);
						ಆದರೆ(token){
							cur_bridge->stable_connection_period = atoi(token);
							ಆದರೆ(cur_bridge->stable_connection_period < 0){
								log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: stable connection period cannot be negative.");
								ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
							}
						}
					}
					cur_bridge->restart_timeout *= 1000; /* backoff is tracked in ms */
					cur_bridge->backoff_ತಳ *= 1000;
					cur_bridge->backoff_cap *= 1000;
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "retain_available")){
					ಆದರೆ(conf__parse_bool(&token, token, &config->retain_ಲಭ್ಯ, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "retain_expiry_interval")){
					ಆದರೆ(conf__parse_int(&token, token, &config->retain_expiry_interval, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(config->retain_expiry_interval < 1){
						log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Error: retain_expiry_interval must be >= 1.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(config->retain_expiry_interval > 10000000){
						log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: retain_expiry_interval being capped at 19 years.");
						config->retain_expiry_interval = 10000000;
					}
					config->retain_expiry_interval *= 60;
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "retry_interval")){
					OPTION_UNAVAILABLE(token);
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "round_robin")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_bool(&token, "round_robin", &cur_bridge->ಸುತ್ತು_robin, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "set_tcp_nodelay")){
					ಆದರೆ(conf__parse_bool(&token, "set_tcp_nodelay", &config->ಗಣ_tcp_nodelay, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "start_type")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(ಖಾಲಿ, " ", &saveptr);
					REQUIRE_NON_ಖಾಲಿಗಳು_OPTION(token, "start_type");

					ಆದರೆ(!strcmp(token, "automatic")){
						cur_bridge->ಆರಂಭ_type = bst_automatic;
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "lazy")){
						cur_bridge->ಆರಂಭ_type = bst_lazy;
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "manual")){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Manual start_type not supported.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "once")){
						cur_bridge->ಆರಂಭ_type = bst_once;
					}ಇಲ್ಲದಿದ್ದರೆ{
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid 'start_type' value in configuration (%s).", token);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "socket_domain")){
					ಆದರೆ(reload){
						ಮುಂದುವರಿ;        /* socket_domain not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(ಖಾಲಿ, " ", &saveptr);
					REQUIRE_NON_ಖಾಲಿಗಳು_OPTION(token, "start_type");

					ಆದರೆ(!strcmp(token, "ipv4")){
						cur_listener->socket_domain = AF_INET;
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "ipv6")){
						cur_listener->socket_domain = AF_INET6;
					}ಇಲ್ಲದಿದ್ದರೆ{
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid 'socket_domain' value '%s' in configuration.", token);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "sys_interval")){
					ಆದರೆ(conf__parse_int(&token, "sys_interval", &config->sys_interval, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(config->sys_interval < 0 || config->sys_interval > 65535){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid 'sys_interval' value (%d).", config->sys_interval);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "threshold")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_int(&token, "threshold", &cur_bridge->ಮಿತಿ, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(cur_bridge->ಮಿತಿ < 1){
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "threshold too low, using 1 message.");
						cur_bridge->ಮಿತಿ = 1;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "tls_engine")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
					ಆದರೆ(reload){
						ಮುಂದುವರಿ;        /* tls_engine not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ಆದರೆ(conf__parse_string(&token, "tls_engine", &cur_listener->tls_engine, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "tls_engine_kpass_sha1")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
					ಆದರೆ(reload){
						ಮುಂದುವರಿ;        /* tls_engine not valid for reloading. */

					}
					ಅಕ್ಷರ *kpass_sha = ಖಾಲಿ, *kpass_sha_bin = ಖಾಲಿ;
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ಆದರೆ(conf__parse_string(&token, "tls_engine_kpass_sha1", &kpass_sha, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(mosquitto__hex2bin_sha1(kpass_sha, (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ **)&kpass_sha_bin) != MOSQ_ERR_SUCCESS){
						mosquitto_FREE(kpass_sha);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					mosquitto_free(cur_listener->tls_engine_kpass_sha1);
					cur_listener->tls_engine_kpass_sha1 = kpass_sha_bin;
					mosquitto_FREE(kpass_sha);
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "tls_keyform")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
					ಆದರೆ(reload){
						ಮುಂದುವರಿ;        /* tls_engine not valid for reloading. */

					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ಅಕ್ಷರ *keyform = ಖಾಲಿ;
					ಆದರೆ(conf__parse_string(&token, "tls_keyform", &keyform, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					cur_listener->tls_keyform = mosq_k_pem;
					ಆದರೆ(!strcmp(keyform, "engine")){
						cur_listener->tls_keyform = mosq_k_engine;
					}
					mosquitto_FREE(keyform);
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "tls_version")){
#ಆದರೆ defined(WITH_TLS)
					ಆದರೆ(reload){
						ಮುಂದುವರಿ;        /* tls_version not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					ಆದರೆ(conf__parse_string(&token, "tls_version", &cur_listener->tls_ಆವೃತ್ತಿ, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "topic")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
					ಅಕ್ಷರ *topic = ಖಾಲಿ;
					ಎಣಿಕೆ mosquitto__bridge_direction direction = bd_ಹೊರಹೋಗು;
					uint8_t qos = 0;
					ಅಕ್ಷರ *local_ಪೂರ್ವಪ್ರತ್ಯಯ = ಖಾಲಿ, *remote_ಪೂರ್ವಪ್ರತ್ಯಯ = ಖಾಲಿ;

					REQUIRE_BRIDGE(token);

					token = strtok_r(ಖಾಲಿ, " ", &saveptr);
					REQUIRE_NON_ಖಾಲಿಗಳು_OPTION(token, "topic");

					// Check if the topic is quoted (e.g. for spaces within topic names), but not the
					// special case of ""
					ಆದರೆ(token[0] == '"' && token [1] != '"'){
						ಆದರೆ(strchr(saveptr, '"') == ಖಾಲಿ){
							log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Missing closing quote in topic value (%s).", saveptr);
							ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
						}

						ಅಕ್ಷರ *topic_ಒಳ_quotes = strtok_r(ಖಾಲಿ, "\"", &saveptr);
						size_t tlen = 1;
						ಆದರೆ(topic_ಒಳ_quotes){
							tlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(topic_ಒಳ_quotes);
						}

						topic = mosquitto_malloc(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(token) + tlen + 1);
						ಆದರೆ(!topic){
							log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
							ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
						}

						strcpy(topic, token + 1);
						strcat(topic, " ");
						ಆದರೆ(topic_ಒಳ_quotes){
							strcat(topic, topic_ಒಳ_quotes);
						}
					}ಇಲ್ಲದಿದ್ದರೆ{
						topic = mosquitto_strdup(token);
						ಆದರೆ(!topic){
							log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
							ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
						}
					}

					token = strtok_r(ಖಾಲಿ, " ", &saveptr);
					ಆದರೆ(token){
						ಆದರೆ(!strcasecmp(token, "out")){
							direction = bd_ಹೊರಹೋಗು;
						}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcasecmp(token, "in")){
							direction = bd_ಒಳ;
						}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcasecmp(token, "both")){
							direction = bd_both;
						}ಇಲ್ಲದಿದ್ದರೆ{
							log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid bridge topic direction '%s'.", token);
							mosquitto_FREE(topic);
							ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
						}
						token = strtok_r(ಖಾಲಿ, " ", &saveptr);
						ಆದರೆ(token){
							ಆದರೆ(token[0] == '#'){
								(ಶೂನ್ಯ)strtok_r(ಖಾಲಿ, "", &saveptr);
							}
							qos = (uint8_t)atoi(token);
							ಆದರೆ(qos > 2){
								log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid bridge QoS level '%s'.", token);
								mosquitto_FREE(topic);
								ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
							}

							token = strtok_r(ಖಾಲಿ, " ", &saveptr);
							ಆದರೆ(token){
								ಆದರೆ(!strcmp(token, "\"\"") || token[0] == '#'){
									local_ಪೂರ್ವಪ್ರತ್ಯಯ = ಖಾಲಿ;
									ಆದರೆ(token[0] == '#'){
										(ಶೂನ್ಯ)strtok_r(ಖಾಲಿ, "", &saveptr);
									}
								}ಇಲ್ಲದಿದ್ದರೆ{
									local_ಪೂರ್ವಪ್ರತ್ಯಯ = token;
								}

								token = strtok_r(ಖಾಲಿ, " ", &saveptr);
								ಆದರೆ(token){
									ಆದರೆ(!strcmp(token, "\"\"") || token[0] == '#'){
										remote_ಪೂರ್ವಪ್ರತ್ಯಯ = ಖಾಲಿ;
									}ಇಲ್ಲದಿದ್ದರೆ{
										remote_ಪೂರ್ವಪ್ರತ್ಯಯ = token;
									}
								}
							}
						}
					}

					ಆದರೆ(bridge_ಕೂಡಿಸು_topic(cur_bridge, topic, direction, qos, local_ಪೂರ್ವಪ್ರತ್ಯಯ, remote_ಪೂರ್ವಪ್ರತ್ಯಯ)){
						mosquitto_FREE(topic);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(topic);
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "max_topic_alias")){
					REQUIRE_LISTENER(token);
					ಆದರೆ(conf__parse_int(&token, "max_topic_alias", &tmp_int, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias' value in configuration.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias = (uint16_t)tmp_int;
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "max_topic_alias_broker")){
					REQUIRE_LISTENER(token);
					ಆದರೆ(conf__parse_int(&token, "max_topic_alias_broker", &tmp_int, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias_broker' value in configuration.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias_broker = (uint16_t)tmp_int;
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "try_private")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_bool(&token, "try_private", &cur_bridge->try_ಖಾಸಗಿ, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "upgrade_outgoing_qos")){
					ಆದರೆ(conf__parse_bool(&token, token, &config->upgrade_outgoing_qos, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "use_identity_as_username")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ಆದರೆ(conf__parse_bool(&token, "use_identity_as_username", &cur_listener->use_ತಟಸ್ಥ_as_username, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "use_subject_as_username")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ಆದರೆ(conf__parse_bool(&token, "use_subject_as_username", &cur_listener->use_subject_as_username, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "user")){
					ಆದರೆ(reload){
						ಮುಂದುವರಿ;        /* Drop privileges user not valid for reloading. */
					}
					mosquitto_FREE(config->user);
					ಆದರೆ(conf__parse_string(&token, "user", &config->user, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "use_username_as_clientid")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ಆದರೆ(conf__parse_bool(&token, "use_username_as_clientid", &cur_listener->use_username_as_clientid, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "username") || !strcmp(token, "remote_username")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ಆದರೆ(conf__parse_string(&token, "bridge remote_username", &cur_bridge->remote_username, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "websockets_log_level")){
#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					ಆದರೆ(conf__parse_int(&token, "websockets_log_level", &config->websockets_log_level, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "websockets_headers_size") || !strcmp(token, "packet_buffer_size")){
					ಆದರೆ(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ಆದರೆ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Error: Packet buffer size must be between 0 and 65535 inclusive.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					config->packet_ಬಫರ್_ಗಾತ್ರ = (uint16_t)tmp_int;
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(token, "websockets_origin")){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_WEBSOCKETS
#  ಆದರೆ LWS_LIBRARY_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN
					REQUIRE_LISTENER(token);
					ws_origins = mosquitto_realloc(cur_listener->ws_origins, ಗಾತ್ರ(ಅಕ್ಷರ *)*(size_t)(cur_listener->ws_origin_ಎಣಿಕೆ+1));
					ಆದರೆ(ws_origins == ಖಾಲಿ){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
					}
					ws_origins[cur_listener->ws_origin_ಎಣಿಕೆ] = ಖಾಲಿ;
					cur_listener->ws_origins = ws_origins;
					ಆದರೆ(conf__parse_string(&token, "websockets_origin", &ws_origins[cur_listener->ws_origin_ಎಣಿಕೆ], &saveptr)){
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					cur_listener->ws_origin_ಎಣಿಕೆ++;
#  ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: websockets_origin support not available, libwebsockets version is too old.");
#  endif
#ಇಲ್ಲದಿದ್ದರೆ
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Websockets support not available.");
#ಆದರೆ_ಅಂತ್ಯ
				}ಇಲ್ಲದಿದ್ದರೆ{
					log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unknown configuration variable '%s'.", token);
					ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
				}
			}
		}
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ config__read_file(ರಚನೆ mosquitto__config *config, bool reload, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file, ರಚನೆ config_recurse *cr, ಪೂರ್ಣಾಂಕ level, ಪೂರ್ಣಾಂಕ *lineno)
{
	ಪೂರ್ಣಾಂಕ rc;
	FILE *fptr = ಖಾಲಿ;
	ಅಕ್ಷರ *buf;
	ಪೂರ್ಣಾಂಕ buflen;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
	DIR *dir;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
	dir = opendir(file);
	ಆದರೆ(dir){
		closedir(dir);
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Config file '%s' is a directory.", file);
		ಹಿಂತಿರುಗಿ 1;
	}
#ಆದರೆ_ಅಂತ್ಯ

	fptr = mosquitto_fopen(file, "rt", false);
	ಆದರೆ(!fptr){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to open config file '%s'.", file);
		ಹಿಂತಿರುಗಿ 1;
	}

	buflen = 1000;
	buf = mosquitto_malloc((size_t)buflen);
	ಆದರೆ(!buf){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
		fclose(fptr);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}

	rc = config__read_file_core(config, reload, cr, level, lineno, fptr, &buf, &buflen);
	mosquitto_FREE(buf);
	fclose(fptr);

	ಹಿಂತಿರುಗಿ rc;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ config__check_proxy(ರಚನೆ mosquitto__config *config)
{
	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<config->listener_ಎಣಿಕೆ; i++){
		ರಚನೆ mosquitto__listener *l = &config->listeners[i];

		ಆದರೆ(l->enable_proxy_protocol == 2){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
			ಆದರೆ(l->use_subject_as_username){
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: use_subject_as_username cannot be used with `enable_proxy_protocol 2`.");
				ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
			}

			ಆದರೆ(l->certfile || l->keyfile){
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: certfile and keyfile cannot be used with `enable_proxy_protocol 2`.");
				ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
			}
#ಆದರೆ_ಅಂತ್ಯ
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ config__check(ರಚನೆ mosquitto__config *config)
{
	/* Checks that are easy to make after the config has been loaded. */

	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *id_ಪೂರ್ವಪ್ರತ್ಯಯ;
	ಪೂರ್ಣಾಂಕ id_ಪೂರ್ವಪ್ರತ್ಯಯ_len;
	ಆದರೆ(config->security_options.auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ){
		id_ಪೂರ್ವಪ್ರತ್ಯಯ = config->security_options.auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ;
		id_ಪೂರ್ವಪ್ರತ್ಯಯ_len = config->security_options.auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ_len;
	}ಇಲ್ಲದಿದ್ದರೆ{
		id_ಪೂರ್ವಪ್ರತ್ಯಯ = "auto-";
		id_ಪೂರ್ವಪ್ರತ್ಯಯ_len = (ಪೂರ್ಣಾಂಕ)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("auto-");
	}

	/* Default to auto_id_prefix = 'auto-' if none set. */
	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<config->listener_ಎಣಿಕೆ; i++){
		ಆದರೆ(!config->listeners[i].security_options->auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ){
			config->listeners[i].security_options->auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ = mosquitto_strdup(id_ಪೂರ್ವಪ್ರತ್ಯಯ);
			ಆದರೆ(!config->listeners[i].security_options->auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ){
				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
			}
			config->listeners[i].security_options->auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ_len = (uint16_t)id_ಪೂರ್ವಪ್ರತ್ಯಯ_len;
		}
	}

	ಹಿಂತಿರುಗಿ config__check_proxy(config);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ config__check_bridges(ರಚನೆ mosquitto__config *config)
{
	ರಚನೆ mosquitto__bridge *bridge1, *bridge2;
	ಅಕ್ಷರ hostname[256];
	size_t len;

	/* Check for bridge duplicate local_clientid, need to generate missing IDs
	 * first. */
	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<config->bridge_ಎಣಿಕೆ; i++){
		bridge1 = config->bridges[i];

		ಆದರೆ(!bridge1->remote_clientid){
			ಆದರೆ(!gethostname(hostname, 256)){
				len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(hostname) + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(bridge1->name) + 2;
				bridge1->remote_clientid = mosquitto_malloc(len);
				ಆದರೆ(!bridge1->remote_clientid){
					ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
				}
				snprintf(bridge1->remote_clientid, len, "%s.%s", hostname, bridge1->name);
			}ಇಲ್ಲದಿದ್ದರೆ{
				ಹಿಂತಿರುಗಿ 1;
			}
		}

		ಆದರೆ(!bridge1->local_clientid){
			len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(bridge1->remote_clientid) + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("local.") + 2;
			bridge1->local_clientid = mosquitto_malloc(len);
			ಆದರೆ(!bridge1->local_clientid){
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
			}
			snprintf(bridge1->local_clientid, len, "local.%s", bridge1->remote_clientid);
		}
	}

	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<config->bridge_ಎಣಿಕೆ; i++){
		bridge1 = config->bridges[i];
		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ j=i+1; j<config->bridge_ಎಣಿಕೆ; j++){
			bridge2 = config->bridges[j];
			ಆದರೆ(!strcmp(bridge1->local_clientid, bridge2->local_clientid)){
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Bridge local_clientid "
						"'%s' is not unique. Try changing or setting the "
						"local_clientid value for one of the bridges.",
						bridge1->local_clientid);
				ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
			}
		}
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
	/* Check for missing TLS cafile/capath/certfile/keyfile */
	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<config->listener_ಎಣಿಕೆ; i++){
		bool cafile = !!config->listeners[i].cafile;
		bool capath = !!config->listeners[i].capath;
		bool certfile = !!config->listeners[i].certfile;
		bool keyfile = !!config->listeners[i].keyfile;

		ಆದರೆ((certfile && !keyfile) || (!certfile && keyfile)){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Both certfile and keyfile must be provided to enable a TLS listener.");
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
		ಆದರೆ(cafile && !certfile){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: cafile specified without certfile and keyfile.");
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
		ಆದರೆ(capath && !certfile){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: capath specified without certfile and keyfile.");
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}
#ಆದರೆ_ಅಂತ್ಯ


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ conf__parse_bool(ಅಕ್ಷರ **token, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, bool *ಮೌಲ್ಯ, ಅಕ್ಷರ **saveptr)
{
	*token = strtok_r(ಖಾಲಿ, " ", saveptr);
	ಆದರೆ(*token){
		ಆದರೆ(!strcmp(*token, "false") || !strcmp(*token, "0")){
			*ಮೌಲ್ಯ = false;
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(*token, "true") || !strcmp(*token, "1")){
			*ಮೌಲ್ಯ = true;
		}ಇಲ್ಲದಿದ್ದರೆ{
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Invalid '%s' value (%s).", name, *token);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ conf__parse_int(ಅಕ್ಷರ **token, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ *ಮೌಲ್ಯ, ಅಕ್ಷರ **saveptr)
{
	*token = strtok_r(ಖಾಲಿ, " ", saveptr);
	ಆದರೆ(*token){
		ಅಕ್ಷರ *endptr = ಖಾಲಿ;
		ದೀರ್ಘ l = strtol(*token, &endptr, 0);
		ಆದರೆ(endptr == *token || endptr[0] != '\0'){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: '%s' value not a number.", name);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
		ಆದರೆ(l > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ || l < INT_MIN){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: '%s' value out of range.", name);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
		*ಮೌಲ್ಯ = (ಪೂರ್ಣಾಂಕ)l;
	}ಇಲ್ಲದಿದ್ದರೆ{
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ conf__parse_ssize_t(ಅಕ್ಷರ **token, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ssize_t *ಮೌಲ್ಯ, ಅಕ್ಷರ **saveptr)
{
	*token = strtok_r(ಖಾಲಿ, " ", saveptr);
	ಆದರೆ(*token){
		*ಮೌಲ್ಯ = atol(*token);
	}ಇಲ್ಲದಿದ್ದರೆ{
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ conf__parse_string(ಅಕ್ಷರ **token, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಅಕ್ಷರ **ಮೌಲ್ಯ, ಅಕ್ಷರ **saveptr)
{
	size_t tlen;

	*token = strtok_r(ಖಾಲಿ, "", saveptr);
	ಆದರೆ(*token){
		ಆದರೆ(*ಮೌಲ್ಯ){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Duplicate '%s' value in configuration.", name);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
		/* Deal with multiple spaces at the beginning of the string. */
		*token = mosquitto_trimblanks(*token);
		ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(*token) == 0){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}

		tlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(*token);
		ಆದರೆ(tlen > UINT16_MAX){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
		ಆದರೆ(mosquitto_validate_utf8(*token, (uint16_t)tlen)){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Malformed UTF-8 in configuration.");
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
		*ಮೌಲ್ಯ = mosquitto_strdup(*token);
		ಆದರೆ(!*ಮೌಲ್ಯ){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}
