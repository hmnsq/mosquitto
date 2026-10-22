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

#जर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#  जर LWS_LIBRARY_VERSION_संख्या >= 3002000 && !defined(LWS_WITH_EXTERNAL_POLL)
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

प्रकारव्याख्या uint64_t dbid_t;

प्रकारव्याख्या पूर्णांक (*FUNC_plugin_init_v5)(mosquitto_plugin_id_t *, शून्य **, संरचना mosquitto_opt *, पूर्णांक);
प्रकारव्याख्या पूर्णांक (*FUNC_plugin_cleanup_v5)(शून्य *, संरचना mosquitto_opt *, पूर्णांक);

प्रकारव्याख्या पूर्णांक (*FUNC_auth_plugin_init_v4)(शून्य **, संरचना mosquitto_opt *, पूर्णांक);
प्रकारव्याख्या पूर्णांक (*FUNC_auth_plugin_cleanup_v4)(शून्य *, संरचना mosquitto_opt *, पूर्णांक);
प्रकारव्याख्या पूर्णांक (*FUNC_auth_plugin_security_init_v4)(शून्य *, संरचना mosquitto_opt *, पूर्णांक, bool);
प्रकारव्याख्या पूर्णांक (*FUNC_auth_plugin_security_cleanup_v4)(शून्य *, संरचना mosquitto_opt *, पूर्णांक, bool);
प्रकारव्याख्या पूर्णांक (*FUNC_auth_plugin_acl_check_v4)(शून्य *, पूर्णांक, संरचना mosquitto *, संरचना mosquitto_acl_संदेश *);
प्रकारव्याख्या पूर्णांक (*FUNC_auth_plugin_unpwd_check_v4)(शून्य *, संरचना mosquitto *, स्थिरांक अक्षर *, स्थिरांक अक्षर *);
प्रकारव्याख्या पूर्णांक (*FUNC_auth_plugin_psk_किल्ली_get_v4)(शून्य *, संरचना mosquitto *, स्थिरांक अक्षर *, स्थिरांक अक्षर *, अक्षर *, पूर्णांक);
प्रकारव्याख्या पूर्णांक (*FUNC_auth_plugin_auth_start_v4)(शून्य *, संरचना mosquitto *, स्थिरांक अक्षर *, bool, स्थिरांक शून्य *, uint16_t, शून्य **, uint16_t *);
प्रकारव्याख्या पूर्णांक (*FUNC_auth_plugin_auth_continue_v4)(शून्य *, संरचना mosquitto *, स्थिरांक अक्षर *, स्थिरांक शून्य *, uint16_t, शून्य **, uint16_t *);

प्रकारव्याख्या पूर्णांक (*FUNC_auth_plugin_init_v3)(शून्य **, संरचना mosquitto_opt *, पूर्णांक);
प्रकारव्याख्या पूर्णांक (*FUNC_auth_plugin_cleanup_v3)(शून्य *, संरचना mosquitto_opt *, पूर्णांक);
प्रकारव्याख्या पूर्णांक (*FUNC_auth_plugin_security_init_v3)(शून्य *, संरचना mosquitto_opt *, पूर्णांक, bool);
प्रकारव्याख्या पूर्णांक (*FUNC_auth_plugin_security_cleanup_v3)(शून्य *, संरचना mosquitto_opt *, पूर्णांक, bool);
प्रकारव्याख्या पूर्णांक (*FUNC_auth_plugin_acl_check_v3)(शून्य *, पूर्णांक, स्थिरांक संरचना mosquitto *, संरचना mosquitto_acl_संदेश *);
प्रकारव्याख्या पूर्णांक (*FUNC_auth_plugin_unpwd_check_v3)(शून्य *, स्थिरांक संरचना mosquitto *, स्थिरांक अक्षर *, स्थिरांक अक्षर *);
प्रकारव्याख्या पूर्णांक (*FUNC_auth_plugin_psk_किल्ली_get_v3)(शून्य *, स्थिरांक संरचना mosquitto *, स्थिरांक अक्षर *, स्थिरांक अक्षर *, अक्षर *, पूर्णांक);

प्रकारव्याख्या पूर्णांक (*FUNC_auth_plugin_init_v2)(शून्य **, संरचना mosquitto_auth_opt *, पूर्णांक);
प्रकारव्याख्या पूर्णांक (*FUNC_auth_plugin_cleanup_v2)(शून्य *, संरचना mosquitto_auth_opt *, पूर्णांक);
प्रकारव्याख्या पूर्णांक (*FUNC_auth_plugin_security_init_v2)(शून्य *, संरचना mosquitto_auth_opt *, पूर्णांक, bool);
प्रकारव्याख्या पूर्णांक (*FUNC_auth_plugin_security_cleanup_v2)(शून्य *, संरचना mosquitto_auth_opt *, पूर्णांक, bool);
प्रकारव्याख्या पूर्णांक (*FUNC_auth_plugin_acl_check_v2)(शून्य *, स्थिरांक अक्षर *, स्थिरांक अक्षर *, स्थिरांक अक्षर *, पूर्णांक);
प्रकारव्याख्या पूर्णांक (*FUNC_auth_plugin_unpwd_check_v2)(शून्य *, स्थिरांक अक्षर *, स्थिरांक अक्षर *);
प्रकारव्याख्या पूर्णांक (*FUNC_auth_plugin_psk_किल्ली_get_v2)(शून्य *, स्थिरांक अक्षर *, स्थिरांक अक्षर *, अक्षर *, पूर्णांक);


