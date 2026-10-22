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

#నిర్వచితం_కాకపోతే MOSQUITTO_BROKER_అంతర్గత_H
#నిర్వచించు MOSQUITTO_BROKER_అంతర్గత_H

#అన్తర్భావ "config.h"
#అన్తర్భావ <stdio.h>

#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#  ఒకవేళ LWS_LIBRARY_సంస్కరణ_సంఖ్య >= 3002000 && !defined(LWS_WITH_EXTERNAL_POLL)
#    warning "libwebsockets is not compiled with LWS_WITH_EXTERNAL_POLL support. Websocket performance will be unusable."
#  endif
#అయితే_ముగింపు

#నిర్వచితమైతే WITH_HTTP_API
#  include <microhttpd.h>
#అయితే_ముగింపు

#నిర్వచితమైతే __linux__
#నిర్వచించు WITH_TCP_USER_TIMEOUT
#అయితే_ముగింపు

#అన్తర్భావ "mosquitto_internal.h"
#అన్తర్భావ "mosquitto/broker.h"
#అన్తర్భావ "mosquitto/broker_plugin.h"
#అన్తర్భావ "mosquitto.h"
#అన్తర్భావ "logging_mosq.h"
#అన్తర్భావ "tls_mosq.h"
#అన్తర్భావ "uthash.h"
#అన్తర్భావ "acl_file.h"
#అన్తర్భావ "password_file.h"

#నిర్వచితం_కాకపోతే __GNUC__
#నిర్వచించు __attribute__(attrib)
#అయితే_ముగింపు

/* Log destinations */
#నిర్వచించు MQTT3_LOG_NONE 0x00
#నిర్వచించు MQTT3_LOG_SYSLOG 0x01
#నిర్వచించు MQTT3_LOG_FILE 0x02
#నిర్వచించు MQTT3_LOG_STDOUT 0x04
#నిర్వచించు MQTT3_LOG_STDERR 0x08
#నిర్వచించు MQTT3_LOG_TOPIC 0x10
#నిర్వచించు MQTT3_LOG_DLT 0x20
#నిర్వచితమైతే ANDROID
#నిర్వచించు MQTT3_LOG_ANDROID 0x40
#అయితే_ముగింపు
#నిర్వచించు MQTT3_LOG_ALL 0xFF

#నిర్వచించు CMD_పోర్టు_హద్దు 10

రకనిర్వచనం uint64_t dbid_t;

రకనిర్వచనం పూర్ణాంకం (*FUNC_plugin_init_v5)(mosquitto_plugin_id_t *, శూన్య **, నిర్మాణం mosquitto_opt *, పూర్ణాంకం);
రకనిర్వచనం పూర్ణాంకం (*FUNC_plugin_cleanup_v5)(శూన్య *, నిర్మాణం mosquitto_opt *, పూర్ణాంకం);

రకనిర్వచనం పూర్ణాంకం (*FUNC_auth_plugin_init_v4)(శూన్య **, నిర్మాణం mosquitto_opt *, పూర్ణాంకం);
రకనిర్వచనం పూర్ణాంకం (*FUNC_auth_plugin_cleanup_v4)(శూన్య *, నిర్మాణం mosquitto_opt *, పూర్ణాంకం);
రకనిర్వచనం పూర్ణాంకం (*FUNC_auth_plugin_security_init_v4)(శూన్య *, నిర్మాణం mosquitto_opt *, పూర్ణాంకం, bool);
రకనిర్వచనం పూర్ణాంకం (*FUNC_auth_plugin_security_cleanup_v4)(శూన్య *, నిర్మాణం mosquitto_opt *, పూర్ణాంకం, bool);
రకనిర్వచనం పూర్ణాంకం (*FUNC_auth_plugin_acl_check_v4)(శూన్య *, పూర్ణాంకం, నిర్మాణం mosquitto *, నిర్మాణం mosquitto_acl_సందేశం *);
రకనిర్వచనం పూర్ణాంకం (*FUNC_auth_plugin_unpwd_check_v4)(శూన్య *, నిర్మాణం mosquitto *, స్థిరరాశి అక్షరం *, స్థిరరాశి అక్షరం *);
రకనిర్వచనం పూర్ణాంకం (*FUNC_auth_plugin_psk_కీలకం_get_v4)(శూన్య *, నిర్మాణం mosquitto *, స్థిరరాశి అక్షరం *, స్థిరరాశి అక్షరం *, అక్షరం *, పూర్ణాంకం);
రకనిర్వచనం పూర్ణాంకం (*FUNC_auth_plugin_auth_ప్రారంభం_v4)(శూన్య *, నిర్మాణం mosquitto *, స్థిరరాశి అక్షరం *, bool, స్థిరరాశి శూన్య *, uint16_t, శూన్య **, uint16_t *);
రకనిర్వచనం పూర్ణాంకం (*FUNC_auth_plugin_auth_continue_v4)(శూన్య *, నిర్మాణం mosquitto *, స్థిరరాశి అక్షరం *, స్థిరరాశి శూన్య *, uint16_t, శూన్య **, uint16_t *);

రకనిర్వచనం పూర్ణాంకం (*FUNC_auth_plugin_init_v3)(శూన్య **, నిర్మాణం mosquitto_opt *, పూర్ణాంకం);
రకనిర్వచనం పూర్ణాంకం (*FUNC_auth_plugin_cleanup_v3)(శూన్య *, నిర్మాణం mosquitto_opt *, పూర్ణాంకం);
రకనిర్వచనం పూర్ణాంకం (*FUNC_auth_plugin_security_init_v3)(శూన్య *, నిర్మాణం mosquitto_opt *, పూర్ణాంకం, bool);
రకనిర్వచనం పూర్ణాంకం (*FUNC_auth_plugin_security_cleanup_v3)(శూన్య *, నిర్మాణం mosquitto_opt *, పూర్ణాంకం, bool);
రకనిర్వచనం పూర్ణాంకం (*FUNC_auth_plugin_acl_check_v3)(శూన్య *, పూర్ణాంకం, స్థిరరాశి నిర్మాణం mosquitto *, నిర్మాణం mosquitto_acl_సందేశం *);
రకనిర్వచనం పూర్ణాంకం (*FUNC_auth_plugin_unpwd_check_v3)(శూన్య *, స్థిరరాశి నిర్మాణం mosquitto *, స్థిరరాశి అక్షరం *, స్థిరరాశి అక్షరం *);
రకనిర్వచనం పూర్ణాంకం (*FUNC_auth_plugin_psk_కీలకం_get_v3)(శూన్య *, స్థిరరాశి నిర్మాణం mosquitto *, స్థిరరాశి అక్షరం *, స్థిరరాశి అక్షరం *, అక్షరం *, పూర్ణాంకం);

