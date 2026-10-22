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

#অন্তর্ভাৱ "config.h"
#অন্তর্ভাৱ <stdio.h>

#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#  যদি LWS_LIBRARY_VERSION_সংখ্যা >= 3002000 && !defined(LWS_WITH_EXTERNAL_POLL)
#    warning "libwebsockets is not compiled with LWS_WITH_EXTERNAL_POLL support. Websocket performance will be unusable."
#  endif
#endif

#ifdef WITH_HTTP_API
#  include <microhttpd.h>
#endif

#ifdef __linux__
#define WITH_TCP_USER_TIMEOUT
#endif

#অন্তর্ভাৱ "mosquitto_internal.h"
#অন্তর্ভাৱ "mosquitto/broker.h"
#অন্তর্ভাৱ "mosquitto/broker_plugin.h"
#অন্তর্ভাৱ "mosquitto.h"
#অন্তর্ভাৱ "logging_mosq.h"
#অন্তর্ভাৱ "tls_mosq.h"
#অন্তর্ভাৱ "uthash.h"
#অন্তর্ভাৱ "acl_file.h"
#অন্তর্ভাৱ "password_file.h"

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

প্রকারনির্দেশ uint64_t dbid_t;

প্রকারনির্দেশ পূর্ণাঙ্ক (*FUNC_plugin_init_v5)(mosquitto_plugin_id_t *, শূন্য **, সংরচনা mosquitto_opt *, পূর্ণাঙ্ক);
প্রকারনির্দেশ পূর্ণাঙ্ক (*FUNC_plugin_cleanup_v5)(শূন্য *, সংরচনা mosquitto_opt *, পূর্ণাঙ্ক);

প্রকারনির্দেশ পূর্ণাঙ্ক (*FUNC_auth_plugin_init_v4)(শূন্য **, সংরচনা mosquitto_opt *, পূর্ণাঙ্ক);
প্রকারনির্দেশ পূর্ণাঙ্ক (*FUNC_auth_plugin_cleanup_v4)(শূন্য *, সংরচনা mosquitto_opt *, পূর্ণাঙ্ক);
প্রকারনির্দেশ পূর্ণাঙ্ক (*FUNC_auth_plugin_security_init_v4)(শূন্য *, সংরচনা mosquitto_opt *, পূর্ণাঙ্ক, bool);
প্রকারনির্দেশ পূর্ণাঙ্ক (*FUNC_auth_plugin_security_cleanup_v4)(শূন্য *, সংরচনা mosquitto_opt *, পূর্ণাঙ্ক, bool);
প্রকারনির্দেশ পূর্ণাঙ্ক (*FUNC_auth_plugin_acl_check_v4)(শূন্য *, পূর্ণাঙ্ক, সংরচনা mosquitto *, সংরচনা mosquitto_acl_বাৰ্তা *);
প্রকারনির্দেশ পূর্ণাঙ্ক (*FUNC_auth_plugin_unpwd_check_v4)(শূন্য *, সংরচনা mosquitto *, ধ্রুৱ ৱর্ণ *, ধ্রুৱ ৱর্ণ *);
প্রকারনির্দেশ পূর্ণাঙ্ক (*FUNC_auth_plugin_psk_চাবি_get_v4)(শূন্য *, সংরচনা mosquitto *, ধ্রুৱ ৱর্ণ *, ধ্রুৱ ৱর্ণ *, ৱর্ণ *, পূর্ণাঙ্ক);
প্রকারনির্দেশ পূর্ণাঙ্ক (*FUNC_auth_plugin_auth_start_v4)(শূন্য *, সংরচনা mosquitto *, ধ্রুৱ ৱর্ণ *, bool, ধ্রুৱ শূন্য *, uint16_t, শূন্য **, uint16_t *);
প্রকারনির্দেশ পূর্ণাঙ্ক (*FUNC_auth_plugin_auth_continue_v4)(শূন্য *, সংরচনা mosquitto *, ধ্রুৱ ৱর্ণ *, ধ্রুৱ শূন্য *, uint16_t, শূন্য **, uint16_t *);

প্রকারনির্দেশ পূর্ণাঙ্ক (*FUNC_auth_plugin_init_v3)(শূন্য **, সংরচনা mosquitto_opt *, পূর্ণাঙ্ক);
প্রকারনির্দেশ পূর্ণাঙ্ক (*FUNC_auth_plugin_cleanup_v3)(শূন্য *, সংরচনা mosquitto_opt *, পূর্ণাঙ্ক);
প্রকারনির্দেশ পূর্ণাঙ্ক (*FUNC_auth_plugin_security_init_v3)(শূন্য *, সংরচনা mosquitto_opt *, পূর্ণাঙ্ক, bool);
প্রকারনির্দেশ পূর্ণাঙ্ক (*FUNC_auth_plugin_security_cleanup_v3)(শূন্য *, সংরচনা mosquitto_opt *, পূর্ণাঙ্ক, bool);
প্রকারনির্দেশ পূর্ণাঙ্ক (*FUNC_auth_plugin_acl_check_v3)(শূন্য *, পূর্ণাঙ্ক, ধ্রুৱ সংরচনা mosquitto *, সংরচনা mosquitto_acl_বাৰ্তা *);
প্রকারনির্দেশ পূর্ণাঙ্ক (*FUNC_auth_plugin_unpwd_check_v3)(শূন্য *, ধ্রুৱ সংরচনা mosquitto *, ধ্রুৱ ৱর্ণ *, ধ্রুৱ ৱর্ণ *);
প্রকারনির্দেশ পূর্ণাঙ্ক (*FUNC_auth_plugin_psk_চাবি_get_v3)(শূন্য *, ধ্রুৱ সংরচনা mosquitto *, ধ্রুৱ ৱর্ণ *, ধ্রুৱ ৱর্ণ *, ৱর্ণ *, পূর্ণাঙ্ক);

প্রকারনির্দেশ পূর্ণাঙ্ক (*FUNC_auth_plugin_init_v2)(শূন্য **, সংরচনা mosquitto_auth_opt *, পূর্ণাঙ্ক);
প্রকারনির্দেশ পূর্ণাঙ্ক (*FUNC_auth_plugin_cleanup_v2)(শূন্য *, সংরচনা mosquitto_auth_opt *, পূর্ণাঙ্ক);
প্রকারনির্দেশ পূর্ণাঙ্ক (*FUNC_auth_plugin_security_init_v2)(শূন্য *, সংরচনা mosquitto_auth_opt *, পূর্ণাঙ্ক, bool);
প্রকারনির্দেশ পূর্ণাঙ্ক (*FUNC_auth_plugin_security_cleanup_v2)(শূন্য *, সংরচনা mosquitto_auth_opt *, পূর্ণাঙ্ক, bool);
প্রকারনির্দেশ পূর্ণাঙ্ক (*FUNC_auth_plugin_acl_check_v2)(শূন্য *, ধ্রুৱ ৱর্ণ *, ধ্রুৱ ৱর্ণ *, ধ্রুৱ ৱর্ণ *, পূর্ণাঙ্ক);
প্রকারনির্দেশ পূর্ণাঙ্ক (*FUNC_auth_plugin_unpwd_check_v2)(শূন্য *, ধ্রুৱ ৱর্ণ *, ধ্রুৱ ৱর্ণ *);
প্রকারনির্দেশ পূর্ণাঙ্ক (*FUNC_auth_plugin_psk_চাবি_get_v2)(শূন্য *, ধ্রুৱ ৱর্ণ *, ধ্রুৱ ৱর্ণ *, ৱর্ণ *, পূর্ণাঙ্ক);


