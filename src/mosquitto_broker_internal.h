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

#اگر_غیر_تعریف_شدہ MOSQUITTO_BROKER_اندرونی_H
#تعریف_کرو MOSQUITTO_BROKER_اندرونی_H

#شامل "config.h"
#شامل <stdio.h>

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#  اگر LWS_LIBRARY_نسخہ_نمبر >= 3002000 && !defined(LWS_WITH_EXTERNAL_POLL)
#    warning "libwebsockets is not compiled with LWS_WITH_EXTERNAL_POLL support. Websocket performance will be unusable."
#  endif
#اگر_ختم

#اگر_تعریف_شدہ WITH_HTTP_API
#  include <microhttpd.h>
#اگر_ختم

#اگر_تعریف_شدہ __linux__
#تعریف_کرو WITH_TCP_USER_TIMEOUT
#اگر_ختم

#شامل "mosquitto_internal.h"
#شامل "mosquitto/broker.h"
#شامل "mosquitto/broker_plugin.h"
#شامل "mosquitto.h"
#شامل "logging_mosq.h"
#شامل "tls_mosq.h"
#شامل "uthash.h"
#شامل "acl_file.h"
#شامل "password_file.h"

#اگر_غیر_تعریف_شدہ __GNUC__
#تعریف_کرو __attribute__(attrib)
#اگر_ختم

/* Log destinations */
#تعریف_کرو MQTT3_LOG_NONE 0x00
#تعریف_کرو MQTT3_LOG_SYSLOG 0x01
#تعریف_کرو MQTT3_LOG_FILE 0x02
#تعریف_کرو MQTT3_LOG_STDOUT 0x04
#تعریف_کرو MQTT3_LOG_STDERR 0x08
#تعریف_کرو MQTT3_LOG_TOPIC 0x10
#تعریف_کرو MQTT3_LOG_DLT 0x20
#اگر_تعریف_شدہ ANDROID
#تعریف_کرو MQTT3_LOG_ANDROID 0x40
#اگر_ختم
#تعریف_کرو MQTT3_LOG_ALL 0xFF

#تعریف_کرو CMD_پورٹ_حد_بندی 10

قسمتعریف uint64_t dbid_t;

قسمتعریف صحیح (*FUNC_plugin_init_v5)(mosquitto_plugin_id_t *, خالی **, ساخت mosquitto_opt *, صحیح);
قسمتعریف صحیح (*FUNC_plugin_cleanup_v5)(خالی *, ساخت mosquitto_opt *, صحیح);

قسمتعریف صحیح (*FUNC_auth_plugin_init_v4)(خالی **, ساخت mosquitto_opt *, صحیح);
قسمتعریف صحیح (*FUNC_auth_plugin_cleanup_v4)(خالی *, ساخت mosquitto_opt *, صحیح);
قسمتعریف صحیح (*FUNC_auth_plugin_security_init_v4)(خالی *, ساخت mosquitto_opt *, صحیح, bool);
قسمتعریف صحیح (*FUNC_auth_plugin_security_cleanup_v4)(خالی *, ساخت mosquitto_opt *, صحیح, bool);
قسمتعریف صحیح (*FUNC_auth_plugin_acl_check_v4)(خالی *, صحیح, ساخت mosquitto *, ساخت mosquitto_acl_پیغام *);
قسمتعریف صحیح (*FUNC_auth_plugin_unpwd_check_v4)(خالی *, ساخت mosquitto *, ثابت حرف *, ثابت حرف *);
قسمتعریف صحیح (*FUNC_auth_plugin_psk_کلید_get_v4)(خالی *, ساخت mosquitto *, ثابت حرف *, ثابت حرف *, حرف *, صحیح);
قسمتعریف صحیح (*FUNC_auth_plugin_auth_آغاز_v4)(خالی *, ساخت mosquitto *, ثابت حرف *, bool, ثابت خالی *, uint16_t, خالی **, uint16_t *);
قسمتعریف صحیح (*FUNC_auth_plugin_auth_continue_v4)(خالی *, ساخت mosquitto *, ثابت حرف *, ثابت خالی *, uint16_t, خالی **, uint16_t *);

قسمتعریف صحیح (*FUNC_auth_plugin_init_v3)(خالی **, ساخت mosquitto_opt *, صحیح);
قسمتعریف صحیح (*FUNC_auth_plugin_cleanup_v3)(خالی *, ساخت mosquitto_opt *, صحیح);
قسمتعریف صحیح (*FUNC_auth_plugin_security_init_v3)(خالی *, ساخت mosquitto_opt *, صحیح, bool);
قسمتعریف صحیح (*FUNC_auth_plugin_security_cleanup_v3)(خالی *, ساخت mosquitto_opt *, صحیح, bool);
قسمتعریف صحیح (*FUNC_auth_plugin_acl_check_v3)(خالی *, صحیح, ثابت ساخت mosquitto *, ساخت mosquitto_acl_پیغام *);
قسمتعریف صحیح (*FUNC_auth_plugin_unpwd_check_v3)(خالی *, ثابت ساخت mosquitto *, ثابت حرف *, ثابت حرف *);
قسمتعریف صحیح (*FUNC_auth_plugin_psk_کلید_get_v3)(خالی *, ثابت ساخت mosquitto *, ثابت حرف *, ثابت حرف *, حرف *, صحیح);

