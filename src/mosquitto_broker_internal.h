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

#که_نه_تعریف_شوی MOSQUITTO_BROKER_دننی_H
#تعریف_کړه MOSQUITTO_BROKER_دننی_H

#ضمن "config.h"
#ضمن <stdio.h>

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#  إذا LWS_LIBRARY_نسخه_شمېره >= 3002000 && !defined(LWS_WITH_EXTERNAL_POLL)
#    warning "libwebsockets is not compiled with LWS_WITH_EXTERNAL_POLL support. Websocket performance will be unusable."
#  endif
#د_که_پای

#که_تعریف_شوی WITH_HTTP_API
#  include <microhttpd.h>
#د_که_پای

#که_تعریف_شوی __linux__
#تعریف_کړه WITH_TCP_USER_TIMEOUT
#د_که_پای

#ضمن "mosquitto_internal.h"
#ضمن "mosquitto/broker.h"
#ضمن "mosquitto/broker_plugin.h"
#ضمن "mosquitto.h"
#ضمن "logging_mosq.h"
#ضمن "tls_mosq.h"
#ضمن "uthash.h"
#ضمن "acl_file.h"
#ضمن "password_file.h"

#که_نه_تعریف_شوی __GNUC__
#تعریف_کړه __attribute__(attrib)
#د_که_پای

/* Log destinations */
#تعریف_کړه MQTT3_LOG_NONE 0x00
#تعریف_کړه MQTT3_LOG_SYSLOG 0x01
#تعریف_کړه MQTT3_LOG_FILE 0x02
#تعریف_کړه MQTT3_LOG_STDOUT 0x04
#تعریف_کړه MQTT3_LOG_STDERR 0x08
#تعریف_کړه MQTT3_LOG_TOPIC 0x10
#تعریف_کړه MQTT3_LOG_DLT 0x20
#که_تعریف_شوی ANDROID
#تعریف_کړه MQTT3_LOG_ANDROID 0x40
#د_که_پای
#تعریف_کړه MQTT3_LOG_ALL 0xFF

#تعریف_کړه CMD_پورټ_حد 10

تعريفنوع uint64_t dbid_t;

تعريفنوع صحيح (*FUNC_plugin_init_v5)(mosquitto_plugin_id_t *, فراغ **, بنية mosquitto_opt *, صحيح);
تعريفنوع صحيح (*FUNC_plugin_cleanup_v5)(فراغ *, بنية mosquitto_opt *, صحيح);

تعريفنوع صحيح (*FUNC_auth_plugin_init_v4)(فراغ **, بنية mosquitto_opt *, صحيح);
تعريفنوع صحيح (*FUNC_auth_plugin_cleanup_v4)(فراغ *, بنية mosquitto_opt *, صحيح);
تعريفنوع صحيح (*FUNC_auth_plugin_security_init_v4)(فراغ *, بنية mosquitto_opt *, صحيح, bool);
تعريفنوع صحيح (*FUNC_auth_plugin_security_cleanup_v4)(فراغ *, بنية mosquitto_opt *, صحيح, bool);
تعريفنوع صحيح (*FUNC_auth_plugin_acl_check_v4)(فراغ *, صحيح, بنية mosquitto *, بنية mosquitto_acl_پیغام *);
تعريفنوع صحيح (*FUNC_auth_plugin_unpwd_check_v4)(فراغ *, بنية mosquitto *, ثابت حرف *, ثابت حرف *);
تعريفنوع صحيح (*FUNC_auth_plugin_psk_کلی_get_v4)(فراغ *, بنية mosquitto *, ثابت حرف *, ثابت حرف *, حرف *, صحيح);
تعريفنوع صحيح (*FUNC_auth_plugin_auth_پیل_v4)(فراغ *, بنية mosquitto *, ثابت حرف *, bool, ثابت فراغ *, uint16_t, فراغ **, uint16_t *);
تعريفنوع صحيح (*FUNC_auth_plugin_auth_continue_v4)(فراغ *, بنية mosquitto *, ثابت حرف *, ثابت فراغ *, uint16_t, فراغ **, uint16_t *);

تعريفنوع صحيح (*FUNC_auth_plugin_init_v3)(فراغ **, بنية mosquitto_opt *, صحيح);
تعريفنوع صحيح (*FUNC_auth_plugin_cleanup_v3)(فراغ *, بنية mosquitto_opt *, صحيح);
تعريفنوع صحيح (*FUNC_auth_plugin_security_init_v3)(فراغ *, بنية mosquitto_opt *, صحيح, bool);
تعريفنوع صحيح (*FUNC_auth_plugin_security_cleanup_v3)(فراغ *, بنية mosquitto_opt *, صحيح, bool);
تعريفنوع صحيح (*FUNC_auth_plugin_acl_check_v3)(فراغ *, صحيح, ثابت بنية mosquitto *, بنية mosquitto_acl_پیغام *);
تعريفنوع صحيح (*FUNC_auth_plugin_unpwd_check_v3)(فراغ *, ثابت بنية mosquitto *, ثابت حرف *, ثابت حرف *);
تعريفنوع صحيح (*FUNC_auth_plugin_psk_کلی_get_v3)(فراغ *, ثابت بنية mosquitto *, ثابت حرف *, ثابت حرف *, حرف *, صحيح);

