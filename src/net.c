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

#ضمن "config.h"

#که_نه_تعریف_شوی WIN32
#  include <arpa/inet.h>
#  ifndef _AIX
#    include <ifaddrs.h>
#  endif
#  include <netdb.h>
#  include <netinet/tcp.h>
#  include <strings.h>
#  include <sys/socket.h>
#  include <unistd.h>
#وإلا
#  include <winsock2.h>
#  include <ws2tcpip.h>
#د_که_پای

#ضمن <errno.h>
#ضمن <fcntl.h>
#ضمن <stdio.h>
#ضمن <string.h>
#که_تعریف_شوی WITH_WRAP
#  include <tcpd.h>
#د_که_پای

#که_تعریف_شوی HAVE_NETINET_دننه_H
#  include <netinet/in.h>
#د_که_پای

#إذا defined(WITH_UNIX_SOCKETS) || defined(WITH_TLS)
#  include "sys/stat.h"
#د_که_پای

#که_تعریف_شوی WITH_UNIX_SOCKETS
#  ifdef WIN32
#    include <afunix.h>
#  وإلا
#    include <sys/un.h>
#  endif
#د_که_پای

#که_تعریف_شوی __QNX__
#  include <net/netbyte.h>
#د_که_پای

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "net_mosq.h"
#ضمن "util_mosq.h"

#که_تعریف_شوی WITH_TLS
#  include "tls_mosq.h"
#  include <openssl/err.h>
ساكن صحيح tls_ex_شاخص_context = -1;
ساكن صحيح tls_ex_شاخص_listener = -1;
#د_که_پای

#ضمن "sys_tree.h"

/* For EMFILE handling */
ساكن mosq_sock_t اضافي_sock = INVALID_SOCKET;


فراغ net__broker_init(فراغ)
{
	اضافي_sock = socket(AF_INET, SOCK_STREAM, 0);
	net__init();
#که_تعریف_شوی WITH_TLS
	net__init_tls();
#د_که_پای
}


فراغ net__broker_cleanup(فراغ)
{
	إذا(اضافي_sock != INVALID_SOCKET){
		COMPAT_CLOSE(اضافي_sock);
		اضافي_sock = INVALID_SOCKET;
	}
	net__cleanup();
}


ساكن فراغ net__print_error(بینښه صحيح لوګاریتم, ثابت حرف *format_str)
{
	حرف *buf;

#که_تعریف_شوی WIN32
	Format_پیغام(FORMAT_پیغام_ALLOCATE_بفر | FORMAT_پیغام_FROM_SYSTEM,
			عدم, WSAGet_وروستی_Error(), LANG_NEUTRAL, (LPTSTR)&buf, 0, عدم);

	log__printf(عدم, لوګاریتم, format_str, buf);
	LocalFree(buf);
#وإلا
	buf = strerror(errno);
	log__printf(عدم, لوګاریتم, format_str, buf);
#د_که_پای
}


