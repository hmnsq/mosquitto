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

#شامل "config.h"

#اگر_غیر_تعریف_شدہ WIN32
#  include <arpa/inet.h>
#  ifndef _AIX
#    include <ifaddrs.h>
#  endif
#  include <netdb.h>
#  include <netinet/tcp.h>
#  include <strings.h>
#  include <sys/socket.h>
#  include <unistd.h>
#ورنہ
#  include <winsock2.h>
#  include <ws2tcpip.h>
#اگر_ختم

#شامل <errno.h>
#شامل <fcntl.h>
#شامل <stdio.h>
#شامل <string.h>
#اگر_تعریف_شدہ WITH_WRAP
#  include <tcpd.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#  include <netinet/in.h>
#اگر_ختم

#اگر defined(WITH_UNIX_SOCKETS) || defined(WITH_TLS)
#  include "sys/stat.h"
#اگر_ختم

#اگر_تعریف_شدہ WITH_UNIX_SOCKETS
#  ifdef WIN32
#    include <afunix.h>
#  ورنہ
#    include <sys/un.h>
#  endif
#اگر_ختم

#اگر_تعریف_شدہ __QNX__
#  include <net/netbyte.h>
#اگر_ختم

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "net_mosq.h"
#شامل "util_mosq.h"

#اگر_تعریف_شدہ WITH_TLS
#  include "tls_mosq.h"
#  include <openssl/err.h>
ساکن صحیح tls_ex_اشاریہ_context = -1;
ساکن صحیح tls_ex_اشاریہ_listener = -1;
#اگر_ختم

#شامل "sys_tree.h"

/* For EMFILE handling */
ساکن mosq_sock_t فالتو_sock = INVALID_SOCKET;


خالی net__broker_init(خالی)
{
	فالتو_sock = socket(AF_INET, SOCK_STREAM, 0);
	net__init();
#اگر_تعریف_شدہ WITH_TLS
	net__init_tls();
#اگر_ختم
}


خالی net__broker_cleanup(خالی)
{
	اگر(فالتو_sock != INVALID_SOCKET){
		COMPAT_CLOSE(فالتو_sock);
		فالتو_sock = INVALID_SOCKET;
	}
	net__cleanup();
}


ساکن خالی net__print_error(غیرعلامتی صحیح لوگارتھم, ثابت حرف *format_str)
{
	حرف *buf;

#اگر_تعریف_شدہ WIN32
	Format_پیغام(FORMAT_پیغام_ALLOCATE_بفر | FORMAT_پیغام_FROM_SYSTEM,
			تہی, WSAGet_آخری_Error(), LANG_NEUTRAL, (LPTSTR)&buf, 0, تہی);

	log__printf(تہی, لوگارتھم, format_str, buf);
	LocalFree(buf);
#ورنہ
	buf = strerror(errno);
	log__printf(تہی, لوگارتھم, format_str, buf);
#اگر_ختم
}