गणना mosquitto_संदेश_origin {
	mosq_mo_client = 0,
	mosq_mo_broker = 1,
};

संरचना mosquitto__plugin_lib {
	शून्य *lib;
	शून्य *user_data;
	पूर्णांक (*plugin_version)(शून्य);
	संरचना mosquitto_plugin_id_t *identifier;

	FUNC_plugin_init_v5 plugin_init_v5;
	FUNC_plugin_cleanup_v5 plugin_cleanup_v5;

	FUNC_auth_plugin_init_v4 plugin_init_v4;
	FUNC_auth_plugin_cleanup_v4 plugin_cleanup_v4;
	FUNC_auth_plugin_security_init_v4 security_init_v4;
	FUNC_auth_plugin_security_cleanup_v4 security_cleanup_v4;
	FUNC_auth_plugin_acl_check_v4 acl_check_v4;
	FUNC_auth_plugin_unpwd_check_v4 unpwd_check_v4;
	FUNC_auth_plugin_psk_किल्ली_get_v4 psk_किल्ली_get_v4;
	FUNC_auth_plugin_auth_start_v4 auth_start_v4;
	FUNC_auth_plugin_auth_continue_v4 auth_continue_v4;

	FUNC_auth_plugin_init_v3 plugin_init_v3;
	FUNC_auth_plugin_cleanup_v3 plugin_cleanup_v3;
	FUNC_auth_plugin_security_init_v3 security_init_v3;
	FUNC_auth_plugin_security_cleanup_v3 security_cleanup_v3;
	FUNC_auth_plugin_acl_check_v3 acl_check_v3;
	FUNC_auth_plugin_unpwd_check_v3 unpwd_check_v3;
	FUNC_auth_plugin_psk_किल्ली_get_v3 psk_किल्ली_get_v3;

	FUNC_auth_plugin_init_v2 plugin_init_v2;
	FUNC_auth_plugin_cleanup_v2 plugin_cleanup_v2;
	FUNC_auth_plugin_security_init_v2 security_init_v2;
	FUNC_auth_plugin_security_cleanup_v2 security_cleanup_v2;
	FUNC_auth_plugin_acl_check_v2 acl_check_v2;
	FUNC_auth_plugin_unpwd_check_v2 unpwd_check_v2;
	FUNC_auth_plugin_psk_किल्ली_get_v2 psk_किल्ली_get_v2;
	पूर्णांक version;
};

संरचना mosquitto__plugin_config {
	अक्षर *path;
	अक्षर *name;
	संरचना mosquitto_opt *options;
	संरचना mosquitto__security_options **security_options;
	पूर्णांक option_मोजणी;
	पूर्णांक security_option_मोजणी;
	bool deny_special_chars;
};

