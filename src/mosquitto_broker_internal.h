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

#অন্তর্ভাব "config.h"
#অন্তর্ভাব <stdio.h>

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

#অন্তর্ভাব "mosquitto_internal.h"
#অন্তর্ভাব "mosquitto/broker.h"
#অন্তর্ভাব "mosquitto/broker_plugin.h"
#অন্তর্ভাব "mosquitto.h"
#অন্তর্ভাব "logging_mosq.h"
#অন্তর্ভাব "tls_mosq.h"
#অন্তর্ভাব "uthash.h"
#অন্তর্ভাব "acl_file.h"
#অন্তর্ভাব "password_file.h"

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

প্রকারলেখ uint64_t dbid_t;

প্রকারলেখ পূর্ণসংখ্যা (*FUNC_plugin_init_v5)(mosquitto_plugin_id_t *, শূন্য **, কাঠামো mosquitto_opt *, পূর্ণসংখ্যা);
প্রকারলেখ পূর্ণসংখ্যা (*FUNC_plugin_cleanup_v5)(শূন্য *, কাঠামো mosquitto_opt *, পূর্ণসংখ্যা);

প্রকারলেখ পূর্ণসংখ্যা (*FUNC_auth_plugin_init_v4)(শূন্য **, কাঠামো mosquitto_opt *, পূর্ণসংখ্যা);
প্রকারলেখ পূর্ণসংখ্যা (*FUNC_auth_plugin_cleanup_v4)(শূন্য *, কাঠামো mosquitto_opt *, পূর্ণসংখ্যা);
প্রকারলেখ পূর্ণসংখ্যা (*FUNC_auth_plugin_security_init_v4)(শূন্য *, কাঠামো mosquitto_opt *, পূর্ণসংখ্যা, bool);
প্রকারলেখ পূর্ণসংখ্যা (*FUNC_auth_plugin_security_cleanup_v4)(শূন্য *, কাঠামো mosquitto_opt *, পূর্ণসংখ্যা, bool);
প্রকারলেখ পূর্ণসংখ্যা (*FUNC_auth_plugin_acl_check_v4)(শূন্য *, পূর্ণসংখ্যা, কাঠামো mosquitto *, কাঠামো mosquitto_acl_বার্তা *);
প্রকারলেখ পূর্ণসংখ্যা (*FUNC_auth_plugin_unpwd_check_v4)(শূন্য *, কাঠামো mosquitto *, ধ্রুবক অক্ষর *, ধ্রুবক অক্ষর *);
প্রকারলেখ পূর্ণসংখ্যা (*FUNC_auth_plugin_psk_চাবি_get_v4)(শূন্য *, কাঠামো mosquitto *, ধ্রুবক অক্ষর *, ধ্রুবক অক্ষর *, অক্ষর *, পূর্ণসংখ্যা);
প্রকারলেখ পূর্ণসংখ্যা (*FUNC_auth_plugin_auth_start_v4)(শূন্য *, কাঠামো mosquitto *, ধ্রুবক অক্ষর *, bool, ধ্রুবক শূন্য *, uint16_t, শূন্য **, uint16_t *);
প্রকারলেখ পূর্ণসংখ্যা (*FUNC_auth_plugin_auth_continue_v4)(শূন্য *, কাঠামো mosquitto *, ধ্রুবক অক্ষর *, ধ্রুবক শূন্য *, uint16_t, শূন্য **, uint16_t *);

প্রকারলেখ পূর্ণসংখ্যা (*FUNC_auth_plugin_init_v3)(শূন্য **, কাঠামো mosquitto_opt *, পূর্ণসংখ্যা);
প্রকারলেখ পূর্ণসংখ্যা (*FUNC_auth_plugin_cleanup_v3)(শূন্য *, কাঠামো mosquitto_opt *, পূর্ণসংখ্যা);
প্রকারলেখ পূর্ণসংখ্যা (*FUNC_auth_plugin_security_init_v3)(শূন্য *, কাঠামো mosquitto_opt *, পূর্ণসংখ্যা, bool);
প্রকারলেখ পূর্ণসংখ্যা (*FUNC_auth_plugin_security_cleanup_v3)(শূন্য *, কাঠামো mosquitto_opt *, পূর্ণসংখ্যা, bool);
প্রকারলেখ পূর্ণসংখ্যা (*FUNC_auth_plugin_acl_check_v3)(শূন্য *, পূর্ণসংখ্যা, ধ্রুবক কাঠামো mosquitto *, কাঠামো mosquitto_acl_বার্তা *);
প্রকারলেখ পূর্ণসংখ্যা (*FUNC_auth_plugin_unpwd_check_v3)(শূন্য *, ধ্রুবক কাঠামো mosquitto *, ধ্রুবক অক্ষর *, ধ্রুবক অক্ষর *);
প্রকারলেখ পূর্ণসংখ্যা (*FUNC_auth_plugin_psk_চাবি_get_v3)(শূন্য *, ধ্রুবক কাঠামো mosquitto *, ধ্রুবক অক্ষর *, ধ্রুবক অক্ষর *, অক্ষর *, পূর্ণসংখ্যা);

প্রকারলেখ পূর্ণসংখ্যা (*FUNC_auth_plugin_init_v2)(শূন্য **, কাঠামো mosquitto_auth_opt *, পূর্ণসংখ্যা);
প্রকারলেখ পূর্ণসংখ্যা (*FUNC_auth_plugin_cleanup_v2)(শূন্য *, কাঠামো mosquitto_auth_opt *, পূর্ণসংখ্যা);
প্রকারলেখ পূর্ণসংখ্যা (*FUNC_auth_plugin_security_init_v2)(শূন্য *, কাঠামো mosquitto_auth_opt *, পূর্ণসংখ্যা, bool);
প্রকারলেখ পূর্ণসংখ্যা (*FUNC_auth_plugin_security_cleanup_v2)(শূন্য *, কাঠামো mosquitto_auth_opt *, পূর্ণসংখ্যা, bool);
প্রকারলেখ পূর্ণসংখ্যা (*FUNC_auth_plugin_acl_check_v2)(শূন্য *, ধ্রুবক অক্ষর *, ধ্রুবক অক্ষর *, ধ্রুবক অক্ষর *, পূর্ণসংখ্যা);
প্রকারলেখ পূর্ণসংখ্যা (*FUNC_auth_plugin_unpwd_check_v2)(শূন্য *, ধ্রুবক অক্ষর *, ধ্রুবক অক্ষর *);
প্রকারলেখ পূর্ণসংখ্যা (*FUNC_auth_plugin_psk_চাবি_get_v2)(শূন্য *, ধ্রুবক অক্ষর *, ধ্রুবক অক্ষর *, অক্ষর *, পূর্ণসংখ্যা);