تعريفنوع صحيح (*FUNC_auth_plugin_init_v2)(فراغ **, بنية mosquitto_auth_opt *, صحيح);
تعريفنوع صحيح (*FUNC_auth_plugin_cleanup_v2)(فراغ *, بنية mosquitto_auth_opt *, صحيح);
تعريفنوع صحيح (*FUNC_auth_plugin_security_init_v2)(فراغ *, بنية mosquitto_auth_opt *, صحيح, bool);
تعريفنوع صحيح (*FUNC_auth_plugin_security_cleanup_v2)(فراغ *, بنية mosquitto_auth_opt *, صحيح, bool);
تعريفنوع صحيح (*FUNC_auth_plugin_acl_check_v2)(فراغ *, ثابت حرف *, ثابت حرف *, ثابت حرف *, صحيح);
تعريفنوع صحيح (*FUNC_auth_plugin_unpwd_check_v2)(فراغ *, ثابت حرف *, ثابت حرف *);
تعريفنوع صحيح (*FUNC_auth_plugin_psk_کلی_get_v2)(فراغ *, ثابت حرف *, ثابت حرف *, حرف *, صحيح);


تعداد mosquitto_پیغام_origin {
	mosq_mo_client = 0,
	mosq_mo_broker = 1,
};

بنية mosquitto__plugin_lib {
	فراغ *lib;
	فراغ *user_معلومات;
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
	FUNC_auth_plugin_psk_کلی_get_v4 psk_کلی_get_v4;
	FUNC_auth_plugin_auth_پیل_v4 auth_پیل_v4;
	FUNC_auth_plugin_auth_continue_v4 auth_continue_v4;

	FUNC_auth_plugin_init_v3 plugin_init_v3;
	FUNC_auth_plugin_cleanup_v3 plugin_cleanup_v3;
	FUNC_auth_plugin_security_init_v3 security_init_v3;
	FUNC_auth_plugin_security_cleanup_v3 security_cleanup_v3;
	FUNC_auth_plugin_acl_check_v3 acl_check_v3;
	FUNC_auth_plugin_unpwd_check_v3 unpwd_check_v3;
	FUNC_auth_plugin_psk_کلی_get_v3 psk_کلی_get_v3;

	FUNC_auth_plugin_init_v2 plugin_init_v2;
	FUNC_auth_plugin_cleanup_v2 plugin_cleanup_v2;
	FUNC_auth_plugin_security_init_v2 security_init_v2;
	FUNC_auth_plugin_security_cleanup_v2 security_cleanup_v2;
	FUNC_auth_plugin_acl_check_v2 acl_check_v2;
	FUNC_auth_plugin_unpwd_check_v2 unpwd_check_v2;
	FUNC_auth_plugin_psk_کلی_get_v2 psk_کلی_get_v2;
	صحيح نسخه_2;
};

بنية mosquitto__plugin_config {
	حرف *لار;
	حرف *name;
	بنية mosquitto_opt *options;
	بنية mosquitto__security_options **security_options;
	صحيح option_شمېر;
	صحيح security_option_شمېر;
	bool deny_special_chars;
};

