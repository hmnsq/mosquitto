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

#അന്തര്ഭാവ "config.h"

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
#അല്ലെങ്കിൽ
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#അന്തര്ഭാവ <errno.h>
#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>
#ifdef WITH_WRAP
#  include <tcpd.h>
#endif

#ifdef HAVE_NETINET_IN_H
#  include <netinet/in.h>
#endif

#എങ്കിൽ defined(WITH_UNIX_SOCKETS) || defined(WITH_TLS)
#  include "sys/stat.h"
#endif

#ifdef WITH_UNIX_SOCKETS
#  ifdef WIN32
#    include <afunix.h>
#  അല്ലെങ്കിൽ
#    include <sys/un.h>
#  endif
#endif

#ifdef __QNX__
#  include <net/netbyte.h>
#endif

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/mqtt_protocol.h"
#അന്തര്ഭാവ "net_mosq.h"
#അന്തര്ഭാവ "util_mosq.h"

#ifdef WITH_TLS
#  include "tls_mosq.h"
#  include <openssl/err.h>
സ്ഥിതം പൂർണ്ണസംഖ്യ tls_ex_index_context = -1;
സ്ഥിതം പൂർണ്ണസംഖ്യ tls_ex_index_listener = -1;
#endif

#അന്തര്ഭാവ "sys_tree.h"

/* For EMFILE handling */
സ്ഥിതം mosq_sock_t spare_sock = INVALID_SOCKET;


ശൂന്യം net__broker_init(ശൂന്യം)
{
	spare_sock = socket(AF_INET, SOCK_STREAM, 0);
	net__init();
#ifdef WITH_TLS
	net__init_tls();
#endif
}


ശൂന്യം net__broker_cleanup(ശൂന്യം)
{
	എങ്കിൽ(spare_sock != INVALID_SOCKET){
		COMPAT_CLOSE(spare_sock);
		spare_sock = INVALID_SOCKET;
	}
	net__cleanup();
}


സ്ഥിതം ശൂന്യം net__print_error(unsigned പൂർണ്ണസംഖ്യ log, സ്ഥിരം അക്ഷരം *format_str)
{
	അക്ഷരം *buf;

#ifdef WIN32
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			ശൂന്യ, WSAGetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, ശൂന്യ);

	log__printf(ശൂന്യ, log, format_str, buf);
	LocalFree(buf);
#അല്ലെങ്കിൽ
	buf = strerror(errno);
	log__printf(ശൂന്യ, log, format_str, buf);
#endif
}


