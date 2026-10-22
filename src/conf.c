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

#समावेश "config.h"

#समावेश <limits.h>
#समावेश <stdio.h>
#समावेश <stdlib.h>
#समावेश <string.h>
#समावेश <errno.h>

#अगर_घोषित WIN32
#अन्यथा
#  include <dirent.h>
#  include <strings.h>
#पूर्ण_अगर

#अगर_अघोषित WIN32
#  include <netdb.h>
#  include <sys/socket.h>
#अन्यथा
#  include <winsock2.h>
#  include <ws2tcpip.h>
#पूर्ण_अगर

#अगर !defined(WIN32) && !defined(__CYGWIN__)
#  include <syslog.h>
#पूर्ण_अगर

#समावेश "mosquitto_broker_internal.h"
#समावेश "tls_mosq.h"
#समावेश "util_mosq.h"
#समावेश "mosquitto/mqtt_protocol.h"

#समावेश "utlist.h"

काष्ठा config_recurse {
	अचिन्हित पूर्णांक log_dest;
	पूर्णांक log_dest_set;
	अचिन्हित पूर्णांक log_type;
	पूर्णांक log_type_set;
};

#अगर defined(WIN32) || defined(__CYGWIN__)
#समावेश <windows.h>
बाह्य SERVICE_STATUS_HANDLE service_handle;
#पूर्ण_अगर


