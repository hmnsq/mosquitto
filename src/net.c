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

#অন্তর্ভাৱ "config.h"

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
#অন্যথা
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#অন্তর্ভাৱ <errno.h>
#অন্তর্ভাৱ <fcntl.h>
#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <string.h>
#ifdef WITH_WRAP
#  include <tcpd.h>
#endif

#ifdef HAVE_NETINET_IN_H
#  include <netinet/in.h>
#endif

#যদি defined(WITH_UNIX_SOCKETS) || defined(WITH_TLS)
#  include "sys/stat.h"
#endif

#ifdef WITH_UNIX_SOCKETS
#  ifdef WIN32
#    include <afunix.h>
#  অন্যথা
#    include <sys/un.h>
#  endif
#endif

#ifdef __QNX__
#  include <net/netbyte.h>
#endif

#অন্তর্ভাৱ "mosquitto_broker_internal.h"
#অন্তর্ভাৱ "mosquitto/mqtt_protocol.h"
#অন্তর্ভাৱ "net_mosq.h"
#অন্তর্ভাৱ "util_mosq.h"

#ifdef WITH_TLS
#  include "tls_mosq.h"
#  include <openssl/err.h>
স্থির পূর্ণাঙ্ক tls_ex_index_context = -1;
স্থির পূর্ণাঙ্ক tls_ex_index_listener = -1;
#endif

#অন্তর্ভাৱ "sys_tree.h"

/* For EMFILE handling */
স্থির mosq_sock_t spare_sock = INVALID_SOCKET;


শূন্য net__broker_init(শূন্য)
{
	spare_sock = socket(AF_INET, SOCK_STREAM, 0);
	net__init();
#ifdef WITH_TLS
	net__init_tls();
#endif
}


শূন্য net__broker_cleanup(শূন্য)
{
	যদি(spare_sock != INVALID_SOCKET){
		COMPAT_CLOSE(spare_sock);
		spare_sock = INVALID_SOCKET;
	}
	net__cleanup();
}


স্থির শূন্য net__print_error(unsigned পূর্ণাঙ্ক log, ধ্রুৱ ৱর্ণ *format_str)
{
	ৱর্ণ *buf;

#ifdef WIN32
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			রিক্ত, WSAGetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, রিক্ত);

	log__printf(রিক্ত, log, format_str, buf);
	LocalFree(buf);
#অন্যথা
	buf = strerror(errno);
	log__printf(রিক্ত, log, format_str, buf);
#endif
}


