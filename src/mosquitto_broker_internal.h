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

#شامل "config.h"
#شامل <stdio.h>

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#  اگر LWS_LIBRARY_VERSION_عدد >= 3002000 && !defined(LWS_WITH_EXTERNAL_POLL)
#    warning "libwebsockets is not compiled with LWS_WITH_EXTERNAL_POLL support. Websocket performance will be unusable."
#  endif
#endif

#ifdef WITH_HTTP_API
#  include <microhttpd.h>
#endif

#ifdef __linux__
#define WITH_TCP_USER_TIMEOUT
#endif

#شامل "mosquitto_internal.h"
#شامل "mosquitto/broker.h"
#شامل "mosquitto/broker_plugin.h"
#شامل "mosquitto.h"
#شامل "logging_mosq.h"
#شامل "tls_mosq.h"
#شامل "uthash.h"
#شامل "acl_file.h"
#شامل "password_file.h"

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

تعریفنوع uint64_t dbid_t;

تعریفنوع صحیح (*FUNC_plugin_init_v5)(mosquitto_plugin_id_t *, تهی **, ساختار mosquitto_opt *, صحیح);
تعریفنوع صحیح (*FUNC_plugin_cleanup_v5)(تهی *, ساختار mosquitto_opt *, صحیح);

تعریفنوع صحیح (*FUNC_auth_plugin_init_v4)(تهی **, ساختار mosquitto_opt *, صحیح);
تعریفنوع صحیح (*FUNC_auth_plugin_cleanup_v4)(تهی *, ساختار mosquitto_opt *, صحیح);
تعریفنوع صحیح (*FUNC_auth_plugin_security_init_v4)(تهی *, ساختار mosquitto_opt *, صحیح, bool);
تعریفنوع صحیح (*FUNC_auth_plugin_security_cleanup_v4)(تهی *, ساختار mosquitto_opt *, صحیح, bool);
تعریفنوع صحیح (*FUNC_auth_plugin_acl_check_v4)(تهی *, صحیح, ساختار mosquitto *, ساختار mosquitto_acl_پیام *);
تعریفنوع صحیح (*FUNC_auth_plugin_unpwd_check_v4)(تهی *, ساختار mosquitto *, ثابت نویسه *, ثابت نویسه *);
تعریفنوع صحیح (*FUNC_auth_plugin_psk_کلید_get_v4)(تهی *, ساختار mosquitto *, ثابت نویسه *, ثابت نویسه *, نویسه *, صحیح);
تعریفنوع صحیح (*FUNC_auth_plugin_auth_start_v4)(تهی *, ساختار mosquitto *, ثابت نویسه *, bool, ثابت تهی *, uint16_t, تهی **, uint16_t *);
تعریفنوع صحیح (*FUNC_auth_plugin_auth_continue_v4)(تهی *, ساختار mosquitto *, ثابت نویسه *, ثابت تهی *, uint16_t, تهی **, uint16_t *);

تعریفنوع صحیح (*FUNC_auth_plugin_init_v3)(تهی **, ساختار mosquitto_opt *, صحیح);
تعریفنوع صحیح (*FUNC_auth_plugin_cleanup_v3)(تهی *, ساختار mosquitto_opt *, صحیح);
تعریفنوع صحیح (*FUNC_auth_plugin_security_init_v3)(تهی *, ساختار mosquitto_opt *, صحیح, bool);
تعریفنوع صحیح (*FUNC_auth_plugin_security_cleanup_v3)(تهی *, ساختار mosquitto_opt *, صحیح, bool);
تعریفنوع صحیح (*FUNC_auth_plugin_acl_check_v3)(تهی *, صحیح, ثابت ساختار mosquitto *, ساختار mosquitto_acl_پیام *);
تعریفنوع صحیح (*FUNC_auth_plugin_unpwd_check_v3)(تهی *, ثابت ساختار mosquitto *, ثابت نویسه *, ثابت نویسه *);
تعریفنوع صحیح (*FUNC_auth_plugin_psk_کلید_get_v3)(تهی *, ثابت ساختار mosquitto *, ثابت نویسه *, ثابت نویسه *, نویسه *, صحیح);

تعریفنوع صحیح (*FUNC_auth_plugin_init_v2)(تهی **, ساختار mosquitto_auth_opt *, صحیح);
تعریفنوع صحیح (*FUNC_auth_plugin_cleanup_v2)(تهی *, ساختار mosquitto_auth_opt *, صحیح);
تعریفنوع صحیح (*FUNC_auth_plugin_security_init_v2)(تهی *, ساختار mosquitto_auth_opt *, صحیح, bool);
تعریفنوع صحیح (*FUNC_auth_plugin_security_cleanup_v2)(تهی *, ساختار mosquitto_auth_opt *, صحیح, bool);
تعریفنوع صحیح (*FUNC_auth_plugin_acl_check_v2)(تهی *, ثابت نویسه *, ثابت نویسه *, ثابت نویسه *, صحیح);
تعریفنوع صحیح (*FUNC_auth_plugin_unpwd_check_v2)(تهی *, ثابت نویسه *, ثابت نویسه *);
تعریفنوع صحیح (*FUNC_auth_plugin_psk_کلید_get_v2)(تهی *, ثابت نویسه *, ثابت نویسه *, نویسه *, صحیح);


شمارش mosquitto_پیام_origin {
	mosq_mo_client = 0,
	mosq_mo_broker = 1,
};

