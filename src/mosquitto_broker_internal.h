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

#அந்தர்பாவ "config.h"
#அந்தர்பாவ <stdio.h>

#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#  எனில் LWS_LIBRARY_VERSION_எண் >= 3002000 && !defined(LWS_WITH_EXTERNAL_POLL)
#    warning "libwebsockets is not compiled with LWS_WITH_EXTERNAL_POLL support. Websocket performance will be unusable."
#  endif
#endif

#ifdef WITH_HTTP_API
#  include <microhttpd.h>
#endif

#ifdef __linux__
#define WITH_TCP_USER_TIMEOUT
#endif

#அந்தர்பாவ "mosquitto_internal.h"
#அந்தர்பாவ "mosquitto/broker.h"
#அந்தர்பாவ "mosquitto/broker_plugin.h"
#அந்தர்பாவ "mosquitto.h"
#அந்தர்பாவ "logging_mosq.h"
#அந்தர்பாவ "tls_mosq.h"
#அந்தர்பாவ "uthash.h"
#அந்தர்பாவ "acl_file.h"
#அந்தர்பாவ "password_file.h"

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

வகைவரை uint64_t dbid_t;

வகைவரை முழு (*FUNC_plugin_init_v5)(mosquitto_plugin_id_t *, வெறுமை **, கட்டமைப்பு mosquitto_opt *, முழு);
வகைவரை முழு (*FUNC_plugin_cleanup_v5)(வெறுமை *, கட்டமைப்பு mosquitto_opt *, முழு);

வகைவரை முழு (*FUNC_auth_plugin_init_v4)(வெறுமை **, கட்டமைப்பு mosquitto_opt *, முழு);
வகைவரை முழு (*FUNC_auth_plugin_cleanup_v4)(வெறுமை *, கட்டமைப்பு mosquitto_opt *, முழு);
வகைவரை முழு (*FUNC_auth_plugin_security_init_v4)(வெறுமை *, கட்டமைப்பு mosquitto_opt *, முழு, bool);
வகைவரை முழு (*FUNC_auth_plugin_security_cleanup_v4)(வெறுமை *, கட்டமைப்பு mosquitto_opt *, முழு, bool);
வகைவரை முழு (*FUNC_auth_plugin_acl_check_v4)(வெறுமை *, முழு, கட்டமைப்பு mosquitto *, கட்டமைப்பு mosquitto_acl_செய்தி *);
வகைவரை முழு (*FUNC_auth_plugin_unpwd_check_v4)(வெறுமை *, கட்டமைப்பு mosquitto *, மாறிலி எழுத்து *, மாறிலி எழுத்து *);
வகைவரை முழு (*FUNC_auth_plugin_psk_சாவி_get_v4)(வெறுமை *, கட்டமைப்பு mosquitto *, மாறிலி எழுத்து *, மாறிலி எழுத்து *, எழுத்து *, முழு);
வகைவரை முழு (*FUNC_auth_plugin_auth_start_v4)(வெறுமை *, கட்டமைப்பு mosquitto *, மாறிலி எழுத்து *, bool, மாறிலி வெறுமை *, uint16_t, வெறுமை **, uint16_t *);
வகைவரை முழு (*FUNC_auth_plugin_auth_continue_v4)(வெறுமை *, கட்டமைப்பு mosquitto *, மாறிலி எழுத்து *, மாறிலி வெறுமை *, uint16_t, வெறுமை **, uint16_t *);

வகைவரை முழு (*FUNC_auth_plugin_init_v3)(வெறுமை **, கட்டமைப்பு mosquitto_opt *, முழு);
வகைவரை முழு (*FUNC_auth_plugin_cleanup_v3)(வெறுமை *, கட்டமைப்பு mosquitto_opt *, முழு);
வகைவரை முழு (*FUNC_auth_plugin_security_init_v3)(வெறுமை *, கட்டமைப்பு mosquitto_opt *, முழு, bool);
வகைவரை முழு (*FUNC_auth_plugin_security_cleanup_v3)(வெறுமை *, கட்டமைப்பு mosquitto_opt *, முழு, bool);
வகைவரை முழு (*FUNC_auth_plugin_acl_check_v3)(வெறுமை *, முழு, மாறிலி கட்டமைப்பு mosquitto *, கட்டமைப்பு mosquitto_acl_செய்தி *);
வகைவரை முழு (*FUNC_auth_plugin_unpwd_check_v3)(வெறுமை *, மாறிலி கட்டமைப்பு mosquitto *, மாறிலி எழுத்து *, மாறிலி எழுத்து *);
வகைவரை முழு (*FUNC_auth_plugin_psk_சாவி_get_v3)(வெறுமை *, மாறிலி கட்டமைப்பு mosquitto *, மாறிலி எழுத்து *, மாறிலி எழுத்து *, எழுத்து *, முழு);

வகைவரை முழு (*FUNC_auth_plugin_init_v2)(வெறுமை **, கட்டமைப்பு mosquitto_auth_opt *, முழு);
வகைவரை முழு (*FUNC_auth_plugin_cleanup_v2)(வெறுமை *, கட்டமைப்பு mosquitto_auth_opt *, முழு);
வகைவரை முழு (*FUNC_auth_plugin_security_init_v2)(வெறுமை *, கட்டமைப்பு mosquitto_auth_opt *, முழு, bool);
வகைவரை முழு (*FUNC_auth_plugin_security_cleanup_v2)(வெறுமை *, கட்டமைப்பு mosquitto_auth_opt *, முழு, bool);
வகைவரை முழு (*FUNC_auth_plugin_acl_check_v2)(வெறுமை *, மாறிலி எழுத்து *, மாறிலி எழுத்து *, மாறிலி எழுத்து *, முழு);
வகைவரை முழு (*FUNC_auth_plugin_unpwd_check_v2)(வெறுமை *, மாறிலி எழுத்து *, மாறிலி எழுத்து *);
வகைவரை முழு (*FUNC_auth_plugin_psk_சாவி_get_v2)(வெறுமை *, மாறிலி எழுத்து *, மாறிலி எழுத்து *, எழுத்து *, முழு);


எண்ணிக்கை mosquitto_செய்தி_origin {
	mosq_mo_client = 0,
	mosq_mo_broker = 1,
};