সংরচনা mosquitto *net__socket_accept(সংরচনা mosquitto__listener_sock *listensock)
{
	mosq_sock_t new_sock = INVALID_SOCKET;
	সংরচনা mosquitto *new_context;
#ifdef WITH_TLS
	BIO *bio;
#endif
#ifdef WITH_WRAP
	সংরচনা request_info wrap_req;
	ৱর্ণ address[1024];
#endif

	new_sock = accept(listensock->sock, রিক্ত, 0);
	যদি(new_sock == INVALID_SOCKET){
#ifdef WIN32
		WINDOWS_SET_ERRNO();
		যদি(errno == WSAEMFILE){
#অন্যথা
		যদি(errno == EMFILE || errno == ENFILE){
#endif
			/* Close the spare socket, which means we should be able to accept
			 * this connection. Accept it, then close it immediately and create
			 * a new spare_sock. This prevents the situation of ever properly
			 * running out of sockets.
			 * It would be nice to send a "server not available" connack here,
			 * but there are lots of reasons why this would be tricky (TLS
			 * being the big one). */
			COMPAT_CLOSE(spare_sock);
			new_sock = accept(listensock->sock, রিক্ত, 0);
			যদি(new_sock != INVALID_SOCKET){
				COMPAT_CLOSE(new_sock);
			}
			spare_sock = socket(AF_INET, SOCK_STREAM, 0);
			log__printf(রিক্ত, MOSQ_LOG_WARNING,
					"Unable to accept new connection, system socket count has been exceeded. Try increasing \"ulimit -n\" or equivalent.");
		}
		ঘূৰাই রিক্ত;
	}

	metrics__int_inc(mosq_counter_socket_connections, 1);

	যদি(net__socket_nonblock(&new_sock)){
		ঘূৰাই রিক্ত;
	}

#ifdef WITH_WRAP
	/* Use tcpd / libwrap to determine whether a connection is allowed. */
	request_init(&wrap_req, RQ_FILE, new_sock, RQ_DAEMON, "mosquitto", 0);
	fromhost(&wrap_req);
	যদি(!hosts_access(&wrap_req)){
		/* Access is denied */
		যদি(db.config->connection_messages == true){
			যদি(!net__socket_get_address(new_sock, address, 1024, রিক্ত)){
				log__printf(রিক্ত, MOSQ_LOG_NOTICE, "Client connection from %s denied access by tcpd.", address);
			}
		}
		COMPAT_CLOSE(new_sock);
		ঘূৰাই রিক্ত;
	}
#endif

	যদি(db.config->set_tcp_nodelay && listensock->listener->port){
		পূর্ণাঙ্ক flag = 1;
		যদি(setsockopt(new_sock, IPPROTO_TCP, TCP_NODELAY, (ধ্রুৱ শূন্য *)&flag, পরিমাণ(পূর্ণাঙ্ক)) != 0){
			log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Unable to set TCP_NODELAY.");
		}
	}

	new_context = context__init();
	যদি(!new_context){
		COMPAT_CLOSE(new_sock);
		ঘূৰাই রিক্ত;
	}
	new_context->listener = listensock->listener;
	যদি(!new_context->listener){
		context__cleanup(new_context, true);
		ঘূৰাই রিক্ত;
	}
	new_context->listener->client_গুনতি++;

	যদি(new_context->listener->enable_proxy_protocol){
		যদি(context__init_sock(new_context, new_sock, false) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			ঘূৰাই রিক্ত;
		}
		যদি(new_context->listener->enable_proxy_protocol == 2){
			new_context->transport = mosq_t_proxy_v2;
		}অন্যথা{
			new_context->transport = mosq_t_proxy_v1;
		}
		new_context->proxy.cmd = -1;
	}অন্যথা{
		যদি(context__init_sock(new_context, new_sock, true) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			ঘূৰাই রিক্ত;
		}
		ৱিকল্প(new_context->listener->protocol){
			পক্ষ mp_mqtt:
				new_context->transport = mosq_t_tcp;
				ভঙ্গ;
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			পক্ষ mp_websockets:
				যদি(http__context_init(new_context)){
					context__cleanup(new_context, true);
					ঘূৰাই রিক্ত;
				}
				ভঙ্গ;
#endif
			স্ৱভাৱ:
				context__cleanup(new_context, true);
				ঘূৰাই রিক্ত;
		}
	}

	যদি((new_context->listener->max_connections > 0 && new_context->listener->client_গুনতি > new_context->listener->max_connections)
			|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (unsigned পূর্ণাঙ্ক)db.config->global_max_connections)){

		যদি(db.config->connection_messages == true){
			log__printf(রিক্ত, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", new_context->address);
		}
		context__cleanup(new_context, true);
		ঘূৰাই রিক্ত;
	}

#ifdef WITH_TLS
	/* TLS init */
	যদি(new_context->listener->ssl_ctx){
		new_context->ssl = SSL_new(new_context->listener->ssl_ctx);
		যদি(!new_context->ssl){
			context__cleanup(new_context, true);
			ঘূৰাই রিক্ত;
		}
		যদি(!SSL_set_ex_data(new_context->ssl, tls_ex_index_context, new_context)
				|| !SSL_set_ex_data(new_context->ssl, tls_ex_index_listener, new_context->listener)){
			context__cleanup(new_context, true);
			ঘূৰাই রিক্ত;
		}
		new_context->want_write = true;
		bio = BIO_new_socket(new_sock, BIO_NOCLOSE);
		SSL_set_bio(new_context->ssl, bio, bio);
		ERR_clear_error();
		SSL_set_accept_state(new_context->ssl);
	}
#endif

	যদি(db.config->connection_messages == true
			&& !new_context->listener->enable_proxy_protocol){

		log__printf(রিক্ত, MOSQ_LOG_NOTICE, "New connection from %s:%d on port %d.",
				new_context->address, new_context->remote_port, new_context->listener->port);
	}

	mux__new(new_context);
	keepalive__add(new_context);

	ঘূৰাই new_context;
}

#ifdef WITH_TLS


স্থির পূর্ণাঙ্ক client_certificate_verify(পূর্ণাঙ্ক preverify_ok, X509_STORE_CTX *ctx)
{
	/* Preverify should check expiry, revocation. */
	যদি(preverify_ok == 0){
		SSL *ssl;
		ssl = X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx());
		যদি(ssl){
			সংরচনা mosquitto__listener *listener;
			listener = SSL_get_ex_data(ssl, tls_ex_index_listener);

			যদি(listener && listener->disable_client_cert_date_checks){
				পূর্ণাঙ্ক err = X509_STORE_CTX_get_error(ctx);
				যদি(err == X509_V_ERR_CERT_NOT_YET_VALID || err == X509_V_ERR_CERT_HAS_EXPIRED){
					preverify_ok = 1;
				}
			}
		}
	}
	ঘূৰাই preverify_ok;
}
#endif

#ifdef FINAL_WITH_TLS_PSK


