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
#अण्णहा
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

#जइ defined(WITH_UNIX_SOCKETS) || defined(WITH_TLS)
#  include "sys/stat.h"
#endif

#ifdef WITH_UNIX_SOCKETS
#  ifdef WIN32
#    include <afunix.h>
#  अण्णहा
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
	जइ(spare_sock != INVALID_SOCKET){
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
#अण्णहा
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
	जइ(new_sock == INVALID_SOCKET){
#ifdef WIN32
		WINDOWS_SET_ERRNO();
		जइ(errno == WSAEMFILE){
#अण्णहा
		जइ(errno == EMFILE || errno == ENFILE){
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
			जइ(new_sock != INVALID_SOCKET){
				COMPAT_CLOSE(new_sock);
			}
			spare_sock = socket(AF_INET, SOCK_STREAM, 0);
			log__printf(रिक्त, MOSQ_LOG_WARNING,
					"Unable to accept new connection, system socket count has been exceeded. Try increasing \"ulimit -n\" or equivalent.");
		}
		प्रत्यागम रिक्त;
	}

	metrics__int_inc(mosq_counter_socket_connections, 1);

	जइ(net__socket_nonblock(&new_sock)){
		प्रत्यागम रिक्त;
	}

#ifdef WITH_WRAP
	/* Use tcpd / libwrap to determine whether a connection is allowed. */
	request_init(&wrap_req, RQ_FILE, new_sock, RQ_DAEMON, "mosquitto", 0);
	fromhost(&wrap_req);
	जइ(!hosts_access(&wrap_req)){
		/* Access is denied */
		जइ(db.config->connection_messages == true){
			जइ(!net__socket_get_address(new_sock, address, 1024, रिक्त)){
				log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client connection from %s denied access by tcpd.", address);
			}
		}
		COMPAT_CLOSE(new_sock);
		प्रत्यागम रिक्त;
	}
#endif

	जइ(db.config->set_tcp_nodelay && listensock->listener->port){
		पूर्णाङ्क flag = 1;
		जइ(setsockopt(new_sock, IPPROTO_TCP, TCP_NODELAY, (ध्रुव शून्य *)&flag, परिमाण(पूर्णाङ्क)) != 0){
			log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Unable to set TCP_NODELAY.");
		}
	}

	new_context = context__init();
	जइ(!new_context){
		COMPAT_CLOSE(new_sock);
		प्रत्यागम रिक्त;
	}
	new_context->listener = listensock->listener;
	जइ(!new_context->listener){
		context__cleanup(new_context, true);
		प्रत्यागम रिक्त;
	}
	new_context->listener->client_गणनम्++;

	जइ(new_context->listener->enable_proxy_protocol){
		जइ(context__init_sock(new_context, new_sock, false) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			प्रत्यागम रिक्त;
		}
		जइ(new_context->listener->enable_proxy_protocol == 2){
			new_context->transport = mosq_t_proxy_v2;
		}अण्णहा{
			new_context->transport = mosq_t_proxy_v1;
		}
		new_context->proxy.cmd = -1;
	}अण्णहा{
		जइ(context__init_sock(new_context, new_sock, true) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			प्रत्यागम रिक्त;
		}
		विकल्प(new_context->listener->protocol){
			पक्ष mp_mqtt:
				new_context->transport = mosq_t_tcp;
				भङ्ग;
#जइ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			पक्ष mp_websockets:
				जइ(http__context_init(new_context)){
					context__cleanup(new_context, true);
					प्रत्यागम रिक्त;
				}
				भङ्ग;
#endif
			स्वभाव:
				context__cleanup(new_context, true);
				प्रत्यागम रिक्त;
		}
	}

	जइ((new_context->listener->max_connections > 0 && new_context->listener->client_गणनम् > new_context->listener->max_connections)
			|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (unsigned पूर्णाङ्क)db.config->global_max_connections)){

		जइ(db.config->connection_messages == true){
			log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", new_context->address);
		}
		context__cleanup(new_context, true);
		प्रत्यागम रिक्त;
	}

#ifdef WITH_TLS
	/* TLS init */
	जइ(new_context->listener->ssl_ctx){
		new_context->ssl = SSL_new(new_context->listener->ssl_ctx);
		जइ(!new_context->ssl){
			context__cleanup(new_context, true);
			प्रत्यागम रिक्त;
		}
		जइ(!SSL_set_ex_data(new_context->ssl, tls_ex_index_context, new_context)
				|| !SSL_set_ex_data(new_context->ssl, tls_ex_index_listener, new_context->listener)){
			context__cleanup(new_context, true);
			प्रत्यागम रिक्त;
		}
		new_context->want_write = true;
		bio = BIO_new_socket(new_sock, BIO_NOCLOSE);
		SSL_set_bio(new_context->ssl, bio, bio);
		ERR_clear_error();
		SSL_set_accept_state(new_context->ssl);
	}
#endif

	जइ(db.config->connection_messages == true
			&& !new_context->listener->enable_proxy_protocol){

		log__printf(रिक्त, MOSQ_LOG_NOTICE, "New connection from %s:%d on port %d.",
				new_context->address, new_context->remote_port, new_context->listener->port);
	}

	mux__new(new_context);
	keepalive__add(new_context);

	प्रत्यागम new_context;
}

