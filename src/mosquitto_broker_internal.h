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

#यदि_अपरिभाषितम् MOSQUITTO_BROKER_आन्तरिकम्_H
#परिभाषय MOSQUITTO_BROKER_आन्तरिकम्_H

#अन्तर्भाव "config.h"
#अन्तर्भाव <stdio.h>

#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#  यदि LWS_LIBRARY_संस्करणम्_सङ्ख्या >= 3002000 && !defined(LWS_WITH_EXTERNAL_POLL)
#    warning "libwebsockets is not compiled with LWS_WITH_EXTERNAL_POLL support. Websocket performance will be unusable."
#  endif
#यदि_अन्तः

#यदि_परिभाषितम् WITH_HTTP_API
#  include <microhttpd.h>
#यदि_अन्तः

#यदि_परिभाषितम् __linux__
#परिभाषय WITH_TCP_USER_TIMEOUT
#यदि_अन्तः

#अन्तर्भाव "mosquitto_internal.h"
#अन्तर्भाव "mosquitto/broker.h"
#अन्तर्भाव "mosquitto/broker_plugin.h"
#अन्तर्भाव "mosquitto.h"
#अन्तर्भाव "logging_mosq.h"
#अन्तर्भाव "tls_mosq.h"
#अन्तर्भाव "uthash.h"
#अन्तर्भाव "acl_file.h"
#अन्तर्भाव "password_file.h"

#यदि_अपरिभाषितम् __GNUC__
#परिभाषय __attribute__(attrib)
#यदि_अन्तः

/* Log destinations */
#परिभाषय MQTT3_LOG_NONE 0x00
#परिभाषय MQTT3_LOG_SYSLOG 0x01
#परिभाषय MQTT3_LOG_FILE 0x02
#परिभाषय MQTT3_LOG_STDOUT 0x04
#परिभाषय MQTT3_LOG_STDERR 0x08
#परिभाषय MQTT3_LOG_TOPIC 0x10
#परिभाषय MQTT3_LOG_DLT 0x20
#यदि_परिभाषितम् ANDROID
#परिभाषय MQTT3_LOG_ANDROID 0x40
#यदि_अन्तः
#परिभाषय MQTT3_LOG_ALL 0xFF

#परिभाषय CMD_पत्तनम्_सीमा 10

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
	शून्य *user_दत्त;
	पूर्णाङ्क (*plugin_संस्करणम्)(शून्य);
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
	पूर्णाङ्क संस्करणम्_2;
};

संरचना mosquitto__plugin_config {
	वर्ण *मार्गः;
	वर्ण *name;
	संरचना mosquitto_opt *options;
	संरचना mosquitto__security_options **security_options;
	पूर्णाङ्क option_गणनम्;
	पूर्णाङ्क security_option_गणनम्;
	bool deny_special_chars;
};

संरचना mosquitto__callback {
	UT_hash_handle hh; /* For callbacks that register for e.g. a specific topic */
	संरचना mosquitto__callback *अग्र, *पूर्व; /* For typical callbacks */
	MOSQ_FUNC_generic_callback cb;
	शून्य *userdata;
	सङ्घ {
		वर्ण *topic;
		संरचना timespec अग्र_tick;
	} दत्त;
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
	संरचना mosquitto__callback *सन्देशः_अन्तर्;
	संरचना mosquitto__callback *सन्देशः_बहिर्गमनम्;
	संरचना mosquitto__callback *psk_कुञ्चिका;
	संरचना mosquitto__callback *reload;
	संरचना mosquitto__callback *subscribe;
	संरचना mosquitto__callback *unsubscribe;
	संरचना mosquitto__callback *persist_restore;
	संरचना mosquitto__callback *persist_client_सङ्कलय;
	संरचना mosquitto__callback *persist_client_delete;
	संरचना mosquitto__callback *persist_client_update;
	संरचना mosquitto__callback *persist_subscription_सङ्कलय;
	संरचना mosquitto__callback *persist_subscription_delete;
	संरचना mosquitto__callback *persist_client_सन्देशः_सङ्कलय;
	संरचना mosquitto__callback *persist_client_सन्देशः_delete;
	संरचना mosquitto__callback *persist_client_सन्देशः_update;
	संरचना mosquitto__callback *persist_आधारः_सन्देशः_सङ्कलय;
	संरचना mosquitto__callback *persist_आधारः_सन्देशः_delete;
	संरचना mosquitto__callback *persist_retain_सन्देशः_समुच्चयः;
	संरचना mosquitto__callback *persist_retain_सन्देशः_delete;
	संरचना mosquitto__callback *persist_will_सङ्कलय;
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
	संरचना acl_file_दत्त acl_दत्त;
	संरचना password_file_दत्त password_दत्त;
	वर्ण *psk_file;
	mosquitto_plugin_id_t **plugins;
	पूर्णाङ्क plugin_गणनम्;
	int8_t allow_anonymous;
	bool allow_zero_दीर्घता_clientid;
	वर्ण *auto_id_उपसर्गः;
	uint16_t auto_id_उपसर्गः_len;
	संरचना plugin__callbacks plugin_callbacks;
	mosquitto_plugin_id_t *pid; /* For registering as a "plugin" */
};

