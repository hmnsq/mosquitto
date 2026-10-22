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

#অন্তর্ভাব "config.h"

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

#অন্তর্ভাব <errno.h>
#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>
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

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto/mqtt_protocol.h"
#অন্তর্ভাব "net_mosq.h"
#অন্তর্ভাব "util_mosq.h"

#ifdef WITH_TLS
#  include "tls_mosq.h"
#  include <openssl/err.h>
স্থির পূর্ণসংখ্যা tls_ex_সূচক_context = -1;
স্থির পূর্ণসংখ্যা tls_ex_সূচক_listener = -1;
#endif

#অন্তর্ভাব "sys_tree.h"

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


স্থির শূন্য net__print_error(unsigned পূর্ণসংখ্যা log, ধ্রুবক অক্ষর *format_str)
{
	অক্ষর *buf;

#ifdef WIN32
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_বাফার | FORMAT_MESSAGE_FROM_SYSTEM,
			ফাঁকা, WSAGetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, ফাঁকা);

	log__printf(ফাঁকা, log, format_str, buf);
	LocalFree(buf);
#অন্যথা
	buf = strerror(errno);
	log__printf(ফাঁকা, log, format_str, buf);
#endif
}


কাঠামো mosquitto *net__socket_accept(কাঠামো mosquitto__listener_sock *listensock)
{
	mosq_sock_t new_sock = INVALID_SOCKET;
	কাঠামো mosquitto *new_context;
#ifdef WITH_TLS
	BIO *bio;
#endif
#ifdef WITH_WRAP
	কাঠামো request_info wrap_req;
	অক্ষর address[1024];
#endif

	new_sock = accept(listensock->sock, ফাঁকা, 0);
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
			new_sock = accept(listensock->sock, ফাঁকা, 0);
			যদি(new_sock != INVALID_SOCKET){
				COMPAT_CLOSE(new_sock);
			}
			spare_sock = socket(AF_INET, SOCK_STREAM, 0);
			log__printf(ফাঁকা, MOSQ_LOG_WARNING,
					"Unable to accept new connection, system socket count has been exceeded. Try increasing \"ulimit -n\" or equivalent.");
		}
		ফেরত ফাঁকা;
	}

	metrics__int_inc(mosq_গণক_socket_connections, 1);

	যদি(net__socket_nonblock(&new_sock)){
		ফেরত ফাঁকা;
	}

#ifdef WITH_WRAP
	/* Use tcpd / libwrap to determine whether a connection is allowed. */
	request_init(&wrap_req, RQ_FILE, new_sock, RQ_DAEMON, "mosquitto", 0);
	fromhost(&wrap_req);
	যদি(!hosts_access(&wrap_req)){
		/* Access is denied */
		যদি(db.config->connection_messages == true){
			যদি(!net__socket_get_address(new_sock, address, 1024, ফাঁকা)){
				log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client connection from %s denied access by tcpd.", address);
			}
		}
		COMPAT_CLOSE(new_sock);
		ফেরত ফাঁকা;
	}
#endif

	যদি(db.config->set_tcp_nodelay && listensock->listener->port){
		পূর্ণসংখ্যা flag = 1;
		যদি(setsockopt(new_sock, IPPROTO_TCP, TCP_NODELAY, (ধ্রুবক শূন্য *)&flag, আকার(পূর্ণসংখ্যা)) != 0){
			log__printf(ফাঁকা, MOSQ_LOG_WARNING, "Warning: Unable to set TCP_NODELAY.");
		}
	}

	new_context = context__init();
	যদি(!new_context){
		COMPAT_CLOSE(new_sock);
		ফেরত ফাঁকা;
	}
	new_context->listener = listensock->listener;
	যদি(!new_context->listener){
		context__cleanup(new_context, true);
		ফেরত ফাঁকা;
	}
	new_context->listener->client_গুনতি++;

	যদি(new_context->listener->enable_proxy_protocol){
		যদি(context__init_sock(new_context, new_sock, false) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			ফেরত ফাঁকা;
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
			ফেরত ফাঁকা;
		}
		নির্বাচন(new_context->listener->protocol){
			ক্ষেত্র mp_mqtt:
				new_context->transport = mosq_t_tcp;
				বিরতি;
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			ক্ষেত্র mp_websockets:
				যদি(http__context_init(new_context)){
					context__cleanup(new_context, true);
					ফেরত ফাঁকা;
				}
				বিরতি;
#endif
			পূর্বনির্ধারিত:
				context__cleanup(new_context, true);
				ফেরত ফাঁকা;
		}
	}

	যদি((new_context->listener->max_connections > 0 && new_context->listener->client_গুনতি > new_context->listener->max_connections)
			|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (unsigned পূর্ণসংখ্যা)db.config->global_max_connections)){

		যদি(db.config->connection_messages == true){
			log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", new_context->address);
		}
		context__cleanup(new_context, true);
		ফেরত ফাঁকা;
	}

