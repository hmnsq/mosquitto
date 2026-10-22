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

#അന്തര്ഭാവ "config.h"
#അന്തര്ഭാവ <stdio.h>

#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#  എങ്കിൽ LWS_LIBRARY_VERSION_സംഖ്യ >= 3002000 && !defined(LWS_WITH_EXTERNAL_POLL)
#    warning "libwebsockets is not compiled with LWS_WITH_EXTERNAL_POLL support. Websocket performance will be unusable."
#  endif
#endif

#ifdef WITH_HTTP_API
#  include <microhttpd.h>
#endif

#ifdef __linux__
#define WITH_TCP_USER_TIMEOUT
#endif

#അന്തര്ഭാവ "mosquitto_internal.h"
#അന്തര്ഭാവ "mosquitto/broker.h"
#അന്തര്ഭാവ "mosquitto/broker_plugin.h"
#അന്തര്ഭാവ "mosquitto.h"
#അന്തര്ഭാവ "logging_mosq.h"
#അന്തര്ഭാവ "tls_mosq.h"
#അന്തര്ഭാവ "uthash.h"
#അന്തര്ഭാവ "acl_file.h"
#അന്തര്ഭാവ "password_file.h"

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

തരനിർവചനം uint64_t dbid_t;

തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_plugin_init_v5)(mosquitto_plugin_id_t *, ശൂന്യം **, ഘടന mosquitto_opt *, പൂർണ്ണസംഖ്യ);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_plugin_cleanup_v5)(ശൂന്യം *, ഘടന mosquitto_opt *, പൂർണ്ണസംഖ്യ);

തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_auth_plugin_init_v4)(ശൂന്യം **, ഘടന mosquitto_opt *, പൂർണ്ണസംഖ്യ);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_auth_plugin_cleanup_v4)(ശൂന്യം *, ഘടന mosquitto_opt *, പൂർണ്ണസംഖ്യ);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_auth_plugin_security_init_v4)(ശൂന്യം *, ഘടന mosquitto_opt *, പൂർണ്ണസംഖ്യ, bool);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_auth_plugin_security_cleanup_v4)(ശൂന്യം *, ഘടന mosquitto_opt *, പൂർണ്ണസംഖ്യ, bool);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_auth_plugin_acl_check_v4)(ശൂന്യം *, പൂർണ്ണസംഖ്യ, ഘടന mosquitto *, ഘടന mosquitto_acl_സന്ദേശം *);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_auth_plugin_unpwd_check_v4)(ശൂന്യം *, ഘടന mosquitto *, സ്ഥിരം അക്ഷരം *, സ്ഥിരം അക്ഷരം *);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_auth_plugin_psk_താക്കോൽ_get_v4)(ശൂന്യം *, ഘടന mosquitto *, സ്ഥിരം അക്ഷരം *, സ്ഥിരം അക്ഷരം *, അക്ഷരം *, പൂർണ്ണസംഖ്യ);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_auth_plugin_auth_start_v4)(ശൂന്യം *, ഘടന mosquitto *, സ്ഥിരം അക്ഷരം *, bool, സ്ഥിരം ശൂന്യം *, uint16_t, ശൂന്യം **, uint16_t *);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_auth_plugin_auth_continue_v4)(ശൂന്യം *, ഘടന mosquitto *, സ്ഥിരം അക്ഷരം *, സ്ഥിരം ശൂന്യം *, uint16_t, ശൂന്യം **, uint16_t *);

തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_auth_plugin_init_v3)(ശൂന്യം **, ഘടന mosquitto_opt *, പൂർണ്ണസംഖ്യ);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_auth_plugin_cleanup_v3)(ശൂന്യം *, ഘടന mosquitto_opt *, പൂർണ്ണസംഖ്യ);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_auth_plugin_security_init_v3)(ശൂന്യം *, ഘടന mosquitto_opt *, പൂർണ്ണസംഖ്യ, bool);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_auth_plugin_security_cleanup_v3)(ശൂന്യം *, ഘടന mosquitto_opt *, പൂർണ്ണസംഖ്യ, bool);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_auth_plugin_acl_check_v3)(ശൂന്യം *, പൂർണ്ണസംഖ്യ, സ്ഥിരം ഘടന mosquitto *, ഘടന mosquitto_acl_സന്ദേശം *);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_auth_plugin_unpwd_check_v3)(ശൂന്യം *, സ്ഥിരം ഘടന mosquitto *, സ്ഥിരം അക്ഷരം *, സ്ഥിരം അക്ഷരം *);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_auth_plugin_psk_താക്കോൽ_get_v3)(ശൂന്യം *, സ്ഥിരം ഘടന mosquitto *, സ്ഥിരം അക്ഷരം *, സ്ഥിരം അക്ഷരം *, അക്ഷരം *, പൂർണ്ണസംഖ്യ);

തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_auth_plugin_init_v2)(ശൂന്യം **, ഘടന mosquitto_auth_opt *, പൂർണ്ണസംഖ്യ);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_auth_plugin_cleanup_v2)(ശൂന്യം *, ഘടന mosquitto_auth_opt *, പൂർണ്ണസംഖ്യ);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_auth_plugin_security_init_v2)(ശൂന്യം *, ഘടന mosquitto_auth_opt *, പൂർണ്ണസംഖ്യ, bool);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_auth_plugin_security_cleanup_v2)(ശൂന്യം *, ഘടന mosquitto_auth_opt *, പൂർണ്ണസംഖ്യ, bool);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_auth_plugin_acl_check_v2)(ശൂന്യം *, സ്ഥിരം അക്ഷരം *, സ്ഥിരം അക്ഷരം *, സ്ഥിരം അക്ഷരം *, പൂർണ്ണസംഖ്യ);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_auth_plugin_unpwd_check_v2)(ശൂന്യം *, സ്ഥിരം അക്ഷരം *, സ്ഥിരം അക്ഷരം *);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_auth_plugin_psk_താക്കോൽ_get_v2)(ശൂന്യം *, സ്ഥിരം അക്ഷരം *, സ്ഥിരം അക്ഷരം *, അക്ഷരം *, പൂർണ്ണസംഖ്യ);


എണ്ണൽ mosquitto_സന്ദേശം_origin {
	mosq_mo_client = 0,
	mosq_mo_broker = 1,
};

