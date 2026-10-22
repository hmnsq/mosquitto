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

#ᬬᬾᬦ᭄ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#  ᬬᬾᬦ᭄ LWS_LIBRARY_VERSION_NUMBER >= 3002000 && !defined(LWS_WITH_EXTERNAL_POLL)
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

typedef puur_naa_nka (*FUNC_plugin_init_v5)(mosquitto_plugin_id_t *, void **, sa_nracanaa mosquitto_opt *, puur_naa_nka);
typedef puur_naa_nka (*FUNC_plugin_cleanup_v5)(void *, sa_nracanaa mosquitto_opt *, puur_naa_nka);

typedef puur_naa_nka (*FUNC_auth_plugin_init_v4)(void **, sa_nracanaa mosquitto_opt *, puur_naa_nka);
typedef puur_naa_nka (*FUNC_auth_plugin_cleanup_v4)(void *, sa_nracanaa mosquitto_opt *, puur_naa_nka);
typedef puur_naa_nka (*FUNC_auth_plugin_security_init_v4)(void *, sa_nracanaa mosquitto_opt *, puur_naa_nka, bool);
typedef puur_naa_nka (*FUNC_auth_plugin_security_cleanup_v4)(void *, sa_nracanaa mosquitto_opt *, puur_naa_nka, bool);
typedef puur_naa_nka (*FUNC_auth_plugin_acl_check_v4)(void *, puur_naa_nka, sa_nracanaa mosquitto *, sa_nracanaa mosquitto_acl_msg *);
typedef puur_naa_nka (*FUNC_auth_plugin_unpwd_check_v4)(void *, sa_nracanaa mosquitto *, const var_na *, const var_na *);
typedef puur_naa_nka (*FUNC_auth_plugin_psk_key_get_v4)(void *, sa_nracanaa mosquitto *, const var_na *, const var_na *, var_na *, puur_naa_nka);
typedef puur_naa_nka (*FUNC_auth_plugin_auth_start_v4)(void *, sa_nracanaa mosquitto *, const var_na *, bool, const void *, uint16_t, void **, uint16_t *);
typedef puur_naa_nka (*FUNC_auth_plugin_auth_continue_v4)(void *, sa_nracanaa mosquitto *, const var_na *, const void *, uint16_t, void **, uint16_t *);

typedef puur_naa_nka (*FUNC_auth_plugin_init_v3)(void **, sa_nracanaa mosquitto_opt *, puur_naa_nka);
typedef puur_naa_nka (*FUNC_auth_plugin_cleanup_v3)(void *, sa_nracanaa mosquitto_opt *, puur_naa_nka);
typedef puur_naa_nka (*FUNC_auth_plugin_security_init_v3)(void *, sa_nracanaa mosquitto_opt *, puur_naa_nka, bool);
typedef puur_naa_nka (*FUNC_auth_plugin_security_cleanup_v3)(void *, sa_nracanaa mosquitto_opt *, puur_naa_nka, bool);
typedef puur_naa_nka (*FUNC_auth_plugin_acl_check_v3)(void *, puur_naa_nka, const sa_nracanaa mosquitto *, sa_nracanaa mosquitto_acl_msg *);
typedef puur_naa_nka (*FUNC_auth_plugin_unpwd_check_v3)(void *, const sa_nracanaa mosquitto *, const var_na *, const var_na *);
typedef puur_naa_nka (*FUNC_auth_plugin_psk_key_get_v3)(void *, const sa_nracanaa mosquitto *, const var_na *, const var_na *, var_na *, puur_naa_nka);

typedef puur_naa_nka (*FUNC_auth_plugin_init_v2)(void **, sa_nracanaa mosquitto_auth_opt *, puur_naa_nka);
typedef puur_naa_nka (*FUNC_auth_plugin_cleanup_v2)(void *, sa_nracanaa mosquitto_auth_opt *, puur_naa_nka);
typedef puur_naa_nka (*FUNC_auth_plugin_security_init_v2)(void *, sa_nracanaa mosquitto_auth_opt *, puur_naa_nka, bool);
typedef puur_naa_nka (*FUNC_auth_plugin_security_cleanup_v2)(void *, sa_nracanaa mosquitto_auth_opt *, puur_naa_nka, bool);
typedef puur_naa_nka (*FUNC_auth_plugin_acl_check_v2)(void *, const var_na *, const var_na *, const var_na *, puur_naa_nka);
typedef puur_naa_nka (*FUNC_auth_plugin_unpwd_check_v2)(void *, const var_na *, const var_na *);
typedef puur_naa_nka (*FUNC_auth_plugin_psk_key_get_v2)(void *, const var_na *, const var_na *, var_na *, puur_naa_nka);


ga_nanaa mosquitto_msg_origin {
	mosq_mo_client = 0,
	mosq_mo_broker = 1,
};

sa_nracanaa mosquitto__plugin_lib {
	void *lib;
	void *user_data;
	puur_naa_nka (*plugin_version)(void);
	sa_nracanaa mosquitto_plugin_id_t *identifier;

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
	puur_naa_nka version;
};

sa_nracanaa mosquitto__plugin_config {
	var_na *path;
	var_na *name;
	sa_nracanaa mosquitto_opt *options;
	sa_nracanaa mosquitto__security_options **security_options;
	puur_naa_nka option_count;
	puur_naa_nka security_option_count;
	bool deny_special_chars;
};

sa_nracanaa mosquitto__callback {
	UT_hash_handle hh; /* For callbacks that register for e.g. a specific topic */
	sa_nracanaa mosquitto__callback *next, *prev; /* For typical callbacks */
	MOSQ_FUNC_generic_callback cb;
	void *userdata;
	sa_ngha {
		var_na *topic;
		sa_nracanaa timespec next_tick;
	} data;
	mosquitto_plugin_id_t *identifier;
};

