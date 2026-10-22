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

#अन्तर्भाव "config.h"
#अन्तर्भाव <stdio.h>

#जइ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#  जइ LWS_LIBRARY_VERSION_सङ्ख्या >= 3002000 && !defined(LWS_WITH_EXTERNAL_POLL)
#    warning "libwebsockets is not compiled with LWS_WITH_EXTERNAL_POLL support. Websocket performance will be unusable."
#  endif
#endif

#ifdef WITH_HTTP_API
#  include <microhttpd.h>
#endif

#ifdef __linux__
#define WITH_TCP_USER_TIMEOUT
#endif

#अन्तर्भाव "mosquitto_internal.h"
#अन्तर्भाव "mosquitto/broker.h"
#अन्तर्भाव "mosquitto/broker_plugin.h"
#अन्तर्भाव "mosquitto.h"
#अन्तर्भाव "logging_mosq.h"
#अन्तर्भाव "tls_mosq.h"
#अन्तर्भाव "uthash.h"
#अन्तर्भाव "acl_file.h"
#अन्तर्भाव "password_file.h"

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

प्रकारनिर्देश uint64_t dbid_t;

प्रकारनिर्देश पूर्णाङ्क (*FUNC_plugin_init_v5)(mosquitto_plugin_id_t *, शून्य **, संरचना mosquitto_opt *, पूर्णाङ्क);
प्रकारनिर्देश पूर्णाङ्क (*FUNC_plugin_cleanup_v5)(शून्य *, संरचना mosquitto_opt *, पूर्णाङ्क);

प्रकारनिर्देश पूर्णाङ्क (*FUNC_auth_plugin_init_v4)(शून्य **, संरचना mosquitto_opt *, पूर्णाङ्क);
प्रकारनिर्देश पूर्णाङ्क (*FUNC_auth_plugin_cleanup_v4)(शून्य *, संरचना mosquitto_opt *, पूर्णाङ्क);
प्रकारनिर्देश पूर्णाङ्क (*FUNC_auth_plugin_security_init_v4)(शून्य *, संरचना mosquitto_opt *, पूर्णाङ्क, bool);
प्रकारनिर्देश पूर्णाङ्क (*FUNC_auth_plugin_security_cleanup_v4)(शून्य *, संरचना mosquitto_opt *, पूर्णाङ्क, bool);
प्रकारनिर्देश पूर्णाङ्क (*FUNC_auth_plugin_acl_check_v4)(शून्य *, पूर्णाङ्क, संरचना mosquitto *, संरचना mosquitto_acl_सन्देशः *);
प्रकारनिर्देश पूर्णाङ्क (*FUNC_auth_plugin_unpwd_check_v4)(शून्य *, संरचना mosquitto *, ध्रुव वर्ण *, ध्रुव वर्ण *);
प्रकारनिर्देश पूर्णाङ्क (*FUNC_auth_plugin_psk_कुञ्चिका_get_v4)(शून्य *, संरचना mosquitto *, ध्रुव वर्ण *, ध्रुव वर्ण *, वर्ण *, पूर्णाङ्क);
प्रकारनिर्देश पूर्णाङ्क (*FUNC_auth_plugin_auth_start_v4)(शून्य *, संरचना mosquitto *, ध्रुव वर्ण *, bool, ध्रुव शून्य *, uint16_t, शून्य **, uint16_t *);
प्रकारनिर्देश पूर्णाङ्क (*FUNC_auth_plugin_auth_continue_v4)(शून्य *, संरचना mosquitto *, ध्रुव वर्ण *, ध्रुव शून्य *, uint16_t, शून्य **, uint16_t *);

प्रकारनिर्देश पूर्णाङ्क (*FUNC_auth_plugin_init_v3)(शून्य **, संरचना mosquitto_opt *, पूर्णाङ्क);
प्रकारनिर्देश पूर्णाङ्क (*FUNC_auth_plugin_cleanup_v3)(शून्य *, संरचना mosquitto_opt *, पूर्णाङ्क);
प्रकारनिर्देश पूर्णाङ्क (*FUNC_auth_plugin_security_init_v3)(शून्य *, संरचना mosquitto_opt *, पूर्णाङ्क, bool);
प्रकारनिर्देश पूर्णाङ्क (*FUNC_auth_plugin_security_cleanup_v3)(शून्य *, संरचना mosquitto_opt *, पूर्णाङ्क, bool);
प्रकारनिर्देश पूर्णाङ्क (*FUNC_auth_plugin_acl_check_v3)(शून्य *, पूर्णाङ्क, ध्रुव संरचना mosquitto *, संरचना mosquitto_acl_सन्देशः *);
प्रकारनिर्देश पूर्णाङ्क (*FUNC_auth_plugin_unpwd_check_v3)(शून्य *, ध्रुव संरचना mosquitto *, ध्रुव वर्ण *, ध्रुव वर्ण *);
प्रकारनिर्देश पूर्णाङ्क (*FUNC_auth_plugin_psk_कुञ्चिका_get_v3)(शून्य *, ध्रुव संरचना mosquitto *, ध्रुव वर्ण *, ध्रुव वर्ण *, वर्ण *, पूर्णाङ्क);

प्रकारनिर्देश पूर्णाङ्क (*FUNC_auth_plugin_init_v2)(शून्य **, संरचना mosquitto_auth_opt *, पूर्णाङ्क);
प्रकारनिर्देश पूर्णाङ्क (*FUNC_auth_plugin_cleanup_v2)(शून्य *, संरचना mosquitto_auth_opt *, पूर्णाङ्क);
प्रकारनिर्देश पूर्णाङ्क (*FUNC_auth_plugin_security_init_v2)(शून्य *, संरचना mosquitto_auth_opt *, पूर्णाङ्क, bool);
प्रकारनिर्देश पूर्णाङ्क (*FUNC_auth_plugin_security_cleanup_v2)(शून्य *, संरचना mosquitto_auth_opt *, पूर्णाङ्क, bool);
प्रकारनिर्देश पूर्णाङ्क (*FUNC_auth_plugin_acl_check_v2)(शून्य *, ध्रुव वर्ण *, ध्रुव वर्ण *, ध्रुव वर्ण *, पूर्णाङ्क);
प्रकारनिर्देश पूर्णाङ्क (*FUNC_auth_plugin_unpwd_check_v2)(शून्य *, ध्रुव वर्ण *, ध्रुव वर्ण *);
प्रकारनिर्देश पूर्णाङ्क (*FUNC_auth_plugin_psk_कुञ्चिका_get_v2)(शून्य *, ध्रुव वर्ण *, ध्रुव वर्ण *, वर्ण *, पूर्णाङ्क);


