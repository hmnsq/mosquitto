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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MOSQUITTO_BROKER_ಆಂತರಿಕ_H
#ವ್ಯಾಖ್ಯಾನಿಸು MOSQUITTO_BROKER_ಆಂತರಿಕ_H

#ಅನ್ತರ್ಭಾವ "config.h"
#ಅನ್ತರ್ಭಾವ <stdio.h>

#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#  ಆದರೆ LWS_LIBRARY_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ >= 3002000 && !defined(LWS_WITH_EXTERNAL_POLL)
#    warning "libwebsockets is not compiled with LWS_WITH_EXTERNAL_POLL support. Websocket performance will be unusable."
#  endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_HTTP_API
#  include <microhttpd.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __linux__
#ವ್ಯಾಖ್ಯಾನಿಸು WITH_TCP_USER_TIMEOUT
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "mosquitto_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/broker.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/broker_plugin.h"
#ಅನ್ತರ್ಭಾವ "mosquitto.h"
#ಅನ್ತರ್ಭಾವ "logging_mosq.h"
#ಅನ್ತರ್ಭಾವ "tls_mosq.h"
#ಅನ್ತರ್ಭಾವ "uthash.h"
#ಅನ್ತರ್ಭಾವ "acl_file.h"
#ಅನ್ತರ್ಭಾವ "password_file.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __GNUC__
#ವ್ಯಾಖ್ಯಾನಿಸು __attribute__(attrib)
#ಆದರೆ_ಅಂತ್ಯ

/* Log destinations */
#ವ್ಯಾಖ್ಯಾನಿಸು MQTT3_LOG_NONE 0x00
#ವ್ಯಾಖ್ಯಾನಿಸು MQTT3_LOG_SYSLOG 0x01
#ವ್ಯಾಖ್ಯಾನಿಸು MQTT3_LOG_FILE 0x02
#ವ್ಯಾಖ್ಯಾನಿಸು MQTT3_LOG_STDOUT 0x04
#ವ್ಯಾಖ್ಯಾನಿಸು MQTT3_LOG_STDERR 0x08
#ವ್ಯಾಖ್ಯಾನಿಸು MQTT3_LOG_TOPIC 0x10
#ವ್ಯಾಖ್ಯಾನಿಸು MQTT3_LOG_DLT 0x20
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ANDROID
#ವ್ಯಾಖ್ಯಾನಿಸು MQTT3_LOG_ANDROID 0x40
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು MQTT3_LOG_ALL 0xFF

#ವ್ಯಾಖ್ಯಾನಿಸು CMD_ಪೋರ್ಟ್_ಎಲ್ಲೆ 10

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint64_t dbid_t;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*FUNC_plugin_init_v5)(mosquitto_plugin_id_t *, ಶೂನ್ಯ **, ರಚನೆ mosquitto_opt *, ಪೂರ್ಣಾಂಕ);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*FUNC_plugin_cleanup_v5)(ಶೂನ್ಯ *, ರಚನೆ mosquitto_opt *, ಪೂರ್ಣಾಂಕ);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*FUNC_auth_plugin_init_v4)(ಶೂನ್ಯ **, ರಚನೆ mosquitto_opt *, ಪೂರ್ಣಾಂಕ);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*FUNC_auth_plugin_cleanup_v4)(ಶೂನ್ಯ *, ರಚನೆ mosquitto_opt *, ಪೂರ್ಣಾಂಕ);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*FUNC_auth_plugin_security_init_v4)(ಶೂನ್ಯ *, ರಚನೆ mosquitto_opt *, ಪೂರ್ಣಾಂಕ, bool);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*FUNC_auth_plugin_security_cleanup_v4)(ಶೂನ್ಯ *, ರಚನೆ mosquitto_opt *, ಪೂರ್ಣಾಂಕ, bool);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*FUNC_auth_plugin_acl_check_v4)(ಶೂನ್ಯ *, ಪೂರ್ಣಾಂಕ, ರಚನೆ mosquitto *, ರಚನೆ mosquitto_acl_ಸಂದೇಶ *);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*FUNC_auth_plugin_unpwd_check_v4)(ಶೂನ್ಯ *, ರಚನೆ mosquitto *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*FUNC_auth_plugin_psk_ಕೀಲಿ_get_v4)(ಶೂನ್ಯ *, ರಚನೆ mosquitto *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಅಕ್ಷರ *, ಪೂರ್ಣಾಂಕ);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*FUNC_auth_plugin_auth_ಆರಂಭ_v4)(ಶೂನ್ಯ *, ರಚನೆ mosquitto *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, bool, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *, uint16_t, ಶೂನ್ಯ **, uint16_t *);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*FUNC_auth_plugin_auth_continue_v4)(ಶೂನ್ಯ *, ರಚನೆ mosquitto *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *, uint16_t, ಶೂನ್ಯ **, uint16_t *);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*FUNC_auth_plugin_init_v3)(ಶೂನ್ಯ **, ರಚನೆ mosquitto_opt *, ಪೂರ್ಣಾಂಕ);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*FUNC_auth_plugin_cleanup_v3)(ಶೂನ್ಯ *, ರಚನೆ mosquitto_opt *, ಪೂರ್ಣಾಂಕ);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*FUNC_auth_plugin_security_init_v3)(ಶೂನ್ಯ *, ರಚನೆ mosquitto_opt *, ಪೂರ್ಣಾಂಕ, bool);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*FUNC_auth_plugin_security_cleanup_v3)(ಶೂನ್ಯ *, ರಚನೆ mosquitto_opt *, ಪೂರ್ಣಾಂಕ, bool);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*FUNC_auth_plugin_acl_check_v3)(ಶೂನ್ಯ *, ಪೂರ್ಣಾಂಕ, ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto *, ರಚನೆ mosquitto_acl_ಸಂದೇಶ *);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*FUNC_auth_plugin_unpwd_check_v3)(ಶೂನ್ಯ *, ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*FUNC_auth_plugin_psk_ಕೀಲಿ_get_v3)(ಶೂನ್ಯ *, ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಅಕ್ಷರ *, ಪೂರ್ಣಾಂಕ);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*FUNC_auth_plugin_init_v2)(ಶೂನ್ಯ **, ರಚನೆ mosquitto_auth_opt *, ಪೂರ್ಣಾಂಕ);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*FUNC_auth_plugin_cleanup_v2)(ಶೂನ್ಯ *, ರಚನೆ mosquitto_auth_opt *, ಪೂರ್ಣಾಂಕ);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*FUNC_auth_plugin_security_init_v2)(ಶೂನ್ಯ *, ರಚನೆ mosquitto_auth_opt *, ಪೂರ್ಣಾಂಕ, bool);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*FUNC_auth_plugin_security_cleanup_v2)(ಶೂನ್ಯ *, ರಚನೆ mosquitto_auth_opt *, ಪೂರ್ಣಾಂಕ, bool);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*FUNC_auth_plugin_acl_check_v2)(ಶೂನ್ಯ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಪೂರ್ಣಾಂಕ);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*FUNC_auth_plugin_unpwd_check_v2)(ಶೂನ್ಯ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*FUNC_auth_plugin_psk_ಕೀಲಿ_get_v2)(ಶೂನ್ಯ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಅಕ್ಷರ *, ಪೂರ್ಣಾಂಕ);


ಎಣಿಕೆ mosquitto_ಸಂದೇಶ_origin {
	mosq_mo_client = 0,
	mosq_mo_broker = 1,
};