#यदि defined(WITH_EPOLL) || defined(WITH_KQUEUE)
गणना struct_ident {
	id_invalid = 0,
	id_listener = 1,
	id_client = 2,
	id_listener_ws = 3,
};
#यदि_अन्तः

संरचना mosquitto__listener {
	uint16_t पत्तनम्;
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
#यदि_परिभाषितम् WITH_TLS
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
	वर्ण *tls_संस्करणम्;
	bool use_तटस्थः_as_username;
	bool use_subject_as_username;
	bool require_certificate;
	bool disable_client_cert_date_checks;
	गणना mosquitto__keyform tls_keyform;
#यदि_अन्तः
#यदि_परिभाषितम् WITH_WEBSOCKETS
#  यदि WITH_WEBSOCKETS == WS_IS_LWS
	संरचना lws_context *ws_context;
	bool ws_अन्तर्_init;
	संरचना lws_protocols *ws_protocol;
#  endif
	वर्ण **ws_origins;
	पूर्णाङ्क ws_origin_गणनम्;
#यदि_अन्तः
#यदि defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
	वर्ण *http_dir;
#यदि_अन्तः
	संरचना mosquitto__security_options *security_options;
#यदि_परिभाषितम् WITH_UNIX_SOCKETS
	वर्ण *unix_socket_मार्गः;
#यदि_अन्तः
	bool disable_protocol_v3;
	bool disable_protocol_v4;
	bool disable_protocol_v5;
	पूर्णाङ्क enable_proxy_protocol;
	bool proxy_protocol_v2_require_tls;
#यदि_परिभाषितम् WITH_HTTP_API
	संरचना MHD_Daemon *mhd;
#यदि_अन्तः
};


संरचना mosquitto__listener_sock {
#यदि defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	/* This *must* be the first element in the struct. */
	पूर्णाङ्क ident;
#यदि_अन्तः
	mosq_sock_t sock;
	संरचना mosquitto__listener *listener;
};


/* Callbacks belonging to a specific plugin
 * Doesn't include MOSQ_EVT_CONTROL events.
 */
संरचना plugin_own_callback {
	संरचना plugin_own_callback *अग्र, *पूर्व;
	MOSQ_FUNC_generic_callback cb_func;
	गणना mosquitto_plugin_event event;
};

संरचना mosquitto_plugin_id_t {
	संरचना mosquitto__plugin_config config;
	संरचना mosquitto__plugin_lib lib;
	संरचना mosquitto__listener *listener;
	वर्ण *plugin_name;
	वर्ण *plugin_संस्करणम्;
	संरचना control_endpoint *control_endpoints;
	संरचना plugin_own_callback *own_callbacks;
	संरचना timespec अग्र_tick;
};

संरचना mosquitto__config {
	bool allow_duplicate_messages;
	पूर्णाङ्क autosave_interval;
	bool autosave_on_changes;
	bool check_retain_source;
	वर्ण *clientid_prefixes;
	bool connection_messages;
	uint16_t cmd_पत्तनम्[CMD_पत्तनम्_सीमा];
	पूर्णाङ्क cmd_पत्तनम्_गणनम्;
	bool daemon;
	bool test_configuration;
	bool enable_control_api;
	पूर्णाङ्क global_max_clients;
	पूर्णाङ्क global_max_connections;
	संरचना mosquitto__listener *default_listener; /* Points to one of `listeners` */
	संरचना mosquitto__listener *listeners;
	पूर्णाङ्क listener_गणनम्;
	bool local_only;
	अचिह्नित पूर्णाङ्क log_dest;
	पूर्णाङ्क log_facility;
	अचिह्नित पूर्णाङ्क log_type;
	bool log_timestamp;
	वर्ण *log_timestamp_format;
	वर्ण *log_file;
	FILE *log_fptr;
	size_t max_inflight_अष्टकानि;
	size_t max_queued_अष्टकानि;
	पूर्णाङ्क max_queued_messages;
	uint32_t max_packet_आकार;
	uint32_t सन्देशः_आकार_सीमा;
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
	bool प्रत्येकम्_listener_settings;
	bool retain_उपलब्धम्;
	पूर्णाङ्क retain_expiry_interval;
	bool समुच्चयः_tcp_nodelay;
	पूर्णाङ्क sys_interval;
	bool upgrade_outgoing_qos;
	वर्ण *user;
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	पूर्णाङ्क websockets_log_level;
#यदि_अन्तः
	uint16_t packet_बफर_आकार;
#यदि_परिभाषितम् WITH_BRIDGE
	संरचना mosquitto__bridge **bridges;
	पूर्णाङ्क bridge_गणनम्;
#यदि_अन्तः
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
	संरचना mosquitto__subleaf *पूर्व;
	संरचना mosquitto__subleaf *अग्र;
	संरचना mosquitto *context;
	संरचना mosquitto__subhier *hier;
	संरचना mosquitto__subshared *shared;
	uint32_t identifier;
	uint8_t subscription_options;
	वर्ण topic_filter[];
};

संरचना sub__token {
	संरचना sub__token *अग्र;
	uint16_t topic_len;
	वर्ण topic[];
};