गणना mosquitto_सन्देशः_origin {
	mosq_mo_client = 0,
	mosq_mo_broker = 1,
};

संरचना mosquitto__plugin_lib {
	शून्य *lib;
	शून्य *user_data;
	पूर्णाङ्क (*plugin_version)(शून्य);
	संरचना mosquitto_plugin_id_t *identifier;

	FUNC_plugin_init_v5 plugin_init_v5;
	FUNC_plugin_cleanup_v5 plugin_cleanup_v5;

	FUNC_auth_plugin_init_v4 plugin_init_v4;
	FUNC_auth_plugin_cleanup_v4 plugin_cleanup_v4;
	FUNC_auth_plugin_security_init_v4 security_init_v4;
	FUNC_auth_plugin_security_cleanup_v4 security_cleanup_v4;
	FUNC_auth_plugin_acl_check_v4 acl_check_v4;
	FUNC_auth_plugin_unpwd_check_v4 unpwd_check_v4;
	FUNC_auth_plugin_psk_कुञ्चिका_get_v4 psk_कुञ्चिका_get_v4;
	FUNC_auth_plugin_auth_start_v4 auth_start_v4;
	FUNC_auth_plugin_auth_continue_v4 auth_continue_v4;

	FUNC_auth_plugin_init_v3 plugin_init_v3;
	FUNC_auth_plugin_cleanup_v3 plugin_cleanup_v3;
	FUNC_auth_plugin_security_init_v3 security_init_v3;
	FUNC_auth_plugin_security_cleanup_v3 security_cleanup_v3;
	FUNC_auth_plugin_acl_check_v3 acl_check_v3;
	FUNC_auth_plugin_unpwd_check_v3 unpwd_check_v3;
	FUNC_auth_plugin_psk_कुञ्चिका_get_v3 psk_कुञ्चिका_get_v3;

	FUNC_auth_plugin_init_v2 plugin_init_v2;
	FUNC_auth_plugin_cleanup_v2 plugin_cleanup_v2;
	FUNC_auth_plugin_security_init_v2 security_init_v2;
	FUNC_auth_plugin_security_cleanup_v2 security_cleanup_v2;
	FUNC_auth_plugin_acl_check_v2 acl_check_v2;
	FUNC_auth_plugin_unpwd_check_v2 unpwd_check_v2;
	FUNC_auth_plugin_psk_कुञ्चिका_get_v2 psk_कुञ्चिका_get_v2;
	पूर्णाङ्क version;
};

संरचना mosquitto__plugin_config {
	वर्ण *path;
	वर्ण *name;
	संरचना mosquitto_opt *options;
	संरचना mosquitto__security_options **security_options;
	पूर्णाङ्क option_गणनम्;
	पूर्णाङ्क security_option_गणनम्;
	bool deny_special_chars;
};

संरचना mosquitto__callback {
	UT_hash_handle hh; /* For callbacks that register for e.g. a specific topic */
	संरचना mosquitto__callback *next, *prev; /* For typical callbacks */
	MOSQ_FUNC_generic_callback cb;
	शून्य *userdata;
	सङ्घ {
		वर्ण *topic;
		संरचना timespec next_tick;
	} data;
	mosquitto_plugin_id_t *identifier;
};

संरचना plugin__callbacks {
	संरचना mosquitto__callback *tick;
	संरचना mosquitto__callback *acl_check;
	संरचना mosquitto__callback *basic_auth;
	संरचना mosquitto__callback *connect;
	संरचना mosquitto__callback *control;
	संरचना mosquitto__callback *disconnect;
	संरचना mosquitto__callback *client_offline;
	संरचना mosquitto__callback *ext_auth_continue;
	संरचना mosquitto__callback *ext_auth_start;
	संरचना mosquitto__callback *message_in;
	संरचना mosquitto__callback *message_out;
	संरचना mosquitto__callback *psk_कुञ्चिका;
	संरचना mosquitto__callback *reload;
	संरचना mosquitto__callback *subscribe;
	संरचना mosquitto__callback *unsubscribe;
	संरचना mosquitto__callback *persist_restore;
	संरचना mosquitto__callback *persist_client_add;
	संरचना mosquitto__callback *persist_client_delete;
	संरचना mosquitto__callback *persist_client_update;
	संरचना mosquitto__callback *persist_subscription_add;
	संरचना mosquitto__callback *persist_subscription_delete;
	संरचना mosquitto__callback *persist_client_सन्देशः_add;
	संरचना mosquitto__callback *persist_client_सन्देशः_delete;
	संरचना mosquitto__callback *persist_client_सन्देशः_update;
	संरचना mosquitto__callback *persist_base_सन्देशः_add;
	संरचना mosquitto__callback *persist_base_सन्देशः_delete;
	संरचना mosquitto__callback *persist_retain_सन्देशः_set;
	संरचना mosquitto__callback *persist_retain_सन्देशः_delete;
	संरचना mosquitto__callback *persist_will_add;
	संरचना mosquitto__callback *persist_will_delete;
};

/* This is owned by mosquitto__config or mosquitto__listener, and only referred
 * to by other structs */
संरचना mosquitto__security_options {
	/* Any options that get added here also need considering
	 * in config__read() with regards whether allow_anonymous
	 * should be disabled when these options are set.
	 */
	संरचना mosquitto__unpwd *unpwd;
	संरचना mosquitto__psk *psk_id;
	संरचना acl_file_data acl_data;
	संरचना password_file_data password_data;
	वर्ण *psk_file;
	mosquitto_plugin_id_t **plugins;
	पूर्णाङ्क plugin_गणनम्;
	int8_t allow_anonymous;
	bool allow_zero_length_clientid;
	वर्ण *auto_id_prefix;
	uint16_t auto_id_prefix_len;
	संरचना plugin__callbacks plugin_callbacks;
	mosquitto_plugin_id_t *pid; /* For registering as a "plugin" */
};