கட்டமைப்பு mosquitto__plugin_lib {
	வெறுமை *lib;
	வெறுமை *user_data;
	முழு (*plugin_version)(வெறுமை);
	கட்டமைப்பு mosquitto_plugin_id_t *identifier;

	FUNC_plugin_init_v5 plugin_init_v5;
	FUNC_plugin_cleanup_v5 plugin_cleanup_v5;

	FUNC_auth_plugin_init_v4 plugin_init_v4;
	FUNC_auth_plugin_cleanup_v4 plugin_cleanup_v4;
	FUNC_auth_plugin_security_init_v4 security_init_v4;
	FUNC_auth_plugin_security_cleanup_v4 security_cleanup_v4;
	FUNC_auth_plugin_acl_check_v4 acl_check_v4;
	FUNC_auth_plugin_unpwd_check_v4 unpwd_check_v4;
	FUNC_auth_plugin_psk_சாவி_get_v4 psk_சாவி_get_v4;
	FUNC_auth_plugin_auth_start_v4 auth_start_v4;
	FUNC_auth_plugin_auth_continue_v4 auth_continue_v4;

	FUNC_auth_plugin_init_v3 plugin_init_v3;
	FUNC_auth_plugin_cleanup_v3 plugin_cleanup_v3;
	FUNC_auth_plugin_security_init_v3 security_init_v3;
	FUNC_auth_plugin_security_cleanup_v3 security_cleanup_v3;
	FUNC_auth_plugin_acl_check_v3 acl_check_v3;
	FUNC_auth_plugin_unpwd_check_v3 unpwd_check_v3;
	FUNC_auth_plugin_psk_சாவி_get_v3 psk_சாவி_get_v3;

	FUNC_auth_plugin_init_v2 plugin_init_v2;
	FUNC_auth_plugin_cleanup_v2 plugin_cleanup_v2;
	FUNC_auth_plugin_security_init_v2 security_init_v2;
	FUNC_auth_plugin_security_cleanup_v2 security_cleanup_v2;
	FUNC_auth_plugin_acl_check_v2 acl_check_v2;
	FUNC_auth_plugin_unpwd_check_v2 unpwd_check_v2;
	FUNC_auth_plugin_psk_சாவி_get_v2 psk_சாவி_get_v2;
	முழு version;
};

கட்டமைப்பு mosquitto__plugin_config {
	எழுத்து *path;
	எழுத்து *name;
	கட்டமைப்பு mosquitto_opt *options;
	கட்டமைப்பு mosquitto__security_options **security_options;
	முழு option_மொத்தம்;
	முழு security_option_மொத்தம்;
	bool deny_special_chars;
};

கட்டமைப்பு mosquitto__callback {
	UT_hash_handle hh; /* For callbacks that register for e.g. a specific topic */
	கட்டமைப்பு mosquitto__callback *next, *prev; /* For typical callbacks */
	MOSQ_FUNC_generic_callback cb;
	வெறுமை *userdata;
	ஒன்றிப்பு {
		எழுத்து *topic;
		கட்டமைப்பு timespec next_tick;
	} data;
	mosquitto_plugin_id_t *identifier;
};

கட்டமைப்பு plugin__callbacks {
	கட்டமைப்பு mosquitto__callback *tick;
	கட்டமைப்பு mosquitto__callback *acl_check;
	கட்டமைப்பு mosquitto__callback *basic_auth;
	கட்டமைப்பு mosquitto__callback *connect;
	கட்டமைப்பு mosquitto__callback *control;
	கட்டமைப்பு mosquitto__callback *disconnect;
	கட்டமைப்பு mosquitto__callback *client_offline;
	கட்டமைப்பு mosquitto__callback *ext_auth_continue;
	கட்டமைப்பு mosquitto__callback *ext_auth_start;
	கட்டமைப்பு mosquitto__callback *message_in;
	கட்டமைப்பு mosquitto__callback *message_out;
	கட்டமைப்பு mosquitto__callback *psk_சாவி;
	கட்டமைப்பு mosquitto__callback *reload;
	கட்டமைப்பு mosquitto__callback *subscribe;
	கட்டமைப்பு mosquitto__callback *unsubscribe;
	கட்டமைப்பு mosquitto__callback *persist_restore;
	கட்டமைப்பு mosquitto__callback *persist_client_add;
	கட்டமைப்பு mosquitto__callback *persist_client_delete;
	கட்டமைப்பு mosquitto__callback *persist_client_update;
	கட்டமைப்பு mosquitto__callback *persist_subscription_add;
	கட்டமைப்பு mosquitto__callback *persist_subscription_delete;
	கட்டமைப்பு mosquitto__callback *persist_client_செய்தி_add;
	கட்டமைப்பு mosquitto__callback *persist_client_செய்தி_delete;
	கட்டமைப்பு mosquitto__callback *persist_client_செய்தி_update;
	கட்டமைப்பு mosquitto__callback *persist_base_செய்தி_add;
	கட்டமைப்பு mosquitto__callback *persist_base_செய்தி_delete;
	கட்டமைப்பு mosquitto__callback *persist_retain_செய்தி_set;
	கட்டமைப்பு mosquitto__callback *persist_retain_செய்தி_delete;
	கட்டமைப்பு mosquitto__callback *persist_will_add;
	கட்டமைப்பு mosquitto__callback *persist_will_delete;
};

/* This is owned by mosquitto__config or mosquitto__listener, and only referred
 * to by other structs */
கட்டமைப்பு mosquitto__security_options {
	/* Any options that get added here also need considering
	 * in config__read() with regards whether allow_anonymous
	 * should be disabled when these options are set.
	 */
	கட்டமைப்பு mosquitto__unpwd *unpwd;
	கட்டமைப்பு mosquitto__psk *psk_id;
	கட்டமைப்பு acl_file_data acl_data;
	கட்டமைப்பு password_file_data password_data;
	எழுத்து *psk_file;
	mosquitto_plugin_id_t **plugins;
	முழு plugin_மொத்தம்;
	int8_t allow_anonymous;
	bool allow_zero_length_clientid;
	எழுத்து *auto_id_prefix;
	uint16_t auto_id_prefix_len;
	கட்டமைப்பு plugin__callbacks plugin_callbacks;
	mosquitto_plugin_id_t *pid; /* For registering as a "plugin" */
};

#எனில் defined(WITH_EPOLL) || defined(WITH_KQUEUE)
எண்ணிக்கை struct_ident {
	id_invalid = 0,
	id_listener = 1,
	id_client = 2,
	id_listener_ws = 3,
};
#endif