ساخت mosquitto *net__socket_accept(ساخت mosquitto__listener_sock *listensock)
{
	mosq_sock_t new_sock = INVALID_SOCKET;
	ساخت mosquitto *new_context;
#اگر_تعریف_شدہ WITH_TLS
	BIO *bio;
#اگر_ختم
#اگر_تعریف_شدہ WITH_WRAP
	ساخت request_info wrap_req;
	حرف پتہ[1024];
#اگر_ختم

	new_sock = accept(listensock->sock, تہی, 0);
	اگر(new_sock == INVALID_SOCKET){
#اگر_تعریف_شدہ WIN32
		WINDOWS_سیٹ_ERRNO();
		اگر(errno == WSAEMFILE){
#ورنہ
		اگر(errno == EMFILE || errno == ENFILE){
#اگر_ختم
			/* Close the spare socket, which means we should be able to accept
			 * this connection. Accept it, then close it immediately and create
			 * a new spare_sock. This prevents the situation of ever properly
			 * running out of sockets.
			 * It would be nice to send a "server not available" connack here,
			 * but there are lots of reasons why this would be tricky (TLS
			 * being the big one). */
			COMPAT_CLOSE(فالتو_sock);
			new_sock = accept(listensock->sock, تہی, 0);
			اگر(new_sock != INVALID_SOCKET){
				COMPAT_CLOSE(new_sock);
			}
			فالتو_sock = socket(AF_INET, SOCK_STREAM, 0);
			log__printf(تہی, MOSQ_LOG_WARNING,
					"Unable to accept new connection, system socket count has been exceeded. Try increasing \"ulimit -n\" or equivalent.");
		}
		واپس تہی;
	}

	metrics__int_inc(mosq_شمار_socket_connections, 1);

	اگر(net__socket_nonblock(&new_sock)){
		واپس تہی;
	}

#اگر_تعریف_شدہ WITH_WRAP
	/* Use tcpd / libwrap to determine whether a connection is allowed. */
	request_init(&wrap_req, RQ_FILE, new_sock, RQ_DAEMON, "mosquitto", 0);
	fromhost(&wrap_req);
	اگر(!hosts_access(&wrap_req)){
		/* Access is denied */
		اگر(db.config->connection_messages == true){
			اگر(!net_socket_get_پتہ(new_sock, پتہ, 1024, تہی)){
				log__printf(تہی, MOSQ_LOG_NOTICE, "Client connection from %s denied access by tcpd.", پتہ);
			}
		}
		COMPAT_CLOSE(new_sock);
		واپس تہی;
	}
#اگر_ختم

	اگر(db.config->سیٹ_tcp_nodelay && listensock->listener->پورٹ){
		صحیح flag = 1;
		اگر(setsockopt(new_sock, IPPROTO_TCP, TCP_NODELAY, (ثابت خالی *)&flag, حجم(صحیح)) != 0){
			log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Unable to set TCP_NODELAY.");
		}
	}

	new_context = context__init();
	اگر(!new_context){
		COMPAT_CLOSE(new_sock);
		واپس تہی;
	}
	new_context->listener = listensock->listener;
	اگر(!new_context->listener){
		context__cleanup(new_context, true);
		واپس تہی;
	}
	new_context->listener->client_گنتی++;

	اگر(new_context->listener->enable_proxy_protocol){
		اگر(context__init_sock(new_context, new_sock, false) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			واپس تہی;
		}
		اگر(new_context->listener->enable_proxy_protocol == 2){
			new_context->transport = mosq_t_proxy_v2;
		}ورنہ{
			new_context->transport = mosq_t_proxy_v1;
		}
		new_context->proxy.cmd = -1;
	}ورنہ{
		اگر(context__init_sock(new_context, new_sock, true) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			واپس تہی;
		}
		انتخاب(new_context->listener->protocol){
			حالت mp_mqtt:
				new_context->transport = mosq_t_tcp;
				توڑو;
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			حالت mp_websockets:
				اگر(http__context_init(new_context)){
					context__cleanup(new_context, true);
					واپس تہی;
				}
				توڑو;
#اگر_ختم
			طےشدہ:
				context__cleanup(new_context, true);
				واپس تہی;
		}
	}

	اگر((new_context->listener->max_connections > 0 && new_context->listener->client_گنتی > new_context->listener->max_connections)
			|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (غیرعلامتی صحیح)db.config->global_max_connections)){

		اگر(db.config->connection_messages == true){
			log__printf(تہی, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", new_context->پتہ);
		}
		context__cleanup(new_context, true);
		واپس تہی;
	}

#اگر_تعریف_شدہ WITH_TLS
	/* TLS init */
	اگر(new_context->listener->ssl_ctx){
		new_context->ssl = SSL_new(new_context->listener->ssl_ctx);
		اگر(!new_context->ssl){
			context__cleanup(new_context, true);
			واپس تہی;
		}
		اگر(!SSL_سیٹ_ex_ڈیٹا(new_context->ssl, tls_ex_اشاریہ_context, new_context)
				|| !SSL_سیٹ_ex_ڈیٹا(new_context->ssl, tls_ex_اشاریہ_listener, new_context->listener)){
			context__cleanup(new_context, true);
			واپس تہی;
		}
		new_context->want_write = true;
		bio = BIO_new_socket(new_sock, BIO_NOCLOSE);
		SSL_سیٹ_bio(new_context->ssl, bio, bio);
		ERR_clear_error();
		SSL_سیٹ_accept_state(new_context->ssl);
	}
#اگر_ختم

	اگر(db.config->connection_messages == true
			&& !new_context->listener->enable_proxy_protocol){

		log__printf(تہی, MOSQ_LOG_NOTICE, "New connection from %s:%d on port %d.",
				new_context->پتہ, new_context->remote_پورٹ, new_context->listener->پورٹ);
	}

	mux__new(new_context);
	keepalive_جوڑو(new_context);

	واپس new_context;
}

#اگر_تعریف_شدہ WITH_TLS


ساکن صحیح client_certificate_verify(صحیح preverify_ok, X509_STORE_CTX *ctx)
{
	/* Preverify should check expiry, revocation. */
	اگر(preverify_ok == 0){
		SSL *ssl;
		ssl = X509_STORE_CTX_get_ex_ڈیٹا(ctx, SSL_get_ex_ڈیٹا_X509_STORE_CTX_idx());
		اگر(ssl){
			ساخت mosquitto__listener *listener;
			listener = SSL_get_ex_ڈیٹا(ssl, tls_ex_اشاریہ_listener);

			اگر(listener && listener->disable_client_cert_date_checks){
				صحیح err = X509_STORE_CTX_get_error(ctx);
				اگر(err == X509_V_ERR_CERT_NOT_YET_VALID || err == X509_V_ERR_CERT_HAS_EXPIRED){
					preverify_ok = 1;
				}
			}
		}
	}
	واپس preverify_ok;
}
#اگر_ختم

#اگر_تعریف_شدہ FINAL_WITH_TLS_PSK


ساکن غیرعلامتی صحیح psk_server_callback(SSL *ssl, ثابت حرف *شناختی_عنصر, غیرعلامتی حرف *psk, غیرعلامتی صحیح max_psk_len)
{
	ساخت mosquitto *context;
	ساخت mosquitto__listener *listener;
	حرف *psk_کلید = تہی;
	صحیح len;
	ثابت حرف *psk_hint;

	اگر(!شناختی_عنصر){
		واپس 0;
	}

	context = SSL_get_ex_ڈیٹا(ssl, tls_ex_اشاریہ_context);
	اگر(!context){
		واپس 0;
	}

	listener = SSL_get_ex_ڈیٹا(ssl, tls_ex_اشاریہ_listener);
	اگر(!listener){
		واپس 0;
	}

	psk_hint = listener->psk_hint;

	/* The hex to BN conversion results in the length halving, so we can pass
	 * max_psk_len*2 as the max hex key here. */
	psk_کلید = mosquitto_calloc(1, (size_t)max_psk_len*2 + 1);
	اگر(!psk_کلید){
		واپس 0;
	}

	اگر(mosquitto_psk_کلید_get(context, psk_hint, شناختی_عنصر, psk_کلید, (صحیح)max_psk_len*2) != MOSQ_ERR_SUCCESS){
		mosquitto_FREE(psk_کلید);
		واپس 0;
	}

	len = mosquitto__hex2bin(psk_کلید, psk, (صحیح)max_psk_len);
	اگر(len < 0){
		mosquitto_FREE(psk_کلید);
		واپس 0;
	}

	اگر(listener->use_شناختی_عنصر_as_username){
		اگر(mosquitto_validate_utf8(شناختی_عنصر, (صحیح)لڑی_لمبائی(شناختی_عنصر))){
			mosquitto_free(psk_کلید);
			واپس 0;
		}
		context->username = mosquitto_strdup(شناختی_عنصر);
		اگر(!context->username){
			mosquitto_FREE(psk_کلید);
			واپس 0;
		}
	}

	mosquitto_FREE(psk_کلید);
	واپس (غیرعلامتی صحیح)len;
}
#اگر_ختم

#اگر_تعریف_شدہ WITH_TLS


ساکن خالی tls_keylog_callback(ثابت SSL *ssl, ثابت حرف *سطر)
{
	غیر_مستعمل(ssl);

	اگر(db.tls_keylog){
		FILE *fptr;
		fptr = mosquitto_fopen(db.tls_keylog, "at", true);
		اگر(fptr){
#اگر_غیر_تعریف_شدہ WIN32
			/* Until mosquitto_fopen enforces file permissions on all files,
			 * enforce it here. We can enforce it here, because it isn't a
			 * change of behaviour. */
			ساخت stat statbuf;
			اگر(fstat(fileno(fptr), &statbuf) < 0
					|| statbuf.st_منوال & (S_IRWXG | S_IRWXO)){

				fclose(fptr);
				واپس;
			}
#اگر_ختم
			fprintf(fptr, "%s\n", سطر);
			fclose(fptr);
		}ورنہ{
#اگر_غیر_تعریف_شدہ WIN32
			اگر(errno == ELOOP){
				log__printf(تہی, MOSQ_LOG_INFO, "Error: keylog file must not be a symbolic link");
			}ورنہ
#اگر_ختم
			{
				log__printf(تہی, MOSQ_LOG_INFO, "Error: Unable to open keylog file: %s", strerror(errno));
			}
		}
	}
}


صحیح net__tls_server_ctx(ساخت mosquitto__listener *listener)
{
	حرف buf[256];
	صحیح rc;

	اگر(tls_ex_اشاریہ_context == -1){
		tls_ex_اشاریہ_context = SSL_get_ex_new_اشاریہ(0, "client context", تہی, تہی, تہی);
	}
	اگر(tls_ex_اشاریہ_listener == -1){
		tls_ex_اشاریہ_listener = SSL_get_ex_new_اشاریہ(0, "listener", تہی, تہی, تہی);
	}

	اگر(listener->ssl_ctx){
		SSL_CTX_free(listener->ssl_ctx);
		listener->ssl_ctx = تہی;
	}

	listener->ssl_ctx = SSL_CTX_new(TLS_server_طریقہ());

	اگر(!listener->ssl_ctx){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to create TLS context.");
		واپس MOSQ_ERR_TLS;
	}

#اگر_تعریف_شدہ SSL_OP_NO_TLSv1_3
	اگر(db.config->فی_listener_settings){
		اگر(listener->security_options->psk_file){
			SSL_CTX_سیٹ_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}ورنہ{
		اگر(db.config->security_options.psk_file){
			SSL_CTX_سیٹ_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}
#اگر_ختم

	اگر(listener->tls_نسخہ == تہی){
		SSL_CTX_سیٹ_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
#اگر_تعریف_شدہ SSL_OP_NO_TLSv1_3
	}ورنہ اگر(!strcmp(listener->tls_نسخہ, "tlsv1.3")){
		SSL_CTX_سیٹ_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2);
#اگر_ختم
	}ورنہ اگر(!strcmp(listener->tls_نسخہ, "tlsv1.2")){
		SSL_CTX_سیٹ_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
	}ورنہ اگر(!strcmp(listener->tls_نسخہ, "tlsv1.1")){
		SSL_CTX_سیٹ_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1);
		log__printf(تہی, MOSQ_LOG_WARNING, "Warning: TLS v1.1 is insecure. It must only be used in the case "
				"where you have devices that cannot be upgraded to use a secure version of TLS. It is "
				"recommended to use as secure a set of ciphers as possible and that will restrict it to "
				"TLS v1.1 only, and to use a separate listener using TLS v1.2 and secure ciphers for your "
				"other devices.");
		log__printf(تہی, MOSQ_LOG_WARNING, "Please be aware that support for TLS v1.1 will go away eventually "
				"and that you should plan now to migrate away from it.");
	}ورنہ{
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Unsupported tls_version \"%s\".", listener->tls_نسخہ);
		واپس MOSQ_ERR_TLS;
	}

#اگر_تعریف_شدہ SSL_OP_NO_COMPRESSION
	/* Disable compression */
	SSL_CTX_سیٹ_options(listener->ssl_ctx, SSL_OP_NO_COMPRESSION);
#اگر_ختم
#اگر_تعریف_شدہ SSL_OP_CIPHER_SERVER_PREFERENCE
	/* Server chooses cipher */
	SSL_CTX_سیٹ_options(listener->ssl_ctx, SSL_OP_CIPHER_SERVER_PREFERENCE);
#اگر_ختم

#اگر_تعریف_شدہ SSL_منوال_RELEASE_BUFFERS
	/* Use even less memory per SSL connection. */
	SSL_CTX_سیٹ_منوال(listener->ssl_ctx, SSL_منوال_RELEASE_BUFFERS);
#اگر_ختم

	SSL_CTX_سیٹ_dh_auto(listener->ssl_ctx, 1);

#اگر_تعریف_شدہ SSL_OP_NO_RENEGOTIATION
	SSL_CTX_سیٹ_options(listener->ssl_ctx, SSL_OP_NO_RENEGOTIATION);
#اگر_ختم

	اگر(db.tls_keylog){
		log__printf(تہی, MOSQ_LOG_NOTICE, "TLS key logging to '%s' enabled for all listeners.",
				db.tls_keylog);
		log__printf(تہی, MOSQ_LOG_NOTICE, "TLS key logging is for DEBUGGING only.");

		SSL_CTX_سیٹ_keylog_callback(listener->ssl_ctx, tls_keylog_callback);
	}

	snprintf(buf, 256, "mosquitto-%d", listener->پورٹ);
	SSL_CTX_سیٹ_session_id_context(listener->ssl_ctx, (غیرعلامتی حرف *)buf, (غیرعلامتی صحیح)لڑی_لمبائی(buf));

	اگر(listener->ciphers){
		rc = SSL_CTX_سیٹ_cipher_list(listener->ssl_ctx, listener->ciphers);
		اگر(rc == 0){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			واپس MOSQ_ERR_TLS;
		}
	}ورنہ{
		rc = SSL_CTX_سیٹ_cipher_list(listener->ssl_ctx, "DEFAULT:!aNULL:!eNULL:!LOW:!EXPORT:!SSLv2:@STRENGTH");
		اگر(rc == 0){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			واپس MOSQ_ERR_TLS;
		}
	}
	اگر(listener->ciphers_tls13){
		rc = SSL_CTX_سیٹ_ciphersuites(listener->ssl_ctx, listener->ciphers_tls13);
		اگر(rc == 0){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to set TLS 1.3 ciphersuites. Check cipher_tls13 list \"%s\".", listener->ciphers_tls13);
			واپس MOSQ_ERR_TLS;
		}
	}

	واپس MOSQ_ERR_SUCCESS;
}
#اگر_ختم


