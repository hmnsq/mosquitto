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

#ଅନ୍ତର୍ଭାଵ "config.h"
#ଅନ୍ତର୍ଭାଵ <stdio.h>

#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#  ଯଦି LWS_LIBRARY_VERSION_ସଂଖ୍ୟା >= 3002000 && !defined(LWS_WITH_EXTERNAL_POLL)
#    warning "libwebsockets is not compiled with LWS_WITH_EXTERNAL_POLL support. Websocket performance will be unusable."
#  endif
#endif

#ifdef WITH_HTTP_API
#  include <microhttpd.h>
#endif

#ifdef __linux__
#define WITH_TCP_USER_TIMEOUT
#endif

#ଅନ୍ତର୍ଭାଵ "mosquitto_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/broker.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/broker_plugin.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto.h"
#ଅନ୍ତର୍ଭାଵ "logging_mosq.h"
#ଅନ୍ତର୍ଭାଵ "tls_mosq.h"
#ଅନ୍ତର୍ଭାଵ "uthash.h"
#ଅନ୍ତର୍ଭାଵ "acl_file.h"
#ଅନ୍ତର୍ଭାଵ "password_file.h"

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

ପ୍ରକାରବ୍ୟାଖ୍ୟା uint64_t dbid_t;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_plugin_init_v5)(mosquitto_plugin_id_t *, ଶୂନ୍ୟ **, ସଂରଚନା mosquitto_opt *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_plugin_cleanup_v5)(ଶୂନ୍ୟ *, ସଂରଚନା mosquitto_opt *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_auth_plugin_init_v4)(ଶୂନ୍ୟ **, ସଂରଚନା mosquitto_opt *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_auth_plugin_cleanup_v4)(ଶୂନ୍ୟ *, ସଂରଚନା mosquitto_opt *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_auth_plugin_security_init_v4)(ଶୂନ୍ୟ *, ସଂରଚନା mosquitto_opt *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, bool);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_auth_plugin_security_cleanup_v4)(ଶୂନ୍ୟ *, ସଂରଚନା mosquitto_opt *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, bool);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_auth_plugin_acl_check_v4)(ଶୂନ୍ୟ *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ସଂରଚନା mosquitto *, ସଂରଚନା mosquitto_acl_ସନ୍ଦେଶ *);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_auth_plugin_unpwd_check_v4)(ଶୂନ୍ୟ *, ସଂରଚନା mosquitto *, ଅଚଳ ଅକ୍ଷର *, ଅଚଳ ଅକ୍ଷର *);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_auth_plugin_psk_ଚାବି_get_v4)(ଶୂନ୍ୟ *, ସଂରଚନା mosquitto *, ଅଚଳ ଅକ୍ଷର *, ଅଚଳ ଅକ୍ଷର *, ଅକ୍ଷର *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_auth_plugin_auth_start_v4)(ଶୂନ୍ୟ *, ସଂରଚନା mosquitto *, ଅଚଳ ଅକ୍ଷର *, bool, ଅଚଳ ଶୂନ୍ୟ *, uint16_t, ଶୂନ୍ୟ **, uint16_t *);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_auth_plugin_auth_continue_v4)(ଶୂନ୍ୟ *, ସଂରଚନା mosquitto *, ଅଚଳ ଅକ୍ଷର *, ଅଚଳ ଶୂନ୍ୟ *, uint16_t, ଶୂନ୍ୟ **, uint16_t *);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_auth_plugin_init_v3)(ଶୂନ୍ୟ **, ସଂରଚନା mosquitto_opt *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_auth_plugin_cleanup_v3)(ଶୂନ୍ୟ *, ସଂରଚନା mosquitto_opt *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_auth_plugin_security_init_v3)(ଶୂନ୍ୟ *, ସଂରଚନା mosquitto_opt *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, bool);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_auth_plugin_security_cleanup_v3)(ଶୂନ୍ୟ *, ସଂରଚନା mosquitto_opt *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, bool);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_auth_plugin_acl_check_v3)(ଶୂନ୍ୟ *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଅଚଳ ସଂରଚନା mosquitto *, ସଂରଚନା mosquitto_acl_ସନ୍ଦେଶ *);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_auth_plugin_unpwd_check_v3)(ଶୂନ୍ୟ *, ଅଚଳ ସଂରଚନା mosquitto *, ଅଚଳ ଅକ୍ଷର *, ଅଚଳ ଅକ୍ଷର *);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_auth_plugin_psk_ଚାବି_get_v3)(ଶୂନ୍ୟ *, ଅଚଳ ସଂରଚନା mosquitto *, ଅଚଳ ଅକ୍ଷର *, ଅଚଳ ଅକ୍ଷର *, ଅକ୍ଷର *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_auth_plugin_init_v2)(ଶୂନ୍ୟ **, ସଂରଚନା mosquitto_auth_opt *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_auth_plugin_cleanup_v2)(ଶୂନ୍ୟ *, ସଂରଚନା mosquitto_auth_opt *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_auth_plugin_security_init_v2)(ଶୂନ୍ୟ *, ସଂରଚନା mosquitto_auth_opt *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, bool);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_auth_plugin_security_cleanup_v2)(ଶୂନ୍ୟ *, ସଂରଚନା mosquitto_auth_opt *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, bool);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_auth_plugin_acl_check_v2)(ଶୂନ୍ୟ *, ଅଚଳ ଅକ୍ଷର *, ଅଚଳ ଅକ୍ଷର *, ଅଚଳ ଅକ୍ଷର *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_auth_plugin_unpwd_check_v2)(ଶୂନ୍ୟ *, ଅଚଳ ଅକ୍ଷର *, ଅଚଳ ଅକ୍ଷର *);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_auth_plugin_psk_ଚାବି_get_v2)(ଶୂନ୍ୟ *, ଅଚଳ ଅକ୍ଷର *, ଅଚଳ ଅକ୍ଷର *, ଅକ୍ଷର *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);


ଗଣନା mosquitto_ସନ୍ଦେଶ_origin {
	mosq_mo_client = 0,
	mosq_mo_broker = 1,
};