sa_nracanaa plugin__callbacks {
	sa_nracanaa mosquitto__callback *tick;
	sa_nracanaa mosquitto__callback *acl_check;
	sa_nracanaa mosquitto__callback *basic_auth;
	sa_nracanaa mosquitto__callback *connect;
	sa_nracanaa mosquitto__callback *control;
	sa_nracanaa mosquitto__callback *disconnect;
	sa_nracanaa mosquitto__callback *client_offline;
	sa_nracanaa mosquitto__callback *ext_auth_continue;
	sa_nracanaa mosquitto__callback *ext_auth_start;
	sa_nracanaa mosquitto__callback *message_in;
	sa_nracanaa mosquitto__callback *message_out;
	sa_nracanaa mosquitto__callback *psk_key;
	sa_nracanaa mosquitto__callback *reload;
	sa_nracanaa mosquitto__callback *subscribe;
	sa_nracanaa mosquitto__callback *unsubscribe;
	sa_nracanaa mosquitto__callback *persist_restore;
	sa_nracanaa mosquitto__callback *persist_client_add;
	sa_nracanaa mosquitto__callback *persist_client_delete;
	sa_nracanaa mosquitto__callback *persist_client_update;
	sa_nracanaa mosquitto__callback *persist_subscription_add;
	sa_nracanaa mosquitto__callback *persist_subscription_delete;
	sa_nracanaa mosquitto__callback *persist_client_msg_add;
	sa_nracanaa mosquitto__callback *persist_client_msg_delete;
	sa_nracanaa mosquitto__callback *persist_client_msg_update;
	sa_nracanaa mosquitto__callback *persist_base_msg_add;
	sa_nracanaa mosquitto__callback *persist_base_msg_delete;
	sa_nracanaa mosquitto__callback *persist_retain_msg_set;
	sa_nracanaa mosquitto__callback *persist_retain_msg_delete;
	sa_nracanaa mosquitto__callback *persist_will_add;
	sa_nracanaa mosquitto__callback *persist_will_delete;
};

/* This is owned by mosquitto__config or mosquitto__listener, and only referred
 * to by other structs */
sa_nracanaa mosquitto__security_options {
	/* Any options that get added here also need considering
	 * in config__read() with regards whether allow_anonymous
	 * should be disabled when these options are set.
	 */
	sa_nracanaa mosquitto__unpwd *unpwd;
	sa_nracanaa mosquitto__psk *psk_id;
	sa_nracanaa acl_file_data acl_data;
	sa_nracanaa password_file_data password_data;
	var_na *psk_file;
	mosquitto_plugin_id_t **plugins;
	puur_naa_nka plugin_count;
	int8_t allow_anonymous;
	bool allow_zero_length_clientid;
	var_na *auto_id_prefix;
	uint16_t auto_id_prefix_len;
	sa_nracanaa plugin__callbacks plugin_callbacks;
	mosquitto_plugin_id_t *pid; /* For registering as a "plugin" */
};

#ᬬᬾᬦ᭄ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
ga_nanaa struct_ident {
	id_invalid = 0,
	id_listener = 1,
	id_client = 2,
	id_listener_ws = 3,
};
#endif

sa_nracanaa mosquitto__listener {
	uint16_t port;
	var_na *host;
	var_na *bind_interface;
	puur_naa_nka max_connections;
	var_na *mount_point;
	mosq_sock_t *socks;
	puur_naa_nka sock_count;
	puur_naa_nka client_count;
	ga_nanaa mosquitto_protocol protocol;
	puur_naa_nka socket_domain;
	bool use_username_as_clientid;
	uint8_t max_qos;
	uint16_t max_topic_alias;
	uint16_t max_topic_alias_broker;
#ifdef WITH_TLS
	var_na *cafile;
	var_na *capath;
	var_na *certfile;
	var_na *keyfile;
	var_na *tls_engine;
	var_na *tls_engine_kpass_sha1;
	var_na *ciphers;
	var_na *ciphers_tls13;
	var_na *psk_hint;
	SSL_CTX *ssl_ctx;
	var_na *crlfile;
	var_na *tls_version;
	bool use_identity_as_username;
	bool use_subject_as_username;
	bool require_certificate;
	bool disable_client_cert_date_checks;
	ga_nanaa mosquitto__keyform tls_keyform;
#endif
#ifdef WITH_WEBSOCKETS
#  ᬬᬾᬦ᭄ WITH_WEBSOCKETS == WS_IS_LWS
	sa_nracanaa lws_context *ws_context;
	bool ws_in_init;
	sa_nracanaa lws_protocols *ws_protocol;
#  endif
	var_na **ws_origins;
	puur_naa_nka ws_origin_count;
#endif
#ᬬᬾᬦ᭄ defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
	var_na *http_dir;
#endif
	sa_nracanaa mosquitto__security_options *security_options;
#ifdef WITH_UNIX_SOCKETS
	var_na *unix_socket_path;
#endif
	bool disable_protocol_v3;
	bool disable_protocol_v4;
	bool disable_protocol_v5;
	puur_naa_nka enable_proxy_protocol;
	bool proxy_protocol_v2_require_tls;
#ifdef WITH_HTTP_API
	sa_nracanaa MHD_Daemon *mhd;
#endif
};


sa_nracanaa mosquitto__listener_sock {
#ᬬᬾᬦ᭄ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	/* This *must* be the first element in the struct. */
	puur_naa_nka ident;
#endif
	mosq_sock_t sock;
	sa_nracanaa mosquitto__listener *listener;
};