গণনা mosquitto_বাৰ্তা_origin {
	mosq_mo_client = 0,
	mosq_mo_broker = 1,
};

সংরচনা mosquitto__plugin_lib {
	শূন্য *lib;
	শূন্য *user_data;
	পূর্ণাঙ্ক (*plugin_version)(শূন্য);
	সংরচনা mosquitto_plugin_id_t *identifier;

	FUNC_plugin_init_v5 plugin_init_v5;
	FUNC_plugin_cleanup_v5 plugin_cleanup_v5;

	FUNC_auth_plugin_init_v4 plugin_init_v4;
	FUNC_auth_plugin_cleanup_v4 plugin_cleanup_v4;
	FUNC_auth_plugin_security_init_v4 security_init_v4;
	FUNC_auth_plugin_security_cleanup_v4 security_cleanup_v4;
	FUNC_auth_plugin_acl_check_v4 acl_check_v4;
	FUNC_auth_plugin_unpwd_check_v4 unpwd_check_v4;
	FUNC_auth_plugin_psk_চাবি_get_v4 psk_চাবি_get_v4;
	FUNC_auth_plugin_auth_start_v4 auth_start_v4;
	FUNC_auth_plugin_auth_continue_v4 auth_continue_v4;

	FUNC_auth_plugin_init_v3 plugin_init_v3;
	FUNC_auth_plugin_cleanup_v3 plugin_cleanup_v3;
	FUNC_auth_plugin_security_init_v3 security_init_v3;
	FUNC_auth_plugin_security_cleanup_v3 security_cleanup_v3;
	FUNC_auth_plugin_acl_check_v3 acl_check_v3;
	FUNC_auth_plugin_unpwd_check_v3 unpwd_check_v3;
	FUNC_auth_plugin_psk_চাবি_get_v3 psk_চাবি_get_v3;

	FUNC_auth_plugin_init_v2 plugin_init_v2;
	FUNC_auth_plugin_cleanup_v2 plugin_cleanup_v2;
	FUNC_auth_plugin_security_init_v2 security_init_v2;
	FUNC_auth_plugin_security_cleanup_v2 security_cleanup_v2;
	FUNC_auth_plugin_acl_check_v2 acl_check_v2;
	FUNC_auth_plugin_unpwd_check_v2 unpwd_check_v2;
	FUNC_auth_plugin_psk_চাবি_get_v2 psk_চাবি_get_v2;
	পূর্ণাঙ্ক version;
};

সংরচনা mosquitto__plugin_config {
	ৱর্ণ *path;
	ৱর্ণ *name;
	সংরচনা mosquitto_opt *options;
	সংরচনা mosquitto__security_options **security_options;
	পূর্ণাঙ্ক option_গুনতি;
	পূর্ণাঙ্ক security_option_গুনতি;
	bool deny_special_chars;
};

সংরচনা mosquitto__callback {
	UT_hash_handle hh; /* For callbacks that register for e.g. a specific topic */
	সংরচনা mosquitto__callback *next, *prev; /* For typical callbacks */
	MOSQ_FUNC_generic_callback cb;
	শূন্য *userdata;
	সঙ্ঘ {
		ৱর্ণ *topic;
		সংরচনা timespec next_tick;
	} data;
	mosquitto_plugin_id_t *identifier;
};

সংরচনা plugin__callbacks {
	সংরচনা mosquitto__callback *tick;
	সংরচনা mosquitto__callback *acl_check;
	সংরচনা mosquitto__callback *basic_auth;
	সংরচনা mosquitto__callback *connect;
	সংরচনা mosquitto__callback *control;
	সংরচনা mosquitto__callback *disconnect;
	সংরচনা mosquitto__callback *client_offline;
	সংরচনা mosquitto__callback *ext_auth_continue;
	সংরচনা mosquitto__callback *ext_auth_start;
	সংরচনা mosquitto__callback *message_in;
	সংরচনা mosquitto__callback *message_out;
	সংরচনা mosquitto__callback *psk_চাবি;
	সংরচনা mosquitto__callback *reload;
	সংরচনা mosquitto__callback *subscribe;
	সংরচনা mosquitto__callback *unsubscribe;
	সংরচনা mosquitto__callback *persist_restore;
	সংরচনা mosquitto__callback *persist_client_add;
	সংরচনা mosquitto__callback *persist_client_delete;
	সংরচনা mosquitto__callback *persist_client_update;
	সংরচনা mosquitto__callback *persist_subscription_add;
	সংরচনা mosquitto__callback *persist_subscription_delete;
	সংরচনা mosquitto__callback *persist_client_বাৰ্তা_add;
	সংরচনা mosquitto__callback *persist_client_বাৰ্তা_delete;
	সংরচনা mosquitto__callback *persist_client_বাৰ্তা_update;
	সংরচনা mosquitto__callback *persist_base_বাৰ্তা_add;
	সংরচনা mosquitto__callback *persist_base_বাৰ্তা_delete;
	সংরচনা mosquitto__callback *persist_retain_বাৰ্তা_set;
	সংরচনা mosquitto__callback *persist_retain_বাৰ্তা_delete;
	সংরচনা mosquitto__callback *persist_will_add;
	সংরচনা mosquitto__callback *persist_will_delete;
};

/* This is owned by mosquitto__config or mosquitto__listener, and only referred
 * to by other structs */
সংরচনা mosquitto__security_options {
	/* Any options that get added here also need considering
	 * in config__read() with regards whether allow_anonymous
	 * should be disabled when these options are set.
	 */
	সংরচনা mosquitto__unpwd *unpwd;
	সংরচনা mosquitto__psk *psk_id;
	সংরচনা acl_file_data acl_data;
	সংরচনা password_file_data password_data;
	ৱর্ণ *psk_file;
	mosquitto_plugin_id_t **plugins;
	পূর্ণাঙ্ক plugin_গুনতি;
	int8_t allow_anonymous;
	bool allow_zero_length_clientid;
	ৱর্ণ *auto_id_prefix;
	uint16_t auto_id_prefix_len;
	সংরচনা plugin__callbacks plugin_callbacks;
	mosquitto_plugin_id_t *pid; /* For registering as a "plugin" */
};