#اگر_تعریف_شدہ WITH_TLS


ساکن صحیح net_بھرو_crl_file(ساخت mosquitto__listener *listener)
{
	X509_STORE *store;
	X509_LOOKUP *lookup;
	صحیح rc;

	store = SSL_CTX_get_cert_store(listener->ssl_ctx);
	اگر(!store){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to obtain TLS store.");
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		واپس MOSQ_ERR_TLS;
	}
	lookup = X509_STORE_جوڑو_lookup(store, X509_LOOKUP_file());
	rc = X509_بھرو_crl_file(lookup, listener->crlfile, X509_FILETYPE_PEM);
	اگر(rc < 1){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to load certificate revocation file \"%s\". Check crlfile.", listener->crlfile);
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		net__print_ssl_error(تہی, تہی);
		واپس MOSQ_ERR_TLS;
	}
	X509_STORE_سیٹ_flags(store, X509_V_FLAG_CRL_CHECK);

	واپس MOSQ_ERR_SUCCESS;
}
#اگر_ختم


صحیح net_بھرو_certificates(ساخت mosquitto__listener *listener)
{
#اگر_تعریف_شدہ WITH_TLS
	صحیح rc;

	اگر(listener->require_certificate){
		SSL_CTX_سیٹ_verify(listener->ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, client_certificate_verify);
	}ورنہ{
		SSL_CTX_سیٹ_verify(listener->ssl_ctx, SSL_VERIFY_NONE, client_certificate_verify);
	}
	rc = SSL_CTX_use_certificate_chain_file(listener->ssl_ctx, listener->certfile);
	اگر(rc != 1){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
		net__print_ssl_error(تہی, تہی);
		واپس MOSQ_ERR_TLS;
	}
	اگر(listener->tls_engine == تہی || listener->tls_keyform == mosq_k_pem){
		rc = SSL_CTX_use_نجی_کلید_file(listener->ssl_ctx, listener->keyfile, SSL_FILETYPE_PEM);
		اگر(rc != 1){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			net__print_ssl_error(تہی, تہی);
			واپس MOSQ_ERR_TLS;
		}
	}
	rc = SSL_CTX_check_نجی_کلید(listener->ssl_ctx);
	اگر(rc != 1){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Server certificate/key are inconsistent.");
		net__print_ssl_error(تہی, تہی);
		واپس MOSQ_ERR_TLS;
	}
	/* Load CRLs if they exist. */
	اگر(listener->crlfile){
		rc = net_بھرو_crl_file(listener);
		اگر(rc){
			واپس rc;
		}
	}
#ورنہ
	غیر_مستعمل(listener);
#اگر_ختم
	واپس MOSQ_ERR_SUCCESS;
}


