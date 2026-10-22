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

#اگر_تعریف_نشده MOSQUITTO_BROKER_داخلی_H
#تعریف_کن MOSQUITTO_BROKER_داخلی_H

#ضمن "config.h"
#ضمن <stdio.h>

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#  إذا LWS_LIBRARY_نسخه_عدد >= 3002000 && !defined(LWS_WITH_EXTERNAL_POLL)
#    warning "libwebsockets is not compiled with LWS_WITH_EXTERNAL_POLL support. Websocket performance will be unusable."
#  endif
#پایان_اگر

#اگر_تعریف_شده WITH_HTTP_API
#  include <microhttpd.h>
#پایان_اگر

#اگر_تعریف_شده __linux__
#تعریف_کن WITH_TCP_USER_TIMEOUT
#پایان_اگر

#ضمن "mosquitto_internal.h"
#ضمن "mosquitto/broker.h"
#ضمن "mosquitto/broker_plugin.h"
#ضمن "mosquitto.h"
#ضمن "logging_mosq.h"
#ضمن "tls_mosq.h"
#ضمن "uthash.h"
#ضمن "acl_file.h"
#ضمن "password_file.h"

#اگر_تعریف_نشده __GNUC__
#تعریف_کن __attribute__(attrib)
#پایان_اگر

/* Log destinations */
#تعریف_کن MQTT3_LOG_NONE 0x00
#تعریف_کن MQTT3_LOG_SYSLOG 0x01
#تعریف_کن MQTT3_LOG_FILE 0x02
#تعریف_کن MQTT3_LOG_STDOUT 0x04
#تعریف_کن MQTT3_LOG_STDERR 0x08
#تعریف_کن MQTT3_LOG_TOPIC 0x10
#تعریف_کن MQTT3_LOG_DLT 0x20
#اگر_تعریف_شده ANDROID
#تعریف_کن MQTT3_LOG_ANDROID 0x40
#پایان_اگر
#تعریف_کن MQTT3_LOG_ALL 0xFF

#تعریف_کن CMD_پورت_حد 10

تعريفنوع uint64_t dbid_t;

تعريفنوع صحيح (*FUNC_plugin_init_v5)(mosquitto_plugin_id_t *, فراغ **, بنية mosquitto_opt *, صحيح);
تعريفنوع صحيح (*FUNC_plugin_cleanup_v5)(فراغ *, بنية mosquitto_opt *, صحيح);

تعريفنوع صحيح (*FUNC_auth_plugin_init_v4)(فراغ **, بنية mosquitto_opt *, صحيح);
تعريفنوع صحيح (*FUNC_auth_plugin_cleanup_v4)(فراغ *, بنية mosquitto_opt *, صحيح);
تعريفنوع صحيح (*FUNC_auth_plugin_security_init_v4)(فراغ *, بنية mosquitto_opt *, صحيح, bool);
تعريفنوع صحيح (*FUNC_auth_plugin_security_cleanup_v4)(فراغ *, بنية mosquitto_opt *, صحيح, bool);
تعريفنوع صحيح (*FUNC_auth_plugin_acl_check_v4)(فراغ *, صحيح, بنية mosquitto *, بنية mosquitto_acl_پیام *);
تعريفنوع صحيح (*FUNC_auth_plugin_unpwd_check_v4)(فراغ *, بنية mosquitto *, ثابت حرف *, ثابت حرف *);
تعريفنوع صحيح (*FUNC_auth_plugin_psk_کلید_get_v4)(فراغ *, بنية mosquitto *, ثابت حرف *, ثابت حرف *, حرف *, صحيح);
تعريفنوع صحيح (*FUNC_auth_plugin_auth_شروع_v4)(فراغ *, بنية mosquitto *, ثابت حرف *, bool, ثابت فراغ *, uint16_t, فراغ **, uint16_t *);
تعريفنوع صحيح (*FUNC_auth_plugin_auth_continue_v4)(فراغ *, بنية mosquitto *, ثابت حرف *, ثابت فراغ *, uint16_t, فراغ **, uint16_t *);

تعريفنوع صحيح (*FUNC_auth_plugin_init_v3)(فراغ **, بنية mosquitto_opt *, صحيح);
تعريفنوع صحيح (*FUNC_auth_plugin_cleanup_v3)(فراغ *, بنية mosquitto_opt *, صحيح);
تعريفنوع صحيح (*FUNC_auth_plugin_security_init_v3)(فراغ *, بنية mosquitto_opt *, صحيح, bool);
تعريفنوع صحيح (*FUNC_auth_plugin_security_cleanup_v3)(فراغ *, بنية mosquitto_opt *, صحيح, bool);
تعريفنوع صحيح (*FUNC_auth_plugin_acl_check_v3)(فراغ *, صحيح, ثابت بنية mosquitto *, بنية mosquitto_acl_پیام *);
تعريفنوع صحيح (*FUNC_auth_plugin_unpwd_check_v3)(فراغ *, ثابت بنية mosquitto *, ثابت حرف *, ثابت حرف *);
تعريفنوع صحيح (*FUNC_auth_plugin_psk_کلید_get_v3)(فراغ *, ثابت بنية mosquitto *, ثابت حرف *, ثابت حرف *, حرف *, صحيح);