#যদি defined(WITH_EPOLL) || defined(WITH_KQUEUE)
গণনা struct_ident {
	id_invalid = 0,
	id_listener = 1,
	id_client = 2,
	id_listener_ws = 3,
};
#endif

সংরচনা mosquitto__listener {
	uint16_t port;
	ৱর্ণ *host;
	ৱর্ণ *bind_interface;
	পূর্ণাঙ্ক max_connections;
	ৱর্ণ *mount_point;
	mosq_sock_t *socks;
	পূর্ণাঙ্ক sock_গুনতি;
	পূর্ণাঙ্ক client_গুনতি;
	গণনা mosquitto_protocol protocol;
	পূর্ণাঙ্ক socket_domain;
	bool use_username_as_clientid;
	uint8_t max_qos;
	uint16_t max_topic_alias;
	uint16_t max_topic_alias_broker;
#ifdef WITH_TLS
	ৱর্ণ *cafile;
	ৱর্ণ *capath;
	ৱর্ণ *certfile;
	ৱর্ণ *keyfile;
	ৱর্ণ *tls_engine;
	ৱর্ণ *tls_engine_kpass_sha1;
	ৱর্ণ *ciphers;
	ৱর্ণ *ciphers_tls13;
	ৱর্ণ *psk_hint;
	SSL_CTX *ssl_ctx;
	ৱর্ণ *crlfile;
	ৱর্ণ *tls_version;
	bool use_identity_as_username;
	bool use_subject_as_username;
	bool require_certificate;
	bool disable_client_cert_date_checks;
	গণনা mosquitto__keyform tls_keyform;
#endif
#ifdef WITH_WEBSOCKETS
#  যদি WITH_WEBSOCKETS == WS_IS_LWS
	সংরচনা lws_context *ws_context;
	bool ws_in_init;
	সংরচনা lws_protocols *ws_protocol;
#  endif
	ৱর্ণ **ws_origins;
	পূর্ণাঙ্ক ws_origin_গুনতি;
#endif
#যদি defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
	ৱর্ণ *http_dir;
#endif
	সংরচনা mosquitto__security_options *security_options;
#ifdef WITH_UNIX_SOCKETS
	ৱর্ণ *unix_socket_path;
#endif
	bool disable_protocol_v3;
	bool disable_protocol_v4;
	bool disable_protocol_v5;
	পূর্ণাঙ্ক enable_proxy_protocol;
	bool proxy_protocol_v2_require_tls;
#ifdef WITH_HTTP_API
	সংরচনা MHD_Daemon *mhd;
#endif
};


সংরচনা mosquitto__listener_sock {
#যদি defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	/* This *must* be the first element in the struct. */
	পূর্ণাঙ্ক ident;
#endif
	mosq_sock_t sock;
	সংরচনা mosquitto__listener *listener;
};


/* Callbacks belonging to a specific plugin
 * Doesn't include MOSQ_EVT_CONTROL events.
 */
সংরচনা plugin_own_callback {
	সংরচনা plugin_own_callback *next, *prev;
	MOSQ_FUNC_generic_callback cb_func;
	গণনা mosquitto_plugin_event event;
};

সংরচনা mosquitto_plugin_id_t {
	সংরচনা mosquitto__plugin_config config;
	সংরচনা mosquitto__plugin_lib lib;
	সংরচনা mosquitto__listener *listener;
	ৱর্ণ *plugin_name;
	ৱর্ণ *plugin_version;
	সংরচনা control_endpoint *control_endpoints;
	সংরচনা plugin_own_callback *own_callbacks;
	সংরচনা timespec next_tick;
};

সংরচনা mosquitto__config {
	bool allow_duplicate_messages;
	পূর্ণাঙ্ক autosave_interval;
	bool autosave_on_changes;
	bool check_retain_source;
	ৱর্ণ *clientid_prefixes;
	bool connection_messages;
	uint16_t cmd_port[CMD_PORT_LIMIT];
	পূর্ণাঙ্ক cmd_port_গুনতি;
	bool daemon;
	bool test_configuration;
	bool enable_control_api;
	পূর্ণাঙ্ক global_max_clients;
	পূর্ণাঙ্ক global_max_connections;
	সংরচনা mosquitto__listener *default_listener; /* Points to one of `listeners` */
	সংরচনা mosquitto__listener *listeners;
	পূর্ণাঙ্ক listener_গুনতি;
	bool local_only;
	unsigned পূর্ণাঙ্ক log_dest;
	পূর্ণাঙ্ক log_facility;
	unsigned পূর্ণাঙ্ক log_type;
	bool log_timestamp;
	ৱর্ণ *log_timestamp_format;
	ৱর্ণ *log_file;
	FILE *log_fptr;
	size_t max_inflight_bytes;
	size_t max_queued_bytes;
	পূর্ণাঙ্ক max_queued_messages;
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
	ৱর্ণ *persistence_location;
	ৱর্ণ *persistence_file;
	ৱর্ণ *persistence_filepath;
	time_t persistent_client_expiration;
	ৱর্ণ *pid_file;
	bool queue_qos0_messages;
	bool per_listener_settings;
	bool retain_available;
	পূর্ণাঙ্ক retain_expiry_interval;
	bool set_tcp_nodelay;
	পূর্ণাঙ্ক sys_interval;
	bool upgrade_outgoing_qos;
	ৱর্ণ *user;
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	পূর্ণাঙ্ক websockets_log_level;
#endif
	uint16_t packet_buffer_size;
#ifdef WITH_BRIDGE
	সংরচনা mosquitto__bridge **bridges;
	পূর্ণাঙ্ক bridge_গুনতি;
#endif
	সংরচনা mosquitto__security_options security_options;
};


সংরচনা mosquitto__subshared {
	UT_hash_handle hh;
	সংরচনা mosquitto__subleaf *subs;
	ৱর্ণ name[];
};

সংরচনা mosquitto__subhier {
	UT_hash_handle hh;
	সংরচনা mosquitto__subhier *parent;
	সংরচনা mosquitto__subhier *children;
	সংরচনা mosquitto__subleaf *subs;
	সংরচনা mosquitto__subshared *shared;
	uint16_t topic_len;
	ৱর্ণ topic[];
};

সংরচনা mosquitto__subleaf {
	সংরচনা mosquitto__subleaf *prev;
	সংরচনা mosquitto__subleaf *next;
	সংরচনা mosquitto *context;
	সংরচনা mosquitto__subhier *hier;
	সংরচনা mosquitto__subshared *shared;
	uint32_t identifier;
	uint8_t subscription_options;
	ৱর্ণ topic_filter[];
};

সংরচনা sub__token {
	সংরচনা sub__token *next;
	uint16_t topic_len;
	ৱর্ণ topic[];
};

সংরচনা mosquitto__retainhier {
	UT_hash_handle hh;
	সংরচনা mosquitto__retainhier *parent;
	সংরচনা mosquitto__retainhier *children;
	সংরচনা mosquitto_base_বাৰ্তা *retained;
	uint16_t topic_len;
	ৱর্ণ topic[];
};