بنية mosquitto *net__socket_accept(بنية mosquitto__listener_sock *listensock)
{
	mosq_sock_t new_sock = INVALID_SOCKET;
	بنية mosquitto *new_context;
#که_تعریف_شوی WITH_TLS
	BIO *bio;
#د_که_پای
#که_تعریف_شوی WITH_WRAP
	بنية request_info wrap_req;
	حرف پته[1024];
#د_که_پای

	new_sock = accept(listensock->sock, عدم, 0);
	إذا(new_sock == INVALID_SOCKET){
#که_تعریف_شوی WIN32
		WINDOWS_سېټ_ERRNO();
		إذا(errno == WSAEMFILE){
#وإلا
		إذا(errno == EMFILE || errno == ENFILE){
#د_که_پای
			/* Close the spare socket, which means we should be able to accept
			 * this connection. Accept it, then close it immediately and create
			 * a new spare_sock. This prevents the situation of ever properly
			 * running out of sockets.
			 * It would be nice to send a "server not available" connack here,
			 * but there are lots of reasons why this would be tricky (TLS
			 * being the big one). */
			COMPAT_CLOSE(اضافي_sock);
			new_sock = accept(listensock->sock, عدم, 0);
			إذا(new_sock != INVALID_SOCKET){
				COMPAT_CLOSE(new_sock);
			}
			اضافي_sock = socket(AF_INET, SOCK_STREAM, 0);
			log__printf(عدم, MOSQ_LOG_WARNING,
					"Unable to accept new connection, system socket count has been exceeded. Try increasing \"ulimit -n\" or equivalent.");
		}
		أرجع عدم;
	}

	metrics__int_inc(mosq_counter_socket_connections, 1);

	إذا(net__socket_nonblock(&new_sock)){
		أرجع عدم;
	}

#که_تعریف_شوی WITH_WRAP
	/* Use tcpd / libwrap to determine whether a connection is allowed. */
	request_init(&wrap_req, RQ_FILE, new_sock, RQ_DAEMON, "mosquitto", 0);
	fromhost(&wrap_req);
	إذا(!hosts_access(&wrap_req)){
		/* Access is denied */
		إذا(db.config->connection_messages == true){
			إذا(!net_socket_get_پته(new_sock, پته, 1024, عدم)){
				log__printf(عدم, MOSQ_LOG_NOTICE, "Client connection from %s denied access by tcpd.", پته);
			}
		}
		COMPAT_CLOSE(new_sock);
		أرجع عدم;
	}
#د_که_پای

	إذا(db.config->سېټ_tcp_nodelay && listensock->listener->پورټ){
		صحيح flag = 1;
		إذا(setsockopt(new_sock, IPPROTO_TCP, TCP_NODELAY, (ثابت فراغ *)&flag, حجم(صحيح)) != 0){
			log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Unable to set TCP_NODELAY.");
		}
	}

	new_context = context__init();
	إذا(!new_context){
		COMPAT_CLOSE(new_sock);
		أرجع عدم;
	}
	new_context->listener = listensock->listener;
	إذا(!new_context->listener){
		context__cleanup(new_context, true);
		أرجع عدم;
	}
	new_context->listener->client_شمېر++;

	إذا(new_context->listener->enable_proxy_protocol){
		إذا(context__init_sock(new_context, new_sock, false) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			أرجع عدم;
		}
		إذا(new_context->listener->enable_proxy_protocol == 2){
			new_context->transport = mosq_t_proxy_v2;
		}وإلا{
			new_context->transport = mosq_t_proxy_v1;
		}
		new_context->proxy.cmd = -1;
	}وإلا{
		إذا(context__init_sock(new_context, new_sock, true) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			أرجع عدم;
		}
		بدل(new_context->listener->protocol){
			حالة mp_mqtt:
				new_context->transport = mosq_t_tcp;
				اكسر;
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			حالة mp_websockets:
				إذا(http__context_init(new_context)){
					context__cleanup(new_context, true);
					أرجع عدم;
				}
				اكسر;
#د_که_پای
			افتراضي:
				context__cleanup(new_context, true);
				أرجع عدم;
		}
	}

	إذا((new_context->listener->max_connections > 0 && new_context->listener->client_شمېر > new_context->listener->max_connections)
			|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (بینښه صحيح)db.config->global_max_connections)){

		إذا(db.config->connection_messages == true){
			log__printf(عدم, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", new_context->پته);
		}
		context__cleanup(new_context, true);
		أرجع عدم;
	}

#که_تعریف_شوی WITH_TLS
	/* TLS init */
	إذا(new_context->listener->ssl_ctx){
		new_context->ssl = SSL_new(new_context->listener->ssl_ctx);
		إذا(!new_context->ssl){
			context__cleanup(new_context, true);
			أرجع عدم;
		}
		إذا(!SSL_سېټ_ex_معلومات(new_context->ssl, tls_ex_شاخص_context, new_context)
				|| !SSL_سېټ_ex_معلومات(new_context->ssl, tls_ex_شاخص_listener, new_context->listener)){
			context__cleanup(new_context, true);
			أرجع عدم;
		}
		new_context->want_write = true;
		bio = BIO_new_socket(new_sock, BIO_NOCLOSE);
		SSL_سېټ_bio(new_context->ssl, bio, bio);
		ERR_clear_error();
		SSL_سېټ_accept_state(new_context->ssl);
	}
#د_که_پای

	إذا(db.config->connection_messages == true
			&& !new_context->listener->enable_proxy_protocol){

		log__printf(عدم, MOSQ_LOG_NOTICE, "New connection from %s:%d on port %d.",
				new_context->پته, new_context->remote_پورټ, new_context->listener->پورټ);
	}

	mux__new(new_context);
	keepalive_جمع_کړه(new_context);

	أرجع new_context;
}

#که_تعریف_شوی WITH_TLS


ساكن صحيح client_certificate_verify(صحيح preverify_ok, X509_STORE_CTX *ctx)
{
	/* Preverify should check expiry, revocation. */
	إذا(preverify_ok == 0){
		SSL *ssl;
		ssl = X509_STORE_CTX_get_ex_معلومات(ctx, SSL_get_ex_معلومات_X509_STORE_CTX_idx());
		إذا(ssl){
			بنية mosquitto__listener *listener;
			listener = SSL_get_ex_معلومات(ssl, tls_ex_شاخص_listener);

			إذا(listener && listener->disable_client_cert_date_checks){
				صحيح err = X509_STORE_CTX_get_error(ctx);
				إذا(err == X509_V_ERR_CERT_NOT_YET_VALID || err == X509_V_ERR_CERT_HAS_EXPIRED){
					preverify_ok = 1;
				}
			}
		}
	}
	أرجع preverify_ok;
}
#د_که_پای

#که_تعریف_شوی FINAL_WITH_TLS_PSK


ساكن بینښه صحيح psk_server_callback(SSL *ssl, ثابت حرف *بې_اغېزه_عنصر, بینښه حرف *psk, بینښه صحيح max_psk_len)
{
	بنية mosquitto *context;
	بنية mosquitto__listener *listener;
	حرف *psk_کلی = عدم;
	صحيح len;
	ثابت حرف *psk_hint;

	إذا(!بې_اغېزه_عنصر){
		أرجع 0;
	}

	context = SSL_get_ex_معلومات(ssl, tls_ex_شاخص_context);
	إذا(!context){
		أرجع 0;
	}

	listener = SSL_get_ex_معلومات(ssl, tls_ex_شاخص_listener);
	إذا(!listener){
		أرجع 0;
	}

	psk_hint = listener->psk_hint;

	/* The hex to BN conversion results in the length halving, so we can pass
	 * max_psk_len*2 as the max hex key here. */
	psk_کلی = mosquitto_calloc(1, (size_t)max_psk_len*2 + 1);
	إذا(!psk_کلی){
		أرجع 0;
	}

	إذا(mosquitto_psk_کلی_get(context, psk_hint, بې_اغېزه_عنصر, psk_کلی, (صحيح)max_psk_len*2) != MOSQ_ERR_SUCCESS){
		mosquitto_FREE(psk_کلی);
		أرجع 0;
	}

	len = mosquitto__hex2bin(psk_کلی, psk, (صحيح)max_psk_len);
	إذا(len < 0){
		mosquitto_FREE(psk_کلی);
		أرجع 0;
	}

	إذا(listener->use_بې_اغېزه_عنصر_as_username){
		إذا(mosquitto_validate_utf8(بې_اغېزه_عنصر, (صحيح)مزي_اوږدوالی(بې_اغېزه_عنصر))){
			mosquitto_free(psk_کلی);
			أرجع 0;
		}
		context->username = mosquitto_strdup(بې_اغېزه_عنصر);
		إذا(!context->username){
			mosquitto_FREE(psk_کلی);
			أرجع 0;
		}
	}

	mosquitto_FREE(psk_کلی);
	أرجع (بینښه صحيح)len;
}
#د_که_پای

#که_تعریف_شوی WITH_TLS


ساكن فراغ tls_keylog_callback(ثابت SSL *ssl, ثابت حرف *کرښه)
{
	ناکارول_شوی(ssl);

	إذا(db.tls_keylog){
		FILE *fptr;
		fptr = mosquitto_fopen(db.tls_keylog, "at", true);
		إذا(fptr){
#که_نه_تعریف_شوی WIN32
			/* Until mosquitto_fopen enforces file permissions on all files,
			 * enforce it here. We can enforce it here, because it isn't a
			 * change of behaviour. */
			بنية stat statbuf;
			إذا(fstat(fileno(fptr), &statbuf) < 0
					|| statbuf.st_ډېرپېښ & (S_IRWXG | S_IRWXO)){

				fclose(fptr);
				أرجع;
			}
#د_که_پای
			fprintf(fptr, "%s\n", کرښه);
			fclose(fptr);
		}وإلا{
#که_نه_تعریف_شوی WIN32
			إذا(errno == ELOOP){
				log__printf(عدم, MOSQ_LOG_INFO, "Error: keylog file must not be a symbolic link");
			}وإلا
#د_که_پای
			{
				log__printf(عدم, MOSQ_LOG_INFO, "Error: Unable to open keylog file: %s", strerror(errno));
			}
		}
	}
}


صحيح net__tls_server_ctx(بنية mosquitto__listener *listener)
{
	حرف buf[256];
	صحيح rc;

	إذا(tls_ex_شاخص_context == -1){
		tls_ex_شاخص_context = SSL_get_ex_new_شاخص(0, "client context", عدم, عدم, عدم);
	}
	إذا(tls_ex_شاخص_listener == -1){
		tls_ex_شاخص_listener = SSL_get_ex_new_شاخص(0, "listener", عدم, عدم, عدم);
	}

	إذا(listener->ssl_ctx){
		SSL_CTX_free(listener->ssl_ctx);
		listener->ssl_ctx = عدم;
	}

	listener->ssl_ctx = SSL_CTX_new(TLS_server_لاره());

	إذا(!listener->ssl_ctx){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to create TLS context.");
		أرجع MOSQ_ERR_TLS;
	}

#که_تعریف_شوی SSL_OP_NO_TLSv1_3
	إذا(db.config->فی_listener_settings){
		إذا(listener->security_options->psk_file){
			SSL_CTX_سېټ_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}وإلا{
		إذا(db.config->security_options.psk_file){
			SSL_CTX_سېټ_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}
#د_که_پای

	إذا(listener->tls_نسخه == عدم){
		SSL_CTX_سېټ_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
#که_تعریف_شوی SSL_OP_NO_TLSv1_3
	}وإلا إذا(!strcmp(listener->tls_نسخه, "tlsv1.3")){
		SSL_CTX_سېټ_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2);
#د_که_پای
	}وإلا إذا(!strcmp(listener->tls_نسخه, "tlsv1.2")){
		SSL_CTX_سېټ_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
	}وإلا إذا(!strcmp(listener->tls_نسخه, "tlsv1.1")){
		SSL_CTX_سېټ_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1);
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS v1.1 is insecure. It must only be used in the case "
				"where you have devices that cannot be upgraded to use a secure version of TLS. It is "
				"recommended to use as secure a set of ciphers as possible and that will restrict it to "
				"TLS v1.1 only, and to use a separate listener using TLS v1.2 and secure ciphers for your "
				"other devices.");
		log__printf(عدم, MOSQ_LOG_WARNING, "Please be aware that support for TLS v1.1 will go away eventually "
				"and that you should plan now to migrate away from it.");
	}وإلا{
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Unsupported tls_version \"%s\".", listener->tls_نسخه);
		أرجع MOSQ_ERR_TLS;
	}