கட்டமைப்பு mosquitto__listener {
	uint16_t port;
	எழுத்து *host;
	எழுத்து *bind_interface;
	முழு max_connections;
	எழுத்து *mount_point;
	mosq_sock_t *socks;
	முழு sock_மொத்தம்;
	முழு client_மொத்தம்;
	எண்ணிக்கை mosquitto_protocol protocol;
	முழு socket_domain;
	bool use_username_as_clientid;
	uint8_t max_qos;
	uint16_t max_topic_alias;
	uint16_t max_topic_alias_broker;
#ifdef WITH_TLS
	எழுத்து *cafile;
	எழுத்து *capath;
	எழுத்து *certfile;
	எழுத்து *keyfile;
	எழுத்து *tls_engine;
	எழுத்து *tls_engine_kpass_sha1;
	எழுத்து *ciphers;
	எழுத்து *ciphers_tls13;
	எழுத்து *psk_hint;
	SSL_CTX *ssl_ctx;
	எழுத்து *crlfile;
	எழுத்து *tls_version;
	bool use_identity_as_username;
	bool use_subject_as_username;
	bool require_certificate;
	bool disable_client_cert_date_checks;
	எண்ணிக்கை mosquitto__keyform tls_keyform;
#endif
#ifdef WITH_WEBSOCKETS
#  எனில் WITH_WEBSOCKETS == WS_IS_LWS
	கட்டமைப்பு lws_context *ws_context;
	bool ws_in_init;
	கட்டமைப்பு lws_protocols *ws_protocol;
#  endif
	எழுத்து **ws_origins;
	முழு ws_origin_மொத்தம்;
#endif
#எனில் defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
	எழுத்து *http_dir;
#endif
	கட்டமைப்பு mosquitto__security_options *security_options;
#ifdef WITH_UNIX_SOCKETS
	எழுத்து *unix_socket_path;
#endif
	bool disable_protocol_v3;
	bool disable_protocol_v4;
	bool disable_protocol_v5;
	முழு enable_proxy_protocol;
	bool proxy_protocol_v2_require_tls;
#ifdef WITH_HTTP_API
	கட்டமைப்பு MHD_Daemon *mhd;
#endif
};


கட்டமைப்பு mosquitto__listener_sock {
#எனில் defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	/* This *must* be the first element in the struct. */
	முழு ident;
#endif
	mosq_sock_t sock;
	கட்டமைப்பு mosquitto__listener *listener;
};


/* Callbacks belonging to a specific plugin
 * Doesn't include MOSQ_EVT_CONTROL events.
 */
கட்டமைப்பு plugin_own_callback {
	கட்டமைப்பு plugin_own_callback *next, *prev;
	MOSQ_FUNC_generic_callback cb_func;
	எண்ணிக்கை mosquitto_plugin_event event;
};

கட்டமைப்பு mosquitto_plugin_id_t {
	கட்டமைப்பு mosquitto__plugin_config config;
	கட்டமைப்பு mosquitto__plugin_lib lib;
	கட்டமைப்பு mosquitto__listener *listener;
	எழுத்து *plugin_name;
	எழுத்து *plugin_version;
	கட்டமைப்பு control_endpoint *control_endpoints;
	கட்டமைப்பு plugin_own_callback *own_callbacks;
	கட்டமைப்பு timespec next_tick;
};

கட்டமைப்பு mosquitto__config {
	bool allow_duplicate_messages;
	முழு autosave_interval;
	bool autosave_on_changes;
	bool check_retain_source;
	எழுத்து *clientid_prefixes;
	bool connection_messages;
	uint16_t cmd_port[CMD_PORT_LIMIT];
	முழு cmd_port_மொத்தம்;
	bool daemon;
	bool test_configuration;
	bool enable_control_api;
	முழு global_max_clients;
	முழு global_max_connections;
	கட்டமைப்பு mosquitto__listener *default_listener; /* Points to one of `listeners` */
	கட்டமைப்பு mosquitto__listener *listeners;
	முழு listener_மொத்தம்;
	bool local_only;
	unsigned முழு log_dest;
	முழு log_facility;
	unsigned முழு log_type;
	bool log_timestamp;
	எழுத்து *log_timestamp_format;
	எழுத்து *log_file;
	FILE *log_fptr;
	size_t max_inflight_bytes;
	size_t max_queued_bytes;
	முழு max_queued_messages;
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
	எழுத்து *persistence_location;
	எழுத்து *persistence_file;
	எழுத்து *persistence_filepath;
	time_t persistent_client_expiration;
	எழுத்து *pid_file;
	bool queue_qos0_messages;
	bool per_listener_settings;
	bool retain_available;
	முழு retain_expiry_interval;
	bool set_tcp_nodelay;
	முழு sys_interval;
	bool upgrade_outgoing_qos;
	எழுத்து *user;
#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	முழு websockets_log_level;
#endif
	uint16_t packet_buffer_size;
#ifdef WITH_BRIDGE
	கட்டமைப்பு mosquitto__bridge **bridges;
	முழு bridge_மொத்தம்;
#endif
	கட்டமைப்பு mosquitto__security_options security_options;
};


கட்டமைப்பு mosquitto__subshared {
	UT_hash_handle hh;
	கட்டமைப்பு mosquitto__subleaf *subs;
	எழுத்து name[];
};

கட்டமைப்பு mosquitto__subhier {
	UT_hash_handle hh;
	கட்டமைப்பு mosquitto__subhier *parent;
	கட்டமைப்பு mosquitto__subhier *children;
	கட்டமைப்பு mosquitto__subleaf *subs;
	கட்டமைப்பு mosquitto__subshared *shared;
	uint16_t topic_len;
	எழுத்து topic[];
};

கட்டமைப்பு mosquitto__subleaf {
	கட்டமைப்பு mosquitto__subleaf *prev;
	கட்டமைப்பு mosquitto__subleaf *next;
	கட்டமைப்பு mosquitto *context;
	கட்டமைப்பு mosquitto__subhier *hier;
	கட்டமைப்பு mosquitto__subshared *shared;
	uint32_t identifier;
	uint8_t subscription_options;
	எழுத்து topic_filter[];
};

கட்டமைப்பு sub__token {
	கட்டமைப்பு sub__token *next;
	uint16_t topic_len;
	எழுத்து topic[];
};

