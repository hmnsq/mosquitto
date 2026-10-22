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
   Tatsuzo Osawa - Add epoll.
*/

#ifndef MOSQUITTO_BROKER_INTERNAL_H
#define MOSQUITTO_BROKER_INTERNAL_H

#ಅನ್ತರ್ಭಾವ "config.h"
#ಅನ್ತರ್ಭಾವ <stdio.h>

#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#  ಯದಿ LWS_LIBRARY_VERSION_ಸಂಖ್ಯೆ >= 3002000 && !defined(LWS_WITH_EXTERNAL_POLL)
#    warning "libwebsockets is not compiled with LWS_WITH_EXTERNAL_POLL support. Websocket performance will be unusable."
#  endif
#endif

#ifdef WITH_HTTP_API
#  include <microhttpd.h>
#endif

#ifdef __linux__
#define WITH_TCP_USER_TIMEOUT
#endif

#ಅನ್ತರ್ಭಾವ "mosquitto_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/broker.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/broker_plugin.h"
#ಅನ್ತರ್ಭಾವ "mosquitto.h"
#ಅನ್ತರ್ಭಾವ "logging_mosq.h"
#ಅನ್ತರ್ಭಾವ "tls_mosq.h"
#ಅನ್ತರ್ಭಾವ "uthash.h"
#ಅನ್ತರ್ಭಾವ "acl_file.h"
#ಅನ್ತರ್ಭಾವ "password_file.h"

#ifndef __GNUC__
#define __attribute__(attrib)
#endif

/* Log destinations */
#define MQTT3_LOG_NONE 0x00
#define MQTT3_LOG_SYSLOG 0x01
#define MQTT3_LOG_FILE 0x02
#define MQTT3_LOG_STDOUT 0x04
#define MQTT3_LOG_STDERR 0x08
#define MQTT3_LOG_TOPIC 0x10
#define MQTT3_LOG_DLT 0x20
#ifdef ANDROID
#define MQTT3_LOG_ANDROID 0x40
#endif
#define MQTT3_LOG_ALL 0xFF

#define CMD_PORT_LIMIT 10

ಪ್ರಕಾರನಿರ್ದೇಶ uint64_t dbid_t;

ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_plugin_init_v5)(mosquitto_plugin_id_t *, ಶೂನ್ಯ **, ಸಂರಚನಾ mosquitto_opt *, ಪೂರ್ಣಾಙ್ಕ);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_plugin_cleanup_v5)(ಶೂನ್ಯ *, ಸಂರಚನಾ mosquitto_opt *, ಪೂರ್ಣಾಙ್ಕ);

ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_auth_plugin_init_v4)(ಶೂನ್ಯ **, ಸಂರಚನಾ mosquitto_opt *, ಪೂರ್ಣಾಙ್ಕ);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_auth_plugin_cleanup_v4)(ಶೂನ್ಯ *, ಸಂರಚನಾ mosquitto_opt *, ಪೂರ್ಣಾಙ್ಕ);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_auth_plugin_security_init_v4)(ಶೂನ್ಯ *, ಸಂರಚನಾ mosquitto_opt *, ಪೂರ್ಣಾಙ್ಕ, bool);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_auth_plugin_security_cleanup_v4)(ಶೂನ್ಯ *, ಸಂರಚನಾ mosquitto_opt *, ಪೂರ್ಣಾಙ್ಕ, bool);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_auth_plugin_acl_check_v4)(ಶೂನ್ಯ *, ಪೂರ್ಣಾಙ್ಕ, ಸಂರಚನಾ mosquitto *, ಸಂರಚನಾ mosquitto_acl_ಸಂದೇಶ *);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_auth_plugin_unpwd_check_v4)(ಶೂನ್ಯ *, ಸಂರಚನಾ mosquitto *, ಧ್ರುವ ವರ್ಣ *, ಧ್ರುವ ವರ್ಣ *);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_auth_plugin_psk_ಕೀಲಿ_get_v4)(ಶೂನ್ಯ *, ಸಂರಚನಾ mosquitto *, ಧ್ರುವ ವರ್ಣ *, ಧ್ರುವ ವರ್ಣ *, ವರ್ಣ *, ಪೂರ್ಣಾಙ್ಕ);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_auth_plugin_auth_start_v4)(ಶೂನ್ಯ *, ಸಂರಚನಾ mosquitto *, ಧ್ರುವ ವರ್ಣ *, bool, ಧ್ರುವ ಶೂನ್ಯ *, uint16_t, ಶೂನ್ಯ **, uint16_t *);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_auth_plugin_auth_continue_v4)(ಶೂನ್ಯ *, ಸಂರಚನಾ mosquitto *, ಧ್ರುವ ವರ್ಣ *, ಧ್ರುವ ಶೂನ್ಯ *, uint16_t, ಶೂನ್ಯ **, uint16_t *);

ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_auth_plugin_init_v3)(ಶೂನ್ಯ **, ಸಂರಚನಾ mosquitto_opt *, ಪೂರ್ಣಾಙ್ಕ);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_auth_plugin_cleanup_v3)(ಶೂನ್ಯ *, ಸಂರಚನಾ mosquitto_opt *, ಪೂರ್ಣಾಙ್ಕ);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_auth_plugin_security_init_v3)(ಶೂನ್ಯ *, ಸಂರಚನಾ mosquitto_opt *, ಪೂರ್ಣಾಙ್ಕ, bool);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_auth_plugin_security_cleanup_v3)(ಶೂನ್ಯ *, ಸಂರಚನಾ mosquitto_opt *, ಪೂರ್ಣಾಙ್ಕ, bool);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_auth_plugin_acl_check_v3)(ಶೂನ್ಯ *, ಪೂರ್ಣಾಙ್ಕ, ಧ್ರುವ ಸಂರಚನಾ mosquitto *, ಸಂರಚನಾ mosquitto_acl_ಸಂದೇಶ *);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_auth_plugin_unpwd_check_v3)(ಶೂನ್ಯ *, ಧ್ರುವ ಸಂರಚನಾ mosquitto *, ಧ್ರುವ ವರ್ಣ *, ಧ್ರುವ ವರ್ಣ *);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_auth_plugin_psk_ಕೀಲಿ_get_v3)(ಶೂನ್ಯ *, ಧ್ರುವ ಸಂರಚನಾ mosquitto *, ಧ್ರುವ ವರ್ಣ *, ಧ್ರುವ ವರ್ಣ *, ವರ್ಣ *, ಪೂರ್ಣಾಙ್ಕ);

ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_auth_plugin_init_v2)(ಶೂನ್ಯ **, ಸಂರಚನಾ mosquitto_auth_opt *, ಪೂರ್ಣಾಙ್ಕ);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_auth_plugin_cleanup_v2)(ಶೂನ್ಯ *, ಸಂರಚನಾ mosquitto_auth_opt *, ಪೂರ್ಣಾಙ್ಕ);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_auth_plugin_security_init_v2)(ಶೂನ್ಯ *, ಸಂರಚನಾ mosquitto_auth_opt *, ಪೂರ್ಣಾಙ್ಕ, bool);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_auth_plugin_security_cleanup_v2)(ಶೂನ್ಯ *, ಸಂರಚನಾ mosquitto_auth_opt *, ಪೂರ್ಣಾಙ್ಕ, bool);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_auth_plugin_acl_check_v2)(ಶೂನ್ಯ *, ಧ್ರುವ ವರ್ಣ *, ಧ್ರುವ ವರ್ಣ *, ಧ್ರುವ ವರ್ಣ *, ಪೂರ್ಣಾಙ್ಕ);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_auth_plugin_unpwd_check_v2)(ಶೂನ್ಯ *, ಧ್ರುವ ವರ್ಣ *, ಧ್ರುವ ವರ್ಣ *);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_auth_plugin_psk_ಕೀಲಿ_get_v2)(ಶೂನ್ಯ *, ಧ್ರುವ ವರ್ಣ *, ಧ್ರುವ ವರ್ಣ *, ವರ್ಣ *, ಪೂರ್ಣಾಙ್ಕ);


