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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ <limits.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <errno.h>

#ifdef WIN32
#નહીંતર
#  include <dirent.h>
#  include <strings.h>
#endif

#ifndef WIN32
#  include <netdb.h>
#  include <sys/socket.h>
#નહીંતર
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#જો !defined(WIN32) && !defined(__CYGWIN__)
#  include <syslog.h>
#endif

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "tls_mosq.h"
#અન્તર્ભાવ "util_mosq.h"
#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"

#અન્તર્ભાવ "utlist.h"

સંરચના config_recurse {
	unsigned પૂર્ણાંક log_dest;
	પૂર્ણાંક log_dest_set;
	unsigned પૂર્ણાંક log_type;
	પૂર્ણાંક log_type_set;
};

#જો defined(WIN32) || defined(__CYGWIN__)
#અન્તર્ભાવ <windows.h>
બાહ્ય SERVICE_STATUS_HANDLE service_handle;
#endif


#define REQUIRE_LISTENER(A) \
		કરો{ \
			જો(cur_listener == ખાલી){ \
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				પાછા MOSQ_ERR_INVAL; \
			} \
		}જ્યારે(0)

#define REQUIRE_LISTENER_OR_DEFAULT_LISTENER(A) \
		કરો{ \
			જો(cur_listener == ખાલી){ \
				જો(config__create_default_listener(config, (A))){ \
					પાછા MOSQ_ERR_NOMEM; \
				} \
				cur_listener = config->default_listener; \
			} \
		}જ્યારે(0)

#define REQUIRE_LISTENER_IF_PER_LISTENER(A) \
		કરો{ \
			જો(config->per_listener_settings == true && cur_listener == ખાલી){ \
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				પાછા MOSQ_ERR_INVAL; \
			} \
		}જ્યારે(0)

#define REQUIRE_NON_DEFAULT_LISTENER(A) \
		કરો{ \
			જો(cur_listener == config->default_listener || cur_listener == ખાલી){ \
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				પાછા MOSQ_ERR_INVAL; \
			} \
		}જ્યારે(0)