రకనిర్వచనం పూర్ణాంకం (*FUNC_auth_plugin_init_v2)(శూన్య **, నిర్మాణం mosquitto_auth_opt *, పూర్ణాంకం);
రకనిర్వచనం పూర్ణాంకం (*FUNC_auth_plugin_cleanup_v2)(శూన్య *, నిర్మాణం mosquitto_auth_opt *, పూర్ణాంకం);
రకనిర్వచనం పూర్ణాంకం (*FUNC_auth_plugin_security_init_v2)(శూన్య *, నిర్మాణం mosquitto_auth_opt *, పూర్ణాంకం, bool);
రకనిర్వచనం పూర్ణాంకం (*FUNC_auth_plugin_security_cleanup_v2)(శూన్య *, నిర్మాణం mosquitto_auth_opt *, పూర్ణాంకం, bool);
రకనిర్వచనం పూర్ణాంకం (*FUNC_auth_plugin_acl_check_v2)(శూన్య *, స్థిరరాశి అక్షరం *, స్థిరరాశి అక్షరం *, స్థిరరాశి అక్షరం *, పూర్ణాంకం);
రకనిర్వచనం పూర్ణాంకం (*FUNC_auth_plugin_unpwd_check_v2)(శూన్య *, స్థిరరాశి అక్షరం *, స్థిరరాశి అక్షరం *);
రకనిర్వచనం పూర్ణాంకం (*FUNC_auth_plugin_psk_కీలకం_get_v2)(శూన్య *, స్థిరరాశి అక్షరం *, స్థిరరాశి అక్షరం *, అక్షరం *, పూర్ణాంకం);


గణన mosquitto_సందేశం_origin {
	mosq_mo_client = 0,
	mosq_mo_broker = 1,
};

నిర్మాణం mosquitto__plugin_lib {
	శూన్య *lib;
	శూన్య *user_దత్తాంశం;
	పూర్ణాంకం (*plugin_సంస్కరణ)(శూన్య);
	నిర్మాణం mosquitto_plugin_id_t *identifier;

	FUNC_plugin_init_v5 plugin_init_v5;
	FUNC_plugin_cleanup_v5 plugin_cleanup_v5;

	FUNC_auth_plugin_init_v4 plugin_init_v4;
	FUNC_auth_plugin_cleanup_v4 plugin_cleanup_v4;
	FUNC_auth_plugin_security_init_v4 security_init_v4;
	FUNC_auth_plugin_security_cleanup_v4 security_cleanup_v4;
	FUNC_auth_plugin_acl_check_v4 acl_check_v4;
	FUNC_auth_plugin_unpwd_check_v4 unpwd_check_v4;
	FUNC_auth_plugin_psk_కీలకం_get_v4 psk_కీలకం_get_v4;
	FUNC_auth_plugin_auth_ప్రారంభం_v4 auth_ప్రారంభం_v4;
	FUNC_auth_plugin_auth_continue_v4 auth_continue_v4;

	FUNC_auth_plugin_init_v3 plugin_init_v3;
	FUNC_auth_plugin_cleanup_v3 plugin_cleanup_v3;
	FUNC_auth_plugin_security_init_v3 security_init_v3;
	FUNC_auth_plugin_security_cleanup_v3 security_cleanup_v3;
	FUNC_auth_plugin_acl_check_v3 acl_check_v3;
	FUNC_auth_plugin_unpwd_check_v3 unpwd_check_v3;
	FUNC_auth_plugin_psk_కీలకం_get_v3 psk_కీలకం_get_v3;

	FUNC_auth_plugin_init_v2 plugin_init_v2;
	FUNC_auth_plugin_cleanup_v2 plugin_cleanup_v2;
	FUNC_auth_plugin_security_init_v2 security_init_v2;
	FUNC_auth_plugin_security_cleanup_v2 security_cleanup_v2;
	FUNC_auth_plugin_acl_check_v2 acl_check_v2;
	FUNC_auth_plugin_unpwd_check_v2 unpwd_check_v2;
	FUNC_auth_plugin_psk_కీలకం_get_v2 psk_కీలకం_get_v2;
	పూర్ణాంకం సంస్కరణ_2;
};

నిర్మాణం mosquitto__plugin_config {
	అక్షరం *మార్గం;
	అక్షరం *name;
	నిర్మాణం mosquitto_opt *options;
	నిర్మాణం mosquitto__security_options **security_options;
	పూర్ణాంకం option_లెక్క;
	పూర్ణాంకం security_option_లెక్క;
	bool deny_special_chars;
};

నిర్మాణం mosquitto__callback {
	UT_hash_handle hh; /* For callbacks that register for e.g. a specific topic */
	నిర్మాణం mosquitto__callback *తదుపరి, *prev; /* For typical callbacks */
	MOSQ_FUNC_generic_callback cb;
	శూన్య *userdata;
	సంఘం {
		అక్షరం *topic;
		నిర్మాణం timespec తదుపరి_tick;
	} దత్తాంశం;
	mosquitto_plugin_id_t *identifier;
};

నిర్మాణం plugin__callbacks {
	నిర్మాణం mosquitto__callback *tick;
	నిర్మాణం mosquitto__callback *acl_check;
	నిర్మాణం mosquitto__callback *basic_auth;
	నిర్మాణం mosquitto__callback *connect;
	నిర్మాణం mosquitto__callback *control;
	నిర్మాణం mosquitto__callback *disconnect;
	నిర్మాణం mosquitto__callback *client_offline;
	నిర్మాణం mosquitto__callback *ext_auth_continue;
	నిర్మాణం mosquitto__callback *ext_auth_ప్రారంభం;
	నిర్మాణం mosquitto__callback *సందేశం_లోపల;
	నిర్మాణం mosquitto__callback *సందేశం_బహిర్గతం;
	నిర్మాణం mosquitto__callback *psk_కీలకం;
	నిర్మాణం mosquitto__callback *reload;
	నిర్మాణం mosquitto__callback *subscribe;
	నిర్మాణం mosquitto__callback *unsubscribe;
	నిర్మాణం mosquitto__callback *persist_restore;
	నిర్మాణం mosquitto__callback *persist_client_కూడు;
	నిర్మాణం mosquitto__callback *persist_client_delete;
	నిర్మాణం mosquitto__callback *persist_client_update;
	నిర్మాణం mosquitto__callback *persist_subscription_కూడు;
	నిర్మాణం mosquitto__callback *persist_subscription_delete;
	నిర్మాణం mosquitto__callback *persist_client_సందేశం_కూడు;
	నిర్మాణం mosquitto__callback *persist_client_సందేశం_delete;
	నిర్మాణం mosquitto__callback *persist_client_సందేశం_update;
	నిర్మాణం mosquitto__callback *persist_పునాది_సందేశం_కూడు;
	నిర్మాణం mosquitto__callback *persist_పునాది_సందేశం_delete;
	నిర్మాణం mosquitto__callback *persist_retain_సందేశం_సమితి;
	నిర్మాణం mosquitto__callback *persist_retain_సందేశం_delete;
	నిర్మాణం mosquitto__callback *persist_will_కూడు;
	నిర్మాణం mosquitto__callback *persist_will_delete;
};

/* This is owned by mosquitto__config or mosquitto__listener, and only referred
 * to by other structs */
నిర్మాణం mosquitto__security_options {
	/* Any options that get added here also need considering
	 * in config__read() with regards whether allow_anonymous
	 * should be disabled when these options are set.
	 */
	నిర్మాణం mosquitto__unpwd *unpwd;
	నిర్మాణం mosquitto__psk *psk_id;
	నిర్మాణం acl_file_దత్తాంశం acl_దత్తాంశం;
	నిర్మాణం password_file_దత్తాంశం password_దత్తాంశం;
	అక్షరం *psk_file;
	mosquitto_plugin_id_t **plugins;
	పూర్ణాంకం plugin_లెక్క;
	int8_t allow_anonymous;
	bool allow_zero_నిడివి_clientid;
	అక్షరం *auto_id_ఉపసర్గ;
	uint16_t auto_id_ఉపసర్గ_len;
	నిర్మాణం plugin__callbacks plugin_callbacks;
	mosquitto_plugin_id_t *pid; /* For registering as a "plugin" */
};