#जइ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
गणना struct_ident {
	id_invalid = 0,
	id_listener = 1,
	id_client = 2,
	id_listener_ws = 3,
};
#endif

संरचना mosquitto__listener {
	uint16_t port;
	वर्ण *host;
	वर्ण *bind_interface;
	पूर्णाङ्क max_connections;
	वर्ण *mount_point;
	mosq_sock_t *socks;
	पूर्णाङ्क sock_गणनम्;
	पूर्णाङ्क client_गणनम्;
	गणना mosquitto_protocol protocol;
	पूर्णाङ्क socket_domain;
	bool use_username_as_clientid;
	uint8_t max_qos;
	uint16_t max_topic_alias;
	uint16_t max_topic_alias_broker;
#ifdef WITH_TLS
	वर्ण *cafile;
	वर्ण *capath;
	वर्ण *certfile;
	वर्ण *keyfile;
	वर्ण *tls_engine;
	वर्ण *tls_engine_kpass_sha1;
	वर्ण *ciphers;
	वर्ण *ciphers_tls13;
	वर्ण *psk_hint;
	SSL_CTX *ssl_ctx;
	वर्ण *crlfile;
	वर्ण *tls_version;
	bool use_identity_as_username;
	bool use_subject_as_username;
	bool require_certificate;
	bool disable_client_cert_date_checks;
	गणना mosquitto__keyform tls_keyform;
#endif
#ifdef WITH_WEBSOCKETS
#  जइ WITH_WEBSOCKETS == WS_IS_LWS
	संरचना lws_context *ws_context;
	bool ws_in_init;
	संरचना lws_protocols *ws_protocol;
#  endif
	वर्ण **ws_origins;
	पूर्णाङ्क ws_origin_गणनम्;
#endif
#जइ defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
	वर्ण *http_dir;
#endif
	संरचना mosquitto__security_options *security_options;
#ifdef WITH_UNIX_SOCKETS
	वर्ण *unix_socket_path;
#endif
	bool disable_protocol_v3;
	bool disable_protocol_v4;
	bool disable_protocol_v5;
	पूर्णाङ्क enable_proxy_protocol;
	bool proxy_protocol_v2_require_tls;
#ifdef WITH_HTTP_API
	संरचना MHD_Daemon *mhd;
#endif
};


संरचना mosquitto__listener_sock {
#जइ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	/* This *must* be the first element in the struct. */
	पूर्णाङ्क ident;
#endif
	mosq_sock_t sock;
	संरचना mosquitto__listener *listener;
};


/* Callbacks belonging to a specific plugin
 * Doesn't include MOSQ_EVT_CONTROL events.
 */
संरचना plugin_own_callback {
	संरचना plugin_own_callback *next, *prev;
	MOSQ_FUNC_generic_callback cb_func;
	गणना mosquitto_plugin_event event;
};

संरचना mosquitto_plugin_id_t {
	संरचना mosquitto__plugin_config config;
	संरचना mosquitto__plugin_lib lib;
	संरचना mosquitto__listener *listener;
	वर्ण *plugin_name;
	वर्ण *plugin_version;
	संरचना control_endpoint *control_endpoints;
	संरचना plugin_own_callback *own_callbacks;
	संरचना timespec next_tick;
};

संरचना mosquitto__config {
	bool allow_duplicate_messages;
	पूर्णाङ्क autosave_interval;
	bool autosave_on_changes;
	bool check_retain_source;
	वर्ण *clientid_prefixes;
	bool connection_messages;
	uint16_t cmd_port[CMD_PORT_LIMIT];
	पूर्णाङ्क cmd_port_गणनम्;
	bool daemon;
	bool test_configuration;
	bool enable_control_api;
	पूर्णाङ्क global_max_clients;
	पूर्णाङ्क global_max_connections;
	संरचना mosquitto__listener *default_listener; /* Points to one of `listeners` */
	संरचना mosquitto__listener *listeners;
	पूर्णाङ्क listener_गणनम्;
	bool local_only;
	unsigned पूर्णाङ्क log_dest;
	पूर्णाङ्क log_facility;
	unsigned पूर्णाङ्क log_type;
	bool log_timestamp;
	वर्ण *log_timestamp_format;
	वर्ण *log_file;
	FILE *log_fptr;
	size_t max_inflight_bytes;
	size_t max_queued_bytes;
	पूर्णाङ्क max_queued_messages;
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
	वर्ण *persistence_location;
	वर्ण *persistence_file;
	वर्ण *persistence_filepath;
	time_t persistent_client_expiration;
	वर्ण *pid_file;
	bool queue_qos0_messages;
	bool per_listener_settings;
	bool retain_available;
	पूर्णाङ्क retain_expiry_interval;
	bool set_tcp_nodelay;
	पूर्णाङ्क sys_interval;
	bool upgrade_outgoing_qos;
	वर्ण *user;
#जइ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	पूर्णाङ्क websockets_log_level;
#endif
	uint16_t packet_buffer_size;
#ifdef WITH_BRIDGE
	संरचना mosquitto__bridge **bridges;
	पूर्णाङ्क bridge_गणनम्;
#endif
	संरचना mosquitto__security_options security_options;
};


संरचना mosquitto__subshared {
	UT_hash_handle hh;
	संरचना mosquitto__subleaf *subs;
	वर्ण name[];
};

संरचना mosquitto__subhier {
	UT_hash_handle hh;
	संरचना mosquitto__subhier *parent;
	संरचना mosquitto__subhier *children;
	संरचना mosquitto__subleaf *subs;
	संरचना mosquitto__subshared *shared;
	uint16_t topic_len;
	वर्ण topic[];
};

संरचना mosquitto__subleaf {
	संरचना mosquitto__subleaf *prev;
	संरचना mosquitto__subleaf *next;
	संरचना mosquitto *context;
	संरचना mosquitto__subhier *hier;
	संरचना mosquitto__subshared *shared;
	uint32_t identifier;
	uint8_t subscription_options;
	वर्ण topic_filter[];
};

संरचना sub__token {
	संरचना sub__token *next;
	uint16_t topic_len;
	वर्ण topic[];
};

संरचना mosquitto__retainhier {
	UT_hash_handle hh;
	संरचना mosquitto__retainhier *parent;
	संरचना mosquitto__retainhier *children;
	संरचना mosquitto_base_सन्देशः *retained;
	uint16_t topic_len;
	वर्ण topic[];
};

