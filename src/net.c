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

#ଅନ୍ତର୍ଭାଵ "config.h"

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
#ଅନ୍ୟଥା
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#ଅନ୍ତର୍ଭାଵ <errno.h>
#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ifdef WITH_WRAP
#  include <tcpd.h>
#endif

#ifdef HAVE_NETINET_IN_H
#  include <netinet/in.h>
#endif

#ଯଦି defined(WITH_UNIX_SOCKETS) || defined(WITH_TLS)
#  include "sys/stat.h"
#endif

#ifdef WITH_UNIX_SOCKETS
#  ifdef WIN32
#    include <afunix.h>
#  ଅନ୍ୟଥା
#    include <sys/un.h>
#  endif
#endif

#ifdef __QNX__
#  include <net/netbyte.h>
#endif

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"
#ଅନ୍ତର୍ଭାଵ "net_mosq.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"

#ifdef WITH_TLS
#  include "tls_mosq.h"
#  include <openssl/err.h>
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tls_ex_index_context = -1;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tls_ex_index_listener = -1;
#endif

#ଅନ୍ତର୍ଭାଵ "sys_tree.h"

/* For EMFILE handling */
ସ୍ଥିର mosq_sock_t spare_sock = INVALID_SOCKET;


ଶୂନ୍ୟ net__broker_init(ଶୂନ୍ୟ)
{
	spare_sock = socket(AF_INET, SOCK_STREAM, 0);
	net__init();
#ifdef WITH_TLS
	net__init_tls();
#endif
}


ଶୂନ୍ୟ net__broker_cleanup(ଶୂନ୍ୟ)
{
	ଯଦି(spare_sock != INVALID_SOCKET){
		COMPAT_CLOSE(spare_sock);
		spare_sock = INVALID_SOCKET;
	}
	net__cleanup();
}


ସ୍ଥିର ଶୂନ୍ୟ net__print_error(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା log, ଅଚଳ ଅକ୍ଷର *format_str)
{
	ଅକ୍ଷର *buf;

#ifdef WIN32
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			ଖାଲି, WSAGetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, ଖାଲି);

	log__printf(ଖାଲି, log, format_str, buf);
	LocalFree(buf);
#ଅନ୍ୟଥା
	buf = strerror(errno);
	log__printf(ଖାଲି, log, format_str, buf);
#endif
}


ସଂରଚନା mosquitto *net__socket_accept(ସଂରଚନା mosquitto__listener_sock *listensock)
{
	mosq_sock_t new_sock = INVALID_SOCKET;
	ସଂରଚନା mosquitto *new_context;
#ifdef WITH_TLS
	BIO *bio;
#endif
#ifdef WITH_WRAP
	ସଂରଚନା request_info wrap_req;
	ଅକ୍ଷର address[1024];
#endif

	new_sock = accept(listensock->sock, ଖାଲି, 0);
	ଯଦି(new_sock == INVALID_SOCKET){
#ifdef WIN32
		WINDOWS_SET_ERRNO();
		ଯଦି(errno == WSAEMFILE){
#ଅନ୍ୟଥା
		ଯଦି(errno == EMFILE || errno == ENFILE){
#endif
			/* Close the spare socket, which means we should be able to accept
			 * this connection. Accept it, then close it immediately and create
			 * a new spare_sock. This prevents the situation of ever properly
			 * running out of sockets.
			 * It would be nice to send a "server not available" connack here,
			 * but there are lots of reasons why this would be tricky (TLS
			 * being the big one). */
			COMPAT_CLOSE(spare_sock);
			new_sock = accept(listensock->sock, ଖାଲି, 0);
			ଯଦି(new_sock != INVALID_SOCKET){
				COMPAT_CLOSE(new_sock);
			}
			spare_sock = socket(AF_INET, SOCK_STREAM, 0);
			log__printf(ଖାଲି, MOSQ_LOG_WARNING,
					"Unable to accept new connection, system socket count has been exceeded. Try increasing \"ulimit -n\" or equivalent.");
		}
		ଫେରନ୍ତୁ ଖାଲି;
	}

	metrics__int_inc(mosq_counter_socket_connections, 1);

	ଯଦି(net__socket_nonblock(&new_sock)){
		ଫେରନ୍ତୁ ଖାଲି;
	}

#ifdef WITH_WRAP
	/* Use tcpd / libwrap to determine whether a connection is allowed. */
	request_init(&wrap_req, RQ_FILE, new_sock, RQ_DAEMON, "mosquitto", 0);
	fromhost(&wrap_req);
	ଯଦି(!hosts_access(&wrap_req)){
		/* Access is denied */
		ଯଦି(db.config->connection_messages == true){
			ଯଦି(!net__socket_get_address(new_sock, address, 1024, ଖାଲି)){
				log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client connection from %s denied access by tcpd.", address);
			}
		}
		COMPAT_CLOSE(new_sock);
		ଫେରନ୍ତୁ ଖାଲି;
	}
#endif

	ଯଦି(db.config->set_tcp_nodelay && listensock->listener->port){
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା flag = 1;
		ଯଦି(setsockopt(new_sock, IPPROTO_TCP, TCP_NODELAY, (ଅଚଳ ଶୂନ୍ୟ *)&flag, ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)) != 0){
			log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Unable to set TCP_NODELAY.");
		}
	}

	new_context = context__init();
	ଯଦି(!new_context){
		COMPAT_CLOSE(new_sock);
		ଫେରନ୍ତୁ ଖାଲି;
	}
	new_context->listener = listensock->listener;
	ଯଦି(!new_context->listener){
		context__cleanup(new_context, true);
		ଫେରନ୍ତୁ ଖାଲି;
	}
	new_context->listener->client_ଗଣନା++;

	ଯଦି(new_context->listener->enable_proxy_protocol){
		ଯଦି(context__init_sock(new_context, new_sock, false) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			ଫେରନ୍ତୁ ଖାଲି;
		}
		ଯଦି(new_context->listener->enable_proxy_protocol == 2){
			new_context->transport = mosq_t_proxy_v2;
		}ଅନ୍ୟଥା{
			new_context->transport = mosq_t_proxy_v1;
		}
		new_context->proxy.cmd = -1;
	}ଅନ୍ୟଥା{
		ଯଦି(context__init_sock(new_context, new_sock, true) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			ଫେରନ୍ତୁ ଖାଲି;
		}
		ବାଛ(new_context->listener->protocol){
			ସ୍ଥିତି mp_mqtt:
				new_context->transport = mosq_t_tcp;
				ଭାଙ୍ଗ;
#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			ସ୍ଥିତି mp_websockets:
				ଯଦି(http__context_init(new_context)){
					context__cleanup(new_context, true);
					ଫେରନ୍ତୁ ଖାଲି;
				}
				ଭାଙ୍ଗ;
#endif
			ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
				context__cleanup(new_context, true);
				ଫେରନ୍ତୁ ଖାଲି;
		}
	}

	ଯଦି((new_context->listener->max_connections > 0 && new_context->listener->client_ଗଣନା > new_context->listener->max_connections)
			|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)db.config->global_max_connections)){

		ଯଦି(db.config->connection_messages == true){
			log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", new_context->address);
		}
		context__cleanup(new_context, true);
		ଫେରନ୍ତୁ ଖାଲି;
	}