قسمتعریف صحیح (*FUNC_auth_plugin_init_v2)(خالی **, ساخت mosquitto_auth_opt *, صحیح);
قسمتعریف صحیح (*FUNC_auth_plugin_cleanup_v2)(خالی *, ساخت mosquitto_auth_opt *, صحیح);
قسمتعریف صحیح (*FUNC_auth_plugin_security_init_v2)(خالی *, ساخت mosquitto_auth_opt *, صحیح, bool);
قسمتعریف صحیح (*FUNC_auth_plugin_security_cleanup_v2)(خالی *, ساخت mosquitto_auth_opt *, صحیح, bool);
قسمتعریف صحیح (*FUNC_auth_plugin_acl_check_v2)(خالی *, ثابت حرف *, ثابت حرف *, ثابت حرف *, صحیح);
قسمتعریف صحیح (*FUNC_auth_plugin_unpwd_check_v2)(خالی *, ثابت حرف *, ثابت حرف *);
قسمتعریف صحیح (*FUNC_auth_plugin_psk_کلید_get_v2)(خالی *, ثابت حرف *, ثابت حرف *, حرف *, صحیح);


شمار mosquitto_پیغام_origin {
	mosq_mo_client = 0,
	mosq_mo_broker = 1,
};

ساخت mosquitto__plugin_lib {
	خالی *lib;
	خالی *user_ڈیٹا;
	صحیح (*plugin_نسخہ)(خالی);
	ساخت mosquitto_plugin_id_t *identifier;

	FUNC_plugin_init_v5 plugin_init_v5;
	FUNC_plugin_cleanup_v5 plugin_cleanup_v5;

	FUNC_auth_plugin_init_v4 plugin_init_v4;
	FUNC_auth_plugin_cleanup_v4 plugin_cleanup_v4;
	FUNC_auth_plugin_security_init_v4 security_init_v4;
	FUNC_auth_plugin_security_cleanup_v4 security_cleanup_v4;
	FUNC_auth_plugin_acl_check_v4 acl_check_v4;
	FUNC_auth_plugin_unpwd_check_v4 unpwd_check_v4;
	FUNC_auth_plugin_psk_کلید_get_v4 psk_کلید_get_v4;
	FUNC_auth_plugin_auth_آغاز_v4 auth_آغاز_v4;
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
	صحیح نسخہ_2;
};

ساخت mosquitto__plugin_config {
	حرف *راستہ;
	حرف *name;
	ساخت mosquitto_opt *options;
	ساخت mosquitto__security_options **security_options;
	صحیح option_گنتی;
	صحیح security_option_گنتی;
	bool deny_special_chars;
};

ساخت mosquitto__callback {
	UT_hash_handle hh; /* For callbacks that register for e.g. a specific topic */
	ساخت mosquitto__callback *اگلا, *پچھلا; /* For typical callbacks */
	MOSQ_FUNC_generic_callback cb;
	خالی *userdata;
	اتحاد {
		حرف *topic;
		ساخت timespec اگلا_tick;
	} ڈیٹا;
	mosquitto_plugin_id_t *identifier;
};

ساخت plugin__callbacks {
	ساخت mosquitto__callback *tick;
	ساخت mosquitto__callback *acl_check;
	ساخت mosquitto__callback *basic_auth;
	ساخت mosquitto__callback *connect;
	ساخت mosquitto__callback *control;
	ساخت mosquitto__callback *disconnect;
	ساخت mosquitto__callback *client_offline;
	ساخت mosquitto__callback *ext_auth_continue;
	ساخت mosquitto__callback *ext_auth_آغاز;
	ساخت mosquitto__callback *پیغام_اندر;
	ساخت mosquitto__callback *پیغام_برآمد;
	ساخت mosquitto__callback *psk_کلید;
	ساخت mosquitto__callback *reload;
	ساخت mosquitto__callback *subscribe;
	ساخت mosquitto__callback *unsubscribe;
	ساخت mosquitto__callback *persist_restore;
	ساخت mosquitto__callback *persist_client_جوڑو;
	ساخت mosquitto__callback *persist_client_delete;
	ساخت mosquitto__callback *persist_client_update;
	ساخت mosquitto__callback *persist_subscription_جوڑو;
	ساخت mosquitto__callback *persist_subscription_delete;
	ساخت mosquitto__callback *persist_client_پیغام_جوڑو;
	ساخت mosquitto__callback *persist_client_پیغام_delete;
	ساخت mosquitto__callback *persist_client_پیغام_update;
	ساخت mosquitto__callback *persist_بنیاد_پیغام_جوڑو;
	ساخت mosquitto__callback *persist_بنیاد_پیغام_delete;
	ساخت mosquitto__callback *persist_retain_پیغام_سیٹ;
	ساخت mosquitto__callback *persist_retain_پیغام_delete;
	ساخت mosquitto__callback *persist_will_جوڑو;
	ساخت mosquitto__callback *persist_will_delete;
};

/* This is owned by mosquitto__config or mosquitto__listener, and only referred
 * to by other structs */
ساخت mosquitto__security_options {
	/* Any options that get added here also need considering
	 * in config__read() with regards whether allow_anonymous
	 * should be disabled when these options are set.
	 */
	ساخت mosquitto__unpwd *unpwd;
	ساخت mosquitto__psk *psk_id;
	ساخت acl_file_ڈیٹا acl_ڈیٹا;
	ساخت password_file_ڈیٹا password_ڈیٹا;
	حرف *psk_file;
	mosquitto_plugin_id_t **plugins;
	صحیح plugin_گنتی;
	int8_t allow_anonymous;
	bool allow_zero_لمبائی_clientid;
	حرف *auto_id_سابقہ;
	uint16_t auto_id_سابقہ_len;
	ساخت plugin__callbacks plugin_callbacks;
	mosquitto_plugin_id_t *pid; /* For registering as a "plugin" */
};

#اگر defined(WITH_EPOLL) || defined(WITH_KQUEUE)
شمار struct_ident {
	id_invalid = 0,
	id_listener = 1,
	id_client = 2,
	id_listener_ws = 3,
};
#اگر_ختم