#که_تعریف_شوی SSL_OP_NO_COMPRESSION
	/* Disable compression */
	SSL_CTX_سېټ_options(listener->ssl_ctx, SSL_OP_NO_COMPRESSION);
#د_که_پای
#که_تعریف_شوی SSL_OP_CIPHER_SERVER_PREFERENCE
	/* Server chooses cipher */
	SSL_CTX_سېټ_options(listener->ssl_ctx, SSL_OP_CIPHER_SERVER_PREFERENCE);
#د_که_پای

#که_تعریف_شوی SSL_ډېرپېښ_RELEASE_BUFFERS
	/* Use even less memory per SSL connection. */
	SSL_CTX_سېټ_ډېرپېښ(listener->ssl_ctx, SSL_ډېرپېښ_RELEASE_BUFFERS);
#د_که_پای

	SSL_CTX_سېټ_dh_auto(listener->ssl_ctx, 1);

#که_تعریف_شوی SSL_OP_NO_RENEGOTIATION
	SSL_CTX_سېټ_options(listener->ssl_ctx, SSL_OP_NO_RENEGOTIATION);
#د_که_پای

	إذا(db.tls_keylog){
		log__printf(عدم, MOSQ_LOG_NOTICE, "TLS key logging to '%s' enabled for all listeners.",
				db.tls_keylog);
		log__printf(عدم, MOSQ_LOG_NOTICE, "TLS key logging is for DEBUGGING only.");

		SSL_CTX_سېټ_keylog_callback(listener->ssl_ctx, tls_keylog_callback);
	}

	snprintf(buf, 256, "mosquitto-%d", listener->پورټ);
	SSL_CTX_سېټ_session_id_context(listener->ssl_ctx, (بینښه حرف *)buf, (بینښه صحيح)مزي_اوږدوالی(buf));

	إذا(listener->ciphers){
		rc = SSL_CTX_سېټ_cipher_list(listener->ssl_ctx, listener->ciphers);
		إذا(rc == 0){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			أرجع MOSQ_ERR_TLS;
		}
	}وإلا{
		rc = SSL_CTX_سېټ_cipher_list(listener->ssl_ctx, "DEFAULT:!aNULL:!eNULL:!LOW:!EXPORT:!SSLv2:@STRENGTH");
		إذا(rc == 0){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			أرجع MOSQ_ERR_TLS;
		}
	}
	إذا(listener->ciphers_tls13){
		rc = SSL_CTX_سېټ_ciphersuites(listener->ssl_ctx, listener->ciphers_tls13);
		إذا(rc == 0){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to set TLS 1.3 ciphersuites. Check cipher_tls13 list \"%s\".", listener->ciphers_tls13);
			أرجع MOSQ_ERR_TLS;
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}
#د_که_پای