#ifdef WITH_TLS
	/* TLS init */
	যদি(new_context->listener->ssl_ctx){
		new_context->ssl = SSL_new(new_context->listener->ssl_ctx);
		যদি(!new_context->ssl){
			context__cleanup(new_context, true);
			ফেরত ফাঁকা;
		}
		যদি(!SSL_set_ex_তথ্য(new_context->ssl, tls_ex_সূচক_context, new_context)
				|| !SSL_set_ex_তথ্য(new_context->ssl, tls_ex_সূচক_listener, new_context->listener)){
			context__cleanup(new_context, true);
			ফেরত ফাঁকা;
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

		log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "New connection from %s:%d on port %d.",
				new_context->address, new_context->remote_port, new_context->listener->port);
	}

	mux__new(new_context);
	keepalive__add(new_context);

	ফেরত new_context;
}

#ifdef WITH_TLS


স্থির পূর্ণসংখ্যা client_certificate_verify(পূর্ণসংখ্যা preverify_ok, X509_STORE_CTX *ctx)
{
	/* Preverify should check expiry, revocation. */
	যদি(preverify_ok == 0){
		SSL *ssl;
		ssl = X509_STORE_CTX_get_ex_তথ্য(ctx, SSL_get_ex_তথ্য_X509_STORE_CTX_idx());
		যদি(ssl){
			কাঠামো mosquitto__listener *listener;
			listener = SSL_get_ex_তথ্য(ssl, tls_ex_সূচক_listener);

			যদি(listener && listener->disable_client_cert_date_checks){
				পূর্ণসংখ্যা err = X509_STORE_CTX_get_error(ctx);
				যদি(err == X509_V_ERR_CERT_NOT_YET_VALID || err == X509_V_ERR_CERT_HAS_EXPIRED){
					preverify_ok = 1;
				}
			}
		}
	}
	ফেরত preverify_ok;
}
#endif

#ifdef FINAL_WITH_TLS_PSK


স্থির unsigned পূর্ণসংখ্যা psk_server_callback(SSL *ssl, ধ্রুবক অক্ষর *identity, unsigned অক্ষর *psk, unsigned পূর্ণসংখ্যা max_psk_len)
{
	কাঠামো mosquitto *context;
	কাঠামো mosquitto__listener *listener;
	অক্ষর *psk_চাবি = ফাঁকা;
	পূর্ণসংখ্যা len;
	ধ্রুবক অক্ষর *psk_hint;

	যদি(!identity){
		ফেরত 0;
	}

	context = SSL_get_ex_তথ্য(ssl, tls_ex_সূচক_context);
	যদি(!context){
		ফেরত 0;
	}

	listener = SSL_get_ex_তথ্য(ssl, tls_ex_সূচক_listener);
	যদি(!listener){
		ফেরত 0;
	}

	psk_hint = listener->psk_hint;

	/* The hex to BN conversion results in the length halving, so we can pass
	 * max_psk_len*2 as the max hex key here. */
	psk_চাবি = mosquitto_calloc(1, (size_t)max_psk_len*2 + 1);
	যদি(!psk_চাবি){
		ফেরত 0;
	}

	যদি(mosquitto_psk_চাবি_get(context, psk_hint, identity, psk_চাবি, (পূর্ণসংখ্যা)max_psk_len*2) != MOSQ_ERR_SUCCESS){
		mosquitto_FREE(psk_চাবি);
		ফেরত 0;
	}

	len = mosquitto__hex2bin(psk_চাবি, psk, (পূর্ণসংখ্যা)max_psk_len);
	যদি(len < 0){
		mosquitto_FREE(psk_চাবি);
		ফেরত 0;
	}

	যদি(listener->use_identity_as_username){
		যদি(mosquitto_validate_utf8(identity, (পূর্ণসংখ্যা)strlen(identity))){
			mosquitto_free(psk_চাবি);
			ফেরত 0;
		}
		context->username = mosquitto_strdup(identity);
		যদি(!context->username){
			mosquitto_FREE(psk_চাবি);
			ফেরত 0;
		}
	}

	mosquitto_FREE(psk_চাবি);
	ফেরত (unsigned পূর্ণসংখ্যা)len;
}
#endif