ساخت mosquitto__listener {
	uint16_t پورٹ;
	حرف *host;
	حرف *bind_interface;
	صحیح max_connections;
	حرف *mount_نقطہ;
	mosq_sock_t *socks;
	صحیح sock_گنتی;
	صحیح client_گنتی;
	شمار mosquitto_protocol protocol;
	صحیح socket_domain;
	bool use_username_as_clientid;
	uint8_t max_qos;
	uint16_t max_topic_alias;
	uint16_t max_topic_alias_broker;
#اگر_تعریف_شدہ WITH_TLS
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
	حرف *tls_نسخہ;
	bool use_شناختی_عنصر_as_username;
	bool use_subject_as_username;
	bool require_certificate;
	bool disable_client_cert_date_checks;
	شمار mosquitto__keyform tls_keyform;
#اگر_ختم
#اگر_تعریف_شدہ WITH_WEBSOCKETS
#  اگر WITH_WEBSOCKETS == WS_IS_LWS
	ساخت lws_context *ws_context;
	bool ws_اندر_init;
	ساخت lws_protocols *ws_protocol;
#  endif
	حرف **ws_origins;
	صحیح ws_origin_گنتی;
#اگر_ختم
#اگر defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
	حرف *http_dir;
#اگر_ختم
	ساخت mosquitto__security_options *security_options;
#اگر_تعریف_شدہ WITH_UNIX_SOCKETS
	حرف *unix_socket_راستہ;
#اگر_ختم
	bool disable_protocol_v3;
	bool disable_protocol_v4;
	bool disable_protocol_v5;
	صحیح enable_proxy_protocol;
	bool proxy_protocol_v2_require_tls;
#اگر_تعریف_شدہ WITH_HTTP_API
	ساخت MHD_Daemon *mhd;
#اگر_ختم
};


ساخت mosquitto__listener_sock {
#اگر defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	/* This *must* be the first element in the struct. */
	صحیح ident;
#اگر_ختم
	mosq_sock_t sock;
	ساخت mosquitto__listener *listener;
};


/* Callbacks belonging to a specific plugin
 * Doesn't include MOSQ_EVT_CONTROL events.
 */
ساخت plugin_own_callback {
	ساخت plugin_own_callback *اگلا, *پچھلا;
	MOSQ_FUNC_generic_callback cb_func;
	شمار mosquitto_plugin_event event;
};

ساخت mosquitto_plugin_id_t {
	ساخت mosquitto__plugin_config config;
	ساخت mosquitto__plugin_lib lib;
	ساخت mosquitto__listener *listener;
	حرف *plugin_name;
	حرف *plugin_نسخہ;
	ساخت control_endpoint *control_endpoints;
	ساخت plugin_own_callback *own_callbacks;
	ساخت timespec اگلا_tick;
};

ساخت mosquitto__config {
	bool allow_duplicate_messages;
	صحیح autosave_interval;
	bool autosave_on_changes;
	bool check_retain_ماخذ;
	حرف *clientid_prefixes;
	bool connection_messages;
	uint16_t cmd_پورٹ[CMD_پورٹ_حد_بندی];
	صحیح cmd_پورٹ_گنتی;
	bool daemon;
	bool test_configuration;
	bool enable_control_api;
	صحیح global_max_clients;
	صحیح global_max_connections;
	ساخت mosquitto__listener *default_listener; /* Points to one of `listeners` */
	ساخت mosquitto__listener *listeners;
	صحیح listener_گنتی;
	bool local_only;
	غیرعلامتی صحیح log_dest;
	صحیح log_facility;
	غیرعلامتی صحیح log_type;
	bool log_timestamp;
	حرف *log_timestamp_format;
	حرف *log_file;
	FILE *log_fptr;
	size_t max_inflight_بائٹیں;
	size_t max_queued_بائٹیں;
	صحیح max_queued_messages;
	uint32_t max_packet_سائز;
	uint32_t پیغام_سائز_حد_بندی;
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
	bool قطار_qos0_messages;
	bool فی_listener_settings;
	bool retain_دستیاب;
	صحیح retain_expiry_interval;
	bool سیٹ_tcp_nodelay;
	صحیح sys_interval;
	bool upgrade_outgoing_qos;
	حرف *user;
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	صحیح websockets_log_level;
#اگر_ختم
	uint16_t packet_بفر_سائز;
#اگر_تعریف_شدہ WITH_BRIDGE
	ساخت mosquitto__bridge **bridges;
	صحیح bridge_گنتی;
#اگر_ختم
	ساخت mosquitto__security_options security_options;
};


ساخت mosquitto__subshared {
	UT_hash_handle hh;
	ساخت mosquitto__subleaf *subs;
	حرف name[];
};

ساخت mosquitto__subhier {
	UT_hash_handle hh;
	ساخت mosquitto__subhier *والد;
	ساخت mosquitto__subhier *children;
	ساخت mosquitto__subleaf *subs;
	ساخت mosquitto__subshared *shared;
	uint16_t topic_len;
	حرف topic[];
};

ساخت mosquitto__subleaf {
	ساخت mosquitto__subleaf *پچھلا;
	ساخت mosquitto__subleaf *اگلا;
	ساخت mosquitto *context;
	ساخت mosquitto__subhier *hier;
	ساخت mosquitto__subshared *shared;
	uint32_t identifier;
	uint8_t subscription_options;
	حرف topic_filter[];
};

ساخت sub__token {
	ساخت sub__token *اگلا;
	uint16_t topic_len;
	حرف topic[];
};

ساخت mosquitto__retainhier {
	UT_hash_handle hh;
	ساخت mosquitto__retainhier *والد;
	ساخت mosquitto__retainhier *children;
	ساخت mosquitto_بنیاد_پیغام *retained;
	uint16_t topic_len;
	حرف topic[];
};

ساخت mosquitto_بنیاد_پیغام {
	UT_hash_handle hh;
	ساخت mosquitto_بنیاد_پیغام_2 ڈیٹا;
	ساخت mosquitto__listener *ماخذ_listener;
	حرف **dest_ids;
	صحیح dest_id_گنتی;
	صحیح ref_گنتی;
	شمار mosquitto_پیغام_origin origin;
	bool stored;
};

