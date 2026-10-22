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
#अन्यथा
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

#जर defined(WITH_UNIX_SOCKETS) || defined(WITH_TLS)
#  include "sys/stat.h"
#endif

#ifdef WITH_UNIX_SOCKETS
#  ifdef WIN32
#    include <afunix.h>
#  अन्यथा
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
स्थिर पूर्णांक tls_ex_index_context = -1;
स्थिर पूर्णांक tls_ex_index_listener = -1;
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
	जर(spare_sock != INVALID_SOCKET){
		COMPAT_CLOSE(spare_sock);
		spare_sock = INVALID_SOCKET;
	}
	net__cleanup();
}


स्थिर शून्य net__print_error(अचिन्हित पूर्णांक log, स्थिरांक अक्षर *format_str)
{
	अक्षर *buf;

#ifdef WIN32
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			रिक्त, WSAGetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, रिक्त);

	log__printf(रिक्त, log, format_str, buf);
	LocalFree(buf);
#अन्यथा
	buf = म_त्रुटि(त्रुटि_सं);
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
	अक्षर address[1024];
#endif

	new_sock = accept(listensock->sock, रिक्त, 0);
	जर(new_sock == INVALID_SOCKET){
#ifdef WIN32
		WINDOWS_SET_ERRNO();
		जर(त्रुटि_सं == WSAEMFILE){
#अन्यथा
		जर(त्रुटि_सं == EMFILE || त्रुटि_सं == ENFILE){
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
			जर(new_sock != INVALID_SOCKET){
				COMPAT_CLOSE(new_sock);
			}
			spare_sock = socket(AF_INET, SOCK_STREAM, 0);
			log__printf(रिक्त, MOSQ_LOG_WARNING,
					"Unable to accept new connection, system socket count has been exceeded. Try increasing \"ulimit -n\" or equivalent.");
		}
		परत रिक्त;
	}

	metrics__int_inc(mosq_counter_socket_connections, 1);

	जर(net__socket_nonblock(&new_sock)){
		परत रिक्त;
	}

#ifdef WITH_WRAP
	/* Use tcpd / libwrap to determine whether a connection is allowed. */
	request_init(&wrap_req, RQ_FILE, new_sock, RQ_DAEMON, "mosquitto", 0);
	fromhost(&wrap_req);
	जर(!hosts_access(&wrap_req)){
		/* Access is denied */
		जर(db.config->connection_messages == true){
			जर(!net__socket_get_address(new_sock, address, 1024, रिक्त)){
				log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client connection from %s denied access by tcpd.", address);
			}
		}
		COMPAT_CLOSE(new_sock);
		परत रिक्त;
	}
#endif

	जर(db.config->set_tcp_nodelay && listensock->listener->port){
		पूर्णांक flag = 1;
		जर(setsockopt(new_sock, IPPROTO_TCP, TCP_NODELAY, (स्थिरांक शून्य *)&flag, आकार(पूर्णांक)) != 0){
			log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Unable to set TCP_NODELAY.");
		}
	}

	new_context = context__init();
	जर(!new_context){
		COMPAT_CLOSE(new_sock);
		परत रिक्त;
	}
	new_context->listener = listensock->listener;
	जर(!new_context->listener){
		context__cleanup(new_context, true);
		परत रिक्त;
	}
	new_context->listener->client_मोजणी++;

	जर(new_context->listener->enable_proxy_protocol){
		जर(context__init_sock(new_context, new_sock, false) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			परत रिक्त;
		}
		जर(new_context->listener->enable_proxy_protocol == 2){
			new_context->transport = mosq_t_proxy_v2;
		}अन्यथा{
			new_context->transport = mosq_t_proxy_v1;
		}
		new_context->proxy.cmd = -1;
	}अन्यथा{
		जर(context__init_sock(new_context, new_sock, true) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			परत रिक्त;
		}
		निवड(new_context->listener->protocol){
			स्थिती mp_mqtt:
				new_context->transport = mosq_t_tcp;
				थांबा;
#जर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			स्थिती mp_websockets:
				जर(http__context_init(new_context)){
					context__cleanup(new_context, true);
					परत रिक्त;
				}
				थांबा;
#endif
			पूर्वनिर्धारित:
				context__cleanup(new_context, true);
				परत रिक्त;
		}
	}

	जर((new_context->listener->max_connections > 0 && new_context->listener->client_मोजणी > new_context->listener->max_connections)
			|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (अचिन्हित पूर्णांक)db.config->global_max_connections)){

		जर(db.config->connection_messages == true){
			log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", new_context->address);
		}
		context__cleanup(new_context, true);
		परत रिक्त;
	}