بنية mosquitto__callback {
	UT_hash_handle hh; /* For callbacks that register for e.g. a specific topic */
	بنية mosquitto__callback *بل, *prev; /* For typical callbacks */
	MOSQ_FUNC_generic_callback cb;
	فراغ *userdata;
	اتحاد {
		حرف *topic;
		بنية timespec بل_tick;
	} معلومات;
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
	بنية mosquitto__callback *ext_auth_پیل;
	بنية mosquitto__callback *پیغام_دننه;
	بنية mosquitto__callback *پیغام_بهر;
	بنية mosquitto__callback *psk_کلی;
	بنية mosquitto__callback *reload;
	بنية mosquitto__callback *subscribe;
	بنية mosquitto__callback *unsubscribe;
	بنية mosquitto__callback *persist_restore;
	بنية mosquitto__callback *persist_client_جمع_کړه;
	بنية mosquitto__callback *persist_client_delete;
	بنية mosquitto__callback *persist_client_update;
	بنية mosquitto__callback *persist_subscription_جمع_کړه;
	بنية mosquitto__callback *persist_subscription_delete;
	بنية mosquitto__callback *persist_client_پیغام_جمع_کړه;
	بنية mosquitto__callback *persist_client_پیغام_delete;
	بنية mosquitto__callback *persist_client_پیغام_update;
	بنية mosquitto__callback *persist_بنسټ_پیغام_جمع_کړه;
	بنية mosquitto__callback *persist_بنسټ_پیغام_delete;
	بنية mosquitto__callback *persist_retain_پیغام_سېټ;
	بنية mosquitto__callback *persist_retain_پیغام_delete;
	بنية mosquitto__callback *persist_will_جمع_کړه;
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
	بنية acl_file_معلومات acl_معلومات;
	بنية password_file_معلومات password_معلومات;
	حرف *psk_file;
	mosquitto_plugin_id_t **plugins;
	صحيح plugin_شمېر;
	int8_t allow_anonymous;
	bool allow_zero_اوږدتیا_clientid;
	حرف *auto_id_مختاړی;
	uint16_t auto_id_مختاړی_len;
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
#د_که_پای

بنية mosquitto__listener {
	uint16_t پورټ;
	حرف *host;
	حرف *bind_interface;
	صحيح max_connections;
	حرف *mount_ټکی;
	mosq_sock_t *socks;
	صحيح sock_شمېر;
	صحيح client_شمېر;
	تعداد mosquitto_protocol protocol;
	صحيح socket_domain;
	bool use_username_as_clientid;
	uint8_t max_qos;
	uint16_t max_topic_alias;
	uint16_t max_topic_alias_broker;
#که_تعریف_شوی WITH_TLS
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
	bool use_بې_اغېزه_عنصر_as_username;
	bool use_subject_as_username;
	bool require_certificate;
	bool disable_client_cert_date_checks;
	تعداد mosquitto__keyform tls_keyform;
#د_که_پای
#که_تعریف_شوی WITH_WEBSOCKETS
#  إذا WITH_WEBSOCKETS == WS_IS_LWS
	بنية lws_context *ws_context;
	bool ws_دننه_init;
	بنية lws_protocols *ws_protocol;
#  endif
	حرف **ws_origins;
	صحيح ws_origin_شمېر;
#د_که_پای
#إذا defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
	حرف *http_dir;
#د_که_پای
	بنية mosquitto__security_options *security_options;
#که_تعریف_شوی WITH_UNIX_SOCKETS
	حرف *unix_socket_لار;
#د_که_پای
	bool disable_protocol_v3;
	bool disable_protocol_v4;
	bool disable_protocol_v5;
	صحيح enable_proxy_protocol;
	bool proxy_protocol_v2_require_tls;
#که_تعریف_شوی WITH_HTTP_API
	بنية MHD_Daemon *mhd;
#د_که_پای
};


بنية mosquitto__listener_sock {
#إذا defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	/* This *must* be the first element in the struct. */
	صحيح ident;
#د_که_پای
	mosq_sock_t sock;
	بنية mosquitto__listener *listener;
};


/* Callbacks belonging to a specific plugin
 * Doesn't include MOSQ_EVT_CONTROL events.
 */
بنية plugin_own_callback {
	بنية plugin_own_callback *بل, *prev;
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
	بنية timespec بل_tick;
};

بنية mosquitto__config {
	bool allow_duplicate_messages;
	صحيح autosave_interval;
	bool autosave_on_changes;
	bool check_retain_سرچینه;
	حرف *clientid_prefixes;
	bool connection_messages;
	uint16_t cmd_پورټ[CMD_پورټ_حد];
	صحيح cmd_پورټ_شمېر;
	bool daemon;
	bool test_configuration;
	bool enable_control_api;
	صحيح global_max_clients;
	صحيح global_max_connections;
	بنية mosquitto__listener *default_listener; /* Points to one of `listeners` */
	بنية mosquitto__listener *listeners;
	صحيح listener_شمېر;
	bool local_only;
	بینښه صحيح log_dest;
	صحيح log_facility;
	بینښه صحيح log_type;
	bool log_timestamp;
	حرف *log_timestamp_format;
	حرف *log_file;
	FILE *log_fptr;
	size_t max_inflight_بایټونه;
	size_t max_queued_بایټونه;
	صحيح max_queued_messages;
	uint32_t max_packet_اندازه;
	uint32_t پیغام_اندازه_حد;
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
	bool کتار_qos0_messages;
	bool فی_listener_settings;
	bool retain_شته;
	صحيح retain_expiry_interval;
	bool سېټ_tcp_nodelay;
	صحيح sys_interval;
	bool upgrade_outgoing_qos;
	حرف *user;
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	صحيح websockets_log_level;
#د_که_پای
	uint16_t packet_بفر_اندازه;
#که_تعریف_شوی WITH_BRIDGE
	بنية mosquitto__bridge **bridges;
	صحيح bridge_شمېر;
#د_که_پای
	بنية mosquitto__security_options security_options;
};


بنية mosquitto__subshared {
	UT_hash_handle hh;
	بنية mosquitto__subleaf *subs;
	حرف name[];
};

بنية mosquitto__subhier {
	UT_hash_handle hh;
	بنية mosquitto__subhier *پلار;
	بنية mosquitto__subhier *children;
	بنية mosquitto__subleaf *subs;
	بنية mosquitto__subshared *shared;
	uint16_t topic_len;
	حرف topic[];
};

بنية mosquitto__subleaf {
	بنية mosquitto__subleaf *prev;
	بنية mosquitto__subleaf *بل;
	بنية mosquitto *context;
	بنية mosquitto__subhier *hier;
	بنية mosquitto__subshared *shared;
	uint32_t identifier;
	uint8_t subscription_options;
	حرف topic_filter[];
};

بنية sub__token {
	بنية sub__token *بل;
	uint16_t topic_len;
	حرف topic[];
};

بنية mosquitto__retainhier {
	UT_hash_handle hh;
	بنية mosquitto__retainhier *پلار;
	بنية mosquitto__retainhier *children;
	بنية mosquitto_بنسټ_پیغام *retained;
	uint16_t topic_len;
	حرف topic[];
};

بنية mosquitto_بنسټ_پیغام {
	UT_hash_handle hh;
	بنية mosquitto_بنسټ_پیغام_2 معلومات;
	بنية mosquitto__listener *سرچینه_listener;
	حرف **dest_ids;
	صحيح dest_id_شمېر;
	صحيح ref_شمېر;
	تعداد mosquitto_پیغام_origin origin;
	bool stored;
};

بنية mosquitto_client_پیغام {
	بنية mosquitto_client_پیغام_2 معلومات;
	بنية mosquitto_client_پیغام *prev;
	بنية mosquitto_client_پیغام *بل;
	بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام;
};


بنية mosquitto__psk {
	UT_hash_handle hh;
	حرف *username;
	حرف *password;
};

بنية mosquitto_پیغام_v5 {
	بنية mosquitto_پیغام_v5 *بل, *prev;
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
	dbid_t وروستی_db_id;
	uint64_t غوټه_id_shifted;
	بنية mosquitto__subhier *نورمال_subs;
	بنية mosquitto__subhier *shared_subs;
	بنية mosquitto__retainhier *retains;
	بنية mosquitto *contexts_by_id;
	بنية mosquitto *contexts_by_sock;
	بنية mosquitto *contexts_by_id_delayed_auth;
	بنية mosquitto *contexts_for_free;
	mosquitto_plugin_id_t **plugins;
	صحيح plugin_شمېر;
#که_تعریف_شوی WITH_BRIDGE
	بنية mosquitto **bridges;
	صحيح bridge_شمېر;
#د_که_پای
	بنية clientid_شاخص_hash_2 *clientid_شاخص_hash;
	بنية mosquitto_بنسټ_پیغام *پیغام_store;
	time_t now_s; /* Monotonic clock, where possible */
	time_t now_real_s; /* Read clock, for measuring session/message expiry */
	uint64_t غوټه_id; /* for unique db ids */
	time_t بل_event_ms; /* for mux timeout */
	صحيح پیغام_store_شمېر;
	بینښه اوږد پیغام_store_بایټونه;
	حرف *config_file;
	بنية mosquitto__config *config;
	bool quiet;
	bool verbose;
#که_تعریف_شوی WITH_SYS_ونه
	صحيح subscription_شمېر;
	صحيح shared_subscription_شمېر;
	صحيح retained_شمېر;
#د_که_پای
	صحيح persistence_changes;
	بنية mosquitto *ll_for_free;
#که_تعریف_شوی WITH_EPOLL
	صحيح epollfd;
#د_که_پای
#که_تعریف_شوی WITH_KQUEUE
	صحيح kqueuefd;
#د_که_پای
	بنية mosquitto_پیغام_v5 *plugin_msgs;
#که_تعریف_شوی WITH_TLS
	/* tls_keylog can't be in the config struct because it is used
	   before the config is allocated. Config probably
	   shouldn't be separately allocated. */
	حرف *tls_keylog;
#د_که_پای
	bool shutdown;
};

تعداد mosquitto__bridge_direction {
	bd_بهر = 0,
	bd_دننه = 1,
	bd_both = 2,
};

تعداد mosquitto_bridge_پیل_type {
	bst_automatic = 0,
	bst_lazy = 1,
	bst_manual = 2,
	bst_once = 3,
};

تعداد mosquitto_bridge_reload_type {
	brt_lazy = 0,
	brt_سمدستي = 1,
};

بنية mosquitto__bridge_topic {
	بنية mosquitto__bridge_topic *بل;
	حرف *topic;
	حرف *local_مختاړی;
	حرف *remote_مختاړی;
	حرف *local_topic; /* topic prefixed with local_prefix */
	حرف *remote_topic; /* topic prefixed with remote_prefix */
	تعداد mosquitto__bridge_direction direction;
	uint8_t qos;
};

بنية bridge_پته {
	حرف *پته;
	uint16_t پورټ;
};

بنية mosquitto__bridge {
	حرف *name;
	بنية bridge_پته *پتې;
	صحيح cur_پته;
	صحيح پته_شمېر;
	time_t primary_retry;
	mosq_sock_t primary_retry_sock;
	bool پړاو_robin;
	bool try_شخصي;
	bool try_شخصي_accepted;
	bool clean_پیل;
	int8_t clean_پیل_local;
	uint16_t keepalive;
	بینښه صحيح tcp_keepalive_idle;
	بینښه صحيح tcp_keepalive_interval;
	بینښه صحيح tcp_keepalive_counter;
#که_تعریف_شوی WITH_TCP_USER_TIMEOUT
	صحيح tcp_user_timeout;
#د_که_پای
	بنية mosquitto__bridge_topic *topics;
	صحيح topic_شمېر;
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
	حرف *bind_پته;
	bool notifications;
	bool notifications_local_only;
	تعداد mosquitto_bridge_پیل_type پیل_type;
	صحيح idle_timeout;
	صحيح restart_timeout;
	صحيح backoff_بنسټ;
	صحيح backoff_cap;
	صحيح stable_connection_period;
	صحيح پوله;
	uint32_t اعظمي_packet_اندازه;
	uint32_t session_expiry_interval;
	uint16_t receive_اعظمي;
	bool lazy_reconnect;
	bool attempt_unsubscribe;
	bool initial_notification_بشپړ;
	bool outgoing_retain;
	تعداد mosquitto_bridge_reload_type reload_type;
	uint16_t max_topic_alias;
#که_تعریف_شوی WITH_TLS
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
	حرف *tls_psk_بې_اغېزه_عنصر;
	حرف *tls_psk;
#  endif
#د_که_پای
};

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
بنية libws_mqtt_hack {
	حرف *http_dir;
	بنية mosquitto__listener *listener;
};

بنية libws_mqtt_معلومات {
	بنية mosquitto *mosq;
};
#د_که_پای

#ضمن <net_mosq.h>


بنية control_endpoint {
	بنية control_endpoint *بل, *prev;
	حرف topic[];
};

خارجي بنية mosquitto_db db;

/* ============================================================
 * Main functions
 * ============================================================ */
صحيح mosquitto_main_loop(بنية mosquitto__listener_sock *listensock, صحيح listensock_شمېر);
فراغ loop_update_بل_event(time_t new_ms);

/* ============================================================
 * Config functions
 * ============================================================ */
/* Initialise config struct to default values. */
فراغ config__init(بنية mosquitto__config *config);
#که_تعریف_شوی WITH_BRIDGE
فراغ config__bridge_cleanup(بنية mosquitto__bridge *bridge);
#د_که_پای
/* Parse command line options into config. */
صحيح config_parse_دلیلونه(بنية mosquitto__config *config, صحيح argc, حرف *argv[]);
/* Read configuration data from config->config_file into config.
 * If reload is true, don't process config options that shouldn't be reloaded (listeners etc)
 * Returns 0 on success, 1 if there is a configuration error or if a file cannot be opened.
 */
صحيح config__read(بنية mosquitto__config *config, bool reload);
/* Free all config data. */
فراغ config__cleanup(بنية mosquitto__config *config);
صحيح config__get_dir_files(ثابت حرف *include_dir, حرف ***files, صحيح *file_شمېر);

/* ============================================================
 * Server send functions
 * ============================================================ */
صحيح send__connack(بنية mosquitto *context, uint8_t ack, uint8_t reason_کوډ, ثابت mosquitto_property *properties);
صحيح send__suback(بنية mosquitto *context, uint16_t mid, uint32_t payloadlen, ثابت فراغ *payload);
صحيح send__unsuback(بنية mosquitto *context, uint16_t mid, صحيح reason_کوډ_شمېر, uint8_t *reason_codes, ثابت mosquitto_property *properties);
صحيح send__auth(بنية mosquitto *context, uint8_t reason_کوډ, ثابت فراغ *auth_معلومات, uint16_t auth_معلومات_len);

/* ============================================================
 * Network functions
 * ============================================================ */
فراغ net__broker_init(فراغ);
فراغ net__broker_cleanup(فراغ);
بنية mosquitto *net__socket_accept(بنية mosquitto__listener_sock *listensock);
صحيح net__socket_listen(بنية mosquitto__listener *listener);
صحيح net_socket_get_پته(mosq_sock_t sock, حرف *buf, size_t len, uint16_t *remote_پته);
صحيح net_tls_بارول_verify(بنية mosquitto__listener *listener);
صحيح net__tls_server_ctx(بنية mosquitto__listener *listener);
صحيح net_بارول_certificates(بنية mosquitto__listener *listener);

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
#که_تعریف_شوی WITH_PERSISTENCE
صحيح persist__backup(bool shutdown);
صحيح persist__restore(فراغ);
#د_که_پای
/* Return the number of in-flight messages in count. */
صحيح db_پیغام_شمېر(صحيح *شمېر);
صحيح db_پیغام_delete_outgoing(بنية mosquitto *context, uint16_t mid, تعداد mosquitto_پیغام_state expect_state, صحيح qos);
صحيح db_پیغام_ورننباسه_outgoing(بنية mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام, uint32_t subscription_identifier, bool update, bool persist);
صحيح db_پیغام_ورننباسه_incoming(بنية mosquitto *context, uint64_t cmsg_id, بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام, bool persist);
صحيح db_پیغام_remove_incoming(بنية mosquitto *context, uint16_t mid);
صحيح db_پیغام_release_incoming(بنية mosquitto *context, uint16_t mid);
صحيح db_پیغام_update_outgoing(بنية mosquitto *context, uint16_t mid, تعداد mosquitto_پیغام_state state, صحيح qos, bool persist);
فراغ db_پیغام_له_کتار_ایستل_لومړی(بنية mosquitto *context, بنية mosquitto_پیغام_معلومات *پیغام_معلومات);
صحيح db__messages_delete(بنية mosquitto *context, bool force_free);
صحيح db__messages_delete_incoming(بنية mosquitto *context);
صحيح db__messages_delete_outgoing(بنية mosquitto *context);
صحيح db_messages_easy_کتار(بنية mosquitto *context, ثابت حرف *topic, uint8_t qos, uint32_t payloadlen, ثابت فراغ *payload, صحيح retain, uint32_t پیغام_expiry_interval, mosquitto_property **properties);
صحيح db_پیغام_store(ثابت بنية mosquitto *سرچینه, بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام, uint32_t *پیغام_expiry_interval, تعداد mosquitto_پیغام_origin origin);
صحيح db_پیغام_store_پیدا_کړه(بنية mosquitto *context, uint16_t mid, بنية mosquitto_client_پیغام **client_پیغام);
صحيح db_پیغام_store_جمع_کړه(بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام);
فراغ db_پیغام_store_remove(بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام, bool notify);
فراغ db_پیغام_store_ref_inc(بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام);
فراغ db_پیغام_store_ref_dec(بنية mosquitto_بنسټ_پیغام **بنسټ_پیغام);
فراغ db_پیغام_store_clean(فراغ);
فراغ db_پیغام_store_compact(فراغ);
فراغ db_پیغام_store_free(بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام);
صحيح db_پیغام_reconnect_reset(بنية mosquitto *context);
bool db__ready_for_flight(بنية mosquitto *context, تعداد mosquitto_پیغام_direction dir, صحيح qos);
bool db_ready_for_کتار(بنية mosquitto *context, صحيح qos, بنية mosquitto_پیغام_معلومات *پیغام_معلومات);
فراغ sys_ونه_init(فراغ);
فراغ sys_ونه_update(bool force);
صحيح db_پیغام_write_inflight_بهر_all(بنية mosquitto *context);
صحيح db_پیغام_write_inflight_بهر_latest(بنية mosquitto *context);
صحيح db_پیغام_write_queued_بهر(بنية mosquitto *context);
صحيح db_پیغام_write_queued_دننه(بنية mosquitto *context);
فراغ db_پیغام_جمع_کړه_منزل_inflight_stats(بنية mosquitto_پیغام_معلومات *پیغام_معلومات, بنية mosquitto_client_پیغام *پیغام);
فراغ db_پیغام_جمع_کړه_منزل_queued_stats(بنية mosquitto_پیغام_معلومات *پیغام_معلومات, بنية mosquitto_client_پیغام *پیغام);
uint64_t db_new_پیغام_id(فراغ);
فراغ db__expire_all_messages(بنية mosquitto *context);
فراغ db_check_acl_د_all_messages(بنية mosquitto *context);

/* ============================================================
 * Subscription functions
 * ============================================================ */
صحيح sub__init(فراغ);
صحيح sub_جمع_کړه(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub);
صحيح sub__remove(بنية mosquitto *context, ثابت حرف *sub, uint8_t *reason);
فراغ sub_ونه_print(بنية mosquitto__subhier *ریښه, صحيح level);
صحيح sub__clean_session(بنية mosquitto *context);
صحيح sub_messages_کتار(ثابت حرف *سرچینه_id, uint8_t qos, بنية mosquitto_بنسټ_پیغام **بنسټ_پیغام);
صحيح sub__topic_tokenise(ثابت حرف *subtopic, حرف **local_sub, حرف ***topics, ثابت حرف **sharename);
فراغ sub_topic_نښې_free(بنية sub__token *نښې);

/* ============================================================
 * Context functions
 * ============================================================ */
بنية mosquitto *context__init(فراغ);
صحيح context__init_sock(بنية mosquitto *context, mosq_sock_t sock, bool get_پته);
فراغ context__cleanup(بنية mosquitto *context, bool force_free);
فراغ context__disconnect(بنية mosquitto *context, صحيح reason);
فراغ context_جمع_کړه_منزل_disused(بنية mosquitto *context);
فراغ context__free_disused(فراغ);
فراغ context__send_will(بنية mosquitto *context);
فراغ context_جمع_کړه_منزل_by_id(بنية mosquitto *context);
فراغ context__remove_from_by_id(بنية mosquitto *context);

صحيح connect__on_authorised(بنية mosquitto *context, فراغ *auth_معلومات_بهر, uint16_t auth_معلومات_بهر_len);


/* ============================================================
 * Control functions
 * ============================================================ */
#که_تعریف_شوی WITH_CONTROL
صحيح control__process(بنية mosquitto *context, بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام);
فراغ control__cleanup(فراغ);
#د_که_پای
صحيح control_ثبتوونکی_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ثابت حرف *topic, فراغ *userdata);
صحيح control__unregister_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ثابت حرف *topic);
فراغ control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier);


