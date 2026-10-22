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

#अन्तर्भाव "config.h"

#अन्तर्भाव <limits.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <errno.h>

#ifdef WIN32
#अण्णहा
#  include <dirent.h>
#  include <strings.h>
#endif

#ifndef WIN32
#  include <netdb.h>
#  include <sys/socket.h>
#अण्णहा
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#जइ !defined(WIN32) && !defined(__CYGWIN__)
#  include <syslog.h>
#endif

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "tls_mosq.h"
#अन्तर्भाव "util_mosq.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"

#अन्तर्भाव "utlist.h"

संरचना config_recurse {
	unsigned पूर्णाङ्क log_dest;
	पूर्णाङ्क log_dest_set;
	unsigned पूर्णाङ्क log_type;
	पूर्णाङ्क log_type_set;
};

#जइ defined(WIN32) || defined(__CYGWIN__)
#अन्तर्भाव <windows.h>
बाह्य SERVICE_STATUS_HANDLE service_handle;
#endif


#define REQUIRE_LISTENER(A) \
		करेहि{ \
			जइ(cur_listener == रिक्त){ \
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				प्रत्यागम MOSQ_ERR_INVAL; \
			} \
		}जाव(0)

#define REQUIRE_LISTENER_OR_DEFAULT_LISTENER(A) \
		करेहि{ \
			जइ(cur_listener == रिक्त){ \
				जइ(config__create_default_listener(config, (A))){ \
					प्रत्यागम MOSQ_ERR_NOMEM; \
				} \
				cur_listener = config->default_listener; \
			} \
		}जाव(0)

#define REQUIRE_LISTENER_IF_PER_LISTENER(A) \
		करेहि{ \
			जइ(config->per_listener_settings == true && cur_listener == रिक्त){ \
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				प्रत्यागम MOSQ_ERR_INVAL; \
			} \
		}जाव(0)

#define REQUIRE_NON_DEFAULT_LISTENER(A) \
		करेहि{ \
			जइ(cur_listener == config->default_listener || cur_listener == रिक्त){ \
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				प्रत्यागम MOSQ_ERR_INVAL; \
			} \
		}जाव(0)