ഘടന mosquitto__plugin_lib {
	ശൂന്യം *lib;
	ശൂന്യം *user_data;
	പൂർണ്ണസംഖ്യ (*plugin_version)(ശൂന്യം);
	ഘടന mosquitto_plugin_id_t *identifier;

	FUNC_plugin_init_v5 plugin_init_v5;
	FUNC_plugin_cleanup_v5 plugin_cleanup_v5;

	FUNC_auth_plugin_init_v4 plugin_init_v4;
	FUNC_auth_plugin_cleanup_v4 plugin_cleanup_v4;
	FUNC_auth_plugin_security_init_v4 security_init_v4;
	FUNC_auth_plugin_security_cleanup_v4 security_cleanup_v4;
	FUNC_auth_plugin_acl_check_v4 acl_check_v4;
	FUNC_auth_plugin_unpwd_check_v4 unpwd_check_v4;
	FUNC_auth_plugin_psk_താക്കോൽ_get_v4 psk_താക്കോൽ_get_v4;
	FUNC_auth_plugin_auth_start_v4 auth_start_v4;
	FUNC_auth_plugin_auth_continue_v4 auth_continue_v4;

	FUNC_auth_plugin_init_v3 plugin_init_v3;
	FUNC_auth_plugin_cleanup_v3 plugin_cleanup_v3;
	FUNC_auth_plugin_security_init_v3 security_init_v3;
	FUNC_auth_plugin_security_cleanup_v3 security_cleanup_v3;
	FUNC_auth_plugin_acl_check_v3 acl_check_v3;
	FUNC_auth_plugin_unpwd_check_v3 unpwd_check_v3;
	FUNC_auth_plugin_psk_താക്കോൽ_get_v3 psk_താക്കോൽ_get_v3;

	FUNC_auth_plugin_init_v2 plugin_init_v2;
	FUNC_auth_plugin_cleanup_v2 plugin_cleanup_v2;
	FUNC_auth_plugin_security_init_v2 security_init_v2;
	FUNC_auth_plugin_security_cleanup_v2 security_cleanup_v2;
	FUNC_auth_plugin_acl_check_v2 acl_check_v2;
	FUNC_auth_plugin_unpwd_check_v2 unpwd_check_v2;
	FUNC_auth_plugin_psk_താക്കോൽ_get_v2 psk_താക്കോൽ_get_v2;
	പൂർണ്ണസംഖ്യ version;
};

ഘടന mosquitto__plugin_config {
	അക്ഷരം *path;
	അക്ഷരം *name;
	ഘടന mosquitto_opt *options;
	ഘടന mosquitto__security_options **security_options;
	പൂർണ്ണസംഖ്യ option_എണ്ണം;
	പൂർണ്ണസംഖ്യ security_option_എണ്ണം;
	bool deny_special_chars;
};

ഘടന mosquitto__callback {
	UT_hash_handle hh; /* For callbacks that register for e.g. a specific topic */
	ഘടന mosquitto__callback *next, *prev; /* For typical callbacks */
	MOSQ_FUNC_generic_callback cb;
	ശൂന്യം *userdata;
	സംഘം {
		അക്ഷരം *topic;
		ഘടന timespec next_tick;
	} data;
	mosquitto_plugin_id_t *identifier;
};

ഘടന plugin__callbacks {
	ഘടന mosquitto__callback *tick;
	ഘടന mosquitto__callback *acl_check;
	ഘടന mosquitto__callback *basic_auth;
	ഘടന mosquitto__callback *connect;
	ഘടന mosquitto__callback *control;
	ഘടന mosquitto__callback *disconnect;
	ഘടന mosquitto__callback *client_offline;
	ഘടന mosquitto__callback *ext_auth_continue;
	ഘടന mosquitto__callback *ext_auth_start;
	ഘടന mosquitto__callback *message_in;
	ഘടന mosquitto__callback *message_out;
	ഘടന mosquitto__callback *psk_താക്കോൽ;
	ഘടന mosquitto__callback *reload;
	ഘടന mosquitto__callback *subscribe;
	ഘടന mosquitto__callback *unsubscribe;
	ഘടന mosquitto__callback *persist_restore;
	ഘടന mosquitto__callback *persist_client_add;
	ഘടന mosquitto__callback *persist_client_delete;
	ഘടന mosquitto__callback *persist_client_update;
	ഘടന mosquitto__callback *persist_subscription_add;
	ഘടന mosquitto__callback *persist_subscription_delete;
	ഘടന mosquitto__callback *persist_client_സന്ദേശം_add;
	ഘടന mosquitto__callback *persist_client_സന്ദേശം_delete;
	ഘടന mosquitto__callback *persist_client_സന്ദേശം_update;
	ഘടന mosquitto__callback *persist_base_സന്ദേശം_add;
	ഘടന mosquitto__callback *persist_base_സന്ദേശം_delete;
	ഘടന mosquitto__callback *persist_retain_സന്ദേശം_set;
	ഘടന mosquitto__callback *persist_retain_സന്ദേശം_delete;
	ഘടന mosquitto__callback *persist_will_add;
	ഘടന mosquitto__callback *persist_will_delete;
};

/* This is owned by mosquitto__config or mosquitto__listener, and only referred
 * to by other structs */
ഘടന mosquitto__security_options {
	/* Any options that get added here also need considering
	 * in config__read() with regards whether allow_anonymous
	 * should be disabled when these options are set.
	 */
	ഘടന mosquitto__unpwd *unpwd;
	ഘടന mosquitto__psk *psk_id;
	ഘടന acl_file_data acl_data;
	ഘടന password_file_data password_data;
	അക്ഷരം *psk_file;
	mosquitto_plugin_id_t **plugins;
	പൂർണ്ണസംഖ്യ plugin_എണ്ണം;
	int8_t allow_anonymous;
	bool allow_zero_length_clientid;
	അക്ഷരം *auto_id_prefix;
	uint16_t auto_id_prefix_len;
	ഘടന plugin__callbacks plugin_callbacks;
	mosquitto_plugin_id_t *pid; /* For registering as a "plugin" */
};

#എങ്കിൽ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
എണ്ണൽ struct_ident {
	id_invalid = 0,
	id_listener = 1,
	id_client = 2,
	id_listener_ws = 3,
};
#endif

ഘടന mosquitto__listener {
	uint16_t port;
	അക്ഷരം *host;
	അക്ഷരം *bind_interface;
	പൂർണ്ണസംഖ്യ max_connections;
	അക്ഷരം *mount_point;
	mosq_sock_t *socks;
	പൂർണ്ണസംഖ്യ sock_എണ്ണം;
	പൂർണ്ണസംഖ്യ client_എണ്ണം;
	എണ്ണൽ mosquitto_protocol protocol;
	പൂർണ്ണസംഖ്യ socket_domain;
	bool use_username_as_clientid;
	uint8_t max_qos;
	uint16_t max_topic_alias;
	uint16_t max_topic_alias_broker;
#ifdef WITH_TLS
	അക്ഷരം *cafile;
	അക്ഷരം *capath;
	അക്ഷരം *certfile;
	അക്ഷരം *keyfile;
	അക്ഷരം *tls_engine;
	അക്ഷരം *tls_engine_kpass_sha1;
	അക്ഷരം *ciphers;
	അക്ഷരം *ciphers_tls13;
	അക്ഷരം *psk_hint;
	SSL_CTX *ssl_ctx;
	അക്ഷരം *crlfile;
	അക്ഷരം *tls_version;
	bool use_identity_as_username;
	bool use_subject_as_username;
	bool require_certificate;
	bool disable_client_cert_date_checks;
	എണ്ണൽ mosquitto__keyform tls_keyform;
#endif
#ifdef WITH_WEBSOCKETS
#  എങ്കിൽ WITH_WEBSOCKETS == WS_IS_LWS
	ഘടന lws_context *ws_context;
	bool ws_in_init;
	ഘടന lws_protocols *ws_protocol;
#  endif
	അക്ഷരം **ws_origins;
	പൂർണ്ണസംഖ്യ ws_origin_എണ്ണം;
#endif
#എങ്കിൽ defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
	അക്ഷരം *http_dir;
#endif
	ഘടന mosquitto__security_options *security_options;
#ifdef WITH_UNIX_SOCKETS
	അക്ഷരം *unix_socket_path;
#endif
	bool disable_protocol_v3;
	bool disable_protocol_v4;
	bool disable_protocol_v5;
	പൂർണ്ണസംഖ്യ enable_proxy_protocol;
	bool proxy_protocol_v2_require_tls;
#ifdef WITH_HTTP_API
	ഘടന MHD_Daemon *mhd;
#endif
};


