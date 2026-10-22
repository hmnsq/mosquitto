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

#अन्तर्भाव "config.h"

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
#अञ्ञथा
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#अन्तर्भाव <errno.h>
#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>
#ifdef WITH_WRAP
#  include <tcpd.h>
#endif

#ifdef HAVE_NETINET_IN_H
#  include <netinet/in.h>
#endif

#सचे defined(WITH_UNIX_SOCKETS) || defined(WITH_TLS)
#  include "sys/stat.h"
#endif

#ifdef WITH_UNIX_SOCKETS
#  ifdef WIN32
#    include <afunix.h>
#  अञ्ञथा
#    include <sys/un.h>
#  endif
#endif

#ifdef __QNX__
#  include <net/netbyte.h>
#endif

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "net_mosq.h"
#अन्तर्भाव "util_mosq.h"

#ifdef WITH_TLS
#  include "tls_mosq.h"
#  include <openssl/err.h>
स्थिर पूर्णाङ्क tls_ex_index_context = -1;
स्थिर पूर्णाङ्क tls_ex_index_listener = -1;
#endif

#अन्तर्भाव "sys_tree.h"

/* For EMFILE handling */
स्थिर mosq_sock_t spare_sock = INVALID_SOCKET;


शून्य net__broker_init(शून्य)
{
	spare_sock = socket(AF_INET, SOCK_STREAM, 0);
	net__init();
#ifdef WITH_TLS
	net__init_tls();
#endif
}


शून्य net__broker_cleanup(शून्य)
{
	सचे(spare_sock != INVALID_SOCKET){
		COMPAT_CLOSE(spare_sock);
		spare_sock = INVALID_SOCKET;
	}
	net__cleanup();
}


स्थिर शून्य net__print_error(unsigned पूर्णाङ्क log, ध्रुव वर्ण *format_str)
{
	वर्ण *buf;

#ifdef WIN32
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			रिक्त, WSAGetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, रिक्त);

	log__printf(रिक्त, log, format_str, buf);
	LocalFree(buf);
#अञ्ञथा
	buf = strerror(errno);
	log__printf(रिक्त, log, format_str, buf);
#endif
}


