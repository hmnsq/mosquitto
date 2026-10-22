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

#include "config.h"
#include <stdio.h>

#ако defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#  ако LWS_LIBRARY_VERSION_NUMBER >= 3002000 && !defined(LWS_WITH_EXTERNAL_POLL)
#    warning "libwebsockets is not compiled with LWS_WITH_EXTERNAL_POLL support. Websocket performance will be unusable."
#  endif
#endif

#ifdef WITH_HTTP_API
#  include <microhttpd.h>
#endif

#ifdef __linux__
#define WITH_TCP_USER_TIMEOUT
#endif

#include "mosquitto_internal.h"
#include "mosquitto/broker.h"
#include "mosquitto/broker_plugin.h"
#include "mosquitto.h"
#include "logging_mosq.h"
#include "tls_mosq.h"
#include "uthash.h"
#include "acl_file.h"
#include "password_file.h"

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

typedef uint64_t dbid_t;

typedef цяло (*FUNC_plugin_init_v5)(mosquitto_plugin_id_t *, void **, struct mosquitto_opt *, цяло);
typedef цяло (*FUNC_plugin_cleanup_v5)(void *, struct mosquitto_opt *, цяло);

typedef цяло (*FUNC_auth_plugin_init_v4)(void **, struct mosquitto_opt *, цяло);
typedef цяло (*FUNC_auth_plugin_cleanup_v4)(void *, struct mosquitto_opt *, цяло);
typedef цяло (*FUNC_auth_plugin_security_init_v4)(void *, struct mosquitto_opt *, цяло, bool);
typedef цяло (*FUNC_auth_plugin_security_cleanup_v4)(void *, struct mosquitto_opt *, цяло, bool);
typedef цяло (*FUNC_auth_plugin_acl_check_v4)(void *, цяло, struct mosquitto *, struct mosquitto_acl_msg *);
typedef цяло (*FUNC_auth_plugin_unpwd_check_v4)(void *, struct mosquitto *, const char *, const char *);
typedef цяло (*FUNC_auth_plugin_psk_key_get_v4)(void *, struct mosquitto *, const char *, const char *, char *, цяло);
typedef цяло (*FUNC_auth_plugin_auth_start_v4)(void *, struct mosquitto *, const char *, bool, const void *, uint16_t, void **, uint16_t *);
typedef цяло (*FUNC_auth_plugin_auth_continue_v4)(void *, struct mosquitto *, const char *, const void *, uint16_t, void **, uint16_t *);

typedef цяло (*FUNC_auth_plugin_init_v3)(void **, struct mosquitto_opt *, цяло);
typedef цяло (*FUNC_auth_plugin_cleanup_v3)(void *, struct mosquitto_opt *, цяло);
typedef цяло (*FUNC_auth_plugin_security_init_v3)(void *, struct mosquitto_opt *, цяло, bool);
typedef цяло (*FUNC_auth_plugin_security_cleanup_v3)(void *, struct mosquitto_opt *, цяло, bool);
typedef цяло (*FUNC_auth_plugin_acl_check_v3)(void *, цяло, const struct mosquitto *, struct mosquitto_acl_msg *);
typedef цяло (*FUNC_auth_plugin_unpwd_check_v3)(void *, const struct mosquitto *, const char *, const char *);
typedef цяло (*FUNC_auth_plugin_psk_key_get_v3)(void *, const struct mosquitto *, const char *, const char *, char *, цяло);

typedef цяло (*FUNC_auth_plugin_init_v2)(void **, struct mosquitto_auth_opt *, цяло);
typedef цяло (*FUNC_auth_plugin_cleanup_v2)(void *, struct mosquitto_auth_opt *, цяло);
typedef цяло (*FUNC_auth_plugin_security_init_v2)(void *, struct mosquitto_auth_opt *, цяло, bool);
typedef цяло (*FUNC_auth_plugin_security_cleanup_v2)(void *, struct mosquitto_auth_opt *, цяло, bool);
typedef цяло (*FUNC_auth_plugin_acl_check_v2)(void *, const char *, const char *, const char *, цяло);
typedef цяло (*FUNC_auth_plugin_unpwd_check_v2)(void *, const char *, const char *);
typedef цяло (*FUNC_auth_plugin_psk_key_get_v2)(void *, const char *, const char *, char *, цяло);


enum mosquitto_msg_origin {
	mosq_mo_client = 0,
	mosq_mo_broker = 1,
};

struct mosquitto__plugin_lib {
	void *lib;
	void *user_data;
	цяло (*plugin_version)(void);
	struct mosquitto_plugin_id_t *identifier;

	FUNC_plugin_init_v5 plugin_init_v5;
	FUNC_plugin_cleanup_v5 plugin_cleanup_v5;