ರಚನೆ mosquitto__plugin_lib {
	ಶೂನ್ಯ *lib;
	ಶೂನ್ಯ *user_ದತ್ತಾಂಶ;
	ಪೂರ್ಣಾಂಕ (*plugin_ಆವೃತ್ತಿ)(ಶೂನ್ಯ);
	ರಚನೆ mosquitto_plugin_id_t *identifier;

	FUNC_plugin_init_v5 plugin_init_v5;
	FUNC_plugin_cleanup_v5 plugin_cleanup_v5;

	FUNC_auth_plugin_init_v4 plugin_init_v4;
	FUNC_auth_plugin_cleanup_v4 plugin_cleanup_v4;
	FUNC_auth_plugin_security_init_v4 security_init_v4;
	FUNC_auth_plugin_security_cleanup_v4 security_cleanup_v4;
	FUNC_auth_plugin_acl_check_v4 acl_check_v4;
	FUNC_auth_plugin_unpwd_check_v4 unpwd_check_v4;
	FUNC_auth_plugin_psk_ಕೀಲಿ_get_v4 psk_ಕೀಲಿ_get_v4;
	FUNC_auth_plugin_auth_ಆರಂಭ_v4 auth_ಆರಂಭ_v4;
	FUNC_auth_plugin_auth_continue_v4 auth_continue_v4;

	FUNC_auth_plugin_init_v3 plugin_init_v3;
	FUNC_auth_plugin_cleanup_v3 plugin_cleanup_v3;
	FUNC_auth_plugin_security_init_v3 security_init_v3;
	FUNC_auth_plugin_security_cleanup_v3 security_cleanup_v3;
	FUNC_auth_plugin_acl_check_v3 acl_check_v3;
	FUNC_auth_plugin_unpwd_check_v3 unpwd_check_v3;
	FUNC_auth_plugin_psk_ಕೀಲಿ_get_v3 psk_ಕೀಲಿ_get_v3;

	FUNC_auth_plugin_init_v2 plugin_init_v2;
	FUNC_auth_plugin_cleanup_v2 plugin_cleanup_v2;
	FUNC_auth_plugin_security_init_v2 security_init_v2;
	FUNC_auth_plugin_security_cleanup_v2 security_cleanup_v2;
	FUNC_auth_plugin_acl_check_v2 acl_check_v2;
	FUNC_auth_plugin_unpwd_check_v2 unpwd_check_v2;
	FUNC_auth_plugin_psk_ಕೀಲಿ_get_v2 psk_ಕೀಲಿ_get_v2;
	ಪೂರ್ಣಾಂಕ ಆವೃತ್ತಿ_2;
};

ರಚನೆ mosquitto__plugin_config {
	ಅಕ್ಷರ *ಮಾರ್ಗ;
	ಅಕ್ಷರ *name;
	ರಚನೆ mosquitto_opt *options;
	ರಚನೆ mosquitto__security_options **security_options;
	ಪೂರ್ಣಾಂಕ option_ಎಣಿಕೆ;
	ಪೂರ್ಣಾಂಕ security_option_ಎಣಿಕೆ;
	bool deny_special_chars;
};

ರಚನೆ mosquitto__callback {
	UT_hash_handle hh; /* For callbacks that register for e.g. a specific topic */
	ರಚನೆ mosquitto__callback *ಮುಂದಿನ, *prev; /* For typical callbacks */
	MOSQ_FUNC_generic_callback cb;
	ಶೂನ್ಯ *userdata;
	ಒಕ್ಕೂಟ {
		ಅಕ್ಷರ *topic;
		ರಚನೆ timespec ಮುಂದಿನ_tick;
	} ದತ್ತಾಂಶ;
	mosquitto_plugin_id_t *identifier;
};

ರಚನೆ plugin__callbacks {
	ರಚನೆ mosquitto__callback *tick;
	ರಚನೆ mosquitto__callback *acl_check;
	ರಚನೆ mosquitto__callback *basic_auth;
	ರಚನೆ mosquitto__callback *connect;
	ರಚನೆ mosquitto__callback *control;
	ರಚನೆ mosquitto__callback *disconnect;
	ರಚನೆ mosquitto__callback *client_offline;
	ರಚನೆ mosquitto__callback *ext_auth_continue;
	ರಚನೆ mosquitto__callback *ext_auth_ಆರಂಭ;
	ರಚನೆ mosquitto__callback *ಸಂದೇಶ_ಒಳ;
	ರಚನೆ mosquitto__callback *ಸಂದೇಶ_ಹೊರಹೋಗು;
	ರಚನೆ mosquitto__callback *psk_ಕೀಲಿ;
	ರಚನೆ mosquitto__callback *reload;
	ರಚನೆ mosquitto__callback *subscribe;
	ರಚನೆ mosquitto__callback *unsubscribe;
	ರಚನೆ mosquitto__callback *persist_restore;
	ರಚನೆ mosquitto__callback *persist_client_ಕೂಡಿಸು;
	ರಚನೆ mosquitto__callback *persist_client_delete;
	ರಚನೆ mosquitto__callback *persist_client_update;
	ರಚನೆ mosquitto__callback *persist_subscription_ಕೂಡಿಸು;
	ರಚನೆ mosquitto__callback *persist_subscription_delete;
	ರಚನೆ mosquitto__callback *persist_client_ಸಂದೇಶ_ಕೂಡಿಸು;
	ರಚನೆ mosquitto__callback *persist_client_ಸಂದೇಶ_delete;
	ರಚನೆ mosquitto__callback *persist_client_ಸಂದೇಶ_update;
	ರಚನೆ mosquitto__callback *persist_ತಳ_ಸಂದೇಶ_ಕೂಡಿಸು;
	ರಚನೆ mosquitto__callback *persist_ತಳ_ಸಂದೇಶ_delete;
	ರಚನೆ mosquitto__callback *persist_retain_ಸಂದೇಶ_ಗಣ;
	ರಚನೆ mosquitto__callback *persist_retain_ಸಂದೇಶ_delete;
	ರಚನೆ mosquitto__callback *persist_will_ಕೂಡಿಸು;
	ರಚನೆ mosquitto__callback *persist_will_delete;
};

/* This is owned by mosquitto__config or mosquitto__listener, and only referred
 * to by other structs */
ರಚನೆ mosquitto__security_options {
	/* Any options that get added here also need considering
	 * in config__read() with regards whether allow_anonymous
	 * should be disabled when these options are set.
	 */
	ರಚನೆ mosquitto__unpwd *unpwd;
	ರಚನೆ mosquitto__psk *psk_id;
	ರಚನೆ acl_file_ದತ್ತಾಂಶ acl_ದತ್ತಾಂಶ;
	ರಚನೆ password_file_ದತ್ತಾಂಶ password_ದತ್ತಾಂಶ;
	ಅಕ್ಷರ *psk_file;
	mosquitto_plugin_id_t **plugins;
	ಪೂರ್ಣಾಂಕ plugin_ಎಣಿಕೆ;
	int8_t allow_anonymous;
	bool allow_zero_ಉದ್ದಳತೆ_clientid;
	ಅಕ್ಷರ *auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ;
	uint16_t auto_id_ಪೂರ್ವಪ್ರತ್ಯಯ_len;
	ರಚನೆ plugin__callbacks plugin_callbacks;
	mosquitto_plugin_id_t *pid; /* For registering as a "plugin" */
};