গণনা mosquitto_বার্তা_origin {
	mosq_mo_client = 0,
	mosq_mo_broker = 1,
};

কাঠামো mosquitto__plugin_lib {
	শূন্য *lib;
	শূন্য *user_তথ্য;
	পূর্ণসংখ্যা (*plugin_version)(শূন্য);
	কাঠামো mosquitto_plugin_id_t *identifier;

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
	পূর্ণসংখ্যা version;
};

কাঠামো mosquitto__plugin_config {
	অক্ষর *path;
	অক্ষর *name;
	কাঠামো mosquitto_opt *options;
	কাঠামো mosquitto__security_options **security_options;
	পূর্ণসংখ্যা option_গুনতি;
	পূর্ণসংখ্যা security_option_গুনতি;
	bool deny_special_chars;
};

কাঠামো mosquitto__callback {
	UT_hash_handle hh; /* For callbacks that register for e.g. a specific topic */
	কাঠামো mosquitto__callback *পরবর্তী, *পূর্ব; /* For typical callbacks */
	MOSQ_FUNC_generic_callback cb;
	শূন্য *userdata;
	সংঘ {
		অক্ষর *topic;
		কাঠামো timespec পরবর্তী_tick;
	} তথ্য;
	mosquitto_plugin_id_t *identifier;
};

কাঠামো plugin__callbacks {
	কাঠামো mosquitto__callback *tick;
	কাঠামো mosquitto__callback *acl_check;
	কাঠামো mosquitto__callback *basic_auth;
	কাঠামো mosquitto__callback *connect;
	কাঠামো mosquitto__callback *control;
	কাঠামো mosquitto__callback *disconnect;
	কাঠামো mosquitto__callback *client_offline;
	কাঠামো mosquitto__callback *ext_auth_continue;
	কাঠামো mosquitto__callback *ext_auth_start;
	কাঠামো mosquitto__callback *message_in;
	কাঠামো mosquitto__callback *message_out;
	কাঠামো mosquitto__callback *psk_চাবি;
	কাঠামো mosquitto__callback *reload;
	কাঠামো mosquitto__callback *subscribe;
	কাঠামো mosquitto__callback *unsubscribe;
	কাঠামো mosquitto__callback *persist_restore;
	কাঠামো mosquitto__callback *persist_client_add;
	কাঠামো mosquitto__callback *persist_client_delete;
	কাঠামো mosquitto__callback *persist_client_update;
	কাঠামো mosquitto__callback *persist_subscription_add;
	কাঠামো mosquitto__callback *persist_subscription_delete;
	কাঠামো mosquitto__callback *persist_client_বার্তা_add;
	কাঠামো mosquitto__callback *persist_client_বার্তা_delete;
	কাঠামো mosquitto__callback *persist_client_বার্তা_update;
	কাঠামো mosquitto__callback *persist_base_বার্তা_add;
	কাঠামো mosquitto__callback *persist_base_বার্তা_delete;
	কাঠামো mosquitto__callback *persist_retain_বার্তা_set;
	কাঠামো mosquitto__callback *persist_retain_বার্তা_delete;
	কাঠামো mosquitto__callback *persist_will_add;
	কাঠামো mosquitto__callback *persist_will_delete;
};

/* This is owned by mosquitto__config or mosquitto__listener, and only referred
 * to by other structs */