	FUNC_auth_plugin_init_v4 plugin_init_v4;
	FUNC_auth_plugin_cleanup_v4 plugin_cleanup_v4;
	FUNC_auth_plugin_security_init_v4 security_init_v4;
	FUNC_auth_plugin_security_cleanup_v4 security_cleanup_v4;
	FUNC_auth_plugin_acl_check_v4 acl_check_v4;
	FUNC_auth_plugin_unpwd_check_v4 unpwd_check_v4;
	FUNC_auth_plugin_psk_key_get_v4 psk_key_get_v4;
	FUNC_auth_plugin_auth_start_v4 auth_start_v4;
	FUNC_auth_plugin_auth_continue_v4 auth_continue_v4;

	FUNC_auth_plugin_init_v3 plugin_init_v3;
	FUNC_auth_plugin_cleanup_v3 plugin_cleanup_v3;
	FUNC_auth_plugin_security_init_v3 security_init_v3;
	FUNC_auth_plugin_security_cleanup_v3 security_cleanup_v3;
	FUNC_auth_plugin_acl_check_v3 acl_check_v3;
	FUNC_auth_plugin_unpwd_check_v3 unpwd_check_v3;
	FUNC_auth_plugin_psk_key_get_v3 psk_key_get_v3;

	FUNC_auth_plugin_init_v2 plugin_init_v2;
	FUNC_auth_plugin_cleanup_v2 plugin_cleanup_v2;
	FUNC_auth_plugin_security_init_v2 security_init_v2;
	FUNC_auth_plugin_security_cleanup_v2 security_cleanup_v2;
	FUNC_auth_plugin_acl_check_v2 acl_check_v2;
	FUNC_auth_plugin_unpwd_check_v2 unpwd_check_v2;
	FUNC_auth_plugin_psk_key_get_v2 psk_key_get_v2;
	цяло version;
};

struct mosquitto__plugin_config {
	char *path;
	char *name;
	struct mosquitto_opt *options;
	struct mosquitto__security_options **security_options;
	цяло option_count;
	цяло security_option_count;
	bool deny_special_chars;
};

struct mosquitto__callback {
	UT_hash_handle hh; /* For callbacks that register for e.g. a specific topic */
	struct mosquitto__callback *next, *prev; /* For typical callbacks */
	MOSQ_FUNC_generic_callback cb;
	void *userdata;
	union {
		char *topic;
		struct timespec next_tick;
	} data;
	mosquitto_plugin_id_t *identifier;
};

struct plugin__callbacks {
	struct mosquitto__callback *tick;
	struct mosquitto__callback *acl_check;
	struct mosquitto__callback *basic_auth;
	struct mosquitto__callback *connect;
	struct mosquitto__callback *control;
	struct mosquitto__callback *disconnect;
	struct mosquitto__callback *client_offline;
	struct mosquitto__callback *ext_auth_continue;
	struct mosquitto__callback *ext_auth_start;
	struct mosquitto__callback *message_in;
	struct mosquitto__callback *message_out;
	struct mosquitto__callback *psk_key;
	struct mosquitto__callback *reload;
	struct mosquitto__callback *subscribe;
	struct mosquitto__callback *unsubscribe;
	struct mosquitto__callback *persist_restore;
	struct mosquitto__callback *persist_client_add;
	struct mosquitto__callback *persist_client_delete;
	struct mosquitto__callback *persist_client_update;
	struct mosquitto__callback *persist_subscription_add;
	struct mosquitto__callback *persist_subscription_delete;
	struct mosquitto__callback *persist_client_msg_add;
	struct mosquitto__callback *persist_client_msg_delete;
	struct mosquitto__callback *persist_client_msg_update;
	struct mosquitto__callback *persist_base_msg_add;
	struct mosquitto__callback *persist_base_msg_delete;
	struct mosquitto__callback *persist_retain_msg_set;
	struct mosquitto__callback *persist_retain_msg_delete;
	struct mosquitto__callback *persist_will_add;
	struct mosquitto__callback *persist_will_delete;
};

/* This is owned by mosquitto__config or mosquitto__listener, and only referred
 * to by other structs */
struct mosquitto__security_options {
	/* Any options that get added here also need considering
	 * in config__read() with regards whether allow_anonymous
	 * should be disabled when these options are set.
	 */
	struct mosquitto__unpwd *unpwd;
	struct mosquitto__psk *psk_id;
	struct acl_file_data acl_data;
	struct password_file_data password_data;
	char *psk_file;
	mosquitto_plugin_id_t **plugins;
	цяло plugin_count;
	int8_t allow_anonymous;
	bool allow_zero_length_clientid;
	char *auto_id_prefix;
	uint16_t auto_id_prefix_len;
	struct plugin__callbacks plugin_callbacks;
	mosquitto_plugin_id_t *pid; /* For registering as a "plugin" */
};

#ако defined(WITH_EPOLL) || defined(WITH_KQUEUE)
enum struct_ident {
	id_invalid = 0,
	id_listener = 1,
	id_client = 2,
	id_listener_ws = 3,
};
#endif