#define REQUIRE_BRIDGE(A) \
		કરો{ \
			જો(cur_bridge == ખાલી){ \
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error: The '%s' option requires a bridge to be defined first.", (A)); \
				પાછા MOSQ_ERR_INVAL; \
			} \
		}જ્યારે(0)

#define REQUIRE_PLUGIN(A) \
		કરો{ \
			જો(cur_plugin == ખાલી){ \
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error: The '%s' option requires plugin/global_plugin/plugin_load to be defined first.", (A)); \
				પાછા MOSQ_ERR_INVAL; \
			} \
		}જ્યારે(0)

#define OPTION_DEPRECATED(A, B) \
		log__printf(ખાલી, MOSQ_LOG_NOTICE, "The '%s' option is now deprecated and will be removed in version 3.0. %s", (A), (B))

#define OPTION_UNAVAILABLE(A) \
		log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: The '%s' option is no longer available.", (A));

#define REQUIRE_NON_EMPTY_OPTION(A, B) \
		કરો{ \
			જો(!(A)){ \
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", (B)); \
				પાછા MOSQ_ERR_INVAL; \
			} \
		}જ્યારે(0)

#define PER_LISTENER_ALTERNATIVE(A, B) \
		જો(config->per_listener_settings){ \
			log__printf(ખાલી, MOSQ_LOG_NOTICE, "You are using the '%s' option with 'per_listener_settings true'. Please replace this with '%s'.", A, B); \
		} \


#ifdef FINAL_WITH_TLS_PSK
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A) \
		કરો{ \
			જો(cur_bridge->tls_psk_identity || cur_bridge->tls_psk){ \
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and psk encryption in a single bridge.", (A)); \
				પાછા MOSQ_ERR_INVAL; \
			} \
		}જ્યારે(0)

#  define REQUIRE_BRIDGE_NO_X509(A) \
		કરો{ \
			જો(cur_bridge->tls_cafile || cur_bridge->tls_capath || cur_bridge->tls_certfile || cur_bridge->tls_keyfile){ \
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and identity encryption in a single bridge.", (A)); \
				પાછા MOSQ_ERR_INVAL; \
			} \
		}જ્યારે(0)

#નહીંતર
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A)
#  define REQUIRE_BRIDGE_NO_X509(A)
#endif

સ્થિર સંરચના mosquitto__security_options *cur_security_options = ખાલી;

સ્થિર પૂર્ણાંક conf__parse_bool(અક્ષર **token, અચલ અક્ષર *name, bool *કિંમત, અક્ષર **saveptr);
સ્થિર પૂર્ણાંક conf__parse_int(અક્ષર **token, અચલ અક્ષર *name, પૂર્ણાંક *કિંમત, અક્ષર **saveptr);
સ્થિર પૂર્ણાંક conf__parse_ssize_t(અક્ષર **token, અચલ અક્ષર *name, ssize_t *કિંમત, અક્ષર **saveptr);
સ્થિર પૂર્ણાંક conf__parse_string(અક્ષર **token, અચલ અક્ષર *name, અક્ષર **કિંમત, અક્ષર **saveptr);
સ્થિર પૂર્ણાંક config__read_file(સંરચના mosquitto__config *config, bool reload, અચલ અક્ષર *file, સંરચના config_recurse *config_tmp, પૂર્ણાંક level, પૂર્ણાંક *lineno);
સ્થિર પૂર્ણાંક config__check(સંરચના mosquitto__config *config);
સ્થિર શૂન્ય config__cleanup_plugins(શૂન્ય);
#ifdef WITH_BRIDGE
સ્થિર પૂર્ણાંક config__check_bridges(સંરચના mosquitto__config *config);
#endif


સ્થિર પૂર્ણાંક config__add_listener(સંરચના mosquitto__config *config)
{
	સંરચના mosquitto__listener *listener;
	સંરચના mosquitto__listener *new_listeners;
	પૂર્ણાંક def_listener = -1;

	જો(config->default_listener){
		માટે(પૂર્ણાંક i=0; i<config->listener_ગણતરી; i++){
			જો(&config->listeners[i] == config->default_listener){
				def_listener = i;
				તોડો;
			}
		}
	}
	new_listeners = mosquitto_realloc(config->listeners, કદ(સંરચના mosquitto__listener)*(size_t)(config->listener_ગણતરી+1));
	જો(!new_listeners){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
		પાછા MOSQ_ERR_NOMEM;
	}
	config->listeners = new_listeners;
	listener = &config->listeners[config->listener_ગણતરી];
	memset(listener, 0, કદ(સંરચના mosquitto__listener));
	listener->security_options = mosquitto_calloc(1, કદ(સંરચના mosquitto__security_options));
	જો(!listener->security_options){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
		પાછા MOSQ_ERR_NOMEM;
	}
	જો(def_listener != -1){
		config->default_listener = &config->listeners[def_listener];
	}
	config->listener_ગણતરી++;

	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક config__create_default_listener(સંરચના mosquitto__config *config, અચલ અક્ષર *option_name)
{
	જો(config->default_listener){
		પાછા MOSQ_ERR_SUCCESS;
	}
	log__printf(ખાલી, MOSQ_LOG_INFO, "Creating default listener due to '%s' option.", option_name);
	log__printf(ખાલી, MOSQ_LOG_INFO, "It is best practice to define a 'listener' first. Using the '%s' option without a listener will be disabled in the future.", option_name);
	જો(config__add_listener(config)){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
		પાછા MOSQ_ERR_NOMEM;
	}

	config->default_listener = &config->listeners[config->listener_ગણતરી-1];
	listener__set_defaults(config->default_listener);
	config->default_listener->port = 1883;

	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર શૂન્ય conf__set_cur_security_options(સંરચના mosquitto__config *config, સંરચના mosquitto__listener **cur_listener, સંરચના mosquitto__security_options **security_options, અચલ અક્ષર *option_name)
{
	જો(config->per_listener_settings){
		જો(*cur_listener == ખાલી){
			જો(config__create_default_listener(config, option_name)){
				પાછા;
			}
			*cur_listener = config->default_listener;
		}
		(*security_options) = (*cur_listener)->security_options;
	}નહીંતર{
		(*security_options) = &config->security_options;
	}
}


સ્થિર પૂર્ણાંક conf__attempt_resolve(અચલ અક્ષર *host, અચલ અક્ષર *text, unsigned પૂર્ણાંક log, અચલ અક્ષર *સંદેશ)
{
	સંરચના addrinfo gai_hints;
	સંરચના addrinfo *gai_res;
	પૂર્ણાંક rc;

	memset(&gai_hints, 0, કદ(સંરચના addrinfo));
	gai_hints.ai_family = AF_UNSPEC;
	gai_hints.ai_socktype = SOCK_STREAM;
	gai_res = ખાલી;
	rc = getaddrinfo(host, ખાલી, &gai_hints, &gai_res);
	જો(gai_res){
		freeaddrinfo(gai_res);
	}
	જો(rc != 0){
#ifndef WIN32
		જો(rc == EAI_SYSTEM){
			જો(errno == ENOENT){
				log__printf(ખાલી, log, "%s: Unable to resolve %s %s.", સંદેશ, text, host);
			}નહીંતર{
				log__printf(ખાલી, log, "%s: Error resolving %s: %s.", સંદેશ, text, strerror(errno));
			}
		}નહીંતર{
			log__printf(ખાલી, log, "%s: Error resolving %s: %s.", સંદેશ, text, gai_strerror(rc));
		}
#નહીંતર
		જો(rc == WSAHOST_NOT_FOUND){
			log__printf(ખાલી, log, "%s: Error resolving %s.", સંદેશ, text);
		}
#endif
		પાછા MOSQ_ERR_INVAL;
	}
	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર શૂન્ય config__init_reload(સંરચના mosquitto__config *config)
{
	/* Set defaults */
	માટે(પૂર્ણાંક i=0; i<config->listener_ગણતરી; i++){
		listener__set_defaults(&config->listeners[i]);
	}

	config->local_only = true;
	config->allow_duplicate_messages = true;

	mosquitto_FREE(config->security_options.acl_data.acl_file);
	mosquitto_FREE(config->security_options.password_data.password_file);
	mosquitto_FREE(config->security_options.psk_file);

	config->security_options.allow_anonymous = -1;
	config->security_options.allow_zero_length_clientid = true;
	config->security_options.auto_id_prefix = ખાલી;
	config->security_options.auto_id_prefix_len = 0;

	config->autosave_interval = 1800;
	config->autosave_on_changes = false;

	mosquitto_FREE(config->clientid_prefixes);

	config->connection_messages = true;
	config->clientid_prefixes = ખાલી;
	config->per_listener_settings = false;
	જો(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = ખાલી;
	}
	mosquitto_FREE(config->log_file);

#જો defined(WIN32) || defined(__CYGWIN__)
	જો(service_handle){
		/* This is running as a Windows service. Default to no logging. Using
		 * stdout/stderr is forbidden because the first clients to connect will
		 * get log information sent to them for some reason. */
		config->log_dest = MQTT3_LOG_NONE;
	}નહીંતર{
		config->log_dest = MQTT3_LOG_STDERR;
	}
#નહીંતર
	config->log_facility = LOG_DAEMON;
	config->log_dest = MQTT3_LOG_STDERR | MQTT3_LOG_DLT;
	જો(db.quiet){
		config->log_type = 0;
	}નહીંતર જો(db.verbose){
		config->log_type = UINT_MAX;
	}નહીંતર{
		config->log_type = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;
	}
#endif
	config->log_timestamp = true;
	mosquitto_FREE(config->log_timestamp_format);
	config->global_max_clients = -1;
	config->global_max_connections = -1;
	config->log_timestamp_format = ખાલી;
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


સ્થિર શૂન્ય config__cleanup_plugin_config(mosquitto_plugin_id_t *plugin)
{
	mosquitto_FREE(plugin->config.path);
	mosquitto_FREE(plugin->config.name);

	જો(plugin->config.options){
		માટે(પૂર્ણાંક j=0; j<plugin->config.option_ગણતરી; j++){
			mosquitto_FREE(plugin->config.options[j].ચાવી);
			mosquitto_FREE(plugin->config.options[j].કિંમત);
		}
		mosquitto_FREE(plugin->config.options);
		plugin->config.option_ગણતરી = 0;
	}
	mosquitto_FREE(plugin->config.security_options);
	mosquitto_FREE(plugin);
}


સ્થિર શૂન્ય config__cleanup_plugins(શૂન્ય)
{
	માટે(પૂર્ણાંક i=0; i<db.plugin_ગણતરી; i++){
		config__cleanup_plugin_config(db.plugins[i]);
	}
	mosquitto_FREE(db.plugins);
}


શૂન્ય config__init(સંરચના mosquitto__config *config)
{
	memset(config, 0, કદ(સંરચના mosquitto__config));
	config__init_reload(config);

	config->daemon = false;
}


શૂન્ય config__cleanup(સંરચના mosquitto__config *config)
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
	જો(config->listeners){
		માટે(પૂર્ણાંક i=0; i<config->listener_ગણતરી; i++){
			mosquitto_FREE(config->listeners[i].host);
			mosquitto_FREE(config->listeners[i].bind_interface);
			mosquitto_FREE(config->listeners[i].mount_point);
			mosquitto_FREE(config->listeners[i].socks);
			જો(config->listeners[i].security_options){
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
#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			જો(!config->listeners[i].ws_context) /* libwebsockets frees its own SSL_CTX */
#endif
			{
				SSL_CTX_free(config->listeners[i].ssl_ctx);
				config->listeners[i].ssl_ctx = ખાલી;
			}
#endif
#જો defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
			mosquitto_FREE(config->listeners[i].http_dir);
#endif
#ifdef WITH_WEBSOCKETS
			માટે(પૂર્ણાંક j=0; j<config->listeners[i].ws_origin_ગણતરી; j++){
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
	જો(config->bridges){
		માટે(પૂર્ણાંક i=0; i<config->bridge_ગણતરી; i++){
			config__bridge_cleanup(config->bridges[i]);
		}
		mosquitto_FREE(config->bridges);
	}
#endif
	config__cleanup_plugins();

	જો(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = ખાલી;
	}
	જો(config->log_file){
		mosquitto_FREE(config->log_file);
		config->log_file = ખાલી;
	}
}

#ifdef WITH_BRIDGE


શૂન્ય config__bridge_cleanup(સંરચના mosquitto__bridge *bridge)
{
	જો(bridge == ખાલી){
		પાછા;
	}

	mosquitto_FREE(bridge->name);
	જો(bridge->addresses){
		માટે(પૂર્ણાંક i=0; i<bridge->address_ગણતરી; i++){
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
	જો(bridge->topics){
		સંરચના mosquitto__bridge_topic *cur_topic, *topic_tmp;

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


સ્થિર શૂન્ય print_version(શૂન્ય)
{
	printf("mosquitto %s\n", VERSION);
	printf("Copyright © 2025 Roger Light.\n");
	printf("License EPL-2.0 OR BSD-3-Clause.\n");
}


સ્થિર શૂન્ય print_usage(શૂન્ય)
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


પૂર્ણાંક config__parse_args(સંરચના mosquitto__config *config, પૂર્ણાંક argc, અક્ષર *argv[])
{
	પૂર્ણાંક i;
	પૂર્ણાંક port_tmp;

	માટે(i=1; i<argc; i++){
		જો(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--config-file")){
			જો(i<argc-1){
				db.config_file = argv[i+1];

				જો(config__read(config, false)){
					પાછા MOSQ_ERR_INVAL;
				}
			}નહીંતર{
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error: -c argument given, but no config file specified.");
				પાછા MOSQ_ERR_INVAL;
			}
			i++;
		}નહીંતર જો(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--daemon")){
			config->daemon = true;
		}નહીંતર જો(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
			print_usage();
			પાછા MOSQ_ERR_UNKNOWN;
		}નહીંતર જો(!strcmp(argv[i], "--version")){
			print_version();
			પાછા MOSQ_ERR_UNKNOWN;
		}નહીંતર જો(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port")){
			જો(i<argc-1){
				port_tmp = atoi(argv[i+1]);
				જો(port_tmp<1 || port_tmp>UINT16_MAX){
					log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid port specified (%d).", port_tmp);
					પાછા MOSQ_ERR_INVAL;
				}નહીંતર{
					જો(config->cmd_port_ગણતરી == CMD_PORT_LIMIT){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Only %d ports can be specified on the command line.", CMD_PORT_LIMIT);
						પાછા MOSQ_ERR_INVAL;
					}
					config->cmd_port[config->cmd_port_ગણતરી] = (uint16_t)port_tmp;
					config->cmd_port_ગણતરી++;
				}
			}નહીંતર{
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error: -p argument given, but no port specified.");
				પાછા MOSQ_ERR_INVAL;
			}
			i++;
		}નહીંતર જો(!strcmp(argv[i], "--tls-keylog")){
#ifdef WITH_TLS
			જો(i<argc-1){
				db.tls_keylog = mosquitto_strdup(argv[i+1]);
				જો(db.tls_keylog == ખાલી){
					log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
					પાછા MOSQ_ERR_NOMEM;
				}
			}નહીંતર{
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error: --tls-keylog argument given, but no file specified.");
				પાછા MOSQ_ERR_INVAL;
			}
			i++;
#નહીંતર
			fprintf(stderr, "Error: TLS support not available so --tls-keylog is not available.\n");
			પાછા MOSQ_ERR_INVAL;
#endif
		}નહીંતર જો(!strcmp(argv[i], "-q") || !strcmp(argv[i], "--quiet")){
			db.quiet = true;
		}નહીંતર જો(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")){
			db.verbose = true;
		}નહીંતર જો(!strcmp(argv[i], "--test-config")){
			config->test_configuration = true;
		}નહીંતર{
			fprintf(stderr, "Error: Unknown option '%s'.\n", argv[i]);
			print_usage();
			પાછા MOSQ_ERR_INVAL;
		}
	}

	/* Default to drop to mosquitto user if we are privileged and no user specified. */
	જો(!config->user){
		config->user = mosquitto_strdup("mosquitto");
		જો(config->user == ખાલી){
			પાછા MOSQ_ERR_NOMEM;
		}
	}
	જો(db.quiet){
		config->log_type = 0;
	}નહીંતર જો(db.verbose){
		config->log_type = UINT_MAX;
	}

	જો(getenv("MOSQUITTO_PERSISTENCE_LOCATION")){
		mosquitto_FREE(config->persistence_location);
		config->persistence_location = mosquitto_strdup(getenv("MOSQUITTO_PERSISTENCE_LOCATION"));
		જો(!config->persistence_location){
			પાછા MOSQ_ERR_NOMEM;
		}
	}
	પાછા config__check(config);
}


સ્થિર શૂન્ય config__copy(સંરચના mosquitto__config *src, સંરચના mosquitto__config *dest)
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
	dest->security_options.plugin_ગણતરી = src->security_options.plugin_ગણતરી;
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

#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	dest->websockets_log_level = src->websockets_log_level;
#endif

#ifdef WITH_BRIDGE
	માટે(પૂર્ણાંક i=0; i<dest->bridge_ગણતરી; i++){
		જો(dest->bridges[i]){
			config__bridge_cleanup(dest->bridges[i]);
		}
	}
	mosquitto_FREE(dest->bridges);
	dest->bridges = src->bridges;
	dest->bridge_ગણતરી = src->bridge_ગણતરી;
#endif
}


પૂર્ણાંક config__read(સંરચના mosquitto__config *config, bool reload)
{
	પૂર્ણાંક rc = MOSQ_ERR_SUCCESS;
	સંરચના config_recurse cr;
	પૂર્ણાંક lineno = 0;
#ifdef WITH_PERSISTENCE
	size_t len;
#endif
	સંરચના mosquitto__config config_reload;
	પૂર્ણાંક i;

	જો(reload){
		memset(&config_reload, 0, કદ(સંરચના mosquitto__config));
	}

	cr.log_dest = MQTT3_LOG_NONE;
	cr.log_dest_set = 0;
	cr.log_type = MOSQ_LOG_NONE;
	cr.log_type_set = 0;

	જો(!db.config_file){
		પાછા 0;
	}

	જો(reload){
		/* Re-initialise appropriate config vars to default for reload. */
		config__init_reload(&config_reload);
		config_reload.listeners = config->listeners;
		config_reload.listener_ગણતરી = config->listener_ગણતરી;
		cur_security_options = ખાલી;
		rc = config__read_file(&config_reload, reload, db.config_file, &cr, 0, &lineno);
	}નહીંતર{
		rc = config__read_file(config, reload, db.config_file, &cr, 0, &lineno);
	}
	જો(rc){
		જો(lineno > 0){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error found at %s:%d.", db.config_file, lineno);
		}
		પાછા rc;
	}

	જો(reload){
		config__copy(&config_reload, config);
	}

	/* If auth/access options are set and allow_anonymous not explicitly set, disallow anon. */
	જો(config->local_only == false){
		જો(config->per_listener_settings){
			માટે(i=0; i<config->listener_ગણતરી; i++){
				/* Default option if no security options set */
				જો(config->listeners[i].security_options->allow_anonymous == -1){
					config->listeners[i].security_options->allow_anonymous = false;
				}
			}
		}નહીંતર{
			જો(config->security_options.allow_anonymous == -1){
				config->security_options.allow_anonymous = false;
			}
		}
	}
#ifdef WITH_PERSISTENCE
	જો(config->persistence){
		જો(!config->persistence_file){
			config->persistence_file = mosquitto_strdup("mosquitto.db");
			જો(!config->persistence_file){
				પાછા MOSQ_ERR_NOMEM;
			}
		}
		mosquitto_FREE(config->persistence_filepath);
		જો(config->persistence_location && strlen(config->persistence_location)){
			len = strlen(config->persistence_location) + strlen(config->persistence_file) + 2;
			config->persistence_filepath = mosquitto_malloc(len);
			જો(!config->persistence_filepath){
				પાછા MOSQ_ERR_NOMEM;
			}
#ifdef WIN32
			snprintf(config->persistence_filepath, len, "%s\\%s", config->persistence_location, config->persistence_file);
#નહીંતર
			snprintf(config->persistence_filepath, len, "%s/%s", config->persistence_location, config->persistence_file);
#endif
		}નહીંતર{
			config->persistence_filepath = mosquitto_strdup(config->persistence_file);
			જો(!config->persistence_filepath){
				પાછા MOSQ_ERR_NOMEM;
			}
		}
	}
#endif
	/* Default to drop to mosquitto user if no other user specified. This must
	 * remain here even though it is covered in config__parse_args() because this
	 * function may be called on its own. */
	જો(!config->user){
		config->user = mosquitto_strdup("mosquitto");
	}

#ifdef WITH_BRIDGE
	માટે(i=0; i<config->bridge_ગણતરી; i++){
		જો(!config->bridges[i]->name){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: bridge name not defined.");
			પાછા MOSQ_ERR_INVAL;
		}
		જો(config->bridges[i]->addresses  == 0){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no remote addresses defined.");
			પાછા MOSQ_ERR_INVAL;
		}
		જો(config->bridges[i]->topic_ગણતરી == 0){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no topics defined.");
			પાછા MOSQ_ERR_INVAL;
		}
#ifdef FINAL_WITH_TLS_PSK
		જો(config->bridges[i]->tls_psk && !config->bridges[i]->tls_psk_identity){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_identity.");
			પાછા MOSQ_ERR_INVAL;
		}
		જો(config->bridges[i]->tls_psk_identity && !config->bridges[i]->tls_psk){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_psk.");
			પાછા MOSQ_ERR_INVAL;
		}
#endif
	}
#endif

	જો(cr.log_dest_set){
		config->log_dest = cr.log_dest;
	}
	જો(db.quiet){
		config->log_type = 0;
	}નહીંતર જો(db.verbose){
		config->log_type = UINT_MAX;
	}નહીંતર જો(cr.log_type_set){
		config->log_type = cr.log_type;
	}
#ifdef WITH_BRIDGE
	પાછા config__check_bridges(config);
#નહીંતર
	પાછા MOSQ_ERR_SUCCESS;
#endif
}


સ્થિર mosquitto_plugin_id_t *config__plugin_find(અચલ અક્ષર *name)
{
	જો(db.plugins && name){
		માટે(પૂર્ણાંક i=0; i<db.plugin_ગણતરી; i++){
			જો(db.plugins[i]->config.name && !strcmp(db.plugins[i]->config.name, name)){
				પાછા db.plugins[i];
			}
		}
	}
	પાછા ખાલી;
}


સ્થિર mosquitto_plugin_id_t *config__plugin_load(અચલ અક્ષર *name, અચલ અક્ષર *path)
{
	mosquitto_plugin_id_t *plugin = ખાલી;
	mosquitto_plugin_id_t **plugins = ખાલી;

	જો(name && config__plugin_find(name)){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Duplicate plugin name '%s'.", name);
		પાછા ખાલી;
	}

	જો(!path || !strcmp(path, "")){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Missing plugin path for plugin name '%s'.", name);
		પાછા ખાલી;
	}

	plugin = mosquitto_calloc(1, કદ(mosquitto_plugin_id_t));
	જો(!plugin){
		જાઓ error;
	}

	જો(name){
		plugin->config.name = mosquitto_strdup(name);
	}
	plugin->config.path = mosquitto_strdup(path);
	જો((name && !plugin->config.name) || !plugin->config.path){
		જાઓ error;
	}
	plugin->config.options = ખાલી;
	plugin->config.option_ગણતરી = 0;
	plugin->config.deny_special_chars = true;

	/* Add to db list */
	plugins = mosquitto_realloc(db.plugins, (size_t)(db.plugin_ગણતરી+1)*કદ(સંરચના mosquitto__plugin_config *));
	જો(!plugins){
		જાઓ error;
	}

	plugins[db.plugin_ગણતરી] = plugin;
	db.plugins = plugins;
	db.plugin_ગણતરી++;

	પાછા plugin;
error:
	log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
	જો(plugin){
		mosquitto_FREE(plugin->config.name);
		mosquitto_FREE(plugin->config.path);
	}
	mosquitto_FREE(plugin);
	પાછા ખાલી;
}


પૂર્ણાંક config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, સંરચના mosquitto__security_options *security_options)
{
	સંરચના mosquitto__security_options **new_options;
	mosquitto_plugin_id_t **new_plugins;

	new_options = mosquitto_realloc(plugin->config.security_options, (size_t)(plugin->config.security_option_ગણતરી+1)*કદ(સંરચના mosquitto__security_options *));
	new_plugins = mosquitto_realloc(security_options->plugins, (size_t)(security_options->plugin_ગણતરી+1)*કદ(mosquitto_plugin_id_t *));

	જો(!new_options || !new_plugins){
		mosquitto_FREE(new_options);
		mosquitto_FREE(new_plugins);
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
		પાછા MOSQ_ERR_NOMEM;
	}

	new_options[plugin->config.security_option_ગણતરી] = security_options;
	plugin->config.security_options = new_options;
	plugin->config.security_option_ગણતરી++;

	new_plugins[security_options->plugin_ગણતરી] = plugin;
	security_options->plugins = new_plugins;
	security_options->plugin_ગણતરી++;

	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક config__read_file_core(સંરચના mosquitto__config *config, bool reload, સંરચના config_recurse *cr, પૂર્ણાંક level, પૂર્ણાંક *lineno, FILE *fptr, અક્ષર **buf, પૂર્ણાંક *buflen)
{
	પૂર્ણાંક rc;
	અક્ષર *token;
	પૂર્ણાંક tmp_int;
	અક્ષર *saveptr = ખાલી;
#ifdef WITH_BRIDGE
	અક્ષર *tmp_char;
	સંરચના mosquitto__bridge *cur_bridge = ખાલી;
#endif
	mosquitto_plugin_id_t *cur_plugin = ખાલી;

	અક્ષર *ચાવી;
	સંરચના mosquitto__listener *cur_listener = ખાલી;
	પૂર્ણાંક i;
	પૂર્ણાંક lineno_ext = 0;
	size_t prefix_len;
	size_t slen;
#ifdef WITH_WEBSOCKETS
	અક્ષર **ws_origins = ખાલી;
#endif

	*lineno = 0;

	જ્યારે(mosquitto_fgets(buf, buflen, fptr)){
		(*lineno)++;
		જો((*buf)[0] != '#' && (*buf)[0] != 10 && (*buf)[0] != 13){
			slen = strlen(*buf);
			જો(slen == 0){
				ચાલુ;
			}
			જ્યારે((*buf)[slen-1] == 10 || (*buf)[slen-1] == 13){
				(*buf)[slen-1] = 0;
				slen = strlen(*buf);
				જો(slen == 0){
					ચાલુ;
				}
			}
			token = strtok_r((*buf), " ", &saveptr);
			જો(token){
				જો(!strcmp(token, "accept_protocol_versions")){
					REQUIRE_NON_DEFAULT_LISTENER(token);
					cur_listener->disable_protocol_v3 = true;
					cur_listener->disable_protocol_v4 = true;
					cur_listener->disable_protocol_v5 = true;
					જો(saveptr == ખાલી){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", "accept_protocol_versions");
						પાછા MOSQ_ERR_INVAL;
					}
					token = strtok_r(saveptr, ", \t", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "accept_protocol_versions");

					જ્યારે(token){
						જો(!strcmp(token, "3")){
							cur_listener->disable_protocol_v3 = false;
						}નહીંતર જો(!strcmp(token, "4")){
							cur_listener->disable_protocol_v4 = false;
						}નહીંતર જો(!strcmp(token, "5")){
							cur_listener->disable_protocol_v5 = false;
						}

						token = strtok_r(ખાલી, ", \t", &saveptr);
					}
				}નહીંતર જો(!strcmp(token, "acl_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);

					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->acl_data.acl_file);
					જો(conf__parse_string(&token, "acl_file", &cur_security_options->acl_data.acl_file, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "address") || !strcmp(token, "addresses")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					જો(cur_bridge->addresses){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid bridge configuration, 'address' only allowed once.");
						પાછા MOSQ_ERR_INVAL;
					}
					જ્યારે((token = strtok_r(ખાલી, " ", &saveptr))){
						જો(token[0] == '#'){
							તોડો;
						}
						સંરચના bridge_address *new_addresses = mosquitto_realloc(cur_bridge->addresses, કદ(સંરચના bridge_address)*(size_t)(cur_bridge->address_ગણતરી+1));
						જો(!new_addresses){
							log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
							પાછા MOSQ_ERR_NOMEM;
						}
						cur_bridge->address_ગણતરી++;
						cur_bridge->addresses = new_addresses;
						memset(&cur_bridge->addresses[cur_bridge->address_ગણતરી-1], 0, કદ(સંરચના bridge_address));
						cur_bridge->addresses[cur_bridge->address_ગણતરી-1].address = mosquitto_strdup(token);
						જો(!cur_bridge->addresses[cur_bridge->address_ગણતરી-1].address){
							log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
							પાછા MOSQ_ERR_NOMEM;
						}
					}
					માટે(i=0; i<cur_bridge->address_ગણતરી; i++){
						/* cur_bridge->addresses[i].address is now
						 * "address[:port]". If address is an IPv6 address,
						 * then port is required. We must check for the :
						 * backwards. */
						tmp_char = strrchr(cur_bridge->addresses[i].address, ':');
						જો(tmp_char){
							/* Remove ':', so cur_bridge->addresses[i].address
							 * now just looks like the address. */
							tmp_char[0] = '\0';

							/* The remainder of the string */
							tmp_int = atoi(&tmp_char[1]);
							જો(tmp_int < 1 || tmp_int > UINT16_MAX){
								log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid bridge port value (%d).", tmp_int);
								પાછા MOSQ_ERR_INVAL;
							}
							cur_bridge->addresses[i].port = (uint16_t)tmp_int;
						}નહીંતર{
							cur_bridge->addresses[i].port = 1883;
						}
						conf__attempt_resolve(cur_bridge->addresses[i].address, "bridge address", MOSQ_LOG_WARNING, "Warning");
					}
					જો(cur_bridge->address_ગણતરી == 0){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Empty address value in configuration.");
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "allow_anonymous")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					PER_LISTENER_ALTERNATIVE(token, "listener_allow_anonymous");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					જો(conf__parse_bool(&token, "allow_anonymous", (bool *)&cur_security_options->allow_anonymous, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "allow_duplicate_messages")){
					OPTION_DEPRECATED(token, "The behaviour will default to true.");
					જો(conf__parse_bool(&token, "allow_duplicate_messages", &config->allow_duplicate_messages, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "allow_zero_length_clientid")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					જો(conf__parse_bool(&token, "allow_zero_length_clientid", &cur_security_options->allow_zero_length_clientid, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strncmp(token, "auth_opt_", strlen("auth_opt_")) || !strncmp(token, "plugin_opt_", strlen("plugin_opt_"))){
					જો(reload){
						ચાલુ;        /* Auth plugin not currently valid for reloading. */

					}
					REQUIRE_PLUGIN(token);

					જો(!strncmp(token, "auth_opt_", strlen("auth_opt_"))){
						prefix_len = strlen("auth_opt_");
					}નહીંતર{
						prefix_len = strlen("plugin_opt_");
					}
					જો(strlen(token) < prefix_len + 3){
						/* auth_opt_ == 9, + one digit key == 10, + one space == 11, + one value == 12 */
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid 'plugin_opt_' config option.");
						પાછા MOSQ_ERR_INVAL;
					}
					ચાવી = mosquitto_strdup(&token[prefix_len]);
					જો(!ચાવી){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
						પાછા MOSQ_ERR_NOMEM;
					}નહીંતર જો(STREMPTY(ચાવી)){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Empty 'plugin_opt_' config option.");
						mosquitto_FREE(ચાવી);
						પાછા MOSQ_ERR_INVAL;
					}
					token = saveptr;
					જો(token && token[0]){
						જ્યારે(token[0] == ' ' || token[0] == '\t'){
							token++;
						}
						સંરચના mosquitto_opt *new_options = mosquitto_realloc(cur_plugin->config.options, (size_t)(cur_plugin->config.option_ગણતરી+1)*કદ(સંરચના mosquitto_opt));
						જો(!new_options){
							log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
							mosquitto_FREE(ચાવી);
							પાછા MOSQ_ERR_NOMEM;
						}
						cur_plugin->config.option_ગણતરી++;
						cur_plugin->config.options = new_options;
						cur_plugin->config.options[cur_plugin->config.option_ગણતરી-1].ચાવી = ચાવી;
						cur_plugin->config.options[cur_plugin->config.option_ગણતરી-1].કિંમત = mosquitto_strdup(token);
						જો(!cur_plugin->config.options[cur_plugin->config.option_ગણતરી-1].કિંમત){
							log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
							પાછા MOSQ_ERR_NOMEM;
						}
					}નહીંતર{
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", ચાવી);
						mosquitto_FREE(ચાવી);
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "auth_plugin") || !strcmp(token, "plugin") || !strcmp(token, "global_plugin")){
					જો(reload){
						ચાલુ;        /* plugin not currently valid for reloading. */
					}
					જો(!strcmp(token, "global_plugin")){
						cur_security_options = &db.config->security_options;
					}નહીંતર{
						REQUIRE_LISTENER_IF_PER_LISTENER(token);
						conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					}

					cur_plugin = config__plugin_load(ખાલી, saveptr);
					જો(cur_plugin == ખાલી){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(config__plugin_add_secopt(cur_plugin, cur_security_options)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "auth_plugin_deny_special_chars")){
					જો(reload){
						ચાલુ;        /* Auth plugin not currently valid for reloading. */
					}
					જો(!cur_plugin){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: An auth_plugin_deny_special_chars option exists in the config file without a plugin.");
						પાછા MOSQ_ERR_INVAL;
					}
					જો(conf__parse_bool(&token, "auth_plugin_deny_special_chars", &cur_plugin->config.deny_special_chars, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "plugin_load")){
					અક્ષર *name = ખાલી;

					જો(reload){
						ચાલુ;        /* plugin not currently valid for reloading. */

					}
					name = strtok_r(ખાલી, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(name, "plugin_load");

					cur_plugin = config__plugin_load(name, saveptr);
					જો(cur_plugin == ખાલી){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "plugin_use")){
					અક્ષર *name = ખાલી;

					જો(reload){
						ચાલુ;        /* plugin not currently valid for reloading. */
					}
					REQUIRE_NON_DEFAULT_LISTENER(token);

					જો(conf__parse_string(&token, "plugin_use", &name, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					cur_plugin = config__plugin_find(name);
					જો(!cur_plugin){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Plugin '%s' not previously loaded.", name);
						mosquitto_FREE(name);
						પાછા MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(name);
					જો(config__plugin_add_secopt(cur_plugin, cur_listener->security_options)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "auto_id_prefix")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					OPTION_DEPRECATED(token, "Please use 'listener_auto_id_prefix' instead.");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					જો(conf__parse_string(&token, "auto_id_prefix", &cur_security_options->auto_id_prefix, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(cur_security_options->auto_id_prefix){
						જો(strlen(cur_security_options->auto_id_prefix) > 50){
							log__printf(ખાલી, MOSQ_LOG_ERR, "Error: auto_id_prefix length must be <= 50.");
							પાછા MOSQ_ERR_INVAL;
						}
						cur_security_options->auto_id_prefix_len = (uint16_t)strlen(cur_security_options->auto_id_prefix);
					}નહીંતર{
						cur_security_options->auto_id_prefix_len = 0;
					}
				}નહીંતર જો(!strcmp(token, "autosave_interval")){
					જો(conf__parse_int(&token, "autosave_interval", &config->autosave_interval, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(config->autosave_interval < 0){
						config->autosave_interval = 0;
					}
				}નહીંતર જો(!strcmp(token, "autosave_on_changes")){
					જો(conf__parse_bool(&token, "autosave_on_changes", &config->autosave_on_changes, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "bind_address")){
					OPTION_DEPRECATED(token, "");
					config->local_only = false;
					જો(reload){
						ચાલુ;        /* Rebinding listeners not valid during reloading. */

					}
					જો(config__create_default_listener(config, token)){
						પાછા MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					જો(conf__parse_string(&token, "default listener bind_address", &config->default_listener->host, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(conf__attempt_resolve(config->default_listener->host, "bind_address", MOSQ_LOG_ERR, "Error")){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "bind_interface")){
#ifdef SO_BINDTODEVICE
					જો(reload){
						ચાલુ;        /* Rebinding listeners not valid during reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					જો(conf__parse_string(&token, "bind_interface", &cur_listener->bind_interface, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_ERR, "Error: bind_interface specified but socket option not available.");
					પાછા MOSQ_ERR_INVAL;
#endif
				}નહીંતર જો(!strcmp(token, "bridge_attempt_unsubscribe")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					જો(conf__parse_bool(&token, "bridge_attempt_unsubscribe", &cur_bridge->attempt_unsubscribe, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "bridge_cafile")){
#જો defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					જો(conf__parse_string(&token, "bridge_cafile", &cur_bridge->tls_cafile, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "bridge_alpn")){
#જો defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					જો(conf__parse_string(&token, "bridge_alpn", &cur_bridge->tls_alpn, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "bridge_ciphers")){
#જો defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					જો(conf__parse_string(&token, "bridge_ciphers", &cur_bridge->tls_ciphers, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "bridge_ciphers_tls1.3")){
#જો defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					જો(conf__parse_string(&token, "bridge_ciphers_tls1.3", &cur_bridge->tls_13_ciphers, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "bridge_bind_address")){
#જો defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					જો(conf__parse_string(&token, "bridge_bind_address", &cur_bridge->bind_address, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "bridge_capath")){
#જો defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					જો(conf__parse_string(&token, "bridge_capath", &cur_bridge->tls_capath, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "bridge_certfile")){
#જો defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					જો(conf__parse_string(&token, "bridge_certfile", &cur_bridge->tls_certfile, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "bridge_identity")){
#જો defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					જો(conf__parse_string(&token, "bridge_identity", &cur_bridge->tls_psk_identity, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "bridge_insecure")){
#જો defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					જો(conf__parse_bool(&token, "bridge_insecure", &cur_bridge->tls_insecure, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(cur_bridge->tls_insecure){
						log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge '%s' using insecure mode.", cur_bridge->name);
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "bridge_require_ocsp")){
#જો defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					જો(conf__parse_bool(&token, "bridge_require_ocsp", &cur_bridge->tls_ocsp_required, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "bridge_max_packet_size")){
#જો defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					જો(conf__parse_int(&token, "bridge_max_packet_size", &tmp_int, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(tmp_int < 0){
						tmp_int = 0;
					}
					cur_bridge->maximum_packet_size = (uint32_t)tmp_int;
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "bridge_max_topic_alias")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					જો(conf__parse_int(&token, "bridge_max_topic_alias", &tmp_int, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}

					જો(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: bridge_max_topic_alias must be > 0 and <= 65535.");
						પાછા MOSQ_ERR_INVAL;
					}
					cur_bridge->max_topic_alias = (uint16_t)tmp_int;
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "bridge_outgoing_retain")){
#જો defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					જો(conf__parse_bool(&token, "bridge_outgoing_retain", &cur_bridge->outgoing_retain, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "bridge_keyfile")){
#જો defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					mosquitto_FREE(cur_bridge->tls_keyfile);
					જો(conf__parse_string(&token, "bridge_keyfile", &cur_bridge->tls_keyfile, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "bridge_protocol_version")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(ખાલી, "", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_protocol_version");

					જો(!strcmp(token, "mqttv31")){
						cur_bridge->protocol_version = mosq_p_mqtt31;
					}નહીંતર જો(!strcmp(token, "mqttv311")){
						cur_bridge->protocol_version = mosq_p_mqtt311;
					}નહીંતર જો(!strcmp(token, "mqttv50")){
						cur_bridge->protocol_version = mosq_p_mqtt5;
					}નહીંતર{
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid 'bridge_protocol_version' value (%s).", token);
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "bridge_psk")){
#જો defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					જો(conf__parse_string(&token, "bridge_psk", &cur_bridge->tls_psk, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "bridge_receive_maximum")){
#જો defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					જો(conf__parse_int(&token, "bridge_receive_maximum", &tmp_int, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(tmp_int <= 0){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be greater than 0.");
						પાછા MOSQ_ERR_INVAL;
					}નહીંતર જો((uint64_t)tmp_int > (uint64_t)UINT16_MAX){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be lower than %u.", UINT16_MAX);
						પાછા MOSQ_ERR_INVAL;
					}
					cur_bridge->receive_maximum = (uint16_t)tmp_int;
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "bridge_reload_type")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(ખાલી, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_reload_type");

					જો(!strcmp(token, "lazy")){
						cur_bridge->reload_type = brt_lazy;
					}નહીંતર જો(!strcmp(token, "immediate")){
						cur_bridge->reload_type = brt_immediate;
					}નહીંતર{
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid 'bridge_reload_type' value in configuration (%s).", token);
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "bridge_session_expiry_interval")){
#જો defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					જો(conf__parse_int(&token, "bridge_session_expiry_interval", &tmp_int, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(tmp_int < 0){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must not be negative.");
						પાછા MOSQ_ERR_INVAL;
					}નહીંતર જો((uint64_t)tmp_int > (uint64_t)MQTT_SESSION_EXPIRY_NEVER){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must be lower than %u.", MQTT_SESSION_EXPIRY_NEVER);
						પાછા MOSQ_ERR_INVAL;
					}
					cur_bridge->session_expiry_interval = (uint32_t)tmp_int;
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "bridge_tcp_keepalive")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);

					જો(conf__parse_int(&token, "bridge_tcp_keepalive_idle", &tmp_int, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(tmp_int <= 0){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: invalid TCP keepalive idle value.");
						પાછા MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_idle = (unsigned પૂર્ણાંક)tmp_int;

					જો(conf__parse_int(&token, "bridge_tcp_keepalive_interval", &tmp_int, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(tmp_int <= 0){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: invalid TCP keepalive interval value.");
						પાછા MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_interval = (unsigned પૂર્ણાંક)tmp_int;

					જો(conf__parse_int(&token, "bridge_tcp_keepalive_counter", &tmp_int, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(tmp_int <= 0){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: invalid TCP keepalive counter value.");
						પાછા MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_counter = (unsigned પૂર્ણાંક)tmp_int;
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "bridge_tcp_user_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
#ifdef WITH_TCP_USER_TIMEOUT
					જો(conf__parse_int(&token, "bridge_tcp_user_timeout", &tmp_int, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(tmp_int < 0){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: invalid TCP user timeout value.");
						પાછા MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_user_timeout = tmp_int;
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge TCP user timeout support not available.");
#endif
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "bridge_tls_use_os_certs")){
#જો defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					જો(conf__parse_bool(&token, "bridge_tls_use_os_certs", &cur_bridge->tls_use_os_certs, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "bridge_tls_version")){
#જો defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					જો(conf__parse_string(&token, "bridge_tls_version", &cur_bridge->tls_version, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "cafile")){
#જો defined(WITH_TLS)
					REQUIRE_LISTENER(token);
					જો(cur_listener->psk_hint){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						પાછા MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->cafile);
					જો(conf__parse_string(&token, "cafile", &cur_listener->cafile, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "capath")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->capath);
					જો(conf__parse_string(&token, "capath", &cur_listener->capath, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "certfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					જો(cur_listener->psk_hint){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						પાછા MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->certfile);
					જો(conf__parse_string(&token, "certfile", &cur_listener->certfile, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "check_retain_source")){
					જો(conf__parse_bool(&token, "check_retain_source", &config->check_retain_source, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "ciphers")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers);
					જો(conf__parse_string(&token, "ciphers", &cur_listener->ciphers, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "ciphers_tls1.3")){
#જો defined(WITH_TLS) && (!defined(LIBRESSL_VERSION_સંખ્યા) || LIBRESSL_VERSION_સંખ્યા > 0x3040000FL)
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers_tls13);
					જો(conf__parse_string(&token, "ciphers_tls1.3", &cur_listener->ciphers_tls13, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: ciphers_tls1.3 support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "clientid") || !strcmp(token, "remote_clientid")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					જો(conf__parse_string(&token, "bridge remote clientid", &cur_bridge->remote_clientid, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "cleansession")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					જો(conf__parse_bool(&token, "cleansession", &cur_bridge->clean_start, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "local_cleansession")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					જો(conf__parse_bool(&token, "local_cleansession", (bool *)&cur_bridge->clean_start_local, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "clientid_prefixes")){
					OPTION_DEPRECATED(token, "");
					mosquitto_FREE(config->clientid_prefixes);
					જો(conf__parse_string(&token, "clientid_prefixes", &config->clientid_prefixes, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "connection")){
#ifdef WITH_BRIDGE
					token = strtok_r(ખાલી, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "connection");

					/* Check for existing bridge name. */
					માટે(i=0; i<config->bridge_ગણતરી; i++){
						જો(!strcmp(config->bridges[i]->name, token)){
							log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Duplicate bridge name '%s'.", token);
							પાછા MOSQ_ERR_INVAL;
						}
					}

					સંરચના mosquitto__bridge **bridges_new = mosquitto_realloc(config->bridges, (size_t)(config->bridge_ગણતરી+1)*કદ(સંરચના mosquitto__bridge *));
					જો(!bridges_new){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
						પાછા MOSQ_ERR_NOMEM;
					}
					config->bridges = bridges_new;
					cur_bridge = mosquitto_malloc(કદ(સંરચના mosquitto__bridge));
					જો(!cur_bridge){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
						પાછા MOSQ_ERR_NOMEM;
					}

					config->bridge_ગણતરી++;
					config->bridges[config->bridge_ગણતરી-1] = cur_bridge;

					memset(cur_bridge, 0, કદ(સંરચના mosquitto__bridge));
					cur_bridge->name = mosquitto_strdup(token);
					જો(!cur_bridge->name){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
						પાછા MOSQ_ERR_NOMEM;
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
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "connection_messages")){
					જો(conf__parse_bool(&token, token, &config->connection_messages, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "crlfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->crlfile);
					જો(conf__parse_string(&token, "crlfile", &cur_listener->crlfile, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "dhparamfile")){
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: dhparamfile is no longer required.");
				}નહીંતર જો(!strcmp(token, "disable_client_cert_date_checks")){
#ifdef WITH_TLS
					REQUIRE_LISTENER(token);
					જો(conf__parse_bool(&token, "disable_client_cert_date_checks", &cur_listener->disable_client_cert_date_checks, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "enable_control_api")){
#ifdef WITH_CONTROL
					જો(conf__parse_bool(&token, "enable_control_api", &config->enable_control_api, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: $CONTROL support not available (enable_control_api).");
#endif
				}નહીંતર જો(!strcmp(token, "enable_proxy_protocol")){
#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(ખાલી, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					પાછા MOSQ_ERR_INVAL;
#endif
					REQUIRE_LISTENER(token);
					જો(conf__parse_int(&token, "enable_proxy_protocol", &cur_listener->enable_proxy_protocol, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(cur_listener->enable_proxy_protocol < 1 || cur_listener->enable_proxy_protocol > 2){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: enable_proxy_protocol must be 1 or 2.");
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "global_max_clients")){
					જો(conf__parse_int(&token, "global_max_clients", &config->global_max_clients, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "global_max_connections")){
					જો(conf__parse_int(&token, "global_max_connections", &config->global_max_connections, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "http_dir")){
#જો defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
					જો(reload){
						ચાલુ;        /* Not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					જો(conf__parse_string(&token, "http_dir", &cur_listener->http_dir, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#ifdef WIN32
					અક્ષર *http_dir_canonical = _fullpath(ખાલી, cur_listener->http_dir, 0);
					અચલ અક્ષર sep = '\\';
#નહીંતર
					અક્ષર *http_dir_canonical = realpath(cur_listener->http_dir, ખાલી);
					અચલ અક્ષર sep = '/';
#endif
					જો(!http_dir_canonical){
						પાછા MOSQ_ERR_NOMEM;
					}
					size_t http_dir_len = strlen(http_dir_canonical) + કદ(sep) + 1;
					અક્ષર *http_dir_canonical_sep = mosquitto_calloc(http_dir_len, કદ(અક્ષર));
					જો(!http_dir_canonical_sep){
						મુક્ત(http_dir_canonical);
						પાછા MOSQ_ERR_NOMEM;
					}
					snprintf(http_dir_canonical_sep, http_dir_len, "%s%c", http_dir_canonical, sep);
					મુક્ત(http_dir_canonical);
					mosquitto_FREE(cur_listener->http_dir);
					cur_listener->http_dir = http_dir_canonical_sep;
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: http_dir support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "idle_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					જો(conf__parse_int(&token, "idle_timeout", &cur_bridge->idle_timeout, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(cur_bridge->idle_timeout < 1){
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "idle_timeout interval too low, using 1 second.");
						cur_bridge->idle_timeout = 1;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "include_dir")){
					જો(level == 0){
						અક્ષર **files;
						પૂર્ણાંક file_ગણતરી;

						/* Only process include_dir from the main config file. */
						token = strtok_r(ખાલી, "", &saveptr);
						REQUIRE_NON_EMPTY_OPTION(token, "include_dir");

						rc = config__get_dir_files(token, &files, &file_ગણતરી);
						જો(rc){
							પાછા rc;
						}

						માટે(i=0; i<file_ગણતરી; i++){
							log__printf(ખાલી, MOSQ_LOG_INFO, "Loading config file '%s'", files[i]);

							rc = config__read_file(config, reload, files[i], cr, level+1, &lineno_ext);
							જો(rc){
								જો(lineno_ext > 0){
									log__printf(ખાલી, MOSQ_LOG_ERR, "Error found at '%s:%d'.", files[i], lineno_ext);
								}
								/* Free happens below */
								તોડો;
							}
						}
						માટે(i=0; i<file_ગણતરી; i++){
							mosquitto_FREE(files[i]);
						}
						mosquitto_FREE(files);
						જો(rc){
							પાછા rc;    /* This returns if config__read_file() fails above */
						}
					}નહીંતર{
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: The include_dir option is only valid in the main configuration file.");
						પાછા 1;
					}
				}નહીંતર જો(!strcmp(token, "keepalive_interval")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					જો(conf__parse_int(&token, "keepalive_interval", &tmp_int, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(tmp_int > UINT16_MAX){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Bridge keepalive value too high.");
						પાછા MOSQ_ERR_INVAL;
					}
					જો(tmp_int < 5){
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "keepalive interval too low, using 5 seconds.");
						tmp_int = 5;
					}
					cur_bridge->keepalive = (uint16_t)tmp_int;
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "keyfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->keyfile);
					જો(conf__parse_string(&token, "keyfile", &cur_listener->keyfile, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "listener")){
					config->local_only = false;

					જો(conf__parse_int(&token, "listener port", &tmp_int, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#ifdef WITH_UNIX_SOCKETS
					જો(tmp_int < 0 || tmp_int > UINT16_MAX){
#નહીંતર
					જો(tmp_int < 1 || tmp_int > UINT16_MAX){
#endif
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						પાછા MOSQ_ERR_INVAL;
					}

					/* Look for bind address / unix socket path */
					token = strtok_r(ખાલી, " ", &saveptr);
					જો(token != ખાલી && token[0] == '#'){
						token = ખાલી;
					}

					જો(tmp_int == 0 && token == ખાલી){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: A listener with port 0 must provide a Unix socket path.");
						પાછા MOSQ_ERR_INVAL;
					}

					જો(reload){
						/* We reload listeners settings based on port number/unix socket path.
						 * If the port number/unix path doesn't already exist, exit with a complaint. */
						cur_listener = ખાલી;
#ifdef WITH_UNIX_SOCKETS
						જો(tmp_int == 0){
							માટે(i=0; i<config->listener_ગણતરી; i++){
								જો(config->listeners[i].unix_socket_path != ખાલી
										&& strcmp(config->listeners[i].unix_socket_path, token) == 0){

									cur_listener = &config->listeners[i];
									તોડો;
								}
							}
						}નહીંતર
#endif
						{
							માટે(i=0; i<config->listener_ગણતરી; i++){
								જો(config->listeners[i].port == tmp_int){
									/* Now check we have a matching bind address, if defined */
									જો(config->listeners[i].host){
										જો(token && !strcmp(config->listeners[i].host, token)){
											/* They both have a bind address, and they match */
											cur_listener = &config->listeners[i];
											તોડો;
										}
									}નહીંતર{
										જો(token == ખાલી){
											/* Neither this config nor the new config have a bind address,
											 * so they match. */
											cur_listener = &config->listeners[i];
											તોડો;
										}
									}
								}
							}
						}
						જો(!cur_listener){
							log__printf(ખાલી, MOSQ_LOG_ERR, "Error: It is not currently possible to add/remove listeners when reloading the config file.");
							પાછા MOSQ_ERR_INVAL;
						}
					}નહીંતર{
						જો(config__add_listener(config)){
							log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
							પાછા MOSQ_ERR_NOMEM;
						}
						cur_listener = &config->listeners[config->listener_ગણતરી-1];
					}

					listener__set_defaults(cur_listener);
					cur_listener->port = (uint16_t)tmp_int;

					mosquitto_FREE(cur_listener->host);

#ifdef WITH_UNIX_SOCKETS
					mosquitto_FREE(cur_listener->unix_socket_path);
#endif

					જો(token){
#ifdef WITH_UNIX_SOCKETS
						જો(cur_listener->port == 0){
							cur_listener->unix_socket_path = mosquitto_strdup(token);
						}નહીંતર
#endif
						{
							cur_listener->host = mosquitto_strdup(token);
						}
					}
				}નહીંતર જો(!strcmp(token, "listener_allow_anonymous")){
					REQUIRE_LISTENER(token);
					જો(conf__parse_bool(&token, "listener_allow_anonymous", (bool *)&cur_listener->security_options->allow_anonymous, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "listener_auto_id_prefix")){
					REQUIRE_LISTENER(token);
					જો(conf__parse_string(&token, "listener_auto_id_prefix", &cur_listener->security_options->auto_id_prefix, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(cur_listener->security_options->auto_id_prefix){
						જો(strlen(cur_listener->security_options->auto_id_prefix) > 50){
							log__printf(ખાલી, MOSQ_LOG_ERR, "Error: 'listener_auto_id_prefix' length must be <= 50.");
							પાછા MOSQ_ERR_INVAL;
						}
						cur_listener->security_options->auto_id_prefix_len = (uint16_t)strlen(cur_listener->security_options->auto_id_prefix);
					}નહીંતર{
						cur_listener->security_options->auto_id_prefix_len = 0;
					}
				}નહીંતર જો(!strcmp(token, "local_clientid")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					જો(conf__parse_string(&token, "bridge local clientd", &cur_bridge->local_clientid, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "local_password")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					જો(conf__parse_string(&token, "bridge local_password", &cur_bridge->local_password, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "local_username")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					જો(conf__parse_string(&token, "bridge local_username", &cur_bridge->local_username, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "log_dest")){
					token = strtok_r(ખાલી, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_dest");

					cr->log_dest_set = 1;
					જો(!strcmp(token, "none")){
						cr->log_dest = MQTT3_LOG_NONE;
					}નહીંતર જો(!strcmp(token, "syslog")){
						cr->log_dest |= MQTT3_LOG_SYSLOG;
					}નહીંતર જો(!strcmp(token, "stdout")){
						cr->log_dest |= MQTT3_LOG_STDOUT;
					}નહીંતર જો(!strcmp(token, "stderr")){
						cr->log_dest |= MQTT3_LOG_STDERR;
					}નહીંતર જો(!strcmp(token, "topic")){
						cr->log_dest |= MQTT3_LOG_TOPIC;
					}નહીંતર જો(!strcmp(token, "dlt")){
						cr->log_dest |= MQTT3_LOG_DLT;
#ifdef ANDROID
					}નહીંતર જો(!strcmp(token, "android")){
						cr->log_dest |= MQTT3_LOG_ANDROID;
#endif
					}નહીંતર જો(!strcmp(token, "file")){
						જો(config->log_fptr || config->log_file){
							log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Duplicate \"log_dest file\" value.");
							પાછા MOSQ_ERR_INVAL;
						}
						/* Get remaining string. */
						token = saveptr;
						જો(token && token[0]){
							જ્યારે(token[0] == ' ' || token[0] == '\t'){
								token++;
							}
						}
						/* Duplicate "token" check here saves a log__printf() */
						જો(token && token[0]){
							config->log_file = mosquitto_strdup(token);
							જો(!config->log_file){
								log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
								પાછા MOSQ_ERR_NOMEM;
							}
							cr->log_dest |= MQTT3_LOG_FILE;
						}નહીંતર{
							log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Empty \"log_dest file\" value in configuration.");
							પાછા MOSQ_ERR_INVAL;
						}
						cr->log_dest |= MQTT3_LOG_FILE;
					}નહીંતર{
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid 'log_dest' value (%s).", token);
						પાછા MOSQ_ERR_INVAL;
					}
#જો defined(WIN32) || defined(__CYGWIN__)
					જો(service_handle){
						જો(cr->log_dest == MQTT3_LOG_STDOUT || cr->log_dest == MQTT3_LOG_STDERR){
							log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Cannot log to stdout/stderr when running as a Windows service.");
							પાછા MOSQ_ERR_INVAL;
						}
					}
#endif
				}નહીંતર જો(!strcmp(token, "log_facility")){
#જો defined(WIN32) || defined(__CYGWIN__)
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: log_facility not supported on Windows.");
#નહીંતર
					જો(conf__parse_int(&token, "log_facility", &tmp_int, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					પસંદગી(tmp_int){
						સ્થિતિ 0:
							config->log_facility = LOG_LOCAL0;
							તોડો;
						સ્થિતિ 1:
							config->log_facility = LOG_LOCAL1;
							તોડો;
						સ્થિતિ 2:
							config->log_facility = LOG_LOCAL2;
							તોડો;
						સ્થિતિ 3:
							config->log_facility = LOG_LOCAL3;
							તોડો;
						સ્થિતિ 4:
							config->log_facility = LOG_LOCAL4;
							તોડો;
						સ્થિતિ 5:
							config->log_facility = LOG_LOCAL5;
							તોડો;
						સ્થિતિ 6:
							config->log_facility = LOG_LOCAL6;
							તોડો;
						સ્થિતિ 7:
							config->log_facility = LOG_LOCAL7;
							તોડો;
						મૂળભૂત:
							log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid 'log_facility' value (%d).", tmp_int);
							પાછા MOSQ_ERR_INVAL;
					}
#endif
				}નહીંતર જો(!strcmp(token, "log_timestamp")){
					જો(conf__parse_bool(&token, token, &config->log_timestamp, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "log_timestamp_format")){
					જો(conf__parse_string(&token, token, &config->log_timestamp_format, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "log_type")){
					token = strtok_r(ખાલી, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_type");

					cr->log_type_set = 1;
					જો(!strcmp(token, "none")){
						cr->log_type = MOSQ_LOG_NONE;
					}નહીંતર જો(!strcmp(token, "information")){
						cr->log_type |= MOSQ_LOG_INFO;
					}નહીંતર જો(!strcmp(token, "notice")){
						cr->log_type |= MOSQ_LOG_NOTICE;
					}નહીંતર જો(!strcmp(token, "warning")){
						cr->log_type |= MOSQ_LOG_WARNING;
					}નહીંતર જો(!strcmp(token, "error")){
						cr->log_type |= MOSQ_LOG_ERR;
					}નહીંતર જો(!strcmp(token, "debug")){
						cr->log_type |= MOSQ_LOG_DEBUG;
					}નહીંતર જો(!strcmp(token, "subscribe")){
						cr->log_type |= MOSQ_LOG_SUBSCRIBE;
					}નહીંતર જો(!strcmp(token, "unsubscribe")){
						cr->log_type |= MOSQ_LOG_UNSUBSCRIBE;
					}નહીંતર જો(!strcmp(token, "internal")){
						cr->log_type |= MOSQ_LOG_INTERNAL;
#ifdef WITH_WEBSOCKETS
					}નહીંતર જો(!strcmp(token, "websockets")){
						cr->log_type |= MOSQ_LOG_WEBSOCKETS;
#endif
					}નહીંતર જો(!strcmp(token, "all")){
						cr->log_type = MOSQ_LOG_ALL;
					}નહીંતર{
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid 'log_type' value (%s).", token);
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "max_connections")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					જો(conf__parse_int(&token, token, &cur_listener->max_connections, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(cur_listener->max_connections < 0){
						cur_listener->max_connections = -1;
					}
				}નહીંતર જો(!strcmp(token, "maximum_qos") || !strcmp(token, "max_qos")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					જો(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(tmp_int < 0 || tmp_int > 2){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: 'max_qos' must be between 0 and 2 inclusive.");
						પાછા MOSQ_ERR_INVAL;
					}
					cur_listener->max_qos = (uint8_t)tmp_int;
				}નહીંતર જો(!strcmp(token, "max_inflight_bytes")){
					જો(conf__parse_int(&token, "max_inflight_bytes", &tmp_int, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_inflight_bytes = (size_t)tmp_int;
				}નહીંતર જો(!strcmp(token, "max_inflight_messages")){
					જો(conf__parse_int(&token, "max_inflight_messages", &tmp_int, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(tmp_int < 0 || tmp_int == UINT16_MAX){
						tmp_int = 0;
					}નહીંતર જો(tmp_int > UINT16_MAX){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: 'max_inflight_messages' must be <= 65535.");
						પાછા MOSQ_ERR_INVAL;
					}
					config->max_inflight_messages = (uint16_t)tmp_int;
				}નહીંતર જો(!strcmp(token, "max_keepalive")){
					જો(conf__parse_int(&token, "max_keepalive", &tmp_int, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid 'max_keepalive' value (%d).", tmp_int);
						પાછા MOSQ_ERR_INVAL;
					}
					config->max_keepalive = (uint16_t)tmp_int;
				}નહીંતર જો(!strcmp(token, "max_packet_size")){
					જો(conf__parse_int(&token, "max_packet_size", &tmp_int, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(tmp_int < 20){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: 'max_packet_size' must be >= 20.");
						પાછા MOSQ_ERR_INVAL;
					}
					config->max_packet_size = (uint32_t)tmp_int;
				}નહીંતર જો(!strcmp(token, "max_queued_bytes")){
					જો(conf__parse_int(&token, "max_queued_bytes", &tmp_int, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_bytes = (size_t)tmp_int;
				}નહીંતર જો(!strcmp(token, "max_queued_messages")){
					જો(conf__parse_int(&token, "max_queued_messages", &tmp_int, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_messages = tmp_int;
				}નહીંતર જો(!strcmp(token, "memory_limit")){
					ssize_t lim;
					જો(conf__parse_ssize_t(&token, "memory_limit", &lim, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(lim < 0){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid 'memory_limit' value (%ld).", lim);
						પાછા MOSQ_ERR_INVAL;
					}
					mosquitto_memory_set_limit((size_t)lim);
				}નહીંતર જો(!strcmp(token, "message_size_limit")){
					log__printf(ખાલી, MOSQ_LOG_NOTICE, "Note: It is recommended to replace `message_size_limit` with `max_packet_size`.");
					જો(conf__parse_int(&token, "message_size_limit", (પૂર્ણાંક *)&config->message_size_limit, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(config->message_size_limit > MQTT_MAX_PAYLOAD){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid 'message_size_limit' value (%u).", config->message_size_limit);
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "mount_point")){
					REQUIRE_LISTENER(token);
					mosquitto_FREE(cur_listener->mount_point);
					જો(conf__parse_string(&token, "mount_point", &cur_listener->mount_point, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(mosquitto_pub_topic_check(cur_listener->mount_point) != MOSQ_ERR_SUCCESS){
						log__printf(ખાલી, MOSQ_LOG_ERR,
								"Error: Invalid 'mount_point' value (%s). Does it contain a wildcard character?",
								cur_listener->mount_point);
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "notifications")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					જો(conf__parse_bool(&token, "notifications", &cur_bridge->notifications, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "notifications_local_only")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					જો(conf__parse_bool(&token, "notifications_local_only", &cur_bridge->notifications_local_only, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "notification_topic")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					જો(conf__parse_string(&token, "notification_topic", &cur_bridge->notification_topic, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "password") || !strcmp(token, "remote_password")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					જો(conf__parse_string(&token, "bridge remote_password", &cur_bridge->remote_password, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "password_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->password_data.password_file);
					જો(conf__parse_string(&token, "password_file", &cur_security_options->password_data.password_file, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "per_listener_settings")){
					OPTION_DEPRECATED(token, "Please see the documentation for how to achieve the same effect.");
					જો(config->per_listener_settings){
						/* Once this is set, don't let it be unset. It should be the first config option ideally. */
						ચાલુ;
					}
					જો(conf__parse_bool(&token, "per_listener_settings", &config->per_listener_settings, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(cur_security_options && config->per_listener_settings){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: per_listener_settings must be set before any other security settings.");
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "persistence") || !strcmp(token, "retained_persistence")){
					જો(conf__parse_bool(&token, token, &config->persistence, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "persistence_file")){
					જો(conf__parse_string(&token, "persistence_file", &config->persistence_file, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "persistence_location")){
					જો(conf__parse_string(&token, "persistence_location", &config->persistence_location, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "persistent_client_expiration")){
					token = strtok_r(ખાલી, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "persistent_client_expiration");

					time_t expiration_mult;

					પસંદગી(token[strlen(token)-1]){
						સ્થિતિ 's':
							expiration_mult = 1;
							તોડો;
						સ્થિતિ 'h':
							expiration_mult = 3600;
							તોડો;
						સ્થિતિ 'd':
							expiration_mult = 86400;
							તોડો;
						સ્થિતિ 'w':
							expiration_mult = 86400*7;
							તોડો;
						સ્થિતિ 'm':
							expiration_mult = 86400*30;
							તોડો;
						સ્થિતિ 'y':
							expiration_mult = 86400*365;
							તોડો;
						મૂળભૂત:
							log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
							પાછા MOSQ_ERR_INVAL;
					}
					token[strlen(token)-1] = '\0';
					config->persistent_client_expiration = atoi(token)*expiration_mult;
					જો(config->persistent_client_expiration <= 0){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "pid_file")){
					જો(reload){
						ચાલુ;        /* pid file not valid for reloading. */
					}
					જો(conf__parse_string(&token, "pid_file", &config->pid_file, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "port")){
					OPTION_DEPRECATED(token, "Please use 'listener' instead.");
					config->local_only = false;
					જો(reload){
						ચાલુ;        /* Listeners not valid for reloading. */

					}
					જો(config__create_default_listener(config, token)){
						પાછા MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					જો(config->default_listener->port){
						log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Default listener port specified multiple times. Only the latest will be used.");
					}
					જો(conf__parse_int(&token, "port", &tmp_int, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(tmp_int < 1 || tmp_int > UINT16_MAX){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						પાછા MOSQ_ERR_INVAL;
					}
					config->default_listener->port = (uint16_t)tmp_int;
				}નહીંતર જો(!strcmp(token, "protocol")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(ખાલી, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "protocol");

					જો(!strcmp(token, "mqtt")){
						cur_listener->protocol = mp_mqtt;
						/*
						}else if(!strcmp(token, "mqttsn")){
						    cur_listener->protocol = mp_mqttsn;
						*/
					}નહીંતર જો(!strcmp(token, "websockets")){
#ifdef WITH_WEBSOCKETS
						cur_listener->protocol = mp_websockets;
#નહીંતર
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Websockets support not available.");
						પાછા MOSQ_ERR_INVAL;
#endif
					}નહીંતર જો(!strcmp(token, "http_api")){
#ifdef WITH_HTTP_API
						cur_listener->protocol = mp_http_api;
#નહીંતર
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: HTTP API support not available.");
						પાછા MOSQ_ERR_INVAL;
#endif
					}નહીંતર{
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid 'protocol' value (%s).", token);
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "proxy_protocol_v2_require_tls")){
#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(ખાલી, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					પાછા MOSQ_ERR_INVAL;
#endif
					REQUIRE_LISTENER(token);
					જો(conf__parse_bool(&token, "proxy_protocol_v2_require_tls", &cur_listener->proxy_protocol_v2_require_tls, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "psk_file")){
#ifdef FINAL_WITH_TLS_PSK
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					જો(cur_listener && cur_listener->certfile){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						પાછા MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_security_options->psk_file);
					જો(conf__parse_string(&token, "psk_file", &cur_security_options->psk_file, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "psk_hint")){
#ifdef FINAL_WITH_TLS_PSK
					જો(reload){
						ચાલુ;        /* PSK file not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					જો(conf__parse_string(&token, "psk_hint", &cur_listener->psk_hint, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "queue_qos0_messages")){
					જો(conf__parse_bool(&token, token, &config->queue_qos0_messages, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "require_certificate")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					જો(conf__parse_bool(&token, "require_certificate", &cur_listener->require_certificate, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "restart_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					cur_bridge->backoff_cap = 0; /* set backoff to constant mode, unless cap is specified further down */
					token = strtok_r(ખાલી, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "restart_timeout");

					cur_bridge->restart_timeout = atoi(token);
					cur_bridge->backoff_base = 0;
					cur_bridge->backoff_cap = 0;
					જો(cur_bridge->restart_timeout < 1){
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "restart_timeout interval too low, using 1 second.");
						cur_bridge->restart_timeout = 1;
					}નહીંતર જો(cur_bridge->restart_timeout > 3600){
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "restart_timeout interval too high, using 3600 seconds.");
						cur_bridge->restart_timeout = 3600;
					}
					token = strtok_r(ખાલી, " ", &saveptr);
					જો(token){
						cur_bridge->backoff_base = cur_bridge->restart_timeout;
						cur_bridge->backoff_cap = atoi(token);
						જો(cur_bridge->backoff_cap < cur_bridge->backoff_base){
							log__printf(ખાલી, MOSQ_LOG_ERR, "Error: backoff cap is lower than the base in restart_timeout.");
							પાછા MOSQ_ERR_INVAL;
						}નહીંતર જો(cur_bridge->backoff_cap > 7200){
							log__printf(ખાલી, MOSQ_LOG_ERR, "Error: backoff cap too high, using 7200 seconds.");
							cur_bridge->backoff_cap = 7200;
						}

						token = strtok_r(ખાલી, " ", &saveptr);
						જો(token){
							cur_bridge->stable_connection_period = atoi(token);
							જો(cur_bridge->stable_connection_period < 0){
								log__printf(ખાલી, MOSQ_LOG_ERR, "Error: stable connection period cannot be negative.");
								પાછા MOSQ_ERR_INVAL;
							}
						}
					}
					cur_bridge->restart_timeout *= 1000; /* backoff is tracked in ms */
					cur_bridge->backoff_base *= 1000;
					cur_bridge->backoff_cap *= 1000;
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "retain_available")){
					જો(conf__parse_bool(&token, token, &config->retain_available, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "retain_expiry_interval")){
					જો(conf__parse_int(&token, token, &config->retain_expiry_interval, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(config->retain_expiry_interval < 1){
						log__printf(ખાલી, MOSQ_LOG_WARNING, "Error: retain_expiry_interval must be >= 1.");
						પાછા MOSQ_ERR_INVAL;
					}નહીંતર જો(config->retain_expiry_interval > 10000000){
						log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: retain_expiry_interval being capped at 19 years.");
						config->retain_expiry_interval = 10000000;
					}
					config->retain_expiry_interval *= 60;
				}નહીંતર જો(!strcmp(token, "retry_interval")){
					OPTION_UNAVAILABLE(token);
				}નહીંતર જો(!strcmp(token, "round_robin")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					જો(conf__parse_bool(&token, "round_robin", &cur_bridge->round_robin, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "set_tcp_nodelay")){
					જો(conf__parse_bool(&token, "set_tcp_nodelay", &config->set_tcp_nodelay, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "start_type")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(ખાલી, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					જો(!strcmp(token, "automatic")){
						cur_bridge->start_type = bst_automatic;
					}નહીંતર જો(!strcmp(token, "lazy")){
						cur_bridge->start_type = bst_lazy;
					}નહીંતર જો(!strcmp(token, "manual")){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Manual start_type not supported.");
						પાછા MOSQ_ERR_INVAL;
					}નહીંતર જો(!strcmp(token, "once")){
						cur_bridge->start_type = bst_once;
					}નહીંતર{
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid 'start_type' value in configuration (%s).", token);
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "socket_domain")){
					જો(reload){
						ચાલુ;        /* socket_domain not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(ખાલી, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					જો(!strcmp(token, "ipv4")){
						cur_listener->socket_domain = AF_INET;
					}નહીંતર જો(!strcmp(token, "ipv6")){
						cur_listener->socket_domain = AF_INET6;
					}નહીંતર{
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid 'socket_domain' value '%s' in configuration.", token);
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "sys_interval")){
					જો(conf__parse_int(&token, "sys_interval", &config->sys_interval, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(config->sys_interval < 0 || config->sys_interval > 65535){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid 'sys_interval' value (%d).", config->sys_interval);
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "threshold")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					જો(conf__parse_int(&token, "threshold", &cur_bridge->threshold, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(cur_bridge->threshold < 1){
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "threshold too low, using 1 message.");
						cur_bridge->threshold = 1;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "tls_engine")){
#ifdef WITH_TLS
					જો(reload){
						ચાલુ;        /* tls_engine not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					જો(conf__parse_string(&token, "tls_engine", &cur_listener->tls_engine, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "tls_engine_kpass_sha1")){
#ifdef WITH_TLS
					જો(reload){
						ચાલુ;        /* tls_engine not valid for reloading. */

					}
					અક્ષર *kpass_sha = ખાલી, *kpass_sha_bin = ખાલી;
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					જો(conf__parse_string(&token, "tls_engine_kpass_sha1", &kpass_sha, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(mosquitto__hex2bin_sha1(kpass_sha, (unsigned અક્ષર **)&kpass_sha_bin) != MOSQ_ERR_SUCCESS){
						mosquitto_FREE(kpass_sha);
						પાછા MOSQ_ERR_INVAL;
					}
					mosquitto_free(cur_listener->tls_engine_kpass_sha1);
					cur_listener->tls_engine_kpass_sha1 = kpass_sha_bin;
					mosquitto_FREE(kpass_sha);
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "tls_keyform")){
#ifdef WITH_TLS
					જો(reload){
						ચાલુ;        /* tls_engine not valid for reloading. */

					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					અક્ષર *keyform = ખાલી;
					જો(conf__parse_string(&token, "tls_keyform", &keyform, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					cur_listener->tls_keyform = mosq_k_pem;
					જો(!strcmp(keyform, "engine")){
						cur_listener->tls_keyform = mosq_k_engine;
					}
					mosquitto_FREE(keyform);
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "tls_version")){
#જો defined(WITH_TLS)
					જો(reload){
						ચાલુ;        /* tls_version not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					જો(conf__parse_string(&token, "tls_version", &cur_listener->tls_version, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "topic")){
#ifdef WITH_BRIDGE
					અક્ષર *topic = ખાલી;
					ગણના mosquitto__bridge_direction direction = bd_out;
					uint8_t qos = 0;
					અક્ષર *local_prefix = ખાલી, *remote_prefix = ખાલી;

					REQUIRE_BRIDGE(token);

					token = strtok_r(ખાલી, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "topic");

					// Check if the topic is quoted (e.g. for spaces within topic names), but not the
					// special case of ""
					જો(token[0] == '"' && token [1] != '"'){
						જો(strchr(saveptr, '"') == ખાલી){
							log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Missing closing quote in topic value (%s).", saveptr);
							પાછા MOSQ_ERR_INVAL;
						}

						અક્ષર *topic_in_quotes = strtok_r(ખાલી, "\"", &saveptr);
						size_t tlen = 1;
						જો(topic_in_quotes){
							tlen = strlen(topic_in_quotes);
						}

						topic = mosquitto_malloc(strlen(token) + tlen + 1);
						જો(!topic){
							log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
							પાછા MOSQ_ERR_NOMEM;
						}

						strcpy(topic, token + 1);
						strcat(topic, " ");
						જો(topic_in_quotes){
							strcat(topic, topic_in_quotes);
						}
					}નહીંતર{
						topic = mosquitto_strdup(token);
						જો(!topic){
							log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
							પાછા MOSQ_ERR_NOMEM;
						}
					}

					token = strtok_r(ખાલી, " ", &saveptr);
					જો(token){
						જો(!strcasecmp(token, "out")){
							direction = bd_out;
						}નહીંતર જો(!strcasecmp(token, "in")){
							direction = bd_in;
						}નહીંતર જો(!strcasecmp(token, "both")){
							direction = bd_both;
						}નહીંતર{
							log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid bridge topic direction '%s'.", token);
							mosquitto_FREE(topic);
							પાછા MOSQ_ERR_INVAL;
						}
						token = strtok_r(ખાલી, " ", &saveptr);
						જો(token){
							જો(token[0] == '#'){
								(શૂન્ય)strtok_r(ખાલી, "", &saveptr);
							}
							qos = (uint8_t)atoi(token);
							જો(qos > 2){
								log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid bridge QoS level '%s'.", token);
								mosquitto_FREE(topic);
								પાછા MOSQ_ERR_INVAL;
							}

							token = strtok_r(ખાલી, " ", &saveptr);
							જો(token){
								જો(!strcmp(token, "\"\"") || token[0] == '#'){
									local_prefix = ખાલી;
									જો(token[0] == '#'){
										(શૂન્ય)strtok_r(ખાલી, "", &saveptr);
									}
								}નહીંતર{
									local_prefix = token;
								}

								token = strtok_r(ખાલી, " ", &saveptr);
								જો(token){
									જો(!strcmp(token, "\"\"") || token[0] == '#'){
										remote_prefix = ખાલી;
									}નહીંતર{
										remote_prefix = token;
									}
								}
							}
						}
					}

					જો(bridge__add_topic(cur_bridge, topic, direction, qos, local_prefix, remote_prefix)){
						mosquitto_FREE(topic);
						પાછા MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(topic);
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "max_topic_alias")){
					REQUIRE_LISTENER(token);
					જો(conf__parse_int(&token, "max_topic_alias", &tmp_int, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias' value in configuration.");
						પાછા MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias = (uint16_t)tmp_int;
				}નહીંતર જો(!strcmp(token, "max_topic_alias_broker")){
					REQUIRE_LISTENER(token);
					જો(conf__parse_int(&token, "max_topic_alias_broker", &tmp_int, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias_broker' value in configuration.");
						પાછા MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias_broker = (uint16_t)tmp_int;
				}નહીંતર જો(!strcmp(token, "try_private")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					જો(conf__parse_bool(&token, "try_private", &cur_bridge->try_private, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "upgrade_outgoing_qos")){
					જો(conf__parse_bool(&token, token, &config->upgrade_outgoing_qos, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "use_identity_as_username")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					જો(conf__parse_bool(&token, "use_identity_as_username", &cur_listener->use_identity_as_username, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "use_subject_as_username")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					જો(conf__parse_bool(&token, "use_subject_as_username", &cur_listener->use_subject_as_username, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "user")){
					જો(reload){
						ચાલુ;        /* Drop privileges user not valid for reloading. */
					}
					mosquitto_FREE(config->user);
					જો(conf__parse_string(&token, "user", &config->user, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "use_username_as_clientid")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					જો(conf__parse_bool(&token, "use_username_as_clientid", &cur_listener->use_username_as_clientid, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
				}નહીંતર જો(!strcmp(token, "username") || !strcmp(token, "remote_username")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					જો(conf__parse_string(&token, "bridge remote_username", &cur_bridge->remote_username, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}નહીંતર જો(!strcmp(token, "websockets_log_level")){
#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					જો(conf__parse_int(&token, "websockets_log_level", &config->websockets_log_level, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
#endif
				}નહીંતર જો(!strcmp(token, "websockets_headers_size") || !strcmp(token, "packet_buffer_size")){
					જો(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					જો(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(ખાલી, MOSQ_LOG_WARNING, "Error: Packet buffer size must be between 0 and 65535 inclusive.");
						પાછા MOSQ_ERR_INVAL;
					}
					config->packet_buffer_size = (uint16_t)tmp_int;
				}નહીંતર જો(!strcmp(token, "websockets_origin")){
#ifdef WITH_WEBSOCKETS
#  જો LWS_LIBRARY_VERSION_સંખ્યા >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN
					REQUIRE_LISTENER(token);
					ws_origins = mosquitto_realloc(cur_listener->ws_origins, કદ(અક્ષર *)*(size_t)(cur_listener->ws_origin_ગણતરી+1));
					જો(ws_origins == ખાલી){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
						પાછા MOSQ_ERR_NOMEM;
					}
					ws_origins[cur_listener->ws_origin_ગણતરી] = ખાલી;
					cur_listener->ws_origins = ws_origins;
					જો(conf__parse_string(&token, "websockets_origin", &ws_origins[cur_listener->ws_origin_ગણતરી], &saveptr)){
						પાછા MOSQ_ERR_INVAL;
					}
					cur_listener->ws_origin_ગણતરી++;
#  નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: websockets_origin support not available, libwebsockets version is too old.");
#  endif
#નહીંતર
					log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Websockets support not available.");
#endif
				}નહીંતર{
					log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unknown configuration variable '%s'.", token);
					પાછા MOSQ_ERR_INVAL;
				}
			}
		}
	}
	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક config__read_file(સંરચના mosquitto__config *config, bool reload, અચલ અક્ષર *file, સંરચના config_recurse *cr, પૂર્ણાંક level, પૂર્ણાંક *lineno)
{
	પૂર્ણાંક rc;
	FILE *fptr = ખાલી;
	અક્ષર *buf;
	પૂર્ણાંક buflen;
#ifndef WIN32
	DIR *dir;
#endif

#ifndef WIN32
	dir = opendir(file);
	જો(dir){
		closedir(dir);
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Config file '%s' is a directory.", file);
		પાછા 1;
	}
#endif

	fptr = mosquitto_fopen(file, "rt", false);
	જો(!fptr){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to open config file '%s'.", file);
		પાછા 1;
	}

	buflen = 1000;
	buf = mosquitto_malloc((size_t)buflen);
	જો(!buf){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
		fclose(fptr);
		પાછા MOSQ_ERR_NOMEM;
	}

	rc = config__read_file_core(config, reload, cr, level, lineno, fptr, &buf, &buflen);
	mosquitto_FREE(buf);
	fclose(fptr);

	પાછા rc;
}


સ્થિર પૂર્ણાંક config__check_proxy(સંરચના mosquitto__config *config)
{
	માટે(પૂર્ણાંક i=0; i<config->listener_ગણતરી; i++){
		સંરચના mosquitto__listener *l = &config->listeners[i];

		જો(l->enable_proxy_protocol == 2){
#ifdef WITH_TLS
			જો(l->use_subject_as_username){
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error: use_subject_as_username cannot be used with `enable_proxy_protocol 2`.");
				પાછા MOSQ_ERR_INVAL;
			}

			જો(l->certfile || l->keyfile){
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error: certfile and keyfile cannot be used with `enable_proxy_protocol 2`.");
				પાછા MOSQ_ERR_INVAL;
			}
#endif
		}
	}

	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક config__check(સંરચના mosquitto__config *config)
{
	/* Checks that are easy to make after the config has been loaded. */

	અચલ અક્ષર *id_prefix;
	પૂર્ણાંક id_prefix_len;
	જો(config->security_options.auto_id_prefix){
		id_prefix = config->security_options.auto_id_prefix;
		id_prefix_len = config->security_options.auto_id_prefix_len;
	}નહીંતર{
		id_prefix = "auto-";
		id_prefix_len = (પૂર્ણાંક)strlen("auto-");
	}

	/* Default to auto_id_prefix = 'auto-' if none set. */
	માટે(પૂર્ણાંક i=0; i<config->listener_ગણતરી; i++){
		જો(!config->listeners[i].security_options->auto_id_prefix){
			config->listeners[i].security_options->auto_id_prefix = mosquitto_strdup(id_prefix);
			જો(!config->listeners[i].security_options->auto_id_prefix){
				પાછા MOSQ_ERR_NOMEM;
			}
			config->listeners[i].security_options->auto_id_prefix_len = (uint16_t)id_prefix_len;
		}
	}

	પાછા config__check_proxy(config);
}

#ifdef WITH_BRIDGE


સ્થિર પૂર્ણાંક config__check_bridges(સંરચના mosquitto__config *config)
{
	સંરચના mosquitto__bridge *bridge1, *bridge2;
	અક્ષર hostname[256];
	size_t len;

	/* Check for bridge duplicate local_clientid, need to generate missing IDs
	 * first. */
	માટે(પૂર્ણાંક i=0; i<config->bridge_ગણતરી; i++){
		bridge1 = config->bridges[i];

		જો(!bridge1->remote_clientid){
			જો(!gethostname(hostname, 256)){
				len = strlen(hostname) + strlen(bridge1->name) + 2;
				bridge1->remote_clientid = mosquitto_malloc(len);
				જો(!bridge1->remote_clientid){
					પાછા MOSQ_ERR_NOMEM;
				}
				snprintf(bridge1->remote_clientid, len, "%s.%s", hostname, bridge1->name);
			}નહીંતર{
				પાછા 1;
			}
		}

		જો(!bridge1->local_clientid){
			len = strlen(bridge1->remote_clientid) + strlen("local.") + 2;
			bridge1->local_clientid = mosquitto_malloc(len);
			જો(!bridge1->local_clientid){
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
				પાછા MOSQ_ERR_NOMEM;
			}
			snprintf(bridge1->local_clientid, len, "local.%s", bridge1->remote_clientid);
		}
	}

	માટે(પૂર્ણાંક i=0; i<config->bridge_ગણતરી; i++){
		bridge1 = config->bridges[i];
		માટે(પૂર્ણાંક j=i+1; j<config->bridge_ગણતરી; j++){
			bridge2 = config->bridges[j];
			જો(!strcmp(bridge1->local_clientid, bridge2->local_clientid)){
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Bridge local_clientid "
						"'%s' is not unique. Try changing or setting the "
						"local_clientid value for one of the bridges.",
						bridge1->local_clientid);
				પાછા MOSQ_ERR_INVAL;
			}
		}
	}

#ifdef WITH_TLS
	/* Check for missing TLS cafile/capath/certfile/keyfile */
	માટે(પૂર્ણાંક i=0; i<config->listener_ગણતરી; i++){
		bool cafile = !!config->listeners[i].cafile;
		bool capath = !!config->listeners[i].capath;
		bool certfile = !!config->listeners[i].certfile;
		bool keyfile = !!config->listeners[i].keyfile;

		જો((certfile && !keyfile) || (!certfile && keyfile)){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Both certfile and keyfile must be provided to enable a TLS listener.");
			પાછા MOSQ_ERR_INVAL;
		}
		જો(cafile && !certfile){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: cafile specified without certfile and keyfile.");
			પાછા MOSQ_ERR_INVAL;
		}
		જો(capath && !certfile){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: capath specified without certfile and keyfile.");
			પાછા MOSQ_ERR_INVAL;
		}
	}
#endif
	પાછા MOSQ_ERR_SUCCESS;
}
#endif


સ્થિર પૂર્ણાંક conf__parse_bool(અક્ષર **token, અચલ અક્ષર *name, bool *કિંમત, અક્ષર **saveptr)
{
	*token = strtok_r(ખાલી, " ", saveptr);
	જો(*token){
		જો(!strcmp(*token, "false") || !strcmp(*token, "0")){
			*કિંમત = false;
		}નહીંતર જો(!strcmp(*token, "true") || !strcmp(*token, "1")){
			*કિંમત = true;
		}નહીંતર{
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Invalid '%s' value (%s).", name, *token);
			પાછા MOSQ_ERR_INVAL;
		}
	}નહીંતર{
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		પાછા MOSQ_ERR_INVAL;
	}

	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક conf__parse_int(અક્ષર **token, અચલ અક્ષર *name, પૂર્ણાંક *કિંમત, અક્ષર **saveptr)
{
	*token = strtok_r(ખાલી, " ", saveptr);
	જો(*token){
		અક્ષર *endptr = ખાલી;
		long l = strtol(*token, &endptr, 0);
		જો(endptr == *token || endptr[0] != '\0'){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: '%s' value not a number.", name);
			પાછા MOSQ_ERR_INVAL;
		}
		જો(l > INT_MAX || l < INT_MIN){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: '%s' value out of range.", name);
			પાછા MOSQ_ERR_INVAL;
		}
		*કિંમત = (પૂર્ણાંક)l;
	}નહીંતર{
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		પાછા MOSQ_ERR_INVAL;
	}

	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક conf__parse_ssize_t(અક્ષર **token, અચલ અક્ષર *name, ssize_t *કિંમત, અક્ષર **saveptr)
{
	*token = strtok_r(ખાલી, " ", saveptr);
	જો(*token){
		*કિંમત = atol(*token);
	}નહીંતર{
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		પાછા MOSQ_ERR_INVAL;
	}

	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક conf__parse_string(અક્ષર **token, અચલ અક્ષર *name, અક્ષર **કિંમત, અક્ષર **saveptr)
{
	size_t tlen;

	*token = strtok_r(ખાલી, "", saveptr);
	જો(*token){
		જો(*કિંમત){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Duplicate '%s' value in configuration.", name);
			પાછા MOSQ_ERR_INVAL;
		}
		/* Deal with multiple spaces at the beginning of the string. */
		*token = mosquitto_trimblanks(*token);
		જો(strlen(*token) == 0){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
			પાછા MOSQ_ERR_INVAL;
		}

		tlen = strlen(*token);
		જો(tlen > UINT16_MAX){
			પાછા MOSQ_ERR_INVAL;
		}
		જો(mosquitto_validate_utf8(*token, (uint16_t)tlen)){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Malformed UTF-8 in configuration.");
			પાછા MOSQ_ERR_INVAL;
		}
		*કિંમત = mosquitto_strdup(*token);
		જો(!*કિંમત){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
			પાછા MOSQ_ERR_NOMEM;
		}
	}નહીંતર{
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		પાછા MOSQ_ERR_INVAL;
	}
	પાછા MOSQ_ERR_SUCCESS;
}
