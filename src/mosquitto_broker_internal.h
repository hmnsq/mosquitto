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

#ضمن "config.h"
#ضمن <stdio.h>

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#  إذا LWS_LIBRARY_VERSION_ژمار >= 3002000 && !defined(LWS_WITH_EXTERNAL_POLL)
#    warning "libwebsockets is not compiled with LWS_WITH_EXTERNAL_POLL support. Websocket performance will be unusable."
#  endif
#endif

#ifdef WITH_HTTP_API
#  include <microhttpd.h>
#endif

#ifdef __linux__
#define WITH_TCP_USER_TIMEOUT
#endif

#ضمن "mosquitto_internal.h"
#ضمن "mosquitto/broker.h"
#ضمن "mosquitto/broker_plugin.h"
#ضمن "mosquitto.h"
#ضمن "logging_mosq.h"
#ضمن "tls_mosq.h"
#ضمن "uthash.h"
#ضمن "acl_file.h"
#ضمن "password_file.h"

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

تعريفنوع uint64_t dbid_t;

تعريفنوع صحيح (*FUNC_plugin_init_v5)(mosquitto_plugin_id_t *, فراغ **, بنية mosquitto_opt *, صحيح);
تعريفنوع صحيح (*FUNC_plugin_cleanup_v5)(فراغ *, بنية mosquitto_opt *, صحيح);

تعريفنوع صحيح (*FUNC_auth_plugin_init_v4)(فراغ **, بنية mosquitto_opt *, صحيح);
تعريفنوع صحيح (*FUNC_auth_plugin_cleanup_v4)(فراغ *, بنية mosquitto_opt *, صحيح);
تعريفنوع صحيح (*FUNC_auth_plugin_security_init_v4)(فراغ *, بنية mosquitto_opt *, صحيح, bool);
تعريفنوع صحيح (*FUNC_auth_plugin_security_cleanup_v4)(فراغ *, بنية mosquitto_opt *, صحيح, bool);
تعريفنوع صحيح (*FUNC_auth_plugin_acl_check_v4)(فراغ *, صحيح, بنية mosquitto *, بنية mosquitto_acl_پەیام *);
تعريفنوع صحيح (*FUNC_auth_plugin_unpwd_check_v4)(فراغ *, بنية mosquitto *, ثابت حرف *, ثابت حرف *);
تعريفنوع صحيح (*FUNC_auth_plugin_psk_کلیل_get_v4)(فراغ *, بنية mosquitto *, ثابت حرف *, ثابت حرف *, حرف *, صحيح);
تعريفنوع صحيح (*FUNC_auth_plugin_auth_start_v4)(فراغ *, بنية mosquitto *, ثابت حرف *, bool, ثابت فراغ *, uint16_t, فراغ **, uint16_t *);
تعريفنوع صحيح (*FUNC_auth_plugin_auth_continue_v4)(فراغ *, بنية mosquitto *, ثابت حرف *, ثابت فراغ *, uint16_t, فراغ **, uint16_t *);

تعريفنوع صحيح (*FUNC_auth_plugin_init_v3)(فراغ **, بنية mosquitto_opt *, صحيح);
تعريفنوع صحيح (*FUNC_auth_plugin_cleanup_v3)(فراغ *, بنية mosquitto_opt *, صحيح);
تعريفنوع صحيح (*FUNC_auth_plugin_security_init_v3)(فراغ *, بنية mosquitto_opt *, صحيح, bool);
تعريفنوع صحيح (*FUNC_auth_plugin_security_cleanup_v3)(فراغ *, بنية mosquitto_opt *, صحيح, bool);
تعريفنوع صحيح (*FUNC_auth_plugin_acl_check_v3)(فراغ *, صحيح, ثابت بنية mosquitto *, بنية mosquitto_acl_پەیام *);
تعريفنوع صحيح (*FUNC_auth_plugin_unpwd_check_v3)(فراغ *, ثابت بنية mosquitto *, ثابت حرف *, ثابت حرف *);
تعريفنوع صحيح (*FUNC_auth_plugin_psk_کلیل_get_v3)(فراغ *, ثابت بنية mosquitto *, ثابت حرف *, ثابت حرف *, حرف *, صحيح);

تعريفنوع صحيح (*FUNC_auth_plugin_init_v2)(فراغ **, بنية mosquitto_auth_opt *, صحيح);
تعريفنوع صحيح (*FUNC_auth_plugin_cleanup_v2)(فراغ *, بنية mosquitto_auth_opt *, صحيح);
تعريفنوع صحيح (*FUNC_auth_plugin_security_init_v2)(فراغ *, بنية mosquitto_auth_opt *, صحيح, bool);
تعريفنوع صحيح (*FUNC_auth_plugin_security_cleanup_v2)(فراغ *, بنية mosquitto_auth_opt *, صحيح, bool);
تعريفنوع صحيح (*FUNC_auth_plugin_acl_check_v2)(فراغ *, ثابت حرف *, ثابت حرف *, ثابت حرف *, صحيح);
تعريفنوع صحيح (*FUNC_auth_plugin_unpwd_check_v2)(فراغ *, ثابت حرف *, ثابت حرف *);
تعريفنوع صحيح (*FUNC_auth_plugin_psk_کلیل_get_v2)(فراغ *, ثابت حرف *, ثابت حرف *, حرف *, صحيح);


تعداد mosquitto_پەیام_origin {
	mosq_mo_client = 0,
	mosq_mo_broker = 1,
};