संरचना mosquitto__retainhier {
	UT_hash_handle hh;
	संरचना mosquitto__retainhier *parent;
	संरचना mosquitto__retainhier *children;
	संरचना mosquitto_आधारः_सन्देशः *retained;
	uint16_t topic_len;
	वर्ण topic[];
};

संरचना mosquitto_आधारः_सन्देशः {
	UT_hash_handle hh;
	संरचना mosquitto_आधारः_सन्देशः_2 दत्त;
	संरचना mosquitto__listener *source_listener;
	वर्ण **dest_ids;
	पूर्णाङ्क dest_id_गणनम्;
	पूर्णाङ्क ref_गणनम्;
	गणना mosquitto_सन्देशः_origin origin;
	bool stored;
};

संरचना mosquitto_client_सन्देशः {
	संरचना mosquitto_client_सन्देशः_2 दत्त;
	संरचना mosquitto_client_सन्देशः *पूर्व;
	संरचना mosquitto_client_सन्देशः *अग्र;
	संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः;
};


संरचना mosquitto__psk {
	UT_hash_handle hh;
	वर्ण *username;
	वर्ण *password;
};

संरचना mosquitto_सन्देशः_v5 {
	संरचना mosquitto_सन्देशः_v5 *अग्र, *पूर्व;
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
	dbid_t चरमः_db_id;
	uint64_t ग्रन्थिः_id_shifted;
	संरचना mosquitto__subhier *प्रसामान्यम्_subs;
	संरचना mosquitto__subhier *shared_subs;
	संरचना mosquitto__retainhier *retains;
	संरचना mosquitto *contexts_by_id;
	संरचना mosquitto *contexts_by_sock;
	संरचना mosquitto *contexts_by_id_delayed_auth;
	संरचना mosquitto *contexts_for_free;
	mosquitto_plugin_id_t **plugins;
	पूर्णाङ्क plugin_गणनम्;
#यदि_परिभाषितम् WITH_BRIDGE
	संरचना mosquitto **bridges;
	पूर्णाङ्क bridge_गणनम्;
#यदि_अन्तः
	संरचना clientid_सूचकांक_hash_2 *clientid_सूचकांक_hash;
	संरचना mosquitto_आधारः_सन्देशः *सन्देशः_store;
	time_t now_s; /* Monotonic clock, where possible */
	time_t now_real_s; /* Read clock, for measuring session/message expiry */
	uint64_t ग्रन्थिः_id; /* for unique db ids */
	time_t अग्र_event_ms; /* for mux timeout */
	पूर्णाङ्क सन्देशः_store_गणनम्;
	अचिह्नित दीर्घ सन्देशः_store_अष्टकानि;
	वर्ण *config_file;
	संरचना mosquitto__config *config;
	bool quiet;
	bool verbose;
#यदि_परिभाषितम् WITH_SYS_वृक्षः
	पूर्णाङ्क subscription_गणनम्;
	पूर्णाङ्क shared_subscription_गणनम्;
	पूर्णाङ्क retained_गणनम्;
#यदि_अन्तः
	पूर्णाङ्क persistence_changes;
	संरचना mosquitto *ll_for_free;
#यदि_परिभाषितम् WITH_EPOLL
	पूर्णाङ्क epollfd;
#यदि_अन्तः
#यदि_परिभाषितम् WITH_KQUEUE
	पूर्णाङ्क kqueuefd;
#यदि_अन्तः
	संरचना mosquitto_सन्देशः_v5 *plugin_msgs;
#यदि_परिभाषितम् WITH_TLS
	/* tls_keylog can't be in the config struct because it is used
	   before the config is allocated. Config probably
	   shouldn't be separately allocated. */
	वर्ण *tls_keylog;
#यदि_अन्तः
	bool shutdown;
};

गणना mosquitto__bridge_direction {
	bd_बहिर्गमनम् = 0,
	bd_अन्तर् = 1,
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
	brt_तात्कालिकम् = 1,
};

संरचना mosquitto__bridge_topic {
	संरचना mosquitto__bridge_topic *अग्र;
	वर्ण *topic;
	वर्ण *local_उपसर्गः;
	वर्ण *remote_उपसर्गः;
	वर्ण *local_topic; /* topic prefixed with local_prefix */
	वर्ण *remote_topic; /* topic prefixed with remote_prefix */
	गणना mosquitto__bridge_direction direction;
	uint8_t qos;
};

संरचना bridge_स्थानाङ्कः {
	वर्ण *स्थानाङ्कः;
	uint16_t पत्तनम्;
};