ಗಣನಾ mosquitto_ಸಂದೇಶ_origin {
	mosq_mo_client = 0,
	mosq_mo_broker = 1,
};

ಸಂರಚನಾ mosquitto__plugin_lib {
	ಶೂನ್ಯ *lib;
	ಶೂನ್ಯ *user_data;
	ಪೂರ್ಣಾಙ್ಕ (*plugin_version)(ಶೂನ್ಯ);
	ಸಂರಚನಾ mosquitto_plugin_id_t *identifier;

	FUNC_plugin_init_v5 plugin_init_v5;
	FUNC_plugin_cleanup_v5 plugin_cleanup_v5;

	FUNC_auth_plugin_init_v4 plugin_init_v4;
	FUNC_auth_plugin_cleanup_v4 plugin_cleanup_v4;
	FUNC_auth_plugin_security_init_v4 security_init_v4;
	FUNC_auth_plugin_security_cleanup_v4 security_cleanup_v4;
	FUNC_auth_plugin_acl_check_v4 acl_check_v4;
	FUNC_auth_plugin_unpwd_check_v4 unpwd_check_v4;
	FUNC_auth_plugin_psk_ಕೀಲಿ_get_v4 psk_ಕೀಲಿ_get_v4;
	FUNC_auth_plugin_auth_start_v4 auth_start_v4;
	FUNC_auth_plugin_auth_continue_v4 auth_continue_v4;

	FUNC_auth_plugin_init_v3 plugin_init_v3;
	FUNC_auth_plugin_cleanup_v3 plugin_cleanup_v3;
	FUNC_auth_plugin_security_init_v3 security_init_v3;
	FUNC_auth_plugin_security_cleanup_v3 security_cleanup_v3;
	FUNC_auth_plugin_acl_check_v3 acl_check_v3;
	FUNC_auth_plugin_unpwd_check_v3 unpwd_check_v3;
	FUNC_auth_plugin_psk_ಕೀಲಿ_get_v3 psk_ಕೀಲಿ_get_v3;

	FUNC_auth_plugin_init_v2 plugin_init_v2;
	FUNC_auth_plugin_cleanup_v2 plugin_cleanup_v2;
	FUNC_auth_plugin_security_init_v2 security_init_v2;
	FUNC_auth_plugin_security_cleanup_v2 security_cleanup_v2;
	FUNC_auth_plugin_acl_check_v2 acl_check_v2;
	FUNC_auth_plugin_unpwd_check_v2 unpwd_check_v2;
	FUNC_auth_plugin_psk_ಕೀಲಿ_get_v2 psk_ಕೀಲಿ_get_v2;
	ಪೂರ್ಣಾಙ್ಕ version;
};

ಸಂರಚನಾ mosquitto__plugin_config {
	ವರ್ಣ *path;
	ವರ್ಣ *name;
	ಸಂರಚನಾ mosquitto_opt *options;
	ಸಂರಚನಾ mosquitto__security_options **security_options;
	ಪೂರ್ಣಾಙ್ಕ option_ಎಣಿಕೆ;
	ಪೂರ್ಣಾಙ್ಕ security_option_ಎಣಿಕೆ;
	bool deny_special_chars;
};

ಸಂರಚನಾ mosquitto__callback {
	UT_hash_handle hh; /* For callbacks that register for e.g. a specific topic */
	ಸಂರಚನಾ mosquitto__callback *next, *prev; /* For typical callbacks */
	MOSQ_FUNC_generic_callback cb;
	ಶೂನ್ಯ *userdata;
	ಸಙ್ಘ {
		ವರ್ಣ *topic;
		ಸಂರಚನಾ timespec next_tick;
	} data;
	mosquitto_plugin_id_t *identifier;
};

ಸಂರಚನಾ plugin__callbacks {
	ಸಂರಚನಾ mosquitto__callback *tick;
	ಸಂರಚನಾ mosquitto__callback *acl_check;
	ಸಂರಚನಾ mosquitto__callback *basic_auth;
	ಸಂರಚನಾ mosquitto__callback *connect;
	ಸಂರಚನಾ mosquitto__callback *control;
	ಸಂರಚನಾ mosquitto__callback *disconnect;
	ಸಂರಚನಾ mosquitto__callback *client_offline;
	ಸಂರಚನಾ mosquitto__callback *ext_auth_continue;
	ಸಂರಚನಾ mosquitto__callback *ext_auth_start;
	ಸಂರಚನಾ mosquitto__callback *message_in;
	ಸಂರಚನಾ mosquitto__callback *message_out;
	ಸಂರಚನಾ mosquitto__callback *psk_ಕೀಲಿ;
	ಸಂರಚನಾ mosquitto__callback *reload;
	ಸಂರಚನಾ mosquitto__callback *subscribe;
	ಸಂರಚನಾ mosquitto__callback *unsubscribe;
	ಸಂರಚನಾ mosquitto__callback *persist_restore;
	ಸಂರಚನಾ mosquitto__callback *persist_client_add;
	ಸಂರಚನಾ mosquitto__callback *persist_client_delete;
	ಸಂರಚನಾ mosquitto__callback *persist_client_update;
	ಸಂರಚನಾ mosquitto__callback *persist_subscription_add;
	ಸಂರಚನಾ mosquitto__callback *persist_subscription_delete;
	ಸಂರಚನಾ mosquitto__callback *persist_client_ಸಂದೇಶ_add;
	ಸಂರಚನಾ mosquitto__callback *persist_client_ಸಂದೇಶ_delete;
	ಸಂರಚನಾ mosquitto__callback *persist_client_ಸಂದೇಶ_update;
	ಸಂರಚನಾ mosquitto__callback *persist_base_ಸಂದೇಶ_add;
	ಸಂರಚನಾ mosquitto__callback *persist_base_ಸಂದೇಶ_delete;
	ಸಂರಚನಾ mosquitto__callback *persist_retain_ಸಂದೇಶ_set;
	ಸಂರಚನಾ mosquitto__callback *persist_retain_ಸಂದೇಶ_delete;
	ಸಂರಚನಾ mosquitto__callback *persist_will_add;
	ಸಂರಚನಾ mosquitto__callback *persist_will_delete;
};

/* This is owned by mosquitto__config or mosquitto__listener, and only referred
 * to by other structs */
ಸಂರಚನಾ mosquitto__security_options {
	/* Any options that get added here also need considering
	 * in config__read() with regards whether allow_anonymous
	 * should be disabled when these options are set.
	 */
	ಸಂರಚನಾ mosquitto__unpwd *unpwd;
	ಸಂರಚನಾ mosquitto__psk *psk_id;
	ಸಂರಚನಾ acl_file_data acl_data;
	ಸಂರಚನಾ password_file_data password_data;
	ವರ್ಣ *psk_file;
	mosquitto_plugin_id_t **plugins;
	ಪೂರ್ಣಾಙ್ಕ plugin_ಎಣಿಕೆ;
	int8_t allow_anonymous;
	bool allow_zero_length_clientid;
	ವರ್ಣ *auto_id_prefix;
	uint16_t auto_id_prefix_len;
	ಸಂರಚನಾ plugin__callbacks plugin_callbacks;
	mosquitto_plugin_id_t *pid; /* For registering as a "plugin" */
};

#ಯದಿ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
ಗಣನಾ struct_ident {
	id_invalid = 0,
	id_listener = 1,
	id_client = 2,
	id_listener_ws = 3,
};
#endif