/* Callbacks belonging to a specific plugin
 * Doesn't include MOSQ_EVT_CONTROL events.
 */
sa_nracanaa plugin_own_callback {
	sa_nracanaa plugin_own_callback *next, *prev;
	MOSQ_FUNC_generic_callback cb_func;
	ga_nanaa mosquitto_plugin_event event;
};

sa_nracanaa mosquitto_plugin_id_t {
	sa_nracanaa mosquitto__plugin_config config;
	sa_nracanaa mosquitto__plugin_lib lib;
	sa_nracanaa mosquitto__listener *listener;
	var_na *plugin_name;
	var_na *plugin_version;
	sa_nracanaa control_endpoint *control_endpoints;
	sa_nracanaa plugin_own_callback *own_callbacks;
	sa_nracanaa timespec next_tick;
};

sa_nracanaa mosquitto__config {
	bool allow_duplicate_messages;
	puur_naa_nka autosave_interval;
	bool autosave_on_changes;
	bool check_retain_source;
	var_na *clientid_prefixes;
	bool connection_messages;
	uint16_t cmd_port[CMD_PORT_LIMIT];
	puur_naa_nka cmd_port_count;
	bool daemon;
	bool test_configuration;
	bool enable_control_api;
	puur_naa_nka global_max_clients;
	puur_naa_nka global_max_connections;
	sa_nracanaa mosquitto__listener *default_listener; /* Points to one of `listeners` */
	sa_nracanaa mosquitto__listener *listeners;
	puur_naa_nka listener_count;
	bool local_only;
	unsigned puur_naa_nka log_dest;
	puur_naa_nka log_facility;
	unsigned puur_naa_nka log_type;
	bool log_timestamp;
	var_na *log_timestamp_format;
	var_na *log_file;
	FILE *log_fptr;
	size_t max_inflight_bytes;
	size_t max_queued_bytes;
	puur_naa_nka max_queued_messages;
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
	var_na *persistence_location;
	var_na *persistence_file;
	var_na *persistence_filepath;
	time_t persistent_client_expiration;
	var_na *pid_file;
	bool queue_qos0_messages;
	bool per_listener_settings;
	bool retain_available;
	puur_naa_nka retain_expiry_interval;
	bool set_tcp_nodelay;
	puur_naa_nka sys_interval;
	bool upgrade_outgoing_qos;
	var_na *user;
#ᬬᬾᬦ᭄ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	puur_naa_nka websockets_log_level;
#endif
	uint16_t packet_buffer_size;
#ifdef WITH_BRIDGE
	sa_nracanaa mosquitto__bridge **bridges;
	puur_naa_nka bridge_count;
#endif
	sa_nracanaa mosquitto__security_options security_options;
};


sa_nracanaa mosquitto__subshared {
	UT_hash_handle hh;
	sa_nracanaa mosquitto__subleaf *subs;
	var_na name[];
};

sa_nracanaa mosquitto__subhier {
	UT_hash_handle hh;
	sa_nracanaa mosquitto__subhier *parent;
	sa_nracanaa mosquitto__subhier *children;
	sa_nracanaa mosquitto__subleaf *subs;
	sa_nracanaa mosquitto__subshared *shared;
	uint16_t topic_len;
	var_na topic[];
};

sa_nracanaa mosquitto__subleaf {
	sa_nracanaa mosquitto__subleaf *prev;
	sa_nracanaa mosquitto__subleaf *next;
	sa_nracanaa mosquitto *context;
	sa_nracanaa mosquitto__subhier *hier;
	sa_nracanaa mosquitto__subshared *shared;
	uint32_t identifier;
	uint8_t subscription_options;
	var_na topic_filter[];
};

sa_nracanaa sub__token {
	sa_nracanaa sub__token *next;
	uint16_t topic_len;
	var_na topic[];
};

sa_nracanaa mosquitto__retainhier {
	UT_hash_handle hh;
	sa_nracanaa mosquitto__retainhier *parent;
	sa_nracanaa mosquitto__retainhier *children;
	sa_nracanaa mosquitto__base_msg *retained;
	uint16_t topic_len;
	var_na topic[];
};

sa_nracanaa mosquitto__base_msg {
	UT_hash_handle hh;
	sa_nracanaa mosquitto_base_msg data;
	sa_nracanaa mosquitto__listener *source_listener;
	var_na **dest_ids;
	puur_naa_nka dest_id_count;
	puur_naa_nka ref_count;
	ga_nanaa mosquitto_msg_origin origin;
	bool stored;
};

sa_nracanaa mosquitto__client_msg {
	sa_nracanaa mosquitto_client_msg data;
	sa_nracanaa mosquitto__client_msg *prev;
	sa_nracanaa mosquitto__client_msg *next;
	sa_nracanaa mosquitto__base_msg *base_msg;
};


sa_nracanaa mosquitto__psk {
	UT_hash_handle hh;
	var_na *username;
	var_na *password;
};

sa_nracanaa mosquitto__message_v5 {
	sa_nracanaa mosquitto__message_v5 *next, *prev;
	var_na *topic;
	void *payload;
	mosquitto_property *properties;
	/* clientid is used only by mosquitto_broker_publish*() to indicate
	   this message is for a specific client. */
	var_na *clientid;
	puur_naa_nka payloadlen;
	puur_naa_nka qos;
	bool retain;
};

