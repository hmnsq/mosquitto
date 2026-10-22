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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <limits.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <errno.h>

#ifdef WIN32
#ଅନ୍ୟଥା
#  include <dirent.h>
#  include <strings.h>
#endif

#ifndef WIN32
#  include <netdb.h>
#  include <sys/socket.h>
#ଅନ୍ୟଥା
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#ଯଦି !defined(WIN32) && !defined(__CYGWIN__)
#  include <syslog.h>
#endif

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "tls_mosq.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"

#ଅନ୍ତର୍ଭାଵ "utlist.h"

ସଂରଚନା config_recurse {
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_dest;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_dest_set;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_type;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_type_set;
};

#ଯଦି defined(WIN32) || defined(__CYGWIN__)
#ଅନ୍ତର୍ଭାଵ <windows.h>
ବାହ୍ୟ SERVICE_STATUS_HANDLE service_handle;
#endif


#define REQUIRE_LISTENER(A) \
		କର{ \
			ଯଦି(cur_listener == ଖାଲି){ \
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				ଫେରନ୍ତୁ MOSQ_ERR_INVAL; \
			} \
		}ଯେତେବେଳେ(0)

#define REQUIRE_LISTENER_OR_DEFAULT_LISTENER(A) \
		କର{ \
			ଯଦି(cur_listener == ଖାଲି){ \
				ଯଦି(config__create_default_listener(config, (A))){ \
					ଫେରନ୍ତୁ MOSQ_ERR_NOMEM; \
				} \
				cur_listener = config->default_listener; \
			} \
		}ଯେତେବେଳେ(0)

#define REQUIRE_LISTENER_IF_PER_LISTENER(A) \
		କର{ \
			ଯଦି(config->per_listener_settings == true && cur_listener == ଖାଲି){ \
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				ଫେରନ୍ତୁ MOSQ_ERR_INVAL; \
			} \
		}ଯେତେବେଳେ(0)

#define REQUIRE_NON_DEFAULT_LISTENER(A) \
		କର{ \
			ଯଦି(cur_listener == config->default_listener || cur_listener == ଖାଲି){ \
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				ଫେରନ୍ତୁ MOSQ_ERR_INVAL; \
			} \
		}ଯେତେବେଳେ(0)

