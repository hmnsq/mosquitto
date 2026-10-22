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

#ضمن <limits.h>
#ضمن <stdio.h>
#ضمن <stdlib.h>
#ضمن <string.h>
#ضمن <errno.h>

#ifdef WIN32
#ܐܚܪܢܐ
#  include <dirent.h>
#  include <strings.h>
#endif

#ifndef WIN32
#  include <netdb.h>
#  include <sys/socket.h>
#ܐܚܪܢܐ
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#ܐܢ !defined(WIN32) && !defined(__CYGWIN__)
#  include <syslog.h>
#endif

#ضمن "mosquitto_broker_internal.h"
#ضمن "tls_mosq.h"
#ضمن "util_mosq.h"
#ضمن "mosquitto/mqtt_protocol.h"

#ضمن "utlist.h"

بنية config_recurse {
	unsigned صحيح log_dest;
	صحيح log_dest_set;
	unsigned صحيح log_type;
	صحيح log_type_set;
};

#ܐܢ defined(WIN32) || defined(__CYGWIN__)
#ضمن <windows.h>
خارجي SERVICE_STATUS_HANDLE service_handle;
#endif


#define REQUIRE_LISTENER(A) \
		ܥܒܕ{ \
			ܐܢ(cur_listener == عدم){ \
				log__printf(عدم, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				ܦܢܐ MOSQ_ERR_INVAL; \
			} \
		}ܟܡܐ(0)

#define REQUIRE_LISTENER_OR_DEFAULT_LISTENER(A) \
		ܥܒܕ{ \
			ܐܢ(cur_listener == عدم){ \
				ܐܢ(config__create_default_listener(config, (A))){ \
					ܦܢܐ MOSQ_ERR_NOMEM; \
				} \
				cur_listener = config->default_listener; \
			} \
		}ܟܡܐ(0)

#define REQUIRE_LISTENER_IF_PER_LISTENER(A) \
		ܥܒܕ{ \
			ܐܢ(config->per_listener_settings == true && cur_listener == عدم){ \
				log__printf(عدم, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				ܦܢܐ MOSQ_ERR_INVAL; \
			} \
		}ܟܡܐ(0)

#define REQUIRE_NON_DEFAULT_LISTENER(A) \
		ܥܒܕ{ \
			ܐܢ(cur_listener == config->default_listener || cur_listener == عدم){ \
				log__printf(عدم, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				ܦܢܐ MOSQ_ERR_INVAL; \
			} \
		}ܟܡܐ(0)

#define REQUIRE_BRIDGE(A) \
		ܥܒܕ{ \
			ܐܢ(cur_bridge == عدم){ \
				log__printf(عدم, MOSQ_LOG_ERR, "Error: The '%s' option requires a bridge to be defined first.", (A)); \
				ܦܢܐ MOSQ_ERR_INVAL; \
			} \
		}ܟܡܐ(0)

#define REQUIRE_PLUGIN(A) \
		ܥܒܕ{ \
			ܐܢ(cur_plugin == عدم){ \
				log__printf(عدم, MOSQ_LOG_ERR, "Error: The '%s' option requires plugin/global_plugin/plugin_load to be defined first.", (A)); \
				ܦܢܐ MOSQ_ERR_INVAL; \
			} \
		}ܟܡܐ(0)

#define OPTION_DEPRECATED(A, B) \
		log__printf(عدم, MOSQ_LOG_NOTICE, "The '%s' option is now deprecated and will be removed in version 3.0. %s", (A), (B))

#define OPTION_UNAVAILABLE(A) \
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: The '%s' option is no longer available.", (A));

#define REQUIRE_NON_EMPTY_OPTION(A, B) \
		ܥܒܕ{ \
			ܐܢ(!(A)){ \
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", (B)); \
				ܦܢܐ MOSQ_ERR_INVAL; \
			} \
		}ܟܡܐ(0)

#define PER_LISTENER_ALTERNATIVE(A, B) \
		ܐܢ(config->per_listener_settings){ \
			log__printf(عدم, MOSQ_LOG_NOTICE, "You are using the '%s' option with 'per_listener_settings true'. Please replace this with '%s'.", A, B); \
		} \


#ifdef FINAL_WITH_TLS_PSK
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A) \
		ܥܒܕ{ \
			ܐܢ(cur_bridge->tls_psk_identity || cur_bridge->tls_psk){ \
				log__printf(عدم, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and psk encryption in a single bridge.", (A)); \
				ܦܢܐ MOSQ_ERR_INVAL; \
			} \
		}ܟܡܐ(0)

#  define REQUIRE_BRIDGE_NO_X509(A) \
		ܥܒܕ{ \
			ܐܢ(cur_bridge->tls_cafile || cur_bridge->tls_capath || cur_bridge->tls_certfile || cur_bridge->tls_keyfile){ \
				log__printf(عدم, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and identity encryption in a single bridge.", (A)); \
				ܦܢܐ MOSQ_ERR_INVAL; \
			} \
		}ܟܡܐ(0)

#ܐܚܪܢܐ
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A)
#  define REQUIRE_BRIDGE_NO_X509(A)
#endif

ساكن بنية mosquitto__security_options *cur_security_options = عدم;

ساكن صحيح conf__parse_bool(حرف **token, ثابت حرف *name, bool *value, حرف **saveptr);
ساكن صحيح conf__parse_int(حرف **token, ثابت حرف *name, صحيح *value, حرف **saveptr);
ساكن صحيح conf__parse_ssize_t(حرف **token, ثابت حرف *name, ssize_t *value, حرف **saveptr);
ساكن صحيح conf__parse_string(حرف **token, ثابت حرف *name, حرف **value, حرف **saveptr);
ساكن صحيح config__read_file(بنية mosquitto__config *config, bool reload, ثابت حرف *file, بنية config_recurse *config_tmp, صحيح level, صحيح *lineno);
ساكن صحيح config__check(بنية mosquitto__config *config);
ساكن فراغ config__cleanup_plugins(فراغ);
#ifdef WITH_BRIDGE
ساكن صحيح config__check_bridges(بنية mosquitto__config *config);
#endif


ساكن صحيح config__add_listener(بنية mosquitto__config *config)
{
	بنية mosquitto__listener *listener;
	بنية mosquitto__listener *new_listeners;
	صحيح def_listener = -1;

	ܐܢ(config->default_listener){
		لكل(صحيح i=0; i<config->listener_count; i++){
			ܐܢ(&config->listeners[i] == config->default_listener){
				def_listener = i;
				اكسر;
			}
		}
	}
	new_listeners = mosquitto_realloc(config->listeners, حجم(بنية mosquitto__listener)*(size_t)(config->listener_count+1));
	ܐܢ(!new_listeners){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		ܦܢܐ MOSQ_ERR_NOMEM;
	}
	config->listeners = new_listeners;
	listener = &config->listeners[config->listener_count];
	memset(listener, 0, حجم(بنية mosquitto__listener));
	listener->security_options = mosquitto_calloc(1, حجم(بنية mosquitto__security_options));
	ܐܢ(!listener->security_options){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		ܦܢܐ MOSQ_ERR_NOMEM;
	}
	ܐܢ(def_listener != -1){
		config->default_listener = &config->listeners[def_listener];
	}
	config->listener_count++;

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


ساكن صحيح config__create_default_listener(بنية mosquitto__config *config, ثابت حرف *option_name)
{
	ܐܢ(config->default_listener){
		ܦܢܐ MOSQ_ERR_SUCCESS;
	}
	log__printf(عدم, MOSQ_LOG_INFO, "Creating default listener due to '%s' option.", option_name);
	log__printf(عدم, MOSQ_LOG_INFO, "It is best practice to define a 'listener' first. Using the '%s' option without a listener will be disabled in the future.", option_name);
	ܐܢ(config__add_listener(config)){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		ܦܢܐ MOSQ_ERR_NOMEM;
	}

	config->default_listener = &config->listeners[config->listener_count-1];
	listener__set_defaults(config->default_listener);
	config->default_listener->port = 1883;

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


ساكن فراغ conf__set_cur_security_options(بنية mosquitto__config *config, بنية mosquitto__listener **cur_listener, بنية mosquitto__security_options **security_options, ثابت حرف *option_name)
{
	ܐܢ(config->per_listener_settings){
		ܐܢ(*cur_listener == عدم){
			ܐܢ(config__create_default_listener(config, option_name)){
				ܦܢܐ;
			}
			*cur_listener = config->default_listener;
		}
		(*security_options) = (*cur_listener)->security_options;
	}ܐܚܪܢܐ{
		(*security_options) = &config->security_options;
	}
}


ساكن صحيح conf__attempt_resolve(ثابت حرف *host, ثابت حرف *text, unsigned صحيح log, ثابت حرف *msg)
{
	بنية addrinfo gai_hints;
	بنية addrinfo *gai_res;
	صحيح rc;

	memset(&gai_hints, 0, حجم(بنية addrinfo));
	gai_hints.ai_family = AF_UNSPEC;
	gai_hints.ai_socktype = SOCK_STREAM;
	gai_res = عدم;
	rc = getaddrinfo(host, عدم, &gai_hints, &gai_res);
	ܐܢ(gai_res){
		freeaddrinfo(gai_res);
	}
	ܐܢ(rc != 0){
#ifndef WIN32
		ܐܢ(rc == EAI_SYSTEM){
			ܐܢ(errno == ENOENT){
				log__printf(عدم, log, "%s: Unable to resolve %s %s.", msg, text, host);
			}ܐܚܪܢܐ{
				log__printf(عدم, log, "%s: Error resolving %s: %s.", msg, text, strerror(errno));
			}
		}ܐܚܪܢܐ{
			log__printf(عدم, log, "%s: Error resolving %s: %s.", msg, text, gai_strerror(rc));
		}
#ܐܚܪܢܐ
		ܐܢ(rc == WSAHOST_NOT_FOUND){
			log__printf(عدم, log, "%s: Error resolving %s.", msg, text);
		}
#endif
		ܦܢܐ MOSQ_ERR_INVAL;
	}
	ܦܢܐ MOSQ_ERR_SUCCESS;
}


ساكن فراغ config__init_reload(بنية mosquitto__config *config)
{
	/* Set defaults */
	لكل(صحيح i=0; i<config->listener_count; i++){
		listener__set_defaults(&config->listeners[i]);
	}

	config->local_only = true;
	config->allow_duplicate_messages = true;

	mosquitto_FREE(config->security_options.acl_data.acl_file);
	mosquitto_FREE(config->security_options.password_data.password_file);
	mosquitto_FREE(config->security_options.psk_file);

	config->security_options.allow_anonymous = -1;
	config->security_options.allow_zero_length_clientid = true;
	config->security_options.auto_id_prefix = عدم;
	config->security_options.auto_id_prefix_len = 0;

	config->autosave_interval = 1800;
	config->autosave_on_changes = false;

	mosquitto_FREE(config->clientid_prefixes);

	config->connection_messages = true;
	config->clientid_prefixes = عدم;
	config->per_listener_settings = false;
	ܐܢ(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = عدم;
	}
	mosquitto_FREE(config->log_file);

#ܐܢ defined(WIN32) || defined(__CYGWIN__)
	ܐܢ(service_handle){
		/* This is running as a Windows service. Default to no logging. Using
		 * stdout/stderr is forbidden because the first clients to connect will
		 * get log information sent to them for some reason. */
		config->log_dest = MQTT3_LOG_NONE;
	}ܐܚܪܢܐ{
		config->log_dest = MQTT3_LOG_STDERR;
	}
#ܐܚܪܢܐ
	config->log_facility = LOG_DAEMON;
	config->log_dest = MQTT3_LOG_STDERR | MQTT3_LOG_DLT;
	ܐܢ(db.quiet){
		config->log_type = 0;
	}ܐܚܪܢܐ ܐܢ(db.verbose){
		config->log_type = UINT_MAX;
	}ܐܚܪܢܐ{
		config->log_type = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;
	}
#endif
	config->log_timestamp = true;
	mosquitto_FREE(config->log_timestamp_format);
	config->global_max_clients = -1;
	config->global_max_connections = -1;
	config->log_timestamp_format = عدم;
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


ساكن فراغ config__cleanup_plugin_config(mosquitto_plugin_id_t *plugin)
{
	mosquitto_FREE(plugin->config.path);
	mosquitto_FREE(plugin->config.name);

	ܐܢ(plugin->config.options){
		لكل(صحيح j=0; j<plugin->config.option_count; j++){
			mosquitto_FREE(plugin->config.options[j].key);
			mosquitto_FREE(plugin->config.options[j].value);
		}
		mosquitto_FREE(plugin->config.options);
		plugin->config.option_count = 0;
	}
	mosquitto_FREE(plugin->config.security_options);
	mosquitto_FREE(plugin);
}


ساكن فراغ config__cleanup_plugins(فراغ)
{
	لكل(صحيح i=0; i<db.plugin_count; i++){
		config__cleanup_plugin_config(db.plugins[i]);
	}
	mosquitto_FREE(db.plugins);
}


فراغ config__init(بنية mosquitto__config *config)
{
	memset(config, 0, حجم(بنية mosquitto__config));
	config__init_reload(config);

	config->daemon = false;
}


فراغ config__cleanup(بنية mosquitto__config *config)
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
	ܐܢ(config->listeners){
		لكل(صحيح i=0; i<config->listener_count; i++){
			mosquitto_FREE(config->listeners[i].host);
			mosquitto_FREE(config->listeners[i].bind_interface);
			mosquitto_FREE(config->listeners[i].mount_point);
			mosquitto_FREE(config->listeners[i].socks);
			ܐܢ(config->listeners[i].security_options){
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
#ܐܢ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			ܐܢ(!config->listeners[i].ws_context) /* libwebsockets frees its own SSL_CTX */
#endif
			{
				SSL_CTX_free(config->listeners[i].ssl_ctx);
				config->listeners[i].ssl_ctx = عدم;
			}
#endif
#ܐܢ defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
			mosquitto_FREE(config->listeners[i].http_dir);
#endif
#ifdef WITH_WEBSOCKETS
			لكل(صحيح j=0; j<config->listeners[i].ws_origin_count; j++){
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
	ܐܢ(config->bridges){
		لكل(صحيح i=0; i<config->bridge_count; i++){
			config__bridge_cleanup(config->bridges[i]);
		}
		mosquitto_FREE(config->bridges);
	}
#endif
	config__cleanup_plugins();

	ܐܢ(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = عدم;
	}
	ܐܢ(config->log_file){
		mosquitto_FREE(config->log_file);
		config->log_file = عدم;
	}
}

#ifdef WITH_BRIDGE


فراغ config__bridge_cleanup(بنية mosquitto__bridge *bridge)
{
	ܐܢ(bridge == عدم){
		ܦܢܐ;
	}

	mosquitto_FREE(bridge->name);
	ܐܢ(bridge->addresses){
		لكل(صحيح i=0; i<bridge->address_count; i++){
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
	ܐܢ(bridge->topics){
		بنية mosquitto__bridge_topic *cur_topic, *topic_tmp;

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


ساكن فراغ print_version(فراغ)
{
	printf("mosquitto %s\n", VERSION);
	printf("Copyright © 2025 Roger Light.\n");
	printf("License EPL-2.0 OR BSD-3-Clause.\n");
}


ساكن فراغ print_usage(فراغ)
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


صحيح config__parse_args(بنية mosquitto__config *config, صحيح argc, حرف *argv[])
{
	صحيح i;
	صحيح port_tmp;

	لكل(i=1; i<argc; i++){
		ܐܢ(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--config-file")){
			ܐܢ(i<argc-1){
				db.config_file = argv[i+1];

				ܐܢ(config__read(config, false)){
					ܦܢܐ MOSQ_ERR_INVAL;
				}
			}ܐܚܪܢܐ{
				log__printf(عدم, MOSQ_LOG_ERR, "Error: -c argument given, but no config file specified.");
				ܦܢܐ MOSQ_ERR_INVAL;
			}
			i++;
		}ܐܚܪܢܐ ܐܢ(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--daemon")){
			config->daemon = true;
		}ܐܚܪܢܐ ܐܢ(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
			print_usage();
			ܦܢܐ MOSQ_ERR_UNKNOWN;
		}ܐܚܪܢܐ ܐܢ(!strcmp(argv[i], "--version")){
			print_version();
			ܦܢܐ MOSQ_ERR_UNKNOWN;
		}ܐܚܪܢܐ ܐܢ(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port")){
			ܐܢ(i<argc-1){
				port_tmp = atoi(argv[i+1]);
				ܐܢ(port_tmp<1 || port_tmp>UINT16_MAX){
					log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid port specified (%d).", port_tmp);
					ܦܢܐ MOSQ_ERR_INVAL;
				}ܐܚܪܢܐ{
					ܐܢ(config->cmd_port_count == CMD_PORT_LIMIT){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Only %d ports can be specified on the command line.", CMD_PORT_LIMIT);
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					config->cmd_port[config->cmd_port_count] = (uint16_t)port_tmp;
					config->cmd_port_count++;
				}
			}ܐܚܪܢܐ{
				log__printf(عدم, MOSQ_LOG_ERR, "Error: -p argument given, but no port specified.");
				ܦܢܐ MOSQ_ERR_INVAL;
			}
			i++;
		}ܐܚܪܢܐ ܐܢ(!strcmp(argv[i], "--tls-keylog")){
#ifdef WITH_TLS
			ܐܢ(i<argc-1){
				db.tls_keylog = mosquitto_strdup(argv[i+1]);
				ܐܢ(db.tls_keylog == عدم){
					log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
					ܦܢܐ MOSQ_ERR_NOMEM;
				}
			}ܐܚܪܢܐ{
				log__printf(عدم, MOSQ_LOG_ERR, "Error: --tls-keylog argument given, but no file specified.");
				ܦܢܐ MOSQ_ERR_INVAL;
			}
			i++;
#ܐܚܪܢܐ
			fprintf(stderr, "Error: TLS support not available so --tls-keylog is not available.\n");
			ܦܢܐ MOSQ_ERR_INVAL;
#endif
		}ܐܚܪܢܐ ܐܢ(!strcmp(argv[i], "-q") || !strcmp(argv[i], "--quiet")){
			db.quiet = true;
		}ܐܚܪܢܐ ܐܢ(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")){
			db.verbose = true;
		}ܐܚܪܢܐ ܐܢ(!strcmp(argv[i], "--test-config")){
			config->test_configuration = true;
		}ܐܚܪܢܐ{
			fprintf(stderr, "Error: Unknown option '%s'.\n", argv[i]);
			print_usage();
			ܦܢܐ MOSQ_ERR_INVAL;
		}
	}

	/* Default to drop to mosquitto user if we are privileged and no user specified. */
	ܐܢ(!config->user){
		config->user = mosquitto_strdup("mosquitto");
		ܐܢ(config->user == عدم){
			ܦܢܐ MOSQ_ERR_NOMEM;
		}
	}
	ܐܢ(db.quiet){
		config->log_type = 0;
	}ܐܚܪܢܐ ܐܢ(db.verbose){
		config->log_type = UINT_MAX;
	}

	ܐܢ(getenv("MOSQUITTO_PERSISTENCE_LOCATION")){
		mosquitto_FREE(config->persistence_location);
		config->persistence_location = mosquitto_strdup(getenv("MOSQUITTO_PERSISTENCE_LOCATION"));
		ܐܢ(!config->persistence_location){
			ܦܢܐ MOSQ_ERR_NOMEM;
		}
	}
	ܦܢܐ config__check(config);
}


ساكن فراغ config__copy(بنية mosquitto__config *src, بنية mosquitto__config *dest)
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
	dest->security_options.plugin_count = src->security_options.plugin_count;
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

#ܐܢ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	dest->websockets_log_level = src->websockets_log_level;
#endif

#ifdef WITH_BRIDGE
	لكل(صحيح i=0; i<dest->bridge_count; i++){
		ܐܢ(dest->bridges[i]){
			config__bridge_cleanup(dest->bridges[i]);
		}
	}
	mosquitto_FREE(dest->bridges);
	dest->bridges = src->bridges;
	dest->bridge_count = src->bridge_count;
#endif
}


صحيح config__read(بنية mosquitto__config *config, bool reload)
{
	صحيح rc = MOSQ_ERR_SUCCESS;
	بنية config_recurse cr;
	صحيح lineno = 0;
#ifdef WITH_PERSISTENCE
	size_t len;
#endif
	بنية mosquitto__config config_reload;
	صحيح i;

	ܐܢ(reload){
		memset(&config_reload, 0, حجم(بنية mosquitto__config));
	}

	cr.log_dest = MQTT3_LOG_NONE;
	cr.log_dest_set = 0;
	cr.log_type = MOSQ_LOG_NONE;
	cr.log_type_set = 0;

	ܐܢ(!db.config_file){
		ܦܢܐ 0;
	}

	ܐܢ(reload){
		/* Re-initialise appropriate config vars to default for reload. */
		config__init_reload(&config_reload);
		config_reload.listeners = config->listeners;
		config_reload.listener_count = config->listener_count;
		cur_security_options = عدم;
		rc = config__read_file(&config_reload, reload, db.config_file, &cr, 0, &lineno);
	}ܐܚܪܢܐ{
		rc = config__read_file(config, reload, db.config_file, &cr, 0, &lineno);
	}
	ܐܢ(rc){
		ܐܢ(lineno > 0){
			log__printf(عدم, MOSQ_LOG_ERR, "Error found at %s:%d.", db.config_file, lineno);
		}
		ܦܢܐ rc;
	}

	ܐܢ(reload){
		config__copy(&config_reload, config);
	}

	/* If auth/access options are set and allow_anonymous not explicitly set, disallow anon. */
	ܐܢ(config->local_only == false){
		ܐܢ(config->per_listener_settings){
			لكل(i=0; i<config->listener_count; i++){
				/* Default option if no security options set */
				ܐܢ(config->listeners[i].security_options->allow_anonymous == -1){
					config->listeners[i].security_options->allow_anonymous = false;
				}
			}
		}ܐܚܪܢܐ{
			ܐܢ(config->security_options.allow_anonymous == -1){
				config->security_options.allow_anonymous = false;
			}
		}
	}
#ifdef WITH_PERSISTENCE
	ܐܢ(config->persistence){
		ܐܢ(!config->persistence_file){
			config->persistence_file = mosquitto_strdup("mosquitto.db");
			ܐܢ(!config->persistence_file){
				ܦܢܐ MOSQ_ERR_NOMEM;
			}
		}
		mosquitto_FREE(config->persistence_filepath);
		ܐܢ(config->persistence_location && strlen(config->persistence_location)){
			len = strlen(config->persistence_location) + strlen(config->persistence_file) + 2;
			config->persistence_filepath = mosquitto_malloc(len);
			ܐܢ(!config->persistence_filepath){
				ܦܢܐ MOSQ_ERR_NOMEM;
			}
#ifdef WIN32
			snprintf(config->persistence_filepath, len, "%s\\%s", config->persistence_location, config->persistence_file);
#ܐܚܪܢܐ
			snprintf(config->persistence_filepath, len, "%s/%s", config->persistence_location, config->persistence_file);
#endif
		}ܐܚܪܢܐ{
			config->persistence_filepath = mosquitto_strdup(config->persistence_file);
			ܐܢ(!config->persistence_filepath){
				ܦܢܐ MOSQ_ERR_NOMEM;
			}
		}
	}
#endif
	/* Default to drop to mosquitto user if no other user specified. This must
	 * remain here even though it is covered in config__parse_args() because this
	 * function may be called on its own. */
	ܐܢ(!config->user){
		config->user = mosquitto_strdup("mosquitto");
	}

#ifdef WITH_BRIDGE
	لكل(i=0; i<config->bridge_count; i++){
		ܐܢ(!config->bridges[i]->name){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: bridge name not defined.");
			ܦܢܐ MOSQ_ERR_INVAL;
		}
		ܐܢ(config->bridges[i]->addresses  == 0){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no remote addresses defined.");
			ܦܢܐ MOSQ_ERR_INVAL;
		}
		ܐܢ(config->bridges[i]->topic_count == 0){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no topics defined.");
			ܦܢܐ MOSQ_ERR_INVAL;
		}
#ifdef FINAL_WITH_TLS_PSK
		ܐܢ(config->bridges[i]->tls_psk && !config->bridges[i]->tls_psk_identity){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_identity.");
			ܦܢܐ MOSQ_ERR_INVAL;
		}
		ܐܢ(config->bridges[i]->tls_psk_identity && !config->bridges[i]->tls_psk){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_psk.");
			ܦܢܐ MOSQ_ERR_INVAL;
		}
#endif
	}
#endif

	ܐܢ(cr.log_dest_set){
		config->log_dest = cr.log_dest;
	}
	ܐܢ(db.quiet){
		config->log_type = 0;
	}ܐܚܪܢܐ ܐܢ(db.verbose){
		config->log_type = UINT_MAX;
	}ܐܚܪܢܐ ܐܢ(cr.log_type_set){
		config->log_type = cr.log_type;
	}
#ifdef WITH_BRIDGE
	ܦܢܐ config__check_bridges(config);
#ܐܚܪܢܐ
	ܦܢܐ MOSQ_ERR_SUCCESS;
#endif
}


ساكن mosquitto_plugin_id_t *config__plugin_find(ثابت حرف *name)
{
	ܐܢ(db.plugins && name){
		لكل(صحيح i=0; i<db.plugin_count; i++){
			ܐܢ(db.plugins[i]->config.name && !strcmp(db.plugins[i]->config.name, name)){
				ܦܢܐ db.plugins[i];
			}
		}
	}
	ܦܢܐ عدم;
}


ساكن mosquitto_plugin_id_t *config__plugin_load(ثابت حرف *name, ثابت حرف *path)
{
	mosquitto_plugin_id_t *plugin = عدم;
	mosquitto_plugin_id_t **plugins = عدم;

	ܐܢ(name && config__plugin_find(name)){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Duplicate plugin name '%s'.", name);
		ܦܢܐ عدم;
	}

	ܐܢ(!path || !strcmp(path, "")){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Missing plugin path for plugin name '%s'.", name);
		ܦܢܐ عدم;
	}

	plugin = mosquitto_calloc(1, حجم(mosquitto_plugin_id_t));
	ܐܢ(!plugin){
		اذهب error;
	}

	ܐܢ(name){
		plugin->config.name = mosquitto_strdup(name);
	}
	plugin->config.path = mosquitto_strdup(path);
	ܐܢ((name && !plugin->config.name) || !plugin->config.path){
		اذهب error;
	}
	plugin->config.options = عدم;
	plugin->config.option_count = 0;
	plugin->config.deny_special_chars = true;

	/* Add to db list */
	plugins = mosquitto_realloc(db.plugins, (size_t)(db.plugin_count+1)*حجم(بنية mosquitto__plugin_config *));
	ܐܢ(!plugins){
		اذهب error;
	}

	plugins[db.plugin_count] = plugin;
	db.plugins = plugins;
	db.plugin_count++;

	ܦܢܐ plugin;
error:
	log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
	ܐܢ(plugin){
		mosquitto_FREE(plugin->config.name);
		mosquitto_FREE(plugin->config.path);
	}
	mosquitto_FREE(plugin);
	ܦܢܐ عدم;
}


صحيح config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, بنية mosquitto__security_options *security_options)
{
	بنية mosquitto__security_options **new_options;
	mosquitto_plugin_id_t **new_plugins;

	new_options = mosquitto_realloc(plugin->config.security_options, (size_t)(plugin->config.security_option_count+1)*حجم(بنية mosquitto__security_options *));
	new_plugins = mosquitto_realloc(security_options->plugins, (size_t)(security_options->plugin_count+1)*حجم(mosquitto_plugin_id_t *));

	ܐܢ(!new_options || !new_plugins){
		mosquitto_FREE(new_options);
		mosquitto_FREE(new_plugins);
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		ܦܢܐ MOSQ_ERR_NOMEM;
	}

	new_options[plugin->config.security_option_count] = security_options;
	plugin->config.security_options = new_options;
	plugin->config.security_option_count++;

	new_plugins[security_options->plugin_count] = plugin;
	security_options->plugins = new_plugins;
	security_options->plugin_count++;

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


ساكن صحيح config__read_file_core(بنية mosquitto__config *config, bool reload, بنية config_recurse *cr, صحيح level, صحيح *lineno, FILE *fptr, حرف **buf, صحيح *buflen)
{
	صحيح rc;
	حرف *token;
	صحيح tmp_int;
	حرف *saveptr = عدم;
#ifdef WITH_BRIDGE
	حرف *tmp_char;
	بنية mosquitto__bridge *cur_bridge = عدم;
#endif
	mosquitto_plugin_id_t *cur_plugin = عدم;

	حرف *key;
	بنية mosquitto__listener *cur_listener = عدم;
	صحيح i;
	صحيح lineno_ext = 0;
	size_t prefix_len;
	size_t slen;
#ifdef WITH_WEBSOCKETS
	حرف **ws_origins = عدم;
#endif

	*lineno = 0;

	ܟܡܐ(mosquitto_fgets(buf, buflen, fptr)){
		(*lineno)++;
		ܐܢ((*buf)[0] != '#' && (*buf)[0] != 10 && (*buf)[0] != 13){
			slen = strlen(*buf);
			ܐܢ(slen == 0){
				استمر;
			}
			ܟܡܐ((*buf)[slen-1] == 10 || (*buf)[slen-1] == 13){
				(*buf)[slen-1] = 0;
				slen = strlen(*buf);
				ܐܢ(slen == 0){
					استمر;
				}
			}
			token = strtok_r((*buf), " ", &saveptr);
			ܐܢ(token){
				ܐܢ(!strcmp(token, "accept_protocol_versions")){
					REQUIRE_NON_DEFAULT_LISTENER(token);
					cur_listener->disable_protocol_v3 = true;
					cur_listener->disable_protocol_v4 = true;
					cur_listener->disable_protocol_v5 = true;
					ܐܢ(saveptr == عدم){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", "accept_protocol_versions");
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					token = strtok_r(saveptr, ", \t", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "accept_protocol_versions");

					ܟܡܐ(token){
						ܐܢ(!strcmp(token, "3")){
							cur_listener->disable_protocol_v3 = false;
						}ܐܚܪܢܐ ܐܢ(!strcmp(token, "4")){
							cur_listener->disable_protocol_v4 = false;
						}ܐܚܪܢܐ ܐܢ(!strcmp(token, "5")){
							cur_listener->disable_protocol_v5 = false;
						}

						token = strtok_r(عدم, ", \t", &saveptr);
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "acl_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);

					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->acl_data.acl_file);
					ܐܢ(conf__parse_string(&token, "acl_file", &cur_security_options->acl_data.acl_file, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "address") || !strcmp(token, "addresses")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ܐܢ(cur_bridge->addresses){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge configuration, 'address' only allowed once.");
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܟܡܐ((token = strtok_r(عدم, " ", &saveptr))){
						ܐܢ(token[0] == '#'){
							اكسر;
						}
						بنية bridge_address *new_addresses = mosquitto_realloc(cur_bridge->addresses, حجم(بنية bridge_address)*(size_t)(cur_bridge->address_count+1));
						ܐܢ(!new_addresses){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
							ܦܢܐ MOSQ_ERR_NOMEM;
						}
						cur_bridge->address_count++;
						cur_bridge->addresses = new_addresses;
						memset(&cur_bridge->addresses[cur_bridge->address_count-1], 0, حجم(بنية bridge_address));
						cur_bridge->addresses[cur_bridge->address_count-1].address = mosquitto_strdup(token);
						ܐܢ(!cur_bridge->addresses[cur_bridge->address_count-1].address){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
							ܦܢܐ MOSQ_ERR_NOMEM;
						}
					}
					لكل(i=0; i<cur_bridge->address_count; i++){
						/* cur_bridge->addresses[i].address is now
						 * "address[:port]". If address is an IPv6 address,
						 * then port is required. We must check for the :
						 * backwards. */
						tmp_char = strrchr(cur_bridge->addresses[i].address, ':');
						ܐܢ(tmp_char){
							/* Remove ':', so cur_bridge->addresses[i].address
							 * now just looks like the address. */
							tmp_char[0] = '\0';

							/* The remainder of the string */
							tmp_int = atoi(&tmp_char[1]);
							ܐܢ(tmp_int < 1 || tmp_int > UINT16_MAX){
								log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge port value (%d).", tmp_int);
								ܦܢܐ MOSQ_ERR_INVAL;
							}
							cur_bridge->addresses[i].port = (uint16_t)tmp_int;
						}ܐܚܪܢܐ{
							cur_bridge->addresses[i].port = 1883;
						}
						conf__attempt_resolve(cur_bridge->addresses[i].address, "bridge address", MOSQ_LOG_WARNING, "Warning");
					}
					ܐܢ(cur_bridge->address_count == 0){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Empty address value in configuration.");
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "allow_anonymous")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					PER_LISTENER_ALTERNATIVE(token, "listener_allow_anonymous");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ܐܢ(conf__parse_bool(&token, "allow_anonymous", (bool *)&cur_security_options->allow_anonymous, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "allow_duplicate_messages")){
					OPTION_DEPRECATED(token, "The behaviour will default to true.");
					ܐܢ(conf__parse_bool(&token, "allow_duplicate_messages", &config->allow_duplicate_messages, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "allow_zero_length_clientid")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ܐܢ(conf__parse_bool(&token, "allow_zero_length_clientid", &cur_security_options->allow_zero_length_clientid, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strncmp(token, "auth_opt_", strlen("auth_opt_")) || !strncmp(token, "plugin_opt_", strlen("plugin_opt_"))){
					ܐܢ(reload){
						استمر;        /* Auth plugin not currently valid for reloading. */

					}
					REQUIRE_PLUGIN(token);

					ܐܢ(!strncmp(token, "auth_opt_", strlen("auth_opt_"))){
						prefix_len = strlen("auth_opt_");
					}ܐܚܪܢܐ{
						prefix_len = strlen("plugin_opt_");
					}
					ܐܢ(strlen(token) < prefix_len + 3){
						/* auth_opt_ == 9, + one digit key == 10, + one space == 11, + one value == 12 */
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'plugin_opt_' config option.");
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					key = mosquitto_strdup(&token[prefix_len]);
					ܐܢ(!key){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
						ܦܢܐ MOSQ_ERR_NOMEM;
					}ܐܚܪܢܐ ܐܢ(STREMPTY(key)){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Empty 'plugin_opt_' config option.");
						mosquitto_FREE(key);
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					token = saveptr;
					ܐܢ(token && token[0]){
						ܟܡܐ(token[0] == ' ' || token[0] == '\t'){
							token++;
						}
						بنية mosquitto_opt *new_options = mosquitto_realloc(cur_plugin->config.options, (size_t)(cur_plugin->config.option_count+1)*حجم(بنية mosquitto_opt));
						ܐܢ(!new_options){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
							mosquitto_FREE(key);
							ܦܢܐ MOSQ_ERR_NOMEM;
						}
						cur_plugin->config.option_count++;
						cur_plugin->config.options = new_options;
						cur_plugin->config.options[cur_plugin->config.option_count-1].key = key;
						cur_plugin->config.options[cur_plugin->config.option_count-1].value = mosquitto_strdup(token);
						ܐܢ(!cur_plugin->config.options[cur_plugin->config.option_count-1].value){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
							ܦܢܐ MOSQ_ERR_NOMEM;
						}
					}ܐܚܪܢܐ{
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", key);
						mosquitto_FREE(key);
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "auth_plugin") || !strcmp(token, "plugin") || !strcmp(token, "global_plugin")){
					ܐܢ(reload){
						استمر;        /* plugin not currently valid for reloading. */
					}
					ܐܢ(!strcmp(token, "global_plugin")){
						cur_security_options = &db.config->security_options;
					}ܐܚܪܢܐ{
						REQUIRE_LISTENER_IF_PER_LISTENER(token);
						conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					}

					cur_plugin = config__plugin_load(عدم, saveptr);
					ܐܢ(cur_plugin == عدم){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(config__plugin_add_secopt(cur_plugin, cur_security_options)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "auth_plugin_deny_special_chars")){
					ܐܢ(reload){
						استمر;        /* Auth plugin not currently valid for reloading. */
					}
					ܐܢ(!cur_plugin){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: An auth_plugin_deny_special_chars option exists in the config file without a plugin.");
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(conf__parse_bool(&token, "auth_plugin_deny_special_chars", &cur_plugin->config.deny_special_chars, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "plugin_load")){
					حرف *name = عدم;

					ܐܢ(reload){
						استمر;        /* plugin not currently valid for reloading. */

					}
					name = strtok_r(عدم, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(name, "plugin_load");

					cur_plugin = config__plugin_load(name, saveptr);
					ܐܢ(cur_plugin == عدم){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "plugin_use")){
					حرف *name = عدم;

					ܐܢ(reload){
						استمر;        /* plugin not currently valid for reloading. */
					}
					REQUIRE_NON_DEFAULT_LISTENER(token);

					ܐܢ(conf__parse_string(&token, "plugin_use", &name, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					cur_plugin = config__plugin_find(name);
					ܐܢ(!cur_plugin){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Plugin '%s' not previously loaded.", name);
						mosquitto_FREE(name);
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(name);
					ܐܢ(config__plugin_add_secopt(cur_plugin, cur_listener->security_options)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "auto_id_prefix")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					OPTION_DEPRECATED(token, "Please use 'listener_auto_id_prefix' instead.");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ܐܢ(conf__parse_string(&token, "auto_id_prefix", &cur_security_options->auto_id_prefix, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(cur_security_options->auto_id_prefix){
						ܐܢ(strlen(cur_security_options->auto_id_prefix) > 50){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: auto_id_prefix length must be <= 50.");
							ܦܢܐ MOSQ_ERR_INVAL;
						}
						cur_security_options->auto_id_prefix_len = (uint16_t)strlen(cur_security_options->auto_id_prefix);
					}ܐܚܪܢܐ{
						cur_security_options->auto_id_prefix_len = 0;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "autosave_interval")){
					ܐܢ(conf__parse_int(&token, "autosave_interval", &config->autosave_interval, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(config->autosave_interval < 0){
						config->autosave_interval = 0;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "autosave_on_changes")){
					ܐܢ(conf__parse_bool(&token, "autosave_on_changes", &config->autosave_on_changes, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "bind_address")){
					OPTION_DEPRECATED(token, "");
					config->local_only = false;
					ܐܢ(reload){
						استمر;        /* Rebinding listeners not valid during reloading. */

					}
					ܐܢ(config__create_default_listener(config, token)){
						ܦܢܐ MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					ܐܢ(conf__parse_string(&token, "default listener bind_address", &config->default_listener->host, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(conf__attempt_resolve(config->default_listener->host, "bind_address", MOSQ_LOG_ERR, "Error")){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "bind_interface")){
#ifdef SO_BINDTODEVICE
					ܐܢ(reload){
						استمر;        /* Rebinding listeners not valid during reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ܐܢ(conf__parse_string(&token, "bind_interface", &cur_listener->bind_interface, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_ERR, "Error: bind_interface specified but socket option not available.");
					ܦܢܐ MOSQ_ERR_INVAL;
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "bridge_attempt_unsubscribe")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_bool(&token, "bridge_attempt_unsubscribe", &cur_bridge->attempt_unsubscribe, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "bridge_cafile")){
#ܐܢ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					ܐܢ(conf__parse_string(&token, "bridge_cafile", &cur_bridge->tls_cafile, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "bridge_alpn")){
#ܐܢ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_string(&token, "bridge_alpn", &cur_bridge->tls_alpn, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "bridge_ciphers")){
#ܐܢ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_string(&token, "bridge_ciphers", &cur_bridge->tls_ciphers, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "bridge_ciphers_tls1.3")){
#ܐܢ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_string(&token, "bridge_ciphers_tls1.3", &cur_bridge->tls_13_ciphers, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "bridge_bind_address")){
#ܐܢ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_string(&token, "bridge_bind_address", &cur_bridge->bind_address, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "bridge_capath")){
#ܐܢ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					ܐܢ(conf__parse_string(&token, "bridge_capath", &cur_bridge->tls_capath, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "bridge_certfile")){
#ܐܢ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					ܐܢ(conf__parse_string(&token, "bridge_certfile", &cur_bridge->tls_certfile, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "bridge_identity")){
#ܐܢ defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					ܐܢ(conf__parse_string(&token, "bridge_identity", &cur_bridge->tls_psk_identity, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "bridge_insecure")){
#ܐܢ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_bool(&token, "bridge_insecure", &cur_bridge->tls_insecure, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(cur_bridge->tls_insecure){
						log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge '%s' using insecure mode.", cur_bridge->name);
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "bridge_require_ocsp")){
#ܐܢ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_bool(&token, "bridge_require_ocsp", &cur_bridge->tls_ocsp_required, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "bridge_max_packet_size")){
#ܐܢ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_int(&token, "bridge_max_packet_size", &tmp_int, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(tmp_int < 0){
						tmp_int = 0;
					}
					cur_bridge->maximum_packet_size = (uint32_t)tmp_int;
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "bridge_max_topic_alias")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_int(&token, "bridge_max_topic_alias", &tmp_int, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}

					ܐܢ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: bridge_max_topic_alias must be > 0 and <= 65535.");
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					cur_bridge->max_topic_alias = (uint16_t)tmp_int;
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "bridge_outgoing_retain")){
#ܐܢ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_bool(&token, "bridge_outgoing_retain", &cur_bridge->outgoing_retain, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "bridge_keyfile")){
#ܐܢ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					mosquitto_FREE(cur_bridge->tls_keyfile);
					ܐܢ(conf__parse_string(&token, "bridge_keyfile", &cur_bridge->tls_keyfile, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "bridge_protocol_version")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(عدم, "", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_protocol_version");

					ܐܢ(!strcmp(token, "mqttv31")){
						cur_bridge->protocol_version = mosq_p_mqtt31;
					}ܐܚܪܢܐ ܐܢ(!strcmp(token, "mqttv311")){
						cur_bridge->protocol_version = mosq_p_mqtt311;
					}ܐܚܪܢܐ ܐܢ(!strcmp(token, "mqttv50")){
						cur_bridge->protocol_version = mosq_p_mqtt5;
					}ܐܚܪܢܐ{
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'bridge_protocol_version' value (%s).", token);
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "bridge_psk")){
#ܐܢ defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					ܐܢ(conf__parse_string(&token, "bridge_psk", &cur_bridge->tls_psk, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "bridge_receive_maximum")){
#ܐܢ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_int(&token, "bridge_receive_maximum", &tmp_int, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(tmp_int <= 0){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be greater than 0.");
						ܦܢܐ MOSQ_ERR_INVAL;
					}ܐܚܪܢܐ ܐܢ((uint64_t)tmp_int > (uint64_t)UINT16_MAX){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be lower than %u.", UINT16_MAX);
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					cur_bridge->receive_maximum = (uint16_t)tmp_int;
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "bridge_reload_type")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(عدم, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_reload_type");

					ܐܢ(!strcmp(token, "lazy")){
						cur_bridge->reload_type = brt_lazy;
					}ܐܚܪܢܐ ܐܢ(!strcmp(token, "immediate")){
						cur_bridge->reload_type = brt_immediate;
					}ܐܚܪܢܐ{
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'bridge_reload_type' value in configuration (%s).", token);
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "bridge_session_expiry_interval")){
#ܐܢ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_int(&token, "bridge_session_expiry_interval", &tmp_int, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(tmp_int < 0){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must not be negative.");
						ܦܢܐ MOSQ_ERR_INVAL;
					}ܐܚܪܢܐ ܐܢ((uint64_t)tmp_int > (uint64_t)MQTT_SESSION_EXPIRY_NEVER){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must be lower than %u.", MQTT_SESSION_EXPIRY_NEVER);
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					cur_bridge->session_expiry_interval = (uint32_t)tmp_int;
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "bridge_tcp_keepalive")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);

					ܐܢ(conf__parse_int(&token, "bridge_tcp_keepalive_idle", &tmp_int, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(tmp_int <= 0){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: invalid TCP keepalive idle value.");
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_idle = (unsigned صحيح)tmp_int;

					ܐܢ(conf__parse_int(&token, "bridge_tcp_keepalive_interval", &tmp_int, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(tmp_int <= 0){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: invalid TCP keepalive interval value.");
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_interval = (unsigned صحيح)tmp_int;

					ܐܢ(conf__parse_int(&token, "bridge_tcp_keepalive_counter", &tmp_int, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(tmp_int <= 0){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: invalid TCP keepalive counter value.");
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_counter = (unsigned صحيح)tmp_int;
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "bridge_tcp_user_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
#ifdef WITH_TCP_USER_TIMEOUT
					ܐܢ(conf__parse_int(&token, "bridge_tcp_user_timeout", &tmp_int, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(tmp_int < 0){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: invalid TCP user timeout value.");
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_user_timeout = tmp_int;
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge TCP user timeout support not available.");
#endif
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "bridge_tls_use_os_certs")){
#ܐܢ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_bool(&token, "bridge_tls_use_os_certs", &cur_bridge->tls_use_os_certs, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "bridge_tls_version")){
#ܐܢ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_string(&token, "bridge_tls_version", &cur_bridge->tls_version, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "cafile")){
#ܐܢ defined(WITH_TLS)
					REQUIRE_LISTENER(token);
					ܐܢ(cur_listener->psk_hint){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->cafile);
					ܐܢ(conf__parse_string(&token, "cafile", &cur_listener->cafile, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "capath")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->capath);
					ܐܢ(conf__parse_string(&token, "capath", &cur_listener->capath, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "certfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ܐܢ(cur_listener->psk_hint){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->certfile);
					ܐܢ(conf__parse_string(&token, "certfile", &cur_listener->certfile, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "check_retain_source")){
					ܐܢ(conf__parse_bool(&token, "check_retain_source", &config->check_retain_source, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "ciphers")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers);
					ܐܢ(conf__parse_string(&token, "ciphers", &cur_listener->ciphers, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "ciphers_tls1.3")){
#ܐܢ defined(WITH_TLS) && (!defined(LIBRESSL_VERSION_NUMBER) || LIBRESSL_VERSION_NUMBER > 0x3040000FL)
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers_tls13);
					ܐܢ(conf__parse_string(&token, "ciphers_tls1.3", &cur_listener->ciphers_tls13, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: ciphers_tls1.3 support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "clientid") || !strcmp(token, "remote_clientid")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_string(&token, "bridge remote clientid", &cur_bridge->remote_clientid, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "cleansession")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_bool(&token, "cleansession", &cur_bridge->clean_start, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "local_cleansession")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_bool(&token, "local_cleansession", (bool *)&cur_bridge->clean_start_local, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "clientid_prefixes")){
					OPTION_DEPRECATED(token, "");
					mosquitto_FREE(config->clientid_prefixes);
					ܐܢ(conf__parse_string(&token, "clientid_prefixes", &config->clientid_prefixes, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "connection")){
#ifdef WITH_BRIDGE
					token = strtok_r(عدم, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "connection");

					/* Check for existing bridge name. */
					لكل(i=0; i<config->bridge_count; i++){
						ܐܢ(!strcmp(config->bridges[i]->name, token)){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Duplicate bridge name '%s'.", token);
							ܦܢܐ MOSQ_ERR_INVAL;
						}
					}

					بنية mosquitto__bridge **bridges_new = mosquitto_realloc(config->bridges, (size_t)(config->bridge_count+1)*حجم(بنية mosquitto__bridge *));
					ܐܢ(!bridges_new){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
						ܦܢܐ MOSQ_ERR_NOMEM;
					}
					config->bridges = bridges_new;
					cur_bridge = mosquitto_malloc(حجم(بنية mosquitto__bridge));
					ܐܢ(!cur_bridge){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
						ܦܢܐ MOSQ_ERR_NOMEM;
					}

					config->bridge_count++;
					config->bridges[config->bridge_count-1] = cur_bridge;

					memset(cur_bridge, 0, حجم(بنية mosquitto__bridge));
					cur_bridge->name = mosquitto_strdup(token);
					ܐܢ(!cur_bridge->name){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
						ܦܢܐ MOSQ_ERR_NOMEM;
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
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "connection_messages")){
					ܐܢ(conf__parse_bool(&token, token, &config->connection_messages, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "crlfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->crlfile);
					ܐܢ(conf__parse_string(&token, "crlfile", &cur_listener->crlfile, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "dhparamfile")){
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: dhparamfile is no longer required.");
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "disable_client_cert_date_checks")){
#ifdef WITH_TLS
					REQUIRE_LISTENER(token);
					ܐܢ(conf__parse_bool(&token, "disable_client_cert_date_checks", &cur_listener->disable_client_cert_date_checks, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "enable_control_api")){
#ifdef WITH_CONTROL
					ܐܢ(conf__parse_bool(&token, "enable_control_api", &config->enable_control_api, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: $CONTROL support not available (enable_control_api).");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "enable_proxy_protocol")){
#ܐܢ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(عدم, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					ܦܢܐ MOSQ_ERR_INVAL;
#endif
					REQUIRE_LISTENER(token);
					ܐܢ(conf__parse_int(&token, "enable_proxy_protocol", &cur_listener->enable_proxy_protocol, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(cur_listener->enable_proxy_protocol < 1 || cur_listener->enable_proxy_protocol > 2){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: enable_proxy_protocol must be 1 or 2.");
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "global_max_clients")){
					ܐܢ(conf__parse_int(&token, "global_max_clients", &config->global_max_clients, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "global_max_connections")){
					ܐܢ(conf__parse_int(&token, "global_max_connections", &config->global_max_connections, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "http_dir")){
#ܐܢ defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
					ܐܢ(reload){
						استمر;        /* Not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					ܐܢ(conf__parse_string(&token, "http_dir", &cur_listener->http_dir, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ifdef WIN32
					حرف *http_dir_canonical = _fullpath(عدم, cur_listener->http_dir, 0);
					ثابت حرف sep = '\\';
#ܐܚܪܢܐ
					حرف *http_dir_canonical = realpath(cur_listener->http_dir, عدم);
					ثابت حرف sep = '/';
#endif
					ܐܢ(!http_dir_canonical){
						ܦܢܐ MOSQ_ERR_NOMEM;
					}
					size_t http_dir_len = strlen(http_dir_canonical) + حجم(sep) + 1;
					حرف *http_dir_canonical_sep = mosquitto_calloc(http_dir_len, حجم(حرف));
					ܐܢ(!http_dir_canonical_sep){
						حرر(http_dir_canonical);
						ܦܢܐ MOSQ_ERR_NOMEM;
					}
					snprintf(http_dir_canonical_sep, http_dir_len, "%s%c", http_dir_canonical, sep);
					حرر(http_dir_canonical);
					mosquitto_FREE(cur_listener->http_dir);
					cur_listener->http_dir = http_dir_canonical_sep;
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: http_dir support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "idle_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_int(&token, "idle_timeout", &cur_bridge->idle_timeout, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(cur_bridge->idle_timeout < 1){
						log__printf(عدم, MOSQ_LOG_NOTICE, "idle_timeout interval too low, using 1 second.");
						cur_bridge->idle_timeout = 1;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "include_dir")){
					ܐܢ(level == 0){
						حرف **files;
						صحيح file_count;

						/* Only process include_dir from the main config file. */
						token = strtok_r(عدم, "", &saveptr);
						REQUIRE_NON_EMPTY_OPTION(token, "include_dir");

						rc = config__get_dir_files(token, &files, &file_count);
						ܐܢ(rc){
							ܦܢܐ rc;
						}

						لكل(i=0; i<file_count; i++){
							log__printf(عدم, MOSQ_LOG_INFO, "Loading config file '%s'", files[i]);

							rc = config__read_file(config, reload, files[i], cr, level+1, &lineno_ext);
							ܐܢ(rc){
								ܐܢ(lineno_ext > 0){
									log__printf(عدم, MOSQ_LOG_ERR, "Error found at '%s:%d'.", files[i], lineno_ext);
								}
								/* Free happens below */
								اكسر;
							}
						}
						لكل(i=0; i<file_count; i++){
							mosquitto_FREE(files[i]);
						}
						mosquitto_FREE(files);
						ܐܢ(rc){
							ܦܢܐ rc;    /* This returns if config__read_file() fails above */
						}
					}ܐܚܪܢܐ{
						log__printf(عدم, MOSQ_LOG_ERR, "Error: The include_dir option is only valid in the main configuration file.");
						ܦܢܐ 1;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "keepalive_interval")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_int(&token, "keepalive_interval", &tmp_int, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(tmp_int > UINT16_MAX){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Bridge keepalive value too high.");
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(tmp_int < 5){
						log__printf(عدم, MOSQ_LOG_NOTICE, "keepalive interval too low, using 5 seconds.");
						tmp_int = 5;
					}
					cur_bridge->keepalive = (uint16_t)tmp_int;
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "keyfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->keyfile);
					ܐܢ(conf__parse_string(&token, "keyfile", &cur_listener->keyfile, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "listener")){
					config->local_only = false;

					ܐܢ(conf__parse_int(&token, "listener port", &tmp_int, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ifdef WITH_UNIX_SOCKETS
					ܐܢ(tmp_int < 0 || tmp_int > UINT16_MAX){
#ܐܚܪܢܐ
					ܐܢ(tmp_int < 1 || tmp_int > UINT16_MAX){
#endif
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						ܦܢܐ MOSQ_ERR_INVAL;
					}

					/* Look for bind address / unix socket path */
					token = strtok_r(عدم, " ", &saveptr);
					ܐܢ(token != عدم && token[0] == '#'){
						token = عدم;
					}

					ܐܢ(tmp_int == 0 && token == عدم){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: A listener with port 0 must provide a Unix socket path.");
						ܦܢܐ MOSQ_ERR_INVAL;
					}

					ܐܢ(reload){
						/* We reload listeners settings based on port number/unix socket path.
						 * If the port number/unix path doesn't already exist, exit with a complaint. */
						cur_listener = عدم;
#ifdef WITH_UNIX_SOCKETS
						ܐܢ(tmp_int == 0){
							لكل(i=0; i<config->listener_count; i++){
								ܐܢ(config->listeners[i].unix_socket_path != عدم
										&& strcmp(config->listeners[i].unix_socket_path, token) == 0){

									cur_listener = &config->listeners[i];
									اكسر;
								}
							}
						}ܐܚܪܢܐ
#endif
						{
							لكل(i=0; i<config->listener_count; i++){
								ܐܢ(config->listeners[i].port == tmp_int){
									/* Now check we have a matching bind address, if defined */
									ܐܢ(config->listeners[i].host){
										ܐܢ(token && !strcmp(config->listeners[i].host, token)){
											/* They both have a bind address, and they match */
											cur_listener = &config->listeners[i];
											اكسر;
										}
									}ܐܚܪܢܐ{
										ܐܢ(token == عدم){
											/* Neither this config nor the new config have a bind address,
											 * so they match. */
											cur_listener = &config->listeners[i];
											اكسر;
										}
									}
								}
							}
						}
						ܐܢ(!cur_listener){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: It is not currently possible to add/remove listeners when reloading the config file.");
							ܦܢܐ MOSQ_ERR_INVAL;
						}
					}ܐܚܪܢܐ{
						ܐܢ(config__add_listener(config)){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
							ܦܢܐ MOSQ_ERR_NOMEM;
						}
						cur_listener = &config->listeners[config->listener_count-1];
					}

					listener__set_defaults(cur_listener);
					cur_listener->port = (uint16_t)tmp_int;

					mosquitto_FREE(cur_listener->host);

#ifdef WITH_UNIX_SOCKETS
					mosquitto_FREE(cur_listener->unix_socket_path);
#endif

					ܐܢ(token){
#ifdef WITH_UNIX_SOCKETS
						ܐܢ(cur_listener->port == 0){
							cur_listener->unix_socket_path = mosquitto_strdup(token);
						}ܐܚܪܢܐ
#endif
						{
							cur_listener->host = mosquitto_strdup(token);
						}
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "listener_allow_anonymous")){
					REQUIRE_LISTENER(token);
					ܐܢ(conf__parse_bool(&token, "listener_allow_anonymous", (bool *)&cur_listener->security_options->allow_anonymous, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "listener_auto_id_prefix")){
					REQUIRE_LISTENER(token);
					ܐܢ(conf__parse_string(&token, "listener_auto_id_prefix", &cur_listener->security_options->auto_id_prefix, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(cur_listener->security_options->auto_id_prefix){
						ܐܢ(strlen(cur_listener->security_options->auto_id_prefix) > 50){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: 'listener_auto_id_prefix' length must be <= 50.");
							ܦܢܐ MOSQ_ERR_INVAL;
						}
						cur_listener->security_options->auto_id_prefix_len = (uint16_t)strlen(cur_listener->security_options->auto_id_prefix);
					}ܐܚܪܢܐ{
						cur_listener->security_options->auto_id_prefix_len = 0;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "local_clientid")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_string(&token, "bridge local clientd", &cur_bridge->local_clientid, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "local_password")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_string(&token, "bridge local_password", &cur_bridge->local_password, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "local_username")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_string(&token, "bridge local_username", &cur_bridge->local_username, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "log_dest")){
					token = strtok_r(عدم, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_dest");

					cr->log_dest_set = 1;
					ܐܢ(!strcmp(token, "none")){
						cr->log_dest = MQTT3_LOG_NONE;
					}ܐܚܪܢܐ ܐܢ(!strcmp(token, "syslog")){
						cr->log_dest |= MQTT3_LOG_SYSLOG;
					}ܐܚܪܢܐ ܐܢ(!strcmp(token, "stdout")){
						cr->log_dest |= MQTT3_LOG_STDOUT;
					}ܐܚܪܢܐ ܐܢ(!strcmp(token, "stderr")){
						cr->log_dest |= MQTT3_LOG_STDERR;
					}ܐܚܪܢܐ ܐܢ(!strcmp(token, "topic")){
						cr->log_dest |= MQTT3_LOG_TOPIC;
					}ܐܚܪܢܐ ܐܢ(!strcmp(token, "dlt")){
						cr->log_dest |= MQTT3_LOG_DLT;
#ifdef ANDROID
					}ܐܚܪܢܐ ܐܢ(!strcmp(token, "android")){
						cr->log_dest |= MQTT3_LOG_ANDROID;
#endif
					}ܐܚܪܢܐ ܐܢ(!strcmp(token, "file")){
						ܐܢ(config->log_fptr || config->log_file){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Duplicate \"log_dest file\" value.");
							ܦܢܐ MOSQ_ERR_INVAL;
						}
						/* Get remaining string. */
						token = saveptr;
						ܐܢ(token && token[0]){
							ܟܡܐ(token[0] == ' ' || token[0] == '\t'){
								token++;
							}
						}
						/* Duplicate "token" check here saves a log__printf() */
						ܐܢ(token && token[0]){
							config->log_file = mosquitto_strdup(token);
							ܐܢ(!config->log_file){
								log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
								ܦܢܐ MOSQ_ERR_NOMEM;
							}
							cr->log_dest |= MQTT3_LOG_FILE;
						}ܐܚܪܢܐ{
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Empty \"log_dest file\" value in configuration.");
							ܦܢܐ MOSQ_ERR_INVAL;
						}
						cr->log_dest |= MQTT3_LOG_FILE;
					}ܐܚܪܢܐ{
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'log_dest' value (%s).", token);
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܢ defined(WIN32) || defined(__CYGWIN__)
					ܐܢ(service_handle){
						ܐܢ(cr->log_dest == MQTT3_LOG_STDOUT || cr->log_dest == MQTT3_LOG_STDERR){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Cannot log to stdout/stderr when running as a Windows service.");
							ܦܢܐ MOSQ_ERR_INVAL;
						}
					}
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "log_facility")){
#ܐܢ defined(WIN32) || defined(__CYGWIN__)
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: log_facility not supported on Windows.");
#ܐܚܪܢܐ
					ܐܢ(conf__parse_int(&token, "log_facility", &tmp_int, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					بدل(tmp_int){
						حالة 0:
							config->log_facility = LOG_LOCAL0;
							اكسر;
						حالة 1:
							config->log_facility = LOG_LOCAL1;
							اكسر;
						حالة 2:
							config->log_facility = LOG_LOCAL2;
							اكسر;
						حالة 3:
							config->log_facility = LOG_LOCAL3;
							اكسر;
						حالة 4:
							config->log_facility = LOG_LOCAL4;
							اكسر;
						حالة 5:
							config->log_facility = LOG_LOCAL5;
							اكسر;
						حالة 6:
							config->log_facility = LOG_LOCAL6;
							اكسر;
						حالة 7:
							config->log_facility = LOG_LOCAL7;
							اكسر;
						افتراضي:
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'log_facility' value (%d).", tmp_int);
							ܦܢܐ MOSQ_ERR_INVAL;
					}
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "log_timestamp")){
					ܐܢ(conf__parse_bool(&token, token, &config->log_timestamp, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "log_timestamp_format")){
					ܐܢ(conf__parse_string(&token, token, &config->log_timestamp_format, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "log_type")){
					token = strtok_r(عدم, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_type");

					cr->log_type_set = 1;
					ܐܢ(!strcmp(token, "none")){
						cr->log_type = MOSQ_LOG_NONE;
					}ܐܚܪܢܐ ܐܢ(!strcmp(token, "information")){
						cr->log_type |= MOSQ_LOG_INFO;
					}ܐܚܪܢܐ ܐܢ(!strcmp(token, "notice")){
						cr->log_type |= MOSQ_LOG_NOTICE;
					}ܐܚܪܢܐ ܐܢ(!strcmp(token, "warning")){
						cr->log_type |= MOSQ_LOG_WARNING;
					}ܐܚܪܢܐ ܐܢ(!strcmp(token, "error")){
						cr->log_type |= MOSQ_LOG_ERR;
					}ܐܚܪܢܐ ܐܢ(!strcmp(token, "debug")){
						cr->log_type |= MOSQ_LOG_DEBUG;
					}ܐܚܪܢܐ ܐܢ(!strcmp(token, "subscribe")){
						cr->log_type |= MOSQ_LOG_SUBSCRIBE;
					}ܐܚܪܢܐ ܐܢ(!strcmp(token, "unsubscribe")){
						cr->log_type |= MOSQ_LOG_UNSUBSCRIBE;
					}ܐܚܪܢܐ ܐܢ(!strcmp(token, "internal")){
						cr->log_type |= MOSQ_LOG_INTERNAL;
#ifdef WITH_WEBSOCKETS
					}ܐܚܪܢܐ ܐܢ(!strcmp(token, "websockets")){
						cr->log_type |= MOSQ_LOG_WEBSOCKETS;
#endif
					}ܐܚܪܢܐ ܐܢ(!strcmp(token, "all")){
						cr->log_type = MOSQ_LOG_ALL;
					}ܐܚܪܢܐ{
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'log_type' value (%s).", token);
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "max_connections")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ܐܢ(conf__parse_int(&token, token, &cur_listener->max_connections, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(cur_listener->max_connections < 0){
						cur_listener->max_connections = -1;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "maximum_qos") || !strcmp(token, "max_qos")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ܐܢ(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(tmp_int < 0 || tmp_int > 2){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: 'max_qos' must be between 0 and 2 inclusive.");
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					cur_listener->max_qos = (uint8_t)tmp_int;
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "max_inflight_bytes")){
					ܐܢ(conf__parse_int(&token, "max_inflight_bytes", &tmp_int, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_inflight_bytes = (size_t)tmp_int;
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "max_inflight_messages")){
					ܐܢ(conf__parse_int(&token, "max_inflight_messages", &tmp_int, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(tmp_int < 0 || tmp_int == UINT16_MAX){
						tmp_int = 0;
					}ܐܚܪܢܐ ܐܢ(tmp_int > UINT16_MAX){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: 'max_inflight_messages' must be <= 65535.");
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					config->max_inflight_messages = (uint16_t)tmp_int;
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "max_keepalive")){
					ܐܢ(conf__parse_int(&token, "max_keepalive", &tmp_int, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'max_keepalive' value (%d).", tmp_int);
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					config->max_keepalive = (uint16_t)tmp_int;
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "max_packet_size")){
					ܐܢ(conf__parse_int(&token, "max_packet_size", &tmp_int, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(tmp_int < 20){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: 'max_packet_size' must be >= 20.");
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					config->max_packet_size = (uint32_t)tmp_int;
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "max_queued_bytes")){
					ܐܢ(conf__parse_int(&token, "max_queued_bytes", &tmp_int, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_bytes = (size_t)tmp_int;
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "max_queued_messages")){
					ܐܢ(conf__parse_int(&token, "max_queued_messages", &tmp_int, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_messages = tmp_int;
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "memory_limit")){
					ssize_t lim;
					ܐܢ(conf__parse_ssize_t(&token, "memory_limit", &lim, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(lim < 0){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'memory_limit' value (%ld).", lim);
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					mosquitto_memory_set_limit((size_t)lim);
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "message_size_limit")){
					log__printf(عدم, MOSQ_LOG_NOTICE, "Note: It is recommended to replace `message_size_limit` with `max_packet_size`.");
					ܐܢ(conf__parse_int(&token, "message_size_limit", (صحيح *)&config->message_size_limit, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(config->message_size_limit > MQTT_MAX_PAYLOAD){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'message_size_limit' value (%u).", config->message_size_limit);
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "mount_point")){
					REQUIRE_LISTENER(token);
					mosquitto_FREE(cur_listener->mount_point);
					ܐܢ(conf__parse_string(&token, "mount_point", &cur_listener->mount_point, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(mosquitto_pub_topic_check(cur_listener->mount_point) != MOSQ_ERR_SUCCESS){
						log__printf(عدم, MOSQ_LOG_ERR,
								"Error: Invalid 'mount_point' value (%s). Does it contain a wildcard character?",
								cur_listener->mount_point);
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "notifications")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_bool(&token, "notifications", &cur_bridge->notifications, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "notifications_local_only")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_bool(&token, "notifications_local_only", &cur_bridge->notifications_local_only, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "notification_topic")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_string(&token, "notification_topic", &cur_bridge->notification_topic, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "password") || !strcmp(token, "remote_password")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_string(&token, "bridge remote_password", &cur_bridge->remote_password, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "password_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->password_data.password_file);
					ܐܢ(conf__parse_string(&token, "password_file", &cur_security_options->password_data.password_file, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "per_listener_settings")){
					OPTION_DEPRECATED(token, "Please see the documentation for how to achieve the same effect.");
					ܐܢ(config->per_listener_settings){
						/* Once this is set, don't let it be unset. It should be the first config option ideally. */
						استمر;
					}
					ܐܢ(conf__parse_bool(&token, "per_listener_settings", &config->per_listener_settings, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(cur_security_options && config->per_listener_settings){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: per_listener_settings must be set before any other security settings.");
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "persistence") || !strcmp(token, "retained_persistence")){
					ܐܢ(conf__parse_bool(&token, token, &config->persistence, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "persistence_file")){
					ܐܢ(conf__parse_string(&token, "persistence_file", &config->persistence_file, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "persistence_location")){
					ܐܢ(conf__parse_string(&token, "persistence_location", &config->persistence_location, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "persistent_client_expiration")){
					token = strtok_r(عدم, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "persistent_client_expiration");

					time_t expiration_mult;

					بدل(token[strlen(token)-1]){
						حالة 's':
							expiration_mult = 1;
							اكسر;
						حالة 'h':
							expiration_mult = 3600;
							اكسر;
						حالة 'd':
							expiration_mult = 86400;
							اكسر;
						حالة 'w':
							expiration_mult = 86400*7;
							اكسر;
						حالة 'm':
							expiration_mult = 86400*30;
							اكسر;
						حالة 'y':
							expiration_mult = 86400*365;
							اكسر;
						افتراضي:
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
							ܦܢܐ MOSQ_ERR_INVAL;
					}
					token[strlen(token)-1] = '\0';
					config->persistent_client_expiration = atoi(token)*expiration_mult;
					ܐܢ(config->persistent_client_expiration <= 0){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "pid_file")){
					ܐܢ(reload){
						استمر;        /* pid file not valid for reloading. */
					}
					ܐܢ(conf__parse_string(&token, "pid_file", &config->pid_file, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "port")){
					OPTION_DEPRECATED(token, "Please use 'listener' instead.");
					config->local_only = false;
					ܐܢ(reload){
						استمر;        /* Listeners not valid for reloading. */

					}
					ܐܢ(config__create_default_listener(config, token)){
						ܦܢܐ MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					ܐܢ(config->default_listener->port){
						log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Default listener port specified multiple times. Only the latest will be used.");
					}
					ܐܢ(conf__parse_int(&token, "port", &tmp_int, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(tmp_int < 1 || tmp_int > UINT16_MAX){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					config->default_listener->port = (uint16_t)tmp_int;
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "protocol")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(عدم, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "protocol");

					ܐܢ(!strcmp(token, "mqtt")){
						cur_listener->protocol = mp_mqtt;
						/*
						}else if(!strcmp(token, "mqttsn")){
						    cur_listener->protocol = mp_mqttsn;
						*/
					}ܐܚܪܢܐ ܐܢ(!strcmp(token, "websockets")){
#ifdef WITH_WEBSOCKETS
						cur_listener->protocol = mp_websockets;
#ܐܚܪܢܐ
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Websockets support not available.");
						ܦܢܐ MOSQ_ERR_INVAL;
#endif
					}ܐܚܪܢܐ ܐܢ(!strcmp(token, "http_api")){
#ifdef WITH_HTTP_API
						cur_listener->protocol = mp_http_api;
#ܐܚܪܢܐ
						log__printf(عدم, MOSQ_LOG_ERR, "Error: HTTP API support not available.");
						ܦܢܐ MOSQ_ERR_INVAL;
#endif
					}ܐܚܪܢܐ{
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'protocol' value (%s).", token);
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "proxy_protocol_v2_require_tls")){
#ܐܢ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(عدم, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					ܦܢܐ MOSQ_ERR_INVAL;
#endif
					REQUIRE_LISTENER(token);
					ܐܢ(conf__parse_bool(&token, "proxy_protocol_v2_require_tls", &cur_listener->proxy_protocol_v2_require_tls, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "psk_file")){
#ifdef FINAL_WITH_TLS_PSK
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ܐܢ(cur_listener && cur_listener->certfile){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_security_options->psk_file);
					ܐܢ(conf__parse_string(&token, "psk_file", &cur_security_options->psk_file, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "psk_hint")){
#ifdef FINAL_WITH_TLS_PSK
					ܐܢ(reload){
						استمر;        /* PSK file not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ܐܢ(conf__parse_string(&token, "psk_hint", &cur_listener->psk_hint, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "queue_qos0_messages")){
					ܐܢ(conf__parse_bool(&token, token, &config->queue_qos0_messages, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "require_certificate")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ܐܢ(conf__parse_bool(&token, "require_certificate", &cur_listener->require_certificate, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "restart_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					cur_bridge->backoff_cap = 0; /* set backoff to constant mode, unless cap is specified further down */
					token = strtok_r(عدم, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "restart_timeout");

					cur_bridge->restart_timeout = atoi(token);
					cur_bridge->backoff_base = 0;
					cur_bridge->backoff_cap = 0;
					ܐܢ(cur_bridge->restart_timeout < 1){
						log__printf(عدم, MOSQ_LOG_NOTICE, "restart_timeout interval too low, using 1 second.");
						cur_bridge->restart_timeout = 1;
					}ܐܚܪܢܐ ܐܢ(cur_bridge->restart_timeout > 3600){
						log__printf(عدم, MOSQ_LOG_NOTICE, "restart_timeout interval too high, using 3600 seconds.");
						cur_bridge->restart_timeout = 3600;
					}
					token = strtok_r(عدم, " ", &saveptr);
					ܐܢ(token){
						cur_bridge->backoff_base = cur_bridge->restart_timeout;
						cur_bridge->backoff_cap = atoi(token);
						ܐܢ(cur_bridge->backoff_cap < cur_bridge->backoff_base){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: backoff cap is lower than the base in restart_timeout.");
							ܦܢܐ MOSQ_ERR_INVAL;
						}ܐܚܪܢܐ ܐܢ(cur_bridge->backoff_cap > 7200){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: backoff cap too high, using 7200 seconds.");
							cur_bridge->backoff_cap = 7200;
						}

						token = strtok_r(عدم, " ", &saveptr);
						ܐܢ(token){
							cur_bridge->stable_connection_period = atoi(token);
							ܐܢ(cur_bridge->stable_connection_period < 0){
								log__printf(عدم, MOSQ_LOG_ERR, "Error: stable connection period cannot be negative.");
								ܦܢܐ MOSQ_ERR_INVAL;
							}
						}
					}
					cur_bridge->restart_timeout *= 1000; /* backoff is tracked in ms */
					cur_bridge->backoff_base *= 1000;
					cur_bridge->backoff_cap *= 1000;
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "retain_available")){
					ܐܢ(conf__parse_bool(&token, token, &config->retain_available, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "retain_expiry_interval")){
					ܐܢ(conf__parse_int(&token, token, &config->retain_expiry_interval, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(config->retain_expiry_interval < 1){
						log__printf(عدم, MOSQ_LOG_WARNING, "Error: retain_expiry_interval must be >= 1.");
						ܦܢܐ MOSQ_ERR_INVAL;
					}ܐܚܪܢܐ ܐܢ(config->retain_expiry_interval > 10000000){
						log__printf(عدم, MOSQ_LOG_WARNING, "Warning: retain_expiry_interval being capped at 19 years.");
						config->retain_expiry_interval = 10000000;
					}
					config->retain_expiry_interval *= 60;
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "retry_interval")){
					OPTION_UNAVAILABLE(token);
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "round_robin")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_bool(&token, "round_robin", &cur_bridge->round_robin, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "set_tcp_nodelay")){
					ܐܢ(conf__parse_bool(&token, "set_tcp_nodelay", &config->set_tcp_nodelay, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "start_type")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(عدم, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					ܐܢ(!strcmp(token, "automatic")){
						cur_bridge->start_type = bst_automatic;
					}ܐܚܪܢܐ ܐܢ(!strcmp(token, "lazy")){
						cur_bridge->start_type = bst_lazy;
					}ܐܚܪܢܐ ܐܢ(!strcmp(token, "manual")){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Manual start_type not supported.");
						ܦܢܐ MOSQ_ERR_INVAL;
					}ܐܚܪܢܐ ܐܢ(!strcmp(token, "once")){
						cur_bridge->start_type = bst_once;
					}ܐܚܪܢܐ{
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'start_type' value in configuration (%s).", token);
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "socket_domain")){
					ܐܢ(reload){
						استمر;        /* socket_domain not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(عدم, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					ܐܢ(!strcmp(token, "ipv4")){
						cur_listener->socket_domain = AF_INET;
					}ܐܚܪܢܐ ܐܢ(!strcmp(token, "ipv6")){
						cur_listener->socket_domain = AF_INET6;
					}ܐܚܪܢܐ{
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'socket_domain' value '%s' in configuration.", token);
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "sys_interval")){
					ܐܢ(conf__parse_int(&token, "sys_interval", &config->sys_interval, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(config->sys_interval < 0 || config->sys_interval > 65535){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'sys_interval' value (%d).", config->sys_interval);
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "threshold")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_int(&token, "threshold", &cur_bridge->threshold, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(cur_bridge->threshold < 1){
						log__printf(عدم, MOSQ_LOG_NOTICE, "threshold too low, using 1 message.");
						cur_bridge->threshold = 1;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "tls_engine")){
#ifdef WITH_TLS
					ܐܢ(reload){
						استمر;        /* tls_engine not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ܐܢ(conf__parse_string(&token, "tls_engine", &cur_listener->tls_engine, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "tls_engine_kpass_sha1")){
#ifdef WITH_TLS
					ܐܢ(reload){
						استمر;        /* tls_engine not valid for reloading. */

					}
					حرف *kpass_sha = عدم, *kpass_sha_bin = عدم;
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ܐܢ(conf__parse_string(&token, "tls_engine_kpass_sha1", &kpass_sha, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(mosquitto__hex2bin_sha1(kpass_sha, (unsigned حرف **)&kpass_sha_bin) != MOSQ_ERR_SUCCESS){
						mosquitto_FREE(kpass_sha);
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					mosquitto_free(cur_listener->tls_engine_kpass_sha1);
					cur_listener->tls_engine_kpass_sha1 = kpass_sha_bin;
					mosquitto_FREE(kpass_sha);
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "tls_keyform")){
#ifdef WITH_TLS
					ܐܢ(reload){
						استمر;        /* tls_engine not valid for reloading. */

					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					حرف *keyform = عدم;
					ܐܢ(conf__parse_string(&token, "tls_keyform", &keyform, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					cur_listener->tls_keyform = mosq_k_pem;
					ܐܢ(!strcmp(keyform, "engine")){
						cur_listener->tls_keyform = mosq_k_engine;
					}
					mosquitto_FREE(keyform);
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "tls_version")){
#ܐܢ defined(WITH_TLS)
					ܐܢ(reload){
						استمر;        /* tls_version not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					ܐܢ(conf__parse_string(&token, "tls_version", &cur_listener->tls_version, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "topic")){
#ifdef WITH_BRIDGE
					حرف *topic = عدم;
					تعداد mosquitto__bridge_direction direction = bd_out;
					uint8_t qos = 0;
					حرف *local_prefix = عدم, *remote_prefix = عدم;

					REQUIRE_BRIDGE(token);

					token = strtok_r(عدم, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "topic");

					// Check if the topic is quoted (e.g. for spaces within topic names), but not the
					// special case of ""
					ܐܢ(token[0] == '"' && token [1] != '"'){
						ܐܢ(strchr(saveptr, '"') == عدم){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Missing closing quote in topic value (%s).", saveptr);
							ܦܢܐ MOSQ_ERR_INVAL;
						}

						حرف *topic_in_quotes = strtok_r(عدم, "\"", &saveptr);
						size_t tlen = 1;
						ܐܢ(topic_in_quotes){
							tlen = strlen(topic_in_quotes);
						}

						topic = mosquitto_malloc(strlen(token) + tlen + 1);
						ܐܢ(!topic){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
							ܦܢܐ MOSQ_ERR_NOMEM;
						}

						strcpy(topic, token + 1);
						strcat(topic, " ");
						ܐܢ(topic_in_quotes){
							strcat(topic, topic_in_quotes);
						}
					}ܐܚܪܢܐ{
						topic = mosquitto_strdup(token);
						ܐܢ(!topic){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
							ܦܢܐ MOSQ_ERR_NOMEM;
						}
					}

					token = strtok_r(عدم, " ", &saveptr);
					ܐܢ(token){
						ܐܢ(!strcasecmp(token, "out")){
							direction = bd_out;
						}ܐܚܪܢܐ ܐܢ(!strcasecmp(token, "in")){
							direction = bd_in;
						}ܐܚܪܢܐ ܐܢ(!strcasecmp(token, "both")){
							direction = bd_both;
						}ܐܚܪܢܐ{
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge topic direction '%s'.", token);
							mosquitto_FREE(topic);
							ܦܢܐ MOSQ_ERR_INVAL;
						}
						token = strtok_r(عدم, " ", &saveptr);
						ܐܢ(token){
							ܐܢ(token[0] == '#'){
								(فراغ)strtok_r(عدم, "", &saveptr);
							}
							qos = (uint8_t)atoi(token);
							ܐܢ(qos > 2){
								log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge QoS level '%s'.", token);
								mosquitto_FREE(topic);
								ܦܢܐ MOSQ_ERR_INVAL;
							}

							token = strtok_r(عدم, " ", &saveptr);
							ܐܢ(token){
								ܐܢ(!strcmp(token, "\"\"") || token[0] == '#'){
									local_prefix = عدم;
									ܐܢ(token[0] == '#'){
										(فراغ)strtok_r(عدم, "", &saveptr);
									}
								}ܐܚܪܢܐ{
									local_prefix = token;
								}

								token = strtok_r(عدم, " ", &saveptr);
								ܐܢ(token){
									ܐܢ(!strcmp(token, "\"\"") || token[0] == '#'){
										remote_prefix = عدم;
									}ܐܚܪܢܐ{
										remote_prefix = token;
									}
								}
							}
						}
					}

					ܐܢ(bridge__add_topic(cur_bridge, topic, direction, qos, local_prefix, remote_prefix)){
						mosquitto_FREE(topic);
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(topic);
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "max_topic_alias")){
					REQUIRE_LISTENER(token);
					ܐܢ(conf__parse_int(&token, "max_topic_alias", &tmp_int, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias' value in configuration.");
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias = (uint16_t)tmp_int;
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "max_topic_alias_broker")){
					REQUIRE_LISTENER(token);
					ܐܢ(conf__parse_int(&token, "max_topic_alias_broker", &tmp_int, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias_broker' value in configuration.");
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias_broker = (uint16_t)tmp_int;
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "try_private")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_bool(&token, "try_private", &cur_bridge->try_private, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "upgrade_outgoing_qos")){
					ܐܢ(conf__parse_bool(&token, token, &config->upgrade_outgoing_qos, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "use_identity_as_username")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ܐܢ(conf__parse_bool(&token, "use_identity_as_username", &cur_listener->use_identity_as_username, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "use_subject_as_username")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ܐܢ(conf__parse_bool(&token, "use_subject_as_username", &cur_listener->use_subject_as_username, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "user")){
					ܐܢ(reload){
						استمر;        /* Drop privileges user not valid for reloading. */
					}
					mosquitto_FREE(config->user);
					ܐܢ(conf__parse_string(&token, "user", &config->user, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "use_username_as_clientid")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ܐܢ(conf__parse_bool(&token, "use_username_as_clientid", &cur_listener->use_username_as_clientid, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "username") || !strcmp(token, "remote_username")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ܐܢ(conf__parse_string(&token, "bridge remote_username", &cur_bridge->remote_username, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "websockets_log_level")){
#ܐܢ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					ܐܢ(conf__parse_int(&token, "websockets_log_level", &config->websockets_log_level, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
#endif
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "websockets_headers_size") || !strcmp(token, "packet_buffer_size")){
					ܐܢ(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					ܐܢ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(عدم, MOSQ_LOG_WARNING, "Error: Packet buffer size must be between 0 and 65535 inclusive.");
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					config->packet_buffer_size = (uint16_t)tmp_int;
				}ܐܚܪܢܐ ܐܢ(!strcmp(token, "websockets_origin")){
#ifdef WITH_WEBSOCKETS
#  ܐܢ LWS_LIBRARY_VERSION_NUMBER >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN
					REQUIRE_LISTENER(token);
					ws_origins = mosquitto_realloc(cur_listener->ws_origins, حجم(حرف *)*(size_t)(cur_listener->ws_origin_count+1));
					ܐܢ(ws_origins == عدم){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
						ܦܢܐ MOSQ_ERR_NOMEM;
					}
					ws_origins[cur_listener->ws_origin_count] = عدم;
					cur_listener->ws_origins = ws_origins;
					ܐܢ(conf__parse_string(&token, "websockets_origin", &ws_origins[cur_listener->ws_origin_count], &saveptr)){
						ܦܢܐ MOSQ_ERR_INVAL;
					}
					cur_listener->ws_origin_count++;
#  ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: websockets_origin support not available, libwebsockets version is too old.");
#  endif
#ܐܚܪܢܐ
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Websockets support not available.");
#endif
				}ܐܚܪܢܐ{
					log__printf(عدم, MOSQ_LOG_ERR, "Error: Unknown configuration variable '%s'.", token);
					ܦܢܐ MOSQ_ERR_INVAL;
				}
			}
		}
	}
	ܦܢܐ MOSQ_ERR_SUCCESS;
}


صحيح config__read_file(بنية mosquitto__config *config, bool reload, ثابت حرف *file, بنية config_recurse *cr, صحيح level, صحيح *lineno)
{
	صحيح rc;
	FILE *fptr = عدم;
	حرف *buf;
	صحيح buflen;
#ifndef WIN32
	DIR *dir;
#endif

#ifndef WIN32
	dir = opendir(file);
	ܐܢ(dir){
		closedir(dir);
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Config file '%s' is a directory.", file);
		ܦܢܐ 1;
	}
#endif

	fptr = mosquitto_fopen(file, "rt", false);
	ܐܢ(!fptr){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to open config file '%s'.", file);
		ܦܢܐ 1;
	}

	buflen = 1000;
	buf = mosquitto_malloc((size_t)buflen);
	ܐܢ(!buf){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		fclose(fptr);
		ܦܢܐ MOSQ_ERR_NOMEM;
	}

	rc = config__read_file_core(config, reload, cr, level, lineno, fptr, &buf, &buflen);
	mosquitto_FREE(buf);
	fclose(fptr);

	ܦܢܐ rc;
}


ساكن صحيح config__check_proxy(بنية mosquitto__config *config)
{
	لكل(صحيح i=0; i<config->listener_count; i++){
		بنية mosquitto__listener *l = &config->listeners[i];

		ܐܢ(l->enable_proxy_protocol == 2){
#ifdef WITH_TLS
			ܐܢ(l->use_subject_as_username){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: use_subject_as_username cannot be used with `enable_proxy_protocol 2`.");
				ܦܢܐ MOSQ_ERR_INVAL;
			}

			ܐܢ(l->certfile || l->keyfile){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: certfile and keyfile cannot be used with `enable_proxy_protocol 2`.");
				ܦܢܐ MOSQ_ERR_INVAL;
			}
#endif
		}
	}

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


ساكن صحيح config__check(بنية mosquitto__config *config)
{
	/* Checks that are easy to make after the config has been loaded. */

	ثابت حرف *id_prefix;
	صحيح id_prefix_len;
	ܐܢ(config->security_options.auto_id_prefix){
		id_prefix = config->security_options.auto_id_prefix;
		id_prefix_len = config->security_options.auto_id_prefix_len;
	}ܐܚܪܢܐ{
		id_prefix = "auto-";
		id_prefix_len = (صحيح)strlen("auto-");
	}

	/* Default to auto_id_prefix = 'auto-' if none set. */
	لكل(صحيح i=0; i<config->listener_count; i++){
		ܐܢ(!config->listeners[i].security_options->auto_id_prefix){
			config->listeners[i].security_options->auto_id_prefix = mosquitto_strdup(id_prefix);
			ܐܢ(!config->listeners[i].security_options->auto_id_prefix){
				ܦܢܐ MOSQ_ERR_NOMEM;
			}
			config->listeners[i].security_options->auto_id_prefix_len = (uint16_t)id_prefix_len;
		}
	}

	ܦܢܐ config__check_proxy(config);
}

#ifdef WITH_BRIDGE


ساكن صحيح config__check_bridges(بنية mosquitto__config *config)
{
	بنية mosquitto__bridge *bridge1, *bridge2;
	حرف hostname[256];
	size_t len;

	/* Check for bridge duplicate local_clientid, need to generate missing IDs
	 * first. */
	لكل(صحيح i=0; i<config->bridge_count; i++){
		bridge1 = config->bridges[i];

		ܐܢ(!bridge1->remote_clientid){
			ܐܢ(!gethostname(hostname, 256)){
				len = strlen(hostname) + strlen(bridge1->name) + 2;
				bridge1->remote_clientid = mosquitto_malloc(len);
				ܐܢ(!bridge1->remote_clientid){
					ܦܢܐ MOSQ_ERR_NOMEM;
				}
				snprintf(bridge1->remote_clientid, len, "%s.%s", hostname, bridge1->name);
			}ܐܚܪܢܐ{
				ܦܢܐ 1;
			}
		}

		ܐܢ(!bridge1->local_clientid){
			len = strlen(bridge1->remote_clientid) + strlen("local.") + 2;
			bridge1->local_clientid = mosquitto_malloc(len);
			ܐܢ(!bridge1->local_clientid){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
				ܦܢܐ MOSQ_ERR_NOMEM;
			}
			snprintf(bridge1->local_clientid, len, "local.%s", bridge1->remote_clientid);
		}
	}

	لكل(صحيح i=0; i<config->bridge_count; i++){
		bridge1 = config->bridges[i];
		لكل(صحيح j=i+1; j<config->bridge_count; j++){
			bridge2 = config->bridges[j];
			ܐܢ(!strcmp(bridge1->local_clientid, bridge2->local_clientid)){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Bridge local_clientid "
						"'%s' is not unique. Try changing or setting the "
						"local_clientid value for one of the bridges.",
						bridge1->local_clientid);
				ܦܢܐ MOSQ_ERR_INVAL;
			}
		}
	}

#ifdef WITH_TLS
	/* Check for missing TLS cafile/capath/certfile/keyfile */
	لكل(صحيح i=0; i<config->listener_count; i++){
		bool cafile = !!config->listeners[i].cafile;
		bool capath = !!config->listeners[i].capath;
		bool certfile = !!config->listeners[i].certfile;
		bool keyfile = !!config->listeners[i].keyfile;

		ܐܢ((certfile && !keyfile) || (!certfile && keyfile)){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Both certfile and keyfile must be provided to enable a TLS listener.");
			ܦܢܐ MOSQ_ERR_INVAL;
		}
		ܐܢ(cafile && !certfile){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: cafile specified without certfile and keyfile.");
			ܦܢܐ MOSQ_ERR_INVAL;
		}
		ܐܢ(capath && !certfile){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: capath specified without certfile and keyfile.");
			ܦܢܐ MOSQ_ERR_INVAL;
		}
	}
#endif
	ܦܢܐ MOSQ_ERR_SUCCESS;
}
#endif


ساكن صحيح conf__parse_bool(حرف **token, ثابت حرف *name, bool *value, حرف **saveptr)
{
	*token = strtok_r(عدم, " ", saveptr);
	ܐܢ(*token){
		ܐܢ(!strcmp(*token, "false") || !strcmp(*token, "0")){
			*value = false;
		}ܐܚܪܢܐ ܐܢ(!strcmp(*token, "true") || !strcmp(*token, "1")){
			*value = true;
		}ܐܚܪܢܐ{
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid '%s' value (%s).", name, *token);
			ܦܢܐ MOSQ_ERR_INVAL;
		}
	}ܐܚܪܢܐ{
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


ساكن صحيح conf__parse_int(حرف **token, ثابت حرف *name, صحيح *value, حرف **saveptr)
{
	*token = strtok_r(عدم, " ", saveptr);
	ܐܢ(*token){
		حرف *endptr = عدم;
		long l = strtol(*token, &endptr, 0);
		ܐܢ(endptr == *token || endptr[0] != '\0'){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: '%s' value not a number.", name);
			ܦܢܐ MOSQ_ERR_INVAL;
		}
		ܐܢ(l > INT_MAX || l < INT_MIN){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: '%s' value out of range.", name);
			ܦܢܐ MOSQ_ERR_INVAL;
		}
		*value = (صحيح)l;
	}ܐܚܪܢܐ{
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


ساكن صحيح conf__parse_ssize_t(حرف **token, ثابت حرف *name, ssize_t *value, حرف **saveptr)
{
	*token = strtok_r(عدم, " ", saveptr);
	ܐܢ(*token){
		*value = atol(*token);
	}ܐܚܪܢܐ{
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


ساكن صحيح conf__parse_string(حرف **token, ثابت حرف *name, حرف **value, حرف **saveptr)
{
	size_t tlen;

	*token = strtok_r(عدم, "", saveptr);
	ܐܢ(*token){
		ܐܢ(*value){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Duplicate '%s' value in configuration.", name);
			ܦܢܐ MOSQ_ERR_INVAL;
		}
		/* Deal with multiple spaces at the beginning of the string. */
		*token = mosquitto_trimblanks(*token);
		ܐܢ(strlen(*token) == 0){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
			ܦܢܐ MOSQ_ERR_INVAL;
		}

		tlen = strlen(*token);
		ܐܢ(tlen > UINT16_MAX){
			ܦܢܐ MOSQ_ERR_INVAL;
		}
		ܐܢ(mosquitto_validate_utf8(*token, (uint16_t)tlen)){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Malformed UTF-8 in configuration.");
			ܦܢܐ MOSQ_ERR_INVAL;
		}
		*value = mosquitto_strdup(*token);
		ܐܢ(!*value){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
			ܦܢܐ MOSQ_ERR_NOMEM;
		}
	}ܐܚܪܢܐ{
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		ܦܢܐ MOSQ_ERR_INVAL;
	}
	ܦܢܐ MOSQ_ERR_SUCCESS;
}