#ifdef WITH_TLS
	/* TLS init */
	ଯଦି(new_context->listener->ssl_ctx){
		new_context->ssl = SSL_new(new_context->listener->ssl_ctx);
		ଯଦି(!new_context->ssl){
			context__cleanup(new_context, true);
			ଫେରନ୍ତୁ ଖାଲି;
		}
		ଯଦି(!SSL_set_ex_data(new_context->ssl, tls_ex_index_context, new_context)
				|| !SSL_set_ex_data(new_context->ssl, tls_ex_index_listener, new_context->listener)){
			context__cleanup(new_context, true);
			ଫେରନ୍ତୁ ଖାଲି;
		}
		new_context->want_write = true;
		bio = BIO_new_socket(new_sock, BIO_NOCLOSE);
		SSL_set_bio(new_context->ssl, bio, bio);
		ERR_clear_error();
		SSL_set_accept_state(new_context->ssl);
	}
#endif

	ଯଦି(db.config->connection_messages == true
			&& !new_context->listener->enable_proxy_protocol){

		log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "New connection from %s:%d on port %d.",
				new_context->address, new_context->remote_port, new_context->listener->port);
	}

	mux__new(new_context);
	keepalive__add(new_context);

	ଫେରନ୍ତୁ new_context;
}

#ifdef WITH_TLS


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା client_certificate_verify(ପୂର୍ଣ୍ଣସଂଖ୍ୟା preverify_ok, X509_STORE_CTX *ctx)
{
	/* Preverify should check expiry, revocation. */
	ଯଦି(preverify_ok == 0){
		SSL *ssl;
		ssl = X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx());
		ଯଦି(ssl){
			ସଂରଚନା mosquitto__listener *listener;
			listener = SSL_get_ex_data(ssl, tls_ex_index_listener);

			ଯଦି(listener && listener->disable_client_cert_date_checks){
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା err = X509_STORE_CTX_get_error(ctx);
				ଯଦି(err == X509_V_ERR_CERT_NOT_YET_VALID || err == X509_V_ERR_CERT_HAS_EXPIRED){
					preverify_ok = 1;
				}
			}
		}
	}
	ଫେରନ୍ତୁ preverify_ok;
}
#endif