#ఒకవేళ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
గణన struct_ident {
	id_invalid = 0,
	id_listener = 1,
	id_client = 2,
	id_listener_ws = 3,
};
#అయితే_ముగింపు

నిర్మాణం mosquitto__listener {
	uint16_t పోర్టు;
	అక్షరం *host;
	అక్షరం *bind_interface;
	పూర్ణాంకం max_connections;
	అక్షరం *mount_బిందువు;
	mosq_sock_t *socks;
	పూర్ణాంకం sock_లెక్క;
	పూర్ణాంకం client_లెక్క;
	గణన mosquitto_protocol protocol;
	పూర్ణాంకం socket_domain;
	bool use_username_as_clientid;
	uint8_t max_qos;
	uint16_t max_topic_alias;
	uint16_t max_topic_alias_broker;
#నిర్వచితమైతే WITH_TLS
	అక్షరం *cafile;
	అక్షరం *capath;
	అక్షరం *certfile;
	అక్షరం *keyfile;
	అక్షరం *tls_engine;
	అక్షరం *tls_engine_kpass_sha1;
	అక్షరం *ciphers;
	అక్షరం *ciphers_tls13;
	అక్షరం *psk_hint;
	SSL_CTX *ssl_ctx;
	అక్షరం *crlfile;
	అక్షరం *tls_సంస్కరణ;
	bool use_తటస్థం_as_username;
	bool use_subject_as_username;
	bool require_certificate;
	bool disable_client_cert_date_checks;
	గణన mosquitto__keyform tls_keyform;
#అయితే_ముగింపు
#నిర్వచితమైతే WITH_WEBSOCKETS
#  ఒకవేళ WITH_WEBSOCKETS == WS_IS_LWS
	నిర్మాణం lws_context *ws_context;
	bool ws_లోపల_init;
	నిర్మాణం lws_protocols *ws_protocol;
#  endif
	అక్షరం **ws_origins;
	పూర్ణాంకం ws_origin_లెక్క;
#అయితే_ముగింపు
#ఒకవేళ defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
	అక్షరం *http_dir;
#అయితే_ముగింపు
	నిర్మాణం mosquitto__security_options *security_options;
#నిర్వచితమైతే WITH_UNIX_SOCKETS
	అక్షరం *unix_socket_మార్గం;
#అయితే_ముగింపు
	bool disable_protocol_v3;
	bool disable_protocol_v4;
	bool disable_protocol_v5;
	పూర్ణాంకం enable_proxy_protocol;
	bool proxy_protocol_v2_require_tls;
#నిర్వచితమైతే WITH_HTTP_API
	నిర్మాణం MHD_Daemon *mhd;
#అయితే_ముగింపు
};


నిర్మాణం mosquitto__listener_sock {
#ఒకవేళ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	/* This *must* be the first element in the struct. */
	పూర్ణాంకం ident;
#అయితే_ముగింపు
	mosq_sock_t sock;
	నిర్మాణం mosquitto__listener *listener;
};


/* Callbacks belonging to a specific plugin
 * Doesn't include MOSQ_EVT_CONTROL events.
 */
నిర్మాణం plugin_own_callback {
	నిర్మాణం plugin_own_callback *తదుపరి, *prev;
	MOSQ_FUNC_generic_callback cb_func;
	గణన mosquitto_plugin_event event;
};

నిర్మాణం mosquitto_plugin_id_t {
	నిర్మాణం mosquitto__plugin_config config;
	నిర్మాణం mosquitto__plugin_lib lib;
	నిర్మాణం mosquitto__listener *listener;
	అక్షరం *plugin_name;
	అక్షరం *plugin_సంస్కరణ;
	నిర్మాణం control_endpoint *control_endpoints;
	నిర్మాణం plugin_own_callback *own_callbacks;
	నిర్మాణం timespec తదుపరి_tick;
};

నిర్మాణం mosquitto__config {
	bool allow_duplicate_messages;
	పూర్ణాంకం autosave_interval;
	bool autosave_on_changes;
	bool check_retain_ఆధారం;
	అక్షరం *clientid_prefixes;
	bool connection_messages;
	uint16_t cmd_పోర్టు[CMD_పోర్టు_హద్దు];
	పూర్ణాంకం cmd_పోర్టు_లెక్క;
	bool daemon;
	bool test_configuration;
	bool enable_control_api;
	పూర్ణాంకం global_max_clients;
	పూర్ణాంకం global_max_connections;
	నిర్మాణం mosquitto__listener *default_listener; /* Points to one of `listeners` */
	నిర్మాణం mosquitto__listener *listeners;
	పూర్ణాంకం listener_లెక్క;
	bool local_only;
	గుర్తులేని పూర్ణాంకం log_dest;
	పూర్ణాంకం log_facility;
	గుర్తులేని పూర్ణాంకం log_type;
	bool log_timestamp;
	అక్షరం *log_timestamp_format;
	అక్షరం *log_file;
	FILE *log_fptr;
	size_t max_inflight_బైట్లు;
	size_t max_queued_బైట్లు;
	పూర్ణాంకం max_queued_messages;
	uint32_t max_packet_కొలత;
	uint32_t సందేశం_కొలత_హద్దు;
	uint16_t max_inflight_messages;
	uint16_t max_keepalive;
	uint8_t max_qos;
	uint32_t packet_max_connect;
	uint32_t packet_max_simple;
	uint32_t packet_max_sub;
	uint32_t packet_max_auth;
	bool persistence;
	అక్షరం *persistence_location;
	అక్షరం *persistence_file;
	అక్షరం *persistence_filepath;
	time_t persistent_client_expiration;
	అక్షరం *pid_file;
	bool వరుస_qos0_messages;
	bool చొప్పున_listener_settings;
	bool retain_అందుబాటు;
	పూర్ణాంకం retain_expiry_interval;
	bool సమితి_tcp_nodelay;
	పూర్ణాంకం sys_interval;
	bool upgrade_outgoing_qos;
	అక్షరం *user;
#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	పూర్ణాంకం websockets_log_level;
#అయితే_ముగింపు
	uint16_t packet_బఫరు_కొలత;
#నిర్వచితమైతే WITH_BRIDGE
	నిర్మాణం mosquitto__bridge **bridges;
	పూర్ణాంకం bridge_లెక్క;
#అయితే_ముగింపు
	నిర్మాణం mosquitto__security_options security_options;
};


నిర్మాణం mosquitto__subshared {
	UT_hash_handle hh;
	నిర్మాణం mosquitto__subleaf *subs;
	అక్షరం name[];
};

నిర్మాణం mosquitto__subhier {
	UT_hash_handle hh;
	నిర్మాణం mosquitto__subhier *మాతృ;
	నిర్మాణం mosquitto__subhier *children;
	నిర్మాణం mosquitto__subleaf *subs;
	నిర్మాణం mosquitto__subshared *shared;
	uint16_t topic_len;
	అక్షరం topic[];
};

నిర్మాణం mosquitto__subleaf {
	నిర్మాణం mosquitto__subleaf *prev;
	నిర్మాణం mosquitto__subleaf *తదుపరి;
	నిర్మాణం mosquitto *context;
	నిర్మాణం mosquitto__subhier *hier;
	నిర్మాణం mosquitto__subshared *shared;
	uint32_t identifier;
	uint8_t subscription_options;
	అక్షరం topic_filter[];
};