/* ============================================================
 * Logging functions
 * ============================================================ */
صحيح log__init(بنية mosquitto__config *config);
صحيح log__close(بنية mosquitto__config *config);
فراغ log_دننی(ثابت حرف *fmt, ...) __attribute__((format(چاپ_کړه, 1, 2)));

/* ============================================================
 * Bridge functions
 * ============================================================ */
#که_تعریف_شوی WITH_BRIDGE
فراغ bridge_پیل_all(فراغ);
فراغ bridge__reload(فراغ);
فراغ bridge__db_cleanup(فراغ);
فراغ bridge__cleanup(بنية mosquitto *context);
صحيح bridge__connect(بنية mosquitto *context);
#إذا defined(__GLIBC__) && defined(WITH_ADNS)
صحيح bridge__connect_step3(بنية mosquitto *context);
#د_که_پای
صحيح bridge__on_connect(بنية mosquitto *context);
فراغ bridge_check(فراغ);
صحيح bridge_ثبتوونکی_local_connections(فراغ);
صحيح bridge_جمع_کړه_topic(بنية mosquitto__bridge *bridge, ثابت حرف *topic, تعداد mosquitto__bridge_direction direction, uint8_t qos, ثابت حرف *local_مختاړی, ثابت حرف *remote_مختاړی);
فراغ bridge__cleanup_topics(بنية mosquitto__bridge *bridge);
صحيح bridge_remap_topic_دننه(بنية mosquitto *context, حرف **topic);
#د_که_پای