ಸಂರಚನಾ mosquitto__listener {
	uint16_t port;
	ವರ್ಣ *host;
	ವರ್ಣ *bind_interface;
	ಪೂರ್ಣಾಙ್ಕ max_connections;
	ವರ್ಣ *mount_point;
	mosq_sock_t *socks;
	ಪೂರ್ಣಾಙ್ಕ sock_ಎಣಿಕೆ;
	ಪೂರ್ಣಾಙ್ಕ client_ಎಣಿಕೆ;
	ಗಣನಾ mosquitto_protocol protocol;
	ಪೂರ್ಣಾಙ್ಕ socket_domain;
	bool use_username_as_clientid;
	uint8_t max_qos;
	uint16_t max_topic_alias;
	uint16_t max_topic_alias_broker;
#ifdef WITH_TLS
	ವರ್ಣ *cafile;
	ವರ್ಣ *capath;
	ವರ್ಣ *certfile;
	ವರ್ಣ *keyfile;
	ವರ್ಣ *tls_engine;
	ವರ್ಣ *tls_engine_kpass_sha1;
	ವರ್ಣ *ciphers;
	ವರ್ಣ *ciphers_tls13;
	ವರ್ಣ *psk_hint;
	SSL_CTX *ssl_ctx;
	ವರ್ಣ *crlfile;
	ವರ್ಣ *tls_version;
	bool use_identity_as_username;
	bool use_subject_as_username;
	bool require_certificate;
	bool disable_client_cert_date_checks;
	ಗಣನಾ mosquitto__keyform tls_keyform;
#endif
#ifdef WITH_WEBSOCKETS
#  ಯದಿ WITH_WEBSOCKETS == WS_IS_LWS
	ಸಂರಚನಾ lws_context *ws_context;
	bool ws_in_init;
	ಸಂರಚನಾ lws_protocols *ws_protocol;
#  endif
	ವರ್ಣ **ws_origins;
	ಪೂರ್ಣಾಙ್ಕ ws_origin_ಎಣಿಕೆ;
#endif
#ಯದಿ defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
	ವರ್ಣ *http_dir;
#endif
	ಸಂರಚನಾ mosquitto__security_options *security_options;
#ifdef WITH_UNIX_SOCKETS
	ವರ್ಣ *unix_socket_path;
#endif
	bool disable_protocol_v3;
	bool disable_protocol_v4;
	bool disable_protocol_v5;
	ಪೂರ್ಣಾಙ್ಕ enable_proxy_protocol;
	bool proxy_protocol_v2_require_tls;
#ifdef WITH_HTTP_API
	ಸಂರಚನಾ MHD_Daemon *mhd;
#endif
};


ಸಂರಚನಾ mosquitto__listener_sock {
#ಯದಿ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	/* This *must* be the first element in the struct. */
	ಪೂರ್ಣಾಙ್ಕ ident;
#endif
	mosq_sock_t sock;
	ಸಂರಚನಾ mosquitto__listener *listener;
};


/* Callbacks belonging to a specific plugin
 * Doesn't include MOSQ_EVT_CONTROL events.
 */
ಸಂರಚನಾ plugin_own_callback {
	ಸಂರಚನಾ plugin_own_callback *next, *prev;
	MOSQ_FUNC_generic_callback cb_func;
	ಗಣನಾ mosquitto_plugin_event event;
};

ಸಂರಚನಾ mosquitto_plugin_id_t {
	ಸಂರಚನಾ mosquitto__plugin_config config;
	ಸಂರಚನಾ mosquitto__plugin_lib lib;
	ಸಂರಚನಾ mosquitto__listener *listener;
	ವರ್ಣ *plugin_name;
	ವರ್ಣ *plugin_version;
	ಸಂರಚನಾ control_endpoint *control_endpoints;
	ಸಂರಚನಾ plugin_own_callback *own_callbacks;
	ಸಂರಚನಾ timespec next_tick;
};

ಸಂರಚನಾ mosquitto__config {
	bool allow_duplicate_messages;
	ಪೂರ್ಣಾಙ್ಕ autosave_interval;
	bool autosave_on_changes;
	bool check_retain_source;
	ವರ್ಣ *clientid_prefixes;
	bool connection_messages;
	uint16_t cmd_port[CMD_PORT_LIMIT];
	ಪೂರ್ಣಾಙ್ಕ cmd_port_ಎಣಿಕೆ;
	bool daemon;
	bool test_configuration;
	bool enable_control_api;
	ಪೂರ್ಣಾಙ್ಕ global_max_clients;
	ಪೂರ್ಣಾಙ್ಕ global_max_connections;
	ಸಂರಚನಾ mosquitto__listener *default_listener; /* Points to one of `listeners` */
	ಸಂರಚನಾ mosquitto__listener *listeners;
	ಪೂರ್ಣಾಙ್ಕ listener_ಎಣಿಕೆ;
	bool local_only;
	unsigned ಪೂರ್ಣಾಙ್ಕ log_dest;
	ಪೂರ್ಣಾಙ್ಕ log_facility;
	unsigned ಪೂರ್ಣಾಙ್ಕ log_type;
	bool log_timestamp;
	ವರ್ಣ *log_timestamp_format;
	ವರ್ಣ *log_file;
	FILE *log_fptr;
	size_t max_inflight_bytes;
	size_t max_queued_bytes;
	ಪೂರ್ಣಾಙ್ಕ max_queued_messages;
	uint32_t max_packet_size;
	uint32_t message_size_limit;
	uint16_t max_inflight_messages;
	uint16_t max_keepalive;
	uint8_t max_qos;
	uint32_t packet_max_connect;
	uint32_t packet_max_simple;
	uint32_t packet_max_sub;
	uint32_t packet_max_auth;
	bool persistence;
	ವರ್ಣ *persistence_location;
	ವರ್ಣ *persistence_file;
	ವರ್ಣ *persistence_filepath;
	time_t persistent_client_expiration;
	ವರ್ಣ *pid_file;
	bool queue_qos0_messages;
	bool per_listener_settings;
	bool retain_available;
	ಪೂರ್ಣಾಙ್ಕ retain_expiry_interval;
	bool set_tcp_nodelay;
	ಪೂರ್ಣಾಙ್ಕ sys_interval;
	bool upgrade_outgoing_qos;
	ವರ್ಣ *user;
#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ಪೂರ್ಣಾಙ್ಕ websockets_log_level;
#endif
	uint16_t packet_buffer_size;
#ifdef WITH_BRIDGE
	ಸಂರಚನಾ mosquitto__bridge **bridges;
	ಪೂರ್ಣಾಙ್ಕ bridge_ಎಣಿಕೆ;
#endif
	ಸಂರಚನಾ mosquitto__security_options security_options;
};


ಸಂರಚನಾ mosquitto__subshared {
	UT_hash_handle hh;
	ಸಂರಚನಾ mosquitto__subleaf *subs;
	ವರ್ಣ name[];
};

ಸಂರಚನಾ mosquitto__subhier {
	UT_hash_handle hh;
	ಸಂರಚನಾ mosquitto__subhier *parent;
	ಸಂರಚನಾ mosquitto__subhier *children;
	ಸಂರಚನಾ mosquitto__subleaf *subs;
	ಸಂರಚನಾ mosquitto__subshared *shared;
	uint16_t topic_len;
	ವರ್ಣ topic[];
};

ಸಂರಚನಾ mosquitto__subleaf {
	ಸಂರಚನಾ mosquitto__subleaf *prev;
	ಸಂರಚನಾ mosquitto__subleaf *next;
	ಸಂರಚನಾ mosquitto *context;
	ಸಂರಚನಾ mosquitto__subhier *hier;
	ಸಂರಚನಾ mosquitto__subshared *shared;
	uint32_t identifier;
	uint8_t subscription_options;
	ವರ್ಣ topic_filter[];
};

ಸಂರಚನಾ sub__token {
	ಸಂರಚನಾ sub__token *next;
	uint16_t topic_len;
	ವರ್ಣ topic[];
};

ಸಂರಚನಾ mosquitto__retainhier {
	UT_hash_handle hh;
	ಸಂರಚನಾ mosquitto__retainhier *parent;
	ಸಂರಚನಾ mosquitto__retainhier *children;
	ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *retained;
	uint16_t topic_len;
	ವರ್ಣ topic[];
};

ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ {
	UT_hash_handle hh;
	ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ_2 data;
	ಸಂರಚನಾ mosquitto__listener *source_listener;
	ವರ್ಣ **dest_ids;
	ಪೂರ್ಣಾಙ್ಕ dest_id_ಎಣಿಕೆ;
	ಪೂರ್ಣಾಙ್ಕ ref_ಎಣಿಕೆ;
	ಗಣನಾ mosquitto_ಸಂದೇಶ_origin origin;
	bool stored;
};

ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ {
	ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ_2 data;
	ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *prev;
	ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *next;
	ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ;
};