ساختار mosquitto__plugin_lib {
	تهی *lib;
	تهی *user_data;
	صحیح (*plugin_version)(تهی);
	ساختار mosquitto_plugin_id_t *identifier;

	FUNC_plugin_init_v5 plugin_init_v5;
	FUNC_plugin_cleanup_v5 plugin_cleanup_v5;

	FUNC_auth_plugin_init_v4 plugin_init_v4;
	FUNC_auth_plugin_cleanup_v4 plugin_cleanup_v4;
	FUNC_auth_plugin_security_init_v4 security_init_v4;
	FUNC_auth_plugin_security_cleanup_v4 security_cleanup_v4;
	FUNC_auth_plugin_acl_check_v4 acl_check_v4;
	FUNC_auth_plugin_unpwd_check_v4 unpwd_check_v4;
	FUNC_auth_plugin_psk_کلید_get_v4 psk_کلید_get_v4;
	FUNC_auth_plugin_auth_start_v4 auth_start_v4;
	FUNC_auth_plugin_auth_continue_v4 auth_continue_v4;

	FUNC_auth_plugin_init_v3 plugin_init_v3;
	FUNC_auth_plugin_cleanup_v3 plugin_cleanup_v3;
	FUNC_auth_plugin_security_init_v3 security_init_v3;
	FUNC_auth_plugin_security_cleanup_v3 security_cleanup_v3;
	FUNC_auth_plugin_acl_check_v3 acl_check_v3;
	FUNC_auth_plugin_unpwd_check_v3 unpwd_check_v3;
	FUNC_auth_plugin_psk_کلید_get_v3 psk_کلید_get_v3;

	FUNC_auth_plugin_init_v2 plugin_init_v2;
	FUNC_auth_plugin_cleanup_v2 plugin_cleanup_v2;
	FUNC_auth_plugin_security_init_v2 security_init_v2;
	FUNC_auth_plugin_security_cleanup_v2 security_cleanup_v2;
	FUNC_auth_plugin_acl_check_v2 acl_check_v2;
	FUNC_auth_plugin_unpwd_check_v2 unpwd_check_v2;
	FUNC_auth_plugin_psk_کلید_get_v2 psk_کلید_get_v2;
	صحیح version;
};

ساختار mosquitto__plugin_config {
	نویسه *path;
	نویسه *name;
	ساختار mosquitto_opt *options;
	ساختار mosquitto__security_options **security_options;
	صحیح option_شمار;
	صحیح security_option_شمار;
	bool deny_special_chars;
};

ساختار mosquitto__callback {
	UT_hash_handle hh; /* For callbacks that register for e.g. a specific topic */
	ساختار mosquitto__callback *next, *prev; /* For typical callbacks */
	MOSQ_FUNC_generic_callback cb;
	تهی *userdata;
	اجتماع {
		نویسه *topic;
		ساختار timespec next_tick;
	} data;
	mosquitto_plugin_id_t *identifier;
};

ساختار plugin__callbacks {
	ساختار mosquitto__callback *tick;
	ساختار mosquitto__callback *acl_check;
	ساختار mosquitto__callback *basic_auth;
	ساختار mosquitto__callback *connect;
	ساختار mosquitto__callback *control;
	ساختار mosquitto__callback *disconnect;
	ساختار mosquitto__callback *client_offline;
	ساختار mosquitto__callback *ext_auth_continue;
	ساختار mosquitto__callback *ext_auth_start;
	ساختار mosquitto__callback *message_in;
	ساختار mosquitto__callback *message_out;
	ساختار mosquitto__callback *psk_کلید;
	ساختار mosquitto__callback *reload;
	ساختار mosquitto__callback *subscribe;
	ساختار mosquitto__callback *unsubscribe;
	ساختار mosquitto__callback *persist_restore;
	ساختار mosquitto__callback *persist_client_add;
	ساختار mosquitto__callback *persist_client_delete;
	ساختار mosquitto__callback *persist_client_update;
	ساختار mosquitto__callback *persist_subscription_add;
	ساختار mosquitto__callback *persist_subscription_delete;
	ساختار mosquitto__callback *persist_client_پیام_add;
	ساختار mosquitto__callback *persist_client_پیام_delete;
	ساختار mosquitto__callback *persist_client_پیام_update;
	ساختار mosquitto__callback *persist_base_پیام_add;
	ساختار mosquitto__callback *persist_base_پیام_delete;
	ساختار mosquitto__callback *persist_retain_پیام_set;
	ساختار mosquitto__callback *persist_retain_پیام_delete;
	ساختار mosquitto__callback *persist_will_add;
	ساختار mosquitto__callback *persist_will_delete;
};

/* This is owned by mosquitto__config or mosquitto__listener, and only referred
 * to by other structs */
ساختار mosquitto__security_options {
	/* Any options that get added here also need considering
	 * in config__read() with regards whether allow_anonymous
	 * should be disabled when these options are set.
	 */
	ساختار mosquitto__unpwd *unpwd;
	ساختار mosquitto__psk *psk_id;
	ساختار acl_file_data acl_data;
	ساختار password_file_data password_data;
	نویسه *psk_file;
	mosquitto_plugin_id_t **plugins;
	صحیح plugin_شمار;
	int8_t allow_anonymous;
	bool allow_zero_length_clientid;
	نویسه *auto_id_prefix;
	uint16_t auto_id_prefix_len;
	ساختار plugin__callbacks plugin_callbacks;
	mosquitto_plugin_id_t *pid; /* For registering as a "plugin" */
};

#اگر defined(WITH_EPOLL) || defined(WITH_KQUEUE)
شمارش struct_ident {
	id_invalid = 0,
	id_listener = 1,
	id_client = 2,
	id_listener_ws = 3,
};
#endif