संरचना mosquitto__callback {
	UT_hash_handle hh; /* For callbacks that register for e.g. a specific topic */
	संरचना mosquitto__callback *next, *prev; /* For typical callbacks */
	MOSQ_FUNC_generic_callback cb;
	शून्य *userdata;
	संघ {
		अक्षर *topic;
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
	संरचना mosquitto__callback *psk_किल्ली;
	संरचना mosquitto__callback *reload;
	संरचना mosquitto__callback *subscribe;
	संरचना mosquitto__callback *unsubscribe;
	संरचना mosquitto__callback *persist_restore;
	संरचना mosquitto__callback *persist_client_add;
	संरचना mosquitto__callback *persist_client_delete;
	संरचना mosquitto__callback *persist_client_update;
	संरचना mosquitto__callback *persist_subscription_add;
	संरचना mosquitto__callback *persist_subscription_delete;
	संरचना mosquitto__callback *persist_client_संदेश_add;
	संरचना mosquitto__callback *persist_client_संदेश_delete;
	संरचना mosquitto__callback *persist_client_संदेश_update;
	संरचना mosquitto__callback *persist_base_संदेश_add;
	संरचना mosquitto__callback *persist_base_संदेश_delete;
	संरचना mosquitto__callback *persist_retain_संदेश_set;
	संरचना mosquitto__callback *persist_retain_संदेश_delete;
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
	अक्षर *psk_file;
	mosquitto_plugin_id_t **plugins;
	पूर्णांक plugin_मोजणी;
	int8_t allow_anonymous;
	bool allow_zero_length_clientid;
	अक्षर *auto_id_prefix;
	uint16_t auto_id_prefix_len;
	संरचना plugin__callbacks plugin_callbacks;
	mosquitto_plugin_id_t *pid; /* For registering as a "plugin" */
};

#जर defined(WITH_EPOLL) || defined(WITH_KQUEUE)
गणना struct_ident {
	id_invalid = 0,
	id_listener = 1,
	id_client = 2,
	id_listener_ws = 3,
};
#endif

संरचना mosquitto__listener {
	uint16_t port;
	अक्षर *host;
	अक्षर *bind_interface;
	पूर्णांक max_connections;
	अक्षर *mount_point;
	mosq_sock_t *socks;
	पूर्णांक sock_मोजणी;
	पूर्णांक client_मोजणी;
	गणना mosquitto_protocol protocol;
	पूर्णांक socket_domain;
	bool use_username_as_clientid;
	uint8_t max_qos;
	uint16_t max_topic_alias;
	uint16_t max_topic_alias_broker;
#ifdef WITH_TLS
	अक्षर *cafile;
	अक्षर *capath;
	अक्षर *certfile;
	अक्षर *keyfile;
	अक्षर *tls_engine;
	अक्षर *tls_engine_kpass_sha1;
	अक्षर *ciphers;
	अक्षर *ciphers_tls13;
	अक्षर *psk_hint;
	SSL_CTX *ssl_ctx;
	अक्षर *crlfile;
	अक्षर *tls_version;
	bool use_identity_as_username;
	bool use_subject_as_username;
	bool require_certificate;
	bool disable_client_cert_date_checks;
	गणना mosquitto__keyform tls_keyform;
#endif
#ifdef WITH_WEBSOCKETS
#  जर WITH_WEBSOCKETS == WS_IS_LWS
	संरचना lws_context *ws_context;
	bool ws_in_init;
	संरचना lws_protocols *ws_protocol;
#  endif
	अक्षर **ws_origins;
	पूर्णांक ws_origin_मोजणी;
#endif
#जर defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
	अक्षर *http_dir;
#endif
	संरचना mosquitto__security_options *security_options;
#ifdef WITH_UNIX_SOCKETS
	अक्षर *unix_socket_path;
#endif
	bool disable_protocol_v3;
	bool disable_protocol_v4;
	bool disable_protocol_v5;
	पूर्णांक enable_proxy_protocol;
	bool proxy_protocol_v2_require_tls;
#ifdef WITH_HTTP_API
	संरचना MHD_Daemon *mhd;
#endif
};


संरचना mosquitto__listener_sock {
#जर defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	/* This *must* be the first element in the struct. */
	पूर्णांक ident;
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
	अक्षर *plugin_name;
	अक्षर *plugin_version;
	संरचना control_endpoint *control_endpoints;
	संरचना plugin_own_callback *own_callbacks;
	संरचना timespec next_tick;
};

संरचना mosquitto__config {
	bool allow_duplicate_messages;
	पूर्णांक autosave_interval;
	bool autosave_on_changes;
	bool check_retain_source;
	अक्षर *clientid_prefixes;
	bool connection_messages;
	uint16_t cmd_port[CMD_PORT_LIMIT];
	पूर्णांक cmd_port_मोजणी;
	bool daemon;
	bool test_configuration;
	bool enable_control_api;
	पूर्णांक global_max_clients;
	पूर्णांक global_max_connections;
	संरचना mosquitto__listener *default_listener; /* Points to one of `listeners` */
	संरचना mosquitto__listener *listeners;
	पूर्णांक listener_मोजणी;
	bool local_only;
	अचिन्हित पूर्णांक log_dest;
	पूर्णांक log_facility;
	अचिन्हित पूर्णांक log_type;
	bool log_timestamp;
	अक्षर *log_timestamp_format;
	अक्षर *log_file;
	खाता *log_fptr;
	माप_प्रकार max_inflight_bytes;
	माप_प्रकार max_queued_bytes;
	पूर्णांक max_queued_messages;
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
	अक्षर *persistence_location;
	अक्षर *persistence_file;
	अक्षर *persistence_filepath;
	समय_प्रकार persistent_client_expiration;
	अक्षर *pid_file;
	bool queue_qos0_messages;
	bool per_listener_settings;
	bool retain_available;
	पूर्णांक retain_expiry_interval;
	bool set_tcp_nodelay;
	पूर्णांक sys_interval;
	bool upgrade_outgoing_qos;
	अक्षर *user;
#जर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	पूर्णांक websockets_log_level;
#endif
	uint16_t packet_buffer_size;
#ifdef WITH_BRIDGE
	संरचना mosquitto__bridge **bridges;
	पूर्णांक bridge_मोजणी;
#endif
	संरचना mosquitto__security_options security_options;
};


संरचना mosquitto__subshared {
	UT_hash_handle hh;
	संरचना mosquitto__subleaf *subs;
	अक्षर name[];
};

संरचना mosquitto__subhier {
	UT_hash_handle hh;
	संरचना mosquitto__subhier *parent;
	संरचना mosquitto__subhier *children;
	संरचना mosquitto__subleaf *subs;
	संरचना mosquitto__subshared *shared;
	uint16_t topic_len;
	अक्षर topic[];
};

संरचना mosquitto__subleaf {
	संरचना mosquitto__subleaf *prev;
	संरचना mosquitto__subleaf *next;
	संरचना mosquitto *context;
	संरचना mosquitto__subhier *hier;
	संरचना mosquitto__subshared *shared;
	uint32_t identifier;
	uint8_t subscription_options;
	अक्षर topic_filter[];
};

संरचना sub__token {
	संरचना sub__token *next;
	uint16_t topic_len;
	अक्षर topic[];
};

संरचना mosquitto__retainhier {
	UT_hash_handle hh;
	संरचना mosquitto__retainhier *parent;
	संरचना mosquitto__retainhier *children;
	संरचना mosquitto_base_संदेश *retained;
	uint16_t topic_len;
	अक्षर topic[];
};

संरचना mosquitto_base_संदेश {
	UT_hash_handle hh;
	संरचना mosquitto_base_संदेश_2 data;
	संरचना mosquitto__listener *source_listener;
	अक्षर **dest_ids;
	पूर्णांक dest_id_मोजणी;
	पूर्णांक ref_मोजणी;
	गणना mosquitto_संदेश_origin origin;
	bool stored;
};

संरचना mosquitto_client_संदेश {
	संरचना mosquitto_client_संदेश_2 data;
	संरचना mosquitto_client_संदेश *prev;
	संरचना mosquitto_client_संदेश *next;
	संरचना mosquitto_base_संदेश *base_संदेश;
};


संरचना mosquitto__psk {
	UT_hash_handle hh;
	अक्षर *username;
	अक्षर *password;
};

संरचना mosquitto__message_v5 {
	संरचना mosquitto__message_v5 *next, *prev;
	अक्षर *topic;
	शून्य *payload;
	mosquitto_property *properties;
	/* clientid is used only by mosquitto_broker_publish*() to indicate
	   this message is for a specific client. */
	अक्षर *clientid;
	पूर्णांक payloadlen;
	पूर्णांक qos;
	bool retain;
};

संरचना mosquitto_db {
	dbid_t last_db_id;
	uint64_t गाठ_id_shifted;
	संरचना mosquitto__subhier *normal_subs;
	संरचना mosquitto__subhier *shared_subs;
	संरचना mosquitto__retainhier *retains;
	संरचना mosquitto *contexts_by_id;
	संरचना mosquitto *contexts_by_sock;
	संरचना mosquitto *contexts_by_id_delayed_auth;
	संरचना mosquitto *contexts_for_free;
	mosquitto_plugin_id_t **plugins;
	पूर्णांक plugin_मोजणी;
#ifdef WITH_BRIDGE
	संरचना mosquitto **bridges;
	पूर्णांक bridge_मोजणी;
#endif
	संरचना clientid__index_hash *clientid_index_hash;
	संरचना mosquitto_base_संदेश *संदेश_store;
	समय_प्रकार now_s; /* Monotonic clock, where possible */
	समय_प्रकार now_real_s; /* Read clock, for measuring session/message expiry */
	uint64_t गाठ_id; /* for unique db ids */
	समय_प्रकार next_event_ms; /* for mux timeout */
	पूर्णांक संदेश_store_मोजणी;
	अचिन्हित दीर्घ संदेश_store_bytes;
	अक्षर *config_file;
	संरचना mosquitto__config *config;
	bool quiet;
	bool verbose;
#ifdef WITH_SYS_वृक्ष
	पूर्णांक subscription_मोजणी;
	पूर्णांक shared_subscription_मोजणी;
	पूर्णांक retained_मोजणी;
#endif
	पूर्णांक persistence_changes;
	संरचना mosquitto *ll_for_free;
#ifdef WITH_EPOLL
	पूर्णांक epollfd;
#endif
#ifdef WITH_KQUEUE
	पूर्णांक kqueuefd;
#endif
	संरचना mosquitto__message_v5 *plugin_msgs;
#ifdef WITH_TLS
	/* tls_keylog can't be in the config struct because it is used
	   before the config is allocated. Config probably
	   shouldn't be separately allocated. */
	अक्षर *tls_keylog;
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
	अक्षर *topic;
	अक्षर *local_prefix;
	अक्षर *remote_prefix;
	अक्षर *local_topic; /* topic prefixed with local_prefix */
	अक्षर *remote_topic; /* topic prefixed with remote_prefix */
	गणना mosquitto__bridge_direction direction;
	uint8_t qos;
};

संरचना bridge_address {
	अक्षर *address;
	uint16_t port;
};

संरचना mosquitto__bridge {
	अक्षर *name;
	संरचना bridge_address *addresses;
	पूर्णांक cur_address;
	पूर्णांक address_मोजणी;
	समय_प्रकार primary_retry;
	mosq_sock_t primary_retry_sock;
	bool round_robin;
	bool try_private;
	bool try_private_accepted;
	bool clean_start;
	int8_t clean_start_local;
	uint16_t keepalive;
	अचिन्हित पूर्णांक tcp_keepalive_idle;
	अचिन्हित पूर्णांक tcp_keepalive_interval;
	अचिन्हित पूर्णांक tcp_keepalive_counter;
#ifdef WITH_TCP_USER_TIMEOUT
	पूर्णांक tcp_user_timeout;
#endif
	संरचना mosquitto__bridge_topic *topics;
	पूर्णांक topic_मोजणी;
	bool topic_remapping;
	गणना mosquitto__protocol protocol_version;
	समय_प्रकार restart_t;
	समय_प्रकार connected_at;
	अक्षर *remote_clientid;
	अक्षर *remote_username;
	अक्षर *remote_password;
	अक्षर *local_clientid;
	अक्षर *local_username;
	अक्षर *local_password;
	अक्षर *notification_topic;
	अक्षर *bind_address;
	bool notifications;
	bool notifications_local_only;
	गणना mosquitto_bridge_start_type start_type;
	पूर्णांक idle_timeout;
	पूर्णांक restart_timeout;
	पूर्णांक backoff_base;
	पूर्णांक backoff_cap;
	पूर्णांक stable_connection_period;
	पूर्णांक threshold;
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
	अक्षर *tls_cafile;
	अक्षर *tls_capath;
	अक्षर *tls_certfile;
	अक्षर *tls_keyfile;
	अक्षर *tls_version;
	अक्षर *tls_alpn;
	अक्षर *tls_ciphers;
	अक्षर *tls_13_ciphers;
#  ifdef FINAL_WITH_TLS_PSK
	अक्षर *tls_psk_identity;
	अक्षर *tls_psk;
#  endif
#endif
};

#जर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
संरचना libws_mqtt_hack {
	अक्षर *http_dir;
	संरचना mosquitto__listener *listener;
};

संरचना libws_mqtt_data {
	संरचना mosquitto *mosq;
};
#endif

#अन्तर्भाव <net_mosq.h>


संरचना control_endpoint {
	संरचना control_endpoint *next, *prev;
	अक्षर topic[];
};

बाह्य संरचना mosquitto_db db;

/* ============================================================
 * Main functions
 * ============================================================ */
पूर्णांक mosquitto_main_loop(संरचना mosquitto__listener_sock *listensock, पूर्णांक listensock_मोजणी);
शून्य loop__update_next_event(समय_प्रकार new_ms);

/* ============================================================
 * Config functions
 * ============================================================ */
/* Initialise config struct to default values. */
शून्य config__init(संरचना mosquitto__config *config);
#ifdef WITH_BRIDGE
शून्य config__bridge_cleanup(संरचना mosquitto__bridge *bridge);
#endif
/* Parse command line options into config. */
पूर्णांक config__parse_args(संरचना mosquitto__config *config, पूर्णांक argc, अक्षर *argv[]);
/* Read configuration data from config->config_file into config.
 * If reload is true, don't process config options that shouldn't be reloaded (listeners etc)
 * Returns 0 on success, 1 if there is a configuration error or if a file cannot be opened.
 */
पूर्णांक config__read(संरचना mosquitto__config *config, bool reload);
/* Free all config data. */
शून्य config__cleanup(संरचना mosquitto__config *config);
पूर्णांक config__get_dir_files(स्थिरांक अक्षर *include_dir, अक्षर ***files, पूर्णांक *file_मोजणी);

/* ============================================================
 * Server send functions
 * ============================================================ */
पूर्णांक send__connack(संरचना mosquitto *context, uint8_t ack, uint8_t reason_code, स्थिरांक mosquitto_property *properties);
पूर्णांक send__suback(संरचना mosquitto *context, uint16_t mid, uint32_t payloadlen, स्थिरांक शून्य *payload);
पूर्णांक send__unsuback(संरचना mosquitto *context, uint16_t mid, पूर्णांक reason_code_मोजणी, uint8_t *reason_codes, स्थिरांक mosquitto_property *properties);
पूर्णांक send__auth(संरचना mosquitto *context, uint8_t reason_code, स्थिरांक शून्य *auth_data, uint16_t auth_data_len);

/* ============================================================
 * Network functions
 * ============================================================ */
शून्य net__broker_init(शून्य);
शून्य net__broker_cleanup(शून्य);
संरचना mosquitto *net__socket_accept(संरचना mosquitto__listener_sock *listensock);
पूर्णांक net__socket_listen(संरचना mosquitto__listener *listener);
पूर्णांक net__socket_get_address(mosq_sock_t sock, अक्षर *buf, माप_प्रकार len, uint16_t *remote_address);
पूर्णांक net__tls_load_verify(संरचना mosquitto__listener *listener);
पूर्णांक net__tls_server_ctx(संरचना mosquitto__listener *listener);
पूर्णांक net__load_certificates(संरचना mosquitto__listener *listener);

/* ============================================================
 * Read handling functions
 * ============================================================ */
पूर्णांक handle__packet(संरचना mosquitto *context);
पूर्णांक handle__connack(संरचना mosquitto *context);
पूर्णांक handle__connect(संरचना mosquitto *context);
पूर्णांक handle__disconnect(संरचना mosquitto *context);
पूर्णांक handle__publish(संरचना mosquitto *context);
पूर्णांक handle__subscribe(संरचना mosquitto *context);
पूर्णांक handle__unsubscribe(संरचना mosquitto *context);
पूर्णांक handle__auth(संरचना mosquitto *context);

/* ============================================================
 * Database handling
 * ============================================================ */
पूर्णांक db__open(संरचना mosquitto__config *config);
पूर्णांक db__close(शून्य);
#ifdef WITH_PERSISTENCE
पूर्णांक persist__backup(bool shutdown);
पूर्णांक persist__restore(शून्य);
#endif
/* Return the number of in-flight messages in count. */
पूर्णांक db_message_मोजणी(पूर्णांक *मोजणी);
पूर्णांक db__message_delete_outgoing(संरचना mosquitto *context, uint16_t mid, गणना mosquitto_संदेश_state expect_state, पूर्णांक qos);
पूर्णांक db_message_घाला_outgoing(संरचना mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, संरचना mosquitto_base_संदेश *base_संदेश, uint32_t subscription_identifier, bool update, bool persist);
पूर्णांक db_message_घाला_incoming(संरचना mosquitto *context, uint64_t cmsg_id, संरचना mosquitto_base_संदेश *base_संदेश, bool persist);
पूर्णांक db__message_remove_incoming(संरचना mosquitto *context, uint16_t mid);
पूर्णांक db__message_release_incoming(संरचना mosquitto *context, uint16_t mid);
पूर्णांक db__message_update_outgoing(संरचना mosquitto *context, uint16_t mid, गणना mosquitto_संदेश_state state, पूर्णांक qos, bool persist);
शून्य db__message_dequeue_first(संरचना mosquitto *context, संरचना mosquitto_संदेश_data *संदेश_data);
पूर्णांक db__messages_delete(संरचना mosquitto *context, bool force_free);
पूर्णांक db__messages_delete_incoming(संरचना mosquitto *context);
पूर्णांक db__messages_delete_outgoing(संरचना mosquitto *context);
पूर्णांक db__messages_easy_queue(संरचना mosquitto *context, स्थिरांक अक्षर *topic, uint8_t qos, uint32_t payloadlen, स्थिरांक शून्य *payload, पूर्णांक retain, uint32_t message_expiry_interval, mosquitto_property **properties);
पूर्णांक db__message_store(स्थिरांक संरचना mosquitto *source, संरचना mosquitto_base_संदेश *base_संदेश, uint32_t *message_expiry_interval, गणना mosquitto_संदेश_origin origin);
पूर्णांक db__message_store_find(संरचना mosquitto *context, uint16_t mid, संरचना mosquitto_client_संदेश **client_संदेश);
पूर्णांक db_संदेश_store_add(संरचना mosquitto_base_संदेश *base_संदेश);
शून्य db_संदेश_store_remove(संरचना mosquitto_base_संदेश *base_संदेश, bool notify);
शून्य db_संदेश_store_ref_inc(संरचना mosquitto_base_संदेश *base_संदेश);
शून्य db_संदेश_store_ref_dec(संरचना mosquitto_base_संदेश **base_संदेश);
शून्य db_संदेश_store_clean(शून्य);
शून्य db_संदेश_store_compact(शून्य);
शून्य db_संदेश_store_free(संरचना mosquitto_base_संदेश *base_संदेश);
पूर्णांक db__message_reconnect_reset(संरचना mosquitto *context);
bool db__ready_for_flight(संरचना mosquitto *context, गणना mosquitto_संदेश_direction dir, पूर्णांक qos);
bool db__ready_for_queue(संरचना mosquitto *context, पूर्णांक qos, संरचना mosquitto_संदेश_data *संदेश_data);
शून्य sys_वृक्ष_init(शून्य);
शून्य sys_वृक्ष_update(bool force);
पूर्णांक db__message_write_inflight_out_all(संरचना mosquitto *context);
पूर्णांक db__message_write_inflight_out_latest(संरचना mosquitto *context);
पूर्णांक db__message_write_queued_out(संरचना mosquitto *context);
पूर्णांक db__message_write_queued_in(संरचना mosquitto *context);
शून्य db_संदेश_add_to_inflight_stats(संरचना mosquitto_संदेश_data *संदेश_data, संरचना mosquitto_client_संदेश *संदेश);
शून्य db_संदेश_add_to_queued_stats(संरचना mosquitto_संदेश_data *संदेश_data, संरचना mosquitto_client_संदेश *संदेश);
uint64_t db_new_संदेश_id(शून्य);
शून्य db__expire_all_messages(संरचना mosquitto *context);
शून्य db__check_acl_of_all_messages(संरचना mosquitto *context);

/* ============================================================
 * Subscription functions
 * ============================================================ */
पूर्णांक sub__init(शून्य);
पूर्णांक sub__add(संरचना mosquitto *context, स्थिरांक संरचना mosquitto_subscription *sub);
पूर्णांक sub__remove(संरचना mosquitto *context, स्थिरांक अक्षर *sub, uint8_t *reason);
शून्य sub_वृक्ष_print(संरचना mosquitto__subhier *मूळ, पूर्णांक level);
पूर्णांक sub__clean_session(संरचना mosquitto *context);
पूर्णांक sub__messages_queue(स्थिरांक अक्षर *source_id, uint8_t qos, संरचना mosquitto_base_संदेश **base_संदेश);
पूर्णांक sub__topic_tokenise(स्थिरांक अक्षर *subtopic, अक्षर **local_sub, अक्षर ***topics, स्थिरांक अक्षर **sharename);
शून्य sub__topic_tokens_free(संरचना sub__token *tokens);

/* ============================================================
 * Context functions
 * ============================================================ */
संरचना mosquitto *context__init(शून्य);
पूर्णांक context__init_sock(संरचना mosquitto *context, mosq_sock_t sock, bool get_address);
शून्य context__cleanup(संरचना mosquitto *context, bool force_free);
शून्य context__disconnect(संरचना mosquitto *context, पूर्णांक reason);
शून्य context__add_to_disused(संरचना mosquitto *context);
शून्य context__free_disused(शून्य);
शून्य context__send_will(संरचना mosquitto *context);
शून्य context__add_to_by_id(संरचना mosquitto *context);
शून्य context__remove_from_by_id(संरचना mosquitto *context);

पूर्णांक connect__on_authorised(संरचना mosquitto *context, शून्य *auth_data_out, uint16_t auth_data_out_len);


/* ============================================================
 * Control functions
 * ============================================================ */
#ifdef WITH_CONTROL
पूर्णांक control__process(संरचना mosquitto *context, संरचना mosquitto_base_संदेश *base_संदेश);
शून्य control__cleanup(शून्य);
#endif
पूर्णांक control__register_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, स्थिरांक अक्षर *topic, शून्य *userdata);
पूर्णांक control__unregister_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, स्थिरांक अक्षर *topic);
शून्य control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier);