#ಆದರೆ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
ಎಣಿಕೆ struct_ident {
	id_invalid = 0,
	id_listener = 1,
	id_client = 2,
	id_listener_ws = 3,
};
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ mosquitto__listener {
	uint16_t ಪೋರ್ಟ್;
	ಅಕ್ಷರ *host;
	ಅಕ್ಷರ *bind_interface;
	ಪೂರ್ಣಾಂಕ max_connections;
	ಅಕ್ಷರ *mount_ಬಿಂದು;
	mosq_sock_t *socks;
	ಪೂರ್ಣಾಂಕ sock_ಎಣಿಕೆ;
	ಪೂರ್ಣಾಂಕ client_ಎಣಿಕೆ;
	ಎಣಿಕೆ mosquitto_protocol protocol;
	ಪೂರ್ಣಾಂಕ socket_domain;
	bool use_username_as_clientid;
	uint8_t max_qos;
	uint16_t max_topic_alias;
	uint16_t max_topic_alias_broker;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
	ಅಕ್ಷರ *cafile;
	ಅಕ್ಷರ *capath;
	ಅಕ್ಷರ *certfile;
	ಅಕ್ಷರ *keyfile;
	ಅಕ್ಷರ *tls_engine;
	ಅಕ್ಷರ *tls_engine_kpass_sha1;
	ಅಕ್ಷರ *ciphers;
	ಅಕ್ಷರ *ciphers_tls13;
	ಅಕ್ಷರ *psk_hint;
	SSL_CTX *ssl_ctx;
	ಅಕ್ಷರ *crlfile;
	ಅಕ್ಷರ *tls_ಆವೃತ್ತಿ;
	bool use_ತಟಸ್ಥ_as_username;
	bool use_subject_as_username;
	bool require_certificate;
	bool disable_client_cert_date_checks;
	ಎಣಿಕೆ mosquitto__keyform tls_keyform;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_WEBSOCKETS
#  ಆದರೆ WITH_WEBSOCKETS == WS_IS_LWS
	ರಚನೆ lws_context *ws_context;
	bool ws_ಒಳ_init;
	ರಚನೆ lws_protocols *ws_protocol;
#  endif
	ಅಕ್ಷರ **ws_origins;
	ಪೂರ್ಣಾಂಕ ws_origin_ಎಣಿಕೆ;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(WITH_WEBSOCKETS) || defined(WITH_HTTP_API)
	ಅಕ್ಷರ *http_dir;
#ಆದರೆ_ಅಂತ್ಯ
	ರಚನೆ mosquitto__security_options *security_options;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_UNIX_SOCKETS
	ಅಕ್ಷರ *unix_socket_ಮಾರ್ಗ;
#ಆದರೆ_ಅಂತ್ಯ
	bool disable_protocol_v3;
	bool disable_protocol_v4;
	bool disable_protocol_v5;
	ಪೂರ್ಣಾಂಕ enable_proxy_protocol;
	bool proxy_protocol_v2_require_tls;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_HTTP_API
	ರಚನೆ MHD_Daemon *mhd;
#ಆದರೆ_ಅಂತ್ಯ
};


ರಚನೆ mosquitto__listener_sock {
#ಆದರೆ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	/* This *must* be the first element in the struct. */
	ಪೂರ್ಣಾಂಕ ident;
#ಆದರೆ_ಅಂತ್ಯ
	mosq_sock_t sock;
	ರಚನೆ mosquitto__listener *listener;
};


/* Callbacks belonging to a specific plugin
 * Doesn't include MOSQ_EVT_CONTROL events.
 */
ರಚನೆ plugin_own_callback {
	ರಚನೆ plugin_own_callback *ಮುಂದಿನ, *prev;
	MOSQ_FUNC_generic_callback cb_func;
	ಎಣಿಕೆ mosquitto_plugin_event event;
};

ರಚನೆ mosquitto_plugin_id_t {
	ರಚನೆ mosquitto__plugin_config config;
	ರಚನೆ mosquitto__plugin_lib lib;
	ರಚನೆ mosquitto__listener *listener;
	ಅಕ್ಷರ *plugin_name;
	ಅಕ್ಷರ *plugin_ಆವೃತ್ತಿ;
	ರಚನೆ control_endpoint *control_endpoints;
	ರಚನೆ plugin_own_callback *own_callbacks;
	ರಚನೆ timespec ಮುಂದಿನ_tick;
};

ರಚನೆ mosquitto__config {
	bool allow_duplicate_messages;
	ಪೂರ್ಣಾಂಕ autosave_interval;
	bool autosave_on_changes;
	bool check_retain_ಮೂಲ;
	ಅಕ್ಷರ *clientid_prefixes;
	bool connection_messages;
	uint16_t cmd_ಪೋರ್ಟ್[CMD_ಪೋರ್ಟ್_ಎಲ್ಲೆ];
	ಪೂರ್ಣಾಂಕ cmd_ಪೋರ್ಟ್_ಎಣಿಕೆ;
	bool daemon;
	bool test_configuration;
	bool enable_control_api;
	ಪೂರ್ಣಾಂಕ global_max_clients;
	ಪೂರ್ಣಾಂಕ global_max_connections;
	ರಚನೆ mosquitto__listener *default_listener; /* Points to one of `listeners` */
	ರಚನೆ mosquitto__listener *listeners;
	ಪೂರ್ಣಾಂಕ listener_ಎಣಿಕೆ;
	bool local_only;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ log_dest;
	ಪೂರ್ಣಾಂಕ log_facility;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ log_type;
	bool log_timestamp;
	ಅಕ್ಷರ *log_timestamp_format;
	ಅಕ್ಷರ *log_file;
	FILE *log_fptr;
	size_t max_inflight_ಬೈಟುಗಳು;
	size_t max_queued_ಬೈಟುಗಳು;
	ಪೂರ್ಣಾಂಕ max_queued_messages;
	uint32_t max_packet_ಗಾತ್ರ;
	uint32_t ಸಂದೇಶ_ಗಾತ್ರ_ಎಲ್ಲೆ;
	uint16_t max_inflight_messages;
	uint16_t max_keepalive;
	uint8_t max_qos;
	uint32_t packet_max_connect;
	uint32_t packet_max_simple;
	uint32_t packet_max_sub;
	uint32_t packet_max_auth;
	bool persistence;
	ಅಕ್ಷರ *persistence_location;
	ಅಕ್ಷರ *persistence_file;
	ಅಕ್ಷರ *persistence_filepath;
	time_t persistent_client_expiration;
	ಅಕ್ಷರ *pid_file;
	bool ಸರತಿ_qos0_messages;
	bool ಪ್ರತಿ_listener_settings;
	bool retain_ಲಭ್ಯ;
	ಪೂರ್ಣಾಂಕ retain_expiry_interval;
	bool ಗಣ_tcp_nodelay;
	ಪೂರ್ಣಾಂಕ sys_interval;
	bool upgrade_outgoing_qos;
	ಅಕ್ಷರ *user;
#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ಪೂರ್ಣಾಂಕ websockets_log_level;
#ಆದರೆ_ಅಂತ್ಯ
	uint16_t packet_ಬಫರ್_ಗಾತ್ರ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
	ರಚನೆ mosquitto__bridge **bridges;
	ಪೂರ್ಣಾಂಕ bridge_ಎಣಿಕೆ;
#ಆದರೆ_ಅಂತ್ಯ
	ರಚನೆ mosquitto__security_options security_options;
};


ರಚನೆ mosquitto__subshared {
	UT_hash_handle hh;
	ರಚನೆ mosquitto__subleaf *subs;
	ಅಕ್ಷರ name[];
};

ರಚನೆ mosquitto__subhier {
	UT_hash_handle hh;
	ರಚನೆ mosquitto__subhier *ಪೋಷಕ;
	ರಚನೆ mosquitto__subhier *children;
	ರಚನೆ mosquitto__subleaf *subs;
	ರಚನೆ mosquitto__subshared *shared;
	uint16_t topic_len;
	ಅಕ್ಷರ topic[];
};

ರಚನೆ mosquitto__subleaf {
	ರಚನೆ mosquitto__subleaf *prev;
	ರಚನೆ mosquitto__subleaf *ಮುಂದಿನ;
	ರಚನೆ mosquitto *context;
	ರಚನೆ mosquitto__subhier *hier;
	ರಚನೆ mosquitto__subshared *shared;
	uint32_t identifier;
	uint8_t subscription_options;
	ಅಕ್ಷರ topic_filter[];
};

ರಚನೆ sub__token {
	ರಚನೆ sub__token *ಮುಂದಿನ;
	uint16_t topic_len;
	ಅಕ್ಷರ topic[];
};