ಸಂರಚನಾ mosquitto__psk {
	UT_hash_handle hh;
	ವರ್ಣ *username;
	ವರ್ಣ *password;
};

ಸಂರಚನಾ mosquitto__message_v5 {
	ಸಂರಚನಾ mosquitto__message_v5 *next, *prev;
	ವರ್ಣ *topic;
	ಶೂನ್ಯ *payload;
	mosquitto_property *properties;
	/* clientid is used only by mosquitto_broker_publish*() to indicate
	   this message is for a specific client. */
	ವರ್ಣ *clientid;
	ಪೂರ್ಣಾಙ್ಕ payloadlen;
	ಪೂರ್ಣಾಙ್ಕ qos;
	bool retain;
};

ಸಂರಚನಾ mosquitto_db {
	dbid_t last_db_id;
	uint64_t ಗಂಟು_id_shifted;
	ಸಂರಚನಾ mosquitto__subhier *normal_subs;
	ಸಂರಚನಾ mosquitto__subhier *shared_subs;
	ಸಂರಚನಾ mosquitto__retainhier *retains;
	ಸಂರಚನಾ mosquitto *contexts_by_id;
	ಸಂರಚನಾ mosquitto *contexts_by_sock;
	ಸಂರಚನಾ mosquitto *contexts_by_id_delayed_auth;
	ಸಂರಚನಾ mosquitto *contexts_for_free;
	mosquitto_plugin_id_t **plugins;
	ಪೂರ್ಣಾಙ್ಕ plugin_ಎಣಿಕೆ;
#ifdef WITH_BRIDGE
	ಸಂರಚನಾ mosquitto **bridges;
	ಪೂರ್ಣಾಙ್ಕ bridge_ಎಣಿಕೆ;
#endif
	ಸಂರಚನಾ clientid__index_hash *clientid_index_hash;
	ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *ಸಂದೇಶ_store;
	time_t now_s; /* Monotonic clock, where possible */
	time_t now_real_s; /* Read clock, for measuring session/message expiry */
	uint64_t ಗಂಟು_id; /* for unique db ids */
	time_t next_event_ms; /* for mux timeout */
	ಪೂರ್ಣಾಙ್ಕ ಸಂದೇಶ_store_ಎಣಿಕೆ;
	unsigned long ಸಂದೇಶ_store_bytes;
	ವರ್ಣ *config_file;
	ಸಂರಚನಾ mosquitto__config *config;
	bool quiet;
	bool verbose;
#ifdef WITH_SYS_ವೃಕ್ಷ
	ಪೂರ್ಣಾಙ್ಕ subscription_ಎಣಿಕೆ;
	ಪೂರ್ಣಾಙ್ಕ shared_subscription_ಎಣಿಕೆ;
	ಪೂರ್ಣಾಙ್ಕ retained_ಎಣಿಕೆ;
#endif
	ಪೂರ್ಣಾಙ್ಕ persistence_changes;
	ಸಂರಚನಾ mosquitto *ll_for_free;
#ifdef WITH_EPOLL
	ಪೂರ್ಣಾಙ್ಕ epollfd;
#endif
#ifdef WITH_KQUEUE
	ಪೂರ್ಣಾಙ್ಕ kqueuefd;
#endif
	ಸಂರಚನಾ mosquitto__message_v5 *plugin_msgs;
#ifdef WITH_TLS
	/* tls_keylog can't be in the config struct because it is used
	   before the config is allocated. Config probably
	   shouldn't be separately allocated. */
	ವರ್ಣ *tls_keylog;
#endif
	bool shutdown;
};

ಗಣನಾ mosquitto__bridge_direction {
	bd_out = 0,
	bd_in = 1,
	bd_both = 2,
};

ಗಣನಾ mosquitto_bridge_start_type {
	bst_automatic = 0,
	bst_lazy = 1,
	bst_manual = 2,
	bst_once = 3,
};

ಗಣನಾ mosquitto_bridge_reload_type {
	brt_lazy = 0,
	brt_immediate = 1,
};

ಸಂರಚನಾ mosquitto__bridge_topic {
	ಸಂರಚನಾ mosquitto__bridge_topic *next;
	ವರ್ಣ *topic;
	ವರ್ಣ *local_prefix;
	ವರ್ಣ *remote_prefix;
	ವರ್ಣ *local_topic; /* topic prefixed with local_prefix */
	ವರ್ಣ *remote_topic; /* topic prefixed with remote_prefix */
	ಗಣನಾ mosquitto__bridge_direction direction;
	uint8_t qos;
};

ಸಂರಚನಾ bridge_address {
	ವರ್ಣ *address;
	uint16_t port;
};

ಸಂರಚನಾ mosquitto__bridge {
	ವರ್ಣ *name;
	ಸಂರಚನಾ bridge_address *addresses;
	ಪೂರ್ಣಾಙ್ಕ cur_address;
	ಪೂರ್ಣಾಙ್ಕ address_ಎಣಿಕೆ;
	time_t primary_retry;
	mosq_sock_t primary_retry_sock;
	bool round_robin;
	bool try_private;
	bool try_private_accepted;
	bool clean_start;
	int8_t clean_start_local;
	uint16_t keepalive;
	unsigned ಪೂರ್ಣಾಙ್ಕ tcp_keepalive_idle;
	unsigned ಪೂರ್ಣಾಙ್ಕ tcp_keepalive_interval;
	unsigned ಪೂರ್ಣಾಙ್ಕ tcp_keepalive_counter;
#ifdef WITH_TCP_USER_TIMEOUT
	ಪೂರ್ಣಾಙ್ಕ tcp_user_timeout;
#endif
	ಸಂರಚನಾ mosquitto__bridge_topic *topics;
	ಪೂರ್ಣಾಙ್ಕ topic_ಎಣಿಕೆ;
	bool topic_remapping;
	ಗಣನಾ mosquitto__protocol protocol_version;
	time_t restart_t;
	time_t connected_at;
	ವರ್ಣ *remote_clientid;
	ವರ್ಣ *remote_username;
	ವರ್ಣ *remote_password;
	ವರ್ಣ *local_clientid;
	ವರ್ಣ *local_username;
	ವರ್ಣ *local_password;
	ವರ್ಣ *notification_topic;
	ವರ್ಣ *bind_address;
	bool notifications;
	bool notifications_local_only;
	ಗಣನಾ mosquitto_bridge_start_type start_type;
	ಪೂರ್ಣಾಙ್ಕ idle_timeout;
	ಪೂರ್ಣಾಙ್ಕ restart_timeout;
	ಪೂರ್ಣಾಙ್ಕ backoff_base;
	ಪೂರ್ಣಾಙ್ಕ backoff_cap;
	ಪೂರ್ಣಾಙ್ಕ stable_connection_period;
	ಪೂರ್ಣಾಙ್ಕ threshold;
	uint32_t maximum_packet_size;
	uint32_t session_expiry_interval;
	uint16_t receive_maximum;
	bool lazy_reconnect;
	bool attempt_unsubscribe;
	bool initial_notification_done;
	bool outgoing_retain;
	ಗಣನಾ mosquitto_bridge_reload_type reload_type;
	uint16_t max_topic_alias;
#ifdef WITH_TLS
	bool tls_insecure;
	bool tls_ocsp_required;
	bool tls_use_os_certs;
	ವರ್ಣ *tls_cafile;
	ವರ್ಣ *tls_capath;
	ವರ್ಣ *tls_certfile;
	ವರ್ಣ *tls_keyfile;
	ವರ್ಣ *tls_version;
	ವರ್ಣ *tls_alpn;
	ವರ್ಣ *tls_ciphers;
	ವರ್ಣ *tls_13_ciphers;
#  ifdef FINAL_WITH_TLS_PSK
	ವರ್ಣ *tls_psk_identity;
	ವರ್ಣ *tls_psk;
#  endif
#endif
};

#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
ಸಂರಚನಾ libws_mqtt_hack {
	ವರ್ಣ *http_dir;
	ಸಂರಚನಾ mosquitto__listener *listener;
};