ഘടന mosquitto *net__socket_accept(ഘടന mosquitto__listener_sock *listensock)
{
	mosq_sock_t new_sock = INVALID_SOCKET;
	ഘടന mosquitto *new_context;
#ifdef WITH_TLS
	BIO *bio;
#endif
#ifdef WITH_WRAP
	ഘടന request_info wrap_req;
	അക്ഷരം address[1024];
#endif

	new_sock = accept(listensock->sock, ശൂന്യ, 0);
	എങ്കിൽ(new_sock == INVALID_SOCKET){
#ifdef WIN32
		WINDOWS_SET_ERRNO();
		എങ്കിൽ(errno == WSAEMFILE){
#അല്ലെങ്കിൽ
		എങ്കിൽ(errno == EMFILE || errno == ENFILE){
#endif
			/* Close the spare socket, which means we should be able to accept
			 * this connection. Accept it, then close it immediately and create
			 * a new spare_sock. This prevents the situation of ever properly
			 * running out of sockets.
			 * It would be nice to send a "server not available" connack here,
			 * but there are lots of reasons why this would be tricky (TLS
			 * being the big one). */
			COMPAT_CLOSE(spare_sock);
			new_sock = accept(listensock->sock, ശൂന്യ, 0);
			എങ്കിൽ(new_sock != INVALID_SOCKET){
				COMPAT_CLOSE(new_sock);
			}
			spare_sock = socket(AF_INET, SOCK_STREAM, 0);
			log__printf(ശൂന്യ, MOSQ_LOG_WARNING,
					"Unable to accept new connection, system socket count has been exceeded. Try increasing \"ulimit -n\" or equivalent.");
		}
		തിരികെ ശൂന്യ;
	}

	metrics__int_inc(mosq_counter_socket_connections, 1);

	എങ്കിൽ(net__socket_nonblock(&new_sock)){
		തിരികെ ശൂന്യ;
	}

#ifdef WITH_WRAP
	/* Use tcpd / libwrap to determine whether a connection is allowed. */
	request_init(&wrap_req, RQ_FILE, new_sock, RQ_DAEMON, "mosquitto", 0);
	fromhost(&wrap_req);
	എങ്കിൽ(!hosts_access(&wrap_req)){
		/* Access is denied */
		എങ്കിൽ(db.config->connection_messages == true){
			എങ്കിൽ(!net__socket_get_address(new_sock, address, 1024, ശൂന്യ)){
				log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client connection from %s denied access by tcpd.", address);
			}
		}
		COMPAT_CLOSE(new_sock);
		തിരികെ ശൂന്യ;
	}
#endif

	എങ്കിൽ(db.config->set_tcp_nodelay && listensock->listener->port){
		പൂർണ്ണസംഖ്യ flag = 1;
		എങ്കിൽ(setsockopt(new_sock, IPPROTO_TCP, TCP_NODELAY, (സ്ഥിരം ശൂന്യം *)&flag, വലുപ്പം(പൂർണ്ണസംഖ്യ)) != 0){
			log__printf(ശൂന്യ, MOSQ_LOG_WARNING, "Warning: Unable to set TCP_NODELAY.");
		}
	}

	new_context = context__init();
	എങ്കിൽ(!new_context){
		COMPAT_CLOSE(new_sock);
		തിരികെ ശൂന്യ;
	}
	new_context->listener = listensock->listener;
	എങ്കിൽ(!new_context->listener){
		context__cleanup(new_context, true);
		തിരികെ ശൂന്യ;
	}
	new_context->listener->client_എണ്ണം++;

	എങ്കിൽ(new_context->listener->enable_proxy_protocol){
		എങ്കിൽ(context__init_sock(new_context, new_sock, false) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			തിരികെ ശൂന്യ;
		}
		എങ്കിൽ(new_context->listener->enable_proxy_protocol == 2){
			new_context->transport = mosq_t_proxy_v2;
		}അല്ലെങ്കിൽ{
			new_context->transport = mosq_t_proxy_v1;
		}
		new_context->proxy.cmd = -1;
	}അല്ലെങ്കിൽ{
		എങ്കിൽ(context__init_sock(new_context, new_sock, true) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			തിരികെ ശൂന്യ;
		}
		തിരഞ്ഞെടുപ്പ്(new_context->listener->protocol){
			അവസ്ഥ mp_mqtt:
				new_context->transport = mosq_t_tcp;
				നിർത്തുക;
#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			അവസ്ഥ mp_websockets:
				എങ്കിൽ(http__context_init(new_context)){
					context__cleanup(new_context, true);
					തിരികെ ശൂന്യ;
				}
				നിർത്തുക;
#endif
			സ്ഥിരസ്ഥിതി:
				context__cleanup(new_context, true);
				തിരികെ ശൂന്യ;
		}
	}

	എങ്കിൽ((new_context->listener->max_connections > 0 && new_context->listener->client_എണ്ണം > new_context->listener->max_connections)
			|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (unsigned പൂർണ്ണസംഖ്യ)db.config->global_max_connections)){

		എങ്കിൽ(db.config->connection_messages == true){
			log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", new_context->address);
		}
		context__cleanup(new_context, true);
		തിരികെ ശൂന്യ;
	}

#ifdef WITH_TLS
	/* TLS init */
	എങ്കിൽ(new_context->listener->ssl_ctx){
		new_context->ssl = SSL_new(new_context->listener->ssl_ctx);
		എങ്കിൽ(!new_context->ssl){
			context__cleanup(new_context, true);
			തിരികെ ശൂന്യ;
		}
		എങ്കിൽ(!SSL_set_ex_data(new_context->ssl, tls_ex_index_context, new_context)
				|| !SSL_set_ex_data(new_context->ssl, tls_ex_index_listener, new_context->listener)){
			context__cleanup(new_context, true);
			തിരികെ ശൂന്യ;
		}
		new_context->want_write = true;
		bio = BIO_new_socket(new_sock, BIO_NOCLOSE);
		SSL_set_bio(new_context->ssl, bio, bio);
		ERR_clear_error();
		SSL_set_accept_state(new_context->ssl);
	}
#endif

	എങ്കിൽ(db.config->connection_messages == true
			&& !new_context->listener->enable_proxy_protocol){

		log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "New connection from %s:%d on port %d.",
				new_context->address, new_context->remote_port, new_context->listener->port);
	}

	mux__new(new_context);
	keepalive__add(new_context);

	തിരികെ new_context;
}

#ifdef WITH_TLS


സ്ഥിതം പൂർണ്ണസംഖ്യ client_certificate_verify(പൂർണ്ണസംഖ്യ preverify_ok, X509_STORE_CTX *ctx)
{
	/* Preverify should check expiry, revocation. */
	എങ്കിൽ(preverify_ok == 0){
		SSL *ssl;
		ssl = X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx());
		എങ്കിൽ(ssl){
			ഘടന mosquitto__listener *listener;
			listener = SSL_get_ex_data(ssl, tls_ex_index_listener);

			എങ്കിൽ(listener && listener->disable_client_cert_date_checks){
				പൂർണ്ണസംഖ്യ err = X509_STORE_CTX_get_error(ctx);
				എങ്കിൽ(err == X509_V_ERR_CERT_NOT_YET_VALID || err == X509_V_ERR_CERT_HAS_EXPIRED){
					preverify_ok = 1;
				}
			}
		}
	}
	തിരികെ preverify_ok;
}
#endif

#ifdef FINAL_WITH_TLS_PSK


സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ psk_server_callback(SSL *ssl, സ്ഥിരം അക്ഷരം *identity, unsigned അക്ഷരം *psk, unsigned പൂർണ്ണസംഖ്യ max_psk_len)
{
	ഘടന mosquitto *context;
	ഘടന mosquitto__listener *listener;
	അക്ഷരം *psk_താക്കോൽ = ശൂന്യ;
	പൂർണ്ണസംഖ്യ len;
	സ്ഥിരം അക്ഷരം *psk_hint;

	എങ്കിൽ(!identity){
		തിരികെ 0;
	}

	context = SSL_get_ex_data(ssl, tls_ex_index_context);
	എങ്കിൽ(!context){
		തിരികെ 0;
	}

	listener = SSL_get_ex_data(ssl, tls_ex_index_listener);
	എങ്കിൽ(!listener){
		തിരികെ 0;
	}

	psk_hint = listener->psk_hint;

	/* The hex to BN conversion results in the length halving, so we can pass
	 * max_psk_len*2 as the max hex key here. */
	psk_താക്കോൽ = mosquitto_calloc(1, (size_t)max_psk_len*2 + 1);
	എങ്കിൽ(!psk_താക്കോൽ){
		തിരികെ 0;
	}

	എങ്കിൽ(mosquitto_psk_താക്കോൽ_get(context, psk_hint, identity, psk_താക്കോൽ, (പൂർണ്ണസംഖ്യ)max_psk_len*2) != MOSQ_ERR_SUCCESS){
		mosquitto_FREE(psk_താക്കോൽ);
		തിരികെ 0;
	}

	len = mosquitto__hex2bin(psk_താക്കോൽ, psk, (പൂർണ്ണസംഖ്യ)max_psk_len);
	എങ്കിൽ(len < 0){
		mosquitto_FREE(psk_താക്കോൽ);
		തിരികെ 0;
	}

	എങ്കിൽ(listener->use_identity_as_username){
		എങ്കിൽ(mosquitto_validate_utf8(identity, (പൂർണ്ണസംഖ്യ)strlen(identity))){
			mosquitto_free(psk_താക്കോൽ);
			തിരികെ 0;
		}
		context->username = mosquitto_strdup(identity);
		എങ്കിൽ(!context->username){
			mosquitto_FREE(psk_താക്കോൽ);
			തിരികെ 0;
		}
	}

	mosquitto_FREE(psk_താക്കോൽ);
	തിരികെ (unsigned പൂർണ്ണസംഖ്യ)len;
}
#endif

#ifdef WITH_TLS


സ്ഥിതം ശൂന്യം tls_keylog_callback(സ്ഥിരം SSL *ssl, സ്ഥിരം അക്ഷരം *വരി)
{
	UNUSED(ssl);

	എങ്കിൽ(db.tls_keylog){
		FILE *fptr;
		fptr = mosquitto_fopen(db.tls_keylog, "at", true);
		എങ്കിൽ(fptr){
#ifndef WIN32
			/* Until mosquitto_fopen enforces file permissions on all files,
			 * enforce it here. We can enforce it here, because it isn't a
			 * change of behaviour. */
			ഘടന stat statbuf;
			എങ്കിൽ(fstat(fileno(fptr), &statbuf) < 0
					|| statbuf.st_mode & (S_IRWXG | S_IRWXO)){

				fclose(fptr);
				തിരികെ;
			}
#endif
			fprintf(fptr, "%s\n", വരി);
			fclose(fptr);
		}അല്ലെങ്കിൽ{
#ifndef WIN32
			എങ്കിൽ(errno == ELOOP){
				log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Error: keylog file must not be a symbolic link");
			}അല്ലെങ്കിൽ
#endif
			{
				log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Error: Unable to open keylog file: %s", strerror(errno));
			}
		}
	}
}