ساختار mosquitto__listener {
	uint16_t port;
	نویسه *host;
	نویسه *bind_interface;
	صحیح max_connections;
	نویسه *mount_point;
	mosq_sock_t *socks;
	صحیح sock_شمار;
	صحیح client_شمار;
	شمارش mosquitto_protocol protocol;
	صحیح socket_domain;
	bool use_username_as_clientid;
	uint8_t max_qos;
	uint16_t max_topic_alias;
	uint16_t max_topic_alias_broker;
#ifdef WITH_TLS
	نویسه *cafile;
	نویسه *capath;
	نویسه *certfile;
	نویسه *keyfile;
	نویسه *tls_engine;
	نویسه *tls_engine_kpass_sha1;
	نویسه *ciphers;
	نویسه *ciphers_tls13;
	نویسه *psk_hint;
	SSL_CTX *ssl_ctx;
	نویسه *crlfile;
	نویسه *tls_version;
	bool use_identity_as_username;
	bool use_subject_as_username;
	bool require_certificate;
	bool disable_client_cert_date_checks;
	شمارش mosquitto__keyform tls_keyform;
#endif
#ifdef WITH_WEBSOCKETS
#  اگر WITH_WEBSOCKETS == WS_IS_LWS
	ساختار lws_context *ws_context;
	bool ws_in_init;
	ساختار lws_protocols *ws_protocol;
#  endif
	نویسه **ws_origins;
	صحیح ws_origin_شمار;
#endif
#اگر defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
	نویسه *http_dir;
#endif
	ساختار mosquitto__security_options *security_options;
#ifdef WITH_UNIX_SOCKETS
	نویسه *unix_socket_path;
#endif
	bool disable_protocol_v3;
	bool disable_protocol_v4;
	bool disable_protocol_v5;
	صحیح enable_proxy_protocol;
	bool proxy_protocol_v2_require_tls;
#ifdef WITH_HTTP_API
	ساختار MHD_Daemon *mhd;
#endif
};


ساختار mosquitto__listener_sock {
#اگر defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	/* This *must* be the first element in the struct. */
	صحیح ident;
#endif
	mosq_sock_t sock;
	ساختار mosquitto__listener *listener;
};


/* Callbacks belonging to a specific plugin
 * Doesn't include MOSQ_EVT_CONTROL events.
 */
ساختار plugin_own_callback {
	ساختار plugin_own_callback *next, *prev;
	MOSQ_FUNC_generic_callback cb_func;
	شمارش mosquitto_plugin_event event;
};

ساختار mosquitto_plugin_id_t {
	ساختار mosquitto__plugin_config config;
	ساختار mosquitto__plugin_lib lib;
	ساختار mosquitto__listener *listener;
	نویسه *plugin_name;
	نویسه *plugin_version;
	ساختار control_endpoint *control_endpoints;
	ساختار plugin_own_callback *own_callbacks;
	ساختار timespec next_tick;
};

ساختار mosquitto__config {
	bool allow_duplicate_messages;
	صحیح autosave_interval;
	bool autosave_on_changes;
	bool check_retain_source;
	نویسه *clientid_prefixes;
	bool connection_messages;
	uint16_t cmd_port[CMD_PORT_LIMIT];
	صحیح cmd_port_شمار;
	bool daemon;
	bool test_configuration;
	bool enable_control_api;
	صحیح global_max_clients;
	صحیح global_max_connections;
	ساختار mosquitto__listener *default_listener; /* Points to one of `listeners` */
	ساختار mosquitto__listener *listeners;
	صحیح listener_شمار;
	bool local_only;
	unsigned صحیح log_dest;
	صحیح log_facility;
	unsigned صحیح log_type;
	bool log_timestamp;
	نویسه *log_timestamp_format;
	نویسه *log_file;
	FILE *log_fptr;
	size_t max_inflight_bytes;
	size_t max_queued_bytes;
	صحیح max_queued_messages;
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
	نویسه *persistence_location;
	نویسه *persistence_file;
	نویسه *persistence_filepath;
	time_t persistent_client_expiration;
	نویسه *pid_file;
	bool queue_qos0_messages;
	bool per_listener_settings;
	bool retain_available;
	صحیح retain_expiry_interval;
	bool set_tcp_nodelay;
	صحیح sys_interval;
	bool upgrade_outgoing_qos;
	نویسه *user;
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	صحیح websockets_log_level;
#endif
	uint16_t packet_buffer_size;
#ifdef WITH_BRIDGE
	ساختار mosquitto__bridge **bridges;
	صحیح bridge_شمار;
#endif
	ساختار mosquitto__security_options security_options;
};


ساختار mosquitto__subshared {
	UT_hash_handle hh;
	ساختار mosquitto__subleaf *subs;
	نویسه name[];
};

ساختار mosquitto__subhier {
	UT_hash_handle hh;
	ساختار mosquitto__subhier *parent;
	ساختار mosquitto__subhier *children;
	ساختار mosquitto__subleaf *subs;
	ساختار mosquitto__subshared *shared;
	uint16_t topic_len;
	نویسه topic[];
};

ساختار mosquitto__subleaf {
	ساختار mosquitto__subleaf *prev;
	ساختار mosquitto__subleaf *next;
	ساختار mosquitto *context;
	ساختار mosquitto__subhier *hier;
	ساختار mosquitto__subshared *shared;
	uint32_t identifier;
	uint8_t subscription_options;
	نویسه topic_filter[];
};

ساختار sub__token {
	ساختار sub__token *next;
	uint16_t topic_len;
	نویسه topic[];
};