تعريفنوع صحيح (*FUNC_auth_plugin_init_v2)(فراغ **, بنية mosquitto_auth_opt *, صحيح);
تعريفنوع صحيح (*FUNC_auth_plugin_cleanup_v2)(فراغ *, بنية mosquitto_auth_opt *, صحيح);
تعريفنوع صحيح (*FUNC_auth_plugin_security_init_v2)(فراغ *, بنية mosquitto_auth_opt *, صحيح, bool);
تعريفنوع صحيح (*FUNC_auth_plugin_security_cleanup_v2)(فراغ *, بنية mosquitto_auth_opt *, صحيح, bool);
تعريفنوع صحيح (*FUNC_auth_plugin_acl_check_v2)(فراغ *, ثابت حرف *, ثابت حرف *, ثابت حرف *, صحيح);
تعريفنوع صحيح (*FUNC_auth_plugin_unpwd_check_v2)(فراغ *, ثابت حرف *, ثابت حرف *);
تعريفنوع صحيح (*FUNC_auth_plugin_psk_کلید_get_v2)(فراغ *, ثابت حرف *, ثابت حرف *, حرف *, صحيح);


تعداد mosquitto_پیام_origin {
	mosq_mo_client = 0,
	mosq_mo_broker = 1,
};

بنية mosquitto__plugin_lib {
	فراغ *lib;
	فراغ *user_داده;
	صحيح (*plugin_نسخه)(فراغ);
	بنية mosquitto_plugin_id_t *identifier;

	FUNC_plugin_init_v5 plugin_init_v5;
	FUNC_plugin_cleanup_v5 plugin_cleanup_v5;

	FUNC_auth_plugin_init_v4 plugin_init_v4;
	FUNC_auth_plugin_cleanup_v4 plugin_cleanup_v4;
	FUNC_auth_plugin_security_init_v4 security_init_v4;
	FUNC_auth_plugin_security_cleanup_v4 security_cleanup_v4;
	FUNC_auth_plugin_acl_check_v4 acl_check_v4;
	FUNC_auth_plugin_unpwd_check_v4 unpwd_check_v4;
	FUNC_auth_plugin_psk_کلید_get_v4 psk_کلید_get_v4;
	FUNC_auth_plugin_auth_شروع_v4 auth_شروع_v4;
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
	صحيح نسخه_2;
};

بنية mosquitto__plugin_config {
	حرف *مسیر;
	حرف *name;
	بنية mosquitto_opt *options;
	بنية mosquitto__security_options **security_options;
	صحيح option_شمار;
	صحيح security_option_شمار;
	bool deny_special_chars;
};

