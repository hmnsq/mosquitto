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

#अगर_अघोषित MOSQUITTO_BROKER_INTERNAL_H
#घोषणा MOSQUITTO_BROKER_INTERNAL_H

#समावेश "config.h"
#समावेश <stdio.h>

#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#  अगर LWS_LIBRARY_VERSION_संख्या >= 3002000 && !defined(LWS_WITH_EXTERNAL_POLL)
#    warning "libwebsockets is not compiled with LWS_WITH_EXTERNAL_POLL support. Websocket performance will be unusable."
#  endif
#पूर्ण_अगर

#अगर_घोषित WITH_HTTP_API
#  include <microhttpd.h>
#पूर्ण_अगर

#अगर_घोषित __linux__
#घोषणा WITH_TCP_USER_TIMEOUT
#पूर्ण_अगर

#समावेश "mosquitto_internal.h"
#समावेश "mosquitto/broker.h"
#समावेश "mosquitto/broker_plugin.h"
#समावेश "mosquitto.h"
#समावेश "logging_mosq.h"
#समावेश "tls_mosq.h"
#समावेश "uthash.h"
#समावेश "acl_file.h"
#समावेश "password_file.h"

#अगर_अघोषित __GNUC__
#घोषणा __attribute__(attrib)
#पूर्ण_अगर

/* Log destinations */
#घोषणा MQTT3_LOG_NONE 0x00
#घोषणा MQTT3_LOG_SYSLOG 0x01
#घोषणा MQTT3_LOG_FILE 0x02
#घोषणा MQTT3_LOG_STDOUT 0x04
#घोषणा MQTT3_LOG_STDERR 0x08
#घोषणा MQTT3_LOG_TOPIC 0x10
#घोषणा MQTT3_LOG_DLT 0x20
#अगर_घोषित ANDROID
#घोषणा MQTT3_LOG_ANDROID 0x40
#पूर्ण_अगर
#घोषणा MQTT3_LOG_ALL 0xFF

#घोषणा CMD_PORT_LIMIT 10

प्रकार uint64_t dbid_t;

प्रकार पूर्णांक (*FUNC_plugin_init_v5)(mosquitto_plugin_id_t *, व्योम **, काष्ठा mosquitto_opt *, पूर्णांक);
प्रकार पूर्णांक (*FUNC_plugin_cleanup_v5)(व्योम *, काष्ठा mosquitto_opt *, पूर्णांक);

प्रकार पूर्णांक (*FUNC_auth_plugin_init_v4)(व्योम **, काष्ठा mosquitto_opt *, पूर्णांक);
प्रकार पूर्णांक (*FUNC_auth_plugin_cleanup_v4)(व्योम *, काष्ठा mosquitto_opt *, पूर्णांक);
प्रकार पूर्णांक (*FUNC_auth_plugin_security_init_v4)(व्योम *, काष्ठा mosquitto_opt *, पूर्णांक, bool);
प्रकार पूर्णांक (*FUNC_auth_plugin_security_cleanup_v4)(व्योम *, काष्ठा mosquitto_opt *, पूर्णांक, bool);
प्रकार पूर्णांक (*FUNC_auth_plugin_acl_check_v4)(व्योम *, पूर्णांक, काष्ठा mosquitto *, काष्ठा mosquitto_acl_संदेश *);
प्रकार पूर्णांक (*FUNC_auth_plugin_unpwd_check_v4)(व्योम *, काष्ठा mosquitto *, स्थिर अक्षर *, स्थिर अक्षर *);
प्रकार पूर्णांक (*FUNC_auth_plugin_psk_कुंजी_get_v4)(व्योम *, काष्ठा mosquitto *, स्थिर अक्षर *, स्थिर अक्षर *, अक्षर *, पूर्णांक);
प्रकार पूर्णांक (*FUNC_auth_plugin_auth_start_v4)(व्योम *, काष्ठा mosquitto *, स्थिर अक्षर *, bool, स्थिर व्योम *, uint16_t, व्योम **, uint16_t *);
प्रकार पूर्णांक (*FUNC_auth_plugin_auth_continue_v4)(व्योम *, काष्ठा mosquitto *, स्थिर अक्षर *, स्थिर व्योम *, uint16_t, व्योम **, uint16_t *);

प्रकार पूर्णांक (*FUNC_auth_plugin_init_v3)(व्योम **, काष्ठा mosquitto_opt *, पूर्णांक);
प्रकार पूर्णांक (*FUNC_auth_plugin_cleanup_v3)(व्योम *, काष्ठा mosquitto_opt *, पूर्णांक);
प्रकार पूर्णांक (*FUNC_auth_plugin_security_init_v3)(व्योम *, काष्ठा mosquitto_opt *, पूर्णांक, bool);
प्रकार पूर्णांक (*FUNC_auth_plugin_security_cleanup_v3)(व्योम *, काष्ठा mosquitto_opt *, पूर्णांक, bool);
प्रकार पूर्णांक (*FUNC_auth_plugin_acl_check_v3)(व्योम *, पूर्णांक, स्थिर काष्ठा mosquitto *, काष्ठा mosquitto_acl_संदेश *);
प्रकार पूर्णांक (*FUNC_auth_plugin_unpwd_check_v3)(व्योम *, स्थिर काष्ठा mosquitto *, स्थिर अक्षर *, स्थिर अक्षर *);
प्रकार पूर्णांक (*FUNC_auth_plugin_psk_कुंजी_get_v3)(व्योम *, स्थिर काष्ठा mosquitto *, स्थिर अक्षर *, स्थिर अक्षर *, अक्षर *, पूर्णांक);