#اگر defined(WITH_TLS) && !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000


ساکن صحیح net_بھرو_engine(ساخت mosquitto__listener *listener)
{
	ENGINE *engine = تہی;
	UI_طریقہ *ui_طریقہ;
	EVP_PKEY *pkey;

	اگر(!listener->tls_engine){
		واپس MOSQ_ERR_SUCCESS;
	}

	engine = ENGINE_by_id(listener->tls_engine);
	اگر(!engine){
		log__printf(تہی, MOSQ_LOG_ERR, "Error loading %s engine\n", listener->tls_engine);
		net__print_ssl_error(تہی, تہی);
		واپس MOSQ_ERR_TLS;
	}
	اگر(!ENGINE_init(engine)){
		log__printf(تہی, MOSQ_LOG_ERR, "Failed engine initialisation\n");
		net__print_ssl_error(تہی, تہی);
		واپس MOSQ_ERR_TLS;
	}
	ENGINE_سیٹ_default(engine, ENGINE_طریقہ_ALL);

	اگر(listener->tls_keyform == mosq_k_engine){
		ui_طریقہ = net_get_ui_طریقہ();
		اگر(listener->tls_engine_kpass_sha1){
			اگر(!ENGINE_ctrl_cmd(engine, ENGINE_SECRET_منوال, ENGINE_SECRET_منوال_SHA, تہی, تہی, 0)){
				log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to set engine secret mode sha");
				net__print_ssl_error(تہی, تہی);
				واپس MOSQ_ERR_TLS;
			}
			اگر(!ENGINE_ctrl_cmd(engine, ENGINE_PIN, 0, listener->tls_engine_kpass_sha1, تہی, 0)){
				log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to set engine pin");
				net__print_ssl_error(تہی, تہی);
				واپس MOSQ_ERR_TLS;
			}
			ui_طریقہ = تہی;
		}
		pkey = ENGINE_بھرو_نجی_کلید(engine, listener->keyfile, ui_طریقہ, تہی);
		اگر(!pkey){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to load engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(تہی, تہی);
			واپس MOSQ_ERR_TLS;
		}
		اگر(SSL_CTX_use_نجی_کلید(listener->ssl_ctx, pkey) <= 0){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to use engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(تہی, تہی);
			واپس MOSQ_ERR_TLS;
		}
	}
	ENGINE_free(engine); /* release the structural reference from ENGINE_by_id() */

	واپس MOSQ_ERR_SUCCESS;
}
#اگر_ختم