#ifdef WITH_TLS


स्थिर पूर्णाङ्क client_certificate_verify(पूर्णाङ्क preverify_ok, X509_STORE_CTX *ctx)
{
	/* Preverify should check expiry, revocation. */
	जइ(preverify_ok == 0){
		SSL *ssl;
		ssl = X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx());
		जइ(ssl){
			संरचना mosquitto__listener *listener;
			listener = SSL_get_ex_data(ssl, tls_ex_index_listener);

			जइ(listener && listener->disable_client_cert_date_checks){
				पूर्णाङ्क err = X509_STORE_CTX_get_error(ctx);
				जइ(err == X509_V_ERR_CERT_NOT_YET_VALID || err == X509_V_ERR_CERT_HAS_EXPIRED){
					preverify_ok = 1;
				}
			}
		}
	}
	प्रत्यागम preverify_ok;
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

	जइ(!identity){
		प्रत्यागम 0;
	}

	context = SSL_get_ex_data(ssl, tls_ex_index_context);
	जइ(!context){
		प्रत्यागम 0;
	}

	listener = SSL_get_ex_data(ssl, tls_ex_index_listener);
	जइ(!listener){
		प्रत्यागम 0;
	}

	psk_hint = listener->psk_hint;

	/* The hex to BN conversion results in the length halving, so we can pass
	 * max_psk_len*2 as the max hex key here. */
	psk_कुञ्चिका = mosquitto_calloc(1, (size_t)max_psk_len*2 + 1);
	जइ(!psk_कुञ्चिका){
		प्रत्यागम 0;
	}

	जइ(mosquitto_psk_कुञ्चिका_get(context, psk_hint, identity, psk_कुञ्चिका, (पूर्णाङ्क)max_psk_len*2) != MOSQ_ERR_SUCCESS){
		mosquitto_FREE(psk_कुञ्चिका);
		प्रत्यागम 0;
	}

	len = mosquitto__hex2bin(psk_कुञ्चिका, psk, (पूर्णाङ्क)max_psk_len);
	जइ(len < 0){
		mosquitto_FREE(psk_कुञ्चिका);
		प्रत्यागम 0;
	}

	जइ(listener->use_identity_as_username){
		जइ(mosquitto_validate_utf8(identity, (पूर्णाङ्क)strlen(identity))){
			mosquitto_free(psk_कुञ्चिका);
			प्रत्यागम 0;
		}
		context->username = mosquitto_strdup(identity);
		जइ(!context->username){
			mosquitto_FREE(psk_कुञ्चिका);
			प्रत्यागम 0;
		}
	}

	mosquitto_FREE(psk_कुञ्चिका);
	प्रत्यागम (unsigned पूर्णाङ्क)len;
}
#endif

