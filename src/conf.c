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
#अन्यथा
#  include <dirent.h>
#  include <strings.h>
#endif

#ifndef WIN32
#  include <netdb.h>
#  include <sys/socket.h>
#अन्यथा
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#यदि !defined(WIN32) && !defined(__CYGWIN__)
#  include <syslog.h>
#endif

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "tls_mosq.h"
#अन्तर्भाव "util_mosq.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"

#अन्तर्भाव "utlist.h"

संरचना config_recurse {
	अचिन्हित पूर्णाङ्क log_dest;
	पूर्णाङ्क log_dest_set;
	अचिन्हित पूर्णाङ्क log_type;
	पूर्णाङ्क log_type_set;
};

#यदि defined(WIN32) || defined(__CYGWIN__)
#अन्तर्भाव <windows.h>
बाह्य SERVICE_STATUS_HANDLE service_handle;
#endif


#define REQUIRE_LISTENER(A) \
		गर{ \
			यदि(cur_listener == रिक्त){ \
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				फर्काऊ MOSQ_ERR_INVAL; \
			} \
		}जबसम्म(0)

#define REQUIRE_LISTENER_OR_DEFAULT_LISTENER(A) \
		गर{ \
			यदि(cur_listener == रिक्त){ \
				यदि(config__create_default_listener(config, (A))){ \
					फर्काऊ MOSQ_ERR_NOMEM; \
				} \
				cur_listener = config->default_listener; \
			} \
		}जबसम्म(0)

#define REQUIRE_LISTENER_IF_PER_LISTENER(A) \
		गर{ \
			यदि(config->per_listener_settings == true && cur_listener == रिक्त){ \
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				फर्काऊ MOSQ_ERR_INVAL; \
			} \
		}जबसम्म(0)

#define REQUIRE_NON_DEFAULT_LISTENER(A) \
		गर{ \
			यदि(cur_listener == config->default_listener || cur_listener == रिक्त){ \
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				फर्काऊ MOSQ_ERR_INVAL; \
			} \
		}जबसम्म(0)