संरचना mosquitto_base_सन्देशः {
	UT_hash_handle hh;
	संरचना mosquitto_base_सन्देशः_2 data;
	संरचना mosquitto__listener *source_listener;
	वर्ण **dest_ids;
	पूर्णाङ्क dest_id_गणनम्;
	पूर्णाङ्क ref_गणनम्;
	गणना mosquitto_सन्देशः_origin origin;
	bool stored;
};

संरचना mosquitto_client_सन्देशः {
	संरचना mosquitto_client_सन्देशः_2 data;
	संरचना mosquitto_client_सन्देशः *prev;
	संरचना mosquitto_client_सन्देशः *next;
	संरचना mosquitto_base_सन्देशः *base_सन्देशः;
};


संरचना mosquitto__psk {
	UT_hash_handle hh;
	वर्ण *username;
	वर्ण *password;
};

संरचना mosquitto__message_v5 {
	संरचना mosquitto__message_v5 *next, *prev;
	वर्ण *topic;
	शून्य *payload;
	mosquitto_property *properties;
	/* clientid is used only by mosquitto_broker_publish*() to indicate
	   this message is for a specific client. */
	वर्ण *clientid;
	पूर्णाङ्क payloadlen;
	पूर्णाङ्क qos;
	bool retain;
};

संरचना mosquitto_db {
	dbid_t last_db_id;
	uint64_t ग्रन्थिः_id_shifted;
	संरचना mosquitto__subhier *normal_subs;
	संरचना mosquitto__subhier *shared_subs;
	संरचना mosquitto__retainhier *retains;
	संरचना mosquitto *contexts_by_id;
	संरचना mosquitto *contexts_by_sock;
	संरचना mosquitto *contexts_by_id_delayed_auth;
	संरचना mosquitto *contexts_for_free;
	mosquitto_plugin_id_t **plugins;
	पूर्णाङ्क plugin_गणनम्;
#ifdef WITH_BRIDGE
	संरचना mosquitto **bridges;
	पूर्णाङ्क bridge_गणनम्;
#endif
	संरचना clientid__index_hash *clientid_index_hash;
	संरचना mosquitto_base_सन्देशः *सन्देशः_store;
	time_t now_s; /* Monotonic clock, where possible */
	time_t now_real_s; /* Read clock, for measuring session/message expiry */
	uint64_t ग्रन्थिः_id; /* for unique db ids */
	time_t next_event_ms; /* for mux timeout */
	पूर्णाङ्क सन्देशः_store_गणनम्;
	unsigned long सन्देशः_store_bytes;
	वर्ण *config_file;
	संरचना mosquitto__config *config;
	bool quiet;
	bool verbose;
#ifdef WITH_SYS_वृक्षः
	पूर्णाङ्क subscription_गणनम्;
	पूर्णाङ्क shared_subscription_गणनम्;
	पूर्णाङ्क retained_गणनम्;
#endif
	पूर्णाङ्क persistence_changes;
	संरचना mosquitto *ll_for_free;
#ifdef WITH_EPOLL
	पूर्णाङ्क epollfd;
#endif
#ifdef WITH_KQUEUE
	पूर्णाङ्क kqueuefd;
#endif
	संरचना mosquitto__message_v5 *plugin_msgs;
#ifdef WITH_TLS
	/* tls_keylog can't be in the config struct because it is used
	   before the config is allocated. Config probably
	   shouldn't be separately allocated. */
	वर्ण *tls_keylog;
#endif
	bool shutdown;
};

गणना mosquitto__bridge_direction {
	bd_out = 0,
	bd_in = 1,
	bd_both = 2,
};

गणना mosquitto_bridge_start_type {
	bst_automatic = 0,
	bst_lazy = 1,
	bst_manual = 2,
	bst_once = 3,
};

गणना mosquitto_bridge_reload_type {
	brt_lazy = 0,
	brt_immediate = 1,
};

संरचना mosquitto__bridge_topic {
	संरचना mosquitto__bridge_topic *next;
	वर्ण *topic;
	वर्ण *local_prefix;
	वर्ण *remote_prefix;
	वर्ण *local_topic; /* topic prefixed with local_prefix */
	वर्ण *remote_topic; /* topic prefixed with remote_prefix */
	गणना mosquitto__bridge_direction direction;
	uint8_t qos;
};

संरचना bridge_address {
	वर्ण *address;
	uint16_t port;
};

संरचना mosquitto__bridge {
	वर्ण *name;
	संरचना bridge_address *addresses;
	पूर्णाङ्क cur_address;
	पूर्णाङ्क address_गणनम्;
	time_t primary_retry;
	mosq_sock_t primary_retry_sock;
	bool round_robin;
	bool try_private;
	bool try_private_accepted;
	bool clean_start;
	int8_t clean_start_local;
	uint16_t keepalive;
	unsigned पूर्णाङ्क tcp_keepalive_idle;
	unsigned पूर्णाङ्क tcp_keepalive_interval;
	unsigned पूर्णाङ्क tcp_keepalive_counter;
#ifdef WITH_TCP_USER_TIMEOUT
	पूर्णाङ्क tcp_user_timeout;
#endif
	संरचना mosquitto__bridge_topic *topics;
	पूर्णाङ्क topic_गणनम्;
	bool topic_remapping;
	गणना mosquitto__protocol protocol_version;
	time_t restart_t;
	time_t connected_at;
	वर्ण *remote_clientid;
	वर्ण *remote_username;
	वर्ण *remote_password;
	वर्ण *local_clientid;
	वर्ण *local_username;
	वर्ण *local_password;
	वर्ण *notification_topic;
	वर्ण *bind_address;
	bool notifications;
	bool notifications_local_only;
	गणना mosquitto_bridge_start_type start_type;
	पूर्णाङ्क idle_timeout;
	पूर्णाङ्क restart_timeout;
	पूर्णाङ्क backoff_base;
	पूर्णाङ्क backoff_cap;
	पूर्णाङ्क stable_connection_period;
	पूर्णाङ्क threshold;
	uint32_t maximum_packet_size;
	uint32_t session_expiry_interval;
	uint16_t receive_maximum;
	bool lazy_reconnect;
	bool attempt_unsubscribe;
	bool initial_notification_done;
	bool outgoing_retain;
	गणना mosquitto_bridge_reload_type reload_type;
	uint16_t max_topic_alias;
#ifdef WITH_TLS
	bool tls_insecure;
	bool tls_ocsp_required;
	bool tls_use_os_certs;
	वर्ण *tls_cafile;
	वर्ण *tls_capath;
	वर्ण *tls_certfile;
	वर्ण *tls_keyfile;
	वर्ण *tls_version;
	वर्ण *tls_alpn;
	वर्ण *tls_ciphers;
	वर्ण *tls_13_ciphers;
#  ifdef FINAL_WITH_TLS_PSK
	वर्ण *tls_psk_identity;
	वर्ण *tls_psk;
#  endif
#endif
};

