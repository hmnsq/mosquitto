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

#અન્તર્ભાવ "config.h"
#અન્તર્ભાવ <stdio.h>

#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#  જો LWS_LIBRARY_VERSION_સંખ્યા >= 3002000 && !defined(LWS_WITH_EXTERNAL_POLL)
#    warning "libwebsockets is not compiled with LWS_WITH_EXTERNAL_POLL support. Websocket performance will be unusable."
#  endif
#endif

#ifdef WITH_HTTP_API
#  include <microhttpd.h>
#endif

#ifdef __linux__
#define WITH_TCP_USER_TIMEOUT
#endif

#અન્તર્ભાવ "mosquitto_internal.h"
#અન્તર્ભાવ "mosquitto/broker.h"
#અન્તર્ભાવ "mosquitto/broker_plugin.h"
#અન્તર્ભાવ "mosquitto.h"
#અન્તર્ભાવ "logging_mosq.h"
#અન્તર્ભાવ "tls_mosq.h"
#અન્તર્ભાવ "uthash.h"
#અન્તર્ભાવ "acl_file.h"
#અન્તર્ભાવ "password_file.h"

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

પ્રકારવ્યાખ્યા uint64_t dbid_t;

પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_plugin_init_v5)(mosquitto_plugin_id_t *, શૂન્ય **, સંરચના mosquitto_opt *, પૂર્ણાંક);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_plugin_cleanup_v5)(શૂન્ય *, સંરચના mosquitto_opt *, પૂર્ણાંક);

પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_auth_plugin_init_v4)(શૂન્ય **, સંરચના mosquitto_opt *, પૂર્ણાંક);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_auth_plugin_cleanup_v4)(શૂન્ય *, સંરચના mosquitto_opt *, પૂર્ણાંક);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_auth_plugin_security_init_v4)(શૂન્ય *, સંરચના mosquitto_opt *, પૂર્ણાંક, bool);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_auth_plugin_security_cleanup_v4)(શૂન્ય *, સંરચના mosquitto_opt *, પૂર્ણાંક, bool);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_auth_plugin_acl_check_v4)(શૂન્ય *, પૂર્ણાંક, સંરચના mosquitto *, સંરચના mosquitto_acl_સંદેશ *);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_auth_plugin_unpwd_check_v4)(શૂન્ય *, સંરચના mosquitto *, અચલ અક્ષર *, અચલ અક્ષર *);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_auth_plugin_psk_ચાવી_get_v4)(શૂન્ય *, સંરચના mosquitto *, અચલ અક્ષર *, અચલ અક્ષર *, અક્ષર *, પૂર્ણાંક);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_auth_plugin_auth_start_v4)(શૂન્ય *, સંરચના mosquitto *, અચલ અક્ષર *, bool, અચલ શૂન્ય *, uint16_t, શૂન્ય **, uint16_t *);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_auth_plugin_auth_continue_v4)(શૂન્ય *, સંરચના mosquitto *, અચલ અક્ષર *, અચલ શૂન્ય *, uint16_t, શૂન્ય **, uint16_t *);

પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_auth_plugin_init_v3)(શૂન્ય **, સંરચના mosquitto_opt *, પૂર્ણાંક);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_auth_plugin_cleanup_v3)(શૂન્ય *, સંરચના mosquitto_opt *, પૂર્ણાંક);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_auth_plugin_security_init_v3)(શૂન્ય *, સંરચના mosquitto_opt *, પૂર્ણાંક, bool);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_auth_plugin_security_cleanup_v3)(શૂન્ય *, સંરચના mosquitto_opt *, પૂર્ણાંક, bool);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_auth_plugin_acl_check_v3)(શૂન્ય *, પૂર્ણાંક, અચલ સંરચના mosquitto *, સંરચના mosquitto_acl_સંદેશ *);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_auth_plugin_unpwd_check_v3)(શૂન્ય *, અચલ સંરચના mosquitto *, અચલ અક્ષર *, અચલ અક્ષર *);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_auth_plugin_psk_ચાવી_get_v3)(શૂન્ય *, અચલ સંરચના mosquitto *, અચલ અક્ષર *, અચલ અક્ષર *, અક્ષર *, પૂર્ણાંક);

પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_auth_plugin_init_v2)(શૂન્ય **, સંરચના mosquitto_auth_opt *, પૂર્ણાંક);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_auth_plugin_cleanup_v2)(શૂન્ય *, સંરચના mosquitto_auth_opt *, પૂર્ણાંક);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_auth_plugin_security_init_v2)(શૂન્ય *, સંરચના mosquitto_auth_opt *, પૂર્ણાંક, bool);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_auth_plugin_security_cleanup_v2)(શૂન્ય *, સંરચના mosquitto_auth_opt *, પૂર્ણાંક, bool);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_auth_plugin_acl_check_v2)(શૂન્ય *, અચલ અક્ષર *, અચલ અક્ષર *, અચલ અક્ષર *, પૂર્ણાંક);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_auth_plugin_unpwd_check_v2)(શૂન્ય *, અચલ અક્ષર *, અચલ અક્ષર *);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_auth_plugin_psk_ચાવી_get_v2)(શૂન્ય *, અચલ અક્ષર *, અચલ અક્ષર *, અક્ષર *, પૂર્ણાંક);


ગણના mosquitto_સંદેશ_origin {
	mosq_mo_client = 0,
	mosq_mo_broker = 1,
};