সংরচনা mosquitto_base_বাৰ্তা {
	UT_hash_handle hh;
	সংরচনা mosquitto_base_বাৰ্তা_2 data;
	সংরচনা mosquitto__listener *source_listener;
	ৱর্ণ **dest_ids;
	পূর্ণাঙ্ক dest_id_গুনতি;
	পূর্ণাঙ্ক ref_গুনতি;
	গণনা mosquitto_বাৰ্তা_origin origin;
	bool stored;
};

সংরচনা mosquitto_client_বাৰ্তা {
	সংরচনা mosquitto_client_বাৰ্তা_2 data;
	সংরচনা mosquitto_client_বাৰ্তা *prev;
	সংরচনা mosquitto_client_বাৰ্তা *next;
	সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা;
};


সংরচনা mosquitto__psk {
	UT_hash_handle hh;
	ৱর্ণ *username;
	ৱর্ণ *password;
};

সংরচনা mosquitto__message_v5 {
	সংরচনা mosquitto__message_v5 *next, *prev;
	ৱর্ণ *topic;
	শূন্য *payload;
	mosquitto_property *properties;
	/* clientid is used only by mosquitto_broker_publish*() to indicate
	   this message is for a specific client. */
	ৱর্ণ *clientid;
	পূর্ণাঙ্ক payloadlen;
	পূর্ণাঙ্ক qos;
	bool retain;
};

সংরচনা mosquitto_db {
	dbid_t last_db_id;
	uint64_t গ্ৰন্থি_id_shifted;
	সংরচনা mosquitto__subhier *normal_subs;
	সংরচনা mosquitto__subhier *shared_subs;
	সংরচনা mosquitto__retainhier *retains;
	সংরচনা mosquitto *contexts_by_id;
	সংরচনা mosquitto *contexts_by_sock;
	সংরচনা mosquitto *contexts_by_id_delayed_auth;
	সংরচনা mosquitto *contexts_for_free;
	mosquitto_plugin_id_t **plugins;
	পূর্ণাঙ্ক plugin_গুনতি;
#ifdef WITH_BRIDGE
	সংরচনা mosquitto **bridges;
	পূর্ণাঙ্ক bridge_গুনতি;
#endif
	সংরচনা clientid__index_hash *clientid_index_hash;
	সংরচনা mosquitto_base_বাৰ্তা *বাৰ্তা_store;
	time_t now_s; /* Monotonic clock, where possible */
	time_t now_real_s; /* Read clock, for measuring session/message expiry */
	uint64_t গ্ৰন্থি_id; /* for unique db ids */
	time_t next_event_ms; /* for mux timeout */
	পূর্ণাঙ্ক বাৰ্তা_store_গুনতি;
	unsigned long বাৰ্তা_store_bytes;
	ৱর্ণ *config_file;
	সংরচনা mosquitto__config *config;
	bool quiet;
	bool verbose;
#ifdef WITH_SYS_বৃক্ষ
	পূর্ণাঙ্ক subscription_গুনতি;
	পূর্ণাঙ্ক shared_subscription_গুনতি;
	পূর্ণাঙ্ক retained_গুনতি;
#endif
	পূর্ণাঙ্ক persistence_changes;
	সংরচনা mosquitto *ll_for_free;
#ifdef WITH_EPOLL
	পূর্ণাঙ্ক epollfd;
#endif
#ifdef WITH_KQUEUE
	পূর্ণাঙ্ক kqueuefd;
#endif
	সংরচনা mosquitto__message_v5 *plugin_msgs;
#ifdef WITH_TLS
	/* tls_keylog can't be in the config struct because it is used
	   before the config is allocated. Config probably
	   shouldn't be separately allocated. */
	ৱর্ণ *tls_keylog;
#endif
	bool shutdown;
};

গণনা mosquitto__bridge_direction {
	bd_out = 0,
	bd_in = 1,
	bd_both = 2,
};

গণনা mosquitto_bridge_start_type {
	bst_automatic = 0,
	bst_lazy = 1,
	bst_manual = 2,
	bst_once = 3,
};

গণনা mosquitto_bridge_reload_type {
	brt_lazy = 0,
	brt_immediate = 1,
};

সংরচনা mosquitto__bridge_topic {
	সংরচনা mosquitto__bridge_topic *next;
	ৱর্ণ *topic;
	ৱর্ণ *local_prefix;
	ৱর্ণ *remote_prefix;
	ৱর্ণ *local_topic; /* topic prefixed with local_prefix */
	ৱর্ণ *remote_topic; /* topic prefixed with remote_prefix */
	গণনা mosquitto__bridge_direction direction;
	uint8_t qos;
};

সংরচনা bridge_address {
	ৱর্ণ *address;
	uint16_t port;
};

সংরচনা mosquitto__bridge {
	ৱর্ণ *name;
	সংরচনা bridge_address *addresses;
	পূর্ণাঙ্ক cur_address;
	পূর্ণাঙ্ক address_গুনতি;
	time_t primary_retry;
	mosq_sock_t primary_retry_sock;
	bool round_robin;
	bool try_private;
	bool try_private_accepted;
	bool clean_start;
	int8_t clean_start_local;
	uint16_t keepalive;
	unsigned পূর্ণাঙ্ক tcp_keepalive_idle;
	unsigned পূর্ণাঙ্ক tcp_keepalive_interval;
	unsigned পূর্ণাঙ্ক tcp_keepalive_counter;
#ifdef WITH_TCP_USER_TIMEOUT
	পূর্ণাঙ্ক tcp_user_timeout;
#endif
	সংরচনা mosquitto__bridge_topic *topics;
	পূর্ণাঙ্ক topic_গুনতি;
	bool topic_remapping;
	গণনা mosquitto__protocol protocol_version;
	time_t restart_t;
	time_t connected_at;
	ৱর্ণ *remote_clientid;
	ৱর্ণ *remote_username;
	ৱর্ণ *remote_password;
	ৱর্ণ *local_clientid;
	ৱর্ণ *local_username;
	ৱর্ণ *local_password;
	ৱর্ণ *notification_topic;
	ৱর্ণ *bind_address;
	bool notifications;
	bool notifications_local_only;
	গণনা mosquitto_bridge_start_type start_type;
	পূর্ণাঙ্ক idle_timeout;
	পূর্ণাঙ্ক restart_timeout;
	পূর্ণাঙ্ক backoff_base;
	পূর্ণাঙ্ক backoff_cap;
	পূর্ণাঙ্ক stable_connection_period;
	পূর্ণাঙ্ক threshold;
	uint32_t maximum_packet_size;
	uint32_t session_expiry_interval;
	uint16_t receive_maximum;
	bool lazy_reconnect;
	bool attempt_unsubscribe;
	bool initial_notification_done;
	bool outgoing_retain;
	গণনা mosquitto_bridge_reload_type reload_type;
	uint16_t max_topic_alias;
#ifdef WITH_TLS
	bool tls_insecure;
	bool tls_ocsp_required;
	bool tls_use_os_certs;
	ৱর্ণ *tls_cafile;
	ৱর্ণ *tls_capath;
	ৱর্ণ *tls_certfile;
	ৱর্ণ *tls_keyfile;
	ৱর্ণ *tls_version;
	ৱর্ণ *tls_alpn;
	ৱর্ণ *tls_ciphers;
	ৱর্ণ *tls_13_ciphers;
#  ifdef FINAL_WITH_TLS_PSK
	ৱর্ণ *tls_psk_identity;
	ৱর্ণ *tls_psk;
#  endif
#endif
};

