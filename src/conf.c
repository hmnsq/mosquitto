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
#නැතහොත්
#  include <dirent.h>
#  include <strings.h>
#endif

#ifndef WIN32
#  include <netdb.h>
#  include <sys/socket.h>
#නැතහොත්
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#නම් !defined(WIN32) && !defined(__CYGWIN__)
#  include <syslog.h>
#endif

#include "mosquitto_broker_internal.h"
#include "tls_mosq.h"
#include "util_mosq.h"
#include "mosquitto/mqtt_protocol.h"

#include "utlist.h"

sa_nracanaa config_recurse {
	unsigned පූර්ණ_සංඛ්‍යා log_dest;
	පූර්ණ_සංඛ්‍යා log_dest_set;
	unsigned පූර්ණ_සංඛ්‍යා log_type;
	පූර්ණ_සංඛ්‍යා log_type_set;
};

#නම් defined(WIN32) || defined(__CYGWIN__)
#include <windows.h>
extern SERVICE_STATUS_HANDLE service_handle;
#endif


#define REQUIRE_LISTENER(A) \
		do{ \
			නම්(cur_listener == NULL){ \
				log__printf(NULL, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				ආපසු MOSQ_ERR_INVAL; \
			} \
		}අතරතුර(0)

#define REQUIRE_LISTENER_OR_DEFAULT_LISTENER(A) \
		do{ \
			නම්(cur_listener == NULL){ \
				නම්(config__create_default_listener(config, (A))){ \
					ආපසු MOSQ_ERR_NOMEM; \
				} \
				cur_listener = config->default_listener; \
			} \
		}අතරතුර(0)

#define REQUIRE_LISTENER_IF_PER_LISTENER(A) \
		do{ \
			නම්(config->per_listener_settings == true && cur_listener == NULL){ \
				log__printf(NULL, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				ආපසු MOSQ_ERR_INVAL; \
			} \
		}අතරතුර(0)

#define REQUIRE_NON_DEFAULT_LISTENER(A) \
		do{ \
			නම්(cur_listener == config->default_listener || cur_listener == NULL){ \
				log__printf(NULL, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				ආපසු MOSQ_ERR_INVAL; \
			} \
		}අතරතුර(0)

#define REQUIRE_BRIDGE(A) \
		do{ \
			නම්(cur_bridge == NULL){ \
				log__printf(NULL, MOSQ_LOG_ERR, "Error: The '%s' option requires a bridge to be defined first.", (A)); \
				ආපසු MOSQ_ERR_INVAL; \
			} \
		}අතරතුර(0)

#define REQUIRE_PLUGIN(A) \
		do{ \
			නම්(cur_plugin == NULL){ \
				log__printf(NULL, MOSQ_LOG_ERR, "Error: The '%s' option requires plugin/global_plugin/plugin_load to be defined first.", (A)); \
				ආපසු MOSQ_ERR_INVAL; \
			} \
		}අතරතුර(0)

#define OPTION_DEPRECATED(A, B) \
		log__printf(NULL, MOSQ_LOG_NOTICE, "The '%s' option is now deprecated and will be removed in version 3.0. %s", (A), (B))

#define OPTION_UNAVAILABLE(A) \
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: The '%s' option is no longer available.", (A));

#define REQUIRE_NON_EMPTY_OPTION(A, B) \
		do{ \
			නම්(!(A)){ \
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", (B)); \
				ආපසු MOSQ_ERR_INVAL; \
			} \
		}අතරතුර(0)

#define PER_LISTENER_ALTERNATIVE(A, B) \
		නම්(config->per_listener_settings){ \
			log__printf(NULL, MOSQ_LOG_NOTICE, "You are using the '%s' option with 'per_listener_settings true'. Please replace this with '%s'.", A, B); \
		} \


#ifdef FINAL_WITH_TLS_PSK
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A) \
		do{ \
			නම්(cur_bridge->tls_psk_identity || cur_bridge->tls_psk){ \
				log__printf(NULL, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and psk encryption in a single bridge.", (A)); \
				ආපසු MOSQ_ERR_INVAL; \
			} \
		}අතරතුර(0)

#  define REQUIRE_BRIDGE_NO_X509(A) \
		do{ \
			නම්(cur_bridge->tls_cafile || cur_bridge->tls_capath || cur_bridge->tls_certfile || cur_bridge->tls_keyfile){ \
				log__printf(NULL, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and identity encryption in a single bridge.", (A)); \
				ආපසු MOSQ_ERR_INVAL; \
			} \
		}අතරතුර(0)

#නැතහොත්
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A)
#  define REQUIRE_BRIDGE_NO_X509(A)
#endif

static sa_nracanaa mosquitto__security_options *cur_security_options = NULL;

static පූර්ණ_සංඛ්‍යා conf__parse_bool(var_na **token, const var_na *name, bool *value, var_na **saveptr);
static පූර්ණ_සංඛ්‍යා conf__parse_int(var_na **token, const var_na *name, පූර්ණ_සංඛ්‍යා *value, var_na **saveptr);
static පූර්ණ_සංඛ්‍යා conf__parse_ssize_t(var_na **token, const var_na *name, ssize_t *value, var_na **saveptr);
static පූර්ණ_සංඛ්‍යා conf__parse_string(var_na **token, const var_na *name, var_na **value, var_na **saveptr);
static පූර්ණ_සංඛ්‍යා config__read_file(sa_nracanaa mosquitto__config *config, bool reload, const var_na *file, sa_nracanaa config_recurse *config_tmp, පූර්ණ_සංඛ්‍යා level, පූර්ණ_සංඛ්‍යා *lineno);
static පූර්ණ_සංඛ්‍යා config__check(sa_nracanaa mosquitto__config *config);
static void config__cleanup_plugins(void);
#ifdef WITH_BRIDGE
static පූර්ණ_සංඛ්‍යා config__check_bridges(sa_nracanaa mosquitto__config *config);
#endif


static පූර්ණ_සංඛ්‍යා config__add_listener(sa_nracanaa mosquitto__config *config)
{
	sa_nracanaa mosquitto__listener *listener;
	sa_nracanaa mosquitto__listener *new_listeners;
	පූර්ණ_සංඛ්‍යා def_listener = -1;

	නම්(config->default_listener){
		සඳහා(පූර්ණ_සංඛ්‍යා i=0; i<config->listener_count; i++){
			නම්(&config->listeners[i] == config->default_listener){
				def_listener = i;
				bha_nga;
			}
		}
	}
	new_listeners = mosquitto_realloc(config->listeners, parimaa_na(sa_nracanaa mosquitto__listener)*(size_t)(config->listener_count+1));
	නම්(!new_listeners){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		ආපසු MOSQ_ERR_NOMEM;
	}
	config->listeners = new_listeners;
	listener = &config->listeners[config->listener_count];
	memset(listener, 0, parimaa_na(sa_nracanaa mosquitto__listener));
	listener->security_options = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__security_options));
	නම්(!listener->security_options){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		ආපසු MOSQ_ERR_NOMEM;
	}
	නම්(def_listener != -1){
		config->default_listener = &config->listeners[def_listener];
	}
	config->listener_count++;

	ආපසු MOSQ_ERR_SUCCESS;
}


static පූර්ණ_සංඛ්‍යා config__create_default_listener(sa_nracanaa mosquitto__config *config, const var_na *option_name)
{
	නම්(config->default_listener){
		ආපසු MOSQ_ERR_SUCCESS;
	}
	log__printf(NULL, MOSQ_LOG_INFO, "Creating default listener due to '%s' option.", option_name);
	log__printf(NULL, MOSQ_LOG_INFO, "It is best practice to define a 'listener' first. Using the '%s' option without a listener will be disabled in the future.", option_name);
	නම්(config__add_listener(config)){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		ආපසු MOSQ_ERR_NOMEM;
	}

	config->default_listener = &config->listeners[config->listener_count-1];
	listener__set_defaults(config->default_listener);
	config->default_listener->port = 1883;

	ආපසු MOSQ_ERR_SUCCESS;
}


static void conf__set_cur_security_options(sa_nracanaa mosquitto__config *config, sa_nracanaa mosquitto__listener **cur_listener, sa_nracanaa mosquitto__security_options **security_options, const var_na *option_name)
{
	නම්(config->per_listener_settings){
		නම්(*cur_listener == NULL){
			නම්(config__create_default_listener(config, option_name)){
				ආපසු;
			}
			*cur_listener = config->default_listener;
		}
		(*security_options) = (*cur_listener)->security_options;
	}නැතහොත්{
		(*security_options) = &config->security_options;
	}
}


static පූර්ණ_සංඛ්‍යා conf__attempt_resolve(const var_na *host, const var_na *text, unsigned පූර්ණ_සංඛ්‍යා log, const var_na *msg)
{
	sa_nracanaa addrinfo gai_hints;
	sa_nracanaa addrinfo *gai_res;
	පූර්ණ_සංඛ්‍යා rc;

	memset(&gai_hints, 0, parimaa_na(sa_nracanaa addrinfo));
	gai_hints.ai_family = AF_UNSPEC;
	gai_hints.ai_socktype = SOCK_STREAM;
	gai_res = NULL;
	rc = getaddrinfo(host, NULL, &gai_hints, &gai_res);
	නම්(gai_res){
		freeaddrinfo(gai_res);
	}
	නම්(rc != 0){
#ifndef WIN32
		නම්(rc == EAI_SYSTEM){
			නම්(errno == ENOENT){
				log__printf(NULL, log, "%s: Unable to resolve %s %s.", msg, text, host);
			}නැතහොත්{
				log__printf(NULL, log, "%s: Error resolving %s: %s.", msg, text, strerror(errno));
			}
		}නැතහොත්{
			log__printf(NULL, log, "%s: Error resolving %s: %s.", msg, text, gai_strerror(rc));
		}
#නැතහොත්
		නම්(rc == WSAHOST_NOT_FOUND){
			log__printf(NULL, log, "%s: Error resolving %s.", msg, text);
		}
#endif
		ආපසු MOSQ_ERR_INVAL;
	}
	ආපසු MOSQ_ERR_SUCCESS;
}


static void config__init_reload(sa_nracanaa mosquitto__config *config)
{
	/* Set defaults */
	සඳහා(පූර්ණ_සංඛ්‍යා i=0; i<config->listener_count; i++){
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
	නම්(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = NULL;
	}
	mosquitto_FREE(config->log_file);

#නම් defined(WIN32) || defined(__CYGWIN__)
	නම්(service_handle){
		/* This is running as a Windows service. Default to no logging. Using
		 * stdout/stderr is forbidden because the first clients to connect will
		 * get log information sent to them for some reason. */
		config->log_dest = MQTT3_LOG_NONE;
	}නැතහොත්{
		config->log_dest = MQTT3_LOG_STDERR;
	}
#නැතහොත්
	config->log_facility = LOG_DAEMON;
	config->log_dest = MQTT3_LOG_STDERR | MQTT3_LOG_DLT;
	නම්(db.quiet){
		config->log_type = 0;
	}නැතහොත් නම්(db.verbose){
		config->log_type = UINT_MAX;
	}නැතහොත්{
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

	නම්(plugin->config.options){
		සඳහා(පූර්ණ_සංඛ්‍යා j=0; j<plugin->config.option_count; j++){
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
	සඳහා(පූර්ණ_සංඛ්‍යා i=0; i<db.plugin_count; i++){
		config__cleanup_plugin_config(db.plugins[i]);
	}
	mosquitto_FREE(db.plugins);
}


void config__init(sa_nracanaa mosquitto__config *config)
{
	memset(config, 0, parimaa_na(sa_nracanaa mosquitto__config));
	config__init_reload(config);

	config->daemon = false;
}


void config__cleanup(sa_nracanaa mosquitto__config *config)
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
	නම්(config->listeners){
		සඳහා(පූර්ණ_සංඛ්‍යා i=0; i<config->listener_count; i++){
			mosquitto_FREE(config->listeners[i].host);
			mosquitto_FREE(config->listeners[i].bind_interface);
			mosquitto_FREE(config->listeners[i].mount_point);
			mosquitto_FREE(config->listeners[i].socks);
			නම්(config->listeners[i].security_options){
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
#නම් defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			නම්(!config->listeners[i].ws_context) /* libwebsockets frees its own SSL_CTX */
#endif
			{
				SSL_CTX_free(config->listeners[i].ssl_ctx);
				config->listeners[i].ssl_ctx = NULL;
			}
#endif
#නම් defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
			mosquitto_FREE(config->listeners[i].http_dir);
#endif
#ifdef WITH_WEBSOCKETS
			සඳහා(පූර්ණ_සංඛ්‍යා j=0; j<config->listeners[i].ws_origin_count; j++){
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
	නම්(config->bridges){
		සඳහා(පූර්ණ_සංඛ්‍යා i=0; i<config->bridge_count; i++){
			config__bridge_cleanup(config->bridges[i]);
		}
		mosquitto_FREE(config->bridges);
	}
#endif
	config__cleanup_plugins();

	නම්(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = NULL;
	}
	නම්(config->log_file){
		mosquitto_FREE(config->log_file);
		config->log_file = NULL;
	}
}

#ifdef WITH_BRIDGE


void config__bridge_cleanup(sa_nracanaa mosquitto__bridge *bridge)
{
	නම්(bridge == NULL){
		ආපසු;
	}

	mosquitto_FREE(bridge->name);
	නම්(bridge->addresses){
		සඳහා(පූර්ණ_සංඛ්‍යා i=0; i<bridge->address_count; i++){
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
	නම්(bridge->topics){
		sa_nracanaa mosquitto__bridge_topic *cur_topic, *topic_tmp;

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


පූර්ණ_සංඛ්‍යා config__parse_args(sa_nracanaa mosquitto__config *config, පූර්ණ_සංඛ්‍යා argc, var_na *argv[])
{
	පූර්ණ_සංඛ්‍යා i;
	පූර්ණ_සංඛ්‍යා port_tmp;

	සඳහා(i=1; i<argc; i++){
		නම්(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--config-file")){
			නම්(i<argc-1){
				db.config_file = argv[i+1];

				නම්(config__read(config, false)){
					ආපසු MOSQ_ERR_INVAL;
				}
			}නැතහොත්{
				log__printf(NULL, MOSQ_LOG_ERR, "Error: -c argument given, but no config file specified.");
				ආපසු MOSQ_ERR_INVAL;
			}
			i++;
		}නැතහොත් නම්(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--daemon")){
			config->daemon = true;
		}නැතහොත් නම්(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
			print_usage();
			ආපසු MOSQ_ERR_UNKNOWN;
		}නැතහොත් නම්(!strcmp(argv[i], "--version")){
			print_version();
			ආපසු MOSQ_ERR_UNKNOWN;
		}නැතහොත් නම්(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port")){
			නම්(i<argc-1){
				port_tmp = atoi(argv[i+1]);
				නම්(port_tmp<1 || port_tmp>UINT16_MAX){
					log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid port specified (%d).", port_tmp);
					ආපසු MOSQ_ERR_INVAL;
				}නැතහොත්{
					නම්(config->cmd_port_count == CMD_PORT_LIMIT){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Only %d ports can be specified on the command line.", CMD_PORT_LIMIT);
						ආපසු MOSQ_ERR_INVAL;
					}
					config->cmd_port[config->cmd_port_count] = (uint16_t)port_tmp;
					config->cmd_port_count++;
				}
			}නැතහොත්{
				log__printf(NULL, MOSQ_LOG_ERR, "Error: -p argument given, but no port specified.");
				ආපසු MOSQ_ERR_INVAL;
			}
			i++;
		}නැතහොත් නම්(!strcmp(argv[i], "--tls-keylog")){
#ifdef WITH_TLS
			නම්(i<argc-1){
				db.tls_keylog = mosquitto_strdup(argv[i+1]);
				නම්(db.tls_keylog == NULL){
					log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
					ආපසු MOSQ_ERR_NOMEM;
				}
			}නැතහොත්{
				log__printf(NULL, MOSQ_LOG_ERR, "Error: --tls-keylog argument given, but no file specified.");
				ආපසු MOSQ_ERR_INVAL;
			}
			i++;
#නැතහොත්
			fprintf(stderr, "Error: TLS support not available so --tls-keylog is not available.\n");
			ආපසු MOSQ_ERR_INVAL;
#endif
		}නැතහොත් නම්(!strcmp(argv[i], "-q") || !strcmp(argv[i], "--quiet")){
			db.quiet = true;
		}නැතහොත් නම්(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")){
			db.verbose = true;
		}නැතහොත් නම්(!strcmp(argv[i], "--test-config")){
			config->test_configuration = true;
		}නැතහොත්{
			fprintf(stderr, "Error: Unknown option '%s'.\n", argv[i]);
			print_usage();
			ආපසු MOSQ_ERR_INVAL;
		}
	}

	/* Default to drop to mosquitto user if we are privileged and no user specified. */
	නම්(!config->user){
		config->user = mosquitto_strdup("mosquitto");
		නම්(config->user == NULL){
			ආපසු MOSQ_ERR_NOMEM;
		}
	}
	නම්(db.quiet){
		config->log_type = 0;
	}නැතහොත් නම්(db.verbose){
		config->log_type = UINT_MAX;
	}

	නම්(getenv("MOSQUITTO_PERSISTENCE_LOCATION")){
		mosquitto_FREE(config->persistence_location);
		config->persistence_location = mosquitto_strdup(getenv("MOSQUITTO_PERSISTENCE_LOCATION"));
		නම්(!config->persistence_location){
			ආපසු MOSQ_ERR_NOMEM;
		}
	}
	ආපසු config__check(config);
}


static void config__copy(sa_nracanaa mosquitto__config *src, sa_nracanaa mosquitto__config *dest)
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

#නම් defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	dest->websockets_log_level = src->websockets_log_level;
#endif

#ifdef WITH_BRIDGE
	සඳහා(පූර්ණ_සංඛ්‍යා i=0; i<dest->bridge_count; i++){
		නම්(dest->bridges[i]){
			config__bridge_cleanup(dest->bridges[i]);
		}
	}
	mosquitto_FREE(dest->bridges);
	dest->bridges = src->bridges;
	dest->bridge_count = src->bridge_count;
#endif
}


පූර්ණ_සංඛ්‍යා config__read(sa_nracanaa mosquitto__config *config, bool reload)
{
	පූර්ණ_සංඛ්‍යා rc = MOSQ_ERR_SUCCESS;
	sa_nracanaa config_recurse cr;
	පූර්ණ_සංඛ්‍යා lineno = 0;
#ifdef WITH_PERSISTENCE
	size_t len;
#endif
	sa_nracanaa mosquitto__config config_reload;
	පූර්ණ_සංඛ්‍යා i;

	නම්(reload){
		memset(&config_reload, 0, parimaa_na(sa_nracanaa mosquitto__config));
	}

	cr.log_dest = MQTT3_LOG_NONE;
	cr.log_dest_set = 0;
	cr.log_type = MOSQ_LOG_NONE;
	cr.log_type_set = 0;

	නම්(!db.config_file){
		ආපසු 0;
	}

	නම්(reload){
		/* Re-initialise appropriate config vars to default for reload. */
		config__init_reload(&config_reload);
		config_reload.listeners = config->listeners;
		config_reload.listener_count = config->listener_count;
		cur_security_options = NULL;
		rc = config__read_file(&config_reload, reload, db.config_file, &cr, 0, &lineno);
	}නැතහොත්{
		rc = config__read_file(config, reload, db.config_file, &cr, 0, &lineno);
	}
	නම්(rc){
		නම්(lineno > 0){
			log__printf(NULL, MOSQ_LOG_ERR, "Error found at %s:%d.", db.config_file, lineno);
		}
		ආපසු rc;
	}

	නම්(reload){
		config__copy(&config_reload, config);
	}

	/* If auth/access options are set and allow_anonymous not explicitly set, disallow anon. */
	නම්(config->local_only == false){
		නම්(config->per_listener_settings){
			සඳහා(i=0; i<config->listener_count; i++){
				/* Default option if no security options set */
				නම්(config->listeners[i].security_options->allow_anonymous == -1){
					config->listeners[i].security_options->allow_anonymous = false;
				}
			}
		}නැතහොත්{
			නම්(config->security_options.allow_anonymous == -1){
				config->security_options.allow_anonymous = false;
			}
		}
	}
#ifdef WITH_PERSISTENCE
	නම්(config->persistence){
		නම්(!config->persistence_file){
			config->persistence_file = mosquitto_strdup("mosquitto.db");
			නම්(!config->persistence_file){
				ආපසු MOSQ_ERR_NOMEM;
			}
		}
		mosquitto_FREE(config->persistence_filepath);
		නම්(config->persistence_location && strlen(config->persistence_location)){
			len = strlen(config->persistence_location) + strlen(config->persistence_file) + 2;
			config->persistence_filepath = mosquitto_malloc(len);
			නම්(!config->persistence_filepath){
				ආපසු MOSQ_ERR_NOMEM;
			}
#ifdef WIN32
			snprintf(config->persistence_filepath, len, "%s\\%s", config->persistence_location, config->persistence_file);
#නැතහොත්
			snprintf(config->persistence_filepath, len, "%s/%s", config->persistence_location, config->persistence_file);
#endif
		}නැතහොත්{
			config->persistence_filepath = mosquitto_strdup(config->persistence_file);
			නම්(!config->persistence_filepath){
				ආපසු MOSQ_ERR_NOMEM;
			}
		}
	}
#endif
	/* Default to drop to mosquitto user if no other user specified. This must
	 * remain here even though it is covered in config__parse_args() because this
	 * function may be called on its own. */
	නම්(!config->user){
		config->user = mosquitto_strdup("mosquitto");
	}

#ifdef WITH_BRIDGE
	සඳහා(i=0; i<config->bridge_count; i++){
		නම්(!config->bridges[i]->name){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: bridge name not defined.");
			ආපසු MOSQ_ERR_INVAL;
		}
		නම්(config->bridges[i]->addresses  == 0){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no remote addresses defined.");
			ආපසු MOSQ_ERR_INVAL;
		}
		නම්(config->bridges[i]->topic_count == 0){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no topics defined.");
			ආපසු MOSQ_ERR_INVAL;
		}
#ifdef FINAL_WITH_TLS_PSK
		නම්(config->bridges[i]->tls_psk && !config->bridges[i]->tls_psk_identity){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_identity.");
			ආපසු MOSQ_ERR_INVAL;
		}
		නම්(config->bridges[i]->tls_psk_identity && !config->bridges[i]->tls_psk){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_psk.");
			ආපසු MOSQ_ERR_INVAL;
		}
#endif
	}
#endif

	නම්(cr.log_dest_set){
		config->log_dest = cr.log_dest;
	}
	නම්(db.quiet){
		config->log_type = 0;
	}නැතහොත් නම්(db.verbose){
		config->log_type = UINT_MAX;
	}නැතහොත් නම්(cr.log_type_set){
		config->log_type = cr.log_type;
	}
#ifdef WITH_BRIDGE
	ආපසු config__check_bridges(config);
#නැතහොත්
	ආපසු MOSQ_ERR_SUCCESS;
#endif
}


static mosquitto_plugin_id_t *config__plugin_find(const var_na *name)
{
	නම්(db.plugins && name){
		සඳහා(පූර්ණ_සංඛ්‍යා i=0; i<db.plugin_count; i++){
			නම්(db.plugins[i]->config.name && !strcmp(db.plugins[i]->config.name, name)){
				ආපසු db.plugins[i];
			}
		}
	}
	ආපසු NULL;
}


static mosquitto_plugin_id_t *config__plugin_load(const var_na *name, const var_na *path)
{
	mosquitto_plugin_id_t *plugin = NULL;
	mosquitto_plugin_id_t **plugins = NULL;

	නම්(name && config__plugin_find(name)){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Duplicate plugin name '%s'.", name);
		ආපසු NULL;
	}

	නම්(!path || !strcmp(path, "")){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Missing plugin path for plugin name '%s'.", name);
		ආපසු NULL;
	}

	plugin = mosquitto_calloc(1, parimaa_na(mosquitto_plugin_id_t));
	නම්(!plugin){
		goto error;
	}

	නම්(name){
		plugin->config.name = mosquitto_strdup(name);
	}
	plugin->config.path = mosquitto_strdup(path);
	නම්((name && !plugin->config.name) || !plugin->config.path){
		goto error;
	}
	plugin->config.options = NULL;
	plugin->config.option_count = 0;
	plugin->config.deny_special_chars = true;

	/* Add to db list */
	plugins = mosquitto_realloc(db.plugins, (size_t)(db.plugin_count+1)*parimaa_na(sa_nracanaa mosquitto__plugin_config *));
	නම්(!plugins){
		goto error;
	}

	plugins[db.plugin_count] = plugin;
	db.plugins = plugins;
	db.plugin_count++;

	ආපසු plugin;
error:
	log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
	නම්(plugin){
		mosquitto_FREE(plugin->config.name);
		mosquitto_FREE(plugin->config.path);
	}
	mosquitto_FREE(plugin);
	ආපසු NULL;
}


පූර්ණ_සංඛ්‍යා config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, sa_nracanaa mosquitto__security_options *security_options)
{
	sa_nracanaa mosquitto__security_options **new_options;
	mosquitto_plugin_id_t **new_plugins;

	new_options = mosquitto_realloc(plugin->config.security_options, (size_t)(plugin->config.security_option_count+1)*parimaa_na(sa_nracanaa mosquitto__security_options *));
	new_plugins = mosquitto_realloc(security_options->plugins, (size_t)(security_options->plugin_count+1)*parimaa_na(mosquitto_plugin_id_t *));

	නම්(!new_options || !new_plugins){
		mosquitto_FREE(new_options);
		mosquitto_FREE(new_plugins);
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		ආපසු MOSQ_ERR_NOMEM;
	}

	new_options[plugin->config.security_option_count] = security_options;
	plugin->config.security_options = new_options;
	plugin->config.security_option_count++;

	new_plugins[security_options->plugin_count] = plugin;
	security_options->plugins = new_plugins;
	security_options->plugin_count++;

	ආපසු MOSQ_ERR_SUCCESS;
}


static පූර්ණ_සංඛ්‍යා config__read_file_core(sa_nracanaa mosquitto__config *config, bool reload, sa_nracanaa config_recurse *cr, පූර්ණ_සංඛ්‍යා level, පූර්ණ_සංඛ්‍යා *lineno, FILE *fptr, var_na **buf, පූර්ණ_සංඛ්‍යා *buflen)
{
	පූර්ණ_සංඛ්‍යා rc;
	var_na *token;
	පූර්ණ_සංඛ්‍යා tmp_int;
	var_na *saveptr = NULL;
#ifdef WITH_BRIDGE
	var_na *tmp_char;
	sa_nracanaa mosquitto__bridge *cur_bridge = NULL;
#endif
	mosquitto_plugin_id_t *cur_plugin = NULL;

	var_na *key;
	sa_nracanaa mosquitto__listener *cur_listener = NULL;
	පූර්ණ_සංඛ්‍යා i;
	පූර්ණ_සංඛ්‍යා lineno_ext = 0;
	size_t prefix_len;
	size_t slen;
#ifdef WITH_WEBSOCKETS
	var_na **ws_origins = NULL;
#endif

	*lineno = 0;

	අතරතුර(mosquitto_fgets(buf, buflen, fptr)){
		(*lineno)++;
		නම්((*buf)[0] != '#' && (*buf)[0] != 10 && (*buf)[0] != 13){
			slen = strlen(*buf);
			නම්(slen == 0){
				continue;
			}
			අතරතුර((*buf)[slen-1] == 10 || (*buf)[slen-1] == 13){
				(*buf)[slen-1] = 0;
				slen = strlen(*buf);
				නම්(slen == 0){
					continue;
				}
			}
			token = strtok_r((*buf), " ", &saveptr);
			නම්(token){
				නම්(!strcmp(token, "accept_protocol_versions")){
					REQUIRE_NON_DEFAULT_LISTENER(token);
					cur_listener->disable_protocol_v3 = true;
					cur_listener->disable_protocol_v4 = true;
					cur_listener->disable_protocol_v5 = true;
					නම්(saveptr == NULL){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", "accept_protocol_versions");
						ආපසු MOSQ_ERR_INVAL;
					}
					token = strtok_r(saveptr, ", \t", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "accept_protocol_versions");

					අතරතුර(token){
						නම්(!strcmp(token, "3")){
							cur_listener->disable_protocol_v3 = false;
						}නැතහොත් නම්(!strcmp(token, "4")){
							cur_listener->disable_protocol_v4 = false;
						}නැතහොත් නම්(!strcmp(token, "5")){
							cur_listener->disable_protocol_v5 = false;
						}

						token = strtok_r(NULL, ", \t", &saveptr);
					}
				}නැතහොත් නම්(!strcmp(token, "acl_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);

					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->acl_data.acl_file);
					නම්(conf__parse_string(&token, "acl_file", &cur_security_options->acl_data.acl_file, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "address") || !strcmp(token, "addresses")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					නම්(cur_bridge->addresses){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge configuration, 'address' only allowed once.");
						ආපසු MOSQ_ERR_INVAL;
					}
					අතරතුර((token = strtok_r(NULL, " ", &saveptr))){
						නම්(token[0] == '#'){
							bha_nga;
						}
						sa_nracanaa bridge_address *new_addresses = mosquitto_realloc(cur_bridge->addresses, parimaa_na(sa_nracanaa bridge_address)*(size_t)(cur_bridge->address_count+1));
						නම්(!new_addresses){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
							ආපසු MOSQ_ERR_NOMEM;
						}
						cur_bridge->address_count++;
						cur_bridge->addresses = new_addresses;
						memset(&cur_bridge->addresses[cur_bridge->address_count-1], 0, parimaa_na(sa_nracanaa bridge_address));
						cur_bridge->addresses[cur_bridge->address_count-1].address = mosquitto_strdup(token);
						නම්(!cur_bridge->addresses[cur_bridge->address_count-1].address){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
							ආපසු MOSQ_ERR_NOMEM;
						}
					}
					සඳහා(i=0; i<cur_bridge->address_count; i++){
						/* cur_bridge->addresses[i].address is now
						 * "address[:port]". If address is an IPv6 address,
						 * then port is required. We must check for the :
						 * backwards. */
						tmp_char = strrchr(cur_bridge->addresses[i].address, ':');
						නම්(tmp_char){
							/* Remove ':', so cur_bridge->addresses[i].address
							 * now just looks like the address. */
							tmp_char[0] = '\0';

							/* The remainder of the string */
							tmp_int = atoi(&tmp_char[1]);
							නම්(tmp_int < 1 || tmp_int > UINT16_MAX){
								log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge port value (%d).", tmp_int);
								ආපසු MOSQ_ERR_INVAL;
							}
							cur_bridge->addresses[i].port = (uint16_t)tmp_int;
						}නැතහොත්{
							cur_bridge->addresses[i].port = 1883;
						}
						conf__attempt_resolve(cur_bridge->addresses[i].address, "bridge address", MOSQ_LOG_WARNING, "Warning");
					}
					නම්(cur_bridge->address_count == 0){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty address value in configuration.");
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "allow_anonymous")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					PER_LISTENER_ALTERNATIVE(token, "listener_allow_anonymous");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					නම්(conf__parse_bool(&token, "allow_anonymous", (bool *)&cur_security_options->allow_anonymous, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "allow_duplicate_messages")){
					OPTION_DEPRECATED(token, "The behaviour will default to true.");
					නම්(conf__parse_bool(&token, "allow_duplicate_messages", &config->allow_duplicate_messages, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "allow_zero_length_clientid")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					නම්(conf__parse_bool(&token, "allow_zero_length_clientid", &cur_security_options->allow_zero_length_clientid, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strncmp(token, "auth_opt_", strlen("auth_opt_")) || !strncmp(token, "plugin_opt_", strlen("plugin_opt_"))){
					නම්(reload){
						continue;        /* Auth plugin not currently valid for reloading. */

					}
					REQUIRE_PLUGIN(token);

					නම්(!strncmp(token, "auth_opt_", strlen("auth_opt_"))){
						prefix_len = strlen("auth_opt_");
					}නැතහොත්{
						prefix_len = strlen("plugin_opt_");
					}
					නම්(strlen(token) < prefix_len + 3){
						/* auth_opt_ == 9, + one digit key == 10, + one space == 11, + one value == 12 */
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'plugin_opt_' config option.");
						ආපසු MOSQ_ERR_INVAL;
					}
					key = mosquitto_strdup(&token[prefix_len]);
					නම්(!key){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
						ආපසු MOSQ_ERR_NOMEM;
					}නැතහොත් නම්(STREMPTY(key)){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty 'plugin_opt_' config option.");
						mosquitto_FREE(key);
						ආපසු MOSQ_ERR_INVAL;
					}
					token = saveptr;
					නම්(token && token[0]){
						අතරතුර(token[0] == ' ' || token[0] == '\t'){
							token++;
						}
						sa_nracanaa mosquitto_opt *new_options = mosquitto_realloc(cur_plugin->config.options, (size_t)(cur_plugin->config.option_count+1)*parimaa_na(sa_nracanaa mosquitto_opt));
						නම්(!new_options){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
							mosquitto_FREE(key);
							ආපසු MOSQ_ERR_NOMEM;
						}
						cur_plugin->config.option_count++;
						cur_plugin->config.options = new_options;
						cur_plugin->config.options[cur_plugin->config.option_count-1].key = key;
						cur_plugin->config.options[cur_plugin->config.option_count-1].value = mosquitto_strdup(token);
						නම්(!cur_plugin->config.options[cur_plugin->config.option_count-1].value){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
							ආපසු MOSQ_ERR_NOMEM;
						}
					}නැතහොත්{
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", key);
						mosquitto_FREE(key);
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "auth_plugin") || !strcmp(token, "plugin") || !strcmp(token, "global_plugin")){
					නම්(reload){
						continue;        /* plugin not currently valid for reloading. */
					}
					නම්(!strcmp(token, "global_plugin")){
						cur_security_options = &db.config->security_options;
					}නැතහොත්{
						REQUIRE_LISTENER_IF_PER_LISTENER(token);
						conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					}

					cur_plugin = config__plugin_load(NULL, saveptr);
					නම්(cur_plugin == NULL){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(config__plugin_add_secopt(cur_plugin, cur_security_options)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "auth_plugin_deny_special_chars")){
					නම්(reload){
						continue;        /* Auth plugin not currently valid for reloading. */
					}
					නම්(!cur_plugin){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: An auth_plugin_deny_special_chars option exists in the config file without a plugin.");
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(conf__parse_bool(&token, "auth_plugin_deny_special_chars", &cur_plugin->config.deny_special_chars, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "plugin_load")){
					var_na *name = NULL;

					නම්(reload){
						continue;        /* plugin not currently valid for reloading. */

					}
					name = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(name, "plugin_load");

					cur_plugin = config__plugin_load(name, saveptr);
					නම්(cur_plugin == NULL){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "plugin_use")){
					var_na *name = NULL;

					නම්(reload){
						continue;        /* plugin not currently valid for reloading. */
					}
					REQUIRE_NON_DEFAULT_LISTENER(token);

					නම්(conf__parse_string(&token, "plugin_use", &name, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					cur_plugin = config__plugin_find(name);
					නම්(!cur_plugin){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Plugin '%s' not previously loaded.", name);
						mosquitto_FREE(name);
						ආපසු MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(name);
					නම්(config__plugin_add_secopt(cur_plugin, cur_listener->security_options)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "auto_id_prefix")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					OPTION_DEPRECATED(token, "Please use 'listener_auto_id_prefix' instead.");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					නම්(conf__parse_string(&token, "auto_id_prefix", &cur_security_options->auto_id_prefix, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(cur_security_options->auto_id_prefix){
						නම්(strlen(cur_security_options->auto_id_prefix) > 50){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: auto_id_prefix length must be <= 50.");
							ආපසු MOSQ_ERR_INVAL;
						}
						cur_security_options->auto_id_prefix_len = (uint16_t)strlen(cur_security_options->auto_id_prefix);
					}නැතහොත්{
						cur_security_options->auto_id_prefix_len = 0;
					}
				}නැතහොත් නම්(!strcmp(token, "autosave_interval")){
					නම්(conf__parse_int(&token, "autosave_interval", &config->autosave_interval, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(config->autosave_interval < 0){
						config->autosave_interval = 0;
					}
				}නැතහොත් නම්(!strcmp(token, "autosave_on_changes")){
					නම්(conf__parse_bool(&token, "autosave_on_changes", &config->autosave_on_changes, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "bind_address")){
					OPTION_DEPRECATED(token, "");
					config->local_only = false;
					නම්(reload){
						continue;        /* Rebinding listeners not valid during reloading. */

					}
					නම්(config__create_default_listener(config, token)){
						ආපසු MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					නම්(conf__parse_string(&token, "default listener bind_address", &config->default_listener->host, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(conf__attempt_resolve(config->default_listener->host, "bind_address", MOSQ_LOG_ERR, "Error")){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "bind_interface")){
#ifdef SO_BINDTODEVICE
					නම්(reload){
						continue;        /* Rebinding listeners not valid during reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					නම්(conf__parse_string(&token, "bind_interface", &cur_listener->bind_interface, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_ERR, "Error: bind_interface specified but socket option not available.");
					ආපසු MOSQ_ERR_INVAL;
#endif
				}නැතහොත් නම්(!strcmp(token, "bridge_attempt_unsubscribe")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_bool(&token, "bridge_attempt_unsubscribe", &cur_bridge->attempt_unsubscribe, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "bridge_cafile")){
#නම් defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					නම්(conf__parse_string(&token, "bridge_cafile", &cur_bridge->tls_cafile, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "bridge_alpn")){
#නම් defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_string(&token, "bridge_alpn", &cur_bridge->tls_alpn, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "bridge_ciphers")){
#නම් defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_string(&token, "bridge_ciphers", &cur_bridge->tls_ciphers, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "bridge_ciphers_tls1.3")){
#නම් defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_string(&token, "bridge_ciphers_tls1.3", &cur_bridge->tls_13_ciphers, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "bridge_bind_address")){
#නම් defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_string(&token, "bridge_bind_address", &cur_bridge->bind_address, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "bridge_capath")){
#නම් defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					නම්(conf__parse_string(&token, "bridge_capath", &cur_bridge->tls_capath, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "bridge_certfile")){
#නම් defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					නම්(conf__parse_string(&token, "bridge_certfile", &cur_bridge->tls_certfile, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "bridge_identity")){
#නම් defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					නම්(conf__parse_string(&token, "bridge_identity", &cur_bridge->tls_psk_identity, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "bridge_insecure")){
#නම් defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_bool(&token, "bridge_insecure", &cur_bridge->tls_insecure, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(cur_bridge->tls_insecure){
						log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge '%s' using insecure mode.", cur_bridge->name);
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "bridge_require_ocsp")){
#නම් defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_bool(&token, "bridge_require_ocsp", &cur_bridge->tls_ocsp_required, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "bridge_max_packet_size")){
#නම් defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_int(&token, "bridge_max_packet_size", &tmp_int, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(tmp_int < 0){
						tmp_int = 0;
					}
					cur_bridge->maximum_packet_size = (uint32_t)tmp_int;
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "bridge_max_topic_alias")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_int(&token, "bridge_max_topic_alias", &tmp_int, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}

					නම්(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: bridge_max_topic_alias must be > 0 and <= 65535.");
						ආපසු MOSQ_ERR_INVAL;
					}
					cur_bridge->max_topic_alias = (uint16_t)tmp_int;
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "bridge_outgoing_retain")){
#නම් defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_bool(&token, "bridge_outgoing_retain", &cur_bridge->outgoing_retain, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "bridge_keyfile")){
#නම් defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					mosquitto_FREE(cur_bridge->tls_keyfile);
					නම්(conf__parse_string(&token, "bridge_keyfile", &cur_bridge->tls_keyfile, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "bridge_protocol_version")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(NULL, "", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_protocol_version");

					නම්(!strcmp(token, "mqttv31")){
						cur_bridge->protocol_version = mosq_p_mqtt31;
					}නැතහොත් නම්(!strcmp(token, "mqttv311")){
						cur_bridge->protocol_version = mosq_p_mqtt311;
					}නැතහොත් නම්(!strcmp(token, "mqttv50")){
						cur_bridge->protocol_version = mosq_p_mqtt5;
					}නැතහොත්{
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'bridge_protocol_version' value (%s).", token);
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "bridge_psk")){
#නම් defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					නම්(conf__parse_string(&token, "bridge_psk", &cur_bridge->tls_psk, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "bridge_receive_maximum")){
#නම් defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_int(&token, "bridge_receive_maximum", &tmp_int, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(tmp_int <= 0){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be greater than 0.");
						ආපසු MOSQ_ERR_INVAL;
					}නැතහොත් නම්((uint64_t)tmp_int > (uint64_t)UINT16_MAX){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be lower than %u.", UINT16_MAX);
						ආපසු MOSQ_ERR_INVAL;
					}
					cur_bridge->receive_maximum = (uint16_t)tmp_int;
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "bridge_reload_type")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_reload_type");

					නම්(!strcmp(token, "lazy")){
						cur_bridge->reload_type = brt_lazy;
					}නැතහොත් නම්(!strcmp(token, "immediate")){
						cur_bridge->reload_type = brt_immediate;
					}නැතහොත්{
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'bridge_reload_type' value in configuration (%s).", token);
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "bridge_session_expiry_interval")){
#නම් defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_int(&token, "bridge_session_expiry_interval", &tmp_int, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(tmp_int < 0){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must not be negative.");
						ආපසු MOSQ_ERR_INVAL;
					}නැතහොත් නම්((uint64_t)tmp_int > (uint64_t)MQTT_SESSION_EXPIRY_NEVER){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must be lower than %u.", MQTT_SESSION_EXPIRY_NEVER);
						ආපසු MOSQ_ERR_INVAL;
					}
					cur_bridge->session_expiry_interval = (uint32_t)tmp_int;
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "bridge_tcp_keepalive")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);

					නම්(conf__parse_int(&token, "bridge_tcp_keepalive_idle", &tmp_int, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(tmp_int <= 0){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: invalid TCP keepalive idle value.");
						ආපසු MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_idle = (unsigned පූර්ණ_සංඛ්‍යා)tmp_int;

					නම්(conf__parse_int(&token, "bridge_tcp_keepalive_interval", &tmp_int, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(tmp_int <= 0){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: invalid TCP keepalive interval value.");
						ආපසු MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_interval = (unsigned පූර්ණ_සංඛ්‍යා)tmp_int;

					නම්(conf__parse_int(&token, "bridge_tcp_keepalive_counter", &tmp_int, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(tmp_int <= 0){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: invalid TCP keepalive counter value.");
						ආපසු MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_counter = (unsigned පූර්ණ_සංඛ්‍යා)tmp_int;
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "bridge_tcp_user_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
#ifdef WITH_TCP_USER_TIMEOUT
					නම්(conf__parse_int(&token, "bridge_tcp_user_timeout", &tmp_int, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(tmp_int < 0){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: invalid TCP user timeout value.");
						ආපසු MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_user_timeout = tmp_int;
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge TCP user timeout support not available.");
#endif
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "bridge_tls_use_os_certs")){
#නම් defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_bool(&token, "bridge_tls_use_os_certs", &cur_bridge->tls_use_os_certs, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "bridge_tls_version")){
#නම් defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_string(&token, "bridge_tls_version", &cur_bridge->tls_version, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "cafile")){
#නම් defined(WITH_TLS)
					REQUIRE_LISTENER(token);
					නම්(cur_listener->psk_hint){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						ආපසු MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->cafile);
					නම්(conf__parse_string(&token, "cafile", &cur_listener->cafile, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "capath")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->capath);
					නම්(conf__parse_string(&token, "capath", &cur_listener->capath, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "certfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					නම්(cur_listener->psk_hint){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						ආපසු MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->certfile);
					නම්(conf__parse_string(&token, "certfile", &cur_listener->certfile, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "check_retain_source")){
					නම්(conf__parse_bool(&token, "check_retain_source", &config->check_retain_source, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "ciphers")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers);
					නම්(conf__parse_string(&token, "ciphers", &cur_listener->ciphers, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "ciphers_tls1.3")){
#නම් defined(WITH_TLS) && (!defined(LIBRESSL_VERSION_NUMBER) || LIBRESSL_VERSION_NUMBER > 0x3040000FL)
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers_tls13);
					නම්(conf__parse_string(&token, "ciphers_tls1.3", &cur_listener->ciphers_tls13, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: ciphers_tls1.3 support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "clientid") || !strcmp(token, "remote_clientid")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_string(&token, "bridge remote clientid", &cur_bridge->remote_clientid, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "cleansession")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_bool(&token, "cleansession", &cur_bridge->clean_start, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "local_cleansession")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_bool(&token, "local_cleansession", (bool *)&cur_bridge->clean_start_local, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "clientid_prefixes")){
					OPTION_DEPRECATED(token, "");
					mosquitto_FREE(config->clientid_prefixes);
					නම්(conf__parse_string(&token, "clientid_prefixes", &config->clientid_prefixes, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "connection")){
#ifdef WITH_BRIDGE
					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "connection");

					/* Check for existing bridge name. */
					සඳහා(i=0; i<config->bridge_count; i++){
						නම්(!strcmp(config->bridges[i]->name, token)){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Duplicate bridge name '%s'.", token);
							ආපසු MOSQ_ERR_INVAL;
						}
					}

					sa_nracanaa mosquitto__bridge **bridges_new = mosquitto_realloc(config->bridges, (size_t)(config->bridge_count+1)*parimaa_na(sa_nracanaa mosquitto__bridge *));
					නම්(!bridges_new){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
						ආපසු MOSQ_ERR_NOMEM;
					}
					config->bridges = bridges_new;
					cur_bridge = mosquitto_malloc(parimaa_na(sa_nracanaa mosquitto__bridge));
					නම්(!cur_bridge){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
						ආපසු MOSQ_ERR_NOMEM;
					}

					config->bridge_count++;
					config->bridges[config->bridge_count-1] = cur_bridge;

					memset(cur_bridge, 0, parimaa_na(sa_nracanaa mosquitto__bridge));
					cur_bridge->name = mosquitto_strdup(token);
					නම්(!cur_bridge->name){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
						ආපසු MOSQ_ERR_NOMEM;
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
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "connection_messages")){
					නම්(conf__parse_bool(&token, token, &config->connection_messages, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "crlfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->crlfile);
					නම්(conf__parse_string(&token, "crlfile", &cur_listener->crlfile, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "dhparamfile")){
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: dhparamfile is no longer required.");
				}නැතහොත් නම්(!strcmp(token, "disable_client_cert_date_checks")){
#ifdef WITH_TLS
					REQUIRE_LISTENER(token);
					නම්(conf__parse_bool(&token, "disable_client_cert_date_checks", &cur_listener->disable_client_cert_date_checks, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "enable_control_api")){
#ifdef WITH_CONTROL
					නම්(conf__parse_bool(&token, "enable_control_api", &config->enable_control_api, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: $CONTROL support not available (enable_control_api).");
#endif
				}නැතහොත් නම්(!strcmp(token, "enable_proxy_protocol")){
#නම් defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(NULL, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					ආපසු MOSQ_ERR_INVAL;
#endif
					REQUIRE_LISTENER(token);
					නම්(conf__parse_int(&token, "enable_proxy_protocol", &cur_listener->enable_proxy_protocol, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(cur_listener->enable_proxy_protocol < 1 || cur_listener->enable_proxy_protocol > 2){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: enable_proxy_protocol must be 1 or 2.");
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "global_max_clients")){
					නම්(conf__parse_int(&token, "global_max_clients", &config->global_max_clients, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "global_max_connections")){
					නම්(conf__parse_int(&token, "global_max_connections", &config->global_max_connections, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "http_dir")){
#නම් defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
					නම්(reload){
						continue;        /* Not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					නම්(conf__parse_string(&token, "http_dir", &cur_listener->http_dir, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#ifdef WIN32
					var_na *http_dir_canonical = _fullpath(NULL, cur_listener->http_dir, 0);
					const var_na sep = '\\';
#නැතහොත්
					var_na *http_dir_canonical = realpath(cur_listener->http_dir, NULL);
					const var_na sep = '/';
#endif
					නම්(!http_dir_canonical){
						ආපසු MOSQ_ERR_NOMEM;
					}
					size_t http_dir_len = strlen(http_dir_canonical) + parimaa_na(sep) + 1;
					var_na *http_dir_canonical_sep = mosquitto_calloc(http_dir_len, parimaa_na(var_na));
					නම්(!http_dir_canonical_sep){
						free(http_dir_canonical);
						ආපසු MOSQ_ERR_NOMEM;
					}
					snprintf(http_dir_canonical_sep, http_dir_len, "%s%c", http_dir_canonical, sep);
					free(http_dir_canonical);
					mosquitto_FREE(cur_listener->http_dir);
					cur_listener->http_dir = http_dir_canonical_sep;
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: http_dir support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "idle_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_int(&token, "idle_timeout", &cur_bridge->idle_timeout, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(cur_bridge->idle_timeout < 1){
						log__printf(NULL, MOSQ_LOG_NOTICE, "idle_timeout interval too low, using 1 second.");
						cur_bridge->idle_timeout = 1;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "include_dir")){
					නම්(level == 0){
						var_na **files;
						පූර්ණ_සංඛ්‍යා file_count;

						/* Only process include_dir from the main config file. */
						token = strtok_r(NULL, "", &saveptr);
						REQUIRE_NON_EMPTY_OPTION(token, "include_dir");

						rc = config__get_dir_files(token, &files, &file_count);
						නම්(rc){
							ආපසු rc;
						}

						සඳහා(i=0; i<file_count; i++){
							log__printf(NULL, MOSQ_LOG_INFO, "Loading config file '%s'", files[i]);

							rc = config__read_file(config, reload, files[i], cr, level+1, &lineno_ext);
							නම්(rc){
								නම්(lineno_ext > 0){
									log__printf(NULL, MOSQ_LOG_ERR, "Error found at '%s:%d'.", files[i], lineno_ext);
								}
								/* Free happens below */
								bha_nga;
							}
						}
						සඳහා(i=0; i<file_count; i++){
							mosquitto_FREE(files[i]);
						}
						mosquitto_FREE(files);
						නම්(rc){
							ආපසු rc;    /* This returns if config__read_file() fails above */
						}
					}නැතහොත්{
						log__printf(NULL, MOSQ_LOG_ERR, "Error: The include_dir option is only valid in the main configuration file.");
						ආපසු 1;
					}
				}නැතහොත් නම්(!strcmp(token, "keepalive_interval")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_int(&token, "keepalive_interval", &tmp_int, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(tmp_int > UINT16_MAX){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Bridge keepalive value too high.");
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(tmp_int < 5){
						log__printf(NULL, MOSQ_LOG_NOTICE, "keepalive interval too low, using 5 seconds.");
						tmp_int = 5;
					}
					cur_bridge->keepalive = (uint16_t)tmp_int;
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "keyfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->keyfile);
					නම්(conf__parse_string(&token, "keyfile", &cur_listener->keyfile, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "listener")){
					config->local_only = false;

					නම්(conf__parse_int(&token, "listener port", &tmp_int, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#ifdef WITH_UNIX_SOCKETS
					නම්(tmp_int < 0 || tmp_int > UINT16_MAX){
#නැතහොත්
					නම්(tmp_int < 1 || tmp_int > UINT16_MAX){
#endif
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						ආපසු MOSQ_ERR_INVAL;
					}

					/* Look for bind address / unix socket path */
					token = strtok_r(NULL, " ", &saveptr);
					නම්(token != NULL && token[0] == '#'){
						token = NULL;
					}

					නම්(tmp_int == 0 && token == NULL){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: A listener with port 0 must provide a Unix socket path.");
						ආපසු MOSQ_ERR_INVAL;
					}

					නම්(reload){
						/* We reload listeners settings based on port number/unix socket path.
						 * If the port number/unix path doesn't already exist, exit with a complaint. */
						cur_listener = NULL;
#ifdef WITH_UNIX_SOCKETS
						නම්(tmp_int == 0){
							සඳහා(i=0; i<config->listener_count; i++){
								නම්(config->listeners[i].unix_socket_path != NULL
										&& strcmp(config->listeners[i].unix_socket_path, token) == 0){

									cur_listener = &config->listeners[i];
									bha_nga;
								}
							}
						}නැතහොත්
#endif
						{
							සඳහා(i=0; i<config->listener_count; i++){
								නම්(config->listeners[i].port == tmp_int){
									/* Now check we have a matching bind address, if defined */
									නම්(config->listeners[i].host){
										නම්(token && !strcmp(config->listeners[i].host, token)){
											/* They both have a bind address, and they match */
											cur_listener = &config->listeners[i];
											bha_nga;
										}
									}නැතහොත්{
										නම්(token == NULL){
											/* Neither this config nor the new config have a bind address,
											 * so they match. */
											cur_listener = &config->listeners[i];
											bha_nga;
										}
									}
								}
							}
						}
						නම්(!cur_listener){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: It is not currently possible to add/remove listeners when reloading the config file.");
							ආපසු MOSQ_ERR_INVAL;
						}
					}නැතහොත්{
						නම්(config__add_listener(config)){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
							ආපසු MOSQ_ERR_NOMEM;
						}
						cur_listener = &config->listeners[config->listener_count-1];
					}

					listener__set_defaults(cur_listener);
					cur_listener->port = (uint16_t)tmp_int;

					mosquitto_FREE(cur_listener->host);

#ifdef WITH_UNIX_SOCKETS
					mosquitto_FREE(cur_listener->unix_socket_path);
#endif

					නම්(token){
#ifdef WITH_UNIX_SOCKETS
						නම්(cur_listener->port == 0){
							cur_listener->unix_socket_path = mosquitto_strdup(token);
						}නැතහොත්
#endif
						{
							cur_listener->host = mosquitto_strdup(token);
						}
					}
				}නැතහොත් නම්(!strcmp(token, "listener_allow_anonymous")){
					REQUIRE_LISTENER(token);
					නම්(conf__parse_bool(&token, "listener_allow_anonymous", (bool *)&cur_listener->security_options->allow_anonymous, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "listener_auto_id_prefix")){
					REQUIRE_LISTENER(token);
					නම්(conf__parse_string(&token, "listener_auto_id_prefix", &cur_listener->security_options->auto_id_prefix, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(cur_listener->security_options->auto_id_prefix){
						නම්(strlen(cur_listener->security_options->auto_id_prefix) > 50){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: 'listener_auto_id_prefix' length must be <= 50.");
							ආපසු MOSQ_ERR_INVAL;
						}
						cur_listener->security_options->auto_id_prefix_len = (uint16_t)strlen(cur_listener->security_options->auto_id_prefix);
					}නැතහොත්{
						cur_listener->security_options->auto_id_prefix_len = 0;
					}
				}නැතහොත් නම්(!strcmp(token, "local_clientid")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_string(&token, "bridge local clientd", &cur_bridge->local_clientid, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "local_password")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_string(&token, "bridge local_password", &cur_bridge->local_password, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "local_username")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_string(&token, "bridge local_username", &cur_bridge->local_username, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "log_dest")){
					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_dest");

					cr->log_dest_set = 1;
					නම්(!strcmp(token, "none")){
						cr->log_dest = MQTT3_LOG_NONE;
					}නැතහොත් නම්(!strcmp(token, "syslog")){
						cr->log_dest |= MQTT3_LOG_SYSLOG;
					}නැතහොත් නම්(!strcmp(token, "stdout")){
						cr->log_dest |= MQTT3_LOG_STDOUT;
					}නැතහොත් නම්(!strcmp(token, "stderr")){
						cr->log_dest |= MQTT3_LOG_STDERR;
					}නැතහොත් නම්(!strcmp(token, "topic")){
						cr->log_dest |= MQTT3_LOG_TOPIC;
					}නැතහොත් නම්(!strcmp(token, "dlt")){
						cr->log_dest |= MQTT3_LOG_DLT;
#ifdef ANDROID
					}නැතහොත් නම්(!strcmp(token, "android")){
						cr->log_dest |= MQTT3_LOG_ANDROID;
#endif
					}නැතහොත් නම්(!strcmp(token, "file")){
						නම්(config->log_fptr || config->log_file){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Duplicate \"log_dest file\" value.");
							ආපසු MOSQ_ERR_INVAL;
						}
						/* Get remaining string. */
						token = saveptr;
						නම්(token && token[0]){
							අතරතුර(token[0] == ' ' || token[0] == '\t'){
								token++;
							}
						}
						/* Duplicate "token" check here saves a log__printf() */
						නම්(token && token[0]){
							config->log_file = mosquitto_strdup(token);
							නම්(!config->log_file){
								log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
								ආපසු MOSQ_ERR_NOMEM;
							}
							cr->log_dest |= MQTT3_LOG_FILE;
						}නැතහොත්{
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty \"log_dest file\" value in configuration.");
							ආපසු MOSQ_ERR_INVAL;
						}
						cr->log_dest |= MQTT3_LOG_FILE;
					}නැතහොත්{
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'log_dest' value (%s).", token);
						ආපසු MOSQ_ERR_INVAL;
					}
#නම් defined(WIN32) || defined(__CYGWIN__)
					නම්(service_handle){
						නම්(cr->log_dest == MQTT3_LOG_STDOUT || cr->log_dest == MQTT3_LOG_STDERR){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Cannot log to stdout/stderr when running as a Windows service.");
							ආපසු MOSQ_ERR_INVAL;
						}
					}
#endif
				}නැතහොත් නම්(!strcmp(token, "log_facility")){
#නම් defined(WIN32) || defined(__CYGWIN__)
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: log_facility not supported on Windows.");
#නැතහොත්
					නම්(conf__parse_int(&token, "log_facility", &tmp_int, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					switch(tmp_int){
						pak_sa 0:
							config->log_facility = LOG_LOCAL0;
							bha_nga;
						pak_sa 1:
							config->log_facility = LOG_LOCAL1;
							bha_nga;
						pak_sa 2:
							config->log_facility = LOG_LOCAL2;
							bha_nga;
						pak_sa 3:
							config->log_facility = LOG_LOCAL3;
							bha_nga;
						pak_sa 4:
							config->log_facility = LOG_LOCAL4;
							bha_nga;
						pak_sa 5:
							config->log_facility = LOG_LOCAL5;
							bha_nga;
						pak_sa 6:
							config->log_facility = LOG_LOCAL6;
							bha_nga;
						pak_sa 7:
							config->log_facility = LOG_LOCAL7;
							bha_nga;
						default:
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'log_facility' value (%d).", tmp_int);
							ආපසු MOSQ_ERR_INVAL;
					}
#endif
				}නැතහොත් නම්(!strcmp(token, "log_timestamp")){
					නම්(conf__parse_bool(&token, token, &config->log_timestamp, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "log_timestamp_format")){
					නම්(conf__parse_string(&token, token, &config->log_timestamp_format, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "log_type")){
					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_type");

					cr->log_type_set = 1;
					නම්(!strcmp(token, "none")){
						cr->log_type = MOSQ_LOG_NONE;
					}නැතහොත් නම්(!strcmp(token, "information")){
						cr->log_type |= MOSQ_LOG_INFO;
					}නැතහොත් නම්(!strcmp(token, "notice")){
						cr->log_type |= MOSQ_LOG_NOTICE;
					}නැතහොත් නම්(!strcmp(token, "warning")){
						cr->log_type |= MOSQ_LOG_WARNING;
					}නැතහොත් නම්(!strcmp(token, "error")){
						cr->log_type |= MOSQ_LOG_ERR;
					}නැතහොත් නම්(!strcmp(token, "debug")){
						cr->log_type |= MOSQ_LOG_DEBUG;
					}නැතහොත් නම්(!strcmp(token, "subscribe")){
						cr->log_type |= MOSQ_LOG_SUBSCRIBE;
					}නැතහොත් නම්(!strcmp(token, "unsubscribe")){
						cr->log_type |= MOSQ_LOG_UNSUBSCRIBE;
					}නැතහොත් නම්(!strcmp(token, "internal")){
						cr->log_type |= MOSQ_LOG_INTERNAL;
#ifdef WITH_WEBSOCKETS
					}නැතහොත් නම්(!strcmp(token, "websockets")){
						cr->log_type |= MOSQ_LOG_WEBSOCKETS;
#endif
					}නැතහොත් නම්(!strcmp(token, "all")){
						cr->log_type = MOSQ_LOG_ALL;
					}නැතහොත්{
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'log_type' value (%s).", token);
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "max_connections")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					නම්(conf__parse_int(&token, token, &cur_listener->max_connections, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(cur_listener->max_connections < 0){
						cur_listener->max_connections = -1;
					}
				}නැතහොත් නම්(!strcmp(token, "maximum_qos") || !strcmp(token, "max_qos")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					නම්(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(tmp_int < 0 || tmp_int > 2){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: 'max_qos' must be between 0 and 2 inclusive.");
						ආපසු MOSQ_ERR_INVAL;
					}
					cur_listener->max_qos = (uint8_t)tmp_int;
				}නැතහොත් නම්(!strcmp(token, "max_inflight_bytes")){
					නම්(conf__parse_int(&token, "max_inflight_bytes", &tmp_int, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_inflight_bytes = (size_t)tmp_int;
				}නැතහොත් නම්(!strcmp(token, "max_inflight_messages")){
					නම්(conf__parse_int(&token, "max_inflight_messages", &tmp_int, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(tmp_int < 0 || tmp_int == UINT16_MAX){
						tmp_int = 0;
					}නැතහොත් නම්(tmp_int > UINT16_MAX){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: 'max_inflight_messages' must be <= 65535.");
						ආපසු MOSQ_ERR_INVAL;
					}
					config->max_inflight_messages = (uint16_t)tmp_int;
				}නැතහොත් නම්(!strcmp(token, "max_keepalive")){
					නම්(conf__parse_int(&token, "max_keepalive", &tmp_int, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'max_keepalive' value (%d).", tmp_int);
						ආපසු MOSQ_ERR_INVAL;
					}
					config->max_keepalive = (uint16_t)tmp_int;
				}නැතහොත් නම්(!strcmp(token, "max_packet_size")){
					නම්(conf__parse_int(&token, "max_packet_size", &tmp_int, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(tmp_int < 20){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: 'max_packet_size' must be >= 20.");
						ආපසු MOSQ_ERR_INVAL;
					}
					config->max_packet_size = (uint32_t)tmp_int;
				}නැතහොත් නම්(!strcmp(token, "max_queued_bytes")){
					නම්(conf__parse_int(&token, "max_queued_bytes", &tmp_int, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_bytes = (size_t)tmp_int;
				}නැතහොත් නම්(!strcmp(token, "max_queued_messages")){
					නම්(conf__parse_int(&token, "max_queued_messages", &tmp_int, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_messages = tmp_int;
				}නැතහොත් නම්(!strcmp(token, "memory_limit")){
					ssize_t lim;
					නම්(conf__parse_ssize_t(&token, "memory_limit", &lim, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(lim < 0){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'memory_limit' value (%ld).", lim);
						ආපසු MOSQ_ERR_INVAL;
					}
					mosquitto_memory_set_limit((size_t)lim);
				}නැතහොත් නම්(!strcmp(token, "message_size_limit")){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Note: It is recommended to replace `message_size_limit` with `max_packet_size`.");
					නම්(conf__parse_int(&token, "message_size_limit", (පූර්ණ_සංඛ්‍යා *)&config->message_size_limit, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(config->message_size_limit > MQTT_MAX_PAYLOAD){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'message_size_limit' value (%u).", config->message_size_limit);
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "mount_point")){
					REQUIRE_LISTENER(token);
					mosquitto_FREE(cur_listener->mount_point);
					නම්(conf__parse_string(&token, "mount_point", &cur_listener->mount_point, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(mosquitto_pub_topic_check(cur_listener->mount_point) != MOSQ_ERR_SUCCESS){
						log__printf(NULL, MOSQ_LOG_ERR,
								"Error: Invalid 'mount_point' value (%s). Does it contain a wildcard character?",
								cur_listener->mount_point);
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "notifications")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_bool(&token, "notifications", &cur_bridge->notifications, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "notifications_local_only")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_bool(&token, "notifications_local_only", &cur_bridge->notifications_local_only, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "notification_topic")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_string(&token, "notification_topic", &cur_bridge->notification_topic, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "password") || !strcmp(token, "remote_password")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_string(&token, "bridge remote_password", &cur_bridge->remote_password, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "password_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->password_data.password_file);
					නම්(conf__parse_string(&token, "password_file", &cur_security_options->password_data.password_file, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "per_listener_settings")){
					OPTION_DEPRECATED(token, "Please see the documentation for how to achieve the same effect.");
					නම්(config->per_listener_settings){
						/* Once this is set, don't let it be unset. It should be the first config option ideally. */
						continue;
					}
					නම්(conf__parse_bool(&token, "per_listener_settings", &config->per_listener_settings, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(cur_security_options && config->per_listener_settings){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: per_listener_settings must be set before any other security settings.");
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "persistence") || !strcmp(token, "retained_persistence")){
					නම්(conf__parse_bool(&token, token, &config->persistence, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "persistence_file")){
					නම්(conf__parse_string(&token, "persistence_file", &config->persistence_file, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "persistence_location")){
					නම්(conf__parse_string(&token, "persistence_location", &config->persistence_location, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "persistent_client_expiration")){
					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "persistent_client_expiration");

					time_t expiration_mult;

					switch(token[strlen(token)-1]){
						pak_sa 's':
							expiration_mult = 1;
							bha_nga;
						pak_sa 'h':
							expiration_mult = 3600;
							bha_nga;
						pak_sa 'd':
							expiration_mult = 86400;
							bha_nga;
						pak_sa 'w':
							expiration_mult = 86400*7;
							bha_nga;
						pak_sa 'm':
							expiration_mult = 86400*30;
							bha_nga;
						pak_sa 'y':
							expiration_mult = 86400*365;
							bha_nga;
						default:
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
							ආපසු MOSQ_ERR_INVAL;
					}
					token[strlen(token)-1] = '\0';
					config->persistent_client_expiration = atoi(token)*expiration_mult;
					නම්(config->persistent_client_expiration <= 0){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "pid_file")){
					නම්(reload){
						continue;        /* pid file not valid for reloading. */
					}
					නම්(conf__parse_string(&token, "pid_file", &config->pid_file, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "port")){
					OPTION_DEPRECATED(token, "Please use 'listener' instead.");
					config->local_only = false;
					නම්(reload){
						continue;        /* Listeners not valid for reloading. */

					}
					නම්(config__create_default_listener(config, token)){
						ආපසු MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					නම්(config->default_listener->port){
						log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Default listener port specified multiple times. Only the latest will be used.");
					}
					නම්(conf__parse_int(&token, "port", &tmp_int, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(tmp_int < 1 || tmp_int > UINT16_MAX){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						ආපසු MOSQ_ERR_INVAL;
					}
					config->default_listener->port = (uint16_t)tmp_int;
				}නැතහොත් නම්(!strcmp(token, "protocol")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "protocol");

					නම්(!strcmp(token, "mqtt")){
						cur_listener->protocol = mp_mqtt;
						/*
						}else if(!strcmp(token, "mqttsn")){
						    cur_listener->protocol = mp_mqttsn;
						*/
					}නැතහොත් නම්(!strcmp(token, "websockets")){
#ifdef WITH_WEBSOCKETS
						cur_listener->protocol = mp_websockets;
#නැතහොත්
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Websockets support not available.");
						ආපසු MOSQ_ERR_INVAL;
#endif
					}නැතහොත් නම්(!strcmp(token, "http_api")){
#ifdef WITH_HTTP_API
						cur_listener->protocol = mp_http_api;
#නැතහොත්
						log__printf(NULL, MOSQ_LOG_ERR, "Error: HTTP API support not available.");
						ආපසු MOSQ_ERR_INVAL;
#endif
					}නැතහොත්{
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'protocol' value (%s).", token);
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "proxy_protocol_v2_require_tls")){
#නම් defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(NULL, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					ආපසු MOSQ_ERR_INVAL;
#endif
					REQUIRE_LISTENER(token);
					නම්(conf__parse_bool(&token, "proxy_protocol_v2_require_tls", &cur_listener->proxy_protocol_v2_require_tls, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "psk_file")){
#ifdef FINAL_WITH_TLS_PSK
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					නම්(cur_listener && cur_listener->certfile){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						ආපසු MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_security_options->psk_file);
					නම්(conf__parse_string(&token, "psk_file", &cur_security_options->psk_file, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "psk_hint")){
#ifdef FINAL_WITH_TLS_PSK
					නම්(reload){
						continue;        /* PSK file not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					නම්(conf__parse_string(&token, "psk_hint", &cur_listener->psk_hint, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "queue_qos0_messages")){
					නම්(conf__parse_bool(&token, token, &config->queue_qos0_messages, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "require_certificate")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					නම්(conf__parse_bool(&token, "require_certificate", &cur_listener->require_certificate, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "restart_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					cur_bridge->backoff_cap = 0; /* set backoff to constant mode, unless cap is specified further down */
					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "restart_timeout");

					cur_bridge->restart_timeout = atoi(token);
					cur_bridge->backoff_base = 0;
					cur_bridge->backoff_cap = 0;
					නම්(cur_bridge->restart_timeout < 1){
						log__printf(NULL, MOSQ_LOG_NOTICE, "restart_timeout interval too low, using 1 second.");
						cur_bridge->restart_timeout = 1;
					}නැතහොත් නම්(cur_bridge->restart_timeout > 3600){
						log__printf(NULL, MOSQ_LOG_NOTICE, "restart_timeout interval too high, using 3600 seconds.");
						cur_bridge->restart_timeout = 3600;
					}
					token = strtok_r(NULL, " ", &saveptr);
					නම්(token){
						cur_bridge->backoff_base = cur_bridge->restart_timeout;
						cur_bridge->backoff_cap = atoi(token);
						නම්(cur_bridge->backoff_cap < cur_bridge->backoff_base){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: backoff cap is lower than the base in restart_timeout.");
							ආපසු MOSQ_ERR_INVAL;
						}නැතහොත් නම්(cur_bridge->backoff_cap > 7200){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: backoff cap too high, using 7200 seconds.");
							cur_bridge->backoff_cap = 7200;
						}

						token = strtok_r(NULL, " ", &saveptr);
						නම්(token){
							cur_bridge->stable_connection_period = atoi(token);
							නම්(cur_bridge->stable_connection_period < 0){
								log__printf(NULL, MOSQ_LOG_ERR, "Error: stable connection period cannot be negative.");
								ආපසු MOSQ_ERR_INVAL;
							}
						}
					}
					cur_bridge->restart_timeout *= 1000; /* backoff is tracked in ms */
					cur_bridge->backoff_base *= 1000;
					cur_bridge->backoff_cap *= 1000;
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "retain_available")){
					නම්(conf__parse_bool(&token, token, &config->retain_available, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "retain_expiry_interval")){
					නම්(conf__parse_int(&token, token, &config->retain_expiry_interval, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(config->retain_expiry_interval < 1){
						log__printf(NULL, MOSQ_LOG_WARNING, "Error: retain_expiry_interval must be >= 1.");
						ආපසු MOSQ_ERR_INVAL;
					}නැතහොත් නම්(config->retain_expiry_interval > 10000000){
						log__printf(NULL, MOSQ_LOG_WARNING, "Warning: retain_expiry_interval being capped at 19 years.");
						config->retain_expiry_interval = 10000000;
					}
					config->retain_expiry_interval *= 60;
				}නැතහොත් නම්(!strcmp(token, "retry_interval")){
					OPTION_UNAVAILABLE(token);
				}නැතහොත් නම්(!strcmp(token, "round_robin")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_bool(&token, "round_robin", &cur_bridge->round_robin, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "set_tcp_nodelay")){
					නම්(conf__parse_bool(&token, "set_tcp_nodelay", &config->set_tcp_nodelay, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "start_type")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					නම්(!strcmp(token, "automatic")){
						cur_bridge->start_type = bst_automatic;
					}නැතහොත් නම්(!strcmp(token, "lazy")){
						cur_bridge->start_type = bst_lazy;
					}නැතහොත් නම්(!strcmp(token, "manual")){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Manual start_type not supported.");
						ආපසු MOSQ_ERR_INVAL;
					}නැතහොත් නම්(!strcmp(token, "once")){
						cur_bridge->start_type = bst_once;
					}නැතහොත්{
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'start_type' value in configuration (%s).", token);
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "socket_domain")){
					නම්(reload){
						continue;        /* socket_domain not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					නම්(!strcmp(token, "ipv4")){
						cur_listener->socket_domain = AF_INET;
					}නැතහොත් නම්(!strcmp(token, "ipv6")){
						cur_listener->socket_domain = AF_INET6;
					}නැතහොත්{
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'socket_domain' value '%s' in configuration.", token);
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "sys_interval")){
					නම්(conf__parse_int(&token, "sys_interval", &config->sys_interval, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(config->sys_interval < 0 || config->sys_interval > 65535){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'sys_interval' value (%d).", config->sys_interval);
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "threshold")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_int(&token, "threshold", &cur_bridge->threshold, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(cur_bridge->threshold < 1){
						log__printf(NULL, MOSQ_LOG_NOTICE, "threshold too low, using 1 message.");
						cur_bridge->threshold = 1;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "tls_engine")){
#ifdef WITH_TLS
					නම්(reload){
						continue;        /* tls_engine not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					නම්(conf__parse_string(&token, "tls_engine", &cur_listener->tls_engine, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "tls_engine_kpass_sha1")){
#ifdef WITH_TLS
					නම්(reload){
						continue;        /* tls_engine not valid for reloading. */

					}
					var_na *kpass_sha = NULL, *kpass_sha_bin = NULL;
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					නම්(conf__parse_string(&token, "tls_engine_kpass_sha1", &kpass_sha, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(mosquitto__hex2bin_sha1(kpass_sha, (unsigned var_na **)&kpass_sha_bin) != MOSQ_ERR_SUCCESS){
						mosquitto_FREE(kpass_sha);
						ආපසු MOSQ_ERR_INVAL;
					}
					mosquitto_free(cur_listener->tls_engine_kpass_sha1);
					cur_listener->tls_engine_kpass_sha1 = kpass_sha_bin;
					mosquitto_FREE(kpass_sha);
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "tls_keyform")){
#ifdef WITH_TLS
					නම්(reload){
						continue;        /* tls_engine not valid for reloading. */

					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					var_na *keyform = NULL;
					නම්(conf__parse_string(&token, "tls_keyform", &keyform, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					cur_listener->tls_keyform = mosq_k_pem;
					නම්(!strcmp(keyform, "engine")){
						cur_listener->tls_keyform = mosq_k_engine;
					}
					mosquitto_FREE(keyform);
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "tls_version")){
#නම් defined(WITH_TLS)
					නම්(reload){
						continue;        /* tls_version not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					නම්(conf__parse_string(&token, "tls_version", &cur_listener->tls_version, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "topic")){
#ifdef WITH_BRIDGE
					var_na *topic = NULL;
					ga_nanaa mosquitto__bridge_direction direction = bd_out;
					uint8_t qos = 0;
					var_na *local_prefix = NULL, *remote_prefix = NULL;

					REQUIRE_BRIDGE(token);

					token = strtok_r(NULL, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "topic");

					// Check if the topic is quoted (e.g. for spaces within topic names), but not the
					// special case of ""
					නම්(token[0] == '"' && token [1] != '"'){
						නම්(strchr(saveptr, '"') == NULL){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Missing closing quote in topic value (%s).", saveptr);
							ආපසු MOSQ_ERR_INVAL;
						}

						var_na *topic_in_quotes = strtok_r(NULL, "\"", &saveptr);
						size_t tlen = 1;
						නම්(topic_in_quotes){
							tlen = strlen(topic_in_quotes);
						}

						topic = mosquitto_malloc(strlen(token) + tlen + 1);
						නම්(!topic){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
							ආපසු MOSQ_ERR_NOMEM;
						}

						strcpy(topic, token + 1);
						strcat(topic, " ");
						නම්(topic_in_quotes){
							strcat(topic, topic_in_quotes);
						}
					}නැතහොත්{
						topic = mosquitto_strdup(token);
						නම්(!topic){
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
							ආපසු MOSQ_ERR_NOMEM;
						}
					}

					token = strtok_r(NULL, " ", &saveptr);
					නම්(token){
						නම්(!strcasecmp(token, "out")){
							direction = bd_out;
						}නැතහොත් නම්(!strcasecmp(token, "in")){
							direction = bd_in;
						}නැතහොත් නම්(!strcasecmp(token, "both")){
							direction = bd_both;
						}නැතහොත්{
							log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge topic direction '%s'.", token);
							mosquitto_FREE(topic);
							ආපසු MOSQ_ERR_INVAL;
						}
						token = strtok_r(NULL, " ", &saveptr);
						නම්(token){
							නම්(token[0] == '#'){
								(void)strtok_r(NULL, "", &saveptr);
							}
							qos = (uint8_t)atoi(token);
							නම්(qos > 2){
								log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid bridge QoS level '%s'.", token);
								mosquitto_FREE(topic);
								ආපසු MOSQ_ERR_INVAL;
							}

							token = strtok_r(NULL, " ", &saveptr);
							නම්(token){
								නම්(!strcmp(token, "\"\"") || token[0] == '#'){
									local_prefix = NULL;
									නම්(token[0] == '#'){
										(void)strtok_r(NULL, "", &saveptr);
									}
								}නැතහොත්{
									local_prefix = token;
								}

								token = strtok_r(NULL, " ", &saveptr);
								නම්(token){
									නම්(!strcmp(token, "\"\"") || token[0] == '#'){
										remote_prefix = NULL;
									}නැතහොත්{
										remote_prefix = token;
									}
								}
							}
						}
					}

					නම්(bridge__add_topic(cur_bridge, topic, direction, qos, local_prefix, remote_prefix)){
						mosquitto_FREE(topic);
						ආපසු MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(topic);
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "max_topic_alias")){
					REQUIRE_LISTENER(token);
					නම්(conf__parse_int(&token, "max_topic_alias", &tmp_int, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias' value in configuration.");
						ආපසු MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias = (uint16_t)tmp_int;
				}නැතහොත් නම්(!strcmp(token, "max_topic_alias_broker")){
					REQUIRE_LISTENER(token);
					නම්(conf__parse_int(&token, "max_topic_alias_broker", &tmp_int, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias_broker' value in configuration.");
						ආපසු MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias_broker = (uint16_t)tmp_int;
				}නැතහොත් නම්(!strcmp(token, "try_private")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_bool(&token, "try_private", &cur_bridge->try_private, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "upgrade_outgoing_qos")){
					නම්(conf__parse_bool(&token, token, &config->upgrade_outgoing_qos, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "use_identity_as_username")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					නම්(conf__parse_bool(&token, "use_identity_as_username", &cur_listener->use_identity_as_username, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "use_subject_as_username")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					නම්(conf__parse_bool(&token, "use_subject_as_username", &cur_listener->use_subject_as_username, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "user")){
					නම්(reload){
						continue;        /* Drop privileges user not valid for reloading. */
					}
					mosquitto_FREE(config->user);
					නම්(conf__parse_string(&token, "user", &config->user, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "use_username_as_clientid")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					නම්(conf__parse_bool(&token, "use_username_as_clientid", &cur_listener->use_username_as_clientid, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
				}නැතහොත් නම්(!strcmp(token, "username") || !strcmp(token, "remote_username")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					නම්(conf__parse_string(&token, "bridge remote_username", &cur_bridge->remote_username, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}නැතහොත් නම්(!strcmp(token, "websockets_log_level")){
#නම් defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					නම්(conf__parse_int(&token, "websockets_log_level", &config->websockets_log_level, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
#endif
				}නැතහොත් නම්(!strcmp(token, "websockets_headers_size") || !strcmp(token, "packet_buffer_size")){
					නම්(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					නම්(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(NULL, MOSQ_LOG_WARNING, "Error: Packet buffer size must be between 0 and 65535 inclusive.");
						ආපසු MOSQ_ERR_INVAL;
					}
					config->packet_buffer_size = (uint16_t)tmp_int;
				}නැතහොත් නම්(!strcmp(token, "websockets_origin")){
#ifdef WITH_WEBSOCKETS
#  නම් LWS_LIBRARY_VERSION_NUMBER >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN
					REQUIRE_LISTENER(token);
					ws_origins = mosquitto_realloc(cur_listener->ws_origins, parimaa_na(var_na *)*(size_t)(cur_listener->ws_origin_count+1));
					නම්(ws_origins == NULL){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
						ආපසු MOSQ_ERR_NOMEM;
					}
					ws_origins[cur_listener->ws_origin_count] = NULL;
					cur_listener->ws_origins = ws_origins;
					නම්(conf__parse_string(&token, "websockets_origin", &ws_origins[cur_listener->ws_origin_count], &saveptr)){
						ආපසු MOSQ_ERR_INVAL;
					}
					cur_listener->ws_origin_count++;
#  නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: websockets_origin support not available, libwebsockets version is too old.");
#  endif
#නැතහොත්
					log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Websockets support not available.");
#endif
				}නැතහොත්{
					log__printf(NULL, MOSQ_LOG_ERR, "Error: Unknown configuration variable '%s'.", token);
					ආපසු MOSQ_ERR_INVAL;
				}
			}
		}
	}
	ආපසු MOSQ_ERR_SUCCESS;
}


පූර්ණ_සංඛ්‍යා config__read_file(sa_nracanaa mosquitto__config *config, bool reload, const var_na *file, sa_nracanaa config_recurse *cr, පූර්ණ_සංඛ්‍යා level, පූර්ණ_සංඛ්‍යා *lineno)
{
	පූර්ණ_සංඛ්‍යා rc;
	FILE *fptr = NULL;
	var_na *buf;
	පූර්ණ_සංඛ්‍යා buflen;
#ifndef WIN32
	DIR *dir;
#endif

#ifndef WIN32
	dir = opendir(file);
	නම්(dir){
		closedir(dir);
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Config file '%s' is a directory.", file);
		ආපසු 1;
	}
#endif

	fptr = mosquitto_fopen(file, "rt", false);
	නම්(!fptr){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to open config file '%s'.", file);
		ආපසු 1;
	}

	buflen = 1000;
	buf = mosquitto_malloc((size_t)buflen);
	නම්(!buf){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		fclose(fptr);
		ආපසු MOSQ_ERR_NOMEM;
	}

	rc = config__read_file_core(config, reload, cr, level, lineno, fptr, &buf, &buflen);
	mosquitto_FREE(buf);
	fclose(fptr);

	ආපසු rc;
}


static පූර්ණ_සංඛ්‍යා config__check_proxy(sa_nracanaa mosquitto__config *config)
{
	සඳහා(පූර්ණ_සංඛ්‍යා i=0; i<config->listener_count; i++){
		sa_nracanaa mosquitto__listener *l = &config->listeners[i];

		නම්(l->enable_proxy_protocol == 2){
#ifdef WITH_TLS
			නම්(l->use_subject_as_username){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: use_subject_as_username cannot be used with `enable_proxy_protocol 2`.");
				ආපසු MOSQ_ERR_INVAL;
			}

			නම්(l->certfile || l->keyfile){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: certfile and keyfile cannot be used with `enable_proxy_protocol 2`.");
				ආපසු MOSQ_ERR_INVAL;
			}
#endif
		}
	}

	ආපසු MOSQ_ERR_SUCCESS;
}


static පූර්ණ_සංඛ්‍යා config__check(sa_nracanaa mosquitto__config *config)
{
	/* Checks that are easy to make after the config has been loaded. */

	const var_na *id_prefix;
	පූර්ණ_සංඛ්‍යා id_prefix_len;
	නම්(config->security_options.auto_id_prefix){
		id_prefix = config->security_options.auto_id_prefix;
		id_prefix_len = config->security_options.auto_id_prefix_len;
	}නැතහොත්{
		id_prefix = "auto-";
		id_prefix_len = (පූර්ණ_සංඛ්‍යා)strlen("auto-");
	}

	/* Default to auto_id_prefix = 'auto-' if none set. */
	සඳහා(පූර්ණ_සංඛ්‍යා i=0; i<config->listener_count; i++){
		නම්(!config->listeners[i].security_options->auto_id_prefix){
			config->listeners[i].security_options->auto_id_prefix = mosquitto_strdup(id_prefix);
			නම්(!config->listeners[i].security_options->auto_id_prefix){
				ආපසු MOSQ_ERR_NOMEM;
			}
			config->listeners[i].security_options->auto_id_prefix_len = (uint16_t)id_prefix_len;
		}
	}

	ආපසු config__check_proxy(config);
}

#ifdef WITH_BRIDGE


static පූර්ණ_සංඛ්‍යා config__check_bridges(sa_nracanaa mosquitto__config *config)
{
	sa_nracanaa mosquitto__bridge *bridge1, *bridge2;
	var_na hostname[256];
	size_t len;

	/* Check for bridge duplicate local_clientid, need to generate missing IDs
	 * first. */
	සඳහා(පූර්ණ_සංඛ්‍යා i=0; i<config->bridge_count; i++){
		bridge1 = config->bridges[i];

		නම්(!bridge1->remote_clientid){
			නම්(!gethostname(hostname, 256)){
				len = strlen(hostname) + strlen(bridge1->name) + 2;
				bridge1->remote_clientid = mosquitto_malloc(len);
				නම්(!bridge1->remote_clientid){
					ආපසු MOSQ_ERR_NOMEM;
				}
				snprintf(bridge1->remote_clientid, len, "%s.%s", hostname, bridge1->name);
			}නැතහොත්{
				ආපසු 1;
			}
		}

		නම්(!bridge1->local_clientid){
			len = strlen(bridge1->remote_clientid) + strlen("local.") + 2;
			bridge1->local_clientid = mosquitto_malloc(len);
			නම්(!bridge1->local_clientid){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
				ආපසු MOSQ_ERR_NOMEM;
			}
			snprintf(bridge1->local_clientid, len, "local.%s", bridge1->remote_clientid);
		}
	}

	සඳහා(පූර්ණ_සංඛ්‍යා i=0; i<config->bridge_count; i++){
		bridge1 = config->bridges[i];
		සඳහා(පූර්ණ_සංඛ්‍යා j=i+1; j<config->bridge_count; j++){
			bridge2 = config->bridges[j];
			නම්(!strcmp(bridge1->local_clientid, bridge2->local_clientid)){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Bridge local_clientid "
						"'%s' is not unique. Try changing or setting the "
						"local_clientid value for one of the bridges.",
						bridge1->local_clientid);
				ආපසු MOSQ_ERR_INVAL;
			}
		}
	}

#ifdef WITH_TLS
	/* Check for missing TLS cafile/capath/certfile/keyfile */
	සඳහා(පූර්ණ_සංඛ්‍යා i=0; i<config->listener_count; i++){
		bool cafile = !!config->listeners[i].cafile;
		bool capath = !!config->listeners[i].capath;
		bool certfile = !!config->listeners[i].certfile;
		bool keyfile = !!config->listeners[i].keyfile;

		නම්((certfile && !keyfile) || (!certfile && keyfile)){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Both certfile and keyfile must be provided to enable a TLS listener.");
			ආපසු MOSQ_ERR_INVAL;
		}
		නම්(cafile && !certfile){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: cafile specified without certfile and keyfile.");
			ආපසු MOSQ_ERR_INVAL;
		}
		නම්(capath && !certfile){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: capath specified without certfile and keyfile.");
			ආපසු MOSQ_ERR_INVAL;
		}
	}
#endif
	ආපසු MOSQ_ERR_SUCCESS;
}
#endif


static පූර්ණ_සංඛ්‍යා conf__parse_bool(var_na **token, const var_na *name, bool *value, var_na **saveptr)
{
	*token = strtok_r(NULL, " ", saveptr);
	නම්(*token){
		නම්(!strcmp(*token, "false") || !strcmp(*token, "0")){
			*value = false;
		}නැතහොත් නම්(!strcmp(*token, "true") || !strcmp(*token, "1")){
			*value = true;
		}නැතහොත්{
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid '%s' value (%s).", name, *token);
			ආපසු MOSQ_ERR_INVAL;
		}
	}නැතහොත්{
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		ආපසු MOSQ_ERR_INVAL;
	}

	ආපසු MOSQ_ERR_SUCCESS;
}


static පූර්ණ_සංඛ්‍යා conf__parse_int(var_na **token, const var_na *name, පූර්ණ_සංඛ්‍යා *value, var_na **saveptr)
{
	*token = strtok_r(NULL, " ", saveptr);
	නම්(*token){
		var_na *endptr = NULL;
		long l = strtol(*token, &endptr, 0);
		නම්(endptr == *token || endptr[0] != '\0'){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: '%s' value not a number.", name);
			ආපසු MOSQ_ERR_INVAL;
		}
		නම්(l > INT_MAX || l < INT_MIN){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: '%s' value out of range.", name);
			ආපසු MOSQ_ERR_INVAL;
		}
		*value = (පූර්ණ_සංඛ්‍යා)l;
	}නැතහොත්{
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		ආපසු MOSQ_ERR_INVAL;
	}

	ආපසු MOSQ_ERR_SUCCESS;
}


static පූර්ණ_සංඛ්‍යා conf__parse_ssize_t(var_na **token, const var_na *name, ssize_t *value, var_na **saveptr)
{
	*token = strtok_r(NULL, " ", saveptr);
	නම්(*token){
		*value = atol(*token);
	}නැතහොත්{
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		ආපසු MOSQ_ERR_INVAL;
	}

	ආපසු MOSQ_ERR_SUCCESS;
}


static පූර්ණ_සංඛ්‍යා conf__parse_string(var_na **token, const var_na *name, var_na **value, var_na **saveptr)
{
	size_t tlen;

	*token = strtok_r(NULL, "", saveptr);
	නම්(*token){
		නම්(*value){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Duplicate '%s' value in configuration.", name);
			ආපසු MOSQ_ERR_INVAL;
		}
		/* Deal with multiple spaces at the beginning of the string. */
		*token = mosquitto_trimblanks(*token);
		නම්(strlen(*token) == 0){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
			ආපසු MOSQ_ERR_INVAL;
		}

		tlen = strlen(*token);
		නම්(tlen > UINT16_MAX){
			ආපසු MOSQ_ERR_INVAL;
		}
		නම්(mosquitto_validate_utf8(*token, (uint16_t)tlen)){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Malformed UTF-8 in configuration.");
			ආපසු MOSQ_ERR_INVAL;
		}
		*value = mosquitto_strdup(*token);
		නම්(!*value){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
			ආපසු MOSQ_ERR_NOMEM;
		}
	}නැතහොත්{
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		ආපසු MOSQ_ERR_INVAL;
	}
	ආපසු MOSQ_ERR_SUCCESS;
}
