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

#அந்தர்பாவ "config.h"

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
#இல்லையெனில்
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#அந்தர்பாவ <errno.h>
#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>
#ifdef WITH_WRAP
#  include <tcpd.h>
#endif

#ifdef HAVE_NETINET_IN_H
#  include <netinet/in.h>
#endif

#எனில் defined(WITH_UNIX_SOCKETS) || defined(WITH_TLS)
#  include "sys/stat.h"
#endif

#ifdef WITH_UNIX_SOCKETS
#  ifdef WIN32
#    include <afunix.h>
#  இல்லையெனில்
#    include <sys/un.h>
#  endif
#endif

#ifdef __QNX__
#  include <net/netbyte.h>
#endif

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto/mqtt_protocol.h"
#அந்தர்பாவ "net_mosq.h"
#அந்தர்பாவ "util_mosq.h"

#ifdef WITH_TLS
#  include "tls_mosq.h"
#  include <openssl/err.h>
நிலையான முழு tls_ex_index_context = -1;
நிலையான முழு tls_ex_index_listener = -1;
#endif

#அந்தர்பாவ "sys_tree.h"

/* For EMFILE handling */
நிலையான mosq_sock_t spare_sock = INVALID_SOCKET;


வெறுமை net__broker_init(வெறுமை)
{
	spare_sock = socket(AF_INET, SOCK_STREAM, 0);
	net__init();
#ifdef WITH_TLS
	net__init_tls();
#endif
}


வெறுமை net__broker_cleanup(வெறுமை)
{
	எனில்(spare_sock != INVALID_SOCKET){
		COMPAT_CLOSE(spare_sock);
		spare_sock = INVALID_SOCKET;
	}
	net__cleanup();
}


நிலையான வெறுமை net__print_error(unsigned முழு log, மாறிலி எழுத்து *format_str)
{
	எழுத்து *buf;

#ifdef WIN32
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			வெற்று, WSAGetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, வெற்று);

	log__printf(வெற்று, log, format_str, buf);
	LocalFree(buf);
#இல்லையெனில்
	buf = strerror(errno);
	log__printf(வெற்று, log, format_str, buf);
#endif
}