સંરચના mosquitto__plugin_lib {
	શૂન્ય *lib;
	શૂન્ય *user_data;
	પૂર્ણાંક (*plugin_version)(શૂન્ય);
	સંરચના mosquitto_plugin_id_t *identifier;

	FUNC_plugin_init_v5 plugin_init_v5;
	FUNC_plugin_cleanup_v5 plugin_cleanup_v5;

	FUNC_auth_plugin_init_v4 plugin_init_v4;
	FUNC_auth_plugin_cleanup_v4 plugin_cleanup_v4;
	FUNC_auth_plugin_security_init_v4 security_init_v4;
	FUNC_auth_plugin_security_cleanup_v4 security_cleanup_v4;
	FUNC_auth_plugin_acl_check_v4 acl_check_v4;
	FUNC_auth_plugin_unpwd_check_v4 unpwd_check_v4;
	FUNC_auth_plugin_psk_ચાવી_get_v4 psk_ચાવી_get_v4;
	FUNC_auth_plugin_auth_start_v4 auth_start_v4;
	FUNC_auth_plugin_auth_continue_v4 auth_continue_v4;

	FUNC_auth_plugin_init_v3 plugin_init_v3;
	FUNC_auth_plugin_cleanup_v3 plugin_cleanup_v3;
	FUNC_auth_plugin_security_init_v3 security_init_v3;
	FUNC_auth_plugin_security_cleanup_v3 security_cleanup_v3;
	FUNC_auth_plugin_acl_check_v3 acl_check_v3;
	FUNC_auth_plugin_unpwd_check_v3 unpwd_check_v3;
	FUNC_auth_plugin_psk_ચાવી_get_v3 psk_ચાવી_get_v3;

	FUNC_auth_plugin_init_v2 plugin_init_v2;
	FUNC_auth_plugin_cleanup_v2 plugin_cleanup_v2;
	FUNC_auth_plugin_security_init_v2 security_init_v2;
	FUNC_auth_plugin_security_cleanup_v2 security_cleanup_v2;
	FUNC_auth_plugin_acl_check_v2 acl_check_v2;
	FUNC_auth_plugin_unpwd_check_v2 unpwd_check_v2;
	FUNC_auth_plugin_psk_ચાવી_get_v2 psk_ચાવી_get_v2;
	પૂર્ણાંક version;
};

સંરચના mosquitto__plugin_config {
	અક્ષર *path;
	અક્ષર *name;
	સંરચના mosquitto_opt *options;
	સંરચના mosquitto__security_options **security_options;
	પૂર્ણાંક option_ગણતરી;
	પૂર્ણાંક security_option_ગણતરી;
	bool deny_special_chars;
};

સંરચના mosquitto__callback {
	UT_hash_handle hh; /* For callbacks that register for e.g. a specific topic */
	સંરચના mosquitto__callback *next, *prev; /* For typical callbacks */
	MOSQ_FUNC_generic_callback cb;
	શૂન્ય *userdata;
	સંઘ {
		અક્ષર *topic;
		સંરચના timespec next_tick;
	} data;
	mosquitto_plugin_id_t *identifier;
};

સંરચના plugin__callbacks {
	સંરચના mosquitto__callback *tick;
	સંરચના mosquitto__callback *acl_check;
	સંરચના mosquitto__callback *basic_auth;
	સંરચના mosquitto__callback *connect;
	સંરચના mosquitto__callback *control;
	સંરચના mosquitto__callback *disconnect;
	સંરચના mosquitto__callback *client_offline;
	સંરચના mosquitto__callback *ext_auth_continue;
	સંરચના mosquitto__callback *ext_auth_start;
	સંરચના mosquitto__callback *message_in;
	સંરચના mosquitto__callback *message_out;
	સંરચના mosquitto__callback *psk_ચાવી;
	સંરચના mosquitto__callback *reload;
	સંરચના mosquitto__callback *subscribe;
	સંરચના mosquitto__callback *unsubscribe;
	સંરચના mosquitto__callback *persist_restore;
	સંરચના mosquitto__callback *persist_client_add;
	સંરચના mosquitto__callback *persist_client_delete;
	સંરચના mosquitto__callback *persist_client_update;
	સંરચના mosquitto__callback *persist_subscription_add;
	સંરચના mosquitto__callback *persist_subscription_delete;
	સંરચના mosquitto__callback *persist_client_સંદેશ_add;
	સંરચના mosquitto__callback *persist_client_સંદેશ_delete;
	સંરચના mosquitto__callback *persist_client_સંદેશ_update;
	સંરચના mosquitto__callback *persist_base_સંદેશ_add;
	સંરચના mosquitto__callback *persist_base_સંદેશ_delete;
	સંરચના mosquitto__callback *persist_retain_સંદેશ_set;
	સંરચના mosquitto__callback *persist_retain_સંદેશ_delete;
	સંરચના mosquitto__callback *persist_will_add;
	સંરચના mosquitto__callback *persist_will_delete;
};

/* This is owned by mosquitto__config or mosquitto__listener, and only referred
 * to by other structs */
સંરચના mosquitto__security_options {
	/* Any options that get added here also need considering
	 * in config__read() with regards whether allow_anonymous
	 * should be disabled when these options are set.
	 */
	સંરચના mosquitto__unpwd *unpwd;
	સંરચના mosquitto__psk *psk_id;
	સંરચના acl_file_data acl_data;
	સંરચના password_file_data password_data;
	અક્ષર *psk_file;
	mosquitto_plugin_id_t **plugins;
	પૂર્ણાંક plugin_ગણતરી;
	int8_t allow_anonymous;
	bool allow_zero_length_clientid;
	અક્ષર *auto_id_prefix;
	uint16_t auto_id_prefix_len;
	સંરચના plugin__callbacks plugin_callbacks;
	mosquitto_plugin_id_t *pid; /* For registering as a "plugin" */
};

#જો defined(WITH_EPOLL) || defined(WITH_KQUEUE)
ગણના struct_ident {
	id_invalid = 0,
	id_listener = 1,
	id_client = 2,
	id_listener_ws = 3,
};
#endif

સંરચના mosquitto__listener {
	uint16_t port;
	અક્ષર *host;
	અક્ષર *bind_interface;
	પૂર્ણાંક max_connections;
	અક્ષર *mount_point;
	mosq_sock_t *socks;
	પૂર્ણાંક sock_ગણતરી;
	પૂર્ણાંક client_ગણતરી;
	ગણના mosquitto_protocol protocol;
	પૂર્ણાંક socket_domain;
	bool use_username_as_clientid;
	uint8_t max_qos;
	uint16_t max_topic_alias;
	uint16_t max_topic_alias_broker;
#ifdef WITH_TLS
	અક્ષર *cafile;
	અક્ષર *capath;
	અક્ષર *certfile;
	અક્ષર *keyfile;
	અક્ષર *tls_engine;
	અક્ષર *tls_engine_kpass_sha1;
	અક્ષર *ciphers;
	અક્ષર *ciphers_tls13;
	અક્ષર *psk_hint;
	SSL_CTX *ssl_ctx;
	અક્ષર *crlfile;
	અક્ષર *tls_version;
	bool use_identity_as_username;
	bool use_subject_as_username;
	bool require_certificate;
	bool disable_client_cert_date_checks;
	ગણના mosquitto__keyform tls_keyform;
#endif
#ifdef WITH_WEBSOCKETS
#  જો WITH_WEBSOCKETS == WS_IS_LWS
	સંરચના lws_context *ws_context;
	bool ws_in_init;
	સંરચના lws_protocols *ws_protocol;
#  endif
	અક્ષર **ws_origins;
	પૂર્ણાંક ws_origin_ગણતરી;
#endif
#જો defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
	અક્ષર *http_dir;
#endif
	સંરચના mosquitto__security_options *security_options;
#ifdef WITH_UNIX_SOCKETS
	અક્ષર *unix_socket_path;
#endif
	bool disable_protocol_v3;
	bool disable_protocol_v4;
	bool disable_protocol_v5;
	પૂર્ણાંક enable_proxy_protocol;
	bool proxy_protocol_v2_require_tls;
#ifdef WITH_HTTP_API
	સંરચના MHD_Daemon *mhd;
#endif
};