#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
সংরচনা libws_mqtt_hack {
	ৱর্ণ *http_dir;
	সংরচনা mosquitto__listener *listener;
};

সংরচনা libws_mqtt_data {
	সংরচনা mosquitto *mosq;
};
#endif

#অন্তর্ভাৱ <net_mosq.h>


সংরচনা control_endpoint {
	সংরচনা control_endpoint *next, *prev;
	ৱর্ণ topic[];
};

বাহ্য সংরচনা mosquitto_db db;

/* ============================================================
 * Main functions
 * ============================================================ */
পূর্ণাঙ্ক mosquitto_main_loop(সংরচনা mosquitto__listener_sock *listensock, পূর্ণাঙ্ক listensock_গুনতি);
শূন্য loop__update_next_event(time_t new_ms);

/* ============================================================
 * Config functions
 * ============================================================ */
/* Initialise config struct to default values. */
শূন্য config__init(সংরচনা mosquitto__config *config);
#ifdef WITH_BRIDGE
শূন্য config__bridge_cleanup(সংরচনা mosquitto__bridge *bridge);
#endif
/* Parse command line options into config. */
পূর্ণাঙ্ক config__parse_args(সংরচনা mosquitto__config *config, পূর্ণাঙ্ক argc, ৱর্ণ *argv[]);
/* Read configuration data from config->config_file into config.
 * If reload is true, don't process config options that shouldn't be reloaded (listeners etc)
 * Returns 0 on success, 1 if there is a configuration error or if a file cannot be opened.
 */
পূর্ণাঙ্ক config__read(সংরচনা mosquitto__config *config, bool reload);
/* Free all config data. */
শূন্য config__cleanup(সংরচনা mosquitto__config *config);
পূর্ণাঙ্ক config__get_dir_files(ধ্রুৱ ৱর্ণ *include_dir, ৱর্ণ ***files, পূর্ণাঙ্ক *file_গুনতি);

/* ============================================================
 * Server send functions
 * ============================================================ */
পূর্ণাঙ্ক send__connack(সংরচনা mosquitto *context, uint8_t ack, uint8_t reason_code, ধ্রুৱ mosquitto_property *properties);
পূর্ণাঙ্ক send__suback(সংরচনা mosquitto *context, uint16_t mid, uint32_t payloadlen, ধ্রুৱ শূন্য *payload);
পূর্ণাঙ্ক send__unsuback(সংরচনা mosquitto *context, uint16_t mid, পূর্ণাঙ্ক reason_code_গুনতি, uint8_t *reason_codes, ধ্রুৱ mosquitto_property *properties);
পূর্ণাঙ্ক send__auth(সংরচনা mosquitto *context, uint8_t reason_code, ধ্রুৱ শূন্য *auth_data, uint16_t auth_data_len);

/* ============================================================
 * Network functions
 * ============================================================ */
শূন্য net__broker_init(শূন্য);
শূন্য net__broker_cleanup(শূন্য);
সংরচনা mosquitto *net__socket_accept(সংরচনা mosquitto__listener_sock *listensock);
পূর্ণাঙ্ক net__socket_listen(সংরচনা mosquitto__listener *listener);
পূর্ণাঙ্ক net__socket_get_address(mosq_sock_t sock, ৱর্ণ *buf, size_t len, uint16_t *remote_address);
পূর্ণাঙ্ক net__tls_load_verify(সংরচনা mosquitto__listener *listener);
পূর্ণাঙ্ক net__tls_server_ctx(সংরচনা mosquitto__listener *listener);
পূর্ণাঙ্ক net__load_certificates(সংরচনা mosquitto__listener *listener);

/* ============================================================
 * Read handling functions
 * ============================================================ */
পূর্ণাঙ্ক handle__packet(সংরচনা mosquitto *context);
পূর্ণাঙ্ক handle__connack(সংরচনা mosquitto *context);
পূর্ণাঙ্ক handle__connect(সংরচনা mosquitto *context);
পূর্ণাঙ্ক handle__disconnect(সংরচনা mosquitto *context);
পূর্ণাঙ্ক handle__publish(সংরচনা mosquitto *context);
পূর্ণাঙ্ক handle__subscribe(সংরচনা mosquitto *context);
পূর্ণাঙ্ক handle__unsubscribe(সংরচনা mosquitto *context);
পূর্ণাঙ্ক handle__auth(সংরচনা mosquitto *context);

/* ============================================================
 * Database handling
 * ============================================================ */