ഘടന mosquitto__listener_sock {
#എങ്കിൽ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	/* This *must* be the first element in the struct. */
	പൂർണ്ണസംഖ്യ ident;
#endif
	mosq_sock_t sock;
	ഘടന mosquitto__listener *listener;
};


/* Callbacks belonging to a specific plugin
 * Doesn't include MOSQ_EVT_CONTROL events.
 */
ഘടന plugin_own_callback {
	ഘടന plugin_own_callback *next, *prev;
	MOSQ_FUNC_generic_callback cb_func;
	എണ്ണൽ mosquitto_plugin_event event;
};

ഘടന mosquitto_plugin_id_t {
	ഘടന mosquitto__plugin_config config;
	ഘടന mosquitto__plugin_lib lib;
	ഘടന mosquitto__listener *listener;
	അക്ഷരം *plugin_name;
	അക്ഷരം *plugin_version;
	ഘടന control_endpoint *control_endpoints;
	ഘടന plugin_own_callback *own_callbacks;
	ഘടന timespec next_tick;
};

ഘടന mosquitto__config {
	bool allow_duplicate_messages;
	പൂർണ്ണസംഖ്യ autosave_interval;
	bool autosave_on_changes;
	bool check_retain_source;
	അക്ഷരം *clientid_prefixes;
	bool connection_messages;
	uint16_t cmd_port[CMD_PORT_LIMIT];
	പൂർണ്ണസംഖ്യ cmd_port_എണ്ണം;
	bool daemon;
	bool test_configuration;
	bool enable_control_api;
	പൂർണ്ണസംഖ്യ global_max_clients;
	പൂർണ്ണസംഖ്യ global_max_connections;
	ഘടന mosquitto__listener *default_listener; /* Points to one of `listeners` */
	ഘടന mosquitto__listener *listeners;
	പൂർണ്ണസംഖ്യ listener_എണ്ണം;
	bool local_only;
	unsigned പൂർണ്ണസംഖ്യ log_dest;
	പൂർണ്ണസംഖ്യ log_facility;
	unsigned പൂർണ്ണസംഖ്യ log_type;
	bool log_timestamp;
	അക്ഷരം *log_timestamp_format;
	അക്ഷരം *log_file;
	FILE *log_fptr;
	size_t max_inflight_bytes;
	size_t max_queued_bytes;
	പൂർണ്ണസംഖ്യ max_queued_messages;
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
	അക്ഷരം *persistence_location;
	അക്ഷരം *persistence_file;
	അക്ഷരം *persistence_filepath;
	time_t persistent_client_expiration;
	അക്ഷരം *pid_file;
	bool queue_qos0_messages;
	bool per_listener_settings;
	bool retain_available;
	പൂർണ്ണസംഖ്യ retain_expiry_interval;
	bool set_tcp_nodelay;
	പൂർണ്ണസംഖ്യ sys_interval;
	bool upgrade_outgoing_qos;
	അക്ഷരം *user;
#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	പൂർണ്ണസംഖ്യ websockets_log_level;
#endif
	uint16_t packet_buffer_size;
#ifdef WITH_BRIDGE
	ഘടന mosquitto__bridge **bridges;
	പൂർണ്ണസംഖ്യ bridge_എണ്ണം;
#endif
	ഘടന mosquitto__security_options security_options;
};


ഘടന mosquitto__subshared {
	UT_hash_handle hh;
	ഘടന mosquitto__subleaf *subs;
	അക്ഷരം name[];
};

ഘടന mosquitto__subhier {
	UT_hash_handle hh;
	ഘടന mosquitto__subhier *parent;
	ഘടന mosquitto__subhier *children;
	ഘടന mosquitto__subleaf *subs;
	ഘടന mosquitto__subshared *shared;
	uint16_t topic_len;
	അക്ഷരം topic[];
};

ഘടന mosquitto__subleaf {
	ഘടന mosquitto__subleaf *prev;
	ഘടന mosquitto__subleaf *next;
	ഘടന mosquitto *context;
	ഘടന mosquitto__subhier *hier;
	ഘടന mosquitto__subshared *shared;
	uint32_t identifier;
	uint8_t subscription_options;
	അക്ഷരം topic_filter[];
};

ഘടന sub__token {
	ഘടന sub__token *next;
	uint16_t topic_len;
	അക്ഷരം topic[];
};

ഘടന mosquitto__retainhier {
	UT_hash_handle hh;
	ഘടന mosquitto__retainhier *parent;
	ഘടന mosquitto__retainhier *children;
	ഘടന mosquitto_base_സന്ദേശം *retained;
	uint16_t topic_len;
	അക്ഷരം topic[];
};

ഘടന mosquitto_base_സന്ദേശം {
	UT_hash_handle hh;
	ഘടന mosquitto_base_സന്ദേശം_2 data;
	ഘടന mosquitto__listener *source_listener;
	അക്ഷരം **dest_ids;
	പൂർണ്ണസംഖ്യ dest_id_എണ്ണം;
	പൂർണ്ണസംഖ്യ ref_എണ്ണം;
	എണ്ണൽ mosquitto_സന്ദേശം_origin origin;
	bool stored;
};

ഘടന mosquitto_client_സന്ദേശം {
	ഘടന mosquitto_client_സന്ദേശം_2 data;
	ഘടന mosquitto_client_സന്ദേശം *prev;
	ഘടന mosquitto_client_സന്ദേശം *next;
	ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം;
};


ഘടന mosquitto__psk {
	UT_hash_handle hh;
	അക്ഷരം *username;
	അക്ഷരം *password;
};

ഘടന mosquitto__message_v5 {
	ഘടന mosquitto__message_v5 *next, *prev;
	അക്ഷരം *topic;
	ശൂന്യം *payload;
	mosquitto_property *properties;
	/* clientid is used only by mosquitto_broker_publish*() to indicate
	   this message is for a specific client. */
	അക്ഷരം *clientid;
	പൂർണ്ണസംഖ്യ payloadlen;
	പൂർണ്ണസംഖ്യ qos;
	bool retain;
};