கட்டமைப்பு mosquitto *net__socket_accept(கட்டமைப்பு mosquitto__listener_sock *listensock)
{
	mosq_sock_t new_sock = INVALID_SOCKET;
	கட்டமைப்பு mosquitto *new_context;
#ifdef WITH_TLS
	BIO *bio;
#endif
#ifdef WITH_WRAP
	கட்டமைப்பு request_info wrap_req;
	எழுத்து address[1024];
#endif

	new_sock = accept(listensock->sock, வெற்று, 0);
	எனில்(new_sock == INVALID_SOCKET){
#ifdef WIN32
		WINDOWS_SET_ERRNO();
		எனில்(errno == WSAEMFILE){
#இல்லையெனில்
		எனில்(errno == EMFILE || errno == ENFILE){
#endif
			/* Close the spare socket, which means we should be able to accept
			 * this connection. Accept it, then close it immediately and create
			 * a new spare_sock. This prevents the situation of ever properly
			 * running out of sockets.
			 * It would be nice to send a "server not available" connack here,
			 * but there are lots of reasons why this would be tricky (TLS
			 * being the big one). */
			COMPAT_CLOSE(spare_sock);
			new_sock = accept(listensock->sock, வெற்று, 0);
			எனில்(new_sock != INVALID_SOCKET){
				COMPAT_CLOSE(new_sock);
			}
			spare_sock = socket(AF_INET, SOCK_STREAM, 0);
			log__printf(வெற்று, MOSQ_LOG_WARNING,
					"Unable to accept new connection, system socket count has been exceeded. Try increasing \"ulimit -n\" or equivalent.");
		}
		திரும்பு வெற்று;
	}

	metrics__int_inc(mosq_counter_socket_connections, 1);

	எனில்(net__socket_nonblock(&new_sock)){
		திரும்பு வெற்று;
	}

#ifdef WITH_WRAP
	/* Use tcpd / libwrap to determine whether a connection is allowed. */
	request_init(&wrap_req, RQ_FILE, new_sock, RQ_DAEMON, "mosquitto", 0);
	fromhost(&wrap_req);
	எனில்(!hosts_access(&wrap_req)){
		/* Access is denied */
		எனில்(db.config->connection_messages == true){
			எனில்(!net__socket_get_address(new_sock, address, 1024, வெற்று)){
				log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client connection from %s denied access by tcpd.", address);
			}
		}
		COMPAT_CLOSE(new_sock);
		திரும்பு வெற்று;
	}
#endif

	எனில்(db.config->set_tcp_nodelay && listensock->listener->port){
		முழு flag = 1;
		எனில்(setsockopt(new_sock, IPPROTO_TCP, TCP_NODELAY, (மாறிலி வெறுமை *)&flag, அளவு(முழு)) != 0){
			log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Unable to set TCP_NODELAY.");
		}
	}

	new_context = context__init();
	எனில்(!new_context){
		COMPAT_CLOSE(new_sock);
		திரும்பு வெற்று;
	}
	new_context->listener = listensock->listener;
	எனில்(!new_context->listener){
		context__cleanup(new_context, true);
		திரும்பு வெற்று;
	}
	new_context->listener->client_மொத்தம்++;

	எனில்(new_context->listener->enable_proxy_protocol){
		எனில்(context__init_sock(new_context, new_sock, false) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			திரும்பு வெற்று;
		}
		எனில்(new_context->listener->enable_proxy_protocol == 2){
			new_context->transport = mosq_t_proxy_v2;
		}இல்லையெனில்{
			new_context->transport = mosq_t_proxy_v1;
		}
		new_context->proxy.cmd = -1;
	}இல்லையெனில்{
		எனில்(context__init_sock(new_context, new_sock, true) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			திரும்பு வெற்று;
		}
		தேர்வு(new_context->listener->protocol){
			நிலை mp_mqtt:
				new_context->transport = mosq_t_tcp;
				நிறுத்து;
#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			நிலை mp_websockets:
				எனில்(http__context_init(new_context)){
					context__cleanup(new_context, true);
					திரும்பு வெற்று;
				}
				நிறுத்து;
#endif
			இயல்பு:
				context__cleanup(new_context, true);
				திரும்பு வெற்று;
		}
	}

	எனில்((new_context->listener->max_connections > 0 && new_context->listener->client_மொத்தம் > new_context->listener->max_connections)
			|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (unsigned முழு)db.config->global_max_connections)){

		எனில்(db.config->connection_messages == true){
			log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", new_context->address);
		}
		context__cleanup(new_context, true);
		திரும்பு வெற்று;
	}

#ifdef WITH_TLS
	/* TLS init */
	எனில்(new_context->listener->ssl_ctx){
		new_context->ssl = SSL_new(new_context->listener->ssl_ctx);
		எனில்(!new_context->ssl){
			context__cleanup(new_context, true);
			திரும்பு வெற்று;
		}
		எனில்(!SSL_set_ex_data(new_context->ssl, tls_ex_index_context, new_context)
				|| !SSL_set_ex_data(new_context->ssl, tls_ex_index_listener, new_context->listener)){
			context__cleanup(new_context, true);
			திரும்பு வெற்று;
		}
		new_context->want_write = true;
		bio = BIO_new_socket(new_sock, BIO_NOCLOSE);
		SSL_set_bio(new_context->ssl, bio, bio);
		ERR_clear_error();
		SSL_set_accept_state(new_context->ssl);
	}
#endif

	எனில்(db.config->connection_messages == true
			&& !new_context->listener->enable_proxy_protocol){

		log__printf(வெற்று, MOSQ_LOG_NOTICE, "New connection from %s:%d on port %d.",
				new_context->address, new_context->remote_port, new_context->listener->port);
	}

	mux__new(new_context);
	keepalive__add(new_context);

	திரும்பு new_context;
}

#ifdef WITH_TLS


நிலையான முழு client_certificate_verify(முழு preverify_ok, X509_STORE_CTX *ctx)
{
	/* Preverify should check expiry, revocation. */
	எனில்(preverify_ok == 0){
		SSL *ssl;
		ssl = X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx());
		எனில்(ssl){
			கட்டமைப்பு mosquitto__listener *listener;
			listener = SSL_get_ex_data(ssl, tls_ex_index_listener);

			எனில்(listener && listener->disable_client_cert_date_checks){
				முழு err = X509_STORE_CTX_get_error(ctx);
				எனில்(err == X509_V_ERR_CERT_NOT_YET_VALID || err == X509_V_ERR_CERT_HAS_EXPIRED){
					preverify_ok = 1;
				}
			}
		}
	}
	திரும்பு preverify_ok;
}
#endif

#ifdef FINAL_WITH_TLS_PSK