ಸಂರಚನಾ libws_mqtt_data {
	ಸಂರಚನಾ mosquitto *mosq;
};
#endif

#ಅನ್ತರ್ಭಾವ <net_mosq.h>


ಸಂರಚನಾ control_endpoint {
	ಸಂರಚನಾ control_endpoint *next, *prev;
	ವರ್ಣ topic[];
};

ಬಾಹ್ಯ ಸಂರಚನಾ mosquitto_db db;

/* ============================================================
 * Main functions
 * ============================================================ */
ಪೂರ್ಣಾಙ್ಕ mosquitto_main_loop(ಸಂರಚನಾ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಙ್ಕ listensock_ಎಣಿಕೆ);
ಶೂನ್ಯ loop__update_next_event(time_t new_ms);

/* ============================================================
 * Config functions
 * ============================================================ */
/* Initialise config struct to default values. */
ಶೂನ್ಯ config__init(ಸಂರಚನಾ mosquitto__config *config);
#ifdef WITH_BRIDGE
ಶೂನ್ಯ config__bridge_cleanup(ಸಂರಚನಾ mosquitto__bridge *bridge);
#endif
/* Parse command line options into config. */
ಪೂರ್ಣಾಙ್ಕ config__parse_args(ಸಂರಚನಾ mosquitto__config *config, ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ *argv[]);
/* Read configuration data from config->config_file into config.
 * If reload is true, don't process config options that shouldn't be reloaded (listeners etc)
 * Returns 0 on success, 1 if there is a configuration error or if a file cannot be opened.
 */
ಪೂರ್ಣಾಙ್ಕ config__read(ಸಂರಚನಾ mosquitto__config *config, bool reload);
/* Free all config data. */
ಶೂನ್ಯ config__cleanup(ಸಂರಚನಾ mosquitto__config *config);
ಪೂರ್ಣಾಙ್ಕ config__get_dir_files(ಧ್ರುವ ವರ್ಣ *include_dir, ವರ್ಣ ***files, ಪೂರ್ಣಾಙ್ಕ *file_ಎಣಿಕೆ);

/* ============================================================
 * Server send functions
 * ============================================================ */
ಪೂರ್ಣಾಙ್ಕ send__connack(ಸಂರಚನಾ mosquitto *context, uint8_t ack, uint8_t reason_code, ಧ್ರುವ mosquitto_property *properties);
ಪೂರ್ಣಾಙ್ಕ send__suback(ಸಂರಚನಾ mosquitto *context, uint16_t mid, uint32_t payloadlen, ಧ್ರುವ ಶೂನ್ಯ *payload);
ಪೂರ್ಣಾಙ್ಕ send__unsuback(ಸಂರಚನಾ mosquitto *context, uint16_t mid, ಪೂರ್ಣಾಙ್ಕ reason_code_ಎಣಿಕೆ, uint8_t *reason_codes, ಧ್ರುವ mosquitto_property *properties);
ಪೂರ್ಣಾಙ್ಕ send__auth(ಸಂರಚನಾ mosquitto *context, uint8_t reason_code, ಧ್ರುವ ಶೂನ್ಯ *auth_data, uint16_t auth_data_len);

/* ============================================================
 * Network functions
 * ============================================================ */
ಶೂನ್ಯ net__broker_init(ಶೂನ್ಯ);
ಶೂನ್ಯ net__broker_cleanup(ಶೂನ್ಯ);
ಸಂರಚನಾ mosquitto *net__socket_accept(ಸಂರಚನಾ mosquitto__listener_sock *listensock);
ಪೂರ್ಣಾಙ್ಕ net__socket_listen(ಸಂರಚನಾ mosquitto__listener *listener);
ಪೂರ್ಣಾಙ್ಕ net__socket_get_address(mosq_sock_t sock, ವರ್ಣ *buf, size_t len, uint16_t *remote_address);
ಪೂರ್ಣಾಙ್ಕ net__tls_load_verify(ಸಂರಚನಾ mosquitto__listener *listener);
ಪೂರ್ಣಾಙ್ಕ net__tls_server_ctx(ಸಂರಚನಾ mosquitto__listener *listener);
ಪೂರ್ಣಾಙ್ಕ net__load_certificates(ಸಂರಚನಾ mosquitto__listener *listener);

/* ============================================================
 * Read handling functions
 * ============================================================ */
ಪೂರ್ಣಾಙ್ಕ handle__packet(ಸಂರಚನಾ mosquitto *context);
ಪೂರ್ಣಾಙ್ಕ handle__connack(ಸಂರಚನಾ mosquitto *context);
ಪೂರ್ಣಾಙ್ಕ handle__connect(ಸಂರಚನಾ mosquitto *context);
ಪೂರ್ಣಾಙ್ಕ handle__disconnect(ಸಂರಚನಾ mosquitto *context);
ಪೂರ್ಣಾಙ್ಕ handle__publish(ಸಂರಚನಾ mosquitto *context);
ಪೂರ್ಣಾಙ್ಕ handle__subscribe(ಸಂರಚನಾ mosquitto *context);
ಪೂರ್ಣಾಙ್ಕ handle__unsubscribe(ಸಂರಚನಾ mosquitto *context);
ಪೂರ್ಣಾಙ್ಕ handle__auth(ಸಂರಚನಾ mosquitto *context);

/* ============================================================
 * Database handling
 * ============================================================ */