কাঠামো mosquitto__security_options {
	/* Any options that get added here also need considering
	 * in config__read() with regards whether allow_anonymous
	 * should be disabled when these options are set.
	 */
	কাঠামো mosquitto__unpwd *unpwd;
	কাঠামো mosquitto__psk *psk_id;
	কাঠামো acl_file_তথ্য acl_তথ্য;
	কাঠামো password_file_তথ্য password_তথ্য;
	অক্ষর *psk_file;
	mosquitto_plugin_id_t **plugins;
	পূর্ণসংখ্যা plugin_গুনতি;
	int8_t allow_anonymous;
	bool allow_zero_দৈর্ঘ্য_clientid;
	অক্ষর *auto_id_prefix;
	uint16_t auto_id_prefix_len;
	কাঠামো plugin__callbacks plugin_callbacks;
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

কাঠামো mosquitto__listener {
	uint16_t port;
	অক্ষর *host;
	অক্ষর *bind_interface;
	পূর্ণসংখ্যা max_connections;
	অক্ষর *mount_point;
	mosq_sock_t *socks;
	পূর্ণসংখ্যা sock_গুনতি;
	পূর্ণসংখ্যা client_গুনতি;
	গণনা mosquitto_protocol protocol;
	পূর্ণসংখ্যা socket_domain;
	bool use_username_as_clientid;
	uint8_t max_qos;
	uint16_t max_topic_alias;
	uint16_t max_topic_alias_broker;
#ifdef WITH_TLS
	অক্ষর *cafile;
	অক্ষর *capath;
	অক্ষর *certfile;
	অক্ষর *keyfile;
	অক্ষর *tls_engine;
	অক্ষর *tls_engine_kpass_sha1;
	অক্ষর *ciphers;
	অক্ষর *ciphers_tls13;
	অক্ষর *psk_hint;
	SSL_CTX *ssl_ctx;
	অক্ষর *crlfile;
	অক্ষর *tls_version;
	bool use_identity_as_username;
	bool use_subject_as_username;
	bool require_certificate;
	bool disable_client_cert_date_checks;
	গণনা mosquitto__keyform tls_keyform;
#endif
#ifdef WITH_WEBSOCKETS
#  যদি WITH_WEBSOCKETS == WS_IS_LWS
	কাঠামো lws_context *ws_context;
	bool ws_in_init;
	কাঠামো lws_protocols *ws_protocol;
#  endif
	অক্ষর **ws_origins;
	পূর্ণসংখ্যা ws_origin_গুনতি;
#endif
#যদি defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
	অক্ষর *http_dir;
#endif
	কাঠামো mosquitto__security_options *security_options;
#ifdef WITH_UNIX_SOCKETS
	অক্ষর *unix_socket_path;
#endif
	bool disable_protocol_v3;
	bool disable_protocol_v4;
	bool disable_protocol_v5;
	পূর্ণসংখ্যা enable_proxy_protocol;
	bool proxy_protocol_v2_require_tls;
#ifdef WITH_HTTP_API
	কাঠামো MHD_Daemon *mhd;
#endif
};


কাঠামো mosquitto__listener_sock {
#যদি defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	/* This *must* be the first element in the struct. */
	পূর্ণসংখ্যা ident;
#endif
	mosq_sock_t sock;
	কাঠামো mosquitto__listener *listener;
};


/* Callbacks belonging to a specific plugin
 * Doesn't include MOSQ_EVT_CONTROL events.
 */
কাঠামো plugin_own_callback {
	কাঠামো plugin_own_callback *পরবর্তী, *পূর্ব;
	MOSQ_FUNC_generic_callback cb_func;
	গণনা mosquitto_plugin_event event;
};

কাঠামো mosquitto_plugin_id_t {
	কাঠামো mosquitto__plugin_config config;
	কাঠামো mosquitto__plugin_lib lib;
	কাঠামো mosquitto__listener *listener;
	অক্ষর *plugin_name;
	অক্ষর *plugin_version;
	কাঠামো control_endpoint *control_endpoints;
	কাঠামো plugin_own_callback *own_callbacks;
	কাঠামো timespec পরবর্তী_tick;
};

কাঠামো mosquitto__config {
	bool allow_duplicate_messages;
	পূর্ণসংখ্যা autosave_interval;
	bool autosave_on_changes;
	bool check_retain_source;
	অক্ষর *clientid_prefixes;
	bool connection_messages;
	uint16_t cmd_port[CMD_PORT_LIMIT];
	পূর্ণসংখ্যা cmd_port_গুনতি;
	bool daemon;
	bool test_configuration;
	bool enable_control_api;
	পূর্ণসংখ্যা global_max_clients;
	পূর্ণসংখ্যা global_max_connections;
	কাঠামো mosquitto__listener *default_listener; /* Points to one of `listeners` */
	কাঠামো mosquitto__listener *listeners;
	পূর্ণসংখ্যা listener_গুনতি;
	bool local_only;
	unsigned পূর্ণসংখ্যা log_dest;
	পূর্ণসংখ্যা log_facility;
	unsigned পূর্ণসংখ্যা log_type;
	bool log_timestamp;
	অক্ষর *log_timestamp_format;
	অক্ষর *log_file;
	FILE *log_fptr;
	size_t max_inflight_bytes;
	size_t max_queued_bytes;
	পূর্ণসংখ্যা max_queued_messages;
	uint32_t max_packet_আকার;
	uint32_t message_আকার_limit;
	uint16_t max_inflight_messages;
	uint16_t max_keepalive;
	uint8_t max_qos;
	uint32_t packet_max_connect;
	uint32_t packet_max_simple;
	uint32_t packet_max_sub;
	uint32_t packet_max_auth;
	bool persistence;
	অক্ষর *persistence_location;
	অক্ষর *persistence_file;
	অক্ষর *persistence_filepath;
	time_t persistent_client_expiration;
	অক্ষর *pid_file;
	bool queue_qos0_messages;
	bool per_listener_settings;
	bool retain_available;
	পূর্ণসংখ্যা retain_expiry_interval;
	bool set_tcp_nodelay;
	পূর্ণসংখ্যা sys_interval;
	bool upgrade_outgoing_qos;
	অক্ষর *user;
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	পূর্ণসংখ্যা websockets_log_level;
#endif
	uint16_t packet_বাফার_আকার;
#ifdef WITH_BRIDGE
	কাঠামো mosquitto__bridge **bridges;
	পূর্ণসংখ্যা bridge_গুনতি;
#endif
	কাঠামো mosquitto__security_options security_options;
};


কাঠামো mosquitto__subshared {
	UT_hash_handle hh;
	কাঠামো mosquitto__subleaf *subs;
	অক্ষর name[];
};

কাঠামো mosquitto__subhier {
	UT_hash_handle hh;
	কাঠামো mosquitto__subhier *parent;
	কাঠামো mosquitto__subhier *children;
	কাঠামো mosquitto__subleaf *subs;
	কাঠামো mosquitto__subshared *shared;
	uint16_t topic_len;
	অক্ষর topic[];
};

কাঠামো mosquitto__subleaf {
	কাঠামো mosquitto__subleaf *পূর্ব;
	কাঠামো mosquitto__subleaf *পরবর্তী;
	কাঠামো mosquitto *context;
	কাঠামো mosquitto__subhier *hier;
	কাঠামো mosquitto__subshared *shared;
	uint32_t identifier;
	uint8_t subscription_options;
	অক্ষর topic_filter[];
};

কাঠামো sub__token {
	কাঠামো sub__token *পরবর্তী;
	uint16_t topic_len;
	অক্ষর topic[];
};

কাঠামো mosquitto__retainhier {
	UT_hash_handle hh;
	কাঠামো mosquitto__retainhier *parent;
	কাঠামো mosquitto__retainhier *children;
	কাঠামো mosquitto_base_বার্তা *retained;
	uint16_t topic_len;
	অক্ষর topic[];
};

কাঠামো mosquitto_base_বার্তা {
	UT_hash_handle hh;
	কাঠামো mosquitto_base_বার্তা_2 তথ্য;
	কাঠামো mosquitto__listener *source_listener;
	অক্ষর **dest_ids;
	পূর্ণসংখ্যা dest_id_গুনতি;
	পূর্ণসংখ্যা ref_গুনতি;
	গণনা mosquitto_বার্তা_origin origin;
	bool stored;
};

কাঠামো mosquitto_client_বার্তা {
	কাঠামো mosquitto_client_বার্তা_2 তথ্য;
	কাঠামো mosquitto_client_বার্তা *পূর্ব;
	কাঠামো mosquitto_client_বার্তা *পরবর্তী;
	কাঠামো mosquitto_base_বার্তা *base_বার্তা;
};


কাঠামো mosquitto__psk {
	UT_hash_handle hh;
	অক্ষর *username;
	অক্ষর *password;
};

কাঠামো mosquitto__message_v5 {
	কাঠামো mosquitto__message_v5 *পরবর্তী, *পূর্ব;
	অক্ষর *topic;
	শূন্য *payload;
	mosquitto_property *properties;
	/* clientid is used only by mosquitto_broker_publish*() to indicate
	   this message is for a specific client. */
	অক্ষর *clientid;
	পূর্ণসংখ্যা payloadlen;
	পূর্ণসংখ্যা qos;
	bool retain;
};

কাঠামো mosquitto_db {
	dbid_t last_db_id;
	uint64_t গ্রন্থি_id_shifted;
	কাঠামো mosquitto__subhier *normal_subs;
	কাঠামো mosquitto__subhier *shared_subs;
	কাঠামো mosquitto__retainhier *retains;
	কাঠামো mosquitto *contexts_by_id;
	কাঠামো mosquitto *contexts_by_sock;
	কাঠামো mosquitto *contexts_by_id_delayed_auth;
	কাঠামো mosquitto *contexts_for_free;
	mosquitto_plugin_id_t **plugins;
	পূর্ণসংখ্যা plugin_গুনতি;
#ifdef WITH_BRIDGE
	কাঠামো mosquitto **bridges;
	পূর্ণসংখ্যা bridge_গুনতি;
#endif
	কাঠামো clientid_সূচক_hash_2 *clientid_সূচক_hash;
	কাঠামো mosquitto_base_বার্তা *বার্তা_store;
	time_t now_s; /* Monotonic clock, where possible */
	time_t now_real_s; /* Read clock, for measuring session/message expiry */
	uint64_t গ্রন্থি_id; /* for unique db ids */
	time_t পরবর্তী_event_ms; /* for mux timeout */
	পূর্ণসংখ্যা বার্তা_store_গুনতি;
	unsigned long বার্তা_store_bytes;
	অক্ষর *config_file;
	কাঠামো mosquitto__config *config;
	bool quiet;
	bool verbose;
#ifdef WITH_SYS_বৃক্ষ
	পূর্ণসংখ্যা subscription_গুনতি;
	পূর্ণসংখ্যা shared_subscription_গুনতি;
	পূর্ণসংখ্যা retained_গুনতি;
#endif
	পূর্ণসংখ্যা persistence_changes;
	কাঠামো mosquitto *ll_for_free;
#ifdef WITH_EPOLL
	পূর্ণসংখ্যা epollfd;
#endif
#ifdef WITH_KQUEUE
	পূর্ণসংখ্যা kqueuefd;
#endif
	কাঠামো mosquitto__message_v5 *plugin_msgs;
#ifdef WITH_TLS
	/* tls_keylog can't be in the config struct because it is used
	   before the config is allocated. Config probably
	   shouldn't be separately allocated. */
	অক্ষর *tls_keylog;
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

কাঠামো mosquitto__bridge_topic {
	কাঠামো mosquitto__bridge_topic *পরবর্তী;
	অক্ষর *topic;
	অক্ষর *local_prefix;
	অক্ষর *remote_prefix;
	অক্ষর *local_topic; /* topic prefixed with local_prefix */
	অক্ষর *remote_topic; /* topic prefixed with remote_prefix */
	গণনা mosquitto__bridge_direction direction;
	uint8_t qos;
};

কাঠামো bridge_address {
	অক্ষর *address;
	uint16_t port;
};

কাঠামো mosquitto__bridge {
	অক্ষর *name;
	কাঠামো bridge_address *addresses;
	পূর্ণসংখ্যা cur_address;
	পূর্ণসংখ্যা address_গুনতি;
	time_t primary_retry;
	mosq_sock_t primary_retry_sock;
	bool round_robin;
	bool try_private;
	bool try_private_accepted;
	bool clean_start;
	int8_t clean_start_local;
	uint16_t keepalive;
	unsigned পূর্ণসংখ্যা tcp_keepalive_idle;
	unsigned পূর্ণসংখ্যা tcp_keepalive_interval;
	unsigned পূর্ণসংখ্যা tcp_keepalive_গণক;
#ifdef WITH_TCP_USER_TIMEOUT
	পূর্ণসংখ্যা tcp_user_timeout;
#endif
	কাঠামো mosquitto__bridge_topic *topics;
	পূর্ণসংখ্যা topic_গুনতি;
	bool topic_remapping;
	গণনা mosquitto__protocol protocol_version;
	time_t restart_t;
	time_t connected_at;
	অক্ষর *remote_clientid;
	অক্ষর *remote_username;
	অক্ষর *remote_password;
	অক্ষর *local_clientid;
	অক্ষর *local_username;
	অক্ষর *local_password;
	অক্ষর *notification_topic;
	অক্ষর *bind_address;
	bool notifications;
	bool notifications_local_only;
	গণনা mosquitto_bridge_start_type start_type;
	পূর্ণসংখ্যা idle_timeout;
	পূর্ণসংখ্যা restart_timeout;
	পূর্ণসংখ্যা backoff_base;
	পূর্ণসংখ্যা backoff_cap;
	পূর্ণসংখ্যা stable_connection_period;
	পূর্ণসংখ্যা threshold;
	uint32_t maximum_packet_আকার;
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
	অক্ষর *tls_cafile;
	অক্ষর *tls_capath;
	অক্ষর *tls_certfile;
	অক্ষর *tls_keyfile;
	অক্ষর *tls_version;
	অক্ষর *tls_alpn;
	অক্ষর *tls_ciphers;
	অক্ষর *tls_13_ciphers;
#  ifdef FINAL_WITH_TLS_PSK
	অক্ষর *tls_psk_identity;
	অক্ষর *tls_psk;
#  endif
#endif
};

#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
কাঠামো libws_mqtt_hack {
	অক্ষর *http_dir;
	কাঠামো mosquitto__listener *listener;
};

কাঠামো libws_mqtt_তথ্য {
	কাঠামো mosquitto *mosq;
};
#endif

#অন্তর্ভাব <net_mosq.h>


কাঠামো control_endpoint {
	কাঠামো control_endpoint *পরবর্তী, *পূর্ব;
	অক্ষর topic[];
};

বাহ্যিক কাঠামো mosquitto_db db;

/* ============================================================
 * Main functions
 * ============================================================ */
পূর্ণসংখ্যা mosquitto_main_loop(কাঠামো mosquitto__listener_sock *listensock, পূর্ণসংখ্যা listensock_গুনতি);
শূন্য loop_update_পরবর্তী_event(time_t new_ms);

/* ============================================================
 * Config functions
 * ============================================================ */
/* Initialise config struct to default values. */
শূন্য config__init(কাঠামো mosquitto__config *config);
#ifdef WITH_BRIDGE
শূন্য config__bridge_cleanup(কাঠামো mosquitto__bridge *bridge);
#endif
/* Parse command line options into config. */
পূর্ণসংখ্যা config__parse_args(কাঠামো mosquitto__config *config, পূর্ণসংখ্যা argc, অক্ষর *argv[]);
/* Read configuration data from config->config_file into config.
 * If reload is true, don't process config options that shouldn't be reloaded (listeners etc)
 * Returns 0 on success, 1 if there is a configuration error or if a file cannot be opened.
 */
পূর্ণসংখ্যা config__read(কাঠামো mosquitto__config *config, bool reload);
/* Free all config data. */
শূন্য config__cleanup(কাঠামো mosquitto__config *config);
পূর্ণসংখ্যা config__get_dir_files(ধ্রুবক অক্ষর *include_dir, অক্ষর ***files, পূর্ণসংখ্যা *file_গুনতি);

/* ============================================================
 * Server send functions
 * ============================================================ */
পূর্ণসংখ্যা send__connack(কাঠামো mosquitto *context, uint8_t ack, uint8_t reason_code, ধ্রুবক mosquitto_property *properties);
পূর্ণসংখ্যা send__suback(কাঠামো mosquitto *context, uint16_t mid, uint32_t payloadlen, ধ্রুবক শূন্য *payload);
পূর্ণসংখ্যা send__unsuback(কাঠামো mosquitto *context, uint16_t mid, পূর্ণসংখ্যা reason_code_গুনতি, uint8_t *reason_codes, ধ্রুবক mosquitto_property *properties);
পূর্ণসংখ্যা send__auth(কাঠামো mosquitto *context, uint8_t reason_code, ধ্রুবক শূন্য *auth_তথ্য, uint16_t auth_তথ্য_len);

/* ============================================================
 * Network functions
 * ============================================================ */
শূন্য net__broker_init(শূন্য);
শূন্য net__broker_cleanup(শূন্য);
কাঠামো mosquitto *net__socket_accept(কাঠামো mosquitto__listener_sock *listensock);
পূর্ণসংখ্যা net__socket_listen(কাঠামো mosquitto__listener *listener);
পূর্ণসংখ্যা net__socket_get_address(mosq_sock_t sock, অক্ষর *buf, size_t len, uint16_t *remote_address);
পূর্ণসংখ্যা net__tls_load_verify(কাঠামো mosquitto__listener *listener);
পূর্ণসংখ্যা net__tls_server_ctx(কাঠামো mosquitto__listener *listener);
পূর্ণসংখ্যা net__load_certificates(কাঠামো mosquitto__listener *listener);

/* ============================================================
 * Read handling functions
 * ============================================================ */
পূর্ণসংখ্যা handle__packet(কাঠামো mosquitto *context);
পূর্ণসংখ্যা handle__connack(কাঠামো mosquitto *context);
পূর্ণসংখ্যা handle__connect(কাঠামো mosquitto *context);
পূর্ণসংখ্যা handle__disconnect(কাঠামো mosquitto *context);
পূর্ণসংখ্যা handle__publish(কাঠামো mosquitto *context);
পূর্ণসংখ্যা handle__subscribe(কাঠামো mosquitto *context);
পূর্ণসংখ্যা handle__unsubscribe(কাঠামো mosquitto *context);
পূর্ণসংখ্যা handle__auth(কাঠামো mosquitto *context);

/* ============================================================
 * Database handling
 * ============================================================ */
পূর্ণসংখ্যা db__open(কাঠামো mosquitto__config *config);
পূর্ণসংখ্যা db__close(শূন্য);
#ifdef WITH_PERSISTENCE
পূর্ণসংখ্যা persist__backup(bool shutdown);
পূর্ণসংখ্যা persist__restore(শূন্য);
#endif
/* Return the number of in-flight messages in count. */
পূর্ণসংখ্যা db_message_গুনতি(পূর্ণসংখ্যা *গুনতি);
পূর্ণসংখ্যা db__message_delete_outgoing(কাঠামো mosquitto *context, uint16_t mid, গণনা mosquitto_বার্তা_state expect_state, পূর্ণসংখ্যা qos);
পূর্ণসংখ্যা db_message_ঢোকাও_outgoing(কাঠামো mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, কাঠামো mosquitto_base_বার্তা *base_বার্তা, uint32_t subscription_identifier, bool update, bool persist);
পূর্ণসংখ্যা db_message_ঢোকাও_incoming(কাঠামো mosquitto *context, uint64_t cmsg_id, কাঠামো mosquitto_base_বার্তা *base_বার্তা, bool persist);
পূর্ণসংখ্যা db__message_remove_incoming(কাঠামো mosquitto *context, uint16_t mid);
পূর্ণসংখ্যা db__message_release_incoming(কাঠামো mosquitto *context, uint16_t mid);
পূর্ণসংখ্যা db__message_update_outgoing(কাঠামো mosquitto *context, uint16_t mid, গণনা mosquitto_বার্তা_state state, পূর্ণসংখ্যা qos, bool persist);
শূন্য db__message_dequeue_first(কাঠামো mosquitto *context, কাঠামো mosquitto_বার্তা_তথ্য *বার্তা_তথ্য);
পূর্ণসংখ্যা db__messages_delete(কাঠামো mosquitto *context, bool force_free);
পূর্ণসংখ্যা db__messages_delete_incoming(কাঠামো mosquitto *context);
পূর্ণসংখ্যা db__messages_delete_outgoing(কাঠামো mosquitto *context);
পূর্ণসংখ্যা db__messages_easy_queue(কাঠামো mosquitto *context, ধ্রুবক অক্ষর *topic, uint8_t qos, uint32_t payloadlen, ধ্রুবক শূন্য *payload, পূর্ণসংখ্যা retain, uint32_t message_expiry_interval, mosquitto_property **properties);
পূর্ণসংখ্যা db__message_store(ধ্রুবক কাঠামো mosquitto *source, কাঠামো mosquitto_base_বার্তা *base_বার্তা, uint32_t *message_expiry_interval, গণনা mosquitto_বার্তা_origin origin);
পূর্ণসংখ্যা db__message_store_find(কাঠামো mosquitto *context, uint16_t mid, কাঠামো mosquitto_client_বার্তা **client_বার্তা);
পূর্ণসংখ্যা db_বার্তা_store_add(কাঠামো mosquitto_base_বার্তা *base_বার্তা);
শূন্য db_বার্তা_store_remove(কাঠামো mosquitto_base_বার্তা *base_বার্তা, bool notify);
শূন্য db_বার্তা_store_ref_inc(কাঠামো mosquitto_base_বার্তা *base_বার্তা);
শূন্য db_বার্তা_store_ref_dec(কাঠামো mosquitto_base_বার্তা **base_বার্তা);
শূন্য db_বার্তা_store_clean(শূন্য);
শূন্য db_বার্তা_store_compact(শূন্য);
শূন্য db_বার্তা_store_free(কাঠামো mosquitto_base_বার্তা *base_বার্তা);
পূর্ণসংখ্যা db__message_reconnect_reset(কাঠামো mosquitto *context);
bool db__ready_for_flight(কাঠামো mosquitto *context, গণনা mosquitto_বার্তা_direction dir, পূর্ণসংখ্যা qos);
bool db__ready_for_queue(কাঠামো mosquitto *context, পূর্ণসংখ্যা qos, কাঠামো mosquitto_বার্তা_তথ্য *বার্তা_তথ্য);
শূন্য sys_বৃক্ষ_init(শূন্য);
শূন্য sys_বৃক্ষ_update(bool force);
পূর্ণসংখ্যা db__message_write_inflight_out_all(কাঠামো mosquitto *context);
পূর্ণসংখ্যা db__message_write_inflight_out_latest(কাঠামো mosquitto *context);
পূর্ণসংখ্যা db__message_write_queued_out(কাঠামো mosquitto *context);
পূর্ণসংখ্যা db__message_write_queued_in(কাঠামো mosquitto *context);
শূন্য db_বার্তা_add_to_inflight_stats(কাঠামো mosquitto_বার্তা_তথ্য *বার্তা_তথ্য, কাঠামো mosquitto_client_বার্তা *বার্তা);
শূন্য db_বার্তা_add_to_queued_stats(কাঠামো mosquitto_বার্তা_তথ্য *বার্তা_তথ্য, কাঠামো mosquitto_client_বার্তা *বার্তা);
uint64_t db_new_বার্তা_id(শূন্য);
শূন্য db__expire_all_messages(কাঠামো mosquitto *context);
শূন্য db__check_acl_of_all_messages(কাঠামো mosquitto *context);

/* ============================================================
 * Subscription functions
 * ============================================================ */
পূর্ণসংখ্যা sub__init(শূন্য);
পূর্ণসংখ্যা sub__add(কাঠামো mosquitto *context, ধ্রুবক কাঠামো mosquitto_subscription *sub);
পূর্ণসংখ্যা sub__remove(কাঠামো mosquitto *context, ধ্রুবক অক্ষর *sub, uint8_t *reason);
শূন্য sub_বৃক্ষ_print(কাঠামো mosquitto__subhier *মূল, পূর্ণসংখ্যা level);
পূর্ণসংখ্যা sub__clean_session(কাঠামো mosquitto *context);
পূর্ণসংখ্যা sub__messages_queue(ধ্রুবক অক্ষর *source_id, uint8_t qos, কাঠামো mosquitto_base_বার্তা **base_বার্তা);
পূর্ণসংখ্যা sub__topic_tokenise(ধ্রুবক অক্ষর *subtopic, অক্ষর **local_sub, অক্ষর ***topics, ধ্রুবক অক্ষর **sharename);
শূন্য sub__topic_tokens_free(কাঠামো sub__token *tokens);

/* ============================================================
 * Context functions
 * ============================================================ */
কাঠামো mosquitto *context__init(শূন্য);
পূর্ণসংখ্যা context__init_sock(কাঠামো mosquitto *context, mosq_sock_t sock, bool get_address);
শূন্য context__cleanup(কাঠামো mosquitto *context, bool force_free);
শূন্য context__disconnect(কাঠামো mosquitto *context, পূর্ণসংখ্যা reason);
শূন্য context__add_to_disused(কাঠামো mosquitto *context);
শূন্য context__free_disused(শূন্য);
শূন্য context__send_will(কাঠামো mosquitto *context);
শূন্য context__add_to_by_id(কাঠামো mosquitto *context);
শূন্য context__remove_from_by_id(কাঠামো mosquitto *context);

পূর্ণসংখ্যা connect__on_authorised(কাঠামো mosquitto *context, শূন্য *auth_তথ্য_out, uint16_t auth_তথ্য_out_len);


/* ============================================================
 * Control functions
 * ============================================================ */
#ifdef WITH_CONTROL
পূর্ণসংখ্যা control__process(কাঠামো mosquitto *context, কাঠামো mosquitto_base_বার্তা *base_বার্তা);
শূন্য control__cleanup(শূন্য);
#endif
পূর্ণসংখ্যা control__register_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ধ্রুবক অক্ষর *topic, শূন্য *userdata);
পূর্ণসংখ্যা control__unregister_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ধ্রুবক অক্ষর *topic);
শূন্য control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier);


