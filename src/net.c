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

#समावेश "config.h"

#अगर_अघोषित WIN32
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
#पूर्ण_अगर

#समावेश <errno.h>
#समावेश <fcntl.h>
#समावेश <stdio.h>
#समावेश <string.h>
#अगर_घोषित WITH_WRAP
#  include <tcpd.h>
#पूर्ण_अगर

#अगर_घोषित HAVE_NETINET_IN_H
#  include <netinet/in.h>
#पूर्ण_अगर

#अगर defined(WITH_UNIX_SOCKETS) || defined(WITH_TLS)
#  include "sys/stat.h"
#पूर्ण_अगर

#अगर_घोषित WITH_UNIX_SOCKETS
#  ifdef WIN32
#    include <afunix.h>
#  अन्यथा
#    include <sys/un.h>
#  endif
#पूर्ण_अगर

#अगर_घोषित __QNX__
#  include <net/netbyte.h>
#पूर्ण_अगर

#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/mqtt_protocol.h"
#समावेश "net_mosq.h"
#समावेश "util_mosq.h"

#अगर_घोषित WITH_TLS
#  include "tls_mosq.h"
#  include <openssl/err.h>
अटल पूर्णांक tls_ex_सूचकांक_context = -1;
अटल पूर्णांक tls_ex_सूचकांक_listener = -1;
#पूर्ण_अगर

#समावेश "sys_tree.h"

/* For EMFILE handling */
अटल mosq_sock_t spare_sock = INVALID_SOCKET;


व्योम net__broker_init(व्योम)
{
	spare_sock = socket(AF_INET, SOCK_STREAM, 0);
	net__init();
#अगर_घोषित WITH_TLS
	net__init_tls();
#पूर्ण_अगर
}


व्योम net__broker_cleanup(व्योम)
{
	अगर(spare_sock != INVALID_SOCKET){
		COMPAT_CLOSE(spare_sock);
		spare_sock = INVALID_SOCKET;
	}
	net__cleanup();
}


अटल व्योम net__print_error(अचिन्हित पूर्णांक log, स्थिर अक्षर *format_str)
{
	अक्षर *buf;

#अगर_घोषित WIN32
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_बफर | FORMAT_MESSAGE_FROM_SYSTEM,
			शून्य, WSAGetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, शून्य);

	log__printf(शून्य, log, format_str, buf);
	LocalFree(buf);
#अन्यथा
	buf = म_त्रुटि(त्रुटि_सं);
	log__printf(शून्य, log, format_str, buf);
#पूर्ण_अगर
}


