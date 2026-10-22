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

#यदि_अपरिभाषितम् WIN32
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
#यदि_अन्तः

#अन्तर्भाव <errno.h>
#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>
#यदि_परिभाषितम् WITH_WRAP
#  include <tcpd.h>
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#  include <netinet/in.h>
#यदि_अन्तः

#यदि defined(WITH_UNIX_SOCKETS) || defined(WITH_TLS)
#  include "sys/stat.h"
#यदि_अन्तः

#यदि_परिभाषितम् WITH_UNIX_SOCKETS
#  ifdef WIN32
#    include <afunix.h>
#  अन्यथा
#    include <sys/un.h>
#  endif
#यदि_अन्तः

#यदि_परिभाषितम् __QNX__
#  include <net/netbyte.h>
#यदि_अन्तः

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "net_mosq.h"
#अन्तर्भाव "util_mosq.h"

#यदि_परिभाषितम् WITH_TLS
#  include "tls_mosq.h"
#  include <openssl/err.h>
स्थिर पूर्णाङ्क tls_ex_सूचकांक_context = -1;
स्थिर पूर्णाङ्क tls_ex_सूचकांक_listener = -1;
#यदि_अन्तः

#अन्तर्भाव "sys_tree.h"

/* For EMFILE handling */
स्थिर mosq_sock_t अतिरिक्तः_sock = INVALID_SOCKET;


शून्य net__broker_init(शून्य)
{
	अतिरिक्तः_sock = socket(AF_INET, SOCK_STREAM, 0);
	net__init();
#यदि_परिभाषितम् WITH_TLS
	net__init_tls();
#यदि_अन्तः
}


शून्य net__broker_cleanup(शून्य)
{
	यदि(अतिरिक्तः_sock != INVALID_SOCKET){
		COMPAT_CLOSE(अतिरिक्तः_sock);
		अतिरिक्तः_sock = INVALID_SOCKET;
	}
	net__cleanup();
}


स्थिर शून्य net__print_error(अचिह्नित पूर्णाङ्क लघुगणकः, ध्रुव वर्ण *format_str)
{
	वर्ण *buf;

#यदि_परिभाषितम् WIN32
	Format_सन्देशः(FORMAT_सन्देशः_ALLOCATE_बफर | FORMAT_सन्देशः_FROM_SYSTEM,
			रिक्त, WSAGet_चरमः_Error(), LANG_NEUTRAL, (LPTSTR)&buf, 0, रिक्त);

	log__printf(रिक्त, लघुगणकः, format_str, buf);
	LocalFree(buf);
#अन्यथा
	buf = strerror(errno);
	log__printf(रिक्त, लघुगणकः, format_str, buf);
#यदि_अन्तः
}