بنية mosquitto__plugin_lib {
	فراغ *lib;
	فراغ *user_data;
	صحيح (*plugin_version)(فراغ);
	بنية mosquitto_plugin_id_t *identifier;

	FUNC_plugin_init_v5 plugin_init_v5;
	FUNC_plugin_cleanup_v5 plugin_cleanup_v5;

	FUNC_auth_plugin_init_v4 plugin_init_v4;
	FUNC_auth_plugin_cleanup_v4 plugin_cleanup_v4;
	FUNC_auth_plugin_security_init_v4 security_init_v4;
	FUNC_auth_plugin_security_cleanup_v4 security_cleanup_v4;
	FUNC_auth_plugin_acl_check_v4 acl_check_v4;
	FUNC_auth_plugin_unpwd_check_v4 unpwd_check_v4;
	FUNC_auth_plugin_psk_کلیل_get_v4 psk_کلیل_get_v4;
	FUNC_auth_plugin_auth_start_v4 auth_start_v4;
	FUNC_auth_plugin_auth_continue_v4 auth_continue_v4;

	FUNC_auth_plugin_init_v3 plugin_init_v3;
	FUNC_auth_plugin_cleanup_v3 plugin_cleanup_v3;
	FUNC_auth_plugin_security_init_v3 security_init_v3;
	FUNC_auth_plugin_security_cleanup_v3 security_cleanup_v3;
	FUNC_auth_plugin_acl_check_v3 acl_check_v3;
	FUNC_auth_plugin_unpwd_check_v3 unpwd_check_v3;
	FUNC_auth_plugin_psk_کلیل_get_v3 psk_کلیل_get_v3;

	FUNC_auth_plugin_init_v2 plugin_init_v2;
	FUNC_auth_plugin_cleanup_v2 plugin_cleanup_v2;
	FUNC_auth_plugin_security_init_v2 security_init_v2;
	FUNC_auth_plugin_security_cleanup_v2 security_cleanup_v2;
	FUNC_auth_plugin_acl_check_v2 acl_check_v2;
	FUNC_auth_plugin_unpwd_check_v2 unpwd_check_v2;
	FUNC_auth_plugin_psk_کلیل_get_v2 psk_کلیل_get_v2;
	صحيح version;
};

بنية mosquitto__plugin_config {
	حرف *path;
	حرف *name;
	بنية mosquitto_opt *options;
	بنية mosquitto__security_options **security_options;
	صحيح option_ژماردن;
	صحيح security_option_ژماردن;
	bool deny_special_chars;
};

بنية mosquitto__callback {
	UT_hash_handle hh; /* For callbacks that register for e.g. a specific topic */
	بنية mosquitto__callback *next, *prev; /* For typical callbacks */
	MOSQ_FUNC_generic_callback cb;
	فراغ *userdata;
	اتحاد {
		حرف *topic;
		بنية timespec next_tick;
	} data;
	mosquitto_plugin_id_t *identifier;
};

بنية plugin__callbacks {
	بنية mosquitto__callback *tick;
	بنية mosquitto__callback *acl_check;
	بنية mosquitto__callback *basic_auth;
	بنية mosquitto__callback *connect;
	بنية mosquitto__callback *control;
	بنية mosquitto__callback *disconnect;
	بنية mosquitto__callback *client_offline;
	بنية mosquitto__callback *ext_auth_continue;
	بنية mosquitto__callback *ext_auth_start;
	بنية mosquitto__callback *message_in;
	بنية mosquitto__callback *message_out;
	بنية mosquitto__callback *psk_کلیل;
	بنية mosquitto__callback *reload;
	بنية mosquitto__callback *subscribe;
	بنية mosquitto__callback *unsubscribe;
	بنية mosquitto__callback *persist_restore;
	بنية mosquitto__callback *persist_client_add;
	بنية mosquitto__callback *persist_client_delete;
	بنية mosquitto__callback *persist_client_update;
	بنية mosquitto__callback *persist_subscription_add;
	بنية mosquitto__callback *persist_subscription_delete;
	بنية mosquitto__callback *persist_client_پەیام_add;
	بنية mosquitto__callback *persist_client_پەیام_delete;
	بنية mosquitto__callback *persist_client_پەیام_update;
	بنية mosquitto__callback *persist_base_پەیام_add;
	بنية mosquitto__callback *persist_base_پەیام_delete;
	بنية mosquitto__callback *persist_retain_پەیام_set;
	بنية mosquitto__callback *persist_retain_پەیام_delete;
	بنية mosquitto__callback *persist_will_add;
	بنية mosquitto__callback *persist_will_delete;
};

/* This is owned by mosquitto__config or mosquitto__listener, and only referred
 * to by other structs */
بنية mosquitto__security_options {
	/* Any options that get added here also need considering
	 * in config__read() with regards whether allow_anonymous
	 * should be disabled when these options are set.
	 */
	بنية mosquitto__unpwd *unpwd;
	بنية mosquitto__psk *psk_id;
	بنية acl_file_data acl_data;
	بنية password_file_data password_data;
	حرف *psk_file;
	mosquitto_plugin_id_t **plugins;
	صحيح plugin_ژماردن;
	int8_t allow_anonymous;
	bool allow_zero_length_clientid;
	حرف *auto_id_prefix;
	uint16_t auto_id_prefix_len;
	بنية plugin__callbacks plugin_callbacks;
	mosquitto_plugin_id_t *pid; /* For registering as a "plugin" */
};

#إذا defined(WITH_EPOLL) || defined(WITH_KQUEUE)
تعداد struct_ident {
	id_invalid = 0,
	id_listener = 1,
	id_client = 2,
	id_listener_ws = 3,
};
#endif