संरचना mosquitto *net__socket_accept(संरचना mosquitto__listener_sock *listensock)
{
	mosq_sock_t new_sock = INVALID_SOCKET;
	संरचना mosquitto *new_context;
#ifdef WITH_TLS
	BIO *bio;
#endif
#ifdef WITH_WRAP
	संरचना request_info wrap_req;
	वर्ण address[1024];
#endif

	new_sock = accept(listensock->sock, रिक्त, 0);
	सचे(new_sock == INVALID_SOCKET){
#ifdef WIN32
		WINDOWS_SET_ERRNO();
		सचे(errno == WSAEMFILE){
#अञ्ञथा
		सचे(errno == EMFILE || errno == ENFILE){
#endif
			/* Close the spare socket, which means we should be able to accept
			 * this connection. Accept it, then close it immediately and create
			 * a new spare_sock. This prevents the situation of ever properly
			 * running out of sockets.
			 * It would be nice to send a "server not available" connack here,
			 * but there are lots of reasons why this would be tricky (TLS
			 * being the big one). */
			COMPAT_CLOSE(spare_sock);
			new_sock = accept(listensock->sock, रिक्त, 0);
			सचे(new_sock != INVALID_SOCKET){
				COMPAT_CLOSE(new_sock);
			}
			spare_sock = socket(AF_INET, SOCK_STREAM, 0);
			log__printf(रिक्त, MOSQ_LOG_WARNING,
					"Unable to accept new connection, system socket count has been exceeded. Try increasing \"ulimit -n\" or equivalent.");
		}
		पटिनिवत्त रिक्त;
	}

	metrics__int_inc(mosq_counter_socket_connections, 1);

	सचे(net__socket_nonblock(&new_sock)){
		पटिनिवत्त रिक्त;
	}

#ifdef WITH_WRAP
	/* Use tcpd / libwrap to determine whether a connection is allowed. */
	request_init(&wrap_req, RQ_FILE, new_sock, RQ_DAEMON, "mosquitto", 0);
	fromhost(&wrap_req);
	सचे(!hosts_access(&wrap_req)){
		/* Access is denied */
		सचे(db.config->connection_messages == true){
			सचे(!net__socket_get_address(new_sock, address, 1024, रिक्त)){
				log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client connection from %s denied access by tcpd.", address);
			}
		}
		COMPAT_CLOSE(new_sock);
		पटिनिवत्त रिक्त;
	}
#endif

	सचे(db.config->set_tcp_nodelay && listensock->listener->port){
		पूर्णाङ्क flag = 1;
		सचे(setsockopt(new_sock, IPPROTO_TCP, TCP_NODELAY, (ध्रुव शून्य *)&flag, परिमाण(पूर्णाङ्क)) != 0){
			log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Unable to set TCP_NODELAY.");
		}
	}

	new_context = context__init();
	सचे(!new_context){
		COMPAT_CLOSE(new_sock);
		पटिनिवत्त रिक्त;
	}
	new_context->listener = listensock->listener;
	सचे(!new_context->listener){
		context__cleanup(new_context, true);
		पटिनिवत्त रिक्त;
	}
	new_context->listener->client_गणनम्++;

	सचे(new_context->listener->enable_proxy_protocol){
		सचे(context__init_sock(new_context, new_sock, false) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			पटिनिवत्त रिक्त;
		}
		सचे(new_context->listener->enable_proxy_protocol == 2){
			new_context->transport = mosq_t_proxy_v2;
		}अञ्ञथा{
			new_context->transport = mosq_t_proxy_v1;
		}
		new_context->proxy.cmd = -1;
	}अञ्ञथा{
		सचे(context__init_sock(new_context, new_sock, true) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			पटिनिवत्त रिक्त;
		}
		विकल्प(new_context->listener->protocol){
			पक्ष mp_mqtt:
				new_context->transport = mosq_t_tcp;
				भङ्ग;
#सचे defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			पक्ष mp_websockets:
				सचे(http__context_init(new_context)){
					context__cleanup(new_context, true);
					पटिनिवत्त रिक्त;
				}
				भङ्ग;
#endif
			स्वभाव:
				context__cleanup(new_context, true);
				पटिनिवत्त रिक्त;
		}
	}

	सचे((new_context->listener->max_connections > 0 && new_context->listener->client_गणनम् > new_context->listener->max_connections)
			|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (unsigned पूर्णाङ्क)db.config->global_max_connections)){

		सचे(db.config->connection_messages == true){
			log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", new_context->address);
		}
		context__cleanup(new_context, true);
		पटिनिवत्त रिक्त;
	}

#ifdef WITH_TLS
	/* TLS init */
	सचे(new_context->listener->ssl_ctx){
		new_context->ssl = SSL_new(new_context->listener->ssl_ctx);
		सचे(!new_context->ssl){
			context__cleanup(new_context, true);
			पटिनिवत्त रिक्त;
		}
		सचे(!SSL_set_ex_data(new_context->ssl, tls_ex_index_context, new_context)
				|| !SSL_set_ex_data(new_context->ssl, tls_ex_index_listener, new_context->listener)){
			context__cleanup(new_context, true);
			पटिनिवत्त रिक्त;
		}
		new_context->want_write = true;
		bio = BIO_new_socket(new_sock, BIO_NOCLOSE);
		SSL_set_bio(new_context->ssl, bio, bio);
		ERR_clear_error();
		SSL_set_accept_state(new_context->ssl);
	}
#endif

	सचे(db.config->connection_messages == true
			&& !new_context->listener->enable_proxy_protocol){

		log__printf(रिक्त, MOSQ_LOG_NOTICE, "New connection from %s:%d on port %d.",
				new_context->address, new_context->remote_port, new_context->listener->port);
	}

	mux__new(new_context);
	keepalive__add(new_context);

	पटिनिवत्त new_context;
}

#ifdef WITH_TLS


स्थिर पूर्णाङ्क client_certificate_verify(पूर्णाङ्क preverify_ok, X509_STORE_CTX *ctx)
{
	/* Preverify should check expiry, revocation. */
	सचे(preverify_ok == 0){
		SSL *ssl;
		ssl = X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx());
		सचे(ssl){
			संरचना mosquitto__listener *listener;
			listener = SSL_get_ex_data(ssl, tls_ex_index_listener);

			सचे(listener && listener->disable_client_cert_date_checks){
				पूर्णाङ्क err = X509_STORE_CTX_get_error(ctx);
				सचे(err == X509_V_ERR_CERT_NOT_YET_VALID || err == X509_V_ERR_CERT_HAS_EXPIRED){
					preverify_ok = 1;
				}
			}
		}
	}
	पटिनिवत्त preverify_ok;
}
#endif

#ifdef FINAL_WITH_TLS_PSK