#define REQUIRE_BRIDGE(A) \
		गर{ \
			यदि(cur_bridge == रिक्त){ \
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: The '%s' option requires a bridge to be defined first.", (A)); \
				फर्काऊ MOSQ_ERR_INVAL; \
			} \
		}जबसम्म(0)

#define REQUIRE_PLUGIN(A) \
		गर{ \
			यदि(cur_plugin == रिक्त){ \
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: The '%s' option requires plugin/global_plugin/plugin_load to be defined first.", (A)); \
				फर्काऊ MOSQ_ERR_INVAL; \
			} \
		}जबसम्म(0)

#define OPTION_DEPRECATED(A, B) \
		log__printf(रिक्त, MOSQ_LOG_NOTICE, "The '%s' option is now deprecated and will be removed in version 3.0. %s", (A), (B))

#define OPTION_UNAVAILABLE(A) \
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: The '%s' option is no longer available.", (A));

#define REQUIRE_NON_EMPTY_OPTION(A, B) \
		गर{ \
			यदि(!(A)){ \
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", (B)); \
				फर्काऊ MOSQ_ERR_INVAL; \
			} \
		}जबसम्म(0)

#define PER_LISTENER_ALTERNATIVE(A, B) \
		यदि(config->per_listener_settings){ \
			log__printf(रिक्त, MOSQ_LOG_NOTICE, "You are using the '%s' option with 'per_listener_settings true'. Please replace this with '%s'.", A, B); \
		} \


#ifdef FINAL_WITH_TLS_PSK
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A) \
		गर{ \
			यदि(cur_bridge->tls_psk_identity || cur_bridge->tls_psk){ \
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and psk encryption in a single bridge.", (A)); \
				फर्काऊ MOSQ_ERR_INVAL; \
			} \
		}जबसम्म(0)

#  define REQUIRE_BRIDGE_NO_X509(A) \
		गर{ \
			यदि(cur_bridge->tls_cafile || cur_bridge->tls_capath || cur_bridge->tls_certfile || cur_bridge->tls_keyfile){ \
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and identity encryption in a single bridge.", (A)); \
				फर्काऊ MOSQ_ERR_INVAL; \
			} \
		}जबसम्म(0)

#अन्यथा
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A)
#  define REQUIRE_BRIDGE_NO_X509(A)
#endif

स्थिर संरचना mosquitto__security_options *cur_security_options = रिक्त;

स्थिर पूर्णाङ्क conf__parse_bool(वर्ण **token, अचर वर्ण *name, bool *मान, वर्ण **saveptr);
स्थिर पूर्णाङ्क conf__parse_int(वर्ण **token, अचर वर्ण *name, पूर्णाङ्क *मान, वर्ण **saveptr);
स्थिर पूर्णाङ्क conf__parse_ssize_t(वर्ण **token, अचर वर्ण *name, ssize_t *मान, वर्ण **saveptr);
स्थिर पूर्णाङ्क conf__parse_string(वर्ण **token, अचर वर्ण *name, वर्ण **मान, वर्ण **saveptr);
स्थिर पूर्णाङ्क config__read_file(संरचना mosquitto__config *config, bool reload, अचर वर्ण *file, संरचना config_recurse *config_tmp, पूर्णाङ्क level, पूर्णाङ्क *lineno);
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

	यदि(config->default_listener){
		लागि(पूर्णाङ्क i=0; i<config->listener_गन्ती; i++){
			यदि(&config->listeners[i] == config->default_listener){
				def_listener = i;
				रोक;
			}
		}
	}
	new_listeners = mosquitto_realloc(config->listeners, परिमाण(संरचना mosquitto__listener)*(माप_प्रकार)(config->listener_गन्ती+1));
	यदि(!new_listeners){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		फर्काऊ MOSQ_ERR_NOMEM;
	}
	config->listeners = new_listeners;
	listener = &config->listeners[config->listener_गन्ती];
	स_रखो(listener, 0, परिमाण(संरचना mosquitto__listener));
	listener->security_options = mosquitto_calloc(1, परिमाण(संरचना mosquitto__security_options));
	यदि(!listener->security_options){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		फर्काऊ MOSQ_ERR_NOMEM;
	}
	यदि(def_listener != -1){
		config->default_listener = &config->listeners[def_listener];
	}
	config->listener_गन्ती++;

	फर्काऊ MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क config__create_default_listener(संरचना mosquitto__config *config, अचर वर्ण *option_name)
{
	यदि(config->default_listener){
		फर्काऊ MOSQ_ERR_SUCCESS;
	}
	log__printf(रिक्त, MOSQ_LOG_INFO, "Creating default listener due to '%s' option.", option_name);
	log__printf(रिक्त, MOSQ_LOG_INFO, "It is best practice to define a 'listener' first. Using the '%s' option without a listener will be disabled in the future.", option_name);
	यदि(config__add_listener(config)){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		फर्काऊ MOSQ_ERR_NOMEM;
	}

	config->default_listener = &config->listeners[config->listener_गन्ती-1];
	listener__set_defaults(config->default_listener);
	config->default_listener->port = 1883;

	फर्काऊ MOSQ_ERR_SUCCESS;
}


स्थिर शून्य conf__set_cur_security_options(संरचना mosquitto__config *config, संरचना mosquitto__listener **cur_listener, संरचना mosquitto__security_options **security_options, अचर वर्ण *option_name)
{
	यदि(config->per_listener_settings){
		यदि(*cur_listener == रिक्त){
			यदि(config__create_default_listener(config, option_name)){
				फर्काऊ;
			}
			*cur_listener = config->default_listener;
		}
		(*security_options) = (*cur_listener)->security_options;
	}अन्यथा{
		(*security_options) = &config->security_options;
	}
}


स्थिर पूर्णाङ्क conf__attempt_resolve(अचर वर्ण *host, अचर वर्ण *text, अचिन्हित पूर्णाङ्क log, अचर वर्ण *सन्देश)
{
	संरचना addrinfo gai_hints;
	संरचना addrinfo *gai_res;
	पूर्णाङ्क rc;

	स_रखो(&gai_hints, 0, परिमाण(संरचना addrinfo));
	gai_hints.ai_family = AF_UNSPEC;
	gai_hints.ai_socktype = SOCK_STREAM;
	gai_res = रिक्त;
	rc = getaddrinfo(host, रिक्त, &gai_hints, &gai_res);
	यदि(gai_res){
		freeaddrinfo(gai_res);
	}
	यदि(rc != 0){
#ifndef WIN32
		यदि(rc == EAI_SYSTEM){
			यदि(त्रुटि_सं == ENOENT){
				log__printf(रिक्त, log, "%s: Unable to resolve %s %s.", सन्देश, text, host);
			}अन्यथा{
				log__printf(रिक्त, log, "%s: Error resolving %s: %s.", सन्देश, text, म_त्रुटि(त्रुटि_सं));
			}
		}अन्यथा{
			log__printf(रिक्त, log, "%s: Error resolving %s: %s.", सन्देश, text, gai_strerror(rc));
		}
#अन्यथा
		यदि(rc == WSAHOST_NOT_FOUND){
			log__printf(रिक्त, log, "%s: Error resolving %s.", सन्देश, text);
		}
#endif
		फर्काऊ MOSQ_ERR_INVAL;
	}
	फर्काऊ MOSQ_ERR_SUCCESS;
}


स्थिर शून्य config__init_reload(संरचना mosquitto__config *config)
{
	/* Set defaults */
	लागि(पूर्णाङ्क i=0; i<config->listener_गन्ती; i++){
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
	यदि(config->log_fptr){
		ख_बंद(config->log_fptr);
		config->log_fptr = रिक्त;
	}
	mosquitto_FREE(config->log_file);

#यदि defined(WIN32) || defined(__CYGWIN__)
	यदि(service_handle){
		/* This is running as a Windows service. Default to no logging. Using
		 * stdout/stderr is forbidden because the first clients to connect will
		 * get log information sent to them for some reason. */
		config->log_dest = MQTT3_LOG_NONE;
	}अन्यथा{
		config->log_dest = MQTT3_LOG_STDERR;
	}
#अन्यथा
	config->log_facility = LOG_DAEMON;
	config->log_dest = MQTT3_LOG_STDERR | MQTT3_LOG_DLT;
	यदि(db.quiet){
		config->log_type = 0;
	}अन्यथा यदि(db.verbose){
		config->log_type = अच_पूर्णांक_उच्च;
	}अन्यथा{
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

	यदि(plugin->config.options){
		लागि(पूर्णाङ्क j=0; j<plugin->config.option_गन्ती; j++){
			mosquitto_FREE(plugin->config.options[j].साँचो);
			mosquitto_FREE(plugin->config.options[j].मान);
		}
		mosquitto_FREE(plugin->config.options);
		plugin->config.option_गन्ती = 0;
	}
	mosquitto_FREE(plugin->config.security_options);
	mosquitto_FREE(plugin);
}


स्थिर शून्य config__cleanup_plugins(शून्य)
{
	लागि(पूर्णाङ्क i=0; i<db.plugin_गन्ती; i++){
		config__cleanup_plugin_config(db.plugins[i]);
	}
	mosquitto_FREE(db.plugins);
}


शून्य config__init(संरचना mosquitto__config *config)
{
	स_रखो(config, 0, परिमाण(संरचना mosquitto__config));
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
	यदि(config->listeners){
		लागि(पूर्णाङ्क i=0; i<config->listener_गन्ती; i++){
			mosquitto_FREE(config->listeners[i].host);
			mosquitto_FREE(config->listeners[i].bind_interface);
			mosquitto_FREE(config->listeners[i].mount_point);
			mosquitto_FREE(config->listeners[i].socks);
			यदि(config->listeners[i].security_options){
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
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			यदि(!config->listeners[i].ws_context) /* libwebsockets frees its own SSL_CTX */
#endif
			{
				SSL_CTX_free(config->listeners[i].ssl_ctx);
				config->listeners[i].ssl_ctx = रिक्त;
			}
#endif
#यदि defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
			mosquitto_FREE(config->listeners[i].http_dir);
#endif
#ifdef WITH_WEBSOCKETS
			लागि(पूर्णाङ्क j=0; j<config->listeners[i].ws_origin_गन्ती; j++){
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
	यदि(config->bridges){
		लागि(पूर्णाङ्क i=0; i<config->bridge_गन्ती; i++){
			config__bridge_cleanup(config->bridges[i]);
		}
		mosquitto_FREE(config->bridges);
	}
#endif
	config__cleanup_plugins();

	यदि(config->log_fptr){
		ख_बंद(config->log_fptr);
		config->log_fptr = रिक्त;
	}
	यदि(config->log_file){
		mosquitto_FREE(config->log_file);
		config->log_file = रिक्त;
	}
}

#ifdef WITH_BRIDGE


शून्य config__bridge_cleanup(संरचना mosquitto__bridge *bridge)
{
	यदि(bridge == रिक्त){
		फर्काऊ;
	}

	mosquitto_FREE(bridge->name);
	यदि(bridge->addresses){
		लागि(पूर्णाङ्क i=0; i<bridge->address_गन्ती; i++){
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
	यदि(bridge->topics){
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
	म_लिखो("mosquitto %s\n", VERSION);
	म_लिखो("Copyright © 2025 Roger Light.\n");
	म_लिखो("License EPL-2.0 OR BSD-3-Clause.\n");
}


स्थिर शून्य print_usage(शून्य)
{
	म_लिखो("mosquitto version %s\n\n", VERSION);
	म_लिखो("mosquitto is an MQTT v5.0/v3.1.1/v3.1 broker.\n\n");
	म_लिखो("Usage: mosquitto [-c config_file] [-d] [-h] [-p port] [-v]\n");
	म_लिखो("                 [--tls-keylog file]\n\n");
	म_लिखो(" -c : specify the broker config file.\n");
	म_लिखो(" -d : put the broker into the background after starting.\n");
	म_लिखो(" -h : display this help.\n");
	म_लिखो(" -p : start the broker listening on the specified port.\n");
	म_लिखो("      Not recommended in conjunction with the -c option.\n");
	म_लिखो(" -q : quiet mode - disable all logging types. This overrides\n");
	म_लिखो("      any logging options given in the config file, and -v.\n");
	म_लिखो(" -v : verbose mode - enable all logging types. This overrides\n");
	म_लिखो("      any logging options given in the config file.\n");
	म_लिखो(" --test-config : test config file and exit\n");
	म_लिखो(" --tls-keylog : log TLS connection information to a file, to allow\n");
	म_लिखो("      debugging with e.g. wireshark. Do not use on a production\n");
	म_लिखो("      server.\n");
	म_लिखो("\nSee https://mosquitto.org/ for more information.\n\n");
}


पूर्णाङ्क config__parse_args(संरचना mosquitto__config *config, पूर्णाङ्क argc, वर्ण *argv[])
{
	पूर्णाङ्क i;
	पूर्णाङ्क port_tmp;

	लागि(i=1; i<argc; i++){
		यदि(!म_भेद(argv[i], "-c") || !म_भेद(argv[i], "--config-file")){
			यदि(i<argc-1){
				db.config_file = argv[i+1];

				यदि(config__read(config, false)){
					फर्काऊ MOSQ_ERR_INVAL;
				}
			}अन्यथा{
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: -c argument given, but no config file specified.");
				फर्काऊ MOSQ_ERR_INVAL;
			}
			i++;
		}अन्यथा यदि(!म_भेद(argv[i], "-d") || !म_भेद(argv[i], "--daemon")){
			config->daemon = true;
		}अन्यथा यदि(!म_भेद(argv[i], "-h") || !म_भेद(argv[i], "--help")){
			print_usage();
			फर्काऊ MOSQ_ERR_UNKNOWN;
		}अन्यथा यदि(!म_भेद(argv[i], "--version")){
			print_version();
			फर्काऊ MOSQ_ERR_UNKNOWN;
		}अन्यथा यदि(!म_भेद(argv[i], "-p") || !म_भेद(argv[i], "--port")){
			यदि(i<argc-1){
				port_tmp = म_से_प(argv[i+1]);
				यदि(port_tmp<1 || port_tmp>UINT16_MAX){
					log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid port specified (%d).", port_tmp);
					फर्काऊ MOSQ_ERR_INVAL;
				}अन्यथा{
					यदि(config->cmd_port_गन्ती == CMD_PORT_LIMIT){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Only %d ports can be specified on the command line.", CMD_PORT_LIMIT);
						फर्काऊ MOSQ_ERR_INVAL;
					}
					config->cmd_port[config->cmd_port_गन्ती] = (uint16_t)port_tmp;
					config->cmd_port_गन्ती++;
				}
			}अन्यथा{
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: -p argument given, but no port specified.");
				फर्काऊ MOSQ_ERR_INVAL;
			}
			i++;
		}अन्यथा यदि(!म_भेद(argv[i], "--tls-keylog")){
#ifdef WITH_TLS
			यदि(i<argc-1){
				db.tls_keylog = mosquitto_strdup(argv[i+1]);
				यदि(db.tls_keylog == रिक्त){
					log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
					फर्काऊ MOSQ_ERR_NOMEM;
				}
			}अन्यथा{
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: --tls-keylog argument given, but no file specified.");
				फर्काऊ MOSQ_ERR_INVAL;
			}
			i++;
#अन्यथा
			ख_लिखो(मानक_त्रुटि, "Error: TLS support not available so --tls-keylog is not available.\n");
			फर्काऊ MOSQ_ERR_INVAL;
#endif
		}अन्यथा यदि(!म_भेद(argv[i], "-q") || !म_भेद(argv[i], "--quiet")){
			db.quiet = true;
		}अन्यथा यदि(!म_भेद(argv[i], "-v") || !म_भेद(argv[i], "--verbose")){
			db.verbose = true;
		}अन्यथा यदि(!म_भेद(argv[i], "--test-config")){
			config->test_configuration = true;
		}अन्यथा{
			ख_लिखो(मानक_त्रुटि, "Error: Unknown option '%s'.\n", argv[i]);
			print_usage();
			फर्काऊ MOSQ_ERR_INVAL;
		}
	}

	/* Default to drop to mosquitto user if we are privileged and no user specified. */
	यदि(!config->user){
		config->user = mosquitto_strdup("mosquitto");
		यदि(config->user == रिक्त){
			फर्काऊ MOSQ_ERR_NOMEM;
		}
	}
	यदि(db.quiet){
		config->log_type = 0;
	}अन्यथा यदि(db.verbose){
		config->log_type = अच_पूर्णांक_उच्च;
	}

	यदि(दो_पर्या("MOSQUITTO_PERSISTENCE_LOCATION")){
		mosquitto_FREE(config->persistence_location);
		config->persistence_location = mosquitto_strdup(दो_पर्या("MOSQUITTO_PERSISTENCE_LOCATION"));
		यदि(!config->persistence_location){
			फर्काऊ MOSQ_ERR_NOMEM;
		}
	}
	फर्काऊ config__check(config);
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
	dest->security_options.plugin_गन्ती = src->security_options.plugin_गन्ती;
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

#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	dest->websockets_log_level = src->websockets_log_level;
#endif

#ifdef WITH_BRIDGE
	लागि(पूर्णाङ्क i=0; i<dest->bridge_गन्ती; i++){
		यदि(dest->bridges[i]){
			config__bridge_cleanup(dest->bridges[i]);
		}
	}
	mosquitto_FREE(dest->bridges);
	dest->bridges = src->bridges;
	dest->bridge_गन्ती = src->bridge_गन्ती;
#endif
}


पूर्णाङ्क config__read(संरचना mosquitto__config *config, bool reload)
{
	पूर्णाङ्क rc = MOSQ_ERR_SUCCESS;
	संरचना config_recurse cr;
	पूर्णाङ्क lineno = 0;
#ifdef WITH_PERSISTENCE
	माप_प्रकार len;
#endif
	संरचना mosquitto__config config_reload;
	पूर्णाङ्क i;

	यदि(reload){
		स_रखो(&config_reload, 0, परिमाण(संरचना mosquitto__config));
	}

	cr.log_dest = MQTT3_LOG_NONE;
	cr.log_dest_set = 0;
	cr.log_type = MOSQ_LOG_NONE;
	cr.log_type_set = 0;

	यदि(!db.config_file){
		फर्काऊ 0;
	}

	यदि(reload){
		/* Re-initialise appropriate config vars to default for reload. */
		config__init_reload(&config_reload);
		config_reload.listeners = config->listeners;
		config_reload.listener_गन्ती = config->listener_गन्ती;
		cur_security_options = रिक्त;
		rc = config__read_file(&config_reload, reload, db.config_file, &cr, 0, &lineno);
	}अन्यथा{
		rc = config__read_file(config, reload, db.config_file, &cr, 0, &lineno);
	}
	यदि(rc){
		यदि(lineno > 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error found at %s:%d.", db.config_file, lineno);
		}
		फर्काऊ rc;
	}

	यदि(reload){
		config__copy(&config_reload, config);
	}

	/* If auth/access options are set and allow_anonymous not explicitly set, disallow anon. */
	यदि(config->local_only == false){
		यदि(config->per_listener_settings){
			लागि(i=0; i<config->listener_गन्ती; i++){
				/* Default option if no security options set */
				यदि(config->listeners[i].security_options->allow_anonymous == -1){
					config->listeners[i].security_options->allow_anonymous = false;
				}
			}
		}अन्यथा{
			यदि(config->security_options.allow_anonymous == -1){
				config->security_options.allow_anonymous = false;
			}
		}
	}
#ifdef WITH_PERSISTENCE
	यदि(config->persistence){
		यदि(!config->persistence_file){
			config->persistence_file = mosquitto_strdup("mosquitto.db");
			यदि(!config->persistence_file){
				फर्काऊ MOSQ_ERR_NOMEM;
			}
		}
		mosquitto_FREE(config->persistence_filepath);
		यदि(config->persistence_location && म_माप(config->persistence_location)){
			len = म_माप(config->persistence_location) + म_माप(config->persistence_file) + 2;
			config->persistence_filepath = mosquitto_malloc(len);
			यदि(!config->persistence_filepath){
				फर्काऊ MOSQ_ERR_NOMEM;
			}
#ifdef WIN32
			snprintf(config->persistence_filepath, len, "%s\\%s", config->persistence_location, config->persistence_file);
#अन्यथा
			snprintf(config->persistence_filepath, len, "%s/%s", config->persistence_location, config->persistence_file);
#endif
		}अन्यथा{
			config->persistence_filepath = mosquitto_strdup(config->persistence_file);
			यदि(!config->persistence_filepath){
				फर्काऊ MOSQ_ERR_NOMEM;
			}
		}
	}
#endif
	/* Default to drop to mosquitto user if no other user specified. This must
	 * remain here even though it is covered in config__parse_args() because this
	 * function may be called on its own. */
	यदि(!config->user){
		config->user = mosquitto_strdup("mosquitto");
	}

#ifdef WITH_BRIDGE
	लागि(i=0; i<config->bridge_गन्ती; i++){
		यदि(!config->bridges[i]->name){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: bridge name not defined.");
			फर्काऊ MOSQ_ERR_INVAL;
		}
		यदि(config->bridges[i]->addresses  == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no remote addresses defined.");
			फर्काऊ MOSQ_ERR_INVAL;
		}
		यदि(config->bridges[i]->topic_गन्ती == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no topics defined.");
			फर्काऊ MOSQ_ERR_INVAL;
		}
#ifdef FINAL_WITH_TLS_PSK
		यदि(config->bridges[i]->tls_psk && !config->bridges[i]->tls_psk_identity){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_identity.");
			फर्काऊ MOSQ_ERR_INVAL;
		}
		यदि(config->bridges[i]->tls_psk_identity && !config->bridges[i]->tls_psk){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_psk.");
			फर्काऊ MOSQ_ERR_INVAL;
		}
#endif
	}
#endif

	यदि(cr.log_dest_set){
		config->log_dest = cr.log_dest;
	}
	यदि(db.quiet){
		config->log_type = 0;
	}अन्यथा यदि(db.verbose){
		config->log_type = अच_पूर्णांक_उच्च;
	}अन्यथा यदि(cr.log_type_set){
		config->log_type = cr.log_type;
	}
#ifdef WITH_BRIDGE
	फर्काऊ config__check_bridges(config);
#अन्यथा
	फर्काऊ MOSQ_ERR_SUCCESS;
#endif
}


स्थिर mosquitto_plugin_id_t *config__plugin_find(अचर वर्ण *name)
{
	यदि(db.plugins && name){
		लागि(पूर्णाङ्क i=0; i<db.plugin_गन्ती; i++){
			यदि(db.plugins[i]->config.name && !म_भेद(db.plugins[i]->config.name, name)){
				फर्काऊ db.plugins[i];
			}
		}
	}
	फर्काऊ रिक्त;
}


स्थिर mosquitto_plugin_id_t *config__plugin_load(अचर वर्ण *name, अचर वर्ण *path)
{
	mosquitto_plugin_id_t *plugin = रिक्त;
	mosquitto_plugin_id_t **plugins = रिक्त;

	यदि(name && config__plugin_find(name)){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Duplicate plugin name '%s'.", name);
		फर्काऊ रिक्त;
	}

	यदि(!path || !म_भेद(path, "")){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Missing plugin path for plugin name '%s'.", name);
		फर्काऊ रिक्त;
	}

	plugin = mosquitto_calloc(1, परिमाण(mosquitto_plugin_id_t));
	यदि(!plugin){
		गच्छ error;
	}

	यदि(name){
		plugin->config.name = mosquitto_strdup(name);
	}
	plugin->config.path = mosquitto_strdup(path);
	यदि((name && !plugin->config.name) || !plugin->config.path){
		गच्छ error;
	}
	plugin->config.options = रिक्त;
	plugin->config.option_गन्ती = 0;
	plugin->config.deny_special_chars = true;

	/* Add to db list */
	plugins = mosquitto_realloc(db.plugins, (माप_प्रकार)(db.plugin_गन्ती+1)*परिमाण(संरचना mosquitto__plugin_config *));
	यदि(!plugins){
		गच्छ error;
	}

	plugins[db.plugin_गन्ती] = plugin;
	db.plugins = plugins;
	db.plugin_गन्ती++;

	फर्काऊ plugin;
error:
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
	यदि(plugin){
		mosquitto_FREE(plugin->config.name);
		mosquitto_FREE(plugin->config.path);
	}
	mosquitto_FREE(plugin);
	फर्काऊ रिक्त;
}


पूर्णाङ्क config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, संरचना mosquitto__security_options *security_options)
{
	संरचना mosquitto__security_options **new_options;
	mosquitto_plugin_id_t **new_plugins;

	new_options = mosquitto_realloc(plugin->config.security_options, (माप_प्रकार)(plugin->config.security_option_गन्ती+1)*परिमाण(संरचना mosquitto__security_options *));
	new_plugins = mosquitto_realloc(security_options->plugins, (माप_प्रकार)(security_options->plugin_गन्ती+1)*परिमाण(mosquitto_plugin_id_t *));

	यदि(!new_options || !new_plugins){
		mosquitto_FREE(new_options);
		mosquitto_FREE(new_plugins);
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		फर्काऊ MOSQ_ERR_NOMEM;
	}

	new_options[plugin->config.security_option_गन्ती] = security_options;
	plugin->config.security_options = new_options;
	plugin->config.security_option_गन्ती++;

	new_plugins[security_options->plugin_गन्ती] = plugin;
	security_options->plugins = new_plugins;
	security_options->plugin_गन्ती++;

	फर्काऊ MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क config__read_file_core(संरचना mosquitto__config *config, bool reload, संरचना config_recurse *cr, पूर्णाङ्क level, पूर्णाङ्क *lineno, खाता *fptr, वर्ण **buf, पूर्णाङ्क *buflen)
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

	वर्ण *साँचो;
	संरचना mosquitto__listener *cur_listener = रिक्त;
	पूर्णाङ्क i;
	पूर्णाङ्क lineno_ext = 0;
	माप_प्रकार prefix_len;
	माप_प्रकार slen;
#ifdef WITH_WEBSOCKETS
	वर्ण **ws_origins = रिक्त;
#endif

	*lineno = 0;

	जबसम्म(mosquitto_fgets(buf, buflen, fptr)){
		(*lineno)++;
		यदि((*buf)[0] != '#' && (*buf)[0] != 10 && (*buf)[0] != 13){
			slen = म_माप(*buf);
			यदि(slen == 0){
				जारी;
			}
			जबसम्म((*buf)[slen-1] == 10 || (*buf)[slen-1] == 13){
				(*buf)[slen-1] = 0;
				slen = म_माप(*buf);
				यदि(slen == 0){
					जारी;
				}
			}
			token = strtok_r((*buf), " ", &saveptr);
			यदि(token){
				यदि(!म_भेद(token, "accept_protocol_versions")){
					REQUIRE_NON_DEFAULT_LISTENER(token);
					cur_listener->disable_protocol_v3 = true;
					cur_listener->disable_protocol_v4 = true;
					cur_listener->disable_protocol_v5 = true;
					यदि(saveptr == रिक्त){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", "accept_protocol_versions");
						फर्काऊ MOSQ_ERR_INVAL;
					}
					token = strtok_r(saveptr, ", \t", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "accept_protocol_versions");

					जबसम्म(token){
						यदि(!म_भेद(token, "3")){
							cur_listener->disable_protocol_v3 = false;
						}अन्यथा यदि(!म_भेद(token, "4")){
							cur_listener->disable_protocol_v4 = false;
						}अन्यथा यदि(!म_भेद(token, "5")){
							cur_listener->disable_protocol_v5 = false;
						}

						token = strtok_r(रिक्त, ", \t", &saveptr);
					}
				}अन्यथा यदि(!म_भेद(token, "acl_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);

					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->acl_data.acl_file);
					यदि(conf__parse_string(&token, "acl_file", &cur_security_options->acl_data.acl_file, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "address") || !म_भेद(token, "addresses")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(cur_bridge->addresses){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge configuration, 'address' only allowed once.");
						फर्काऊ MOSQ_ERR_INVAL;
					}
					जबसम्म((token = strtok_r(रिक्त, " ", &saveptr))){
						यदि(token[0] == '#'){
							रोक;
						}
						संरचना bridge_address *new_addresses = mosquitto_realloc(cur_bridge->addresses, परिमाण(संरचना bridge_address)*(माप_प्रकार)(cur_bridge->address_गन्ती+1));
						यदि(!new_addresses){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
							फर्काऊ MOSQ_ERR_NOMEM;
						}
						cur_bridge->address_गन्ती++;
						cur_bridge->addresses = new_addresses;
						स_रखो(&cur_bridge->addresses[cur_bridge->address_गन्ती-1], 0, परिमाण(संरचना bridge_address));
						cur_bridge->addresses[cur_bridge->address_गन्ती-1].address = mosquitto_strdup(token);
						यदि(!cur_bridge->addresses[cur_bridge->address_गन्ती-1].address){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
							फर्काऊ MOSQ_ERR_NOMEM;
						}
					}
					लागि(i=0; i<cur_bridge->address_गन्ती; i++){
						/* cur_bridge->addresses[i].address is now
						 * "address[:port]". If address is an IPv6 address,
						 * then port is required. We must check for the :
						 * backwards. */
						tmp_char = म_खोजप(cur_bridge->addresses[i].address, ':');
						यदि(tmp_char){
							/* Remove ':', so cur_bridge->addresses[i].address
							 * now just looks like the address. */
							tmp_char[0] = '\0';

							/* The remainder of the string */
							tmp_int = म_से_प(&tmp_char[1]);
							यदि(tmp_int < 1 || tmp_int > UINT16_MAX){
								log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge port value (%d).", tmp_int);
								फर्काऊ MOSQ_ERR_INVAL;
							}
							cur_bridge->addresses[i].port = (uint16_t)tmp_int;
						}अन्यथा{
							cur_bridge->addresses[i].port = 1883;
						}
						conf__attempt_resolve(cur_bridge->addresses[i].address, "bridge address", MOSQ_LOG_WARNING, "Warning");
					}
					यदि(cur_bridge->address_गन्ती == 0){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty address value in configuration.");
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "allow_anonymous")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					PER_LISTENER_ALTERNATIVE(token, "listener_allow_anonymous");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					यदि(conf__parse_bool(&token, "allow_anonymous", (bool *)&cur_security_options->allow_anonymous, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "allow_duplicate_messages")){
					OPTION_DEPRECATED(token, "The behaviour will default to true.");
					यदि(conf__parse_bool(&token, "allow_duplicate_messages", &config->allow_duplicate_messages, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "allow_zero_length_clientid")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					यदि(conf__parse_bool(&token, "allow_zero_length_clientid", &cur_security_options->allow_zero_length_clientid, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेदन(token, "auth_opt_", म_माप("auth_opt_")) || !म_भेदन(token, "plugin_opt_", म_माप("plugin_opt_"))){
					यदि(reload){
						जारी;        /* Auth plugin not currently valid for reloading. */

					}
					REQUIRE_PLUGIN(token);

					यदि(!म_भेदन(token, "auth_opt_", म_माप("auth_opt_"))){
						prefix_len = म_माप("auth_opt_");
					}अन्यथा{
						prefix_len = म_माप("plugin_opt_");
					}
					यदि(म_माप(token) < prefix_len + 3){
						/* auth_opt_ == 9, + one digit key == 10, + one space == 11, + one value == 12 */
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'plugin_opt_' config option.");
						फर्काऊ MOSQ_ERR_INVAL;
					}
					साँचो = mosquitto_strdup(&token[prefix_len]);
					यदि(!साँचो){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
						फर्काऊ MOSQ_ERR_NOMEM;
					}अन्यथा यदि(STREMPTY(साँचो)){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty 'plugin_opt_' config option.");
						mosquitto_FREE(साँचो);
						फर्काऊ MOSQ_ERR_INVAL;
					}
					token = saveptr;
					यदि(token && token[0]){
						जबसम्म(token[0] == ' ' || token[0] == '\t'){
							token++;
						}
						संरचना mosquitto_opt *new_options = mosquitto_realloc(cur_plugin->config.options, (माप_प्रकार)(cur_plugin->config.option_गन्ती+1)*परिमाण(संरचना mosquitto_opt));
						यदि(!new_options){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
							mosquitto_FREE(साँचो);
							फर्काऊ MOSQ_ERR_NOMEM;
						}
						cur_plugin->config.option_गन्ती++;
						cur_plugin->config.options = new_options;
						cur_plugin->config.options[cur_plugin->config.option_गन्ती-1].साँचो = साँचो;
						cur_plugin->config.options[cur_plugin->config.option_गन्ती-1].मान = mosquitto_strdup(token);
						यदि(!cur_plugin->config.options[cur_plugin->config.option_गन्ती-1].मान){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
							फर्काऊ MOSQ_ERR_NOMEM;
						}
					}अन्यथा{
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", साँचो);
						mosquitto_FREE(साँचो);
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "auth_plugin") || !म_भेद(token, "plugin") || !म_भेद(token, "global_plugin")){
					यदि(reload){
						जारी;        /* plugin not currently valid for reloading. */
					}
					यदि(!म_भेद(token, "global_plugin")){
						cur_security_options = &db.config->security_options;
					}अन्यथा{
						REQUIRE_LISTENER_IF_PER_LISTENER(token);
						conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					}

					cur_plugin = config__plugin_load(रिक्त, saveptr);
					यदि(cur_plugin == रिक्त){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(config__plugin_add_secopt(cur_plugin, cur_security_options)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "auth_plugin_deny_special_chars")){
					यदि(reload){
						जारी;        /* Auth plugin not currently valid for reloading. */
					}
					यदि(!cur_plugin){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: An auth_plugin_deny_special_chars option exists in the config file without a plugin.");
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(conf__parse_bool(&token, "auth_plugin_deny_special_chars", &cur_plugin->config.deny_special_chars, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "plugin_load")){
					वर्ण *name = रिक्त;

					यदि(reload){
						जारी;        /* plugin not currently valid for reloading. */

					}
					name = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(name, "plugin_load");

					cur_plugin = config__plugin_load(name, saveptr);
					यदि(cur_plugin == रिक्त){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "plugin_use")){
					वर्ण *name = रिक्त;

					यदि(reload){
						जारी;        /* plugin not currently valid for reloading. */
					}
					REQUIRE_NON_DEFAULT_LISTENER(token);

					यदि(conf__parse_string(&token, "plugin_use", &name, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					cur_plugin = config__plugin_find(name);
					यदि(!cur_plugin){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Plugin '%s' not previously loaded.", name);
						mosquitto_FREE(name);
						फर्काऊ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(name);
					यदि(config__plugin_add_secopt(cur_plugin, cur_listener->security_options)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "auto_id_prefix")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					OPTION_DEPRECATED(token, "Please use 'listener_auto_id_prefix' instead.");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					यदि(conf__parse_string(&token, "auto_id_prefix", &cur_security_options->auto_id_prefix, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(cur_security_options->auto_id_prefix){
						यदि(म_माप(cur_security_options->auto_id_prefix) > 50){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: auto_id_prefix length must be <= 50.");
							फर्काऊ MOSQ_ERR_INVAL;
						}
						cur_security_options->auto_id_prefix_len = (uint16_t)म_माप(cur_security_options->auto_id_prefix);
					}अन्यथा{
						cur_security_options->auto_id_prefix_len = 0;
					}
				}अन्यथा यदि(!म_भेद(token, "autosave_interval")){
					यदि(conf__parse_int(&token, "autosave_interval", &config->autosave_interval, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(config->autosave_interval < 0){
						config->autosave_interval = 0;
					}
				}अन्यथा यदि(!म_भेद(token, "autosave_on_changes")){
					यदि(conf__parse_bool(&token, "autosave_on_changes", &config->autosave_on_changes, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "bind_address")){
					OPTION_DEPRECATED(token, "");
					config->local_only = false;
					यदि(reload){
						जारी;        /* Rebinding listeners not valid during reloading. */

					}
					यदि(config__create_default_listener(config, token)){
						फर्काऊ MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					यदि(conf__parse_string(&token, "default listener bind_address", &config->default_listener->host, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(conf__attempt_resolve(config->default_listener->host, "bind_address", MOSQ_LOG_ERR, "Error")){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "bind_interface")){
#ifdef SO_BINDTODEVICE
					यदि(reload){
						जारी;        /* Rebinding listeners not valid during reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					यदि(conf__parse_string(&token, "bind_interface", &cur_listener->bind_interface, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_ERR, "Error: bind_interface specified but socket option not available.");
					फर्काऊ MOSQ_ERR_INVAL;
#endif
				}अन्यथा यदि(!म_भेद(token, "bridge_attempt_unsubscribe")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_bool(&token, "bridge_attempt_unsubscribe", &cur_bridge->attempt_unsubscribe, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "bridge_cafile")){
#यदि defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					यदि(conf__parse_string(&token, "bridge_cafile", &cur_bridge->tls_cafile, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "bridge_alpn")){
#यदि defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_string(&token, "bridge_alpn", &cur_bridge->tls_alpn, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "bridge_ciphers")){
#यदि defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_string(&token, "bridge_ciphers", &cur_bridge->tls_ciphers, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "bridge_ciphers_tls1.3")){
#यदि defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_string(&token, "bridge_ciphers_tls1.3", &cur_bridge->tls_13_ciphers, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "bridge_bind_address")){
#यदि defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_string(&token, "bridge_bind_address", &cur_bridge->bind_address, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "bridge_capath")){
#यदि defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					यदि(conf__parse_string(&token, "bridge_capath", &cur_bridge->tls_capath, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "bridge_certfile")){
#यदि defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					यदि(conf__parse_string(&token, "bridge_certfile", &cur_bridge->tls_certfile, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "bridge_identity")){
#यदि defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					यदि(conf__parse_string(&token, "bridge_identity", &cur_bridge->tls_psk_identity, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "bridge_insecure")){
#यदि defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_bool(&token, "bridge_insecure", &cur_bridge->tls_insecure, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(cur_bridge->tls_insecure){
						log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge '%s' using insecure mode.", cur_bridge->name);
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "bridge_require_ocsp")){
#यदि defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_bool(&token, "bridge_require_ocsp", &cur_bridge->tls_ocsp_required, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "bridge_max_packet_size")){
#यदि defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_int(&token, "bridge_max_packet_size", &tmp_int, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 0){
						tmp_int = 0;
					}
					cur_bridge->maximum_packet_size = (uint32_t)tmp_int;
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "bridge_max_topic_alias")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_int(&token, "bridge_max_topic_alias", &tmp_int, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}

					यदि(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: bridge_max_topic_alias must be > 0 and <= 65535.");
						फर्काऊ MOSQ_ERR_INVAL;
					}
					cur_bridge->max_topic_alias = (uint16_t)tmp_int;
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "bridge_outgoing_retain")){
#यदि defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_bool(&token, "bridge_outgoing_retain", &cur_bridge->outgoing_retain, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "bridge_keyfile")){
#यदि defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					mosquitto_FREE(cur_bridge->tls_keyfile);
					यदि(conf__parse_string(&token, "bridge_keyfile", &cur_bridge->tls_keyfile, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "bridge_protocol_version")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(रिक्त, "", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_protocol_version");

					यदि(!म_भेद(token, "mqttv31")){
						cur_bridge->protocol_version = mosq_p_mqtt31;
					}अन्यथा यदि(!म_भेद(token, "mqttv311")){
						cur_bridge->protocol_version = mosq_p_mqtt311;
					}अन्यथा यदि(!म_भेद(token, "mqttv50")){
						cur_bridge->protocol_version = mosq_p_mqtt5;
					}अन्यथा{
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'bridge_protocol_version' value (%s).", token);
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "bridge_psk")){
#यदि defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					यदि(conf__parse_string(&token, "bridge_psk", &cur_bridge->tls_psk, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "bridge_receive_maximum")){
#यदि defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_int(&token, "bridge_receive_maximum", &tmp_int, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(tmp_int <= 0){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be greater than 0.");
						फर्काऊ MOSQ_ERR_INVAL;
					}अन्यथा यदि((uint64_t)tmp_int > (uint64_t)UINT16_MAX){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be lower than %u.", UINT16_MAX);
						फर्काऊ MOSQ_ERR_INVAL;
					}
					cur_bridge->receive_maximum = (uint16_t)tmp_int;
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "bridge_reload_type")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_reload_type");

					यदि(!म_भेद(token, "lazy")){
						cur_bridge->reload_type = brt_lazy;
					}अन्यथा यदि(!म_भेद(token, "immediate")){
						cur_bridge->reload_type = brt_immediate;
					}अन्यथा{
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'bridge_reload_type' value in configuration (%s).", token);
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "bridge_session_expiry_interval")){
#यदि defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_int(&token, "bridge_session_expiry_interval", &tmp_int, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 0){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must not be negative.");
						फर्काऊ MOSQ_ERR_INVAL;
					}अन्यथा यदि((uint64_t)tmp_int > (uint64_t)MQTT_SESSION_EXPIRY_NEVER){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must be lower than %u.", MQTT_SESSION_EXPIRY_NEVER);
						फर्काऊ MOSQ_ERR_INVAL;
					}
					cur_bridge->session_expiry_interval = (uint32_t)tmp_int;
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "bridge_tcp_keepalive")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);

					यदि(conf__parse_int(&token, "bridge_tcp_keepalive_idle", &tmp_int, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(tmp_int <= 0){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: invalid TCP keepalive idle value.");
						फर्काऊ MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_idle = (अचिन्हित पूर्णाङ्क)tmp_int;

					यदि(conf__parse_int(&token, "bridge_tcp_keepalive_interval", &tmp_int, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(tmp_int <= 0){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: invalid TCP keepalive interval value.");
						फर्काऊ MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_interval = (अचिन्हित पूर्णाङ्क)tmp_int;

					यदि(conf__parse_int(&token, "bridge_tcp_keepalive_counter", &tmp_int, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(tmp_int <= 0){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: invalid TCP keepalive counter value.");
						फर्काऊ MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_counter = (अचिन्हित पूर्णाङ्क)tmp_int;
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "bridge_tcp_user_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
#ifdef WITH_TCP_USER_TIMEOUT
					यदि(conf__parse_int(&token, "bridge_tcp_user_timeout", &tmp_int, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 0){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: invalid TCP user timeout value.");
						फर्काऊ MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_user_timeout = tmp_int;
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge TCP user timeout support not available.");
#endif
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "bridge_tls_use_os_certs")){
#यदि defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_bool(&token, "bridge_tls_use_os_certs", &cur_bridge->tls_use_os_certs, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "bridge_tls_version")){
#यदि defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_string(&token, "bridge_tls_version", &cur_bridge->tls_version, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "cafile")){
#यदि defined(WITH_TLS)
					REQUIRE_LISTENER(token);
					यदि(cur_listener->psk_hint){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						फर्काऊ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->cafile);
					यदि(conf__parse_string(&token, "cafile", &cur_listener->cafile, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "capath")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->capath);
					यदि(conf__parse_string(&token, "capath", &cur_listener->capath, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "certfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					यदि(cur_listener->psk_hint){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						फर्काऊ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->certfile);
					यदि(conf__parse_string(&token, "certfile", &cur_listener->certfile, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "check_retain_source")){
					यदि(conf__parse_bool(&token, "check_retain_source", &config->check_retain_source, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "ciphers")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers);
					यदि(conf__parse_string(&token, "ciphers", &cur_listener->ciphers, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "ciphers_tls1.3")){
#यदि defined(WITH_TLS) && (!defined(LIBRESSL_VERSION_सङ्ख्या) || LIBRESSL_VERSION_सङ्ख्या > 0x3040000FL)
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers_tls13);
					यदि(conf__parse_string(&token, "ciphers_tls1.3", &cur_listener->ciphers_tls13, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: ciphers_tls1.3 support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "clientid") || !म_भेद(token, "remote_clientid")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_string(&token, "bridge remote clientid", &cur_bridge->remote_clientid, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "cleansession")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_bool(&token, "cleansession", &cur_bridge->clean_start, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "local_cleansession")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_bool(&token, "local_cleansession", (bool *)&cur_bridge->clean_start_local, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "clientid_prefixes")){
					OPTION_DEPRECATED(token, "");
					mosquitto_FREE(config->clientid_prefixes);
					यदि(conf__parse_string(&token, "clientid_prefixes", &config->clientid_prefixes, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "connection")){
#ifdef WITH_BRIDGE
					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "connection");

					/* Check for existing bridge name. */
					लागि(i=0; i<config->bridge_गन्ती; i++){
						यदि(!म_भेद(config->bridges[i]->name, token)){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Duplicate bridge name '%s'.", token);
							फर्काऊ MOSQ_ERR_INVAL;
						}
					}

					संरचना mosquitto__bridge **bridges_new = mosquitto_realloc(config->bridges, (माप_प्रकार)(config->bridge_गन्ती+1)*परिमाण(संरचना mosquitto__bridge *));
					यदि(!bridges_new){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
						फर्काऊ MOSQ_ERR_NOMEM;
					}
					config->bridges = bridges_new;
					cur_bridge = mosquitto_malloc(परिमाण(संरचना mosquitto__bridge));
					यदि(!cur_bridge){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
						फर्काऊ MOSQ_ERR_NOMEM;
					}

					config->bridge_गन्ती++;
					config->bridges[config->bridge_गन्ती-1] = cur_bridge;

					स_रखो(cur_bridge, 0, परिमाण(संरचना mosquitto__bridge));
					cur_bridge->name = mosquitto_strdup(token);
					यदि(!cur_bridge->name){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
						फर्काऊ MOSQ_ERR_NOMEM;
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
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "connection_messages")){
					यदि(conf__parse_bool(&token, token, &config->connection_messages, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "crlfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->crlfile);
					यदि(conf__parse_string(&token, "crlfile", &cur_listener->crlfile, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "dhparamfile")){
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: dhparamfile is no longer required.");
				}अन्यथा यदि(!म_भेद(token, "disable_client_cert_date_checks")){
#ifdef WITH_TLS
					REQUIRE_LISTENER(token);
					यदि(conf__parse_bool(&token, "disable_client_cert_date_checks", &cur_listener->disable_client_cert_date_checks, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "enable_control_api")){
#ifdef WITH_CONTROL
					यदि(conf__parse_bool(&token, "enable_control_api", &config->enable_control_api, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: $CONTROL support not available (enable_control_api).");
#endif
				}अन्यथा यदि(!म_भेद(token, "enable_proxy_protocol")){
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(रिक्त, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					फर्काऊ MOSQ_ERR_INVAL;
#endif
					REQUIRE_LISTENER(token);
					यदि(conf__parse_int(&token, "enable_proxy_protocol", &cur_listener->enable_proxy_protocol, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(cur_listener->enable_proxy_protocol < 1 || cur_listener->enable_proxy_protocol > 2){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: enable_proxy_protocol must be 1 or 2.");
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "global_max_clients")){
					यदि(conf__parse_int(&token, "global_max_clients", &config->global_max_clients, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "global_max_connections")){
					यदि(conf__parse_int(&token, "global_max_connections", &config->global_max_connections, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "http_dir")){
#यदि defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
					यदि(reload){
						जारी;        /* Not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					यदि(conf__parse_string(&token, "http_dir", &cur_listener->http_dir, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#ifdef WIN32
					वर्ण *http_dir_canonical = _fullpath(रिक्त, cur_listener->http_dir, 0);
					अचर वर्ण sep = '\\';
#अन्यथा
					वर्ण *http_dir_canonical = realpath(cur_listener->http_dir, रिक्त);
					अचर वर्ण sep = '/';
#endif
					यदि(!http_dir_canonical){
						फर्काऊ MOSQ_ERR_NOMEM;
					}
					माप_प्रकार http_dir_len = म_माप(http_dir_canonical) + परिमाण(sep) + 1;
					वर्ण *http_dir_canonical_sep = mosquitto_calloc(http_dir_len, परिमाण(वर्ण));
					यदि(!http_dir_canonical_sep){
						मुक्त(http_dir_canonical);
						फर्काऊ MOSQ_ERR_NOMEM;
					}
					snprintf(http_dir_canonical_sep, http_dir_len, "%s%c", http_dir_canonical, sep);
					मुक्त(http_dir_canonical);
					mosquitto_FREE(cur_listener->http_dir);
					cur_listener->http_dir = http_dir_canonical_sep;
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: http_dir support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "idle_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_int(&token, "idle_timeout", &cur_bridge->idle_timeout, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(cur_bridge->idle_timeout < 1){
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "idle_timeout interval too low, using 1 second.");
						cur_bridge->idle_timeout = 1;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "include_dir")){
					यदि(level == 0){
						वर्ण **files;
						पूर्णाङ्क file_गन्ती;

						/* Only process include_dir from the main config file. */
						token = strtok_r(रिक्त, "", &saveptr);
						REQUIRE_NON_EMPTY_OPTION(token, "include_dir");

						rc = config__get_dir_files(token, &files, &file_गन्ती);
						यदि(rc){
							फर्काऊ rc;
						}

						लागि(i=0; i<file_गन्ती; i++){
							log__printf(रिक्त, MOSQ_LOG_INFO, "Loading config file '%s'", files[i]);

							rc = config__read_file(config, reload, files[i], cr, level+1, &lineno_ext);
							यदि(rc){
								यदि(lineno_ext > 0){
									log__printf(रिक्त, MOSQ_LOG_ERR, "Error found at '%s:%d'.", files[i], lineno_ext);
								}
								/* Free happens below */
								रोक;
							}
						}
						लागि(i=0; i<file_गन्ती; i++){
							mosquitto_FREE(files[i]);
						}
						mosquitto_FREE(files);
						यदि(rc){
							फर्काऊ rc;    /* This returns if config__read_file() fails above */
						}
					}अन्यथा{
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: The include_dir option is only valid in the main configuration file.");
						फर्काऊ 1;
					}
				}अन्यथा यदि(!म_भेद(token, "keepalive_interval")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_int(&token, "keepalive_interval", &tmp_int, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(tmp_int > UINT16_MAX){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Bridge keepalive value too high.");
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 5){
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "keepalive interval too low, using 5 seconds.");
						tmp_int = 5;
					}
					cur_bridge->keepalive = (uint16_t)tmp_int;
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "keyfile")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->keyfile);
					यदि(conf__parse_string(&token, "keyfile", &cur_listener->keyfile, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "listener")){
					config->local_only = false;

					यदि(conf__parse_int(&token, "listener port", &tmp_int, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#ifdef WITH_UNIX_SOCKETS
					यदि(tmp_int < 0 || tmp_int > UINT16_MAX){
#अन्यथा
					यदि(tmp_int < 1 || tmp_int > UINT16_MAX){
#endif
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						फर्काऊ MOSQ_ERR_INVAL;
					}

					/* Look for bind address / unix socket path */
					token = strtok_r(रिक्त, " ", &saveptr);
					यदि(token != रिक्त && token[0] == '#'){
						token = रिक्त;
					}

					यदि(tmp_int == 0 && token == रिक्त){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: A listener with port 0 must provide a Unix socket path.");
						फर्काऊ MOSQ_ERR_INVAL;
					}

					यदि(reload){
						/* We reload listeners settings based on port number/unix socket path.
						 * If the port number/unix path doesn't already exist, exit with a complaint. */
						cur_listener = रिक्त;
#ifdef WITH_UNIX_SOCKETS
						यदि(tmp_int == 0){
							लागि(i=0; i<config->listener_गन्ती; i++){
								यदि(config->listeners[i].unix_socket_path != रिक्त
										&& म_भेद(config->listeners[i].unix_socket_path, token) == 0){

									cur_listener = &config->listeners[i];
									रोक;
								}
							}
						}अन्यथा
#endif
						{
							लागि(i=0; i<config->listener_गन्ती; i++){
								यदि(config->listeners[i].port == tmp_int){
									/* Now check we have a matching bind address, if defined */
									यदि(config->listeners[i].host){
										यदि(token && !म_भेद(config->listeners[i].host, token)){
											/* They both have a bind address, and they match */
											cur_listener = &config->listeners[i];
											रोक;
										}
									}अन्यथा{
										यदि(token == रिक्त){
											/* Neither this config nor the new config have a bind address,
											 * so they match. */
											cur_listener = &config->listeners[i];
											रोक;
										}
									}
								}
							}
						}
						यदि(!cur_listener){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: It is not currently possible to add/remove listeners when reloading the config file.");
							फर्काऊ MOSQ_ERR_INVAL;
						}
					}अन्यथा{
						यदि(config__add_listener(config)){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
							फर्काऊ MOSQ_ERR_NOMEM;
						}
						cur_listener = &config->listeners[config->listener_गन्ती-1];
					}

					listener__set_defaults(cur_listener);
					cur_listener->port = (uint16_t)tmp_int;

					mosquitto_FREE(cur_listener->host);

#ifdef WITH_UNIX_SOCKETS
					mosquitto_FREE(cur_listener->unix_socket_path);
#endif

					यदि(token){
#ifdef WITH_UNIX_SOCKETS
						यदि(cur_listener->port == 0){
							cur_listener->unix_socket_path = mosquitto_strdup(token);
						}अन्यथा
#endif
						{
							cur_listener->host = mosquitto_strdup(token);
						}
					}
				}अन्यथा यदि(!म_भेद(token, "listener_allow_anonymous")){
					REQUIRE_LISTENER(token);
					यदि(conf__parse_bool(&token, "listener_allow_anonymous", (bool *)&cur_listener->security_options->allow_anonymous, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "listener_auto_id_prefix")){
					REQUIRE_LISTENER(token);
					यदि(conf__parse_string(&token, "listener_auto_id_prefix", &cur_listener->security_options->auto_id_prefix, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(cur_listener->security_options->auto_id_prefix){
						यदि(म_माप(cur_listener->security_options->auto_id_prefix) > 50){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: 'listener_auto_id_prefix' length must be <= 50.");
							फर्काऊ MOSQ_ERR_INVAL;
						}
						cur_listener->security_options->auto_id_prefix_len = (uint16_t)म_माप(cur_listener->security_options->auto_id_prefix);
					}अन्यथा{
						cur_listener->security_options->auto_id_prefix_len = 0;
					}
				}अन्यथा यदि(!म_भेद(token, "local_clientid")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_string(&token, "bridge local clientd", &cur_bridge->local_clientid, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "local_password")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_string(&token, "bridge local_password", &cur_bridge->local_password, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "local_username")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_string(&token, "bridge local_username", &cur_bridge->local_username, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "log_dest")){
					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_dest");

					cr->log_dest_set = 1;
					यदि(!म_भेद(token, "none")){
						cr->log_dest = MQTT3_LOG_NONE;
					}अन्यथा यदि(!म_भेद(token, "syslog")){
						cr->log_dest |= MQTT3_LOG_SYSLOG;
					}अन्यथा यदि(!म_भेद(token, "stdout")){
						cr->log_dest |= MQTT3_LOG_STDOUT;
					}अन्यथा यदि(!म_भेद(token, "stderr")){
						cr->log_dest |= MQTT3_LOG_STDERR;
					}अन्यथा यदि(!म_भेद(token, "topic")){
						cr->log_dest |= MQTT3_LOG_TOPIC;
					}अन्यथा यदि(!म_भेद(token, "dlt")){
						cr->log_dest |= MQTT3_LOG_DLT;
#ifdef ANDROID
					}अन्यथा यदि(!म_भेद(token, "android")){
						cr->log_dest |= MQTT3_LOG_ANDROID;
#endif
					}अन्यथा यदि(!म_भेद(token, "file")){
						यदि(config->log_fptr || config->log_file){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Duplicate \"log_dest file\" value.");
							फर्काऊ MOSQ_ERR_INVAL;
						}
						/* Get remaining string. */
						token = saveptr;
						यदि(token && token[0]){
							जबसम्म(token[0] == ' ' || token[0] == '\t'){
								token++;
							}
						}
						/* Duplicate "token" check here saves a log__printf() */
						यदि(token && token[0]){
							config->log_file = mosquitto_strdup(token);
							यदि(!config->log_file){
								log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
								फर्काऊ MOSQ_ERR_NOMEM;
							}
							cr->log_dest |= MQTT3_LOG_FILE;
						}अन्यथा{
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty \"log_dest file\" value in configuration.");
							फर्काऊ MOSQ_ERR_INVAL;
						}
						cr->log_dest |= MQTT3_LOG_FILE;
					}अन्यथा{
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'log_dest' value (%s).", token);
						फर्काऊ MOSQ_ERR_INVAL;
					}
#यदि defined(WIN32) || defined(__CYGWIN__)
					यदि(service_handle){
						यदि(cr->log_dest == MQTT3_LOG_STDOUT || cr->log_dest == MQTT3_LOG_STDERR){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Cannot log to stdout/stderr when running as a Windows service.");
							फर्काऊ MOSQ_ERR_INVAL;
						}
					}
#endif
				}अन्यथा यदि(!म_भेद(token, "log_facility")){
#यदि defined(WIN32) || defined(__CYGWIN__)
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: log_facility not supported on Windows.");
#अन्यथा
					यदि(conf__parse_int(&token, "log_facility", &tmp_int, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					विकल्प(tmp_int){
						पक्ष 0:
							config->log_facility = LOG_LOCAL0;
							रोक;
						पक्ष 1:
							config->log_facility = LOG_LOCAL1;
							रोक;
						पक्ष 2:
							config->log_facility = LOG_LOCAL2;
							रोक;
						पक्ष 3:
							config->log_facility = LOG_LOCAL3;
							रोक;
						पक्ष 4:
							config->log_facility = LOG_LOCAL4;
							रोक;
						पक्ष 5:
							config->log_facility = LOG_LOCAL5;
							रोक;
						पक्ष 6:
							config->log_facility = LOG_LOCAL6;
							रोक;
						पक्ष 7:
							config->log_facility = LOG_LOCAL7;
							रोक;
						स्वभाव:
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'log_facility' value (%d).", tmp_int);
							फर्काऊ MOSQ_ERR_INVAL;
					}
#endif
				}अन्यथा यदि(!म_भेद(token, "log_timestamp")){
					यदि(conf__parse_bool(&token, token, &config->log_timestamp, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "log_timestamp_format")){
					यदि(conf__parse_string(&token, token, &config->log_timestamp_format, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "log_type")){
					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_type");

					cr->log_type_set = 1;
					यदि(!म_भेद(token, "none")){
						cr->log_type = MOSQ_LOG_NONE;
					}अन्यथा यदि(!म_भेद(token, "information")){
						cr->log_type |= MOSQ_LOG_INFO;
					}अन्यथा यदि(!म_भेद(token, "notice")){
						cr->log_type |= MOSQ_LOG_NOTICE;
					}अन्यथा यदि(!म_भेद(token, "warning")){
						cr->log_type |= MOSQ_LOG_WARNING;
					}अन्यथा यदि(!म_भेद(token, "error")){
						cr->log_type |= MOSQ_LOG_ERR;
					}अन्यथा यदि(!म_भेद(token, "debug")){
						cr->log_type |= MOSQ_LOG_DEBUG;
					}अन्यथा यदि(!म_भेद(token, "subscribe")){
						cr->log_type |= MOSQ_LOG_SUBSCRIBE;
					}अन्यथा यदि(!म_भेद(token, "unsubscribe")){
						cr->log_type |= MOSQ_LOG_UNSUBSCRIBE;
					}अन्यथा यदि(!म_भेद(token, "internal")){
						cr->log_type |= MOSQ_LOG_INTERNAL;
#ifdef WITH_WEBSOCKETS
					}अन्यथा यदि(!म_भेद(token, "websockets")){
						cr->log_type |= MOSQ_LOG_WEBSOCKETS;
#endif
					}अन्यथा यदि(!म_भेद(token, "all")){
						cr->log_type = MOSQ_LOG_ALL;
					}अन्यथा{
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'log_type' value (%s).", token);
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "max_connections")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					यदि(conf__parse_int(&token, token, &cur_listener->max_connections, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(cur_listener->max_connections < 0){
						cur_listener->max_connections = -1;
					}
				}अन्यथा यदि(!म_भेद(token, "maximum_qos") || !म_भेद(token, "max_qos")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					यदि(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 0 || tmp_int > 2){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: 'max_qos' must be between 0 and 2 inclusive.");
						फर्काऊ MOSQ_ERR_INVAL;
					}
					cur_listener->max_qos = (uint8_t)tmp_int;
				}अन्यथा यदि(!म_भेद(token, "max_inflight_bytes")){
					यदि(conf__parse_int(&token, "max_inflight_bytes", &tmp_int, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_inflight_bytes = (माप_प्रकार)tmp_int;
				}अन्यथा यदि(!म_भेद(token, "max_inflight_messages")){
					यदि(conf__parse_int(&token, "max_inflight_messages", &tmp_int, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 0 || tmp_int == UINT16_MAX){
						tmp_int = 0;
					}अन्यथा यदि(tmp_int > UINT16_MAX){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: 'max_inflight_messages' must be <= 65535.");
						फर्काऊ MOSQ_ERR_INVAL;
					}
					config->max_inflight_messages = (uint16_t)tmp_int;
				}अन्यथा यदि(!म_भेद(token, "max_keepalive")){
					यदि(conf__parse_int(&token, "max_keepalive", &tmp_int, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'max_keepalive' value (%d).", tmp_int);
						फर्काऊ MOSQ_ERR_INVAL;
					}
					config->max_keepalive = (uint16_t)tmp_int;
				}अन्यथा यदि(!म_भेद(token, "max_packet_size")){
					यदि(conf__parse_int(&token, "max_packet_size", &tmp_int, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 20){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: 'max_packet_size' must be >= 20.");
						फर्काऊ MOSQ_ERR_INVAL;
					}
					config->max_packet_size = (uint32_t)tmp_int;
				}अन्यथा यदि(!म_भेद(token, "max_queued_bytes")){
					यदि(conf__parse_int(&token, "max_queued_bytes", &tmp_int, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_bytes = (माप_प्रकार)tmp_int;
				}अन्यथा यदि(!म_भेद(token, "max_queued_messages")){
					यदि(conf__parse_int(&token, "max_queued_messages", &tmp_int, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_messages = tmp_int;
				}अन्यथा यदि(!म_भेद(token, "memory_limit")){
					ssize_t lim;
					यदि(conf__parse_ssize_t(&token, "memory_limit", &lim, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(lim < 0){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'memory_limit' value (%ld).", lim);
						फर्काऊ MOSQ_ERR_INVAL;
					}
					mosquitto_memory_set_limit((माप_प्रकार)lim);
				}अन्यथा यदि(!म_भेद(token, "message_size_limit")){
					log__printf(रिक्त, MOSQ_LOG_NOTICE, "Note: It is recommended to replace `message_size_limit` with `max_packet_size`.");
					यदि(conf__parse_int(&token, "message_size_limit", (पूर्णाङ्क *)&config->message_size_limit, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(config->message_size_limit > MQTT_MAX_PAYLOAD){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'message_size_limit' value (%u).", config->message_size_limit);
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "mount_point")){
					REQUIRE_LISTENER(token);
					mosquitto_FREE(cur_listener->mount_point);
					यदि(conf__parse_string(&token, "mount_point", &cur_listener->mount_point, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(mosquitto_pub_topic_check(cur_listener->mount_point) != MOSQ_ERR_SUCCESS){
						log__printf(रिक्त, MOSQ_LOG_ERR,
								"Error: Invalid 'mount_point' value (%s). Does it contain a wildcard character?",
								cur_listener->mount_point);
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "notifications")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_bool(&token, "notifications", &cur_bridge->notifications, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "notifications_local_only")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_bool(&token, "notifications_local_only", &cur_bridge->notifications_local_only, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "notification_topic")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_string(&token, "notification_topic", &cur_bridge->notification_topic, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "password") || !म_भेद(token, "remote_password")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_string(&token, "bridge remote_password", &cur_bridge->remote_password, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "password_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->password_data.password_file);
					यदि(conf__parse_string(&token, "password_file", &cur_security_options->password_data.password_file, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "per_listener_settings")){
					OPTION_DEPRECATED(token, "Please see the documentation for how to achieve the same effect.");
					यदि(config->per_listener_settings){
						/* Once this is set, don't let it be unset. It should be the first config option ideally. */
						जारी;
					}
					यदि(conf__parse_bool(&token, "per_listener_settings", &config->per_listener_settings, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(cur_security_options && config->per_listener_settings){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: per_listener_settings must be set before any other security settings.");
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "persistence") || !म_भेद(token, "retained_persistence")){
					यदि(conf__parse_bool(&token, token, &config->persistence, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "persistence_file")){
					यदि(conf__parse_string(&token, "persistence_file", &config->persistence_file, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "persistence_location")){
					यदि(conf__parse_string(&token, "persistence_location", &config->persistence_location, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "persistent_client_expiration")){
					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "persistent_client_expiration");

					समय_प्रकार expiration_mult;

					विकल्प(token[म_माप(token)-1]){
						पक्ष 's':
							expiration_mult = 1;
							रोक;
						पक्ष 'h':
							expiration_mult = 3600;
							रोक;
						पक्ष 'd':
							expiration_mult = 86400;
							रोक;
						पक्ष 'w':
							expiration_mult = 86400*7;
							रोक;
						पक्ष 'm':
							expiration_mult = 86400*30;
							रोक;
						पक्ष 'y':
							expiration_mult = 86400*365;
							रोक;
						स्वभाव:
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
							फर्काऊ MOSQ_ERR_INVAL;
					}
					token[म_माप(token)-1] = '\0';
					config->persistent_client_expiration = म_से_प(token)*expiration_mult;
					यदि(config->persistent_client_expiration <= 0){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "pid_file")){
					यदि(reload){
						जारी;        /* pid file not valid for reloading. */
					}
					यदि(conf__parse_string(&token, "pid_file", &config->pid_file, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "port")){
					OPTION_DEPRECATED(token, "Please use 'listener' instead.");
					config->local_only = false;
					यदि(reload){
						जारी;        /* Listeners not valid for reloading. */

					}
					यदि(config__create_default_listener(config, token)){
						फर्काऊ MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					यदि(config->default_listener->port){
						log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Default listener port specified multiple times. Only the latest will be used.");
					}
					यदि(conf__parse_int(&token, "port", &tmp_int, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 1 || tmp_int > UINT16_MAX){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						फर्काऊ MOSQ_ERR_INVAL;
					}
					config->default_listener->port = (uint16_t)tmp_int;
				}अन्यथा यदि(!म_भेद(token, "protocol")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "protocol");

					यदि(!म_भेद(token, "mqtt")){
						cur_listener->protocol = mp_mqtt;
						/*
						}else if(!strcmp(token, "mqttsn")){
						    cur_listener->protocol = mp_mqttsn;
						*/
					}अन्यथा यदि(!म_भेद(token, "websockets")){
#ifdef WITH_WEBSOCKETS
						cur_listener->protocol = mp_websockets;
#अन्यथा
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Websockets support not available.");
						फर्काऊ MOSQ_ERR_INVAL;
#endif
					}अन्यथा यदि(!म_भेद(token, "http_api")){
#ifdef WITH_HTTP_API
						cur_listener->protocol = mp_http_api;
#अन्यथा
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: HTTP API support not available.");
						फर्काऊ MOSQ_ERR_INVAL;
#endif
					}अन्यथा{
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'protocol' value (%s).", token);
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "proxy_protocol_v2_require_tls")){
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(रिक्त, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					फर्काऊ MOSQ_ERR_INVAL;
#endif
					REQUIRE_LISTENER(token);
					यदि(conf__parse_bool(&token, "proxy_protocol_v2_require_tls", &cur_listener->proxy_protocol_v2_require_tls, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "psk_file")){
#ifdef FINAL_WITH_TLS_PSK
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					यदि(cur_listener && cur_listener->certfile){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						फर्काऊ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_security_options->psk_file);
					यदि(conf__parse_string(&token, "psk_file", &cur_security_options->psk_file, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "psk_hint")){
#ifdef FINAL_WITH_TLS_PSK
					यदि(reload){
						जारी;        /* PSK file not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					यदि(conf__parse_string(&token, "psk_hint", &cur_listener->psk_hint, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "queue_qos0_messages")){
					यदि(conf__parse_bool(&token, token, &config->queue_qos0_messages, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "require_certificate")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					यदि(conf__parse_bool(&token, "require_certificate", &cur_listener->require_certificate, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "restart_timeout")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					cur_bridge->backoff_cap = 0; /* set backoff to constant mode, unless cap is specified further down */
					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "restart_timeout");

					cur_bridge->restart_timeout = म_से_प(token);
					cur_bridge->backoff_base = 0;
					cur_bridge->backoff_cap = 0;
					यदि(cur_bridge->restart_timeout < 1){
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "restart_timeout interval too low, using 1 second.");
						cur_bridge->restart_timeout = 1;
					}अन्यथा यदि(cur_bridge->restart_timeout > 3600){
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "restart_timeout interval too high, using 3600 seconds.");
						cur_bridge->restart_timeout = 3600;
					}
					token = strtok_r(रिक्त, " ", &saveptr);
					यदि(token){
						cur_bridge->backoff_base = cur_bridge->restart_timeout;
						cur_bridge->backoff_cap = म_से_प(token);
						यदि(cur_bridge->backoff_cap < cur_bridge->backoff_base){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: backoff cap is lower than the base in restart_timeout.");
							फर्काऊ MOSQ_ERR_INVAL;
						}अन्यथा यदि(cur_bridge->backoff_cap > 7200){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: backoff cap too high, using 7200 seconds.");
							cur_bridge->backoff_cap = 7200;
						}

						token = strtok_r(रिक्त, " ", &saveptr);
						यदि(token){
							cur_bridge->stable_connection_period = म_से_प(token);
							यदि(cur_bridge->stable_connection_period < 0){
								log__printf(रिक्त, MOSQ_LOG_ERR, "Error: stable connection period cannot be negative.");
								फर्काऊ MOSQ_ERR_INVAL;
							}
						}
					}
					cur_bridge->restart_timeout *= 1000; /* backoff is tracked in ms */
					cur_bridge->backoff_base *= 1000;
					cur_bridge->backoff_cap *= 1000;
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "retain_available")){
					यदि(conf__parse_bool(&token, token, &config->retain_available, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "retain_expiry_interval")){
					यदि(conf__parse_int(&token, token, &config->retain_expiry_interval, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(config->retain_expiry_interval < 1){
						log__printf(रिक्त, MOSQ_LOG_WARNING, "Error: retain_expiry_interval must be >= 1.");
						फर्काऊ MOSQ_ERR_INVAL;
					}अन्यथा यदि(config->retain_expiry_interval > 10000000){
						log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: retain_expiry_interval being capped at 19 years.");
						config->retain_expiry_interval = 10000000;
					}
					config->retain_expiry_interval *= 60;
				}अन्यथा यदि(!म_भेद(token, "retry_interval")){
					OPTION_UNAVAILABLE(token);
				}अन्यथा यदि(!म_भेद(token, "round_robin")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_bool(&token, "round_robin", &cur_bridge->round_robin, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "set_tcp_nodelay")){
					यदि(conf__parse_bool(&token, "set_tcp_nodelay", &config->set_tcp_nodelay, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "start_type")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					यदि(!म_भेद(token, "automatic")){
						cur_bridge->start_type = bst_automatic;
					}अन्यथा यदि(!म_भेद(token, "lazy")){
						cur_bridge->start_type = bst_lazy;
					}अन्यथा यदि(!म_भेद(token, "manual")){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Manual start_type not supported.");
						फर्काऊ MOSQ_ERR_INVAL;
					}अन्यथा यदि(!म_भेद(token, "once")){
						cur_bridge->start_type = bst_once;
					}अन्यथा{
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'start_type' value in configuration (%s).", token);
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "socket_domain")){
					यदि(reload){
						जारी;        /* socket_domain not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					यदि(!म_भेद(token, "ipv4")){
						cur_listener->socket_domain = AF_INET;
					}अन्यथा यदि(!म_भेद(token, "ipv6")){
						cur_listener->socket_domain = AF_INET6;
					}अन्यथा{
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'socket_domain' value '%s' in configuration.", token);
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "sys_interval")){
					यदि(conf__parse_int(&token, "sys_interval", &config->sys_interval, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(config->sys_interval < 0 || config->sys_interval > 65535){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'sys_interval' value (%d).", config->sys_interval);
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "threshold")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_int(&token, "threshold", &cur_bridge->threshold, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(cur_bridge->threshold < 1){
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "threshold too low, using 1 message.");
						cur_bridge->threshold = 1;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "tls_engine")){
#ifdef WITH_TLS
					यदि(reload){
						जारी;        /* tls_engine not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					यदि(conf__parse_string(&token, "tls_engine", &cur_listener->tls_engine, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "tls_engine_kpass_sha1")){
#ifdef WITH_TLS
					यदि(reload){
						जारी;        /* tls_engine not valid for reloading. */

					}
					वर्ण *kpass_sha = रिक्त, *kpass_sha_bin = रिक्त;
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					यदि(conf__parse_string(&token, "tls_engine_kpass_sha1", &kpass_sha, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(mosquitto__hex2bin_sha1(kpass_sha, (अचिन्हित वर्ण **)&kpass_sha_bin) != MOSQ_ERR_SUCCESS){
						mosquitto_FREE(kpass_sha);
						फर्काऊ MOSQ_ERR_INVAL;
					}
					mosquitto_free(cur_listener->tls_engine_kpass_sha1);
					cur_listener->tls_engine_kpass_sha1 = kpass_sha_bin;
					mosquitto_FREE(kpass_sha);
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "tls_keyform")){
#ifdef WITH_TLS
					यदि(reload){
						जारी;        /* tls_engine not valid for reloading. */

					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					वर्ण *keyform = रिक्त;
					यदि(conf__parse_string(&token, "tls_keyform", &keyform, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					cur_listener->tls_keyform = mosq_k_pem;
					यदि(!म_भेद(keyform, "engine")){
						cur_listener->tls_keyform = mosq_k_engine;
					}
					mosquitto_FREE(keyform);
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "tls_version")){
#यदि defined(WITH_TLS)
					यदि(reload){
						जारी;        /* tls_version not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					यदि(conf__parse_string(&token, "tls_version", &cur_listener->tls_version, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "topic")){
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
					यदि(token[0] == '"' && token [1] != '"'){
						यदि(म_अक्षर(saveptr, '"') == रिक्त){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Missing closing quote in topic value (%s).", saveptr);
							फर्काऊ MOSQ_ERR_INVAL;
						}

						वर्ण *topic_in_quotes = strtok_r(रिक्त, "\"", &saveptr);
						माप_प्रकार tlen = 1;
						यदि(topic_in_quotes){
							tlen = म_माप(topic_in_quotes);
						}

						topic = mosquitto_malloc(म_माप(token) + tlen + 1);
						यदि(!topic){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
							फर्काऊ MOSQ_ERR_NOMEM;
						}

						म_नकल(topic, token + 1);
						म_जोड़ो(topic, " ");
						यदि(topic_in_quotes){
							म_जोड़ो(topic, topic_in_quotes);
						}
					}अन्यथा{
						topic = mosquitto_strdup(token);
						यदि(!topic){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
							फर्काऊ MOSQ_ERR_NOMEM;
						}
					}

					token = strtok_r(रिक्त, " ", &saveptr);
					यदि(token){
						यदि(!strcasecmp(token, "out")){
							direction = bd_out;
						}अन्यथा यदि(!strcasecmp(token, "in")){
							direction = bd_in;
						}अन्यथा यदि(!strcasecmp(token, "both")){
							direction = bd_both;
						}अन्यथा{
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge topic direction '%s'.", token);
							mosquitto_FREE(topic);
							फर्काऊ MOSQ_ERR_INVAL;
						}
						token = strtok_r(रिक्त, " ", &saveptr);
						यदि(token){
							यदि(token[0] == '#'){
								(शून्य)strtok_r(रिक्त, "", &saveptr);
							}
							qos = (uint8_t)म_से_प(token);
							यदि(qos > 2){
								log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge QoS level '%s'.", token);
								mosquitto_FREE(topic);
								फर्काऊ MOSQ_ERR_INVAL;
							}

							token = strtok_r(रिक्त, " ", &saveptr);
							यदि(token){
								यदि(!म_भेद(token, "\"\"") || token[0] == '#'){
									local_prefix = रिक्त;
									यदि(token[0] == '#'){
										(शून्य)strtok_r(रिक्त, "", &saveptr);
									}
								}अन्यथा{
									local_prefix = token;
								}

								token = strtok_r(रिक्त, " ", &saveptr);
								यदि(token){
									यदि(!म_भेद(token, "\"\"") || token[0] == '#'){
										remote_prefix = रिक्त;
									}अन्यथा{
										remote_prefix = token;
									}
								}
							}
						}
					}

					यदि(bridge__add_topic(cur_bridge, topic, direction, qos, local_prefix, remote_prefix)){
						mosquitto_FREE(topic);
						फर्काऊ MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(topic);
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "max_topic_alias")){
					REQUIRE_LISTENER(token);
					यदि(conf__parse_int(&token, "max_topic_alias", &tmp_int, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias' value in configuration.");
						फर्काऊ MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias = (uint16_t)tmp_int;
				}अन्यथा यदि(!म_भेद(token, "max_topic_alias_broker")){
					REQUIRE_LISTENER(token);
					यदि(conf__parse_int(&token, "max_topic_alias_broker", &tmp_int, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias_broker' value in configuration.");
						फर्काऊ MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias_broker = (uint16_t)tmp_int;
				}अन्यथा यदि(!म_भेद(token, "try_private")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_bool(&token, "try_private", &cur_bridge->try_private, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "upgrade_outgoing_qos")){
					यदि(conf__parse_bool(&token, token, &config->upgrade_outgoing_qos, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "use_identity_as_username")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					यदि(conf__parse_bool(&token, "use_identity_as_username", &cur_listener->use_identity_as_username, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "use_subject_as_username")){
#ifdef WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					यदि(conf__parse_bool(&token, "use_subject_as_username", &cur_listener->use_subject_as_username, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "user")){
					यदि(reload){
						जारी;        /* Drop privileges user not valid for reloading. */
					}
					mosquitto_FREE(config->user);
					यदि(conf__parse_string(&token, "user", &config->user, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "use_username_as_clientid")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					यदि(conf__parse_bool(&token, "use_username_as_clientid", &cur_listener->use_username_as_clientid, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!म_भेद(token, "username") || !म_भेद(token, "remote_username")){
#ifdef WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_string(&token, "bridge remote_username", &cur_bridge->remote_username, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#endif
				}अन्यथा यदि(!म_भेद(token, "websockets_log_level")){
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					यदि(conf__parse_int(&token, "websockets_log_level", &config->websockets_log_level, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
#endif
				}अन्यथा यदि(!म_भेद(token, "websockets_headers_size") || !म_भेद(token, "packet_buffer_size")){
					यदि(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(रिक्त, MOSQ_LOG_WARNING, "Error: Packet buffer size must be between 0 and 65535 inclusive.");
						फर्काऊ MOSQ_ERR_INVAL;
					}
					config->packet_buffer_size = (uint16_t)tmp_int;
				}अन्यथा यदि(!म_भेद(token, "websockets_origin")){
#ifdef WITH_WEBSOCKETS
#  यदि LWS_LIBRARY_VERSION_सङ्ख्या >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN
					REQUIRE_LISTENER(token);
					ws_origins = mosquitto_realloc(cur_listener->ws_origins, परिमाण(वर्ण *)*(माप_प्रकार)(cur_listener->ws_origin_गन्ती+1));
					यदि(ws_origins == रिक्त){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
						फर्काऊ MOSQ_ERR_NOMEM;
					}
					ws_origins[cur_listener->ws_origin_गन्ती] = रिक्त;
					cur_listener->ws_origins = ws_origins;
					यदि(conf__parse_string(&token, "websockets_origin", &ws_origins[cur_listener->ws_origin_गन्ती], &saveptr)){
						फर्काऊ MOSQ_ERR_INVAL;
					}
					cur_listener->ws_origin_गन्ती++;
#  अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: websockets_origin support not available, libwebsockets version is too old.");
#  endif
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Websockets support not available.");
#endif
				}अन्यथा{
					log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unknown configuration variable '%s'.", token);
					फर्काऊ MOSQ_ERR_INVAL;
				}
			}
		}
	}
	फर्काऊ MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क config__read_file(संरचना mosquitto__config *config, bool reload, अचर वर्ण *file, संरचना config_recurse *cr, पूर्णाङ्क level, पूर्णाङ्क *lineno)
{
	पूर्णाङ्क rc;
	खाता *fptr = रिक्त;
	वर्ण *buf;
	पूर्णाङ्क buflen;
#ifndef WIN32
	सूची *dir;
#endif

#ifndef WIN32
	dir = सूची_खोलो(file);
	यदि(dir){
		बंद_सूची(dir);
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Config file '%s' is a directory.", file);
		फर्काऊ 1;
	}
#endif

	fptr = mosquitto_fopen(file, "rt", false);
	यदि(!fptr){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to open config file '%s'.", file);
		फर्काऊ 1;
	}

	buflen = 1000;
	buf = mosquitto_malloc((माप_प्रकार)buflen);
	यदि(!buf){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		ख_बंद(fptr);
		फर्काऊ MOSQ_ERR_NOMEM;
	}

	rc = config__read_file_core(config, reload, cr, level, lineno, fptr, &buf, &buflen);
	mosquitto_FREE(buf);
	ख_बंद(fptr);

	फर्काऊ rc;
}


स्थिर पूर्णाङ्क config__check_proxy(संरचना mosquitto__config *config)
{
	लागि(पूर्णाङ्क i=0; i<config->listener_गन्ती; i++){
		संरचना mosquitto__listener *l = &config->listeners[i];

		यदि(l->enable_proxy_protocol == 2){
#ifdef WITH_TLS
			यदि(l->use_subject_as_username){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: use_subject_as_username cannot be used with `enable_proxy_protocol 2`.");
				फर्काऊ MOSQ_ERR_INVAL;
			}

			यदि(l->certfile || l->keyfile){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: certfile and keyfile cannot be used with `enable_proxy_protocol 2`.");
				फर्काऊ MOSQ_ERR_INVAL;
			}
#endif
		}
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क config__check(संरचना mosquitto__config *config)
{
	/* Checks that are easy to make after the config has been loaded. */

	अचर वर्ण *id_prefix;
	पूर्णाङ्क id_prefix_len;
	यदि(config->security_options.auto_id_prefix){
		id_prefix = config->security_options.auto_id_prefix;
		id_prefix_len = config->security_options.auto_id_prefix_len;
	}अन्यथा{
		id_prefix = "auto-";
		id_prefix_len = (पूर्णाङ्क)म_माप("auto-");
	}

	/* Default to auto_id_prefix = 'auto-' if none set. */
	लागि(पूर्णाङ्क i=0; i<config->listener_गन्ती; i++){
		यदि(!config->listeners[i].security_options->auto_id_prefix){
			config->listeners[i].security_options->auto_id_prefix = mosquitto_strdup(id_prefix);
			यदि(!config->listeners[i].security_options->auto_id_prefix){
				फर्काऊ MOSQ_ERR_NOMEM;
			}
			config->listeners[i].security_options->auto_id_prefix_len = (uint16_t)id_prefix_len;
		}
	}

	फर्काऊ config__check_proxy(config);
}

#ifdef WITH_BRIDGE


स्थिर पूर्णाङ्क config__check_bridges(संरचना mosquitto__config *config)
{
	संरचना mosquitto__bridge *bridge1, *bridge2;
	वर्ण hostname[256];
	माप_प्रकार len;

	/* Check for bridge duplicate local_clientid, need to generate missing IDs
	 * first. */
	लागि(पूर्णाङ्क i=0; i<config->bridge_गन्ती; i++){
		bridge1 = config->bridges[i];

		यदि(!bridge1->remote_clientid){
			यदि(!gethostname(hostname, 256)){
				len = म_माप(hostname) + म_माप(bridge1->name) + 2;
				bridge1->remote_clientid = mosquitto_malloc(len);
				यदि(!bridge1->remote_clientid){
					फर्काऊ MOSQ_ERR_NOMEM;
				}
				snprintf(bridge1->remote_clientid, len, "%s.%s", hostname, bridge1->name);
			}अन्यथा{
				फर्काऊ 1;
			}
		}

		यदि(!bridge1->local_clientid){
			len = म_माप(bridge1->remote_clientid) + म_माप("local.") + 2;
			bridge1->local_clientid = mosquitto_malloc(len);
			यदि(!bridge1->local_clientid){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
				फर्काऊ MOSQ_ERR_NOMEM;
			}
			snprintf(bridge1->local_clientid, len, "local.%s", bridge1->remote_clientid);
		}
	}

	लागि(पूर्णाङ्क i=0; i<config->bridge_गन्ती; i++){
		bridge1 = config->bridges[i];
		लागि(पूर्णाङ्क j=i+1; j<config->bridge_गन्ती; j++){
			bridge2 = config->bridges[j];
			यदि(!म_भेद(bridge1->local_clientid, bridge2->local_clientid)){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Bridge local_clientid "
						"'%s' is not unique. Try changing or setting the "
						"local_clientid value for one of the bridges.",
						bridge1->local_clientid);
				फर्काऊ MOSQ_ERR_INVAL;
			}
		}
	}

#ifdef WITH_TLS
	/* Check for missing TLS cafile/capath/certfile/keyfile */
	लागि(पूर्णाङ्क i=0; i<config->listener_गन्ती; i++){
		bool cafile = !!config->listeners[i].cafile;
		bool capath = !!config->listeners[i].capath;
		bool certfile = !!config->listeners[i].certfile;
		bool keyfile = !!config->listeners[i].keyfile;

		यदि((certfile && !keyfile) || (!certfile && keyfile)){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Both certfile and keyfile must be provided to enable a TLS listener.");
			फर्काऊ MOSQ_ERR_INVAL;
		}
		यदि(cafile && !certfile){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: cafile specified without certfile and keyfile.");
			फर्काऊ MOSQ_ERR_INVAL;
		}
		यदि(capath && !certfile){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: capath specified without certfile and keyfile.");
			फर्काऊ MOSQ_ERR_INVAL;
		}
	}
#endif
	फर्काऊ MOSQ_ERR_SUCCESS;
}
#endif


स्थिर पूर्णाङ्क conf__parse_bool(वर्ण **token, अचर वर्ण *name, bool *मान, वर्ण **saveptr)
{
	*token = strtok_r(रिक्त, " ", saveptr);
	यदि(*token){
		यदि(!म_भेद(*token, "false") || !म_भेद(*token, "0")){
			*मान = false;
		}अन्यथा यदि(!म_भेद(*token, "true") || !म_भेद(*token, "1")){
			*मान = true;
		}अन्यथा{
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid '%s' value (%s).", name, *token);
			फर्काऊ MOSQ_ERR_INVAL;
		}
	}अन्यथा{
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		फर्काऊ MOSQ_ERR_INVAL;
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क conf__parse_int(वर्ण **token, अचर वर्ण *name, पूर्णाङ्क *मान, वर्ण **saveptr)
{
	*token = strtok_r(रिक्त, " ", saveptr);
	यदि(*token){
		वर्ण *endptr = रिक्त;
		दीर्घ l = म_से_दीर्घ(*token, &endptr, 0);
		यदि(endptr == *token || endptr[0] != '\0'){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: '%s' value not a number.", name);
			फर्काऊ MOSQ_ERR_INVAL;
		}
		यदि(l > पूर्णांक_उच्च || l < पूर्णांक_न्यून){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: '%s' value out of range.", name);
			फर्काऊ MOSQ_ERR_INVAL;
		}
		*मान = (पूर्णाङ्क)l;
	}अन्यथा{
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		फर्काऊ MOSQ_ERR_INVAL;
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क conf__parse_ssize_t(वर्ण **token, अचर वर्ण *name, ssize_t *मान, वर्ण **saveptr)
{
	*token = strtok_r(रिक्त, " ", saveptr);
	यदि(*token){
		*मान = म_से_द(*token);
	}अन्यथा{
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		फर्काऊ MOSQ_ERR_INVAL;
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क conf__parse_string(वर्ण **token, अचर वर्ण *name, वर्ण **मान, वर्ण **saveptr)
{
	माप_प्रकार tlen;

	*token = strtok_r(रिक्त, "", saveptr);
	यदि(*token){
		यदि(*मान){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Duplicate '%s' value in configuration.", name);
			फर्काऊ MOSQ_ERR_INVAL;
		}
		/* Deal with multiple spaces at the beginning of the string. */
		*token = mosquitto_trimblanks(*token);
		यदि(म_माप(*token) == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
			फर्काऊ MOSQ_ERR_INVAL;
		}

		tlen = म_माप(*token);
		यदि(tlen > UINT16_MAX){
			फर्काऊ MOSQ_ERR_INVAL;
		}
		यदि(mosquitto_validate_utf8(*token, (uint16_t)tlen)){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Malformed UTF-8 in configuration.");
			फर्काऊ MOSQ_ERR_INVAL;
		}
		*मान = mosquitto_strdup(*token);
		यदि(!*मान){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
			फर्काऊ MOSQ_ERR_NOMEM;
		}
	}अन्यथा{
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		फर्काऊ MOSQ_ERR_INVAL;
	}
	फर्काऊ MOSQ_ERR_SUCCESS;
}