संरचना mosquitto__bridge {
	वर्ण *name;
	संरचना bridge_स्थानाङ्कः *स्थानाङ्काः;
	पूर्णाङ्क cur_स्थानाङ्कः;
	पूर्णाङ्क स्थानाङ्कः_गणनम्;
	time_t primary_retry;
	mosq_sock_t primary_retry_sock;
	bool आवर्तनम्_robin;
	bool try_वैयक्तिकम्;
	bool try_वैयक्तिकम्_accepted;
	bool clean_start;
	int8_t clean_start_local;
	uint16_t keepalive;
	अचिह्नित पूर्णाङ्क tcp_keepalive_idle;
	अचिह्नित पूर्णाङ्क tcp_keepalive_interval;
	अचिह्नित पूर्णाङ्क tcp_keepalive_गणक;
#यदि_परिभाषितम् WITH_TCP_USER_TIMEOUT
	पूर्णाङ्क tcp_user_timeout;
#यदि_अन्तः
	संरचना mosquitto__bridge_topic *topics;
	पूर्णाङ्क topic_गणनम्;
	bool topic_remapping;
	गणना mosquitto__protocol protocol_संस्करणम्;
	time_t restart_t;
	time_t connected_at;
	वर्ण *remote_clientid;
	वर्ण *remote_username;
	वर्ण *remote_password;
	वर्ण *local_clientid;
	वर्ण *local_username;
	वर्ण *local_password;
	वर्ण *notification_topic;
	वर्ण *bind_स्थानाङ्कः;
	bool notifications;
	bool notifications_local_only;
	गणना mosquitto_bridge_start_type start_type;
	पूर्णाङ्क idle_timeout;
	पूर्णाङ्क restart_timeout;
	पूर्णाङ्क backoff_आधारः;
	पूर्णाङ्क backoff_cap;
	पूर्णाङ्क stable_connection_period;
	पूर्णाङ्क देहली;
	uint32_t अधिकतमम्_packet_आकार;
	uint32_t session_expiry_interval;
	uint16_t receive_अधिकतमम्;
	bool lazy_reconnect;
	bool attempt_unsubscribe;
	bool initial_notification_समाप्तम्;
	bool outgoing_retain;
	गणना mosquitto_bridge_reload_type reload_type;
	uint16_t max_topic_alias;
#यदि_परिभाषितम् WITH_TLS
	bool tls_insecure;
	bool tls_ocsp_required;
	bool tls_use_os_certs;
	वर्ण *tls_cafile;
	वर्ण *tls_capath;
	वर्ण *tls_certfile;
	वर्ण *tls_keyfile;
	वर्ण *tls_संस्करणम्;
	वर्ण *tls_alpn;
	वर्ण *tls_ciphers;
	वर्ण *tls_13_ciphers;
#  ifdef FINAL_WITH_TLS_PSK
	वर्ण *tls_psk_तटस्थः;
	वर्ण *tls_psk;
#  endif
#यदि_अन्तः
};

#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
संरचना libws_mqtt_hack {
	वर्ण *http_dir;
	संरचना mosquitto__listener *listener;
};

संरचना libws_mqtt_दत्त {
	संरचना mosquitto *mosq;
};
#यदि_अन्तः

#अन्तर्भाव <net_mosq.h>


संरचना control_endpoint {
	संरचना control_endpoint *अग्र, *पूर्व;
	वर्ण topic[];
};

बाह्य संरचना mosquitto_db db;

/* ============================================================
 * Main functions
 * ============================================================ */
पूर्णाङ्क mosquitto_main_loop(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्);
शून्य loop_update_अग्र_event(time_t new_ms);

/* ============================================================
 * Config functions
 * ============================================================ */
/* Initialise config struct to default values. */
शून्य config__init(संरचना mosquitto__config *config);
#यदि_परिभाषितम् WITH_BRIDGE
शून्य config__bridge_cleanup(संरचना mosquitto__bridge *bridge);
#यदि_अन्तः
/* Parse command line options into config. */
पूर्णाङ्क config_parse_तर्काः(संरचना mosquitto__config *config, पूर्णाङ्क argc, वर्ण *argv[]);
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
पूर्णाङ्क send__connack(संरचना mosquitto *context, uint8_t ack, uint8_t reason_संहिता, ध्रुव mosquitto_property *properties);
पूर्णाङ्क send__suback(संरचना mosquitto *context, uint16_t mid, uint32_t payloadlen, ध्रुव शून्य *payload);
पूर्णाङ्क send__unsuback(संरचना mosquitto *context, uint16_t mid, पूर्णाङ्क reason_संहिता_गणनम्, uint8_t *reason_codes, ध्रुव mosquitto_property *properties);
पूर्णाङ्क send__auth(संरचना mosquitto *context, uint8_t reason_संहिता, ध्रुव शून्य *auth_दत्त, uint16_t auth_दत्त_len);

/* ============================================================
 * Network functions
 * ============================================================ */