/* ============================================================
 * Logging functions
 * ============================================================ */
পূর্ণসংখ্যা log__init(কাঠামো mosquitto__config *config);
পূর্ণসংখ্যা log__close(কাঠামো mosquitto__config *config);
শূন্য log__internal(ধ্রুবক অক্ষর *fmt, ...) __attribute__((format(printf, 1, 2)));

/* ============================================================
 * Bridge functions
 * ============================================================ */
#ifdef WITH_BRIDGE
শূন্য bridge__start_all(শূন্য);
শূন্য bridge__reload(শূন্য);
শূন্য bridge__db_cleanup(শূন্য);
শূন্য bridge__cleanup(কাঠামো mosquitto *context);
পূর্ণসংখ্যা bridge__connect(কাঠামো mosquitto *context);
#যদি defined(__GLIBC__) && defined(WITH_ADNS)
পূর্ণসংখ্যা bridge__connect_step3(কাঠামো mosquitto *context);
#endif
পূর্ণসংখ্যা bridge__on_connect(কাঠামো mosquitto *context);
শূন্য bridge_check(শূন্য);
পূর্ণসংখ্যা bridge__register_local_connections(শূন্য);
পূর্ণসংখ্যা bridge__add_topic(কাঠামো mosquitto__bridge *bridge, ধ্রুবক অক্ষর *topic, গণনা mosquitto__bridge_direction direction, uint8_t qos, ধ্রুবক অক্ষর *local_prefix, ধ্রুবক অক্ষর *remote_prefix);
শূন্য bridge__cleanup_topics(কাঠামো mosquitto__bridge *bridge);
পূর্ণসংখ্যা bridge__remap_topic_in(কাঠামো mosquitto *context, অক্ষর **topic);
#endif