#ifdef WITH_TLS


স্থির শূন্য tls_keylog_callback(ধ্রুবক SSL *ssl, ধ্রুবক অক্ষর *সারি)
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
			কাঠামো stat statbuf;
			যদি(fstat(fileno(fptr), &statbuf) < 0
					|| statbuf.st_mode & (S_IRWXG | S_IRWXO)){

				fclose(fptr);
				ফেরত;
			}
#endif
			fprintf(fptr, "%s\n", সারি);
			fclose(fptr);
		}অন্যথা{
#ifndef WIN32
			যদি(errno == ELOOP){
				log__printf(ফাঁকা, MOSQ_LOG_INFO, "Error: keylog file must not be a symbolic link");
			}অন্যথা
#endif
			{
				log__printf(ফাঁকা, MOSQ_LOG_INFO, "Error: Unable to open keylog file: %s", strerror(errno));
			}
		}
	}
}


পূর্ণসংখ্যা net__tls_server_ctx(কাঠামো mosquitto__listener *listener)
{
	অক্ষর buf[256];
	পূর্ণসংখ্যা rc;

	যদি(tls_ex_সূচক_context == -1){
		tls_ex_সূচক_context = SSL_get_ex_new_সূচক(0, "client context", ফাঁকা, ফাঁকা, ফাঁকা);
	}
	যদি(tls_ex_সূচক_listener == -1){
		tls_ex_সূচক_listener = SSL_get_ex_new_সূচক(0, "listener", ফাঁকা, ফাঁকা, ফাঁকা);
	}

	যদি(listener->ssl_ctx){
		SSL_CTX_free(listener->ssl_ctx);
		listener->ssl_ctx = ফাঁকা;
	}

	listener->ssl_ctx = SSL_CTX_new(TLS_server_method());

	যদি(!listener->ssl_ctx){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to create TLS context.");
		ফেরত MOSQ_ERR_TLS;
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

	যদি(listener->tls_version == ফাঁকা){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
#ifdef SSL_OP_NO_TLSv1_3
	}অন্যথা যদি(!strcmp(listener->tls_version, "tlsv1.3")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2);
#endif
	}অন্যথা যদি(!strcmp(listener->tls_version, "tlsv1.2")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
	}অন্যথা যদি(!strcmp(listener->tls_version, "tlsv1.1")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1);
		log__printf(ফাঁকা, MOSQ_LOG_WARNING, "Warning: TLS v1.1 is insecure. It must only be used in the case "
				"where you have devices that cannot be upgraded to use a secure version of TLS. It is "
				"recommended to use as secure a set of ciphers as possible and that will restrict it to "
				"TLS v1.1 only, and to use a separate listener using TLS v1.2 and secure ciphers for your "
				"other devices.");
		log__printf(ফাঁকা, MOSQ_LOG_WARNING, "Please be aware that support for TLS v1.1 will go away eventually "
				"and that you should plan now to migrate away from it.");
	}অন্যথা{
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unsupported tls_version \"%s\".", listener->tls_version);
		ফেরত MOSQ_ERR_TLS;
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
		log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "TLS key logging to '%s' enabled for all listeners.",
				db.tls_keylog);
		log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "TLS key logging is for DEBUGGING only.");

		SSL_CTX_set_keylog_callback(listener->ssl_ctx, tls_keylog_callback);
	}

	snprintf(buf, 256, "mosquitto-%d", listener->port);
	SSL_CTX_set_session_id_context(listener->ssl_ctx, (unsigned অক্ষর *)buf, (unsigned পূর্ণসংখ্যা)strlen(buf));

	যদি(listener->ciphers){
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, listener->ciphers);
		যদি(rc == 0){
			log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			ফেরত MOSQ_ERR_TLS;
		}
	}অন্যথা{
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, "DEFAULT:!aNULL:!eNULL:!LOW:!EXPORT:!SSLv2:@STRENGTH");
		যদি(rc == 0){
			log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			ফেরত MOSQ_ERR_TLS;
		}
	}
	যদি(listener->ciphers_tls13){
		rc = SSL_CTX_set_ciphersuites(listener->ssl_ctx, listener->ciphers_tls13);
		যদি(rc == 0){
			log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to set TLS 1.3 ciphersuites. Check cipher_tls13 list \"%s\".", listener->ciphers_tls13);
			ফেরত MOSQ_ERR_TLS;
		}
	}

	ফেরত MOSQ_ERR_SUCCESS;
}
#endif


