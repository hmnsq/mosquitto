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

#यदि_परिभाषितम् WIN32
#अन्यथा
#  include <dirent.h>
#  include <strings.h>
#यदि_अन्तः

#यदि_अपरिभाषितम् WIN32
#  include <netdb.h>
#  include <sys/socket.h>
#अन्यथा
#  include <winsock2.h>
#  include <ws2tcpip.h>
#यदि_अन्तः

#यदि !defined(WIN32) && !defined(__CYGWIN__)
#  include <syslog.h>
#यदि_अन्तः

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "tls_mosq.h"
#अन्तर्भाव "util_mosq.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"

#अन्तर्भाव "utlist.h"

संरचना config_recurse {
	अचिह्नित पूर्णाङ्क log_dest;
	पूर्णाङ्क log_dest_समुच्चयः;
	अचिह्नित पूर्णाङ्क log_type;
	पूर्णाङ्क log_type_समुच्चयः;
};

#यदि defined(WIN32) || defined(__CYGWIN__)
#अन्तर्भाव <windows.h>
बाह्य SERVICE_स्थितिः_HANDLE service_handle;
#यदि_अन्तः


#परिभाषय REQUIRE_LISTENER(A) \
		कुरु{ \
			यदि(cur_listener == रिक्त){ \
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				प्रत्यागम MOSQ_ERR_INVAL; \
			} \
		}यावत्(0)

#परिभाषय REQUIRE_LISTENER_OR_DEFAULT_LISTENER(A) \
		कुरु{ \
			यदि(cur_listener == रिक्त){ \
				यदि(config__create_default_listener(config, (A))){ \
					प्रत्यागम MOSQ_ERR_NOMEM; \
				} \
				cur_listener = config->default_listener; \
			} \
		}यावत्(0)

#परिभाषय REQUIRE_LISTENER_IF_प्रत्येकम्_LISTENER(A) \
		कुरु{ \
			यदि(config->प्रत्येकम्_listener_settings == true && cur_listener == रिक्त){ \
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				प्रत्यागम MOSQ_ERR_INVAL; \
			} \
		}यावत्(0)