#ifdef FINAL_WITH_TLS_PSK


ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା psk_server_callback(SSL *ssl, ଅଚଳ ଅକ୍ଷର *identity, unsigned ଅକ୍ଷର *psk, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_psk_len)
{
	ସଂରଚନା mosquitto *context;
	ସଂରଚନା mosquitto__listener *listener;
	ଅକ୍ଷର *psk_ଚାବି = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;
	ଅଚଳ ଅକ୍ଷର *psk_hint;

	ଯଦି(!identity){
		ଫେରନ୍ତୁ 0;
	}

	context = SSL_get_ex_data(ssl, tls_ex_index_context);
	ଯଦି(!context){
		ଫେରନ୍ତୁ 0;
	}

	listener = SSL_get_ex_data(ssl, tls_ex_index_listener);
	ଯଦି(!listener){
		ଫେରନ୍ତୁ 0;
	}

	psk_hint = listener->psk_hint;

	/* The hex to BN conversion results in the length halving, so we can pass
	 * max_psk_len*2 as the max hex key here. */
	psk_ଚାବି = mosquitto_calloc(1, (size_t)max_psk_len*2 + 1);
	ଯଦି(!psk_ଚାବି){
		ଫେରନ୍ତୁ 0;
	}

	ଯଦି(mosquitto_psk_ଚାବି_get(context, psk_hint, identity, psk_ଚାବି, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)max_psk_len*2) != MOSQ_ERR_SUCCESS){
		mosquitto_FREE(psk_ଚାବି);
		ଫେରନ୍ତୁ 0;
	}

	len = mosquitto__hex2bin(psk_ଚାବି, psk, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)max_psk_len);
	ଯଦି(len < 0){
		mosquitto_FREE(psk_ଚାବି);
		ଫେରନ୍ତୁ 0;
	}

	ଯଦି(listener->use_identity_as_username){
		ଯଦି(mosquitto_validate_utf8(identity, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)strlen(identity))){
			mosquitto_free(psk_ଚାବି);
			ଫେରନ୍ତୁ 0;
		}
		context->username = mosquitto_strdup(identity);
		ଯଦି(!context->username){
			mosquitto_FREE(psk_ଚାବି);
			ଫେରନ୍ତୁ 0;
		}
	}

	mosquitto_FREE(psk_ଚାବି);
	ଫେରନ୍ତୁ (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len;
}
#endif

#ifdef WITH_TLS


ସ୍ଥିର ଶୂନ୍ୟ tls_keylog_callback(ଅଚଳ SSL *ssl, ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି)
{
	UNUSED(ssl);

	ଯଦି(db.tls_keylog){
		FILE *fptr;
		fptr = mosquitto_fopen(db.tls_keylog, "at", true);
		ଯଦି(fptr){
#ifndef WIN32
			/* Until mosquitto_fopen enforces file permissions on all files,
			 * enforce it here. We can enforce it here, because it isn't a
			 * change of behaviour. */
			ସଂରଚନା stat statbuf;
			ଯଦି(fstat(fileno(fptr), &statbuf) < 0
					|| statbuf.st_mode & (S_IRWXG | S_IRWXO)){

				fclose(fptr);
				ଫେରନ୍ତୁ;
			}
#endif
			fprintf(fptr, "%s\n", ଧାଡ଼ି);
			fclose(fptr);
		}ଅନ୍ୟଥା{
#ifndef WIN32
			ଯଦି(errno == ELOOP){
				log__printf(ଖାଲି, MOSQ_LOG_INFO, "Error: keylog file must not be a symbolic link");
			}ଅନ୍ୟଥା
#endif
			{
				log__printf(ଖାଲି, MOSQ_LOG_INFO, "Error: Unable to open keylog file: %s", strerror(errno));
			}
		}
	}
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା net__tls_server_ctx(ସଂରଚନା mosquitto__listener *listener)
{
	ଅକ୍ଷର buf[256];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି(tls_ex_index_context == -1){
		tls_ex_index_context = SSL_get_ex_new_index(0, "client context", ଖାଲି, ଖାଲି, ଖାଲି);
	}
	ଯଦି(tls_ex_index_listener == -1){
		tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", ଖାଲି, ଖାଲି, ଖାଲି);
	}

	ଯଦି(listener->ssl_ctx){
		SSL_CTX_free(listener->ssl_ctx);
		listener->ssl_ctx = ଖାଲି;
	}

	listener->ssl_ctx = SSL_CTX_new(TLS_server_method());

	ଯଦି(!listener->ssl_ctx){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to create TLS context.");
		ଫେରନ୍ତୁ MOSQ_ERR_TLS;
	}

#ifdef SSL_OP_NO_TLSv1_3
	ଯଦି(db.config->per_listener_settings){
		ଯଦି(listener->security_options->psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}ଅନ୍ୟଥା{
		ଯଦି(db.config->security_options.psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}
#endif

	ଯଦି(listener->tls_version == ଖାଲି){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
#ifdef SSL_OP_NO_TLSv1_3
	}ଅନ୍ୟଥା ଯଦି(!strcmp(listener->tls_version, "tlsv1.3")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2);
#endif
	}ଅନ୍ୟଥା ଯଦି(!strcmp(listener->tls_version, "tlsv1.2")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
	}ଅନ୍ୟଥା ଯଦି(!strcmp(listener->tls_version, "tlsv1.1")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1);
		log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: TLS v1.1 is insecure. It must only be used in the case "
				"where you have devices that cannot be upgraded to use a secure version of TLS. It is "
				"recommended to use as secure a set of ciphers as possible and that will restrict it to "
				"TLS v1.1 only, and to use a separate listener using TLS v1.2 and secure ciphers for your "
				"other devices.");
		log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Please be aware that support for TLS v1.1 will go away eventually "
				"and that you should plan now to migrate away from it.");
	}ଅନ୍ୟଥା{
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unsupported tls_version \"%s\".", listener->tls_version);
		ଫେରନ୍ତୁ MOSQ_ERR_TLS;
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

	ଯଦି(db.tls_keylog){
		log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "TLS key logging to '%s' enabled for all listeners.",
				db.tls_keylog);
		log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "TLS key logging is for DEBUGGING only.");

		SSL_CTX_set_keylog_callback(listener->ssl_ctx, tls_keylog_callback);
	}

	snprintf(buf, 256, "mosquitto-%d", listener->port);
	SSL_CTX_set_session_id_context(listener->ssl_ctx, (unsigned ଅକ୍ଷର *)buf, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)strlen(buf));

	ଯଦି(listener->ciphers){
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, listener->ciphers);
		ଯଦି(rc == 0){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			ଫେରନ୍ତୁ MOSQ_ERR_TLS;
		}
	}ଅନ୍ୟଥା{
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, "DEFAULT:!aNULL:!eNULL:!LOW:!EXPORT:!SSLv2:@STRENGTH");
		ଯଦି(rc == 0){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			ଫେରନ୍ତୁ MOSQ_ERR_TLS;
		}
	}
	ଯଦି(listener->ciphers_tls13){
		rc = SSL_CTX_set_ciphersuites(listener->ssl_ctx, listener->ciphers_tls13);
		ଯଦି(rc == 0){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to set TLS 1.3 ciphersuites. Check cipher_tls13 list \"%s\".", listener->ciphers_tls13);
			ଫେରନ୍ତୁ MOSQ_ERR_TLS;
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}
#endif


