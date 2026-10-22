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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ <limits.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <errno.h>

#ifdef WIN32
#இல்லையெனில்
#  include <dirent.h>
#  include <strings.h>
#endif

#ifndef WIN32
#  include <netdb.h>
#  include <sys/socket.h>
#இல்லையெனில்
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#எனில் !defined(WIN32) && !defined(__CYGWIN__)
#  include <syslog.h>
#endif

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "tls_mosq.h"
#அந்தர்பாவ "util_mosq.h"
#அந்தர்பாவ "mosquitto/mqtt_protocol.h"

#அந்தர்பாவ "utlist.h"

கட்டமைப்பு config_recurse {
	unsigned முழு log_dest;
	முழு log_dest_set;
	unsigned முழு log_type;
	முழு log_type_set;
};

#எனில் defined(WIN32) || defined(__CYGWIN__)
#அந்தர்பாவ <windows.h>
வெளி SERVICE_STATUS_HANDLE service_handle;
#endif


#define REQUIRE_LISTENER(A) \
		செய்{ \
			எனில்(cur_listener == வெற்று){ \
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				திரும்பு MOSQ_ERR_INVAL; \
			} \
		}போது(0)

#define REQUIRE_LISTENER_OR_DEFAULT_LISTENER(A) \
		செய்{ \
			எனில்(cur_listener == வெற்று){ \
				எனில்(config__create_default_listener(config, (A))){ \
					திரும்பு MOSQ_ERR_NOMEM; \
				} \
				cur_listener = config->default_listener; \
			} \
		}போது(0)

#define REQUIRE_LISTENER_IF_PER_LISTENER(A) \
		செய்{ \
			எனில்(config->per_listener_settings == true && cur_listener == வெற்று){ \
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				திரும்பு MOSQ_ERR_INVAL; \
			} \
		}போது(0)

#define REQUIRE_NON_DEFAULT_LISTENER(A) \
		செய்{ \
			எனில்(cur_listener == config->default_listener || cur_listener == வெற்று){ \
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				திரும்பு MOSQ_ERR_INVAL; \
			} \
		}போது(0)