ساختار mosquitto__retainhier {
	UT_hash_handle hh;
	ساختار mosquitto__retainhier *parent;
	ساختار mosquitto__retainhier *children;
	ساختار mosquitto_base_پیام *retained;
	uint16_t topic_len;
	نویسه topic[];
};

ساختار mosquitto_base_پیام {
	UT_hash_handle hh;
	ساختار mosquitto_base_پیام_2 data;
	ساختار mosquitto__listener *source_listener;
	نویسه **dest_ids;
	صحیح dest_id_شمار;
	صحیح ref_شمار;
	شمارش mosquitto_پیام_origin origin;
	bool stored;
};

ساختار mosquitto_client_پیام {
	ساختار mosquitto_client_پیام_2 data;
	ساختار mosquitto_client_پیام *prev;
	ساختار mosquitto_client_پیام *next;
	ساختار mosquitto_base_پیام *base_پیام;
};


ساختار mosquitto__psk {
	UT_hash_handle hh;
	نویسه *username;
	نویسه *password;
};

ساختار mosquitto__message_v5 {
	ساختار mosquitto__message_v5 *next, *prev;
	نویسه *topic;
	تهی *payload;
	mosquitto_property *properties;
	/* clientid is used only by mosquitto_broker_publish*() to indicate
	   this message is for a specific client. */
	نویسه *clientid;
	صحیح payloadlen;
	صحیح qos;
	bool retain;
};

ساختار mosquitto_db {
	dbid_t last_db_id;
	uint64_t گره_id_shifted;
	ساختار mosquitto__subhier *normal_subs;
	ساختار mosquitto__subhier *shared_subs;
	ساختار mosquitto__retainhier *retains;
	ساختار mosquitto *contexts_by_id;
	ساختار mosquitto *contexts_by_sock;
	ساختار mosquitto *contexts_by_id_delayed_auth;
	ساختار mosquitto *contexts_for_free;
	mosquitto_plugin_id_t **plugins;
	صحیح plugin_شمار;
#ifdef WITH_BRIDGE
	ساختار mosquitto **bridges;
	صحیح bridge_شمار;
#endif
	ساختار clientid__index_hash *clientid_index_hash;
	ساختار mosquitto_base_پیام *پیام_store;
	time_t now_s; /* Monotonic clock, where possible */
	time_t now_real_s; /* Read clock, for measuring session/message expiry */
	uint64_t گره_id; /* for unique db ids */
	time_t next_event_ms; /* for mux timeout */
	صحیح پیام_store_شمار;
	unsigned long پیام_store_bytes;
	نویسه *config_file;
	ساختار mosquitto__config *config;
	bool quiet;
	bool verbose;
#ifdef WITH_SYS_درخت
	صحیح subscription_شمار;
	صحیح shared_subscription_شمار;
	صحیح retained_شمار;
#endif
	صحیح persistence_changes;
	ساختار mosquitto *ll_for_free;
#ifdef WITH_EPOLL
	صحیح epollfd;
#endif
#ifdef WITH_KQUEUE
	صحیح kqueuefd;
#endif
	ساختار mosquitto__message_v5 *plugin_msgs;
#ifdef WITH_TLS
	/* tls_keylog can't be in the config struct because it is used
	   before the config is allocated. Config probably
	   shouldn't be separately allocated. */
	نویسه *tls_keylog;
#endif
	bool shutdown;
};

شمارش mosquitto__bridge_direction {
	bd_out = 0,
	bd_in = 1,
	bd_both = 2,
};

شمارش mosquitto_bridge_start_type {
	bst_automatic = 0,
	bst_lazy = 1,
	bst_manual = 2,
	bst_once = 3,
};

شمارش mosquitto_bridge_reload_type {
	brt_lazy = 0,
	brt_immediate = 1,
};

ساختار mosquitto__bridge_topic {
	ساختار mosquitto__bridge_topic *next;
	نویسه *topic;
	نویسه *local_prefix;
	نویسه *remote_prefix;
	نویسه *local_topic; /* topic prefixed with local_prefix */
	نویسه *remote_topic; /* topic prefixed with remote_prefix */
	شمارش mosquitto__bridge_direction direction;
	uint8_t qos;
};

ساختار bridge_address {
	نویسه *address;
	uint16_t port;
};

ساختار mosquitto__bridge {
	نویسه *name;
	ساختار bridge_address *addresses;
	صحیح cur_address;
	صحیح address_شمار;
	time_t primary_retry;
	mosq_sock_t primary_retry_sock;
	bool round_robin;
	bool try_private;
	bool try_private_accepted;
	bool clean_start;
	int8_t clean_start_local;
	uint16_t keepalive;
	unsigned صحیح tcp_keepalive_idle;
	unsigned صحیح tcp_keepalive_interval;
	unsigned صحیح tcp_keepalive_counter;
#ifdef WITH_TCP_USER_TIMEOUT
	صحیح tcp_user_timeout;
#endif
	ساختار mosquitto__bridge_topic *topics;
	صحیح topic_شمار;
	bool topic_remapping;
	شمارش mosquitto__protocol protocol_version;
	time_t restart_t;
	time_t connected_at;
	نویسه *remote_clientid;
	نویسه *remote_username;
	نویسه *remote_password;
	نویسه *local_clientid;
	نویسه *local_username;
	نویسه *local_password;
	نویسه *notification_topic;
	نویسه *bind_address;
	bool notifications;
	bool notifications_local_only;
	شمارش mosquitto_bridge_start_type start_type;
	صحیح idle_timeout;
	صحیح restart_timeout;
	صحیح backoff_base;
	صحیح backoff_cap;
	صحیح stable_connection_period;
	صحیح threshold;
	uint32_t maximum_packet_size;
	uint32_t session_expiry_interval;
	uint16_t receive_maximum;
	bool lazy_reconnect;
	bool attempt_unsubscribe;
	bool initial_notification_done;
	bool outgoing_retain;
	شمارش mosquitto_bridge_reload_type reload_type;
	uint16_t max_topic_alias;
#ifdef WITH_TLS
	bool tls_insecure;
	bool tls_ocsp_required;
	bool tls_use_os_certs;
	نویسه *tls_cafile;
	نویسه *tls_capath;
	نویسه *tls_certfile;
	نویسه *tls_keyfile;
	نویسه *tls_version;
	نویسه *tls_alpn;
	نویسه *tls_ciphers;
	نویسه *tls_13_ciphers;
#  ifdef FINAL_WITH_TLS_PSK
	نویسه *tls_psk_identity;
	نویسه *tls_psk;
#  endif
#endif
};

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
ساختار libws_mqtt_hack {
	نویسه *http_dir;
	ساختار mosquitto__listener *listener;
};