கட்டமைப்பு mosquitto__retainhier {
	UT_hash_handle hh;
	கட்டமைப்பு mosquitto__retainhier *parent;
	கட்டமைப்பு mosquitto__retainhier *children;
	கட்டமைப்பு mosquitto_base_செய்தி *retained;
	uint16_t topic_len;
	எழுத்து topic[];
};

கட்டமைப்பு mosquitto_base_செய்தி {
	UT_hash_handle hh;
	கட்டமைப்பு mosquitto_base_செய்தி_2 data;
	கட்டமைப்பு mosquitto__listener *source_listener;
	எழுத்து **dest_ids;
	முழு dest_id_மொத்தம்;
	முழு ref_மொத்தம்;
	எண்ணிக்கை mosquitto_செய்தி_origin origin;
	bool stored;
};

கட்டமைப்பு mosquitto_client_செய்தி {
	கட்டமைப்பு mosquitto_client_செய்தி_2 data;
	கட்டமைப்பு mosquitto_client_செய்தி *prev;
	கட்டமைப்பு mosquitto_client_செய்தி *next;
	கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி;
};


கட்டமைப்பு mosquitto__psk {
	UT_hash_handle hh;
	எழுத்து *username;
	எழுத்து *password;
};

கட்டமைப்பு mosquitto__message_v5 {
	கட்டமைப்பு mosquitto__message_v5 *next, *prev;
	எழுத்து *topic;
	வெறுமை *payload;
	mosquitto_property *properties;
	/* clientid is used only by mosquitto_broker_publish*() to indicate
	   this message is for a specific client. */
	எழுத்து *clientid;
	முழு payloadlen;
	முழு qos;
	bool retain;
};

கட்டமைப்பு mosquitto_db {
	dbid_t last_db_id;
	uint64_t முனை_id_shifted;
	கட்டமைப்பு mosquitto__subhier *normal_subs;
	கட்டமைப்பு mosquitto__subhier *shared_subs;
	கட்டமைப்பு mosquitto__retainhier *retains;
	கட்டமைப்பு mosquitto *contexts_by_id;
	கட்டமைப்பு mosquitto *contexts_by_sock;
	கட்டமைப்பு mosquitto *contexts_by_id_delayed_auth;
	கட்டமைப்பு mosquitto *contexts_for_free;
	mosquitto_plugin_id_t **plugins;
	முழு plugin_மொத்தம்;
#ifdef WITH_BRIDGE
	கட்டமைப்பு mosquitto **bridges;
	முழு bridge_மொத்தம்;
#endif
	கட்டமைப்பு clientid__index_hash *clientid_index_hash;
	கட்டமைப்பு mosquitto_base_செய்தி *செய்தி_store;
	time_t now_s; /* Monotonic clock, where possible */
	time_t now_real_s; /* Read clock, for measuring session/message expiry */
	uint64_t முனை_id; /* for unique db ids */
	time_t next_event_ms; /* for mux timeout */
	முழு செய்தி_store_மொத்தம்;
	unsigned long செய்தி_store_bytes;
	எழுத்து *config_file;
	கட்டமைப்பு mosquitto__config *config;
	bool quiet;
	bool verbose;
#ifdef WITH_SYS_மரம்
	முழு subscription_மொத்தம்;
	முழு shared_subscription_மொத்தம்;
	முழு retained_மொத்தம்;
#endif
	முழு persistence_changes;
	கட்டமைப்பு mosquitto *ll_for_free;
#ifdef WITH_EPOLL
	முழு epollfd;
#endif
#ifdef WITH_KQUEUE
	முழு kqueuefd;
#endif
	கட்டமைப்பு mosquitto__message_v5 *plugin_msgs;
#ifdef WITH_TLS
	/* tls_keylog can't be in the config struct because it is used
	   before the config is allocated. Config probably
	   shouldn't be separately allocated. */
	எழுத்து *tls_keylog;
#endif
	bool shutdown;
};

எண்ணிக்கை mosquitto__bridge_direction {
	bd_out = 0,
	bd_in = 1,
	bd_both = 2,
};

எண்ணிக்கை mosquitto_bridge_start_type {
	bst_automatic = 0,
	bst_lazy = 1,
	bst_manual = 2,
	bst_once = 3,
};

எண்ணிக்கை mosquitto_bridge_reload_type {
	brt_lazy = 0,
	brt_immediate = 1,
};

கட்டமைப்பு mosquitto__bridge_topic {
	கட்டமைப்பு mosquitto__bridge_topic *next;
	எழுத்து *topic;
	எழுத்து *local_prefix;
	எழுத்து *remote_prefix;
	எழுத்து *local_topic; /* topic prefixed with local_prefix */
	எழுத்து *remote_topic; /* topic prefixed with remote_prefix */
	எண்ணிக்கை mosquitto__bridge_direction direction;
	uint8_t qos;
};

கட்டமைப்பு bridge_address {
	எழுத்து *address;
	uint16_t port;
};

கட்டமைப்பு mosquitto__bridge {
	எழுத்து *name;
	கட்டமைப்பு bridge_address *addresses;
	முழு cur_address;
	முழு address_மொத்தம்;
	time_t primary_retry;
	mosq_sock_t primary_retry_sock;
	bool round_robin;
	bool try_private;
	bool try_private_accepted;
	bool clean_start;
	int8_t clean_start_local;
	uint16_t keepalive;
	unsigned முழு tcp_keepalive_idle;
	unsigned முழு tcp_keepalive_interval;
	unsigned முழு tcp_keepalive_counter;
#ifdef WITH_TCP_USER_TIMEOUT
	முழு tcp_user_timeout;
#endif
	கட்டமைப்பு mosquitto__bridge_topic *topics;
	முழு topic_மொத்தம்;
	bool topic_remapping;
	எண்ணிக்கை mosquitto__protocol protocol_version;
	time_t restart_t;
	time_t connected_at;
	எழுத்து *remote_clientid;
	எழுத்து *remote_username;
	எழுத்து *remote_password;
	எழுத்து *local_clientid;
	எழுத்து *local_username;
	எழுத்து *local_password;
	எழுத்து *notification_topic;
	எழுத்து *bind_address;
	bool notifications;
	bool notifications_local_only;
	எண்ணிக்கை mosquitto_bridge_start_type start_type;
	முழு idle_timeout;
	முழு restart_timeout;
	முழு backoff_base;
	முழு backoff_cap;
	முழு stable_connection_period;
	முழு threshold;
	uint32_t maximum_packet_size;
	uint32_t session_expiry_interval;
	uint16_t receive_maximum;
	bool lazy_reconnect;
	bool attempt_unsubscribe;
	bool initial_notification_done;
	bool outgoing_retain;
	எண்ணிக்கை mosquitto_bridge_reload_type reload_type;
	uint16_t max_topic_alias;
#ifdef WITH_TLS
	bool tls_insecure;
	bool tls_ocsp_required;
	bool tls_use_os_certs;
	எழுத்து *tls_cafile;
	எழுத்து *tls_capath;
	எழுத்து *tls_certfile;
	எழுத்து *tls_keyfile;
	எழுத்து *tls_version;
	எழுத்து *tls_alpn;
	எழுத்து *tls_ciphers;
	எழுத்து *tls_13_ciphers;
#  ifdef FINAL_WITH_TLS_PSK
	எழுத்து *tls_psk_identity;
	எழுத்து *tls_psk;
#  endif
#endif
};