నిర్మాణం sub__token {
	నిర్మాణం sub__token *తదుపరి;
	uint16_t topic_len;
	అక్షరం topic[];
};

నిర్మాణం mosquitto__retainhier {
	UT_hash_handle hh;
	నిర్మాణం mosquitto__retainhier *మాతృ;
	నిర్మాణం mosquitto__retainhier *children;
	నిర్మాణం mosquitto_పునాది_సందేశం *retained;
	uint16_t topic_len;
	అక్షరం topic[];
};

నిర్మాణం mosquitto_పునాది_సందేశం {
	UT_hash_handle hh;
	నిర్మాణం mosquitto_పునాది_సందేశం_2 దత్తాంశం;
	నిర్మాణం mosquitto__listener *ఆధారం_listener;
	అక్షరం **dest_ids;
	పూర్ణాంకం dest_id_లెక్క;
	పూర్ణాంకం ref_లెక్క;
	గణన mosquitto_సందేశం_origin origin;
	bool stored;
};

నిర్మాణం mosquitto_client_సందేశం {
	నిర్మాణం mosquitto_client_సందేశం_2 దత్తాంశం;
	నిర్మాణం mosquitto_client_సందేశం *prev;
	నిర్మాణం mosquitto_client_సందేశం *తదుపరి;
	నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం;
};


నిర్మాణం mosquitto__psk {
	UT_hash_handle hh;
	అక్షరం *username;
	అక్షరం *password;
};

నిర్మాణం mosquitto_సందేశం_v5 {
	నిర్మాణం mosquitto_సందేశం_v5 *తదుపరి, *prev;
	అక్షరం *topic;
	శూన్య *payload;
	mosquitto_property *properties;
	/* clientid is used only by mosquitto_broker_publish*() to indicate
	   this message is for a specific client. */
	అక్షరం *clientid;
	పూర్ణాంకం payloadlen;
	పూర్ణాంకం qos;
	bool retain;
};

నిర్మాణం mosquitto_db {
	dbid_t చివరి_db_id;
	uint64_t కణుపు_id_shifted;
	నిర్మాణం mosquitto__subhier *సాధారణ_subs;
	నిర్మాణం mosquitto__subhier *shared_subs;
	నిర్మాణం mosquitto__retainhier *retains;
	నిర్మాణం mosquitto *contexts_by_id;
	నిర్మాణం mosquitto *contexts_by_sock;
	నిర్మాణం mosquitto *contexts_by_id_delayed_auth;
	నిర్మాణం mosquitto *contexts_for_free;
	mosquitto_plugin_id_t **plugins;
	పూర్ణాంకం plugin_లెక్క;
#నిర్వచితమైతే WITH_BRIDGE
	నిర్మాణం mosquitto **bridges;
	పూర్ణాంకం bridge_లెక్క;
#అయితే_ముగింపు
	నిర్మాణం clientid_సూచీ_hash_2 *clientid_సూచీ_hash;
	నిర్మాణం mosquitto_పునాది_సందేశం *సందేశం_store;
	time_t now_s; /* Monotonic clock, where possible */
	time_t now_real_s; /* Read clock, for measuring session/message expiry */
	uint64_t కణుపు_id; /* for unique db ids */
	time_t తదుపరి_event_ms; /* for mux timeout */
	పూర్ణాంకం సందేశం_store_లెక్క;
	గుర్తులేని దీర్ఘ సందేశం_store_బైట్లు;
	అక్షరం *config_file;
	నిర్మాణం mosquitto__config *config;
	bool quiet;
	bool verbose;
#నిర్వచితమైతే WITH_SYS_వృక్షం
	పూర్ణాంకం subscription_లెక్క;
	పూర్ణాంకం shared_subscription_లెక్క;
	పూర్ణాంకం retained_లెక్క;
#అయితే_ముగింపు
	పూర్ణాంకం persistence_changes;
	నిర్మాణం mosquitto *ll_for_free;
#నిర్వచితమైతే WITH_EPOLL
	పూర్ణాంకం epollfd;
#అయితే_ముగింపు
#నిర్వచితమైతే WITH_KQUEUE
	పూర్ణాంకం kqueuefd;
#అయితే_ముగింపు
	నిర్మాణం mosquitto_సందేశం_v5 *plugin_msgs;
#నిర్వచితమైతే WITH_TLS
	/* tls_keylog can't be in the config struct because it is used
	   before the config is allocated. Config probably
	   shouldn't be separately allocated. */
	అక్షరం *tls_keylog;
#అయితే_ముగింపు
	bool shutdown;
};

గణన mosquitto__bridge_direction {
	bd_బహిర్గతం = 0,
	bd_లోపల = 1,
	bd_both = 2,
};

గణన mosquitto_bridge_ప్రారంభం_type {
	bst_automatic = 0,
	bst_lazy = 1,
	bst_manual = 2,
	bst_once = 3,
};

గణన mosquitto_bridge_reload_type {
	brt_lazy = 0,
	brt_తక్షణ = 1,
};

నిర్మాణం mosquitto__bridge_topic {
	నిర్మాణం mosquitto__bridge_topic *తదుపరి;
	అక్షరం *topic;
	అక్షరం *local_ఉపసర్గ;
	అక్షరం *remote_ఉపసర్గ;
	అక్షరం *local_topic; /* topic prefixed with local_prefix */
	అక్షరం *remote_topic; /* topic prefixed with remote_prefix */
	గణన mosquitto__bridge_direction direction;
	uint8_t qos;
};

నిర్మాణం bridge_చిరునామా {
	అక్షరం *చిరునామా;
	uint16_t పోర్టు;
};

నిర్మాణం mosquitto__bridge {
	అక్షరం *name;
	నిర్మాణం bridge_చిరునామా *చిరునామాలు;
	పూర్ణాంకం cur_చిరునామా;
	పూర్ణాంకం చిరునామా_లెక్క;
	time_t primary_retry;
	mosq_sock_t primary_retry_sock;
	bool విడత_robin;
	bool try_వ్యక్తిగత;
	bool try_వ్యక్తిగత_accepted;
	bool clean_ప్రారంభం;
	int8_t clean_ప్రారంభం_local;
	uint16_t keepalive;
	గుర్తులేని పూర్ణాంకం tcp_keepalive_idle;
	గుర్తులేని పూర్ణాంకం tcp_keepalive_interval;
	గుర్తులేని పూర్ణాంకం tcp_keepalive_counter;
#నిర్వచితమైతే WITH_TCP_USER_TIMEOUT
	పూర్ణాంకం tcp_user_timeout;
#అయితే_ముగింపు
	నిర్మాణం mosquitto__bridge_topic *topics;
	పూర్ణాంకం topic_లెక్క;
	bool topic_remapping;
	గణన mosquitto__protocol protocol_సంస్కరణ;
	time_t restart_t;
	time_t connected_at;
	అక్షరం *remote_clientid;
	అక్షరం *remote_username;
	అక్షరం *remote_password;
	అక్షరం *local_clientid;
	అక్షరం *local_username;
	అక్షరం *local_password;
	అక్షరం *notification_topic;
	అక్షరం *bind_చిరునామా;
	bool notifications;
	bool notifications_local_only;
	గణన mosquitto_bridge_ప్రారంభం_type ప్రారంభం_type;
	పూర్ణాంకం idle_timeout;
	పూర్ణాంకం restart_timeout;
	పూర్ణాంకం backoff_పునాది;
	పూర్ణాంకం backoff_cap;
	పూర్ణాంకం stable_connection_period;
	పూర్ణాంకం పరిమితి;
	uint32_t గరిష్ఠ_packet_కొలత;
	uint32_t session_expiry_interval;
	uint16_t receive_గరిష్ఠ;
	bool lazy_reconnect;
	bool attempt_unsubscribe;
	bool initial_notification_పూర్తి;
	bool outgoing_retain;
	గణన mosquitto_bridge_reload_type reload_type;
	uint16_t max_topic_alias;
#నిర్వచితమైతే WITH_TLS
	bool tls_insecure;
	bool tls_ocsp_required;
	bool tls_use_os_certs;
	అక్షరం *tls_cafile;
	అక్షరం *tls_capath;
	అక్షరం *tls_certfile;
	అక్షరం *tls_keyfile;
	అక్షరం *tls_సంస్కరణ;
	అక్షరం *tls_alpn;
	అక్షరం *tls_ciphers;
	అక్షరం *tls_13_ciphers;
#  ifdef FINAL_WITH_TLS_PSK
	అక్షరం *tls_psk_తటస్థం;
	అక్షరం *tls_psk;
#  endif
#అయితే_ముగింపు
};

