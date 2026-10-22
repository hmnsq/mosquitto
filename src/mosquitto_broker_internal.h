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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MOSQUITTO_BROKER_ਅੰਦਰੂਨੀ_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOSQUITTO_BROKER_ਅੰਦਰੂਨੀ_H

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>

#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#  ਜੇ LWS_LIBRARY_ਸੰਸਕਰਣ_ਸੰਖਿਆ >= 3002000 && !defined(LWS_WITH_EXTERNAL_POLL)
#    warning "libwebsockets is not compiled with LWS_WITH_EXTERNAL_POLL support. Websocket performance will be unusable."
#  endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_HTTP_API
#  include <microhttpd.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __linux__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WITH_TCP_USER_TIMEOUT
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/broker.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/broker_plugin.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "logging_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "tls_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uthash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "acl_file.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "password_file.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __GNUC__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __attribute__(attrib)
#ਜੇ_ਅੰਤ

/* Log destinations */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MQTT3_LOG_NONE 0x00
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MQTT3_LOG_SYSLOG 0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MQTT3_LOG_FILE 0x02
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MQTT3_LOG_STDOUT 0x04
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MQTT3_LOG_STDERR 0x08
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MQTT3_LOG_TOPIC 0x10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MQTT3_LOG_DLT 0x20
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ANDROID
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MQTT3_LOG_ANDROID 0x40
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MQTT3_LOG_ALL 0xFF

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMD_ਪੋਰਟ_ਹੱਦਬੰਦੀ 10

ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint64_t dbid_t;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_plugin_init_v5)(mosquitto_plugin_id_t *, ਖਾਲੀ **, ਬਣਤਰ mosquitto_opt *, ਪੂਰਨ);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_plugin_cleanup_v5)(ਖਾਲੀ *, ਬਣਤਰ mosquitto_opt *, ਪੂਰਨ);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_auth_plugin_init_v4)(ਖਾਲੀ **, ਬਣਤਰ mosquitto_opt *, ਪੂਰਨ);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_auth_plugin_cleanup_v4)(ਖਾਲੀ *, ਬਣਤਰ mosquitto_opt *, ਪੂਰਨ);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_auth_plugin_security_init_v4)(ਖਾਲੀ *, ਬਣਤਰ mosquitto_opt *, ਪੂਰਨ, bool);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_auth_plugin_security_cleanup_v4)(ਖਾਲੀ *, ਬਣਤਰ mosquitto_opt *, ਪੂਰਨ, bool);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_auth_plugin_acl_check_v4)(ਖਾਲੀ *, ਪੂਰਨ, ਬਣਤਰ mosquitto *, ਬਣਤਰ mosquitto_acl_ਸੁਨੇਹਾ *);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_auth_plugin_unpwd_check_v4)(ਖਾਲੀ *, ਬਣਤਰ mosquitto *, ਅਚਲ ਅੱਖਰ *, ਅਚਲ ਅੱਖਰ *);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_auth_plugin_psk_ਕੁੰਜੀ_get_v4)(ਖਾਲੀ *, ਬਣਤਰ mosquitto *, ਅਚਲ ਅੱਖਰ *, ਅਚਲ ਅੱਖਰ *, ਅੱਖਰ *, ਪੂਰਨ);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_auth_plugin_auth_ਸ਼ੁਰੂ_v4)(ਖਾਲੀ *, ਬਣਤਰ mosquitto *, ਅਚਲ ਅੱਖਰ *, bool, ਅਚਲ ਖਾਲੀ *, uint16_t, ਖਾਲੀ **, uint16_t *);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_auth_plugin_auth_continue_v4)(ਖਾਲੀ *, ਬਣਤਰ mosquitto *, ਅਚਲ ਅੱਖਰ *, ਅਚਲ ਖਾਲੀ *, uint16_t, ਖਾਲੀ **, uint16_t *);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_auth_plugin_init_v3)(ਖਾਲੀ **, ਬਣਤਰ mosquitto_opt *, ਪੂਰਨ);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_auth_plugin_cleanup_v3)(ਖਾਲੀ *, ਬਣਤਰ mosquitto_opt *, ਪੂਰਨ);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_auth_plugin_security_init_v3)(ਖਾਲੀ *, ਬਣਤਰ mosquitto_opt *, ਪੂਰਨ, bool);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_auth_plugin_security_cleanup_v3)(ਖਾਲੀ *, ਬਣਤਰ mosquitto_opt *, ਪੂਰਨ, bool);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_auth_plugin_acl_check_v3)(ਖਾਲੀ *, ਪੂਰਨ, ਅਚਲ ਬਣਤਰ mosquitto *, ਬਣਤਰ mosquitto_acl_ਸੁਨੇਹਾ *);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_auth_plugin_unpwd_check_v3)(ਖਾਲੀ *, ਅਚਲ ਬਣਤਰ mosquitto *, ਅਚਲ ਅੱਖਰ *, ਅਚਲ ਅੱਖਰ *);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_auth_plugin_psk_ਕੁੰਜੀ_get_v3)(ਖਾਲੀ *, ਅਚਲ ਬਣਤਰ mosquitto *, ਅਚਲ ਅੱਖਰ *, ਅਚਲ ਅੱਖਰ *, ਅੱਖਰ *, ਪੂਰਨ);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_auth_plugin_init_v2)(ਖਾਲੀ **, ਬਣਤਰ mosquitto_auth_opt *, ਪੂਰਨ);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_auth_plugin_cleanup_v2)(ਖਾਲੀ *, ਬਣਤਰ mosquitto_auth_opt *, ਪੂਰਨ);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_auth_plugin_security_init_v2)(ਖਾਲੀ *, ਬਣਤਰ mosquitto_auth_opt *, ਪੂਰਨ, bool);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_auth_plugin_security_cleanup_v2)(ਖਾਲੀ *, ਬਣਤਰ mosquitto_auth_opt *, ਪੂਰਨ, bool);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_auth_plugin_acl_check_v2)(ਖਾਲੀ *, ਅਚਲ ਅੱਖਰ *, ਅਚਲ ਅੱਖਰ *, ਅਚਲ ਅੱਖਰ *, ਪੂਰਨ);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_auth_plugin_unpwd_check_v2)(ਖਾਲੀ *, ਅਚਲ ਅੱਖਰ *, ਅਚਲ ਅੱਖਰ *);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_auth_plugin_psk_ਕੁੰਜੀ_get_v2)(ਖਾਲੀ *, ਅਚਲ ਅੱਖਰ *, ਅਚਲ ਅੱਖਰ *, ਅੱਖਰ *, ਪੂਰਨ);


ਗਿਣਤੀ mosquitto_ਸੁਨੇਹਾ_origin {
	mosq_mo_client = 0,
	mosq_mo_broker = 1,
};