प्रकार पूर्णांक (*FUNC_auth_plugin_init_v2)(व्योम **, काष्ठा mosquitto_auth_opt *, पूर्णांक);
प्रकार पूर्णांक (*FUNC_auth_plugin_cleanup_v2)(व्योम *, काष्ठा mosquitto_auth_opt *, पूर्णांक);
प्रकार पूर्णांक (*FUNC_auth_plugin_security_init_v2)(व्योम *, काष्ठा mosquitto_auth_opt *, पूर्णांक, bool);
प्रकार पूर्णांक (*FUNC_auth_plugin_security_cleanup_v2)(व्योम *, काष्ठा mosquitto_auth_opt *, पूर्णांक, bool);
प्रकार पूर्णांक (*FUNC_auth_plugin_acl_check_v2)(व्योम *, स्थिर अक्षर *, स्थिर अक्षर *, स्थिर अक्षर *, पूर्णांक);
प्रकार पूर्णांक (*FUNC_auth_plugin_unpwd_check_v2)(व्योम *, स्थिर अक्षर *, स्थिर अक्षर *);
प्रकार पूर्णांक (*FUNC_auth_plugin_psk_कुंजी_get_v2)(व्योम *, स्थिर अक्षर *, स्थिर अक्षर *, अक्षर *, पूर्णांक);


क्रमागत mosquitto_संदेश_origin {
	mosq_mo_client = 0,
	mosq_mo_broker = 1,
};

काष्ठा mosquitto__plugin_lib {
	व्योम *lib;
	व्योम *user_तथ्य;
	पूर्णांक (*plugin_version)(व्योम);
	काष्ठा mosquitto_plugin_id_t *identifier;

	FUNC_plugin_init_v5 plugin_init_v5;
	FUNC_plugin_cleanup_v5 plugin_cleanup_v5;

	FUNC_auth_plugin_init_v4 plugin_init_v4;
	FUNC_auth_plugin_cleanup_v4 plugin_cleanup_v4;
	FUNC_auth_plugin_security_init_v4 security_init_v4;
	FUNC_auth_plugin_security_cleanup_v4 security_cleanup_v4;
	FUNC_auth_plugin_acl_check_v4 acl_check_v4;
	FUNC_auth_plugin_unpwd_check_v4 unpwd_check_v4;
	FUNC_auth_plugin_psk_कुंजी_get_v4 psk_कुंजी_get_v4;
	FUNC_auth_plugin_auth_start_v4 auth_start_v4;
	FUNC_auth_plugin_auth_continue_v4 auth_continue_v4;

	FUNC_auth_plugin_init_v3 plugin_init_v3;
	FUNC_auth_plugin_cleanup_v3 plugin_cleanup_v3;
	FUNC_auth_plugin_security_init_v3 security_init_v3;
	FUNC_auth_plugin_security_cleanup_v3 security_cleanup_v3;
	FUNC_auth_plugin_acl_check_v3 acl_check_v3;
	FUNC_auth_plugin_unpwd_check_v3 unpwd_check_v3;
	FUNC_auth_plugin_psk_कुंजी_get_v3 psk_कुंजी_get_v3;

	FUNC_auth_plugin_init_v2 plugin_init_v2;
	FUNC_auth_plugin_cleanup_v2 plugin_cleanup_v2;
	FUNC_auth_plugin_security_init_v2 security_init_v2;
	FUNC_auth_plugin_security_cleanup_v2 security_cleanup_v2;
	FUNC_auth_plugin_acl_check_v2 acl_check_v2;
	FUNC_auth_plugin_unpwd_check_v2 unpwd_check_v2;
	FUNC_auth_plugin_psk_कुंजी_get_v2 psk_कुंजी_get_v2;
	पूर्णांक version;
};

काष्ठा mosquitto__plugin_config {
	अक्षर *path;
	अक्षर *name;
	काष्ठा mosquitto_opt *options;
	काष्ठा mosquitto__security_options **security_options;
	पूर्णांक option_गिनती;
	पूर्णांक security_option_गिनती;
	bool deny_special_chars;
};

काष्ठा mosquitto__callback {
	UT_hash_handle hh; /* For callbacks that register for e.g. a specific topic */
	काष्ठा mosquitto__callback *अग्र, *पूर्व; /* For typical callbacks */
	MOSQ_FUNC_generic_callback cb;
	व्योम *userdata;
	जोड़ {
		अक्षर *topic;
		काष्ठा timespec अग्र_tick;
	} तथ्य;
	mosquitto_plugin_id_t *identifier;
};

काष्ठा plugin__callbacks {
	काष्ठा mosquitto__callback *tick;
	काष्ठा mosquitto__callback *acl_check;
	काष्ठा mosquitto__callback *basic_auth;
	काष्ठा mosquitto__callback *connect;
	काष्ठा mosquitto__callback *control;
	काष्ठा mosquitto__callback *disconnect;
	काष्ठा mosquitto__callback *client_offline;
	काष्ठा mosquitto__callback *ext_auth_continue;
	काष्ठा mosquitto__callback *ext_auth_start;
	काष्ठा mosquitto__callback *message_in;
	काष्ठा mosquitto__callback *message_out;
	काष्ठा mosquitto__callback *psk_कुंजी;
	काष्ठा mosquitto__callback *reload;
	काष्ठा mosquitto__callback *subscribe;
	काष्ठा mosquitto__callback *unsubscribe;
	काष्ठा mosquitto__callback *persist_restore;
	काष्ठा mosquitto__callback *persist_client_add;
	काष्ठा mosquitto__callback *persist_client_delete;
	काष्ठा mosquitto__callback *persist_client_update;
	काष्ठा mosquitto__callback *persist_subscription_add;
	काष्ठा mosquitto__callback *persist_subscription_delete;
	काष्ठा mosquitto__callback *persist_client_संदेश_add;
	काष्ठा mosquitto__callback *persist_client_संदेश_delete;
	काष्ठा mosquitto__callback *persist_client_संदेश_update;
	काष्ठा mosquitto__callback *persist_base_संदेश_add;
	काष्ठा mosquitto__callback *persist_base_संदेश_delete;
	काष्ठा mosquitto__callback *persist_retain_संदेश_set;
	काष्ठा mosquitto__callback *persist_retain_संदेश_delete;
	काष्ठा mosquitto__callback *persist_will_add;
	काष्ठा mosquitto__callback *persist_will_delete;
};

/* This is owned by mosquitto__config or mosquitto__listener, and only referred
 * to by other structs */