ସଂରଚନା mosquitto__plugin_lib {
	ଶୂନ୍ୟ *lib;
	ଶୂନ୍ୟ *user_data;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*plugin_version)(ଶୂନ୍ୟ);
	ସଂରଚନା mosquitto_plugin_id_t *identifier;

	FUNC_plugin_init_v5 plugin_init_v5;
	FUNC_plugin_cleanup_v5 plugin_cleanup_v5;

	FUNC_auth_plugin_init_v4 plugin_init_v4;
	FUNC_auth_plugin_cleanup_v4 plugin_cleanup_v4;
	FUNC_auth_plugin_security_init_v4 security_init_v4;
	FUNC_auth_plugin_security_cleanup_v4 security_cleanup_v4;
	FUNC_auth_plugin_acl_check_v4 acl_check_v4;
	FUNC_auth_plugin_unpwd_check_v4 unpwd_check_v4;
	FUNC_auth_plugin_psk_ଚାବି_get_v4 psk_ଚାବି_get_v4;
	FUNC_auth_plugin_auth_start_v4 auth_start_v4;
	FUNC_auth_plugin_auth_continue_v4 auth_continue_v4;

	FUNC_auth_plugin_init_v3 plugin_init_v3;
	FUNC_auth_plugin_cleanup_v3 plugin_cleanup_v3;
	FUNC_auth_plugin_security_init_v3 security_init_v3;
	FUNC_auth_plugin_security_cleanup_v3 security_cleanup_v3;
	FUNC_auth_plugin_acl_check_v3 acl_check_v3;
	FUNC_auth_plugin_unpwd_check_v3 unpwd_check_v3;
	FUNC_auth_plugin_psk_ଚାବି_get_v3 psk_ଚାବି_get_v3;

	FUNC_auth_plugin_init_v2 plugin_init_v2;
	FUNC_auth_plugin_cleanup_v2 plugin_cleanup_v2;
	FUNC_auth_plugin_security_init_v2 security_init_v2;
	FUNC_auth_plugin_security_cleanup_v2 security_cleanup_v2;
	FUNC_auth_plugin_acl_check_v2 acl_check_v2;
	FUNC_auth_plugin_unpwd_check_v2 unpwd_check_v2;
	FUNC_auth_plugin_psk_ଚାବି_get_v2 psk_ଚାବି_get_v2;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା version;
};

ସଂରଚନା mosquitto__plugin_config {
	ଅକ୍ଷର *path;
	ଅକ୍ଷର *name;
	ସଂରଚନା mosquitto_opt *options;
	ସଂରଚନା mosquitto__security_options **security_options;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା option_ଗଣନା;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା security_option_ଗଣନା;
	bool deny_special_chars;
};

ସଂରଚନା mosquitto__callback {
	UT_hash_handle hh; /* For callbacks that register for e.g. a specific topic */
	ସଂରଚନା mosquitto__callback *next, *prev; /* For typical callbacks */
	MOSQ_FUNC_generic_callback cb;
	ଶୂନ୍ୟ *userdata;
	ସଂଘ {
		ଅକ୍ଷର *topic;
		ସଂରଚନା timespec next_tick;
	} data;
	mosquitto_plugin_id_t *identifier;
};

ସଂରଚନା plugin__callbacks {
	ସଂରଚନା mosquitto__callback *tick;
	ସଂରଚନା mosquitto__callback *acl_check;
	ସଂରଚନା mosquitto__callback *basic_auth;
	ସଂରଚନା mosquitto__callback *connect;
	ସଂରଚନା mosquitto__callback *control;
	ସଂରଚନା mosquitto__callback *disconnect;
	ସଂରଚନା mosquitto__callback *client_offline;
	ସଂରଚନା mosquitto__callback *ext_auth_continue;
	ସଂରଚନା mosquitto__callback *ext_auth_start;
	ସଂରଚନା mosquitto__callback *message_in;
	ସଂରଚନା mosquitto__callback *message_out;
	ସଂରଚନା mosquitto__callback *psk_ଚାବି;
	ସଂରଚନା mosquitto__callback *reload;
	ସଂରଚନା mosquitto__callback *subscribe;
	ସଂରଚନା mosquitto__callback *unsubscribe;
	ସଂରଚନା mosquitto__callback *persist_restore;
	ସଂରଚନା mosquitto__callback *persist_client_add;
	ସଂରଚନା mosquitto__callback *persist_client_delete;
	ସଂରଚନା mosquitto__callback *persist_client_update;
	ସଂରଚନା mosquitto__callback *persist_subscription_add;
	ସଂରଚନା mosquitto__callback *persist_subscription_delete;
	ସଂରଚନା mosquitto__callback *persist_client_ସନ୍ଦେଶ_add;
	ସଂରଚନା mosquitto__callback *persist_client_ସନ୍ଦେଶ_delete;
	ସଂରଚନା mosquitto__callback *persist_client_ସନ୍ଦେଶ_update;
	ସଂରଚନା mosquitto__callback *persist_base_ସନ୍ଦେଶ_add;
	ସଂରଚନା mosquitto__callback *persist_base_ସନ୍ଦେଶ_delete;
	ସଂରଚନା mosquitto__callback *persist_retain_ସନ୍ଦେଶ_set;
	ସଂରଚନା mosquitto__callback *persist_retain_ସନ୍ଦେଶ_delete;
	ସଂରଚନା mosquitto__callback *persist_will_add;
	ସଂରଚନା mosquitto__callback *persist_will_delete;
};

/* This is owned by mosquitto__config or mosquitto__listener, and only referred
 * to by other structs */
ସଂରଚନା mosquitto__security_options {
	/* Any options that get added here also need considering
	 * in config__read() with regards whether allow_anonymous
	 * should be disabled when these options are set.
	 */
	ସଂରଚନା mosquitto__unpwd *unpwd;
	ସଂରଚନା mosquitto__psk *psk_id;
	ସଂରଚନା acl_file_data acl_data;
	ସଂରଚନା password_file_data password_data;
	ଅକ୍ଷର *psk_file;
	mosquitto_plugin_id_t **plugins;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin_ଗଣନା;
	int8_t allow_anonymous;
	bool allow_zero_length_clientid;
	ଅକ୍ଷର *auto_id_prefix;
	uint16_t auto_id_prefix_len;
	ସଂରଚନା plugin__callbacks plugin_callbacks;
	mosquitto_plugin_id_t *pid; /* For registering as a "plugin" */
};

#ଯଦି defined(WITH_EPOLL) || defined(WITH_KQUEUE)
ଗଣନା struct_ident {
	id_invalid = 0,
	id_listener = 1,
	id_client = 2,
	id_listener_ws = 3,
};
#endif