/* ============================================================
 * IO multiplex related functions
 * ============================================================ */
صحيح mux__init(فراغ);
صحيح mux_جمع_کړه_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_شمېر);
صحيح mux__delete_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_شمېر);
صحيح mux__loop_prepare(فراغ);
صحيح mux__new(بنية mosquitto *context);
صحيح mux_جمع_کړه_بهر(بنية mosquitto *context);
صحيح mux_remove_بهر(بنية mosquitto *context);
صحيح mux__delete(بنية mosquitto *context);
صحيح mux__wait(فراغ);
صحيح mux__handle(بنية mosquitto__listener_sock *listensock, صحيح listensock_شمېر);
صحيح mux__cleanup(فراغ);

/* ============================================================
 * Listener related functions
 * ============================================================ */
خارجي بنية mosquitto__listener_sock *g_listensock;
خارجي صحيح g_listensock_شمېر;

فراغ listener_سېټ_defaults(بنية mosquitto__listener *listener);
فراغ listeners__reload_all_certificates(فراغ);
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
فراغ listeners_جمع_کړه_websockets(بنية lws_context *ws_context, mosq_sock_t fd);
#د_که_پای
صحيح listeners_پیل(فراغ);
فراغ listeners__stop(فراغ);

/* ============================================================
 * Plugin related functions
 * ============================================================ */