संरचना mosquitto *net__socket_accept(संरचना mosquitto__listener_sock *listensock)
{
	mosq_sock_t new_sock = INVALID_SOCKET;
	संरचना mosquitto *new_context;
#यदि_परिभाषितम् WITH_TLS
	BIO *bio;
#यदि_अन्तः
#यदि_परिभाषितम् WITH_WRAP
	संरचना request_info wrap_req;
	वर्ण स्थानाङ्कः[1024];
#यदि_अन्तः

	new_sock = accept(listensock->sock, रिक्त, 0);
	यदि(new_sock == INVALID_SOCKET){
#यदि_परिभाषितम् WIN32
		WINDOWS_समुच्चयः_ERRNO();
		यदि(errno == WSAEMFILE){
#अन्यथा
		यदि(errno == EMFILE || errno == ENFILE){
#यदि_अन्तः
			/* Close the spare socket, which means we should be able to accept
			 * this connection. Accept it, then close it immediately and create
			 * a new spare_sock. This prevents the situation of ever properly
			 * running out of sockets.
			 * It would be nice to send a "server not available" connack here,
			 * but there are lots of reasons why this would be tricky (TLS
			 * being the big one). */
			COMPAT_CLOSE(अतिरिक्तः_sock);
			new_sock = accept(listensock->sock, रिक्त, 0);
			यदि(new_sock != INVALID_SOCKET){
				COMPAT_CLOSE(new_sock);
			}
			अतिरिक्तः_sock = socket(AF_INET, SOCK_STREAM, 0);
			log__printf(रिक्त, MOSQ_LOG_WARNING,
					"Unable to accept new connection, system socket count has been exceeded. Try increasing \"ulimit -n\" or equivalent.");
		}
		प्रत्यागम रिक्त;
	}

	metrics__int_inc(mosq_गणक_socket_connections, 1);

	यदि(net__socket_nonblock(&new_sock)){
		प्रत्यागम रिक्त;
	}

#यदि_परिभाषितम् WITH_WRAP
	/* Use tcpd / libwrap to determine whether a connection is allowed. */
	request_init(&wrap_req, RQ_FILE, new_sock, RQ_DAEMON, "mosquitto", 0);
	fromhost(&wrap_req);
	यदि(!hosts_access(&wrap_req)){
		/* Access is denied */
		यदि(db.config->connection_messages == true){
			यदि(!net_socket_get_स्थानाङ्कः(new_sock, स्थानाङ्कः, 1024, रिक्त)){
				log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client connection from %s denied access by tcpd.", स्थानाङ्कः);
			}
		}
		COMPAT_CLOSE(new_sock);
		प्रत्यागम रिक्त;
	}
#यदि_अन्तः

	यदि(db.config->समुच्चयः_tcp_nodelay && listensock->listener->पत्तनम्){
		पूर्णाङ्क flag = 1;
		यदि(setsockopt(new_sock, IPPROTO_TCP, TCP_NODELAY, (ध्रुव शून्य *)&flag, परिमाण(पूर्णाङ्क)) != 0){
			log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Unable to set TCP_NODELAY.");
		}
	}

	new_context = context__init();
	यदि(!new_context){
		COMPAT_CLOSE(new_sock);
		प्रत्यागम रिक्त;
	}
	new_context->listener = listensock->listener;
	यदि(!new_context->listener){
		context__cleanup(new_context, true);
		प्रत्यागम रिक्त;
	}
	new_context->listener->client_गणनम्++;

	यदि(new_context->listener->enable_proxy_protocol){
		यदि(context__init_sock(new_context, new_sock, false) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			प्रत्यागम रिक्त;
		}
		यदि(new_context->listener->enable_proxy_protocol == 2){
			new_context->transport = mosq_t_proxy_v2;
		}अन्यथा{
			new_context->transport = mosq_t_proxy_v1;
		}
		new_context->proxy.cmd = -1;
	}अन्यथा{
		यदि(context__init_sock(new_context, new_sock, true) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			प्रत्यागम रिक्त;
		}
		विकल्प(new_context->listener->protocol){
			पक्ष mp_mqtt:
				new_context->transport = mosq_t_tcp;
				भङ्ग;
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			पक्ष mp_websockets:
				यदि(http__context_init(new_context)){
					context__cleanup(new_context, true);
					प्रत्यागम रिक्त;
				}
				भङ्ग;
#यदि_अन्तः
			स्वभाव:
				context__cleanup(new_context, true);
				प्रत्यागम रिक्त;
		}
	}

	यदि((new_context->listener->max_connections > 0 && new_context->listener->client_गणनम् > new_context->listener->max_connections)
			|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (अचिह्नित पूर्णाङ्क)db.config->global_max_connections)){

		यदि(db.config->connection_messages == true){
			log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", new_context->स्थानाङ्कः);
		}
		context__cleanup(new_context, true);
		प्रत्यागम रिक्त;
	}

#यदि_परिभाषितम् WITH_TLS
	/* TLS init */
	यदि(new_context->listener->ssl_ctx){
		new_context->ssl = SSL_new(new_context->listener->ssl_ctx);
		यदि(!new_context->ssl){
			context__cleanup(new_context, true);
			प्रत्यागम रिक्त;
		}
		यदि(!SSL_समुच्चयः_ex_दत्त(new_context->ssl, tls_ex_सूचकांक_context, new_context)
				|| !SSL_समुच्चयः_ex_दत्त(new_context->ssl, tls_ex_सूचकांक_listener, new_context->listener)){
			context__cleanup(new_context, true);
			प्रत्यागम रिक्त;
		}
		new_context->want_write = true;
		bio = BIO_new_socket(new_sock, BIO_NOCLOSE);
		SSL_समुच्चयः_bio(new_context->ssl, bio, bio);
		ERR_clear_error();
		SSL_समुच्चयः_accept_state(new_context->ssl);
	}
#यदि_अन्तः

	यदि(db.config->connection_messages == true
			&& !new_context->listener->enable_proxy_protocol){

		log__printf(रिक्त, MOSQ_LOG_NOTICE, "New connection from %s:%d on port %d.",
				new_context->स्थानाङ्कः, new_context->remote_पत्तनम्, new_context->listener->पत्तनम्);
	}

	mux__new(new_context);
	keepalive_सङ्कलय(new_context);

	प्रत्यागम new_context;
}

#यदि_परिभाषितम् WITH_TLS


स्थिर पूर्णाङ्क client_certificate_verify(पूर्णाङ्क preverify_ok, X509_STORE_CTX *ctx)
{
	/* Preverify should check expiry, revocation. */
	यदि(preverify_ok == 0){
		SSL *ssl;
		ssl = X509_STORE_CTX_get_ex_दत्त(ctx, SSL_get_ex_दत्त_X509_STORE_CTX_idx());
		यदि(ssl){
			संरचना mosquitto__listener *listener;
			listener = SSL_get_ex_दत्त(ssl, tls_ex_सूचकांक_listener);

			यदि(listener && listener->disable_client_cert_date_checks){
				पूर्णाङ्क err = X509_STORE_CTX_get_error(ctx);
				यदि(err == X509_V_ERR_CERT_NOT_YET_VALID || err == X509_V_ERR_CERT_HAS_EXPIRED){
					preverify_ok = 1;
				}
			}
		}
	}
	प्रत्यागम preverify_ok;
}
#यदि_अन्तः

#यदि_परिभाषितम् FINAL_WITH_TLS_PSK