#ifdef WITH_TLS


स्थिर शून्य tls_keylog_callback(ध्रुव SSL *ssl, ध्रुव वर्ण *पङ्क्तिः)
{
	UNUSED(ssl);

	जइ(db.tls_keylog){
		FILE *fptr;
		fptr = mosquitto_fopen(db.tls_keylog, "at", true);
		जइ(fptr){
#ifndef WIN32
			/* Until mosquitto_fopen enforces file permissions on all files,
			 * enforce it here. We can enforce it here, because it isn't a
			 * change of behaviour. */
			संरचना stat statbuf;
			जइ(fstat(fileno(fptr), &statbuf) < 0
					|| statbuf.st_mode & (S_IRWXG | S_IRWXO)){

				fclose(fptr);
				प्रत्यागम;
			}
#endif
			fprintf(fptr, "%s\n", पङ्क्तिः);
			fclose(fptr);
		}अण्णहा{
#ifndef WIN32
			जइ(errno == ELOOP){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Error: keylog file must not be a symbolic link");
			}अण्णहा
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

	जइ(tls_ex_index_context == -1){
		tls_ex_index_context = SSL_get_ex_new_index(0, "client context", रिक्त, रिक्त, रिक्त);
	}
	जइ(tls_ex_index_listener == -1){
		tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", रिक्त, रिक्त, रिक्त);
	}

	जइ(listener->ssl_ctx){
		SSL_CTX_free(listener->ssl_ctx);
		listener->ssl_ctx = रिक्त;
	}

	listener->ssl_ctx = SSL_CTX_new(TLS_server_method());

	जइ(!listener->ssl_ctx){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to create TLS context.");
		प्रत्यागम MOSQ_ERR_TLS;
	}

#ifdef SSL_OP_NO_TLSv1_3
	जइ(db.config->per_listener_settings){
		जइ(listener->security_options->psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}अण्णहा{
		जइ(db.config->security_options.psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}
#endif

	जइ(listener->tls_version == रिक्त){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
#ifdef SSL_OP_NO_TLSv1_3
	}अण्णहा जइ(!strcmp(listener->tls_version, "tlsv1.3")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2);
#endif
	}अण्णहा जइ(!strcmp(listener->tls_version, "tlsv1.2")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
	}अण्णहा जइ(!strcmp(listener->tls_version, "tlsv1.1")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1);
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS v1.1 is insecure. It must only be used in the case "
				"where you have devices that cannot be upgraded to use a secure version of TLS. It is "
				"recommended to use as secure a set of ciphers as possible and that will restrict it to "
				"TLS v1.1 only, and to use a separate listener using TLS v1.2 and secure ciphers for your "
				"other devices.");
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Please be aware that support for TLS v1.1 will go away eventually "
				"and that you should plan now to migrate away from it.");
	}अण्णहा{
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unsupported tls_version \"%s\".", listener->tls_version);
		प्रत्यागम MOSQ_ERR_TLS;
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

	जइ(db.tls_keylog){
		log__printf(रिक्त, MOSQ_LOG_NOTICE, "TLS key logging to '%s' enabled for all listeners.",
				db.tls_keylog);
		log__printf(रिक्त, MOSQ_LOG_NOTICE, "TLS key logging is for DEBUGGING only.");

		SSL_CTX_set_keylog_callback(listener->ssl_ctx, tls_keylog_callback);
	}

	snprintf(buf, 256, "mosquitto-%d", listener->port);
	SSL_CTX_set_session_id_context(listener->ssl_ctx, (unsigned वर्ण *)buf, (unsigned पूर्णाङ्क)strlen(buf));

	जइ(listener->ciphers){
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, listener->ciphers);
		जइ(rc == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			प्रत्यागम MOSQ_ERR_TLS;
		}
	}अण्णहा{
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, "DEFAULT:!aNULL:!eNULL:!LOW:!EXPORT:!SSLv2:@STRENGTH");
		जइ(rc == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			प्रत्यागम MOSQ_ERR_TLS;
		}
	}
	जइ(listener->ciphers_tls13){
		rc = SSL_CTX_set_ciphersuites(listener->ssl_ctx, listener->ciphers_tls13);
		जइ(rc == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to set TLS 1.3 ciphersuites. Check cipher_tls13 list \"%s\".", listener->ciphers_tls13);
			प्रत्यागम MOSQ_ERR_TLS;
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}
#endif