સંરચના mosquitto__listener_sock {
#જો defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	/* This *must* be the first element in the struct. */
	પૂર્ણાંક ident;
#endif
	mosq_sock_t sock;
	સંરચના mosquitto__listener *listener;
};


/* Callbacks belonging to a specific plugin
 * Doesn't include MOSQ_EVT_CONTROL events.
 */
સંરચના plugin_own_callback {
	સંરચના plugin_own_callback *next, *prev;
	MOSQ_FUNC_generic_callback cb_func;
	ગણના mosquitto_plugin_event event;
};

સંરચના mosquitto_plugin_id_t {
	સંરચના mosquitto__plugin_config config;
	સંરચના mosquitto__plugin_lib lib;
	સંરચના mosquitto__listener *listener;
	અક્ષર *plugin_name;
	અક્ષર *plugin_version;
	સંરચના control_endpoint *control_endpoints;
	સંરચના plugin_own_callback *own_callbacks;
	સંરચના timespec next_tick;
};

સંરચના mosquitto__config {
	bool allow_duplicate_messages;
	પૂર્ણાંક autosave_interval;
	bool autosave_on_changes;
	bool check_retain_source;
	અક્ષર *clientid_prefixes;
	bool connection_messages;
	uint16_t cmd_port[CMD_PORT_LIMIT];
	પૂર્ણાંક cmd_port_ગણતરી;
	bool daemon;
	bool test_configuration;
	bool enable_control_api;
	પૂર્ણાંક global_max_clients;
	પૂર્ણાંક global_max_connections;
	સંરચના mosquitto__listener *default_listener; /* Points to one of `listeners` */
	સંરચના mosquitto__listener *listeners;
	પૂર્ણાંક listener_ગણતરી;
	bool local_only;
	unsigned પૂર્ણાંક log_dest;
	પૂર્ણાંક log_facility;
	unsigned પૂર્ણાંક log_type;
	bool log_timestamp;
	અક્ષર *log_timestamp_format;
	અક્ષર *log_file;
	FILE *log_fptr;
	size_t max_inflight_bytes;
	size_t max_queued_bytes;
	પૂર્ણાંક max_queued_messages;
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
	અક્ષર *persistence_location;
	અક્ષર *persistence_file;
	અક્ષર *persistence_filepath;
	time_t persistent_client_expiration;
	અક્ષર *pid_file;
	bool queue_qos0_messages;
	bool per_listener_settings;
	bool retain_available;
	પૂર્ણાંક retain_expiry_interval;
	bool set_tcp_nodelay;
	પૂર્ણાંક sys_interval;
	bool upgrade_outgoing_qos;
	અક્ષર *user;
#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	પૂર્ણાંક websockets_log_level;
#endif
	uint16_t packet_buffer_size;
#ifdef WITH_BRIDGE
	સંરચના mosquitto__bridge **bridges;
	પૂર્ણાંક bridge_ગણતરી;
#endif
	સંરચના mosquitto__security_options security_options;
};


સંરચના mosquitto__subshared {
	UT_hash_handle hh;
	સંરચના mosquitto__subleaf *subs;
	અક્ષર name[];
};

સંરચના mosquitto__subhier {
	UT_hash_handle hh;
	સંરચના mosquitto__subhier *parent;
	સંરચના mosquitto__subhier *children;
	સંરચના mosquitto__subleaf *subs;
	સંરચના mosquitto__subshared *shared;
	uint16_t topic_len;
	અક્ષર topic[];
};

સંરચના mosquitto__subleaf {
	સંરચના mosquitto__subleaf *prev;
	સંરચના mosquitto__subleaf *next;
	સંરચના mosquitto *context;
	સંરચના mosquitto__subhier *hier;
	સંરચના mosquitto__subshared *shared;
	uint32_t identifier;
	uint8_t subscription_options;
	અક્ષર topic_filter[];
};

સંરચના sub__token {
	સંરચના sub__token *next;
	uint16_t topic_len;
	અક્ષર topic[];
};

સંરચના mosquitto__retainhier {
	UT_hash_handle hh;
	સંરચના mosquitto__retainhier *parent;
	સંરચના mosquitto__retainhier *children;
	સંરચના mosquitto_base_સંદેશ *retained;
	uint16_t topic_len;
	અક્ષર topic[];
};

સંરચના mosquitto_base_સંદેશ {
	UT_hash_handle hh;
	સંરચના mosquitto_base_સંદેશ_2 data;
	સંરચના mosquitto__listener *source_listener;
	અક્ષર **dest_ids;
	પૂર્ણાંક dest_id_ગણતરી;
	પૂર્ણાંક ref_ગણતરી;
	ગણના mosquitto_સંદેશ_origin origin;
	bool stored;
};

સંરચના mosquitto_client_સંદેશ {
	સંરચના mosquitto_client_સંદેશ_2 data;
	સંરચના mosquitto_client_સંદેશ *prev;
	સંરચના mosquitto_client_સંદેશ *next;
	સંરચના mosquitto_base_સંદેશ *base_સંદેશ;
};


સંરચના mosquitto__psk {
	UT_hash_handle hh;
	અક્ષર *username;
	અક્ષર *password;
};

સંરચના mosquitto__message_v5 {
	સંરચના mosquitto__message_v5 *next, *prev;
	અક્ષર *topic;
	શૂન્ય *payload;
	mosquitto_property *properties;
	/* clientid is used only by mosquitto_broker_publish*() to indicate
	   this message is for a specific client. */
	અક્ષર *clientid;
	પૂર્ણાંક payloadlen;
	પૂર્ણાંક qos;
	bool retain;
};