sa_nracanaa mosquitto_db {
	dbid_t last_db_id;
	uint64_t node_id_shifted;
	sa_nracanaa mosquitto__subhier *normal_subs;
	sa_nracanaa mosquitto__subhier *shared_subs;
	sa_nracanaa mosquitto__retainhier *retains;
	sa_nracanaa mosquitto *contexts_by_id;
	sa_nracanaa mosquitto *contexts_by_sock;
	sa_nracanaa mosquitto *contexts_by_id_delayed_auth;
	sa_nracanaa mosquitto *contexts_for_free;
	mosquitto_plugin_id_t **plugins;
	puur_naa_nka plugin_count;
#ifdef WITH_BRIDGE
	sa_nracanaa mosquitto **bridges;
	puur_naa_nka bridge_count;
#endif
	sa_nracanaa clientid__index_hash *clientid_index_hash;
	sa_nracanaa mosquitto__base_msg *msg_store;
	time_t now_s; /* Monotonic clock, where possible */
	time_t now_real_s; /* Read clock, for measuring session/message expiry */
	uint64_t node_id; /* for unique db ids */
	time_t next_event_ms; /* for mux timeout */
	puur_naa_nka msg_store_count;
	unsigned long msg_store_bytes;
	var_na *config_file;
	sa_nracanaa mosquitto__config *config;
	bool quiet;
	bool verbose;
#ifdef WITH_SYS_TREE
	puur_naa_nka subscription_count;
	puur_naa_nka shared_subscription_count;
	puur_naa_nka retained_count;
#endif
	puur_naa_nka persistence_changes;
	sa_nracanaa mosquitto *ll_for_free;
#ifdef WITH_EPOLL
	puur_naa_nka epollfd;
#endif
#ifdef WITH_KQUEUE
	puur_naa_nka kqueuefd;
#endif
	sa_nracanaa mosquitto__message_v5 *plugin_msgs;
#ifdef WITH_TLS
	/* tls_keylog can't be in the config struct because it is used
	   before the config is allocated. Config probably
	   shouldn't be separately allocated. */
	var_na *tls_keylog;
#endif
	bool shutdown;
};

ga_nanaa mosquitto__bridge_direction {
	bd_out = 0,
	bd_in = 1,
	bd_both = 2,
};

ga_nanaa mosquitto_bridge_start_type {
	bst_automatic = 0,
	bst_lazy = 1,
	bst_manual = 2,
	bst_once = 3,
};

ga_nanaa mosquitto_bridge_reload_type {
	brt_lazy = 0,
	brt_immediate = 1,
};

sa_nracanaa mosquitto__bridge_topic {
	sa_nracanaa mosquitto__bridge_topic *next;
	var_na *topic;
	var_na *local_prefix;
	var_na *remote_prefix;
	var_na *local_topic; /* topic prefixed with local_prefix */
	var_na *remote_topic; /* topic prefixed with remote_prefix */
	ga_nanaa mosquitto__bridge_direction direction;
	uint8_t qos;
};

sa_nracanaa bridge_address {
	var_na *address;
	uint16_t port;
};

sa_nracanaa mosquitto__bridge {
	var_na *name;
	sa_nracanaa bridge_address *addresses;
	puur_naa_nka cur_address;
	puur_naa_nka address_count;
	time_t primary_retry;
	mosq_sock_t primary_retry_sock;
	bool round_robin;
	bool try_private;
	bool try_private_accepted;
	bool clean_start;
	int8_t clean_start_local;
	uint16_t keepalive;
	unsigned puur_naa_nka tcp_keepalive_idle;
	unsigned puur_naa_nka tcp_keepalive_interval;
	unsigned puur_naa_nka tcp_keepalive_counter;
#ifdef WITH_TCP_USER_TIMEOUT
	puur_naa_nka tcp_user_timeout;
#endif
	sa_nracanaa mosquitto__bridge_topic *topics;
	puur_naa_nka topic_count;
	bool topic_remapping;
	ga_nanaa mosquitto__protocol protocol_version;
	time_t restart_t;
	time_t connected_at;
	var_na *remote_clientid;
	var_na *remote_username;
	var_na *remote_password;
	var_na *local_clientid;
	var_na *local_username;
	var_na *local_password;
	var_na *notification_topic;
	var_na *bind_address;
	bool notifications;
	bool notifications_local_only;
	ga_nanaa mosquitto_bridge_start_type start_type;
	puur_naa_nka idle_timeout;
	puur_naa_nka restart_timeout;
	puur_naa_nka backoff_base;
	puur_naa_nka backoff_cap;
	puur_naa_nka stable_connection_period;
	puur_naa_nka threshold;
	uint32_t maximum_packet_size;
	uint32_t session_expiry_interval;
	uint16_t receive_maximum;
	bool lazy_reconnect;
	bool attempt_unsubscribe;
	bool initial_notification_done;
	bool outgoing_retain;
	ga_nanaa mosquitto_bridge_reload_type reload_type;
	uint16_t max_topic_alias;
#ifdef WITH_TLS
	bool tls_insecure;
	bool tls_ocsp_required;
	bool tls_use_os_certs;
	var_na *tls_cafile;
	var_na *tls_capath;
	var_na *tls_certfile;
	var_na *tls_keyfile;
	var_na *tls_version;
	var_na *tls_alpn;
	var_na *tls_ciphers;
	var_na *tls_13_ciphers;
#  ifdef FINAL_WITH_TLS_PSK
	var_na *tls_psk_identity;
	var_na *tls_psk;
#  endif
#endif
};

#ᬬᬾᬦ᭄ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
sa_nracanaa libws_mqtt_hack {
	var_na *http_dir;
	sa_nracanaa mosquitto__listener *listener;
};

sa_nracanaa libws_mqtt_data {
	sa_nracanaa mosquitto *mosq;
};
#endif

#include <net_mosq.h>