काष्ठा mosquitto *net__socket_accept(काष्ठा mosquitto__listener_sock *listensock)
{
	mosq_sock_t new_sock = INVALID_SOCKET;
	काष्ठा mosquitto *new_context;
#अगर_घोषित WITH_TLS
	BIO *bio;
#पूर्ण_अगर
#अगर_घोषित WITH_WRAP
	काष्ठा request_info wrap_req;
	अक्षर address[1024];
#पूर्ण_अगर

	new_sock = accept(listensock->sock, शून्य, 0);
	अगर(new_sock == INVALID_SOCKET){
#अगर_घोषित WIN32
		WINDOWS_SET_ERRNO();
		अगर(त्रुटि_सं == WSAEMFILE){
#अन्यथा
		अगर(त्रुटि_सं == EMFILE || त्रुटि_सं == ENFILE){
#पूर्ण_अगर
			/* Close the spare socket, which means we should be able to accept
			 * this connection. Accept it, then close it immediately and create
			 * a new spare_sock. This prevents the situation of ever properly
			 * running out of sockets.
			 * It would be nice to send a "server not available" connack here,
			 * but there are lots of reasons why this would be tricky (TLS
			 * being the big one). */
			COMPAT_CLOSE(spare_sock);
			new_sock = accept(listensock->sock, शून्य, 0);
			अगर(new_sock != INVALID_SOCKET){
				COMPAT_CLOSE(new_sock);
			}
			spare_sock = socket(AF_INET, SOCK_STREAM, 0);
			log__printf(शून्य, MOSQ_LOG_WARNING,
					"Unable to accept new connection, system socket count has been exceeded. Try increasing \"ulimit -n\" or equivalent.");
		}
		वापस शून्य;
	}

	metrics__int_inc(mosq_गणक_socket_connections, 1);

	अगर(net__socket_nonblock(&new_sock)){
		वापस शून्य;
	}

#अगर_घोषित WITH_WRAP
	/* Use tcpd / libwrap to determine whether a connection is allowed. */
	request_init(&wrap_req, RQ_FILE, new_sock, RQ_DAEMON, "mosquitto", 0);
	fromhost(&wrap_req);
	अगर(!hosts_access(&wrap_req)){
		/* Access is denied */
		अगर(db.config->connection_messages == true){
			अगर(!net__socket_get_address(new_sock, address, 1024, शून्य)){
				log__printf(शून्य, MOSQ_LOG_NOTICE, "Client connection from %s denied access by tcpd.", address);
			}
		}
		COMPAT_CLOSE(new_sock);
		वापस शून्य;
	}
#पूर्ण_अगर

	अगर(db.config->set_tcp_nodelay && listensock->listener->port){
		पूर्णांक flag = 1;
		अगर(setsockopt(new_sock, IPPROTO_TCP, TCP_NODELAY, (स्थिर व्योम *)&flag, माप(पूर्णांक)) != 0){
			log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Unable to set TCP_NODELAY.");
		}
	}

	new_context = context__init();
	अगर(!new_context){
		COMPAT_CLOSE(new_sock);
		वापस शून्य;
	}
	new_context->listener = listensock->listener;
	अगर(!new_context->listener){
		context__cleanup(new_context, true);
		वापस शून्य;
	}
	new_context->listener->client_गिनती++;

	अगर(new_context->listener->enable_proxy_protocol){
		अगर(context__init_sock(new_context, new_sock, false) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			वापस शून्य;
		}
		अगर(new_context->listener->enable_proxy_protocol == 2){
			new_context->transport = mosq_t_proxy_v2;
		}अन्यथा{
			new_context->transport = mosq_t_proxy_v1;
		}
		new_context->proxy.cmd = -1;
	}अन्यथा{
		अगर(context__init_sock(new_context, new_sock, true) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			वापस शून्य;
		}
		चयन(new_context->listener->protocol){
			हाल mp_mqtt:
				new_context->transport = mosq_t_tcp;
				अवरोध;
#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			हाल mp_websockets:
				अगर(http__context_init(new_context)){
					context__cleanup(new_context, true);
					वापस शून्य;
				}
				अवरोध;
#पूर्ण_अगर
			शेष:
				context__cleanup(new_context, true);
				वापस शून्य;
		}
	}

	अगर((new_context->listener->max_connections > 0 && new_context->listener->client_गिनती > new_context->listener->max_connections)
			|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (अचिन्हित पूर्णांक)db.config->global_max_connections)){

		अगर(db.config->connection_messages == true){
			log__printf(शून्य, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", new_context->address);
		}
		context__cleanup(new_context, true);
		वापस शून्य;
	}

#अगर_घोषित WITH_TLS
	/* TLS init */
	अगर(new_context->listener->ssl_ctx){
		new_context->ssl = SSL_new(new_context->listener->ssl_ctx);
		अगर(!new_context->ssl){
			context__cleanup(new_context, true);
			वापस शून्य;
		}
		अगर(!SSL_set_ex_तथ्य(new_context->ssl, tls_ex_सूचकांक_context, new_context)
				|| !SSL_set_ex_तथ्य(new_context->ssl, tls_ex_सूचकांक_listener, new_context->listener)){
			context__cleanup(new_context, true);
			वापस शून्य;
		}
		new_context->want_write = true;
		bio = BIO_new_socket(new_sock, BIO_NOCLOSE);
		SSL_set_bio(new_context->ssl, bio, bio);
		ERR_clear_error();
		SSL_set_accept_state(new_context->ssl);
	}
#पूर्ण_अगर

	अगर(db.config->connection_messages == true
			&& !new_context->listener->enable_proxy_protocol){

		log__printf(शून्य, MOSQ_LOG_NOTICE, "New connection from %s:%d on port %d.",
				new_context->address, new_context->remote_port, new_context->listener->port);
	}

	mux__new(new_context);
	keepalive__add(new_context);

	वापस new_context;
}

#अगर_घोषित WITH_TLS


अटल पूर्णांक client_certificate_verify(पूर्णांक preverify_ok, X509_STORE_CTX *ctx)
{
	/* Preverify should check expiry, revocation. */
	अगर(preverify_ok == 0){
		SSL *ssl;
		ssl = X509_STORE_CTX_get_ex_तथ्य(ctx, SSL_get_ex_तथ्य_X509_STORE_CTX_idx());
		अगर(ssl){
			काष्ठा mosquitto__listener *listener;
			listener = SSL_get_ex_तथ्य(ssl, tls_ex_सूचकांक_listener);

			अगर(listener && listener->disable_client_cert_date_checks){
				पूर्णांक err = X509_STORE_CTX_get_error(ctx);
				अगर(err == X509_V_ERR_CERT_NOT_YET_VALID || err == X509_V_ERR_CERT_HAS_EXPIRED){
					preverify_ok = 1;
				}
			}
		}
	}
	वापस preverify_ok;
}
#पूर्ण_अगर

#अगर_घोषित FINAL_WITH_TLS_PSK


अटल अचिन्हित पूर्णांक psk_server_callback(SSL *ssl, स्थिर अक्षर *identity, अचिन्हित अक्षर *psk, अचिन्हित पूर्णांक max_psk_len)
{
	काष्ठा mosquitto *context;
	काष्ठा mosquitto__listener *listener;
	अक्षर *psk_कुंजी = शून्य;
	पूर्णांक len;
	स्थिर अक्षर *psk_hint;

	अगर(!identity){
		वापस 0;
	}

	context = SSL_get_ex_तथ्य(ssl, tls_ex_सूचकांक_context);
	अगर(!context){
		वापस 0;
	}

	listener = SSL_get_ex_तथ्य(ssl, tls_ex_सूचकांक_listener);
	अगर(!listener){
		वापस 0;
	}

	psk_hint = listener->psk_hint;

	/* The hex to BN conversion results in the length halving, so we can pass
	 * max_psk_len*2 as the max hex key here. */
	psk_कुंजी = mosquitto_calloc(1, (माप_प्रकार)max_psk_len*2 + 1);
	अगर(!psk_कुंजी){
		वापस 0;
	}

	अगर(mosquitto_psk_कुंजी_get(context, psk_hint, identity, psk_कुंजी, (पूर्णांक)max_psk_len*2) != MOSQ_ERR_SUCCESS){
		mosquitto_FREE(psk_कुंजी);
		वापस 0;
	}

	len = mosquitto__hex2bin(psk_कुंजी, psk, (पूर्णांक)max_psk_len);
	अगर(len < 0){
		mosquitto_FREE(psk_कुंजी);
		वापस 0;
	}

	अगर(listener->use_identity_as_username){
		अगर(mosquitto_validate_utf8(identity, (पूर्णांक)म_माप(identity))){
			mosquitto_free(psk_कुंजी);
			वापस 0;
		}
		context->username = mosquitto_strdup(identity);
		अगर(!context->username){
			mosquitto_FREE(psk_कुंजी);
			वापस 0;
		}
	}

	mosquitto_FREE(psk_कुंजी);
	वापस (अचिन्हित पूर्णांक)len;
}
#पूर्ण_अगर

#अगर_घोषित WITH_TLS


अटल व्योम tls_keylog_callback(स्थिर SSL *ssl, स्थिर अक्षर *पंक्ति)
{
	UNUSED(ssl);

	अगर(db.tls_keylog){
		खाता *fptr;
		fptr = mosquitto_fopen(db.tls_keylog, "at", true);
		अगर(fptr){
#अगर_अघोषित WIN32
			/* Until mosquitto_fopen enforces file permissions on all files,
			 * enforce it here. We can enforce it here, because it isn't a
			 * change of behaviour. */
			काष्ठा stat statbuf;
			अगर(ख_स्थिति(fileno(fptr), &statbuf) < 0
					|| statbuf.st_mode & (S_IRWXG | S_IRWXO)){

				ख_बंद(fptr);
				वापस;
			}
#पूर्ण_अगर
			ख_लिखो(fptr, "%s\n", पंक्ति);
			ख_बंद(fptr);
		}अन्यथा{
#अगर_अघोषित WIN32
			अगर(त्रुटि_सं == ELOOP){
				log__printf(शून्य, MOSQ_LOG_INFO, "Error: keylog file must not be a symbolic link");
			}अन्यथा
#पूर्ण_अगर
			{
				log__printf(शून्य, MOSQ_LOG_INFO, "Error: Unable to open keylog file: %s", म_त्रुटि(त्रुटि_सं));
			}
		}
	}
}


पूर्णांक net__tls_server_ctx(काष्ठा mosquitto__listener *listener)
{
	अक्षर buf[256];
	पूर्णांक rc;

	अगर(tls_ex_सूचकांक_context == -1){
		tls_ex_सूचकांक_context = SSL_get_ex_new_सूचकांक(0, "client context", शून्य, शून्य, शून्य);
	}
	अगर(tls_ex_सूचकांक_listener == -1){
		tls_ex_सूचकांक_listener = SSL_get_ex_new_सूचकांक(0, "listener", शून्य, शून्य, शून्य);
	}

	अगर(listener->ssl_ctx){
		SSL_CTX_free(listener->ssl_ctx);
		listener->ssl_ctx = शून्य;
	}

	listener->ssl_ctx = SSL_CTX_new(TLS_server_method());

	अगर(!listener->ssl_ctx){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to create TLS context.");
		वापस MOSQ_ERR_TLS;
	}

#अगर_घोषित SSL_OP_NO_TLSv1_3
	अगर(db.config->per_listener_settings){
		अगर(listener->security_options->psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}अन्यथा{
		अगर(db.config->security_options.psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}
#पूर्ण_अगर

	अगर(listener->tls_version == शून्य){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
#अगर_घोषित SSL_OP_NO_TLSv1_3
	}अन्यथा अगर(!म_भेद(listener->tls_version, "tlsv1.3")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2);
#पूर्ण_अगर
	}अन्यथा अगर(!म_भेद(listener->tls_version, "tlsv1.2")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
	}अन्यथा अगर(!म_भेद(listener->tls_version, "tlsv1.1")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1);
		log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: TLS v1.1 is insecure. It must only be used in the case "
				"where you have devices that cannot be upgraded to use a secure version of TLS. It is "
				"recommended to use as secure a set of ciphers as possible and that will restrict it to "
				"TLS v1.1 only, and to use a separate listener using TLS v1.2 and secure ciphers for your "
				"other devices.");
		log__printf(शून्य, MOSQ_LOG_WARNING, "Please be aware that support for TLS v1.1 will go away eventually "
				"and that you should plan now to migrate away from it.");
	}अन्यथा{
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unsupported tls_version \"%s\".", listener->tls_version);
		वापस MOSQ_ERR_TLS;
	}