ರಚನೆ mosquitto__retainhier {
	UT_hash_handle hh;
	ರಚನೆ mosquitto__retainhier *ಪೋಷಕ;
	ರಚನೆ mosquitto__retainhier *children;
	ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *retained;
	uint16_t topic_len;
	ಅಕ್ಷರ topic[];
};

ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ {
	UT_hash_handle hh;
	ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ_2 ದತ್ತಾಂಶ;
	ರಚನೆ mosquitto__listener *ಮೂಲ_listener;
	ಅಕ್ಷರ **dest_ids;
	ಪೂರ್ಣಾಂಕ dest_id_ಎಣಿಕೆ;
	ಪೂರ್ಣಾಂಕ ref_ಎಣಿಕೆ;
	ಎಣಿಕೆ mosquitto_ಸಂದೇಶ_origin origin;
	bool stored;
};

ರಚನೆ mosquitto_client_ಸಂದೇಶ {
	ರಚನೆ mosquitto_client_ಸಂದೇಶ_2 ದತ್ತಾಂಶ;
	ರಚನೆ mosquitto_client_ಸಂದೇಶ *prev;
	ರಚನೆ mosquitto_client_ಸಂದೇಶ *ಮುಂದಿನ;
	ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ;
};


ರಚನೆ mosquitto__psk {
	UT_hash_handle hh;
	ಅಕ್ಷರ *username;
	ಅಕ್ಷರ *password;
};

ರಚನೆ mosquitto_ಸಂದೇಶ_v5 {
	ರಚನೆ mosquitto_ಸಂದೇಶ_v5 *ಮುಂದಿನ, *prev;
	ಅಕ್ಷರ *topic;
	ಶೂನ್ಯ *payload;
	mosquitto_property *properties;
	/* clientid is used only by mosquitto_broker_publish*() to indicate
	   this message is for a specific client. */
	ಅಕ್ಷರ *clientid;
	ಪೂರ್ಣಾಂಕ payloadlen;
	ಪೂರ್ಣಾಂಕ qos;
	bool retain;
};

ರಚನೆ mosquitto_db {
	dbid_t ಕೊನೆಯ_db_id;
	uint64_t ಗಂಟು_id_shifted;
	ರಚನೆ mosquitto__subhier *ಸಾಮಾನ್ಯ_subs;
	ರಚನೆ mosquitto__subhier *shared_subs;
	ರಚನೆ mosquitto__retainhier *retains;
	ರಚನೆ mosquitto *contexts_by_id;
	ರಚನೆ mosquitto *contexts_by_sock;
	ರಚನೆ mosquitto *contexts_by_id_delayed_auth;
	ರಚನೆ mosquitto *contexts_for_free;
	mosquitto_plugin_id_t **plugins;
	ಪೂರ್ಣಾಂಕ plugin_ಎಣಿಕೆ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
	ರಚನೆ mosquitto **bridges;
	ಪೂರ್ಣಾಂಕ bridge_ಎಣಿಕೆ;
#ಆದರೆ_ಅಂತ್ಯ
	ರಚನೆ clientid_ಸೂಚ್ಯಂಕ_hash_2 *clientid_ಸೂಚ್ಯಂಕ_hash;
	ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ಸಂದೇಶ_store;
	time_t now_s; /* Monotonic clock, where possible */
	time_t now_real_s; /* Read clock, for measuring session/message expiry */
	uint64_t ಗಂಟು_id; /* for unique db ids */
	time_t ಮುಂದಿನ_event_ms; /* for mux timeout */
	ಪೂರ್ಣಾಂಕ ಸಂದೇಶ_store_ಎಣಿಕೆ;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಸಂದೇಶ_store_ಬೈಟುಗಳು;
	ಅಕ್ಷರ *config_file;
	ರಚನೆ mosquitto__config *config;
	bool quiet;
	bool verbose;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_SYS_ವೃಕ್ಷ
	ಪೂರ್ಣಾಂಕ subscription_ಎಣಿಕೆ;
	ಪೂರ್ಣಾಂಕ shared_subscription_ಎಣಿಕೆ;
	ಪೂರ್ಣಾಂಕ retained_ಎಣಿಕೆ;
#ಆದರೆ_ಅಂತ್ಯ
	ಪೂರ್ಣಾಂಕ persistence_changes;
	ರಚನೆ mosquitto *ll_for_free;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_EPOLL
	ಪೂರ್ಣಾಂಕ epollfd;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_KQUEUE
	ಪೂರ್ಣಾಂಕ kqueuefd;
#ಆದರೆ_ಅಂತ್ಯ
	ರಚನೆ mosquitto_ಸಂದೇಶ_v5 *plugin_msgs;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
	/* tls_keylog can't be in the config struct because it is used
	   before the config is allocated. Config probably
	   shouldn't be separately allocated. */
	ಅಕ್ಷರ *tls_keylog;
#ಆದರೆ_ಅಂತ್ಯ
	bool shutdown;
};

ಎಣಿಕೆ mosquitto__bridge_direction {
	bd_ಹೊರಹೋಗು = 0,
	bd_ಒಳ = 1,
	bd_both = 2,
};

ಎಣಿಕೆ mosquitto_bridge_ಆರಂಭ_type {
	bst_automatic = 0,
	bst_lazy = 1,
	bst_manual = 2,
	bst_once = 3,
};

ಎಣಿಕೆ mosquitto_bridge_reload_type {
	brt_lazy = 0,
	brt_ತಕ್ಷಣ = 1,
};

ರಚನೆ mosquitto__bridge_topic {
	ರಚನೆ mosquitto__bridge_topic *ಮುಂದಿನ;
	ಅಕ್ಷರ *topic;
	ಅಕ್ಷರ *local_ಪೂರ್ವಪ್ರತ್ಯಯ;
	ಅಕ್ಷರ *remote_ಪೂರ್ವಪ್ರತ್ಯಯ;
	ಅಕ್ಷರ *local_topic; /* topic prefixed with local_prefix */
	ಅಕ್ಷರ *remote_topic; /* topic prefixed with remote_prefix */
	ಎಣಿಕೆ mosquitto__bridge_direction direction;
	uint8_t qos;
};

ರಚನೆ bridge_ವಿಳಾಸ {
	ಅಕ್ಷರ *ವಿಳಾಸ;
	uint16_t ಪೋರ್ಟ್;
};