ساخت mosquitto_client_پیغام {
	ساخت mosquitto_client_پیغام_2 ڈیٹا;
	ساخت mosquitto_client_پیغام *پچھلا;
	ساخت mosquitto_client_پیغام *اگلا;
	ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام;
};


ساخت mosquitto__psk {
	UT_hash_handle hh;
	حرف *username;
	حرف *password;
};

ساخت mosquitto_پیغام_v5 {
	ساخت mosquitto_پیغام_v5 *اگلا, *پچھلا;
	حرف *topic;
	خالی *payload;
	mosquitto_property *properties;
	/* clientid is used only by mosquitto_broker_publish*() to indicate
	   this message is for a specific client. */
	حرف *clientid;
	صحیح payloadlen;
	صحیح qos;
	bool retain;
};

ساخت mosquitto_db {
	dbid_t آخری_db_id;
	uint64_t گرہ_id_shifted;
	ساخت mosquitto__subhier *معمول_subs;
	ساخت mosquitto__subhier *shared_subs;
	ساخت mosquitto__retainhier *retains;
	ساخت mosquitto *contexts_by_id;
	ساخت mosquitto *contexts_by_sock;
	ساخت mosquitto *contexts_by_id_delayed_auth;
	ساخت mosquitto *contexts_for_free;
	mosquitto_plugin_id_t **plugins;
	صحیح plugin_گنتی;
#اگر_تعریف_شدہ WITH_BRIDGE
	ساخت mosquitto **bridges;
	صحیح bridge_گنتی;
#اگر_ختم
	ساخت clientid_اشاریہ_hash_2 *clientid_اشاریہ_hash;
	ساخت mosquitto_بنیاد_پیغام *پیغام_store;
	time_t now_s; /* Monotonic clock, where possible */
	time_t now_real_s; /* Read clock, for measuring session/message expiry */
	uint64_t گرہ_id; /* for unique db ids */
	time_t اگلا_event_ms; /* for mux timeout */
	صحیح پیغام_store_گنتی;
	غیرعلامتی طویل پیغام_store_بائٹیں;
	حرف *config_file;
	ساخت mosquitto__config *config;
	bool quiet;
	bool verbose;
#اگر_تعریف_شدہ WITH_SYS_درخت
	صحیح subscription_گنتی;
	صحیح shared_subscription_گنتی;
	صحیح retained_گنتی;
#اگر_ختم
	صحیح persistence_changes;
	ساخت mosquitto *ll_for_free;
#اگر_تعریف_شدہ WITH_EPOLL
	صحیح epollfd;
#اگر_ختم
#اگر_تعریف_شدہ WITH_KQUEUE
	صحیح kqueuefd;
#اگر_ختم
	ساخت mosquitto_پیغام_v5 *plugin_msgs;
#اگر_تعریف_شدہ WITH_TLS
	/* tls_keylog can't be in the config struct because it is used
	   before the config is allocated. Config probably
	   shouldn't be separately allocated. */
	حرف *tls_keylog;
#اگر_ختم
	bool shutdown;
};

شمار mosquitto__bridge_direction {
	bd_برآمد = 0,
	bd_اندر = 1,
	bd_both = 2,
};

شمار mosquitto_bridge_آغاز_type {
	bst_automatic = 0,
	bst_lazy = 1,
	bst_manual = 2,
	bst_once = 3,
};

شمار mosquitto_bridge_reload_type {
	brt_lazy = 0,
	brt_فوری = 1,
};

ساخت mosquitto__bridge_topic {
	ساخت mosquitto__bridge_topic *اگلا;
	حرف *topic;
	حرف *local_سابقہ;
	حرف *remote_سابقہ;
	حرف *local_topic; /* topic prefixed with local_prefix */
	حرف *remote_topic; /* topic prefixed with remote_prefix */
	شمار mosquitto__bridge_direction direction;
	uint8_t qos;
};

ساخت bridge_پتہ {
	حرف *پتہ;
	uint16_t پورٹ;
};

ساخت mosquitto__bridge {
	حرف *name;
	ساخت bridge_پتہ *پتے;
	صحیح cur_پتہ;
	صحیح پتہ_گنتی;
	time_t primary_retry;
	mosq_sock_t primary_retry_sock;
	bool باری_robin;
	bool try_نجی;
	bool try_نجی_accepted;
	bool clean_آغاز;
	int8_t clean_آغاز_local;
	uint16_t keepalive;
	غیرعلامتی صحیح tcp_keepalive_idle;
	غیرعلامتی صحیح tcp_keepalive_interval;
	غیرعلامتی صحیح tcp_keepalive_شمار;
#اگر_تعریف_شدہ WITH_TCP_USER_TIMEOUT
	صحیح tcp_user_timeout;
#اگر_ختم
	ساخت mosquitto__bridge_topic *topics;
	صحیح topic_گنتی;
	bool topic_remapping;
	شمار mosquitto__protocol protocol_نسخہ;
	time_t restart_t;
	time_t connected_at;
	حرف *remote_clientid;
	حرف *remote_username;
	حرف *remote_password;
	حرف *local_clientid;
	حرف *local_username;
	حرف *local_password;
	حرف *notification_topic;
	حرف *bind_پتہ;
	bool notifications;
	bool notifications_local_only;
	شمار mosquitto_bridge_آغاز_type آغاز_type;
	صحیح idle_timeout;
	صحیح restart_timeout;
	صحیح backoff_بنیاد;
	صحیح backoff_cap;
	صحیح stable_connection_period;
	صحیح حد;
	uint32_t زیادہ_سے_زیادہ_packet_سائز;
	uint32_t session_expiry_interval;
	uint16_t receive_زیادہ_سے_زیادہ;
	bool lazy_reconnect;
	bool attempt_unsubscribe;
	bool initial_notification_مکمل;
	bool outgoing_retain;
	شمار mosquitto_bridge_reload_type reload_type;
	uint16_t max_topic_alias;
#اگر_تعریف_شدہ WITH_TLS
	bool tls_insecure;
	bool tls_ocsp_required;
	bool tls_use_os_certs;
	حرف *tls_cafile;
	حرف *tls_capath;
	حرف *tls_certfile;
	حرف *tls_keyfile;
	حرف *tls_نسخہ;
	حرف *tls_alpn;
	حرف *tls_ciphers;
	حرف *tls_13_ciphers;
#  ifdef FINAL_WITH_TLS_PSK
	حرف *tls_psk_شناختی_عنصر;
	حرف *tls_psk;
#  endif
#اگر_ختم
};

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
ساخت libws_mqtt_hack {
	حرف *http_dir;
	ساخت mosquitto__listener *listener;
};