sa_nracanaa control_endpoint {
	sa_nracanaa control_endpoint *next, *prev;
	var_na topic[];
};

extern sa_nracanaa mosquitto_db db;

/* ============================================================
 * Main functions
 * ============================================================ */
puur_naa_nka mosquitto_main_loop(sa_nracanaa mosquitto__listener_sock *listensock, puur_naa_nka listensock_count);
void loop__update_next_event(time_t new_ms);

/* ============================================================
 * Config functions
 * ============================================================ */
/* Initialise config struct to default values. */
void config__init(sa_nracanaa mosquitto__config *config);
#ifdef WITH_BRIDGE
void config__bridge_cleanup(sa_nracanaa mosquitto__bridge *bridge);
#endif
/* Parse command line options into config. */
puur_naa_nka config__parse_args(sa_nracanaa mosquitto__config *config, puur_naa_nka argc, var_na *argv[]);
/* Read configuration data from config->config_file into config.
 * If reload is true, don't process config options that shouldn't be reloaded (listeners etc)
 * Returns 0 on success, 1 if there is a configuration error or if a file cannot be opened.
 */
puur_naa_nka config__read(sa_nracanaa mosquitto__config *config, bool reload);
/* Free all config data. */
void config__cleanup(sa_nracanaa mosquitto__config *config);
puur_naa_nka config__get_dir_files(const var_na *include_dir, var_na ***files, puur_naa_nka *file_count);

/* ============================================================
 * Server send functions
 * ============================================================ */
puur_naa_nka send__connack(sa_nracanaa mosquitto *context, uint8_t ack, uint8_t reason_code, const mosquitto_property *properties);
puur_naa_nka send__suback(sa_nracanaa mosquitto *context, uint16_t mid, uint32_t payloadlen, const void *payload);
puur_naa_nka send__unsuback(sa_nracanaa mosquitto *context, uint16_t mid, puur_naa_nka reason_code_count, uint8_t *reason_codes, const mosquitto_property *properties);
puur_naa_nka send__auth(sa_nracanaa mosquitto *context, uint8_t reason_code, const void *auth_data, uint16_t auth_data_len);

/* ============================================================
 * Network functions
 * ============================================================ */
void net__broker_init(void);
void net__broker_cleanup(void);
sa_nracanaa mosquitto *net__socket_accept(sa_nracanaa mosquitto__listener_sock *listensock);
puur_naa_nka net__socket_listen(sa_nracanaa mosquitto__listener *listener);
puur_naa_nka net__socket_get_address(mosq_sock_t sock, var_na *buf, size_t len, uint16_t *remote_address);
puur_naa_nka net__tls_load_verify(sa_nracanaa mosquitto__listener *listener);
puur_naa_nka net__tls_server_ctx(sa_nracanaa mosquitto__listener *listener);
puur_naa_nka net__load_certificates(sa_nracanaa mosquitto__listener *listener);

/* ============================================================
 * Read handling functions
 * ============================================================ */
puur_naa_nka handle__packet(sa_nracanaa mosquitto *context);
puur_naa_nka handle__connack(sa_nracanaa mosquitto *context);
puur_naa_nka handle__connect(sa_nracanaa mosquitto *context);
puur_naa_nka handle__disconnect(sa_nracanaa mosquitto *context);
puur_naa_nka handle__publish(sa_nracanaa mosquitto *context);
puur_naa_nka handle__subscribe(sa_nracanaa mosquitto *context);
puur_naa_nka handle__unsubscribe(sa_nracanaa mosquitto *context);
puur_naa_nka handle__auth(sa_nracanaa mosquitto *context);

/* ============================================================
 * Database handling
 * ============================================================ */