بنية mosquitto__callback {
	UT_hash_handle hh; /* For callbacks that register for e.g. a specific topic */
	بنية mosquitto__callback *بعدی, *prev; /* For typical callbacks */
	MOSQ_FUNC_generic_callback cb;
	فراغ *userdata;
	اتحاد {
		حرف *topic;
		بنية timespec بعدی_tick;
	} داده;
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
	بنية mosquitto__callback *ext_auth_شروع;
	بنية mosquitto__callback *پیام_درون;
	بنية mosquitto__callback *پیام_بیرون;
	بنية mosquitto__callback *psk_کلید;
	بنية mosquitto__callback *reload;
	بنية mosquitto__callback *subscribe;
	بنية mosquitto__callback *unsubscribe;
	بنية mosquitto__callback *persist_restore;
	بنية mosquitto__callback *persist_client_جمع_کن;
	بنية mosquitto__callback *persist_client_delete;
	بنية mosquitto__callback *persist_client_update;
	بنية mosquitto__callback *persist_subscription_جمع_کن;
	بنية mosquitto__callback *persist_subscription_delete;
	بنية mosquitto__callback *persist_client_پیام_جمع_کن;
	بنية mosquitto__callback *persist_client_پیام_delete;
	بنية mosquitto__callback *persist_client_پیام_update;
	بنية mosquitto__callback *persist_پایه_پیام_جمع_کن;
	بنية mosquitto__callback *persist_پایه_پیام_delete;
	بنية mosquitto__callback *persist_retain_پیام_مجموعه;
	بنية mosquitto__callback *persist_retain_پیام_delete;
	بنية mosquitto__callback *persist_will_جمع_کن;
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
	بنية acl_file_داده acl_داده;
	بنية password_file_داده password_داده;
	حرف *psk_file;
	mosquitto_plugin_id_t **plugins;
	صحيح plugin_شمار;
	int8_t allow_anonymous;
	bool allow_zero_درازی_clientid;
	حرف *auto_id_پیشوند;
	uint16_t auto_id_پیشوند_len;
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
#پایان_اگر

بنية mosquitto__listener {
	uint16_t پورت;
	حرف *host;
	حرف *bind_interface;
	صحيح max_connections;
	حرف *mount_نقطه;
	mosq_sock_t *socks;
	صحيح sock_شمار;
	صحيح client_شمار;
	تعداد mosquitto_protocol protocol;
	صحيح socket_domain;
	bool use_username_as_clientid;
	uint8_t max_qos;
	uint16_t max_topic_alias;
	uint16_t max_topic_alias_broker;
#اگر_تعریف_شده WITH_TLS
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
	حرف *tls_نسخه;
	bool use_عنصر_خنثی_as_username;
	bool use_subject_as_username;
	bool require_certificate;
	bool disable_client_cert_date_checks;
	تعداد mosquitto__keyform tls_keyform;
#پایان_اگر
#اگر_تعریف_شده WITH_WEBSOCKETS
#  إذا WITH_WEBSOCKETS == WS_IS_LWS
	بنية lws_context *ws_context;
	bool ws_درون_init;
	بنية lws_protocols *ws_protocol;
#  endif
	حرف **ws_origins;
	صحيح ws_origin_شمار;
#پایان_اگر
#إذا defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
	حرف *http_dir;
#پایان_اگر
	بنية mosquitto__security_options *security_options;
#اگر_تعریف_شده WITH_UNIX_SOCKETS
	حرف *unix_socket_مسیر;
#پایان_اگر
	bool disable_protocol_v3;
	bool disable_protocol_v4;
	bool disable_protocol_v5;
	صحيح enable_proxy_protocol;
	bool proxy_protocol_v2_require_tls;
#اگر_تعریف_شده WITH_HTTP_API
	بنية MHD_Daemon *mhd;
#پایان_اگر
};


بنية mosquitto__listener_sock {
#إذا defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	/* This *must* be the first element in the struct. */
	صحيح ident;
#پایان_اگر
	mosq_sock_t sock;
	بنية mosquitto__listener *listener;
};


/* Callbacks belonging to a specific plugin
 * Doesn't include MOSQ_EVT_CONTROL events.
 */
بنية plugin_own_callback {
	بنية plugin_own_callback *بعدی, *prev;
	MOSQ_FUNC_generic_callback cb_func;
	تعداد mosquitto_plugin_event event;
};

بنية mosquitto_plugin_id_t {
	بنية mosquitto__plugin_config config;
	بنية mosquitto__plugin_lib lib;
	بنية mosquitto__listener *listener;
	حرف *plugin_name;
	حرف *plugin_نسخه;
	بنية control_endpoint *control_endpoints;
	بنية plugin_own_callback *own_callbacks;
	بنية timespec بعدی_tick;
};

بنية mosquitto__config {
	bool allow_duplicate_messages;
	صحيح autosave_interval;
	bool autosave_on_changes;
	bool check_retain_منبع;
	حرف *clientid_prefixes;
	bool connection_messages;
	uint16_t cmd_پورت[CMD_پورت_حد];
	صحيح cmd_پورت_شمار;
	bool daemon;
	bool test_configuration;
	bool enable_control_api;
	صحيح global_max_clients;
	صحيح global_max_connections;
	بنية mosquitto__listener *default_listener; /* Points to one of `listeners` */
	بنية mosquitto__listener *listeners;
	صحيح listener_شمار;
	bool local_only;
	بیعلامت صحيح log_dest;
	صحيح log_facility;
	بیعلامت صحيح log_type;
	bool log_timestamp;
	حرف *log_timestamp_format;
	حرف *log_file;
	FILE *log_fptr;
	size_t max_inflight_بایتها;
	size_t max_queued_بایتها;
	صحيح max_queued_messages;
	uint32_t max_packet_اندازه;
	uint32_t پیام_اندازه_حد;
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
	bool صف_qos0_messages;
	bool در_هر_listener_settings;
	bool retain_موجود;
	صحيح retain_expiry_interval;
	bool مجموعه_tcp_nodelay;
	صحيح sys_interval;
	bool upgrade_outgoing_qos;
	حرف *user;
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	صحيح websockets_log_level;
#پایان_اگر
	uint16_t packet_میانگیر_اندازه;
#اگر_تعریف_شده WITH_BRIDGE
	بنية mosquitto__bridge **bridges;
	صحيح bridge_شمار;
#پایان_اگر
	بنية mosquitto__security_options security_options;
};


بنية mosquitto__subshared {
	UT_hash_handle hh;
	بنية mosquitto__subleaf *subs;
	حرف name[];
};

بنية mosquitto__subhier {
	UT_hash_handle hh;
	بنية mosquitto__subhier *والد;
	بنية mosquitto__subhier *children;
	بنية mosquitto__subleaf *subs;
	بنية mosquitto__subshared *shared;
	uint16_t topic_len;
	حرف topic[];
};

بنية mosquitto__subleaf {
	بنية mosquitto__subleaf *prev;
	بنية mosquitto__subleaf *بعدی;
	بنية mosquitto *context;
	بنية mosquitto__subhier *hier;
	بنية mosquitto__subshared *shared;
	uint32_t identifier;
	uint8_t subscription_options;
	حرف topic_filter[];
};

بنية sub__token {
	بنية sub__token *بعدی;
	uint16_t topic_len;
	حرف topic[];
};

بنية mosquitto__retainhier {
	UT_hash_handle hh;
	بنية mosquitto__retainhier *والد;
	بنية mosquitto__retainhier *children;
	بنية mosquitto_پایه_پیام *retained;
	uint16_t topic_len;
	حرف topic[];
};

بنية mosquitto_پایه_پیام {
	UT_hash_handle hh;
	بنية mosquitto_پایه_پیام_2 داده;
	بنية mosquitto__listener *منبع_listener;
	حرف **dest_ids;
	صحيح dest_id_شمار;
	صحيح ref_شمار;
	تعداد mosquitto_پیام_origin origin;
	bool stored;
};

بنية mosquitto_client_پیام {
	بنية mosquitto_client_پیام_2 داده;
	بنية mosquitto_client_پیام *prev;
	بنية mosquitto_client_پیام *بعدی;
	بنية mosquitto_پایه_پیام *پایه_پیام;
};


بنية mosquitto__psk {
	UT_hash_handle hh;
	حرف *username;
	حرف *password;
};

بنية mosquitto_پیام_v5 {
	بنية mosquitto_پیام_v5 *بعدی, *prev;
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
	dbid_t آخرین_db_id;
	uint64_t گره_id_shifted;
	بنية mosquitto__subhier *نرمال_subs;
	بنية mosquitto__subhier *shared_subs;
	بنية mosquitto__retainhier *retains;
	بنية mosquitto *contexts_by_id;
	بنية mosquitto *contexts_by_sock;
	بنية mosquitto *contexts_by_id_delayed_auth;
	بنية mosquitto *contexts_for_free;
	mosquitto_plugin_id_t **plugins;
	صحيح plugin_شمار;
#اگر_تعریف_شده WITH_BRIDGE
	بنية mosquitto **bridges;
	صحيح bridge_شمار;
#پایان_اگر
	بنية clientid_شاخص_hash_2 *clientid_شاخص_hash;
	بنية mosquitto_پایه_پیام *پیام_store;
	time_t now_s; /* Monotonic clock, where possible */
	time_t now_real_s; /* Read clock, for measuring session/message expiry */
	uint64_t گره_id; /* for unique db ids */
	time_t بعدی_event_ms; /* for mux timeout */
	صحيح پیام_store_شمار;
	بیعلامت دراز پیام_store_بایتها;
	حرف *config_file;
	بنية mosquitto__config *config;
	bool quiet;
	bool verbose;
#اگر_تعریف_شده WITH_SYS_درخت
	صحيح subscription_شمار;
	صحيح shared_subscription_شمار;
	صحيح retained_شمار;
#پایان_اگر
	صحيح persistence_changes;
	بنية mosquitto *ll_for_free;
#اگر_تعریف_شده WITH_EPOLL
	صحيح epollfd;
#پایان_اگر
#اگر_تعریف_شده WITH_KQUEUE
	صحيح kqueuefd;
#پایان_اگر
	بنية mosquitto_پیام_v5 *plugin_msgs;
#اگر_تعریف_شده WITH_TLS
	/* tls_keylog can't be in the config struct because it is used
	   before the config is allocated. Config probably
	   shouldn't be separately allocated. */
	حرف *tls_keylog;
#پایان_اگر
	bool shutdown;
};

تعداد mosquitto__bridge_direction {
	bd_بیرون = 0,
	bd_درون = 1,
	bd_both = 2,
};

تعداد mosquitto_bridge_شروع_type {
	bst_automatic = 0,
	bst_lazy = 1,
	bst_manual = 2,
	bst_once = 3,
};

تعداد mosquitto_bridge_reload_type {
	brt_lazy = 0,
	brt_فوری = 1,
};

بنية mosquitto__bridge_topic {
	بنية mosquitto__bridge_topic *بعدی;
	حرف *topic;
	حرف *local_پیشوند;
	حرف *remote_پیشوند;
	حرف *local_topic; /* topic prefixed with local_prefix */
	حرف *remote_topic; /* topic prefixed with remote_prefix */
	تعداد mosquitto__bridge_direction direction;
	uint8_t qos;
};

بنية bridge_آدرس {
	حرف *آدرس;
	uint16_t پورت;
};

بنية mosquitto__bridge {
	حرف *name;
	بنية bridge_آدرس *آدرسها;
	صحيح cur_آدرس;
	صحيح آدرس_شمار;
	time_t primary_retry;
	mosq_sock_t primary_retry_sock;
	bool دور_robin;
	bool try_خصوصی;
	bool try_خصوصی_accepted;
	bool clean_شروع;
	int8_t clean_شروع_local;
	uint16_t keepalive;
	بیعلامت صحيح tcp_keepalive_idle;
	بیعلامت صحيح tcp_keepalive_interval;
	بیعلامت صحيح tcp_keepalive_counter;
#اگر_تعریف_شده WITH_TCP_USER_TIMEOUT
	صحيح tcp_user_timeout;
#پایان_اگر
	بنية mosquitto__bridge_topic *topics;
	صحيح topic_شمار;
	bool topic_remapping;
	تعداد mosquitto__protocol protocol_نسخه;
	time_t restart_t;
	time_t connected_at;
	حرف *remote_clientid;
	حرف *remote_username;
	حرف *remote_password;
	حرف *local_clientid;
	حرف *local_username;
	حرف *local_password;
	حرف *notification_topic;
	حرف *bind_آدرس;
	bool notifications;
	bool notifications_local_only;
	تعداد mosquitto_bridge_شروع_type شروع_type;
	صحيح idle_timeout;
	صحيح restart_timeout;
	صحيح backoff_پایه;
	صحيح backoff_cap;
	صحيح stable_connection_period;
	صحيح آستانه;
	uint32_t بیشینه_packet_اندازه;
	uint32_t session_expiry_interval;
	uint16_t receive_بیشینه;
	bool lazy_reconnect;
	bool attempt_unsubscribe;
	bool initial_notification_انجامشده;
	bool outgoing_retain;
	تعداد mosquitto_bridge_reload_type reload_type;
	uint16_t max_topic_alias;
#اگر_تعریف_شده WITH_TLS
	bool tls_insecure;
	bool tls_ocsp_required;
	bool tls_use_os_certs;
	حرف *tls_cafile;
	حرف *tls_capath;
	حرف *tls_certfile;
	حرف *tls_keyfile;
	حرف *tls_نسخه;
	حرف *tls_alpn;
	حرف *tls_ciphers;
	حرف *tls_13_ciphers;
#  ifdef FINAL_WITH_TLS_PSK
	حرف *tls_psk_عنصر_خنثی;
	حرف *tls_psk;
#  endif
#پایان_اگر
};

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
بنية libws_mqtt_hack {
	حرف *http_dir;
	بنية mosquitto__listener *listener;
};

بنية libws_mqtt_داده {
	بنية mosquitto *mosq;
};
#پایان_اگر

#ضمن <net_mosq.h>


بنية control_endpoint {
	بنية control_endpoint *بعدی, *prev;
	حرف topic[];
};

خارجي بنية mosquitto_db db;

/* ============================================================
 * Main functions
 * ============================================================ */
صحيح mosquitto_main_loop(بنية mosquitto__listener_sock *listensock, صحيح listensock_شمار);
فراغ loop_update_بعدی_event(time_t new_ms);

/* ============================================================
 * Config functions
 * ============================================================ */
/* Initialise config struct to default values. */
فراغ config__init(بنية mosquitto__config *config);
#اگر_تعریف_شده WITH_BRIDGE
فراغ config__bridge_cleanup(بنية mosquitto__bridge *bridge);
#پایان_اگر
/* Parse command line options into config. */
صحيح config_parse_آرگومانها(بنية mosquitto__config *config, صحيح argc, حرف *argv[]);
/* Read configuration data from config->config_file into config.
 * If reload is true, don't process config options that shouldn't be reloaded (listeners etc)
 * Returns 0 on success, 1 if there is a configuration error or if a file cannot be opened.
 */
صحيح config__read(بنية mosquitto__config *config, bool reload);
/* Free all config data. */
فراغ config__cleanup(بنية mosquitto__config *config);
صحيح config__get_dir_files(ثابت حرف *include_dir, حرف ***files, صحيح *file_شمار);

/* ============================================================
 * Server send functions
 * ============================================================ */
صحيح send__connack(بنية mosquitto *context, uint8_t ack, uint8_t reason_کد, ثابت mosquitto_property *properties);
صحيح send__suback(بنية mosquitto *context, uint16_t mid, uint32_t payloadlen, ثابت فراغ *payload);
صحيح send__unsuback(بنية mosquitto *context, uint16_t mid, صحيح reason_کد_شمار, uint8_t *reason_codes, ثابت mosquitto_property *properties);
صحيح send__auth(بنية mosquitto *context, uint8_t reason_کد, ثابت فراغ *auth_داده, uint16_t auth_داده_len);

/* ============================================================
 * Network functions
 * ============================================================ */
فراغ net__broker_init(فراغ);
فراغ net__broker_cleanup(فراغ);
بنية mosquitto *net__socket_accept(بنية mosquitto__listener_sock *listensock);
صحيح net__socket_listen(بنية mosquitto__listener *listener);
صحيح net_socket_get_آدرس(mosq_sock_t sock, حرف *buf, size_t len, uint16_t *remote_آدرس);
صحيح net_tls_بارگیری_verify(بنية mosquitto__listener *listener);
صحيح net__tls_server_ctx(بنية mosquitto__listener *listener);
صحيح net_بارگیری_certificates(بنية mosquitto__listener *listener);

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
#اگر_تعریف_شده WITH_PERSISTENCE
صحيح persist__backup(bool shutdown);
صحيح persist__restore(فراغ);
#پایان_اگر
/* Return the number of in-flight messages in count. */
صحيح db_پیام_شمار(صحيح *شمار);
صحيح db_پیام_delete_outgoing(بنية mosquitto *context, uint16_t mid, تعداد mosquitto_پیام_state expect_state, صحيح qos);
صحيح db_پیام_درج_outgoing(بنية mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, بنية mosquitto_پایه_پیام *پایه_پیام, uint32_t subscription_identifier, bool update, bool persist);
صحيح db_پیام_درج_incoming(بنية mosquitto *context, uint64_t cmsg_id, بنية mosquitto_پایه_پیام *پایه_پیام, bool persist);
صحيح db_پیام_remove_incoming(بنية mosquitto *context, uint16_t mid);
صحيح db_پیام_release_incoming(بنية mosquitto *context, uint16_t mid);
صحيح db_پیام_update_outgoing(بنية mosquitto *context, uint16_t mid, تعداد mosquitto_پیام_state state, صحيح qos, bool persist);
فراغ db_پیام_خارج_از_صف_اولی(بنية mosquitto *context, بنية mosquitto_پیام_داده *پیام_داده);
صحيح db__messages_delete(بنية mosquitto *context, bool force_free);
صحيح db__messages_delete_incoming(بنية mosquitto *context);
صحيح db__messages_delete_outgoing(بنية mosquitto *context);
صحيح db_messages_easy_صف(بنية mosquitto *context, ثابت حرف *topic, uint8_t qos, uint32_t payloadlen, ثابت فراغ *payload, صحيح retain, uint32_t پیام_expiry_interval, mosquitto_property **properties);
صحيح db_پیام_store(ثابت بنية mosquitto *منبع, بنية mosquitto_پایه_پیام *پایه_پیام, uint32_t *پیام_expiry_interval, تعداد mosquitto_پیام_origin origin);
صحيح db_پیام_store_پیدا_کن(بنية mosquitto *context, uint16_t mid, بنية mosquitto_client_پیام **client_پیام);
صحيح db_پیام_store_جمع_کن(بنية mosquitto_پایه_پیام *پایه_پیام);
فراغ db_پیام_store_remove(بنية mosquitto_پایه_پیام *پایه_پیام, bool notify);
فراغ db_پیام_store_ref_inc(بنية mosquitto_پایه_پیام *پایه_پیام);
فراغ db_پیام_store_ref_dec(بنية mosquitto_پایه_پیام **پایه_پیام);
فراغ db_پیام_store_clean(فراغ);
فراغ db_پیام_store_compact(فراغ);
فراغ db_پیام_store_free(بنية mosquitto_پایه_پیام *پایه_پیام);
صحيح db_پیام_reconnect_reset(بنية mosquitto *context);
bool db__ready_for_flight(بنية mosquitto *context, تعداد mosquitto_پیام_direction dir, صحيح qos);
bool db_ready_for_صف(بنية mosquitto *context, صحيح qos, بنية mosquitto_پیام_داده *پیام_داده);
فراغ sys_درخت_init(فراغ);
فراغ sys_درخت_update(bool force);
صحيح db_پیام_write_inflight_بیرون_all(بنية mosquitto *context);
صحيح db_پیام_write_inflight_بیرون_latest(بنية mosquitto *context);
صحيح db_پیام_write_queued_بیرون(بنية mosquitto *context);
صحيح db_پیام_write_queued_درون(بنية mosquitto *context);
فراغ db_پیام_جمع_کن_مقصد_inflight_stats(بنية mosquitto_پیام_داده *پیام_داده, بنية mosquitto_client_پیام *پیام);
فراغ db_پیام_جمع_کن_مقصد_queued_stats(بنية mosquitto_پیام_داده *پیام_داده, بنية mosquitto_client_پیام *پیام);
uint64_t db_new_پیام_id(فراغ);
فراغ db__expire_all_messages(بنية mosquitto *context);
فراغ db_check_acl_از_all_messages(بنية mosquitto *context);

/* ============================================================
 * Subscription functions
 * ============================================================ */
صحيح sub__init(فراغ);
صحيح sub_جمع_کن(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub);
صحيح sub__remove(بنية mosquitto *context, ثابت حرف *sub, uint8_t *reason);
فراغ sub_درخت_print(بنية mosquitto__subhier *ریشه, صحيح level);
صحيح sub__clean_session(بنية mosquitto *context);
صحيح sub_messages_صف(ثابت حرف *منبع_id, uint8_t qos, بنية mosquitto_پایه_پیام **پایه_پیام);
صحيح sub__topic_tokenise(ثابت حرف *subtopic, حرف **local_sub, حرف ***topics, ثابت حرف **sharename);
فراغ sub_topic_نشانهها_free(بنية sub__token *نشانهها);

/* ============================================================
 * Context functions
 * ============================================================ */
بنية mosquitto *context__init(فراغ);
صحيح context__init_sock(بنية mosquitto *context, mosq_sock_t sock, bool get_آدرس);
فراغ context__cleanup(بنية mosquitto *context, bool force_free);
فراغ context__disconnect(بنية mosquitto *context, صحيح reason);
فراغ context_جمع_کن_مقصد_disused(بنية mosquitto *context);
فراغ context__free_disused(فراغ);
فراغ context__send_will(بنية mosquitto *context);
فراغ context_جمع_کن_مقصد_by_id(بنية mosquitto *context);
فراغ context__remove_from_by_id(بنية mosquitto *context);

صحيح connect__on_authorised(بنية mosquitto *context, فراغ *auth_داده_بیرون, uint16_t auth_داده_بیرون_len);


/* ============================================================
 * Control functions
 * ============================================================ */
#اگر_تعریف_شده WITH_CONTROL
صحيح control__process(بنية mosquitto *context, بنية mosquitto_پایه_پیام *پایه_پیام);
فراغ control__cleanup(فراغ);
#پایان_اگر
صحيح control_ثبات_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ثابت حرف *topic, فراغ *userdata);
صحيح control__unregister_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ثابت حرف *topic);
فراغ control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier);