काष्ठा mosquitto__security_options {
	/* Any options that get added here also need considering
	 * in config__read() with regards whether allow_anonymous
	 * should be disabled when these options are set.
	 */
	काष्ठा mosquitto__unpwd *unpwd;
	काष्ठा mosquitto__psk *psk_id;
	काष्ठा acl_file_तथ्य acl_तथ्य;
	काष्ठा password_file_तथ्य password_तथ्य;
	अक्षर *psk_file;
	mosquitto_plugin_id_t **plugins;
	पूर्णांक plugin_गिनती;
	int8_t allow_anonymous;
	bool allow_zero_लंबाई_clientid;
	अक्षर *auto_id_prefix;
	uint16_t auto_id_prefix_len;
	काष्ठा plugin__callbacks plugin_callbacks;
	mosquitto_plugin_id_t *pid; /* For registering as a "plugin" */
};

#अगर defined(WITH_EPOLL) || defined(WITH_KQUEUE)
क्रमागत struct_ident {
	id_invalid = 0,
	id_listener = 1,
	id_client = 2,
	id_listener_ws = 3,
};
#पूर्ण_अगर

काष्ठा mosquitto__listener {
	uint16_t port;
	अक्षर *host;
	अक्षर *bind_interface;
	पूर्णांक max_connections;
	अक्षर *mount_point;
	mosq_sock_t *socks;
	पूर्णांक sock_गिनती;
	पूर्णांक client_गिनती;
	क्रमागत mosquitto_protocol protocol;
	पूर्णांक socket_domain;
	bool use_username_as_clientid;
	uint8_t max_qos;
	uint16_t max_topic_alias;
	uint16_t max_topic_alias_broker;
#अगर_घोषित WITH_TLS
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
	क्रमागत mosquitto__keyform tls_keyform;
#पूर्ण_अगर
#अगर_घोषित WITH_WEBSOCKETS
#  अगर WITH_WEBSOCKETS == WS_IS_LWS
	काष्ठा lws_context *ws_context;
	bool ws_in_init;
	काष्ठा lws_protocols *ws_protocol;
#  endif
	अक्षर **ws_origins;
	पूर्णांक ws_origin_गिनती;
#पूर्ण_अगर
#अगर defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
	अक्षर *http_dir;
#पूर्ण_अगर
	काष्ठा mosquitto__security_options *security_options;
#अगर_घोषित WITH_UNIX_SOCKETS
	अक्षर *unix_socket_path;
#पूर्ण_अगर
	bool disable_protocol_v3;
	bool disable_protocol_v4;
	bool disable_protocol_v5;
	पूर्णांक enable_proxy_protocol;
	bool proxy_protocol_v2_require_tls;
#अगर_घोषित WITH_HTTP_API
	काष्ठा MHD_Daemon *mhd;
#पूर्ण_अगर
};


काष्ठा mosquitto__listener_sock {
#अगर defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	/* This *must* be the first element in the struct. */
	पूर्णांक ident;
#पूर्ण_अगर
	mosq_sock_t sock;
	काष्ठा mosquitto__listener *listener;
};


/* Callbacks belonging to a specific plugin
 * Doesn't include MOSQ_EVT_CONTROL events.
 */
काष्ठा plugin_own_callback {
	काष्ठा plugin_own_callback *अग्र, *पूर्व;
	MOSQ_FUNC_generic_callback cb_func;
	क्रमागत mosquitto_plugin_event event;
};

काष्ठा mosquitto_plugin_id_t {
	काष्ठा mosquitto__plugin_config config;
	काष्ठा mosquitto__plugin_lib lib;
	काष्ठा mosquitto__listener *listener;
	अक्षर *plugin_name;
	अक्षर *plugin_version;
	काष्ठा control_endpoint *control_endpoints;
	काष्ठा plugin_own_callback *own_callbacks;
	काष्ठा timespec अग्र_tick;
};

काष्ठा mosquitto__config {
	bool allow_duplicate_messages;
	पूर्णांक autosave_interval;
	bool autosave_on_changes;
	bool check_retain_source;
	अक्षर *clientid_prefixes;
	bool connection_messages;
	uint16_t cmd_port[CMD_PORT_LIMIT];
	पूर्णांक cmd_port_गिनती;
	bool daemon;
	bool test_configuration;
	bool enable_control_api;
	पूर्णांक global_max_clients;
	पूर्णांक global_max_connections;
	काष्ठा mosquitto__listener *default_listener; /* Points to one of `listeners` */
	काष्ठा mosquitto__listener *listeners;
	पूर्णांक listener_गिनती;
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
	uint32_t max_packet_आकार;
	uint32_t message_आकार_limit;
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
#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	पूर्णांक websockets_log_level;
#पूर्ण_अगर
	uint16_t packet_बफर_आकार;
#अगर_घोषित WITH_BRIDGE
	काष्ठा mosquitto__bridge **bridges;
	पूर्णांक bridge_गिनती;
#पूर्ण_अगर
	काष्ठा mosquitto__security_options security_options;
};


काष्ठा mosquitto__subshared {
	UT_hash_handle hh;
	काष्ठा mosquitto__subleaf *subs;
	अक्षर name[];
};

काष्ठा mosquitto__subhier {
	UT_hash_handle hh;
	काष्ठा mosquitto__subhier *parent;
	काष्ठा mosquitto__subhier *children;
	काष्ठा mosquitto__subleaf *subs;
	काष्ठा mosquitto__subshared *shared;
	uint16_t topic_len;
	अक्षर topic[];
};

काष्ठा mosquitto__subleaf {
	काष्ठा mosquitto__subleaf *पूर्व;
	काष्ठा mosquitto__subleaf *अग्र;
	काष्ठा mosquitto *context;
	काष्ठा mosquitto__subhier *hier;
	काष्ठा mosquitto__subshared *shared;
	uint32_t identifier;
	uint8_t subscription_options;
	अक्षर topic_filter[];
};

काष्ठा sub__token {
	काष्ठा sub__token *अग्र;
	uint16_t topic_len;
	अक्षर topic[];
};

काष्ठा mosquitto__retainhier {
	UT_hash_handle hh;
	काष्ठा mosquitto__retainhier *parent;
	काष्ठा mosquitto__retainhier *children;
	काष्ठा mosquitto_base_संदेश *retained;
	uint16_t topic_len;
	अक्षर topic[];
};