struct mosquitto__listener {
	uint16_t port;
	char *host;
	char *bind_interface;
	цяло max_connections;
	char *mount_point;
	mosq_sock_t *socks;
	цяло sock_count;
	цяло client_count;
	enum mosquitto_protocol protocol;
	цяло socket_domain;
	bool use_username_as_clientid;
	uint8_t max_qos;
	uint16_t max_topic_alias;
	uint16_t max_topic_alias_broker;
#ifdef WITH_TLS
	char *cafile;
	char *capath;
	char *certfile;
	char *keyfile;
	char *tls_engine;
	char *tls_engine_kpass_sha1;
	char *ciphers;
	char *ciphers_tls13;
	char *psk_hint;
	SSL_CTX *ssl_ctx;
	char *crlfile;
	char *tls_version;
	bool use_identity_as_username;
	bool use_subject_as_username;
	bool require_certificate;
	bool disable_client_cert_date_checks;
	enum mosquitto__keyform tls_keyform;
#endif
#ifdef WITH_WEBSOCKETS
#  ако WITH_WEBSOCKETS == WS_IS_LWS
	struct lws_context *ws_context;
	bool ws_in_init;
	struct lws_protocols *ws_protocol;
#  endif
	char **ws_origins;
	цяло ws_origin_count;
#endif
#ако defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
	char *http_dir;
#endif
	struct mosquitto__security_options *security_options;
#ifdef WITH_UNIX_SOCKETS
	char *unix_socket_path;
#endif
	bool disable_protocol_v3;
	bool disable_protocol_v4;
	bool disable_protocol_v5;
	цяло enable_proxy_protocol;
	bool proxy_protocol_v2_require_tls;
#ifdef WITH_HTTP_API
	struct MHD_Daemon *mhd;
#endif
};


struct mosquitto__listener_sock {
#ако defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	/* This *must* be the first element in the struct. */
	цяло ident;
#endif
	mosq_sock_t sock;
	struct mosquitto__listener *listener;
};


/* Callbacks belonging to a specific plugin
 * Doesn't include MOSQ_EVT_CONTROL events.
 */
struct plugin_own_callback {
	struct plugin_own_callback *next, *prev;
	MOSQ_FUNC_generic_callback cb_func;
	enum mosquitto_plugin_event event;
};

struct mosquitto_plugin_id_t {
	struct mosquitto__plugin_config config;
	struct mosquitto__plugin_lib lib;
	struct mosquitto__listener *listener;
	char *plugin_name;
	char *plugin_version;
	struct control_endpoint *control_endpoints;
	struct plugin_own_callback *own_callbacks;
	struct timespec next_tick;
};

struct mosquitto__config {
	bool allow_duplicate_messages;
	цяло autosave_interval;
	bool autosave_on_changes;
	bool check_retain_source;
	char *clientid_prefixes;
	bool connection_messages;
	uint16_t cmd_port[CMD_PORT_LIMIT];
	цяло cmd_port_count;
	bool daemon;
	bool test_configuration;
	bool enable_control_api;
	цяло global_max_clients;
	цяло global_max_connections;
	struct mosquitto__listener *default_listener; /* Points to one of `listeners` */
	struct mosquitto__listener *listeners;
	цяло listener_count;
	bool local_only;
	unsigned цяло log_dest;
	цяло log_facility;
	unsigned цяло log_type;
	bool log_timestamp;
	char *log_timestamp_format;
	char *log_file;
	FILE *log_fptr;
	size_t max_inflight_bytes;
	size_t max_queued_bytes;
	цяло max_queued_messages;
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
	char *persistence_location;
	char *persistence_file;
	char *persistence_filepath;
	time_t persistent_client_expiration;
	char *pid_file;
	bool queue_qos0_messages;
	bool per_listener_settings;
	bool retain_available;
	цяло retain_expiry_interval;
	bool set_tcp_nodelay;
	цяло sys_interval;
	bool upgrade_outgoing_qos;
	char *user;
#ако defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	цяло websockets_log_level;
#endif
	uint16_t packet_buffer_size;
#ifdef WITH_BRIDGE
	struct mosquitto__bridge **bridges;
	цяло bridge_count;
#endif
	struct mosquitto__security_options security_options;
};


struct mosquitto__subshared {
	UT_hash_handle hh;
	struct mosquitto__subleaf *subs;
	char name[];
};

struct mosquitto__subhier {
	UT_hash_handle hh;
	struct mosquitto__subhier *parent;
	struct mosquitto__subhier *children;
	struct mosquitto__subleaf *subs;
	struct mosquitto__subshared *shared;
	uint16_t topic_len;
	char topic[];
};

struct mosquitto__subleaf {
	struct mosquitto__subleaf *prev;
	struct mosquitto__subleaf *next;
	struct mosquitto *context;
	struct mosquitto__subhier *hier;
	struct mosquitto__subshared *shared;
	uint32_t identifier;
	uint8_t subscription_options;
	char topic_filter[];
};