સંરચના mosquitto_db {
	dbid_t last_db_id;
	uint64_t ગાંઠ_id_shifted;
	સંરચના mosquitto__subhier *normal_subs;
	સંરચના mosquitto__subhier *shared_subs;
	સંરચના mosquitto__retainhier *retains;
	સંરચના mosquitto *contexts_by_id;
	સંરચના mosquitto *contexts_by_sock;
	સંરચના mosquitto *contexts_by_id_delayed_auth;
	સંરચના mosquitto *contexts_for_free;
	mosquitto_plugin_id_t **plugins;
	પૂર્ણાંક plugin_ગણતરી;
#ifdef WITH_BRIDGE
	સંરચના mosquitto **bridges;
	પૂર્ણાંક bridge_ગણતરી;
#endif
	સંરચના clientid__index_hash *clientid_index_hash;
	સંરચના mosquitto_base_સંદેશ *સંદેશ_store;
	time_t now_s; /* Monotonic clock, where possible */
	time_t now_real_s; /* Read clock, for measuring session/message expiry */
	uint64_t ગાંઠ_id; /* for unique db ids */
	time_t next_event_ms; /* for mux timeout */
	પૂર્ણાંક સંદેશ_store_ગણતરી;
	unsigned long સંદેશ_store_bytes;
	અક્ષર *config_file;
	સંરચના mosquitto__config *config;
	bool quiet;
	bool verbose;
#ifdef WITH_SYS_વૃક્ષ
	પૂર્ણાંક subscription_ગણતરી;
	પૂર્ણાંક shared_subscription_ગણતરી;
	પૂર્ણાંક retained_ગણતરી;
#endif
	પૂર્ણાંક persistence_changes;
	સંરચના mosquitto *ll_for_free;
#ifdef WITH_EPOLL
	પૂર્ણાંક epollfd;
#endif
#ifdef WITH_KQUEUE
	પૂર્ણાંક kqueuefd;
#endif
	સંરચના mosquitto__message_v5 *plugin_msgs;
#ifdef WITH_TLS
	/* tls_keylog can't be in the config struct because it is used
	   before the config is allocated. Config probably
	   shouldn't be separately allocated. */
	અક્ષર *tls_keylog;
#endif
	bool shutdown;
};

ગણના mosquitto__bridge_direction {
	bd_out = 0,
	bd_in = 1,
	bd_both = 2,
};

ગણના mosquitto_bridge_start_type {
	bst_automatic = 0,
	bst_lazy = 1,
	bst_manual = 2,
	bst_once = 3,
};

ગણના mosquitto_bridge_reload_type {
	brt_lazy = 0,
	brt_immediate = 1,
};

સંરચના mosquitto__bridge_topic {
	સંરચના mosquitto__bridge_topic *next;
	અક્ષર *topic;
	અક્ષર *local_prefix;
	અક્ષર *remote_prefix;
	અક્ષર *local_topic; /* topic prefixed with local_prefix */
	અક્ષર *remote_topic; /* topic prefixed with remote_prefix */
	ગણના mosquitto__bridge_direction direction;
	uint8_t qos;
};

સંરચના bridge_address {
	અક્ષર *address;
	uint16_t port;
};

સંરચના mosquitto__bridge {
	અક્ષર *name;
	સંરચના bridge_address *addresses;
	પૂર્ણાંક cur_address;
	પૂર્ણાંક address_ગણતરી;
	time_t primary_retry;
	mosq_sock_t primary_retry_sock;
	bool round_robin;
	bool try_private;
	bool try_private_accepted;
	bool clean_start;
	int8_t clean_start_local;
	uint16_t keepalive;
	unsigned પૂર્ણાંક tcp_keepalive_idle;
	unsigned પૂર્ણાંક tcp_keepalive_interval;
	unsigned પૂર્ણાંક tcp_keepalive_counter;
#ifdef WITH_TCP_USER_TIMEOUT
	પૂર્ણાંક tcp_user_timeout;
#endif
	સંરચના mosquitto__bridge_topic *topics;
	પૂર્ણાંક topic_ગણતરી;
	bool topic_remapping;
	ગણના mosquitto__protocol protocol_version;
	time_t restart_t;
	time_t connected_at;
	અક્ષર *remote_clientid;
	અક્ષર *remote_username;
	અક્ષર *remote_password;
	અક્ષર *local_clientid;
	અક્ષર *local_username;
	અક્ષર *local_password;
	અક્ષર *notification_topic;
	અક્ષર *bind_address;
	bool notifications;
	bool notifications_local_only;
	ગણના mosquitto_bridge_start_type start_type;
	પૂર્ણાંક idle_timeout;
	પૂર્ણાંક restart_timeout;
	પૂર્ણાંક backoff_base;
	પૂર્ણાંક backoff_cap;
	પૂર્ણાંક stable_connection_period;
	પૂર્ણાંક threshold;
	uint32_t maximum_packet_size;
	uint32_t session_expiry_interval;
	uint16_t receive_maximum;
	bool lazy_reconnect;
	bool attempt_unsubscribe;
	bool initial_notification_done;
	bool outgoing_retain;
	ગણના mosquitto_bridge_reload_type reload_type;
	uint16_t max_topic_alias;
#ifdef WITH_TLS
	bool tls_insecure;
	bool tls_ocsp_required;
	bool tls_use_os_certs;
	અક્ષર *tls_cafile;
	અક્ષર *tls_capath;
	અક્ષર *tls_certfile;
	અક્ષર *tls_keyfile;
	અક્ષર *tls_version;
	અક્ષર *tls_alpn;
	અક્ષર *tls_ciphers;
	અક્ષર *tls_13_ciphers;
#  ifdef FINAL_WITH_TLS_PSK
	અક્ષર *tls_psk_identity;
	અક્ષર *tls_psk;
#  endif
#endif
};

#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
સંરચના libws_mqtt_hack {
	અક્ષર *http_dir;
	સંરચના mosquitto__listener *listener;
};

સંરચના libws_mqtt_data {
	સંરચના mosquitto *mosq;
};
#endif

#અન્તર્ભાવ <net_mosq.h>


સંરચના control_endpoint {
	સંરચના control_endpoint *next, *prev;
	અક્ષર topic[];
};

બાહ્ય સંરચના mosquitto_db db;

/* ============================================================
 * Main functions
 * ============================================================ */
પૂર્ણાંક mosquitto_main_loop(સંરચના mosquitto__listener_sock *listensock, પૂર્ણાંક listensock_ગણતરી);
શૂન્ય loop__update_next_event(time_t new_ms);