#define REQUIRE_BRIDGE(A) \
		செய்{ \
			எனில்(cur_bridge == வெற்று){ \
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error: The '%s' option requires a bridge to be defined first.", (A)); \
				திரும்பு MOSQ_ERR_INVAL; \
			} \
		}போது(0)

#define REQUIRE_PLUGIN(A) \
		செய்{ \
			எனில்(cur_plugin == வெற்று){ \
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error: The '%s' option requires plugin/global_plugin/plugin_load to be defined first.", (A)); \
				திரும்பு MOSQ_ERR_INVAL; \
			} \
		}போது(0)

#define OPTION_DEPRECATED(A, B) \
		log__printf(வெற்று, MOSQ_LOG_NOTICE, "The '%s' option is now deprecated and will be removed in version 3.0. %s", (A), (B))

#define OPTION_UNAVAILABLE(A) \
		log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: The '%s' option is no longer available.", (A));

#define REQUIRE_NON_EMPTY_OPTION(A, B) \
		செய்{ \
			எனில்(!(A)){ \
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", (B)); \
				திரும்பு MOSQ_ERR_INVAL; \
			} \
		}போது(0)

#define PER_LISTENER_ALTERNATIVE(A, B) \
		எனில்(config->per_listener_settings){ \
			log__printf(வெற்று, MOSQ_LOG_NOTICE, "You are using the '%s' option with 'per_listener_settings true'. Please replace this with '%s'.", A, B); \
		} \


#ifdef FINAL_WITH_TLS_PSK
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A) \
		செய்{ \
			எனில்(cur_bridge->tls_psk_identity || cur_bridge->tls_psk){ \
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and psk encryption in a single bridge.", (A)); \
				திரும்பு MOSQ_ERR_INVAL; \
			} \
		}போது(0)

#  define REQUIRE_BRIDGE_NO_X509(A) \
		செய்{ \
			எனில்(cur_bridge->tls_cafile || cur_bridge->tls_capath || cur_bridge->tls_certfile || cur_bridge->tls_keyfile){ \
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and identity encryption in a single bridge.", (A)); \
				திரும்பு MOSQ_ERR_INVAL; \
			} \
		}போது(0)

#இல்லையெனில்
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A)
#  define REQUIRE_BRIDGE_NO_X509(A)
#endif

நிலையான கட்டமைப்பு mosquitto__security_options *cur_security_options = வெற்று;

நிலையான முழு conf__parse_bool(எழுத்து **token, மாறிலி எழுத்து *name, bool *மதிப்பு, எழுத்து **saveptr);
நிலையான முழு conf__parse_int(எழுத்து **token, மாறிலி எழுத்து *name, முழு *மதிப்பு, எழுத்து **saveptr);
நிலையான முழு conf__parse_ssize_t(எழுத்து **token, மாறிலி எழுத்து *name, ssize_t *மதிப்பு, எழுத்து **saveptr);
நிலையான முழு conf__parse_string(எழுத்து **token, மாறிலி எழுத்து *name, எழுத்து **மதிப்பு, எழுத்து **saveptr);
நிலையான முழு config__read_file(கட்டமைப்பு mosquitto__config *config, bool reload, மாறிலி எழுத்து *file, கட்டமைப்பு config_recurse *config_tmp, முழு level, முழு *lineno);
நிலையான முழு config__check(கட்டமைப்பு mosquitto__config *config);
நிலையான வெறுமை config__cleanup_plugins(வெறுமை);
#ifdef WITH_BRIDGE
நிலையான முழு config__check_bridges(கட்டமைப்பு mosquitto__config *config);
#endif


நிலையான முழு config__add_listener(கட்டமைப்பு mosquitto__config *config)
{
	கட்டமைப்பு mosquitto__listener *listener;
	கட்டமைப்பு mosquitto__listener *new_listeners;
	முழு def_listener = -1;

	எனில்(config->default_listener){
		க்கு(முழு i=0; i<config->listener_மொத்தம்; i++){
			எனில்(&config->listeners[i] == config->default_listener){
				def_listener = i;
				நிறுத்து;
			}
		}
	}
	new_listeners = mosquitto_realloc(config->listeners, அளவு(கட்டமைப்பு mosquitto__listener)*(size_t)(config->listener_மொத்தம்+1));
	எனில்(!new_listeners){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
		திரும்பு MOSQ_ERR_NOMEM;
	}
	config->listeners = new_listeners;
	listener = &config->listeners[config->listener_மொத்தம்];
	memset(listener, 0, அளவு(கட்டமைப்பு mosquitto__listener));
	listener->security_options = mosquitto_calloc(1, அளவு(கட்டமைப்பு mosquitto__security_options));
	எனில்(!listener->security_options){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
		திரும்பு MOSQ_ERR_NOMEM;
	}
	எனில்(def_listener != -1){
		config->default_listener = &config->listeners[def_listener];
	}
	config->listener_மொத்தம்++;

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு config__create_default_listener(கட்டமைப்பு mosquitto__config *config, மாறிலி எழுத்து *option_name)
{
	எனில்(config->default_listener){
		திரும்பு MOSQ_ERR_SUCCESS;
	}
	log__printf(வெற்று, MOSQ_LOG_INFO, "Creating default listener due to '%s' option.", option_name);
	log__printf(வெற்று, MOSQ_LOG_INFO, "It is best practice to define a 'listener' first. Using the '%s' option without a listener will be disabled in the future.", option_name);
	எனில்(config__add_listener(config)){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
		திரும்பு MOSQ_ERR_NOMEM;
	}

	config->default_listener = &config->listeners[config->listener_மொத்தம்-1];
	listener__set_defaults(config->default_listener);
	config->default_listener->port = 1883;

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான வெறுமை conf__set_cur_security_options(கட்டமைப்பு mosquitto__config *config, கட்டமைப்பு mosquitto__listener **cur_listener, கட்டமைப்பு mosquitto__security_options **security_options, மாறிலி எழுத்து *option_name)
{
	எனில்(config->per_listener_settings){
		எனில்(*cur_listener == வெற்று){
			எனில்(config__create_default_listener(config, option_name)){
				திரும்பு;
			}
			*cur_listener = config->default_listener;
		}
		(*security_options) = (*cur_listener)->security_options;
	}இல்லையெனில்{
		(*security_options) = &config->security_options;
	}
}


நிலையான முழு conf__attempt_resolve(மாறிலி எழுத்து *host, மாறிலி எழுத்து *text, unsigned முழு log, மாறிலி எழுத்து *செய்தி)
{
	கட்டமைப்பு addrinfo gai_hints;
	கட்டமைப்பு addrinfo *gai_res;
	முழு rc;

	memset(&gai_hints, 0, அளவு(கட்டமைப்பு addrinfo));
	gai_hints.ai_family = AF_UNSPEC;
	gai_hints.ai_socktype = SOCK_STREAM;
	gai_res = வெற்று;
	rc = getaddrinfo(host, வெற்று, &gai_hints, &gai_res);
	எனில்(gai_res){
		freeaddrinfo(gai_res);
	}
	எனில்(rc != 0){
#ifndef WIN32
		எனில்(rc == EAI_SYSTEM){
			எனில்(errno == ENOENT){
				log__printf(வெற்று, log, "%s: Unable to resolve %s %s.", செய்தி, text, host);
			}இல்லையெனில்{
				log__printf(வெற்று, log, "%s: Error resolving %s: %s.", செய்தி, text, strerror(errno));
			}
		}இல்லையெனில்{
			log__printf(வெற்று, log, "%s: Error resolving %s: %s.", செய்தி, text, gai_strerror(rc));
		}
#இல்லையெனில்
		எனில்(rc == WSAHOST_NOT_FOUND){
			log__printf(வெற்று, log, "%s: Error resolving %s.", செய்தி, text);
		}
#endif
		திரும்பு MOSQ_ERR_INVAL;
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான வெறுமை config__init_reload(கட்டமைப்பு mosquitto__config *config)
{
	/* Set defaults */
	க்கு(முழு i=0; i<config->listener_மொத்தம்; i++){
		listener__set_defaults(&config->listeners[i]);
	}

	config->local_only = true;
	config->allow_duplicate_messages = true;

	mosquitto_FREE(config->security_options.acl_data.acl_file);
	mosquitto_FREE(config->security_options.password_data.password_file);
	mosquitto_FREE(config->security_options.psk_file);

	config->security_options.allow_anonymous = -1;
	config->security_options.allow_zero_length_clientid = true;
	config->security_options.auto_id_prefix = வெற்று;
	config->security_options.auto_id_prefix_len = 0;

	config->autosave_interval = 1800;
	config->autosave_on_changes = false;

	mosquitto_FREE(config->clientid_prefixes);

	config->connection_messages = true;
	config->clientid_prefixes = வெற்று;
	config->per_listener_settings = false;
	எனில்(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = வெற்று;
	}
	mosquitto_FREE(config->log_file);

#எனில் defined(WIN32) || defined(__CYGWIN__)
	எனில்(service_handle){
		/* This is running as a Windows service. Default to no logging. Using
		 * stdout/stderr is forbidden because the first clients to connect will
		 * get log information sent to them for some reason. */
		config->log_dest = MQTT3_LOG_NONE;
	}இல்லையெனில்{
		config->log_dest = MQTT3_LOG_STDERR;
	}
#இல்லையெனில்
	config->log_facility = LOG_DAEMON;
	config->log_dest = MQTT3_LOG_STDERR | MQTT3_LOG_DLT;
	எனில்(db.quiet){
		config->log_type = 0;
	}இல்லையெனில் எனில்(db.verbose){
		config->log_type = UINT_MAX;
	}இல்லையெனில்{
		config->log_type = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;
	}
#endif
	config->log_timestamp = true;
	mosquitto_FREE(config->log_timestamp_format);
	config->global_max_clients = -1;
	config->global_max_connections = -1;
	config->log_timestamp_format = வெற்று;
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


நிலையான வெறுமை config__cleanup_plugin_config(mosquitto_plugin_id_t *plugin)
{
	mosquitto_FREE(plugin->config.path);
	mosquitto_FREE(plugin->config.name);

	எனில்(plugin->config.options){
		க்கு(முழு j=0; j<plugin->config.option_மொத்தம்; j++){
			mosquitto_FREE(plugin->config.options[j].சாவி);
			mosquitto_FREE(plugin->config.options[j].மதிப்பு);
		}
		mosquitto_FREE(plugin->config.options);
		plugin->config.option_மொத்தம் = 0;
	}
	mosquitto_FREE(plugin->config.security_options);
	mosquitto_FREE(plugin);
}


நிலையான வெறுமை config__cleanup_plugins(வெறுமை)
{
	க்கு(முழு i=0; i<db.plugin_மொத்தம்; i++){
		config__cleanup_plugin_config(db.plugins[i]);
	}
	mosquitto_FREE(db.plugins);
}


வெறுமை config__init(கட்டமைப்பு mosquitto__config *config)
{
	memset(config, 0, அளவு(கட்டமைப்பு mosquitto__config));
	config__init_reload(config);

	config->daemon = false;
}


வெறுமை config__cleanup(கட்டமைப்பு mosquitto__config *config)
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
	எனில்(config->listeners){
		க்கு(முழு i=0; i<config->listener_மொத்தம்; i++){
			mosquitto_FREE(config->listeners[i].host);
			mosquitto_FREE(config->listeners[i].bind_interface);
			mosquitto_FREE(config->listeners[i].mount_point);
			mosquitto_FREE(config->listeners[i].socks);
			எனில்(config->listeners[i].security_options){
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
#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			எனில்(!config->listeners[i].ws_context) /* libwebsockets frees its own SSL_CTX */
#endif
			{
				SSL_CTX_free(config->listeners[i].ssl_ctx);
				config->listeners[i].ssl_ctx = வெற்று;
			}
#endif
#எனில் defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
			mosquitto_FREE(config->listeners[i].http_dir);
#endif
#ifdef WITH_WEBSOCKETS
			க்கு(முழு j=0; j<config->listeners[i].ws_origin_மொத்தம்; j++){
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
	எனில்(config->bridges){
		க்கு(முழு i=0; i<config->bridge_மொத்தம்; i++){
			config__bridge_cleanup(config->bridges[i]);
		}
		mosquitto_FREE(config->bridges);
	}
#endif
	config__cleanup_plugins();

	எனில்(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = வெற்று;
	}
	எனில்(config->log_file){
		mosquitto_FREE(config->log_file);
		config->log_file = வெற்று;
	}
}

#ifdef WITH_BRIDGE


வெறுமை config__bridge_cleanup(கட்டமைப்பு mosquitto__bridge *bridge)
{
	எனில்(bridge == வெற்று){
		திரும்பு;
	}

	mosquitto_FREE(bridge->name);
	எனில்(bridge->addresses){
		க்கு(முழு i=0; i<bridge->address_மொத்தம்; i++){
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
	எனில்(bridge->topics){
		கட்டமைப்பு mosquitto__bridge_topic *cur_topic, *topic_tmp;

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


நிலையான வெறுமை print_version(வெறுமை)
{
	printf("mosquitto %s\n", VERSION);
	printf("Copyright © 2025 Roger Light.\n");
	printf("License EPL-2.0 OR BSD-3-Clause.\n");
}


நிலையான வெறுமை print_usage(வெறுமை)
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


முழு config__parse_args(கட்டமைப்பு mosquitto__config *config, முழு argc, எழுத்து *argv[])
{
	முழு i;
	முழு port_tmp;

	க்கு(i=1; i<argc; i++){
		எனில்(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--config-file")){
			எனில்(i<argc-1){
				db.config_file = argv[i+1];

				எனில்(config__read(config, false)){
					திரும்பு MOSQ_ERR_INVAL;
				}
			}இல்லையெனில்{
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error: -c argument given, but no config file specified.");
				திரும்பு MOSQ_ERR_INVAL;
			}
			i++;
		}இல்லையெனில் எனில்(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--daemon")){
			config->daemon = true;
		}இல்லையெனில் எனில்(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
			print_usage();
			திரும்பு MOSQ_ERR_UNKNOWN;
		}இல்லையெனில் எனில்(!strcmp(argv[i], "--version")){
			print_version();
			திரும்பு MOSQ_ERR_UNKNOWN;
		}இல்லையெனில் எனில்(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port")){
			எனில்(i<argc-1){
				port_tmp = atoi(argv[i+1]);
				எனில்(port_tmp<1 || port_tmp>UINT16_MAX){
					log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid port specified (%d).", port_tmp);
					திரும்பு MOSQ_ERR_INVAL;
				}இல்லையெனில்{
					எனில்(config->cmd_port_மொத்தம் == CMD_PORT_LIMIT){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Only %d ports can be specified on the command line.", CMD_PORT_LIMIT);
						திரும்பு MOSQ_ERR_INVAL;
					}
					config->cmd_port[config->cmd_port_மொத்தம்] = (uint16_t)port_tmp;
					config->cmd_port_மொத்தம்++;
				}
			}இல்லையெனில்{
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error: -p argument given, but no port specified.");
				திரும்பு MOSQ_ERR_INVAL;
			}
			i++;
		}இல்லையெனில் எனில்(!strcmp(argv[i], "--tls-keylog")){
#ifdef WITH_TLS
			எனில்(i<argc-1){
				db.tls_keylog = mosquitto_strdup(argv[i+1]);
				எனில்(db.tls_keylog == வெற்று){
					log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
					திரும்பு MOSQ_ERR_NOMEM;
				}
			}இல்லையெனில்{
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error: --tls-keylog argument given, but no file specified.");
				திரும்பு MOSQ_ERR_INVAL;
			}
			i++;
#இல்லையெனில்
			fprintf(stderr, "Error: TLS support not available so --tls-keylog is not available.\n");
			திரும்பு MOSQ_ERR_INVAL;
#endif
		}இல்லையெனில் எனில்(!strcmp(argv[i], "-q") || !strcmp(argv[i], "--quiet")){
			db.quiet = true;
		}இல்லையெனில் எனில்(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")){
			db.verbose = true;
		}இல்லையெனில் எனில்(!strcmp(argv[i], "--test-config")){
			config->test_configuration = true;
		}இல்லையெனில்{
			fprintf(stderr, "Error: Unknown option '%s'.\n", argv[i]);
			print_usage();
			திரும்பு MOSQ_ERR_INVAL;
		}
	}

	/* Default to drop to mosquitto user if we are privileged and no user specified. */
	எனில்(!config->user){
		config->user = mosquitto_strdup("mosquitto");
		எனில்(config->user == வெற்று){
			திரும்பு MOSQ_ERR_NOMEM;
		}
	}
	எனில்(db.quiet){
		config->log_type = 0;
	}இல்லையெனில் எனில்(db.verbose){
		config->log_type = UINT_MAX;
	}

	எனில்(getenv("MOSQUITTO_PERSISTENCE_LOCATION")){
		mosquitto_FREE(config->persistence_location);
		config->persistence_location = mosquitto_strdup(getenv("MOSQUITTO_PERSISTENCE_LOCATION"));
		எனில்(!config->persistence_location){
			திரும்பு MOSQ_ERR_NOMEM;
		}
	}
	திரும்பு config__check(config);
}


நிலையான வெறுமை config__copy(கட்டமைப்பு mosquitto__config *src, கட்டமைப்பு mosquitto__config *dest)
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
	dest->security_options.plugin_மொத்தம் = src->security_options.plugin_மொத்தம்;
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

#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	dest->websockets_log_level = src->websockets_log_level;
#endif

#ifdef WITH_BRIDGE
	க்கு(முழு i=0; i<dest->bridge_மொத்தம்; i++){
		எனில்(dest->bridges[i]){
			config__bridge_cleanup(dest->bridges[i]);
		}
	}
	mosquitto_FREE(dest->bridges);
	dest->bridges = src->bridges;
	dest->bridge_மொத்தம் = src->bridge_மொத்தம்;
#endif
}


முழு config__read(கட்டமைப்பு mosquitto__config *config, bool reload)
{
	முழு rc = MOSQ_ERR_SUCCESS;
	கட்டமைப்பு config_recurse cr;
	முழு lineno = 0;
#ifdef WITH_PERSISTENCE
	size_t len;
#endif
	கட்டமைப்பு mosquitto__config config_reload;
	முழு i;

	எனில்(reload){
		memset(&config_reload, 0, அளவு(கட்டமைப்பு mosquitto__config));
	}

	cr.log_dest = MQTT3_LOG_NONE;
	cr.log_dest_set = 0;
	cr.log_type = MOSQ_LOG_NONE;
	cr.log_type_set = 0;

	எனில்(!db.config_file){
		திரும்பு 0;
	}

	எனில்(reload){
		/* Re-initialise appropriate config vars to default for reload. */
		config__init_reload(&config_reload);
		config_reload.listeners = config->listeners;
		config_reload.listener_மொத்தம் = config->listener_மொத்தம்;
		cur_security_options = வெற்று;
		rc = config__read_file(&config_reload, reload, db.config_file, &cr, 0, &lineno);
	}இல்லையெனில்{
		rc = config__read_file(config, reload, db.config_file, &cr, 0, &lineno);
	}
	எனில்(rc){
		எனில்(lineno > 0){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error found at %s:%d.", db.config_file, lineno);
		}
		திரும்பு rc;
	}

	எனில்(reload){
		config__copy(&config_reload, config);
	}

	/* If auth/access options are set and allow_anonymous not explicitly set, disallow anon. */
	எனில்(config->local_only == false){
		எனில்(config->per_listener_settings){
			க்கு(i=0; i<config->listener_மொத்தம்; i++){
				/* Default option if no security options set */
				எனில்(config->listeners[i].security_options->allow_anonymous == -1){
					config->listeners[i].security_options->allow_anonymous = false;
				}
			}
		}இல்லையெனில்{
			எனில்(config->security_options.allow_anonymous == -1){
				config->security_options.allow_anonymous = false;
			}
		}
	}
#ifdef WITH_PERSISTENCE
	எனில்(config->persistence){
		எனில்(!config->persistence_file){
			config->persistence_file = mosquitto_strdup("mosquitto.db");
			எனில்(!config->persistence_file){
				திரும்பு MOSQ_ERR_NOMEM;
			}
		}
		mosquitto_FREE(config->persistence_filepath);
		எனில்(config->persistence_location && strlen(config->persistence_location)){
			len = strlen(config->persistence_location) + strlen(config->persistence_file) + 2;
			config->persistence_filepath = mosquitto_malloc(len);
			எனில்(!config->persistence_filepath){
				திரும்பு MOSQ_ERR_NOMEM;
			}
#ifdef WIN32
			snprintf(config->persistence_filepath, len, "%s\\%s", config->persistence_location, config->persistence_file);
#இல்லையெனில்
			snprintf(config->persistence_filepath, len, "%s/%s", config->persistence_location, config->persistence_file);
#endif
		}இல்லையெனில்{
			config->persistence_filepath = mosquitto_strdup(config->persistence_file);
			எனில்(!config->persistence_filepath){
				திரும்பு MOSQ_ERR_NOMEM;
			}
		}
	}
#endif
	/* Default to drop to mosquitto user if no other user specified. This must
	 * remain here even though it is covered in config__parse_args() because this
	 * function may be called on its own. */
	எனில்(!config->user){
		config->user = mosquitto_strdup("mosquitto");
	}

#ifdef WITH_BRIDGE
	க்கு(i=0; i<config->bridge_மொத்தம்; i++){
		எனில்(!config->bridges[i]->name){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: bridge name not defined.");
			திரும்பு MOSQ_ERR_INVAL;
		}
		எனில்(config->bridges[i]->addresses  == 0){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no remote addresses defined.");
			திரும்பு MOSQ_ERR_INVAL;
		}
		எனில்(config->bridges[i]->topic_மொத்தம் == 0){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no topics defined.");
			திரும்பு MOSQ_ERR_INVAL;
		}
#ifdef FINAL_WITH_TLS_PSK
		எனில்(config->bridges[i]->tls_psk && !config->bridges[i]->tls_psk_identity){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_identity.");
			திரும்பு MOSQ_ERR_INVAL;
		}
		எனில்(config->bridges[i]->tls_psk_identity && !config->bridges[i]->tls_psk){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_psk.");
			திரும்பு MOSQ_ERR_INVAL;
		}
#endif
	}
#endif

	எனில்(cr.log_dest_set){
		config->log_dest = cr.log_dest;
	}
	எனில்(db.quiet){
		config->log_type = 0;
	}இல்லையெனில் எனில்(db.verbose){
		config->log_type = UINT_MAX;
	}இல்லையெனில் எனில்(cr.log_type_set){
		config->log_type = cr.log_type;
	}
#ifdef WITH_BRIDGE
	திரும்பு config__check_bridges(config);
#இல்லையெனில்
	திரும்பு MOSQ_ERR_SUCCESS;
#endif
}


நிலையான mosquitto_plugin_id_t *config__plugin_find(மாறிலி எழுத்து *name)
{
	எனில்(db.plugins && name){
		க்கு(முழு i=0; i<db.plugin_மொத்தம்; i++){
			எனில்(db.plugins[i]->config.name && !strcmp(db.plugins[i]->config.name, name)){
				திரும்பு db.plugins[i];
			}
		}
	}
	திரும்பு வெற்று;
}


நிலையான mosquitto_plugin_id_t *config__plugin_load(மாறிலி எழுத்து *name, மாறிலி எழுத்து *path)
{
	mosquitto_plugin_id_t *plugin = வெற்று;
	mosquitto_plugin_id_t **plugins = வெற்று;

	எனில்(name && config__plugin_find(name)){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Duplicate plugin name '%s'.", name);
		திரும்பு வெற்று;
	}

	எனில்(!path || !strcmp(path, "")){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Missing plugin path for plugin name '%s'.", name);
		திரும்பு வெற்று;
	}

	plugin = mosquitto_calloc(1, அளவு(mosquitto_plugin_id_t));
	எனில்(!plugin){
		செல் error;
	}

	எனில்(name){
		plugin->config.name = mosquitto_strdup(name);
	}
	plugin->config.path = mosquitto_strdup(path);
	எனில்((name && !plugin->config.name) || !plugin->config.path){
		செல் error;
	}
	plugin->config.options = வெற்று;
	plugin->config.option_மொத்தம் = 0;
	plugin->config.deny_special_chars = true;

	/* Add to db list */
	plugins = mosquitto_realloc(db.plugins, (size_t)(db.plugin_மொத்தம்+1)*அளவு(கட்டமைப்பு mosquitto__plugin_config *));
	எனில்(!plugins){
		செல் error;
	}

	plugins[db.plugin_மொத்தம்] = plugin;
	db.plugins = plugins;
	db.plugin_மொத்தம்++;

	திரும்பு plugin;
error:
	log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
	எனில்(plugin){
		mosquitto_FREE(plugin->config.name);
		mosquitto_FREE(plugin->config.path);
	}
	mosquitto_FREE(plugin);
	திரும்பு வெற்று;
}


முழு config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, கட்டமைப்பு mosquitto__security_options *security_options)
{
	கட்டமைப்பு mosquitto__security_options **new_options;
	mosquitto_plugin_id_t **new_plugins;

	new_options = mosquitto_realloc(plugin->config.security_options, (size_t)(plugin->config.security_option_மொத்தம்+1)*அளவு(கட்டமைப்பு mosquitto__security_options *));
	new_plugins = mosquitto_realloc(security_options->plugins, (size_t)(security_options->plugin_மொத்தம்+1)*அளவு(mosquitto_plugin_id_t *));

	எனில்(!new_options || !new_plugins){
		mosquitto_FREE(new_options);
		mosquitto_FREE(new_plugins);
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
		திரும்பு MOSQ_ERR_NOMEM;
	}

	new_options[plugin->config.security_option_மொத்தம்] = security_options;
	plugin->config.security_options = new_options;
	plugin->config.security_option_மொத்தம்++;

	new_plugins[security_options->plugin_மொத்தம்] = plugin;
	security_options->plugins = new_plugins;
	security_options->plugin_மொத்தம்++;

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு config__read_file_core(கட்டமைப்பு mosquitto__config *config, bool reload, கட்டமைப்பு config_recurse *cr, முழு level, முழு *lineno, FILE *fptr, எழுத்து **buf, முழு *buflen)
{
	முழு rc;
	எழுத்து *token;
	முழு tmp_int;
	எழுத்து *saveptr = வெற்று;
#ifdef WITH_BRIDGE
	எழுத்து *tmp_char;
	கட்டமைப்பு mosquitto__bridge *cur_bridge = வெற்று;
#endif
	mosquitto_plugin_id_t *cur_plugin = வெற்று;

	எழுத்து *சாவி;
	கட்டமைப்பு mosquitto__listener *cur_listener = வெற்று;
	முழு i;
	முழு lineno_ext = 0;
	size_t prefix_len;
	size_t slen;
#ifdef WITH_WEBSOCKETS
	எழுத்து **ws_origins = வெற்று;
#endif

	*lineno = 0;

	போது(mosquitto_fgets(buf, buflen, fptr)){
		(*lineno)++;
		எனில்((*buf)[0] != '#' && (*buf)[0] != 10 && (*buf)[0] != 13){
			slen = strlen(*buf);
			எனில்(slen == 0){
				தொடர்;
			}
			போது((*buf)[slen-1] == 10 || (*buf)[slen-1] == 13){
				(*buf)[slen-1] = 0;
				slen = strlen(*buf);
				எனில்(slen == 0){
					தொடர்;
				}
			}
			token = strtok_r((*buf), " ", &saveptr);
			எனில்(token){
				எனில்(!strcmp(token, "accept_protocol_versions")){
					REQUIRE_NON_DEFAULT_LISTENER(token);
					cur_listener->disable_protocol_v3 = true;
					cur_listener->disable_protocol_v4 = true;
					cur_listener->disable_protocol_v5 = true;
					எனில்(saveptr == வெற்று){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", "accept_protocol_versions");
						திரும்பு MOSQ_ERR_INVAL;
					}
					token = strtok_r(saveptr, ", \t", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "accept_protocol_versions");

					போது(token){
						எனில்(!strcmp(token, "3")){
							cur_listener->disable_protocol_v3 = false;
						}இல்லையெனில் எனில்(!strcmp(token, "4")){
							cur_listener->disable_protocol_v4 = false;
						}இல்லையெனில் எனில்(!strcmp(token, "5")){
							cur_listener->disable_protocol_v5 = false;
						}

						token = strtok_r(வெற்று, ", \t", &saveptr);
					}
				}இல்லையெனில் எனில்(!strcmp(token, "acl_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);

					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->acl_data.acl_file);
					எனில்(conf__parse_string(&token, "acl_file", &cur_security_options->acl_data.acl_file, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "address") || !strcmp(token, "addresses")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					எனில்(cur_bridge->addresses){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid bridge configuration, 'address' only allowed once.");
						திரும்பு MOSQ_ERR_INVAL;
					}
					போது((token = strtok_r(வெற்று, " ", &saveptr))){
						எனில்(token[0] == '#'){
							நிறுத்து;
						}
						கட்டமைப்பு bridge_address *new_addresses = mosquitto_realloc(cur_bridge->addresses, அளவு(கட்டமைப்பு bridge_address)*(size_t)(cur_bridge->address_மொத்தம்+1));
						எனில்(!new_addresses){
							log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
							திரும்பு MOSQ_ERR_NOMEM;
						}
						cur_bridge->address_மொத்தம்++;
						cur_bridge->addresses = new_addresses;
						memset(&cur_bridge->addresses[cur_bridge->address_மொத்தம்-1], 0, அளவு(கட்டமைப்பு bridge_address));
						cur_bridge->addresses[cur_bridge->address_மொத்தம்-1].address = mosquitto_strdup(token);
						எனில்(!cur_bridge->addresses[cur_bridge->address_மொத்தம்-1].address){
							log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
							திரும்பு MOSQ_ERR_NOMEM;
						}
					}
					க்கு(i=0; i<cur_bridge->address_மொத்தம்; i++){
						/* cur_bridge->addresses[i].address is now
						 * "address[:port]". If address is an IPv6 address,
						 * then port is required. We must check for the :
						 * backwards. */
						tmp_char = strrchr(cur_bridge->addresses[i].address, ':');
						எனில்(tmp_char){
							/* Remove ':', so cur_bridge->addresses[i].address
							 * now just looks like the address. */
							tmp_char[0] = '\0';

							/* The remainder of the string */
							tmp_int = atoi(&tmp_char[1]);
							எனில்(tmp_int < 1 || tmp_int > UINT16_MAX){
								log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid bridge port value (%d).", tmp_int);
								திரும்பு MOSQ_ERR_INVAL;
							}
							cur_bridge->addresses[i].port = (uint16_t)tmp_int;
						}இல்லையெனில்{
							cur_bridge->addresses[i].port = 1883;
						}
						conf__attempt_resolve(cur_bridge->addresses[i].address, "bridge address", MOSQ_LOG_WARNING, "Warning");
					}
					எனில்(cur_bridge->address_மொத்தம் == 0){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Empty address value in configuration.");
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "allow_anonymous")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					PER_LISTENER_ALTERNATIVE(token, "listener_allow_anonymous");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					எனில்(conf__parse_bool(&token, "allow_anonymous", (bool *)&cur_security_options->allow_anonymous, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "allow_duplicate_messages")){
					OPTION_DEPRECATED(token, "The behaviour will default to true.");
					எனில்(conf__parse_bool(&token, "allow_duplicate_messages", &config->allow_duplicate_messages, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "allow_zero_length_clientid")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					எனில்(conf__parse_bool(&token, "allow_zero_length_clientid", &cur_security_options->allow_zero_length_clientid, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strncmp(token, "auth_opt_", strlen("auth_opt_")) || !strncmp(token, "plugin_opt_", strlen("plugin_opt_"))){
					எனில்(reload){
						தொடர்;        /* Auth plugin not currently valid for reloading. */

					}
					REQUIRE_PLUGIN(token);

					எனில்(!strncmp(token, "auth_opt_", strlen("auth_opt_"))){
						prefix_len = strlen("auth_opt_");
					}இல்லையெனில்{
						prefix_len = strlen("plugin_opt_");
					}
					எனில்(strlen(token) < prefix_len + 3){
						/* auth_opt_ == 9, + one digit key == 10, + one space == 11, + one value == 12 */
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid 'plugin_opt_' config option.");
						திரும்பு MOSQ_ERR_INVAL;
					}
					சாவி = mosquitto_strdup(&token[prefix_len]);
					எனில்(!சாவி){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
						திரும்பு MOSQ_ERR_NOMEM;
					}இல்லையெனில் எனில்(STREMPTY(சாவி)){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Empty 'plugin_opt_' config option.");
						mosquitto_FREE(சாவி);
						திரும்பு MOSQ_ERR_INVAL;
					}
					token = saveptr;
					எனில்(token && token[0]){
						போது(token[0] == ' ' || token[0] == '\t'){
							token++;
						}
						கட்டமைப்பு mosquitto_opt *new_options = mosquitto_realloc(cur_plugin->config.options, (size_t)(cur_plugin->config.option_மொத்தம்+1)*அளவு(கட்டமைப்பு mosquitto_opt));
						எனில்(!new_options){
							log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
							mosquitto_FREE(சாவி);
							திரும்பு MOSQ_ERR_NOMEM;
						}
						cur_plugin->config.option_மொத்தம்++;
						cur_plugin->config.options = new_options;
						cur_plugin->config.options[cur_plugin->config.option_மொத்தம்-1].சாவி = சாவி;
						cur_plugin->config.options[cur_plugin->config.option_மொத்தம்-1].மதிப்பு = mosquitto_strdup(token);
						எனில்(!cur_plugin->config.options[cur_plugin->config.option_மொத்தம்-1].மதிப்பு){
							log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
							திரும்பு MOSQ_ERR_NOMEM;
						}
					}இல்லையெனில்{
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", சாவி);
						mosquitto_FREE(சாவி);
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "auth_plugin") || !strcmp(token, "plugin") || !strcmp(token, "global_plugin")){
					எனில்(reload){
						தொடர்;        /* plugin not currently valid for reloading. */
					}
					எனில்(!strcmp(token, "global_plugin")){
						cur_security_options = &db.config->security_options;
					}இல்லையெனில்{
						REQUIRE_LISTENER_IF_PER_LISTENER(token);
						conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					}

					cur_plugin = config__plugin_load(வெற்று, saveptr);
					எனில்(cur_plugin == வெற்று){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(config__plugin_add_secopt(cur_plugin, cur_security_options)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "auth_plugin_deny_special_chars")){
					எனில்(reload){
						தொடர்;        /* Auth plugin not currently valid for reloading. */
					}
					எனில்(!cur_plugin){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: An auth_plugin_deny_special_chars option exists in the config file without a plugin.");
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(conf__parse_bool(&token, "auth_plugin_deny_special_chars", &cur_plugin->config.deny_special_chars, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "plugin_load")){
					எழுத்து *name = வெற்று;

					எனில்(reload){
						தொடர்;        /* plugin not currently valid for reloading. */

					}
					name = strtok_r(வெற்று, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(name, "plugin_load");

					cur_plugin = config__plugin_load(name, saveptr);
					எனில்(cur_plugin == வெற்று){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "plugin_use")){
					எழுத்து *name = வெற்று;

					எனில்(reload){
						தொடர்;        /* plugin not currently valid for reloading. */
					}
					REQUIRE_NON_DEFAULT_LISTENER(token);

					எனில்(conf__parse_string(&token, "plugin_use", &name, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					cur_plugin = config__plugin_find(name);
					எனில்(!cur_plugin){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Plugin '%s' not previously loaded.", name);
						mosquitto_FREE(name);
						திரும்பு MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(name);
					எனில்(config__plugin_add_secopt(cur_plugin, cur_listener->security_options)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "auto_id_prefix")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					OPTION_DEPRECATED(token, "Please use 'listener_auto_id_prefix' instead.");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					எனில்(conf__parse_string(&token, "auto_id_prefix", &cur_security_options->auto_id_prefix, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(cur_security_options->auto_id_prefix){
						எனில்(strlen(cur_security_options->auto_id_prefix) > 50){
							log__printf(வெற்று, MOSQ_LOG_ERR, "Error: auto_id_prefix length must be <= 50.");
							திரும்பு MOSQ_ERR_INVAL;
						}
						cur_security_options->auto_id_prefix_len = (uint16_t)strlen(cur_security_options->auto_id_prefix);
					}இல்லையெனில்{
						cur_security_options->auto_id_prefix_len = 0;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "autosave_interval")){
					எனில்(conf__parse_int(&token, "autosave_interval", &config->autosave_interval, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(config->autosave_interval < 0){
						config->autosave_interval = 0;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "autosave_on_changes")){
					எனில்(conf__parse_bool(&token, "autosave_on_changes", &config->autosave_on_changes, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "bind_address")){
					OPTION_DEPRECATED(token, "");
					config->local_only = false;
					எனில்(reload){
						தொடர்;        /* Rebinding listeners not valid during reloading. */

					}
					எனில்(config__create_default_listener(config, token)){
						திரும்பு MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					எனில்(conf__parse_string(&token, "default listener bind_address", &config->default_listener->host, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(conf__attempt_resolve(config->default_listener->host, "bind_address", MOSQ_LOG_ERR, "Error")){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "bind_interface")){
#ifdef SO_BINDTODEVICE
					எனில்(reload){
						தொடர்;        /* Rebinding listeners not valid during reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					எனில்(conf__parse_string(&token, "bind_interface", &cur_listener->bind_interface, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_ERR, "Error: bind_interface specified but socket option not available.");
					திரும்பு MOSQ_ERR_INVAL;
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "bridge_attempt_unsubscribe")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_bool(&token, "bridge_attempt_unsubscribe", &cur_bridge->attempt_unsubscribe, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "bridge_cafile")){
#எனில் defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					எனில்(conf__parse_string(&token, "bridge_cafile", &cur_bridge->tls_cafile, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "bridge_alpn")){
#எனில் defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_string(&token, "bridge_alpn", &cur_bridge->tls_alpn, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "bridge_ciphers")){
#எனில் defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_string(&token, "bridge_ciphers", &cur_bridge->tls_ciphers, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "bridge_ciphers_tls1.3")){
#எனில் defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_string(&token, "bridge_ciphers_tls1.3", &cur_bridge->tls_13_ciphers, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "bridge_bind_address")){
#எனில் defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_string(&token, "bridge_bind_address", &cur_bridge->bind_address, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "bridge_capath")){
#எனில் defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					எனில்(conf__parse_string(&token, "bridge_capath", &cur_bridge->tls_capath, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "bridge_certfile")){
#எனில் defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					எனில்(conf__parse_string(&token, "bridge_certfile", &cur_bridge->tls_certfile, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "bridge_identity")){
#எனில் defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					எனில்(conf__parse_string(&token, "bridge_identity", &cur_bridge->tls_psk_identity, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "bridge_insecure")){
#எனில் defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_bool(&token, "bridge_insecure", &cur_bridge->tls_insecure, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(cur_bridge->tls_insecure){
						log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge '%s' using insecure mode.", cur_bridge->name);
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "bridge_require_ocsp")){
#எனில் defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_bool(&token, "bridge_require_ocsp", &cur_bridge->tls_ocsp_required, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "bridge_max_packet_size")){
#எனில் defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_int(&token, "bridge_max_packet_size", &tmp_int, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(tmp_int < 0){
						tmp_int = 0;
					}
					cur_bridge->maximum_packet_size = (uint32_t)tmp_int;
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "bridge_max_topic_alias")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_int(&token, "bridge_max_topic_alias", &tmp_int, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}

					எனில்(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: bridge_max_topic_alias must be > 0 and <= 65535.");
						திரும்பு MOSQ_ERR_INVAL;
					}
					cur_bridge->max_topic_alias = (uint16_t)tmp_int;
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "bridge_outgoing_retain")){
#எனில் defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_bool(&token, "bridge_outgoing_retain", &cur_bridge->outgoing_retain, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "bridge_keyfile")){
#எனில் defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					mosquitto_FREE(cur_bridge->tls_keyfile);
					எனில்(conf__parse_string(&token, "bridge_keyfile", &cur_bridge->tls_keyfile, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "bridge_protocol_version")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(வெற்று, "", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_protocol_version");

					எனில்(!strcmp(token, "mqttv31")){
						cur_bridge->protocol_version = mosq_p_mqtt31;
					}இல்லையெனில் எனில்(!strcmp(token, "mqttv311")){
						cur_bridge->protocol_version = mosq_p_mqtt311;
					}இல்லையெனில் எனில்(!strcmp(token, "mqttv50")){
						cur_bridge->protocol_version = mosq_p_mqtt5;
					}இல்லையெனில்{
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid 'bridge_protocol_version' value (%s).", token);
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "bridge_psk")){
#எனில் defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					எனில்(conf__parse_string(&token, "bridge_psk", &cur_bridge->tls_psk, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "bridge_receive_maximum")){
#எனில் defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_int(&token, "bridge_receive_maximum", &tmp_int, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(tmp_int <= 0){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be greater than 0.");
						திரும்பு MOSQ_ERR_INVAL;
					}இல்லையெனில் எனில்((uint64_t)tmp_int > (uint64_t)UINT16_MAX){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be lower than %u.", UINT16_MAX);
						திரும்பு MOSQ_ERR_INVAL;
					}
					cur_bridge->receive_maximum = (uint16_t)tmp_int;
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "bridge_reload_type")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(வெற்று, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_reload_type");

					எனில்(!strcmp(token, "lazy")){
						cur_bridge->reload_type = brt_lazy;
					}இல்லையெனில் எனில்(!strcmp(token, "immediate")){
						cur_bridge->reload_type = brt_immediate;
					}இல்லையெனில்{
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid 'bridge_reload_type' value in configuration (%s).", token);
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "bridge_session_expiry_interval")){
#எனில் defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_int(&token, "bridge_session_expiry_interval", &tmp_int, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(tmp_int < 0){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must not be negative.");
						திரும்பு MOSQ_ERR_INVAL;
					}இல்லையெனில் எனில்((uint64_t)tmp_int > (uint64_t)MQTT_SESSION_EXPIRY_NEVER){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must be lower than %u.", MQTT_SESSION_EXPIRY_NEVER);
						திரும்பு MOSQ_ERR_INVAL;
					}
					cur_bridge->session_expiry_interval = (uint32_t)tmp_int;
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "bridge_tcp_keepalive")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);

					எனில்(conf__parse_int(&token, "bridge_tcp_keepalive_idle", &tmp_int, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(tmp_int <= 0){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: invalid TCP keepalive idle value.");
						திரும்பு MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_idle = (unsigned முழு)tmp_int;

					எனில்(conf__parse_int(&token, "bridge_tcp_keepalive_interval", &tmp_int, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(tmp_int <= 0){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: invalid TCP keepalive interval value.");
						திரும்பு MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_interval = (unsigned முழு)tmp_int;

					எனில்(conf__parse_int(&token, "bridge_tcp_keepalive_counter", &tmp_int, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(tmp_int <= 0){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: invalid TCP keepalive counter value.");
						திரும்பு MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_counter = (unsigned முழு)tmp_int;
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "bridge_tcp_user_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
#ifdef WITH_TCP_USER_TIMEOUT
					எனில்(conf__parse_int(&token, "bridge_tcp_user_timeout", &tmp_int, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(tmp_int < 0){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: invalid TCP user timeout value.");
						திரும்பு MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_user_timeout = tmp_int;
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge TCP user timeout support not available.");
#endif
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "bridge_tls_use_os_certs")){
#எனில் defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_bool(&token, "bridge_tls_use_os_certs", &cur_bridge->tls_use_os_certs, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "bridge_tls_version")){
#எனில் defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_string(&token, "bridge_tls_version", &cur_bridge->tls_version, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "cafile")){
#எனில் defined(WITH_TLS)
					REQUIRE_LISTENER(token);
					எனில்(cur_listener->psk_hint){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						திரும்பு MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->cafile);
					எனில்(conf__parse_string(&token, "cafile", &cur_listener->cafile, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "capath")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->capath);
					எனில்(conf__parse_string(&token, "capath", &cur_listener->capath, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "certfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					எனில்(cur_listener->psk_hint){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						திரும்பு MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->certfile);
					எனில்(conf__parse_string(&token, "certfile", &cur_listener->certfile, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "check_retain_source")){
					எனில்(conf__parse_bool(&token, "check_retain_source", &config->check_retain_source, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "ciphers")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers);
					எனில்(conf__parse_string(&token, "ciphers", &cur_listener->ciphers, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "ciphers_tls1.3")){
#எனில் defined(WITH_TLS) && (!defined(LIBRESSL_VERSION_எண்) || LIBRESSL_VERSION_எண் > 0x3040000FL)
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers_tls13);
					எனில்(conf__parse_string(&token, "ciphers_tls1.3", &cur_listener->ciphers_tls13, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: ciphers_tls1.3 support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "clientid") || !strcmp(token, "remote_clientid")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_string(&token, "bridge remote clientid", &cur_bridge->remote_clientid, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "cleansession")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_bool(&token, "cleansession", &cur_bridge->clean_start, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "local_cleansession")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_bool(&token, "local_cleansession", (bool *)&cur_bridge->clean_start_local, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "clientid_prefixes")){
					OPTION_DEPRECATED(token, "");
					mosquitto_FREE(config->clientid_prefixes);
					எனில்(conf__parse_string(&token, "clientid_prefixes", &config->clientid_prefixes, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "connection")){
#ifdef WITH_BRIDGE
					token = strtok_r(வெற்று, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "connection");

					/* Check for existing bridge name. */
					க்கு(i=0; i<config->bridge_மொத்தம்; i++){
						எனில்(!strcmp(config->bridges[i]->name, token)){
							log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Duplicate bridge name '%s'.", token);
							திரும்பு MOSQ_ERR_INVAL;
						}
					}

					கட்டமைப்பு mosquitto__bridge **bridges_new = mosquitto_realloc(config->bridges, (size_t)(config->bridge_மொத்தம்+1)*அளவு(கட்டமைப்பு mosquitto__bridge *));
					எனில்(!bridges_new){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
						திரும்பு MOSQ_ERR_NOMEM;
					}
					config->bridges = bridges_new;
					cur_bridge = mosquitto_malloc(அளவு(கட்டமைப்பு mosquitto__bridge));
					எனில்(!cur_bridge){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
						திரும்பு MOSQ_ERR_NOMEM;
					}

					config->bridge_மொத்தம்++;
					config->bridges[config->bridge_மொத்தம்-1] = cur_bridge;

					memset(cur_bridge, 0, அளவு(கட்டமைப்பு mosquitto__bridge));
					cur_bridge->name = mosquitto_strdup(token);
					எனில்(!cur_bridge->name){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
						திரும்பு MOSQ_ERR_NOMEM;
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
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "connection_messages")){
					எனில்(conf__parse_bool(&token, token, &config->connection_messages, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "crlfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->crlfile);
					எனில்(conf__parse_string(&token, "crlfile", &cur_listener->crlfile, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "dhparamfile")){
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: dhparamfile is no longer required.");
				}இல்லையெனில் எனில்(!strcmp(token, "disable_client_cert_date_checks")){
#ifdef WITH_TLS
					REQUIRE_LISTENER(token);
					எனில்(conf__parse_bool(&token, "disable_client_cert_date_checks", &cur_listener->disable_client_cert_date_checks, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "enable_control_api")){
#ifdef WITH_CONTROL
					எனில்(conf__parse_bool(&token, "enable_control_api", &config->enable_control_api, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: $CONTROL support not available (enable_control_api).");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "enable_proxy_protocol")){
#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(வெற்று, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					திரும்பு MOSQ_ERR_INVAL;
#endif
					REQUIRE_LISTENER(token);
					எனில்(conf__parse_int(&token, "enable_proxy_protocol", &cur_listener->enable_proxy_protocol, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(cur_listener->enable_proxy_protocol < 1 || cur_listener->enable_proxy_protocol > 2){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: enable_proxy_protocol must be 1 or 2.");
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "global_max_clients")){
					எனில்(conf__parse_int(&token, "global_max_clients", &config->global_max_clients, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "global_max_connections")){
					எனில்(conf__parse_int(&token, "global_max_connections", &config->global_max_connections, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "http_dir")){
#எனில் defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
					எனில்(reload){
						தொடர்;        /* Not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					எனில்(conf__parse_string(&token, "http_dir", &cur_listener->http_dir, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#ifdef WIN32
					எழுத்து *http_dir_canonical = _fullpath(வெற்று, cur_listener->http_dir, 0);
					மாறிலி எழுத்து sep = '\\';
#இல்லையெனில்
					எழுத்து *http_dir_canonical = realpath(cur_listener->http_dir, வெற்று);
					மாறிலி எழுத்து sep = '/';
#endif
					எனில்(!http_dir_canonical){
						திரும்பு MOSQ_ERR_NOMEM;
					}
					size_t http_dir_len = strlen(http_dir_canonical) + அளவு(sep) + 1;
					எழுத்து *http_dir_canonical_sep = mosquitto_calloc(http_dir_len, அளவு(எழுத்து));
					எனில்(!http_dir_canonical_sep){
						விடுவி(http_dir_canonical);
						திரும்பு MOSQ_ERR_NOMEM;
					}
					snprintf(http_dir_canonical_sep, http_dir_len, "%s%c", http_dir_canonical, sep);
					விடுவி(http_dir_canonical);
					mosquitto_FREE(cur_listener->http_dir);
					cur_listener->http_dir = http_dir_canonical_sep;
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: http_dir support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "idle_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_int(&token, "idle_timeout", &cur_bridge->idle_timeout, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(cur_bridge->idle_timeout < 1){
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "idle_timeout interval too low, using 1 second.");
						cur_bridge->idle_timeout = 1;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "include_dir")){
					எனில்(level == 0){
						எழுத்து **files;
						முழு file_மொத்தம்;

						/* Only process include_dir from the main config file. */
						token = strtok_r(வெற்று, "", &saveptr);
						REQUIRE_NON_EMPTY_OPTION(token, "include_dir");

						rc = config__get_dir_files(token, &files, &file_மொத்தம்);
						எனில்(rc){
							திரும்பு rc;
						}

						க்கு(i=0; i<file_மொத்தம்; i++){
							log__printf(வெற்று, MOSQ_LOG_INFO, "Loading config file '%s'", files[i]);

							rc = config__read_file(config, reload, files[i], cr, level+1, &lineno_ext);
							எனில்(rc){
								எனில்(lineno_ext > 0){
									log__printf(வெற்று, MOSQ_LOG_ERR, "Error found at '%s:%d'.", files[i], lineno_ext);
								}
								/* Free happens below */
								நிறுத்து;
							}
						}
						க்கு(i=0; i<file_மொத்தம்; i++){
							mosquitto_FREE(files[i]);
						}
						mosquitto_FREE(files);
						எனில்(rc){
							திரும்பு rc;    /* This returns if config__read_file() fails above */
						}
					}இல்லையெனில்{
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: The include_dir option is only valid in the main configuration file.");
						திரும்பு 1;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "keepalive_interval")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_int(&token, "keepalive_interval", &tmp_int, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(tmp_int > UINT16_MAX){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Bridge keepalive value too high.");
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(tmp_int < 5){
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "keepalive interval too low, using 5 seconds.");
						tmp_int = 5;
					}
					cur_bridge->keepalive = (uint16_t)tmp_int;
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "keyfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->keyfile);
					எனில்(conf__parse_string(&token, "keyfile", &cur_listener->keyfile, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "listener")){
					config->local_only = false;

					எனில்(conf__parse_int(&token, "listener port", &tmp_int, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#ifdef WITH_UNIX_SOCKETS
					எனில்(tmp_int < 0 || tmp_int > UINT16_MAX){
#இல்லையெனில்
					எனில்(tmp_int < 1 || tmp_int > UINT16_MAX){
#endif
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						திரும்பு MOSQ_ERR_INVAL;
					}

					/* Look for bind address / unix socket path */
					token = strtok_r(வெற்று, " ", &saveptr);
					எனில்(token != வெற்று && token[0] == '#'){
						token = வெற்று;
					}

					எனில்(tmp_int == 0 && token == வெற்று){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: A listener with port 0 must provide a Unix socket path.");
						திரும்பு MOSQ_ERR_INVAL;
					}

					எனில்(reload){
						/* We reload listeners settings based on port number/unix socket path.
						 * If the port number/unix path doesn't already exist, exit with a complaint. */
						cur_listener = வெற்று;
#ifdef WITH_UNIX_SOCKETS
						எனில்(tmp_int == 0){
							க்கு(i=0; i<config->listener_மொத்தம்; i++){
								எனில்(config->listeners[i].unix_socket_path != வெற்று
										&& strcmp(config->listeners[i].unix_socket_path, token) == 0){

									cur_listener = &config->listeners[i];
									நிறுத்து;
								}
							}
						}இல்லையெனில்
#endif
						{
							க்கு(i=0; i<config->listener_மொத்தம்; i++){
								எனில்(config->listeners[i].port == tmp_int){
									/* Now check we have a matching bind address, if defined */
									எனில்(config->listeners[i].host){
										எனில்(token && !strcmp(config->listeners[i].host, token)){
											/* They both have a bind address, and they match */
											cur_listener = &config->listeners[i];
											நிறுத்து;
										}
									}இல்லையெனில்{
										எனில்(token == வெற்று){
											/* Neither this config nor the new config have a bind address,
											 * so they match. */
											cur_listener = &config->listeners[i];
											நிறுத்து;
										}
									}
								}
							}
						}
						எனில்(!cur_listener){
							log__printf(வெற்று, MOSQ_LOG_ERR, "Error: It is not currently possible to add/remove listeners when reloading the config file.");
							திரும்பு MOSQ_ERR_INVAL;
						}
					}இல்லையெனில்{
						எனில்(config__add_listener(config)){
							log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
							திரும்பு MOSQ_ERR_NOMEM;
						}
						cur_listener = &config->listeners[config->listener_மொத்தம்-1];
					}

					listener__set_defaults(cur_listener);
					cur_listener->port = (uint16_t)tmp_int;

					mosquitto_FREE(cur_listener->host);

#ifdef WITH_UNIX_SOCKETS
					mosquitto_FREE(cur_listener->unix_socket_path);
#endif

					எனில்(token){
#ifdef WITH_UNIX_SOCKETS
						எனில்(cur_listener->port == 0){
							cur_listener->unix_socket_path = mosquitto_strdup(token);
						}இல்லையெனில்
#endif
						{
							cur_listener->host = mosquitto_strdup(token);
						}
					}
				}இல்லையெனில் எனில்(!strcmp(token, "listener_allow_anonymous")){
					REQUIRE_LISTENER(token);
					எனில்(conf__parse_bool(&token, "listener_allow_anonymous", (bool *)&cur_listener->security_options->allow_anonymous, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "listener_auto_id_prefix")){
					REQUIRE_LISTENER(token);
					எனில்(conf__parse_string(&token, "listener_auto_id_prefix", &cur_listener->security_options->auto_id_prefix, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(cur_listener->security_options->auto_id_prefix){
						எனில்(strlen(cur_listener->security_options->auto_id_prefix) > 50){
							log__printf(வெற்று, MOSQ_LOG_ERR, "Error: 'listener_auto_id_prefix' length must be <= 50.");
							திரும்பு MOSQ_ERR_INVAL;
						}
						cur_listener->security_options->auto_id_prefix_len = (uint16_t)strlen(cur_listener->security_options->auto_id_prefix);
					}இல்லையெனில்{
						cur_listener->security_options->auto_id_prefix_len = 0;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "local_clientid")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_string(&token, "bridge local clientd", &cur_bridge->local_clientid, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "local_password")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_string(&token, "bridge local_password", &cur_bridge->local_password, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "local_username")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_string(&token, "bridge local_username", &cur_bridge->local_username, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "log_dest")){
					token = strtok_r(வெற்று, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_dest");

					cr->log_dest_set = 1;
					எனில்(!strcmp(token, "none")){
						cr->log_dest = MQTT3_LOG_NONE;
					}இல்லையெனில் எனில்(!strcmp(token, "syslog")){
						cr->log_dest |= MQTT3_LOG_SYSLOG;
					}இல்லையெனில் எனில்(!strcmp(token, "stdout")){
						cr->log_dest |= MQTT3_LOG_STDOUT;
					}இல்லையெனில் எனில்(!strcmp(token, "stderr")){
						cr->log_dest |= MQTT3_LOG_STDERR;
					}இல்லையெனில் எனில்(!strcmp(token, "topic")){
						cr->log_dest |= MQTT3_LOG_TOPIC;
					}இல்லையெனில் எனில்(!strcmp(token, "dlt")){
						cr->log_dest |= MQTT3_LOG_DLT;
#ifdef ANDROID
					}இல்லையெனில் எனில்(!strcmp(token, "android")){
						cr->log_dest |= MQTT3_LOG_ANDROID;
#endif
					}இல்லையெனில் எனில்(!strcmp(token, "file")){
						எனில்(config->log_fptr || config->log_file){
							log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Duplicate \"log_dest file\" value.");
							திரும்பு MOSQ_ERR_INVAL;
						}
						/* Get remaining string. */
						token = saveptr;
						எனில்(token && token[0]){
							போது(token[0] == ' ' || token[0] == '\t'){
								token++;
							}
						}
						/* Duplicate "token" check here saves a log__printf() */
						எனில்(token && token[0]){
							config->log_file = mosquitto_strdup(token);
							எனில்(!config->log_file){
								log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
								திரும்பு MOSQ_ERR_NOMEM;
							}
							cr->log_dest |= MQTT3_LOG_FILE;
						}இல்லையெனில்{
							log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Empty \"log_dest file\" value in configuration.");
							திரும்பு MOSQ_ERR_INVAL;
						}
						cr->log_dest |= MQTT3_LOG_FILE;
					}இல்லையெனில்{
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid 'log_dest' value (%s).", token);
						திரும்பு MOSQ_ERR_INVAL;
					}
#எனில் defined(WIN32) || defined(__CYGWIN__)
					எனில்(service_handle){
						எனில்(cr->log_dest == MQTT3_LOG_STDOUT || cr->log_dest == MQTT3_LOG_STDERR){
							log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Cannot log to stdout/stderr when running as a Windows service.");
							திரும்பு MOSQ_ERR_INVAL;
						}
					}
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "log_facility")){
#எனில் defined(WIN32) || defined(__CYGWIN__)
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: log_facility not supported on Windows.");
#இல்லையெனில்
					எனில்(conf__parse_int(&token, "log_facility", &tmp_int, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					தேர்வு(tmp_int){
						நிலை 0:
							config->log_facility = LOG_LOCAL0;
							நிறுத்து;
						நிலை 1:
							config->log_facility = LOG_LOCAL1;
							நிறுத்து;
						நிலை 2:
							config->log_facility = LOG_LOCAL2;
							நிறுத்து;
						நிலை 3:
							config->log_facility = LOG_LOCAL3;
							நிறுத்து;
						நிலை 4:
							config->log_facility = LOG_LOCAL4;
							நிறுத்து;
						நிலை 5:
							config->log_facility = LOG_LOCAL5;
							நிறுத்து;
						நிலை 6:
							config->log_facility = LOG_LOCAL6;
							நிறுத்து;
						நிலை 7:
							config->log_facility = LOG_LOCAL7;
							நிறுத்து;
						இயல்பு:
							log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid 'log_facility' value (%d).", tmp_int);
							திரும்பு MOSQ_ERR_INVAL;
					}
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "log_timestamp")){
					எனில்(conf__parse_bool(&token, token, &config->log_timestamp, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "log_timestamp_format")){
					எனில்(conf__parse_string(&token, token, &config->log_timestamp_format, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "log_type")){
					token = strtok_r(வெற்று, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_type");

					cr->log_type_set = 1;
					எனில்(!strcmp(token, "none")){
						cr->log_type = MOSQ_LOG_NONE;
					}இல்லையெனில் எனில்(!strcmp(token, "information")){
						cr->log_type |= MOSQ_LOG_INFO;
					}இல்லையெனில் எனில்(!strcmp(token, "notice")){
						cr->log_type |= MOSQ_LOG_NOTICE;
					}இல்லையெனில் எனில்(!strcmp(token, "warning")){
						cr->log_type |= MOSQ_LOG_WARNING;
					}இல்லையெனில் எனில்(!strcmp(token, "error")){
						cr->log_type |= MOSQ_LOG_ERR;
					}இல்லையெனில் எனில்(!strcmp(token, "debug")){
						cr->log_type |= MOSQ_LOG_DEBUG;
					}இல்லையெனில் எனில்(!strcmp(token, "subscribe")){
						cr->log_type |= MOSQ_LOG_SUBSCRIBE;
					}இல்லையெனில் எனில்(!strcmp(token, "unsubscribe")){
						cr->log_type |= MOSQ_LOG_UNSUBSCRIBE;
					}இல்லையெனில் எனில்(!strcmp(token, "internal")){
						cr->log_type |= MOSQ_LOG_INTERNAL;
#ifdef WITH_WEBSOCKETS
					}இல்லையெனில் எனில்(!strcmp(token, "websockets")){
						cr->log_type |= MOSQ_LOG_WEBSOCKETS;
#endif
					}இல்லையெனில் எனில்(!strcmp(token, "all")){
						cr->log_type = MOSQ_LOG_ALL;
					}இல்லையெனில்{
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid 'log_type' value (%s).", token);
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "max_connections")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					எனில்(conf__parse_int(&token, token, &cur_listener->max_connections, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(cur_listener->max_connections < 0){
						cur_listener->max_connections = -1;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "maximum_qos") || !strcmp(token, "max_qos")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					எனில்(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(tmp_int < 0 || tmp_int > 2){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: 'max_qos' must be between 0 and 2 inclusive.");
						திரும்பு MOSQ_ERR_INVAL;
					}
					cur_listener->max_qos = (uint8_t)tmp_int;
				}இல்லையெனில் எனில்(!strcmp(token, "max_inflight_bytes")){
					எனில்(conf__parse_int(&token, "max_inflight_bytes", &tmp_int, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_inflight_bytes = (size_t)tmp_int;
				}இல்லையெனில் எனில்(!strcmp(token, "max_inflight_messages")){
					எனில்(conf__parse_int(&token, "max_inflight_messages", &tmp_int, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(tmp_int < 0 || tmp_int == UINT16_MAX){
						tmp_int = 0;
					}இல்லையெனில் எனில்(tmp_int > UINT16_MAX){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: 'max_inflight_messages' must be <= 65535.");
						திரும்பு MOSQ_ERR_INVAL;
					}
					config->max_inflight_messages = (uint16_t)tmp_int;
				}இல்லையெனில் எனில்(!strcmp(token, "max_keepalive")){
					எனில்(conf__parse_int(&token, "max_keepalive", &tmp_int, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid 'max_keepalive' value (%d).", tmp_int);
						திரும்பு MOSQ_ERR_INVAL;
					}
					config->max_keepalive = (uint16_t)tmp_int;
				}இல்லையெனில் எனில்(!strcmp(token, "max_packet_size")){
					எனில்(conf__parse_int(&token, "max_packet_size", &tmp_int, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(tmp_int < 20){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: 'max_packet_size' must be >= 20.");
						திரும்பு MOSQ_ERR_INVAL;
					}
					config->max_packet_size = (uint32_t)tmp_int;
				}இல்லையெனில் எனில்(!strcmp(token, "max_queued_bytes")){
					எனில்(conf__parse_int(&token, "max_queued_bytes", &tmp_int, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_bytes = (size_t)tmp_int;
				}இல்லையெனில் எனில்(!strcmp(token, "max_queued_messages")){
					எனில்(conf__parse_int(&token, "max_queued_messages", &tmp_int, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_messages = tmp_int;
				}இல்லையெனில் எனில்(!strcmp(token, "memory_limit")){
					ssize_t lim;
					எனில்(conf__parse_ssize_t(&token, "memory_limit", &lim, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(lim < 0){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid 'memory_limit' value (%ld).", lim);
						திரும்பு MOSQ_ERR_INVAL;
					}
					mosquitto_memory_set_limit((size_t)lim);
				}இல்லையெனில் எனில்(!strcmp(token, "message_size_limit")){
					log__printf(வெற்று, MOSQ_LOG_NOTICE, "Note: It is recommended to replace `message_size_limit` with `max_packet_size`.");
					எனில்(conf__parse_int(&token, "message_size_limit", (முழு *)&config->message_size_limit, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(config->message_size_limit > MQTT_MAX_PAYLOAD){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid 'message_size_limit' value (%u).", config->message_size_limit);
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "mount_point")){
					REQUIRE_LISTENER(token);
					mosquitto_FREE(cur_listener->mount_point);
					எனில்(conf__parse_string(&token, "mount_point", &cur_listener->mount_point, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(mosquitto_pub_topic_check(cur_listener->mount_point) != MOSQ_ERR_SUCCESS){
						log__printf(வெற்று, MOSQ_LOG_ERR,
								"Error: Invalid 'mount_point' value (%s). Does it contain a wildcard character?",
								cur_listener->mount_point);
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "notifications")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_bool(&token, "notifications", &cur_bridge->notifications, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "notifications_local_only")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_bool(&token, "notifications_local_only", &cur_bridge->notifications_local_only, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "notification_topic")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_string(&token, "notification_topic", &cur_bridge->notification_topic, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "password") || !strcmp(token, "remote_password")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_string(&token, "bridge remote_password", &cur_bridge->remote_password, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "password_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->password_data.password_file);
					எனில்(conf__parse_string(&token, "password_file", &cur_security_options->password_data.password_file, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "per_listener_settings")){
					OPTION_DEPRECATED(token, "Please see the documentation for how to achieve the same effect.");
					எனில்(config->per_listener_settings){
						/* Once this is set, don't let it be unset. It should be the first config option ideally. */
						தொடர்;
					}
					எனில்(conf__parse_bool(&token, "per_listener_settings", &config->per_listener_settings, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(cur_security_options && config->per_listener_settings){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: per_listener_settings must be set before any other security settings.");
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "persistence") || !strcmp(token, "retained_persistence")){
					எனில்(conf__parse_bool(&token, token, &config->persistence, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "persistence_file")){
					எனில்(conf__parse_string(&token, "persistence_file", &config->persistence_file, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "persistence_location")){
					எனில்(conf__parse_string(&token, "persistence_location", &config->persistence_location, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "persistent_client_expiration")){
					token = strtok_r(வெற்று, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "persistent_client_expiration");

					time_t expiration_mult;

					தேர்வு(token[strlen(token)-1]){
						நிலை 's':
							expiration_mult = 1;
							நிறுத்து;
						நிலை 'h':
							expiration_mult = 3600;
							நிறுத்து;
						நிலை 'd':
							expiration_mult = 86400;
							நிறுத்து;
						நிலை 'w':
							expiration_mult = 86400*7;
							நிறுத்து;
						நிலை 'm':
							expiration_mult = 86400*30;
							நிறுத்து;
						நிலை 'y':
							expiration_mult = 86400*365;
							நிறுத்து;
						இயல்பு:
							log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
							திரும்பு MOSQ_ERR_INVAL;
					}
					token[strlen(token)-1] = '\0';
					config->persistent_client_expiration = atoi(token)*expiration_mult;
					எனில்(config->persistent_client_expiration <= 0){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "pid_file")){
					எனில்(reload){
						தொடர்;        /* pid file not valid for reloading. */
					}
					எனில்(conf__parse_string(&token, "pid_file", &config->pid_file, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "port")){
					OPTION_DEPRECATED(token, "Please use 'listener' instead.");
					config->local_only = false;
					எனில்(reload){
						தொடர்;        /* Listeners not valid for reloading. */

					}
					எனில்(config__create_default_listener(config, token)){
						திரும்பு MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					எனில்(config->default_listener->port){
						log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Default listener port specified multiple times. Only the latest will be used.");
					}
					எனில்(conf__parse_int(&token, "port", &tmp_int, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(tmp_int < 1 || tmp_int > UINT16_MAX){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						திரும்பு MOSQ_ERR_INVAL;
					}
					config->default_listener->port = (uint16_t)tmp_int;
				}இல்லையெனில் எனில்(!strcmp(token, "protocol")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(வெற்று, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "protocol");

					எனில்(!strcmp(token, "mqtt")){
						cur_listener->protocol = mp_mqtt;
						/*
						}else if(!strcmp(token, "mqttsn")){
						    cur_listener->protocol = mp_mqttsn;
						*/
					}இல்லையெனில் எனில்(!strcmp(token, "websockets")){
#ifdef WITH_WEBSOCKETS
						cur_listener->protocol = mp_websockets;
#இல்லையெனில்
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Websockets support not available.");
						திரும்பு MOSQ_ERR_INVAL;
#endif
					}இல்லையெனில் எனில்(!strcmp(token, "http_api")){
#ifdef WITH_HTTP_API
						cur_listener->protocol = mp_http_api;
#இல்லையெனில்
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: HTTP API support not available.");
						திரும்பு MOSQ_ERR_INVAL;
#endif
					}இல்லையெனில்{
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid 'protocol' value (%s).", token);
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "proxy_protocol_v2_require_tls")){
#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(வெற்று, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					திரும்பு MOSQ_ERR_INVAL;
#endif
					REQUIRE_LISTENER(token);
					எனில்(conf__parse_bool(&token, "proxy_protocol_v2_require_tls", &cur_listener->proxy_protocol_v2_require_tls, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "psk_file")){
#ifdef FINAL_WITH_TLS_PSK
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					எனில்(cur_listener && cur_listener->certfile){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						திரும்பு MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_security_options->psk_file);
					எனில்(conf__parse_string(&token, "psk_file", &cur_security_options->psk_file, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "psk_hint")){
#ifdef FINAL_WITH_TLS_PSK
					எனில்(reload){
						தொடர்;        /* PSK file not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					எனில்(conf__parse_string(&token, "psk_hint", &cur_listener->psk_hint, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "queue_qos0_messages")){
					எனில்(conf__parse_bool(&token, token, &config->queue_qos0_messages, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "require_certificate")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					எனில்(conf__parse_bool(&token, "require_certificate", &cur_listener->require_certificate, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "restart_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					cur_bridge->backoff_cap = 0; /* set backoff to constant mode, unless cap is specified further down */
					token = strtok_r(வெற்று, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "restart_timeout");

					cur_bridge->restart_timeout = atoi(token);
					cur_bridge->backoff_base = 0;
					cur_bridge->backoff_cap = 0;
					எனில்(cur_bridge->restart_timeout < 1){
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "restart_timeout interval too low, using 1 second.");
						cur_bridge->restart_timeout = 1;
					}இல்லையெனில் எனில்(cur_bridge->restart_timeout > 3600){
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "restart_timeout interval too high, using 3600 seconds.");
						cur_bridge->restart_timeout = 3600;
					}
					token = strtok_r(வெற்று, " ", &saveptr);
					எனில்(token){
						cur_bridge->backoff_base = cur_bridge->restart_timeout;
						cur_bridge->backoff_cap = atoi(token);
						எனில்(cur_bridge->backoff_cap < cur_bridge->backoff_base){
							log__printf(வெற்று, MOSQ_LOG_ERR, "Error: backoff cap is lower than the base in restart_timeout.");
							திரும்பு MOSQ_ERR_INVAL;
						}இல்லையெனில் எனில்(cur_bridge->backoff_cap > 7200){
							log__printf(வெற்று, MOSQ_LOG_ERR, "Error: backoff cap too high, using 7200 seconds.");
							cur_bridge->backoff_cap = 7200;
						}

						token = strtok_r(வெற்று, " ", &saveptr);
						எனில்(token){
							cur_bridge->stable_connection_period = atoi(token);
							எனில்(cur_bridge->stable_connection_period < 0){
								log__printf(வெற்று, MOSQ_LOG_ERR, "Error: stable connection period cannot be negative.");
								திரும்பு MOSQ_ERR_INVAL;
							}
						}
					}
					cur_bridge->restart_timeout *= 1000; /* backoff is tracked in ms */
					cur_bridge->backoff_base *= 1000;
					cur_bridge->backoff_cap *= 1000;
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "retain_available")){
					எனில்(conf__parse_bool(&token, token, &config->retain_available, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "retain_expiry_interval")){
					எனில்(conf__parse_int(&token, token, &config->retain_expiry_interval, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(config->retain_expiry_interval < 1){
						log__printf(வெற்று, MOSQ_LOG_WARNING, "Error: retain_expiry_interval must be >= 1.");
						திரும்பு MOSQ_ERR_INVAL;
					}இல்லையெனில் எனில்(config->retain_expiry_interval > 10000000){
						log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: retain_expiry_interval being capped at 19 years.");
						config->retain_expiry_interval = 10000000;
					}
					config->retain_expiry_interval *= 60;
				}இல்லையெனில் எனில்(!strcmp(token, "retry_interval")){
					OPTION_UNAVAILABLE(token);
				}இல்லையெனில் எனில்(!strcmp(token, "round_robin")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_bool(&token, "round_robin", &cur_bridge->round_robin, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "set_tcp_nodelay")){
					எனில்(conf__parse_bool(&token, "set_tcp_nodelay", &config->set_tcp_nodelay, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "start_type")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(வெற்று, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					எனில்(!strcmp(token, "automatic")){
						cur_bridge->start_type = bst_automatic;
					}இல்லையெனில் எனில்(!strcmp(token, "lazy")){
						cur_bridge->start_type = bst_lazy;
					}இல்லையெனில் எனில்(!strcmp(token, "manual")){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Manual start_type not supported.");
						திரும்பு MOSQ_ERR_INVAL;
					}இல்லையெனில் எனில்(!strcmp(token, "once")){
						cur_bridge->start_type = bst_once;
					}இல்லையெனில்{
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid 'start_type' value in configuration (%s).", token);
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "socket_domain")){
					எனில்(reload){
						தொடர்;        /* socket_domain not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(வெற்று, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					எனில்(!strcmp(token, "ipv4")){
						cur_listener->socket_domain = AF_INET;
					}இல்லையெனில் எனில்(!strcmp(token, "ipv6")){
						cur_listener->socket_domain = AF_INET6;
					}இல்லையெனில்{
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid 'socket_domain' value '%s' in configuration.", token);
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "sys_interval")){
					எனில்(conf__parse_int(&token, "sys_interval", &config->sys_interval, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(config->sys_interval < 0 || config->sys_interval > 65535){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid 'sys_interval' value (%d).", config->sys_interval);
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "threshold")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_int(&token, "threshold", &cur_bridge->threshold, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(cur_bridge->threshold < 1){
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "threshold too low, using 1 message.");
						cur_bridge->threshold = 1;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "tls_engine")){
#ifdef WITH_TLS
					எனில்(reload){
						தொடர்;        /* tls_engine not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					எனில்(conf__parse_string(&token, "tls_engine", &cur_listener->tls_engine, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "tls_engine_kpass_sha1")){
#ifdef WITH_TLS
					எனில்(reload){
						தொடர்;        /* tls_engine not valid for reloading. */

					}
					எழுத்து *kpass_sha = வெற்று, *kpass_sha_bin = வெற்று;
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					எனில்(conf__parse_string(&token, "tls_engine_kpass_sha1", &kpass_sha, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(mosquitto__hex2bin_sha1(kpass_sha, (unsigned எழுத்து **)&kpass_sha_bin) != MOSQ_ERR_SUCCESS){
						mosquitto_FREE(kpass_sha);
						திரும்பு MOSQ_ERR_INVAL;
					}
					mosquitto_free(cur_listener->tls_engine_kpass_sha1);
					cur_listener->tls_engine_kpass_sha1 = kpass_sha_bin;
					mosquitto_FREE(kpass_sha);
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "tls_keyform")){
#ifdef WITH_TLS
					எனில்(reload){
						தொடர்;        /* tls_engine not valid for reloading. */

					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					எழுத்து *keyform = வெற்று;
					எனில்(conf__parse_string(&token, "tls_keyform", &keyform, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					cur_listener->tls_keyform = mosq_k_pem;
					எனில்(!strcmp(keyform, "engine")){
						cur_listener->tls_keyform = mosq_k_engine;
					}
					mosquitto_FREE(keyform);
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "tls_version")){
#எனில் defined(WITH_TLS)
					எனில்(reload){
						தொடர்;        /* tls_version not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					எனில்(conf__parse_string(&token, "tls_version", &cur_listener->tls_version, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "topic")){
#ifdef WITH_BRIDGE
					எழுத்து *topic = வெற்று;
					எண்ணிக்கை mosquitto__bridge_direction direction = bd_out;
					uint8_t qos = 0;
					எழுத்து *local_prefix = வெற்று, *remote_prefix = வெற்று;

					REQUIRE_BRIDGE(token);

					token = strtok_r(வெற்று, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "topic");

					// Check if the topic is quoted (e.g. for spaces within topic names), but not the
					// special case of ""
					எனில்(token[0] == '"' && token [1] != '"'){
						எனில்(strchr(saveptr, '"') == வெற்று){
							log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Missing closing quote in topic value (%s).", saveptr);
							திரும்பு MOSQ_ERR_INVAL;
						}

						எழுத்து *topic_in_quotes = strtok_r(வெற்று, "\"", &saveptr);
						size_t tlen = 1;
						எனில்(topic_in_quotes){
							tlen = strlen(topic_in_quotes);
						}

						topic = mosquitto_malloc(strlen(token) + tlen + 1);
						எனில்(!topic){
							log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
							திரும்பு MOSQ_ERR_NOMEM;
						}

						strcpy(topic, token + 1);
						strcat(topic, " ");
						எனில்(topic_in_quotes){
							strcat(topic, topic_in_quotes);
						}
					}இல்லையெனில்{
						topic = mosquitto_strdup(token);
						எனில்(!topic){
							log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
							திரும்பு MOSQ_ERR_NOMEM;
						}
					}

					token = strtok_r(வெற்று, " ", &saveptr);
					எனில்(token){
						எனில்(!strcasecmp(token, "out")){
							direction = bd_out;
						}இல்லையெனில் எனில்(!strcasecmp(token, "in")){
							direction = bd_in;
						}இல்லையெனில் எனில்(!strcasecmp(token, "both")){
							direction = bd_both;
						}இல்லையெனில்{
							log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid bridge topic direction '%s'.", token);
							mosquitto_FREE(topic);
							திரும்பு MOSQ_ERR_INVAL;
						}
						token = strtok_r(வெற்று, " ", &saveptr);
						எனில்(token){
							எனில்(token[0] == '#'){
								(வெறுமை)strtok_r(வெற்று, "", &saveptr);
							}
							qos = (uint8_t)atoi(token);
							எனில்(qos > 2){
								log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid bridge QoS level '%s'.", token);
								mosquitto_FREE(topic);
								திரும்பு MOSQ_ERR_INVAL;
							}

							token = strtok_r(வெற்று, " ", &saveptr);
							எனில்(token){
								எனில்(!strcmp(token, "\"\"") || token[0] == '#'){
									local_prefix = வெற்று;
									எனில்(token[0] == '#'){
										(வெறுமை)strtok_r(வெற்று, "", &saveptr);
									}
								}இல்லையெனில்{
									local_prefix = token;
								}

								token = strtok_r(வெற்று, " ", &saveptr);
								எனில்(token){
									எனில்(!strcmp(token, "\"\"") || token[0] == '#'){
										remote_prefix = வெற்று;
									}இல்லையெனில்{
										remote_prefix = token;
									}
								}
							}
						}
					}

					எனில்(bridge__add_topic(cur_bridge, topic, direction, qos, local_prefix, remote_prefix)){
						mosquitto_FREE(topic);
						திரும்பு MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(topic);
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "max_topic_alias")){
					REQUIRE_LISTENER(token);
					எனில்(conf__parse_int(&token, "max_topic_alias", &tmp_int, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias' value in configuration.");
						திரும்பு MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias = (uint16_t)tmp_int;
				}இல்லையெனில் எனில்(!strcmp(token, "max_topic_alias_broker")){
					REQUIRE_LISTENER(token);
					எனில்(conf__parse_int(&token, "max_topic_alias_broker", &tmp_int, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias_broker' value in configuration.");
						திரும்பு MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias_broker = (uint16_t)tmp_int;
				}இல்லையெனில் எனில்(!strcmp(token, "try_private")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_bool(&token, "try_private", &cur_bridge->try_private, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "upgrade_outgoing_qos")){
					எனில்(conf__parse_bool(&token, token, &config->upgrade_outgoing_qos, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "use_identity_as_username")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					எனில்(conf__parse_bool(&token, "use_identity_as_username", &cur_listener->use_identity_as_username, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "use_subject_as_username")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					எனில்(conf__parse_bool(&token, "use_subject_as_username", &cur_listener->use_subject_as_username, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "user")){
					எனில்(reload){
						தொடர்;        /* Drop privileges user not valid for reloading. */
					}
					mosquitto_FREE(config->user);
					எனில்(conf__parse_string(&token, "user", &config->user, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "use_username_as_clientid")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					எனில்(conf__parse_bool(&token, "use_username_as_clientid", &cur_listener->use_username_as_clientid, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
				}இல்லையெனில் எனில்(!strcmp(token, "username") || !strcmp(token, "remote_username")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					எனில்(conf__parse_string(&token, "bridge remote_username", &cur_bridge->remote_username, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "websockets_log_level")){
#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					எனில்(conf__parse_int(&token, "websockets_log_level", &config->websockets_log_level, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
#endif
				}இல்லையெனில் எனில்(!strcmp(token, "websockets_headers_size") || !strcmp(token, "packet_buffer_size")){
					எனில்(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					எனில்(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(வெற்று, MOSQ_LOG_WARNING, "Error: Packet buffer size must be between 0 and 65535 inclusive.");
						திரும்பு MOSQ_ERR_INVAL;
					}
					config->packet_buffer_size = (uint16_t)tmp_int;
				}இல்லையெனில் எனில்(!strcmp(token, "websockets_origin")){
#ifdef WITH_WEBSOCKETS
#  எனில் LWS_LIBRARY_VERSION_எண் >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN
					REQUIRE_LISTENER(token);
					ws_origins = mosquitto_realloc(cur_listener->ws_origins, அளவு(எழுத்து *)*(size_t)(cur_listener->ws_origin_மொத்தம்+1));
					எனில்(ws_origins == வெற்று){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
						திரும்பு MOSQ_ERR_NOMEM;
					}
					ws_origins[cur_listener->ws_origin_மொத்தம்] = வெற்று;
					cur_listener->ws_origins = ws_origins;
					எனில்(conf__parse_string(&token, "websockets_origin", &ws_origins[cur_listener->ws_origin_மொத்தம்], &saveptr)){
						திரும்பு MOSQ_ERR_INVAL;
					}
					cur_listener->ws_origin_மொத்தம்++;
#  இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: websockets_origin support not available, libwebsockets version is too old.");
#  endif
#இல்லையெனில்
					log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Websockets support not available.");
#endif
				}இல்லையெனில்{
					log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unknown configuration variable '%s'.", token);
					திரும்பு MOSQ_ERR_INVAL;
				}
			}
		}
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு config__read_file(கட்டமைப்பு mosquitto__config *config, bool reload, மாறிலி எழுத்து *file, கட்டமைப்பு config_recurse *cr, முழு level, முழு *lineno)
{
	முழு rc;
	FILE *fptr = வெற்று;
	எழுத்து *buf;
	முழு buflen;
#ifndef WIN32
	DIR *dir;
#endif

#ifndef WIN32
	dir = opendir(file);
	எனில்(dir){
		closedir(dir);
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Config file '%s' is a directory.", file);
		திரும்பு 1;
	}
#endif

	fptr = mosquitto_fopen(file, "rt", false);
	எனில்(!fptr){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to open config file '%s'.", file);
		திரும்பு 1;
	}

	buflen = 1000;
	buf = mosquitto_malloc((size_t)buflen);
	எனில்(!buf){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
		fclose(fptr);
		திரும்பு MOSQ_ERR_NOMEM;
	}

	rc = config__read_file_core(config, reload, cr, level, lineno, fptr, &buf, &buflen);
	mosquitto_FREE(buf);
	fclose(fptr);

	திரும்பு rc;
}


நிலையான முழு config__check_proxy(கட்டமைப்பு mosquitto__config *config)
{
	க்கு(முழு i=0; i<config->listener_மொத்தம்; i++){
		கட்டமைப்பு mosquitto__listener *l = &config->listeners[i];

		எனில்(l->enable_proxy_protocol == 2){
#ifdef WITH_TLS
			எனில்(l->use_subject_as_username){
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error: use_subject_as_username cannot be used with `enable_proxy_protocol 2`.");
				திரும்பு MOSQ_ERR_INVAL;
			}

			எனில்(l->certfile || l->keyfile){
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error: certfile and keyfile cannot be used with `enable_proxy_protocol 2`.");
				திரும்பு MOSQ_ERR_INVAL;
			}
#endif
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு config__check(கட்டமைப்பு mosquitto__config *config)
{
	/* Checks that are easy to make after the config has been loaded. */

	மாறிலி எழுத்து *id_prefix;
	முழு id_prefix_len;
	எனில்(config->security_options.auto_id_prefix){
		id_prefix = config->security_options.auto_id_prefix;
		id_prefix_len = config->security_options.auto_id_prefix_len;
	}இல்லையெனில்{
		id_prefix = "auto-";
		id_prefix_len = (முழு)strlen("auto-");
	}

	/* Default to auto_id_prefix = 'auto-' if none set. */
	க்கு(முழு i=0; i<config->listener_மொத்தம்; i++){
		எனில்(!config->listeners[i].security_options->auto_id_prefix){
			config->listeners[i].security_options->auto_id_prefix = mosquitto_strdup(id_prefix);
			எனில்(!config->listeners[i].security_options->auto_id_prefix){
				திரும்பு MOSQ_ERR_NOMEM;
			}
			config->listeners[i].security_options->auto_id_prefix_len = (uint16_t)id_prefix_len;
		}
	}

	திரும்பு config__check_proxy(config);
}

#ifdef WITH_BRIDGE


நிலையான முழு config__check_bridges(கட்டமைப்பு mosquitto__config *config)
{
	கட்டமைப்பு mosquitto__bridge *bridge1, *bridge2;
	எழுத்து hostname[256];
	size_t len;

	/* Check for bridge duplicate local_clientid, need to generate missing IDs
	 * first. */
	க்கு(முழு i=0; i<config->bridge_மொத்தம்; i++){
		bridge1 = config->bridges[i];

		எனில்(!bridge1->remote_clientid){
			எனில்(!gethostname(hostname, 256)){
				len = strlen(hostname) + strlen(bridge1->name) + 2;
				bridge1->remote_clientid = mosquitto_malloc(len);
				எனில்(!bridge1->remote_clientid){
					திரும்பு MOSQ_ERR_NOMEM;
				}
				snprintf(bridge1->remote_clientid, len, "%s.%s", hostname, bridge1->name);
			}இல்லையெனில்{
				திரும்பு 1;
			}
		}

		எனில்(!bridge1->local_clientid){
			len = strlen(bridge1->remote_clientid) + strlen("local.") + 2;
			bridge1->local_clientid = mosquitto_malloc(len);
			எனில்(!bridge1->local_clientid){
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
				திரும்பு MOSQ_ERR_NOMEM;
			}
			snprintf(bridge1->local_clientid, len, "local.%s", bridge1->remote_clientid);
		}
	}

	க்கு(முழு i=0; i<config->bridge_மொத்தம்; i++){
		bridge1 = config->bridges[i];
		க்கு(முழு j=i+1; j<config->bridge_மொத்தம்; j++){
			bridge2 = config->bridges[j];
			எனில்(!strcmp(bridge1->local_clientid, bridge2->local_clientid)){
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Bridge local_clientid "
						"'%s' is not unique. Try changing or setting the "
						"local_clientid value for one of the bridges.",
						bridge1->local_clientid);
				திரும்பு MOSQ_ERR_INVAL;
			}
		}
	}

#ifdef WITH_TLS
	/* Check for missing TLS cafile/capath/certfile/keyfile */
	க்கு(முழு i=0; i<config->listener_மொத்தம்; i++){
		bool cafile = !!config->listeners[i].cafile;
		bool capath = !!config->listeners[i].capath;
		bool certfile = !!config->listeners[i].certfile;
		bool keyfile = !!config->listeners[i].keyfile;

		எனில்((certfile && !keyfile) || (!certfile && keyfile)){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Both certfile and keyfile must be provided to enable a TLS listener.");
			திரும்பு MOSQ_ERR_INVAL;
		}
		எனில்(cafile && !certfile){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: cafile specified without certfile and keyfile.");
			திரும்பு MOSQ_ERR_INVAL;
		}
		எனில்(capath && !certfile){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: capath specified without certfile and keyfile.");
			திரும்பு MOSQ_ERR_INVAL;
		}
	}
#endif
	திரும்பு MOSQ_ERR_SUCCESS;
}
#endif


நிலையான முழு conf__parse_bool(எழுத்து **token, மாறிலி எழுத்து *name, bool *மதிப்பு, எழுத்து **saveptr)
{
	*token = strtok_r(வெற்று, " ", saveptr);
	எனில்(*token){
		எனில்(!strcmp(*token, "false") || !strcmp(*token, "0")){
			*மதிப்பு = false;
		}இல்லையெனில் எனில்(!strcmp(*token, "true") || !strcmp(*token, "1")){
			*மதிப்பு = true;
		}இல்லையெனில்{
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Invalid '%s' value (%s).", name, *token);
			திரும்பு MOSQ_ERR_INVAL;
		}
	}இல்லையெனில்{
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		திரும்பு MOSQ_ERR_INVAL;
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு conf__parse_int(எழுத்து **token, மாறிலி எழுத்து *name, முழு *மதிப்பு, எழுத்து **saveptr)
{
	*token = strtok_r(வெற்று, " ", saveptr);
	எனில்(*token){
		எழுத்து *endptr = வெற்று;
		long l = strtol(*token, &endptr, 0);
		எனில்(endptr == *token || endptr[0] != '\0'){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: '%s' value not a number.", name);
			திரும்பு MOSQ_ERR_INVAL;
		}
		எனில்(l > INT_MAX || l < INT_MIN){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: '%s' value out of range.", name);
			திரும்பு MOSQ_ERR_INVAL;
		}
		*மதிப்பு = (முழு)l;
	}இல்லையெனில்{
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		திரும்பு MOSQ_ERR_INVAL;
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு conf__parse_ssize_t(எழுத்து **token, மாறிலி எழுத்து *name, ssize_t *மதிப்பு, எழுத்து **saveptr)
{
	*token = strtok_r(வெற்று, " ", saveptr);
	எனில்(*token){
		*மதிப்பு = atol(*token);
	}இல்லையெனில்{
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		திரும்பு MOSQ_ERR_INVAL;
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு conf__parse_string(எழுத்து **token, மாறிலி எழுத்து *name, எழுத்து **மதிப்பு, எழுத்து **saveptr)
{
	size_t tlen;

	*token = strtok_r(வெற்று, "", saveptr);
	எனில்(*token){
		எனில்(*மதிப்பு){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Duplicate '%s' value in configuration.", name);
			திரும்பு MOSQ_ERR_INVAL;
		}
		/* Deal with multiple spaces at the beginning of the string. */
		*token = mosquitto_trimblanks(*token);
		எனில்(strlen(*token) == 0){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
			திரும்பு MOSQ_ERR_INVAL;
		}

		tlen = strlen(*token);
		எனில்(tlen > UINT16_MAX){
			திரும்பு MOSQ_ERR_INVAL;
		}
		எனில்(mosquitto_validate_utf8(*token, (uint16_t)tlen)){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Malformed UTF-8 in configuration.");
			திரும்பு MOSQ_ERR_INVAL;
		}
		*மதிப்பு = mosquitto_strdup(*token);
		எனில்(!*மதிப்பு){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
			திரும்பு MOSQ_ERR_NOMEM;
		}
	}இல்லையெனில்{
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		திரும்பு MOSQ_ERR_INVAL;
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}