स्थिर अचिह्नित पूर्णाङ्क psk_server_callback(SSL *ssl, ध्रुव वर्ण *तटस्थः, अचिह्नित वर्ण *psk, अचिह्नित पूर्णाङ्क max_psk_len)
{
	संरचना mosquitto *context;
	संरचना mosquitto__listener *listener;
	वर्ण *psk_कुञ्चिका = रिक्त;
	पूर्णाङ्क len;
	ध्रुव वर्ण *psk_hint;

	यदि(!तटस्थः){
		प्रत्यागम 0;
	}

	context = SSL_get_ex_दत्त(ssl, tls_ex_सूचकांक_context);
	यदि(!context){
		प्रत्यागम 0;
	}

	listener = SSL_get_ex_दत्त(ssl, tls_ex_सूचकांक_listener);
	यदि(!listener){
		प्रत्यागम 0;
	}

	psk_hint = listener->psk_hint;

	/* The hex to BN conversion results in the length halving, so we can pass
	 * max_psk_len*2 as the max hex key here. */
	psk_कुञ्चिका = mosquitto_calloc(1, (size_t)max_psk_len*2 + 1);
	यदि(!psk_कुञ्चिका){
		प्रत्यागम 0;
	}

	यदि(mosquitto_psk_कुञ्चिका_get(context, psk_hint, तटस्थः, psk_कुञ्चिका, (पूर्णाङ्क)max_psk_len*2) != MOSQ_ERR_SUCCESS){
		mosquitto_FREE(psk_कुञ्चिका);
		प्रत्यागम 0;
	}

	len = mosquitto__hex2bin(psk_कुञ्चिका, psk, (पूर्णाङ्क)max_psk_len);
	यदि(len < 0){
		mosquitto_FREE(psk_कुञ्चिका);
		प्रत्यागम 0;
	}

	यदि(listener->use_तटस्थः_as_username){
		यदि(mosquitto_validate_utf8(तटस्थः, (पूर्णाङ्क)वाक्यदैर्घ्यम्(तटस्थः))){
			mosquitto_free(psk_कुञ्चिका);
			प्रत्यागम 0;
		}
		context->username = mosquitto_strdup(तटस्थः);
		यदि(!context->username){
			mosquitto_FREE(psk_कुञ्चिका);
			प्रत्यागम 0;
		}
	}

	mosquitto_FREE(psk_कुञ्चिका);
	प्रत्यागम (अचिह्नित पूर्णाङ्क)len;
}
#यदि_अन्तः

#यदि_परिभाषितम् WITH_TLS


स्थिर शून्य tls_keylog_callback(ध्रुव SSL *ssl, ध्रुव वर्ण *पङ्क्तिः)
{
	अप्रयुक्तम्(ssl);

	यदि(db.tls_keylog){
		FILE *fptr;
		fptr = mosquitto_fopen(db.tls_keylog, "at", true);
		यदि(fptr){
#यदि_अपरिभाषितम् WIN32
			/* Until mosquitto_fopen enforces file permissions on all files,
			 * enforce it here. We can enforce it here, because it isn't a
			 * change of behaviour. */
			संरचना stat statbuf;
			यदि(fstat(fileno(fptr), &statbuf) < 0
					|| statbuf.st_बहुलकम् & (S_IRWXG | S_IRWXO)){

				fclose(fptr);
				प्रत्यागम;
			}
#यदि_अन्तः
			fprintf(fptr, "%s\n", पङ्क्तिः);
			fclose(fptr);
		}अन्यथा{
#यदि_अपरिभाषितम् WIN32
			यदि(errno == ELOOP){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Error: keylog file must not be a symbolic link");
			}अन्यथा
#यदि_अन्तः
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

	यदि(tls_ex_सूचकांक_context == -1){
		tls_ex_सूचकांक_context = SSL_get_ex_new_सूचकांक(0, "client context", रिक्त, रिक्त, रिक्त);
	}
	यदि(tls_ex_सूचकांक_listener == -1){
		tls_ex_सूचकांक_listener = SSL_get_ex_new_सूचकांक(0, "listener", रिक्त, रिक्त, रिक्त);
	}

	यदि(listener->ssl_ctx){
		SSL_CTX_free(listener->ssl_ctx);
		listener->ssl_ctx = रिक्त;
	}

	listener->ssl_ctx = SSL_CTX_new(TLS_server_विधानम्());

	यदि(!listener->ssl_ctx){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to create TLS context.");
		प्रत्यागम MOSQ_ERR_TLS;
	}

#यदि_परिभाषितम् SSL_OP_NO_TLSv1_3
	यदि(db.config->प्रत्येकम्_listener_settings){
		यदि(listener->security_options->psk_file){
			SSL_CTX_समुच्चयः_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}अन्यथा{
		यदि(db.config->security_options.psk_file){
			SSL_CTX_समुच्चयः_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}
#यदि_अन्तः

	यदि(listener->tls_संस्करणम् == रिक्त){
		SSL_CTX_समुच्चयः_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
#यदि_परिभाषितम् SSL_OP_NO_TLSv1_3
	}अन्यथा यदि(!वाक्यं_तुलय(listener->tls_संस्करणम्, "tlsv1.3")){
		SSL_CTX_समुच्चयः_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2);
#यदि_अन्तः
	}अन्यथा यदि(!वाक्यं_तुलय(listener->tls_संस्करणम्, "tlsv1.2")){
		SSL_CTX_समुच्चयः_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
	}अन्यथा यदि(!वाक्यं_तुलय(listener->tls_संस्करणम्, "tlsv1.1")){
		SSL_CTX_समुच्चयः_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1);
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: TLS v1.1 is insecure. It must only be used in the case "
				"where you have devices that cannot be upgraded to use a secure version of TLS. It is "
				"recommended to use as secure a set of ciphers as possible and that will restrict it to "
				"TLS v1.1 only, and to use a separate listener using TLS v1.2 and secure ciphers for your "
				"other devices.");
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Please be aware that support for TLS v1.1 will go away eventually "
				"and that you should plan now to migrate away from it.");
	}अन्यथा{
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unsupported tls_version \"%s\".", listener->tls_संस्करणम्);
		प्रत्यागम MOSQ_ERR_TLS;
	}