#ifdef WITH_TLS
	/* TLS init */
	जर(new_context->listener->ssl_ctx){
		new_context->ssl = SSL_new(new_context->listener->ssl_ctx);
		जर(!new_context->ssl){
			context__cleanup(new_context, true);
			परत रिक्त;
		}
		जर(!SSL_set_ex_data(new_context->ssl, tls_ex_index_context, new_context)
				|| !SSL_set_ex_data(new_context->ssl, tls_ex_index_listener, new_context->listener)){
			context__cleanup(new_context, true);
			परत रिक्त;
		}
		new_context->want_write = true;
		bio = BIO_new_socket(new_sock, BIO_NOCLOSE);
		SSL_set_bio(new_context->ssl, bio, bio);
		ERR_clear_error();
		SSL_set_accept_state(new_context->ssl);
	}
#endif

	जर(db.config->connection_messages == true
			&& !new_context->listener->enable_proxy_protocol){

		log__printf(रिक्त, MOSQ_LOG_NOTICE, "New connection from %s:%d on port %d.",
				new_context->address, new_context->remote_port, new_context->listener->port);
	}

	mux__new(new_context);
	keepalive__add(new_context);

	परत new_context;
}

#ifdef WITH_TLS


स्थिर पूर्णांक client_certificate_verify(पूर्णांक preverify_ok, X509_STORE_CTX *ctx)
{
	/* Preverify should check expiry, revocation. */
	जर(preverify_ok == 0){
		SSL *ssl;
		ssl = X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx());
		जर(ssl){
			संरचना mosquitto__listener *listener;
			listener = SSL_get_ex_data(ssl, tls_ex_index_listener);

			जर(listener && listener->disable_client_cert_date_checks){
				पूर्णांक err = X509_STORE_CTX_get_error(ctx);
				जर(err == X509_V_ERR_CERT_NOT_YET_VALID || err == X509_V_ERR_CERT_HAS_EXPIRED){
					preverify_ok = 1;
				}
			}
		}
	}
	परत preverify_ok;
}
#endif

#ifdef FINAL_WITH_TLS_PSK


स्थिर अचिन्हित पूर्णांक psk_server_callback(SSL *ssl, स्थिरांक अक्षर *identity, अचिन्हित अक्षर *psk, अचिन्हित पूर्णांक max_psk_len)
{
	संरचना mosquitto *context;
	संरचना mosquitto__listener *listener;
	अक्षर *psk_किल्ली = रिक्त;
	पूर्णांक len;
	स्थिरांक अक्षर *psk_hint;

	जर(!identity){
		परत 0;
	}

	context = SSL_get_ex_data(ssl, tls_ex_index_context);
	जर(!context){
		परत 0;
	}

	listener = SSL_get_ex_data(ssl, tls_ex_index_listener);
	जर(!listener){
		परत 0;
	}

	psk_hint = listener->psk_hint;

	/* The hex to BN conversion results in the length halving, so we can pass
	 * max_psk_len*2 as the max hex key here. */
	psk_किल्ली = mosquitto_calloc(1, (माप_प्रकार)max_psk_len*2 + 1);
	जर(!psk_किल्ली){
		परत 0;
	}

	जर(mosquitto_psk_किल्ली_get(context, psk_hint, identity, psk_किल्ली, (पूर्णांक)max_psk_len*2) != MOSQ_ERR_SUCCESS){
		mosquitto_FREE(psk_किल्ली);
		परत 0;
	}

	len = mosquitto__hex2bin(psk_किल्ली, psk, (पूर्णांक)max_psk_len);
	जर(len < 0){
		mosquitto_FREE(psk_किल्ली);
		परत 0;
	}

	जर(listener->use_identity_as_username){
		जर(mosquitto_validate_utf8(identity, (पूर्णांक)म_माप(identity))){
			mosquitto_free(psk_किल्ली);
			परत 0;
		}
		context->username = mosquitto_strdup(identity);
		जर(!context->username){
			mosquitto_FREE(psk_किल्ली);
			परत 0;
		}
	}

	mosquitto_FREE(psk_किल्ली);
	परत (अचिन्हित पूर्णांक)len;
}
#endif