நிலையான unsigned முழு psk_server_callback(SSL *ssl, மாறிலி எழுத்து *identity, unsigned எழுத்து *psk, unsigned முழு max_psk_len)
{
	கட்டமைப்பு mosquitto *context;
	கட்டமைப்பு mosquitto__listener *listener;
	எழுத்து *psk_சாவி = வெற்று;
	முழு len;
	மாறிலி எழுத்து *psk_hint;

	எனில்(!identity){
		திரும்பு 0;
	}

	context = SSL_get_ex_data(ssl, tls_ex_index_context);
	எனில்(!context){
		திரும்பு 0;
	}

	listener = SSL_get_ex_data(ssl, tls_ex_index_listener);
	எனில்(!listener){
		திரும்பு 0;
	}

	psk_hint = listener->psk_hint;

	/* The hex to BN conversion results in the length halving, so we can pass
	 * max_psk_len*2 as the max hex key here. */
	psk_சாவி = mosquitto_calloc(1, (size_t)max_psk_len*2 + 1);
	எனில்(!psk_சாவி){
		திரும்பு 0;
	}

	எனில்(mosquitto_psk_சாவி_get(context, psk_hint, identity, psk_சாவி, (முழு)max_psk_len*2) != MOSQ_ERR_SUCCESS){
		mosquitto_FREE(psk_சாவி);
		திரும்பு 0;
	}

	len = mosquitto__hex2bin(psk_சாவி, psk, (முழு)max_psk_len);
	எனில்(len < 0){
		mosquitto_FREE(psk_சாவி);
		திரும்பு 0;
	}

	எனில்(listener->use_identity_as_username){
		எனில்(mosquitto_validate_utf8(identity, (முழு)strlen(identity))){
			mosquitto_free(psk_சாவி);
			திரும்பு 0;
		}
		context->username = mosquitto_strdup(identity);
		எனில்(!context->username){
			mosquitto_FREE(psk_சாவி);
			திரும்பு 0;
		}
	}

	mosquitto_FREE(psk_சாவி);
	திரும்பு (unsigned முழு)len;
}
#endif

#ifdef WITH_TLS


நிலையான வெறுமை tls_keylog_callback(மாறிலி SSL *ssl, மாறிலி எழுத்து *வரி)
{
	UNUSED(ssl);

	எனில்(db.tls_keylog){
		FILE *fptr;
		fptr = mosquitto_fopen(db.tls_keylog, "at", true);
		எனில்(fptr){
#ifndef WIN32
			/* Until mosquitto_fopen enforces file permissions on all files,
			 * enforce it here. We can enforce it here, because it isn't a
			 * change of behaviour. */
			கட்டமைப்பு stat statbuf;
			எனில்(fstat(fileno(fptr), &statbuf) < 0
					|| statbuf.st_mode & (S_IRWXG | S_IRWXO)){

				fclose(fptr);
				திரும்பு;
			}
#endif
			fprintf(fptr, "%s\n", வரி);
			fclose(fptr);
		}இல்லையெனில்{
#ifndef WIN32
			எனில்(errno == ELOOP){
				log__printf(வெற்று, MOSQ_LOG_INFO, "Error: keylog file must not be a symbolic link");
			}இல்லையெனில்
#endif
			{
				log__printf(வெற்று, MOSQ_LOG_INFO, "Error: Unable to open keylog file: %s", strerror(errno));
			}
		}
	}
}


முழு net__tls_server_ctx(கட்டமைப்பு mosquitto__listener *listener)
{
	எழுத்து buf[256];
	முழு rc;

	எனில்(tls_ex_index_context == -1){
		tls_ex_index_context = SSL_get_ex_new_index(0, "client context", வெற்று, வெற்று, வெற்று);
	}
	எனில்(tls_ex_index_listener == -1){
		tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", வெற்று, வெற்று, வெற்று);
	}

	எனில்(listener->ssl_ctx){
		SSL_CTX_free(listener->ssl_ctx);
		listener->ssl_ctx = வெற்று;
	}

	listener->ssl_ctx = SSL_CTX_new(TLS_server_method());

	எனில்(!listener->ssl_ctx){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to create TLS context.");
		திரும்பு MOSQ_ERR_TLS;
	}

#ifdef SSL_OP_NO_TLSv1_3
	எனில்(db.config->per_listener_settings){
		எனில்(listener->security_options->psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}இல்லையெனில்{
		எனில்(db.config->security_options.psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}
#endif

	எனில்(listener->tls_version == வெற்று){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
#ifdef SSL_OP_NO_TLSv1_3
	}இல்லையெனில் எனில்(!strcmp(listener->tls_version, "tlsv1.3")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2);
#endif
	}இல்லையெனில் எனில்(!strcmp(listener->tls_version, "tlsv1.2")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
	}இல்லையெனில் எனில்(!strcmp(listener->tls_version, "tlsv1.1")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1);
		log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: TLS v1.1 is insecure. It must only be used in the case "
				"where you have devices that cannot be upgraded to use a secure version of TLS. It is "
				"recommended to use as secure a set of ciphers as possible and that will restrict it to "
				"TLS v1.1 only, and to use a separate listener using TLS v1.2 and secure ciphers for your "
				"other devices.");
		log__printf(வெற்று, MOSQ_LOG_WARNING, "Please be aware that support for TLS v1.1 will go away eventually "
				"and that you should plan now to migrate away from it.");
	}இல்லையெனில்{
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unsupported tls_version \"%s\".", listener->tls_version);
		திரும்பு MOSQ_ERR_TLS;
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

	எனில்(db.tls_keylog){
		log__printf(வெற்று, MOSQ_LOG_NOTICE, "TLS key logging to '%s' enabled for all listeners.",
				db.tls_keylog);
		log__printf(வெற்று, MOSQ_LOG_NOTICE, "TLS key logging is for DEBUGGING only.");

		SSL_CTX_set_keylog_callback(listener->ssl_ctx, tls_keylog_callback);
	}

	snprintf(buf, 256, "mosquitto-%d", listener->port);
	SSL_CTX_set_session_id_context(listener->ssl_ctx, (unsigned எழுத்து *)buf, (unsigned முழு)strlen(buf));

	எனில்(listener->ciphers){
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, listener->ciphers);
		எனில்(rc == 0){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			திரும்பு MOSQ_ERR_TLS;
		}
	}இல்லையெனில்{
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, "DEFAULT:!aNULL:!eNULL:!LOW:!EXPORT:!SSLv2:@STRENGTH");
		எனில்(rc == 0){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			திரும்பு MOSQ_ERR_TLS;
		}
	}
	எனில்(listener->ciphers_tls13){
		rc = SSL_CTX_set_ciphersuites(listener->ssl_ctx, listener->ciphers_tls13);
		எனில்(rc == 0){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to set TLS 1.3 ciphersuites. Check cipher_tls13 list \"%s\".", listener->ciphers_tls13);
			திரும்பு MOSQ_ERR_TLS;
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}
#endif