/* ============================================================
 * Config functions
 * ============================================================ */
/* Initialise config struct to default values. */
શૂન્ય config__init(સંરચના mosquitto__config *config);
#ifdef WITH_BRIDGE
શૂન્ય config__bridge_cleanup(સંરચના mosquitto__bridge *bridge);
#endif
/* Parse command line options into config. */
પૂર્ણાંક config__parse_args(સંરચના mosquitto__config *config, પૂર્ણાંક argc, અક્ષર *argv[]);
/* Read configuration data from config->config_file into config.
 * If reload is true, don't process config options that shouldn't be reloaded (listeners etc)
 * Returns 0 on success, 1 if there is a configuration error or if a file cannot be opened.
 */
પૂર્ણાંક config__read(સંરચના mosquitto__config *config, bool reload);
/* Free all config data. */
શૂન્ય config__cleanup(સંરચના mosquitto__config *config);
પૂર્ણાંક config__get_dir_files(અચલ અક્ષર *include_dir, અક્ષર ***files, પૂર્ણાંક *file_ગણતરી);

/* ============================================================
 * Server send functions
 * ============================================================ */
પૂર્ણાંક send__connack(સંરચના mosquitto *context, uint8_t ack, uint8_t reason_code, અચલ mosquitto_property *properties);
પૂર્ણાંક send__suback(સંરચના mosquitto *context, uint16_t mid, uint32_t payloadlen, અચલ શૂન્ય *payload);
પૂર્ણાંક send__unsuback(સંરચના mosquitto *context, uint16_t mid, પૂર્ણાંક reason_code_ગણતરી, uint8_t *reason_codes, અચલ mosquitto_property *properties);
પૂર્ણાંક send__auth(સંરચના mosquitto *context, uint8_t reason_code, અચલ શૂન્ય *auth_data, uint16_t auth_data_len);

/* ============================================================
 * Network functions
 * ============================================================ */
શૂન્ય net__broker_init(શૂન્ય);
શૂન્ય net__broker_cleanup(શૂન્ય);
સંરચના mosquitto *net__socket_accept(સંરચના mosquitto__listener_sock *listensock);
પૂર્ણાંક net__socket_listen(સંરચના mosquitto__listener *listener);
પૂર્ણાંક net__socket_get_address(mosq_sock_t sock, અક્ષર *buf, size_t len, uint16_t *remote_address);
પૂર્ણાંક net__tls_load_verify(સંરચના mosquitto__listener *listener);
પૂર્ણાંક net__tls_server_ctx(સંરચના mosquitto__listener *listener);
પૂર્ણાંક net__load_certificates(સંરચના mosquitto__listener *listener);

/* ============================================================
 * Read handling functions
 * ============================================================ */
પૂર્ણાંક handle__packet(સંરચના mosquitto *context);
પૂર્ણાંક handle__connack(સંરચના mosquitto *context);
પૂર્ણાંક handle__connect(સંરચના mosquitto *context);
પૂર્ણાંક handle__disconnect(સંરચના mosquitto *context);
પૂર્ણાંક handle__publish(સંરચના mosquitto *context);
પૂર્ણાંક handle__subscribe(સંરચના mosquitto *context);
પૂર્ણાંક handle__unsubscribe(સંરચના mosquitto *context);
પૂર્ણાંક handle__auth(સંરચના mosquitto *context);

/* ============================================================
 * Database handling
 * ============================================================ */
પૂર્ણાંક db__open(સંરચના mosquitto__config *config);
પૂર્ણાંક db__close(શૂન્ય);
#ifdef WITH_PERSISTENCE
પૂર્ણાંક persist__backup(bool shutdown);
પૂર્ણાંક persist__restore(શૂન્ય);
#endif
/* Return the number of in-flight messages in count. */
પૂર્ણાંક db_message_ગણતરી(પૂર્ણાંક *ગણતરી);
પૂર્ણાંક db__message_delete_outgoing(સંરચના mosquitto *context, uint16_t mid, ગણના mosquitto_સંદેશ_state expect_state, પૂર્ણાંક qos);
પૂર્ણાંક db_message_ઉમેરો_outgoing(સંરચના mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, સંરચના mosquitto_base_સંદેશ *base_સંદેશ, uint32_t subscription_identifier, bool update, bool persist);
પૂર્ણાંક db_message_ઉમેરો_incoming(સંરચના mosquitto *context, uint64_t cmsg_id, સંરચના mosquitto_base_સંદેશ *base_સંદેશ, bool persist);
પૂર્ણાંક db__message_remove_incoming(સંરચના mosquitto *context, uint16_t mid);
પૂર્ણાંક db__message_release_incoming(સંરચના mosquitto *context, uint16_t mid);
પૂર્ણાંક db__message_update_outgoing(સંરચના mosquitto *context, uint16_t mid, ગણના mosquitto_સંદેશ_state state, પૂર્ણાંક qos, bool persist);
શૂન્ય db__message_dequeue_first(સંરચના mosquitto *context, સંરચના mosquitto_સંદેશ_data *સંદેશ_data);
પૂર્ણાંક db__messages_delete(સંરચના mosquitto *context, bool force_free);
પૂર્ણાંક db__messages_delete_incoming(સંરચના mosquitto *context);
પૂર્ણાંક db__messages_delete_outgoing(સંરચના mosquitto *context);
પૂર્ણાંક db__messages_easy_queue(સંરચના mosquitto *context, અચલ અક્ષર *topic, uint8_t qos, uint32_t payloadlen, અચલ શૂન્ય *payload, પૂર્ણાંક retain, uint32_t message_expiry_interval, mosquitto_property **properties);
પૂર્ણાંક db__message_store(અચલ સંરચના mosquitto *source, સંરચના mosquitto_base_સંદેશ *base_સંદેશ, uint32_t *message_expiry_interval, ગણના mosquitto_સંદેશ_origin origin);
પૂર્ણાંક db__message_store_find(સંરચના mosquitto *context, uint16_t mid, સંરચના mosquitto_client_સંદેશ **client_સંદેશ);
પૂર્ણાંક db_સંદેશ_store_add(સંરચના mosquitto_base_સંદેશ *base_સંદેશ);
શૂન્ય db_સંદેશ_store_remove(સંરચના mosquitto_base_સંદેશ *base_સંદેશ, bool notify);
શૂન્ય db_સંદેશ_store_ref_inc(સંરચના mosquitto_base_સંદેશ *base_સંદેશ);
શૂન્ય db_સંદેશ_store_ref_dec(સંરચના mosquitto_base_સંદેશ **base_સંદેશ);
શૂન્ય db_સંદેશ_store_clean(શૂન્ય);
શૂન્ય db_સંદેશ_store_compact(શૂન્ય);
શૂન્ય db_સંદેશ_store_free(સંરચના mosquitto_base_સંદેશ *base_સંદેશ);
પૂર્ણાંક db__message_reconnect_reset(સંરચના mosquitto *context);
bool db__ready_for_flight(સંરચના mosquitto *context, ગણના mosquitto_સંદેશ_direction dir, પૂર્ણાંક qos);
bool db__ready_for_queue(સંરચના mosquitto *context, પૂર્ણાંક qos, સંરચના mosquitto_સંદેશ_data *સંદેશ_data);
શૂન્ય sys_વૃક્ષ_init(શૂન્ય);
શૂન્ય sys_વૃક્ષ_update(bool force);
પૂર્ણાંક db__message_write_inflight_out_all(સંરચના mosquitto *context);
પૂર્ણાંક db__message_write_inflight_out_latest(સંરચના mosquitto *context);
પૂર્ણાંક db__message_write_queued_out(સંરચના mosquitto *context);
પૂર્ણાંક db__message_write_queued_in(સંરચના mosquitto *context);
શૂન્ય db_સંદેશ_add_to_inflight_stats(સંરચના mosquitto_સંદેશ_data *સંદેશ_data, સંરચના mosquitto_client_સંદેશ *સંદેશ);
શૂન્ય db_સંદેશ_add_to_queued_stats(સંરચના mosquitto_સંદેશ_data *સંદેશ_data, સંરચના mosquitto_client_સંદેશ *સંદેશ);
uint64_t db_new_સંદેશ_id(શૂન્ય);
શૂન્ય db__expire_all_messages(સંરચના mosquitto *context);
શૂન્ય db__check_acl_of_all_messages(સંરચના mosquitto *context);