स्थिर unsigned पूर्णाङ्क psk_server_callback(SSL *ssl, ध्रुव वर्ण *identity, unsigned वर्ण *psk, unsigned पूर्णाङ्क max_psk_len)
{
	संरचना mosquitto *context;
	संरचना mosquitto__listener *listener;
	वर्ण *psk_कुञ्चिका = रिक्त;
	पूर्णाङ्क len;
	ध्रुव वर्ण *psk_hint;

	सचे(!identity){
		पटिनिवत्त 0;
	}

	context = SSL_get_ex_data(ssl, tls_ex_index_context);
	सचे(!context){
		पटिनिवत्त 0;
	}

	listener = SSL_get_ex_data(ssl, tls_ex_index_listener);
	सचे(!listener){
		पटिनिवत्त 0;
	}

	psk_hint = listener->psk_hint;

	/* The hex to BN conversion results in the length halving, so we can pass
	 * max_psk_len*2 as the max hex key here. */
	psk_कुञ्चिका = mosquitto_calloc(1, (size_t)max_psk_len*2 + 1);
	सचे(!psk_कुञ्चिका){
		पटिनिवत्त 0;
	}

	सचे(mosquitto_psk_कुञ्चिका_get(context, psk_hint, identity, psk_कुञ्चिका, (पूर्णाङ्क)max_psk_len*2) != MOSQ_ERR_SUCCESS){
		mosquitto_FREE(psk_कुञ्चिका);
		पटिनिवत्त 0;
	}

	len = mosquitto__hex2bin(psk_कुञ्चिका, psk, (पूर्णाङ्क)max_psk_len);
	सचे(len < 0){
		mosquitto_FREE(psk_कुञ्चिका);
		पटिनिवत्त 0;
	}

	सचे(listener->use_identity_as_username){
		सचे(mosquitto_validate_utf8(identity, (पूर्णाङ्क)strlen(identity))){
			mosquitto_free(psk_कुञ्चिका);
			पटिनिवत्त 0;
		}
		context->username = mosquitto_strdup(identity);
		सचे(!context->username){
			mosquitto_FREE(psk_कुञ्चिका);
			पटिनिवत्त 0;
		}
	}

	mosquitto_FREE(psk_कुञ्चिका);
	पटिनिवत्त (unsigned पूर्णाङ्क)len;
}
#endif

#ifdef WITH_TLS


स्थिर शून्य tls_keylog_callback(ध्रुव SSL *ssl, ध्रुव वर्ण *पङ्क्तिः)
{
	UNUSED(ssl);

	सचे(db.tls_keylog){
		FILE *fptr;
		fptr = mosquitto_fopen(db.tls_keylog, "at", true);
		सचे(fptr){
#ifndef WIN32
			/* Until mosquitto_fopen enforces file permissions on all files,
			 * enforce it here. We can enforce it here, because it isn't a
			 * change of behaviour. */
			संरचना stat statbuf;
			सचे(fstat(fileno(fptr), &statbuf) < 0
					|| statbuf.st_mode & (S_IRWXG | S_IRWXO)){

				fclose(fptr);
				पटिनिवत्त;
			}
#endif
			fprintf(fptr, "%s\n", पङ्क्तिः);
			fclose(fptr);
		}अञ्ञथा{
#ifndef WIN32
			सचे(errno == ELOOP){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Error: keylog file must not be a symbolic link");
			}अञ्ञथा
#endif
			{
				log__printf(रिक्त, MOSQ_LOG_INFO, "Error: Unable to open keylog file: %s", strerror(errno));
			}
		}
	}
}