പൂർണ്ണസംഖ്യ net__tls_server_ctx(ഘടന mosquitto__listener *listener)
{
	അക്ഷരം buf[256];
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ(tls_ex_index_context == -1){
		tls_ex_index_context = SSL_get_ex_new_index(0, "client context", ശൂന്യ, ശൂന്യ, ശൂന്യ);
	}
	എങ്കിൽ(tls_ex_index_listener == -1){
		tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", ശൂന്യ, ശൂന്യ, ശൂന്യ);
	}

	എങ്കിൽ(listener->ssl_ctx){
		SSL_CTX_free(listener->ssl_ctx);
		listener->ssl_ctx = ശൂന്യ;
	}

	listener->ssl_ctx = SSL_CTX_new(TLS_server_method());

	എങ്കിൽ(!listener->ssl_ctx){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to create TLS context.");
		തിരികെ MOSQ_ERR_TLS;
	}

#ifdef SSL_OP_NO_TLSv1_3
	എങ്കിൽ(db.config->per_listener_settings){
		എങ്കിൽ(listener->security_options->psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}അല്ലെങ്കിൽ{
		എങ്കിൽ(db.config->security_options.psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}
#endif

	എങ്കിൽ(listener->tls_version == ശൂന്യ){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
#ifdef SSL_OP_NO_TLSv1_3
	}അല്ലെങ്കിൽ എങ്കിൽ(!strcmp(listener->tls_version, "tlsv1.3")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2);
#endif
	}അല്ലെങ്കിൽ എങ്കിൽ(!strcmp(listener->tls_version, "tlsv1.2")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
	}അല്ലെങ്കിൽ എങ്കിൽ(!strcmp(listener->tls_version, "tlsv1.1")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1);
		log__printf(ശൂന്യ, MOSQ_LOG_WARNING, "Warning: TLS v1.1 is insecure. It must only be used in the case "
				"where you have devices that cannot be upgraded to use a secure version of TLS. It is "
				"recommended to use as secure a set of ciphers as possible and that will restrict it to "
				"TLS v1.1 only, and to use a separate listener using TLS v1.2 and secure ciphers for your "
				"other devices.");
		log__printf(ശൂന്യ, MOSQ_LOG_WARNING, "Please be aware that support for TLS v1.1 will go away eventually "
				"and that you should plan now to migrate away from it.");
	}അല്ലെങ്കിൽ{
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unsupported tls_version \"%s\".", listener->tls_version);
		തിരികെ MOSQ_ERR_TLS;
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

	എങ്കിൽ(db.tls_keylog){
		log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "TLS key logging to '%s' enabled for all listeners.",
				db.tls_keylog);
		log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "TLS key logging is for DEBUGGING only.");

		SSL_CTX_set_keylog_callback(listener->ssl_ctx, tls_keylog_callback);
	}

	snprintf(buf, 256, "mosquitto-%d", listener->port);
	SSL_CTX_set_session_id_context(listener->ssl_ctx, (unsigned അക്ഷരം *)buf, (unsigned പൂർണ്ണസംഖ്യ)strlen(buf));

	എങ്കിൽ(listener->ciphers){
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, listener->ciphers);
		എങ്കിൽ(rc == 0){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			തിരികെ MOSQ_ERR_TLS;
		}
	}അല്ലെങ്കിൽ{
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, "DEFAULT:!aNULL:!eNULL:!LOW:!EXPORT:!SSLv2:@STRENGTH");
		എങ്കിൽ(rc == 0){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			തിരികെ MOSQ_ERR_TLS;
		}
	}
	എങ്കിൽ(listener->ciphers_tls13){
		rc = SSL_CTX_set_ciphersuites(listener->ssl_ctx, listener->ciphers_tls13);
		എങ്കിൽ(rc == 0){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to set TLS 1.3 ciphersuites. Check cipher_tls13 list \"%s\".", listener->ciphers_tls13);
			തിരികെ MOSQ_ERR_TLS;
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}
#endif


#ifdef WITH_TLS


സ്ഥിതം പൂർണ്ണസംഖ്യ net__load_crl_file(ഘടന mosquitto__listener *listener)
{
	X509_STORE *store;
	X509_LOOKUP *lookup;
	പൂർണ്ണസംഖ്യ rc;

	store = SSL_CTX_get_cert_store(listener->ssl_ctx);
	എങ്കിൽ(!store){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to obtain TLS store.");
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		തിരികെ MOSQ_ERR_TLS;
	}
	lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
	rc = X509_load_crl_file(lookup, listener->crlfile, X509_FILETYPE_PEM);
	എങ്കിൽ(rc < 1){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to load certificate revocation file \"%s\". Check crlfile.", listener->crlfile);
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		net__print_ssl_error(ശൂന്യ, ശൂന്യ);
		തിരികെ MOSQ_ERR_TLS;
	}
	X509_STORE_set_flags(store, X509_V_FLAG_CRL_CHECK);

	തിരികെ MOSQ_ERR_SUCCESS;
}
#endif


പൂർണ്ണസംഖ്യ net__load_certificates(ഘടന mosquitto__listener *listener)
{
#ifdef WITH_TLS
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ(listener->require_certificate){
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, client_certificate_verify);
	}അല്ലെങ്കിൽ{
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_NONE, client_certificate_verify);
	}
	rc = SSL_CTX_use_certificate_chain_file(listener->ssl_ctx, listener->certfile);
	എങ്കിൽ(rc != 1){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
		net__print_ssl_error(ശൂന്യ, ശൂന്യ);
		തിരികെ MOSQ_ERR_TLS;
	}
	എങ്കിൽ(listener->tls_engine == ശൂന്യ || listener->tls_keyform == mosq_k_pem){
		rc = SSL_CTX_use_Private_താക്കോൽ_file(listener->ssl_ctx, listener->keyfile, SSL_FILETYPE_PEM);
		എങ്കിൽ(rc != 1){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			net__print_ssl_error(ശൂന്യ, ശൂന്യ);
			തിരികെ MOSQ_ERR_TLS;
		}
	}
	rc = SSL_CTX_check_private_താക്കോൽ(listener->ssl_ctx);
	എങ്കിൽ(rc != 1){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Server certificate/key are inconsistent.");
		net__print_ssl_error(ശൂന്യ, ശൂന്യ);
		തിരികെ MOSQ_ERR_TLS;
	}
	/* Load CRLs if they exist. */
	എങ്കിൽ(listener->crlfile){
		rc = net__load_crl_file(listener);
		എങ്കിൽ(rc){
			തിരികെ rc;
		}
	}
#അല്ലെങ്കിൽ
	UNUSED(listener);
#endif
	തിരികെ MOSQ_ERR_SUCCESS;
}


#എങ്കിൽ defined(WITH_TLS) && !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000