/* ============================================================
 * Subscription functions
 * ============================================================ */
પૂર્ણાંક sub__init(શૂન્ય);
પૂર્ણાંક sub__add(સંરચના mosquitto *context, અચલ સંરચના mosquitto_subscription *sub);
પૂર્ણાંક sub__remove(સંરચના mosquitto *context, અચલ અક્ષર *sub, uint8_t *reason);
શૂન્ય sub_વૃક્ષ_print(સંરચના mosquitto__subhier *મૂળ, પૂર્ણાંક level);
પૂર્ણાંક sub__clean_session(સંરચના mosquitto *context);
પૂર્ણાંક sub__messages_queue(અચલ અક્ષર *source_id, uint8_t qos, સંરચના mosquitto_base_સંદેશ **base_સંદેશ);
પૂર્ણાંક sub__topic_tokenise(અચલ અક્ષર *subtopic, અક્ષર **local_sub, અક્ષર ***topics, અચલ અક્ષર **sharename);
શૂન્ય sub__topic_tokens_free(સંરચના sub__token *tokens);

/* ============================================================
 * Context functions
 * ============================================================ */
સંરચના mosquitto *context__init(શૂન્ય);
પૂર્ણાંક context__init_sock(સંરચના mosquitto *context, mosq_sock_t sock, bool get_address);
શૂન્ય context__cleanup(સંરચના mosquitto *context, bool force_free);
શૂન્ય context__disconnect(સંરચના mosquitto *context, પૂર્ણાંક reason);
શૂન્ય context__add_to_disused(સંરચના mosquitto *context);
શૂન્ય context__free_disused(શૂન્ય);
શૂન્ય context__send_will(સંરચના mosquitto *context);
શૂન્ય context__add_to_by_id(સંરચના mosquitto *context);
શૂન્ય context__remove_from_by_id(સંરચના mosquitto *context);

પૂર્ણાંક connect__on_authorised(સંરચના mosquitto *context, શૂન્ય *auth_data_out, uint16_t auth_data_out_len);


/* ============================================================
 * Control functions
 * ============================================================ */
#ifdef WITH_CONTROL
પૂર્ણાંક control__process(સંરચના mosquitto *context, સંરચના mosquitto_base_સંદેશ *base_સંદેશ);
શૂન્ય control__cleanup(શૂન્ય);
#endif
પૂર્ણાંક control__register_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, અચલ અક્ષર *topic, શૂન્ય *userdata);
પૂર્ણાંક control__unregister_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, અચલ અક્ષર *topic);
શૂન્ય control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier);


/* ============================================================
 * Logging functions
 * ============================================================ */
પૂર્ણાંક log__init(સંરચના mosquitto__config *config);
પૂર્ણાંક log__close(સંરચના mosquitto__config *config);
શૂન્ય log__internal(અચલ અક્ષર *fmt, ...) __attribute__((format(printf, 1, 2)));

/* ============================================================
 * Bridge functions
 * ============================================================ */
#ifdef WITH_BRIDGE
શૂન્ય bridge__start_all(શૂન્ય);
શૂન્ય bridge__reload(શૂન્ય);
શૂન્ય bridge__db_cleanup(શૂન્ય);
શૂન્ય bridge__cleanup(સંરચના mosquitto *context);
પૂર્ણાંક bridge__connect(સંરચના mosquitto *context);
#જો defined(__GLIBC__) && defined(WITH_ADNS)
પૂર્ણાંક bridge__connect_step3(સંરચના mosquitto *context);
#endif
પૂર્ણાંક bridge__on_connect(સંરચના mosquitto *context);
શૂન્ય bridge_check(શૂન્ય);
પૂર્ણાંક bridge__register_local_connections(શૂન્ય);
પૂર્ણાંક bridge__add_topic(સંરચના mosquitto__bridge *bridge, અચલ અક્ષર *topic, ગણના mosquitto__bridge_direction direction, uint8_t qos, અચલ અક્ષર *local_prefix, અચલ અક્ષર *remote_prefix);
શૂન્ય bridge__cleanup_topics(સંરચના mosquitto__bridge *bridge);
પૂર્ણાંક bridge__remap_topic_in(સંરચના mosquitto *context, અક્ષર **topic);
#endif

/* ============================================================
 * IO multiplex related functions
 * ============================================================ */