पूर्णाङ्क net__tls_server_ctx(संरचना mosquitto__listener *listener)
{
	वर्ण buf[256];
	पूर्णाङ्क rc;

	सचे(tls_ex_index_context == -1){
		tls_ex_index_context = SSL_get_ex_new_index(0, "client context", रिक्त, रिक्त, रिक्त);
	}
	सचे(tls_ex_index_listener == -1){
		tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", रिक्त, रिक्त, रिक्त);
	}

	सचे(listener->ssl_ctx){
		SSL_CTX_free(listener->ssl_ctx);
		listener->ssl_ctx = रिक्त;
	}

	listener->ssl_ctx = SSL_CTX_new(TLS_server_method());

	सचे(!listener->ssl_ctx){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to create TLS context.");
		पटिनिवत्त MOSQ_ERR_TLS;
	}

#ifdef SSL_OP_NO_TLSv1_3
	सचे(db.config->per_listener_settings){
		सचे(listener->security_options->psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}अञ्ञथा{
		सचे(db.config->security_options.psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}
#endif

	सचे(listener->tls_version == रिक्त){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
#ifdef SSL_OP_NO_TLSv1_3
	}अञ्ञथा सचे(!strcmp(listener->tls_version, "tlsv1.3")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2);
#endif
	}अञ्ञथा सचे(!strcmp(listener->tls_version, "tlsv1.2")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
	}अञ्ञथा सचे(!strcmp(listener->tls_version, "tlsv1.1")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1);
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS v1.1 is insecure. It must only be used in the case "
				"where you have devices that cannot be upgraded to use a secure version of TLS. It is "
				"recommended to use as secure a set of ciphers as possible and that will restrict it to "
				"TLS v1.1 only, and to use a separate listener using TLS v1.2 and secure ciphers for your "
				"other devices.");
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Please be aware that support for TLS v1.1 will go away eventually "
				"and that you should plan now to migrate away from it.");
	}अञ्ञथा{
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unsupported tls_version \"%s\".", listener->tls_version);
		पटिनिवत्त MOSQ_ERR_TLS;
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

	सचे(db.tls_keylog){
		log__printf(रिक्त, MOSQ_LOG_NOTICE, "TLS key logging to '%s' enabled for all listeners.",
				db.tls_keylog);
		log__printf(रिक्त, MOSQ_LOG_NOTICE, "TLS key logging is for DEBUGGING only.");

		SSL_CTX_set_keylog_callback(listener->ssl_ctx, tls_keylog_callback);
	}

	snprintf(buf, 256, "mosquitto-%d", listener->port);
	SSL_CTX_set_session_id_context(listener->ssl_ctx, (unsigned वर्ण *)buf, (unsigned पूर्णाङ्क)strlen(buf));

	सचे(listener->ciphers){
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, listener->ciphers);
		सचे(rc == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			पटिनिवत्त MOSQ_ERR_TLS;
		}
	}अञ्ञथा{
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, "DEFAULT:!aNULL:!eNULL:!LOW:!EXPORT:!SSLv2:@STRENGTH");
		सचे(rc == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			पटिनिवत्त MOSQ_ERR_TLS;
		}
	}
	सचे(listener->ciphers_tls13){
		rc = SSL_CTX_set_ciphersuites(listener->ssl_ctx, listener->ciphers_tls13);
		सचे(rc == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to set TLS 1.3 ciphersuites. Check cipher_tls13 list \"%s\".", listener->ciphers_tls13);
			पटिनिवत्त MOSQ_ERR_TLS;
		}
	}

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}
#endif


#ifdef WITH_TLS


स्थिर पूर्णाङ्क net__load_crl_file(संरचना mosquitto__listener *listener)
{
	X509_STORE *store;
	X509_LOOKUP *lookup;
	पूर्णाङ्क rc;

	store = SSL_CTX_get_cert_store(listener->ssl_ctx);
	सचे(!store){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to obtain TLS store.");
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		पटिनिवत्त MOSQ_ERR_TLS;
	}
	lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
	rc = X509_load_crl_file(lookup, listener->crlfile, X509_FILETYPE_PEM);
	सचे(rc < 1){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load certificate revocation file \"%s\". Check crlfile.", listener->crlfile);
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		net__print_ssl_error(रिक्त, रिक्त);
		पटिनिवत्त MOSQ_ERR_TLS;
	}
	X509_STORE_set_flags(store, X509_V_FLAG_CRL_CHECK);

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}
#endif


पूर्णाङ्क net__load_certificates(संरचना mosquitto__listener *listener)
{
#ifdef WITH_TLS
	पूर्णाङ्क rc;

	सचे(listener->require_certificate){
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, client_certificate_verify);
	}अञ्ञथा{
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_NONE, client_certificate_verify);
	}
	rc = SSL_CTX_use_certificate_chain_file(listener->ssl_ctx, listener->certfile);
	सचे(rc != 1){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
		net__print_ssl_error(रिक्त, रिक्त);
		पटिनिवत्त MOSQ_ERR_TLS;
	}
	सचे(listener->tls_engine == रिक्त || listener->tls_keyform == mosq_k_pem){
		rc = SSL_CTX_use_Private_कुञ्चिका_file(listener->ssl_ctx, listener->keyfile, SSL_FILETYPE_PEM);
		सचे(rc != 1){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			net__print_ssl_error(रिक्त, रिक्त);
			पटिनिवत्त MOSQ_ERR_TLS;
		}
	}
	rc = SSL_CTX_check_private_कुञ्चिका(listener->ssl_ctx);
	सचे(rc != 1){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Server certificate/key are inconsistent.");
		net__print_ssl_error(रिक्त, रिक्त);
		पटिनिवत्त MOSQ_ERR_TLS;
	}
	/* Load CRLs if they exist. */
	सचे(listener->crlfile){
		rc = net__load_crl_file(listener);
		सचे(rc){
			पटिनिवत्त rc;
		}
	}
#अञ्ञथा
	UNUSED(listener);
#endif
	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


#सचे defined(WITH_TLS) && !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000