#अगर_घोषित SSL_OP_NO_COMPRESSION
	/* Disable compression */
	SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_COMPRESSION);
#पूर्ण_अगर
#अगर_घोषित SSL_OP_CIPHER_SERVER_PREFERENCE
	/* Server chooses cipher */
	SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_CIPHER_SERVER_PREFERENCE);
#पूर्ण_अगर

#अगर_घोषित SSL_MODE_RELEASE_BUFFERS
	/* Use even less memory per SSL connection. */
	SSL_CTX_set_mode(listener->ssl_ctx, SSL_MODE_RELEASE_BUFFERS);
#पूर्ण_अगर

	SSL_CTX_set_dh_auto(listener->ssl_ctx, 1);

#अगर_घोषित SSL_OP_NO_RENEGOTIATION
	SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_RENEGOTIATION);
#पूर्ण_अगर

	अगर(db.tls_keylog){
		log__printf(शून्य, MOSQ_LOG_NOTICE, "TLS key logging to '%s' enabled for all listeners.",
				db.tls_keylog);
		log__printf(शून्य, MOSQ_LOG_NOTICE, "TLS key logging is for DEBUGGING only.");

		SSL_CTX_set_keylog_callback(listener->ssl_ctx, tls_keylog_callback);
	}

	snprintf(buf, 256, "mosquitto-%d", listener->port);
	SSL_CTX_set_session_id_context(listener->ssl_ctx, (अचिन्हित अक्षर *)buf, (अचिन्हित पूर्णांक)म_माप(buf));

	अगर(listener->ciphers){
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, listener->ciphers);
		अगर(rc == 0){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			वापस MOSQ_ERR_TLS;
		}
	}अन्यथा{
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, "DEFAULT:!aNULL:!eNULL:!LOW:!EXPORT:!SSLv2:@STRENGTH");
		अगर(rc == 0){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			वापस MOSQ_ERR_TLS;
		}
	}
	अगर(listener->ciphers_tls13){
		rc = SSL_CTX_set_ciphersuites(listener->ssl_ctx, listener->ciphers_tls13);
		अगर(rc == 0){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to set TLS 1.3 ciphersuites. Check cipher_tls13 list \"%s\".", listener->ciphers_tls13);
			वापस MOSQ_ERR_TLS;
		}
	}

	वापस MOSQ_ERR_SUCCESS;
}
#पूर्ण_अगर


