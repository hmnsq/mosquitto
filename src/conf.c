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

#شامل "config.h"

#شامل <limits.h>
#شامل <stdio.h>
#شامل <stdlib.h>
#شامل <string.h>
#شامل <errno.h>

#ifdef WIN32
#وگرنه
#  include <dirent.h>
#  include <strings.h>
#endif

#ifndef WIN32
#  include <netdb.h>
#  include <sys/socket.h>
#وگرنه
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#اگر !defined(WIN32) && !defined(__CYGWIN__)
#  include <syslog.h>
#endif

#شامل "mosquitto_broker_internal.h"
#شامل "tls_mosq.h"
#شامل "util_mosq.h"
#شامل "mosquitto/mqtt_protocol.h"

#شامل "utlist.h"

ساختار config_recurse {
	unsigned صحیح log_dest;
	صحیح log_dest_set;
	unsigned صحیح log_type;
	صحیح log_type_set;
};

#اگر defined(WIN32) || defined(__CYGWIN__)
#شامل <windows.h>
بیرونی SERVICE_STATUS_HANDLE service_handle;
#endif


#define REQUIRE_LISTENER(A) \
		انجامبده{ \
			اگر(cur_listener == پوچ){ \
				log__printf(پوچ, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				بازگشت MOSQ_ERR_INVAL; \
			} \
		}وقتی(0)

#define REQUIRE_LISTENER_OR_DEFAULT_LISTENER(A) \
		انجامبده{ \
			اگر(cur_listener == پوچ){ \
				اگر(config__create_default_listener(config, (A))){ \
					بازگشت MOSQ_ERR_NOMEM; \
				} \
				cur_listener = config->default_listener; \
			} \
		}وقتی(0)

#define REQUIRE_LISTENER_IF_PER_LISTENER(A) \
		انجامبده{ \
			اگر(config->per_listener_settings == true && cur_listener == پوچ){ \
				log__printf(پوچ, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				بازگشت MOSQ_ERR_INVAL; \
			} \
		}وقتی(0)

#define REQUIRE_NON_DEFAULT_LISTENER(A) \
		انجامبده{ \
			اگر(cur_listener == config->default_listener || cur_listener == پوچ){ \
				log__printf(پوچ, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				بازگشت MOSQ_ERR_INVAL; \
			} \
		}وقتی(0)

#define REQUIRE_BRIDGE(A) \
		انجامبده{ \
			اگر(cur_bridge == پوچ){ \
				log__printf(پوچ, MOSQ_LOG_ERR, "Error: The '%s' option requires a bridge to be defined first.", (A)); \
				بازگشت MOSQ_ERR_INVAL; \
			} \
		}وقتی(0)

#define REQUIRE_PLUGIN(A) \
		انجامبده{ \
			اگر(cur_plugin == پوچ){ \
				log__printf(پوچ, MOSQ_LOG_ERR, "Error: The '%s' option requires plugin/global_plugin/plugin_load to be defined first.", (A)); \
				بازگشت MOSQ_ERR_INVAL; \
			} \
		}وقتی(0)

#define OPTION_DEPRECATED(A, B) \
		log__printf(پوچ, MOSQ_LOG_NOTICE, "The '%s' option is now deprecated and will be removed in version 3.0. %s", (A), (B))

#define OPTION_UNAVAILABLE(A) \
		log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: The '%s' option is no longer available.", (A));

#define REQUIRE_NON_EMPTY_OPTION(A, B) \
		انجامبده{ \
			اگر(!(A)){ \
				log__printf(پوچ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", (B)); \
				بازگشت MOSQ_ERR_INVAL; \
			} \
		}وقتی(0)

#define PER_LISTENER_ALTERNATIVE(A, B) \
		اگر(config->per_listener_settings){ \
			log__printf(پوچ, MOSQ_LOG_NOTICE, "You are using the '%s' option with 'per_listener_settings true'. Please replace this with '%s'.", A, B); \
		} \


#ifdef FINAL_WITH_TLS_PSK
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A) \
		انجامبده{ \
			اگر(cur_bridge->tls_psk_identity || cur_bridge->tls_psk){ \
				log__printf(پوچ, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and psk encryption in a single bridge.", (A)); \
				بازگشت MOSQ_ERR_INVAL; \
			} \
		}وقتی(0)

#  define REQUIRE_BRIDGE_NO_X509(A) \
		انجامبده{ \
			اگر(cur_bridge->tls_cafile || cur_bridge->tls_capath || cur_bridge->tls_certfile || cur_bridge->tls_keyfile){ \
				log__printf(پوچ, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and identity encryption in a single bridge.", (A)); \
				بازگشت MOSQ_ERR_INVAL; \
			} \
		}وقتی(0)

#وگرنه
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A)
#  define REQUIRE_BRIDGE_NO_X509(A)
#endif

ایستا ساختار mosquitto__security_options *cur_security_options = پوچ;

ایستا صحیح conf__parse_bool(نویسه **token, ثابت نویسه *name, bool *مقدار, نویسه **saveptr);
ایستا صحیح conf__parse_int(نویسه **token, ثابت نویسه *name, صحیح *مقدار, نویسه **saveptr);
ایستا صحیح conf__parse_ssize_t(نویسه **token, ثابت نویسه *name, ssize_t *مقدار, نویسه **saveptr);
ایستا صحیح conf__parse_string(نویسه **token, ثابت نویسه *name, نویسه **مقدار, نویسه **saveptr);
ایستا صحیح config__read_file(ساختار mosquitto__config *config, bool reload, ثابت نویسه *file, ساختار config_recurse *config_tmp, صحیح level, صحیح *lineno);
ایستا صحیح config__check(ساختار mosquitto__config *config);
ایستا تهی config__cleanup_plugins(تهی);
#ifdef WITH_BRIDGE
ایستا صحیح config__check_bridges(ساختار mosquitto__config *config);
#endif


ایستا صحیح config__add_listener(ساختار mosquitto__config *config)
{
	ساختار mosquitto__listener *listener;
	ساختار mosquitto__listener *new_listeners;
	صحیح def_listener = -1;

	اگر(config->default_listener){
		برای(صحیح i=0; i<config->listener_شمار; i++){
			اگر(&config->listeners[i] == config->default_listener){
				def_listener = i;
				بشکن;
			}
		}
	}
	new_listeners = mosquitto_realloc(config->listeners, اندازه(ساختار mosquitto__listener)*(size_t)(config->listener_شمار+1));
	اگر(!new_listeners){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
		بازگشت MOSQ_ERR_NOMEM;
	}
	config->listeners = new_listeners;
	listener = &config->listeners[config->listener_شمار];
	memset(listener, 0, اندازه(ساختار mosquitto__listener));
	listener->security_options = mosquitto_calloc(1, اندازه(ساختار mosquitto__security_options));
	اگر(!listener->security_options){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
		بازگشت MOSQ_ERR_NOMEM;
	}
	اگر(def_listener != -1){
		config->default_listener = &config->listeners[def_listener];
	}
	config->listener_شمار++;

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح config__create_default_listener(ساختار mosquitto__config *config, ثابت نویسه *option_name)
{
	اگر(config->default_listener){
		بازگشت MOSQ_ERR_SUCCESS;
	}
	log__printf(پوچ, MOSQ_LOG_INFO, "Creating default listener due to '%s' option.", option_name);
	log__printf(پوچ, MOSQ_LOG_INFO, "It is best practice to define a 'listener' first. Using the '%s' option without a listener will be disabled in the future.", option_name);
	اگر(config__add_listener(config)){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
		بازگشت MOSQ_ERR_NOMEM;
	}

	config->default_listener = &config->listeners[config->listener_شمار-1];
	listener__set_defaults(config->default_listener);
	config->default_listener->port = 1883;

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا تهی conf__set_cur_security_options(ساختار mosquitto__config *config, ساختار mosquitto__listener **cur_listener, ساختار mosquitto__security_options **security_options, ثابت نویسه *option_name)
{
	اگر(config->per_listener_settings){
		اگر(*cur_listener == پوچ){
			اگر(config__create_default_listener(config, option_name)){
				بازگشت;
			}
			*cur_listener = config->default_listener;
		}
		(*security_options) = (*cur_listener)->security_options;
	}وگرنه{
		(*security_options) = &config->security_options;
	}
}


ایستا صحیح conf__attempt_resolve(ثابت نویسه *host, ثابت نویسه *text, unsigned صحیح log, ثابت نویسه *پیام)
{
	ساختار addrinfo gai_hints;
	ساختار addrinfo *gai_res;
	صحیح rc;

	memset(&gai_hints, 0, اندازه(ساختار addrinfo));
	gai_hints.ai_family = AF_UNSPEC;
	gai_hints.ai_socktype = SOCK_STREAM;
	gai_res = پوچ;
	rc = getaddrinfo(host, پوچ, &gai_hints, &gai_res);
	اگر(gai_res){
		freeaddrinfo(gai_res);
	}
	اگر(rc != 0){
#ifndef WIN32
		اگر(rc == EAI_SYSTEM){
			اگر(errno == ENOENT){
				log__printf(پوچ, log, "%s: Unable to resolve %s %s.", پیام, text, host);
			}وگرنه{
				log__printf(پوچ, log, "%s: Error resolving %s: %s.", پیام, text, strerror(errno));
			}
		}وگرنه{
			log__printf(پوچ, log, "%s: Error resolving %s: %s.", پیام, text, gai_strerror(rc));
		}
#وگرنه
		اگر(rc == WSAHOST_NOT_FOUND){
			log__printf(پوچ, log, "%s: Error resolving %s.", پیام, text);
		}
#endif
		بازگشت MOSQ_ERR_INVAL;
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا تهی config__init_reload(ساختار mosquitto__config *config)
{
	/* Set defaults */
	برای(صحیح i=0; i<config->listener_شمار; i++){
		listener__set_defaults(&config->listeners[i]);
	}

	config->local_only = true;
	config->allow_duplicate_messages = true;

	mosquitto_FREE(config->security_options.acl_data.acl_file);
	mosquitto_FREE(config->security_options.password_data.password_file);
	mosquitto_FREE(config->security_options.psk_file);

	config->security_options.allow_anonymous = -1;
	config->security_options.allow_zero_length_clientid = true;
	config->security_options.auto_id_prefix = پوچ;
	config->security_options.auto_id_prefix_len = 0;

	config->autosave_interval = 1800;
	config->autosave_on_changes = false;

	mosquitto_FREE(config->clientid_prefixes);

	config->connection_messages = true;
	config->clientid_prefixes = پوچ;
	config->per_listener_settings = false;
	اگر(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = پوچ;
	}
	mosquitto_FREE(config->log_file);

#اگر defined(WIN32) || defined(__CYGWIN__)
	اگر(service_handle){
		/* This is running as a Windows service. Default to no logging. Using
		 * stdout/stderr is forbidden because the first clients to connect will
		 * get log information sent to them for some reason. */
		config->log_dest = MQTT3_LOG_NONE;
	}وگرنه{
		config->log_dest = MQTT3_LOG_STDERR;
	}
#وگرنه
	config->log_facility = LOG_DAEMON;
	config->log_dest = MQTT3_LOG_STDERR | MQTT3_LOG_DLT;
	اگر(db.quiet){
		config->log_type = 0;
	}وگرنه اگر(db.verbose){
		config->log_type = UINT_MAX;
	}وگرنه{
		config->log_type = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;
	}
#endif
	config->log_timestamp = true;
	mosquitto_FREE(config->log_timestamp_format);
	config->global_max_clients = -1;
	config->global_max_connections = -1;
	config->log_timestamp_format = پوچ;
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


ایستا تهی config__cleanup_plugin_config(mosquitto_plugin_id_t *plugin)
{
	mosquitto_FREE(plugin->config.path);
	mosquitto_FREE(plugin->config.name);

	اگر(plugin->config.options){
		برای(صحیح j=0; j<plugin->config.option_شمار; j++){
			mosquitto_FREE(plugin->config.options[j].کلید);
			mosquitto_FREE(plugin->config.options[j].مقدار);
		}
		mosquitto_FREE(plugin->config.options);
		plugin->config.option_شمار = 0;
	}
	mosquitto_FREE(plugin->config.security_options);
	mosquitto_FREE(plugin);
}


ایستا تهی config__cleanup_plugins(تهی)
{
	برای(صحیح i=0; i<db.plugin_شمار; i++){
		config__cleanup_plugin_config(db.plugins[i]);
	}
	mosquitto_FREE(db.plugins);
}


تهی config__init(ساختار mosquitto__config *config)
{
	memset(config, 0, اندازه(ساختار mosquitto__config));
	config__init_reload(config);

	config->daemon = false;
}


تهی config__cleanup(ساختار mosquitto__config *config)
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
	اگر(config->listeners){
		برای(صحیح i=0; i<config->listener_شمار; i++){
			mosquitto_FREE(config->listeners[i].host);
			mosquitto_FREE(config->listeners[i].bind_interface);
			mosquitto_FREE(config->listeners[i].mount_point);
			mosquitto_FREE(config->listeners[i].socks);
			اگر(config->listeners[i].security_options){
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
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			اگر(!config->listeners[i].ws_context) /* libwebsockets frees its own SSL_CTX */
#endif
			{
				SSL_CTX_free(config->listeners[i].ssl_ctx);
				config->listeners[i].ssl_ctx = پوچ;
			}
#endif
#اگر defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
			mosquitto_FREE(config->listeners[i].http_dir);
#endif
#ifdef WITH_WEBSOCKETS
			برای(صحیح j=0; j<config->listeners[i].ws_origin_شمار; j++){
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
	اگر(config->bridges){
		برای(صحیح i=0; i<config->bridge_شمار; i++){
			config__bridge_cleanup(config->bridges[i]);
		}
		mosquitto_FREE(config->bridges);
	}
#endif
	config__cleanup_plugins();

	اگر(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = پوچ;
	}
	اگر(config->log_file){
		mosquitto_FREE(config->log_file);
		config->log_file = پوچ;
	}
}

#ifdef WITH_BRIDGE


تهی config__bridge_cleanup(ساختار mosquitto__bridge *bridge)
{
	اگر(bridge == پوچ){
		بازگشت;
	}

	mosquitto_FREE(bridge->name);
	اگر(bridge->addresses){
		برای(صحیح i=0; i<bridge->address_شمار; i++){
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
	اگر(bridge->topics){
		ساختار mosquitto__bridge_topic *cur_topic, *topic_tmp;

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


ایستا تهی print_version(تهی)
{
	printf("mosquitto %s\n", VERSION);
	printf("Copyright © 2025 Roger Light.\n");
	printf("License EPL-2.0 OR BSD-3-Clause.\n");
}


ایستا تهی print_usage(تهی)
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


صحیح config__parse_args(ساختار mosquitto__config *config, صحیح argc, نویسه *argv[])
{
	صحیح i;
	صحیح port_tmp;

	برای(i=1; i<argc; i++){
		اگر(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--config-file")){
			اگر(i<argc-1){
				db.config_file = argv[i+1];

				اگر(config__read(config, false)){
					بازگشت MOSQ_ERR_INVAL;
				}
			}وگرنه{
				log__printf(پوچ, MOSQ_LOG_ERR, "Error: -c argument given, but no config file specified.");
				بازگشت MOSQ_ERR_INVAL;
			}
			i++;
		}وگرنه اگر(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--daemon")){
			config->daemon = true;
		}وگرنه اگر(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
			print_usage();
			بازگشت MOSQ_ERR_UNKNOWN;
		}وگرنه اگر(!strcmp(argv[i], "--version")){
			print_version();
			بازگشت MOSQ_ERR_UNKNOWN;
		}وگرنه اگر(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port")){
			اگر(i<argc-1){
				port_tmp = atoi(argv[i+1]);
				اگر(port_tmp<1 || port_tmp>UINT16_MAX){
					log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid port specified (%d).", port_tmp);
					بازگشت MOSQ_ERR_INVAL;
				}وگرنه{
					اگر(config->cmd_port_شمار == CMD_PORT_LIMIT){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Only %d ports can be specified on the command line.", CMD_PORT_LIMIT);
						بازگشت MOSQ_ERR_INVAL;
					}
					config->cmd_port[config->cmd_port_شمار] = (uint16_t)port_tmp;
					config->cmd_port_شمار++;
				}
			}وگرنه{
				log__printf(پوچ, MOSQ_LOG_ERR, "Error: -p argument given, but no port specified.");
				بازگشت MOSQ_ERR_INVAL;
			}
			i++;
		}وگرنه اگر(!strcmp(argv[i], "--tls-keylog")){
#ifdef WITH_TLS
			اگر(i<argc-1){
				db.tls_keylog = mosquitto_strdup(argv[i+1]);
				اگر(db.tls_keylog == پوچ){
					log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
					بازگشت MOSQ_ERR_NOMEM;
				}
			}وگرنه{
				log__printf(پوچ, MOSQ_LOG_ERR, "Error: --tls-keylog argument given, but no file specified.");
				بازگشت MOSQ_ERR_INVAL;
			}
			i++;
#وگرنه
			fprintf(stderr, "Error: TLS support not available so --tls-keylog is not available.\n");
			بازگشت MOSQ_ERR_INVAL;
#endif
		}وگرنه اگر(!strcmp(argv[i], "-q") || !strcmp(argv[i], "--quiet")){
			db.quiet = true;
		}وگرنه اگر(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")){
			db.verbose = true;
		}وگرنه اگر(!strcmp(argv[i], "--test-config")){
			config->test_configuration = true;
		}وگرنه{
			fprintf(stderr, "Error: Unknown option '%s'.\n", argv[i]);
			print_usage();
			بازگشت MOSQ_ERR_INVAL;
		}
	}

	/* Default to drop to mosquitto user if we are privileged and no user specified. */
	اگر(!config->user){
		config->user = mosquitto_strdup("mosquitto");
		اگر(config->user == پوچ){
			بازگشت MOSQ_ERR_NOMEM;
		}
	}
	اگر(db.quiet){
		config->log_type = 0;
	}وگرنه اگر(db.verbose){
		config->log_type = UINT_MAX;
	}

	اگر(getenv("MOSQUITTO_PERSISTENCE_LOCATION")){
		mosquitto_FREE(config->persistence_location);
		config->persistence_location = mosquitto_strdup(getenv("MOSQUITTO_PERSISTENCE_LOCATION"));
		اگر(!config->persistence_location){
			بازگشت MOSQ_ERR_NOMEM;
		}
	}
	بازگشت config__check(config);
}


ایستا تهی config__copy(ساختار mosquitto__config *src, ساختار mosquitto__config *dest)
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
	dest->security_options.plugin_شمار = src->security_options.plugin_شمار;
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

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	dest->websockets_log_level = src->websockets_log_level;
#endif

#ifdef WITH_BRIDGE
	برای(صحیح i=0; i<dest->bridge_شمار; i++){
		اگر(dest->bridges[i]){
			config__bridge_cleanup(dest->bridges[i]);
		}
	}
	mosquitto_FREE(dest->bridges);
	dest->bridges = src->bridges;
	dest->bridge_شمار = src->bridge_شمار;
#endif
}


صحیح config__read(ساختار mosquitto__config *config, bool reload)
{
	صحیح rc = MOSQ_ERR_SUCCESS;
	ساختار config_recurse cr;
	صحیح lineno = 0;
#ifdef WITH_PERSISTENCE
	size_t len;
#endif
	ساختار mosquitto__config config_reload;
	صحیح i;

	اگر(reload){
		memset(&config_reload, 0, اندازه(ساختار mosquitto__config));
	}

	cr.log_dest = MQTT3_LOG_NONE;
	cr.log_dest_set = 0;
	cr.log_type = MOSQ_LOG_NONE;
	cr.log_type_set = 0;

	اگر(!db.config_file){
		بازگشت 0;
	}

	اگر(reload){
		/* Re-initialise appropriate config vars to default for reload. */
		config__init_reload(&config_reload);
		config_reload.listeners = config->listeners;
		config_reload.listener_شمار = config->listener_شمار;
		cur_security_options = پوچ;
		rc = config__read_file(&config_reload, reload, db.config_file, &cr, 0, &lineno);
	}وگرنه{
		rc = config__read_file(config, reload, db.config_file, &cr, 0, &lineno);
	}
	اگر(rc){
		اگر(lineno > 0){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error found at %s:%d.", db.config_file, lineno);
		}
		بازگشت rc;
	}

	اگر(reload){
		config__copy(&config_reload, config);
	}

	/* If auth/access options are set and allow_anonymous not explicitly set, disallow anon. */
	اگر(config->local_only == false){
		اگر(config->per_listener_settings){
			برای(i=0; i<config->listener_شمار; i++){
				/* Default option if no security options set */
				اگر(config->listeners[i].security_options->allow_anonymous == -1){
					config->listeners[i].security_options->allow_anonymous = false;
				}
			}
		}وگرنه{
			اگر(config->security_options.allow_anonymous == -1){
				config->security_options.allow_anonymous = false;
			}
		}
	}
#ifdef WITH_PERSISTENCE
	اگر(config->persistence){
		اگر(!config->persistence_file){
			config->persistence_file = mosquitto_strdup("mosquitto.db");
			اگر(!config->persistence_file){
				بازگشت MOSQ_ERR_NOMEM;
			}
		}
		mosquitto_FREE(config->persistence_filepath);
		اگر(config->persistence_location && strlen(config->persistence_location)){
			len = strlen(config->persistence_location) + strlen(config->persistence_file) + 2;
			config->persistence_filepath = mosquitto_malloc(len);
			اگر(!config->persistence_filepath){
				بازگشت MOSQ_ERR_NOMEM;
			}
#ifdef WIN32
			snprintf(config->persistence_filepath, len, "%s\\%s", config->persistence_location, config->persistence_file);
#وگرنه
			snprintf(config->persistence_filepath, len, "%s/%s", config->persistence_location, config->persistence_file);
#endif
		}وگرنه{
			config->persistence_filepath = mosquitto_strdup(config->persistence_file);
			اگر(!config->persistence_filepath){
				بازگشت MOSQ_ERR_NOMEM;
			}
		}
	}
#endif
	/* Default to drop to mosquitto user if no other user specified. This must
	 * remain here even though it is covered in config__parse_args() because this
	 * function may be called on its own. */
	اگر(!config->user){
		config->user = mosquitto_strdup("mosquitto");
	}

#ifdef WITH_BRIDGE
	برای(i=0; i<config->bridge_شمار; i++){
		اگر(!config->bridges[i]->name){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: bridge name not defined.");
			بازگشت MOSQ_ERR_INVAL;
		}
		اگر(config->bridges[i]->addresses  == 0){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no remote addresses defined.");
			بازگشت MOSQ_ERR_INVAL;
		}
		اگر(config->bridges[i]->topic_شمار == 0){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no topics defined.");
			بازگشت MOSQ_ERR_INVAL;
		}
#ifdef FINAL_WITH_TLS_PSK
		اگر(config->bridges[i]->tls_psk && !config->bridges[i]->tls_psk_identity){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_identity.");
			بازگشت MOSQ_ERR_INVAL;
		}
		اگر(config->bridges[i]->tls_psk_identity && !config->bridges[i]->tls_psk){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_psk.");
			بازگشت MOSQ_ERR_INVAL;
		}
#endif
	}
#endif

	اگر(cr.log_dest_set){
		config->log_dest = cr.log_dest;
	}
	اگر(db.quiet){
		config->log_type = 0;
	}وگرنه اگر(db.verbose){
		config->log_type = UINT_MAX;
	}وگرنه اگر(cr.log_type_set){
		config->log_type = cr.log_type;
	}
#ifdef WITH_BRIDGE
	بازگشت config__check_bridges(config);
#وگرنه
	بازگشت MOSQ_ERR_SUCCESS;
#endif
}


ایستا mosquitto_plugin_id_t *config__plugin_find(ثابت نویسه *name)
{
	اگر(db.plugins && name){
		برای(صحیح i=0; i<db.plugin_شمار; i++){
			اگر(db.plugins[i]->config.name && !strcmp(db.plugins[i]->config.name, name)){
				بازگشت db.plugins[i];
			}
		}
	}
	بازگشت پوچ;
}


ایستا mosquitto_plugin_id_t *config__plugin_load(ثابت نویسه *name, ثابت نویسه *path)
{
	mosquitto_plugin_id_t *plugin = پوچ;
	mosquitto_plugin_id_t **plugins = پوچ;

	اگر(name && config__plugin_find(name)){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Duplicate plugin name '%s'.", name);
		بازگشت پوچ;
	}

	اگر(!path || !strcmp(path, "")){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Missing plugin path for plugin name '%s'.", name);
		بازگشت پوچ;
	}

	plugin = mosquitto_calloc(1, اندازه(mosquitto_plugin_id_t));
	اگر(!plugin){
		برو error;
	}

	اگر(name){
		plugin->config.name = mosquitto_strdup(name);
	}
	plugin->config.path = mosquitto_strdup(path);
	اگر((name && !plugin->config.name) || !plugin->config.path){
		برو error;
	}
	plugin->config.options = پوچ;
	plugin->config.option_شمار = 0;
	plugin->config.deny_special_chars = true;

	/* Add to db list */
	plugins = mosquitto_realloc(db.plugins, (size_t)(db.plugin_شمار+1)*اندازه(ساختار mosquitto__plugin_config *));
	اگر(!plugins){
		برو error;
	}

	plugins[db.plugin_شمار] = plugin;
	db.plugins = plugins;
	db.plugin_شمار++;

	بازگشت plugin;
error:
	log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
	اگر(plugin){
		mosquitto_FREE(plugin->config.name);
		mosquitto_FREE(plugin->config.path);
	}
	mosquitto_FREE(plugin);
	بازگشت پوچ;
}


صحیح config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, ساختار mosquitto__security_options *security_options)
{
	ساختار mosquitto__security_options **new_options;
	mosquitto_plugin_id_t **new_plugins;

	new_options = mosquitto_realloc(plugin->config.security_options, (size_t)(plugin->config.security_option_شمار+1)*اندازه(ساختار mosquitto__security_options *));
	new_plugins = mosquitto_realloc(security_options->plugins, (size_t)(security_options->plugin_شمار+1)*اندازه(mosquitto_plugin_id_t *));

	اگر(!new_options || !new_plugins){
		mosquitto_FREE(new_options);
		mosquitto_FREE(new_plugins);
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
		بازگشت MOSQ_ERR_NOMEM;
	}

	new_options[plugin->config.security_option_شمار] = security_options;
	plugin->config.security_options = new_options;
	plugin->config.security_option_شمار++;

	new_plugins[security_options->plugin_شمار] = plugin;
	security_options->plugins = new_plugins;
	security_options->plugin_شمار++;

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح config__read_file_core(ساختار mosquitto__config *config, bool reload, ساختار config_recurse *cr, صحیح level, صحیح *lineno, FILE *fptr, نویسه **buf, صحیح *buflen)
{
	صحیح rc;
	نویسه *token;
	صحیح tmp_int;
	نویسه *saveptr = پوچ;
#ifdef WITH_BRIDGE
	نویسه *tmp_char;
	ساختار mosquitto__bridge *cur_bridge = پوچ;
#endif
	mosquitto_plugin_id_t *cur_plugin = پوچ;

	نویسه *کلید;
	ساختار mosquitto__listener *cur_listener = پوچ;
	صحیح i;
	صحیح lineno_ext = 0;
	size_t prefix_len;
	size_t slen;
#ifdef WITH_WEBSOCKETS
	نویسه **ws_origins = پوچ;
#endif

	*lineno = 0;

	وقتی(mosquitto_fgets(buf, buflen, fptr)){
		(*lineno)++;
		اگر((*buf)[0] != '#' && (*buf)[0] != 10 && (*buf)[0] != 13){
			slen = strlen(*buf);
			اگر(slen == 0){
				ادامه;
			}
			وقتی((*buf)[slen-1] == 10 || (*buf)[slen-1] == 13){
				(*buf)[slen-1] = 0;
				slen = strlen(*buf);
				اگر(slen == 0){
					ادامه;
				}
			}
			token = strtok_r((*buf), " ", &saveptr);
			اگر(token){
				اگر(!strcmp(token, "accept_protocol_versions")){
					REQUIRE_NON_DEFAULT_LISTENER(token);
					cur_listener->disable_protocol_v3 = true;
					cur_listener->disable_protocol_v4 = true;
					cur_listener->disable_protocol_v5 = true;
					اگر(saveptr == پوچ){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", "accept_protocol_versions");
						بازگشت MOSQ_ERR_INVAL;
					}
					token = strtok_r(saveptr, ", \t", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "accept_protocol_versions");

					وقتی(token){
						اگر(!strcmp(token, "3")){
							cur_listener->disable_protocol_v3 = false;
						}وگرنه اگر(!strcmp(token, "4")){
							cur_listener->disable_protocol_v4 = false;
						}وگرنه اگر(!strcmp(token, "5")){
							cur_listener->disable_protocol_v5 = false;
						}

						token = strtok_r(پوچ, ", \t", &saveptr);
					}
				}وگرنه اگر(!strcmp(token, "acl_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);

					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->acl_data.acl_file);
					اگر(conf__parse_string(&token, "acl_file", &cur_security_options->acl_data.acl_file, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "address") || !strcmp(token, "addresses")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(cur_bridge->addresses){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid bridge configuration, 'address' only allowed once.");
						بازگشت MOSQ_ERR_INVAL;
					}
					وقتی((token = strtok_r(پوچ, " ", &saveptr))){
						اگر(token[0] == '#'){
							بشکن;
						}
						ساختار bridge_address *new_addresses = mosquitto_realloc(cur_bridge->addresses, اندازه(ساختار bridge_address)*(size_t)(cur_bridge->address_شمار+1));
						اگر(!new_addresses){
							log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
							بازگشت MOSQ_ERR_NOMEM;
						}
						cur_bridge->address_شمار++;
						cur_bridge->addresses = new_addresses;
						memset(&cur_bridge->addresses[cur_bridge->address_شمار-1], 0, اندازه(ساختار bridge_address));
						cur_bridge->addresses[cur_bridge->address_شمار-1].address = mosquitto_strdup(token);
						اگر(!cur_bridge->addresses[cur_bridge->address_شمار-1].address){
							log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
							بازگشت MOSQ_ERR_NOMEM;
						}
					}
					برای(i=0; i<cur_bridge->address_شمار; i++){
						/* cur_bridge->addresses[i].address is now
						 * "address[:port]". If address is an IPv6 address,
						 * then port is required. We must check for the :
						 * backwards. */
						tmp_char = strrchr(cur_bridge->addresses[i].address, ':');
						اگر(tmp_char){
							/* Remove ':', so cur_bridge->addresses[i].address
							 * now just looks like the address. */
							tmp_char[0] = '\0';

							/* The remainder of the string */
							tmp_int = atoi(&tmp_char[1]);
							اگر(tmp_int < 1 || tmp_int > UINT16_MAX){
								log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid bridge port value (%d).", tmp_int);
								بازگشت MOSQ_ERR_INVAL;
							}
							cur_bridge->addresses[i].port = (uint16_t)tmp_int;
						}وگرنه{
							cur_bridge->addresses[i].port = 1883;
						}
						conf__attempt_resolve(cur_bridge->addresses[i].address, "bridge address", MOSQ_LOG_WARNING, "Warning");
					}
					اگر(cur_bridge->address_شمار == 0){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Empty address value in configuration.");
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "allow_anonymous")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					PER_LISTENER_ALTERNATIVE(token, "listener_allow_anonymous");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					اگر(conf__parse_bool(&token, "allow_anonymous", (bool *)&cur_security_options->allow_anonymous, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "allow_duplicate_messages")){
					OPTION_DEPRECATED(token, "The behaviour will default to true.");
					اگر(conf__parse_bool(&token, "allow_duplicate_messages", &config->allow_duplicate_messages, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "allow_zero_length_clientid")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					اگر(conf__parse_bool(&token, "allow_zero_length_clientid", &cur_security_options->allow_zero_length_clientid, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strncmp(token, "auth_opt_", strlen("auth_opt_")) || !strncmp(token, "plugin_opt_", strlen("plugin_opt_"))){
					اگر(reload){
						ادامه;        /* Auth plugin not currently valid for reloading. */

					}
					REQUIRE_PLUGIN(token);

					اگر(!strncmp(token, "auth_opt_", strlen("auth_opt_"))){
						prefix_len = strlen("auth_opt_");
					}وگرنه{
						prefix_len = strlen("plugin_opt_");
					}
					اگر(strlen(token) < prefix_len + 3){
						/* auth_opt_ == 9, + one digit key == 10, + one space == 11, + one value == 12 */
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid 'plugin_opt_' config option.");
						بازگشت MOSQ_ERR_INVAL;
					}
					کلید = mosquitto_strdup(&token[prefix_len]);
					اگر(!کلید){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
						بازگشت MOSQ_ERR_NOMEM;
					}وگرنه اگر(STREMPTY(کلید)){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Empty 'plugin_opt_' config option.");
						mosquitto_FREE(کلید);
						بازگشت MOSQ_ERR_INVAL;
					}
					token = saveptr;
					اگر(token && token[0]){
						وقتی(token[0] == ' ' || token[0] == '\t'){
							token++;
						}
						ساختار mosquitto_opt *new_options = mosquitto_realloc(cur_plugin->config.options, (size_t)(cur_plugin->config.option_شمار+1)*اندازه(ساختار mosquitto_opt));
						اگر(!new_options){
							log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
							mosquitto_FREE(کلید);
							بازگشت MOSQ_ERR_NOMEM;
						}
						cur_plugin->config.option_شمار++;
						cur_plugin->config.options = new_options;
						cur_plugin->config.options[cur_plugin->config.option_شمار-1].کلید = کلید;
						cur_plugin->config.options[cur_plugin->config.option_شمار-1].مقدار = mosquitto_strdup(token);
						اگر(!cur_plugin->config.options[cur_plugin->config.option_شمار-1].مقدار){
							log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
							بازگشت MOSQ_ERR_NOMEM;
						}
					}وگرنه{
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", کلید);
						mosquitto_FREE(کلید);
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "auth_plugin") || !strcmp(token, "plugin") || !strcmp(token, "global_plugin")){
					اگر(reload){
						ادامه;        /* plugin not currently valid for reloading. */
					}
					اگر(!strcmp(token, "global_plugin")){
						cur_security_options = &db.config->security_options;
					}وگرنه{
						REQUIRE_LISTENER_IF_PER_LISTENER(token);
						conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					}

					cur_plugin = config__plugin_load(پوچ, saveptr);
					اگر(cur_plugin == پوچ){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(config__plugin_add_secopt(cur_plugin, cur_security_options)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "auth_plugin_deny_special_chars")){
					اگر(reload){
						ادامه;        /* Auth plugin not currently valid for reloading. */
					}
					اگر(!cur_plugin){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: An auth_plugin_deny_special_chars option exists in the config file without a plugin.");
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(conf__parse_bool(&token, "auth_plugin_deny_special_chars", &cur_plugin->config.deny_special_chars, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "plugin_load")){
					نویسه *name = پوچ;

					اگر(reload){
						ادامه;        /* plugin not currently valid for reloading. */

					}
					name = strtok_r(پوچ, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(name, "plugin_load");

					cur_plugin = config__plugin_load(name, saveptr);
					اگر(cur_plugin == پوچ){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "plugin_use")){
					نویسه *name = پوچ;

					اگر(reload){
						ادامه;        /* plugin not currently valid for reloading. */
					}
					REQUIRE_NON_DEFAULT_LISTENER(token);

					اگر(conf__parse_string(&token, "plugin_use", &name, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					cur_plugin = config__plugin_find(name);
					اگر(!cur_plugin){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Plugin '%s' not previously loaded.", name);
						mosquitto_FREE(name);
						بازگشت MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(name);
					اگر(config__plugin_add_secopt(cur_plugin, cur_listener->security_options)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "auto_id_prefix")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					OPTION_DEPRECATED(token, "Please use 'listener_auto_id_prefix' instead.");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					اگر(conf__parse_string(&token, "auto_id_prefix", &cur_security_options->auto_id_prefix, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(cur_security_options->auto_id_prefix){
						اگر(strlen(cur_security_options->auto_id_prefix) > 50){
							log__printf(پوچ, MOSQ_LOG_ERR, "Error: auto_id_prefix length must be <= 50.");
							بازگشت MOSQ_ERR_INVAL;
						}
						cur_security_options->auto_id_prefix_len = (uint16_t)strlen(cur_security_options->auto_id_prefix);
					}وگرنه{
						cur_security_options->auto_id_prefix_len = 0;
					}
				}وگرنه اگر(!strcmp(token, "autosave_interval")){
					اگر(conf__parse_int(&token, "autosave_interval", &config->autosave_interval, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(config->autosave_interval < 0){
						config->autosave_interval = 0;
					}
				}وگرنه اگر(!strcmp(token, "autosave_on_changes")){
					اگر(conf__parse_bool(&token, "autosave_on_changes", &config->autosave_on_changes, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "bind_address")){
					OPTION_DEPRECATED(token, "");
					config->local_only = false;
					اگر(reload){
						ادامه;        /* Rebinding listeners not valid during reloading. */

					}
					اگر(config__create_default_listener(config, token)){
						بازگشت MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					اگر(conf__parse_string(&token, "default listener bind_address", &config->default_listener->host, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(conf__attempt_resolve(config->default_listener->host, "bind_address", MOSQ_LOG_ERR, "Error")){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "bind_interface")){
#ifdef SO_BINDTODEVICE
					اگر(reload){
						ادامه;        /* Rebinding listeners not valid during reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					اگر(conf__parse_string(&token, "bind_interface", &cur_listener->bind_interface, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_ERR, "Error: bind_interface specified but socket option not available.");
					بازگشت MOSQ_ERR_INVAL;
#endif
				}وگرنه اگر(!strcmp(token, "bridge_attempt_unsubscribe")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_bool(&token, "bridge_attempt_unsubscribe", &cur_bridge->attempt_unsubscribe, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "bridge_cafile")){
#اگر defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					اگر(conf__parse_string(&token, "bridge_cafile", &cur_bridge->tls_cafile, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "bridge_alpn")){
#اگر defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_string(&token, "bridge_alpn", &cur_bridge->tls_alpn, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "bridge_ciphers")){
#اگر defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_string(&token, "bridge_ciphers", &cur_bridge->tls_ciphers, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "bridge_ciphers_tls1.3")){
#اگر defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_string(&token, "bridge_ciphers_tls1.3", &cur_bridge->tls_13_ciphers, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "bridge_bind_address")){
#اگر defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_string(&token, "bridge_bind_address", &cur_bridge->bind_address, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "bridge_capath")){
#اگر defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					اگر(conf__parse_string(&token, "bridge_capath", &cur_bridge->tls_capath, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "bridge_certfile")){
#اگر defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					اگر(conf__parse_string(&token, "bridge_certfile", &cur_bridge->tls_certfile, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "bridge_identity")){
#اگر defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					اگر(conf__parse_string(&token, "bridge_identity", &cur_bridge->tls_psk_identity, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "bridge_insecure")){
#اگر defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_bool(&token, "bridge_insecure", &cur_bridge->tls_insecure, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(cur_bridge->tls_insecure){
						log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge '%s' using insecure mode.", cur_bridge->name);
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "bridge_require_ocsp")){
#اگر defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_bool(&token, "bridge_require_ocsp", &cur_bridge->tls_ocsp_required, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "bridge_max_packet_size")){
#اگر defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_int(&token, "bridge_max_packet_size", &tmp_int, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 0){
						tmp_int = 0;
					}
					cur_bridge->maximum_packet_size = (uint32_t)tmp_int;
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "bridge_max_topic_alias")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_int(&token, "bridge_max_topic_alias", &tmp_int, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}

					اگر(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: bridge_max_topic_alias must be > 0 and <= 65535.");
						بازگشت MOSQ_ERR_INVAL;
					}
					cur_bridge->max_topic_alias = (uint16_t)tmp_int;
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "bridge_outgoing_retain")){
#اگر defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_bool(&token, "bridge_outgoing_retain", &cur_bridge->outgoing_retain, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "bridge_keyfile")){
#اگر defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					mosquitto_FREE(cur_bridge->tls_keyfile);
					اگر(conf__parse_string(&token, "bridge_keyfile", &cur_bridge->tls_keyfile, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "bridge_protocol_version")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(پوچ, "", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_protocol_version");

					اگر(!strcmp(token, "mqttv31")){
						cur_bridge->protocol_version = mosq_p_mqtt31;
					}وگرنه اگر(!strcmp(token, "mqttv311")){
						cur_bridge->protocol_version = mosq_p_mqtt311;
					}وگرنه اگر(!strcmp(token, "mqttv50")){
						cur_bridge->protocol_version = mosq_p_mqtt5;
					}وگرنه{
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid 'bridge_protocol_version' value (%s).", token);
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "bridge_psk")){
#اگر defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					اگر(conf__parse_string(&token, "bridge_psk", &cur_bridge->tls_psk, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "bridge_receive_maximum")){
#اگر defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_int(&token, "bridge_receive_maximum", &tmp_int, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(tmp_int <= 0){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be greater than 0.");
						بازگشت MOSQ_ERR_INVAL;
					}وگرنه اگر((uint64_t)tmp_int > (uint64_t)UINT16_MAX){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be lower than %u.", UINT16_MAX);
						بازگشت MOSQ_ERR_INVAL;
					}
					cur_bridge->receive_maximum = (uint16_t)tmp_int;
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "bridge_reload_type")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(پوچ, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_reload_type");

					اگر(!strcmp(token, "lazy")){
						cur_bridge->reload_type = brt_lazy;
					}وگرنه اگر(!strcmp(token, "immediate")){
						cur_bridge->reload_type = brt_immediate;
					}وگرنه{
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid 'bridge_reload_type' value in configuration (%s).", token);
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "bridge_session_expiry_interval")){
#اگر defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_int(&token, "bridge_session_expiry_interval", &tmp_int, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 0){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must not be negative.");
						بازگشت MOSQ_ERR_INVAL;
					}وگرنه اگر((uint64_t)tmp_int > (uint64_t)MQTT_SESSION_EXPIRY_NEVER){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must be lower than %u.", MQTT_SESSION_EXPIRY_NEVER);
						بازگشت MOSQ_ERR_INVAL;
					}
					cur_bridge->session_expiry_interval = (uint32_t)tmp_int;
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "bridge_tcp_keepalive")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);

					اگر(conf__parse_int(&token, "bridge_tcp_keepalive_idle", &tmp_int, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(tmp_int <= 0){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: invalid TCP keepalive idle value.");
						بازگشت MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_idle = (unsigned صحیح)tmp_int;

					اگر(conf__parse_int(&token, "bridge_tcp_keepalive_interval", &tmp_int, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(tmp_int <= 0){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: invalid TCP keepalive interval value.");
						بازگشت MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_interval = (unsigned صحیح)tmp_int;

					اگر(conf__parse_int(&token, "bridge_tcp_keepalive_counter", &tmp_int, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(tmp_int <= 0){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: invalid TCP keepalive counter value.");
						بازگشت MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_counter = (unsigned صحیح)tmp_int;
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "bridge_tcp_user_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
#ifdef WITH_TCP_USER_TIMEOUT
					اگر(conf__parse_int(&token, "bridge_tcp_user_timeout", &tmp_int, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 0){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: invalid TCP user timeout value.");
						بازگشت MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_user_timeout = tmp_int;
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge TCP user timeout support not available.");
#endif
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "bridge_tls_use_os_certs")){
#اگر defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_bool(&token, "bridge_tls_use_os_certs", &cur_bridge->tls_use_os_certs, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "bridge_tls_version")){
#اگر defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_string(&token, "bridge_tls_version", &cur_bridge->tls_version, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "cafile")){
#اگر defined(WITH_TLS)
					REQUIRE_LISTENER(token);
					اگر(cur_listener->psk_hint){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						بازگشت MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->cafile);
					اگر(conf__parse_string(&token, "cafile", &cur_listener->cafile, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "capath")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->capath);
					اگر(conf__parse_string(&token, "capath", &cur_listener->capath, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "certfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					اگر(cur_listener->psk_hint){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						بازگشت MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->certfile);
					اگر(conf__parse_string(&token, "certfile", &cur_listener->certfile, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "check_retain_source")){
					اگر(conf__parse_bool(&token, "check_retain_source", &config->check_retain_source, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "ciphers")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers);
					اگر(conf__parse_string(&token, "ciphers", &cur_listener->ciphers, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "ciphers_tls1.3")){
#اگر defined(WITH_TLS) && (!defined(LIBRESSL_VERSION_عدد) || LIBRESSL_VERSION_عدد > 0x3040000FL)
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers_tls13);
					اگر(conf__parse_string(&token, "ciphers_tls1.3", &cur_listener->ciphers_tls13, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: ciphers_tls1.3 support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "clientid") || !strcmp(token, "remote_clientid")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_string(&token, "bridge remote clientid", &cur_bridge->remote_clientid, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "cleansession")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_bool(&token, "cleansession", &cur_bridge->clean_start, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "local_cleansession")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_bool(&token, "local_cleansession", (bool *)&cur_bridge->clean_start_local, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "clientid_prefixes")){
					OPTION_DEPRECATED(token, "");
					mosquitto_FREE(config->clientid_prefixes);
					اگر(conf__parse_string(&token, "clientid_prefixes", &config->clientid_prefixes, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "connection")){
#ifdef WITH_BRIDGE
					token = strtok_r(پوچ, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "connection");

					/* Check for existing bridge name. */
					برای(i=0; i<config->bridge_شمار; i++){
						اگر(!strcmp(config->bridges[i]->name, token)){
							log__printf(پوچ, MOSQ_LOG_ERR, "Error: Duplicate bridge name '%s'.", token);
							بازگشت MOSQ_ERR_INVAL;
						}
					}

					ساختار mosquitto__bridge **bridges_new = mosquitto_realloc(config->bridges, (size_t)(config->bridge_شمار+1)*اندازه(ساختار mosquitto__bridge *));
					اگر(!bridges_new){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
						بازگشت MOSQ_ERR_NOMEM;
					}
					config->bridges = bridges_new;
					cur_bridge = mosquitto_malloc(اندازه(ساختار mosquitto__bridge));
					اگر(!cur_bridge){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
						بازگشت MOSQ_ERR_NOMEM;
					}

					config->bridge_شمار++;
					config->bridges[config->bridge_شمار-1] = cur_bridge;

					memset(cur_bridge, 0, اندازه(ساختار mosquitto__bridge));
					cur_bridge->name = mosquitto_strdup(token);
					اگر(!cur_bridge->name){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
						بازگشت MOSQ_ERR_NOMEM;
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
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "connection_messages")){
					اگر(conf__parse_bool(&token, token, &config->connection_messages, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "crlfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->crlfile);
					اگر(conf__parse_string(&token, "crlfile", &cur_listener->crlfile, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "dhparamfile")){
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: dhparamfile is no longer required.");
				}وگرنه اگر(!strcmp(token, "disable_client_cert_date_checks")){
#ifdef WITH_TLS
					REQUIRE_LISTENER(token);
					اگر(conf__parse_bool(&token, "disable_client_cert_date_checks", &cur_listener->disable_client_cert_date_checks, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "enable_control_api")){
#ifdef WITH_CONTROL
					اگر(conf__parse_bool(&token, "enable_control_api", &config->enable_control_api, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: $CONTROL support not available (enable_control_api).");
#endif
				}وگرنه اگر(!strcmp(token, "enable_proxy_protocol")){
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(پوچ, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					بازگشت MOSQ_ERR_INVAL;
#endif
					REQUIRE_LISTENER(token);
					اگر(conf__parse_int(&token, "enable_proxy_protocol", &cur_listener->enable_proxy_protocol, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(cur_listener->enable_proxy_protocol < 1 || cur_listener->enable_proxy_protocol > 2){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: enable_proxy_protocol must be 1 or 2.");
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "global_max_clients")){
					اگر(conf__parse_int(&token, "global_max_clients", &config->global_max_clients, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "global_max_connections")){
					اگر(conf__parse_int(&token, "global_max_connections", &config->global_max_connections, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "http_dir")){
#اگر defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
					اگر(reload){
						ادامه;        /* Not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					اگر(conf__parse_string(&token, "http_dir", &cur_listener->http_dir, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#ifdef WIN32
					نویسه *http_dir_canonical = _fullpath(پوچ, cur_listener->http_dir, 0);
					ثابت نویسه sep = '\\';
#وگرنه
					نویسه *http_dir_canonical = realpath(cur_listener->http_dir, پوچ);
					ثابت نویسه sep = '/';
#endif
					اگر(!http_dir_canonical){
						بازگشت MOSQ_ERR_NOMEM;
					}
					size_t http_dir_len = strlen(http_dir_canonical) + اندازه(sep) + 1;
					نویسه *http_dir_canonical_sep = mosquitto_calloc(http_dir_len, اندازه(نویسه));
					اگر(!http_dir_canonical_sep){
						آزاد(http_dir_canonical);
						بازگشت MOSQ_ERR_NOMEM;
					}
					snprintf(http_dir_canonical_sep, http_dir_len, "%s%c", http_dir_canonical, sep);
					آزاد(http_dir_canonical);
					mosquitto_FREE(cur_listener->http_dir);
					cur_listener->http_dir = http_dir_canonical_sep;
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: http_dir support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "idle_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_int(&token, "idle_timeout", &cur_bridge->idle_timeout, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(cur_bridge->idle_timeout < 1){
						log__printf(پوچ, MOSQ_LOG_NOTICE, "idle_timeout interval too low, using 1 second.");
						cur_bridge->idle_timeout = 1;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "include_dir")){
					اگر(level == 0){
						نویسه **files;
						صحیح file_شمار;

						/* Only process include_dir from the main config file. */
						token = strtok_r(پوچ, "", &saveptr);
						REQUIRE_NON_EMPTY_OPTION(token, "include_dir");

						rc = config__get_dir_files(token, &files, &file_شمار);
						اگر(rc){
							بازگشت rc;
						}

						برای(i=0; i<file_شمار; i++){
							log__printf(پوچ, MOSQ_LOG_INFO, "Loading config file '%s'", files[i]);

							rc = config__read_file(config, reload, files[i], cr, level+1, &lineno_ext);
							اگر(rc){
								اگر(lineno_ext > 0){
									log__printf(پوچ, MOSQ_LOG_ERR, "Error found at '%s:%d'.", files[i], lineno_ext);
								}
								/* Free happens below */
								بشکن;
							}
						}
						برای(i=0; i<file_شمار; i++){
							mosquitto_FREE(files[i]);
						}
						mosquitto_FREE(files);
						اگر(rc){
							بازگشت rc;    /* This returns if config__read_file() fails above */
						}
					}وگرنه{
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: The include_dir option is only valid in the main configuration file.");
						بازگشت 1;
					}
				}وگرنه اگر(!strcmp(token, "keepalive_interval")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_int(&token, "keepalive_interval", &tmp_int, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(tmp_int > UINT16_MAX){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Bridge keepalive value too high.");
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 5){
						log__printf(پوچ, MOSQ_LOG_NOTICE, "keepalive interval too low, using 5 seconds.");
						tmp_int = 5;
					}
					cur_bridge->keepalive = (uint16_t)tmp_int;
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "keyfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->keyfile);
					اگر(conf__parse_string(&token, "keyfile", &cur_listener->keyfile, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "listener")){
					config->local_only = false;

					اگر(conf__parse_int(&token, "listener port", &tmp_int, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#ifdef WITH_UNIX_SOCKETS
					اگر(tmp_int < 0 || tmp_int > UINT16_MAX){
#وگرنه
					اگر(tmp_int < 1 || tmp_int > UINT16_MAX){
#endif
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						بازگشت MOSQ_ERR_INVAL;
					}

					/* Look for bind address / unix socket path */
					token = strtok_r(پوچ, " ", &saveptr);
					اگر(token != پوچ && token[0] == '#'){
						token = پوچ;
					}

					اگر(tmp_int == 0 && token == پوچ){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: A listener with port 0 must provide a Unix socket path.");
						بازگشت MOSQ_ERR_INVAL;
					}

					اگر(reload){
						/* We reload listeners settings based on port number/unix socket path.
						 * If the port number/unix path doesn't already exist, exit with a complaint. */
						cur_listener = پوچ;
#ifdef WITH_UNIX_SOCKETS
						اگر(tmp_int == 0){
							برای(i=0; i<config->listener_شمار; i++){
								اگر(config->listeners[i].unix_socket_path != پوچ
										&& strcmp(config->listeners[i].unix_socket_path, token) == 0){

									cur_listener = &config->listeners[i];
									بشکن;
								}
							}
						}وگرنه
#endif
						{
							برای(i=0; i<config->listener_شمار; i++){
								اگر(config->listeners[i].port == tmp_int){
									/* Now check we have a matching bind address, if defined */
									اگر(config->listeners[i].host){
										اگر(token && !strcmp(config->listeners[i].host, token)){
											/* They both have a bind address, and they match */
											cur_listener = &config->listeners[i];
											بشکن;
										}
									}وگرنه{
										اگر(token == پوچ){
											/* Neither this config nor the new config have a bind address,
											 * so they match. */
											cur_listener = &config->listeners[i];
											بشکن;
										}
									}
								}
							}
						}
						اگر(!cur_listener){
							log__printf(پوچ, MOSQ_LOG_ERR, "Error: It is not currently possible to add/remove listeners when reloading the config file.");
							بازگشت MOSQ_ERR_INVAL;
						}
					}وگرنه{
						اگر(config__add_listener(config)){
							log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
							بازگشت MOSQ_ERR_NOMEM;
						}
						cur_listener = &config->listeners[config->listener_شمار-1];
					}

					listener__set_defaults(cur_listener);
					cur_listener->port = (uint16_t)tmp_int;

					mosquitto_FREE(cur_listener->host);

#ifdef WITH_UNIX_SOCKETS
					mosquitto_FREE(cur_listener->unix_socket_path);
#endif

					اگر(token){
#ifdef WITH_UNIX_SOCKETS
						اگر(cur_listener->port == 0){
							cur_listener->unix_socket_path = mosquitto_strdup(token);
						}وگرنه
#endif
						{
							cur_listener->host = mosquitto_strdup(token);
						}
					}
				}وگرنه اگر(!strcmp(token, "listener_allow_anonymous")){
					REQUIRE_LISTENER(token);
					اگر(conf__parse_bool(&token, "listener_allow_anonymous", (bool *)&cur_listener->security_options->allow_anonymous, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "listener_auto_id_prefix")){
					REQUIRE_LISTENER(token);
					اگر(conf__parse_string(&token, "listener_auto_id_prefix", &cur_listener->security_options->auto_id_prefix, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(cur_listener->security_options->auto_id_prefix){
						اگر(strlen(cur_listener->security_options->auto_id_prefix) > 50){
							log__printf(پوچ, MOSQ_LOG_ERR, "Error: 'listener_auto_id_prefix' length must be <= 50.");
							بازگشت MOSQ_ERR_INVAL;
						}
						cur_listener->security_options->auto_id_prefix_len = (uint16_t)strlen(cur_listener->security_options->auto_id_prefix);
					}وگرنه{
						cur_listener->security_options->auto_id_prefix_len = 0;
					}
				}وگرنه اگر(!strcmp(token, "local_clientid")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_string(&token, "bridge local clientd", &cur_bridge->local_clientid, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "local_password")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_string(&token, "bridge local_password", &cur_bridge->local_password, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "local_username")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_string(&token, "bridge local_username", &cur_bridge->local_username, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "log_dest")){
					token = strtok_r(پوچ, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_dest");

					cr->log_dest_set = 1;
					اگر(!strcmp(token, "none")){
						cr->log_dest = MQTT3_LOG_NONE;
					}وگرنه اگر(!strcmp(token, "syslog")){
						cr->log_dest |= MQTT3_LOG_SYSLOG;
					}وگرنه اگر(!strcmp(token, "stdout")){
						cr->log_dest |= MQTT3_LOG_STDOUT;
					}وگرنه اگر(!strcmp(token, "stderr")){
						cr->log_dest |= MQTT3_LOG_STDERR;
					}وگرنه اگر(!strcmp(token, "topic")){
						cr->log_dest |= MQTT3_LOG_TOPIC;
					}وگرنه اگر(!strcmp(token, "dlt")){
						cr->log_dest |= MQTT3_LOG_DLT;
#ifdef ANDROID
					}وگرنه اگر(!strcmp(token, "android")){
						cr->log_dest |= MQTT3_LOG_ANDROID;
#endif
					}وگرنه اگر(!strcmp(token, "file")){
						اگر(config->log_fptr || config->log_file){
							log__printf(پوچ, MOSQ_LOG_ERR, "Error: Duplicate \"log_dest file\" value.");
							بازگشت MOSQ_ERR_INVAL;
						}
						/* Get remaining string. */
						token = saveptr;
						اگر(token && token[0]){
							وقتی(token[0] == ' ' || token[0] == '\t'){
								token++;
							}
						}
						/* Duplicate "token" check here saves a log__printf() */
						اگر(token && token[0]){
							config->log_file = mosquitto_strdup(token);
							اگر(!config->log_file){
								log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
								بازگشت MOSQ_ERR_NOMEM;
							}
							cr->log_dest |= MQTT3_LOG_FILE;
						}وگرنه{
							log__printf(پوچ, MOSQ_LOG_ERR, "Error: Empty \"log_dest file\" value in configuration.");
							بازگشت MOSQ_ERR_INVAL;
						}
						cr->log_dest |= MQTT3_LOG_FILE;
					}وگرنه{
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid 'log_dest' value (%s).", token);
						بازگشت MOSQ_ERR_INVAL;
					}
#اگر defined(WIN32) || defined(__CYGWIN__)
					اگر(service_handle){
						اگر(cr->log_dest == MQTT3_LOG_STDOUT || cr->log_dest == MQTT3_LOG_STDERR){
							log__printf(پوچ, MOSQ_LOG_ERR, "Error: Cannot log to stdout/stderr when running as a Windows service.");
							بازگشت MOSQ_ERR_INVAL;
						}
					}
#endif
				}وگرنه اگر(!strcmp(token, "log_facility")){
#اگر defined(WIN32) || defined(__CYGWIN__)
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: log_facility not supported on Windows.");
#وگرنه
					اگر(conf__parse_int(&token, "log_facility", &tmp_int, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					انتخاب(tmp_int){
						حالت 0:
							config->log_facility = LOG_LOCAL0;
							بشکن;
						حالت 1:
							config->log_facility = LOG_LOCAL1;
							بشکن;
						حالت 2:
							config->log_facility = LOG_LOCAL2;
							بشکن;
						حالت 3:
							config->log_facility = LOG_LOCAL3;
							بشکن;
						حالت 4:
							config->log_facility = LOG_LOCAL4;
							بشکن;
						حالت 5:
							config->log_facility = LOG_LOCAL5;
							بشکن;
						حالت 6:
							config->log_facility = LOG_LOCAL6;
							بشکن;
						حالت 7:
							config->log_facility = LOG_LOCAL7;
							بشکن;
						پیشفرض:
							log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid 'log_facility' value (%d).", tmp_int);
							بازگشت MOSQ_ERR_INVAL;
					}
#endif
				}وگرنه اگر(!strcmp(token, "log_timestamp")){
					اگر(conf__parse_bool(&token, token, &config->log_timestamp, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "log_timestamp_format")){
					اگر(conf__parse_string(&token, token, &config->log_timestamp_format, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "log_type")){
					token = strtok_r(پوچ, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_type");

					cr->log_type_set = 1;
					اگر(!strcmp(token, "none")){
						cr->log_type = MOSQ_LOG_NONE;
					}وگرنه اگر(!strcmp(token, "information")){
						cr->log_type |= MOSQ_LOG_INFO;
					}وگرنه اگر(!strcmp(token, "notice")){
						cr->log_type |= MOSQ_LOG_NOTICE;
					}وگرنه اگر(!strcmp(token, "warning")){
						cr->log_type |= MOSQ_LOG_WARNING;
					}وگرنه اگر(!strcmp(token, "error")){
						cr->log_type |= MOSQ_LOG_ERR;
					}وگرنه اگر(!strcmp(token, "debug")){
						cr->log_type |= MOSQ_LOG_DEBUG;
					}وگرنه اگر(!strcmp(token, "subscribe")){
						cr->log_type |= MOSQ_LOG_SUBSCRIBE;
					}وگرنه اگر(!strcmp(token, "unsubscribe")){
						cr->log_type |= MOSQ_LOG_UNSUBSCRIBE;
					}وگرنه اگر(!strcmp(token, "internal")){
						cr->log_type |= MOSQ_LOG_INTERNAL;
#ifdef WITH_WEBSOCKETS
					}وگرنه اگر(!strcmp(token, "websockets")){
						cr->log_type |= MOSQ_LOG_WEBSOCKETS;
#endif
					}وگرنه اگر(!strcmp(token, "all")){
						cr->log_type = MOSQ_LOG_ALL;
					}وگرنه{
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid 'log_type' value (%s).", token);
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "max_connections")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					اگر(conf__parse_int(&token, token, &cur_listener->max_connections, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(cur_listener->max_connections < 0){
						cur_listener->max_connections = -1;
					}
				}وگرنه اگر(!strcmp(token, "maximum_qos") || !strcmp(token, "max_qos")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					اگر(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 0 || tmp_int > 2){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: 'max_qos' must be between 0 and 2 inclusive.");
						بازگشت MOSQ_ERR_INVAL;
					}
					cur_listener->max_qos = (uint8_t)tmp_int;
				}وگرنه اگر(!strcmp(token, "max_inflight_bytes")){
					اگر(conf__parse_int(&token, "max_inflight_bytes", &tmp_int, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_inflight_bytes = (size_t)tmp_int;
				}وگرنه اگر(!strcmp(token, "max_inflight_messages")){
					اگر(conf__parse_int(&token, "max_inflight_messages", &tmp_int, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 0 || tmp_int == UINT16_MAX){
						tmp_int = 0;
					}وگرنه اگر(tmp_int > UINT16_MAX){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: 'max_inflight_messages' must be <= 65535.");
						بازگشت MOSQ_ERR_INVAL;
					}
					config->max_inflight_messages = (uint16_t)tmp_int;
				}وگرنه اگر(!strcmp(token, "max_keepalive")){
					اگر(conf__parse_int(&token, "max_keepalive", &tmp_int, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid 'max_keepalive' value (%d).", tmp_int);
						بازگشت MOSQ_ERR_INVAL;
					}
					config->max_keepalive = (uint16_t)tmp_int;
				}وگرنه اگر(!strcmp(token, "max_packet_size")){
					اگر(conf__parse_int(&token, "max_packet_size", &tmp_int, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 20){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: 'max_packet_size' must be >= 20.");
						بازگشت MOSQ_ERR_INVAL;
					}
					config->max_packet_size = (uint32_t)tmp_int;
				}وگرنه اگر(!strcmp(token, "max_queued_bytes")){
					اگر(conf__parse_int(&token, "max_queued_bytes", &tmp_int, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_bytes = (size_t)tmp_int;
				}وگرنه اگر(!strcmp(token, "max_queued_messages")){
					اگر(conf__parse_int(&token, "max_queued_messages", &tmp_int, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_messages = tmp_int;
				}وگرنه اگر(!strcmp(token, "memory_limit")){
					ssize_t lim;
					اگر(conf__parse_ssize_t(&token, "memory_limit", &lim, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(lim < 0){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid 'memory_limit' value (%ld).", lim);
						بازگشت MOSQ_ERR_INVAL;
					}
					mosquitto_memory_set_limit((size_t)lim);
				}وگرنه اگر(!strcmp(token, "message_size_limit")){
					log__printf(پوچ, MOSQ_LOG_NOTICE, "Note: It is recommended to replace `message_size_limit` with `max_packet_size`.");
					اگر(conf__parse_int(&token, "message_size_limit", (صحیح *)&config->message_size_limit, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(config->message_size_limit > MQTT_MAX_PAYLOAD){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid 'message_size_limit' value (%u).", config->message_size_limit);
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "mount_point")){
					REQUIRE_LISTENER(token);
					mosquitto_FREE(cur_listener->mount_point);
					اگر(conf__parse_string(&token, "mount_point", &cur_listener->mount_point, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(mosquitto_pub_topic_check(cur_listener->mount_point) != MOSQ_ERR_SUCCESS){
						log__printf(پوچ, MOSQ_LOG_ERR,
								"Error: Invalid 'mount_point' value (%s). Does it contain a wildcard character?",
								cur_listener->mount_point);
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "notifications")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_bool(&token, "notifications", &cur_bridge->notifications, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "notifications_local_only")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_bool(&token, "notifications_local_only", &cur_bridge->notifications_local_only, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "notification_topic")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_string(&token, "notification_topic", &cur_bridge->notification_topic, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "password") || !strcmp(token, "remote_password")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_string(&token, "bridge remote_password", &cur_bridge->remote_password, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "password_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->password_data.password_file);
					اگر(conf__parse_string(&token, "password_file", &cur_security_options->password_data.password_file, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "per_listener_settings")){
					OPTION_DEPRECATED(token, "Please see the documentation for how to achieve the same effect.");
					اگر(config->per_listener_settings){
						/* Once this is set, don't let it be unset. It should be the first config option ideally. */
						ادامه;
					}
					اگر(conf__parse_bool(&token, "per_listener_settings", &config->per_listener_settings, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(cur_security_options && config->per_listener_settings){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: per_listener_settings must be set before any other security settings.");
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "persistence") || !strcmp(token, "retained_persistence")){
					اگر(conf__parse_bool(&token, token, &config->persistence, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "persistence_file")){
					اگر(conf__parse_string(&token, "persistence_file", &config->persistence_file, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "persistence_location")){
					اگر(conf__parse_string(&token, "persistence_location", &config->persistence_location, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "persistent_client_expiration")){
					token = strtok_r(پوچ, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "persistent_client_expiration");

					time_t expiration_mult;

					انتخاب(token[strlen(token)-1]){
						حالت 's':
							expiration_mult = 1;
							بشکن;
						حالت 'h':
							expiration_mult = 3600;
							بشکن;
						حالت 'd':
							expiration_mult = 86400;
							بشکن;
						حالت 'w':
							expiration_mult = 86400*7;
							بشکن;
						حالت 'm':
							expiration_mult = 86400*30;
							بشکن;
						حالت 'y':
							expiration_mult = 86400*365;
							بشکن;
						پیشفرض:
							log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
							بازگشت MOSQ_ERR_INVAL;
					}
					token[strlen(token)-1] = '\0';
					config->persistent_client_expiration = atoi(token)*expiration_mult;
					اگر(config->persistent_client_expiration <= 0){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "pid_file")){
					اگر(reload){
						ادامه;        /* pid file not valid for reloading. */
					}
					اگر(conf__parse_string(&token, "pid_file", &config->pid_file, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "port")){
					OPTION_DEPRECATED(token, "Please use 'listener' instead.");
					config->local_only = false;
					اگر(reload){
						ادامه;        /* Listeners not valid for reloading. */

					}
					اگر(config__create_default_listener(config, token)){
						بازگشت MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					اگر(config->default_listener->port){
						log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Default listener port specified multiple times. Only the latest will be used.");
					}
					اگر(conf__parse_int(&token, "port", &tmp_int, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 1 || tmp_int > UINT16_MAX){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						بازگشت MOSQ_ERR_INVAL;
					}
					config->default_listener->port = (uint16_t)tmp_int;
				}وگرنه اگر(!strcmp(token, "protocol")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(پوچ, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "protocol");

					اگر(!strcmp(token, "mqtt")){
						cur_listener->protocol = mp_mqtt;
						/*
						}else if(!strcmp(token, "mqttsn")){
						    cur_listener->protocol = mp_mqttsn;
						*/
					}وگرنه اگر(!strcmp(token, "websockets")){
#ifdef WITH_WEBSOCKETS
						cur_listener->protocol = mp_websockets;
#وگرنه
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Websockets support not available.");
						بازگشت MOSQ_ERR_INVAL;
#endif
					}وگرنه اگر(!strcmp(token, "http_api")){
#ifdef WITH_HTTP_API
						cur_listener->protocol = mp_http_api;
#وگرنه
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: HTTP API support not available.");
						بازگشت MOSQ_ERR_INVAL;
#endif
					}وگرنه{
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid 'protocol' value (%s).", token);
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "proxy_protocol_v2_require_tls")){
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(پوچ, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					بازگشت MOSQ_ERR_INVAL;
#endif
					REQUIRE_LISTENER(token);
					اگر(conf__parse_bool(&token, "proxy_protocol_v2_require_tls", &cur_listener->proxy_protocol_v2_require_tls, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "psk_file")){
#ifdef FINAL_WITH_TLS_PSK
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					اگر(cur_listener && cur_listener->certfile){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						بازگشت MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_security_options->psk_file);
					اگر(conf__parse_string(&token, "psk_file", &cur_security_options->psk_file, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "psk_hint")){
#ifdef FINAL_WITH_TLS_PSK
					اگر(reload){
						ادامه;        /* PSK file not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					اگر(conf__parse_string(&token, "psk_hint", &cur_listener->psk_hint, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "queue_qos0_messages")){
					اگر(conf__parse_bool(&token, token, &config->queue_qos0_messages, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "require_certificate")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					اگر(conf__parse_bool(&token, "require_certificate", &cur_listener->require_certificate, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "restart_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					cur_bridge->backoff_cap = 0; /* set backoff to constant mode, unless cap is specified further down */
					token = strtok_r(پوچ, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "restart_timeout");

					cur_bridge->restart_timeout = atoi(token);
					cur_bridge->backoff_base = 0;
					cur_bridge->backoff_cap = 0;
					اگر(cur_bridge->restart_timeout < 1){
						log__printf(پوچ, MOSQ_LOG_NOTICE, "restart_timeout interval too low, using 1 second.");
						cur_bridge->restart_timeout = 1;
					}وگرنه اگر(cur_bridge->restart_timeout > 3600){
						log__printf(پوچ, MOSQ_LOG_NOTICE, "restart_timeout interval too high, using 3600 seconds.");
						cur_bridge->restart_timeout = 3600;
					}
					token = strtok_r(پوچ, " ", &saveptr);
					اگر(token){
						cur_bridge->backoff_base = cur_bridge->restart_timeout;
						cur_bridge->backoff_cap = atoi(token);
						اگر(cur_bridge->backoff_cap < cur_bridge->backoff_base){
							log__printf(پوچ, MOSQ_LOG_ERR, "Error: backoff cap is lower than the base in restart_timeout.");
							بازگشت MOSQ_ERR_INVAL;
						}وگرنه اگر(cur_bridge->backoff_cap > 7200){
							log__printf(پوچ, MOSQ_LOG_ERR, "Error: backoff cap too high, using 7200 seconds.");
							cur_bridge->backoff_cap = 7200;
						}

						token = strtok_r(پوچ, " ", &saveptr);
						اگر(token){
							cur_bridge->stable_connection_period = atoi(token);
							اگر(cur_bridge->stable_connection_period < 0){
								log__printf(پوچ, MOSQ_LOG_ERR, "Error: stable connection period cannot be negative.");
								بازگشت MOSQ_ERR_INVAL;
							}
						}
					}
					cur_bridge->restart_timeout *= 1000; /* backoff is tracked in ms */
					cur_bridge->backoff_base *= 1000;
					cur_bridge->backoff_cap *= 1000;
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "retain_available")){
					اگر(conf__parse_bool(&token, token, &config->retain_available, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "retain_expiry_interval")){
					اگر(conf__parse_int(&token, token, &config->retain_expiry_interval, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(config->retain_expiry_interval < 1){
						log__printf(پوچ, MOSQ_LOG_WARNING, "Error: retain_expiry_interval must be >= 1.");
						بازگشت MOSQ_ERR_INVAL;
					}وگرنه اگر(config->retain_expiry_interval > 10000000){
						log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: retain_expiry_interval being capped at 19 years.");
						config->retain_expiry_interval = 10000000;
					}
					config->retain_expiry_interval *= 60;
				}وگرنه اگر(!strcmp(token, "retry_interval")){
					OPTION_UNAVAILABLE(token);
				}وگرنه اگر(!strcmp(token, "round_robin")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_bool(&token, "round_robin", &cur_bridge->round_robin, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "set_tcp_nodelay")){
					اگر(conf__parse_bool(&token, "set_tcp_nodelay", &config->set_tcp_nodelay, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "start_type")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(پوچ, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					اگر(!strcmp(token, "automatic")){
						cur_bridge->start_type = bst_automatic;
					}وگرنه اگر(!strcmp(token, "lazy")){
						cur_bridge->start_type = bst_lazy;
					}وگرنه اگر(!strcmp(token, "manual")){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Manual start_type not supported.");
						بازگشت MOSQ_ERR_INVAL;
					}وگرنه اگر(!strcmp(token, "once")){
						cur_bridge->start_type = bst_once;
					}وگرنه{
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid 'start_type' value in configuration (%s).", token);
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "socket_domain")){
					اگر(reload){
						ادامه;        /* socket_domain not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(پوچ, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					اگر(!strcmp(token, "ipv4")){
						cur_listener->socket_domain = AF_INET;
					}وگرنه اگر(!strcmp(token, "ipv6")){
						cur_listener->socket_domain = AF_INET6;
					}وگرنه{
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid 'socket_domain' value '%s' in configuration.", token);
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "sys_interval")){
					اگر(conf__parse_int(&token, "sys_interval", &config->sys_interval, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(config->sys_interval < 0 || config->sys_interval > 65535){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid 'sys_interval' value (%d).", config->sys_interval);
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "threshold")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_int(&token, "threshold", &cur_bridge->threshold, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(cur_bridge->threshold < 1){
						log__printf(پوچ, MOSQ_LOG_NOTICE, "threshold too low, using 1 message.");
						cur_bridge->threshold = 1;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "tls_engine")){
#ifdef WITH_TLS
					اگر(reload){
						ادامه;        /* tls_engine not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					اگر(conf__parse_string(&token, "tls_engine", &cur_listener->tls_engine, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "tls_engine_kpass_sha1")){
#ifdef WITH_TLS
					اگر(reload){
						ادامه;        /* tls_engine not valid for reloading. */

					}
					نویسه *kpass_sha = پوچ, *kpass_sha_bin = پوچ;
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					اگر(conf__parse_string(&token, "tls_engine_kpass_sha1", &kpass_sha, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(mosquitto__hex2bin_sha1(kpass_sha, (unsigned نویسه **)&kpass_sha_bin) != MOSQ_ERR_SUCCESS){
						mosquitto_FREE(kpass_sha);
						بازگشت MOSQ_ERR_INVAL;
					}
					mosquitto_free(cur_listener->tls_engine_kpass_sha1);
					cur_listener->tls_engine_kpass_sha1 = kpass_sha_bin;
					mosquitto_FREE(kpass_sha);
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "tls_keyform")){
#ifdef WITH_TLS
					اگر(reload){
						ادامه;        /* tls_engine not valid for reloading. */

					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					نویسه *keyform = پوچ;
					اگر(conf__parse_string(&token, "tls_keyform", &keyform, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					cur_listener->tls_keyform = mosq_k_pem;
					اگر(!strcmp(keyform, "engine")){
						cur_listener->tls_keyform = mosq_k_engine;
					}
					mosquitto_FREE(keyform);
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "tls_version")){
#اگر defined(WITH_TLS)
					اگر(reload){
						ادامه;        /* tls_version not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					اگر(conf__parse_string(&token, "tls_version", &cur_listener->tls_version, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "topic")){
#ifdef WITH_BRIDGE
					نویسه *topic = پوچ;
					شمارش mosquitto__bridge_direction direction = bd_out;
					uint8_t qos = 0;
					نویسه *local_prefix = پوچ, *remote_prefix = پوچ;

					REQUIRE_BRIDGE(token);

					token = strtok_r(پوچ, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "topic");

					// Check if the topic is quoted (e.g. for spaces within topic names), but not the
					// special case of ""
					اگر(token[0] == '"' && token [1] != '"'){
						اگر(strchr(saveptr, '"') == پوچ){
							log__printf(پوچ, MOSQ_LOG_ERR, "Error: Missing closing quote in topic value (%s).", saveptr);
							بازگشت MOSQ_ERR_INVAL;
						}

						نویسه *topic_in_quotes = strtok_r(پوچ, "\"", &saveptr);
						size_t tlen = 1;
						اگر(topic_in_quotes){
							tlen = strlen(topic_in_quotes);
						}

						topic = mosquitto_malloc(strlen(token) + tlen + 1);
						اگر(!topic){
							log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
							بازگشت MOSQ_ERR_NOMEM;
						}

						strcpy(topic, token + 1);
						strcat(topic, " ");
						اگر(topic_in_quotes){
							strcat(topic, topic_in_quotes);
						}
					}وگرنه{
						topic = mosquitto_strdup(token);
						اگر(!topic){
							log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
							بازگشت MOSQ_ERR_NOMEM;
						}
					}

					token = strtok_r(پوچ, " ", &saveptr);
					اگر(token){
						اگر(!strcasecmp(token, "out")){
							direction = bd_out;
						}وگرنه اگر(!strcasecmp(token, "in")){
							direction = bd_in;
						}وگرنه اگر(!strcasecmp(token, "both")){
							direction = bd_both;
						}وگرنه{
							log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid bridge topic direction '%s'.", token);
							mosquitto_FREE(topic);
							بازگشت MOSQ_ERR_INVAL;
						}
						token = strtok_r(پوچ, " ", &saveptr);
						اگر(token){
							اگر(token[0] == '#'){
								(تهی)strtok_r(پوچ, "", &saveptr);
							}
							qos = (uint8_t)atoi(token);
							اگر(qos > 2){
								log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid bridge QoS level '%s'.", token);
								mosquitto_FREE(topic);
								بازگشت MOSQ_ERR_INVAL;
							}

							token = strtok_r(پوچ, " ", &saveptr);
							اگر(token){
								اگر(!strcmp(token, "\"\"") || token[0] == '#'){
									local_prefix = پوچ;
									اگر(token[0] == '#'){
										(تهی)strtok_r(پوچ, "", &saveptr);
									}
								}وگرنه{
									local_prefix = token;
								}

								token = strtok_r(پوچ, " ", &saveptr);
								اگر(token){
									اگر(!strcmp(token, "\"\"") || token[0] == '#'){
										remote_prefix = پوچ;
									}وگرنه{
										remote_prefix = token;
									}
								}
							}
						}
					}

					اگر(bridge__add_topic(cur_bridge, topic, direction, qos, local_prefix, remote_prefix)){
						mosquitto_FREE(topic);
						بازگشت MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(topic);
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "max_topic_alias")){
					REQUIRE_LISTENER(token);
					اگر(conf__parse_int(&token, "max_topic_alias", &tmp_int, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias' value in configuration.");
						بازگشت MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias = (uint16_t)tmp_int;
				}وگرنه اگر(!strcmp(token, "max_topic_alias_broker")){
					REQUIRE_LISTENER(token);
					اگر(conf__parse_int(&token, "max_topic_alias_broker", &tmp_int, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias_broker' value in configuration.");
						بازگشت MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias_broker = (uint16_t)tmp_int;
				}وگرنه اگر(!strcmp(token, "try_private")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_bool(&token, "try_private", &cur_bridge->try_private, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "upgrade_outgoing_qos")){
					اگر(conf__parse_bool(&token, token, &config->upgrade_outgoing_qos, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "use_identity_as_username")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					اگر(conf__parse_bool(&token, "use_identity_as_username", &cur_listener->use_identity_as_username, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "use_subject_as_username")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					اگر(conf__parse_bool(&token, "use_subject_as_username", &cur_listener->use_subject_as_username, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "user")){
					اگر(reload){
						ادامه;        /* Drop privileges user not valid for reloading. */
					}
					mosquitto_FREE(config->user);
					اگر(conf__parse_string(&token, "user", &config->user, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "use_username_as_clientid")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					اگر(conf__parse_bool(&token, "use_username_as_clientid", &cur_listener->use_username_as_clientid, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
				}وگرنه اگر(!strcmp(token, "username") || !strcmp(token, "remote_username")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_string(&token, "bridge remote_username", &cur_bridge->remote_username, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}وگرنه اگر(!strcmp(token, "websockets_log_level")){
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					اگر(conf__parse_int(&token, "websockets_log_level", &config->websockets_log_level, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
#endif
				}وگرنه اگر(!strcmp(token, "websockets_headers_size") || !strcmp(token, "packet_buffer_size")){
					اگر(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(پوچ, MOSQ_LOG_WARNING, "Error: Packet buffer size must be between 0 and 65535 inclusive.");
						بازگشت MOSQ_ERR_INVAL;
					}
					config->packet_buffer_size = (uint16_t)tmp_int;
				}وگرنه اگر(!strcmp(token, "websockets_origin")){
#ifdef WITH_WEBSOCKETS
#  اگر LWS_LIBRARY_VERSION_عدد >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN
					REQUIRE_LISTENER(token);
					ws_origins = mosquitto_realloc(cur_listener->ws_origins, اندازه(نویسه *)*(size_t)(cur_listener->ws_origin_شمار+1));
					اگر(ws_origins == پوچ){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
						بازگشت MOSQ_ERR_NOMEM;
					}
					ws_origins[cur_listener->ws_origin_شمار] = پوچ;
					cur_listener->ws_origins = ws_origins;
					اگر(conf__parse_string(&token, "websockets_origin", &ws_origins[cur_listener->ws_origin_شمار], &saveptr)){
						بازگشت MOSQ_ERR_INVAL;
					}
					cur_listener->ws_origin_شمار++;
#  وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: websockets_origin support not available, libwebsockets version is too old.");
#  endif
#وگرنه
					log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Websockets support not available.");
#endif
				}وگرنه{
					log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unknown configuration variable '%s'.", token);
					بازگشت MOSQ_ERR_INVAL;
				}
			}
		}
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح config__read_file(ساختار mosquitto__config *config, bool reload, ثابت نویسه *file, ساختار config_recurse *cr, صحیح level, صحیح *lineno)
{
	صحیح rc;
	FILE *fptr = پوچ;
	نویسه *buf;
	صحیح buflen;
#ifndef WIN32
	DIR *dir;
#endif

#ifndef WIN32
	dir = opendir(file);
	اگر(dir){
		closedir(dir);
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Config file '%s' is a directory.", file);
		بازگشت 1;
	}
#endif

	fptr = mosquitto_fopen(file, "rt", false);
	اگر(!fptr){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to open config file '%s'.", file);
		بازگشت 1;
	}

	buflen = 1000;
	buf = mosquitto_malloc((size_t)buflen);
	اگر(!buf){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
		fclose(fptr);
		بازگشت MOSQ_ERR_NOMEM;
	}

	rc = config__read_file_core(config, reload, cr, level, lineno, fptr, &buf, &buflen);
	mosquitto_FREE(buf);
	fclose(fptr);

	بازگشت rc;
}


ایستا صحیح config__check_proxy(ساختار mosquitto__config *config)
{
	برای(صحیح i=0; i<config->listener_شمار; i++){
		ساختار mosquitto__listener *l = &config->listeners[i];

		اگر(l->enable_proxy_protocol == 2){
#ifdef WITH_TLS
			اگر(l->use_subject_as_username){
				log__printf(پوچ, MOSQ_LOG_ERR, "Error: use_subject_as_username cannot be used with `enable_proxy_protocol 2`.");
				بازگشت MOSQ_ERR_INVAL;
			}

			اگر(l->certfile || l->keyfile){
				log__printf(پوچ, MOSQ_LOG_ERR, "Error: certfile and keyfile cannot be used with `enable_proxy_protocol 2`.");
				بازگشت MOSQ_ERR_INVAL;
			}
#endif
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح config__check(ساختار mosquitto__config *config)
{
	/* Checks that are easy to make after the config has been loaded. */

	ثابت نویسه *id_prefix;
	صحیح id_prefix_len;
	اگر(config->security_options.auto_id_prefix){
		id_prefix = config->security_options.auto_id_prefix;
		id_prefix_len = config->security_options.auto_id_prefix_len;
	}وگرنه{
		id_prefix = "auto-";
		id_prefix_len = (صحیح)strlen("auto-");
	}

	/* Default to auto_id_prefix = 'auto-' if none set. */
	برای(صحیح i=0; i<config->listener_شمار; i++){
		اگر(!config->listeners[i].security_options->auto_id_prefix){
			config->listeners[i].security_options->auto_id_prefix = mosquitto_strdup(id_prefix);
			اگر(!config->listeners[i].security_options->auto_id_prefix){
				بازگشت MOSQ_ERR_NOMEM;
			}
			config->listeners[i].security_options->auto_id_prefix_len = (uint16_t)id_prefix_len;
		}
	}

	بازگشت config__check_proxy(config);
}

#ifdef WITH_BRIDGE


ایستا صحیح config__check_bridges(ساختار mosquitto__config *config)
{
	ساختار mosquitto__bridge *bridge1, *bridge2;
	نویسه hostname[256];
	size_t len;

	/* Check for bridge duplicate local_clientid, need to generate missing IDs
	 * first. */
	برای(صحیح i=0; i<config->bridge_شمار; i++){
		bridge1 = config->bridges[i];

		اگر(!bridge1->remote_clientid){
			اگر(!gethostname(hostname, 256)){
				len = strlen(hostname) + strlen(bridge1->name) + 2;
				bridge1->remote_clientid = mosquitto_malloc(len);
				اگر(!bridge1->remote_clientid){
					بازگشت MOSQ_ERR_NOMEM;
				}
				snprintf(bridge1->remote_clientid, len, "%s.%s", hostname, bridge1->name);
			}وگرنه{
				بازگشت 1;
			}
		}

		اگر(!bridge1->local_clientid){
			len = strlen(bridge1->remote_clientid) + strlen("local.") + 2;
			bridge1->local_clientid = mosquitto_malloc(len);
			اگر(!bridge1->local_clientid){
				log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
				بازگشت MOSQ_ERR_NOMEM;
			}
			snprintf(bridge1->local_clientid, len, "local.%s", bridge1->remote_clientid);
		}
	}

	برای(صحیح i=0; i<config->bridge_شمار; i++){
		bridge1 = config->bridges[i];
		برای(صحیح j=i+1; j<config->bridge_شمار; j++){
			bridge2 = config->bridges[j];
			اگر(!strcmp(bridge1->local_clientid, bridge2->local_clientid)){
				log__printf(پوچ, MOSQ_LOG_ERR, "Error: Bridge local_clientid "
						"'%s' is not unique. Try changing or setting the "
						"local_clientid value for one of the bridges.",
						bridge1->local_clientid);
				بازگشت MOSQ_ERR_INVAL;
			}
		}
	}

#ifdef WITH_TLS
	/* Check for missing TLS cafile/capath/certfile/keyfile */
	برای(صحیح i=0; i<config->listener_شمار; i++){
		bool cafile = !!config->listeners[i].cafile;
		bool capath = !!config->listeners[i].capath;
		bool certfile = !!config->listeners[i].certfile;
		bool keyfile = !!config->listeners[i].keyfile;

		اگر((certfile && !keyfile) || (!certfile && keyfile)){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Both certfile and keyfile must be provided to enable a TLS listener.");
			بازگشت MOSQ_ERR_INVAL;
		}
		اگر(cafile && !certfile){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: cafile specified without certfile and keyfile.");
			بازگشت MOSQ_ERR_INVAL;
		}
		اگر(capath && !certfile){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: capath specified without certfile and keyfile.");
			بازگشت MOSQ_ERR_INVAL;
		}
	}
#endif
	بازگشت MOSQ_ERR_SUCCESS;
}
#endif


ایستا صحیح conf__parse_bool(نویسه **token, ثابت نویسه *name, bool *مقدار, نویسه **saveptr)
{
	*token = strtok_r(پوچ, " ", saveptr);
	اگر(*token){
		اگر(!strcmp(*token, "false") || !strcmp(*token, "0")){
			*مقدار = false;
		}وگرنه اگر(!strcmp(*token, "true") || !strcmp(*token, "1")){
			*مقدار = true;
		}وگرنه{
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Invalid '%s' value (%s).", name, *token);
			بازگشت MOSQ_ERR_INVAL;
		}
	}وگرنه{
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		بازگشت MOSQ_ERR_INVAL;
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح conf__parse_int(نویسه **token, ثابت نویسه *name, صحیح *مقدار, نویسه **saveptr)
{
	*token = strtok_r(پوچ, " ", saveptr);
	اگر(*token){
		نویسه *endptr = پوچ;
		long l = strtol(*token, &endptr, 0);
		اگر(endptr == *token || endptr[0] != '\0'){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: '%s' value not a number.", name);
			بازگشت MOSQ_ERR_INVAL;
		}
		اگر(l > INT_MAX || l < INT_MIN){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: '%s' value out of range.", name);
			بازگشت MOSQ_ERR_INVAL;
		}
		*مقدار = (صحیح)l;
	}وگرنه{
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		بازگشت MOSQ_ERR_INVAL;
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح conf__parse_ssize_t(نویسه **token, ثابت نویسه *name, ssize_t *مقدار, نویسه **saveptr)
{
	*token = strtok_r(پوچ, " ", saveptr);
	اگر(*token){
		*مقدار = atol(*token);
	}وگرنه{
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		بازگشت MOSQ_ERR_INVAL;
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح conf__parse_string(نویسه **token, ثابت نویسه *name, نویسه **مقدار, نویسه **saveptr)
{
	size_t tlen;

	*token = strtok_r(پوچ, "", saveptr);
	اگر(*token){
		اگر(*مقدار){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Duplicate '%s' value in configuration.", name);
			بازگشت MOSQ_ERR_INVAL;
		}
		/* Deal with multiple spaces at the beginning of the string. */
		*token = mosquitto_trimblanks(*token);
		اگر(strlen(*token) == 0){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
			بازگشت MOSQ_ERR_INVAL;
		}

		tlen = strlen(*token);
		اگر(tlen > UINT16_MAX){
			بازگشت MOSQ_ERR_INVAL;
		}
		اگر(mosquitto_validate_utf8(*token, (uint16_t)tlen)){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Malformed UTF-8 in configuration.");
			بازگشت MOSQ_ERR_INVAL;
		}
		*مقدار = mosquitto_strdup(*token);
		اگر(!*مقدار){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
			بازگشت MOSQ_ERR_NOMEM;
		}
	}وگرنه{
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		بازگشت MOSQ_ERR_INVAL;
	}
	بازگشت MOSQ_ERR_SUCCESS;
}