#که_تعریف_شوی WITH_TLS


ساكن صحيح net_بارول_crl_file(بنية mosquitto__listener *listener)
{
	X509_STORE *store;
	X509_LOOKUP *lookup;
	صحيح rc;

	store = SSL_CTX_get_cert_store(listener->ssl_ctx);
	إذا(!store){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to obtain TLS store.");
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		أرجع MOSQ_ERR_TLS;
	}
	lookup = X509_STORE_جمع_کړه_lookup(store, X509_LOOKUP_file());
	rc = X509_بارول_crl_file(lookup, listener->crlfile, X509_FILETYPE_PEM);
	إذا(rc < 1){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load certificate revocation file \"%s\". Check crlfile.", listener->crlfile);
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		net__print_ssl_error(عدم, عدم);
		أرجع MOSQ_ERR_TLS;
	}
	X509_STORE_سېټ_flags(store, X509_V_FLAG_CRL_CHECK);

	أرجع MOSQ_ERR_SUCCESS;
}
#د_که_پای


صحيح net_بارول_certificates(بنية mosquitto__listener *listener)
{
#که_تعریف_شوی WITH_TLS
	صحيح rc;

	إذا(listener->require_certificate){
		SSL_CTX_سېټ_verify(listener->ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, client_certificate_verify);
	}وإلا{
		SSL_CTX_سېټ_verify(listener->ssl_ctx, SSL_VERIFY_NONE, client_certificate_verify);
	}
	rc = SSL_CTX_use_certificate_chain_file(listener->ssl_ctx, listener->certfile);
	إذا(rc != 1){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
		net__print_ssl_error(عدم, عدم);
		أرجع MOSQ_ERR_TLS;
	}
	إذا(listener->tls_engine == عدم || listener->tls_keyform == mosq_k_pem){
		rc = SSL_CTX_use_شخصي_کلی_file(listener->ssl_ctx, listener->keyfile, SSL_FILETYPE_PEM);
		إذا(rc != 1){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			net__print_ssl_error(عدم, عدم);
			أرجع MOSQ_ERR_TLS;
		}
	}
	rc = SSL_CTX_check_شخصي_کلی(listener->ssl_ctx);
	إذا(rc != 1){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Server certificate/key are inconsistent.");
		net__print_ssl_error(عدم, عدم);
		أرجع MOSQ_ERR_TLS;
	}
	/* Load CRLs if they exist. */
	إذا(listener->crlfile){
		rc = net_بارول_crl_file(listener);
		إذا(rc){
			أرجع rc;
		}
	}
#وإلا
	ناکارول_شوی(listener);
#د_که_پای
	أرجع MOSQ_ERR_SUCCESS;
}