પૂર્ણાંક mux__init(શૂન્ય);
પૂર્ણાંક mux__add_listeners(સંરચના mosquitto__listener_sock *listensock, પૂર્ણાંક listensock_ગણતરી);
પૂર્ણાંક mux__delete_listeners(સંરચના mosquitto__listener_sock *listensock, પૂર્ણાંક listensock_ગણતરી);
પૂર્ણાંક mux__loop_prepare(શૂન્ય);
પૂર્ણાંક mux__new(સંરચના mosquitto *context);
પૂર્ણાંક mux__add_out(સંરચના mosquitto *context);
પૂર્ણાંક mux__remove_out(સંરચના mosquitto *context);
પૂર્ણાંક mux__delete(સંરચના mosquitto *context);
પૂર્ણાંક mux__wait(શૂન્ય);
પૂર્ણાંક mux__handle(સંરચના mosquitto__listener_sock *listensock, પૂર્ણાંક listensock_ગણતરી);
પૂર્ણાંક mux__cleanup(શૂન્ય);

/* ============================================================
 * Listener related functions
 * ============================================================ */
બાહ્ય સંરચના mosquitto__listener_sock *g_listensock;
બાહ્ય પૂર્ણાંક g_listensock_ગણતરી;

શૂન્ય listener__set_defaults(સંરચના mosquitto__listener *listener);
શૂન્ય listeners__reload_all_certificates(શૂન્ય);
#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
શૂન્ય listeners__add_websockets(સંરચના lws_context *ws_context, mosq_sock_t fd);
#endif
પૂર્ણાંક listeners__start(શૂન્ય);
શૂન્ય listeners__stop(શૂન્ય);

/* ============================================================
 * Plugin related functions
 * ============================================================ */
પૂર્ણાંક plugin__load_v5(mosquitto_plugin_id_t *plugin, શૂન્ય *lib);
પૂર્ણાંક plugin__load_v4(mosquitto_plugin_id_t *plugin, શૂન્ય *lib);
પૂર્ણાંક plugin__load_v3(mosquitto_plugin_id_t *plugin, શૂન્ય *lib);
પૂર્ણાંક plugin__load_v2(mosquitto_plugin_id_t *plugin, શૂન્ય *lib);
પૂર્ણાંક acl__pre_check(mosquitto_plugin_id_t *plugin, સંરચના mosquitto *context, પૂર્ણાંક access);