struct sub__token {
	struct sub__token *next;
	uint16_t topic_len;
	char topic[];
};

struct mosquitto__retainhier {
	UT_hash_handle hh;
	struct mosquitto__retainhier *parent;
	struct mosquitto__retainhier *children;
	struct mosquitto__base_msg *retained;
	uint16_t topic_len;
	char topic[];
};

struct mosquitto__base_msg {
	UT_hash_handle hh;
	struct mosquitto_base_msg data;
	struct mosquitto__listener *source_listener;
	char **dest_ids;
	цяло dest_id_count;
	цяло ref_count;
	enum mosquitto_msg_origin origin;
	bool stored;
};

struct mosquitto__client_msg {
	struct mosquitto_client_msg data;
	struct mosquitto__client_msg *prev;
	struct mosquitto__client_msg *next;
	struct mosquitto__base_msg *base_msg;
};


struct mosquitto__psk {
	UT_hash_handle hh;
	char *username;
	char *password;
};

struct mosquitto__message_v5 {
	struct mosquitto__message_v5 *next, *prev;
	char *topic;
	void *payload;
	mosquitto_property *properties;
	/* clientid is used only by mosquitto_broker_publish*() to indicate
	   this message is for a specific client. */
	char *clientid;
	цяло payloadlen;
	цяло qos;
	bool retain;
};

struct mosquitto_db {
	dbid_t last_db_id;
	uint64_t node_id_shifted;
	struct mosquitto__subhier *normal_subs;
	struct mosquitto__subhier *shared_subs;
	struct mosquitto__retainhier *retains;
	struct mosquitto *contexts_by_id;
	struct mosquitto *contexts_by_sock;
	struct mosquitto *contexts_by_id_delayed_auth;
	struct mosquitto *contexts_for_free;
	mosquitto_plugin_id_t **plugins;
	цяло plugin_count;
#ifdef WITH_BRIDGE
	struct mosquitto **bridges;
	цяло bridge_count;
#endif
	struct clientid__index_hash *clientid_index_hash;
	struct mosquitto__base_msg *msg_store;
	time_t now_s; /* Monotonic clock, where possible */
	time_t now_real_s; /* Read clock, for measuring session/message expiry */
	uint64_t node_id; /* for unique db ids */
	time_t next_event_ms; /* for mux timeout */
	цяло msg_store_count;
	unsigned long msg_store_bytes;
	char *config_file;
	struct mosquitto__config *config;
	bool quiet;
	bool verbose;
#ifdef WITH_SYS_TREE
	цяло subscription_count;
	цяло shared_subscription_count;
	цяло retained_count;
#endif
	цяло persistence_changes;
	struct mosquitto *ll_for_free;
#ifdef WITH_EPOLL
	цяло epollfd;
#endif
#ifdef WITH_KQUEUE
	цяло kqueuefd;
#endif
	struct mosquitto__message_v5 *plugin_msgs;
#ifdef WITH_TLS
	/* tls_keylog can't be in the config struct because it is used
	   before the config is allocated. Config probably
	   shouldn't be separately allocated. */
	char *tls_keylog;
#endif
	bool shutdown;
};

enum mosquitto__bridge_direction {
	bd_out = 0,
	bd_in = 1,
	bd_both = 2,
};

enum mosquitto_bridge_start_type {
	bst_automatic = 0,
	bst_lazy = 1,
	bst_manual = 2,
	bst_once = 3,
};

enum mosquitto_bridge_reload_type {
	brt_lazy = 0,
	brt_immediate = 1,
};

struct mosquitto__bridge_topic {
	struct mosquitto__bridge_topic *next;
	char *topic;
	char *local_prefix;
	char *remote_prefix;
	char *local_topic; /* topic prefixed with local_prefix */
	char *remote_topic; /* topic prefixed with remote_prefix */
	enum mosquitto__bridge_direction direction;
	uint8_t qos;
};

struct bridge_address {
	char *address;
	uint16_t port;
};