#ifdef WITH_TLS


स्थिर पूर्णाङ्क net__load_crl_file(संरचना mosquitto__listener *listener)
{
	X509_STORE *store;
	X509_LOOKUP *lookup;
	पूर्णाङ्क rc;

	store = SSL_CTX_get_cert_store(listener->ssl_ctx);
	जइ(!store){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to obtain TLS store.");
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		प्रत्यागम MOSQ_ERR_TLS;
	}
	lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
	rc = X509_load_crl_file(lookup, listener->crlfile, X509_FILETYPE_PEM);
	जइ(rc < 1){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load certificate revocation file \"%s\". Check crlfile.", listener->crlfile);
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		net__print_ssl_error(रिक्त, रिक्त);
		प्रत्यागम MOSQ_ERR_TLS;
	}
	X509_STORE_set_flags(store, X509_V_FLAG_CRL_CHECK);

	प्रत्यागम MOSQ_ERR_SUCCESS;
}
#endif


पूर्णाङ्क net__load_certificates(संरचना mosquitto__listener *listener)
{
#ifdef WITH_TLS
	पूर्णाङ्क rc;

	जइ(listener->require_certificate){
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, client_certificate_verify);
	}अण्णहा{
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_NONE, client_certificate_verify);
	}
	rc = SSL_CTX_use_certificate_chain_file(listener->ssl_ctx, listener->certfile);
	जइ(rc != 1){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
		net__print_ssl_error(रिक्त, रिक्त);
		प्रत्यागम MOSQ_ERR_TLS;
	}
	जइ(listener->tls_engine == रिक्त || listener->tls_keyform == mosq_k_pem){
		rc = SSL_CTX_use_Private_कुञ्चिका_file(listener->ssl_ctx, listener->keyfile, SSL_FILETYPE_PEM);
		जइ(rc != 1){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			net__print_ssl_error(रिक्त, रिक्त);
			प्रत्यागम MOSQ_ERR_TLS;
		}
	}
	rc = SSL_CTX_check_private_कुञ्चिका(listener->ssl_ctx);
	जइ(rc != 1){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Server certificate/key are inconsistent.");
		net__print_ssl_error(रिक्त, रिक्त);
		प्रत्यागम MOSQ_ERR_TLS;
	}
	/* Load CRLs if they exist. */
	जइ(listener->crlfile){
		rc = net__load_crl_file(listener);
		जइ(rc){
			प्रत्यागम rc;
		}
	}
#अण्णहा
	UNUSED(listener);
#endif
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


#जइ defined(WITH_TLS) && !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000