काष्ठा mosquitto_base_संदेश {
	UT_hash_handle hh;
	काष्ठा mosquitto_base_संदेश_2 तथ्य;
	काष्ठा mosquitto__listener *source_listener;
	अक्षर **dest_ids;
	पूर्णांक dest_id_गिनती;
	पूर्णांक ref_गिनती;
	क्रमागत mosquitto_संदेश_origin origin;
	bool stored;
};

काष्ठा mosquitto_client_संदेश {
	काष्ठा mosquitto_client_संदेश_2 तथ्य;
	काष्ठा mosquitto_client_संदेश *पूर्व;
	काष्ठा mosquitto_client_संदेश *अग्र;
	काष्ठा mosquitto_base_संदेश *base_संदेश;
};


काष्ठा mosquitto__psk {
	UT_hash_handle hh;
	अक्षर *username;
	अक्षर *password;
};

काष्ठा mosquitto__message_v5 {
	काष्ठा mosquitto__message_v5 *अग्र, *पूर्व;
	अक्षर *topic;
	व्योम *payload;
	mosquitto_property *properties;
	/* clientid is used only by mosquitto_broker_publish*() to indicate
	   this message is for a specific client. */
	अक्षर *clientid;
	पूर्णांक payloadlen;
	पूर्णांक qos;
	bool retain;
};

काष्ठा mosquitto_db {
	dbid_t last_db_id;
	uint64_t गाँठ_id_shifted;
	काष्ठा mosquitto__subhier *normal_subs;
	काष्ठा mosquitto__subhier *shared_subs;
	काष्ठा mosquitto__retainhier *retains;
	काष्ठा mosquitto *contexts_by_id;
	काष्ठा mosquitto *contexts_by_sock;
	काष्ठा mosquitto *contexts_by_id_delayed_auth;
	काष्ठा mosquitto *contexts_for_free;
	mosquitto_plugin_id_t **plugins;
	पूर्णांक plugin_गिनती;
#अगर_घोषित WITH_BRIDGE
	काष्ठा mosquitto **bridges;
	पूर्णांक bridge_गिनती;
#पूर्ण_अगर
	काष्ठा clientid_सूचकांक_hash_2 *clientid_सूचकांक_hash;
	काष्ठा mosquitto_base_संदेश *संदेश_store;
	समय_प्रकार now_s; /* Monotonic clock, where possible */
	समय_प्रकार now_real_s; /* Read clock, for measuring session/message expiry */
	uint64_t गाँठ_id; /* for unique db ids */
	समय_प्रकार अग्र_event_ms; /* for mux timeout */
	पूर्णांक संदेश_store_गिनती;
	अचिन्हित दीर्घ संदेश_store_bytes;
	अक्षर *config_file;
	काष्ठा mosquitto__config *config;
	bool quiet;
	bool verbose;
#अगर_घोषित WITH_SYS_वृक्ष
	पूर्णांक subscription_गिनती;
	पूर्णांक shared_subscription_गिनती;
	पूर्णांक retained_गिनती;
#पूर्ण_अगर
	पूर्णांक persistence_changes;
	काष्ठा mosquitto *ll_for_free;
#अगर_घोषित WITH_EPOLL
	पूर्णांक epollfd;
#पूर्ण_अगर
#अगर_घोषित WITH_KQUEUE
	पूर्णांक kqueuefd;
#पूर्ण_अगर
	काष्ठा mosquitto__message_v5 *plugin_msgs;
#अगर_घोषित WITH_TLS
	/* tls_keylog can't be in the config struct because it is used
	   before the config is allocated. Config probably
	   shouldn't be separately allocated. */
	अक्षर *tls_keylog;
#पूर्ण_अगर
	bool shutdown;
};

क्रमागत mosquitto__bridge_direction {
	bd_out = 0,
	bd_in = 1,
	bd_both = 2,
};

क्रमागत mosquitto_bridge_start_type {
	bst_automatic = 0,
	bst_lazy = 1,
	bst_manual = 2,
	bst_once = 3,
};

क्रमागत mosquitto_bridge_reload_type {
	brt_lazy = 0,
	brt_immediate = 1,
};

काष्ठा mosquitto__bridge_topic {
	काष्ठा mosquitto__bridge_topic *अग्र;
	अक्षर *topic;
	अक्षर *local_prefix;
	अक्षर *remote_prefix;
	अक्षर *local_topic; /* topic prefixed with local_prefix */
	अक्षर *remote_topic; /* topic prefixed with remote_prefix */
	क्रमागत mosquitto__bridge_direction direction;
	uint8_t qos;
};

काष्ठा bridge_address {
	अक्षर *address;
	uint16_t port;
};

काष्ठा mosquitto__bridge {
	अक्षर *name;
	काष्ठा bridge_address *addresses;
	पूर्णांक cur_address;
	पूर्णांक address_गिनती;
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
	अचिन्हित पूर्णांक tcp_keepalive_गणक;
#अगर_घोषित WITH_TCP_USER_TIMEOUT
	पूर्णांक tcp_user_timeout;
#पूर्ण_अगर
	काष्ठा mosquitto__bridge_topic *topics;
	पूर्णांक topic_गिनती;
	bool topic_remapping;
	क्रमागत mosquitto__protocol protocol_version;
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
	क्रमागत mosquitto_bridge_start_type start_type;
	पूर्णांक idle_timeout;
	पूर्णांक restart_timeout;
	पूर्णांक backoff_base;
	पूर्णांक backoff_cap;
	पूर्णांक stable_connection_period;
	पूर्णांक threshold;
	uint32_t maximum_packet_आकार;
	uint32_t session_expiry_interval;
	uint16_t receive_maximum;
	bool lazy_reconnect;
	bool attempt_unsubscribe;
	bool initial_notification_done;
	bool outgoing_retain;
	क्रमागत mosquitto_bridge_reload_type reload_type;
	uint16_t max_topic_alias;
#अगर_घोषित WITH_TLS
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
#पूर्ण_अगर
};

#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
काष्ठा libws_mqtt_hack {
	अक्षर *http_dir;
	काष्ठा mosquitto__listener *listener;
};