സ്ഥിതം പൂർണ്ണസംഖ്യ net__load_engine(ഘടന mosquitto__listener *listener)
{
	ENGINE *engine = ശൂന്യ;
	UI_METHOD *ui_method;
	EVP_PKEY *pkey;

	എങ്കിൽ(!listener->tls_engine){
		തിരികെ MOSQ_ERR_SUCCESS;
	}

	engine = ENGINE_by_id(listener->tls_engine);
	എങ്കിൽ(!engine){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error loading %s engine\n", listener->tls_engine);
		net__print_ssl_error(ശൂന്യ, ശൂന്യ);
		തിരികെ MOSQ_ERR_TLS;
	}
	എങ്കിൽ(!ENGINE_init(engine)){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Failed engine initialisation\n");
		net__print_ssl_error(ശൂന്യ, ശൂന്യ);
		തിരികെ MOSQ_ERR_TLS;
	}
	ENGINE_set_default(engine, ENGINE_METHOD_ALL);

	എങ്കിൽ(listener->tls_keyform == mosq_k_engine){
		ui_method = net__get_ui_method();
		എങ്കിൽ(listener->tls_engine_kpass_sha1){
			എങ്കിൽ(!ENGINE_ctrl_cmd(engine, ENGINE_SECRET_MODE, ENGINE_SECRET_MODE_SHA, ശൂന്യ, ശൂന്യ, 0)){
				log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to set engine secret mode sha");
				net__print_ssl_error(ശൂന്യ, ശൂന്യ);
				തിരികെ MOSQ_ERR_TLS;
			}
			എങ്കിൽ(!ENGINE_ctrl_cmd(engine, ENGINE_PIN, 0, listener->tls_engine_kpass_sha1, ശൂന്യ, 0)){
				log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to set engine pin");
				net__print_ssl_error(ശൂന്യ, ശൂന്യ);
				തിരികെ MOSQ_ERR_TLS;
			}
			ui_method = ശൂന്യ;
		}
		pkey = ENGINE_load_private_താക്കോൽ(engine, listener->keyfile, ui_method, ശൂന്യ);
		എങ്കിൽ(!pkey){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to load engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(ശൂന്യ, ശൂന്യ);
			തിരികെ MOSQ_ERR_TLS;
		}
		എങ്കിൽ(SSL_CTX_use_Private_താക്കോൽ(listener->ssl_ctx, pkey) <= 0){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to use engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(ശൂന്യ, ശൂന്യ);
			തിരികെ MOSQ_ERR_TLS;
		}
	}
	ENGINE_free(engine); /* release the structural reference from ENGINE_by_id() */

	തിരികെ MOSQ_ERR_SUCCESS;
}
#endif


പൂർണ്ണസംഖ്യ net__tls_load_verify(ഘടന mosquitto__listener *listener)
{
#ifdef WITH_TLS
	പൂർണ്ണസംഖ്യ rc;

#  എങ്കിൽ OPENSSL_VERSION_സംഖ്യ < 0x30000000L
	എങ്കിൽ(listener->cafile || listener->capath){
		rc = SSL_CTX_load_verify_locations(listener->ssl_ctx, listener->cafile, listener->capath);
		എങ്കിൽ(rc == 0){
			എങ്കിൽ(listener->cafile && listener->capath){
				log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\" and capath \"%s\".", listener->cafile, listener->capath);
			}അല്ലെങ്കിൽ എങ്കിൽ(listener->cafile){
				log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			}അല്ലെങ്കിൽ{
				log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			}
		}
	}
#  അല്ലെങ്കിൽ
	അട്ടി_OF(X509_NAME) *ca_names = ശൂന്യ;
	എങ്കിൽ(listener->cafile || listener->capath){
		ca_names = sk_X509_NAME_new_null();
	}

	എങ്കിൽ(listener->cafile){
		rc = SSL_CTX_load_verify_file(listener->ssl_ctx, listener->cafile);
		എങ്കിൽ(rc == 0){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			net__print_ssl_error(ശൂന്യ, ശൂന്യ);
			തിരികെ MOSQ_ERR_TLS;
		}

		അട്ടി_OF(X509_NAME) *cert_names = SSL_load_client_CA_file(listener->cafile);
		എങ്കിൽ(cert_names){
			വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<sk_X509_NAME_അക്കം(cert_names); i++){
				sk_X509_NAME_തള്ളുക(ca_names, X509_NAME_dup(sk_X509_NAME_മൂല്യം(cert_names, i)));
			}
			sk_X509_NAME_എടുക്കുക_free(cert_names, X509_NAME_free);
		}

	}
	എങ്കിൽ(listener->capath){
		rc = SSL_CTX_load_verify_dir(listener->ssl_ctx, listener->capath);
		എങ്കിൽ(rc == 0){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			net__print_ssl_error(ശൂന്യ, ശൂന്യ);
			sk_X509_NAME_എടുക്കുക_free(ca_names, X509_NAME_free);
			തിരികെ MOSQ_ERR_TLS;
		}

		SSL_add_dir_cert_subjects_to_അട്ടി(ca_names, listener->capath);
	}
	എങ്കിൽ(ca_names){
		SSL_CTX_set_client_CA_list(listener->ssl_ctx, ca_names);
	}
#  endif

#  എങ്കിൽ !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000
	എങ്കിൽ(net__load_engine(listener)){
		തിരികെ MOSQ_ERR_TLS;
	}
#  endif
#endif
	തിരികെ net__load_certificates(listener);
}


