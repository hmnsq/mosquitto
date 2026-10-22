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

#اگر_تعریف_شدہ WIN32
#ورنہ
#  include <dirent.h>
#  include <strings.h>
#اگر_ختم

#اگر_غیر_تعریف_شدہ WIN32
#  include <netdb.h>
#  include <sys/socket.h>
#ورنہ
#  include <winsock2.h>
#  include <ws2tcpip.h>
#اگر_ختم

#اگر !defined(WIN32) && !defined(__CYGWIN__)
#  include <syslog.h>
#اگر_ختم

#شامل "mosquitto_broker_internal.h"
#شامل "tls_mosq.h"
#شامل "util_mosq.h"
#شامل "mosquitto/mqtt_protocol.h"

#شامل "utlist.h"

ساخت config_recurse {
	غیرعلامتی صحیح log_dest;
	صحیح log_dest_سیٹ;
	غیرعلامتی صحیح log_type;
	صحیح log_type_سیٹ;
};

#اگر defined(WIN32) || defined(__CYGWIN__)
#شامل <windows.h>
خارجی SERVICE_حالت_HANDLE service_handle;
#اگر_ختم


#تعریف_کرو REQUIRE_LISTENER(A) \
		کرو{ \
			اگر(cur_listener == تہی){ \
				log__printf(تہی, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				واپس MOSQ_ERR_INVAL; \
			} \
		}جب(0)

#تعریف_کرو REQUIRE_LISTENER_OR_DEFAULT_LISTENER(A) \
		کرو{ \
			اگر(cur_listener == تہی){ \
				اگر(config__create_default_listener(config, (A))){ \
					واپس MOSQ_ERR_NOMEM; \
				} \
				cur_listener = config->default_listener; \
			} \
		}جب(0)

#تعریف_کرو REQUIRE_LISTENER_IF_فی_LISTENER(A) \
		کرو{ \
			اگر(config->فی_listener_settings == true && cur_listener == تہی){ \
				log__printf(تہی, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				واپس MOSQ_ERR_INVAL; \
			} \
		}جب(0)

#تعریف_کرو REQUIRE_NON_DEFAULT_LISTENER(A) \
		کرو{ \
			اگر(cur_listener == config->default_listener || cur_listener == تہی){ \
				log__printf(تہی, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				واپس MOSQ_ERR_INVAL; \
			} \
		}جب(0)

#تعریف_کرو REQUIRE_BRIDGE(A) \
		کرو{ \
			اگر(cur_bridge == تہی){ \
				log__printf(تہی, MOSQ_LOG_ERR, "Error: The '%s' option requires a bridge to be defined first.", (A)); \
				واپس MOSQ_ERR_INVAL; \
			} \
		}جب(0)

#تعریف_کرو REQUIRE_PLUGIN(A) \
		کرو{ \
			اگر(cur_plugin == تہی){ \
				log__printf(تہی, MOSQ_LOG_ERR, "Error: The '%s' option requires plugin/global_plugin/plugin_load to be defined first.", (A)); \
				واپس MOSQ_ERR_INVAL; \
			} \
		}جب(0)

#تعریف_کرو OPTION_DEPRECATED(A, B) \
		log__printf(تہی, MOSQ_LOG_NOTICE, "The '%s' option is now deprecated and will be removed in version 3.0. %s", (A), (B))

#تعریف_کرو OPTION_UNAVAILABLE(A) \
		log__printf(تہی, MOSQ_LOG_WARNING, "Warning: The '%s' option is no longer available.", (A));

#تعریف_کرو REQUIRE_NON_خالی_خانے_OPTION(A, B) \
		کرو{ \
			اگر(!(A)){ \
				log__printf(تہی, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", (B)); \
				واپس MOSQ_ERR_INVAL; \
			} \
		}جب(0)

#تعریف_کرو فی_LISTENER_ALTERNATIVE(A, B) \
		اگر(config->فی_listener_settings){ \
			log__printf(تہی, MOSQ_LOG_NOTICE, "You are using the '%s' option with 'per_listener_settings true'. Please replace this with '%s'.", A, B); \
		} \


#اگر_تعریف_شدہ FINAL_WITH_TLS_PSK
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A) \
		کرو{ \
			اگر(cur_bridge->tls_psk_شناختی_عنصر || cur_bridge->tls_psk){ \
				log__printf(تہی, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and psk encryption in a single bridge.", (A)); \
				واپس MOSQ_ERR_INVAL; \
			} \
		}جب(0)

#  define REQUIRE_BRIDGE_NO_X509(A) \
		کرو{ \
			اگر(cur_bridge->tls_cafile || cur_bridge->tls_capath || cur_bridge->tls_certfile || cur_bridge->tls_keyfile){ \
				log__printf(تہی, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and identity encryption in a single bridge.", (A)); \
				واپس MOSQ_ERR_INVAL; \
			} \
		}جب(0)

#ورنہ
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A)
#  define REQUIRE_BRIDGE_NO_X509(A)
#اگر_ختم

ساکن ساخت mosquitto__security_options *cur_security_options = تہی;

ساکن صحیح conf__parse_bool(حرف **token, ثابت حرف *name, bool *قدر, حرف **saveptr);
ساکن صحیح conf__parse_int(حرف **token, ثابت حرف *name, صحیح *قدر, حرف **saveptr);
ساکن صحیح conf__parse_ssize_t(حرف **token, ثابت حرف *name, ssize_t *قدر, حرف **saveptr);
ساکن صحیح conf__parse_string(حرف **token, ثابت حرف *name, حرف **قدر, حرف **saveptr);
ساکن صحیح config__read_file(ساخت mosquitto__config *config, bool reload, ثابت حرف *file, ساخت config_recurse *config_tmp, صحیح level, صحیح *lineno);
ساکن صحیح config__check(ساخت mosquitto__config *config);
ساکن خالی config__cleanup_plugins(خالی);
#اگر_تعریف_شدہ WITH_BRIDGE
ساکن صحیح config__check_bridges(ساخت mosquitto__config *config);
#اگر_ختم


ساکن صحیح config_جوڑو_listener(ساخت mosquitto__config *config)
{
	ساخت mosquitto__listener *listener;
	ساخت mosquitto__listener *new_listeners;
	صحیح def_listener = -1;

	اگر(config->default_listener){
		برائے(صحیح i=0; i<config->listener_گنتی; i++){
			اگر(&config->listeners[i] == config->default_listener){
				def_listener = i;
				توڑو;
			}
		}
	}
	new_listeners = mosquitto_realloc(config->listeners, حجم(ساخت mosquitto__listener)*(size_t)(config->listener_گنتی+1));
	اگر(!new_listeners){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
		واپس MOSQ_ERR_NOMEM;
	}
	config->listeners = new_listeners;
	listener = &config->listeners[config->listener_گنتی];
	memset(listener, 0, حجم(ساخت mosquitto__listener));
	listener->security_options = mosquitto_calloc(1, حجم(ساخت mosquitto__security_options));
	اگر(!listener->security_options){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
		واپس MOSQ_ERR_NOMEM;
	}
	اگر(def_listener != -1){
		config->default_listener = &config->listeners[def_listener];
	}
	config->listener_گنتی++;

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح config__create_default_listener(ساخت mosquitto__config *config, ثابت حرف *option_name)
{
	اگر(config->default_listener){
		واپس MOSQ_ERR_SUCCESS;
	}
	log__printf(تہی, MOSQ_LOG_INFO, "Creating default listener due to '%s' option.", option_name);
	log__printf(تہی, MOSQ_LOG_INFO, "It is best practice to define a 'listener' first. Using the '%s' option without a listener will be disabled in the future.", option_name);
	اگر(config_جوڑو_listener(config)){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
		واپس MOSQ_ERR_NOMEM;
	}

	config->default_listener = &config->listeners[config->listener_گنتی-1];
	listener_سیٹ_defaults(config->default_listener);
	config->default_listener->پورٹ = 1883;

	واپس MOSQ_ERR_SUCCESS;
}


ساکن خالی conf_سیٹ_cur_security_options(ساخت mosquitto__config *config, ساخت mosquitto__listener **cur_listener, ساخت mosquitto__security_options **security_options, ثابت حرف *option_name)
{
	اگر(config->فی_listener_settings){
		اگر(*cur_listener == تہی){
			اگر(config__create_default_listener(config, option_name)){
				واپس;
			}
			*cur_listener = config->default_listener;
		}
		(*security_options) = (*cur_listener)->security_options;
	}ورنہ{
		(*security_options) = &config->security_options;
	}
}


ساکن صحیح conf__attempt_resolve(ثابت حرف *host, ثابت حرف *متن, غیرعلامتی صحیح لوگارتھم, ثابت حرف *پیغام)
{
	ساخت addrinfo gai_hints;
	ساخت addrinfo *gai_res;
	صحیح rc;

	memset(&gai_hints, 0, حجم(ساخت addrinfo));
	gai_hints.ai_family = AF_UNSPEC;
	gai_hints.ai_socktype = SOCK_STREAM;
	gai_res = تہی;
	rc = getaddrinfo(host, تہی, &gai_hints, &gai_res);
	اگر(gai_res){
		freeaddrinfo(gai_res);
	}
	اگر(rc != 0){
#اگر_غیر_تعریف_شدہ WIN32
		اگر(rc == EAI_SYSTEM){
			اگر(errno == ENOENT){
				log__printf(تہی, لوگارتھم, "%s: Unable to resolve %s %s.", پیغام, متن, host);
			}ورنہ{
				log__printf(تہی, لوگارتھم, "%s: Error resolving %s: %s.", پیغام, متن, strerror(errno));
			}
		}ورنہ{
			log__printf(تہی, لوگارتھم, "%s: Error resolving %s: %s.", پیغام, متن, gai_strerror(rc));
		}
#ورنہ
		اگر(rc == WSAHOST_NOT_ملا){
			log__printf(تہی, لوگارتھم, "%s: Error resolving %s.", پیغام, متن);
		}
#اگر_ختم
		واپس MOSQ_ERR_INVAL;
	}
	واپس MOSQ_ERR_SUCCESS;
}


ساکن خالی config__init_reload(ساخت mosquitto__config *config)
{
	/* Set defaults */
	برائے(صحیح i=0; i<config->listener_گنتی; i++){
		listener_سیٹ_defaults(&config->listeners[i]);
	}

	config->local_only = true;
	config->allow_duplicate_messages = true;

	mosquitto_FREE(config->security_options.acl_ڈیٹا.acl_file);
	mosquitto_FREE(config->security_options.password_ڈیٹا.password_file);
	mosquitto_FREE(config->security_options.psk_file);

	config->security_options.allow_anonymous = -1;
	config->security_options.allow_zero_لمبائی_clientid = true;
	config->security_options.auto_id_سابقہ = تہی;
	config->security_options.auto_id_سابقہ_len = 0;

	config->autosave_interval = 1800;
	config->autosave_on_changes = false;

	mosquitto_FREE(config->clientid_prefixes);

	config->connection_messages = true;
	config->clientid_prefixes = تہی;
	config->فی_listener_settings = false;
	اگر(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = تہی;
	}
	mosquitto_FREE(config->log_file);

#اگر defined(WIN32) || defined(__CYGWIN__)
	اگر(service_handle){
		/* This is running as a Windows service. Default to no logging. Using
		 * stdout/stderr is forbidden because the first clients to connect will
		 * get log information sent to them for some reason. */
		config->log_dest = MQTT3_LOG_NONE;
	}ورنہ{
		config->log_dest = MQTT3_LOG_STDERR;
	}
#ورنہ
	config->log_facility = LOG_DAEMON;
	config->log_dest = MQTT3_LOG_STDERR | MQTT3_LOG_DLT;
	اگر(db.quiet){
		config->log_type = 0;
	}ورنہ اگر(db.verbose){
		config->log_type = UINT_MAX;
	}ورنہ{
		config->log_type = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;
	}
#اگر_ختم
	config->log_timestamp = true;
	mosquitto_FREE(config->log_timestamp_format);
	config->global_max_clients = -1;
	config->global_max_connections = -1;
	config->log_timestamp_format = تہی;
	config->max_keepalive = 0;
	config->max_packet_سائز = 2000000;
	config->max_inflight_messages = 20;
	config->max_queued_messages = 1000;
	config->max_inflight_بائٹیں = 0;
	config->max_queued_بائٹیں = 0;
	config->persistence = false;
	mosquitto_FREE(config->persistence_location);
	mosquitto_FREE(config->persistence_file);
	config->persistent_client_expiration = 0;
	config->قطار_qos0_messages = false;
	config->retain_دستیاب = true;
	config->retain_expiry_interval = 0;
	config->سیٹ_tcp_nodelay = false;
	config->sys_interval = 10;
	config->upgrade_outgoing_qos = false;
	config->packet_بفر_سائز = 4096;

	config->packet_max_auth = 100000;
	config->packet_max_connect = 100000;
	config->packet_max_sub = 100000;
	config->packet_max_simple = 10000;
}


ساکن خالی config__cleanup_plugin_config(mosquitto_plugin_id_t *plugin)
{
	mosquitto_FREE(plugin->config.راستہ);
	mosquitto_FREE(plugin->config.name);

	اگر(plugin->config.options){
		برائے(صحیح j=0; j<plugin->config.option_گنتی; j++){
			mosquitto_FREE(plugin->config.options[j].کلید);
			mosquitto_FREE(plugin->config.options[j].قدر);
		}
		mosquitto_FREE(plugin->config.options);
		plugin->config.option_گنتی = 0;
	}
	mosquitto_FREE(plugin->config.security_options);
	mosquitto_FREE(plugin);
}


ساکن خالی config__cleanup_plugins(خالی)
{
	برائے(صحیح i=0; i<db.plugin_گنتی; i++){
		config__cleanup_plugin_config(db.plugins[i]);
	}
	mosquitto_FREE(db.plugins);
}


خالی config__init(ساخت mosquitto__config *config)
{
	memset(config, 0, حجم(ساخت mosquitto__config));
	config__init_reload(config);

	config->daemon = false;
}


خالی config__cleanup(ساخت mosquitto__config *config)
{
	mosquitto_FREE(config->clientid_prefixes);
	mosquitto_FREE(config->persistence_location);
	mosquitto_FREE(config->persistence_file);
	mosquitto_FREE(config->persistence_filepath);
	mosquitto_FREE(config->security_options.auto_id_سابقہ);
	mosquitto_FREE(config->security_options.acl_ڈیٹا.acl_file);
	mosquitto_FREE(config->security_options.password_ڈیٹا.password_file);
	mosquitto_FREE(config->security_options.psk_file);
	mosquitto_FREE(config->security_options.plugins);
	mosquitto_FREE(config->pid_file);
	mosquitto_FREE(config->user);
	mosquitto_FREE(config->log_timestamp_format);
	اگر(config->listeners){
		برائے(صحیح i=0; i<config->listener_گنتی; i++){
			mosquitto_FREE(config->listeners[i].host);
			mosquitto_FREE(config->listeners[i].bind_interface);
			mosquitto_FREE(config->listeners[i].mount_نقطہ);
			mosquitto_FREE(config->listeners[i].socks);
			اگر(config->listeners[i].security_options){
				mosquitto_FREE(config->listeners[i].security_options->auto_id_سابقہ);
				mosquitto_FREE(config->listeners[i].security_options->acl_ڈیٹا.acl_file);
				mosquitto_FREE(config->listeners[i].security_options->password_ڈیٹا.password_file);
				mosquitto_FREE(config->listeners[i].security_options->psk_file);
				mosquitto_FREE(config->listeners[i].security_options->plugins);
				mosquitto_FREE(config->listeners[i].security_options);
			}
#اگر_تعریف_شدہ WITH_TLS
			mosquitto_FREE(config->listeners[i].cafile);
			mosquitto_FREE(config->listeners[i].capath);
			mosquitto_FREE(config->listeners[i].certfile);
			mosquitto_FREE(config->listeners[i].keyfile);
			mosquitto_FREE(config->listeners[i].ciphers);
			mosquitto_FREE(config->listeners[i].ciphers_tls13);
			mosquitto_FREE(config->listeners[i].psk_hint);
			mosquitto_FREE(config->listeners[i].crlfile);
			mosquitto_FREE(config->listeners[i].tls_نسخہ);
			mosquitto_FREE(config->listeners[i].tls_engine);
			mosquitto_FREE(config->listeners[i].tls_engine_kpass_sha1);
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			اگر(!config->listeners[i].ws_context) /* libwebsockets frees its own SSL_CTX */
#اگر_ختم
			{
				SSL_CTX_free(config->listeners[i].ssl_ctx);
				config->listeners[i].ssl_ctx = تہی;
			}
#اگر_ختم
#اگر defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
			mosquitto_FREE(config->listeners[i].http_dir);
#اگر_ختم
#اگر_تعریف_شدہ WITH_WEBSOCKETS
			برائے(صحیح j=0; j<config->listeners[i].ws_origin_گنتی; j++){
				mosquitto_FREE(config->listeners[i].ws_origins[j]);
			}
			mosquitto_FREE(config->listeners[i].ws_origins);
#اگر_ختم
#اگر_تعریف_شدہ WITH_UNIX_SOCKETS
			mosquitto_FREE(config->listeners[i].unix_socket_راستہ);
#اگر_ختم
		}
		mosquitto_FREE(config->listeners);
	}
#اگر_تعریف_شدہ WITH_BRIDGE
	اگر(config->bridges){
		برائے(صحیح i=0; i<config->bridge_گنتی; i++){
			config__bridge_cleanup(config->bridges[i]);
		}
		mosquitto_FREE(config->bridges);
	}
#اگر_ختم
	config__cleanup_plugins();

	اگر(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = تہی;
	}
	اگر(config->log_file){
		mosquitto_FREE(config->log_file);
		config->log_file = تہی;
	}
}

#اگر_تعریف_شدہ WITH_BRIDGE


خالی config__bridge_cleanup(ساخت mosquitto__bridge *bridge)
{
	اگر(bridge == تہی){
		واپس;
	}

	mosquitto_FREE(bridge->name);
	اگر(bridge->پتے){
		برائے(صحیح i=0; i<bridge->پتہ_گنتی; i++){
			mosquitto_FREE(bridge->پتے[i].پتہ);
		}
		mosquitto_FREE(bridge->پتے);
	}
	mosquitto_FREE(bridge->bind_پتہ);
	mosquitto_FREE(bridge->remote_clientid);
	mosquitto_FREE(bridge->remote_username);
	mosquitto_FREE(bridge->remote_password);
	mosquitto_FREE(bridge->local_clientid);
	mosquitto_FREE(bridge->local_username);
	mosquitto_FREE(bridge->local_password);
	اگر(bridge->topics){
		ساخت mosquitto__bridge_topic *cur_topic, *topic_tmp;

		LL_FOREACH_محفوظ(bridge->topics, cur_topic, topic_tmp){
			mosquitto_FREE(cur_topic->topic);
			mosquitto_FREE(cur_topic->local_سابقہ);
			mosquitto_FREE(cur_topic->remote_سابقہ);
			mosquitto_FREE(cur_topic->local_topic);
			mosquitto_FREE(cur_topic->remote_topic);
			LL_DELETE(bridge->topics, cur_topic);
			mosquitto_FREE(cur_topic);
		}
		mosquitto_FREE(bridge->topics);
	}
	mosquitto_FREE(bridge->notification_topic);
#اگر_تعریف_شدہ WITH_TLS
	mosquitto_FREE(bridge->tls_certfile);
	mosquitto_FREE(bridge->tls_keyfile);
	mosquitto_FREE(bridge->tls_نسخہ);
	mosquitto_FREE(bridge->tls_cafile);
	mosquitto_FREE(bridge->tls_capath);
	mosquitto_FREE(bridge->tls_alpn);
	mosquitto_FREE(bridge->tls_ciphers);
	mosquitto_FREE(bridge->tls_13_ciphers);
#اگر_تعریف_شدہ FINAL_WITH_TLS_PSK
	mosquitto_FREE(bridge->tls_psk_شناختی_عنصر);
	mosquitto_FREE(bridge->tls_psk);
#اگر_ختم
#اگر_ختم
	mosquitto_FREE(bridge);
}
#اگر_ختم


ساکن خالی print_نسخہ(خالی)
{
	لکھو("mosquitto %s\n", نسخہ);
	لکھو("Copyright © 2025 Roger Light.\n");
	لکھو("License EPL-2.0 OR BSD-3-Clause.\n");
}


ساکن خالی print_usage(خالی)
{
	لکھو("mosquitto version %s\n\n", نسخہ);
	لکھو("mosquitto is an MQTT v5.0/v3.1.1/v3.1 broker.\n\n");
	لکھو("Usage: mosquitto [-c config_file] [-d] [-h] [-p port] [-v]\n");
	لکھو("                 [--tls-keylog file]\n\n");
	لکھو(" -c : specify the broker config file.\n");
	لکھو(" -d : put the broker into the background after starting.\n");
	لکھو(" -h : display this help.\n");
	لکھو(" -p : start the broker listening on the specified port.\n");
	لکھو("      Not recommended in conjunction with the -c option.\n");
	لکھو(" -q : quiet mode - disable all logging types. This overrides\n");
	لکھو("      any logging options given in the config file, and -v.\n");
	لکھو(" -v : verbose mode - enable all logging types. This overrides\n");
	لکھو("      any logging options given in the config file.\n");
	لکھو(" --test-config : test config file and exit\n");
	لکھو(" --tls-keylog : log TLS connection information to a file, to allow\n");
	لکھو("      debugging with e.g. wireshark. Do not use on a production\n");
	لکھو("      server.\n");
	لکھو("\nSee https://mosquitto.org/ for more information.\n\n");
}


صحیح config_parse_دلائل(ساخت mosquitto__config *config, صحیح argc, حرف *argv[])
{
	صحیح i;
	صحیح پورٹ_tmp;

	برائے(i=1; i<argc; i++){
		اگر(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--config-file")){
			اگر(i<argc-1){
				db.config_file = argv[i+1];

				اگر(config__read(config, false)){
					واپس MOSQ_ERR_INVAL;
				}
			}ورنہ{
				log__printf(تہی, MOSQ_LOG_ERR, "Error: -c argument given, but no config file specified.");
				واپس MOSQ_ERR_INVAL;
			}
			i++;
		}ورنہ اگر(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--daemon")){
			config->daemon = true;
		}ورنہ اگر(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
			print_usage();
			واپس MOSQ_ERR_UNKNOWN;
		}ورنہ اگر(!strcmp(argv[i], "--version")){
			print_نسخہ();
			واپس MOSQ_ERR_UNKNOWN;
		}ورنہ اگر(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port")){
			اگر(i<argc-1){
				پورٹ_tmp = atoi(argv[i+1]);
				اگر(پورٹ_tmp<1 || پورٹ_tmp>UINT16_MAX){
					log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid port specified (%d).", پورٹ_tmp);
					واپس MOSQ_ERR_INVAL;
				}ورنہ{
					اگر(config->cmd_پورٹ_گنتی == CMD_پورٹ_حد_بندی){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Only %d ports can be specified on the command line.", CMD_پورٹ_حد_بندی);
						واپس MOSQ_ERR_INVAL;
					}
					config->cmd_پورٹ[config->cmd_پورٹ_گنتی] = (uint16_t)پورٹ_tmp;
					config->cmd_پورٹ_گنتی++;
				}
			}ورنہ{
				log__printf(تہی, MOSQ_LOG_ERR, "Error: -p argument given, but no port specified.");
				واپس MOSQ_ERR_INVAL;
			}
			i++;
		}ورنہ اگر(!strcmp(argv[i], "--tls-keylog")){
#اگر_تعریف_شدہ WITH_TLS
			اگر(i<argc-1){
				db.tls_keylog = mosquitto_strdup(argv[i+1]);
				اگر(db.tls_keylog == تہی){
					log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
					واپس MOSQ_ERR_NOMEM;
				}
			}ورنہ{
				log__printf(تہی, MOSQ_LOG_ERR, "Error: --tls-keylog argument given, but no file specified.");
				واپس MOSQ_ERR_INVAL;
			}
			i++;
#ورنہ
			fprintf(stderr, "Error: TLS support not available so --tls-keylog is not available.\n");
			واپس MOSQ_ERR_INVAL;
#اگر_ختم
		}ورنہ اگر(!strcmp(argv[i], "-q") || !strcmp(argv[i], "--quiet")){
			db.quiet = true;
		}ورنہ اگر(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")){
			db.verbose = true;
		}ورنہ اگر(!strcmp(argv[i], "--test-config")){
			config->test_configuration = true;
		}ورنہ{
			fprintf(stderr, "Error: Unknown option '%s'.\n", argv[i]);
			print_usage();
			واپس MOSQ_ERR_INVAL;
		}
	}

	/* Default to drop to mosquitto user if we are privileged and no user specified. */
	اگر(!config->user){
		config->user = mosquitto_strdup("mosquitto");
		اگر(config->user == تہی){
			واپس MOSQ_ERR_NOMEM;
		}
	}
	اگر(db.quiet){
		config->log_type = 0;
	}ورنہ اگر(db.verbose){
		config->log_type = UINT_MAX;
	}

	اگر(getenv("MOSQUITTO_PERSISTENCE_LOCATION")){
		mosquitto_FREE(config->persistence_location);
		config->persistence_location = mosquitto_strdup(getenv("MOSQUITTO_PERSISTENCE_LOCATION"));
		اگر(!config->persistence_location){
			واپس MOSQ_ERR_NOMEM;
		}
	}
	واپس config__check(config);
}


ساکن خالی config__copy(ساخت mosquitto__config *src, ساخت mosquitto__config *dest)
{
	mosquitto_FREE(dest->security_options.acl_ڈیٹا.acl_file);
	dest->security_options.acl_ڈیٹا.acl_file = src->security_options.acl_ڈیٹا.acl_file;

	acl_file__cleanup(&dest->security_options.acl_ڈیٹا);
	dest->security_options.acl_ڈیٹا.acl_users = src->security_options.acl_ڈیٹا.acl_users;
	dest->security_options.acl_ڈیٹا.acl_patterns = src->security_options.acl_ڈیٹا.acl_patterns;
	dest->security_options.acl_ڈیٹا.acl_anon.username = src->security_options.acl_ڈیٹا.acl_anon.username;
	dest->security_options.acl_ڈیٹا.acl_anon.acl = src->security_options.acl_ڈیٹا.acl_anon.acl;

	dest->security_options.allow_anonymous = src->security_options.allow_anonymous;
	dest->security_options.allow_zero_لمبائی_clientid = src->security_options.allow_zero_لمبائی_clientid;

	mosquitto_FREE(dest->security_options.auto_id_سابقہ);
	dest->security_options.auto_id_سابقہ = src->security_options.auto_id_سابقہ;
	dest->security_options.auto_id_سابقہ_len = src->security_options.auto_id_سابقہ_len;

	mosquitto_FREE(dest->security_options.password_ڈیٹا.password_file);
	dest->security_options.password_ڈیٹا.password_file = src->security_options.password_ڈیٹا.password_file;

	password_file__cleanup(&dest->security_options.password_ڈیٹا);
	dest->security_options.password_ڈیٹا.unpwd = src->security_options.password_ڈیٹا.unpwd;

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

	dest->پیغام_سائز_حد_بندی = src->پیغام_سائز_حد_بندی;

	dest->persistence = src->persistence;

	mosquitto_FREE(dest->persistence_location);
	dest->persistence_location = src->persistence_location;

	mosquitto_FREE(dest->persistence_file);
	dest->persistence_file = src->persistence_file;

	mosquitto_FREE(dest->persistence_filepath);
	dest->persistence_filepath = src->persistence_filepath;

	dest->persistent_client_expiration = src->persistent_client_expiration;


	dest->قطار_qos0_messages = src->قطار_qos0_messages;
	dest->sys_interval = src->sys_interval;
	dest->upgrade_outgoing_qos = src->upgrade_outgoing_qos;

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	dest->websockets_log_level = src->websockets_log_level;
#اگر_ختم

#اگر_تعریف_شدہ WITH_BRIDGE
	برائے(صحیح i=0; i<dest->bridge_گنتی; i++){
		اگر(dest->bridges[i]){
			config__bridge_cleanup(dest->bridges[i]);
		}
	}
	mosquitto_FREE(dest->bridges);
	dest->bridges = src->bridges;
	dest->bridge_گنتی = src->bridge_گنتی;
#اگر_ختم
}


صحیح config__read(ساخت mosquitto__config *config, bool reload)
{
	صحیح rc = MOSQ_ERR_SUCCESS;
	ساخت config_recurse cr;
	صحیح lineno = 0;
#اگر_تعریف_شدہ WITH_PERSISTENCE
	size_t len;
#اگر_ختم
	ساخت mosquitto__config config_reload;
	صحیح i;

	اگر(reload){
		memset(&config_reload, 0, حجم(ساخت mosquitto__config));
	}

	cr.log_dest = MQTT3_LOG_NONE;
	cr.log_dest_سیٹ = 0;
	cr.log_type = MOSQ_LOG_NONE;
	cr.log_type_سیٹ = 0;

	اگر(!db.config_file){
		واپس 0;
	}

	اگر(reload){
		/* Re-initialise appropriate config vars to default for reload. */
		config__init_reload(&config_reload);
		config_reload.listeners = config->listeners;
		config_reload.listener_گنتی = config->listener_گنتی;
		cur_security_options = تہی;
		rc = config__read_file(&config_reload, reload, db.config_file, &cr, 0, &lineno);
	}ورنہ{
		rc = config__read_file(config, reload, db.config_file, &cr, 0, &lineno);
	}
	اگر(rc){
		اگر(lineno > 0){
			log__printf(تہی, MOSQ_LOG_ERR, "Error found at %s:%d.", db.config_file, lineno);
		}
		واپس rc;
	}

	اگر(reload){
		config__copy(&config_reload, config);
	}

	/* If auth/access options are set and allow_anonymous not explicitly set, disallow anon. */
	اگر(config->local_only == false){
		اگر(config->فی_listener_settings){
			برائے(i=0; i<config->listener_گنتی; i++){
				/* Default option if no security options set */
				اگر(config->listeners[i].security_options->allow_anonymous == -1){
					config->listeners[i].security_options->allow_anonymous = false;
				}
			}
		}ورنہ{
			اگر(config->security_options.allow_anonymous == -1){
				config->security_options.allow_anonymous = false;
			}
		}
	}
#اگر_تعریف_شدہ WITH_PERSISTENCE
	اگر(config->persistence){
		اگر(!config->persistence_file){
			config->persistence_file = mosquitto_strdup("mosquitto.db");
			اگر(!config->persistence_file){
				واپس MOSQ_ERR_NOMEM;
			}
		}
		mosquitto_FREE(config->persistence_filepath);
		اگر(config->persistence_location && لڑی_لمبائی(config->persistence_location)){
			len = لڑی_لمبائی(config->persistence_location) + لڑی_لمبائی(config->persistence_file) + 2;
			config->persistence_filepath = mosquitto_malloc(len);
			اگر(!config->persistence_filepath){
				واپس MOSQ_ERR_NOMEM;
			}
#اگر_تعریف_شدہ WIN32
			snprintf(config->persistence_filepath, len, "%s\\%s", config->persistence_location, config->persistence_file);
#ورنہ
			snprintf(config->persistence_filepath, len, "%s/%s", config->persistence_location, config->persistence_file);
#اگر_ختم
		}ورنہ{
			config->persistence_filepath = mosquitto_strdup(config->persistence_file);
			اگر(!config->persistence_filepath){
				واپس MOSQ_ERR_NOMEM;
			}
		}
	}
#اگر_ختم
	/* Default to drop to mosquitto user if no other user specified. This must
	 * remain here even though it is covered in config__parse_args() because this
	 * function may be called on its own. */
	اگر(!config->user){
		config->user = mosquitto_strdup("mosquitto");
	}

#اگر_تعریف_شدہ WITH_BRIDGE
	برائے(i=0; i<config->bridge_گنتی; i++){
		اگر(!config->bridges[i]->name){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: bridge name not defined.");
			واپس MOSQ_ERR_INVAL;
		}
		اگر(config->bridges[i]->پتے  == 0){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no remote addresses defined.");
			واپس MOSQ_ERR_INVAL;
		}
		اگر(config->bridges[i]->topic_گنتی == 0){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no topics defined.");
			واپس MOSQ_ERR_INVAL;
		}
#اگر_تعریف_شدہ FINAL_WITH_TLS_PSK
		اگر(config->bridges[i]->tls_psk && !config->bridges[i]->tls_psk_شناختی_عنصر){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_identity.");
			واپس MOSQ_ERR_INVAL;
		}
		اگر(config->bridges[i]->tls_psk_شناختی_عنصر && !config->bridges[i]->tls_psk){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_psk.");
			واپس MOSQ_ERR_INVAL;
		}
#اگر_ختم
	}
#اگر_ختم

	اگر(cr.log_dest_سیٹ){
		config->log_dest = cr.log_dest;
	}
	اگر(db.quiet){
		config->log_type = 0;
	}ورنہ اگر(db.verbose){
		config->log_type = UINT_MAX;
	}ورنہ اگر(cr.log_type_سیٹ){
		config->log_type = cr.log_type;
	}
#اگر_تعریف_شدہ WITH_BRIDGE
	واپس config__check_bridges(config);
#ورنہ
	واپس MOSQ_ERR_SUCCESS;
#اگر_ختم
}


ساکن mosquitto_plugin_id_t *config_plugin_ڈھونڈو(ثابت حرف *name)
{
	اگر(db.plugins && name){
		برائے(صحیح i=0; i<db.plugin_گنتی; i++){
			اگر(db.plugins[i]->config.name && !strcmp(db.plugins[i]->config.name, name)){
				واپس db.plugins[i];
			}
		}
	}
	واپس تہی;
}


ساکن mosquitto_plugin_id_t *config_plugin_بھرو(ثابت حرف *name, ثابت حرف *راستہ)
{
	mosquitto_plugin_id_t *plugin = تہی;
	mosquitto_plugin_id_t **plugins = تہی;

	اگر(name && config_plugin_ڈھونڈو(name)){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Duplicate plugin name '%s'.", name);
		واپس تہی;
	}

	اگر(!راستہ || !strcmp(راستہ, "")){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Missing plugin path for plugin name '%s'.", name);
		واپس تہی;
	}

	plugin = mosquitto_calloc(1, حجم(mosquitto_plugin_id_t));
	اگر(!plugin){
		جاؤ error;
	}

	اگر(name){
		plugin->config.name = mosquitto_strdup(name);
	}
	plugin->config.راستہ = mosquitto_strdup(راستہ);
	اگر((name && !plugin->config.name) || !plugin->config.راستہ){
		جاؤ error;
	}
	plugin->config.options = تہی;
	plugin->config.option_گنتی = 0;
	plugin->config.deny_special_chars = true;

	/* Add to db list */
	plugins = mosquitto_realloc(db.plugins, (size_t)(db.plugin_گنتی+1)*حجم(ساخت mosquitto__plugin_config *));
	اگر(!plugins){
		جاؤ error;
	}

	plugins[db.plugin_گنتی] = plugin;
	db.plugins = plugins;
	db.plugin_گنتی++;

	واپس plugin;
error:
	log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
	اگر(plugin){
		mosquitto_FREE(plugin->config.name);
		mosquitto_FREE(plugin->config.راستہ);
	}
	mosquitto_FREE(plugin);
	واپس تہی;
}


صحیح config_plugin_جوڑو_secopt(mosquitto_plugin_id_t *plugin, ساخت mosquitto__security_options *security_options)
{
	ساخت mosquitto__security_options **new_options;
	mosquitto_plugin_id_t **new_plugins;

	new_options = mosquitto_realloc(plugin->config.security_options, (size_t)(plugin->config.security_option_گنتی+1)*حجم(ساخت mosquitto__security_options *));
	new_plugins = mosquitto_realloc(security_options->plugins, (size_t)(security_options->plugin_گنتی+1)*حجم(mosquitto_plugin_id_t *));

	اگر(!new_options || !new_plugins){
		mosquitto_FREE(new_options);
		mosquitto_FREE(new_plugins);
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
		واپس MOSQ_ERR_NOMEM;
	}

	new_options[plugin->config.security_option_گنتی] = security_options;
	plugin->config.security_options = new_options;
	plugin->config.security_option_گنتی++;

	new_plugins[security_options->plugin_گنتی] = plugin;
	security_options->plugins = new_plugins;
	security_options->plugin_گنتی++;

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح config__read_file_core(ساخت mosquitto__config *config, bool reload, ساخت config_recurse *cr, صحیح level, صحیح *lineno, FILE *fptr, حرف **buf, صحیح *buflen)
{
	صحیح rc;
	حرف *token;
	صحیح tmp_int;
	حرف *saveptr = تہی;
#اگر_تعریف_شدہ WITH_BRIDGE
	حرف *tmp_char;
	ساخت mosquitto__bridge *cur_bridge = تہی;
#اگر_ختم
	mosquitto_plugin_id_t *cur_plugin = تہی;

	حرف *کلید;
	ساخت mosquitto__listener *cur_listener = تہی;
	صحیح i;
	صحیح lineno_ext = 0;
	size_t سابقہ_len;
	size_t slen;
#اگر_تعریف_شدہ WITH_WEBSOCKETS
	حرف **ws_origins = تہی;
#اگر_ختم

	*lineno = 0;

	جب(mosquitto_fgets(buf, buflen, fptr)){
		(*lineno)++;
		اگر((*buf)[0] != '#' && (*buf)[0] != 10 && (*buf)[0] != 13){
			slen = لڑی_لمبائی(*buf);
			اگر(slen == 0){
				جاری;
			}
			جب((*buf)[slen-1] == 10 || (*buf)[slen-1] == 13){
				(*buf)[slen-1] = 0;
				slen = لڑی_لمبائی(*buf);
				اگر(slen == 0){
					جاری;
				}
			}
			token = strtok_r((*buf), " ", &saveptr);
			اگر(token){
				اگر(!strcmp(token, "accept_protocol_versions")){
					REQUIRE_NON_DEFAULT_LISTENER(token);
					cur_listener->disable_protocol_v3 = true;
					cur_listener->disable_protocol_v4 = true;
					cur_listener->disable_protocol_v5 = true;
					اگر(saveptr == تہی){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", "accept_protocol_versions");
						واپس MOSQ_ERR_INVAL;
					}
					token = strtok_r(saveptr, ", \t", &saveptr);
					REQUIRE_NON_خالی_خانے_OPTION(token, "accept_protocol_versions");

					جب(token){
						اگر(!strcmp(token, "3")){
							cur_listener->disable_protocol_v3 = false;
						}ورنہ اگر(!strcmp(token, "4")){
							cur_listener->disable_protocol_v4 = false;
						}ورنہ اگر(!strcmp(token, "5")){
							cur_listener->disable_protocol_v5 = false;
						}

						token = strtok_r(تہی, ", \t", &saveptr);
					}
				}ورنہ اگر(!strcmp(token, "acl_file")){
					REQUIRE_LISTENER_IF_فی_LISTENER(token);

					conf_سیٹ_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->acl_ڈیٹا.acl_file);
					اگر(conf__parse_string(&token, "acl_file", &cur_security_options->acl_ڈیٹا.acl_file, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "address") || !strcmp(token, "addresses")){
#اگر_تعریف_شدہ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(cur_bridge->پتے){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid bridge configuration, 'address' only allowed once.");
						واپس MOSQ_ERR_INVAL;
					}
					جب((token = strtok_r(تہی, " ", &saveptr))){
						اگر(token[0] == '#'){
							توڑو;
						}
						ساخت bridge_پتہ *new_پتے = mosquitto_realloc(cur_bridge->پتے, حجم(ساخت bridge_پتہ)*(size_t)(cur_bridge->پتہ_گنتی+1));
						اگر(!new_پتے){
							log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
							واپس MOSQ_ERR_NOMEM;
						}
						cur_bridge->پتہ_گنتی++;
						cur_bridge->پتے = new_پتے;
						memset(&cur_bridge->پتے[cur_bridge->پتہ_گنتی-1], 0, حجم(ساخت bridge_پتہ));
						cur_bridge->پتے[cur_bridge->پتہ_گنتی-1].پتہ = mosquitto_strdup(token);
						اگر(!cur_bridge->پتے[cur_bridge->پتہ_گنتی-1].پتہ){
							log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
							واپس MOSQ_ERR_NOMEM;
						}
					}
					برائے(i=0; i<cur_bridge->پتہ_گنتی; i++){
						/* cur_bridge->addresses[i].address is now
						 * "address[:port]". If address is an IPv6 address,
						 * then port is required. We must check for the :
						 * backwards. */
						tmp_char = strrchr(cur_bridge->پتے[i].پتہ, ':');
						اگر(tmp_char){
							/* Remove ':', so cur_bridge->addresses[i].address
							 * now just looks like the address. */
							tmp_char[0] = '\0';

							/* The remainder of the string */
							tmp_int = atoi(&tmp_char[1]);
							اگر(tmp_int < 1 || tmp_int > UINT16_MAX){
								log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid bridge port value (%d).", tmp_int);
								واپس MOSQ_ERR_INVAL;
							}
							cur_bridge->پتے[i].پورٹ = (uint16_t)tmp_int;
						}ورنہ{
							cur_bridge->پتے[i].پورٹ = 1883;
						}
						conf__attempt_resolve(cur_bridge->پتے[i].پتہ, "bridge address", MOSQ_LOG_WARNING, "Warning");
					}
					اگر(cur_bridge->پتہ_گنتی == 0){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Empty address value in configuration.");
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "allow_anonymous")){
					REQUIRE_LISTENER_IF_فی_LISTENER(token);
					فی_LISTENER_ALTERNATIVE(token, "listener_allow_anonymous");
					conf_سیٹ_cur_security_options(config, &cur_listener, &cur_security_options, token);
					اگر(conf__parse_bool(&token, "allow_anonymous", (bool *)&cur_security_options->allow_anonymous, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "allow_duplicate_messages")){
					OPTION_DEPRECATED(token, "The behaviour will default to true.");
					اگر(conf__parse_bool(&token, "allow_duplicate_messages", &config->allow_duplicate_messages, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "allow_zero_length_clientid")){
					REQUIRE_LISTENER_IF_فی_LISTENER(token);
					conf_سیٹ_cur_security_options(config, &cur_listener, &cur_security_options, token);
					اگر(conf__parse_bool(&token, "allow_zero_length_clientid", &cur_security_options->allow_zero_لمبائی_clientid, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!لڑی_موازنہ(token, "auth_opt_", لڑی_لمبائی("auth_opt_")) || !لڑی_موازنہ(token, "plugin_opt_", لڑی_لمبائی("plugin_opt_"))){
					اگر(reload){
						جاری;        /* Auth plugin not currently valid for reloading. */

					}
					REQUIRE_PLUGIN(token);

					اگر(!لڑی_موازنہ(token, "auth_opt_", لڑی_لمبائی("auth_opt_"))){
						سابقہ_len = لڑی_لمبائی("auth_opt_");
					}ورنہ{
						سابقہ_len = لڑی_لمبائی("plugin_opt_");
					}
					اگر(لڑی_لمبائی(token) < سابقہ_len + 3){
						/* auth_opt_ == 9, + one digit key == 10, + one space == 11, + one value == 12 */
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid 'plugin_opt_' config option.");
						واپس MOSQ_ERR_INVAL;
					}
					کلید = mosquitto_strdup(&token[سابقہ_len]);
					اگر(!کلید){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
						واپس MOSQ_ERR_NOMEM;
					}ورنہ اگر(STREMPTY(کلید)){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Empty 'plugin_opt_' config option.");
						mosquitto_FREE(کلید);
						واپس MOSQ_ERR_INVAL;
					}
					token = saveptr;
					اگر(token && token[0]){
						جب(token[0] == ' ' || token[0] == '\t'){
							token++;
						}
						ساخت mosquitto_opt *new_options = mosquitto_realloc(cur_plugin->config.options, (size_t)(cur_plugin->config.option_گنتی+1)*حجم(ساخت mosquitto_opt));
						اگر(!new_options){
							log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
							mosquitto_FREE(کلید);
							واپس MOSQ_ERR_NOMEM;
						}
						cur_plugin->config.option_گنتی++;
						cur_plugin->config.options = new_options;
						cur_plugin->config.options[cur_plugin->config.option_گنتی-1].کلید = کلید;
						cur_plugin->config.options[cur_plugin->config.option_گنتی-1].قدر = mosquitto_strdup(token);
						اگر(!cur_plugin->config.options[cur_plugin->config.option_گنتی-1].قدر){
							log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
							واپس MOSQ_ERR_NOMEM;
						}
					}ورنہ{
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", کلید);
						mosquitto_FREE(کلید);
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "auth_plugin") || !strcmp(token, "plugin") || !strcmp(token, "global_plugin")){
					اگر(reload){
						جاری;        /* plugin not currently valid for reloading. */
					}
					اگر(!strcmp(token, "global_plugin")){
						cur_security_options = &db.config->security_options;
					}ورنہ{
						REQUIRE_LISTENER_IF_فی_LISTENER(token);
						conf_سیٹ_cur_security_options(config, &cur_listener, &cur_security_options, token);
					}

					cur_plugin = config_plugin_بھرو(تہی, saveptr);
					اگر(cur_plugin == تہی){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(config_plugin_جوڑو_secopt(cur_plugin, cur_security_options)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "auth_plugin_deny_special_chars")){
					اگر(reload){
						جاری;        /* Auth plugin not currently valid for reloading. */
					}
					اگر(!cur_plugin){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: An auth_plugin_deny_special_chars option exists in the config file without a plugin.");
						واپس MOSQ_ERR_INVAL;
					}
					اگر(conf__parse_bool(&token, "auth_plugin_deny_special_chars", &cur_plugin->config.deny_special_chars, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "plugin_load")){
					حرف *name = تہی;

					اگر(reload){
						جاری;        /* plugin not currently valid for reloading. */

					}
					name = strtok_r(تہی, " ", &saveptr);
					REQUIRE_NON_خالی_خانے_OPTION(name, "plugin_load");

					cur_plugin = config_plugin_بھرو(name, saveptr);
					اگر(cur_plugin == تہی){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "plugin_use")){
					حرف *name = تہی;

					اگر(reload){
						جاری;        /* plugin not currently valid for reloading. */
					}
					REQUIRE_NON_DEFAULT_LISTENER(token);

					اگر(conf__parse_string(&token, "plugin_use", &name, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					cur_plugin = config_plugin_ڈھونڈو(name);
					اگر(!cur_plugin){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Plugin '%s' not previously loaded.", name);
						mosquitto_FREE(name);
						واپس MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(name);
					اگر(config_plugin_جوڑو_secopt(cur_plugin, cur_listener->security_options)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "auto_id_prefix")){
					REQUIRE_LISTENER_IF_فی_LISTENER(token);
					OPTION_DEPRECATED(token, "Please use 'listener_auto_id_prefix' instead.");
					conf_سیٹ_cur_security_options(config, &cur_listener, &cur_security_options, token);
					اگر(conf__parse_string(&token, "auto_id_prefix", &cur_security_options->auto_id_سابقہ, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(cur_security_options->auto_id_سابقہ){
						اگر(لڑی_لمبائی(cur_security_options->auto_id_سابقہ) > 50){
							log__printf(تہی, MOSQ_LOG_ERR, "Error: auto_id_prefix length must be <= 50.");
							واپس MOSQ_ERR_INVAL;
						}
						cur_security_options->auto_id_سابقہ_len = (uint16_t)لڑی_لمبائی(cur_security_options->auto_id_سابقہ);
					}ورنہ{
						cur_security_options->auto_id_سابقہ_len = 0;
					}
				}ورنہ اگر(!strcmp(token, "autosave_interval")){
					اگر(conf__parse_int(&token, "autosave_interval", &config->autosave_interval, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(config->autosave_interval < 0){
						config->autosave_interval = 0;
					}
				}ورنہ اگر(!strcmp(token, "autosave_on_changes")){
					اگر(conf__parse_bool(&token, "autosave_on_changes", &config->autosave_on_changes, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "bind_address")){
					OPTION_DEPRECATED(token, "");
					config->local_only = false;
					اگر(reload){
						جاری;        /* Rebinding listeners not valid during reloading. */

					}
					اگر(config__create_default_listener(config, token)){
						واپس MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					اگر(conf__parse_string(&token, "default listener bind_address", &config->default_listener->host, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(conf__attempt_resolve(config->default_listener->host, "bind_address", MOSQ_LOG_ERR, "Error")){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "bind_interface")){
#اگر_تعریف_شدہ SO_BINDTODEVICE
					اگر(reload){
						جاری;        /* Rebinding listeners not valid during reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					اگر(conf__parse_string(&token, "bind_interface", &cur_listener->bind_interface, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_ERR, "Error: bind_interface specified but socket option not available.");
					واپس MOSQ_ERR_INVAL;
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "bridge_attempt_unsubscribe")){
#اگر_تعریف_شدہ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_bool(&token, "bridge_attempt_unsubscribe", &cur_bridge->attempt_unsubscribe, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "bridge_cafile")){
#اگر defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					اگر(conf__parse_string(&token, "bridge_cafile", &cur_bridge->tls_cafile, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "bridge_alpn")){
#اگر defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_string(&token, "bridge_alpn", &cur_bridge->tls_alpn, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "bridge_ciphers")){
#اگر defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_string(&token, "bridge_ciphers", &cur_bridge->tls_ciphers, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "bridge_ciphers_tls1.3")){
#اگر defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_string(&token, "bridge_ciphers_tls1.3", &cur_bridge->tls_13_ciphers, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "bridge_bind_address")){
#اگر defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_string(&token, "bridge_bind_address", &cur_bridge->bind_پتہ, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "bridge_capath")){
#اگر defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					اگر(conf__parse_string(&token, "bridge_capath", &cur_bridge->tls_capath, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "bridge_certfile")){
#اگر defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					اگر(conf__parse_string(&token, "bridge_certfile", &cur_bridge->tls_certfile, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "bridge_identity")){
#اگر defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					اگر(conf__parse_string(&token, "bridge_identity", &cur_bridge->tls_psk_شناختی_عنصر, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "bridge_insecure")){
#اگر defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_bool(&token, "bridge_insecure", &cur_bridge->tls_insecure, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(cur_bridge->tls_insecure){
						log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge '%s' using insecure mode.", cur_bridge->name);
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "bridge_require_ocsp")){
#اگر defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_bool(&token, "bridge_require_ocsp", &cur_bridge->tls_ocsp_required, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "bridge_max_packet_size")){
#اگر defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_int(&token, "bridge_max_packet_size", &tmp_int, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 0){
						tmp_int = 0;
					}
					cur_bridge->زیادہ_سے_زیادہ_packet_سائز = (uint32_t)tmp_int;
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "bridge_max_topic_alias")){
#اگر_تعریف_شدہ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_int(&token, "bridge_max_topic_alias", &tmp_int, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}

					اگر(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: bridge_max_topic_alias must be > 0 and <= 65535.");
						واپس MOSQ_ERR_INVAL;
					}
					cur_bridge->max_topic_alias = (uint16_t)tmp_int;
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "bridge_outgoing_retain")){
#اگر defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_bool(&token, "bridge_outgoing_retain", &cur_bridge->outgoing_retain, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "bridge_keyfile")){
#اگر defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					mosquitto_FREE(cur_bridge->tls_keyfile);
					اگر(conf__parse_string(&token, "bridge_keyfile", &cur_bridge->tls_keyfile, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "bridge_protocol_version")){
#اگر_تعریف_شدہ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(تہی, "", &saveptr);
					REQUIRE_NON_خالی_خانے_OPTION(token, "bridge_protocol_version");

					اگر(!strcmp(token, "mqttv31")){
						cur_bridge->protocol_نسخہ = mosq_p_mqtt31;
					}ورنہ اگر(!strcmp(token, "mqttv311")){
						cur_bridge->protocol_نسخہ = mosq_p_mqtt311;
					}ورنہ اگر(!strcmp(token, "mqttv50")){
						cur_bridge->protocol_نسخہ = mosq_p_mqtt5;
					}ورنہ{
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid 'bridge_protocol_version' value (%s).", token);
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "bridge_psk")){
#اگر defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					اگر(conf__parse_string(&token, "bridge_psk", &cur_bridge->tls_psk, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "bridge_receive_maximum")){
#اگر defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_int(&token, "bridge_receive_maximum", &tmp_int, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(tmp_int <= 0){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be greater than 0.");
						واپس MOSQ_ERR_INVAL;
					}ورنہ اگر((uint64_t)tmp_int > (uint64_t)UINT16_MAX){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be lower than %u.", UINT16_MAX);
						واپس MOSQ_ERR_INVAL;
					}
					cur_bridge->receive_زیادہ_سے_زیادہ = (uint16_t)tmp_int;
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "bridge_reload_type")){
#اگر_تعریف_شدہ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(تہی, " ", &saveptr);
					REQUIRE_NON_خالی_خانے_OPTION(token, "bridge_reload_type");

					اگر(!strcmp(token, "lazy")){
						cur_bridge->reload_type = brt_lazy;
					}ورنہ اگر(!strcmp(token, "immediate")){
						cur_bridge->reload_type = brt_فوری;
					}ورنہ{
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid 'bridge_reload_type' value in configuration (%s).", token);
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "bridge_session_expiry_interval")){
#اگر defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_int(&token, "bridge_session_expiry_interval", &tmp_int, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 0){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must not be negative.");
						واپس MOSQ_ERR_INVAL;
					}ورنہ اگر((uint64_t)tmp_int > (uint64_t)MQTT_SESSION_EXPIRY_NEVER){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must be lower than %u.", MQTT_SESSION_EXPIRY_NEVER);
						واپس MOSQ_ERR_INVAL;
					}
					cur_bridge->session_expiry_interval = (uint32_t)tmp_int;
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "bridge_tcp_keepalive")){
#اگر_تعریف_شدہ WITH_BRIDGE
					REQUIRE_BRIDGE(token);

					اگر(conf__parse_int(&token, "bridge_tcp_keepalive_idle", &tmp_int, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(tmp_int <= 0){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: invalid TCP keepalive idle value.");
						واپس MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_idle = (غیرعلامتی صحیح)tmp_int;

					اگر(conf__parse_int(&token, "bridge_tcp_keepalive_interval", &tmp_int, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(tmp_int <= 0){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: invalid TCP keepalive interval value.");
						واپس MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_interval = (غیرعلامتی صحیح)tmp_int;

					اگر(conf__parse_int(&token, "bridge_tcp_keepalive_counter", &tmp_int, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(tmp_int <= 0){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: invalid TCP keepalive counter value.");
						واپس MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_شمار = (غیرعلامتی صحیح)tmp_int;
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "bridge_tcp_user_timeout")){
#اگر_تعریف_شدہ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
#اگر_تعریف_شدہ WITH_TCP_USER_TIMEOUT
					اگر(conf__parse_int(&token, "bridge_tcp_user_timeout", &tmp_int, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 0){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: invalid TCP user timeout value.");
						واپس MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_user_timeout = tmp_int;
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge TCP user timeout support not available.");
#اگر_ختم
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "bridge_tls_use_os_certs")){
#اگر defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_bool(&token, "bridge_tls_use_os_certs", &cur_bridge->tls_use_os_certs, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "bridge_tls_version")){
#اگر defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_string(&token, "bridge_tls_version", &cur_bridge->tls_نسخہ, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "cafile")){
#اگر defined(WITH_TLS)
					REQUIRE_LISTENER(token);
					اگر(cur_listener->psk_hint){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						واپس MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->cafile);
					اگر(conf__parse_string(&token, "cafile", &cur_listener->cafile, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "capath")){
#اگر_تعریف_شدہ WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->capath);
					اگر(conf__parse_string(&token, "capath", &cur_listener->capath, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "certfile")){
#اگر_تعریف_شدہ WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					اگر(cur_listener->psk_hint){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						واپس MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->certfile);
					اگر(conf__parse_string(&token, "certfile", &cur_listener->certfile, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "check_retain_source")){
					اگر(conf__parse_bool(&token, "check_retain_source", &config->check_retain_ماخذ, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "ciphers")){
#اگر_تعریف_شدہ WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers);
					اگر(conf__parse_string(&token, "ciphers", &cur_listener->ciphers, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "ciphers_tls1.3")){
#اگر defined(WITH_TLS) && (!defined(LIBRESSL_نسخہ_نمبر) || LIBRESSL_نسخہ_نمبر > 0x3040000FL)
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers_tls13);
					اگر(conf__parse_string(&token, "ciphers_tls1.3", &cur_listener->ciphers_tls13, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: ciphers_tls1.3 support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "clientid") || !strcmp(token, "remote_clientid")){
#اگر_تعریف_شدہ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_string(&token, "bridge remote clientid", &cur_bridge->remote_clientid, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "cleansession")){
#اگر_تعریف_شدہ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_bool(&token, "cleansession", &cur_bridge->clean_آغاز, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "local_cleansession")){
#اگر_تعریف_شدہ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_bool(&token, "local_cleansession", (bool *)&cur_bridge->clean_آغاز_local, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "clientid_prefixes")){
					OPTION_DEPRECATED(token, "");
					mosquitto_FREE(config->clientid_prefixes);
					اگر(conf__parse_string(&token, "clientid_prefixes", &config->clientid_prefixes, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "connection")){
#اگر_تعریف_شدہ WITH_BRIDGE
					token = strtok_r(تہی, " ", &saveptr);
					REQUIRE_NON_خالی_خانے_OPTION(token, "connection");

					/* Check for existing bridge name. */
					برائے(i=0; i<config->bridge_گنتی; i++){
						اگر(!strcmp(config->bridges[i]->name, token)){
							log__printf(تہی, MOSQ_LOG_ERR, "Error: Duplicate bridge name '%s'.", token);
							واپس MOSQ_ERR_INVAL;
						}
					}

					ساخت mosquitto__bridge **bridges_new = mosquitto_realloc(config->bridges, (size_t)(config->bridge_گنتی+1)*حجم(ساخت mosquitto__bridge *));
					اگر(!bridges_new){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
						واپس MOSQ_ERR_NOMEM;
					}
					config->bridges = bridges_new;
					cur_bridge = mosquitto_malloc(حجم(ساخت mosquitto__bridge));
					اگر(!cur_bridge){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
						واپس MOSQ_ERR_NOMEM;
					}

					config->bridge_گنتی++;
					config->bridges[config->bridge_گنتی-1] = cur_bridge;

					memset(cur_bridge, 0, حجم(ساخت mosquitto__bridge));
					cur_bridge->name = mosquitto_strdup(token);
					اگر(!cur_bridge->name){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
						واپس MOSQ_ERR_NOMEM;
					}
					cur_bridge->keepalive = 60;
					cur_bridge->notifications = true;
					cur_bridge->notifications_local_only = false;
					cur_bridge->آغاز_type = bst_automatic;
					cur_bridge->idle_timeout = 60;
					cur_bridge->restart_timeout = 0;
					cur_bridge->backoff_بنیاد = 5 * 1000;
					cur_bridge->backoff_cap = 30 * 1000;
					cur_bridge->stable_connection_period = 0;
					cur_bridge->حد = 10;
					cur_bridge->try_نجی = true;
					cur_bridge->attempt_unsubscribe = true;
					cur_bridge->protocol_نسخہ = mosq_p_mqtt311;
					cur_bridge->primary_retry_sock = INVALID_SOCKET;
					cur_bridge->outgoing_retain = true;
					cur_bridge->clean_آغاز_local = -1;
					cur_bridge->reload_type = brt_lazy;
					cur_bridge->max_topic_alias = 10;
#اگر_تعریف_شدہ WITH_TCP_USER_TIMEOUT
					cur_bridge->tcp_user_timeout = -1;
#اگر_ختم
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "connection_messages")){
					اگر(conf__parse_bool(&token, token, &config->connection_messages, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "crlfile")){
#اگر_تعریف_شدہ WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->crlfile);
					اگر(conf__parse_string(&token, "crlfile", &cur_listener->crlfile, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "dhparamfile")){
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: dhparamfile is no longer required.");
				}ورنہ اگر(!strcmp(token, "disable_client_cert_date_checks")){
#اگر_تعریف_شدہ WITH_TLS
					REQUIRE_LISTENER(token);
					اگر(conf__parse_bool(&token, "disable_client_cert_date_checks", &cur_listener->disable_client_cert_date_checks, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "enable_control_api")){
#اگر_تعریف_شدہ WITH_CONTROL
					اگر(conf__parse_bool(&token, "enable_control_api", &config->enable_control_api, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: $CONTROL support not available (enable_control_api).");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "enable_proxy_protocol")){
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(تہی, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					واپس MOSQ_ERR_INVAL;
#اگر_ختم
					REQUIRE_LISTENER(token);
					اگر(conf__parse_int(&token, "enable_proxy_protocol", &cur_listener->enable_proxy_protocol, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(cur_listener->enable_proxy_protocol < 1 || cur_listener->enable_proxy_protocol > 2){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: enable_proxy_protocol must be 1 or 2.");
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "global_max_clients")){
					اگر(conf__parse_int(&token, "global_max_clients", &config->global_max_clients, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "global_max_connections")){
					اگر(conf__parse_int(&token, "global_max_connections", &config->global_max_connections, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "http_dir")){
#اگر defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
					اگر(reload){
						جاری;        /* Not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					اگر(conf__parse_string(&token, "http_dir", &cur_listener->http_dir, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#اگر_تعریف_شدہ WIN32
					حرف *http_dir_canonical = _fullpath(تہی, cur_listener->http_dir, 0);
					ثابت حرف sep = '\\';
#ورنہ
					حرف *http_dir_canonical = realpath(cur_listener->http_dir, تہی);
					ثابت حرف sep = '/';
#اگر_ختم
					اگر(!http_dir_canonical){
						واپس MOSQ_ERR_NOMEM;
					}
					size_t http_dir_len = لڑی_لمبائی(http_dir_canonical) + حجم(sep) + 1;
					حرف *http_dir_canonical_sep = mosquitto_calloc(http_dir_len, حجم(حرف));
					اگر(!http_dir_canonical_sep){
						آزاد(http_dir_canonical);
						واپس MOSQ_ERR_NOMEM;
					}
					snprintf(http_dir_canonical_sep, http_dir_len, "%s%c", http_dir_canonical, sep);
					آزاد(http_dir_canonical);
					mosquitto_FREE(cur_listener->http_dir);
					cur_listener->http_dir = http_dir_canonical_sep;
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: http_dir support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "idle_timeout")){
#اگر_تعریف_شدہ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_int(&token, "idle_timeout", &cur_bridge->idle_timeout, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(cur_bridge->idle_timeout < 1){
						log__printf(تہی, MOSQ_LOG_NOTICE, "idle_timeout interval too low, using 1 second.");
						cur_bridge->idle_timeout = 1;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "include_dir")){
					اگر(level == 0){
						حرف **files;
						صحیح file_گنتی;

						/* Only process include_dir from the main config file. */
						token = strtok_r(تہی, "", &saveptr);
						REQUIRE_NON_خالی_خانے_OPTION(token, "include_dir");

						rc = config__get_dir_files(token, &files, &file_گنتی);
						اگر(rc){
							واپس rc;
						}

						برائے(i=0; i<file_گنتی; i++){
							log__printf(تہی, MOSQ_LOG_INFO, "Loading config file '%s'", files[i]);

							rc = config__read_file(config, reload, files[i], cr, level+1, &lineno_ext);
							اگر(rc){
								اگر(lineno_ext > 0){
									log__printf(تہی, MOSQ_LOG_ERR, "Error found at '%s:%d'.", files[i], lineno_ext);
								}
								/* Free happens below */
								توڑو;
							}
						}
						برائے(i=0; i<file_گنتی; i++){
							mosquitto_FREE(files[i]);
						}
						mosquitto_FREE(files);
						اگر(rc){
							واپس rc;    /* This returns if config__read_file() fails above */
						}
					}ورنہ{
						log__printf(تہی, MOSQ_LOG_ERR, "Error: The include_dir option is only valid in the main configuration file.");
						واپس 1;
					}
				}ورنہ اگر(!strcmp(token, "keepalive_interval")){
#اگر_تعریف_شدہ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_int(&token, "keepalive_interval", &tmp_int, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(tmp_int > UINT16_MAX){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Bridge keepalive value too high.");
						واپس MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 5){
						log__printf(تہی, MOSQ_LOG_NOTICE, "keepalive interval too low, using 5 seconds.");
						tmp_int = 5;
					}
					cur_bridge->keepalive = (uint16_t)tmp_int;
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "keyfile")){
#اگر_تعریف_شدہ WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->keyfile);
					اگر(conf__parse_string(&token, "keyfile", &cur_listener->keyfile, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "listener")){
					config->local_only = false;

					اگر(conf__parse_int(&token, "listener port", &tmp_int, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#اگر_تعریف_شدہ WITH_UNIX_SOCKETS
					اگر(tmp_int < 0 || tmp_int > UINT16_MAX){
#ورنہ
					اگر(tmp_int < 1 || tmp_int > UINT16_MAX){
#اگر_ختم
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						واپس MOSQ_ERR_INVAL;
					}

					/* Look for bind address / unix socket path */
					token = strtok_r(تہی, " ", &saveptr);
					اگر(token != تہی && token[0] == '#'){
						token = تہی;
					}

					اگر(tmp_int == 0 && token == تہی){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: A listener with port 0 must provide a Unix socket path.");
						واپس MOSQ_ERR_INVAL;
					}

					اگر(reload){
						/* We reload listeners settings based on port number/unix socket path.
						 * If the port number/unix path doesn't already exist, exit with a complaint. */
						cur_listener = تہی;
#اگر_تعریف_شدہ WITH_UNIX_SOCKETS
						اگر(tmp_int == 0){
							برائے(i=0; i<config->listener_گنتی; i++){
								اگر(config->listeners[i].unix_socket_راستہ != تہی
										&& strcmp(config->listeners[i].unix_socket_راستہ, token) == 0){

									cur_listener = &config->listeners[i];
									توڑو;
								}
							}
						}ورنہ
#اگر_ختم
						{
							برائے(i=0; i<config->listener_گنتی; i++){
								اگر(config->listeners[i].پورٹ == tmp_int){
									/* Now check we have a matching bind address, if defined */
									اگر(config->listeners[i].host){
										اگر(token && !strcmp(config->listeners[i].host, token)){
											/* They both have a bind address, and they match */
											cur_listener = &config->listeners[i];
											توڑو;
										}
									}ورنہ{
										اگر(token == تہی){
											/* Neither this config nor the new config have a bind address,
											 * so they match. */
											cur_listener = &config->listeners[i];
											توڑو;
										}
									}
								}
							}
						}
						اگر(!cur_listener){
							log__printf(تہی, MOSQ_LOG_ERR, "Error: It is not currently possible to add/remove listeners when reloading the config file.");
							واپس MOSQ_ERR_INVAL;
						}
					}ورنہ{
						اگر(config_جوڑو_listener(config)){
							log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
							واپس MOSQ_ERR_NOMEM;
						}
						cur_listener = &config->listeners[config->listener_گنتی-1];
					}

					listener_سیٹ_defaults(cur_listener);
					cur_listener->پورٹ = (uint16_t)tmp_int;

					mosquitto_FREE(cur_listener->host);

#اگر_تعریف_شدہ WITH_UNIX_SOCKETS
					mosquitto_FREE(cur_listener->unix_socket_راستہ);
#اگر_ختم

					اگر(token){
#اگر_تعریف_شدہ WITH_UNIX_SOCKETS
						اگر(cur_listener->پورٹ == 0){
							cur_listener->unix_socket_راستہ = mosquitto_strdup(token);
						}ورنہ
#اگر_ختم
						{
							cur_listener->host = mosquitto_strdup(token);
						}
					}
				}ورنہ اگر(!strcmp(token, "listener_allow_anonymous")){
					REQUIRE_LISTENER(token);
					اگر(conf__parse_bool(&token, "listener_allow_anonymous", (bool *)&cur_listener->security_options->allow_anonymous, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "listener_auto_id_prefix")){
					REQUIRE_LISTENER(token);
					اگر(conf__parse_string(&token, "listener_auto_id_prefix", &cur_listener->security_options->auto_id_سابقہ, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(cur_listener->security_options->auto_id_سابقہ){
						اگر(لڑی_لمبائی(cur_listener->security_options->auto_id_سابقہ) > 50){
							log__printf(تہی, MOSQ_LOG_ERR, "Error: 'listener_auto_id_prefix' length must be <= 50.");
							واپس MOSQ_ERR_INVAL;
						}
						cur_listener->security_options->auto_id_سابقہ_len = (uint16_t)لڑی_لمبائی(cur_listener->security_options->auto_id_سابقہ);
					}ورنہ{
						cur_listener->security_options->auto_id_سابقہ_len = 0;
					}
				}ورنہ اگر(!strcmp(token, "local_clientid")){
#اگر_تعریف_شدہ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_string(&token, "bridge local clientd", &cur_bridge->local_clientid, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "local_password")){
#اگر_تعریف_شدہ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_string(&token, "bridge local_password", &cur_bridge->local_password, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "local_username")){
#اگر_تعریف_شدہ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_string(&token, "bridge local_username", &cur_bridge->local_username, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "log_dest")){
					token = strtok_r(تہی, " ", &saveptr);
					REQUIRE_NON_خالی_خانے_OPTION(token, "log_dest");

					cr->log_dest_سیٹ = 1;
					اگر(!strcmp(token, "none")){
						cr->log_dest = MQTT3_LOG_NONE;
					}ورنہ اگر(!strcmp(token, "syslog")){
						cr->log_dest |= MQTT3_LOG_SYSLOG;
					}ورنہ اگر(!strcmp(token, "stdout")){
						cr->log_dest |= MQTT3_LOG_STDOUT;
					}ورنہ اگر(!strcmp(token, "stderr")){
						cr->log_dest |= MQTT3_LOG_STDERR;
					}ورنہ اگر(!strcmp(token, "topic")){
						cr->log_dest |= MQTT3_LOG_TOPIC;
					}ورنہ اگر(!strcmp(token, "dlt")){
						cr->log_dest |= MQTT3_LOG_DLT;
#اگر_تعریف_شدہ ANDROID
					}ورنہ اگر(!strcmp(token, "android")){
						cr->log_dest |= MQTT3_LOG_ANDROID;
#اگر_ختم
					}ورنہ اگر(!strcmp(token, "file")){
						اگر(config->log_fptr || config->log_file){
							log__printf(تہی, MOSQ_LOG_ERR, "Error: Duplicate \"log_dest file\" value.");
							واپس MOSQ_ERR_INVAL;
						}
						/* Get remaining string. */
						token = saveptr;
						اگر(token && token[0]){
							جب(token[0] == ' ' || token[0] == '\t'){
								token++;
							}
						}
						/* Duplicate "token" check here saves a log__printf() */
						اگر(token && token[0]){
							config->log_file = mosquitto_strdup(token);
							اگر(!config->log_file){
								log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
								واپس MOSQ_ERR_NOMEM;
							}
							cr->log_dest |= MQTT3_LOG_FILE;
						}ورنہ{
							log__printf(تہی, MOSQ_LOG_ERR, "Error: Empty \"log_dest file\" value in configuration.");
							واپس MOSQ_ERR_INVAL;
						}
						cr->log_dest |= MQTT3_LOG_FILE;
					}ورنہ{
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid 'log_dest' value (%s).", token);
						واپس MOSQ_ERR_INVAL;
					}
#اگر defined(WIN32) || defined(__CYGWIN__)
					اگر(service_handle){
						اگر(cr->log_dest == MQTT3_LOG_STDOUT || cr->log_dest == MQTT3_LOG_STDERR){
							log__printf(تہی, MOSQ_LOG_ERR, "Error: Cannot log to stdout/stderr when running as a Windows service.");
							واپس MOSQ_ERR_INVAL;
						}
					}
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "log_facility")){
#اگر defined(WIN32) || defined(__CYGWIN__)
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: log_facility not supported on Windows.");
#ورنہ
					اگر(conf__parse_int(&token, "log_facility", &tmp_int, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					انتخاب(tmp_int){
						حالت 0:
							config->log_facility = LOG_LOCAL0;
							توڑو;
						حالت 1:
							config->log_facility = LOG_LOCAL1;
							توڑو;
						حالت 2:
							config->log_facility = LOG_LOCAL2;
							توڑو;
						حالت 3:
							config->log_facility = LOG_LOCAL3;
							توڑو;
						حالت 4:
							config->log_facility = LOG_LOCAL4;
							توڑو;
						حالت 5:
							config->log_facility = LOG_LOCAL5;
							توڑو;
						حالت 6:
							config->log_facility = LOG_LOCAL6;
							توڑو;
						حالت 7:
							config->log_facility = LOG_LOCAL7;
							توڑو;
						طےشدہ:
							log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid 'log_facility' value (%d).", tmp_int);
							واپس MOSQ_ERR_INVAL;
					}
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "log_timestamp")){
					اگر(conf__parse_bool(&token, token, &config->log_timestamp, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "log_timestamp_format")){
					اگر(conf__parse_string(&token, token, &config->log_timestamp_format, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "log_type")){
					token = strtok_r(تہی, " ", &saveptr);
					REQUIRE_NON_خالی_خانے_OPTION(token, "log_type");

					cr->log_type_سیٹ = 1;
					اگر(!strcmp(token, "none")){
						cr->log_type = MOSQ_LOG_NONE;
					}ورنہ اگر(!strcmp(token, "information")){
						cr->log_type |= MOSQ_LOG_INFO;
					}ورنہ اگر(!strcmp(token, "notice")){
						cr->log_type |= MOSQ_LOG_NOTICE;
					}ورنہ اگر(!strcmp(token, "warning")){
						cr->log_type |= MOSQ_LOG_WARNING;
					}ورنہ اگر(!strcmp(token, "error")){
						cr->log_type |= MOSQ_LOG_ERR;
					}ورنہ اگر(!strcmp(token, "debug")){
						cr->log_type |= MOSQ_LOG_DEBUG;
					}ورنہ اگر(!strcmp(token, "subscribe")){
						cr->log_type |= MOSQ_LOG_SUBSCRIBE;
					}ورنہ اگر(!strcmp(token, "unsubscribe")){
						cr->log_type |= MOSQ_LOG_UNSUBSCRIBE;
					}ورنہ اگر(!strcmp(token, "internal")){
						cr->log_type |= MOSQ_LOG_اندرونی;
#اگر_تعریف_شدہ WITH_WEBSOCKETS
					}ورنہ اگر(!strcmp(token, "websockets")){
						cr->log_type |= MOSQ_LOG_WEBSOCKETS;
#اگر_ختم
					}ورنہ اگر(!strcmp(token, "all")){
						cr->log_type = MOSQ_LOG_ALL;
					}ورنہ{
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid 'log_type' value (%s).", token);
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "max_connections")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					اگر(conf__parse_int(&token, token, &cur_listener->max_connections, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(cur_listener->max_connections < 0){
						cur_listener->max_connections = -1;
					}
				}ورنہ اگر(!strcmp(token, "maximum_qos") || !strcmp(token, "max_qos")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					اگر(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 0 || tmp_int > 2){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: 'max_qos' must be between 0 and 2 inclusive.");
						واپس MOSQ_ERR_INVAL;
					}
					cur_listener->max_qos = (uint8_t)tmp_int;
				}ورنہ اگر(!strcmp(token, "max_inflight_bytes")){
					اگر(conf__parse_int(&token, "max_inflight_bytes", &tmp_int, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_inflight_بائٹیں = (size_t)tmp_int;
				}ورنہ اگر(!strcmp(token, "max_inflight_messages")){
					اگر(conf__parse_int(&token, "max_inflight_messages", &tmp_int, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 0 || tmp_int == UINT16_MAX){
						tmp_int = 0;
					}ورنہ اگر(tmp_int > UINT16_MAX){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: 'max_inflight_messages' must be <= 65535.");
						واپس MOSQ_ERR_INVAL;
					}
					config->max_inflight_messages = (uint16_t)tmp_int;
				}ورنہ اگر(!strcmp(token, "max_keepalive")){
					اگر(conf__parse_int(&token, "max_keepalive", &tmp_int, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid 'max_keepalive' value (%d).", tmp_int);
						واپس MOSQ_ERR_INVAL;
					}
					config->max_keepalive = (uint16_t)tmp_int;
				}ورنہ اگر(!strcmp(token, "max_packet_size")){
					اگر(conf__parse_int(&token, "max_packet_size", &tmp_int, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 20){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: 'max_packet_size' must be >= 20.");
						واپس MOSQ_ERR_INVAL;
					}
					config->max_packet_سائز = (uint32_t)tmp_int;
				}ورنہ اگر(!strcmp(token, "max_queued_bytes")){
					اگر(conf__parse_int(&token, "max_queued_bytes", &tmp_int, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_بائٹیں = (size_t)tmp_int;
				}ورنہ اگر(!strcmp(token, "max_queued_messages")){
					اگر(conf__parse_int(&token, "max_queued_messages", &tmp_int, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_messages = tmp_int;
				}ورنہ اگر(!strcmp(token, "memory_limit")){
					ssize_t lim;
					اگر(conf__parse_ssize_t(&token, "memory_limit", &lim, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(lim < 0){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid 'memory_limit' value (%ld).", lim);
						واپس MOSQ_ERR_INVAL;
					}
					mosquitto_حافظہ_سیٹ_حد_بندی((size_t)lim);
				}ورنہ اگر(!strcmp(token, "message_size_limit")){
					log__printf(تہی, MOSQ_LOG_NOTICE, "Note: It is recommended to replace `message_size_limit` with `max_packet_size`.");
					اگر(conf__parse_int(&token, "message_size_limit", (صحیح *)&config->پیغام_سائز_حد_بندی, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(config->پیغام_سائز_حد_بندی > MQTT_MAX_PAYLOAD){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid 'message_size_limit' value (%u).", config->پیغام_سائز_حد_بندی);
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "mount_point")){
					REQUIRE_LISTENER(token);
					mosquitto_FREE(cur_listener->mount_نقطہ);
					اگر(conf__parse_string(&token, "mount_point", &cur_listener->mount_نقطہ, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(mosquitto_pub_topic_check(cur_listener->mount_نقطہ) != MOSQ_ERR_SUCCESS){
						log__printf(تہی, MOSQ_LOG_ERR,
								"Error: Invalid 'mount_point' value (%s). Does it contain a wildcard character?",
								cur_listener->mount_نقطہ);
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "notifications")){
#اگر_تعریف_شدہ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_bool(&token, "notifications", &cur_bridge->notifications, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "notifications_local_only")){
#اگر_تعریف_شدہ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_bool(&token, "notifications_local_only", &cur_bridge->notifications_local_only, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "notification_topic")){
#اگر_تعریف_شدہ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_string(&token, "notification_topic", &cur_bridge->notification_topic, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "password") || !strcmp(token, "remote_password")){
#اگر_تعریف_شدہ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_string(&token, "bridge remote_password", &cur_bridge->remote_password, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "password_file")){
					REQUIRE_LISTENER_IF_فی_LISTENER(token);
					conf_سیٹ_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->password_ڈیٹا.password_file);
					اگر(conf__parse_string(&token, "password_file", &cur_security_options->password_ڈیٹا.password_file, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "per_listener_settings")){
					OPTION_DEPRECATED(token, "Please see the documentation for how to achieve the same effect.");
					اگر(config->فی_listener_settings){
						/* Once this is set, don't let it be unset. It should be the first config option ideally. */
						جاری;
					}
					اگر(conf__parse_bool(&token, "per_listener_settings", &config->فی_listener_settings, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(cur_security_options && config->فی_listener_settings){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: per_listener_settings must be set before any other security settings.");
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "persistence") || !strcmp(token, "retained_persistence")){
					اگر(conf__parse_bool(&token, token, &config->persistence, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "persistence_file")){
					اگر(conf__parse_string(&token, "persistence_file", &config->persistence_file, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "persistence_location")){
					اگر(conf__parse_string(&token, "persistence_location", &config->persistence_location, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "persistent_client_expiration")){
					token = strtok_r(تہی, " ", &saveptr);
					REQUIRE_NON_خالی_خانے_OPTION(token, "persistent_client_expiration");

					time_t expiration_mult;

					انتخاب(token[لڑی_لمبائی(token)-1]){
						حالت 's':
							expiration_mult = 1;
							توڑو;
						حالت 'h':
							expiration_mult = 3600;
							توڑو;
						حالت 'd':
							expiration_mult = 86400;
							توڑو;
						حالت 'w':
							expiration_mult = 86400*7;
							توڑو;
						حالت 'm':
							expiration_mult = 86400*30;
							توڑو;
						حالت 'y':
							expiration_mult = 86400*365;
							توڑو;
						طےشدہ:
							log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
							واپس MOSQ_ERR_INVAL;
					}
					token[لڑی_لمبائی(token)-1] = '\0';
					config->persistent_client_expiration = atoi(token)*expiration_mult;
					اگر(config->persistent_client_expiration <= 0){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "pid_file")){
					اگر(reload){
						جاری;        /* pid file not valid for reloading. */
					}
					اگر(conf__parse_string(&token, "pid_file", &config->pid_file, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "port")){
					OPTION_DEPRECATED(token, "Please use 'listener' instead.");
					config->local_only = false;
					اگر(reload){
						جاری;        /* Listeners not valid for reloading. */

					}
					اگر(config__create_default_listener(config, token)){
						واپس MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					اگر(config->default_listener->پورٹ){
						log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Default listener port specified multiple times. Only the latest will be used.");
					}
					اگر(conf__parse_int(&token, "port", &tmp_int, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 1 || tmp_int > UINT16_MAX){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						واپس MOSQ_ERR_INVAL;
					}
					config->default_listener->پورٹ = (uint16_t)tmp_int;
				}ورنہ اگر(!strcmp(token, "protocol")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(تہی, " ", &saveptr);
					REQUIRE_NON_خالی_خانے_OPTION(token, "protocol");

					اگر(!strcmp(token, "mqtt")){
						cur_listener->protocol = mp_mqtt;
						/*
						}else if(!strcmp(token, "mqttsn")){
						    cur_listener->protocol = mp_mqttsn;
						*/
					}ورنہ اگر(!strcmp(token, "websockets")){
#اگر_تعریف_شدہ WITH_WEBSOCKETS
						cur_listener->protocol = mp_websockets;
#ورنہ
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Websockets support not available.");
						واپس MOSQ_ERR_INVAL;
#اگر_ختم
					}ورنہ اگر(!strcmp(token, "http_api")){
#اگر_تعریف_شدہ WITH_HTTP_API
						cur_listener->protocol = mp_http_api;
#ورنہ
						log__printf(تہی, MOSQ_LOG_ERR, "Error: HTTP API support not available.");
						واپس MOSQ_ERR_INVAL;
#اگر_ختم
					}ورنہ{
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid 'protocol' value (%s).", token);
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "proxy_protocol_v2_require_tls")){
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(تہی, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					واپس MOSQ_ERR_INVAL;
#اگر_ختم
					REQUIRE_LISTENER(token);
					اگر(conf__parse_bool(&token, "proxy_protocol_v2_require_tls", &cur_listener->proxy_protocol_v2_require_tls, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "psk_file")){
#اگر_تعریف_شدہ FINAL_WITH_TLS_PSK
					REQUIRE_LISTENER_IF_فی_LISTENER(token);
					conf_سیٹ_cur_security_options(config, &cur_listener, &cur_security_options, token);
					اگر(cur_listener && cur_listener->certfile){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						واپس MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_security_options->psk_file);
					اگر(conf__parse_string(&token, "psk_file", &cur_security_options->psk_file, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "psk_hint")){
#اگر_تعریف_شدہ FINAL_WITH_TLS_PSK
					اگر(reload){
						جاری;        /* PSK file not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					اگر(conf__parse_string(&token, "psk_hint", &cur_listener->psk_hint, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "queue_qos0_messages")){
					اگر(conf__parse_bool(&token, token, &config->قطار_qos0_messages, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "require_certificate")){
#اگر_تعریف_شدہ WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					اگر(conf__parse_bool(&token, "require_certificate", &cur_listener->require_certificate, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "restart_timeout")){
#اگر_تعریف_شدہ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					cur_bridge->backoff_cap = 0; /* set backoff to constant mode, unless cap is specified further down */
					token = strtok_r(تہی, " ", &saveptr);
					REQUIRE_NON_خالی_خانے_OPTION(token, "restart_timeout");

					cur_bridge->restart_timeout = atoi(token);
					cur_bridge->backoff_بنیاد = 0;
					cur_bridge->backoff_cap = 0;
					اگر(cur_bridge->restart_timeout < 1){
						log__printf(تہی, MOSQ_LOG_NOTICE, "restart_timeout interval too low, using 1 second.");
						cur_bridge->restart_timeout = 1;
					}ورنہ اگر(cur_bridge->restart_timeout > 3600){
						log__printf(تہی, MOSQ_LOG_NOTICE, "restart_timeout interval too high, using 3600 seconds.");
						cur_bridge->restart_timeout = 3600;
					}
					token = strtok_r(تہی, " ", &saveptr);
					اگر(token){
						cur_bridge->backoff_بنیاد = cur_bridge->restart_timeout;
						cur_bridge->backoff_cap = atoi(token);
						اگر(cur_bridge->backoff_cap < cur_bridge->backoff_بنیاد){
							log__printf(تہی, MOSQ_LOG_ERR, "Error: backoff cap is lower than the base in restart_timeout.");
							واپس MOSQ_ERR_INVAL;
						}ورنہ اگر(cur_bridge->backoff_cap > 7200){
							log__printf(تہی, MOSQ_LOG_ERR, "Error: backoff cap too high, using 7200 seconds.");
							cur_bridge->backoff_cap = 7200;
						}

						token = strtok_r(تہی, " ", &saveptr);
						اگر(token){
							cur_bridge->stable_connection_period = atoi(token);
							اگر(cur_bridge->stable_connection_period < 0){
								log__printf(تہی, MOSQ_LOG_ERR, "Error: stable connection period cannot be negative.");
								واپس MOSQ_ERR_INVAL;
							}
						}
					}
					cur_bridge->restart_timeout *= 1000; /* backoff is tracked in ms */
					cur_bridge->backoff_بنیاد *= 1000;
					cur_bridge->backoff_cap *= 1000;
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "retain_available")){
					اگر(conf__parse_bool(&token, token, &config->retain_دستیاب, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "retain_expiry_interval")){
					اگر(conf__parse_int(&token, token, &config->retain_expiry_interval, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(config->retain_expiry_interval < 1){
						log__printf(تہی, MOSQ_LOG_WARNING, "Error: retain_expiry_interval must be >= 1.");
						واپس MOSQ_ERR_INVAL;
					}ورنہ اگر(config->retain_expiry_interval > 10000000){
						log__printf(تہی, MOSQ_LOG_WARNING, "Warning: retain_expiry_interval being capped at 19 years.");
						config->retain_expiry_interval = 10000000;
					}
					config->retain_expiry_interval *= 60;
				}ورنہ اگر(!strcmp(token, "retry_interval")){
					OPTION_UNAVAILABLE(token);
				}ورنہ اگر(!strcmp(token, "round_robin")){
#اگر_تعریف_شدہ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_bool(&token, "round_robin", &cur_bridge->باری_robin, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "set_tcp_nodelay")){
					اگر(conf__parse_bool(&token, "set_tcp_nodelay", &config->سیٹ_tcp_nodelay, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "start_type")){
#اگر_تعریف_شدہ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(تہی, " ", &saveptr);
					REQUIRE_NON_خالی_خانے_OPTION(token, "start_type");

					اگر(!strcmp(token, "automatic")){
						cur_bridge->آغاز_type = bst_automatic;
					}ورنہ اگر(!strcmp(token, "lazy")){
						cur_bridge->آغاز_type = bst_lazy;
					}ورنہ اگر(!strcmp(token, "manual")){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Manual start_type not supported.");
						واپس MOSQ_ERR_INVAL;
					}ورنہ اگر(!strcmp(token, "once")){
						cur_bridge->آغاز_type = bst_once;
					}ورنہ{
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid 'start_type' value in configuration (%s).", token);
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "socket_domain")){
					اگر(reload){
						جاری;        /* socket_domain not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(تہی, " ", &saveptr);
					REQUIRE_NON_خالی_خانے_OPTION(token, "start_type");

					اگر(!strcmp(token, "ipv4")){
						cur_listener->socket_domain = AF_INET;
					}ورنہ اگر(!strcmp(token, "ipv6")){
						cur_listener->socket_domain = AF_INET6;
					}ورنہ{
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid 'socket_domain' value '%s' in configuration.", token);
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "sys_interval")){
					اگر(conf__parse_int(&token, "sys_interval", &config->sys_interval, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(config->sys_interval < 0 || config->sys_interval > 65535){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid 'sys_interval' value (%d).", config->sys_interval);
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "threshold")){
#اگر_تعریف_شدہ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_int(&token, "threshold", &cur_bridge->حد, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(cur_bridge->حد < 1){
						log__printf(تہی, MOSQ_LOG_NOTICE, "threshold too low, using 1 message.");
						cur_bridge->حد = 1;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "tls_engine")){
#اگر_تعریف_شدہ WITH_TLS
					اگر(reload){
						جاری;        /* tls_engine not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					اگر(conf__parse_string(&token, "tls_engine", &cur_listener->tls_engine, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "tls_engine_kpass_sha1")){
#اگر_تعریف_شدہ WITH_TLS
					اگر(reload){
						جاری;        /* tls_engine not valid for reloading. */

					}
					حرف *kpass_sha = تہی, *kpass_sha_bin = تہی;
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					اگر(conf__parse_string(&token, "tls_engine_kpass_sha1", &kpass_sha, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(mosquitto__hex2bin_sha1(kpass_sha, (غیرعلامتی حرف **)&kpass_sha_bin) != MOSQ_ERR_SUCCESS){
						mosquitto_FREE(kpass_sha);
						واپس MOSQ_ERR_INVAL;
					}
					mosquitto_free(cur_listener->tls_engine_kpass_sha1);
					cur_listener->tls_engine_kpass_sha1 = kpass_sha_bin;
					mosquitto_FREE(kpass_sha);
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "tls_keyform")){
#اگر_تعریف_شدہ WITH_TLS
					اگر(reload){
						جاری;        /* tls_engine not valid for reloading. */

					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					حرف *keyform = تہی;
					اگر(conf__parse_string(&token, "tls_keyform", &keyform, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					cur_listener->tls_keyform = mosq_k_pem;
					اگر(!strcmp(keyform, "engine")){
						cur_listener->tls_keyform = mosq_k_engine;
					}
					mosquitto_FREE(keyform);
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "tls_version")){
#اگر defined(WITH_TLS)
					اگر(reload){
						جاری;        /* tls_version not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					اگر(conf__parse_string(&token, "tls_version", &cur_listener->tls_نسخہ, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "topic")){
#اگر_تعریف_شدہ WITH_BRIDGE
					حرف *topic = تہی;
					شمار mosquitto__bridge_direction direction = bd_برآمد;
					uint8_t qos = 0;
					حرف *local_سابقہ = تہی, *remote_سابقہ = تہی;

					REQUIRE_BRIDGE(token);

					token = strtok_r(تہی, " ", &saveptr);
					REQUIRE_NON_خالی_خانے_OPTION(token, "topic");

					// Check if the topic is quoted (e.g. for spaces within topic names), but not the
					// special case of ""
					اگر(token[0] == '"' && token [1] != '"'){
						اگر(strchr(saveptr, '"') == تہی){
							log__printf(تہی, MOSQ_LOG_ERR, "Error: Missing closing quote in topic value (%s).", saveptr);
							واپس MOSQ_ERR_INVAL;
						}

						حرف *topic_اندر_quotes = strtok_r(تہی, "\"", &saveptr);
						size_t tlen = 1;
						اگر(topic_اندر_quotes){
							tlen = لڑی_لمبائی(topic_اندر_quotes);
						}

						topic = mosquitto_malloc(لڑی_لمبائی(token) + tlen + 1);
						اگر(!topic){
							log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
							واپس MOSQ_ERR_NOMEM;
						}

						strcpy(topic, token + 1);
						strcat(topic, " ");
						اگر(topic_اندر_quotes){
							strcat(topic, topic_اندر_quotes);
						}
					}ورنہ{
						topic = mosquitto_strdup(token);
						اگر(!topic){
							log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
							واپس MOSQ_ERR_NOMEM;
						}
					}

					token = strtok_r(تہی, " ", &saveptr);
					اگر(token){
						اگر(!strcasecmp(token, "out")){
							direction = bd_برآمد;
						}ورنہ اگر(!strcasecmp(token, "in")){
							direction = bd_اندر;
						}ورنہ اگر(!strcasecmp(token, "both")){
							direction = bd_both;
						}ورنہ{
							log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid bridge topic direction '%s'.", token);
							mosquitto_FREE(topic);
							واپس MOSQ_ERR_INVAL;
						}
						token = strtok_r(تہی, " ", &saveptr);
						اگر(token){
							اگر(token[0] == '#'){
								(خالی)strtok_r(تہی, "", &saveptr);
							}
							qos = (uint8_t)atoi(token);
							اگر(qos > 2){
								log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid bridge QoS level '%s'.", token);
								mosquitto_FREE(topic);
								واپس MOSQ_ERR_INVAL;
							}

							token = strtok_r(تہی, " ", &saveptr);
							اگر(token){
								اگر(!strcmp(token, "\"\"") || token[0] == '#'){
									local_سابقہ = تہی;
									اگر(token[0] == '#'){
										(خالی)strtok_r(تہی, "", &saveptr);
									}
								}ورنہ{
									local_سابقہ = token;
								}

								token = strtok_r(تہی, " ", &saveptr);
								اگر(token){
									اگر(!strcmp(token, "\"\"") || token[0] == '#'){
										remote_سابقہ = تہی;
									}ورنہ{
										remote_سابقہ = token;
									}
								}
							}
						}
					}

					اگر(bridge_جوڑو_topic(cur_bridge, topic, direction, qos, local_سابقہ, remote_سابقہ)){
						mosquitto_FREE(topic);
						واپس MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(topic);
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "max_topic_alias")){
					REQUIRE_LISTENER(token);
					اگر(conf__parse_int(&token, "max_topic_alias", &tmp_int, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias' value in configuration.");
						واپس MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias = (uint16_t)tmp_int;
				}ورنہ اگر(!strcmp(token, "max_topic_alias_broker")){
					REQUIRE_LISTENER(token);
					اگر(conf__parse_int(&token, "max_topic_alias_broker", &tmp_int, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias_broker' value in configuration.");
						واپس MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias_broker = (uint16_t)tmp_int;
				}ورنہ اگر(!strcmp(token, "try_private")){
#اگر_تعریف_شدہ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_bool(&token, "try_private", &cur_bridge->try_نجی, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "upgrade_outgoing_qos")){
					اگر(conf__parse_bool(&token, token, &config->upgrade_outgoing_qos, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "use_identity_as_username")){
#اگر_تعریف_شدہ WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					اگر(conf__parse_bool(&token, "use_identity_as_username", &cur_listener->use_شناختی_عنصر_as_username, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "use_subject_as_username")){
#اگر_تعریف_شدہ WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					اگر(conf__parse_bool(&token, "use_subject_as_username", &cur_listener->use_subject_as_username, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "user")){
					اگر(reload){
						جاری;        /* Drop privileges user not valid for reloading. */
					}
					mosquitto_FREE(config->user);
					اگر(conf__parse_string(&token, "user", &config->user, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "use_username_as_clientid")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					اگر(conf__parse_bool(&token, "use_username_as_clientid", &cur_listener->use_username_as_clientid, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
				}ورنہ اگر(!strcmp(token, "username") || !strcmp(token, "remote_username")){
#اگر_تعریف_شدہ WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					اگر(conf__parse_string(&token, "bridge remote_username", &cur_bridge->remote_username, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "websockets_log_level")){
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					اگر(conf__parse_int(&token, "websockets_log_level", &config->websockets_log_level, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
#اگر_ختم
				}ورنہ اگر(!strcmp(token, "websockets_headers_size") || !strcmp(token, "packet_buffer_size")){
					اگر(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					اگر(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(تہی, MOSQ_LOG_WARNING, "Error: Packet buffer size must be between 0 and 65535 inclusive.");
						واپس MOSQ_ERR_INVAL;
					}
					config->packet_بفر_سائز = (uint16_t)tmp_int;
				}ورنہ اگر(!strcmp(token, "websockets_origin")){
#اگر_تعریف_شدہ WITH_WEBSOCKETS
#  اگر LWS_LIBRARY_نسخہ_نمبر >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN
					REQUIRE_LISTENER(token);
					ws_origins = mosquitto_realloc(cur_listener->ws_origins, حجم(حرف *)*(size_t)(cur_listener->ws_origin_گنتی+1));
					اگر(ws_origins == تہی){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
						واپس MOSQ_ERR_NOMEM;
					}
					ws_origins[cur_listener->ws_origin_گنتی] = تہی;
					cur_listener->ws_origins = ws_origins;
					اگر(conf__parse_string(&token, "websockets_origin", &ws_origins[cur_listener->ws_origin_گنتی], &saveptr)){
						واپس MOSQ_ERR_INVAL;
					}
					cur_listener->ws_origin_گنتی++;
#  ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: websockets_origin support not available, libwebsockets version is too old.");
#  endif
#ورنہ
					log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Websockets support not available.");
#اگر_ختم
				}ورنہ{
					log__printf(تہی, MOSQ_LOG_ERR, "Error: Unknown configuration variable '%s'.", token);
					واپس MOSQ_ERR_INVAL;
				}
			}
		}
	}
	واپس MOSQ_ERR_SUCCESS;
}


صحیح config__read_file(ساخت mosquitto__config *config, bool reload, ثابت حرف *file, ساخت config_recurse *cr, صحیح level, صحیح *lineno)
{
	صحیح rc;
	FILE *fptr = تہی;
	حرف *buf;
	صحیح buflen;
#اگر_غیر_تعریف_شدہ WIN32
	DIR *dir;
#اگر_ختم

#اگر_غیر_تعریف_شدہ WIN32
	dir = opendir(file);
	اگر(dir){
		closedir(dir);
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Config file '%s' is a directory.", file);
		واپس 1;
	}
#اگر_ختم

	fptr = mosquitto_fopen(file, "rt", false);
	اگر(!fptr){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to open config file '%s'.", file);
		واپس 1;
	}

	buflen = 1000;
	buf = mosquitto_malloc((size_t)buflen);
	اگر(!buf){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
		fclose(fptr);
		واپس MOSQ_ERR_NOMEM;
	}

	rc = config__read_file_core(config, reload, cr, level, lineno, fptr, &buf, &buflen);
	mosquitto_FREE(buf);
	fclose(fptr);

	واپس rc;
}


ساکن صحیح config__check_proxy(ساخت mosquitto__config *config)
{
	برائے(صحیح i=0; i<config->listener_گنتی; i++){
		ساخت mosquitto__listener *l = &config->listeners[i];

		اگر(l->enable_proxy_protocol == 2){
#اگر_تعریف_شدہ WITH_TLS
			اگر(l->use_subject_as_username){
				log__printf(تہی, MOSQ_LOG_ERR, "Error: use_subject_as_username cannot be used with `enable_proxy_protocol 2`.");
				واپس MOSQ_ERR_INVAL;
			}

			اگر(l->certfile || l->keyfile){
				log__printf(تہی, MOSQ_LOG_ERR, "Error: certfile and keyfile cannot be used with `enable_proxy_protocol 2`.");
				واپس MOSQ_ERR_INVAL;
			}
#اگر_ختم
		}
	}

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح config__check(ساخت mosquitto__config *config)
{
	/* Checks that are easy to make after the config has been loaded. */

	ثابت حرف *id_سابقہ;
	صحیح id_سابقہ_len;
	اگر(config->security_options.auto_id_سابقہ){
		id_سابقہ = config->security_options.auto_id_سابقہ;
		id_سابقہ_len = config->security_options.auto_id_سابقہ_len;
	}ورنہ{
		id_سابقہ = "auto-";
		id_سابقہ_len = (صحیح)لڑی_لمبائی("auto-");
	}

	/* Default to auto_id_prefix = 'auto-' if none set. */
	برائے(صحیح i=0; i<config->listener_گنتی; i++){
		اگر(!config->listeners[i].security_options->auto_id_سابقہ){
			config->listeners[i].security_options->auto_id_سابقہ = mosquitto_strdup(id_سابقہ);
			اگر(!config->listeners[i].security_options->auto_id_سابقہ){
				واپس MOSQ_ERR_NOMEM;
			}
			config->listeners[i].security_options->auto_id_سابقہ_len = (uint16_t)id_سابقہ_len;
		}
	}

	واپس config__check_proxy(config);
}

#اگر_تعریف_شدہ WITH_BRIDGE


ساکن صحیح config__check_bridges(ساخت mosquitto__config *config)
{
	ساخت mosquitto__bridge *bridge1, *bridge2;
	حرف hostname[256];
	size_t len;

	/* Check for bridge duplicate local_clientid, need to generate missing IDs
	 * first. */
	برائے(صحیح i=0; i<config->bridge_گنتی; i++){
		bridge1 = config->bridges[i];

		اگر(!bridge1->remote_clientid){
			اگر(!gethostname(hostname, 256)){
				len = لڑی_لمبائی(hostname) + لڑی_لمبائی(bridge1->name) + 2;
				bridge1->remote_clientid = mosquitto_malloc(len);
				اگر(!bridge1->remote_clientid){
					واپس MOSQ_ERR_NOMEM;
				}
				snprintf(bridge1->remote_clientid, len, "%s.%s", hostname, bridge1->name);
			}ورنہ{
				واپس 1;
			}
		}

		اگر(!bridge1->local_clientid){
			len = لڑی_لمبائی(bridge1->remote_clientid) + لڑی_لمبائی("local.") + 2;
			bridge1->local_clientid = mosquitto_malloc(len);
			اگر(!bridge1->local_clientid){
				log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
				واپس MOSQ_ERR_NOMEM;
			}
			snprintf(bridge1->local_clientid, len, "local.%s", bridge1->remote_clientid);
		}
	}

	برائے(صحیح i=0; i<config->bridge_گنتی; i++){
		bridge1 = config->bridges[i];
		برائے(صحیح j=i+1; j<config->bridge_گنتی; j++){
			bridge2 = config->bridges[j];
			اگر(!strcmp(bridge1->local_clientid, bridge2->local_clientid)){
				log__printf(تہی, MOSQ_LOG_ERR, "Error: Bridge local_clientid "
						"'%s' is not unique. Try changing or setting the "
						"local_clientid value for one of the bridges.",
						bridge1->local_clientid);
				واپس MOSQ_ERR_INVAL;
			}
		}
	}

#اگر_تعریف_شدہ WITH_TLS
	/* Check for missing TLS cafile/capath/certfile/keyfile */
	برائے(صحیح i=0; i<config->listener_گنتی; i++){
		bool cafile = !!config->listeners[i].cafile;
		bool capath = !!config->listeners[i].capath;
		bool certfile = !!config->listeners[i].certfile;
		bool keyfile = !!config->listeners[i].keyfile;

		اگر((certfile && !keyfile) || (!certfile && keyfile)){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Both certfile and keyfile must be provided to enable a TLS listener.");
			واپس MOSQ_ERR_INVAL;
		}
		اگر(cafile && !certfile){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: cafile specified without certfile and keyfile.");
			واپس MOSQ_ERR_INVAL;
		}
		اگر(capath && !certfile){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: capath specified without certfile and keyfile.");
			واپس MOSQ_ERR_INVAL;
		}
	}
#اگر_ختم
	واپس MOSQ_ERR_SUCCESS;
}
#اگر_ختم


ساکن صحیح conf__parse_bool(حرف **token, ثابت حرف *name, bool *قدر, حرف **saveptr)
{
	*token = strtok_r(تہی, " ", saveptr);
	اگر(*token){
		اگر(!strcmp(*token, "false") || !strcmp(*token, "0")){
			*قدر = false;
		}ورنہ اگر(!strcmp(*token, "true") || !strcmp(*token, "1")){
			*قدر = true;
		}ورنہ{
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Invalid '%s' value (%s).", name, *token);
			واپس MOSQ_ERR_INVAL;
		}
	}ورنہ{
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		واپس MOSQ_ERR_INVAL;
	}

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح conf__parse_int(حرف **token, ثابت حرف *name, صحیح *قدر, حرف **saveptr)
{
	*token = strtok_r(تہی, " ", saveptr);
	اگر(*token){
		حرف *endptr = تہی;
		طویل l = strtol(*token, &endptr, 0);
		اگر(endptr == *token || endptr[0] != '\0'){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: '%s' value not a number.", name);
			واپس MOSQ_ERR_INVAL;
		}
		اگر(l > زیادہ_سے_زیادہ_عدد || l < INT_MIN){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: '%s' value out of range.", name);
			واپس MOSQ_ERR_INVAL;
		}
		*قدر = (صحیح)l;
	}ورنہ{
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		واپس MOSQ_ERR_INVAL;
	}

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح conf__parse_ssize_t(حرف **token, ثابت حرف *name, ssize_t *قدر, حرف **saveptr)
{
	*token = strtok_r(تہی, " ", saveptr);
	اگر(*token){
		*قدر = atol(*token);
	}ورنہ{
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		واپس MOSQ_ERR_INVAL;
	}

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح conf__parse_string(حرف **token, ثابت حرف *name, حرف **قدر, حرف **saveptr)
{
	size_t tlen;

	*token = strtok_r(تہی, "", saveptr);
	اگر(*token){
		اگر(*قدر){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Duplicate '%s' value in configuration.", name);
			واپس MOSQ_ERR_INVAL;
		}
		/* Deal with multiple spaces at the beginning of the string. */
		*token = mosquitto_trimblanks(*token);
		اگر(لڑی_لمبائی(*token) == 0){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
			واپس MOSQ_ERR_INVAL;
		}

		tlen = لڑی_لمبائی(*token);
		اگر(tlen > UINT16_MAX){
			واپس MOSQ_ERR_INVAL;
		}
		اگر(mosquitto_validate_utf8(*token, (uint16_t)tlen)){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Malformed UTF-8 in configuration.");
			واپس MOSQ_ERR_INVAL;
		}
		*قدر = mosquitto_strdup(*token);
		اگر(!*قدر){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
			واپس MOSQ_ERR_NOMEM;
		}
	}ورنہ{
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		واپس MOSQ_ERR_INVAL;
	}
	واپس MOSQ_ERR_SUCCESS;
}