काष्ठा libws_mqtt_तथ्य {
	काष्ठा mosquitto *mosq;
};
#पूर्ण_अगर

#समावेश <net_mosq.h>


काष्ठा control_endpoint {
	काष्ठा control_endpoint *अग्र, *पूर्व;
	अक्षर topic[];
};

बाह्य काष्ठा mosquitto_db db;

/* ============================================================
 * Main functions
 * ============================================================ */
पूर्णांक mosquitto_main_loop(काष्ठा mosquitto__listener_sock *listensock, पूर्णांक listensock_गिनती);
व्योम loop_update_अग्र_event(समय_प्रकार new_ms);

/* ============================================================
 * Config functions
 * ============================================================ */
/* Initialise config struct to default values. */
व्योम config__init(काष्ठा mosquitto__config *config);
#अगर_घोषित WITH_BRIDGE
व्योम config__bridge_cleanup(काष्ठा mosquitto__bridge *bridge);
#पूर्ण_अगर
/* Parse command line options into config. */
पूर्णांक config__parse_args(काष्ठा mosquitto__config *config, पूर्णांक argc, अक्षर *argv[]);
/* Read configuration data from config->config_file into config.
 * If reload is true, don't process config options that shouldn't be reloaded (listeners etc)
 * Returns 0 on success, 1 if there is a configuration error or if a file cannot be opened.
 */
पूर्णांक config__read(काष्ठा mosquitto__config *config, bool reload);
/* Free all config data. */
व्योम config__cleanup(काष्ठा mosquitto__config *config);
पूर्णांक config__get_dir_files(स्थिर अक्षर *include_dir, अक्षर ***files, पूर्णांक *file_गिनती);

/* ============================================================
 * Server send functions
 * ============================================================ */
पूर्णांक send__connack(काष्ठा mosquitto *context, uint8_t ack, uint8_t reason_code, स्थिर mosquitto_property *properties);
पूर्णांक send__suback(काष्ठा mosquitto *context, uint16_t mid, uint32_t payloadlen, स्थिर व्योम *payload);
पूर्णांक send__unsuback(काष्ठा mosquitto *context, uint16_t mid, पूर्णांक reason_code_गिनती, uint8_t *reason_codes, स्थिर mosquitto_property *properties);
पूर्णांक send__auth(काष्ठा mosquitto *context, uint8_t reason_code, स्थिर व्योम *auth_तथ्य, uint16_t auth_तथ्य_len);

/* ============================================================
 * Network functions
 * ============================================================ */
व्योम net__broker_init(व्योम);
व्योम net__broker_cleanup(व्योम);
काष्ठा mosquitto *net__socket_accept(काष्ठा mosquitto__listener_sock *listensock);
पूर्णांक net__socket_listen(काष्ठा mosquitto__listener *listener);
पूर्णांक net__socket_get_address(mosq_sock_t sock, अक्षर *buf, माप_प्रकार len, uint16_t *remote_address);
पूर्णांक net__tls_load_verify(काष्ठा mosquitto__listener *listener);
पूर्णांक net__tls_server_ctx(काष्ठा mosquitto__listener *listener);
पूर्णांक net__load_certificates(काष्ठा mosquitto__listener *listener);

/* ============================================================
 * Read handling functions
 * ============================================================ */
पूर्णांक handle__packet(काष्ठा mosquitto *context);
पूर्णांक handle__connack(काष्ठा mosquitto *context);
पूर्णांक handle__connect(काष्ठा mosquitto *context);
पूर्णांक handle__disconnect(काष्ठा mosquitto *context);
पूर्णांक handle__publish(काष्ठा mosquitto *context);
पूर्णांक handle__subscribe(काष्ठा mosquitto *context);
पूर्णांक handle__unsubscribe(काष्ठा mosquitto *context);
पूर्णांक handle__auth(काष्ठा mosquitto *context);

/* ============================================================
 * Database handling
 * ============================================================ */
