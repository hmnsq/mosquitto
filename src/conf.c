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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব <limits.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <errno.h>

#ifdef WIN32
#অন্যথা
#  include <dirent.h>
#  include <strings.h>
#endif

#ifndef WIN32
#  include <netdb.h>
#  include <sys/socket.h>
#অন্যথা
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#যদি !defined(WIN32) && !defined(__CYGWIN__)
#  include <syslog.h>
#endif

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "tls_mosq.h"
#অন্তর্ভাব "util_mosq.h"
#অন্তর্ভাব "mosquitto/mqtt_protocol.h"

#অন্তর্ভাব "utlist.h"

সংরচনা config_recurse {
	unsigned পূর্ণাঙ্ক log_dest;
	পূর্ণাঙ্ক log_dest_set;
	unsigned পূর্ণাঙ্ক log_type;
	পূর্ণাঙ্ক log_type_set;
};

#যদি defined(WIN32) || defined(__CYGWIN__)
#অন্তর্ভাব <windows.h>
বাহ্য SERVICE_STATUS_HANDLE service_handle;
#endif


#define REQUIRE_LISTENER(A) \
		কুরু{ \
			যদি(cur_listener == রিক্ত){ \
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				প্রত্যাগম MOSQ_ERR_INVAL; \
			} \
		}যাবত্(0)

#define REQUIRE_LISTENER_OR_DEFAULT_LISTENER(A) \
		কুরু{ \
			যদি(cur_listener == রিক্ত){ \
				যদি(config__create_default_listener(config, (A))){ \
					প্রত্যাগম MOSQ_ERR_NOMEM; \
				} \
				cur_listener = config->default_listener; \
			} \
		}যাবত্(0)

#define REQUIRE_LISTENER_IF_PER_LISTENER(A) \
		কুরু{ \
			যদি(config->per_listener_settings == true && cur_listener == রিক্ত){ \
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				প্রত্যাগম MOSQ_ERR_INVAL; \
			} \
		}যাবত্(0)

#define REQUIRE_NON_DEFAULT_LISTENER(A) \
		কুরু{ \
			যদি(cur_listener == config->default_listener || cur_listener == রিক্ত){ \
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				প্রত্যাগম MOSQ_ERR_INVAL; \
			} \
		}যাবত্(0)