#ifdef WITH_TLS


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା net__load_crl_file(ସଂରଚନା mosquitto__listener *listener)
{
	X509_STORE *store;
	X509_LOOKUP *lookup;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	store = SSL_CTX_get_cert_store(listener->ssl_ctx);
	ଯଦି(!store){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to obtain TLS store.");
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		ଫେରନ୍ତୁ MOSQ_ERR_TLS;
	}
	lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
	rc = X509_load_crl_file(lookup, listener->crlfile, X509_FILETYPE_PEM);
	ଯଦି(rc < 1){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to load certificate revocation file \"%s\". Check crlfile.", listener->crlfile);
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		net__print_ssl_error(ଖାଲି, ଖାଲି);
		ଫେରନ୍ତୁ MOSQ_ERR_TLS;
	}
	X509_STORE_set_flags(store, X509_V_FLAG_CRL_CHECK);

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}
#endif


ପୂର୍ଣ୍ଣସଂଖ୍ୟା net__load_certificates(ସଂରଚନା mosquitto__listener *listener)
{
#ifdef WITH_TLS
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି(listener->require_certificate){
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, client_certificate_verify);
	}ଅନ୍ୟଥା{
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_NONE, client_certificate_verify);
	}
	rc = SSL_CTX_use_certificate_chain_file(listener->ssl_ctx, listener->certfile);
	ଯଦି(rc != 1){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
		net__print_ssl_error(ଖାଲି, ଖାଲି);
		ଫେରନ୍ତୁ MOSQ_ERR_TLS;
	}
	ଯଦି(listener->tls_engine == ଖାଲି || listener->tls_keyform == mosq_k_pem){
		rc = SSL_CTX_use_Private_ଚାବି_file(listener->ssl_ctx, listener->keyfile, SSL_FILETYPE_PEM);
		ଯଦି(rc != 1){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			net__print_ssl_error(ଖାଲି, ଖାଲି);
			ଫେରନ୍ତୁ MOSQ_ERR_TLS;
		}
	}
	rc = SSL_CTX_check_private_ଚାବି(listener->ssl_ctx);
	ଯଦି(rc != 1){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Server certificate/key are inconsistent.");
		net__print_ssl_error(ଖାଲି, ଖାଲି);
		ଫେରନ୍ତୁ MOSQ_ERR_TLS;
	}
	/* Load CRLs if they exist. */
	ଯଦି(listener->crlfile){
		rc = net__load_crl_file(listener);
		ଯଦି(rc){
			ଫେରନ୍ତୁ rc;
		}
	}
#ଅନ୍ୟଥା
	UNUSED(listener);
#endif
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