#अगर_घोषित WITH_TLS


अटल पूर्णांक net__load_crl_file(काष्ठा mosquitto__listener *listener)
{
	X509_STORE *store;
	X509_LOOKUP *lookup;
	पूर्णांक rc;

	store = SSL_CTX_get_cert_store(listener->ssl_ctx);
	अगर(!store){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to obtain TLS store.");
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		वापस MOSQ_ERR_TLS;
	}
	lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
	rc = X509_load_crl_file(lookup, listener->crlfile, X509_FILETYPE_PEM);
	अगर(rc < 1){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to load certificate revocation file \"%s\". Check crlfile.", listener->crlfile);
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		net__print_ssl_error(शून्य, शून्य);
		वापस MOSQ_ERR_TLS;
	}
	X509_STORE_set_flags(store, X509_V_FLAG_CRL_CHECK);

	वापस MOSQ_ERR_SUCCESS;
}
#पूर्ण_अगर


पूर्णांक net__load_certificates(काष्ठा mosquitto__listener *listener)
{
#अगर_घोषित WITH_TLS
	पूर्णांक rc;

	अगर(listener->require_certificate){
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, client_certificate_verify);
	}अन्यथा{
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_NONE, client_certificate_verify);
	}
	rc = SSL_CTX_use_certificate_chain_file(listener->ssl_ctx, listener->certfile);
	अगर(rc != 1){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
		net__print_ssl_error(शून्य, शून्य);
		वापस MOSQ_ERR_TLS;
	}
	अगर(listener->tls_engine == शून्य || listener->tls_keyform == mosq_k_pem){
		rc = SSL_CTX_use_Private_कुंजी_file(listener->ssl_ctx, listener->keyfile, SSL_FILETYPE_PEM);
		अगर(rc != 1){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			net__print_ssl_error(शून्य, शून्य);
			वापस MOSQ_ERR_TLS;
		}
	}
	rc = SSL_CTX_check_private_कुंजी(listener->ssl_ctx);
	अगर(rc != 1){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Server certificate/key are inconsistent.");
		net__print_ssl_error(शून्य, शून्य);
		वापस MOSQ_ERR_TLS;
	}
	/* Load CRLs if they exist. */
	अगर(listener->crlfile){
		rc = net__load_crl_file(listener);
		अगर(rc){
			वापस rc;
		}
	}