ساخت libws_mqtt_ڈیٹا {
	ساخت mosquitto *mosq;
};
#اگر_ختم

#شامل <net_mosq.h>


ساخت control_endpoint {
	ساخت control_endpoint *اگلا, *پچھلا;
	حرف topic[];
};

خارجی ساخت mosquitto_db db;

/* ============================================================
 * Main functions
 * ============================================================ */
صحیح mosquitto_main_loop(ساخت mosquitto__listener_sock *listensock, صحیح listensock_گنتی);
خالی loop_update_اگلا_event(time_t new_ms);

/* ============================================================
 * Config functions
 * ============================================================ */
/* Initialise config struct to default values. */
خالی config__init(ساخت mosquitto__config *config);
#اگر_تعریف_شدہ WITH_BRIDGE
خالی config__bridge_cleanup(ساخت mosquitto__bridge *bridge);
#اگر_ختم
/* Parse command line options into config. */
صحیح config_parse_دلائل(ساخت mosquitto__config *config, صحیح argc, حرف *argv[]);
/* Read configuration data from config->config_file into config.
 * If reload is true, don't process config options that shouldn't be reloaded (listeners etc)
 * Returns 0 on success, 1 if there is a configuration error or if a file cannot be opened.
 */
صحیح config__read(ساخت mosquitto__config *config, bool reload);
/* Free all config data. */
خالی config__cleanup(ساخت mosquitto__config *config);
صحیح config__get_dir_files(ثابت حرف *include_dir, حرف ***files, صحیح *file_گنتی);

/* ============================================================
 * Server send functions
 * ============================================================ */
صحیح send__connack(ساخت mosquitto *context, uint8_t ack, uint8_t reason_رمز, ثابت mosquitto_property *properties);
صحیح send__suback(ساخت mosquitto *context, uint16_t mid, uint32_t payloadlen, ثابت خالی *payload);
صحیح send__unsuback(ساخت mosquitto *context, uint16_t mid, صحیح reason_رمز_گنتی, uint8_t *reason_codes, ثابت mosquitto_property *properties);
صحیح send__auth(ساخت mosquitto *context, uint8_t reason_رمز, ثابت خالی *auth_ڈیٹا, uint16_t auth_ڈیٹا_len);

/* ============================================================
 * Network functions
 * ============================================================ */
خالی net__broker_init(خالی);
خالی net__broker_cleanup(خالی);
ساخت mosquitto *net__socket_accept(ساخت mosquitto__listener_sock *listensock);
صحیح net__socket_listen(ساخت mosquitto__listener *listener);
صحیح net_socket_get_پتہ(mosq_sock_t sock, حرف *buf, size_t len, uint16_t *remote_پتہ);
صحیح net_tls_بھرو_verify(ساخت mosquitto__listener *listener);
صحیح net__tls_server_ctx(ساخت mosquitto__listener *listener);
صحیح net_بھرو_certificates(ساخت mosquitto__listener *listener);

/* ============================================================
 * Read handling functions
 * ============================================================ */
صحیح handle__packet(ساخت mosquitto *context);
صحیح handle__connack(ساخت mosquitto *context);
صحیح handle__connect(ساخت mosquitto *context);
صحیح handle__disconnect(ساخت mosquitto *context);
صحیح handle__publish(ساخت mosquitto *context);
صحیح handle__subscribe(ساخت mosquitto *context);
صحیح handle__unsubscribe(ساخت mosquitto *context);
صحیح handle__auth(ساخت mosquitto *context);

/* ============================================================
 * Database handling
 * ============================================================ */