#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
கட்டமைப்பு libws_mqtt_hack {
	எழுத்து *http_dir;
	கட்டமைப்பு mosquitto__listener *listener;
};

கட்டமைப்பு libws_mqtt_data {
	கட்டமைப்பு mosquitto *mosq;
};
#endif

#அந்தர்பாவ <net_mosq.h>


கட்டமைப்பு control_endpoint {
	கட்டமைப்பு control_endpoint *next, *prev;
	எழுத்து topic[];
};

வெளி கட்டமைப்பு mosquitto_db db;

/* ============================================================
 * Main functions
 * ============================================================ */
முழு mosquitto_main_loop(கட்டமைப்பு mosquitto__listener_sock *listensock, முழு listensock_மொத்தம்);
வெறுமை loop__update_next_event(time_t new_ms);

/* ============================================================
 * Config functions
 * ============================================================ */
/* Initialise config struct to default values. */
வெறுமை config__init(கட்டமைப்பு mosquitto__config *config);
#ifdef WITH_BRIDGE
வெறுமை config__bridge_cleanup(கட்டமைப்பு mosquitto__bridge *bridge);
#endif
/* Parse command line options into config. */
முழு config__parse_args(கட்டமைப்பு mosquitto__config *config, முழு argc, எழுத்து *argv[]);
/* Read configuration data from config->config_file into config.
 * If reload is true, don't process config options that shouldn't be reloaded (listeners etc)
 * Returns 0 on success, 1 if there is a configuration error or if a file cannot be opened.
 */
முழு config__read(கட்டமைப்பு mosquitto__config *config, bool reload);
/* Free all config data. */
வெறுமை config__cleanup(கட்டமைப்பு mosquitto__config *config);
முழு config__get_dir_files(மாறிலி எழுத்து *include_dir, எழுத்து ***files, முழு *file_மொத்தம்);

/* ============================================================
 * Server send functions
 * ============================================================ */
முழு send__connack(கட்டமைப்பு mosquitto *context, uint8_t ack, uint8_t reason_code, மாறிலி mosquitto_property *properties);
முழு send__suback(கட்டமைப்பு mosquitto *context, uint16_t mid, uint32_t payloadlen, மாறிலி வெறுமை *payload);
முழு send__unsuback(கட்டமைப்பு mosquitto *context, uint16_t mid, முழு reason_code_மொத்தம், uint8_t *reason_codes, மாறிலி mosquitto_property *properties);
முழு send__auth(கட்டமைப்பு mosquitto *context, uint8_t reason_code, மாறிலி வெறுமை *auth_data, uint16_t auth_data_len);

/* ============================================================
 * Network functions
 * ============================================================ */
வெறுமை net__broker_init(வெறுமை);
வெறுமை net__broker_cleanup(வெறுமை);
கட்டமைப்பு mosquitto *net__socket_accept(கட்டமைப்பு mosquitto__listener_sock *listensock);
முழு net__socket_listen(கட்டமைப்பு mosquitto__listener *listener);
முழு net__socket_get_address(mosq_sock_t sock, எழுத்து *buf, size_t len, uint16_t *remote_address);
முழு net__tls_load_verify(கட்டமைப்பு mosquitto__listener *listener);
முழு net__tls_server_ctx(கட்டமைப்பு mosquitto__listener *listener);
முழு net__load_certificates(கட்டமைப்பு mosquitto__listener *listener);

/* ============================================================
 * Read handling functions
 * ============================================================ */
முழு handle__packet(கட்டமைப்பு mosquitto *context);
முழு handle__connack(கட்டமைப்பு mosquitto *context);
முழு handle__connect(கட்டமைப்பு mosquitto *context);
முழு handle__disconnect(கட்டமைப்பு mosquitto *context);
முழு handle__publish(கட்டமைப்பு mosquitto *context);
முழு handle__subscribe(கட்டமைப்பு mosquitto *context);
முழு handle__unsubscribe(கட்டமைப்பு mosquitto *context);
முழு handle__auth(கட்டமைப்பு mosquitto *context);

/* ============================================================
 * Database handling
 * ============================================================ */