#ifdef WITH_TLS


स्थिर शून्य tls_keylog_callback(स्थिरांक SSL *ssl, स्थिरांक अक्षर *ओळ)
{
	UNUSED(ssl);

	जर(db.tls_keylog){
		खाता *fptr;
		fptr = mosquitto_fopen(db.tls_keylog, "at", true);
		जर(fptr){
#ifndef WIN32
			/* Until mosquitto_fopen enforces file permissions on all files,
			 * enforce it here. We can enforce it here, because it isn't a
			 * change of behaviour. */
			संरचना stat statbuf;
			जर(ख_स्थिति(fileno(fptr), &statbuf) < 0
					|| statbuf.st_mode & (S_IRWXG | S_IRWXO)){

				ख_बंद(fptr);
				परत;
			}
#endif
			ख_लिखो(fptr, "%s\n", ओळ);
			ख_बंद(fptr);
		}अन्यथा{
#ifndef WIN32
			जर(त्रुटि_सं == ELOOP){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Error: keylog file must not be a symbolic link");
			}अन्यथा
#endif
			{
				log__printf(रिक्त, MOSQ_LOG_INFO, "Error: Unable to open keylog file: %s", म_त्रुटि(त्रुटि_सं));
			}
		}
	}
}


पूर्णांक net__tls_server_ctx(संरचना mosquitto__listener *listener)
{
	अक्षर buf[256];
	पूर्णांक rc;

	जर(tls_ex_index_context == -1){
		tls_ex_index_context = SSL_get_ex_new_index(0, "client context", रिक्त, रिक्त, रिक्त);
	}
	जर(tls_ex_index_listener == -1){
		tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", रिक्त, रिक्त, रिक्त);
	}

	जर(listener->ssl_ctx){
		SSL_CTX_free(listener->ssl_ctx);
		listener->ssl_ctx = रिक्त;
	}

	listener->ssl_ctx = SSL_CTX_new(TLS_server_method());

	जर(!listener->ssl_ctx){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to create TLS context.");
		परत MOSQ_ERR_TLS;
	}

#ifdef SSL_OP_NO_TLSv1_3
	जर(db.config->per_listener_settings){
		जर(listener->security_options->psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}अन्यथा{
		जर(db.config->security_options.psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}
#endif

	जर(listener->tls_version == रिक्त){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
#ifdef SSL_OP_NO_TLSv1_3
	}अन्यथा जर(!म_भेद(listener->tls_version, "tlsv1.3")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2);
#endif
	}अन्यथा जर(!म_भेद(listener->tls_version, "tlsv1.2")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
	}अन्यथा जर(!म_भेद(listener->tls_version, "tlsv1.1")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1);
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS v1.1 is insecure. It must only be used in the case "
				"where you have devices that cannot be upgraded to use a secure version of TLS. It is "
				"recommended to use as secure a set of ciphers as possible and that will restrict it to "
				"TLS v1.1 only, and to use a separate listener using TLS v1.2 and secure ciphers for your "
				"other devices.");
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Please be aware that support for TLS v1.1 will go away eventually "
				"and that you should plan now to migrate away from it.");
	}अन्यथा{
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unsupported tls_version \"%s\".", listener->tls_version);
		परत MOSQ_ERR_TLS;
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

	जर(db.tls_keylog){
		log__printf(रिक्त, MOSQ_LOG_NOTICE, "TLS key logging to '%s' enabled for all listeners.",
				db.tls_keylog);
		log__printf(रिक्त, MOSQ_LOG_NOTICE, "TLS key logging is for DEBUGGING only.");

		SSL_CTX_set_keylog_callback(listener->ssl_ctx, tls_keylog_callback);
	}

	snprintf(buf, 256, "mosquitto-%d", listener->port);
	SSL_CTX_set_session_id_context(listener->ssl_ctx, (अचिन्हित अक्षर *)buf, (अचिन्हित पूर्णांक)म_माप(buf));

	जर(listener->ciphers){
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, listener->ciphers);
		जर(rc == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			परत MOSQ_ERR_TLS;
		}
	}अन्यथा{
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, "DEFAULT:!aNULL:!eNULL:!LOW:!EXPORT:!SSLv2:@STRENGTH");
		जर(rc == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			परत MOSQ_ERR_TLS;
		}
	}
	जर(listener->ciphers_tls13){
		rc = SSL_CTX_set_ciphersuites(listener->ssl_ctx, listener->ciphers_tls13);
		जर(rc == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to set TLS 1.3 ciphersuites. Check cipher_tls13 list \"%s\".", listener->ciphers_tls13);
			परत MOSQ_ERR_TLS;
		}
	}

	परत MOSQ_ERR_SUCCESS;
}
#endif