صحیح db__open(ساخت mosquitto__config *config);
صحیح db__close(خالی);
#اگر_تعریف_شدہ WITH_PERSISTENCE
صحیح persist__backup(bool shutdown);
صحیح persist__restore(خالی);
#اگر_ختم
/* Return the number of in-flight messages in count. */
صحیح db_پیغام_گنتی(صحیح *گنتی);
صحیح db_پیغام_delete_outgoing(ساخت mosquitto *context, uint16_t mid, شمار mosquitto_پیغام_state expect_state, صحیح qos);
صحیح db_پیغام_داخل_کرو_outgoing(ساخت mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام, uint32_t subscription_identifier, bool update, bool persist);
صحیح db_پیغام_داخل_کرو_incoming(ساخت mosquitto *context, uint64_t cmsg_id, ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام, bool persist);
صحیح db_پیغام_remove_incoming(ساخت mosquitto *context, uint16_t mid);
صحیح db_پیغام_release_incoming(ساخت mosquitto *context, uint16_t mid);
صحیح db_پیغام_update_outgoing(ساخت mosquitto *context, uint16_t mid, شمار mosquitto_پیغام_state state, صحیح qos, bool persist);
خالی db_پیغام_قطار_سے_نکالو_پہلا(ساخت mosquitto *context, ساخت mosquitto_پیغام_ڈیٹا *پیغام_ڈیٹا);
صحیح db__messages_delete(ساخت mosquitto *context, bool force_free);
صحیح db__messages_delete_incoming(ساخت mosquitto *context);
صحیح db__messages_delete_outgoing(ساخت mosquitto *context);
صحیح db_messages_easy_قطار(ساخت mosquitto *context, ثابت حرف *topic, uint8_t qos, uint32_t payloadlen, ثابت خالی *payload, صحیح retain, uint32_t پیغام_expiry_interval, mosquitto_property **properties);
صحیح db_پیغام_store(ثابت ساخت mosquitto *ماخذ, ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام, uint32_t *پیغام_expiry_interval, شمار mosquitto_پیغام_origin origin);
صحیح db_پیغام_store_ڈھونڈو(ساخت mosquitto *context, uint16_t mid, ساخت mosquitto_client_پیغام **client_پیغام);
صحیح db_پیغام_store_جوڑو(ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام);
خالی db_پیغام_store_remove(ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام, bool notify);
خالی db_پیغام_store_ref_inc(ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام);
خالی db_پیغام_store_ref_dec(ساخت mosquitto_بنیاد_پیغام **بنیاد_پیغام);
خالی db_پیغام_store_clean(خالی);
خالی db_پیغام_store_compact(خالی);
خالی db_پیغام_store_free(ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام);
صحیح db_پیغام_reconnect_reset(ساخت mosquitto *context);
bool db__ready_for_flight(ساخت mosquitto *context, شمار mosquitto_پیغام_direction dir, صحیح qos);
bool db_ready_for_قطار(ساخت mosquitto *context, صحیح qos, ساخت mosquitto_پیغام_ڈیٹا *پیغام_ڈیٹا);
خالی sys_درخت_init(خالی);
خالی sys_درخت_update(bool force);
صحیح db_پیغام_write_inflight_برآمد_all(ساخت mosquitto *context);
صحیح db_پیغام_write_inflight_برآمد_latest(ساخت mosquitto *context);
صحیح db_پیغام_write_queued_برآمد(ساخت mosquitto *context);
صحیح db_پیغام_write_queued_اندر(ساخت mosquitto *context);
خالی db_پیغام_جوڑو_مقصد_inflight_stats(ساخت mosquitto_پیغام_ڈیٹا *پیغام_ڈیٹا, ساخت mosquitto_client_پیغام *پیغام);
خالی db_پیغام_جوڑو_مقصد_queued_stats(ساخت mosquitto_پیغام_ڈیٹا *پیغام_ڈیٹا, ساخت mosquitto_client_پیغام *پیغام);
uint64_t db_new_پیغام_id(خالی);
خالی db__expire_all_messages(ساخت mosquitto *context);
خالی db_check_acl_کا_all_messages(ساخت mosquitto *context);

/* ============================================================
 * Subscription functions
 * ============================================================ */
صحیح sub__init(خالی);
صحیح sub_جوڑو(ساخت mosquitto *context, ثابت ساخت mosquitto_subscription *sub);
صحیح sub__remove(ساخت mosquitto *context, ثابت حرف *sub, uint8_t *reason);
خالی sub_درخت_print(ساخت mosquitto__subhier *جڑ, صحیح level);
صحیح sub__clean_session(ساخت mosquitto *context);
صحیح sub_messages_قطار(ثابت حرف *ماخذ_id, uint8_t qos, ساخت mosquitto_بنیاد_پیغام **بنیاد_پیغام);
صحیح sub__topic_tokenise(ثابت حرف *subtopic, حرف **local_sub, حرف ***topics, ثابت حرف **sharename);
خالی sub_topic_علامات_free(ساخت sub__token *علامات);

/* ============================================================
 * Context functions
 * ============================================================ */
ساخت mosquitto *context__init(خالی);
صحیح context__init_sock(ساخت mosquitto *context, mosq_sock_t sock, bool get_پتہ);
خالی context__cleanup(ساخت mosquitto *context, bool force_free);
خالی context__disconnect(ساخت mosquitto *context, صحیح reason);
خالی context_جوڑو_مقصد_disused(ساخت mosquitto *context);
خالی context__free_disused(خالی);
خالی context__send_will(ساخت mosquitto *context);
خالی context_جوڑو_مقصد_by_id(ساخت mosquitto *context);
خالی context__remove_from_by_id(ساخت mosquitto *context);

صحیح connect__on_authorised(ساخت mosquitto *context, خالی *auth_ڈیٹا_برآمد, uint16_t auth_ڈیٹا_برآمد_len);


/* ============================================================
 * Control functions
 * ============================================================ */
#اگر_تعریف_شدہ WITH_CONTROL
صحیح control__process(ساخت mosquitto *context, ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام);
خالی control__cleanup(خالی);
#اگر_ختم
صحیح control_رجسٹر_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ثابت حرف *topic, خالی *userdata);
صحیح control__unregister_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ثابت حرف *topic);
خالی control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier);


/* ============================================================
 * Logging functions
 * ============================================================ */
صحیح log__init(ساخت mosquitto__config *config);
صحیح log__close(ساخت mosquitto__config *config);
خالی log_اندرونی(ثابت حرف *fmt, ...) __attribute__((format(لکھو, 1, 2)));

/* ============================================================
 * Bridge functions
 * ============================================================ */