ସଂରଚନା mosquitto__listener {
	uint16_t port;
	ଅକ୍ଷର *host;
	ଅକ୍ଷର *bind_interface;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_connections;
	ଅକ୍ଷର *mount_point;
	mosq_sock_t *socks;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା sock_ଗଣନା;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା client_ଗଣନା;
	ଗଣନା mosquitto_protocol protocol;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_domain;
	bool use_username_as_clientid;
	uint8_t max_qos;
	uint16_t max_topic_alias;
	uint16_t max_topic_alias_broker;
#ifdef WITH_TLS
	ଅକ୍ଷର *cafile;
	ଅକ୍ଷର *capath;
	ଅକ୍ଷର *certfile;
	ଅକ୍ଷର *keyfile;
	ଅକ୍ଷର *tls_engine;
	ଅକ୍ଷର *tls_engine_kpass_sha1;
	ଅକ୍ଷର *ciphers;
	ଅକ୍ଷର *ciphers_tls13;
	ଅକ୍ଷର *psk_hint;
	SSL_CTX *ssl_ctx;
	ଅକ୍ଷର *crlfile;
	ଅକ୍ଷର *tls_version;
	bool use_identity_as_username;
	bool use_subject_as_username;
	bool require_certificate;
	bool disable_client_cert_date_checks;
	ଗଣନା mosquitto__keyform tls_keyform;
#endif
#ifdef WITH_WEBSOCKETS
#  ଯଦି WITH_WEBSOCKETS == WS_IS_LWS
	ସଂରଚନା lws_context *ws_context;
	bool ws_in_init;
	ସଂରଚନା lws_protocols *ws_protocol;
#  endif
	ଅକ୍ଷର **ws_origins;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ws_origin_ଗଣନା;
#endif
#ଯଦି defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
	ଅକ୍ଷର *http_dir;
#endif
	ସଂରଚନା mosquitto__security_options *security_options;
#ifdef WITH_UNIX_SOCKETS
	ଅକ୍ଷର *unix_socket_path;
#endif
	bool disable_protocol_v3;
	bool disable_protocol_v4;
	bool disable_protocol_v5;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା enable_proxy_protocol;
	bool proxy_protocol_v2_require_tls;
#ifdef WITH_HTTP_API
	ସଂରଚନା MHD_Daemon *mhd;
#endif
};


ସଂରଚନା mosquitto__listener_sock {
#ଯଦି defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	/* This *must* be the first element in the struct. */
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ident;
#endif
	mosq_sock_t sock;
	ସଂରଚନା mosquitto__listener *listener;
};


/* Callbacks belonging to a specific plugin
 * Doesn't include MOSQ_EVT_CONTROL events.
 */
ସଂରଚନା plugin_own_callback {
	ସଂରଚନା plugin_own_callback *next, *prev;
	MOSQ_FUNC_generic_callback cb_func;
	ଗଣନା mosquitto_plugin_event event;
};

ସଂରଚନା mosquitto_plugin_id_t {
	ସଂରଚନା mosquitto__plugin_config config;
	ସଂରଚନା mosquitto__plugin_lib lib;
	ସଂରଚନା mosquitto__listener *listener;
	ଅକ୍ଷର *plugin_name;
	ଅକ୍ଷର *plugin_version;
	ସଂରଚନା control_endpoint *control_endpoints;
	ସଂରଚନା plugin_own_callback *own_callbacks;
	ସଂରଚନା timespec next_tick;
};

ସଂରଚନା mosquitto__config {
	bool allow_duplicate_messages;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା autosave_interval;
	bool autosave_on_changes;
	bool check_retain_source;
	ଅକ୍ଷର *clientid_prefixes;
	bool connection_messages;
	uint16_t cmd_port[CMD_PORT_LIMIT];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmd_port_ଗଣନା;
	bool daemon;
	bool test_configuration;
	bool enable_control_api;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା global_max_clients;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା global_max_connections;
	ସଂରଚନା mosquitto__listener *default_listener; /* Points to one of `listeners` */
	ସଂରଚନା mosquitto__listener *listeners;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା listener_ଗଣନା;
	bool local_only;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_dest;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_facility;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_type;
	bool log_timestamp;
	ଅକ୍ଷର *log_timestamp_format;
	ଅକ୍ଷର *log_file;
	FILE *log_fptr;
	size_t max_inflight_bytes;
	size_t max_queued_bytes;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_queued_messages;
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
	ଅକ୍ଷର *persistence_location;
	ଅକ୍ଷର *persistence_file;
	ଅକ୍ଷର *persistence_filepath;
	time_t persistent_client_expiration;
	ଅକ୍ଷର *pid_file;
	bool queue_qos0_messages;
	bool per_listener_settings;
	bool retain_available;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା retain_expiry_interval;
	bool set_tcp_nodelay;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା sys_interval;
	bool upgrade_outgoing_qos;
	ଅକ୍ଷର *user;
#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା websockets_log_level;
#endif
	uint16_t packet_buffer_size;
#ifdef WITH_BRIDGE
	ସଂରଚନା mosquitto__bridge **bridges;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା bridge_ଗଣନା;
#endif
	ସଂରଚନା mosquitto__security_options security_options;
};


ସଂରଚନା mosquitto__subshared {
	UT_hash_handle hh;
	ସଂରଚନା mosquitto__subleaf *subs;
	ଅକ୍ଷର name[];
};

ସଂରଚନା mosquitto__subhier {
	UT_hash_handle hh;
	ସଂରଚନା mosquitto__subhier *parent;
	ସଂରଚନା mosquitto__subhier *children;
	ସଂରଚନା mosquitto__subleaf *subs;
	ସଂରଚନା mosquitto__subshared *shared;
	uint16_t topic_len;
	ଅକ୍ଷର topic[];
};

ସଂରଚନା mosquitto__subleaf {
	ସଂରଚନା mosquitto__subleaf *prev;
	ସଂରଚନା mosquitto__subleaf *next;
	ସଂରଚନା mosquitto *context;
	ସଂରଚନା mosquitto__subhier *hier;
	ସଂରଚନା mosquitto__subshared *shared;
	uint32_t identifier;
	uint8_t subscription_options;
	ଅକ୍ଷର topic_filter[];
};

ସଂରଚନା sub__token {
	ସଂରଚନା sub__token *next;
	uint16_t topic_len;
	ଅକ୍ଷର topic[];
};

ସଂରଚନା mosquitto__retainhier {
	UT_hash_handle hh;
	ସଂରଚନା mosquitto__retainhier *parent;
	ସଂରଚନା mosquitto__retainhier *children;
	ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *retained;
	uint16_t topic_len;
	ଅକ୍ଷର topic[];
};

ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ {
	UT_hash_handle hh;
	ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ_2 data;
	ସଂରଚନା mosquitto__listener *source_listener;
	ଅକ୍ଷର **dest_ids;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା dest_id_ଗଣନା;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ref_ଗଣନା;
	ଗଣନା mosquitto_ସନ୍ଦେଶ_origin origin;
	bool stored;
};

ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ {
	ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ_2 data;
	ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *prev;
	ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *next;
	ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ;
};