/* ============================================================
 * Logging functions
 * ============================================================ */
صحيح log__init(بنية mosquitto__config *config);
صحيح log__close(بنية mosquitto__config *config);
فراغ log_داخلی(ثابت حرف *fmt, ...) __attribute__((format(چاپ_کن, 1, 2)));

/* ============================================================
 * Bridge functions
 * ============================================================ */
#اگر_تعریف_شده WITH_BRIDGE
فراغ bridge_شروع_all(فراغ);
فراغ bridge__reload(فراغ);
فراغ bridge__db_cleanup(فراغ);
فراغ bridge__cleanup(بنية mosquitto *context);
صحيح bridge__connect(بنية mosquitto *context);
#إذا defined(__GLIBC__) && defined(WITH_ADNS)
صحيح bridge__connect_step3(بنية mosquitto *context);
#پایان_اگر
صحيح bridge__on_connect(بنية mosquitto *context);
فراغ bridge_check(فراغ);
صحيح bridge_ثبات_local_connections(فراغ);
صحيح bridge_جمع_کن_topic(بنية mosquitto__bridge *bridge, ثابت حرف *topic, تعداد mosquitto__bridge_direction direction, uint8_t qos, ثابت حرف *local_پیشوند, ثابت حرف *remote_پیشوند);
فراغ bridge__cleanup_topics(بنية mosquitto__bridge *bridge);
صحيح bridge_remap_topic_درون(بنية mosquitto *context, حرف **topic);
#پایان_اگر