بنية mosquitto__listener {
	uint16_t port;
	حرف *host;
	حرف *bind_interface;
	صحيح max_connections;
	حرف *mount_point;
	mosq_sock_t *socks;
	صحيح sock_ژماردن;
	صحيح client_ژماردن;
	تعداد mosquitto_protocol protocol;
	صحيح socket_domain;
	bool use_username_as_clientid;
	uint8_t max_qos;
	uint16_t max_topic_alias;
	uint16_t max_topic_alias_broker;
#ifdef WITH_TLS
	حرف *cafile;
	حرف *capath;
	حرف *certfile;
	حرف *keyfile;
	حرف *tls_engine;
	حرف *tls_engine_kpass_sha1;
	حرف *ciphers;
	حرف *ciphers_tls13;
	حرف *psk_hint;
	SSL_CTX *ssl_ctx;
	حرف *crlfile;
	حرف *tls_version;
	bool use_identity_as_username;
	bool use_subject_as_username;
	bool require_certificate;
	bool disable_client_cert_date_checks;
	تعداد mosquitto__keyform tls_keyform;
#endif
#ifdef WITH_WEBSOCKETS
#  إذا WITH_WEBSOCKETS == WS_IS_LWS
	بنية lws_context *ws_context;
	bool ws_in_init;
	بنية lws_protocols *ws_protocol;
#  endif
	حرف **ws_origins;
	صحيح ws_origin_ژماردن;
#endif
#إذا defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
	حرف *http_dir;
#endif
	بنية mosquitto__security_options *security_options;
#ifdef WITH_UNIX_SOCKETS
	حرف *unix_socket_path;
#endif
	bool disable_protocol_v3;
	bool disable_protocol_v4;
	bool disable_protocol_v5;
	صحيح enable_proxy_protocol;
	bool proxy_protocol_v2_require_tls;
#ifdef WITH_HTTP_API
	بنية MHD_Daemon *mhd;
#endif
};


بنية mosquitto__listener_sock {
#إذا defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	/* This *must* be the first element in the struct. */
	صحيح ident;
#endif
	mosq_sock_t sock;
	بنية mosquitto__listener *listener;
};


/* Callbacks belonging to a specific plugin
 * Doesn't include MOSQ_EVT_CONTROL events.
 */
بنية plugin_own_callback {
	بنية plugin_own_callback *next, *prev;
	MOSQ_FUNC_generic_callback cb_func;
	تعداد mosquitto_plugin_event event;
};

بنية mosquitto_plugin_id_t {
	بنية mosquitto__plugin_config config;
	بنية mosquitto__plugin_lib lib;
	بنية mosquitto__listener *listener;
	حرف *plugin_name;
	حرف *plugin_version;
	بنية control_endpoint *control_endpoints;
	بنية plugin_own_callback *own_callbacks;
	بنية timespec next_tick;
};

بنية mosquitto__config {
	bool allow_duplicate_messages;
	صحيح autosave_interval;
	bool autosave_on_changes;
	bool check_retain_source;
	حرف *clientid_prefixes;
	bool connection_messages;
	uint16_t cmd_port[CMD_PORT_LIMIT];
	صحيح cmd_port_ژماردن;
	bool daemon;
	bool test_configuration;
	bool enable_control_api;
	صحيح global_max_clients;
	صحيح global_max_connections;
	بنية mosquitto__listener *default_listener; /* Points to one of `listeners` */
	بنية mosquitto__listener *listeners;
	صحيح listener_ژماردن;
	bool local_only;
	unsigned صحيح log_dest;
	صحيح log_facility;
	unsigned صحيح log_type;
	bool log_timestamp;
	حرف *log_timestamp_format;
	حرف *log_file;
	FILE *log_fptr;
	size_t max_inflight_bytes;
	size_t max_queued_bytes;
	صحيح max_queued_messages;
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
	حرف *persistence_location;
	حرف *persistence_file;
	حرف *persistence_filepath;
	time_t persistent_client_expiration;
	حرف *pid_file;
	bool queue_qos0_messages;
	bool per_listener_settings;
	bool retain_available;
	صحيح retain_expiry_interval;
	bool set_tcp_nodelay;
	صحيح sys_interval;
	bool upgrade_outgoing_qos;
	حرف *user;
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	صحيح websockets_log_level;
#endif
	uint16_t packet_buffer_size;
#ifdef WITH_BRIDGE
	بنية mosquitto__bridge **bridges;
	صحيح bridge_ژماردن;
#endif
	بنية mosquitto__security_options security_options;
};


بنية mosquitto__subshared {
	UT_hash_handle hh;
	بنية mosquitto__subleaf *subs;
	حرف name[];
};

بنية mosquitto__subhier {
	UT_hash_handle hh;
	بنية mosquitto__subhier *parent;
	بنية mosquitto__subhier *children;
	بنية mosquitto__subleaf *subs;
	بنية mosquitto__subshared *shared;
	uint16_t topic_len;
	حرف topic[];
};

بنية mosquitto__subleaf {
	بنية mosquitto__subleaf *prev;
	بنية mosquitto__subleaf *next;
	بنية mosquitto *context;
	بنية mosquitto__subhier *hier;
	بنية mosquitto__subshared *shared;
	uint32_t identifier;
	uint8_t subscription_options;
	حرف topic_filter[];
};

بنية sub__token {
	بنية sub__token *next;
	uint16_t topic_len;
	حرف topic[];
};