ساختار libws_mqtt_data {
	ساختار mosquitto *mosq;
};
#endif

#شامل <net_mosq.h>


ساختار control_endpoint {
	ساختار control_endpoint *next, *prev;
	نویسه topic[];
};

بیرونی ساختار mosquitto_db db;

/* ============================================================
 * Main functions
 * ============================================================ */
صحیح mosquitto_main_loop(ساختار mosquitto__listener_sock *listensock, صحیح listensock_شمار);
تهی loop__update_next_event(time_t new_ms);

/* ============================================================
 * Config functions
 * ============================================================ */
/* Initialise config struct to default values. */
تهی config__init(ساختار mosquitto__config *config);
#ifdef WITH_BRIDGE
تهی config__bridge_cleanup(ساختار mosquitto__bridge *bridge);
#endif
/* Parse command line options into config. */
صحیح config__parse_args(ساختار mosquitto__config *config, صحیح argc, نویسه *argv[]);
/* Read configuration data from config->config_file into config.
 * If reload is true, don't process config options that shouldn't be reloaded (listeners etc)
 * Returns 0 on success, 1 if there is a configuration error or if a file cannot be opened.
 */
صحیح config__read(ساختار mosquitto__config *config, bool reload);
/* Free all config data. */
تهی config__cleanup(ساختار mosquitto__config *config);
صحیح config__get_dir_files(ثابت نویسه *include_dir, نویسه ***files, صحیح *file_شمار);

/* ============================================================
 * Server send functions
 * ============================================================ */
صحیح send__connack(ساختار mosquitto *context, uint8_t ack, uint8_t reason_code, ثابت mosquitto_property *properties);
صحیح send__suback(ساختار mosquitto *context, uint16_t mid, uint32_t payloadlen, ثابت تهی *payload);
صحیح send__unsuback(ساختار mosquitto *context, uint16_t mid, صحیح reason_code_شمار, uint8_t *reason_codes, ثابت mosquitto_property *properties);
صحیح send__auth(ساختار mosquitto *context, uint8_t reason_code, ثابت تهی *auth_data, uint16_t auth_data_len);

/* ============================================================
 * Network functions
 * ============================================================ */
تهی net__broker_init(تهی);
تهی net__broker_cleanup(تهی);
ساختار mosquitto *net__socket_accept(ساختار mosquitto__listener_sock *listensock);
صحیح net__socket_listen(ساختار mosquitto__listener *listener);
صحیح net__socket_get_address(mosq_sock_t sock, نویسه *buf, size_t len, uint16_t *remote_address);
صحیح net__tls_load_verify(ساختار mosquitto__listener *listener);
صحیح net__tls_server_ctx(ساختار mosquitto__listener *listener);
صحیح net__load_certificates(ساختار mosquitto__listener *listener);

/* ============================================================
 * Read handling functions
 * ============================================================ */
صحیح handle__packet(ساختار mosquitto *context);
صحیح handle__connack(ساختار mosquitto *context);
صحیح handle__connect(ساختار mosquitto *context);
صحیح handle__disconnect(ساختار mosquitto *context);
صحیح handle__publish(ساختار mosquitto *context);
صحیح handle__subscribe(ساختار mosquitto *context);
صحیح handle__unsubscribe(ساختار mosquitto *context);
صحیح handle__auth(ساختار mosquitto *context);

/* ============================================================
 * Database handling
 * ============================================================ */