/* ============================================================
 * IO multiplex related functions
 * ============================================================ */
পূর্ণসংখ্যা mux__init(শূন্য);
পূর্ণসংখ্যা mux__add_listeners(কাঠামো mosquitto__listener_sock *listensock, পূর্ণসংখ্যা listensock_গুনতি);
পূর্ণসংখ্যা mux__delete_listeners(কাঠামো mosquitto__listener_sock *listensock, পূর্ণসংখ্যা listensock_গুনতি);
পূর্ণসংখ্যা mux__loop_prepare(শূন্য);
পূর্ণসংখ্যা mux__new(কাঠামো mosquitto *context);
পূর্ণসংখ্যা mux__add_out(কাঠামো mosquitto *context);
পূর্ণসংখ্যা mux__remove_out(কাঠামো mosquitto *context);
পূর্ণসংখ্যা mux__delete(কাঠামো mosquitto *context);
পূর্ণসংখ্যা mux__wait(শূন্য);
পূর্ণসংখ্যা mux__handle(কাঠামো mosquitto__listener_sock *listensock, পূর্ণসংখ্যা listensock_গুনতি);
পূর্ণসংখ্যা mux__cleanup(শূন্য);

/* ============================================================
 * Listener related functions
 * ============================================================ */
বাহ্যিক কাঠামো mosquitto__listener_sock *g_listensock;
বাহ্যিক পূর্ণসংখ্যা g_listensock_গুনতি;

