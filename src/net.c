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
*/

#ಅನ್ತರ್ಭಾವ "config.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
#  include <arpa/inet.h>
#  ifndef _AIX
#    include <ifaddrs.h>
#  endif
#  include <netdb.h>
#  include <netinet/tcp.h>
#  include <strings.h>
#  include <sys/socket.h>
#  include <unistd.h>
#ಇಲ್ಲದಿದ್ದರೆ
#  include <winsock2.h>
#  include <ws2tcpip.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_WRAP
#  include <tcpd.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#  include <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(WITH_UNIX_SOCKETS) || defined(WITH_TLS)
#  include "sys/stat.h"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_UNIX_SOCKETS
#  ifdef WIN32
#    include <afunix.h>
#  ಇಲ್ಲದಿದ್ದರೆ
#    include <sys/un.h>
#  endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __QNX__
#  include <net/netbyte.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "net_mosq.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
#  include "tls_mosq.h"
#  include <openssl/err.h>
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tls_ex_ಸೂಚ್ಯಂಕ_context = -1;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tls_ex_ಸೂಚ್ಯಂಕ_listener = -1;
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "sys_tree.h"

/* For EMFILE handling */
ಸ್ಥಿರ mosq_sock_t ಹೆಚ್ಚುವರಿ_sock = INVALID_SOCKET;


ಶೂನ್ಯ net__broker_init(ಶೂನ್ಯ)
{
	ಹೆಚ್ಚುವರಿ_sock = socket(AF_INET, SOCK_STREAM, 0);
	net__init();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
	net__init_tls();
#ಆದರೆ_ಅಂತ್ಯ
}


ಶೂನ್ಯ net__broker_cleanup(ಶೂನ್ಯ)
{
	ಆದರೆ(ಹೆಚ್ಚುವರಿ_sock != INVALID_SOCKET){
		COMPAT_CLOSE(ಹೆಚ್ಚುವರಿ_sock);
		ಹೆಚ್ಚುವರಿ_sock = INVALID_SOCKET;
	}
	net__cleanup();
}


ಸ್ಥಿರ ಶೂನ್ಯ net__print_error(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಲಘುಗಣಕ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format_str)
{
	ಅಕ್ಷರ *buf;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
	Format_ಸಂದೇಶ(FORMAT_ಸಂದೇಶ_ALLOCATE_ಬಫರ್ | FORMAT_ಸಂದೇಶ_FROM_SYSTEM,
			ಖಾಲಿ, WSAGet_ಕೊನೆಯ_Error(), LANG_NEUTRAL, (LPTSTR)&buf, 0, ಖಾಲಿ);

	log__printf(ಖಾಲಿ, ಲಘುಗಣಕ, format_str, buf);
	LocalFree(buf);
#ಇಲ್ಲದಿದ್ದರೆ
	buf = strerror(errno);
	log__printf(ಖಾಲಿ, ಲಘುಗಣಕ, format_str, buf);
#ಆದರೆ_ಅಂತ್ಯ
}