#ifdef WITH_TLS


நிலையான முழு net__load_crl_file(கட்டமைப்பு mosquitto__listener *listener)
{
	X509_STORE *store;
	X509_LOOKUP *lookup;
	முழு rc;

	store = SSL_CTX_get_cert_store(listener->ssl_ctx);
	எனில்(!store){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to obtain TLS store.");
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		திரும்பு MOSQ_ERR_TLS;
	}
	lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
	rc = X509_load_crl_file(lookup, listener->crlfile, X509_FILETYPE_PEM);
	எனில்(rc < 1){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to load certificate revocation file \"%s\". Check crlfile.", listener->crlfile);
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		net__print_ssl_error(வெற்று, வெற்று);
		திரும்பு MOSQ_ERR_TLS;
	}
	X509_STORE_set_flags(store, X509_V_FLAG_CRL_CHECK);

	திரும்பு MOSQ_ERR_SUCCESS;
}
#endif


முழு net__load_certificates(கட்டமைப்பு mosquitto__listener *listener)
{
#ifdef WITH_TLS
	முழு rc;

	எனில்(listener->require_certificate){
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, client_certificate_verify);
	}இல்லையெனில்{
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_NONE, client_certificate_verify);
	}
	rc = SSL_CTX_use_certificate_chain_file(listener->ssl_ctx, listener->certfile);
	எனில்(rc != 1){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
		net__print_ssl_error(வெற்று, வெற்று);
		திரும்பு MOSQ_ERR_TLS;
	}
	எனில்(listener->tls_engine == வெற்று || listener->tls_keyform == mosq_k_pem){
		rc = SSL_CTX_use_Private_சாவி_file(listener->ssl_ctx, listener->keyfile, SSL_FILETYPE_PEM);
		எனில்(rc != 1){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			net__print_ssl_error(வெற்று, வெற்று);
			திரும்பு MOSQ_ERR_TLS;
		}
	}
	rc = SSL_CTX_check_private_சாவி(listener->ssl_ctx);
	எனில்(rc != 1){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Server certificate/key are inconsistent.");
		net__print_ssl_error(வெற்று, வெற்று);
		திரும்பு MOSQ_ERR_TLS;
	}
	/* Load CRLs if they exist. */
	எனில்(listener->crlfile){
		rc = net__load_crl_file(listener);
		எனில்(rc){
			திரும்பு rc;
		}
	}
#இல்லையெனில்
	UNUSED(listener);
#endif
	திரும்பு MOSQ_ERR_SUCCESS;
}


#எனில் defined(WITH_TLS) && !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000