#define REQUIRE_BRIDGE(A) \
		करेहि{ \
			जइ(cur_bridge == रिक्त){ \
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: The '%s' option requires a bridge to be defined first.", (A)); \
				प्रत्यागम MOSQ_ERR_INVAL; \
			} \
		}जाव(0)

#define REQUIRE_PLUGIN(A) \
		करेहि{ \
			जइ(cur_plugin == रिक्त){ \
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: The '%s' option requires plugin/global_plugin/plugin_load to be defined first.", (A)); \
				प्रत्यागम MOSQ_ERR_INVAL; \
			} \
		}जाव(0)

#define OPTION_DEPRECATED(A, B) \
		log__printf(रिक्त, MOSQ_LOG_NOTICE, "The '%s' option is now deprecated and will be removed in version 3.0. %s", (A), (B))

#define OPTION_UNAVAILABLE(A) \
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: The '%s' option is no longer available.", (A));

#define REQUIRE_NON_EMPTY_OPTION(A, B) \
		करेहि{ \
			जइ(!(A)){ \
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", (B)); \
				प्रत्यागम MOSQ_ERR_INVAL; \
			} \
		}जाव(0)

#define PER_LISTENER_ALTERNATIVE(A, B) \
		जइ(config->per_listener_settings){ \
			log__printf(रिक्त, MOSQ_LOG_NOTICE, "You are using the '%s' option with 'per_listener_settings true'. Please replace this with '%s'.", A, B); \
		} \


#ifdef FINAL_WITH_TLS_PSK
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A) \
		करेहि{ \
			जइ(cur_bridge->tls_psk_identity || cur_bridge->tls_psk){ \
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and psk encryption in a single bridge.", (A)); \
				प्रत्यागम MOSQ_ERR_INVAL; \
			} \
		}जाव(0)

#  define REQUIRE_BRIDGE_NO_X509(A) \
		करेहि{ \
			जइ(cur_bridge->tls_cafile || cur_bridge->tls_capath || cur_bridge->tls_certfile || cur_bridge->tls_keyfile){ \
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and identity encryption in a single bridge.", (A)); \
				प्रत्यागम MOSQ_ERR_INVAL; \
			} \
		}जाव(0)

#अण्णहा
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A)
#  define REQUIRE_BRIDGE_NO_X509(A)
#endif

स्थिर संरचना mosquitto__security_options *cur_security_options = रिक्त;

स्थिर पूर्णाङ्क conf__parse_bool(वर्ण **token, ध्रुव वर्ण *name, bool *मानम्, वर्ण **saveptr);
स्थिर पूर्णाङ्क conf__parse_int(वर्ण **token, ध्रुव वर्ण *name, पूर्णाङ्क *मानम्, वर्ण **saveptr);
स्थिर पूर्णाङ्क conf__parse_ssize_t(वर्ण **token, ध्रुव वर्ण *name, ssize_t *मानम्, वर्ण **saveptr);
स्थिर पूर्णाङ्क conf__parse_string(वर्ण **token, ध्रुव वर्ण *name, वर्ण **मानम्, वर्ण **saveptr);
स्थिर पूर्णाङ्क config__read_file(संरचना mosquitto__config *config, bool reload, ध्रुव वर्ण *file, संरचना config_recurse *config_tmp, पूर्णाङ्क level, पूर्णाङ्क *lineno);
स्थिर पूर्णाङ्क config__check(संरचना mosquitto__config *config);
स्थिर शून्य config__cleanup_plugins(शून्य);
#ifdef WITH_BRIDGE
स्थिर पूर्णाङ्क config__check_bridges(संरचना mosquitto__config *config);
#endif


स्थिर पूर्णाङ्क config__add_listener(संरचना mosquitto__config *config)
{
	संरचना mosquitto__listener *listener;
	संरचना mosquitto__listener *new_listeners;
	पूर्णाङ्क def_listener = -1;

	जइ(config->default_listener){
		प्रति(पूर्णाङ्क i=0; i<config->listener_गणनम्; i++){
			जइ(&config->listeners[i] == config->default_listener){
				def_listener = i;
				भङ्ग;
			}
		}
	}
	new_listeners = mosquitto_realloc(config->listeners, परिमाण(संरचना mosquitto__listener)*(size_t)(config->listener_गणनम्+1));
	जइ(!new_listeners){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	config->listeners = new_listeners;
	listener = &config->listeners[config->listener_गणनम्];
	memset(listener, 0, परिमाण(संरचना mosquitto__listener));
	listener->security_options = mosquitto_calloc(1, परिमाण(संरचना mosquitto__security_options));
	जइ(!listener->security_options){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	जइ(def_listener != -1){
		config->default_listener = &config->listeners[def_listener];
	}
	config->listener_गणनम्++;

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क config__create_default_listener(संरचना mosquitto__config *config, ध्रुव वर्ण *option_name)
{
	जइ(config->default_listener){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}
	log__printf(रिक्त, MOSQ_LOG_INFO, "Creating default listener due to '%s' option.", option_name);
	log__printf(रिक्त, MOSQ_LOG_INFO, "It is best practice to define a 'listener' first. Using the '%s' option without a listener will be disabled in the future.", option_name);
	जइ(config__add_listener(config)){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	config->default_listener = &config->listeners[config->listener_गणनम्-1];
	listener__set_defaults(config->default_listener);
	config->default_listener->port = 1883;

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर शून्य conf__set_cur_security_options(संरचना mosquitto__config *config, संरचना mosquitto__listener **cur_listener, संरचना mosquitto__security_options **security_options, ध्रुव वर्ण *option_name)
{
	जइ(config->per_listener_settings){
		जइ(*cur_listener == रिक्त){
			जइ(config__create_default_listener(config, option_name)){
				प्रत्यागम;
			}
			*cur_listener = config->default_listener;
		}
		(*security_options) = (*cur_listener)->security_options;
	}अण्णहा{
		(*security_options) = &config->security_options;
	}
}


स्थिर पूर्णाङ्क conf__attempt_resolve(ध्रुव वर्ण *host, ध्रुव वर्ण *text, unsigned पूर्णाङ्क log, ध्रुव वर्ण *सन्देशः)
{
	संरचना addrinfo gai_hints;
	संरचना addrinfo *gai_res;
	पूर्णाङ्क rc;

	memset(&gai_hints, 0, परिमाण(संरचना addrinfo));
	gai_hints.ai_family = AF_UNSPEC;
	gai_hints.ai_socktype = SOCK_STREAM;
	gai_res = रिक्त;
	rc = getaddrinfo(host, रिक्त, &gai_hints, &gai_res);
	जइ(gai_res){
		freeaddrinfo(gai_res);
	}
	जइ(rc != 0){
#ifndef WIN32
		जइ(rc == EAI_SYSTEM){
			जइ(errno == ENOENT){
				log__printf(रिक्त, log, "%s: Unable to resolve %s %s.", सन्देशः, text, host);
			}अण्णहा{
				log__printf(रिक्त, log, "%s: Error resolving %s: %s.", सन्देशः, text, strerror(errno));
			}
		}अण्णहा{
			log__printf(रिक्त, log, "%s: Error resolving %s: %s.", सन्देशः, text, gai_strerror(rc));
		}
#अण्णहा
		जइ(rc == WSAHOST_NOT_FOUND){
			log__printf(रिक्त, log, "%s: Error resolving %s.", सन्देशः, text);
		}
#endif
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर शून्य config__init_reload(संरचना mosquitto__config *config)
{
	/* Set defaults */
	प्रति(पूर्णाङ्क i=0; i<config->listener_गणनम्; i++){
		listener__set_defaults(&config->listeners[i]);
	}

	config->local_only = true;
	config->allow_duplicate_messages = true;

	mosquitto_FREE(config->security_options.acl_data.acl_file);
	mosquitto_FREE(config->security_options.password_data.password_file);
	mosquitto_FREE(config->security_options.psk_file);

	config->security_options.allow_anonymous = -1;
	config->security_options.allow_zero_length_clientid = true;
	config->security_options.auto_id_prefix = रिक्त;
	config->security_options.auto_id_prefix_len = 0;

	config->autosave_interval = 1800;
	config->autosave_on_changes = false;

	mosquitto_FREE(config->clientid_prefixes);

	config->connection_messages = true;
	config->clientid_prefixes = रिक्त;
	config->per_listener_settings = false;
	जइ(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = रिक्त;
	}
	mosquitto_FREE(config->log_file);

#जइ defined(WIN32) || defined(__CYGWIN__)
	जइ(service_handle){
		/* This is running as a Windows service. Default to no logging. Using
		 * stdout/stderr is forbidden because the first clients to connect will
		 * get log information sent to them for some reason. */
		config->log_dest = MQTT3_LOG_NONE;
	}अण्णहा{
		config->log_dest = MQTT3_LOG_STDERR;
	}
#अण्णहा
	config->log_facility = LOG_DAEMON;
	config->log_dest = MQTT3_LOG_STDERR | MQTT3_LOG_DLT;
	जइ(db.quiet){
		config->log_type = 0;
	}अण्णहा जइ(db.verbose){
		config->log_type = UINT_MAX;
	}अण्णहा{
		config->log_type = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;
	}
#endif
	config->log_timestamp = true;
	mosquitto_FREE(config->log_timestamp_format);
	config->global_max_clients = -1;
	config->global_max_connections = -1;
	config->log_timestamp_format = रिक्त;
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


स्थिर शून्य config__cleanup_plugin_config(mosquitto_plugin_id_t *plugin)
{
	mosquitto_FREE(plugin->config.path);
	mosquitto_FREE(plugin->config.name);

	जइ(plugin->config.options){
		प्रति(पूर्णाङ्क j=0; j<plugin->config.option_गणनम्; j++){
			mosquitto_FREE(plugin->config.options[j].कुञ्चिका);
			mosquitto_FREE(plugin->config.options[j].मानम्);
		}
		mosquitto_FREE(plugin->config.options);
		plugin->config.option_गणनम् = 0;
	}
	mosquitto_FREE(plugin->config.security_options);
	mosquitto_FREE(plugin);
}


स्थिर शून्य config__cleanup_plugins(शून्य)
{
	प्रति(पूर्णाङ्क i=0; i<db.plugin_गणनम्; i++){
		config__cleanup_plugin_config(db.plugins[i]);
	}
	mosquitto_FREE(db.plugins);
}


शून्य config__init(संरचना mosquitto__config *config)
{
	memset(config, 0, परिमाण(संरचना mosquitto__config));
	config__init_reload(config);

	config->daemon = false;
}


शून्य config__cleanup(संरचना mosquitto__config *config)
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
	जइ(config->listeners){
		प्रति(पूर्णाङ्क i=0; i<config->listener_गणनम्; i++){
			mosquitto_FREE(config->listeners[i].host);
			mosquitto_FREE(config->listeners[i].bind_interface);
			mosquitto_FREE(config->listeners[i].mount_point);
			mosquitto_FREE(config->listeners[i].socks);
			जइ(config->listeners[i].security_options){
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
#जइ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			जइ(!config->listeners[i].ws_context) /* libwebsockets frees its own SSL_CTX */
#endif
			{
				SSL_CTX_free(config->listeners[i].ssl_ctx);
				config->listeners[i].ssl_ctx = रिक्त;
			}
#endif
#जइ defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
			mosquitto_FREE(config->listeners[i].http_dir);
#endif
#ifdef WITH_WEBSOCKETS
			प्रति(पूर्णाङ्क j=0; j<config->listeners[i].ws_origin_गणनम्; j++){
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
	जइ(config->bridges){
		प्रति(पूर्णाङ्क i=0; i<config->bridge_गणनम्; i++){
			config__bridge_cleanup(config->bridges[i]);
		}
		mosquitto_FREE(config->bridges);
	}
#endif
	config__cleanup_plugins();

	जइ(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = रिक्त;
	}
	जइ(config->log_file){
		mosquitto_FREE(config->log_file);
		config->log_file = रिक्त;
	}
}

#ifdef WITH_BRIDGE


शून्य config__bridge_cleanup(संरचना mosquitto__bridge *bridge)
{
	जइ(bridge == रिक्त){
		प्रत्यागम;
	}

	mosquitto_FREE(bridge->name);
	जइ(bridge->addresses){
		प्रति(पूर्णाङ्क i=0; i<bridge->address_गणनम्; i++){
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
	जइ(bridge->topics){
		संरचना mosquitto__bridge_topic *cur_topic, *topic_tmp;

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


स्थिर शून्य print_version(शून्य)
{
	printf("mosquitto %s\n", VERSION);
	printf("Copyright © 2025 Roger Light.\n");
	printf("License EPL-2.0 OR BSD-3-Clause.\n");
}


स्थिर शून्य print_usage(शून्य)
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


पूर्णाङ्क config__parse_args(संरचना mosquitto__config *config, पूर्णाङ्क argc, वर्ण *argv[])
{
	पूर्णाङ्क i;
	पूर्णाङ्क port_tmp;

	प्रति(i=1; i<argc; i++){
		जइ(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--config-file")){
			जइ(i<argc-1){
				db.config_file = argv[i+1];

				जइ(config__read(config, false)){
					प्रत्यागम MOSQ_ERR_INVAL;
				}
			}अण्णहा{
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: -c argument given, but no config file specified.");
				प्रत्यागम MOSQ_ERR_INVAL;
			}
			i++;
		}अण्णहा जइ(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--daemon")){
			config->daemon = true;
		}अण्णहा जइ(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
			print_usage();
			प्रत्यागम MOSQ_ERR_UNKNOWN;
		}अण्णहा जइ(!strcmp(argv[i], "--version")){
			print_version();
			प्रत्यागम MOSQ_ERR_UNKNOWN;
		}अण्णहा जइ(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port")){
			जइ(i<argc-1){
				port_tmp = atoi(argv[i+1]);
				जइ(port_tmp<1 || port_tmp>UINT16_MAX){
					log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid port specified (%d).", port_tmp);
					प्रत्यागम MOSQ_ERR_INVAL;
				}अण्णहा{
					जइ(config->cmd_port_गणनम् == CMD_PORT_LIMIT){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Only %d ports can be specified on the command line.", CMD_PORT_LIMIT);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					config->cmd_port[config->cmd_port_गणनम्] = (uint16_t)port_tmp;
					config->cmd_port_गणनम्++;
				}
			}अण्णहा{
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: -p argument given, but no port specified.");
				प्रत्यागम MOSQ_ERR_INVAL;
			}
			i++;
		}अण्णहा जइ(!strcmp(argv[i], "--tls-keylog")){
#ifdef WITH_TLS
			जइ(i<argc-1){
				db.tls_keylog = mosquitto_strdup(argv[i+1]);
				जइ(db.tls_keylog == रिक्त){
					log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
					प्रत्यागम MOSQ_ERR_NOMEM;
				}
			}अण्णहा{
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: --tls-keylog argument given, but no file specified.");
				प्रत्यागम MOSQ_ERR_INVAL;
			}
			i++;
#अण्णहा
			fprintf(stderr, "Error: TLS support not available so --tls-keylog is not available.\n");
			प्रत्यागम MOSQ_ERR_INVAL;
#endif
		}अण्णहा जइ(!strcmp(argv[i], "-q") || !strcmp(argv[i], "--quiet")){
			db.quiet = true;
		}अण्णहा जइ(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")){
			db.verbose = true;
		}अण्णहा जइ(!strcmp(argv[i], "--test-config")){
			config->test_configuration = true;
		}अण्णहा{
			fprintf(stderr, "Error: Unknown option '%s'.\n", argv[i]);
			print_usage();
			प्रत्यागम MOSQ_ERR_INVAL;
		}
	}

	/* Default to drop to mosquitto user if we are privileged and no user specified. */
	जइ(!config->user){
		config->user = mosquitto_strdup("mosquitto");
		जइ(config->user == रिक्त){
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
	}
	जइ(db.quiet){
		config->log_type = 0;
	}अण्णहा जइ(db.verbose){
		config->log_type = UINT_MAX;
	}

	जइ(getenv("MOSQUITTO_PERSISTENCE_LOCATION")){
		mosquitto_FREE(config->persistence_location);
		config->persistence_location = mosquitto_strdup(getenv("MOSQUITTO_PERSISTENCE_LOCATION"));
		जइ(!config->persistence_location){
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
	}
	प्रत्यागम config__check(config);
}


स्थिर शून्य config__copy(संरचना mosquitto__config *src, संरचना mosquitto__config *dest)
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
	dest->security_options.plugin_गणनम् = src->security_options.plugin_गणनम्;
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

#जइ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	dest->websockets_log_level = src->websockets_log_level;
#endif

#ifdef WITH_BRIDGE
	प्रति(पूर्णाङ्क i=0; i<dest->bridge_गणनम्; i++){
		जइ(dest->bridges[i]){
			config__bridge_cleanup(dest->bridges[i]);
		}
	}
	mosquitto_FREE(dest->bridges);
	dest->bridges = src->bridges;
	dest->bridge_गणनम् = src->bridge_गणनम्;
#endif
}


पूर्णाङ्क config__read(संरचना mosquitto__config *config, bool reload)
{
	पूर्णाङ्क rc = MOSQ_ERR_SUCCESS;
	संरचना config_recurse cr;
	पूर्णाङ्क lineno = 0;
#ifdef WITH_PERSISTENCE
	size_t len;
#endif
	संरचना mosquitto__config config_reload;
	पूर्णाङ्क i;

	जइ(reload){
		memset(&config_reload, 0, परिमाण(संरचना mosquitto__config));
	}

	cr.log_dest = MQTT3_LOG_NONE;
	cr.log_dest_set = 0;
	cr.log_type = MOSQ_LOG_NONE;
	cr.log_type_set = 0;

	जइ(!db.config_file){
		प्रत्यागम 0;
	}

	जइ(reload){
		/* Re-initialise appropriate config vars to default for reload. */
		config__init_reload(&config_reload);
		config_reload.listeners = config->listeners;
		config_reload.listener_गणनम् = config->listener_गणनम्;
		cur_security_options = रिक्त;
		rc = config__read_file(&config_reload, reload, db.config_file, &cr, 0, &lineno);
	}अण्णहा{
		rc = config__read_file(config, reload, db.config_file, &cr, 0, &lineno);
	}
	जइ(rc){
		जइ(lineno > 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error found at %s:%d.", db.config_file, lineno);
		}
		प्रत्यागम rc;
	}

	जइ(reload){
		config__copy(&config_reload, config);
	}

	/* If auth/access options are set and allow_anonymous not explicitly set, disallow anon. */
	जइ(config->local_only == false){
		जइ(config->per_listener_settings){
			प्रति(i=0; i<config->listener_गणनम्; i++){
				/* Default option if no security options set */
				जइ(config->listeners[i].security_options->allow_anonymous == -1){
					config->listeners[i].security_options->allow_anonymous = false;
				}
			}
		}अण्णहा{
			जइ(config->security_options.allow_anonymous == -1){
				config->security_options.allow_anonymous = false;
			}
		}
	}
#ifdef WITH_PERSISTENCE
	जइ(config->persistence){
		जइ(!config->persistence_file){
			config->persistence_file = mosquitto_strdup("mosquitto.db");
			जइ(!config->persistence_file){
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
		}
		mosquitto_FREE(config->persistence_filepath);
		जइ(config->persistence_location && strlen(config->persistence_location)){
			len = strlen(config->persistence_location) + strlen(config->persistence_file) + 2;
			config->persistence_filepath = mosquitto_malloc(len);
			जइ(!config->persistence_filepath){
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
#ifdef WIN32
			snprintf(config->persistence_filepath, len, "%s\\%s", config->persistence_location, config->persistence_file);
#अण्णहा
			snprintf(config->persistence_filepath, len, "%s/%s", config->persistence_location, config->persistence_file);
#endif
		}अण्णहा{
			config->persistence_filepath = mosquitto_strdup(config->persistence_file);
			जइ(!config->persistence_filepath){
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
		}
	}
#endif
	/* Default to drop to mosquitto user if no other user specified. This must
	 * remain here even though it is covered in config__parse_args() because this
	 * function may be called on its own. */
	जइ(!config->user){
		config->user = mosquitto_strdup("mosquitto");
	}

#ifdef WITH_BRIDGE
	प्रति(i=0; i<config->bridge_गणनम्; i++){
		जइ(!config->bridges[i]->name){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: bridge name not defined.");
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		जइ(config->bridges[i]->addresses  == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no remote addresses defined.");
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		जइ(config->bridges[i]->topic_गणनम् == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no topics defined.");
			प्रत्यागम MOSQ_ERR_INVAL;
		}
#ifdef FINAL_WITH_TLS_PSK
		जइ(config->bridges[i]->tls_psk && !config->bridges[i]->tls_psk_identity){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_identity.");
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		जइ(config->bridges[i]->tls_psk_identity && !config->bridges[i]->tls_psk){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_psk.");
			प्रत्यागम MOSQ_ERR_INVAL;
		}
#endif
	}
#endif

	जइ(cr.log_dest_set){
		config->log_dest = cr.log_dest;
	}
	जइ(db.quiet){
		config->log_type = 0;
	}अण्णहा जइ(db.verbose){
		config->log_type = UINT_MAX;
	}अण्णहा जइ(cr.log_type_set){
		config->log_type = cr.log_type;
	}
#ifdef WITH_BRIDGE
	प्रत्यागम config__check_bridges(config);
#अण्णहा
	प्रत्यागम MOSQ_ERR_SUCCESS;
#endif
}


स्थिर mosquitto_plugin_id_t *config__plugin_find(ध्रुव वर्ण *name)
{
	जइ(db.plugins && name){
		प्रति(पूर्णाङ्क i=0; i<db.plugin_गणनम्; i++){
			जइ(db.plugins[i]->config.name && !strcmp(db.plugins[i]->config.name, name)){
				प्रत्यागम db.plugins[i];
			}
		}
	}
	प्रत्यागम रिक्त;
}


स्थिर mosquitto_plugin_id_t *config__plugin_load(ध्रुव वर्ण *name, ध्रुव वर्ण *path)
{
	mosquitto_plugin_id_t *plugin = रिक्त;
	mosquitto_plugin_id_t **plugins = रिक्त;

	जइ(name && config__plugin_find(name)){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Duplicate plugin name '%s'.", name);
		प्रत्यागम रिक्त;
	}

	जइ(!path || !strcmp(path, "")){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Missing plugin path for plugin name '%s'.", name);
		प्रत्यागम रिक्त;
	}

	plugin = mosquitto_calloc(1, परिमाण(mosquitto_plugin_id_t));
	जइ(!plugin){
		गच्छ error;
	}

	जइ(name){
		plugin->config.name = mosquitto_strdup(name);
	}
	plugin->config.path = mosquitto_strdup(path);
	जइ((name && !plugin->config.name) || !plugin->config.path){
		गच्छ error;
	}
	plugin->config.options = रिक्त;
	plugin->config.option_गणनम् = 0;
	plugin->config.deny_special_chars = true;

	/* Add to db list */
	plugins = mosquitto_realloc(db.plugins, (size_t)(db.plugin_गणनम्+1)*परिमाण(संरचना mosquitto__plugin_config *));
	जइ(!plugins){
		गच्छ error;
	}

	plugins[db.plugin_गणनम्] = plugin;
	db.plugins = plugins;
	db.plugin_गणनम्++;

	प्रत्यागम plugin;
error:
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
	जइ(plugin){
		mosquitto_FREE(plugin->config.name);
		mosquitto_FREE(plugin->config.path);
	}
	mosquitto_FREE(plugin);
	प्रत्यागम रिक्त;
}


पूर्णाङ्क config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, संरचना mosquitto__security_options *security_options)
{
	संरचना mosquitto__security_options **new_options;
	mosquitto_plugin_id_t **new_plugins;

	new_options = mosquitto_realloc(plugin->config.security_options, (size_t)(plugin->config.security_option_गणनम्+1)*परिमाण(संरचना mosquitto__security_options *));
	new_plugins = mosquitto_realloc(security_options->plugins, (size_t)(security_options->plugin_गणनम्+1)*परिमाण(mosquitto_plugin_id_t *));

	जइ(!new_options || !new_plugins){
		mosquitto_FREE(new_options);
		mosquitto_FREE(new_plugins);
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	new_options[plugin->config.security_option_गणनम्] = security_options;
	plugin->config.security_options = new_options;
	plugin->config.security_option_गणनम्++;

	new_plugins[security_options->plugin_गणनम्] = plugin;
	security_options->plugins = new_plugins;
	security_options->plugin_गणनम्++;

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क config__read_file_core(संरचना mosquitto__config *config, bool reload, संरचना config_recurse *cr, पूर्णाङ्क level, पूर्णाङ्क *lineno, FILE *fptr, वर्ण **buf, पूर्णाङ्क *buflen)
{
	पूर्णाङ्क rc;
	वर्ण *token;
	पूर्णाङ्क tmp_int;
	वर्ण *saveptr = रिक्त;
#ifdef WITH_BRIDGE
	वर्ण *tmp_char;
	संरचना mosquitto__bridge *cur_bridge = रिक्त;
#endif
	mosquitto_plugin_id_t *cur_plugin = रिक्त;

	वर्ण *कुञ्चिका;
	संरचना mosquitto__listener *cur_listener = रिक्त;
	पूर्णाङ्क i;
	पूर्णाङ्क lineno_ext = 0;
	size_t prefix_len;
	size_t slen;
#ifdef WITH_WEBSOCKETS
	वर्ण **ws_origins = रिक्त;
#endif

	*lineno = 0;

	जाव(mosquitto_fgets(buf, buflen, fptr)){
		(*lineno)++;
		जइ((*buf)[0] != '#' && (*buf)[0] != 10 && (*buf)[0] != 13){
			slen = strlen(*buf);
			जइ(slen == 0){
				अनुवर्त;
			}
			जाव((*buf)[slen-1] == 10 || (*buf)[slen-1] == 13){
				(*buf)[slen-1] = 0;
				slen = strlen(*buf);
				जइ(slen == 0){
					अनुवर्त;
				}
			}
			token = strtok_r((*buf), " ", &saveptr);
			जइ(token){
				जइ(!strcmp(token, "accept_protocol_versions")){
					REQUIRE_NON_DEFAULT_LISTENER(token);
					cur_listener->disable_protocol_v3 = true;
					cur_listener->disable_protocol_v4 = true;
					cur_listener->disable_protocol_v5 = true;
					जइ(saveptr == रिक्त){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", "accept_protocol_versions");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					token = strtok_r(saveptr, ", \t", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "accept_protocol_versions");

					जाव(token){
						जइ(!strcmp(token, "3")){
							cur_listener->disable_protocol_v3 = false;
						}अण्णहा जइ(!strcmp(token, "4")){
							cur_listener->disable_protocol_v4 = false;
						}अण्णहा जइ(!strcmp(token, "5")){
							cur_listener->disable_protocol_v5 = false;
						}

						token = strtok_r(रिक्त, ", \t", &saveptr);
					}
				}अण्णहा जइ(!strcmp(token, "acl_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);

					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->acl_data.acl_file);
					जइ(conf__parse_string(&token, "acl_file", &cur_security_options->acl_data.acl_file, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "address") || !strcmp(token, "addresses")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					जइ(cur_bridge->addresses){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge configuration, 'address' only allowed once.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जाव((token = strtok_r(रिक्त, " ", &saveptr))){
						जइ(token[0] == '#'){
							भङ्ग;
						}
						संरचना bridge_address *new_addresses = mosquitto_realloc(cur_bridge->addresses, परिमाण(संरचना bridge_address)*(size_t)(cur_bridge->address_गणनम्+1));
						जइ(!new_addresses){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
							प्रत्यागम MOSQ_ERR_NOMEM;
						}
						cur_bridge->address_गणनम्++;
						cur_bridge->addresses = new_addresses;
						memset(&cur_bridge->addresses[cur_bridge->address_गणनम्-1], 0, परिमाण(संरचना bridge_address));
						cur_bridge->addresses[cur_bridge->address_गणनम्-1].address = mosquitto_strdup(token);
						जइ(!cur_bridge->addresses[cur_bridge->address_गणनम्-1].address){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
							प्रत्यागम MOSQ_ERR_NOMEM;
						}
					}
					प्रति(i=0; i<cur_bridge->address_गणनम्; i++){
						/* cur_bridge->addresses[i].address is now
						 * "address[:port]". If address is an IPv6 address,
						 * then port is required. We must check for the :
						 * backwards. */
						tmp_char = strrchr(cur_bridge->addresses[i].address, ':');
						जइ(tmp_char){
							/* Remove ':', so cur_bridge->addresses[i].address
							 * now just looks like the address. */
							tmp_char[0] = '\0';

							/* The remainder of the string */
							tmp_int = atoi(&tmp_char[1]);
							जइ(tmp_int < 1 || tmp_int > UINT16_MAX){
								log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge port value (%d).", tmp_int);
								प्रत्यागम MOSQ_ERR_INVAL;
							}
							cur_bridge->addresses[i].port = (uint16_t)tmp_int;
						}अण्णहा{
							cur_bridge->addresses[i].port = 1883;
						}
						conf__attempt_resolve(cur_bridge->addresses[i].address, "bridge address", MOSQ_LOG_WARNING, "Warning");
					}
					जइ(cur_bridge->address_गणनम् == 0){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty address value in configuration.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "allow_anonymous")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					PER_LISTENER_ALTERNATIVE(token, "listener_allow_anonymous");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					जइ(conf__parse_bool(&token, "allow_anonymous", (bool *)&cur_security_options->allow_anonymous, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "allow_duplicate_messages")){
					OPTION_DEPRECATED(token, "The behaviour will default to true.");
					जइ(conf__parse_bool(&token, "allow_duplicate_messages", &config->allow_duplicate_messages, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "allow_zero_length_clientid")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					जइ(conf__parse_bool(&token, "allow_zero_length_clientid", &cur_security_options->allow_zero_length_clientid, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strncmp(token, "auth_opt_", strlen("auth_opt_")) || !strncmp(token, "plugin_opt_", strlen("plugin_opt_"))){
					जइ(reload){
						अनुवर्त;        /* Auth plugin not currently valid for reloading. */

					}
					REQUIRE_PLUGIN(token);

					जइ(!strncmp(token, "auth_opt_", strlen("auth_opt_"))){
						prefix_len = strlen("auth_opt_");
					}अण्णहा{
						prefix_len = strlen("plugin_opt_");
					}
					जइ(strlen(token) < prefix_len + 3){
						/* auth_opt_ == 9, + one digit key == 10, + one space == 11, + one value == 12 */
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'plugin_opt_' config option.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					कुञ्चिका = mosquitto_strdup(&token[prefix_len]);
					जइ(!कुञ्चिका){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
						प्रत्यागम MOSQ_ERR_NOMEM;
					}अण्णहा जइ(STREMPTY(कुञ्चिका)){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty 'plugin_opt_' config option.");
						mosquitto_FREE(कुञ्चिका);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					token = saveptr;
					जइ(token && token[0]){
						जाव(token[0] == ' ' || token[0] == '\t'){
							token++;
						}
						संरचना mosquitto_opt *new_options = mosquitto_realloc(cur_plugin->config.options, (size_t)(cur_plugin->config.option_गणनम्+1)*परिमाण(संरचना mosquitto_opt));
						जइ(!new_options){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
							mosquitto_FREE(कुञ्चिका);
							प्रत्यागम MOSQ_ERR_NOMEM;
						}
						cur_plugin->config.option_गणनम्++;
						cur_plugin->config.options = new_options;
						cur_plugin->config.options[cur_plugin->config.option_गणनम्-1].कुञ्चिका = कुञ्चिका;
						cur_plugin->config.options[cur_plugin->config.option_गणनम्-1].मानम् = mosquitto_strdup(token);
						जइ(!cur_plugin->config.options[cur_plugin->config.option_गणनम्-1].मानम्){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
							प्रत्यागम MOSQ_ERR_NOMEM;
						}
					}अण्णहा{
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", कुञ्चिका);
						mosquitto_FREE(कुञ्चिका);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "auth_plugin") || !strcmp(token, "plugin") || !strcmp(token, "global_plugin")){
					जइ(reload){
						अनुवर्त;        /* plugin not currently valid for reloading. */
					}
					जइ(!strcmp(token, "global_plugin")){
						cur_security_options = &db.config->security_options;
					}अण्णहा{
						REQUIRE_LISTENER_IF_PER_LISTENER(token);
						conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					}

					cur_plugin = config__plugin_load(रिक्त, saveptr);
					जइ(cur_plugin == रिक्त){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(config__plugin_add_secopt(cur_plugin, cur_security_options)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "auth_plugin_deny_special_chars")){
					जइ(reload){
						अनुवर्त;        /* Auth plugin not currently valid for reloading. */
					}
					जइ(!cur_plugin){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: An auth_plugin_deny_special_chars option exists in the config file without a plugin.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(conf__parse_bool(&token, "auth_plugin_deny_special_chars", &cur_plugin->config.deny_special_chars, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "plugin_load")){
					वर्ण *name = रिक्त;

					जइ(reload){
						अनुवर्त;        /* plugin not currently valid for reloading. */

					}
					name = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(name, "plugin_load");

					cur_plugin = config__plugin_load(name, saveptr);
					जइ(cur_plugin == रिक्त){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "plugin_use")){
					वर्ण *name = रिक्त;

					जइ(reload){
						अनुवर्त;        /* plugin not currently valid for reloading. */
					}
					REQUIRE_NON_DEFAULT_LISTENER(token);

					जइ(conf__parse_string(&token, "plugin_use", &name, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					cur_plugin = config__plugin_find(name);
					जइ(!cur_plugin){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Plugin '%s' not previously loaded.", name);
						mosquitto_FREE(name);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(name);
					जइ(config__plugin_add_secopt(cur_plugin, cur_listener->security_options)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "auto_id_prefix")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					OPTION_DEPRECATED(token, "Please use 'listener_auto_id_prefix' instead.");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					जइ(conf__parse_string(&token, "auto_id_prefix", &cur_security_options->auto_id_prefix, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(cur_security_options->auto_id_prefix){
						जइ(strlen(cur_security_options->auto_id_prefix) > 50){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: auto_id_prefix length must be <= 50.");
							प्रत्यागम MOSQ_ERR_INVAL;
						}
						cur_security_options->auto_id_prefix_len = (uint16_t)strlen(cur_security_options->auto_id_prefix);
					}अण्णहा{
						cur_security_options->auto_id_prefix_len = 0;
					}
				}अण्णहा जइ(!strcmp(token, "autosave_interval")){
					जइ(conf__parse_int(&token, "autosave_interval", &config->autosave_interval, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(config->autosave_interval < 0){
						config->autosave_interval = 0;
					}
				}अण्णहा जइ(!strcmp(token, "autosave_on_changes")){
					जइ(conf__parse_bool(&token, "autosave_on_changes", &config->autosave_on_changes, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "bind_address")){
					OPTION_DEPRECATED(token, "");
					config->local_only = false;
					जइ(reload){
						अनुवर्त;        /* Rebinding listeners not valid during reloading. */

					}
					जइ(config__create_default_listener(config, token)){
						प्रत्यागम MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					जइ(conf__parse_string(&token, "default listener bind_address", &config->default_listener->host, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(conf__attempt_resolve(config->default_listener->host, "bind_address", MOSQ_LOG_ERR, "Error")){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "bind_interface")){
#ifdef SO_BINDTODEVICE
					जइ(reload){
						अनुवर्त;        /* Rebinding listeners not valid during reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					जइ(conf__parse_string(&token, "bind_interface", &cur_listener->bind_interface, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_ERR, "Error: bind_interface specified but socket option not available.");
					प्रत्यागम MOSQ_ERR_INVAL;
#endif
				}अण्णहा जइ(!strcmp(token, "bridge_attempt_unsubscribe")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_bool(&token, "bridge_attempt_unsubscribe", &cur_bridge->attempt_unsubscribe, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "bridge_cafile")){
#जइ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					जइ(conf__parse_string(&token, "bridge_cafile", &cur_bridge->tls_cafile, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "bridge_alpn")){
#जइ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_string(&token, "bridge_alpn", &cur_bridge->tls_alpn, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "bridge_ciphers")){
#जइ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_string(&token, "bridge_ciphers", &cur_bridge->tls_ciphers, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "bridge_ciphers_tls1.3")){
#जइ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_string(&token, "bridge_ciphers_tls1.3", &cur_bridge->tls_13_ciphers, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "bridge_bind_address")){
#जइ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_string(&token, "bridge_bind_address", &cur_bridge->bind_address, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "bridge_capath")){
#जइ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					जइ(conf__parse_string(&token, "bridge_capath", &cur_bridge->tls_capath, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "bridge_certfile")){
#जइ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					जइ(conf__parse_string(&token, "bridge_certfile", &cur_bridge->tls_certfile, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "bridge_identity")){
#जइ defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					जइ(conf__parse_string(&token, "bridge_identity", &cur_bridge->tls_psk_identity, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "bridge_insecure")){
#जइ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_bool(&token, "bridge_insecure", &cur_bridge->tls_insecure, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(cur_bridge->tls_insecure){
						log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge '%s' using insecure mode.", cur_bridge->name);
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "bridge_require_ocsp")){
#जइ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_bool(&token, "bridge_require_ocsp", &cur_bridge->tls_ocsp_required, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "bridge_max_packet_size")){
#जइ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_int(&token, "bridge_max_packet_size", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(tmp_int < 0){
						tmp_int = 0;
					}
					cur_bridge->maximum_packet_size = (uint32_t)tmp_int;
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "bridge_max_topic_alias")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_int(&token, "bridge_max_topic_alias", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}

					जइ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: bridge_max_topic_alias must be > 0 and <= 65535.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					cur_bridge->max_topic_alias = (uint16_t)tmp_int;
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "bridge_outgoing_retain")){
#जइ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_bool(&token, "bridge_outgoing_retain", &cur_bridge->outgoing_retain, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "bridge_keyfile")){
#जइ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					mosquitto_FREE(cur_bridge->tls_keyfile);
					जइ(conf__parse_string(&token, "bridge_keyfile", &cur_bridge->tls_keyfile, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "bridge_protocol_version")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(रिक्त, "", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_protocol_version");

					जइ(!strcmp(token, "mqttv31")){
						cur_bridge->protocol_version = mosq_p_mqtt31;
					}अण्णहा जइ(!strcmp(token, "mqttv311")){
						cur_bridge->protocol_version = mosq_p_mqtt311;
					}अण्णहा जइ(!strcmp(token, "mqttv50")){
						cur_bridge->protocol_version = mosq_p_mqtt5;
					}अण्णहा{
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'bridge_protocol_version' value (%s).", token);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "bridge_psk")){
#जइ defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					जइ(conf__parse_string(&token, "bridge_psk", &cur_bridge->tls_psk, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "bridge_receive_maximum")){
#जइ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_int(&token, "bridge_receive_maximum", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(tmp_int <= 0){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be greater than 0.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}अण्णहा जइ((uint64_t)tmp_int > (uint64_t)UINT16_MAX){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be lower than %u.", UINT16_MAX);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					cur_bridge->receive_maximum = (uint16_t)tmp_int;
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "bridge_reload_type")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_reload_type");

					जइ(!strcmp(token, "lazy")){
						cur_bridge->reload_type = brt_lazy;
					}अण्णहा जइ(!strcmp(token, "immediate")){
						cur_bridge->reload_type = brt_immediate;
					}अण्णहा{
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'bridge_reload_type' value in configuration (%s).", token);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "bridge_session_expiry_interval")){
#जइ defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_int(&token, "bridge_session_expiry_interval", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(tmp_int < 0){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must not be negative.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}अण्णहा जइ((uint64_t)tmp_int > (uint64_t)MQTT_SESSION_EXPIRY_NEVER){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must be lower than %u.", MQTT_SESSION_EXPIRY_NEVER);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					cur_bridge->session_expiry_interval = (uint32_t)tmp_int;
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "bridge_tcp_keepalive")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);

					जइ(conf__parse_int(&token, "bridge_tcp_keepalive_idle", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(tmp_int <= 0){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: invalid TCP keepalive idle value.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_idle = (unsigned पूर्णाङ्क)tmp_int;

					जइ(conf__parse_int(&token, "bridge_tcp_keepalive_interval", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(tmp_int <= 0){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: invalid TCP keepalive interval value.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_interval = (unsigned पूर्णाङ्क)tmp_int;

					जइ(conf__parse_int(&token, "bridge_tcp_keepalive_counter", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(tmp_int <= 0){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: invalid TCP keepalive counter value.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_counter = (unsigned पूर्णाङ्क)tmp_int;
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "bridge_tcp_user_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
#ifdef WITH_TCP_USER_TIMEOUT
					जइ(conf__parse_int(&token, "bridge_tcp_user_timeout", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(tmp_int < 0){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: invalid TCP user timeout value.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_user_timeout = tmp_int;
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge TCP user timeout support not available.");
#endif
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "bridge_tls_use_os_certs")){
#जइ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_bool(&token, "bridge_tls_use_os_certs", &cur_bridge->tls_use_os_certs, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "bridge_tls_version")){
#जइ defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_string(&token, "bridge_tls_version", &cur_bridge->tls_version, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "cafile")){
#जइ defined(WITH_TLS)
					REQUIRE_LISTENER(token);
					जइ(cur_listener->psk_hint){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->cafile);
					जइ(conf__parse_string(&token, "cafile", &cur_listener->cafile, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "capath")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->capath);
					जइ(conf__parse_string(&token, "capath", &cur_listener->capath, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "certfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					जइ(cur_listener->psk_hint){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->certfile);
					जइ(conf__parse_string(&token, "certfile", &cur_listener->certfile, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "check_retain_source")){
					जइ(conf__parse_bool(&token, "check_retain_source", &config->check_retain_source, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "ciphers")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers);
					जइ(conf__parse_string(&token, "ciphers", &cur_listener->ciphers, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "ciphers_tls1.3")){
#जइ defined(WITH_TLS) && (!defined(LIBRESSL_VERSION_सङ्ख्या) || LIBRESSL_VERSION_सङ्ख्या > 0x3040000FL)
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers_tls13);
					जइ(conf__parse_string(&token, "ciphers_tls1.3", &cur_listener->ciphers_tls13, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: ciphers_tls1.3 support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "clientid") || !strcmp(token, "remote_clientid")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_string(&token, "bridge remote clientid", &cur_bridge->remote_clientid, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "cleansession")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_bool(&token, "cleansession", &cur_bridge->clean_start, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "local_cleansession")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_bool(&token, "local_cleansession", (bool *)&cur_bridge->clean_start_local, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "clientid_prefixes")){
					OPTION_DEPRECATED(token, "");
					mosquitto_FREE(config->clientid_prefixes);
					जइ(conf__parse_string(&token, "clientid_prefixes", &config->clientid_prefixes, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "connection")){
#ifdef WITH_BRIDGE
					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "connection");

					/* Check for existing bridge name. */
					प्रति(i=0; i<config->bridge_गणनम्; i++){
						जइ(!strcmp(config->bridges[i]->name, token)){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Duplicate bridge name '%s'.", token);
							प्रत्यागम MOSQ_ERR_INVAL;
						}
					}

					संरचना mosquitto__bridge **bridges_new = mosquitto_realloc(config->bridges, (size_t)(config->bridge_गणनम्+1)*परिमाण(संरचना mosquitto__bridge *));
					जइ(!bridges_new){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
						प्रत्यागम MOSQ_ERR_NOMEM;
					}
					config->bridges = bridges_new;
					cur_bridge = mosquitto_malloc(परिमाण(संरचना mosquitto__bridge));
					जइ(!cur_bridge){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
						प्रत्यागम MOSQ_ERR_NOMEM;
					}

					config->bridge_गणनम्++;
					config->bridges[config->bridge_गणनम्-1] = cur_bridge;

					memset(cur_bridge, 0, परिमाण(संरचना mosquitto__bridge));
					cur_bridge->name = mosquitto_strdup(token);
					जइ(!cur_bridge->name){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
						प्रत्यागम MOSQ_ERR_NOMEM;
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
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "connection_messages")){
					जइ(conf__parse_bool(&token, token, &config->connection_messages, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "crlfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->crlfile);
					जइ(conf__parse_string(&token, "crlfile", &cur_listener->crlfile, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "dhparamfile")){
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: dhparamfile is no longer required.");
				}अण्णहा जइ(!strcmp(token, "disable_client_cert_date_checks")){
#ifdef WITH_TLS
					REQUIRE_LISTENER(token);
					जइ(conf__parse_bool(&token, "disable_client_cert_date_checks", &cur_listener->disable_client_cert_date_checks, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "enable_control_api")){
#ifdef WITH_CONTROL
					जइ(conf__parse_bool(&token, "enable_control_api", &config->enable_control_api, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: $CONTROL support not available (enable_control_api).");
#endif
				}अण्णहा जइ(!strcmp(token, "enable_proxy_protocol")){
#जइ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(रिक्त, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					प्रत्यागम MOSQ_ERR_INVAL;
#endif
					REQUIRE_LISTENER(token);
					जइ(conf__parse_int(&token, "enable_proxy_protocol", &cur_listener->enable_proxy_protocol, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(cur_listener->enable_proxy_protocol < 1 || cur_listener->enable_proxy_protocol > 2){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: enable_proxy_protocol must be 1 or 2.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "global_max_clients")){
					जइ(conf__parse_int(&token, "global_max_clients", &config->global_max_clients, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "global_max_connections")){
					जइ(conf__parse_int(&token, "global_max_connections", &config->global_max_connections, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "http_dir")){
#जइ defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
					जइ(reload){
						अनुवर्त;        /* Not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					जइ(conf__parse_string(&token, "http_dir", &cur_listener->http_dir, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#ifdef WIN32
					वर्ण *http_dir_canonical = _fullpath(रिक्त, cur_listener->http_dir, 0);
					ध्रुव वर्ण sep = '\\';
#अण्णहा
					वर्ण *http_dir_canonical = realpath(cur_listener->http_dir, रिक्त);
					ध्रुव वर्ण sep = '/';
#endif
					जइ(!http_dir_canonical){
						प्रत्यागम MOSQ_ERR_NOMEM;
					}
					size_t http_dir_len = strlen(http_dir_canonical) + परिमाण(sep) + 1;
					वर्ण *http_dir_canonical_sep = mosquitto_calloc(http_dir_len, परिमाण(वर्ण));
					जइ(!http_dir_canonical_sep){
						मुक्त(http_dir_canonical);
						प्रत्यागम MOSQ_ERR_NOMEM;
					}
					snprintf(http_dir_canonical_sep, http_dir_len, "%s%c", http_dir_canonical, sep);
					मुक्त(http_dir_canonical);
					mosquitto_FREE(cur_listener->http_dir);
					cur_listener->http_dir = http_dir_canonical_sep;
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: http_dir support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "idle_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_int(&token, "idle_timeout", &cur_bridge->idle_timeout, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(cur_bridge->idle_timeout < 1){
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "idle_timeout interval too low, using 1 second.");
						cur_bridge->idle_timeout = 1;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "include_dir")){
					जइ(level == 0){
						वर्ण **files;
						पूर्णाङ्क file_गणनम्;

						/* Only process include_dir from the main config file. */
						token = strtok_r(रिक्त, "", &saveptr);
						REQUIRE_NON_EMPTY_OPTION(token, "include_dir");

						rc = config__get_dir_files(token, &files, &file_गणनम्);
						जइ(rc){
							प्रत्यागम rc;
						}

						प्रति(i=0; i<file_गणनम्; i++){
							log__printf(रिक्त, MOSQ_LOG_INFO, "Loading config file '%s'", files[i]);

							rc = config__read_file(config, reload, files[i], cr, level+1, &lineno_ext);
							जइ(rc){
								जइ(lineno_ext > 0){
									log__printf(रिक्त, MOSQ_LOG_ERR, "Error found at '%s:%d'.", files[i], lineno_ext);
								}
								/* Free happens below */
								भङ्ग;
							}
						}
						प्रति(i=0; i<file_गणनम्; i++){
							mosquitto_FREE(files[i]);
						}
						mosquitto_FREE(files);
						जइ(rc){
							प्रत्यागम rc;    /* This returns if config__read_file() fails above */
						}
					}अण्णहा{
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: The include_dir option is only valid in the main configuration file.");
						प्रत्यागम 1;
					}
				}अण्णहा जइ(!strcmp(token, "keepalive_interval")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_int(&token, "keepalive_interval", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(tmp_int > UINT16_MAX){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Bridge keepalive value too high.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(tmp_int < 5){
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "keepalive interval too low, using 5 seconds.");
						tmp_int = 5;
					}
					cur_bridge->keepalive = (uint16_t)tmp_int;
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "keyfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->keyfile);
					जइ(conf__parse_string(&token, "keyfile", &cur_listener->keyfile, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "listener")){
					config->local_only = false;

					जइ(conf__parse_int(&token, "listener port", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#ifdef WITH_UNIX_SOCKETS
					जइ(tmp_int < 0 || tmp_int > UINT16_MAX){
#अण्णहा
					जइ(tmp_int < 1 || tmp_int > UINT16_MAX){
#endif
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						प्रत्यागम MOSQ_ERR_INVAL;
					}

					/* Look for bind address / unix socket path */
					token = strtok_r(रिक्त, " ", &saveptr);
					जइ(token != रिक्त && token[0] == '#'){
						token = रिक्त;
					}

					जइ(tmp_int == 0 && token == रिक्त){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: A listener with port 0 must provide a Unix socket path.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}

					जइ(reload){
						/* We reload listeners settings based on port number/unix socket path.
						 * If the port number/unix path doesn't already exist, exit with a complaint. */
						cur_listener = रिक्त;
#ifdef WITH_UNIX_SOCKETS
						जइ(tmp_int == 0){
							प्रति(i=0; i<config->listener_गणनम्; i++){
								जइ(config->listeners[i].unix_socket_path != रिक्त
										&& strcmp(config->listeners[i].unix_socket_path, token) == 0){

									cur_listener = &config->listeners[i];
									भङ्ग;
								}
							}
						}अण्णहा
#endif
						{
							प्रति(i=0; i<config->listener_गणनम्; i++){
								जइ(config->listeners[i].port == tmp_int){
									/* Now check we have a matching bind address, if defined */
									जइ(config->listeners[i].host){
										जइ(token && !strcmp(config->listeners[i].host, token)){
											/* They both have a bind address, and they match */
											cur_listener = &config->listeners[i];
											भङ्ग;
										}
									}अण्णहा{
										जइ(token == रिक्त){
											/* Neither this config nor the new config have a bind address,
											 * so they match. */
											cur_listener = &config->listeners[i];
											भङ्ग;
										}
									}
								}
							}
						}
						जइ(!cur_listener){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: It is not currently possible to add/remove listeners when reloading the config file.");
							प्रत्यागम MOSQ_ERR_INVAL;
						}
					}अण्णहा{
						जइ(config__add_listener(config)){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
							प्रत्यागम MOSQ_ERR_NOMEM;
						}
						cur_listener = &config->listeners[config->listener_गणनम्-1];
					}

					listener__set_defaults(cur_listener);
					cur_listener->port = (uint16_t)tmp_int;

					mosquitto_FREE(cur_listener->host);

#ifdef WITH_UNIX_SOCKETS
					mosquitto_FREE(cur_listener->unix_socket_path);
#endif

					जइ(token){
#ifdef WITH_UNIX_SOCKETS
						जइ(cur_listener->port == 0){
							cur_listener->unix_socket_path = mosquitto_strdup(token);
						}अण्णहा
#endif
						{
							cur_listener->host = mosquitto_strdup(token);
						}
					}
				}अण्णहा जइ(!strcmp(token, "listener_allow_anonymous")){
					REQUIRE_LISTENER(token);
					जइ(conf__parse_bool(&token, "listener_allow_anonymous", (bool *)&cur_listener->security_options->allow_anonymous, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "listener_auto_id_prefix")){
					REQUIRE_LISTENER(token);
					जइ(conf__parse_string(&token, "listener_auto_id_prefix", &cur_listener->security_options->auto_id_prefix, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(cur_listener->security_options->auto_id_prefix){
						जइ(strlen(cur_listener->security_options->auto_id_prefix) > 50){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: 'listener_auto_id_prefix' length must be <= 50.");
							प्रत्यागम MOSQ_ERR_INVAL;
						}
						cur_listener->security_options->auto_id_prefix_len = (uint16_t)strlen(cur_listener->security_options->auto_id_prefix);
					}अण्णहा{
						cur_listener->security_options->auto_id_prefix_len = 0;
					}
				}अण्णहा जइ(!strcmp(token, "local_clientid")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_string(&token, "bridge local clientd", &cur_bridge->local_clientid, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "local_password")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_string(&token, "bridge local_password", &cur_bridge->local_password, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "local_username")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_string(&token, "bridge local_username", &cur_bridge->local_username, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "log_dest")){
					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_dest");

					cr->log_dest_set = 1;
					जइ(!strcmp(token, "none")){
						cr->log_dest = MQTT3_LOG_NONE;
					}अण्णहा जइ(!strcmp(token, "syslog")){
						cr->log_dest |= MQTT3_LOG_SYSLOG;
					}अण्णहा जइ(!strcmp(token, "stdout")){
						cr->log_dest |= MQTT3_LOG_STDOUT;
					}अण्णहा जइ(!strcmp(token, "stderr")){
						cr->log_dest |= MQTT3_LOG_STDERR;
					}अण्णहा जइ(!strcmp(token, "topic")){
						cr->log_dest |= MQTT3_LOG_TOPIC;
					}अण्णहा जइ(!strcmp(token, "dlt")){
						cr->log_dest |= MQTT3_LOG_DLT;
#ifdef ANDROID
					}अण्णहा जइ(!strcmp(token, "android")){
						cr->log_dest |= MQTT3_LOG_ANDROID;
#endif
					}अण्णहा जइ(!strcmp(token, "file")){
						जइ(config->log_fptr || config->log_file){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Duplicate \"log_dest file\" value.");
							प्रत्यागम MOSQ_ERR_INVAL;
						}
						/* Get remaining string. */
						token = saveptr;
						जइ(token && token[0]){
							जाव(token[0] == ' ' || token[0] == '\t'){
								token++;
							}
						}
						/* Duplicate "token" check here saves a log__printf() */
						जइ(token && token[0]){
							config->log_file = mosquitto_strdup(token);
							जइ(!config->log_file){
								log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
								प्रत्यागम MOSQ_ERR_NOMEM;
							}
							cr->log_dest |= MQTT3_LOG_FILE;
						}अण्णहा{
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty \"log_dest file\" value in configuration.");
							प्रत्यागम MOSQ_ERR_INVAL;
						}
						cr->log_dest |= MQTT3_LOG_FILE;
					}अण्णहा{
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'log_dest' value (%s).", token);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#जइ defined(WIN32) || defined(__CYGWIN__)
					जइ(service_handle){
						जइ(cr->log_dest == MQTT3_LOG_STDOUT || cr->log_dest == MQTT3_LOG_STDERR){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Cannot log to stdout/stderr when running as a Windows service.");
							प्रत्यागम MOSQ_ERR_INVAL;
						}
					}
#endif
				}अण्णहा जइ(!strcmp(token, "log_facility")){
#जइ defined(WIN32) || defined(__CYGWIN__)
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: log_facility not supported on Windows.");
#अण्णहा
					जइ(conf__parse_int(&token, "log_facility", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					विकल्प(tmp_int){
						पक्ष 0:
							config->log_facility = LOG_LOCAL0;
							भङ्ग;
						पक्ष 1:
							config->log_facility = LOG_LOCAL1;
							भङ्ग;
						पक्ष 2:
							config->log_facility = LOG_LOCAL2;
							भङ्ग;
						पक्ष 3:
							config->log_facility = LOG_LOCAL3;
							भङ्ग;
						पक्ष 4:
							config->log_facility = LOG_LOCAL4;
							भङ्ग;
						पक्ष 5:
							config->log_facility = LOG_LOCAL5;
							भङ्ग;
						पक्ष 6:
							config->log_facility = LOG_LOCAL6;
							भङ्ग;
						पक्ष 7:
							config->log_facility = LOG_LOCAL7;
							भङ्ग;
						स्वभाव:
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'log_facility' value (%d).", tmp_int);
							प्रत्यागम MOSQ_ERR_INVAL;
					}
#endif
				}अण्णहा जइ(!strcmp(token, "log_timestamp")){
					जइ(conf__parse_bool(&token, token, &config->log_timestamp, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "log_timestamp_format")){
					जइ(conf__parse_string(&token, token, &config->log_timestamp_format, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "log_type")){
					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_type");

					cr->log_type_set = 1;
					जइ(!strcmp(token, "none")){
						cr->log_type = MOSQ_LOG_NONE;
					}अण्णहा जइ(!strcmp(token, "information")){
						cr->log_type |= MOSQ_LOG_INFO;
					}अण्णहा जइ(!strcmp(token, "notice")){
						cr->log_type |= MOSQ_LOG_NOTICE;
					}अण्णहा जइ(!strcmp(token, "warning")){
						cr->log_type |= MOSQ_LOG_WARNING;
					}अण्णहा जइ(!strcmp(token, "error")){
						cr->log_type |= MOSQ_LOG_ERR;
					}अण्णहा जइ(!strcmp(token, "debug")){
						cr->log_type |= MOSQ_LOG_DEBUG;
					}अण्णहा जइ(!strcmp(token, "subscribe")){
						cr->log_type |= MOSQ_LOG_SUBSCRIBE;
					}अण्णहा जइ(!strcmp(token, "unsubscribe")){
						cr->log_type |= MOSQ_LOG_UNSUBSCRIBE;
					}अण्णहा जइ(!strcmp(token, "internal")){
						cr->log_type |= MOSQ_LOG_INTERNAL;
#ifdef WITH_WEBSOCKETS
					}अण्णहा जइ(!strcmp(token, "websockets")){
						cr->log_type |= MOSQ_LOG_WEBSOCKETS;
#endif
					}अण्णहा जइ(!strcmp(token, "all")){
						cr->log_type = MOSQ_LOG_ALL;
					}अण्णहा{
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'log_type' value (%s).", token);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "max_connections")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					जइ(conf__parse_int(&token, token, &cur_listener->max_connections, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(cur_listener->max_connections < 0){
						cur_listener->max_connections = -1;
					}
				}अण्णहा जइ(!strcmp(token, "maximum_qos") || !strcmp(token, "max_qos")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					जइ(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(tmp_int < 0 || tmp_int > 2){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: 'max_qos' must be between 0 and 2 inclusive.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					cur_listener->max_qos = (uint8_t)tmp_int;
				}अण्णहा जइ(!strcmp(token, "max_inflight_bytes")){
					जइ(conf__parse_int(&token, "max_inflight_bytes", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_inflight_bytes = (size_t)tmp_int;
				}अण्णहा जइ(!strcmp(token, "max_inflight_messages")){
					जइ(conf__parse_int(&token, "max_inflight_messages", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(tmp_int < 0 || tmp_int == UINT16_MAX){
						tmp_int = 0;
					}अण्णहा जइ(tmp_int > UINT16_MAX){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: 'max_inflight_messages' must be <= 65535.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					config->max_inflight_messages = (uint16_t)tmp_int;
				}अण्णहा जइ(!strcmp(token, "max_keepalive")){
					जइ(conf__parse_int(&token, "max_keepalive", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'max_keepalive' value (%d).", tmp_int);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					config->max_keepalive = (uint16_t)tmp_int;
				}अण्णहा जइ(!strcmp(token, "max_packet_size")){
					जइ(conf__parse_int(&token, "max_packet_size", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(tmp_int < 20){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: 'max_packet_size' must be >= 20.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					config->max_packet_size = (uint32_t)tmp_int;
				}अण्णहा जइ(!strcmp(token, "max_queued_bytes")){
					जइ(conf__parse_int(&token, "max_queued_bytes", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_bytes = (size_t)tmp_int;
				}अण्णहा जइ(!strcmp(token, "max_queued_messages")){
					जइ(conf__parse_int(&token, "max_queued_messages", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_messages = tmp_int;
				}अण्णहा जइ(!strcmp(token, "memory_limit")){
					ssize_t lim;
					जइ(conf__parse_ssize_t(&token, "memory_limit", &lim, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(lim < 0){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'memory_limit' value (%ld).", lim);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					mosquitto_memory_set_limit((size_t)lim);
				}अण्णहा जइ(!strcmp(token, "message_size_limit")){
					log__printf(रिक्त, MOSQ_LOG_NOTICE, "Note: It is recommended to replace `message_size_limit` with `max_packet_size`.");
					जइ(conf__parse_int(&token, "message_size_limit", (पूर्णाङ्क *)&config->message_size_limit, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(config->message_size_limit > MQTT_MAX_PAYLOAD){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'message_size_limit' value (%u).", config->message_size_limit);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "mount_point")){
					REQUIRE_LISTENER(token);
					mosquitto_FREE(cur_listener->mount_point);
					जइ(conf__parse_string(&token, "mount_point", &cur_listener->mount_point, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(mosquitto_pub_topic_check(cur_listener->mount_point) != MOSQ_ERR_SUCCESS){
						log__printf(रिक्त, MOSQ_LOG_ERR,
								"Error: Invalid 'mount_point' value (%s). Does it contain a wildcard character?",
								cur_listener->mount_point);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "notifications")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_bool(&token, "notifications", &cur_bridge->notifications, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "notifications_local_only")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_bool(&token, "notifications_local_only", &cur_bridge->notifications_local_only, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "notification_topic")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_string(&token, "notification_topic", &cur_bridge->notification_topic, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "password") || !strcmp(token, "remote_password")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_string(&token, "bridge remote_password", &cur_bridge->remote_password, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "password_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->password_data.password_file);
					जइ(conf__parse_string(&token, "password_file", &cur_security_options->password_data.password_file, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "per_listener_settings")){
					OPTION_DEPRECATED(token, "Please see the documentation for how to achieve the same effect.");
					जइ(config->per_listener_settings){
						/* Once this is set, don't let it be unset. It should be the first config option ideally. */
						अनुवर्त;
					}
					जइ(conf__parse_bool(&token, "per_listener_settings", &config->per_listener_settings, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(cur_security_options && config->per_listener_settings){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: per_listener_settings must be set before any other security settings.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "persistence") || !strcmp(token, "retained_persistence")){
					जइ(conf__parse_bool(&token, token, &config->persistence, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "persistence_file")){
					जइ(conf__parse_string(&token, "persistence_file", &config->persistence_file, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "persistence_location")){
					जइ(conf__parse_string(&token, "persistence_location", &config->persistence_location, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "persistent_client_expiration")){
					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "persistent_client_expiration");

					time_t expiration_mult;

					विकल्प(token[strlen(token)-1]){
						पक्ष 's':
							expiration_mult = 1;
							भङ्ग;
						पक्ष 'h':
							expiration_mult = 3600;
							भङ्ग;
						पक्ष 'd':
							expiration_mult = 86400;
							भङ्ग;
						पक्ष 'w':
							expiration_mult = 86400*7;
							भङ्ग;
						पक्ष 'm':
							expiration_mult = 86400*30;
							भङ्ग;
						पक्ष 'y':
							expiration_mult = 86400*365;
							भङ्ग;
						स्वभाव:
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
							प्रत्यागम MOSQ_ERR_INVAL;
					}
					token[strlen(token)-1] = '\0';
					config->persistent_client_expiration = atoi(token)*expiration_mult;
					जइ(config->persistent_client_expiration <= 0){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "pid_file")){
					जइ(reload){
						अनुवर्त;        /* pid file not valid for reloading. */
					}
					जइ(conf__parse_string(&token, "pid_file", &config->pid_file, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "port")){
					OPTION_DEPRECATED(token, "Please use 'listener' instead.");
					config->local_only = false;
					जइ(reload){
						अनुवर्त;        /* Listeners not valid for reloading. */

					}
					जइ(config__create_default_listener(config, token)){
						प्रत्यागम MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					जइ(config->default_listener->port){
						log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Default listener port specified multiple times. Only the latest will be used.");
					}
					जइ(conf__parse_int(&token, "port", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(tmp_int < 1 || tmp_int > UINT16_MAX){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					config->default_listener->port = (uint16_t)tmp_int;
				}अण्णहा जइ(!strcmp(token, "protocol")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "protocol");

					जइ(!strcmp(token, "mqtt")){
						cur_listener->protocol = mp_mqtt;
						/*
						}else if(!strcmp(token, "mqttsn")){
						    cur_listener->protocol = mp_mqttsn;
						*/
					}अण्णहा जइ(!strcmp(token, "websockets")){
#ifdef WITH_WEBSOCKETS
						cur_listener->protocol = mp_websockets;
#अण्णहा
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Websockets support not available.");
						प्रत्यागम MOSQ_ERR_INVAL;
#endif
					}अण्णहा जइ(!strcmp(token, "http_api")){
#ifdef WITH_HTTP_API
						cur_listener->protocol = mp_http_api;
#अण्णहा
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: HTTP API support not available.");
						प्रत्यागम MOSQ_ERR_INVAL;
#endif
					}अण्णहा{
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'protocol' value (%s).", token);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "proxy_protocol_v2_require_tls")){
#जइ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(रिक्त, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					प्रत्यागम MOSQ_ERR_INVAL;
#endif
					REQUIRE_LISTENER(token);
					जइ(conf__parse_bool(&token, "proxy_protocol_v2_require_tls", &cur_listener->proxy_protocol_v2_require_tls, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "psk_file")){
#ifdef FINAL_WITH_TLS_PSK
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					जइ(cur_listener && cur_listener->certfile){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_security_options->psk_file);
					जइ(conf__parse_string(&token, "psk_file", &cur_security_options->psk_file, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "psk_hint")){
#ifdef FINAL_WITH_TLS_PSK
					जइ(reload){
						अनुवर्त;        /* PSK file not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					जइ(conf__parse_string(&token, "psk_hint", &cur_listener->psk_hint, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "queue_qos0_messages")){
					जइ(conf__parse_bool(&token, token, &config->queue_qos0_messages, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "require_certificate")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					जइ(conf__parse_bool(&token, "require_certificate", &cur_listener->require_certificate, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "restart_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					cur_bridge->backoff_cap = 0; /* set backoff to constant mode, unless cap is specified further down */
					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "restart_timeout");

					cur_bridge->restart_timeout = atoi(token);
					cur_bridge->backoff_base = 0;
					cur_bridge->backoff_cap = 0;
					जइ(cur_bridge->restart_timeout < 1){
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "restart_timeout interval too low, using 1 second.");
						cur_bridge->restart_timeout = 1;
					}अण्णहा जइ(cur_bridge->restart_timeout > 3600){
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "restart_timeout interval too high, using 3600 seconds.");
						cur_bridge->restart_timeout = 3600;
					}
					token = strtok_r(रिक्त, " ", &saveptr);
					जइ(token){
						cur_bridge->backoff_base = cur_bridge->restart_timeout;
						cur_bridge->backoff_cap = atoi(token);
						जइ(cur_bridge->backoff_cap < cur_bridge->backoff_base){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: backoff cap is lower than the base in restart_timeout.");
							प्रत्यागम MOSQ_ERR_INVAL;
						}अण्णहा जइ(cur_bridge->backoff_cap > 7200){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: backoff cap too high, using 7200 seconds.");
							cur_bridge->backoff_cap = 7200;
						}

						token = strtok_r(रिक्त, " ", &saveptr);
						जइ(token){
							cur_bridge->stable_connection_period = atoi(token);
							जइ(cur_bridge->stable_connection_period < 0){
								log__printf(रिक्त, MOSQ_LOG_ERR, "Error: stable connection period cannot be negative.");
								प्रत्यागम MOSQ_ERR_INVAL;
							}
						}
					}
					cur_bridge->restart_timeout *= 1000; /* backoff is tracked in ms */
					cur_bridge->backoff_base *= 1000;
					cur_bridge->backoff_cap *= 1000;
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "retain_available")){
					जइ(conf__parse_bool(&token, token, &config->retain_available, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "retain_expiry_interval")){
					जइ(conf__parse_int(&token, token, &config->retain_expiry_interval, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(config->retain_expiry_interval < 1){
						log__printf(रिक्त, MOSQ_LOG_WARNING, "Error: retain_expiry_interval must be >= 1.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}अण्णहा जइ(config->retain_expiry_interval > 10000000){
						log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: retain_expiry_interval being capped at 19 years.");
						config->retain_expiry_interval = 10000000;
					}
					config->retain_expiry_interval *= 60;
				}अण्णहा जइ(!strcmp(token, "retry_interval")){
					OPTION_UNAVAILABLE(token);
				}अण्णहा जइ(!strcmp(token, "round_robin")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_bool(&token, "round_robin", &cur_bridge->round_robin, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "set_tcp_nodelay")){
					जइ(conf__parse_bool(&token, "set_tcp_nodelay", &config->set_tcp_nodelay, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "start_type")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					जइ(!strcmp(token, "automatic")){
						cur_bridge->start_type = bst_automatic;
					}अण्णहा जइ(!strcmp(token, "lazy")){
						cur_bridge->start_type = bst_lazy;
					}अण्णहा जइ(!strcmp(token, "manual")){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Manual start_type not supported.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}अण्णहा जइ(!strcmp(token, "once")){
						cur_bridge->start_type = bst_once;
					}अण्णहा{
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'start_type' value in configuration (%s).", token);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "socket_domain")){
					जइ(reload){
						अनुवर्त;        /* socket_domain not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					जइ(!strcmp(token, "ipv4")){
						cur_listener->socket_domain = AF_INET;
					}अण्णहा जइ(!strcmp(token, "ipv6")){
						cur_listener->socket_domain = AF_INET6;
					}अण्णहा{
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'socket_domain' value '%s' in configuration.", token);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "sys_interval")){
					जइ(conf__parse_int(&token, "sys_interval", &config->sys_interval, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(config->sys_interval < 0 || config->sys_interval > 65535){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'sys_interval' value (%d).", config->sys_interval);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "threshold")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_int(&token, "threshold", &cur_bridge->threshold, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(cur_bridge->threshold < 1){
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "threshold too low, using 1 message.");
						cur_bridge->threshold = 1;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "tls_engine")){
#ifdef WITH_TLS
					जइ(reload){
						अनुवर्त;        /* tls_engine not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					जइ(conf__parse_string(&token, "tls_engine", &cur_listener->tls_engine, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "tls_engine_kpass_sha1")){
#ifdef WITH_TLS
					जइ(reload){
						अनुवर्त;        /* tls_engine not valid for reloading. */

					}
					वर्ण *kpass_sha = रिक्त, *kpass_sha_bin = रिक्त;
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					जइ(conf__parse_string(&token, "tls_engine_kpass_sha1", &kpass_sha, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(mosquitto__hex2bin_sha1(kpass_sha, (unsigned वर्ण **)&kpass_sha_bin) != MOSQ_ERR_SUCCESS){
						mosquitto_FREE(kpass_sha);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					mosquitto_free(cur_listener->tls_engine_kpass_sha1);
					cur_listener->tls_engine_kpass_sha1 = kpass_sha_bin;
					mosquitto_FREE(kpass_sha);
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "tls_keyform")){
#ifdef WITH_TLS
					जइ(reload){
						अनुवर्त;        /* tls_engine not valid for reloading. */

					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					वर्ण *keyform = रिक्त;
					जइ(conf__parse_string(&token, "tls_keyform", &keyform, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					cur_listener->tls_keyform = mosq_k_pem;
					जइ(!strcmp(keyform, "engine")){
						cur_listener->tls_keyform = mosq_k_engine;
					}
					mosquitto_FREE(keyform);
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "tls_version")){
#जइ defined(WITH_TLS)
					जइ(reload){
						अनुवर्त;        /* tls_version not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					जइ(conf__parse_string(&token, "tls_version", &cur_listener->tls_version, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "topic")){
#ifdef WITH_BRIDGE
					वर्ण *topic = रिक्त;
					गणना mosquitto__bridge_direction direction = bd_out;
					uint8_t qos = 0;
					वर्ण *local_prefix = रिक्त, *remote_prefix = रिक्त;

					REQUIRE_BRIDGE(token);

					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "topic");

					// Check if the topic is quoted (e.g. for spaces within topic names), but not the
					// special case of ""
					जइ(token[0] == '"' && token [1] != '"'){
						जइ(strchr(saveptr, '"') == रिक्त){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Missing closing quote in topic value (%s).", saveptr);
							प्रत्यागम MOSQ_ERR_INVAL;
						}

						वर्ण *topic_in_quotes = strtok_r(रिक्त, "\"", &saveptr);
						size_t tlen = 1;
						जइ(topic_in_quotes){
							tlen = strlen(topic_in_quotes);
						}

						topic = mosquitto_malloc(strlen(token) + tlen + 1);
						जइ(!topic){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
							प्रत्यागम MOSQ_ERR_NOMEM;
						}

						strcpy(topic, token + 1);
						strcat(topic, " ");
						जइ(topic_in_quotes){
							strcat(topic, topic_in_quotes);
						}
					}अण्णहा{
						topic = mosquitto_strdup(token);
						जइ(!topic){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
							प्रत्यागम MOSQ_ERR_NOMEM;
						}
					}

					token = strtok_r(रिक्त, " ", &saveptr);
					जइ(token){
						जइ(!strcasecmp(token, "out")){
							direction = bd_out;
						}अण्णहा जइ(!strcasecmp(token, "in")){
							direction = bd_in;
						}अण्णहा जइ(!strcasecmp(token, "both")){
							direction = bd_both;
						}अण्णहा{
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge topic direction '%s'.", token);
							mosquitto_FREE(topic);
							प्रत्यागम MOSQ_ERR_INVAL;
						}
						token = strtok_r(रिक्त, " ", &saveptr);
						जइ(token){
							जइ(token[0] == '#'){
								(शून्य)strtok_r(रिक्त, "", &saveptr);
							}
							qos = (uint8_t)atoi(token);
							जइ(qos > 2){
								log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge QoS level '%s'.", token);
								mosquitto_FREE(topic);
								प्रत्यागम MOSQ_ERR_INVAL;
							}

							token = strtok_r(रिक्त, " ", &saveptr);
							जइ(token){
								जइ(!strcmp(token, "\"\"") || token[0] == '#'){
									local_prefix = रिक्त;
									जइ(token[0] == '#'){
										(शून्य)strtok_r(रिक्त, "", &saveptr);
									}
								}अण्णहा{
									local_prefix = token;
								}

								token = strtok_r(रिक्त, " ", &saveptr);
								जइ(token){
									जइ(!strcmp(token, "\"\"") || token[0] == '#'){
										remote_prefix = रिक्त;
									}अण्णहा{
										remote_prefix = token;
									}
								}
							}
						}
					}

					जइ(bridge__add_topic(cur_bridge, topic, direction, qos, local_prefix, remote_prefix)){
						mosquitto_FREE(topic);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(topic);
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "max_topic_alias")){
					REQUIRE_LISTENER(token);
					जइ(conf__parse_int(&token, "max_topic_alias", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias' value in configuration.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias = (uint16_t)tmp_int;
				}अण्णहा जइ(!strcmp(token, "max_topic_alias_broker")){
					REQUIRE_LISTENER(token);
					जइ(conf__parse_int(&token, "max_topic_alias_broker", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias_broker' value in configuration.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias_broker = (uint16_t)tmp_int;
				}अण्णहा जइ(!strcmp(token, "try_private")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_bool(&token, "try_private", &cur_bridge->try_private, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "upgrade_outgoing_qos")){
					जइ(conf__parse_bool(&token, token, &config->upgrade_outgoing_qos, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "use_identity_as_username")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					जइ(conf__parse_bool(&token, "use_identity_as_username", &cur_listener->use_identity_as_username, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "use_subject_as_username")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					जइ(conf__parse_bool(&token, "use_subject_as_username", &cur_listener->use_subject_as_username, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "user")){
					जइ(reload){
						अनुवर्त;        /* Drop privileges user not valid for reloading. */
					}
					mosquitto_FREE(config->user);
					जइ(conf__parse_string(&token, "user", &config->user, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "use_username_as_clientid")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					जइ(conf__parse_bool(&token, "use_username_as_clientid", &cur_listener->use_username_as_clientid, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अण्णहा जइ(!strcmp(token, "username") || !strcmp(token, "remote_username")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					जइ(conf__parse_string(&token, "bridge remote_username", &cur_bridge->remote_username, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अण्णहा जइ(!strcmp(token, "websockets_log_level")){
#जइ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					जइ(conf__parse_int(&token, "websockets_log_level", &config->websockets_log_level, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#endif
				}अण्णहा जइ(!strcmp(token, "websockets_headers_size") || !strcmp(token, "packet_buffer_size")){
					जइ(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					जइ(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(रिक्त, MOSQ_LOG_WARNING, "Error: Packet buffer size must be between 0 and 65535 inclusive.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					config->packet_buffer_size = (uint16_t)tmp_int;
				}अण्णहा जइ(!strcmp(token, "websockets_origin")){
#ifdef WITH_WEBSOCKETS
#  जइ LWS_LIBRARY_VERSION_सङ्ख्या >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN
					REQUIRE_LISTENER(token);
					ws_origins = mosquitto_realloc(cur_listener->ws_origins, परिमाण(वर्ण *)*(size_t)(cur_listener->ws_origin_गणनम्+1));
					जइ(ws_origins == रिक्त){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
						प्रत्यागम MOSQ_ERR_NOMEM;
					}
					ws_origins[cur_listener->ws_origin_गणनम्] = रिक्त;
					cur_listener->ws_origins = ws_origins;
					जइ(conf__parse_string(&token, "websockets_origin", &ws_origins[cur_listener->ws_origin_गणनम्], &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					cur_listener->ws_origin_गणनम्++;
#  अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: websockets_origin support not available, libwebsockets version is too old.");
#  endif
#अण्णहा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Websockets support not available.");
#endif
				}अण्णहा{
					log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unknown configuration variable '%s'.", token);
					प्रत्यागम MOSQ_ERR_INVAL;
				}
			}
		}
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क config__read_file(संरचना mosquitto__config *config, bool reload, ध्रुव वर्ण *file, संरचना config_recurse *cr, पूर्णाङ्क level, पूर्णाङ्क *lineno)
{
	पूर्णाङ्क rc;
	FILE *fptr = रिक्त;
	वर्ण *buf;
	पूर्णाङ्क buflen;
#ifndef WIN32
	DIR *dir;
#endif

#ifndef WIN32
	dir = opendir(file);
	जइ(dir){
		closedir(dir);
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Config file '%s' is a directory.", file);
		प्रत्यागम 1;
	}
#endif

	fptr = mosquitto_fopen(file, "rt", false);
	जइ(!fptr){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to open config file '%s'.", file);
		प्रत्यागम 1;
	}

	buflen = 1000;
	buf = mosquitto_malloc((size_t)buflen);
	जइ(!buf){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		fclose(fptr);
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	rc = config__read_file_core(config, reload, cr, level, lineno, fptr, &buf, &buflen);
	mosquitto_FREE(buf);
	fclose(fptr);

	प्रत्यागम rc;
}


स्थिर पूर्णाङ्क config__check_proxy(संरचना mosquitto__config *config)
{
	प्रति(पूर्णाङ्क i=0; i<config->listener_गणनम्; i++){
		संरचना mosquitto__listener *l = &config->listeners[i];

		जइ(l->enable_proxy_protocol == 2){
#ifdef WITH_TLS
			जइ(l->use_subject_as_username){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: use_subject_as_username cannot be used with `enable_proxy_protocol 2`.");
				प्रत्यागम MOSQ_ERR_INVAL;
			}

			जइ(l->certfile || l->keyfile){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: certfile and keyfile cannot be used with `enable_proxy_protocol 2`.");
				प्रत्यागम MOSQ_ERR_INVAL;
			}
#endif
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क config__check(संरचना mosquitto__config *config)
{
	/* Checks that are easy to make after the config has been loaded. */

	ध्रुव वर्ण *id_prefix;
	पूर्णाङ्क id_prefix_len;
	जइ(config->security_options.auto_id_prefix){
		id_prefix = config->security_options.auto_id_prefix;
		id_prefix_len = config->security_options.auto_id_prefix_len;
	}अण्णहा{
		id_prefix = "auto-";
		id_prefix_len = (पूर्णाङ्क)strlen("auto-");
	}

	/* Default to auto_id_prefix = 'auto-' if none set. */
	प्रति(पूर्णाङ्क i=0; i<config->listener_गणनम्; i++){
		जइ(!config->listeners[i].security_options->auto_id_prefix){
			config->listeners[i].security_options->auto_id_prefix = mosquitto_strdup(id_prefix);
			जइ(!config->listeners[i].security_options->auto_id_prefix){
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
			config->listeners[i].security_options->auto_id_prefix_len = (uint16_t)id_prefix_len;
		}
	}

	प्रत्यागम config__check_proxy(config);
}

#ifdef WITH_BRIDGE


स्थिर पूर्णाङ्क config__check_bridges(संरचना mosquitto__config *config)
{
	संरचना mosquitto__bridge *bridge1, *bridge2;
	वर्ण hostname[256];
	size_t len;

	/* Check for bridge duplicate local_clientid, need to generate missing IDs
	 * first. */
	प्रति(पूर्णाङ्क i=0; i<config->bridge_गणनम्; i++){
		bridge1 = config->bridges[i];

		जइ(!bridge1->remote_clientid){
			जइ(!gethostname(hostname, 256)){
				len = strlen(hostname) + strlen(bridge1->name) + 2;
				bridge1->remote_clientid = mosquitto_malloc(len);
				जइ(!bridge1->remote_clientid){
					प्रत्यागम MOSQ_ERR_NOMEM;
				}
				snprintf(bridge1->remote_clientid, len, "%s.%s", hostname, bridge1->name);
			}अण्णहा{
				प्रत्यागम 1;
			}
		}

		जइ(!bridge1->local_clientid){
			len = strlen(bridge1->remote_clientid) + strlen("local.") + 2;
			bridge1->local_clientid = mosquitto_malloc(len);
			जइ(!bridge1->local_clientid){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
			snprintf(bridge1->local_clientid, len, "local.%s", bridge1->remote_clientid);
		}
	}

	प्रति(पूर्णाङ्क i=0; i<config->bridge_गणनम्; i++){
		bridge1 = config->bridges[i];
		प्रति(पूर्णाङ्क j=i+1; j<config->bridge_गणनम्; j++){
			bridge2 = config->bridges[j];
			जइ(!strcmp(bridge1->local_clientid, bridge2->local_clientid)){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Bridge local_clientid "
						"'%s' is not unique. Try changing or setting the "
						"local_clientid value for one of the bridges.",
						bridge1->local_clientid);
				प्रत्यागम MOSQ_ERR_INVAL;
			}
		}
	}

#ifdef WITH_TLS
	/* Check for missing TLS cafile/capath/certfile/keyfile */
	प्रति(पूर्णाङ्क i=0; i<config->listener_गणनम्; i++){
		bool cafile = !!config->listeners[i].cafile;
		bool capath = !!config->listeners[i].capath;
		bool certfile = !!config->listeners[i].certfile;
		bool keyfile = !!config->listeners[i].keyfile;

		जइ((certfile && !keyfile) || (!certfile && keyfile)){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Both certfile and keyfile must be provided to enable a TLS listener.");
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		जइ(cafile && !certfile){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: cafile specified without certfile and keyfile.");
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		जइ(capath && !certfile){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: capath specified without certfile and keyfile.");
			प्रत्यागम MOSQ_ERR_INVAL;
		}
	}
#endif
	प्रत्यागम MOSQ_ERR_SUCCESS;
}
#endif


स्थिर पूर्णाङ्क conf__parse_bool(वर्ण **token, ध्रुव वर्ण *name, bool *मानम्, वर्ण **saveptr)
{
	*token = strtok_r(रिक्त, " ", saveptr);
	जइ(*token){
		जइ(!strcmp(*token, "false") || !strcmp(*token, "0")){
			*मानम् = false;
		}अण्णहा जइ(!strcmp(*token, "true") || !strcmp(*token, "1")){
			*मानम् = true;
		}अण्णहा{
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid '%s' value (%s).", name, *token);
			प्रत्यागम MOSQ_ERR_INVAL;
		}
	}अण्णहा{
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क conf__parse_int(वर्ण **token, ध्रुव वर्ण *name, पूर्णाङ्क *मानम्, वर्ण **saveptr)
{
	*token = strtok_r(रिक्त, " ", saveptr);
	जइ(*token){
		वर्ण *endptr = रिक्त;
		long l = strtol(*token, &endptr, 0);
		जइ(endptr == *token || endptr[0] != '\0'){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: '%s' value not a number.", name);
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		जइ(l > INT_MAX || l < INT_MIN){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: '%s' value out of range.", name);
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		*मानम् = (पूर्णाङ्क)l;
	}अण्णहा{
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क conf__parse_ssize_t(वर्ण **token, ध्रुव वर्ण *name, ssize_t *मानम्, वर्ण **saveptr)
{
	*token = strtok_r(रिक्त, " ", saveptr);
	जइ(*token){
		*मानम् = atol(*token);
	}अण्णहा{
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क conf__parse_string(वर्ण **token, ध्रुव वर्ण *name, वर्ण **मानम्, वर्ण **saveptr)
{
	size_t tlen;

	*token = strtok_r(रिक्त, "", saveptr);
	जइ(*token){
		जइ(*मानम्){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Duplicate '%s' value in configuration.", name);
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		/* Deal with multiple spaces at the beginning of the string. */
		*token = mosquitto_trimblanks(*token);
		जइ(strlen(*token) == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
			प्रत्यागम MOSQ_ERR_INVAL;
		}

		tlen = strlen(*token);
		जइ(tlen > UINT16_MAX){
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		जइ(mosquitto_validate_utf8(*token, (uint16_t)tlen)){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Malformed UTF-8 in configuration.");
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		*मानम् = mosquitto_strdup(*token);
		जइ(!*मानम्){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
	}अण्णहा{
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}