صحیح db__open(ساختار mosquitto__config *config);
صحیح db__close(تهی);
#ifdef WITH_PERSISTENCE
صحیح persist__backup(bool shutdown);
صحیح persist__restore(تهی);
#endif
/* Return the number of in-flight messages in count. */
صحیح db_message_شمار(صحیح *شمار);
صحیح db__message_delete_outgoing(ساختار mosquitto *context, uint16_t mid, شمارش mosquitto_پیام_state expect_state, صحیح qos);
صحیح db_message_درج_outgoing(ساختار mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, ساختار mosquitto_base_پیام *base_پیام, uint32_t subscription_identifier, bool update, bool persist);
صحیح db_message_درج_incoming(ساختار mosquitto *context, uint64_t cmsg_id, ساختار mosquitto_base_پیام *base_پیام, bool persist);
صحیح db__message_remove_incoming(ساختار mosquitto *context, uint16_t mid);
صحیح db__message_release_incoming(ساختار mosquitto *context, uint16_t mid);
صحیح db__message_update_outgoing(ساختار mosquitto *context, uint16_t mid, شمارش mosquitto_پیام_state state, صحیح qos, bool persist);
تهی db__message_dequeue_first(ساختار mosquitto *context, ساختار mosquitto_پیام_data *پیام_data);
صحیح db__messages_delete(ساختار mosquitto *context, bool force_free);
صحیح db__messages_delete_incoming(ساختار mosquitto *context);
صحیح db__messages_delete_outgoing(ساختار mosquitto *context);
صحیح db__messages_easy_queue(ساختار mosquitto *context, ثابت نویسه *topic, uint8_t qos, uint32_t payloadlen, ثابت تهی *payload, صحیح retain, uint32_t message_expiry_interval, mosquitto_property **properties);
صحیح db__message_store(ثابت ساختار mosquitto *source, ساختار mosquitto_base_پیام *base_پیام, uint32_t *message_expiry_interval, شمارش mosquitto_پیام_origin origin);
صحیح db__message_store_find(ساختار mosquitto *context, uint16_t mid, ساختار mosquitto_client_پیام **client_پیام);
صحیح db_پیام_store_add(ساختار mosquitto_base_پیام *base_پیام);
تهی db_پیام_store_remove(ساختار mosquitto_base_پیام *base_پیام, bool notify);
تهی db_پیام_store_ref_inc(ساختار mosquitto_base_پیام *base_پیام);
تهی db_پیام_store_ref_dec(ساختار mosquitto_base_پیام **base_پیام);
تهی db_پیام_store_clean(تهی);
تهی db_پیام_store_compact(تهی);
تهی db_پیام_store_free(ساختار mosquitto_base_پیام *base_پیام);
صحیح db__message_reconnect_reset(ساختار mosquitto *context);
bool db__ready_for_flight(ساختار mosquitto *context, شمارش mosquitto_پیام_direction dir, صحیح qos);
bool db__ready_for_queue(ساختار mosquitto *context, صحیح qos, ساختار mosquitto_پیام_data *پیام_data);
تهی sys_درخت_init(تهی);
تهی sys_درخت_update(bool force);
صحیح db__message_write_inflight_out_all(ساختار mosquitto *context);
صحیح db__message_write_inflight_out_latest(ساختار mosquitto *context);
صحیح db__message_write_queued_out(ساختار mosquitto *context);
صحیح db__message_write_queued_in(ساختار mosquitto *context);
تهی db_پیام_add_to_inflight_stats(ساختار mosquitto_پیام_data *پیام_data, ساختار mosquitto_client_پیام *پیام);
تهی db_پیام_add_to_queued_stats(ساختار mosquitto_پیام_data *پیام_data, ساختار mosquitto_client_پیام *پیام);
uint64_t db_new_پیام_id(تهی);
تهی db__expire_all_messages(ساختار mosquitto *context);
تهی db__check_acl_of_all_messages(ساختار mosquitto *context);

/* ============================================================
 * Subscription functions
 * ============================================================ */
صحیح sub__init(تهی);
صحیح sub__add(ساختار mosquitto *context, ثابت ساختار mosquitto_subscription *sub);
صحیح sub__remove(ساختار mosquitto *context, ثابت نویسه *sub, uint8_t *reason);
تهی sub_درخت_print(ساختار mosquitto__subhier *ریشه, صحیح level);
صحیح sub__clean_session(ساختار mosquitto *context);
صحیح sub__messages_queue(ثابت نویسه *source_id, uint8_t qos, ساختار mosquitto_base_پیام **base_پیام);
صحیح sub__topic_tokenise(ثابت نویسه *subtopic, نویسه **local_sub, نویسه ***topics, ثابت نویسه **sharename);
تهی sub__topic_tokens_free(ساختار sub__token *tokens);

/* ============================================================
 * Context functions
 * ============================================================ */
ساختار mosquitto *context__init(تهی);
صحیح context__init_sock(ساختار mosquitto *context, mosq_sock_t sock, bool get_address);
تهی context__cleanup(ساختار mosquitto *context, bool force_free);
تهی context__disconnect(ساختار mosquitto *context, صحیح reason);
تهی context__add_to_disused(ساختار mosquitto *context);
تهی context__free_disused(تهی);
تهی context__send_will(ساختار mosquitto *context);
تهی context__add_to_by_id(ساختار mosquitto *context);
تهی context__remove_from_by_id(ساختار mosquitto *context);

صحیح connect__on_authorised(ساختار mosquitto *context, تهی *auth_data_out, uint16_t auth_data_out_len);


/* ============================================================
 * Control functions
 * ============================================================ */
#ifdef WITH_CONTROL
صحیح control__process(ساختار mosquitto *context, ساختار mosquitto_base_پیام *base_پیام);
تهی control__cleanup(تهی);
#endif
صحیح control__register_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ثابت نویسه *topic, تهی *userdata);
صحیح control__unregister_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ثابت نویسه *topic);
تهی control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier);


/* ============================================================
 * Logging functions
 * ============================================================ */
صحیح log__init(ساختار mosquitto__config *config);
صحیح log__close(ساختار mosquitto__config *config);
تهی log__internal(ثابت نویسه *fmt, ...) __attribute__((format(printf, 1, 2)));

/* ============================================================
 * Bridge functions
 * ============================================================ */