शून्य net__broker_init(शून्य);
शून्य net__broker_cleanup(शून्य);
संरचना mosquitto *net__socket_accept(संरचना mosquitto__listener_sock *listensock);
पूर्णाङ्क net__socket_listen(संरचना mosquitto__listener *listener);
पूर्णाङ्क net_socket_get_स्थानाङ्कः(mosq_sock_t sock, वर्ण *buf, size_t len, uint16_t *remote_स्थानाङ्कः);
पूर्णाङ्क net_tls_आरोपय_verify(संरचना mosquitto__listener *listener);
पूर्णाङ्क net__tls_server_ctx(संरचना mosquitto__listener *listener);
पूर्णाङ्क net_आरोपय_certificates(संरचना mosquitto__listener *listener);

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
#यदि_परिभाषितम् WITH_PERSISTENCE
पूर्णाङ्क persist__backup(bool shutdown);
पूर्णाङ्क persist__restore(शून्य);
#यदि_अन्तः
/* Return the number of in-flight messages in count. */
पूर्णाङ्क db_सन्देशः_गणनम्(पूर्णाङ्क *गणनम्);
पूर्णाङ्क db_सन्देशः_delete_outgoing(संरचना mosquitto *context, uint16_t mid, गणना mosquitto_सन्देशः_state expect_state, पूर्णाङ्क qos);
पूर्णाङ्क db_सन्देशः_निवेशय_outgoing(संरचना mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः, uint32_t subscription_identifier, bool update, bool persist);
पूर्णाङ्क db_सन्देशः_निवेशय_incoming(संरचना mosquitto *context, uint64_t cmsg_id, संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः, bool persist);
पूर्णाङ्क db_सन्देशः_remove_incoming(संरचना mosquitto *context, uint16_t mid);
पूर्णाङ्क db_सन्देशः_release_incoming(संरचना mosquitto *context, uint16_t mid);
पूर्णाङ्क db_सन्देशः_update_outgoing(संरचना mosquitto *context, uint16_t mid, गणना mosquitto_सन्देशः_state state, पूर्णाङ्क qos, bool persist);
शून्य db_सन्देशः_पङ्क्तेः_निष्कासय_first(संरचना mosquitto *context, संरचना mosquitto_सन्देशः_दत्त *सन्देशः_दत्त);
पूर्णाङ्क db__messages_delete(संरचना mosquitto *context, bool force_free);
पूर्णाङ्क db__messages_delete_incoming(संरचना mosquitto *context);
पूर्णाङ्क db__messages_delete_outgoing(संरचना mosquitto *context);
पूर्णाङ्क db__messages_easy_queue(संरचना mosquitto *context, ध्रुव वर्ण *topic, uint8_t qos, uint32_t payloadlen, ध्रुव शून्य *payload, पूर्णाङ्क retain, uint32_t सन्देशः_expiry_interval, mosquitto_property **properties);
पूर्णाङ्क db_सन्देशः_store(ध्रुव संरचना mosquitto *source, संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः, uint32_t *सन्देशः_expiry_interval, गणना mosquitto_सन्देशः_origin origin);
पूर्णाङ्क db_सन्देशः_store_find(संरचना mosquitto *context, uint16_t mid, संरचना mosquitto_client_सन्देशः **client_सन्देशः);
पूर्णाङ्क db_सन्देशः_store_सङ्कलय(संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः);
शून्य db_सन्देशः_store_remove(संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः, bool notify);
शून्य db_सन्देशः_store_ref_inc(संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः);
शून्य db_सन्देशः_store_ref_dec(संरचना mosquitto_आधारः_सन्देशः **आधारः_सन्देशः);
शून्य db_सन्देशः_store_clean(शून्य);
शून्य db_सन्देशः_store_compact(शून्य);
शून्य db_सन्देशः_store_free(संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः);
पूर्णाङ्क db_सन्देशः_reconnect_reset(संरचना mosquitto *context);
bool db__ready_for_flight(संरचना mosquitto *context, गणना mosquitto_सन्देशः_direction dir, पूर्णाङ्क qos);
bool db__ready_for_queue(संरचना mosquitto *context, पूर्णाङ्क qos, संरचना mosquitto_सन्देशः_दत्त *सन्देशः_दत्त);
शून्य sys_वृक्षः_init(शून्य);
शून्य sys_वृक्षः_update(bool force);
पूर्णाङ्क db_सन्देशः_write_inflight_बहिर्गमनम्_all(संरचना mosquitto *context);
पूर्णाङ्क db_सन्देशः_write_inflight_बहिर्गमनम्_latest(संरचना mosquitto *context);
पूर्णाङ्क db_सन्देशः_write_queued_बहिर्गमनम्(संरचना mosquitto *context);
पूर्णाङ्क db_सन्देशः_write_queued_अन्तर्(संरचना mosquitto *context);
शून्य db_सन्देशः_सङ्कलय_गन्तव्यम्_inflight_stats(संरचना mosquitto_सन्देशः_दत्त *सन्देशः_दत्त, संरचना mosquitto_client_सन्देशः *सन्देशः);
शून्य db_सन्देशः_सङ्कलय_गन्तव्यम्_queued_stats(संरचना mosquitto_सन्देशः_दत्त *सन्देशः_दत्त, संरचना mosquitto_client_सन्देशः *सन्देशः);
uint64_t db_new_सन्देशः_id(शून्य);
शून्य db__expire_all_messages(संरचना mosquitto *context);
शून्य db_check_acl_सम्बन्धि_all_messages(संरचना mosquitto *context);

/* ============================================================
 * Subscription functions
 * ============================================================ */
पूर्णाङ्क sub__init(शून्य);
पूर्णाङ्क sub_सङ्कलय(संरचना mosquitto *context, ध्रुव संरचना mosquitto_subscription *sub);
पूर्णाङ्क sub__remove(संरचना mosquitto *context, ध्रुव वर्ण *sub, uint8_t *reason);
शून्य sub_वृक्षः_print(संरचना mosquitto__subhier *मूलम्, पूर्णाङ्क level);
पूर्णाङ्क sub__clean_session(संरचना mosquitto *context);
पूर्णाङ्क sub__messages_queue(ध्रुव वर्ण *source_id, uint8_t qos, संरचना mosquitto_आधारः_सन्देशः **आधारः_सन्देशः);
पूर्णाङ्क sub__topic_tokenise(ध्रुव वर्ण *subtopic, वर्ण **local_sub, वर्ण ***topics, ध्रुव वर्ण **sharename);
शून्य sub__topic_tokens_free(संरचना sub__token *tokens);