#ifdef WITH_TLS


स्थिर पूर्णांक net__load_crl_file(संरचना mosquitto__listener *listener)
{
	X509_STORE *store;
	X509_LOOKUP *lookup;
	पूर्णांक rc;

	store = SSL_CTX_get_cert_store(listener->ssl_ctx);
	जर(!store){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to obtain TLS store.");
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		परत MOSQ_ERR_TLS;
	}
	lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
	rc = X509_load_crl_file(lookup, listener->crlfile, X509_FILETYPE_PEM);
	जर(rc < 1){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load certificate revocation file \"%s\". Check crlfile.", listener->crlfile);
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		net__print_ssl_error(रिक्त, रिक्त);
		परत MOSQ_ERR_TLS;
	}
	X509_STORE_set_flags(store, X509_V_FLAG_CRL_CHECK);

	परत MOSQ_ERR_SUCCESS;
}
#endif


पूर्णांक net__load_certificates(संरचना mosquitto__listener *listener)
{
#ifdef WITH_TLS
	पूर्णांक rc;

	जर(listener->require_certificate){
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, client_certificate_verify);
	}अन्यथा{
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_NONE, client_certificate_verify);
	}
	rc = SSL_CTX_use_certificate_chain_file(listener->ssl_ctx, listener->certfile);
	जर(rc != 1){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
		net__print_ssl_error(रिक्त, रिक्त);
		परत MOSQ_ERR_TLS;
	}
	जर(listener->tls_engine == रिक्त || listener->tls_keyform == mosq_k_pem){
		rc = SSL_CTX_use_Private_किल्ली_file(listener->ssl_ctx, listener->keyfile, SSL_FILETYPE_PEM);
		जर(rc != 1){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			net__print_ssl_error(रिक्त, रिक्त);
			परत MOSQ_ERR_TLS;
		}
	}
	rc = SSL_CTX_check_private_किल्ली(listener->ssl_ctx);
	जर(rc != 1){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Server certificate/key are inconsistent.");
		net__print_ssl_error(रिक्त, रिक्त);
		परत MOSQ_ERR_TLS;
	}
	/* Load CRLs if they exist. */
	जर(listener->crlfile){
		rc = net__load_crl_file(listener);
		जर(rc){
			परत rc;
		}
	}
#अन्यथा
	UNUSED(listener);
#endif
	परत MOSQ_ERR_SUCCESS;
}


#जर defined(WITH_TLS) && !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000