ഘടന mosquitto_db {
	dbid_t last_db_id;
	uint64_t കണ്ണി_id_shifted;
	ഘടന mosquitto__subhier *normal_subs;
	ഘടന mosquitto__subhier *shared_subs;
	ഘടന mosquitto__retainhier *retains;
	ഘടന mosquitto *contexts_by_id;
	ഘടന mosquitto *contexts_by_sock;
	ഘടന mosquitto *contexts_by_id_delayed_auth;
	ഘടന mosquitto *contexts_for_free;
	mosquitto_plugin_id_t **plugins;
	പൂർണ്ണസംഖ്യ plugin_എണ്ണം;
#ifdef WITH_BRIDGE
	ഘടന mosquitto **bridges;
	പൂർണ്ണസംഖ്യ bridge_എണ്ണം;
#endif
	ഘടന clientid__index_hash *clientid_index_hash;
	ഘടന mosquitto_base_സന്ദേശം *സന്ദേശം_store;
	time_t now_s; /* Monotonic clock, where possible */
	time_t now_real_s; /* Read clock, for measuring session/message expiry */
	uint64_t കണ്ണി_id; /* for unique db ids */
	time_t next_event_ms; /* for mux timeout */
	പൂർണ്ണസംഖ്യ സന്ദേശം_store_എണ്ണം;
	unsigned long സന്ദേശം_store_bytes;
	അക്ഷരം *config_file;
	ഘടന mosquitto__config *config;
	bool quiet;
	bool verbose;
#ifdef WITH_SYS_വൃക്ഷം
	പൂർണ്ണസംഖ്യ subscription_എണ്ണം;
	പൂർണ്ണസംഖ്യ shared_subscription_എണ്ണം;
	പൂർണ്ണസംഖ്യ retained_എണ്ണം;
#endif
	പൂർണ്ണസംഖ്യ persistence_changes;
	ഘടന mosquitto *ll_for_free;
#ifdef WITH_EPOLL
	പൂർണ്ണസംഖ്യ epollfd;
#endif
#ifdef WITH_KQUEUE
	പൂർണ്ണസംഖ്യ kqueuefd;
#endif
	ഘടന mosquitto__message_v5 *plugin_msgs;
#ifdef WITH_TLS
	/* tls_keylog can't be in the config struct because it is used
	   before the config is allocated. Config probably
	   shouldn't be separately allocated. */
	അക്ഷരം *tls_keylog;
#endif
	bool shutdown;
};

എണ്ണൽ mosquitto__bridge_direction {
	bd_out = 0,
	bd_in = 1,
	bd_both = 2,
};

എണ്ണൽ mosquitto_bridge_start_type {
	bst_automatic = 0,
	bst_lazy = 1,
	bst_manual = 2,
	bst_once = 3,
};

എണ്ണൽ mosquitto_bridge_reload_type {
	brt_lazy = 0,
	brt_immediate = 1,
};

ഘടന mosquitto__bridge_topic {
	ഘടന mosquitto__bridge_topic *next;
	അക്ഷരം *topic;
	അക്ഷരം *local_prefix;
	അക്ഷരം *remote_prefix;
	അക്ഷരം *local_topic; /* topic prefixed with local_prefix */
	അക്ഷരം *remote_topic; /* topic prefixed with remote_prefix */
	എണ്ണൽ mosquitto__bridge_direction direction;
	uint8_t qos;
};

ഘടന bridge_address {
	അക്ഷരം *address;
	uint16_t port;
};

ഘടന mosquitto__bridge {
	അക്ഷരം *name;
	ഘടന bridge_address *addresses;
	പൂർണ്ണസംഖ്യ cur_address;
	പൂർണ്ണസംഖ്യ address_എണ്ണം;
	time_t primary_retry;
	mosq_sock_t primary_retry_sock;
	bool round_robin;
	bool try_private;
	bool try_private_accepted;
	bool clean_start;
	int8_t clean_start_local;
	uint16_t keepalive;
	unsigned പൂർണ്ണസംഖ്യ tcp_keepalive_idle;
	unsigned പൂർണ്ണസംഖ്യ tcp_keepalive_interval;
	unsigned പൂർണ്ണസംഖ്യ tcp_keepalive_counter;
#ifdef WITH_TCP_USER_TIMEOUT
	പൂർണ്ണസംഖ്യ tcp_user_timeout;
#endif
	ഘടന mosquitto__bridge_topic *topics;
	പൂർണ്ണസംഖ്യ topic_എണ്ണം;
	bool topic_remapping;
	എണ്ണൽ mosquitto__protocol protocol_version;
	time_t restart_t;
	time_t connected_at;
	അക്ഷരം *remote_clientid;
	അക്ഷരം *remote_username;
	അക്ഷരം *remote_password;
	അക്ഷരം *local_clientid;
	അക്ഷരം *local_username;
	അക്ഷരം *local_password;
	അക്ഷരം *notification_topic;
	അക്ഷരം *bind_address;
	bool notifications;
	bool notifications_local_only;
	എണ്ണൽ mosquitto_bridge_start_type start_type;
	പൂർണ്ണസംഖ്യ idle_timeout;
	പൂർണ്ണസംഖ്യ restart_timeout;
	പൂർണ്ണസംഖ്യ backoff_base;
	പൂർണ്ണസംഖ്യ backoff_cap;
	പൂർണ്ണസംഖ്യ stable_connection_period;
	പൂർണ്ണസംഖ്യ threshold;
	uint32_t maximum_packet_size;
	uint32_t session_expiry_interval;
	uint16_t receive_maximum;
	bool lazy_reconnect;
	bool attempt_unsubscribe;
	bool initial_notification_done;
	bool outgoing_retain;
	എണ്ണൽ mosquitto_bridge_reload_type reload_type;
	uint16_t max_topic_alias;
#ifdef WITH_TLS
	bool tls_insecure;
	bool tls_ocsp_required;
	bool tls_use_os_certs;
	അക്ഷരം *tls_cafile;
	അക്ഷരം *tls_capath;
	അക്ഷരം *tls_certfile;
	അക്ഷരം *tls_keyfile;
	അക്ഷരം *tls_version;
	അക്ഷരം *tls_alpn;
	അക്ഷരം *tls_ciphers;
	അക്ഷരം *tls_13_ciphers;
#  ifdef FINAL_WITH_TLS_PSK
	അക്ഷരം *tls_psk_identity;
	അക്ഷരം *tls_psk;
#  endif
#endif
};

#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
ഘടന libws_mqtt_hack {
	അക്ഷരം *http_dir;
	ഘടന mosquitto__listener *listener;
};

ഘടന libws_mqtt_data {
	ഘടന mosquitto *mosq;
};
#endif