স্থির unsigned পূর্ণাঙ্ক psk_server_callback(SSL *ssl, ধ্রুৱ ৱর্ণ *identity, unsigned ৱর্ণ *psk, unsigned পূর্ণাঙ্ক max_psk_len)
{
	সংরচনা mosquitto *context;
	সংরচনা mosquitto__listener *listener;
	ৱর্ণ *psk_চাবি = রিক্ত;
	পূর্ণাঙ্ক len;
	ধ্রুৱ ৱর্ণ *psk_hint;

	যদি(!identity){
		ঘূৰাই 0;
	}

	context = SSL_get_ex_data(ssl, tls_ex_index_context);
	যদি(!context){
		ঘূৰাই 0;
	}

	listener = SSL_get_ex_data(ssl, tls_ex_index_listener);
	যদি(!listener){
		ঘূৰাই 0;
	}

	psk_hint = listener->psk_hint;

	/* The hex to BN conversion results in the length halving, so we can pass
	 * max_psk_len*2 as the max hex key here. */
	psk_চাবি = mosquitto_calloc(1, (size_t)max_psk_len*2 + 1);
	যদি(!psk_চাবি){
		ঘূৰাই 0;
	}

	যদি(mosquitto_psk_চাবি_get(context, psk_hint, identity, psk_চাবি, (পূর্ণাঙ্ক)max_psk_len*2) != MOSQ_ERR_SUCCESS){
		mosquitto_FREE(psk_চাবি);
		ঘূৰাই 0;
	}

	len = mosquitto__hex2bin(psk_চাবি, psk, (পূর্ণাঙ্ক)max_psk_len);
	যদি(len < 0){
		mosquitto_FREE(psk_চাবি);
		ঘূৰাই 0;
	}

	যদি(listener->use_identity_as_username){
		যদি(mosquitto_validate_utf8(identity, (পূর্ণাঙ্ক)strlen(identity))){
			mosquitto_free(psk_চাবি);
			ঘূৰাই 0;
		}
		context->username = mosquitto_strdup(identity);
		যদি(!context->username){
			mosquitto_FREE(psk_চাবি);
			ঘূৰাই 0;
		}
	}

	mosquitto_FREE(psk_চাবি);
	ঘূৰাই (unsigned পূর্ণাঙ্ক)len;
}
#endif

#ifdef WITH_TLS


স্থির শূন্য tls_keylog_callback(ধ্রুৱ SSL *ssl, ধ্রুৱ ৱর্ণ *সাৰি)
{
	UNUSED(ssl);

	যদি(db.tls_keylog){
		FILE *fptr;
		fptr = mosquitto_fopen(db.tls_keylog, "at", true);
		যদি(fptr){
#ifndef WIN32
			/* Until mosquitto_fopen enforces file permissions on all files,
			 * enforce it here. We can enforce it here, because it isn't a
			 * change of behaviour. */
			সংরচনা stat statbuf;
			যদি(fstat(fileno(fptr), &statbuf) < 0
					|| statbuf.st_mode & (S_IRWXG | S_IRWXO)){

				fclose(fptr);
				ঘূৰাই;
			}
#endif
			fprintf(fptr, "%s\n", সাৰি);
			fclose(fptr);
		}অন্যথা{
#ifndef WIN32
			যদি(errno == ELOOP){
				log__printf(রিক্ত, MOSQ_LOG_INFO, "Error: keylog file must not be a symbolic link");
			}অন্যথা
#endif
			{
				log__printf(রিক্ত, MOSQ_LOG_INFO, "Error: Unable to open keylog file: %s", strerror(errno));
			}
		}
	}
}