#अन्यथा
	UNUSED(listener);
#पूर्ण_अगर
	वापस MOSQ_ERR_SUCCESS;
}


#अगर defined(WITH_TLS) && !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000


अटल पूर्णांक net__load_engine(काष्ठा mosquitto__listener *listener)
{
	ENGINE *engine = शून्य;
	UI_METHOD *ui_method;
	EVP_PKEY *pkey;

	अगर(!listener->tls_engine){
		वापस MOSQ_ERR_SUCCESS;
	}

	engine = ENGINE_by_id(listener->tls_engine);
	अगर(!engine){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error loading %s engine\n", listener->tls_engine);
		net__print_ssl_error(शून्य, शून्य);
		वापस MOSQ_ERR_TLS;
	}
	अगर(!ENGINE_init(engine)){
		log__printf(शून्य, MOSQ_LOG_ERR, "Failed engine initialisation\n");
		net__print_ssl_error(शून्य, शून्य);
		वापस MOSQ_ERR_TLS;
	}
	ENGINE_set_default(engine, ENGINE_METHOD_ALL);

	अगर(listener->tls_keyform == mosq_k_engine){
		ui_method = net__get_ui_method();
		अगर(listener->tls_engine_kpass_sha1){
			अगर(!ENGINE_ctrl_cmd(engine, ENGINE_SECRET_MODE, ENGINE_SECRET_MODE_SHA, शून्य, शून्य, 0)){
				log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to set engine secret mode sha");
				net__print_ssl_error(शून्य, शून्य);
				वापस MOSQ_ERR_TLS;
			}
			अगर(!ENGINE_ctrl_cmd(engine, ENGINE_PIN, 0, listener->tls_engine_kpass_sha1, शून्य, 0)){
				log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to set engine pin");
				net__print_ssl_error(शून्य, शून्य);
				वापस MOSQ_ERR_TLS;
			}
			ui_method = शून्य;
		}
		pkey = ENGINE_load_private_कुंजी(engine, listener->keyfile, ui_method, शून्य);
		अगर(!pkey){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to load engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(शून्य, शून्य);
			वापस MOSQ_ERR_TLS;
		}
		अगर(SSL_CTX_use_Private_कुंजी(listener->ssl_ctx, pkey) <= 0){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to use engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(शून्य, शून्य);
			वापस MOSQ_ERR_TLS;
		}
	}
	ENGINE_free(engine); /* release the structural reference from ENGINE_by_id() */

	वापस MOSQ_ERR_SUCCESS;
}
#पूर्ण_अगर


पूर्णांक net__tls_load_verify(काष्ठा mosquitto__listener *listener)
{
#अगर_घोषित WITH_TLS
	पूर्णांक rc;

#  अगर OPENSSL_VERSION_संख्या < 0x30000000L
	अगर(listener->cafile || listener->capath){
		rc = SSL_CTX_load_verify_locations(listener->ssl_ctx, listener->cafile, listener->capath);
		अगर(rc == 0){
			अगर(listener->cafile && listener->capath){
				log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\" and capath \"%s\".", listener->cafile, listener->capath);
			}अन्यथा अगर(listener->cafile){
				log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			}अन्यथा{
				log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			}
		}
	}
#  अन्यथा
	ढेर_OF(X509_NAME) *ca_names = शून्य;
	अगर(listener->cafile || listener->capath){
		ca_names = sk_X509_NAME_new_null();
	}

	अगर(listener->cafile){
		rc = SSL_CTX_load_verify_file(listener->ssl_ctx, listener->cafile);
		अगर(rc == 0){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			net__print_ssl_error(शून्य, शून्य);
			वापस MOSQ_ERR_TLS;
		}

		ढेर_OF(X509_NAME) *cert_names = SSL_load_client_CA_file(listener->cafile);
		अगर(cert_names){
			क्रम(पूर्णांक i=0; i<sk_X509_NAME_अंक(cert_names); i++){
				sk_X509_NAME_धकेलो(ca_names, X509_NAME_dup(sk_X509_NAME_मूल्य(cert_names, i)));
			}
			sk_X509_NAME_निकालो_free(cert_names, X509_NAME_free);
		}

	}
	अगर(listener->capath){
		rc = SSL_CTX_load_verify_dir(listener->ssl_ctx, listener->capath);
		अगर(rc == 0){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			net__print_ssl_error(शून्य, शून्य);
			sk_X509_NAME_निकालो_free(ca_names, X509_NAME_free);
			वापस MOSQ_ERR_TLS;
		}

		SSL_add_dir_cert_subjects_to_ढेर(ca_names, listener->capath);
	}
	अगर(ca_names){
		SSL_CTX_set_client_CA_list(listener->ssl_ctx, ca_names);
	}