পূর্ণাঙ্ক db__open(সংরচনা mosquitto__config *config);
পূর্ণাঙ্ক db__close(শূন্য);
#ifdef WITH_PERSISTENCE
পূর্ণাঙ্ক persist__backup(bool shutdown);
পূর্ণাঙ্ক persist__restore(শূন্য);
#endif
/* Return the number of in-flight messages in count. */
পূর্ণাঙ্ক db_message_গুনতি(পূর্ণাঙ্ক *গুনতি);
পূর্ণাঙ্ক db__message_delete_outgoing(সংরচনা mosquitto *context, uint16_t mid, গণনা mosquitto_বাৰ্তা_state expect_state, পূর্ণাঙ্ক qos);
পূর্ণাঙ্ক db_message_ঢোকাও_outgoing(সংরচনা mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা, uint32_t subscription_identifier, bool update, bool persist);
পূর্ণাঙ্ক db_message_ঢোকাও_incoming(সংরচনা mosquitto *context, uint64_t cmsg_id, সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা, bool persist);
পূর্ণাঙ্ক db__message_remove_incoming(সংরচনা mosquitto *context, uint16_t mid);
পূর্ণাঙ্ক db__message_release_incoming(সংরচনা mosquitto *context, uint16_t mid);
পূর্ণাঙ্ক db__message_update_outgoing(সংরচনা mosquitto *context, uint16_t mid, গণনা mosquitto_বাৰ্তা_state state, পূর্ণাঙ্ক qos, bool persist);
শূন্য db__message_dequeue_first(সংরচনা mosquitto *context, সংরচনা mosquitto_বাৰ্তা_data *বাৰ্তা_data);
পূর্ণাঙ্ক db__messages_delete(সংরচনা mosquitto *context, bool force_free);
পূর্ণাঙ্ক db__messages_delete_incoming(সংরচনা mosquitto *context);
পূর্ণাঙ্ক db__messages_delete_outgoing(সংরচনা mosquitto *context);
পূর্ণাঙ্ক db__messages_easy_queue(সংরচনা mosquitto *context, ধ্রুৱ ৱর্ণ *topic, uint8_t qos, uint32_t payloadlen, ধ্রুৱ শূন্য *payload, পূর্ণাঙ্ক retain, uint32_t message_expiry_interval, mosquitto_property **properties);
পূর্ণাঙ্ক db__message_store(ধ্রুৱ সংরচনা mosquitto *source, সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা, uint32_t *message_expiry_interval, গণনা mosquitto_বাৰ্তা_origin origin);
পূর্ণাঙ্ক db__message_store_find(সংরচনা mosquitto *context, uint16_t mid, সংরচনা mosquitto_client_বাৰ্তা **client_বাৰ্তা);
পূর্ণাঙ্ক db_বাৰ্তা_store_add(সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা);
শূন্য db_বাৰ্তা_store_remove(সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা, bool notify);
শূন্য db_বাৰ্তা_store_ref_inc(সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা);
শূন্য db_বাৰ্তা_store_ref_dec(সংরচনা mosquitto_base_বাৰ্তা **base_বাৰ্তা);
শূন্য db_বাৰ্তা_store_clean(শূন্য);
শূন্য db_বাৰ্তা_store_compact(শূন্য);
শূন্য db_বাৰ্তা_store_free(সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা);
পূর্ণাঙ্ক db__message_reconnect_reset(সংরচনা mosquitto *context);
bool db__ready_for_flight(সংরচনা mosquitto *context, গণনা mosquitto_বাৰ্তা_direction dir, পূর্ণাঙ্ক qos);
bool db__ready_for_queue(সংরচনা mosquitto *context, পূর্ণাঙ্ক qos, সংরচনা mosquitto_বাৰ্তা_data *বাৰ্তা_data);
শূন্য sys_বৃক্ষ_init(শূন্য);
শূন্য sys_বৃক্ষ_update(bool force);
পূর্ণাঙ্ক db__message_write_inflight_out_all(সংরচনা mosquitto *context);
পূর্ণাঙ্ক db__message_write_inflight_out_latest(সংরচনা mosquitto *context);
পূর্ণাঙ্ক db__message_write_queued_out(সংরচনা mosquitto *context);
পূর্ণাঙ্ক db__message_write_queued_in(সংরচনা mosquitto *context);
শূন্য db_বাৰ্তা_add_to_inflight_stats(সংরচনা mosquitto_বাৰ্তা_data *বাৰ্তা_data, সংরচনা mosquitto_client_বাৰ্তা *বাৰ্তা);
শূন্য db_বাৰ্তা_add_to_queued_stats(সংরচনা mosquitto_বাৰ্তা_data *বাৰ্তা_data, সংরচনা mosquitto_client_বাৰ্তা *বাৰ্তা);
uint64_t db_new_বাৰ্তা_id(শূন্য);
শূন্য db__expire_all_messages(সংরচনা mosquitto *context);
শূন্য db__check_acl_of_all_messages(সংরচনা mosquitto *context);

/* ============================================================
 * Subscription functions
 * ============================================================ */
পূর্ণাঙ্ক sub__init(শূন্য);
পূর্ণাঙ্ক sub__add(সংরচনা mosquitto *context, ধ্রুৱ সংরচনা mosquitto_subscription *sub);
পূর্ণাঙ্ক sub__remove(সংরচনা mosquitto *context, ধ্রুৱ ৱর্ণ *sub, uint8_t *reason);
শূন্য sub_বৃক্ষ_print(সংরচনা mosquitto__subhier *মূল, পূর্ণাঙ্ক level);
পূর্ণাঙ্ক sub__clean_session(সংরচনা mosquitto *context);
পূর্ণাঙ্ক sub__messages_queue(ধ্রুৱ ৱর্ণ *source_id, uint8_t qos, সংরচনা mosquitto_base_বাৰ্তা **base_বাৰ্তা);
পূর্ণাঙ্ক sub__topic_tokenise(ধ্রুৱ ৱর্ণ *subtopic, ৱর্ণ **local_sub, ৱর্ণ ***topics, ধ্রুৱ ৱর্ণ **sharename);
শূন্য sub__topic_tokens_free(সংরচনা sub__token *tokens);

/* ============================================================
 * Context functions
 * ============================================================ */
সংরচনা mosquitto *context__init(শূন্য);
পূর্ণাঙ্ক context__init_sock(সংরচনা mosquitto *context, mosq_sock_t sock, bool get_address);
শূন্য context__cleanup(সংরচনা mosquitto *context, bool force_free);
শূন্য context__disconnect(সংরচনা mosquitto *context, পূর্ণাঙ্ক reason);
শূন্য context__add_to_disused(সংরচনা mosquitto *context);
শূন্য context__free_disused(শূন্য);
শূন্য context__send_will(সংরচনা mosquitto *context);
শূন্য context__add_to_by_id(সংরচনা mosquitto *context);
শূন্য context__remove_from_by_id(সংরচনা mosquitto *context);

পূর্ণাঙ্ক connect__on_authorised(সংরচনা mosquitto *context, শূন্য *auth_data_out, uint16_t auth_data_out_len);


/* ============================================================
 * Control functions
 * ============================================================ */
#ifdef WITH_CONTROL
পূর্ণাঙ্ক control__process(সংরচনা mosquitto *context, সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা);
শূন্য control__cleanup(শূন্য);
#endif
পূর্ণাঙ্ক control__register_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ধ্রুৱ ৱর্ণ *topic, শূন্য *userdata);
পূর্ণাঙ্ক control__unregister_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ধ্রুৱ ৱর্ণ *topic);
শূন্য control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier);


/* ============================================================
 * Logging functions
 * ============================================================ */
পূর্ণাঙ্ক log__init(সংরচনা mosquitto__config *config);
পূর্ণাঙ্ক log__close(সংরচনা mosquitto__config *config);
শূন্য log__internal(ধ্রুৱ ৱর্ণ *fmt, ...) __attribute__((format(printf, 1, 2)));

/* ============================================================
 * Bridge functions
 * ============================================================ */
#ifdef WITH_BRIDGE
শূন্য bridge__start_all(শূন্য);
শূন্য bridge__reload(শূন্য);
শূন্য bridge__db_cleanup(শূন্য);
শূন্য bridge__cleanup(সংরচনা mosquitto *context);
পূর্ণাঙ্ক bridge__connect(সংরচনা mosquitto *context);
#যদি defined(__GLIBC__) && defined(WITH_ADNS)
পূর্ণাঙ্ক bridge__connect_step3(সংরচনা mosquitto *context);
#endif
পূর্ণাঙ্ক bridge__on_connect(সংরচনা mosquitto *context);
শূন্য bridge_check(শূন্য);
পূর্ণাঙ্ক bridge__register_local_connections(শূন্য);
পূর্ণাঙ্ক bridge__add_topic(সংরচনা mosquitto__bridge *bridge, ধ্রুৱ ৱর্ণ *topic, গণনা mosquitto__bridge_direction direction, uint8_t qos, ধ্রুৱ ৱর্ণ *local_prefix, ধ্রুৱ ৱর্ণ *remote_prefix);
শূন্য bridge__cleanup_topics(সংরচনা mosquitto__bridge *bridge);
পূর্ণাঙ্ক bridge__remap_topic_in(সংরচনা mosquitto *context, ৱর্ণ **topic);
#endif