ರಚನೆ mosquitto__bridge {
	ಅಕ್ಷರ *name;
	ರಚನೆ bridge_ವಿಳಾಸ *ವಿಳಾಸಗಳು;
	ಪೂರ್ಣಾಂಕ cur_ವಿಳಾಸ;
	ಪೂರ್ಣಾಂಕ ವಿಳಾಸ_ಎಣಿಕೆ;
	time_t primary_retry;
	mosq_sock_t primary_retry_sock;
	bool ಸುತ್ತು_robin;
	bool try_ಖಾಸಗಿ;
	bool try_ಖಾಸಗಿ_accepted;
	bool clean_ಆರಂಭ;
	int8_t clean_ಆರಂಭ_local;
	uint16_t keepalive;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ tcp_keepalive_idle;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ tcp_keepalive_interval;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ tcp_keepalive_counter;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TCP_USER_TIMEOUT
	ಪೂರ್ಣಾಂಕ tcp_user_timeout;
#ಆದರೆ_ಅಂತ್ಯ
	ರಚನೆ mosquitto__bridge_topic *topics;
	ಪೂರ್ಣಾಂಕ topic_ಎಣಿಕೆ;
	bool topic_remapping;
	ಎಣಿಕೆ mosquitto__protocol protocol_ಆವೃತ್ತಿ;
	time_t restart_t;
	time_t connected_at;
	ಅಕ್ಷರ *remote_clientid;
	ಅಕ್ಷರ *remote_username;
	ಅಕ್ಷರ *remote_password;
	ಅಕ್ಷರ *local_clientid;
	ಅಕ್ಷರ *local_username;
	ಅಕ್ಷರ *local_password;
	ಅಕ್ಷರ *notification_topic;
	ಅಕ್ಷರ *bind_ವಿಳಾಸ;
	bool notifications;
	bool notifications_local_only;
	ಎಣಿಕೆ mosquitto_bridge_ಆರಂಭ_type ಆರಂಭ_type;
	ಪೂರ್ಣಾಂಕ idle_timeout;
	ಪೂರ್ಣಾಂಕ restart_timeout;
	ಪೂರ್ಣಾಂಕ backoff_ತಳ;
	ಪೂರ್ಣಾಂಕ backoff_cap;
	ಪೂರ್ಣಾಂಕ stable_connection_period;
	ಪೂರ್ಣಾಂಕ ಮಿತಿ;
	uint32_t ಗರಿಷ್ಠ_packet_ಗಾತ್ರ;
	uint32_t session_expiry_interval;
	uint16_t receive_ಗರಿಷ್ಠ;
	bool lazy_reconnect;
	bool attempt_unsubscribe;
	bool initial_notification_ಮುಗಿದ;
	bool outgoing_retain;
	ಎಣಿಕೆ mosquitto_bridge_reload_type reload_type;
	uint16_t max_topic_alias;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
	bool tls_insecure;
	bool tls_ocsp_required;
	bool tls_use_os_certs;
	ಅಕ್ಷರ *tls_cafile;
	ಅಕ್ಷರ *tls_capath;
	ಅಕ್ಷರ *tls_certfile;
	ಅಕ್ಷರ *tls_keyfile;
	ಅಕ್ಷರ *tls_ಆವೃತ್ತಿ;
	ಅಕ್ಷರ *tls_alpn;
	ಅಕ್ಷರ *tls_ciphers;
	ಅಕ್ಷರ *tls_13_ciphers;
#  ifdef FINAL_WITH_TLS_PSK
	ಅಕ್ಷರ *tls_psk_ತಟಸ್ಥ;
	ಅಕ್ಷರ *tls_psk;
#  endif
#ಆದರೆ_ಅಂತ್ಯ
};

#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
ರಚನೆ libws_mqtt_hack {
	ಅಕ್ಷರ *http_dir;
	ರಚನೆ mosquitto__listener *listener;
};

ರಚನೆ libws_mqtt_ದತ್ತಾಂಶ {
	ರಚನೆ mosquitto *mosq;
};
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <net_mosq.h>


ರಚನೆ control_endpoint {
	ರಚನೆ control_endpoint *ಮುಂದಿನ, *prev;
	ಅಕ್ಷರ topic[];
};

ಬಾಹ್ಯ ರಚನೆ mosquitto_db db;

/* ============================================================
 * Main functions
 * ============================================================ */
ಪೂರ್ಣಾಂಕ mosquitto_main_loop(ರಚನೆ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಂಕ listensock_ಎಣಿಕೆ);
ಶೂನ್ಯ loop_update_ಮುಂದಿನ_event(time_t new_ms);

/* ============================================================
 * Config functions
 * ============================================================ */
/* Initialise config struct to default values. */
ಶೂನ್ಯ config__init(ರಚನೆ mosquitto__config *config);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
ಶೂನ್ಯ config__bridge_cleanup(ರಚನೆ mosquitto__bridge *bridge);
#ಆದರೆ_ಅಂತ್ಯ
/* Parse command line options into config. */
ಪೂರ್ಣಾಂಕ config_parse_ವಾದಗಳು(ರಚನೆ mosquitto__config *config, ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ *argv[]);
/* Read configuration data from config->config_file into config.
 * If reload is true, don't process config options that shouldn't be reloaded (listeners etc)
 * Returns 0 on success, 1 if there is a configuration error or if a file cannot be opened.
 */
ಪೂರ್ಣಾಂಕ config__read(ರಚನೆ mosquitto__config *config, bool reload);
/* Free all config data. */
ಶೂನ್ಯ config__cleanup(ರಚನೆ mosquitto__config *config);
ಪೂರ್ಣಾಂಕ config__get_dir_files(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *include_dir, ಅಕ್ಷರ ***files, ಪೂರ್ಣಾಂಕ *file_ಎಣಿಕೆ);

/* ============================================================
 * Server send functions
 * ============================================================ */
ಪೂರ್ಣಾಂಕ send__connack(ರಚನೆ mosquitto *context, uint8_t ack, uint8_t reason_ಸಂಕೇತ, ಸ್ಥಿರಾಂಕ mosquitto_property *properties);
ಪೂರ್ಣಾಂಕ send__suback(ರಚನೆ mosquitto *context, uint16_t mid, uint32_t payloadlen, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *payload);
ಪೂರ್ಣಾಂಕ send__unsuback(ರಚನೆ mosquitto *context, uint16_t mid, ಪೂರ್ಣಾಂಕ reason_ಸಂಕೇತ_ಎಣಿಕೆ, uint8_t *reason_codes, ಸ್ಥಿರಾಂಕ mosquitto_property *properties);
ಪೂರ್ಣಾಂಕ send__auth(ರಚನೆ mosquitto *context, uint8_t reason_ಸಂಕೇತ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *auth_ದತ್ತಾಂಶ, uint16_t auth_ದತ್ತಾಂಶ_len);

/* ============================================================
 * Network functions
 * ============================================================ */
ಶೂನ್ಯ net__broker_init(ಶೂನ್ಯ);
ಶೂನ್ಯ net__broker_cleanup(ಶೂನ್ಯ);
ರಚನೆ mosquitto *net__socket_accept(ರಚನೆ mosquitto__listener_sock *listensock);
ಪೂರ್ಣಾಂಕ net__socket_listen(ರಚನೆ mosquitto__listener *listener);
ಪೂರ್ಣಾಂಕ net_socket_get_ವಿಳಾಸ(mosq_sock_t sock, ಅಕ್ಷರ *buf, size_t len, uint16_t *remote_ವಿಳಾಸ);
ಪೂರ್ಣಾಂಕ net_tls_ತುಂಬು_verify(ರಚನೆ mosquitto__listener *listener);
ಪೂರ್ಣಾಂಕ net__tls_server_ctx(ರಚನೆ mosquitto__listener *listener);
ಪೂರ್ಣಾಂಕ net_ತುಂಬು_certificates(ರಚನೆ mosquitto__listener *listener);

/* ============================================================
 * Read handling functions
 * ============================================================ */