#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
నిర్మాణం libws_mqtt_hack {
	అక్షరం *http_dir;
	నిర్మాణం mosquitto__listener *listener;
};

నిర్మాణం libws_mqtt_దత్తాంశం {
	నిర్మాణం mosquitto *mosq;
};
#అయితే_ముగింపు

#అన్తర్భావ <net_mosq.h>


నిర్మాణం control_endpoint {
	నిర్మాణం control_endpoint *తదుపరి, *prev;
	అక్షరం topic[];
};

బాహ్య నిర్మాణం mosquitto_db db;

/* ============================================================
 * Main functions
 * ============================================================ */
పూర్ణాంకం mosquitto_main_loop(నిర్మాణం mosquitto__listener_sock *listensock, పూర్ణాంకం listensock_లెక్క);
శూన్య loop_update_తదుపరి_event(time_t new_ms);

/* ============================================================
 * Config functions
 * ============================================================ */
/* Initialise config struct to default values. */
శూన్య config__init(నిర్మాణం mosquitto__config *config);
#నిర్వచితమైతే WITH_BRIDGE
శూన్య config__bridge_cleanup(నిర్మాణం mosquitto__bridge *bridge);
#అయితే_ముగింపు
/* Parse command line options into config. */
పూర్ణాంకం config_parse_వాదనలు(నిర్మాణం mosquitto__config *config, పూర్ణాంకం argc, అక్షరం *argv[]);
/* Read configuration data from config->config_file into config.
 * If reload is true, don't process config options that shouldn't be reloaded (listeners etc)
 * Returns 0 on success, 1 if there is a configuration error or if a file cannot be opened.
 */
పూర్ణాంకం config__read(నిర్మాణం mosquitto__config *config, bool reload);
/* Free all config data. */
శూన్య config__cleanup(నిర్మాణం mosquitto__config *config);
పూర్ణాంకం config__get_dir_files(స్థిరరాశి అక్షరం *include_dir, అక్షరం ***files, పూర్ణాంకం *file_లెక్క);

/* ============================================================
 * Server send functions
 * ============================================================ */
పూర్ణాంకం send__connack(నిర్మాణం mosquitto *context, uint8_t ack, uint8_t reason_సంకేతం, స్థిరరాశి mosquitto_property *properties);
పూర్ణాంకం send__suback(నిర్మాణం mosquitto *context, uint16_t mid, uint32_t payloadlen, స్థిరరాశి శూన్య *payload);
పూర్ణాంకం send__unsuback(నిర్మాణం mosquitto *context, uint16_t mid, పూర్ణాంకం reason_సంకేతం_లెక్క, uint8_t *reason_codes, స్థిరరాశి mosquitto_property *properties);
పూర్ణాంకం send__auth(నిర్మాణం mosquitto *context, uint8_t reason_సంకేతం, స్థిరరాశి శూన్య *auth_దత్తాంశం, uint16_t auth_దత్తాంశం_len);

/* ============================================================
 * Network functions
 * ============================================================ */
శూన్య net__broker_init(శూన్య);
శూన్య net__broker_cleanup(శూన్య);
నిర్మాణం mosquitto *net__socket_accept(నిర్మాణం mosquitto__listener_sock *listensock);
పూర్ణాంకం net__socket_listen(నిర్మాణం mosquitto__listener *listener);
పూర్ణాంకం net_socket_get_చిరునామా(mosq_sock_t sock, అక్షరం *buf, size_t len, uint16_t *remote_చిరునామా);
పూర్ణాంకం net_tls_ఎక్కించు_verify(నిర్మాణం mosquitto__listener *listener);
పూర్ణాంకం net__tls_server_ctx(నిర్మాణం mosquitto__listener *listener);
పూర్ణాంకం net_ఎక్కించు_certificates(నిర్మాణం mosquitto__listener *listener);

/* ============================================================
 * Read handling functions
 * ============================================================ */
పూర్ణాంకం handle__packet(నిర్మాణం mosquitto *context);
పూర్ణాంకం handle__connack(నిర్మాణం mosquitto *context);
పూర్ణాంకం handle__connect(నిర్మాణం mosquitto *context);
పూర్ణాంకం handle__disconnect(నిర్మాణం mosquitto *context);
పూర్ణాంకం handle__publish(నిర్మాణం mosquitto *context);
పూర్ణాంకం handle__subscribe(నిర్మాణం mosquitto *context);
పూర్ణాంకం handle__unsubscribe(నిర్మాణం mosquitto *context);
పూర్ణాంకం handle__auth(నిర్మాణం mosquitto *context);

/* ============================================================
 * Database handling
 * ============================================================ */