स्थिर पूर्णांक net__load_engine(संरचना mosquitto__listener *listener)
{
	ENGINE *engine = रिक्त;
	UI_METHOD *ui_method;
	EVP_PKEY *pkey;

	जर(!listener->tls_engine){
		परत MOSQ_ERR_SUCCESS;
	}

	engine = ENGINE_by_id(listener->tls_engine);
	जर(!engine){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error loading %s engine\n", listener->tls_engine);
		net__print_ssl_error(रिक्त, रिक्त);
		परत MOSQ_ERR_TLS;
	}
	जर(!ENGINE_init(engine)){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Failed engine initialisation\n");
		net__print_ssl_error(रिक्त, रिक्त);
		परत MOSQ_ERR_TLS;
	}
	ENGINE_set_default(engine, ENGINE_METHOD_ALL);

	जर(listener->tls_keyform == mosq_k_engine){
		ui_method = net__get_ui_method();
		जर(listener->tls_engine_kpass_sha1){
			जर(!ENGINE_ctrl_cmd(engine, ENGINE_SECRET_MODE, ENGINE_SECRET_MODE_SHA, रिक्त, रिक्त, 0)){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to set engine secret mode sha");
				net__print_ssl_error(रिक्त, रिक्त);
				परत MOSQ_ERR_TLS;
			}
			जर(!ENGINE_ctrl_cmd(engine, ENGINE_PIN, 0, listener->tls_engine_kpass_sha1, रिक्त, 0)){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to set engine pin");
				net__print_ssl_error(रिक्त, रिक्त);
				परत MOSQ_ERR_TLS;
			}
			ui_method = रिक्त;
		}
		pkey = ENGINE_load_private_किल्ली(engine, listener->keyfile, ui_method, रिक्त);
		जर(!pkey){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(रिक्त, रिक्त);
			परत MOSQ_ERR_TLS;
		}
		जर(SSL_CTX_use_Private_किल्ली(listener->ssl_ctx, pkey) <= 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to use engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(रिक्त, रिक्त);
			परत MOSQ_ERR_TLS;
		}
	}
	ENGINE_free(engine); /* release the structural reference from ENGINE_by_id() */

	परत MOSQ_ERR_SUCCESS;
}
#endif


पूर्णांक net__tls_load_verify(संरचना mosquitto__listener *listener)
{
#ifdef WITH_TLS
	पूर्णांक rc;

#  जर OPENSSL_VERSION_संख्या < 0x30000000L
	जर(listener->cafile || listener->capath){
		rc = SSL_CTX_load_verify_locations(listener->ssl_ctx, listener->cafile, listener->capath);
		जर(rc == 0){
			जर(listener->cafile && listener->capath){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\" and capath \"%s\".", listener->cafile, listener->capath);
			}अन्यथा जर(listener->cafile){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			}अन्यथा{
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			}
		}
	}
#  अन्यथा
	चळत_OF(X509_NAME) *ca_names = रिक्त;
	जर(listener->cafile || listener->capath){
		ca_names = sk_X509_NAME_new_null();
	}

	जर(listener->cafile){
		rc = SSL_CTX_load_verify_file(listener->ssl_ctx, listener->cafile);
		जर(rc == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			net__print_ssl_error(रिक्त, रिक्त);
			परत MOSQ_ERR_TLS;
		}

		चळत_OF(X509_NAME) *cert_names = SSL_load_client_CA_file(listener->cafile);
		जर(cert_names){
			साठी(पूर्णांक i=0; i<sk_X509_NAME_अंक(cert_names); i++){
				sk_X509_NAME_ढकला(ca_names, X509_NAME_dup(sk_X509_NAME_मूल्य(cert_names, i)));
			}
			sk_X509_NAME_काढा_free(cert_names, X509_NAME_free);
		}

	}
	जर(listener->capath){
		rc = SSL_CTX_load_verify_dir(listener->ssl_ctx, listener->capath);
		जर(rc == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			net__print_ssl_error(रिक्त, रिक्त);
			sk_X509_NAME_काढा_free(ca_names, X509_NAME_free);
			परत MOSQ_ERR_TLS;
		}

		SSL_add_dir_cert_subjects_to_चळत(ca_names, listener->capath);
	}
	जर(ca_names){
		SSL_CTX_set_client_CA_list(listener->ssl_ctx, ca_names);
	}
#  endif

#  जर !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000
	जर(net__load_engine(listener)){
		परत MOSQ_ERR_TLS;
	}
#  endif
#endif
	परत net__load_certificates(listener);
}


#जर !defined(WIN32) && !defined(_AIX)