#परिभाषय REQUIRE_NON_DEFAULT_LISTENER(A) \
		कुरु{ \
			यदि(cur_listener == config->default_listener || cur_listener == रिक्त){ \
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: The '%s' option requires a listener to be defined first.", (A)); \
				प्रत्यागम MOSQ_ERR_INVAL; \
			} \
		}यावत्(0)

#परिभाषय REQUIRE_BRIDGE(A) \
		कुरु{ \
			यदि(cur_bridge == रिक्त){ \
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: The '%s' option requires a bridge to be defined first.", (A)); \
				प्रत्यागम MOSQ_ERR_INVAL; \
			} \
		}यावत्(0)

#परिभाषय REQUIRE_PLUGIN(A) \
		कुरु{ \
			यदि(cur_plugin == रिक्त){ \
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: The '%s' option requires plugin/global_plugin/plugin_load to be defined first.", (A)); \
				प्रत्यागम MOSQ_ERR_INVAL; \
			} \
		}यावत्(0)

#परिभाषय OPTION_DEPRECATED(A, B) \
		log__printf(रिक्त, MOSQ_LOG_NOTICE, "The '%s' option is now deprecated and will be removed in version 3.0. %s", (A), (B))

#परिभाषय OPTION_UNAVAILABLE(A) \
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: The '%s' option is no longer available.", (A));

#परिभाषय REQUIRE_NON_रिक्तानि_OPTION(A, B) \
		कुरु{ \
			यदि(!(A)){ \
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", (B)); \
				प्रत्यागम MOSQ_ERR_INVAL; \
			} \
		}यावत्(0)

#परिभाषय प्रत्येकम्_LISTENER_ALTERNATIVE(A, B) \
		यदि(config->प्रत्येकम्_listener_settings){ \
			log__printf(रिक्त, MOSQ_LOG_NOTICE, "You are using the '%s' option with 'per_listener_settings true'. Please replace this with '%s'.", A, B); \
		} \


#यदि_परिभाषितम् FINAL_WITH_TLS_PSK
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A) \
		कुरु{ \
			यदि(cur_bridge->tls_psk_तटस्थः || cur_bridge->tls_psk){ \
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and psk encryption in a single bridge.", (A)); \
				प्रत्यागम MOSQ_ERR_INVAL; \
			} \
		}यावत्(0)

#  define REQUIRE_BRIDGE_NO_X509(A) \
		कुरु{ \
			यदि(cur_bridge->tls_cafile || cur_bridge->tls_capath || cur_bridge->tls_certfile || cur_bridge->tls_keyfile){ \
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: '%s': Cannot use both certificate and identity encryption in a single bridge.", (A)); \
				प्रत्यागम MOSQ_ERR_INVAL; \
			} \
		}यावत्(0)

#अन्यथा
#  define REQUIRE_BRIDGE_NO_TLS_PSK(A)
#  define REQUIRE_BRIDGE_NO_X509(A)
#यदि_अन्तः

स्थिर संरचना mosquitto__security_options *cur_security_options = रिक्त;

स्थिर पूर्णाङ्क conf__parse_bool(वर्ण **token, ध्रुव वर्ण *name, bool *मानम्, वर्ण **saveptr);
स्थिर पूर्णाङ्क conf__parse_int(वर्ण **token, ध्रुव वर्ण *name, पूर्णाङ्क *मानम्, वर्ण **saveptr);
स्थिर पूर्णाङ्क conf__parse_ssize_t(वर्ण **token, ध्रुव वर्ण *name, ssize_t *मानम्, वर्ण **saveptr);
स्थिर पूर्णाङ्क conf__parse_string(वर्ण **token, ध्रुव वर्ण *name, वर्ण **मानम्, वर्ण **saveptr);
स्थिर पूर्णाङ्क config__read_file(संरचना mosquitto__config *config, bool reload, ध्रुव वर्ण *file, संरचना config_recurse *config_tmp, पूर्णाङ्क level, पूर्णाङ्क *lineno);
स्थिर पूर्णाङ्क config__check(संरचना mosquitto__config *config);
स्थिर शून्य config__cleanup_plugins(शून्य);
#यदि_परिभाषितम् WITH_BRIDGE
स्थिर पूर्णाङ्क config__check_bridges(संरचना mosquitto__config *config);
#यदि_अन्तः


स्थिर पूर्णाङ्क config_सङ्कलय_listener(संरचना mosquitto__config *config)
{
	संरचना mosquitto__listener *listener;
	संरचना mosquitto__listener *new_listeners;
	पूर्णाङ्क def_listener = -1;

	यदि(config->default_listener){
		प्रति(पूर्णाङ्क i=0; i<config->listener_गणनम्; i++){
			यदि(&config->listeners[i] == config->default_listener){
				def_listener = i;
				भङ्ग;
			}
		}
	}
	new_listeners = mosquitto_realloc(config->listeners, परिमाण(संरचना mosquitto__listener)*(size_t)(config->listener_गणनम्+1));
	यदि(!new_listeners){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	config->listeners = new_listeners;
	listener = &config->listeners[config->listener_गणनम्];
	स्मृतिं_पूरय(listener, 0, परिमाण(संरचना mosquitto__listener));
	listener->security_options = mosquitto_calloc(1, परिमाण(संरचना mosquitto__security_options));
	यदि(!listener->security_options){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	यदि(def_listener != -1){
		config->default_listener = &config->listeners[def_listener];
	}
	config->listener_गणनम्++;

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क config__create_default_listener(संरचना mosquitto__config *config, ध्रुव वर्ण *option_name)
{
	यदि(config->default_listener){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}
	log__printf(रिक्त, MOSQ_LOG_INFO, "Creating default listener due to '%s' option.", option_name);
	log__printf(रिक्त, MOSQ_LOG_INFO, "It is best practice to define a 'listener' first. Using the '%s' option without a listener will be disabled in the future.", option_name);
	यदि(config_सङ्कलय_listener(config)){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	config->default_listener = &config->listeners[config->listener_गणनम्-1];
	listener_समुच्चयः_defaults(config->default_listener);
	config->default_listener->पत्तनम् = 1883;

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर शून्य conf_समुच्चयः_cur_security_options(संरचना mosquitto__config *config, संरचना mosquitto__listener **cur_listener, संरचना mosquitto__security_options **security_options, ध्रुव वर्ण *option_name)
{
	यदि(config->प्रत्येकम्_listener_settings){
		यदि(*cur_listener == रिक्त){
			यदि(config__create_default_listener(config, option_name)){
				प्रत्यागम;
			}
			*cur_listener = config->default_listener;
		}
		(*security_options) = (*cur_listener)->security_options;
	}अन्यथा{
		(*security_options) = &config->security_options;
	}
}


स्थिर पूर्णाङ्क conf__attempt_resolve(ध्रुव वर्ण *host, ध्रुव वर्ण *text, अचिह्नित पूर्णाङ्क लघुगणकः, ध्रुव वर्ण *सन्देशः)
{
	संरचना addrinfo gai_hints;
	संरचना addrinfo *gai_res;
	पूर्णाङ्क rc;

	स्मृतिं_पूरय(&gai_hints, 0, परिमाण(संरचना addrinfo));
	gai_hints.ai_family = AF_UNSPEC;
	gai_hints.ai_socktype = SOCK_STREAM;
	gai_res = रिक्त;
	rc = getaddrinfo(host, रिक्त, &gai_hints, &gai_res);
	यदि(gai_res){
		freeaddrinfo(gai_res);
	}
	यदि(rc != 0){
#यदि_अपरिभाषितम् WIN32
		यदि(rc == EAI_SYSTEM){
			यदि(errno == ENOENT){
				log__printf(रिक्त, लघुगणकः, "%s: Unable to resolve %s %s.", सन्देशः, text, host);
			}अन्यथा{
				log__printf(रिक्त, लघुगणकः, "%s: Error resolving %s: %s.", सन्देशः, text, strerror(errno));
			}
		}अन्यथा{
			log__printf(रिक्त, लघुगणकः, "%s: Error resolving %s: %s.", सन्देशः, text, gai_strerror(rc));
		}
#अन्यथा
		यदि(rc == WSAHOST_NOT_प्राप्तम्){
			log__printf(रिक्त, लघुगणकः, "%s: Error resolving %s.", सन्देशः, text);
		}
#यदि_अन्तः
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर शून्य config__init_reload(संरचना mosquitto__config *config)
{
	/* Set defaults */
	प्रति(पूर्णाङ्क i=0; i<config->listener_गणनम्; i++){
		listener_समुच्चयः_defaults(&config->listeners[i]);
	}

	config->local_only = true;
	config->allow_duplicate_messages = true;

	mosquitto_FREE(config->security_options.acl_दत्त.acl_file);
	mosquitto_FREE(config->security_options.password_दत्त.password_file);
	mosquitto_FREE(config->security_options.psk_file);

	config->security_options.allow_anonymous = -1;
	config->security_options.allow_zero_दीर्घता_clientid = true;
	config->security_options.auto_id_उपसर्गः = रिक्त;
	config->security_options.auto_id_उपसर्गः_len = 0;

	config->autosave_interval = 1800;
	config->autosave_on_changes = false;

	mosquitto_FREE(config->clientid_prefixes);

	config->connection_messages = true;
	config->clientid_prefixes = रिक्त;
	config->प्रत्येकम्_listener_settings = false;
	यदि(config->log_fptr){
		fclose(config->log_fptr);
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
		config->log_type = UINT_MAX;
	}अन्यथा{
		config->log_type = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;
	}
#यदि_अन्तः
	config->log_timestamp = true;
	mosquitto_FREE(config->log_timestamp_format);
	config->global_max_clients = -1;
	config->global_max_connections = -1;
	config->log_timestamp_format = रिक्त;
	config->max_keepalive = 0;
	config->max_packet_आकार = 2000000;
	config->max_inflight_messages = 20;
	config->max_queued_messages = 1000;
	config->max_inflight_अष्टकानि = 0;
	config->max_queued_अष्टकानि = 0;
	config->persistence = false;
	mosquitto_FREE(config->persistence_location);
	mosquitto_FREE(config->persistence_file);
	config->persistent_client_expiration = 0;
	config->queue_qos0_messages = false;
	config->retain_उपलब्धम् = true;
	config->retain_expiry_interval = 0;
	config->समुच्चयः_tcp_nodelay = false;
	config->sys_interval = 10;
	config->upgrade_outgoing_qos = false;
	config->packet_बफर_आकार = 4096;

	config->packet_max_auth = 100000;
	config->packet_max_connect = 100000;
	config->packet_max_sub = 100000;
	config->packet_max_simple = 10000;
}


स्थिर शून्य config__cleanup_plugin_config(mosquitto_plugin_id_t *plugin)
{
	mosquitto_FREE(plugin->config.मार्गः);
	mosquitto_FREE(plugin->config.name);

	यदि(plugin->config.options){
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
	स्मृतिं_पूरय(config, 0, परिमाण(संरचना mosquitto__config));
	config__init_reload(config);

	config->daemon = false;
}


शून्य config__cleanup(संरचना mosquitto__config *config)
{
	mosquitto_FREE(config->clientid_prefixes);
	mosquitto_FREE(config->persistence_location);
	mosquitto_FREE(config->persistence_file);
	mosquitto_FREE(config->persistence_filepath);
	mosquitto_FREE(config->security_options.auto_id_उपसर्गः);
	mosquitto_FREE(config->security_options.acl_दत्त.acl_file);
	mosquitto_FREE(config->security_options.password_दत्त.password_file);
	mosquitto_FREE(config->security_options.psk_file);
	mosquitto_FREE(config->security_options.plugins);
	mosquitto_FREE(config->pid_file);
	mosquitto_FREE(config->user);
	mosquitto_FREE(config->log_timestamp_format);
	यदि(config->listeners){
		प्रति(पूर्णाङ्क i=0; i<config->listener_गणनम्; i++){
			mosquitto_FREE(config->listeners[i].host);
			mosquitto_FREE(config->listeners[i].bind_interface);
			mosquitto_FREE(config->listeners[i].mount_point);
			mosquitto_FREE(config->listeners[i].socks);
			यदि(config->listeners[i].security_options){
				mosquitto_FREE(config->listeners[i].security_options->auto_id_उपसर्गः);
				mosquitto_FREE(config->listeners[i].security_options->acl_दत्त.acl_file);
				mosquitto_FREE(config->listeners[i].security_options->password_दत्त.password_file);
				mosquitto_FREE(config->listeners[i].security_options->psk_file);
				mosquitto_FREE(config->listeners[i].security_options->plugins);
				mosquitto_FREE(config->listeners[i].security_options);
			}
#यदि_परिभाषितम् WITH_TLS
			mosquitto_FREE(config->listeners[i].cafile);
			mosquitto_FREE(config->listeners[i].capath);
			mosquitto_FREE(config->listeners[i].certfile);
			mosquitto_FREE(config->listeners[i].keyfile);
			mosquitto_FREE(config->listeners[i].ciphers);
			mosquitto_FREE(config->listeners[i].ciphers_tls13);
			mosquitto_FREE(config->listeners[i].psk_hint);
			mosquitto_FREE(config->listeners[i].crlfile);
			mosquitto_FREE(config->listeners[i].tls_संस्करणम्);
			mosquitto_FREE(config->listeners[i].tls_engine);
			mosquitto_FREE(config->listeners[i].tls_engine_kpass_sha1);
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			यदि(!config->listeners[i].ws_context) /* libwebsockets frees its own SSL_CTX */
#यदि_अन्तः
			{
				SSL_CTX_free(config->listeners[i].ssl_ctx);
				config->listeners[i].ssl_ctx = रिक्त;
			}
#यदि_अन्तः
#यदि defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
			mosquitto_FREE(config->listeners[i].http_dir);
#यदि_अन्तः
#यदि_परिभाषितम् WITH_WEBSOCKETS
			प्रति(पूर्णाङ्क j=0; j<config->listeners[i].ws_origin_गणनम्; j++){
				mosquitto_FREE(config->listeners[i].ws_origins[j]);
			}
			mosquitto_FREE(config->listeners[i].ws_origins);
#यदि_अन्तः
#यदि_परिभाषितम् WITH_UNIX_SOCKETS
			mosquitto_FREE(config->listeners[i].unix_socket_मार्गः);
#यदि_अन्तः
		}
		mosquitto_FREE(config->listeners);
	}
#यदि_परिभाषितम् WITH_BRIDGE
	यदि(config->bridges){
		प्रति(पूर्णाङ्क i=0; i<config->bridge_गणनम्; i++){
			config__bridge_cleanup(config->bridges[i]);
		}
		mosquitto_FREE(config->bridges);
	}
#यदि_अन्तः
	config__cleanup_plugins();

	यदि(config->log_fptr){
		fclose(config->log_fptr);
		config->log_fptr = रिक्त;
	}
	यदि(config->log_file){
		mosquitto_FREE(config->log_file);
		config->log_file = रिक्त;
	}
}

#यदि_परिभाषितम् WITH_BRIDGE


शून्य config__bridge_cleanup(संरचना mosquitto__bridge *bridge)
{
	यदि(bridge == रिक्त){
		प्रत्यागम;
	}

	mosquitto_FREE(bridge->name);
	यदि(bridge->स्थानाङ्काः){
		प्रति(पूर्णाङ्क i=0; i<bridge->स्थानाङ्कः_गणनम्; i++){
			mosquitto_FREE(bridge->स्थानाङ्काः[i].स्थानाङ्कः);
		}
		mosquitto_FREE(bridge->स्थानाङ्काः);
	}
	mosquitto_FREE(bridge->bind_स्थानाङ्कः);
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
			mosquitto_FREE(cur_topic->local_उपसर्गः);
			mosquitto_FREE(cur_topic->remote_उपसर्गः);
			mosquitto_FREE(cur_topic->local_topic);
			mosquitto_FREE(cur_topic->remote_topic);
			LL_DELETE(bridge->topics, cur_topic);
			mosquitto_FREE(cur_topic);
		}
		mosquitto_FREE(bridge->topics);
	}
	mosquitto_FREE(bridge->notification_topic);
#यदि_परिभाषितम् WITH_TLS
	mosquitto_FREE(bridge->tls_certfile);
	mosquitto_FREE(bridge->tls_keyfile);
	mosquitto_FREE(bridge->tls_संस्करणम्);
	mosquitto_FREE(bridge->tls_cafile);
	mosquitto_FREE(bridge->tls_capath);
	mosquitto_FREE(bridge->tls_alpn);
	mosquitto_FREE(bridge->tls_ciphers);
	mosquitto_FREE(bridge->tls_13_ciphers);
#यदि_परिभाषितम् FINAL_WITH_TLS_PSK
	mosquitto_FREE(bridge->tls_psk_तटस्थः);
	mosquitto_FREE(bridge->tls_psk);
#यदि_अन्तः
#यदि_अन्तः
	mosquitto_FREE(bridge);
}
#यदि_अन्तः


स्थिर शून्य print_संस्करणम्(शून्य)
{
	प्रारूप_लिख("mosquitto %s\n", संस्करणम्);
	प्रारूप_लिख("Copyright © 2025 Roger Light.\n");
	प्रारूप_लिख("License EPL-2.0 OR BSD-3-Clause.\n");
}


स्थिर शून्य print_usage(शून्य)
{
	प्रारूप_लिख("mosquitto version %s\n\n", संस्करणम्);
	प्रारूप_लिख("mosquitto is an MQTT v5.0/v3.1.1/v3.1 broker.\n\n");
	प्रारूप_लिख("Usage: mosquitto [-c config_file] [-d] [-h] [-p port] [-v]\n");
	प्रारूप_लिख("                 [--tls-keylog file]\n\n");
	प्रारूप_लिख(" -c : specify the broker config file.\n");
	प्रारूप_लिख(" -d : put the broker into the background after starting.\n");
	प्रारूप_लिख(" -h : display this help.\n");
	प्रारूप_लिख(" -p : start the broker listening on the specified port.\n");
	प्रारूप_लिख("      Not recommended in conjunction with the -c option.\n");
	प्रारूप_लिख(" -q : quiet mode - disable all logging types. This overrides\n");
	प्रारूप_लिख("      any logging options given in the config file, and -v.\n");
	प्रारूप_लिख(" -v : verbose mode - enable all logging types. This overrides\n");
	प्रारूप_लिख("      any logging options given in the config file.\n");
	प्रारूप_लिख(" --test-config : test config file and exit\n");
	प्रारूप_लिख(" --tls-keylog : log TLS connection information to a file, to allow\n");
	प्रारूप_लिख("      debugging with e.g. wireshark. Do not use on a production\n");
	प्रारूप_लिख("      server.\n");
	प्रारूप_लिख("\nSee https://mosquitto.org/ for more information.\n\n");
}


पूर्णाङ्क config_parse_तर्काः(संरचना mosquitto__config *config, पूर्णाङ्क argc, वर्ण *argv[])
{
	पूर्णाङ्क i;
	पूर्णाङ्क पत्तनम्_tmp;

	प्रति(i=1; i<argc; i++){
		यदि(!वाक्यं_तुलय(argv[i], "-c") || !वाक्यं_तुलय(argv[i], "--config-file")){
			यदि(i<argc-1){
				db.config_file = argv[i+1];

				यदि(config__read(config, false)){
					प्रत्यागम MOSQ_ERR_INVAL;
				}
			}अन्यथा{
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: -c argument given, but no config file specified.");
				प्रत्यागम MOSQ_ERR_INVAL;
			}
			i++;
		}अन्यथा यदि(!वाक्यं_तुलय(argv[i], "-d") || !वाक्यं_तुलय(argv[i], "--daemon")){
			config->daemon = true;
		}अन्यथा यदि(!वाक्यं_तुलय(argv[i], "-h") || !वाक्यं_तुलय(argv[i], "--help")){
			print_usage();
			प्रत्यागम MOSQ_ERR_UNKNOWN;
		}अन्यथा यदि(!वाक्यं_तुलय(argv[i], "--version")){
			print_संस्करणम्();
			प्रत्यागम MOSQ_ERR_UNKNOWN;
		}अन्यथा यदि(!वाक्यं_तुलय(argv[i], "-p") || !वाक्यं_तुलय(argv[i], "--port")){
			यदि(i<argc-1){
				पत्तनम्_tmp = atoi(argv[i+1]);
				यदि(पत्तनम्_tmp<1 || पत्तनम्_tmp>UINT16_MAX){
					log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid port specified (%d).", पत्तनम्_tmp);
					प्रत्यागम MOSQ_ERR_INVAL;
				}अन्यथा{
					यदि(config->cmd_पत्तनम्_गणनम् == CMD_पत्तनम्_सीमा){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Only %d ports can be specified on the command line.", CMD_पत्तनम्_सीमा);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					config->cmd_पत्तनम्[config->cmd_पत्तनम्_गणनम्] = (uint16_t)पत्तनम्_tmp;
					config->cmd_पत्तनम्_गणनम्++;
				}
			}अन्यथा{
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: -p argument given, but no port specified.");
				प्रत्यागम MOSQ_ERR_INVAL;
			}
			i++;
		}अन्यथा यदि(!वाक्यं_तुलय(argv[i], "--tls-keylog")){
#यदि_परिभाषितम् WITH_TLS
			यदि(i<argc-1){
				db.tls_keylog = mosquitto_strdup(argv[i+1]);
				यदि(db.tls_keylog == रिक्त){
					log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
					प्रत्यागम MOSQ_ERR_NOMEM;
				}
			}अन्यथा{
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: --tls-keylog argument given, but no file specified.");
				प्रत्यागम MOSQ_ERR_INVAL;
			}
			i++;
#अन्यथा
			fprintf(stderr, "Error: TLS support not available so --tls-keylog is not available.\n");
			प्रत्यागम MOSQ_ERR_INVAL;
#यदि_अन्तः
		}अन्यथा यदि(!वाक्यं_तुलय(argv[i], "-q") || !वाक्यं_तुलय(argv[i], "--quiet")){
			db.quiet = true;
		}अन्यथा यदि(!वाक्यं_तुलय(argv[i], "-v") || !वाक्यं_तुलय(argv[i], "--verbose")){
			db.verbose = true;
		}अन्यथा यदि(!वाक्यं_तुलय(argv[i], "--test-config")){
			config->test_configuration = true;
		}अन्यथा{
			fprintf(stderr, "Error: Unknown option '%s'.\n", argv[i]);
			print_usage();
			प्रत्यागम MOSQ_ERR_INVAL;
		}
	}

	/* Default to drop to mosquitto user if we are privileged and no user specified. */
	यदि(!config->user){
		config->user = mosquitto_strdup("mosquitto");
		यदि(config->user == रिक्त){
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
	}
	यदि(db.quiet){
		config->log_type = 0;
	}अन्यथा यदि(db.verbose){
		config->log_type = UINT_MAX;
	}

	यदि(getenv("MOSQUITTO_PERSISTENCE_LOCATION")){
		mosquitto_FREE(config->persistence_location);
		config->persistence_location = mosquitto_strdup(getenv("MOSQUITTO_PERSISTENCE_LOCATION"));
		यदि(!config->persistence_location){
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
	}
	प्रत्यागम config__check(config);
}


स्थिर शून्य config__copy(संरचना mosquitto__config *src, संरचना mosquitto__config *dest)
{
	mosquitto_FREE(dest->security_options.acl_दत्त.acl_file);
	dest->security_options.acl_दत्त.acl_file = src->security_options.acl_दत्त.acl_file;

	acl_file__cleanup(&dest->security_options.acl_दत्त);
	dest->security_options.acl_दत्त.acl_users = src->security_options.acl_दत्त.acl_users;
	dest->security_options.acl_दत्त.acl_patterns = src->security_options.acl_दत्त.acl_patterns;
	dest->security_options.acl_दत्त.acl_anon.username = src->security_options.acl_दत्त.acl_anon.username;
	dest->security_options.acl_दत्त.acl_anon.acl = src->security_options.acl_दत्त.acl_anon.acl;

	dest->security_options.allow_anonymous = src->security_options.allow_anonymous;
	dest->security_options.allow_zero_दीर्घता_clientid = src->security_options.allow_zero_दीर्घता_clientid;

	mosquitto_FREE(dest->security_options.auto_id_उपसर्गः);
	dest->security_options.auto_id_उपसर्गः = src->security_options.auto_id_उपसर्गः;
	dest->security_options.auto_id_उपसर्गः_len = src->security_options.auto_id_उपसर्गः_len;

	mosquitto_FREE(dest->security_options.password_दत्त.password_file);
	dest->security_options.password_दत्त.password_file = src->security_options.password_दत्त.password_file;

	password_file__cleanup(&dest->security_options.password_दत्त);
	dest->security_options.password_दत्त.unpwd = src->security_options.password_दत्त.unpwd;

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

	dest->सन्देशः_आकार_सीमा = src->सन्देशः_आकार_सीमा;

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
#यदि_अन्तः

#यदि_परिभाषितम् WITH_BRIDGE
	प्रति(पूर्णाङ्क i=0; i<dest->bridge_गणनम्; i++){
		यदि(dest->bridges[i]){
			config__bridge_cleanup(dest->bridges[i]);
		}
	}
	mosquitto_FREE(dest->bridges);
	dest->bridges = src->bridges;
	dest->bridge_गणनम् = src->bridge_गणनम्;
#यदि_अन्तः
}


पूर्णाङ्क config__read(संरचना mosquitto__config *config, bool reload)
{
	पूर्णाङ्क rc = MOSQ_ERR_SUCCESS;
	संरचना config_recurse cr;
	पूर्णाङ्क lineno = 0;
#यदि_परिभाषितम् WITH_PERSISTENCE
	size_t len;
#यदि_अन्तः
	संरचना mosquitto__config config_reload;
	पूर्णाङ्क i;

	यदि(reload){
		स्मृतिं_पूरय(&config_reload, 0, परिमाण(संरचना mosquitto__config));
	}

	cr.log_dest = MQTT3_LOG_NONE;
	cr.log_dest_समुच्चयः = 0;
	cr.log_type = MOSQ_LOG_NONE;
	cr.log_type_समुच्चयः = 0;

	यदि(!db.config_file){
		प्रत्यागम 0;
	}

	यदि(reload){
		/* Re-initialise appropriate config vars to default for reload. */
		config__init_reload(&config_reload);
		config_reload.listeners = config->listeners;
		config_reload.listener_गणनम् = config->listener_गणनम्;
		cur_security_options = रिक्त;
		rc = config__read_file(&config_reload, reload, db.config_file, &cr, 0, &lineno);
	}अन्यथा{
		rc = config__read_file(config, reload, db.config_file, &cr, 0, &lineno);
	}
	यदि(rc){
		यदि(lineno > 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error found at %s:%d.", db.config_file, lineno);
		}
		प्रत्यागम rc;
	}

	यदि(reload){
		config__copy(&config_reload, config);
	}

	/* If auth/access options are set and allow_anonymous not explicitly set, disallow anon. */
	यदि(config->local_only == false){
		यदि(config->प्रत्येकम्_listener_settings){
			प्रति(i=0; i<config->listener_गणनम्; i++){
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
#यदि_परिभाषितम् WITH_PERSISTENCE
	यदि(config->persistence){
		यदि(!config->persistence_file){
			config->persistence_file = mosquitto_strdup("mosquitto.db");
			यदि(!config->persistence_file){
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
		}
		mosquitto_FREE(config->persistence_filepath);
		यदि(config->persistence_location && वाक्यदैर्घ्यम्(config->persistence_location)){
			len = वाक्यदैर्घ्यम्(config->persistence_location) + वाक्यदैर्घ्यम्(config->persistence_file) + 2;
			config->persistence_filepath = mosquitto_malloc(len);
			यदि(!config->persistence_filepath){
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
#यदि_परिभाषितम् WIN32
			snprintf(config->persistence_filepath, len, "%s\\%s", config->persistence_location, config->persistence_file);
#अन्यथा
			snprintf(config->persistence_filepath, len, "%s/%s", config->persistence_location, config->persistence_file);
#यदि_अन्तः
		}अन्यथा{
			config->persistence_filepath = mosquitto_strdup(config->persistence_file);
			यदि(!config->persistence_filepath){
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
		}
	}
#यदि_अन्तः
	/* Default to drop to mosquitto user if no other user specified. This must
	 * remain here even though it is covered in config__parse_args() because this
	 * function may be called on its own. */
	यदि(!config->user){
		config->user = mosquitto_strdup("mosquitto");
	}

#यदि_परिभाषितम् WITH_BRIDGE
	प्रति(i=0; i<config->bridge_गणनम्; i++){
		यदि(!config->bridges[i]->name){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: bridge name not defined.");
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		यदि(config->bridges[i]->स्थानाङ्काः  == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no remote addresses defined.");
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		यदि(config->bridges[i]->topic_गणनम् == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: no topics defined.");
			प्रत्यागम MOSQ_ERR_INVAL;
		}
#यदि_परिभाषितम् FINAL_WITH_TLS_PSK
		यदि(config->bridges[i]->tls_psk && !config->bridges[i]->tls_psk_तटस्थः){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_identity.");
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		यदि(config->bridges[i]->tls_psk_तटस्थः && !config->bridges[i]->tls_psk){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge configuration: missing bridge_psk.");
			प्रत्यागम MOSQ_ERR_INVAL;
		}
#यदि_अन्तः
	}
#यदि_अन्तः

	यदि(cr.log_dest_समुच्चयः){
		config->log_dest = cr.log_dest;
	}
	यदि(db.quiet){
		config->log_type = 0;
	}अन्यथा यदि(db.verbose){
		config->log_type = UINT_MAX;
	}अन्यथा यदि(cr.log_type_समुच्चयः){
		config->log_type = cr.log_type;
	}
#यदि_परिभाषितम् WITH_BRIDGE
	प्रत्यागम config__check_bridges(config);
#अन्यथा
	प्रत्यागम MOSQ_ERR_SUCCESS;
#यदि_अन्तः
}


स्थिर mosquitto_plugin_id_t *config__plugin_find(ध्रुव वर्ण *name)
{
	यदि(db.plugins && name){
		प्रति(पूर्णाङ्क i=0; i<db.plugin_गणनम्; i++){
			यदि(db.plugins[i]->config.name && !वाक्यं_तुलय(db.plugins[i]->config.name, name)){
				प्रत्यागम db.plugins[i];
			}
		}
	}
	प्रत्यागम रिक्त;
}


स्थिर mosquitto_plugin_id_t *config_plugin_आरोपय(ध्रुव वर्ण *name, ध्रुव वर्ण *मार्गः)
{
	mosquitto_plugin_id_t *plugin = रिक्त;
	mosquitto_plugin_id_t **plugins = रिक्त;

	यदि(name && config__plugin_find(name)){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Duplicate plugin name '%s'.", name);
		प्रत्यागम रिक्त;
	}

	यदि(!मार्गः || !वाक्यं_तुलय(मार्गः, "")){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Missing plugin path for plugin name '%s'.", name);
		प्रत्यागम रिक्त;
	}

	plugin = mosquitto_calloc(1, परिमाण(mosquitto_plugin_id_t));
	यदि(!plugin){
		गच्छ error;
	}

	यदि(name){
		plugin->config.name = mosquitto_strdup(name);
	}
	plugin->config.मार्गः = mosquitto_strdup(मार्गः);
	यदि((name && !plugin->config.name) || !plugin->config.मार्गः){
		गच्छ error;
	}
	plugin->config.options = रिक्त;
	plugin->config.option_गणनम् = 0;
	plugin->config.deny_special_chars = true;

	/* Add to db list */
	plugins = mosquitto_realloc(db.plugins, (size_t)(db.plugin_गणनम्+1)*परिमाण(संरचना mosquitto__plugin_config *));
	यदि(!plugins){
		गच्छ error;
	}

	plugins[db.plugin_गणनम्] = plugin;
	db.plugins = plugins;
	db.plugin_गणनम्++;

	प्रत्यागम plugin;
error:
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
	यदि(plugin){
		mosquitto_FREE(plugin->config.name);
		mosquitto_FREE(plugin->config.मार्गः);
	}
	mosquitto_FREE(plugin);
	प्रत्यागम रिक्त;
}


पूर्णाङ्क config_plugin_सङ्कलय_secopt(mosquitto_plugin_id_t *plugin, संरचना mosquitto__security_options *security_options)
{
	संरचना mosquitto__security_options **new_options;
	mosquitto_plugin_id_t **new_plugins;

	new_options = mosquitto_realloc(plugin->config.security_options, (size_t)(plugin->config.security_option_गणनम्+1)*परिमाण(संरचना mosquitto__security_options *));
	new_plugins = mosquitto_realloc(security_options->plugins, (size_t)(security_options->plugin_गणनम्+1)*परिमाण(mosquitto_plugin_id_t *));

	यदि(!new_options || !new_plugins){
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
#यदि_परिभाषितम् WITH_BRIDGE
	वर्ण *tmp_char;
	संरचना mosquitto__bridge *cur_bridge = रिक्त;
#यदि_अन्तः
	mosquitto_plugin_id_t *cur_plugin = रिक्त;

	वर्ण *कुञ्चिका;
	संरचना mosquitto__listener *cur_listener = रिक्त;
	पूर्णाङ्क i;
	पूर्णाङ्क lineno_ext = 0;
	size_t उपसर्गः_len;
	size_t slen;
#यदि_परिभाषितम् WITH_WEBSOCKETS
	वर्ण **ws_origins = रिक्त;
#यदि_अन्तः

	*lineno = 0;

	यावत्(mosquitto_fgets(buf, buflen, fptr)){
		(*lineno)++;
		यदि((*buf)[0] != '#' && (*buf)[0] != 10 && (*buf)[0] != 13){
			slen = वाक्यदैर्घ्यम्(*buf);
			यदि(slen == 0){
				अनुवर्त;
			}
			यावत्((*buf)[slen-1] == 10 || (*buf)[slen-1] == 13){
				(*buf)[slen-1] = 0;
				slen = वाक्यदैर्घ्यम्(*buf);
				यदि(slen == 0){
					अनुवर्त;
				}
			}
			token = strtok_r((*buf), " ", &saveptr);
			यदि(token){
				यदि(!वाक्यं_तुलय(token, "accept_protocol_versions")){
					REQUIRE_NON_DEFAULT_LISTENER(token);
					cur_listener->disable_protocol_v3 = true;
					cur_listener->disable_protocol_v4 = true;
					cur_listener->disable_protocol_v5 = true;
					यदि(saveptr == रिक्त){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", "accept_protocol_versions");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					token = strtok_r(saveptr, ", \t", &saveptr);
					REQUIRE_NON_रिक्तानि_OPTION(token, "accept_protocol_versions");

					यावत्(token){
						यदि(!वाक्यं_तुलय(token, "3")){
							cur_listener->disable_protocol_v3 = false;
						}अन्यथा यदि(!वाक्यं_तुलय(token, "4")){
							cur_listener->disable_protocol_v4 = false;
						}अन्यथा यदि(!वाक्यं_तुलय(token, "5")){
							cur_listener->disable_protocol_v5 = false;
						}

						token = strtok_r(रिक्त, ", \t", &saveptr);
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "acl_file")){
					REQUIRE_LISTENER_IF_प्रत्येकम्_LISTENER(token);

					conf_समुच्चयः_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->acl_दत्त.acl_file);
					यदि(conf__parse_string(&token, "acl_file", &cur_security_options->acl_दत्त.acl_file, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "address") || !वाक्यं_तुलय(token, "addresses")){
#यदि_परिभाषितम् WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(cur_bridge->स्थानाङ्काः){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge configuration, 'address' only allowed once.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यावत्((token = strtok_r(रिक्त, " ", &saveptr))){
						यदि(token[0] == '#'){
							भङ्ग;
						}
						संरचना bridge_स्थानाङ्कः *new_स्थानाङ्काः = mosquitto_realloc(cur_bridge->स्थानाङ्काः, परिमाण(संरचना bridge_स्थानाङ्कः)*(size_t)(cur_bridge->स्थानाङ्कः_गणनम्+1));
						यदि(!new_स्थानाङ्काः){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
							प्रत्यागम MOSQ_ERR_NOMEM;
						}
						cur_bridge->स्थानाङ्कः_गणनम्++;
						cur_bridge->स्थानाङ्काः = new_स्थानाङ्काः;
						स्मृतिं_पूरय(&cur_bridge->स्थानाङ्काः[cur_bridge->स्थानाङ्कः_गणनम्-1], 0, परिमाण(संरचना bridge_स्थानाङ्कः));
						cur_bridge->स्थानाङ्काः[cur_bridge->स्थानाङ्कः_गणनम्-1].स्थानाङ्कः = mosquitto_strdup(token);
						यदि(!cur_bridge->स्थानाङ्काः[cur_bridge->स्थानाङ्कः_गणनम्-1].स्थानाङ्कः){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
							प्रत्यागम MOSQ_ERR_NOMEM;
						}
					}
					प्रति(i=0; i<cur_bridge->स्थानाङ्कः_गणनम्; i++){
						/* cur_bridge->addresses[i].address is now
						 * "address[:port]". If address is an IPv6 address,
						 * then port is required. We must check for the :
						 * backwards. */
						tmp_char = strrchr(cur_bridge->स्थानाङ्काः[i].स्थानाङ्कः, ':');
						यदि(tmp_char){
							/* Remove ':', so cur_bridge->addresses[i].address
							 * now just looks like the address. */
							tmp_char[0] = '\0';

							/* The remainder of the string */
							tmp_int = atoi(&tmp_char[1]);
							यदि(tmp_int < 1 || tmp_int > UINT16_MAX){
								log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge port value (%d).", tmp_int);
								प्रत्यागम MOSQ_ERR_INVAL;
							}
							cur_bridge->स्थानाङ्काः[i].पत्तनम् = (uint16_t)tmp_int;
						}अन्यथा{
							cur_bridge->स्थानाङ्काः[i].पत्तनम् = 1883;
						}
						conf__attempt_resolve(cur_bridge->स्थानाङ्काः[i].स्थानाङ्कः, "bridge address", MOSQ_LOG_WARNING, "Warning");
					}
					यदि(cur_bridge->स्थानाङ्कः_गणनम् == 0){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty address value in configuration.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "allow_anonymous")){
					REQUIRE_LISTENER_IF_प्रत्येकम्_LISTENER(token);
					प्रत्येकम्_LISTENER_ALTERNATIVE(token, "listener_allow_anonymous");
					conf_समुच्चयः_cur_security_options(config, &cur_listener, &cur_security_options, token);
					यदि(conf__parse_bool(&token, "allow_anonymous", (bool *)&cur_security_options->allow_anonymous, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "allow_duplicate_messages")){
					OPTION_DEPRECATED(token, "The behaviour will default to true.");
					यदि(conf__parse_bool(&token, "allow_duplicate_messages", &config->allow_duplicate_messages, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "allow_zero_length_clientid")){
					REQUIRE_LISTENER_IF_प्रत्येकम्_LISTENER(token);
					conf_समुच्चयः_cur_security_options(config, &cur_listener, &cur_security_options, token);
					यदि(conf__parse_bool(&token, "allow_zero_length_clientid", &cur_security_options->allow_zero_दीर्घता_clientid, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्ये_तुलय(token, "auth_opt_", वाक्यदैर्घ्यम्("auth_opt_")) || !वाक्ये_तुलय(token, "plugin_opt_", वाक्यदैर्घ्यम्("plugin_opt_"))){
					यदि(reload){
						अनुवर्त;        /* Auth plugin not currently valid for reloading. */

					}
					REQUIRE_PLUGIN(token);

					यदि(!वाक्ये_तुलय(token, "auth_opt_", वाक्यदैर्घ्यम्("auth_opt_"))){
						उपसर्गः_len = वाक्यदैर्घ्यम्("auth_opt_");
					}अन्यथा{
						उपसर्गः_len = वाक्यदैर्घ्यम्("plugin_opt_");
					}
					यदि(वाक्यदैर्घ्यम्(token) < उपसर्गः_len + 3){
						/* auth_opt_ == 9, + one digit key == 10, + one space == 11, + one value == 12 */
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'plugin_opt_' config option.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					कुञ्चिका = mosquitto_strdup(&token[उपसर्गः_len]);
					यदि(!कुञ्चिका){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
						प्रत्यागम MOSQ_ERR_NOMEM;
					}अन्यथा यदि(STREMPTY(कुञ्चिका)){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty 'plugin_opt_' config option.");
						mosquitto_FREE(कुञ्चिका);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					token = saveptr;
					यदि(token && token[0]){
						यावत्(token[0] == ' ' || token[0] == '\t'){
							token++;
						}
						संरचना mosquitto_opt *new_options = mosquitto_realloc(cur_plugin->config.options, (size_t)(cur_plugin->config.option_गणनम्+1)*परिमाण(संरचना mosquitto_opt));
						यदि(!new_options){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
							mosquitto_FREE(कुञ्चिका);
							प्रत्यागम MOSQ_ERR_NOMEM;
						}
						cur_plugin->config.option_गणनम्++;
						cur_plugin->config.options = new_options;
						cur_plugin->config.options[cur_plugin->config.option_गणनम्-1].कुञ्चिका = कुञ्चिका;
						cur_plugin->config.options[cur_plugin->config.option_गणनम्-1].मानम् = mosquitto_strdup(token);
						यदि(!cur_plugin->config.options[cur_plugin->config.option_गणनम्-1].मानम्){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
							प्रत्यागम MOSQ_ERR_NOMEM;
						}
					}अन्यथा{
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", कुञ्चिका);
						mosquitto_FREE(कुञ्चिका);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "auth_plugin") || !वाक्यं_तुलय(token, "plugin") || !वाक्यं_तुलय(token, "global_plugin")){
					यदि(reload){
						अनुवर्त;        /* plugin not currently valid for reloading. */
					}
					यदि(!वाक्यं_तुलय(token, "global_plugin")){
						cur_security_options = &db.config->security_options;
					}अन्यथा{
						REQUIRE_LISTENER_IF_प्रत्येकम्_LISTENER(token);
						conf_समुच्चयः_cur_security_options(config, &cur_listener, &cur_security_options, token);
					}

					cur_plugin = config_plugin_आरोपय(रिक्त, saveptr);
					यदि(cur_plugin == रिक्त){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(config_plugin_सङ्कलय_secopt(cur_plugin, cur_security_options)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "auth_plugin_deny_special_chars")){
					यदि(reload){
						अनुवर्त;        /* Auth plugin not currently valid for reloading. */
					}
					यदि(!cur_plugin){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: An auth_plugin_deny_special_chars option exists in the config file without a plugin.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(conf__parse_bool(&token, "auth_plugin_deny_special_chars", &cur_plugin->config.deny_special_chars, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "plugin_load")){
					वर्ण *name = रिक्त;

					यदि(reload){
						अनुवर्त;        /* plugin not currently valid for reloading. */

					}
					name = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_रिक्तानि_OPTION(name, "plugin_load");

					cur_plugin = config_plugin_आरोपय(name, saveptr);
					यदि(cur_plugin == रिक्त){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "plugin_use")){
					वर्ण *name = रिक्त;

					यदि(reload){
						अनुवर्त;        /* plugin not currently valid for reloading. */
					}
					REQUIRE_NON_DEFAULT_LISTENER(token);

					यदि(conf__parse_string(&token, "plugin_use", &name, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					cur_plugin = config__plugin_find(name);
					यदि(!cur_plugin){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Plugin '%s' not previously loaded.", name);
						mosquitto_FREE(name);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(name);
					यदि(config_plugin_सङ्कलय_secopt(cur_plugin, cur_listener->security_options)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "auto_id_prefix")){
					REQUIRE_LISTENER_IF_प्रत्येकम्_LISTENER(token);
					OPTION_DEPRECATED(token, "Please use 'listener_auto_id_prefix' instead.");
					conf_समुच्चयः_cur_security_options(config, &cur_listener, &cur_security_options, token);
					यदि(conf__parse_string(&token, "auto_id_prefix", &cur_security_options->auto_id_उपसर्गः, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(cur_security_options->auto_id_उपसर्गः){
						यदि(वाक्यदैर्घ्यम्(cur_security_options->auto_id_उपसर्गः) > 50){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: auto_id_prefix length must be <= 50.");
							प्रत्यागम MOSQ_ERR_INVAL;
						}
						cur_security_options->auto_id_उपसर्गः_len = (uint16_t)वाक्यदैर्घ्यम्(cur_security_options->auto_id_उपसर्गः);
					}अन्यथा{
						cur_security_options->auto_id_उपसर्गः_len = 0;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "autosave_interval")){
					यदि(conf__parse_int(&token, "autosave_interval", &config->autosave_interval, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(config->autosave_interval < 0){
						config->autosave_interval = 0;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "autosave_on_changes")){
					यदि(conf__parse_bool(&token, "autosave_on_changes", &config->autosave_on_changes, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "bind_address")){
					OPTION_DEPRECATED(token, "");
					config->local_only = false;
					यदि(reload){
						अनुवर्त;        /* Rebinding listeners not valid during reloading. */

					}
					यदि(config__create_default_listener(config, token)){
						प्रत्यागम MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					यदि(conf__parse_string(&token, "default listener bind_address", &config->default_listener->host, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(conf__attempt_resolve(config->default_listener->host, "bind_address", MOSQ_LOG_ERR, "Error")){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "bind_interface")){
#यदि_परिभाषितम् SO_BINDTODEVICE
					यदि(reload){
						अनुवर्त;        /* Rebinding listeners not valid during reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					यदि(conf__parse_string(&token, "bind_interface", &cur_listener->bind_interface, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_ERR, "Error: bind_interface specified but socket option not available.");
					प्रत्यागम MOSQ_ERR_INVAL;
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "bridge_attempt_unsubscribe")){
#यदि_परिभाषितम् WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_bool(&token, "bridge_attempt_unsubscribe", &cur_bridge->attempt_unsubscribe, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "bridge_cafile")){
#यदि defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					यदि(conf__parse_string(&token, "bridge_cafile", &cur_bridge->tls_cafile, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "bridge_alpn")){
#यदि defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_string(&token, "bridge_alpn", &cur_bridge->tls_alpn, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "bridge_ciphers")){
#यदि defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_string(&token, "bridge_ciphers", &cur_bridge->tls_ciphers, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "bridge_ciphers_tls1.3")){
#यदि defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_string(&token, "bridge_ciphers_tls1.3", &cur_bridge->tls_13_ciphers, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "bridge_bind_address")){
#यदि defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_string(&token, "bridge_bind_address", &cur_bridge->bind_स्थानाङ्कः, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "bridge_capath")){
#यदि defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					यदि(conf__parse_string(&token, "bridge_capath", &cur_bridge->tls_capath, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "bridge_certfile")){
#यदि defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					यदि(conf__parse_string(&token, "bridge_certfile", &cur_bridge->tls_certfile, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "bridge_identity")){
#यदि defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					यदि(conf__parse_string(&token, "bridge_identity", &cur_bridge->tls_psk_तटस्थः, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "bridge_insecure")){
#यदि defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_bool(&token, "bridge_insecure", &cur_bridge->tls_insecure, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(cur_bridge->tls_insecure){
						log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge '%s' using insecure mode.", cur_bridge->name);
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "bridge_require_ocsp")){
#यदि defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_bool(&token, "bridge_require_ocsp", &cur_bridge->tls_ocsp_required, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "bridge_max_packet_size")){
#यदि defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_int(&token, "bridge_max_packet_size", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 0){
						tmp_int = 0;
					}
					cur_bridge->अधिकतमम्_packet_आकार = (uint32_t)tmp_int;
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "bridge_max_topic_alias")){
#यदि_परिभाषितम् WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_int(&token, "bridge_max_topic_alias", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}

					यदि(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: bridge_max_topic_alias must be > 0 and <= 65535.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					cur_bridge->max_topic_alias = (uint16_t)tmp_int;
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "bridge_outgoing_retain")){
#यदि defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_bool(&token, "bridge_outgoing_retain", &cur_bridge->outgoing_retain, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "bridge_keyfile")){
#यदि defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_TLS_PSK(token);
					mosquitto_FREE(cur_bridge->tls_keyfile);
					यदि(conf__parse_string(&token, "bridge_keyfile", &cur_bridge->tls_keyfile, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "bridge_protocol_version")){
#यदि_परिभाषितम् WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(रिक्त, "", &saveptr);
					REQUIRE_NON_रिक्तानि_OPTION(token, "bridge_protocol_version");

					यदि(!वाक्यं_तुलय(token, "mqttv31")){
						cur_bridge->protocol_संस्करणम् = mosq_p_mqtt31;
					}अन्यथा यदि(!वाक्यं_तुलय(token, "mqttv311")){
						cur_bridge->protocol_संस्करणम् = mosq_p_mqtt311;
					}अन्यथा यदि(!वाक्यं_तुलय(token, "mqttv50")){
						cur_bridge->protocol_संस्करणम् = mosq_p_mqtt5;
					}अन्यथा{
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'bridge_protocol_version' value (%s).", token);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "bridge_psk")){
#यदि defined(WITH_BRIDGE) && defined(FINAL_WITH_TLS_PSK)
					REQUIRE_BRIDGE(token);
					REQUIRE_BRIDGE_NO_X509(token);
					यदि(conf__parse_string(&token, "bridge_psk", &cur_bridge->tls_psk, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS-PSK support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "bridge_receive_maximum")){
#यदि defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_int(&token, "bridge_receive_maximum", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(tmp_int <= 0){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be greater than 0.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}अन्यथा यदि((uint64_t)tmp_int > (uint64_t)UINT16_MAX){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: bridge_receive_maximum must be lower than %u.", UINT16_MAX);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					cur_bridge->receive_अधिकतमम् = (uint16_t)tmp_int;
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "bridge_reload_type")){
#यदि_परिभाषितम् WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_रिक्तानि_OPTION(token, "bridge_reload_type");

					यदि(!वाक्यं_तुलय(token, "lazy")){
						cur_bridge->reload_type = brt_lazy;
					}अन्यथा यदि(!वाक्यं_तुलय(token, "immediate")){
						cur_bridge->reload_type = brt_तात्कालिकम्;
					}अन्यथा{
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'bridge_reload_type' value in configuration (%s).", token);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "bridge_session_expiry_interval")){
#यदि defined(WITH_BRIDGE)
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_int(&token, "bridge_session_expiry_interval", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 0){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must not be negative.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}अन्यथा यदि((uint64_t)tmp_int > (uint64_t)MQTT_SESSION_EXPIRY_NEVER){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: bridge_session_expiry_interval must be lower than %u.", MQTT_SESSION_EXPIRY_NEVER);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					cur_bridge->session_expiry_interval = (uint32_t)tmp_int;
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "bridge_tcp_keepalive")){
#यदि_परिभाषितम् WITH_BRIDGE
					REQUIRE_BRIDGE(token);

					यदि(conf__parse_int(&token, "bridge_tcp_keepalive_idle", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(tmp_int <= 0){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: invalid TCP keepalive idle value.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_idle = (अचिह्नित पूर्णाङ्क)tmp_int;

					यदि(conf__parse_int(&token, "bridge_tcp_keepalive_interval", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(tmp_int <= 0){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: invalid TCP keepalive interval value.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_interval = (अचिह्नित पूर्णाङ्क)tmp_int;

					यदि(conf__parse_int(&token, "bridge_tcp_keepalive_counter", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(tmp_int <= 0){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: invalid TCP keepalive counter value.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_keepalive_गणक = (अचिह्नित पूर्णाङ्क)tmp_int;
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "bridge_tcp_user_timeout")){
#यदि_परिभाषितम् WITH_BRIDGE
					REQUIRE_BRIDGE(token);
#यदि_परिभाषितम् WITH_TCP_USER_TIMEOUT
					यदि(conf__parse_int(&token, "bridge_tcp_user_timeout", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 0){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: invalid TCP user timeout value.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					cur_bridge->tcp_user_timeout = tmp_int;
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge TCP user timeout support not available.");
#यदि_अन्तः
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "bridge_tls_use_os_certs")){
#यदि defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_bool(&token, "bridge_tls_use_os_certs", &cur_bridge->tls_use_os_certs, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "bridge_tls_version")){
#यदि defined(WITH_BRIDGE) && defined(WITH_TLS)
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_string(&token, "bridge_tls_version", &cur_bridge->tls_संस्करणम्, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge and/or TLS support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "cafile")){
#यदि defined(WITH_TLS)
					REQUIRE_LISTENER(token);
					यदि(cur_listener->psk_hint){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->cafile);
					यदि(conf__parse_string(&token, "cafile", &cur_listener->cafile, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "capath")){
#यदि_परिभाषितम् WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->capath);
					यदि(conf__parse_string(&token, "capath", &cur_listener->capath, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "certfile")){
#यदि_परिभाषितम् WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					यदि(cur_listener->psk_hint){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_listener->certfile);
					यदि(conf__parse_string(&token, "certfile", &cur_listener->certfile, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "check_retain_source")){
					यदि(conf__parse_bool(&token, "check_retain_source", &config->check_retain_source, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "ciphers")){
#यदि_परिभाषितम् WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers);
					यदि(conf__parse_string(&token, "ciphers", &cur_listener->ciphers, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "ciphers_tls1.3")){
#यदि defined(WITH_TLS) && (!defined(LIBRESSL_संस्करणम्_सङ्ख्या) || LIBRESSL_संस्करणम्_सङ्ख्या > 0x3040000FL)
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->ciphers_tls13);
					यदि(conf__parse_string(&token, "ciphers_tls1.3", &cur_listener->ciphers_tls13, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: ciphers_tls1.3 support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "clientid") || !वाक्यं_तुलय(token, "remote_clientid")){
#यदि_परिभाषितम् WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_string(&token, "bridge remote clientid", &cur_bridge->remote_clientid, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "cleansession")){
#यदि_परिभाषितम् WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_bool(&token, "cleansession", &cur_bridge->clean_start, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "local_cleansession")){
#यदि_परिभाषितम् WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_bool(&token, "local_cleansession", (bool *)&cur_bridge->clean_start_local, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "clientid_prefixes")){
					OPTION_DEPRECATED(token, "");
					mosquitto_FREE(config->clientid_prefixes);
					यदि(conf__parse_string(&token, "clientid_prefixes", &config->clientid_prefixes, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "connection")){
#यदि_परिभाषितम् WITH_BRIDGE
					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_रिक्तानि_OPTION(token, "connection");

					/* Check for existing bridge name. */
					प्रति(i=0; i<config->bridge_गणनम्; i++){
						यदि(!वाक्यं_तुलय(config->bridges[i]->name, token)){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Duplicate bridge name '%s'.", token);
							प्रत्यागम MOSQ_ERR_INVAL;
						}
					}

					संरचना mosquitto__bridge **bridges_new = mosquitto_realloc(config->bridges, (size_t)(config->bridge_गणनम्+1)*परिमाण(संरचना mosquitto__bridge *));
					यदि(!bridges_new){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
						प्रत्यागम MOSQ_ERR_NOMEM;
					}
					config->bridges = bridges_new;
					cur_bridge = mosquitto_malloc(परिमाण(संरचना mosquitto__bridge));
					यदि(!cur_bridge){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
						प्रत्यागम MOSQ_ERR_NOMEM;
					}

					config->bridge_गणनम्++;
					config->bridges[config->bridge_गणनम्-1] = cur_bridge;

					स्मृतिं_पूरय(cur_bridge, 0, परिमाण(संरचना mosquitto__bridge));
					cur_bridge->name = mosquitto_strdup(token);
					यदि(!cur_bridge->name){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
						प्रत्यागम MOSQ_ERR_NOMEM;
					}
					cur_bridge->keepalive = 60;
					cur_bridge->notifications = true;
					cur_bridge->notifications_local_only = false;
					cur_bridge->start_type = bst_automatic;
					cur_bridge->idle_timeout = 60;
					cur_bridge->restart_timeout = 0;
					cur_bridge->backoff_आधारः = 5 * 1000;
					cur_bridge->backoff_cap = 30 * 1000;
					cur_bridge->stable_connection_period = 0;
					cur_bridge->देहली = 10;
					cur_bridge->try_वैयक्तिकम् = true;
					cur_bridge->attempt_unsubscribe = true;
					cur_bridge->protocol_संस्करणम् = mosq_p_mqtt311;
					cur_bridge->primary_retry_sock = INVALID_SOCKET;
					cur_bridge->outgoing_retain = true;
					cur_bridge->clean_start_local = -1;
					cur_bridge->reload_type = brt_lazy;
					cur_bridge->max_topic_alias = 10;
#यदि_परिभाषितम् WITH_TCP_USER_TIMEOUT
					cur_bridge->tcp_user_timeout = -1;
#यदि_अन्तः
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "connection_messages")){
					यदि(conf__parse_bool(&token, token, &config->connection_messages, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "crlfile")){
#यदि_परिभाषितम् WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->crlfile);
					यदि(conf__parse_string(&token, "crlfile", &cur_listener->crlfile, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "dhparamfile")){
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: dhparamfile is no longer required.");
				}अन्यथा यदि(!वाक्यं_तुलय(token, "disable_client_cert_date_checks")){
#यदि_परिभाषितम् WITH_TLS
					REQUIRE_LISTENER(token);
					यदि(conf__parse_bool(&token, "disable_client_cert_date_checks", &cur_listener->disable_client_cert_date_checks, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "enable_control_api")){
#यदि_परिभाषितम् WITH_CONTROL
					यदि(conf__parse_bool(&token, "enable_control_api", &config->enable_control_api, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: $CONTROL support not available (enable_control_api).");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "enable_proxy_protocol")){
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(रिक्त, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					प्रत्यागम MOSQ_ERR_INVAL;
#यदि_अन्तः
					REQUIRE_LISTENER(token);
					यदि(conf__parse_int(&token, "enable_proxy_protocol", &cur_listener->enable_proxy_protocol, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(cur_listener->enable_proxy_protocol < 1 || cur_listener->enable_proxy_protocol > 2){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: enable_proxy_protocol must be 1 or 2.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "global_max_clients")){
					यदि(conf__parse_int(&token, "global_max_clients", &config->global_max_clients, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "global_max_connections")){
					यदि(conf__parse_int(&token, "global_max_connections", &config->global_max_connections, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "http_dir")){
#यदि defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
					यदि(reload){
						अनुवर्त;        /* Not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					यदि(conf__parse_string(&token, "http_dir", &cur_listener->http_dir, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#यदि_परिभाषितम् WIN32
					वर्ण *http_dir_canonical = _fullpath(रिक्त, cur_listener->http_dir, 0);
					ध्रुव वर्ण sep = '\\';
#अन्यथा
					वर्ण *http_dir_canonical = realpath(cur_listener->http_dir, रिक्त);
					ध्रुव वर्ण sep = '/';
#यदि_अन्तः
					यदि(!http_dir_canonical){
						प्रत्यागम MOSQ_ERR_NOMEM;
					}
					size_t http_dir_len = वाक्यदैर्घ्यम्(http_dir_canonical) + परिमाण(sep) + 1;
					वर्ण *http_dir_canonical_sep = mosquitto_calloc(http_dir_len, परिमाण(वर्ण));
					यदि(!http_dir_canonical_sep){
						मुक्त(http_dir_canonical);
						प्रत्यागम MOSQ_ERR_NOMEM;
					}
					snprintf(http_dir_canonical_sep, http_dir_len, "%s%c", http_dir_canonical, sep);
					मुक्त(http_dir_canonical);
					mosquitto_FREE(cur_listener->http_dir);
					cur_listener->http_dir = http_dir_canonical_sep;
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: http_dir support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "idle_timeout")){
#यदि_परिभाषितम् WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_int(&token, "idle_timeout", &cur_bridge->idle_timeout, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(cur_bridge->idle_timeout < 1){
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "idle_timeout interval too low, using 1 second.");
						cur_bridge->idle_timeout = 1;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "include_dir")){
					यदि(level == 0){
						वर्ण **files;
						पूर्णाङ्क file_गणनम्;

						/* Only process include_dir from the main config file. */
						token = strtok_r(रिक्त, "", &saveptr);
						REQUIRE_NON_रिक्तानि_OPTION(token, "include_dir");

						rc = config__get_dir_files(token, &files, &file_गणनम्);
						यदि(rc){
							प्रत्यागम rc;
						}

						प्रति(i=0; i<file_गणनम्; i++){
							log__printf(रिक्त, MOSQ_LOG_INFO, "Loading config file '%s'", files[i]);

							rc = config__read_file(config, reload, files[i], cr, level+1, &lineno_ext);
							यदि(rc){
								यदि(lineno_ext > 0){
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
						यदि(rc){
							प्रत्यागम rc;    /* This returns if config__read_file() fails above */
						}
					}अन्यथा{
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: The include_dir option is only valid in the main configuration file.");
						प्रत्यागम 1;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "keepalive_interval")){
#यदि_परिभाषितम् WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_int(&token, "keepalive_interval", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(tmp_int > UINT16_MAX){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Bridge keepalive value too high.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 5){
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "keepalive interval too low, using 5 seconds.");
						tmp_int = 5;
					}
					cur_bridge->keepalive = (uint16_t)tmp_int;
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "keyfile")){
#यदि_परिभाषितम् WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					mosquitto_FREE(cur_listener->keyfile);
					यदि(conf__parse_string(&token, "keyfile", &cur_listener->keyfile, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "listener")){
					config->local_only = false;

					यदि(conf__parse_int(&token, "listener port", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#यदि_परिभाषितम् WITH_UNIX_SOCKETS
					यदि(tmp_int < 0 || tmp_int > UINT16_MAX){
#अन्यथा
					यदि(tmp_int < 1 || tmp_int > UINT16_MAX){
#यदि_अन्तः
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						प्रत्यागम MOSQ_ERR_INVAL;
					}

					/* Look for bind address / unix socket path */
					token = strtok_r(रिक्त, " ", &saveptr);
					यदि(token != रिक्त && token[0] == '#'){
						token = रिक्त;
					}

					यदि(tmp_int == 0 && token == रिक्त){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: A listener with port 0 must provide a Unix socket path.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}

					यदि(reload){
						/* We reload listeners settings based on port number/unix socket path.
						 * If the port number/unix path doesn't already exist, exit with a complaint. */
						cur_listener = रिक्त;
#यदि_परिभाषितम् WITH_UNIX_SOCKETS
						यदि(tmp_int == 0){
							प्रति(i=0; i<config->listener_गणनम्; i++){
								यदि(config->listeners[i].unix_socket_मार्गः != रिक्त
										&& वाक्यं_तुलय(config->listeners[i].unix_socket_मार्गः, token) == 0){

									cur_listener = &config->listeners[i];
									भङ्ग;
								}
							}
						}अन्यथा
#यदि_अन्तः
						{
							प्रति(i=0; i<config->listener_गणनम्; i++){
								यदि(config->listeners[i].पत्तनम् == tmp_int){
									/* Now check we have a matching bind address, if defined */
									यदि(config->listeners[i].host){
										यदि(token && !वाक्यं_तुलय(config->listeners[i].host, token)){
											/* They both have a bind address, and they match */
											cur_listener = &config->listeners[i];
											भङ्ग;
										}
									}अन्यथा{
										यदि(token == रिक्त){
											/* Neither this config nor the new config have a bind address,
											 * so they match. */
											cur_listener = &config->listeners[i];
											भङ्ग;
										}
									}
								}
							}
						}
						यदि(!cur_listener){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: It is not currently possible to add/remove listeners when reloading the config file.");
							प्रत्यागम MOSQ_ERR_INVAL;
						}
					}अन्यथा{
						यदि(config_सङ्कलय_listener(config)){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
							प्रत्यागम MOSQ_ERR_NOMEM;
						}
						cur_listener = &config->listeners[config->listener_गणनम्-1];
					}

					listener_समुच्चयः_defaults(cur_listener);
					cur_listener->पत्तनम् = (uint16_t)tmp_int;

					mosquitto_FREE(cur_listener->host);

#यदि_परिभाषितम् WITH_UNIX_SOCKETS
					mosquitto_FREE(cur_listener->unix_socket_मार्गः);
#यदि_अन्तः

					यदि(token){
#यदि_परिभाषितम् WITH_UNIX_SOCKETS
						यदि(cur_listener->पत्तनम् == 0){
							cur_listener->unix_socket_मार्गः = mosquitto_strdup(token);
						}अन्यथा
#यदि_अन्तः
						{
							cur_listener->host = mosquitto_strdup(token);
						}
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "listener_allow_anonymous")){
					REQUIRE_LISTENER(token);
					यदि(conf__parse_bool(&token, "listener_allow_anonymous", (bool *)&cur_listener->security_options->allow_anonymous, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "listener_auto_id_prefix")){
					REQUIRE_LISTENER(token);
					यदि(conf__parse_string(&token, "listener_auto_id_prefix", &cur_listener->security_options->auto_id_उपसर्गः, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(cur_listener->security_options->auto_id_उपसर्गः){
						यदि(वाक्यदैर्घ्यम्(cur_listener->security_options->auto_id_उपसर्गः) > 50){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: 'listener_auto_id_prefix' length must be <= 50.");
							प्रत्यागम MOSQ_ERR_INVAL;
						}
						cur_listener->security_options->auto_id_उपसर्गः_len = (uint16_t)वाक्यदैर्घ्यम्(cur_listener->security_options->auto_id_उपसर्गः);
					}अन्यथा{
						cur_listener->security_options->auto_id_उपसर्गः_len = 0;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "local_clientid")){
#यदि_परिभाषितम् WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_string(&token, "bridge local clientd", &cur_bridge->local_clientid, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "local_password")){
#यदि_परिभाषितम् WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_string(&token, "bridge local_password", &cur_bridge->local_password, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "local_username")){
#यदि_परिभाषितम् WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_string(&token, "bridge local_username", &cur_bridge->local_username, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "log_dest")){
					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_रिक्तानि_OPTION(token, "log_dest");

					cr->log_dest_समुच्चयः = 1;
					यदि(!वाक्यं_तुलय(token, "none")){
						cr->log_dest = MQTT3_LOG_NONE;
					}अन्यथा यदि(!वाक्यं_तुलय(token, "syslog")){
						cr->log_dest |= MQTT3_LOG_SYSLOG;
					}अन्यथा यदि(!वाक्यं_तुलय(token, "stdout")){
						cr->log_dest |= MQTT3_LOG_STDOUT;
					}अन्यथा यदि(!वाक्यं_तुलय(token, "stderr")){
						cr->log_dest |= MQTT3_LOG_STDERR;
					}अन्यथा यदि(!वाक्यं_तुलय(token, "topic")){
						cr->log_dest |= MQTT3_LOG_TOPIC;
					}अन्यथा यदि(!वाक्यं_तुलय(token, "dlt")){
						cr->log_dest |= MQTT3_LOG_DLT;
#यदि_परिभाषितम् ANDROID
					}अन्यथा यदि(!वाक्यं_तुलय(token, "android")){
						cr->log_dest |= MQTT3_LOG_ANDROID;
#यदि_अन्तः
					}अन्यथा यदि(!वाक्यं_तुलय(token, "file")){
						यदि(config->log_fptr || config->log_file){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Duplicate \"log_dest file\" value.");
							प्रत्यागम MOSQ_ERR_INVAL;
						}
						/* Get remaining string. */
						token = saveptr;
						यदि(token && token[0]){
							यावत्(token[0] == ' ' || token[0] == '\t'){
								token++;
							}
						}
						/* Duplicate "token" check here saves a log__printf() */
						यदि(token && token[0]){
							config->log_file = mosquitto_strdup(token);
							यदि(!config->log_file){
								log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
								प्रत्यागम MOSQ_ERR_NOMEM;
							}
							cr->log_dest |= MQTT3_LOG_FILE;
						}अन्यथा{
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty \"log_dest file\" value in configuration.");
							प्रत्यागम MOSQ_ERR_INVAL;
						}
						cr->log_dest |= MQTT3_LOG_FILE;
					}अन्यथा{
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'log_dest' value (%s).", token);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#यदि defined(WIN32) || defined(__CYGWIN__)
					यदि(service_handle){
						यदि(cr->log_dest == MQTT3_LOG_STDOUT || cr->log_dest == MQTT3_LOG_STDERR){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Cannot log to stdout/stderr when running as a Windows service.");
							प्रत्यागम MOSQ_ERR_INVAL;
						}
					}
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "log_facility")){
#यदि defined(WIN32) || defined(__CYGWIN__)
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: log_facility not supported on Windows.");
#अन्यथा
					यदि(conf__parse_int(&token, "log_facility", &tmp_int, &saveptr)){
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
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "log_timestamp")){
					यदि(conf__parse_bool(&token, token, &config->log_timestamp, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "log_timestamp_format")){
					यदि(conf__parse_string(&token, token, &config->log_timestamp_format, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "log_type")){
					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_रिक्तानि_OPTION(token, "log_type");

					cr->log_type_समुच्चयः = 1;
					यदि(!वाक्यं_तुलय(token, "none")){
						cr->log_type = MOSQ_LOG_NONE;
					}अन्यथा यदि(!वाक्यं_तुलय(token, "information")){
						cr->log_type |= MOSQ_LOG_INFO;
					}अन्यथा यदि(!वाक्यं_तुलय(token, "notice")){
						cr->log_type |= MOSQ_LOG_NOTICE;
					}अन्यथा यदि(!वाक्यं_तुलय(token, "warning")){
						cr->log_type |= MOSQ_LOG_WARNING;
					}अन्यथा यदि(!वाक्यं_तुलय(token, "error")){
						cr->log_type |= MOSQ_LOG_ERR;
					}अन्यथा यदि(!वाक्यं_तुलय(token, "debug")){
						cr->log_type |= MOSQ_LOG_DEBUG;
					}अन्यथा यदि(!वाक्यं_तुलय(token, "subscribe")){
						cr->log_type |= MOSQ_LOG_SUBSCRIBE;
					}अन्यथा यदि(!वाक्यं_तुलय(token, "unsubscribe")){
						cr->log_type |= MOSQ_LOG_UNSUBSCRIBE;
					}अन्यथा यदि(!वाक्यं_तुलय(token, "internal")){
						cr->log_type |= MOSQ_LOG_आन्तरिकम्;
#यदि_परिभाषितम् WITH_WEBSOCKETS
					}अन्यथा यदि(!वाक्यं_तुलय(token, "websockets")){
						cr->log_type |= MOSQ_LOG_WEBSOCKETS;
#यदि_अन्तः
					}अन्यथा यदि(!वाक्यं_तुलय(token, "all")){
						cr->log_type = MOSQ_LOG_ALL;
					}अन्यथा{
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'log_type' value (%s).", token);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "max_connections")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					यदि(conf__parse_int(&token, token, &cur_listener->max_connections, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(cur_listener->max_connections < 0){
						cur_listener->max_connections = -1;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "maximum_qos") || !वाक्यं_तुलय(token, "max_qos")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					यदि(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 0 || tmp_int > 2){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: 'max_qos' must be between 0 and 2 inclusive.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					cur_listener->max_qos = (uint8_t)tmp_int;
				}अन्यथा यदि(!वाक्यं_तुलय(token, "max_inflight_bytes")){
					यदि(conf__parse_int(&token, "max_inflight_bytes", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_inflight_अष्टकानि = (size_t)tmp_int;
				}अन्यथा यदि(!वाक्यं_तुलय(token, "max_inflight_messages")){
					यदि(conf__parse_int(&token, "max_inflight_messages", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 0 || tmp_int == UINT16_MAX){
						tmp_int = 0;
					}अन्यथा यदि(tmp_int > UINT16_MAX){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: 'max_inflight_messages' must be <= 65535.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					config->max_inflight_messages = (uint16_t)tmp_int;
				}अन्यथा यदि(!वाक्यं_तुलय(token, "max_keepalive")){
					यदि(conf__parse_int(&token, "max_keepalive", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'max_keepalive' value (%d).", tmp_int);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					config->max_keepalive = (uint16_t)tmp_int;
				}अन्यथा यदि(!वाक्यं_तुलय(token, "max_packet_size")){
					यदि(conf__parse_int(&token, "max_packet_size", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 20){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: 'max_packet_size' must be >= 20.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					config->max_packet_आकार = (uint32_t)tmp_int;
				}अन्यथा यदि(!वाक्यं_तुलय(token, "max_queued_bytes")){
					यदि(conf__parse_int(&token, "max_queued_bytes", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_अष्टकानि = (size_t)tmp_int;
				}अन्यथा यदि(!वाक्यं_तुलय(token, "max_queued_messages")){
					यदि(conf__parse_int(&token, "max_queued_messages", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 0){
						tmp_int = 0;
					}
					config->max_queued_messages = tmp_int;
				}अन्यथा यदि(!वाक्यं_तुलय(token, "memory_limit")){
					ssize_t lim;
					यदि(conf__parse_ssize_t(&token, "memory_limit", &lim, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(lim < 0){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'memory_limit' value (%ld).", lim);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					mosquitto_स्मृतिः_समुच्चयः_सीमा((size_t)lim);
				}अन्यथा यदि(!वाक्यं_तुलय(token, "message_size_limit")){
					log__printf(रिक्त, MOSQ_LOG_NOTICE, "Note: It is recommended to replace `message_size_limit` with `max_packet_size`.");
					यदि(conf__parse_int(&token, "message_size_limit", (पूर्णाङ्क *)&config->सन्देशः_आकार_सीमा, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(config->सन्देशः_आकार_सीमा > MQTT_MAX_PAYLOAD){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'message_size_limit' value (%u).", config->सन्देशः_आकार_सीमा);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "mount_point")){
					REQUIRE_LISTENER(token);
					mosquitto_FREE(cur_listener->mount_point);
					यदि(conf__parse_string(&token, "mount_point", &cur_listener->mount_point, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(mosquitto_pub_topic_check(cur_listener->mount_point) != MOSQ_ERR_SUCCESS){
						log__printf(रिक्त, MOSQ_LOG_ERR,
								"Error: Invalid 'mount_point' value (%s). Does it contain a wildcard character?",
								cur_listener->mount_point);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "notifications")){
#यदि_परिभाषितम् WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_bool(&token, "notifications", &cur_bridge->notifications, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "notifications_local_only")){
#यदि_परिभाषितम् WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_bool(&token, "notifications_local_only", &cur_bridge->notifications_local_only, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "notification_topic")){
#यदि_परिभाषितम् WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_string(&token, "notification_topic", &cur_bridge->notification_topic, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "password") || !वाक्यं_तुलय(token, "remote_password")){
#यदि_परिभाषितम् WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_string(&token, "bridge remote_password", &cur_bridge->remote_password, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "password_file")){
					REQUIRE_LISTENER_IF_प्रत्येकम्_LISTENER(token);
					conf_समुच्चयः_cur_security_options(config, &cur_listener, &cur_security_options, token);
					mosquitto_FREE(cur_security_options->password_दत्त.password_file);
					यदि(conf__parse_string(&token, "password_file", &cur_security_options->password_दत्त.password_file, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "per_listener_settings")){
					OPTION_DEPRECATED(token, "Please see the documentation for how to achieve the same effect.");
					यदि(config->प्रत्येकम्_listener_settings){
						/* Once this is set, don't let it be unset. It should be the first config option ideally. */
						अनुवर्त;
					}
					यदि(conf__parse_bool(&token, "per_listener_settings", &config->प्रत्येकम्_listener_settings, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(cur_security_options && config->प्रत्येकम्_listener_settings){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: per_listener_settings must be set before any other security settings.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "persistence") || !वाक्यं_तुलय(token, "retained_persistence")){
					यदि(conf__parse_bool(&token, token, &config->persistence, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "persistence_file")){
					यदि(conf__parse_string(&token, "persistence_file", &config->persistence_file, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "persistence_location")){
					यदि(conf__parse_string(&token, "persistence_location", &config->persistence_location, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "persistent_client_expiration")){
					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_रिक्तानि_OPTION(token, "persistent_client_expiration");

					time_t expiration_mult;

					विकल्प(token[वाक्यदैर्घ्यम्(token)-1]){
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
					token[वाक्यदैर्घ्यम्(token)-1] = '\0';
					config->persistent_client_expiration = atoi(token)*expiration_mult;
					यदि(config->persistent_client_expiration <= 0){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'persistent_client_expiration' duration in configuration.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "pid_file")){
					यदि(reload){
						अनुवर्त;        /* pid file not valid for reloading. */
					}
					यदि(conf__parse_string(&token, "pid_file", &config->pid_file, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "port")){
					OPTION_DEPRECATED(token, "Please use 'listener' instead.");
					config->local_only = false;
					यदि(reload){
						अनुवर्त;        /* Listeners not valid for reloading. */

					}
					यदि(config__create_default_listener(config, token)){
						प्रत्यागम MOSQ_ERR_NOMEM;
					}
					cur_listener = config->default_listener;

					यदि(config->default_listener->पत्तनम्){
						log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Default listener port specified multiple times. Only the latest will be used.");
					}
					यदि(conf__parse_int(&token, "port", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 1 || tmp_int > UINT16_MAX){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'port' value (%d).", tmp_int);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					config->default_listener->पत्तनम् = (uint16_t)tmp_int;
				}अन्यथा यदि(!वाक्यं_तुलय(token, "protocol")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_रिक्तानि_OPTION(token, "protocol");

					यदि(!वाक्यं_तुलय(token, "mqtt")){
						cur_listener->protocol = mp_mqtt;
						/*
						}else if(!strcmp(token, "mqttsn")){
						    cur_listener->protocol = mp_mqttsn;
						*/
					}अन्यथा यदि(!वाक्यं_तुलय(token, "websockets")){
#यदि_परिभाषितम् WITH_WEBSOCKETS
						cur_listener->protocol = mp_websockets;
#अन्यथा
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Websockets support not available.");
						प्रत्यागम MOSQ_ERR_INVAL;
#यदि_अन्तः
					}अन्यथा यदि(!वाक्यं_तुलय(token, "http_api")){
#यदि_परिभाषितम् WITH_HTTP_API
						cur_listener->protocol = mp_http_api;
#अन्यथा
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: HTTP API support not available.");
						प्रत्यागम MOSQ_ERR_INVAL;
#यदि_अन्तः
					}अन्यथा{
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'protocol' value (%s).", token);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "proxy_protocol_v2_require_tls")){
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					log__printf(रिक्त, MOSQ_LOG_ERR, "Error: PROXY support not available with libwebsockets.");
					प्रत्यागम MOSQ_ERR_INVAL;
#यदि_अन्तः
					REQUIRE_LISTENER(token);
					यदि(conf__parse_bool(&token, "proxy_protocol_v2_require_tls", &cur_listener->proxy_protocol_v2_require_tls, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "psk_file")){
#यदि_परिभाषितम् FINAL_WITH_TLS_PSK
					REQUIRE_LISTENER_IF_प्रत्येकम्_LISTENER(token);
					conf_समुच्चयः_cur_security_options(config, &cur_listener, &cur_security_options, token);
					यदि(cur_listener && cur_listener->certfile){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Cannot use both certificate and psk encryption in a single listener.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(cur_security_options->psk_file);
					यदि(conf__parse_string(&token, "psk_file", &cur_security_options->psk_file, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "psk_hint")){
#यदि_परिभाषितम् FINAL_WITH_TLS_PSK
					यदि(reload){
						अनुवर्त;        /* PSK file not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					यदि(conf__parse_string(&token, "psk_hint", &cur_listener->psk_hint, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS/TLS-PSK support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "queue_qos0_messages")){
					यदि(conf__parse_bool(&token, token, &config->queue_qos0_messages, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "require_certificate")){
#यदि_परिभाषितम् WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					यदि(conf__parse_bool(&token, "require_certificate", &cur_listener->require_certificate, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "restart_timeout")){
#यदि_परिभाषितम् WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					cur_bridge->backoff_cap = 0; /* set backoff to constant mode, unless cap is specified further down */
					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_रिक्तानि_OPTION(token, "restart_timeout");

					cur_bridge->restart_timeout = atoi(token);
					cur_bridge->backoff_आधारः = 0;
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
						cur_bridge->backoff_आधारः = cur_bridge->restart_timeout;
						cur_bridge->backoff_cap = atoi(token);
						यदि(cur_bridge->backoff_cap < cur_bridge->backoff_आधारः){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: backoff cap is lower than the base in restart_timeout.");
							प्रत्यागम MOSQ_ERR_INVAL;
						}अन्यथा यदि(cur_bridge->backoff_cap > 7200){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: backoff cap too high, using 7200 seconds.");
							cur_bridge->backoff_cap = 7200;
						}

						token = strtok_r(रिक्त, " ", &saveptr);
						यदि(token){
							cur_bridge->stable_connection_period = atoi(token);
							यदि(cur_bridge->stable_connection_period < 0){
								log__printf(रिक्त, MOSQ_LOG_ERR, "Error: stable connection period cannot be negative.");
								प्रत्यागम MOSQ_ERR_INVAL;
							}
						}
					}
					cur_bridge->restart_timeout *= 1000; /* backoff is tracked in ms */
					cur_bridge->backoff_आधारः *= 1000;
					cur_bridge->backoff_cap *= 1000;
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "retain_available")){
					यदि(conf__parse_bool(&token, token, &config->retain_उपलब्धम्, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "retain_expiry_interval")){
					यदि(conf__parse_int(&token, token, &config->retain_expiry_interval, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(config->retain_expiry_interval < 1){
						log__printf(रिक्त, MOSQ_LOG_WARNING, "Error: retain_expiry_interval must be >= 1.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}अन्यथा यदि(config->retain_expiry_interval > 10000000){
						log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: retain_expiry_interval being capped at 19 years.");
						config->retain_expiry_interval = 10000000;
					}
					config->retain_expiry_interval *= 60;
				}अन्यथा यदि(!वाक्यं_तुलय(token, "retry_interval")){
					OPTION_UNAVAILABLE(token);
				}अन्यथा यदि(!वाक्यं_तुलय(token, "round_robin")){
#यदि_परिभाषितम् WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_bool(&token, "round_robin", &cur_bridge->आवर्तनम्_robin, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "set_tcp_nodelay")){
					यदि(conf__parse_bool(&token, "set_tcp_nodelay", &config->समुच्चयः_tcp_nodelay, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "start_type")){
#यदि_परिभाषितम् WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_रिक्तानि_OPTION(token, "start_type");

					यदि(!वाक्यं_तुलय(token, "automatic")){
						cur_bridge->start_type = bst_automatic;
					}अन्यथा यदि(!वाक्यं_तुलय(token, "lazy")){
						cur_bridge->start_type = bst_lazy;
					}अन्यथा यदि(!वाक्यं_तुलय(token, "manual")){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Manual start_type not supported.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}अन्यथा यदि(!वाक्यं_तुलय(token, "once")){
						cur_bridge->start_type = bst_once;
					}अन्यथा{
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'start_type' value in configuration (%s).", token);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "socket_domain")){
					यदि(reload){
						अनुवर्त;        /* socket_domain not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_रिक्तानि_OPTION(token, "start_type");

					यदि(!वाक्यं_तुलय(token, "ipv4")){
						cur_listener->socket_domain = AF_INET;
					}अन्यथा यदि(!वाक्यं_तुलय(token, "ipv6")){
						cur_listener->socket_domain = AF_INET6;
					}अन्यथा{
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'socket_domain' value '%s' in configuration.", token);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "sys_interval")){
					यदि(conf__parse_int(&token, "sys_interval", &config->sys_interval, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(config->sys_interval < 0 || config->sys_interval > 65535){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'sys_interval' value (%d).", config->sys_interval);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "threshold")){
#यदि_परिभाषितम् WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_int(&token, "threshold", &cur_bridge->देहली, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(cur_bridge->देहली < 1){
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "threshold too low, using 1 message.");
						cur_bridge->देहली = 1;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "tls_engine")){
#यदि_परिभाषितम् WITH_TLS
					यदि(reload){
						अनुवर्त;        /* tls_engine not valid for reloading. */
					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					यदि(conf__parse_string(&token, "tls_engine", &cur_listener->tls_engine, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "tls_engine_kpass_sha1")){
#यदि_परिभाषितम् WITH_TLS
					यदि(reload){
						अनुवर्त;        /* tls_engine not valid for reloading. */

					}
					वर्ण *kpass_sha = रिक्त, *kpass_sha_bin = रिक्त;
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					यदि(conf__parse_string(&token, "tls_engine_kpass_sha1", &kpass_sha, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(mosquitto__hex2bin_sha1(kpass_sha, (अचिह्नित वर्ण **)&kpass_sha_bin) != MOSQ_ERR_SUCCESS){
						mosquitto_FREE(kpass_sha);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					mosquitto_free(cur_listener->tls_engine_kpass_sha1);
					cur_listener->tls_engine_kpass_sha1 = kpass_sha_bin;
					mosquitto_FREE(kpass_sha);
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "tls_keyform")){
#यदि_परिभाषितम् WITH_TLS
					यदि(reload){
						अनुवर्त;        /* tls_engine not valid for reloading. */

					}
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					वर्ण *keyform = रिक्त;
					यदि(conf__parse_string(&token, "tls_keyform", &keyform, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					cur_listener->tls_keyform = mosq_k_pem;
					यदि(!वाक्यं_तुलय(keyform, "engine")){
						cur_listener->tls_keyform = mosq_k_engine;
					}
					mosquitto_FREE(keyform);
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "tls_version")){
#यदि defined(WITH_TLS)
					यदि(reload){
						अनुवर्त;        /* tls_version not valid for reloading. */
					}
					REQUIRE_LISTENER(token);
					यदि(conf__parse_string(&token, "tls_version", &cur_listener->tls_संस्करणम्, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "topic")){
#यदि_परिभाषितम् WITH_BRIDGE
					वर्ण *topic = रिक्त;
					गणना mosquitto__bridge_direction direction = bd_बहिर्गमनम्;
					uint8_t qos = 0;
					वर्ण *local_उपसर्गः = रिक्त, *remote_उपसर्गः = रिक्त;

					REQUIRE_BRIDGE(token);

					token = strtok_r(रिक्त, " ", &saveptr);
					REQUIRE_NON_रिक्तानि_OPTION(token, "topic");

					// Check if the topic is quoted (e.g. for spaces within topic names), but not the
					// special case of ""
					यदि(token[0] == '"' && token [1] != '"'){
						यदि(strchr(saveptr, '"') == रिक्त){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Missing closing quote in topic value (%s).", saveptr);
							प्रत्यागम MOSQ_ERR_INVAL;
						}

						वर्ण *topic_अन्तर्_quotes = strtok_r(रिक्त, "\"", &saveptr);
						size_t tlen = 1;
						यदि(topic_अन्तर्_quotes){
							tlen = वाक्यदैर्घ्यम्(topic_अन्तर्_quotes);
						}

						topic = mosquitto_malloc(वाक्यदैर्घ्यम्(token) + tlen + 1);
						यदि(!topic){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
							प्रत्यागम MOSQ_ERR_NOMEM;
						}

						वाक्यं_प्रतिलिख(topic, token + 1);
						वाक्यं_योजय(topic, " ");
						यदि(topic_अन्तर्_quotes){
							वाक्यं_योजय(topic, topic_अन्तर्_quotes);
						}
					}अन्यथा{
						topic = mosquitto_strdup(token);
						यदि(!topic){
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
							प्रत्यागम MOSQ_ERR_NOMEM;
						}
					}

					token = strtok_r(रिक्त, " ", &saveptr);
					यदि(token){
						यदि(!strcasecmp(token, "out")){
							direction = bd_बहिर्गमनम्;
						}अन्यथा यदि(!strcasecmp(token, "in")){
							direction = bd_अन्तर्;
						}अन्यथा यदि(!strcasecmp(token, "both")){
							direction = bd_both;
						}अन्यथा{
							log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge topic direction '%s'.", token);
							mosquitto_FREE(topic);
							प्रत्यागम MOSQ_ERR_INVAL;
						}
						token = strtok_r(रिक्त, " ", &saveptr);
						यदि(token){
							यदि(token[0] == '#'){
								(शून्य)strtok_r(रिक्त, "", &saveptr);
							}
							qos = (uint8_t)atoi(token);
							यदि(qos > 2){
								log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid bridge QoS level '%s'.", token);
								mosquitto_FREE(topic);
								प्रत्यागम MOSQ_ERR_INVAL;
							}

							token = strtok_r(रिक्त, " ", &saveptr);
							यदि(token){
								यदि(!वाक्यं_तुलय(token, "\"\"") || token[0] == '#'){
									local_उपसर्गः = रिक्त;
									यदि(token[0] == '#'){
										(शून्य)strtok_r(रिक्त, "", &saveptr);
									}
								}अन्यथा{
									local_उपसर्गः = token;
								}

								token = strtok_r(रिक्त, " ", &saveptr);
								यदि(token){
									यदि(!वाक्यं_तुलय(token, "\"\"") || token[0] == '#'){
										remote_उपसर्गः = रिक्त;
									}अन्यथा{
										remote_उपसर्गः = token;
									}
								}
							}
						}
					}

					यदि(bridge_सङ्कलय_topic(cur_bridge, topic, direction, qos, local_उपसर्गः, remote_उपसर्गः)){
						mosquitto_FREE(topic);
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					mosquitto_FREE(topic);
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "max_topic_alias")){
					REQUIRE_LISTENER(token);
					यदि(conf__parse_int(&token, "max_topic_alias", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias' value in configuration.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias = (uint16_t)tmp_int;
				}अन्यथा यदि(!वाक्यं_तुलय(token, "max_topic_alias_broker")){
					REQUIRE_LISTENER(token);
					यदि(conf__parse_int(&token, "max_topic_alias_broker", &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid 'max_topic_alias_broker' value in configuration.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					cur_listener->max_topic_alias_broker = (uint16_t)tmp_int;
				}अन्यथा यदि(!वाक्यं_तुलय(token, "try_private")){
#यदि_परिभाषितम् WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_bool(&token, "try_private", &cur_bridge->try_वैयक्तिकम्, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "upgrade_outgoing_qos")){
					यदि(conf__parse_bool(&token, token, &config->upgrade_outgoing_qos, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "use_identity_as_username")){
#यदि_परिभाषितम् WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					यदि(conf__parse_bool(&token, "use_identity_as_username", &cur_listener->use_तटस्थः_as_username, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "use_subject_as_username")){
#यदि_परिभाषितम् WITH_TLS
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					यदि(conf__parse_bool(&token, "use_subject_as_username", &cur_listener->use_subject_as_username, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "user")){
					यदि(reload){
						अनुवर्त;        /* Drop privileges user not valid for reloading. */
					}
					mosquitto_FREE(config->user);
					यदि(conf__parse_string(&token, "user", &config->user, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "use_username_as_clientid")){
					REQUIRE_LISTENER_OR_DEFAULT_LISTENER(token);
					यदि(conf__parse_bool(&token, "use_username_as_clientid", &cur_listener->use_username_as_clientid, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
				}अन्यथा यदि(!वाक्यं_तुलय(token, "username") || !वाक्यं_तुलय(token, "remote_username")){
#यदि_परिभाषितम् WITH_BRIDGE
					REQUIRE_BRIDGE(token);
					यदि(conf__parse_string(&token, "bridge remote_username", &cur_bridge->remote_username, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Bridge support not available.");
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "websockets_log_level")){
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
					यदि(conf__parse_int(&token, "websockets_log_level", &config->websockets_log_level, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
#यदि_अन्तः
				}अन्यथा यदि(!वाक्यं_तुलय(token, "websockets_headers_size") || !वाक्यं_तुलय(token, "packet_buffer_size")){
					यदि(conf__parse_int(&token, token, &tmp_int, &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					यदि(tmp_int < 0 || tmp_int > UINT16_MAX){
						log__printf(रिक्त, MOSQ_LOG_WARNING, "Error: Packet buffer size must be between 0 and 65535 inclusive.");
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					config->packet_बफर_आकार = (uint16_t)tmp_int;
				}अन्यथा यदि(!वाक्यं_तुलय(token, "websockets_origin")){
#यदि_परिभाषितम् WITH_WEBSOCKETS
#  यदि LWS_LIBRARY_संस्करणम्_सङ्ख्या >= 3001000 || WITH_WEBSOCKETS == WS_IS_BUILTIN
					REQUIRE_LISTENER(token);
					ws_origins = mosquitto_realloc(cur_listener->ws_origins, परिमाण(वर्ण *)*(size_t)(cur_listener->ws_origin_गणनम्+1));
					यदि(ws_origins == रिक्त){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
						प्रत्यागम MOSQ_ERR_NOMEM;
					}
					ws_origins[cur_listener->ws_origin_गणनम्] = रिक्त;
					cur_listener->ws_origins = ws_origins;
					यदि(conf__parse_string(&token, "websockets_origin", &ws_origins[cur_listener->ws_origin_गणनम्], &saveptr)){
						प्रत्यागम MOSQ_ERR_INVAL;
					}
					cur_listener->ws_origin_गणनम्++;
#  अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: websockets_origin support not available, libwebsockets version is too old.");
#  endif
#अन्यथा
					log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Websockets support not available.");
#यदि_अन्तः
				}अन्यथा{
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
#यदि_अपरिभाषितम् WIN32
	DIR *dir;
#यदि_अन्तः

#यदि_अपरिभाषितम् WIN32
	dir = opendir(file);
	यदि(dir){
		closedir(dir);
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Config file '%s' is a directory.", file);
		प्रत्यागम 1;
	}
#यदि_अन्तः

	fptr = mosquitto_fopen(file, "rt", false);
	यदि(!fptr){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to open config file '%s'.", file);
		प्रत्यागम 1;
	}

	buflen = 1000;
	buf = mosquitto_malloc((size_t)buflen);
	यदि(!buf){
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

		यदि(l->enable_proxy_protocol == 2){
#यदि_परिभाषितम् WITH_TLS
			यदि(l->use_subject_as_username){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: use_subject_as_username cannot be used with `enable_proxy_protocol 2`.");
				प्रत्यागम MOSQ_ERR_INVAL;
			}

			यदि(l->certfile || l->keyfile){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: certfile and keyfile cannot be used with `enable_proxy_protocol 2`.");
				प्रत्यागम MOSQ_ERR_INVAL;
			}
#यदि_अन्तः
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क config__check(संरचना mosquitto__config *config)
{
	/* Checks that are easy to make after the config has been loaded. */

	ध्रुव वर्ण *id_उपसर्गः;
	पूर्णाङ्क id_उपसर्गः_len;
	यदि(config->security_options.auto_id_उपसर्गः){
		id_उपसर्गः = config->security_options.auto_id_उपसर्गः;
		id_उपसर्गः_len = config->security_options.auto_id_उपसर्गः_len;
	}अन्यथा{
		id_उपसर्गः = "auto-";
		id_उपसर्गः_len = (पूर्णाङ्क)वाक्यदैर्घ्यम्("auto-");
	}

	/* Default to auto_id_prefix = 'auto-' if none set. */
	प्रति(पूर्णाङ्क i=0; i<config->listener_गणनम्; i++){
		यदि(!config->listeners[i].security_options->auto_id_उपसर्गः){
			config->listeners[i].security_options->auto_id_उपसर्गः = mosquitto_strdup(id_उपसर्गः);
			यदि(!config->listeners[i].security_options->auto_id_उपसर्गः){
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
			config->listeners[i].security_options->auto_id_उपसर्गः_len = (uint16_t)id_उपसर्गः_len;
		}
	}

	प्रत्यागम config__check_proxy(config);
}

#यदि_परिभाषितम् WITH_BRIDGE


स्थिर पूर्णाङ्क config__check_bridges(संरचना mosquitto__config *config)
{
	संरचना mosquitto__bridge *bridge1, *bridge2;
	वर्ण hostname[256];
	size_t len;

	/* Check for bridge duplicate local_clientid, need to generate missing IDs
	 * first. */
	प्रति(पूर्णाङ्क i=0; i<config->bridge_गणनम्; i++){
		bridge1 = config->bridges[i];

		यदि(!bridge1->remote_clientid){
			यदि(!gethostname(hostname, 256)){
				len = वाक्यदैर्घ्यम्(hostname) + वाक्यदैर्घ्यम्(bridge1->name) + 2;
				bridge1->remote_clientid = mosquitto_malloc(len);
				यदि(!bridge1->remote_clientid){
					प्रत्यागम MOSQ_ERR_NOMEM;
				}
				snprintf(bridge1->remote_clientid, len, "%s.%s", hostname, bridge1->name);
			}अन्यथा{
				प्रत्यागम 1;
			}
		}

		यदि(!bridge1->local_clientid){
			len = वाक्यदैर्घ्यम्(bridge1->remote_clientid) + वाक्यदैर्घ्यम्("local.") + 2;
			bridge1->local_clientid = mosquitto_malloc(len);
			यदि(!bridge1->local_clientid){
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
			यदि(!वाक्यं_तुलय(bridge1->local_clientid, bridge2->local_clientid)){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Bridge local_clientid "
						"'%s' is not unique. Try changing or setting the "
						"local_clientid value for one of the bridges.",
						bridge1->local_clientid);
				प्रत्यागम MOSQ_ERR_INVAL;
			}
		}
	}

#यदि_परिभाषितम् WITH_TLS
	/* Check for missing TLS cafile/capath/certfile/keyfile */
	प्रति(पूर्णाङ्क i=0; i<config->listener_गणनम्; i++){
		bool cafile = !!config->listeners[i].cafile;
		bool capath = !!config->listeners[i].capath;
		bool certfile = !!config->listeners[i].certfile;
		bool keyfile = !!config->listeners[i].keyfile;

		यदि((certfile && !keyfile) || (!certfile && keyfile)){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Both certfile and keyfile must be provided to enable a TLS listener.");
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		यदि(cafile && !certfile){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: cafile specified without certfile and keyfile.");
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		यदि(capath && !certfile){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: capath specified without certfile and keyfile.");
			प्रत्यागम MOSQ_ERR_INVAL;
		}
	}
#यदि_अन्तः
	प्रत्यागम MOSQ_ERR_SUCCESS;
}
#यदि_अन्तः


स्थिर पूर्णाङ्क conf__parse_bool(वर्ण **token, ध्रुव वर्ण *name, bool *मानम्, वर्ण **saveptr)
{
	*token = strtok_r(रिक्त, " ", saveptr);
	यदि(*token){
		यदि(!वाक्यं_तुलय(*token, "false") || !वाक्यं_तुलय(*token, "0")){
			*मानम् = false;
		}अन्यथा यदि(!वाक्यं_तुलय(*token, "true") || !वाक्यं_तुलय(*token, "1")){
			*मानम् = true;
		}अन्यथा{
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Invalid '%s' value (%s).", name, *token);
			प्रत्यागम MOSQ_ERR_INVAL;
		}
	}अन्यथा{
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क conf__parse_int(वर्ण **token, ध्रुव वर्ण *name, पूर्णाङ्क *मानम्, वर्ण **saveptr)
{
	*token = strtok_r(रिक्त, " ", saveptr);
	यदि(*token){
		वर्ण *endptr = रिक्त;
		दीर्घ l = strtol(*token, &endptr, 0);
		यदि(endptr == *token || endptr[0] != '\0'){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: '%s' value not a number.", name);
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		यदि(l > अधिकतम_पूर्णाङ्कः || l < INT_MIN){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: '%s' value out of range.", name);
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		*मानम् = (पूर्णाङ्क)l;
	}अन्यथा{
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क conf__parse_ssize_t(वर्ण **token, ध्रुव वर्ण *name, ssize_t *मानम्, वर्ण **saveptr)
{
	*token = strtok_r(रिक्त, " ", saveptr);
	यदि(*token){
		*मानम् = atol(*token);
	}अन्यथा{
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क conf__parse_string(वर्ण **token, ध्रुव वर्ण *name, वर्ण **मानम्, वर्ण **saveptr)
{
	size_t tlen;

	*token = strtok_r(रिक्त, "", saveptr);
	यदि(*token){
		यदि(*मानम्){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Duplicate '%s' value in configuration.", name);
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		/* Deal with multiple spaces at the beginning of the string. */
		*token = mosquitto_trimblanks(*token);
		यदि(वाक्यदैर्घ्यम्(*token) == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
			प्रत्यागम MOSQ_ERR_INVAL;
		}

		tlen = वाक्यदैर्घ्यम्(*token);
		यदि(tlen > UINT16_MAX){
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		यदि(mosquitto_validate_utf8(*token, (uint16_t)tlen)){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Malformed UTF-8 in configuration.");
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		*मानम् = mosquitto_strdup(*token);
		यदि(!*मानम्){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
	}अन्यथा{
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty '%s' value in configuration.", name);
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}