పూర్ణాంకం db__open(నిర్మాణం mosquitto__config *config);
పూర్ణాంకం db__close(శూన్య);
#నిర్వచితమైతే WITH_PERSISTENCE
పూర్ణాంకం persist__backup(bool shutdown);
పూర్ణాంకం persist__restore(శూన్య);
#అయితే_ముగింపు
/* Return the number of in-flight messages in count. */
పూర్ణాంకం db_సందేశం_లెక్క(పూర్ణాంకం *లెక్క);
పూర్ణాంకం db_సందేశం_delete_outgoing(నిర్మాణం mosquitto *context, uint16_t mid, గణన mosquitto_సందేశం_state expect_state, పూర్ణాంకం qos);
పూర్ణాంకం db_సందేశం_చొప్పించు_outgoing(నిర్మాణం mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం, uint32_t subscription_identifier, bool update, bool persist);
పూర్ణాంకం db_సందేశం_చొప్పించు_incoming(నిర్మాణం mosquitto *context, uint64_t cmsg_id, నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం, bool persist);
పూర్ణాంకం db_సందేశం_remove_incoming(నిర్మాణం mosquitto *context, uint16_t mid);
పూర్ణాంకం db_సందేశం_release_incoming(నిర్మాణం mosquitto *context, uint16_t mid);
పూర్ణాంకం db_సందేశం_update_outgoing(నిర్మాణం mosquitto *context, uint16_t mid, గణన mosquitto_సందేశం_state state, పూర్ణాంకం qos, bool persist);
శూన్య db_సందేశం_వరుస_నుండి_తీయి_మొదటి(నిర్మాణం mosquitto *context, నిర్మాణం mosquitto_సందేశం_దత్తాంశం *సందేశం_దత్తాంశం);
పూర్ణాంకం db__messages_delete(నిర్మాణం mosquitto *context, bool force_free);
పూర్ణాంకం db__messages_delete_incoming(నిర్మాణం mosquitto *context);
పూర్ణాంకం db__messages_delete_outgoing(నిర్మాణం mosquitto *context);
పూర్ణాంకం db_messages_easy_వరుస(నిర్మాణం mosquitto *context, స్థిరరాశి అక్షరం *topic, uint8_t qos, uint32_t payloadlen, స్థిరరాశి శూన్య *payload, పూర్ణాంకం retain, uint32_t సందేశం_expiry_interval, mosquitto_property **properties);
పూర్ణాంకం db_సందేశం_store(స్థిరరాశి నిర్మాణం mosquitto *ఆధారం, నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం, uint32_t *సందేశం_expiry_interval, గణన mosquitto_సందేశం_origin origin);
పూర్ణాంకం db_సందేశం_store_కనుగొను(నిర్మాణం mosquitto *context, uint16_t mid, నిర్మాణం mosquitto_client_సందేశం **client_సందేశం);
పూర్ణాంకం db_సందేశం_store_కూడు(నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం);
శూన్య db_సందేశం_store_remove(నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం, bool notify);
శూన్య db_సందేశం_store_ref_inc(నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం);
శూన్య db_సందేశం_store_ref_dec(నిర్మాణం mosquitto_పునాది_సందేశం **పునాది_సందేశం);
శూన్య db_సందేశం_store_clean(శూన్య);
శూన్య db_సందేశం_store_compact(శూన్య);
శూన్య db_సందేశం_store_free(నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం);
పూర్ణాంకం db_సందేశం_reconnect_reset(నిర్మాణం mosquitto *context);
bool db__ready_for_flight(నిర్మాణం mosquitto *context, గణన mosquitto_సందేశం_direction dir, పూర్ణాంకం qos);
bool db_ready_for_వరుస(నిర్మాణం mosquitto *context, పూర్ణాంకం qos, నిర్మాణం mosquitto_సందేశం_దత్తాంశం *సందేశం_దత్తాంశం);
శూన్య sys_వృక్షం_init(శూన్య);
శూన్య sys_వృక్షం_update(bool force);
పూర్ణాంకం db_సందేశం_write_inflight_బహిర్గతం_all(నిర్మాణం mosquitto *context);
పూర్ణాంకం db_సందేశం_write_inflight_బహిర్గతం_latest(నిర్మాణం mosquitto *context);
పూర్ణాంకం db_సందేశం_write_queued_బహిర్గతం(నిర్మాణం mosquitto *context);
పూర్ణాంకం db_సందేశం_write_queued_లోపల(నిర్మాణం mosquitto *context);
శూన్య db_సందేశం_కూడు_గమ్యం_inflight_stats(నిర్మాణం mosquitto_సందేశం_దత్తాంశం *సందేశం_దత్తాంశం, నిర్మాణం mosquitto_client_సందేశం *సందేశం);
శూన్య db_సందేశం_కూడు_గమ్యం_queued_stats(నిర్మాణం mosquitto_సందేశం_దత్తాంశం *సందేశం_దత్తాంశం, నిర్మాణం mosquitto_client_సందేశం *సందేశం);
uint64_t db_new_సందేశం_id(శూన్య);
శూన్య db__expire_all_messages(నిర్మాణం mosquitto *context);
శూన్య db_check_acl_యొక్క_all_messages(నిర్మాణం mosquitto *context);

/* ============================================================
 * Subscription functions
 * ============================================================ */
పూర్ణాంకం sub__init(శూన్య);
పూర్ణాంకం sub_కూడు(నిర్మాణం mosquitto *context, స్థిరరాశి నిర్మాణం mosquitto_subscription *sub);
పూర్ణాంకం sub__remove(నిర్మాణం mosquitto *context, స్థిరరాశి అక్షరం *sub, uint8_t *reason);
శూన్య sub_వృక్షం_print(నిర్మాణం mosquitto__subhier *మూలం, పూర్ణాంకం level);
పూర్ణాంకం sub__clean_session(నిర్మాణం mosquitto *context);
పూర్ణాంకం sub_messages_వరుస(స్థిరరాశి అక్షరం *ఆధారం_id, uint8_t qos, నిర్మాణం mosquitto_పునాది_సందేశం **పునాది_సందేశం);
పూర్ణాంకం sub__topic_tokenise(స్థిరరాశి అక్షరం *subtopic, అక్షరం **local_sub, అక్షరం ***topics, స్థిరరాశి అక్షరం **sharename);
శూన్య sub_topic_టోకెన్లు_free(నిర్మాణం sub__token *టోకెన్లు);

/* ============================================================
 * Context functions
 * ============================================================ */
నిర్మాణం mosquitto *context__init(శూన్య);
పూర్ణాంకం context__init_sock(నిర్మాణం mosquitto *context, mosq_sock_t sock, bool get_చిరునామా);
శూన్య context__cleanup(నిర్మాణం mosquitto *context, bool force_free);
శూన్య context__disconnect(నిర్మాణం mosquitto *context, పూర్ణాంకం reason);
శూన్య context_కూడు_గమ్యం_disused(నిర్మాణం mosquitto *context);
శూన్య context__free_disused(శూన్య);
శూన్య context__send_will(నిర్మాణం mosquitto *context);
శూన్య context_కూడు_గమ్యం_by_id(నిర్మాణం mosquitto *context);
శూన్య context__remove_from_by_id(నిర్మాణం mosquitto *context);

పూర్ణాంకం connect__on_authorised(నిర్మాణం mosquitto *context, శూన్య *auth_దత్తాంశం_బహిర్గతం, uint16_t auth_దత్తాంశం_బహిర్గతం_len);


/* ============================================================
 * Control functions
 * ============================================================ */
#నిర్వచితమైతే WITH_CONTROL
పూర్ణాంకం control__process(నిర్మాణం mosquitto *context, నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం);
శూన్య control__cleanup(శూన్య);
#అయితే_ముగింపు
పూర్ణాంకం control_రిజిస్టరు_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, స్థిరరాశి అక్షరం *topic, శూన్య *userdata);
పూర్ణాంకం control__unregister_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, స్థిరరాశి అక్షరం *topic);
శూన్య control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier);


/* ============================================================
 * Logging functions
 * ============================================================ */
పూర్ణాంకం log__init(నిర్మాణం mosquitto__config *config);
పూర్ణాంకం log__close(నిర్మాణం mosquitto__config *config);
శూన్య log_అంతర్గత(స్థిరరాశి అక్షరం *fmt, ...) __attribute__((format(రాయి, 1, 2)));

/* ============================================================
 * Bridge functions
 * ============================================================ */