/* ============================================================
 * Logging functions
 * ============================================================ */
पूर्णांक log__init(संरचना mosquitto__config *config);
पूर्णांक log__close(संरचना mosquitto__config *config);
शून्य log__internal(स्थिरांक अक्षर *fmt, ...) __attribute__((format(म_लिखो, 1, 2)));

/* ============================================================
 * Bridge functions
 * ============================================================ */
#ifdef WITH_BRIDGE
शून्य bridge__start_all(शून्य);
शून्य bridge__reload(शून्य);
शून्य bridge__db_cleanup(शून्य);
शून्य bridge__cleanup(संरचना mosquitto *context);
पूर्णांक bridge__connect(संरचना mosquitto *context);
#जर defined(__GLIBC__) && defined(WITH_ADNS)
पूर्णांक bridge__connect_step3(संरचना mosquitto *context);
#endif
पूर्णांक bridge__on_connect(संरचना mosquitto *context);
शून्य bridge_check(शून्य);
पूर्णांक bridge__register_local_connections(शून्य);
पूर्णांक bridge__add_topic(संरचना mosquitto__bridge *bridge, स्थिरांक अक्षर *topic, गणना mosquitto__bridge_direction direction, uint8_t qos, स्थिरांक अक्षर *local_prefix, स्थिरांक अक्षर *remote_prefix);
शून्य bridge__cleanup_topics(संरचना mosquitto__bridge *bridge);
पूर्णांक bridge__remap_topic_in(संरचना mosquitto *context, अक्षर **topic);
#endif