#إذا defined(WITH_TLS) && !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000


ساكن صحيح net_بارول_engine(بنية mosquitto__listener *listener)
{
	ENGINE *engine = عدم;
	UI_لاره *ui_لاره;
	EVP_PKEY *pkey;

	إذا(!listener->tls_engine){
		أرجع MOSQ_ERR_SUCCESS;
	}

	engine = ENGINE_by_id(listener->tls_engine);
	إذا(!engine){
		log__printf(عدم, MOSQ_LOG_ERR, "Error loading %s engine\n", listener->tls_engine);
		net__print_ssl_error(عدم, عدم);
		أرجع MOSQ_ERR_TLS;
	}
	إذا(!ENGINE_init(engine)){
		log__printf(عدم, MOSQ_LOG_ERR, "Failed engine initialisation\n");
		net__print_ssl_error(عدم, عدم);
		أرجع MOSQ_ERR_TLS;
	}
	ENGINE_سېټ_default(engine, ENGINE_لاره_ALL);

	إذا(listener->tls_keyform == mosq_k_engine){
		ui_لاره = net_get_ui_لاره();
		إذا(listener->tls_engine_kpass_sha1){
			إذا(!ENGINE_ctrl_cmd(engine, ENGINE_SECRET_ډېرپېښ, ENGINE_SECRET_ډېرپېښ_SHA, عدم, عدم, 0)){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to set engine secret mode sha");
				net__print_ssl_error(عدم, عدم);
				أرجع MOSQ_ERR_TLS;
			}
			إذا(!ENGINE_ctrl_cmd(engine, ENGINE_PIN, 0, listener->tls_engine_kpass_sha1, عدم, 0)){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to set engine pin");
				net__print_ssl_error(عدم, عدم);
				أرجع MOSQ_ERR_TLS;
			}
			ui_لاره = عدم;
		}
		pkey = ENGINE_بارول_شخصي_کلی(engine, listener->keyfile, ui_لاره, عدم);
		إذا(!pkey){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(عدم, عدم);
			أرجع MOSQ_ERR_TLS;
		}
		إذا(SSL_CTX_use_شخصي_کلی(listener->ssl_ctx, pkey) <= 0){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to use engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(عدم, عدم);
			أرجع MOSQ_ERR_TLS;
		}
	}
	ENGINE_free(engine); /* release the structural reference from ENGINE_by_id() */

	أرجع MOSQ_ERR_SUCCESS;
}
#د_که_پای


صحيح net_tls_بارول_verify(بنية mosquitto__listener *listener)
{
#که_تعریف_شوی WITH_TLS
	صحيح rc;

#  إذا OPENSSL_نسخه_شمېره < 0x30000000L
	إذا(listener->cafile || listener->capath){
		rc = SSL_CTX_بارول_verify_locations(listener->ssl_ctx, listener->cafile, listener->capath);
		إذا(rc == 0){
			إذا(listener->cafile && listener->capath){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\" and capath \"%s\".", listener->cafile, listener->capath);
			}وإلا إذا(listener->cafile){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			}وإلا{
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			}
		}
	}