#जइ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
संरचना libws_mqtt_hack {
	वर्ण *http_dir;
	संरचना mosquitto__listener *listener;
};

संरचना libws_mqtt_data {
	संरचना mosquitto *mosq;
};
#endif

#अन्तर्भाव <net_mosq.h>


संरचना control_endpoint {
	संरचना control_endpoint *next, *prev;
	वर्ण topic[];
};

बाह्य संरचना mosquitto_db db;

/* ============================================================
 * Main functions
 * ============================================================ */
पूर्णाङ्क mosquitto_main_loop(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्);
शून्य loop__update_next_event(time_t new_ms);

/* ============================================================
 * Config functions
 * ============================================================ */
/* Initialise config struct to default values. */
शून्य config__init(संरचना mosquitto__config *config);
#ifdef WITH_BRIDGE
शून्य config__bridge_cleanup(संरचना mosquitto__bridge *bridge);
#endif
/* Parse command line options into config. */
पूर्णाङ्क config__parse_args(संरचना mosquitto__config *config, पूर्णाङ्क argc, वर्ण *argv[]);
/* Read configuration data from config->config_file into config.
 * If reload is true, don't process config options that shouldn't be reloaded (listeners etc)
 * Returns 0 on success, 1 if there is a configuration error or if a file cannot be opened.
 */
पूर्णाङ्क config__read(संरचना mosquitto__config *config, bool reload);
/* Free all config data. */
शून्य config__cleanup(संरचना mosquitto__config *config);
पूर्णाङ्क config__get_dir_files(ध्रुव वर्ण *include_dir, वर्ण ***files, पूर्णाङ्क *file_गणनम्);

/* ============================================================
 * Server send functions
 * ============================================================ */
पूर्णाङ्क send__connack(संरचना mosquitto *context, uint8_t ack, uint8_t reason_code, ध्रुव mosquitto_property *properties);
पूर्णाङ्क send__suback(संरचना mosquitto *context, uint16_t mid, uint32_t payloadlen, ध्रुव शून्य *payload);
पूर्णाङ्क send__unsuback(संरचना mosquitto *context, uint16_t mid, पूर्णाङ्क reason_code_गणनम्, uint8_t *reason_codes, ध्रुव mosquitto_property *properties);
पूर्णाङ्क send__auth(संरचना mosquitto *context, uint8_t reason_code, ध्रुव शून्य *auth_data, uint16_t auth_data_len);

/* ============================================================
 * Network functions
 * ============================================================ */
शून्य net__broker_init(शून्य);
शून्य net__broker_cleanup(शून्य);
संरचना mosquitto *net__socket_accept(संरचना mosquitto__listener_sock *listensock);
पूर्णाङ्क net__socket_listen(संरचना mosquitto__listener *listener);
पूर्णाङ्क net__socket_get_address(mosq_sock_t sock, वर्ण *buf, size_t len, uint16_t *remote_address);
पूर्णाङ्क net__tls_load_verify(संरचना mosquitto__listener *listener);
पूर्णाङ्क net__tls_server_ctx(संरचना mosquitto__listener *listener);
पूर्णाङ्क net__load_certificates(संरचना mosquitto__listener *listener);

/* ============================================================
 * Read handling functions
 * ============================================================ */
पूर्णाङ्क handle__packet(संरचना mosquitto *context);
पूर्णाङ्क handle__connack(संरचना mosquitto *context);
पूर्णाङ्क handle__connect(संरचना mosquitto *context);
पूर्णाङ्क handle__disconnect(संरचना mosquitto *context);
पूर्णाङ्क handle__publish(संरचना mosquitto *context);
पूर्णाङ्क handle__subscribe(संरचना mosquitto *context);
पूर्णाङ्क handle__unsubscribe(संरचना mosquitto *context);
पूर्णाङ्क handle__auth(संरचना mosquitto *context);

/* ============================================================
 * Database handling
 * ============================================================ */