पूर्णांक db__open(काष्ठा mosquitto__config *config);
पूर्णांक db__close(व्योम);
#अगर_घोषित WITH_PERSISTENCE
पूर्णांक persist__backup(bool shutdown);
पूर्णांक persist__restore(व्योम);
#पूर्ण_अगर
/* Return the number of in-flight messages in count. */
पूर्णांक db_message_गिनती(पूर्णांक *गिनती);
पूर्णांक db__message_delete_outgoing(काष्ठा mosquitto *context, uint16_t mid, क्रमागत mosquitto_संदेश_state expect_state, पूर्णांक qos);
पूर्णांक db_message_जोड़ो_outgoing(काष्ठा mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, काष्ठा mosquitto_base_संदेश *base_संदेश, uint32_t subscription_identifier, bool update, bool persist);
पूर्णांक db_message_जोड़ो_incoming(काष्ठा mosquitto *context, uint64_t cmsg_id, काष्ठा mosquitto_base_संदेश *base_संदेश, bool persist);
पूर्णांक db__message_remove_incoming(काष्ठा mosquitto *context, uint16_t mid);
पूर्णांक db__message_release_incoming(काष्ठा mosquitto *context, uint16_t mid);
पूर्णांक db__message_update_outgoing(काष्ठा mosquitto *context, uint16_t mid, क्रमागत mosquitto_संदेश_state state, पूर्णांक qos, bool persist);
व्योम db__message_dequeue_first(काष्ठा mosquitto *context, काष्ठा mosquitto_संदेश_तथ्य *संदेश_तथ्य);
पूर्णांक db__messages_delete(काष्ठा mosquitto *context, bool force_free);
पूर्णांक db__messages_delete_incoming(काष्ठा mosquitto *context);
पूर्णांक db__messages_delete_outgoing(काष्ठा mosquitto *context);
पूर्णांक db__messages_easy_queue(काष्ठा mosquitto *context, स्थिर अक्षर *topic, uint8_t qos, uint32_t payloadlen, स्थिर व्योम *payload, पूर्णांक retain, uint32_t message_expiry_interval, mosquitto_property **properties);
पूर्णांक db__message_store(स्थिर काष्ठा mosquitto *source, काष्ठा mosquitto_base_संदेश *base_संदेश, uint32_t *message_expiry_interval, क्रमागत mosquitto_संदेश_origin origin);
पूर्णांक db__message_store_find(काष्ठा mosquitto *context, uint16_t mid, काष्ठा mosquitto_client_संदेश **client_संदेश);
पूर्णांक db_संदेश_store_add(काष्ठा mosquitto_base_संदेश *base_संदेश);
व्योम db_संदेश_store_remove(काष्ठा mosquitto_base_संदेश *base_संदेश, bool notify);
व्योम db_संदेश_store_ref_inc(काष्ठा mosquitto_base_संदेश *base_संदेश);
व्योम db_संदेश_store_ref_dec(काष्ठा mosquitto_base_संदेश **base_संदेश);
व्योम db_संदेश_store_clean(व्योम);
व्योम db_संदेश_store_compact(व्योम);
व्योम db_संदेश_store_free(काष्ठा mosquitto_base_संदेश *base_संदेश);
पूर्णांक db__message_reconnect_reset(काष्ठा mosquitto *context);
bool db__ready_for_flight(काष्ठा mosquitto *context, क्रमागत mosquitto_संदेश_direction dir, पूर्णांक qos);
bool db__ready_for_queue(काष्ठा mosquitto *context, पूर्णांक qos, काष्ठा mosquitto_संदेश_तथ्य *संदेश_तथ्य);
व्योम sys_वृक्ष_init(व्योम);
व्योम sys_वृक्ष_update(bool force);
पूर्णांक db__message_write_inflight_out_all(काष्ठा mosquitto *context);
पूर्णांक db__message_write_inflight_out_latest(काष्ठा mosquitto *context);
पूर्णांक db__message_write_queued_out(काष्ठा mosquitto *context);
पूर्णांक db__message_write_queued_in(काष्ठा mosquitto *context);
व्योम db_संदेश_add_to_inflight_stats(काष्ठा mosquitto_संदेश_तथ्य *संदेश_तथ्य, काष्ठा mosquitto_client_संदेश *संदेश);
व्योम db_संदेश_add_to_queued_stats(काष्ठा mosquitto_संदेश_तथ्य *संदेश_तथ्य, काष्ठा mosquitto_client_संदेश *संदेश);
uint64_t db_new_संदेश_id(व्योम);
व्योम db__expire_all_messages(काष्ठा mosquitto *context);
व्योम db__check_acl_of_all_messages(काष्ठा mosquitto *context);

/* ============================================================
 * Subscription functions
 * ============================================================ */
पूर्णांक sub__init(व्योम);
पूर्णांक sub__add(काष्ठा mosquitto *context, स्थिर काष्ठा mosquitto_subscription *sub);
पूर्णांक sub__remove(काष्ठा mosquitto *context, स्थिर अक्षर *sub, uint8_t *reason);
व्योम sub_वृक्ष_print(काष्ठा mosquitto__subhier *मूल, पूर्णांक level);
पूर्णांक sub__clean_session(काष्ठा mosquitto *context);
पूर्णांक sub__messages_queue(स्थिर अक्षर *source_id, uint8_t qos, काष्ठा mosquitto_base_संदेश **base_संदेश);
पूर्णांक sub__topic_tokenise(स्थिर अक्षर *subtopic, अक्षर **local_sub, अक्षर ***topics, स्थिर अक्षर **sharename);
व्योम sub__topic_tokens_free(काष्ठा sub__token *tokens);

/* ============================================================
 * Context functions
 * ============================================================ */
काष्ठा mosquitto *context__init(व्योम);
पूर्णांक context__init_sock(काष्ठा mosquitto *context, mosq_sock_t sock, bool get_address);
व्योम context__cleanup(काष्ठा mosquitto *context, bool force_free);
व्योम context__disconnect(काष्ठा mosquitto *context, पूर्णांक reason);
व्योम context__add_to_disused(काष्ठा mosquitto *context);
व्योम context__free_disused(व्योम);
व्योम context__send_will(काष्ठा mosquitto *context);
व्योम context__add_to_by_id(काष्ठा mosquitto *context);
व्योम context__remove_from_by_id(काष्ठा mosquitto *context);

पूर्णांक connect__on_authorised(काष्ठा mosquitto *context, व्योम *auth_तथ्य_out, uint16_t auth_तथ्य_out_len);


/* ============================================================
 * Control functions
 * ============================================================ */
#अगर_घोषित WITH_CONTROL
पूर्णांक control__process(काष्ठा mosquitto *context, काष्ठा mosquitto_base_संदेश *base_संदेश);
व्योम control__cleanup(व्योम);
#पूर्ण_अगर
पूर्णांक control__register_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, स्थिर अक्षर *topic, व्योम *userdata);
पूर्णांक control__unregister_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, स्थिर अक्षर *topic);
व्योम control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier);


/* ============================================================
 * Logging functions
 * ============================================================ */
पूर्णांक log__init(काष्ठा mosquitto__config *config);
पूर्णांक log__close(काष्ठा mosquitto__config *config);
व्योम log__internal(स्थिर अक्षर *fmt, ...) __attribute__((format(म_लिखो, 1, 2)));

/* ============================================================
 * Bridge functions
 * ============================================================ */
#अगर_घोषित WITH_BRIDGE
व्योम bridge__start_all(व्योम);
व्योम bridge__reload(व्योम);
व्योम bridge__db_cleanup(व्योम);
व्योम bridge__cleanup(काष्ठा mosquitto *context);
पूर्णांक bridge__connect(काष्ठा mosquitto *context);
#अगर defined(__GLIBC__) && defined(WITH_ADNS)
पूर्णांक bridge__connect_step3(काष्ठा mosquitto *context);
#पूर्ण_अगर
पूर्णांक bridge__on_connect(काष्ठा mosquitto *context);
व्योम bridge_check(व्योम);
पूर्णांक bridge__register_local_connections(व्योम);
पूर्णांक bridge__add_topic(काष्ठा mosquitto__bridge *bridge, स्थिर अक्षर *topic, क्रमागत mosquitto__bridge_direction direction, uint8_t qos, स्थिर अक्षर *local_prefix, स्थिर अक्षर *remote_prefix);
व्योम bridge__cleanup_topics(काष्ठा mosquitto__bridge *bridge);
पूर्णांक bridge__remap_topic_in(काष्ठा mosquitto *context, अक्षर **topic);
#पूर्ण_अगर