/* ============================================================
 * IO multiplex related functions
 * ============================================================ */
পূর্ণাঙ্ক mux__init(শূন্য);
পূর্ণাঙ্ক mux__add_listeners(সংরচনা mosquitto__listener_sock *listensock, পূর্ণাঙ্ক listensock_গুনতি);
পূর্ণাঙ্ক mux__delete_listeners(সংরচনা mosquitto__listener_sock *listensock, পূর্ণাঙ্ক listensock_গুনতি);
পূর্ণাঙ্ক mux__loop_prepare(শূন্য);
পূর্ণাঙ্ক mux__new(সংরচনা mosquitto *context);
পূর্ণাঙ্ক mux__add_out(সংরচনা mosquitto *context);
পূর্ণাঙ্ক mux__remove_out(সংরচনা mosquitto *context);
পূর্ণাঙ্ক mux__delete(সংরচনা mosquitto *context);
পূর্ণাঙ্ক mux__wait(শূন্য);
পূর্ণাঙ্ক mux__handle(সংরচনা mosquitto__listener_sock *listensock, পূর্ণাঙ্ক listensock_গুনতি);
পূর্ণাঙ্ক mux__cleanup(শূন্য);

/* ============================================================
 * Listener related functions
 * ============================================================ */
বাহ্য সংরচনা mosquitto__listener_sock *g_listensock;
বাহ্য পূর্ণাঙ্ক g_listensock_গুনতি;

শূন্য listener__set_defaults(সংরচনা mosquitto__listener *listener);
শূন্য listeners__reload_all_certificates(শূন্য);
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
শূন্য listeners__add_websockets(সংরচনা lws_context *ws_context, mosq_sock_t fd);
#endif
পূর্ণাঙ্ক listeners__start(শূন্য);
শূন্য listeners__stop(শূন্য);

/* ============================================================
 * Plugin related functions
 * ============================================================ */
পূর্ণাঙ্ক plugin__load_v5(mosquitto_plugin_id_t *plugin, শূন্য *lib);
পূর্ণাঙ্ক plugin__load_v4(mosquitto_plugin_id_t *plugin, শূন্য *lib);
পূর্ণাঙ্ক plugin__load_v3(mosquitto_plugin_id_t *plugin, শূন্য *lib);
পূর্ণাঙ্ক plugin__load_v2(mosquitto_plugin_id_t *plugin, শূন্য *lib);
পূর্ণাঙ্ক acl__pre_check(mosquitto_plugin_id_t *plugin, সংরচনা mosquitto *context, পূর্ণাঙ্ক access);