#ifdef WITH_TLS


স্থির পূর্ণসংখ্যা net__load_crl_file(কাঠামো mosquitto__listener *listener)
{
	X509_STORE *store;
	X509_LOOKUP *lookup;
	পূর্ণসংখ্যা rc;

	store = SSL_CTX_get_cert_store(listener->ssl_ctx);
	যদি(!store){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to obtain TLS store.");
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		ফেরত MOSQ_ERR_TLS;
	}
	lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
	rc = X509_load_crl_file(lookup, listener->crlfile, X509_FILETYPE_PEM);
	যদি(rc < 1){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to load certificate revocation file \"%s\". Check crlfile.", listener->crlfile);
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		net__print_ssl_error(ফাঁকা, ফাঁকা);
		ফেরত MOSQ_ERR_TLS;
	}
	X509_STORE_set_flags(store, X509_V_FLAG_CRL_CHECK);

	ফেরত MOSQ_ERR_SUCCESS;
}
#endif


পূর্ণসংখ্যা net__load_certificates(কাঠামো mosquitto__listener *listener)
{
#ifdef WITH_TLS
	পূর্ণসংখ্যা rc;

	যদি(listener->require_certificate){
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, client_certificate_verify);
	}অন্যথা{
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_NONE, client_certificate_verify);
	}
	rc = SSL_CTX_use_certificate_chain_file(listener->ssl_ctx, listener->certfile);
	যদি(rc != 1){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
		net__print_ssl_error(ফাঁকা, ফাঁকা);
		ফেরত MOSQ_ERR_TLS;
	}
	যদি(listener->tls_engine == ফাঁকা || listener->tls_keyform == mosq_k_pem){
		rc = SSL_CTX_use_Private_চাবি_file(listener->ssl_ctx, listener->keyfile, SSL_FILETYPE_PEM);
		যদি(rc != 1){
			log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			net__print_ssl_error(ফাঁকা, ফাঁকা);
			ফেরত MOSQ_ERR_TLS;
		}
	}
	rc = SSL_CTX_check_private_চাবি(listener->ssl_ctx);
	যদি(rc != 1){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Server certificate/key are inconsistent.");
		net__print_ssl_error(ফাঁকা, ফাঁকা);
		ফেরত MOSQ_ERR_TLS;
	}
	/* Load CRLs if they exist. */
	যদি(listener->crlfile){
		rc = net__load_crl_file(listener);
		যদি(rc){
			ফেরত rc;
		}
	}
#অন্যথা
	UNUSED(listener);
#endif
	ফেরত MOSQ_ERR_SUCCESS;
}


#যদি defined(WITH_TLS) && !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000


স্থির পূর্ণসংখ্যা net__load_engine(কাঠামো mosquitto__listener *listener)
{
	ENGINE *engine = ফাঁকা;
	UI_METHOD *ui_method;
	EVP_PKEY *pkey;

	যদি(!listener->tls_engine){
		ফেরত MOSQ_ERR_SUCCESS;
	}

	engine = ENGINE_by_id(listener->tls_engine);
	যদি(!engine){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error loading %s engine\n", listener->tls_engine);
		net__print_ssl_error(ফাঁকা, ফাঁকা);
		ফেরত MOSQ_ERR_TLS;
	}
	যদি(!ENGINE_init(engine)){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Failed engine initialisation\n");
		net__print_ssl_error(ফাঁকা, ফাঁকা);
		ফেরত MOSQ_ERR_TLS;
	}
	ENGINE_set_default(engine, ENGINE_METHOD_ALL);

	যদি(listener->tls_keyform == mosq_k_engine){
		ui_method = net__get_ui_method();
		যদি(listener->tls_engine_kpass_sha1){
			যদি(!ENGINE_ctrl_cmd(engine, ENGINE_SECRET_MODE, ENGINE_SECRET_MODE_SHA, ফাঁকা, ফাঁকা, 0)){
				log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to set engine secret mode sha");
				net__print_ssl_error(ফাঁকা, ফাঁকা);
				ফেরত MOSQ_ERR_TLS;
			}
			যদি(!ENGINE_ctrl_cmd(engine, ENGINE_PIN, 0, listener->tls_engine_kpass_sha1, ফাঁকা, 0)){
				log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to set engine pin");
				net__print_ssl_error(ফাঁকা, ফাঁকা);
				ফেরত MOSQ_ERR_TLS;
			}
			ui_method = ফাঁকা;
		}
		pkey = ENGINE_load_private_চাবি(engine, listener->keyfile, ui_method, ফাঁকা);
		যদি(!pkey){
			log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to load engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(ফাঁকা, ফাঁকা);
			ফেরত MOSQ_ERR_TLS;
		}
		যদি(SSL_CTX_use_Private_চাবি(listener->ssl_ctx, pkey) <= 0){
			log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to use engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(ফাঁকা, ফাঁকা);
			ফেরত MOSQ_ERR_TLS;
		}
	}
	ENGINE_free(engine); /* release the structural reference from ENGINE_by_id() */

	ফেরত MOSQ_ERR_SUCCESS;
}
#endif