#నిర్వచితమైతే WITH_BRIDGE
శూన్య bridge_ప్రారంభం_all(శూన్య);
శూన్య bridge__reload(శూన్య);
శూన్య bridge__db_cleanup(శూన్య);
శూన్య bridge__cleanup(నిర్మాణం mosquitto *context);
పూర్ణాంకం bridge__connect(నిర్మాణం mosquitto *context);
#ఒకవేళ defined(__GLIBC__) && defined(WITH_ADNS)
పూర్ణాంకం bridge__connect_step3(నిర్మాణం mosquitto *context);
#అయితే_ముగింపు
పూర్ణాంకం bridge__on_connect(నిర్మాణం mosquitto *context);
శూన్య bridge_check(శూన్య);
పూర్ణాంకం bridge_రిజిస్టరు_local_connections(శూన్య);
పూర్ణాంకం bridge_కూడు_topic(నిర్మాణం mosquitto__bridge *bridge, స్థిరరాశి అక్షరం *topic, గణన mosquitto__bridge_direction direction, uint8_t qos, స్థిరరాశి అక్షరం *local_ఉపసర్గ, స్థిరరాశి అక్షరం *remote_ఉపసర్గ);
శూన్య bridge__cleanup_topics(నిర్మాణం mosquitto__bridge *bridge);
పూర్ణాంకం bridge_remap_topic_లోపల(నిర్మాణం mosquitto *context, అక్షరం **topic);
#అయితే_ముగింపు

/* ============================================================
 * IO multiplex related functions
 * ============================================================ */
పూర్ణాంకం mux__init(శూన్య);
పూర్ణాంకం mux_కూడు_listeners(నిర్మాణం mosquitto__listener_sock *listensock, పూర్ణాంకం listensock_లెక్క);
పూర్ణాంకం mux__delete_listeners(నిర్మాణం mosquitto__listener_sock *listensock, పూర్ణాంకం listensock_లెక్క);
పూర్ణాంకం mux__loop_prepare(శూన్య);
పూర్ణాంకం mux__new(నిర్మాణం mosquitto *context);
పూర్ణాంకం mux_కూడు_బహిర్గతం(నిర్మాణం mosquitto *context);
పూర్ణాంకం mux_remove_బహిర్గతం(నిర్మాణం mosquitto *context);
పూర్ణాంకం mux__delete(నిర్మాణం mosquitto *context);
పూర్ణాంకం mux__wait(శూన్య);
పూర్ణాంకం mux__handle(నిర్మాణం mosquitto__listener_sock *listensock, పూర్ణాంకం listensock_లెక్క);
పూర్ణాంకం mux__cleanup(శూన్య);

/* ============================================================
 * Listener related functions
 * ============================================================ */
బాహ్య నిర్మాణం mosquitto__listener_sock *g_listensock;
బాహ్య పూర్ణాంకం g_listensock_లెక్క;

శూన్య listener_సమితి_defaults(నిర్మాణం mosquitto__listener *listener);
శూన్య listeners__reload_all_certificates(శూన్య);
#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
శూన్య listeners_కూడు_websockets(నిర్మాణం lws_context *ws_context, mosq_sock_t fd);
#అయితే_ముగింపు
పూర్ణాంకం listeners_ప్రారంభం(శూన్య);
శూన్య listeners__stop(శూన్య);

/* ============================================================
 * Plugin related functions
 * ============================================================ */
పూర్ణాంకం plugin_ఎక్కించు_v5(mosquitto_plugin_id_t *plugin, శూన్య *lib);
పూర్ణాంకం plugin_ఎక్కించు_v4(mosquitto_plugin_id_t *plugin, శూన్య *lib);
పూర్ణాంకం plugin_ఎక్కించు_v3(mosquitto_plugin_id_t *plugin, శూన్య *lib);
పూర్ణాంకం plugin_ఎక్కించు_v2(mosquitto_plugin_id_t *plugin, శూన్య *lib);
పూర్ణాంకం acl__pre_check(mosquitto_plugin_id_t *plugin, నిర్మాణం mosquitto *context, పూర్ణాంకం access);