#यदि_परिभाषितम् SSL_OP_NO_COMPRESSION
	/* Disable compression */
	SSL_CTX_समुच्चयः_options(listener->ssl_ctx, SSL_OP_NO_COMPRESSION);
#यदि_अन्तः
#यदि_परिभाषितम् SSL_OP_CIPHER_SERVER_PREFERENCE
	/* Server chooses cipher */
	SSL_CTX_समुच्चयः_options(listener->ssl_ctx, SSL_OP_CIPHER_SERVER_PREFERENCE);
#यदि_अन्तः

#यदि_परिभाषितम् SSL_बहुलकम्_RELEASE_BUFFERS
	/* Use even less memory per SSL connection. */
	SSL_CTX_समुच्चयः_बहुलकम्(listener->ssl_ctx, SSL_बहुलकम्_RELEASE_BUFFERS);
#यदि_अन्तः

	SSL_CTX_समुच्चयः_dh_auto(listener->ssl_ctx, 1);

#यदि_परिभाषितम् SSL_OP_NO_RENEGOTIATION
	SSL_CTX_समुच्चयः_options(listener->ssl_ctx, SSL_OP_NO_RENEGOTIATION);
#यदि_अन्तः

	यदि(db.tls_keylog){
		log__printf(रिक्त, MOSQ_LOG_NOTICE, "TLS key logging to '%s' enabled for all listeners.",
				db.tls_keylog);
		log__printf(रिक्त, MOSQ_LOG_NOTICE, "TLS key logging is for DEBUGGING only.");

		SSL_CTX_समुच्चयः_keylog_callback(listener->ssl_ctx, tls_keylog_callback);
	}

	snprintf(buf, 256, "mosquitto-%d", listener->पत्तनम्);
	SSL_CTX_समुच्चयः_session_id_context(listener->ssl_ctx, (अचिह्नित वर्ण *)buf, (अचिह्नित पूर्णाङ्क)वाक्यदैर्घ्यम्(buf));

	यदि(listener->ciphers){
		rc = SSL_CTX_समुच्चयः_cipher_list(listener->ssl_ctx, listener->ciphers);
		यदि(rc == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			प्रत्यागम MOSQ_ERR_TLS;
		}
	}अन्यथा{
		rc = SSL_CTX_समुच्चयः_cipher_list(listener->ssl_ctx, "DEFAULT:!aNULL:!eNULL:!LOW:!EXPORT:!SSLv2:@STRENGTH");
		यदि(rc == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			प्रत्यागम MOSQ_ERR_TLS;
		}
	}
	यदि(listener->ciphers_tls13){
		rc = SSL_CTX_समुच्चयः_ciphersuites(listener->ssl_ctx, listener->ciphers_tls13);
		यदि(rc == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to set TLS 1.3 ciphersuites. Check cipher_tls13 list \"%s\".", listener->ciphers_tls13);
			प्रत्यागम MOSQ_ERR_TLS;
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}
#यदि_अन्तः


#यदि_परिभाषितम् WITH_TLS


स्थिर पूर्णाङ्क net_आरोपय_crl_file(संरचना mosquitto__listener *listener)
{
	X509_STORE *store;
	X509_LOOKUP *lookup;
	पूर्णाङ्क rc;

	store = SSL_CTX_get_cert_store(listener->ssl_ctx);
	यदि(!store){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to obtain TLS store.");
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		प्रत्यागम MOSQ_ERR_TLS;
	}
	lookup = X509_STORE_सङ्कलय_lookup(store, X509_LOOKUP_file());
	rc = X509_आरोपय_crl_file(lookup, listener->crlfile, X509_FILETYPE_PEM);
	यदि(rc < 1){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load certificate revocation file \"%s\". Check crlfile.", listener->crlfile);
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		net__print_ssl_error(रिक्त, रिक्त);
		प्रत्यागम MOSQ_ERR_TLS;
	}
	X509_STORE_समुच्चयः_flags(store, X509_V_FLAG_CRL_CHECK);

	प्रत्यागम MOSQ_ERR_SUCCESS;
}
#यदि_अन्तः