শূন্য LIB_ERROR(শূন্য);
শূন্য plugin__handle_connect(সংরচনা mosquitto *context);
শূন্য plugin__handle_disconnect(সংরচনা mosquitto *context, পূর্ণাঙ্ক reason);
শূন্য plugin__handle_client_offline(সংরচনা mosquitto *context, পূর্ণাঙ্ক reason);
পূর্ণাঙ্ক plugin__handle_message_in(সংরচনা mosquitto *context, সংরচনা mosquitto_base_বাৰ্তা_2 *base_বাৰ্তা);
পূর্ণাঙ্ক plugin__handle_message_out(সংরচনা mosquitto *context, সংরচনা mosquitto_base_বাৰ্তা_2 *base_বাৰ্তা);
পূর্ণাঙ্ক plugin__handle_subscribe(সংরচনা mosquitto *context, সংরচনা mosquitto_subscription *sub);
পূর্ণাঙ্ক plugin__handle_unsubscribe(সংরচনা mosquitto *context, সংরচনা mosquitto_subscription *sub);
পূর্ণাঙ্ক plugin__handle_reload(শূন্য);
শূন্য plugin__handle_tick(শূন্য);
পূর্ণাঙ্ক plugin__callback_unregister_all(mosquitto_plugin_id_t *identifier);
শূন্য plugin_persist__handle_restore(শূন্য);
শূন্য plugin_persist__handle_client_add(সংরচনা mosquitto *context);
শূন্য plugin_persist__handle_client_delete(সংরচনা mosquitto *context);
শূন্য plugin_persist__handle_client_update(সংরচনা mosquitto *context);
শূন্য plugin_persist__handle_subscription_add(সংরচনা mosquitto *context, ধ্রুৱ সংরচনা mosquitto_subscription *sub);
শূন্য plugin_persist__handle_subscription_delete(সংরচনা mosquitto *context, ৱর্ণ *sub);
শূন্য plugin_persist_handle_client_বাৰ্তা_add(সংরচনা mosquitto *context, ধ্রুৱ সংরচনা mosquitto_client_বাৰ্তা *cmsg);
শূন্য plugin_persist_handle_client_বাৰ্তা_delete(সংরচনা mosquitto *context, ধ্রুৱ সংরচনা mosquitto_client_বাৰ্তা *cmsg);
শূন্য plugin_persist_handle_client_বাৰ্তা_update(সংরচনা mosquitto *context, ধ্রুৱ সংরচনা mosquitto_client_বাৰ্তা *cmsg);
শূন্য plugin_persist_handle_base_বাৰ্তা_add(সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা);
শূন্য plugin_persist_handle_base_বাৰ্তা_delete(সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা);
শূন্য plugin_persist_handle_retain_বাৰ্তা_set(সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা);
শূন্য plugin_persist_handle_retain_বাৰ্তা_delete(সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা);
শূন্য plugin_persist__handle_will_add(সংরচনা mosquitto *context);
শূন্য plugin_persist__handle_will_delete(সংরচনা mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
পূর্ণাঙ্ক keepalive__init(শূন্য);
শূন্য keepalive__cleanup(শূন্য);
পূর্ণাঙ্ক keepalive__add(সংরচনা mosquitto *context);
শূন্য keepalive__check(শূন্য);
পূর্ণাঙ্ক keepalive__remove(সংরচনা mosquitto *context);
পূর্ণাঙ্ক keepalive__update(সংরচনা mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
পূর্ণাঙ্ক property__process_connect(সংরচনা mosquitto *context, mosquitto_property **props);
পূর্ণাঙ্ক property__process_will(সংরচনা mosquitto *context, সংরচনা mosquitto_message_all *বাৰ্তা, mosquitto_property **props);
পূর্ণাঙ্ক property__process_publish(সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা, mosquitto_property **props, পূর্ণাঙ্ক *topic_alias, uint32_t *message_expiry_interval, bool is_bridge);
পূর্ণাঙ্ক property__process_disconnect(সংরচনা mosquitto *context, mosquitto_property **props);

/* ============================================================
 * Retain tree related functions
 * ============================================================ */
পূর্ণাঙ্ক retain__init(শূন্য);
শূন্য retain__clean(সংরচনা mosquitto__retainhier **retainhier);
পূর্ণাঙ্ক retain__queue(সংরচনা mosquitto *context, ধ্রুৱ সংরচনা mosquitto_subscription *sub);
পূর্ণাঙ্ক retain__store(ধ্রুৱ ৱর্ণ *topic, সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা, ৱর্ণ **split_topics, bool persist);
শূন্য retain__expiry_check(শূন্য);
শূন্য retain__expire(সংরচনা mosquitto__retainhier **retainhier);

/* ============================================================
 * Security related functions
 * ============================================================ */
পূর্ণাঙ্ক acl__find_acls(সংরচনা mosquitto *context);
পূর্ণাঙ্ক plugin__load_all(শূন্য);
পূর্ণাঙ্ক plugin__unload_all(শূন্য);
পূর্ণাঙ্ক config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, সংরচনা mosquitto__security_options *security_options);

পূর্ণাঙ্ক mosquitto_security_init(bool reload);
পূর্ণাঙ্ক mosquitto_security_cleanup(bool reload);
পূর্ণাঙ্ক mosquitto_acl_check(সংরচনা mosquitto *context, ধ্রুৱ ৱর্ণ *topic, uint32_t payloadlen, শূন্য *payload, uint8_t qos, bool retain, mosquitto_property *properties, পূর্ণাঙ্ক access);
পূর্ণাঙ্ক mosquitto_basic_auth(সংরচনা mosquitto *context);
পূর্ণাঙ্ক mosquitto_psk_চাবি_get(সংরচনা mosquitto *context, ধ্রুৱ ৱর্ণ *hint, ধ্রুৱ ৱর্ণ *identity, ৱর্ণ *চাবি, পূর্ণাঙ্ক max_চাবি_len);

পূর্ণাঙ্ক mosquitto_security_init_default(শূন্য);
পূর্ণাঙ্ক mosquitto_security_apply_default(শূন্য);
পূর্ণাঙ্ক mosquitto_security_cleanup_default(শূন্য);
পূর্ণাঙ্ক mosquitto_psk_চাবি_get_default(সংরচনা mosquitto *context, ধ্রুৱ ৱর্ণ *hint, ধ্রুৱ ৱর্ণ *identity, ৱর্ণ *চাবি, পূর্ণাঙ্ক max_চাবি_len);
পূর্ণাঙ্ক broker_acl_file__init(শূন্য);
শূন্য broker_acl_file__cleanup(শূন্য);
পূর্ণাঙ্ক broker_password_file__init(শূন্য);
শূন্য broker_password_file__cleanup(শূন্য);
পূর্ণাঙ্ক psk_file__init(শূন্য);
পূর্ণাঙ্ক psk_file__cleanup(শূন্য);

পূর্ণাঙ্ক mosquitto_security_auth_start(সংরচনা mosquitto *context, bool reauth, ধ্রুৱ শূন্য *data_in, uint16_t data_in_len, শূন্য **data_out, uint16_t *data_out_len);
পূর্ণাঙ্ক mosquitto_security_auth_continue(সংরচনা mosquitto *context, ধ্রুৱ শূন্য *data_in, uint16_t data_len, শূন্য **data_out, uint16_t *data_out_len);

শূন্য unpwd__free_item(সংরচনা mosquitto__unpwd **unpwd, সংরচনা mosquitto__unpwd *item);

/* ============================================================
 * Session expiry
 * ============================================================ */
পূর্ণাঙ্ক session_expiry__add(সংরচনা mosquitto *context);
পূর্ণাঙ্ক session_expiry__add_from_persistence(সংরচনা mosquitto *context, time_t expiry_time);
শূন্য session_expiry__add_on_shutdown(শূন্য);
শূন্য session_expiry__remove(সংরচনা mosquitto *context);
শূন্য session_expiry__remove_all(শূন্য);
শূন্য session_expiry__check(শূন্য);
শূন্য session_expiry__send_all(শূন্য);

/* ============================================================
 * Signals
 * ============================================================ */
শূন্য signal__setup(শূন্য);
পূর্ণাঙ্ক signal__flag_check(শূন্য);

/* ============================================================
 * Window service and signal related functions
 * ============================================================ */
#যদি defined(WIN32) || defined(__CYGWIN__)
শূন্য service_install(ৱর্ণ *name);
শূন্য service_uninstall(ৱর্ণ *name);
শূন্য service_run(ৱর্ণ *name);

#ifdef WIN32
DWORD WINAPI SigThreadProc(শূন্য *data);
#endif
#endif

/* ============================================================
 * Watchdog
 * ============================================================ */
শূন্য watchdog__init(শূন্য);
শূন্য watchdog__check(শূন্য);

/* ============================================================
 * Websockets related functions
 * ============================================================ */
#ifdef WITH_WEBSOCKETS
#  যদি WITH_WEBSOCKETS == WS_IS_LWS
শূন্য mosq_websockets_init(সংরচনা mosquitto__listener *listener, ধ্রুৱ সংরচনা mosquitto__config *conf);
#  endif
পূর্ণাঙ্ক http__context_init(সংরচনা mosquitto *context);
পূর্ণাঙ্ক http__context_cleanup(সংরচনা mosquitto *context);
পূর্ণাঙ্ক http__read(সংরচনা mosquitto *context);
#endif
শূন্য do_disconnect(সংরচনা mosquitto *context, পূর্ণাঙ্ক reason);

/* ============================================================
 * PROXY related functions
 * ============================================================ */
পূর্ণাঙ্ক proxy_v2__read(সংরচনা mosquitto *context);
পূর্ণাঙ্ক proxy_v1__read(সংরচনা mosquitto *context);

/* ============================================================
 * Will delay
 * ============================================================ */
পূর্ণাঙ্ক will_delay__add(সংরচনা mosquitto *context);
শূন্য will_delay__check(শূন্য);
শূন্য will_delay__send_all(শূন্য);
শূন্য will_delay__remove(সংরচনা mosquitto *mosq);

/* ============================================================
 * HTTP Info
 * ============================================================ */
পূর্ণাঙ্ক http_api__start_local(সংরচনা mosquitto__listener *listener);
পূর্ণাঙ্ক http_api__start(সংরচনা mosquitto__listener *listener);
শূন্য http_api__stop(সংরচনা mosquitto__listener *listener);


/* ============================================================
 * Other
 * ============================================================ */
#ifdef WITH_XTREPORT
শূন্য xtreport(শূন্য);
#endif

শূন্য broker_control__init(শূন্য);
শূন্য broker_control__cleanup(শূন্য);
শূন্য broker_control__reload(শূন্য);

#endif