#ଯଦି defined(WITH_TLS) && !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା net__load_engine(ସଂରଚନା mosquitto__listener *listener)
{
	ENGINE *engine = ଖାଲି;
	UI_METHOD *ui_method;
	EVP_PKEY *pkey;

	ଯଦି(!listener->tls_engine){
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

	engine = ENGINE_by_id(listener->tls_engine);
	ଯଦି(!engine){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error loading %s engine\n", listener->tls_engine);
		net__print_ssl_error(ଖାଲି, ଖାଲି);
		ଫେରନ୍ତୁ MOSQ_ERR_TLS;
	}
	ଯଦି(!ENGINE_init(engine)){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Failed engine initialisation\n");
		net__print_ssl_error(ଖାଲି, ଖାଲି);
		ଫେରନ୍ତୁ MOSQ_ERR_TLS;
	}
	ENGINE_set_default(engine, ENGINE_METHOD_ALL);

	ଯଦି(listener->tls_keyform == mosq_k_engine){
		ui_method = net__get_ui_method();
		ଯଦି(listener->tls_engine_kpass_sha1){
			ଯଦି(!ENGINE_ctrl_cmd(engine, ENGINE_SECRET_MODE, ENGINE_SECRET_MODE_SHA, ଖାଲି, ଖାଲି, 0)){
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to set engine secret mode sha");
				net__print_ssl_error(ଖାଲି, ଖାଲି);
				ଫେରନ୍ତୁ MOSQ_ERR_TLS;
			}
			ଯଦି(!ENGINE_ctrl_cmd(engine, ENGINE_PIN, 0, listener->tls_engine_kpass_sha1, ଖାଲି, 0)){
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to set engine pin");
				net__print_ssl_error(ଖାଲି, ଖାଲି);
				ଫେରନ୍ତୁ MOSQ_ERR_TLS;
			}
			ui_method = ଖାଲି;
		}
		pkey = ENGINE_load_private_ଚାବି(engine, listener->keyfile, ui_method, ଖାଲି);
		ଯଦି(!pkey){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to load engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(ଖାଲି, ଖାଲି);
			ଫେରନ୍ତୁ MOSQ_ERR_TLS;
		}
		ଯଦି(SSL_CTX_use_Private_ଚାବି(listener->ssl_ctx, pkey) <= 0){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to use engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(ଖାଲି, ଖାଲି);
			ଫେରନ୍ତୁ MOSQ_ERR_TLS;
		}
	}
	ENGINE_free(engine); /* release the structural reference from ENGINE_by_id() */

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}
#endif


ପୂର୍ଣ୍ଣସଂଖ୍ୟା net__tls_load_verify(ସଂରଚନା mosquitto__listener *listener)
{
#ifdef WITH_TLS
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

#  ଯଦି OPENSSL_VERSION_ସଂଖ୍ୟା < 0x30000000L
	ଯଦି(listener->cafile || listener->capath){
		rc = SSL_CTX_load_verify_locations(listener->ssl_ctx, listener->cafile, listener->capath);
		ଯଦି(rc == 0){
			ଯଦି(listener->cafile && listener->capath){
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\" and capath \"%s\".", listener->cafile, listener->capath);
			}ଅନ୍ୟଥା ଯଦି(listener->cafile){
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			}ଅନ୍ୟଥା{
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			}
		}
	}
#  ଅନ୍ୟଥା
	ଗଦା_OF(X509_NAME) *ca_names = ଖାଲି;
	ଯଦି(listener->cafile || listener->capath){
		ca_names = sk_X509_NAME_new_null();
	}

	ଯଦି(listener->cafile){
		rc = SSL_CTX_load_verify_file(listener->ssl_ctx, listener->cafile);
		ଯଦି(rc == 0){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			net__print_ssl_error(ଖାଲି, ଖାଲି);
			ଫେରନ୍ତୁ MOSQ_ERR_TLS;
		}

		ଗଦା_OF(X509_NAME) *cert_names = SSL_load_client_CA_file(listener->cafile);
		ଯଦି(cert_names){
			ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<sk_X509_NAME_ଅଙ୍କ(cert_names); i++){
				sk_X509_NAME_ଠେଲ(ca_names, X509_NAME_dup(sk_X509_NAME_ମୂଲ୍ୟ(cert_names, i)));
			}
			sk_X509_NAME_କାଢ଼_free(cert_names, X509_NAME_free);
		}

	}
	ଯଦି(listener->capath){
		rc = SSL_CTX_load_verify_dir(listener->ssl_ctx, listener->capath);
		ଯଦି(rc == 0){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			net__print_ssl_error(ଖାଲି, ଖାଲି);
			sk_X509_NAME_କାଢ଼_free(ca_names, X509_NAME_free);
			ଫେରନ୍ତୁ MOSQ_ERR_TLS;
		}

		SSL_add_dir_cert_subjects_to_ଗଦା(ca_names, listener->capath);
	}
	ଯଦି(ca_names){
		SSL_CTX_set_client_CA_list(listener->ssl_ctx, ca_names);
	}
#  endif

#  ଯଦି !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000
	ଯଦି(net__load_engine(listener)){
		ଫେରନ୍ତୁ MOSQ_ERR_TLS;
	}