/* ============================================================
 * IO multiplex related functions
 * ============================================================ */
صحيح mux__init(فراغ);
صحيح mux_جمع_کن_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_شمار);
صحيح mux__delete_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_شمار);
صحيح mux__loop_prepare(فراغ);
صحيح mux__new(بنية mosquitto *context);
صحيح mux_جمع_کن_بیرون(بنية mosquitto *context);
صحيح mux_remove_بیرون(بنية mosquitto *context);
صحيح mux__delete(بنية mosquitto *context);
صحيح mux__wait(فراغ);
صحيح mux__handle(بنية mosquitto__listener_sock *listensock, صحيح listensock_شمار);
صحيح mux__cleanup(فراغ);

/* ============================================================
 * Listener related functions
 * ============================================================ */
خارجي بنية mosquitto__listener_sock *g_listensock;
خارجي صحيح g_listensock_شمار;

فراغ listener_مجموعه_defaults(بنية mosquitto__listener *listener);
فراغ listeners__reload_all_certificates(فراغ);
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
فراغ listeners_جمع_کن_websockets(بنية lws_context *ws_context, mosq_sock_t fd);
#پایان_اگر
صحيح listeners_شروع(فراغ);
فراغ listeners__stop(فراغ);

/* ============================================================
 * Plugin related functions
 * ============================================================ */