स्थिर पूर्णाङ्क net__load_engine(संरचना mosquitto__listener *listener)
{
	ENGINE *engine = रिक्त;
	UI_METHOD *ui_method;
	EVP_PKEY *pkey;

	जइ(!listener->tls_engine){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	engine = ENGINE_by_id(listener->tls_engine);
	जइ(!engine){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error loading %s engine\n", listener->tls_engine);
		net__print_ssl_error(रिक्त, रिक्त);
		प्रत्यागम MOSQ_ERR_TLS;
	}
	जइ(!ENGINE_init(engine)){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Failed engine initialisation\n");
		net__print_ssl_error(रिक्त, रिक्त);
		प्रत्यागम MOSQ_ERR_TLS;
	}
	ENGINE_set_default(engine, ENGINE_METHOD_ALL);

	जइ(listener->tls_keyform == mosq_k_engine){
		ui_method = net__get_ui_method();
		जइ(listener->tls_engine_kpass_sha1){
			जइ(!ENGINE_ctrl_cmd(engine, ENGINE_SECRET_MODE, ENGINE_SECRET_MODE_SHA, रिक्त, रिक्त, 0)){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to set engine secret mode sha");
				net__print_ssl_error(रिक्त, रिक्त);
				प्रत्यागम MOSQ_ERR_TLS;
			}
			जइ(!ENGINE_ctrl_cmd(engine, ENGINE_PIN, 0, listener->tls_engine_kpass_sha1, रिक्त, 0)){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to set engine pin");
				net__print_ssl_error(रिक्त, रिक्त);
				प्रत्यागम MOSQ_ERR_TLS;
			}
			ui_method = रिक्त;
		}
		pkey = ENGINE_load_private_कुञ्चिका(engine, listener->keyfile, ui_method, रिक्त);
		जइ(!pkey){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(रिक्त, रिक्त);
			प्रत्यागम MOSQ_ERR_TLS;
		}
		जइ(SSL_CTX_use_Private_कुञ्चिका(listener->ssl_ctx, pkey) <= 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to use engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(रिक्त, रिक्त);
			प्रत्यागम MOSQ_ERR_TLS;
		}
	}
	ENGINE_free(engine); /* release the structural reference from ENGINE_by_id() */

	प्रत्यागम MOSQ_ERR_SUCCESS;
}
#endif


पूर्णाङ्क net__tls_load_verify(संरचना mosquitto__listener *listener)
{
#ifdef WITH_TLS
	पूर्णाङ्क rc;

#  जइ OPENSSL_VERSION_सङ्ख्या < 0x30000000L
	जइ(listener->cafile || listener->capath){
		rc = SSL_CTX_load_verify_locations(listener->ssl_ctx, listener->cafile, listener->capath);
		जइ(rc == 0){
			जइ(listener->cafile && listener->capath){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\" and capath \"%s\".", listener->cafile, listener->capath);
			}अण्णहा जइ(listener->cafile){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			}अण्णहा{
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			}
		}
	}
#  अण्णहा
	राशिः_OF(X509_NAME) *ca_names = रिक्त;
	जइ(listener->cafile || listener->capath){
		ca_names = sk_X509_NAME_new_null();
	}

	जइ(listener->cafile){
		rc = SSL_CTX_load_verify_file(listener->ssl_ctx, listener->cafile);
		जइ(rc == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			net__print_ssl_error(रिक्त, रिक्त);
			प्रत्यागम MOSQ_ERR_TLS;
		}

		राशिः_OF(X509_NAME) *cert_names = SSL_load_client_CA_file(listener->cafile);
		जइ(cert_names){
			प्रति(पूर्णाङ्क i=0; i<sk_X509_NAME_अङ्कः(cert_names); i++){
				sk_X509_NAME_प्रक्षिप(ca_names, X509_NAME_dup(sk_X509_NAME_मानम्(cert_names, i)));
			}
			sk_X509_NAME_निष्कासय_free(cert_names, X509_NAME_free);
		}

	}
	जइ(listener->capath){
		rc = SSL_CTX_load_verify_dir(listener->ssl_ctx, listener->capath);
		जइ(rc == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			net__print_ssl_error(रिक्त, रिक्त);
			sk_X509_NAME_निष्कासय_free(ca_names, X509_NAME_free);
			प्रत्यागम MOSQ_ERR_TLS;
		}

		SSL_add_dir_cert_subjects_to_राशिः(ca_names, listener->capath);
	}
	जइ(ca_names){
		SSL_CTX_set_client_CA_list(listener->ssl_ctx, ca_names);
	}
#  endif

#  जइ !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000
	जइ(net__load_engine(listener)){
		प्रत्यागम MOSQ_ERR_TLS;
	}
#  endif
#endif
	प्रत्यागम net__load_certificates(listener);
}