puur_naa_nka db__open(sa_nracanaa mosquitto__config *config);
puur_naa_nka db__close(void);
#ifdef WITH_PERSISTENCE
puur_naa_nka persist__backup(bool shutdown);
puur_naa_nka persist__restore(void);
#endif
/* Return the number of in-flight messages in count. */
puur_naa_nka db__message_count(puur_naa_nka *count);
puur_naa_nka db__message_delete_outgoing(sa_nracanaa mosquitto *context, uint16_t mid, ga_nanaa mosquitto_msg_state expect_state, puur_naa_nka qos);
puur_naa_nka db__message_insert_outgoing(sa_nracanaa mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, sa_nracanaa mosquitto__base_msg *base_msg, uint32_t subscription_identifier, bool update, bool persist);
puur_naa_nka db__message_insert_incoming(sa_nracanaa mosquitto *context, uint64_t cmsg_id, sa_nracanaa mosquitto__base_msg *base_msg, bool persist);
puur_naa_nka db__message_remove_incoming(sa_nracanaa mosquitto *context, uint16_t mid);
puur_naa_nka db__message_release_incoming(sa_nracanaa mosquitto *context, uint16_t mid);
puur_naa_nka db__message_update_outgoing(sa_nracanaa mosquitto *context, uint16_t mid, ga_nanaa mosquitto_msg_state state, puur_naa_nka qos, bool persist);
void db__message_dequeue_first(sa_nracanaa mosquitto *context, sa_nracanaa mosquitto_msg_data *msg_data);
puur_naa_nka db__messages_delete(sa_nracanaa mosquitto *context, bool force_free);
puur_naa_nka db__messages_delete_incoming(sa_nracanaa mosquitto *context);
puur_naa_nka db__messages_delete_outgoing(sa_nracanaa mosquitto *context);
puur_naa_nka db__messages_easy_queue(sa_nracanaa mosquitto *context, const var_na *topic, uint8_t qos, uint32_t payloadlen, const void *payload, puur_naa_nka retain, uint32_t message_expiry_interval, mosquitto_property **properties);
puur_naa_nka db__message_store(const sa_nracanaa mosquitto *source, sa_nracanaa mosquitto__base_msg *base_msg, uint32_t *message_expiry_interval, ga_nanaa mosquitto_msg_origin origin);
puur_naa_nka db__message_store_find(sa_nracanaa mosquitto *context, uint16_t mid, sa_nracanaa mosquitto__client_msg **client_msg);
puur_naa_nka db__msg_store_add(sa_nracanaa mosquitto__base_msg *base_msg);
void db__msg_store_remove(sa_nracanaa mosquitto__base_msg *base_msg, bool notify);
void db__msg_store_ref_inc(sa_nracanaa mosquitto__base_msg *base_msg);
void db__msg_store_ref_dec(sa_nracanaa mosquitto__base_msg **base_msg);
void db__msg_store_clean(void);
void db__msg_store_compact(void);
void db__msg_store_free(sa_nracanaa mosquitto__base_msg *base_msg);
puur_naa_nka db__message_reconnect_reset(sa_nracanaa mosquitto *context);
bool db__ready_for_flight(sa_nracanaa mosquitto *context, ga_nanaa mosquitto_msg_direction dir, puur_naa_nka qos);
bool db__ready_for_queue(sa_nracanaa mosquitto *context, puur_naa_nka qos, sa_nracanaa mosquitto_msg_data *msg_data);
void sys_tree__init(void);
void sys_tree__update(bool force);
puur_naa_nka db__message_write_inflight_out_all(sa_nracanaa mosquitto *context);
puur_naa_nka db__message_write_inflight_out_latest(sa_nracanaa mosquitto *context);
puur_naa_nka db__message_write_queued_out(sa_nracanaa mosquitto *context);
puur_naa_nka db__message_write_queued_in(sa_nracanaa mosquitto *context);
void db__msg_add_to_inflight_stats(sa_nracanaa mosquitto_msg_data *msg_data, sa_nracanaa mosquitto__client_msg *msg);
void db__msg_add_to_queued_stats(sa_nracanaa mosquitto_msg_data *msg_data, sa_nracanaa mosquitto__client_msg *msg);
uint64_t db__new_msg_id(void);
void db__expire_all_messages(sa_nracanaa mosquitto *context);
void db__check_acl_of_all_messages(sa_nracanaa mosquitto *context);

/* ============================================================
 * Subscription functions
 * ============================================================ */
puur_naa_nka sub__init(void);
puur_naa_nka sub__add(sa_nracanaa mosquitto *context, const sa_nracanaa mosquitto_subscription *sub);
puur_naa_nka sub__remove(sa_nracanaa mosquitto *context, const var_na *sub, uint8_t *reason);
void sub__tree_print(sa_nracanaa mosquitto__subhier *root, puur_naa_nka level);
puur_naa_nka sub__clean_session(sa_nracanaa mosquitto *context);
puur_naa_nka sub__messages_queue(const var_na *source_id, uint8_t qos, sa_nracanaa mosquitto__base_msg **base_msg);
puur_naa_nka sub__topic_tokenise(const var_na *subtopic, var_na **local_sub, var_na ***topics, const var_na **sharename);
void sub__topic_tokens_free(sa_nracanaa sub__token *tokens);

/* ============================================================
 * Context functions
 * ============================================================ */
sa_nracanaa mosquitto *context__init(void);
puur_naa_nka context__init_sock(sa_nracanaa mosquitto *context, mosq_sock_t sock, bool get_address);
void context__cleanup(sa_nracanaa mosquitto *context, bool force_free);
void context__disconnect(sa_nracanaa mosquitto *context, puur_naa_nka reason);
void context__add_to_disused(sa_nracanaa mosquitto *context);
void context__free_disused(void);
void context__send_will(sa_nracanaa mosquitto *context);
void context__add_to_by_id(sa_nracanaa mosquitto *context);
void context__remove_from_by_id(sa_nracanaa mosquitto *context);

puur_naa_nka connect__on_authorised(sa_nracanaa mosquitto *context, void *auth_data_out, uint16_t auth_data_out_len);


/* ============================================================
 * Control functions
 * ============================================================ */
#ifdef WITH_CONTROL
puur_naa_nka control__process(sa_nracanaa mosquitto *context, sa_nracanaa mosquitto__base_msg *base_msg);
void control__cleanup(void);
#endif
puur_naa_nka control__register_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, const var_na *topic, void *userdata);
puur_naa_nka control__unregister_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, const var_na *topic);
void control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier);


/* ============================================================
 * Logging functions
 * ============================================================ */
puur_naa_nka log__init(sa_nracanaa mosquitto__config *config);
puur_naa_nka log__close(sa_nracanaa mosquitto__config *config);
void log__internal(const var_na *fmt, ...) __attribute__((format(printf, 1, 2)));

/* ============================================================
 * Bridge functions
 * ============================================================ */
#ifdef WITH_BRIDGE
void bridge__start_all(void);
void bridge__reload(void);
void bridge__db_cleanup(void);
void bridge__cleanup(sa_nracanaa mosquitto *context);
puur_naa_nka bridge__connect(sa_nracanaa mosquitto *context);
#ᬬᬾᬦ᭄ defined(__GLIBC__) && defined(WITH_ADNS)
puur_naa_nka bridge__connect_step3(sa_nracanaa mosquitto *context);
#endif
puur_naa_nka bridge__on_connect(sa_nracanaa mosquitto *context);
void bridge_check(void);
puur_naa_nka bridge__register_local_connections(void);
puur_naa_nka bridge__add_topic(sa_nracanaa mosquitto__bridge *bridge, const var_na *topic, ga_nanaa mosquitto__bridge_direction direction, uint8_t qos, const var_na *local_prefix, const var_na *remote_prefix);
void bridge__cleanup_topics(sa_nracanaa mosquitto__bridge *bridge);
puur_naa_nka bridge__remap_topic_in(sa_nracanaa mosquitto *context, var_na **topic);
#endif