ସଂରଚନା mosquitto__psk {
	UT_hash_handle hh;
	ଅକ୍ଷର *username;
	ଅକ୍ଷର *password;
};

ସଂରଚନା mosquitto__message_v5 {
	ସଂରଚନା mosquitto__message_v5 *next, *prev;
	ଅକ୍ଷର *topic;
	ଶୂନ୍ୟ *payload;
	mosquitto_property *properties;
	/* clientid is used only by mosquitto_broker_publish*() to indicate
	   this message is for a specific client. */
	ଅକ୍ଷର *clientid;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା payloadlen;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା qos;
	bool retain;
};

ସଂରଚନା mosquitto_db {
	dbid_t last_db_id;
	uint64_t ଗଣ୍ଠି_id_shifted;
	ସଂରଚନା mosquitto__subhier *normal_subs;
	ସଂରଚନା mosquitto__subhier *shared_subs;
	ସଂରଚନା mosquitto__retainhier *retains;
	ସଂରଚନା mosquitto *contexts_by_id;
	ସଂରଚନା mosquitto *contexts_by_sock;
	ସଂରଚନା mosquitto *contexts_by_id_delayed_auth;
	ସଂରଚନା mosquitto *contexts_for_free;
	mosquitto_plugin_id_t **plugins;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin_ଗଣନା;
#ifdef WITH_BRIDGE
	ସଂରଚନା mosquitto **bridges;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା bridge_ଗଣନା;
#endif
	ସଂରଚନା clientid__index_hash *clientid_index_hash;
	ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *ସନ୍ଦେଶ_store;
	time_t now_s; /* Monotonic clock, where possible */
	time_t now_real_s; /* Read clock, for measuring session/message expiry */
	uint64_t ଗଣ୍ଠି_id; /* for unique db ids */
	time_t next_event_ms; /* for mux timeout */
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସନ୍ଦେଶ_store_ଗଣନା;
	unsigned long ସନ୍ଦେଶ_store_bytes;
	ଅକ୍ଷର *config_file;
	ସଂରଚନା mosquitto__config *config;
	bool quiet;
	bool verbose;
#ifdef WITH_SYS_ବୃକ୍ଷ
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା subscription_ଗଣନା;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା shared_subscription_ଗଣନା;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା retained_ଗଣନା;
#endif
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା persistence_changes;
	ସଂରଚନା mosquitto *ll_for_free;
#ifdef WITH_EPOLL
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା epollfd;
#endif
#ifdef WITH_KQUEUE
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା kqueuefd;
#endif
	ସଂରଚନା mosquitto__message_v5 *plugin_msgs;
#ifdef WITH_TLS
	/* tls_keylog can't be in the config struct because it is used
	   before the config is allocated. Config probably
	   shouldn't be separately allocated. */
	ଅକ୍ଷର *tls_keylog;
#endif
	bool shutdown;
};

ଗଣନା mosquitto__bridge_direction {
	bd_out = 0,
	bd_in = 1,
	bd_both = 2,
};

ଗଣନା mosquitto_bridge_start_type {
	bst_automatic = 0,
	bst_lazy = 1,
	bst_manual = 2,
	bst_once = 3,
};

ଗଣନା mosquitto_bridge_reload_type {
	brt_lazy = 0,
	brt_immediate = 1,
};

ସଂରଚନା mosquitto__bridge_topic {
	ସଂରଚନା mosquitto__bridge_topic *next;
	ଅକ୍ଷର *topic;
	ଅକ୍ଷର *local_prefix;
	ଅକ୍ଷର *remote_prefix;
	ଅକ୍ଷର *local_topic; /* topic prefixed with local_prefix */
	ଅକ୍ଷର *remote_topic; /* topic prefixed with remote_prefix */
	ଗଣନା mosquitto__bridge_direction direction;
	uint8_t qos;
};

ସଂରଚନା bridge_address {
	ଅକ୍ଷର *address;
	uint16_t port;
};

ସଂରଚନା mosquitto__bridge {
	ଅକ୍ଷର *name;
	ସଂରଚନା bridge_address *addresses;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cur_address;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା address_ଗଣନା;
	time_t primary_retry;
	mosq_sock_t primary_retry_sock;
	bool round_robin;
	bool try_private;
	bool try_private_accepted;
	bool clean_start;
	int8_t clean_start_local;
	uint16_t keepalive;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା tcp_keepalive_idle;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା tcp_keepalive_interval;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା tcp_keepalive_counter;
#ifdef WITH_TCP_USER_TIMEOUT
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା tcp_user_timeout;
#endif
	ସଂରଚନା mosquitto__bridge_topic *topics;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା topic_ଗଣନା;
	bool topic_remapping;
	ଗଣନା mosquitto__protocol protocol_version;
	time_t restart_t;
	time_t connected_at;
	ଅକ୍ଷର *remote_clientid;
	ଅକ୍ଷର *remote_username;
	ଅକ୍ଷର *remote_password;
	ଅକ୍ଷର *local_clientid;
	ଅକ୍ଷର *local_username;
	ଅକ୍ଷର *local_password;
	ଅକ୍ଷର *notification_topic;
	ଅକ୍ଷର *bind_address;
	bool notifications;
	bool notifications_local_only;
	ଗଣନା mosquitto_bridge_start_type start_type;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା idle_timeout;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା restart_timeout;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା backoff_base;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା backoff_cap;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା stable_connection_period;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା threshold;
	uint32_t maximum_packet_size;
	uint32_t session_expiry_interval;
	uint16_t receive_maximum;
	bool lazy_reconnect;
	bool attempt_unsubscribe;
	bool initial_notification_done;
	bool outgoing_retain;
	ଗଣନା mosquitto_bridge_reload_type reload_type;
	uint16_t max_topic_alias;
#ifdef WITH_TLS
	bool tls_insecure;
	bool tls_ocsp_required;
	bool tls_use_os_certs;
	ଅକ୍ଷର *tls_cafile;
	ଅକ୍ଷର *tls_capath;
	ଅକ୍ଷର *tls_certfile;
	ଅକ୍ଷର *tls_keyfile;
	ଅକ୍ଷର *tls_version;
	ଅକ୍ଷର *tls_alpn;
	ଅକ୍ଷର *tls_ciphers;
	ଅକ୍ଷର *tls_13_ciphers;
#  ifdef FINAL_WITH_TLS_PSK
	ଅକ୍ଷର *tls_psk_identity;
	ଅକ୍ଷର *tls_psk;
#  endif
#endif
};

#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
ସଂରଚନା libws_mqtt_hack {
	ଅକ୍ଷର *http_dir;
	ସଂରଚନା mosquitto__listener *listener;
};