/* ============================================================
 * Context functions
 * ============================================================ */
संरचना mosquitto *context__init(शून्य);
पूर्णाङ्क context__init_sock(संरचना mosquitto *context, mosq_sock_t sock, bool get_स्थानाङ्कः);
शून्य context__cleanup(संरचना mosquitto *context, bool force_free);
शून्य context__disconnect(संरचना mosquitto *context, पूर्णाङ्क reason);
शून्य context_सङ्कलय_गन्तव्यम्_disused(संरचना mosquitto *context);
शून्य context__free_disused(शून्य);
शून्य context__send_will(संरचना mosquitto *context);
शून्य context_सङ्कलय_गन्तव्यम्_by_id(संरचना mosquitto *context);
शून्य context__remove_from_by_id(संरचना mosquitto *context);

पूर्णाङ्क connect__on_authorised(संरचना mosquitto *context, शून्य *auth_दत्त_बहिर्गमनम्, uint16_t auth_दत्त_बहिर्गमनम्_len);


/* ============================================================
 * Control functions
 * ============================================================ */
#यदि_परिभाषितम् WITH_CONTROL
पूर्णाङ्क control__process(संरचना mosquitto *context, संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः);
शून्य control__cleanup(शून्य);
#यदि_अन्तः
पूर्णाङ्क control_पञ्जिकाकोशः_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ध्रुव वर्ण *topic, शून्य *userdata);
पूर्णाङ्क control__unregister_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ध्रुव वर्ण *topic);
शून्य control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier);


/* ============================================================
 * Logging functions
 * ============================================================ */
पूर्णाङ्क log__init(संरचना mosquitto__config *config);
पूर्णाङ्क log__close(संरचना mosquitto__config *config);
शून्य log_आन्तरिकम्(ध्रुव वर्ण *fmt, ...) __attribute__((format(प्रारूप_लिख, 1, 2)));

/* ============================================================
 * Bridge functions
 * ============================================================ */
#यदि_परिभाषितम् WITH_BRIDGE
शून्य bridge__start_all(शून्य);
शून्य bridge__reload(शून्य);
शून्य bridge__db_cleanup(शून्य);
शून्य bridge__cleanup(संरचना mosquitto *context);
पूर्णाङ्क bridge__connect(संरचना mosquitto *context);
#यदि defined(__GLIBC__) && defined(WITH_ADNS)
पूर्णाङ्क bridge__connect_step3(संरचना mosquitto *context);
#यदि_अन्तः
पूर्णाङ्क bridge__on_connect(संरचना mosquitto *context);
शून्य bridge_check(शून्य);
पूर्णाङ्क bridge_पञ्जिकाकोशः_local_connections(शून्य);
पूर्णाङ्क bridge_सङ्कलय_topic(संरचना mosquitto__bridge *bridge, ध्रुव वर्ण *topic, गणना mosquitto__bridge_direction direction, uint8_t qos, ध्रुव वर्ण *local_उपसर्गः, ध्रुव वर्ण *remote_उपसर्गः);
शून्य bridge__cleanup_topics(संरचना mosquitto__bridge *bridge);
पूर्णाङ्क bridge_remap_topic_अन्तर्(संरचना mosquitto *context, वर्ण **topic);
#यदि_अन्तः

/* ============================================================
 * IO multiplex related functions
 * ============================================================ */
पूर्णाङ्क mux__init(शून्य);
पूर्णाङ्क mux_सङ्कलय_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्);
पूर्णाङ्क mux__delete_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्);
पूर्णाङ्क mux__loop_prepare(शून्य);
पूर्णाङ्क mux__new(संरचना mosquitto *context);
पूर्णाङ्क mux_सङ्कलय_बहिर्गमनम्(संरचना mosquitto *context);
पूर्णाङ्क mux_remove_बहिर्गमनम्(संरचना mosquitto *context);
पूर्णाङ्क mux__delete(संरचना mosquitto *context);
पूर्णाङ्क mux__wait(शून्य);
पूर्णाङ्क mux__handle(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्);
पूर्णाङ्क mux__cleanup(शून्य);

/* ============================================================
 * Listener related functions
 * ============================================================ */
बाह्य संरचना mosquitto__listener_sock *g_listensock;
बाह्य पूर्णाङ्क g_listensock_गणनम्;

शून्य listener_समुच्चयः_defaults(संरचना mosquitto__listener *listener);
शून्य listeners__reload_all_certificates(शून्य);
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
शून्य listeners_सङ्कलय_websockets(संरचना lws_context *ws_context, mosq_sock_t fd);
#यदि_अन्तः
पूर्णाङ्क listeners__start(शून्य);
शून्य listeners__stop(शून्य);

/* ============================================================
 * Plugin related functions
 * ============================================================ */