صحیح net_tls_بھرو_verify(ساخت mosquitto__listener *listener)
{
#اگر_تعریف_شدہ WITH_TLS
	صحیح rc;

#  اگر OPENSSL_نسخہ_نمبر < 0x30000000L
	اگر(listener->cafile || listener->capath){
		rc = SSL_CTX_بھرو_verify_locations(listener->ssl_ctx, listener->cafile, listener->capath);
		اگر(rc == 0){
			اگر(listener->cafile && listener->capath){
				log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\" and capath \"%s\".", listener->cafile, listener->capath);
			}ورنہ اگر(listener->cafile){
				log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			}ورنہ{
				log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			}
		}
	}
#  ورنہ
	ڈھیر_کا(X509_NAME) *ca_نام = تہی;
	اگر(listener->cafile || listener->capath){
		ca_نام = sk_X509_NAME_new_null();
	}

	اگر(listener->cafile){
		rc = SSL_CTX_بھرو_verify_file(listener->ssl_ctx, listener->cafile);
		اگر(rc == 0){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			net__print_ssl_error(تہی, تہی);
			واپس MOSQ_ERR_TLS;
		}

		ڈھیر_کا(X509_NAME) *cert_نام = SSL_بھرو_client_CA_file(listener->cafile);
		اگر(cert_نام){
			برائے(صحیح i=0; i<sk_X509_NAME_عدد(cert_نام); i++){
				sk_X509_NAME_ڈالو(ca_نام, X509_NAME_dup(sk_X509_NAME_قدر(cert_نام, i)));
			}
			sk_X509_NAME_نکالو_free(cert_نام, X509_NAME_free);
		}

	}
	اگر(listener->capath){
		rc = SSL_CTX_بھرو_verify_dir(listener->ssl_ctx, listener->capath);
		اگر(rc == 0){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			net__print_ssl_error(تہی, تہی);
			sk_X509_NAME_نکالو_free(ca_نام, X509_NAME_free);
			واپس MOSQ_ERR_TLS;
		}

		SSL_جوڑو_dir_cert_subjects_مقصد_ڈھیر(ca_نام, listener->capath);
	}
	اگر(ca_نام){
		SSL_CTX_سیٹ_client_CA_list(listener->ssl_ctx, ca_نام);
	}