#  وإلا
	ډېری_د(X509_NAME) *ca_نومونه = عدم;
	إذا(listener->cafile || listener->capath){
		ca_نومونه = sk_X509_NAME_new_null();
	}

	إذا(listener->cafile){
		rc = SSL_CTX_بارول_verify_file(listener->ssl_ctx, listener->cafile);
		إذا(rc == 0){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			net__print_ssl_error(عدم, عدم);
			أرجع MOSQ_ERR_TLS;
		}

		ډېری_د(X509_NAME) *cert_نومونه = SSL_بارول_client_CA_file(listener->cafile);
		إذا(cert_نومونه){
			لكل(صحيح i=0; i<sk_X509_NAME_عدد(cert_نومونه); i++){
				sk_X509_NAME_ټېل(ca_نومونه, X509_NAME_dup(sk_X509_NAME_ارزښت(cert_نومونه, i)));
			}
			sk_X509_NAME_راوباسه_free(cert_نومونه, X509_NAME_free);
		}

	}
	إذا(listener->capath){
		rc = SSL_CTX_بارول_verify_dir(listener->ssl_ctx, listener->capath);
		إذا(rc == 0){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			net__print_ssl_error(عدم, عدم);
			sk_X509_NAME_راوباسه_free(ca_نومونه, X509_NAME_free);
			أرجع MOSQ_ERR_TLS;
		}

		SSL_جمع_کړه_dir_cert_subjects_منزل_ډېری(ca_نومونه, listener->capath);
	}
	إذا(ca_نومونه){
		SSL_CTX_سېټ_client_CA_list(listener->ssl_ctx, ca_نومونه);
	}
#  endif

#  إذا !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000
	إذا(net_بارول_engine(listener)){
		أرجع MOSQ_ERR_TLS;
	}
#  endif
#د_که_پای
	أرجع net_بارول_certificates(listener);
}


#إذا !defined(WIN32) && !defined(_AIX)