পূর্ণাঙ্ক net__tls_server_ctx(সংরচনা mosquitto__listener *listener)
{
	ৱর্ণ buf[256];
	পূর্ণাঙ্ক rc;

	যদি(tls_ex_index_context == -1){
		tls_ex_index_context = SSL_get_ex_new_index(0, "client context", রিক্ত, রিক্ত, রিক্ত);
	}
	যদি(tls_ex_index_listener == -1){
		tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", রিক্ত, রিক্ত, রিক্ত);
	}

	যদি(listener->ssl_ctx){
		SSL_CTX_free(listener->ssl_ctx);
		listener->ssl_ctx = রিক্ত;
	}

	listener->ssl_ctx = SSL_CTX_new(TLS_server_method());

	যদি(!listener->ssl_ctx){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to create TLS context.");
		ঘূৰাই MOSQ_ERR_TLS;
	}

#ifdef SSL_OP_NO_TLSv1_3
	যদি(db.config->per_listener_settings){
		যদি(listener->security_options->psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}অন্যথা{
		যদি(db.config->security_options.psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}
#endif

	যদি(listener->tls_version == রিক্ত){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
#ifdef SSL_OP_NO_TLSv1_3
	}অন্যথা যদি(!strcmp(listener->tls_version, "tlsv1.3")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2);
#endif
	}অন্যথা যদি(!strcmp(listener->tls_version, "tlsv1.2")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
	}অন্যথা যদি(!strcmp(listener->tls_version, "tlsv1.1")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1);
		log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: TLS v1.1 is insecure. It must only be used in the case "
				"where you have devices that cannot be upgraded to use a secure version of TLS. It is "
				"recommended to use as secure a set of ciphers as possible and that will restrict it to "
				"TLS v1.1 only, and to use a separate listener using TLS v1.2 and secure ciphers for your "
				"other devices.");
		log__printf(রিক্ত, MOSQ_LOG_WARNING, "Please be aware that support for TLS v1.1 will go away eventually "
				"and that you should plan now to migrate away from it.");
	}অন্যথা{
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unsupported tls_version \"%s\".", listener->tls_version);
		ঘূৰাই MOSQ_ERR_TLS;
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

	যদি(db.tls_keylog){
		log__printf(রিক্ত, MOSQ_LOG_NOTICE, "TLS key logging to '%s' enabled for all listeners.",
				db.tls_keylog);
		log__printf(রিক্ত, MOSQ_LOG_NOTICE, "TLS key logging is for DEBUGGING only.");

		SSL_CTX_set_keylog_callback(listener->ssl_ctx, tls_keylog_callback);
	}

	snprintf(buf, 256, "mosquitto-%d", listener->port);
	SSL_CTX_set_session_id_context(listener->ssl_ctx, (unsigned ৱর্ণ *)buf, (unsigned পূর্ণাঙ্ক)strlen(buf));

	যদি(listener->ciphers){
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, listener->ciphers);
		যদি(rc == 0){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			ঘূৰাই MOSQ_ERR_TLS;
		}
	}অন্যথা{
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, "DEFAULT:!aNULL:!eNULL:!LOW:!EXPORT:!SSLv2:@STRENGTH");
		যদি(rc == 0){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			ঘূৰাই MOSQ_ERR_TLS;
		}
	}
	যদি(listener->ciphers_tls13){
		rc = SSL_CTX_set_ciphersuites(listener->ssl_ctx, listener->ciphers_tls13);
		যদি(rc == 0){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to set TLS 1.3 ciphersuites. Check cipher_tls13 list \"%s\".", listener->ciphers_tls13);
			ঘূৰাই MOSQ_ERR_TLS;
		}
	}

	ঘূৰাই MOSQ_ERR_SUCCESS;
}
#endif


#ifdef WITH_TLS


স্থির পূর্ণাঙ্ক net__load_crl_file(সংরচনা mosquitto__listener *listener)
{
	X509_STORE *store;
	X509_LOOKUP *lookup;
	পূর্ণাঙ্ক rc;

	store = SSL_CTX_get_cert_store(listener->ssl_ctx);
	যদি(!store){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to obtain TLS store.");
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		ঘূৰাই MOSQ_ERR_TLS;
	}
	lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
	rc = X509_load_crl_file(lookup, listener->crlfile, X509_FILETYPE_PEM);
	যদি(rc < 1){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to load certificate revocation file \"%s\". Check crlfile.", listener->crlfile);
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		net__print_ssl_error(রিক্ত, রিক্ত);
		ঘূৰাই MOSQ_ERR_TLS;
	}
	X509_STORE_set_flags(store, X509_V_FLAG_CRL_CHECK);

	ঘূৰাই MOSQ_ERR_SUCCESS;
}
#endif


পূর্ণাঙ্ক net__load_certificates(সংরচনা mosquitto__listener *listener)
{
#ifdef WITH_TLS
	পূর্ণাঙ্ক rc;

	যদি(listener->require_certificate){
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, client_certificate_verify);
	}অন্যথা{
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_NONE, client_certificate_verify);
	}
	rc = SSL_CTX_use_certificate_chain_file(listener->ssl_ctx, listener->certfile);
	যদি(rc != 1){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
		net__print_ssl_error(রিক্ত, রিক্ত);
		ঘূৰাই MOSQ_ERR_TLS;
	}
	যদি(listener->tls_engine == রিক্ত || listener->tls_keyform == mosq_k_pem){
		rc = SSL_CTX_use_Private_চাবি_file(listener->ssl_ctx, listener->keyfile, SSL_FILETYPE_PEM);
		যদি(rc != 1){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			net__print_ssl_error(রিক্ত, রিক্ত);
			ঘূৰাই MOSQ_ERR_TLS;
		}
	}
	rc = SSL_CTX_check_private_চাবি(listener->ssl_ctx);
	যদি(rc != 1){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Server certificate/key are inconsistent.");
		net__print_ssl_error(রিক্ত, রিক্ত);
		ঘূৰাই MOSQ_ERR_TLS;
	}
	/* Load CRLs if they exist. */
	যদি(listener->crlfile){
		rc = net__load_crl_file(listener);
		যদি(rc){
			ঘূৰাই rc;
		}
	}
#অন্যথা
	UNUSED(listener);
#endif
	ঘূৰাই MOSQ_ERR_SUCCESS;
}


#যদি defined(WITH_TLS) && !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000