#  endif

#  अगर !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000
	अगर(net__load_engine(listener)){
		वापस MOSQ_ERR_TLS;
	}
#  endif
#पूर्ण_अगर
	वापस net__load_certificates(listener);
}


#अगर !defined(WIN32) && !defined(_AIX)


अटल पूर्णांक net__bind_interface(काष्ठा mosquitto__listener *listener, काष्ठा addrinfo *rp)
{
	/*
	 * This binds the listener sock to a network interface.
	 * The use of SO_BINDTODEVICE requires root access, which we don't have, so instead
	 * use getifaddrs to find the interface addresses, and use IP of the
	 * matching interface in the later bind().
	 */
	काष्ठा ifaddrs *ifaddr;
	bool have_interface = false;

	अगर(getifaddrs(&ifaddr) < 0){
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		वापस MOSQ_ERR_ERRNO;
	}

	क्रम(काष्ठा ifaddrs *ifa=ifaddr; ifa!=शून्य; ifa=ifa->ifa_अग्र){
		अगर(ifa->ifa_addr == शून्य){
			जारी;
		}

		अगर(!strcasecmp(listener->bind_interface, ifa->ifa_name)){
			have_interface = true;

			अगर(ifa->ifa_addr->sa_family == rp->ai_addr->sa_family){
				अगर(rp->ai_addr->sa_family == AF_INET){
					अगर(listener->host &&
							स_भेद(&((काष्ठा sockaddr_in *)rp->ai_addr)->sin_addr,
							&((काष्ठा sockaddr_in *)ifa->ifa_addr)->sin_addr,
							माप(काष्ठा in_addr))){

						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						वापस MOSQ_ERR_INVAL;
					}अन्यथा{
						स_नकल(&((काष्ठा sockaddr_in *)rp->ai_addr)->sin_addr,
								&((काष्ठा sockaddr_in *)ifa->ifa_addr)->sin_addr,
								माप(काष्ठा in_addr));

						freeifaddrs(ifaddr);
						वापस MOSQ_ERR_SUCCESS;
					}
				}अन्यथा अगर(rp->ai_addr->sa_family == AF_INET6){
					अगर(listener->host &&
							स_भेद(&((काष्ठा sockaddr_in6 *)rp->ai_addr)->sin6_addr,
							&((काष्ठा sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
							माप(काष्ठा in6_addr))){

						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						वापस MOSQ_ERR_INVAL;
					}अन्यथा{
						स_नकल(&((काष्ठा sockaddr_in6 *)rp->ai_addr)->sin6_addr,
								&((काष्ठा sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
								माप(काष्ठा in6_addr));

						((काष्ठा sockaddr_in6 *)rp->ai_addr)->sin6_scope_id = ((काष्ठा sockaddr_in6 *)ifa->ifa_addr)->sin6_scope_id;

						freeifaddrs(ifaddr);
						वापस MOSQ_ERR_SUCCESS;
					}
				}
			}
		}
	}
	freeifaddrs(ifaddr);
	अगर(have_interface){
		log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Interface %s does not support %s configuration.",
				listener->bind_interface, rp->ai_addr->sa_family == AF_INET ? "IPv4" : "IPv6");
		वापस MOSQ_ERR_NOT_SUPPORTED;
	}अन्यथा{
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Interface %s does not exist.", listener->bind_interface);
		वापस MOSQ_ERR_NOT_FOUND;
	}
}
#पूर्ण_अगर


अटल पूर्णांक net__socket_listen_tcp(काष्ठा mosquitto__listener *listener)
{
	mosq_sock_t sock = INVALID_SOCKET;
	काष्ठा addrinfo hints;
	काष्ठा addrinfo *ainfo, *rp;
	अक्षर service[10];
	पूर्णांक rc;
	पूर्णांक ss_opt = 1;
#अगर_अघोषित WIN32
	bool interface_bound = false;
#पूर्ण_अगर

	अगर(!listener){
		वापस MOSQ_ERR_INVAL;
	}

	snprintf(service, 10, "%d", listener->port);
	स_रखो(&hints, 0, माप(काष्ठा addrinfo));
	अगर(listener->socket_domain){
		hints.ai_family = listener->socket_domain;
	}अन्यथा{
		hints.ai_family = AF_UNSPEC;
	}
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	rc = getaddrinfo(listener->host, service, &hints, &ainfo);
	अगर(rc){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error creating listener: %s.", gai_strerror(rc));
		वापस INVALID_SOCKET;
	}

	listener->sock_गिनती = 0;
	listener->socks = शून्य;

	क्रम(rp = ainfo; rp; rp = rp->ai_अग्र){
		अगर(rp->ai_family == AF_INET){
			log__printf(शून्य, MOSQ_LOG_INFO, "Opening ipv4 listen socket on port %d.", ntohs(((काष्ठा sockaddr_in *)rp->ai_addr)->sin_port));
		}अन्यथा अगर(rp->ai_family == AF_INET6){
			log__printf(शून्य, MOSQ_LOG_INFO, "Opening ipv6 listen socket on port %d.", ntohs(((काष्ठा sockaddr_in6 *)rp->ai_addr)->sin6_port));
		}अन्यथा{
			जारी;
		}

		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		अगर(sock == INVALID_SOCKET){
			net__print_error(MOSQ_LOG_WARNING, "Warning: %s");
			जारी;
		}
		listener->sock_गिनती++;
		listener->socks = mosquitto_realloc(listener->socks, माप(mosq_sock_t)*(माप_प्रकार)listener->sock_गिनती);
		अगर(!listener->socks){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			वापस MOSQ_ERR_NOMEM;
		}
		listener->socks[listener->sock_गिनती-1] = sock;

#अगर_अघोषित WIN32
		ss_opt = 1;
		/* Unimportant if this fails */
		(व्योम)setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (स्थिर व्योम *)&ss_opt, माप(ss_opt));
#पूर्ण_अगर
#अगर_घोषित IPV6_V6ONLY
		ss_opt = 1;
		(व्योम)setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (स्थिर व्योम *)&ss_opt, माप(ss_opt));
#पूर्ण_अगर

		अगर(net__socket_nonblock(&sock)){
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			वापस 1;
		}

#अगर !defined(WIN32) && !defined(_AIX)
		अगर(listener->bind_interface){
			/* It might be possible that an interface does not support all relevant sa_families.
			 * We should successfully find at least one. */
			rc = net__bind_interface(listener, rp);
			अगर(rc){
				COMPAT_CLOSE(sock);
				listener->sock_गिनती--;
				अगर(rc == MOSQ_ERR_NOT_FOUND || rc == MOSQ_ERR_INVAL){
					freeaddrinfo(ainfo);
					वापस rc;
				}अन्यथा{
					जारी;
				}
			}
			interface_bound = true;
		}
#पूर्ण_अगर

		अगर(bind(sock, rp->ai_addr, rp->ai_addrlen) == -1){
#अगर defined(__linux__)
			अगर(त्रुटि_सं == EACCES){
				log__printf(शून्य, MOSQ_LOG_ERR, "If you are trying to bind to a privileged port (<1024), try using setcap and do not start the broker as root:");
				log__printf(शून्य, MOSQ_LOG_ERR, "    sudo setcap 'CAP_NET_BIND_SERVICE=+ep /usr/sbin/mosquitto'");
			}
#पूर्ण_अगर
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			COMPAT_CLOSE(sock);
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			वापस 1;
		}

		अगर(listen(sock, 100) == -1){
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			mosquitto_FREE(listener->socks);
			वापस 1;
		}
	}
	freeaddrinfo(ainfo);

#अगर_अघोषित WIN32
	अगर(listener->bind_interface && !interface_bound){
		mosquitto_FREE(listener->socks);
		वापस 1;
	}
#पूर्ण_अगर

	वापस 0;
}


#अगर_घोषित WITH_UNIX_SOCKETS


अटल पूर्णांक net__socket_listen_unix(काष्ठा mosquitto__listener *listener)
{
	काष्ठा sockaddr_un addr;
	mosq_sock_t sock = INVALID_SOCKET;
	पूर्णांक rc;
#अगर_अघोषित WIN32
	mode_t old_mask;
#पूर्ण_अगर

	अगर(listener->unix_socket_path == शून्य){
		वापस MOSQ_ERR_INVAL;
	}
	अगर(म_माप(listener->unix_socket_path) > माप(addr.sun_path)-1){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Path to unix socket is too long \"%s\".", listener->unix_socket_path);
		वापस MOSQ_ERR_INVAL;
	}

#अगर_घोषित WIN32
	DeleteFile(listener->unix_socket_path);
#अन्यथा
	unlink(listener->unix_socket_path);
#पूर्ण_अगर
	log__printf(शून्य, MOSQ_LOG_INFO, "Opening unix listen socket on path %s.", listener->unix_socket_path);
	स_रखो(&addr, 0, माप(काष्ठा sockaddr_un));
	addr.sun_family = AF_UNIX;
	म_नकलन(addr.sun_path, listener->unix_socket_path, माप(addr.sun_path)-1);

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	अगर(sock == INVALID_SOCKET){
		net__print_error(MOSQ_LOG_ERR, "Error creating unix socket: %s");
		वापस 1;
	}
	listener->sock_गिनती++;
	listener->socks = mosquitto_realloc(listener->socks, माप(mosq_sock_t)*(माप_प्रकार)listener->sock_गिनती);
	अगर(!listener->socks){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
		COMPAT_CLOSE(sock);
		वापस MOSQ_ERR_NOMEM;
	}
	listener->socks[listener->sock_गिनती-1] = sock;


#अगर_अघोषित WIN32
	old_mask = umask(0007);
#पूर्ण_अगर
	rc = bind(sock, (काष्ठा sockaddr *)&addr, माप(काष्ठा sockaddr_un));
#अगर_अघोषित WIN32
	umask(old_mask);
#पूर्ण_अगर

	अगर(rc == -1){
		net__print_error(MOSQ_LOG_ERR, "Error binding unix socket: %s");
		वापस 1;
	}

	अगर(listen(sock, 10) == -1){
		net__print_error(MOSQ_LOG_ERR, "Error listening to unix socket: %s");
		वापस 1;
	}

	अगर(net__socket_nonblock(&sock)){
		वापस 1;
	}

	वापस 0;
}
#पूर्ण_अगर


