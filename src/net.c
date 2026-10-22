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

#ifndef WIN32
#  include <arpa/inet.h>
#  ifndef _AIX
#    include <ifaddrs.h>
#  endif
#  include <netdb.h>
#  include <netinet/tcp.h>
#  include <strings.h>
#  include <sys/socket.h>
#  include <unistd.h>
#ಅನ್ಯಥಾ
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ifdef WITH_WRAP
#  include <tcpd.h>
#endif

#ifdef HAVE_NETINET_IN_H
#  include <netinet/in.h>
#endif

#ಯದಿ defined(WITH_UNIX_SOCKETS) || defined(WITH_TLS)
#  include "sys/stat.h"
#endif

#ifdef WITH_UNIX_SOCKETS
#  ifdef WIN32
#    include <afunix.h>
#  ಅನ್ಯಥಾ
#    include <sys/un.h>
#  endif
#endif

#ifdef __QNX__
#  include <net/netbyte.h>
#endif

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "net_mosq.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"

#ifdef WITH_TLS
#  include "tls_mosq.h"
#  include <openssl/err.h>
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ tls_ex_index_context = -1;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ tls_ex_index_listener = -1;
#endif

#ಅನ್ತರ್ಭಾವ "sys_tree.h"

/* For EMFILE handling */
ಸ್ಥಿರ mosq_sock_t spare_sock = INVALID_SOCKET;


ಶೂನ್ಯ net__broker_init(ಶೂನ್ಯ)
{
	spare_sock = socket(AF_INET, SOCK_STREAM, 0);
	net__init();
#ifdef WITH_TLS
	net__init_tls();
#endif
}


ಶೂನ್ಯ net__broker_cleanup(ಶೂನ್ಯ)
{
	ಯದಿ(spare_sock != INVALID_SOCKET){
		COMPAT_CLOSE(spare_sock);
		spare_sock = INVALID_SOCKET;
	}
	net__cleanup();
}


ಸ್ಥಿರ ಶೂನ್ಯ net__print_error(unsigned ಪೂರ್ಣಾಙ್ಕ log, ಧ್ರುವ ವರ್ಣ *format_str)
{
	ವರ್ಣ *buf;

#ifdef WIN32
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			ರಿಕ್ತ, WSAGetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, ರಿಕ್ತ);

	log__printf(ರಿಕ್ತ, log, format_str, buf);
	LocalFree(buf);
#ಅನ್ಯಥಾ
	buf = strerror(errno);
	log__printf(ರಿಕ್ತ, log, format_str, buf);
#endif
}