স্থির পূর্ণাঙ্ক net__load_engine(সংরচনা mosquitto__listener *listener)
{
	ENGINE *engine = রিক্ত;
	UI_METHOD *ui_method;
	EVP_PKEY *pkey;

	যদি(!listener->tls_engine){
		ঘূৰাই MOSQ_ERR_SUCCESS;
	}

	engine = ENGINE_by_id(listener->tls_engine);
	যদি(!engine){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error loading %s engine\n", listener->tls_engine);
		net__print_ssl_error(রিক্ত, রিক্ত);
		ঘূৰাই MOSQ_ERR_TLS;
	}
	যদি(!ENGINE_init(engine)){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Failed engine initialisation\n");
		net__print_ssl_error(রিক্ত, রিক্ত);
		ঘূৰাই MOSQ_ERR_TLS;
	}
	ENGINE_set_default(engine, ENGINE_METHOD_ALL);

	যদি(listener->tls_keyform == mosq_k_engine){
		ui_method = net__get_ui_method();
		যদি(listener->tls_engine_kpass_sha1){
			যদি(!ENGINE_ctrl_cmd(engine, ENGINE_SECRET_MODE, ENGINE_SECRET_MODE_SHA, রিক্ত, রিক্ত, 0)){
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to set engine secret mode sha");
				net__print_ssl_error(রিক্ত, রিক্ত);
				ঘূৰাই MOSQ_ERR_TLS;
			}
			যদি(!ENGINE_ctrl_cmd(engine, ENGINE_PIN, 0, listener->tls_engine_kpass_sha1, রিক্ত, 0)){
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to set engine pin");
				net__print_ssl_error(রিক্ত, রিক্ত);
				ঘূৰাই MOSQ_ERR_TLS;
			}
			ui_method = রিক্ত;
		}
		pkey = ENGINE_load_private_চাবি(engine, listener->keyfile, ui_method, রিক্ত);
		যদি(!pkey){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to load engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(রিক্ত, রিক্ত);
			ঘূৰাই MOSQ_ERR_TLS;
		}
		যদি(SSL_CTX_use_Private_চাবি(listener->ssl_ctx, pkey) <= 0){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to use engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(রিক্ত, রিক্ত);
			ঘূৰাই MOSQ_ERR_TLS;
		}
	}
	ENGINE_free(engine); /* release the structural reference from ENGINE_by_id() */

	ঘূৰাই MOSQ_ERR_SUCCESS;
}
#endif


পূর্ণাঙ্ক net__tls_load_verify(সংরচনা mosquitto__listener *listener)
{
#ifdef WITH_TLS
	পূর্ণাঙ্ক rc;

#  যদি OPENSSL_VERSION_সংখ্যা < 0x30000000L
	যদি(listener->cafile || listener->capath){
		rc = SSL_CTX_load_verify_locations(listener->ssl_ctx, listener->cafile, listener->capath);
		যদি(rc == 0){
			যদি(listener->cafile && listener->capath){
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\" and capath \"%s\".", listener->cafile, listener->capath);
			}অন্যথা যদি(listener->cafile){
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			}অন্যথা{
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			}
		}
	}
#  অন্যথা
	স্তূপ_OF(X509_NAME) *ca_names = রিক্ত;
	যদি(listener->cafile || listener->capath){
		ca_names = sk_X509_NAME_new_null();
	}

	যদি(listener->cafile){
		rc = SSL_CTX_load_verify_file(listener->ssl_ctx, listener->cafile);
		যদি(rc == 0){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			net__print_ssl_error(রিক্ত, রিক্ত);
			ঘূৰাই MOSQ_ERR_TLS;
		}

		স্তূপ_OF(X509_NAME) *cert_names = SSL_load_client_CA_file(listener->cafile);
		যদি(cert_names){
			বাবে(পূর্ণাঙ্ক i=0; i<sk_X509_NAME_অঙ্ক(cert_names); i++){
				sk_X509_NAME_ঠেলো(ca_names, X509_NAME_dup(sk_X509_NAME_মান(cert_names, i)));
			}
			sk_X509_NAME_বেৰ_কৰো_free(cert_names, X509_NAME_free);
		}

	}
	যদি(listener->capath){
		rc = SSL_CTX_load_verify_dir(listener->ssl_ctx, listener->capath);
		যদি(rc == 0){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			net__print_ssl_error(রিক্ত, রিক্ত);
			sk_X509_NAME_বেৰ_কৰো_free(ca_names, X509_NAME_free);
			ঘূৰাই MOSQ_ERR_TLS;
		}

		SSL_add_dir_cert_subjects_to_স্তূপ(ca_names, listener->capath);
	}
	যদি(ca_names){
		SSL_CTX_set_client_CA_list(listener->ssl_ctx, ca_names);
	}
#  endif

#  যদি !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000
	যদি(net__load_engine(listener)){
		ঘূৰাই MOSQ_ERR_TLS;
	}