ਬਣਤਰ mosquitto__plugin_lib {
	ਖਾਲੀ *lib;
	ਖਾਲੀ *user_ਅੰਕੜੇ;
	ਪੂਰਨ (*plugin_ਸੰਸਕਰਣ)(ਖਾਲੀ);
	ਬਣਤਰ mosquitto_plugin_id_t *identifier;

	FUNC_plugin_init_v5 plugin_init_v5;
	FUNC_plugin_cleanup_v5 plugin_cleanup_v5;

	FUNC_auth_plugin_init_v4 plugin_init_v4;
	FUNC_auth_plugin_cleanup_v4 plugin_cleanup_v4;
	FUNC_auth_plugin_security_init_v4 security_init_v4;
	FUNC_auth_plugin_security_cleanup_v4 security_cleanup_v4;
	FUNC_auth_plugin_acl_check_v4 acl_check_v4;
	FUNC_auth_plugin_unpwd_check_v4 unpwd_check_v4;
	FUNC_auth_plugin_psk_ਕੁੰਜੀ_get_v4 psk_ਕੁੰਜੀ_get_v4;
	FUNC_auth_plugin_auth_ਸ਼ੁਰੂ_v4 auth_ਸ਼ੁਰੂ_v4;
	FUNC_auth_plugin_auth_continue_v4 auth_continue_v4;

	FUNC_auth_plugin_init_v3 plugin_init_v3;
	FUNC_auth_plugin_cleanup_v3 plugin_cleanup_v3;
	FUNC_auth_plugin_security_init_v3 security_init_v3;
	FUNC_auth_plugin_security_cleanup_v3 security_cleanup_v3;
	FUNC_auth_plugin_acl_check_v3 acl_check_v3;
	FUNC_auth_plugin_unpwd_check_v3 unpwd_check_v3;
	FUNC_auth_plugin_psk_ਕੁੰਜੀ_get_v3 psk_ਕੁੰਜੀ_get_v3;

	FUNC_auth_plugin_init_v2 plugin_init_v2;
	FUNC_auth_plugin_cleanup_v2 plugin_cleanup_v2;
	FUNC_auth_plugin_security_init_v2 security_init_v2;
	FUNC_auth_plugin_security_cleanup_v2 security_cleanup_v2;
	FUNC_auth_plugin_acl_check_v2 acl_check_v2;
	FUNC_auth_plugin_unpwd_check_v2 unpwd_check_v2;
	FUNC_auth_plugin_psk_ਕੁੰਜੀ_get_v2 psk_ਕੁੰਜੀ_get_v2;
	ਪੂਰਨ ਸੰਸਕਰਣ_2;
};

ਬਣਤਰ mosquitto__plugin_config {
	ਅੱਖਰ *ਰਸਤਾ;
	ਅੱਖਰ *name;
	ਬਣਤਰ mosquitto_opt *options;
	ਬਣਤਰ mosquitto__security_options **security_options;
	ਪੂਰਨ option_ਗਿਣਤੀ;
	ਪੂਰਨ security_option_ਗਿਣਤੀ;
	bool deny_special_chars;
};

ਬਣਤਰ mosquitto__callback {
	UT_hash_handle hh; /* For callbacks that register for e.g. a specific topic */
	ਬਣਤਰ mosquitto__callback *ਅਗਲਾ, *prev; /* For typical callbacks */
	MOSQ_FUNC_generic_callback cb;
	ਖਾਲੀ *userdata;
	ਸੰਘ {
		ਅੱਖਰ *topic;
		ਬਣਤਰ timespec ਅਗਲਾ_tick;
	} ਅੰਕੜੇ;
	mosquitto_plugin_id_t *identifier;
};

ਬਣਤਰ plugin__callbacks {
	ਬਣਤਰ mosquitto__callback *tick;
	ਬਣਤਰ mosquitto__callback *acl_check;
	ਬਣਤਰ mosquitto__callback *basic_auth;
	ਬਣਤਰ mosquitto__callback *connect;
	ਬਣਤਰ mosquitto__callback *control;
	ਬਣਤਰ mosquitto__callback *disconnect;
	ਬਣਤਰ mosquitto__callback *client_offline;
	ਬਣਤਰ mosquitto__callback *ext_auth_continue;
	ਬਣਤਰ mosquitto__callback *ext_auth_ਸ਼ੁਰੂ;
	ਬਣਤਰ mosquitto__callback *ਸੁਨੇਹਾ_ਵਿੱਚ;
	ਬਣਤਰ mosquitto__callback *ਸੁਨੇਹਾ_ਬਾਹਰਲਾ;
	ਬਣਤਰ mosquitto__callback *psk_ਕੁੰਜੀ;
	ਬਣਤਰ mosquitto__callback *reload;
	ਬਣਤਰ mosquitto__callback *subscribe;
	ਬਣਤਰ mosquitto__callback *unsubscribe;
	ਬਣਤਰ mosquitto__callback *persist_restore;
	ਬਣਤਰ mosquitto__callback *persist_client_ਜੋੜੋ;
	ਬਣਤਰ mosquitto__callback *persist_client_delete;
	ਬਣਤਰ mosquitto__callback *persist_client_update;
	ਬਣਤਰ mosquitto__callback *persist_subscription_ਜੋੜੋ;
	ਬਣਤਰ mosquitto__callback *persist_subscription_delete;
	ਬਣਤਰ mosquitto__callback *persist_client_ਸੁਨੇਹਾ_ਜੋੜੋ;
	ਬਣਤਰ mosquitto__callback *persist_client_ਸੁਨੇਹਾ_delete;
	ਬਣਤਰ mosquitto__callback *persist_client_ਸੁਨੇਹਾ_update;
	ਬਣਤਰ mosquitto__callback *persist_ਅਧਾਰ_ਸੁਨੇਹਾ_ਜੋੜੋ;
	ਬਣਤਰ mosquitto__callback *persist_ਅਧਾਰ_ਸੁਨੇਹਾ_delete;
	ਬਣਤਰ mosquitto__callback *persist_retain_ਸੁਨੇਹਾ_ਸੈੱਟ;
	ਬਣਤਰ mosquitto__callback *persist_retain_ਸੁਨੇਹਾ_delete;
	ਬਣਤਰ mosquitto__callback *persist_will_ਜੋੜੋ;
	ਬਣਤਰ mosquitto__callback *persist_will_delete;
};

/* This is owned by mosquitto__config or mosquitto__listener, and only referred
 * to by other structs */
ਬਣਤਰ mosquitto__security_options {
	/* Any options that get added here also need considering
	 * in config__read() with regards whether allow_anonymous
	 * should be disabled when these options are set.
	 */
	ਬਣਤਰ mosquitto__unpwd *unpwd;
	ਬਣਤਰ mosquitto__psk *psk_id;
	ਬਣਤਰ acl_file_ਅੰਕੜੇ acl_ਅੰਕੜੇ;
	ਬਣਤਰ password_file_ਅੰਕੜੇ password_ਅੰਕੜੇ;
	ਅੱਖਰ *psk_file;
	mosquitto_plugin_id_t **plugins;
	ਪੂਰਨ plugin_ਗਿਣਤੀ;
	int8_t allow_anonymous;
	bool allow_zero_ਲੰਮਾਈ_clientid;
	ਅੱਖਰ *auto_id_ਅਗੇਤਰ;
	uint16_t auto_id_ਅਗੇਤਰ_len;
	ਬਣਤਰ plugin__callbacks plugin_callbacks;
	mosquitto_plugin_id_t *pid; /* For registering as a "plugin" */
};

#ਜੇ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
ਗਿਣਤੀ struct_ident {
	id_invalid = 0,
	id_listener = 1,
	id_client = 2,
	id_listener_ws = 3,
};
#ਜੇ_ਅੰਤ