#അന്തര്ഭാവ <net_mosq.h>


ഘടന control_endpoint {
	ഘടന control_endpoint *next, *prev;
	അക്ഷരം topic[];
};

ബാഹ്യ ഘടന mosquitto_db db;

/* ============================================================
 * Main functions
 * ============================================================ */
പൂർണ്ണസംഖ്യ mosquitto_main_loop(ഘടന mosquitto__listener_sock *listensock, പൂർണ്ണസംഖ്യ listensock_എണ്ണം);
ശൂന്യം loop__update_next_event(time_t new_ms);

/* ============================================================
 * Config functions
 * ============================================================ */
/* Initialise config struct to default values. */
ശൂന്യം config__init(ഘടന mosquitto__config *config);
#ifdef WITH_BRIDGE
ശൂന്യം config__bridge_cleanup(ഘടന mosquitto__bridge *bridge);
#endif
/* Parse command line options into config. */
പൂർണ്ണസംഖ്യ config__parse_args(ഘടന mosquitto__config *config, പൂർണ്ണസംഖ്യ argc, അക്ഷരം *argv[]);
/* Read configuration data from config->config_file into config.
 * If reload is true, don't process config options that shouldn't be reloaded (listeners etc)
 * Returns 0 on success, 1 if there is a configuration error or if a file cannot be opened.
 */
പൂർണ്ണസംഖ്യ config__read(ഘടന mosquitto__config *config, bool reload);
/* Free all config data. */
ശൂന്യം config__cleanup(ഘടന mosquitto__config *config);
പൂർണ്ണസംഖ്യ config__get_dir_files(സ്ഥിരം അക്ഷരം *include_dir, അക്ഷരം ***files, പൂർണ്ണസംഖ്യ *file_എണ്ണം);

/* ============================================================
 * Server send functions
 * ============================================================ */
പൂർണ്ണസംഖ്യ send__connack(ഘടന mosquitto *context, uint8_t ack, uint8_t reason_code, സ്ഥിരം mosquitto_property *properties);
പൂർണ്ണസംഖ്യ send__suback(ഘടന mosquitto *context, uint16_t mid, uint32_t payloadlen, സ്ഥിരം ശൂന്യം *payload);
പൂർണ്ണസംഖ്യ send__unsuback(ഘടന mosquitto *context, uint16_t mid, പൂർണ്ണസംഖ്യ reason_code_എണ്ണം, uint8_t *reason_codes, സ്ഥിരം mosquitto_property *properties);
പൂർണ്ണസംഖ്യ send__auth(ഘടന mosquitto *context, uint8_t reason_code, സ്ഥിരം ശൂന്യം *auth_data, uint16_t auth_data_len);

/* ============================================================
 * Network functions
 * ============================================================ */
ശൂന്യം net__broker_init(ശൂന്യം);
ശൂന്യം net__broker_cleanup(ശൂന്യം);
ഘടന mosquitto *net__socket_accept(ഘടന mosquitto__listener_sock *listensock);
പൂർണ്ണസംഖ്യ net__socket_listen(ഘടന mosquitto__listener *listener);
പൂർണ്ണസംഖ്യ net__socket_get_address(mosq_sock_t sock, അക്ഷരം *buf, size_t len, uint16_t *remote_address);
പൂർണ്ണസംഖ്യ net__tls_load_verify(ഘടന mosquitto__listener *listener);
പൂർണ്ണസംഖ്യ net__tls_server_ctx(ഘടന mosquitto__listener *listener);
പൂർണ്ണസംഖ്യ net__load_certificates(ഘടന mosquitto__listener *listener);

/* ============================================================
 * Read handling functions
 * ============================================================ */