நிலையான முழு net__load_engine(கட்டமைப்பு mosquitto__listener *listener)
{
	ENGINE *engine = வெற்று;
	UI_METHOD *ui_method;
	EVP_PKEY *pkey;

	எனில்(!listener->tls_engine){
		திரும்பு MOSQ_ERR_SUCCESS;
	}

	engine = ENGINE_by_id(listener->tls_engine);
	எனில்(!engine){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error loading %s engine\n", listener->tls_engine);
		net__print_ssl_error(வெற்று, வெற்று);
		திரும்பு MOSQ_ERR_TLS;
	}
	எனில்(!ENGINE_init(engine)){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Failed engine initialisation\n");
		net__print_ssl_error(வெற்று, வெற்று);
		திரும்பு MOSQ_ERR_TLS;
	}
	ENGINE_set_default(engine, ENGINE_METHOD_ALL);

	எனில்(listener->tls_keyform == mosq_k_engine){
		ui_method = net__get_ui_method();
		எனில்(listener->tls_engine_kpass_sha1){
			எனில்(!ENGINE_ctrl_cmd(engine, ENGINE_SECRET_MODE, ENGINE_SECRET_MODE_SHA, வெற்று, வெற்று, 0)){
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to set engine secret mode sha");
				net__print_ssl_error(வெற்று, வெற்று);
				திரும்பு MOSQ_ERR_TLS;
			}
			எனில்(!ENGINE_ctrl_cmd(engine, ENGINE_PIN, 0, listener->tls_engine_kpass_sha1, வெற்று, 0)){
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to set engine pin");
				net__print_ssl_error(வெற்று, வெற்று);
				திரும்பு MOSQ_ERR_TLS;
			}
			ui_method = வெற்று;
		}
		pkey = ENGINE_load_private_சாவி(engine, listener->keyfile, ui_method, வெற்று);
		எனில்(!pkey){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to load engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(வெற்று, வெற்று);
			திரும்பு MOSQ_ERR_TLS;
		}
		எனில்(SSL_CTX_use_Private_சாவி(listener->ssl_ctx, pkey) <= 0){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to use engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(வெற்று, வெற்று);
			திரும்பு MOSQ_ERR_TLS;
		}
	}
	ENGINE_free(engine); /* release the structural reference from ENGINE_by_id() */

	திரும்பு MOSQ_ERR_SUCCESS;
}
#endif


முழு net__tls_load_verify(கட்டமைப்பு mosquitto__listener *listener)
{
#ifdef WITH_TLS
	முழு rc;

#  எனில் OPENSSL_VERSION_எண் < 0x30000000L
	எனில்(listener->cafile || listener->capath){
		rc = SSL_CTX_load_verify_locations(listener->ssl_ctx, listener->cafile, listener->capath);
		எனில்(rc == 0){
			எனில்(listener->cafile && listener->capath){
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\" and capath \"%s\".", listener->cafile, listener->capath);
			}இல்லையெனில் எனில்(listener->cafile){
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			}இல்லையெனில்{
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			}
		}
	}
#  இல்லையெனில்
	அடுக்கு_OF(X509_NAME) *ca_names = வெற்று;
	எனில்(listener->cafile || listener->capath){
		ca_names = sk_X509_NAME_new_null();
	}

	எனில்(listener->cafile){
		rc = SSL_CTX_load_verify_file(listener->ssl_ctx, listener->cafile);
		எனில்(rc == 0){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			net__print_ssl_error(வெற்று, வெற்று);
			திரும்பு MOSQ_ERR_TLS;
		}

		அடுக்கு_OF(X509_NAME) *cert_names = SSL_load_client_CA_file(listener->cafile);
		எனில்(cert_names){
			க்கு(முழு i=0; i<sk_X509_NAME_இலக்கம்(cert_names); i++){
				sk_X509_NAME_தள்ளு(ca_names, X509_NAME_dup(sk_X509_NAME_மதிப்பு(cert_names, i)));
			}
			sk_X509_NAME_எடு_free(cert_names, X509_NAME_free);
		}

	}
	எனில்(listener->capath){
		rc = SSL_CTX_load_verify_dir(listener->ssl_ctx, listener->capath);
		எனில்(rc == 0){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			net__print_ssl_error(வெற்று, வெற்று);
			sk_X509_NAME_எடு_free(ca_names, X509_NAME_free);
			திரும்பு MOSQ_ERR_TLS;
		}

		SSL_add_dir_cert_subjects_to_அடுக்கு(ca_names, listener->capath);
	}
	எனில்(ca_names){
		SSL_CTX_set_client_CA_list(listener->ssl_ctx, ca_names);
	}
#  endif

#  எனில் !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000
	எனில்(net__load_engine(listener)){
		திரும்பு MOSQ_ERR_TLS;
	}
#  endif
#endif
	திரும்பு net__load_certificates(listener);
}


#எனில் !defined(WIN32) && !defined(_AIX)