পূর্ণসংখ্যা net__tls_load_verify(কাঠামো mosquitto__listener *listener)
{
#ifdef WITH_TLS
	পূর্ণসংখ্যা rc;

#  যদি OPENSSL_VERSION_সংখ্যা < 0x30000000L
	যদি(listener->cafile || listener->capath){
		rc = SSL_CTX_load_verify_locations(listener->ssl_ctx, listener->cafile, listener->capath);
		যদি(rc == 0){
			যদি(listener->cafile && listener->capath){
				log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\" and capath \"%s\".", listener->cafile, listener->capath);
			}অন্যথা যদি(listener->cafile){
				log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			}অন্যথা{
				log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			}
		}
	}
#  অন্যথা
	স্তূপ_OF(X509_NAME) *ca_names = ফাঁকা;
	যদি(listener->cafile || listener->capath){
		ca_names = sk_X509_NAME_new_null();
	}

	যদি(listener->cafile){
		rc = SSL_CTX_load_verify_file(listener->ssl_ctx, listener->cafile);
		যদি(rc == 0){
			log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			net__print_ssl_error(ফাঁকা, ফাঁকা);
			ফেরত MOSQ_ERR_TLS;
		}

		স্তূপ_OF(X509_NAME) *cert_names = SSL_load_client_CA_file(listener->cafile);
		যদি(cert_names){
			জন্য(পূর্ণসংখ্যা i=0; i<sk_X509_NAME_অঙ্ক(cert_names); i++){
				sk_X509_NAME_ঠেলো(ca_names, X509_NAME_dup(sk_X509_NAME_মান(cert_names, i)));
			}
			sk_X509_NAME_বের_করো_free(cert_names, X509_NAME_free);
		}

	}
	যদি(listener->capath){
		rc = SSL_CTX_load_verify_dir(listener->ssl_ctx, listener->capath);
		যদি(rc == 0){
			log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			net__print_ssl_error(ফাঁকা, ফাঁকা);
			sk_X509_NAME_বের_করো_free(ca_names, X509_NAME_free);
			ফেরত MOSQ_ERR_TLS;
		}

		SSL_add_dir_cert_subjects_to_স্তূপ(ca_names, listener->capath);
	}
	যদি(ca_names){
		SSL_CTX_set_client_CA_list(listener->ssl_ctx, ca_names);
	}
#  endif

#  যদি !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000
	যদি(net__load_engine(listener)){
		ফেরত MOSQ_ERR_TLS;
	}
#  endif
#endif
	ফেরত net__load_certificates(listener);
}


#যদি !defined(WIN32) && !defined(_AIX)