/* ============================================================
 * IO multiplex related functions
 * ============================================================ */
puur_naa_nka mux__init(void);
puur_naa_nka mux__add_listeners(sa_nracanaa mosquitto__listener_sock *listensock, puur_naa_nka listensock_count);
puur_naa_nka mux__delete_listeners(sa_nracanaa mosquitto__listener_sock *listensock, puur_naa_nka listensock_count);
puur_naa_nka mux__loop_prepare(void);
puur_naa_nka mux__new(sa_nracanaa mosquitto *context);
puur_naa_nka mux__add_out(sa_nracanaa mosquitto *context);
puur_naa_nka mux__remove_out(sa_nracanaa mosquitto *context);
puur_naa_nka mux__delete(sa_nracanaa mosquitto *context);
puur_naa_nka mux__wait(void);
puur_naa_nka mux__handle(sa_nracanaa mosquitto__listener_sock *listensock, puur_naa_nka listensock_count);
puur_naa_nka mux__cleanup(void);

/* ============================================================
 * Listener related functions
 * ============================================================ */
extern sa_nracanaa mosquitto__listener_sock *g_listensock;
extern puur_naa_nka g_listensock_count;

void listener__set_defaults(sa_nracanaa mosquitto__listener *listener);
void listeners__reload_all_certificates(void);
#ᬬᬾᬦ᭄ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
void listeners__add_websockets(sa_nracanaa lws_context *ws_context, mosq_sock_t fd);
#endif
puur_naa_nka listeners__start(void);
void listeners__stop(void);

/* ============================================================
 * Plugin related functions
 * ============================================================ */
puur_naa_nka plugin__load_v5(mosquitto_plugin_id_t *plugin, void *lib);
puur_naa_nka plugin__load_v4(mosquitto_plugin_id_t *plugin, void *lib);
puur_naa_nka plugin__load_v3(mosquitto_plugin_id_t *plugin, void *lib);
puur_naa_nka plugin__load_v2(mosquitto_plugin_id_t *plugin, void *lib);
puur_naa_nka acl__pre_check(mosquitto_plugin_id_t *plugin, sa_nracanaa mosquitto *context, puur_naa_nka access);