/* Creates a socket and listens on port 'port'.
 * Returns 1 on failure
 * Returns 0 on success.
 */
पूर्णांक net__socket_listen(काष्ठा mosquitto__listener *listener)
{
	पूर्णांक rc;

	अगर(!listener){
		वापस MOSQ_ERR_INVAL;
	}

#अगर_घोषित WITH_UNIX_SOCKETS
	अगर(listener->port == 0 && listener->unix_socket_path != शून्य){
		rc = net__socket_listen_unix(listener);
	}अन्यथा
#पूर्ण_अगर
	{
		rc = net__socket_listen_tcp(listener);
	}
	अगर(rc){
		वापस rc;
	}

	/* We need to have at least one working socket. */
	अगर(listener->sock_गिनती > 0){
#अगर_घोषित WITH_TLS
		अगर(listener->certfile && listener->keyfile){
			अगर(net__tls_server_ctx(listener)){
				वापस 1;
			}

			अगर(net__tls_load_verify(listener)){
				वापस 1;
			}
		}
#  ifdef FINAL_WITH_TLS_PSK
		अगर(listener->psk_hint){
			अगर(listener->certfile == शून्य || listener->keyfile == शून्य){
				अगर(net__tls_server_ctx(listener)){
					वापस 1;
				}
			}
			SSL_CTX_set_psk_server_callback(listener->ssl_ctx, psk_server_callback);
			अगर(listener->psk_hint){
				rc = SSL_CTX_use_psk_identity_hint(listener->ssl_ctx, listener->psk_hint);
				अगर(rc == 0){
					log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to set TLS PSK hint.");
					net__print_ssl_error(शून्य, शून्य);
					वापस 1;
				}
			}
		}
#  endif /* FINAL_WITH_TLS_PSK */
		अगर(tls_ex_सूचकांक_context == -1){
			tls_ex_सूचकांक_context = SSL_get_ex_new_सूचकांक(0, "client context", शून्य, शून्य, शून्य);
		}
		अगर(tls_ex_सूचकांक_listener == -1){
			tls_ex_सूचकांक_listener = SSL_get_ex_new_सूचकांक(0, "listener", शून्य, शून्य, शून्य);
		}
#पूर्ण_अगर /* WITH_TLS */
		वापस 0;
	}अन्यथा{
		वापस 1;
	}
}