स्थिर पूर्णांक net__bind_interface(संरचना mosquitto__listener *listener, संरचना addrinfo *rp)
{
	/*
	 * This binds the listener sock to a network interface.
	 * The use of SO_BINDTODEVICE requires root access, which we don't have, so instead
	 * use getifaddrs to find the interface addresses, and use IP of the
	 * matching interface in the later bind().
	 */
	संरचना ifaddrs *ifaddr;
	bool have_interface = false;

	जर(getifaddrs(&ifaddr) < 0){
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		परत MOSQ_ERR_ERRNO;
	}

	साठी(संरचना ifaddrs *ifa=ifaddr; ifa!=रिक्त; ifa=ifa->ifa_next){
		जर(ifa->ifa_addr == रिक्त){
			सुरूठेवा;
		}

		जर(!strcasecmp(listener->bind_interface, ifa->ifa_name)){
			have_interface = true;

			जर(ifa->ifa_addr->sa_family == rp->ai_addr->sa_family){
				जर(rp->ai_addr->sa_family == AF_INET){
					जर(listener->host &&
							स_भेद(&((संरचना sockaddr_in *)rp->ai_addr)->sin_addr,
							&((संरचना sockaddr_in *)ifa->ifa_addr)->sin_addr,
							आकार(संरचना in_addr))){

						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						परत MOSQ_ERR_INVAL;
					}अन्यथा{
						स_नकल(&((संरचना sockaddr_in *)rp->ai_addr)->sin_addr,
								&((संरचना sockaddr_in *)ifa->ifa_addr)->sin_addr,
								आकार(संरचना in_addr));

						freeifaddrs(ifaddr);
						परत MOSQ_ERR_SUCCESS;
					}
				}अन्यथा जर(rp->ai_addr->sa_family == AF_INET6){
					जर(listener->host &&
							स_भेद(&((संरचना sockaddr_in6 *)rp->ai_addr)->sin6_addr,
							&((संरचना sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
							आकार(संरचना in6_addr))){

						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						परत MOSQ_ERR_INVAL;
					}अन्यथा{
						स_नकल(&((संरचना sockaddr_in6 *)rp->ai_addr)->sin6_addr,
								&((संरचना sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
								आकार(संरचना in6_addr));

						((संरचना sockaddr_in6 *)rp->ai_addr)->sin6_scope_id = ((संरचना sockaddr_in6 *)ifa->ifa_addr)->sin6_scope_id;

						freeifaddrs(ifaddr);
						परत MOSQ_ERR_SUCCESS;
					}
				}
			}
		}
	}
	freeifaddrs(ifaddr);
	जर(have_interface){
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Interface %s does not support %s configuration.",
				listener->bind_interface, rp->ai_addr->sa_family == AF_INET ? "IPv4" : "IPv6");
		परत MOSQ_ERR_NOT_SUPPORTED;
	}अन्यथा{
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Interface %s does not exist.", listener->bind_interface);
		परत MOSQ_ERR_NOT_FOUND;
	}
}
#endif


स्थिर पूर्णांक net__socket_listen_tcp(संरचना mosquitto__listener *listener)
{
	mosq_sock_t sock = INVALID_SOCKET;
	संरचना addrinfo hints;
	संरचना addrinfo *ainfo, *rp;
	अक्षर service[10];
	पूर्णांक rc;
	पूर्णांक ss_opt = 1;
#ifndef WIN32
	bool interface_bound = false;
#endif

	जर(!listener){
		परत MOSQ_ERR_INVAL;
	}

	snprintf(service, 10, "%d", listener->port);
	स_रखो(&hints, 0, आकार(संरचना addrinfo));
	जर(listener->socket_domain){
		hints.ai_family = listener->socket_domain;
	}अन्यथा{
		hints.ai_family = AF_UNSPEC;
	}
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	rc = getaddrinfo(listener->host, service, &hints, &ainfo);
	जर(rc){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating listener: %s.", gai_strerror(rc));
		परत INVALID_SOCKET;
	}

	listener->sock_मोजणी = 0;
	listener->socks = रिक्त;

	साठी(rp = ainfo; rp; rp = rp->ai_next){
		जर(rp->ai_family == AF_INET){
			log__printf(रिक्त, MOSQ_LOG_INFO, "Opening ipv4 listen socket on port %d.", ntohs(((संरचना sockaddr_in *)rp->ai_addr)->sin_port));
		}अन्यथा जर(rp->ai_family == AF_INET6){
			log__printf(रिक्त, MOSQ_LOG_INFO, "Opening ipv6 listen socket on port %d.", ntohs(((संरचना sockaddr_in6 *)rp->ai_addr)->sin6_port));
		}अन्यथा{
			सुरूठेवा;
		}

		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		जर(sock == INVALID_SOCKET){
			net__print_error(MOSQ_LOG_WARNING, "Warning: %s");
			सुरूठेवा;
		}
		listener->sock_मोजणी++;
		listener->socks = mosquitto_realloc(listener->socks, आकार(mosq_sock_t)*(माप_प्रकार)listener->sock_मोजणी);
		जर(!listener->socks){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			परत MOSQ_ERR_NOMEM;
		}
		listener->socks[listener->sock_मोजणी-1] = sock;

#ifndef WIN32
		ss_opt = 1;
		/* Unimportant if this fails */
		(शून्य)setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (स्थिरांक शून्य *)&ss_opt, आकार(ss_opt));
#endif
#ifdef IPV6_V6ONLY
		ss_opt = 1;
		(शून्य)setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (स्थिरांक शून्य *)&ss_opt, आकार(ss_opt));
#endif

		जर(net__socket_nonblock(&sock)){
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			परत 1;
		}

#जर !defined(WIN32) && !defined(_AIX)
		जर(listener->bind_interface){
			/* It might be possible that an interface does not support all relevant sa_families.
			 * We should successfully find at least one. */
			rc = net__bind_interface(listener, rp);
			जर(rc){
				COMPAT_CLOSE(sock);
				listener->sock_मोजणी--;
				जर(rc == MOSQ_ERR_NOT_FOUND || rc == MOSQ_ERR_INVAL){
					freeaddrinfo(ainfo);
					परत rc;
				}अन्यथा{
					सुरूठेवा;
				}
			}
			interface_bound = true;
		}
#endif

		जर(bind(sock, rp->ai_addr, rp->ai_addrlen) == -1){
#जर defined(__linux__)
			जर(त्रुटि_सं == EACCES){
				log__printf(रिक्त, MOSQ_LOG_ERR, "If you are trying to bind to a privileged port (<1024), try using setcap and do not start the broker as root:");
				log__printf(रिक्त, MOSQ_LOG_ERR, "    sudo setcap 'CAP_NET_BIND_SERVICE=+ep /usr/sbin/mosquitto'");
			}
#endif
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			COMPAT_CLOSE(sock);
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			परत 1;
		}

		जर(listen(sock, 100) == -1){
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			mosquitto_FREE(listener->socks);
			परत 1;
		}
	}
	freeaddrinfo(ainfo);

#ifndef WIN32
	जर(listener->bind_interface && !interface_bound){
		mosquitto_FREE(listener->socks);
		परत 1;
	}
#endif

	परत 0;
}


#ifdef WITH_UNIX_SOCKETS


स्थिर पूर्णांक net__socket_listen_unix(संरचना mosquitto__listener *listener)
{
	संरचना sockaddr_un addr;
	mosq_sock_t sock = INVALID_SOCKET;
	पूर्णांक rc;
#ifndef WIN32
	mode_t old_mask;
#endif

	जर(listener->unix_socket_path == रिक्त){
		परत MOSQ_ERR_INVAL;
	}
	जर(म_माप(listener->unix_socket_path) > आकार(addr.sun_path)-1){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Path to unix socket is too long \"%s\".", listener->unix_socket_path);
		परत MOSQ_ERR_INVAL;
	}

#ifdef WIN32
	DeleteFile(listener->unix_socket_path);
#अन्यथा
	unlink(listener->unix_socket_path);
#endif
	log__printf(रिक्त, MOSQ_LOG_INFO, "Opening unix listen socket on path %s.", listener->unix_socket_path);
	स_रखो(&addr, 0, आकार(संरचना sockaddr_un));
	addr.sun_family = AF_UNIX;
	म_नकलन(addr.sun_path, listener->unix_socket_path, आकार(addr.sun_path)-1);

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	जर(sock == INVALID_SOCKET){
		net__print_error(MOSQ_LOG_ERR, "Error creating unix socket: %s");
		परत 1;
	}
	listener->sock_मोजणी++;
	listener->socks = mosquitto_realloc(listener->socks, आकार(mosq_sock_t)*(माप_प्रकार)listener->sock_मोजणी);
	जर(!listener->socks){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		COMPAT_CLOSE(sock);
		परत MOSQ_ERR_NOMEM;
	}
	listener->socks[listener->sock_मोजणी-1] = sock;


#ifndef WIN32
	old_mask = umask(0007);
#endif
	rc = bind(sock, (संरचना sockaddr *)&addr, आकार(संरचना sockaddr_un));
#ifndef WIN32
	umask(old_mask);
#endif

	जर(rc == -1){
		net__print_error(MOSQ_LOG_ERR, "Error binding unix socket: %s");
		परत 1;
	}

	जर(listen(sock, 10) == -1){
		net__print_error(MOSQ_LOG_ERR, "Error listening to unix socket: %s");
		परत 1;
	}

	जर(net__socket_nonblock(&sock)){
		परत 1;
	}

	परत 0;
}
#endif