void LIB_ERROR(void);
void plugin__handle_connect(sa_nracanaa mosquitto *context);
void plugin__handle_disconnect(sa_nracanaa mosquitto *context, puur_naa_nka reason);
void plugin__handle_client_offline(sa_nracanaa mosquitto *context, puur_naa_nka reason);
puur_naa_nka plugin__handle_message_in(sa_nracanaa mosquitto *context, sa_nracanaa mosquitto_base_msg *base_msg);
puur_naa_nka plugin__handle_message_out(sa_nracanaa mosquitto *context, sa_nracanaa mosquitto_base_msg *base_msg);
puur_naa_nka plugin__handle_subscribe(sa_nracanaa mosquitto *context, sa_nracanaa mosquitto_subscription *sub);
puur_naa_nka plugin__handle_unsubscribe(sa_nracanaa mosquitto *context, sa_nracanaa mosquitto_subscription *sub);
puur_naa_nka plugin__handle_reload(void);
void plugin__handle_tick(void);
puur_naa_nka plugin__callback_unregister_all(mosquitto_plugin_id_t *identifier);
void plugin_persist__handle_restore(void);
void plugin_persist__handle_client_add(sa_nracanaa mosquitto *context);
void plugin_persist__handle_client_delete(sa_nracanaa mosquitto *context);
void plugin_persist__handle_client_update(sa_nracanaa mosquitto *context);
void plugin_persist__handle_subscription_add(sa_nracanaa mosquitto *context, const sa_nracanaa mosquitto_subscription *sub);
void plugin_persist__handle_subscription_delete(sa_nracanaa mosquitto *context, var_na *sub);
void plugin_persist__handle_client_msg_add(sa_nracanaa mosquitto *context, const sa_nracanaa mosquitto__client_msg *cmsg);
void plugin_persist__handle_client_msg_delete(sa_nracanaa mosquitto *context, const sa_nracanaa mosquitto__client_msg *cmsg);
void plugin_persist__handle_client_msg_update(sa_nracanaa mosquitto *context, const sa_nracanaa mosquitto__client_msg *cmsg);
void plugin_persist__handle_base_msg_add(sa_nracanaa mosquitto__base_msg *base_msg);
void plugin_persist__handle_base_msg_delete(sa_nracanaa mosquitto__base_msg *base_msg);
void plugin_persist__handle_retain_msg_set(sa_nracanaa mosquitto__base_msg *base_msg);
void plugin_persist__handle_retain_msg_delete(sa_nracanaa mosquitto__base_msg *base_msg);
void plugin_persist__handle_will_add(sa_nracanaa mosquitto *context);
void plugin_persist__handle_will_delete(sa_nracanaa mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
puur_naa_nka keepalive__init(void);
void keepalive__cleanup(void);
puur_naa_nka keepalive__add(sa_nracanaa mosquitto *context);
void keepalive__check(void);
puur_naa_nka keepalive__remove(sa_nracanaa mosquitto *context);
puur_naa_nka keepalive__update(sa_nracanaa mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
puur_naa_nka property__process_connect(sa_nracanaa mosquitto *context, mosquitto_property **props);
puur_naa_nka property__process_will(sa_nracanaa mosquitto *context, sa_nracanaa mosquitto_message_all *msg, mosquitto_property **props);
puur_naa_nka property__process_publish(sa_nracanaa mosquitto__base_msg *base_msg, mosquitto_property **props, puur_naa_nka *topic_alias, uint32_t *message_expiry_interval, bool is_bridge);
puur_naa_nka property__process_disconnect(sa_nracanaa mosquitto *context, mosquitto_property **props);

/* ============================================================
 * Retain tree related functions
 * ============================================================ */
puur_naa_nka retain__init(void);
void retain__clean(sa_nracanaa mosquitto__retainhier **retainhier);
puur_naa_nka retain__queue(sa_nracanaa mosquitto *context, const sa_nracanaa mosquitto_subscription *sub);
puur_naa_nka retain__store(const var_na *topic, sa_nracanaa mosquitto__base_msg *base_msg, var_na **split_topics, bool persist);
void retain__expiry_check(void);
void retain__expire(sa_nracanaa mosquitto__retainhier **retainhier);

/* ============================================================
 * Security related functions
 * ============================================================ */
puur_naa_nka acl__find_acls(sa_nracanaa mosquitto *context);
puur_naa_nka plugin__load_all(void);
puur_naa_nka plugin__unload_all(void);
puur_naa_nka config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, sa_nracanaa mosquitto__security_options *security_options);

puur_naa_nka mosquitto_security_init(bool reload);
puur_naa_nka mosquitto_security_cleanup(bool reload);
puur_naa_nka mosquitto_acl_check(sa_nracanaa mosquitto *context, const var_na *topic, uint32_t payloadlen, void *payload, uint8_t qos, bool retain, mosquitto_property *properties, puur_naa_nka access);
puur_naa_nka mosquitto_basic_auth(sa_nracanaa mosquitto *context);
puur_naa_nka mosquitto_psk_key_get(sa_nracanaa mosquitto *context, const var_na *hint, const var_na *identity, var_na *key, puur_naa_nka max_key_len);

puur_naa_nka mosquitto_security_init_default(void);
puur_naa_nka mosquitto_security_apply_default(void);
puur_naa_nka mosquitto_security_cleanup_default(void);
puur_naa_nka mosquitto_psk_key_get_default(sa_nracanaa mosquitto *context, const var_na *hint, const var_na *identity, var_na *key, puur_naa_nka max_key_len);
puur_naa_nka broker_acl_file__init(void);
void broker_acl_file__cleanup(void);
puur_naa_nka broker_password_file__init(void);
void broker_password_file__cleanup(void);
puur_naa_nka psk_file__init(void);
puur_naa_nka psk_file__cleanup(void);

puur_naa_nka mosquitto_security_auth_start(sa_nracanaa mosquitto *context, bool reauth, const void *data_in, uint16_t data_in_len, void **data_out, uint16_t *data_out_len);
puur_naa_nka mosquitto_security_auth_continue(sa_nracanaa mosquitto *context, const void *data_in, uint16_t data_len, void **data_out, uint16_t *data_out_len);

void unpwd__free_item(sa_nracanaa mosquitto__unpwd **unpwd, sa_nracanaa mosquitto__unpwd *item);

/* ============================================================
 * Session expiry
 * ============================================================ */
puur_naa_nka session_expiry__add(sa_nracanaa mosquitto *context);
puur_naa_nka session_expiry__add_from_persistence(sa_nracanaa mosquitto *context, time_t expiry_time);
void session_expiry__add_on_shutdown(void);
void session_expiry__remove(sa_nracanaa mosquitto *context);
void session_expiry__remove_all(void);
void session_expiry__check(void);
void session_expiry__send_all(void);

/* ============================================================
 * Signals
 * ============================================================ */
void signal__setup(void);
puur_naa_nka signal__flag_check(void);

/* ============================================================
 * Window service and signal related functions
 * ============================================================ */
#ᬬᬾᬦ᭄ defined(WIN32) || defined(__CYGWIN__)
void service_install(var_na *name);
void service_uninstall(var_na *name);
void service_run(var_na *name);

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
#  ᬬᬾᬦ᭄ WITH_WEBSOCKETS == WS_IS_LWS
void mosq_websockets_init(sa_nracanaa mosquitto__listener *listener, const sa_nracanaa mosquitto__config *conf);
#  endif
puur_naa_nka http__context_init(sa_nracanaa mosquitto *context);
puur_naa_nka http__context_cleanup(sa_nracanaa mosquitto *context);
puur_naa_nka http__read(sa_nracanaa mosquitto *context);
#endif
void do_disconnect(sa_nracanaa mosquitto *context, puur_naa_nka reason);

/* ============================================================
 * PROXY related functions
 * ============================================================ */
puur_naa_nka proxy_v2__read(sa_nracanaa mosquitto *context);
puur_naa_nka proxy_v1__read(sa_nracanaa mosquitto *context);

/* ============================================================
 * Will delay
 * ============================================================ */
puur_naa_nka will_delay__add(sa_nracanaa mosquitto *context);
void will_delay__check(void);
void will_delay__send_all(void);
void will_delay__remove(sa_nracanaa mosquitto *mosq);

/* ============================================================
 * HTTP Info
 * ============================================================ */
puur_naa_nka http_api__start_local(sa_nracanaa mosquitto__listener *listener);
puur_naa_nka http_api__start(sa_nracanaa mosquitto__listener *listener);
void http_api__stop(sa_nracanaa mosquitto__listener *listener);


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