struct mosquitto__bridge {
	char *name;
	struct bridge_address *addresses;
	цяло cur_address;
	цяло address_count;
	time_t primary_retry;
	mosq_sock_t primary_retry_sock;
	bool round_robin;
	bool try_private;
	bool try_private_accepted;
	bool clean_start;
	int8_t clean_start_local;
	uint16_t keepalive;
	unsigned цяло tcp_keepalive_idle;
	unsigned цяло tcp_keepalive_interval;
	unsigned цяло tcp_keepalive_counter;
#ifdef WITH_TCP_USER_TIMEOUT
	цяло tcp_user_timeout;
#endif
	struct mosquitto__bridge_topic *topics;
	цяло topic_count;
	bool topic_remapping;
	enum mosquitto__protocol protocol_version;
	time_t restart_t;
	time_t connected_at;
	char *remote_clientid;
	char *remote_username;
	char *remote_password;
	char *local_clientid;
	char *local_username;
	char *local_password;
	char *notification_topic;
	char *bind_address;
	bool notifications;
	bool notifications_local_only;
	enum mosquitto_bridge_start_type start_type;
	цяло idle_timeout;
	цяло restart_timeout;
	цяло backoff_base;
	цяло backoff_cap;
	цяло stable_connection_period;
	цяло threshold;
	uint32_t maximum_packet_size;
	uint32_t session_expiry_interval;
	uint16_t receive_maximum;
	bool lazy_reconnect;
	bool attempt_unsubscribe;
	bool initial_notification_done;
	bool outgoing_retain;
	enum mosquitto_bridge_reload_type reload_type;
	uint16_t max_topic_alias;
#ifdef WITH_TLS
	bool tls_insecure;
	bool tls_ocsp_required;
	bool tls_use_os_certs;
	char *tls_cafile;
	char *tls_capath;
	char *tls_certfile;
	char *tls_keyfile;
	char *tls_version;
	char *tls_alpn;
	char *tls_ciphers;
	char *tls_13_ciphers;
#  ifdef FINAL_WITH_TLS_PSK
	char *tls_psk_identity;
	char *tls_psk;
#  endif
#endif
};

#ако defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
struct libws_mqtt_hack {
	char *http_dir;
	struct mosquitto__listener *listener;
};

struct libws_mqtt_data {
	struct mosquitto *mosq;
};
#endif

#include <net_mosq.h>


struct control_endpoint {
	struct control_endpoint *next, *prev;
	char topic[];
};

extern struct mosquitto_db db;

/* ============================================================
 * Main functions
 * ============================================================ */
цяло mosquitto_main_loop(struct mosquitto__listener_sock *listensock, цяло listensock_count);
void loop__update_next_event(time_t new_ms);

/* ============================================================
 * Config functions
 * ============================================================ */
/* Initialise config struct to default values. */
void config__init(struct mosquitto__config *config);
#ifdef WITH_BRIDGE
void config__bridge_cleanup(struct mosquitto__bridge *bridge);
#endif
/* Parse command line options into config. */
цяло config__parse_args(struct mosquitto__config *config, цяло argc, char *argv[]);
/* Read configuration data from config->config_file into config.
 * If reload is true, don't process config options that shouldn't be reloaded (listeners etc)
 * Returns 0 on success, 1 if there is a configuration error or if a file cannot be opened.
 */
цяло config__read(struct mosquitto__config *config, bool reload);
/* Free all config data. */
void config__cleanup(struct mosquitto__config *config);
цяло config__get_dir_files(const char *include_dir, char ***files, цяло *file_count);

/* ============================================================
 * Server send functions
 * ============================================================ */
цяло send__connack(struct mosquitto *context, uint8_t ack, uint8_t reason_code, const mosquitto_property *properties);
цяло send__suback(struct mosquitto *context, uint16_t mid, uint32_t payloadlen, const void *payload);
цяло send__unsuback(struct mosquitto *context, uint16_t mid, цяло reason_code_count, uint8_t *reason_codes, const mosquitto_property *properties);
цяло send__auth(struct mosquitto *context, uint8_t reason_code, const void *auth_data, uint16_t auth_data_len);

/* ============================================================
 * Network functions
 * ============================================================ */
void net__broker_init(void);
void net__broker_cleanup(void);
struct mosquitto *net__socket_accept(struct mosquitto__listener_sock *listensock);
цяло net__socket_listen(struct mosquitto__listener *listener);
цяло net__socket_get_address(mosq_sock_t sock, char *buf, size_t len, uint16_t *remote_address);
цяло net__tls_load_verify(struct mosquitto__listener *listener);
цяло net__tls_server_ctx(struct mosquitto__listener *listener);
цяло net__load_certificates(struct mosquitto__listener *listener);

/* ============================================================
 * Read handling functions
 * ============================================================ */
цяло handle__packet(struct mosquitto *context);
цяло handle__connack(struct mosquitto *context);
цяло handle__connect(struct mosquitto *context);
цяло handle__disconnect(struct mosquitto *context);
цяло handle__publish(struct mosquitto *context);
цяло handle__subscribe(struct mosquitto *context);
цяло handle__unsubscribe(struct mosquitto *context);
цяло handle__auth(struct mosquitto *context);

/* ============================================================
 * Database handling
 * ============================================================ */