ସଂରଚନା libws_mqtt_data {
	ସଂରଚନା mosquitto *mosq;
};
#endif

#ଅନ୍ତର୍ଭାଵ <net_mosq.h>


ସଂରଚନା control_endpoint {
	ସଂରଚନା control_endpoint *next, *prev;
	ଅକ୍ଷର topic[];
};

ବାହ୍ୟ ସଂରଚନା mosquitto_db db;

/* ============================================================
 * Main functions
 * ============================================================ */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_main_loop(ସଂରଚନା mosquitto__listener_sock *listensock, ପୂର୍ଣ୍ଣସଂଖ୍ୟା listensock_ଗଣନା);
ଶୂନ୍ୟ loop__update_next_event(time_t new_ms);

/* ============================================================
 * Config functions
 * ============================================================ */
/* Initialise config struct to default values. */
ଶୂନ୍ୟ config__init(ସଂରଚନା mosquitto__config *config);
#ifdef WITH_BRIDGE
ଶୂନ୍ୟ config__bridge_cleanup(ସଂରଚନା mosquitto__bridge *bridge);
#endif
/* Parse command line options into config. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା config__parse_args(ସଂରଚନା mosquitto__config *config, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର *argv[]);
/* Read configuration data from config->config_file into config.
 * If reload is true, don't process config options that shouldn't be reloaded (listeners etc)
 * Returns 0 on success, 1 if there is a configuration error or if a file cannot be opened.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା config__read(ସଂରଚନା mosquitto__config *config, bool reload);
/* Free all config data. */
ଶୂନ୍ୟ config__cleanup(ସଂରଚନା mosquitto__config *config);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା config__get_dir_files(ଅଚଳ ଅକ୍ଷର *include_dir, ଅକ୍ଷର ***files, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *file_ଗଣନା);

/* ============================================================
 * Server send functions
 * ============================================================ */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା send__connack(ସଂରଚନା mosquitto *context, uint8_t ack, uint8_t reason_code, ଅଚଳ mosquitto_property *properties);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା send__suback(ସଂରଚନା mosquitto *context, uint16_t mid, uint32_t payloadlen, ଅଚଳ ଶୂନ୍ୟ *payload);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା send__unsuback(ସଂରଚନା mosquitto *context, uint16_t mid, ପୂର୍ଣ୍ଣସଂଖ୍ୟା reason_code_ଗଣନା, uint8_t *reason_codes, ଅଚଳ mosquitto_property *properties);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା send__auth(ସଂରଚନା mosquitto *context, uint8_t reason_code, ଅଚଳ ଶୂନ୍ୟ *auth_data, uint16_t auth_data_len);

/* ============================================================
 * Network functions
 * ============================================================ */
ଶୂନ୍ୟ net__broker_init(ଶୂନ୍ୟ);
ଶୂନ୍ୟ net__broker_cleanup(ଶୂନ୍ୟ);
ସଂରଚନା mosquitto *net__socket_accept(ସଂରଚନା mosquitto__listener_sock *listensock);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା net__socket_listen(ସଂରଚନା mosquitto__listener *listener);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା net__socket_get_address(mosq_sock_t sock, ଅକ୍ଷର *buf, size_t len, uint16_t *remote_address);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା net__tls_load_verify(ସଂରଚନା mosquitto__listener *listener);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା net__tls_server_ctx(ସଂରଚନା mosquitto__listener *listener);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା net__load_certificates(ସଂରଚନା mosquitto__listener *listener);