पूर्णाङ्क db__open(संरचना mosquitto__config *config);
पूर्णाङ्क db__close(शून्य);
#ifdef WITH_PERSISTENCE
पूर्णाङ्क persist__backup(bool shutdown);
पूर्णाङ्क persist__restore(शून्य);
#endif
/* Return the number of in-flight messages in count. */
पूर्णाङ्क db_message_गणनम्(पूर्णाङ्क *गणनम्);
पूर्णाङ्क db__message_delete_outgoing(संरचना mosquitto *context, uint16_t mid, गणना mosquitto_सन्देशः_state expect_state, पूर्णाङ्क qos);
पूर्णाङ्क db_message_निवेशय_outgoing(संरचना mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, संरचना mosquitto_base_सन्देशः *base_सन्देशः, uint32_t subscription_identifier, bool update, bool persist);
पूर्णाङ्क db_message_निवेशय_incoming(संरचना mosquitto *context, uint64_t cmsg_id, संरचना mosquitto_base_सन्देशः *base_सन्देशः, bool persist);
पूर्णाङ्क db__message_remove_incoming(संरचना mosquitto *context, uint16_t mid);
पूर्णाङ्क db__message_release_incoming(संरचना mosquitto *context, uint16_t mid);
पूर्णाङ्क db__message_update_outgoing(संरचना mosquitto *context, uint16_t mid, गणना mosquitto_सन्देशः_state state, पूर्णाङ्क qos, bool persist);
शून्य db__message_dequeue_first(संरचना mosquitto *context, संरचना mosquitto_सन्देशः_data *सन्देशः_data);
पूर्णाङ्क db__messages_delete(संरचना mosquitto *context, bool force_free);
पूर्णाङ्क db__messages_delete_incoming(संरचना mosquitto *context);
पूर्णाङ्क db__messages_delete_outgoing(संरचना mosquitto *context);
पूर्णाङ्क db__messages_easy_queue(संरचना mosquitto *context, ध्रुव वर्ण *topic, uint8_t qos, uint32_t payloadlen, ध्रुव शून्य *payload, पूर्णाङ्क retain, uint32_t message_expiry_interval, mosquitto_property **properties);
पूर्णाङ्क db__message_store(ध्रुव संरचना mosquitto *source, संरचना mosquitto_base_सन्देशः *base_सन्देशः, uint32_t *message_expiry_interval, गणना mosquitto_सन्देशः_origin origin);
पूर्णाङ्क db__message_store_find(संरचना mosquitto *context, uint16_t mid, संरचना mosquitto_client_सन्देशः **client_सन्देशः);
पूर्णाङ्क db_सन्देशः_store_add(संरचना mosquitto_base_सन्देशः *base_सन्देशः);
शून्य db_सन्देशः_store_remove(संरचना mosquitto_base_सन्देशः *base_सन्देशः, bool notify);
शून्य db_सन्देशः_store_ref_inc(संरचना mosquitto_base_सन्देशः *base_सन्देशः);
शून्य db_सन्देशः_store_ref_dec(संरचना mosquitto_base_सन्देशः **base_सन्देशः);
शून्य db_सन्देशः_store_clean(शून्य);
शून्य db_सन्देशः_store_compact(शून्य);
शून्य db_सन्देशः_store_free(संरचना mosquitto_base_सन्देशः *base_सन्देशः);
पूर्णाङ्क db__message_reconnect_reset(संरचना mosquitto *context);
bool db__ready_for_flight(संरचना mosquitto *context, गणना mosquitto_सन्देशः_direction dir, पूर्णाङ्क qos);
bool db__ready_for_queue(संरचना mosquitto *context, पूर्णाङ्क qos, संरचना mosquitto_सन्देशः_data *सन्देशः_data);
शून्य sys_वृक्षः_init(शून्य);
शून्य sys_वृक्षः_update(bool force);
पूर्णाङ्क db__message_write_inflight_out_all(संरचना mosquitto *context);
पूर्णाङ्क db__message_write_inflight_out_latest(संरचना mosquitto *context);
पूर्णाङ्क db__message_write_queued_out(संरचना mosquitto *context);
पूर्णाङ्क db__message_write_queued_in(संरचना mosquitto *context);
शून्य db_सन्देशः_add_to_inflight_stats(संरचना mosquitto_सन्देशः_data *सन्देशः_data, संरचना mosquitto_client_सन्देशः *सन्देशः);
शून्य db_सन्देशः_add_to_queued_stats(संरचना mosquitto_सन्देशः_data *सन्देशः_data, संरचना mosquitto_client_सन्देशः *सन्देशः);
uint64_t db_new_सन्देशः_id(शून्य);
शून्य db__expire_all_messages(संरचना mosquitto *context);
शून्य db__check_acl_of_all_messages(संरचना mosquitto *context);

/* ============================================================
 * Subscription functions
 * ============================================================ */
पूर्णाङ्क sub__init(शून्य);
पूर्णाङ्क sub__add(संरचना mosquitto *context, ध्रुव संरचना mosquitto_subscription *sub);
पूर्णाङ्क sub__remove(संरचना mosquitto *context, ध्रुव वर्ण *sub, uint8_t *reason);
शून्य sub_वृक्षः_print(संरचना mosquitto__subhier *मूलम्, पूर्णाङ्क level);
पूर्णाङ्क sub__clean_session(संरचना mosquitto *context);
पूर्णाङ्क sub__messages_queue(ध्रुव वर्ण *source_id, uint8_t qos, संरचना mosquitto_base_सन्देशः **base_सन्देशः);
पूर्णाङ्क sub__topic_tokenise(ध्रुव वर्ण *subtopic, वर्ण **local_sub, वर्ण ***topics, ध्रुव वर्ण **sharename);
शून्य sub__topic_tokens_free(संरचना sub__token *tokens);

/* ============================================================
 * Context functions
 * ============================================================ */
संरचना mosquitto *context__init(शून्य);
पूर्णाङ्क context__init_sock(संरचना mosquitto *context, mosq_sock_t sock, bool get_address);
शून्य context__cleanup(संरचना mosquitto *context, bool force_free);
शून्य context__disconnect(संरचना mosquitto *context, पूर्णाङ्क reason);
शून्य context__add_to_disused(संरचना mosquitto *context);
शून्य context__free_disused(शून्य);
शून्य context__send_will(संरचना mosquitto *context);
शून्य context__add_to_by_id(संरचना mosquitto *context);
शून्य context__remove_from_by_id(संरचना mosquitto *context);

पूर्णाङ्क connect__on_authorised(संरचना mosquitto *context, शून्य *auth_data_out, uint16_t auth_data_out_len);


/* ============================================================
 * Control functions
 * ============================================================ */
#ifdef WITH_CONTROL
पूर्णाङ्क control__process(संरचना mosquitto *context, संरचना mosquitto_base_सन्देशः *base_सन्देशः);
शून्य control__cleanup(शून्य);
#endif
पूर्णाङ्क control__register_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ध्रुव वर्ण *topic, शून्य *userdata);
पूर्णाङ्क control__unregister_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ध्रुव वर्ण *topic);
शून्य control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier);


/* ============================================================
 * Logging functions
 * ============================================================ */
पूर्णाङ्क log__init(संरचना mosquitto__config *config);
पूर्णाङ्क log__close(संरचना mosquitto__config *config);
शून्य log__internal(ध्रुव वर्ण *fmt, ...) __attribute__((format(printf, 1, 2)));

/* ============================================================
 * Bridge functions
 * ============================================================ */