#define REQUIRE_BRIDGE(A) \
		କର{ \
			ଯଦି(cur_bridge == ଖାଲି){ \
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: The '%s' option requires a bridge to be defined first.", (A)); \
				ଫେରନ୍ତୁ MOSQ_ERR_INVAL; \
			} \
		}ଯେତେବେଳେ(0)

#define REQUIRE_PLUGIN(A) \
		କର{ \
			ଯଦି(cur_plugin == ଖାଲି){ \
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: The '%s' option requires plugin/global_plugin/plugin_load to be defined first.", (A)); \
				ଫେରନ୍ତୁ MOSQ_ERR_INVAL; \
			} \
		}ଯେତେବେଳେ(0)

#define OPTION_DEPRECATED(A, B) \
		log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "The '%s' option is now deprecated and will be removed in version 3.0. %s", (A), (B))

#define OPTION_UNAVAILABLE(A) \
		log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: The '%s' option is no longer available.", (A));

#define REQUIRE_NON_EMPTY_OPTION(A, B) \
		କର{ \
			ଯଦି(!(A)){ \
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", (B)); \
				ଫେରନ୍ତୁ MOSQ_ERR_INVAL; \
			} \
		}ଯେତେବେଳେ(0)

#define PER_LISTENER_ALTERNATIVE(A, B) \
		ଯଦି(config->per_listener_settings){ \
			log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "You are using the '%s' option with 'per_listener_settings true'. Please replace this with '%s'.", A, B); \
		} \


#ifdef FINAL_WITH_TLS_PSK
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A) \
		କର{ \
			ଯଦି(cur_bridge->tls_psk_identity || cur_bridge->tls_psk){ \
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and psk encryption in a single bridge.", (A)); \
				ଫେରନ୍ତୁ MOSQ_ERR_INVAL; \
			} \
		}ଯେତେବେଳେ(0)

#  define REQUIRE_BRIDGE_NO_X509(A) \
		କର{ \
			ଯଦି(cur_bridge->tls_cafile || cur_bridge->tls_capath || cur_bridge->tls_certfile || cur_bridge->tls_keyfile){ \
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and identity encryption in a single bridge.", (A)); \
				ଫେରନ୍ତୁ MOSQ_ERR_INVAL; \
			} \
		}ଯେତେବେଳେ(0)

#ଅନ୍ୟଥା
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A)
#  define REQUIRE_BRIDGE_NO_X509(A)
#endif

ସ୍ଥିର ସଂରଚନା mosquitto__security_options *cur_security_options = ଖାଲି;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf__parse_bool(ଅକ୍ଷର **token, ଅଚଳ ଅକ୍ଷର *name, bool *ମୂଲ୍ୟ, ଅକ୍ଷର **saveptr);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf__parse_int(ଅକ୍ଷର **token, ଅଚଳ ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ମୂଲ୍ୟ, ଅକ୍ଷର **saveptr);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf__parse_ssize_t(ଅକ୍ଷର **token, ଅଚଳ ଅକ୍ଷର *name, ssize_t *ମୂଲ୍ୟ, ଅକ୍ଷର **saveptr);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf__parse_string(ଅକ୍ଷର **token, ଅଚଳ ଅକ୍ଷର *name, ଅକ୍ଷର **ମୂଲ୍ୟ, ଅକ୍ଷର **saveptr);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା config__read_file(ସଂରଚନା mosquitto__config *config, bool reload, ଅଚଳ ଅକ୍ଷର *file, ସଂରଚନା config_recurse *config_tmp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା level, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *lineno);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା config__check(ସଂରଚନା mosquitto__config *config);
ସ୍ଥିର ଶୂନ୍ୟ config__cleanup_plugins(ଶୂନ୍ୟ);
#ifdef WITH_BRIDGE
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା config__check_bridges(ସଂରଚନା mosquitto__config *config);
#endif


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା config__add_listener(ସଂରଚନା mosquitto__config *config)
{
	ସଂରଚନା mosquitto__listener *listener;
	ସଂରଚନା mosquitto__listener *new_listeners;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା def_listener = -1;

	ଯଦି(config->default_listener){
		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<config->listener_ଗଣନା; i++){
			ଯଦି(&config->listeners[i] == config->default_listener){
				def_listener = i;
				ଭାଙ୍ଗ;
			}
		}
	}
	new_listeners = mosquitto_realloc(config->listeners, ଆକାର(ସଂରଚନା mosquitto__listener)*(size_t)(config->listener_ଗଣନା+1));
	ଯଦି(!new_listeners){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}
	config->listeners = new_listeners;
	listener = &config->listeners[config->listener_ଗଣନା];
	memset(listener, 0, ଆକାର(ସଂରଚନା mosquitto__listener));
	listener->security_options = mosquitto_calloc(1, ଆକାର(ସଂରଚନା mosquitto__security_options));
	ଯଦି(!listener->security_options){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}
	ଯଦି(def_listener != -1){
		config->default_listener = &config->listeners[def_listener];
	}
	config->listener_ଗଣନା++;

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା config__create_default_listener(ସଂରଚନା mosquitto__config *config, ଅଚଳ ଅକ୍ଷର *option_name)
{
	ଯଦି(config->default_listener){
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}
	log__printf(ଖାଲି, MOSQ_LOG_INFO, "Creating default listener due to '%s' option.", option_name);
	log__printf(ଖାଲି, MOSQ_LOG_INFO, "It is best practice to define a 'listener' first. Using the '%s' option without a listener will be disabled in the future.", option_name);
	ଯଦି(config__add_listener(config)){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	config->default_listener = &config->listeners[config->listener_ଗଣନା-1];
	listener__set_defaults(config->default_listener);
	config->default_listener->port = 1883;

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ଶୂନ୍ୟ conf__set_cur_security_options(ସଂରଚନା mosquitto__config *config, ସଂରଚନା mosquitto__listener **cur_listener, ସଂରଚନା mosquitto__security_options **security_options, ଅଚଳ ଅକ୍ଷର *option_name)
{
	ଯଦି(config->per_listener_settings){
		ଯଦି(*cur_listener == ଖାଲି){
			ଯଦି(config__create_default_listener(config, option_name)){
				ଫେରନ୍ତୁ;
			}
			*cur_listener = config->default_listener;
		}
		(*security_options) = (*cur_listener)->security_options;
	}ଅନ୍ୟଥା{
		(*security_options) = &config->security_options;
	}
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf__attempt_resolve(ଅଚଳ ଅକ୍ଷର *host, ଅଚଳ ଅକ୍ଷର *text, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା log, ଅଚଳ ଅକ୍ଷର *ସନ୍ଦେଶ)
{
	ସଂରଚନା addrinfo gai_hints;
	ସଂରଚନା addrinfo *gai_res;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	memset(&gai_hints, 0, ଆକାର(ସଂରଚନା addrinfo));
	gai_hints.ai_family = AF_UNSPEC;
	gai_hints.ai_socktype = SOCK_STREAM;
	gai_res = ଖାଲି;
	rc = getaddrinfo(host, ଖାଲି, &gai_hints, &gai_res);
	ଯଦି(gai_res){
		freeaddrinfo(gai_res);
	}
	ଯଦି(rc != 0){
#ifndef WIN32
		ଯଦି(rc == EAI_SYSTEM){
			ଯଦି(errno == ENOENT){
				log__printf(ଖାଲି, log, "%s: Unable to resolve %s %s.", ସନ୍ଦେଶ, text, host);
			}ଅନ୍ୟଥା{
				log__printf(ଖାଲି, log, "%s: Error resolving %s: %s.", ସନ୍ଦେଶ, text, strerror(errno));
			}
		}ଅନ୍ୟଥା{
			log__printf(ଖାଲି, log, "%s: Error resolving %s: %s.", ସନ୍ଦେଶ, text, gai_strerror(rc));
		}
#ଅନ୍ୟଥା
		ଯଦି(rc == WSAHOST_NOT_FOUND){
			log__printf(ଖାଲି, log, "%s: Error resolving %s.", ସନ୍ଦେଶ, text);
		}
#endif
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ଶୂନ୍ୟ config__init_reload(ସଂରଚନା mosquitto__config *config)
{
	/* Set defaults */
	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<config->listener_ଗଣନା; i++){
		listener__set_defaults(&config->listeners[i]);
	}

	config->local_only = true;
	config->allow_duplicate_messages = true;

	mosquitto_FREE(config->security_options.acl_data.acl_file);
	mosquitto_FREE(config->security_options.password_data.password_file);
	mosquitto_FREE(config->security_options.psk_file);

	config->security_options.allow_anonymous = -1;
	config->security_options.allow_zero_length_clientid = true;
	config->security_options.auto_id_prefix = ଖାଲି;
	config->security_options.auto_id_prefix_len = 0;

	config->autosave_interval = 1800;
	config->autosave_on_changes = false;

	mosquitto_FREE(config->clientid_prefixes);

	config->connection_messages = true;
	config->clientid_prefixes = ଖାଲି;
	config->per_listener_settings = false;
	ଯଦି(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = ଖାଲି;
	}
	mosquitto_FREE(config->log_file);

#ଯଦି defined(WIN32) || defined(__CYGWIN__)
	ଯଦି(service_handle){
		/* This is running as a Windows service. Default to no logging. Using
		 * stdout/stderr is forbidden because the first clients to connect will
		 * get log information sent to them for some reason. */
		config->log_dest = MQTT3_LOG_NONE;
	}ଅନ୍ୟଥା{
		config->log_dest = MQTT3_LOG_STDERR;
	}
#ଅନ୍ୟଥା
	config->log_facility = LOG_DAEMON;
	config->log_dest = MQTT3_LOG_STDERR | MQTT3_LOG_DLT;
	ଯଦି(db.quiet){
		config->log_type = 0;
	}ଅନ୍ୟଥା ଯଦି(db.verbose){
		config->log_type = UINT_MAX;
	}ଅନ୍ୟଥା{
		config->log_type = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;
	}
#endif
	config->log_timestamp = true;
	mosquitto_FREE(config->log_timestamp_format);
	config->global_max_clients = -1;
	config->global_max_connections = -1;
	config->log_timestamp_format = ଖାଲି;
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


ସ୍ଥିର ଶୂନ୍ୟ config__cleanup_plugin_config(mosquitto_plugin_id_t *plugin)
{
	mosquitto_FREE(plugin->config.path);
	mosquitto_FREE(plugin->config.name);

	ଯଦି(plugin->config.options){
		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା j=0; j<plugin->config.option_ଗଣନା; j++){
			mosquitto_FREE(plugin->config.options[j].ଚାବି);
			mosquitto_FREE(plugin->config.options[j].ମୂଲ୍ୟ);
		}
		mosquitto_FREE(plugin->config.options);
		plugin->config.option_ଗଣନା = 0;
	}
	mosquitto_FREE(plugin->config.security_options);
	mosquitto_FREE(plugin);
}


ସ୍ଥିର ଶୂନ୍ୟ config__cleanup_plugins(ଶୂନ୍ୟ)
{
	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<db.plugin_ଗଣନା; i++){
		config__cleanup_plugin_config(db.plugins[i]);
	}
	mosquitto_FREE(db.plugins);
}


ଶୂନ୍ୟ config__init(ସଂରଚନା mosquitto__config *config)
{
	memset(config, 0, ଆକାର(ସଂରଚନା mosquitto__config));
	config__init_reload(config);

	config->daemon = false;
}


ଶୂନ୍ୟ config__cleanup(ସଂରଚନା mosquitto__config *config)
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
	ଯଦି(config->listeners){
		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<config->listener_ଗଣନା; i++){
			mosquitto_FREE(config->listeners[i].host);
			mosquitto_FREE(config->listeners[i].bind_interface);
			mosquitto_FREE(config->listeners[i].mount_point);
			mosquitto_FREE(config->listeners[i].socks);
			ଯଦି(config->listeners[i].security_options){
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
#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			ଯଦି(!config->listeners[i].ws_context) /* libwebsockets frees its own SSL_CTX */
#endif
			{
				SSL_CTX_free(config->listeners[i].ssl_ctx);
				config->listeners[i].ssl_ctx = ଖାଲି;
			}
#endif
#ଯଦି defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
			mosquitto_FREE(config->listeners[i].http_dir);
#endif
#ifdef WITH_WEBSOCKETS
			ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା j=0; j<config->listeners[i].ws_origin_ଗଣନା; j++){
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
	ଯଦି(config->bridges){
		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<config->bridge_ଗଣନା; i++){
			config__bridge_cleanup(config->bridges[i]);
		}
		mosquitto_FREE(config->bridges);
	}
#endif
	config__cleanup_plugins();

	ଯଦି(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = ଖାଲି;
	}
	ଯଦି(config->log_file){
		mosquitto_FREE(config->log_file);
		config->log_file = ଖାଲି;
	}
}

#ifdef WITH_BRIDGE


ଶୂନ୍ୟ config__bridge_cleanup(ସଂରଚନା mosquitto__bridge *bridge)
{
	ଯଦି(bridge == ଖାଲି){
		ଫେରନ୍ତୁ;
	}

	mosquitto_FREE(bridge->name);
	ଯଦି(bridge->addresses){
		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<bridge->address_ଗଣନା; i++){
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
	ଯଦି(bridge->topics){
		ସଂରଚନା mosquitto__bridge_topic *cur_topic, *topic_tmp;

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


ସ୍ଥିର ଶୂନ୍ୟ print_version(ଶୂନ୍ୟ)
{
	printf("mosquitto %s\n", VERSION);
	printf("Copyright © 2025 Roger Light.\n");
	printf("License EPL-2.0 OR BSD-3-Clause.\n");
}


ସ୍ଥିର ଶୂନ୍ୟ print_usage(ଶୂନ୍ୟ)
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


ପୂର୍ଣ୍ଣସଂଖ୍ୟା config__parse_args(ସଂରଚନା mosquitto__config *config, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର *argv[])
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା port_tmp;

	ପାଇଁ(i=1; i<argc; i++){
		ଯଦି(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--config-file")){
			ଯଦି(i<argc-1){
				db.config_file = argv[i+1];

				ଯଦି(config__read(config, false)){
					ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
				}
			}ଅନ୍ୟଥା{
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: -c argument given, but no config file specified.");
				ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
			}
			i++;
		}ଅନ୍ୟଥା ଯଦି(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--daemon")){
			config->daemon = true;
		}ଅନ୍ୟଥା ଯଦି(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
			print_usage();
			ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
		}ଅନ୍ୟଥା ଯଦି(!strcmp(argv[i], "--version")){
			print_version();
			ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
		}ଅନ୍ୟଥା ଯଦି(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port")){
			ଯଦି(i<argc-1){
				port_tmp = atoi(argv[i+1]);
				ଯଦି(port_tmp<1 || port_tmp>UINT16_MAX){
					log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid port specified (%d).", port_tmp);
					ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
				}ଅନ୍ୟଥା{
					ଯଦି(config->cmd_port_ଗଣନା == CMD_PORT_LIMIT){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Only %d ports can be specified on the command line.", CMD_PORT_LIMIT);
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					config->cmd_port[config->cmd_port_ଗଣନା] = (uint16_t)port_tmp;
					config->cmd_port_ଗଣନା++;
				}
			}ଅନ୍ୟଥା{
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: -p argument given, but no port specified.");
				ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
			}
			i++;
		}ଅନ୍ୟଥା ଯଦି(!strcmp(argv[i], "--tls-keylog")){
#ifdef WITH_TLS
			ଯଦି(i<argc-1){
				db.tls_keylog = mosquitto_strdup(argv[i+1]);
				ଯଦି(db.tls_keylog == ଖାଲି){
					log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
					ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
				}
			}ଅନ୍ୟଥା{
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: --tls-keylog argument given, but no file specified.");
				ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
			}
			i++;
#ଅନ୍ୟଥା
			fprintf(stderr, "Error: TLS support not available so --tls-keylog is not available.\n");
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
#endif
		}ଅନ୍ୟଥା ଯଦି(!strcmp(argv[i], "-q") || !strcmp(argv[i], "--quiet")){
			db.quiet = true;
		}ଅନ୍ୟଥା ଯଦି(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")){
			db.verbose = true;
		}ଅନ୍ୟଥା ଯଦି(!strcmp(argv[i], "--test-config")){
			config->test_configuration = true;
		}ଅନ୍ୟଥା{
			fprintf(stderr, "Error: Unknown option '%s'.\n", argv[i]);
			print_usage();
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
	}

	/* Default to drop to mosquitto user if we are privileged and no user specified. */
	ଯଦି(!config->user){
		config->user = mosquitto_strdup("mosquitto");
		ଯଦି(config->user == ଖାଲି){
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}
	}
	ଯଦି(db.quiet){
		config->log_type = 0;
	}ଅନ୍ୟଥା ଯଦି(db.verbose){
		config->log_type = UINT_MAX;
	}

	ଯଦି(getenv("MOSQUITTO_PERSISTENCE_LOCATION")){
		mosquitto_FREE(config->persistence_location);
		config->persistence_location = mosquitto_strdup(getenv("MOSQUITTO_PERSISTENCE_LOCATION"));
		ଯଦି(!config->persistence_location){
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}
	}
	ଫେରନ୍ତୁ config__check(config);
}


ସ୍ଥିର ଶୂନ୍ୟ config__copy(ସଂରଚନା mosquitto__config *src, ସଂରଚନା mosquitto__config *dest)
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
	dest->security_options.plugin_ଗଣନା = src->security_options.plugin_ଗଣନା;
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

#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	dest->websockets_log_level = src->websockets_log_level;
#endif

#ifdef WITH_BRIDGE
	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<dest->bridge_ଗଣନା; i++){
		ଯଦି(dest->bridges[i]){
			config__bridge_cleanup(dest->bridges[i]);
		}
	}
	mosquitto_FREE(dest->bridges);
	dest->bridges = src->bridges;
	dest->bridge_ଗଣନା = src->bridge_ଗଣନା;
#endif
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା config__read(ସଂରଚନା mosquitto__config *config, bool reload)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = MOSQ_ERR_SUCCESS;
	ସଂରଚନା config_recurse cr;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno = 0;
#ifdef WITH_PERSISTENCE
	size_t len;
#endif
	ସଂରଚନା mosquitto__config config_reload;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	ଯଦି(reload){
		memset(&config_reload, 0, ଆକାର(ସଂରଚନା mosquitto__config));
	}

	cr.log_dest = MQTT3_LOG_NONE;
	cr.log_dest_set = 0;
	cr.log_type = MOSQ_LOG_NONE;
	cr.log_type_set = 0;

	ଯଦି(!db.config_file){
		ଫେରନ୍ତୁ 0;
	}

	ଯଦି(reload){
		/* Re-initialise appropriate config vars to default for reload. */
		config__init_reload(&config_reload);
		config_reload.listeners = config->listeners;
		config_reload.listener_ଗଣନା = config->listener_ଗଣନା;
		cur_security_options = ଖାଲି;
		rc = config__read_file(&config_reload, reload, db.config_file, &cr, 0, &lineno);
	}ଅନ୍ୟଥା{
		rc = config__read_file(config, reload, db.config_file, &cr, 0, &lineno);
	}
	ଯଦି(rc){
		ଯଦି(lineno > 0){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error found at %s:%d.", db.config_file, lineno);
		}
		ଫେରନ୍ତୁ rc;
	}

	ଯଦି(reload){
		config__copy(&config_reload, config);
	}

	/* If auth/access options are set and allow_anonymous not explicitly set, disallow anon. */
	ଯଦି(config->local_only == false){
		ଯଦି(config->per_listener_settings){
			ପାଇଁ(i=0; i<config->listener_ଗଣନା; i++){
				/* Default option if no security options set */
				ଯଦି(config->listeners[i].security_options->allow_anonymous == -1){
					config->listeners[i].security_options->allow_anonymous = false;
				}
			}
		}ଅନ୍ୟଥା{
			ଯଦି(config->security_options.allow_anonymous == -1){
				config->security_options.allow_anonymous = false;
			}
		}
	}
#ifdef WITH_PERSISTENCE
	ଯଦି(config->persistence){
		ଯଦି(!config->persistence_file){
			config->persistence_file = mosquitto_strdup("mosquitto.db");
			ଯଦି(!config->persistence_file){
				ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
			}
		}
		mosquitto_FREE(config->persistence_filepath);
		ଯଦି(config->persistence_location && strlen(config->persistence_location)){
			len = strlen(config->persistence_location) + strlen(config->persistence_file) + 2;
			config->persistence_filepath = mosquitto_malloc(len);
			ଯଦି(!config->persistence_filepath){
				ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
			}
#ifdef WIN32
			snprintf(config->persistence_filepath, len, "%s\\%s", config->persistence_location, config->persistence_file);
#ଅନ୍ୟଥା
			snprintf(config->persistence_filepath, len, "%s/%s", config->persistence_location, config->persistence_file);
#endif
		}ଅନ୍ୟଥା{
			config->persistence_filepath = mosquitto_strdup(config->persistence_file);
			ଯଦି(!config->persistence_filepath){
				ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
			}
		}
	}
#endif
	/* Default to drop to mosquitto user if no other user specified. This must
	 * remain here even though it is covered in config__parse_args() because this
	 * function may be called on its own. */
	ଯଦି(!config->user){
		config->user = mosquitto_strdup("mosquitto");
	}

#ifdef WITH_BRIDGE
	ପାଇଁ(i=0; i<config->bridge_ଗଣନା; i++){
		ଯଦି(!config->bridges[i]->name){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: bridge name not defined.");
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
		ଯଦି(config->bridges[i]->addresses  == 0){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no remote addresses defined.");
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
		ଯଦି(config->bridges[i]->topic_ଗଣନା == 0){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no topics defined.");
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
#ifdef FINAL_WITH_TLS_PSK
		ଯଦି(config->bridges[i]->tls_psk && !config->bridges[i]->tls_psk_identity){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_identity.");
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
		ଯଦି(config->bridges[i]->tls_psk_identity && !config->bridges[i]->tls_psk){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_psk.");
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
#endif
	}
#endif

	ଯଦି(cr.log_dest_set){
		config->log_dest = cr.log_dest;
	}
	ଯଦି(db.quiet){
		config->log_type = 0;
	}ଅନ୍ୟଥା ଯଦି(db.verbose){
		config->log_type = UINT_MAX;
	}ଅନ୍ୟଥା ଯଦି(cr.log_type_set){
		config->log_type = cr.log_type;
	}
#ifdef WITH_BRIDGE
	ଫେରନ୍ତୁ config__check_bridges(config);
#ଅନ୍ୟଥା
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
#endif
}


ସ୍ଥିର mosquitto_plugin_id_t *config__plugin_find(ଅଚଳ ଅକ୍ଷର *name)
{
	ଯଦି(db.plugins && name){
		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<db.plugin_ଗଣନା; i++){
			ଯଦି(db.plugins[i]->config.name && !strcmp(db.plugins[i]->config.name, name)){
				ଫେରନ୍ତୁ db.plugins[i];
			}
		}
	}
	ଫେରନ୍ତୁ ଖାଲି;
}


ସ୍ଥିର mosquitto_plugin_id_t *config__plugin_load(ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଅକ୍ଷର *path)
{
	mosquitto_plugin_id_t *plugin = ଖାଲି;
	mosquitto_plugin_id_t **plugins = ଖାଲି;

	ଯଦି(name && config__plugin_find(name)){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Duplicate plugin name '%s'.", name);
		ଫେରନ୍ତୁ ଖାଲି;
	}

	ଯଦି(!path || !strcmp(path, "")){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Missing plugin path for plugin name '%s'.", name);
		ଫେରନ୍ତୁ ଖାଲି;
	}

	plugin = mosquitto_calloc(1, ଆକାର(mosquitto_plugin_id_t));
	ଯଦି(!plugin){
		ଯାଅ error;
	}

	ଯଦି(name){
		plugin->config.name = mosquitto_strdup(name);
	}
	plugin->config.path = mosquitto_strdup(path);
	ଯଦି((name && !plugin->config.name) || !plugin->config.path){
		ଯାଅ error;
	}
	plugin->config.options = ଖାଲି;
	plugin->config.option_ଗଣନା = 0;
	plugin->config.deny_special_chars = true;

	/* Add to db list */
	plugins = mosquitto_realloc(db.plugins, (size_t)(db.plugin_ଗଣନା+1)*ଆକାର(ସଂରଚନା mosquitto__plugin_config *));
	ଯଦି(!plugins){
		ଯାଅ error;
	}

	plugins[db.plugin_ଗଣନା] = plugin;
	db.plugins = plugins;
	db.plugin_ଗଣନା++;

	ଫେରନ୍ତୁ plugin;
error:
	log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
	ଯଦି(plugin){
		mosquitto_FREE(plugin->config.name);
		mosquitto_FREE(plugin->config.path);
	}
	mosquitto_FREE(plugin);
	ଫେରନ୍ତୁ ଖାଲି;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, ସଂରଚନା mosquitto__security_options *security_options)
{
	ସଂରଚନା mosquitto__security_options **new_options;
	mosquitto_plugin_id_t **new_plugins;

	new_options = mosquitto_realloc(plugin->config.security_options, (size_t)(plugin->config.security_option_ଗଣନା+1)*ଆକାର(ସଂରଚନା mosquitto__security_options *));
	new_plugins = mosquitto_realloc(security_options->plugins, (size_t)(security_options->plugin_ଗଣନା+1)*ଆକାର(mosquitto_plugin_id_t *));

	ଯଦି(!new_options || !new_plugins){
		mosquitto_FREE(new_options);
		mosquitto_FREE(new_plugins);
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	new_options[plugin->config.security_option_ଗଣନା] = security_options;
	plugin->config.security_options = new_options;
	plugin->config.security_option_ଗଣନା++;

	new_plugins[security_options->plugin_ଗଣନା] = plugin;
	security_options->plugins = new_plugins;
	security_options->plugin_ଗଣନା++;

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା config__read_file_core(ସଂରଚନା mosquitto__config *config, bool reload, ସଂରଚନା config_recurse *cr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା level, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *lineno, FILE *fptr, ଅକ୍ଷର **buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *buflen)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ଅକ୍ଷର *token;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା tmp_int;
	ଅକ୍ଷର *saveptr = ଖାଲି;
#ifdef WITH_BRIDGE
	ଅକ୍ଷର *tmp_char;
	ସଂରଚନା mosquitto__bridge *cur_bridge = ଖାଲି;
#endif
	mosquitto_plugin_id_t *cur_plugin = ଖାଲି;

	ଅକ୍ଷର *ଚାବି;
	ସଂରଚନା mosquitto__listener *cur_listener = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno_ext = 0;
	size_t prefix_len;
	size_t slen;
#ifdef WITH_WEBSOCKETS
	ଅକ୍ଷର **ws_origins = ଖାଲି;
#endif

	*lineno = 0;

	ଯେତେବେଳେ(mosquitto_fgets(buf, buflen, fptr)){
		(*lineno)++;
		ଯଦି((*buf)[0] != '#' && (*buf)[0] != 10 && (*buf)[0] != 13){
			slen = strlen(*buf);
			ଯଦି(slen == 0){
				ଜାରି;
			}
			ଯେତେବେଳେ((*buf)[slen-1] == 10 || (*buf)[slen-1] == 13){
				(*buf)[slen-1] = 0;
				slen = strlen(*buf);
				ଯଦି(slen == 0){
					ଜାରି;
				}
			}
			token = strtok_r((*buf), " ", &saveptr);
			ଯଦି(token){
				ଯଦି(!strcmp(token, "accept_protocol_versions")){
					REQUIRE_NON_DEFAULT_LISTENER(token);
					cur_listener->disable_protocol_v3 = true;
					cur_listener->disable_protocol_v4 = true;
					cur_listener->disable_protocol_v5 = true;
					ଯଦି(saveptr == ଖାଲି){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", "accept_protocol_versions");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					token = strtok_r(saveptr, ", \t", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "accept_protocol_versions");

					ଯେତେବେଳେ(token){
						ଯଦି(!strcmp(token, "3")){
							cur_listener->disable_protocol_v3 = false;
						}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "4")){
							cur_listener->disable_protocol_v4 = false;
						}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "5")){
							cur_listener->disable_protocol_v5 = false;
						}

						token = strtok_r(ଖାଲି, ", \t", &saveptr);
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "acl_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);

					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->acl_data.acl_file);
					ଯଦି(conf__parse_string(&token, "acl_file", &cur_security_options->acl_data.acl_file, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "address") || !strcmp(token, "addresses")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ଯଦି(cur_bridge->addresses){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid bridge configuration, 'address' only allowed once.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯେତେବେଳେ((token = strtok_r(ଖାଲି, " ", &saveptr))){
						ଯଦି(token[0] == '#'){
							ଭାଙ୍ଗ;
						}
						ସଂରଚନା bridge_address *new_addresses = mosquitto_realloc(cur_bridge->addresses, ଆକାର(ସଂରଚନା bridge_address)*(size_t)(cur_bridge->address_ଗଣନା+1));
						ଯଦି(!new_addresses){
							log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
							ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
						}
						cur_bridge->address_ଗଣନା++;
						cur_bridge->addresses = new_addresses;
						memset(&cur_bridge->addresses[cur_bridge->address_ଗଣନା-1], 0, ଆକାର(ସଂରଚନା bridge_address));
						cur_bridge->addresses[cur_bridge->address_ଗଣନା-1].address = mosquitto_strdup(token);
						ଯଦି(!cur_bridge->addresses[cur_bridge->address_ଗଣନା-1].address){
							log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
							ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
						}
					}
					ପାଇଁ(i=0; i<cur_bridge->address_ଗଣନା; i++){
						/* cur_bridge->addresses[i].address is now
						 * "address[:port]". If address is an IPv6 address,
						 * then port is required. We must check for the :
						 * backwards. */
						tmp_char = strrchr(cur_bridge->addresses[i].address, ':');
						ଯଦି(tmp_char){
							/* Remove ':', so cur_bridge->addresses[i].address
							 * now just looks like the address. */
							tmp_char[0] = '\0';

							/* The remainder of the string */
							tmp_int = atoi(&tmp_char[1]);
							ଯଦି(tmp_int < 1 || tmp_int > UINT16_MAX){
								log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid bridge port value (%d).", tmp_int);
								ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
							}
							cur_bridge->addresses[i].port = (uint16_t)tmp_int;
						}ଅନ୍ୟଥା{
							cur_bridge->addresses[i].port = 1883;
						}
						conf__attempt_resolve(cur_bridge->addresses[i].address, "bridge address", MOSQ_LOG_WARNING, "Warning");
					}
					ଯଦି(cur_bridge->address_ଗଣନା == 0){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Empty address value in configuration.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "allow_anonymous")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					PER_LISTENER_ALTERNATIVE(token, "listener_allow_anonymous");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ଯଦି(conf__parse_bool(&token, "allow_anonymous", (bool *)&cur_security_options->allow_anonymous, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "allow_duplicate_messages")){
					OPTION_DEPRECATED(token, "The behaviour will default to true.");
					ଯଦି(conf__parse_bool(&token, "allow_duplicate_messages", &config->allow_duplicate_messages, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "allow_zero_length_clientid")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ଯଦି(conf__parse_bool(&token, "allow_zero_length_clientid", &cur_security_options->allow_zero_length_clientid, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strncmp(token, "auth_opt_", strlen("auth_opt_")) || !strncmp(token, "plugin_opt_", strlen("plugin_opt_"))){
					ଯଦି(reload){
						ଜାରି;        /* Auth plugin not currently valid for reloading. */

					}
					REQUIRE_PLUGIN(token);

					ଯଦି(!strncmp(token, "auth_opt_", strlen("auth_opt_"))){
						prefix_len = strlen("auth_opt_");
					}ଅନ୍ୟଥା{
						prefix_len = strlen("plugin_opt_");
					}
					ଯଦି(strlen(token) < prefix_len + 3){
						/* auth_opt_ == 9, + one digit key == 10, + one space == 11, + one value == 12 */
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid 'plugin_opt_' config option.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଚାବି = mosquitto_strdup(&token[prefix_len]);
					ଯଦି(!ଚାବି){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
						ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
					}ଅନ୍ୟଥା ଯଦି(STREMPTY(ଚାବି)){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Empty 'plugin_opt_' config option.");
						mosquitto_FREE(ଚାବି);
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					token = saveptr;
					ଯଦି(token && token[0]){
						ଯେତେବେଳେ(token[0] == ' ' || token[0] == '\t'){
							token++;
						}
						ସଂରଚନା mosquitto_opt *new_options = mosquitto_realloc(cur_plugin->config.options, (size_t)(cur_plugin->config.option_ଗଣନା+1)*ଆକାର(ସଂରଚନା mosquitto_opt));
						ଯଦି(!new_options){
							log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
							mosquitto_FREE(ଚାବି);
							ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
						}
						cur_plugin->config.option_ଗଣନା++;
						cur_plugin->config.options = new_options;
						cur_plugin->config.options[cur_plugin->config.option_ଗଣନା-1].ଚାବି = ଚାବି;
						cur_plugin->config.options[cur_plugin->config.option_ଗଣନା-1].ମୂଲ୍ୟ = mosquitto_strdup(token);
						ଯଦି(!cur_plugin->config.options[cur_plugin->config.option_ଗଣନା-1].ମୂଲ୍ୟ){
							log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
							ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
						}
					}ଅନ୍ୟଥା{
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", ଚାବି);
						mosquitto_FREE(ଚାବି);
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "auth_plugin") || !strcmp(token, "plugin") || !strcmp(token, "global_plugin")){
					ଯଦି(reload){
						ଜାରି;        /* plugin not currently valid for reloading. */
					}
					ଯଦି(!strcmp(token, "global_plugin")){
						cur_security_options = &db.config->security_options;
					}ଅନ୍ୟଥା{
						REQUIRE_LISTENER_IF_PER_LISTENER(token);
						conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					}

					cur_plugin = config__plugin_load(ଖାଲି, saveptr);
					ଯଦି(cur_plugin == ଖାଲି){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(config__plugin_add_secopt(cur_plugin, cur_security_options)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "auth_plugin_deny_special_chars")){
					ଯଦି(reload){
						ଜାରି;        /* Auth plugin not currently valid for reloading. */
					}
					ଯଦି(!cur_plugin){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: An auth_plugin_deny_special_chars option exists in the config file without a plugin.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(conf__parse_bool(&token, "auth_plugin_deny_special_chars", &cur_plugin->config.deny_special_chars, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "plugin_load")){
					ଅକ୍ଷର *name = ଖାଲି;

					ଯଦି(reload){
						ଜାରି;        /* plugin not currently valid for reloading. */

					}
					name = strtok_r(ଖାଲି, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(name, "plugin_load");

					cur_plugin = config__plugin_load(name, saveptr);
					ଯଦି(cur_plugin == ଖାଲି){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "plugin_use")){
					ଅକ୍ଷର *name = ଖାଲି;

					ଯଦି(reload){
						ଜାରି;        /* plugin not currently valid for reloading. */
					}
					REQUIRE_NON_DEFAULT_LISTENER(token);

					ଯଦି(conf__parse_string(&token, "plugin_use", &name, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					cur_plugin = config__plugin_find(name);
					ଯଦି(!cur_plugin){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Plugin '%s' not previously loaded.", name);
						mosquitto_FREE(name);
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(name);
					ଯଦି(config__plugin_add_secopt(cur_plugin, cur_listener->security_options)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "auto_id_prefix")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					OPTION_DEPRECATED(token, "Please use 'listener_auto_id_prefix' instead.");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ଯଦି(conf__parse_string(&token, "auto_id_prefix", &cur_security_options->auto_id_prefix, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(cur_security_options->auto_id_prefix){
						ଯଦି(strlen(cur_security_options->auto_id_prefix) > 50){
							log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: auto_id_prefix length must be <= 50.");
							ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
						}
						cur_security_options->auto_id_prefix_len = (uint16_t)strlen(cur_security_options->auto_id_prefix);
					}ଅନ୍ୟଥା{
						cur_security_options->auto_id_prefix_len = 0;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "autosave_interval")){
					ଯଦି(conf__parse_int(&token, "autosave_interval", &config->autosave_interval, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(config->autosave_interval < 0){
						config->autosave_interval = 0;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "autosave_on_changes")){
					ଯଦି(conf__parse_bool(&token, "autosave_on_changes", &config->autosave_on_changes, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "bind_address")){
					OPTION_DEPRECATED(token, "");
					config->local_only = false;
					ଯଦି(reload){
						ଜାରି;        /* Rebinding listeners not valid during reloading. */

					}
					ଯଦି(config__create_default_listener(config, token)){
						ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					ଯଦି(conf__parse_string(&token, "default listener bind_address", &config->default_listener->host, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(conf__attempt_resolve(config->default_listener->host, "bind_address", MOSQ_LOG_ERR, "Error")){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "bind_interface")){
#ifdef SO_BINDTODEVICE
					ଯଦି(reload){
						ଜାରି;        /* Rebinding listeners not valid during reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ଯଦି(conf__parse_string(&token, "bind_interface", &cur_listener->bind_interface, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: bind_interface specified but socket option not available.");
					ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "bridge_attempt_unsubscribe")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_bool(&token, "bridge_attempt_unsubscribe", &cur_bridge->attempt_unsubscribe, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "bridge_cafile")){
#ଯଦି defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					ଯଦି(conf__parse_string(&token, "bridge_cafile", &cur_bridge->tls_cafile, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "bridge_alpn")){
#ଯଦି defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_string(&token, "bridge_alpn", &cur_bridge->tls_alpn, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "bridge_ciphers")){
#ଯଦି defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_string(&token, "bridge_ciphers", &cur_bridge->tls_ciphers, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "bridge_ciphers_tls1.3")){
#ଯଦି defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_string(&token, "bridge_ciphers_tls1.3", &cur_bridge->tls_13_ciphers, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "bridge_bind_address")){
#ଯଦି defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_string(&token, "bridge_bind_address", &cur_bridge->bind_address, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "bridge_capath")){
#ଯଦି defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					ଯଦି(conf__parse_string(&token, "bridge_capath", &cur_bridge->tls_capath, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "bridge_certfile")){
#ଯଦି defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					ଯଦି(conf__parse_string(&token, "bridge_certfile", &cur_bridge->tls_certfile, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "bridge_identity")){
#ଯଦି defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					ଯଦି(conf__parse_string(&token, "bridge_identity", &cur_bridge->tls_psk_identity, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "bridge_insecure")){
#ଯଦି defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_bool(&token, "bridge_insecure", &cur_bridge->tls_insecure, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(cur_bridge->tls_insecure){
						log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge '%s' using insecure mode.", cur_bridge->name);
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "bridge_require_ocsp")){
#ଯଦି defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_bool(&token, "bridge_require_ocsp", &cur_bridge->tls_ocsp_required, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "bridge_max_packet_size")){
#ଯଦି defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_int(&token, "bridge_max_packet_size", &tmp_int, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(tmp_int < 0){
						tmp_int = 0;
					}
					cur_bridge->maximum_packet_size = (uint32_t)tmp_int;
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "bridge_max_topic_alias")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_int(&token, "bridge_max_topic_alias", &tmp_int, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}

					ଯଦି(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: bridge_max_topic_alias must be > 0 and <= 65535.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					cur_bridge->max_topic_alias = (uint16_t)tmp_int;
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "bridge_outgoing_retain")){
#ଯଦି defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_bool(&token, "bridge_outgoing_retain", &cur_bridge->outgoing_retain, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "bridge_keyfile")){
#ଯଦି defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					mosquitto_FREE(cur_bridge->tls_keyfile);
					ଯଦି(conf__parse_string(&token, "bridge_keyfile", &cur_bridge->tls_keyfile, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "bridge_protocol_version")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(ଖାଲି, "", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_protocol_version");

					ଯଦି(!strcmp(token, "mqttv31")){
						cur_bridge->protocol_version = mosq_p_mqtt31;
					}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "mqttv311")){
						cur_bridge->protocol_version = mosq_p_mqtt311;
					}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "mqttv50")){
						cur_bridge->protocol_version = mosq_p_mqtt5;
					}ଅନ୍ୟଥା{
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid 'bridge_protocol_version' value (%s).", token);
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "bridge_psk")){
#ଯଦି defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					ଯଦି(conf__parse_string(&token, "bridge_psk", &cur_bridge->tls_psk, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "bridge_receive_maximum")){
#ଯଦି defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_int(&token, "bridge_receive_maximum", &tmp_int, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(tmp_int <= 0){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be greater than 0.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}ଅନ୍ୟଥା ଯଦି((uint64_t)tmp_int > (uint64_t)UINT16_MAX){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be lower than %u.", UINT16_MAX);
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					cur_bridge->receive_maximum = (uint16_t)tmp_int;
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "bridge_reload_type")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(ଖାଲି, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_reload_type");

					ଯଦି(!strcmp(token, "lazy")){
						cur_bridge->reload_type = brt_lazy;
					}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "immediate")){
						cur_bridge->reload_type = brt_immediate;
					}ଅନ୍ୟଥା{
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid 'bridge_reload_type' value in configuration (%s).", token);
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "bridge_session_expiry_interval")){
#ଯଦି defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_int(&token, "bridge_session_expiry_interval", &tmp_int, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(tmp_int < 0){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must not be negative.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}ଅନ୍ୟଥା ଯଦି((uint64_t)tmp_int > (uint64_t)MQTT_SESSION_EXPIRY_NEVER){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must be lower than %u.", MQTT_SESSION_EXPIRY_NEVER);
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					cur_bridge->session_expiry_interval = (uint32_t)tmp_int;
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "bridge_tcp_keepalive")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);

					ଯଦି(conf__parse_int(&token, "bridge_tcp_keepalive_idle", &tmp_int, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(tmp_int <= 0){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: invalid TCP keepalive idle value.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_idle = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tmp_int;

					ଯଦି(conf__parse_int(&token, "bridge_tcp_keepalive_interval", &tmp_int, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(tmp_int <= 0){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: invalid TCP keepalive interval value.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_interval = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tmp_int;

					ଯଦି(conf__parse_int(&token, "bridge_tcp_keepalive_counter", &tmp_int, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(tmp_int <= 0){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: invalid TCP keepalive counter value.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_counter = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tmp_int;
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "bridge_tcp_user_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
#ifdef WITH_TCP_USER_TIMEOUT
					ଯଦି(conf__parse_int(&token, "bridge_tcp_user_timeout", &tmp_int, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(tmp_int < 0){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: invalid TCP user timeout value.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_user_timeout = tmp_int;
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge TCP user timeout support not available.");
#endif
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "bridge_tls_use_os_certs")){
#ଯଦି defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_bool(&token, "bridge_tls_use_os_certs", &cur_bridge->tls_use_os_certs, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "bridge_tls_version")){
#ଯଦି defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_string(&token, "bridge_tls_version", &cur_bridge->tls_version, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "cafile")){
#ଯଦି defined(WITH_TLS)
					REQUIRE_LISTENER(token);
					ଯଦି(cur_listener->psk_hint){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->cafile);
					ଯଦି(conf__parse_string(&token, "cafile", &cur_listener->cafile, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "capath")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->capath);
					ଯଦି(conf__parse_string(&token, "capath", &cur_listener->capath, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "certfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ଯଦି(cur_listener->psk_hint){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->certfile);
					ଯଦି(conf__parse_string(&token, "certfile", &cur_listener->certfile, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "check_retain_source")){
					ଯଦି(conf__parse_bool(&token, "check_retain_source", &config->check_retain_source, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "ciphers")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers);
					ଯଦି(conf__parse_string(&token, "ciphers", &cur_listener->ciphers, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "ciphers_tls1.3")){
#ଯଦି defined(WITH_TLS) && (!defined(LIBRESSL_VERSION_ସଂଖ୍ୟା) || LIBRESSL_VERSION_ସଂଖ୍ୟା > 0x3040000FL)
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers_tls13);
					ଯଦି(conf__parse_string(&token, "ciphers_tls1.3", &cur_listener->ciphers_tls13, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: ciphers_tls1.3 support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "clientid") || !strcmp(token, "remote_clientid")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_string(&token, "bridge remote clientid", &cur_bridge->remote_clientid, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "cleansession")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_bool(&token, "cleansession", &cur_bridge->clean_start, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "local_cleansession")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_bool(&token, "local_cleansession", (bool *)&cur_bridge->clean_start_local, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "clientid_prefixes")){
					OPTION_DEPRECATED(token, "");
					mosquitto_FREE(config->clientid_prefixes);
					ଯଦି(conf__parse_string(&token, "clientid_prefixes", &config->clientid_prefixes, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "connection")){
#ifdef WITH_BRIDGE
					token = strtok_r(ଖାଲି, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "connection");

					/* Check for existing bridge name. */
					ପାଇଁ(i=0; i<config->bridge_ଗଣନା; i++){
						ଯଦି(!strcmp(config->bridges[i]->name, token)){
							log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Duplicate bridge name '%s'.", token);
							ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
						}
					}

					ସଂରଚନା mosquitto__bridge **bridges_new = mosquitto_realloc(config->bridges, (size_t)(config->bridge_ଗଣନା+1)*ଆକାର(ସଂରଚନା mosquitto__bridge *));
					ଯଦି(!bridges_new){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
						ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
					}
					config->bridges = bridges_new;
					cur_bridge = mosquitto_malloc(ଆକାର(ସଂରଚନା mosquitto__bridge));
					ଯଦି(!cur_bridge){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
						ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
					}

					config->bridge_ଗଣନା++;
					config->bridges[config->bridge_ଗଣନା-1] = cur_bridge;

					memset(cur_bridge, 0, ଆକାର(ସଂରଚନା mosquitto__bridge));
					cur_bridge->name = mosquitto_strdup(token);
					ଯଦି(!cur_bridge->name){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
						ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
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
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "connection_messages")){
					ଯଦି(conf__parse_bool(&token, token, &config->connection_messages, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "crlfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->crlfile);
					ଯଦି(conf__parse_string(&token, "crlfile", &cur_listener->crlfile, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "dhparamfile")){
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: dhparamfile is no longer required.");
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "disable_client_cert_date_checks")){
#ifdef WITH_TLS
					REQUIRE_LISTENER(token);
					ଯଦି(conf__parse_bool(&token, "disable_client_cert_date_checks", &cur_listener->disable_client_cert_date_checks, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "enable_control_api")){
#ifdef WITH_CONTROL
					ଯଦି(conf__parse_bool(&token, "enable_control_api", &config->enable_control_api, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: $CONTROL support not available (enable_control_api).");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "enable_proxy_protocol")){
#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
#endif
					REQUIRE_LISTENER(token);
					ଯଦି(conf__parse_int(&token, "enable_proxy_protocol", &cur_listener->enable_proxy_protocol, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(cur_listener->enable_proxy_protocol < 1 || cur_listener->enable_proxy_protocol > 2){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: enable_proxy_protocol must be 1 or 2.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "global_max_clients")){
					ଯଦି(conf__parse_int(&token, "global_max_clients", &config->global_max_clients, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "global_max_connections")){
					ଯଦି(conf__parse_int(&token, "global_max_connections", &config->global_max_connections, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "http_dir")){
#ଯଦି defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
					ଯଦି(reload){
						ଜାରି;        /* Not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					ଯଦି(conf__parse_string(&token, "http_dir", &cur_listener->http_dir, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ifdef WIN32
					ଅକ୍ଷର *http_dir_canonical = _fullpath(ଖାଲି, cur_listener->http_dir, 0);
					ଅଚଳ ଅକ୍ଷର sep = '\\';
#ଅନ୍ୟଥା
					ଅକ୍ଷର *http_dir_canonical = realpath(cur_listener->http_dir, ଖାଲି);
					ଅଚଳ ଅକ୍ଷର sep = '/';
#endif
					ଯଦି(!http_dir_canonical){
						ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
					}
					size_t http_dir_len = strlen(http_dir_canonical) + ଆକାର(sep) + 1;
					ଅକ୍ଷର *http_dir_canonical_sep = mosquitto_calloc(http_dir_len, ଆକାର(ଅକ୍ଷର));
					ଯଦି(!http_dir_canonical_sep){
						ମୁକ୍ତ(http_dir_canonical);
						ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
					}
					snprintf(http_dir_canonical_sep, http_dir_len, "%s%c", http_dir_canonical, sep);
					ମୁକ୍ତ(http_dir_canonical);
					mosquitto_FREE(cur_listener->http_dir);
					cur_listener->http_dir = http_dir_canonical_sep;
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: http_dir support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "idle_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_int(&token, "idle_timeout", &cur_bridge->idle_timeout, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(cur_bridge->idle_timeout < 1){
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "idle_timeout interval too low, using 1 second.");
						cur_bridge->idle_timeout = 1;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "include_dir")){
					ଯଦି(level == 0){
						ଅକ୍ଷର **files;
						ପୂର୍ଣ୍ଣସଂଖ୍ୟା file_ଗଣନା;

						/* Only process include_dir from the main config file. */
						token = strtok_r(ଖାଲି, "", &saveptr);
						REQUIRE_NON_EMPTY_OPTION(token, "include_dir");

						rc = config__get_dir_files(token, &files, &file_ଗଣନା);
						ଯଦି(rc){
							ଫେରନ୍ତୁ rc;
						}

						ପାଇଁ(i=0; i<file_ଗଣନା; i++){
							log__printf(ଖାଲି, MOSQ_LOG_INFO, "Loading config file '%s'", files[i]);

							rc = config__read_file(config, reload, files[i], cr, level+1, &lineno_ext);
							ଯଦି(rc){
								ଯଦି(lineno_ext > 0){
									log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error found at '%s:%d'.", files[i], lineno_ext);
								}
								/* Free happens below */
								ଭାଙ୍ଗ;
							}
						}
						ପାଇଁ(i=0; i<file_ଗଣନା; i++){
							mosquitto_FREE(files[i]);
						}
						mosquitto_FREE(files);
						ଯଦି(rc){
							ଫେରନ୍ତୁ rc;    /* This returns if config__read_file() fails above */
						}
					}ଅନ୍ୟଥା{
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: The include_dir option is only valid in the main configuration file.");
						ଫେରନ୍ତୁ 1;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "keepalive_interval")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_int(&token, "keepalive_interval", &tmp_int, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(tmp_int > UINT16_MAX){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Bridge keepalive value too high.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(tmp_int < 5){
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "keepalive interval too low, using 5 seconds.");
						tmp_int = 5;
					}
					cur_bridge->keepalive = (uint16_t)tmp_int;
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "keyfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->keyfile);
					ଯଦି(conf__parse_string(&token, "keyfile", &cur_listener->keyfile, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "listener")){
					config->local_only = false;

					ଯଦି(conf__parse_int(&token, "listener port", &tmp_int, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ifdef WITH_UNIX_SOCKETS
					ଯଦି(tmp_int < 0 || tmp_int > UINT16_MAX){
#ଅନ୍ୟଥା
					ଯଦି(tmp_int < 1 || tmp_int > UINT16_MAX){
#endif
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}

					/* Look for bind address / unix socket path */
					token = strtok_r(ଖାଲି, " ", &saveptr);
					ଯଦି(token != ଖାଲି && token[0] == '#'){
						token = ଖାଲି;
					}

					ଯଦି(tmp_int == 0 && token == ଖାଲି){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: A listener with port 0 must provide a Unix socket path.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}

					ଯଦି(reload){
						/* We reload listeners settings based on port number/unix socket path.
						 * If the port number/unix path doesn't already exist, exit with a complaint. */
						cur_listener = ଖାଲି;
#ifdef WITH_UNIX_SOCKETS
						ଯଦି(tmp_int == 0){
							ପାଇଁ(i=0; i<config->listener_ଗଣନା; i++){
								ଯଦି(config->listeners[i].unix_socket_path != ଖାଲି
										&& strcmp(config->listeners[i].unix_socket_path, token) == 0){

									cur_listener = &config->listeners[i];
									ଭାଙ୍ଗ;
								}
							}
						}ଅନ୍ୟଥା
#endif
						{
							ପାଇଁ(i=0; i<config->listener_ଗଣନା; i++){
								ଯଦି(config->listeners[i].port == tmp_int){
									/* Now check we have a matching bind address, if defined */
									ଯଦି(config->listeners[i].host){
										ଯଦି(token && !strcmp(config->listeners[i].host, token)){
											/* They both have a bind address, and they match */
											cur_listener = &config->listeners[i];
											ଭାଙ୍ଗ;
										}
									}ଅନ୍ୟଥା{
										ଯଦି(token == ଖାଲି){
											/* Neither this config nor the new config have a bind address,
											 * so they match. */
											cur_listener = &config->listeners[i];
											ଭାଙ୍ଗ;
										}
									}
								}
							}
						}
						ଯଦି(!cur_listener){
							log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: It is not currently possible to add/remove listeners when reloading the config file.");
							ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
						}
					}ଅନ୍ୟଥା{
						ଯଦି(config__add_listener(config)){
							log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
							ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
						}
						cur_listener = &config->listeners[config->listener_ଗଣନା-1];
					}

					listener__set_defaults(cur_listener);
					cur_listener->port = (uint16_t)tmp_int;

					mosquitto_FREE(cur_listener->host);

#ifdef WITH_UNIX_SOCKETS
					mosquitto_FREE(cur_listener->unix_socket_path);
#endif

					ଯଦି(token){
#ifdef WITH_UNIX_SOCKETS
						ଯଦି(cur_listener->port == 0){
							cur_listener->unix_socket_path = mosquitto_strdup(token);
						}ଅନ୍ୟଥା
#endif
						{
							cur_listener->host = mosquitto_strdup(token);
						}
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "listener_allow_anonymous")){
					REQUIRE_LISTENER(token);
					ଯଦି(conf__parse_bool(&token, "listener_allow_anonymous", (bool *)&cur_listener->security_options->allow_anonymous, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "listener_auto_id_prefix")){
					REQUIRE_LISTENER(token);
					ଯଦି(conf__parse_string(&token, "listener_auto_id_prefix", &cur_listener->security_options->auto_id_prefix, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(cur_listener->security_options->auto_id_prefix){
						ଯଦି(strlen(cur_listener->security_options->auto_id_prefix) > 50){
							log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: 'listener_auto_id_prefix' length must be <= 50.");
							ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
						}
						cur_listener->security_options->auto_id_prefix_len = (uint16_t)strlen(cur_listener->security_options->auto_id_prefix);
					}ଅନ୍ୟଥା{
						cur_listener->security_options->auto_id_prefix_len = 0;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "local_clientid")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_string(&token, "bridge local clientd", &cur_bridge->local_clientid, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "local_password")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_string(&token, "bridge local_password", &cur_bridge->local_password, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "local_username")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_string(&token, "bridge local_username", &cur_bridge->local_username, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "log_dest")){
					token = strtok_r(ଖାଲି, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_dest");

					cr->log_dest_set = 1;
					ଯଦି(!strcmp(token, "none")){
						cr->log_dest = MQTT3_LOG_NONE;
					}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "syslog")){
						cr->log_dest |= MQTT3_LOG_SYSLOG;
					}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "stdout")){
						cr->log_dest |= MQTT3_LOG_STDOUT;
					}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "stderr")){
						cr->log_dest |= MQTT3_LOG_STDERR;
					}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "topic")){
						cr->log_dest |= MQTT3_LOG_TOPIC;
					}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "dlt")){
						cr->log_dest |= MQTT3_LOG_DLT;
#ifdef ANDROID
					}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "android")){
						cr->log_dest |= MQTT3_LOG_ANDROID;
#endif
					}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "file")){
						ଯଦି(config->log_fptr || config->log_file){
							log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Duplicate \"log_dest file\" value.");
							ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
						}
						/* Get remaining string. */
						token = saveptr;
						ଯଦି(token && token[0]){
							ଯେତେବେଳେ(token[0] == ' ' || token[0] == '\t'){
								token++;
							}
						}
						/* Duplicate "token" check here saves a log__printf() */
						ଯଦି(token && token[0]){
							config->log_file = mosquitto_strdup(token);
							ଯଦି(!config->log_file){
								log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
								ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
							}
							cr->log_dest |= MQTT3_LOG_FILE;
						}ଅନ୍ୟଥା{
							log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Empty \"log_dest file\" value in configuration.");
							ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
						}
						cr->log_dest |= MQTT3_LOG_FILE;
					}ଅନ୍ୟଥା{
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid 'log_dest' value (%s).", token);
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଯଦି defined(WIN32) || defined(__CYGWIN__)
					ଯଦି(service_handle){
						ଯଦି(cr->log_dest == MQTT3_LOG_STDOUT || cr->log_dest == MQTT3_LOG_STDERR){
							log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Cannot log to stdout/stderr when running as a Windows service.");
							ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
						}
					}
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "log_facility")){
#ଯଦି defined(WIN32) || defined(__CYGWIN__)
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: log_facility not supported on Windows.");
#ଅନ୍ୟଥା
					ଯଦି(conf__parse_int(&token, "log_facility", &tmp_int, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ବାଛ(tmp_int){
						ସ୍ଥିତି 0:
							config->log_facility = LOG_LOCAL0;
							ଭାଙ୍ଗ;
						ସ୍ଥିତି 1:
							config->log_facility = LOG_LOCAL1;
							ଭାଙ୍ଗ;
						ସ୍ଥିତି 2:
							config->log_facility = LOG_LOCAL2;
							ଭାଙ୍ଗ;
						ସ୍ଥିତି 3:
							config->log_facility = LOG_LOCAL3;
							ଭାଙ୍ଗ;
						ସ୍ଥିତି 4:
							config->log_facility = LOG_LOCAL4;
							ଭାଙ୍ଗ;
						ସ୍ଥିତି 5:
							config->log_facility = LOG_LOCAL5;
							ଭାଙ୍ଗ;
						ସ୍ଥିତି 6:
							config->log_facility = LOG_LOCAL6;
							ଭାଙ୍ଗ;
						ସ୍ଥିତି 7:
							config->log_facility = LOG_LOCAL7;
							ଭାଙ୍ଗ;
						ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
							log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid 'log_facility' value (%d).", tmp_int);
							ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "log_timestamp")){
					ଯଦି(conf__parse_bool(&token, token, &config->log_timestamp, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "log_timestamp_format")){
					ଯଦି(conf__parse_string(&token, token, &config->log_timestamp_format, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "log_type")){
					token = strtok_r(ଖାଲି, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_type");

					cr->log_type_set = 1;
					ଯଦି(!strcmp(token, "none")){
						cr->log_type = MOSQ_LOG_NONE;
					}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "information")){
						cr->log_type |= MOSQ_LOG_INFO;
					}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "notice")){
						cr->log_type |= MOSQ_LOG_NOTICE;
					}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "warning")){
						cr->log_type |= MOSQ_LOG_WARNING;
					}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "error")){
						cr->log_type |= MOSQ_LOG_ERR;
					}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "debug")){
						cr->log_type |= MOSQ_LOG_DEBUG;
					}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "subscribe")){
						cr->log_type |= MOSQ_LOG_SUBSCRIBE;
					}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "unsubscribe")){
						cr->log_type |= MOSQ_LOG_UNSUBSCRIBE;
					}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "internal")){
						cr->log_type |= MOSQ_LOG_INTERNAL;
#ifdef WITH_WEBSOCKETS
					}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "websockets")){
						cr->log_type |= MOSQ_LOG_WEBSOCKETS;
#endif
					}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "all")){
						cr->log_type = MOSQ_LOG_ALL;
					}ଅନ୍ୟଥା{
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid 'log_type' value (%s).", token);
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "max_connections")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ଯଦି(conf__parse_int(&token, token, &cur_listener->max_connections, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(cur_listener->max_connections < 0){
						cur_listener->max_connections = -1;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "maximum_qos") || !strcmp(token, "max_qos")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ଯଦି(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(tmp_int < 0 || tmp_int > 2){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: 'max_qos' must be between 0 and 2 inclusive.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					cur_listener->max_qos = (uint8_t)tmp_int;
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "max_inflight_bytes")){
					ଯଦି(conf__parse_int(&token, "max_inflight_bytes", &tmp_int, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_inflight_bytes = (size_t)tmp_int;
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "max_inflight_messages")){
					ଯଦି(conf__parse_int(&token, "max_inflight_messages", &tmp_int, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(tmp_int < 0 || tmp_int == UINT16_MAX){
						tmp_int = 0;
					}ଅନ୍ୟଥା ଯଦି(tmp_int > UINT16_MAX){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: 'max_inflight_messages' must be <= 65535.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					config->max_inflight_messages = (uint16_t)tmp_int;
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "max_keepalive")){
					ଯଦି(conf__parse_int(&token, "max_keepalive", &tmp_int, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid 'max_keepalive' value (%d).", tmp_int);
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					config->max_keepalive = (uint16_t)tmp_int;
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "max_packet_size")){
					ଯଦି(conf__parse_int(&token, "max_packet_size", &tmp_int, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(tmp_int < 20){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: 'max_packet_size' must be >= 20.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					config->max_packet_size = (uint32_t)tmp_int;
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "max_queued_bytes")){
					ଯଦି(conf__parse_int(&token, "max_queued_bytes", &tmp_int, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_bytes = (size_t)tmp_int;
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "max_queued_messages")){
					ଯଦି(conf__parse_int(&token, "max_queued_messages", &tmp_int, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_messages = tmp_int;
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "memory_limit")){
					ssize_t lim;
					ଯଦି(conf__parse_ssize_t(&token, "memory_limit", &lim, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(lim < 0){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid 'memory_limit' value (%ld).", lim);
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					mosquitto_memory_set_limit((size_t)lim);
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "message_size_limit")){
					log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Note: It is recommended to replace `message_size_limit` with `max_packet_size`.");
					ଯଦି(conf__parse_int(&token, "message_size_limit", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)&config->message_size_limit, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(config->message_size_limit > MQTT_MAX_PAYLOAD){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid 'message_size_limit' value (%u).", config->message_size_limit);
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "mount_point")){
					REQUIRE_LISTENER(token);
					mosquitto_FREE(cur_listener->mount_point);
					ଯଦି(conf__parse_string(&token, "mount_point", &cur_listener->mount_point, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(mosquitto_pub_topic_check(cur_listener->mount_point) != MOSQ_ERR_SUCCESS){
						log__printf(ଖାଲି, MOSQ_LOG_ERR,
								"Error: Invalid 'mount_point' value (%s). Does it contain a wildcard character?",
								cur_listener->mount_point);
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "notifications")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_bool(&token, "notifications", &cur_bridge->notifications, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "notifications_local_only")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_bool(&token, "notifications_local_only", &cur_bridge->notifications_local_only, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "notification_topic")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_string(&token, "notification_topic", &cur_bridge->notification_topic, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "password") || !strcmp(token, "remote_password")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_string(&token, "bridge remote_password", &cur_bridge->remote_password, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "password_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->password_data.password_file);
					ଯଦି(conf__parse_string(&token, "password_file", &cur_security_options->password_data.password_file, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "per_listener_settings")){
					OPTION_DEPRECATED(token, "Please see the documentation for how to achieve the same effect.");
					ଯଦି(config->per_listener_settings){
						/* Once this is set, don't let it be unset. It should be the first config option ideally. */
						ଜାରି;
					}
					ଯଦି(conf__parse_bool(&token, "per_listener_settings", &config->per_listener_settings, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(cur_security_options && config->per_listener_settings){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: per_listener_settings must be set before any other security settings.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "persistence") || !strcmp(token, "retained_persistence")){
					ଯଦି(conf__parse_bool(&token, token, &config->persistence, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "persistence_file")){
					ଯଦି(conf__parse_string(&token, "persistence_file", &config->persistence_file, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "persistence_location")){
					ଯଦି(conf__parse_string(&token, "persistence_location", &config->persistence_location, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "persistent_client_expiration")){
					token = strtok_r(ଖାଲି, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "persistent_client_expiration");

					time_t expiration_mult;

					ବାଛ(token[strlen(token)-1]){
						ସ୍ଥିତି 's':
							expiration_mult = 1;
							ଭାଙ୍ଗ;
						ସ୍ଥିତି 'h':
							expiration_mult = 3600;
							ଭାଙ୍ଗ;
						ସ୍ଥିତି 'd':
							expiration_mult = 86400;
							ଭାଙ୍ଗ;
						ସ୍ଥିତି 'w':
							expiration_mult = 86400*7;
							ଭାଙ୍ଗ;
						ସ୍ଥିତି 'm':
							expiration_mult = 86400*30;
							ଭାଙ୍ଗ;
						ସ୍ଥିତି 'y':
							expiration_mult = 86400*365;
							ଭାଙ୍ଗ;
						ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
							log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
							ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					token[strlen(token)-1] = '\0';
					config->persistent_client_expiration = atoi(token)*expiration_mult;
					ଯଦି(config->persistent_client_expiration <= 0){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "pid_file")){
					ଯଦି(reload){
						ଜାରି;        /* pid file not valid for reloading. */
					}
					ଯଦି(conf__parse_string(&token, "pid_file", &config->pid_file, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "port")){
					OPTION_DEPRECATED(token, "Please use 'listener' instead.");
					config->local_only = false;
					ଯଦି(reload){
						ଜାରି;        /* Listeners not valid for reloading. */

					}
					ଯଦି(config__create_default_listener(config, token)){
						ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					ଯଦି(config->default_listener->port){
						log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Default listener port specified multiple times. Only the latest will be used.");
					}
					ଯଦି(conf__parse_int(&token, "port", &tmp_int, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(tmp_int < 1 || tmp_int > UINT16_MAX){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					config->default_listener->port = (uint16_t)tmp_int;
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "protocol")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(ଖାଲି, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "protocol");

					ଯଦି(!strcmp(token, "mqtt")){
						cur_listener->protocol = mp_mqtt;
						/*
						}else if(!strcmp(token, "mqttsn")){
						    cur_listener->protocol = mp_mqttsn;
						*/
					}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "websockets")){
#ifdef WITH_WEBSOCKETS
						cur_listener->protocol = mp_websockets;
#ଅନ୍ୟଥା
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Websockets support not available.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
#endif
					}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "http_api")){
#ifdef WITH_HTTP_API
						cur_listener->protocol = mp_http_api;
#ଅନ୍ୟଥା
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: HTTP API support not available.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
#endif
					}ଅନ୍ୟଥା{
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid 'protocol' value (%s).", token);
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "proxy_protocol_v2_require_tls")){
#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
#endif
					REQUIRE_LISTENER(token);
					ଯଦି(conf__parse_bool(&token, "proxy_protocol_v2_require_tls", &cur_listener->proxy_protocol_v2_require_tls, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "psk_file")){
#ifdef FINAL_WITH_TLS_PSK
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ଯଦି(cur_listener && cur_listener->certfile){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_security_options->psk_file);
					ଯଦି(conf__parse_string(&token, "psk_file", &cur_security_options->psk_file, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "psk_hint")){
#ifdef FINAL_WITH_TLS_PSK
					ଯଦି(reload){
						ଜାରି;        /* PSK file not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ଯଦି(conf__parse_string(&token, "psk_hint", &cur_listener->psk_hint, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "queue_qos0_messages")){
					ଯଦି(conf__parse_bool(&token, token, &config->queue_qos0_messages, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "require_certificate")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ଯଦି(conf__parse_bool(&token, "require_certificate", &cur_listener->require_certificate, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "restart_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					cur_bridge->backoff_cap = 0; /* set backoff to constant mode, unless cap is specified further down */
					token = strtok_r(ଖାଲି, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "restart_timeout");

					cur_bridge->restart_timeout = atoi(token);
					cur_bridge->backoff_base = 0;
					cur_bridge->backoff_cap = 0;
					ଯଦି(cur_bridge->restart_timeout < 1){
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "restart_timeout interval too low, using 1 second.");
						cur_bridge->restart_timeout = 1;
					}ଅନ୍ୟଥା ଯଦି(cur_bridge->restart_timeout > 3600){
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "restart_timeout interval too high, using 3600 seconds.");
						cur_bridge->restart_timeout = 3600;
					}
					token = strtok_r(ଖାଲି, " ", &saveptr);
					ଯଦି(token){
						cur_bridge->backoff_base = cur_bridge->restart_timeout;
						cur_bridge->backoff_cap = atoi(token);
						ଯଦି(cur_bridge->backoff_cap < cur_bridge->backoff_base){
							log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: backoff cap is lower than the base in restart_timeout.");
							ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
						}ଅନ୍ୟଥା ଯଦି(cur_bridge->backoff_cap > 7200){
							log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: backoff cap too high, using 7200 seconds.");
							cur_bridge->backoff_cap = 7200;
						}

						token = strtok_r(ଖାଲି, " ", &saveptr);
						ଯଦି(token){
							cur_bridge->stable_connection_period = atoi(token);
							ଯଦି(cur_bridge->stable_connection_period < 0){
								log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: stable connection period cannot be negative.");
								ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
							}
						}
					}
					cur_bridge->restart_timeout *= 1000; /* backoff is tracked in ms */
					cur_bridge->backoff_base *= 1000;
					cur_bridge->backoff_cap *= 1000;
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "retain_available")){
					ଯଦି(conf__parse_bool(&token, token, &config->retain_available, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "retain_expiry_interval")){
					ଯଦି(conf__parse_int(&token, token, &config->retain_expiry_interval, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(config->retain_expiry_interval < 1){
						log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Error: retain_expiry_interval must be >= 1.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}ଅନ୍ୟଥା ଯଦି(config->retain_expiry_interval > 10000000){
						log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: retain_expiry_interval being capped at 19 years.");
						config->retain_expiry_interval = 10000000;
					}
					config->retain_expiry_interval *= 60;
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "retry_interval")){
					OPTION_UNAVAILABLE(token);
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "round_robin")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_bool(&token, "round_robin", &cur_bridge->round_robin, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "set_tcp_nodelay")){
					ଯଦି(conf__parse_bool(&token, "set_tcp_nodelay", &config->set_tcp_nodelay, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "start_type")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(ଖାଲି, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					ଯଦି(!strcmp(token, "automatic")){
						cur_bridge->start_type = bst_automatic;
					}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "lazy")){
						cur_bridge->start_type = bst_lazy;
					}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "manual")){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Manual start_type not supported.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "once")){
						cur_bridge->start_type = bst_once;
					}ଅନ୍ୟଥା{
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid 'start_type' value in configuration (%s).", token);
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "socket_domain")){
					ଯଦି(reload){
						ଜାରି;        /* socket_domain not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(ଖାଲି, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					ଯଦି(!strcmp(token, "ipv4")){
						cur_listener->socket_domain = AF_INET;
					}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "ipv6")){
						cur_listener->socket_domain = AF_INET6;
					}ଅନ୍ୟଥା{
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid 'socket_domain' value '%s' in configuration.", token);
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "sys_interval")){
					ଯଦି(conf__parse_int(&token, "sys_interval", &config->sys_interval, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(config->sys_interval < 0 || config->sys_interval > 65535){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid 'sys_interval' value (%d).", config->sys_interval);
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "threshold")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_int(&token, "threshold", &cur_bridge->threshold, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(cur_bridge->threshold < 1){
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "threshold too low, using 1 message.");
						cur_bridge->threshold = 1;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "tls_engine")){
#ifdef WITH_TLS
					ଯଦି(reload){
						ଜାରି;        /* tls_engine not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ଯଦି(conf__parse_string(&token, "tls_engine", &cur_listener->tls_engine, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "tls_engine_kpass_sha1")){
#ifdef WITH_TLS
					ଯଦି(reload){
						ଜାରି;        /* tls_engine not valid for reloading. */

					}
					ଅକ୍ଷର *kpass_sha = ଖାଲି, *kpass_sha_bin = ଖାଲି;
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ଯଦି(conf__parse_string(&token, "tls_engine_kpass_sha1", &kpass_sha, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(mosquitto__hex2bin_sha1(kpass_sha, (unsigned ଅକ୍ଷର **)&kpass_sha_bin) != MOSQ_ERR_SUCCESS){
						mosquitto_FREE(kpass_sha);
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					mosquitto_free(cur_listener->tls_engine_kpass_sha1);
					cur_listener->tls_engine_kpass_sha1 = kpass_sha_bin;
					mosquitto_FREE(kpass_sha);
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "tls_keyform")){
#ifdef WITH_TLS
					ଯଦି(reload){
						ଜାରି;        /* tls_engine not valid for reloading. */

					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ଅକ୍ଷର *keyform = ଖାଲି;
					ଯଦି(conf__parse_string(&token, "tls_keyform", &keyform, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					cur_listener->tls_keyform = mosq_k_pem;
					ଯଦି(!strcmp(keyform, "engine")){
						cur_listener->tls_keyform = mosq_k_engine;
					}
					mosquitto_FREE(keyform);
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "tls_version")){
#ଯଦି defined(WITH_TLS)
					ଯଦି(reload){
						ଜାରି;        /* tls_version not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					ଯଦି(conf__parse_string(&token, "tls_version", &cur_listener->tls_version, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "topic")){
#ifdef WITH_BRIDGE
					ଅକ୍ଷର *topic = ଖାଲି;
					ଗଣନା mosquitto__bridge_direction direction = bd_out;
					uint8_t qos = 0;
					ଅକ୍ଷର *local_prefix = ଖାଲି, *remote_prefix = ଖାଲି;

					REQUIRE_BRIDGE(token);

					token = strtok_r(ଖାଲି, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "topic");

					// Check if the topic is quoted (e.g. for spaces within topic names), but not the
					// special case of ""
					ଯଦି(token[0] == '"' && token [1] != '"'){
						ଯଦି(strchr(saveptr, '"') == ଖାଲି){
							log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Missing closing quote in topic value (%s).", saveptr);
							ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
						}

						ଅକ୍ଷର *topic_in_quotes = strtok_r(ଖାଲି, "\"", &saveptr);
						size_t tlen = 1;
						ଯଦି(topic_in_quotes){
							tlen = strlen(topic_in_quotes);
						}

						topic = mosquitto_malloc(strlen(token) + tlen + 1);
						ଯଦି(!topic){
							log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
							ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
						}

						strcpy(topic, token + 1);
						strcat(topic, " ");
						ଯଦି(topic_in_quotes){
							strcat(topic, topic_in_quotes);
						}
					}ଅନ୍ୟଥା{
						topic = mosquitto_strdup(token);
						ଯଦି(!topic){
							log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
							ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
						}
					}

					token = strtok_r(ଖାଲି, " ", &saveptr);
					ଯଦି(token){
						ଯଦି(!strcasecmp(token, "out")){
							direction = bd_out;
						}ଅନ୍ୟଥା ଯଦି(!strcasecmp(token, "in")){
							direction = bd_in;
						}ଅନ୍ୟଥା ଯଦି(!strcasecmp(token, "both")){
							direction = bd_both;
						}ଅନ୍ୟଥା{
							log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid bridge topic direction '%s'.", token);
							mosquitto_FREE(topic);
							ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
						}
						token = strtok_r(ଖାଲି, " ", &saveptr);
						ଯଦି(token){
							ଯଦି(token[0] == '#'){
								(ଶୂନ୍ୟ)strtok_r(ଖାଲି, "", &saveptr);
							}
							qos = (uint8_t)atoi(token);
							ଯଦି(qos > 2){
								log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid bridge QoS level '%s'.", token);
								mosquitto_FREE(topic);
								ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
							}

							token = strtok_r(ଖାଲି, " ", &saveptr);
							ଯଦି(token){
								ଯଦି(!strcmp(token, "\"\"") || token[0] == '#'){
									local_prefix = ଖାଲି;
									ଯଦି(token[0] == '#'){
										(ଶୂନ୍ୟ)strtok_r(ଖାଲି, "", &saveptr);
									}
								}ଅନ୍ୟଥା{
									local_prefix = token;
								}

								token = strtok_r(ଖାଲି, " ", &saveptr);
								ଯଦି(token){
									ଯଦି(!strcmp(token, "\"\"") || token[0] == '#'){
										remote_prefix = ଖାଲି;
									}ଅନ୍ୟଥା{
										remote_prefix = token;
									}
								}
							}
						}
					}

					ଯଦି(bridge__add_topic(cur_bridge, topic, direction, qos, local_prefix, remote_prefix)){
						mosquitto_FREE(topic);
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(topic);
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "max_topic_alias")){
					REQUIRE_LISTENER(token);
					ଯଦି(conf__parse_int(&token, "max_topic_alias", &tmp_int, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias' value in configuration.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias = (uint16_t)tmp_int;
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "max_topic_alias_broker")){
					REQUIRE_LISTENER(token);
					ଯଦି(conf__parse_int(&token, "max_topic_alias_broker", &tmp_int, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias_broker' value in configuration.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias_broker = (uint16_t)tmp_int;
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "try_private")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_bool(&token, "try_private", &cur_bridge->try_private, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "upgrade_outgoing_qos")){
					ଯଦି(conf__parse_bool(&token, token, &config->upgrade_outgoing_qos, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "use_identity_as_username")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ଯଦି(conf__parse_bool(&token, "use_identity_as_username", &cur_listener->use_identity_as_username, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "use_subject_as_username")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ଯଦି(conf__parse_bool(&token, "use_subject_as_username", &cur_listener->use_subject_as_username, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "user")){
					ଯଦି(reload){
						ଜାରି;        /* Drop privileges user not valid for reloading. */
					}
					mosquitto_FREE(config->user);
					ଯଦି(conf__parse_string(&token, "user", &config->user, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "use_username_as_clientid")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ଯଦି(conf__parse_bool(&token, "use_username_as_clientid", &cur_listener->use_username_as_clientid, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "username") || !strcmp(token, "remote_username")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ଯଦି(conf__parse_string(&token, "bridge remote_username", &cur_bridge->remote_username, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "websockets_log_level")){
#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					ଯଦି(conf__parse_int(&token, "websockets_log_level", &config->websockets_log_level, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
#endif
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "websockets_headers_size") || !strcmp(token, "packet_buffer_size")){
					ଯଦି(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					ଯଦି(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Error: Packet buffer size must be between 0 and 65535 inclusive.");
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					config->packet_buffer_size = (uint16_t)tmp_int;
				}ଅନ୍ୟଥା ଯଦି(!strcmp(token, "websockets_origin")){
#ifdef WITH_WEBSOCKETS
#  ଯଦି LWS_LIBRARY_VERSION_ସଂଖ୍ୟା >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN
					REQUIRE_LISTENER(token);
					ws_origins = mosquitto_realloc(cur_listener->ws_origins, ଆକାର(ଅକ୍ଷର *)*(size_t)(cur_listener->ws_origin_ଗଣନା+1));
					ଯଦି(ws_origins == ଖାଲି){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
						ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
					}
					ws_origins[cur_listener->ws_origin_ଗଣନା] = ଖାଲି;
					cur_listener->ws_origins = ws_origins;
					ଯଦି(conf__parse_string(&token, "websockets_origin", &ws_origins[cur_listener->ws_origin_ଗଣନା], &saveptr)){
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}
					cur_listener->ws_origin_ଗଣନା++;
#  ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: websockets_origin support not available, libwebsockets version is too old.");
#  endif
#ଅନ୍ୟଥା
					log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Websockets support not available.");
#endif
				}ଅନ୍ୟଥା{
					log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unknown configuration variable '%s'.", token);
					ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
				}
			}
		}
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା config__read_file(ସଂରଚନା mosquitto__config *config, bool reload, ଅଚଳ ଅକ୍ଷର *file, ସଂରଚନା config_recurse *cr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା level, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *lineno)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	FILE *fptr = ଖାଲି;
	ଅକ୍ଷର *buf;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା buflen;
#ifndef WIN32
	DIR *dir;
#endif

#ifndef WIN32
	dir = opendir(file);
	ଯଦି(dir){
		closedir(dir);
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Config file '%s' is a directory.", file);
		ଫେରନ୍ତୁ 1;
	}
#endif

	fptr = mosquitto_fopen(file, "rt", false);
	ଯଦି(!fptr){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to open config file '%s'.", file);
		ଫେରନ୍ତୁ 1;
	}

	buflen = 1000;
	buf = mosquitto_malloc((size_t)buflen);
	ଯଦି(!buf){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
		fclose(fptr);
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	rc = config__read_file_core(config, reload, cr, level, lineno, fptr, &buf, &buflen);
	mosquitto_FREE(buf);
	fclose(fptr);

	ଫେରନ୍ତୁ rc;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା config__check_proxy(ସଂରଚନା mosquitto__config *config)
{
	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<config->listener_ଗଣନା; i++){
		ସଂରଚନା mosquitto__listener *l = &config->listeners[i];

		ଯଦି(l->enable_proxy_protocol == 2){
#ifdef WITH_TLS
			ଯଦି(l->use_subject_as_username){
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: use_subject_as_username cannot be used with `enable_proxy_protocol 2`.");
				ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
			}

			ଯଦି(l->certfile || l->keyfile){
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: certfile and keyfile cannot be used with `enable_proxy_protocol 2`.");
				ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
			}
#endif
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା config__check(ସଂରଚନା mosquitto__config *config)
{
	/* Checks that are easy to make after the config has been loaded. */

	ଅଚଳ ଅକ୍ଷର *id_prefix;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା id_prefix_len;
	ଯଦି(config->security_options.auto_id_prefix){
		id_prefix = config->security_options.auto_id_prefix;
		id_prefix_len = config->security_options.auto_id_prefix_len;
	}ଅନ୍ୟଥା{
		id_prefix = "auto-";
		id_prefix_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)strlen("auto-");
	}

	/* Default to auto_id_prefix = 'auto-' if none set. */
	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<config->listener_ଗଣନା; i++){
		ଯଦି(!config->listeners[i].security_options->auto_id_prefix){
			config->listeners[i].security_options->auto_id_prefix = mosquitto_strdup(id_prefix);
			ଯଦି(!config->listeners[i].security_options->auto_id_prefix){
				ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
			}
			config->listeners[i].security_options->auto_id_prefix_len = (uint16_t)id_prefix_len;
		}
	}

	ଫେରନ୍ତୁ config__check_proxy(config);
}

#ifdef WITH_BRIDGE


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା config__check_bridges(ସଂରଚନା mosquitto__config *config)
{
	ସଂରଚନା mosquitto__bridge *bridge1, *bridge2;
	ଅକ୍ଷର hostname[256];
	size_t len;

	/* Check for bridge duplicate local_clientid, need to generate missing IDs
	 * first. */
	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<config->bridge_ଗଣନା; i++){
		bridge1 = config->bridges[i];

		ଯଦି(!bridge1->remote_clientid){
			ଯଦି(!gethostname(hostname, 256)){
				len = strlen(hostname) + strlen(bridge1->name) + 2;
				bridge1->remote_clientid = mosquitto_malloc(len);
				ଯଦି(!bridge1->remote_clientid){
					ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
				}
				snprintf(bridge1->remote_clientid, len, "%s.%s", hostname, bridge1->name);
			}ଅନ୍ୟଥା{
				ଫେରନ୍ତୁ 1;
			}
		}

		ଯଦି(!bridge1->local_clientid){
			len = strlen(bridge1->remote_clientid) + strlen("local.") + 2;
			bridge1->local_clientid = mosquitto_malloc(len);
			ଯଦି(!bridge1->local_clientid){
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
				ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
			}
			snprintf(bridge1->local_clientid, len, "local.%s", bridge1->remote_clientid);
		}
	}

	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<config->bridge_ଗଣନା; i++){
		bridge1 = config->bridges[i];
		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା j=i+1; j<config->bridge_ଗଣନା; j++){
			bridge2 = config->bridges[j];
			ଯଦି(!strcmp(bridge1->local_clientid, bridge2->local_clientid)){
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Bridge local_clientid "
						"'%s' is not unique. Try changing or setting the "
						"local_clientid value for one of the bridges.",
						bridge1->local_clientid);
				ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
			}
		}
	}

#ifdef WITH_TLS
	/* Check for missing TLS cafile/capath/certfile/keyfile */
	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<config->listener_ଗଣନା; i++){
		bool cafile = !!config->listeners[i].cafile;
		bool capath = !!config->listeners[i].capath;
		bool certfile = !!config->listeners[i].certfile;
		bool keyfile = !!config->listeners[i].keyfile;

		ଯଦି((certfile && !keyfile) || (!certfile && keyfile)){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Both certfile and keyfile must be provided to enable a TLS listener.");
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
		ଯଦି(cafile && !certfile){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: cafile specified without certfile and keyfile.");
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
		ଯଦି(capath && !certfile){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: capath specified without certfile and keyfile.");
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
	}
#endif
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}
#endif


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf__parse_bool(ଅକ୍ଷର **token, ଅଚଳ ଅକ୍ଷର *name, bool *ମୂଲ୍ୟ, ଅକ୍ଷର **saveptr)
{
	*token = strtok_r(ଖାଲି, " ", saveptr);
	ଯଦି(*token){
		ଯଦି(!strcmp(*token, "false") || !strcmp(*token, "0")){
			*ମୂଲ୍ୟ = false;
		}ଅନ୍ୟଥା ଯଦି(!strcmp(*token, "true") || !strcmp(*token, "1")){
			*ମୂଲ୍ୟ = true;
		}ଅନ୍ୟଥା{
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Invalid '%s' value (%s).", name, *token);
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
	}ଅନ୍ୟଥା{
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf__parse_int(ଅକ୍ଷର **token, ଅଚଳ ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ମୂଲ୍ୟ, ଅକ୍ଷର **saveptr)
{
	*token = strtok_r(ଖାଲି, " ", saveptr);
	ଯଦି(*token){
		ଅକ୍ଷର *endptr = ଖାଲି;
		long l = strtol(*token, &endptr, 0);
		ଯଦି(endptr == *token || endptr[0] != '\0'){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: '%s' value not a number.", name);
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
		ଯଦି(l > INT_MAX || l < INT_MIN){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: '%s' value out of range.", name);
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
		*ମୂଲ୍ୟ = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)l;
	}ଅନ୍ୟଥା{
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf__parse_ssize_t(ଅକ୍ଷର **token, ଅଚଳ ଅକ୍ଷର *name, ssize_t *ମୂଲ୍ୟ, ଅକ୍ଷର **saveptr)
{
	*token = strtok_r(ଖାଲି, " ", saveptr);
	ଯଦି(*token){
		*ମୂଲ୍ୟ = atol(*token);
	}ଅନ୍ୟଥା{
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf__parse_string(ଅକ୍ଷର **token, ଅଚଳ ଅକ୍ଷର *name, ଅକ୍ଷର **ମୂଲ୍ୟ, ଅକ୍ଷର **saveptr)
{
	size_t tlen;

	*token = strtok_r(ଖାଲି, "", saveptr);
	ଯଦି(*token){
		ଯଦି(*ମୂଲ୍ୟ){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Duplicate '%s' value in configuration.", name);
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
		/* Deal with multiple spaces at the beginning of the string. */
		*token = mosquitto_trimblanks(*token);
		ଯଦି(strlen(*token) == 0){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}

		tlen = strlen(*token);
		ଯଦି(tlen > UINT16_MAX){
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
		ଯଦି(mosquitto_validate_utf8(*token, (uint16_t)tlen)){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Malformed UTF-8 in configuration.");
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
		*ମୂଲ୍ୟ = mosquitto_strdup(*token);
		ଯଦି(!*ମୂଲ୍ୟ){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}
	}ଅନ୍ୟଥା{
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}
