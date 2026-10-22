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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <limits.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
#ਨਹੀਂਤਾਂ
#  include <dirent.h>
#  include <strings.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
#  include <netdb.h>
#  include <sys/socket.h>
#ਨਹੀਂਤਾਂ
#  include <winsock2.h>
#  include <ws2tcpip.h>
#ਜੇ_ਅੰਤ

#ਜੇ !defined(WIN32) && !defined(__CYGWIN__)
#  include <syslog.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "tls_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "utlist.h"

ਬਣਤਰ config_recurse {
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ log_dest;
	ਪੂਰਨ log_dest_ਸੈੱਟ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ log_type;
	ਪੂਰਨ log_type_ਸੈੱਟ;
};

#ਜੇ defined(WIN32) || defined(__CYGWIN__)
#ਅਨ੍ਤਰ੍ਭਾਵ <windows.h>
ਬਾਹਰੀ SERVICE_ਸਥਿਤੀ_HANDLE service_handle;
#ਜੇ_ਅੰਤ


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REQUIRE_LISTENER(A) \
		ਕਰੋ{ \
			ਜੇ(cur_listener == ਨਿਰਾ){ \
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				ਵਾਪਸ MOSQ_ERR_INVAL; \
			} \
		}ਜਦੋਂ(0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REQUIRE_LISTENER_OR_DEFAULT_LISTENER(A) \
		ਕਰੋ{ \
			ਜੇ(cur_listener == ਨਿਰਾ){ \
				ਜੇ(config__create_default_listener(config, (A))){ \
					ਵਾਪਸ MOSQ_ERR_NOMEM; \
				} \
				cur_listener = config->default_listener; \
			} \
		}ਜਦੋਂ(0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REQUIRE_LISTENER_IF_ਪ੍ਰਤੀ_LISTENER(A) \
		ਕਰੋ{ \
			ਜੇ(config->ਪ੍ਰਤੀ_listener_settings == true && cur_listener == ਨਿਰਾ){ \
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				ਵਾਪਸ MOSQ_ERR_INVAL; \
			} \
		}ਜਦੋਂ(0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REQUIRE_NON_DEFAULT_LISTENER(A) \
		ਕਰੋ{ \
			ਜੇ(cur_listener == config->default_listener || cur_listener == ਨਿਰਾ){ \
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				ਵਾਪਸ MOSQ_ERR_INVAL; \
			} \
		}ਜਦੋਂ(0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REQUIRE_BRIDGE(A) \
		ਕਰੋ{ \
			ਜੇ(cur_bridge == ਨਿਰਾ){ \
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: The '%s' option requires a bridge to be defined first.", (A)); \
				ਵਾਪਸ MOSQ_ERR_INVAL; \
			} \
		}ਜਦੋਂ(0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REQUIRE_PLUGIN(A) \
		ਕਰੋ{ \
			ਜੇ(cur_plugin == ਨਿਰਾ){ \
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: The '%s' option requires plugin/global_plugin/plugin_load to be defined first.", (A)); \
				ਵਾਪਸ MOSQ_ERR_INVAL; \
			} \
		}ਜਦੋਂ(0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPTION_DEPRECATED(A, B) \
		log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "The '%s' option is now deprecated and will be removed in version 3.0. %s", (A), (B))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPTION_UNAVAILABLE(A) \
		log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: The '%s' option is no longer available.", (A));

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REQUIRE_NON_ਖਾਲੀ_ਥਾਵਾਂ_OPTION(A, B) \
		ਕਰੋ{ \
			ਜੇ(!(A)){ \
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", (B)); \
				ਵਾਪਸ MOSQ_ERR_INVAL; \
			} \
		}ਜਦੋਂ(0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪ੍ਰਤੀ_LISTENER_ALTERNATIVE(A, B) \
		ਜੇ(config->ਪ੍ਰਤੀ_listener_settings){ \
			log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "You are using the '%s' option with 'per_listener_settings true'. Please replace this with '%s'.", A, B); \
		} \


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FINAL_WITH_TLS_PSK
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A) \
		ਕਰੋ{ \
			ਜੇ(cur_bridge->tls_psk_ਪਛਾਣ_ਤੱਤ || cur_bridge->tls_psk){ \
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and psk encryption in a single bridge.", (A)); \
				ਵਾਪਸ MOSQ_ERR_INVAL; \
			} \
		}ਜਦੋਂ(0)

#  define REQUIRE_BRIDGE_NO_X509(A) \
		ਕਰੋ{ \
			ਜੇ(cur_bridge->tls_cafile || cur_bridge->tls_capath || cur_bridge->tls_certfile || cur_bridge->tls_keyfile){ \
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and identity encryption in a single bridge.", (A)); \
				ਵਾਪਸ MOSQ_ERR_INVAL; \
			} \
		}ਜਦੋਂ(0)

#ਨਹੀਂਤਾਂ
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A)
#  define REQUIRE_BRIDGE_NO_X509(A)
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਬਣਤਰ mosquitto__security_options *cur_security_options = ਨਿਰਾ;

ਸਥਿਰ ਪੂਰਨ conf__parse_bool(ਅੱਖਰ **token, ਅਚਲ ਅੱਖਰ *name, bool *ਮੁੱਲ, ਅੱਖਰ **saveptr);
ਸਥਿਰ ਪੂਰਨ conf__parse_int(ਅੱਖਰ **token, ਅਚਲ ਅੱਖਰ *name, ਪੂਰਨ *ਮੁੱਲ, ਅੱਖਰ **saveptr);
ਸਥਿਰ ਪੂਰਨ conf__parse_ssize_t(ਅੱਖਰ **token, ਅਚਲ ਅੱਖਰ *name, ssize_t *ਮੁੱਲ, ਅੱਖਰ **saveptr);
ਸਥਿਰ ਪੂਰਨ conf__parse_string(ਅੱਖਰ **token, ਅਚਲ ਅੱਖਰ *name, ਅੱਖਰ **ਮੁੱਲ, ਅੱਖਰ **saveptr);
ਸਥਿਰ ਪੂਰਨ config__read_file(ਬਣਤਰ mosquitto__config *config, bool reload, ਅਚਲ ਅੱਖਰ *file, ਬਣਤਰ config_recurse *config_tmp, ਪੂਰਨ level, ਪੂਰਨ *lineno);
ਸਥਿਰ ਪੂਰਨ config__check(ਬਣਤਰ mosquitto__config *config);
ਸਥਿਰ ਖਾਲੀ config__cleanup_plugins(ਖਾਲੀ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
ਸਥਿਰ ਪੂਰਨ config__check_bridges(ਬਣਤਰ mosquitto__config *config);
#ਜੇ_ਅੰਤ


ਸਥਿਰ ਪੂਰਨ config_ਜੋੜੋ_listener(ਬਣਤਰ mosquitto__config *config)
{
	ਬਣਤਰ mosquitto__listener *listener;
	ਬਣਤਰ mosquitto__listener *new_listeners;
	ਪੂਰਨ def_listener = -1;

	ਜੇ(config->default_listener){
		ਲਈ(ਪੂਰਨ i=0; i<config->listener_ਗਿਣਤੀ; i++){
			ਜੇ(&config->listeners[i] == config->default_listener){
				def_listener = i;
				ਰੋਕੋ;
			}
		}
	}
	new_listeners = mosquitto_realloc(config->listeners, ਆਕਾਰ(ਬਣਤਰ mosquitto__listener)*(size_t)(config->listener_ਗਿਣਤੀ+1));
	ਜੇ(!new_listeners){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}
	config->listeners = new_listeners;
	listener = &config->listeners[config->listener_ਗਿਣਤੀ];
	memset(listener, 0, ਆਕਾਰ(ਬਣਤਰ mosquitto__listener));
	listener->security_options = mosquitto_calloc(1, ਆਕਾਰ(ਬਣਤਰ mosquitto__security_options));
	ਜੇ(!listener->security_options){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}
	ਜੇ(def_listener != -1){
		config->default_listener = &config->listeners[def_listener];
	}
	config->listener_ਗਿਣਤੀ++;

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ config__create_default_listener(ਬਣਤਰ mosquitto__config *config, ਅਚਲ ਅੱਖਰ *option_name)
{
	ਜੇ(config->default_listener){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}
	log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Creating default listener due to '%s' option.", option_name);
	log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "It is best practice to define a 'listener' first. Using the '%s' option without a listener will be disabled in the future.", option_name);
	ਜੇ(config_ਜੋੜੋ_listener(config)){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	config->default_listener = &config->listeners[config->listener_ਗਿਣਤੀ-1];
	listener_ਸੈੱਟ_defaults(config->default_listener);
	config->default_listener->ਪੋਰਟ = 1883;

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਖਾਲੀ conf_ਸੈੱਟ_cur_security_options(ਬਣਤਰ mosquitto__config *config, ਬਣਤਰ mosquitto__listener **cur_listener, ਬਣਤਰ mosquitto__security_options **security_options, ਅਚਲ ਅੱਖਰ *option_name)
{
	ਜੇ(config->ਪ੍ਰਤੀ_listener_settings){
		ਜੇ(*cur_listener == ਨਿਰਾ){
			ਜੇ(config__create_default_listener(config, option_name)){
				ਵਾਪਸ;
			}
			*cur_listener = config->default_listener;
		}
		(*security_options) = (*cur_listener)->security_options;
	}ਨਹੀਂਤਾਂ{
		(*security_options) = &config->security_options;
	}
}


ਸਥਿਰ ਪੂਰਨ conf__attempt_resolve(ਅਚਲ ਅੱਖਰ *host, ਅਚਲ ਅੱਖਰ *ਪਾਠ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਲਘੂਗਣਕ, ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ)
{
	ਬਣਤਰ addrinfo gai_hints;
	ਬਣਤਰ addrinfo *gai_res;
	ਪੂਰਨ rc;

	memset(&gai_hints, 0, ਆਕਾਰ(ਬਣਤਰ addrinfo));
	gai_hints.ai_family = AF_UNSPEC;
	gai_hints.ai_socktype = SOCK_STREAM;
	gai_res = ਨਿਰਾ;
	rc = getaddrinfo(host, ਨਿਰਾ, &gai_hints, &gai_res);
	ਜੇ(gai_res){
		freeaddrinfo(gai_res);
	}
	ਜੇ(rc != 0){
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
		ਜੇ(rc == EAI_SYSTEM){
			ਜੇ(errno == ENOENT){
				log__printf(ਨਿਰਾ, ਲਘੂਗਣਕ, "%s: Unable to resolve %s %s.", ਸੁਨੇਹਾ, ਪਾਠ, host);
			}ਨਹੀਂਤਾਂ{
				log__printf(ਨਿਰਾ, ਲਘੂਗਣਕ, "%s: Error resolving %s: %s.", ਸੁਨੇਹਾ, ਪਾਠ, strerror(errno));
			}
		}ਨਹੀਂਤਾਂ{
			log__printf(ਨਿਰਾ, ਲਘੂਗਣਕ, "%s: Error resolving %s: %s.", ਸੁਨੇਹਾ, ਪਾਠ, gai_strerror(rc));
		}
#ਨਹੀਂਤਾਂ
		ਜੇ(rc == WSAHOST_NOT_ਮਿਲਿਆ){
			log__printf(ਨਿਰਾ, ਲਘੂਗਣਕ, "%s: Error resolving %s.", ਸੁਨੇਹਾ, ਪਾਠ);
		}
#ਜੇ_ਅੰਤ
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਖਾਲੀ config__init_reload(ਬਣਤਰ mosquitto__config *config)
{
	/* Set defaults */
	ਲਈ(ਪੂਰਨ i=0; i<config->listener_ਗਿਣਤੀ; i++){
		listener_ਸੈੱਟ_defaults(&config->listeners[i]);
	}

	config->local_only = true;
	config->allow_duplicate_messages = true;

	mosquitto_FREE(config->security_options.acl_ਅੰਕੜੇ.acl_file);
	mosquitto_FREE(config->security_options.password_ਅੰਕੜੇ.password_file);
	mosquitto_FREE(config->security_options.psk_file);

	config->security_options.allow_anonymous = -1;
	config->security_options.allow_zero_ਲੰਮਾਈ_clientid = true;
	config->security_options.auto_id_ਅਗੇਤਰ = ਨਿਰਾ;
	config->security_options.auto_id_ਅਗੇਤਰ_len = 0;

	config->autosave_interval = 1800;
	config->autosave_on_changes = false;

	mosquitto_FREE(config->clientid_prefixes);

	config->connection_messages = true;
	config->clientid_prefixes = ਨਿਰਾ;
	config->ਪ੍ਰਤੀ_listener_settings = false;
	ਜੇ(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = ਨਿਰਾ;
	}
	mosquitto_FREE(config->log_file);

#ਜੇ defined(WIN32) || defined(__CYGWIN__)
	ਜੇ(service_handle){
		/* This is running as a Windows service. Default to no logging. Using
		 * stdout/stderr is forbidden because the first clients to connect will
		 * get log information sent to them for some reason. */
		config->log_dest = MQTT3_LOG_NONE;
	}ਨਹੀਂਤਾਂ{
		config->log_dest = MQTT3_LOG_STDERR;
	}
#ਨਹੀਂਤਾਂ
	config->log_facility = LOG_DAEMON;
	config->log_dest = MQTT3_LOG_STDERR | MQTT3_LOG_DLT;
	ਜੇ(db.quiet){
		config->log_type = 0;
	}ਨਹੀਂਤਾਂ ਜੇ(db.verbose){
		config->log_type = UINT_MAX;
	}ਨਹੀਂਤਾਂ{
		config->log_type = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;
	}
#ਜੇ_ਅੰਤ
	config->log_timestamp = true;
	mosquitto_FREE(config->log_timestamp_format);
	config->global_max_clients = -1;
	config->global_max_connections = -1;
	config->log_timestamp_format = ਨਿਰਾ;
	config->max_keepalive = 0;
	config->max_packet_ਮਾਪ = 2000000;
	config->max_inflight_messages = 20;
	config->max_queued_messages = 1000;
	config->max_inflight_ਬਾਈਟਾਂ = 0;
	config->max_queued_ਬਾਈਟਾਂ = 0;
	config->persistence = false;
	mosquitto_FREE(config->persistence_location);
	mosquitto_FREE(config->persistence_file);
	config->persistent_client_expiration = 0;
	config->ਕਤਾਰ_qos0_messages = false;
	config->retain_ਉਪਲਬਧ = true;
	config->retain_expiry_interval = 0;
	config->ਸੈੱਟ_tcp_nodelay = false;
	config->sys_interval = 10;
	config->upgrade_outgoing_qos = false;
	config->packet_ਬਫਰ_ਮਾਪ = 4096;

	config->packet_max_auth = 100000;
	config->packet_max_connect = 100000;
	config->packet_max_sub = 100000;
	config->packet_max_simple = 10000;
}


ਸਥਿਰ ਖਾਲੀ config__cleanup_plugin_config(mosquitto_plugin_id_t *plugin)
{
	mosquitto_FREE(plugin->config.ਰਸਤਾ);
	mosquitto_FREE(plugin->config.name);

	ਜੇ(plugin->config.options){
		ਲਈ(ਪੂਰਨ j=0; j<plugin->config.option_ਗਿਣਤੀ; j++){
			mosquitto_FREE(plugin->config.options[j].ਕੁੰਜੀ);
			mosquitto_FREE(plugin->config.options[j].ਮੁੱਲ);
		}
		mosquitto_FREE(plugin->config.options);
		plugin->config.option_ਗਿਣਤੀ = 0;
	}
	mosquitto_FREE(plugin->config.security_options);
	mosquitto_FREE(plugin);
}


ਸਥਿਰ ਖਾਲੀ config__cleanup_plugins(ਖਾਲੀ)
{
	ਲਈ(ਪੂਰਨ i=0; i<db.plugin_ਗਿਣਤੀ; i++){
		config__cleanup_plugin_config(db.plugins[i]);
	}
	mosquitto_FREE(db.plugins);
}


ਖਾਲੀ config__init(ਬਣਤਰ mosquitto__config *config)
{
	memset(config, 0, ਆਕਾਰ(ਬਣਤਰ mosquitto__config));
	config__init_reload(config);

	config->daemon = false;
}


ਖਾਲੀ config__cleanup(ਬਣਤਰ mosquitto__config *config)
{
	mosquitto_FREE(config->clientid_prefixes);
	mosquitto_FREE(config->persistence_location);
	mosquitto_FREE(config->persistence_file);
	mosquitto_FREE(config->persistence_filepath);
	mosquitto_FREE(config->security_options.auto_id_ਅਗੇਤਰ);
	mosquitto_FREE(config->security_options.acl_ਅੰਕੜੇ.acl_file);
	mosquitto_FREE(config->security_options.password_ਅੰਕੜੇ.password_file);
	mosquitto_FREE(config->security_options.psk_file);
	mosquitto_FREE(config->security_options.plugins);
	mosquitto_FREE(config->pid_file);
	mosquitto_FREE(config->user);
	mosquitto_FREE(config->log_timestamp_format);
	ਜੇ(config->listeners){
		ਲਈ(ਪੂਰਨ i=0; i<config->listener_ਗਿਣਤੀ; i++){
			mosquitto_FREE(config->listeners[i].host);
			mosquitto_FREE(config->listeners[i].bind_interface);
			mosquitto_FREE(config->listeners[i].mount_ਬਿੰਦੂ);
			mosquitto_FREE(config->listeners[i].socks);
			ਜੇ(config->listeners[i].security_options){
				mosquitto_FREE(config->listeners[i].security_options->auto_id_ਅਗੇਤਰ);
				mosquitto_FREE(config->listeners[i].security_options->acl_ਅੰਕੜੇ.acl_file);
				mosquitto_FREE(config->listeners[i].security_options->password_ਅੰਕੜੇ.password_file);
				mosquitto_FREE(config->listeners[i].security_options->psk_file);
				mosquitto_FREE(config->listeners[i].security_options->plugins);
				mosquitto_FREE(config->listeners[i].security_options);
			}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
			mosquitto_FREE(config->listeners[i].cafile);
			mosquitto_FREE(config->listeners[i].capath);
			mosquitto_FREE(config->listeners[i].certfile);
			mosquitto_FREE(config->listeners[i].keyfile);
			mosquitto_FREE(config->listeners[i].ciphers);
			mosquitto_FREE(config->listeners[i].ciphers_tls13);
			mosquitto_FREE(config->listeners[i].psk_hint);
			mosquitto_FREE(config->listeners[i].crlfile);
			mosquitto_FREE(config->listeners[i].tls_ਸੰਸਕਰਣ);
			mosquitto_FREE(config->listeners[i].tls_engine);
			mosquitto_FREE(config->listeners[i].tls_engine_kpass_sha1);
#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			ਜੇ(!config->listeners[i].ws_context) /* libwebsockets frees its own SSL_CTX */
#ਜੇ_ਅੰਤ
			{
				SSL_CTX_free(config->listeners[i].ssl_ctx);
				config->listeners[i].ssl_ctx = ਨਿਰਾ;
			}
#ਜੇ_ਅੰਤ
#ਜੇ defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
			mosquitto_FREE(config->listeners[i].http_dir);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_WEBSOCKETS
			ਲਈ(ਪੂਰਨ j=0; j<config->listeners[i].ws_origin_ਗਿਣਤੀ; j++){
				mosquitto_FREE(config->listeners[i].ws_origins[j]);
			}
			mosquitto_FREE(config->listeners[i].ws_origins);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_UNIX_SOCKETS
			mosquitto_FREE(config->listeners[i].unix_socket_ਰਸਤਾ);
#ਜੇ_ਅੰਤ
		}
		mosquitto_FREE(config->listeners);
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
	ਜੇ(config->bridges){
		ਲਈ(ਪੂਰਨ i=0; i<config->bridge_ਗਿਣਤੀ; i++){
			config__bridge_cleanup(config->bridges[i]);
		}
		mosquitto_FREE(config->bridges);
	}
#ਜੇ_ਅੰਤ
	config__cleanup_plugins();

	ਜੇ(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = ਨਿਰਾ;
	}
	ਜੇ(config->log_file){
		mosquitto_FREE(config->log_file);
		config->log_file = ਨਿਰਾ;
	}
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE


ਖਾਲੀ config__bridge_cleanup(ਬਣਤਰ mosquitto__bridge *bridge)
{
	ਜੇ(bridge == ਨਿਰਾ){
		ਵਾਪਸ;
	}

	mosquitto_FREE(bridge->name);
	ਜੇ(bridge->ਪਤੇ){
		ਲਈ(ਪੂਰਨ i=0; i<bridge->ਪਤਾ_ਗਿਣਤੀ; i++){
			mosquitto_FREE(bridge->ਪਤੇ[i].ਪਤਾ);
		}
		mosquitto_FREE(bridge->ਪਤੇ);
	}
	mosquitto_FREE(bridge->bind_ਪਤਾ);
	mosquitto_FREE(bridge->remote_clientid);
	mosquitto_FREE(bridge->remote_username);
	mosquitto_FREE(bridge->remote_password);
	mosquitto_FREE(bridge->local_clientid);
	mosquitto_FREE(bridge->local_username);
	mosquitto_FREE(bridge->local_password);
	ਜੇ(bridge->topics){
		ਬਣਤਰ mosquitto__bridge_topic *cur_topic, *topic_tmp;

		LL_FOREACH_ਸੁਰੱਖਿਅਤ(bridge->topics, cur_topic, topic_tmp){
			mosquitto_FREE(cur_topic->topic);
			mosquitto_FREE(cur_topic->local_ਅਗੇਤਰ);
			mosquitto_FREE(cur_topic->remote_ਅਗੇਤਰ);
			mosquitto_FREE(cur_topic->local_topic);
			mosquitto_FREE(cur_topic->remote_topic);
			LL_DELETE(bridge->topics, cur_topic);
			mosquitto_FREE(cur_topic);
		}
		mosquitto_FREE(bridge->topics);
	}
	mosquitto_FREE(bridge->notification_topic);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
	mosquitto_FREE(bridge->tls_certfile);
	mosquitto_FREE(bridge->tls_keyfile);
	mosquitto_FREE(bridge->tls_ਸੰਸਕਰਣ);
	mosquitto_FREE(bridge->tls_cafile);
	mosquitto_FREE(bridge->tls_capath);
	mosquitto_FREE(bridge->tls_alpn);
	mosquitto_FREE(bridge->tls_ciphers);
	mosquitto_FREE(bridge->tls_13_ciphers);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FINAL_WITH_TLS_PSK
	mosquitto_FREE(bridge->tls_psk_ਪਛਾਣ_ਤੱਤ);
	mosquitto_FREE(bridge->tls_psk);
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ
	mosquitto_FREE(bridge);
}
#ਜੇ_ਅੰਤ


ਸਥਿਰ ਖਾਲੀ print_ਸੰਸਕਰਣ(ਖਾਲੀ)
{
	ਲਿਖੋ("mosquitto %s\n", ਸੰਸਕਰਣ);
	ਲਿਖੋ("Copyright © 2025 Roger Light.\n");
	ਲਿਖੋ("License EPL-2.0 OR BSD-3-Clause.\n");
}


ਸਥਿਰ ਖਾਲੀ print_usage(ਖਾਲੀ)
{
	ਲਿਖੋ("mosquitto version %s\n\n", ਸੰਸਕਰਣ);
	ਲਿਖੋ("mosquitto is an MQTT v5.0/v3.1.1/v3.1 broker.\n\n");
	ਲਿਖੋ("Usage: mosquitto [-c config_file] [-d] [-h] [-p port] [-v]\n");
	ਲਿਖੋ("                 [--tls-keylog file]\n\n");
	ਲਿਖੋ(" -c : specify the broker config file.\n");
	ਲਿਖੋ(" -d : put the broker into the background after starting.\n");
	ਲਿਖੋ(" -h : display this help.\n");
	ਲਿਖੋ(" -p : start the broker listening on the specified port.\n");
	ਲਿਖੋ("      Not recommended in conjunction with the -c option.\n");
	ਲਿਖੋ(" -q : quiet mode - disable all logging types. This overrides\n");
	ਲਿਖੋ("      any logging options given in the config file, and -v.\n");
	ਲਿਖੋ(" -v : verbose mode - enable all logging types. This overrides\n");
	ਲਿਖੋ("      any logging options given in the config file.\n");
	ਲਿਖੋ(" --test-config : test config file and exit\n");
	ਲਿਖੋ(" --tls-keylog : log TLS connection information to a file, to allow\n");
	ਲਿਖੋ("      debugging with e.g. wireshark. Do not use on a production\n");
	ਲਿਖੋ("      server.\n");
	ਲਿਖੋ("\nSee https://mosquitto.org/ for more information.\n\n");
}


ਪੂਰਨ config_parse_ਦਲੀਲਾਂ(ਬਣਤਰ mosquitto__config *config, ਪੂਰਨ argc, ਅੱਖਰ *argv[])
{
	ਪੂਰਨ i;
	ਪੂਰਨ ਪੋਰਟ_tmp;

	ਲਈ(i=1; i<argc; i++){
		ਜੇ(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--config-file")){
			ਜੇ(i<argc-1){
				db.config_file = argv[i+1];

				ਜੇ(config__read(config, false)){
					ਵਾਪਸ MOSQ_ERR_INVAL;
				}
			}ਨਹੀਂਤਾਂ{
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: -c argument given, but no config file specified.");
				ਵਾਪਸ MOSQ_ERR_INVAL;
			}
			i++;
		}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--daemon")){
			config->daemon = true;
		}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
			print_usage();
			ਵਾਪਸ MOSQ_ERR_UNKNOWN;
		}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(argv[i], "--version")){
			print_ਸੰਸਕਰਣ();
			ਵਾਪਸ MOSQ_ERR_UNKNOWN;
		}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port")){
			ਜੇ(i<argc-1){
				ਪੋਰਟ_tmp = atoi(argv[i+1]);
				ਜੇ(ਪੋਰਟ_tmp<1 || ਪੋਰਟ_tmp>UINT16_MAX){
					log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid port specified (%d).", ਪੋਰਟ_tmp);
					ਵਾਪਸ MOSQ_ERR_INVAL;
				}ਨਹੀਂਤਾਂ{
					ਜੇ(config->cmd_ਪੋਰਟ_ਗਿਣਤੀ == CMD_ਪੋਰਟ_ਹੱਦਬੰਦੀ){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Only %d ports can be specified on the command line.", CMD_ਪੋਰਟ_ਹੱਦਬੰਦੀ);
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					config->cmd_ਪੋਰਟ[config->cmd_ਪੋਰਟ_ਗਿਣਤੀ] = (uint16_t)ਪੋਰਟ_tmp;
					config->cmd_ਪੋਰਟ_ਗਿਣਤੀ++;
				}
			}ਨਹੀਂਤਾਂ{
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: -p argument given, but no port specified.");
				ਵਾਪਸ MOSQ_ERR_INVAL;
			}
			i++;
		}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(argv[i], "--tls-keylog")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
			ਜੇ(i<argc-1){
				db.tls_keylog = mosquitto_strdup(argv[i+1]);
				ਜੇ(db.tls_keylog == ਨਿਰਾ){
					log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
					ਵਾਪਸ MOSQ_ERR_NOMEM;
				}
			}ਨਹੀਂਤਾਂ{
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: --tls-keylog argument given, but no file specified.");
				ਵਾਪਸ MOSQ_ERR_INVAL;
			}
			i++;
#ਨਹੀਂਤਾਂ
			fprintf(stderr, "Error: TLS support not available so --tls-keylog is not available.\n");
			ਵਾਪਸ MOSQ_ERR_INVAL;
#ਜੇ_ਅੰਤ
		}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(argv[i], "-q") || !strcmp(argv[i], "--quiet")){
			db.quiet = true;
		}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")){
			db.verbose = true;
		}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(argv[i], "--test-config")){
			config->test_configuration = true;
		}ਨਹੀਂਤਾਂ{
			fprintf(stderr, "Error: Unknown option '%s'.\n", argv[i]);
			print_usage();
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
	}

	/* Default to drop to mosquitto user if we are privileged and no user specified. */
	ਜੇ(!config->user){
		config->user = mosquitto_strdup("mosquitto");
		ਜੇ(config->user == ਨਿਰਾ){
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}
	}
	ਜੇ(db.quiet){
		config->log_type = 0;
	}ਨਹੀਂਤਾਂ ਜੇ(db.verbose){
		config->log_type = UINT_MAX;
	}

	ਜੇ(getenv("MOSQUITTO_PERSISTENCE_LOCATION")){
		mosquitto_FREE(config->persistence_location);
		config->persistence_location = mosquitto_strdup(getenv("MOSQUITTO_PERSISTENCE_LOCATION"));
		ਜੇ(!config->persistence_location){
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}
	}
	ਵਾਪਸ config__check(config);
}


ਸਥਿਰ ਖਾਲੀ config__copy(ਬਣਤਰ mosquitto__config *src, ਬਣਤਰ mosquitto__config *dest)
{
	mosquitto_FREE(dest->security_options.acl_ਅੰਕੜੇ.acl_file);
	dest->security_options.acl_ਅੰਕੜੇ.acl_file = src->security_options.acl_ਅੰਕੜੇ.acl_file;

	acl_file__cleanup(&dest->security_options.acl_ਅੰਕੜੇ);
	dest->security_options.acl_ਅੰਕੜੇ.acl_users = src->security_options.acl_ਅੰਕੜੇ.acl_users;
	dest->security_options.acl_ਅੰਕੜੇ.acl_patterns = src->security_options.acl_ਅੰਕੜੇ.acl_patterns;
	dest->security_options.acl_ਅੰਕੜੇ.acl_anon.username = src->security_options.acl_ਅੰਕੜੇ.acl_anon.username;
	dest->security_options.acl_ਅੰਕੜੇ.acl_anon.acl = src->security_options.acl_ਅੰਕੜੇ.acl_anon.acl;

	dest->security_options.allow_anonymous = src->security_options.allow_anonymous;
	dest->security_options.allow_zero_ਲੰਮਾਈ_clientid = src->security_options.allow_zero_ਲੰਮਾਈ_clientid;

	mosquitto_FREE(dest->security_options.auto_id_ਅਗੇਤਰ);
	dest->security_options.auto_id_ਅਗੇਤਰ = src->security_options.auto_id_ਅਗੇਤਰ;
	dest->security_options.auto_id_ਅਗੇਤਰ_len = src->security_options.auto_id_ਅਗੇਤਰ_len;

	mosquitto_FREE(dest->security_options.password_ਅੰਕੜੇ.password_file);
	dest->security_options.password_ਅੰਕੜੇ.password_file = src->security_options.password_ਅੰਕੜੇ.password_file;

	password_file__cleanup(&dest->security_options.password_ਅੰਕੜੇ);
	dest->security_options.password_ਅੰਕੜੇ.unpwd = src->security_options.password_ਅੰਕੜੇ.unpwd;

	mosquitto_FREE(dest->security_options.psk_file);
	dest->security_options.psk_file = src->security_options.psk_file;

	mosquitto_FREE(dest->security_options.plugins);
	dest->security_options.plugin_ਗਿਣਤੀ = src->security_options.plugin_ਗਿਣਤੀ;
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

	dest->ਸੁਨੇਹਾ_ਮਾਪ_ਹੱਦਬੰਦੀ = src->ਸੁਨੇਹਾ_ਮਾਪ_ਹੱਦਬੰਦੀ;

	dest->persistence = src->persistence;

	mosquitto_FREE(dest->persistence_location);
	dest->persistence_location = src->persistence_location;

	mosquitto_FREE(dest->persistence_file);
	dest->persistence_file = src->persistence_file;

	mosquitto_FREE(dest->persistence_filepath);
	dest->persistence_filepath = src->persistence_filepath;

	dest->persistent_client_expiration = src->persistent_client_expiration;


	dest->ਕਤਾਰ_qos0_messages = src->ਕਤਾਰ_qos0_messages;
	dest->sys_interval = src->sys_interval;
	dest->upgrade_outgoing_qos = src->upgrade_outgoing_qos;

#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	dest->websockets_log_level = src->websockets_log_level;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
	ਲਈ(ਪੂਰਨ i=0; i<dest->bridge_ਗਿਣਤੀ; i++){
		ਜੇ(dest->bridges[i]){
			config__bridge_cleanup(dest->bridges[i]);
		}
	}
	mosquitto_FREE(dest->bridges);
	dest->bridges = src->bridges;
	dest->bridge_ਗਿਣਤੀ = src->bridge_ਗਿਣਤੀ;
#ਜੇ_ਅੰਤ
}


ਪੂਰਨ config__read(ਬਣਤਰ mosquitto__config *config, bool reload)
{
	ਪੂਰਨ rc = MOSQ_ERR_SUCCESS;
	ਬਣਤਰ config_recurse cr;
	ਪੂਰਨ lineno = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_PERSISTENCE
	size_t len;
#ਜੇ_ਅੰਤ
	ਬਣਤਰ mosquitto__config config_reload;
	ਪੂਰਨ i;

	ਜੇ(reload){
		memset(&config_reload, 0, ਆਕਾਰ(ਬਣਤਰ mosquitto__config));
	}

	cr.log_dest = MQTT3_LOG_NONE;
	cr.log_dest_ਸੈੱਟ = 0;
	cr.log_type = MOSQ_LOG_NONE;
	cr.log_type_ਸੈੱਟ = 0;

	ਜੇ(!db.config_file){
		ਵਾਪਸ 0;
	}

	ਜੇ(reload){
		/* Re-initialise appropriate config vars to default for reload. */
		config__init_reload(&config_reload);
		config_reload.listeners = config->listeners;
		config_reload.listener_ਗਿਣਤੀ = config->listener_ਗਿਣਤੀ;
		cur_security_options = ਨਿਰਾ;
		rc = config__read_file(&config_reload, reload, db.config_file, &cr, 0, &lineno);
	}ਨਹੀਂਤਾਂ{
		rc = config__read_file(config, reload, db.config_file, &cr, 0, &lineno);
	}
	ਜੇ(rc){
		ਜੇ(lineno > 0){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error found at %s:%d.", db.config_file, lineno);
		}
		ਵਾਪਸ rc;
	}

	ਜੇ(reload){
		config__copy(&config_reload, config);
	}

	/* If auth/access options are set and allow_anonymous not explicitly set, disallow anon. */
	ਜੇ(config->local_only == false){
		ਜੇ(config->ਪ੍ਰਤੀ_listener_settings){
			ਲਈ(i=0; i<config->listener_ਗਿਣਤੀ; i++){
				/* Default option if no security options set */
				ਜੇ(config->listeners[i].security_options->allow_anonymous == -1){
					config->listeners[i].security_options->allow_anonymous = false;
				}
			}
		}ਨਹੀਂਤਾਂ{
			ਜੇ(config->security_options.allow_anonymous == -1){
				config->security_options.allow_anonymous = false;
			}
		}
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_PERSISTENCE
	ਜੇ(config->persistence){
		ਜੇ(!config->persistence_file){
			config->persistence_file = mosquitto_strdup("mosquitto.db");
			ਜੇ(!config->persistence_file){
				ਵਾਪਸ MOSQ_ERR_NOMEM;
			}
		}
		mosquitto_FREE(config->persistence_filepath);
		ਜੇ(config->persistence_location && ਲੜੀ_ਲੰਬਾਈ(config->persistence_location)){
			len = ਲੜੀ_ਲੰਬਾਈ(config->persistence_location) + ਲੜੀ_ਲੰਬਾਈ(config->persistence_file) + 2;
			config->persistence_filepath = mosquitto_malloc(len);
			ਜੇ(!config->persistence_filepath){
				ਵਾਪਸ MOSQ_ERR_NOMEM;
			}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
			snprintf(config->persistence_filepath, len, "%s\\%s", config->persistence_location, config->persistence_file);
#ਨਹੀਂਤਾਂ
			snprintf(config->persistence_filepath, len, "%s/%s", config->persistence_location, config->persistence_file);
#ਜੇ_ਅੰਤ
		}ਨਹੀਂਤਾਂ{
			config->persistence_filepath = mosquitto_strdup(config->persistence_file);
			ਜੇ(!config->persistence_filepath){
				ਵਾਪਸ MOSQ_ERR_NOMEM;
			}
		}
	}
#ਜੇ_ਅੰਤ
	/* Default to drop to mosquitto user if no other user specified. This must
	 * remain here even though it is covered in config__parse_args() because this
	 * function may be called on its own. */
	ਜੇ(!config->user){
		config->user = mosquitto_strdup("mosquitto");
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
	ਲਈ(i=0; i<config->bridge_ਗਿਣਤੀ; i++){
		ਜੇ(!config->bridges[i]->name){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: bridge name not defined.");
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
		ਜੇ(config->bridges[i]->ਪਤੇ  == 0){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no remote addresses defined.");
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
		ਜੇ(config->bridges[i]->topic_ਗਿਣਤੀ == 0){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no topics defined.");
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FINAL_WITH_TLS_PSK
		ਜੇ(config->bridges[i]->tls_psk && !config->bridges[i]->tls_psk_ਪਛਾਣ_ਤੱਤ){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_identity.");
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
		ਜੇ(config->bridges[i]->tls_psk_ਪਛਾਣ_ਤੱਤ && !config->bridges[i]->tls_psk){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_psk.");
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
#ਜੇ_ਅੰਤ
	}
#ਜੇ_ਅੰਤ

	ਜੇ(cr.log_dest_ਸੈੱਟ){
		config->log_dest = cr.log_dest;
	}
	ਜੇ(db.quiet){
		config->log_type = 0;
	}ਨਹੀਂਤਾਂ ਜੇ(db.verbose){
		config->log_type = UINT_MAX;
	}ਨਹੀਂਤਾਂ ਜੇ(cr.log_type_ਸੈੱਟ){
		config->log_type = cr.log_type;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
	ਵਾਪਸ config__check_bridges(config);
#ਨਹੀਂਤਾਂ
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
#ਜੇ_ਅੰਤ
}


ਸਥਿਰ mosquitto_plugin_id_t *config_plugin_ਲੱਭੋ(ਅਚਲ ਅੱਖਰ *name)
{
	ਜੇ(db.plugins && name){
		ਲਈ(ਪੂਰਨ i=0; i<db.plugin_ਗਿਣਤੀ; i++){
			ਜੇ(db.plugins[i]->config.name && !strcmp(db.plugins[i]->config.name, name)){
				ਵਾਪਸ db.plugins[i];
			}
		}
	}
	ਵਾਪਸ ਨਿਰਾ;
}


ਸਥਿਰ mosquitto_plugin_id_t *config_plugin_ਲੱਦੋ(ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ)
{
	mosquitto_plugin_id_t *plugin = ਨਿਰਾ;
	mosquitto_plugin_id_t **plugins = ਨਿਰਾ;

	ਜੇ(name && config_plugin_ਲੱਭੋ(name)){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Duplicate plugin name '%s'.", name);
		ਵਾਪਸ ਨਿਰਾ;
	}

	ਜੇ(!ਰਸਤਾ || !strcmp(ਰਸਤਾ, "")){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Missing plugin path for plugin name '%s'.", name);
		ਵਾਪਸ ਨਿਰਾ;
	}

	plugin = mosquitto_calloc(1, ਆਕਾਰ(mosquitto_plugin_id_t));
	ਜੇ(!plugin){
		ਜਾਓ error;
	}

	ਜੇ(name){
		plugin->config.name = mosquitto_strdup(name);
	}
	plugin->config.ਰਸਤਾ = mosquitto_strdup(ਰਸਤਾ);
	ਜੇ((name && !plugin->config.name) || !plugin->config.ਰਸਤਾ){
		ਜਾਓ error;
	}
	plugin->config.options = ਨਿਰਾ;
	plugin->config.option_ਗਿਣਤੀ = 0;
	plugin->config.deny_special_chars = true;

	/* Add to db list */
	plugins = mosquitto_realloc(db.plugins, (size_t)(db.plugin_ਗਿਣਤੀ+1)*ਆਕਾਰ(ਬਣਤਰ mosquitto__plugin_config *));
	ਜੇ(!plugins){
		ਜਾਓ error;
	}

	plugins[db.plugin_ਗਿਣਤੀ] = plugin;
	db.plugins = plugins;
	db.plugin_ਗਿਣਤੀ++;

	ਵਾਪਸ plugin;
error:
	log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
	ਜੇ(plugin){
		mosquitto_FREE(plugin->config.name);
		mosquitto_FREE(plugin->config.ਰਸਤਾ);
	}
	mosquitto_FREE(plugin);
	ਵਾਪਸ ਨਿਰਾ;
}


ਪੂਰਨ config_plugin_ਜੋੜੋ_secopt(mosquitto_plugin_id_t *plugin, ਬਣਤਰ mosquitto__security_options *security_options)
{
	ਬਣਤਰ mosquitto__security_options **new_options;
	mosquitto_plugin_id_t **new_plugins;

	new_options = mosquitto_realloc(plugin->config.security_options, (size_t)(plugin->config.security_option_ਗਿਣਤੀ+1)*ਆਕਾਰ(ਬਣਤਰ mosquitto__security_options *));
	new_plugins = mosquitto_realloc(security_options->plugins, (size_t)(security_options->plugin_ਗਿਣਤੀ+1)*ਆਕਾਰ(mosquitto_plugin_id_t *));

	ਜੇ(!new_options || !new_plugins){
		mosquitto_FREE(new_options);
		mosquitto_FREE(new_plugins);
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	new_options[plugin->config.security_option_ਗਿਣਤੀ] = security_options;
	plugin->config.security_options = new_options;
	plugin->config.security_option_ਗਿਣਤੀ++;

	new_plugins[security_options->plugin_ਗਿਣਤੀ] = plugin;
	security_options->plugins = new_plugins;
	security_options->plugin_ਗਿਣਤੀ++;

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ config__read_file_core(ਬਣਤਰ mosquitto__config *config, bool reload, ਬਣਤਰ config_recurse *cr, ਪੂਰਨ level, ਪੂਰਨ *lineno, FILE *fptr, ਅੱਖਰ **buf, ਪੂਰਨ *buflen)
{
	ਪੂਰਨ rc;
	ਅੱਖਰ *token;
	ਪੂਰਨ tmp_int;
	ਅੱਖਰ *saveptr = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
	ਅੱਖਰ *tmp_char;
	ਬਣਤਰ mosquitto__bridge *cur_bridge = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
	mosquitto_plugin_id_t *cur_plugin = ਨਿਰਾ;

	ਅੱਖਰ *ਕੁੰਜੀ;
	ਬਣਤਰ mosquitto__listener *cur_listener = ਨਿਰਾ;
	ਪੂਰਨ i;
	ਪੂਰਨ lineno_ext = 0;
	size_t ਅਗੇਤਰ_len;
	size_t slen;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_WEBSOCKETS
	ਅੱਖਰ **ws_origins = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

	*lineno = 0;

	ਜਦੋਂ(mosquitto_fgets(buf, buflen, fptr)){
		(*lineno)++;
		ਜੇ((*buf)[0] != '#' && (*buf)[0] != 10 && (*buf)[0] != 13){
			slen = ਲੜੀ_ਲੰਬਾਈ(*buf);
			ਜੇ(slen == 0){
				ਜਾਰੀ;
			}
			ਜਦੋਂ((*buf)[slen-1] == 10 || (*buf)[slen-1] == 13){
				(*buf)[slen-1] = 0;
				slen = ਲੜੀ_ਲੰਬਾਈ(*buf);
				ਜੇ(slen == 0){
					ਜਾਰੀ;
				}
			}
			token = strtok_r((*buf), " ", &saveptr);
			ਜੇ(token){
				ਜੇ(!strcmp(token, "accept_protocol_versions")){
					REQUIRE_NON_DEFAULT_LISTENER(token);
					cur_listener->disable_protocol_v3 = true;
					cur_listener->disable_protocol_v4 = true;
					cur_listener->disable_protocol_v5 = true;
					ਜੇ(saveptr == ਨਿਰਾ){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", "accept_protocol_versions");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					token = strtok_r(saveptr, ", \t", &saveptr);
					REQUIRE_NON_ਖਾਲੀ_ਥਾਵਾਂ_OPTION(token, "accept_protocol_versions");

					ਜਦੋਂ(token){
						ਜੇ(!strcmp(token, "3")){
							cur_listener->disable_protocol_v3 = false;
						}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "4")){
							cur_listener->disable_protocol_v4 = false;
						}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "5")){
							cur_listener->disable_protocol_v5 = false;
						}

						token = strtok_r(ਨਿਰਾ, ", \t", &saveptr);
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "acl_file")){
					REQUIRE_LISTENER_IF_ਪ੍ਰਤੀ_LISTENER(token);

					conf_ਸੈੱਟ_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->acl_ਅੰਕੜੇ.acl_file);
					ਜੇ(conf__parse_string(&token, "acl_file", &cur_security_options->acl_ਅੰਕੜੇ.acl_file, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "address") || !strcmp(token, "addresses")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ਜੇ(cur_bridge->ਪਤੇ){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration, 'address' only allowed once.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜਦੋਂ((token = strtok_r(ਨਿਰਾ, " ", &saveptr))){
						ਜੇ(token[0] == '#'){
							ਰੋਕੋ;
						}
						ਬਣਤਰ bridge_ਪਤਾ *new_ਪਤੇ = mosquitto_realloc(cur_bridge->ਪਤੇ, ਆਕਾਰ(ਬਣਤਰ bridge_ਪਤਾ)*(size_t)(cur_bridge->ਪਤਾ_ਗਿਣਤੀ+1));
						ਜੇ(!new_ਪਤੇ){
							log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
							ਵਾਪਸ MOSQ_ERR_NOMEM;
						}
						cur_bridge->ਪਤਾ_ਗਿਣਤੀ++;
						cur_bridge->ਪਤੇ = new_ਪਤੇ;
						memset(&cur_bridge->ਪਤੇ[cur_bridge->ਪਤਾ_ਗਿਣਤੀ-1], 0, ਆਕਾਰ(ਬਣਤਰ bridge_ਪਤਾ));
						cur_bridge->ਪਤੇ[cur_bridge->ਪਤਾ_ਗਿਣਤੀ-1].ਪਤਾ = mosquitto_strdup(token);
						ਜੇ(!cur_bridge->ਪਤੇ[cur_bridge->ਪਤਾ_ਗਿਣਤੀ-1].ਪਤਾ){
							log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
							ਵਾਪਸ MOSQ_ERR_NOMEM;
						}
					}
					ਲਈ(i=0; i<cur_bridge->ਪਤਾ_ਗਿਣਤੀ; i++){
						/* cur_bridge->addresses[i].address is now
						 * "address[:port]". If address is an IPv6 address,
						 * then port is required. We must check for the :
						 * backwards. */
						tmp_char = strrchr(cur_bridge->ਪਤੇ[i].ਪਤਾ, ':');
						ਜੇ(tmp_char){
							/* Remove ':', so cur_bridge->addresses[i].address
							 * now just looks like the address. */
							tmp_char[0] = '\0';

							/* The remainder of the string */
							tmp_int = atoi(&tmp_char[1]);
							ਜੇ(tmp_int < 1 || tmp_int > UINT16_MAX){
								log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid bridge port value (%d).", tmp_int);
								ਵਾਪਸ MOSQ_ERR_INVAL;
							}
							cur_bridge->ਪਤੇ[i].ਪੋਰਟ = (uint16_t)tmp_int;
						}ਨਹੀਂਤਾਂ{
							cur_bridge->ਪਤੇ[i].ਪੋਰਟ = 1883;
						}
						conf__attempt_resolve(cur_bridge->ਪਤੇ[i].ਪਤਾ, "bridge address", MOSQ_LOG_WARNING, "Warning");
					}
					ਜੇ(cur_bridge->ਪਤਾ_ਗਿਣਤੀ == 0){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Empty address value in configuration.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "allow_anonymous")){
					REQUIRE_LISTENER_IF_ਪ੍ਰਤੀ_LISTENER(token);
					ਪ੍ਰਤੀ_LISTENER_ALTERNATIVE(token, "listener_allow_anonymous");
					conf_ਸੈੱਟ_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ਜੇ(conf__parse_bool(&token, "allow_anonymous", (bool *)&cur_security_options->allow_anonymous, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "allow_duplicate_messages")){
					OPTION_DEPRECATED(token, "The behaviour will default to true.");
					ਜੇ(conf__parse_bool(&token, "allow_duplicate_messages", &config->allow_duplicate_messages, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "allow_zero_length_clientid")){
					REQUIRE_LISTENER_IF_ਪ੍ਰਤੀ_LISTENER(token);
					conf_ਸੈੱਟ_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ਜੇ(conf__parse_bool(&token, "allow_zero_length_clientid", &cur_security_options->allow_zero_ਲੰਮਾਈ_clientid, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(token, "auth_opt_", ਲੜੀ_ਲੰਬਾਈ("auth_opt_")) || !ਲੜੀ_ਤੁਲਨਾ(token, "plugin_opt_", ਲੜੀ_ਲੰਬਾਈ("plugin_opt_"))){
					ਜੇ(reload){
						ਜਾਰੀ;        /* Auth plugin not currently valid for reloading. */

					}
					REQUIRE_PLUGIN(token);

					ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(token, "auth_opt_", ਲੜੀ_ਲੰਬਾਈ("auth_opt_"))){
						ਅਗੇਤਰ_len = ਲੜੀ_ਲੰਬਾਈ("auth_opt_");
					}ਨਹੀਂਤਾਂ{
						ਅਗੇਤਰ_len = ਲੜੀ_ਲੰਬਾਈ("plugin_opt_");
					}
					ਜੇ(ਲੜੀ_ਲੰਬਾਈ(token) < ਅਗੇਤਰ_len + 3){
						/* auth_opt_ == 9, + one digit key == 10, + one space == 11, + one value == 12 */
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid 'plugin_opt_' config option.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਕੁੰਜੀ = mosquitto_strdup(&token[ਅਗੇਤਰ_len]);
					ਜੇ(!ਕੁੰਜੀ){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
						ਵਾਪਸ MOSQ_ERR_NOMEM;
					}ਨਹੀਂਤਾਂ ਜੇ(STREMPTY(ਕੁੰਜੀ)){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Empty 'plugin_opt_' config option.");
						mosquitto_FREE(ਕੁੰਜੀ);
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					token = saveptr;
					ਜੇ(token && token[0]){
						ਜਦੋਂ(token[0] == ' ' || token[0] == '\t'){
							token++;
						}
						ਬਣਤਰ mosquitto_opt *new_options = mosquitto_realloc(cur_plugin->config.options, (size_t)(cur_plugin->config.option_ਗਿਣਤੀ+1)*ਆਕਾਰ(ਬਣਤਰ mosquitto_opt));
						ਜੇ(!new_options){
							log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
							mosquitto_FREE(ਕੁੰਜੀ);
							ਵਾਪਸ MOSQ_ERR_NOMEM;
						}
						cur_plugin->config.option_ਗਿਣਤੀ++;
						cur_plugin->config.options = new_options;
						cur_plugin->config.options[cur_plugin->config.option_ਗਿਣਤੀ-1].ਕੁੰਜੀ = ਕੁੰਜੀ;
						cur_plugin->config.options[cur_plugin->config.option_ਗਿਣਤੀ-1].ਮੁੱਲ = mosquitto_strdup(token);
						ਜੇ(!cur_plugin->config.options[cur_plugin->config.option_ਗਿਣਤੀ-1].ਮੁੱਲ){
							log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
							ਵਾਪਸ MOSQ_ERR_NOMEM;
						}
					}ਨਹੀਂਤਾਂ{
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", ਕੁੰਜੀ);
						mosquitto_FREE(ਕੁੰਜੀ);
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "auth_plugin") || !strcmp(token, "plugin") || !strcmp(token, "global_plugin")){
					ਜੇ(reload){
						ਜਾਰੀ;        /* plugin not currently valid for reloading. */
					}
					ਜੇ(!strcmp(token, "global_plugin")){
						cur_security_options = &db.config->security_options;
					}ਨਹੀਂਤਾਂ{
						REQUIRE_LISTENER_IF_ਪ੍ਰਤੀ_LISTENER(token);
						conf_ਸੈੱਟ_cur_security_options(config, &cur_listener, &cur_security_options, token);
					}

					cur_plugin = config_plugin_ਲੱਦੋ(ਨਿਰਾ, saveptr);
					ਜੇ(cur_plugin == ਨਿਰਾ){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(config_plugin_ਜੋੜੋ_secopt(cur_plugin, cur_security_options)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "auth_plugin_deny_special_chars")){
					ਜੇ(reload){
						ਜਾਰੀ;        /* Auth plugin not currently valid for reloading. */
					}
					ਜੇ(!cur_plugin){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: An auth_plugin_deny_special_chars option exists in the config file without a plugin.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(conf__parse_bool(&token, "auth_plugin_deny_special_chars", &cur_plugin->config.deny_special_chars, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "plugin_load")){
					ਅੱਖਰ *name = ਨਿਰਾ;

					ਜੇ(reload){
						ਜਾਰੀ;        /* plugin not currently valid for reloading. */

					}
					name = strtok_r(ਨਿਰਾ, " ", &saveptr);
					REQUIRE_NON_ਖਾਲੀ_ਥਾਵਾਂ_OPTION(name, "plugin_load");

					cur_plugin = config_plugin_ਲੱਦੋ(name, saveptr);
					ਜੇ(cur_plugin == ਨਿਰਾ){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "plugin_use")){
					ਅੱਖਰ *name = ਨਿਰਾ;

					ਜੇ(reload){
						ਜਾਰੀ;        /* plugin not currently valid for reloading. */
					}
					REQUIRE_NON_DEFAULT_LISTENER(token);

					ਜੇ(conf__parse_string(&token, "plugin_use", &name, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					cur_plugin = config_plugin_ਲੱਭੋ(name);
					ਜੇ(!cur_plugin){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Plugin '%s' not previously loaded.", name);
						mosquitto_FREE(name);
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(name);
					ਜੇ(config_plugin_ਜੋੜੋ_secopt(cur_plugin, cur_listener->security_options)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "auto_id_prefix")){
					REQUIRE_LISTENER_IF_ਪ੍ਰਤੀ_LISTENER(token);
					OPTION_DEPRECATED(token, "Please use 'listener_auto_id_prefix' instead.");
					conf_ਸੈੱਟ_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ਜੇ(conf__parse_string(&token, "auto_id_prefix", &cur_security_options->auto_id_ਅਗੇਤਰ, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(cur_security_options->auto_id_ਅਗੇਤਰ){
						ਜੇ(ਲੜੀ_ਲੰਬਾਈ(cur_security_options->auto_id_ਅਗੇਤਰ) > 50){
							log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: auto_id_prefix length must be <= 50.");
							ਵਾਪਸ MOSQ_ERR_INVAL;
						}
						cur_security_options->auto_id_ਅਗੇਤਰ_len = (uint16_t)ਲੜੀ_ਲੰਬਾਈ(cur_security_options->auto_id_ਅਗੇਤਰ);
					}ਨਹੀਂਤਾਂ{
						cur_security_options->auto_id_ਅਗੇਤਰ_len = 0;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "autosave_interval")){
					ਜੇ(conf__parse_int(&token, "autosave_interval", &config->autosave_interval, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(config->autosave_interval < 0){
						config->autosave_interval = 0;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "autosave_on_changes")){
					ਜੇ(conf__parse_bool(&token, "autosave_on_changes", &config->autosave_on_changes, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "bind_address")){
					OPTION_DEPRECATED(token, "");
					config->local_only = false;
					ਜੇ(reload){
						ਜਾਰੀ;        /* Rebinding listeners not valid during reloading. */

					}
					ਜੇ(config__create_default_listener(config, token)){
						ਵਾਪਸ MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					ਜੇ(conf__parse_string(&token, "default listener bind_address", &config->default_listener->host, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(conf__attempt_resolve(config->default_listener->host, "bind_address", MOSQ_LOG_ERR, "Error")){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "bind_interface")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SO_BINDTODEVICE
					ਜੇ(reload){
						ਜਾਰੀ;        /* Rebinding listeners not valid during reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ਜੇ(conf__parse_string(&token, "bind_interface", &cur_listener->bind_interface, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: bind_interface specified but socket option not available.");
					ਵਾਪਸ MOSQ_ERR_INVAL;
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "bridge_attempt_unsubscribe")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_bool(&token, "bridge_attempt_unsubscribe", &cur_bridge->attempt_unsubscribe, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "bridge_cafile")){
#ਜੇ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					ਜੇ(conf__parse_string(&token, "bridge_cafile", &cur_bridge->tls_cafile, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "bridge_alpn")){
#ਜੇ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_string(&token, "bridge_alpn", &cur_bridge->tls_alpn, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "bridge_ciphers")){
#ਜੇ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_string(&token, "bridge_ciphers", &cur_bridge->tls_ciphers, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "bridge_ciphers_tls1.3")){
#ਜੇ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_string(&token, "bridge_ciphers_tls1.3", &cur_bridge->tls_13_ciphers, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "bridge_bind_address")){
#ਜੇ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_string(&token, "bridge_bind_address", &cur_bridge->bind_ਪਤਾ, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "bridge_capath")){
#ਜੇ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					ਜੇ(conf__parse_string(&token, "bridge_capath", &cur_bridge->tls_capath, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "bridge_certfile")){
#ਜੇ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					ਜੇ(conf__parse_string(&token, "bridge_certfile", &cur_bridge->tls_certfile, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "bridge_identity")){
#ਜੇ defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					ਜੇ(conf__parse_string(&token, "bridge_identity", &cur_bridge->tls_psk_ਪਛਾਣ_ਤੱਤ, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "bridge_insecure")){
#ਜੇ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_bool(&token, "bridge_insecure", &cur_bridge->tls_insecure, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(cur_bridge->tls_insecure){
						log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge '%s' using insecure mode.", cur_bridge->name);
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "bridge_require_ocsp")){
#ਜੇ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_bool(&token, "bridge_require_ocsp", &cur_bridge->tls_ocsp_required, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "bridge_max_packet_size")){
#ਜੇ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_int(&token, "bridge_max_packet_size", &tmp_int, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(tmp_int < 0){
						tmp_int = 0;
					}
					cur_bridge->ਵੱਧ_ਤੋਂ_ਵੱਧ_packet_ਮਾਪ = (uint32_t)tmp_int;
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "bridge_max_topic_alias")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_int(&token, "bridge_max_topic_alias", &tmp_int, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}

					ਜੇ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: bridge_max_topic_alias must be > 0 and <= 65535.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					cur_bridge->max_topic_alias = (uint16_t)tmp_int;
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "bridge_outgoing_retain")){
#ਜੇ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_bool(&token, "bridge_outgoing_retain", &cur_bridge->outgoing_retain, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "bridge_keyfile")){
#ਜੇ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					mosquitto_FREE(cur_bridge->tls_keyfile);
					ਜੇ(conf__parse_string(&token, "bridge_keyfile", &cur_bridge->tls_keyfile, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "bridge_protocol_version")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(ਨਿਰਾ, "", &saveptr);
					REQUIRE_NON_ਖਾਲੀ_ਥਾਵਾਂ_OPTION(token, "bridge_protocol_version");

					ਜੇ(!strcmp(token, "mqttv31")){
						cur_bridge->protocol_ਸੰਸਕਰਣ = mosq_p_mqtt31;
					}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "mqttv311")){
						cur_bridge->protocol_ਸੰਸਕਰਣ = mosq_p_mqtt311;
					}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "mqttv50")){
						cur_bridge->protocol_ਸੰਸਕਰਣ = mosq_p_mqtt5;
					}ਨਹੀਂਤਾਂ{
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid 'bridge_protocol_version' value (%s).", token);
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "bridge_psk")){
#ਜੇ defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					ਜੇ(conf__parse_string(&token, "bridge_psk", &cur_bridge->tls_psk, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "bridge_receive_maximum")){
#ਜੇ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_int(&token, "bridge_receive_maximum", &tmp_int, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(tmp_int <= 0){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be greater than 0.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}ਨਹੀਂਤਾਂ ਜੇ((uint64_t)tmp_int > (uint64_t)UINT16_MAX){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be lower than %u.", UINT16_MAX);
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					cur_bridge->receive_ਵੱਧ_ਤੋਂ_ਵੱਧ = (uint16_t)tmp_int;
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "bridge_reload_type")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(ਨਿਰਾ, " ", &saveptr);
					REQUIRE_NON_ਖਾਲੀ_ਥਾਵਾਂ_OPTION(token, "bridge_reload_type");

					ਜੇ(!strcmp(token, "lazy")){
						cur_bridge->reload_type = brt_lazy;
					}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "immediate")){
						cur_bridge->reload_type = brt_ਤੁਰੰਤ;
					}ਨਹੀਂਤਾਂ{
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid 'bridge_reload_type' value in configuration (%s).", token);
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "bridge_session_expiry_interval")){
#ਜੇ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_int(&token, "bridge_session_expiry_interval", &tmp_int, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(tmp_int < 0){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must not be negative.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}ਨਹੀਂਤਾਂ ਜੇ((uint64_t)tmp_int > (uint64_t)MQTT_SESSION_EXPIRY_NEVER){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must be lower than %u.", MQTT_SESSION_EXPIRY_NEVER);
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					cur_bridge->session_expiry_interval = (uint32_t)tmp_int;
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "bridge_tcp_keepalive")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
					REQUIRE_BRIDGE(token);

					ਜੇ(conf__parse_int(&token, "bridge_tcp_keepalive_idle", &tmp_int, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(tmp_int <= 0){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: invalid TCP keepalive idle value.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_idle = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)tmp_int;

					ਜੇ(conf__parse_int(&token, "bridge_tcp_keepalive_interval", &tmp_int, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(tmp_int <= 0){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: invalid TCP keepalive interval value.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_interval = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)tmp_int;

					ਜੇ(conf__parse_int(&token, "bridge_tcp_keepalive_counter", &tmp_int, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(tmp_int <= 0){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: invalid TCP keepalive counter value.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_counter = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)tmp_int;
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "bridge_tcp_user_timeout")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TCP_USER_TIMEOUT
					ਜੇ(conf__parse_int(&token, "bridge_tcp_user_timeout", &tmp_int, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(tmp_int < 0){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: invalid TCP user timeout value.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_user_timeout = tmp_int;
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge TCP user timeout support not available.");
#ਜੇ_ਅੰਤ
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "bridge_tls_use_os_certs")){
#ਜੇ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_bool(&token, "bridge_tls_use_os_certs", &cur_bridge->tls_use_os_certs, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "bridge_tls_version")){
#ਜੇ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_string(&token, "bridge_tls_version", &cur_bridge->tls_ਸੰਸਕਰਣ, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "cafile")){
#ਜੇ defined(WITH_TLS)
					REQUIRE_LISTENER(token);
					ਜੇ(cur_listener->psk_hint){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->cafile);
					ਜੇ(conf__parse_string(&token, "cafile", &cur_listener->cafile, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "capath")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->capath);
					ਜੇ(conf__parse_string(&token, "capath", &cur_listener->capath, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "certfile")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ਜੇ(cur_listener->psk_hint){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->certfile);
					ਜੇ(conf__parse_string(&token, "certfile", &cur_listener->certfile, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "check_retain_source")){
					ਜੇ(conf__parse_bool(&token, "check_retain_source", &config->check_retain_ਸਰੋਤ, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "ciphers")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers);
					ਜੇ(conf__parse_string(&token, "ciphers", &cur_listener->ciphers, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "ciphers_tls1.3")){
#ਜੇ defined(WITH_TLS) && (!defined(LIBRESSL_ਸੰਸਕਰਣ_ਸੰਖਿਆ) || LIBRESSL_ਸੰਸਕਰਣ_ਸੰਖਿਆ > 0x3040000FL)
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers_tls13);
					ਜੇ(conf__parse_string(&token, "ciphers_tls1.3", &cur_listener->ciphers_tls13, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: ciphers_tls1.3 support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "clientid") || !strcmp(token, "remote_clientid")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_string(&token, "bridge remote clientid", &cur_bridge->remote_clientid, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "cleansession")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_bool(&token, "cleansession", &cur_bridge->clean_ਸ਼ੁਰੂ, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "local_cleansession")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_bool(&token, "local_cleansession", (bool *)&cur_bridge->clean_ਸ਼ੁਰੂ_local, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "clientid_prefixes")){
					OPTION_DEPRECATED(token, "");
					mosquitto_FREE(config->clientid_prefixes);
					ਜੇ(conf__parse_string(&token, "clientid_prefixes", &config->clientid_prefixes, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "connection")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
					token = strtok_r(ਨਿਰਾ, " ", &saveptr);
					REQUIRE_NON_ਖਾਲੀ_ਥਾਵਾਂ_OPTION(token, "connection");

					/* Check for existing bridge name. */
					ਲਈ(i=0; i<config->bridge_ਗਿਣਤੀ; i++){
						ਜੇ(!strcmp(config->bridges[i]->name, token)){
							log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Duplicate bridge name '%s'.", token);
							ਵਾਪਸ MOSQ_ERR_INVAL;
						}
					}

					ਬਣਤਰ mosquitto__bridge **bridges_new = mosquitto_realloc(config->bridges, (size_t)(config->bridge_ਗਿਣਤੀ+1)*ਆਕਾਰ(ਬਣਤਰ mosquitto__bridge *));
					ਜੇ(!bridges_new){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
						ਵਾਪਸ MOSQ_ERR_NOMEM;
					}
					config->bridges = bridges_new;
					cur_bridge = mosquitto_malloc(ਆਕਾਰ(ਬਣਤਰ mosquitto__bridge));
					ਜੇ(!cur_bridge){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
						ਵਾਪਸ MOSQ_ERR_NOMEM;
					}

					config->bridge_ਗਿਣਤੀ++;
					config->bridges[config->bridge_ਗਿਣਤੀ-1] = cur_bridge;

					memset(cur_bridge, 0, ਆਕਾਰ(ਬਣਤਰ mosquitto__bridge));
					cur_bridge->name = mosquitto_strdup(token);
					ਜੇ(!cur_bridge->name){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
						ਵਾਪਸ MOSQ_ERR_NOMEM;
					}
					cur_bridge->keepalive = 60;
					cur_bridge->notifications = true;
					cur_bridge->notifications_local_only = false;
					cur_bridge->ਸ਼ੁਰੂ_type = bst_automatic;
					cur_bridge->idle_timeout = 60;
					cur_bridge->restart_timeout = 0;
					cur_bridge->backoff_ਅਧਾਰ = 5 * 1000;
					cur_bridge->backoff_cap = 30 * 1000;
					cur_bridge->stable_connection_period = 0;
					cur_bridge->ਹੱਦ = 10;
					cur_bridge->try_ਨਿੱਜੀ = true;
					cur_bridge->attempt_unsubscribe = true;
					cur_bridge->protocol_ਸੰਸਕਰਣ = mosq_p_mqtt311;
					cur_bridge->primary_retry_sock = INVALID_SOCKET;
					cur_bridge->outgoing_retain = true;
					cur_bridge->clean_ਸ਼ੁਰੂ_local = -1;
					cur_bridge->reload_type = brt_lazy;
					cur_bridge->max_topic_alias = 10;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TCP_USER_TIMEOUT
					cur_bridge->tcp_user_timeout = -1;
#ਜੇ_ਅੰਤ
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "connection_messages")){
					ਜੇ(conf__parse_bool(&token, token, &config->connection_messages, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "crlfile")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->crlfile);
					ਜੇ(conf__parse_string(&token, "crlfile", &cur_listener->crlfile, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "dhparamfile")){
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: dhparamfile is no longer required.");
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "disable_client_cert_date_checks")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
					REQUIRE_LISTENER(token);
					ਜੇ(conf__parse_bool(&token, "disable_client_cert_date_checks", &cur_listener->disable_client_cert_date_checks, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "enable_control_api")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_CONTROL
					ਜੇ(conf__parse_bool(&token, "enable_control_api", &config->enable_control_api, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: $CONTROL support not available (enable_control_api).");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "enable_proxy_protocol")){
#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					ਵਾਪਸ MOSQ_ERR_INVAL;
#ਜੇ_ਅੰਤ
					REQUIRE_LISTENER(token);
					ਜੇ(conf__parse_int(&token, "enable_proxy_protocol", &cur_listener->enable_proxy_protocol, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(cur_listener->enable_proxy_protocol < 1 || cur_listener->enable_proxy_protocol > 2){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: enable_proxy_protocol must be 1 or 2.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "global_max_clients")){
					ਜੇ(conf__parse_int(&token, "global_max_clients", &config->global_max_clients, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "global_max_connections")){
					ਜੇ(conf__parse_int(&token, "global_max_connections", &config->global_max_connections, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "http_dir")){
#ਜੇ defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
					ਜੇ(reload){
						ਜਾਰੀ;        /* Not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					ਜੇ(conf__parse_string(&token, "http_dir", &cur_listener->http_dir, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
					ਅੱਖਰ *http_dir_canonical = _fullpath(ਨਿਰਾ, cur_listener->http_dir, 0);
					ਅਚਲ ਅੱਖਰ sep = '\\';
#ਨਹੀਂਤਾਂ
					ਅੱਖਰ *http_dir_canonical = realpath(cur_listener->http_dir, ਨਿਰਾ);
					ਅਚਲ ਅੱਖਰ sep = '/';
#ਜੇ_ਅੰਤ
					ਜੇ(!http_dir_canonical){
						ਵਾਪਸ MOSQ_ERR_NOMEM;
					}
					size_t http_dir_len = ਲੜੀ_ਲੰਬਾਈ(http_dir_canonical) + ਆਕਾਰ(sep) + 1;
					ਅੱਖਰ *http_dir_canonical_sep = mosquitto_calloc(http_dir_len, ਆਕਾਰ(ਅੱਖਰ));
					ਜੇ(!http_dir_canonical_sep){
						ਮੁਕਤ(http_dir_canonical);
						ਵਾਪਸ MOSQ_ERR_NOMEM;
					}
					snprintf(http_dir_canonical_sep, http_dir_len, "%s%c", http_dir_canonical, sep);
					ਮੁਕਤ(http_dir_canonical);
					mosquitto_FREE(cur_listener->http_dir);
					cur_listener->http_dir = http_dir_canonical_sep;
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: http_dir support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "idle_timeout")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_int(&token, "idle_timeout", &cur_bridge->idle_timeout, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(cur_bridge->idle_timeout < 1){
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "idle_timeout interval too low, using 1 second.");
						cur_bridge->idle_timeout = 1;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "include_dir")){
					ਜੇ(level == 0){
						ਅੱਖਰ **files;
						ਪੂਰਨ file_ਗਿਣਤੀ;

						/* Only process include_dir from the main config file. */
						token = strtok_r(ਨਿਰਾ, "", &saveptr);
						REQUIRE_NON_ਖਾਲੀ_ਥਾਵਾਂ_OPTION(token, "include_dir");

						rc = config__get_dir_files(token, &files, &file_ਗਿਣਤੀ);
						ਜੇ(rc){
							ਵਾਪਸ rc;
						}

						ਲਈ(i=0; i<file_ਗਿਣਤੀ; i++){
							log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Loading config file '%s'", files[i]);

							rc = config__read_file(config, reload, files[i], cr, level+1, &lineno_ext);
							ਜੇ(rc){
								ਜੇ(lineno_ext > 0){
									log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error found at '%s:%d'.", files[i], lineno_ext);
								}
								/* Free happens below */
								ਰੋਕੋ;
							}
						}
						ਲਈ(i=0; i<file_ਗਿਣਤੀ; i++){
							mosquitto_FREE(files[i]);
						}
						mosquitto_FREE(files);
						ਜੇ(rc){
							ਵਾਪਸ rc;    /* This returns if config__read_file() fails above */
						}
					}ਨਹੀਂਤਾਂ{
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: The include_dir option is only valid in the main configuration file.");
						ਵਾਪਸ 1;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "keepalive_interval")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_int(&token, "keepalive_interval", &tmp_int, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(tmp_int > UINT16_MAX){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Bridge keepalive value too high.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(tmp_int < 5){
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "keepalive interval too low, using 5 seconds.");
						tmp_int = 5;
					}
					cur_bridge->keepalive = (uint16_t)tmp_int;
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "keyfile")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->keyfile);
					ਜੇ(conf__parse_string(&token, "keyfile", &cur_listener->keyfile, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "listener")){
					config->local_only = false;

					ਜੇ(conf__parse_int(&token, "listener port", &tmp_int, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_UNIX_SOCKETS
					ਜੇ(tmp_int < 0 || tmp_int > UINT16_MAX){
#ਨਹੀਂਤਾਂ
					ਜੇ(tmp_int < 1 || tmp_int > UINT16_MAX){
#ਜੇ_ਅੰਤ
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}

					/* Look for bind address / unix socket path */
					token = strtok_r(ਨਿਰਾ, " ", &saveptr);
					ਜੇ(token != ਨਿਰਾ && token[0] == '#'){
						token = ਨਿਰਾ;
					}

					ਜੇ(tmp_int == 0 && token == ਨਿਰਾ){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: A listener with port 0 must provide a Unix socket path.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}

					ਜੇ(reload){
						/* We reload listeners settings based on port number/unix socket path.
						 * If the port number/unix path doesn't already exist, exit with a complaint. */
						cur_listener = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_UNIX_SOCKETS
						ਜੇ(tmp_int == 0){
							ਲਈ(i=0; i<config->listener_ਗਿਣਤੀ; i++){
								ਜੇ(config->listeners[i].unix_socket_ਰਸਤਾ != ਨਿਰਾ
										&& strcmp(config->listeners[i].unix_socket_ਰਸਤਾ, token) == 0){

									cur_listener = &config->listeners[i];
									ਰੋਕੋ;
								}
							}
						}ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
						{
							ਲਈ(i=0; i<config->listener_ਗਿਣਤੀ; i++){
								ਜੇ(config->listeners[i].ਪੋਰਟ == tmp_int){
									/* Now check we have a matching bind address, if defined */
									ਜੇ(config->listeners[i].host){
										ਜੇ(token && !strcmp(config->listeners[i].host, token)){
											/* They both have a bind address, and they match */
											cur_listener = &config->listeners[i];
											ਰੋਕੋ;
										}
									}ਨਹੀਂਤਾਂ{
										ਜੇ(token == ਨਿਰਾ){
											/* Neither this config nor the new config have a bind address,
											 * so they match. */
											cur_listener = &config->listeners[i];
											ਰੋਕੋ;
										}
									}
								}
							}
						}
						ਜੇ(!cur_listener){
							log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: It is not currently possible to add/remove listeners when reloading the config file.");
							ਵਾਪਸ MOSQ_ERR_INVAL;
						}
					}ਨਹੀਂਤਾਂ{
						ਜੇ(config_ਜੋੜੋ_listener(config)){
							log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
							ਵਾਪਸ MOSQ_ERR_NOMEM;
						}
						cur_listener = &config->listeners[config->listener_ਗਿਣਤੀ-1];
					}

					listener_ਸੈੱਟ_defaults(cur_listener);
					cur_listener->ਪੋਰਟ = (uint16_t)tmp_int;

					mosquitto_FREE(cur_listener->host);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_UNIX_SOCKETS
					mosquitto_FREE(cur_listener->unix_socket_ਰਸਤਾ);
#ਜੇ_ਅੰਤ

					ਜੇ(token){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_UNIX_SOCKETS
						ਜੇ(cur_listener->ਪੋਰਟ == 0){
							cur_listener->unix_socket_ਰਸਤਾ = mosquitto_strdup(token);
						}ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
						{
							cur_listener->host = mosquitto_strdup(token);
						}
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "listener_allow_anonymous")){
					REQUIRE_LISTENER(token);
					ਜੇ(conf__parse_bool(&token, "listener_allow_anonymous", (bool *)&cur_listener->security_options->allow_anonymous, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "listener_auto_id_prefix")){
					REQUIRE_LISTENER(token);
					ਜੇ(conf__parse_string(&token, "listener_auto_id_prefix", &cur_listener->security_options->auto_id_ਅਗੇਤਰ, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(cur_listener->security_options->auto_id_ਅਗੇਤਰ){
						ਜੇ(ਲੜੀ_ਲੰਬਾਈ(cur_listener->security_options->auto_id_ਅਗੇਤਰ) > 50){
							log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: 'listener_auto_id_prefix' length must be <= 50.");
							ਵਾਪਸ MOSQ_ERR_INVAL;
						}
						cur_listener->security_options->auto_id_ਅਗੇਤਰ_len = (uint16_t)ਲੜੀ_ਲੰਬਾਈ(cur_listener->security_options->auto_id_ਅਗੇਤਰ);
					}ਨਹੀਂਤਾਂ{
						cur_listener->security_options->auto_id_ਅਗੇਤਰ_len = 0;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "local_clientid")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_string(&token, "bridge local clientd", &cur_bridge->local_clientid, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "local_password")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_string(&token, "bridge local_password", &cur_bridge->local_password, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "local_username")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_string(&token, "bridge local_username", &cur_bridge->local_username, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "log_dest")){
					token = strtok_r(ਨਿਰਾ, " ", &saveptr);
					REQUIRE_NON_ਖਾਲੀ_ਥਾਵਾਂ_OPTION(token, "log_dest");

					cr->log_dest_ਸੈੱਟ = 1;
					ਜੇ(!strcmp(token, "none")){
						cr->log_dest = MQTT3_LOG_NONE;
					}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "syslog")){
						cr->log_dest |= MQTT3_LOG_SYSLOG;
					}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "stdout")){
						cr->log_dest |= MQTT3_LOG_STDOUT;
					}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "stderr")){
						cr->log_dest |= MQTT3_LOG_STDERR;
					}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "topic")){
						cr->log_dest |= MQTT3_LOG_TOPIC;
					}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "dlt")){
						cr->log_dest |= MQTT3_LOG_DLT;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ANDROID
					}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "android")){
						cr->log_dest |= MQTT3_LOG_ANDROID;
#ਜੇ_ਅੰਤ
					}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "file")){
						ਜੇ(config->log_fptr || config->log_file){
							log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Duplicate \"log_dest file\" value.");
							ਵਾਪਸ MOSQ_ERR_INVAL;
						}
						/* Get remaining string. */
						token = saveptr;
						ਜੇ(token && token[0]){
							ਜਦੋਂ(token[0] == ' ' || token[0] == '\t'){
								token++;
							}
						}
						/* Duplicate "token" check here saves a log__printf() */
						ਜੇ(token && token[0]){
							config->log_file = mosquitto_strdup(token);
							ਜੇ(!config->log_file){
								log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
								ਵਾਪਸ MOSQ_ERR_NOMEM;
							}
							cr->log_dest |= MQTT3_LOG_FILE;
						}ਨਹੀਂਤਾਂ{
							log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Empty \"log_dest file\" value in configuration.");
							ਵਾਪਸ MOSQ_ERR_INVAL;
						}
						cr->log_dest |= MQTT3_LOG_FILE;
					}ਨਹੀਂਤਾਂ{
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid 'log_dest' value (%s).", token);
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਜੇ defined(WIN32) || defined(__CYGWIN__)
					ਜੇ(service_handle){
						ਜੇ(cr->log_dest == MQTT3_LOG_STDOUT || cr->log_dest == MQTT3_LOG_STDERR){
							log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Cannot log to stdout/stderr when running as a Windows service.");
							ਵਾਪਸ MOSQ_ERR_INVAL;
						}
					}
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "log_facility")){
#ਜੇ defined(WIN32) || defined(__CYGWIN__)
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: log_facility not supported on Windows.");
#ਨਹੀਂਤਾਂ
					ਜੇ(conf__parse_int(&token, "log_facility", &tmp_int, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਚੋਣ(tmp_int){
						ਹਾਲਤ 0:
							config->log_facility = LOG_LOCAL0;
							ਰੋਕੋ;
						ਹਾਲਤ 1:
							config->log_facility = LOG_LOCAL1;
							ਰੋਕੋ;
						ਹਾਲਤ 2:
							config->log_facility = LOG_LOCAL2;
							ਰੋਕੋ;
						ਹਾਲਤ 3:
							config->log_facility = LOG_LOCAL3;
							ਰੋਕੋ;
						ਹਾਲਤ 4:
							config->log_facility = LOG_LOCAL4;
							ਰੋਕੋ;
						ਹਾਲਤ 5:
							config->log_facility = LOG_LOCAL5;
							ਰੋਕੋ;
						ਹਾਲਤ 6:
							config->log_facility = LOG_LOCAL6;
							ਰੋਕੋ;
						ਹਾਲਤ 7:
							config->log_facility = LOG_LOCAL7;
							ਰੋਕੋ;
						ਮੂਲ:
							log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid 'log_facility' value (%d).", tmp_int);
							ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "log_timestamp")){
					ਜੇ(conf__parse_bool(&token, token, &config->log_timestamp, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "log_timestamp_format")){
					ਜੇ(conf__parse_string(&token, token, &config->log_timestamp_format, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "log_type")){
					token = strtok_r(ਨਿਰਾ, " ", &saveptr);
					REQUIRE_NON_ਖਾਲੀ_ਥਾਵਾਂ_OPTION(token, "log_type");

					cr->log_type_ਸੈੱਟ = 1;
					ਜੇ(!strcmp(token, "none")){
						cr->log_type = MOSQ_LOG_NONE;
					}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "information")){
						cr->log_type |= MOSQ_LOG_INFO;
					}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "notice")){
						cr->log_type |= MOSQ_LOG_NOTICE;
					}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "warning")){
						cr->log_type |= MOSQ_LOG_WARNING;
					}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "error")){
						cr->log_type |= MOSQ_LOG_ERR;
					}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "debug")){
						cr->log_type |= MOSQ_LOG_DEBUG;
					}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "subscribe")){
						cr->log_type |= MOSQ_LOG_SUBSCRIBE;
					}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "unsubscribe")){
						cr->log_type |= MOSQ_LOG_UNSUBSCRIBE;
					}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "internal")){
						cr->log_type |= MOSQ_LOG_ਅੰਦਰੂਨੀ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_WEBSOCKETS
					}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "websockets")){
						cr->log_type |= MOSQ_LOG_WEBSOCKETS;
#ਜੇ_ਅੰਤ
					}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "all")){
						cr->log_type = MOSQ_LOG_ALL;
					}ਨਹੀਂਤਾਂ{
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid 'log_type' value (%s).", token);
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "max_connections")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ਜੇ(conf__parse_int(&token, token, &cur_listener->max_connections, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(cur_listener->max_connections < 0){
						cur_listener->max_connections = -1;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "maximum_qos") || !strcmp(token, "max_qos")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ਜੇ(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(tmp_int < 0 || tmp_int > 2){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: 'max_qos' must be between 0 and 2 inclusive.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					cur_listener->max_qos = (uint8_t)tmp_int;
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "max_inflight_bytes")){
					ਜੇ(conf__parse_int(&token, "max_inflight_bytes", &tmp_int, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_inflight_ਬਾਈਟਾਂ = (size_t)tmp_int;
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "max_inflight_messages")){
					ਜੇ(conf__parse_int(&token, "max_inflight_messages", &tmp_int, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(tmp_int < 0 || tmp_int == UINT16_MAX){
						tmp_int = 0;
					}ਨਹੀਂਤਾਂ ਜੇ(tmp_int > UINT16_MAX){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: 'max_inflight_messages' must be <= 65535.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					config->max_inflight_messages = (uint16_t)tmp_int;
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "max_keepalive")){
					ਜੇ(conf__parse_int(&token, "max_keepalive", &tmp_int, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid 'max_keepalive' value (%d).", tmp_int);
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					config->max_keepalive = (uint16_t)tmp_int;
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "max_packet_size")){
					ਜੇ(conf__parse_int(&token, "max_packet_size", &tmp_int, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(tmp_int < 20){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: 'max_packet_size' must be >= 20.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					config->max_packet_ਮਾਪ = (uint32_t)tmp_int;
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "max_queued_bytes")){
					ਜੇ(conf__parse_int(&token, "max_queued_bytes", &tmp_int, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_ਬਾਈਟਾਂ = (size_t)tmp_int;
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "max_queued_messages")){
					ਜੇ(conf__parse_int(&token, "max_queued_messages", &tmp_int, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_messages = tmp_int;
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "memory_limit")){
					ssize_t lim;
					ਜੇ(conf__parse_ssize_t(&token, "memory_limit", &lim, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(lim < 0){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid 'memory_limit' value (%ld).", lim);
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					mosquitto_ਸਮ੍ਰਿਤੀ_ਸੈੱਟ_ਹੱਦਬੰਦੀ((size_t)lim);
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "message_size_limit")){
					log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Note: It is recommended to replace `message_size_limit` with `max_packet_size`.");
					ਜੇ(conf__parse_int(&token, "message_size_limit", (ਪੂਰਨ *)&config->ਸੁਨੇਹਾ_ਮਾਪ_ਹੱਦਬੰਦੀ, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(config->ਸੁਨੇਹਾ_ਮਾਪ_ਹੱਦਬੰਦੀ > MQTT_MAX_PAYLOAD){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid 'message_size_limit' value (%u).", config->ਸੁਨੇਹਾ_ਮਾਪ_ਹੱਦਬੰਦੀ);
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "mount_point")){
					REQUIRE_LISTENER(token);
					mosquitto_FREE(cur_listener->mount_ਬਿੰਦੂ);
					ਜੇ(conf__parse_string(&token, "mount_point", &cur_listener->mount_ਬਿੰਦੂ, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(mosquitto_pub_topic_check(cur_listener->mount_ਬਿੰਦੂ) != MOSQ_ERR_SUCCESS){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR,
								"Error: Invalid 'mount_point' value (%s). Does it contain a wildcard character?",
								cur_listener->mount_ਬਿੰਦੂ);
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "notifications")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_bool(&token, "notifications", &cur_bridge->notifications, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "notifications_local_only")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_bool(&token, "notifications_local_only", &cur_bridge->notifications_local_only, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "notification_topic")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_string(&token, "notification_topic", &cur_bridge->notification_topic, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "password") || !strcmp(token, "remote_password")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_string(&token, "bridge remote_password", &cur_bridge->remote_password, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "password_file")){
					REQUIRE_LISTENER_IF_ਪ੍ਰਤੀ_LISTENER(token);
					conf_ਸੈੱਟ_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->password_ਅੰਕੜੇ.password_file);
					ਜੇ(conf__parse_string(&token, "password_file", &cur_security_options->password_ਅੰਕੜੇ.password_file, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "per_listener_settings")){
					OPTION_DEPRECATED(token, "Please see the documentation for how to achieve the same effect.");
					ਜੇ(config->ਪ੍ਰਤੀ_listener_settings){
						/* Once this is set, don't let it be unset. It should be the first config option ideally. */
						ਜਾਰੀ;
					}
					ਜੇ(conf__parse_bool(&token, "per_listener_settings", &config->ਪ੍ਰਤੀ_listener_settings, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(cur_security_options && config->ਪ੍ਰਤੀ_listener_settings){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: per_listener_settings must be set before any other security settings.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "persistence") || !strcmp(token, "retained_persistence")){
					ਜੇ(conf__parse_bool(&token, token, &config->persistence, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "persistence_file")){
					ਜੇ(conf__parse_string(&token, "persistence_file", &config->persistence_file, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "persistence_location")){
					ਜੇ(conf__parse_string(&token, "persistence_location", &config->persistence_location, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "persistent_client_expiration")){
					token = strtok_r(ਨਿਰਾ, " ", &saveptr);
					REQUIRE_NON_ਖਾਲੀ_ਥਾਵਾਂ_OPTION(token, "persistent_client_expiration");

					time_t expiration_mult;

					ਚੋਣ(token[ਲੜੀ_ਲੰਬਾਈ(token)-1]){
						ਹਾਲਤ 's':
							expiration_mult = 1;
							ਰੋਕੋ;
						ਹਾਲਤ 'h':
							expiration_mult = 3600;
							ਰੋਕੋ;
						ਹਾਲਤ 'd':
							expiration_mult = 86400;
							ਰੋਕੋ;
						ਹਾਲਤ 'w':
							expiration_mult = 86400*7;
							ਰੋਕੋ;
						ਹਾਲਤ 'm':
							expiration_mult = 86400*30;
							ਰੋਕੋ;
						ਹਾਲਤ 'y':
							expiration_mult = 86400*365;
							ਰੋਕੋ;
						ਮੂਲ:
							log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
							ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					token[ਲੜੀ_ਲੰਬਾਈ(token)-1] = '\0';
					config->persistent_client_expiration = atoi(token)*expiration_mult;
					ਜੇ(config->persistent_client_expiration <= 0){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "pid_file")){
					ਜੇ(reload){
						ਜਾਰੀ;        /* pid file not valid for reloading. */
					}
					ਜੇ(conf__parse_string(&token, "pid_file", &config->pid_file, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "port")){
					OPTION_DEPRECATED(token, "Please use 'listener' instead.");
					config->local_only = false;
					ਜੇ(reload){
						ਜਾਰੀ;        /* Listeners not valid for reloading. */

					}
					ਜੇ(config__create_default_listener(config, token)){
						ਵਾਪਸ MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					ਜੇ(config->default_listener->ਪੋਰਟ){
						log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Default listener port specified multiple times. Only the latest will be used.");
					}
					ਜੇ(conf__parse_int(&token, "port", &tmp_int, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(tmp_int < 1 || tmp_int > UINT16_MAX){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					config->default_listener->ਪੋਰਟ = (uint16_t)tmp_int;
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "protocol")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(ਨਿਰਾ, " ", &saveptr);
					REQUIRE_NON_ਖਾਲੀ_ਥਾਵਾਂ_OPTION(token, "protocol");

					ਜੇ(!strcmp(token, "mqtt")){
						cur_listener->protocol = mp_mqtt;
						/*
						}else if(!strcmp(token, "mqttsn")){
						    cur_listener->protocol = mp_mqttsn;
						*/
					}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "websockets")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_WEBSOCKETS
						cur_listener->protocol = mp_websockets;
#ਨਹੀਂਤਾਂ
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Websockets support not available.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
#ਜੇ_ਅੰਤ
					}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "http_api")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_HTTP_API
						cur_listener->protocol = mp_http_api;
#ਨਹੀਂਤਾਂ
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: HTTP API support not available.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
#ਜੇ_ਅੰਤ
					}ਨਹੀਂਤਾਂ{
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid 'protocol' value (%s).", token);
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "proxy_protocol_v2_require_tls")){
#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					ਵਾਪਸ MOSQ_ERR_INVAL;
#ਜੇ_ਅੰਤ
					REQUIRE_LISTENER(token);
					ਜੇ(conf__parse_bool(&token, "proxy_protocol_v2_require_tls", &cur_listener->proxy_protocol_v2_require_tls, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "psk_file")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FINAL_WITH_TLS_PSK
					REQUIRE_LISTENER_IF_ਪ੍ਰਤੀ_LISTENER(token);
					conf_ਸੈੱਟ_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ਜੇ(cur_listener && cur_listener->certfile){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_security_options->psk_file);
					ਜੇ(conf__parse_string(&token, "psk_file", &cur_security_options->psk_file, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "psk_hint")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FINAL_WITH_TLS_PSK
					ਜੇ(reload){
						ਜਾਰੀ;        /* PSK file not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ਜੇ(conf__parse_string(&token, "psk_hint", &cur_listener->psk_hint, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "queue_qos0_messages")){
					ਜੇ(conf__parse_bool(&token, token, &config->ਕਤਾਰ_qos0_messages, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "require_certificate")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ਜੇ(conf__parse_bool(&token, "require_certificate", &cur_listener->require_certificate, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "restart_timeout")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					cur_bridge->backoff_cap = 0; /* set backoff to constant mode, unless cap is specified further down */
					token = strtok_r(ਨਿਰਾ, " ", &saveptr);
					REQUIRE_NON_ਖਾਲੀ_ਥਾਵਾਂ_OPTION(token, "restart_timeout");

					cur_bridge->restart_timeout = atoi(token);
					cur_bridge->backoff_ਅਧਾਰ = 0;
					cur_bridge->backoff_cap = 0;
					ਜੇ(cur_bridge->restart_timeout < 1){
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "restart_timeout interval too low, using 1 second.");
						cur_bridge->restart_timeout = 1;
					}ਨਹੀਂਤਾਂ ਜੇ(cur_bridge->restart_timeout > 3600){
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "restart_timeout interval too high, using 3600 seconds.");
						cur_bridge->restart_timeout = 3600;
					}
					token = strtok_r(ਨਿਰਾ, " ", &saveptr);
					ਜੇ(token){
						cur_bridge->backoff_ਅਧਾਰ = cur_bridge->restart_timeout;
						cur_bridge->backoff_cap = atoi(token);
						ਜੇ(cur_bridge->backoff_cap < cur_bridge->backoff_ਅਧਾਰ){
							log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: backoff cap is lower than the base in restart_timeout.");
							ਵਾਪਸ MOSQ_ERR_INVAL;
						}ਨਹੀਂਤਾਂ ਜੇ(cur_bridge->backoff_cap > 7200){
							log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: backoff cap too high, using 7200 seconds.");
							cur_bridge->backoff_cap = 7200;
						}

						token = strtok_r(ਨਿਰਾ, " ", &saveptr);
						ਜੇ(token){
							cur_bridge->stable_connection_period = atoi(token);
							ਜੇ(cur_bridge->stable_connection_period < 0){
								log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: stable connection period cannot be negative.");
								ਵਾਪਸ MOSQ_ERR_INVAL;
							}
						}
					}
					cur_bridge->restart_timeout *= 1000; /* backoff is tracked in ms */
					cur_bridge->backoff_ਅਧਾਰ *= 1000;
					cur_bridge->backoff_cap *= 1000;
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "retain_available")){
					ਜੇ(conf__parse_bool(&token, token, &config->retain_ਉਪਲਬਧ, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "retain_expiry_interval")){
					ਜੇ(conf__parse_int(&token, token, &config->retain_expiry_interval, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(config->retain_expiry_interval < 1){
						log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Error: retain_expiry_interval must be >= 1.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}ਨਹੀਂਤਾਂ ਜੇ(config->retain_expiry_interval > 10000000){
						log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: retain_expiry_interval being capped at 19 years.");
						config->retain_expiry_interval = 10000000;
					}
					config->retain_expiry_interval *= 60;
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "retry_interval")){
					OPTION_UNAVAILABLE(token);
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "round_robin")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_bool(&token, "round_robin", &cur_bridge->ਗੇੜਾ_robin, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "set_tcp_nodelay")){
					ਜੇ(conf__parse_bool(&token, "set_tcp_nodelay", &config->ਸੈੱਟ_tcp_nodelay, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "start_type")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(ਨਿਰਾ, " ", &saveptr);
					REQUIRE_NON_ਖਾਲੀ_ਥਾਵਾਂ_OPTION(token, "start_type");

					ਜੇ(!strcmp(token, "automatic")){
						cur_bridge->ਸ਼ੁਰੂ_type = bst_automatic;
					}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "lazy")){
						cur_bridge->ਸ਼ੁਰੂ_type = bst_lazy;
					}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "manual")){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Manual start_type not supported.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "once")){
						cur_bridge->ਸ਼ੁਰੂ_type = bst_once;
					}ਨਹੀਂਤਾਂ{
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid 'start_type' value in configuration (%s).", token);
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "socket_domain")){
					ਜੇ(reload){
						ਜਾਰੀ;        /* socket_domain not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(ਨਿਰਾ, " ", &saveptr);
					REQUIRE_NON_ਖਾਲੀ_ਥਾਵਾਂ_OPTION(token, "start_type");

					ਜੇ(!strcmp(token, "ipv4")){
						cur_listener->socket_domain = AF_INET;
					}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "ipv6")){
						cur_listener->socket_domain = AF_INET6;
					}ਨਹੀਂਤਾਂ{
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid 'socket_domain' value '%s' in configuration.", token);
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "sys_interval")){
					ਜੇ(conf__parse_int(&token, "sys_interval", &config->sys_interval, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(config->sys_interval < 0 || config->sys_interval > 65535){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid 'sys_interval' value (%d).", config->sys_interval);
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "threshold")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_int(&token, "threshold", &cur_bridge->ਹੱਦ, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(cur_bridge->ਹੱਦ < 1){
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "threshold too low, using 1 message.");
						cur_bridge->ਹੱਦ = 1;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "tls_engine")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
					ਜੇ(reload){
						ਜਾਰੀ;        /* tls_engine not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ਜੇ(conf__parse_string(&token, "tls_engine", &cur_listener->tls_engine, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "tls_engine_kpass_sha1")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
					ਜੇ(reload){
						ਜਾਰੀ;        /* tls_engine not valid for reloading. */

					}
					ਅੱਖਰ *kpass_sha = ਨਿਰਾ, *kpass_sha_bin = ਨਿਰਾ;
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ਜੇ(conf__parse_string(&token, "tls_engine_kpass_sha1", &kpass_sha, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(mosquitto__hex2bin_sha1(kpass_sha, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ **)&kpass_sha_bin) != MOSQ_ERR_SUCCESS){
						mosquitto_FREE(kpass_sha);
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					mosquitto_free(cur_listener->tls_engine_kpass_sha1);
					cur_listener->tls_engine_kpass_sha1 = kpass_sha_bin;
					mosquitto_FREE(kpass_sha);
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "tls_keyform")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
					ਜੇ(reload){
						ਜਾਰੀ;        /* tls_engine not valid for reloading. */

					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ਅੱਖਰ *keyform = ਨਿਰਾ;
					ਜੇ(conf__parse_string(&token, "tls_keyform", &keyform, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					cur_listener->tls_keyform = mosq_k_pem;
					ਜੇ(!strcmp(keyform, "engine")){
						cur_listener->tls_keyform = mosq_k_engine;
					}
					mosquitto_FREE(keyform);
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "tls_version")){
#ਜੇ defined(WITH_TLS)
					ਜੇ(reload){
						ਜਾਰੀ;        /* tls_version not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					ਜੇ(conf__parse_string(&token, "tls_version", &cur_listener->tls_ਸੰਸਕਰਣ, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "topic")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
					ਅੱਖਰ *topic = ਨਿਰਾ;
					ਗਿਣਤੀ mosquitto__bridge_direction direction = bd_ਬਾਹਰਲਾ;
					uint8_t qos = 0;
					ਅੱਖਰ *local_ਅਗੇਤਰ = ਨਿਰਾ, *remote_ਅਗੇਤਰ = ਨਿਰਾ;

					REQUIRE_BRIDGE(token);

					token = strtok_r(ਨਿਰਾ, " ", &saveptr);
					REQUIRE_NON_ਖਾਲੀ_ਥਾਵਾਂ_OPTION(token, "topic");

					// Check if the topic is quoted (e.g. for spaces within topic names), but not the
					// special case of ""
					ਜੇ(token[0] == '"' && token [1] != '"'){
						ਜੇ(strchr(saveptr, '"') == ਨਿਰਾ){
							log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Missing closing quote in topic value (%s).", saveptr);
							ਵਾਪਸ MOSQ_ERR_INVAL;
						}

						ਅੱਖਰ *topic_ਵਿੱਚ_quotes = strtok_r(ਨਿਰਾ, "\"", &saveptr);
						size_t tlen = 1;
						ਜੇ(topic_ਵਿੱਚ_quotes){
							tlen = ਲੜੀ_ਲੰਬਾਈ(topic_ਵਿੱਚ_quotes);
						}

						topic = mosquitto_malloc(ਲੜੀ_ਲੰਬਾਈ(token) + tlen + 1);
						ਜੇ(!topic){
							log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
							ਵਾਪਸ MOSQ_ERR_NOMEM;
						}

						strcpy(topic, token + 1);
						strcat(topic, " ");
						ਜੇ(topic_ਵਿੱਚ_quotes){
							strcat(topic, topic_ਵਿੱਚ_quotes);
						}
					}ਨਹੀਂਤਾਂ{
						topic = mosquitto_strdup(token);
						ਜੇ(!topic){
							log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
							ਵਾਪਸ MOSQ_ERR_NOMEM;
						}
					}

					token = strtok_r(ਨਿਰਾ, " ", &saveptr);
					ਜੇ(token){
						ਜੇ(!strcasecmp(token, "out")){
							direction = bd_ਬਾਹਰਲਾ;
						}ਨਹੀਂਤਾਂ ਜੇ(!strcasecmp(token, "in")){
							direction = bd_ਵਿੱਚ;
						}ਨਹੀਂਤਾਂ ਜੇ(!strcasecmp(token, "both")){
							direction = bd_both;
						}ਨਹੀਂਤਾਂ{
							log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid bridge topic direction '%s'.", token);
							mosquitto_FREE(topic);
							ਵਾਪਸ MOSQ_ERR_INVAL;
						}
						token = strtok_r(ਨਿਰਾ, " ", &saveptr);
						ਜੇ(token){
							ਜੇ(token[0] == '#'){
								(ਖਾਲੀ)strtok_r(ਨਿਰਾ, "", &saveptr);
							}
							qos = (uint8_t)atoi(token);
							ਜੇ(qos > 2){
								log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid bridge QoS level '%s'.", token);
								mosquitto_FREE(topic);
								ਵਾਪਸ MOSQ_ERR_INVAL;
							}

							token = strtok_r(ਨਿਰਾ, " ", &saveptr);
							ਜੇ(token){
								ਜੇ(!strcmp(token, "\"\"") || token[0] == '#'){
									local_ਅਗੇਤਰ = ਨਿਰਾ;
									ਜੇ(token[0] == '#'){
										(ਖਾਲੀ)strtok_r(ਨਿਰਾ, "", &saveptr);
									}
								}ਨਹੀਂਤਾਂ{
									local_ਅਗੇਤਰ = token;
								}

								token = strtok_r(ਨਿਰਾ, " ", &saveptr);
								ਜੇ(token){
									ਜੇ(!strcmp(token, "\"\"") || token[0] == '#'){
										remote_ਅਗੇਤਰ = ਨਿਰਾ;
									}ਨਹੀਂਤਾਂ{
										remote_ਅਗੇਤਰ = token;
									}
								}
							}
						}
					}

					ਜੇ(bridge_ਜੋੜੋ_topic(cur_bridge, topic, direction, qos, local_ਅਗੇਤਰ, remote_ਅਗੇਤਰ)){
						mosquitto_FREE(topic);
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(topic);
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "max_topic_alias")){
					REQUIRE_LISTENER(token);
					ਜੇ(conf__parse_int(&token, "max_topic_alias", &tmp_int, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias' value in configuration.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias = (uint16_t)tmp_int;
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "max_topic_alias_broker")){
					REQUIRE_LISTENER(token);
					ਜੇ(conf__parse_int(&token, "max_topic_alias_broker", &tmp_int, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias_broker' value in configuration.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias_broker = (uint16_t)tmp_int;
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "try_private")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_bool(&token, "try_private", &cur_bridge->try_ਨਿੱਜੀ, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "upgrade_outgoing_qos")){
					ਜੇ(conf__parse_bool(&token, token, &config->upgrade_outgoing_qos, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "use_identity_as_username")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ਜੇ(conf__parse_bool(&token, "use_identity_as_username", &cur_listener->use_ਪਛਾਣ_ਤੱਤ_as_username, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "use_subject_as_username")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ਜੇ(conf__parse_bool(&token, "use_subject_as_username", &cur_listener->use_subject_as_username, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "user")){
					ਜੇ(reload){
						ਜਾਰੀ;        /* Drop privileges user not valid for reloading. */
					}
					mosquitto_FREE(config->user);
					ਜੇ(conf__parse_string(&token, "user", &config->user, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "use_username_as_clientid")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ਜੇ(conf__parse_bool(&token, "use_username_as_clientid", &cur_listener->use_username_as_clientid, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "username") || !strcmp(token, "remote_username")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ਜੇ(conf__parse_string(&token, "bridge remote_username", &cur_bridge->remote_username, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "websockets_log_level")){
#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					ਜੇ(conf__parse_int(&token, "websockets_log_level", &config->websockets_log_level, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "websockets_headers_size") || !strcmp(token, "packet_buffer_size")){
					ਜੇ(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					ਜੇ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Error: Packet buffer size must be between 0 and 65535 inclusive.");
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					config->packet_ਬਫਰ_ਮਾਪ = (uint16_t)tmp_int;
				}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(token, "websockets_origin")){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_WEBSOCKETS
#  ਜੇ LWS_LIBRARY_ਸੰਸਕਰਣ_ਸੰਖਿਆ >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN
					REQUIRE_LISTENER(token);
					ws_origins = mosquitto_realloc(cur_listener->ws_origins, ਆਕਾਰ(ਅੱਖਰ *)*(size_t)(cur_listener->ws_origin_ਗਿਣਤੀ+1));
					ਜੇ(ws_origins == ਨਿਰਾ){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
						ਵਾਪਸ MOSQ_ERR_NOMEM;
					}
					ws_origins[cur_listener->ws_origin_ਗਿਣਤੀ] = ਨਿਰਾ;
					cur_listener->ws_origins = ws_origins;
					ਜੇ(conf__parse_string(&token, "websockets_origin", &ws_origins[cur_listener->ws_origin_ਗਿਣਤੀ], &saveptr)){
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}
					cur_listener->ws_origin_ਗਿਣਤੀ++;
#  ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: websockets_origin support not available, libwebsockets version is too old.");
#  endif
#ਨਹੀਂਤਾਂ
					log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Websockets support not available.");
#ਜੇ_ਅੰਤ
				}ਨਹੀਂਤਾਂ{
					log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unknown configuration variable '%s'.", token);
					ਵਾਪਸ MOSQ_ERR_INVAL;
				}
			}
		}
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ config__read_file(ਬਣਤਰ mosquitto__config *config, bool reload, ਅਚਲ ਅੱਖਰ *file, ਬਣਤਰ config_recurse *cr, ਪੂਰਨ level, ਪੂਰਨ *lineno)
{
	ਪੂਰਨ rc;
	FILE *fptr = ਨਿਰਾ;
	ਅੱਖਰ *buf;
	ਪੂਰਨ buflen;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
	DIR *dir;
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
	dir = opendir(file);
	ਜੇ(dir){
		closedir(dir);
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Config file '%s' is a directory.", file);
		ਵਾਪਸ 1;
	}
#ਜੇ_ਅੰਤ

	fptr = mosquitto_fopen(file, "rt", false);
	ਜੇ(!fptr){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to open config file '%s'.", file);
		ਵਾਪਸ 1;
	}

	buflen = 1000;
	buf = mosquitto_malloc((size_t)buflen);
	ਜੇ(!buf){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
		fclose(fptr);
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	rc = config__read_file_core(config, reload, cr, level, lineno, fptr, &buf, &buflen);
	mosquitto_FREE(buf);
	fclose(fptr);

	ਵਾਪਸ rc;
}


ਸਥਿਰ ਪੂਰਨ config__check_proxy(ਬਣਤਰ mosquitto__config *config)
{
	ਲਈ(ਪੂਰਨ i=0; i<config->listener_ਗਿਣਤੀ; i++){
		ਬਣਤਰ mosquitto__listener *l = &config->listeners[i];

		ਜੇ(l->enable_proxy_protocol == 2){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
			ਜੇ(l->use_subject_as_username){
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: use_subject_as_username cannot be used with `enable_proxy_protocol 2`.");
				ਵਾਪਸ MOSQ_ERR_INVAL;
			}

			ਜੇ(l->certfile || l->keyfile){
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: certfile and keyfile cannot be used with `enable_proxy_protocol 2`.");
				ਵਾਪਸ MOSQ_ERR_INVAL;
			}
#ਜੇ_ਅੰਤ
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ config__check(ਬਣਤਰ mosquitto__config *config)
{
	/* Checks that are easy to make after the config has been loaded. */

	ਅਚਲ ਅੱਖਰ *id_ਅਗੇਤਰ;
	ਪੂਰਨ id_ਅਗੇਤਰ_len;
	ਜੇ(config->security_options.auto_id_ਅਗੇਤਰ){
		id_ਅਗੇਤਰ = config->security_options.auto_id_ਅਗੇਤਰ;
		id_ਅਗੇਤਰ_len = config->security_options.auto_id_ਅਗੇਤਰ_len;
	}ਨਹੀਂਤਾਂ{
		id_ਅਗੇਤਰ = "auto-";
		id_ਅਗੇਤਰ_len = (ਪੂਰਨ)ਲੜੀ_ਲੰਬਾਈ("auto-");
	}

	/* Default to auto_id_prefix = 'auto-' if none set. */
	ਲਈ(ਪੂਰਨ i=0; i<config->listener_ਗਿਣਤੀ; i++){
		ਜੇ(!config->listeners[i].security_options->auto_id_ਅਗੇਤਰ){
			config->listeners[i].security_options->auto_id_ਅਗੇਤਰ = mosquitto_strdup(id_ਅਗੇਤਰ);
			ਜੇ(!config->listeners[i].security_options->auto_id_ਅਗੇਤਰ){
				ਵਾਪਸ MOSQ_ERR_NOMEM;
			}
			config->listeners[i].security_options->auto_id_ਅਗੇਤਰ_len = (uint16_t)id_ਅਗੇਤਰ_len;
		}
	}

	ਵਾਪਸ config__check_proxy(config);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE


ਸਥਿਰ ਪੂਰਨ config__check_bridges(ਬਣਤਰ mosquitto__config *config)
{
	ਬਣਤਰ mosquitto__bridge *bridge1, *bridge2;
	ਅੱਖਰ hostname[256];
	size_t len;

	/* Check for bridge duplicate local_clientid, need to generate missing IDs
	 * first. */
	ਲਈ(ਪੂਰਨ i=0; i<config->bridge_ਗਿਣਤੀ; i++){
		bridge1 = config->bridges[i];

		ਜੇ(!bridge1->remote_clientid){
			ਜੇ(!gethostname(hostname, 256)){
				len = ਲੜੀ_ਲੰਬਾਈ(hostname) + ਲੜੀ_ਲੰਬਾਈ(bridge1->name) + 2;
				bridge1->remote_clientid = mosquitto_malloc(len);
				ਜੇ(!bridge1->remote_clientid){
					ਵਾਪਸ MOSQ_ERR_NOMEM;
				}
				snprintf(bridge1->remote_clientid, len, "%s.%s", hostname, bridge1->name);
			}ਨਹੀਂਤਾਂ{
				ਵਾਪਸ 1;
			}
		}

		ਜੇ(!bridge1->local_clientid){
			len = ਲੜੀ_ਲੰਬਾਈ(bridge1->remote_clientid) + ਲੜੀ_ਲੰਬਾਈ("local.") + 2;
			bridge1->local_clientid = mosquitto_malloc(len);
			ਜੇ(!bridge1->local_clientid){
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
				ਵਾਪਸ MOSQ_ERR_NOMEM;
			}
			snprintf(bridge1->local_clientid, len, "local.%s", bridge1->remote_clientid);
		}
	}

	ਲਈ(ਪੂਰਨ i=0; i<config->bridge_ਗਿਣਤੀ; i++){
		bridge1 = config->bridges[i];
		ਲਈ(ਪੂਰਨ j=i+1; j<config->bridge_ਗਿਣਤੀ; j++){
			bridge2 = config->bridges[j];
			ਜੇ(!strcmp(bridge1->local_clientid, bridge2->local_clientid)){
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Bridge local_clientid "
						"'%s' is not unique. Try changing or setting the "
						"local_clientid value for one of the bridges.",
						bridge1->local_clientid);
				ਵਾਪਸ MOSQ_ERR_INVAL;
			}
		}
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
	/* Check for missing TLS cafile/capath/certfile/keyfile */
	ਲਈ(ਪੂਰਨ i=0; i<config->listener_ਗਿਣਤੀ; i++){
		bool cafile = !!config->listeners[i].cafile;
		bool capath = !!config->listeners[i].capath;
		bool certfile = !!config->listeners[i].certfile;
		bool keyfile = !!config->listeners[i].keyfile;

		ਜੇ((certfile && !keyfile) || (!certfile && keyfile)){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Both certfile and keyfile must be provided to enable a TLS listener.");
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
		ਜੇ(cafile && !certfile){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: cafile specified without certfile and keyfile.");
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
		ਜੇ(capath && !certfile){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: capath specified without certfile and keyfile.");
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
	}
#ਜੇ_ਅੰਤ
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}
#ਜੇ_ਅੰਤ


ਸਥਿਰ ਪੂਰਨ conf__parse_bool(ਅੱਖਰ **token, ਅਚਲ ਅੱਖਰ *name, bool *ਮੁੱਲ, ਅੱਖਰ **saveptr)
{
	*token = strtok_r(ਨਿਰਾ, " ", saveptr);
	ਜੇ(*token){
		ਜੇ(!strcmp(*token, "false") || !strcmp(*token, "0")){
			*ਮੁੱਲ = false;
		}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(*token, "true") || !strcmp(*token, "1")){
			*ਮੁੱਲ = true;
		}ਨਹੀਂਤਾਂ{
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Invalid '%s' value (%s).", name, *token);
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
	}ਨਹੀਂਤਾਂ{
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ conf__parse_int(ਅੱਖਰ **token, ਅਚਲ ਅੱਖਰ *name, ਪੂਰਨ *ਮੁੱਲ, ਅੱਖਰ **saveptr)
{
	*token = strtok_r(ਨਿਰਾ, " ", saveptr);
	ਜੇ(*token){
		ਅੱਖਰ *endptr = ਨਿਰਾ;
		ਲੰਮਾ l = strtol(*token, &endptr, 0);
		ਜੇ(endptr == *token || endptr[0] != '\0'){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: '%s' value not a number.", name);
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
		ਜੇ(l > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ || l < INT_MIN){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: '%s' value out of range.", name);
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
		*ਮੁੱਲ = (ਪੂਰਨ)l;
	}ਨਹੀਂਤਾਂ{
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ conf__parse_ssize_t(ਅੱਖਰ **token, ਅਚਲ ਅੱਖਰ *name, ssize_t *ਮੁੱਲ, ਅੱਖਰ **saveptr)
{
	*token = strtok_r(ਨਿਰਾ, " ", saveptr);
	ਜੇ(*token){
		*ਮੁੱਲ = atol(*token);
	}ਨਹੀਂਤਾਂ{
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ conf__parse_string(ਅੱਖਰ **token, ਅਚਲ ਅੱਖਰ *name, ਅੱਖਰ **ਮੁੱਲ, ਅੱਖਰ **saveptr)
{
	size_t tlen;

	*token = strtok_r(ਨਿਰਾ, "", saveptr);
	ਜੇ(*token){
		ਜੇ(*ਮੁੱਲ){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Duplicate '%s' value in configuration.", name);
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
		/* Deal with multiple spaces at the beginning of the string. */
		*token = mosquitto_trimblanks(*token);
		ਜੇ(ਲੜੀ_ਲੰਬਾਈ(*token) == 0){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}

		tlen = ਲੜੀ_ਲੰਬਾਈ(*token);
		ਜੇ(tlen > UINT16_MAX){
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
		ਜੇ(mosquitto_validate_utf8(*token, (uint16_t)tlen)){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Malformed UTF-8 in configuration.");
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
		*ਮੁੱਲ = mosquitto_strdup(*token);
		ਜੇ(!*ਮੁੱਲ){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}
	}ਨਹੀਂਤਾਂ{
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}