স্থির পূর্ণসংখ্যা net__bind_interface(কাঠামো mosquitto__listener *listener, কাঠামো addrinfo *rp)
{
	/*
	 * This binds the listener sock to a network interface.
	 * The use of SO_BINDTODEVICE requires root access, which we don't have, so instead
	 * use getifaddrs to find the interface addresses, and use IP of the
	 * matching interface in the later bind().
	 */
	কাঠামো ifaddrs *ifaddr;
	bool have_interface = false;

	যদি(getifaddrs(&ifaddr) < 0){
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		ফেরত MOSQ_ERR_ERRNO;
	}

	জন্য(কাঠামো ifaddrs *ifa=ifaddr; ifa!=ফাঁকা; ifa=ifa->ifa_পরবর্তী){
		যদি(ifa->ifa_addr == ফাঁকা){
			চালু;
		}

		যদি(!strcasecmp(listener->bind_interface, ifa->ifa_name)){
			have_interface = true;

			যদি(ifa->ifa_addr->sa_family == rp->ai_addr->sa_family){
				যদি(rp->ai_addr->sa_family == AF_INET){
					যদি(listener->host &&
							memcmp(&((কাঠামো sockaddr_in *)rp->ai_addr)->sin_addr,
							&((কাঠামো sockaddr_in *)ifa->ifa_addr)->sin_addr,
							আকার(কাঠামো in_addr))){

						log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						ফেরত MOSQ_ERR_INVAL;
					}অন্যথা{
						memcpy(&((কাঠামো sockaddr_in *)rp->ai_addr)->sin_addr,
								&((কাঠামো sockaddr_in *)ifa->ifa_addr)->sin_addr,
								আকার(কাঠামো in_addr));

						freeifaddrs(ifaddr);
						ফেরত MOSQ_ERR_SUCCESS;
					}
				}অন্যথা যদি(rp->ai_addr->sa_family == AF_INET6){
					যদি(listener->host &&
							memcmp(&((কাঠামো sockaddr_in6 *)rp->ai_addr)->sin6_addr,
							&((কাঠামো sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
							আকার(কাঠামো in6_addr))){

						log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						ফেরত MOSQ_ERR_INVAL;
					}অন্যথা{
						memcpy(&((কাঠামো sockaddr_in6 *)rp->ai_addr)->sin6_addr,
								&((কাঠামো sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
								আকার(কাঠামো in6_addr));

						((কাঠামো sockaddr_in6 *)rp->ai_addr)->sin6_scope_id = ((কাঠামো sockaddr_in6 *)ifa->ifa_addr)->sin6_scope_id;

						freeifaddrs(ifaddr);
						ফেরত MOSQ_ERR_SUCCESS;
					}
				}
			}
		}
	}
	freeifaddrs(ifaddr);
	যদি(have_interface){
		log__printf(ফাঁকা, MOSQ_LOG_WARNING, "Warning: Interface %s does not support %s configuration.",
				listener->bind_interface, rp->ai_addr->sa_family == AF_INET ? "IPv4" : "IPv6");
		ফেরত MOSQ_ERR_NOT_SUPPORTED;
	}অন্যথা{
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Interface %s does not exist.", listener->bind_interface);
		ফেরত MOSQ_ERR_NOT_FOUND;
	}
}
#endif


স্থির পূর্ণসংখ্যা net__socket_listen_tcp(কাঠামো mosquitto__listener *listener)
{
	mosq_sock_t sock = INVALID_SOCKET;
	কাঠামো addrinfo hints;
	কাঠামো addrinfo *ainfo, *rp;
	অক্ষর service[10];
	পূর্ণসংখ্যা rc;
	পূর্ণসংখ্যা ss_opt = 1;
#ifndef WIN32
	bool interface_bound = false;
#endif

	যদি(!listener){
		ফেরত MOSQ_ERR_INVAL;
	}

	snprintf(service, 10, "%d", listener->port);
	memset(&hints, 0, আকার(কাঠামো addrinfo));
	যদি(listener->socket_domain){
		hints.ai_family = listener->socket_domain;
	}অন্যথা{
		hints.ai_family = AF_UNSPEC;
	}
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	rc = getaddrinfo(listener->host, service, &hints, &ainfo);
	যদি(rc){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error creating listener: %s.", gai_strerror(rc));
		ফেরত INVALID_SOCKET;
	}

	listener->sock_গুনতি = 0;
	listener->socks = ফাঁকা;

	জন্য(rp = ainfo; rp; rp = rp->ai_পরবর্তী){
		যদি(rp->ai_family == AF_INET){
			log__printf(ফাঁকা, MOSQ_LOG_INFO, "Opening ipv4 listen socket on port %d.", ntohs(((কাঠামো sockaddr_in *)rp->ai_addr)->sin_port));
		}অন্যথা যদি(rp->ai_family == AF_INET6){
			log__printf(ফাঁকা, MOSQ_LOG_INFO, "Opening ipv6 listen socket on port %d.", ntohs(((কাঠামো sockaddr_in6 *)rp->ai_addr)->sin6_port));
		}অন্যথা{
			চালু;
		}

		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		যদি(sock == INVALID_SOCKET){
			net__print_error(MOSQ_LOG_WARNING, "Warning: %s");
			চালু;
		}
		listener->sock_গুনতি++;
		listener->socks = mosquitto_realloc(listener->socks, আকার(mosq_sock_t)*(size_t)listener->sock_গুনতি);
		যদি(!listener->socks){
			log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Out of memory.");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			ফেরত MOSQ_ERR_NOMEM;
		}
		listener->socks[listener->sock_গুনতি-1] = sock;

#ifndef WIN32
		ss_opt = 1;
		/* Unimportant if this fails */
		(শূন্য)setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (ধ্রুবক শূন্য *)&ss_opt, আকার(ss_opt));
#endif
#ifdef IPV6_V6ONLY
		ss_opt = 1;
		(শূন্য)setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (ধ্রুবক শূন্য *)&ss_opt, আকার(ss_opt));
#endif

		যদি(net__socket_nonblock(&sock)){
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			ফেরত 1;
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
					ফেরত rc;
				}অন্যথা{
					চালু;
				}
			}
			interface_bound = true;
		}
#endif

		যদি(bind(sock, rp->ai_addr, rp->ai_addrlen) == -1){
#যদি defined(__linux__)
			যদি(errno == EACCES){
				log__printf(ফাঁকা, MOSQ_LOG_ERR, "If you are trying to bind to a privileged port (<1024), try using setcap and do not start the broker as root:");
				log__printf(ফাঁকা, MOSQ_LOG_ERR, "    sudo setcap 'CAP_NET_BIND_SERVICE=+ep /usr/sbin/mosquitto'");
			}
#endif
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			COMPAT_CLOSE(sock);
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			ফেরত 1;
		}

		যদি(listen(sock, 100) == -1){
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			mosquitto_FREE(listener->socks);
			ফেরত 1;
		}
	}
	freeaddrinfo(ainfo);

#ifndef WIN32
	যদি(listener->bind_interface && !interface_bound){
		mosquitto_FREE(listener->socks);
		ফেরত 1;
	}
#endif

	ফেরত 0;
}