ਬਣਤਰ mosquitto__listener {
	uint16_t ਪੋਰਟ;
	ਅੱਖਰ *host;
	ਅੱਖਰ *bind_interface;
	ਪੂਰਨ max_connections;
	ਅੱਖਰ *mount_ਬਿੰਦੂ;
	mosq_sock_t *socks;
	ਪੂਰਨ sock_ਗਿਣਤੀ;
	ਪੂਰਨ client_ਗਿਣਤੀ;
	ਗਿਣਤੀ mosquitto_protocol protocol;
	ਪੂਰਨ socket_domain;
	bool use_username_as_clientid;
	uint8_t max_qos;
	uint16_t max_topic_alias;
	uint16_t max_topic_alias_broker;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
	ਅੱਖਰ *cafile;
	ਅੱਖਰ *capath;
	ਅੱਖਰ *certfile;
	ਅੱਖਰ *keyfile;
	ਅੱਖਰ *tls_engine;
	ਅੱਖਰ *tls_engine_kpass_sha1;
	ਅੱਖਰ *ciphers;
	ਅੱਖਰ *ciphers_tls13;
	ਅੱਖਰ *psk_hint;
	SSL_CTX *ssl_ctx;
	ਅੱਖਰ *crlfile;
	ਅੱਖਰ *tls_ਸੰਸਕਰਣ;
	bool use_ਪਛਾਣ_ਤੱਤ_as_username;
	bool use_subject_as_username;
	bool require_certificate;
	bool disable_client_cert_date_checks;
	ਗਿਣਤੀ mosquitto__keyform tls_keyform;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_WEBSOCKETS
#  ਜੇ WITH_WEBSOCKETS == WS_IS_LWS
	ਬਣਤਰ lws_context *ws_context;
	bool ws_ਵਿੱਚ_init;
	ਬਣਤਰ lws_protocols *ws_protocol;
#  endif
	ਅੱਖਰ **ws_origins;
	ਪੂਰਨ ws_origin_ਗਿਣਤੀ;
#ਜੇ_ਅੰਤ
#ਜੇ defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
	ਅੱਖਰ *http_dir;
#ਜੇ_ਅੰਤ
	ਬਣਤਰ mosquitto__security_options *security_options;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_UNIX_SOCKETS
	ਅੱਖਰ *unix_socket_ਰਸਤਾ;
#ਜੇ_ਅੰਤ
	bool disable_protocol_v3;
	bool disable_protocol_v4;
	bool disable_protocol_v5;
	ਪੂਰਨ enable_proxy_protocol;
	bool proxy_protocol_v2_require_tls;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_HTTP_API
	ਬਣਤਰ MHD_Daemon *mhd;
#ਜੇ_ਅੰਤ
};


ਬਣਤਰ mosquitto__listener_sock {
#ਜੇ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	/* This *must* be the first element in the struct. */
	ਪੂਰਨ ident;
#ਜੇ_ਅੰਤ
	mosq_sock_t sock;
	ਬਣਤਰ mosquitto__listener *listener;
};


/* Callbacks belonging to a specific plugin
 * Doesn't include MOSQ_EVT_CONTROL events.
 */
ਬਣਤਰ plugin_own_callback {
	ਬਣਤਰ plugin_own_callback *ਅਗਲਾ, *prev;
	MOSQ_FUNC_generic_callback cb_func;
	ਗਿਣਤੀ mosquitto_plugin_event event;
};

ਬਣਤਰ mosquitto_plugin_id_t {
	ਬਣਤਰ mosquitto__plugin_config config;
	ਬਣਤਰ mosquitto__plugin_lib lib;
	ਬਣਤਰ mosquitto__listener *listener;
	ਅੱਖਰ *plugin_name;
	ਅੱਖਰ *plugin_ਸੰਸਕਰਣ;
	ਬਣਤਰ control_endpoint *control_endpoints;
	ਬਣਤਰ plugin_own_callback *own_callbacks;
	ਬਣਤਰ timespec ਅਗਲਾ_tick;
};

ਬਣਤਰ mosquitto__config {
	bool allow_duplicate_messages;
	ਪੂਰਨ autosave_interval;
	bool autosave_on_changes;
	bool check_retain_ਸਰੋਤ;
	ਅੱਖਰ *clientid_prefixes;
	bool connection_messages;
	uint16_t cmd_ਪੋਰਟ[CMD_ਪੋਰਟ_ਹੱਦਬੰਦੀ];
	ਪੂਰਨ cmd_ਪੋਰਟ_ਗਿਣਤੀ;
	bool daemon;
	bool test_configuration;
	bool enable_control_api;
	ਪੂਰਨ global_max_clients;
	ਪੂਰਨ global_max_connections;
	ਬਣਤਰ mosquitto__listener *default_listener; /* Points to one of `listeners` */
	ਬਣਤਰ mosquitto__listener *listeners;
	ਪੂਰਨ listener_ਗਿਣਤੀ;
	bool local_only;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ log_dest;
	ਪੂਰਨ log_facility;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ log_type;
	bool log_timestamp;
	ਅੱਖਰ *log_timestamp_format;
	ਅੱਖਰ *log_file;
	FILE *log_fptr;
	size_t max_inflight_ਬਾਈਟਾਂ;
	size_t max_queued_ਬਾਈਟਾਂ;
	ਪੂਰਨ max_queued_messages;
	uint32_t max_packet_ਮਾਪ;
	uint32_t ਸੁਨੇਹਾ_ਮਾਪ_ਹੱਦਬੰਦੀ;
	uint16_t max_inflight_messages;
	uint16_t max_keepalive;
	uint8_t max_qos;
	uint32_t packet_max_connect;
	uint32_t packet_max_simple;
	uint32_t packet_max_sub;
	uint32_t packet_max_auth;
	bool persistence;
	ਅੱਖਰ *persistence_location;
	ਅੱਖਰ *persistence_file;
	ਅੱਖਰ *persistence_filepath;
	time_t persistent_client_expiration;
	ਅੱਖਰ *pid_file;
	bool ਕਤਾਰ_qos0_messages;
	bool ਪ੍ਰਤੀ_listener_settings;
	bool retain_ਉਪਲਬਧ;
	ਪੂਰਨ retain_expiry_interval;
	bool ਸੈੱਟ_tcp_nodelay;
	ਪੂਰਨ sys_interval;
	bool upgrade_outgoing_qos;
	ਅੱਖਰ *user;
#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ਪੂਰਨ websockets_log_level;
#ਜੇ_ਅੰਤ
	uint16_t packet_ਬਫਰ_ਮਾਪ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
	ਬਣਤਰ mosquitto__bridge **bridges;
	ਪੂਰਨ bridge_ਗਿਣਤੀ;
#ਜੇ_ਅੰਤ
	ਬਣਤਰ mosquitto__security_options security_options;
};


ਬਣਤਰ mosquitto__subshared {
	UT_hash_handle hh;
	ਬਣਤਰ mosquitto__subleaf *subs;
	ਅੱਖਰ name[];
};

ਬਣਤਰ mosquitto__subhier {
	UT_hash_handle hh;
	ਬਣਤਰ mosquitto__subhier *ਮਾਪਾ;
	ਬਣਤਰ mosquitto__subhier *children;
	ਬਣਤਰ mosquitto__subleaf *subs;
	ਬਣਤਰ mosquitto__subshared *shared;
	uint16_t topic_len;
	ਅੱਖਰ topic[];
};

ਬਣਤਰ mosquitto__subleaf {
	ਬਣਤਰ mosquitto__subleaf *prev;
	ਬਣਤਰ mosquitto__subleaf *ਅਗਲਾ;
	ਬਣਤਰ mosquitto *context;
	ਬਣਤਰ mosquitto__subhier *hier;
	ਬਣਤਰ mosquitto__subshared *shared;
	uint32_t identifier;
	uint8_t subscription_options;
	ਅੱਖਰ topic_filter[];
};

ਬਣਤਰ sub__token {
	ਬਣਤਰ sub__token *ਅਗਲਾ;
	uint16_t topic_len;
	ਅੱਖਰ topic[];
};

ਬਣਤਰ mosquitto__retainhier {
	UT_hash_handle hh;
	ਬਣਤਰ mosquitto__retainhier *ਮਾਪਾ;
	ਬਣਤਰ mosquitto__retainhier *children;
	ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *retained;
	uint16_t topic_len;
	ਅੱਖਰ topic[];
};

ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ {
	UT_hash_handle hh;
	ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ_2 ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto__listener *ਸਰੋਤ_listener;
	ਅੱਖਰ **dest_ids;
	ਪੂਰਨ dest_id_ਗਿਣਤੀ;
	ਪੂਰਨ ref_ਗਿਣਤੀ;
	ਗਿਣਤੀ mosquitto_ਸੁਨੇਹਾ_origin origin;
	bool stored;
};

ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ {
	ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ_2 ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *prev;
	ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *ਅਗਲਾ;
	ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ;
};