நிலையான முழு net__bind_interface(கட்டமைப்பு mosquitto__listener *listener, கட்டமைப்பு addrinfo *rp)
{
	/*
	 * This binds the listener sock to a network interface.
	 * The use of SO_BINDTODEVICE requires root access, which we don't have, so instead
	 * use getifaddrs to find the interface addresses, and use IP of the
	 * matching interface in the later bind().
	 */
	கட்டமைப்பு ifaddrs *ifaddr;
	bool have_interface = false;

	எனில்(getifaddrs(&ifaddr) < 0){
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		திரும்பு MOSQ_ERR_ERRNO;
	}

	க்கு(கட்டமைப்பு ifaddrs *ifa=ifaddr; ifa!=வெற்று; ifa=ifa->ifa_next){
		எனில்(ifa->ifa_addr == வெற்று){
			தொடர்;
		}

		எனில்(!strcasecmp(listener->bind_interface, ifa->ifa_name)){
			have_interface = true;

			எனில்(ifa->ifa_addr->sa_family == rp->ai_addr->sa_family){
				எனில்(rp->ai_addr->sa_family == AF_INET){
					எனில்(listener->host &&
							memcmp(&((கட்டமைப்பு sockaddr_in *)rp->ai_addr)->sin_addr,
							&((கட்டமைப்பு sockaddr_in *)ifa->ifa_addr)->sin_addr,
							அளவு(கட்டமைப்பு in_addr))){

						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						திரும்பு MOSQ_ERR_INVAL;
					}இல்லையெனில்{
						memcpy(&((கட்டமைப்பு sockaddr_in *)rp->ai_addr)->sin_addr,
								&((கட்டமைப்பு sockaddr_in *)ifa->ifa_addr)->sin_addr,
								அளவு(கட்டமைப்பு in_addr));

						freeifaddrs(ifaddr);
						திரும்பு MOSQ_ERR_SUCCESS;
					}
				}இல்லையெனில் எனில்(rp->ai_addr->sa_family == AF_INET6){
					எனில்(listener->host &&
							memcmp(&((கட்டமைப்பு sockaddr_in6 *)rp->ai_addr)->sin6_addr,
							&((கட்டமைப்பு sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
							அளவு(கட்டமைப்பு in6_addr))){

						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						திரும்பு MOSQ_ERR_INVAL;
					}இல்லையெனில்{
						memcpy(&((கட்டமைப்பு sockaddr_in6 *)rp->ai_addr)->sin6_addr,
								&((கட்டமைப்பு sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
								அளவு(கட்டமைப்பு in6_addr));

						((கட்டமைப்பு sockaddr_in6 *)rp->ai_addr)->sin6_scope_id = ((கட்டமைப்பு sockaddr_in6 *)ifa->ifa_addr)->sin6_scope_id;

						freeifaddrs(ifaddr);
						திரும்பு MOSQ_ERR_SUCCESS;
					}
				}
			}
		}
	}
	freeifaddrs(ifaddr);
	எனில்(have_interface){
		log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Interface %s does not support %s configuration.",
				listener->bind_interface, rp->ai_addr->sa_family == AF_INET ? "IPv4" : "IPv6");
		திரும்பு MOSQ_ERR_NOT_SUPPORTED;
	}இல்லையெனில்{
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Interface %s does not exist.", listener->bind_interface);
		திரும்பு MOSQ_ERR_NOT_FOUND;
	}
}
#endif


நிலையான முழு net__socket_listen_tcp(கட்டமைப்பு mosquitto__listener *listener)
{
	mosq_sock_t sock = INVALID_SOCKET;
	கட்டமைப்பு addrinfo hints;
	கட்டமைப்பு addrinfo *ainfo, *rp;
	எழுத்து service[10];
	முழு rc;
	முழு ss_opt = 1;
#ifndef WIN32
	bool interface_bound = false;
#endif

	எனில்(!listener){
		திரும்பு MOSQ_ERR_INVAL;
	}

	snprintf(service, 10, "%d", listener->port);
	memset(&hints, 0, அளவு(கட்டமைப்பு addrinfo));
	எனில்(listener->socket_domain){
		hints.ai_family = listener->socket_domain;
	}இல்லையெனில்{
		hints.ai_family = AF_UNSPEC;
	}
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	rc = getaddrinfo(listener->host, service, &hints, &ainfo);
	எனில்(rc){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error creating listener: %s.", gai_strerror(rc));
		திரும்பு INVALID_SOCKET;
	}

	listener->sock_மொத்தம் = 0;
	listener->socks = வெற்று;

	க்கு(rp = ainfo; rp; rp = rp->ai_next){
		எனில்(rp->ai_family == AF_INET){
			log__printf(வெற்று, MOSQ_LOG_INFO, "Opening ipv4 listen socket on port %d.", ntohs(((கட்டமைப்பு sockaddr_in *)rp->ai_addr)->sin_port));
		}இல்லையெனில் எனில்(rp->ai_family == AF_INET6){
			log__printf(வெற்று, MOSQ_LOG_INFO, "Opening ipv6 listen socket on port %d.", ntohs(((கட்டமைப்பு sockaddr_in6 *)rp->ai_addr)->sin6_port));
		}இல்லையெனில்{
			தொடர்;
		}

		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		எனில்(sock == INVALID_SOCKET){
			net__print_error(MOSQ_LOG_WARNING, "Warning: %s");
			தொடர்;
		}
		listener->sock_மொத்தம்++;
		listener->socks = mosquitto_realloc(listener->socks, அளவு(mosq_sock_t)*(size_t)listener->sock_மொத்தம்);
		எனில்(!listener->socks){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			திரும்பு MOSQ_ERR_NOMEM;
		}
		listener->socks[listener->sock_மொத்தம்-1] = sock;

#ifndef WIN32
		ss_opt = 1;
		/* Unimportant if this fails */
		(வெறுமை)setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (மாறிலி வெறுமை *)&ss_opt, அளவு(ss_opt));
#endif
#ifdef IPV6_V6ONLY
		ss_opt = 1;
		(வெறுமை)setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (மாறிலி வெறுமை *)&ss_opt, அளவு(ss_opt));
#endif

		எனில்(net__socket_nonblock(&sock)){
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			திரும்பு 1;
		}

#எனில் !defined(WIN32) && !defined(_AIX)
		எனில்(listener->bind_interface){
			/* It might be possible that an interface does not support all relevant sa_families.
			 * We should successfully find at least one. */
			rc = net__bind_interface(listener, rp);
			எனில்(rc){
				COMPAT_CLOSE(sock);
				listener->sock_மொத்தம்--;
				எனில்(rc == MOSQ_ERR_NOT_FOUND || rc == MOSQ_ERR_INVAL){
					freeaddrinfo(ainfo);
					திரும்பு rc;
				}இல்லையெனில்{
					தொடர்;
				}
			}
			interface_bound = true;
		}
#endif

		எனில்(bind(sock, rp->ai_addr, rp->ai_addrlen) == -1){
#எனில் defined(__linux__)
			எனில்(errno == EACCES){
				log__printf(வெற்று, MOSQ_LOG_ERR, "If you are trying to bind to a privileged port (<1024), try using setcap and do not start the broker as root:");
				log__printf(வெற்று, MOSQ_LOG_ERR, "    sudo setcap 'CAP_NET_BIND_SERVICE=+ep /usr/sbin/mosquitto'");
			}
#endif
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			COMPAT_CLOSE(sock);
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			திரும்பு 1;
		}

		எனில்(listen(sock, 100) == -1){
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			mosquitto_FREE(listener->socks);
			திரும்பு 1;
		}
	}
	freeaddrinfo(ainfo);

#ifndef WIN32
	எனில்(listener->bind_interface && !interface_bound){
		mosquitto_FREE(listener->socks);
		திரும்பு 1;
	}
#endif

	திரும்பு 0;
}


#ifdef WITH_UNIX_SOCKETS


நிலையான முழு net__socket_listen_unix(கட்டமைப்பு mosquitto__listener *listener)
{
	கட்டமைப்பு sockaddr_un addr;
	mosq_sock_t sock = INVALID_SOCKET;
	முழு rc;
#ifndef WIN32
	mode_t old_mask;
#endif

	எனில்(listener->unix_socket_path == வெற்று){
		திரும்பு MOSQ_ERR_INVAL;
	}
	எனில்(strlen(listener->unix_socket_path) > அளவு(addr.sun_path)-1){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Path to unix socket is too long \"%s\".", listener->unix_socket_path);
		திரும்பு MOSQ_ERR_INVAL;
	}

#ifdef WIN32
	DeleteFile(listener->unix_socket_path);
#இல்லையெனில்
	unlink(listener->unix_socket_path);
#endif
	log__printf(வெற்று, MOSQ_LOG_INFO, "Opening unix listen socket on path %s.", listener->unix_socket_path);
	memset(&addr, 0, அளவு(கட்டமைப்பு sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, listener->unix_socket_path, அளவு(addr.sun_path)-1);

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	எனில்(sock == INVALID_SOCKET){
		net__print_error(MOSQ_LOG_ERR, "Error creating unix socket: %s");
		திரும்பு 1;
	}
	listener->sock_மொத்தம்++;
	listener->socks = mosquitto_realloc(listener->socks, அளவு(mosq_sock_t)*(size_t)listener->sock_மொத்தம்);
	எனில்(!listener->socks){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
		COMPAT_CLOSE(sock);
		திரும்பு MOSQ_ERR_NOMEM;
	}
	listener->socks[listener->sock_மொத்தம்-1] = sock;


#ifndef WIN32
	old_mask = umask(0007);
#endif
	rc = bind(sock, (கட்டமைப்பு sockaddr *)&addr, அளவு(கட்டமைப்பு sockaddr_un));
#ifndef WIN32
	umask(old_mask);
#endif

	எனில்(rc == -1){
		net__print_error(MOSQ_LOG_ERR, "Error binding unix socket: %s");
		திரும்பு 1;
	}

	எனில்(listen(sock, 10) == -1){
		net__print_error(MOSQ_LOG_ERR, "Error listening to unix socket: %s");
		திரும்பு 1;
	}

	எனில்(net__socket_nonblock(&sock)){
		திரும்பு 1;
	}

	திரும்பு 0;
}
#endif


/* Creates a socket and listens on port 'port'.
 * Returns 1 on failure
 * Returns 0 on success.
 */
முழு net__socket_listen(கட்டமைப்பு mosquitto__listener *listener)
{
	முழு rc;

	எனில்(!listener){
		திரும்பு MOSQ_ERR_INVAL;
	}

#ifdef WITH_UNIX_SOCKETS
	எனில்(listener->port == 0 && listener->unix_socket_path != வெற்று){
		rc = net__socket_listen_unix(listener);
	}இல்லையெனில்
#endif
	{
		rc = net__socket_listen_tcp(listener);
	}
	எனில்(rc){
		திரும்பு rc;
	}

	/* We need to have at least one working socket. */
	எனில்(listener->sock_மொத்தம் > 0){
#ifdef WITH_TLS
		எனில்(listener->certfile && listener->keyfile){
			எனில்(net__tls_server_ctx(listener)){
				திரும்பு 1;
			}

			எனில்(net__tls_load_verify(listener)){
				திரும்பு 1;
			}
		}
#  ifdef FINAL_WITH_TLS_PSK
		எனில்(listener->psk_hint){
			எனில்(listener->certfile == வெற்று || listener->keyfile == வெற்று){
				எனில்(net__tls_server_ctx(listener)){
					திரும்பு 1;
				}
			}
			SSL_CTX_set_psk_server_callback(listener->ssl_ctx, psk_server_callback);
			எனில்(listener->psk_hint){
				rc = SSL_CTX_use_psk_identity_hint(listener->ssl_ctx, listener->psk_hint);
				எனில்(rc == 0){
					log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to set TLS PSK hint.");
					net__print_ssl_error(வெற்று, வெற்று);
					திரும்பு 1;
				}
			}
		}
#  endif /* FINAL_WITH_TLS_PSK */
		எனில்(tls_ex_index_context == -1){
			tls_ex_index_context = SSL_get_ex_new_index(0, "client context", வெற்று, வெற்று, வெற்று);
		}
		எனில்(tls_ex_index_listener == -1){
			tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", வெற்று, வெற்று, வெற்று);
		}
#endif /* WITH_TLS */
		திரும்பு 0;
	}இல்லையெனில்{
		திரும்பு 1;
	}
}


முழு net__socket_get_address(mosq_sock_t sock, எழுத்து *buf, size_t len, uint16_t *remote_port)
{
	கட்டமைப்பு sockaddr_storage addr;
	socklen_t addrlen;

	memset(&addr, 0, அளவு(கட்டமைப்பு sockaddr_storage));
	addrlen = அளவு(addr);
	எனில்(!getpeername(sock, (கட்டமைப்பு sockaddr *)&addr, &addrlen)){
		எனில்(addr.ss_family == AF_INET){
			எனில்(remote_port){
				*remote_port = ntohs(((கட்டமைப்பு sockaddr_in *)&addr)->sin_port);
			}
			எனில்(inet_ntop(AF_INET, &((கட்டமைப்பு sockaddr_in *)&addr)->sin_addr.s_addr, buf, (socklen_t)len)){
				திரும்பு 0;
			}
		}இல்லையெனில் எனில்(addr.ss_family == AF_INET6){
			எனில்(remote_port){
				*remote_port = ntohs(((கட்டமைப்பு sockaddr_in6 *)&addr)->sin6_port);
			}
			எனில்(inet_ntop(AF_INET6, &((கட்டமைப்பு sockaddr_in6 *)&addr)->sin6_addr.s6_addr, buf, (socklen_t)len)){
				திரும்பு 0;
			}
#ifdef WITH_UNIX_SOCKETS
		}இல்லையெனில் எனில்(addr.ss_family == AF_UNIX){
			கட்டமைப்பு sockaddr_un un;
			addrlen = அளவு(கட்டமைப்பு sockaddr_un);
			எனில்(!getsockname(sock, (கட்டமைப்பு sockaddr *)&un, &addrlen)){
				snprintf(buf, len, "%s", un.sun_path);
			}இல்லையெனில்{
				snprintf(buf, len, "unix-socket");
			}
			திரும்பு 0;
#endif
		}
	}
	திரும்பு 1;
}