#ifdef WITH_UNIX_SOCKETS


স্থির পূর্ণসংখ্যা net__socket_listen_unix(কাঠামো mosquitto__listener *listener)
{
	কাঠামো sockaddr_un addr;
	mosq_sock_t sock = INVALID_SOCKET;
	পূর্ণসংখ্যা rc;
#ifndef WIN32
	mode_t old_mask;
#endif

	যদি(listener->unix_socket_path == ফাঁকা){
		ফেরত MOSQ_ERR_INVAL;
	}
	যদি(strlen(listener->unix_socket_path) > আকার(addr.sun_path)-1){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Path to unix socket is too long \"%s\".", listener->unix_socket_path);
		ফেরত MOSQ_ERR_INVAL;
	}

#ifdef WIN32
	DeleteFile(listener->unix_socket_path);
#অন্যথা
	unlink(listener->unix_socket_path);
#endif
	log__printf(ফাঁকা, MOSQ_LOG_INFO, "Opening unix listen socket on path %s.", listener->unix_socket_path);
	memset(&addr, 0, আকার(কাঠামো sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, listener->unix_socket_path, আকার(addr.sun_path)-1);

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	যদি(sock == INVALID_SOCKET){
		net__print_error(MOSQ_LOG_ERR, "Error creating unix socket: %s");
		ফেরত 1;
	}
	listener->sock_গুনতি++;
	listener->socks = mosquitto_realloc(listener->socks, আকার(mosq_sock_t)*(size_t)listener->sock_গুনতি);
	যদি(!listener->socks){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Out of memory.");
		COMPAT_CLOSE(sock);
		ফেরত MOSQ_ERR_NOMEM;
	}
	listener->socks[listener->sock_গুনতি-1] = sock;


#ifndef WIN32
	old_mask = umask(0007);
#endif
	rc = bind(sock, (কাঠামো sockaddr *)&addr, আকার(কাঠামো sockaddr_un));
#ifndef WIN32
	umask(old_mask);
#endif

	যদি(rc == -1){
		net__print_error(MOSQ_LOG_ERR, "Error binding unix socket: %s");
		ফেরত 1;
	}

	যদি(listen(sock, 10) == -1){
		net__print_error(MOSQ_LOG_ERR, "Error listening to unix socket: %s");
		ফেরত 1;
	}

	যদি(net__socket_nonblock(&sock)){
		ফেরত 1;
	}

	ফেরত 0;
}
#endif