শূন্য listener__set_defaults(কাঠামো mosquitto__listener *listener);
শূন্য listeners__reload_all_certificates(শূন্য);
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
শূন্য listeners__add_websockets(কাঠামো lws_context *ws_context, mosq_sock_t fd);
#endif
পূর্ণসংখ্যা listeners__start(শূন্য);
শূন্য listeners__stop(শূন্য);

/* ============================================================
 * Plugin related functions
 * ============================================================ */
পূর্ণসংখ্যা plugin__load_v5(mosquitto_plugin_id_t *plugin, শূন্য *lib);
পূর্ণসংখ্যা plugin__load_v4(mosquitto_plugin_id_t *plugin, শূন্য *lib);
পূর্ণসংখ্যা plugin__load_v3(mosquitto_plugin_id_t *plugin, শূন্য *lib);
পূর্ণসংখ্যা plugin__load_v2(mosquitto_plugin_id_t *plugin, শূন্য *lib);
পূর্ণসংখ্যা acl__pre_check(mosquitto_plugin_id_t *plugin, কাঠামো mosquitto *context, পূর্ণসংখ্যা access);

শূন্য LIB_ERROR(শূন্য);
শূন্য plugin__handle_connect(কাঠামো mosquitto *context);
শূন্য plugin__handle_disconnect(কাঠামো mosquitto *context, পূর্ণসংখ্যা reason);
শূন্য plugin__handle_client_offline(কাঠামো mosquitto *context, পূর্ণসংখ্যা reason);
পূর্ণসংখ্যা plugin__handle_message_in(কাঠামো mosquitto *context, কাঠামো mosquitto_base_বার্তা_2 *base_বার্তা);
পূর্ণসংখ্যা plugin__handle_message_out(কাঠামো mosquitto *context, কাঠামো mosquitto_base_বার্তা_2 *base_বার্তা);
পূর্ণসংখ্যা plugin__handle_subscribe(কাঠামো mosquitto *context, কাঠামো mosquitto_subscription *sub);
পূর্ণসংখ্যা plugin__handle_unsubscribe(কাঠামো mosquitto *context, কাঠামো mosquitto_subscription *sub);
পূর্ণসংখ্যা plugin__handle_reload(শূন্য);
শূন্য plugin__handle_tick(শূন্য);
পূর্ণসংখ্যা plugin__callback_unregister_all(mosquitto_plugin_id_t *identifier);
শূন্য plugin_persist__handle_restore(শূন্য);
শূন্য plugin_persist__handle_client_add(কাঠামো mosquitto *context);
শূন্য plugin_persist__handle_client_delete(কাঠামো mosquitto *context);
শূন্য plugin_persist__handle_client_update(কাঠামো mosquitto *context);
শূন্য plugin_persist__handle_subscription_add(কাঠামো mosquitto *context, ধ্রুবক কাঠামো mosquitto_subscription *sub);
শূন্য plugin_persist__handle_subscription_delete(কাঠামো mosquitto *context, অক্ষর *sub);
শূন্য plugin_persist_handle_client_বার্তা_add(কাঠামো mosquitto *context, ধ্রুবক কাঠামো mosquitto_client_বার্তা *cmsg);
শূন্য plugin_persist_handle_client_বার্তা_delete(কাঠামো mosquitto *context, ধ্রুবক কাঠামো mosquitto_client_বার্তা *cmsg);
শূন্য plugin_persist_handle_client_বার্তা_update(কাঠামো mosquitto *context, ধ্রুবক কাঠামো mosquitto_client_বার্তা *cmsg);
শূন্য plugin_persist_handle_base_বার্তা_add(কাঠামো mosquitto_base_বার্তা *base_বার্তা);
শূন্য plugin_persist_handle_base_বার্তা_delete(কাঠামো mosquitto_base_বার্তা *base_বার্তা);
শূন্য plugin_persist_handle_retain_বার্তা_set(কাঠামো mosquitto_base_বার্তা *base_বার্তা);
শূন্য plugin_persist_handle_retain_বার্তা_delete(কাঠামো mosquitto_base_বার্তা *base_বার্তা);
শূন্য plugin_persist__handle_will_add(কাঠামো mosquitto *context);
শূন্য plugin_persist__handle_will_delete(কাঠামো mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
পূর্ণসংখ্যা keepalive__init(শূন্য);
শূন্য keepalive__cleanup(শূন্য);
পূর্ণসংখ্যা keepalive__add(কাঠামো mosquitto *context);
শূন্য keepalive__check(শূন্য);
পূর্ণসংখ্যা keepalive__remove(কাঠামো mosquitto *context);
পূর্ণসংখ্যা keepalive__update(কাঠামো mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
পূর্ণসংখ্যা property__process_connect(কাঠামো mosquitto *context, mosquitto_property **props);
পূর্ণসংখ্যা property__process_will(কাঠামো mosquitto *context, কাঠামো mosquitto_message_all *বার্তা, mosquitto_property **props);
পূর্ণসংখ্যা property__process_publish(কাঠামো mosquitto_base_বার্তা *base_বার্তা, mosquitto_property **props, পূর্ণসংখ্যা *topic_alias, uint32_t *message_expiry_interval, bool is_bridge);
পূর্ণসংখ্যা property__process_disconnect(কাঠামো mosquitto *context, mosquitto_property **props);

/* ============================================================
 * Retain tree related functions
 * ============================================================ */
পূর্ণসংখ্যা retain__init(শূন্য);
শূন্য retain__clean(কাঠামো mosquitto__retainhier **retainhier);
পূর্ণসংখ্যা retain__queue(কাঠামো mosquitto *context, ধ্রুবক কাঠামো mosquitto_subscription *sub);
পূর্ণসংখ্যা retain__store(ধ্রুবক অক্ষর *topic, কাঠামো mosquitto_base_বার্তা *base_বার্তা, অক্ষর **split_topics, bool persist);
শূন্য retain__expiry_check(শূন্য);
শূন্য retain__expire(কাঠামো mosquitto__retainhier **retainhier);

/* ============================================================
 * Security related functions
 * ============================================================ */
পূর্ণসংখ্যা acl__find_acls(কাঠামো mosquitto *context);
পূর্ণসংখ্যা plugin__load_all(শূন্য);
পূর্ণসংখ্যা plugin__unload_all(শূন্য);
পূর্ণসংখ্যা config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, কাঠামো mosquitto__security_options *security_options);

পূর্ণসংখ্যা mosquitto_security_init(bool reload);
পূর্ণসংখ্যা mosquitto_security_cleanup(bool reload);
পূর্ণসংখ্যা mosquitto_acl_check(কাঠামো mosquitto *context, ধ্রুবক অক্ষর *topic, uint32_t payloadlen, শূন্য *payload, uint8_t qos, bool retain, mosquitto_property *properties, পূর্ণসংখ্যা access);
পূর্ণসংখ্যা mosquitto_basic_auth(কাঠামো mosquitto *context);
পূর্ণসংখ্যা mosquitto_psk_চাবি_get(কাঠামো mosquitto *context, ধ্রুবক অক্ষর *hint, ধ্রুবক অক্ষর *identity, অক্ষর *চাবি, পূর্ণসংখ্যা max_চাবি_len);

পূর্ণসংখ্যা mosquitto_security_init_default(শূন্য);
পূর্ণসংখ্যা mosquitto_security_apply_default(শূন্য);
পূর্ণসংখ্যা mosquitto_security_cleanup_default(শূন্য);
পূর্ণসংখ্যা mosquitto_psk_চাবি_get_default(কাঠামো mosquitto *context, ধ্রুবক অক্ষর *hint, ধ্রুবক অক্ষর *identity, অক্ষর *চাবি, পূর্ণসংখ্যা max_চাবি_len);
পূর্ণসংখ্যা broker_acl_file__init(শূন্য);
শূন্য broker_acl_file__cleanup(শূন্য);
পূর্ণসংখ্যা broker_password_file__init(শূন্য);
শূন্য broker_password_file__cleanup(শূন্য);
পূর্ণসংখ্যা psk_file__init(শূন্য);
পূর্ণসংখ্যা psk_file__cleanup(শূন্য);

পূর্ণসংখ্যা mosquitto_security_auth_start(কাঠামো mosquitto *context, bool reauth, ধ্রুবক শূন্য *তথ্য_in, uint16_t তথ্য_in_len, শূন্য **তথ্য_out, uint16_t *তথ্য_out_len);
পূর্ণসংখ্যা mosquitto_security_auth_continue(কাঠামো mosquitto *context, ধ্রুবক শূন্য *তথ্য_in, uint16_t তথ্য_len, শূন্য **তথ্য_out, uint16_t *তথ্য_out_len);

শূন্য unpwd__free_item(কাঠামো mosquitto__unpwd **unpwd, কাঠামো mosquitto__unpwd *item);

/* ============================================================
 * Session expiry
 * ============================================================ */
পূর্ণসংখ্যা session_expiry__add(কাঠামো mosquitto *context);
পূর্ণসংখ্যা session_expiry__add_from_persistence(কাঠামো mosquitto *context, time_t expiry_time);
শূন্য session_expiry__add_on_shutdown(শূন্য);
শূন্য session_expiry__remove(কাঠামো mosquitto *context);
শূন্য session_expiry__remove_all(শূন্য);
শূন্য session_expiry__check(শূন্য);
শূন্য session_expiry__send_all(শূন্য);

/* ============================================================
 * Signals
 * ============================================================ */
শূন্য signal__setup(শূন্য);
পূর্ণসংখ্যা signal__flag_check(শূন্য);

/* ============================================================
 * Window service and signal related functions
 * ============================================================ */
#যদি defined(WIN32) || defined(__CYGWIN__)
শূন্য service_install(অক্ষর *name);
শূন্য service_uninstall(অক্ষর *name);
শূন্য service_run(অক্ষর *name);

#ifdef WIN32
DWORD WINAPI SigThreadProc(শূন্য *তথ্য);
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
শূন্য mosq_websockets_init(কাঠামো mosquitto__listener *listener, ধ্রুবক কাঠামো mosquitto__config *conf);
#  endif
পূর্ণসংখ্যা http__context_init(কাঠামো mosquitto *context);
পূর্ণসংখ্যা http__context_cleanup(কাঠামো mosquitto *context);
পূর্ণসংখ্যা http__read(কাঠামো mosquitto *context);
#endif
শূন্য do_disconnect(কাঠামো mosquitto *context, পূর্ণসংখ্যা reason);

/* ============================================================
 * PROXY related functions
 * ============================================================ */
পূর্ণসংখ্যা proxy_v2__read(কাঠামো mosquitto *context);
পূর্ণসংখ্যা proxy_v1__read(কাঠামো mosquitto *context);

/* ============================================================
 * Will delay
 * ============================================================ */
পূর্ণসংখ্যা will_delay__add(কাঠামো mosquitto *context);
শূন্য will_delay__check(শূন্য);
শূন্য will_delay__send_all(শূন্য);
শূন্য will_delay__remove(কাঠামো mosquitto *mosq);

/* ============================================================
 * HTTP Info
 * ============================================================ */
পূর্ণসংখ্যা http_api__start_local(কাঠামো mosquitto__listener *listener);
পূর্ণসংখ্যা http_api__start(কাঠামো mosquitto__listener *listener);
শূন্য http_api__stop(কাঠামো mosquitto__listener *listener);


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