#ifdef WITH_BRIDGE
शून्य bridge__start_all(शून्य);
शून्य bridge__reload(शून्य);
शून्य bridge__db_cleanup(शून्य);
शून्य bridge__cleanup(संरचना mosquitto *context);
पूर्णाङ्क bridge__connect(संरचना mosquitto *context);
#जइ defined(__GLIBC__) && defined(WITH_ADNS)
पूर्णाङ्क bridge__connect_step3(संरचना mosquitto *context);
#endif
पूर्णाङ्क bridge__on_connect(संरचना mosquitto *context);
शून्य bridge_check(शून्य);
पूर्णाङ्क bridge__register_local_connections(शून्य);
पूर्णाङ्क bridge__add_topic(संरचना mosquitto__bridge *bridge, ध्रुव वर्ण *topic, गणना mosquitto__bridge_direction direction, uint8_t qos, ध्रुव वर्ण *local_prefix, ध्रुव वर्ण *remote_prefix);
शून्य bridge__cleanup_topics(संरचना mosquitto__bridge *bridge);
पूर्णाङ्क bridge__remap_topic_in(संरचना mosquitto *context, वर्ण **topic);
#endif

/* ============================================================
 * IO multiplex related functions
 * ============================================================ */
पूर्णाङ्क mux__init(शून्य);
पूर्णाङ्क mux__add_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्);
पूर्णाङ्क mux__delete_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्);
पूर्णाङ्क mux__loop_prepare(शून्य);
पूर्णाङ्क mux__new(संरचना mosquitto *context);
पूर्णाङ्क mux__add_out(संरचना mosquitto *context);
पूर्णाङ्क mux__remove_out(संरचना mosquitto *context);
पूर्णाङ्क mux__delete(संरचना mosquitto *context);
पूर्णाङ्क mux__wait(शून्य);
पूर्णाङ्क mux__handle(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्);
पूर्णाङ्क mux__cleanup(शून्य);

/* ============================================================
 * Listener related functions
 * ============================================================ */
बाह्य संरचना mosquitto__listener_sock *g_listensock;
बाह्य पूर्णाङ्क g_listensock_गणनम्;

शून्य listener__set_defaults(संरचना mosquitto__listener *listener);
शून्य listeners__reload_all_certificates(शून्य);
#जइ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
शून्य listeners__add_websockets(संरचना lws_context *ws_context, mosq_sock_t fd);
#endif
पूर्णाङ्क listeners__start(शून्य);
शून्य listeners__stop(शून्य);

/* ============================================================
 * Plugin related functions
 * ============================================================ */
पूर्णाङ्क plugin__load_v5(mosquitto_plugin_id_t *plugin, शून्य *lib);
पूर्णाङ्क plugin__load_v4(mosquitto_plugin_id_t *plugin, शून्य *lib);
पूर्णाङ्क plugin__load_v3(mosquitto_plugin_id_t *plugin, शून्य *lib);
पूर्णाङ्क plugin__load_v2(mosquitto_plugin_id_t *plugin, शून्य *lib);
पूर्णाङ्क acl__pre_check(mosquitto_plugin_id_t *plugin, संरचना mosquitto *context, पूर्णाङ्क access);