ಪೂರ್ಣಾಙ್ಕ db__open(ಸಂರಚನಾ mosquitto__config *config);
ಪೂರ್ಣಾಙ್ಕ db__close(ಶೂನ್ಯ);
#ifdef WITH_PERSISTENCE
ಪೂರ್ಣಾಙ್ಕ persist__backup(bool shutdown);
ಪೂರ್ಣಾಙ್ಕ persist__restore(ಶೂನ್ಯ);
#endif
/* Return the number of in-flight messages in count. */
ಪೂರ್ಣಾಙ್ಕ db_message_ಎಣಿಕೆ(ಪೂರ್ಣಾಙ್ಕ *ಎಣಿಕೆ);
ಪೂರ್ಣಾಙ್ಕ db__message_delete_outgoing(ಸಂರಚನಾ mosquitto *context, uint16_t mid, ಗಣನಾ mosquitto_ಸಂದೇಶ_state expect_state, ಪೂರ್ಣಾಙ್ಕ qos);
ಪೂರ್ಣಾಙ್ಕ db_message_ಸೇರಿಸು_outgoing(ಸಂರಚನಾ mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ, uint32_t subscription_identifier, bool update, bool persist);
ಪೂರ್ಣಾಙ್ಕ db_message_ಸೇರಿಸು_incoming(ಸಂರಚನಾ mosquitto *context, uint64_t cmsg_id, ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ, bool persist);
ಪೂರ್ಣಾಙ್ಕ db__message_remove_incoming(ಸಂರಚನಾ mosquitto *context, uint16_t mid);
ಪೂರ್ಣಾಙ್ಕ db__message_release_incoming(ಸಂರಚನಾ mosquitto *context, uint16_t mid);
ಪೂರ್ಣಾಙ್ಕ db__message_update_outgoing(ಸಂರಚನಾ mosquitto *context, uint16_t mid, ಗಣನಾ mosquitto_ಸಂದೇಶ_state state, ಪೂರ್ಣಾಙ್ಕ qos, bool persist);
ಶೂನ್ಯ db__message_dequeue_first(ಸಂರಚನಾ mosquitto *context, ಸಂರಚನಾ mosquitto_ಸಂದೇಶ_data *ಸಂದೇಶ_data);
ಪೂರ್ಣಾಙ್ಕ db__messages_delete(ಸಂರಚನಾ mosquitto *context, bool force_free);
ಪೂರ್ಣಾಙ್ಕ db__messages_delete_incoming(ಸಂರಚನಾ mosquitto *context);
ಪೂರ್ಣಾಙ್ಕ db__messages_delete_outgoing(ಸಂರಚನಾ mosquitto *context);
ಪೂರ್ಣಾಙ್ಕ db__messages_easy_queue(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ವರ್ಣ *topic, uint8_t qos, uint32_t payloadlen, ಧ್ರುವ ಶೂನ್ಯ *payload, ಪೂರ್ಣಾಙ್ಕ retain, uint32_t message_expiry_interval, mosquitto_property **properties);
ಪೂರ್ಣಾಙ್ಕ db__message_store(ಧ್ರುವ ಸಂರಚನಾ mosquitto *source, ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ, uint32_t *message_expiry_interval, ಗಣನಾ mosquitto_ಸಂದೇಶ_origin origin);
ಪೂರ್ಣಾಙ್ಕ db__message_store_find(ಸಂರಚನಾ mosquitto *context, uint16_t mid, ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ **client_ಸಂದೇಶ);
ಪೂರ್ಣಾಙ್ಕ db_ಸಂದೇಶ_store_add(ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ);
ಶೂನ್ಯ db_ಸಂದೇಶ_store_remove(ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ, bool notify);
ಶೂನ್ಯ db_ಸಂದೇಶ_store_ref_inc(ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ);
ಶೂನ್ಯ db_ಸಂದೇಶ_store_ref_dec(ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ **base_ಸಂದೇಶ);
ಶೂನ್ಯ db_ಸಂದೇಶ_store_clean(ಶೂನ್ಯ);
ಶೂನ್ಯ db_ಸಂದೇಶ_store_compact(ಶೂನ್ಯ);
ಶೂನ್ಯ db_ಸಂದೇಶ_store_free(ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ);
ಪೂರ್ಣಾಙ್ಕ db__message_reconnect_reset(ಸಂರಚನಾ mosquitto *context);
bool db__ready_for_flight(ಸಂರಚನಾ mosquitto *context, ಗಣನಾ mosquitto_ಸಂದೇಶ_direction dir, ಪೂರ್ಣಾಙ್ಕ qos);
bool db__ready_for_queue(ಸಂರಚನಾ mosquitto *context, ಪೂರ್ಣಾಙ್ಕ qos, ಸಂರಚನಾ mosquitto_ಸಂದೇಶ_data *ಸಂದೇಶ_data);
ಶೂನ್ಯ sys_ವೃಕ್ಷ_init(ಶೂನ್ಯ);
ಶೂನ್ಯ sys_ವೃಕ್ಷ_update(bool force);
ಪೂರ್ಣಾಙ್ಕ db__message_write_inflight_out_all(ಸಂರಚನಾ mosquitto *context);
ಪೂರ್ಣಾಙ್ಕ db__message_write_inflight_out_latest(ಸಂರಚನಾ mosquitto *context);
ಪೂರ್ಣಾಙ್ಕ db__message_write_queued_out(ಸಂರಚನಾ mosquitto *context);
ಪೂರ್ಣಾಙ್ಕ db__message_write_queued_in(ಸಂರಚನಾ mosquitto *context);
ಶೂನ್ಯ db_ಸಂದೇಶ_add_to_inflight_stats(ಸಂರಚನಾ mosquitto_ಸಂದೇಶ_data *ಸಂದೇಶ_data, ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *ಸಂದೇಶ);
ಶೂನ್ಯ db_ಸಂದೇಶ_add_to_queued_stats(ಸಂರಚನಾ mosquitto_ಸಂದೇಶ_data *ಸಂದೇಶ_data, ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *ಸಂದೇಶ);
uint64_t db_new_ಸಂದೇಶ_id(ಶೂನ್ಯ);
ಶೂನ್ಯ db__expire_all_messages(ಸಂರಚನಾ mosquitto *context);
ಶೂನ್ಯ db__check_acl_of_all_messages(ಸಂರಚನಾ mosquitto *context);

/* ============================================================
 * Subscription functions
 * ============================================================ */
ಪೂರ್ಣಾಙ್ಕ sub__init(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ sub__add(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ಸಂರಚನಾ mosquitto_subscription *sub);
ಪೂರ್ಣಾಙ್ಕ sub__remove(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ವರ್ಣ *sub, uint8_t *reason);
ಶೂನ್ಯ sub_ವೃಕ್ಷ_print(ಸಂರಚನಾ mosquitto__subhier *ಬೇರು, ಪೂರ್ಣಾಙ್ಕ level);
ಪೂರ್ಣಾಙ್ಕ sub__clean_session(ಸಂರಚನಾ mosquitto *context);
ಪೂರ್ಣಾಙ್ಕ sub__messages_queue(ಧ್ರುವ ವರ್ಣ *source_id, uint8_t qos, ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ **base_ಸಂದೇಶ);
ಪೂರ್ಣಾಙ್ಕ sub__topic_tokenise(ಧ್ರುವ ವರ್ಣ *subtopic, ವರ್ಣ **local_sub, ವರ್ಣ ***topics, ಧ್ರುವ ವರ್ಣ **sharename);
ಶೂನ್ಯ sub__topic_tokens_free(ಸಂರಚನಾ sub__token *tokens);

/* ============================================================
 * Context functions
 * ============================================================ */
ಸಂರಚನಾ mosquitto *context__init(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ context__init_sock(ಸಂರಚನಾ mosquitto *context, mosq_sock_t sock, bool get_address);
ಶೂನ್ಯ context__cleanup(ಸಂರಚನಾ mosquitto *context, bool force_free);
ಶೂನ್ಯ context__disconnect(ಸಂರಚನಾ mosquitto *context, ಪೂರ್ಣಾಙ್ಕ reason);
ಶೂನ್ಯ context__add_to_disused(ಸಂರಚನಾ mosquitto *context);
ಶೂನ್ಯ context__free_disused(ಶೂನ್ಯ);
ಶೂನ್ಯ context__send_will(ಸಂರಚನಾ mosquitto *context);
ಶೂನ್ಯ context__add_to_by_id(ಸಂರಚನಾ mosquitto *context);
ಶೂನ್ಯ context__remove_from_by_id(ಸಂರಚನಾ mosquitto *context);

ಪೂರ್ಣಾಙ್ಕ connect__on_authorised(ಸಂರಚನಾ mosquitto *context, ಶೂನ್ಯ *auth_data_out, uint16_t auth_data_out_len);


/* ============================================================
 * Control functions
 * ============================================================ */
#ifdef WITH_CONTROL
ಪೂರ್ಣಾಙ್ಕ control__process(ಸಂರಚನಾ mosquitto *context, ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ);
ಶೂನ್ಯ control__cleanup(ಶೂನ್ಯ);
#endif
ಪೂರ್ಣಾಙ್ಕ control__register_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ಧ್ರುವ ವರ್ಣ *topic, ಶೂನ್ಯ *userdata);
ಪೂರ್ಣಾಙ್ಕ control__unregister_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ಧ್ರುವ ವರ್ಣ *topic);
ಶೂನ್ಯ control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier);


/* ============================================================
 * Logging functions
 * ============================================================ */
ಪೂರ್ಣಾಙ್ಕ log__init(ಸಂರಚನಾ mosquitto__config *config);
ಪೂರ್ಣಾಙ್ಕ log__close(ಸಂರಚನಾ mosquitto__config *config);
ಶೂನ್ಯ log__internal(ಧ್ರುವ ವರ್ಣ *fmt, ...) __attribute__((format(printf, 1, 2)));

/* ============================================================
 * Bridge functions
 * ============================================================ */