स्थिर पूर्णाङ्क net__load_engine(संरचना mosquitto__listener *listener)
{
	ENGINE *engine = रिक्त;
	UI_METHOD *ui_method;
	EVP_PKEY *pkey;

	सचे(!listener->tls_engine){
		पटिनिवत्त MOSQ_ERR_SUCCESS;
	}

	engine = ENGINE_by_id(listener->tls_engine);
	सचे(!engine){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error loading %s engine\n", listener->tls_engine);
		net__print_ssl_error(रिक्त, रिक्त);
		पटिनिवत्त MOSQ_ERR_TLS;
	}
	सचे(!ENGINE_init(engine)){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Failed engine initialisation\n");
		net__print_ssl_error(रिक्त, रिक्त);
		पटिनिवत्त MOSQ_ERR_TLS;
	}
	ENGINE_set_default(engine, ENGINE_METHOD_ALL);

	सचे(listener->tls_keyform == mosq_k_engine){
		ui_method = net__get_ui_method();
		सचे(listener->tls_engine_kpass_sha1){
			सचे(!ENGINE_ctrl_cmd(engine, ENGINE_SECRET_MODE, ENGINE_SECRET_MODE_SHA, रिक्त, रिक्त, 0)){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to set engine secret mode sha");
				net__print_ssl_error(रिक्त, रिक्त);
				पटिनिवत्त MOSQ_ERR_TLS;
			}
			सचे(!ENGINE_ctrl_cmd(engine, ENGINE_PIN, 0, listener->tls_engine_kpass_sha1, रिक्त, 0)){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to set engine pin");
				net__print_ssl_error(रिक्त, रिक्त);
				पटिनिवत्त MOSQ_ERR_TLS;
			}
			ui_method = रिक्त;
		}
		pkey = ENGINE_load_private_कुञ्चिका(engine, listener->keyfile, ui_method, रिक्त);
		सचे(!pkey){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(रिक्त, रिक्त);
			पटिनिवत्त MOSQ_ERR_TLS;
		}
		सचे(SSL_CTX_use_Private_कुञ्चिका(listener->ssl_ctx, pkey) <= 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to use engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(रिक्त, रिक्त);
			पटिनिवत्त MOSQ_ERR_TLS;
		}
	}
	ENGINE_free(engine); /* release the structural reference from ENGINE_by_id() */

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}
#endif


पूर्णाङ्क net__tls_load_verify(संरचना mosquitto__listener *listener)
{
#ifdef WITH_TLS
	पूर्णाङ्क rc;

#  सचे OPENSSL_VERSION_सङ्ख्या < 0x30000000L
	सचे(listener->cafile || listener->capath){
		rc = SSL_CTX_load_verify_locations(listener->ssl_ctx, listener->cafile, listener->capath);
		सचे(rc == 0){
			सचे(listener->cafile && listener->capath){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\" and capath \"%s\".", listener->cafile, listener->capath);
			}अञ्ञथा सचे(listener->cafile){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			}अञ्ञथा{
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			}
		}
	}
#  अञ्ञथा
	राशिः_OF(X509_NAME) *ca_names = रिक्त;
	सचे(listener->cafile || listener->capath){
		ca_names = sk_X509_NAME_new_null();
	}

	सचे(listener->cafile){
		rc = SSL_CTX_load_verify_file(listener->ssl_ctx, listener->cafile);
		सचे(rc == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			net__print_ssl_error(रिक्त, रिक्त);
			पटिनिवत्त MOSQ_ERR_TLS;
		}

		राशिः_OF(X509_NAME) *cert_names = SSL_load_client_CA_file(listener->cafile);
		सचे(cert_names){
			अत्थाय(पूर्णाङ्क i=0; i<sk_X509_NAME_अङ्कः(cert_names); i++){
				sk_X509_NAME_प्रक्षिप(ca_names, X509_NAME_dup(sk_X509_NAME_मानम्(cert_names, i)));
			}
			sk_X509_NAME_निष्कासय_free(cert_names, X509_NAME_free);
		}

	}
	सचे(listener->capath){
		rc = SSL_CTX_load_verify_dir(listener->ssl_ctx, listener->capath);
		सचे(rc == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			net__print_ssl_error(रिक्त, रिक्त);
			sk_X509_NAME_निष्कासय_free(ca_names, X509_NAME_free);
			पटिनिवत्त MOSQ_ERR_TLS;
		}

		SSL_add_dir_cert_subjects_to_राशिः(ca_names, listener->capath);
	}
	सचे(ca_names){
		SSL_CTX_set_client_CA_list(listener->ssl_ctx, ca_names);
	}
#  endif

#  सचे !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000
	सचे(net__load_engine(listener)){
		पटिनिवत्त MOSQ_ERR_TLS;
	}
#  endif
#endif
	पटिनिवत्त net__load_certificates(listener);
}