ਬਣਤਰ mosquitto__psk {
	UT_hash_handle hh;
	ਅੱਖਰ *username;
	ਅੱਖਰ *password;
};

ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_v5 {
	ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_v5 *ਅਗਲਾ, *prev;
	ਅੱਖਰ *topic;
	ਖਾਲੀ *payload;
	mosquitto_property *properties;
	/* clientid is used only by mosquitto_broker_publish*() to indicate
	   this message is for a specific client. */
	ਅੱਖਰ *clientid;
	ਪੂਰਨ payloadlen;
	ਪੂਰਨ qos;
	bool retain;
};

ਬਣਤਰ mosquitto_db {
	dbid_t ਆਖਰੀ_db_id;
	uint64_t ਗੰਢ_id_shifted;
	ਬਣਤਰ mosquitto__subhier *ਸਾਧਾਰਨ_subs;
	ਬਣਤਰ mosquitto__subhier *shared_subs;
	ਬਣਤਰ mosquitto__retainhier *retains;
	ਬਣਤਰ mosquitto *contexts_by_id;
	ਬਣਤਰ mosquitto *contexts_by_sock;
	ਬਣਤਰ mosquitto *contexts_by_id_delayed_auth;
	ਬਣਤਰ mosquitto *contexts_for_free;
	mosquitto_plugin_id_t **plugins;
	ਪੂਰਨ plugin_ਗਿਣਤੀ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
	ਬਣਤਰ mosquitto **bridges;
	ਪੂਰਨ bridge_ਗਿਣਤੀ;
#ਜੇ_ਅੰਤ
	ਬਣਤਰ clientid_ਸੂਚਕਾਂਕ_hash_2 *clientid_ਸੂਚਕਾਂਕ_hash;
	ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਸੁਨੇਹਾ_store;
	time_t now_s; /* Monotonic clock, where possible */
	time_t now_real_s; /* Read clock, for measuring session/message expiry */
	uint64_t ਗੰਢ_id; /* for unique db ids */
	time_t ਅਗਲਾ_event_ms; /* for mux timeout */
	ਪੂਰਨ ਸੁਨੇਹਾ_store_ਗਿਣਤੀ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਸੁਨੇਹਾ_store_ਬਾਈਟਾਂ;
	ਅੱਖਰ *config_file;
	ਬਣਤਰ mosquitto__config *config;
	bool quiet;
	bool verbose;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_SYS_ਰੁੱਖ
	ਪੂਰਨ subscription_ਗਿਣਤੀ;
	ਪੂਰਨ shared_subscription_ਗਿਣਤੀ;
	ਪੂਰਨ retained_ਗਿਣਤੀ;
#ਜੇ_ਅੰਤ
	ਪੂਰਨ persistence_changes;
	ਬਣਤਰ mosquitto *ll_for_free;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_EPOLL
	ਪੂਰਨ epollfd;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_KQUEUE
	ਪੂਰਨ kqueuefd;
#ਜੇ_ਅੰਤ
	ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_v5 *plugin_msgs;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
	/* tls_keylog can't be in the config struct because it is used
	   before the config is allocated. Config probably
	   shouldn't be separately allocated. */
	ਅੱਖਰ *tls_keylog;
#ਜੇ_ਅੰਤ
	bool shutdown;
};

ਗਿਣਤੀ mosquitto__bridge_direction {
	bd_ਬਾਹਰਲਾ = 0,
	bd_ਵਿੱਚ = 1,
	bd_both = 2,
};

ਗਿਣਤੀ mosquitto_bridge_ਸ਼ੁਰੂ_type {
	bst_automatic = 0,
	bst_lazy = 1,
	bst_manual = 2,
	bst_once = 3,
};

ਗਿਣਤੀ mosquitto_bridge_reload_type {
	brt_lazy = 0,
	brt_ਤੁਰੰਤ = 1,
};

ਬਣਤਰ mosquitto__bridge_topic {
	ਬਣਤਰ mosquitto__bridge_topic *ਅਗਲਾ;
	ਅੱਖਰ *topic;
	ਅੱਖਰ *local_ਅਗੇਤਰ;
	ਅੱਖਰ *remote_ਅਗੇਤਰ;
	ਅੱਖਰ *local_topic; /* topic prefixed with local_prefix */
	ਅੱਖਰ *remote_topic; /* topic prefixed with remote_prefix */
	ਗਿਣਤੀ mosquitto__bridge_direction direction;
	uint8_t qos;
};

ਬਣਤਰ bridge_ਪਤਾ {
	ਅੱਖਰ *ਪਤਾ;
	uint16_t ਪੋਰਟ;
};

ਬਣਤਰ mosquitto__bridge {
	ਅੱਖਰ *name;
	ਬਣਤਰ bridge_ਪਤਾ *ਪਤੇ;
	ਪੂਰਨ cur_ਪਤਾ;
	ਪੂਰਨ ਪਤਾ_ਗਿਣਤੀ;
	time_t primary_retry;
	mosq_sock_t primary_retry_sock;
	bool ਗੇੜਾ_robin;
	bool try_ਨਿੱਜੀ;
	bool try_ਨਿੱਜੀ_accepted;
	bool clean_ਸ਼ੁਰੂ;
	int8_t clean_ਸ਼ੁਰੂ_local;
	uint16_t keepalive;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ tcp_keepalive_idle;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ tcp_keepalive_interval;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ tcp_keepalive_counter;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TCP_USER_TIMEOUT
	ਪੂਰਨ tcp_user_timeout;
#ਜੇ_ਅੰਤ
	ਬਣਤਰ mosquitto__bridge_topic *topics;
	ਪੂਰਨ topic_ਗਿਣਤੀ;
	bool topic_remapping;
	ਗਿਣਤੀ mosquitto__protocol protocol_ਸੰਸਕਰਣ;
	time_t restart_t;
	time_t connected_at;
	ਅੱਖਰ *remote_clientid;
	ਅੱਖਰ *remote_username;
	ਅੱਖਰ *remote_password;
	ਅੱਖਰ *local_clientid;
	ਅੱਖਰ *local_username;
	ਅੱਖਰ *local_password;
	ਅੱਖਰ *notification_topic;
	ਅੱਖਰ *bind_ਪਤਾ;
	bool notifications;
	bool notifications_local_only;
	ਗਿਣਤੀ mosquitto_bridge_ਸ਼ੁਰੂ_type ਸ਼ੁਰੂ_type;
	ਪੂਰਨ idle_timeout;
	ਪੂਰਨ restart_timeout;
	ਪੂਰਨ backoff_ਅਧਾਰ;
	ਪੂਰਨ backoff_cap;
	ਪੂਰਨ stable_connection_period;
	ਪੂਰਨ ਹੱਦ;
	uint32_t ਵੱਧ_ਤੋਂ_ਵੱਧ_packet_ਮਾਪ;
	uint32_t session_expiry_interval;
	uint16_t receive_ਵੱਧ_ਤੋਂ_ਵੱਧ;
	bool lazy_reconnect;
	bool attempt_unsubscribe;
	bool initial_notification_ਪੂਰਾ;
	bool outgoing_retain;
	ਗਿਣਤੀ mosquitto_bridge_reload_type reload_type;
	uint16_t max_topic_alias;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
	bool tls_insecure;
	bool tls_ocsp_required;
	bool tls_use_os_certs;
	ਅੱਖਰ *tls_cafile;
	ਅੱਖਰ *tls_capath;
	ਅੱਖਰ *tls_certfile;
	ਅੱਖਰ *tls_keyfile;
	ਅੱਖਰ *tls_ਸੰਸਕਰਣ;
	ਅੱਖਰ *tls_alpn;
	ਅੱਖਰ *tls_ciphers;
	ਅੱਖਰ *tls_13_ciphers;
#  ifdef FINAL_WITH_TLS_PSK
	ਅੱਖਰ *tls_psk_ਪਛਾਣ_ਤੱਤ;
	ਅੱਖਰ *tls_psk;
#  endif
#ਜੇ_ਅੰਤ
};

