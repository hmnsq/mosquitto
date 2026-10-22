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

#include "config.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef WIN32
#інакше
#  include <dirent.h>
#  include <strings.h>
#endif

#ifndef WIN32
#  include <netdb.h>
#  include <sys/socket.h>
#інакше
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#якщо !defined(WIN32) && !defined(__CYGWIN__)
#  include <syslog.h>
#endif

#include "mosquitto_broker_internal.h"
#include "tls_mosq.h"
#include "util_mosq.h"
#include "mosquitto/mqtt_protocol.h"

#include "utlist.h"

struct config_recurse {
	unsigned ціле log_dest;
	ціле log_dest_set;
	unsigned ціле log_type;
	ціле log_type_set;
};

#якщо defined(WIN32) || defined(__CYGWIN__)
#include <windows.h>
extern SERVICE_STATUS_HANDLE service_handle;
#endif


#define REQUIRE_LISTENER(A) \
		do{ \
			якщо(cur_listener == NULL){ \
				log__printf(NULL, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				повернути MOSQ_ERR_INVAL; \
			} \
		}поки(0)

#define REQUIRE_LISTENER_OR_DEFAULT_LISTENER(A) \
		do{ \
			якщо(cur_listener == NULL){ \
				якщо(config__create_default_listener(config, (A))){ \
					повернути MOSQ_ERR_NOMEM; \
				} \
				cur_listener = config->default_listener; \
			} \
		}поки(0)

#define REQUIRE_LISTENER_IF_PER_LISTENER(A) \
		do{ \
			якщо(config->per_listener_settings == true && cur_listener == NULL){ \
				log__printf(NULL, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				повернути MOSQ_ERR_INVAL; \
			} \
		}поки(0)

#define REQUIRE_NON_DEFAULT_LISTENER(A) \
		do{ \
			якщо(cur_listener == config->default_listener || cur_listener == NULL){ \
				log__printf(NULL, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				повернути MOSQ_ERR_INVAL; \
			} \
		}поки(0)

#define REQUIRE_BRIDGE(A) \
		do{ \
			якщо(cur_bridge == NULL){ \
				log__printf(NULL, MOSQ_LOG_ERR, "Error: The '%s' option requires a bridge to be defined first.", (A)); \
				повернути MOSQ_ERR_INVAL; \
			} \
		}поки(0)

#define REQUIRE_PLUGIN(A) \
		do{ \
			якщо(cur_plugin == NULL){ \
				log__printf(NULL, MOSQ_LOG_ERR, "Error: The '%s' option requires plugin/global_plugin/plugin_load to be defined first.", (A)); \
				повернути MOSQ_ERR_INVAL; \
			} \
		}поки(0)

#define OPTION_DEPRECATED(A, B) \
		log__printf(NULL, MOSQ_LOG_NOTICE, "The '%s' option is now deprecated and will be removed in version 3.0. %s", (A), (B))

#define OPTION_UNAVAILABLE(A) \
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: The '%s' option is no longer available.", (A));

#define REQUIRE_NON_EMPTY_OPTION(A, B) \
		do{ \
			якщо(!(A)){ \
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", (B)); \
				повернути MOSQ_ERR_INVAL; \
			} \
		}поки(0)

#define PER_LISTENER_ALTERNATIVE(A, B) \
		якщо(config->per_listener_settings){ \
			log__printf(NULL, MOSQ_LOG_NOTICE, "You are using the '%s' option with 'per_listener_settings true'. Please replace this with '%s'.", A, B); \
		} \


#ifdef FINAL_WITH_TLS_PSK
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A) \
		do{ \
			якщо(cur_bridge->tls_psk_identity || cur_bridge->tls_psk){ \
				log__printf(NULL, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and psk encryption in a single bridge.", (A)); \
				повернути MOSQ_ERR_INVAL; \
			} \
		}поки(0)

#  define REQUIRE_BRIDGE_NO_X509(A) \
		do{ \
			якщо(cur_bridge->tls_cafile || cur_bridge->tls_capath || cur_bridge->tls_certfile || cur_bridge->tls_keyfile){ \
				log__printf(NULL, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and identity encryption in a single bridge.", (A)); \
				повернути MOSQ_ERR_INVAL; \
			} \
		}поки(0)

#інакше
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A)
#  define REQUIRE_BRIDGE_NO_X509(A)
#endif

static struct mosquitto__security_options *cur_security_options = NULL;

static ціле conf__parse_bool(char **token, const char *name, bool *value, char **saveptr);
static ціле conf__parse_int(char **token, const char *name, ціле *value, char **saveptr);
static ціле conf__parse_ssize_t(char **token, const char *name, ssize_t *value, char **saveptr);
static ціле conf__parse_string(char **token, const char *name, char **value, char **saveptr);
static ціле config__read_file(struct mosquitto__config *config, bool reload, const char *file, struct config_recurse *config_tmp, ціле level, ціле *lineno);
static ціле config__check(struct mosquitto__config *config);
static void config__cleanup_plugins(void);
#ifdef WITH_BRIDGE
static ціле config__check_bridges(struct mosquitto__config *config);
#endif


static ціле config__add_listener(struct mosquitto__config *config)
{
	struct mosquitto__listener *listener;
	struct mosquitto__listener *new_listeners;
	ціле def_listener = -1;

	якщо(config->default_listener){
		для(ціле i=0; i<config->listener_count; i++){
			якщо(&config->listeners[i] == config->default_listener){
				def_listener = i;
				break;
			}
		}
	}
	new_listeners = mosquitto_realloc(config->listeners, sizeof(struct mosquitto__listener)*(size_t)(config->listener_count+1));
	якщо(!new_listeners){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		повернути MOSQ_ERR_NOMEM;
	}
	config->listeners = new_listeners;
	listener = &config->listeners[config->listener_count];
	memset(listener, 0, sizeof(struct mosquitto__listener));
	listener->security_options = mosquitto_calloc(1, sizeof(struct mosquitto__security_options));
	якщо(!listener->security_options){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		повернути MOSQ_ERR_NOMEM;
	}
	якщо(def_listener != -1){
		config->default_listener = &config->listeners[def_listener];
	}
	config->listener_count++;

	повернути MOSQ_ERR_SUCCESS;
}


static ціле config__create_default_listener(struct mosquitto__config *config, const char *option_name)
{
	якщо(config->default_listener){
		повернути MOSQ_ERR_SUCCESS;
	}
	log__printf(NULL, MOSQ_LOG_INFO, "Creating default listener due to '%s' option.", option_name);
	log__printf(NULL, MOSQ_LOG_INFO, "It is best practice to define a 'listener' first. Using the '%s' option without a listener will be disabled in the future.", option_name);
	якщо(config__add_listener(config)){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		повернути MOSQ_ERR_NOMEM;
	}

	config->default_listener = &config->listeners[config->listener_count-1];
	listener__set_defaults(config->default_listener);
	config->default_listener->port = 1883;

	повернути MOSQ_ERR_SUCCESS;
}


static void conf__set_cur_security_options(struct mosquitto__config *config, struct mosquitto__listener **cur_listener, struct mosquitto__security_options **security_options, const char *option_name)
{
	якщо(config->per_listener_settings){
		якщо(*cur_listener == NULL){
			якщо(config__create_default_listener(config, option_name)){
				повернути;
			}
			*cur_listener = config->default_listener;
		}
		(*security_options) = (*cur_listener)->security_options;
	}інакше{
		(*security_options) = &config->security_options;
	}
}


static ціле conf__attempt_resolve(const char *host, const char *text, unsigned ціле log, const char *msg)
{
	struct addrinfo gai_hints;
	struct addrinfo *gai_res;
	ціле rc;

	memset(&gai_hints, 0, sizeof(struct addrinfo));
	gai_hints.ai_family = AF_UNSPEC;
	gai_hints.ai_socktype = SOCK_STREAM;
	gai_res = NULL;
	rc = getaddrinfo(host, NULL, &gai_hints, &gai_res);
	якщо(gai_res){
		freeaddrinfo(gai_res);
	}
	якщо(rc != 0){
#ifndef WIN32
		якщо(rc == EAI_SYSTEM){
			якщо(errno == ENOENT){
				log__printf(NULL, log, "%s: Unable to resolve %s %s.", msg, text, host);
			}інакше{
				log__printf(NULL, log, "%s: Error resolving %s: %s.", msg, text, strerror(errno));
			}
		}інакше{
			log__printf(NULL, log, "%s: Error resolving %s: %s.", msg, text, gai_strerror(rc));
		}
#інакше
		якщо(rc == WSAHOST_NOT_FOUND){
			log__printf(NULL, log, "%s: Error resolving %s.", msg, text);
		}
#endif
		повернути MOSQ_ERR_INVAL;
	}
	повернути MOSQ_ERR_SUCCESS;
}


static void config__init_reload(struct mosquitto__config *config)
{
	/* Set defaults */
	для(ціле i=0; i<config->listener_count; i++){
		listener__set_defaults(&config->listeners[i]);
	}

	config->local_only = true;
	config->allow_duplicate_messages = true;

	mosquitto_FREE(config->security_options.acl_data.acl_file);
	mosquitto_FREE(config->security_options.password_data.password_file);
	mosquitto_FREE(config->security_options.psk_file);

	config->security_options.allow_anonymous = -1;
	config->security_options.allow_zero_length_clientid = true;
	config->security_options.auto_id_prefix = NULL;
	config->security_options.auto_id_prefix_len = 0;

	config->autosave_interval = 1800;
	config->autosave_on_changes = false;

	mosquitto_FREE(config->clientid_prefixes);

	config->connection_messages = true;
	config->clientid_prefixes = NULL;
	config->per_listener_settings = false;
	якщо(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = NULL;
	}
	mosquitto_FREE(config->log_file);

#якщо defined(WIN32) || defined(__CYGWIN__)
	якщо(service_handle){
		/* This is running as a Windows service. Default to no logging. Using
		 * stdout/stderr is forbidden because the first clients to connect will
		 * get log information sent to them for some reason. */
		config->log_dest = MQTT3_LOG_NONE;
	}інакше{
		config->log_dest = MQTT3_LOG_STDERR;
	}
#інакше
	config->log_facility = LOG_DAEMON;
	config->log_dest = MQTT3_LOG_STDERR | MQTT3_LOG_DLT;
	якщо(db.quiet){
		config->log_type = 0;
	}інакше якщо(db.verbose){
		config->log_type = UINT_MAX;
	}інакше{
		config->log_type = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;
	}
#endif
	config->log_timestamp = true;
	mosquitto_FREE(config->log_timestamp_format);
	config->global_max_clients = -1;
	config->global_max_connections = -1;
	config->log_timestamp_format = NULL;
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


static void config__cleanup_plugin_config(mosquitto_plugin_id_t *plugin)
{
	mosquitto_FREE(plugin->config.path);
	mosquitto_FREE(plugin->config.name);

	якщо(plugin->config.options){
		для(ціле j=0; j<plugin->config.option_count; j++){
			mosquitto_FREE(plugin->config.options[j].key);
			mosquitto_FREE(plugin->config.options[j].value);
		}
		mosquitto_FREE(plugin->config.options);
		plugin->config.option_count = 0;
	}
	mosquitto_FREE(plugin->config.security_options);
	mosquitto_FREE(plugin);
}


static void config__cleanup_plugins(void)
{
	для(ціле i=0; i<db.plugin_count; i++){
		config__cleanup_plugin_config(db.plugins[i]);
	}
	mosquitto_FREE(db.plugins);
}


void config__init(struct mosquitto__config *config)
{
	memset(config, 0, sizeof(struct mosquitto__config));
	config__init_reload(config);

	config->daemon = false;
}


void config__cleanup(struct mosquitto__config *config)
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
	якщо(config->listeners){
		для(ціле i=0; i<config->listener_count; i++){
			mosquitto_FREE(config->listeners[i].host);
			mosquitto_FREE(config->listeners[i].bind_interface);
			mosquitto_FREE(config->listeners[i].mount_point);
			mosquitto_FREE(config->listeners[i].socks);
			якщо(config->listeners[i].security_options){
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
#якщо defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			якщо(!config->listeners[i].ws_context) /* libwebsockets frees its own SSL_CTX */
#endif
			{
				SSL_CTX_free(config->listeners[i].ssl_ctx);
				config->listeners[i].ssl_ctx = NULL;
			}
#endif
#якщо defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
			mosquitto_FREE(config->listeners[i].http_dir);
#endif
#ifdef WITH_WEBSOCKETS
			для(ціле j=0; j<config->listeners[i].ws_origin_count; j++){
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
	якщо(config->bridges){
		для(ціле i=0; i<config->bridge_count; i++){
			config__bridge_cleanup(config->bridges[i]);
		}
		mosquitto_FREE(config->bridges);
	}
#endif
	config__cleanup_plugins();

	якщо(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = NULL;
	}
	якщо(config->log_file){
		mosquitto_FREE(config->log_file);
		config->log_file = NULL;
	}
}

#ifdef WITH_BRIDGE


void config__bridge_cleanup(struct mosquitto__bridge *bridge)
{
	якщо(bridge == NULL){
		повернути;
	}

	mosquitto_FREE(bridge->name);
	якщо(bridge->addresses){
		для(ціле i=0; i<bridge->address_count; i++){
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
	якщо(bridge->topics){
		struct mosquitto__bridge_topic *cur_topic, *topic_tmp;

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


static void print_version(void)
{
	printf("mosquitto %s\n", VERSION);
	printf("Copyright © 2025 Roger Light.\n");
	printf("License EPL-2.0 OR BSD-3-Clause.\n");
}


static void print_usage(void)
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


ціле config__parse_args(struct mosquitto__config *config, ціле argc, char *argv[])
{
	ціле i;
	ціле port_tmp;

	для(i=1; i<argc; i++){
		якщо(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--config-file")){
			якщо(i<argc-1){
				db.config_file = argv[i+1];

				якщо(config__read(config, false)){
					повернути MOSQ_ERR_INVAL;
				}
			}інакше{
				log__printf(NULL, MOSQ_LOG_ERR, "Error: -c argument given, but no config file specified.");
				повернути MOSQ_ERR_INVAL;
			}
			i++;
		}інакше якщо(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--daemon")){
			config->daemon = true;
		}інакше якщо(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
			print_usage();
			повернути MOSQ_ERR_UNKNOWN;
		}інакше якщо(!strcmp(argv[i], "--version")){
			print_version();
			повернути MOSQ_ERR_UNKNOWN;
		}інакше якщо(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port")){
			якщо(i<argc-1){
				port_tmp = atoi(argv[i+1]);
				якщо(port_tmp<1 || port_tmp>UINT16_MAX){
					log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid port specified (%d).", port_tmp);
					повернути MOSQ_ERR_INVAL;
				}інакше{
					якщо(config->cmd_port_count == CMD_PORT_LIMIT){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Only %d ports can be specified on the command line.", CMD_PORT_LIMIT);
						повернути MOSQ_ERR_INVAL;
					}
					config->cmd_port[config->cmd_port_count] = (uint16_t)port_tmp;
					config->cmd_port_count++;
				}
			}інакше{
				log__printf(NULL, MOSQ_LOG_ERR, "Error: -p argument given, but no port specified.");
				повернути MOSQ_ERR_INVAL;
			}
			i++;
		}інакше якщо(!strcmp(argv[i], "--tls-keylog")){
#ifdef WITH_TLS
			якщо(i<argc-1){
				db.tls_keylog = mosquitto_strdup(argv[i+1]);
				якщо(db.tls_keylog == NULL){
					log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
					повернути MOSQ_ERR_NOMEM;
				}
			}інакше{
				log__printf(NULL, MOSQ_LOG_ERR, "Error: --tls-keylog argument given, but no file specified.");
				повернути MOSQ_ERR_INVAL;
			}
			i++;
#інакше
			fprintf(stderr, "Error: TLS support not available so --tls-keylog is not available.\n");
			повернути MOSQ_ERR_INVAL;
#endif
		}інакше якщо(!strcmp(argv[i], "-q") || !strcmp(argv[i], "--quiet")){
			db.quiet = true;
		}інакше якщо(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")){
			db.verbose = true;
		}інакше якщо(!strcmp(argv[i], "--test-config")){
			config->test_configuration = true;
		}інакше{
			fprintf(stderr, "Error: Unknown option '%s'.\n", argv[i]);
			print_usage();
			повернути MOSQ_ERR_INVAL;
		}
	}

	/* Default to drop to mosquitto user if we are privileged and no user specified. */
	якщо(!config->user){
		config->user = mosquitto_strdup("mosquitto");
		якщо(config->user == NULL){
			повернути MOSQ_ERR_NOMEM;
		}
	}
	якщо(db.quiet){
		config->log_type = 0;
	}інакше якщо(db.verbose){
		config->log_type = UINT_MAX;
	}

	якщо(getenv("MOSQUITTO_PERSISTENCE_LOCATION")){
		mosquitto_FREE(config->persistence_location);
		config->persistence_location = mosquitto_strdup(getenv("MOSQUITTO_PERSISTENCE_LOCATION"));
		якщо(!config->persistence_location){
			повернути MOSQ_ERR_NOMEM;
		}
	}
	повернути config__check(config);
}


static void config__copy(struct mosquitto__config *src, struct mosquitto__config *dest)
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

#якщо defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	dest->websockets_log_level = src->websockets_log_level;
#endif

#ifdef WITH_BRIDGE
	для(ціле i=0; i<dest->bridge_count; i++){
		якщо(dest->bridges[i]){
			config__bridge_cleanup(dest->bridges[i]);
		}
	}
	mosquitto_FREE(dest->bridges);
	dest->bridges = src->bridges;
	dest->bridge_count = src->bridge_count;
#endif
}


ціле config__read(struct mosquitto__config *config, bool reload)
{
	ціле rc = MOSQ_ERR_SUCCESS;
	struct config_recurse cr;
	ціле lineno = 0;
#ifdef WITH_PERSISTENCE
	size_t len;
#endif
	struct mosquitto__config config_reload;
	ціле i;

	якщо(reload){
		memset(&config_reload, 0, sizeof(struct mosquitto__config));
	}

	cr.log_dest = MQTT3_LOG_NONE;
	cr.log_dest_set = 0;
	cr.log_type = MOSQ_LOG_NONE;
	cr.log_type_set = 0;

	якщо(!db.config_file){
		повернути 0;
	}

	якщо(reload){
		/* Re-initialise appropriate config vars to default for reload. */
		config__init_reload(&config_reload);
		config_reload.listeners = config->listeners;
		config_reload.listener_count = config->listener_count;
		cur_security_options = NULL;
		rc = config__read_file(&config_reload, reload, db.config_file, &cr, 0, &lineno);
	}інакше{
		rc = config__read_file(config, reload, db.config_file, &cr, 0, &lineno);
	}
	якщо(rc){
		якщо(lineno > 0){
			log__printf(NULL, MOSQ_LOG_ERR, "Error found at %s:%d.", db.config_file, lineno);
		}
		повернути rc;
	}

	якщо(reload){
		config__copy(&config_reload, config);
	}

	/* If auth/access options are set and allow_anonymous not explicitly set, disallow anon. */
	якщо(config->local_only == false){
		якщо(config->per_listener_settings){
			для(i=0; i<config->listener_count; i++){
				/* Default option if no security options set */
				якщо(config->listeners[i].security_options->allow_anonymous == -1){
					config->listeners[i].security_options->allow_anonymous = false;
				}
			}
		}інакше{
			якщо(config->security_options.allow_anonymous == -1){
				config->security_options.allow_anonymous = false;
			}
		}
	}
#ifdef WITH_PERSISTENCE
	якщо(config->persistence){
		якщо(!config->persistence_file){
			config->persistence_file = mosquitto_strdup("mosquitto.db");
			якщо(!config->persistence_file){
				повернути MOSQ_ERR_NOMEM;
			}
		}
		mosquitto_FREE(config->persistence_filepath);
		якщо(config->persistence_location && strlen(config->persistence_location)){
			len = strlen(config->persistence_location) + strlen(config->persistence_file) + 2;
			config->persistence_filepath = mosquitto_malloc(len);
			якщо(!config->persistence_filepath){
				повернути MOSQ_ERR_NOMEM;
			}
#ifdef WIN32
			snprintf(config->persistence_filepath, len, "%s\\%s", config->persistence_location, config->persistence_file);
#інакше
			snprintf(config->persistence_filepath, len, "%s/%s", config->persistence_location, config->persistence_file);
#endif
		}інакше{
			config->persistence_filepath = mosquitto_strdup(config->persistence_file);
			якщо(!config->persistence_filepath){
				повернути MOSQ_ERR_NOMEM;
			}
		}
	}
#endif
	/* Default to drop to mosquitto user if no other user specified. This must
	 * remain here even though it is covered in config__parse_args() because this
	 * function may be called on its own. */
	якщо(!config->user){
		config->user = mosquitto_strdup("mosquitto");
	}

#ifdef WITH_BRIDGE
	для(i=0; i<config->bridge_count; i++){
		якщо(!config->bridges[i]->name){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: bridge name not defined.");
			повернути MOSQ_ERR_INVAL;
		}
		якщо(config->bridges[i]->addresses  == 0){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no remote addresses defined.");
			повернути MOSQ_ERR_INVAL;
		}
		якщо(config->bridges[i]->topic_count == 0){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no topics defined.");
			повернути MOSQ_ERR_INVAL;
		}
#ifdef FINAL_WITH_TLS_PSK
		якщо(config->bridges[i]->tls_psk && !config->bridges[i]->tls_psk_identity){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_identity.");
			повернути MOSQ_ERR_INVAL;
		}
		якщо(config->bridges[i]->tls_psk_identity && !config->bridges[i]->tls_psk){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_psk.");
			повернути MOSQ_ERR_INVAL;
		}
#endif
	}
#endif

	якщо(cr.log_dest_set){
		config->log_dest = cr.log_dest;
	}
	якщо(db.quiet){
		config->log_type = 0;
	}інакше якщо(db.verbose){
		config->log_type = UINT_MAX;
	}інакше якщо(cr.log_type_set){
		config->log_type = cr.log_type;
	}
#ifdef WITH_BRIDGE
	повернути config__check_bridges(config);
#інакше
	повернути MOSQ_ERR_SUCCESS;
#endif
}


static mosquitto_plugin_id_t *config__plugin_find(const char *name)
{
	якщо(db.plugins && name){
		для(ціле i=0; i<db.plugin_count; i++){
			якщо(db.plugins[i]->config.name && !strcmp(db.plugins[i]->config.name, name)){
				повернути db.plugins[i];
			}
		}
	}
	повернути NULL;
}


static mosquitto_plugin_id_t *config__plugin_load(const char *name, const char *path)
{
	mosquitto_plugin_id_t *plugin = NULL;
	mosquitto_plugin_id_t **plugins = NULL;

	якщо(name && config__plugin_find(name)){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Duplicate plugin name '%s'.", name);
		повернути NULL;
	}

	якщо(!path || !strcmp(path, "")){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Missing plugin path for plugin name '%s'.", name);
		повернути NULL;
	}

	plugin = mosquitto_calloc(1, sizeof(mosquitto_plugin_id_t));
	якщо(!plugin){
		goto error;
	}

	якщо(name){
		plugin->config.name = mosquitto_strdup(name);
	}
	plugin->config.path = mosquitto_strdup(path);
	якщо((name && !plugin->config.name) || !plugin->config.path){
		goto error;
	}
	plugin->config.options = NULL;
	plugin->config.option_count = 0;
	plugin->config.deny_special_chars = true;

	/* Add to db list */
	plugins = mosquitto_realloc(db.plugins, (size_t)(db.plugin_count+1)*sizeof(struct mosquitto__plugin_config *));
	якщо(!plugins){
		goto error;
	}

	plugins[db.plugin_count] = plugin;
	db.plugins = plugins;
	db.plugin_count++;

	повернути plugin;
error:
	log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
	якщо(plugin){
		mosquitto_FREE(plugin->config.name);
		mosquitto_FREE(plugin->config.path);
	}
	mosquitto_FREE(plugin);
	повернути NULL;
}


ціле config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, struct mosquitto__security_options *security_options)
{
	struct mosquitto__security_options **new_options;
	mosquitto_plugin_id_t **new_plugins;

	new_options = mosquitto_realloc(plugin->config.security_options, (size_t)(plugin->config.security_option_count+1)*sizeof(struct mosquitto__security_options *));
	new_plugins = mosquitto_realloc(security_options->plugins, (size_t)(security_options->plugin_count+1)*sizeof(mosquitto_plugin_id_t *));

	якщо(!new_options || !new_plugins){
		mosquitto_FREE(new_options);
		mosquitto_FREE(new_plugins);
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		повернути MOSQ_ERR_NOMEM;
	}

	new_options[plugin->config.security_option_count] = security_options;
	plugin->config.security_options = new_options;
	plugin->config.security_option_count++;

	new_plugins[security_options->plugin_count] = plugin;
	security_options->plugins = new_plugins;
	security_options->plugin_count++;

	повернути MOSQ_ERR_SUCCESS;
}


static ціле config__read_file_core(struct mosquitto__config *config, bool reload, struct config_recurse *cr, ціле level, ціле *lineno, FILE *fptr, char **buf, ціле *buflen)
{
	ціле rc;
	char *token;
	ціле tmp_int;
	char *saveptr = NULL;
#ifdef WITH_BRIDGE
	char *tmp_char;
	struct mosquitto__bridge *cur_bridge = NULL;
#endif
	mosquitto_plugin_id_t *cur_plugin = NULL;

	char *key;
	struct mosquitto__listener *cur_listener = NULL;
	ціле i;
	ціле lineno_ext = 0;
	size_t prefix_len;
	size_t slen;
#ifdef WITH_WEBSOCKETS
	char **ws_origins = NULL;
#endif

	*lineno = 0;

	поки(mosquitto_fgets(buf, buflen, fptr)){
		(*lineno)++;
		якщо((*buf)[0] != '#' && (*buf)[0] != 10 && (*buf)[0] != 13){
			slen = strlen(*buf);
			якщо(slen == 0){
				continue;
			}
			поки((*buf)[slen-1] == 10 || (*buf)[slen-1] == 13){
				(*buf)[slen-1] = 0;
				slen = strlen(*buf);
				якщо(slen == 0){
					continue;
				}
			}
			token = strtok_r((*buf), " ", &saveptr);
			якщо(token){
				якщо(!strcmp(token, "accept_protocol_versions")){
					REQUIRE_NON_DEFAULT_LISTENER(token);
					cur_listener->disable_protocol_v3 = true;
					cur_listener->disable_protocol_v4 = true;
					cur_listener->disable_protocol_v5 = true;
					якщо(saveptr == NULL){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", "accept_protocol_versions");
						повернути MOSQ_ERR_INVAL;
					}
					token = strtok_r(saveptr, ", \t", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "accept_protocol_versions");

					поки(token){
						якщо(!strcmp(token, "3")){
							cur_listener->disable_protocol_v3 = false;
						}інакше якщо(!strcmp(token, "4")){
							cur_listener->disable_protocol_v4 = false;
						}інакше якщо(!strcmp(token, "5")){
							cur_listener->disable_protocol_v5 = false;
						}

						token = strtok_r(NULL, ", \t", &saveptr);
					}
				}інакше якщо(!strcmp(token, "acl_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);

					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->acl_data.acl_file);
					якщо(conf__parse_string(&token, "acl_file", &cur_security_options->acl_data.acl_file, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "address") || !strcmp(token, "addresses")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					якщо(cur_bridge->addresses){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge configuration, 'address' only allowed once.");
						повернути MOSQ_ERR_INVAL;
					}
					поки((token = strtok_r(NULL, " ", &saveptr))){
						якщо(token[0] == '#'){
							break;
						}
						struct bridge_address *new_addresses = mosquitto_realloc(cur_bridge->addresses, sizeof(struct bridge_address)*(size_t)(cur_bridge->address_count+1));
						якщо(!new_addresses){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
							повернути MOSQ_ERR_NOMEM;
						}
						cur_bridge->address_count++;
						cur_bridge->addresses = new_addresses;
						memset(&cur_bridge->addresses[cur_bridge->address_count-1], 0, sizeof(struct bridge_address));
						cur_bridge->addresses[cur_bridge->address_count-1].address = mosquitto_strdup(token);
						якщо(!cur_bridge->addresses[cur_bridge->address_count-1].address){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
							повернути MOSQ_ERR_NOMEM;
						}
					}
					для(i=0; i<cur_bridge->address_count; i++){
						/* cur_bridge->addresses[i].address is now
						 * "address[:port]". If address is an IPv6 address,
						 * then port is required. We must check for the :
						 * backwards. */
						tmp_char = strrchr(cur_bridge->addresses[i].address, ':');
						якщо(tmp_char){
							/* Remove ':', so cur_bridge->addresses[i].address
							 * now just looks like the address. */
							tmp_char[0] = '\0';

							/* The remainder of the string */
							tmp_int = atoi(&tmp_char[1]);
							якщо(tmp_int < 1 || tmp_int > UINT16_MAX){
								log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge port value (%d).", tmp_int);
								повернути MOSQ_ERR_INVAL;
							}
							cur_bridge->addresses[i].port = (uint16_t)tmp_int;
						}інакше{
							cur_bridge->addresses[i].port = 1883;
						}
						conf__attempt_resolve(cur_bridge->addresses[i].address, "bridge address", MOSQ_LOG_WARNING, "Warning");
					}
					якщо(cur_bridge->address_count == 0){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty address value in configuration.");
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "allow_anonymous")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					PER_LISTENER_ALTERNATIVE(token, "listener_allow_anonymous");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					якщо(conf__parse_bool(&token, "allow_anonymous", (bool *)&cur_security_options->allow_anonymous, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "allow_duplicate_messages")){
					OPTION_DEPRECATED(token, "The behaviour will default to true.");
					якщо(conf__parse_bool(&token, "allow_duplicate_messages", &config->allow_duplicate_messages, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "allow_zero_length_clientid")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					якщо(conf__parse_bool(&token, "allow_zero_length_clientid", &cur_security_options->allow_zero_length_clientid, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strncmp(token, "auth_opt_", strlen("auth_opt_")) || !strncmp(token, "plugin_opt_", strlen("plugin_opt_"))){
					якщо(reload){
						continue;        /* Auth plugin not currently valid for reloading. */

					}
					REQUIRE_PLUGIN(token);

					якщо(!strncmp(token, "auth_opt_", strlen("auth_opt_"))){
						prefix_len = strlen("auth_opt_");
					}інакше{
						prefix_len = strlen("plugin_opt_");
					}
					якщо(strlen(token) < prefix_len + 3){
						/* auth_opt_ == 9, + one digit key == 10, + one space == 11, + one value == 12 */
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'plugin_opt_' config option.");
						повернути MOSQ_ERR_INVAL;
					}
					key = mosquitto_strdup(&token[prefix_len]);
					якщо(!key){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
						повернути MOSQ_ERR_NOMEM;
					}інакше якщо(STREMPTY(key)){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty 'plugin_opt_' config option.");
						mosquitto_FREE(key);
						повернути MOSQ_ERR_INVAL;
					}
					token = saveptr;
					якщо(token && token[0]){
						поки(token[0] == ' ' || token[0] == '\t'){
							token++;
						}
						struct mosquitto_opt *new_options = mosquitto_realloc(cur_plugin->config.options, (size_t)(cur_plugin->config.option_count+1)*sizeof(struct mosquitto_opt));
						якщо(!new_options){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
							mosquitto_FREE(key);
							повернути MOSQ_ERR_NOMEM;
						}
						cur_plugin->config.option_count++;
						cur_plugin->config.options = new_options;
						cur_plugin->config.options[cur_plugin->config.option_count-1].key = key;
						cur_plugin->config.options[cur_plugin->config.option_count-1].value = mosquitto_strdup(token);
						якщо(!cur_plugin->config.options[cur_plugin->config.option_count-1].value){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
							повернути MOSQ_ERR_NOMEM;
						}
					}інакше{
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", key);
						mosquitto_FREE(key);
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "auth_plugin") || !strcmp(token, "plugin") || !strcmp(token, "global_plugin")){
					якщо(reload){
						continue;        /* plugin not currently valid for reloading. */
					}
					якщо(!strcmp(token, "global_plugin")){
						cur_security_options = &db.config->security_options;
					}інакше{
						REQUIRE_LISTENER_IF_PER_LISTENER(token);
						conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					}

					cur_plugin = config__plugin_load(NULL, saveptr);
					якщо(cur_plugin == NULL){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(config__plugin_add_secopt(cur_plugin, cur_security_options)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "auth_plugin_deny_special_chars")){
					якщо(reload){
						continue;        /* Auth plugin not currently valid for reloading. */
					}
					якщо(!cur_plugin){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: An auth_plugin_deny_special_chars option exists in the config file without a plugin.");
						повернути MOSQ_ERR_INVAL;
					}
					якщо(conf__parse_bool(&token, "auth_plugin_deny_special_chars", &cur_plugin->config.deny_special_chars, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "plugin_load")){
					char *name = NULL;

					якщо(reload){
						continue;        /* plugin not currently valid for reloading. */

					}
					name = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(name, "plugin_load");

					cur_plugin = config__plugin_load(name, saveptr);
					якщо(cur_plugin == NULL){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "plugin_use")){
					char *name = NULL;

					якщо(reload){
						continue;        /* plugin not currently valid for reloading. */
					}
					REQUIRE_NON_DEFAULT_LISTENER(token);

					якщо(conf__parse_string(&token, "plugin_use", &name, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					cur_plugin = config__plugin_find(name);
					якщо(!cur_plugin){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Plugin '%s' not previously loaded.", name);
						mosquitto_FREE(name);
						повернути MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(name);
					якщо(config__plugin_add_secopt(cur_plugin, cur_listener->security_options)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "auto_id_prefix")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					OPTION_DEPRECATED(token, "Please use 'listener_auto_id_prefix' instead.");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					якщо(conf__parse_string(&token, "auto_id_prefix", &cur_security_options->auto_id_prefix, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(cur_security_options->auto_id_prefix){
						якщо(strlen(cur_security_options->auto_id_prefix) > 50){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: auto_id_prefix length must be <= 50.");
							повернути MOSQ_ERR_INVAL;
						}
						cur_security_options->auto_id_prefix_len = (uint16_t)strlen(cur_security_options->auto_id_prefix);
					}інакше{
						cur_security_options->auto_id_prefix_len = 0;
					}
				}інакше якщо(!strcmp(token, "autosave_interval")){
					якщо(conf__parse_int(&token, "autosave_interval", &config->autosave_interval, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(config->autosave_interval < 0){
						config->autosave_interval = 0;
					}
				}інакше якщо(!strcmp(token, "autosave_on_changes")){
					якщо(conf__parse_bool(&token, "autosave_on_changes", &config->autosave_on_changes, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "bind_address")){
					OPTION_DEPRECATED(token, "");
					config->local_only = false;
					якщо(reload){
						continue;        /* Rebinding listeners not valid during reloading. */

					}
					якщо(config__create_default_listener(config, token)){
						повернути MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					якщо(conf__parse_string(&token, "default listener bind_address", &config->default_listener->host, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(conf__attempt_resolve(config->default_listener->host, "bind_address", MOSQ_LOG_ERR, "Error")){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "bind_interface")){
#ifdef SO_BINDTODEVICE
					якщо(reload){
						continue;        /* Rebinding listeners not valid during reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					якщо(conf__parse_string(&token, "bind_interface", &cur_listener->bind_interface, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_ERR, "Error: bind_interface specified but socket option not available.");
					повернути MOSQ_ERR_INVAL;
#endif
				}інакше якщо(!strcmp(token, "bridge_attempt_unsubscribe")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_bool(&token, "bridge_attempt_unsubscribe", &cur_bridge->attempt_unsubscribe, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "bridge_cafile")){
#якщо defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					якщо(conf__parse_string(&token, "bridge_cafile", &cur_bridge->tls_cafile, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}інакше якщо(!strcmp(token, "bridge_alpn")){
#якщо defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_string(&token, "bridge_alpn", &cur_bridge->tls_alpn, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}інакше якщо(!strcmp(token, "bridge_ciphers")){
#якщо defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_string(&token, "bridge_ciphers", &cur_bridge->tls_ciphers, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}інакше якщо(!strcmp(token, "bridge_ciphers_tls1.3")){
#якщо defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_string(&token, "bridge_ciphers_tls1.3", &cur_bridge->tls_13_ciphers, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}інакше якщо(!strcmp(token, "bridge_bind_address")){
#якщо defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_string(&token, "bridge_bind_address", &cur_bridge->bind_address, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "bridge_capath")){
#якщо defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					якщо(conf__parse_string(&token, "bridge_capath", &cur_bridge->tls_capath, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}інакше якщо(!strcmp(token, "bridge_certfile")){
#якщо defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					якщо(conf__parse_string(&token, "bridge_certfile", &cur_bridge->tls_certfile, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}інакше якщо(!strcmp(token, "bridge_identity")){
#якщо defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					якщо(conf__parse_string(&token, "bridge_identity", &cur_bridge->tls_psk_identity, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}інакше якщо(!strcmp(token, "bridge_insecure")){
#якщо defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_bool(&token, "bridge_insecure", &cur_bridge->tls_insecure, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(cur_bridge->tls_insecure){
						log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge '%s' using insecure mode.", cur_bridge->name);
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}інакше якщо(!strcmp(token, "bridge_require_ocsp")){
#якщо defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_bool(&token, "bridge_require_ocsp", &cur_bridge->tls_ocsp_required, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}інакше якщо(!strcmp(token, "bridge_max_packet_size")){
#якщо defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_int(&token, "bridge_max_packet_size", &tmp_int, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(tmp_int < 0){
						tmp_int = 0;
					}
					cur_bridge->maximum_packet_size = (uint32_t)tmp_int;
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "bridge_max_topic_alias")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_int(&token, "bridge_max_topic_alias", &tmp_int, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}

					якщо(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: bridge_max_topic_alias must be > 0 and <= 65535.");
						повернути MOSQ_ERR_INVAL;
					}
					cur_bridge->max_topic_alias = (uint16_t)tmp_int;
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "bridge_outgoing_retain")){
#якщо defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_bool(&token, "bridge_outgoing_retain", &cur_bridge->outgoing_retain, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "bridge_keyfile")){
#якщо defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					mosquitto_FREE(cur_bridge->tls_keyfile);
					якщо(conf__parse_string(&token, "bridge_keyfile", &cur_bridge->tls_keyfile, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}інакше якщо(!strcmp(token, "bridge_protocol_version")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(NULL, "", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_protocol_version");

					якщо(!strcmp(token, "mqttv31")){
						cur_bridge->protocol_version = mosq_p_mqtt31;
					}інакше якщо(!strcmp(token, "mqttv311")){
						cur_bridge->protocol_version = mosq_p_mqtt311;
					}інакше якщо(!strcmp(token, "mqttv50")){
						cur_bridge->protocol_version = mosq_p_mqtt5;
					}інакше{
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'bridge_protocol_version' value (%s).", token);
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "bridge_psk")){
#якщо defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					якщо(conf__parse_string(&token, "bridge_psk", &cur_bridge->tls_psk, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}інакше якщо(!strcmp(token, "bridge_receive_maximum")){
#якщо defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_int(&token, "bridge_receive_maximum", &tmp_int, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(tmp_int <= 0){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be greater than 0.");
						повернути MOSQ_ERR_INVAL;
					}інакше якщо((uint64_t)tmp_int > (uint64_t)UINT16_MAX){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be lower than %u.", UINT16_MAX);
						повернути MOSQ_ERR_INVAL;
					}
					cur_bridge->receive_maximum = (uint16_t)tmp_int;
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "bridge_reload_type")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_reload_type");

					якщо(!strcmp(token, "lazy")){
						cur_bridge->reload_type = brt_lazy;
					}інакше якщо(!strcmp(token, "immediate")){
						cur_bridge->reload_type = brt_immediate;
					}інакше{
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'bridge_reload_type' value in configuration (%s).", token);
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "bridge_session_expiry_interval")){
#якщо defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_int(&token, "bridge_session_expiry_interval", &tmp_int, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(tmp_int < 0){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must not be negative.");
						повернути MOSQ_ERR_INVAL;
					}інакше якщо((uint64_t)tmp_int > (uint64_t)MQTT_SESSION_EXPIRY_NEVER){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must be lower than %u.", MQTT_SESSION_EXPIRY_NEVER);
						повернути MOSQ_ERR_INVAL;
					}
					cur_bridge->session_expiry_interval = (uint32_t)tmp_int;
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "bridge_tcp_keepalive")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);

					якщо(conf__parse_int(&token, "bridge_tcp_keepalive_idle", &tmp_int, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(tmp_int <= 0){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: invalid TCP keepalive idle value.");
						повернути MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_idle = (unsigned ціле)tmp_int;

					якщо(conf__parse_int(&token, "bridge_tcp_keepalive_interval", &tmp_int, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(tmp_int <= 0){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: invalid TCP keepalive interval value.");
						повернути MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_interval = (unsigned ціле)tmp_int;

					якщо(conf__parse_int(&token, "bridge_tcp_keepalive_counter", &tmp_int, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(tmp_int <= 0){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: invalid TCP keepalive counter value.");
						повернути MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_counter = (unsigned ціле)tmp_int;
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "bridge_tcp_user_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
#ifdef WITH_TCP_USER_TIMEOUT
					якщо(conf__parse_int(&token, "bridge_tcp_user_timeout", &tmp_int, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(tmp_int < 0){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: invalid TCP user timeout value.");
						повернути MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_user_timeout = tmp_int;
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge TCP user timeout support not available.");
#endif
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "bridge_tls_use_os_certs")){
#якщо defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_bool(&token, "bridge_tls_use_os_certs", &cur_bridge->tls_use_os_certs, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}інакше якщо(!strcmp(token, "bridge_tls_version")){
#якщо defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_string(&token, "bridge_tls_version", &cur_bridge->tls_version, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}інакше якщо(!strcmp(token, "cafile")){
#якщо defined(WITH_TLS)
					REQUIRE_LISTENER(token);
					якщо(cur_listener->psk_hint){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						повернути MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->cafile);
					якщо(conf__parse_string(&token, "cafile", &cur_listener->cafile, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}інакше якщо(!strcmp(token, "capath")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->capath);
					якщо(conf__parse_string(&token, "capath", &cur_listener->capath, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}інакше якщо(!strcmp(token, "certfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					якщо(cur_listener->psk_hint){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						повернути MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->certfile);
					якщо(conf__parse_string(&token, "certfile", &cur_listener->certfile, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}інакше якщо(!strcmp(token, "check_retain_source")){
					якщо(conf__parse_bool(&token, "check_retain_source", &config->check_retain_source, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "ciphers")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers);
					якщо(conf__parse_string(&token, "ciphers", &cur_listener->ciphers, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}інакше якщо(!strcmp(token, "ciphers_tls1.3")){
#якщо defined(WITH_TLS) && (!defined(LIBRESSL_VERSION_NUMBER) || LIBRESSL_VERSION_NUMBER > 0x3040000FL)
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers_tls13);
					якщо(conf__parse_string(&token, "ciphers_tls1.3", &cur_listener->ciphers_tls13, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: ciphers_tls1.3 support not available.");
#endif
				}інакше якщо(!strcmp(token, "clientid") || !strcmp(token, "remote_clientid")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_string(&token, "bridge remote clientid", &cur_bridge->remote_clientid, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "cleansession")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_bool(&token, "cleansession", &cur_bridge->clean_start, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "local_cleansession")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_bool(&token, "local_cleansession", (bool *)&cur_bridge->clean_start_local, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "clientid_prefixes")){
					OPTION_DEPRECATED(token, "");
					mosquitto_FREE(config->clientid_prefixes);
					якщо(conf__parse_string(&token, "clientid_prefixes", &config->clientid_prefixes, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "connection")){
#ifdef WITH_BRIDGE
					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "connection");

					/* Check for existing bridge name. */
					для(i=0; i<config->bridge_count; i++){
						якщо(!strcmp(config->bridges[i]->name, token)){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Duplicate bridge name '%s'.", token);
							повернути MOSQ_ERR_INVAL;
						}
					}

					struct mosquitto__bridge **bridges_new = mosquitto_realloc(config->bridges, (size_t)(config->bridge_count+1)*sizeof(struct mosquitto__bridge *));
					якщо(!bridges_new){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
						повернути MOSQ_ERR_NOMEM;
					}
					config->bridges = bridges_new;
					cur_bridge = mosquitto_malloc(sizeof(struct mosquitto__bridge));
					якщо(!cur_bridge){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
						повернути MOSQ_ERR_NOMEM;
					}

					config->bridge_count++;
					config->bridges[config->bridge_count-1] = cur_bridge;

					memset(cur_bridge, 0, sizeof(struct mosquitto__bridge));
					cur_bridge->name = mosquitto_strdup(token);
					якщо(!cur_bridge->name){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
						повернути MOSQ_ERR_NOMEM;
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
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "connection_messages")){
					якщо(conf__parse_bool(&token, token, &config->connection_messages, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "crlfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->crlfile);
					якщо(conf__parse_string(&token, "crlfile", &cur_listener->crlfile, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}інакше якщо(!strcmp(token, "dhparamfile")){
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: dhparamfile is no longer required.");
				}інакше якщо(!strcmp(token, "disable_client_cert_date_checks")){
#ifdef WITH_TLS
					REQUIRE_LISTENER(token);
					якщо(conf__parse_bool(&token, "disable_client_cert_date_checks", &cur_listener->disable_client_cert_date_checks, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}інакше якщо(!strcmp(token, "enable_control_api")){
#ifdef WITH_CONTROL
					якщо(conf__parse_bool(&token, "enable_control_api", &config->enable_control_api, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: $CONTROL support not available (enable_control_api).");
#endif
				}інакше якщо(!strcmp(token, "enable_proxy_protocol")){
#якщо defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(NULL, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					повернути MOSQ_ERR_INVAL;
#endif
					REQUIRE_LISTENER(token);
					якщо(conf__parse_int(&token, "enable_proxy_protocol", &cur_listener->enable_proxy_protocol, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(cur_listener->enable_proxy_protocol < 1 || cur_listener->enable_proxy_protocol > 2){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: enable_proxy_protocol must be 1 or 2.");
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "global_max_clients")){
					якщо(conf__parse_int(&token, "global_max_clients", &config->global_max_clients, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "global_max_connections")){
					якщо(conf__parse_int(&token, "global_max_connections", &config->global_max_connections, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "http_dir")){
#якщо defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
					якщо(reload){
						continue;        /* Not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					якщо(conf__parse_string(&token, "http_dir", &cur_listener->http_dir, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#ifdef WIN32
					char *http_dir_canonical = _fullpath(NULL, cur_listener->http_dir, 0);
					const char sep = '\\';
#інакше
					char *http_dir_canonical = realpath(cur_listener->http_dir, NULL);
					const char sep = '/';
#endif
					якщо(!http_dir_canonical){
						повернути MOSQ_ERR_NOMEM;
					}
					size_t http_dir_len = strlen(http_dir_canonical) + sizeof(sep) + 1;
					char *http_dir_canonical_sep = mosquitto_calloc(http_dir_len, sizeof(char));
					якщо(!http_dir_canonical_sep){
						free(http_dir_canonical);
						повернути MOSQ_ERR_NOMEM;
					}
					snprintf(http_dir_canonical_sep, http_dir_len, "%s%c", http_dir_canonical, sep);
					free(http_dir_canonical);
					mosquitto_FREE(cur_listener->http_dir);
					cur_listener->http_dir = http_dir_canonical_sep;
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: http_dir support not available.");
#endif
				}інакше якщо(!strcmp(token, "idle_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_int(&token, "idle_timeout", &cur_bridge->idle_timeout, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(cur_bridge->idle_timeout < 1){
						log__printf(NULL, MOSQ_LOG_NOTICE, "idle_timeout interval too low, using 1 second.");
						cur_bridge->idle_timeout = 1;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "include_dir")){
					якщо(level == 0){
						char **files;
						ціле file_count;

						/* Only process include_dir from the main config file. */
						token = strtok_r(NULL, "", &saveptr);
						REQUIRE_NON_EMPTY_OPTION(token, "include_dir");

						rc = config__get_dir_files(token, &files, &file_count);
						якщо(rc){
							повернути rc;
						}

						для(i=0; i<file_count; i++){
							log__printf(NULL, MOSQ_LOG_INFO, "Loading config file '%s'", files[i]);

							rc = config__read_file(config, reload, files[i], cr, level+1, &lineno_ext);
							якщо(rc){
								якщо(lineno_ext > 0){
									log__printf(NULL, MOSQ_LOG_ERR, "Error found at '%s:%d'.", files[i], lineno_ext);
								}
								/* Free happens below */
								break;
							}
						}
						для(i=0; i<file_count; i++){
							mosquitto_FREE(files[i]);
						}
						mosquitto_FREE(files);
						якщо(rc){
							повернути rc;    /* This returns if config__read_file() fails above */
						}
					}інакше{
						log__printf(NULL, MOSQ_LOG_ERR, "Error: The include_dir option is only valid in the main configuration file.");
						повернути 1;
					}
				}інакше якщо(!strcmp(token, "keepalive_interval")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_int(&token, "keepalive_interval", &tmp_int, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(tmp_int > UINT16_MAX){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Bridge keepalive value too high.");
						повернути MOSQ_ERR_INVAL;
					}
					якщо(tmp_int < 5){
						log__printf(NULL, MOSQ_LOG_NOTICE, "keepalive interval too low, using 5 seconds.");
						tmp_int = 5;
					}
					cur_bridge->keepalive = (uint16_t)tmp_int;
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "keyfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->keyfile);
					якщо(conf__parse_string(&token, "keyfile", &cur_listener->keyfile, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}інакше якщо(!strcmp(token, "listener")){
					config->local_only = false;

					якщо(conf__parse_int(&token, "listener port", &tmp_int, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#ifdef WITH_UNIX_SOCKETS
					якщо(tmp_int < 0 || tmp_int > UINT16_MAX){
#інакше
					якщо(tmp_int < 1 || tmp_int > UINT16_MAX){
#endif
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						повернути MOSQ_ERR_INVAL;
					}

					/* Look for bind address / unix socket path */
					token = strtok_r(NULL, " ", &saveptr);
					якщо(token != NULL && token[0] == '#'){
						token = NULL;
					}

					якщо(tmp_int == 0 && token == NULL){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: A listener with port 0 must provide a Unix socket path.");
						повернути MOSQ_ERR_INVAL;
					}

					якщо(reload){
						/* We reload listeners settings based on port number/unix socket path.
						 * If the port number/unix path doesn't already exist, exit with a complaint. */
						cur_listener = NULL;
#ifdef WITH_UNIX_SOCKETS
						якщо(tmp_int == 0){
							для(i=0; i<config->listener_count; i++){
								якщо(config->listeners[i].unix_socket_path != NULL
										&& strcmp(config->listeners[i].unix_socket_path, token) == 0){

									cur_listener = &config->listeners[i];
									break;
								}
							}
						}інакше
#endif
						{
							для(i=0; i<config->listener_count; i++){
								якщо(config->listeners[i].port == tmp_int){
									/* Now check we have a matching bind address, if defined */
									якщо(config->listeners[i].host){
										якщо(token && !strcmp(config->listeners[i].host, token)){
											/* They both have a bind address, and they match */
											cur_listener = &config->listeners[i];
											break;
										}
									}інакше{
										якщо(token == NULL){
											/* Neither this config nor the new config have a bind address,
											 * so they match. */
											cur_listener = &config->listeners[i];
											break;
										}
									}
								}
							}
						}
						якщо(!cur_listener){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: It is not currently possible to add/remove listeners when reloading the config file.");
							повернути MOSQ_ERR_INVAL;
						}
					}інакше{
						якщо(config__add_listener(config)){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
							повернути MOSQ_ERR_NOMEM;
						}
						cur_listener = &config->listeners[config->listener_count-1];
					}

					listener__set_defaults(cur_listener);
					cur_listener->port = (uint16_t)tmp_int;

					mosquitto_FREE(cur_listener->host);

#ifdef WITH_UNIX_SOCKETS
					mosquitto_FREE(cur_listener->unix_socket_path);
#endif

					якщо(token){
#ifdef WITH_UNIX_SOCKETS
						якщо(cur_listener->port == 0){
							cur_listener->unix_socket_path = mosquitto_strdup(token);
						}інакше
#endif
						{
							cur_listener->host = mosquitto_strdup(token);
						}
					}
				}інакше якщо(!strcmp(token, "listener_allow_anonymous")){
					REQUIRE_LISTENER(token);
					якщо(conf__parse_bool(&token, "listener_allow_anonymous", (bool *)&cur_listener->security_options->allow_anonymous, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "listener_auto_id_prefix")){
					REQUIRE_LISTENER(token);
					якщо(conf__parse_string(&token, "listener_auto_id_prefix", &cur_listener->security_options->auto_id_prefix, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(cur_listener->security_options->auto_id_prefix){
						якщо(strlen(cur_listener->security_options->auto_id_prefix) > 50){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: 'listener_auto_id_prefix' length must be <= 50.");
							повернути MOSQ_ERR_INVAL;
						}
						cur_listener->security_options->auto_id_prefix_len = (uint16_t)strlen(cur_listener->security_options->auto_id_prefix);
					}інакше{
						cur_listener->security_options->auto_id_prefix_len = 0;
					}
				}інакше якщо(!strcmp(token, "local_clientid")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_string(&token, "bridge local clientd", &cur_bridge->local_clientid, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "local_password")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_string(&token, "bridge local_password", &cur_bridge->local_password, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "local_username")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_string(&token, "bridge local_username", &cur_bridge->local_username, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "log_dest")){
					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_dest");

					cr->log_dest_set = 1;
					якщо(!strcmp(token, "none")){
						cr->log_dest = MQTT3_LOG_NONE;
					}інакше якщо(!strcmp(token, "syslog")){
						cr->log_dest |= MQTT3_LOG_SYSLOG;
					}інакше якщо(!strcmp(token, "stdout")){
						cr->log_dest |= MQTT3_LOG_STDOUT;
					}інакше якщо(!strcmp(token, "stderr")){
						cr->log_dest |= MQTT3_LOG_STDERR;
					}інакше якщо(!strcmp(token, "topic")){
						cr->log_dest |= MQTT3_LOG_TOPIC;
					}інакше якщо(!strcmp(token, "dlt")){
						cr->log_dest |= MQTT3_LOG_DLT;
#ifdef ANDROID
					}інакше якщо(!strcmp(token, "android")){
						cr->log_dest |= MQTT3_LOG_ANDROID;
#endif
					}інакше якщо(!strcmp(token, "file")){
						якщо(config->log_fptr || config->log_file){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Duplicate \"log_dest file\" value.");
							повернути MOSQ_ERR_INVAL;
						}
						/* Get remaining string. */
						token = saveptr;
						якщо(token && token[0]){
							поки(token[0] == ' ' || token[0] == '\t'){
								token++;
							}
						}
						/* Duplicate "token" check here saves a log__printf() */
						якщо(token && token[0]){
							config->log_file = mosquitto_strdup(token);
							якщо(!config->log_file){
								log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
								повернути MOSQ_ERR_NOMEM;
							}
							cr->log_dest |= MQTT3_LOG_FILE;
						}інакше{
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty \"log_dest file\" value in configuration.");
							повернути MOSQ_ERR_INVAL;
						}
						cr->log_dest |= MQTT3_LOG_FILE;
					}інакше{
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'log_dest' value (%s).", token);
						повернути MOSQ_ERR_INVAL;
					}
#якщо defined(WIN32) || defined(__CYGWIN__)
					якщо(service_handle){
						якщо(cr->log_dest == MQTT3_LOG_STDOUT || cr->log_dest == MQTT3_LOG_STDERR){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Cannot log to stdout/stderr when running as a Windows service.");
							повернути MOSQ_ERR_INVAL;
						}
					}
#endif
				}інакше якщо(!strcmp(token, "log_facility")){
#якщо defined(WIN32) || defined(__CYGWIN__)
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: log_facility not supported on Windows.");
#інакше
					якщо(conf__parse_int(&token, "log_facility", &tmp_int, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					switch(tmp_int){
						case 0:
							config->log_facility = LOG_LOCAL0;
							break;
						case 1:
							config->log_facility = LOG_LOCAL1;
							break;
						case 2:
							config->log_facility = LOG_LOCAL2;
							break;
						case 3:
							config->log_facility = LOG_LOCAL3;
							break;
						case 4:
							config->log_facility = LOG_LOCAL4;
							break;
						case 5:
							config->log_facility = LOG_LOCAL5;
							break;
						case 6:
							config->log_facility = LOG_LOCAL6;
							break;
						case 7:
							config->log_facility = LOG_LOCAL7;
							break;
						default:
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'log_facility' value (%d).", tmp_int);
							повернути MOSQ_ERR_INVAL;
					}
#endif
				}інакше якщо(!strcmp(token, "log_timestamp")){
					якщо(conf__parse_bool(&token, token, &config->log_timestamp, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "log_timestamp_format")){
					якщо(conf__parse_string(&token, token, &config->log_timestamp_format, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "log_type")){
					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_type");

					cr->log_type_set = 1;
					якщо(!strcmp(token, "none")){
						cr->log_type = MOSQ_LOG_NONE;
					}інакше якщо(!strcmp(token, "information")){
						cr->log_type |= MOSQ_LOG_INFO;
					}інакше якщо(!strcmp(token, "notice")){
						cr->log_type |= MOSQ_LOG_NOTICE;
					}інакше якщо(!strcmp(token, "warning")){
						cr->log_type |= MOSQ_LOG_WARNING;
					}інакше якщо(!strcmp(token, "error")){
						cr->log_type |= MOSQ_LOG_ERR;
					}інакше якщо(!strcmp(token, "debug")){
						cr->log_type |= MOSQ_LOG_DEBUG;
					}інакше якщо(!strcmp(token, "subscribe")){
						cr->log_type |= MOSQ_LOG_SUBSCRIBE;
					}інакше якщо(!strcmp(token, "unsubscribe")){
						cr->log_type |= MOSQ_LOG_UNSUBSCRIBE;
					}інакше якщо(!strcmp(token, "internal")){
						cr->log_type |= MOSQ_LOG_INTERNAL;
#ifdef WITH_WEBSOCKETS
					}інакше якщо(!strcmp(token, "websockets")){
						cr->log_type |= MOSQ_LOG_WEBSOCKETS;
#endif
					}інакше якщо(!strcmp(token, "all")){
						cr->log_type = MOSQ_LOG_ALL;
					}інакше{
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'log_type' value (%s).", token);
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "max_connections")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					якщо(conf__parse_int(&token, token, &cur_listener->max_connections, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(cur_listener->max_connections < 0){
						cur_listener->max_connections = -1;
					}
				}інакше якщо(!strcmp(token, "maximum_qos") || !strcmp(token, "max_qos")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					якщо(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(tmp_int < 0 || tmp_int > 2){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: 'max_qos' must be between 0 and 2 inclusive.");
						повернути MOSQ_ERR_INVAL;
					}
					cur_listener->max_qos = (uint8_t)tmp_int;
				}інакше якщо(!strcmp(token, "max_inflight_bytes")){
					якщо(conf__parse_int(&token, "max_inflight_bytes", &tmp_int, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_inflight_bytes = (size_t)tmp_int;
				}інакше якщо(!strcmp(token, "max_inflight_messages")){
					якщо(conf__parse_int(&token, "max_inflight_messages", &tmp_int, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(tmp_int < 0 || tmp_int == UINT16_MAX){
						tmp_int = 0;
					}інакше якщо(tmp_int > UINT16_MAX){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: 'max_inflight_messages' must be <= 65535.");
						повернути MOSQ_ERR_INVAL;
					}
					config->max_inflight_messages = (uint16_t)tmp_int;
				}інакше якщо(!strcmp(token, "max_keepalive")){
					якщо(conf__parse_int(&token, "max_keepalive", &tmp_int, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'max_keepalive' value (%d).", tmp_int);
						повернути MOSQ_ERR_INVAL;
					}
					config->max_keepalive = (uint16_t)tmp_int;
				}інакше якщо(!strcmp(token, "max_packet_size")){
					якщо(conf__parse_int(&token, "max_packet_size", &tmp_int, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(tmp_int < 20){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: 'max_packet_size' must be >= 20.");
						повернути MOSQ_ERR_INVAL;
					}
					config->max_packet_size = (uint32_t)tmp_int;
				}інакше якщо(!strcmp(token, "max_queued_bytes")){
					якщо(conf__parse_int(&token, "max_queued_bytes", &tmp_int, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_bytes = (size_t)tmp_int;
				}інакше якщо(!strcmp(token, "max_queued_messages")){
					якщо(conf__parse_int(&token, "max_queued_messages", &tmp_int, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_messages = tmp_int;
				}інакше якщо(!strcmp(token, "memory_limit")){
					ssize_t lim;
					якщо(conf__parse_ssize_t(&token, "memory_limit", &lim, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(lim < 0){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'memory_limit' value (%ld).", lim);
						повернути MOSQ_ERR_INVAL;
					}
					mosquitto_memory_set_limit((size_t)lim);
				}інакше якщо(!strcmp(token, "message_size_limit")){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Note: It is recommended to replace `message_size_limit` with `max_packet_size`.");
					якщо(conf__parse_int(&token, "message_size_limit", (ціле *)&config->message_size_limit, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(config->message_size_limit > MQTT_MAX_PAYLOAD){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'message_size_limit' value (%u).", config->message_size_limit);
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "mount_point")){
					REQUIRE_LISTENER(token);
					mosquitto_FREE(cur_listener->mount_point);
					якщо(conf__parse_string(&token, "mount_point", &cur_listener->mount_point, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(mosquitto_pub_topic_check(cur_listener->mount_point) != MOSQ_ERR_SUCCESS){
						log__printf(NULL, MOSQ_LOG_ERR,
								"Error: Invalid 'mount_point' value (%s). Does it contain a wildcard character?",
								cur_listener->mount_point);
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "notifications")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_bool(&token, "notifications", &cur_bridge->notifications, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "notifications_local_only")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_bool(&token, "notifications_local_only", &cur_bridge->notifications_local_only, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "notification_topic")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_string(&token, "notification_topic", &cur_bridge->notification_topic, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "password") || !strcmp(token, "remote_password")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_string(&token, "bridge remote_password", &cur_bridge->remote_password, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "password_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->password_data.password_file);
					якщо(conf__parse_string(&token, "password_file", &cur_security_options->password_data.password_file, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "per_listener_settings")){
					OPTION_DEPRECATED(token, "Please see the documentation for how to achieve the same effect.");
					якщо(config->per_listener_settings){
						/* Once this is set, don't let it be unset. It should be the first config option ideally. */
						continue;
					}
					якщо(conf__parse_bool(&token, "per_listener_settings", &config->per_listener_settings, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(cur_security_options && config->per_listener_settings){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: per_listener_settings must be set before any other security settings.");
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "persistence") || !strcmp(token, "retained_persistence")){
					якщо(conf__parse_bool(&token, token, &config->persistence, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "persistence_file")){
					якщо(conf__parse_string(&token, "persistence_file", &config->persistence_file, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "persistence_location")){
					якщо(conf__parse_string(&token, "persistence_location", &config->persistence_location, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "persistent_client_expiration")){
					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "persistent_client_expiration");

					time_t expiration_mult;

					switch(token[strlen(token)-1]){
						case 's':
							expiration_mult = 1;
							break;
						case 'h':
							expiration_mult = 3600;
							break;
						case 'd':
							expiration_mult = 86400;
							break;
						case 'w':
							expiration_mult = 86400*7;
							break;
						case 'm':
							expiration_mult = 86400*30;
							break;
						case 'y':
							expiration_mult = 86400*365;
							break;
						default:
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
							повернути MOSQ_ERR_INVAL;
					}
					token[strlen(token)-1] = '\0';
					config->persistent_client_expiration = atoi(token)*expiration_mult;
					якщо(config->persistent_client_expiration <= 0){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "pid_file")){
					якщо(reload){
						continue;        /* pid file not valid for reloading. */
					}
					якщо(conf__parse_string(&token, "pid_file", &config->pid_file, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "port")){
					OPTION_DEPRECATED(token, "Please use 'listener' instead.");
					config->local_only = false;
					якщо(reload){
						continue;        /* Listeners not valid for reloading. */

					}
					якщо(config__create_default_listener(config, token)){
						повернути MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					якщо(config->default_listener->port){
						log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Default listener port specified multiple times. Only the latest will be used.");
					}
					якщо(conf__parse_int(&token, "port", &tmp_int, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(tmp_int < 1 || tmp_int > UINT16_MAX){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						повернути MOSQ_ERR_INVAL;
					}
					config->default_listener->port = (uint16_t)tmp_int;
				}інакше якщо(!strcmp(token, "protocol")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "protocol");

					якщо(!strcmp(token, "mqtt")){
						cur_listener->protocol = mp_mqtt;
						/*
						}else if(!strcmp(token, "mqttsn")){
						    cur_listener->protocol = mp_mqttsn;
						*/
					}інакше якщо(!strcmp(token, "websockets")){
#ifdef WITH_WEBSOCKETS
						cur_listener->protocol = mp_websockets;
#інакше
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Websockets support not available.");
						повернути MOSQ_ERR_INVAL;
#endif
					}інакше якщо(!strcmp(token, "http_api")){
#ifdef WITH_HTTP_API
						cur_listener->protocol = mp_http_api;
#інакше
						log__printf(NULL, MOSQ_LOG_ERR, "Error: HTTP API support not available.");
						повернути MOSQ_ERR_INVAL;
#endif
					}інакше{
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'protocol' value (%s).", token);
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "proxy_protocol_v2_require_tls")){
#якщо defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(NULL, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					повернути MOSQ_ERR_INVAL;
#endif
					REQUIRE_LISTENER(token);
					якщо(conf__parse_bool(&token, "proxy_protocol_v2_require_tls", &cur_listener->proxy_protocol_v2_require_tls, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "psk_file")){
#ifdef FINAL_WITH_TLS_PSK
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					якщо(cur_listener && cur_listener->certfile){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						повернути MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_security_options->psk_file);
					якщо(conf__parse_string(&token, "psk_file", &cur_security_options->psk_file, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#endif
				}інакше якщо(!strcmp(token, "psk_hint")){
#ifdef FINAL_WITH_TLS_PSK
					якщо(reload){
						continue;        /* PSK file not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					якщо(conf__parse_string(&token, "psk_hint", &cur_listener->psk_hint, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#endif
				}інакше якщо(!strcmp(token, "queue_qos0_messages")){
					якщо(conf__parse_bool(&token, token, &config->queue_qos0_messages, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "require_certificate")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					якщо(conf__parse_bool(&token, "require_certificate", &cur_listener->require_certificate, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}інакше якщо(!strcmp(token, "restart_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					cur_bridge->backoff_cap = 0; /* set backoff to constant mode, unless cap is specified further down */
					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "restart_timeout");

					cur_bridge->restart_timeout = atoi(token);
					cur_bridge->backoff_base = 0;
					cur_bridge->backoff_cap = 0;
					якщо(cur_bridge->restart_timeout < 1){
						log__printf(NULL, MOSQ_LOG_NOTICE, "restart_timeout interval too low, using 1 second.");
						cur_bridge->restart_timeout = 1;
					}інакше якщо(cur_bridge->restart_timeout > 3600){
						log__printf(NULL, MOSQ_LOG_NOTICE, "restart_timeout interval too high, using 3600 seconds.");
						cur_bridge->restart_timeout = 3600;
					}
					token = strtok_r(NULL, " ", &saveptr);
					якщо(token){
						cur_bridge->backoff_base = cur_bridge->restart_timeout;
						cur_bridge->backoff_cap = atoi(token);
						якщо(cur_bridge->backoff_cap < cur_bridge->backoff_base){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: backoff cap is lower than the base in restart_timeout.");
							повернути MOSQ_ERR_INVAL;
						}інакше якщо(cur_bridge->backoff_cap > 7200){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: backoff cap too high, using 7200 seconds.");
							cur_bridge->backoff_cap = 7200;
						}

						token = strtok_r(NULL, " ", &saveptr);
						якщо(token){
							cur_bridge->stable_connection_period = atoi(token);
							якщо(cur_bridge->stable_connection_period < 0){
								log__printf(NULL, MOSQ_LOG_ERR, "Error: stable connection period cannot be negative.");
								повернути MOSQ_ERR_INVAL;
							}
						}
					}
					cur_bridge->restart_timeout *= 1000; /* backoff is tracked in ms */
					cur_bridge->backoff_base *= 1000;
					cur_bridge->backoff_cap *= 1000;
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "retain_available")){
					якщо(conf__parse_bool(&token, token, &config->retain_available, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "retain_expiry_interval")){
					якщо(conf__parse_int(&token, token, &config->retain_expiry_interval, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(config->retain_expiry_interval < 1){
						log__printf(NULL, MOSQ_LOG_WARNING, "Error: retain_expiry_interval must be >= 1.");
						повернути MOSQ_ERR_INVAL;
					}інакше якщо(config->retain_expiry_interval > 10000000){
						log__printf(NULL, MOSQ_LOG_WARNING, "Warning: retain_expiry_interval being capped at 19 years.");
						config->retain_expiry_interval = 10000000;
					}
					config->retain_expiry_interval *= 60;
				}інакше якщо(!strcmp(token, "retry_interval")){
					OPTION_UNAVAILABLE(token);
				}інакше якщо(!strcmp(token, "round_robin")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_bool(&token, "round_robin", &cur_bridge->round_robin, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "set_tcp_nodelay")){
					якщо(conf__parse_bool(&token, "set_tcp_nodelay", &config->set_tcp_nodelay, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "start_type")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					якщо(!strcmp(token, "automatic")){
						cur_bridge->start_type = bst_automatic;
					}інакше якщо(!strcmp(token, "lazy")){
						cur_bridge->start_type = bst_lazy;
					}інакше якщо(!strcmp(token, "manual")){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Manual start_type not supported.");
						повернути MOSQ_ERR_INVAL;
					}інакше якщо(!strcmp(token, "once")){
						cur_bridge->start_type = bst_once;
					}інакше{
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'start_type' value in configuration (%s).", token);
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "socket_domain")){
					якщо(reload){
						continue;        /* socket_domain not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					якщо(!strcmp(token, "ipv4")){
						cur_listener->socket_domain = AF_INET;
					}інакше якщо(!strcmp(token, "ipv6")){
						cur_listener->socket_domain = AF_INET6;
					}інакше{
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'socket_domain' value '%s' in configuration.", token);
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "sys_interval")){
					якщо(conf__parse_int(&token, "sys_interval", &config->sys_interval, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(config->sys_interval < 0 || config->sys_interval > 65535){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'sys_interval' value (%d).", config->sys_interval);
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "threshold")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_int(&token, "threshold", &cur_bridge->threshold, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(cur_bridge->threshold < 1){
						log__printf(NULL, MOSQ_LOG_NOTICE, "threshold too low, using 1 message.");
						cur_bridge->threshold = 1;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "tls_engine")){
#ifdef WITH_TLS
					якщо(reload){
						continue;        /* tls_engine not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					якщо(conf__parse_string(&token, "tls_engine", &cur_listener->tls_engine, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}інакше якщо(!strcmp(token, "tls_engine_kpass_sha1")){
#ifdef WITH_TLS
					якщо(reload){
						continue;        /* tls_engine not valid for reloading. */

					}
					char *kpass_sha = NULL, *kpass_sha_bin = NULL;
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					якщо(conf__parse_string(&token, "tls_engine_kpass_sha1", &kpass_sha, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(mosquitto__hex2bin_sha1(kpass_sha, (unsigned char **)&kpass_sha_bin) != MOSQ_ERR_SUCCESS){
						mosquitto_FREE(kpass_sha);
						повернути MOSQ_ERR_INVAL;
					}
					mosquitto_free(cur_listener->tls_engine_kpass_sha1);
					cur_listener->tls_engine_kpass_sha1 = kpass_sha_bin;
					mosquitto_FREE(kpass_sha);
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}інакше якщо(!strcmp(token, "tls_keyform")){
#ifdef WITH_TLS
					якщо(reload){
						continue;        /* tls_engine not valid for reloading. */

					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					char *keyform = NULL;
					якщо(conf__parse_string(&token, "tls_keyform", &keyform, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					cur_listener->tls_keyform = mosq_k_pem;
					якщо(!strcmp(keyform, "engine")){
						cur_listener->tls_keyform = mosq_k_engine;
					}
					mosquitto_FREE(keyform);
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}інакше якщо(!strcmp(token, "tls_version")){
#якщо defined(WITH_TLS)
					якщо(reload){
						continue;        /* tls_version not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					якщо(conf__parse_string(&token, "tls_version", &cur_listener->tls_version, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}інакше якщо(!strcmp(token, "topic")){
#ifdef WITH_BRIDGE
					char *topic = NULL;
					enum mosquitto__bridge_direction direction = bd_out;
					uint8_t qos = 0;
					char *local_prefix = NULL, *remote_prefix = NULL;

					REQUIRE_BRIDGE(token);

					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "topic");

					// Check if the topic is quoted (e.g. for spaces within topic names), but not the
					// special case of ""
					якщо(token[0] == '"' && token [1] != '"'){
						якщо(strchr(saveptr, '"') == NULL){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Missing closing quote in topic value (%s).", saveptr);
							повернути MOSQ_ERR_INVAL;
						}

						char *topic_in_quotes = strtok_r(NULL, "\"", &saveptr);
						size_t tlen = 1;
						якщо(topic_in_quotes){
							tlen = strlen(topic_in_quotes);
						}

						topic = mosquitto_malloc(strlen(token) + tlen + 1);
						якщо(!topic){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
							повернути MOSQ_ERR_NOMEM;
						}

						strcpy(topic, token + 1);
						strcat(topic, " ");
						якщо(topic_in_quotes){
							strcat(topic, topic_in_quotes);
						}
					}інакше{
						topic = mosquitto_strdup(token);
						якщо(!topic){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
							повернути MOSQ_ERR_NOMEM;
						}
					}

					token = strtok_r(NULL, " ", &saveptr);
					якщо(token){
						якщо(!strcasecmp(token, "out")){
							direction = bd_out;
						}інакше якщо(!strcasecmp(token, "in")){
							direction = bd_in;
						}інакше якщо(!strcasecmp(token, "both")){
							direction = bd_both;
						}інакше{
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge topic direction '%s'.", token);
							mosquitto_FREE(topic);
							повернути MOSQ_ERR_INVAL;
						}
						token = strtok_r(NULL, " ", &saveptr);
						якщо(token){
							якщо(token[0] == '#'){
								(void)strtok_r(NULL, "", &saveptr);
							}
							qos = (uint8_t)atoi(token);
							якщо(qos > 2){
								log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge QoS level '%s'.", token);
								mosquitto_FREE(topic);
								повернути MOSQ_ERR_INVAL;
							}

							token = strtok_r(NULL, " ", &saveptr);
							якщо(token){
								якщо(!strcmp(token, "\"\"") || token[0] == '#'){
									local_prefix = NULL;
									якщо(token[0] == '#'){
										(void)strtok_r(NULL, "", &saveptr);
									}
								}інакше{
									local_prefix = token;
								}

								token = strtok_r(NULL, " ", &saveptr);
								якщо(token){
									якщо(!strcmp(token, "\"\"") || token[0] == '#'){
										remote_prefix = NULL;
									}інакше{
										remote_prefix = token;
									}
								}
							}
						}
					}

					якщо(bridge__add_topic(cur_bridge, topic, direction, qos, local_prefix, remote_prefix)){
						mosquitto_FREE(topic);
						повернути MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(topic);
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "max_topic_alias")){
					REQUIRE_LISTENER(token);
					якщо(conf__parse_int(&token, "max_topic_alias", &tmp_int, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias' value in configuration.");
						повернути MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias = (uint16_t)tmp_int;
				}інакше якщо(!strcmp(token, "max_topic_alias_broker")){
					REQUIRE_LISTENER(token);
					якщо(conf__parse_int(&token, "max_topic_alias_broker", &tmp_int, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias_broker' value in configuration.");
						повернути MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias_broker = (uint16_t)tmp_int;
				}інакше якщо(!strcmp(token, "try_private")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_bool(&token, "try_private", &cur_bridge->try_private, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "upgrade_outgoing_qos")){
					якщо(conf__parse_bool(&token, token, &config->upgrade_outgoing_qos, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "use_identity_as_username")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					якщо(conf__parse_bool(&token, "use_identity_as_username", &cur_listener->use_identity_as_username, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}інакше якщо(!strcmp(token, "use_subject_as_username")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					якщо(conf__parse_bool(&token, "use_subject_as_username", &cur_listener->use_subject_as_username, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}інакше якщо(!strcmp(token, "user")){
					якщо(reload){
						continue;        /* Drop privileges user not valid for reloading. */
					}
					mosquitto_FREE(config->user);
					якщо(conf__parse_string(&token, "user", &config->user, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "use_username_as_clientid")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					якщо(conf__parse_bool(&token, "use_username_as_clientid", &cur_listener->use_username_as_clientid, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
				}інакше якщо(!strcmp(token, "username") || !strcmp(token, "remote_username")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					якщо(conf__parse_string(&token, "bridge remote_username", &cur_bridge->remote_username, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}інакше якщо(!strcmp(token, "websockets_log_level")){
#якщо defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					якщо(conf__parse_int(&token, "websockets_log_level", &config->websockets_log_level, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
#endif
				}інакше якщо(!strcmp(token, "websockets_headers_size") || !strcmp(token, "packet_buffer_size")){
					якщо(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					якщо(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(NULL, MOSQ_LOG_WARNING, "Error: Packet buffer size must be between 0 and 65535 inclusive.");
						повернути MOSQ_ERR_INVAL;
					}
					config->packet_buffer_size = (uint16_t)tmp_int;
				}інакше якщо(!strcmp(token, "websockets_origin")){
#ifdef WITH_WEBSOCKETS
#  якщо LWS_LIBRARY_VERSION_NUMBER >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN
					REQUIRE_LISTENER(token);
					ws_origins = mosquitto_realloc(cur_listener->ws_origins, sizeof(char *)*(size_t)(cur_listener->ws_origin_count+1));
					якщо(ws_origins == NULL){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
						повернути MOSQ_ERR_NOMEM;
					}
					ws_origins[cur_listener->ws_origin_count] = NULL;
					cur_listener->ws_origins = ws_origins;
					якщо(conf__parse_string(&token, "websockets_origin", &ws_origins[cur_listener->ws_origin_count], &saveptr)){
						повернути MOSQ_ERR_INVAL;
					}
					cur_listener->ws_origin_count++;
#  інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: websockets_origin support not available, libwebsockets version is too old.");
#  endif
#інакше
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Websockets support not available.");
#endif
				}інакше{
					log__printf(NULL, MOSQ_LOG_ERR, "Error: Unknown configuration variable '%s'.", token);
					повернути MOSQ_ERR_INVAL;
				}
			}
		}
	}
	повернути MOSQ_ERR_SUCCESS;
}


ціле config__read_file(struct mosquitto__config *config, bool reload, const char *file, struct config_recurse *cr, ціле level, ціле *lineno)
{
	ціле rc;
	FILE *fptr = NULL;
	char *buf;
	ціле buflen;
#ifndef WIN32
	DIR *dir;
#endif

#ifndef WIN32
	dir = opendir(file);
	якщо(dir){
		closedir(dir);
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Config file '%s' is a directory.", file);
		повернути 1;
	}
#endif

	fptr = mosquitto_fopen(file, "rt", false);
	якщо(!fptr){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to open config file '%s'.", file);
		повернути 1;
	}

	buflen = 1000;
	buf = mosquitto_malloc((size_t)buflen);
	якщо(!buf){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		fclose(fptr);
		повернути MOSQ_ERR_NOMEM;
	}

	rc = config__read_file_core(config, reload, cr, level, lineno, fptr, &buf, &buflen);
	mosquitto_FREE(buf);
	fclose(fptr);

	повернути rc;
}


static ціле config__check_proxy(struct mosquitto__config *config)
{
	для(ціле i=0; i<config->listener_count; i++){
		struct mosquitto__listener *l = &config->listeners[i];

		якщо(l->enable_proxy_protocol == 2){
#ifdef WITH_TLS
			якщо(l->use_subject_as_username){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: use_subject_as_username cannot be used with `enable_proxy_protocol 2`.");
				повернути MOSQ_ERR_INVAL;
			}

			якщо(l->certfile || l->keyfile){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: certfile and keyfile cannot be used with `enable_proxy_protocol 2`.");
				повернути MOSQ_ERR_INVAL;
			}
#endif
		}
	}

	повернути MOSQ_ERR_SUCCESS;
}


static ціле config__check(struct mosquitto__config *config)
{
	/* Checks that are easy to make after the config has been loaded. */

	const char *id_prefix;
	ціле id_prefix_len;
	якщо(config->security_options.auto_id_prefix){
		id_prefix = config->security_options.auto_id_prefix;
		id_prefix_len = config->security_options.auto_id_prefix_len;
	}інакше{
		id_prefix = "auto-";
		id_prefix_len = (ціле)strlen("auto-");
	}

	/* Default to auto_id_prefix = 'auto-' if none set. */
	для(ціле i=0; i<config->listener_count; i++){
		якщо(!config->listeners[i].security_options->auto_id_prefix){
			config->listeners[i].security_options->auto_id_prefix = mosquitto_strdup(id_prefix);
			якщо(!config->listeners[i].security_options->auto_id_prefix){
				повернути MOSQ_ERR_NOMEM;
			}
			config->listeners[i].security_options->auto_id_prefix_len = (uint16_t)id_prefix_len;
		}
	}

	повернути config__check_proxy(config);
}

#ifdef WITH_BRIDGE


static ціле config__check_bridges(struct mosquitto__config *config)
{
	struct mosquitto__bridge *bridge1, *bridge2;
	char hostname[256];
	size_t len;

	/* Check for bridge duplicate local_clientid, need to generate missing IDs
	 * first. */
	для(ціле i=0; i<config->bridge_count; i++){
		bridge1 = config->bridges[i];

		якщо(!bridge1->remote_clientid){
			якщо(!gethostname(hostname, 256)){
				len = strlen(hostname) + strlen(bridge1->name) + 2;
				bridge1->remote_clientid = mosquitto_malloc(len);
				якщо(!bridge1->remote_clientid){
					повернути MOSQ_ERR_NOMEM;
				}
				snprintf(bridge1->remote_clientid, len, "%s.%s", hostname, bridge1->name);
			}інакше{
				повернути 1;
			}
		}

		якщо(!bridge1->local_clientid){
			len = strlen(bridge1->remote_clientid) + strlen("local.") + 2;
			bridge1->local_clientid = mosquitto_malloc(len);
			якщо(!bridge1->local_clientid){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
				повернути MOSQ_ERR_NOMEM;
			}
			snprintf(bridge1->local_clientid, len, "local.%s", bridge1->remote_clientid);
		}
	}

	для(ціле i=0; i<config->bridge_count; i++){
		bridge1 = config->bridges[i];
		для(ціле j=i+1; j<config->bridge_count; j++){
			bridge2 = config->bridges[j];
			якщо(!strcmp(bridge1->local_clientid, bridge2->local_clientid)){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Bridge local_clientid "
						"'%s' is not unique. Try changing or setting the "
						"local_clientid value for one of the bridges.",
						bridge1->local_clientid);
				повернути MOSQ_ERR_INVAL;
			}
		}
	}

#ifdef WITH_TLS
	/* Check for missing TLS cafile/capath/certfile/keyfile */
	для(ціле i=0; i<config->listener_count; i++){
		bool cafile = !!config->listeners[i].cafile;
		bool capath = !!config->listeners[i].capath;
		bool certfile = !!config->listeners[i].certfile;
		bool keyfile = !!config->listeners[i].keyfile;

		якщо((certfile && !keyfile) || (!certfile && keyfile)){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Both certfile and keyfile must be provided to enable a TLS listener.");
			повернути MOSQ_ERR_INVAL;
		}
		якщо(cafile && !certfile){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: cafile specified without certfile and keyfile.");
			повернути MOSQ_ERR_INVAL;
		}
		якщо(capath && !certfile){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: capath specified without certfile and keyfile.");
			повернути MOSQ_ERR_INVAL;
		}
	}
#endif
	повернути MOSQ_ERR_SUCCESS;
}
#endif


static ціле conf__parse_bool(char **token, const char *name, bool *value, char **saveptr)
{
	*token = strtok_r(NULL, " ", saveptr);
	якщо(*token){
		якщо(!strcmp(*token, "false") || !strcmp(*token, "0")){
			*value = false;
		}інакше якщо(!strcmp(*token, "true") || !strcmp(*token, "1")){
			*value = true;
		}інакше{
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid '%s' value (%s).", name, *token);
			повернути MOSQ_ERR_INVAL;
		}
	}інакше{
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		повернути MOSQ_ERR_INVAL;
	}

	повернути MOSQ_ERR_SUCCESS;
}


static ціле conf__parse_int(char **token, const char *name, ціле *value, char **saveptr)
{
	*token = strtok_r(NULL, " ", saveptr);
	якщо(*token){
		char *endptr = NULL;
		long l = strtol(*token, &endptr, 0);
		якщо(endptr == *token || endptr[0] != '\0'){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: '%s' value not a number.", name);
			повернути MOSQ_ERR_INVAL;
		}
		якщо(l > INT_MAX || l < INT_MIN){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: '%s' value out of range.", name);
			повернути MOSQ_ERR_INVAL;
		}
		*value = (ціле)l;
	}інакше{
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		повернути MOSQ_ERR_INVAL;
	}

	повернути MOSQ_ERR_SUCCESS;
}


static ціле conf__parse_ssize_t(char **token, const char *name, ssize_t *value, char **saveptr)
{
	*token = strtok_r(NULL, " ", saveptr);
	якщо(*token){
		*value = atol(*token);
	}інакше{
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		повернути MOSQ_ERR_INVAL;
	}

	повернути MOSQ_ERR_SUCCESS;
}


static ціле conf__parse_string(char **token, const char *name, char **value, char **saveptr)
{
	size_t tlen;

	*token = strtok_r(NULL, "", saveptr);
	якщо(*token){
		якщо(*value){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Duplicate '%s' value in configuration.", name);
			повернути MOSQ_ERR_INVAL;
		}
		/* Deal with multiple spaces at the beginning of the string. */
		*token = mosquitto_trimblanks(*token);
		якщо(strlen(*token) == 0){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
			повернути MOSQ_ERR_INVAL;
		}

		tlen = strlen(*token);
		якщо(tlen > UINT16_MAX){
			повернути MOSQ_ERR_INVAL;
		}
		якщо(mosquitto_validate_utf8(*token, (uint16_t)tlen)){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Malformed UTF-8 in configuration.");
			повернути MOSQ_ERR_INVAL;
		}
		*value = mosquitto_strdup(*token);
		якщо(!*value){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
			повернути MOSQ_ERR_NOMEM;
		}
	}інакше{
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		повернути MOSQ_ERR_INVAL;
	}
	повернути MOSQ_ERR_SUCCESS;
}