ಸಂರಚನಾ mosquitto *net__socket_accept(ಸಂರಚನಾ mosquitto__listener_sock *listensock)
{
	mosq_sock_t new_sock = INVALID_SOCKET;
	ಸಂರಚನಾ mosquitto *new_context;
#ifdef WITH_TLS
	BIO *bio;
#endif
#ifdef WITH_WRAP
	ಸಂರಚನಾ request_info wrap_req;
	ವರ್ಣ address[1024];
#endif

	new_sock = accept(listensock->sock, ರಿಕ್ತ, 0);
	ಯದಿ(new_sock == INVALID_SOCKET){
#ifdef WIN32
		WINDOWS_SET_ERRNO();
		ಯದಿ(errno == WSAEMFILE){
#ಅನ್ಯಥಾ
		ಯದಿ(errno == EMFILE || errno == ENFILE){
#endif
			/* Close the spare socket, which means we should be able to accept
			 * this connection. Accept it, then close it immediately and create
			 * a new spare_sock. This prevents the situation of ever properly
			 * running out of sockets.
			 * It would be nice to send a "server not available" connack here,
			 * but there are lots of reasons why this would be tricky (TLS
			 * being the big one). */
			COMPAT_CLOSE(spare_sock);
			new_sock = accept(listensock->sock, ರಿಕ್ತ, 0);
			ಯದಿ(new_sock != INVALID_SOCKET){
				COMPAT_CLOSE(new_sock);
			}
			spare_sock = socket(AF_INET, SOCK_STREAM, 0);
			log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING,
					"Unable to accept new connection, system socket count has been exceeded. Try increasing \"ulimit -n\" or equivalent.");
		}
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	metrics__int_inc(mosq_counter_socket_connections, 1);

	ಯದಿ(net__socket_nonblock(&new_sock)){
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

#ifdef WITH_WRAP
	/* Use tcpd / libwrap to determine whether a connection is allowed. */
	request_init(&wrap_req, RQ_FILE, new_sock, RQ_DAEMON, "mosquitto", 0);
	fromhost(&wrap_req);
	ಯದಿ(!hosts_access(&wrap_req)){
		/* Access is denied */
		ಯದಿ(db.config->connection_messages == true){
			ಯದಿ(!net__socket_get_address(new_sock, address, 1024, ರಿಕ್ತ)){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client connection from %s denied access by tcpd.", address);
			}
		}
		COMPAT_CLOSE(new_sock);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
#endif

	ಯದಿ(db.config->set_tcp_nodelay && listensock->listener->port){
		ಪೂರ್ಣಾಙ್ಕ flag = 1;
		ಯದಿ(setsockopt(new_sock, IPPROTO_TCP, TCP_NODELAY, (ಧ್ರುವ ಶೂನ್ಯ *)&flag, ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ)) != 0){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Unable to set TCP_NODELAY.");
		}
	}

	new_context = context__init();
	ಯದಿ(!new_context){
		COMPAT_CLOSE(new_sock);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	new_context->listener = listensock->listener;
	ಯದಿ(!new_context->listener){
		context__cleanup(new_context, true);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	new_context->listener->client_ಎಣಿಕೆ++;

	ಯದಿ(new_context->listener->enable_proxy_protocol){
		ಯದಿ(context__init_sock(new_context, new_sock, false) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		ಯದಿ(new_context->listener->enable_proxy_protocol == 2){
			new_context->transport = mosq_t_proxy_v2;
		}ಅನ್ಯಥಾ{
			new_context->transport = mosq_t_proxy_v1;
		}
		new_context->proxy.cmd = -1;
	}ಅನ್ಯಥಾ{
		ಯದಿ(context__init_sock(new_context, new_sock, true) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		ವಿಕಲ್ಪ(new_context->listener->protocol){
			ಪಕ್ಷ mp_mqtt:
				new_context->transport = mosq_t_tcp;
				ಭಙ್ಗ;
#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			ಪಕ್ಷ mp_websockets:
				ಯದಿ(http__context_init(new_context)){
					context__cleanup(new_context, true);
					ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
				}
				ಭಙ್ಗ;
#endif
			ಸ್ವಭಾವ:
				context__cleanup(new_context, true);
				ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
	}

	ಯದಿ((new_context->listener->max_connections > 0 && new_context->listener->client_ಎಣಿಕೆ > new_context->listener->max_connections)
			|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (unsigned ಪೂರ್ಣಾಙ್ಕ)db.config->global_max_connections)){

		ಯದಿ(db.config->connection_messages == true){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", new_context->address);
		}
		context__cleanup(new_context, true);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

#ifdef WITH_TLS
	/* TLS init */
	ಯದಿ(new_context->listener->ssl_ctx){
		new_context->ssl = SSL_new(new_context->listener->ssl_ctx);
		ಯದಿ(!new_context->ssl){
			context__cleanup(new_context, true);
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		ಯದಿ(!SSL_set_ex_data(new_context->ssl, tls_ex_index_context, new_context)
				|| !SSL_set_ex_data(new_context->ssl, tls_ex_index_listener, new_context->listener)){
			context__cleanup(new_context, true);
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		new_context->want_write = true;
		bio = BIO_new_socket(new_sock, BIO_NOCLOSE);
		SSL_set_bio(new_context->ssl, bio, bio);
		ERR_clear_error();
		SSL_set_accept_state(new_context->ssl);
	}
#endif

	ಯದಿ(db.config->connection_messages == true
			&& !new_context->listener->enable_proxy_protocol){

		log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "New connection from %s:%d on port %d.",
				new_context->address, new_context->remote_port, new_context->listener->port);
	}

	mux__new(new_context);
	keepalive__add(new_context);

	ಪ್ರತ್ಯಾಗಮ new_context;
}

#ifdef WITH_TLS


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ client_certificate_verify(ಪೂರ್ಣಾಙ್ಕ preverify_ok, X509_STORE_CTX *ctx)
{
	/* Preverify should check expiry, revocation. */
	ಯದಿ(preverify_ok == 0){
		SSL *ssl;
		ssl = X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx());
		ಯದಿ(ssl){
			ಸಂರಚನಾ mosquitto__listener *listener;
			listener = SSL_get_ex_data(ssl, tls_ex_index_listener);

			ಯದಿ(listener && listener->disable_client_cert_date_checks){
				ಪೂರ್ಣಾಙ್ಕ err = X509_STORE_CTX_get_error(ctx);
				ಯದಿ(err == X509_V_ERR_CERT_NOT_YET_VALID || err == X509_V_ERR_CERT_HAS_EXPIRED){
					preverify_ok = 1;
				}
			}
		}
	}
	ಪ್ರತ್ಯಾಗಮ preverify_ok;
}
#endif

#ifdef FINAL_WITH_TLS_PSK


ಸ್ಥಿರ unsigned ಪೂರ್ಣಾಙ್ಕ psk_server_callback(SSL *ssl, ಧ್ರುವ ವರ್ಣ *identity, unsigned ವರ್ಣ *psk, unsigned ಪೂರ್ಣಾಙ್ಕ max_psk_len)
{
	ಸಂರಚನಾ mosquitto *context;
	ಸಂರಚನಾ mosquitto__listener *listener;
	ವರ್ಣ *psk_ಕೀಲಿ = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ len;
	ಧ್ರುವ ವರ್ಣ *psk_hint;

	ಯದಿ(!identity){
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	context = SSL_get_ex_data(ssl, tls_ex_index_context);
	ಯದಿ(!context){
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	listener = SSL_get_ex_data(ssl, tls_ex_index_listener);
	ಯದಿ(!listener){
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	psk_hint = listener->psk_hint;

	/* The hex to BN conversion results in the length halving, so we can pass
	 * max_psk_len*2 as the max hex key here. */
	psk_ಕೀಲಿ = mosquitto_calloc(1, (size_t)max_psk_len*2 + 1);
	ಯದಿ(!psk_ಕೀಲಿ){
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	ಯದಿ(mosquitto_psk_ಕೀಲಿ_get(context, psk_hint, identity, psk_ಕೀಲಿ, (ಪೂರ್ಣಾಙ್ಕ)max_psk_len*2) != MOSQ_ERR_SUCCESS){
		mosquitto_FREE(psk_ಕೀಲಿ);
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	len = mosquitto__hex2bin(psk_ಕೀಲಿ, psk, (ಪೂರ್ಣಾಙ್ಕ)max_psk_len);
	ಯದಿ(len < 0){
		mosquitto_FREE(psk_ಕೀಲಿ);
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	ಯದಿ(listener->use_identity_as_username){
		ಯದಿ(mosquitto_validate_utf8(identity, (ಪೂರ್ಣಾಙ್ಕ)strlen(identity))){
			mosquitto_free(psk_ಕೀಲಿ);
			ಪ್ರತ್ಯಾಗಮ 0;
		}
		context->username = mosquitto_strdup(identity);
		ಯದಿ(!context->username){
			mosquitto_FREE(psk_ಕೀಲಿ);
			ಪ್ರತ್ಯಾಗಮ 0;
		}
	}

	mosquitto_FREE(psk_ಕೀಲಿ);
	ಪ್ರತ್ಯಾಗಮ (unsigned ಪೂರ್ಣಾಙ್ಕ)len;
}
#endif

#ifdef WITH_TLS


ಸ್ಥಿರ ಶೂನ್ಯ tls_keylog_callback(ಧ್ರುವ SSL *ssl, ಧ್ರುವ ವರ್ಣ *ಸಾಲು)
{
	UNUSED(ssl);

	ಯದಿ(db.tls_keylog){
		FILE *fptr;
		fptr = mosquitto_fopen(db.tls_keylog, "at", true);
		ಯದಿ(fptr){
#ifndef WIN32
			/* Until mosquitto_fopen enforces file permissions on all files,
			 * enforce it here. We can enforce it here, because it isn't a
			 * change of behaviour. */
			ಸಂರಚನಾ stat statbuf;
			ಯದಿ(fstat(fileno(fptr), &statbuf) < 0
					|| statbuf.st_mode & (S_IRWXG | S_IRWXO)){

				fclose(fptr);
				ಪ್ರತ್ಯಾಗಮ;
			}
#endif
			fprintf(fptr, "%s\n", ಸಾಲು);
			fclose(fptr);
		}ಅನ್ಯಥಾ{
#ifndef WIN32
			ಯದಿ(errno == ELOOP){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Error: keylog file must not be a symbolic link");
			}ಅನ್ಯಥಾ
#endif
			{
				log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Error: Unable to open keylog file: %s", strerror(errno));
			}
		}
	}
}


ಪೂರ್ಣಾಙ್ಕ net__tls_server_ctx(ಸಂರಚನಾ mosquitto__listener *listener)
{
	ವರ್ಣ buf[256];
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ(tls_ex_index_context == -1){
		tls_ex_index_context = SSL_get_ex_new_index(0, "client context", ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
	}
	ಯದಿ(tls_ex_index_listener == -1){
		tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
	}

	ಯದಿ(listener->ssl_ctx){
		SSL_CTX_free(listener->ssl_ctx);
		listener->ssl_ctx = ರಿಕ್ತ;
	}

	listener->ssl_ctx = SSL_CTX_new(TLS_server_method());

	ಯದಿ(!listener->ssl_ctx){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to create TLS context.");
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_TLS;
	}

#ifdef SSL_OP_NO_TLSv1_3
	ಯದಿ(db.config->per_listener_settings){
		ಯದಿ(listener->security_options->psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}ಅನ್ಯಥಾ{
		ಯದಿ(db.config->security_options.psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}
#endif

	ಯದಿ(listener->tls_version == ರಿಕ್ತ){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
#ifdef SSL_OP_NO_TLSv1_3
	}ಅನ್ಯಥಾ ಯದಿ(!strcmp(listener->tls_version, "tlsv1.3")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2);
#endif
	}ಅನ್ಯಥಾ ಯದಿ(!strcmp(listener->tls_version, "tlsv1.2")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
	}ಅನ್ಯಥಾ ಯದಿ(!strcmp(listener->tls_version, "tlsv1.1")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1);
		log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: TLS v1.1 is insecure. It must only be used in the case "
				"where you have devices that cannot be upgraded to use a secure version of TLS. It is "
				"recommended to use as secure a set of ciphers as possible and that will restrict it to "
				"TLS v1.1 only, and to use a separate listener using TLS v1.2 and secure ciphers for your "
				"other devices.");
		log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Please be aware that support for TLS v1.1 will go away eventually "
				"and that you should plan now to migrate away from it.");
	}ಅನ್ಯಥಾ{
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unsupported tls_version \"%s\".", listener->tls_version);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_TLS;
	}

#ifdef SSL_OP_NO_COMPRESSION
	/* Disable compression */
	SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_COMPRESSION);
#endif
#ifdef SSL_OP_CIPHER_SERVER_PREFERENCE
	/* Server chooses cipher */
	SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_CIPHER_SERVER_PREFERENCE);
#endif

#ifdef SSL_MODE_RELEASE_BUFFERS
	/* Use even less memory per SSL connection. */
	SSL_CTX_set_mode(listener->ssl_ctx, SSL_MODE_RELEASE_BUFFERS);
#endif

	SSL_CTX_set_dh_auto(listener->ssl_ctx, 1);

#ifdef SSL_OP_NO_RENEGOTIATION
	SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_RENEGOTIATION);
#endif

	ಯದಿ(db.tls_keylog){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "TLS key logging to '%s' enabled for all listeners.",
				db.tls_keylog);
		log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "TLS key logging is for DEBUGGING only.");

		SSL_CTX_set_keylog_callback(listener->ssl_ctx, tls_keylog_callback);
	}

	snprintf(buf, 256, "mosquitto-%d", listener->port);
	SSL_CTX_set_session_id_context(listener->ssl_ctx, (unsigned ವರ್ಣ *)buf, (unsigned ಪೂರ್ಣಾಙ್ಕ)strlen(buf));

	ಯದಿ(listener->ciphers){
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, listener->ciphers);
		ಯದಿ(rc == 0){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_TLS;
		}
	}ಅನ್ಯಥಾ{
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, "DEFAULT:!aNULL:!eNULL:!LOW:!EXPORT:!SSLv2:@STRENGTH");
		ಯದಿ(rc == 0){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_TLS;
		}
	}
	ಯದಿ(listener->ciphers_tls13){
		rc = SSL_CTX_set_ciphersuites(listener->ssl_ctx, listener->ciphers_tls13);
		ಯದಿ(rc == 0){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to set TLS 1.3 ciphersuites. Check cipher_tls13 list \"%s\".", listener->ciphers_tls13);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_TLS;
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}
#endif


#ifdef WITH_TLS


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ net__load_crl_file(ಸಂರಚನಾ mosquitto__listener *listener)
{
	X509_STORE *store;
	X509_LOOKUP *lookup;
	ಪೂರ್ಣಾಙ್ಕ rc;

	store = SSL_CTX_get_cert_store(listener->ssl_ctx);
	ಯದಿ(!store){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to obtain TLS store.");
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_TLS;
	}
	lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
	rc = X509_load_crl_file(lookup, listener->crlfile, X509_FILETYPE_PEM);
	ಯದಿ(rc < 1){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to load certificate revocation file \"%s\". Check crlfile.", listener->crlfile);
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		net__print_ssl_error(ರಿಕ್ತ, ರಿಕ್ತ);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_TLS;
	}
	X509_STORE_set_flags(store, X509_V_FLAG_CRL_CHECK);

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}
#endif


ಪೂರ್ಣಾಙ್ಕ net__load_certificates(ಸಂರಚನಾ mosquitto__listener *listener)
{
#ifdef WITH_TLS
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ(listener->require_certificate){
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, client_certificate_verify);
	}ಅನ್ಯಥಾ{
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_NONE, client_certificate_verify);
	}
	rc = SSL_CTX_use_certificate_chain_file(listener->ssl_ctx, listener->certfile);
	ಯದಿ(rc != 1){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
		net__print_ssl_error(ರಿಕ್ತ, ರಿಕ್ತ);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_TLS;
	}
	ಯದಿ(listener->tls_engine == ರಿಕ್ತ || listener->tls_keyform == mosq_k_pem){
		rc = SSL_CTX_use_Private_ಕೀಲಿ_file(listener->ssl_ctx, listener->keyfile, SSL_FILETYPE_PEM);
		ಯದಿ(rc != 1){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			net__print_ssl_error(ರಿಕ್ತ, ರಿಕ್ತ);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_TLS;
		}
	}
	rc = SSL_CTX_check_private_ಕೀಲಿ(listener->ssl_ctx);
	ಯದಿ(rc != 1){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Server certificate/key are inconsistent.");
		net__print_ssl_error(ರಿಕ್ತ, ರಿಕ್ತ);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_TLS;
	}
	/* Load CRLs if they exist. */
	ಯದಿ(listener->crlfile){
		rc = net__load_crl_file(listener);
		ಯದಿ(rc){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}
#ಅನ್ಯಥಾ
	UNUSED(listener);
#endif
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


#ಯದಿ defined(WITH_TLS) && !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ net__load_engine(ಸಂರಚನಾ mosquitto__listener *listener)
{
	ENGINE *engine = ರಿಕ್ತ;
	UI_METHOD *ui_method;
	EVP_PKEY *pkey;

	ಯದಿ(!listener->tls_engine){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}

	engine = ENGINE_by_id(listener->tls_engine);
	ಯದಿ(!engine){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error loading %s engine\n", listener->tls_engine);
		net__print_ssl_error(ರಿಕ್ತ, ರಿಕ್ತ);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_TLS;
	}
	ಯದಿ(!ENGINE_init(engine)){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Failed engine initialisation\n");
		net__print_ssl_error(ರಿಕ್ತ, ರಿಕ್ತ);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_TLS;
	}
	ENGINE_set_default(engine, ENGINE_METHOD_ALL);

	ಯದಿ(listener->tls_keyform == mosq_k_engine){
		ui_method = net__get_ui_method();
		ಯದಿ(listener->tls_engine_kpass_sha1){
			ಯದಿ(!ENGINE_ctrl_cmd(engine, ENGINE_SECRET_MODE, ENGINE_SECRET_MODE_SHA, ರಿಕ್ತ, ರಿಕ್ತ, 0)){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to set engine secret mode sha");
				net__print_ssl_error(ರಿಕ್ತ, ರಿಕ್ತ);
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_TLS;
			}
			ಯದಿ(!ENGINE_ctrl_cmd(engine, ENGINE_PIN, 0, listener->tls_engine_kpass_sha1, ರಿಕ್ತ, 0)){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to set engine pin");
				net__print_ssl_error(ರಿಕ್ತ, ರಿಕ್ತ);
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_TLS;
			}
			ui_method = ರಿಕ್ತ;
		}
		pkey = ENGINE_load_private_ಕೀಲಿ(engine, listener->keyfile, ui_method, ರಿಕ್ತ);
		ಯದಿ(!pkey){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to load engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(ರಿಕ್ತ, ರಿಕ್ತ);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_TLS;
		}
		ಯದಿ(SSL_CTX_use_Private_ಕೀಲಿ(listener->ssl_ctx, pkey) <= 0){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to use engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(ರಿಕ್ತ, ರಿಕ್ತ);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_TLS;
		}
	}
	ENGINE_free(engine); /* release the structural reference from ENGINE_by_id() */

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}
#endif


ಪೂರ್ಣಾಙ್ಕ net__tls_load_verify(ಸಂರಚನಾ mosquitto__listener *listener)
{
#ifdef WITH_TLS
	ಪೂರ್ಣಾಙ್ಕ rc;

#  ಯದಿ OPENSSL_VERSION_ಸಂಖ್ಯೆ < 0x30000000L
	ಯದಿ(listener->cafile || listener->capath){
		rc = SSL_CTX_load_verify_locations(listener->ssl_ctx, listener->cafile, listener->capath);
		ಯದಿ(rc == 0){
			ಯದಿ(listener->cafile && listener->capath){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\" and capath \"%s\".", listener->cafile, listener->capath);
			}ಅನ್ಯಥಾ ಯದಿ(listener->cafile){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			}ಅನ್ಯಥಾ{
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			}
		}
	}
#  ಅನ್ಯಥಾ
	ರಾಶಿ_OF(X509_NAME) *ca_names = ರಿಕ್ತ;
	ಯದಿ(listener->cafile || listener->capath){
		ca_names = sk_X509_NAME_new_null();
	}

	ಯದಿ(listener->cafile){
		rc = SSL_CTX_load_verify_file(listener->ssl_ctx, listener->cafile);
		ಯದಿ(rc == 0){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			net__print_ssl_error(ರಿಕ್ತ, ರಿಕ್ತ);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_TLS;
		}

		ರಾಶಿ_OF(X509_NAME) *cert_names = SSL_load_client_CA_file(listener->cafile);
		ಯದಿ(cert_names){
			ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<sk_X509_NAME_ಅಂಕೆ(cert_names); i++){
				sk_X509_NAME_ತಳ್ಳು(ca_names, X509_NAME_dup(sk_X509_NAME_ಮೌಲ್ಯ(cert_names, i)));
			}
			sk_X509_NAME_ತೆಗೆ_free(cert_names, X509_NAME_free);
		}

	}
	ಯದಿ(listener->capath){
		rc = SSL_CTX_load_verify_dir(listener->ssl_ctx, listener->capath);
		ಯದಿ(rc == 0){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			net__print_ssl_error(ರಿಕ್ತ, ರಿಕ್ತ);
			sk_X509_NAME_ತೆಗೆ_free(ca_names, X509_NAME_free);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_TLS;
		}

		SSL_add_dir_cert_subjects_to_ರಾಶಿ(ca_names, listener->capath);
	}
	ಯದಿ(ca_names){
		SSL_CTX_set_client_CA_list(listener->ssl_ctx, ca_names);
	}
#  endif

#  ಯದಿ !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000
	ಯದಿ(net__load_engine(listener)){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_TLS;
	}
#  endif
#endif
	ಪ್ರತ್ಯಾಗಮ net__load_certificates(listener);
}


#ಯದಿ !defined(WIN32) && !defined(_AIX)


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ net__bind_interface(ಸಂರಚನಾ mosquitto__listener *listener, ಸಂರಚನಾ addrinfo *rp)
{
	/*
	 * This binds the listener sock to a network interface.
	 * The use of SO_BINDTODEVICE requires root access, which we don't have, so instead
	 * use getifaddrs to find the interface addresses, and use IP of the
	 * matching interface in the later bind().
	 */
	ಸಂರಚನಾ ifaddrs *ifaddr;
	bool have_interface = false;

	ಯದಿ(getifaddrs(&ifaddr) < 0){
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_ERRNO;
	}

	ಪ್ರತಿ(ಸಂರಚನಾ ifaddrs *ifa=ifaddr; ifa!=ರಿಕ್ತ; ifa=ifa->ifa_next){
		ಯದಿ(ifa->ifa_addr == ರಿಕ್ತ){
			ಅನುವರ್ತ;
		}

		ಯದಿ(!strcasecmp(listener->bind_interface, ifa->ifa_name)){
			have_interface = true;

			ಯದಿ(ifa->ifa_addr->sa_family == rp->ai_addr->sa_family){
				ಯದಿ(rp->ai_addr->sa_family == AF_INET){
					ಯದಿ(listener->host &&
							memcmp(&((ಸಂರಚನಾ sockaddr_in *)rp->ai_addr)->sin_addr,
							&((ಸಂರಚನಾ sockaddr_in *)ifa->ifa_addr)->sin_addr,
							ಪರಿಮಾಣ(ಸಂರಚನಾ in_addr))){

						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}ಅನ್ಯಥಾ{
						memcpy(&((ಸಂರಚನಾ sockaddr_in *)rp->ai_addr)->sin_addr,
								&((ಸಂರಚನಾ sockaddr_in *)ifa->ifa_addr)->sin_addr,
								ಪರಿಮಾಣ(ಸಂರಚನಾ in_addr));

						freeifaddrs(ifaddr);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
					}
				}ಅನ್ಯಥಾ ಯದಿ(rp->ai_addr->sa_family == AF_INET6){
					ಯದಿ(listener->host &&
							memcmp(&((ಸಂರಚನಾ sockaddr_in6 *)rp->ai_addr)->sin6_addr,
							&((ಸಂರಚನಾ sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
							ಪರಿಮಾಣ(ಸಂರಚನಾ in6_addr))){

						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}ಅನ್ಯಥಾ{
						memcpy(&((ಸಂರಚನಾ sockaddr_in6 *)rp->ai_addr)->sin6_addr,
								&((ಸಂರಚನಾ sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
								ಪರಿಮಾಣ(ಸಂರಚನಾ in6_addr));

						((ಸಂರಚನಾ sockaddr_in6 *)rp->ai_addr)->sin6_scope_id = ((ಸಂರಚನಾ sockaddr_in6 *)ifa->ifa_addr)->sin6_scope_id;

						freeifaddrs(ifaddr);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
					}
				}
			}
		}
	}
	freeifaddrs(ifaddr);
	ಯದಿ(have_interface){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Interface %s does not support %s configuration.",
				listener->bind_interface, rp->ai_addr->sa_family == AF_INET ? "IPv4" : "IPv6");
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_SUPPORTED;
	}ಅನ್ಯಥಾ{
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Interface %s does not exist.", listener->bind_interface);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_FOUND;
	}
}
#endif


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ net__socket_listen_tcp(ಸಂರಚನಾ mosquitto__listener *listener)
{
	mosq_sock_t sock = INVALID_SOCKET;
	ಸಂರಚನಾ addrinfo hints;
	ಸಂರಚನಾ addrinfo *ainfo, *rp;
	ವರ್ಣ service[10];
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಪೂರ್ಣಾಙ್ಕ ss_opt = 1;
#ifndef WIN32
	bool interface_bound = false;
#endif

	ಯದಿ(!listener){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	snprintf(service, 10, "%d", listener->port);
	memset(&hints, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ addrinfo));
	ಯದಿ(listener->socket_domain){
		hints.ai_family = listener->socket_domain;
	}ಅನ್ಯಥಾ{
		hints.ai_family = AF_UNSPEC;
	}
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	rc = getaddrinfo(listener->host, service, &hints, &ainfo);
	ಯದಿ(rc){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error creating listener: %s.", gai_strerror(rc));
		ಪ್ರತ್ಯಾಗಮ INVALID_SOCKET;
	}

	listener->sock_ಎಣಿಕೆ = 0;
	listener->socks = ರಿಕ್ತ;

	ಪ್ರತಿ(rp = ainfo; rp; rp = rp->ai_next){
		ಯದಿ(rp->ai_family == AF_INET){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Opening ipv4 listen socket on port %d.", ntohs(((ಸಂರಚನಾ sockaddr_in *)rp->ai_addr)->sin_port));
		}ಅನ್ಯಥಾ ಯದಿ(rp->ai_family == AF_INET6){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Opening ipv6 listen socket on port %d.", ntohs(((ಸಂರಚನಾ sockaddr_in6 *)rp->ai_addr)->sin6_port));
		}ಅನ್ಯಥಾ{
			ಅನುವರ್ತ;
		}

		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		ಯದಿ(sock == INVALID_SOCKET){
			net__print_error(MOSQ_LOG_WARNING, "Warning: %s");
			ಅನುವರ್ತ;
		}
		listener->sock_ಎಣಿಕೆ++;
		listener->socks = mosquitto_realloc(listener->socks, ಪರಿಮಾಣ(mosq_sock_t)*(size_t)listener->sock_ಎಣಿಕೆ);
		ಯದಿ(!listener->socks){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}
		listener->socks[listener->sock_ಎಣಿಕೆ-1] = sock;

#ifndef WIN32
		ss_opt = 1;
		/* Unimportant if this fails */
		(ಶೂನ್ಯ)setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (ಧ್ರುವ ಶೂನ್ಯ *)&ss_opt, ಪರಿಮಾಣ(ss_opt));
#endif
#ifdef IPV6_V6ONLY
		ss_opt = 1;
		(ಶೂನ್ಯ)setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (ಧ್ರುವ ಶೂನ್ಯ *)&ss_opt, ಪರಿಮಾಣ(ss_opt));
#endif

		ಯದಿ(net__socket_nonblock(&sock)){
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			ಪ್ರತ್ಯಾಗಮ 1;
		}

#ಯದಿ !defined(WIN32) && !defined(_AIX)
		ಯದಿ(listener->bind_interface){
			/* It might be possible that an interface does not support all relevant sa_families.
			 * We should successfully find at least one. */
			rc = net__bind_interface(listener, rp);
			ಯದಿ(rc){
				COMPAT_CLOSE(sock);
				listener->sock_ಎಣಿಕೆ--;
				ಯದಿ(rc == MOSQ_ERR_NOT_FOUND || rc == MOSQ_ERR_INVAL){
					freeaddrinfo(ainfo);
					ಪ್ರತ್ಯಾಗಮ rc;
				}ಅನ್ಯಥಾ{
					ಅನುವರ್ತ;
				}
			}
			interface_bound = true;
		}
#endif

		ಯದಿ(bind(sock, rp->ai_addr, rp->ai_addrlen) == -1){
#ಯದಿ defined(__linux__)
			ಯದಿ(errno == EACCES){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "If you are trying to bind to a privileged port (<1024), try using setcap and do not start the broker as root:");
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "    sudo setcap 'CAP_NET_BIND_SERVICE=+ep /usr/sbin/mosquitto'");
			}
#endif
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			COMPAT_CLOSE(sock);
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			ಪ್ರತ್ಯಾಗಮ 1;
		}

		ಯದಿ(listen(sock, 100) == -1){
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			mosquitto_FREE(listener->socks);
			ಪ್ರತ್ಯಾಗಮ 1;
		}
	}
	freeaddrinfo(ainfo);

#ifndef WIN32
	ಯದಿ(listener->bind_interface && !interface_bound){
		mosquitto_FREE(listener->socks);
		ಪ್ರತ್ಯಾಗಮ 1;
	}
#endif

	ಪ್ರತ್ಯಾಗಮ 0;
}


#ifdef WITH_UNIX_SOCKETS


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ net__socket_listen_unix(ಸಂರಚನಾ mosquitto__listener *listener)
{
	ಸಂರಚನಾ sockaddr_un addr;
	mosq_sock_t sock = INVALID_SOCKET;
	ಪೂರ್ಣಾಙ್ಕ rc;
#ifndef WIN32
	mode_t old_mask;
#endif

	ಯದಿ(listener->unix_socket_path == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}
	ಯದಿ(strlen(listener->unix_socket_path) > ಪರಿಮಾಣ(addr.sun_path)-1){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Path to unix socket is too long \"%s\".", listener->unix_socket_path);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

#ifdef WIN32
	DeleteFile(listener->unix_socket_path);
#ಅನ್ಯಥಾ
	unlink(listener->unix_socket_path);
#endif
	log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Opening unix listen socket on path %s.", listener->unix_socket_path);
	memset(&addr, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, listener->unix_socket_path, ಪರಿಮಾಣ(addr.sun_path)-1);

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	ಯದಿ(sock == INVALID_SOCKET){
		net__print_error(MOSQ_LOG_ERR, "Error creating unix socket: %s");
		ಪ್ರತ್ಯಾಗಮ 1;
	}
	listener->sock_ಎಣಿಕೆ++;
	listener->socks = mosquitto_realloc(listener->socks, ಪರಿಮಾಣ(mosq_sock_t)*(size_t)listener->sock_ಎಣಿಕೆ);
	ಯದಿ(!listener->socks){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
		COMPAT_CLOSE(sock);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}
	listener->socks[listener->sock_ಎಣಿಕೆ-1] = sock;


#ifndef WIN32
	old_mask = umask(0007);
#endif
	rc = bind(sock, (ಸಂರಚನಾ sockaddr *)&addr, ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_un));
#ifndef WIN32
	umask(old_mask);
#endif

	ಯದಿ(rc == -1){
		net__print_error(MOSQ_LOG_ERR, "Error binding unix socket: %s");
		ಪ್ರತ್ಯಾಗಮ 1;
	}

	ಯದಿ(listen(sock, 10) == -1){
		net__print_error(MOSQ_LOG_ERR, "Error listening to unix socket: %s");
		ಪ್ರತ್ಯಾಗಮ 1;
	}

	ಯದಿ(net__socket_nonblock(&sock)){
		ಪ್ರತ್ಯಾಗಮ 1;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}
#endif


/* Creates a socket and listens on port 'port'.
 * Returns 1 on failure
 * Returns 0 on success.
 */
ಪೂರ್ಣಾಙ್ಕ net__socket_listen(ಸಂರಚನಾ mosquitto__listener *listener)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ(!listener){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

#ifdef WITH_UNIX_SOCKETS
	ಯದಿ(listener->port == 0 && listener->unix_socket_path != ರಿಕ್ತ){
		rc = net__socket_listen_unix(listener);
	}ಅನ್ಯಥಾ
#endif
	{
		rc = net__socket_listen_tcp(listener);
	}
	ಯದಿ(rc){
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	/* We need to have at least one working socket. */
	ಯದಿ(listener->sock_ಎಣಿಕೆ > 0){
#ifdef WITH_TLS
		ಯದಿ(listener->certfile && listener->keyfile){
			ಯದಿ(net__tls_server_ctx(listener)){
				ಪ್ರತ್ಯಾಗಮ 1;
			}

			ಯದಿ(net__tls_load_verify(listener)){
				ಪ್ರತ್ಯಾಗಮ 1;
			}
		}
#  ifdef FINAL_WITH_TLS_PSK
		ಯದಿ(listener->psk_hint){
			ಯದಿ(listener->certfile == ರಿಕ್ತ || listener->keyfile == ರಿಕ್ತ){
				ಯದಿ(net__tls_server_ctx(listener)){
					ಪ್ರತ್ಯಾಗಮ 1;
				}
			}
			SSL_CTX_set_psk_server_callback(listener->ssl_ctx, psk_server_callback);
			ಯದಿ(listener->psk_hint){
				rc = SSL_CTX_use_psk_identity_hint(listener->ssl_ctx, listener->psk_hint);
				ಯದಿ(rc == 0){
					log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to set TLS PSK hint.");
					net__print_ssl_error(ರಿಕ್ತ, ರಿಕ್ತ);
					ಪ್ರತ್ಯಾಗಮ 1;
				}
			}
		}
#  endif /* FINAL_WITH_TLS_PSK */
		ಯದಿ(tls_ex_index_context == -1){
			tls_ex_index_context = SSL_get_ex_new_index(0, "client context", ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
		}
		ಯದಿ(tls_ex_index_listener == -1){
			tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
		}
#endif /* WITH_TLS */
		ಪ್ರತ್ಯಾಗಮ 0;
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ 1;
	}
}


ಪೂರ್ಣಾಙ್ಕ net__socket_get_address(mosq_sock_t sock, ವರ್ಣ *buf, size_t len, uint16_t *remote_port)
{
	ಸಂರಚನಾ sockaddr_storage addr;
	socklen_t addrlen;

	memset(&addr, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_storage));
	addrlen = ಪರಿಮಾಣ(addr);
	ಯದಿ(!getpeername(sock, (ಸಂರಚನಾ sockaddr *)&addr, &addrlen)){
		ಯದಿ(addr.ss_family == AF_INET){
			ಯದಿ(remote_port){
				*remote_port = ntohs(((ಸಂರಚನಾ sockaddr_in *)&addr)->sin_port);
			}
			ಯದಿ(inet_ntop(AF_INET, &((ಸಂರಚನಾ sockaddr_in *)&addr)->sin_addr.s_addr, buf, (socklen_t)len)){
				ಪ್ರತ್ಯಾಗಮ 0;
			}
		}ಅನ್ಯಥಾ ಯದಿ(addr.ss_family == AF_INET6){
			ಯದಿ(remote_port){
				*remote_port = ntohs(((ಸಂರಚನಾ sockaddr_in6 *)&addr)->sin6_port);
			}
			ಯದಿ(inet_ntop(AF_INET6, &((ಸಂರಚನಾ sockaddr_in6 *)&addr)->sin6_addr.s6_addr, buf, (socklen_t)len)){
				ಪ್ರತ್ಯಾಗಮ 0;
			}
#ifdef WITH_UNIX_SOCKETS
		}ಅನ್ಯಥಾ ಯದಿ(addr.ss_family == AF_UNIX){
			ಸಂರಚನಾ sockaddr_un un;
			addrlen = ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_un);
			ಯದಿ(!getsockname(sock, (ಸಂರಚನಾ sockaddr *)&un, &addrlen)){
				snprintf(buf, len, "%s", un.sun_path);
			}ಅನ್ಯಥಾ{
				snprintf(buf, len, "unix-socket");
			}
			ಪ್ರತ್ಯಾಗಮ 0;
#endif
		}
	}
	ಪ್ರತ್ಯಾಗಮ 1;
}