/* Creates a socket and listens on port 'port'.
 * Returns 1 on failure
 * Returns 0 on success.
 */
পূর্ণসংখ্যা net__socket_listen(কাঠামো mosquitto__listener *listener)
{
	পূর্ণসংখ্যা rc;

	যদি(!listener){
		ফেরত MOSQ_ERR_INVAL;
	}

#ifdef WITH_UNIX_SOCKETS
	যদি(listener->port == 0 && listener->unix_socket_path != ফাঁকা){
		rc = net__socket_listen_unix(listener);
	}অন্যথা
#endif
	{
		rc = net__socket_listen_tcp(listener);
	}
	যদি(rc){
		ফেরত rc;
	}

	/* We need to have at least one working socket. */
	যদি(listener->sock_গুনতি > 0){
#ifdef WITH_TLS
		যদি(listener->certfile && listener->keyfile){
			যদি(net__tls_server_ctx(listener)){
				ফেরত 1;
			}

			যদি(net__tls_load_verify(listener)){
				ফেরত 1;
			}
		}
#  ifdef FINAL_WITH_TLS_PSK
		যদি(listener->psk_hint){
			যদি(listener->certfile == ফাঁকা || listener->keyfile == ফাঁকা){
				যদি(net__tls_server_ctx(listener)){
					ফেরত 1;
				}
			}
			SSL_CTX_set_psk_server_callback(listener->ssl_ctx, psk_server_callback);
			যদি(listener->psk_hint){
				rc = SSL_CTX_use_psk_identity_hint(listener->ssl_ctx, listener->psk_hint);
				যদি(rc == 0){
					log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to set TLS PSK hint.");
					net__print_ssl_error(ফাঁকা, ফাঁকা);
					ফেরত 1;
				}
			}
		}
#  endif /* FINAL_WITH_TLS_PSK */
		যদি(tls_ex_সূচক_context == -1){
			tls_ex_সূচক_context = SSL_get_ex_new_সূচক(0, "client context", ফাঁকা, ফাঁকা, ফাঁকা);
		}
		যদি(tls_ex_সূচক_listener == -1){
			tls_ex_সূচক_listener = SSL_get_ex_new_সূচক(0, "listener", ফাঁকা, ফাঁকা, ফাঁকা);
		}
#endif /* WITH_TLS */
		ফেরত 0;
	}অন্যথা{
		ফেরত 1;
	}
}


পূর্ণসংখ্যা net__socket_get_address(mosq_sock_t sock, অক্ষর *buf, size_t len, uint16_t *remote_port)
{
	কাঠামো sockaddr_storage addr;
	socklen_t addrlen;

	memset(&addr, 0, আকার(কাঠামো sockaddr_storage));
	addrlen = আকার(addr);
	যদি(!getpeername(sock, (কাঠামো sockaddr *)&addr, &addrlen)){
		যদি(addr.ss_family == AF_INET){
			যদি(remote_port){
				*remote_port = ntohs(((কাঠামো sockaddr_in *)&addr)->sin_port);
			}
			যদি(inet_ntop(AF_INET, &((কাঠামো sockaddr_in *)&addr)->sin_addr.s_addr, buf, (socklen_t)len)){
				ফেরত 0;
			}
		}অন্যথা যদি(addr.ss_family == AF_INET6){
			যদি(remote_port){
				*remote_port = ntohs(((কাঠামো sockaddr_in6 *)&addr)->sin6_port);
			}
			যদি(inet_ntop(AF_INET6, &((কাঠামো sockaddr_in6 *)&addr)->sin6_addr.s6_addr, buf, (socklen_t)len)){
				ফেরত 0;
			}
#ifdef WITH_UNIX_SOCKETS
		}অন্যথা যদি(addr.ss_family == AF_UNIX){
			কাঠামো sockaddr_un un;
			addrlen = আকার(কাঠামো sockaddr_un);
			যদি(!getsockname(sock, (কাঠামো sockaddr *)&un, &addrlen)){
				snprintf(buf, len, "%s", un.sun_path);
			}অন্যথা{
				snprintf(buf, len, "unix-socket");
			}
			ফেরত 0;
#endif
		}
	}
	ফেরত 1;
}