शून्य LIB_ERROR(शून्य);
शून्य plugin__handle_connect(संरचना mosquitto *context);
शून्य plugin__handle_disconnect(संरचना mosquitto *context, पूर्णाङ्क reason);
शून्य plugin__handle_client_offline(संरचना mosquitto *context, पूर्णाङ्क reason);
पूर्णाङ्क plugin__handle_message_in(संरचना mosquitto *context, संरचना mosquitto_base_सन्देशः_2 *base_सन्देशः);
पूर्णाङ्क plugin__handle_message_out(संरचना mosquitto *context, संरचना mosquitto_base_सन्देशः_2 *base_सन्देशः);
पूर्णाङ्क plugin__handle_subscribe(संरचना mosquitto *context, संरचना mosquitto_subscription *sub);
पूर्णाङ्क plugin__handle_unsubscribe(संरचना mosquitto *context, संरचना mosquitto_subscription *sub);
पूर्णाङ्क plugin__handle_reload(शून्य);
शून्य plugin__handle_tick(शून्य);
पूर्णाङ्क plugin__callback_unregister_all(mosquitto_plugin_id_t *identifier);
शून्य plugin_persist__handle_restore(शून्य);
शून्य plugin_persist__handle_client_add(संरचना mosquitto *context);
शून्य plugin_persist__handle_client_delete(संरचना mosquitto *context);
शून्य plugin_persist__handle_client_update(संरचना mosquitto *context);
शून्य plugin_persist__handle_subscription_add(संरचना mosquitto *context, ध्रुव संरचना mosquitto_subscription *sub);
शून्य plugin_persist__handle_subscription_delete(संरचना mosquitto *context, वर्ण *sub);
शून्य plugin_persist_handle_client_सन्देशः_add(संरचना mosquitto *context, ध्रुव संरचना mosquitto_client_सन्देशः *cmsg);
शून्य plugin_persist_handle_client_सन्देशः_delete(संरचना mosquitto *context, ध्रुव संरचना mosquitto_client_सन्देशः *cmsg);
शून्य plugin_persist_handle_client_सन्देशः_update(संरचना mosquitto *context, ध्रुव संरचना mosquitto_client_सन्देशः *cmsg);
शून्य plugin_persist_handle_base_सन्देशः_add(संरचना mosquitto_base_सन्देशः *base_सन्देशः);
शून्य plugin_persist_handle_base_सन्देशः_delete(संरचना mosquitto_base_सन्देशः *base_सन्देशः);
शून्य plugin_persist_handle_retain_सन्देशः_set(संरचना mosquitto_base_सन्देशः *base_सन्देशः);
शून्य plugin_persist_handle_retain_सन्देशः_delete(संरचना mosquitto_base_सन्देशः *base_सन्देशः);
शून्य plugin_persist__handle_will_add(संरचना mosquitto *context);
शून्य plugin_persist__handle_will_delete(संरचना mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
पूर्णाङ्क keepalive__init(शून्य);
शून्य keepalive__cleanup(शून्य);
पूर्णाङ्क keepalive__add(संरचना mosquitto *context);
शून्य keepalive__check(शून्य);
पूर्णाङ्क keepalive__remove(संरचना mosquitto *context);
पूर्णाङ्क keepalive__update(संरचना mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
पूर्णाङ्क property__process_connect(संरचना mosquitto *context, mosquitto_property **props);
पूर्णाङ्क property__process_will(संरचना mosquitto *context, संरचना mosquitto_message_all *सन्देशः, mosquitto_property **props);
पूर्णाङ्क property__process_publish(संरचना mosquitto_base_सन्देशः *base_सन्देशः, mosquitto_property **props, पूर्णाङ्क *topic_alias, uint32_t *message_expiry_interval, bool is_bridge);
पूर्णाङ्क property__process_disconnect(संरचना mosquitto *context, mosquitto_property **props);

/* ============================================================
 * Retain tree related functions
 * ============================================================ */
पूर्णाङ्क retain__init(शून्य);
शून्य retain__clean(संरचना mosquitto__retainhier **retainhier);
पूर्णाङ्क retain__queue(संरचना mosquitto *context, ध्रुव संरचना mosquitto_subscription *sub);
पूर्णाङ्क retain__store(ध्रुव वर्ण *topic, संरचना mosquitto_base_सन्देशः *base_सन्देशः, वर्ण **split_topics, bool persist);
शून्य retain__expiry_check(शून्य);
शून्य retain__expire(संरचना mosquitto__retainhier **retainhier);

/* ============================================================
 * Security related functions
 * ============================================================ */
पूर्णाङ्क acl__find_acls(संरचना mosquitto *context);
पूर्णाङ्क plugin__load_all(शून्य);
पूर्णाङ्क plugin__unload_all(शून्य);
पूर्णाङ्क config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, संरचना mosquitto__security_options *security_options);

पूर्णाङ्क mosquitto_security_init(bool reload);
पूर्णाङ्क mosquitto_security_cleanup(bool reload);
पूर्णाङ्क mosquitto_acl_check(संरचना mosquitto *context, ध्रुव वर्ण *topic, uint32_t payloadlen, शून्य *payload, uint8_t qos, bool retain, mosquitto_property *properties, पूर्णाङ्क access);
पूर्णाङ्क mosquitto_basic_auth(संरचना mosquitto *context);
पूर्णाङ्क mosquitto_psk_कुञ्चिका_get(संरचना mosquitto *context, ध्रुव वर्ण *hint, ध्रुव वर्ण *identity, वर्ण *कुञ्चिका, पूर्णाङ्क max_कुञ्चिका_len);

पूर्णाङ्क mosquitto_security_init_default(शून्य);
पूर्णाङ्क mosquitto_security_apply_default(शून्य);
पूर्णाङ्क mosquitto_security_cleanup_default(शून्य);
पूर्णाङ्क mosquitto_psk_कुञ्चिका_get_default(संरचना mosquitto *context, ध्रुव वर्ण *hint, ध्रुव वर्ण *identity, वर्ण *कुञ्चिका, पूर्णाङ्क max_कुञ्चिका_len);
पूर्णाङ्क broker_acl_file__init(शून्य);
शून्य broker_acl_file__cleanup(शून्य);
पूर्णाङ्क broker_password_file__init(शून्य);
शून्य broker_password_file__cleanup(शून्य);
पूर्णाङ्क psk_file__init(शून्य);
पूर्णाङ्क psk_file__cleanup(शून्य);

पूर्णाङ्क mosquitto_security_auth_start(संरचना mosquitto *context, bool reauth, ध्रुव शून्य *data_in, uint16_t data_in_len, शून्य **data_out, uint16_t *data_out_len);
पूर्णाङ्क mosquitto_security_auth_continue(संरचना mosquitto *context, ध्रुव शून्य *data_in, uint16_t data_len, शून्य **data_out, uint16_t *data_out_len);

शून्य unpwd__free_item(संरचना mosquitto__unpwd **unpwd, संरचना mosquitto__unpwd *item);

/* ============================================================
 * Session expiry
 * ============================================================ */
पूर्णाङ्क session_expiry__add(संरचना mosquitto *context);
पूर्णाङ्क session_expiry__add_from_persistence(संरचना mosquitto *context, time_t expiry_time);
शून्य session_expiry__add_on_shutdown(शून्य);
शून्य session_expiry__remove(संरचना mosquitto *context);
शून्य session_expiry__remove_all(शून्य);
शून्य session_expiry__check(शून्य);
शून्य session_expiry__send_all(शून्य);

/* ============================================================
 * Signals
 * ============================================================ */
शून्य signal__setup(शून्य);
पूर्णाङ्क signal__flag_check(शून्य);

/* ============================================================
 * Window service and signal related functions
 * ============================================================ */
#जइ defined(WIN32) || defined(__CYGWIN__)
शून्य service_install(वर्ण *name);
शून्य service_uninstall(वर्ण *name);
शून्य service_run(वर्ण *name);

#ifdef WIN32
DWORD WINAPI SigThreadProc(शून्य *data);
#endif
#endif

/* ============================================================
 * Watchdog
 * ============================================================ */
शून्य watchdog__init(शून्य);
शून्य watchdog__check(शून्य);

/* ============================================================
 * Websockets related functions
 * ============================================================ */
#ifdef WITH_WEBSOCKETS
#  जइ WITH_WEBSOCKETS == WS_IS_LWS
शून्य mosq_websockets_init(संरचना mosquitto__listener *listener, ध्रुव संरचना mosquitto__config *conf);
#  endif
पूर्णाङ्क http__context_init(संरचना mosquitto *context);
पूर्णाङ्क http__context_cleanup(संरचना mosquitto *context);
पूर्णाङ्क http__read(संरचना mosquitto *context);
#endif
शून्य do_disconnect(संरचना mosquitto *context, पूर्णाङ्क reason);

/* ============================================================
 * PROXY related functions
 * ============================================================ */
पूर्णाङ्क proxy_v2__read(संरचना mosquitto *context);
पूर्णाङ्क proxy_v1__read(संरचना mosquitto *context);

/* ============================================================
 * Will delay
 * ============================================================ */
पूर्णाङ्क will_delay__add(संरचना mosquitto *context);
शून्य will_delay__check(शून्य);
शून्य will_delay__send_all(शून्य);
शून्य will_delay__remove(संरचना mosquitto *mosq);

/* ============================================================
 * HTTP Info
 * ============================================================ */
पूर्णाङ्क http_api__start_local(संरचना mosquitto__listener *listener);
पूर्णाङ्क http_api__start(संरचना mosquitto__listener *listener);
शून्य http_api__stop(संरचना mosquitto__listener *listener);


/* ============================================================
 * Other
 * ============================================================ */
#ifdef WITH_XTREPORT
शून्य xtreport(शून्य);
#endif

शून्य broker_control__init(शून्य);
शून्य broker_control__cleanup(शून्य);
शून्य broker_control__reload(शून्य);

#endif