/* Creates a socket and listens on port 'port'.
 * Returns 1 on failure
 * Returns 0 on success.
 */
पूर्णांक net__socket_listen(संरचना mosquitto__listener *listener)
{
	पूर्णांक rc;

	जर(!listener){
		परत MOSQ_ERR_INVAL;
	}

#ifdef WITH_UNIX_SOCKETS
	जर(listener->port == 0 && listener->unix_socket_path != रिक्त){
		rc = net__socket_listen_unix(listener);
	}अन्यथा
#endif
	{
		rc = net__socket_listen_tcp(listener);
	}
	जर(rc){
		परत rc;
	}

	/* We need to have at least one working socket. */
	जर(listener->sock_मोजणी > 0){
#ifdef WITH_TLS
		जर(listener->certfile && listener->keyfile){
			जर(net__tls_server_ctx(listener)){
				परत 1;
			}

			जर(net__tls_load_verify(listener)){
				परत 1;
			}
		}
#  ifdef FINAL_WITH_TLS_PSK
		जर(listener->psk_hint){
			जर(listener->certfile == रिक्त || listener->keyfile == रिक्त){
				जर(net__tls_server_ctx(listener)){
					परत 1;
				}
			}
			SSL_CTX_set_psk_server_callback(listener->ssl_ctx, psk_server_callback);
			जर(listener->psk_hint){
				rc = SSL_CTX_use_psk_identity_hint(listener->ssl_ctx, listener->psk_hint);
				जर(rc == 0){
					log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to set TLS PSK hint.");
					net__print_ssl_error(रिक्त, रिक्त);
					परत 1;
				}
			}
		}
#  endif /* FINAL_WITH_TLS_PSK */
		जर(tls_ex_index_context == -1){
			tls_ex_index_context = SSL_get_ex_new_index(0, "client context", रिक्त, रिक्त, रिक्त);
		}
		जर(tls_ex_index_listener == -1){
			tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", रिक्त, रिक्त, रिक्त);
		}
#endif /* WITH_TLS */
		परत 0;
	}अन्यथा{
		परत 1;
	}
}


