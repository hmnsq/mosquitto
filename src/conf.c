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
#инако
#  include <dirent.h>
#  include <strings.h>
#endif

#ifndef WIN32
#  include <netdb.h>
#  include <sys/socket.h>
#инако
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#ако !defined(WIN32) && !defined(__CYGWIN__)
#  include <syslog.h>
#endif

#include "mosquitto_broker_internal.h"
#include "tls_mosq.h"
#include "util_mosq.h"
#include "mosquitto/mqtt_protocol.h"

#include "utlist.h"

struct config_recurse {
	unsigned цео log_dest;
	цео log_dest_set;
	unsigned цео log_type;
	цео log_type_set;
};

#ако defined(WIN32) || defined(__CYGWIN__)
#include <windows.h>
extern SERVICE_STATUS_HANDLE service_handle;
#endif


#define REQUIRE_LISTENER(A) \
		do{ \
			ако(cur_listener == NULL){ \
				log__printf(NULL, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				врати MOSQ_ERR_INVAL; \
			} \
		}док(0)

#define REQUIRE_LISTENER_OR_DEFAULT_LISTENER(A) \
		do{ \
			ако(cur_listener == NULL){ \
				ако(config__create_default_listener(config, (A))){ \
					врати MOSQ_ERR_NOMEM; \
				} \
				cur_listener = config->default_listener; \
			} \
		}док(0)

#define REQUIRE_LISTENER_IF_PER_LISTENER(A) \
		do{ \
			ако(config->per_listener_settings == true && cur_listener == NULL){ \
				log__printf(NULL, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				врати MOSQ_ERR_INVAL; \
			} \
		}док(0)

#define REQUIRE_NON_DEFAULT_LISTENER(A) \
		do{ \
			ако(cur_listener == config->default_listener || cur_listener == NULL){ \
				log__printf(NULL, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				врати MOSQ_ERR_INVAL; \
			} \
		}док(0)

#define REQUIRE_BRIDGE(A) \
		do{ \
			ако(cur_bridge == NULL){ \
				log__printf(NULL, MOSQ_LOG_ERR, "Error: The '%s' option requires a bridge to be defined first.", (A)); \
				врати MOSQ_ERR_INVAL; \
			} \
		}док(0)

#define REQUIRE_PLUGIN(A) \
		do{ \
			ако(cur_plugin == NULL){ \
				log__printf(NULL, MOSQ_LOG_ERR, "Error: The '%s' option requires plugin/global_plugin/plugin_load to be defined first.", (A)); \
				врати MOSQ_ERR_INVAL; \
			} \
		}док(0)

#define OPTION_DEPRECATED(A, B) \
		log__printf(NULL, MOSQ_LOG_NOTICE, "The '%s' option is now deprecated and will be removed in version 3.0. %s", (A), (B))

#define OPTION_UNAVAILABLE(A) \
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: The '%s' option is no longer available.", (A));

#define REQUIRE_NON_EMPTY_OPTION(A, B) \
		do{ \
			ако(!(A)){ \
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", (B)); \
				врати MOSQ_ERR_INVAL; \
			} \
		}док(0)

#define PER_LISTENER_ALTERNATIVE(A, B) \
		ако(config->per_listener_settings){ \
			log__printf(NULL, MOSQ_LOG_NOTICE, "You are using the '%s' option with 'per_listener_settings true'. Please replace this with '%s'.", A, B); \
		} \


#ifdef FINAL_WITH_TLS_PSK
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A) \
		do{ \
			ако(cur_bridge->tls_psk_identity || cur_bridge->tls_psk){ \
				log__printf(NULL, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and psk encryption in a single bridge.", (A)); \
				врати MOSQ_ERR_INVAL; \
			} \
		}док(0)

#  define REQUIRE_BRIDGE_NO_X509(A) \
		do{ \
			ако(cur_bridge->tls_cafile || cur_bridge->tls_capath || cur_bridge->tls_certfile || cur_bridge->tls_keyfile){ \
				log__printf(NULL, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and identity encryption in a single bridge.", (A)); \
				врати MOSQ_ERR_INVAL; \
			} \
		}док(0)

#инако
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A)
#  define REQUIRE_BRIDGE_NO_X509(A)
#endif

static struct mosquitto__security_options *cur_security_options = NULL;

static цео conf__parse_bool(char **token, const char *name, bool *value, char **saveptr);
static цео conf__parse_int(char **token, const char *name, цео *value, char **saveptr);
static цео conf__parse_ssize_t(char **token, const char *name, ssize_t *value, char **saveptr);
static цео conf__parse_string(char **token, const char *name, char **value, char **saveptr);
static цео config__read_file(struct mosquitto__config *config, bool reload, const char *file, struct config_recurse *config_tmp, цео level, цео *lineno);
static цео config__check(struct mosquitto__config *config);
static void config__cleanup_plugins(void);
#ifdef WITH_BRIDGE
static цео config__check_bridges(struct mosquitto__config *config);
#endif


static цео config__add_listener(struct mosquitto__config *config)
{
	struct mosquitto__listener *listener;
	struct mosquitto__listener *new_listeners;
	цео def_listener = -1;

	ако(config->default_listener){
		за(цео i=0; i<config->listener_count; i++){
			ако(&config->listeners[i] == config->default_listener){
				def_listener = i;
				break;
			}
		}
	}
	new_listeners = mosquitto_realloc(config->listeners, sizeof(struct mosquitto__listener)*(size_t)(config->listener_count+1));
	ако(!new_listeners){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		врати MOSQ_ERR_NOMEM;
	}
	config->listeners = new_listeners;
	listener = &config->listeners[config->listener_count];
	memset(listener, 0, sizeof(struct mosquitto__listener));
	listener->security_options = mosquitto_calloc(1, sizeof(struct mosquitto__security_options));
	ако(!listener->security_options){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		врати MOSQ_ERR_NOMEM;
	}
	ако(def_listener != -1){
		config->default_listener = &config->listeners[def_listener];
	}
	config->listener_count++;

	врати MOSQ_ERR_SUCCESS;
}


static цео config__create_default_listener(struct mosquitto__config *config, const char *option_name)
{
	ако(config->default_listener){
		врати MOSQ_ERR_SUCCESS;
	}
	log__printf(NULL, MOSQ_LOG_INFO, "Creating default listener due to '%s' option.", option_name);
	log__printf(NULL, MOSQ_LOG_INFO, "It is best practice to define a 'listener' first. Using the '%s' option without a listener will be disabled in the future.", option_name);
	ако(config__add_listener(config)){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		врати MOSQ_ERR_NOMEM;
	}

	config->default_listener = &config->listeners[config->listener_count-1];
	listener__set_defaults(config->default_listener);
	config->default_listener->port = 1883;

	врати MOSQ_ERR_SUCCESS;
}


static void conf__set_cur_security_options(struct mosquitto__config *config, struct mosquitto__listener **cur_listener, struct mosquitto__security_options **security_options, const char *option_name)
{
	ако(config->per_listener_settings){
		ако(*cur_listener == NULL){
			ако(config__create_default_listener(config, option_name)){
				врати;
			}
			*cur_listener = config->default_listener;
		}
		(*security_options) = (*cur_listener)->security_options;
	}инако{
		(*security_options) = &config->security_options;
	}
}


static цео conf__attempt_resolve(const char *host, const char *text, unsigned цео log, const char *msg)
{
	struct addrinfo gai_hints;
	struct addrinfo *gai_res;
	цео rc;

	memset(&gai_hints, 0, sizeof(struct addrinfo));
	gai_hints.ai_family = AF_UNSPEC;
	gai_hints.ai_socktype = SOCK_STREAM;
	gai_res = NULL;
	rc = getaddrinfo(host, NULL, &gai_hints, &gai_res);
	ако(gai_res){
		freeaddrinfo(gai_res);
	}
	ако(rc != 0){
#ifndef WIN32
		ако(rc == EAI_SYSTEM){
			ако(errno == ENOENT){
				log__printf(NULL, log, "%s: Unable to resolve %s %s.", msg, text, host);
			}инако{
				log__printf(NULL, log, "%s: Error resolving %s: %s.", msg, text, strerror(errno));
			}
		}инако{
			log__printf(NULL, log, "%s: Error resolving %s: %s.", msg, text, gai_strerror(rc));
		}
#инако
		ако(rc == WSAHOST_NOT_FOUND){
			log__printf(NULL, log, "%s: Error resolving %s.", msg, text);
		}
#endif
		врати MOSQ_ERR_INVAL;
	}
	врати MOSQ_ERR_SUCCESS;
}


static void config__init_reload(struct mosquitto__config *config)
{
	/* Set defaults */
	за(цео i=0; i<config->listener_count; i++){
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
	ако(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = NULL;
	}
	mosquitto_FREE(config->log_file);

#ако defined(WIN32) || defined(__CYGWIN__)
	ако(service_handle){
		/* This is running as a Windows service. Default to no logging. Using
		 * stdout/stderr is forbidden because the first clients to connect will
		 * get log information sent to them for some reason. */
		config->log_dest = MQTT3_LOG_NONE;
	}инако{
		config->log_dest = MQTT3_LOG_STDERR;
	}
#инако
	config->log_facility = LOG_DAEMON;
	config->log_dest = MQTT3_LOG_STDERR | MQTT3_LOG_DLT;
	ако(db.quiet){
		config->log_type = 0;
	}инако ако(db.verbose){
		config->log_type = UINT_MAX;
	}инако{
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

	ако(plugin->config.options){
		за(цео j=0; j<plugin->config.option_count; j++){
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
	за(цео i=0; i<db.plugin_count; i++){
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
	ако(config->listeners){
		за(цео i=0; i<config->listener_count; i++){
			mosquitto_FREE(config->listeners[i].host);
			mosquitto_FREE(config->listeners[i].bind_interface);
			mosquitto_FREE(config->listeners[i].mount_point);
			mosquitto_FREE(config->listeners[i].socks);
			ако(config->listeners[i].security_options){
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
#ако defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			ако(!config->listeners[i].ws_context) /* libwebsockets frees its own SSL_CTX */
#endif
			{
				SSL_CTX_free(config->listeners[i].ssl_ctx);
				config->listeners[i].ssl_ctx = NULL;
			}
#endif
#ако defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
			mosquitto_FREE(config->listeners[i].http_dir);
#endif
#ifdef WITH_WEBSOCKETS
			за(цео j=0; j<config->listeners[i].ws_origin_count; j++){
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
	ако(config->bridges){
		за(цео i=0; i<config->bridge_count; i++){
			config__bridge_cleanup(config->bridges[i]);
		}
		mosquitto_FREE(config->bridges);
	}
#endif
	config__cleanup_plugins();

	ако(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = NULL;
	}
	ако(config->log_file){
		mosquitto_FREE(config->log_file);
		config->log_file = NULL;
	}
}

#ifdef WITH_BRIDGE


void config__bridge_cleanup(struct mosquitto__bridge *bridge)
{
	ако(bridge == NULL){
		врати;
	}

	mosquitto_FREE(bridge->name);
	ако(bridge->addresses){
		за(цео i=0; i<bridge->address_count; i++){
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
	ако(bridge->topics){
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


цео config__parse_args(struct mosquitto__config *config, цео argc, char *argv[])
{
	цео i;
	цео port_tmp;

	за(i=1; i<argc; i++){
		ако(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--config-file")){
			ако(i<argc-1){
				db.config_file = argv[i+1];

				ако(config__read(config, false)){
					врати MOSQ_ERR_INVAL;
				}
			}инако{
				log__printf(NULL, MOSQ_LOG_ERR, "Error: -c argument given, but no config file specified.");
				врати MOSQ_ERR_INVAL;
			}
			i++;
		}инако ако(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--daemon")){
			config->daemon = true;
		}инако ако(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
			print_usage();
			врати MOSQ_ERR_UNKNOWN;
		}инако ако(!strcmp(argv[i], "--version")){
			print_version();
			врати MOSQ_ERR_UNKNOWN;
		}инако ако(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port")){
			ако(i<argc-1){
				port_tmp = atoi(argv[i+1]);
				ако(port_tmp<1 || port_tmp>UINT16_MAX){
					log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid port specified (%d).", port_tmp);
					врати MOSQ_ERR_INVAL;
				}инако{
					ако(config->cmd_port_count == CMD_PORT_LIMIT){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Only %d ports can be specified on the command line.", CMD_PORT_LIMIT);
						врати MOSQ_ERR_INVAL;
					}
					config->cmd_port[config->cmd_port_count] = (uint16_t)port_tmp;
					config->cmd_port_count++;
				}
			}инако{
				log__printf(NULL, MOSQ_LOG_ERR, "Error: -p argument given, but no port specified.");
				врати MOSQ_ERR_INVAL;
			}
			i++;
		}инако ако(!strcmp(argv[i], "--tls-keylog")){
#ifdef WITH_TLS
			ако(i<argc-1){
				db.tls_keylog = mosquitto_strdup(argv[i+1]);
				ако(db.tls_keylog == NULL){
					log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
					врати MOSQ_ERR_NOMEM;
				}
			}инако{
				log__printf(NULL, MOSQ_LOG_ERR, "Error: --tls-keylog argument given, but no file specified.");
				врати MOSQ_ERR_INVAL;
			}
			i++;
#инако
			fprintf(stderr, "Error: TLS support not available so --tls-keylog is not available.\n");
			врати MOSQ_ERR_INVAL;
#endif
		}инако ако(!strcmp(argv[i], "-q") || !strcmp(argv[i], "--quiet")){
			db.quiet = true;
		}инако ако(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")){
			db.verbose = true;
		}инако ако(!strcmp(argv[i], "--test-config")){
			config->test_configuration = true;
		}инако{
			fprintf(stderr, "Error: Unknown option '%s'.\n", argv[i]);
			print_usage();
			врати MOSQ_ERR_INVAL;
		}
	}

	/* Default to drop to mosquitto user if we are privileged and no user specified. */
	ако(!config->user){
		config->user = mosquitto_strdup("mosquitto");
		ако(config->user == NULL){
			врати MOSQ_ERR_NOMEM;
		}
	}
	ако(db.quiet){
		config->log_type = 0;
	}инако ако(db.verbose){
		config->log_type = UINT_MAX;
	}

	ако(getenv("MOSQUITTO_PERSISTENCE_LOCATION")){
		mosquitto_FREE(config->persistence_location);
		config->persistence_location = mosquitto_strdup(getenv("MOSQUITTO_PERSISTENCE_LOCATION"));
		ако(!config->persistence_location){
			врати MOSQ_ERR_NOMEM;
		}
	}
	врати config__check(config);
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

#ако defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	dest->websockets_log_level = src->websockets_log_level;
#endif

#ifdef WITH_BRIDGE
	за(цео i=0; i<dest->bridge_count; i++){
		ако(dest->bridges[i]){
			config__bridge_cleanup(dest->bridges[i]);
		}
	}
	mosquitto_FREE(dest->bridges);
	dest->bridges = src->bridges;
	dest->bridge_count = src->bridge_count;
#endif
}


цео config__read(struct mosquitto__config *config, bool reload)
{
	цео rc = MOSQ_ERR_SUCCESS;
	struct config_recurse cr;
	цео lineno = 0;
#ifdef WITH_PERSISTENCE
	size_t len;
#endif
	struct mosquitto__config config_reload;
	цео i;

	ако(reload){
		memset(&config_reload, 0, sizeof(struct mosquitto__config));
	}

	cr.log_dest = MQTT3_LOG_NONE;
	cr.log_dest_set = 0;
	cr.log_type = MOSQ_LOG_NONE;
	cr.log_type_set = 0;

	ако(!db.config_file){
		врати 0;
	}

	ако(reload){
		/* Re-initialise appropriate config vars to default for reload. */
		config__init_reload(&config_reload);
		config_reload.listeners = config->listeners;
		config_reload.listener_count = config->listener_count;
		cur_security_options = NULL;
		rc = config__read_file(&config_reload, reload, db.config_file, &cr, 0, &lineno);
	}инако{
		rc = config__read_file(config, reload, db.config_file, &cr, 0, &lineno);
	}
	ако(rc){
		ако(lineno > 0){
			log__printf(NULL, MOSQ_LOG_ERR, "Error found at %s:%d.", db.config_file, lineno);
		}
		врати rc;
	}

	ако(reload){
		config__copy(&config_reload, config);
	}

	/* If auth/access options are set and allow_anonymous not explicitly set, disallow anon. */
	ако(config->local_only == false){
		ако(config->per_listener_settings){
			за(i=0; i<config->listener_count; i++){
				/* Default option if no security options set */
				ако(config->listeners[i].security_options->allow_anonymous == -1){
					config->listeners[i].security_options->allow_anonymous = false;
				}
			}
		}инако{
			ако(config->security_options.allow_anonymous == -1){
				config->security_options.allow_anonymous = false;
			}
		}
	}
#ifdef WITH_PERSISTENCE
	ако(config->persistence){
		ако(!config->persistence_file){
			config->persistence_file = mosquitto_strdup("mosquitto.db");
			ако(!config->persistence_file){
				врати MOSQ_ERR_NOMEM;
			}
		}
		mosquitto_FREE(config->persistence_filepath);
		ако(config->persistence_location && strlen(config->persistence_location)){
			len = strlen(config->persistence_location) + strlen(config->persistence_file) + 2;
			config->persistence_filepath = mosquitto_malloc(len);
			ако(!config->persistence_filepath){
				врати MOSQ_ERR_NOMEM;
			}
#ifdef WIN32
			snprintf(config->persistence_filepath, len, "%s\\%s", config->persistence_location, config->persistence_file);
#инако
			snprintf(config->persistence_filepath, len, "%s/%s", config->persistence_location, config->persistence_file);
#endif
		}инако{
			config->persistence_filepath = mosquitto_strdup(config->persistence_file);
			ако(!config->persistence_filepath){
				врати MOSQ_ERR_NOMEM;
			}
		}
	}
#endif
	/* Default to drop to mosquitto user if no other user specified. This must
	 * remain here even though it is covered in config__parse_args() because this
	 * function may be called on its own. */
	ако(!config->user){
		config->user = mosquitto_strdup("mosquitto");
	}

#ifdef WITH_BRIDGE
	за(i=0; i<config->bridge_count; i++){
		ако(!config->bridges[i]->name){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: bridge name not defined.");
			врати MOSQ_ERR_INVAL;
		}
		ако(config->bridges[i]->addresses  == 0){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no remote addresses defined.");
			врати MOSQ_ERR_INVAL;
		}
		ако(config->bridges[i]->topic_count == 0){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no topics defined.");
			врати MOSQ_ERR_INVAL;
		}
#ifdef FINAL_WITH_TLS_PSK
		ако(config->bridges[i]->tls_psk && !config->bridges[i]->tls_psk_identity){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_identity.");
			врати MOSQ_ERR_INVAL;
		}
		ако(config->bridges[i]->tls_psk_identity && !config->bridges[i]->tls_psk){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_psk.");
			врати MOSQ_ERR_INVAL;
		}
#endif
	}
#endif

	ако(cr.log_dest_set){
		config->log_dest = cr.log_dest;
	}
	ако(db.quiet){
		config->log_type = 0;
	}инако ако(db.verbose){
		config->log_type = UINT_MAX;
	}инако ако(cr.log_type_set){
		config->log_type = cr.log_type;
	}
#ifdef WITH_BRIDGE
	врати config__check_bridges(config);
#инако
	врати MOSQ_ERR_SUCCESS;
#endif
}


static mosquitto_plugin_id_t *config__plugin_find(const char *name)
{
	ако(db.plugins && name){
		за(цео i=0; i<db.plugin_count; i++){
			ако(db.plugins[i]->config.name && !strcmp(db.plugins[i]->config.name, name)){
				врати db.plugins[i];
			}
		}
	}
	врати NULL;
}


static mosquitto_plugin_id_t *config__plugin_load(const char *name, const char *path)
{
	mosquitto_plugin_id_t *plugin = NULL;
	mosquitto_plugin_id_t **plugins = NULL;

	ако(name && config__plugin_find(name)){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Duplicate plugin name '%s'.", name);
		врати NULL;
	}

	ако(!path || !strcmp(path, "")){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Missing plugin path for plugin name '%s'.", name);
		врати NULL;
	}

	plugin = mosquitto_calloc(1, sizeof(mosquitto_plugin_id_t));
	ако(!plugin){
		goto error;
	}

	ако(name){
		plugin->config.name = mosquitto_strdup(name);
	}
	plugin->config.path = mosquitto_strdup(path);
	ако((name && !plugin->config.name) || !plugin->config.path){
		goto error;
	}
	plugin->config.options = NULL;
	plugin->config.option_count = 0;
	plugin->config.deny_special_chars = true;

	/* Add to db list */
	plugins = mosquitto_realloc(db.plugins, (size_t)(db.plugin_count+1)*sizeof(struct mosquitto__plugin_config *));
	ако(!plugins){
		goto error;
	}

	plugins[db.plugin_count] = plugin;
	db.plugins = plugins;
	db.plugin_count++;

	врати plugin;
error:
	log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
	ако(plugin){
		mosquitto_FREE(plugin->config.name);
		mosquitto_FREE(plugin->config.path);
	}
	mosquitto_FREE(plugin);
	врати NULL;
}


цео config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, struct mosquitto__security_options *security_options)
{
	struct mosquitto__security_options **new_options;
	mosquitto_plugin_id_t **new_plugins;

	new_options = mosquitto_realloc(plugin->config.security_options, (size_t)(plugin->config.security_option_count+1)*sizeof(struct mosquitto__security_options *));
	new_plugins = mosquitto_realloc(security_options->plugins, (size_t)(security_options->plugin_count+1)*sizeof(mosquitto_plugin_id_t *));

	ако(!new_options || !new_plugins){
		mosquitto_FREE(new_options);
		mosquitto_FREE(new_plugins);
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		врати MOSQ_ERR_NOMEM;
	}

	new_options[plugin->config.security_option_count] = security_options;
	plugin->config.security_options = new_options;
	plugin->config.security_option_count++;

	new_plugins[security_options->plugin_count] = plugin;
	security_options->plugins = new_plugins;
	security_options->plugin_count++;

	врати MOSQ_ERR_SUCCESS;
}


static цео config__read_file_core(struct mosquitto__config *config, bool reload, struct config_recurse *cr, цео level, цео *lineno, FILE *fptr, char **buf, цео *buflen)
{
	цео rc;
	char *token;
	цео tmp_int;
	char *saveptr = NULL;
#ifdef WITH_BRIDGE
	char *tmp_char;
	struct mosquitto__bridge *cur_bridge = NULL;
#endif
	mosquitto_plugin_id_t *cur_plugin = NULL;

	char *key;
	struct mosquitto__listener *cur_listener = NULL;
	цео i;
	цео lineno_ext = 0;
	size_t prefix_len;
	size_t slen;
#ifdef WITH_WEBSOCKETS
	char **ws_origins = NULL;
#endif

	*lineno = 0;

	док(mosquitto_fgets(buf, buflen, fptr)){
		(*lineno)++;
		ако((*buf)[0] != '#' && (*buf)[0] != 10 && (*buf)[0] != 13){
			slen = strlen(*buf);
			ако(slen == 0){
				continue;
			}
			док((*buf)[slen-1] == 10 || (*buf)[slen-1] == 13){
				(*buf)[slen-1] = 0;
				slen = strlen(*buf);
				ако(slen == 0){
					continue;
				}
			}
			token = strtok_r((*buf), " ", &saveptr);
			ако(token){
				ако(!strcmp(token, "accept_protocol_versions")){
					REQUIRE_NON_DEFAULT_LISTENER(token);
					cur_listener->disable_protocol_v3 = true;
					cur_listener->disable_protocol_v4 = true;
					cur_listener->disable_protocol_v5 = true;
					ако(saveptr == NULL){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", "accept_protocol_versions");
						врати MOSQ_ERR_INVAL;
					}
					token = strtok_r(saveptr, ", \t", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "accept_protocol_versions");

					док(token){
						ако(!strcmp(token, "3")){
							cur_listener->disable_protocol_v3 = false;
						}инако ако(!strcmp(token, "4")){
							cur_listener->disable_protocol_v4 = false;
						}инако ако(!strcmp(token, "5")){
							cur_listener->disable_protocol_v5 = false;
						}

						token = strtok_r(NULL, ", \t", &saveptr);
					}
				}инако ако(!strcmp(token, "acl_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);

					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->acl_data.acl_file);
					ако(conf__parse_string(&token, "acl_file", &cur_security_options->acl_data.acl_file, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "address") || !strcmp(token, "addresses")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ако(cur_bridge->addresses){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge configuration, 'address' only allowed once.");
						врати MOSQ_ERR_INVAL;
					}
					док((token = strtok_r(NULL, " ", &saveptr))){
						ако(token[0] == '#'){
							break;
						}
						struct bridge_address *new_addresses = mosquitto_realloc(cur_bridge->addresses, sizeof(struct bridge_address)*(size_t)(cur_bridge->address_count+1));
						ако(!new_addresses){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
							врати MOSQ_ERR_NOMEM;
						}
						cur_bridge->address_count++;
						cur_bridge->addresses = new_addresses;
						memset(&cur_bridge->addresses[cur_bridge->address_count-1], 0, sizeof(struct bridge_address));
						cur_bridge->addresses[cur_bridge->address_count-1].address = mosquitto_strdup(token);
						ако(!cur_bridge->addresses[cur_bridge->address_count-1].address){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
							врати MOSQ_ERR_NOMEM;
						}
					}
					за(i=0; i<cur_bridge->address_count; i++){
						/* cur_bridge->addresses[i].address is now
						 * "address[:port]". If address is an IPv6 address,
						 * then port is required. We must check for the :
						 * backwards. */
						tmp_char = strrchr(cur_bridge->addresses[i].address, ':');
						ако(tmp_char){
							/* Remove ':', so cur_bridge->addresses[i].address
							 * now just looks like the address. */
							tmp_char[0] = '\0';

							/* The remainder of the string */
							tmp_int = atoi(&tmp_char[1]);
							ако(tmp_int < 1 || tmp_int > UINT16_MAX){
								log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge port value (%d).", tmp_int);
								врати MOSQ_ERR_INVAL;
							}
							cur_bridge->addresses[i].port = (uint16_t)tmp_int;
						}инако{
							cur_bridge->addresses[i].port = 1883;
						}
						conf__attempt_resolve(cur_bridge->addresses[i].address, "bridge address", MOSQ_LOG_WARNING, "Warning");
					}
					ако(cur_bridge->address_count == 0){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty address value in configuration.");
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "allow_anonymous")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					PER_LISTENER_ALTERNATIVE(token, "listener_allow_anonymous");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ако(conf__parse_bool(&token, "allow_anonymous", (bool *)&cur_security_options->allow_anonymous, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "allow_duplicate_messages")){
					OPTION_DEPRECATED(token, "The behaviour will default to true.");
					ако(conf__parse_bool(&token, "allow_duplicate_messages", &config->allow_duplicate_messages, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "allow_zero_length_clientid")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ако(conf__parse_bool(&token, "allow_zero_length_clientid", &cur_security_options->allow_zero_length_clientid, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strncmp(token, "auth_opt_", strlen("auth_opt_")) || !strncmp(token, "plugin_opt_", strlen("plugin_opt_"))){
					ако(reload){
						continue;        /* Auth plugin not currently valid for reloading. */

					}
					REQUIRE_PLUGIN(token);

					ако(!strncmp(token, "auth_opt_", strlen("auth_opt_"))){
						prefix_len = strlen("auth_opt_");
					}инако{
						prefix_len = strlen("plugin_opt_");
					}
					ако(strlen(token) < prefix_len + 3){
						/* auth_opt_ == 9, + one digit key == 10, + one space == 11, + one value == 12 */
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'plugin_opt_' config option.");
						врати MOSQ_ERR_INVAL;
					}
					key = mosquitto_strdup(&token[prefix_len]);
					ако(!key){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
						врати MOSQ_ERR_NOMEM;
					}инако ако(STREMPTY(key)){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty 'plugin_opt_' config option.");
						mosquitto_FREE(key);
						врати MOSQ_ERR_INVAL;
					}
					token = saveptr;
					ако(token && token[0]){
						док(token[0] == ' ' || token[0] == '\t'){
							token++;
						}
						struct mosquitto_opt *new_options = mosquitto_realloc(cur_plugin->config.options, (size_t)(cur_plugin->config.option_count+1)*sizeof(struct mosquitto_opt));
						ако(!new_options){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
							mosquitto_FREE(key);
							врати MOSQ_ERR_NOMEM;
						}
						cur_plugin->config.option_count++;
						cur_plugin->config.options = new_options;
						cur_plugin->config.options[cur_plugin->config.option_count-1].key = key;
						cur_plugin->config.options[cur_plugin->config.option_count-1].value = mosquitto_strdup(token);
						ако(!cur_plugin->config.options[cur_plugin->config.option_count-1].value){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
							врати MOSQ_ERR_NOMEM;
						}
					}инако{
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", key);
						mosquitto_FREE(key);
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "auth_plugin") || !strcmp(token, "plugin") || !strcmp(token, "global_plugin")){
					ако(reload){
						continue;        /* plugin not currently valid for reloading. */
					}
					ако(!strcmp(token, "global_plugin")){
						cur_security_options = &db.config->security_options;
					}инако{
						REQUIRE_LISTENER_IF_PER_LISTENER(token);
						conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					}

					cur_plugin = config__plugin_load(NULL, saveptr);
					ако(cur_plugin == NULL){
						врати MOSQ_ERR_INVAL;
					}
					ако(config__plugin_add_secopt(cur_plugin, cur_security_options)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "auth_plugin_deny_special_chars")){
					ако(reload){
						continue;        /* Auth plugin not currently valid for reloading. */
					}
					ако(!cur_plugin){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: An auth_plugin_deny_special_chars option exists in the config file without a plugin.");
						врати MOSQ_ERR_INVAL;
					}
					ако(conf__parse_bool(&token, "auth_plugin_deny_special_chars", &cur_plugin->config.deny_special_chars, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "plugin_load")){
					char *name = NULL;

					ако(reload){
						continue;        /* plugin not currently valid for reloading. */

					}
					name = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(name, "plugin_load");

					cur_plugin = config__plugin_load(name, saveptr);
					ако(cur_plugin == NULL){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "plugin_use")){
					char *name = NULL;

					ако(reload){
						continue;        /* plugin not currently valid for reloading. */
					}
					REQUIRE_NON_DEFAULT_LISTENER(token);

					ако(conf__parse_string(&token, "plugin_use", &name, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					cur_plugin = config__plugin_find(name);
					ако(!cur_plugin){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Plugin '%s' not previously loaded.", name);
						mosquitto_FREE(name);
						врати MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(name);
					ако(config__plugin_add_secopt(cur_plugin, cur_listener->security_options)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "auto_id_prefix")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					OPTION_DEPRECATED(token, "Please use 'listener_auto_id_prefix' instead.");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ако(conf__parse_string(&token, "auto_id_prefix", &cur_security_options->auto_id_prefix, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(cur_security_options->auto_id_prefix){
						ако(strlen(cur_security_options->auto_id_prefix) > 50){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: auto_id_prefix length must be <= 50.");
							врати MOSQ_ERR_INVAL;
						}
						cur_security_options->auto_id_prefix_len = (uint16_t)strlen(cur_security_options->auto_id_prefix);
					}инако{
						cur_security_options->auto_id_prefix_len = 0;
					}
				}инако ако(!strcmp(token, "autosave_interval")){
					ако(conf__parse_int(&token, "autosave_interval", &config->autosave_interval, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(config->autosave_interval < 0){
						config->autosave_interval = 0;
					}
				}инако ако(!strcmp(token, "autosave_on_changes")){
					ако(conf__parse_bool(&token, "autosave_on_changes", &config->autosave_on_changes, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "bind_address")){
					OPTION_DEPRECATED(token, "");
					config->local_only = false;
					ако(reload){
						continue;        /* Rebinding listeners not valid during reloading. */

					}
					ако(config__create_default_listener(config, token)){
						врати MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					ако(conf__parse_string(&token, "default listener bind_address", &config->default_listener->host, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(conf__attempt_resolve(config->default_listener->host, "bind_address", MOSQ_LOG_ERR, "Error")){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "bind_interface")){
#ifdef SO_BINDTODEVICE
					ако(reload){
						continue;        /* Rebinding listeners not valid during reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ако(conf__parse_string(&token, "bind_interface", &cur_listener->bind_interface, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_ERR, "Error: bind_interface specified but socket option not available.");
					врати MOSQ_ERR_INVAL;
#endif
				}инако ако(!strcmp(token, "bridge_attempt_unsubscribe")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ако(conf__parse_bool(&token, "bridge_attempt_unsubscribe", &cur_bridge->attempt_unsubscribe, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "bridge_cafile")){
#ако defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					ако(conf__parse_string(&token, "bridge_cafile", &cur_bridge->tls_cafile, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}инако ако(!strcmp(token, "bridge_alpn")){
#ако defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ако(conf__parse_string(&token, "bridge_alpn", &cur_bridge->tls_alpn, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}инако ако(!strcmp(token, "bridge_ciphers")){
#ако defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ако(conf__parse_string(&token, "bridge_ciphers", &cur_bridge->tls_ciphers, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}инако ако(!strcmp(token, "bridge_ciphers_tls1.3")){
#ако defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ако(conf__parse_string(&token, "bridge_ciphers_tls1.3", &cur_bridge->tls_13_ciphers, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}инако ако(!strcmp(token, "bridge_bind_address")){
#ако defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ако(conf__parse_string(&token, "bridge_bind_address", &cur_bridge->bind_address, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "bridge_capath")){
#ако defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					ако(conf__parse_string(&token, "bridge_capath", &cur_bridge->tls_capath, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}инако ако(!strcmp(token, "bridge_certfile")){
#ако defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					ако(conf__parse_string(&token, "bridge_certfile", &cur_bridge->tls_certfile, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}инако ако(!strcmp(token, "bridge_identity")){
#ако defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					ако(conf__parse_string(&token, "bridge_identity", &cur_bridge->tls_psk_identity, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}инако ако(!strcmp(token, "bridge_insecure")){
#ако defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ако(conf__parse_bool(&token, "bridge_insecure", &cur_bridge->tls_insecure, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(cur_bridge->tls_insecure){
						log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge '%s' using insecure mode.", cur_bridge->name);
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}инако ако(!strcmp(token, "bridge_require_ocsp")){
#ако defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ако(conf__parse_bool(&token, "bridge_require_ocsp", &cur_bridge->tls_ocsp_required, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}инако ако(!strcmp(token, "bridge_max_packet_size")){
#ако defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ако(conf__parse_int(&token, "bridge_max_packet_size", &tmp_int, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(tmp_int < 0){
						tmp_int = 0;
					}
					cur_bridge->maximum_packet_size = (uint32_t)tmp_int;
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "bridge_max_topic_alias")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ако(conf__parse_int(&token, "bridge_max_topic_alias", &tmp_int, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}

					ако(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: bridge_max_topic_alias must be > 0 and <= 65535.");
						врати MOSQ_ERR_INVAL;
					}
					cur_bridge->max_topic_alias = (uint16_t)tmp_int;
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "bridge_outgoing_retain")){
#ако defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ако(conf__parse_bool(&token, "bridge_outgoing_retain", &cur_bridge->outgoing_retain, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "bridge_keyfile")){
#ако defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					mosquitto_FREE(cur_bridge->tls_keyfile);
					ако(conf__parse_string(&token, "bridge_keyfile", &cur_bridge->tls_keyfile, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}инако ако(!strcmp(token, "bridge_protocol_version")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(NULL, "", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_protocol_version");

					ако(!strcmp(token, "mqttv31")){
						cur_bridge->protocol_version = mosq_p_mqtt31;
					}инако ако(!strcmp(token, "mqttv311")){
						cur_bridge->protocol_version = mosq_p_mqtt311;
					}инако ако(!strcmp(token, "mqttv50")){
						cur_bridge->protocol_version = mosq_p_mqtt5;
					}инако{
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'bridge_protocol_version' value (%s).", token);
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "bridge_psk")){
#ако defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					ако(conf__parse_string(&token, "bridge_psk", &cur_bridge->tls_psk, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}инако ако(!strcmp(token, "bridge_receive_maximum")){
#ако defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ако(conf__parse_int(&token, "bridge_receive_maximum", &tmp_int, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(tmp_int <= 0){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be greater than 0.");
						врати MOSQ_ERR_INVAL;
					}инако ако((uint64_t)tmp_int > (uint64_t)UINT16_MAX){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be lower than %u.", UINT16_MAX);
						врати MOSQ_ERR_INVAL;
					}
					cur_bridge->receive_maximum = (uint16_t)tmp_int;
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "bridge_reload_type")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_reload_type");

					ако(!strcmp(token, "lazy")){
						cur_bridge->reload_type = brt_lazy;
					}инако ако(!strcmp(token, "immediate")){
						cur_bridge->reload_type = brt_immediate;
					}инако{
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'bridge_reload_type' value in configuration (%s).", token);
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "bridge_session_expiry_interval")){
#ако defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					ако(conf__parse_int(&token, "bridge_session_expiry_interval", &tmp_int, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(tmp_int < 0){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must not be negative.");
						врати MOSQ_ERR_INVAL;
					}инако ако((uint64_t)tmp_int > (uint64_t)MQTT_SESSION_EXPIRY_NEVER){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must be lower than %u.", MQTT_SESSION_EXPIRY_NEVER);
						врати MOSQ_ERR_INVAL;
					}
					cur_bridge->session_expiry_interval = (uint32_t)tmp_int;
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "bridge_tcp_keepalive")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);

					ако(conf__parse_int(&token, "bridge_tcp_keepalive_idle", &tmp_int, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(tmp_int <= 0){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: invalid TCP keepalive idle value.");
						врати MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_idle = (unsigned цео)tmp_int;

					ако(conf__parse_int(&token, "bridge_tcp_keepalive_interval", &tmp_int, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(tmp_int <= 0){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: invalid TCP keepalive interval value.");
						врати MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_interval = (unsigned цео)tmp_int;

					ако(conf__parse_int(&token, "bridge_tcp_keepalive_counter", &tmp_int, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(tmp_int <= 0){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: invalid TCP keepalive counter value.");
						врати MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_counter = (unsigned цео)tmp_int;
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "bridge_tcp_user_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
#ifdef WITH_TCP_USER_TIMEOUT
					ако(conf__parse_int(&token, "bridge_tcp_user_timeout", &tmp_int, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(tmp_int < 0){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: invalid TCP user timeout value.");
						врати MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_user_timeout = tmp_int;
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge TCP user timeout support not available.");
#endif
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "bridge_tls_use_os_certs")){
#ако defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ако(conf__parse_bool(&token, "bridge_tls_use_os_certs", &cur_bridge->tls_use_os_certs, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}инако ако(!strcmp(token, "bridge_tls_version")){
#ако defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					ако(conf__parse_string(&token, "bridge_tls_version", &cur_bridge->tls_version, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}инако ако(!strcmp(token, "cafile")){
#ако defined(WITH_TLS)
					REQUIRE_LISTENER(token);
					ако(cur_listener->psk_hint){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						врати MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->cafile);
					ако(conf__parse_string(&token, "cafile", &cur_listener->cafile, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}инако ако(!strcmp(token, "capath")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->capath);
					ако(conf__parse_string(&token, "capath", &cur_listener->capath, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}инако ако(!strcmp(token, "certfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ако(cur_listener->psk_hint){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						врати MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->certfile);
					ако(conf__parse_string(&token, "certfile", &cur_listener->certfile, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}инако ако(!strcmp(token, "check_retain_source")){
					ако(conf__parse_bool(&token, "check_retain_source", &config->check_retain_source, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "ciphers")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers);
					ако(conf__parse_string(&token, "ciphers", &cur_listener->ciphers, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}инако ако(!strcmp(token, "ciphers_tls1.3")){
#ако defined(WITH_TLS) && (!defined(LIBRESSL_VERSION_NUMBER) || LIBRESSL_VERSION_NUMBER > 0x3040000FL)
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers_tls13);
					ако(conf__parse_string(&token, "ciphers_tls1.3", &cur_listener->ciphers_tls13, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: ciphers_tls1.3 support not available.");
#endif
				}инако ако(!strcmp(token, "clientid") || !strcmp(token, "remote_clientid")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ако(conf__parse_string(&token, "bridge remote clientid", &cur_bridge->remote_clientid, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "cleansession")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ако(conf__parse_bool(&token, "cleansession", &cur_bridge->clean_start, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "local_cleansession")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ако(conf__parse_bool(&token, "local_cleansession", (bool *)&cur_bridge->clean_start_local, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "clientid_prefixes")){
					OPTION_DEPRECATED(token, "");
					mosquitto_FREE(config->clientid_prefixes);
					ако(conf__parse_string(&token, "clientid_prefixes", &config->clientid_prefixes, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "connection")){
#ifdef WITH_BRIDGE
					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "connection");

					/* Check for existing bridge name. */
					за(i=0; i<config->bridge_count; i++){
						ако(!strcmp(config->bridges[i]->name, token)){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Duplicate bridge name '%s'.", token);
							врати MOSQ_ERR_INVAL;
						}
					}

					struct mosquitto__bridge **bridges_new = mosquitto_realloc(config->bridges, (size_t)(config->bridge_count+1)*sizeof(struct mosquitto__bridge *));
					ако(!bridges_new){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
						врати MOSQ_ERR_NOMEM;
					}
					config->bridges = bridges_new;
					cur_bridge = mosquitto_malloc(sizeof(struct mosquitto__bridge));
					ако(!cur_bridge){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
						врати MOSQ_ERR_NOMEM;
					}

					config->bridge_count++;
					config->bridges[config->bridge_count-1] = cur_bridge;

					memset(cur_bridge, 0, sizeof(struct mosquitto__bridge));
					cur_bridge->name = mosquitto_strdup(token);
					ако(!cur_bridge->name){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
						врати MOSQ_ERR_NOMEM;
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
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "connection_messages")){
					ако(conf__parse_bool(&token, token, &config->connection_messages, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "crlfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->crlfile);
					ако(conf__parse_string(&token, "crlfile", &cur_listener->crlfile, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}инако ако(!strcmp(token, "dhparamfile")){
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: dhparamfile is no longer required.");
				}инако ако(!strcmp(token, "disable_client_cert_date_checks")){
#ifdef WITH_TLS
					REQUIRE_LISTENER(token);
					ако(conf__parse_bool(&token, "disable_client_cert_date_checks", &cur_listener->disable_client_cert_date_checks, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}инако ако(!strcmp(token, "enable_control_api")){
#ifdef WITH_CONTROL
					ако(conf__parse_bool(&token, "enable_control_api", &config->enable_control_api, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: $CONTROL support not available (enable_control_api).");
#endif
				}инако ако(!strcmp(token, "enable_proxy_protocol")){
#ако defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(NULL, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					врати MOSQ_ERR_INVAL;
#endif
					REQUIRE_LISTENER(token);
					ако(conf__parse_int(&token, "enable_proxy_protocol", &cur_listener->enable_proxy_protocol, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(cur_listener->enable_proxy_protocol < 1 || cur_listener->enable_proxy_protocol > 2){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: enable_proxy_protocol must be 1 or 2.");
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "global_max_clients")){
					ако(conf__parse_int(&token, "global_max_clients", &config->global_max_clients, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "global_max_connections")){
					ако(conf__parse_int(&token, "global_max_connections", &config->global_max_connections, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "http_dir")){
#ако defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
					ако(reload){
						continue;        /* Not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					ако(conf__parse_string(&token, "http_dir", &cur_listener->http_dir, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#ifdef WIN32
					char *http_dir_canonical = _fullpath(NULL, cur_listener->http_dir, 0);
					const char sep = '\\';
#инако
					char *http_dir_canonical = realpath(cur_listener->http_dir, NULL);
					const char sep = '/';
#endif
					ако(!http_dir_canonical){
						врати MOSQ_ERR_NOMEM;
					}
					size_t http_dir_len = strlen(http_dir_canonical) + sizeof(sep) + 1;
					char *http_dir_canonical_sep = mosquitto_calloc(http_dir_len, sizeof(char));
					ако(!http_dir_canonical_sep){
						free(http_dir_canonical);
						врати MOSQ_ERR_NOMEM;
					}
					snprintf(http_dir_canonical_sep, http_dir_len, "%s%c", http_dir_canonical, sep);
					free(http_dir_canonical);
					mosquitto_FREE(cur_listener->http_dir);
					cur_listener->http_dir = http_dir_canonical_sep;
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: http_dir support not available.");
#endif
				}инако ако(!strcmp(token, "idle_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ако(conf__parse_int(&token, "idle_timeout", &cur_bridge->idle_timeout, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(cur_bridge->idle_timeout < 1){
						log__printf(NULL, MOSQ_LOG_NOTICE, "idle_timeout interval too low, using 1 second.");
						cur_bridge->idle_timeout = 1;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "include_dir")){
					ако(level == 0){
						char **files;
						цео file_count;

						/* Only process include_dir from the main config file. */
						token = strtok_r(NULL, "", &saveptr);
						REQUIRE_NON_EMPTY_OPTION(token, "include_dir");

						rc = config__get_dir_files(token, &files, &file_count);
						ако(rc){
							врати rc;
						}

						за(i=0; i<file_count; i++){
							log__printf(NULL, MOSQ_LOG_INFO, "Loading config file '%s'", files[i]);

							rc = config__read_file(config, reload, files[i], cr, level+1, &lineno_ext);
							ако(rc){
								ако(lineno_ext > 0){
									log__printf(NULL, MOSQ_LOG_ERR, "Error found at '%s:%d'.", files[i], lineno_ext);
								}
								/* Free happens below */
								break;
							}
						}
						за(i=0; i<file_count; i++){
							mosquitto_FREE(files[i]);
						}
						mosquitto_FREE(files);
						ако(rc){
							врати rc;    /* This returns if config__read_file() fails above */
						}
					}инако{
						log__printf(NULL, MOSQ_LOG_ERR, "Error: The include_dir option is only valid in the main configuration file.");
						врати 1;
					}
				}инако ако(!strcmp(token, "keepalive_interval")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ако(conf__parse_int(&token, "keepalive_interval", &tmp_int, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(tmp_int > UINT16_MAX){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Bridge keepalive value too high.");
						врати MOSQ_ERR_INVAL;
					}
					ако(tmp_int < 5){
						log__printf(NULL, MOSQ_LOG_NOTICE, "keepalive interval too low, using 5 seconds.");
						tmp_int = 5;
					}
					cur_bridge->keepalive = (uint16_t)tmp_int;
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "keyfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->keyfile);
					ако(conf__parse_string(&token, "keyfile", &cur_listener->keyfile, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}инако ако(!strcmp(token, "listener")){
					config->local_only = false;

					ако(conf__parse_int(&token, "listener port", &tmp_int, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#ifdef WITH_UNIX_SOCKETS
					ако(tmp_int < 0 || tmp_int > UINT16_MAX){
#инако
					ако(tmp_int < 1 || tmp_int > UINT16_MAX){
#endif
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						врати MOSQ_ERR_INVAL;
					}

					/* Look for bind address / unix socket path */
					token = strtok_r(NULL, " ", &saveptr);
					ако(token != NULL && token[0] == '#'){
						token = NULL;
					}

					ако(tmp_int == 0 && token == NULL){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: A listener with port 0 must provide a Unix socket path.");
						врати MOSQ_ERR_INVAL;
					}

					ако(reload){
						/* We reload listeners settings based on port number/unix socket path.
						 * If the port number/unix path doesn't already exist, exit with a complaint. */
						cur_listener = NULL;
#ifdef WITH_UNIX_SOCKETS
						ако(tmp_int == 0){
							за(i=0; i<config->listener_count; i++){
								ако(config->listeners[i].unix_socket_path != NULL
										&& strcmp(config->listeners[i].unix_socket_path, token) == 0){

									cur_listener = &config->listeners[i];
									break;
								}
							}
						}инако
#endif
						{
							за(i=0; i<config->listener_count; i++){
								ако(config->listeners[i].port == tmp_int){
									/* Now check we have a matching bind address, if defined */
									ако(config->listeners[i].host){
										ако(token && !strcmp(config->listeners[i].host, token)){
											/* They both have a bind address, and they match */
											cur_listener = &config->listeners[i];
											break;
										}
									}инако{
										ако(token == NULL){
											/* Neither this config nor the new config have a bind address,
											 * so they match. */
											cur_listener = &config->listeners[i];
											break;
										}
									}
								}
							}
						}
						ако(!cur_listener){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: It is not currently possible to add/remove listeners when reloading the config file.");
							врати MOSQ_ERR_INVAL;
						}
					}инако{
						ако(config__add_listener(config)){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
							врати MOSQ_ERR_NOMEM;
						}
						cur_listener = &config->listeners[config->listener_count-1];
					}

					listener__set_defaults(cur_listener);
					cur_listener->port = (uint16_t)tmp_int;

					mosquitto_FREE(cur_listener->host);

#ifdef WITH_UNIX_SOCKETS
					mosquitto_FREE(cur_listener->unix_socket_path);
#endif

					ако(token){
#ifdef WITH_UNIX_SOCKETS
						ако(cur_listener->port == 0){
							cur_listener->unix_socket_path = mosquitto_strdup(token);
						}инако
#endif
						{
							cur_listener->host = mosquitto_strdup(token);
						}
					}
				}инако ако(!strcmp(token, "listener_allow_anonymous")){
					REQUIRE_LISTENER(token);
					ако(conf__parse_bool(&token, "listener_allow_anonymous", (bool *)&cur_listener->security_options->allow_anonymous, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "listener_auto_id_prefix")){
					REQUIRE_LISTENER(token);
					ако(conf__parse_string(&token, "listener_auto_id_prefix", &cur_listener->security_options->auto_id_prefix, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(cur_listener->security_options->auto_id_prefix){
						ако(strlen(cur_listener->security_options->auto_id_prefix) > 50){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: 'listener_auto_id_prefix' length must be <= 50.");
							врати MOSQ_ERR_INVAL;
						}
						cur_listener->security_options->auto_id_prefix_len = (uint16_t)strlen(cur_listener->security_options->auto_id_prefix);
					}инако{
						cur_listener->security_options->auto_id_prefix_len = 0;
					}
				}инако ако(!strcmp(token, "local_clientid")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ако(conf__parse_string(&token, "bridge local clientd", &cur_bridge->local_clientid, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "local_password")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ако(conf__parse_string(&token, "bridge local_password", &cur_bridge->local_password, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "local_username")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ако(conf__parse_string(&token, "bridge local_username", &cur_bridge->local_username, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "log_dest")){
					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_dest");

					cr->log_dest_set = 1;
					ако(!strcmp(token, "none")){
						cr->log_dest = MQTT3_LOG_NONE;
					}инако ако(!strcmp(token, "syslog")){
						cr->log_dest |= MQTT3_LOG_SYSLOG;
					}инако ако(!strcmp(token, "stdout")){
						cr->log_dest |= MQTT3_LOG_STDOUT;
					}инако ако(!strcmp(token, "stderr")){
						cr->log_dest |= MQTT3_LOG_STDERR;
					}инако ако(!strcmp(token, "topic")){
						cr->log_dest |= MQTT3_LOG_TOPIC;
					}инако ако(!strcmp(token, "dlt")){
						cr->log_dest |= MQTT3_LOG_DLT;
#ifdef ANDROID
					}инако ако(!strcmp(token, "android")){
						cr->log_dest |= MQTT3_LOG_ANDROID;
#endif
					}инако ако(!strcmp(token, "file")){
						ако(config->log_fptr || config->log_file){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Duplicate \"log_dest file\" value.");
							врати MOSQ_ERR_INVAL;
						}
						/* Get remaining string. */
						token = saveptr;
						ако(token && token[0]){
							док(token[0] == ' ' || token[0] == '\t'){
								token++;
							}
						}
						/* Duplicate "token" check here saves a log__printf() */
						ако(token && token[0]){
							config->log_file = mosquitto_strdup(token);
							ако(!config->log_file){
								log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
								врати MOSQ_ERR_NOMEM;
							}
							cr->log_dest |= MQTT3_LOG_FILE;
						}инако{
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty \"log_dest file\" value in configuration.");
							врати MOSQ_ERR_INVAL;
						}
						cr->log_dest |= MQTT3_LOG_FILE;
					}инако{
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'log_dest' value (%s).", token);
						врати MOSQ_ERR_INVAL;
					}
#ако defined(WIN32) || defined(__CYGWIN__)
					ако(service_handle){
						ако(cr->log_dest == MQTT3_LOG_STDOUT || cr->log_dest == MQTT3_LOG_STDERR){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Cannot log to stdout/stderr when running as a Windows service.");
							врати MOSQ_ERR_INVAL;
						}
					}
#endif
				}инако ако(!strcmp(token, "log_facility")){
#ако defined(WIN32) || defined(__CYGWIN__)
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: log_facility not supported on Windows.");
#инако
					ако(conf__parse_int(&token, "log_facility", &tmp_int, &saveptr)){
						врати MOSQ_ERR_INVAL;
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
							врати MOSQ_ERR_INVAL;
					}
#endif
				}инако ако(!strcmp(token, "log_timestamp")){
					ако(conf__parse_bool(&token, token, &config->log_timestamp, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "log_timestamp_format")){
					ако(conf__parse_string(&token, token, &config->log_timestamp_format, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "log_type")){
					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_type");

					cr->log_type_set = 1;
					ако(!strcmp(token, "none")){
						cr->log_type = MOSQ_LOG_NONE;
					}инако ако(!strcmp(token, "information")){
						cr->log_type |= MOSQ_LOG_INFO;
					}инако ако(!strcmp(token, "notice")){
						cr->log_type |= MOSQ_LOG_NOTICE;
					}инако ако(!strcmp(token, "warning")){
						cr->log_type |= MOSQ_LOG_WARNING;
					}инако ако(!strcmp(token, "error")){
						cr->log_type |= MOSQ_LOG_ERR;
					}инако ако(!strcmp(token, "debug")){
						cr->log_type |= MOSQ_LOG_DEBUG;
					}инако ако(!strcmp(token, "subscribe")){
						cr->log_type |= MOSQ_LOG_SUBSCRIBE;
					}инако ако(!strcmp(token, "unsubscribe")){
						cr->log_type |= MOSQ_LOG_UNSUBSCRIBE;
					}инако ако(!strcmp(token, "internal")){
						cr->log_type |= MOSQ_LOG_INTERNAL;
#ifdef WITH_WEBSOCKETS
					}инако ако(!strcmp(token, "websockets")){
						cr->log_type |= MOSQ_LOG_WEBSOCKETS;
#endif
					}инако ако(!strcmp(token, "all")){
						cr->log_type = MOSQ_LOG_ALL;
					}инако{
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'log_type' value (%s).", token);
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "max_connections")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ако(conf__parse_int(&token, token, &cur_listener->max_connections, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(cur_listener->max_connections < 0){
						cur_listener->max_connections = -1;
					}
				}инако ако(!strcmp(token, "maximum_qos") || !strcmp(token, "max_qos")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ако(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(tmp_int < 0 || tmp_int > 2){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: 'max_qos' must be between 0 and 2 inclusive.");
						врати MOSQ_ERR_INVAL;
					}
					cur_listener->max_qos = (uint8_t)tmp_int;
				}инако ако(!strcmp(token, "max_inflight_bytes")){
					ако(conf__parse_int(&token, "max_inflight_bytes", &tmp_int, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_inflight_bytes = (size_t)tmp_int;
				}инако ако(!strcmp(token, "max_inflight_messages")){
					ако(conf__parse_int(&token, "max_inflight_messages", &tmp_int, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(tmp_int < 0 || tmp_int == UINT16_MAX){
						tmp_int = 0;
					}инако ако(tmp_int > UINT16_MAX){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: 'max_inflight_messages' must be <= 65535.");
						врати MOSQ_ERR_INVAL;
					}
					config->max_inflight_messages = (uint16_t)tmp_int;
				}инако ако(!strcmp(token, "max_keepalive")){
					ако(conf__parse_int(&token, "max_keepalive", &tmp_int, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'max_keepalive' value (%d).", tmp_int);
						врати MOSQ_ERR_INVAL;
					}
					config->max_keepalive = (uint16_t)tmp_int;
				}инако ако(!strcmp(token, "max_packet_size")){
					ако(conf__parse_int(&token, "max_packet_size", &tmp_int, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(tmp_int < 20){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: 'max_packet_size' must be >= 20.");
						врати MOSQ_ERR_INVAL;
					}
					config->max_packet_size = (uint32_t)tmp_int;
				}инако ако(!strcmp(token, "max_queued_bytes")){
					ако(conf__parse_int(&token, "max_queued_bytes", &tmp_int, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_bytes = (size_t)tmp_int;
				}инако ако(!strcmp(token, "max_queued_messages")){
					ако(conf__parse_int(&token, "max_queued_messages", &tmp_int, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_messages = tmp_int;
				}инако ако(!strcmp(token, "memory_limit")){
					ssize_t lim;
					ако(conf__parse_ssize_t(&token, "memory_limit", &lim, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(lim < 0){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'memory_limit' value (%ld).", lim);
						врати MOSQ_ERR_INVAL;
					}
					mosquitto_memory_set_limit((size_t)lim);
				}инако ако(!strcmp(token, "message_size_limit")){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Note: It is recommended to replace `message_size_limit` with `max_packet_size`.");
					ако(conf__parse_int(&token, "message_size_limit", (цео *)&config->message_size_limit, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(config->message_size_limit > MQTT_MAX_PAYLOAD){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'message_size_limit' value (%u).", config->message_size_limit);
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "mount_point")){
					REQUIRE_LISTENER(token);
					mosquitto_FREE(cur_listener->mount_point);
					ако(conf__parse_string(&token, "mount_point", &cur_listener->mount_point, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(mosquitto_pub_topic_check(cur_listener->mount_point) != MOSQ_ERR_SUCCESS){
						log__printf(NULL, MOSQ_LOG_ERR,
								"Error: Invalid 'mount_point' value (%s). Does it contain a wildcard character?",
								cur_listener->mount_point);
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "notifications")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ако(conf__parse_bool(&token, "notifications", &cur_bridge->notifications, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "notifications_local_only")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ако(conf__parse_bool(&token, "notifications_local_only", &cur_bridge->notifications_local_only, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "notification_topic")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ако(conf__parse_string(&token, "notification_topic", &cur_bridge->notification_topic, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "password") || !strcmp(token, "remote_password")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ако(conf__parse_string(&token, "bridge remote_password", &cur_bridge->remote_password, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "password_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->password_data.password_file);
					ако(conf__parse_string(&token, "password_file", &cur_security_options->password_data.password_file, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "per_listener_settings")){
					OPTION_DEPRECATED(token, "Please see the documentation for how to achieve the same effect.");
					ако(config->per_listener_settings){
						/* Once this is set, don't let it be unset. It should be the first config option ideally. */
						continue;
					}
					ако(conf__parse_bool(&token, "per_listener_settings", &config->per_listener_settings, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(cur_security_options && config->per_listener_settings){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: per_listener_settings must be set before any other security settings.");
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "persistence") || !strcmp(token, "retained_persistence")){
					ако(conf__parse_bool(&token, token, &config->persistence, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "persistence_file")){
					ако(conf__parse_string(&token, "persistence_file", &config->persistence_file, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "persistence_location")){
					ако(conf__parse_string(&token, "persistence_location", &config->persistence_location, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "persistent_client_expiration")){
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
							врати MOSQ_ERR_INVAL;
					}
					token[strlen(token)-1] = '\0';
					config->persistent_client_expiration = atoi(token)*expiration_mult;
					ако(config->persistent_client_expiration <= 0){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "pid_file")){
					ако(reload){
						continue;        /* pid file not valid for reloading. */
					}
					ако(conf__parse_string(&token, "pid_file", &config->pid_file, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "port")){
					OPTION_DEPRECATED(token, "Please use 'listener' instead.");
					config->local_only = false;
					ако(reload){
						continue;        /* Listeners not valid for reloading. */

					}
					ако(config__create_default_listener(config, token)){
						врати MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					ако(config->default_listener->port){
						log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Default listener port specified multiple times. Only the latest will be used.");
					}
					ако(conf__parse_int(&token, "port", &tmp_int, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(tmp_int < 1 || tmp_int > UINT16_MAX){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						врати MOSQ_ERR_INVAL;
					}
					config->default_listener->port = (uint16_t)tmp_int;
				}инако ако(!strcmp(token, "protocol")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "protocol");

					ако(!strcmp(token, "mqtt")){
						cur_listener->protocol = mp_mqtt;
						/*
						}else if(!strcmp(token, "mqttsn")){
						    cur_listener->protocol = mp_mqttsn;
						*/
					}инако ако(!strcmp(token, "websockets")){
#ifdef WITH_WEBSOCKETS
						cur_listener->protocol = mp_websockets;
#инако
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Websockets support not available.");
						врати MOSQ_ERR_INVAL;
#endif
					}инако ако(!strcmp(token, "http_api")){
#ifdef WITH_HTTP_API
						cur_listener->protocol = mp_http_api;
#инако
						log__printf(NULL, MOSQ_LOG_ERR, "Error: HTTP API support not available.");
						врати MOSQ_ERR_INVAL;
#endif
					}инако{
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'protocol' value (%s).", token);
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "proxy_protocol_v2_require_tls")){
#ако defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(NULL, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					врати MOSQ_ERR_INVAL;
#endif
					REQUIRE_LISTENER(token);
					ако(conf__parse_bool(&token, "proxy_protocol_v2_require_tls", &cur_listener->proxy_protocol_v2_require_tls, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "psk_file")){
#ifdef FINAL_WITH_TLS_PSK
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					ако(cur_listener && cur_listener->certfile){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						врати MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_security_options->psk_file);
					ако(conf__parse_string(&token, "psk_file", &cur_security_options->psk_file, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#endif
				}инако ако(!strcmp(token, "psk_hint")){
#ifdef FINAL_WITH_TLS_PSK
					ако(reload){
						continue;        /* PSK file not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ако(conf__parse_string(&token, "psk_hint", &cur_listener->psk_hint, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#endif
				}инако ако(!strcmp(token, "queue_qos0_messages")){
					ако(conf__parse_bool(&token, token, &config->queue_qos0_messages, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "require_certificate")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ако(conf__parse_bool(&token, "require_certificate", &cur_listener->require_certificate, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}инако ако(!strcmp(token, "restart_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					cur_bridge->backoff_cap = 0; /* set backoff to constant mode, unless cap is specified further down */
					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "restart_timeout");

					cur_bridge->restart_timeout = atoi(token);
					cur_bridge->backoff_base = 0;
					cur_bridge->backoff_cap = 0;
					ако(cur_bridge->restart_timeout < 1){
						log__printf(NULL, MOSQ_LOG_NOTICE, "restart_timeout interval too low, using 1 second.");
						cur_bridge->restart_timeout = 1;
					}инако ако(cur_bridge->restart_timeout > 3600){
						log__printf(NULL, MOSQ_LOG_NOTICE, "restart_timeout interval too high, using 3600 seconds.");
						cur_bridge->restart_timeout = 3600;
					}
					token = strtok_r(NULL, " ", &saveptr);
					ако(token){
						cur_bridge->backoff_base = cur_bridge->restart_timeout;
						cur_bridge->backoff_cap = atoi(token);
						ако(cur_bridge->backoff_cap < cur_bridge->backoff_base){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: backoff cap is lower than the base in restart_timeout.");
							врати MOSQ_ERR_INVAL;
						}инако ако(cur_bridge->backoff_cap > 7200){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: backoff cap too high, using 7200 seconds.");
							cur_bridge->backoff_cap = 7200;
						}

						token = strtok_r(NULL, " ", &saveptr);
						ако(token){
							cur_bridge->stable_connection_period = atoi(token);
							ако(cur_bridge->stable_connection_period < 0){
								log__printf(NULL, MOSQ_LOG_ERR, "Error: stable connection period cannot be negative.");
								врати MOSQ_ERR_INVAL;
							}
						}
					}
					cur_bridge->restart_timeout *= 1000; /* backoff is tracked in ms */
					cur_bridge->backoff_base *= 1000;
					cur_bridge->backoff_cap *= 1000;
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "retain_available")){
					ако(conf__parse_bool(&token, token, &config->retain_available, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "retain_expiry_interval")){
					ако(conf__parse_int(&token, token, &config->retain_expiry_interval, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(config->retain_expiry_interval < 1){
						log__printf(NULL, MOSQ_LOG_WARNING, "Error: retain_expiry_interval must be >= 1.");
						врати MOSQ_ERR_INVAL;
					}инако ако(config->retain_expiry_interval > 10000000){
						log__printf(NULL, MOSQ_LOG_WARNING, "Warning: retain_expiry_interval being capped at 19 years.");
						config->retain_expiry_interval = 10000000;
					}
					config->retain_expiry_interval *= 60;
				}инако ако(!strcmp(token, "retry_interval")){
					OPTION_UNAVAILABLE(token);
				}инако ако(!strcmp(token, "round_robin")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ако(conf__parse_bool(&token, "round_robin", &cur_bridge->round_robin, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "set_tcp_nodelay")){
					ако(conf__parse_bool(&token, "set_tcp_nodelay", &config->set_tcp_nodelay, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "start_type")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					ако(!strcmp(token, "automatic")){
						cur_bridge->start_type = bst_automatic;
					}инако ако(!strcmp(token, "lazy")){
						cur_bridge->start_type = bst_lazy;
					}инако ако(!strcmp(token, "manual")){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Manual start_type not supported.");
						врати MOSQ_ERR_INVAL;
					}инако ако(!strcmp(token, "once")){
						cur_bridge->start_type = bst_once;
					}инако{
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'start_type' value in configuration (%s).", token);
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "socket_domain")){
					ако(reload){
						continue;        /* socket_domain not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					ако(!strcmp(token, "ipv4")){
						cur_listener->socket_domain = AF_INET;
					}инако ако(!strcmp(token, "ipv6")){
						cur_listener->socket_domain = AF_INET6;
					}инако{
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'socket_domain' value '%s' in configuration.", token);
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "sys_interval")){
					ако(conf__parse_int(&token, "sys_interval", &config->sys_interval, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(config->sys_interval < 0 || config->sys_interval > 65535){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'sys_interval' value (%d).", config->sys_interval);
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "threshold")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ако(conf__parse_int(&token, "threshold", &cur_bridge->threshold, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(cur_bridge->threshold < 1){
						log__printf(NULL, MOSQ_LOG_NOTICE, "threshold too low, using 1 message.");
						cur_bridge->threshold = 1;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "tls_engine")){
#ifdef WITH_TLS
					ако(reload){
						continue;        /* tls_engine not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ако(conf__parse_string(&token, "tls_engine", &cur_listener->tls_engine, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}инако ако(!strcmp(token, "tls_engine_kpass_sha1")){
#ifdef WITH_TLS
					ако(reload){
						continue;        /* tls_engine not valid for reloading. */

					}
					char *kpass_sha = NULL, *kpass_sha_bin = NULL;
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ако(conf__parse_string(&token, "tls_engine_kpass_sha1", &kpass_sha, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(mosquitto__hex2bin_sha1(kpass_sha, (unsigned char **)&kpass_sha_bin) != MOSQ_ERR_SUCCESS){
						mosquitto_FREE(kpass_sha);
						врати MOSQ_ERR_INVAL;
					}
					mosquitto_free(cur_listener->tls_engine_kpass_sha1);
					cur_listener->tls_engine_kpass_sha1 = kpass_sha_bin;
					mosquitto_FREE(kpass_sha);
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}инако ако(!strcmp(token, "tls_keyform")){
#ifdef WITH_TLS
					ако(reload){
						continue;        /* tls_engine not valid for reloading. */

					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					char *keyform = NULL;
					ако(conf__parse_string(&token, "tls_keyform", &keyform, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					cur_listener->tls_keyform = mosq_k_pem;
					ако(!strcmp(keyform, "engine")){
						cur_listener->tls_keyform = mosq_k_engine;
					}
					mosquitto_FREE(keyform);
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}инако ако(!strcmp(token, "tls_version")){
#ако defined(WITH_TLS)
					ако(reload){
						continue;        /* tls_version not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					ако(conf__parse_string(&token, "tls_version", &cur_listener->tls_version, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}инако ако(!strcmp(token, "topic")){
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
					ако(token[0] == '"' && token [1] != '"'){
						ако(strchr(saveptr, '"') == NULL){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Missing closing quote in topic value (%s).", saveptr);
							врати MOSQ_ERR_INVAL;
						}

						char *topic_in_quotes = strtok_r(NULL, "\"", &saveptr);
						size_t tlen = 1;
						ако(topic_in_quotes){
							tlen = strlen(topic_in_quotes);
						}

						topic = mosquitto_malloc(strlen(token) + tlen + 1);
						ако(!topic){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
							врати MOSQ_ERR_NOMEM;
						}

						strcpy(topic, token + 1);
						strcat(topic, " ");
						ако(topic_in_quotes){
							strcat(topic, topic_in_quotes);
						}
					}инако{
						topic = mosquitto_strdup(token);
						ако(!topic){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
							врати MOSQ_ERR_NOMEM;
						}
					}

					token = strtok_r(NULL, " ", &saveptr);
					ако(token){
						ако(!strcasecmp(token, "out")){
							direction = bd_out;
						}инако ако(!strcasecmp(token, "in")){
							direction = bd_in;
						}инако ако(!strcasecmp(token, "both")){
							direction = bd_both;
						}инако{
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge topic direction '%s'.", token);
							mosquitto_FREE(topic);
							врати MOSQ_ERR_INVAL;
						}
						token = strtok_r(NULL, " ", &saveptr);
						ако(token){
							ако(token[0] == '#'){
								(void)strtok_r(NULL, "", &saveptr);
							}
							qos = (uint8_t)atoi(token);
							ако(qos > 2){
								log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge QoS level '%s'.", token);
								mosquitto_FREE(topic);
								врати MOSQ_ERR_INVAL;
							}

							token = strtok_r(NULL, " ", &saveptr);
							ако(token){
								ако(!strcmp(token, "\"\"") || token[0] == '#'){
									local_prefix = NULL;
									ако(token[0] == '#'){
										(void)strtok_r(NULL, "", &saveptr);
									}
								}инако{
									local_prefix = token;
								}

								token = strtok_r(NULL, " ", &saveptr);
								ако(token){
									ако(!strcmp(token, "\"\"") || token[0] == '#'){
										remote_prefix = NULL;
									}инако{
										remote_prefix = token;
									}
								}
							}
						}
					}

					ако(bridge__add_topic(cur_bridge, topic, direction, qos, local_prefix, remote_prefix)){
						mosquitto_FREE(topic);
						врати MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(topic);
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "max_topic_alias")){
					REQUIRE_LISTENER(token);
					ако(conf__parse_int(&token, "max_topic_alias", &tmp_int, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias' value in configuration.");
						врати MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias = (uint16_t)tmp_int;
				}инако ако(!strcmp(token, "max_topic_alias_broker")){
					REQUIRE_LISTENER(token);
					ако(conf__parse_int(&token, "max_topic_alias_broker", &tmp_int, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias_broker' value in configuration.");
						врати MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias_broker = (uint16_t)tmp_int;
				}инако ако(!strcmp(token, "try_private")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ако(conf__parse_bool(&token, "try_private", &cur_bridge->try_private, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "upgrade_outgoing_qos")){
					ако(conf__parse_bool(&token, token, &config->upgrade_outgoing_qos, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "use_identity_as_username")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ако(conf__parse_bool(&token, "use_identity_as_username", &cur_listener->use_identity_as_username, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}инако ако(!strcmp(token, "use_subject_as_username")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ако(conf__parse_bool(&token, "use_subject_as_username", &cur_listener->use_subject_as_username, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}инако ако(!strcmp(token, "user")){
					ако(reload){
						continue;        /* Drop privileges user not valid for reloading. */
					}
					mosquitto_FREE(config->user);
					ако(conf__parse_string(&token, "user", &config->user, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "use_username_as_clientid")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					ако(conf__parse_bool(&token, "use_username_as_clientid", &cur_listener->use_username_as_clientid, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
				}инако ако(!strcmp(token, "username") || !strcmp(token, "remote_username")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					ако(conf__parse_string(&token, "bridge remote_username", &cur_bridge->remote_username, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}инако ако(!strcmp(token, "websockets_log_level")){
#ако defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					ако(conf__parse_int(&token, "websockets_log_level", &config->websockets_log_level, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
#endif
				}инако ако(!strcmp(token, "websockets_headers_size") || !strcmp(token, "packet_buffer_size")){
					ако(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					ако(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(NULL, MOSQ_LOG_WARNING, "Error: Packet buffer size must be between 0 and 65535 inclusive.");
						врати MOSQ_ERR_INVAL;
					}
					config->packet_buffer_size = (uint16_t)tmp_int;
				}инако ако(!strcmp(token, "websockets_origin")){
#ifdef WITH_WEBSOCKETS
#  ако LWS_LIBRARY_VERSION_NUMBER >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN
					REQUIRE_LISTENER(token);
					ws_origins = mosquitto_realloc(cur_listener->ws_origins, sizeof(char *)*(size_t)(cur_listener->ws_origin_count+1));
					ако(ws_origins == NULL){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
						врати MOSQ_ERR_NOMEM;
					}
					ws_origins[cur_listener->ws_origin_count] = NULL;
					cur_listener->ws_origins = ws_origins;
					ако(conf__parse_string(&token, "websockets_origin", &ws_origins[cur_listener->ws_origin_count], &saveptr)){
						врати MOSQ_ERR_INVAL;
					}
					cur_listener->ws_origin_count++;
#  инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: websockets_origin support not available, libwebsockets version is too old.");
#  endif
#инако
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Websockets support not available.");
#endif
				}инако{
					log__printf(NULL, MOSQ_LOG_ERR, "Error: Unknown configuration variable '%s'.", token);
					врати MOSQ_ERR_INVAL;
				}
			}
		}
	}
	врати MOSQ_ERR_SUCCESS;
}


цео config__read_file(struct mosquitto__config *config, bool reload, const char *file, struct config_recurse *cr, цео level, цео *lineno)
{
	цео rc;
	FILE *fptr = NULL;
	char *buf;
	цео buflen;
#ifndef WIN32
	DIR *dir;
#endif

#ifndef WIN32
	dir = opendir(file);
	ако(dir){
		closedir(dir);
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Config file '%s' is a directory.", file);
		врати 1;
	}
#endif

	fptr = mosquitto_fopen(file, "rt", false);
	ако(!fptr){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to open config file '%s'.", file);
		врати 1;
	}

	buflen = 1000;
	buf = mosquitto_malloc((size_t)buflen);
	ако(!buf){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		fclose(fptr);
		врати MOSQ_ERR_NOMEM;
	}

	rc = config__read_file_core(config, reload, cr, level, lineno, fptr, &buf, &buflen);
	mosquitto_FREE(buf);
	fclose(fptr);

	врати rc;
}


static цео config__check_proxy(struct mosquitto__config *config)
{
	за(цео i=0; i<config->listener_count; i++){
		struct mosquitto__listener *l = &config->listeners[i];

		ако(l->enable_proxy_protocol == 2){
#ifdef WITH_TLS
			ако(l->use_subject_as_username){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: use_subject_as_username cannot be used with `enable_proxy_protocol 2`.");
				врати MOSQ_ERR_INVAL;
			}

			ако(l->certfile || l->keyfile){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: certfile and keyfile cannot be used with `enable_proxy_protocol 2`.");
				врати MOSQ_ERR_INVAL;
			}
#endif
		}
	}

	врати MOSQ_ERR_SUCCESS;
}


static цео config__check(struct mosquitto__config *config)
{
	/* Checks that are easy to make after the config has been loaded. */

	const char *id_prefix;
	цео id_prefix_len;
	ако(config->security_options.auto_id_prefix){
		id_prefix = config->security_options.auto_id_prefix;
		id_prefix_len = config->security_options.auto_id_prefix_len;
	}инако{
		id_prefix = "auto-";
		id_prefix_len = (цео)strlen("auto-");
	}

	/* Default to auto_id_prefix = 'auto-' if none set. */
	за(цео i=0; i<config->listener_count; i++){
		ако(!config->listeners[i].security_options->auto_id_prefix){
			config->listeners[i].security_options->auto_id_prefix = mosquitto_strdup(id_prefix);
			ако(!config->listeners[i].security_options->auto_id_prefix){
				врати MOSQ_ERR_NOMEM;
			}
			config->listeners[i].security_options->auto_id_prefix_len = (uint16_t)id_prefix_len;
		}
	}

	врати config__check_proxy(config);
}

#ifdef WITH_BRIDGE


static цео config__check_bridges(struct mosquitto__config *config)
{
	struct mosquitto__bridge *bridge1, *bridge2;
	char hostname[256];
	size_t len;

	/* Check for bridge duplicate local_clientid, need to generate missing IDs
	 * first. */
	за(цео i=0; i<config->bridge_count; i++){
		bridge1 = config->bridges[i];

		ако(!bridge1->remote_clientid){
			ако(!gethostname(hostname, 256)){
				len = strlen(hostname) + strlen(bridge1->name) + 2;
				bridge1->remote_clientid = mosquitto_malloc(len);
				ако(!bridge1->remote_clientid){
					врати MOSQ_ERR_NOMEM;
				}
				snprintf(bridge1->remote_clientid, len, "%s.%s", hostname, bridge1->name);
			}инако{
				врати 1;
			}
		}

		ако(!bridge1->local_clientid){
			len = strlen(bridge1->remote_clientid) + strlen("local.") + 2;
			bridge1->local_clientid = mosquitto_malloc(len);
			ако(!bridge1->local_clientid){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
				врати MOSQ_ERR_NOMEM;
			}
			snprintf(bridge1->local_clientid, len, "local.%s", bridge1->remote_clientid);
		}
	}

	за(цео i=0; i<config->bridge_count; i++){
		bridge1 = config->bridges[i];
		за(цео j=i+1; j<config->bridge_count; j++){
			bridge2 = config->bridges[j];
			ако(!strcmp(bridge1->local_clientid, bridge2->local_clientid)){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Bridge local_clientid "
						"'%s' is not unique. Try changing or setting the "
						"local_clientid value for one of the bridges.",
						bridge1->local_clientid);
				врати MOSQ_ERR_INVAL;
			}
		}
	}

#ifdef WITH_TLS
	/* Check for missing TLS cafile/capath/certfile/keyfile */
	за(цео i=0; i<config->listener_count; i++){
		bool cafile = !!config->listeners[i].cafile;
		bool capath = !!config->listeners[i].capath;
		bool certfile = !!config->listeners[i].certfile;
		bool keyfile = !!config->listeners[i].keyfile;

		ако((certfile && !keyfile) || (!certfile && keyfile)){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Both certfile and keyfile must be provided to enable a TLS listener.");
			врати MOSQ_ERR_INVAL;
		}
		ако(cafile && !certfile){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: cafile specified without certfile and keyfile.");
			врати MOSQ_ERR_INVAL;
		}
		ако(capath && !certfile){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: capath specified without certfile and keyfile.");
			врати MOSQ_ERR_INVAL;
		}
	}
#endif
	врати MOSQ_ERR_SUCCESS;
}
#endif


static цео conf__parse_bool(char **token, const char *name, bool *value, char **saveptr)
{
	*token = strtok_r(NULL, " ", saveptr);
	ако(*token){
		ако(!strcmp(*token, "false") || !strcmp(*token, "0")){
			*value = false;
		}инако ако(!strcmp(*token, "true") || !strcmp(*token, "1")){
			*value = true;
		}инако{
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid '%s' value (%s).", name, *token);
			врати MOSQ_ERR_INVAL;
		}
	}инако{
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		врати MOSQ_ERR_INVAL;
	}

	врати MOSQ_ERR_SUCCESS;
}


static цео conf__parse_int(char **token, const char *name, цео *value, char **saveptr)
{
	*token = strtok_r(NULL, " ", saveptr);
	ако(*token){
		char *endptr = NULL;
		long l = strtol(*token, &endptr, 0);
		ако(endptr == *token || endptr[0] != '\0'){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: '%s' value not a number.", name);
			врати MOSQ_ERR_INVAL;
		}
		ако(l > INT_MAX || l < INT_MIN){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: '%s' value out of range.", name);
			врати MOSQ_ERR_INVAL;
		}
		*value = (цео)l;
	}инако{
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		врати MOSQ_ERR_INVAL;
	}

	врати MOSQ_ERR_SUCCESS;
}


static цео conf__parse_ssize_t(char **token, const char *name, ssize_t *value, char **saveptr)
{
	*token = strtok_r(NULL, " ", saveptr);
	ако(*token){
		*value = atol(*token);
	}инако{
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		врати MOSQ_ERR_INVAL;
	}

	врати MOSQ_ERR_SUCCESS;
}


static цео conf__parse_string(char **token, const char *name, char **value, char **saveptr)
{
	size_t tlen;

	*token = strtok_r(NULL, "", saveptr);
	ако(*token){
		ако(*value){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Duplicate '%s' value in configuration.", name);
			врати MOSQ_ERR_INVAL;
		}
		/* Deal with multiple spaces at the beginning of the string. */
		*token = mosquitto_trimblanks(*token);
		ако(strlen(*token) == 0){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
			врати MOSQ_ERR_INVAL;
		}

		tlen = strlen(*token);
		ако(tlen > UINT16_MAX){
			врати MOSQ_ERR_INVAL;
		}
		ако(mosquitto_validate_utf8(*token, (uint16_t)tlen)){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Malformed UTF-8 in configuration.");
			врати MOSQ_ERR_INVAL;
		}
		*value = mosquitto_strdup(*token);
		ако(!*value){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
			врати MOSQ_ERR_NOMEM;
		}
	}инако{
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		врати MOSQ_ERR_INVAL;
	}
	врати MOSQ_ERR_SUCCESS;
}