بنية mosquitto__retainhier {
	UT_hash_handle hh;
	بنية mosquitto__retainhier *parent;
	بنية mosquitto__retainhier *children;
	بنية mosquitto_base_پەیام *retained;
	uint16_t topic_len;
	حرف topic[];
};

بنية mosquitto_base_پەیام {
	UT_hash_handle hh;
	بنية mosquitto_base_پەیام_2 data;
	بنية mosquitto__listener *source_listener;
	حرف **dest_ids;
	صحيح dest_id_ژماردن;
	صحيح ref_ژماردن;
	تعداد mosquitto_پەیام_origin origin;
	bool stored;
};

بنية mosquitto_client_پەیام {
	بنية mosquitto_client_پەیام_2 data;
	بنية mosquitto_client_پەیام *prev;
	بنية mosquitto_client_پەیام *next;
	بنية mosquitto_base_پەیام *base_پەیام;
};


بنية mosquitto__psk {
	UT_hash_handle hh;
	حرف *username;
	حرف *password;
};

بنية mosquitto__message_v5 {
	بنية mosquitto__message_v5 *next, *prev;
	حرف *topic;
	فراغ *payload;
	mosquitto_property *properties;
	/* clientid is used only by mosquitto_broker_publish*() to indicate
	   this message is for a specific client. */
	حرف *clientid;
	صحيح payloadlen;
	صحيح qos;
	bool retain;
};

بنية mosquitto_db {
	dbid_t last_db_id;
	uint64_t گرێ_id_shifted;
	بنية mosquitto__subhier *normal_subs;
	بنية mosquitto__subhier *shared_subs;
	بنية mosquitto__retainhier *retains;
	بنية mosquitto *contexts_by_id;
	بنية mosquitto *contexts_by_sock;
	بنية mosquitto *contexts_by_id_delayed_auth;
	بنية mosquitto *contexts_for_free;
	mosquitto_plugin_id_t **plugins;
	صحيح plugin_ژماردن;
#ifdef WITH_BRIDGE
	بنية mosquitto **bridges;
	صحيح bridge_ژماردن;
#endif
	بنية clientid__index_hash *clientid_index_hash;
	بنية mosquitto_base_پەیام *پەیام_store;
	time_t now_s; /* Monotonic clock, where possible */
	time_t now_real_s; /* Read clock, for measuring session/message expiry */
	uint64_t گرێ_id; /* for unique db ids */
	time_t next_event_ms; /* for mux timeout */
	صحيح پەیام_store_ژماردن;
	unsigned long پەیام_store_bytes;
	حرف *config_file;
	بنية mosquitto__config *config;
	bool quiet;
	bool verbose;
#ifdef WITH_SYS_درەخت
	صحيح subscription_ژماردن;
	صحيح shared_subscription_ژماردن;
	صحيح retained_ژماردن;
#endif
	صحيح persistence_changes;
	بنية mosquitto *ll_for_free;
#ifdef WITH_EPOLL
	صحيح epollfd;
#endif
#ifdef WITH_KQUEUE
	صحيح kqueuefd;
#endif
	بنية mosquitto__message_v5 *plugin_msgs;
#ifdef WITH_TLS
	/* tls_keylog can't be in the config struct because it is used
	   before the config is allocated. Config probably
	   shouldn't be separately allocated. */
	حرف *tls_keylog;
#endif
	bool shutdown;
};

تعداد mosquitto__bridge_direction {
	bd_out = 0,
	bd_in = 1,
	bd_both = 2,
};

تعداد mosquitto_bridge_start_type {
	bst_automatic = 0,
	bst_lazy = 1,
	bst_manual = 2,
	bst_once = 3,
};

تعداد mosquitto_bridge_reload_type {
	brt_lazy = 0,
	brt_immediate = 1,
};

بنية mosquitto__bridge_topic {
	بنية mosquitto__bridge_topic *next;
	حرف *topic;
	حرف *local_prefix;
	حرف *remote_prefix;
	حرف *local_topic; /* topic prefixed with local_prefix */
	حرف *remote_topic; /* topic prefixed with remote_prefix */
	تعداد mosquitto__bridge_direction direction;
	uint8_t qos;
};

بنية bridge_address {
	حرف *address;
	uint16_t port;
};

بنية mosquitto__bridge {
	حرف *name;
	بنية bridge_address *addresses;
	صحيح cur_address;
	صحيح address_ژماردن;
	time_t primary_retry;
	mosq_sock_t primary_retry_sock;
	bool round_robin;
	bool try_private;
	bool try_private_accepted;
	bool clean_start;
	int8_t clean_start_local;
	uint16_t keepalive;
	unsigned صحيح tcp_keepalive_idle;
	unsigned صحيح tcp_keepalive_interval;
	unsigned صحيح tcp_keepalive_counter;
#ifdef WITH_TCP_USER_TIMEOUT
	صحيح tcp_user_timeout;
#endif
	بنية mosquitto__bridge_topic *topics;
	صحيح topic_ژماردن;
	bool topic_remapping;
	تعداد mosquitto__protocol protocol_version;
	time_t restart_t;
	time_t connected_at;
	حرف *remote_clientid;
	حرف *remote_username;
	حرف *remote_password;
	حرف *local_clientid;
	حرف *local_username;
	حرف *local_password;
	حرف *notification_topic;
	حرف *bind_address;
	bool notifications;
	bool notifications_local_only;
	تعداد mosquitto_bridge_start_type start_type;
	صحيح idle_timeout;
	صحيح restart_timeout;
	صحيح backoff_base;
	صحيح backoff_cap;
	صحيح stable_connection_period;
	صحيح threshold;
	uint32_t maximum_packet_size;
	uint32_t session_expiry_interval;
	uint16_t receive_maximum;
	bool lazy_reconnect;
	bool attempt_unsubscribe;
	bool initial_notification_done;
	bool outgoing_retain;
	تعداد mosquitto_bridge_reload_type reload_type;
	uint16_t max_topic_alias;
#ifdef WITH_TLS
	bool tls_insecure;
	bool tls_ocsp_required;
	bool tls_use_os_certs;
	حرف *tls_cafile;
	حرف *tls_capath;
	حرف *tls_certfile;
	حرف *tls_keyfile;
	حرف *tls_version;
	حرف *tls_alpn;
	حرف *tls_ciphers;
	حرف *tls_13_ciphers;
#  ifdef FINAL_WITH_TLS_PSK
	حرف *tls_psk_identity;
	حرف *tls_psk;
#  endif
#endif
};

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
بنية libws_mqtt_hack {
	حرف *http_dir;
	بنية mosquitto__listener *listener;
};