#ifdef WITH_BRIDGE
ಶೂನ್ಯ bridge__start_all(ಶೂನ್ಯ);
ಶೂನ್ಯ bridge__reload(ಶೂನ್ಯ);
ಶೂನ್ಯ bridge__db_cleanup(ಶೂನ್ಯ);
ಶೂನ್ಯ bridge__cleanup(ಸಂರಚನಾ mosquitto *context);
ಪೂರ್ಣಾಙ್ಕ bridge__connect(ಸಂರಚನಾ mosquitto *context);
#ಯದಿ defined(__GLIBC__) && defined(WITH_ADNS)
ಪೂರ್ಣಾಙ್ಕ bridge__connect_step3(ಸಂರಚನಾ mosquitto *context);
#endif
ಪೂರ್ಣಾಙ್ಕ bridge__on_connect(ಸಂರಚನಾ mosquitto *context);
ಶೂನ್ಯ bridge_check(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ bridge__register_local_connections(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ bridge__add_topic(ಸಂರಚನಾ mosquitto__bridge *bridge, ಧ್ರುವ ವರ್ಣ *topic, ಗಣನಾ mosquitto__bridge_direction direction, uint8_t qos, ಧ್ರುವ ವರ್ಣ *local_prefix, ಧ್ರುವ ವರ್ಣ *remote_prefix);
ಶೂನ್ಯ bridge__cleanup_topics(ಸಂರಚನಾ mosquitto__bridge *bridge);
ಪೂರ್ಣಾಙ್ಕ bridge__remap_topic_in(ಸಂರಚನಾ mosquitto *context, ವರ್ಣ **topic);
#endif

/* ============================================================
 * IO multiplex related functions
 * ============================================================ */
ಪೂರ್ಣಾಙ್ಕ mux__init(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ mux__add_listeners(ಸಂರಚನಾ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಙ್ಕ listensock_ಎಣಿಕೆ);
ಪೂರ್ಣಾಙ್ಕ mux__delete_listeners(ಸಂರಚನಾ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಙ್ಕ listensock_ಎಣಿಕೆ);
ಪೂರ್ಣಾಙ್ಕ mux__loop_prepare(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ mux__new(ಸಂರಚನಾ mosquitto *context);
ಪೂರ್ಣಾಙ್ಕ mux__add_out(ಸಂರಚನಾ mosquitto *context);
ಪೂರ್ಣಾಙ್ಕ mux__remove_out(ಸಂರಚನಾ mosquitto *context);
ಪೂರ್ಣಾಙ್ಕ mux__delete(ಸಂರಚನಾ mosquitto *context);
ಪೂರ್ಣಾಙ್ಕ mux__wait(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ mux__handle(ಸಂರಚನಾ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಙ್ಕ listensock_ಎಣಿಕೆ);
ಪೂರ್ಣಾಙ್ಕ mux__cleanup(ಶೂನ್ಯ);

/* ============================================================
 * Listener related functions
 * ============================================================ */
ಬಾಹ್ಯ ಸಂರಚನಾ mosquitto__listener_sock *g_listensock;
ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ g_listensock_ಎಣಿಕೆ;

ಶೂನ್ಯ listener__set_defaults(ಸಂರಚನಾ mosquitto__listener *listener);
ಶೂನ್ಯ listeners__reload_all_certificates(ಶೂನ್ಯ);
#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
ಶೂನ್ಯ listeners__add_websockets(ಸಂರಚನಾ lws_context *ws_context, mosq_sock_t fd);
#endif
ಪೂರ್ಣಾಙ್ಕ listeners__start(ಶೂನ್ಯ);
ಶೂನ್ಯ listeners__stop(ಶೂನ್ಯ);

/* ============================================================
 * Plugin related functions
 * ============================================================ */
ಪೂರ್ಣಾಙ್ಕ plugin__load_v5(mosquitto_plugin_id_t *plugin, ಶೂನ್ಯ *lib);
ಪೂರ್ಣಾಙ್ಕ plugin__load_v4(mosquitto_plugin_id_t *plugin, ಶೂನ್ಯ *lib);
ಪೂರ್ಣಾಙ್ಕ plugin__load_v3(mosquitto_plugin_id_t *plugin, ಶೂನ್ಯ *lib);
ಪೂರ್ಣಾಙ್ಕ plugin__load_v2(mosquitto_plugin_id_t *plugin, ಶೂನ್ಯ *lib);
ಪೂರ್ಣಾಙ್ಕ acl__pre_check(mosquitto_plugin_id_t *plugin, ಸಂರಚನಾ mosquitto *context, ಪೂರ್ಣಾಙ್ಕ access);

ಶೂನ್ಯ LIB_ERROR(ಶೂನ್ಯ);
ಶೂನ್ಯ plugin__handle_connect(ಸಂರಚನಾ mosquitto *context);
ಶೂನ್ಯ plugin__handle_disconnect(ಸಂರಚನಾ mosquitto *context, ಪೂರ್ಣಾಙ್ಕ reason);
ಶೂನ್ಯ plugin__handle_client_offline(ಸಂರಚನಾ mosquitto *context, ಪೂರ್ಣಾಙ್ಕ reason);
ಪೂರ್ಣಾಙ್ಕ plugin__handle_message_in(ಸಂರಚನಾ mosquitto *context, ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ_2 *base_ಸಂದೇಶ);
ಪೂರ್ಣಾಙ್ಕ plugin__handle_message_out(ಸಂರಚನಾ mosquitto *context, ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ_2 *base_ಸಂದೇಶ);
ಪೂರ್ಣಾಙ್ಕ plugin__handle_subscribe(ಸಂರಚನಾ mosquitto *context, ಸಂರಚನಾ mosquitto_subscription *sub);
ಪೂರ್ಣಾಙ್ಕ plugin__handle_unsubscribe(ಸಂರಚನಾ mosquitto *context, ಸಂರಚನಾ mosquitto_subscription *sub);
ಪೂರ್ಣಾಙ್ಕ plugin__handle_reload(ಶೂನ್ಯ);
ಶೂನ್ಯ plugin__handle_tick(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ plugin__callback_unregister_all(mosquitto_plugin_id_t *identifier);
ಶೂನ್ಯ plugin_persist__handle_restore(ಶೂನ್ಯ);
ಶೂನ್ಯ plugin_persist__handle_client_add(ಸಂರಚನಾ mosquitto *context);
ಶೂನ್ಯ plugin_persist__handle_client_delete(ಸಂರಚನಾ mosquitto *context);
ಶೂನ್ಯ plugin_persist__handle_client_update(ಸಂರಚನಾ mosquitto *context);
ಶೂನ್ಯ plugin_persist__handle_subscription_add(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ಸಂರಚನಾ mosquitto_subscription *sub);
ಶೂನ್ಯ plugin_persist__handle_subscription_delete(ಸಂರಚನಾ mosquitto *context, ವರ್ಣ *sub);
ಶೂನ್ಯ plugin_persist_handle_client_ಸಂದೇಶ_add(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *cmsg);
ಶೂನ್ಯ plugin_persist_handle_client_ಸಂದೇಶ_delete(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *cmsg);
ಶೂನ್ಯ plugin_persist_handle_client_ಸಂದೇಶ_update(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *cmsg);
ಶೂನ್ಯ plugin_persist_handle_base_ಸಂದೇಶ_add(ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ);
ಶೂನ್ಯ plugin_persist_handle_base_ಸಂದೇಶ_delete(ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ);
ಶೂನ್ಯ plugin_persist_handle_retain_ಸಂದೇಶ_set(ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ);
ಶೂನ್ಯ plugin_persist_handle_retain_ಸಂದೇಶ_delete(ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ);
ಶೂನ್ಯ plugin_persist__handle_will_add(ಸಂರಚನಾ mosquitto *context);
ಶೂನ್ಯ plugin_persist__handle_will_delete(ಸಂರಚನಾ mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
ಪೂರ್ಣಾಙ್ಕ keepalive__init(ಶೂನ್ಯ);
ಶೂನ್ಯ keepalive__cleanup(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ keepalive__add(ಸಂರಚನಾ mosquitto *context);
ಶೂನ್ಯ keepalive__check(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ keepalive__remove(ಸಂರಚನಾ mosquitto *context);
ಪೂರ್ಣಾಙ್ಕ keepalive__update(ಸಂರಚನಾ mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
ಪೂರ್ಣಾಙ್ಕ property__process_connect(ಸಂರಚನಾ mosquitto *context, mosquitto_property **props);
ಪೂರ್ಣಾಙ್ಕ property__process_will(ಸಂರಚನಾ mosquitto *context, ಸಂರಚನಾ mosquitto_message_all *ಸಂದೇಶ, mosquitto_property **props);
ಪೂರ್ಣಾಙ್ಕ property__process_publish(ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ, mosquitto_property **props, ಪೂರ್ಣಾಙ್ಕ *topic_alias, uint32_t *message_expiry_interval, bool is_bridge);
ಪೂರ್ಣಾಙ್ಕ property__process_disconnect(ಸಂರಚನಾ mosquitto *context, mosquitto_property **props);

/* ============================================================
 * Retain tree related functions
 * ============================================================ */
ಪೂರ್ಣಾಙ್ಕ retain__init(ಶೂನ್ಯ);
ಶೂನ್ಯ retain__clean(ಸಂರಚನಾ mosquitto__retainhier **retainhier);
ಪೂರ್ಣಾಙ್ಕ retain__queue(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ಸಂರಚನಾ mosquitto_subscription *sub);
ಪೂರ್ಣಾಙ್ಕ retain__store(ಧ್ರುವ ವರ್ಣ *topic, ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ, ವರ್ಣ **split_topics, bool persist);
ಶೂನ್ಯ retain__expiry_check(ಶೂನ್ಯ);
ಶೂನ್ಯ retain__expire(ಸಂರಚನಾ mosquitto__retainhier **retainhier);

/* ============================================================
 * Security related functions
 * ============================================================ */
ಪೂರ್ಣಾಙ್ಕ acl__find_acls(ಸಂರಚನಾ mosquitto *context);
ಪೂರ್ಣಾಙ್ಕ plugin__load_all(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ plugin__unload_all(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, ಸಂರಚನಾ mosquitto__security_options *security_options);

ಪೂರ್ಣಾಙ್ಕ mosquitto_security_init(bool reload);
ಪೂರ್ಣಾಙ್ಕ mosquitto_security_cleanup(bool reload);
ಪೂರ್ಣಾಙ್ಕ mosquitto_acl_check(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ವರ್ಣ *topic, uint32_t payloadlen, ಶೂನ್ಯ *payload, uint8_t qos, bool retain, mosquitto_property *properties, ಪೂರ್ಣಾಙ್ಕ access);
ಪೂರ್ಣಾಙ್ಕ mosquitto_basic_auth(ಸಂರಚನಾ mosquitto *context);
ಪೂರ್ಣಾಙ್ಕ mosquitto_psk_ಕೀಲಿ_get(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ವರ್ಣ *hint, ಧ್ರುವ ವರ್ಣ *identity, ವರ್ಣ *ಕೀಲಿ, ಪೂರ್ಣಾಙ್ಕ max_ಕೀಲಿ_len);

ಪೂರ್ಣಾಙ್ಕ mosquitto_security_init_default(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ mosquitto_security_apply_default(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ mosquitto_security_cleanup_default(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ mosquitto_psk_ಕೀಲಿ_get_default(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ವರ್ಣ *hint, ಧ್ರುವ ವರ್ಣ *identity, ವರ್ಣ *ಕೀಲಿ, ಪೂರ್ಣಾಙ್ಕ max_ಕೀಲಿ_len);
ಪೂರ್ಣಾಙ್ಕ broker_acl_file__init(ಶೂನ್ಯ);
ಶೂನ್ಯ broker_acl_file__cleanup(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ broker_password_file__init(ಶೂನ್ಯ);
ಶೂನ್ಯ broker_password_file__cleanup(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ psk_file__init(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ psk_file__cleanup(ಶೂನ್ಯ);

ಪೂರ್ಣಾಙ್ಕ mosquitto_security_auth_start(ಸಂರಚನಾ mosquitto *context, bool reauth, ಧ್ರುವ ಶೂನ್ಯ *data_in, uint16_t data_in_len, ಶೂನ್ಯ **data_out, uint16_t *data_out_len);
ಪೂರ್ಣಾಙ್ಕ mosquitto_security_auth_continue(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ಶೂನ್ಯ *data_in, uint16_t data_len, ಶೂನ್ಯ **data_out, uint16_t *data_out_len);

ಶೂನ್ಯ unpwd__free_item(ಸಂರಚನಾ mosquitto__unpwd **unpwd, ಸಂರಚನಾ mosquitto__unpwd *item);

/* ============================================================
 * Session expiry
 * ============================================================ */
ಪೂರ್ಣಾಙ್ಕ session_expiry__add(ಸಂರಚನಾ mosquitto *context);
ಪೂರ್ಣಾಙ್ಕ session_expiry__add_from_persistence(ಸಂರಚನಾ mosquitto *context, time_t expiry_time);
ಶೂನ್ಯ session_expiry__add_on_shutdown(ಶೂನ್ಯ);
ಶೂನ್ಯ session_expiry__remove(ಸಂರಚನಾ mosquitto *context);
ಶೂನ್ಯ session_expiry__remove_all(ಶೂನ್ಯ);
ಶೂನ್ಯ session_expiry__check(ಶೂನ್ಯ);
ಶೂನ್ಯ session_expiry__send_all(ಶೂನ್ಯ);

/* ============================================================
 * Signals
 * ============================================================ */
ಶೂನ್ಯ signal__setup(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ signal__flag_check(ಶೂನ್ಯ);

/* ============================================================
 * Window service and signal related functions
 * ============================================================ */
#ಯದಿ defined(WIN32) || defined(__CYGWIN__)
ಶೂನ್ಯ service_install(ವರ್ಣ *name);
ಶೂನ್ಯ service_uninstall(ವರ್ಣ *name);
ಶೂನ್ಯ service_run(ವರ್ಣ *name);

#ifdef WIN32
DWORD WINAPI SigThreadProc(ಶೂನ್ಯ *data);
#endif
#endif

/* ============================================================
 * Watchdog
 * ============================================================ */
ಶೂನ್ಯ watchdog__init(ಶೂನ್ಯ);
ಶೂನ್ಯ watchdog__check(ಶೂನ್ಯ);

/* ============================================================
 * Websockets related functions
 * ============================================================ */
#ifdef WITH_WEBSOCKETS
#  ಯದಿ WITH_WEBSOCKETS == WS_IS_LWS
ಶೂನ್ಯ mosq_websockets_init(ಸಂರಚನಾ mosquitto__listener *listener, ಧ್ರುವ ಸಂರಚನಾ mosquitto__config *conf);
#  endif
ಪೂರ್ಣಾಙ್ಕ http__context_init(ಸಂರಚನಾ mosquitto *context);
ಪೂರ್ಣಾಙ್ಕ http__context_cleanup(ಸಂರಚನಾ mosquitto *context);
ಪೂರ್ಣಾಙ್ಕ http__read(ಸಂರಚನಾ mosquitto *context);
#endif
ಶೂನ್ಯ do_disconnect(ಸಂರಚನಾ mosquitto *context, ಪೂರ್ಣಾಙ್ಕ reason);

/* ============================================================
 * PROXY related functions
 * ============================================================ */
ಪೂರ್ಣಾಙ್ಕ proxy_v2__read(ಸಂರಚನಾ mosquitto *context);
ಪೂರ್ಣಾಙ್ಕ proxy_v1__read(ಸಂರಚನಾ mosquitto *context);

/* ============================================================
 * Will delay
 * ============================================================ */
ಪೂರ್ಣಾಙ್ಕ will_delay__add(ಸಂರಚನಾ mosquitto *context);
ಶೂನ್ಯ will_delay__check(ಶೂನ್ಯ);
ಶೂನ್ಯ will_delay__send_all(ಶೂನ್ಯ);
ಶೂನ್ಯ will_delay__remove(ಸಂರಚನಾ mosquitto *mosq);

/* ============================================================
 * HTTP Info
 * ============================================================ */
ಪೂರ್ಣಾಙ್ಕ http_api__start_local(ಸಂರಚನಾ mosquitto__listener *listener);
ಪೂರ್ಣಾಙ್ಕ http_api__start(ಸಂರಚನಾ mosquitto__listener *listener);
ಶೂನ್ಯ http_api__stop(ಸಂರಚನಾ mosquitto__listener *listener);


/* ============================================================
 * Other
 * ============================================================ */
#ifdef WITH_XTREPORT
ಶೂನ್ಯ xtreport(ಶೂನ್ಯ);
#endif

ಶೂನ್ಯ broker_control__init(ಶೂನ್ಯ);
ಶೂನ್ಯ broker_control__cleanup(ಶೂನ್ಯ);
ಶೂನ್ಯ broker_control__reload(ಶೂನ್ಯ);

#endif
