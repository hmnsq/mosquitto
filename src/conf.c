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
#وإلا
#  include <dirent.h>
#  include <strings.h>
#endif

#ifndef WIN32
#  include <netdb.h>
#  include <sys/socket.h>
#وإلا
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#إذا !defined(WIN32) && !defined(__CYGWIN__)
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

#إذا defined(WIN32) || defined(__CYGWIN__)
#ضمن <windows.h>
خارجي SERVICE_STATUS_HANDLE service_handle;
#endif


#define REQUIRE_LISTENER(A) \
		افعل{ \
			إذا(cur_listener == عدم){ \
				log__printf(عدم, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				أرجع MOSQ_ERR_INVAL; \
			} \
		}طالما(0)

#define REQUIRE_LISTENER_OR_DEFAULT_LISTENER(A) \
		افعل{ \
			إذا(cur_listener == عدم){ \
				إذا(config__create_default_listener(config, (A))){ \
					أرجع MOSQ_ERR_NOMEM; \
				} \
				cur_listener = config->default_listener; \
			} \
		}طالما(0)

#define REQUIRE_LISTENER_IF_PER_LISTENER(A) \
		افعل{ \
			إذا(config->per_listener_settings == true && cur_listener == عدم){ \
				log__printf(عدم, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				أرجع MOSQ_ERR_INVAL; \
			} \
		}طالما(0)

#define REQUIRE_NON_DEFAULT_LISTENER(A) \
		افعل{ \
			إذا(cur_listener == config->default_listener || cur_listener == عدم){ \
				log__printf(عدم, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				أرجع MOSQ_ERR_INVAL; \
			} \
		}طالما(0)

#define REQUIRE_BRIDGE(A) \
		افعل{ \
			إذا(cur_bridge == عدم){ \
				log__printf(عدم, MOSQ_LOG_ERR, "Error: The '%s' option requires a bridge to be defined first.", (A)); \
				أرجع MOSQ_ERR_INVAL; \
			} \
		}طالما(0)

#define REQUIRE_PLUGIN(A) \
		افعل{ \
			إذا(cur_plugin == عدم){ \
				log__printf(عدم, MOSQ_LOG_ERR, "Error: The '%s' option requires plugin/global_plugin/plugin_load to be defined first.", (A)); \
				أرجع MOSQ_ERR_INVAL; \
			} \
		}طالما(0)

#define OPTION_DEPRECATED(A, B) \
		log__printf(عدم, MOSQ_LOG_NOTICE, "The '%s' option is now deprecated and will be removed in version 3.0. %s", (A), (B))

#define OPTION_UNAVAILABLE(A) \
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: The '%s' option is no longer available.", (A));

#define REQUIRE_NON_EMPTY_OPTION(A, B) \
		افعل{ \
			إذا(!(A)){ \
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", (B)); \
				أرجع MOSQ_ERR_INVAL; \
			} \
		}طالما(0)

#define PER_LISTENER_ALTERNATIVE(A, B) \
		إذا(config->per_listener_settings){ \
			log__printf(عدم, MOSQ_LOG_NOTICE, "You are using the '%s' option with 'per_listener_settings true'. Please replace this with '%s'.", A, B); \
		} \


#ifdef FINAL_WITH_TLS_PSK
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A) \
		افعل{ \
			إذا(cur_bridge->tls_psk_identity || cur_bridge->tls_psk){ \
				log__printf(عدم, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and psk encryption in a single bridge.", (A)); \
				أرجع MOSQ_ERR_INVAL; \
			} \
		}طالما(0)

#  define REQUIRE_BRIDGE_NO_X509(A) \
		افعل{ \
			إذا(cur_bridge->tls_cafile || cur_bridge->tls_capath || cur_bridge->tls_certfile || cur_bridge->tls_keyfile){ \
				log__printf(عدم, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and identity encryption in a single bridge.", (A)); \
				أرجع MOSQ_ERR_INVAL; \
			} \
		}طالما(0)

#وإلا
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A)
#  define REQUIRE_BRIDGE_NO_X509(A)
#endif

ساكن بنية mosquitto__security_options *cur_security_options = عدم;

ساكن صحيح conf__parse_bool(حرف **token, ثابت حرف *name, bool *قدر, حرف **saveptr);
ساكن صحيح conf__parse_int(حرف **token, ثابت حرف *name, صحيح *قدر, حرف **saveptr);
ساكن صحيح conf__parse_ssize_t(حرف **token, ثابت حرف *name, ssize_t *قدر, حرف **saveptr);
ساكن صحيح conf__parse_string(حرف **token, ثابت حرف *name, حرف **قدر, حرف **saveptr);
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

	إذا(config->default_listener){
		لكل(صحيح i=0; i<config->listener_گنتی; i++){
			إذا(&config->listeners[i] == config->default_listener){
				def_listener = i;
				اكسر;
			}
		}
	}
	new_listeners = mosquitto_realloc(config->listeners, حجم(بنية mosquitto__listener)*(size_t)(config->listener_گنتی+1));
	إذا(!new_listeners){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		أرجع MOSQ_ERR_NOMEM;
	}
	config->listeners = new_listeners;
	listener = &config->listeners[config->listener_گنتی];
	memset(listener, 0, حجم(بنية mosquitto__listener));
	listener->security_options = mosquitto_calloc(1, حجم(بنية mosquitto__security_options));
	إذا(!listener->security_options){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		أرجع MOSQ_ERR_NOMEM;
	}
	إذا(def_listener != -1){
		config->default_listener = &config->listeners[def_listener];
	}
	config->listener_گنتی++;

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح config__create_default_listener(بنية mosquitto__config *config, ثابت حرف *option_name)
{
	إذا(config->default_listener){
		أرجع MOSQ_ERR_SUCCESS;
	}
	log__printf(عدم, MOSQ_LOG_INFO, "Creating default listener due to '%s' option.", option_name);
	log__printf(عدم, MOSQ_LOG_INFO, "It is best practice to define a 'listener' first. Using the '%s' option without a listener will be disabled in the future.", option_name);
	إذا(config__add_listener(config)){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		أرجع MOSQ_ERR_NOMEM;
	}

	config->default_listener = &config->listeners[config->listener_گنتی-1];
	listener__set_defaults(config->default_listener);
	config->default_listener->port = 1883;

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن فراغ conf__set_cur_security_options(بنية mosquitto__config *config, بنية mosquitto__listener **cur_listener, بنية mosquitto__security_options **security_options, ثابت حرف *option_name)
{
	إذا(config->per_listener_settings){
		إذا(*cur_listener == عدم){
			إذا(config__create_default_listener(config, option_name)){
				أرجع;
			}
			*cur_listener = config->default_listener;
		}
		(*security_options) = (*cur_listener)->security_options;
	}وإلا{
		(*security_options) = &config->security_options;
	}
}


ساكن صحيح conf__attempt_resolve(ثابت حرف *host, ثابت حرف *text, unsigned صحيح log, ثابت حرف *پیغام)
{
	بنية addrinfo gai_hints;
	بنية addrinfo *gai_res;
	صحيح rc;

	memset(&gai_hints, 0, حجم(بنية addrinfo));
	gai_hints.ai_family = AF_UNSPEC;
	gai_hints.ai_socktype = SOCK_STREAM;
	gai_res = عدم;
	rc = getaddrinfo(host, عدم, &gai_hints, &gai_res);
	إذا(gai_res){
		freeaddrinfo(gai_res);
	}
	إذا(rc != 0){
#ifndef WIN32
		إذا(rc == EAI_SYSTEM){
			إذا(errno == ENOENT){
				log__printf(عدم, log, "%s: Unable to resolve %s %s.", پیغام, text, host);
			}وإلا{
				log__printf(عدم, log, "%s: Error resolving %s: %s.", پیغام, text, strerror(errno));
			}
		}وإلا{
			log__printf(عدم, log, "%s: Error resolving %s: %s.", پیغام, text, gai_strerror(rc));
		}
#وإلا
		إذا(rc == WSAHOST_NOT_FOUND){
			log__printf(عدم, log, "%s: Error resolving %s.", پیغام, text);
		}
#endif
		أرجع MOSQ_ERR_INVAL;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن فراغ config__init_reload(بنية mosquitto__config *config)
{
	/* Set defaults */
	لكل(صحيح i=0; i<config->listener_گنتی; i++){
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
	إذا(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = عدم;
	}
	mosquitto_FREE(config->log_file);

#إذا defined(WIN32) || defined(__CYGWIN__)
	إذا(service_handle){
		/* This is running as a Windows service. Default to no logging. Using
		 * stdout/stderr is forbidden because the first clients to connect will
		 * get log information sent to them for some reason. */
		config->log_dest = MQTT3_LOG_NONE;
	}وإلا{
		config->log_dest = MQTT3_LOG_STDERR;
	}
#وإلا
	config->log_facility = LOG_DAEMON;
	config->log_dest = MQTT3_LOG_STDERR | MQTT3_LOG_DLT;
	إذا(db.quiet){
		config->log_type = 0;
	}وإلا إذا(db.verbose){
		config->log_type = UINT_MAX;
	}وإلا{
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

	إذا(plugin->config.options){
		لكل(صحيح j=0; j<plugin->config.option_گنتی; j++){
			mosquitto_FREE(plugin->config.options[j].کلید);
			mosquitto_FREE(plugin->config.options[j].قدر);
		}
		mosquitto_FREE(plugin->config.options);
		plugin->config.option_گنتی = 0;
	}
	mosquitto_FREE(plugin->config.security_options);
	mosquitto_FREE(plugin);
}


ساكن فراغ config__cleanup_plugins(فراغ)
{
	لكل(صحيح i=0; i<db.plugin_گنتی; i++){
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
	إذا(config->listeners){
		لكل(صحيح i=0; i<config->listener_گنتی; i++){
			mosquitto_FREE(config->listeners[i].host);
			mosquitto_FREE(config->listeners[i].bind_interface);
			mosquitto_FREE(config->listeners[i].mount_point);
			mosquitto_FREE(config->listeners[i].socks);
			إذا(config->listeners[i].security_options){
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
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			إذا(!config->listeners[i].ws_context) /* libwebsockets frees its own SSL_CTX */
#endif
			{
				SSL_CTX_free(config->listeners[i].ssl_ctx);
				config->listeners[i].ssl_ctx = عدم;
			}
#endif
#إذا defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
			mosquitto_FREE(config->listeners[i].http_dir);
#endif
#ifdef WITH_WEBSOCKETS
			لكل(صحيح j=0; j<config->listeners[i].ws_origin_گنتی; j++){
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
	إذا(config->bridges){
		لكل(صحيح i=0; i<config->bridge_گنتی; i++){
			config__bridge_cleanup(config->bridges[i]);
		}
		mosquitto_FREE(config->bridges);
	}
#endif
	config__cleanup_plugins();

	إذا(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = عدم;
	}
	إذا(config->log_file){
		mosquitto_FREE(config->log_file);
		config->log_file = عدم;
	}
}

#ifdef WITH_BRIDGE


فراغ config__bridge_cleanup(بنية mosquitto__bridge *bridge)
{
	إذا(bridge == عدم){
		أرجع;
	}

	mosquitto_FREE(bridge->name);
	إذا(bridge->addresses){
		لكل(صحيح i=0; i<bridge->address_گنتی; i++){
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
	إذا(bridge->topics){
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
		إذا(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--config-file")){
			إذا(i<argc-1){
				db.config_file = argv[i+1];

				إذا(config__read(config, false)){
					أرجع MOSQ_ERR_INVAL;
				}
			}وإلا{
				log__printf(عدم, MOSQ_LOG_ERR, "Error: -c argument given, but no config file specified.");
				أرجع MOSQ_ERR_INVAL;
			}
			i++;
		}وإلا إذا(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--daemon")){
			config->daemon = true;
		}وإلا إذا(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
			print_usage();
			أرجع MOSQ_ERR_UNKNOWN;
		}وإلا إذا(!strcmp(argv[i], "--version")){
			print_version();
			أرجع MOSQ_ERR_UNKNOWN;
		}وإلا إذا(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port")){
			إذا(i<argc-1){
				port_tmp = atoi(argv[i+1]);
				إذا(port_tmp<1 || port_tmp>UINT16_MAX){
					log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid port specified (%d).", port_tmp);
					أرجع MOSQ_ERR_INVAL;
				}وإلا{
					إذا(config->cmd_port_گنتی == CMD_PORT_LIMIT){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Only %d ports can be specified on the command line.", CMD_PORT_LIMIT);
						أرجع MOSQ_ERR_INVAL;
					}
					config->cmd_port[config->cmd_port_گنتی] = (uint16_t)port_tmp;
					config->cmd_port_گنتی++;
				}
			}وإلا{
				log__printf(عدم, MOSQ_LOG_ERR, "Error: -p argument given, but no port specified.");
				أرجع MOSQ_ERR_INVAL;
			}
			i++;
		}وإلا إذا(!strcmp(argv[i], "--tls-keylog")){
#ifdef WITH_TLS
			إذا(i<argc-1){
				db.tls_keylog = mosquitto_strdup(argv[i+1]);
				إذا(db.tls_keylog == عدم){
					log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
					أرجع MOSQ_ERR_NOMEM;
				}
			}وإلا{
				log__printf(عدم, MOSQ_LOG_ERR, "Error: --tls-keylog argument given, but no file specified.");
				أرجع MOSQ_ERR_INVAL;
			}
			i++;
#وإلا
			fprintf(stderr, "Error: TLS support not available so --tls-keylog is not available.\n");
			أرجع MOSQ_ERR_INVAL;
#endif
		}وإلا إذا(!strcmp(argv[i], "-q") || !strcmp(argv[i], "--quiet")){
			db.quiet = true;
		}وإلا إذا(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")){
			db.verbose = true;
		}وإلا إذا(!strcmp(argv[i], "--test-config")){
			config->test_configuration = true;
		}وإلا{
			fprintf(stderr, "Error: Unknown option '%s'.\n", argv[i]);
			print_usage();
			أرجع MOSQ_ERR_INVAL;
		}
	}

	/* Default to drop to mosquitto user if we are privileged and no user specified. */
	إذا(!config->user){
		config->user = mosquitto_strdup("mosquitto");
		إذا(config->user == عدم){
			أرجع MOSQ_ERR_NOMEM;
		}
	}
	إذا(db.quiet){
		config->log_type = 0;
	}وإلا إذا(db.verbose){
		config->log_type = UINT_MAX;
	}

	إذا(getenv("MOSQUITTO_PERSISTENCE_LOCATION")){
		mosquitto_FREE(config->persistence_location);
		config->persistence_location = mosquitto_strdup(getenv("MOSQUITTO_PERSISTENCE_LOCATION"));
		إذا(!config->persistence_location){
			أرجع MOSQ_ERR_NOMEM;
		}
	}
	أرجع config__check(config);
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
	dest->security_options.plugin_گنتی = src->security_options.plugin_گنتی;
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

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	dest->websockets_log_level = src->websockets_log_level;
#endif

#ifdef WITH_BRIDGE
	لكل(صحيح i=0; i<dest->bridge_گنتی; i++){
		إذا(dest->bridges[i]){
			config__bridge_cleanup(dest->bridges[i]);
		}
	}
	mosquitto_FREE(dest->bridges);
	dest->bridges = src->bridges;
	dest->bridge_گنتی = src->bridge_گنتی;
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

	إذا(reload){
		memset(&config_reload, 0, حجم(بنية mosquitto__config));
	}

	cr.log_dest = MQTT3_LOG_NONE;
	cr.log_dest_set = 0;
	cr.log_type = MOSQ_LOG_NONE;
	cr.log_type_set = 0;

	إذا(!db.config_file){
		أرجع 0;
	}

	إذا(reload){
		/* Re-initialise appropriate config vars to default for reload. */
		config__init_reload(&config_reload);
		config_reload.listeners = config->listeners;
		config_reload.listener_گنتی = config->listener_گنتی;
		cur_security_options = عدم;
		rc = config__read_file(&config_reload, reload, db.config_file, &cr, 0, &lineno);
	}وإلا{
		rc = config__read_file(config, reload, db.config_file, &cr, 0, &lineno);
	}
	إذا(rc){
		إذا(lineno > 0){
			log__printf(عدم, MOSQ_LOG_ERR, "Error found at %s:%d.", db.config_file, lineno);
		}
		أرجع rc;
	}

	إذا(reload){
		config__copy(&config_reload, config);
	}

	/* If auth/access options are set and allow_anonymous not explicitly set, disallow anon. */
	إذا(config->local_only == false){
		إذا(config->per_listener_settings){
			لكل(i=0; i<config->listener_گنتی; i++){
				/* Default option if no security options set */
				إذا(config->listeners[i].security_options->allow_anonymous == -1){
					config->listeners[i].security_options->allow_anonymous = false;
				}
			}
		}وإلا{
			إذا(config->security_options.allow_anonymous == -1){
				config->security_options.allow_anonymous = false;
			}
		}
	}
#ifdef WITH_PERSISTENCE
	إذا(config->persistence){
		إذا(!config->persistence_file){
			config->persistence_file = mosquitto_strdup("mosquitto.db");
			إذا(!config->persistence_file){
				أرجع MOSQ_ERR_NOMEM;
			}
		}
		mosquitto_FREE(config->persistence_filepath);
		إذا(config->persistence_location && strlen(config->persistence_location)){
			len = strlen(config->persistence_location) + strlen(config->persistence_file) + 2;
			config->persistence_filepath = mosquitto_malloc(len);
			إذا(!config->persistence_filepath){
				أرجع MOSQ_ERR_NOMEM;
			}
#ifdef WIN32
			snprintf(config->persistence_filepath, len, "%s\\%s", config->persistence_location, config->persistence_file);
#وإلا
			snprintf(config->persistence_filepath, len, "%s/%s", config->persistence_location, config->persistence_file);
#endif
		}وإلا{
			config->persistence_filepath = mosquitto_strdup(config->persistence_file);
			إذا(!config->persistence_filepath){
				أرجع MOSQ_ERR_NOMEM;
			}
		}
	}
#endif
	/* Default to drop to mosquitto user if no other user specified. This must
	 * remain here even though it is covered in config__parse_args() because this
	 * function may be called on its own. */
	إذا(!config->user){
		config->user = mosquitto_strdup("mosquitto");
	}

#ifdef WITH_BRIDGE
	لكل(i=0; i<config->bridge_گنتی; i++){
		إذا(!config->bridges[i]->name){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: bridge name not defined.");
			أرجع MOSQ_ERR_INVAL;
		}
		إذا(config->bridges[i]->addresses  == 0){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no remote addresses defined.");
			أرجع MOSQ_ERR_INVAL;
		}
		إذا(config->bridges[i]->topic_گنتی == 0){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no topics defined.");
			أرجع MOSQ_ERR_INVAL;
		}
#ifdef FINAL_WITH_TLS_PSK
		إذا(config->bridges[i]->tls_psk && !config->bridges[i]->tls_psk_identity){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_identity.");
			أرجع MOSQ_ERR_INVAL;
		}
		إذا(config->bridges[i]->tls_psk_identity && !config->bridges[i]->tls_psk){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_psk.");
			أرجع MOSQ_ERR_INVAL;
		}
#endif
	}
#endif

	إذا(cr.log_dest_set){
		config->log_dest = cr.log_dest;
	}
	إذا(db.quiet){
		config->log_type = 0;
	}وإلا إذا(db.verbose){
		config->log_type = UINT_MAX;
	}وإلا إذا(cr.log_type_set){
		config->log_type = cr.log_type;
	}
#ifdef WITH_BRIDGE
	أرجع config__check_bridges(config);
#وإلا
	أرجع MOSQ_ERR_SUCCESS;
#endif
}


ساكن mosquitto_plugin_id_t *config__plugin_find(ثابت حرف *name)
{
	إذا(db.plugins && name){
		لكل(صحيح i=0; i<db.plugin_گنتی; i++){
			إذا(db.plugins[i]->config.name && !strcmp(db.plugins[i]->config.name, name)){
				أرجع db.plugins[i];
			}
		}
	}
	أرجع عدم;
}


ساكن mosquitto_plugin_id_t *config__plugin_load(ثابت حرف *name, ثابت حرف *path)
{
	mosquitto_plugin_id_t *plugin = عدم;
	mosquitto_plugin_id_t **plugins = عدم;

	إذا(name && config__plugin_find(name)){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Duplicate plugin name '%s'.", name);
		أرجع عدم;
	}

	إذا(!path || !strcmp(path, "")){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Missing plugin path for plugin name '%s'.", name);
		أرجع عدم;
	}

	plugin = mosquitto_calloc(1, حجم(mosquitto_plugin_id_t));
	إذا(!plugin){
		اذهب error;
	}

	إذا(name){
		plugin->config.name = mosquitto_strdup(name);
	}
	plugin->config.path = mosquitto_strdup(path);
	إذا((name && !plugin->config.name) || !plugin->config.path){
		اذهب error;
	}
	plugin->config.options = عدم;
	plugin->config.option_گنتی = 0;
	plugin->config.deny_special_chars = true;

	/* Add to db list */
	plugins = mosquitto_realloc(db.plugins, (size_t)(db.plugin_گنتی+1)*حجم(بنية mosquitto__plugin_config *));
	إذا(!plugins){
		اذهب error;
	}

	plugins[db.plugin_گنتی] = plugin;
	db.plugins = plugins;
	db.plugin_گنتی++;

	أرجع plugin;
error:
	log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
	إذا(plugin){
		mosquitto_FREE(plugin->config.name);
		mosquitto_FREE(plugin->config.path);
	}
	mosquitto_FREE(plugin);
	أرجع عدم;
}


صحيح config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, بنية mosquitto__security_options *security_options)
{
	بنية mosquitto__security_options **new_options;
	mosquitto_plugin_id_t **new_plugins;

	new_options = mosquitto_realloc(plugin->config.security_options, (size_t)(plugin->config.security_option_گنتی+1)*حجم(بنية mosquitto__security_options *));
	new_plugins = mosquitto_realloc(security_options->plugins, (size_t)(security_options->plugin_گنتی+1)*حجم(mosquitto_plugin_id_t *));

	إذا(!new_options || !new_plugins){
		mosquitto_FREE(new_options);
		mosquitto_FREE(new_plugins);
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		أرجع MOSQ_ERR_NOMEM;
	}

	new_options[plugin->config.security_option_گنتی] = security_options;
	plugin->config.security_options = new_options;
	plugin->config.security_option_گنتی++;

	new_plugins[security_options->plugin_گنتی] = plugin;
	security_options->plugins = new_plugins;
	security_options->plugin_گنتی++;

	أرجع MOSQ_ERR_SUCCESS;
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

	حرف *کلید;
	بنية mosquitto__listener *cur_listener = عدم;
	صحيح i;
	صحيح lineno_ext = 0;
	size_t prefix_len;
	size_t slen;
#ifdef WITH_WEBSOCKETS
	حرف **ws_origins = عدم;
#endif

	*lineno = 0;

	طالما(mosquitto_fgets(buf, buflen, fptr)){
		(*lineno)++;
		إذا((*buf)[0] != '#' && (*buf)[0] != 10 && (*buf)[0] != 13){
			slen = strlen(*buf);
			إذا(slen == 0){
				استمر;
			}
			طالما((*buf)[slen-1] == 10 || (*buf)[slen-1] == 13){
				(*buf)[slen-1] = 0;
				slen = strlen(*buf);
				إذا(slen == 0){
					استمر;
				}
			}
			token = strtok_r((*buf), " ", &saveptr);
			إذا(token){
				إذا(!strcmp(token, "accept_protocol_versions")){
					REQUIRE_NON_DEFAULT_LISTENER(token);
					cur_listener->disable_protocol_v3 = true;
					cur_listener->disable_protocol_v4 = true;
					cur_listener->disable_protocol_v5 = true;
					إذا(saveptr == عدم){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", "accept_protocol_versions");
						أرجع MOSQ_ERR_INVAL;
					}
					token = strtok_r(saveptr, ", \t", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "accept_protocol_versions");

					طالما(token){
						إذا(!strcmp(token, "3")){
							cur_listener->disable_protocol_v3 = false;
						}وإلا إذا(!strcmp(token, "4")){
							cur_listener->disable_protocol_v4 = false;
						}وإلا إذا(!strcmp(token, "5")){
							cur_listener->disable_protocol_v5 = false;
						}

						token = strtok_r(عدم, ", \t", &saveptr);
					}
				}وإلا إذا(!strcmp(token, "acl_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);

					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->acl_data.acl_file);
					إذا(conf__parse_string(&token, "acl_file", &cur_security_options->acl_data.acl_file, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "address") || !strcmp(token, "addresses")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					إذا(cur_bridge->addresses){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge configuration, 'address' only allowed once.");
						أرجع MOSQ_ERR_INVAL;
					}
					طالما((token = strtok_r(عدم, " ", &saveptr))){
						إذا(token[0] == '#'){
							اكسر;
						}
						بنية bridge_address *new_addresses = mosquitto_realloc(cur_bridge->addresses, حجم(بنية bridge_address)*(size_t)(cur_bridge->address_گنتی+1));
						إذا(!new_addresses){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
							أرجع MOSQ_ERR_NOMEM;
						}
						cur_bridge->address_گنتی++;
						cur_bridge->addresses = new_addresses;
						memset(&cur_bridge->addresses[cur_bridge->address_گنتی-1], 0, حجم(بنية bridge_address));
						cur_bridge->addresses[cur_bridge->address_گنتی-1].address = mosquitto_strdup(token);
						إذا(!cur_bridge->addresses[cur_bridge->address_گنتی-1].address){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
							أرجع MOSQ_ERR_NOMEM;
						}
					}
					لكل(i=0; i<cur_bridge->address_گنتی; i++){
						/* cur_bridge->addresses[i].address is now
						 * "address[:port]". If address is an IPv6 address,
						 * then port is required. We must check for the :
						 * backwards. */
						tmp_char = strrchr(cur_bridge->addresses[i].address, ':');
						إذا(tmp_char){
							/* Remove ':', so cur_bridge->addresses[i].address
							 * now just looks like the address. */
							tmp_char[0] = '\0';

							/* The remainder of the string */
							tmp_int = atoi(&tmp_char[1]);
							إذا(tmp_int < 1 || tmp_int > UINT16_MAX){
								log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge port value (%d).", tmp_int);
								أرجع MOSQ_ERR_INVAL;
							}
							cur_bridge->addresses[i].port = (uint16_t)tmp_int;
						}وإلا{
							cur_bridge->addresses[i].port = 1883;
						}
						conf__attempt_resolve(cur_bridge->addresses[i].address, "bridge address", MOSQ_LOG_WARNING, "Warning");
					}
					إذا(cur_bridge->address_گنتی == 0){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Empty address value in configuration.");
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "allow_anonymous")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					PER_LISTENER_ALTERNATIVE(token, "listener_allow_anonymous");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					إذا(conf__parse_bool(&token, "allow_anonymous", (bool *)&cur_security_options->allow_anonymous, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "allow_duplicate_messages")){
					OPTION_DEPRECATED(token, "The behaviour will default to true.");
					إذا(conf__parse_bool(&token, "allow_duplicate_messages", &config->allow_duplicate_messages, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "allow_zero_length_clientid")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					إذا(conf__parse_bool(&token, "allow_zero_length_clientid", &cur_security_options->allow_zero_length_clientid, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strncmp(token, "auth_opt_", strlen("auth_opt_")) || !strncmp(token, "plugin_opt_", strlen("plugin_opt_"))){
					إذا(reload){
						استمر;        /* Auth plugin not currently valid for reloading. */

					}
					REQUIRE_PLUGIN(token);

					إذا(!strncmp(token, "auth_opt_", strlen("auth_opt_"))){
						prefix_len = strlen("auth_opt_");
					}وإلا{
						prefix_len = strlen("plugin_opt_");
					}
					إذا(strlen(token) < prefix_len + 3){
						/* auth_opt_ == 9, + one digit key == 10, + one space == 11, + one value == 12 */
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'plugin_opt_' config option.");
						أرجع MOSQ_ERR_INVAL;
					}
					کلید = mosquitto_strdup(&token[prefix_len]);
					إذا(!کلید){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
						أرجع MOSQ_ERR_NOMEM;
					}وإلا إذا(STREMPTY(کلید)){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Empty 'plugin_opt_' config option.");
						mosquitto_FREE(کلید);
						أرجع MOSQ_ERR_INVAL;
					}
					token = saveptr;
					إذا(token && token[0]){
						طالما(token[0] == ' ' || token[0] == '\t'){
							token++;
						}
						بنية mosquitto_opt *new_options = mosquitto_realloc(cur_plugin->config.options, (size_t)(cur_plugin->config.option_گنتی+1)*حجم(بنية mosquitto_opt));
						إذا(!new_options){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
							mosquitto_FREE(کلید);
							أرجع MOSQ_ERR_NOMEM;
						}
						cur_plugin->config.option_گنتی++;
						cur_plugin->config.options = new_options;
						cur_plugin->config.options[cur_plugin->config.option_گنتی-1].کلید = کلید;
						cur_plugin->config.options[cur_plugin->config.option_گنتی-1].قدر = mosquitto_strdup(token);
						إذا(!cur_plugin->config.options[cur_plugin->config.option_گنتی-1].قدر){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
							أرجع MOSQ_ERR_NOMEM;
						}
					}وإلا{
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", کلید);
						mosquitto_FREE(کلید);
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "auth_plugin") || !strcmp(token, "plugin") || !strcmp(token, "global_plugin")){
					إذا(reload){
						استمر;        /* plugin not currently valid for reloading. */
					}
					إذا(!strcmp(token, "global_plugin")){
						cur_security_options = &db.config->security_options;
					}وإلا{
						REQUIRE_LISTENER_IF_PER_LISTENER(token);
						conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					}

					cur_plugin = config__plugin_load(عدم, saveptr);
					إذا(cur_plugin == عدم){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(config__plugin_add_secopt(cur_plugin, cur_security_options)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "auth_plugin_deny_special_chars")){
					إذا(reload){
						استمر;        /* Auth plugin not currently valid for reloading. */
					}
					إذا(!cur_plugin){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: An auth_plugin_deny_special_chars option exists in the config file without a plugin.");
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(conf__parse_bool(&token, "auth_plugin_deny_special_chars", &cur_plugin->config.deny_special_chars, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "plugin_load")){
					حرف *name = عدم;

					إذا(reload){
						استمر;        /* plugin not currently valid for reloading. */

					}
					name = strtok_r(عدم, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(name, "plugin_load");

					cur_plugin = config__plugin_load(name, saveptr);
					إذا(cur_plugin == عدم){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "plugin_use")){
					حرف *name = عدم;

					إذا(reload){
						استمر;        /* plugin not currently valid for reloading. */
					}
					REQUIRE_NON_DEFAULT_LISTENER(token);

					إذا(conf__parse_string(&token, "plugin_use", &name, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					cur_plugin = config__plugin_find(name);
					إذا(!cur_plugin){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Plugin '%s' not previously loaded.", name);
						mosquitto_FREE(name);
						أرجع MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(name);
					إذا(config__plugin_add_secopt(cur_plugin, cur_listener->security_options)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "auto_id_prefix")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					OPTION_DEPRECATED(token, "Please use 'listener_auto_id_prefix' instead.");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					إذا(conf__parse_string(&token, "auto_id_prefix", &cur_security_options->auto_id_prefix, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(cur_security_options->auto_id_prefix){
						إذا(strlen(cur_security_options->auto_id_prefix) > 50){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: auto_id_prefix length must be <= 50.");
							أرجع MOSQ_ERR_INVAL;
						}
						cur_security_options->auto_id_prefix_len = (uint16_t)strlen(cur_security_options->auto_id_prefix);
					}وإلا{
						cur_security_options->auto_id_prefix_len = 0;
					}
				}وإلا إذا(!strcmp(token, "autosave_interval")){
					إذا(conf__parse_int(&token, "autosave_interval", &config->autosave_interval, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(config->autosave_interval < 0){
						config->autosave_interval = 0;
					}
				}وإلا إذا(!strcmp(token, "autosave_on_changes")){
					إذا(conf__parse_bool(&token, "autosave_on_changes", &config->autosave_on_changes, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "bind_address")){
					OPTION_DEPRECATED(token, "");
					config->local_only = false;
					إذا(reload){
						استمر;        /* Rebinding listeners not valid during reloading. */

					}
					إذا(config__create_default_listener(config, token)){
						أرجع MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					إذا(conf__parse_string(&token, "default listener bind_address", &config->default_listener->host, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(conf__attempt_resolve(config->default_listener->host, "bind_address", MOSQ_LOG_ERR, "Error")){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "bind_interface")){
#ifdef SO_BINDTODEVICE
					إذا(reload){
						استمر;        /* Rebinding listeners not valid during reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					إذا(conf__parse_string(&token, "bind_interface", &cur_listener->bind_interface, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_ERR, "Error: bind_interface specified but socket option not available.");
					أرجع MOSQ_ERR_INVAL;
#endif
				}وإلا إذا(!strcmp(token, "bridge_attempt_unsubscribe")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_bool(&token, "bridge_attempt_unsubscribe", &cur_bridge->attempt_unsubscribe, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "bridge_cafile")){
#إذا defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					إذا(conf__parse_string(&token, "bridge_cafile", &cur_bridge->tls_cafile, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}وإلا إذا(!strcmp(token, "bridge_alpn")){
#إذا defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_string(&token, "bridge_alpn", &cur_bridge->tls_alpn, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}وإلا إذا(!strcmp(token, "bridge_ciphers")){
#إذا defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_string(&token, "bridge_ciphers", &cur_bridge->tls_ciphers, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}وإلا إذا(!strcmp(token, "bridge_ciphers_tls1.3")){
#إذا defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_string(&token, "bridge_ciphers_tls1.3", &cur_bridge->tls_13_ciphers, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}وإلا إذا(!strcmp(token, "bridge_bind_address")){
#إذا defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_string(&token, "bridge_bind_address", &cur_bridge->bind_address, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "bridge_capath")){
#إذا defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					إذا(conf__parse_string(&token, "bridge_capath", &cur_bridge->tls_capath, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}وإلا إذا(!strcmp(token, "bridge_certfile")){
#إذا defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					إذا(conf__parse_string(&token, "bridge_certfile", &cur_bridge->tls_certfile, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}وإلا إذا(!strcmp(token, "bridge_identity")){
#إذا defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					إذا(conf__parse_string(&token, "bridge_identity", &cur_bridge->tls_psk_identity, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}وإلا إذا(!strcmp(token, "bridge_insecure")){
#إذا defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_bool(&token, "bridge_insecure", &cur_bridge->tls_insecure, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(cur_bridge->tls_insecure){
						log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge '%s' using insecure mode.", cur_bridge->name);
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}وإلا إذا(!strcmp(token, "bridge_require_ocsp")){
#إذا defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_bool(&token, "bridge_require_ocsp", &cur_bridge->tls_ocsp_required, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وإلا إذا(!strcmp(token, "bridge_max_packet_size")){
#إذا defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_int(&token, "bridge_max_packet_size", &tmp_int, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(tmp_int < 0){
						tmp_int = 0;
					}
					cur_bridge->maximum_packet_size = (uint32_t)tmp_int;
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "bridge_max_topic_alias")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_int(&token, "bridge_max_topic_alias", &tmp_int, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}

					إذا(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: bridge_max_topic_alias must be > 0 and <= 65535.");
						أرجع MOSQ_ERR_INVAL;
					}
					cur_bridge->max_topic_alias = (uint16_t)tmp_int;
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "bridge_outgoing_retain")){
#إذا defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_bool(&token, "bridge_outgoing_retain", &cur_bridge->outgoing_retain, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "bridge_keyfile")){
#إذا defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					mosquitto_FREE(cur_bridge->tls_keyfile);
					إذا(conf__parse_string(&token, "bridge_keyfile", &cur_bridge->tls_keyfile, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}وإلا إذا(!strcmp(token, "bridge_protocol_version")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(عدم, "", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_protocol_version");

					إذا(!strcmp(token, "mqttv31")){
						cur_bridge->protocol_version = mosq_p_mqtt31;
					}وإلا إذا(!strcmp(token, "mqttv311")){
						cur_bridge->protocol_version = mosq_p_mqtt311;
					}وإلا إذا(!strcmp(token, "mqttv50")){
						cur_bridge->protocol_version = mosq_p_mqtt5;
					}وإلا{
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'bridge_protocol_version' value (%s).", token);
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "bridge_psk")){
#إذا defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					إذا(conf__parse_string(&token, "bridge_psk", &cur_bridge->tls_psk, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}وإلا إذا(!strcmp(token, "bridge_receive_maximum")){
#إذا defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_int(&token, "bridge_receive_maximum", &tmp_int, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(tmp_int <= 0){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be greater than 0.");
						أرجع MOSQ_ERR_INVAL;
					}وإلا إذا((uint64_t)tmp_int > (uint64_t)UINT16_MAX){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be lower than %u.", UINT16_MAX);
						أرجع MOSQ_ERR_INVAL;
					}
					cur_bridge->receive_maximum = (uint16_t)tmp_int;
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "bridge_reload_type")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(عدم, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_reload_type");

					إذا(!strcmp(token, "lazy")){
						cur_bridge->reload_type = brt_lazy;
					}وإلا إذا(!strcmp(token, "immediate")){
						cur_bridge->reload_type = brt_immediate;
					}وإلا{
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'bridge_reload_type' value in configuration (%s).", token);
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "bridge_session_expiry_interval")){
#إذا defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_int(&token, "bridge_session_expiry_interval", &tmp_int, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(tmp_int < 0){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must not be negative.");
						أرجع MOSQ_ERR_INVAL;
					}وإلا إذا((uint64_t)tmp_int > (uint64_t)MQTT_SESSION_EXPIRY_NEVER){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must be lower than %u.", MQTT_SESSION_EXPIRY_NEVER);
						أرجع MOSQ_ERR_INVAL;
					}
					cur_bridge->session_expiry_interval = (uint32_t)tmp_int;
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "bridge_tcp_keepalive")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);

					إذا(conf__parse_int(&token, "bridge_tcp_keepalive_idle", &tmp_int, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(tmp_int <= 0){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: invalid TCP keepalive idle value.");
						أرجع MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_idle = (unsigned صحيح)tmp_int;

					إذا(conf__parse_int(&token, "bridge_tcp_keepalive_interval", &tmp_int, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(tmp_int <= 0){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: invalid TCP keepalive interval value.");
						أرجع MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_interval = (unsigned صحيح)tmp_int;

					إذا(conf__parse_int(&token, "bridge_tcp_keepalive_counter", &tmp_int, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(tmp_int <= 0){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: invalid TCP keepalive counter value.");
						أرجع MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_counter = (unsigned صحيح)tmp_int;
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "bridge_tcp_user_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
#ifdef WITH_TCP_USER_TIMEOUT
					إذا(conf__parse_int(&token, "bridge_tcp_user_timeout", &tmp_int, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(tmp_int < 0){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: invalid TCP user timeout value.");
						أرجع MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_user_timeout = tmp_int;
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge TCP user timeout support not available.");
#endif
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "bridge_tls_use_os_certs")){
#إذا defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_bool(&token, "bridge_tls_use_os_certs", &cur_bridge->tls_use_os_certs, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}وإلا إذا(!strcmp(token, "bridge_tls_version")){
#إذا defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_string(&token, "bridge_tls_version", &cur_bridge->tls_version, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}وإلا إذا(!strcmp(token, "cafile")){
#إذا defined(WITH_TLS)
					REQUIRE_LISTENER(token);
					إذا(cur_listener->psk_hint){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						أرجع MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->cafile);
					إذا(conf__parse_string(&token, "cafile", &cur_listener->cafile, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وإلا إذا(!strcmp(token, "capath")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->capath);
					إذا(conf__parse_string(&token, "capath", &cur_listener->capath, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وإلا إذا(!strcmp(token, "certfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					إذا(cur_listener->psk_hint){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						أرجع MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->certfile);
					إذا(conf__parse_string(&token, "certfile", &cur_listener->certfile, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وإلا إذا(!strcmp(token, "check_retain_source")){
					إذا(conf__parse_bool(&token, "check_retain_source", &config->check_retain_source, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "ciphers")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers);
					إذا(conf__parse_string(&token, "ciphers", &cur_listener->ciphers, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وإلا إذا(!strcmp(token, "ciphers_tls1.3")){
#إذا defined(WITH_TLS) && (!defined(LIBRESSL_VERSION_نمبر) || LIBRESSL_VERSION_نمبر > 0x3040000FL)
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers_tls13);
					إذا(conf__parse_string(&token, "ciphers_tls1.3", &cur_listener->ciphers_tls13, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: ciphers_tls1.3 support not available.");
#endif
				}وإلا إذا(!strcmp(token, "clientid") || !strcmp(token, "remote_clientid")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_string(&token, "bridge remote clientid", &cur_bridge->remote_clientid, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "cleansession")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_bool(&token, "cleansession", &cur_bridge->clean_start, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "local_cleansession")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_bool(&token, "local_cleansession", (bool *)&cur_bridge->clean_start_local, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "clientid_prefixes")){
					OPTION_DEPRECATED(token, "");
					mosquitto_FREE(config->clientid_prefixes);
					إذا(conf__parse_string(&token, "clientid_prefixes", &config->clientid_prefixes, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "connection")){
#ifdef WITH_BRIDGE
					token = strtok_r(عدم, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "connection");

					/* Check for existing bridge name. */
					لكل(i=0; i<config->bridge_گنتی; i++){
						إذا(!strcmp(config->bridges[i]->name, token)){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Duplicate bridge name '%s'.", token);
							أرجع MOSQ_ERR_INVAL;
						}
					}

					بنية mosquitto__bridge **bridges_new = mosquitto_realloc(config->bridges, (size_t)(config->bridge_گنتی+1)*حجم(بنية mosquitto__bridge *));
					إذا(!bridges_new){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
						أرجع MOSQ_ERR_NOMEM;
					}
					config->bridges = bridges_new;
					cur_bridge = mosquitto_malloc(حجم(بنية mosquitto__bridge));
					إذا(!cur_bridge){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
						أرجع MOSQ_ERR_NOMEM;
					}

					config->bridge_گنتی++;
					config->bridges[config->bridge_گنتی-1] = cur_bridge;

					memset(cur_bridge, 0, حجم(بنية mosquitto__bridge));
					cur_bridge->name = mosquitto_strdup(token);
					إذا(!cur_bridge->name){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
						أرجع MOSQ_ERR_NOMEM;
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
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "connection_messages")){
					إذا(conf__parse_bool(&token, token, &config->connection_messages, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "crlfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->crlfile);
					إذا(conf__parse_string(&token, "crlfile", &cur_listener->crlfile, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وإلا إذا(!strcmp(token, "dhparamfile")){
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: dhparamfile is no longer required.");
				}وإلا إذا(!strcmp(token, "disable_client_cert_date_checks")){
#ifdef WITH_TLS
					REQUIRE_LISTENER(token);
					إذا(conf__parse_bool(&token, "disable_client_cert_date_checks", &cur_listener->disable_client_cert_date_checks, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وإلا إذا(!strcmp(token, "enable_control_api")){
#ifdef WITH_CONTROL
					إذا(conf__parse_bool(&token, "enable_control_api", &config->enable_control_api, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: $CONTROL support not available (enable_control_api).");
#endif
				}وإلا إذا(!strcmp(token, "enable_proxy_protocol")){
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(عدم, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					أرجع MOSQ_ERR_INVAL;
#endif
					REQUIRE_LISTENER(token);
					إذا(conf__parse_int(&token, "enable_proxy_protocol", &cur_listener->enable_proxy_protocol, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(cur_listener->enable_proxy_protocol < 1 || cur_listener->enable_proxy_protocol > 2){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: enable_proxy_protocol must be 1 or 2.");
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "global_max_clients")){
					إذا(conf__parse_int(&token, "global_max_clients", &config->global_max_clients, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "global_max_connections")){
					إذا(conf__parse_int(&token, "global_max_connections", &config->global_max_connections, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "http_dir")){
#إذا defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
					إذا(reload){
						استمر;        /* Not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					إذا(conf__parse_string(&token, "http_dir", &cur_listener->http_dir, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#ifdef WIN32
					حرف *http_dir_canonical = _fullpath(عدم, cur_listener->http_dir, 0);
					ثابت حرف sep = '\\';
#وإلا
					حرف *http_dir_canonical = realpath(cur_listener->http_dir, عدم);
					ثابت حرف sep = '/';
#endif
					إذا(!http_dir_canonical){
						أرجع MOSQ_ERR_NOMEM;
					}
					size_t http_dir_len = strlen(http_dir_canonical) + حجم(sep) + 1;
					حرف *http_dir_canonical_sep = mosquitto_calloc(http_dir_len, حجم(حرف));
					إذا(!http_dir_canonical_sep){
						حرر(http_dir_canonical);
						أرجع MOSQ_ERR_NOMEM;
					}
					snprintf(http_dir_canonical_sep, http_dir_len, "%s%c", http_dir_canonical, sep);
					حرر(http_dir_canonical);
					mosquitto_FREE(cur_listener->http_dir);
					cur_listener->http_dir = http_dir_canonical_sep;
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: http_dir support not available.");
#endif
				}وإلا إذا(!strcmp(token, "idle_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_int(&token, "idle_timeout", &cur_bridge->idle_timeout, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(cur_bridge->idle_timeout < 1){
						log__printf(عدم, MOSQ_LOG_NOTICE, "idle_timeout interval too low, using 1 second.");
						cur_bridge->idle_timeout = 1;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "include_dir")){
					إذا(level == 0){
						حرف **files;
						صحيح file_گنتی;

						/* Only process include_dir from the main config file. */
						token = strtok_r(عدم, "", &saveptr);
						REQUIRE_NON_EMPTY_OPTION(token, "include_dir");

						rc = config__get_dir_files(token, &files, &file_گنتی);
						إذا(rc){
							أرجع rc;
						}

						لكل(i=0; i<file_گنتی; i++){
							log__printf(عدم, MOSQ_LOG_INFO, "Loading config file '%s'", files[i]);

							rc = config__read_file(config, reload, files[i], cr, level+1, &lineno_ext);
							إذا(rc){
								إذا(lineno_ext > 0){
									log__printf(عدم, MOSQ_LOG_ERR, "Error found at '%s:%d'.", files[i], lineno_ext);
								}
								/* Free happens below */
								اكسر;
							}
						}
						لكل(i=0; i<file_گنتی; i++){
							mosquitto_FREE(files[i]);
						}
						mosquitto_FREE(files);
						إذا(rc){
							أرجع rc;    /* This returns if config__read_file() fails above */
						}
					}وإلا{
						log__printf(عدم, MOSQ_LOG_ERR, "Error: The include_dir option is only valid in the main configuration file.");
						أرجع 1;
					}
				}وإلا إذا(!strcmp(token, "keepalive_interval")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_int(&token, "keepalive_interval", &tmp_int, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(tmp_int > UINT16_MAX){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Bridge keepalive value too high.");
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(tmp_int < 5){
						log__printf(عدم, MOSQ_LOG_NOTICE, "keepalive interval too low, using 5 seconds.");
						tmp_int = 5;
					}
					cur_bridge->keepalive = (uint16_t)tmp_int;
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "keyfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->keyfile);
					إذا(conf__parse_string(&token, "keyfile", &cur_listener->keyfile, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وإلا إذا(!strcmp(token, "listener")){
					config->local_only = false;

					إذا(conf__parse_int(&token, "listener port", &tmp_int, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#ifdef WITH_UNIX_SOCKETS
					إذا(tmp_int < 0 || tmp_int > UINT16_MAX){
#وإلا
					إذا(tmp_int < 1 || tmp_int > UINT16_MAX){
#endif
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						أرجع MOSQ_ERR_INVAL;
					}

					/* Look for bind address / unix socket path */
					token = strtok_r(عدم, " ", &saveptr);
					إذا(token != عدم && token[0] == '#'){
						token = عدم;
					}

					إذا(tmp_int == 0 && token == عدم){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: A listener with port 0 must provide a Unix socket path.");
						أرجع MOSQ_ERR_INVAL;
					}

					إذا(reload){
						/* We reload listeners settings based on port number/unix socket path.
						 * If the port number/unix path doesn't already exist, exit with a complaint. */
						cur_listener = عدم;
#ifdef WITH_UNIX_SOCKETS
						إذا(tmp_int == 0){
							لكل(i=0; i<config->listener_گنتی; i++){
								إذا(config->listeners[i].unix_socket_path != عدم
										&& strcmp(config->listeners[i].unix_socket_path, token) == 0){

									cur_listener = &config->listeners[i];
									اكسر;
								}
							}
						}وإلا
#endif
						{
							لكل(i=0; i<config->listener_گنتی; i++){
								إذا(config->listeners[i].port == tmp_int){
									/* Now check we have a matching bind address, if defined */
									إذا(config->listeners[i].host){
										إذا(token && !strcmp(config->listeners[i].host, token)){
											/* They both have a bind address, and they match */
											cur_listener = &config->listeners[i];
											اكسر;
										}
									}وإلا{
										إذا(token == عدم){
											/* Neither this config nor the new config have a bind address,
											 * so they match. */
											cur_listener = &config->listeners[i];
											اكسر;
										}
									}
								}
							}
						}
						إذا(!cur_listener){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: It is not currently possible to add/remove listeners when reloading the config file.");
							أرجع MOSQ_ERR_INVAL;
						}
					}وإلا{
						إذا(config__add_listener(config)){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
							أرجع MOSQ_ERR_NOMEM;
						}
						cur_listener = &config->listeners[config->listener_گنتی-1];
					}

					listener__set_defaults(cur_listener);
					cur_listener->port = (uint16_t)tmp_int;

					mosquitto_FREE(cur_listener->host);

#ifdef WITH_UNIX_SOCKETS
					mosquitto_FREE(cur_listener->unix_socket_path);
#endif

					إذا(token){
#ifdef WITH_UNIX_SOCKETS
						إذا(cur_listener->port == 0){
							cur_listener->unix_socket_path = mosquitto_strdup(token);
						}وإلا
#endif
						{
							cur_listener->host = mosquitto_strdup(token);
						}
					}
				}وإلا إذا(!strcmp(token, "listener_allow_anonymous")){
					REQUIRE_LISTENER(token);
					إذا(conf__parse_bool(&token, "listener_allow_anonymous", (bool *)&cur_listener->security_options->allow_anonymous, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "listener_auto_id_prefix")){
					REQUIRE_LISTENER(token);
					إذا(conf__parse_string(&token, "listener_auto_id_prefix", &cur_listener->security_options->auto_id_prefix, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(cur_listener->security_options->auto_id_prefix){
						إذا(strlen(cur_listener->security_options->auto_id_prefix) > 50){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: 'listener_auto_id_prefix' length must be <= 50.");
							أرجع MOSQ_ERR_INVAL;
						}
						cur_listener->security_options->auto_id_prefix_len = (uint16_t)strlen(cur_listener->security_options->auto_id_prefix);
					}وإلا{
						cur_listener->security_options->auto_id_prefix_len = 0;
					}
				}وإلا إذا(!strcmp(token, "local_clientid")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_string(&token, "bridge local clientd", &cur_bridge->local_clientid, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "local_password")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_string(&token, "bridge local_password", &cur_bridge->local_password, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "local_username")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_string(&token, "bridge local_username", &cur_bridge->local_username, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "log_dest")){
					token = strtok_r(عدم, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_dest");

					cr->log_dest_set = 1;
					إذا(!strcmp(token, "none")){
						cr->log_dest = MQTT3_LOG_NONE;
					}وإلا إذا(!strcmp(token, "syslog")){
						cr->log_dest |= MQTT3_LOG_SYSLOG;
					}وإلا إذا(!strcmp(token, "stdout")){
						cr->log_dest |= MQTT3_LOG_STDOUT;
					}وإلا إذا(!strcmp(token, "stderr")){
						cr->log_dest |= MQTT3_LOG_STDERR;
					}وإلا إذا(!strcmp(token, "topic")){
						cr->log_dest |= MQTT3_LOG_TOPIC;
					}وإلا إذا(!strcmp(token, "dlt")){
						cr->log_dest |= MQTT3_LOG_DLT;
#ifdef ANDROID
					}وإلا إذا(!strcmp(token, "android")){
						cr->log_dest |= MQTT3_LOG_ANDROID;
#endif
					}وإلا إذا(!strcmp(token, "file")){
						إذا(config->log_fptr || config->log_file){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Duplicate \"log_dest file\" value.");
							أرجع MOSQ_ERR_INVAL;
						}
						/* Get remaining string. */
						token = saveptr;
						إذا(token && token[0]){
							طالما(token[0] == ' ' || token[0] == '\t'){
								token++;
							}
						}
						/* Duplicate "token" check here saves a log__printf() */
						إذا(token && token[0]){
							config->log_file = mosquitto_strdup(token);
							إذا(!config->log_file){
								log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
								أرجع MOSQ_ERR_NOMEM;
							}
							cr->log_dest |= MQTT3_LOG_FILE;
						}وإلا{
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Empty \"log_dest file\" value in configuration.");
							أرجع MOSQ_ERR_INVAL;
						}
						cr->log_dest |= MQTT3_LOG_FILE;
					}وإلا{
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'log_dest' value (%s).", token);
						أرجع MOSQ_ERR_INVAL;
					}
#إذا defined(WIN32) || defined(__CYGWIN__)
					إذا(service_handle){
						إذا(cr->log_dest == MQTT3_LOG_STDOUT || cr->log_dest == MQTT3_LOG_STDERR){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Cannot log to stdout/stderr when running as a Windows service.");
							أرجع MOSQ_ERR_INVAL;
						}
					}
#endif
				}وإلا إذا(!strcmp(token, "log_facility")){
#إذا defined(WIN32) || defined(__CYGWIN__)
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: log_facility not supported on Windows.");
#وإلا
					إذا(conf__parse_int(&token, "log_facility", &tmp_int, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
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
							أرجع MOSQ_ERR_INVAL;
					}
#endif
				}وإلا إذا(!strcmp(token, "log_timestamp")){
					إذا(conf__parse_bool(&token, token, &config->log_timestamp, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "log_timestamp_format")){
					إذا(conf__parse_string(&token, token, &config->log_timestamp_format, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "log_type")){
					token = strtok_r(عدم, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_type");

					cr->log_type_set = 1;
					إذا(!strcmp(token, "none")){
						cr->log_type = MOSQ_LOG_NONE;
					}وإلا إذا(!strcmp(token, "information")){
						cr->log_type |= MOSQ_LOG_INFO;
					}وإلا إذا(!strcmp(token, "notice")){
						cr->log_type |= MOSQ_LOG_NOTICE;
					}وإلا إذا(!strcmp(token, "warning")){
						cr->log_type |= MOSQ_LOG_WARNING;
					}وإلا إذا(!strcmp(token, "error")){
						cr->log_type |= MOSQ_LOG_ERR;
					}وإلا إذا(!strcmp(token, "debug")){
						cr->log_type |= MOSQ_LOG_DEBUG;
					}وإلا إذا(!strcmp(token, "subscribe")){
						cr->log_type |= MOSQ_LOG_SUBSCRIBE;
					}وإلا إذا(!strcmp(token, "unsubscribe")){
						cr->log_type |= MOSQ_LOG_UNSUBSCRIBE;
					}وإلا إذا(!strcmp(token, "internal")){
						cr->log_type |= MOSQ_LOG_INTERNAL;
#ifdef WITH_WEBSOCKETS
					}وإلا إذا(!strcmp(token, "websockets")){
						cr->log_type |= MOSQ_LOG_WEBSOCKETS;
#endif
					}وإلا إذا(!strcmp(token, "all")){
						cr->log_type = MOSQ_LOG_ALL;
					}وإلا{
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'log_type' value (%s).", token);
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "max_connections")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					إذا(conf__parse_int(&token, token, &cur_listener->max_connections, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(cur_listener->max_connections < 0){
						cur_listener->max_connections = -1;
					}
				}وإلا إذا(!strcmp(token, "maximum_qos") || !strcmp(token, "max_qos")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					إذا(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(tmp_int < 0 || tmp_int > 2){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: 'max_qos' must be between 0 and 2 inclusive.");
						أرجع MOSQ_ERR_INVAL;
					}
					cur_listener->max_qos = (uint8_t)tmp_int;
				}وإلا إذا(!strcmp(token, "max_inflight_bytes")){
					إذا(conf__parse_int(&token, "max_inflight_bytes", &tmp_int, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_inflight_bytes = (size_t)tmp_int;
				}وإلا إذا(!strcmp(token, "max_inflight_messages")){
					إذا(conf__parse_int(&token, "max_inflight_messages", &tmp_int, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(tmp_int < 0 || tmp_int == UINT16_MAX){
						tmp_int = 0;
					}وإلا إذا(tmp_int > UINT16_MAX){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: 'max_inflight_messages' must be <= 65535.");
						أرجع MOSQ_ERR_INVAL;
					}
					config->max_inflight_messages = (uint16_t)tmp_int;
				}وإلا إذا(!strcmp(token, "max_keepalive")){
					إذا(conf__parse_int(&token, "max_keepalive", &tmp_int, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'max_keepalive' value (%d).", tmp_int);
						أرجع MOSQ_ERR_INVAL;
					}
					config->max_keepalive = (uint16_t)tmp_int;
				}وإلا إذا(!strcmp(token, "max_packet_size")){
					إذا(conf__parse_int(&token, "max_packet_size", &tmp_int, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(tmp_int < 20){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: 'max_packet_size' must be >= 20.");
						أرجع MOSQ_ERR_INVAL;
					}
					config->max_packet_size = (uint32_t)tmp_int;
				}وإلا إذا(!strcmp(token, "max_queued_bytes")){
					إذا(conf__parse_int(&token, "max_queued_bytes", &tmp_int, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_bytes = (size_t)tmp_int;
				}وإلا إذا(!strcmp(token, "max_queued_messages")){
					إذا(conf__parse_int(&token, "max_queued_messages", &tmp_int, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_messages = tmp_int;
				}وإلا إذا(!strcmp(token, "memory_limit")){
					ssize_t lim;
					إذا(conf__parse_ssize_t(&token, "memory_limit", &lim, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(lim < 0){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'memory_limit' value (%ld).", lim);
						أرجع MOSQ_ERR_INVAL;
					}
					mosquitto_memory_set_limit((size_t)lim);
				}وإلا إذا(!strcmp(token, "message_size_limit")){
					log__printf(عدم, MOSQ_LOG_NOTICE, "Note: It is recommended to replace `message_size_limit` with `max_packet_size`.");
					إذا(conf__parse_int(&token, "message_size_limit", (صحيح *)&config->message_size_limit, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(config->message_size_limit > MQTT_MAX_PAYLOAD){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'message_size_limit' value (%u).", config->message_size_limit);
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "mount_point")){
					REQUIRE_LISTENER(token);
					mosquitto_FREE(cur_listener->mount_point);
					إذا(conf__parse_string(&token, "mount_point", &cur_listener->mount_point, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(mosquitto_pub_topic_check(cur_listener->mount_point) != MOSQ_ERR_SUCCESS){
						log__printf(عدم, MOSQ_LOG_ERR,
								"Error: Invalid 'mount_point' value (%s). Does it contain a wildcard character?",
								cur_listener->mount_point);
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "notifications")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_bool(&token, "notifications", &cur_bridge->notifications, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "notifications_local_only")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_bool(&token, "notifications_local_only", &cur_bridge->notifications_local_only, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "notification_topic")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_string(&token, "notification_topic", &cur_bridge->notification_topic, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "password") || !strcmp(token, "remote_password")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_string(&token, "bridge remote_password", &cur_bridge->remote_password, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "password_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->password_data.password_file);
					إذا(conf__parse_string(&token, "password_file", &cur_security_options->password_data.password_file, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "per_listener_settings")){
					OPTION_DEPRECATED(token, "Please see the documentation for how to achieve the same effect.");
					إذا(config->per_listener_settings){
						/* Once this is set, don't let it be unset. It should be the first config option ideally. */
						استمر;
					}
					إذا(conf__parse_bool(&token, "per_listener_settings", &config->per_listener_settings, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(cur_security_options && config->per_listener_settings){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: per_listener_settings must be set before any other security settings.");
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "persistence") || !strcmp(token, "retained_persistence")){
					إذا(conf__parse_bool(&token, token, &config->persistence, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "persistence_file")){
					إذا(conf__parse_string(&token, "persistence_file", &config->persistence_file, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "persistence_location")){
					إذا(conf__parse_string(&token, "persistence_location", &config->persistence_location, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "persistent_client_expiration")){
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
							أرجع MOSQ_ERR_INVAL;
					}
					token[strlen(token)-1] = '\0';
					config->persistent_client_expiration = atoi(token)*expiration_mult;
					إذا(config->persistent_client_expiration <= 0){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "pid_file")){
					إذا(reload){
						استمر;        /* pid file not valid for reloading. */
					}
					إذا(conf__parse_string(&token, "pid_file", &config->pid_file, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "port")){
					OPTION_DEPRECATED(token, "Please use 'listener' instead.");
					config->local_only = false;
					إذا(reload){
						استمر;        /* Listeners not valid for reloading. */

					}
					إذا(config__create_default_listener(config, token)){
						أرجع MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					إذا(config->default_listener->port){
						log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Default listener port specified multiple times. Only the latest will be used.");
					}
					إذا(conf__parse_int(&token, "port", &tmp_int, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(tmp_int < 1 || tmp_int > UINT16_MAX){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						أرجع MOSQ_ERR_INVAL;
					}
					config->default_listener->port = (uint16_t)tmp_int;
				}وإلا إذا(!strcmp(token, "protocol")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(عدم, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "protocol");

					إذا(!strcmp(token, "mqtt")){
						cur_listener->protocol = mp_mqtt;
						/*
						}else if(!strcmp(token, "mqttsn")){
						    cur_listener->protocol = mp_mqttsn;
						*/
					}وإلا إذا(!strcmp(token, "websockets")){
#ifdef WITH_WEBSOCKETS
						cur_listener->protocol = mp_websockets;
#وإلا
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Websockets support not available.");
						أرجع MOSQ_ERR_INVAL;
#endif
					}وإلا إذا(!strcmp(token, "http_api")){
#ifdef WITH_HTTP_API
						cur_listener->protocol = mp_http_api;
#وإلا
						log__printf(عدم, MOSQ_LOG_ERR, "Error: HTTP API support not available.");
						أرجع MOSQ_ERR_INVAL;
#endif
					}وإلا{
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'protocol' value (%s).", token);
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "proxy_protocol_v2_require_tls")){
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(عدم, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					أرجع MOSQ_ERR_INVAL;
#endif
					REQUIRE_LISTENER(token);
					إذا(conf__parse_bool(&token, "proxy_protocol_v2_require_tls", &cur_listener->proxy_protocol_v2_require_tls, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "psk_file")){
#ifdef FINAL_WITH_TLS_PSK
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					إذا(cur_listener && cur_listener->certfile){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						أرجع MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_security_options->psk_file);
					إذا(conf__parse_string(&token, "psk_file", &cur_security_options->psk_file, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#endif
				}وإلا إذا(!strcmp(token, "psk_hint")){
#ifdef FINAL_WITH_TLS_PSK
					إذا(reload){
						استمر;        /* PSK file not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					إذا(conf__parse_string(&token, "psk_hint", &cur_listener->psk_hint, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#endif
				}وإلا إذا(!strcmp(token, "queue_qos0_messages")){
					إذا(conf__parse_bool(&token, token, &config->queue_qos0_messages, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "require_certificate")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					إذا(conf__parse_bool(&token, "require_certificate", &cur_listener->require_certificate, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وإلا إذا(!strcmp(token, "restart_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					cur_bridge->backoff_cap = 0; /* set backoff to constant mode, unless cap is specified further down */
					token = strtok_r(عدم, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "restart_timeout");

					cur_bridge->restart_timeout = atoi(token);
					cur_bridge->backoff_base = 0;
					cur_bridge->backoff_cap = 0;
					إذا(cur_bridge->restart_timeout < 1){
						log__printf(عدم, MOSQ_LOG_NOTICE, "restart_timeout interval too low, using 1 second.");
						cur_bridge->restart_timeout = 1;
					}وإلا إذا(cur_bridge->restart_timeout > 3600){
						log__printf(عدم, MOSQ_LOG_NOTICE, "restart_timeout interval too high, using 3600 seconds.");
						cur_bridge->restart_timeout = 3600;
					}
					token = strtok_r(عدم, " ", &saveptr);
					إذا(token){
						cur_bridge->backoff_base = cur_bridge->restart_timeout;
						cur_bridge->backoff_cap = atoi(token);
						إذا(cur_bridge->backoff_cap < cur_bridge->backoff_base){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: backoff cap is lower than the base in restart_timeout.");
							أرجع MOSQ_ERR_INVAL;
						}وإلا إذا(cur_bridge->backoff_cap > 7200){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: backoff cap too high, using 7200 seconds.");
							cur_bridge->backoff_cap = 7200;
						}

						token = strtok_r(عدم, " ", &saveptr);
						إذا(token){
							cur_bridge->stable_connection_period = atoi(token);
							إذا(cur_bridge->stable_connection_period < 0){
								log__printf(عدم, MOSQ_LOG_ERR, "Error: stable connection period cannot be negative.");
								أرجع MOSQ_ERR_INVAL;
							}
						}
					}
					cur_bridge->restart_timeout *= 1000; /* backoff is tracked in ms */
					cur_bridge->backoff_base *= 1000;
					cur_bridge->backoff_cap *= 1000;
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "retain_available")){
					إذا(conf__parse_bool(&token, token, &config->retain_available, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "retain_expiry_interval")){
					إذا(conf__parse_int(&token, token, &config->retain_expiry_interval, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(config->retain_expiry_interval < 1){
						log__printf(عدم, MOSQ_LOG_WARNING, "Error: retain_expiry_interval must be >= 1.");
						أرجع MOSQ_ERR_INVAL;
					}وإلا إذا(config->retain_expiry_interval > 10000000){
						log__printf(عدم, MOSQ_LOG_WARNING, "Warning: retain_expiry_interval being capped at 19 years.");
						config->retain_expiry_interval = 10000000;
					}
					config->retain_expiry_interval *= 60;
				}وإلا إذا(!strcmp(token, "retry_interval")){
					OPTION_UNAVAILABLE(token);
				}وإلا إذا(!strcmp(token, "round_robin")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_bool(&token, "round_robin", &cur_bridge->round_robin, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "set_tcp_nodelay")){
					إذا(conf__parse_bool(&token, "set_tcp_nodelay", &config->set_tcp_nodelay, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "start_type")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(عدم, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					إذا(!strcmp(token, "automatic")){
						cur_bridge->start_type = bst_automatic;
					}وإلا إذا(!strcmp(token, "lazy")){
						cur_bridge->start_type = bst_lazy;
					}وإلا إذا(!strcmp(token, "manual")){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Manual start_type not supported.");
						أرجع MOSQ_ERR_INVAL;
					}وإلا إذا(!strcmp(token, "once")){
						cur_bridge->start_type = bst_once;
					}وإلا{
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'start_type' value in configuration (%s).", token);
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "socket_domain")){
					إذا(reload){
						استمر;        /* socket_domain not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(عدم, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					إذا(!strcmp(token, "ipv4")){
						cur_listener->socket_domain = AF_INET;
					}وإلا إذا(!strcmp(token, "ipv6")){
						cur_listener->socket_domain = AF_INET6;
					}وإلا{
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'socket_domain' value '%s' in configuration.", token);
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "sys_interval")){
					إذا(conf__parse_int(&token, "sys_interval", &config->sys_interval, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(config->sys_interval < 0 || config->sys_interval > 65535){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'sys_interval' value (%d).", config->sys_interval);
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "threshold")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_int(&token, "threshold", &cur_bridge->threshold, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(cur_bridge->threshold < 1){
						log__printf(عدم, MOSQ_LOG_NOTICE, "threshold too low, using 1 message.");
						cur_bridge->threshold = 1;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "tls_engine")){
#ifdef WITH_TLS
					إذا(reload){
						استمر;        /* tls_engine not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					إذا(conf__parse_string(&token, "tls_engine", &cur_listener->tls_engine, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وإلا إذا(!strcmp(token, "tls_engine_kpass_sha1")){
#ifdef WITH_TLS
					إذا(reload){
						استمر;        /* tls_engine not valid for reloading. */

					}
					حرف *kpass_sha = عدم, *kpass_sha_bin = عدم;
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					إذا(conf__parse_string(&token, "tls_engine_kpass_sha1", &kpass_sha, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(mosquitto__hex2bin_sha1(kpass_sha, (unsigned حرف **)&kpass_sha_bin) != MOSQ_ERR_SUCCESS){
						mosquitto_FREE(kpass_sha);
						أرجع MOSQ_ERR_INVAL;
					}
					mosquitto_free(cur_listener->tls_engine_kpass_sha1);
					cur_listener->tls_engine_kpass_sha1 = kpass_sha_bin;
					mosquitto_FREE(kpass_sha);
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وإلا إذا(!strcmp(token, "tls_keyform")){
#ifdef WITH_TLS
					إذا(reload){
						استمر;        /* tls_engine not valid for reloading. */

					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					حرف *keyform = عدم;
					إذا(conf__parse_string(&token, "tls_keyform", &keyform, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					cur_listener->tls_keyform = mosq_k_pem;
					إذا(!strcmp(keyform, "engine")){
						cur_listener->tls_keyform = mosq_k_engine;
					}
					mosquitto_FREE(keyform);
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وإلا إذا(!strcmp(token, "tls_version")){
#إذا defined(WITH_TLS)
					إذا(reload){
						استمر;        /* tls_version not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					إذا(conf__parse_string(&token, "tls_version", &cur_listener->tls_version, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وإلا إذا(!strcmp(token, "topic")){
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
					إذا(token[0] == '"' && token [1] != '"'){
						إذا(strchr(saveptr, '"') == عدم){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Missing closing quote in topic value (%s).", saveptr);
							أرجع MOSQ_ERR_INVAL;
						}

						حرف *topic_in_quotes = strtok_r(عدم, "\"", &saveptr);
						size_t tlen = 1;
						إذا(topic_in_quotes){
							tlen = strlen(topic_in_quotes);
						}

						topic = mosquitto_malloc(strlen(token) + tlen + 1);
						إذا(!topic){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
							أرجع MOSQ_ERR_NOMEM;
						}

						strcpy(topic, token + 1);
						strcat(topic, " ");
						إذا(topic_in_quotes){
							strcat(topic, topic_in_quotes);
						}
					}وإلا{
						topic = mosquitto_strdup(token);
						إذا(!topic){
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
							أرجع MOSQ_ERR_NOMEM;
						}
					}

					token = strtok_r(عدم, " ", &saveptr);
					إذا(token){
						إذا(!strcasecmp(token, "out")){
							direction = bd_out;
						}وإلا إذا(!strcasecmp(token, "in")){
							direction = bd_in;
						}وإلا إذا(!strcasecmp(token, "both")){
							direction = bd_both;
						}وإلا{
							log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge topic direction '%s'.", token);
							mosquitto_FREE(topic);
							أرجع MOSQ_ERR_INVAL;
						}
						token = strtok_r(عدم, " ", &saveptr);
						إذا(token){
							إذا(token[0] == '#'){
								(فراغ)strtok_r(عدم, "", &saveptr);
							}
							qos = (uint8_t)atoi(token);
							إذا(qos > 2){
								log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid bridge QoS level '%s'.", token);
								mosquitto_FREE(topic);
								أرجع MOSQ_ERR_INVAL;
							}

							token = strtok_r(عدم, " ", &saveptr);
							إذا(token){
								إذا(!strcmp(token, "\"\"") || token[0] == '#'){
									local_prefix = عدم;
									إذا(token[0] == '#'){
										(فراغ)strtok_r(عدم, "", &saveptr);
									}
								}وإلا{
									local_prefix = token;
								}

								token = strtok_r(عدم, " ", &saveptr);
								إذا(token){
									إذا(!strcmp(token, "\"\"") || token[0] == '#'){
										remote_prefix = عدم;
									}وإلا{
										remote_prefix = token;
									}
								}
							}
						}
					}

					إذا(bridge__add_topic(cur_bridge, topic, direction, qos, local_prefix, remote_prefix)){
						mosquitto_FREE(topic);
						أرجع MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(topic);
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "max_topic_alias")){
					REQUIRE_LISTENER(token);
					إذا(conf__parse_int(&token, "max_topic_alias", &tmp_int, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias' value in configuration.");
						أرجع MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias = (uint16_t)tmp_int;
				}وإلا إذا(!strcmp(token, "max_topic_alias_broker")){
					REQUIRE_LISTENER(token);
					إذا(conf__parse_int(&token, "max_topic_alias_broker", &tmp_int, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias_broker' value in configuration.");
						أرجع MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias_broker = (uint16_t)tmp_int;
				}وإلا إذا(!strcmp(token, "try_private")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_bool(&token, "try_private", &cur_bridge->try_private, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "upgrade_outgoing_qos")){
					إذا(conf__parse_bool(&token, token, &config->upgrade_outgoing_qos, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "use_identity_as_username")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					إذا(conf__parse_bool(&token, "use_identity_as_username", &cur_listener->use_identity_as_username, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وإلا إذا(!strcmp(token, "use_subject_as_username")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					إذا(conf__parse_bool(&token, "use_subject_as_username", &cur_listener->use_subject_as_username, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وإلا إذا(!strcmp(token, "user")){
					إذا(reload){
						استمر;        /* Drop privileges user not valid for reloading. */
					}
					mosquitto_FREE(config->user);
					إذا(conf__parse_string(&token, "user", &config->user, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "use_username_as_clientid")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					إذا(conf__parse_bool(&token, "use_username_as_clientid", &cur_listener->use_username_as_clientid, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
				}وإلا إذا(!strcmp(token, "username") || !strcmp(token, "remote_username")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					إذا(conf__parse_string(&token, "bridge remote_username", &cur_bridge->remote_username, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وإلا إذا(!strcmp(token, "websockets_log_level")){
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					إذا(conf__parse_int(&token, "websockets_log_level", &config->websockets_log_level, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
#endif
				}وإلا إذا(!strcmp(token, "websockets_headers_size") || !strcmp(token, "packet_buffer_size")){
					إذا(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					إذا(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(عدم, MOSQ_LOG_WARNING, "Error: Packet buffer size must be between 0 and 65535 inclusive.");
						أرجع MOSQ_ERR_INVAL;
					}
					config->packet_buffer_size = (uint16_t)tmp_int;
				}وإلا إذا(!strcmp(token, "websockets_origin")){
#ifdef WITH_WEBSOCKETS
#  إذا LWS_LIBRARY_VERSION_نمبر >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN
					REQUIRE_LISTENER(token);
					ws_origins = mosquitto_realloc(cur_listener->ws_origins, حجم(حرف *)*(size_t)(cur_listener->ws_origin_گنتی+1));
					إذا(ws_origins == عدم){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
						أرجع MOSQ_ERR_NOMEM;
					}
					ws_origins[cur_listener->ws_origin_گنتی] = عدم;
					cur_listener->ws_origins = ws_origins;
					إذا(conf__parse_string(&token, "websockets_origin", &ws_origins[cur_listener->ws_origin_گنتی], &saveptr)){
						أرجع MOSQ_ERR_INVAL;
					}
					cur_listener->ws_origin_گنتی++;
#  وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: websockets_origin support not available, libwebsockets version is too old.");
#  endif
#وإلا
					log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Websockets support not available.");
#endif
				}وإلا{
					log__printf(عدم, MOSQ_LOG_ERR, "Error: Unknown configuration variable '%s'.", token);
					أرجع MOSQ_ERR_INVAL;
				}
			}
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
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
	إذا(dir){
		closedir(dir);
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Config file '%s' is a directory.", file);
		أرجع 1;
	}
#endif

	fptr = mosquitto_fopen(file, "rt", false);
	إذا(!fptr){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to open config file '%s'.", file);
		أرجع 1;
	}

	buflen = 1000;
	buf = mosquitto_malloc((size_t)buflen);
	إذا(!buf){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		fclose(fptr);
		أرجع MOSQ_ERR_NOMEM;
	}

	rc = config__read_file_core(config, reload, cr, level, lineno, fptr, &buf, &buflen);
	mosquitto_FREE(buf);
	fclose(fptr);

	أرجع rc;
}


ساكن صحيح config__check_proxy(بنية mosquitto__config *config)
{
	لكل(صحيح i=0; i<config->listener_گنتی; i++){
		بنية mosquitto__listener *l = &config->listeners[i];

		إذا(l->enable_proxy_protocol == 2){
#ifdef WITH_TLS
			إذا(l->use_subject_as_username){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: use_subject_as_username cannot be used with `enable_proxy_protocol 2`.");
				أرجع MOSQ_ERR_INVAL;
			}

			إذا(l->certfile || l->keyfile){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: certfile and keyfile cannot be used with `enable_proxy_protocol 2`.");
				أرجع MOSQ_ERR_INVAL;
			}
#endif
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح config__check(بنية mosquitto__config *config)
{
	/* Checks that are easy to make after the config has been loaded. */

	ثابت حرف *id_prefix;
	صحيح id_prefix_len;
	إذا(config->security_options.auto_id_prefix){
		id_prefix = config->security_options.auto_id_prefix;
		id_prefix_len = config->security_options.auto_id_prefix_len;
	}وإلا{
		id_prefix = "auto-";
		id_prefix_len = (صحيح)strlen("auto-");
	}

	/* Default to auto_id_prefix = 'auto-' if none set. */
	لكل(صحيح i=0; i<config->listener_گنتی; i++){
		إذا(!config->listeners[i].security_options->auto_id_prefix){
			config->listeners[i].security_options->auto_id_prefix = mosquitto_strdup(id_prefix);
			إذا(!config->listeners[i].security_options->auto_id_prefix){
				أرجع MOSQ_ERR_NOMEM;
			}
			config->listeners[i].security_options->auto_id_prefix_len = (uint16_t)id_prefix_len;
		}
	}

	أرجع config__check_proxy(config);
}

#ifdef WITH_BRIDGE


ساكن صحيح config__check_bridges(بنية mosquitto__config *config)
{
	بنية mosquitto__bridge *bridge1, *bridge2;
	حرف hostname[256];
	size_t len;

	/* Check for bridge duplicate local_clientid, need to generate missing IDs
	 * first. */
	لكل(صحيح i=0; i<config->bridge_گنتی; i++){
		bridge1 = config->bridges[i];

		إذا(!bridge1->remote_clientid){
			إذا(!gethostname(hostname, 256)){
				len = strlen(hostname) + strlen(bridge1->name) + 2;
				bridge1->remote_clientid = mosquitto_malloc(len);
				إذا(!bridge1->remote_clientid){
					أرجع MOSQ_ERR_NOMEM;
				}
				snprintf(bridge1->remote_clientid, len, "%s.%s", hostname, bridge1->name);
			}وإلا{
				أرجع 1;
			}
		}

		إذا(!bridge1->local_clientid){
			len = strlen(bridge1->remote_clientid) + strlen("local.") + 2;
			bridge1->local_clientid = mosquitto_malloc(len);
			إذا(!bridge1->local_clientid){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
				أرجع MOSQ_ERR_NOMEM;
			}
			snprintf(bridge1->local_clientid, len, "local.%s", bridge1->remote_clientid);
		}
	}

	لكل(صحيح i=0; i<config->bridge_گنتی; i++){
		bridge1 = config->bridges[i];
		لكل(صحيح j=i+1; j<config->bridge_گنتی; j++){
			bridge2 = config->bridges[j];
			إذا(!strcmp(bridge1->local_clientid, bridge2->local_clientid)){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Bridge local_clientid "
						"'%s' is not unique. Try changing or setting the "
						"local_clientid value for one of the bridges.",
						bridge1->local_clientid);
				أرجع MOSQ_ERR_INVAL;
			}
		}
	}

#ifdef WITH_TLS
	/* Check for missing TLS cafile/capath/certfile/keyfile */
	لكل(صحيح i=0; i<config->listener_گنتی; i++){
		bool cafile = !!config->listeners[i].cafile;
		bool capath = !!config->listeners[i].capath;
		bool certfile = !!config->listeners[i].certfile;
		bool keyfile = !!config->listeners[i].keyfile;

		إذا((certfile && !keyfile) || (!certfile && keyfile)){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Both certfile and keyfile must be provided to enable a TLS listener.");
			أرجع MOSQ_ERR_INVAL;
		}
		إذا(cafile && !certfile){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: cafile specified without certfile and keyfile.");
			أرجع MOSQ_ERR_INVAL;
		}
		إذا(capath && !certfile){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: capath specified without certfile and keyfile.");
			أرجع MOSQ_ERR_INVAL;
		}
	}
#endif
	أرجع MOSQ_ERR_SUCCESS;
}
#endif


ساكن صحيح conf__parse_bool(حرف **token, ثابت حرف *name, bool *قدر, حرف **saveptr)
{
	*token = strtok_r(عدم, " ", saveptr);
	إذا(*token){
		إذا(!strcmp(*token, "false") || !strcmp(*token, "0")){
			*قدر = false;
		}وإلا إذا(!strcmp(*token, "true") || !strcmp(*token, "1")){
			*قدر = true;
		}وإلا{
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Invalid '%s' value (%s).", name, *token);
			أرجع MOSQ_ERR_INVAL;
		}
	}وإلا{
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		أرجع MOSQ_ERR_INVAL;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح conf__parse_int(حرف **token, ثابت حرف *name, صحيح *قدر, حرف **saveptr)
{
	*token = strtok_r(عدم, " ", saveptr);
	إذا(*token){
		حرف *endptr = عدم;
		long l = strtol(*token, &endptr, 0);
		إذا(endptr == *token || endptr[0] != '\0'){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: '%s' value not a number.", name);
			أرجع MOSQ_ERR_INVAL;
		}
		إذا(l > INT_MAX || l < INT_MIN){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: '%s' value out of range.", name);
			أرجع MOSQ_ERR_INVAL;
		}
		*قدر = (صحيح)l;
	}وإلا{
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		أرجع MOSQ_ERR_INVAL;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح conf__parse_ssize_t(حرف **token, ثابت حرف *name, ssize_t *قدر, حرف **saveptr)
{
	*token = strtok_r(عدم, " ", saveptr);
	إذا(*token){
		*قدر = atol(*token);
	}وإلا{
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		أرجع MOSQ_ERR_INVAL;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح conf__parse_string(حرف **token, ثابت حرف *name, حرف **قدر, حرف **saveptr)
{
	size_t tlen;

	*token = strtok_r(عدم, "", saveptr);
	إذا(*token){
		إذا(*قدر){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Duplicate '%s' value in configuration.", name);
			أرجع MOSQ_ERR_INVAL;
		}
		/* Deal with multiple spaces at the beginning of the string. */
		*token = mosquitto_trimblanks(*token);
		إذا(strlen(*token) == 0){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
			أرجع MOSQ_ERR_INVAL;
		}

		tlen = strlen(*token);
		إذا(tlen > UINT16_MAX){
			أرجع MOSQ_ERR_INVAL;
		}
		إذا(mosquitto_validate_utf8(*token, (uint16_t)tlen)){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Malformed UTF-8 in configuration.");
			أرجع MOSQ_ERR_INVAL;
		}
		*قدر = mosquitto_strdup(*token);
		إذا(!*قدر){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
			أرجع MOSQ_ERR_NOMEM;
		}
	}وإلا{
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		أرجع MOSQ_ERR_INVAL;
	}
	أرجع MOSQ_ERR_SUCCESS;
}