શૂન્ય LIB_ERROR(શૂન્ય);
શૂન્ય plugin__handle_connect(સંરચના mosquitto *context);
શૂન્ય plugin__handle_disconnect(સંરચના mosquitto *context, પૂર્ણાંક reason);
શૂન્ય plugin__handle_client_offline(સંરચના mosquitto *context, પૂર્ણાંક reason);
પૂર્ણાંક plugin__handle_message_in(સંરચના mosquitto *context, સંરચના mosquitto_base_સંદેશ_2 *base_સંદેશ);
પૂર્ણાંક plugin__handle_message_out(સંરચના mosquitto *context, સંરચના mosquitto_base_સંદેશ_2 *base_સંદેશ);
પૂર્ણાંક plugin__handle_subscribe(સંરચના mosquitto *context, સંરચના mosquitto_subscription *sub);
પૂર્ણાંક plugin__handle_unsubscribe(સંરચના mosquitto *context, સંરચના mosquitto_subscription *sub);
પૂર્ણાંક plugin__handle_reload(શૂન્ય);
શૂન્ય plugin__handle_tick(શૂન્ય);
પૂર્ણાંક plugin__callback_unregister_all(mosquitto_plugin_id_t *identifier);
શૂન્ય plugin_persist__handle_restore(શૂન્ય);
શૂન્ય plugin_persist__handle_client_add(સંરચના mosquitto *context);
શૂન્ય plugin_persist__handle_client_delete(સંરચના mosquitto *context);
શૂન્ય plugin_persist__handle_client_update(સંરચના mosquitto *context);
શૂન્ય plugin_persist__handle_subscription_add(સંરચના mosquitto *context, અચલ સંરચના mosquitto_subscription *sub);
શૂન્ય plugin_persist__handle_subscription_delete(સંરચના mosquitto *context, અક્ષર *sub);
શૂન્ય plugin_persist_handle_client_સંદેશ_add(સંરચના mosquitto *context, અચલ સંરચના mosquitto_client_સંદેશ *cmsg);
શૂન્ય plugin_persist_handle_client_સંદેશ_delete(સંરચના mosquitto *context, અચલ સંરચના mosquitto_client_સંદેશ *cmsg);
શૂન્ય plugin_persist_handle_client_સંદેશ_update(સંરચના mosquitto *context, અચલ સંરચના mosquitto_client_સંદેશ *cmsg);
શૂન્ય plugin_persist_handle_base_સંદેશ_add(સંરચના mosquitto_base_સંદેશ *base_સંદેશ);
શૂન્ય plugin_persist_handle_base_સંદેશ_delete(સંરચના mosquitto_base_સંદેશ *base_સંદેશ);
શૂન્ય plugin_persist_handle_retain_સંદેશ_set(સંરચના mosquitto_base_સંદેશ *base_સંદેશ);
શૂન્ય plugin_persist_handle_retain_સંદેશ_delete(સંરચના mosquitto_base_સંદેશ *base_સંદેશ);
શૂન્ય plugin_persist__handle_will_add(સંરચના mosquitto *context);
શૂન્ય plugin_persist__handle_will_delete(સંરચના mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
પૂર્ણાંક keepalive__init(શૂન્ય);
શૂન્ય keepalive__cleanup(શૂન્ય);
પૂર્ણાંક keepalive__add(સંરચના mosquitto *context);
શૂન્ય keepalive__check(શૂન્ય);
પૂર્ણાંક keepalive__remove(સંરચના mosquitto *context);
પૂર્ણાંક keepalive__update(સંરચના mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
પૂર્ણાંક property__process_connect(સંરચના mosquitto *context, mosquitto_property **props);
પૂર્ણાંક property__process_will(સંરચના mosquitto *context, સંરચના mosquitto_message_all *સંદેશ, mosquitto_property **props);
પૂર્ણાંક property__process_publish(સંરચના mosquitto_base_સંદેશ *base_સંદેશ, mosquitto_property **props, પૂર્ણાંક *topic_alias, uint32_t *message_expiry_interval, bool is_bridge);
પૂર્ણાંક property__process_disconnect(સંરચના mosquitto *context, mosquitto_property **props);

/* ============================================================
 * Retain tree related functions
 * ============================================================ */
પૂર્ણાંક retain__init(શૂન્ય);
શૂન્ય retain__clean(સંરચના mosquitto__retainhier **retainhier);
પૂર્ણાંક retain__queue(સંરચના mosquitto *context, અચલ સંરચના mosquitto_subscription *sub);
પૂર્ણાંક retain__store(અચલ અક્ષર *topic, સંરચના mosquitto_base_સંદેશ *base_સંદેશ, અક્ષર **split_topics, bool persist);
શૂન્ય retain__expiry_check(શૂન્ય);
શૂન્ય retain__expire(સંરચના mosquitto__retainhier **retainhier);

/* ============================================================
 * Security related functions
 * ============================================================ */
પૂર્ણાંક acl__find_acls(સંરચના mosquitto *context);
પૂર્ણાંક plugin__load_all(શૂન્ય);
પૂર્ણાંક plugin__unload_all(શૂન્ય);
પૂર્ણાંક config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, સંરચના mosquitto__security_options *security_options);

પૂર્ણાંક mosquitto_security_init(bool reload);
પૂર્ણાંક mosquitto_security_cleanup(bool reload);
પૂર્ણાંક mosquitto_acl_check(સંરચના mosquitto *context, અચલ અક્ષર *topic, uint32_t payloadlen, શૂન્ય *payload, uint8_t qos, bool retain, mosquitto_property *properties, પૂર્ણાંક access);
પૂર્ણાંક mosquitto_basic_auth(સંરચના mosquitto *context);
પૂર્ણાંક mosquitto_psk_ચાવી_get(સંરચના mosquitto *context, અચલ અક્ષર *hint, અચલ અક્ષર *identity, અક્ષર *ચાવી, પૂર્ણાંક max_ચાવી_len);

પૂર્ણાંક mosquitto_security_init_default(શૂન્ય);
પૂર્ણાંક mosquitto_security_apply_default(શૂન્ય);
પૂર્ણાંક mosquitto_security_cleanup_default(શૂન્ય);
પૂર્ણાંક mosquitto_psk_ચાવી_get_default(સંરચના mosquitto *context, અચલ અક્ષર *hint, અચલ અક્ષર *identity, અક્ષર *ચાવી, પૂર્ણાંક max_ચાવી_len);
પૂર્ણાંક broker_acl_file__init(શૂન્ય);
શૂન્ય broker_acl_file__cleanup(શૂન્ય);
પૂર્ણાંક broker_password_file__init(શૂન્ય);
શૂન્ય broker_password_file__cleanup(શૂન્ય);
પૂર્ણાંક psk_file__init(શૂન્ય);
પૂર્ણાંક psk_file__cleanup(શૂન્ય);

પૂર્ણાંક mosquitto_security_auth_start(સંરચના mosquitto *context, bool reauth, અચલ શૂન્ય *data_in, uint16_t data_in_len, શૂન્ય **data_out, uint16_t *data_out_len);
પૂર્ણાંક mosquitto_security_auth_continue(સંરચના mosquitto *context, અચલ શૂન્ય *data_in, uint16_t data_len, શૂન્ય **data_out, uint16_t *data_out_len);

શૂન્ય unpwd__free_item(સંરચના mosquitto__unpwd **unpwd, સંરચના mosquitto__unpwd *item);

/* ============================================================
 * Session expiry
 * ============================================================ */
પૂર્ણાંક session_expiry__add(સંરચના mosquitto *context);
પૂર્ણાંક session_expiry__add_from_persistence(સંરચના mosquitto *context, time_t expiry_time);
શૂન્ય session_expiry__add_on_shutdown(શૂન્ય);
શૂન્ય session_expiry__remove(સંરચના mosquitto *context);
શૂન્ય session_expiry__remove_all(શૂન્ય);
શૂન્ય session_expiry__check(શૂન્ય);
શૂન્ય session_expiry__send_all(શૂન્ય);

/* ============================================================
 * Signals
 * ============================================================ */
શૂન્ય signal__setup(શૂન્ય);
પૂર્ણાંક signal__flag_check(શૂન્ય);

/* ============================================================
 * Window service and signal related functions
 * ============================================================ */
#જો defined(WIN32) || defined(__CYGWIN__)
શૂન્ય service_install(અક્ષર *name);
શૂન્ય service_uninstall(અક્ષર *name);
શૂન્ય service_run(અક્ષર *name);

#ifdef WIN32
DWORD WINAPI SigThreadProc(શૂન્ય *data);
#endif
#endif

/* ============================================================
 * Watchdog
 * ============================================================ */
શૂન્ય watchdog__init(શૂન્ય);
શૂન્ય watchdog__check(શૂન્ય);

/* ============================================================
 * Websockets related functions
 * ============================================================ */
#ifdef WITH_WEBSOCKETS
#  જો WITH_WEBSOCKETS == WS_IS_LWS
શૂન્ય mosq_websockets_init(સંરચના mosquitto__listener *listener, અચલ સંરચના mosquitto__config *conf);
#  endif
પૂર્ણાંક http__context_init(સંરચના mosquitto *context);
પૂર્ણાંક http__context_cleanup(સંરચના mosquitto *context);
પૂર્ણાંક http__read(સંરચના mosquitto *context);
#endif
શૂન્ય do_disconnect(સંરચના mosquitto *context, પૂર્ણાંક reason);

/* ============================================================
 * PROXY related functions
 * ============================================================ */
પૂર્ણાંક proxy_v2__read(સંરચના mosquitto *context);
પૂર્ણાંક proxy_v1__read(સંરચના mosquitto *context);

/* ============================================================
 * Will delay
 * ============================================================ */
પૂર્ણાંક will_delay__add(સંરચના mosquitto *context);
શૂન્ય will_delay__check(શૂન્ય);
શૂન્ય will_delay__send_all(શૂન્ય);
શૂન્ય will_delay__remove(સંરચના mosquitto *mosq);

/* ============================================================
 * HTTP Info
 * ============================================================ */
પૂર્ણાંક http_api__start_local(સંરચના mosquitto__listener *listener);
પૂર્ણાંક http_api__start(સંરચના mosquitto__listener *listener);
શૂન્ય http_api__stop(સંરચના mosquitto__listener *listener);


/* ============================================================
 * Other
 * ============================================================ */
#ifdef WITH_XTREPORT
શૂન્ય xtreport(શૂન્ય);
#endif

શૂન્ય broker_control__init(શૂન્ય);
શૂન્ય broker_control__cleanup(શૂન્ય);
શૂન્ય broker_control__reload(શૂન્ય);

#endif