#ifdef WITH_BRIDGE
تهی bridge__start_all(تهی);
تهی bridge__reload(تهی);
تهی bridge__db_cleanup(تهی);
تهی bridge__cleanup(ساختار mosquitto *context);
صحیح bridge__connect(ساختار mosquitto *context);
#اگر defined(__GLIBC__) && defined(WITH_ADNS)
صحیح bridge__connect_step3(ساختار mosquitto *context);
#endif
صحیح bridge__on_connect(ساختار mosquitto *context);
تهی bridge_check(تهی);
صحیح bridge__register_local_connections(تهی);
صحیح bridge__add_topic(ساختار mosquitto__bridge *bridge, ثابت نویسه *topic, شمارش mosquitto__bridge_direction direction, uint8_t qos, ثابت نویسه *local_prefix, ثابت نویسه *remote_prefix);
تهی bridge__cleanup_topics(ساختار mosquitto__bridge *bridge);
صحیح bridge__remap_topic_in(ساختار mosquitto *context, نویسه **topic);
#endif

/* ============================================================
 * IO multiplex related functions
 * ============================================================ */
صحیح mux__init(تهی);
صحیح mux__add_listeners(ساختار mosquitto__listener_sock *listensock, صحیح listensock_شمار);
صحیح mux__delete_listeners(ساختار mosquitto__listener_sock *listensock, صحیح listensock_شمار);
صحیح mux__loop_prepare(تهی);
صحیح mux__new(ساختار mosquitto *context);
صحیح mux__add_out(ساختار mosquitto *context);
صحیح mux__remove_out(ساختار mosquitto *context);
صحیح mux__delete(ساختار mosquitto *context);
صحیح mux__wait(تهی);
صحیح mux__handle(ساختار mosquitto__listener_sock *listensock, صحیح listensock_شمار);
صحیح mux__cleanup(تهی);

/* ============================================================
 * Listener related functions
 * ============================================================ */
بیرونی ساختار mosquitto__listener_sock *g_listensock;
بیرونی صحیح g_listensock_شمار;

تهی listener__set_defaults(ساختار mosquitto__listener *listener);
تهی listeners__reload_all_certificates(تهی);
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
تهی listeners__add_websockets(ساختار lws_context *ws_context, mosq_sock_t fd);
#endif
صحیح listeners__start(تهی);
تهی listeners__stop(تهی);

/* ============================================================
 * Plugin related functions
 * ============================================================ */
صحیح plugin__load_v5(mosquitto_plugin_id_t *plugin, تهی *lib);
صحیح plugin__load_v4(mosquitto_plugin_id_t *plugin, تهی *lib);
صحیح plugin__load_v3(mosquitto_plugin_id_t *plugin, تهی *lib);
صحیح plugin__load_v2(mosquitto_plugin_id_t *plugin, تهی *lib);
صحیح acl__pre_check(mosquitto_plugin_id_t *plugin, ساختار mosquitto *context, صحیح access);