#define REQUIRE_BRIDGE(A) \
		কুরু{ \
			যদি(cur_bridge == রিক্ত){ \
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: The '%s' option requires a bridge to be defined first.", (A)); \
				প্রত্যাগম MOSQ_ERR_INVAL; \
			} \
		}যাবত্(0)

#define REQUIRE_PLUGIN(A) \
		কুরু{ \
			যদি(cur_plugin == রিক্ত){ \
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: The '%s' option requires plugin/global_plugin/plugin_load to be defined first.", (A)); \
				প্রত্যাগম MOSQ_ERR_INVAL; \
			} \
		}যাবত্(0)

#define OPTION_DEPRECATED(A, B) \
		log__printf(রিক্ত, MOSQ_LOG_NOTICE, "The '%s' option is now deprecated and will be removed in version 3.0. %s", (A), (B))

#define OPTION_UNAVAILABLE(A) \
		log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: The '%s' option is no longer available.", (A));

#define REQUIRE_NON_EMPTY_OPTION(A, B) \
		কুরু{ \
			যদি(!(A)){ \
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", (B)); \
				প্রত্যাগম MOSQ_ERR_INVAL; \
			} \
		}যাবত্(0)

#define PER_LISTENER_ALTERNATIVE(A, B) \
		যদি(config->per_listener_settings){ \
			log__printf(রিক্ত, MOSQ_LOG_NOTICE, "You are using the '%s' option with 'per_listener_settings true'. Please replace this with '%s'.", A, B); \
		} \


#ifdef FINAL_WITH_TLS_PSK
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A) \
		কুরু{ \
			যদি(cur_bridge->tls_psk_identity || cur_bridge->tls_psk){ \
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and psk encryption in a single bridge.", (A)); \
				প্রত্যাগম MOSQ_ERR_INVAL; \
			} \
		}যাবত্(0)

#  define REQUIRE_BRIDGE_NO_X509(A) \
		কুরু{ \
			যদি(cur_bridge->tls_cafile || cur_bridge->tls_capath || cur_bridge->tls_certfile || cur_bridge->tls_keyfile){ \
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and identity encryption in a single bridge.", (A)); \
				প্রত্যাগম MOSQ_ERR_INVAL; \
			} \
		}যাবত্(0)

#অন্যথা
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A)
#  define REQUIRE_BRIDGE_NO_X509(A)
#endif

স্থির সংরচনা mosquitto__security_options *cur_security_options = রিক্ত;

স্থির পূর্ণাঙ্ক conf__parse_bool(বর্ণ **token, ধ্রুব বর্ণ *name, bool *মান, বর্ণ **saveptr);
স্থির পূর্ণাঙ্ক conf__parse_int(বর্ণ **token, ধ্রুব বর্ণ *name, পূর্ণাঙ্ক *মান, বর্ণ **saveptr);
স্থির পূর্ণাঙ্ক conf__parse_ssize_t(বর্ণ **token, ধ্রুব বর্ণ *name, ssize_t *মান, বর্ণ **saveptr);
স্থির পূর্ণাঙ্ক conf__parse_string(বর্ণ **token, ধ্রুব বর্ণ *name, বর্ণ **মান, বর্ণ **saveptr);
স্থির পূর্ণাঙ্ক config__read_file(সংরচনা mosquitto__config *config, bool reload, ধ্রুব বর্ণ *file, সংরচনা config_recurse *config_tmp, পূর্ণাঙ্ক level, পূর্ণাঙ্ক *lineno);
স্থির পূর্ণাঙ্ক config__check(সংরচনা mosquitto__config *config);
স্থির শূন্য config__cleanup_plugins(শূন্য);
#ifdef WITH_BRIDGE
স্থির পূর্ণাঙ্ক config__check_bridges(সংরচনা mosquitto__config *config);
#endif


স্থির পূর্ণাঙ্ক config__add_listener(সংরচনা mosquitto__config *config)
{
	সংরচনা mosquitto__listener *listener;
	সংরচনা mosquitto__listener *new_listeners;
	পূর্ণাঙ্ক def_listener = -1;

	যদি(config->default_listener){
		প্রতি(পূর্ণাঙ্ক i=0; i<config->listener_গুনতি; i++){
			যদি(&config->listeners[i] == config->default_listener){
				def_listener = i;
				ভঙ্গ;
			}
		}
	}
	new_listeners = mosquitto_realloc(config->listeners, পরিমাণ(সংরচনা mosquitto__listener)*(size_t)(config->listener_গুনতি+1));
	যদি(!new_listeners){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
		প্রত্যাগম MOSQ_ERR_NOMEM;
	}
	config->listeners = new_listeners;
	listener = &config->listeners[config->listener_গুনতি];
	memset(listener, 0, পরিমাণ(সংরচনা mosquitto__listener));
	listener->security_options = mosquitto_calloc(1, পরিমাণ(সংরচনা mosquitto__security_options));
	যদি(!listener->security_options){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
		প্রত্যাগম MOSQ_ERR_NOMEM;
	}
	যদি(def_listener != -1){
		config->default_listener = &config->listeners[def_listener];
	}
	config->listener_গুনতি++;

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণাঙ্ক config__create_default_listener(সংরচনা mosquitto__config *config, ধ্রুব বর্ণ *option_name)
{
	যদি(config->default_listener){
		প্রত্যাগম MOSQ_ERR_SUCCESS;
	}
	log__printf(রিক্ত, MOSQ_LOG_INFO, "Creating default listener due to '%s' option.", option_name);
	log__printf(রিক্ত, MOSQ_LOG_INFO, "It is best practice to define a 'listener' first. Using the '%s' option without a listener will be disabled in the future.", option_name);
	যদি(config__add_listener(config)){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
		প্রত্যাগম MOSQ_ERR_NOMEM;
	}

	config->default_listener = &config->listeners[config->listener_গুনতি-1];
	listener__set_defaults(config->default_listener);
	config->default_listener->port = 1883;

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


স্থির শূন্য conf__set_cur_security_options(সংরচনা mosquitto__config *config, সংরচনা mosquitto__listener **cur_listener, সংরচনা mosquitto__security_options **security_options, ধ্রুব বর্ণ *option_name)
{
	যদি(config->per_listener_settings){
		যদি(*cur_listener == রিক্ত){
			যদি(config__create_default_listener(config, option_name)){
				প্রত্যাগম;
			}
			*cur_listener = config->default_listener;
		}
		(*security_options) = (*cur_listener)->security_options;
	}অন্যথা{
		(*security_options) = &config->security_options;
	}
}


স্থির পূর্ণাঙ্ক conf__attempt_resolve(ধ্রুব বর্ণ *host, ধ্রুব বর্ণ *text, unsigned পূর্ণাঙ্ক log, ধ্রুব বর্ণ *বার্তা)
{
	সংরচনা addrinfo gai_hints;
	সংরচনা addrinfo *gai_res;
	পূর্ণাঙ্ক rc;

	memset(&gai_hints, 0, পরিমাণ(সংরচনা addrinfo));
	gai_hints.ai_family = AF_UNSPEC;
	gai_hints.ai_socktype = SOCK_STREAM;
	gai_res = রিক্ত;
	rc = getaddrinfo(host, রিক্ত, &gai_hints, &gai_res);
	যদি(gai_res){
		freeaddrinfo(gai_res);
	}
	যদি(rc != 0){
#ifndef WIN32
		যদি(rc == EAI_SYSTEM){
			যদি(errno == ENOENT){
				log__printf(রিক্ত, log, "%s: Unable to resolve %s %s.", বার্তা, text, host);
			}অন্যথা{
				log__printf(রিক্ত, log, "%s: Error resolving %s: %s.", বার্তা, text, strerror(errno));
			}
		}অন্যথা{
			log__printf(রিক্ত, log, "%s: Error resolving %s: %s.", বার্তা, text, gai_strerror(rc));
		}
#অন্যথা
		যদি(rc == WSAHOST_NOT_FOUND){
			log__printf(রিক্ত, log, "%s: Error resolving %s.", বার্তা, text);
		}
#endif
		প্রত্যাগম MOSQ_ERR_INVAL;
	}
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


স্থির শূন্য config__init_reload(সংরচনা mosquitto__config *config)
{
	/* Set defaults */
	প্রতি(পূর্ণাঙ্ক i=0; i<config->listener_গুনতি; i++){
		listener__set_defaults(&config->listeners[i]);
	}

	config->local_only = true;
	config->allow_duplicate_messages = true;

	mosquitto_FREE(config->security_options.acl_data.acl_file);
	mosquitto_FREE(config->security_options.password_data.password_file);
	mosquitto_FREE(config->security_options.psk_file);

	config->security_options.allow_anonymous = -1;
	config->security_options.allow_zero_length_clientid = true;
	config->security_options.auto_id_prefix = রিক্ত;
	config->security_options.auto_id_prefix_len = 0;

	config->autosave_interval = 1800;
	config->autosave_on_changes = false;

	mosquitto_FREE(config->clientid_prefixes);

	config->connection_messages = true;
	config->clientid_prefixes = রিক্ত;
	config->per_listener_settings = false;
	যদি(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = রিক্ত;
	}
	mosquitto_FREE(config->log_file);

#যদি defined(WIN32) || defined(__CYGWIN__)
	যদি(service_handle){
		/* This is running as a Windows service. Default to no logging. Using
		 * stdout/stderr is forbidden because the first clients to connect will
		 * get log information sent to them for some reason. */
		config->log_dest = MQTT3_LOG_NONE;
	}অন্যথা{
		config->log_dest = MQTT3_LOG_STDERR;
	}
#অন্যথা
	config->log_facility = LOG_DAEMON;
	config->log_dest = MQTT3_LOG_STDERR | MQTT3_LOG_DLT;
	যদি(db.quiet){
		config->log_type = 0;
	}অন্যথা যদি(db.verbose){
		config->log_type = UINT_MAX;
	}অন্যথা{
		config->log_type = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;
	}
#endif
	config->log_timestamp = true;
	mosquitto_FREE(config->log_timestamp_format);
	config->global_max_clients = -1;
	config->global_max_connections = -1;
	config->log_timestamp_format = রিক্ত;
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


স্থির শূন্য config__cleanup_plugin_config(mosquitto_plugin_id_t *plugin)
{
	mosquitto_FREE(plugin->config.path);
	mosquitto_FREE(plugin->config.name);

	যদি(plugin->config.options){
		প্রতি(পূর্ণাঙ্ক j=0; j<plugin->config.option_গুনতি; j++){
			mosquitto_FREE(plugin->config.options[j].চাবি);
			mosquitto_FREE(plugin->config.options[j].মান);
		}
		mosquitto_FREE(plugin->config.options);
		plugin->config.option_গুনতি = 0;
	}
	mosquitto_FREE(plugin->config.security_options);
	mosquitto_FREE(plugin);
}


স্থির শূন্য config__cleanup_plugins(শূন্য)
{
	প্রতি(পূর্ণাঙ্ক i=0; i<db.plugin_গুনতি; i++){
		config__cleanup_plugin_config(db.plugins[i]);
	}
	mosquitto_FREE(db.plugins);
}


শূন্য config__init(সংরচনা mosquitto__config *config)
{
	memset(config, 0, পরিমাণ(সংরচনা mosquitto__config));
	config__init_reload(config);

	config->daemon = false;
}


শূন্য config__cleanup(সংরচনা mosquitto__config *config)
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
	যদি(config->listeners){
		প্রতি(পূর্ণাঙ্ক i=0; i<config->listener_গুনতি; i++){
			mosquitto_FREE(config->listeners[i].host);
			mosquitto_FREE(config->listeners[i].bind_interface);
			mosquitto_FREE(config->listeners[i].mount_point);
			mosquitto_FREE(config->listeners[i].socks);
			যদি(config->listeners[i].security_options){
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
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			যদি(!config->listeners[i].ws_context) /* libwebsockets frees its own SSL_CTX */
#endif
			{
				SSL_CTX_free(config->listeners[i].ssl_ctx);
				config->listeners[i].ssl_ctx = রিক্ত;
			}
#endif
#যদি defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
			mosquitto_FREE(config->listeners[i].http_dir);
#endif
#ifdef WITH_WEBSOCKETS
			প্রতি(পূর্ণাঙ্ক j=0; j<config->listeners[i].ws_origin_গুনতি; j++){
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
	যদি(config->bridges){
		প্রতি(পূর্ণাঙ্ক i=0; i<config->bridge_গুনতি; i++){
			config__bridge_cleanup(config->bridges[i]);
		}
		mosquitto_FREE(config->bridges);
	}
#endif
	config__cleanup_plugins();

	যদি(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = রিক্ত;
	}
	যদি(config->log_file){
		mosquitto_FREE(config->log_file);
		config->log_file = রিক্ত;
	}
}

#ifdef WITH_BRIDGE


শূন্য config__bridge_cleanup(সংরচনা mosquitto__bridge *bridge)
{
	যদি(bridge == রিক্ত){
		প্রত্যাগম;
	}

	mosquitto_FREE(bridge->name);
	যদি(bridge->addresses){
		প্রতি(পূর্ণাঙ্ক i=0; i<bridge->address_গুনতি; i++){
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
	যদি(bridge->topics){
		সংরচনা mosquitto__bridge_topic *cur_topic, *topic_tmp;

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


স্থির শূন্য print_version(শূন্য)
{
	printf("mosquitto %s\n", VERSION);
	printf("Copyright © 2025 Roger Light.\n");
	printf("License EPL-2.0 OR BSD-3-Clause.\n");
}


স্থির শূন্য print_usage(শূন্য)
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


পূর্ণাঙ্ক config__parse_args(সংরচনা mosquitto__config *config, পূর্ণাঙ্ক argc, বর্ণ *argv[])
{
	পূর্ণাঙ্ক i;
	পূর্ণাঙ্ক port_tmp;

	প্রতি(i=1; i<argc; i++){
		যদি(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--config-file")){
			যদি(i<argc-1){
				db.config_file = argv[i+1];

				যদি(config__read(config, false)){
					প্রত্যাগম MOSQ_ERR_INVAL;
				}
			}অন্যথা{
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: -c argument given, but no config file specified.");
				প্রত্যাগম MOSQ_ERR_INVAL;
			}
			i++;
		}অন্যথা যদি(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--daemon")){
			config->daemon = true;
		}অন্যথা যদি(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
			print_usage();
			প্রত্যাগম MOSQ_ERR_UNKNOWN;
		}অন্যথা যদি(!strcmp(argv[i], "--version")){
			print_version();
			প্রত্যাগম MOSQ_ERR_UNKNOWN;
		}অন্যথা যদি(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port")){
			যদি(i<argc-1){
				port_tmp = atoi(argv[i+1]);
				যদি(port_tmp<1 || port_tmp>UINT16_MAX){
					log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid port specified (%d).", port_tmp);
					প্রত্যাগম MOSQ_ERR_INVAL;
				}অন্যথা{
					যদি(config->cmd_port_গুনতি == CMD_PORT_LIMIT){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Only %d ports can be specified on the command line.", CMD_PORT_LIMIT);
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					config->cmd_port[config->cmd_port_গুনতি] = (uint16_t)port_tmp;
					config->cmd_port_গুনতি++;
				}
			}অন্যথা{
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: -p argument given, but no port specified.");
				প্রত্যাগম MOSQ_ERR_INVAL;
			}
			i++;
		}অন্যথা যদি(!strcmp(argv[i], "--tls-keylog")){
#ifdef WITH_TLS
			যদি(i<argc-1){
				db.tls_keylog = mosquitto_strdup(argv[i+1]);
				যদি(db.tls_keylog == রিক্ত){
					log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
					প্রত্যাগম MOSQ_ERR_NOMEM;
				}
			}অন্যথা{
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: --tls-keylog argument given, but no file specified.");
				প্রত্যাগম MOSQ_ERR_INVAL;
			}
			i++;
#অন্যথা
			fprintf(stderr, "Error: TLS support not available so --tls-keylog is not available.\n");
			প্রত্যাগম MOSQ_ERR_INVAL;
#endif
		}অন্যথা যদি(!strcmp(argv[i], "-q") || !strcmp(argv[i], "--quiet")){
			db.quiet = true;
		}অন্যথা যদি(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")){
			db.verbose = true;
		}অন্যথা যদি(!strcmp(argv[i], "--test-config")){
			config->test_configuration = true;
		}অন্যথা{
			fprintf(stderr, "Error: Unknown option '%s'.\n", argv[i]);
			print_usage();
			প্রত্যাগম MOSQ_ERR_INVAL;
		}
	}

	/* Default to drop to mosquitto user if we are privileged and no user specified. */
	যদি(!config->user){
		config->user = mosquitto_strdup("mosquitto");
		যদি(config->user == রিক্ত){
			প্রত্যাগম MOSQ_ERR_NOMEM;
		}
	}
	যদি(db.quiet){
		config->log_type = 0;
	}অন্যথা যদি(db.verbose){
		config->log_type = UINT_MAX;
	}

	যদি(getenv("MOSQUITTO_PERSISTENCE_LOCATION")){
		mosquitto_FREE(config->persistence_location);
		config->persistence_location = mosquitto_strdup(getenv("MOSQUITTO_PERSISTENCE_LOCATION"));
		যদি(!config->persistence_location){
			প্রত্যাগম MOSQ_ERR_NOMEM;
		}
	}
	প্রত্যাগম config__check(config);
}


স্থির শূন্য config__copy(সংরচনা mosquitto__config *src, সংরচনা mosquitto__config *dest)
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
	dest->security_options.plugin_গুনতি = src->security_options.plugin_গুনতি;
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

#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	dest->websockets_log_level = src->websockets_log_level;
#endif

#ifdef WITH_BRIDGE
	প্রতি(পূর্ণাঙ্ক i=0; i<dest->bridge_গুনতি; i++){
		যদি(dest->bridges[i]){
			config__bridge_cleanup(dest->bridges[i]);
		}
	}
	mosquitto_FREE(dest->bridges);
	dest->bridges = src->bridges;
	dest->bridge_গুনতি = src->bridge_গুনতি;
#endif
}


পূর্ণাঙ্ক config__read(সংরচনা mosquitto__config *config, bool reload)
{
	পূর্ণাঙ্ক rc = MOSQ_ERR_SUCCESS;
	সংরচনা config_recurse cr;
	পূর্ণাঙ্ক lineno = 0;
#ifdef WITH_PERSISTENCE
	size_t len;
#endif
	সংরচনা mosquitto__config config_reload;
	পূর্ণাঙ্ক i;

	যদি(reload){
		memset(&config_reload, 0, পরিমাণ(সংরচনা mosquitto__config));
	}

	cr.log_dest = MQTT3_LOG_NONE;
	cr.log_dest_set = 0;
	cr.log_type = MOSQ_LOG_NONE;
	cr.log_type_set = 0;

	যদি(!db.config_file){
		প্রত্যাগম 0;
	}

	যদি(reload){
		/* Re-initialise appropriate config vars to default for reload. */
		config__init_reload(&config_reload);
		config_reload.listeners = config->listeners;
		config_reload.listener_গুনতি = config->listener_গুনতি;
		cur_security_options = রিক্ত;
		rc = config__read_file(&config_reload, reload, db.config_file, &cr, 0, &lineno);
	}অন্যথা{
		rc = config__read_file(config, reload, db.config_file, &cr, 0, &lineno);
	}
	যদি(rc){
		যদি(lineno > 0){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error found at %s:%d.", db.config_file, lineno);
		}
		প্রত্যাগম rc;
	}

	যদি(reload){
		config__copy(&config_reload, config);
	}

	/* If auth/access options are set and allow_anonymous not explicitly set, disallow anon. */
	যদি(config->local_only == false){
		যদি(config->per_listener_settings){
			প্রতি(i=0; i<config->listener_গুনতি; i++){
				/* Default option if no security options set */
				যদি(config->listeners[i].security_options->allow_anonymous == -1){
					config->listeners[i].security_options->allow_anonymous = false;
				}
			}
		}অন্যথা{
			যদি(config->security_options.allow_anonymous == -1){
				config->security_options.allow_anonymous = false;
			}
		}
	}
#ifdef WITH_PERSISTENCE
	যদি(config->persistence){
		যদি(!config->persistence_file){
			config->persistence_file = mosquitto_strdup("mosquitto.db");
			যদি(!config->persistence_file){
				প্রত্যাগম MOSQ_ERR_NOMEM;
			}
		}
		mosquitto_FREE(config->persistence_filepath);
		যদি(config->persistence_location && strlen(config->persistence_location)){
			len = strlen(config->persistence_location) + strlen(config->persistence_file) + 2;
			config->persistence_filepath = mosquitto_malloc(len);
			যদি(!config->persistence_filepath){
				প্রত্যাগম MOSQ_ERR_NOMEM;
			}
#ifdef WIN32
			snprintf(config->persistence_filepath, len, "%s\\%s", config->persistence_location, config->persistence_file);
#অন্যথা
			snprintf(config->persistence_filepath, len, "%s/%s", config->persistence_location, config->persistence_file);
#endif
		}অন্যথা{
			config->persistence_filepath = mosquitto_strdup(config->persistence_file);
			যদি(!config->persistence_filepath){
				প্রত্যাগম MOSQ_ERR_NOMEM;
			}
		}
	}
#endif
	/* Default to drop to mosquitto user if no other user specified. This must
	 * remain here even though it is covered in config__parse_args() because this
	 * function may be called on its own. */
	যদি(!config->user){
		config->user = mosquitto_strdup("mosquitto");
	}

#ifdef WITH_BRIDGE
	প্রতি(i=0; i<config->bridge_গুনতি; i++){
		যদি(!config->bridges[i]->name){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: bridge name not defined.");
			প্রত্যাগম MOSQ_ERR_INVAL;
		}
		যদি(config->bridges[i]->addresses  == 0){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no remote addresses defined.");
			প্রত্যাগম MOSQ_ERR_INVAL;
		}
		যদি(config->bridges[i]->topic_গুনতি == 0){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no topics defined.");
			প্রত্যাগম MOSQ_ERR_INVAL;
		}
#ifdef FINAL_WITH_TLS_PSK
		যদি(config->bridges[i]->tls_psk && !config->bridges[i]->tls_psk_identity){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_identity.");
			প্রত্যাগম MOSQ_ERR_INVAL;
		}
		যদি(config->bridges[i]->tls_psk_identity && !config->bridges[i]->tls_psk){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_psk.");
			প্রত্যাগম MOSQ_ERR_INVAL;
		}
#endif
	}
#endif

	যদি(cr.log_dest_set){
		config->log_dest = cr.log_dest;
	}
	যদি(db.quiet){
		config->log_type = 0;
	}অন্যথা যদি(db.verbose){
		config->log_type = UINT_MAX;
	}অন্যথা যদি(cr.log_type_set){
		config->log_type = cr.log_type;
	}
#ifdef WITH_BRIDGE
	প্রত্যাগম config__check_bridges(config);
#অন্যথা
	প্রত্যাগম MOSQ_ERR_SUCCESS;
#endif
}


স্থির mosquitto_plugin_id_t *config__plugin_find(ধ্রুব বর্ণ *name)
{
	যদি(db.plugins && name){
		প্রতি(পূর্ণাঙ্ক i=0; i<db.plugin_গুনতি; i++){
			যদি(db.plugins[i]->config.name && !strcmp(db.plugins[i]->config.name, name)){
				প্রত্যাগম db.plugins[i];
			}
		}
	}
	প্রত্যাগম রিক্ত;
}


স্থির mosquitto_plugin_id_t *config__plugin_load(ধ্রুব বর্ণ *name, ধ্রুব বর্ণ *path)
{
	mosquitto_plugin_id_t *plugin = রিক্ত;
	mosquitto_plugin_id_t **plugins = রিক্ত;

	যদি(name && config__plugin_find(name)){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Duplicate plugin name '%s'.", name);
		প্রত্যাগম রিক্ত;
	}

	যদি(!path || !strcmp(path, "")){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Missing plugin path for plugin name '%s'.", name);
		প্রত্যাগম রিক্ত;
	}

	plugin = mosquitto_calloc(1, পরিমাণ(mosquitto_plugin_id_t));
	যদি(!plugin){
		গচ্ছ error;
	}

	যদি(name){
		plugin->config.name = mosquitto_strdup(name);
	}
	plugin->config.path = mosquitto_strdup(path);
	যদি((name && !plugin->config.name) || !plugin->config.path){
		গচ্ছ error;
	}
	plugin->config.options = রিক্ত;
	plugin->config.option_গুনতি = 0;
	plugin->config.deny_special_chars = true;

	/* Add to db list */
	plugins = mosquitto_realloc(db.plugins, (size_t)(db.plugin_গুনতি+1)*পরিমাণ(সংরচনা mosquitto__plugin_config *));
	যদি(!plugins){
		গচ্ছ error;
	}

	plugins[db.plugin_গুনতি] = plugin;
	db.plugins = plugins;
	db.plugin_গুনতি++;

	প্রত্যাগম plugin;
error:
	log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
	যদি(plugin){
		mosquitto_FREE(plugin->config.name);
		mosquitto_FREE(plugin->config.path);
	}
	mosquitto_FREE(plugin);
	প্রত্যাগম রিক্ত;
}


পূর্ণাঙ্ক config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, সংরচনা mosquitto__security_options *security_options)
{
	সংরচনা mosquitto__security_options **new_options;
	mosquitto_plugin_id_t **new_plugins;

	new_options = mosquitto_realloc(plugin->config.security_options, (size_t)(plugin->config.security_option_গুনতি+1)*পরিমাণ(সংরচনা mosquitto__security_options *));
	new_plugins = mosquitto_realloc(security_options->plugins, (size_t)(security_options->plugin_গুনতি+1)*পরিমাণ(mosquitto_plugin_id_t *));

	যদি(!new_options || !new_plugins){
		mosquitto_FREE(new_options);
		mosquitto_FREE(new_plugins);
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
		প্রত্যাগম MOSQ_ERR_NOMEM;
	}

	new_options[plugin->config.security_option_গুনতি] = security_options;
	plugin->config.security_options = new_options;
	plugin->config.security_option_গুনতি++;

	new_plugins[security_options->plugin_গুনতি] = plugin;
	security_options->plugins = new_plugins;
	security_options->plugin_গুনতি++;

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণাঙ্ক config__read_file_core(সংরচনা mosquitto__config *config, bool reload, সংরচনা config_recurse *cr, পূর্ণাঙ্ক level, পূর্ণাঙ্ক *lineno, FILE *fptr, বর্ণ **buf, পূর্ণাঙ্ক *buflen)
{
	পূর্ণাঙ্ক rc;
	বর্ণ *token;
	পূর্ণাঙ্ক tmp_int;
	বর্ণ *saveptr = রিক্ত;
#ifdef WITH_BRIDGE
	বর্ণ *tmp_char;
	সংরচনা mosquitto__bridge *cur_bridge = রিক্ত;
#endif
	mosquitto_plugin_id_t *cur_plugin = রিক্ত;

	বর্ণ *চাবি;
	সংরচনা mosquitto__listener *cur_listener = রিক্ত;
	পূর্ণাঙ্ক i;
	পূর্ণাঙ্ক lineno_ext = 0;
	size_t prefix_len;
	size_t slen;
#ifdef WITH_WEBSOCKETS
	বর্ণ **ws_origins = রিক্ত;
#endif

	*lineno = 0;

	যাবত্(mosquitto_fgets(buf, buflen, fptr)){
		(*lineno)++;
		যদি((*buf)[0] != '#' && (*buf)[0] != 10 && (*buf)[0] != 13){
			slen = strlen(*buf);
			যদি(slen == 0){
				অনুবর্ত;
			}
			যাবত্((*buf)[slen-1] == 10 || (*buf)[slen-1] == 13){
				(*buf)[slen-1] = 0;
				slen = strlen(*buf);
				যদি(slen == 0){
					অনুবর্ত;
				}
			}
			token = strtok_r((*buf), " ", &saveptr);
			যদি(token){
				যদি(!strcmp(token, "accept_protocol_versions")){
					REQUIRE_NON_DEFAULT_LISTENER(token);
					cur_listener->disable_protocol_v3 = true;
					cur_listener->disable_protocol_v4 = true;
					cur_listener->disable_protocol_v5 = true;
					যদি(saveptr == রিক্ত){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", "accept_protocol_versions");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					token = strtok_r(saveptr, ", \t", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "accept_protocol_versions");

					যাবত্(token){
						যদি(!strcmp(token, "3")){
							cur_listener->disable_protocol_v3 = false;
						}অন্যথা যদি(!strcmp(token, "4")){
							cur_listener->disable_protocol_v4 = false;
						}অন্যথা যদি(!strcmp(token, "5")){
							cur_listener->disable_protocol_v5 = false;
						}

						token = strtok_r(রিক্ত, ", \t", &saveptr);
					}
				}অন্যথা যদি(!strcmp(token, "acl_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);

					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->acl_data.acl_file);
					যদি(conf__parse_string(&token, "acl_file", &cur_security_options->acl_data.acl_file, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "address") || !strcmp(token, "addresses")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					যদি(cur_bridge->addresses){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid bridge configuration, 'address' only allowed once.");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যাবত্((token = strtok_r(রিক্ত, " ", &saveptr))){
						যদি(token[0] == '#'){
							ভঙ্গ;
						}
						সংরচনা bridge_address *new_addresses = mosquitto_realloc(cur_bridge->addresses, পরিমাণ(সংরচনা bridge_address)*(size_t)(cur_bridge->address_গুনতি+1));
						যদি(!new_addresses){
							log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
							প্রত্যাগম MOSQ_ERR_NOMEM;
						}
						cur_bridge->address_গুনতি++;
						cur_bridge->addresses = new_addresses;
						memset(&cur_bridge->addresses[cur_bridge->address_গুনতি-1], 0, পরিমাণ(সংরচনা bridge_address));
						cur_bridge->addresses[cur_bridge->address_গুনতি-1].address = mosquitto_strdup(token);
						যদি(!cur_bridge->addresses[cur_bridge->address_গুনতি-1].address){
							log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
							প্রত্যাগম MOSQ_ERR_NOMEM;
						}
					}
					প্রতি(i=0; i<cur_bridge->address_গুনতি; i++){
						/* cur_bridge->addresses[i].address is now
						 * "address[:port]". If address is an IPv6 address,
						 * then port is required. We must check for the :
						 * backwards. */
						tmp_char = strrchr(cur_bridge->addresses[i].address, ':');
						যদি(tmp_char){
							/* Remove ':', so cur_bridge->addresses[i].address
							 * now just looks like the address. */
							tmp_char[0] = '\0';

							/* The remainder of the string */
							tmp_int = atoi(&tmp_char[1]);
							যদি(tmp_int < 1 || tmp_int > UINT16_MAX){
								log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid bridge port value (%d).", tmp_int);
								প্রত্যাগম MOSQ_ERR_INVAL;
							}
							cur_bridge->addresses[i].port = (uint16_t)tmp_int;
						}অন্যথা{
							cur_bridge->addresses[i].port = 1883;
						}
						conf__attempt_resolve(cur_bridge->addresses[i].address, "bridge address", MOSQ_LOG_WARNING, "Warning");
					}
					যদি(cur_bridge->address_গুনতি == 0){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Empty address value in configuration.");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "allow_anonymous")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					PER_LISTENER_ALTERNATIVE(token, "listener_allow_anonymous");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					যদি(conf__parse_bool(&token, "allow_anonymous", (bool *)&cur_security_options->allow_anonymous, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "allow_duplicate_messages")){
					OPTION_DEPRECATED(token, "The behaviour will default to true.");
					যদি(conf__parse_bool(&token, "allow_duplicate_messages", &config->allow_duplicate_messages, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "allow_zero_length_clientid")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					যদি(conf__parse_bool(&token, "allow_zero_length_clientid", &cur_security_options->allow_zero_length_clientid, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strncmp(token, "auth_opt_", strlen("auth_opt_")) || !strncmp(token, "plugin_opt_", strlen("plugin_opt_"))){
					যদি(reload){
						অনুবর্ত;        /* Auth plugin not currently valid for reloading. */

					}
					REQUIRE_PLUGIN(token);

					যদি(!strncmp(token, "auth_opt_", strlen("auth_opt_"))){
						prefix_len = strlen("auth_opt_");
					}অন্যথা{
						prefix_len = strlen("plugin_opt_");
					}
					যদি(strlen(token) < prefix_len + 3){
						/* auth_opt_ == 9, + one digit key == 10, + one space == 11, + one value == 12 */
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid 'plugin_opt_' config option.");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					চাবি = mosquitto_strdup(&token[prefix_len]);
					যদি(!চাবি){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
						প্রত্যাগম MOSQ_ERR_NOMEM;
					}অন্যথা যদি(STREMPTY(চাবি)){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Empty 'plugin_opt_' config option.");
						mosquitto_FREE(চাবি);
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					token = saveptr;
					যদি(token && token[0]){
						যাবত্(token[0] == ' ' || token[0] == '\t'){
							token++;
						}
						সংরচনা mosquitto_opt *new_options = mosquitto_realloc(cur_plugin->config.options, (size_t)(cur_plugin->config.option_গুনতি+1)*পরিমাণ(সংরচনা mosquitto_opt));
						যদি(!new_options){
							log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
							mosquitto_FREE(চাবি);
							প্রত্যাগম MOSQ_ERR_NOMEM;
						}
						cur_plugin->config.option_গুনতি++;
						cur_plugin->config.options = new_options;
						cur_plugin->config.options[cur_plugin->config.option_গুনতি-1].চাবি = চাবি;
						cur_plugin->config.options[cur_plugin->config.option_গুনতি-1].মান = mosquitto_strdup(token);
						যদি(!cur_plugin->config.options[cur_plugin->config.option_গুনতি-1].মান){
							log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
							প্রত্যাগম MOSQ_ERR_NOMEM;
						}
					}অন্যথা{
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", চাবি);
						mosquitto_FREE(চাবি);
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "auth_plugin") || !strcmp(token, "plugin") || !strcmp(token, "global_plugin")){
					যদি(reload){
						অনুবর্ত;        /* plugin not currently valid for reloading. */
					}
					যদি(!strcmp(token, "global_plugin")){
						cur_security_options = &db.config->security_options;
					}অন্যথা{
						REQUIRE_LISTENER_IF_PER_LISTENER(token);
						conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					}

					cur_plugin = config__plugin_load(রিক্ত, saveptr);
					যদি(cur_plugin == রিক্ত){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(config__plugin_add_secopt(cur_plugin, cur_security_options)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "auth_plugin_deny_special_chars")){
					যদি(reload){
						অনুবর্ত;        /* Auth plugin not currently valid for reloading. */
					}
					যদি(!cur_plugin){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: An auth_plugin_deny_special_chars option exists in the config file without a plugin.");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(conf__parse_bool(&token, "auth_plugin_deny_special_chars", &cur_plugin->config.deny_special_chars, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "plugin_load")){
					বর্ণ *name = রিক্ত;

					যদি(reload){
						অনুবর্ত;        /* plugin not currently valid for reloading. */

					}
					name = strtok_r(রিক্ত, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(name, "plugin_load");

					cur_plugin = config__plugin_load(name, saveptr);
					যদি(cur_plugin == রিক্ত){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "plugin_use")){
					বর্ণ *name = রিক্ত;

					যদি(reload){
						অনুবর্ত;        /* plugin not currently valid for reloading. */
					}
					REQUIRE_NON_DEFAULT_LISTENER(token);

					যদি(conf__parse_string(&token, "plugin_use", &name, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					cur_plugin = config__plugin_find(name);
					যদি(!cur_plugin){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Plugin '%s' not previously loaded.", name);
						mosquitto_FREE(name);
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(name);
					যদি(config__plugin_add_secopt(cur_plugin, cur_listener->security_options)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "auto_id_prefix")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					OPTION_DEPRECATED(token, "Please use 'listener_auto_id_prefix' instead.");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					যদি(conf__parse_string(&token, "auto_id_prefix", &cur_security_options->auto_id_prefix, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(cur_security_options->auto_id_prefix){
						যদি(strlen(cur_security_options->auto_id_prefix) > 50){
							log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: auto_id_prefix length must be <= 50.");
							প্রত্যাগম MOSQ_ERR_INVAL;
						}
						cur_security_options->auto_id_prefix_len = (uint16_t)strlen(cur_security_options->auto_id_prefix);
					}অন্যথা{
						cur_security_options->auto_id_prefix_len = 0;
					}
				}অন্যথা যদি(!strcmp(token, "autosave_interval")){
					যদি(conf__parse_int(&token, "autosave_interval", &config->autosave_interval, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(config->autosave_interval < 0){
						config->autosave_interval = 0;
					}
				}অন্যথা যদি(!strcmp(token, "autosave_on_changes")){
					যদি(conf__parse_bool(&token, "autosave_on_changes", &config->autosave_on_changes, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "bind_address")){
					OPTION_DEPRECATED(token, "");
					config->local_only = false;
					যদি(reload){
						অনুবর্ত;        /* Rebinding listeners not valid during reloading. */

					}
					যদি(config__create_default_listener(config, token)){
						প্রত্যাগম MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					যদি(conf__parse_string(&token, "default listener bind_address", &config->default_listener->host, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(conf__attempt_resolve(config->default_listener->host, "bind_address", MOSQ_LOG_ERR, "Error")){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "bind_interface")){
#ifdef SO_BINDTODEVICE
					যদি(reload){
						অনুবর্ত;        /* Rebinding listeners not valid during reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					যদি(conf__parse_string(&token, "bind_interface", &cur_listener->bind_interface, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: bind_interface specified but socket option not available.");
					প্রত্যাগম MOSQ_ERR_INVAL;
#endif
				}অন্যথা যদি(!strcmp(token, "bridge_attempt_unsubscribe")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_bool(&token, "bridge_attempt_unsubscribe", &cur_bridge->attempt_unsubscribe, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "bridge_cafile")){
#যদি defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					যদি(conf__parse_string(&token, "bridge_cafile", &cur_bridge->tls_cafile, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "bridge_alpn")){
#যদি defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_string(&token, "bridge_alpn", &cur_bridge->tls_alpn, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "bridge_ciphers")){
#যদি defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_string(&token, "bridge_ciphers", &cur_bridge->tls_ciphers, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "bridge_ciphers_tls1.3")){
#যদি defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_string(&token, "bridge_ciphers_tls1.3", &cur_bridge->tls_13_ciphers, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "bridge_bind_address")){
#যদি defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_string(&token, "bridge_bind_address", &cur_bridge->bind_address, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "bridge_capath")){
#যদি defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					যদি(conf__parse_string(&token, "bridge_capath", &cur_bridge->tls_capath, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "bridge_certfile")){
#যদি defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					যদি(conf__parse_string(&token, "bridge_certfile", &cur_bridge->tls_certfile, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "bridge_identity")){
#যদি defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					যদি(conf__parse_string(&token, "bridge_identity", &cur_bridge->tls_psk_identity, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "bridge_insecure")){
#যদি defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_bool(&token, "bridge_insecure", &cur_bridge->tls_insecure, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(cur_bridge->tls_insecure){
						log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge '%s' using insecure mode.", cur_bridge->name);
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "bridge_require_ocsp")){
#যদি defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_bool(&token, "bridge_require_ocsp", &cur_bridge->tls_ocsp_required, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "bridge_max_packet_size")){
#যদি defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_int(&token, "bridge_max_packet_size", &tmp_int, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(tmp_int < 0){
						tmp_int = 0;
					}
					cur_bridge->maximum_packet_size = (uint32_t)tmp_int;
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "bridge_max_topic_alias")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_int(&token, "bridge_max_topic_alias", &tmp_int, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}

					যদি(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: bridge_max_topic_alias must be > 0 and <= 65535.");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					cur_bridge->max_topic_alias = (uint16_t)tmp_int;
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "bridge_outgoing_retain")){
#যদি defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_bool(&token, "bridge_outgoing_retain", &cur_bridge->outgoing_retain, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "bridge_keyfile")){
#যদি defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					mosquitto_FREE(cur_bridge->tls_keyfile);
					যদি(conf__parse_string(&token, "bridge_keyfile", &cur_bridge->tls_keyfile, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "bridge_protocol_version")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(রিক্ত, "", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_protocol_version");

					যদি(!strcmp(token, "mqttv31")){
						cur_bridge->protocol_version = mosq_p_mqtt31;
					}অন্যথা যদি(!strcmp(token, "mqttv311")){
						cur_bridge->protocol_version = mosq_p_mqtt311;
					}অন্যথা যদি(!strcmp(token, "mqttv50")){
						cur_bridge->protocol_version = mosq_p_mqtt5;
					}অন্যথা{
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid 'bridge_protocol_version' value (%s).", token);
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "bridge_psk")){
#যদি defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					যদি(conf__parse_string(&token, "bridge_psk", &cur_bridge->tls_psk, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "bridge_receive_maximum")){
#যদি defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_int(&token, "bridge_receive_maximum", &tmp_int, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(tmp_int <= 0){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be greater than 0.");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}অন্যথা যদি((uint64_t)tmp_int > (uint64_t)UINT16_MAX){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be lower than %u.", UINT16_MAX);
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					cur_bridge->receive_maximum = (uint16_t)tmp_int;
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "bridge_reload_type")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(রিক্ত, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_reload_type");

					যদি(!strcmp(token, "lazy")){
						cur_bridge->reload_type = brt_lazy;
					}অন্যথা যদি(!strcmp(token, "immediate")){
						cur_bridge->reload_type = brt_immediate;
					}অন্যথা{
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid 'bridge_reload_type' value in configuration (%s).", token);
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "bridge_session_expiry_interval")){
#যদি defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_int(&token, "bridge_session_expiry_interval", &tmp_int, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(tmp_int < 0){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must not be negative.");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}অন্যথা যদি((uint64_t)tmp_int > (uint64_t)MQTT_SESSION_EXPIRY_NEVER){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must be lower than %u.", MQTT_SESSION_EXPIRY_NEVER);
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					cur_bridge->session_expiry_interval = (uint32_t)tmp_int;
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "bridge_tcp_keepalive")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);

					যদি(conf__parse_int(&token, "bridge_tcp_keepalive_idle", &tmp_int, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(tmp_int <= 0){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: invalid TCP keepalive idle value.");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_idle = (unsigned পূর্ণাঙ্ক)tmp_int;

					যদি(conf__parse_int(&token, "bridge_tcp_keepalive_interval", &tmp_int, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(tmp_int <= 0){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: invalid TCP keepalive interval value.");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_interval = (unsigned পূর্ণাঙ্ক)tmp_int;

					যদি(conf__parse_int(&token, "bridge_tcp_keepalive_counter", &tmp_int, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(tmp_int <= 0){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: invalid TCP keepalive counter value.");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_counter = (unsigned পূর্ণাঙ্ক)tmp_int;
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "bridge_tcp_user_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
#ifdef WITH_TCP_USER_TIMEOUT
					যদি(conf__parse_int(&token, "bridge_tcp_user_timeout", &tmp_int, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(tmp_int < 0){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: invalid TCP user timeout value.");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_user_timeout = tmp_int;
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge TCP user timeout support not available.");
#endif
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "bridge_tls_use_os_certs")){
#যদি defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_bool(&token, "bridge_tls_use_os_certs", &cur_bridge->tls_use_os_certs, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "bridge_tls_version")){
#যদি defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_string(&token, "bridge_tls_version", &cur_bridge->tls_version, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "cafile")){
#যদি defined(WITH_TLS)
					REQUIRE_LISTENER(token);
					যদি(cur_listener->psk_hint){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->cafile);
					যদি(conf__parse_string(&token, "cafile", &cur_listener->cafile, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "capath")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->capath);
					যদি(conf__parse_string(&token, "capath", &cur_listener->capath, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "certfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					যদি(cur_listener->psk_hint){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->certfile);
					যদি(conf__parse_string(&token, "certfile", &cur_listener->certfile, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "check_retain_source")){
					যদি(conf__parse_bool(&token, "check_retain_source", &config->check_retain_source, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "ciphers")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers);
					যদি(conf__parse_string(&token, "ciphers", &cur_listener->ciphers, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "ciphers_tls1.3")){
#যদি defined(WITH_TLS) && (!defined(LIBRESSL_VERSION_সংখ্যা) || LIBRESSL_VERSION_সংখ্যা > 0x3040000FL)
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers_tls13);
					যদি(conf__parse_string(&token, "ciphers_tls1.3", &cur_listener->ciphers_tls13, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: ciphers_tls1.3 support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "clientid") || !strcmp(token, "remote_clientid")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_string(&token, "bridge remote clientid", &cur_bridge->remote_clientid, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "cleansession")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_bool(&token, "cleansession", &cur_bridge->clean_start, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "local_cleansession")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_bool(&token, "local_cleansession", (bool *)&cur_bridge->clean_start_local, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "clientid_prefixes")){
					OPTION_DEPRECATED(token, "");
					mosquitto_FREE(config->clientid_prefixes);
					যদি(conf__parse_string(&token, "clientid_prefixes", &config->clientid_prefixes, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "connection")){
#ifdef WITH_BRIDGE
					token = strtok_r(রিক্ত, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "connection");

					/* Check for existing bridge name. */
					প্রতি(i=0; i<config->bridge_গুনতি; i++){
						যদি(!strcmp(config->bridges[i]->name, token)){
							log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Duplicate bridge name '%s'.", token);
							প্রত্যাগম MOSQ_ERR_INVAL;
						}
					}

					সংরচনা mosquitto__bridge **bridges_new = mosquitto_realloc(config->bridges, (size_t)(config->bridge_গুনতি+1)*পরিমাণ(সংরচনা mosquitto__bridge *));
					যদি(!bridges_new){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
						প্রত্যাগম MOSQ_ERR_NOMEM;
					}
					config->bridges = bridges_new;
					cur_bridge = mosquitto_malloc(পরিমাণ(সংরচনা mosquitto__bridge));
					যদি(!cur_bridge){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
						প্রত্যাগম MOSQ_ERR_NOMEM;
					}

					config->bridge_গুনতি++;
					config->bridges[config->bridge_গুনতি-1] = cur_bridge;

					memset(cur_bridge, 0, পরিমাণ(সংরচনা mosquitto__bridge));
					cur_bridge->name = mosquitto_strdup(token);
					যদি(!cur_bridge->name){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
						প্রত্যাগম MOSQ_ERR_NOMEM;
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
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "connection_messages")){
					যদি(conf__parse_bool(&token, token, &config->connection_messages, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "crlfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->crlfile);
					যদি(conf__parse_string(&token, "crlfile", &cur_listener->crlfile, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "dhparamfile")){
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: dhparamfile is no longer required.");
				}অন্যথা যদি(!strcmp(token, "disable_client_cert_date_checks")){
#ifdef WITH_TLS
					REQUIRE_LISTENER(token);
					যদি(conf__parse_bool(&token, "disable_client_cert_date_checks", &cur_listener->disable_client_cert_date_checks, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "enable_control_api")){
#ifdef WITH_CONTROL
					যদি(conf__parse_bool(&token, "enable_control_api", &config->enable_control_api, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: $CONTROL support not available (enable_control_api).");
#endif
				}অন্যথা যদি(!strcmp(token, "enable_proxy_protocol")){
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					প্রত্যাগম MOSQ_ERR_INVAL;
#endif
					REQUIRE_LISTENER(token);
					যদি(conf__parse_int(&token, "enable_proxy_protocol", &cur_listener->enable_proxy_protocol, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(cur_listener->enable_proxy_protocol < 1 || cur_listener->enable_proxy_protocol > 2){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: enable_proxy_protocol must be 1 or 2.");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "global_max_clients")){
					যদি(conf__parse_int(&token, "global_max_clients", &config->global_max_clients, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "global_max_connections")){
					যদি(conf__parse_int(&token, "global_max_connections", &config->global_max_connections, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "http_dir")){
#যদি defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
					যদি(reload){
						অনুবর্ত;        /* Not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					যদি(conf__parse_string(&token, "http_dir", &cur_listener->http_dir, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#ifdef WIN32
					বর্ণ *http_dir_canonical = _fullpath(রিক্ত, cur_listener->http_dir, 0);
					ধ্রুব বর্ণ sep = '\\';
#অন্যথা
					বর্ণ *http_dir_canonical = realpath(cur_listener->http_dir, রিক্ত);
					ধ্রুব বর্ণ sep = '/';
#endif
					যদি(!http_dir_canonical){
						প্রত্যাগম MOSQ_ERR_NOMEM;
					}
					size_t http_dir_len = strlen(http_dir_canonical) + পরিমাণ(sep) + 1;
					বর্ণ *http_dir_canonical_sep = mosquitto_calloc(http_dir_len, পরিমাণ(বর্ণ));
					যদি(!http_dir_canonical_sep){
						মুক্ত(http_dir_canonical);
						প্রত্যাগম MOSQ_ERR_NOMEM;
					}
					snprintf(http_dir_canonical_sep, http_dir_len, "%s%c", http_dir_canonical, sep);
					মুক্ত(http_dir_canonical);
					mosquitto_FREE(cur_listener->http_dir);
					cur_listener->http_dir = http_dir_canonical_sep;
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: http_dir support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "idle_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_int(&token, "idle_timeout", &cur_bridge->idle_timeout, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(cur_bridge->idle_timeout < 1){
						log__printf(রিক্ত, MOSQ_LOG_NOTICE, "idle_timeout interval too low, using 1 second.");
						cur_bridge->idle_timeout = 1;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "include_dir")){
					যদি(level == 0){
						বর্ণ **files;
						পূর্ণাঙ্ক file_গুনতি;

						/* Only process include_dir from the main config file. */
						token = strtok_r(রিক্ত, "", &saveptr);
						REQUIRE_NON_EMPTY_OPTION(token, "include_dir");

						rc = config__get_dir_files(token, &files, &file_গুনতি);
						যদি(rc){
							প্রত্যাগম rc;
						}

						প্রতি(i=0; i<file_গুনতি; i++){
							log__printf(রিক্ত, MOSQ_LOG_INFO, "Loading config file '%s'", files[i]);

							rc = config__read_file(config, reload, files[i], cr, level+1, &lineno_ext);
							যদি(rc){
								যদি(lineno_ext > 0){
									log__printf(রিক্ত, MOSQ_LOG_ERR, "Error found at '%s:%d'.", files[i], lineno_ext);
								}
								/* Free happens below */
								ভঙ্গ;
							}
						}
						প্রতি(i=0; i<file_গুনতি; i++){
							mosquitto_FREE(files[i]);
						}
						mosquitto_FREE(files);
						যদি(rc){
							প্রত্যাগম rc;    /* This returns if config__read_file() fails above */
						}
					}অন্যথা{
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: The include_dir option is only valid in the main configuration file.");
						প্রত্যাগম 1;
					}
				}অন্যথা যদি(!strcmp(token, "keepalive_interval")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_int(&token, "keepalive_interval", &tmp_int, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(tmp_int > UINT16_MAX){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Bridge keepalive value too high.");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(tmp_int < 5){
						log__printf(রিক্ত, MOSQ_LOG_NOTICE, "keepalive interval too low, using 5 seconds.");
						tmp_int = 5;
					}
					cur_bridge->keepalive = (uint16_t)tmp_int;
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "keyfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->keyfile);
					যদি(conf__parse_string(&token, "keyfile", &cur_listener->keyfile, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "listener")){
					config->local_only = false;

					যদি(conf__parse_int(&token, "listener port", &tmp_int, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#ifdef WITH_UNIX_SOCKETS
					যদি(tmp_int < 0 || tmp_int > UINT16_MAX){
#অন্যথা
					যদি(tmp_int < 1 || tmp_int > UINT16_MAX){
#endif
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						প্রত্যাগম MOSQ_ERR_INVAL;
					}

					/* Look for bind address / unix socket path */
					token = strtok_r(রিক্ত, " ", &saveptr);
					যদি(token != রিক্ত && token[0] == '#'){
						token = রিক্ত;
					}

					যদি(tmp_int == 0 && token == রিক্ত){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: A listener with port 0 must provide a Unix socket path.");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}

					যদি(reload){
						/* We reload listeners settings based on port number/unix socket path.
						 * If the port number/unix path doesn't already exist, exit with a complaint. */
						cur_listener = রিক্ত;
#ifdef WITH_UNIX_SOCKETS
						যদি(tmp_int == 0){
							প্রতি(i=0; i<config->listener_গুনতি; i++){
								যদি(config->listeners[i].unix_socket_path != রিক্ত
										&& strcmp(config->listeners[i].unix_socket_path, token) == 0){

									cur_listener = &config->listeners[i];
									ভঙ্গ;
								}
							}
						}অন্যথা
#endif
						{
							প্রতি(i=0; i<config->listener_গুনতি; i++){
								যদি(config->listeners[i].port == tmp_int){
									/* Now check we have a matching bind address, if defined */
									যদি(config->listeners[i].host){
										যদি(token && !strcmp(config->listeners[i].host, token)){
											/* They both have a bind address, and they match */
											cur_listener = &config->listeners[i];
											ভঙ্গ;
										}
									}অন্যথা{
										যদি(token == রিক্ত){
											/* Neither this config nor the new config have a bind address,
											 * so they match. */
											cur_listener = &config->listeners[i];
											ভঙ্গ;
										}
									}
								}
							}
						}
						যদি(!cur_listener){
							log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: It is not currently possible to add/remove listeners when reloading the config file.");
							প্রত্যাগম MOSQ_ERR_INVAL;
						}
					}অন্যথা{
						যদি(config__add_listener(config)){
							log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
							প্রত্যাগম MOSQ_ERR_NOMEM;
						}
						cur_listener = &config->listeners[config->listener_গুনতি-1];
					}

					listener__set_defaults(cur_listener);
					cur_listener->port = (uint16_t)tmp_int;

					mosquitto_FREE(cur_listener->host);

#ifdef WITH_UNIX_SOCKETS
					mosquitto_FREE(cur_listener->unix_socket_path);
#endif

					যদি(token){
#ifdef WITH_UNIX_SOCKETS
						যদি(cur_listener->port == 0){
							cur_listener->unix_socket_path = mosquitto_strdup(token);
						}অন্যথা
#endif
						{
							cur_listener->host = mosquitto_strdup(token);
						}
					}
				}অন্যথা যদি(!strcmp(token, "listener_allow_anonymous")){
					REQUIRE_LISTENER(token);
					যদি(conf__parse_bool(&token, "listener_allow_anonymous", (bool *)&cur_listener->security_options->allow_anonymous, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "listener_auto_id_prefix")){
					REQUIRE_LISTENER(token);
					যদি(conf__parse_string(&token, "listener_auto_id_prefix", &cur_listener->security_options->auto_id_prefix, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(cur_listener->security_options->auto_id_prefix){
						যদি(strlen(cur_listener->security_options->auto_id_prefix) > 50){
							log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: 'listener_auto_id_prefix' length must be <= 50.");
							প্রত্যাগম MOSQ_ERR_INVAL;
						}
						cur_listener->security_options->auto_id_prefix_len = (uint16_t)strlen(cur_listener->security_options->auto_id_prefix);
					}অন্যথা{
						cur_listener->security_options->auto_id_prefix_len = 0;
					}
				}অন্যথা যদি(!strcmp(token, "local_clientid")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_string(&token, "bridge local clientd", &cur_bridge->local_clientid, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "local_password")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_string(&token, "bridge local_password", &cur_bridge->local_password, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "local_username")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_string(&token, "bridge local_username", &cur_bridge->local_username, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "log_dest")){
					token = strtok_r(রিক্ত, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_dest");

					cr->log_dest_set = 1;
					যদি(!strcmp(token, "none")){
						cr->log_dest = MQTT3_LOG_NONE;
					}অন্যথা যদি(!strcmp(token, "syslog")){
						cr->log_dest |= MQTT3_LOG_SYSLOG;
					}অন্যথা যদি(!strcmp(token, "stdout")){
						cr->log_dest |= MQTT3_LOG_STDOUT;
					}অন্যথা যদি(!strcmp(token, "stderr")){
						cr->log_dest |= MQTT3_LOG_STDERR;
					}অন্যথা যদি(!strcmp(token, "topic")){
						cr->log_dest |= MQTT3_LOG_TOPIC;
					}অন্যথা যদি(!strcmp(token, "dlt")){
						cr->log_dest |= MQTT3_LOG_DLT;
#ifdef ANDROID
					}অন্যথা যদি(!strcmp(token, "android")){
						cr->log_dest |= MQTT3_LOG_ANDROID;
#endif
					}অন্যথা যদি(!strcmp(token, "file")){
						যদি(config->log_fptr || config->log_file){
							log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Duplicate \"log_dest file\" value.");
							প্রত্যাগম MOSQ_ERR_INVAL;
						}
						/* Get remaining string. */
						token = saveptr;
						যদি(token && token[0]){
							যাবত্(token[0] == ' ' || token[0] == '\t'){
								token++;
							}
						}
						/* Duplicate "token" check here saves a log__printf() */
						যদি(token && token[0]){
							config->log_file = mosquitto_strdup(token);
							যদি(!config->log_file){
								log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
								প্রত্যাগম MOSQ_ERR_NOMEM;
							}
							cr->log_dest |= MQTT3_LOG_FILE;
						}অন্যথা{
							log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Empty \"log_dest file\" value in configuration.");
							প্রত্যাগম MOSQ_ERR_INVAL;
						}
						cr->log_dest |= MQTT3_LOG_FILE;
					}অন্যথা{
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid 'log_dest' value (%s).", token);
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#যদি defined(WIN32) || defined(__CYGWIN__)
					যদি(service_handle){
						যদি(cr->log_dest == MQTT3_LOG_STDOUT || cr->log_dest == MQTT3_LOG_STDERR){
							log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Cannot log to stdout/stderr when running as a Windows service.");
							প্রত্যাগম MOSQ_ERR_INVAL;
						}
					}
#endif
				}অন্যথা যদি(!strcmp(token, "log_facility")){
#যদি defined(WIN32) || defined(__CYGWIN__)
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: log_facility not supported on Windows.");
#অন্যথা
					যদি(conf__parse_int(&token, "log_facility", &tmp_int, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					বিকল্প(tmp_int){
						পক্ষ 0:
							config->log_facility = LOG_LOCAL0;
							ভঙ্গ;
						পক্ষ 1:
							config->log_facility = LOG_LOCAL1;
							ভঙ্গ;
						পক্ষ 2:
							config->log_facility = LOG_LOCAL2;
							ভঙ্গ;
						পক্ষ 3:
							config->log_facility = LOG_LOCAL3;
							ভঙ্গ;
						পক্ষ 4:
							config->log_facility = LOG_LOCAL4;
							ভঙ্গ;
						পক্ষ 5:
							config->log_facility = LOG_LOCAL5;
							ভঙ্গ;
						পক্ষ 6:
							config->log_facility = LOG_LOCAL6;
							ভঙ্গ;
						পক্ষ 7:
							config->log_facility = LOG_LOCAL7;
							ভঙ্গ;
						স্বভাব:
							log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid 'log_facility' value (%d).", tmp_int);
							প্রত্যাগম MOSQ_ERR_INVAL;
					}
#endif
				}অন্যথা যদি(!strcmp(token, "log_timestamp")){
					যদি(conf__parse_bool(&token, token, &config->log_timestamp, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "log_timestamp_format")){
					যদি(conf__parse_string(&token, token, &config->log_timestamp_format, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "log_type")){
					token = strtok_r(রিক্ত, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_type");

					cr->log_type_set = 1;
					যদি(!strcmp(token, "none")){
						cr->log_type = MOSQ_LOG_NONE;
					}অন্যথা যদি(!strcmp(token, "information")){
						cr->log_type |= MOSQ_LOG_INFO;
					}অন্যথা যদি(!strcmp(token, "notice")){
						cr->log_type |= MOSQ_LOG_NOTICE;
					}অন্যথা যদি(!strcmp(token, "warning")){
						cr->log_type |= MOSQ_LOG_WARNING;
					}অন্যথা যদি(!strcmp(token, "error")){
						cr->log_type |= MOSQ_LOG_ERR;
					}অন্যথা যদি(!strcmp(token, "debug")){
						cr->log_type |= MOSQ_LOG_DEBUG;
					}অন্যথা যদি(!strcmp(token, "subscribe")){
						cr->log_type |= MOSQ_LOG_SUBSCRIBE;
					}অন্যথা যদি(!strcmp(token, "unsubscribe")){
						cr->log_type |= MOSQ_LOG_UNSUBSCRIBE;
					}অন্যথা যদি(!strcmp(token, "internal")){
						cr->log_type |= MOSQ_LOG_INTERNAL;
#ifdef WITH_WEBSOCKETS
					}অন্যথা যদি(!strcmp(token, "websockets")){
						cr->log_type |= MOSQ_LOG_WEBSOCKETS;
#endif
					}অন্যথা যদি(!strcmp(token, "all")){
						cr->log_type = MOSQ_LOG_ALL;
					}অন্যথা{
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid 'log_type' value (%s).", token);
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "max_connections")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					যদি(conf__parse_int(&token, token, &cur_listener->max_connections, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(cur_listener->max_connections < 0){
						cur_listener->max_connections = -1;
					}
				}অন্যথা যদি(!strcmp(token, "maximum_qos") || !strcmp(token, "max_qos")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					যদি(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(tmp_int < 0 || tmp_int > 2){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: 'max_qos' must be between 0 and 2 inclusive.");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					cur_listener->max_qos = (uint8_t)tmp_int;
				}অন্যথা যদি(!strcmp(token, "max_inflight_bytes")){
					যদি(conf__parse_int(&token, "max_inflight_bytes", &tmp_int, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_inflight_bytes = (size_t)tmp_int;
				}অন্যথা যদি(!strcmp(token, "max_inflight_messages")){
					যদি(conf__parse_int(&token, "max_inflight_messages", &tmp_int, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(tmp_int < 0 || tmp_int == UINT16_MAX){
						tmp_int = 0;
					}অন্যথা যদি(tmp_int > UINT16_MAX){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: 'max_inflight_messages' must be <= 65535.");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					config->max_inflight_messages = (uint16_t)tmp_int;
				}অন্যথা যদি(!strcmp(token, "max_keepalive")){
					যদি(conf__parse_int(&token, "max_keepalive", &tmp_int, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid 'max_keepalive' value (%d).", tmp_int);
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					config->max_keepalive = (uint16_t)tmp_int;
				}অন্যথা যদি(!strcmp(token, "max_packet_size")){
					যদি(conf__parse_int(&token, "max_packet_size", &tmp_int, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(tmp_int < 20){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: 'max_packet_size' must be >= 20.");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					config->max_packet_size = (uint32_t)tmp_int;
				}অন্যথা যদি(!strcmp(token, "max_queued_bytes")){
					যদি(conf__parse_int(&token, "max_queued_bytes", &tmp_int, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_bytes = (size_t)tmp_int;
				}অন্যথা যদি(!strcmp(token, "max_queued_messages")){
					যদি(conf__parse_int(&token, "max_queued_messages", &tmp_int, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_messages = tmp_int;
				}অন্যথা যদি(!strcmp(token, "memory_limit")){
					ssize_t lim;
					যদি(conf__parse_ssize_t(&token, "memory_limit", &lim, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(lim < 0){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid 'memory_limit' value (%ld).", lim);
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					mosquitto_memory_set_limit((size_t)lim);
				}অন্যথা যদি(!strcmp(token, "message_size_limit")){
					log__printf(রিক্ত, MOSQ_LOG_NOTICE, "Note: It is recommended to replace `message_size_limit` with `max_packet_size`.");
					যদি(conf__parse_int(&token, "message_size_limit", (পূর্ণাঙ্ক *)&config->message_size_limit, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(config->message_size_limit > MQTT_MAX_PAYLOAD){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid 'message_size_limit' value (%u).", config->message_size_limit);
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "mount_point")){
					REQUIRE_LISTENER(token);
					mosquitto_FREE(cur_listener->mount_point);
					যদি(conf__parse_string(&token, "mount_point", &cur_listener->mount_point, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(mosquitto_pub_topic_check(cur_listener->mount_point) != MOSQ_ERR_SUCCESS){
						log__printf(রিক্ত, MOSQ_LOG_ERR,
								"Error: Invalid 'mount_point' value (%s). Does it contain a wildcard character?",
								cur_listener->mount_point);
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "notifications")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_bool(&token, "notifications", &cur_bridge->notifications, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "notifications_local_only")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_bool(&token, "notifications_local_only", &cur_bridge->notifications_local_only, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "notification_topic")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_string(&token, "notification_topic", &cur_bridge->notification_topic, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "password") || !strcmp(token, "remote_password")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_string(&token, "bridge remote_password", &cur_bridge->remote_password, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "password_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->password_data.password_file);
					যদি(conf__parse_string(&token, "password_file", &cur_security_options->password_data.password_file, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "per_listener_settings")){
					OPTION_DEPRECATED(token, "Please see the documentation for how to achieve the same effect.");
					যদি(config->per_listener_settings){
						/* Once this is set, don't let it be unset. It should be the first config option ideally. */
						অনুবর্ত;
					}
					যদি(conf__parse_bool(&token, "per_listener_settings", &config->per_listener_settings, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(cur_security_options && config->per_listener_settings){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: per_listener_settings must be set before any other security settings.");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "persistence") || !strcmp(token, "retained_persistence")){
					যদি(conf__parse_bool(&token, token, &config->persistence, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "persistence_file")){
					যদি(conf__parse_string(&token, "persistence_file", &config->persistence_file, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "persistence_location")){
					যদি(conf__parse_string(&token, "persistence_location", &config->persistence_location, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "persistent_client_expiration")){
					token = strtok_r(রিক্ত, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "persistent_client_expiration");

					time_t expiration_mult;

					বিকল্প(token[strlen(token)-1]){
						পক্ষ 's':
							expiration_mult = 1;
							ভঙ্গ;
						পক্ষ 'h':
							expiration_mult = 3600;
							ভঙ্গ;
						পক্ষ 'd':
							expiration_mult = 86400;
							ভঙ্গ;
						পক্ষ 'w':
							expiration_mult = 86400*7;
							ভঙ্গ;
						পক্ষ 'm':
							expiration_mult = 86400*30;
							ভঙ্গ;
						পক্ষ 'y':
							expiration_mult = 86400*365;
							ভঙ্গ;
						স্বভাব:
							log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
							প্রত্যাগম MOSQ_ERR_INVAL;
					}
					token[strlen(token)-1] = '\0';
					config->persistent_client_expiration = atoi(token)*expiration_mult;
					যদি(config->persistent_client_expiration <= 0){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "pid_file")){
					যদি(reload){
						অনুবর্ত;        /* pid file not valid for reloading. */
					}
					যদি(conf__parse_string(&token, "pid_file", &config->pid_file, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "port")){
					OPTION_DEPRECATED(token, "Please use 'listener' instead.");
					config->local_only = false;
					যদি(reload){
						অনুবর্ত;        /* Listeners not valid for reloading. */

					}
					যদি(config__create_default_listener(config, token)){
						প্রত্যাগম MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					যদি(config->default_listener->port){
						log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Default listener port specified multiple times. Only the latest will be used.");
					}
					যদি(conf__parse_int(&token, "port", &tmp_int, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(tmp_int < 1 || tmp_int > UINT16_MAX){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					config->default_listener->port = (uint16_t)tmp_int;
				}অন্যথা যদি(!strcmp(token, "protocol")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(রিক্ত, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "protocol");

					যদি(!strcmp(token, "mqtt")){
						cur_listener->protocol = mp_mqtt;
						/*
						}else if(!strcmp(token, "mqttsn")){
						    cur_listener->protocol = mp_mqttsn;
						*/
					}অন্যথা যদি(!strcmp(token, "websockets")){
#ifdef WITH_WEBSOCKETS
						cur_listener->protocol = mp_websockets;
#অন্যথা
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Websockets support not available.");
						প্রত্যাগম MOSQ_ERR_INVAL;
#endif
					}অন্যথা যদি(!strcmp(token, "http_api")){
#ifdef WITH_HTTP_API
						cur_listener->protocol = mp_http_api;
#অন্যথা
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: HTTP API support not available.");
						প্রত্যাগম MOSQ_ERR_INVAL;
#endif
					}অন্যথা{
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid 'protocol' value (%s).", token);
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "proxy_protocol_v2_require_tls")){
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					প্রত্যাগম MOSQ_ERR_INVAL;
#endif
					REQUIRE_LISTENER(token);
					যদি(conf__parse_bool(&token, "proxy_protocol_v2_require_tls", &cur_listener->proxy_protocol_v2_require_tls, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "psk_file")){
#ifdef FINAL_WITH_TLS_PSK
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					যদি(cur_listener && cur_listener->certfile){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_security_options->psk_file);
					যদি(conf__parse_string(&token, "psk_file", &cur_security_options->psk_file, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "psk_hint")){
#ifdef FINAL_WITH_TLS_PSK
					যদি(reload){
						অনুবর্ত;        /* PSK file not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					যদি(conf__parse_string(&token, "psk_hint", &cur_listener->psk_hint, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "queue_qos0_messages")){
					যদি(conf__parse_bool(&token, token, &config->queue_qos0_messages, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "require_certificate")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					যদি(conf__parse_bool(&token, "require_certificate", &cur_listener->require_certificate, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "restart_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					cur_bridge->backoff_cap = 0; /* set backoff to constant mode, unless cap is specified further down */
					token = strtok_r(রিক্ত, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "restart_timeout");

					cur_bridge->restart_timeout = atoi(token);
					cur_bridge->backoff_base = 0;
					cur_bridge->backoff_cap = 0;
					যদি(cur_bridge->restart_timeout < 1){
						log__printf(রিক্ত, MOSQ_LOG_NOTICE, "restart_timeout interval too low, using 1 second.");
						cur_bridge->restart_timeout = 1;
					}অন্যথা যদি(cur_bridge->restart_timeout > 3600){
						log__printf(রিক্ত, MOSQ_LOG_NOTICE, "restart_timeout interval too high, using 3600 seconds.");
						cur_bridge->restart_timeout = 3600;
					}
					token = strtok_r(রিক্ত, " ", &saveptr);
					যদি(token){
						cur_bridge->backoff_base = cur_bridge->restart_timeout;
						cur_bridge->backoff_cap = atoi(token);
						যদি(cur_bridge->backoff_cap < cur_bridge->backoff_base){
							log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: backoff cap is lower than the base in restart_timeout.");
							প্রত্যাগম MOSQ_ERR_INVAL;
						}অন্যথা যদি(cur_bridge->backoff_cap > 7200){
							log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: backoff cap too high, using 7200 seconds.");
							cur_bridge->backoff_cap = 7200;
						}

						token = strtok_r(রিক্ত, " ", &saveptr);
						যদি(token){
							cur_bridge->stable_connection_period = atoi(token);
							যদি(cur_bridge->stable_connection_period < 0){
								log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: stable connection period cannot be negative.");
								প্রত্যাগম MOSQ_ERR_INVAL;
							}
						}
					}
					cur_bridge->restart_timeout *= 1000; /* backoff is tracked in ms */
					cur_bridge->backoff_base *= 1000;
					cur_bridge->backoff_cap *= 1000;
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "retain_available")){
					যদি(conf__parse_bool(&token, token, &config->retain_available, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "retain_expiry_interval")){
					যদি(conf__parse_int(&token, token, &config->retain_expiry_interval, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(config->retain_expiry_interval < 1){
						log__printf(রিক্ত, MOSQ_LOG_WARNING, "Error: retain_expiry_interval must be >= 1.");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}অন্যথা যদি(config->retain_expiry_interval > 10000000){
						log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: retain_expiry_interval being capped at 19 years.");
						config->retain_expiry_interval = 10000000;
					}
					config->retain_expiry_interval *= 60;
				}অন্যথা যদি(!strcmp(token, "retry_interval")){
					OPTION_UNAVAILABLE(token);
				}অন্যথা যদি(!strcmp(token, "round_robin")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_bool(&token, "round_robin", &cur_bridge->round_robin, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "set_tcp_nodelay")){
					যদি(conf__parse_bool(&token, "set_tcp_nodelay", &config->set_tcp_nodelay, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "start_type")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(রিক্ত, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					যদি(!strcmp(token, "automatic")){
						cur_bridge->start_type = bst_automatic;
					}অন্যথা যদি(!strcmp(token, "lazy")){
						cur_bridge->start_type = bst_lazy;
					}অন্যথা যদি(!strcmp(token, "manual")){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Manual start_type not supported.");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}অন্যথা যদি(!strcmp(token, "once")){
						cur_bridge->start_type = bst_once;
					}অন্যথা{
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid 'start_type' value in configuration (%s).", token);
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "socket_domain")){
					যদি(reload){
						অনুবর্ত;        /* socket_domain not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(রিক্ত, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					যদি(!strcmp(token, "ipv4")){
						cur_listener->socket_domain = AF_INET;
					}অন্যথা যদি(!strcmp(token, "ipv6")){
						cur_listener->socket_domain = AF_INET6;
					}অন্যথা{
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid 'socket_domain' value '%s' in configuration.", token);
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "sys_interval")){
					যদি(conf__parse_int(&token, "sys_interval", &config->sys_interval, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(config->sys_interval < 0 || config->sys_interval > 65535){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid 'sys_interval' value (%d).", config->sys_interval);
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "threshold")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_int(&token, "threshold", &cur_bridge->threshold, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(cur_bridge->threshold < 1){
						log__printf(রিক্ত, MOSQ_LOG_NOTICE, "threshold too low, using 1 message.");
						cur_bridge->threshold = 1;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "tls_engine")){
#ifdef WITH_TLS
					যদি(reload){
						অনুবর্ত;        /* tls_engine not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					যদি(conf__parse_string(&token, "tls_engine", &cur_listener->tls_engine, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "tls_engine_kpass_sha1")){
#ifdef WITH_TLS
					যদি(reload){
						অনুবর্ত;        /* tls_engine not valid for reloading. */

					}
					বর্ণ *kpass_sha = রিক্ত, *kpass_sha_bin = রিক্ত;
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					যদি(conf__parse_string(&token, "tls_engine_kpass_sha1", &kpass_sha, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(mosquitto__hex2bin_sha1(kpass_sha, (unsigned বর্ণ **)&kpass_sha_bin) != MOSQ_ERR_SUCCESS){
						mosquitto_FREE(kpass_sha);
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					mosquitto_free(cur_listener->tls_engine_kpass_sha1);
					cur_listener->tls_engine_kpass_sha1 = kpass_sha_bin;
					mosquitto_FREE(kpass_sha);
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "tls_keyform")){
#ifdef WITH_TLS
					যদি(reload){
						অনুবর্ত;        /* tls_engine not valid for reloading. */

					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					বর্ণ *keyform = রিক্ত;
					যদি(conf__parse_string(&token, "tls_keyform", &keyform, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					cur_listener->tls_keyform = mosq_k_pem;
					যদি(!strcmp(keyform, "engine")){
						cur_listener->tls_keyform = mosq_k_engine;
					}
					mosquitto_FREE(keyform);
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "tls_version")){
#যদি defined(WITH_TLS)
					যদি(reload){
						অনুবর্ত;        /* tls_version not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					যদি(conf__parse_string(&token, "tls_version", &cur_listener->tls_version, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "topic")){
#ifdef WITH_BRIDGE
					বর্ণ *topic = রিক্ত;
					গণনা mosquitto__bridge_direction direction = bd_out;
					uint8_t qos = 0;
					বর্ণ *local_prefix = রিক্ত, *remote_prefix = রিক্ত;

					REQUIRE_BRIDGE(token);

					token = strtok_r(রিক্ত, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "topic");

					// Check if the topic is quoted (e.g. for spaces within topic names), but not the
					// special case of ""
					যদি(token[0] == '"' && token [1] != '"'){
						যদি(strchr(saveptr, '"') == রিক্ত){
							log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Missing closing quote in topic value (%s).", saveptr);
							প্রত্যাগম MOSQ_ERR_INVAL;
						}

						বর্ণ *topic_in_quotes = strtok_r(রিক্ত, "\"", &saveptr);
						size_t tlen = 1;
						যদি(topic_in_quotes){
							tlen = strlen(topic_in_quotes);
						}

						topic = mosquitto_malloc(strlen(token) + tlen + 1);
						যদি(!topic){
							log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
							প্রত্যাগম MOSQ_ERR_NOMEM;
						}

						strcpy(topic, token + 1);
						strcat(topic, " ");
						যদি(topic_in_quotes){
							strcat(topic, topic_in_quotes);
						}
					}অন্যথা{
						topic = mosquitto_strdup(token);
						যদি(!topic){
							log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
							প্রত্যাগম MOSQ_ERR_NOMEM;
						}
					}

					token = strtok_r(রিক্ত, " ", &saveptr);
					যদি(token){
						যদি(!strcasecmp(token, "out")){
							direction = bd_out;
						}অন্যথা যদি(!strcasecmp(token, "in")){
							direction = bd_in;
						}অন্যথা যদি(!strcasecmp(token, "both")){
							direction = bd_both;
						}অন্যথা{
							log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid bridge topic direction '%s'.", token);
							mosquitto_FREE(topic);
							প্রত্যাগম MOSQ_ERR_INVAL;
						}
						token = strtok_r(রিক্ত, " ", &saveptr);
						যদি(token){
							যদি(token[0] == '#'){
								(শূন্য)strtok_r(রিক্ত, "", &saveptr);
							}
							qos = (uint8_t)atoi(token);
							যদি(qos > 2){
								log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid bridge QoS level '%s'.", token);
								mosquitto_FREE(topic);
								প্রত্যাগম MOSQ_ERR_INVAL;
							}

							token = strtok_r(রিক্ত, " ", &saveptr);
							যদি(token){
								যদি(!strcmp(token, "\"\"") || token[0] == '#'){
									local_prefix = রিক্ত;
									যদি(token[0] == '#'){
										(শূন্য)strtok_r(রিক্ত, "", &saveptr);
									}
								}অন্যথা{
									local_prefix = token;
								}

								token = strtok_r(রিক্ত, " ", &saveptr);
								যদি(token){
									যদি(!strcmp(token, "\"\"") || token[0] == '#'){
										remote_prefix = রিক্ত;
									}অন্যথা{
										remote_prefix = token;
									}
								}
							}
						}
					}

					যদি(bridge__add_topic(cur_bridge, topic, direction, qos, local_prefix, remote_prefix)){
						mosquitto_FREE(topic);
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(topic);
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "max_topic_alias")){
					REQUIRE_LISTENER(token);
					যদি(conf__parse_int(&token, "max_topic_alias", &tmp_int, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias' value in configuration.");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias = (uint16_t)tmp_int;
				}অন্যথা যদি(!strcmp(token, "max_topic_alias_broker")){
					REQUIRE_LISTENER(token);
					যদি(conf__parse_int(&token, "max_topic_alias_broker", &tmp_int, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias_broker' value in configuration.");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias_broker = (uint16_t)tmp_int;
				}অন্যথা যদি(!strcmp(token, "try_private")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_bool(&token, "try_private", &cur_bridge->try_private, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "upgrade_outgoing_qos")){
					যদি(conf__parse_bool(&token, token, &config->upgrade_outgoing_qos, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "use_identity_as_username")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					যদি(conf__parse_bool(&token, "use_identity_as_username", &cur_listener->use_identity_as_username, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "use_subject_as_username")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					যদি(conf__parse_bool(&token, "use_subject_as_username", &cur_listener->use_subject_as_username, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "user")){
					যদি(reload){
						অনুবর্ত;        /* Drop privileges user not valid for reloading. */
					}
					mosquitto_FREE(config->user);
					যদি(conf__parse_string(&token, "user", &config->user, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "use_username_as_clientid")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					যদি(conf__parse_bool(&token, "use_username_as_clientid", &cur_listener->use_username_as_clientid, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
				}অন্যথা যদি(!strcmp(token, "username") || !strcmp(token, "remote_username")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					যদি(conf__parse_string(&token, "bridge remote_username", &cur_bridge->remote_username, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}অন্যথা যদি(!strcmp(token, "websockets_log_level")){
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					যদি(conf__parse_int(&token, "websockets_log_level", &config->websockets_log_level, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
#endif
				}অন্যথা যদি(!strcmp(token, "websockets_headers_size") || !strcmp(token, "packet_buffer_size")){
					যদি(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					যদি(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(রিক্ত, MOSQ_LOG_WARNING, "Error: Packet buffer size must be between 0 and 65535 inclusive.");
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					config->packet_buffer_size = (uint16_t)tmp_int;
				}অন্যথা যদি(!strcmp(token, "websockets_origin")){
#ifdef WITH_WEBSOCKETS
#  যদি LWS_LIBRARY_VERSION_সংখ্যা >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN
					REQUIRE_LISTENER(token);
					ws_origins = mosquitto_realloc(cur_listener->ws_origins, পরিমাণ(বর্ণ *)*(size_t)(cur_listener->ws_origin_গুনতি+1));
					যদি(ws_origins == রিক্ত){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
						প্রত্যাগম MOSQ_ERR_NOMEM;
					}
					ws_origins[cur_listener->ws_origin_গুনতি] = রিক্ত;
					cur_listener->ws_origins = ws_origins;
					যদি(conf__parse_string(&token, "websockets_origin", &ws_origins[cur_listener->ws_origin_গুনতি], &saveptr)){
						প্রত্যাগম MOSQ_ERR_INVAL;
					}
					cur_listener->ws_origin_গুনতি++;
#  অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: websockets_origin support not available, libwebsockets version is too old.");
#  endif
#অন্যথা
					log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Websockets support not available.");
#endif
				}অন্যথা{
					log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unknown configuration variable '%s'.", token);
					প্রত্যাগম MOSQ_ERR_INVAL;
				}
			}
		}
	}
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক config__read_file(সংরচনা mosquitto__config *config, bool reload, ধ্রুব বর্ণ *file, সংরচনা config_recurse *cr, পূর্ণাঙ্ক level, পূর্ণাঙ্ক *lineno)
{
	পূর্ণাঙ্ক rc;
	FILE *fptr = রিক্ত;
	বর্ণ *buf;
	পূর্ণাঙ্ক buflen;
#ifndef WIN32
	DIR *dir;
#endif

#ifndef WIN32
	dir = opendir(file);
	যদি(dir){
		closedir(dir);
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Config file '%s' is a directory.", file);
		প্রত্যাগম 1;
	}
#endif

	fptr = mosquitto_fopen(file, "rt", false);
	যদি(!fptr){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to open config file '%s'.", file);
		প্রত্যাগম 1;
	}

	buflen = 1000;
	buf = mosquitto_malloc((size_t)buflen);
	যদি(!buf){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
		fclose(fptr);
		প্রত্যাগম MOSQ_ERR_NOMEM;
	}

	rc = config__read_file_core(config, reload, cr, level, lineno, fptr, &buf, &buflen);
	mosquitto_FREE(buf);
	fclose(fptr);

	প্রত্যাগম rc;
}


স্থির পূর্ণাঙ্ক config__check_proxy(সংরচনা mosquitto__config *config)
{
	প্রতি(পূর্ণাঙ্ক i=0; i<config->listener_গুনতি; i++){
		সংরচনা mosquitto__listener *l = &config->listeners[i];

		যদি(l->enable_proxy_protocol == 2){
#ifdef WITH_TLS
			যদি(l->use_subject_as_username){
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: use_subject_as_username cannot be used with `enable_proxy_protocol 2`.");
				প্রত্যাগম MOSQ_ERR_INVAL;
			}

			যদি(l->certfile || l->keyfile){
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: certfile and keyfile cannot be used with `enable_proxy_protocol 2`.");
				প্রত্যাগম MOSQ_ERR_INVAL;
			}
#endif
		}
	}

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণাঙ্ক config__check(সংরচনা mosquitto__config *config)
{
	/* Checks that are easy to make after the config has been loaded. */

	ধ্রুব বর্ণ *id_prefix;
	পূর্ণাঙ্ক id_prefix_len;
	যদি(config->security_options.auto_id_prefix){
		id_prefix = config->security_options.auto_id_prefix;
		id_prefix_len = config->security_options.auto_id_prefix_len;
	}অন্যথা{
		id_prefix = "auto-";
		id_prefix_len = (পূর্ণাঙ্ক)strlen("auto-");
	}

	/* Default to auto_id_prefix = 'auto-' if none set. */
	প্রতি(পূর্ণাঙ্ক i=0; i<config->listener_গুনতি; i++){
		যদি(!config->listeners[i].security_options->auto_id_prefix){
			config->listeners[i].security_options->auto_id_prefix = mosquitto_strdup(id_prefix);
			যদি(!config->listeners[i].security_options->auto_id_prefix){
				প্রত্যাগম MOSQ_ERR_NOMEM;
			}
			config->listeners[i].security_options->auto_id_prefix_len = (uint16_t)id_prefix_len;
		}
	}

	প্রত্যাগম config__check_proxy(config);
}

#ifdef WITH_BRIDGE


স্থির পূর্ণাঙ্ক config__check_bridges(সংরচনা mosquitto__config *config)
{
	সংরচনা mosquitto__bridge *bridge1, *bridge2;
	বর্ণ hostname[256];
	size_t len;

	/* Check for bridge duplicate local_clientid, need to generate missing IDs
	 * first. */
	প্রতি(পূর্ণাঙ্ক i=0; i<config->bridge_গুনতি; i++){
		bridge1 = config->bridges[i];

		যদি(!bridge1->remote_clientid){
			যদি(!gethostname(hostname, 256)){
				len = strlen(hostname) + strlen(bridge1->name) + 2;
				bridge1->remote_clientid = mosquitto_malloc(len);
				যদি(!bridge1->remote_clientid){
					প্রত্যাগম MOSQ_ERR_NOMEM;
				}
				snprintf(bridge1->remote_clientid, len, "%s.%s", hostname, bridge1->name);
			}অন্যথা{
				প্রত্যাগম 1;
			}
		}

		যদি(!bridge1->local_clientid){
			len = strlen(bridge1->remote_clientid) + strlen("local.") + 2;
			bridge1->local_clientid = mosquitto_malloc(len);
			যদি(!bridge1->local_clientid){
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
				প্রত্যাগম MOSQ_ERR_NOMEM;
			}
			snprintf(bridge1->local_clientid, len, "local.%s", bridge1->remote_clientid);
		}
	}

	প্রতি(পূর্ণাঙ্ক i=0; i<config->bridge_গুনতি; i++){
		bridge1 = config->bridges[i];
		প্রতি(পূর্ণাঙ্ক j=i+1; j<config->bridge_গুনতি; j++){
			bridge2 = config->bridges[j];
			যদি(!strcmp(bridge1->local_clientid, bridge2->local_clientid)){
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Bridge local_clientid "
						"'%s' is not unique. Try changing or setting the "
						"local_clientid value for one of the bridges.",
						bridge1->local_clientid);
				প্রত্যাগম MOSQ_ERR_INVAL;
			}
		}
	}

#ifdef WITH_TLS
	/* Check for missing TLS cafile/capath/certfile/keyfile */
	প্রতি(পূর্ণাঙ্ক i=0; i<config->listener_গুনতি; i++){
		bool cafile = !!config->listeners[i].cafile;
		bool capath = !!config->listeners[i].capath;
		bool certfile = !!config->listeners[i].certfile;
		bool keyfile = !!config->listeners[i].keyfile;

		যদি((certfile && !keyfile) || (!certfile && keyfile)){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Both certfile and keyfile must be provided to enable a TLS listener.");
			প্রত্যাগম MOSQ_ERR_INVAL;
		}
		যদি(cafile && !certfile){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: cafile specified without certfile and keyfile.");
			প্রত্যাগম MOSQ_ERR_INVAL;
		}
		যদি(capath && !certfile){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: capath specified without certfile and keyfile.");
			প্রত্যাগম MOSQ_ERR_INVAL;
		}
	}
#endif
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}
#endif


স্থির পূর্ণাঙ্ক conf__parse_bool(বর্ণ **token, ধ্রুব বর্ণ *name, bool *মান, বর্ণ **saveptr)
{
	*token = strtok_r(রিক্ত, " ", saveptr);
	যদি(*token){
		যদি(!strcmp(*token, "false") || !strcmp(*token, "0")){
			*মান = false;
		}অন্যথা যদি(!strcmp(*token, "true") || !strcmp(*token, "1")){
			*মান = true;
		}অন্যথা{
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Invalid '%s' value (%s).", name, *token);
			প্রত্যাগম MOSQ_ERR_INVAL;
		}
	}অন্যথা{
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণাঙ্ক conf__parse_int(বর্ণ **token, ধ্রুব বর্ণ *name, পূর্ণাঙ্ক *মান, বর্ণ **saveptr)
{
	*token = strtok_r(রিক্ত, " ", saveptr);
	যদি(*token){
		বর্ণ *endptr = রিক্ত;
		long l = strtol(*token, &endptr, 0);
		যদি(endptr == *token || endptr[0] != '\0'){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: '%s' value not a number.", name);
			প্রত্যাগম MOSQ_ERR_INVAL;
		}
		যদি(l > INT_MAX || l < INT_MIN){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: '%s' value out of range.", name);
			প্রত্যাগম MOSQ_ERR_INVAL;
		}
		*মান = (পূর্ণাঙ্ক)l;
	}অন্যথা{
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণাঙ্ক conf__parse_ssize_t(বর্ণ **token, ধ্রুব বর্ণ *name, ssize_t *মান, বর্ণ **saveptr)
{
	*token = strtok_r(রিক্ত, " ", saveptr);
	যদি(*token){
		*মান = atol(*token);
	}অন্যথা{
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণাঙ্ক conf__parse_string(বর্ণ **token, ধ্রুব বর্ণ *name, বর্ণ **মান, বর্ণ **saveptr)
{
	size_t tlen;

	*token = strtok_r(রিক্ত, "", saveptr);
	যদি(*token){
		যদি(*মান){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Duplicate '%s' value in configuration.", name);
			প্রত্যাগম MOSQ_ERR_INVAL;
		}
		/* Deal with multiple spaces at the beginning of the string. */
		*token = mosquitto_trimblanks(*token);
		যদি(strlen(*token) == 0){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
			প্রত্যাগম MOSQ_ERR_INVAL;
		}

		tlen = strlen(*token);
		যদি(tlen > UINT16_MAX){
			প্রত্যাগম MOSQ_ERR_INVAL;
		}
		যদি(mosquitto_validate_utf8(*token, (uint16_t)tlen)){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Malformed UTF-8 in configuration.");
			প্রত্যাগম MOSQ_ERR_INVAL;
		}
		*মান = mosquitto_strdup(*token);
		যদি(!*মান){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
			প্রত্যাগম MOSQ_ERR_NOMEM;
		}
	}অন্যথা{
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		প্রত্যাগম MOSQ_ERR_INVAL;
	}
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}