पूर्णाङ्क plugin_आरोपय_v5(mosquitto_plugin_id_t *plugin, शून्य *lib);
पूर्णाङ्क plugin_आरोपय_v4(mosquitto_plugin_id_t *plugin, शून्य *lib);
पूर्णाङ्क plugin_आरोपय_v3(mosquitto_plugin_id_t *plugin, शून्य *lib);
पूर्णाङ्क plugin_आरोपय_v2(mosquitto_plugin_id_t *plugin, शून्य *lib);
पूर्णाङ्क acl__pre_check(mosquitto_plugin_id_t *plugin, संरचना mosquitto *context, पूर्णाङ्क access);

शून्य LIB_ERROR(शून्य);
शून्य plugin__handle_connect(संरचना mosquitto *context);
शून्य plugin__handle_disconnect(संरचना mosquitto *context, पूर्णाङ्क reason);
शून्य plugin__handle_client_offline(संरचना mosquitto *context, पूर्णाङ्क reason);
पूर्णाङ्क plugin_handle_सन्देशः_अन्तर्(संरचना mosquitto *context, संरचना mosquitto_आधारः_सन्देशः_2 *आधारः_सन्देशः);
पूर्णाङ्क plugin_handle_सन्देशः_बहिर्गमनम्(संरचना mosquitto *context, संरचना mosquitto_आधारः_सन्देशः_2 *आधारः_सन्देशः);
पूर्णाङ्क plugin__handle_subscribe(संरचना mosquitto *context, संरचना mosquitto_subscription *sub);
पूर्णाङ्क plugin__handle_unsubscribe(संरचना mosquitto *context, संरचना mosquitto_subscription *sub);
पूर्णाङ्क plugin__handle_reload(शून्य);
शून्य plugin__handle_tick(शून्य);
पूर्णाङ्क plugin__callback_unregister_all(mosquitto_plugin_id_t *identifier);
शून्य plugin_persist__handle_restore(शून्य);
शून्य plugin_persist_handle_client_सङ्कलय(संरचना mosquitto *context);
शून्य plugin_persist__handle_client_delete(संरचना mosquitto *context);
शून्य plugin_persist__handle_client_update(संरचना mosquitto *context);
शून्य plugin_persist_handle_subscription_सङ्कलय(संरचना mosquitto *context, ध्रुव संरचना mosquitto_subscription *sub);
शून्य plugin_persist__handle_subscription_delete(संरचना mosquitto *context, वर्ण *sub);
शून्य plugin_persist_handle_client_सन्देशः_सङ्कलय(संरचना mosquitto *context, ध्रुव संरचना mosquitto_client_सन्देशः *cmsg);
शून्य plugin_persist_handle_client_सन्देशः_delete(संरचना mosquitto *context, ध्रुव संरचना mosquitto_client_सन्देशः *cmsg);
शून्य plugin_persist_handle_client_सन्देशः_update(संरचना mosquitto *context, ध्रुव संरचना mosquitto_client_सन्देशः *cmsg);
शून्य plugin_persist_handle_आधारः_सन्देशः_सङ्कलय(संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः);
शून्य plugin_persist_handle_आधारः_सन्देशः_delete(संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः);
शून्य plugin_persist_handle_retain_सन्देशः_समुच्चयः(संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः);
शून्य plugin_persist_handle_retain_सन्देशः_delete(संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः);
शून्य plugin_persist_handle_will_सङ्कलय(संरचना mosquitto *context);
शून्य plugin_persist__handle_will_delete(संरचना mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
पूर्णाङ्क keepalive__init(शून्य);
शून्य keepalive__cleanup(शून्य);
पूर्णाङ्क keepalive_सङ्कलय(संरचना mosquitto *context);
शून्य keepalive__check(शून्य);
पूर्णाङ्क keepalive__remove(संरचना mosquitto *context);
पूर्णाङ्क keepalive__update(संरचना mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
पूर्णाङ्क property__process_connect(संरचना mosquitto *context, mosquitto_property **props);
पूर्णाङ्क property__process_will(संरचना mosquitto *context, संरचना mosquitto_सन्देशः_all *सन्देशः, mosquitto_property **props);
पूर्णाङ्क property__process_publish(संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः, mosquitto_property **props, पूर्णाङ्क *topic_alias, uint32_t *सन्देशः_expiry_interval, bool is_bridge);
पूर्णाङ्क property__process_disconnect(संरचना mosquitto *context, mosquitto_property **props);

/* ============================================================
 * Retain tree related functions
 * ============================================================ */
पूर्णाङ्क retain__init(शून्य);
शून्य retain__clean(संरचना mosquitto__retainhier **retainhier);
पूर्णाङ्क retain__queue(संरचना mosquitto *context, ध्रुव संरचना mosquitto_subscription *sub);
पूर्णाङ्क retain__store(ध्रुव वर्ण *topic, संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः, वर्ण **विभज_topics, bool persist);
शून्य retain__expiry_check(शून्य);
शून्य retain__expire(संरचना mosquitto__retainhier **retainhier);

/* ============================================================
 * Security related functions
 * ============================================================ */
पूर्णाङ्क acl__find_acls(संरचना mosquitto *context);
पूर्णाङ्क plugin_आरोपय_all(शून्य);
पूर्णाङ्क plugin__unload_all(शून्य);
पूर्णाङ्क config_plugin_सङ्कलय_secopt(mosquitto_plugin_id_t *plugin, संरचना mosquitto__security_options *security_options);

पूर्णाङ्क mosquitto_security_init(bool reload);
पूर्णाङ्क mosquitto_security_cleanup(bool reload);
पूर्णाङ्क mosquitto_acl_check(संरचना mosquitto *context, ध्रुव वर्ण *topic, uint32_t payloadlen, शून्य *payload, uint8_t qos, bool retain, mosquitto_property *properties, पूर्णाङ्क access);
पूर्णाङ्क mosquitto_basic_auth(संरचना mosquitto *context);
पूर्णाङ्क mosquitto_psk_कुञ्चिका_get(संरचना mosquitto *context, ध्रुव वर्ण *hint, ध्रुव वर्ण *तटस्थः, वर्ण *कुञ्चिका, पूर्णाङ्क max_कुञ्चिका_len);

पूर्णाङ्क mosquitto_security_init_default(शून्य);
पूर्णाङ्क mosquitto_security_apply_default(शून्य);
पूर्णाङ्क mosquitto_security_cleanup_default(शून्य);
पूर्णाङ्क mosquitto_psk_कुञ्चिका_get_default(संरचना mosquitto *context, ध्रुव वर्ण *hint, ध्रुव वर्ण *तटस्थः, वर्ण *कुञ्चिका, पूर्णाङ्क max_कुञ्चिका_len);
पूर्णाङ्क broker_acl_file__init(शून्य);
शून्य broker_acl_file__cleanup(शून्य);
पूर्णाङ्क broker_password_file__init(शून्य);
शून्य broker_password_file__cleanup(शून्य);
पूर्णाङ्क psk_file__init(शून्य);
पूर्णाङ्क psk_file__cleanup(शून्य);

पूर्णाङ्क mosquitto_security_auth_start(संरचना mosquitto *context, bool reauth, ध्रुव शून्य *दत्त_अन्तर्, uint16_t दत्त_अन्तर्_len, शून्य **दत्त_बहिर्गमनम्, uint16_t *दत्त_बहिर्गमनम्_len);
पूर्णाङ्क mosquitto_security_auth_continue(संरचना mosquitto *context, ध्रुव शून्य *दत्त_अन्तर्, uint16_t दत्त_len, शून्य **दत्त_बहिर्गमनम्, uint16_t *दत्त_बहिर्गमनम्_len);

शून्य unpwd__free_item(संरचना mosquitto__unpwd **unpwd, संरचना mosquitto__unpwd *item);

/* ============================================================
 * Session expiry
 * ============================================================ */
पूर्णाङ्क session_expiry_सङ्कलय(संरचना mosquitto *context);
पूर्णाङ्क session_expiry_सङ्कलय_from_persistence(संरचना mosquitto *context, time_t expiry_time);
शून्य session_expiry_सङ्कलय_on_shutdown(शून्य);
शून्य session_expiry__remove(संरचना mosquitto *context);
शून्य session_expiry__remove_all(शून्य);
शून्य session_expiry__check(शून्य);
शून्य session_expiry__send_all(शून्य);

/* ============================================================
 * Signals
 * ============================================================ */
शून्य signal_सज्जता(शून्य);
पूर्णाङ्क signal__flag_check(शून्य);

/* ============================================================
 * Window service and signal related functions
 * ============================================================ */
#यदि defined(WIN32) || defined(__CYGWIN__)
शून्य service_install(वर्ण *name);
शून्य service_uninstall(वर्ण *name);
शून्य service_सततम्(वर्ण *name);

#यदि_परिभाषितम् WIN32
DWORD WINAPI SigThreadProc(शून्य *दत्त);
#यदि_अन्तः
#यदि_अन्तः

/* ============================================================
 * Watchdog
 * ============================================================ */
शून्य watchdog__init(शून्य);
शून्य watchdog__check(शून्य);

/* ============================================================
 * Websockets related functions
 * ============================================================ */
#यदि_परिभाषितम् WITH_WEBSOCKETS
#  यदि WITH_WEBSOCKETS == WS_IS_LWS
शून्य mosq_websockets_init(संरचना mosquitto__listener *listener, ध्रुव संरचना mosquitto__config *conf);
#  endif
पूर्णाङ्क http__context_init(संरचना mosquitto *context);
पूर्णाङ्क http__context_cleanup(संरचना mosquitto *context);
पूर्णाङ्क http__read(संरचना mosquitto *context);
#यदि_अन्तः
शून्य do_disconnect(संरचना mosquitto *context, पूर्णाङ्क reason);

/* ============================================================
 * PROXY related functions
 * ============================================================ */
पूर्णाङ्क proxy_v2__read(संरचना mosquitto *context);
पूर्णाङ्क proxy_v1__read(संरचना mosquitto *context);

/* ============================================================
 * Will delay
 * ============================================================ */
पूर्णाङ्क will_delay_सङ्कलय(संरचना mosquitto *context);
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
#यदि_परिभाषितम् WITH_XTREPORT
शून्य xtreport(शून्य);
#यदि_अन्तः

शून्य broker_control__init(शून्य);
शून्य broker_control__cleanup(शून्य);
शून्य broker_control__reload(शून्य);

#यदि_अन्तः