#सचे !defined(WIN32) && !defined(_AIX)


स्थिर पूर्णाङ्क net__bind_interface(संरचना mosquitto__listener *listener, संरचना addrinfo *rp)
{
	/*
	 * This binds the listener sock to a network interface.
	 * The use of SO_BINDTODEVICE requires root access, which we don't have, so instead
	 * use getifaddrs to find the interface addresses, and use IP of the
	 * matching interface in the later bind().
	 */
	संरचना ifaddrs *ifaddr;
	bool have_interface = false;

	सचे(getifaddrs(&ifaddr) < 0){
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		पटिनिवत्त MOSQ_ERR_ERRNO;
	}

	अत्थाय(संरचना ifaddrs *ifa=ifaddr; ifa!=रिक्त; ifa=ifa->ifa_next){
		सचे(ifa->ifa_addr == रिक्त){
			अनुवर्त;
		}

		सचे(!strcasecmp(listener->bind_interface, ifa->ifa_name)){
			have_interface = true;

			सचे(ifa->ifa_addr->sa_family == rp->ai_addr->sa_family){
				सचे(rp->ai_addr->sa_family == AF_INET){
					सचे(listener->host &&
							memcmp(&((संरचना sockaddr_in *)rp->ai_addr)->sin_addr,
							&((संरचना sockaddr_in *)ifa->ifa_addr)->sin_addr,
							परिमाण(संरचना in_addr))){

						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						पटिनिवत्त MOSQ_ERR_INVAL;
					}अञ्ञथा{
						memcpy(&((संरचना sockaddr_in *)rp->ai_addr)->sin_addr,
								&((संरचना sockaddr_in *)ifa->ifa_addr)->sin_addr,
								परिमाण(संरचना in_addr));

						freeifaddrs(ifaddr);
						पटिनिवत्त MOSQ_ERR_SUCCESS;
					}
				}अञ्ञथा सचे(rp->ai_addr->sa_family == AF_INET6){
					सचे(listener->host &&
							memcmp(&((संरचना sockaddr_in6 *)rp->ai_addr)->sin6_addr,
							&((संरचना sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
							परिमाण(संरचना in6_addr))){

						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						पटिनिवत्त MOSQ_ERR_INVAL;
					}अञ्ञथा{
						memcpy(&((संरचना sockaddr_in6 *)rp->ai_addr)->sin6_addr,
								&((संरचना sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
								परिमाण(संरचना in6_addr));

						((संरचना sockaddr_in6 *)rp->ai_addr)->sin6_scope_id = ((संरचना sockaddr_in6 *)ifa->ifa_addr)->sin6_scope_id;

						freeifaddrs(ifaddr);
						पटिनिवत्त MOSQ_ERR_SUCCESS;
					}
				}
			}
		}
	}
	freeifaddrs(ifaddr);
	सचे(have_interface){
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Interface %s does not support %s configuration.",
				listener->bind_interface, rp->ai_addr->sa_family == AF_INET ? "IPv4" : "IPv6");
		पटिनिवत्त MOSQ_ERR_NOT_SUPPORTED;
	}अञ्ञथा{
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Interface %s does not exist.", listener->bind_interface);
		पटिनिवत्त MOSQ_ERR_NOT_FOUND;
	}
}
#endif


स्थिर पूर्णाङ्क net__socket_listen_tcp(संरचना mosquitto__listener *listener)
{
	mosq_sock_t sock = INVALID_SOCKET;
	संरचना addrinfo hints;
	संरचना addrinfo *ainfo, *rp;
	वर्ण service[10];
	पूर्णाङ्क rc;
	पूर्णाङ्क ss_opt = 1;
#ifndef WIN32
	bool interface_bound = false;
#endif

	सचे(!listener){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}

	snprintf(service, 10, "%d", listener->port);
	memset(&hints, 0, परिमाण(संरचना addrinfo));
	सचे(listener->socket_domain){
		hints.ai_family = listener->socket_domain;
	}अञ्ञथा{
		hints.ai_family = AF_UNSPEC;
	}
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	rc = getaddrinfo(listener->host, service, &hints, &ainfo);
	सचे(rc){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating listener: %s.", gai_strerror(rc));
		पटिनिवत्त INVALID_SOCKET;
	}

	listener->sock_गणनम् = 0;
	listener->socks = रिक्त;

	अत्थाय(rp = ainfo; rp; rp = rp->ai_next){
		सचे(rp->ai_family == AF_INET){
			log__printf(रिक्त, MOSQ_LOG_INFO, "Opening ipv4 listen socket on port %d.", ntohs(((संरचना sockaddr_in *)rp->ai_addr)->sin_port));
		}अञ्ञथा सचे(rp->ai_family == AF_INET6){
			log__printf(रिक्त, MOSQ_LOG_INFO, "Opening ipv6 listen socket on port %d.", ntohs(((संरचना sockaddr_in6 *)rp->ai_addr)->sin6_port));
		}अञ्ञथा{
			अनुवर्त;
		}

		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		सचे(sock == INVALID_SOCKET){
			net__print_error(MOSQ_LOG_WARNING, "Warning: %s");
			अनुवर्त;
		}
		listener->sock_गणनम्++;
		listener->socks = mosquitto_realloc(listener->socks, परिमाण(mosq_sock_t)*(size_t)listener->sock_गणनम्);
		सचे(!listener->socks){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			पटिनिवत्त MOSQ_ERR_NOMEM;
		}
		listener->socks[listener->sock_गणनम्-1] = sock;

#ifndef WIN32
		ss_opt = 1;
		/* Unimportant if this fails */
		(शून्य)setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (ध्रुव शून्य *)&ss_opt, परिमाण(ss_opt));
#endif
#ifdef IPV6_V6ONLY
		ss_opt = 1;
		(शून्य)setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (ध्रुव शून्य *)&ss_opt, परिमाण(ss_opt));
#endif

		सचे(net__socket_nonblock(&sock)){
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			पटिनिवत्त 1;
		}

#सचे !defined(WIN32) && !defined(_AIX)
		सचे(listener->bind_interface){
			/* It might be possible that an interface does not support all relevant sa_families.
			 * We should successfully find at least one. */
			rc = net__bind_interface(listener, rp);
			सचे(rc){
				COMPAT_CLOSE(sock);
				listener->sock_गणनम्--;
				सचे(rc == MOSQ_ERR_NOT_FOUND || rc == MOSQ_ERR_INVAL){
					freeaddrinfo(ainfo);
					पटिनिवत्त rc;
				}अञ्ञथा{
					अनुवर्त;
				}
			}
			interface_bound = true;
		}
#endif

		सचे(bind(sock, rp->ai_addr, rp->ai_addrlen) == -1){
#सचे defined(__linux__)
			सचे(errno == EACCES){
				log__printf(रिक्त, MOSQ_LOG_ERR, "If you are trying to bind to a privileged port (<1024), try using setcap and do not start the broker as root:");
				log__printf(रिक्त, MOSQ_LOG_ERR, "    sudo setcap 'CAP_NET_BIND_SERVICE=+ep /usr/sbin/mosquitto'");
			}
#endif
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			COMPAT_CLOSE(sock);
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			पटिनिवत्त 1;
		}

		सचे(listen(sock, 100) == -1){
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			mosquitto_FREE(listener->socks);
			पटिनिवत्त 1;
		}
	}
	freeaddrinfo(ainfo);

#ifndef WIN32
	सचे(listener->bind_interface && !interface_bound){
		mosquitto_FREE(listener->socks);
		पटिनिवत्त 1;
	}
#endif

	पटिनिवत्त 0;
}


#ifdef WITH_UNIX_SOCKETS


स्थिर पूर्णाङ्क net__socket_listen_unix(संरचना mosquitto__listener *listener)
{
	संरचना sockaddr_un addr;
	mosq_sock_t sock = INVALID_SOCKET;
	पूर्णाङ्क rc;
#ifndef WIN32
	mode_t old_mask;
#endif

	सचे(listener->unix_socket_path == रिक्त){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}
	सचे(strlen(listener->unix_socket_path) > परिमाण(addr.sun_path)-1){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Path to unix socket is too long \"%s\".", listener->unix_socket_path);
		पटिनिवत्त MOSQ_ERR_INVAL;
	}

#ifdef WIN32
	DeleteFile(listener->unix_socket_path);
#अञ्ञथा
	unlink(listener->unix_socket_path);
#endif
	log__printf(रिक्त, MOSQ_LOG_INFO, "Opening unix listen socket on path %s.", listener->unix_socket_path);
	memset(&addr, 0, परिमाण(संरचना sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, listener->unix_socket_path, परिमाण(addr.sun_path)-1);

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	सचे(sock == INVALID_SOCKET){
		net__print_error(MOSQ_LOG_ERR, "Error creating unix socket: %s");
		पटिनिवत्त 1;
	}
	listener->sock_गणनम्++;
	listener->socks = mosquitto_realloc(listener->socks, परिमाण(mosq_sock_t)*(size_t)listener->sock_गणनम्);
	सचे(!listener->socks){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		COMPAT_CLOSE(sock);
		पटिनिवत्त MOSQ_ERR_NOMEM;
	}
	listener->socks[listener->sock_गणनम्-1] = sock;


#ifndef WIN32
	old_mask = umask(0007);
#endif
	rc = bind(sock, (संरचना sockaddr *)&addr, परिमाण(संरचना sockaddr_un));
#ifndef WIN32
	umask(old_mask);
#endif

	सचे(rc == -1){
		net__print_error(MOSQ_LOG_ERR, "Error binding unix socket: %s");
		पटिनिवत्त 1;
	}

	सचे(listen(sock, 10) == -1){
		net__print_error(MOSQ_LOG_ERR, "Error listening to unix socket: %s");
		पटिनिवत्त 1;
	}

	सचे(net__socket_nonblock(&sock)){
		पटिनिवत्त 1;
	}

	पटिनिवत्त 0;
}
#endif


/* Creates a socket and listens on port 'port'.
 * Returns 1 on failure
 * Returns 0 on success.
 */
पूर्णाङ्क net__socket_listen(संरचना mosquitto__listener *listener)
{
	पूर्णाङ्क rc;

	सचे(!listener){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}

#ifdef WITH_UNIX_SOCKETS
	सचे(listener->port == 0 && listener->unix_socket_path != रिक्त){
		rc = net__socket_listen_unix(listener);
	}अञ्ञथा
#endif
	{
		rc = net__socket_listen_tcp(listener);
	}
	सचे(rc){
		पटिनिवत्त rc;
	}

	/* We need to have at least one working socket. */
	सचे(listener->sock_गणनम् > 0){
#ifdef WITH_TLS
		सचे(listener->certfile && listener->keyfile){
			सचे(net__tls_server_ctx(listener)){
				पटिनिवत्त 1;
			}

			सचे(net__tls_load_verify(listener)){
				पटिनिवत्त 1;
			}
		}
#  ifdef FINAL_WITH_TLS_PSK
		सचे(listener->psk_hint){
			सचे(listener->certfile == रिक्त || listener->keyfile == रिक्त){
				सचे(net__tls_server_ctx(listener)){
					पटिनिवत्त 1;
				}
			}
			SSL_CTX_set_psk_server_callback(listener->ssl_ctx, psk_server_callback);
			सचे(listener->psk_hint){
				rc = SSL_CTX_use_psk_identity_hint(listener->ssl_ctx, listener->psk_hint);
				सचे(rc == 0){
					log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to set TLS PSK hint.");
					net__print_ssl_error(रिक्त, रिक्त);
					पटिनिवत्त 1;
				}
			}
		}
#  endif /* FINAL_WITH_TLS_PSK */
		सचे(tls_ex_index_context == -1){
			tls_ex_index_context = SSL_get_ex_new_index(0, "client context", रिक्त, रिक्त, रिक्त);
		}
		सचे(tls_ex_index_listener == -1){
			tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", रिक्त, रिक्त, रिक्त);
		}
#endif /* WITH_TLS */
		पटिनिवत्त 0;
	}अञ्ञथा{
		पटिनिवत्त 1;
	}
}


पूर्णाङ्क net__socket_get_address(mosq_sock_t sock, वर्ण *buf, size_t len, uint16_t *remote_port)
{
	संरचना sockaddr_storage addr;
	socklen_t addrlen;

	memset(&addr, 0, परिमाण(संरचना sockaddr_storage));
	addrlen = परिमाण(addr);
	सचे(!getpeername(sock, (संरचना sockaddr *)&addr, &addrlen)){
		सचे(addr.ss_family == AF_INET){
			सचे(remote_port){
				*remote_port = ntohs(((संरचना sockaddr_in *)&addr)->sin_port);
			}
			सचे(inet_ntop(AF_INET, &((संरचना sockaddr_in *)&addr)->sin_addr.s_addr, buf, (socklen_t)len)){
				पटिनिवत्त 0;
			}
		}अञ्ञथा सचे(addr.ss_family == AF_INET6){
			सचे(remote_port){
				*remote_port = ntohs(((संरचना sockaddr_in6 *)&addr)->sin6_port);
			}
			सचे(inet_ntop(AF_INET6, &((संरचना sockaddr_in6 *)&addr)->sin6_addr.s6_addr, buf, (socklen_t)len)){
				पटिनिवत्त 0;
			}
#ifdef WITH_UNIX_SOCKETS
		}अञ्ञथा सचे(addr.ss_family == AF_UNIX){
			संरचना sockaddr_un un;
			addrlen = परिमाण(संरचना sockaddr_un);
			सचे(!getsockname(sock, (संरचना sockaddr *)&un, &addrlen)){
				snprintf(buf, len, "%s", un.sun_path);
			}अञ्ञथा{
				snprintf(buf, len, "unix-socket");
			}
			पटिनिवत्त 0;
#endif
		}
	}
	पटिनिवत्त 1;
}