цяло db__open(struct mosquitto__config *config);
цяло db__close(void);
#ifdef WITH_PERSISTENCE
цяло persist__backup(bool shutdown);
цяло persist__restore(void);
#endif
/* Return the number of in-flight messages in count. */
цяло db__message_count(цяло *count);
цяло db__message_delete_outgoing(struct mosquitto *context, uint16_t mid, enum mosquitto_msg_state expect_state, цяло qos);
цяло db__message_insert_outgoing(struct mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, struct mosquitto__base_msg *base_msg, uint32_t subscription_identifier, bool update, bool persist);
цяло db__message_insert_incoming(struct mosquitto *context, uint64_t cmsg_id, struct mosquitto__base_msg *base_msg, bool persist);
цяло db__message_remove_incoming(struct mosquitto *context, uint16_t mid);
цяло db__message_release_incoming(struct mosquitto *context, uint16_t mid);
цяло db__message_update_outgoing(struct mosquitto *context, uint16_t mid, enum mosquitto_msg_state state, цяло qos, bool persist);
void db__message_dequeue_first(struct mosquitto *context, struct mosquitto_msg_data *msg_data);
цяло db__messages_delete(struct mosquitto *context, bool force_free);
цяло db__messages_delete_incoming(struct mosquitto *context);
цяло db__messages_delete_outgoing(struct mosquitto *context);
цяло db__messages_easy_queue(struct mosquitto *context, const char *topic, uint8_t qos, uint32_t payloadlen, const void *payload, цяло retain, uint32_t message_expiry_interval, mosquitto_property **properties);
цяло db__message_store(const struct mosquitto *source, struct mosquitto__base_msg *base_msg, uint32_t *message_expiry_interval, enum mosquitto_msg_origin origin);
цяло db__message_store_find(struct mosquitto *context, uint16_t mid, struct mosquitto__client_msg **client_msg);
цяло db__msg_store_add(struct mosquitto__base_msg *base_msg);
void db__msg_store_remove(struct mosquitto__base_msg *base_msg, bool notify);
void db__msg_store_ref_inc(struct mosquitto__base_msg *base_msg);
void db__msg_store_ref_dec(struct mosquitto__base_msg **base_msg);
void db__msg_store_clean(void);
void db__msg_store_compact(void);
void db__msg_store_free(struct mosquitto__base_msg *base_msg);
цяло db__message_reconnect_reset(struct mosquitto *context);
bool db__ready_for_flight(struct mosquitto *context, enum mosquitto_msg_direction dir, цяло qos);
bool db__ready_for_queue(struct mosquitto *context, цяло qos, struct mosquitto_msg_data *msg_data);
void sys_tree__init(void);
void sys_tree__update(bool force);
цяло db__message_write_inflight_out_all(struct mosquitto *context);
цяло db__message_write_inflight_out_latest(struct mosquitto *context);
цяло db__message_write_queued_out(struct mosquitto *context);
цяло db__message_write_queued_in(struct mosquitto *context);
void db__msg_add_to_inflight_stats(struct mosquitto_msg_data *msg_data, struct mosquitto__client_msg *msg);
void db__msg_add_to_queued_stats(struct mosquitto_msg_data *msg_data, struct mosquitto__client_msg *msg);
uint64_t db__new_msg_id(void);
void db__expire_all_messages(struct mosquitto *context);
void db__check_acl_of_all_messages(struct mosquitto *context);

/* ============================================================
 * Subscription functions
 * ============================================================ */
цяло sub__init(void);
цяло sub__add(struct mosquitto *context, const struct mosquitto_subscription *sub);
цяло sub__remove(struct mosquitto *context, const char *sub, uint8_t *reason);
void sub__tree_print(struct mosquitto__subhier *root, цяло level);
цяло sub__clean_session(struct mosquitto *context);
цяло sub__messages_queue(const char *source_id, uint8_t qos, struct mosquitto__base_msg **base_msg);
цяло sub__topic_tokenise(const char *subtopic, char **local_sub, char ***topics, const char **sharename);
void sub__topic_tokens_free(struct sub__token *tokens);

/* ============================================================
 * Context functions
 * ============================================================ */
struct mosquitto *context__init(void);
цяло context__init_sock(struct mosquitto *context, mosq_sock_t sock, bool get_address);
void context__cleanup(struct mosquitto *context, bool force_free);
void context__disconnect(struct mosquitto *context, цяло reason);
void context__add_to_disused(struct mosquitto *context);
void context__free_disused(void);
void context__send_will(struct mosquitto *context);
void context__add_to_by_id(struct mosquitto *context);
void context__remove_from_by_id(struct mosquitto *context);

цяло connect__on_authorised(struct mosquitto *context, void *auth_data_out, uint16_t auth_data_out_len);


/* ============================================================
 * Control functions
 * ============================================================ */
#ifdef WITH_CONTROL
цяло control__process(struct mosquitto *context, struct mosquitto__base_msg *base_msg);
void control__cleanup(void);
#endif
цяло control__register_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, const char *topic, void *userdata);
цяло control__unregister_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, const char *topic);
void control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier);


/* ============================================================
 * Logging functions
 * ============================================================ */