تهی LIB_ERROR(تهی);
تهی plugin__handle_connect(ساختار mosquitto *context);
تهی plugin__handle_disconnect(ساختار mosquitto *context, صحیح reason);
تهی plugin__handle_client_offline(ساختار mosquitto *context, صحیح reason);
صحیح plugin__handle_message_in(ساختار mosquitto *context, ساختار mosquitto_base_پیام_2 *base_پیام);
صحیح plugin__handle_message_out(ساختار mosquitto *context, ساختار mosquitto_base_پیام_2 *base_پیام);
صحیح plugin__handle_subscribe(ساختار mosquitto *context, ساختار mosquitto_subscription *sub);
صحیح plugin__handle_unsubscribe(ساختار mosquitto *context, ساختار mosquitto_subscription *sub);
صحیح plugin__handle_reload(تهی);
تهی plugin__handle_tick(تهی);
صحیح plugin__callback_unregister_all(mosquitto_plugin_id_t *identifier);
تهی plugin_persist__handle_restore(تهی);
تهی plugin_persist__handle_client_add(ساختار mosquitto *context);
تهی plugin_persist__handle_client_delete(ساختار mosquitto *context);
تهی plugin_persist__handle_client_update(ساختار mosquitto *context);
تهی plugin_persist__handle_subscription_add(ساختار mosquitto *context, ثابت ساختار mosquitto_subscription *sub);
تهی plugin_persist__handle_subscription_delete(ساختار mosquitto *context, نویسه *sub);
تهی plugin_persist_handle_client_پیام_add(ساختار mosquitto *context, ثابت ساختار mosquitto_client_پیام *cmsg);
تهی plugin_persist_handle_client_پیام_delete(ساختار mosquitto *context, ثابت ساختار mosquitto_client_پیام *cmsg);
تهی plugin_persist_handle_client_پیام_update(ساختار mosquitto *context, ثابت ساختار mosquitto_client_پیام *cmsg);
تهی plugin_persist_handle_base_پیام_add(ساختار mosquitto_base_پیام *base_پیام);
تهی plugin_persist_handle_base_پیام_delete(ساختار mosquitto_base_پیام *base_پیام);
تهی plugin_persist_handle_retain_پیام_set(ساختار mosquitto_base_پیام *base_پیام);
تهی plugin_persist_handle_retain_پیام_delete(ساختار mosquitto_base_پیام *base_پیام);
تهی plugin_persist__handle_will_add(ساختار mosquitto *context);
تهی plugin_persist__handle_will_delete(ساختار mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
صحیح keepalive__init(تهی);
تهی keepalive__cleanup(تهی);
صحیح keepalive__add(ساختار mosquitto *context);
تهی keepalive__check(تهی);
صحیح keepalive__remove(ساختار mosquitto *context);
صحیح keepalive__update(ساختار mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
صحیح property__process_connect(ساختار mosquitto *context, mosquitto_property **props);
صحیح property__process_will(ساختار mosquitto *context, ساختار mosquitto_message_all *پیام, mosquitto_property **props);
صحیح property__process_publish(ساختار mosquitto_base_پیام *base_پیام, mosquitto_property **props, صحیح *topic_alias, uint32_t *message_expiry_interval, bool is_bridge);
صحیح property__process_disconnect(ساختار mosquitto *context, mosquitto_property **props);

/* ============================================================
 * Retain tree related functions
 * ============================================================ */
صحیح retain__init(تهی);
تهی retain__clean(ساختار mosquitto__retainhier **retainhier);
صحیح retain__queue(ساختار mosquitto *context, ثابت ساختار mosquitto_subscription *sub);
صحیح retain__store(ثابت نویسه *topic, ساختار mosquitto_base_پیام *base_پیام, نویسه **split_topics, bool persist);
تهی retain__expiry_check(تهی);
تهی retain__expire(ساختار mosquitto__retainhier **retainhier);

/* ============================================================
 * Security related functions
 * ============================================================ */
صحیح acl__find_acls(ساختار mosquitto *context);
صحیح plugin__load_all(تهی);
صحیح plugin__unload_all(تهی);
صحیح config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, ساختار mosquitto__security_options *security_options);

صحیح mosquitto_security_init(bool reload);
صحیح mosquitto_security_cleanup(bool reload);
صحیح mosquitto_acl_check(ساختار mosquitto *context, ثابت نویسه *topic, uint32_t payloadlen, تهی *payload, uint8_t qos, bool retain, mosquitto_property *properties, صحیح access);
صحیح mosquitto_basic_auth(ساختار mosquitto *context);
صحیح mosquitto_psk_کلید_get(ساختار mosquitto *context, ثابت نویسه *hint, ثابت نویسه *identity, نویسه *کلید, صحیح max_کلید_len);

صحیح mosquitto_security_init_default(تهی);
صحیح mosquitto_security_apply_default(تهی);
صحیح mosquitto_security_cleanup_default(تهی);
صحیح mosquitto_psk_کلید_get_default(ساختار mosquitto *context, ثابت نویسه *hint, ثابت نویسه *identity, نویسه *کلید, صحیح max_کلید_len);
صحیح broker_acl_file__init(تهی);
تهی broker_acl_file__cleanup(تهی);
صحیح broker_password_file__init(تهی);
تهی broker_password_file__cleanup(تهی);
صحیح psk_file__init(تهی);
صحیح psk_file__cleanup(تهی);

صحیح mosquitto_security_auth_start(ساختار mosquitto *context, bool reauth, ثابت تهی *data_in, uint16_t data_in_len, تهی **data_out, uint16_t *data_out_len);
صحیح mosquitto_security_auth_continue(ساختار mosquitto *context, ثابت تهی *data_in, uint16_t data_len, تهی **data_out, uint16_t *data_out_len);

تهی unpwd__free_item(ساختار mosquitto__unpwd **unpwd, ساختار mosquitto__unpwd *item);

/* ============================================================
 * Session expiry
 * ============================================================ */
صحیح session_expiry__add(ساختار mosquitto *context);
صحیح session_expiry__add_from_persistence(ساختار mosquitto *context, time_t expiry_time);
تهی session_expiry__add_on_shutdown(تهی);
تهی session_expiry__remove(ساختار mosquitto *context);
تهی session_expiry__remove_all(تهی);
تهی session_expiry__check(تهی);
تهی session_expiry__send_all(تهی);

/* ============================================================
 * Signals
 * ============================================================ */
تهی signal__setup(تهی);
صحیح signal__flag_check(تهی);

/* ============================================================
 * Window service and signal related functions
 * ============================================================ */
#اگر defined(WIN32) || defined(__CYGWIN__)
تهی service_install(نویسه *name);
تهی service_uninstall(نویسه *name);
تهی service_run(نویسه *name);

#ifdef WIN32
DWORD WINAPI SigThreadProc(تهی *data);
#endif
#endif

/* ============================================================
 * Watchdog
 * ============================================================ */
تهی watchdog__init(تهی);
تهی watchdog__check(تهی);

/* ============================================================
 * Websockets related functions
 * ============================================================ */
#ifdef WITH_WEBSOCKETS
#  اگر WITH_WEBSOCKETS == WS_IS_LWS
تهی mosq_websockets_init(ساختار mosquitto__listener *listener, ثابت ساختار mosquitto__config *conf);
#  endif
صحیح http__context_init(ساختار mosquitto *context);
صحیح http__context_cleanup(ساختار mosquitto *context);
صحیح http__read(ساختار mosquitto *context);
#endif
تهی do_disconnect(ساختار mosquitto *context, صحیح reason);

/* ============================================================
 * PROXY related functions
 * ============================================================ */
صحیح proxy_v2__read(ساختار mosquitto *context);
صحیح proxy_v1__read(ساختار mosquitto *context);

/* ============================================================
 * Will delay
 * ============================================================ */
صحیح will_delay__add(ساختار mosquitto *context);
تهی will_delay__check(تهی);
تهی will_delay__send_all(تهی);
تهی will_delay__remove(ساختار mosquitto *mosq);

/* ============================================================
 * HTTP Info
 * ============================================================ */
صحیح http_api__start_local(ساختار mosquitto__listener *listener);
صحیح http_api__start(ساختار mosquitto__listener *listener);
تهی http_api__stop(ساختار mosquitto__listener *listener);


/* ============================================================
 * Other
 * ============================================================ */
#ifdef WITH_XTREPORT
تهی xtreport(تهی);
#endif

تهی broker_control__init(تهی);
تهی broker_control__cleanup(تهی);
تهی broker_control__reload(تهی);

#endif