पूर्णांक net__socket_get_address(mosq_sock_t sock, अक्षर *buf, माप_प्रकार len, uint16_t *remote_port)
{
	काष्ठा sockaddr_storage addr;
	socklen_t addrlen;

	स_रखो(&addr, 0, माप(काष्ठा sockaddr_storage));
	addrlen = माप(addr);
	अगर(!getpeername(sock, (काष्ठा sockaddr *)&addr, &addrlen)){
		अगर(addr.ss_family == AF_INET){
			अगर(remote_port){
				*remote_port = ntohs(((काष्ठा sockaddr_in *)&addr)->sin_port);
			}
			अगर(inet_ntop(AF_INET, &((काष्ठा sockaddr_in *)&addr)->sin_addr.s_addr, buf, (socklen_t)len)){
				वापस 0;
			}
		}अन्यथा अगर(addr.ss_family == AF_INET6){
			अगर(remote_port){
				*remote_port = ntohs(((काष्ठा sockaddr_in6 *)&addr)->sin6_port);
			}
			अगर(inet_ntop(AF_INET6, &((काष्ठा sockaddr_in6 *)&addr)->sin6_addr.s6_addr, buf, (socklen_t)len)){
				वापस 0;
			}
#अगर_घोषित WITH_UNIX_SOCKETS
		}अन्यथा अगर(addr.ss_family == AF_UNIX){
			काष्ठा sockaddr_un un;
			addrlen = माप(काष्ठा sockaddr_un);
			अगर(!getsockname(sock, (काष्ठा sockaddr *)&un, &addrlen)){
				snprintf(buf, len, "%s", un.sun_path);
			}अन्यथा{
				snprintf(buf, len, "unix-socket");
			}
			वापस 0;
#पूर्ण_अगर
		}
	}
	वापस 1;
}