முழு db__open(கட்டமைப்பு mosquitto__config *config);
முழு db__close(வெறுமை);
#ifdef WITH_PERSISTENCE
முழு persist__backup(bool shutdown);
முழு persist__restore(வெறுமை);
#endif
/* Return the number of in-flight messages in count. */
முழு db_message_மொத்தம்(முழு *மொத்தம்);
முழு db__message_delete_outgoing(கட்டமைப்பு mosquitto *context, uint16_t mid, எண்ணிக்கை mosquitto_செய்தி_state expect_state, முழு qos);
முழு db_message_செருகு_outgoing(கட்டமைப்பு mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி, uint32_t subscription_identifier, bool update, bool persist);
முழு db_message_செருகு_incoming(கட்டமைப்பு mosquitto *context, uint64_t cmsg_id, கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி, bool persist);
முழு db__message_remove_incoming(கட்டமைப்பு mosquitto *context, uint16_t mid);
முழு db__message_release_incoming(கட்டமைப்பு mosquitto *context, uint16_t mid);
முழு db__message_update_outgoing(கட்டமைப்பு mosquitto *context, uint16_t mid, எண்ணிக்கை mosquitto_செய்தி_state state, முழு qos, bool persist);
வெறுமை db__message_dequeue_first(கட்டமைப்பு mosquitto *context, கட்டமைப்பு mosquitto_செய்தி_data *செய்தி_data);
முழு db__messages_delete(கட்டமைப்பு mosquitto *context, bool force_free);
முழு db__messages_delete_incoming(கட்டமைப்பு mosquitto *context);
முழு db__messages_delete_outgoing(கட்டமைப்பு mosquitto *context);
முழு db__messages_easy_queue(கட்டமைப்பு mosquitto *context, மாறிலி எழுத்து *topic, uint8_t qos, uint32_t payloadlen, மாறிலி வெறுமை *payload, முழு retain, uint32_t message_expiry_interval, mosquitto_property **properties);
முழு db__message_store(மாறிலி கட்டமைப்பு mosquitto *source, கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி, uint32_t *message_expiry_interval, எண்ணிக்கை mosquitto_செய்தி_origin origin);
முழு db__message_store_find(கட்டமைப்பு mosquitto *context, uint16_t mid, கட்டமைப்பு mosquitto_client_செய்தி **client_செய்தி);
முழு db_செய்தி_store_add(கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி);
வெறுமை db_செய்தி_store_remove(கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி, bool notify);
வெறுமை db_செய்தி_store_ref_inc(கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி);
வெறுமை db_செய்தி_store_ref_dec(கட்டமைப்பு mosquitto_base_செய்தி **base_செய்தி);
வெறுமை db_செய்தி_store_clean(வெறுமை);
வெறுமை db_செய்தி_store_compact(வெறுமை);
வெறுமை db_செய்தி_store_free(கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி);
முழு db__message_reconnect_reset(கட்டமைப்பு mosquitto *context);
bool db__ready_for_flight(கட்டமைப்பு mosquitto *context, எண்ணிக்கை mosquitto_செய்தி_direction dir, முழு qos);
bool db__ready_for_queue(கட்டமைப்பு mosquitto *context, முழு qos, கட்டமைப்பு mosquitto_செய்தி_data *செய்தி_data);
வெறுமை sys_மரம்_init(வெறுமை);
வெறுமை sys_மரம்_update(bool force);
முழு db__message_write_inflight_out_all(கட்டமைப்பு mosquitto *context);
முழு db__message_write_inflight_out_latest(கட்டமைப்பு mosquitto *context);
முழு db__message_write_queued_out(கட்டமைப்பு mosquitto *context);
முழு db__message_write_queued_in(கட்டமைப்பு mosquitto *context);
வெறுமை db_செய்தி_add_to_inflight_stats(கட்டமைப்பு mosquitto_செய்தி_data *செய்தி_data, கட்டமைப்பு mosquitto_client_செய்தி *செய்தி);
வெறுமை db_செய்தி_add_to_queued_stats(கட்டமைப்பு mosquitto_செய்தி_data *செய்தி_data, கட்டமைப்பு mosquitto_client_செய்தி *செய்தி);
uint64_t db_new_செய்தி_id(வெறுமை);
வெறுமை db__expire_all_messages(கட்டமைப்பு mosquitto *context);
வெறுமை db__check_acl_of_all_messages(கட்டமைப்பு mosquitto *context);

/* ============================================================
 * Subscription functions
 * ============================================================ */
முழு sub__init(வெறுமை);
முழு sub__add(கட்டமைப்பு mosquitto *context, மாறிலி கட்டமைப்பு mosquitto_subscription *sub);
முழு sub__remove(கட்டமைப்பு mosquitto *context, மாறிலி எழுத்து *sub, uint8_t *reason);
வெறுமை sub_மரம்_print(கட்டமைப்பு mosquitto__subhier *வேர், முழு level);
முழு sub__clean_session(கட்டமைப்பு mosquitto *context);
முழு sub__messages_queue(மாறிலி எழுத்து *source_id, uint8_t qos, கட்டமைப்பு mosquitto_base_செய்தி **base_செய்தி);
முழு sub__topic_tokenise(மாறிலி எழுத்து *subtopic, எழுத்து **local_sub, எழுத்து ***topics, மாறிலி எழுத்து **sharename);
வெறுமை sub__topic_tokens_free(கட்டமைப்பு sub__token *tokens);

/* ============================================================
 * Context functions
 * ============================================================ */
கட்டமைப்பு mosquitto *context__init(வெறுமை);
முழு context__init_sock(கட்டமைப்பு mosquitto *context, mosq_sock_t sock, bool get_address);
வெறுமை context__cleanup(கட்டமைப்பு mosquitto *context, bool force_free);
வெறுமை context__disconnect(கட்டமைப்பு mosquitto *context, முழு reason);
வெறுமை context__add_to_disused(கட்டமைப்பு mosquitto *context);
வெறுமை context__free_disused(வெறுமை);
வெறுமை context__send_will(கட்டமைப்பு mosquitto *context);
வெறுமை context__add_to_by_id(கட்டமைப்பு mosquitto *context);
வெறுமை context__remove_from_by_id(கட்டமைப்பு mosquitto *context);

முழு connect__on_authorised(கட்டமைப்பு mosquitto *context, வெறுமை *auth_data_out, uint16_t auth_data_out_len);


/* ============================================================
 * Control functions
 * ============================================================ */
#ifdef WITH_CONTROL
முழு control__process(கட்டமைப்பு mosquitto *context, கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி);
வெறுமை control__cleanup(வெறுமை);
#endif
முழு control__register_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, மாறிலி எழுத்து *topic, வெறுமை *userdata);
முழு control__unregister_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, மாறிலி எழுத்து *topic);
வெறுமை control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier);


/* ============================================================
 * Logging functions
 * ============================================================ */
முழு log__init(கட்டமைப்பு mosquitto__config *config);
முழு log__close(கட்டமைப்பு mosquitto__config *config);
வெறுமை log__internal(மாறிலி எழுத்து *fmt, ...) __attribute__((format(printf, 1, 2)));

/* ============================================================
 * Bridge functions
 * ============================================================ */