#എങ്കിൽ !defined(WIN32) && !defined(_AIX)


സ്ഥിതം പൂർണ്ണസംഖ്യ net__bind_interface(ഘടന mosquitto__listener *listener, ഘടന addrinfo *rp)
{
	/*
	 * This binds the listener sock to a network interface.
	 * The use of SO_BINDTODEVICE requires root access, which we don't have, so instead
	 * use getifaddrs to find the interface addresses, and use IP of the
	 * matching interface in the later bind().
	 */
	ഘടന ifaddrs *ifaddr;
	bool have_interface = false;

	എങ്കിൽ(getifaddrs(&ifaddr) < 0){
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		തിരികെ MOSQ_ERR_ERRNO;
	}

	വേണ്ടി(ഘടന ifaddrs *ifa=ifaddr; ifa!=ശൂന്യ; ifa=ifa->ifa_next){
		എങ്കിൽ(ifa->ifa_addr == ശൂന്യ){
			തുടരുക;
		}

		എങ്കിൽ(!strcasecmp(listener->bind_interface, ifa->ifa_name)){
			have_interface = true;

			എങ്കിൽ(ifa->ifa_addr->sa_family == rp->ai_addr->sa_family){
				എങ്കിൽ(rp->ai_addr->sa_family == AF_INET){
					എങ്കിൽ(listener->host &&
							memcmp(&((ഘടന sockaddr_in *)rp->ai_addr)->sin_addr,
							&((ഘടന sockaddr_in *)ifa->ifa_addr)->sin_addr,
							വലുപ്പം(ഘടന in_addr))){

						log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						തിരികെ MOSQ_ERR_INVAL;
					}അല്ലെങ്കിൽ{
						memcpy(&((ഘടന sockaddr_in *)rp->ai_addr)->sin_addr,
								&((ഘടന sockaddr_in *)ifa->ifa_addr)->sin_addr,
								വലുപ്പം(ഘടന in_addr));

						freeifaddrs(ifaddr);
						തിരികെ MOSQ_ERR_SUCCESS;
					}
				}അല്ലെങ്കിൽ എങ്കിൽ(rp->ai_addr->sa_family == AF_INET6){
					എങ്കിൽ(listener->host &&
							memcmp(&((ഘടന sockaddr_in6 *)rp->ai_addr)->sin6_addr,
							&((ഘടന sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
							വലുപ്പം(ഘടന in6_addr))){

						log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						തിരികെ MOSQ_ERR_INVAL;
					}അല്ലെങ്കിൽ{
						memcpy(&((ഘടന sockaddr_in6 *)rp->ai_addr)->sin6_addr,
								&((ഘടന sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
								വലുപ്പം(ഘടന in6_addr));

						((ഘടന sockaddr_in6 *)rp->ai_addr)->sin6_scope_id = ((ഘടന sockaddr_in6 *)ifa->ifa_addr)->sin6_scope_id;

						freeifaddrs(ifaddr);
						തിരികെ MOSQ_ERR_SUCCESS;
					}
				}
			}
		}
	}
	freeifaddrs(ifaddr);
	എങ്കിൽ(have_interface){
		log__printf(ശൂന്യ, MOSQ_LOG_WARNING, "Warning: Interface %s does not support %s configuration.",
				listener->bind_interface, rp->ai_addr->sa_family == AF_INET ? "IPv4" : "IPv6");
		തിരികെ MOSQ_ERR_NOT_SUPPORTED;
	}അല്ലെങ്കിൽ{
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Interface %s does not exist.", listener->bind_interface);
		തിരികെ MOSQ_ERR_NOT_FOUND;
	}
}
#endif


സ്ഥിതം പൂർണ്ണസംഖ്യ net__socket_listen_tcp(ഘടന mosquitto__listener *listener)
{
	mosq_sock_t sock = INVALID_SOCKET;
	ഘടന addrinfo hints;
	ഘടന addrinfo *ainfo, *rp;
	അക്ഷരം service[10];
	പൂർണ്ണസംഖ്യ rc;
	പൂർണ്ണസംഖ്യ ss_opt = 1;
#ifndef WIN32
	bool interface_bound = false;
#endif

	എങ്കിൽ(!listener){
		തിരികെ MOSQ_ERR_INVAL;
	}

	snprintf(service, 10, "%d", listener->port);
	memset(&hints, 0, വലുപ്പം(ഘടന addrinfo));
	എങ്കിൽ(listener->socket_domain){
		hints.ai_family = listener->socket_domain;
	}അല്ലെങ്കിൽ{
		hints.ai_family = AF_UNSPEC;
	}
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	rc = getaddrinfo(listener->host, service, &hints, &ainfo);
	എങ്കിൽ(rc){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error creating listener: %s.", gai_strerror(rc));
		തിരികെ INVALID_SOCKET;
	}

	listener->sock_എണ്ണം = 0;
	listener->socks = ശൂന്യ;

	വേണ്ടി(rp = ainfo; rp; rp = rp->ai_next){
		എങ്കിൽ(rp->ai_family == AF_INET){
			log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Opening ipv4 listen socket on port %d.", ntohs(((ഘടന sockaddr_in *)rp->ai_addr)->sin_port));
		}അല്ലെങ്കിൽ എങ്കിൽ(rp->ai_family == AF_INET6){
			log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Opening ipv6 listen socket on port %d.", ntohs(((ഘടന sockaddr_in6 *)rp->ai_addr)->sin6_port));
		}അല്ലെങ്കിൽ{
			തുടരുക;
		}

		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		എങ്കിൽ(sock == INVALID_SOCKET){
			net__print_error(MOSQ_LOG_WARNING, "Warning: %s");
			തുടരുക;
		}
		listener->sock_എണ്ണം++;
		listener->socks = mosquitto_realloc(listener->socks, വലുപ്പം(mosq_sock_t)*(size_t)listener->sock_എണ്ണം);
		എങ്കിൽ(!listener->socks){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Out of memory.");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			തിരികെ MOSQ_ERR_NOMEM;
		}
		listener->socks[listener->sock_എണ്ണം-1] = sock;

#ifndef WIN32
		ss_opt = 1;
		/* Unimportant if this fails */
		(ശൂന്യം)setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (സ്ഥിരം ശൂന്യം *)&ss_opt, വലുപ്പം(ss_opt));
#endif
#ifdef IPV6_V6ONLY
		ss_opt = 1;
		(ശൂന്യം)setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (സ്ഥിരം ശൂന്യം *)&ss_opt, വലുപ്പം(ss_opt));
#endif

		എങ്കിൽ(net__socket_nonblock(&sock)){
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			തിരികെ 1;
		}

#എങ്കിൽ !defined(WIN32) && !defined(_AIX)
		എങ്കിൽ(listener->bind_interface){
			/* It might be possible that an interface does not support all relevant sa_families.
			 * We should successfully find at least one. */
			rc = net__bind_interface(listener, rp);
			എങ്കിൽ(rc){
				COMPAT_CLOSE(sock);
				listener->sock_എണ്ണം--;
				എങ്കിൽ(rc == MOSQ_ERR_NOT_FOUND || rc == MOSQ_ERR_INVAL){
					freeaddrinfo(ainfo);
					തിരികെ rc;
				}അല്ലെങ്കിൽ{
					തുടരുക;
				}
			}
			interface_bound = true;
		}
#endif

		എങ്കിൽ(bind(sock, rp->ai_addr, rp->ai_addrlen) == -1){
#എങ്കിൽ defined(__linux__)
			എങ്കിൽ(errno == EACCES){
				log__printf(ശൂന്യ, MOSQ_LOG_ERR, "If you are trying to bind to a privileged port (<1024), try using setcap and do not start the broker as root:");
				log__printf(ശൂന്യ, MOSQ_LOG_ERR, "    sudo setcap 'CAP_NET_BIND_SERVICE=+ep /usr/sbin/mosquitto'");
			}
#endif
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			COMPAT_CLOSE(sock);
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			തിരികെ 1;
		}

		എങ്കിൽ(listen(sock, 100) == -1){
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			mosquitto_FREE(listener->socks);
			തിരികെ 1;
		}
	}
	freeaddrinfo(ainfo);

#ifndef WIN32
	എങ്കിൽ(listener->bind_interface && !interface_bound){
		mosquitto_FREE(listener->socks);
		തിരികെ 1;
	}
#endif

	തിരികെ 0;
}


#ifdef WITH_UNIX_SOCKETS


സ്ഥിതം പൂർണ്ണസംഖ്യ net__socket_listen_unix(ഘടന mosquitto__listener *listener)
{
	ഘടന sockaddr_un addr;
	mosq_sock_t sock = INVALID_SOCKET;
	പൂർണ്ണസംഖ്യ rc;
#ifndef WIN32
	mode_t old_mask;
#endif

	എങ്കിൽ(listener->unix_socket_path == ശൂന്യ){
		തിരികെ MOSQ_ERR_INVAL;
	}
	എങ്കിൽ(strlen(listener->unix_socket_path) > വലുപ്പം(addr.sun_path)-1){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Path to unix socket is too long \"%s\".", listener->unix_socket_path);
		തിരികെ MOSQ_ERR_INVAL;
	}

#ifdef WIN32
	DeleteFile(listener->unix_socket_path);
#അല്ലെങ്കിൽ
	unlink(listener->unix_socket_path);
#endif
	log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Opening unix listen socket on path %s.", listener->unix_socket_path);
	memset(&addr, 0, വലുപ്പം(ഘടന sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, listener->unix_socket_path, വലുപ്പം(addr.sun_path)-1);

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	എങ്കിൽ(sock == INVALID_SOCKET){
		net__print_error(MOSQ_LOG_ERR, "Error creating unix socket: %s");
		തിരികെ 1;
	}
	listener->sock_എണ്ണം++;
	listener->socks = mosquitto_realloc(listener->socks, വലുപ്പം(mosq_sock_t)*(size_t)listener->sock_എണ്ണം);
	എങ്കിൽ(!listener->socks){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Out of memory.");
		COMPAT_CLOSE(sock);
		തിരികെ MOSQ_ERR_NOMEM;
	}
	listener->socks[listener->sock_എണ്ണം-1] = sock;


#ifndef WIN32
	old_mask = umask(0007);
#endif
	rc = bind(sock, (ഘടന sockaddr *)&addr, വലുപ്പം(ഘടന sockaddr_un));
#ifndef WIN32
	umask(old_mask);
#endif

	എങ്കിൽ(rc == -1){
		net__print_error(MOSQ_LOG_ERR, "Error binding unix socket: %s");
		തിരികെ 1;
	}

	എങ്കിൽ(listen(sock, 10) == -1){
		net__print_error(MOSQ_LOG_ERR, "Error listening to unix socket: %s");
		തിരികെ 1;
	}

	എങ്കിൽ(net__socket_nonblock(&sock)){
		തിരികെ 1;
	}

	തിരികെ 0;
}
#endif


/* Creates a socket and listens on port 'port'.
 * Returns 1 on failure
 * Returns 0 on success.
 */
പൂർണ്ണസംഖ്യ net__socket_listen(ഘടന mosquitto__listener *listener)
{
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ(!listener){
		തിരികെ MOSQ_ERR_INVAL;
	}

#ifdef WITH_UNIX_SOCKETS
	എങ്കിൽ(listener->port == 0 && listener->unix_socket_path != ശൂന്യ){
		rc = net__socket_listen_unix(listener);
	}അല്ലെങ്കിൽ
#endif
	{
		rc = net__socket_listen_tcp(listener);
	}
	എങ്കിൽ(rc){
		തിരികെ rc;
	}

	/* We need to have at least one working socket. */
	എങ്കിൽ(listener->sock_എണ്ണം > 0){
#ifdef WITH_TLS
		എങ്കിൽ(listener->certfile && listener->keyfile){
			എങ്കിൽ(net__tls_server_ctx(listener)){
				തിരികെ 1;
			}

			എങ്കിൽ(net__tls_load_verify(listener)){
				തിരികെ 1;
			}
		}
#  ifdef FINAL_WITH_TLS_PSK
		എങ്കിൽ(listener->psk_hint){
			എങ്കിൽ(listener->certfile == ശൂന്യ || listener->keyfile == ശൂന്യ){
				എങ്കിൽ(net__tls_server_ctx(listener)){
					തിരികെ 1;
				}
			}
			SSL_CTX_set_psk_server_callback(listener->ssl_ctx, psk_server_callback);
			എങ്കിൽ(listener->psk_hint){
				rc = SSL_CTX_use_psk_identity_hint(listener->ssl_ctx, listener->psk_hint);
				എങ്കിൽ(rc == 0){
					log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to set TLS PSK hint.");
					net__print_ssl_error(ശൂന്യ, ശൂന്യ);
					തിരികെ 1;
				}
			}
		}
#  endif /* FINAL_WITH_TLS_PSK */
		എങ്കിൽ(tls_ex_index_context == -1){
			tls_ex_index_context = SSL_get_ex_new_index(0, "client context", ശൂന്യ, ശൂന്യ, ശൂന്യ);
		}
		എങ്കിൽ(tls_ex_index_listener == -1){
			tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", ശൂന്യ, ശൂന്യ, ശൂന്യ);
		}
#endif /* WITH_TLS */
		തിരികെ 0;
	}അല്ലെങ്കിൽ{
		തിരികെ 1;
	}
}


പൂർണ്ണസംഖ്യ net__socket_get_address(mosq_sock_t sock, അക്ഷരം *buf, size_t len, uint16_t *remote_port)
{
	ഘടന sockaddr_storage addr;
	socklen_t addrlen;

	memset(&addr, 0, വലുപ്പം(ഘടന sockaddr_storage));
	addrlen = വലുപ്പം(addr);
	എങ്കിൽ(!getpeername(sock, (ഘടന sockaddr *)&addr, &addrlen)){
		എങ്കിൽ(addr.ss_family == AF_INET){
			എങ്കിൽ(remote_port){
				*remote_port = ntohs(((ഘടന sockaddr_in *)&addr)->sin_port);
			}
			എങ്കിൽ(inet_ntop(AF_INET, &((ഘടന sockaddr_in *)&addr)->sin_addr.s_addr, buf, (socklen_t)len)){
				തിരികെ 0;
			}
		}അല്ലെങ്കിൽ എങ്കിൽ(addr.ss_family == AF_INET6){
			എങ്കിൽ(remote_port){
				*remote_port = ntohs(((ഘടന sockaddr_in6 *)&addr)->sin6_port);
			}
			എങ്കിൽ(inet_ntop(AF_INET6, &((ഘടന sockaddr_in6 *)&addr)->sin6_addr.s6_addr, buf, (socklen_t)len)){
				തിരികെ 0;
			}
#ifdef WITH_UNIX_SOCKETS
		}അല്ലെങ്കിൽ എങ്കിൽ(addr.ss_family == AF_UNIX){
			ഘടന sockaddr_un un;
			addrlen = വലുപ്പം(ഘടന sockaddr_un);
			എങ്കിൽ(!getsockname(sock, (ഘടന sockaddr *)&un, &addrlen)){
				snprintf(buf, len, "%s", un.sun_path);
			}അല്ലെങ്കിൽ{
				snprintf(buf, len, "unix-socket");
			}
			തിരികെ 0;
#endif
		}
	}
	തിരികെ 1;
}