صحيح plugin_بارول_v5(mosquitto_plugin_id_t *plugin, فراغ *lib);
صحيح plugin_بارول_v4(mosquitto_plugin_id_t *plugin, فراغ *lib);
صحيح plugin_بارول_v3(mosquitto_plugin_id_t *plugin, فراغ *lib);
صحيح plugin_بارول_v2(mosquitto_plugin_id_t *plugin, فراغ *lib);
صحيح acl__pre_check(mosquitto_plugin_id_t *plugin, بنية mosquitto *context, صحيح access);

فراغ LIB_ERROR(فراغ);
فراغ plugin__handle_connect(بنية mosquitto *context);
فراغ plugin__handle_disconnect(بنية mosquitto *context, صحيح reason);
فراغ plugin__handle_client_offline(بنية mosquitto *context, صحيح reason);
صحيح plugin_handle_پیغام_دننه(بنية mosquitto *context, بنية mosquitto_بنسټ_پیغام_2 *بنسټ_پیغام);
صحيح plugin_handle_پیغام_بهر(بنية mosquitto *context, بنية mosquitto_بنسټ_پیغام_2 *بنسټ_پیغام);
صحيح plugin__handle_subscribe(بنية mosquitto *context, بنية mosquitto_subscription *sub);
صحيح plugin__handle_unsubscribe(بنية mosquitto *context, بنية mosquitto_subscription *sub);
صحيح plugin__handle_reload(فراغ);
فراغ plugin__handle_tick(فراغ);
صحيح plugin__callback_unregister_all(mosquitto_plugin_id_t *identifier);
فراغ plugin_persist__handle_restore(فراغ);
فراغ plugin_persist_handle_client_جمع_کړه(بنية mosquitto *context);
فراغ plugin_persist__handle_client_delete(بنية mosquitto *context);
فراغ plugin_persist__handle_client_update(بنية mosquitto *context);
فراغ plugin_persist_handle_subscription_جمع_کړه(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub);
فراغ plugin_persist__handle_subscription_delete(بنية mosquitto *context, حرف *sub);
فراغ plugin_persist_handle_client_پیغام_جمع_کړه(بنية mosquitto *context, ثابت بنية mosquitto_client_پیغام *cmsg);
فراغ plugin_persist_handle_client_پیغام_delete(بنية mosquitto *context, ثابت بنية mosquitto_client_پیغام *cmsg);
فراغ plugin_persist_handle_client_پیغام_update(بنية mosquitto *context, ثابت بنية mosquitto_client_پیغام *cmsg);
فراغ plugin_persist_handle_بنسټ_پیغام_جمع_کړه(بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام);
فراغ plugin_persist_handle_بنسټ_پیغام_delete(بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام);
فراغ plugin_persist_handle_retain_پیغام_سېټ(بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام);
فراغ plugin_persist_handle_retain_پیغام_delete(بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام);
فراغ plugin_persist_handle_will_جمع_کړه(بنية mosquitto *context);
فراغ plugin_persist__handle_will_delete(بنية mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
صحيح keepalive__init(فراغ);
فراغ keepalive__cleanup(فراغ);
صحيح keepalive_جمع_کړه(بنية mosquitto *context);
فراغ keepalive__check(فراغ);
صحيح keepalive__remove(بنية mosquitto *context);
صحيح keepalive__update(بنية mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
صحيح property__process_connect(بنية mosquitto *context, mosquitto_property **props);
صحيح property__process_will(بنية mosquitto *context, بنية mosquitto_پیغام_all *پیغام, mosquitto_property **props);
صحيح property__process_publish(بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام, mosquitto_property **props, صحيح *topic_alias, uint32_t *پیغام_expiry_interval, bool is_bridge);
صحيح property__process_disconnect(بنية mosquitto *context, mosquitto_property **props);

/* ============================================================
 * Retain tree related functions
 * ============================================================ */
صحيح retain__init(فراغ);
فراغ retain__clean(بنية mosquitto__retainhier **retainhier);
صحيح retain_کتار(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub);
صحيح retain__store(ثابت حرف *topic, بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام, حرف **وېشل_topics, bool persist);
فراغ retain__expiry_check(فراغ);
فراغ retain__expire(بنية mosquitto__retainhier **retainhier);

/* ============================================================
 * Security related functions
 * ============================================================ */
صحيح acl_پیدا_کړه_acls(بنية mosquitto *context);
صحيح plugin_بارول_all(فراغ);
صحيح plugin__unload_all(فراغ);
صحيح config_plugin_جمع_کړه_secopt(mosquitto_plugin_id_t *plugin, بنية mosquitto__security_options *security_options);

صحيح mosquitto_security_init(bool reload);
صحيح mosquitto_security_cleanup(bool reload);
صحيح mosquitto_acl_check(بنية mosquitto *context, ثابت حرف *topic, uint32_t payloadlen, فراغ *payload, uint8_t qos, bool retain, mosquitto_property *properties, صحيح access);
صحيح mosquitto_basic_auth(بنية mosquitto *context);
صحيح mosquitto_psk_کلی_get(بنية mosquitto *context, ثابت حرف *hint, ثابت حرف *بې_اغېزه_عنصر, حرف *کلی, صحيح max_کلی_len);

صحيح mosquitto_security_init_default(فراغ);
صحيح mosquitto_security_apply_default(فراغ);
صحيح mosquitto_security_cleanup_default(فراغ);
صحيح mosquitto_psk_کلی_get_default(بنية mosquitto *context, ثابت حرف *hint, ثابت حرف *بې_اغېزه_عنصر, حرف *کلی, صحيح max_کلی_len);
صحيح broker_acl_file__init(فراغ);
فراغ broker_acl_file__cleanup(فراغ);
صحيح broker_password_file__init(فراغ);
فراغ broker_password_file__cleanup(فراغ);
صحيح psk_file__init(فراغ);
صحيح psk_file__cleanup(فراغ);

صحيح mosquitto_security_auth_پیل(بنية mosquitto *context, bool reauth, ثابت فراغ *معلومات_دننه, uint16_t معلومات_دننه_len, فراغ **معلومات_بهر, uint16_t *معلومات_بهر_len);
صحيح mosquitto_security_auth_continue(بنية mosquitto *context, ثابت فراغ *معلومات_دننه, uint16_t معلومات_len, فراغ **معلومات_بهر, uint16_t *معلومات_بهر_len);

فراغ unpwd_free_توکی(بنية mosquitto__unpwd **unpwd, بنية mosquitto__unpwd *توکی);

/* ============================================================
 * Session expiry
 * ============================================================ */
صحيح session_expiry_جمع_کړه(بنية mosquitto *context);
صحيح session_expiry_جمع_کړه_from_persistence(بنية mosquitto *context, time_t expiry_وخت);
فراغ session_expiry_جمع_کړه_on_shutdown(فراغ);
فراغ session_expiry__remove(بنية mosquitto *context);
فراغ session_expiry__remove_all(فراغ);
فراغ session_expiry__check(فراغ);
فراغ session_expiry__send_all(فراغ);

/* ============================================================
 * Signals
 * ============================================================ */
فراغ signal_تیاری(فراغ);
صحيح signal__flag_check(فراغ);

/* ============================================================
 * Window service and signal related functions
 * ============================================================ */
#إذا defined(WIN32) || defined(__CYGWIN__)
فراغ service_install(حرف *name);
فراغ service_uninstall(حرف *name);
فراغ service_پرلپسې(حرف *name);

#که_تعریف_شوی WIN32
DWORD WINAPI SigThreadProc(فراغ *معلومات);
#د_که_پای
#د_که_پای

/* ============================================================
 * Watchdog
 * ============================================================ */
فراغ watchdog__init(فراغ);
فراغ watchdog__check(فراغ);

/* ============================================================
 * Websockets related functions
 * ============================================================ */
#که_تعریف_شوی WITH_WEBSOCKETS
#  إذا WITH_WEBSOCKETS == WS_IS_LWS
فراغ mosq_websockets_init(بنية mosquitto__listener *listener, ثابت بنية mosquitto__config *conf);
#  endif
صحيح http__context_init(بنية mosquitto *context);
صحيح http__context_cleanup(بنية mosquitto *context);
صحيح http__read(بنية mosquitto *context);
#د_که_پای
فراغ do_disconnect(بنية mosquitto *context, صحيح reason);

/* ============================================================
 * PROXY related functions
 * ============================================================ */
صحيح proxy_v2__read(بنية mosquitto *context);
صحيح proxy_v1__read(بنية mosquitto *context);

/* ============================================================
 * Will delay
 * ============================================================ */
صحيح will_delay_جمع_کړه(بنية mosquitto *context);
فراغ will_delay__check(فراغ);
فراغ will_delay__send_all(فراغ);
فراغ will_delay__remove(بنية mosquitto *mosq);

/* ============================================================
 * HTTP Info
 * ============================================================ */
صحيح http_api_پیل_local(بنية mosquitto__listener *listener);
صحيح http_api_پیل(بنية mosquitto__listener *listener);
فراغ http_api__stop(بنية mosquitto__listener *listener);


/* ============================================================
 * Other
 * ============================================================ */
#که_تعریف_شوی WITH_XTREPORT
فراغ xtreport(فراغ);
#د_که_پای

فراغ broker_control__init(فراغ);
فراغ broker_control__cleanup(فراغ);
فراغ broker_control__reload(فراغ);

#د_که_پای