శూన్య LIB_ERROR(శూన్య);
శూన్య plugin__handle_connect(నిర్మాణం mosquitto *context);
శూన్య plugin__handle_disconnect(నిర్మాణం mosquitto *context, పూర్ణాంకం reason);
శూన్య plugin__handle_client_offline(నిర్మాణం mosquitto *context, పూర్ణాంకం reason);
పూర్ణాంకం plugin_handle_సందేశం_లోపల(నిర్మాణం mosquitto *context, నిర్మాణం mosquitto_పునాది_సందేశం_2 *పునాది_సందేశం);
పూర్ణాంకం plugin_handle_సందేశం_బహిర్గతం(నిర్మాణం mosquitto *context, నిర్మాణం mosquitto_పునాది_సందేశం_2 *పునాది_సందేశం);
పూర్ణాంకం plugin__handle_subscribe(నిర్మాణం mosquitto *context, నిర్మాణం mosquitto_subscription *sub);
పూర్ణాంకం plugin__handle_unsubscribe(నిర్మాణం mosquitto *context, నిర్మాణం mosquitto_subscription *sub);
పూర్ణాంకం plugin__handle_reload(శూన్య);
శూన్య plugin__handle_tick(శూన్య);
పూర్ణాంకం plugin__callback_unregister_all(mosquitto_plugin_id_t *identifier);
శూన్య plugin_persist__handle_restore(శూన్య);
శూన్య plugin_persist_handle_client_కూడు(నిర్మాణం mosquitto *context);
శూన్య plugin_persist__handle_client_delete(నిర్మాణం mosquitto *context);
శూన్య plugin_persist__handle_client_update(నిర్మాణం mosquitto *context);
శూన్య plugin_persist_handle_subscription_కూడు(నిర్మాణం mosquitto *context, స్థిరరాశి నిర్మాణం mosquitto_subscription *sub);
శూన్య plugin_persist__handle_subscription_delete(నిర్మాణం mosquitto *context, అక్షరం *sub);
శూన్య plugin_persist_handle_client_సందేశం_కూడు(నిర్మాణం mosquitto *context, స్థిరరాశి నిర్మాణం mosquitto_client_సందేశం *cmsg);
శూన్య plugin_persist_handle_client_సందేశం_delete(నిర్మాణం mosquitto *context, స్థిరరాశి నిర్మాణం mosquitto_client_సందేశం *cmsg);
శూన్య plugin_persist_handle_client_సందేశం_update(నిర్మాణం mosquitto *context, స్థిరరాశి నిర్మాణం mosquitto_client_సందేశం *cmsg);
శూన్య plugin_persist_handle_పునాది_సందేశం_కూడు(నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం);
శూన్య plugin_persist_handle_పునాది_సందేశం_delete(నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం);
శూన్య plugin_persist_handle_retain_సందేశం_సమితి(నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం);
శూన్య plugin_persist_handle_retain_సందేశం_delete(నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం);
శూన్య plugin_persist_handle_will_కూడు(నిర్మాణం mosquitto *context);
శూన్య plugin_persist__handle_will_delete(నిర్మాణం mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
పూర్ణాంకం keepalive__init(శూన్య);
శూన్య keepalive__cleanup(శూన్య);
పూర్ణాంకం keepalive_కూడు(నిర్మాణం mosquitto *context);
శూన్య keepalive__check(శూన్య);
పూర్ణాంకం keepalive__remove(నిర్మాణం mosquitto *context);
పూర్ణాంకం keepalive__update(నిర్మాణం mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
పూర్ణాంకం property__process_connect(నిర్మాణం mosquitto *context, mosquitto_property **props);
పూర్ణాంకం property__process_will(నిర్మాణం mosquitto *context, నిర్మాణం mosquitto_సందేశం_all *సందేశం, mosquitto_property **props);
పూర్ణాంకం property__process_publish(నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం, mosquitto_property **props, పూర్ణాంకం *topic_alias, uint32_t *సందేశం_expiry_interval, bool is_bridge);
పూర్ణాంకం property__process_disconnect(నిర్మాణం mosquitto *context, mosquitto_property **props);

/* ============================================================
 * Retain tree related functions
 * ============================================================ */
పూర్ణాంకం retain__init(శూన్య);
శూన్య retain__clean(నిర్మాణం mosquitto__retainhier **retainhier);
పూర్ణాంకం retain_వరుస(నిర్మాణం mosquitto *context, స్థిరరాశి నిర్మాణం mosquitto_subscription *sub);
పూర్ణాంకం retain__store(స్థిరరాశి అక్షరం *topic, నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం, అక్షరం **విభజించు_topics, bool persist);
శూన్య retain__expiry_check(శూన్య);
శూన్య retain__expire(నిర్మాణం mosquitto__retainhier **retainhier);

/* ============================================================
 * Security related functions
 * ============================================================ */
పూర్ణాంకం acl_కనుగొను_acls(నిర్మాణం mosquitto *context);
పూర్ణాంకం plugin_ఎక్కించు_all(శూన్య);
పూర్ణాంకం plugin__unload_all(శూన్య);
పూర్ణాంకం config_plugin_కూడు_secopt(mosquitto_plugin_id_t *plugin, నిర్మాణం mosquitto__security_options *security_options);

పూర్ణాంకం mosquitto_security_init(bool reload);
పూర్ణాంకం mosquitto_security_cleanup(bool reload);
పూర్ణాంకం mosquitto_acl_check(నిర్మాణం mosquitto *context, స్థిరరాశి అక్షరం *topic, uint32_t payloadlen, శూన్య *payload, uint8_t qos, bool retain, mosquitto_property *properties, పూర్ణాంకం access);
పూర్ణాంకం mosquitto_basic_auth(నిర్మాణం mosquitto *context);
పూర్ణాంకం mosquitto_psk_కీలకం_get(నిర్మాణం mosquitto *context, స్థిరరాశి అక్షరం *hint, స్థిరరాశి అక్షరం *తటస్థం, అక్షరం *కీలకం, పూర్ణాంకం max_కీలకం_len);

పూర్ణాంకం mosquitto_security_init_default(శూన్య);
పూర్ణాంకం mosquitto_security_apply_default(శూన్య);
పూర్ణాంకం mosquitto_security_cleanup_default(శూన్య);
పూర్ణాంకం mosquitto_psk_కీలకం_get_default(నిర్మాణం mosquitto *context, స్థిరరాశి అక్షరం *hint, స్థిరరాశి అక్షరం *తటస్థం, అక్షరం *కీలకం, పూర్ణాంకం max_కీలకం_len);
పూర్ణాంకం broker_acl_file__init(శూన్య);
శూన్య broker_acl_file__cleanup(శూన్య);
పూర్ణాంకం broker_password_file__init(శూన్య);
శూన్య broker_password_file__cleanup(శూన్య);
పూర్ణాంకం psk_file__init(శూన్య);
పూర్ణాంకం psk_file__cleanup(శూన్య);

పూర్ణాంకం mosquitto_security_auth_ప్రారంభం(నిర్మాణం mosquitto *context, bool reauth, స్థిరరాశి శూన్య *దత్తాంశం_లోపల, uint16_t దత్తాంశం_లోపల_len, శూన్య **దత్తాంశం_బహిర్గతం, uint16_t *దత్తాంశం_బహిర్గతం_len);
పూర్ణాంకం mosquitto_security_auth_continue(నిర్మాణం mosquitto *context, స్థిరరాశి శూన్య *దత్తాంశం_లోపల, uint16_t దత్తాంశం_len, శూన్య **దత్తాంశం_బహిర్గతం, uint16_t *దత్తాంశం_బహిర్గతం_len);

శూన్య unpwd_free_అంశం(నిర్మాణం mosquitto__unpwd **unpwd, నిర్మాణం mosquitto__unpwd *అంశం);

/* ============================================================
 * Session expiry
 * ============================================================ */
పూర్ణాంకం session_expiry_కూడు(నిర్మాణం mosquitto *context);
పూర్ణాంకం session_expiry_కూడు_from_persistence(నిర్మాణం mosquitto *context, time_t expiry_సమయం);
శూన్య session_expiry_కూడు_on_shutdown(శూన్య);
శూన్య session_expiry__remove(నిర్మాణం mosquitto *context);
శూన్య session_expiry__remove_all(శూన్య);
శూన్య session_expiry__check(శూన్య);
శూన్య session_expiry__send_all(శూన్య);

/* ============================================================
 * Signals
 * ============================================================ */
శూన్య signal_అమరిక(శూన్య);
పూర్ణాంకం signal__flag_check(శూన్య);

/* ============================================================
 * Window service and signal related functions
 * ============================================================ */
#ఒకవేళ defined(WIN32) || defined(__CYGWIN__)
శూన్య service_install(అక్షరం *name);
శూన్య service_uninstall(అక్షరం *name);
శూన్య service_నిరంతరం(అక్షరం *name);

#నిర్వచితమైతే WIN32
DWORD WINAPI SigThreadProc(శూన్య *దత్తాంశం);
#అయితే_ముగింపు
#అయితే_ముగింపు

/* ============================================================
 * Watchdog
 * ============================================================ */
శూన్య watchdog__init(శూన్య);
శూన్య watchdog__check(శూన్య);

/* ============================================================
 * Websockets related functions
 * ============================================================ */
#నిర్వచితమైతే WITH_WEBSOCKETS
#  ఒకవేళ WITH_WEBSOCKETS == WS_IS_LWS
శూన్య mosq_websockets_init(నిర్మాణం mosquitto__listener *listener, స్థిరరాశి నిర్మాణం mosquitto__config *conf);
#  endif
పూర్ణాంకం http__context_init(నిర్మాణం mosquitto *context);
పూర్ణాంకం http__context_cleanup(నిర్మాణం mosquitto *context);
పూర్ణాంకం http__read(నిర్మాణం mosquitto *context);
#అయితే_ముగింపు
శూన్య do_disconnect(నిర్మాణం mosquitto *context, పూర్ణాంకం reason);

/* ============================================================
 * PROXY related functions
 * ============================================================ */
పూర్ణాంకం proxy_v2__read(నిర్మాణం mosquitto *context);
పూర్ణాంకం proxy_v1__read(నిర్మాణం mosquitto *context);

/* ============================================================
 * Will delay
 * ============================================================ */
పూర్ణాంకం will_delay_కూడు(నిర్మాణం mosquitto *context);
శూన్య will_delay__check(శూన్య);
శూన్య will_delay__send_all(శూన్య);
శూన్య will_delay__remove(నిర్మాణం mosquitto *mosq);

/* ============================================================
 * HTTP Info
 * ============================================================ */
పూర్ణాంకం http_api_ప్రారంభం_local(నిర్మాణం mosquitto__listener *listener);
పూర్ణాంకం http_api_ప్రారంభం(నిర్మాణం mosquitto__listener *listener);
శూన్య http_api__stop(నిర్మాణం mosquitto__listener *listener);


/* ============================================================
 * Other
 * ============================================================ */
#నిర్వచితమైతే WITH_XTREPORT
శూన్య xtreport(శూన్య);
#అయితే_ముగింపు

శూన్య broker_control__init(శూన్య);
శూన్య broker_control__cleanup(శూన్య);
శూన్య broker_control__reload(శూన్య);

#అయితే_ముగింపు