പൂർണ്ണസംഖ്യ handle__packet(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ handle__connack(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ handle__connect(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ handle__disconnect(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ handle__publish(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ handle__subscribe(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ handle__unsubscribe(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ handle__auth(ഘടന mosquitto *context);

/* ============================================================
 * Database handling
 * ============================================================ */
പൂർണ്ണസംഖ്യ db__open(ഘടന mosquitto__config *config);
പൂർണ്ണസംഖ്യ db__close(ശൂന്യം);
#ifdef WITH_PERSISTENCE
പൂർണ്ണസംഖ്യ persist__backup(bool shutdown);
പൂർണ്ണസംഖ്യ persist__restore(ശൂന്യം);
#endif
/* Return the number of in-flight messages in count. */
പൂർണ്ണസംഖ്യ db_message_എണ്ണം(പൂർണ്ണസംഖ്യ *എണ്ണം);
പൂർണ്ണസംഖ്യ db__message_delete_outgoing(ഘടന mosquitto *context, uint16_t mid, എണ്ണൽ mosquitto_സന്ദേശം_state expect_state, പൂർണ്ണസംഖ്യ qos);
പൂർണ്ണസംഖ്യ db_message_തിരുകുക_outgoing(ഘടന mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം, uint32_t subscription_identifier, bool update, bool persist);
പൂർണ്ണസംഖ്യ db_message_തിരുകുക_incoming(ഘടന mosquitto *context, uint64_t cmsg_id, ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം, bool persist);
പൂർണ്ണസംഖ്യ db__message_remove_incoming(ഘടന mosquitto *context, uint16_t mid);
പൂർണ്ണസംഖ്യ db__message_release_incoming(ഘടന mosquitto *context, uint16_t mid);
പൂർണ്ണസംഖ്യ db__message_update_outgoing(ഘടന mosquitto *context, uint16_t mid, എണ്ണൽ mosquitto_സന്ദേശം_state state, പൂർണ്ണസംഖ്യ qos, bool persist);
ശൂന്യം db__message_dequeue_first(ഘടന mosquitto *context, ഘടന mosquitto_സന്ദേശം_data *സന്ദേശം_data);
പൂർണ്ണസംഖ്യ db__messages_delete(ഘടന mosquitto *context, bool force_free);
പൂർണ്ണസംഖ്യ db__messages_delete_incoming(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ db__messages_delete_outgoing(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ db__messages_easy_queue(ഘടന mosquitto *context, സ്ഥിരം അക്ഷരം *topic, uint8_t qos, uint32_t payloadlen, സ്ഥിരം ശൂന്യം *payload, പൂർണ്ണസംഖ്യ retain, uint32_t message_expiry_interval, mosquitto_property **properties);
പൂർണ്ണസംഖ്യ db__message_store(സ്ഥിരം ഘടന mosquitto *source, ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം, uint32_t *message_expiry_interval, എണ്ണൽ mosquitto_സന്ദേശം_origin origin);
പൂർണ്ണസംഖ്യ db__message_store_find(ഘടന mosquitto *context, uint16_t mid, ഘടന mosquitto_client_സന്ദേശം **client_സന്ദേശം);
പൂർണ്ണസംഖ്യ db_സന്ദേശം_store_add(ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം);
ശൂന്യം db_സന്ദേശം_store_remove(ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം, bool notify);
ശൂന്യം db_സന്ദേശം_store_ref_inc(ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം);
ശൂന്യം db_സന്ദേശം_store_ref_dec(ഘടന mosquitto_base_സന്ദേശം **base_സന്ദേശം);
ശൂന്യം db_സന്ദേശം_store_clean(ശൂന്യം);
ശൂന്യം db_സന്ദേശം_store_compact(ശൂന്യം);
ശൂന്യം db_സന്ദേശം_store_free(ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം);
പൂർണ്ണസംഖ്യ db__message_reconnect_reset(ഘടന mosquitto *context);
bool db__ready_for_flight(ഘടന mosquitto *context, എണ്ണൽ mosquitto_സന്ദേശം_direction dir, പൂർണ്ണസംഖ്യ qos);
bool db__ready_for_queue(ഘടന mosquitto *context, പൂർണ്ണസംഖ്യ qos, ഘടന mosquitto_സന്ദേശം_data *സന്ദേശം_data);
ശൂന്യം sys_വൃക്ഷം_init(ശൂന്യം);
ശൂന്യം sys_വൃക്ഷം_update(bool force);
പൂർണ്ണസംഖ്യ db__message_write_inflight_out_all(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ db__message_write_inflight_out_latest(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ db__message_write_queued_out(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ db__message_write_queued_in(ഘടന mosquitto *context);
ശൂന്യം db_സന്ദേശം_add_to_inflight_stats(ഘടന mosquitto_സന്ദേശം_data *സന്ദേശം_data, ഘടന mosquitto_client_സന്ദേശം *സന്ദേശം);
ശൂന്യം db_സന്ദേശം_add_to_queued_stats(ഘടന mosquitto_സന്ദേശം_data *സന്ദേശം_data, ഘടന mosquitto_client_സന്ദേശം *സന്ദേശം);
uint64_t db_new_സന്ദേശം_id(ശൂന്യം);
ശൂന്യം db__expire_all_messages(ഘടന mosquitto *context);
ശൂന്യം db__check_acl_of_all_messages(ഘടന mosquitto *context);

/* ============================================================
 * Subscription functions
 * ============================================================ */
പൂർണ്ണസംഖ്യ sub__init(ശൂന്യം);
പൂർണ്ണസംഖ്യ sub__add(ഘടന mosquitto *context, സ്ഥിരം ഘടന mosquitto_subscription *sub);
പൂർണ്ണസംഖ്യ sub__remove(ഘടന mosquitto *context, സ്ഥിരം അക്ഷരം *sub, uint8_t *reason);
ശൂന്യം sub_വൃക്ഷം_print(ഘടന mosquitto__subhier *വേര്, പൂർണ്ണസംഖ്യ level);
പൂർണ്ണസംഖ്യ sub__clean_session(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ sub__messages_queue(സ്ഥിരം അക്ഷരം *source_id, uint8_t qos, ഘടന mosquitto_base_സന്ദേശം **base_സന്ദേശം);
പൂർണ്ണസംഖ്യ sub__topic_tokenise(സ്ഥിരം അക്ഷരം *subtopic, അക്ഷരം **local_sub, അക്ഷരം ***topics, സ്ഥിരം അക്ഷരം **sharename);
ശൂന്യം sub__topic_tokens_free(ഘടന sub__token *tokens);

/* ============================================================
 * Context functions
 * ============================================================ */
ഘടന mosquitto *context__init(ശൂന്യം);
പൂർണ്ണസംഖ്യ context__init_sock(ഘടന mosquitto *context, mosq_sock_t sock, bool get_address);
ശൂന്യം context__cleanup(ഘടന mosquitto *context, bool force_free);
ശൂന്യം context__disconnect(ഘടന mosquitto *context, പൂർണ്ണസംഖ്യ reason);
ശൂന്യം context__add_to_disused(ഘടന mosquitto *context);
ശൂന്യം context__free_disused(ശൂന്യം);
ശൂന്യം context__send_will(ഘടന mosquitto *context);
ശൂന്യം context__add_to_by_id(ഘടന mosquitto *context);
ശൂന്യം context__remove_from_by_id(ഘടന mosquitto *context);

പൂർണ്ണസംഖ്യ connect__on_authorised(ഘടന mosquitto *context, ശൂന്യം *auth_data_out, uint16_t auth_data_out_len);


/* ============================================================
 * Control functions
 * ============================================================ */
#ifdef WITH_CONTROL
പൂർണ്ണസംഖ്യ control__process(ഘടന mosquitto *context, ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം);
ശൂന്യം control__cleanup(ശൂന്യം);
#endif
പൂർണ്ണസംഖ്യ control__register_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, സ്ഥിരം അക്ഷരം *topic, ശൂന്യം *userdata);
പൂർണ്ണസംഖ്യ control__unregister_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, സ്ഥിരം അക്ഷരം *topic);
ശൂന്യം control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier);


/* ============================================================
 * Logging functions
 * ============================================================ */
പൂർണ്ണസംഖ്യ log__init(ഘടന mosquitto__config *config);
പൂർണ്ണസംഖ്യ log__close(ഘടന mosquitto__config *config);
ശൂന്യം log__internal(സ്ഥിരം അക്ഷരം *fmt, ...) __attribute__((format(printf, 1, 2)));

/* ============================================================
 * Bridge functions
 * ============================================================ */
#ifdef WITH_BRIDGE
ശൂന്യം bridge__start_all(ശൂന്യം);
ശൂന്യം bridge__reload(ശൂന്യം);
ശൂന്യം bridge__db_cleanup(ശൂന്യം);
ശൂന്യം bridge__cleanup(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ bridge__connect(ഘടന mosquitto *context);
#എങ്കിൽ defined(__GLIBC__) && defined(WITH_ADNS)
പൂർണ്ണസംഖ്യ bridge__connect_step3(ഘടന mosquitto *context);
#endif
പൂർണ്ണസംഖ്യ bridge__on_connect(ഘടന mosquitto *context);
ശൂന്യം bridge_check(ശൂന്യം);
പൂർണ്ണസംഖ്യ bridge__register_local_connections(ശൂന്യം);
പൂർണ്ണസംഖ്യ bridge__add_topic(ഘടന mosquitto__bridge *bridge, സ്ഥിരം അക്ഷരം *topic, എണ്ണൽ mosquitto__bridge_direction direction, uint8_t qos, സ്ഥിരം അക്ഷരം *local_prefix, സ്ഥിരം അക്ഷരം *remote_prefix);
ശൂന്യം bridge__cleanup_topics(ഘടന mosquitto__bridge *bridge);
പൂർണ്ണസംഖ്യ bridge__remap_topic_in(ഘടന mosquitto *context, അക്ഷരം **topic);
#endif

/* ============================================================
 * IO multiplex related functions
 * ============================================================ */
പൂർണ്ണസംഖ്യ mux__init(ശൂന്യം);
പൂർണ്ണസംഖ്യ mux__add_listeners(ഘടന mosquitto__listener_sock *listensock, പൂർണ്ണസംഖ്യ listensock_എണ്ണം);
പൂർണ്ണസംഖ്യ mux__delete_listeners(ഘടന mosquitto__listener_sock *listensock, പൂർണ്ണസംഖ്യ listensock_എണ്ണം);
പൂർണ്ണസംഖ്യ mux__loop_prepare(ശൂന്യം);
പൂർണ്ണസംഖ്യ mux__new(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ mux__add_out(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ mux__remove_out(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ mux__delete(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ mux__wait(ശൂന്യം);
പൂർണ്ണസംഖ്യ mux__handle(ഘടന mosquitto__listener_sock *listensock, പൂർണ്ണസംഖ്യ listensock_എണ്ണം);
പൂർണ്ണസംഖ്യ mux__cleanup(ശൂന്യം);

/* ============================================================
 * Listener related functions
 * ============================================================ */
ബാഹ്യ ഘടന mosquitto__listener_sock *g_listensock;
ബാഹ്യ പൂർണ്ണസംഖ്യ g_listensock_എണ്ണം;

ശൂന്യം listener__set_defaults(ഘടന mosquitto__listener *listener);
ശൂന്യം listeners__reload_all_certificates(ശൂന്യം);
#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
ശൂന്യം listeners__add_websockets(ഘടന lws_context *ws_context, mosq_sock_t fd);
#endif
പൂർണ്ണസംഖ്യ listeners__start(ശൂന്യം);
ശൂന്യം listeners__stop(ശൂന്യം);

/* ============================================================
 * Plugin related functions
 * ============================================================ */
പൂർണ്ണസംഖ്യ plugin__load_v5(mosquitto_plugin_id_t *plugin, ശൂന്യം *lib);
പൂർണ്ണസംഖ്യ plugin__load_v4(mosquitto_plugin_id_t *plugin, ശൂന്യം *lib);
പൂർണ്ണസംഖ്യ plugin__load_v3(mosquitto_plugin_id_t *plugin, ശൂന്യം *lib);
പൂർണ്ണസംഖ്യ plugin__load_v2(mosquitto_plugin_id_t *plugin, ശൂന്യം *lib);
പൂർണ്ണസംഖ്യ acl__pre_check(mosquitto_plugin_id_t *plugin, ഘടന mosquitto *context, പൂർണ്ണസംഖ്യ access);

ശൂന്യം LIB_ERROR(ശൂന്യം);
ശൂന്യം plugin__handle_connect(ഘടന mosquitto *context);
ശൂന്യം plugin__handle_disconnect(ഘടന mosquitto *context, പൂർണ്ണസംഖ്യ reason);
ശൂന്യം plugin__handle_client_offline(ഘടന mosquitto *context, പൂർണ്ണസംഖ്യ reason);
പൂർണ്ണസംഖ്യ plugin__handle_message_in(ഘടന mosquitto *context, ഘടന mosquitto_base_സന്ദേശം_2 *base_സന്ദേശം);
പൂർണ്ണസംഖ്യ plugin__handle_message_out(ഘടന mosquitto *context, ഘടന mosquitto_base_സന്ദേശം_2 *base_സന്ദേശം);
പൂർണ്ണസംഖ്യ plugin__handle_subscribe(ഘടന mosquitto *context, ഘടന mosquitto_subscription *sub);
പൂർണ്ണസംഖ്യ plugin__handle_unsubscribe(ഘടന mosquitto *context, ഘടന mosquitto_subscription *sub);
പൂർണ്ണസംഖ്യ plugin__handle_reload(ശൂന്യം);
ശൂന്യം plugin__handle_tick(ശൂന്യം);
പൂർണ്ണസംഖ്യ plugin__callback_unregister_all(mosquitto_plugin_id_t *identifier);
ശൂന്യം plugin_persist__handle_restore(ശൂന്യം);
ശൂന്യം plugin_persist__handle_client_add(ഘടന mosquitto *context);
ശൂന്യം plugin_persist__handle_client_delete(ഘടന mosquitto *context);
ശൂന്യം plugin_persist__handle_client_update(ഘടന mosquitto *context);
ശൂന്യം plugin_persist__handle_subscription_add(ഘടന mosquitto *context, സ്ഥിരം ഘടന mosquitto_subscription *sub);
ശൂന്യം plugin_persist__handle_subscription_delete(ഘടന mosquitto *context, അക്ഷരം *sub);
ശൂന്യം plugin_persist_handle_client_സന്ദേശം_add(ഘടന mosquitto *context, സ്ഥിരം ഘടന mosquitto_client_സന്ദേശം *cmsg);
ശൂന്യം plugin_persist_handle_client_സന്ദേശം_delete(ഘടന mosquitto *context, സ്ഥിരം ഘടന mosquitto_client_സന്ദേശം *cmsg);
ശൂന്യം plugin_persist_handle_client_സന്ദേശം_update(ഘടന mosquitto *context, സ്ഥിരം ഘടന mosquitto_client_സന്ദേശം *cmsg);
ശൂന്യം plugin_persist_handle_base_സന്ദേശം_add(ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം);
ശൂന്യം plugin_persist_handle_base_സന്ദേശം_delete(ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം);
ശൂന്യം plugin_persist_handle_retain_സന്ദേശം_set(ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം);
ശൂന്യം plugin_persist_handle_retain_സന്ദേശം_delete(ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം);
ശൂന്യം plugin_persist__handle_will_add(ഘടന mosquitto *context);
ശൂന്യം plugin_persist__handle_will_delete(ഘടന mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
പൂർണ്ണസംഖ്യ keepalive__init(ശൂന്യം);
ശൂന്യം keepalive__cleanup(ശൂന്യം);
പൂർണ്ണസംഖ്യ keepalive__add(ഘടന mosquitto *context);
ശൂന്യം keepalive__check(ശൂന്യം);
പൂർണ്ണസംഖ്യ keepalive__remove(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ keepalive__update(ഘടന mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
പൂർണ്ണസംഖ്യ property__process_connect(ഘടന mosquitto *context, mosquitto_property **props);
പൂർണ്ണസംഖ്യ property__process_will(ഘടന mosquitto *context, ഘടന mosquitto_message_all *സന്ദേശം, mosquitto_property **props);
പൂർണ്ണസംഖ്യ property__process_publish(ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം, mosquitto_property **props, പൂർണ്ണസംഖ്യ *topic_alias, uint32_t *message_expiry_interval, bool is_bridge);
പൂർണ്ണസംഖ്യ property__process_disconnect(ഘടന mosquitto *context, mosquitto_property **props);

/* ============================================================
 * Retain tree related functions
 * ============================================================ */
പൂർണ്ണസംഖ്യ retain__init(ശൂന്യം);
ശൂന്യം retain__clean(ഘടന mosquitto__retainhier **retainhier);
പൂർണ്ണസംഖ്യ retain__queue(ഘടന mosquitto *context, സ്ഥിരം ഘടന mosquitto_subscription *sub);
പൂർണ്ണസംഖ്യ retain__store(സ്ഥിരം അക്ഷരം *topic, ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം, അക്ഷരം **split_topics, bool persist);
ശൂന്യം retain__expiry_check(ശൂന്യം);
ശൂന്യം retain__expire(ഘടന mosquitto__retainhier **retainhier);

/* ============================================================
 * Security related functions
 * ============================================================ */
പൂർണ്ണസംഖ്യ acl__find_acls(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ plugin__load_all(ശൂന്യം);
പൂർണ്ണസംഖ്യ plugin__unload_all(ശൂന്യം);
പൂർണ്ണസംഖ്യ config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, ഘടന mosquitto__security_options *security_options);

പൂർണ്ണസംഖ്യ mosquitto_security_init(bool reload);
പൂർണ്ണസംഖ്യ mosquitto_security_cleanup(bool reload);
പൂർണ്ണസംഖ്യ mosquitto_acl_check(ഘടന mosquitto *context, സ്ഥിരം അക്ഷരം *topic, uint32_t payloadlen, ശൂന്യം *payload, uint8_t qos, bool retain, mosquitto_property *properties, പൂർണ്ണസംഖ്യ access);
പൂർണ്ണസംഖ്യ mosquitto_basic_auth(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ mosquitto_psk_താക്കോൽ_get(ഘടന mosquitto *context, സ്ഥിരം അക്ഷരം *hint, സ്ഥിരം അക്ഷരം *identity, അക്ഷരം *താക്കോൽ, പൂർണ്ണസംഖ്യ max_താക്കോൽ_len);

പൂർണ്ണസംഖ്യ mosquitto_security_init_default(ശൂന്യം);
പൂർണ്ണസംഖ്യ mosquitto_security_apply_default(ശൂന്യം);
പൂർണ്ണസംഖ്യ mosquitto_security_cleanup_default(ശൂന്യം);
പൂർണ്ണസംഖ്യ mosquitto_psk_താക്കോൽ_get_default(ഘടന mosquitto *context, സ്ഥിരം അക്ഷരം *hint, സ്ഥിരം അക്ഷരം *identity, അക്ഷരം *താക്കോൽ, പൂർണ്ണസംഖ്യ max_താക്കോൽ_len);
പൂർണ്ണസംഖ്യ broker_acl_file__init(ശൂന്യം);
ശൂന്യം broker_acl_file__cleanup(ശൂന്യം);
പൂർണ്ണസംഖ്യ broker_password_file__init(ശൂന്യം);
ശൂന്യം broker_password_file__cleanup(ശൂന്യം);
പൂർണ്ണസംഖ്യ psk_file__init(ശൂന്യം);
പൂർണ്ണസംഖ്യ psk_file__cleanup(ശൂന്യം);

പൂർണ്ണസംഖ്യ mosquitto_security_auth_start(ഘടന mosquitto *context, bool reauth, സ്ഥിരം ശൂന്യം *data_in, uint16_t data_in_len, ശൂന്യം **data_out, uint16_t *data_out_len);
പൂർണ്ണസംഖ്യ mosquitto_security_auth_continue(ഘടന mosquitto *context, സ്ഥിരം ശൂന്യം *data_in, uint16_t data_len, ശൂന്യം **data_out, uint16_t *data_out_len);

ശൂന്യം unpwd__free_item(ഘടന mosquitto__unpwd **unpwd, ഘടന mosquitto__unpwd *item);

/* ============================================================
 * Session expiry
 * ============================================================ */
പൂർണ്ണസംഖ്യ session_expiry__add(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ session_expiry__add_from_persistence(ഘടന mosquitto *context, time_t expiry_time);
ശൂന്യം session_expiry__add_on_shutdown(ശൂന്യം);
ശൂന്യം session_expiry__remove(ഘടന mosquitto *context);
ശൂന്യം session_expiry__remove_all(ശൂന്യം);
ശൂന്യം session_expiry__check(ശൂന്യം);
ശൂന്യം session_expiry__send_all(ശൂന്യം);

/* ============================================================
 * Signals
 * ============================================================ */
ശൂന്യം signal__setup(ശൂന്യം);
പൂർണ്ണസംഖ്യ signal__flag_check(ശൂന്യം);

/* ============================================================
 * Window service and signal related functions
 * ============================================================ */
#എങ്കിൽ defined(WIN32) || defined(__CYGWIN__)
ശൂന്യം service_install(അക്ഷരം *name);
ശൂന്യം service_uninstall(അക്ഷരം *name);
ശൂന്യം service_run(അക്ഷരം *name);

#ifdef WIN32
DWORD WINAPI SigThreadProc(ശൂന്യം *data);
#endif
#endif

/* ============================================================
 * Watchdog
 * ============================================================ */
ശൂന്യം watchdog__init(ശൂന്യം);
ശൂന്യം watchdog__check(ശൂന്യം);

/* ============================================================
 * Websockets related functions
 * ============================================================ */
#ifdef WITH_WEBSOCKETS
#  എങ്കിൽ WITH_WEBSOCKETS == WS_IS_LWS
ശൂന്യം mosq_websockets_init(ഘടന mosquitto__listener *listener, സ്ഥിരം ഘടന mosquitto__config *conf);
#  endif
പൂർണ്ണസംഖ്യ http__context_init(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ http__context_cleanup(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ http__read(ഘടന mosquitto *context);
#endif
ശൂന്യം do_disconnect(ഘടന mosquitto *context, പൂർണ്ണസംഖ്യ reason);

/* ============================================================
 * PROXY related functions
 * ============================================================ */
പൂർണ്ണസംഖ്യ proxy_v2__read(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ proxy_v1__read(ഘടന mosquitto *context);

/* ============================================================
 * Will delay
 * ============================================================ */
പൂർണ്ണസംഖ്യ will_delay__add(ഘടന mosquitto *context);
ശൂന്യം will_delay__check(ശൂന്യം);
ശൂന്യം will_delay__send_all(ശൂന്യം);
ശൂന്യം will_delay__remove(ഘടന mosquitto *mosq);

/* ============================================================
 * HTTP Info
 * ============================================================ */
പൂർണ്ണസംഖ്യ http_api__start_local(ഘടന mosquitto__listener *listener);
പൂർണ്ണസംഖ്യ http_api__start(ഘടന mosquitto__listener *listener);
ശൂന്യം http_api__stop(ഘടന mosquitto__listener *listener);


/* ============================================================
 * Other
 * ============================================================ */
#ifdef WITH_XTREPORT
ശൂന്യം xtreport(ശൂന്യം);
#endif

ശൂന്യം broker_control__init(ശൂന്യം);
ശൂന്യം broker_control__cleanup(ശൂന്യം);
ശൂന്യം broker_control__reload(ശൂന്യം);

#endif