بنية libws_mqtt_data {
	بنية mosquitto *mosq;
};
#endif

#ضمن <net_mosq.h>


بنية control_endpoint {
	بنية control_endpoint *next, *prev;
	حرف topic[];
};

خارجي بنية mosquitto_db db;

/* ============================================================
 * Main functions
 * ============================================================ */
صحيح mosquitto_main_loop(بنية mosquitto__listener_sock *listensock, صحيح listensock_ژماردن);
فراغ loop__update_next_event(time_t new_ms);

/* ============================================================
 * Config functions
 * ============================================================ */
/* Initialise config struct to default values. */
فراغ config__init(بنية mosquitto__config *config);
#ifdef WITH_BRIDGE
فراغ config__bridge_cleanup(بنية mosquitto__bridge *bridge);
#endif
/* Parse command line options into config. */
صحيح config__parse_args(بنية mosquitto__config *config, صحيح argc, حرف *argv[]);
/* Read configuration data from config->config_file into config.
 * If reload is true, don't process config options that shouldn't be reloaded (listeners etc)
 * Returns 0 on success, 1 if there is a configuration error or if a file cannot be opened.
 */
صحيح config__read(بنية mosquitto__config *config, bool reload);
/* Free all config data. */
فراغ config__cleanup(بنية mosquitto__config *config);
صحيح config__get_dir_files(ثابت حرف *include_dir, حرف ***files, صحيح *file_ژماردن);

/* ============================================================
 * Server send functions
 * ============================================================ */
صحيح send__connack(بنية mosquitto *context, uint8_t ack, uint8_t reason_code, ثابت mosquitto_property *properties);
صحيح send__suback(بنية mosquitto *context, uint16_t mid, uint32_t payloadlen, ثابت فراغ *payload);
صحيح send__unsuback(بنية mosquitto *context, uint16_t mid, صحيح reason_code_ژماردن, uint8_t *reason_codes, ثابت mosquitto_property *properties);
صحيح send__auth(بنية mosquitto *context, uint8_t reason_code, ثابت فراغ *auth_data, uint16_t auth_data_len);

/* ============================================================
 * Network functions
 * ============================================================ */
فراغ net__broker_init(فراغ);
فراغ net__broker_cleanup(فراغ);
بنية mosquitto *net__socket_accept(بنية mosquitto__listener_sock *listensock);
صحيح net__socket_listen(بنية mosquitto__listener *listener);
صحيح net__socket_get_address(mosq_sock_t sock, حرف *buf, size_t len, uint16_t *remote_address);
صحيح net__tls_load_verify(بنية mosquitto__listener *listener);
صحيح net__tls_server_ctx(بنية mosquitto__listener *listener);
صحيح net__load_certificates(بنية mosquitto__listener *listener);

/* ============================================================
 * Read handling functions
 * ============================================================ */
صحيح handle__packet(بنية mosquitto *context);
صحيح handle__connack(بنية mosquitto *context);
صحيح handle__connect(بنية mosquitto *context);
صحيح handle__disconnect(بنية mosquitto *context);
صحيح handle__publish(بنية mosquitto *context);
صحيح handle__subscribe(بنية mosquitto *context);
صحيح handle__unsubscribe(بنية mosquitto *context);
صحيح handle__auth(بنية mosquitto *context);

/* ============================================================
 * Database handling
 * ============================================================ */