पूर्णाङ्क net_आरोपय_certificates(संरचना mosquitto__listener *listener)
{
#यदि_परिभाषितम् WITH_TLS
	पूर्णाङ्क rc;

	यदि(listener->require_certificate){
		SSL_CTX_समुच्चयः_verify(listener->ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, client_certificate_verify);
	}अन्यथा{
		SSL_CTX_समुच्चयः_verify(listener->ssl_ctx, SSL_VERIFY_NONE, client_certificate_verify);
	}
	rc = SSL_CTX_use_certificate_chain_file(listener->ssl_ctx, listener->certfile);
	यदि(rc != 1){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
		net__print_ssl_error(रिक्त, रिक्त);
		प्रत्यागम MOSQ_ERR_TLS;
	}
	यदि(listener->tls_engine == रिक्त || listener->tls_keyform == mosq_k_pem){
		rc = SSL_CTX_use_वैयक्तिकम्_कुञ्चिका_file(listener->ssl_ctx, listener->keyfile, SSL_FILETYPE_PEM);
		यदि(rc != 1){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			net__print_ssl_error(रिक्त, रिक्त);
			प्रत्यागम MOSQ_ERR_TLS;
		}
	}
	rc = SSL_CTX_check_वैयक्तिकम्_कुञ्चिका(listener->ssl_ctx);
	यदि(rc != 1){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Server certificate/key are inconsistent.");
		net__print_ssl_error(रिक्त, रिक्त);
		प्रत्यागम MOSQ_ERR_TLS;
	}
	/* Load CRLs if they exist. */
	यदि(listener->crlfile){
		rc = net_आरोपय_crl_file(listener);
		यदि(rc){
			प्रत्यागम rc;
		}
	}
#अन्यथा
	अप्रयुक्तम्(listener);
#यदि_अन्तः
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


#यदि defined(WITH_TLS) && !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000


स्थिर पूर्णाङ्क net_आरोपय_engine(संरचना mosquitto__listener *listener)
{
	ENGINE *engine = रिक्त;
	UI_विधानम् *ui_विधानम्;
	EVP_PKEY *pkey;

	यदि(!listener->tls_engine){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	engine = ENGINE_by_id(listener->tls_engine);
	यदि(!engine){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error loading %s engine\n", listener->tls_engine);
		net__print_ssl_error(रिक्त, रिक्त);
		प्रत्यागम MOSQ_ERR_TLS;
	}
	यदि(!ENGINE_init(engine)){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Failed engine initialisation\n");
		net__print_ssl_error(रिक्त, रिक्त);
		प्रत्यागम MOSQ_ERR_TLS;
	}
	ENGINE_समुच्चयः_default(engine, ENGINE_विधानम्_ALL);

	यदि(listener->tls_keyform == mosq_k_engine){
		ui_विधानम् = net_get_ui_विधानम्();
		यदि(listener->tls_engine_kpass_sha1){
			यदि(!ENGINE_ctrl_cmd(engine, ENGINE_SECRET_बहुलकम्, ENGINE_SECRET_बहुलकम्_SHA, रिक्त, रिक्त, 0)){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to set engine secret mode sha");
				net__print_ssl_error(रिक्त, रिक्त);
				प्रत्यागम MOSQ_ERR_TLS;
			}
			यदि(!ENGINE_ctrl_cmd(engine, ENGINE_PIN, 0, listener->tls_engine_kpass_sha1, रिक्त, 0)){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to set engine pin");
				net__print_ssl_error(रिक्त, रिक्त);
				प्रत्यागम MOSQ_ERR_TLS;
			}
			ui_विधानम् = रिक्त;
		}
		pkey = ENGINE_आरोपय_वैयक्तिकम्_कुञ्चिका(engine, listener->keyfile, ui_विधानम्, रिक्त);
		यदि(!pkey){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(रिक्त, रिक्त);
			प्रत्यागम MOSQ_ERR_TLS;
		}
		यदि(SSL_CTX_use_वैयक्तिकम्_कुञ्चिका(listener->ssl_ctx, pkey) <= 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to use engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(रिक्त, रिक्त);
			प्रत्यागम MOSQ_ERR_TLS;
		}
	}
	ENGINE_free(engine); /* release the structural reference from ENGINE_by_id() */

	प्रत्यागम MOSQ_ERR_SUCCESS;
}
#यदि_अन्तः


पूर्णाङ्क net_tls_आरोपय_verify(संरचना mosquitto__listener *listener)
{
#यदि_परिभाषितम् WITH_TLS
	पूर्णाङ्क rc;

#  यदि OPENSSL_संस्करणम्_सङ्ख्या < 0x30000000L
	यदि(listener->cafile || listener->capath){
		rc = SSL_CTX_आरोपय_verify_locations(listener->ssl_ctx, listener->cafile, listener->capath);
		यदि(rc == 0){
			यदि(listener->cafile && listener->capath){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\" and capath \"%s\".", listener->cafile, listener->capath);
			}अन्यथा यदि(listener->cafile){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			}अन्यथा{
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			}
		}
	}
#  अन्यथा
	राशिः_सम्बन्धि(X509_NAME) *ca_नामानि = रिक्त;
	यदि(listener->cafile || listener->capath){
		ca_नामानि = sk_X509_NAME_new_null();
	}

	यदि(listener->cafile){
		rc = SSL_CTX_आरोपय_verify_file(listener->ssl_ctx, listener->cafile);
		यदि(rc == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			net__print_ssl_error(रिक्त, रिक्त);
			प्रत्यागम MOSQ_ERR_TLS;
		}

		राशिः_सम्बन्धि(X509_NAME) *cert_नामानि = SSL_आरोपय_client_CA_file(listener->cafile);
		यदि(cert_नामानि){
			प्रति(पूर्णाङ्क i=0; i<sk_X509_NAME_अङ्कः(cert_नामानि); i++){
				sk_X509_NAME_प्रक्षिप(ca_नामानि, X509_NAME_dup(sk_X509_NAME_मानम्(cert_नामानि, i)));
			}
			sk_X509_NAME_निष्कासय_free(cert_नामानि, X509_NAME_free);
		}

	}
	यदि(listener->capath){
		rc = SSL_CTX_आरोपय_verify_dir(listener->ssl_ctx, listener->capath);
		यदि(rc == 0){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			net__print_ssl_error(रिक्त, रिक्त);
			sk_X509_NAME_निष्कासय_free(ca_नामानि, X509_NAME_free);
			प्रत्यागम MOSQ_ERR_TLS;
		}

		SSL_सङ्कलय_dir_cert_subjects_गन्तव्यम्_राशिः(ca_नामानि, listener->capath);
	}
	यदि(ca_नामानि){
		SSL_CTX_समुच्चयः_client_CA_list(listener->ssl_ctx, ca_नामानि);
	}
#  endif

#  यदि !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000
	यदि(net_आरोपय_engine(listener)){
		प्रत्यागम MOSQ_ERR_TLS;
	}
#  endif
#यदि_अन्तः
	प्रत्यागम net_आरोपय_certificates(listener);
}


#यदि !defined(WIN32) && !defined(_AIX)


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

	यदि(getifaddrs(&ifaddr) < 0){
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		प्रत्यागम MOSQ_ERR_ERRNO;
	}

	प्रति(संरचना ifaddrs *ifa=ifaddr; ifa!=रिक्त; ifa=ifa->ifa_अग्र){
		यदि(ifa->ifa_addr == रिक्त){
			अनुवर्त;
		}

		यदि(!strcasecmp(listener->bind_interface, ifa->ifa_name)){
			have_interface = true;

			यदि(ifa->ifa_addr->sa_family == rp->ai_addr->sa_family){
				यदि(rp->ai_addr->sa_family == AF_INET){
					यदि(listener->host &&
							memcmp(&((संरचना sockaddr_in *)rp->ai_addr)->sin_addr,
							&((संरचना sockaddr_in *)ifa->ifa_addr)->sin_addr,
							परिमाण(संरचना in_addr))){

						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						प्रत्यागम MOSQ_ERR_INVAL;
					}अन्यथा{
						स्मृतिं_प्रतिलिख(&((संरचना sockaddr_in *)rp->ai_addr)->sin_addr,
								&((संरचना sockaddr_in *)ifa->ifa_addr)->sin_addr,
								परिमाण(संरचना in_addr));

						freeifaddrs(ifaddr);
						प्रत्यागम MOSQ_ERR_SUCCESS;
					}
				}अन्यथा यदि(rp->ai_addr->sa_family == AF_INET6){
					यदि(listener->host &&
							memcmp(&((संरचना sockaddr_in6 *)rp->ai_addr)->sin6_addr,
							&((संरचना sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
							परिमाण(संरचना in6_addr))){

						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						प्रत्यागम MOSQ_ERR_INVAL;
					}अन्यथा{
						स्मृतिं_प्रतिलिख(&((संरचना sockaddr_in6 *)rp->ai_addr)->sin6_addr,
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
	यदि(have_interface){
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Interface %s does not support %s configuration.",
				listener->bind_interface, rp->ai_addr->sa_family == AF_INET ? "IPv4" : "IPv6");
		प्रत्यागम MOSQ_ERR_NOT_SUPPORTED;
	}अन्यथा{
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Interface %s does not exist.", listener->bind_interface);
		प्रत्यागम MOSQ_ERR_NOT_प्राप्तम्;
	}
}
#यदि_अन्तः


स्थिर पूर्णाङ्क net__socket_listen_tcp(संरचना mosquitto__listener *listener)
{
	mosq_sock_t sock = INVALID_SOCKET;
	संरचना addrinfo hints;
	संरचना addrinfo *ainfo, *rp;
	वर्ण service[10];
	पूर्णाङ्क rc;
	पूर्णाङ्क ss_opt = 1;
#यदि_अपरिभाषितम् WIN32
	bool interface_bound = false;
#यदि_अन्तः

	यदि(!listener){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	snprintf(service, 10, "%d", listener->पत्तनम्);
	स्मृतिं_पूरय(&hints, 0, परिमाण(संरचना addrinfo));
	यदि(listener->socket_domain){
		hints.ai_family = listener->socket_domain;
	}अन्यथा{
		hints.ai_family = AF_UNSPEC;
	}
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	rc = getaddrinfo(listener->host, service, &hints, &ainfo);
	यदि(rc){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating listener: %s.", gai_strerror(rc));
		प्रत्यागम INVALID_SOCKET;
	}

	listener->sock_गणनम् = 0;
	listener->socks = रिक्त;

	प्रति(rp = ainfo; rp; rp = rp->ai_अग्र){
		यदि(rp->ai_family == AF_INET){
			log__printf(रिक्त, MOSQ_LOG_INFO, "Opening ipv4 listen socket on port %d.", ntohs(((संरचना sockaddr_in *)rp->ai_addr)->sin_port));
		}अन्यथा यदि(rp->ai_family == AF_INET6){
			log__printf(रिक्त, MOSQ_LOG_INFO, "Opening ipv6 listen socket on port %d.", ntohs(((संरचना sockaddr_in6 *)rp->ai_addr)->sin6_port));
		}अन्यथा{
			अनुवर्त;
		}

		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		यदि(sock == INVALID_SOCKET){
			net__print_error(MOSQ_LOG_WARNING, "Warning: %s");
			अनुवर्त;
		}
		listener->sock_गणनम्++;
		listener->socks = mosquitto_realloc(listener->socks, परिमाण(mosq_sock_t)*(size_t)listener->sock_गणनम्);
		यदि(!listener->socks){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
		listener->socks[listener->sock_गणनम्-1] = sock;

#यदि_अपरिभाषितम् WIN32
		ss_opt = 1;
		/* Unimportant if this fails */
		(शून्य)setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (ध्रुव शून्य *)&ss_opt, परिमाण(ss_opt));
#यदि_अन्तः
#यदि_परिभाषितम् IPV6_V6ONLY
		ss_opt = 1;
		(शून्य)setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (ध्रुव शून्य *)&ss_opt, परिमाण(ss_opt));
#यदि_अन्तः

		यदि(net__socket_nonblock(&sock)){
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			प्रत्यागम 1;
		}

#यदि !defined(WIN32) && !defined(_AIX)
		यदि(listener->bind_interface){
			/* It might be possible that an interface does not support all relevant sa_families.
			 * We should successfully find at least one. */
			rc = net__bind_interface(listener, rp);
			यदि(rc){
				COMPAT_CLOSE(sock);
				listener->sock_गणनम्--;
				यदि(rc == MOSQ_ERR_NOT_प्राप्तम् || rc == MOSQ_ERR_INVAL){
					freeaddrinfo(ainfo);
					प्रत्यागम rc;
				}अन्यथा{
					अनुवर्त;
				}
			}
			interface_bound = true;
		}
#यदि_अन्तः

		यदि(bind(sock, rp->ai_addr, rp->ai_addrlen) == -1){
#यदि defined(__linux__)
			यदि(errno == EACCES){
				log__printf(रिक्त, MOSQ_LOG_ERR, "If you are trying to bind to a privileged port (<1024), try using setcap and do not start the broker as root:");
				log__printf(रिक्त, MOSQ_LOG_ERR, "    sudo setcap 'CAP_NET_BIND_SERVICE=+ep /usr/sbin/mosquitto'");
			}
#यदि_अन्तः
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			COMPAT_CLOSE(sock);
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			प्रत्यागम 1;
		}

		यदि(listen(sock, 100) == -1){
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			mosquitto_FREE(listener->socks);
			प्रत्यागम 1;
		}
	}
	freeaddrinfo(ainfo);

#यदि_अपरिभाषितम् WIN32
	यदि(listener->bind_interface && !interface_bound){
		mosquitto_FREE(listener->socks);
		प्रत्यागम 1;
	}
#यदि_अन्तः

	प्रत्यागम 0;
}


#यदि_परिभाषितम् WITH_UNIX_SOCKETS


स्थिर पूर्णाङ्क net__socket_listen_unix(संरचना mosquitto__listener *listener)
{
	संरचना sockaddr_un addr;
	mosq_sock_t sock = INVALID_SOCKET;
	पूर्णाङ्क rc;
#यदि_अपरिभाषितम् WIN32
	mode_t old_आवरणम्;
#यदि_अन्तः

	यदि(listener->unix_socket_मार्गः == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि(वाक्यदैर्घ्यम्(listener->unix_socket_मार्गः) > परिमाण(addr.sun_मार्गः)-1){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Path to unix socket is too long \"%s\".", listener->unix_socket_मार्गः);
		प्रत्यागम MOSQ_ERR_INVAL;
	}

#यदि_परिभाषितम् WIN32
	DeleteFile(listener->unix_socket_मार्गः);
#अन्यथा
	unlink(listener->unix_socket_मार्गः);
#यदि_अन्तः
	log__printf(रिक्त, MOSQ_LOG_INFO, "Opening unix listen socket on path %s.", listener->unix_socket_मार्गः);
	स्मृतिं_पूरय(&addr, 0, परिमाण(संरचना sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_मार्गः, listener->unix_socket_मार्गः, परिमाण(addr.sun_मार्गः)-1);

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	यदि(sock == INVALID_SOCKET){
		net__print_error(MOSQ_LOG_ERR, "Error creating unix socket: %s");
		प्रत्यागम 1;
	}
	listener->sock_गणनम्++;
	listener->socks = mosquitto_realloc(listener->socks, परिमाण(mosq_sock_t)*(size_t)listener->sock_गणनम्);
	यदि(!listener->socks){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		COMPAT_CLOSE(sock);
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	listener->socks[listener->sock_गणनम्-1] = sock;


#यदि_अपरिभाषितम् WIN32
	old_आवरणम् = umask(0007);
#यदि_अन्तः
	rc = bind(sock, (संरचना sockaddr *)&addr, परिमाण(संरचना sockaddr_un));
#यदि_अपरिभाषितम् WIN32
	umask(old_आवरणम्);
#यदि_अन्तः

	यदि(rc == -1){
		net__print_error(MOSQ_LOG_ERR, "Error binding unix socket: %s");
		प्रत्यागम 1;
	}

	यदि(listen(sock, 10) == -1){
		net__print_error(MOSQ_LOG_ERR, "Error listening to unix socket: %s");
		प्रत्यागम 1;
	}

	यदि(net__socket_nonblock(&sock)){
		प्रत्यागम 1;
	}

	प्रत्यागम 0;
}
#यदि_अन्तः


/* Creates a socket and listens on port 'port'.
 * Returns 1 on failure
 * Returns 0 on success.
 */
पूर्णाङ्क net__socket_listen(संरचना mosquitto__listener *listener)
{
	पूर्णाङ्क rc;

	यदि(!listener){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

#यदि_परिभाषितम् WITH_UNIX_SOCKETS
	यदि(listener->पत्तनम् == 0 && listener->unix_socket_मार्गः != रिक्त){
		rc = net__socket_listen_unix(listener);
	}अन्यथा
#यदि_अन्तः
	{
		rc = net__socket_listen_tcp(listener);
	}
	यदि(rc){
		प्रत्यागम rc;
	}

	/* We need to have at least one working socket. */
	यदि(listener->sock_गणनम् > 0){
#यदि_परिभाषितम् WITH_TLS
		यदि(listener->certfile && listener->keyfile){
			यदि(net__tls_server_ctx(listener)){
				प्रत्यागम 1;
			}

			यदि(net_tls_आरोपय_verify(listener)){
				प्रत्यागम 1;
			}
		}
#  ifdef FINAL_WITH_TLS_PSK
		यदि(listener->psk_hint){
			यदि(listener->certfile == रिक्त || listener->keyfile == रिक्त){
				यदि(net__tls_server_ctx(listener)){
					प्रत्यागम 1;
				}
			}
			SSL_CTX_समुच्चयः_psk_server_callback(listener->ssl_ctx, psk_server_callback);
			यदि(listener->psk_hint){
				rc = SSL_CTX_use_psk_तटस्थः_hint(listener->ssl_ctx, listener->psk_hint);
				यदि(rc == 0){
					log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to set TLS PSK hint.");
					net__print_ssl_error(रिक्त, रिक्त);
					प्रत्यागम 1;
				}
			}
		}
#  endif /* FINAL_WITH_TLS_PSK */
		यदि(tls_ex_सूचकांक_context == -1){
			tls_ex_सूचकांक_context = SSL_get_ex_new_सूचकांक(0, "client context", रिक्त, रिक्त, रिक्त);
		}
		यदि(tls_ex_सूचकांक_listener == -1){
			tls_ex_सूचकांक_listener = SSL_get_ex_new_सूचकांक(0, "listener", रिक्त, रिक्त, रिक्त);
		}
#यदि_अन्तः /* WITH_TLS */
		प्रत्यागम 0;
	}अन्यथा{
		प्रत्यागम 1;
	}
}


पूर्णाङ्क net_socket_get_स्थानाङ्कः(mosq_sock_t sock, वर्ण *buf, size_t len, uint16_t *remote_पत्तनम्)
{
	संरचना sockaddr_storage addr;
	socklen_t addrlen;

	स्मृतिं_पूरय(&addr, 0, परिमाण(संरचना sockaddr_storage));
	addrlen = परिमाण(addr);
	यदि(!getpeername(sock, (संरचना sockaddr *)&addr, &addrlen)){
		यदि(addr.ss_family == AF_INET){
			यदि(remote_पत्तनम्){
				*remote_पत्तनम् = ntohs(((संरचना sockaddr_in *)&addr)->sin_port);
			}
			यदि(inet_ntop(AF_INET, &((संरचना sockaddr_in *)&addr)->sin_addr.s_addr, buf, (socklen_t)len)){
				प्रत्यागम 0;
			}
		}अन्यथा यदि(addr.ss_family == AF_INET6){
			यदि(remote_पत्तनम्){
				*remote_पत्तनम् = ntohs(((संरचना sockaddr_in6 *)&addr)->sin6_port);
			}
			यदि(inet_ntop(AF_INET6, &((संरचना sockaddr_in6 *)&addr)->sin6_addr.s6_addr, buf, (socklen_t)len)){
				प्रत्यागम 0;
			}
#यदि_परिभाषितम् WITH_UNIX_SOCKETS
		}अन्यथा यदि(addr.ss_family == AF_UNIX){
			संरचना sockaddr_un un;
			addrlen = परिमाण(संरचना sockaddr_un);
			यदि(!getsockname(sock, (संरचना sockaddr *)&un, &addrlen)){
				snprintf(buf, len, "%s", un.sun_मार्गः);
			}अन्यथा{
				snprintf(buf, len, "unix-socket");
			}
			प्रत्यागम 0;
#यदि_अन्तः
		}
	}
	प्रत्यागम 1;
}