#  endif
#endif
	ଫେରନ୍ତୁ net__load_certificates(listener);
}


#ଯଦି !defined(WIN32) && !defined(_AIX)


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା net__bind_interface(ସଂରଚନା mosquitto__listener *listener, ସଂରଚନା addrinfo *rp)
{
	/*
	 * This binds the listener sock to a network interface.
	 * The use of SO_BINDTODEVICE requires root access, which we don't have, so instead
	 * use getifaddrs to find the interface addresses, and use IP of the
	 * matching interface in the later bind().
	 */
	ସଂରଚନା ifaddrs *ifaddr;
	bool have_interface = false;

	ଯଦି(getifaddrs(&ifaddr) < 0){
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		ଫେରନ୍ତୁ MOSQ_ERR_ERRNO;
	}

	ପାଇଁ(ସଂରଚନା ifaddrs *ifa=ifaddr; ifa!=ଖାଲି; ifa=ifa->ifa_next){
		ଯଦି(ifa->ifa_addr == ଖାଲି){
			ଜାରି;
		}

		ଯଦି(!strcasecmp(listener->bind_interface, ifa->ifa_name)){
			have_interface = true;

			ଯଦି(ifa->ifa_addr->sa_family == rp->ai_addr->sa_family){
				ଯଦି(rp->ai_addr->sa_family == AF_INET){
					ଯଦି(listener->host &&
							memcmp(&((ସଂରଚନା sockaddr_in *)rp->ai_addr)->sin_addr,
							&((ସଂରଚନା sockaddr_in *)ifa->ifa_addr)->sin_addr,
							ଆକାର(ସଂରଚନା in_addr))){

						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}ଅନ୍ୟଥା{
						memcpy(&((ସଂରଚନା sockaddr_in *)rp->ai_addr)->sin_addr,
								&((ସଂରଚନା sockaddr_in *)ifa->ifa_addr)->sin_addr,
								ଆକାର(ସଂରଚନା in_addr));

						freeifaddrs(ifaddr);
						ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
					}
				}ଅନ୍ୟଥା ଯଦି(rp->ai_addr->sa_family == AF_INET6){
					ଯଦି(listener->host &&
							memcmp(&((ସଂରଚନା sockaddr_in6 *)rp->ai_addr)->sin6_addr,
							&((ସଂରଚନା sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
							ଆକାର(ସଂରଚନା in6_addr))){

						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
					}ଅନ୍ୟଥା{
						memcpy(&((ସଂରଚନା sockaddr_in6 *)rp->ai_addr)->sin6_addr,
								&((ସଂରଚନା sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
								ଆକାର(ସଂରଚନା in6_addr));

						((ସଂରଚନା sockaddr_in6 *)rp->ai_addr)->sin6_scope_id = ((ସଂରଚନା sockaddr_in6 *)ifa->ifa_addr)->sin6_scope_id;

						freeifaddrs(ifaddr);
						ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
					}
				}
			}
		}
	}
	freeifaddrs(ifaddr);
	ଯଦି(have_interface){
		log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Interface %s does not support %s configuration.",
				listener->bind_interface, rp->ai_addr->sa_family == AF_INET ? "IPv4" : "IPv6");
		ଫେରନ୍ତୁ MOSQ_ERR_NOT_SUPPORTED;
	}ଅନ୍ୟଥା{
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Interface %s does not exist.", listener->bind_interface);
		ଫେରନ୍ତୁ MOSQ_ERR_NOT_FOUND;
	}
}
#endif


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା net__socket_listen_tcp(ସଂରଚନା mosquitto__listener *listener)
{
	mosq_sock_t sock = INVALID_SOCKET;
	ସଂରଚନା addrinfo hints;
	ସଂରଚନା addrinfo *ainfo, *rp;
	ଅକ୍ଷର service[10];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ss_opt = 1;
#ifndef WIN32
	bool interface_bound = false;
#endif

	ଯଦି(!listener){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	snprintf(service, 10, "%d", listener->port);
	memset(&hints, 0, ଆକାର(ସଂରଚନା addrinfo));
	ଯଦି(listener->socket_domain){
		hints.ai_family = listener->socket_domain;
	}ଅନ୍ୟଥା{
		hints.ai_family = AF_UNSPEC;
	}
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	rc = getaddrinfo(listener->host, service, &hints, &ainfo);
	ଯଦି(rc){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error creating listener: %s.", gai_strerror(rc));
		ଫେରନ୍ତୁ INVALID_SOCKET;
	}

	listener->sock_ଗଣନା = 0;
	listener->socks = ଖାଲି;

	ପାଇଁ(rp = ainfo; rp; rp = rp->ai_next){
		ଯଦି(rp->ai_family == AF_INET){
			log__printf(ଖାଲି, MOSQ_LOG_INFO, "Opening ipv4 listen socket on port %d.", ntohs(((ସଂରଚନା sockaddr_in *)rp->ai_addr)->sin_port));
		}ଅନ୍ୟଥା ଯଦି(rp->ai_family == AF_INET6){
			log__printf(ଖାଲି, MOSQ_LOG_INFO, "Opening ipv6 listen socket on port %d.", ntohs(((ସଂରଚନା sockaddr_in6 *)rp->ai_addr)->sin6_port));
		}ଅନ୍ୟଥା{
			ଜାରି;
		}

		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		ଯଦି(sock == INVALID_SOCKET){
			net__print_error(MOSQ_LOG_WARNING, "Warning: %s");
			ଜାରି;
		}
		listener->sock_ଗଣନା++;
		listener->socks = mosquitto_realloc(listener->socks, ଆକାର(mosq_sock_t)*(size_t)listener->sock_ଗଣନା);
		ଯଦି(!listener->socks){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}
		listener->socks[listener->sock_ଗଣନା-1] = sock;

#ifndef WIN32
		ss_opt = 1;
		/* Unimportant if this fails */
		(ଶୂନ୍ୟ)setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (ଅଚଳ ଶୂନ୍ୟ *)&ss_opt, ଆକାର(ss_opt));
#endif
#ifdef IPV6_V6ONLY
		ss_opt = 1;
		(ଶୂନ୍ୟ)setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (ଅଚଳ ଶୂନ୍ୟ *)&ss_opt, ଆକାର(ss_opt));
#endif

		ଯଦି(net__socket_nonblock(&sock)){
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			ଫେରନ୍ତୁ 1;
		}

#ଯଦି !defined(WIN32) && !defined(_AIX)
		ଯଦି(listener->bind_interface){
			/* It might be possible that an interface does not support all relevant sa_families.
			 * We should successfully find at least one. */
			rc = net__bind_interface(listener, rp);
			ଯଦି(rc){
				COMPAT_CLOSE(sock);
				listener->sock_ଗଣନା--;
				ଯଦି(rc == MOSQ_ERR_NOT_FOUND || rc == MOSQ_ERR_INVAL){
					freeaddrinfo(ainfo);
					ଫେରନ୍ତୁ rc;
				}ଅନ୍ୟଥା{
					ଜାରି;
				}
			}
			interface_bound = true;
		}
#endif

		ଯଦି(bind(sock, rp->ai_addr, rp->ai_addrlen) == -1){
#ଯଦି defined(__linux__)
			ଯଦି(errno == EACCES){
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "If you are trying to bind to a privileged port (<1024), try using setcap and do not start the broker as root:");
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "    sudo setcap 'CAP_NET_BIND_SERVICE=+ep /usr/sbin/mosquitto'");
			}
#endif
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			COMPAT_CLOSE(sock);
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			ଫେରନ୍ତୁ 1;
		}

		ଯଦି(listen(sock, 100) == -1){
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			mosquitto_FREE(listener->socks);
			ଫେରନ୍ତୁ 1;
		}
	}
	freeaddrinfo(ainfo);

#ifndef WIN32
	ଯଦି(listener->bind_interface && !interface_bound){
		mosquitto_FREE(listener->socks);
		ଫେରନ୍ତୁ 1;
	}
#endif

	ଫେରନ୍ତୁ 0;
}