#घोषणा REQUIRE_LISTENER(A) \
		करो{ \
			अगर(cur_listener == शून्य){ \
				log__printf(शून्य, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				वापस MOSQ_ERR_INVAL; \
			} \
		}जबतक(0)

#घोषणा REQUIRE_LISTENER_OR_DEFAULT_LISTENER(A) \
		करो{ \
			अगर(cur_listener == शून्य){ \
				अगर(config__create_default_listener(config, (A))){ \
					वापस MOSQ_ERR_NOMEM; \
				} \
				cur_listener = config->default_listener; \
			} \
		}जबतक(0)

#घोषणा REQUIRE_LISTENER_IF_PER_LISTENER(A) \
		करो{ \
			अगर(config->per_listener_settings == true && cur_listener == शून्य){ \
				log__printf(शून्य, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				वापस MOSQ_ERR_INVAL; \
			} \
		}जबतक(0)

#घोषणा REQUIRE_NON_DEFAULT_LISTENER(A) \
		करो{ \
			अगर(cur_listener == config->default_listener || cur_listener == शून्य){ \
				log__printf(शून्य, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				वापस MOSQ_ERR_INVAL; \
			} \
		}जबतक(0)

#घोषणा REQUIRE_BRIDGE(A) \
		करो{ \
			अगर(cur_bridge == शून्य){ \
				log__printf(शून्य, MOSQ_LOG_ERR, "Error: The '%s' option requires a bridge to be defined first.", (A)); \
				वापस MOSQ_ERR_INVAL; \
			} \
		}जबतक(0)

#घोषणा REQUIRE_PLUGIN(A) \
		करो{ \
			अगर(cur_plugin == शून्य){ \
				log__printf(शून्य, MOSQ_LOG_ERR, "Error: The '%s' option requires plugin/global_plugin/plugin_load to be defined first.", (A)); \
				वापस MOSQ_ERR_INVAL; \
			} \
		}जबतक(0)

#घोषणा OPTION_DEPRECATED(A, B) \
		log__printf(शून्य, MOSQ_LOG_NOTICE, "The '%s' option is now deprecated and will be removed in version 3.0. %s", (A), (B))

#घोषणा OPTION_UNAVAILABLE(A) \
		log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: The '%s' option is no longer available.", (A));

#घोषणा REQUIRE_NON_EMPTY_OPTION(A, B) \
		करो{ \
			अगर(!(A)){ \
				log__printf(शून्य, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", (B)); \
				वापस MOSQ_ERR_INVAL; \
			} \
		}जबतक(0)

#घोषणा PER_LISTENER_ALTERNATIVE(A, B) \
		अगर(config->per_listener_settings){ \
			log__printf(शून्य, MOSQ_LOG_NOTICE, "You are using the '%s' option with 'per_listener_settings true'. Please replace this with '%s'.", A, B); \
		} \


#अगर_घोषित FINAL_WITH_TLS_PSK
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A) \
		करो{ \
			अगर(cur_bridge->tls_psk_identity || cur_bridge->tls_psk){ \
				log__printf(शून्य, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and psk encryption in a single bridge.", (A)); \
				वापस MOSQ_ERR_INVAL; \
			} \
		}जबतक(0)

#  define REQUIRE_BRIDGE_NO_X509(A) \
		करो{ \
			अगर(cur_bridge->tls_cafile || cur_bridge->tls_capath || cur_bridge->tls_certfile || cur_bridge->tls_keyfile){ \
				log__printf(शून्य, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and identity encryption in a single bridge.", (A)); \
				वापस MOSQ_ERR_INVAL; \
			} \
		}जबतक(0)

#अन्यथा
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A)
#  define REQUIRE_BRIDGE_NO_X509(A)
#पूर्ण_अगर

अटल काष्ठा mosquitto__security_options *cur_security_options = शून्य;

अटल पूर्णांक conf__parse_bool(अक्षर **token, स्थिर अक्षर *name, bool *मूल्य, अक्षर **saveptr);
अटल पूर्णांक conf__parse_int(अक्षर **token, स्थिर अक्षर *name, पूर्णांक *मूल्य, अक्षर **saveptr);
अटल पूर्णांक conf__parse_ssize_t(अक्षर **token, स्थिर अक्षर *name, ssize_t *मूल्य, अक्षर **saveptr);
अटल पूर्णांक conf__parse_string(अक्षर **token, स्थिर अक्षर *name, अक्षर **मूल्य, अक्षर **saveptr);
अटल पूर्णांक config__read_file(काष्ठा mosquitto__config *config, bool reload, स्थिर अक्षर *file, काष्ठा config_recurse *config_tmp, पूर्णांक level, पूर्णांक *lineno);
अटल पूर्णांक config__check(काष्ठा mosquitto__config *config);
अटल व्योम config__cleanup_plugins(व्योम);
#अगर_घोषित WITH_BRIDGE
अटल पूर्णांक config__check_bridges(काष्ठा mosquitto__config *config);
#पूर्ण_अगर


अटल पूर्णांक config__add_listener(काष्ठा mosquitto__config *config)
{
	काष्ठा mosquitto__listener *listener;
	काष्ठा mosquitto__listener *new_listeners;
	पूर्णांक def_listener = -1;

	अगर(config->default_listener){
		क्रम(पूर्णांक i=0; i<config->listener_गिनती; i++){
			अगर(&config->listeners[i] == config->default_listener){
				def_listener = i;
				अवरोध;
			}
		}
	}
	new_listeners = mosquitto_realloc(config->listeners, माप(काष्ठा mosquitto__listener)*(माप_प्रकार)(config->listener_गिनती+1));
	अगर(!new_listeners){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
		वापस MOSQ_ERR_NOMEM;
	}
	config->listeners = new_listeners;
	listener = &config->listeners[config->listener_गिनती];
	स_रखो(listener, 0, माप(काष्ठा mosquitto__listener));
	listener->security_options = mosquitto_calloc(1, माप(काष्ठा mosquitto__security_options));
	अगर(!listener->security_options){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
		वापस MOSQ_ERR_NOMEM;
	}
	अगर(def_listener != -1){
		config->default_listener = &config->listeners[def_listener];
	}
	config->listener_गिनती++;

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक config__create_default_listener(काष्ठा mosquitto__config *config, स्थिर अक्षर *option_name)
{
	अगर(config->default_listener){
		वापस MOSQ_ERR_SUCCESS;
	}
	log__printf(शून्य, MOSQ_LOG_INFO, "Creating default listener due to '%s' option.", option_name);
	log__printf(शून्य, MOSQ_LOG_INFO, "It is best practice to define a 'listener' first. Using the '%s' option without a listener will be disabled in the future.", option_name);
	अगर(config__add_listener(config)){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
		वापस MOSQ_ERR_NOMEM;
	}

	config->default_listener = &config->listeners[config->listener_गिनती-1];
	listener__set_defaults(config->default_listener);
	config->default_listener->port = 1883;

	वापस MOSQ_ERR_SUCCESS;
}


अटल व्योम conf__set_cur_security_options(काष्ठा mosquitto__config *config, काष्ठा mosquitto__listener **cur_listener, काष्ठा mosquitto__security_options **security_options, स्थिर अक्षर *option_name)
{
	अगर(config->per_listener_settings){
		अगर(*cur_listener == शून्य){
			अगर(config__create_default_listener(config, option_name)){
				वापस;
			}
			*cur_listener = config->default_listener;
		}
		(*security_options) = (*cur_listener)->security_options;
	}अन्यथा{
		(*security_options) = &config->security_options;
	}
}


अटल पूर्णांक conf__attempt_resolve(स्थिर अक्षर *host, स्थिर अक्षर *text, अचिन्हित पूर्णांक log, स्थिर अक्षर *संदेश)
{
	काष्ठा addrinfo gai_hints;
	काष्ठा addrinfo *gai_res;
	पूर्णांक rc;

	स_रखो(&gai_hints, 0, माप(काष्ठा addrinfo));
	gai_hints.ai_family = AF_UNSPEC;
	gai_hints.ai_socktype = SOCK_STREAM;
	gai_res = शून्य;
	rc = getaddrinfo(host, शून्य, &gai_hints, &gai_res);
	अगर(gai_res){
		freeaddrinfo(gai_res);
	}
	अगर(rc != 0){
#अगर_अघोषित WIN32
		अगर(rc == EAI_SYSTEM){
			अगर(त्रुटि_सं == ENOENT){
				log__printf(शून्य, log, "%s: Unable to resolve %s %s.", संदेश, text, host);
			}अन्यथा{
				log__printf(शून्य, log, "%s: Error resolving %s: %s.", संदेश, text, म_त्रुटि(त्रुटि_सं));
			}
		}अन्यथा{
			log__printf(शून्य, log, "%s: Error resolving %s: %s.", संदेश, text, gai_strerror(rc));
		}
#अन्यथा
		अगर(rc == WSAHOST_NOT_FOUND){
			log__printf(शून्य, log, "%s: Error resolving %s.", संदेश, text);
		}
#पूर्ण_अगर
		वापस MOSQ_ERR_INVAL;
	}
	वापस MOSQ_ERR_SUCCESS;
}


अटल व्योम config__init_reload(काष्ठा mosquitto__config *config)
{
	/* Set defaults */
	क्रम(पूर्णांक i=0; i<config->listener_गिनती; i++){
		listener__set_defaults(&config->listeners[i]);
	}

	config->local_only = true;
	config->allow_duplicate_messages = true;

	mosquitto_FREE(config->security_options.acl_तथ्य.acl_file);
	mosquitto_FREE(config->security_options.password_तथ्य.password_file);
	mosquitto_FREE(config->security_options.psk_file);

	config->security_options.allow_anonymous = -1;
	config->security_options.allow_zero_लंबाई_clientid = true;
	config->security_options.auto_id_prefix = शून्य;
	config->security_options.auto_id_prefix_len = 0;

	config->autosave_interval = 1800;
	config->autosave_on_changes = false;

	mosquitto_FREE(config->clientid_prefixes);

	config->connection_messages = true;
	config->clientid_prefixes = शून्य;
	config->per_listener_settings = false;
	अगर(config->log_fptr){
		ख_बंद(config->log_fptr);
		config->log_fptr = शून्य;
	}
	mosquitto_FREE(config->log_file);

#अगर defined(WIN32) || defined(__CYGWIN__)
	अगर(service_handle){
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
	अगर(db.quiet){
		config->log_type = 0;
	}अन्यथा अगर(db.verbose){
		config->log_type = अच_पूर्णांक_उच्च;
	}अन्यथा{
		config->log_type = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;
	}
#पूर्ण_अगर
	config->log_timestamp = true;
	mosquitto_FREE(config->log_timestamp_format);
	config->global_max_clients = -1;
	config->global_max_connections = -1;
	config->log_timestamp_format = शून्य;
	config->max_keepalive = 0;
	config->max_packet_आकार = 2000000;
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
	config->packet_बफर_आकार = 4096;

	config->packet_max_auth = 100000;
	config->packet_max_connect = 100000;
	config->packet_max_sub = 100000;
	config->packet_max_simple = 10000;
}


अटल व्योम config__cleanup_plugin_config(mosquitto_plugin_id_t *plugin)
{
	mosquitto_FREE(plugin->config.path);
	mosquitto_FREE(plugin->config.name);

	अगर(plugin->config.options){
		क्रम(पूर्णांक j=0; j<plugin->config.option_गिनती; j++){
			mosquitto_FREE(plugin->config.options[j].कुंजी);
			mosquitto_FREE(plugin->config.options[j].मूल्य);
		}
		mosquitto_FREE(plugin->config.options);
		plugin->config.option_गिनती = 0;
	}
	mosquitto_FREE(plugin->config.security_options);
	mosquitto_FREE(plugin);
}


अटल व्योम config__cleanup_plugins(व्योम)
{
	क्रम(पूर्णांक i=0; i<db.plugin_गिनती; i++){
		config__cleanup_plugin_config(db.plugins[i]);
	}
	mosquitto_FREE(db.plugins);
}


व्योम config__init(काष्ठा mosquitto__config *config)
{
	स_रखो(config, 0, माप(काष्ठा mosquitto__config));
	config__init_reload(config);

	config->daemon = false;
}


व्योम config__cleanup(काष्ठा mosquitto__config *config)
{
	mosquitto_FREE(config->clientid_prefixes);
	mosquitto_FREE(config->persistence_location);
	mosquitto_FREE(config->persistence_file);
	mosquitto_FREE(config->persistence_filepath);
	mosquitto_FREE(config->security_options.auto_id_prefix);
	mosquitto_FREE(config->security_options.acl_तथ्य.acl_file);
	mosquitto_FREE(config->security_options.password_तथ्य.password_file);
	mosquitto_FREE(config->security_options.psk_file);
	mosquitto_FREE(config->security_options.plugins);
	mosquitto_FREE(config->pid_file);
	mosquitto_FREE(config->user);
	mosquitto_FREE(config->log_timestamp_format);
	अगर(config->listeners){
		क्रम(पूर्णांक i=0; i<config->listener_गिनती; i++){
			mosquitto_FREE(config->listeners[i].host);
			mosquitto_FREE(config->listeners[i].bind_interface);
			mosquitto_FREE(config->listeners[i].mount_point);
			mosquitto_FREE(config->listeners[i].socks);
			अगर(config->listeners[i].security_options){
				mosquitto_FREE(config->listeners[i].security_options->auto_id_prefix);
				mosquitto_FREE(config->listeners[i].security_options->acl_तथ्य.acl_file);
				mosquitto_FREE(config->listeners[i].security_options->password_तथ्य.password_file);
				mosquitto_FREE(config->listeners[i].security_options->psk_file);
				mosquitto_FREE(config->listeners[i].security_options->plugins);
				mosquitto_FREE(config->listeners[i].security_options);
			}
#अगर_घोषित WITH_TLS
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
#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			अगर(!config->listeners[i].ws_context) /* libwebsockets frees its own SSL_CTX */
#पूर्ण_अगर
			{
				SSL_CTX_free(config->listeners[i].ssl_ctx);
				config->listeners[i].ssl_ctx = शून्य;
			}
#पूर्ण_अगर
#अगर defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
			mosquitto_FREE(config->listeners[i].http_dir);
#पूर्ण_अगर
#अगर_घोषित WITH_WEBSOCKETS
			क्रम(पूर्णांक j=0; j<config->listeners[i].ws_origin_गिनती; j++){
				mosquitto_FREE(config->listeners[i].ws_origins[j]);
			}
			mosquitto_FREE(config->listeners[i].ws_origins);
#पूर्ण_अगर
#अगर_घोषित WITH_UNIX_SOCKETS
			mosquitto_FREE(config->listeners[i].unix_socket_path);
#पूर्ण_अगर
		}
		mosquitto_FREE(config->listeners);
	}
#अगर_घोषित WITH_BRIDGE
	अगर(config->bridges){
		क्रम(पूर्णांक i=0; i<config->bridge_गिनती; i++){
			config__bridge_cleanup(config->bridges[i]);
		}
		mosquitto_FREE(config->bridges);
	}
#पूर्ण_अगर
	config__cleanup_plugins();

	अगर(config->log_fptr){
		ख_बंद(config->log_fptr);
		config->log_fptr = शून्य;
	}
	अगर(config->log_file){
		mosquitto_FREE(config->log_file);
		config->log_file = शून्य;
	}
}

#अगर_घोषित WITH_BRIDGE


व्योम config__bridge_cleanup(काष्ठा mosquitto__bridge *bridge)
{
	अगर(bridge == शून्य){
		वापस;
	}

	mosquitto_FREE(bridge->name);
	अगर(bridge->addresses){
		क्रम(पूर्णांक i=0; i<bridge->address_गिनती; i++){
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
	अगर(bridge->topics){
		काष्ठा mosquitto__bridge_topic *cur_topic, *topic_tmp;

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
#अगर_घोषित WITH_TLS
	mosquitto_FREE(bridge->tls_certfile);
	mosquitto_FREE(bridge->tls_keyfile);
	mosquitto_FREE(bridge->tls_version);
	mosquitto_FREE(bridge->tls_cafile);
	mosquitto_FREE(bridge->tls_capath);
	mosquitto_FREE(bridge->tls_alpn);
	mosquitto_FREE(bridge->tls_ciphers);
	mosquitto_FREE(bridge->tls_13_ciphers);
#अगर_घोषित FINAL_WITH_TLS_PSK
	mosquitto_FREE(bridge->tls_psk_identity);
	mosquitto_FREE(bridge->tls_psk);
#पूर्ण_अगर
#पूर्ण_अगर
	mosquitto_FREE(bridge);
}
#पूर्ण_अगर


अटल व्योम print_version(व्योम)
{
	म_लिखो("mosquitto %s\n", VERSION);
	म_लिखो("Copyright © 2025 Roger Light.\n");
	म_लिखो("License EPL-2.0 OR BSD-3-Clause.\n");
}


अटल व्योम print_usage(व्योम)
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


पूर्णांक config__parse_args(काष्ठा mosquitto__config *config, पूर्णांक argc, अक्षर *argv[])
{
	पूर्णांक i;
	पूर्णांक port_tmp;

	क्रम(i=1; i<argc; i++){
		अगर(!म_भेद(argv[i], "-c") || !म_भेद(argv[i], "--config-file")){
			अगर(i<argc-1){
				db.config_file = argv[i+1];

				अगर(config__read(config, false)){
					वापस MOSQ_ERR_INVAL;
				}
			}अन्यथा{
				log__printf(शून्य, MOSQ_LOG_ERR, "Error: -c argument given, but no config file specified.");
				वापस MOSQ_ERR_INVAL;
			}
			i++;
		}अन्यथा अगर(!म_भेद(argv[i], "-d") || !म_भेद(argv[i], "--daemon")){
			config->daemon = true;
		}अन्यथा अगर(!म_भेद(argv[i], "-h") || !म_भेद(argv[i], "--help")){
			print_usage();
			वापस MOSQ_ERR_UNKNOWN;
		}अन्यथा अगर(!म_भेद(argv[i], "--version")){
			print_version();
			वापस MOSQ_ERR_UNKNOWN;
		}अन्यथा अगर(!म_भेद(argv[i], "-p") || !म_भेद(argv[i], "--port")){
			अगर(i<argc-1){
				port_tmp = म_से_प(argv[i+1]);
				अगर(port_tmp<1 || port_tmp>UINT16_MAX){
					log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid port specified (%d).", port_tmp);
					वापस MOSQ_ERR_INVAL;
				}अन्यथा{
					अगर(config->cmd_port_गिनती == CMD_PORT_LIMIT){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Only %d ports can be specified on the command line.", CMD_PORT_LIMIT);
						वापस MOSQ_ERR_INVAL;
					}
					config->cmd_port[config->cmd_port_गिनती] = (uint16_t)port_tmp;
					config->cmd_port_गिनती++;
				}
			}अन्यथा{
				log__printf(शून्य, MOSQ_LOG_ERR, "Error: -p argument given, but no port specified.");
				वापस MOSQ_ERR_INVAL;
			}
			i++;
		}अन्यथा अगर(!म_भेद(argv[i], "--tls-keylog")){
#अगर_घोषित WITH_TLS
			अगर(i<argc-1){
				db.tls_keylog = mosquitto_strdup(argv[i+1]);
				अगर(db.tls_keylog == शून्य){
					log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
					वापस MOSQ_ERR_NOMEM;
				}
			}अन्यथा{
				log__printf(शून्य, MOSQ_LOG_ERR, "Error: --tls-keylog argument given, but no file specified.");
				वापस MOSQ_ERR_INVAL;
			}
			i++;
#अन्यथा
			ख_लिखो(मानक_त्रुटि, "Error: TLS support not available so --tls-keylog is not available.\n");
			वापस MOSQ_ERR_INVAL;
#पूर्ण_अगर
		}अन्यथा अगर(!म_भेद(argv[i], "-q") || !म_भेद(argv[i], "--quiet")){
			db.quiet = true;
		}अन्यथा अगर(!म_भेद(argv[i], "-v") || !म_भेद(argv[i], "--verbose")){
			db.verbose = true;
		}अन्यथा अगर(!म_भेद(argv[i], "--test-config")){
			config->test_configuration = true;
		}अन्यथा{
			ख_लिखो(मानक_त्रुटि, "Error: Unknown option '%s'.\n", argv[i]);
			print_usage();
			वापस MOSQ_ERR_INVAL;
		}
	}

	/* Default to drop to mosquitto user if we are privileged and no user specified. */
	अगर(!config->user){
		config->user = mosquitto_strdup("mosquitto");
		अगर(config->user == शून्य){
			वापस MOSQ_ERR_NOMEM;
		}
	}
	अगर(db.quiet){
		config->log_type = 0;
	}अन्यथा अगर(db.verbose){
		config->log_type = अच_पूर्णांक_उच्च;
	}

	अगर(दो_पर्या("MOSQUITTO_PERSISTENCE_LOCATION")){
		mosquitto_FREE(config->persistence_location);
		config->persistence_location = mosquitto_strdup(दो_पर्या("MOSQUITTO_PERSISTENCE_LOCATION"));
		अगर(!config->persistence_location){
			वापस MOSQ_ERR_NOMEM;
		}
	}
	वापस config__check(config);
}


अटल व्योम config__copy(काष्ठा mosquitto__config *src, काष्ठा mosquitto__config *dest)
{
	mosquitto_FREE(dest->security_options.acl_तथ्य.acl_file);
	dest->security_options.acl_तथ्य.acl_file = src->security_options.acl_तथ्य.acl_file;

	acl_file__cleanup(&dest->security_options.acl_तथ्य);
	dest->security_options.acl_तथ्य.acl_users = src->security_options.acl_तथ्य.acl_users;
	dest->security_options.acl_तथ्य.acl_patterns = src->security_options.acl_तथ्य.acl_patterns;
	dest->security_options.acl_तथ्य.acl_anon.username = src->security_options.acl_तथ्य.acl_anon.username;
	dest->security_options.acl_तथ्य.acl_anon.acl = src->security_options.acl_तथ्य.acl_anon.acl;

	dest->security_options.allow_anonymous = src->security_options.allow_anonymous;
	dest->security_options.allow_zero_लंबाई_clientid = src->security_options.allow_zero_लंबाई_clientid;

	mosquitto_FREE(dest->security_options.auto_id_prefix);
	dest->security_options.auto_id_prefix = src->security_options.auto_id_prefix;
	dest->security_options.auto_id_prefix_len = src->security_options.auto_id_prefix_len;

	mosquitto_FREE(dest->security_options.password_तथ्य.password_file);
	dest->security_options.password_तथ्य.password_file = src->security_options.password_तथ्य.password_file;

	password_file__cleanup(&dest->security_options.password_तथ्य);
	dest->security_options.password_तथ्य.unpwd = src->security_options.password_तथ्य.unpwd;

	mosquitto_FREE(dest->security_options.psk_file);
	dest->security_options.psk_file = src->security_options.psk_file;

	mosquitto_FREE(dest->security_options.plugins);
	dest->security_options.plugin_गिनती = src->security_options.plugin_गिनती;
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

	dest->message_आकार_limit = src->message_आकार_limit;

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

#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	dest->websockets_log_level = src->websockets_log_level;
#पूर्ण_अगर

#अगर_घोषित WITH_BRIDGE
	क्रम(पूर्णांक i=0; i<dest->bridge_गिनती; i++){
		अगर(dest->bridges[i]){
			config__bridge_cleanup(dest->bridges[i]);
		}
	}
	mosquitto_FREE(dest->bridges);
	dest->bridges = src->bridges;
	dest->bridge_गिनती = src->bridge_गिनती;
#पूर्ण_अगर
}


पूर्णांक config__read(काष्ठा mosquitto__config *config, bool reload)
{
	पूर्णांक rc = MOSQ_ERR_SUCCESS;
	काष्ठा config_recurse cr;
	पूर्णांक lineno = 0;
#अगर_घोषित WITH_PERSISTENCE
	माप_प्रकार len;
#पूर्ण_अगर
	काष्ठा mosquitto__config config_reload;
	पूर्णांक i;

	अगर(reload){
		स_रखो(&config_reload, 0, माप(काष्ठा mosquitto__config));
	}

	cr.log_dest = MQTT3_LOG_NONE;
	cr.log_dest_set = 0;
	cr.log_type = MOSQ_LOG_NONE;
	cr.log_type_set = 0;

	अगर(!db.config_file){
		वापस 0;
	}

	अगर(reload){
		/* Re-initialise appropriate config vars to default for reload. */
		config__init_reload(&config_reload);
		config_reload.listeners = config->listeners;
		config_reload.listener_गिनती = config->listener_गिनती;
		cur_security_options = शून्य;
		rc = config__read_file(&config_reload, reload, db.config_file, &cr, 0, &lineno);
	}अन्यथा{
		rc = config__read_file(config, reload, db.config_file, &cr, 0, &lineno);
	}
	अगर(rc){
		अगर(lineno > 0){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error found at %s:%d.", db.config_file, lineno);
		}
		वापस rc;
	}

	अगर(reload){
		config__copy(&config_reload, config);
	}

	/* If auth/access options are set and allow_anonymous not explicitly set, disallow anon. */
	अगर(config->local_only == false){
		अगर(config->per_listener_settings){
			क्रम(i=0; i<config->listener_गिनती; i++){
				/* Default option if no security options set */
				अगर(config->listeners[i].security_options->allow_anonymous == -1){
					config->listeners[i].security_options->allow_anonymous = false;
				}
			}
		}अन्यथा{
			अगर(config->security_options.allow_anonymous == -1){
				config->security_options.allow_anonymous = false;
			}
		}
	}
#अगर_घोषित WITH_PERSISTENCE
	अगर(config->persistence){
		अगर(!config->persistence_file){
			config->persistence_file = mosquitto_strdup("mosquitto.db");
			अगर(!config->persistence_file){
				वापस MOSQ_ERR_NOMEM;
			}
		}
		mosquitto_FREE(config->persistence_filepath);
		अगर(config->persistence_location && म_माप(config->persistence_location)){
			len = म_माप(config->persistence_location) + म_माप(config->persistence_file) + 2;
			config->persistence_filepath = mosquitto_malloc(len);
			अगर(!config->persistence_filepath){
				वापस MOSQ_ERR_NOMEM;
			}
#अगर_घोषित WIN32
			snprintf(config->persistence_filepath, len, "%s\\%s", config->persistence_location, config->persistence_file);
#अन्यथा
			snprintf(config->persistence_filepath, len, "%s/%s", config->persistence_location, config->persistence_file);
#पूर्ण_अगर
		}अन्यथा{
			config->persistence_filepath = mosquitto_strdup(config->persistence_file);
			अगर(!config->persistence_filepath){
				वापस MOSQ_ERR_NOMEM;
			}
		}
	}
#पूर्ण_अगर
	/* Default to drop to mosquitto user if no other user specified. This must
	 * remain here even though it is covered in config__parse_args() because this
	 * function may be called on its own. */
	अगर(!config->user){
		config->user = mosquitto_strdup("mosquitto");
	}

#अगर_घोषित WITH_BRIDGE
	क्रम(i=0; i<config->bridge_गिनती; i++){
		अगर(!config->bridges[i]->name){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: bridge name not defined.");
			वापस MOSQ_ERR_INVAL;
		}
		अगर(config->bridges[i]->addresses  == 0){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no remote addresses defined.");
			वापस MOSQ_ERR_INVAL;
		}
		अगर(config->bridges[i]->topic_गिनती == 0){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no topics defined.");
			वापस MOSQ_ERR_INVAL;
		}
#अगर_घोषित FINAL_WITH_TLS_PSK
		अगर(config->bridges[i]->tls_psk && !config->bridges[i]->tls_psk_identity){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_identity.");
			वापस MOSQ_ERR_INVAL;
		}
		अगर(config->bridges[i]->tls_psk_identity && !config->bridges[i]->tls_psk){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_psk.");
			वापस MOSQ_ERR_INVAL;
		}
#पूर्ण_अगर
	}
#पूर्ण_अगर

	अगर(cr.log_dest_set){
		config->log_dest = cr.log_dest;
	}
	अगर(db.quiet){
		config->log_type = 0;
	}अन्यथा अगर(db.verbose){
		config->log_type = अच_पूर्णांक_उच्च;
	}अन्यथा अगर(cr.log_type_set){
		config->log_type = cr.log_type;
	}
#अगर_घोषित WITH_BRIDGE
	वापस config__check_bridges(config);
#अन्यथा
	वापस MOSQ_ERR_SUCCESS;
#पूर्ण_अगर
}


अटल mosquitto_plugin_id_t *config__plugin_find(स्थिर अक्षर *name)
{
	अगर(db.plugins && name){
		क्रम(पूर्णांक i=0; i<db.plugin_गिनती; i++){
			अगर(db.plugins[i]->config.name && !म_भेद(db.plugins[i]->config.name, name)){
				वापस db.plugins[i];
			}
		}
	}
	वापस शून्य;
}


अटल mosquitto_plugin_id_t *config__plugin_load(स्थिर अक्षर *name, स्थिर अक्षर *path)
{
	mosquitto_plugin_id_t *plugin = शून्य;
	mosquitto_plugin_id_t **plugins = शून्य;

	अगर(name && config__plugin_find(name)){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Duplicate plugin name '%s'.", name);
		वापस शून्य;
	}

	अगर(!path || !म_भेद(path, "")){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Missing plugin path for plugin name '%s'.", name);
		वापस शून्य;
	}

	plugin = mosquitto_calloc(1, माप(mosquitto_plugin_id_t));
	अगर(!plugin){
		जाओ error;
	}

	अगर(name){
		plugin->config.name = mosquitto_strdup(name);
	}
	plugin->config.path = mosquitto_strdup(path);
	अगर((name && !plugin->config.name) || !plugin->config.path){
		जाओ error;
	}
	plugin->config.options = शून्य;
	plugin->config.option_गिनती = 0;
	plugin->config.deny_special_chars = true;

	/* Add to db list */
	plugins = mosquitto_realloc(db.plugins, (माप_प्रकार)(db.plugin_गिनती+1)*माप(काष्ठा mosquitto__plugin_config *));
	अगर(!plugins){
		जाओ error;
	}

	plugins[db.plugin_गिनती] = plugin;
	db.plugins = plugins;
	db.plugin_गिनती++;

	वापस plugin;
error:
	log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
	अगर(plugin){
		mosquitto_FREE(plugin->config.name);
		mosquitto_FREE(plugin->config.path);
	}
	mosquitto_FREE(plugin);
	वापस शून्य;
}


पूर्णांक config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, काष्ठा mosquitto__security_options *security_options)
{
	काष्ठा mosquitto__security_options **new_options;
	mosquitto_plugin_id_t **new_plugins;

	new_options = mosquitto_realloc(plugin->config.security_options, (माप_प्रकार)(plugin->config.security_option_गिनती+1)*माप(काष्ठा mosquitto__security_options *));
	new_plugins = mosquitto_realloc(security_options->plugins, (माप_प्रकार)(security_options->plugin_गिनती+1)*माप(mosquitto_plugin_id_t *));

	अगर(!new_options || !new_plugins){
		mosquitto_FREE(new_options);
		mosquitto_FREE(new_plugins);
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
		वापस MOSQ_ERR_NOMEM;
	}

	new_options[plugin->config.security_option_गिनती] = security_options;
	plugin->config.security_options = new_options;
	plugin->config.security_option_गिनती++;

	new_plugins[security_options->plugin_गिनती] = plugin;
	security_options->plugins = new_plugins;
	security_options->plugin_गिनती++;

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक config__read_file_core(काष्ठा mosquitto__config *config, bool reload, काष्ठा config_recurse *cr, पूर्णांक level, पूर्णांक *lineno, खाता *fptr, अक्षर **buf, पूर्णांक *buflen)
{
	पूर्णांक rc;
	अक्षर *token;
	पूर्णांक tmp_int;
	अक्षर *saveptr = शून्य;
#अगर_घोषित WITH_BRIDGE
	अक्षर *tmp_char;
	काष्ठा mosquitto__bridge *cur_bridge = शून्य;
#पूर्ण_अगर
	mosquitto_plugin_id_t *cur_plugin = शून्य;

	अक्षर *कुंजी;
	काष्ठा mosquitto__listener *cur_listener = शून्य;
	पूर्णांक i;
	पूर्णांक lineno_ext = 0;
	माप_प्रकार prefix_len;
	माप_प्रकार slen;
#अगर_घोषित WITH_WEBSOCKETS
	अक्षर **ws_origins = शून्य;
#पूर्ण_अगर

	*lineno = 0;

	जबतक(mosquitto_fgets(buf, buflen, fptr)){
		(*lineno)++;
		अगर((*buf)[0] != '#' && (*buf)[0] != 10 && (*buf)[0] != 13){
			slen = म_माप(*buf);
			अगर(slen == 0){
				जारी;
			}
			जबतक((*buf)[slen-1] == 10 || (*buf)[slen-1] == 13){
				(*buf)[slen-1] = 0;
				slen = म_माप(*buf);
				अगर(slen == 0){
					जारी;
				}
			}
			token = strtok_r((*buf), " ", &saveptr);
			अगर(token){
				अगर(!म_भेद(token, "accept_protocol_versions")){
					REQUIRE_NON_DEFAULT_LISTENER(token);
					cur_listener->disable_protocol_v3 = true;
					cur_listener->disable_protocol_v4 = true;
					cur_listener->disable_protocol_v5 = true;
					अगर(saveptr == शून्य){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", "accept_protocol_versions");
						वापस MOSQ_ERR_INVAL;
					}
					token = strtok_r(saveptr, ", \t", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "accept_protocol_versions");

					जबतक(token){
						अगर(!म_भेद(token, "3")){
							cur_listener->disable_protocol_v3 = false;
						}अन्यथा अगर(!म_भेद(token, "4")){
							cur_listener->disable_protocol_v4 = false;
						}अन्यथा अगर(!म_भेद(token, "5")){
							cur_listener->disable_protocol_v5 = false;
						}

						token = strtok_r(शून्य, ", \t", &saveptr);
					}
				}अन्यथा अगर(!म_भेद(token, "acl_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);

					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->acl_तथ्य.acl_file);
					अगर(conf__parse_string(&token, "acl_file", &cur_security_options->acl_तथ्य.acl_file, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "address") || !म_भेद(token, "addresses")){
#अगर_घोषित WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					अगर(cur_bridge->addresses){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid bridge configuration, 'address' only allowed once.");
						वापस MOSQ_ERR_INVAL;
					}
					जबतक((token = strtok_r(शून्य, " ", &saveptr))){
						अगर(token[0] == '#'){
							अवरोध;
						}
						काष्ठा bridge_address *new_addresses = mosquitto_realloc(cur_bridge->addresses, माप(काष्ठा bridge_address)*(माप_प्रकार)(cur_bridge->address_गिनती+1));
						अगर(!new_addresses){
							log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
							वापस MOSQ_ERR_NOMEM;
						}
						cur_bridge->address_गिनती++;
						cur_bridge->addresses = new_addresses;
						स_रखो(&cur_bridge->addresses[cur_bridge->address_गिनती-1], 0, माप(काष्ठा bridge_address));
						cur_bridge->addresses[cur_bridge->address_गिनती-1].address = mosquitto_strdup(token);
						अगर(!cur_bridge->addresses[cur_bridge->address_गिनती-1].address){
							log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
							वापस MOSQ_ERR_NOMEM;
						}
					}
					क्रम(i=0; i<cur_bridge->address_गिनती; i++){
						/* cur_bridge->addresses[i].address is now
						 * "address[:port]". If address is an IPv6 address,
						 * then port is required. We must check for the :
						 * backwards. */
						tmp_char = म_खोजप(cur_bridge->addresses[i].address, ':');
						अगर(tmp_char){
							/* Remove ':', so cur_bridge->addresses[i].address
							 * now just looks like the address. */
							tmp_char[0] = '\0';

							/* The remainder of the string */
							tmp_int = म_से_प(&tmp_char[1]);
							अगर(tmp_int < 1 || tmp_int > UINT16_MAX){
								log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid bridge port value (%d).", tmp_int);
								वापस MOSQ_ERR_INVAL;
							}
							cur_bridge->addresses[i].port = (uint16_t)tmp_int;
						}अन्यथा{
							cur_bridge->addresses[i].port = 1883;
						}
						conf__attempt_resolve(cur_bridge->addresses[i].address, "bridge address", MOSQ_LOG_WARNING, "Warning");
					}
					अगर(cur_bridge->address_गिनती == 0){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Empty address value in configuration.");
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "allow_anonymous")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					PER_LISTENER_ALTERNATIVE(token, "listener_allow_anonymous");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					अगर(conf__parse_bool(&token, "allow_anonymous", (bool *)&cur_security_options->allow_anonymous, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "allow_duplicate_messages")){
					OPTION_DEPRECATED(token, "The behaviour will default to true.");
					अगर(conf__parse_bool(&token, "allow_duplicate_messages", &config->allow_duplicate_messages, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "allow_zero_length_clientid")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					अगर(conf__parse_bool(&token, "allow_zero_length_clientid", &cur_security_options->allow_zero_लंबाई_clientid, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेदन(token, "auth_opt_", म_माप("auth_opt_")) || !म_भेदन(token, "plugin_opt_", म_माप("plugin_opt_"))){
					अगर(reload){
						जारी;        /* Auth plugin not currently valid for reloading. */

					}
					REQUIRE_PLUGIN(token);

					अगर(!म_भेदन(token, "auth_opt_", म_माप("auth_opt_"))){
						prefix_len = म_माप("auth_opt_");
					}अन्यथा{
						prefix_len = म_माप("plugin_opt_");
					}
					अगर(म_माप(token) < prefix_len + 3){
						/* auth_opt_ == 9, + one digit key == 10, + one space == 11, + one value == 12 */
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid 'plugin_opt_' config option.");
						वापस MOSQ_ERR_INVAL;
					}
					कुंजी = mosquitto_strdup(&token[prefix_len]);
					अगर(!कुंजी){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
						वापस MOSQ_ERR_NOMEM;
					}अन्यथा अगर(STREMPTY(कुंजी)){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Empty 'plugin_opt_' config option.");
						mosquitto_FREE(कुंजी);
						वापस MOSQ_ERR_INVAL;
					}
					token = saveptr;
					अगर(token && token[0]){
						जबतक(token[0] == ' ' || token[0] == '\t'){
							token++;
						}
						काष्ठा mosquitto_opt *new_options = mosquitto_realloc(cur_plugin->config.options, (माप_प्रकार)(cur_plugin->config.option_गिनती+1)*माप(काष्ठा mosquitto_opt));
						अगर(!new_options){
							log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
							mosquitto_FREE(कुंजी);
							वापस MOSQ_ERR_NOMEM;
						}
						cur_plugin->config.option_गिनती++;
						cur_plugin->config.options = new_options;
						cur_plugin->config.options[cur_plugin->config.option_गिनती-1].कुंजी = कुंजी;
						cur_plugin->config.options[cur_plugin->config.option_गिनती-1].मूल्य = mosquitto_strdup(token);
						अगर(!cur_plugin->config.options[cur_plugin->config.option_गिनती-1].मूल्य){
							log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
							वापस MOSQ_ERR_NOMEM;
						}
					}अन्यथा{
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", कुंजी);
						mosquitto_FREE(कुंजी);
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "auth_plugin") || !म_भेद(token, "plugin") || !म_भेद(token, "global_plugin")){
					अगर(reload){
						जारी;        /* plugin not currently valid for reloading. */
					}
					अगर(!म_भेद(token, "global_plugin")){
						cur_security_options = &db.config->security_options;
					}अन्यथा{
						REQUIRE_LISTENER_IF_PER_LISTENER(token);
						conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					}

					cur_plugin = config__plugin_load(शून्य, saveptr);
					अगर(cur_plugin == शून्य){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(config__plugin_add_secopt(cur_plugin, cur_security_options)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "auth_plugin_deny_special_chars")){
					अगर(reload){
						जारी;        /* Auth plugin not currently valid for reloading. */
					}
					अगर(!cur_plugin){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: An auth_plugin_deny_special_chars option exists in the config file without a plugin.");
						वापस MOSQ_ERR_INVAL;
					}
					अगर(conf__parse_bool(&token, "auth_plugin_deny_special_chars", &cur_plugin->config.deny_special_chars, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "plugin_load")){
					अक्षर *name = शून्य;

					अगर(reload){
						जारी;        /* plugin not currently valid for reloading. */

					}
					name = strtok_r(शून्य, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(name, "plugin_load");

					cur_plugin = config__plugin_load(name, saveptr);
					अगर(cur_plugin == शून्य){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "plugin_use")){
					अक्षर *name = शून्य;

					अगर(reload){
						जारी;        /* plugin not currently valid for reloading. */
					}
					REQUIRE_NON_DEFAULT_LISTENER(token);

					अगर(conf__parse_string(&token, "plugin_use", &name, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					cur_plugin = config__plugin_find(name);
					अगर(!cur_plugin){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Plugin '%s' not previously loaded.", name);
						mosquitto_FREE(name);
						वापस MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(name);
					अगर(config__plugin_add_secopt(cur_plugin, cur_listener->security_options)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "auto_id_prefix")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					OPTION_DEPRECATED(token, "Please use 'listener_auto_id_prefix' instead.");
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					अगर(conf__parse_string(&token, "auto_id_prefix", &cur_security_options->auto_id_prefix, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(cur_security_options->auto_id_prefix){
						अगर(म_माप(cur_security_options->auto_id_prefix) > 50){
							log__printf(शून्य, MOSQ_LOG_ERR, "Error: auto_id_prefix length must be <= 50.");
							वापस MOSQ_ERR_INVAL;
						}
						cur_security_options->auto_id_prefix_len = (uint16_t)म_माप(cur_security_options->auto_id_prefix);
					}अन्यथा{
						cur_security_options->auto_id_prefix_len = 0;
					}
				}अन्यथा अगर(!म_भेद(token, "autosave_interval")){
					अगर(conf__parse_int(&token, "autosave_interval", &config->autosave_interval, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(config->autosave_interval < 0){
						config->autosave_interval = 0;
					}
				}अन्यथा अगर(!म_भेद(token, "autosave_on_changes")){
					अगर(conf__parse_bool(&token, "autosave_on_changes", &config->autosave_on_changes, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "bind_address")){
					OPTION_DEPRECATED(token, "");
					config->local_only = false;
					अगर(reload){
						जारी;        /* Rebinding listeners not valid during reloading. */

					}
					अगर(config__create_default_listener(config, token)){
						वापस MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					अगर(conf__parse_string(&token, "default listener bind_address", &config->default_listener->host, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(conf__attempt_resolve(config->default_listener->host, "bind_address", MOSQ_LOG_ERR, "Error")){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "bind_interface")){
#अगर_घोषित SO_BINDTODEVICE
					अगर(reload){
						जारी;        /* Rebinding listeners not valid during reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					अगर(conf__parse_string(&token, "bind_interface", &cur_listener->bind_interface, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_ERR, "Error: bind_interface specified but socket option not available.");
					वापस MOSQ_ERR_INVAL;
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "bridge_attempt_unsubscribe")){
#अगर_घोषित WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_bool(&token, "bridge_attempt_unsubscribe", &cur_bridge->attempt_unsubscribe, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "bridge_cafile")){
#अगर defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					अगर(conf__parse_string(&token, "bridge_cafile", &cur_bridge->tls_cafile, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "bridge_alpn")){
#अगर defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_string(&token, "bridge_alpn", &cur_bridge->tls_alpn, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "bridge_ciphers")){
#अगर defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_string(&token, "bridge_ciphers", &cur_bridge->tls_ciphers, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "bridge_ciphers_tls1.3")){
#अगर defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_string(&token, "bridge_ciphers_tls1.3", &cur_bridge->tls_13_ciphers, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "bridge_bind_address")){
#अगर defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_string(&token, "bridge_bind_address", &cur_bridge->bind_address, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "bridge_capath")){
#अगर defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					अगर(conf__parse_string(&token, "bridge_capath", &cur_bridge->tls_capath, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "bridge_certfile")){
#अगर defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					अगर(conf__parse_string(&token, "bridge_certfile", &cur_bridge->tls_certfile, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "bridge_identity")){
#अगर defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					अगर(conf__parse_string(&token, "bridge_identity", &cur_bridge->tls_psk_identity, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "bridge_insecure")){
#अगर defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_bool(&token, "bridge_insecure", &cur_bridge->tls_insecure, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(cur_bridge->tls_insecure){
						log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge '%s' using insecure mode.", cur_bridge->name);
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "bridge_require_ocsp")){
#अगर defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_bool(&token, "bridge_require_ocsp", &cur_bridge->tls_ocsp_required, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "bridge_max_packet_size")){
#अगर defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_int(&token, "bridge_max_packet_size", &tmp_int, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(tmp_int < 0){
						tmp_int = 0;
					}
					cur_bridge->maximum_packet_आकार = (uint32_t)tmp_int;
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "bridge_max_topic_alias")){
#अगर_घोषित WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_int(&token, "bridge_max_topic_alias", &tmp_int, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}

					अगर(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: bridge_max_topic_alias must be > 0 and <= 65535.");
						वापस MOSQ_ERR_INVAL;
					}
					cur_bridge->max_topic_alias = (uint16_t)tmp_int;
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "bridge_outgoing_retain")){
#अगर defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_bool(&token, "bridge_outgoing_retain", &cur_bridge->outgoing_retain, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "bridge_keyfile")){
#अगर defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					mosquitto_FREE(cur_bridge->tls_keyfile);
					अगर(conf__parse_string(&token, "bridge_keyfile", &cur_bridge->tls_keyfile, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "bridge_protocol_version")){
#अगर_घोषित WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(शून्य, "", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_protocol_version");

					अगर(!म_भेद(token, "mqttv31")){
						cur_bridge->protocol_version = mosq_p_mqtt31;
					}अन्यथा अगर(!म_भेद(token, "mqttv311")){
						cur_bridge->protocol_version = mosq_p_mqtt311;
					}अन्यथा अगर(!म_भेद(token, "mqttv50")){
						cur_bridge->protocol_version = mosq_p_mqtt5;
					}अन्यथा{
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid 'bridge_protocol_version' value (%s).", token);
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "bridge_psk")){
#अगर defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					अगर(conf__parse_string(&token, "bridge_psk", &cur_bridge->tls_psk, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "bridge_receive_maximum")){
#अगर defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_int(&token, "bridge_receive_maximum", &tmp_int, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(tmp_int <= 0){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be greater than 0.");
						वापस MOSQ_ERR_INVAL;
					}अन्यथा अगर((uint64_t)tmp_int > (uint64_t)UINT16_MAX){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be lower than %u.", UINT16_MAX);
						वापस MOSQ_ERR_INVAL;
					}
					cur_bridge->receive_maximum = (uint16_t)tmp_int;
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "bridge_reload_type")){
#अगर_घोषित WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(शून्य, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "bridge_reload_type");

					अगर(!म_भेद(token, "lazy")){
						cur_bridge->reload_type = brt_lazy;
					}अन्यथा अगर(!म_भेद(token, "immediate")){
						cur_bridge->reload_type = brt_immediate;
					}अन्यथा{
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid 'bridge_reload_type' value in configuration (%s).", token);
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "bridge_session_expiry_interval")){
#अगर defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_int(&token, "bridge_session_expiry_interval", &tmp_int, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(tmp_int < 0){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must not be negative.");
						वापस MOSQ_ERR_INVAL;
					}अन्यथा अगर((uint64_t)tmp_int > (uint64_t)MQTT_SESSION_EXPIRY_NEVER){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must be lower than %u.", MQTT_SESSION_EXPIRY_NEVER);
						वापस MOSQ_ERR_INVAL;
					}
					cur_bridge->session_expiry_interval = (uint32_t)tmp_int;
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "bridge_tcp_keepalive")){
#अगर_घोषित WITH_BRIDGE
					REQUIRE_BRIDGE(token);

					अगर(conf__parse_int(&token, "bridge_tcp_keepalive_idle", &tmp_int, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(tmp_int <= 0){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: invalid TCP keepalive idle value.");
						वापस MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_idle = (अचिन्हित पूर्णांक)tmp_int;

					अगर(conf__parse_int(&token, "bridge_tcp_keepalive_interval", &tmp_int, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(tmp_int <= 0){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: invalid TCP keepalive interval value.");
						वापस MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_interval = (अचिन्हित पूर्णांक)tmp_int;

					अगर(conf__parse_int(&token, "bridge_tcp_keepalive_counter", &tmp_int, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(tmp_int <= 0){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: invalid TCP keepalive counter value.");
						वापस MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_गणक = (अचिन्हित पूर्णांक)tmp_int;
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "bridge_tcp_user_timeout")){
#अगर_घोषित WITH_BRIDGE
					REQUIRE_BRIDGE(token);
#अगर_घोषित WITH_TCP_USER_TIMEOUT
					अगर(conf__parse_int(&token, "bridge_tcp_user_timeout", &tmp_int, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(tmp_int < 0){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: invalid TCP user timeout value.");
						वापस MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_user_timeout = tmp_int;
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge TCP user timeout support not available.");
#पूर्ण_अगर
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "bridge_tls_use_os_certs")){
#अगर defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_bool(&token, "bridge_tls_use_os_certs", &cur_bridge->tls_use_os_certs, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "bridge_tls_version")){
#अगर defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_string(&token, "bridge_tls_version", &cur_bridge->tls_version, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "cafile")){
#अगर defined(WITH_TLS)
					REQUIRE_LISTENER(token);
					अगर(cur_listener->psk_hint){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						वापस MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->cafile);
					अगर(conf__parse_string(&token, "cafile", &cur_listener->cafile, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "capath")){
#अगर_घोषित WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->capath);
					अगर(conf__parse_string(&token, "capath", &cur_listener->capath, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "certfile")){
#अगर_घोषित WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					अगर(cur_listener->psk_hint){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						वापस MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->certfile);
					अगर(conf__parse_string(&token, "certfile", &cur_listener->certfile, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "check_retain_source")){
					अगर(conf__parse_bool(&token, "check_retain_source", &config->check_retain_source, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "ciphers")){
#अगर_घोषित WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers);
					अगर(conf__parse_string(&token, "ciphers", &cur_listener->ciphers, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "ciphers_tls1.3")){
#अगर defined(WITH_TLS) && (!defined(LIBRESSL_VERSION_संख्या) || LIBRESSL_VERSION_संख्या > 0x3040000FL)
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers_tls13);
					अगर(conf__parse_string(&token, "ciphers_tls1.3", &cur_listener->ciphers_tls13, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: ciphers_tls1.3 support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "clientid") || !म_भेद(token, "remote_clientid")){
#अगर_घोषित WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_string(&token, "bridge remote clientid", &cur_bridge->remote_clientid, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "cleansession")){
#अगर_घोषित WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_bool(&token, "cleansession", &cur_bridge->clean_start, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "local_cleansession")){
#अगर_घोषित WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_bool(&token, "local_cleansession", (bool *)&cur_bridge->clean_start_local, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "clientid_prefixes")){
					OPTION_DEPRECATED(token, "");
					mosquitto_FREE(config->clientid_prefixes);
					अगर(conf__parse_string(&token, "clientid_prefixes", &config->clientid_prefixes, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "connection")){
#अगर_घोषित WITH_BRIDGE
					token = strtok_r(शून्य, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "connection");

					/* Check for existing bridge name. */
					क्रम(i=0; i<config->bridge_गिनती; i++){
						अगर(!म_भेद(config->bridges[i]->name, token)){
							log__printf(शून्य, MOSQ_LOG_ERR, "Error: Duplicate bridge name '%s'.", token);
							वापस MOSQ_ERR_INVAL;
						}
					}

					काष्ठा mosquitto__bridge **bridges_new = mosquitto_realloc(config->bridges, (माप_प्रकार)(config->bridge_गिनती+1)*माप(काष्ठा mosquitto__bridge *));
					अगर(!bridges_new){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
						वापस MOSQ_ERR_NOMEM;
					}
					config->bridges = bridges_new;
					cur_bridge = mosquitto_malloc(माप(काष्ठा mosquitto__bridge));
					अगर(!cur_bridge){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
						वापस MOSQ_ERR_NOMEM;
					}

					config->bridge_गिनती++;
					config->bridges[config->bridge_गिनती-1] = cur_bridge;

					स_रखो(cur_bridge, 0, माप(काष्ठा mosquitto__bridge));
					cur_bridge->name = mosquitto_strdup(token);
					अगर(!cur_bridge->name){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
						वापस MOSQ_ERR_NOMEM;
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
#अगर_घोषित WITH_TCP_USER_TIMEOUT
					cur_bridge->tcp_user_timeout = -1;
#पूर्ण_अगर
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "connection_messages")){
					अगर(conf__parse_bool(&token, token, &config->connection_messages, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "crlfile")){
#अगर_घोषित WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->crlfile);
					अगर(conf__parse_string(&token, "crlfile", &cur_listener->crlfile, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "dhparamfile")){
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: dhparamfile is no longer required.");
				}अन्यथा अगर(!म_भेद(token, "disable_client_cert_date_checks")){
#अगर_घोषित WITH_TLS
					REQUIRE_LISTENER(token);
					अगर(conf__parse_bool(&token, "disable_client_cert_date_checks", &cur_listener->disable_client_cert_date_checks, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "enable_control_api")){
#अगर_घोषित WITH_CONTROL
					अगर(conf__parse_bool(&token, "enable_control_api", &config->enable_control_api, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: $CONTROL support not available (enable_control_api).");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "enable_proxy_protocol")){
#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(शून्य, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					वापस MOSQ_ERR_INVAL;
#पूर्ण_अगर
					REQUIRE_LISTENER(token);
					अगर(conf__parse_int(&token, "enable_proxy_protocol", &cur_listener->enable_proxy_protocol, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(cur_listener->enable_proxy_protocol < 1 || cur_listener->enable_proxy_protocol > 2){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: enable_proxy_protocol must be 1 or 2.");
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "global_max_clients")){
					अगर(conf__parse_int(&token, "global_max_clients", &config->global_max_clients, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "global_max_connections")){
					अगर(conf__parse_int(&token, "global_max_connections", &config->global_max_connections, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "http_dir")){
#अगर defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
					अगर(reload){
						जारी;        /* Not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					अगर(conf__parse_string(&token, "http_dir", &cur_listener->http_dir, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अगर_घोषित WIN32
					अक्षर *http_dir_canonical = _fullpath(शून्य, cur_listener->http_dir, 0);
					स्थिर अक्षर sep = '\\';
#अन्यथा
					अक्षर *http_dir_canonical = realpath(cur_listener->http_dir, शून्य);
					स्थिर अक्षर sep = '/';
#पूर्ण_अगर
					अगर(!http_dir_canonical){
						वापस MOSQ_ERR_NOMEM;
					}
					माप_प्रकार http_dir_len = म_माप(http_dir_canonical) + माप(sep) + 1;
					अक्षर *http_dir_canonical_sep = mosquitto_calloc(http_dir_len, माप(अक्षर));
					अगर(!http_dir_canonical_sep){
						मुक्त(http_dir_canonical);
						वापस MOSQ_ERR_NOMEM;
					}
					snprintf(http_dir_canonical_sep, http_dir_len, "%s%c", http_dir_canonical, sep);
					मुक्त(http_dir_canonical);
					mosquitto_FREE(cur_listener->http_dir);
					cur_listener->http_dir = http_dir_canonical_sep;
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: http_dir support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "idle_timeout")){
#अगर_घोषित WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_int(&token, "idle_timeout", &cur_bridge->idle_timeout, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(cur_bridge->idle_timeout < 1){
						log__printf(शून्य, MOSQ_LOG_NOTICE, "idle_timeout interval too low, using 1 second.");
						cur_bridge->idle_timeout = 1;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "include_dir")){
					अगर(level == 0){
						अक्षर **files;
						पूर्णांक file_गिनती;

						/* Only process include_dir from the main config file. */
						token = strtok_r(शून्य, "", &saveptr);
						REQUIRE_NON_EMPTY_OPTION(token, "include_dir");

						rc = config__get_dir_files(token, &files, &file_गिनती);
						अगर(rc){
							वापस rc;
						}

						क्रम(i=0; i<file_गिनती; i++){
							log__printf(शून्य, MOSQ_LOG_INFO, "Loading config file '%s'", files[i]);

							rc = config__read_file(config, reload, files[i], cr, level+1, &lineno_ext);
							अगर(rc){
								अगर(lineno_ext > 0){
									log__printf(शून्य, MOSQ_LOG_ERR, "Error found at '%s:%d'.", files[i], lineno_ext);
								}
								/* Free happens below */
								अवरोध;
							}
						}
						क्रम(i=0; i<file_गिनती; i++){
							mosquitto_FREE(files[i]);
						}
						mosquitto_FREE(files);
						अगर(rc){
							वापस rc;    /* This returns if config__read_file() fails above */
						}
					}अन्यथा{
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: The include_dir option is only valid in the main configuration file.");
						वापस 1;
					}
				}अन्यथा अगर(!म_भेद(token, "keepalive_interval")){
#अगर_घोषित WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_int(&token, "keepalive_interval", &tmp_int, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(tmp_int > UINT16_MAX){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Bridge keepalive value too high.");
						वापस MOSQ_ERR_INVAL;
					}
					अगर(tmp_int < 5){
						log__printf(शून्य, MOSQ_LOG_NOTICE, "keepalive interval too low, using 5 seconds.");
						tmp_int = 5;
					}
					cur_bridge->keepalive = (uint16_t)tmp_int;
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "keyfile")){
#अगर_घोषित WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->keyfile);
					अगर(conf__parse_string(&token, "keyfile", &cur_listener->keyfile, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "listener")){
					config->local_only = false;

					अगर(conf__parse_int(&token, "listener port", &tmp_int, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अगर_घोषित WITH_UNIX_SOCKETS
					अगर(tmp_int < 0 || tmp_int > UINT16_MAX){
#अन्यथा
					अगर(tmp_int < 1 || tmp_int > UINT16_MAX){
#पूर्ण_अगर
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						वापस MOSQ_ERR_INVAL;
					}

					/* Look for bind address / unix socket path */
					token = strtok_r(शून्य, " ", &saveptr);
					अगर(token != शून्य && token[0] == '#'){
						token = शून्य;
					}

					अगर(tmp_int == 0 && token == शून्य){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: A listener with port 0 must provide a Unix socket path.");
						वापस MOSQ_ERR_INVAL;
					}

					अगर(reload){
						/* We reload listeners settings based on port number/unix socket path.
						 * If the port number/unix path doesn't already exist, exit with a complaint. */
						cur_listener = शून्य;
#अगर_घोषित WITH_UNIX_SOCKETS
						अगर(tmp_int == 0){
							क्रम(i=0; i<config->listener_गिनती; i++){
								अगर(config->listeners[i].unix_socket_path != शून्य
										&& म_भेद(config->listeners[i].unix_socket_path, token) == 0){

									cur_listener = &config->listeners[i];
									अवरोध;
								}
							}
						}अन्यथा
#पूर्ण_अगर
						{
							क्रम(i=0; i<config->listener_गिनती; i++){
								अगर(config->listeners[i].port == tmp_int){
									/* Now check we have a matching bind address, if defined */
									अगर(config->listeners[i].host){
										अगर(token && !म_भेद(config->listeners[i].host, token)){
											/* They both have a bind address, and they match */
											cur_listener = &config->listeners[i];
											अवरोध;
										}
									}अन्यथा{
										अगर(token == शून्य){
											/* Neither this config nor the new config have a bind address,
											 * so they match. */
											cur_listener = &config->listeners[i];
											अवरोध;
										}
									}
								}
							}
						}
						अगर(!cur_listener){
							log__printf(शून्य, MOSQ_LOG_ERR, "Error: It is not currently possible to add/remove listeners when reloading the config file.");
							वापस MOSQ_ERR_INVAL;
						}
					}अन्यथा{
						अगर(config__add_listener(config)){
							log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
							वापस MOSQ_ERR_NOMEM;
						}
						cur_listener = &config->listeners[config->listener_गिनती-1];
					}

					listener__set_defaults(cur_listener);
					cur_listener->port = (uint16_t)tmp_int;

					mosquitto_FREE(cur_listener->host);

#अगर_घोषित WITH_UNIX_SOCKETS
					mosquitto_FREE(cur_listener->unix_socket_path);
#पूर्ण_अगर

					अगर(token){
#अगर_घोषित WITH_UNIX_SOCKETS
						अगर(cur_listener->port == 0){
							cur_listener->unix_socket_path = mosquitto_strdup(token);
						}अन्यथा
#पूर्ण_अगर
						{
							cur_listener->host = mosquitto_strdup(token);
						}
					}
				}अन्यथा अगर(!म_भेद(token, "listener_allow_anonymous")){
					REQUIRE_LISTENER(token);
					अगर(conf__parse_bool(&token, "listener_allow_anonymous", (bool *)&cur_listener->security_options->allow_anonymous, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "listener_auto_id_prefix")){
					REQUIRE_LISTENER(token);
					अगर(conf__parse_string(&token, "listener_auto_id_prefix", &cur_listener->security_options->auto_id_prefix, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(cur_listener->security_options->auto_id_prefix){
						अगर(म_माप(cur_listener->security_options->auto_id_prefix) > 50){
							log__printf(शून्य, MOSQ_LOG_ERR, "Error: 'listener_auto_id_prefix' length must be <= 50.");
							वापस MOSQ_ERR_INVAL;
						}
						cur_listener->security_options->auto_id_prefix_len = (uint16_t)म_माप(cur_listener->security_options->auto_id_prefix);
					}अन्यथा{
						cur_listener->security_options->auto_id_prefix_len = 0;
					}
				}अन्यथा अगर(!म_भेद(token, "local_clientid")){
#अगर_घोषित WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_string(&token, "bridge local clientd", &cur_bridge->local_clientid, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "local_password")){
#अगर_घोषित WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_string(&token, "bridge local_password", &cur_bridge->local_password, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "local_username")){
#अगर_घोषित WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_string(&token, "bridge local_username", &cur_bridge->local_username, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "log_dest")){
					token = strtok_r(शून्य, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_dest");

					cr->log_dest_set = 1;
					अगर(!म_भेद(token, "none")){
						cr->log_dest = MQTT3_LOG_NONE;
					}अन्यथा अगर(!म_भेद(token, "syslog")){
						cr->log_dest |= MQTT3_LOG_SYSLOG;
					}अन्यथा अगर(!म_भेद(token, "stdout")){
						cr->log_dest |= MQTT3_LOG_STDOUT;
					}अन्यथा अगर(!म_भेद(token, "stderr")){
						cr->log_dest |= MQTT3_LOG_STDERR;
					}अन्यथा अगर(!म_भेद(token, "topic")){
						cr->log_dest |= MQTT3_LOG_TOPIC;
					}अन्यथा अगर(!म_भेद(token, "dlt")){
						cr->log_dest |= MQTT3_LOG_DLT;
#अगर_घोषित ANDROID
					}अन्यथा अगर(!म_भेद(token, "android")){
						cr->log_dest |= MQTT3_LOG_ANDROID;
#पूर्ण_अगर
					}अन्यथा अगर(!म_भेद(token, "file")){
						अगर(config->log_fptr || config->log_file){
							log__printf(शून्य, MOSQ_LOG_ERR, "Error: Duplicate \"log_dest file\" value.");
							वापस MOSQ_ERR_INVAL;
						}
						/* Get remaining string. */
						token = saveptr;
						अगर(token && token[0]){
							जबतक(token[0] == ' ' || token[0] == '\t'){
								token++;
							}
						}
						/* Duplicate "token" check here saves a log__printf() */
						अगर(token && token[0]){
							config->log_file = mosquitto_strdup(token);
							अगर(!config->log_file){
								log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
								वापस MOSQ_ERR_NOMEM;
							}
							cr->log_dest |= MQTT3_LOG_FILE;
						}अन्यथा{
							log__printf(शून्य, MOSQ_LOG_ERR, "Error: Empty \"log_dest file\" value in configuration.");
							वापस MOSQ_ERR_INVAL;
						}
						cr->log_dest |= MQTT3_LOG_FILE;
					}अन्यथा{
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid 'log_dest' value (%s).", token);
						वापस MOSQ_ERR_INVAL;
					}
#अगर defined(WIN32) || defined(__CYGWIN__)
					अगर(service_handle){
						अगर(cr->log_dest == MQTT3_LOG_STDOUT || cr->log_dest == MQTT3_LOG_STDERR){
							log__printf(शून्य, MOSQ_LOG_ERR, "Error: Cannot log to stdout/stderr when running as a Windows service.");
							वापस MOSQ_ERR_INVAL;
						}
					}
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "log_facility")){
#अगर defined(WIN32) || defined(__CYGWIN__)
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: log_facility not supported on Windows.");
#अन्यथा
					अगर(conf__parse_int(&token, "log_facility", &tmp_int, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					चयन(tmp_int){
						हाल 0:
							config->log_facility = LOG_LOCAL0;
							अवरोध;
						हाल 1:
							config->log_facility = LOG_LOCAL1;
							अवरोध;
						हाल 2:
							config->log_facility = LOG_LOCAL2;
							अवरोध;
						हाल 3:
							config->log_facility = LOG_LOCAL3;
							अवरोध;
						हाल 4:
							config->log_facility = LOG_LOCAL4;
							अवरोध;
						हाल 5:
							config->log_facility = LOG_LOCAL5;
							अवरोध;
						हाल 6:
							config->log_facility = LOG_LOCAL6;
							अवरोध;
						हाल 7:
							config->log_facility = LOG_LOCAL7;
							अवरोध;
						शेष:
							log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid 'log_facility' value (%d).", tmp_int);
							वापस MOSQ_ERR_INVAL;
					}
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "log_timestamp")){
					अगर(conf__parse_bool(&token, token, &config->log_timestamp, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "log_timestamp_format")){
					अगर(conf__parse_string(&token, token, &config->log_timestamp_format, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "log_type")){
					token = strtok_r(शून्य, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "log_type");

					cr->log_type_set = 1;
					अगर(!म_भेद(token, "none")){
						cr->log_type = MOSQ_LOG_NONE;
					}अन्यथा अगर(!म_भेद(token, "information")){
						cr->log_type |= MOSQ_LOG_INFO;
					}अन्यथा अगर(!म_भेद(token, "notice")){
						cr->log_type |= MOSQ_LOG_NOTICE;
					}अन्यथा अगर(!म_भेद(token, "warning")){
						cr->log_type |= MOSQ_LOG_WARNING;
					}अन्यथा अगर(!म_भेद(token, "error")){
						cr->log_type |= MOSQ_LOG_ERR;
					}अन्यथा अगर(!म_भेद(token, "debug")){
						cr->log_type |= MOSQ_LOG_DEBUG;
					}अन्यथा अगर(!म_भेद(token, "subscribe")){
						cr->log_type |= MOSQ_LOG_SUBSCRIBE;
					}अन्यथा अगर(!म_भेद(token, "unsubscribe")){
						cr->log_type |= MOSQ_LOG_UNSUBSCRIBE;
					}अन्यथा अगर(!म_भेद(token, "internal")){
						cr->log_type |= MOSQ_LOG_INTERNAL;
#अगर_घोषित WITH_WEBSOCKETS
					}अन्यथा अगर(!म_भेद(token, "websockets")){
						cr->log_type |= MOSQ_LOG_WEBSOCKETS;
#पूर्ण_अगर
					}अन्यथा अगर(!म_भेद(token, "all")){
						cr->log_type = MOSQ_LOG_ALL;
					}अन्यथा{
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid 'log_type' value (%s).", token);
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "max_connections")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					अगर(conf__parse_int(&token, token, &cur_listener->max_connections, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(cur_listener->max_connections < 0){
						cur_listener->max_connections = -1;
					}
				}अन्यथा अगर(!म_भेद(token, "maximum_qos") || !म_भेद(token, "max_qos")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					अगर(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(tmp_int < 0 || tmp_int > 2){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: 'max_qos' must be between 0 and 2 inclusive.");
						वापस MOSQ_ERR_INVAL;
					}
					cur_listener->max_qos = (uint8_t)tmp_int;
				}अन्यथा अगर(!म_भेद(token, "max_inflight_bytes")){
					अगर(conf__parse_int(&token, "max_inflight_bytes", &tmp_int, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_inflight_bytes = (माप_प्रकार)tmp_int;
				}अन्यथा अगर(!म_भेद(token, "max_inflight_messages")){
					अगर(conf__parse_int(&token, "max_inflight_messages", &tmp_int, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(tmp_int < 0 || tmp_int == UINT16_MAX){
						tmp_int = 0;
					}अन्यथा अगर(tmp_int > UINT16_MAX){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: 'max_inflight_messages' must be <= 65535.");
						वापस MOSQ_ERR_INVAL;
					}
					config->max_inflight_messages = (uint16_t)tmp_int;
				}अन्यथा अगर(!म_भेद(token, "max_keepalive")){
					अगर(conf__parse_int(&token, "max_keepalive", &tmp_int, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid 'max_keepalive' value (%d).", tmp_int);
						वापस MOSQ_ERR_INVAL;
					}
					config->max_keepalive = (uint16_t)tmp_int;
				}अन्यथा अगर(!म_भेद(token, "max_packet_size")){
					अगर(conf__parse_int(&token, "max_packet_size", &tmp_int, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(tmp_int < 20){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: 'max_packet_size' must be >= 20.");
						वापस MOSQ_ERR_INVAL;
					}
					config->max_packet_आकार = (uint32_t)tmp_int;
				}अन्यथा अगर(!म_भेद(token, "max_queued_bytes")){
					अगर(conf__parse_int(&token, "max_queued_bytes", &tmp_int, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_bytes = (माप_प्रकार)tmp_int;
				}अन्यथा अगर(!म_भेद(token, "max_queued_messages")){
					अगर(conf__parse_int(&token, "max_queued_messages", &tmp_int, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_messages = tmp_int;
				}अन्यथा अगर(!म_भेद(token, "memory_limit")){
					ssize_t lim;
					अगर(conf__parse_ssize_t(&token, "memory_limit", &lim, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(lim < 0){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid 'memory_limit' value (%ld).", lim);
						वापस MOSQ_ERR_INVAL;
					}
					mosquitto_memory_set_limit((माप_प्रकार)lim);
				}अन्यथा अगर(!म_भेद(token, "message_size_limit")){
					log__printf(शून्य, MOSQ_LOG_NOTICE, "Note: It is recommended to replace `message_size_limit` with `max_packet_size`.");
					अगर(conf__parse_int(&token, "message_size_limit", (पूर्णांक *)&config->message_आकार_limit, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(config->message_आकार_limit > MQTT_MAX_PAYLOAD){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid 'message_size_limit' value (%u).", config->message_आकार_limit);
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "mount_point")){
					REQUIRE_LISTENER(token);
					mosquitto_FREE(cur_listener->mount_point);
					अगर(conf__parse_string(&token, "mount_point", &cur_listener->mount_point, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(mosquitto_pub_topic_check(cur_listener->mount_point) != MOSQ_ERR_SUCCESS){
						log__printf(शून्य, MOSQ_LOG_ERR,
								"Error: Invalid 'mount_point' value (%s). Does it contain a wildcard character?",
								cur_listener->mount_point);
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "notifications")){
#अगर_घोषित WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_bool(&token, "notifications", &cur_bridge->notifications, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "notifications_local_only")){
#अगर_घोषित WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_bool(&token, "notifications_local_only", &cur_bridge->notifications_local_only, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "notification_topic")){
#अगर_घोषित WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_string(&token, "notification_topic", &cur_bridge->notification_topic, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "password") || !म_भेद(token, "remote_password")){
#अगर_घोषित WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_string(&token, "bridge remote_password", &cur_bridge->remote_password, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "password_file")){
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->password_तथ्य.password_file);
					अगर(conf__parse_string(&token, "password_file", &cur_security_options->password_तथ्य.password_file, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "per_listener_settings")){
					OPTION_DEPRECATED(token, "Please see the documentation for how to achieve the same effect.");
					अगर(config->per_listener_settings){
						/* Once this is set, don't let it be unset. It should be the first config option ideally. */
						जारी;
					}
					अगर(conf__parse_bool(&token, "per_listener_settings", &config->per_listener_settings, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(cur_security_options && config->per_listener_settings){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: per_listener_settings must be set before any other security settings.");
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "persistence") || !म_भेद(token, "retained_persistence")){
					अगर(conf__parse_bool(&token, token, &config->persistence, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "persistence_file")){
					अगर(conf__parse_string(&token, "persistence_file", &config->persistence_file, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "persistence_location")){
					अगर(conf__parse_string(&token, "persistence_location", &config->persistence_location, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "persistent_client_expiration")){
					token = strtok_r(शून्य, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "persistent_client_expiration");

					समय_प्रकार expiration_mult;

					चयन(token[म_माप(token)-1]){
						हाल 's':
							expiration_mult = 1;
							अवरोध;
						हाल 'h':
							expiration_mult = 3600;
							अवरोध;
						हाल 'd':
							expiration_mult = 86400;
							अवरोध;
						हाल 'w':
							expiration_mult = 86400*7;
							अवरोध;
						हाल 'm':
							expiration_mult = 86400*30;
							अवरोध;
						हाल 'y':
							expiration_mult = 86400*365;
							अवरोध;
						शेष:
							log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
							वापस MOSQ_ERR_INVAL;
					}
					token[म_माप(token)-1] = '\0';
					config->persistent_client_expiration = म_से_प(token)*expiration_mult;
					अगर(config->persistent_client_expiration <= 0){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "pid_file")){
					अगर(reload){
						जारी;        /* pid file not valid for reloading. */
					}
					अगर(conf__parse_string(&token, "pid_file", &config->pid_file, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "port")){
					OPTION_DEPRECATED(token, "Please use 'listener' instead.");
					config->local_only = false;
					अगर(reload){
						जारी;        /* Listeners not valid for reloading. */

					}
					अगर(config__create_default_listener(config, token)){
						वापस MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					अगर(config->default_listener->port){
						log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Default listener port specified multiple times. Only the latest will be used.");
					}
					अगर(conf__parse_int(&token, "port", &tmp_int, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(tmp_int < 1 || tmp_int > UINT16_MAX){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						वापस MOSQ_ERR_INVAL;
					}
					config->default_listener->port = (uint16_t)tmp_int;
				}अन्यथा अगर(!म_भेद(token, "protocol")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(शून्य, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "protocol");

					अगर(!म_भेद(token, "mqtt")){
						cur_listener->protocol = mp_mqtt;
						/*
						}else if(!strcmp(token, "mqttsn")){
						    cur_listener->protocol = mp_mqttsn;
						*/
					}अन्यथा अगर(!म_भेद(token, "websockets")){
#अगर_घोषित WITH_WEBSOCKETS
						cur_listener->protocol = mp_websockets;
#अन्यथा
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Websockets support not available.");
						वापस MOSQ_ERR_INVAL;
#पूर्ण_अगर
					}अन्यथा अगर(!म_भेद(token, "http_api")){
#अगर_घोषित WITH_HTTP_API
						cur_listener->protocol = mp_http_api;
#अन्यथा
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: HTTP API support not available.");
						वापस MOSQ_ERR_INVAL;
#पूर्ण_अगर
					}अन्यथा{
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid 'protocol' value (%s).", token);
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "proxy_protocol_v2_require_tls")){
#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(शून्य, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					वापस MOSQ_ERR_INVAL;
#पूर्ण_अगर
					REQUIRE_LISTENER(token);
					अगर(conf__parse_bool(&token, "proxy_protocol_v2_require_tls", &cur_listener->proxy_protocol_v2_require_tls, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "psk_file")){
#अगर_घोषित FINAL_WITH_TLS_PSK
					REQUIRE_LISTENER_IF_PER_LISTENER(token);
					conf__set_cur_security_options(config, &cur_listener, &cur_security_options, token);
					अगर(cur_listener && cur_listener->certfile){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						वापस MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_security_options->psk_file);
					अगर(conf__parse_string(&token, "psk_file", &cur_security_options->psk_file, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "psk_hint")){
#अगर_घोषित FINAL_WITH_TLS_PSK
					अगर(reload){
						जारी;        /* PSK file not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					अगर(conf__parse_string(&token, "psk_hint", &cur_listener->psk_hint, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "queue_qos0_messages")){
					अगर(conf__parse_bool(&token, token, &config->queue_qos0_messages, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "require_certificate")){
#अगर_घोषित WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					अगर(conf__parse_bool(&token, "require_certificate", &cur_listener->require_certificate, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "restart_timeout")){
#अगर_घोषित WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					cur_bridge->backoff_cap = 0; /* set backoff to constant mode, unless cap is specified further down */
					token = strtok_r(शून्य, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "restart_timeout");

					cur_bridge->restart_timeout = म_से_प(token);
					cur_bridge->backoff_base = 0;
					cur_bridge->backoff_cap = 0;
					अगर(cur_bridge->restart_timeout < 1){
						log__printf(शून्य, MOSQ_LOG_NOTICE, "restart_timeout interval too low, using 1 second.");
						cur_bridge->restart_timeout = 1;
					}अन्यथा अगर(cur_bridge->restart_timeout > 3600){
						log__printf(शून्य, MOSQ_LOG_NOTICE, "restart_timeout interval too high, using 3600 seconds.");
						cur_bridge->restart_timeout = 3600;
					}
					token = strtok_r(शून्य, " ", &saveptr);
					अगर(token){
						cur_bridge->backoff_base = cur_bridge->restart_timeout;
						cur_bridge->backoff_cap = म_से_प(token);
						अगर(cur_bridge->backoff_cap < cur_bridge->backoff_base){
							log__printf(शून्य, MOSQ_LOG_ERR, "Error: backoff cap is lower than the base in restart_timeout.");
							वापस MOSQ_ERR_INVAL;
						}अन्यथा अगर(cur_bridge->backoff_cap > 7200){
							log__printf(शून्य, MOSQ_LOG_ERR, "Error: backoff cap too high, using 7200 seconds.");
							cur_bridge->backoff_cap = 7200;
						}

						token = strtok_r(शून्य, " ", &saveptr);
						अगर(token){
							cur_bridge->stable_connection_period = म_से_प(token);
							अगर(cur_bridge->stable_connection_period < 0){
								log__printf(शून्य, MOSQ_LOG_ERR, "Error: stable connection period cannot be negative.");
								वापस MOSQ_ERR_INVAL;
							}
						}
					}
					cur_bridge->restart_timeout *= 1000; /* backoff is tracked in ms */
					cur_bridge->backoff_base *= 1000;
					cur_bridge->backoff_cap *= 1000;
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "retain_available")){
					अगर(conf__parse_bool(&token, token, &config->retain_available, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "retain_expiry_interval")){
					अगर(conf__parse_int(&token, token, &config->retain_expiry_interval, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(config->retain_expiry_interval < 1){
						log__printf(शून्य, MOSQ_LOG_WARNING, "Error: retain_expiry_interval must be >= 1.");
						वापस MOSQ_ERR_INVAL;
					}अन्यथा अगर(config->retain_expiry_interval > 10000000){
						log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: retain_expiry_interval being capped at 19 years.");
						config->retain_expiry_interval = 10000000;
					}
					config->retain_expiry_interval *= 60;
				}अन्यथा अगर(!म_भेद(token, "retry_interval")){
					OPTION_UNAVAILABLE(token);
				}अन्यथा अगर(!म_भेद(token, "round_robin")){
#अगर_घोषित WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_bool(&token, "round_robin", &cur_bridge->round_robin, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "set_tcp_nodelay")){
					अगर(conf__parse_bool(&token, "set_tcp_nodelay", &config->set_tcp_nodelay, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "start_type")){
#अगर_घोषित WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(शून्य, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					अगर(!म_भेद(token, "automatic")){
						cur_bridge->start_type = bst_automatic;
					}अन्यथा अगर(!म_भेद(token, "lazy")){
						cur_bridge->start_type = bst_lazy;
					}अन्यथा अगर(!म_भेद(token, "manual")){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Manual start_type not supported.");
						वापस MOSQ_ERR_INVAL;
					}अन्यथा अगर(!म_भेद(token, "once")){
						cur_bridge->start_type = bst_once;
					}अन्यथा{
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid 'start_type' value in configuration (%s).", token);
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "socket_domain")){
					अगर(reload){
						जारी;        /* socket_domain not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(शून्य, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "start_type");

					अगर(!म_भेद(token, "ipv4")){
						cur_listener->socket_domain = AF_INET;
					}अन्यथा अगर(!म_भेद(token, "ipv6")){
						cur_listener->socket_domain = AF_INET6;
					}अन्यथा{
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid 'socket_domain' value '%s' in configuration.", token);
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "sys_interval")){
					अगर(conf__parse_int(&token, "sys_interval", &config->sys_interval, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(config->sys_interval < 0 || config->sys_interval > 65535){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid 'sys_interval' value (%d).", config->sys_interval);
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "threshold")){
#अगर_घोषित WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_int(&token, "threshold", &cur_bridge->threshold, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(cur_bridge->threshold < 1){
						log__printf(शून्य, MOSQ_LOG_NOTICE, "threshold too low, using 1 message.");
						cur_bridge->threshold = 1;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "tls_engine")){
#अगर_घोषित WITH_TLS
					अगर(reload){
						जारी;        /* tls_engine not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					अगर(conf__parse_string(&token, "tls_engine", &cur_listener->tls_engine, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "tls_engine_kpass_sha1")){
#अगर_घोषित WITH_TLS
					अगर(reload){
						जारी;        /* tls_engine not valid for reloading. */

					}
					अक्षर *kpass_sha = शून्य, *kpass_sha_bin = शून्य;
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					अगर(conf__parse_string(&token, "tls_engine_kpass_sha1", &kpass_sha, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(mosquitto__hex2bin_sha1(kpass_sha, (अचिन्हित अक्षर **)&kpass_sha_bin) != MOSQ_ERR_SUCCESS){
						mosquitto_FREE(kpass_sha);
						वापस MOSQ_ERR_INVAL;
					}
					mosquitto_free(cur_listener->tls_engine_kpass_sha1);
					cur_listener->tls_engine_kpass_sha1 = kpass_sha_bin;
					mosquitto_FREE(kpass_sha);
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "tls_keyform")){
#अगर_घोषित WITH_TLS
					अगर(reload){
						जारी;        /* tls_engine not valid for reloading. */

					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					अक्षर *keyform = शून्य;
					अगर(conf__parse_string(&token, "tls_keyform", &keyform, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					cur_listener->tls_keyform = mosq_k_pem;
					अगर(!म_भेद(keyform, "engine")){
						cur_listener->tls_keyform = mosq_k_engine;
					}
					mosquitto_FREE(keyform);
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "tls_version")){
#अगर defined(WITH_TLS)
					अगर(reload){
						जारी;        /* tls_version not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					अगर(conf__parse_string(&token, "tls_version", &cur_listener->tls_version, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "topic")){
#अगर_घोषित WITH_BRIDGE
					अक्षर *topic = शून्य;
					क्रमागत mosquitto__bridge_direction direction = bd_out;
					uint8_t qos = 0;
					अक्षर *local_prefix = शून्य, *remote_prefix = शून्य;

					REQUIRE_BRIDGE(token);

					token = strtok_r(शून्य, " ", &saveptr);
					REQUIRE_NON_EMPTY_OPTION(token, "topic");

					// Check if the topic is quoted (e.g. for spaces within topic names), but not the
					// special case of ""
					अगर(token[0] == '"' && token [1] != '"'){
						अगर(म_अक्षर(saveptr, '"') == शून्य){
							log__printf(शून्य, MOSQ_LOG_ERR, "Error: Missing closing quote in topic value (%s).", saveptr);
							वापस MOSQ_ERR_INVAL;
						}

						अक्षर *topic_in_quotes = strtok_r(शून्य, "\"", &saveptr);
						माप_प्रकार tlen = 1;
						अगर(topic_in_quotes){
							tlen = म_माप(topic_in_quotes);
						}

						topic = mosquitto_malloc(म_माप(token) + tlen + 1);
						अगर(!topic){
							log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
							वापस MOSQ_ERR_NOMEM;
						}

						म_नकल(topic, token + 1);
						म_जोड़ो(topic, " ");
						अगर(topic_in_quotes){
							म_जोड़ो(topic, topic_in_quotes);
						}
					}अन्यथा{
						topic = mosquitto_strdup(token);
						अगर(!topic){
							log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
							वापस MOSQ_ERR_NOMEM;
						}
					}

					token = strtok_r(शून्य, " ", &saveptr);
					अगर(token){
						अगर(!strcasecmp(token, "out")){
							direction = bd_out;
						}अन्यथा अगर(!strcasecmp(token, "in")){
							direction = bd_in;
						}अन्यथा अगर(!strcasecmp(token, "both")){
							direction = bd_both;
						}अन्यथा{
							log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid bridge topic direction '%s'.", token);
							mosquitto_FREE(topic);
							वापस MOSQ_ERR_INVAL;
						}
						token = strtok_r(शून्य, " ", &saveptr);
						अगर(token){
							अगर(token[0] == '#'){
								(व्योम)strtok_r(शून्य, "", &saveptr);
							}
							qos = (uint8_t)म_से_प(token);
							अगर(qos > 2){
								log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid bridge QoS level '%s'.", token);
								mosquitto_FREE(topic);
								वापस MOSQ_ERR_INVAL;
							}

							token = strtok_r(शून्य, " ", &saveptr);
							अगर(token){
								अगर(!म_भेद(token, "\"\"") || token[0] == '#'){
									local_prefix = शून्य;
									अगर(token[0] == '#'){
										(व्योम)strtok_r(शून्य, "", &saveptr);
									}
								}अन्यथा{
									local_prefix = token;
								}

								token = strtok_r(शून्य, " ", &saveptr);
								अगर(token){
									अगर(!म_भेद(token, "\"\"") || token[0] == '#'){
										remote_prefix = शून्य;
									}अन्यथा{
										remote_prefix = token;
									}
								}
							}
						}
					}

					अगर(bridge__add_topic(cur_bridge, topic, direction, qos, local_prefix, remote_prefix)){
						mosquitto_FREE(topic);
						वापस MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(topic);
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "max_topic_alias")){
					REQUIRE_LISTENER(token);
					अगर(conf__parse_int(&token, "max_topic_alias", &tmp_int, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias' value in configuration.");
						वापस MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias = (uint16_t)tmp_int;
				}अन्यथा अगर(!म_भेद(token, "max_topic_alias_broker")){
					REQUIRE_LISTENER(token);
					अगर(conf__parse_int(&token, "max_topic_alias_broker", &tmp_int, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias_broker' value in configuration.");
						वापस MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias_broker = (uint16_t)tmp_int;
				}अन्यथा अगर(!म_भेद(token, "try_private")){
#अगर_घोषित WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_bool(&token, "try_private", &cur_bridge->try_private, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "upgrade_outgoing_qos")){
					अगर(conf__parse_bool(&token, token, &config->upgrade_outgoing_qos, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "use_identity_as_username")){
#अगर_घोषित WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					अगर(conf__parse_bool(&token, "use_identity_as_username", &cur_listener->use_identity_as_username, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "use_subject_as_username")){
#अगर_घोषित WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					अगर(conf__parse_bool(&token, "use_subject_as_username", &cur_listener->use_subject_as_username, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "user")){
					अगर(reload){
						जारी;        /* Drop privileges user not valid for reloading. */
					}
					mosquitto_FREE(config->user);
					अगर(conf__parse_string(&token, "user", &config->user, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "use_username_as_clientid")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					अगर(conf__parse_bool(&token, "use_username_as_clientid", &cur_listener->use_username_as_clientid, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
				}अन्यथा अगर(!म_भेद(token, "username") || !म_भेद(token, "remote_username")){
#अगर_घोषित WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					अगर(conf__parse_string(&token, "bridge remote_username", &cur_bridge->remote_username, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "websockets_log_level")){
#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					अगर(conf__parse_int(&token, "websockets_log_level", &config->websockets_log_level, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
#पूर्ण_अगर
				}अन्यथा अगर(!म_भेद(token, "websockets_headers_size") || !म_भेद(token, "packet_buffer_size")){
					अगर(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					अगर(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(शून्य, MOSQ_LOG_WARNING, "Error: Packet buffer size must be between 0 and 65535 inclusive.");
						वापस MOSQ_ERR_INVAL;
					}
					config->packet_बफर_आकार = (uint16_t)tmp_int;
				}अन्यथा अगर(!म_भेद(token, "websockets_origin")){
#अगर_घोषित WITH_WEBSOCKETS
#  अगर LWS_LIBRARY_VERSION_संख्या >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN
					REQUIRE_LISTENER(token);
					ws_origins = mosquitto_realloc(cur_listener->ws_origins, माप(अक्षर *)*(माप_प्रकार)(cur_listener->ws_origin_गिनती+1));
					अगर(ws_origins == शून्य){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
						वापस MOSQ_ERR_NOMEM;
					}
					ws_origins[cur_listener->ws_origin_गिनती] = शून्य;
					cur_listener->ws_origins = ws_origins;
					अगर(conf__parse_string(&token, "websockets_origin", &ws_origins[cur_listener->ws_origin_गिनती], &saveptr)){
						वापस MOSQ_ERR_INVAL;
					}
					cur_listener->ws_origin_गिनती++;
#  अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: websockets_origin support not available, libwebsockets version is too old.");
#  endif
#अन्यथा
					log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Websockets support not available.");
#पूर्ण_अगर
				}अन्यथा{
					log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unknown configuration variable '%s'.", token);
					वापस MOSQ_ERR_INVAL;
				}
			}
		}
	}
	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक config__read_file(काष्ठा mosquitto__config *config, bool reload, स्थिर अक्षर *file, काष्ठा config_recurse *cr, पूर्णांक level, पूर्णांक *lineno)
{
	पूर्णांक rc;
	खाता *fptr = शून्य;
	अक्षर *buf;
	पूर्णांक buflen;
#अगर_अघोषित WIN32
	सूची *dir;
#पूर्ण_अगर

#अगर_अघोषित WIN32
	dir = सूची_खोलो(file);
	अगर(dir){
		बंद_सूची(dir);
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Config file '%s' is a directory.", file);
		वापस 1;
	}
#पूर्ण_अगर

	fptr = mosquitto_fopen(file, "rt", false);
	अगर(!fptr){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to open config file '%s'.", file);
		वापस 1;
	}

	buflen = 1000;
	buf = mosquitto_malloc((माप_प्रकार)buflen);
	अगर(!buf){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
		ख_बंद(fptr);
		वापस MOSQ_ERR_NOMEM;
	}

	rc = config__read_file_core(config, reload, cr, level, lineno, fptr, &buf, &buflen);
	mosquitto_FREE(buf);
	ख_बंद(fptr);

	वापस rc;
}


अटल पूर्णांक config__check_proxy(काष्ठा mosquitto__config *config)
{
	क्रम(पूर्णांक i=0; i<config->listener_गिनती; i++){
		काष्ठा mosquitto__listener *l = &config->listeners[i];

		अगर(l->enable_proxy_protocol == 2){
#अगर_घोषित WITH_TLS
			अगर(l->use_subject_as_username){
				log__printf(शून्य, MOSQ_LOG_ERR, "Error: use_subject_as_username cannot be used with `enable_proxy_protocol 2`.");
				वापस MOSQ_ERR_INVAL;
			}

			अगर(l->certfile || l->keyfile){
				log__printf(शून्य, MOSQ_LOG_ERR, "Error: certfile and keyfile cannot be used with `enable_proxy_protocol 2`.");
				वापस MOSQ_ERR_INVAL;
			}
#पूर्ण_अगर
		}
	}

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक config__check(काष्ठा mosquitto__config *config)
{
	/* Checks that are easy to make after the config has been loaded. */

	स्थिर अक्षर *id_prefix;
	पूर्णांक id_prefix_len;
	अगर(config->security_options.auto_id_prefix){
		id_prefix = config->security_options.auto_id_prefix;
		id_prefix_len = config->security_options.auto_id_prefix_len;
	}अन्यथा{
		id_prefix = "auto-";
		id_prefix_len = (पूर्णांक)म_माप("auto-");
	}

	/* Default to auto_id_prefix = 'auto-' if none set. */
	क्रम(पूर्णांक i=0; i<config->listener_गिनती; i++){
		अगर(!config->listeners[i].security_options->auto_id_prefix){
			config->listeners[i].security_options->auto_id_prefix = mosquitto_strdup(id_prefix);
			अगर(!config->listeners[i].security_options->auto_id_prefix){
				वापस MOSQ_ERR_NOMEM;
			}
			config->listeners[i].security_options->auto_id_prefix_len = (uint16_t)id_prefix_len;
		}
	}

	वापस config__check_proxy(config);
}

#अगर_घोषित WITH_BRIDGE


अटल पूर्णांक config__check_bridges(काष्ठा mosquitto__config *config)
{
	काष्ठा mosquitto__bridge *bridge1, *bridge2;
	अक्षर hostname[256];
	माप_प्रकार len;

	/* Check for bridge duplicate local_clientid, need to generate missing IDs
	 * first. */
	क्रम(पूर्णांक i=0; i<config->bridge_गिनती; i++){
		bridge1 = config->bridges[i];

		अगर(!bridge1->remote_clientid){
			अगर(!gethostname(hostname, 256)){
				len = म_माप(hostname) + म_माप(bridge1->name) + 2;
				bridge1->remote_clientid = mosquitto_malloc(len);
				अगर(!bridge1->remote_clientid){
					वापस MOSQ_ERR_NOMEM;
				}
				snprintf(bridge1->remote_clientid, len, "%s.%s", hostname, bridge1->name);
			}अन्यथा{
				वापस 1;
			}
		}

		अगर(!bridge1->local_clientid){
			len = म_माप(bridge1->remote_clientid) + म_माप("local.") + 2;
			bridge1->local_clientid = mosquitto_malloc(len);
			अगर(!bridge1->local_clientid){
				log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
				वापस MOSQ_ERR_NOMEM;
			}
			snprintf(bridge1->local_clientid, len, "local.%s", bridge1->remote_clientid);
		}
	}

	क्रम(पूर्णांक i=0; i<config->bridge_गिनती; i++){
		bridge1 = config->bridges[i];
		क्रम(पूर्णांक j=i+1; j<config->bridge_गिनती; j++){
			bridge2 = config->bridges[j];
			अगर(!म_भेद(bridge1->local_clientid, bridge2->local_clientid)){
				log__printf(शून्य, MOSQ_LOG_ERR, "Error: Bridge local_clientid "
						"'%s' is not unique. Try changing or setting the "
						"local_clientid value for one of the bridges.",
						bridge1->local_clientid);
				वापस MOSQ_ERR_INVAL;
			}
		}
	}

#अगर_घोषित WITH_TLS
	/* Check for missing TLS cafile/capath/certfile/keyfile */
	क्रम(पूर्णांक i=0; i<config->listener_गिनती; i++){
		bool cafile = !!config->listeners[i].cafile;
		bool capath = !!config->listeners[i].capath;
		bool certfile = !!config->listeners[i].certfile;
		bool keyfile = !!config->listeners[i].keyfile;

		अगर((certfile && !keyfile) || (!certfile && keyfile)){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Both certfile and keyfile must be provided to enable a TLS listener.");
			वापस MOSQ_ERR_INVAL;
		}
		अगर(cafile && !certfile){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: cafile specified without certfile and keyfile.");
			वापस MOSQ_ERR_INVAL;
		}
		अगर(capath && !certfile){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: capath specified without certfile and keyfile.");
			वापस MOSQ_ERR_INVAL;
		}
	}
#पूर्ण_अगर
	वापस MOSQ_ERR_SUCCESS;
}
#पूर्ण_अगर


अटल पूर्णांक conf__parse_bool(अक्षर **token, स्थिर अक्षर *name, bool *मूल्य, अक्षर **saveptr)
{
	*token = strtok_r(शून्य, " ", saveptr);
	अगर(*token){
		अगर(!म_भेद(*token, "false") || !म_भेद(*token, "0")){
			*मूल्य = false;
		}अन्यथा अगर(!म_भेद(*token, "true") || !म_भेद(*token, "1")){
			*मूल्य = true;
		}अन्यथा{
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Invalid '%s' value (%s).", name, *token);
			वापस MOSQ_ERR_INVAL;
		}
	}अन्यथा{
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		वापस MOSQ_ERR_INVAL;
	}

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक conf__parse_int(अक्षर **token, स्थिर अक्षर *name, पूर्णांक *मूल्य, अक्षर **saveptr)
{
	*token = strtok_r(शून्य, " ", saveptr);
	अगर(*token){
		अक्षर *endptr = शून्य;
		दीर्घ l = म_से_दीर्घ(*token, &endptr, 0);
		अगर(endptr == *token || endptr[0] != '\0'){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: '%s' value not a number.", name);
			वापस MOSQ_ERR_INVAL;
		}
		अगर(l > पूर्णांक_उच्च || l < पूर्णांक_न्यून){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: '%s' value out of range.", name);
			वापस MOSQ_ERR_INVAL;
		}
		*मूल्य = (पूर्णांक)l;
	}अन्यथा{
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		वापस MOSQ_ERR_INVAL;
	}

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक conf__parse_ssize_t(अक्षर **token, स्थिर अक्षर *name, ssize_t *मूल्य, अक्षर **saveptr)
{
	*token = strtok_r(शून्य, " ", saveptr);
	अगर(*token){
		*मूल्य = म_से_द(*token);
	}अन्यथा{
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		वापस MOSQ_ERR_INVAL;
	}

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक conf__parse_string(अक्षर **token, स्थिर अक्षर *name, अक्षर **मूल्य, अक्षर **saveptr)
{
	माप_प्रकार tlen;

	*token = strtok_r(शून्य, "", saveptr);
	अगर(*token){
		अगर(*मूल्य){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Duplicate '%s' value in configuration.", name);
			वापस MOSQ_ERR_INVAL;
		}
		/* Deal with multiple spaces at the beginning of the string. */
		*token = mosquitto_trimblanks(*token);
		अगर(म_माप(*token) == 0){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
			वापस MOSQ_ERR_INVAL;
		}

		tlen = म_माप(*token);
		अगर(tlen > UINT16_MAX){
			वापस MOSQ_ERR_INVAL;
		}
		अगर(mosquitto_validate_utf8(*token, (uint16_t)tlen)){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Malformed UTF-8 in configuration.");
			वापस MOSQ_ERR_INVAL;
		}
		*मूल्य = mosquitto_strdup(*token);
		अगर(!*मूल्य){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
			वापस MOSQ_ERR_NOMEM;
		}
	}अन्यथा{
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		वापस MOSQ_ERR_INVAL;
	}
	वापस MOSQ_ERR_SUCCESS;
}