#  endif

#  اگر !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000
	اگر(net_بھرو_engine(listener)){
		واپس MOSQ_ERR_TLS;
	}
#  endif
#اگر_ختم
	واپس net_بھرو_certificates(listener);
}


#اگر !defined(WIN32) && !defined(_AIX)


ساکن صحیح net__bind_interface(ساخت mosquitto__listener *listener, ساخت addrinfo *rp)
{
	/*
	 * This binds the listener sock to a network interface.
	 * The use of SO_BINDTODEVICE requires root access, which we don't have, so instead
	 * use getifaddrs to find the interface addresses, and use IP of the
	 * matching interface in the later bind().
	 */
	ساخت ifaddrs *ifaddr;
	bool have_interface = false;

	اگر(getifaddrs(&ifaddr) < 0){
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		واپس MOSQ_ERR_ERRNO;
	}

	برائے(ساخت ifaddrs *ifa=ifaddr; ifa!=تہی; ifa=ifa->ifa_اگلا){
		اگر(ifa->ifa_addr == تہی){
			جاری;
		}

		اگر(!strcasecmp(listener->bind_interface, ifa->ifa_name)){
			have_interface = true;

			اگر(ifa->ifa_addr->sa_family == rp->ai_addr->sa_family){
				اگر(rp->ai_addr->sa_family == AF_INET){
					اگر(listener->host &&
							memcmp(&((ساخت sockaddr_in *)rp->ai_addr)->sin_addr,
							&((ساخت sockaddr_in *)ifa->ifa_addr)->sin_addr,
							حجم(ساخت in_addr))){

						log__printf(تہی, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						واپس MOSQ_ERR_INVAL;
					}ورنہ{
						حافظہ_نقل(&((ساخت sockaddr_in *)rp->ai_addr)->sin_addr,
								&((ساخت sockaddr_in *)ifa->ifa_addr)->sin_addr,
								حجم(ساخت in_addr));

						freeifaddrs(ifaddr);
						واپس MOSQ_ERR_SUCCESS;
					}
				}ورنہ اگر(rp->ai_addr->sa_family == AF_INET6){
					اگر(listener->host &&
							memcmp(&((ساخت sockaddr_in6 *)rp->ai_addr)->sin6_addr,
							&((ساخت sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
							حجم(ساخت in6_addr))){

						log__printf(تہی, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						واپس MOSQ_ERR_INVAL;
					}ورنہ{
						حافظہ_نقل(&((ساخت sockaddr_in6 *)rp->ai_addr)->sin6_addr,
								&((ساخت sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
								حجم(ساخت in6_addr));

						((ساخت sockaddr_in6 *)rp->ai_addr)->sin6_scope_id = ((ساخت sockaddr_in6 *)ifa->ifa_addr)->sin6_scope_id;

						freeifaddrs(ifaddr);
						واپس MOSQ_ERR_SUCCESS;
					}
				}
			}
		}
	}
	freeifaddrs(ifaddr);
	اگر(have_interface){
		log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Interface %s does not support %s configuration.",
				listener->bind_interface, rp->ai_addr->sa_family == AF_INET ? "IPv4" : "IPv6");
		واپس MOSQ_ERR_NOT_SUPPORTED;
	}ورنہ{
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Interface %s does not exist.", listener->bind_interface);
		واپس MOSQ_ERR_NOT_ملا;
	}
}
#اگر_ختم


ساکن صحیح net__socket_listen_tcp(ساخت mosquitto__listener *listener)
{
	mosq_sock_t sock = INVALID_SOCKET;
	ساخت addrinfo hints;
	ساخت addrinfo *ainfo, *rp;
	حرف service[10];
	صحیح rc;
	صحیح ss_opt = 1;
#اگر_غیر_تعریف_شدہ WIN32
	bool interface_bound = false;
#اگر_ختم

	اگر(!listener){
		واپس MOSQ_ERR_INVAL;
	}

	snprintf(service, 10, "%d", listener->پورٹ);
	memset(&hints, 0, حجم(ساخت addrinfo));
	اگر(listener->socket_domain){
		hints.ai_family = listener->socket_domain;
	}ورنہ{
		hints.ai_family = AF_UNSPEC;
	}
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	rc = getaddrinfo(listener->host, service, &hints, &ainfo);
	اگر(rc){
		log__printf(تہی, MOSQ_LOG_ERR, "Error creating listener: %s.", gai_strerror(rc));
		واپس INVALID_SOCKET;
	}

	listener->sock_گنتی = 0;
	listener->socks = تہی;

	برائے(rp = ainfo; rp; rp = rp->ai_اگلا){
		اگر(rp->ai_family == AF_INET){
			log__printf(تہی, MOSQ_LOG_INFO, "Opening ipv4 listen socket on port %d.", ntohs(((ساخت sockaddr_in *)rp->ai_addr)->sin_port));
		}ورنہ اگر(rp->ai_family == AF_INET6){
			log__printf(تہی, MOSQ_LOG_INFO, "Opening ipv6 listen socket on port %d.", ntohs(((ساخت sockaddr_in6 *)rp->ai_addr)->sin6_port));
		}ورنہ{
			جاری;
		}

		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		اگر(sock == INVALID_SOCKET){
			net__print_error(MOSQ_LOG_WARNING, "Warning: %s");
			جاری;
		}
		listener->sock_گنتی++;
		listener->socks = mosquitto_realloc(listener->socks, حجم(mosq_sock_t)*(size_t)listener->sock_گنتی);
		اگر(!listener->socks){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			واپس MOSQ_ERR_NOMEM;
		}
		listener->socks[listener->sock_گنتی-1] = sock;

#اگر_غیر_تعریف_شدہ WIN32
		ss_opt = 1;
		/* Unimportant if this fails */
		(خالی)setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (ثابت خالی *)&ss_opt, حجم(ss_opt));
#اگر_ختم
#اگر_تعریف_شدہ IPV6_V6ONLY
		ss_opt = 1;
		(خالی)setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (ثابت خالی *)&ss_opt, حجم(ss_opt));
#اگر_ختم

		اگر(net__socket_nonblock(&sock)){
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			واپس 1;
		}

#اگر !defined(WIN32) && !defined(_AIX)
		اگر(listener->bind_interface){
			/* It might be possible that an interface does not support all relevant sa_families.
			 * We should successfully find at least one. */
			rc = net__bind_interface(listener, rp);
			اگر(rc){
				COMPAT_CLOSE(sock);
				listener->sock_گنتی--;
				اگر(rc == MOSQ_ERR_NOT_ملا || rc == MOSQ_ERR_INVAL){
					freeaddrinfo(ainfo);
					واپس rc;
				}ورنہ{
					جاری;
				}
			}
			interface_bound = true;
		}
#اگر_ختم

		اگر(bind(sock, rp->ai_addr, rp->ai_addrlen) == -1){
#اگر defined(__linux__)
			اگر(errno == EACCES){
				log__printf(تہی, MOSQ_LOG_ERR, "If you are trying to bind to a privileged port (<1024), try using setcap and do not start the broker as root:");
				log__printf(تہی, MOSQ_LOG_ERR, "    sudo setcap 'CAP_NET_BIND_SERVICE=+ep /usr/sbin/mosquitto'");
			}
#اگر_ختم
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			COMPAT_CLOSE(sock);
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			واپس 1;
		}

		اگر(listen(sock, 100) == -1){
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			mosquitto_FREE(listener->socks);
			واپس 1;
		}
	}
	freeaddrinfo(ainfo);

#اگر_غیر_تعریف_شدہ WIN32
	اگر(listener->bind_interface && !interface_bound){
		mosquitto_FREE(listener->socks);
		واپس 1;
	}
#اگر_ختم

	واپس 0;
}


#اگر_تعریف_شدہ WITH_UNIX_SOCKETS


ساکن صحیح net__socket_listen_unix(ساخت mosquitto__listener *listener)
{
	ساخت sockaddr_un addr;
	mosq_sock_t sock = INVALID_SOCKET;
	صحیح rc;
#اگر_غیر_تعریف_شدہ WIN32
	mode_t old_نقاب;
#اگر_ختم

	اگر(listener->unix_socket_راستہ == تہی){
		واپس MOSQ_ERR_INVAL;
	}
	اگر(لڑی_لمبائی(listener->unix_socket_راستہ) > حجم(addr.sun_راستہ)-1){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Path to unix socket is too long \"%s\".", listener->unix_socket_راستہ);
		واپس MOSQ_ERR_INVAL;
	}

#اگر_تعریف_شدہ WIN32
	DeleteFile(listener->unix_socket_راستہ);
#ورنہ
	unlink(listener->unix_socket_راستہ);
#اگر_ختم
	log__printf(تہی, MOSQ_LOG_INFO, "Opening unix listen socket on path %s.", listener->unix_socket_راستہ);
	memset(&addr, 0, حجم(ساخت sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_راستہ, listener->unix_socket_راستہ, حجم(addr.sun_راستہ)-1);

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	اگر(sock == INVALID_SOCKET){
		net__print_error(MOSQ_LOG_ERR, "Error creating unix socket: %s");
		واپس 1;
	}
	listener->sock_گنتی++;
	listener->socks = mosquitto_realloc(listener->socks, حجم(mosq_sock_t)*(size_t)listener->sock_گنتی);
	اگر(!listener->socks){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
		COMPAT_CLOSE(sock);
		واپس MOSQ_ERR_NOMEM;
	}
	listener->socks[listener->sock_گنتی-1] = sock;


#اگر_غیر_تعریف_شدہ WIN32
	old_نقاب = umask(0007);
#اگر_ختم
	rc = bind(sock, (ساخت sockaddr *)&addr, حجم(ساخت sockaddr_un));
#اگر_غیر_تعریف_شدہ WIN32
	umask(old_نقاب);
#اگر_ختم

	اگر(rc == -1){
		net__print_error(MOSQ_LOG_ERR, "Error binding unix socket: %s");
		واپس 1;
	}

	اگر(listen(sock, 10) == -1){
		net__print_error(MOSQ_LOG_ERR, "Error listening to unix socket: %s");
		واپس 1;
	}

	اگر(net__socket_nonblock(&sock)){
		واپس 1;
	}

	واپس 0;
}
#اگر_ختم


/* Creates a socket and listens on port 'port'.
 * Returns 1 on failure
 * Returns 0 on success.
 */
صحیح net__socket_listen(ساخت mosquitto__listener *listener)
{
	صحیح rc;

	اگر(!listener){
		واپس MOSQ_ERR_INVAL;
	}

#اگر_تعریف_شدہ WITH_UNIX_SOCKETS
	اگر(listener->پورٹ == 0 && listener->unix_socket_راستہ != تہی){
		rc = net__socket_listen_unix(listener);
	}ورنہ
#اگر_ختم
	{
		rc = net__socket_listen_tcp(listener);
	}
	اگر(rc){
		واپس rc;
	}

	/* We need to have at least one working socket. */
	اگر(listener->sock_گنتی > 0){
#اگر_تعریف_شدہ WITH_TLS
		اگر(listener->certfile && listener->keyfile){
			اگر(net__tls_server_ctx(listener)){
				واپس 1;
			}

			اگر(net_tls_بھرو_verify(listener)){
				واپس 1;
			}
		}
#  ifdef FINAL_WITH_TLS_PSK
		اگر(listener->psk_hint){
			اگر(listener->certfile == تہی || listener->keyfile == تہی){
				اگر(net__tls_server_ctx(listener)){
					واپس 1;
				}
			}
			SSL_CTX_سیٹ_psk_server_callback(listener->ssl_ctx, psk_server_callback);
			اگر(listener->psk_hint){
				rc = SSL_CTX_use_psk_شناختی_عنصر_hint(listener->ssl_ctx, listener->psk_hint);
				اگر(rc == 0){
					log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to set TLS PSK hint.");
					net__print_ssl_error(تہی, تہی);
					واپس 1;
				}
			}
		}
#  endif /* FINAL_WITH_TLS_PSK */
		اگر(tls_ex_اشاریہ_context == -1){
			tls_ex_اشاریہ_context = SSL_get_ex_new_اشاریہ(0, "client context", تہی, تہی, تہی);
		}
		اگر(tls_ex_اشاریہ_listener == -1){
			tls_ex_اشاریہ_listener = SSL_get_ex_new_اشاریہ(0, "listener", تہی, تہی, تہی);
		}
#اگر_ختم /* WITH_TLS */
		واپس 0;
	}ورنہ{
		واپس 1;
	}
}


صحیح net_socket_get_پتہ(mosq_sock_t sock, حرف *buf, size_t len, uint16_t *remote_پورٹ)
{
	ساخت sockaddr_storage addr;
	socklen_t addrlen;

	memset(&addr, 0, حجم(ساخت sockaddr_storage));
	addrlen = حجم(addr);
	اگر(!getpeername(sock, (ساخت sockaddr *)&addr, &addrlen)){
		اگر(addr.ss_family == AF_INET){
			اگر(remote_پورٹ){
				*remote_پورٹ = ntohs(((ساخت sockaddr_in *)&addr)->sin_port);
			}
			اگر(inet_ntop(AF_INET, &((ساخت sockaddr_in *)&addr)->sin_addr.s_addr, buf, (socklen_t)len)){
				واپس 0;
			}
		}ورنہ اگر(addr.ss_family == AF_INET6){
			اگر(remote_پورٹ){
				*remote_پورٹ = ntohs(((ساخت sockaddr_in6 *)&addr)->sin6_port);
			}
			اگر(inet_ntop(AF_INET6, &((ساخت sockaddr_in6 *)&addr)->sin6_addr.s6_addr, buf, (socklen_t)len)){
				واپس 0;
			}
#اگر_تعریف_شدہ WITH_UNIX_SOCKETS
		}ورنہ اگر(addr.ss_family == AF_UNIX){
			ساخت sockaddr_un un;
			addrlen = حجم(ساخت sockaddr_un);
			اگر(!getsockname(sock, (ساخت sockaddr *)&un, &addrlen)){
				snprintf(buf, len, "%s", un.sun_راستہ);
			}ورنہ{
				snprintf(buf, len, "unix-socket");
			}
			واپس 0;
#اگر_ختم
		}
	}
	واپس 1;
}