/* ============================================================
 * IO multiplex related functions
 * ============================================================ */
पूर्णांक mux__init(शून्य);
पूर्णांक mux__add_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णांक listensock_मोजणी);
पूर्णांक mux__delete_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णांक listensock_मोजणी);
पूर्णांक mux__loop_prepare(शून्य);
पूर्णांक mux__new(संरचना mosquitto *context);
पूर्णांक mux__add_out(संरचना mosquitto *context);
पूर्णांक mux__remove_out(संरचना mosquitto *context);
पूर्णांक mux__delete(संरचना mosquitto *context);
पूर्णांक mux__wait(शून्य);
पूर्णांक mux__handle(संरचना mosquitto__listener_sock *listensock, पूर्णांक listensock_मोजणी);
पूर्णांक mux__cleanup(शून्य);

/* ============================================================
 * Listener related functions
 * ============================================================ */
बाह्य संरचना mosquitto__listener_sock *g_listensock;
बाह्य पूर्णांक g_listensock_मोजणी;

शून्य listener__set_defaults(संरचना mosquitto__listener *listener);
शून्य listeners__reload_all_certificates(शून्य);
#जर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
शून्य listeners__add_websockets(संरचना lws_context *ws_context, mosq_sock_t fd);
#endif
पूर्णांक listeners__start(शून्य);
शून्य listeners__stop(शून्य);