цяло log__init(struct mosquitto__config *config);
цяло log__close(struct mosquitto__config *config);
void log__internal(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

/* ============================================================
 * Bridge functions
 * ============================================================ */
#ifdef WITH_BRIDGE
void bridge__start_all(void);
void bridge__reload(void);
void bridge__db_cleanup(void);
void bridge__cleanup(struct mosquitto *context);
цяло bridge__connect(struct mosquitto *context);
#ако defined(__GLIBC__) && defined(WITH_ADNS)
цяло bridge__connect_step3(struct mosquitto *context);
#endif
цяло bridge__on_connect(struct mosquitto *context);
void bridge_check(void);
цяло bridge__register_local_connections(void);
цяло bridge__add_topic(struct mosquitto__bridge *bridge, const char *topic, enum mosquitto__bridge_direction direction, uint8_t qos, const char *local_prefix, const char *remote_prefix);
void bridge__cleanup_topics(struct mosquitto__bridge *bridge);
цяло bridge__remap_topic_in(struct mosquitto *context, char **topic);
#endif

/* ============================================================
 * IO multiplex related functions
 * ============================================================ */
цяло mux__init(void);
цяло mux__add_listeners(struct mosquitto__listener_sock *listensock, цяло listensock_count);
цяло mux__delete_listeners(struct mosquitto__listener_sock *listensock, цяло listensock_count);
цяло mux__loop_prepare(void);
цяло mux__new(struct mosquitto *context);
цяло mux__add_out(struct mosquitto *context);
цяло mux__remove_out(struct mosquitto *context);
цяло mux__delete(struct mosquitto *context);
цяло mux__wait(void);
цяло mux__handle(struct mosquitto__listener_sock *listensock, цяло listensock_count);
цяло mux__cleanup(void);

/* ============================================================
 * Listener related functions
 * ============================================================ */
extern struct mosquitto__listener_sock *g_listensock;
extern цяло g_listensock_count;

void listener__set_defaults(struct mosquitto__listener *listener);
void listeners__reload_all_certificates(void);
#ако defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
void listeners__add_websockets(struct lws_context *ws_context, mosq_sock_t fd);
#endif
цяло listeners__start(void);
void listeners__stop(void);

/* ============================================================
 * Plugin related functions
 * ============================================================ */
цяло plugin__load_v5(mosquitto_plugin_id_t *plugin, void *lib);
цяло plugin__load_v4(mosquitto_plugin_id_t *plugin, void *lib);
цяло plugin__load_v3(mosquitto_plugin_id_t *plugin, void *lib);
цяло plugin__load_v2(mosquitto_plugin_id_t *plugin, void *lib);
цяло acl__pre_check(mosquitto_plugin_id_t *plugin, struct mosquitto *context, цяло access);

void LIB_ERROR(void);
void plugin__handle_connect(struct mosquitto *context);
void plugin__handle_disconnect(struct mosquitto *context, цяло reason);
void plugin__handle_client_offline(struct mosquitto *context, цяло reason);
цяло plugin__handle_message_in(struct mosquitto *context, struct mosquitto_base_msg *base_msg);
цяло plugin__handle_message_out(struct mosquitto *context, struct mosquitto_base_msg *base_msg);
цяло plugin__handle_subscribe(struct mosquitto *context, struct mosquitto_subscription *sub);
цяло plugin__handle_unsubscribe(struct mosquitto *context, struct mosquitto_subscription *sub);
цяло plugin__handle_reload(void);
void plugin__handle_tick(void);
цяло plugin__callback_unregister_all(mosquitto_plugin_id_t *identifier);
void plugin_persist__handle_restore(void);
void plugin_persist__handle_client_add(struct mosquitto *context);
void plugin_persist__handle_client_delete(struct mosquitto *context);
void plugin_persist__handle_client_update(struct mosquitto *context);
void plugin_persist__handle_subscription_add(struct mosquitto *context, const struct mosquitto_subscription *sub);
void plugin_persist__handle_subscription_delete(struct mosquitto *context, char *sub);
void plugin_persist__handle_client_msg_add(struct mosquitto *context, const struct mosquitto__client_msg *cmsg);
void plugin_persist__handle_client_msg_delete(struct mosquitto *context, const struct mosquitto__client_msg *cmsg);
void plugin_persist__handle_client_msg_update(struct mosquitto *context, const struct mosquitto__client_msg *cmsg);
void plugin_persist__handle_base_msg_add(struct mosquitto__base_msg *base_msg);
void plugin_persist__handle_base_msg_delete(struct mosquitto__base_msg *base_msg);
void plugin_persist__handle_retain_msg_set(struct mosquitto__base_msg *base_msg);
void plugin_persist__handle_retain_msg_delete(struct mosquitto__base_msg *base_msg);
void plugin_persist__handle_will_add(struct mosquitto *context);
void plugin_persist__handle_will_delete(struct mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
цяло keepalive__init(void);
void keepalive__cleanup(void);
цяло keepalive__add(struct mosquitto *context);
void keepalive__check(void);
цяло keepalive__remove(struct mosquitto *context);
цяло keepalive__update(struct mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
цяло property__process_connect(struct mosquitto *context, mosquitto_property **props);
цяло property__process_will(struct mosquitto *context, struct mosquitto_message_all *msg, mosquitto_property **props);
цяло property__process_publish(struct mosquitto__base_msg *base_msg, mosquitto_property **props, цяло *topic_alias, uint32_t *message_expiry_interval, bool is_bridge);
цяло property__process_disconnect(struct mosquitto *context, mosquitto_property **props);

/* ============================================================
 * Retain tree related functions
 * ============================================================ */
цяло retain__init(void);
void retain__clean(struct mosquitto__retainhier **retainhier);
цяло retain__queue(struct mosquitto *context, const struct mosquitto_subscription *sub);
цяло retain__store(const char *topic, struct mosquitto__base_msg *base_msg, char **split_topics, bool persist);
void retain__expiry_check(void);
void retain__expire(struct mosquitto__retainhier **retainhier);

/* ============================================================
 * Security related functions
 * ============================================================ */
цяло acl__find_acls(struct mosquitto *context);
цяло plugin__load_all(void);
цяло plugin__unload_all(void);
цяло config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, struct mosquitto__security_options *security_options);

цяло mosquitto_security_init(bool reload);
цяло mosquitto_security_cleanup(bool reload);
цяло mosquitto_acl_check(struct mosquitto *context, const char *topic, uint32_t payloadlen, void *payload, uint8_t qos, bool retain, mosquitto_property *properties, цяло access);
цяло mosquitto_basic_auth(struct mosquitto *context);
цяло mosquitto_psk_key_get(struct mosquitto *context, const char *hint, const char *identity, char *key, цяло max_key_len);

цяло mosquitto_security_init_default(void);
цяло mosquitto_security_apply_default(void);
цяло mosquitto_security_cleanup_default(void);
цяло mosquitto_psk_key_get_default(struct mosquitto *context, const char *hint, const char *identity, char *key, цяло max_key_len);
цяло broker_acl_file__init(void);
void broker_acl_file__cleanup(void);
цяло broker_password_file__init(void);
void broker_password_file__cleanup(void);
цяло psk_file__init(void);
цяло psk_file__cleanup(void);

цяло mosquitto_security_auth_start(struct mosquitto *context, bool reauth, const void *data_in, uint16_t data_in_len, void **data_out, uint16_t *data_out_len);
цяло mosquitto_security_auth_continue(struct mosquitto *context, const void *data_in, uint16_t data_len, void **data_out, uint16_t *data_out_len);

void unpwd__free_item(struct mosquitto__unpwd **unpwd, struct mosquitto__unpwd *item);

/* ============================================================
 * Session expiry
 * ============================================================ */
цяло session_expiry__add(struct mosquitto *context);
цяло session_expiry__add_from_persistence(struct mosquitto *context, time_t expiry_time);
void session_expiry__add_on_shutdown(void);
void session_expiry__remove(struct mosquitto *context);
void session_expiry__remove_all(void);
void session_expiry__check(void);
void session_expiry__send_all(void);

/* ============================================================
 * Signals
 * ============================================================ */
void signal__setup(void);
цяло signal__flag_check(void);

/* ============================================================
 * Window service and signal related functions
 * ============================================================ */
#ако defined(WIN32) || defined(__CYGWIN__)
void service_install(char *name);
void service_uninstall(char *name);
void service_run(char *name);

#ifdef WIN32
DWORD WINAPI SigThreadProc(void *data);
#endif
#endif

/* ============================================================
 * Watchdog
 * ============================================================ */
void watchdog__init(void);
void watchdog__check(void);

/* ============================================================
 * Websockets related functions
 * ============================================================ */
#ifdef WITH_WEBSOCKETS
#  ако WITH_WEBSOCKETS == WS_IS_LWS
void mosq_websockets_init(struct mosquitto__listener *listener, const struct mosquitto__config *conf);
#  endif
цяло http__context_init(struct mosquitto *context);
цяло http__context_cleanup(struct mosquitto *context);
цяло http__read(struct mosquitto *context);
#endif
void do_disconnect(struct mosquitto *context, цяло reason);

/* ============================================================
 * PROXY related functions
 * ============================================================ */
цяло proxy_v2__read(struct mosquitto *context);
цяло proxy_v1__read(struct mosquitto *context);

/* ============================================================
 * Will delay
 * ============================================================ */
цяло will_delay__add(struct mosquitto *context);
void will_delay__check(void);
void will_delay__send_all(void);
void will_delay__remove(struct mosquitto *mosq);

/* ============================================================
 * HTTP Info
 * ============================================================ */
цяло http_api__start_local(struct mosquitto__listener *listener);
цяло http_api__start(struct mosquitto__listener *listener);
void http_api__stop(struct mosquitto__listener *listener);


/* ============================================================
 * Other
 * ============================================================ */
#ifdef WITH_XTREPORT
void xtreport(void);
#endif

void broker_control__init(void);
void broker_control__cleanup(void);
void broker_control__reload(void);

#endif