#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
ਬਣਤਰ libws_mqtt_hack {
	ਅੱਖਰ *http_dir;
	ਬਣਤਰ mosquitto__listener *listener;
};

ਬਣਤਰ libws_mqtt_ਅੰਕੜੇ {
	ਬਣਤਰ mosquitto *mosq;
};
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <net_mosq.h>


ਬਣਤਰ control_endpoint {
	ਬਣਤਰ control_endpoint *ਅਗਲਾ, *prev;
	ਅੱਖਰ topic[];
};

ਬਾਹਰੀ ਬਣਤਰ mosquitto_db db;

/* ============================================================
 * Main functions
 * ============================================================ */
ਪੂਰਨ mosquitto_main_loop(ਬਣਤਰ mosquitto__listener_sock *listensock, ਪੂਰਨ listensock_ਗਿਣਤੀ);
ਖਾਲੀ loop_update_ਅਗਲਾ_event(time_t new_ms);

/* ============================================================
 * Config functions
 * ============================================================ */
/* Initialise config struct to default values. */
ਖਾਲੀ config__init(ਬਣਤਰ mosquitto__config *config);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
ਖਾਲੀ config__bridge_cleanup(ਬਣਤਰ mosquitto__bridge *bridge);
#ਜੇ_ਅੰਤ
/* Parse command line options into config. */
ਪੂਰਨ config_parse_ਦਲੀਲਾਂ(ਬਣਤਰ mosquitto__config *config, ਪੂਰਨ argc, ਅੱਖਰ *argv[]);
/* Read configuration data from config->config_file into config.
 * If reload is true, don't process config options that shouldn't be reloaded (listeners etc)
 * Returns 0 on success, 1 if there is a configuration error or if a file cannot be opened.
 */
ਪੂਰਨ config__read(ਬਣਤਰ mosquitto__config *config, bool reload);
/* Free all config data. */
ਖਾਲੀ config__cleanup(ਬਣਤਰ mosquitto__config *config);
ਪੂਰਨ config__get_dir_files(ਅਚਲ ਅੱਖਰ *include_dir, ਅੱਖਰ ***files, ਪੂਰਨ *file_ਗਿਣਤੀ);

/* ============================================================
 * Server send functions
 * ============================================================ */
ਪੂਰਨ send__connack(ਬਣਤਰ mosquitto *context, uint8_t ack, uint8_t reason_ਕੋਡ, ਅਚਲ mosquitto_property *properties);
ਪੂਰਨ send__suback(ਬਣਤਰ mosquitto *context, uint16_t mid, uint32_t payloadlen, ਅਚਲ ਖਾਲੀ *payload);
ਪੂਰਨ send__unsuback(ਬਣਤਰ mosquitto *context, uint16_t mid, ਪੂਰਨ reason_ਕੋਡ_ਗਿਣਤੀ, uint8_t *reason_codes, ਅਚਲ mosquitto_property *properties);
ਪੂਰਨ send__auth(ਬਣਤਰ mosquitto *context, uint8_t reason_ਕੋਡ, ਅਚਲ ਖਾਲੀ *auth_ਅੰਕੜੇ, uint16_t auth_ਅੰਕੜੇ_len);

/* ============================================================
 * Network functions
 * ============================================================ */
ਖਾਲੀ net__broker_init(ਖਾਲੀ);
ਖਾਲੀ net__broker_cleanup(ਖਾਲੀ);
ਬਣਤਰ mosquitto *net__socket_accept(ਬਣਤਰ mosquitto__listener_sock *listensock);
ਪੂਰਨ net__socket_listen(ਬਣਤਰ mosquitto__listener *listener);
ਪੂਰਨ net_socket_get_ਪਤਾ(mosq_sock_t sock, ਅੱਖਰ *buf, size_t len, uint16_t *remote_ਪਤਾ);
ਪੂਰਨ net_tls_ਲੱਦੋ_verify(ਬਣਤਰ mosquitto__listener *listener);
ਪੂਰਨ net__tls_server_ctx(ਬਣਤਰ mosquitto__listener *listener);
ਪੂਰਨ net_ਲੱਦੋ_certificates(ਬਣਤਰ mosquitto__listener *listener);

/* ============================================================
 * Read handling functions
 * ============================================================ */