#اگر_تعریف_شدہ WITH_BRIDGE
خالی bridge_آغاز_all(خالی);
خالی bridge__reload(خالی);
خالی bridge__db_cleanup(خالی);
خالی bridge__cleanup(ساخت mosquitto *context);
صحیح bridge__connect(ساخت mosquitto *context);
#اگر defined(__GLIBC__) && defined(WITH_ADNS)
صحیح bridge__connect_step3(ساخت mosquitto *context);
#اگر_ختم
صحیح bridge__on_connect(ساخت mosquitto *context);
خالی bridge_check(خالی);
صحیح bridge_رجسٹر_local_connections(خالی);
صحیح bridge_جوڑو_topic(ساخت mosquitto__bridge *bridge, ثابت حرف *topic, شمار mosquitto__bridge_direction direction, uint8_t qos, ثابت حرف *local_سابقہ, ثابت حرف *remote_سابقہ);
خالی bridge__cleanup_topics(ساخت mosquitto__bridge *bridge);
صحیح bridge_remap_topic_اندر(ساخت mosquitto *context, حرف **topic);
#اگر_ختم

/* ============================================================
 * IO multiplex related functions
 * ============================================================ */
صحیح mux__init(خالی);
صحیح mux_جوڑو_listeners(ساخت mosquitto__listener_sock *listensock, صحیح listensock_گنتی);
صحیح mux__delete_listeners(ساخت mosquitto__listener_sock *listensock, صحیح listensock_گنتی);
صحیح mux__loop_prepare(خالی);
صحیح mux__new(ساخت mosquitto *context);
صحیح mux_جوڑو_برآمد(ساخت mosquitto *context);
صحیح mux_remove_برآمد(ساخت mosquitto *context);
صحیح mux__delete(ساخت mosquitto *context);
صحیح mux__wait(خالی);
صحیح mux__handle(ساخت mosquitto__listener_sock *listensock, صحیح listensock_گنتی);
صحیح mux__cleanup(خالی);

/* ============================================================
 * Listener related functions
 * ============================================================ */
خارجی ساخت mosquitto__listener_sock *g_listensock;
خارجی صحیح g_listensock_گنتی;

خالی listener_سیٹ_defaults(ساخت mosquitto__listener *listener);
خالی listeners__reload_all_certificates(خالی);
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
خالی listeners_جوڑو_websockets(ساخت lws_context *ws_context, mosq_sock_t fd);
#اگر_ختم
صحیح listeners_آغاز(خالی);
خالی listeners__stop(خالی);

/* ============================================================
 * Plugin related functions
 * ============================================================ */
صحیح plugin_بھرو_v5(mosquitto_plugin_id_t *plugin, خالی *lib);
صحیح plugin_بھرو_v4(mosquitto_plugin_id_t *plugin, خالی *lib);
صحیح plugin_بھرو_v3(mosquitto_plugin_id_t *plugin, خالی *lib);
صحیح plugin_بھرو_v2(mosquitto_plugin_id_t *plugin, خالی *lib);
صحیح acl__pre_check(mosquitto_plugin_id_t *plugin, ساخت mosquitto *context, صحیح access);