صحيح plugin_بارگیری_v5(mosquitto_plugin_id_t *plugin, فراغ *lib);
صحيح plugin_بارگیری_v4(mosquitto_plugin_id_t *plugin, فراغ *lib);
صحيح plugin_بارگیری_v3(mosquitto_plugin_id_t *plugin, فراغ *lib);
صحيح plugin_بارگیری_v2(mosquitto_plugin_id_t *plugin, فراغ *lib);
صحيح acl__pre_check(mosquitto_plugin_id_t *plugin, بنية mosquitto *context, صحيح access);

فراغ LIB_ERROR(فراغ);
فراغ plugin__handle_connect(بنية mosquitto *context);
فراغ plugin__handle_disconnect(بنية mosquitto *context, صحيح reason);
فراغ plugin__handle_client_offline(بنية mosquitto *context, صحيح reason);
صحيح plugin_handle_پیام_درون(بنية mosquitto *context, بنية mosquitto_پایه_پیام_2 *پایه_پیام);
صحيح plugin_handle_پیام_بیرون(بنية mosquitto *context, بنية mosquitto_پایه_پیام_2 *پایه_پیام);
صحيح plugin__handle_subscribe(بنية mosquitto *context, بنية mosquitto_subscription *sub);
صحيح plugin__handle_unsubscribe(بنية mosquitto *context, بنية mosquitto_subscription *sub);
صحيح plugin__handle_reload(فراغ);
فراغ plugin__handle_tick(فراغ);
صحيح plugin__callback_unregister_all(mosquitto_plugin_id_t *identifier);
فراغ plugin_persist__handle_restore(فراغ);
فراغ plugin_persist_handle_client_جمع_کن(بنية mosquitto *context);
فراغ plugin_persist__handle_client_delete(بنية mosquitto *context);
فراغ plugin_persist__handle_client_update(بنية mosquitto *context);
فراغ plugin_persist_handle_subscription_جمع_کن(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub);
فراغ plugin_persist__handle_subscription_delete(بنية mosquitto *context, حرف *sub);
فراغ plugin_persist_handle_client_پیام_جمع_کن(بنية mosquitto *context, ثابت بنية mosquitto_client_پیام *cmsg);
فراغ plugin_persist_handle_client_پیام_delete(بنية mosquitto *context, ثابت بنية mosquitto_client_پیام *cmsg);
فراغ plugin_persist_handle_client_پیام_update(بنية mosquitto *context, ثابت بنية mosquitto_client_پیام *cmsg);
فراغ plugin_persist_handle_پایه_پیام_جمع_کن(بنية mosquitto_پایه_پیام *پایه_پیام);
فراغ plugin_persist_handle_پایه_پیام_delete(بنية mosquitto_پایه_پیام *پایه_پیام);
فراغ plugin_persist_handle_retain_پیام_مجموعه(بنية mosquitto_پایه_پیام *پایه_پیام);
فراغ plugin_persist_handle_retain_پیام_delete(بنية mosquitto_پایه_پیام *پایه_پیام);
فراغ plugin_persist_handle_will_جمع_کن(بنية mosquitto *context);
فراغ plugin_persist__handle_will_delete(بنية mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
صحيح keepalive__init(فراغ);
فراغ keepalive__cleanup(فراغ);
صحيح keepalive_جمع_کن(بنية mosquitto *context);
فراغ keepalive__check(فراغ);
صحيح keepalive__remove(بنية mosquitto *context);
صحيح keepalive__update(بنية mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
صحيح property__process_connect(بنية mosquitto *context, mosquitto_property **props);
صحيح property__process_will(بنية mosquitto *context, بنية mosquitto_پیام_all *پیام, mosquitto_property **props);
صحيح property__process_publish(بنية mosquitto_پایه_پیام *پایه_پیام, mosquitto_property **props, صحيح *topic_alias, uint32_t *پیام_expiry_interval, bool is_bridge);
صحيح property__process_disconnect(بنية mosquitto *context, mosquitto_property **props);

/* ============================================================
 * Retain tree related functions
 * ============================================================ */
صحيح retain__init(فراغ);
فراغ retain__clean(بنية mosquitto__retainhier **retainhier);
صحيح retain_صف(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub);
صحيح retain__store(ثابت حرف *topic, بنية mosquitto_پایه_پیام *پایه_پیام, حرف **تقسیم_topics, bool persist);
فراغ retain__expiry_check(فراغ);
فراغ retain__expire(بنية mosquitto__retainhier **retainhier);

/* ============================================================
 * Security related functions
 * ============================================================ */
صحيح acl_پیدا_کن_acls(بنية mosquitto *context);
صحيح plugin_بارگیری_all(فراغ);
صحيح plugin__unload_all(فراغ);
صحيح config_plugin_جمع_کن_secopt(mosquitto_plugin_id_t *plugin, بنية mosquitto__security_options *security_options);

صحيح mosquitto_security_init(bool reload);
صحيح mosquitto_security_cleanup(bool reload);
صحيح mosquitto_acl_check(بنية mosquitto *context, ثابت حرف *topic, uint32_t payloadlen, فراغ *payload, uint8_t qos, bool retain, mosquitto_property *properties, صحيح access);
صحيح mosquitto_basic_auth(بنية mosquitto *context);
صحيح mosquitto_psk_کلید_get(بنية mosquitto *context, ثابت حرف *hint, ثابت حرف *عنصر_خنثی, حرف *کلید, صحيح max_کلید_len);

صحيح mosquitto_security_init_default(فراغ);
صحيح mosquitto_security_apply_default(فراغ);
صحيح mosquitto_security_cleanup_default(فراغ);
صحيح mosquitto_psk_کلید_get_default(بنية mosquitto *context, ثابت حرف *hint, ثابت حرف *عنصر_خنثی, حرف *کلید, صحيح max_کلید_len);
صحيح broker_acl_file__init(فراغ);
فراغ broker_acl_file__cleanup(فراغ);
صحيح broker_password_file__init(فراغ);
فراغ broker_password_file__cleanup(فراغ);
صحيح psk_file__init(فراغ);
صحيح psk_file__cleanup(فراغ);

صحيح mosquitto_security_auth_شروع(بنية mosquitto *context, bool reauth, ثابت فراغ *داده_درون, uint16_t داده_درون_len, فراغ **داده_بیرون, uint16_t *داده_بیرون_len);
صحيح mosquitto_security_auth_continue(بنية mosquitto *context, ثابت فراغ *داده_درون, uint16_t داده_len, فراغ **داده_بیرون, uint16_t *داده_بیرون_len);

فراغ unpwd_free_قلم(بنية mosquitto__unpwd **unpwd, بنية mosquitto__unpwd *قلم);

/* ============================================================
 * Session expiry
 * ============================================================ */
صحيح session_expiry_جمع_کن(بنية mosquitto *context);
صحيح session_expiry_جمع_کن_from_persistence(بنية mosquitto *context, time_t expiry_زمان);
فراغ session_expiry_جمع_کن_on_shutdown(فراغ);
فراغ session_expiry__remove(بنية mosquitto *context);
فراغ session_expiry__remove_all(فراغ);
فراغ session_expiry__check(فراغ);
فراغ session_expiry__send_all(فراغ);

/* ============================================================
 * Signals
 * ============================================================ */
فراغ signal_راهاندازی(فراغ);
صحيح signal__flag_check(فراغ);

/* ============================================================
 * Window service and signal related functions
 * ============================================================ */
#إذا defined(WIN32) || defined(__CYGWIN__)
فراغ service_install(حرف *name);
فراغ service_uninstall(حرف *name);
فراغ service_پیاپی(حرف *name);

#اگر_تعریف_شده WIN32
DWORD WINAPI SigThreadProc(فراغ *داده);
#پایان_اگر
#پایان_اگر

/* ============================================================
 * Watchdog
 * ============================================================ */
فراغ watchdog__init(فراغ);
فراغ watchdog__check(فراغ);

/* ============================================================
 * Websockets related functions
 * ============================================================ */
#اگر_تعریف_شده WITH_WEBSOCKETS
#  إذا WITH_WEBSOCKETS == WS_IS_LWS
فراغ mosq_websockets_init(بنية mosquitto__listener *listener, ثابت بنية mosquitto__config *conf);
#  endif
صحيح http__context_init(بنية mosquitto *context);
صحيح http__context_cleanup(بنية mosquitto *context);
صحيح http__read(بنية mosquitto *context);
#پایان_اگر
فراغ do_disconnect(بنية mosquitto *context, صحيح reason);

/* ============================================================
 * PROXY related functions
 * ============================================================ */
صحيح proxy_v2__read(بنية mosquitto *context);
صحيح proxy_v1__read(بنية mosquitto *context);

/* ============================================================
 * Will delay
 * ============================================================ */
صحيح will_delay_جمع_کن(بنية mosquitto *context);
فراغ will_delay__check(فراغ);
فراغ will_delay__send_all(فراغ);
فراغ will_delay__remove(بنية mosquitto *mosq);

/* ============================================================
 * HTTP Info
 * ============================================================ */
صحيح http_api_شروع_local(بنية mosquitto__listener *listener);
صحيح http_api_شروع(بنية mosquitto__listener *listener);
فراغ http_api__stop(بنية mosquitto__listener *listener);


/* ============================================================
 * Other
 * ============================================================ */
#اگر_تعریف_شده WITH_XTREPORT
فراغ xtreport(فراغ);
#پایان_اگر

فراغ broker_control__init(فراغ);
فراغ broker_control__cleanup(فراغ);
فراغ broker_control__reload(فراغ);

#پایان_اگر