ਪੂਰਨ handle__packet(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ handle__connack(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ handle__connect(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ handle__disconnect(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ handle__publish(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ handle__subscribe(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ handle__unsubscribe(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ handle__auth(ਬਣਤਰ mosquitto *context);

/* ============================================================
 * Database handling
 * ============================================================ */
ਪੂਰਨ db__open(ਬਣਤਰ mosquitto__config *config);
ਪੂਰਨ db__close(ਖਾਲੀ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_PERSISTENCE
ਪੂਰਨ persist__backup(bool shutdown);
ਪੂਰਨ persist__restore(ਖਾਲੀ);
#ਜੇ_ਅੰਤ
/* Return the number of in-flight messages in count. */
ਪੂਰਨ db_ਸੁਨੇਹਾ_ਗਿਣਤੀ(ਪੂਰਨ *ਗਿਣਤੀ_2);
ਪੂਰਨ db_ਸੁਨੇਹਾ_delete_outgoing(ਬਣਤਰ mosquitto *context, uint16_t mid, ਗਿਣਤੀ mosquitto_ਸੁਨੇਹਾ_state expect_state, ਪੂਰਨ qos);
ਪੂਰਨ db_ਸੁਨੇਹਾ_ਪਾਓ_outgoing(ਬਣਤਰ mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ, uint32_t subscription_identifier, bool update, bool persist);
ਪੂਰਨ db_ਸੁਨੇਹਾ_ਪਾਓ_incoming(ਬਣਤਰ mosquitto *context, uint64_t cmsg_id, ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ, bool persist);
ਪੂਰਨ db_ਸੁਨੇਹਾ_remove_incoming(ਬਣਤਰ mosquitto *context, uint16_t mid);
ਪੂਰਨ db_ਸੁਨੇਹਾ_release_incoming(ਬਣਤਰ mosquitto *context, uint16_t mid);
ਪੂਰਨ db_ਸੁਨੇਹਾ_update_outgoing(ਬਣਤਰ mosquitto *context, uint16_t mid, ਗਿਣਤੀ mosquitto_ਸੁਨੇਹਾ_state state, ਪੂਰਨ qos, bool persist);
ਖਾਲੀ db_ਸੁਨੇਹਾ_ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ_ਪਹਿਲਾ(ਬਣਤਰ mosquitto *context, ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_ਅੰਕੜੇ *ਸੁਨੇਹਾ_ਅੰਕੜੇ);
ਪੂਰਨ db__messages_delete(ਬਣਤਰ mosquitto *context, bool force_free);
ਪੂਰਨ db__messages_delete_incoming(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ db__messages_delete_outgoing(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ db_messages_easy_ਕਤਾਰ(ਬਣਤਰ mosquitto *context, ਅਚਲ ਅੱਖਰ *topic, uint8_t qos, uint32_t payloadlen, ਅਚਲ ਖਾਲੀ *payload, ਪੂਰਨ retain, uint32_t ਸੁਨੇਹਾ_expiry_interval, mosquitto_property **properties);
ਪੂਰਨ db_ਸੁਨੇਹਾ_store(ਅਚਲ ਬਣਤਰ mosquitto *ਸਰੋਤ, ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ, uint32_t *ਸੁਨੇਹਾ_expiry_interval, ਗਿਣਤੀ mosquitto_ਸੁਨੇਹਾ_origin origin);
ਪੂਰਨ db_ਸੁਨੇਹਾ_store_ਲੱਭੋ(ਬਣਤਰ mosquitto *context, uint16_t mid, ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ **client_ਸੁਨੇਹਾ);
ਪੂਰਨ db_ਸੁਨੇਹਾ_store_ਜੋੜੋ(ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ);
ਖਾਲੀ db_ਸੁਨੇਹਾ_store_remove(ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ, bool notify);
ਖਾਲੀ db_ਸੁਨੇਹਾ_store_ref_inc(ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ);
ਖਾਲੀ db_ਸੁਨੇਹਾ_store_ref_dec(ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ **ਅਧਾਰ_ਸੁਨੇਹਾ);
ਖਾਲੀ db_ਸੁਨੇਹਾ_store_clean(ਖਾਲੀ);
ਖਾਲੀ db_ਸੁਨੇਹਾ_store_compact(ਖਾਲੀ);
ਖਾਲੀ db_ਸੁਨੇਹਾ_store_free(ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ);
ਪੂਰਨ db_ਸੁਨੇਹਾ_reconnect_reset(ਬਣਤਰ mosquitto *context);
bool db__ready_for_flight(ਬਣਤਰ mosquitto *context, ਗਿਣਤੀ mosquitto_ਸੁਨੇਹਾ_direction dir, ਪੂਰਨ qos);
bool db_ready_for_ਕਤਾਰ(ਬਣਤਰ mosquitto *context, ਪੂਰਨ qos, ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_ਅੰਕੜੇ *ਸੁਨੇਹਾ_ਅੰਕੜੇ);
ਖਾਲੀ sys_ਰੁੱਖ_init(ਖਾਲੀ);
ਖਾਲੀ sys_ਰੁੱਖ_update(bool force);
ਪੂਰਨ db_ਸੁਨੇਹਾ_write_inflight_ਬਾਹਰਲਾ_all(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ db_ਸੁਨੇਹਾ_write_inflight_ਬਾਹਰਲਾ_latest(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ db_ਸੁਨੇਹਾ_write_queued_ਬਾਹਰਲਾ(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ db_ਸੁਨੇਹਾ_write_queued_ਵਿੱਚ(ਬਣਤਰ mosquitto *context);
ਖਾਲੀ db_ਸੁਨੇਹਾ_ਜੋੜੋ_ਮੰਜ਼ਿਲ_inflight_stats(ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_ਅੰਕੜੇ *ਸੁਨੇਹਾ_ਅੰਕੜੇ, ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *ਸੁਨੇਹਾ);
ਖਾਲੀ db_ਸੁਨੇਹਾ_ਜੋੜੋ_ਮੰਜ਼ਿਲ_queued_stats(ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_ਅੰਕੜੇ *ਸੁਨੇਹਾ_ਅੰਕੜੇ, ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *ਸੁਨੇਹਾ);
uint64_t db_new_ਸੁਨੇਹਾ_id(ਖਾਲੀ);
ਖਾਲੀ db__expire_all_messages(ਬਣਤਰ mosquitto *context);
ਖਾਲੀ db_check_acl_ਦਾ_all_messages(ਬਣਤਰ mosquitto *context);

/* ============================================================
 * Subscription functions
 * ============================================================ */
ਪੂਰਨ sub__init(ਖਾਲੀ);
ਪੂਰਨ sub_ਜੋੜੋ(ਬਣਤਰ mosquitto *context, ਅਚਲ ਬਣਤਰ mosquitto_subscription *sub);
ਪੂਰਨ sub__remove(ਬਣਤਰ mosquitto *context, ਅਚਲ ਅੱਖਰ *sub, uint8_t *reason);
ਖਾਲੀ sub_ਰੁੱਖ_print(ਬਣਤਰ mosquitto__subhier *ਜੜ੍ਹ, ਪੂਰਨ level);
ਪੂਰਨ sub__clean_session(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ sub_messages_ਕਤਾਰ(ਅਚਲ ਅੱਖਰ *ਸਰੋਤ_id, uint8_t qos, ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ **ਅਧਾਰ_ਸੁਨੇਹਾ);
ਪੂਰਨ sub__topic_tokenise(ਅਚਲ ਅੱਖਰ *subtopic, ਅੱਖਰ **local_sub, ਅੱਖਰ ***topics, ਅਚਲ ਅੱਖਰ **sharename);
ਖਾਲੀ sub_topic_ਟੋਕਨ_free(ਬਣਤਰ sub__token *ਟੋਕਨ);

/* ============================================================
 * Context functions
 * ============================================================ */
ਬਣਤਰ mosquitto *context__init(ਖਾਲੀ);
ਪੂਰਨ context__init_sock(ਬਣਤਰ mosquitto *context, mosq_sock_t sock, bool get_ਪਤਾ);
ਖਾਲੀ context__cleanup(ਬਣਤਰ mosquitto *context, bool force_free);
ਖਾਲੀ context__disconnect(ਬਣਤਰ mosquitto *context, ਪੂਰਨ reason);
ਖਾਲੀ context_ਜੋੜੋ_ਮੰਜ਼ਿਲ_disused(ਬਣਤਰ mosquitto *context);
ਖਾਲੀ context__free_disused(ਖਾਲੀ);
ਖਾਲੀ context__send_will(ਬਣਤਰ mosquitto *context);
ਖਾਲੀ context_ਜੋੜੋ_ਮੰਜ਼ਿਲ_by_id(ਬਣਤਰ mosquitto *context);
ਖਾਲੀ context__remove_from_by_id(ਬਣਤਰ mosquitto *context);

ਪੂਰਨ connect__on_authorised(ਬਣਤਰ mosquitto *context, ਖਾਲੀ *auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ, uint16_t auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ_len);


/* ============================================================
 * Control functions
 * ============================================================ */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_CONTROL
ਪੂਰਨ control__process(ਬਣਤਰ mosquitto *context, ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ);
ਖਾਲੀ control__cleanup(ਖਾਲੀ);
#ਜੇ_ਅੰਤ
ਪੂਰਨ control_ਰਜਿਸਟਰ_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ਅਚਲ ਅੱਖਰ *topic, ਖਾਲੀ *userdata);
ਪੂਰਨ control__unregister_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ਅਚਲ ਅੱਖਰ *topic);
ਖਾਲੀ control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier);


/* ============================================================
 * Logging functions
 * ============================================================ */
ਪੂਰਨ log__init(ਬਣਤਰ mosquitto__config *config);
ਪੂਰਨ log__close(ਬਣਤਰ mosquitto__config *config);
ਖਾਲੀ log_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *fmt, ...) __attribute__((format(ਲਿਖੋ, 1, 2)));

/* ============================================================
 * Bridge functions
 * ============================================================ */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
ਖਾਲੀ bridge_ਸ਼ੁਰੂ_all(ਖਾਲੀ);
ਖਾਲੀ bridge__reload(ਖਾਲੀ);
ਖਾਲੀ bridge__db_cleanup(ਖਾਲੀ);
ਖਾਲੀ bridge__cleanup(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ bridge__connect(ਬਣਤਰ mosquitto *context);
#ਜੇ defined(__GLIBC__) && defined(WITH_ADNS)
ਪੂਰਨ bridge__connect_step3(ਬਣਤਰ mosquitto *context);
#ਜੇ_ਅੰਤ
ਪੂਰਨ bridge__on_connect(ਬਣਤਰ mosquitto *context);
ਖਾਲੀ bridge_check(ਖਾਲੀ);
ਪੂਰਨ bridge_ਰਜਿਸਟਰ_local_connections(ਖਾਲੀ);
ਪੂਰਨ bridge_ਜੋੜੋ_topic(ਬਣਤਰ mosquitto__bridge *bridge, ਅਚਲ ਅੱਖਰ *topic, ਗਿਣਤੀ mosquitto__bridge_direction direction, uint8_t qos, ਅਚਲ ਅੱਖਰ *local_ਅਗੇਤਰ, ਅਚਲ ਅੱਖਰ *remote_ਅਗੇਤਰ);
ਖਾਲੀ bridge__cleanup_topics(ਬਣਤਰ mosquitto__bridge *bridge);
ਪੂਰਨ bridge_remap_topic_ਵਿੱਚ(ਬਣਤਰ mosquitto *context, ਅੱਖਰ **topic);
#ਜੇ_ਅੰਤ

/* ============================================================
 * IO multiplex related functions
 * ============================================================ */
ਪੂਰਨ mux__init(ਖਾਲੀ);
ਪੂਰਨ mux_ਜੋੜੋ_listeners(ਬਣਤਰ mosquitto__listener_sock *listensock, ਪੂਰਨ listensock_ਗਿਣਤੀ);
ਪੂਰਨ mux__delete_listeners(ਬਣਤਰ mosquitto__listener_sock *listensock, ਪੂਰਨ listensock_ਗਿਣਤੀ);
ਪੂਰਨ mux__loop_prepare(ਖਾਲੀ);
ਪੂਰਨ mux__new(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ mux_ਜੋੜੋ_ਬਾਹਰਲਾ(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ mux_remove_ਬਾਹਰਲਾ(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ mux__delete(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ mux__wait(ਖਾਲੀ);
ਪੂਰਨ mux__handle(ਬਣਤਰ mosquitto__listener_sock *listensock, ਪੂਰਨ listensock_ਗਿਣਤੀ);
ਪੂਰਨ mux__cleanup(ਖਾਲੀ);

/* ============================================================
 * Listener related functions
 * ============================================================ */
ਬਾਹਰੀ ਬਣਤਰ mosquitto__listener_sock *g_listensock;
ਬਾਹਰੀ ਪੂਰਨ g_listensock_ਗਿਣਤੀ;

ਖਾਲੀ listener_ਸੈੱਟ_defaults(ਬਣਤਰ mosquitto__listener *listener);
ਖਾਲੀ listeners__reload_all_certificates(ਖਾਲੀ);
#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
ਖਾਲੀ listeners_ਜੋੜੋ_websockets(ਬਣਤਰ lws_context *ws_context, mosq_sock_t fd);
#ਜੇ_ਅੰਤ
ਪੂਰਨ listeners_ਸ਼ੁਰੂ(ਖਾਲੀ);
ਖਾਲੀ listeners__stop(ਖਾਲੀ);

/* ============================================================
 * Plugin related functions
 * ============================================================ */
ਪੂਰਨ plugin_ਲੱਦੋ_v5(mosquitto_plugin_id_t *plugin, ਖਾਲੀ *lib);
ਪੂਰਨ plugin_ਲੱਦੋ_v4(mosquitto_plugin_id_t *plugin, ਖਾਲੀ *lib);
ਪੂਰਨ plugin_ਲੱਦੋ_v3(mosquitto_plugin_id_t *plugin, ਖਾਲੀ *lib);
ਪੂਰਨ plugin_ਲੱਦੋ_v2(mosquitto_plugin_id_t *plugin, ਖਾਲੀ *lib);
ਪੂਰਨ acl__pre_check(mosquitto_plugin_id_t *plugin, ਬਣਤਰ mosquitto *context, ਪੂਰਨ access);

ਖਾਲੀ LIB_ERROR(ਖਾਲੀ);
ਖਾਲੀ plugin__handle_connect(ਬਣਤਰ mosquitto *context);
ਖਾਲੀ plugin__handle_disconnect(ਬਣਤਰ mosquitto *context, ਪੂਰਨ reason);
ਖਾਲੀ plugin__handle_client_offline(ਬਣਤਰ mosquitto *context, ਪੂਰਨ reason);
ਪੂਰਨ plugin_handle_ਸੁਨੇਹਾ_ਵਿੱਚ(ਬਣਤਰ mosquitto *context, ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ_2 *ਅਧਾਰ_ਸੁਨੇਹਾ);
ਪੂਰਨ plugin_handle_ਸੁਨੇਹਾ_ਬਾਹਰਲਾ(ਬਣਤਰ mosquitto *context, ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ_2 *ਅਧਾਰ_ਸੁਨੇਹਾ);
ਪੂਰਨ plugin__handle_subscribe(ਬਣਤਰ mosquitto *context, ਬਣਤਰ mosquitto_subscription *sub);
ਪੂਰਨ plugin__handle_unsubscribe(ਬਣਤਰ mosquitto *context, ਬਣਤਰ mosquitto_subscription *sub);
ਪੂਰਨ plugin__handle_reload(ਖਾਲੀ);
ਖਾਲੀ plugin__handle_tick(ਖਾਲੀ);
ਪੂਰਨ plugin__callback_unregister_all(mosquitto_plugin_id_t *identifier);
ਖਾਲੀ plugin_persist__handle_restore(ਖਾਲੀ);
ਖਾਲੀ plugin_persist_handle_client_ਜੋੜੋ(ਬਣਤਰ mosquitto *context);
ਖਾਲੀ plugin_persist__handle_client_delete(ਬਣਤਰ mosquitto *context);
ਖਾਲੀ plugin_persist__handle_client_update(ਬਣਤਰ mosquitto *context);
ਖਾਲੀ plugin_persist_handle_subscription_ਜੋੜੋ(ਬਣਤਰ mosquitto *context, ਅਚਲ ਬਣਤਰ mosquitto_subscription *sub);
ਖਾਲੀ plugin_persist__handle_subscription_delete(ਬਣਤਰ mosquitto *context, ਅੱਖਰ *sub);
ਖਾਲੀ plugin_persist_handle_client_ਸੁਨੇਹਾ_ਜੋੜੋ(ਬਣਤਰ mosquitto *context, ਅਚਲ ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *cmsg);
ਖਾਲੀ plugin_persist_handle_client_ਸੁਨੇਹਾ_delete(ਬਣਤਰ mosquitto *context, ਅਚਲ ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *cmsg);
ਖਾਲੀ plugin_persist_handle_client_ਸੁਨੇਹਾ_update(ਬਣਤਰ mosquitto *context, ਅਚਲ ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *cmsg);
ਖਾਲੀ plugin_persist_handle_ਅਧਾਰ_ਸੁਨੇਹਾ_ਜੋੜੋ(ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ);
ਖਾਲੀ plugin_persist_handle_ਅਧਾਰ_ਸੁਨੇਹਾ_delete(ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ);
ਖਾਲੀ plugin_persist_handle_retain_ਸੁਨੇਹਾ_ਸੈੱਟ(ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ);
ਖਾਲੀ plugin_persist_handle_retain_ਸੁਨੇਹਾ_delete(ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ);
ਖਾਲੀ plugin_persist_handle_will_ਜੋੜੋ(ਬਣਤਰ mosquitto *context);
ਖਾਲੀ plugin_persist__handle_will_delete(ਬਣਤਰ mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
ਪੂਰਨ keepalive__init(ਖਾਲੀ);
ਖਾਲੀ keepalive__cleanup(ਖਾਲੀ);
ਪੂਰਨ keepalive_ਜੋੜੋ(ਬਣਤਰ mosquitto *context);
ਖਾਲੀ keepalive__check(ਖਾਲੀ);
ਪੂਰਨ keepalive__remove(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ keepalive__update(ਬਣਤਰ mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
ਪੂਰਨ property__process_connect(ਬਣਤਰ mosquitto *context, mosquitto_property **props);
ਪੂਰਨ property__process_will(ਬਣਤਰ mosquitto *context, ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_all *ਸੁਨੇਹਾ, mosquitto_property **props);
ਪੂਰਨ property__process_publish(ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ, mosquitto_property **props, ਪੂਰਨ *topic_alias, uint32_t *ਸੁਨੇਹਾ_expiry_interval, bool is_bridge);
ਪੂਰਨ property__process_disconnect(ਬਣਤਰ mosquitto *context, mosquitto_property **props);

/* ============================================================
 * Retain tree related functions
 * ============================================================ */
ਪੂਰਨ retain__init(ਖਾਲੀ);
ਖਾਲੀ retain__clean(ਬਣਤਰ mosquitto__retainhier **retainhier);
ਪੂਰਨ retain_ਕਤਾਰ(ਬਣਤਰ mosquitto *context, ਅਚਲ ਬਣਤਰ mosquitto_subscription *sub);
ਪੂਰਨ retain__store(ਅਚਲ ਅੱਖਰ *topic, ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ, ਅੱਖਰ **ਵੰਡੋ_topics, bool persist);
ਖਾਲੀ retain__expiry_check(ਖਾਲੀ);
ਖਾਲੀ retain__expire(ਬਣਤਰ mosquitto__retainhier **retainhier);

/* ============================================================
 * Security related functions
 * ============================================================ */
ਪੂਰਨ acl_ਲੱਭੋ_acls(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ plugin_ਲੱਦੋ_all(ਖਾਲੀ);
ਪੂਰਨ plugin__unload_all(ਖਾਲੀ);
ਪੂਰਨ config_plugin_ਜੋੜੋ_secopt(mosquitto_plugin_id_t *plugin, ਬਣਤਰ mosquitto__security_options *security_options);

ਪੂਰਨ mosquitto_security_init(bool reload);
ਪੂਰਨ mosquitto_security_cleanup(bool reload);
ਪੂਰਨ mosquitto_acl_check(ਬਣਤਰ mosquitto *context, ਅਚਲ ਅੱਖਰ *topic, uint32_t payloadlen, ਖਾਲੀ *payload, uint8_t qos, bool retain, mosquitto_property *properties, ਪੂਰਨ access);
ਪੂਰਨ mosquitto_basic_auth(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ mosquitto_psk_ਕੁੰਜੀ_get(ਬਣਤਰ mosquitto *context, ਅਚਲ ਅੱਖਰ *hint, ਅਚਲ ਅੱਖਰ *ਪਛਾਣ_ਤੱਤ, ਅੱਖਰ *ਕੁੰਜੀ, ਪੂਰਨ max_ਕੁੰਜੀ_len);

ਪੂਰਨ mosquitto_security_init_default(ਖਾਲੀ);
ਪੂਰਨ mosquitto_security_apply_default(ਖਾਲੀ);
ਪੂਰਨ mosquitto_security_cleanup_default(ਖਾਲੀ);
ਪੂਰਨ mosquitto_psk_ਕੁੰਜੀ_get_default(ਬਣਤਰ mosquitto *context, ਅਚਲ ਅੱਖਰ *hint, ਅਚਲ ਅੱਖਰ *ਪਛਾਣ_ਤੱਤ, ਅੱਖਰ *ਕੁੰਜੀ, ਪੂਰਨ max_ਕੁੰਜੀ_len);
ਪੂਰਨ broker_acl_file__init(ਖਾਲੀ);
ਖਾਲੀ broker_acl_file__cleanup(ਖਾਲੀ);
ਪੂਰਨ broker_password_file__init(ਖਾਲੀ);
ਖਾਲੀ broker_password_file__cleanup(ਖਾਲੀ);
ਪੂਰਨ psk_file__init(ਖਾਲੀ);
ਪੂਰਨ psk_file__cleanup(ਖਾਲੀ);

ਪੂਰਨ mosquitto_security_auth_ਸ਼ੁਰੂ(ਬਣਤਰ mosquitto *context, bool reauth, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ_ਵਿੱਚ, uint16_t ਅੰਕੜੇ_ਵਿੱਚ_len, ਖਾਲੀ **ਅੰਕੜੇ_ਬਾਹਰਲਾ, uint16_t *ਅੰਕੜੇ_ਬਾਹਰਲਾ_len);
ਪੂਰਨ mosquitto_security_auth_continue(ਬਣਤਰ mosquitto *context, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ_ਵਿੱਚ, uint16_t ਅੰਕੜੇ_len, ਖਾਲੀ **ਅੰਕੜੇ_ਬਾਹਰਲਾ, uint16_t *ਅੰਕੜੇ_ਬਾਹਰਲਾ_len);

ਖਾਲੀ unpwd_free_ਵਸਤੂ(ਬਣਤਰ mosquitto__unpwd **unpwd, ਬਣਤਰ mosquitto__unpwd *ਵਸਤੂ);

/* ============================================================
 * Session expiry
 * ============================================================ */
ਪੂਰਨ session_expiry_ਜੋੜੋ(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ session_expiry_ਜੋੜੋ_from_persistence(ਬਣਤਰ mosquitto *context, time_t expiry_ਸਮਾਂ);
ਖਾਲੀ session_expiry_ਜੋੜੋ_on_shutdown(ਖਾਲੀ);
ਖਾਲੀ session_expiry__remove(ਬਣਤਰ mosquitto *context);
ਖਾਲੀ session_expiry__remove_all(ਖਾਲੀ);
ਖਾਲੀ session_expiry__check(ਖਾਲੀ);
ਖਾਲੀ session_expiry__send_all(ਖਾਲੀ);

/* ============================================================
 * Signals
 * ============================================================ */
ਖਾਲੀ signal_ਤਿਆਰੀ(ਖਾਲੀ);
ਪੂਰਨ signal__flag_check(ਖਾਲੀ);

/* ============================================================
 * Window service and signal related functions
 * ============================================================ */
#ਜੇ defined(WIN32) || defined(__CYGWIN__)
ਖਾਲੀ service_install(ਅੱਖਰ *name);
ਖਾਲੀ service_uninstall(ਅੱਖਰ *name);
ਖਾਲੀ service_ਲੜੀਵਾਰ(ਅੱਖਰ *name);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
DWORD WINAPI SigThreadProc(ਖਾਲੀ *ਅੰਕੜੇ);
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

/* ============================================================
 * Watchdog
 * ============================================================ */
ਖਾਲੀ watchdog__init(ਖਾਲੀ);
ਖਾਲੀ watchdog__check(ਖਾਲੀ);

/* ============================================================
 * Websockets related functions
 * ============================================================ */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_WEBSOCKETS
#  ਜੇ WITH_WEBSOCKETS == WS_IS_LWS
ਖਾਲੀ mosq_websockets_init(ਬਣਤਰ mosquitto__listener *listener, ਅਚਲ ਬਣਤਰ mosquitto__config *conf);
#  endif
ਪੂਰਨ http__context_init(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ http__context_cleanup(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ http__read(ਬਣਤਰ mosquitto *context);
#ਜੇ_ਅੰਤ
ਖਾਲੀ do_disconnect(ਬਣਤਰ mosquitto *context, ਪੂਰਨ reason);

/* ============================================================
 * PROXY related functions
 * ============================================================ */
ਪੂਰਨ proxy_v2__read(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ proxy_v1__read(ਬਣਤਰ mosquitto *context);

/* ============================================================
 * Will delay
 * ============================================================ */
ਪੂਰਨ will_delay_ਜੋੜੋ(ਬਣਤਰ mosquitto *context);
ਖਾਲੀ will_delay__check(ਖਾਲੀ);
ਖਾਲੀ will_delay__send_all(ਖਾਲੀ);
ਖਾਲੀ will_delay__remove(ਬਣਤਰ mosquitto *mosq);

/* ============================================================
 * HTTP Info
 * ============================================================ */
ਪੂਰਨ http_api_ਸ਼ੁਰੂ_local(ਬਣਤਰ mosquitto__listener *listener);
ਪੂਰਨ http_api_ਸ਼ੁਰੂ(ਬਣਤਰ mosquitto__listener *listener);
ਖਾਲੀ http_api__stop(ਬਣਤਰ mosquitto__listener *listener);


/* ============================================================
 * Other
 * ============================================================ */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_XTREPORT
ਖਾਲੀ xtreport(ਖਾਲੀ);
#ਜੇ_ਅੰਤ

ਖਾਲੀ broker_control__init(ਖਾਲੀ);
ਖਾਲੀ broker_control__cleanup(ਖਾਲੀ);
ਖਾਲੀ broker_control__reload(ਖਾਲੀ);

#ਜੇ_ਅੰਤ