صحيح db__open(بنية mosquitto__config *config);
صحيح db__close(فراغ);
#ifdef WITH_PERSISTENCE
صحيح persist__backup(bool shutdown);
صحيح persist__restore(فراغ);
#endif
/* Return the number of in-flight messages in count. */
صحيح db_message_ژماردن(صحيح *ژماردن);
صحيح db__message_delete_outgoing(بنية mosquitto *context, uint16_t mid, تعداد mosquitto_پەیام_state expect_state, صحيح qos);
صحيح db_message_زیادکردن_outgoing(بنية mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, بنية mosquitto_base_پەیام *base_پەیام, uint32_t subscription_identifier, bool update, bool persist);
صحيح db_message_زیادکردن_incoming(بنية mosquitto *context, uint64_t cmsg_id, بنية mosquitto_base_پەیام *base_پەیام, bool persist);
صحيح db__message_remove_incoming(بنية mosquitto *context, uint16_t mid);
صحيح db__message_release_incoming(بنية mosquitto *context, uint16_t mid);
صحيح db__message_update_outgoing(بنية mosquitto *context, uint16_t mid, تعداد mosquitto_پەیام_state state, صحيح qos, bool persist);
فراغ db__message_dequeue_first(بنية mosquitto *context, بنية mosquitto_پەیام_data *پەیام_data);
صحيح db__messages_delete(بنية mosquitto *context, bool force_free);
صحيح db__messages_delete_incoming(بنية mosquitto *context);
صحيح db__messages_delete_outgoing(بنية mosquitto *context);
صحيح db__messages_easy_queue(بنية mosquitto *context, ثابت حرف *topic, uint8_t qos, uint32_t payloadlen, ثابت فراغ *payload, صحيح retain, uint32_t message_expiry_interval, mosquitto_property **properties);
صحيح db__message_store(ثابت بنية mosquitto *source, بنية mosquitto_base_پەیام *base_پەیام, uint32_t *message_expiry_interval, تعداد mosquitto_پەیام_origin origin);
صحيح db__message_store_find(بنية mosquitto *context, uint16_t mid, بنية mosquitto_client_پەیام **client_پەیام);
صحيح db_پەیام_store_add(بنية mosquitto_base_پەیام *base_پەیام);
فراغ db_پەیام_store_remove(بنية mosquitto_base_پەیام *base_پەیام, bool notify);
فراغ db_پەیام_store_ref_inc(بنية mosquitto_base_پەیام *base_پەیام);
فراغ db_پەیام_store_ref_dec(بنية mosquitto_base_پەیام **base_پەیام);
فراغ db_پەیام_store_clean(فراغ);
فراغ db_پەیام_store_compact(فراغ);
فراغ db_پەیام_store_free(بنية mosquitto_base_پەیام *base_پەیام);
صحيح db__message_reconnect_reset(بنية mosquitto *context);
bool db__ready_for_flight(بنية mosquitto *context, تعداد mosquitto_پەیام_direction dir, صحيح qos);
bool db__ready_for_queue(بنية mosquitto *context, صحيح qos, بنية mosquitto_پەیام_data *پەیام_data);
فراغ sys_درەخت_init(فراغ);
فراغ sys_درەخت_update(bool force);
صحيح db__message_write_inflight_out_all(بنية mosquitto *context);
صحيح db__message_write_inflight_out_latest(بنية mosquitto *context);
صحيح db__message_write_queued_out(بنية mosquitto *context);
صحيح db__message_write_queued_in(بنية mosquitto *context);
فراغ db_پەیام_add_to_inflight_stats(بنية mosquitto_پەیام_data *پەیام_data, بنية mosquitto_client_پەیام *پەیام);
فراغ db_پەیام_add_to_queued_stats(بنية mosquitto_پەیام_data *پەیام_data, بنية mosquitto_client_پەیام *پەیام);
uint64_t db_new_پەیام_id(فراغ);
فراغ db__expire_all_messages(بنية mosquitto *context);
فراغ db__check_acl_of_all_messages(بنية mosquitto *context);

/* ============================================================
 * Subscription functions
 * ============================================================ */
صحيح sub__init(فراغ);
صحيح sub__add(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub);
صحيح sub__remove(بنية mosquitto *context, ثابت حرف *sub, uint8_t *reason);
فراغ sub_درەخت_print(بنية mosquitto__subhier *ڕەگ, صحيح level);
صحيح sub__clean_session(بنية mosquitto *context);
صحيح sub__messages_queue(ثابت حرف *source_id, uint8_t qos, بنية mosquitto_base_پەیام **base_پەیام);
صحيح sub__topic_tokenise(ثابت حرف *subtopic, حرف **local_sub, حرف ***topics, ثابت حرف **sharename);
فراغ sub__topic_tokens_free(بنية sub__token *tokens);

/* ============================================================
 * Context functions
 * ============================================================ */
بنية mosquitto *context__init(فراغ);
صحيح context__init_sock(بنية mosquitto *context, mosq_sock_t sock, bool get_address);
فراغ context__cleanup(بنية mosquitto *context, bool force_free);
فراغ context__disconnect(بنية mosquitto *context, صحيح reason);
فراغ context__add_to_disused(بنية mosquitto *context);
فراغ context__free_disused(فراغ);
فراغ context__send_will(بنية mosquitto *context);
فراغ context__add_to_by_id(بنية mosquitto *context);
فراغ context__remove_from_by_id(بنية mosquitto *context);

صحيح connect__on_authorised(بنية mosquitto *context, فراغ *auth_data_out, uint16_t auth_data_out_len);


/* ============================================================
 * Control functions
 * ============================================================ */
#ifdef WITH_CONTROL
صحيح control__process(بنية mosquitto *context, بنية mosquitto_base_پەیام *base_پەیام);
فراغ control__cleanup(فراغ);
#endif
صحيح control__register_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ثابت حرف *topic, فراغ *userdata);
صحيح control__unregister_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ثابت حرف *topic);
فراغ control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier);


/* ============================================================
 * Logging functions
 * ============================================================ */
صحيح log__init(بنية mosquitto__config *config);
صحيح log__close(بنية mosquitto__config *config);
فراغ log__internal(ثابت حرف *fmt, ...) __attribute__((format(printf, 1, 2)));

/* ============================================================
 * Bridge functions
 * ============================================================ */