ساكن صحيح net__bind_interface(بنية mosquitto__listener *listener, بنية addrinfo *rp)
{
	/*
	 * This binds the listener sock to a network interface.
	 * The use of SO_BINDTODEVICE requires root access, which we don't have, so instead
	 * use getifaddrs to find the interface addresses, and use IP of the
	 * matching interface in the later bind().
	 */
	بنية ifaddrs *ifaddr;
	bool have_interface = false;

	إذا(getifaddrs(&ifaddr) < 0){
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		أرجع MOSQ_ERR_ERRNO;
	}

	لكل(بنية ifaddrs *ifa=ifaddr; ifa!=عدم; ifa=ifa->ifa_بل){
		إذا(ifa->ifa_addr == عدم){
			استمر;
		}

		إذا(!strcasecmp(listener->bind_interface, ifa->ifa_name)){
			have_interface = true;

			إذا(ifa->ifa_addr->sa_family == rp->ai_addr->sa_family){
				إذا(rp->ai_addr->sa_family == AF_INET){
					إذا(listener->host &&
							memcmp(&((بنية sockaddr_in *)rp->ai_addr)->sin_addr,
							&((بنية sockaddr_in *)ifa->ifa_addr)->sin_addr,
							حجم(بنية in_addr))){

						log__printf(عدم, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						أرجع MOSQ_ERR_INVAL;
					}وإلا{
						حافظه_نقل(&((بنية sockaddr_in *)rp->ai_addr)->sin_addr,
								&((بنية sockaddr_in *)ifa->ifa_addr)->sin_addr,
								حجم(بنية in_addr));

						freeifaddrs(ifaddr);
						أرجع MOSQ_ERR_SUCCESS;
					}
				}وإلا إذا(rp->ai_addr->sa_family == AF_INET6){
					إذا(listener->host &&
							memcmp(&((بنية sockaddr_in6 *)rp->ai_addr)->sin6_addr,
							&((بنية sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
							حجم(بنية in6_addr))){

						log__printf(عدم, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						أرجع MOSQ_ERR_INVAL;
					}وإلا{
						حافظه_نقل(&((بنية sockaddr_in6 *)rp->ai_addr)->sin6_addr,
								&((بنية sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
								حجم(بنية in6_addr));

						((بنية sockaddr_in6 *)rp->ai_addr)->sin6_scope_id = ((بنية sockaddr_in6 *)ifa->ifa_addr)->sin6_scope_id;

						freeifaddrs(ifaddr);
						أرجع MOSQ_ERR_SUCCESS;
					}
				}
			}
		}
	}
	freeifaddrs(ifaddr);
	إذا(have_interface){
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Interface %s does not support %s configuration.",
				listener->bind_interface, rp->ai_addr->sa_family == AF_INET ? "IPv4" : "IPv6");
		أرجع MOSQ_ERR_NOT_SUPPORTED;
	}وإلا{
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Interface %s does not exist.", listener->bind_interface);
		أرجع MOSQ_ERR_NOT_وموندل;
	}
}
#د_که_پای


ساكن صحيح net__socket_listen_tcp(بنية mosquitto__listener *listener)
{
	mosq_sock_t sock = INVALID_SOCKET;
	بنية addrinfo hints;
	بنية addrinfo *ainfo, *rp;
	حرف service[10];
	صحيح rc;
	صحيح ss_opt = 1;
#که_نه_تعریف_شوی WIN32
	bool interface_bound = false;
#د_که_پای

	إذا(!listener){
		أرجع MOSQ_ERR_INVAL;
	}

	snprintf(service, 10, "%d", listener->پورټ);
	memset(&hints, 0, حجم(بنية addrinfo));
	إذا(listener->socket_domain){
		hints.ai_family = listener->socket_domain;
	}وإلا{
		hints.ai_family = AF_UNSPEC;
	}
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	rc = getaddrinfo(listener->host, service, &hints, &ainfo);
	إذا(rc){
		log__printf(عدم, MOSQ_LOG_ERR, "Error creating listener: %s.", gai_strerror(rc));
		أرجع INVALID_SOCKET;
	}

	listener->sock_شمېر = 0;
	listener->socks = عدم;

	لكل(rp = ainfo; rp; rp = rp->ai_بل){
		إذا(rp->ai_family == AF_INET){
			log__printf(عدم, MOSQ_LOG_INFO, "Opening ipv4 listen socket on port %d.", ntohs(((بنية sockaddr_in *)rp->ai_addr)->sin_port));
		}وإلا إذا(rp->ai_family == AF_INET6){
			log__printf(عدم, MOSQ_LOG_INFO, "Opening ipv6 listen socket on port %d.", ntohs(((بنية sockaddr_in6 *)rp->ai_addr)->sin6_port));
		}وإلا{
			استمر;
		}

		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		إذا(sock == INVALID_SOCKET){
			net__print_error(MOSQ_LOG_WARNING, "Warning: %s");
			استمر;
		}
		listener->sock_شمېر++;
		listener->socks = mosquitto_realloc(listener->socks, حجم(mosq_sock_t)*(size_t)listener->sock_شمېر);
		إذا(!listener->socks){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			أرجع MOSQ_ERR_NOMEM;
		}
		listener->socks[listener->sock_شمېر-1] = sock;

#که_نه_تعریف_شوی WIN32
		ss_opt = 1;
		/* Unimportant if this fails */
		(فراغ)setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (ثابت فراغ *)&ss_opt, حجم(ss_opt));
#د_که_پای
#که_تعریف_شوی IPV6_V6ONLY
		ss_opt = 1;
		(فراغ)setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (ثابت فراغ *)&ss_opt, حجم(ss_opt));
#د_که_پای

		إذا(net__socket_nonblock(&sock)){
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			أرجع 1;
		}

#إذا !defined(WIN32) && !defined(_AIX)
		إذا(listener->bind_interface){
			/* It might be possible that an interface does not support all relevant sa_families.
			 * We should successfully find at least one. */
			rc = net__bind_interface(listener, rp);
			إذا(rc){
				COMPAT_CLOSE(sock);
				listener->sock_شمېر--;
				إذا(rc == MOSQ_ERR_NOT_وموندل || rc == MOSQ_ERR_INVAL){
					freeaddrinfo(ainfo);
					أرجع rc;
				}وإلا{
					استمر;
				}
			}
			interface_bound = true;
		}
#د_که_پای

		إذا(bind(sock, rp->ai_addr, rp->ai_addrlen) == -1){
#إذا defined(__linux__)
			إذا(errno == EACCES){
				log__printf(عدم, MOSQ_LOG_ERR, "If you are trying to bind to a privileged port (<1024), try using setcap and do not start the broker as root:");
				log__printf(عدم, MOSQ_LOG_ERR, "    sudo setcap 'CAP_NET_BIND_SERVICE=+ep /usr/sbin/mosquitto'");
			}
#د_که_پای
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			COMPAT_CLOSE(sock);
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			أرجع 1;
		}

		إذا(listen(sock, 100) == -1){
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			mosquitto_FREE(listener->socks);
			أرجع 1;
		}
	}
	freeaddrinfo(ainfo);

#که_نه_تعریف_شوی WIN32
	إذا(listener->bind_interface && !interface_bound){
		mosquitto_FREE(listener->socks);
		أرجع 1;
	}
#د_که_پای

	أرجع 0;
}


#که_تعریف_شوی WITH_UNIX_SOCKETS


ساكن صحيح net__socket_listen_unix(بنية mosquitto__listener *listener)
{
	بنية sockaddr_un addr;
	mosq_sock_t sock = INVALID_SOCKET;
	صحيح rc;
#که_نه_تعریف_شوی WIN32
	mode_t old_ماسک;
#د_که_پای

	إذا(listener->unix_socket_لار == عدم){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(مزي_اوږدوالی(listener->unix_socket_لار) > حجم(addr.sun_لار)-1){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Path to unix socket is too long \"%s\".", listener->unix_socket_لار);
		أرجع MOSQ_ERR_INVAL;
	}

#که_تعریف_شوی WIN32
	DeleteFile(listener->unix_socket_لار);
#وإلا
	unlink(listener->unix_socket_لار);
#د_که_پای
	log__printf(عدم, MOSQ_LOG_INFO, "Opening unix listen socket on path %s.", listener->unix_socket_لار);
	memset(&addr, 0, حجم(بنية sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_لار, listener->unix_socket_لار, حجم(addr.sun_لار)-1);

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	إذا(sock == INVALID_SOCKET){
		net__print_error(MOSQ_LOG_ERR, "Error creating unix socket: %s");
		أرجع 1;
	}
	listener->sock_شمېر++;
	listener->socks = mosquitto_realloc(listener->socks, حجم(mosq_sock_t)*(size_t)listener->sock_شمېر);
	إذا(!listener->socks){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		COMPAT_CLOSE(sock);
		أرجع MOSQ_ERR_NOMEM;
	}
	listener->socks[listener->sock_شمېر-1] = sock;


#که_نه_تعریف_شوی WIN32
	old_ماسک = umask(0007);
#د_که_پای
	rc = bind(sock, (بنية sockaddr *)&addr, حجم(بنية sockaddr_un));
#که_نه_تعریف_شوی WIN32
	umask(old_ماسک);
#د_که_پای

	إذا(rc == -1){
		net__print_error(MOSQ_LOG_ERR, "Error binding unix socket: %s");
		أرجع 1;
	}

	إذا(listen(sock, 10) == -1){
		net__print_error(MOSQ_LOG_ERR, "Error listening to unix socket: %s");
		أرجع 1;
	}

	إذا(net__socket_nonblock(&sock)){
		أرجع 1;
	}

	أرجع 0;
}
#د_که_پای


/* Creates a socket and listens on port 'port'.
 * Returns 1 on failure
 * Returns 0 on success.
 */
صحيح net__socket_listen(بنية mosquitto__listener *listener)
{
	صحيح rc;

	إذا(!listener){
		أرجع MOSQ_ERR_INVAL;
	}

#که_تعریف_شوی WITH_UNIX_SOCKETS
	إذا(listener->پورټ == 0 && listener->unix_socket_لار != عدم){
		rc = net__socket_listen_unix(listener);
	}وإلا
#د_که_پای
	{
		rc = net__socket_listen_tcp(listener);
	}
	إذا(rc){
		أرجع rc;
	}

	/* We need to have at least one working socket. */
	إذا(listener->sock_شمېر > 0){
#که_تعریف_شوی WITH_TLS
		إذا(listener->certfile && listener->keyfile){
			إذا(net__tls_server_ctx(listener)){
				أرجع 1;
			}

			إذا(net_tls_بارول_verify(listener)){
				أرجع 1;
			}
		}
#  ifdef FINAL_WITH_TLS_PSK
		إذا(listener->psk_hint){
			إذا(listener->certfile == عدم || listener->keyfile == عدم){
				إذا(net__tls_server_ctx(listener)){
					أرجع 1;
				}
			}
			SSL_CTX_سېټ_psk_server_callback(listener->ssl_ctx, psk_server_callback);
			إذا(listener->psk_hint){
				rc = SSL_CTX_use_psk_بې_اغېزه_عنصر_hint(listener->ssl_ctx, listener->psk_hint);
				إذا(rc == 0){
					log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to set TLS PSK hint.");
					net__print_ssl_error(عدم, عدم);
					أرجع 1;
				}
			}
		}
#  endif /* FINAL_WITH_TLS_PSK */
		إذا(tls_ex_شاخص_context == -1){
			tls_ex_شاخص_context = SSL_get_ex_new_شاخص(0, "client context", عدم, عدم, عدم);
		}
		إذا(tls_ex_شاخص_listener == -1){
			tls_ex_شاخص_listener = SSL_get_ex_new_شاخص(0, "listener", عدم, عدم, عدم);
		}
#د_که_پای /* WITH_TLS */
		أرجع 0;
	}وإلا{
		أرجع 1;
	}
}


صحيح net_socket_get_پته(mosq_sock_t sock, حرف *buf, size_t len, uint16_t *remote_پورټ)
{
	بنية sockaddr_storage addr;
	socklen_t addrlen;

	memset(&addr, 0, حجم(بنية sockaddr_storage));
	addrlen = حجم(addr);
	إذا(!getpeername(sock, (بنية sockaddr *)&addr, &addrlen)){
		إذا(addr.ss_family == AF_INET){
			إذا(remote_پورټ){
				*remote_پورټ = ntohs(((بنية sockaddr_in *)&addr)->sin_port);
			}
			إذا(inet_ntop(AF_INET, &((بنية sockaddr_in *)&addr)->sin_addr.s_addr, buf, (socklen_t)len)){
				أرجع 0;
			}
		}وإلا إذا(addr.ss_family == AF_INET6){
			إذا(remote_پورټ){
				*remote_پورټ = ntohs(((بنية sockaddr_in6 *)&addr)->sin6_port);
			}
			إذا(inet_ntop(AF_INET6, &((بنية sockaddr_in6 *)&addr)->sin6_addr.s6_addr, buf, (socklen_t)len)){
				أرجع 0;
			}
#که_تعریف_شوی WITH_UNIX_SOCKETS
		}وإلا إذا(addr.ss_family == AF_UNIX){
			بنية sockaddr_un un;
			addrlen = حجم(بنية sockaddr_un);
			إذا(!getsockname(sock, (بنية sockaddr *)&un, &addrlen)){
				snprintf(buf, len, "%s", un.sun_لار);
			}وإلا{
				snprintf(buf, len, "unix-socket");
			}
			أرجع 0;
#د_که_پای
		}
	}
	أرجع 1;
}