/* ============================================================
 * Plugin related functions
 * ============================================================ */
पूर्णांक plugin__load_v5(mosquitto_plugin_id_t *plugin, शून्य *lib);
पूर्णांक plugin__load_v4(mosquitto_plugin_id_t *plugin, शून्य *lib);
पूर्णांक plugin__load_v3(mosquitto_plugin_id_t *plugin, शून्य *lib);
पूर्णांक plugin__load_v2(mosquitto_plugin_id_t *plugin, शून्य *lib);
पूर्णांक acl__pre_check(mosquitto_plugin_id_t *plugin, संरचना mosquitto *context, पूर्णांक access);

शून्य LIB_ERROR(शून्य);
शून्य plugin__handle_connect(संरचना mosquitto *context);
शून्य plugin__handle_disconnect(संरचना mosquitto *context, पूर्णांक reason);
शून्य plugin__handle_client_offline(संरचना mosquitto *context, पूर्णांक reason);
पूर्णांक plugin__handle_message_in(संरचना mosquitto *context, संरचना mosquitto_base_संदेश_2 *base_संदेश);
पूर्णांक plugin__handle_message_out(संरचना mosquitto *context, संरचना mosquitto_base_संदेश_2 *base_संदेश);
पूर्णांक plugin__handle_subscribe(संरचना mosquitto *context, संरचना mosquitto_subscription *sub);
पूर्णांक plugin__handle_unsubscribe(संरचना mosquitto *context, संरचना mosquitto_subscription *sub);
पूर्णांक plugin__handle_reload(शून्य);
शून्य plugin__handle_tick(शून्य);
पूर्णांक plugin__callback_unregister_all(mosquitto_plugin_id_t *identifier);
शून्य plugin_persist__handle_restore(शून्य);
शून्य plugin_persist__handle_client_add(संरचना mosquitto *context);
शून्य plugin_persist__handle_client_delete(संरचना mosquitto *context);
शून्य plugin_persist__handle_client_update(संरचना mosquitto *context);
शून्य plugin_persist__handle_subscription_add(संरचना mosquitto *context, स्थिरांक संरचना mosquitto_subscription *sub);
शून्य plugin_persist__handle_subscription_delete(संरचना mosquitto *context, अक्षर *sub);
शून्य plugin_persist_handle_client_संदेश_add(संरचना mosquitto *context, स्थिरांक संरचना mosquitto_client_संदेश *cmsg);
शून्य plugin_persist_handle_client_संदेश_delete(संरचना mosquitto *context, स्थिरांक संरचना mosquitto_client_संदेश *cmsg);
शून्य plugin_persist_handle_client_संदेश_update(संरचना mosquitto *context, स्थिरांक संरचना mosquitto_client_संदेश *cmsg);
शून्य plugin_persist_handle_base_संदेश_add(संरचना mosquitto_base_संदेश *base_संदेश);
शून्य plugin_persist_handle_base_संदेश_delete(संरचना mosquitto_base_संदेश *base_संदेश);
शून्य plugin_persist_handle_retain_संदेश_set(संरचना mosquitto_base_संदेश *base_संदेश);
शून्य plugin_persist_handle_retain_संदेश_delete(संरचना mosquitto_base_संदेश *base_संदेश);
शून्य plugin_persist__handle_will_add(संरचना mosquitto *context);
शून्य plugin_persist__handle_will_delete(संरचना mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
पूर्णांक keepalive__init(शून्य);
शून्य keepalive__cleanup(शून्य);
पूर्णांक keepalive__add(संरचना mosquitto *context);
शून्य keepalive__check(शून्य);
पूर्णांक keepalive__remove(संरचना mosquitto *context);
पूर्णांक keepalive__update(संरचना mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
पूर्णांक property__process_connect(संरचना mosquitto *context, mosquitto_property **props);
पूर्णांक property__process_will(संरचना mosquitto *context, संरचना mosquitto_message_all *संदेश, mosquitto_property **props);
पूर्णांक property__process_publish(संरचना mosquitto_base_संदेश *base_संदेश, mosquitto_property **props, पूर्णांक *topic_alias, uint32_t *message_expiry_interval, bool is_bridge);
पूर्णांक property__process_disconnect(संरचना mosquitto *context, mosquitto_property **props);

/* ============================================================
 * Retain tree related functions
 * ============================================================ */
पूर्णांक retain__init(शून्य);
शून्य retain__clean(संरचना mosquitto__retainhier **retainhier);
पूर्णांक retain__queue(संरचना mosquitto *context, स्थिरांक संरचना mosquitto_subscription *sub);
पूर्णांक retain__store(स्थिरांक अक्षर *topic, संरचना mosquitto_base_संदेश *base_संदेश, अक्षर **split_topics, bool persist);
शून्य retain__expiry_check(शून्य);
शून्य retain__expire(संरचना mosquitto__retainhier **retainhier);

/* ============================================================
 * Security related functions
 * ============================================================ */
पूर्णांक acl__find_acls(संरचना mosquitto *context);
पूर्णांक plugin__load_all(शून्य);
पूर्णांक plugin__unload_all(शून्य);
पूर्णांक config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, संरचना mosquitto__security_options *security_options);

पूर्णांक mosquitto_security_init(bool reload);
पूर्णांक mosquitto_security_cleanup(bool reload);
पूर्णांक mosquitto_acl_check(संरचना mosquitto *context, स्थिरांक अक्षर *topic, uint32_t payloadlen, शून्य *payload, uint8_t qos, bool retain, mosquitto_property *properties, पूर्णांक access);
पूर्णांक mosquitto_basic_auth(संरचना mosquitto *context);
पूर्णांक mosquitto_psk_किल्ली_get(संरचना mosquitto *context, स्थिरांक अक्षर *hint, स्थिरांक अक्षर *identity, अक्षर *किल्ली, पूर्णांक max_किल्ली_len);

पूर्णांक mosquitto_security_init_default(शून्य);
पूर्णांक mosquitto_security_apply_default(शून्य);
पूर्णांक mosquitto_security_cleanup_default(शून्य);
पूर्णांक mosquitto_psk_किल्ली_get_default(संरचना mosquitto *context, स्थिरांक अक्षर *hint, स्थिरांक अक्षर *identity, अक्षर *किल्ली, पूर्णांक max_किल्ली_len);
पूर्णांक broker_acl_file__init(शून्य);
शून्य broker_acl_file__cleanup(शून्य);
पूर्णांक broker_password_file__init(शून्य);
शून्य broker_password_file__cleanup(शून्य);
पूर्णांक psk_file__init(शून्य);
पूर्णांक psk_file__cleanup(शून्य);

पूर्णांक mosquitto_security_auth_start(संरचना mosquitto *context, bool reauth, स्थिरांक शून्य *data_in, uint16_t data_in_len, शून्य **data_out, uint16_t *data_out_len);
पूर्णांक mosquitto_security_auth_continue(संरचना mosquitto *context, स्थिरांक शून्य *data_in, uint16_t data_len, शून्य **data_out, uint16_t *data_out_len);

शून्य unpwd__free_item(संरचना mosquitto__unpwd **unpwd, संरचना mosquitto__unpwd *item);

/* ============================================================
 * Session expiry
 * ============================================================ */
पूर्णांक session_expiry__add(संरचना mosquitto *context);
पूर्णांक session_expiry__add_from_persistence(संरचना mosquitto *context, समय_प्रकार expiry_time);
शून्य session_expiry__add_on_shutdown(शून्य);
शून्य session_expiry__remove(संरचना mosquitto *context);
शून्य session_expiry__remove_all(शून्य);
शून्य session_expiry__check(शून्य);
शून्य session_expiry__send_all(शून्य);

/* ============================================================
 * Signals
 * ============================================================ */
शून्य signal__setup(शून्य);
पूर्णांक signal__flag_check(शून्य);

/* ============================================================
 * Window service and signal related functions
 * ============================================================ */
#जर defined(WIN32) || defined(__CYGWIN__)
शून्य service_install(अक्षर *name);
शून्य service_uninstall(अक्षर *name);
शून्य service_run(अक्षर *name);

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
#  जर WITH_WEBSOCKETS == WS_IS_LWS
शून्य mosq_websockets_init(संरचना mosquitto__listener *listener, स्थिरांक संरचना mosquitto__config *conf);
#  endif
पूर्णांक http__context_init(संरचना mosquitto *context);
पूर्णांक http__context_cleanup(संरचना mosquitto *context);
पूर्णांक http__read(संरचना mosquitto *context);
#endif
शून्य do_disconnect(संरचना mosquitto *context, पूर्णांक reason);

/* ============================================================
 * PROXY related functions
 * ============================================================ */
पूर्णांक proxy_v2__read(संरचना mosquitto *context);
पूर्णांक proxy_v1__read(संरचना mosquitto *context);

/* ============================================================
 * Will delay
 * ============================================================ */
पूर्णांक will_delay__add(संरचना mosquitto *context);
शून्य will_delay__check(शून्य);
शून्य will_delay__send_all(शून्य);
शून्य will_delay__remove(संरचना mosquitto *mosq);

/* ============================================================
 * HTTP Info
 * ============================================================ */
पूर्णांक http_api__start_local(संरचना mosquitto__listener *listener);
पूर्णांक http_api__start(संरचना mosquitto__listener *listener);
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