/* ============================================================
 * IO multiplex related functions
 * ============================================================ */
पूर्णांक mux__init(व्योम);
पूर्णांक mux__add_listeners(काष्ठा mosquitto__listener_sock *listensock, पूर्णांक listensock_गिनती);
पूर्णांक mux__delete_listeners(काष्ठा mosquitto__listener_sock *listensock, पूर्णांक listensock_गिनती);
पूर्णांक mux__loop_prepare(व्योम);
पूर्णांक mux__new(काष्ठा mosquitto *context);
पूर्णांक mux__add_out(काष्ठा mosquitto *context);
पूर्णांक mux__remove_out(काष्ठा mosquitto *context);
पूर्णांक mux__delete(काष्ठा mosquitto *context);
पूर्णांक mux__wait(व्योम);
पूर्णांक mux__handle(काष्ठा mosquitto__listener_sock *listensock, पूर्णांक listensock_गिनती);
पूर्णांक mux__cleanup(व्योम);

/* ============================================================
 * Listener related functions
 * ============================================================ */
बाह्य काष्ठा mosquitto__listener_sock *g_listensock;
बाह्य पूर्णांक g_listensock_गिनती;

व्योम listener__set_defaults(काष्ठा mosquitto__listener *listener);
व्योम listeners__reload_all_certificates(व्योम);
#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
व्योम listeners__add_websockets(काष्ठा lws_context *ws_context, mosq_sock_t fd);
#पूर्ण_अगर
पूर्णांक listeners__start(व्योम);
व्योम listeners__stop(व्योम);

/* ============================================================
 * Plugin related functions
 * ============================================================ */
पूर्णांक plugin__load_v5(mosquitto_plugin_id_t *plugin, व्योम *lib);
पूर्णांक plugin__load_v4(mosquitto_plugin_id_t *plugin, व्योम *lib);
पूर्णांक plugin__load_v3(mosquitto_plugin_id_t *plugin, व्योम *lib);
पूर्णांक plugin__load_v2(mosquitto_plugin_id_t *plugin, व्योम *lib);
पूर्णांक acl__pre_check(mosquitto_plugin_id_t *plugin, काष्ठा mosquitto *context, पूर्णांक access);