#ifdef WITH_UNIX_SOCKETS


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା net__socket_listen_unix(ସଂରଚନା mosquitto__listener *listener)
{
	ସଂରଚନା sockaddr_un addr;
	mosq_sock_t sock = INVALID_SOCKET;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
#ifndef WIN32
	mode_t old_mask;
#endif

	ଯଦି(listener->unix_socket_path == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}
	ଯଦି(strlen(listener->unix_socket_path) > ଆକାର(addr.sun_path)-1){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Path to unix socket is too long \"%s\".", listener->unix_socket_path);
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

#ifdef WIN32
	DeleteFile(listener->unix_socket_path);
#ଅନ୍ୟଥା
	unlink(listener->unix_socket_path);
#endif
	log__printf(ଖାଲି, MOSQ_LOG_INFO, "Opening unix listen socket on path %s.", listener->unix_socket_path);
	memset(&addr, 0, ଆକାର(ସଂରଚନା sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, listener->unix_socket_path, ଆକାର(addr.sun_path)-1);

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	ଯଦି(sock == INVALID_SOCKET){
		net__print_error(MOSQ_LOG_ERR, "Error creating unix socket: %s");
		ଫେରନ୍ତୁ 1;
	}
	listener->sock_ଗଣନା++;
	listener->socks = mosquitto_realloc(listener->socks, ଆକାର(mosq_sock_t)*(size_t)listener->sock_ଗଣନା);
	ଯଦି(!listener->socks){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
		COMPAT_CLOSE(sock);
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}
	listener->socks[listener->sock_ଗଣନା-1] = sock;


#ifndef WIN32
	old_mask = umask(0007);
#endif
	rc = bind(sock, (ସଂରଚନା sockaddr *)&addr, ଆକାର(ସଂରଚନା sockaddr_un));
#ifndef WIN32
	umask(old_mask);
#endif

	ଯଦି(rc == -1){
		net__print_error(MOSQ_LOG_ERR, "Error binding unix socket: %s");
		ଫେରନ୍ତୁ 1;
	}

	ଯଦି(listen(sock, 10) == -1){
		net__print_error(MOSQ_LOG_ERR, "Error listening to unix socket: %s");
		ଫେରନ୍ତୁ 1;
	}

	ଯଦି(net__socket_nonblock(&sock)){
		ଫେରନ୍ତୁ 1;
	}

	ଫେରନ୍ତୁ 0;
}
#endif


/* Creates a socket and listens on port 'port'.
 * Returns 1 on failure
 * Returns 0 on success.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା net__socket_listen(ସଂରଚନା mosquitto__listener *listener)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି(!listener){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

#ifdef WITH_UNIX_SOCKETS
	ଯଦି(listener->port == 0 && listener->unix_socket_path != ଖାଲି){
		rc = net__socket_listen_unix(listener);
	}ଅନ୍ୟଥା
#endif
	{
		rc = net__socket_listen_tcp(listener);
	}
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}

	/* We need to have at least one working socket. */
	ଯଦି(listener->sock_ଗଣନା > 0){
#ifdef WITH_TLS
		ଯଦି(listener->certfile && listener->keyfile){
			ଯଦି(net__tls_server_ctx(listener)){
				ଫେରନ୍ତୁ 1;
			}

			ଯଦି(net__tls_load_verify(listener)){
				ଫେରନ୍ତୁ 1;
			}
		}
#  ifdef FINAL_WITH_TLS_PSK
		ଯଦି(listener->psk_hint){
			ଯଦି(listener->certfile == ଖାଲି || listener->keyfile == ଖାଲି){
				ଯଦି(net__tls_server_ctx(listener)){
					ଫେରନ୍ତୁ 1;
				}
			}
			SSL_CTX_set_psk_server_callback(listener->ssl_ctx, psk_server_callback);
			ଯଦି(listener->psk_hint){
				rc = SSL_CTX_use_psk_identity_hint(listener->ssl_ctx, listener->psk_hint);
				ଯଦି(rc == 0){
					log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to set TLS PSK hint.");
					net__print_ssl_error(ଖାଲି, ଖାଲି);
					ଫେରନ୍ତୁ 1;
				}
			}
		}
#  endif /* FINAL_WITH_TLS_PSK */
		ଯଦି(tls_ex_index_context == -1){
			tls_ex_index_context = SSL_get_ex_new_index(0, "client context", ଖାଲି, ଖାଲି, ଖାଲି);
		}
		ଯଦି(tls_ex_index_listener == -1){
			tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", ଖାଲି, ଖାଲି, ଖାଲି);
		}
#endif /* WITH_TLS */
		ଫେରନ୍ତୁ 0;
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ 1;
	}
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା net__socket_get_address(mosq_sock_t sock, ଅକ୍ଷର *buf, size_t len, uint16_t *remote_port)
{
	ସଂରଚନା sockaddr_storage addr;
	socklen_t addrlen;

	memset(&addr, 0, ଆକାର(ସଂରଚନା sockaddr_storage));
	addrlen = ଆକାର(addr);
	ଯଦି(!getpeername(sock, (ସଂରଚନା sockaddr *)&addr, &addrlen)){
		ଯଦି(addr.ss_family == AF_INET){
			ଯଦି(remote_port){
				*remote_port = ntohs(((ସଂରଚନା sockaddr_in *)&addr)->sin_port);
			}
			ଯଦି(inet_ntop(AF_INET, &((ସଂରଚନା sockaddr_in *)&addr)->sin_addr.s_addr, buf, (socklen_t)len)){
				ଫେରନ୍ତୁ 0;
			}
		}ଅନ୍ୟଥା ଯଦି(addr.ss_family == AF_INET6){
			ଯଦି(remote_port){
				*remote_port = ntohs(((ସଂରଚନା sockaddr_in6 *)&addr)->sin6_port);
			}
			ଯଦି(inet_ntop(AF_INET6, &((ସଂରଚନା sockaddr_in6 *)&addr)->sin6_addr.s6_addr, buf, (socklen_t)len)){
				ଫେରନ୍ତୁ 0;
			}
#ifdef WITH_UNIX_SOCKETS
		}ଅନ୍ୟଥା ଯଦି(addr.ss_family == AF_UNIX){
			ସଂରଚନା sockaddr_un un;
			addrlen = ଆକାର(ସଂରଚନା sockaddr_un);
			ଯଦି(!getsockname(sock, (ସଂରଚନା sockaddr *)&un, &addrlen)){
				snprintf(buf, len, "%s", un.sun_path);
			}ଅନ୍ୟଥା{
				snprintf(buf, len, "unix-socket");
			}
			ଫେରନ୍ତୁ 0;
#endif
		}
	}
	ଫେରନ୍ତୁ 1;
}