#ifdef WITH_BRIDGE
فراغ bridge__start_all(فراغ);
فراغ bridge__reload(فراغ);
فراغ bridge__db_cleanup(فراغ);
فراغ bridge__cleanup(بنية mosquitto *context);
صحيح bridge__connect(بنية mosquitto *context);
#إذا defined(__GLIBC__) && defined(WITH_ADNS)
صحيح bridge__connect_step3(بنية mosquitto *context);
#endif
صحيح bridge__on_connect(بنية mosquitto *context);
فراغ bridge_check(فراغ);
صحيح bridge__register_local_connections(فراغ);
صحيح bridge__add_topic(بنية mosquitto__bridge *bridge, ثابت حرف *topic, تعداد mosquitto__bridge_direction direction, uint8_t qos, ثابت حرف *local_prefix, ثابت حرف *remote_prefix);
فراغ bridge__cleanup_topics(بنية mosquitto__bridge *bridge);
صحيح bridge__remap_topic_in(بنية mosquitto *context, حرف **topic);
#endif

/* ============================================================
 * IO multiplex related functions
 * ============================================================ */
صحيح mux__init(فراغ);
صحيح mux__add_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_ژماردن);
صحيح mux__delete_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_ژماردن);
صحيح mux__loop_prepare(فراغ);
صحيح mux__new(بنية mosquitto *context);
صحيح mux__add_out(بنية mosquitto *context);
صحيح mux__remove_out(بنية mosquitto *context);
صحيح mux__delete(بنية mosquitto *context);
صحيح mux__wait(فراغ);
صحيح mux__handle(بنية mosquitto__listener_sock *listensock, صحيح listensock_ژماردن);
صحيح mux__cleanup(فراغ);

/* ============================================================
 * Listener related functions
 * ============================================================ */
خارجي بنية mosquitto__listener_sock *g_listensock;
خارجي صحيح g_listensock_ژماردن;

فراغ listener__set_defaults(بنية mosquitto__listener *listener);
فراغ listeners__reload_all_certificates(فراغ);
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
فراغ listeners__add_websockets(بنية lws_context *ws_context, mosq_sock_t fd);
#endif
صحيح listeners__start(فراغ);
فراغ listeners__stop(فراغ);

/* ============================================================
 * Plugin related functions
 * ============================================================ */
صحيح plugin__load_v5(mosquitto_plugin_id_t *plugin, فراغ *lib);
صحيح plugin__load_v4(mosquitto_plugin_id_t *plugin, فراغ *lib);
صحيح plugin__load_v3(mosquitto_plugin_id_t *plugin, فراغ *lib);
صحيح plugin__load_v2(mosquitto_plugin_id_t *plugin, فراغ *lib);
صحيح acl__pre_check(mosquitto_plugin_id_t *plugin, بنية mosquitto *context, صحيح access);