#ifdef WITH_BRIDGE
வெறுமை bridge__start_all(வெறுமை);
வெறுமை bridge__reload(வெறுமை);
வெறுமை bridge__db_cleanup(வெறுமை);
வெறுமை bridge__cleanup(கட்டமைப்பு mosquitto *context);
முழு bridge__connect(கட்டமைப்பு mosquitto *context);
#எனில் defined(__GLIBC__) && defined(WITH_ADNS)
முழு bridge__connect_step3(கட்டமைப்பு mosquitto *context);
#endif
முழு bridge__on_connect(கட்டமைப்பு mosquitto *context);
வெறுமை bridge_check(வெறுமை);
முழு bridge__register_local_connections(வெறுமை);
முழு bridge__add_topic(கட்டமைப்பு mosquitto__bridge *bridge, மாறிலி எழுத்து *topic, எண்ணிக்கை mosquitto__bridge_direction direction, uint8_t qos, மாறிலி எழுத்து *local_prefix, மாறிலி எழுத்து *remote_prefix);
வெறுமை bridge__cleanup_topics(கட்டமைப்பு mosquitto__bridge *bridge);
முழு bridge__remap_topic_in(கட்டமைப்பு mosquitto *context, எழுத்து **topic);
#endif

/* ============================================================
 * IO multiplex related functions
 * ============================================================ */
முழு mux__init(வெறுமை);
முழு mux__add_listeners(கட்டமைப்பு mosquitto__listener_sock *listensock, முழு listensock_மொத்தம்);
முழு mux__delete_listeners(கட்டமைப்பு mosquitto__listener_sock *listensock, முழு listensock_மொத்தம்);
முழு mux__loop_prepare(வெறுமை);
முழு mux__new(கட்டமைப்பு mosquitto *context);
முழு mux__add_out(கட்டமைப்பு mosquitto *context);
முழு mux__remove_out(கட்டமைப்பு mosquitto *context);
முழு mux__delete(கட்டமைப்பு mosquitto *context);
முழு mux__wait(வெறுமை);
முழு mux__handle(கட்டமைப்பு mosquitto__listener_sock *listensock, முழு listensock_மொத்தம்);
முழு mux__cleanup(வெறுமை);

/* ============================================================
 * Listener related functions
 * ============================================================ */
வெளி கட்டமைப்பு mosquitto__listener_sock *g_listensock;
வெளி முழு g_listensock_மொத்தம்;

வெறுமை listener__set_defaults(கட்டமைப்பு mosquitto__listener *listener);
வெறுமை listeners__reload_all_certificates(வெறுமை);
#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
வெறுமை listeners__add_websockets(கட்டமைப்பு lws_context *ws_context, mosq_sock_t fd);
#endif
முழு listeners__start(வெறுமை);
வெறுமை listeners__stop(வெறுமை);

/* ============================================================
 * Plugin related functions
 * ============================================================ */
முழு plugin__load_v5(mosquitto_plugin_id_t *plugin, வெறுமை *lib);
முழு plugin__load_v4(mosquitto_plugin_id_t *plugin, வெறுமை *lib);
முழு plugin__load_v3(mosquitto_plugin_id_t *plugin, வெறுமை *lib);
முழு plugin__load_v2(mosquitto_plugin_id_t *plugin, வெறுமை *lib);
முழு acl__pre_check(mosquitto_plugin_id_t *plugin, கட்டமைப்பு mosquitto *context, முழு access);