ಪೂರ್ಣಾಂಕ handle__packet(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ handle__connack(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ handle__connect(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ handle__disconnect(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ handle__publish(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ handle__subscribe(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ handle__unsubscribe(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ handle__auth(ರಚನೆ mosquitto *context);

/* ============================================================
 * Database handling
 * ============================================================ */
ಪೂರ್ಣಾಂಕ db__open(ರಚನೆ mosquitto__config *config);
ಪೂರ್ಣಾಂಕ db__close(ಶೂನ್ಯ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_PERSISTENCE
ಪೂರ್ಣಾಂಕ persist__backup(bool shutdown);
ಪೂರ್ಣಾಂಕ persist__restore(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ
/* Return the number of in-flight messages in count. */
ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_ಎಣಿಕೆ(ಪೂರ್ಣಾಂಕ *ಎಣಿಕೆ_2);
ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_delete_outgoing(ರಚನೆ mosquitto *context, uint16_t mid, ಎಣಿಕೆ mosquitto_ಸಂದೇಶ_state expect_state, ಪೂರ್ಣಾಂಕ qos);
ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_ಸೇರಿಸು_outgoing(ರಚನೆ mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ, uint32_t subscription_identifier, bool update, bool persist);
ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_ಸೇರಿಸು_incoming(ರಚನೆ mosquitto *context, uint64_t cmsg_id, ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ, bool persist);
ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_remove_incoming(ರಚನೆ mosquitto *context, uint16_t mid);
ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_release_incoming(ರಚನೆ mosquitto *context, uint16_t mid);
ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_update_outgoing(ರಚನೆ mosquitto *context, uint16_t mid, ಎಣಿಕೆ mosquitto_ಸಂದೇಶ_state state, ಪೂರ್ಣಾಂಕ qos, bool persist);
ಶೂನ್ಯ db_ಸಂದೇಶ_ಸರತಿಯಿಂದ_ತೆಗೆ_ಮೊದಲ(ರಚನೆ mosquitto *context, ರಚನೆ mosquitto_ಸಂದೇಶ_ದತ್ತಾಂಶ *ಸಂದೇಶ_ದತ್ತಾಂಶ);
ಪೂರ್ಣಾಂಕ db__messages_delete(ರಚನೆ mosquitto *context, bool force_free);
ಪೂರ್ಣಾಂಕ db__messages_delete_incoming(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ db__messages_delete_outgoing(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ db_messages_easy_ಸರತಿ(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic, uint8_t qos, uint32_t payloadlen, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *payload, ಪೂರ್ಣಾಂಕ retain, uint32_t ಸಂದೇಶ_expiry_interval, mosquitto_property **properties);
ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_store(ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto *ಮೂಲ, ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ, uint32_t *ಸಂದೇಶ_expiry_interval, ಎಣಿಕೆ mosquitto_ಸಂದೇಶ_origin origin);
ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_store_ಕಂಡುಹಿಡಿ(ರಚನೆ mosquitto *context, uint16_t mid, ರಚನೆ mosquitto_client_ಸಂದೇಶ **client_ಸಂದೇಶ);
ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_store_ಕೂಡಿಸು(ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ);
ಶೂನ್ಯ db_ಸಂದೇಶ_store_remove(ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ, bool notify);
ಶೂನ್ಯ db_ಸಂದೇಶ_store_ref_inc(ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ);
ಶೂನ್ಯ db_ಸಂದೇಶ_store_ref_dec(ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ **ತಳ_ಸಂದೇಶ);
ಶೂನ್ಯ db_ಸಂದೇಶ_store_clean(ಶೂನ್ಯ);
ಶೂನ್ಯ db_ಸಂದೇಶ_store_compact(ಶೂನ್ಯ);
ಶೂನ್ಯ db_ಸಂದೇಶ_store_free(ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ);
ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_reconnect_reset(ರಚನೆ mosquitto *context);
bool db__ready_for_flight(ರಚನೆ mosquitto *context, ಎಣಿಕೆ mosquitto_ಸಂದೇಶ_direction dir, ಪೂರ್ಣಾಂಕ qos);
bool db_ready_for_ಸರತಿ(ರಚನೆ mosquitto *context, ಪೂರ್ಣಾಂಕ qos, ರಚನೆ mosquitto_ಸಂದೇಶ_ದತ್ತಾಂಶ *ಸಂದೇಶ_ದತ್ತಾಂಶ);
ಶೂನ್ಯ sys_ವೃಕ್ಷ_init(ಶೂನ್ಯ);
ಶೂನ್ಯ sys_ವೃಕ್ಷ_update(bool force);
ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_write_inflight_ಹೊರಹೋಗು_all(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_write_inflight_ಹೊರಹೋಗು_latest(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_write_queued_ಹೊರಹೋಗು(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_write_queued_ಒಳ(ರಚನೆ mosquitto *context);
ಶೂನ್ಯ db_ಸಂದೇಶ_ಕೂಡಿಸು_ಗಮ್ಯ_inflight_stats(ರಚನೆ mosquitto_ಸಂದೇಶ_ದತ್ತಾಂಶ *ಸಂದೇಶ_ದತ್ತಾಂಶ, ರಚನೆ mosquitto_client_ಸಂದೇಶ *ಸಂದೇಶ);
ಶೂನ್ಯ db_ಸಂದೇಶ_ಕೂಡಿಸು_ಗಮ್ಯ_queued_stats(ರಚನೆ mosquitto_ಸಂದೇಶ_ದತ್ತಾಂಶ *ಸಂದೇಶ_ದತ್ತಾಂಶ, ರಚನೆ mosquitto_client_ಸಂದೇಶ *ಸಂದೇಶ);
uint64_t db_new_ಸಂದೇಶ_id(ಶೂನ್ಯ);
ಶೂನ್ಯ db__expire_all_messages(ರಚನೆ mosquitto *context);
ಶೂನ್ಯ db_check_acl_ದ_all_messages(ರಚನೆ mosquitto *context);

/* ============================================================
 * Subscription functions
 * ============================================================ */
ಪೂರ್ಣಾಂಕ sub__init(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ sub_ಕೂಡಿಸು(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto_subscription *sub);
ಪೂರ್ಣಾಂಕ sub__remove(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sub, uint8_t *reason);
ಶೂನ್ಯ sub_ವೃಕ್ಷ_print(ರಚನೆ mosquitto__subhier *ಬೇರು, ಪೂರ್ಣಾಂಕ level);
ಪೂರ್ಣಾಂಕ sub__clean_session(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ sub_messages_ಸರತಿ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ_id, uint8_t qos, ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ **ತಳ_ಸಂದೇಶ);
ಪೂರ್ಣಾಂಕ sub__topic_tokenise(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *subtopic, ಅಕ್ಷರ **local_sub, ಅಕ್ಷರ ***topics, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **sharename);
ಶೂನ್ಯ sub_topic_ಟೋಕನುಗಳು_free(ರಚನೆ sub__token *ಟೋಕನುಗಳು);

/* ============================================================
 * Context functions
 * ============================================================ */
ರಚನೆ mosquitto *context__init(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ context__init_sock(ರಚನೆ mosquitto *context, mosq_sock_t sock, bool get_ವಿಳಾಸ);
ಶೂನ್ಯ context__cleanup(ರಚನೆ mosquitto *context, bool force_free);
ಶೂನ್ಯ context__disconnect(ರಚನೆ mosquitto *context, ಪೂರ್ಣಾಂಕ reason);
ಶೂನ್ಯ context_ಕೂಡಿಸು_ಗಮ್ಯ_disused(ರಚನೆ mosquitto *context);
ಶೂನ್ಯ context__free_disused(ಶೂನ್ಯ);
ಶೂನ್ಯ context__send_will(ರಚನೆ mosquitto *context);
ಶೂನ್ಯ context_ಕೂಡಿಸು_ಗಮ್ಯ_by_id(ರಚನೆ mosquitto *context);
ಶೂನ್ಯ context__remove_from_by_id(ರಚನೆ mosquitto *context);

ಪೂರ್ಣಾಂಕ connect__on_authorised(ರಚನೆ mosquitto *context, ಶೂನ್ಯ *auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು, uint16_t auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len);


/* ============================================================
 * Control functions
 * ============================================================ */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_CONTROL
ಪೂರ್ಣಾಂಕ control__process(ರಚನೆ mosquitto *context, ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ);
ಶೂನ್ಯ control__cleanup(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ
ಪೂರ್ಣಾಂಕ control_ರಿಜಿಸ್ಟರ್_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic, ಶೂನ್ಯ *userdata);
ಪೂರ್ಣಾಂಕ control__unregister_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic);
ಶೂನ್ಯ control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier);


/* ============================================================
 * Logging functions
 * ============================================================ */
ಪೂರ್ಣಾಂಕ log__init(ರಚನೆ mosquitto__config *config);
ಪೂರ್ಣಾಂಕ log__close(ರಚನೆ mosquitto__config *config);
ಶೂನ್ಯ log_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...) __attribute__((format(ಬರೆ, 1, 2)));

/* ============================================================
 * Bridge functions
 * ============================================================ */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
ಶೂನ್ಯ bridge_ಆರಂಭ_all(ಶೂನ್ಯ);
ಶೂನ್ಯ bridge__reload(ಶೂನ್ಯ);
ಶೂನ್ಯ bridge__db_cleanup(ಶೂನ್ಯ);
ಶೂನ್ಯ bridge__cleanup(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ bridge__connect(ರಚನೆ mosquitto *context);
#ಆದರೆ defined(__GLIBC__) && defined(WITH_ADNS)
ಪೂರ್ಣಾಂಕ bridge__connect_step3(ರಚನೆ mosquitto *context);
#ಆದರೆ_ಅಂತ್ಯ
ಪೂರ್ಣಾಂಕ bridge__on_connect(ರಚನೆ mosquitto *context);
ಶೂನ್ಯ bridge_check(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ bridge_ರಿಜಿಸ್ಟರ್_local_connections(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ bridge_ಕೂಡಿಸು_topic(ರಚನೆ mosquitto__bridge *bridge, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic, ಎಣಿಕೆ mosquitto__bridge_direction direction, uint8_t qos, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *local_ಪೂರ್ವಪ್ರತ್ಯಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *remote_ಪೂರ್ವಪ್ರತ್ಯಯ);
ಶೂನ್ಯ bridge__cleanup_topics(ರಚನೆ mosquitto__bridge *bridge);
ಪೂರ್ಣಾಂಕ bridge_remap_topic_ಒಳ(ರಚನೆ mosquitto *context, ಅಕ್ಷರ **topic);
#ಆದರೆ_ಅಂತ್ಯ

/* ============================================================
 * IO multiplex related functions
 * ============================================================ */
ಪೂರ್ಣಾಂಕ mux__init(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ mux_ಕೂಡಿಸು_listeners(ರಚನೆ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಂಕ listensock_ಎಣಿಕೆ);
ಪೂರ್ಣಾಂಕ mux__delete_listeners(ರಚನೆ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಂಕ listensock_ಎಣಿಕೆ);
ಪೂರ್ಣಾಂಕ mux__loop_prepare(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ mux__new(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ mux_ಕೂಡಿಸು_ಹೊರಹೋಗು(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ mux_remove_ಹೊರಹೋಗು(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ mux__delete(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ mux__wait(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ mux__handle(ರಚನೆ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಂಕ listensock_ಎಣಿಕೆ);
ಪೂರ್ಣಾಂಕ mux__cleanup(ಶೂನ್ಯ);

/* ============================================================
 * Listener related functions
 * ============================================================ */
ಬಾಹ್ಯ ರಚನೆ mosquitto__listener_sock *g_listensock;
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ g_listensock_ಎಣಿಕೆ;

ಶೂನ್ಯ listener_ಗಣ_defaults(ರಚನೆ mosquitto__listener *listener);
ಶೂನ್ಯ listeners__reload_all_certificates(ಶೂನ್ಯ);
#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
ಶೂನ್ಯ listeners_ಕೂಡಿಸು_websockets(ರಚನೆ lws_context *ws_context, mosq_sock_t fd);
#ಆದರೆ_ಅಂತ್ಯ
ಪೂರ್ಣಾಂಕ listeners_ಆರಂಭ(ಶೂನ್ಯ);
ಶೂನ್ಯ listeners__stop(ಶೂನ್ಯ);

/* ============================================================
 * Plugin related functions
 * ============================================================ */
ಪೂರ್ಣಾಂಕ plugin_ತುಂಬು_v5(mosquitto_plugin_id_t *plugin, ಶೂನ್ಯ *lib);
ಪೂರ್ಣಾಂಕ plugin_ತುಂಬು_v4(mosquitto_plugin_id_t *plugin, ಶೂನ್ಯ *lib);
ಪೂರ್ಣಾಂಕ plugin_ತುಂಬು_v3(mosquitto_plugin_id_t *plugin, ಶೂನ್ಯ *lib);
ಪೂರ್ಣಾಂಕ plugin_ತುಂಬು_v2(mosquitto_plugin_id_t *plugin, ಶೂನ್ಯ *lib);
ಪೂರ್ಣಾಂಕ acl__pre_check(mosquitto_plugin_id_t *plugin, ರಚನೆ mosquitto *context, ಪೂರ್ಣಾಂಕ access);

ಶೂನ್ಯ LIB_ERROR(ಶೂನ್ಯ);
ಶೂನ್ಯ plugin__handle_connect(ರಚನೆ mosquitto *context);
ಶೂನ್ಯ plugin__handle_disconnect(ರಚನೆ mosquitto *context, ಪೂರ್ಣಾಂಕ reason);
ಶೂನ್ಯ plugin__handle_client_offline(ರಚನೆ mosquitto *context, ಪೂರ್ಣಾಂಕ reason);
ಪೂರ್ಣಾಂಕ plugin_handle_ಸಂದೇಶ_ಒಳ(ರಚನೆ mosquitto *context, ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ_2 *ತಳ_ಸಂದೇಶ);
ಪೂರ್ಣಾಂಕ plugin_handle_ಸಂದೇಶ_ಹೊರಹೋಗು(ರಚನೆ mosquitto *context, ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ_2 *ತಳ_ಸಂದೇಶ);
ಪೂರ್ಣಾಂಕ plugin__handle_subscribe(ರಚನೆ mosquitto *context, ರಚನೆ mosquitto_subscription *sub);
ಪೂರ್ಣಾಂಕ plugin__handle_unsubscribe(ರಚನೆ mosquitto *context, ರಚನೆ mosquitto_subscription *sub);
ಪೂರ್ಣಾಂಕ plugin__handle_reload(ಶೂನ್ಯ);
ಶೂನ್ಯ plugin__handle_tick(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ plugin__callback_unregister_all(mosquitto_plugin_id_t *identifier);
ಶೂನ್ಯ plugin_persist__handle_restore(ಶೂನ್ಯ);
ಶೂನ್ಯ plugin_persist_handle_client_ಕೂಡಿಸು(ರಚನೆ mosquitto *context);
ಶೂನ್ಯ plugin_persist__handle_client_delete(ರಚನೆ mosquitto *context);
ಶೂನ್ಯ plugin_persist__handle_client_update(ರಚನೆ mosquitto *context);
ಶೂನ್ಯ plugin_persist_handle_subscription_ಕೂಡಿಸು(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto_subscription *sub);
ಶೂನ್ಯ plugin_persist__handle_subscription_delete(ರಚನೆ mosquitto *context, ಅಕ್ಷರ *sub);
ಶೂನ್ಯ plugin_persist_handle_client_ಸಂದೇಶ_ಕೂಡಿಸು(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto_client_ಸಂದೇಶ *cmsg);
ಶೂನ್ಯ plugin_persist_handle_client_ಸಂದೇಶ_delete(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto_client_ಸಂದೇಶ *cmsg);
ಶೂನ್ಯ plugin_persist_handle_client_ಸಂದೇಶ_update(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto_client_ಸಂದೇಶ *cmsg);
ಶೂನ್ಯ plugin_persist_handle_ತಳ_ಸಂದೇಶ_ಕೂಡಿಸು(ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ);
ಶೂನ್ಯ plugin_persist_handle_ತಳ_ಸಂದೇಶ_delete(ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ);
ಶೂನ್ಯ plugin_persist_handle_retain_ಸಂದೇಶ_ಗಣ(ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ);
ಶೂನ್ಯ plugin_persist_handle_retain_ಸಂದೇಶ_delete(ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ);
ಶೂನ್ಯ plugin_persist_handle_will_ಕೂಡಿಸು(ರಚನೆ mosquitto *context);
ಶೂನ್ಯ plugin_persist__handle_will_delete(ರಚನೆ mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
ಪೂರ್ಣಾಂಕ keepalive__init(ಶೂನ್ಯ);
ಶೂನ್ಯ keepalive__cleanup(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ keepalive_ಕೂಡಿಸು(ರಚನೆ mosquitto *context);
ಶೂನ್ಯ keepalive__check(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ keepalive__remove(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ keepalive__update(ರಚನೆ mosquitto *context);

/* ============================================================
 * Property related functions
 * ============================================================ */
ಪೂರ್ಣಾಂಕ property__process_connect(ರಚನೆ mosquitto *context, mosquitto_property **props);
ಪೂರ್ಣಾಂಕ property__process_will(ರಚನೆ mosquitto *context, ರಚನೆ mosquitto_ಸಂದೇಶ_all *ಸಂದೇಶ, mosquitto_property **props);
ಪೂರ್ಣಾಂಕ property__process_publish(ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ, mosquitto_property **props, ಪೂರ್ಣಾಂಕ *topic_alias, uint32_t *ಸಂದೇಶ_expiry_interval, bool is_bridge);
ಪೂರ್ಣಾಂಕ property__process_disconnect(ರಚನೆ mosquitto *context, mosquitto_property **props);

/* ============================================================
 * Retain tree related functions
 * ============================================================ */
ಪೂರ್ಣಾಂಕ retain__init(ಶೂನ್ಯ);
ಶೂನ್ಯ retain__clean(ರಚನೆ mosquitto__retainhier **retainhier);
ಪೂರ್ಣಾಂಕ retain_ಸರತಿ(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto_subscription *sub);
ಪೂರ್ಣಾಂಕ retain__store(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic, ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ, ಅಕ್ಷರ **ವಿಭಜಿಸು_topics, bool persist);
ಶೂನ್ಯ retain__expiry_check(ಶೂನ್ಯ);
ಶೂನ್ಯ retain__expire(ರಚನೆ mosquitto__retainhier **retainhier);

/* ============================================================
 * Security related functions
 * ============================================================ */
ಪೂರ್ಣಾಂಕ acl_ಕಂಡುಹಿಡಿ_acls(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ plugin_ತುಂಬು_all(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ plugin__unload_all(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ config_plugin_ಕೂಡಿಸು_secopt(mosquitto_plugin_id_t *plugin, ರಚನೆ mosquitto__security_options *security_options);

ಪೂರ್ಣಾಂಕ mosquitto_security_init(bool reload);
ಪೂರ್ಣಾಂಕ mosquitto_security_cleanup(bool reload);
ಪೂರ್ಣಾಂಕ mosquitto_acl_check(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic, uint32_t payloadlen, ಶೂನ್ಯ *payload, uint8_t qos, bool retain, mosquitto_property *properties, ಪೂರ್ಣಾಂಕ access);
ಪೂರ್ಣಾಂಕ mosquitto_basic_auth(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ mosquitto_psk_ಕೀಲಿ_get(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hint, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ತಟಸ್ಥ, ಅಕ್ಷರ *ಕೀಲಿ, ಪೂರ್ಣಾಂಕ max_ಕೀಲಿ_len);

ಪೂರ್ಣಾಂಕ mosquitto_security_init_default(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ mosquitto_security_apply_default(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ mosquitto_security_cleanup_default(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ mosquitto_psk_ಕೀಲಿ_get_default(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hint, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ತಟಸ್ಥ, ಅಕ್ಷರ *ಕೀಲಿ, ಪೂರ್ಣಾಂಕ max_ಕೀಲಿ_len);
ಪೂರ್ಣಾಂಕ broker_acl_file__init(ಶೂನ್ಯ);
ಶೂನ್ಯ broker_acl_file__cleanup(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ broker_password_file__init(ಶೂನ್ಯ);
ಶೂನ್ಯ broker_password_file__cleanup(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ psk_file__init(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ psk_file__cleanup(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ mosquitto_security_auth_ಆರಂಭ(ರಚನೆ mosquitto *context, bool reauth, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ_ಒಳ, uint16_t ದತ್ತಾಂಶ_ಒಳ_len, ಶೂನ್ಯ **ದತ್ತಾಂಶ_ಹೊರಹೋಗು, uint16_t *ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len);
ಪೂರ್ಣಾಂಕ mosquitto_security_auth_continue(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ_ಒಳ, uint16_t ದತ್ತಾಂಶ_len, ಶೂನ್ಯ **ದತ್ತಾಂಶ_ಹೊರಹೋಗು, uint16_t *ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len);

ಶೂನ್ಯ unpwd_free_ಅಂಶ(ರಚನೆ mosquitto__unpwd **unpwd, ರಚನೆ mosquitto__unpwd *ಅಂಶ);

/* ============================================================
 * Session expiry
 * ============================================================ */
ಪೂರ್ಣಾಂಕ session_expiry_ಕೂಡಿಸು(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ session_expiry_ಕೂಡಿಸು_from_persistence(ರಚನೆ mosquitto *context, time_t expiry_ಸಮಯ);
ಶೂನ್ಯ session_expiry_ಕೂಡಿಸು_on_shutdown(ಶೂನ್ಯ);
ಶೂನ್ಯ session_expiry__remove(ರಚನೆ mosquitto *context);
ಶೂನ್ಯ session_expiry__remove_all(ಶೂನ್ಯ);
ಶೂನ್ಯ session_expiry__check(ಶೂನ್ಯ);
ಶೂನ್ಯ session_expiry__send_all(ಶೂನ್ಯ);

/* ============================================================
 * Signals
 * ============================================================ */
ಶೂನ್ಯ signal_ಸಿದ್ಧತೆ(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ signal__flag_check(ಶೂನ್ಯ);

/* ============================================================
 * Window service and signal related functions
 * ============================================================ */
#ಆದರೆ defined(WIN32) || defined(__CYGWIN__)
ಶೂನ್ಯ service_install(ಅಕ್ಷರ *name);
ಶೂನ್ಯ service_uninstall(ಅಕ್ಷರ *name);
ಶೂನ್ಯ service_ಸತತ(ಅಕ್ಷರ *name);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
DWORD WINAPI SigThreadProc(ಶೂನ್ಯ *ದತ್ತಾಂಶ);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

/* ============================================================
 * Watchdog
 * ============================================================ */
ಶೂನ್ಯ watchdog__init(ಶೂನ್ಯ);
ಶೂನ್ಯ watchdog__check(ಶೂನ್ಯ);

/* ============================================================
 * Websockets related functions
 * ============================================================ */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_WEBSOCKETS
#  ಆದರೆ WITH_WEBSOCKETS == WS_IS_LWS
ಶೂನ್ಯ mosq_websockets_init(ರಚನೆ mosquitto__listener *listener, ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto__config *conf);
#  endif
ಪೂರ್ಣಾಂಕ http__context_init(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ http__context_cleanup(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ http__read(ರಚನೆ mosquitto *context);
#ಆದರೆ_ಅಂತ್ಯ
ಶೂನ್ಯ do_disconnect(ರಚನೆ mosquitto *context, ಪೂರ್ಣಾಂಕ reason);

/* ============================================================
 * PROXY related functions
 * ============================================================ */
ಪೂರ್ಣಾಂಕ proxy_v2__read(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ proxy_v1__read(ರಚನೆ mosquitto *context);

/* ============================================================
 * Will delay
 * ============================================================ */
ಪೂರ್ಣಾಂಕ will_delay_ಕೂಡಿಸು(ರಚನೆ mosquitto *context);
ಶೂನ್ಯ will_delay__check(ಶೂನ್ಯ);
ಶೂನ್ಯ will_delay__send_all(ಶೂನ್ಯ);
ಶೂನ್ಯ will_delay__remove(ರಚನೆ mosquitto *mosq);

/* ============================================================
 * HTTP Info
 * ============================================================ */
ಪೂರ್ಣಾಂಕ http_api_ಆರಂಭ_local(ರಚನೆ mosquitto__listener *listener);
ಪೂರ್ಣಾಂಕ http_api_ಆರಂಭ(ರಚನೆ mosquitto__listener *listener);
ಶೂನ್ಯ http_api__stop(ರಚನೆ mosquitto__listener *listener);


/* ============================================================
 * Other
 * ============================================================ */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_XTREPORT
ಶೂನ್ಯ xtreport(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ broker_control__init(ಶೂನ್ಯ);
ಶೂನ್ಯ broker_control__cleanup(ಶೂನ್ಯ);
ಶೂನ್ಯ broker_control__reload(ಶೂನ್ಯ);

#ಆದರೆ_ಅಂತ್ಯ