فراغ LIB_ERROR(فراغ);
فراغ plugin__handle_connect(بنية mosquitto *context);
فراغ plugin__handle_disconnect(بنية mosquitto *context, صحيح reason);
فراغ plugin__handle_client_offline(بنية mosquitto *context, صحيح reason);
صحيح plugin__handle_message_in(بنية mosquitto *context, بنية mosquitto_base_پەیام_2 *base_پەیام);
صحيح plugin__handle_message_out(بنية mosquitto *context, بنية mosquitto_base_پەیام_2 *base_پەیام);
صحيح plugin__handle_subscribe(بنية mosquitto *context, بنية mosquitto_subscription *sub);
صحيح plugin__handle_unsubscribe(بنية mosquitto *context, بنية mosquitto_subscription *sub);
صحيح plugin__handle_reload(فراغ);
فراغ plugin__handle_tick(فراغ);
صحيح plugin__callback_unregister_all(mosquitto_plugin_id_t *identifier);
فراغ plugin_persist__handle_restore(فراغ);
فراغ plugin_persist__handle_client_add(بنية mosquitto *context);
فراغ plugin_persist__handle_client_delete(بنية mosquitto *context);
فراغ plugin_persist__handle_client_update(بنية mosquitto *context);
فراغ plugin_persist__handle_subscription_add(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub);
فراغ plugin_persist__handle_subscription_delete(بنية mosquitto *context, حرف *sub);
فراغ plugin_persist_handle_client_پەیام_add(بنية mosquitto *context, ثابت بنية mosquitto_client_پەیام *cmsg);
فراغ plugin_persist_handle_client_پەیام_delete(بنية mosquitto *context, ثابت بنية mosquitto_client_پەیام *cmsg);
فراغ plugin_persist_handle_client_پەیام_update(بنية mosquitto *context, ثابت بنية mosquitto_client_پەیام *cmsg);
فراغ plugin_persist_handle_base_پەیام_add(بنية mosquitto_base_پەیام *base_پەیام);
فراغ plugin_persist_handle_base_پەیام_delete(بنية mosquitto_base_پەیام *base_پەیام);
فراغ plugin_persist_handle_retain_پەیام_set(بنية mosquitto_base_پەیام *base_پەیام);
فراغ plugin_persist_handle_retain_پەیام_delete(بنية mosquitto_base_پەیام *base_پەیام);
فراغ plugin_persist__handle_will_add(بنية mosquitto *context);
فراغ plugin_persist__handle_will_delete(بنية mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
صحيح keepalive__init(فراغ);
فراغ keepalive__cleanup(فراغ);
صحيح keepalive__add(بنية mosquitto *context);
فراغ keepalive__check(فراغ);
صحيح keepalive__remove(بنية mosquitto *context);
صحيح keepalive__update(بنية mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
صحيح property__process_connect(بنية mosquitto *context, mosquitto_property **props);
صحيح property__process_will(بنية mosquitto *context, بنية mosquitto_message_all *پەیام, mosquitto_property **props);
صحيح property__process_publish(بنية mosquitto_base_پەیام *base_پەیام, mosquitto_property **props, صحيح *topic_alias, uint32_t *message_expiry_interval, bool is_bridge);
صحيح property__process_disconnect(بنية mosquitto *context, mosquitto_property **props);

/* ============================================================
 * Retain tree related functions
 * ============================================================ */
صحيح retain__init(فراغ);
فراغ retain__clean(بنية mosquitto__retainhier **retainhier);
صحيح retain__queue(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub);
صحيح retain__store(ثابت حرف *topic, بنية mosquitto_base_پەیام *base_پەیام, حرف **split_topics, bool persist);
فراغ retain__expiry_check(فراغ);
فراغ retain__expire(بنية mosquitto__retainhier **retainhier);

/* ============================================================
 * Security related functions
 * ============================================================ */
صحيح acl__find_acls(بنية mosquitto *context);
صحيح plugin__load_all(فراغ);
صحيح plugin__unload_all(فراغ);
صحيح config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, بنية mosquitto__security_options *security_options);

صحيح mosquitto_security_init(bool reload);
صحيح mosquitto_security_cleanup(bool reload);
صحيح mosquitto_acl_check(بنية mosquitto *context, ثابت حرف *topic, uint32_t payloadlen, فراغ *payload, uint8_t qos, bool retain, mosquitto_property *properties, صحيح access);
صحيح mosquitto_basic_auth(بنية mosquitto *context);
صحيح mosquitto_psk_کلیل_get(بنية mosquitto *context, ثابت حرف *hint, ثابت حرف *identity, حرف *کلیل, صحيح max_کلیل_len);

صحيح mosquitto_security_init_default(فراغ);
صحيح mosquitto_security_apply_default(فراغ);
صحيح mosquitto_security_cleanup_default(فراغ);
صحيح mosquitto_psk_کلیل_get_default(بنية mosquitto *context, ثابت حرف *hint, ثابت حرف *identity, حرف *کلیل, صحيح max_کلیل_len);
صحيح broker_acl_file__init(فراغ);
فراغ broker_acl_file__cleanup(فراغ);
صحيح broker_password_file__init(فراغ);
فراغ broker_password_file__cleanup(فراغ);
صحيح psk_file__init(فراغ);
صحيح psk_file__cleanup(فراغ);

صحيح mosquitto_security_auth_start(بنية mosquitto *context, bool reauth, ثابت فراغ *data_in, uint16_t data_in_len, فراغ **data_out, uint16_t *data_out_len);
صحيح mosquitto_security_auth_continue(بنية mosquitto *context, ثابت فراغ *data_in, uint16_t data_len, فراغ **data_out, uint16_t *data_out_len);

فراغ unpwd__free_item(بنية mosquitto__unpwd **unpwd, بنية mosquitto__unpwd *item);

/* ============================================================
 * Session expiry
 * ============================================================ */
صحيح session_expiry__add(بنية mosquitto *context);
صحيح session_expiry__add_from_persistence(بنية mosquitto *context, time_t expiry_time);
فراغ session_expiry__add_on_shutdown(فراغ);
فراغ session_expiry__remove(بنية mosquitto *context);
فراغ session_expiry__remove_all(فراغ);
فراغ session_expiry__check(فراغ);
فراغ session_expiry__send_all(فراغ);

/* ============================================================
 * Signals
 * ============================================================ */
فراغ signal__setup(فراغ);
صحيح signal__flag_check(فراغ);

/* ============================================================
 * Window service and signal related functions
 * ============================================================ */
#إذا defined(WIN32) || defined(__CYGWIN__)
فراغ service_install(حرف *name);
فراغ service_uninstall(حرف *name);
فراغ service_run(حرف *name);

#ifdef WIN32
DWORD WINAPI SigThreadProc(فراغ *data);
#endif
#endif

/* ============================================================
 * Watchdog
 * ============================================================ */
فراغ watchdog__init(فراغ);
فراغ watchdog__check(فراغ);

/* ============================================================
 * Websockets related functions
 * ============================================================ */
#ifdef WITH_WEBSOCKETS
#  إذا WITH_WEBSOCKETS == WS_IS_LWS
فراغ mosq_websockets_init(بنية mosquitto__listener *listener, ثابت بنية mosquitto__config *conf);
#  endif
صحيح http__context_init(بنية mosquitto *context);
صحيح http__context_cleanup(بنية mosquitto *context);
صحيح http__read(بنية mosquitto *context);
#endif
فراغ do_disconnect(بنية mosquitto *context, صحيح reason);

/* ============================================================
 * PROXY related functions
 * ============================================================ */
صحيح proxy_v2__read(بنية mosquitto *context);
صحيح proxy_v1__read(بنية mosquitto *context);

/* ============================================================
 * Will delay
 * ============================================================ */
صحيح will_delay__add(بنية mosquitto *context);
فراغ will_delay__check(فراغ);
فراغ will_delay__send_all(فراغ);
فراغ will_delay__remove(بنية mosquitto *mosq);

/* ============================================================
 * HTTP Info
 * ============================================================ */
صحيح http_api__start_local(بنية mosquitto__listener *listener);
صحيح http_api__start(بنية mosquitto__listener *listener);
فراغ http_api__stop(بنية mosquitto__listener *listener);


/* ============================================================
 * Other
 * ============================================================ */
#ifdef WITH_XTREPORT
فراغ xtreport(فراغ);
#endif

فراغ broker_control__init(فراغ);
فراغ broker_control__cleanup(فراغ);
فراغ broker_control__reload(فراغ);

#endif