पूर्णांक net__socket_get_address(mosq_sock_t sock, अक्षर *buf, माप_प्रकार len, uint16_t *remote_port)
{
	संरचना sockaddr_storage addr;
	socklen_t addrlen;

	स_रखो(&addr, 0, आकार(संरचना sockaddr_storage));
	addrlen = आकार(addr);
	जर(!getpeername(sock, (संरचना sockaddr *)&addr, &addrlen)){
		जर(addr.ss_family == AF_INET){
			जर(remote_port){
				*remote_port = ntohs(((संरचना sockaddr_in *)&addr)->sin_port);
			}
			जर(inet_ntop(AF_INET, &((संरचना sockaddr_in *)&addr)->sin_addr.s_addr, buf, (socklen_t)len)){
				परत 0;
			}
		}अन्यथा जर(addr.ss_family == AF_INET6){
			जर(remote_port){
				*remote_port = ntohs(((संरचना sockaddr_in6 *)&addr)->sin6_port);
			}
			जर(inet_ntop(AF_INET6, &((संरचना sockaddr_in6 *)&addr)->sin6_addr.s6_addr, buf, (socklen_t)len)){
				परत 0;
			}
#ifdef WITH_UNIX_SOCKETS
		}अन्यथा जर(addr.ss_family == AF_UNIX){
			संरचना sockaddr_un un;
			addrlen = आकार(संरचना sockaddr_un);
			जर(!getsockname(sock, (संरचना sockaddr *)&un, &addrlen)){
				snprintf(buf, len, "%s", un.sun_path);
			}अन्यथा{
				snprintf(buf, len, "unix-socket");
			}
			परत 0;
#endif
		}
	}
	परत 1;
}