व्योम LIB_ERROR(व्योम);
व्योम plugin__handle_connect(काष्ठा mosquitto *context);
व्योम plugin__handle_disconnect(काष्ठा mosquitto *context, पूर्णांक reason);
व्योम plugin__handle_client_offline(काष्ठा mosquitto *context, पूर्णांक reason);
पूर्णांक plugin__handle_message_in(काष्ठा mosquitto *context, काष्ठा mosquitto_base_संदेश_2 *base_संदेश);
पूर्णांक plugin__handle_message_out(काष्ठा mosquitto *context, काष्ठा mosquitto_base_संदेश_2 *base_संदेश);
पूर्णांक plugin__handle_subscribe(काष्ठा mosquitto *context, काष्ठा mosquitto_subscription *sub);
पूर्णांक plugin__handle_unsubscribe(काष्ठा mosquitto *context, काष्ठा mosquitto_subscription *sub);
पूर्णांक plugin__handle_reload(व्योम);
व्योम plugin__handle_tick(व्योम);
पूर्णांक plugin__callback_unregister_all(mosquitto_plugin_id_t *identifier);
व्योम plugin_persist__handle_restore(व्योम);
व्योम plugin_persist__handle_client_add(काष्ठा mosquitto *context);
व्योम plugin_persist__handle_client_delete(काष्ठा mosquitto *context);
व्योम plugin_persist__handle_client_update(काष्ठा mosquitto *context);
व्योम plugin_persist__handle_subscription_add(काष्ठा mosquitto *context, स्थिर काष्ठा mosquitto_subscription *sub);
व्योम plugin_persist__handle_subscription_delete(काष्ठा mosquitto *context, अक्षर *sub);
व्योम plugin_persist_handle_client_संदेश_add(काष्ठा mosquitto *context, स्थिर काष्ठा mosquitto_client_संदेश *cmsg);
व्योम plugin_persist_handle_client_संदेश_delete(काष्ठा mosquitto *context, स्थिर काष्ठा mosquitto_client_संदेश *cmsg);
व्योम plugin_persist_handle_client_संदेश_update(काष्ठा mosquitto *context, स्थिर काष्ठा mosquitto_client_संदेश *cmsg);
व्योम plugin_persist_handle_base_संदेश_add(काष्ठा mosquitto_base_संदेश *base_संदेश);
व्योम plugin_persist_handle_base_संदेश_delete(काष्ठा mosquitto_base_संदेश *base_संदेश);
व्योम plugin_persist_handle_retain_संदेश_set(काष्ठा mosquitto_base_संदेश *base_संदेश);
व्योम plugin_persist_handle_retain_संदेश_delete(काष्ठा mosquitto_base_संदेश *base_संदेश);
व्योम plugin_persist__handle_will_add(काष्ठा mosquitto *context);
व्योम plugin_persist__handle_will_delete(काष्ठा mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
पूर्णांक keepalive__init(व्योम);
व्योम keepalive__cleanup(व्योम);
पूर्णांक keepalive__add(काष्ठा mosquitto *context);
व्योम keepalive__check(व्योम);
पूर्णांक keepalive__remove(काष्ठा mosquitto *context);
पूर्णांक keepalive__update(काष्ठा mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
पूर्णांक property__process_connect(काष्ठा mosquitto *context, mosquitto_property **props);
पूर्णांक property__process_will(काष्ठा mosquitto *context, काष्ठा mosquitto_message_all *संदेश, mosquitto_property **props);
पूर्णांक property__process_publish(काष्ठा mosquitto_base_संदेश *base_संदेश, mosquitto_property **props, पूर्णांक *topic_alias, uint32_t *message_expiry_interval, bool is_bridge);
पूर्णांक property__process_disconnect(काष्ठा mosquitto *context, mosquitto_property **props);

/* ============================================================
 * Retain tree related functions
 * ============================================================ */
पूर्णांक retain__init(व्योम);
व्योम retain__clean(काष्ठा mosquitto__retainhier **retainhier);
पूर्णांक retain__queue(काष्ठा mosquitto *context, स्थिर काष्ठा mosquitto_subscription *sub);
पूर्णांक retain__store(स्थिर अक्षर *topic, काष्ठा mosquitto_base_संदेश *base_संदेश, अक्षर **split_topics, bool persist);
व्योम retain__expiry_check(व्योम);
व्योम retain__expire(काष्ठा mosquitto__retainhier **retainhier);

/* ============================================================
 * Security related functions
 * ============================================================ */
पूर्णांक acl__find_acls(काष्ठा mosquitto *context);
पूर्णांक plugin__load_all(व्योम);
पूर्णांक plugin__unload_all(व्योम);
पूर्णांक config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, काष्ठा mosquitto__security_options *security_options);

पूर्णांक mosquitto_security_init(bool reload);
पूर्णांक mosquitto_security_cleanup(bool reload);
पूर्णांक mosquitto_acl_check(काष्ठा mosquitto *context, स्थिर अक्षर *topic, uint32_t payloadlen, व्योम *payload, uint8_t qos, bool retain, mosquitto_property *properties, पूर्णांक access);
पूर्णांक mosquitto_basic_auth(काष्ठा mosquitto *context);
पूर्णांक mosquitto_psk_कुंजी_get(काष्ठा mosquitto *context, स्थिर अक्षर *hint, स्थिर अक्षर *identity, अक्षर *कुंजी, पूर्णांक max_कुंजी_len);

पूर्णांक mosquitto_security_init_default(व्योम);
पूर्णांक mosquitto_security_apply_default(व्योम);
पूर्णांक mosquitto_security_cleanup_default(व्योम);
पूर्णांक mosquitto_psk_कुंजी_get_default(काष्ठा mosquitto *context, स्थिर अक्षर *hint, स्थिर अक्षर *identity, अक्षर *कुंजी, पूर्णांक max_कुंजी_len);
पूर्णांक broker_acl_file__init(व्योम);
व्योम broker_acl_file__cleanup(व्योम);
पूर्णांक broker_password_file__init(व्योम);
व्योम broker_password_file__cleanup(व्योम);
पूर्णांक psk_file__init(व्योम);
पूर्णांक psk_file__cleanup(व्योम);

पूर्णांक mosquitto_security_auth_start(काष्ठा mosquitto *context, bool reauth, स्थिर व्योम *तथ्य_in, uint16_t तथ्य_in_len, व्योम **तथ्य_out, uint16_t *तथ्य_out_len);
पूर्णांक mosquitto_security_auth_continue(काष्ठा mosquitto *context, स्थिर व्योम *तथ्य_in, uint16_t तथ्य_len, व्योम **तथ्य_out, uint16_t *तथ्य_out_len);

व्योम unpwd__free_item(काष्ठा mosquitto__unpwd **unpwd, काष्ठा mosquitto__unpwd *item);

/* ============================================================
 * Session expiry
 * ============================================================ */
पूर्णांक session_expiry__add(काष्ठा mosquitto *context);
पूर्णांक session_expiry__add_from_persistence(काष्ठा mosquitto *context, समय_प्रकार expiry_time);
व्योम session_expiry__add_on_shutdown(व्योम);
व्योम session_expiry__remove(काष्ठा mosquitto *context);
व्योम session_expiry__remove_all(व्योम);
व्योम session_expiry__check(व्योम);
व्योम session_expiry__send_all(व्योम);

/* ============================================================
 * Signals
 * ============================================================ */
व्योम signal__setup(व्योम);
पूर्णांक signal__flag_check(व्योम);

/* ============================================================
 * Window service and signal related functions
 * ============================================================ */
#अगर defined(WIN32) || defined(__CYGWIN__)
व्योम service_install(अक्षर *name);
व्योम service_uninstall(अक्षर *name);
व्योम service_run(अक्षर *name);

#अगर_घोषित WIN32
DWORD WINAPI SigThreadProc(व्योम *तथ्य);
#पूर्ण_अगर
#पूर्ण_अगर

/* ============================================================
 * Watchdog
 * ============================================================ */
व्योम watchdog__init(व्योम);
व्योम watchdog__check(व्योम);

/* ============================================================
 * Websockets related functions
 * ============================================================ */
#अगर_घोषित WITH_WEBSOCKETS
#  अगर WITH_WEBSOCKETS == WS_IS_LWS
व्योम mosq_websockets_init(काष्ठा mosquitto__listener *listener, स्थिर काष्ठा mosquitto__config *conf);
#  endif
पूर्णांक http__context_init(काष्ठा mosquitto *context);
पूर्णांक http__context_cleanup(काष्ठा mosquitto *context);
पूर्णांक http__read(काष्ठा mosquitto *context);
#पूर्ण_अगर
व्योम do_disconnect(काष्ठा mosquitto *context, पूर्णांक reason);

/* ============================================================
 * PROXY related functions
 * ============================================================ */
पूर्णांक proxy_v2__read(काष्ठा mosquitto *context);
पूर्णांक proxy_v1__read(काष्ठा mosquitto *context);

/* ============================================================
 * Will delay
 * ============================================================ */
पूर्णांक will_delay__add(काष्ठा mosquitto *context);
व्योम will_delay__check(व्योम);
व्योम will_delay__send_all(व्योम);
व्योम will_delay__remove(काष्ठा mosquitto *mosq);

/* ============================================================
 * HTTP Info
 * ============================================================ */
पूर्णांक http_api__start_local(काष्ठा mosquitto__listener *listener);
पूर्णांक http_api__start(काष्ठा mosquitto__listener *listener);
व्योम http_api__stop(काष्ठा mosquitto__listener *listener);


/* ============================================================
 * Other
 * ============================================================ */
#अगर_घोषित WITH_XTREPORT
व्योम xtreport(व्योम);
#पूर्ण_अगर

व्योम broker_control__init(व्योम);
व्योम broker_control__cleanup(व्योम);
व्योम broker_control__reload(व्योम);

#पूर्ण_अगर