#  endif
#endif
	ঘূৰাই net__load_certificates(listener);
}


#যদি !defined(WIN32) && !defined(_AIX)


স্থির পূর্ণাঙ্ক net__bind_interface(সংরচনা mosquitto__listener *listener, সংরচনা addrinfo *rp)
{
	/*
	 * This binds the listener sock to a network interface.
	 * The use of SO_BINDTODEVICE requires root access, which we don't have, so instead
	 * use getifaddrs to find the interface addresses, and use IP of the
	 * matching interface in the later bind().
	 */
	সংরচনা ifaddrs *ifaddr;
	bool have_interface = false;

	যদি(getifaddrs(&ifaddr) < 0){
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		ঘূৰাই MOSQ_ERR_ERRNO;
	}

	বাবে(সংরচনা ifaddrs *ifa=ifaddr; ifa!=রিক্ত; ifa=ifa->ifa_next){
		যদি(ifa->ifa_addr == রিক্ত){
			অনুৱর্ত;
		}

		যদি(!strcasecmp(listener->bind_interface, ifa->ifa_name)){
			have_interface = true;

			যদি(ifa->ifa_addr->sa_family == rp->ai_addr->sa_family){
				যদি(rp->ai_addr->sa_family == AF_INET){
					যদি(listener->host &&
							memcmp(&((সংরচনা sockaddr_in *)rp->ai_addr)->sin_addr,
							&((সংরচনা sockaddr_in *)ifa->ifa_addr)->sin_addr,
							পরিমাণ(সংরচনা in_addr))){

						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						ঘূৰাই MOSQ_ERR_INVAL;
					}অন্যথা{
						memcpy(&((সংরচনা sockaddr_in *)rp->ai_addr)->sin_addr,
								&((সংরচনা sockaddr_in *)ifa->ifa_addr)->sin_addr,
								পরিমাণ(সংরচনা in_addr));

						freeifaddrs(ifaddr);
						ঘূৰাই MOSQ_ERR_SUCCESS;
					}
				}অন্যথা যদি(rp->ai_addr->sa_family == AF_INET6){
					যদি(listener->host &&
							memcmp(&((সংরচনা sockaddr_in6 *)rp->ai_addr)->sin6_addr,
							&((সংরচনা sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
							পরিমাণ(সংরচনা in6_addr))){

						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						ঘূৰাই MOSQ_ERR_INVAL;
					}অন্যথা{
						memcpy(&((সংরচনা sockaddr_in6 *)rp->ai_addr)->sin6_addr,
								&((সংরচনা sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
								পরিমাণ(সংরচনা in6_addr));

						((সংরচনা sockaddr_in6 *)rp->ai_addr)->sin6_scope_id = ((সংরচনা sockaddr_in6 *)ifa->ifa_addr)->sin6_scope_id;

						freeifaddrs(ifaddr);
						ঘূৰাই MOSQ_ERR_SUCCESS;
					}
				}
			}
		}
	}
	freeifaddrs(ifaddr);
	যদি(have_interface){
		log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Interface %s does not support %s configuration.",
				listener->bind_interface, rp->ai_addr->sa_family == AF_INET ? "IPv4" : "IPv6");
		ঘূৰাই MOSQ_ERR_NOT_SUPPORTED;
	}অন্যথা{
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Interface %s does not exist.", listener->bind_interface);
		ঘূৰাই MOSQ_ERR_NOT_FOUND;
	}
}
#endif


স্থির পূর্ণাঙ্ক net__socket_listen_tcp(সংরচনা mosquitto__listener *listener)
{
	mosq_sock_t sock = INVALID_SOCKET;
	সংরচনা addrinfo hints;
	সংরচনা addrinfo *ainfo, *rp;
	ৱর্ণ service[10];
	পূর্ণাঙ্ক rc;
	পূর্ণাঙ্ক ss_opt = 1;
#ifndef WIN32
	bool interface_bound = false;
#endif

	যদি(!listener){
		ঘূৰাই MOSQ_ERR_INVAL;
	}

	snprintf(service, 10, "%d", listener->port);
	memset(&hints, 0, পরিমাণ(সংরচনা addrinfo));
	যদি(listener->socket_domain){
		hints.ai_family = listener->socket_domain;
	}অন্যথা{
		hints.ai_family = AF_UNSPEC;
	}
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	rc = getaddrinfo(listener->host, service, &hints, &ainfo);
	যদি(rc){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error creating listener: %s.", gai_strerror(rc));
		ঘূৰাই INVALID_SOCKET;
	}

	listener->sock_গুনতি = 0;
	listener->socks = রিক্ত;

	বাবে(rp = ainfo; rp; rp = rp->ai_next){
		যদি(rp->ai_family == AF_INET){
			log__printf(রিক্ত, MOSQ_LOG_INFO, "Opening ipv4 listen socket on port %d.", ntohs(((সংরচনা sockaddr_in *)rp->ai_addr)->sin_port));
		}অন্যথা যদি(rp->ai_family == AF_INET6){
			log__printf(রিক্ত, MOSQ_LOG_INFO, "Opening ipv6 listen socket on port %d.", ntohs(((সংরচনা sockaddr_in6 *)rp->ai_addr)->sin6_port));
		}অন্যথা{
			অনুৱর্ত;
		}

		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		যদি(sock == INVALID_SOCKET){
			net__print_error(MOSQ_LOG_WARNING, "Warning: %s");
			অনুৱর্ত;
		}
		listener->sock_গুনতি++;
		listener->socks = mosquitto_realloc(listener->socks, পরিমাণ(mosq_sock_t)*(size_t)listener->sock_গুনতি);
		যদি(!listener->socks){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			ঘূৰাই MOSQ_ERR_NOMEM;
		}
		listener->socks[listener->sock_গুনতি-1] = sock;

#ifndef WIN32
		ss_opt = 1;
		/* Unimportant if this fails */
		(শূন্য)setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (ধ্রুৱ শূন্য *)&ss_opt, পরিমাণ(ss_opt));
#endif
#ifdef IPV6_V6ONLY
		ss_opt = 1;
		(শূন্য)setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (ধ্রুৱ শূন্য *)&ss_opt, পরিমাণ(ss_opt));
#endif

		যদি(net__socket_nonblock(&sock)){
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			ঘূৰাই 1;
		}

#যদি !defined(WIN32) && !defined(_AIX)
		যদি(listener->bind_interface){
			/* It might be possible that an interface does not support all relevant sa_families.
			 * We should successfully find at least one. */
			rc = net__bind_interface(listener, rp);
			যদি(rc){
				COMPAT_CLOSE(sock);
				listener->sock_গুনতি--;
				যদি(rc == MOSQ_ERR_NOT_FOUND || rc == MOSQ_ERR_INVAL){
					freeaddrinfo(ainfo);
					ঘূৰাই rc;
				}অন্যথা{
					অনুৱর্ত;
				}
			}
			interface_bound = true;
		}
#endif

		যদি(bind(sock, rp->ai_addr, rp->ai_addrlen) == -1){
#যদি defined(__linux__)
			যদি(errno == EACCES){
				log__printf(রিক্ত, MOSQ_LOG_ERR, "If you are trying to bind to a privileged port (<1024), try using setcap and do not start the broker as root:");
				log__printf(রিক্ত, MOSQ_LOG_ERR, "    sudo setcap 'CAP_NET_BIND_SERVICE=+ep /usr/sbin/mosquitto'");
			}
#endif
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			COMPAT_CLOSE(sock);
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			ঘূৰাই 1;
		}

		যদি(listen(sock, 100) == -1){
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			mosquitto_FREE(listener->socks);
			ঘূৰাই 1;
		}
	}
	freeaddrinfo(ainfo);

#ifndef WIN32
	যদি(listener->bind_interface && !interface_bound){
		mosquitto_FREE(listener->socks);
		ঘূৰাই 1;
	}
#endif

	ঘূৰাই 0;
}


#ifdef WITH_UNIX_SOCKETS


স্থির পূর্ণাঙ্ক net__socket_listen_unix(সংরচনা mosquitto__listener *listener)
{
	সংরচনা sockaddr_un addr;
	mosq_sock_t sock = INVALID_SOCKET;
	পূর্ণাঙ্ক rc;
#ifndef WIN32
	mode_t old_mask;
#endif

	যদি(listener->unix_socket_path == রিক্ত){
		ঘূৰাই MOSQ_ERR_INVAL;
	}
	যদি(strlen(listener->unix_socket_path) > পরিমাণ(addr.sun_path)-1){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Path to unix socket is too long \"%s\".", listener->unix_socket_path);
		ঘূৰাই MOSQ_ERR_INVAL;
	}

#ifdef WIN32
	DeleteFile(listener->unix_socket_path);
#অন্যথা
	unlink(listener->unix_socket_path);
#endif
	log__printf(রিক্ত, MOSQ_LOG_INFO, "Opening unix listen socket on path %s.", listener->unix_socket_path);
	memset(&addr, 0, পরিমাণ(সংরচনা sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, listener->unix_socket_path, পরিমাণ(addr.sun_path)-1);

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	যদি(sock == INVALID_SOCKET){
		net__print_error(MOSQ_LOG_ERR, "Error creating unix socket: %s");
		ঘূৰাই 1;
	}
	listener->sock_গুনতি++;
	listener->socks = mosquitto_realloc(listener->socks, পরিমাণ(mosq_sock_t)*(size_t)listener->sock_গুনতি);
	যদি(!listener->socks){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
		COMPAT_CLOSE(sock);
		ঘূৰাই MOSQ_ERR_NOMEM;
	}
	listener->socks[listener->sock_গুনতি-1] = sock;


#ifndef WIN32
	old_mask = umask(0007);
#endif
	rc = bind(sock, (সংরচনা sockaddr *)&addr, পরিমাণ(সংরচনা sockaddr_un));
#ifndef WIN32
	umask(old_mask);
#endif

	যদি(rc == -1){
		net__print_error(MOSQ_LOG_ERR, "Error binding unix socket: %s");
		ঘূৰাই 1;
	}

	যদি(listen(sock, 10) == -1){
		net__print_error(MOSQ_LOG_ERR, "Error listening to unix socket: %s");
		ঘূৰাই 1;
	}

	যদি(net__socket_nonblock(&sock)){
		ঘূৰাই 1;
	}

	ঘূৰাই 0;
}
#endif


/* Creates a socket and listens on port 'port'.
 * Returns 1 on failure
 * Returns 0 on success.
 */
পূর্ণাঙ্ক net__socket_listen(সংরচনা mosquitto__listener *listener)
{
	পূর্ণাঙ্ক rc;

	যদি(!listener){
		ঘূৰাই MOSQ_ERR_INVAL;
	}

#ifdef WITH_UNIX_SOCKETS
	যদি(listener->port == 0 && listener->unix_socket_path != রিক্ত){
		rc = net__socket_listen_unix(listener);
	}অন্যথা
#endif
	{
		rc = net__socket_listen_tcp(listener);
	}
	যদি(rc){
		ঘূৰাই rc;
	}

	/* We need to have at least one working socket. */
	যদি(listener->sock_গুনতি > 0){
#ifdef WITH_TLS
		যদি(listener->certfile && listener->keyfile){
			যদি(net__tls_server_ctx(listener)){
				ঘূৰাই 1;
			}

			যদি(net__tls_load_verify(listener)){
				ঘূৰাই 1;
			}
		}
#  ifdef FINAL_WITH_TLS_PSK
		যদি(listener->psk_hint){
			যদি(listener->certfile == রিক্ত || listener->keyfile == রিক্ত){
				যদি(net__tls_server_ctx(listener)){
					ঘূৰাই 1;
				}
			}
			SSL_CTX_set_psk_server_callback(listener->ssl_ctx, psk_server_callback);
			যদি(listener->psk_hint){
				rc = SSL_CTX_use_psk_identity_hint(listener->ssl_ctx, listener->psk_hint);
				যদি(rc == 0){
					log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to set TLS PSK hint.");
					net__print_ssl_error(রিক্ত, রিক্ত);
					ঘূৰাই 1;
				}
			}
		}
#  endif /* FINAL_WITH_TLS_PSK */
		যদি(tls_ex_index_context == -1){
			tls_ex_index_context = SSL_get_ex_new_index(0, "client context", রিক্ত, রিক্ত, রিক্ত);
		}
		যদি(tls_ex_index_listener == -1){
			tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", রিক্ত, রিক্ত, রিক্ত);
		}
#endif /* WITH_TLS */
		ঘূৰাই 0;
	}অন্যথা{
		ঘূৰাই 1;
	}
}


পূর্ণাঙ্ক net__socket_get_address(mosq_sock_t sock, ৱর্ণ *buf, size_t len, uint16_t *remote_port)
{
	সংরচনা sockaddr_storage addr;
	socklen_t addrlen;

	memset(&addr, 0, পরিমাণ(সংরচনা sockaddr_storage));
	addrlen = পরিমাণ(addr);
	যদি(!getpeername(sock, (সংরচনা sockaddr *)&addr, &addrlen)){
		যদি(addr.ss_family == AF_INET){
			যদি(remote_port){
				*remote_port = ntohs(((সংরচনা sockaddr_in *)&addr)->sin_port);
			}
			যদি(inet_ntop(AF_INET, &((সংরচনা sockaddr_in *)&addr)->sin_addr.s_addr, buf, (socklen_t)len)){
				ঘূৰাই 0;
			}
		}অন্যথা যদি(addr.ss_family == AF_INET6){
			যদি(remote_port){
				*remote_port = ntohs(((সংরচনা sockaddr_in6 *)&addr)->sin6_port);
			}
			যদি(inet_ntop(AF_INET6, &((সংরচনা sockaddr_in6 *)&addr)->sin6_addr.s6_addr, buf, (socklen_t)len)){
				ঘূৰাই 0;
			}
#ifdef WITH_UNIX_SOCKETS
		}অন্যথা যদি(addr.ss_family == AF_UNIX){
			সংরচনা sockaddr_un un;
			addrlen = পরিমাণ(সংরচনা sockaddr_un);
			যদি(!getsockname(sock, (সংরচনা sockaddr *)&un, &addrlen)){
				snprintf(buf, len, "%s", un.sun_path);
			}অন্যথা{
				snprintf(buf, len, "unix-socket");
			}
			ঘূৰাই 0;
#endif
		}
	}
	ঘূৰাই 1;
}