#जइ !defined(WIN32) && !defined(_AIX)


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

	जइ(getifaddrs(&ifaddr) < 0){
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		प्रत्यागम MOSQ_ERR_ERRNO;
	}

	प्रति(संरचना ifaddrs *ifa=ifaddr; ifa!=रिक्त; ifa=ifa->ifa_next){
		जइ(ifa->ifa_addr == रिक्त){
			अनुवर्त;
		}

		जइ(!strcasecmp(listener->bind_interface, ifa->ifa_name)){
			have_interface = true;

			जइ(ifa->ifa_addr->sa_family == rp->ai_addr->sa_family){
				जइ(rp->ai_addr->sa_family == AF_INET){
					जइ(listener->host &&
							memcmp(&((संरचना sockaddr_in *)rp->ai_addr)->sin_addr,
							&((संरचना sockaddr_in *)ifa->ifa_addr)->sin_addr,
							परिमाण(संरचना in_addr))){

						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						प्रत्यागम MOSQ_ERR_INVAL;
					}अण्णहा{
						memcpy(&((संरचना sockaddr_in *)rp->ai_addr)->sin_addr,
								&((संरचना sockaddr_in *)ifa->ifa_addr)->sin_addr,
								परिमाण(संरचना in_addr));

						freeifaddrs(ifaddr);
						प्रत्यागम MOSQ_ERR_SUCCESS;
					}
				}अण्णहा जइ(rp->ai_addr->sa_family == AF_INET6){
					जइ(listener->host &&
							memcmp(&((संरचना sockaddr_in6 *)rp->ai_addr)->sin6_addr,
							&((संरचना sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
							परिमाण(संरचना in6_addr))){

						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						प्रत्यागम MOSQ_ERR_INVAL;
					}अण्णहा{
						memcpy(&((संरचना sockaddr_in6 *)rp->ai_addr)->sin6_addr,
								&((संरचना sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
								परिमाण(संरचना in6_addr));

						((संरचना sockaddr_in6 *)rp->ai_addr)->sin6_scope_id = ((संरचना sockaddr_in6 *)ifa->ifa_addr)->sin6_scope_id;

						freeifaddrs(ifaddr);
						प्रत्यागम MOSQ_ERR_SUCCESS;
					}
				}
			}
		}
	}
	freeifaddrs(ifaddr);
	जइ(have_interface){
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Interface %s does not support %s configuration.",
				listener->bind_interface, rp->ai_addr->sa_family == AF_INET ? "IPv4" : "IPv6");
		प्रत्यागम MOSQ_ERR_NOT_SUPPORTED;
	}अण्णहा{
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Interface %s does not exist.", listener->bind_interface);
		प्रत्यागम MOSQ_ERR_NOT_FOUND;
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

	जइ(!listener){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	snprintf(service, 10, "%d", listener->port);
	memset(&hints, 0, परिमाण(संरचना addrinfo));
	जइ(listener->socket_domain){
		hints.ai_family = listener->socket_domain;
	}अण्णहा{
		hints.ai_family = AF_UNSPEC;
	}
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	rc = getaddrinfo(listener->host, service, &hints, &ainfo);
	जइ(rc){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating listener: %s.", gai_strerror(rc));
		प्रत्यागम INVALID_SOCKET;
	}

	listener->sock_गणनम् = 0;
	listener->socks = रिक्त;

	प्रति(rp = ainfo; rp; rp = rp->ai_next){
		जइ(rp->ai_family == AF_INET){
			log__printf(रिक्त, MOSQ_LOG_INFO, "Opening ipv4 listen socket on port %d.", ntohs(((संरचना sockaddr_in *)rp->ai_addr)->sin_port));
		}अण्णहा जइ(rp->ai_family == AF_INET6){
			log__printf(रिक्त, MOSQ_LOG_INFO, "Opening ipv6 listen socket on port %d.", ntohs(((संरचना sockaddr_in6 *)rp->ai_addr)->sin6_port));
		}अण्णहा{
			अनुवर्त;
		}

		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		जइ(sock == INVALID_SOCKET){
			net__print_error(MOSQ_LOG_WARNING, "Warning: %s");
			अनुवर्त;
		}
		listener->sock_गणनम्++;
		listener->socks = mosquitto_realloc(listener->socks, परिमाण(mosq_sock_t)*(size_t)listener->sock_गणनम्);
		जइ(!listener->socks){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			प्रत्यागम MOSQ_ERR_NOMEM;
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

		जइ(net__socket_nonblock(&sock)){
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			प्रत्यागम 1;
		}

#जइ !defined(WIN32) && !defined(_AIX)
		जइ(listener->bind_interface){
			/* It might be possible that an interface does not support all relevant sa_families.
			 * We should successfully find at least one. */
			rc = net__bind_interface(listener, rp);
			जइ(rc){
				COMPAT_CLOSE(sock);
				listener->sock_गणनम्--;
				जइ(rc == MOSQ_ERR_NOT_FOUND || rc == MOSQ_ERR_INVAL){
					freeaddrinfo(ainfo);
					प्रत्यागम rc;
				}अण्णहा{
					अनुवर्त;
				}
			}
			interface_bound = true;
		}
#endif

		जइ(bind(sock, rp->ai_addr, rp->ai_addrlen) == -1){
#जइ defined(__linux__)
			जइ(errno == EACCES){
				log__printf(रिक्त, MOSQ_LOG_ERR, "If you are trying to bind to a privileged port (<1024), try using setcap and do not start the broker as root:");
				log__printf(रिक्त, MOSQ_LOG_ERR, "    sudo setcap 'CAP_NET_BIND_SERVICE=+ep /usr/sbin/mosquitto'");
			}
#endif
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			COMPAT_CLOSE(sock);
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			प्रत्यागम 1;
		}

		जइ(listen(sock, 100) == -1){
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			mosquitto_FREE(listener->socks);
			प्रत्यागम 1;
		}
	}
	freeaddrinfo(ainfo);

#ifndef WIN32
	जइ(listener->bind_interface && !interface_bound){
		mosquitto_FREE(listener->socks);
		प्रत्यागम 1;
	}
#endif

	प्रत्यागम 0;
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

	जइ(listener->unix_socket_path == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	जइ(strlen(listener->unix_socket_path) > परिमाण(addr.sun_path)-1){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Path to unix socket is too long \"%s\".", listener->unix_socket_path);
		प्रत्यागम MOSQ_ERR_INVAL;
	}

#ifdef WIN32
	DeleteFile(listener->unix_socket_path);
#अण्णहा
	unlink(listener->unix_socket_path);
#endif
	log__printf(रिक्त, MOSQ_LOG_INFO, "Opening unix listen socket on path %s.", listener->unix_socket_path);
	memset(&addr, 0, परिमाण(संरचना sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, listener->unix_socket_path, परिमाण(addr.sun_path)-1);

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	जइ(sock == INVALID_SOCKET){
		net__print_error(MOSQ_LOG_ERR, "Error creating unix socket: %s");
		प्रत्यागम 1;
	}
	listener->sock_गणनम्++;
	listener->socks = mosquitto_realloc(listener->socks, परिमाण(mosq_sock_t)*(size_t)listener->sock_गणनम्);
	जइ(!listener->socks){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		COMPAT_CLOSE(sock);
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	listener->socks[listener->sock_गणनम्-1] = sock;


#ifndef WIN32
	old_mask = umask(0007);
#endif
	rc = bind(sock, (संरचना sockaddr *)&addr, परिमाण(संरचना sockaddr_un));
#ifndef WIN32
	umask(old_mask);
#endif

	जइ(rc == -1){
		net__print_error(MOSQ_LOG_ERR, "Error binding unix socket: %s");
		प्रत्यागम 1;
	}

	जइ(listen(sock, 10) == -1){
		net__print_error(MOSQ_LOG_ERR, "Error listening to unix socket: %s");
		प्रत्यागम 1;
	}

	जइ(net__socket_nonblock(&sock)){
		प्रत्यागम 1;
	}

	प्रत्यागम 0;
}
#endif


/* Creates a socket and listens on port 'port'.
 * Returns 1 on failure
 * Returns 0 on success.
 */
पूर्णाङ्क net__socket_listen(संरचना mosquitto__listener *listener)
{
	पूर्णाङ्क rc;

	जइ(!listener){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

#ifdef WITH_UNIX_SOCKETS
	जइ(listener->port == 0 && listener->unix_socket_path != रिक्त){
		rc = net__socket_listen_unix(listener);
	}अण्णहा
#endif
	{
		rc = net__socket_listen_tcp(listener);
	}
	जइ(rc){
		प्रत्यागम rc;
	}

	/* We need to have at least one working socket. */
	जइ(listener->sock_गणनम् > 0){
#ifdef WITH_TLS
		जइ(listener->certfile && listener->keyfile){
			जइ(net__tls_server_ctx(listener)){
				प्रत्यागम 1;
			}

			जइ(net__tls_load_verify(listener)){
				प्रत्यागम 1;
			}
		}
#  ifdef FINAL_WITH_TLS_PSK
		जइ(listener->psk_hint){
			जइ(listener->certfile == रिक्त || listener->keyfile == रिक्त){
				जइ(net__tls_server_ctx(listener)){
					प्रत्यागम 1;
				}
			}
			SSL_CTX_set_psk_server_callback(listener->ssl_ctx, psk_server_callback);
			जइ(listener->psk_hint){
				rc = SSL_CTX_use_psk_identity_hint(listener->ssl_ctx, listener->psk_hint);
				जइ(rc == 0){
					log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to set TLS PSK hint.");
					net__print_ssl_error(रिक्त, रिक्त);
					प्रत्यागम 1;
				}
			}
		}
#  endif /* FINAL_WITH_TLS_PSK */
		जइ(tls_ex_index_context == -1){
			tls_ex_index_context = SSL_get_ex_new_index(0, "client context", रिक्त, रिक्त, रिक्त);
		}
		जइ(tls_ex_index_listener == -1){
			tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", रिक्त, रिक्त, रिक्त);
		}
#endif /* WITH_TLS */
		प्रत्यागम 0;
	}अण्णहा{
		प्रत्यागम 1;
	}
}


पूर्णाङ्क net__socket_get_address(mosq_sock_t sock, वर्ण *buf, size_t len, uint16_t *remote_port)
{
	संरचना sockaddr_storage addr;
	socklen_t addrlen;

	memset(&addr, 0, परिमाण(संरचना sockaddr_storage));
	addrlen = परिमाण(addr);
	जइ(!getpeername(sock, (संरचना sockaddr *)&addr, &addrlen)){
		जइ(addr.ss_family == AF_INET){
			जइ(remote_port){
				*remote_port = ntohs(((संरचना sockaddr_in *)&addr)->sin_port);
			}
			जइ(inet_ntop(AF_INET, &((संरचना sockaddr_in *)&addr)->sin_addr.s_addr, buf, (socklen_t)len)){
				प्रत्यागम 0;
			}
		}अण्णहा जइ(addr.ss_family == AF_INET6){
			जइ(remote_port){
				*remote_port = ntohs(((संरचना sockaddr_in6 *)&addr)->sin6_port);
			}
			जइ(inet_ntop(AF_INET6, &((संरचना sockaddr_in6 *)&addr)->sin6_addr.s6_addr, buf, (socklen_t)len)){
				प्रत्यागम 0;
			}
#ifdef WITH_UNIX_SOCKETS
		}अण्णहा जइ(addr.ss_family == AF_UNIX){
			संरचना sockaddr_un un;
			addrlen = परिमाण(संरचना sockaddr_un);
			जइ(!getsockname(sock, (संरचना sockaddr *)&un, &addrlen)){
				snprintf(buf, len, "%s", un.sun_path);
			}अण्णहा{
				snprintf(buf, len, "unix-socket");
			}
			प्रत्यागम 0;
#endif
		}
	}
	प्रत्यागम 1;
}