/* ============================================================
 * Read handling functions
 * ============================================================ */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା handle__packet(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା handle__connack(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା handle__connect(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା handle__disconnect(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା handle__publish(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା handle__subscribe(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା handle__unsubscribe(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା handle__auth(ସଂରଚନା mosquitto *context);

/* ============================================================
 * Database handling
 * ============================================================ */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__open(ସଂରଚନା mosquitto__config *config);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__close(ଶୂନ୍ୟ);
#ifdef WITH_PERSISTENCE
ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__backup(bool shutdown);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__restore(ଶୂନ୍ୟ);
#endif
/* Return the number of in-flight messages in count. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା db_message_ଗଣନା(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଗଣନା_2);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__message_delete_outgoing(ସଂରଚନା mosquitto *context, uint16_t mid, ଗଣନା mosquitto_ସନ୍ଦେଶ_state expect_state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା qos);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା db_message_ଭର୍ତ୍ତି_outgoing(ସଂରଚନା mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ, uint32_t subscription_identifier, bool update, bool persist);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା db_message_ଭର୍ତ୍ତି_incoming(ସଂରଚନା mosquitto *context, uint64_t cmsg_id, ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ, bool persist);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__message_remove_incoming(ସଂରଚନା mosquitto *context, uint16_t mid);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__message_release_incoming(ସଂରଚନା mosquitto *context, uint16_t mid);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__message_update_outgoing(ସଂରଚନା mosquitto *context, uint16_t mid, ଗଣନା mosquitto_ସନ୍ଦେଶ_state state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା qos, bool persist);
ଶୂନ୍ୟ db__message_dequeue_first(ସଂରଚନା mosquitto *context, ସଂରଚନା mosquitto_ସନ୍ଦେଶ_data *ସନ୍ଦେଶ_data);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__messages_delete(ସଂରଚନା mosquitto *context, bool force_free);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__messages_delete_incoming(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__messages_delete_outgoing(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__messages_easy_queue(ସଂରଚନା mosquitto *context, ଅଚଳ ଅକ୍ଷର *topic, uint8_t qos, uint32_t payloadlen, ଅଚଳ ଶୂନ୍ୟ *payload, ପୂର୍ଣ୍ଣସଂଖ୍ୟା retain, uint32_t message_expiry_interval, mosquitto_property **properties);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__message_store(ଅଚଳ ସଂରଚନା mosquitto *source, ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ, uint32_t *message_expiry_interval, ଗଣନା mosquitto_ସନ୍ଦେଶ_origin origin);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__message_store_find(ସଂରଚନା mosquitto *context, uint16_t mid, ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ **client_ସନ୍ଦେଶ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା db_ସନ୍ଦେଶ_store_add(ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ);
ଶୂନ୍ୟ db_ସନ୍ଦେଶ_store_remove(ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ, bool notify);
ଶୂନ୍ୟ db_ସନ୍ଦେଶ_store_ref_inc(ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ);
ଶୂନ୍ୟ db_ସନ୍ଦେଶ_store_ref_dec(ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ **base_ସନ୍ଦେଶ);
ଶୂନ୍ୟ db_ସନ୍ଦେଶ_store_clean(ଶୂନ୍ୟ);
ଶୂନ୍ୟ db_ସନ୍ଦେଶ_store_compact(ଶୂନ୍ୟ);
ଶୂନ୍ୟ db_ସନ୍ଦେଶ_store_free(ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__message_reconnect_reset(ସଂରଚନା mosquitto *context);
bool db__ready_for_flight(ସଂରଚନା mosquitto *context, ଗଣନା mosquitto_ସନ୍ଦେଶ_direction dir, ପୂର୍ଣ୍ଣସଂଖ୍ୟା qos);
bool db__ready_for_queue(ସଂରଚନା mosquitto *context, ପୂର୍ଣ୍ଣସଂଖ୍ୟା qos, ସଂରଚନା mosquitto_ସନ୍ଦେଶ_data *ସନ୍ଦେଶ_data);
ଶୂନ୍ୟ sys_ବୃକ୍ଷ_init(ଶୂନ୍ୟ);
ଶୂନ୍ୟ sys_ବୃକ୍ଷ_update(bool force);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__message_write_inflight_out_all(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__message_write_inflight_out_latest(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__message_write_queued_out(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__message_write_queued_in(ସଂରଚନା mosquitto *context);
ଶୂନ୍ୟ db_ସନ୍ଦେଶ_add_to_inflight_stats(ସଂରଚନା mosquitto_ସନ୍ଦେଶ_data *ସନ୍ଦେଶ_data, ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *ସନ୍ଦେଶ);
ଶୂନ୍ୟ db_ସନ୍ଦେଶ_add_to_queued_stats(ସଂରଚନା mosquitto_ସନ୍ଦେଶ_data *ସନ୍ଦେଶ_data, ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *ସନ୍ଦେଶ);
uint64_t db_new_ସନ୍ଦେଶ_id(ଶୂନ୍ୟ);
ଶୂନ୍ୟ db__expire_all_messages(ସଂରଚନା mosquitto *context);
ଶୂନ୍ୟ db__check_acl_of_all_messages(ସଂରଚନା mosquitto *context);

/* ============================================================
 * Subscription functions
 * ============================================================ */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା sub__init(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା sub__add(ସଂରଚନା mosquitto *context, ଅଚଳ ସଂରଚନା mosquitto_subscription *sub);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା sub__remove(ସଂରଚନା mosquitto *context, ଅଚଳ ଅକ୍ଷର *sub, uint8_t *reason);
ଶୂନ୍ୟ sub_ବୃକ୍ଷ_print(ସଂରଚନା mosquitto__subhier *ମୂଳ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା level);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା sub__clean_session(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା sub__messages_queue(ଅଚଳ ଅକ୍ଷର *source_id, uint8_t qos, ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ **base_ସନ୍ଦେଶ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା sub__topic_tokenise(ଅଚଳ ଅକ୍ଷର *subtopic, ଅକ୍ଷର **local_sub, ଅକ୍ଷର ***topics, ଅଚଳ ଅକ୍ଷର **sharename);
ଶୂନ୍ୟ sub__topic_tokens_free(ସଂରଚନା sub__token *tokens);

/* ============================================================
 * Context functions
 * ============================================================ */
ସଂରଚନା mosquitto *context__init(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା context__init_sock(ସଂରଚନା mosquitto *context, mosq_sock_t sock, bool get_address);
ଶୂନ୍ୟ context__cleanup(ସଂରଚନା mosquitto *context, bool force_free);
ଶୂନ୍ୟ context__disconnect(ସଂରଚନା mosquitto *context, ପୂର୍ଣ୍ଣସଂଖ୍ୟା reason);
ଶୂନ୍ୟ context__add_to_disused(ସଂରଚନା mosquitto *context);
ଶୂନ୍ୟ context__free_disused(ଶୂନ୍ୟ);
ଶୂନ୍ୟ context__send_will(ସଂରଚନା mosquitto *context);
ଶୂନ୍ୟ context__add_to_by_id(ସଂରଚନା mosquitto *context);
ଶୂନ୍ୟ context__remove_from_by_id(ସଂରଚନା mosquitto *context);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା connect__on_authorised(ସଂରଚନା mosquitto *context, ଶୂନ୍ୟ *auth_data_out, uint16_t auth_data_out_len);


/* ============================================================
 * Control functions
 * ============================================================ */
#ifdef WITH_CONTROL
ପୂର୍ଣ୍ଣସଂଖ୍ୟା control__process(ସଂରଚନା mosquitto *context, ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ);
ଶୂନ୍ୟ control__cleanup(ଶୂନ୍ୟ);
#endif
ପୂର୍ଣ୍ଣସଂଖ୍ୟା control__register_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ଅଚଳ ଅକ୍ଷର *topic, ଶୂନ୍ୟ *userdata);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା control__unregister_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ଅଚଳ ଅକ୍ଷର *topic);
ଶୂନ୍ୟ control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier);


/* ============================================================
 * Logging functions
 * ============================================================ */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା log__init(ସଂରଚନା mosquitto__config *config);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା log__close(ସଂରଚନା mosquitto__config *config);
ଶୂନ୍ୟ log__internal(ଅଚଳ ଅକ୍ଷର *fmt, ...) __attribute__((format(printf, 1, 2)));

/* ============================================================
 * Bridge functions
 * ============================================================ */
#ifdef WITH_BRIDGE
ଶୂନ୍ୟ bridge__start_all(ଶୂନ୍ୟ);
ଶୂନ୍ୟ bridge__reload(ଶୂନ୍ୟ);
ଶୂନ୍ୟ bridge__db_cleanup(ଶୂନ୍ୟ);
ଶୂନ୍ୟ bridge__cleanup(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା bridge__connect(ସଂରଚନା mosquitto *context);
#ଯଦି defined(__GLIBC__) && defined(WITH_ADNS)
ପୂର୍ଣ୍ଣସଂଖ୍ୟା bridge__connect_step3(ସଂରଚନା mosquitto *context);
#endif
ପୂର୍ଣ୍ଣସଂଖ୍ୟା bridge__on_connect(ସଂରଚନା mosquitto *context);
ଶୂନ୍ୟ bridge_check(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା bridge__register_local_connections(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା bridge__add_topic(ସଂରଚନା mosquitto__bridge *bridge, ଅଚଳ ଅକ୍ଷର *topic, ଗଣନା mosquitto__bridge_direction direction, uint8_t qos, ଅଚଳ ଅକ୍ଷର *local_prefix, ଅଚଳ ଅକ୍ଷର *remote_prefix);
ଶୂନ୍ୟ bridge__cleanup_topics(ସଂରଚନା mosquitto__bridge *bridge);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା bridge__remap_topic_in(ସଂରଚନା mosquitto *context, ଅକ୍ଷର **topic);
#endif

/* ============================================================
 * IO multiplex related functions
 * ============================================================ */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux__init(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux__add_listeners(ସଂରଚନା mosquitto__listener_sock *listensock, ପୂର୍ଣ୍ଣସଂଖ୍ୟା listensock_ଗଣନା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux__delete_listeners(ସଂରଚନା mosquitto__listener_sock *listensock, ପୂର୍ଣ୍ଣସଂଖ୍ୟା listensock_ଗଣନା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux__loop_prepare(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux__new(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux__add_out(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux__remove_out(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux__delete(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux__wait(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux__handle(ସଂରଚନା mosquitto__listener_sock *listensock, ପୂର୍ଣ୍ଣସଂଖ୍ୟା listensock_ଗଣନା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux__cleanup(ଶୂନ୍ୟ);

/* ============================================================
 * Listener related functions
 * ============================================================ */
ବାହ୍ୟ ସଂରଚନା mosquitto__listener_sock *g_listensock;
ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା g_listensock_ଗଣନା;

ଶୂନ୍ୟ listener__set_defaults(ସଂରଚନା mosquitto__listener *listener);
ଶୂନ୍ୟ listeners__reload_all_certificates(ଶୂନ୍ୟ);
#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
ଶୂନ୍ୟ listeners__add_websockets(ସଂରଚନା lws_context *ws_context, mosq_sock_t fd);
#endif
ପୂର୍ଣ୍ଣସଂଖ୍ୟା listeners__start(ଶୂନ୍ୟ);
ଶୂନ୍ୟ listeners__stop(ଶୂନ୍ୟ);

/* ============================================================
 * Plugin related functions
 * ============================================================ */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin__load_v5(mosquitto_plugin_id_t *plugin, ଶୂନ୍ୟ *lib);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin__load_v4(mosquitto_plugin_id_t *plugin, ଶୂନ୍ୟ *lib);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin__load_v3(mosquitto_plugin_id_t *plugin, ଶୂନ୍ୟ *lib);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin__load_v2(mosquitto_plugin_id_t *plugin, ଶୂନ୍ୟ *lib);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା acl__pre_check(mosquitto_plugin_id_t *plugin, ସଂରଚନା mosquitto *context, ପୂର୍ଣ୍ଣସଂଖ୍ୟା access);

ଶୂନ୍ୟ LIB_ERROR(ଶୂନ୍ୟ);
ଶୂନ୍ୟ plugin__handle_connect(ସଂରଚନା mosquitto *context);
ଶୂନ୍ୟ plugin__handle_disconnect(ସଂରଚନା mosquitto *context, ପୂର୍ଣ୍ଣସଂଖ୍ୟା reason);
ଶୂନ୍ୟ plugin__handle_client_offline(ସଂରଚନା mosquitto *context, ପୂର୍ଣ୍ଣସଂଖ୍ୟା reason);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin__handle_message_in(ସଂରଚନା mosquitto *context, ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ_2 *base_ସନ୍ଦେଶ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin__handle_message_out(ସଂରଚନା mosquitto *context, ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ_2 *base_ସନ୍ଦେଶ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin__handle_subscribe(ସଂରଚନା mosquitto *context, ସଂରଚନା mosquitto_subscription *sub);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin__handle_unsubscribe(ସଂରଚନା mosquitto *context, ସଂରଚନା mosquitto_subscription *sub);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin__handle_reload(ଶୂନ୍ୟ);
ଶୂନ୍ୟ plugin__handle_tick(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin__callback_unregister_all(mosquitto_plugin_id_t *identifier);
ଶୂନ୍ୟ plugin_persist__handle_restore(ଶୂନ୍ୟ);
ଶୂନ୍ୟ plugin_persist__handle_client_add(ସଂରଚନା mosquitto *context);
ଶୂନ୍ୟ plugin_persist__handle_client_delete(ସଂରଚନା mosquitto *context);
ଶୂନ୍ୟ plugin_persist__handle_client_update(ସଂରଚନା mosquitto *context);
ଶୂନ୍ୟ plugin_persist__handle_subscription_add(ସଂରଚନା mosquitto *context, ଅଚଳ ସଂରଚନା mosquitto_subscription *sub);
ଶୂନ୍ୟ plugin_persist__handle_subscription_delete(ସଂରଚନା mosquitto *context, ଅକ୍ଷର *sub);
ଶୂନ୍ୟ plugin_persist_handle_client_ସନ୍ଦେଶ_add(ସଂରଚନା mosquitto *context, ଅଚଳ ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *cmsg);
ଶୂନ୍ୟ plugin_persist_handle_client_ସନ୍ଦେଶ_delete(ସଂରଚନା mosquitto *context, ଅଚଳ ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *cmsg);
ଶୂନ୍ୟ plugin_persist_handle_client_ସନ୍ଦେଶ_update(ସଂରଚନା mosquitto *context, ଅଚଳ ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *cmsg);
ଶୂନ୍ୟ plugin_persist_handle_base_ସନ୍ଦେଶ_add(ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ);
ଶୂନ୍ୟ plugin_persist_handle_base_ସନ୍ଦେଶ_delete(ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ);
ଶୂନ୍ୟ plugin_persist_handle_retain_ସନ୍ଦେଶ_set(ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ);
ଶୂନ୍ୟ plugin_persist_handle_retain_ସନ୍ଦେଶ_delete(ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ);
ଶୂନ୍ୟ plugin_persist__handle_will_add(ସଂରଚନା mosquitto *context);
ଶୂନ୍ୟ plugin_persist__handle_will_delete(ସଂରଚନା mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା keepalive__init(ଶୂନ୍ୟ);
ଶୂନ୍ୟ keepalive__cleanup(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା keepalive__add(ସଂରଚନା mosquitto *context);
ଶୂନ୍ୟ keepalive__check(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା keepalive__remove(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା keepalive__update(ସଂରଚନା mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା property__process_connect(ସଂରଚନା mosquitto *context, mosquitto_property **props);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା property__process_will(ସଂରଚନା mosquitto *context, ସଂରଚନା mosquitto_message_all *ସନ୍ଦେଶ, mosquitto_property **props);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା property__process_publish(ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ, mosquitto_property **props, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *topic_alias, uint32_t *message_expiry_interval, bool is_bridge);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା property__process_disconnect(ସଂରଚନା mosquitto *context, mosquitto_property **props);

/* ============================================================
 * Retain tree related functions
 * ============================================================ */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା retain__init(ଶୂନ୍ୟ);
ଶୂନ୍ୟ retain__clean(ସଂରଚନା mosquitto__retainhier **retainhier);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା retain__queue(ସଂରଚନା mosquitto *context, ଅଚଳ ସଂରଚନା mosquitto_subscription *sub);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା retain__store(ଅଚଳ ଅକ୍ଷର *topic, ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ, ଅକ୍ଷର **split_topics, bool persist);
ଶୂନ୍ୟ retain__expiry_check(ଶୂନ୍ୟ);
ଶୂନ୍ୟ retain__expire(ସଂରଚନା mosquitto__retainhier **retainhier);

/* ============================================================
 * Security related functions
 * ============================================================ */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା acl__find_acls(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin__load_all(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin__unload_all(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା config__plugin_add_secopt(mosquitto_plugin_id_t *plugin, ସଂରଚନା mosquitto__security_options *security_options);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_security_init(bool reload);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_security_cleanup(bool reload);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_acl_check(ସଂରଚନା mosquitto *context, ଅଚଳ ଅକ୍ଷର *topic, uint32_t payloadlen, ଶୂନ୍ୟ *payload, uint8_t qos, bool retain, mosquitto_property *properties, ପୂର୍ଣ୍ଣସଂଖ୍ୟା access);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_basic_auth(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_psk_ଚାବି_get(ସଂରଚନା mosquitto *context, ଅଚଳ ଅକ୍ଷର *hint, ଅଚଳ ଅକ୍ଷର *identity, ଅକ୍ଷର *ଚାବି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_ଚାବି_len);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_security_init_default(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_security_apply_default(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_security_cleanup_default(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_psk_ଚାବି_get_default(ସଂରଚନା mosquitto *context, ଅଚଳ ଅକ୍ଷର *hint, ଅଚଳ ଅକ୍ଷର *identity, ଅକ୍ଷର *ଚାବି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_ଚାବି_len);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା broker_acl_file__init(ଶୂନ୍ୟ);
ଶୂନ୍ୟ broker_acl_file__cleanup(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା broker_password_file__init(ଶୂନ୍ୟ);
ଶୂନ୍ୟ broker_password_file__cleanup(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା psk_file__init(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା psk_file__cleanup(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_security_auth_start(ସଂରଚନା mosquitto *context, bool reauth, ଅଚଳ ଶୂନ୍ୟ *data_in, uint16_t data_in_len, ଶୂନ୍ୟ **data_out, uint16_t *data_out_len);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_security_auth_continue(ସଂରଚନା mosquitto *context, ଅଚଳ ଶୂନ୍ୟ *data_in, uint16_t data_len, ଶୂନ୍ୟ **data_out, uint16_t *data_out_len);

ଶୂନ୍ୟ unpwd__free_item(ସଂରଚନା mosquitto__unpwd **unpwd, ସଂରଚନା mosquitto__unpwd *item);

/* ============================================================
 * Session expiry
 * ============================================================ */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା session_expiry__add(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା session_expiry__add_from_persistence(ସଂରଚନା mosquitto *context, time_t expiry_time);
ଶୂନ୍ୟ session_expiry__add_on_shutdown(ଶୂନ୍ୟ);
ଶୂନ୍ୟ session_expiry__remove(ସଂରଚନା mosquitto *context);
ଶୂନ୍ୟ session_expiry__remove_all(ଶୂନ୍ୟ);
ଶୂନ୍ୟ session_expiry__check(ଶୂନ୍ୟ);
ଶୂନ୍ୟ session_expiry__send_all(ଶୂନ୍ୟ);

/* ============================================================
 * Signals
 * ============================================================ */
ଶୂନ୍ୟ signal__setup(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା signal__flag_check(ଶୂନ୍ୟ);

/* ============================================================
 * Window service and signal related functions
 * ============================================================ */
#ଯଦି defined(WIN32) || defined(__CYGWIN__)
ଶୂନ୍ୟ service_install(ଅକ୍ଷର *name);
ଶୂନ୍ୟ service_uninstall(ଅକ୍ଷର *name);
ଶୂନ୍ୟ service_run(ଅକ୍ଷର *name);

#ifdef WIN32
DWORD WINAPI SigThreadProc(ଶୂନ୍ୟ *data);
#endif
#endif

/* ============================================================
 * Watchdog
 * ============================================================ */
ଶୂନ୍ୟ watchdog__init(ଶୂନ୍ୟ);
ଶୂନ୍ୟ watchdog__check(ଶୂନ୍ୟ);

/* ============================================================
 * Websockets related functions
 * ============================================================ */
#ifdef WITH_WEBSOCKETS
#  ଯଦି WITH_WEBSOCKETS == WS_IS_LWS
ଶୂନ୍ୟ mosq_websockets_init(ସଂରଚନା mosquitto__listener *listener, ଅଚଳ ସଂରଚନା mosquitto__config *conf);
#  endif
ପୂର୍ଣ୍ଣସଂଖ୍ୟା http__context_init(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା http__context_cleanup(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା http__read(ସଂରଚନା mosquitto *context);
#endif
ଶୂନ୍ୟ do_disconnect(ସଂରଚନା mosquitto *context, ପୂର୍ଣ୍ଣସଂଖ୍ୟା reason);

/* ============================================================
 * PROXY related functions
 * ============================================================ */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା proxy_v2__read(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା proxy_v1__read(ସଂରଚନା mosquitto *context);

/* ============================================================
 * Will delay
 * ============================================================ */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା will_delay__add(ସଂରଚନା mosquitto *context);
ଶୂନ୍ୟ will_delay__check(ଶୂନ୍ୟ);
ଶୂନ୍ୟ will_delay__send_all(ଶୂନ୍ୟ);
ଶୂନ୍ୟ will_delay__remove(ସଂରଚନା mosquitto *mosq);

/* ============================================================
 * HTTP Info
 * ============================================================ */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା http_api__start_local(ସଂରଚନା mosquitto__listener *listener);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା http_api__start(ସଂରଚନା mosquitto__listener *listener);
ଶୂନ୍ୟ http_api__stop(ସଂରଚନା mosquitto__listener *listener);


/* ============================================================
 * Other
 * ============================================================ */
#ifdef WITH_XTREPORT
ଶୂନ୍ୟ xtreport(ଶୂନ୍ୟ);
#endif

ଶୂନ୍ୟ broker_control__init(ଶୂନ୍ୟ);
ଶୂନ୍ୟ broker_control__cleanup(ଶୂନ୍ୟ);
ଶୂନ୍ୟ broker_control__reload(ଶୂନ୍ୟ);

#endif