ರಚನೆ mosquitto *net__socket_accept(ರಚನೆ mosquitto__listener_sock *listensock)
{
	mosq_sock_t new_sock = INVALID_SOCKET;
	ರಚನೆ mosquitto *new_context;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
	BIO *bio;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_WRAP
	ರಚನೆ request_info wrap_req;
	ಅಕ್ಷರ ವಿಳಾಸ[1024];
#ಆದರೆ_ಅಂತ್ಯ

	new_sock = accept(listensock->sock, ಖಾಲಿ, 0);
	ಆದರೆ(new_sock == INVALID_SOCKET){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
		WINDOWS_ಗಣ_ERRNO();
		ಆದರೆ(errno == WSAEMFILE){
#ಇಲ್ಲದಿದ್ದರೆ
		ಆದರೆ(errno == EMFILE || errno == ENFILE){
#ಆದರೆ_ಅಂತ್ಯ
			/* Close the spare socket, which means we should be able to accept
			 * this connection. Accept it, then close it immediately and create
			 * a new spare_sock. This prevents the situation of ever properly
			 * running out of sockets.
			 * It would be nice to send a "server not available" connack here,
			 * but there are lots of reasons why this would be tricky (TLS
			 * being the big one). */
			COMPAT_CLOSE(ಹೆಚ್ಚುವರಿ_sock);
			new_sock = accept(listensock->sock, ಖಾಲಿ, 0);
			ಆದರೆ(new_sock != INVALID_SOCKET){
				COMPAT_CLOSE(new_sock);
			}
			ಹೆಚ್ಚುವರಿ_sock = socket(AF_INET, SOCK_STREAM, 0);
			log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING,
					"Unable to accept new connection, system socket count has been exceeded. Try increasing \"ulimit -n\" or equivalent.");
		}
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	metrics__int_inc(mosq_counter_socket_connections, 1);

	ಆದರೆ(net__socket_nonblock(&new_sock)){
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_WRAP
	/* Use tcpd / libwrap to determine whether a connection is allowed. */
	request_init(&wrap_req, RQ_FILE, new_sock, RQ_DAEMON, "mosquitto", 0);
	fromhost(&wrap_req);
	ಆದರೆ(!hosts_access(&wrap_req)){
		/* Access is denied */
		ಆದರೆ(db.config->connection_messages == true){
			ಆದರೆ(!net_socket_get_ವಿಳಾಸ(new_sock, ವಿಳಾಸ, 1024, ಖಾಲಿ)){
				log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client connection from %s denied access by tcpd.", ವಿಳಾಸ);
			}
		}
		COMPAT_CLOSE(new_sock);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ(db.config->ಗಣ_tcp_nodelay && listensock->listener->ಪೋರ್ಟ್){
		ಪೂರ್ಣಾಂಕ flag = 1;
		ಆದರೆ(setsockopt(new_sock, IPPROTO_TCP, TCP_NODELAY, (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)&flag, ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ)) != 0){
			log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Unable to set TCP_NODELAY.");
		}
	}

	new_context = context__init();
	ಆದರೆ(!new_context){
		COMPAT_CLOSE(new_sock);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	new_context->listener = listensock->listener;
	ಆದರೆ(!new_context->listener){
		context__cleanup(new_context, true);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	new_context->listener->client_ಎಣಿಕೆ++;

	ಆದರೆ(new_context->listener->enable_proxy_protocol){
		ಆದರೆ(context__init_sock(new_context, new_sock, false) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		ಆದರೆ(new_context->listener->enable_proxy_protocol == 2){
			new_context->transport = mosq_t_proxy_v2;
		}ಇಲ್ಲದಿದ್ದರೆ{
			new_context->transport = mosq_t_proxy_v1;
		}
		new_context->proxy.cmd = -1;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಆದರೆ(context__init_sock(new_context, new_sock, true) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		ಆಯ್ಕೆ(new_context->listener->protocol){
			ಸ್ಥಿತಿ mp_mqtt:
				new_context->transport = mosq_t_tcp;
				ನಿಲ್ಲಿಸು;
#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			ಸ್ಥಿತಿ mp_websockets:
				ಆದರೆ(http__context_init(new_context)){
					context__cleanup(new_context, true);
					ಹಿಂತಿರುಗಿ ಖಾಲಿ;
				}
				ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
			ಪೂರ್ವನಿಯೋಜಿತ:
				context__cleanup(new_context, true);
				ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
	}

	ಆದರೆ((new_context->listener->max_connections > 0 && new_context->listener->client_ಎಣಿಕೆ > new_context->listener->max_connections)
			|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)db.config->global_max_connections)){

		ಆದರೆ(db.config->connection_messages == true){
			log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", new_context->ವಿಳಾಸ);
		}
		context__cleanup(new_context, true);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
	/* TLS init */
	ಆದರೆ(new_context->listener->ssl_ctx){
		new_context->ssl = SSL_new(new_context->listener->ssl_ctx);
		ಆದರೆ(!new_context->ssl){
			context__cleanup(new_context, true);
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		ಆದರೆ(!SSL_ಗಣ_ex_ದತ್ತಾಂಶ(new_context->ssl, tls_ex_ಸೂಚ್ಯಂಕ_context, new_context)
				|| !SSL_ಗಣ_ex_ದತ್ತಾಂಶ(new_context->ssl, tls_ex_ಸೂಚ್ಯಂಕ_listener, new_context->listener)){
			context__cleanup(new_context, true);
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		new_context->want_write = true;
		bio = BIO_new_socket(new_sock, BIO_NOCLOSE);
		SSL_ಗಣ_bio(new_context->ssl, bio, bio);
		ERR_clear_error();
		SSL_ಗಣ_accept_state(new_context->ssl);
	}
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ(db.config->connection_messages == true
			&& !new_context->listener->enable_proxy_protocol){

		log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "New connection from %s:%d on port %d.",
				new_context->ವಿಳಾಸ, new_context->remote_ಪೋರ್ಟ್, new_context->listener->ಪೋರ್ಟ್);
	}

	mux__new(new_context);
	keepalive_ಕೂಡಿಸು(new_context);

	ಹಿಂತಿರುಗಿ new_context;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ client_certificate_verify(ಪೂರ್ಣಾಂಕ preverify_ok, X509_STORE_CTX *ctx)
{
	/* Preverify should check expiry, revocation. */
	ಆದರೆ(preverify_ok == 0){
		SSL *ssl;
		ssl = X509_STORE_CTX_get_ex_ದತ್ತಾಂಶ(ctx, SSL_get_ex_ದತ್ತಾಂಶ_X509_STORE_CTX_idx());
		ಆದರೆ(ssl){
			ರಚನೆ mosquitto__listener *listener;
			listener = SSL_get_ex_ದತ್ತಾಂಶ(ssl, tls_ex_ಸೂಚ್ಯಂಕ_listener);

			ಆದರೆ(listener && listener->disable_client_cert_date_checks){
				ಪೂರ್ಣಾಂಕ err = X509_STORE_CTX_get_error(ctx);
				ಆದರೆ(err == X509_V_ERR_CERT_NOT_YET_VALID || err == X509_V_ERR_CERT_HAS_EXPIRED){
					preverify_ok = 1;
				}
			}
		}
	}
	ಹಿಂತಿರುಗಿ preverify_ok;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FINAL_WITH_TLS_PSK


ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ psk_server_callback(SSL *ssl, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ತಟಸ್ಥ, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *psk, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ max_psk_len)
{
	ರಚನೆ mosquitto *context;
	ರಚನೆ mosquitto__listener *listener;
	ಅಕ್ಷರ *psk_ಕೀಲಿ = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ len;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *psk_hint;

	ಆದರೆ(!ತಟಸ್ಥ){
		ಹಿಂತಿರುಗಿ 0;
	}

	context = SSL_get_ex_ದತ್ತಾಂಶ(ssl, tls_ex_ಸೂಚ್ಯಂಕ_context);
	ಆದರೆ(!context){
		ಹಿಂತಿರುಗಿ 0;
	}

	listener = SSL_get_ex_ದತ್ತಾಂಶ(ssl, tls_ex_ಸೂಚ್ಯಂಕ_listener);
	ಆದರೆ(!listener){
		ಹಿಂತಿರುಗಿ 0;
	}

	psk_hint = listener->psk_hint;

	/* The hex to BN conversion results in the length halving, so we can pass
	 * max_psk_len*2 as the max hex key here. */
	psk_ಕೀಲಿ = mosquitto_calloc(1, (size_t)max_psk_len*2 + 1);
	ಆದರೆ(!psk_ಕೀಲಿ){
		ಹಿಂತಿರುಗಿ 0;
	}

	ಆದರೆ(mosquitto_psk_ಕೀಲಿ_get(context, psk_hint, ತಟಸ್ಥ, psk_ಕೀಲಿ, (ಪೂರ್ಣಾಂಕ)max_psk_len*2) != MOSQ_ERR_SUCCESS){
		mosquitto_FREE(psk_ಕೀಲಿ);
		ಹಿಂತಿರುಗಿ 0;
	}

	len = mosquitto__hex2bin(psk_ಕೀಲಿ, psk, (ಪೂರ್ಣಾಂಕ)max_psk_len);
	ಆದರೆ(len < 0){
		mosquitto_FREE(psk_ಕೀಲಿ);
		ಹಿಂತಿರುಗಿ 0;
	}

	ಆದರೆ(listener->use_ತಟಸ್ಥ_as_username){
		ಆದರೆ(mosquitto_validate_utf8(ತಟಸ್ಥ, (ಪೂರ್ಣಾಂಕ)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ತಟಸ್ಥ))){
			mosquitto_free(psk_ಕೀಲಿ);
			ಹಿಂತಿರುಗಿ 0;
		}
		context->username = mosquitto_strdup(ತಟಸ್ಥ);
		ಆದರೆ(!context->username){
			mosquitto_FREE(psk_ಕೀಲಿ);
			ಹಿಂತಿರುಗಿ 0;
		}
	}

	mosquitto_FREE(psk_ಕೀಲಿ);
	ಹಿಂತಿರುಗಿ (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)len;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS


ಸ್ಥಿರ ಶೂನ್ಯ tls_keylog_callback(ಸ್ಥಿರಾಂಕ SSL *ssl, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು)
{
	ಬಳಸದ(ssl);

	ಆದರೆ(db.tls_keylog){
		FILE *fptr;
		fptr = mosquitto_fopen(db.tls_keylog, "at", true);
		ಆದರೆ(fptr){
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
			/* Until mosquitto_fopen enforces file permissions on all files,
			 * enforce it here. We can enforce it here, because it isn't a
			 * change of behaviour. */
			ರಚನೆ stat statbuf;
			ಆದರೆ(fstat(fileno(fptr), &statbuf) < 0
					|| statbuf.st_ಬಹುಲಕ & (S_IRWXG | S_IRWXO)){

				fclose(fptr);
				ಹಿಂತಿರುಗಿ;
			}
#ಆದರೆ_ಅಂತ್ಯ
			fprintf(fptr, "%s\n", ಸಾಲು);
			fclose(fptr);
		}ಇಲ್ಲದಿದ್ದರೆ{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
			ಆದರೆ(errno == ELOOP){
				log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Error: keylog file must not be a symbolic link");
			}ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
			{
				log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Error: Unable to open keylog file: %s", strerror(errno));
			}
		}
	}
}


ಪೂರ್ಣಾಂಕ net__tls_server_ctx(ರಚನೆ mosquitto__listener *listener)
{
	ಅಕ್ಷರ buf[256];
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ(tls_ex_ಸೂಚ್ಯಂಕ_context == -1){
		tls_ex_ಸೂಚ್ಯಂಕ_context = SSL_get_ex_new_ಸೂಚ್ಯಂಕ(0, "client context", ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
	}
	ಆದರೆ(tls_ex_ಸೂಚ್ಯಂಕ_listener == -1){
		tls_ex_ಸೂಚ್ಯಂಕ_listener = SSL_get_ex_new_ಸೂಚ್ಯಂಕ(0, "listener", ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
	}

	ಆದರೆ(listener->ssl_ctx){
		SSL_CTX_free(listener->ssl_ctx);
		listener->ssl_ctx = ಖಾಲಿ;
	}

	listener->ssl_ctx = SSL_CTX_new(TLS_server_ವಿಧಾನ());

	ಆದರೆ(!listener->ssl_ctx){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to create TLS context.");
		ಹಿಂತಿರುಗಿ MOSQ_ERR_TLS;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SSL_OP_NO_TLSv1_3
	ಆದರೆ(db.config->ಪ್ರತಿ_listener_settings){
		ಆದರೆ(listener->security_options->psk_file){
			SSL_CTX_ಗಣ_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಆದರೆ(db.config->security_options.psk_file){
			SSL_CTX_ಗಣ_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ(listener->tls_ಆವೃತ್ತಿ == ಖಾಲಿ){
		SSL_CTX_ಗಣ_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SSL_OP_NO_TLSv1_3
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(listener->tls_ಆವೃತ್ತಿ, "tlsv1.3")){
		SSL_CTX_ಗಣ_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2);
#ಆದರೆ_ಅಂತ್ಯ
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(listener->tls_ಆವೃತ್ತಿ, "tlsv1.2")){
		SSL_CTX_ಗಣ_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(listener->tls_ಆವೃತ್ತಿ, "tlsv1.1")){
		SSL_CTX_ಗಣ_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1);
		log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: TLS v1.1 is insecure. It must only be used in the case "
				"where you have devices that cannot be upgraded to use a secure version of TLS. It is "
				"recommended to use as secure a set of ciphers as possible and that will restrict it to "
				"TLS v1.1 only, and to use a separate listener using TLS v1.2 and secure ciphers for your "
				"other devices.");
		log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Please be aware that support for TLS v1.1 will go away eventually "
				"and that you should plan now to migrate away from it.");
	}ಇಲ್ಲದಿದ್ದರೆ{
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unsupported tls_version \"%s\".", listener->tls_ಆವೃತ್ತಿ);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_TLS;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SSL_OP_NO_COMPRESSION
	/* Disable compression */
	SSL_CTX_ಗಣ_options(listener->ssl_ctx, SSL_OP_NO_COMPRESSION);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SSL_OP_CIPHER_SERVER_PREFERENCE
	/* Server chooses cipher */
	SSL_CTX_ಗಣ_options(listener->ssl_ctx, SSL_OP_CIPHER_SERVER_PREFERENCE);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SSL_ಬಹುಲಕ_RELEASE_BUFFERS
	/* Use even less memory per SSL connection. */
	SSL_CTX_ಗಣ_ಬಹುಲಕ(listener->ssl_ctx, SSL_ಬಹುಲಕ_RELEASE_BUFFERS);
#ಆದರೆ_ಅಂತ್ಯ

	SSL_CTX_ಗಣ_dh_auto(listener->ssl_ctx, 1);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SSL_OP_NO_RENEGOTIATION
	SSL_CTX_ಗಣ_options(listener->ssl_ctx, SSL_OP_NO_RENEGOTIATION);
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ(db.tls_keylog){
		log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "TLS key logging to '%s' enabled for all listeners.",
				db.tls_keylog);
		log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "TLS key logging is for DEBUGGING only.");

		SSL_CTX_ಗಣ_keylog_callback(listener->ssl_ctx, tls_keylog_callback);
	}

	snprintf(buf, 256, "mosquitto-%d", listener->ಪೋರ್ಟ್);
	SSL_CTX_ಗಣ_session_id_context(listener->ssl_ctx, (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)buf, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(buf));

	ಆದರೆ(listener->ciphers){
		rc = SSL_CTX_ಗಣ_cipher_list(listener->ssl_ctx, listener->ciphers);
		ಆದರೆ(rc == 0){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_TLS;
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		rc = SSL_CTX_ಗಣ_cipher_list(listener->ssl_ctx, "DEFAULT:!aNULL:!eNULL:!LOW:!EXPORT:!SSLv2:@STRENGTH");
		ಆದರೆ(rc == 0){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_TLS;
		}
	}
	ಆದರೆ(listener->ciphers_tls13){
		rc = SSL_CTX_ಗಣ_ciphersuites(listener->ssl_ctx, listener->ciphers_tls13);
		ಆದರೆ(rc == 0){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to set TLS 1.3 ciphersuites. Check cipher_tls13 list \"%s\".", listener->ciphers_tls13);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_TLS;
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}
#ಆದರೆ_ಅಂತ್ಯ


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ net_ತುಂಬು_crl_file(ರಚನೆ mosquitto__listener *listener)
{
	X509_STORE *store;
	X509_LOOKUP *lookup;
	ಪೂರ್ಣಾಂಕ rc;

	store = SSL_CTX_get_cert_store(listener->ssl_ctx);
	ಆದರೆ(!store){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to obtain TLS store.");
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		ಹಿಂತಿರುಗಿ MOSQ_ERR_TLS;
	}
	lookup = X509_STORE_ಕೂಡಿಸು_lookup(store, X509_LOOKUP_file());
	rc = X509_ತುಂಬು_crl_file(lookup, listener->crlfile, X509_FILETYPE_PEM);
	ಆದರೆ(rc < 1){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to load certificate revocation file \"%s\". Check crlfile.", listener->crlfile);
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		net__print_ssl_error(ಖಾಲಿ, ಖಾಲಿ);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_TLS;
	}
	X509_STORE_ಗಣ_flags(store, X509_V_FLAG_CRL_CHECK);

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}
#ಆದರೆ_ಅಂತ್ಯ


ಪೂರ್ಣಾಂಕ net_ತುಂಬು_certificates(ರಚನೆ mosquitto__listener *listener)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ(listener->require_certificate){
		SSL_CTX_ಗಣ_verify(listener->ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, client_certificate_verify);
	}ಇಲ್ಲದಿದ್ದರೆ{
		SSL_CTX_ಗಣ_verify(listener->ssl_ctx, SSL_VERIFY_NONE, client_certificate_verify);
	}
	rc = SSL_CTX_use_certificate_chain_file(listener->ssl_ctx, listener->certfile);
	ಆದರೆ(rc != 1){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
		net__print_ssl_error(ಖಾಲಿ, ಖಾಲಿ);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_TLS;
	}
	ಆದರೆ(listener->tls_engine == ಖಾಲಿ || listener->tls_keyform == mosq_k_pem){
		rc = SSL_CTX_use_ಖಾಸಗಿ_ಕೀಲಿ_file(listener->ssl_ctx, listener->keyfile, SSL_FILETYPE_PEM);
		ಆದರೆ(rc != 1){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			net__print_ssl_error(ಖಾಲಿ, ಖಾಲಿ);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_TLS;
		}
	}
	rc = SSL_CTX_check_ಖಾಸಗಿ_ಕೀಲಿ(listener->ssl_ctx);
	ಆದರೆ(rc != 1){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Server certificate/key are inconsistent.");
		net__print_ssl_error(ಖಾಲಿ, ಖಾಲಿ);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_TLS;
	}
	/* Load CRLs if they exist. */
	ಆದರೆ(listener->crlfile){
		rc = net_ತುಂಬು_crl_file(listener);
		ಆದರೆ(rc){
			ಹಿಂತಿರುಗಿ rc;
		}
	}
#ಇಲ್ಲದಿದ್ದರೆ
	ಬಳಸದ(listener);
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


#ಆದರೆ defined(WITH_TLS) && !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ net_ತುಂಬು_engine(ರಚನೆ mosquitto__listener *listener)
{
	ENGINE *engine = ಖಾಲಿ;
	UI_ವಿಧಾನ *ui_ವಿಧಾನ;
	EVP_PKEY *pkey;

	ಆದರೆ(!listener->tls_engine){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}

	engine = ENGINE_by_id(listener->tls_engine);
	ಆದರೆ(!engine){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error loading %s engine\n", listener->tls_engine);
		net__print_ssl_error(ಖಾಲಿ, ಖಾಲಿ);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_TLS;
	}
	ಆದರೆ(!ENGINE_init(engine)){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Failed engine initialisation\n");
		net__print_ssl_error(ಖಾಲಿ, ಖಾಲಿ);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_TLS;
	}
	ENGINE_ಗಣ_default(engine, ENGINE_ವಿಧಾನ_ALL);

	ಆದರೆ(listener->tls_keyform == mosq_k_engine){
		ui_ವಿಧಾನ = net_get_ui_ವಿಧಾನ();
		ಆದರೆ(listener->tls_engine_kpass_sha1){
			ಆದರೆ(!ENGINE_ctrl_cmd(engine, ENGINE_SECRET_ಬಹುಲಕ, ENGINE_SECRET_ಬಹುಲಕ_SHA, ಖಾಲಿ, ಖಾಲಿ, 0)){
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to set engine secret mode sha");
				net__print_ssl_error(ಖಾಲಿ, ಖಾಲಿ);
				ಹಿಂತಿರುಗಿ MOSQ_ERR_TLS;
			}
			ಆದರೆ(!ENGINE_ctrl_cmd(engine, ENGINE_PIN, 0, listener->tls_engine_kpass_sha1, ಖಾಲಿ, 0)){
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to set engine pin");
				net__print_ssl_error(ಖಾಲಿ, ಖಾಲಿ);
				ಹಿಂತಿರುಗಿ MOSQ_ERR_TLS;
			}
			ui_ವಿಧಾನ = ಖಾಲಿ;
		}
		pkey = ENGINE_ತುಂಬು_ಖಾಸಗಿ_ಕೀಲಿ(engine, listener->keyfile, ui_ವಿಧಾನ, ಖಾಲಿ);
		ಆದರೆ(!pkey){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to load engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(ಖಾಲಿ, ಖಾಲಿ);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_TLS;
		}
		ಆದರೆ(SSL_CTX_use_ಖಾಸಗಿ_ಕೀಲಿ(listener->ssl_ctx, pkey) <= 0){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to use engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(ಖಾಲಿ, ಖಾಲಿ);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_TLS;
		}
	}
	ENGINE_free(engine); /* release the structural reference from ENGINE_by_id() */

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}
#ಆದರೆ_ಅಂತ್ಯ


ಪೂರ್ಣಾಂಕ net_tls_ತುಂಬು_verify(ರಚನೆ mosquitto__listener *listener)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
	ಪೂರ್ಣಾಂಕ rc;

#  ಆದರೆ OPENSSL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ < 0x30000000L
	ಆದರೆ(listener->cafile || listener->capath){
		rc = SSL_CTX_ತುಂಬು_verify_locations(listener->ssl_ctx, listener->cafile, listener->capath);
		ಆದರೆ(rc == 0){
			ಆದರೆ(listener->cafile && listener->capath){
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\" and capath \"%s\".", listener->cafile, listener->capath);
			}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(listener->cafile){
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			}ಇಲ್ಲದಿದ್ದರೆ{
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			}
		}
	}
#  ಇಲ್ಲದಿದ್ದರೆ
	ರಾಶಿ_ದ(X509_NAME) *ca_ಹೆಸರುಗಳು = ಖಾಲಿ;
	ಆದರೆ(listener->cafile || listener->capath){
		ca_ಹೆಸರುಗಳು = sk_X509_NAME_new_null();
	}

	ಆದರೆ(listener->cafile){
		rc = SSL_CTX_ತುಂಬು_verify_file(listener->ssl_ctx, listener->cafile);
		ಆದರೆ(rc == 0){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			net__print_ssl_error(ಖಾಲಿ, ಖಾಲಿ);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_TLS;
		}

		ರಾಶಿ_ದ(X509_NAME) *cert_ಹೆಸರುಗಳು = SSL_ತುಂಬು_client_CA_file(listener->cafile);
		ಆದರೆ(cert_ಹೆಸರುಗಳು){
			ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<sk_X509_NAME_ಅಂಕೆ(cert_ಹೆಸರುಗಳು); i++){
				sk_X509_NAME_ತಳ್ಳು(ca_ಹೆಸರುಗಳು, X509_NAME_dup(sk_X509_NAME_ಮೌಲ್ಯ(cert_ಹೆಸರುಗಳು, i)));
			}
			sk_X509_NAME_ತೆಗೆ_free(cert_ಹೆಸರುಗಳು, X509_NAME_free);
		}

	}
	ಆದರೆ(listener->capath){
		rc = SSL_CTX_ತುಂಬು_verify_dir(listener->ssl_ctx, listener->capath);
		ಆದರೆ(rc == 0){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			net__print_ssl_error(ಖಾಲಿ, ಖಾಲಿ);
			sk_X509_NAME_ತೆಗೆ_free(ca_ಹೆಸರುಗಳು, X509_NAME_free);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_TLS;
		}

		SSL_ಕೂಡಿಸು_dir_cert_subjects_ಗಮ್ಯ_ರಾಶಿ(ca_ಹೆಸರುಗಳು, listener->capath);
	}
	ಆದರೆ(ca_ಹೆಸರುಗಳು){
		SSL_CTX_ಗಣ_client_CA_list(listener->ssl_ctx, ca_ಹೆಸರುಗಳು);
	}
#  endif

#  ಆದರೆ !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000
	ಆದರೆ(net_ತುಂಬು_engine(listener)){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_TLS;
	}
#  endif
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ net_ತುಂಬು_certificates(listener);
}


#ಆದರೆ !defined(WIN32) && !defined(_AIX)


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ net__bind_interface(ರಚನೆ mosquitto__listener *listener, ರಚನೆ addrinfo *rp)
{
	/*
	 * This binds the listener sock to a network interface.
	 * The use of SO_BINDTODEVICE requires root access, which we don't have, so instead
	 * use getifaddrs to find the interface addresses, and use IP of the
	 * matching interface in the later bind().
	 */
	ರಚನೆ ifaddrs *ifaddr;
	bool have_interface = false;

	ಆದರೆ(getifaddrs(&ifaddr) < 0){
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		ಹಿಂತಿರುಗಿ MOSQ_ERR_ERRNO;
	}

	ಗಾಗಿ(ರಚನೆ ifaddrs *ifa=ifaddr; ifa!=ಖಾಲಿ; ifa=ifa->ifa_ಮುಂದಿನ){
		ಆದರೆ(ifa->ifa_addr == ಖಾಲಿ){
			ಮುಂದುವರಿ;
		}

		ಆದರೆ(!strcasecmp(listener->bind_interface, ifa->ifa_name)){
			have_interface = true;

			ಆದರೆ(ifa->ifa_addr->sa_family == rp->ai_addr->sa_family){
				ಆದರೆ(rp->ai_addr->sa_family == AF_INET){
					ಆದರೆ(listener->host &&
							memcmp(&((ರಚನೆ sockaddr_in *)rp->ai_addr)->sin_addr,
							&((ರಚನೆ sockaddr_in *)ifa->ifa_addr)->sin_addr,
							ಗಾತ್ರ(ರಚನೆ in_addr))){

						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}ಇಲ್ಲದಿದ್ದರೆ{
						ಸ್ಮರಣೆ_ನಕಲು(&((ರಚನೆ sockaddr_in *)rp->ai_addr)->sin_addr,
								&((ರಚನೆ sockaddr_in *)ifa->ifa_addr)->sin_addr,
								ಗಾತ್ರ(ರಚನೆ in_addr));

						freeifaddrs(ifaddr);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
					}
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rp->ai_addr->sa_family == AF_INET6){
					ಆದರೆ(listener->host &&
							memcmp(&((ರಚನೆ sockaddr_in6 *)rp->ai_addr)->sin6_addr,
							&((ರಚನೆ sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
							ಗಾತ್ರ(ರಚನೆ in6_addr))){

						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}ಇಲ್ಲದಿದ್ದರೆ{
						ಸ್ಮರಣೆ_ನಕಲು(&((ರಚನೆ sockaddr_in6 *)rp->ai_addr)->sin6_addr,
								&((ರಚನೆ sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
								ಗಾತ್ರ(ರಚನೆ in6_addr));

						((ರಚನೆ sockaddr_in6 *)rp->ai_addr)->sin6_scope_id = ((ರಚನೆ sockaddr_in6 *)ifa->ifa_addr)->sin6_scope_id;

						freeifaddrs(ifaddr);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
					}
				}
			}
		}
	}
	freeifaddrs(ifaddr);
	ಆದರೆ(have_interface){
		log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Interface %s does not support %s configuration.",
				listener->bind_interface, rp->ai_addr->sa_family == AF_INET ? "IPv4" : "IPv6");
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_SUPPORTED;
	}ಇಲ್ಲದಿದ್ದರೆ{
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Interface %s does not exist.", listener->bind_interface);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_ಸಿಕ್ಕಿತು;
	}
}
#ಆದರೆ_ಅಂತ್ಯ


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ net__socket_listen_tcp(ರಚನೆ mosquitto__listener *listener)
{
	mosq_sock_t sock = INVALID_SOCKET;
	ರಚನೆ addrinfo hints;
	ರಚನೆ addrinfo *ainfo, *rp;
	ಅಕ್ಷರ service[10];
	ಪೂರ್ಣಾಂಕ rc;
	ಪೂರ್ಣಾಂಕ ss_opt = 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
	bool interface_bound = false;
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ(!listener){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	snprintf(service, 10, "%d", listener->ಪೋರ್ಟ್);
	memset(&hints, 0, ಗಾತ್ರ(ರಚನೆ addrinfo));
	ಆದರೆ(listener->socket_domain){
		hints.ai_family = listener->socket_domain;
	}ಇಲ್ಲದಿದ್ದರೆ{
		hints.ai_family = AF_UNSPEC;
	}
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	rc = getaddrinfo(listener->host, service, &hints, &ainfo);
	ಆದರೆ(rc){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error creating listener: %s.", gai_strerror(rc));
		ಹಿಂತಿರುಗಿ INVALID_SOCKET;
	}

	listener->sock_ಎಣಿಕೆ = 0;
	listener->socks = ಖಾಲಿ;

	ಗಾಗಿ(rp = ainfo; rp; rp = rp->ai_ಮುಂದಿನ){
		ಆದರೆ(rp->ai_family == AF_INET){
			log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Opening ipv4 listen socket on port %d.", ntohs(((ರಚನೆ sockaddr_in *)rp->ai_addr)->sin_port));
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rp->ai_family == AF_INET6){
			log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Opening ipv6 listen socket on port %d.", ntohs(((ರಚನೆ sockaddr_in6 *)rp->ai_addr)->sin6_port));
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಮುಂದುವರಿ;
		}

		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		ಆದರೆ(sock == INVALID_SOCKET){
			net__print_error(MOSQ_LOG_WARNING, "Warning: %s");
			ಮುಂದುವರಿ;
		}
		listener->sock_ಎಣಿಕೆ++;
		listener->socks = mosquitto_realloc(listener->socks, ಗಾತ್ರ(mosq_sock_t)*(size_t)listener->sock_ಎಣಿಕೆ);
		ಆದರೆ(!listener->socks){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}
		listener->socks[listener->sock_ಎಣಿಕೆ-1] = sock;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
		ss_opt = 1;
		/* Unimportant if this fails */
		(ಶೂನ್ಯ)setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)&ss_opt, ಗಾತ್ರ(ss_opt));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ IPV6_V6ONLY
		ss_opt = 1;
		(ಶೂನ್ಯ)setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)&ss_opt, ಗಾತ್ರ(ss_opt));
#ಆದರೆ_ಅಂತ್ಯ

		ಆದರೆ(net__socket_nonblock(&sock)){
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			ಹಿಂತಿರುಗಿ 1;
		}

#ಆದರೆ !defined(WIN32) && !defined(_AIX)
		ಆದರೆ(listener->bind_interface){
			/* It might be possible that an interface does not support all relevant sa_families.
			 * We should successfully find at least one. */
			rc = net__bind_interface(listener, rp);
			ಆದರೆ(rc){
				COMPAT_CLOSE(sock);
				listener->sock_ಎಣಿಕೆ--;
				ಆದರೆ(rc == MOSQ_ERR_NOT_ಸಿಕ್ಕಿತು || rc == MOSQ_ERR_INVAL){
					freeaddrinfo(ainfo);
					ಹಿಂತಿರುಗಿ rc;
				}ಇಲ್ಲದಿದ್ದರೆ{
					ಮುಂದುವರಿ;
				}
			}
			interface_bound = true;
		}
#ಆದರೆ_ಅಂತ್ಯ

		ಆದರೆ(bind(sock, rp->ai_addr, rp->ai_addrlen) == -1){
#ಆದರೆ defined(__linux__)
			ಆದರೆ(errno == EACCES){
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "If you are trying to bind to a privileged port (<1024), try using setcap and do not start the broker as root:");
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "    sudo setcap 'CAP_NET_BIND_SERVICE=+ep /usr/sbin/mosquitto'");
			}
#ಆದರೆ_ಅಂತ್ಯ
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			COMPAT_CLOSE(sock);
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			ಹಿಂತಿರುಗಿ 1;
		}

		ಆದರೆ(listen(sock, 100) == -1){
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			mosquitto_FREE(listener->socks);
			ಹಿಂತಿರುಗಿ 1;
		}
	}
	freeaddrinfo(ainfo);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
	ಆದರೆ(listener->bind_interface && !interface_bound){
		mosquitto_FREE(listener->socks);
		ಹಿಂತಿರುಗಿ 1;
	}
#ಆದರೆ_ಅಂತ್ಯ

	ಹಿಂತಿರುಗಿ 0;
}


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_UNIX_SOCKETS


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ net__socket_listen_unix(ರಚನೆ mosquitto__listener *listener)
{
	ರಚನೆ sockaddr_un addr;
	mosq_sock_t sock = INVALID_SOCKET;
	ಪೂರ್ಣಾಂಕ rc;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
	mode_t old_ಮುಖವಾಡ;
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ(listener->unix_socket_ಮಾರ್ಗ == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}
	ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(listener->unix_socket_ಮಾರ್ಗ) > ಗಾತ್ರ(addr.sun_ಮಾರ್ಗ)-1){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Path to unix socket is too long \"%s\".", listener->unix_socket_ಮಾರ್ಗ);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
	DeleteFile(listener->unix_socket_ಮಾರ್ಗ);
#ಇಲ್ಲದಿದ್ದರೆ
	unlink(listener->unix_socket_ಮಾರ್ಗ);
#ಆದರೆ_ಅಂತ್ಯ
	log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Opening unix listen socket on path %s.", listener->unix_socket_ಮಾರ್ಗ);
	memset(&addr, 0, ಗಾತ್ರ(ರಚನೆ sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_ಮಾರ್ಗ, listener->unix_socket_ಮಾರ್ಗ, ಗಾತ್ರ(addr.sun_ಮಾರ್ಗ)-1);

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	ಆದರೆ(sock == INVALID_SOCKET){
		net__print_error(MOSQ_LOG_ERR, "Error creating unix socket: %s");
		ಹಿಂತಿರುಗಿ 1;
	}
	listener->sock_ಎಣಿಕೆ++;
	listener->socks = mosquitto_realloc(listener->socks, ಗಾತ್ರ(mosq_sock_t)*(size_t)listener->sock_ಎಣಿಕೆ);
	ಆದರೆ(!listener->socks){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
		COMPAT_CLOSE(sock);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}
	listener->socks[listener->sock_ಎಣಿಕೆ-1] = sock;


#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
	old_ಮುಖವಾಡ = umask(0007);
#ಆದರೆ_ಅಂತ್ಯ
	rc = bind(sock, (ರಚನೆ sockaddr *)&addr, ಗಾತ್ರ(ರಚನೆ sockaddr_un));
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
	umask(old_ಮುಖವಾಡ);
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ(rc == -1){
		net__print_error(MOSQ_LOG_ERR, "Error binding unix socket: %s");
		ಹಿಂತಿರುಗಿ 1;
	}

	ಆದರೆ(listen(sock, 10) == -1){
		net__print_error(MOSQ_LOG_ERR, "Error listening to unix socket: %s");
		ಹಿಂತಿರುಗಿ 1;
	}

	ಆದರೆ(net__socket_nonblock(&sock)){
		ಹಿಂತಿರುಗಿ 1;
	}

	ಹಿಂತಿರುಗಿ 0;
}
#ಆದರೆ_ಅಂತ್ಯ


/* Creates a socket and listens on port 'port'.
 * Returns 1 on failure
 * Returns 0 on success.
 */
ಪೂರ್ಣಾಂಕ net__socket_listen(ರಚನೆ mosquitto__listener *listener)
{
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ(!listener){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_UNIX_SOCKETS
	ಆದರೆ(listener->ಪೋರ್ಟ್ == 0 && listener->unix_socket_ಮಾರ್ಗ != ಖಾಲಿ){
		rc = net__socket_listen_unix(listener);
	}ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	{
		rc = net__socket_listen_tcp(listener);
	}
	ಆದರೆ(rc){
		ಹಿಂತಿರುಗಿ rc;
	}

	/* We need to have at least one working socket. */
	ಆದರೆ(listener->sock_ಎಣಿಕೆ > 0){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
		ಆದರೆ(listener->certfile && listener->keyfile){
			ಆದರೆ(net__tls_server_ctx(listener)){
				ಹಿಂತಿರುಗಿ 1;
			}

			ಆದರೆ(net_tls_ತುಂಬು_verify(listener)){
				ಹಿಂತಿರುಗಿ 1;
			}
		}
#  ifdef FINAL_WITH_TLS_PSK
		ಆದರೆ(listener->psk_hint){
			ಆದರೆ(listener->certfile == ಖಾಲಿ || listener->keyfile == ಖಾಲಿ){
				ಆದರೆ(net__tls_server_ctx(listener)){
					ಹಿಂತಿರುಗಿ 1;
				}
			}
			SSL_CTX_ಗಣ_psk_server_callback(listener->ssl_ctx, psk_server_callback);
			ಆದರೆ(listener->psk_hint){
				rc = SSL_CTX_use_psk_ತಟಸ್ಥ_hint(listener->ssl_ctx, listener->psk_hint);
				ಆದರೆ(rc == 0){
					log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to set TLS PSK hint.");
					net__print_ssl_error(ಖಾಲಿ, ಖಾಲಿ);
					ಹಿಂತಿರುಗಿ 1;
				}
			}
		}
#  endif /* FINAL_WITH_TLS_PSK */
		ಆದರೆ(tls_ex_ಸೂಚ್ಯಂಕ_context == -1){
			tls_ex_ಸೂಚ್ಯಂಕ_context = SSL_get_ex_new_ಸೂಚ್ಯಂಕ(0, "client context", ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
		}
		ಆದರೆ(tls_ex_ಸೂಚ್ಯಂಕ_listener == -1){
			tls_ex_ಸೂಚ್ಯಂಕ_listener = SSL_get_ex_new_ಸೂಚ್ಯಂಕ(0, "listener", ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
		}
#ಆದರೆ_ಅಂತ್ಯ /* WITH_TLS */
		ಹಿಂತಿರುಗಿ 0;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ 1;
	}
}


ಪೂರ್ಣಾಂಕ net_socket_get_ವಿಳಾಸ(mosq_sock_t sock, ಅಕ್ಷರ *buf, size_t len, uint16_t *remote_ಪೋರ್ಟ್)
{
	ರಚನೆ sockaddr_storage addr;
	socklen_t addrlen;

	memset(&addr, 0, ಗಾತ್ರ(ರಚನೆ sockaddr_storage));
	addrlen = ಗಾತ್ರ(addr);
	ಆದರೆ(!getpeername(sock, (ರಚನೆ sockaddr *)&addr, &addrlen)){
		ಆದರೆ(addr.ss_family == AF_INET){
			ಆದರೆ(remote_ಪೋರ್ಟ್){
				*remote_ಪೋರ್ಟ್ = ntohs(((ರಚನೆ sockaddr_in *)&addr)->sin_port);
			}
			ಆದರೆ(inet_ntop(AF_INET, &((ರಚನೆ sockaddr_in *)&addr)->sin_addr.s_addr, buf, (socklen_t)len)){
				ಹಿಂತಿರುಗಿ 0;
			}
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(addr.ss_family == AF_INET6){
			ಆದರೆ(remote_ಪೋರ್ಟ್){
				*remote_ಪೋರ್ಟ್ = ntohs(((ರಚನೆ sockaddr_in6 *)&addr)->sin6_port);
			}
			ಆದರೆ(inet_ntop(AF_INET6, &((ರಚನೆ sockaddr_in6 *)&addr)->sin6_addr.s6_addr, buf, (socklen_t)len)){
				ಹಿಂತಿರುಗಿ 0;
			}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_UNIX_SOCKETS
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(addr.ss_family == AF_UNIX){
			ರಚನೆ sockaddr_un un;
			addrlen = ಗಾತ್ರ(ರಚನೆ sockaddr_un);
			ಆದರೆ(!getsockname(sock, (ರಚನೆ sockaddr *)&un, &addrlen)){
				snprintf(buf, len, "%s", un.sun_ಮಾರ್ಗ);
			}ಇಲ್ಲದಿದ್ದರೆ{
				snprintf(buf, len, "unix-socket");
			}
			ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ_ಅಂತ್ಯ
		}
	}
	ಹಿಂತಿರುಗಿ 1;
}