வெறுமை LIB_ERROR(வெறுமை);
வெறுமை plugin__handle_connect(கட்டமைப்பு mosquitto *context);
வெறுமை plugin__handle_disconnect(கட்டமைப்பு mosquitto *context, முழு reason);
வெறுமை plugin__handle_client_offline(கட்டமைப்பு mosquitto *context, முழு reason);
முழு plugin__handle_message_in(கட்டமைப்பு mosquitto *context, கட்டமைப்பு mosquitto_base_செய்தி_2 *base_செய்தி);
முழு plugin__handle_message_out(கட்டமைப்பு mosquitto *context, கட்டமைப்பு mosquitto_base_செய்தி_2 *base_செய்தி);
முழு plugin__handle_subscribe(கட்டமைப்பு mosquitto *context, கட்டமைப்பு mosquitto_subscription *sub);
முழு plugin__handle_unsubscribe(கட்டமைப்பு mosquitto *context, கட்டமைப்பு mosquitto_subscription *sub);
முழு plugin__handle_reload(வெறுமை);
வெறுமை plugin__handle_tick(வெறுமை);
முழு plugin__callback_unregister_all(mosquitto_plugin_id_t *identifier);
வெறுமை plugin_persist__handle_restore(வெறுமை);
வெறுமை plugin_persist__handle_client_add(கட்டமைப்பு mosquitto *context);
வெறுமை plugin_persist__handle_client_delete(கட்டமைப்பு mosquitto *context);
வெறுமை plugin_persist__handle_client_update(கட்டமைப்பு mosquitto *context);
வெறுமை plugin_persist__handle_subscription_add(கட்டமைப்பு mosquitto *context, மாறிலி கட்டமைப்பு mosquitto_subscription *sub);
வெறுமை plugin_persist__handle_subscription_delete(கட்டமைப்பு mosquitto *context, எழுத்து *sub);
வெறுமை plugin_persist_handle_client_செய்தி_add(கட்டமைப்பு mosquitto *context, மாறிலி கட்டமைப்பு mosquitto_client_செய்தி *cmsg);
வெறுமை plugin_persist_handle_client_செய்தி_delete(கட்டமைப்பு mosquitto *context, மாறிலி கட்டமைப்பு mosquitto_client_செய்தி *cmsg);
வெறுமை plugin_persist_handle_client_செய்தி_update(கட்டமைப்பு mosquitto *context, மாறிலி கட்டமைப்பு mosquitto_client_செய்தி *cmsg);
வெறுமை plugin_persist_handle_base_செய்தி_add(கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி);
வெறுமை plugin_persist_handle_base_செய்தி_delete(கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி);
வெறுமை plugin_persist_handle_retain_செய்தி_set(கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி);
வெறுமை plugin_persist_handle_retain_செய்தி_delete(கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி);
வெறுமை plugin_persist__handle_will_add(கட்டமைப்பு mosquitto *context);
வெறுமை plugin_persist__handle_will_delete(கட்டமைப்பு mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
முழு keepalive__init(வெறுமை);
வெறுமை keepalive__cleanup(வெறுமை);
முழு keepalive__add(கட்டமைப்பு mosquitto *context);
வெறுமை keepalive__check(வெறுமை);
முழு keepalive__remove(கட்டமைப்பு mosquitto *context);
முழு keepalive__update(கட்டமைப்பு mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
முழு property__process_connect(கட்டமைப்பு mosquitto *context, mosquitto_property **props);
முழு property__process_will(கட்டமைப்பு mosquitto *context, கட்டமைப்பு mosquitto_message_all *செய்தி, mosquitto_property **props);
முழு property__process_publish(கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி, mosquitto_property **props, முழு *topic_alias, uint32_t *message_expiry_interval, bool is_bridge);
முழு property__process_disconnect(கட்டமைப்பு mosquitto *context, mosquitto_property **props);

/* ============================================================
 * Retain tree related functions
 * ============================================================ */
முழு retain__init(வெறுமை);
வெறுமை retain__clean(கட்டமைப்பு mosquitto__retainhier **retainhier);
முழு retain__queue(கட்டமைப்பு mosquitto *context, மாறிலி கட்டமைப்பு mosquitto_subscription *sub);
முழு retain__store(மாறிலி எழுத்து *topic, கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி, எழுத்து **split_topics, bool persist);
வெறுமை retain__expiry_check(வெறுமை);
வெறுமை retain__expire(கட்டமைப்பு mosquitto__retainhier **retainhier);

/* ============================================================
 * Security related functions
 * ============================================================ */
முழு acl__find_acls(கட்டமைப்பு mosquitto *context);
முழு plugin__load_all(வெறுமை);
முழு plugin__unload_all(வெறுமை);
முழு config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, கட்டமைப்பு mosquitto__security_options *security_options);

முழு mosquitto_security_init(bool reload);
முழு mosquitto_security_cleanup(bool reload);
முழு mosquitto_acl_check(கட்டமைப்பு mosquitto *context, மாறிலி எழுத்து *topic, uint32_t payloadlen, வெறுமை *payload, uint8_t qos, bool retain, mosquitto_property *properties, முழு access);
முழு mosquitto_basic_auth(கட்டமைப்பு mosquitto *context);
முழு mosquitto_psk_சாவி_get(கட்டமைப்பு mosquitto *context, மாறிலி எழுத்து *hint, மாறிலி எழுத்து *identity, எழுத்து *சாவி, முழு max_சாவி_len);

முழு mosquitto_security_init_default(வெறுமை);
முழு mosquitto_security_apply_default(வெறுமை);
முழு mosquitto_security_cleanup_default(வெறுமை);
முழு mosquitto_psk_சாவி_get_default(கட்டமைப்பு mosquitto *context, மாறிலி எழுத்து *hint, மாறிலி எழுத்து *identity, எழுத்து *சாவி, முழு max_சாவி_len);
முழு broker_acl_file__init(வெறுமை);
வெறுமை broker_acl_file__cleanup(வெறுமை);
முழு broker_password_file__init(வெறுமை);
வெறுமை broker_password_file__cleanup(வெறுமை);
முழு psk_file__init(வெறுமை);
முழு psk_file__cleanup(வெறுமை);

முழு mosquitto_security_auth_start(கட்டமைப்பு mosquitto *context, bool reauth, மாறிலி வெறுமை *data_in, uint16_t data_in_len, வெறுமை **data_out, uint16_t *data_out_len);
முழு mosquitto_security_auth_continue(கட்டமைப்பு mosquitto *context, மாறிலி வெறுமை *data_in, uint16_t data_len, வெறுமை **data_out, uint16_t *data_out_len);

வெறுமை unpwd__free_item(கட்டமைப்பு mosquitto__unpwd **unpwd, கட்டமைப்பு mosquitto__unpwd *item);

/* ============================================================
 * Session expiry
 * ============================================================ */
முழு session_expiry__add(கட்டமைப்பு mosquitto *context);
முழு session_expiry__add_from_persistence(கட்டமைப்பு mosquitto *context, time_t expiry_time);
வெறுமை session_expiry__add_on_shutdown(வெறுமை);
வெறுமை session_expiry__remove(கட்டமைப்பு mosquitto *context);
வெறுமை session_expiry__remove_all(வெறுமை);
வெறுமை session_expiry__check(வெறுமை);
வெறுமை session_expiry__send_all(வெறுமை);

/* ============================================================
 * Signals
 * ============================================================ */
வெறுமை signal__setup(வெறுமை);
முழு signal__flag_check(வெறுமை);

/* ============================================================
 * Window service and signal related functions
 * ============================================================ */
#எனில் defined(WIN32) || defined(__CYGWIN__)
வெறுமை service_install(எழுத்து *name);
வெறுமை service_uninstall(எழுத்து *name);
வெறுமை service_run(எழுத்து *name);

#ifdef WIN32
DWORD WINAPI SigThreadProc(வெறுமை *data);
#endif
#endif

/* ============================================================
 * Watchdog
 * ============================================================ */
வெறுமை watchdog__init(வெறுமை);
வெறுமை watchdog__check(வெறுமை);

/* ============================================================
 * Websockets related functions
 * ============================================================ */
#ifdef WITH_WEBSOCKETS
#  எனில் WITH_WEBSOCKETS == WS_IS_LWS
வெறுமை mosq_websockets_init(கட்டமைப்பு mosquitto__listener *listener, மாறிலி கட்டமைப்பு mosquitto__config *conf);
#  endif
முழு http__context_init(கட்டமைப்பு mosquitto *context);
முழு http__context_cleanup(கட்டமைப்பு mosquitto *context);
முழு http__read(கட்டமைப்பு mosquitto *context);
#endif
வெறுமை do_disconnect(கட்டமைப்பு mosquitto *context, முழு reason);

/* ============================================================
 * PROXY related functions
 * ============================================================ */
முழு proxy_v2__read(கட்டமைப்பு mosquitto *context);
முழு proxy_v1__read(கட்டமைப்பு mosquitto *context);

/* ============================================================
 * Will delay
 * ============================================================ */
முழு will_delay__add(கட்டமைப்பு mosquitto *context);
வெறுமை will_delay__check(வெறுமை);
வெறுமை will_delay__send_all(வெறுமை);
வெறுமை will_delay__remove(கட்டமைப்பு mosquitto *mosq);

/* ============================================================
 * HTTP Info
 * ============================================================ */
முழு http_api__start_local(கட்டமைப்பு mosquitto__listener *listener);
முழு http_api__start(கட்டமைப்பு mosquitto__listener *listener);
வெறுமை http_api__stop(கட்டமைப்பு mosquitto__listener *listener);


/* ============================================================
 * Other
 * ============================================================ */
#ifdef WITH_XTREPORT
வெறுமை xtreport(வெறுமை);
#endif

வெறுமை broker_control__init(வெறுமை);
வெறுமை broker_control__cleanup(வெறுமை);
வெறுமை broker_control__reload(வெறுமை);

#endif