خالی LIB_ERROR(خالی);
خالی plugin__handle_connect(ساخت mosquitto *context);
خالی plugin__handle_disconnect(ساخت mosquitto *context, صحیح reason);
خالی plugin__handle_client_offline(ساخت mosquitto *context, صحیح reason);
صحیح plugin_handle_پیغام_اندر(ساخت mosquitto *context, ساخت mosquitto_بنیاد_پیغام_2 *بنیاد_پیغام);
صحیح plugin_handle_پیغام_برآمد(ساخت mosquitto *context, ساخت mosquitto_بنیاد_پیغام_2 *بنیاد_پیغام);
صحیح plugin__handle_subscribe(ساخت mosquitto *context, ساخت mosquitto_subscription *sub);
صحیح plugin__handle_unsubscribe(ساخت mosquitto *context, ساخت mosquitto_subscription *sub);
صحیح plugin__handle_reload(خالی);
خالی plugin__handle_tick(خالی);
صحیح plugin__callback_unregister_all(mosquitto_plugin_id_t *identifier);
خالی plugin_persist__handle_restore(خالی);
خالی plugin_persist_handle_client_جوڑو(ساخت mosquitto *context);
خالی plugin_persist__handle_client_delete(ساخت mosquitto *context);
خالی plugin_persist__handle_client_update(ساخت mosquitto *context);
خالی plugin_persist_handle_subscription_جوڑو(ساخت mosquitto *context, ثابت ساخت mosquitto_subscription *sub);
خالی plugin_persist__handle_subscription_delete(ساخت mosquitto *context, حرف *sub);
خالی plugin_persist_handle_client_پیغام_جوڑو(ساخت mosquitto *context, ثابت ساخت mosquitto_client_پیغام *cmsg);
خالی plugin_persist_handle_client_پیغام_delete(ساخت mosquitto *context, ثابت ساخت mosquitto_client_پیغام *cmsg);
خالی plugin_persist_handle_client_پیغام_update(ساخت mosquitto *context, ثابت ساخت mosquitto_client_پیغام *cmsg);
خالی plugin_persist_handle_بنیاد_پیغام_جوڑو(ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام);
خالی plugin_persist_handle_بنیاد_پیغام_delete(ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام);
خالی plugin_persist_handle_retain_پیغام_سیٹ(ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام);
خالی plugin_persist_handle_retain_پیغام_delete(ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام);
خالی plugin_persist_handle_will_جوڑو(ساخت mosquitto *context);
خالی plugin_persist__handle_will_delete(ساخت mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
صحیح keepalive__init(خالی);
خالی keepalive__cleanup(خالی);
صحیح keepalive_جوڑو(ساخت mosquitto *context);
خالی keepalive__check(خالی);
صحیح keepalive__remove(ساخت mosquitto *context);
صحیح keepalive__update(ساخت mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
صحیح property__process_connect(ساخت mosquitto *context, mosquitto_property **props);
صحیح property__process_will(ساخت mosquitto *context, ساخت mosquitto_پیغام_all *پیغام, mosquitto_property **props);
صحیح property__process_publish(ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام, mosquitto_property **props, صحیح *topic_alias, uint32_t *پیغام_expiry_interval, bool is_bridge);
صحیح property__process_disconnect(ساخت mosquitto *context, mosquitto_property **props);

/* ============================================================
 * Retain tree related functions
 * ============================================================ */
صحیح retain__init(خالی);
خالی retain__clean(ساخت mosquitto__retainhier **retainhier);
صحیح retain_قطار(ساخت mosquitto *context, ثابت ساخت mosquitto_subscription *sub);
صحیح retain__store(ثابت حرف *topic, ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام, حرف **بانٹو_topics, bool persist);
خالی retain__expiry_check(خالی);
خالی retain__expire(ساخت mosquitto__retainhier **retainhier);

/* ============================================================
 * Security related functions
 * ============================================================ */
صحیح acl_ڈھونڈو_acls(ساخت mosquitto *context);
صحیح plugin_بھرو_all(خالی);
صحیح plugin__unload_all(خالی);
صحیح config_plugin_جوڑو_secopt(mosquitto_plugin_id_t *plugin, ساخت mosquitto__security_options *security_options);

صحیح mosquitto_security_init(bool reload);
صحیح mosquitto_security_cleanup(bool reload);
صحیح mosquitto_acl_check(ساخت mosquitto *context, ثابت حرف *topic, uint32_t payloadlen, خالی *payload, uint8_t qos, bool retain, mosquitto_property *properties, صحیح access);
صحیح mosquitto_basic_auth(ساخت mosquitto *context);
صحیح mosquitto_psk_کلید_get(ساخت mosquitto *context, ثابت حرف *hint, ثابت حرف *شناختی_عنصر, حرف *کلید, صحیح max_کلید_len);

صحیح mosquitto_security_init_default(خالی);
صحیح mosquitto_security_apply_default(خالی);
صحیح mosquitto_security_cleanup_default(خالی);
صحیح mosquitto_psk_کلید_get_default(ساخت mosquitto *context, ثابت حرف *hint, ثابت حرف *شناختی_عنصر, حرف *کلید, صحیح max_کلید_len);
صحیح broker_acl_file__init(خالی);
خالی broker_acl_file__cleanup(خالی);
صحیح broker_password_file__init(خالی);
خالی broker_password_file__cleanup(خالی);
صحیح psk_file__init(خالی);
صحیح psk_file__cleanup(خالی);

صحیح mosquitto_security_auth_آغاز(ساخت mosquitto *context, bool reauth, ثابت خالی *ڈیٹا_اندر, uint16_t ڈیٹا_اندر_len, خالی **ڈیٹا_برآمد, uint16_t *ڈیٹا_برآمد_len);
صحیح mosquitto_security_auth_continue(ساخت mosquitto *context, ثابت خالی *ڈیٹا_اندر, uint16_t ڈیٹا_len, خالی **ڈیٹا_برآمد, uint16_t *ڈیٹا_برآمد_len);

خالی unpwd_free_شے(ساخت mosquitto__unpwd **unpwd, ساخت mosquitto__unpwd *شے);

/* ============================================================
 * Session expiry
 * ============================================================ */
صحیح session_expiry_جوڑو(ساخت mosquitto *context);
صحیح session_expiry_جوڑو_from_persistence(ساخت mosquitto *context, time_t expiry_وقت);
خالی session_expiry_جوڑو_on_shutdown(خالی);
خالی session_expiry__remove(ساخت mosquitto *context);
خالی session_expiry__remove_all(خالی);
خالی session_expiry__check(خالی);
خالی session_expiry__send_all(خالی);

/* ============================================================
 * Signals
 * ============================================================ */
خالی signal_تیاری(خالی);
صحیح signal__flag_check(خالی);

/* ============================================================
 * Window service and signal related functions
 * ============================================================ */
#اگر defined(WIN32) || defined(__CYGWIN__)
خالی service_install(حرف *name);
خالی service_uninstall(حرف *name);
خالی service_تسلسل(حرف *name);

#اگر_تعریف_شدہ WIN32
DWORD WINAPI SigThreadProc(خالی *ڈیٹا);
#اگر_ختم
#اگر_ختم

/* ============================================================
 * Watchdog
 * ============================================================ */
خالی watchdog__init(خالی);
خالی watchdog__check(خالی);

/* ============================================================
 * Websockets related functions
 * ============================================================ */
#اگر_تعریف_شدہ WITH_WEBSOCKETS
#  اگر WITH_WEBSOCKETS == WS_IS_LWS
خالی mosq_websockets_init(ساخت mosquitto__listener *listener, ثابت ساخت mosquitto__config *conf);
#  endif
صحیح http__context_init(ساخت mosquitto *context);
صحیح http__context_cleanup(ساخت mosquitto *context);
صحیح http__read(ساخت mosquitto *context);
#اگر_ختم
خالی do_disconnect(ساخت mosquitto *context, صحیح reason);

/* ============================================================
 * PROXY related functions
 * ============================================================ */
صحیح proxy_v2__read(ساخت mosquitto *context);
صحیح proxy_v1__read(ساخت mosquitto *context);

/* ============================================================
 * Will delay
 * ============================================================ */
صحیح will_delay_جوڑو(ساخت mosquitto *context);
خالی will_delay__check(خالی);
خالی will_delay__send_all(خالی);
خالی will_delay__remove(ساخت mosquitto *mosq);

/* ============================================================
 * HTTP Info
 * ============================================================ */
صحیح http_api_آغاز_local(ساخت mosquitto__listener *listener);
صحیح http_api_آغاز(ساخت mosquitto__listener *listener);
خالی http_api__stop(ساخت mosquitto__listener *listener);


/* ============================================================
 * Other
 * ============================================================ */
#اگر_تعریف_شدہ WITH_XTREPORT
خالی xtreport(خالی);
#اگر_ختم

خالی broker_control__init(خالی);
خالی broker_control__cleanup(خالی);
خالی broker_control__reload(خالی);

#اگر_ختم
