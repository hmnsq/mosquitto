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

#અન્તર્ભાવ "config.h"

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
#નહીંતર
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#અન્તર્ભાવ <errno.h>
#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>
#ifdef WITH_WRAP
#  include <tcpd.h>
#endif

#ifdef HAVE_NETINET_IN_H
#  include <netinet/in.h>
#endif

#જો defined(WITH_UNIX_SOCKETS) || defined(WITH_TLS)
#  include "sys/stat.h"
#endif

#ifdef WITH_UNIX_SOCKETS
#  ifdef WIN32
#    include <afunix.h>
#  નહીંતર
#    include <sys/un.h>
#  endif
#endif

#ifdef __QNX__
#  include <net/netbyte.h>
#endif

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"
#અન્તર્ભાવ "net_mosq.h"
#અન્તર્ભાવ "util_mosq.h"

#ifdef WITH_TLS
#  include "tls_mosq.h"
#  include <openssl/err.h>
સ્થિર પૂર્ણાંક tls_ex_index_context = -1;
સ્થિર પૂર્ણાંક tls_ex_index_listener = -1;
#endif

#અન્તર્ભાવ "sys_tree.h"

/* For EMFILE handling */
સ્થિર mosq_sock_t spare_sock = INVALID_SOCKET;


શૂન્ય net__broker_init(શૂન્ય)
{
	spare_sock = socket(AF_INET, SOCK_STREAM, 0);
	net__init();
#ifdef WITH_TLS
	net__init_tls();
#endif
}


શૂન્ય net__broker_cleanup(શૂન્ય)
{
	જો(spare_sock != INVALID_SOCKET){
		COMPAT_CLOSE(spare_sock);
		spare_sock = INVALID_SOCKET;
	}
	net__cleanup();
}


સ્થિર શૂન્ય net__print_error(unsigned પૂર્ણાંક log, અચલ અક્ષર *format_str)
{
	અક્ષર *buf;

#ifdef WIN32
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			ખાલી, WSAGetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, ખાલી);

	log__printf(ખાલી, log, format_str, buf);
	LocalFree(buf);
#નહીંતર
	buf = strerror(errno);
	log__printf(ખાલી, log, format_str, buf);
#endif
}


સંરચના mosquitto *net__socket_accept(સંરચના mosquitto__listener_sock *listensock)
{
	mosq_sock_t new_sock = INVALID_SOCKET;
	સંરચના mosquitto *new_context;
#ifdef WITH_TLS
	BIO *bio;
#endif
#ifdef WITH_WRAP
	સંરચના request_info wrap_req;
	અક્ષર address[1024];
#endif

	new_sock = accept(listensock->sock, ખાલી, 0);
	જો(new_sock == INVALID_SOCKET){
#ifdef WIN32
		WINDOWS_SET_ERRNO();
		જો(errno == WSAEMFILE){
#નહીંતર
		જો(errno == EMFILE || errno == ENFILE){
#endif
			/* Close the spare socket, which means we should be able to accept
			 * this connection. Accept it, then close it immediately and create
			 * a new spare_sock. This prevents the situation of ever properly
			 * running out of sockets.
			 * It would be nice to send a "server not available" connack here,
			 * but there are lots of reasons why this would be tricky (TLS
			 * being the big one). */
			COMPAT_CLOSE(spare_sock);
			new_sock = accept(listensock->sock, ખાલી, 0);
			જો(new_sock != INVALID_SOCKET){
				COMPAT_CLOSE(new_sock);
			}
			spare_sock = socket(AF_INET, SOCK_STREAM, 0);
			log__printf(ખાલી, MOSQ_LOG_WARNING,
					"Unable to accept new connection, system socket count has been exceeded. Try increasing \"ulimit -n\" or equivalent.");
		}
		પાછા ખાલી;
	}

	metrics__int_inc(mosq_counter_socket_connections, 1);

	જો(net__socket_nonblock(&new_sock)){
		પાછા ખાલી;
	}

#ifdef WITH_WRAP
	/* Use tcpd / libwrap to determine whether a connection is allowed. */
	request_init(&wrap_req, RQ_FILE, new_sock, RQ_DAEMON, "mosquitto", 0);
	fromhost(&wrap_req);
	જો(!hosts_access(&wrap_req)){
		/* Access is denied */
		જો(db.config->connection_messages == true){
			જો(!net__socket_get_address(new_sock, address, 1024, ખાલી)){
				log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client connection from %s denied access by tcpd.", address);
			}
		}
		COMPAT_CLOSE(new_sock);
		પાછા ખાલી;
	}
#endif

	જો(db.config->set_tcp_nodelay && listensock->listener->port){
		પૂર્ણાંક flag = 1;
		જો(setsockopt(new_sock, IPPROTO_TCP, TCP_NODELAY, (અચલ શૂન્ય *)&flag, કદ(પૂર્ણાંક)) != 0){
			log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Unable to set TCP_NODELAY.");
		}
	}

	new_context = context__init();
	જો(!new_context){
		COMPAT_CLOSE(new_sock);
		પાછા ખાલી;
	}
	new_context->listener = listensock->listener;
	જો(!new_context->listener){
		context__cleanup(new_context, true);
		પાછા ખાલી;
	}
	new_context->listener->client_ગણતરી++;

	જો(new_context->listener->enable_proxy_protocol){
		જો(context__init_sock(new_context, new_sock, false) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			પાછા ખાલી;
		}
		જો(new_context->listener->enable_proxy_protocol == 2){
			new_context->transport = mosq_t_proxy_v2;
		}નહીંતર{
			new_context->transport = mosq_t_proxy_v1;
		}
		new_context->proxy.cmd = -1;
	}નહીંતર{
		જો(context__init_sock(new_context, new_sock, true) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			પાછા ખાલી;
		}
		પસંદગી(new_context->listener->protocol){
			સ્થિતિ mp_mqtt:
				new_context->transport = mosq_t_tcp;
				તોડો;
#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			સ્થિતિ mp_websockets:
				જો(http__context_init(new_context)){
					context__cleanup(new_context, true);
					પાછા ખાલી;
				}
				તોડો;
#endif
			મૂળભૂત:
				context__cleanup(new_context, true);
				પાછા ખાલી;
		}
	}

	જો((new_context->listener->max_connections > 0 && new_context->listener->client_ગણતરી > new_context->listener->max_connections)
			|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (unsigned પૂર્ણાંક)db.config->global_max_connections)){

		જો(db.config->connection_messages == true){
			log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", new_context->address);
		}
		context__cleanup(new_context, true);
		પાછા ખાલી;
	}

#ifdef WITH_TLS
	/* TLS init */
	જો(new_context->listener->ssl_ctx){
		new_context->ssl = SSL_new(new_context->listener->ssl_ctx);
		જો(!new_context->ssl){
			context__cleanup(new_context, true);
			પાછા ખાલી;
		}
		જો(!SSL_set_ex_data(new_context->ssl, tls_ex_index_context, new_context)
				|| !SSL_set_ex_data(new_context->ssl, tls_ex_index_listener, new_context->listener)){
			context__cleanup(new_context, true);
			પાછા ખાલી;
		}
		new_context->want_write = true;
		bio = BIO_new_socket(new_sock, BIO_NOCLOSE);
		SSL_set_bio(new_context->ssl, bio, bio);
		ERR_clear_error();
		SSL_set_accept_state(new_context->ssl);
	}
#endif

	જો(db.config->connection_messages == true
			&& !new_context->listener->enable_proxy_protocol){

		log__printf(ખાલી, MOSQ_LOG_NOTICE, "New connection from %s:%d on port %d.",
				new_context->address, new_context->remote_port, new_context->listener->port);
	}

	mux__new(new_context);
	keepalive__add(new_context);

	પાછા new_context;
}

#ifdef WITH_TLS


સ્થિર પૂર્ણાંક client_certificate_verify(પૂર્ણાંક preverify_ok, X509_STORE_CTX *ctx)
{
	/* Preverify should check expiry, revocation. */
	જો(preverify_ok == 0){
		SSL *ssl;
		ssl = X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx());
		જો(ssl){
			સંરચના mosquitto__listener *listener;
			listener = SSL_get_ex_data(ssl, tls_ex_index_listener);

			જો(listener && listener->disable_client_cert_date_checks){
				પૂર્ણાંક err = X509_STORE_CTX_get_error(ctx);
				જો(err == X509_V_ERR_CERT_NOT_YET_VALID || err == X509_V_ERR_CERT_HAS_EXPIRED){
					preverify_ok = 1;
				}
			}
		}
	}
	પાછા preverify_ok;
}
#endif

#ifdef FINAL_WITH_TLS_PSK


સ્થિર unsigned પૂર્ણાંક psk_server_callback(SSL *ssl, અચલ અક્ષર *identity, unsigned અક્ષર *psk, unsigned પૂર્ણાંક max_psk_len)
{
	સંરચના mosquitto *context;
	સંરચના mosquitto__listener *listener;
	અક્ષર *psk_ચાવી = ખાલી;
	પૂર્ણાંક len;
	અચલ અક્ષર *psk_hint;

	જો(!identity){
		પાછા 0;
	}

	context = SSL_get_ex_data(ssl, tls_ex_index_context);
	જો(!context){
		પાછા 0;
	}

	listener = SSL_get_ex_data(ssl, tls_ex_index_listener);
	જો(!listener){
		પાછા 0;
	}

	psk_hint = listener->psk_hint;

	/* The hex to BN conversion results in the length halving, so we can pass
	 * max_psk_len*2 as the max hex key here. */
	psk_ચાવી = mosquitto_calloc(1, (size_t)max_psk_len*2 + 1);
	જો(!psk_ચાવી){
		પાછા 0;
	}

	જો(mosquitto_psk_ચાવી_get(context, psk_hint, identity, psk_ચાવી, (પૂર્ણાંક)max_psk_len*2) != MOSQ_ERR_SUCCESS){
		mosquitto_FREE(psk_ચાવી);
		પાછા 0;
	}

	len = mosquitto__hex2bin(psk_ચાવી, psk, (પૂર્ણાંક)max_psk_len);
	જો(len < 0){
		mosquitto_FREE(psk_ચાવી);
		પાછા 0;
	}

	જો(listener->use_identity_as_username){
		જો(mosquitto_validate_utf8(identity, (પૂર્ણાંક)strlen(identity))){
			mosquitto_free(psk_ચાવી);
			પાછા 0;
		}
		context->username = mosquitto_strdup(identity);
		જો(!context->username){
			mosquitto_FREE(psk_ચાવી);
			પાછા 0;
		}
	}

	mosquitto_FREE(psk_ચાવી);
	પાછા (unsigned પૂર્ણાંક)len;
}
#endif

#ifdef WITH_TLS


સ્થિર શૂન્ય tls_keylog_callback(અચલ SSL *ssl, અચલ અક્ષર *લીટી)
{
	UNUSED(ssl);

	જો(db.tls_keylog){
		FILE *fptr;
		fptr = mosquitto_fopen(db.tls_keylog, "at", true);
		જો(fptr){
#ifndef WIN32
			/* Until mosquitto_fopen enforces file permissions on all files,
			 * enforce it here. We can enforce it here, because it isn't a
			 * change of behaviour. */
			સંરચના stat statbuf;
			જો(fstat(fileno(fptr), &statbuf) < 0
					|| statbuf.st_mode & (S_IRWXG | S_IRWXO)){

				fclose(fptr);
				પાછા;
			}
#endif
			fprintf(fptr, "%s\n", લીટી);
			fclose(fptr);
		}નહીંતર{
#ifndef WIN32
			જો(errno == ELOOP){
				log__printf(ખાલી, MOSQ_LOG_INFO, "Error: keylog file must not be a symbolic link");
			}નહીંતર
#endif
			{
				log__printf(ખાલી, MOSQ_LOG_INFO, "Error: Unable to open keylog file: %s", strerror(errno));
			}
		}
	}
}


પૂર્ણાંક net__tls_server_ctx(સંરચના mosquitto__listener *listener)
{
	અક્ષર buf[256];
	પૂર્ણાંક rc;

	જો(tls_ex_index_context == -1){
		tls_ex_index_context = SSL_get_ex_new_index(0, "client context", ખાલી, ખાલી, ખાલી);
	}
	જો(tls_ex_index_listener == -1){
		tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", ખાલી, ખાલી, ખાલી);
	}

	જો(listener->ssl_ctx){
		SSL_CTX_free(listener->ssl_ctx);
		listener->ssl_ctx = ખાલી;
	}

	listener->ssl_ctx = SSL_CTX_new(TLS_server_method());

	જો(!listener->ssl_ctx){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to create TLS context.");
		પાછા MOSQ_ERR_TLS;
	}

#ifdef SSL_OP_NO_TLSv1_3
	જો(db.config->per_listener_settings){
		જો(listener->security_options->psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}નહીંતર{
		જો(db.config->security_options.psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}
#endif

	જો(listener->tls_version == ખાલી){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
#ifdef SSL_OP_NO_TLSv1_3
	}નહીંતર જો(!strcmp(listener->tls_version, "tlsv1.3")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2);
#endif
	}નહીંતર જો(!strcmp(listener->tls_version, "tlsv1.2")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
	}નહીંતર જો(!strcmp(listener->tls_version, "tlsv1.1")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1);
		log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: TLS v1.1 is insecure. It must only be used in the case "
				"where you have devices that cannot be upgraded to use a secure version of TLS. It is "
				"recommended to use as secure a set of ciphers as possible and that will restrict it to "
				"TLS v1.1 only, and to use a separate listener using TLS v1.2 and secure ciphers for your "
				"other devices.");
		log__printf(ખાલી, MOSQ_LOG_WARNING, "Please be aware that support for TLS v1.1 will go away eventually "
				"and that you should plan now to migrate away from it.");
	}નહીંતર{
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unsupported tls_version \"%s\".", listener->tls_version);
		પાછા MOSQ_ERR_TLS;
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

	જો(db.tls_keylog){
		log__printf(ખાલી, MOSQ_LOG_NOTICE, "TLS key logging to '%s' enabled for all listeners.",
				db.tls_keylog);
		log__printf(ખાલી, MOSQ_LOG_NOTICE, "TLS key logging is for DEBUGGING only.");

		SSL_CTX_set_keylog_callback(listener->ssl_ctx, tls_keylog_callback);
	}

	snprintf(buf, 256, "mosquitto-%d", listener->port);
	SSL_CTX_set_session_id_context(listener->ssl_ctx, (unsigned અક્ષર *)buf, (unsigned પૂર્ણાંક)strlen(buf));

	જો(listener->ciphers){
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, listener->ciphers);
		જો(rc == 0){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			પાછા MOSQ_ERR_TLS;
		}
	}નહીંતર{
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, "DEFAULT:!aNULL:!eNULL:!LOW:!EXPORT:!SSLv2:@STRENGTH");
		જો(rc == 0){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			પાછા MOSQ_ERR_TLS;
		}
	}
	જો(listener->ciphers_tls13){
		rc = SSL_CTX_set_ciphersuites(listener->ssl_ctx, listener->ciphers_tls13);
		જો(rc == 0){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to set TLS 1.3 ciphersuites. Check cipher_tls13 list \"%s\".", listener->ciphers_tls13);
			પાછા MOSQ_ERR_TLS;
		}
	}

	પાછા MOSQ_ERR_SUCCESS;
}
#endif


#ifdef WITH_TLS


સ્થિર પૂર્ણાંક net__load_crl_file(સંરચના mosquitto__listener *listener)
{
	X509_STORE *store;
	X509_LOOKUP *lookup;
	પૂર્ણાંક rc;

	store = SSL_CTX_get_cert_store(listener->ssl_ctx);
	જો(!store){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to obtain TLS store.");
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		પાછા MOSQ_ERR_TLS;
	}
	lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
	rc = X509_load_crl_file(lookup, listener->crlfile, X509_FILETYPE_PEM);
	જો(rc < 1){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to load certificate revocation file \"%s\". Check crlfile.", listener->crlfile);
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		net__print_ssl_error(ખાલી, ખાલી);
		પાછા MOSQ_ERR_TLS;
	}
	X509_STORE_set_flags(store, X509_V_FLAG_CRL_CHECK);

	પાછા MOSQ_ERR_SUCCESS;
}
#endif


પૂર્ણાંક net__load_certificates(સંરચના mosquitto__listener *listener)
{
#ifdef WITH_TLS
	પૂર્ણાંક rc;

	જો(listener->require_certificate){
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, client_certificate_verify);
	}નહીંતર{
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_NONE, client_certificate_verify);
	}
	rc = SSL_CTX_use_certificate_chain_file(listener->ssl_ctx, listener->certfile);
	જો(rc != 1){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
		net__print_ssl_error(ખાલી, ખાલી);
		પાછા MOSQ_ERR_TLS;
	}
	જો(listener->tls_engine == ખાલી || listener->tls_keyform == mosq_k_pem){
		rc = SSL_CTX_use_Private_ચાવી_file(listener->ssl_ctx, listener->keyfile, SSL_FILETYPE_PEM);
		જો(rc != 1){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			net__print_ssl_error(ખાલી, ખાલી);
			પાછા MOSQ_ERR_TLS;
		}
	}
	rc = SSL_CTX_check_private_ચાવી(listener->ssl_ctx);
	જો(rc != 1){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Server certificate/key are inconsistent.");
		net__print_ssl_error(ખાલી, ખાલી);
		પાછા MOSQ_ERR_TLS;
	}
	/* Load CRLs if they exist. */
	જો(listener->crlfile){
		rc = net__load_crl_file(listener);
		જો(rc){
			પાછા rc;
		}
	}
#નહીંતર
	UNUSED(listener);
#endif
	પાછા MOSQ_ERR_SUCCESS;
}


#જો defined(WITH_TLS) && !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000


સ્થિર પૂર્ણાંક net__load_engine(સંરચના mosquitto__listener *listener)
{
	ENGINE *engine = ખાલી;
	UI_METHOD *ui_method;
	EVP_PKEY *pkey;

	જો(!listener->tls_engine){
		પાછા MOSQ_ERR_SUCCESS;
	}

	engine = ENGINE_by_id(listener->tls_engine);
	જો(!engine){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error loading %s engine\n", listener->tls_engine);
		net__print_ssl_error(ખાલી, ખાલી);
		પાછા MOSQ_ERR_TLS;
	}
	જો(!ENGINE_init(engine)){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Failed engine initialisation\n");
		net__print_ssl_error(ખાલી, ખાલી);
		પાછા MOSQ_ERR_TLS;
	}
	ENGINE_set_default(engine, ENGINE_METHOD_ALL);

	જો(listener->tls_keyform == mosq_k_engine){
		ui_method = net__get_ui_method();
		જો(listener->tls_engine_kpass_sha1){
			જો(!ENGINE_ctrl_cmd(engine, ENGINE_SECRET_MODE, ENGINE_SECRET_MODE_SHA, ખાલી, ખાલી, 0)){
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to set engine secret mode sha");
				net__print_ssl_error(ખાલી, ખાલી);
				પાછા MOSQ_ERR_TLS;
			}
			જો(!ENGINE_ctrl_cmd(engine, ENGINE_PIN, 0, listener->tls_engine_kpass_sha1, ખાલી, 0)){
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to set engine pin");
				net__print_ssl_error(ખાલી, ખાલી);
				પાછા MOSQ_ERR_TLS;
			}
			ui_method = ખાલી;
		}
		pkey = ENGINE_load_private_ચાવી(engine, listener->keyfile, ui_method, ખાલી);
		જો(!pkey){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to load engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(ખાલી, ખાલી);
			પાછા MOSQ_ERR_TLS;
		}
		જો(SSL_CTX_use_Private_ચાવી(listener->ssl_ctx, pkey) <= 0){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to use engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(ખાલી, ખાલી);
			પાછા MOSQ_ERR_TLS;
		}
	}
	ENGINE_free(engine); /* release the structural reference from ENGINE_by_id() */

	પાછા MOSQ_ERR_SUCCESS;
}
#endif


પૂર્ણાંક net__tls_load_verify(સંરચના mosquitto__listener *listener)
{
#ifdef WITH_TLS
	પૂર્ણાંક rc;

#  જો OPENSSL_VERSION_સંખ્યા < 0x30000000L
	જો(listener->cafile || listener->capath){
		rc = SSL_CTX_load_verify_locations(listener->ssl_ctx, listener->cafile, listener->capath);
		જો(rc == 0){
			જો(listener->cafile && listener->capath){
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\" and capath \"%s\".", listener->cafile, listener->capath);
			}નહીંતર જો(listener->cafile){
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			}નહીંતર{
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			}
		}
	}
#  નહીંતર
	ઢગલો_OF(X509_NAME) *ca_names = ખાલી;
	જો(listener->cafile || listener->capath){
		ca_names = sk_X509_NAME_new_null();
	}

	જો(listener->cafile){
		rc = SSL_CTX_load_verify_file(listener->ssl_ctx, listener->cafile);
		જો(rc == 0){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			net__print_ssl_error(ખાલી, ખાલી);
			પાછા MOSQ_ERR_TLS;
		}

		ઢગલો_OF(X509_NAME) *cert_names = SSL_load_client_CA_file(listener->cafile);
		જો(cert_names){
			માટે(પૂર્ણાંક i=0; i<sk_X509_NAME_અંક(cert_names); i++){
				sk_X509_NAME_ધકેલો(ca_names, X509_NAME_dup(sk_X509_NAME_કિંમત(cert_names, i)));
			}
			sk_X509_NAME_કાઢો_free(cert_names, X509_NAME_free);
		}

	}
	જો(listener->capath){
		rc = SSL_CTX_load_verify_dir(listener->ssl_ctx, listener->capath);
		જો(rc == 0){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			net__print_ssl_error(ખાલી, ખાલી);
			sk_X509_NAME_કાઢો_free(ca_names, X509_NAME_free);
			પાછા MOSQ_ERR_TLS;
		}

		SSL_add_dir_cert_subjects_to_ઢગલો(ca_names, listener->capath);
	}
	જો(ca_names){
		SSL_CTX_set_client_CA_list(listener->ssl_ctx, ca_names);
	}
#  endif

#  જો !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000
	જો(net__load_engine(listener)){
		પાછા MOSQ_ERR_TLS;
	}
#  endif
#endif
	પાછા net__load_certificates(listener);
}


#જો !defined(WIN32) && !defined(_AIX)


સ્થિર પૂર્ણાંક net__bind_interface(સંરચના mosquitto__listener *listener, સંરચના addrinfo *rp)
{
	/*
	 * This binds the listener sock to a network interface.
	 * The use of SO_BINDTODEVICE requires root access, which we don't have, so instead
	 * use getifaddrs to find the interface addresses, and use IP of the
	 * matching interface in the later bind().
	 */
	સંરચના ifaddrs *ifaddr;
	bool have_interface = false;

	જો(getifaddrs(&ifaddr) < 0){
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		પાછા MOSQ_ERR_ERRNO;
	}

	માટે(સંરચના ifaddrs *ifa=ifaddr; ifa!=ખાલી; ifa=ifa->ifa_next){
		જો(ifa->ifa_addr == ખાલી){
			ચાલુ;
		}

		જો(!strcasecmp(listener->bind_interface, ifa->ifa_name)){
			have_interface = true;

			જો(ifa->ifa_addr->sa_family == rp->ai_addr->sa_family){
				જો(rp->ai_addr->sa_family == AF_INET){
					જો(listener->host &&
							memcmp(&((સંરચના sockaddr_in *)rp->ai_addr)->sin_addr,
							&((સંરચના sockaddr_in *)ifa->ifa_addr)->sin_addr,
							કદ(સંરચના in_addr))){

						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						પાછા MOSQ_ERR_INVAL;
					}નહીંતર{
						memcpy(&((સંરચના sockaddr_in *)rp->ai_addr)->sin_addr,
								&((સંરચના sockaddr_in *)ifa->ifa_addr)->sin_addr,
								કદ(સંરચના in_addr));

						freeifaddrs(ifaddr);
						પાછા MOSQ_ERR_SUCCESS;
					}
				}નહીંતર જો(rp->ai_addr->sa_family == AF_INET6){
					જો(listener->host &&
							memcmp(&((સંરચના sockaddr_in6 *)rp->ai_addr)->sin6_addr,
							&((સંરચના sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
							કદ(સંરચના in6_addr))){

						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						પાછા MOSQ_ERR_INVAL;
					}નહીંતર{
						memcpy(&((સંરચના sockaddr_in6 *)rp->ai_addr)->sin6_addr,
								&((સંરચના sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
								કદ(સંરચના in6_addr));

						((સંરચના sockaddr_in6 *)rp->ai_addr)->sin6_scope_id = ((સંરચના sockaddr_in6 *)ifa->ifa_addr)->sin6_scope_id;

						freeifaddrs(ifaddr);
						પાછા MOSQ_ERR_SUCCESS;
					}
				}
			}
		}
	}
	freeifaddrs(ifaddr);
	જો(have_interface){
		log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Interface %s does not support %s configuration.",
				listener->bind_interface, rp->ai_addr->sa_family == AF_INET ? "IPv4" : "IPv6");
		પાછા MOSQ_ERR_NOT_SUPPORTED;
	}નહીંતર{
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Interface %s does not exist.", listener->bind_interface);
		પાછા MOSQ_ERR_NOT_FOUND;
	}
}
#endif


સ્થિર પૂર્ણાંક net__socket_listen_tcp(સંરચના mosquitto__listener *listener)
{
	mosq_sock_t sock = INVALID_SOCKET;
	સંરચના addrinfo hints;
	સંરચના addrinfo *ainfo, *rp;
	અક્ષર service[10];
	પૂર્ણાંક rc;
	પૂર્ણાંક ss_opt = 1;
#ifndef WIN32
	bool interface_bound = false;
#endif

	જો(!listener){
		પાછા MOSQ_ERR_INVAL;
	}

	snprintf(service, 10, "%d", listener->port);
	memset(&hints, 0, કદ(સંરચના addrinfo));
	જો(listener->socket_domain){
		hints.ai_family = listener->socket_domain;
	}નહીંતર{
		hints.ai_family = AF_UNSPEC;
	}
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	rc = getaddrinfo(listener->host, service, &hints, &ainfo);
	જો(rc){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error creating listener: %s.", gai_strerror(rc));
		પાછા INVALID_SOCKET;
	}

	listener->sock_ગણતરી = 0;
	listener->socks = ખાલી;

	માટે(rp = ainfo; rp; rp = rp->ai_next){
		જો(rp->ai_family == AF_INET){
			log__printf(ખાલી, MOSQ_LOG_INFO, "Opening ipv4 listen socket on port %d.", ntohs(((સંરચના sockaddr_in *)rp->ai_addr)->sin_port));
		}નહીંતર જો(rp->ai_family == AF_INET6){
			log__printf(ખાલી, MOSQ_LOG_INFO, "Opening ipv6 listen socket on port %d.", ntohs(((સંરચના sockaddr_in6 *)rp->ai_addr)->sin6_port));
		}નહીંતર{
			ચાલુ;
		}

		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		જો(sock == INVALID_SOCKET){
			net__print_error(MOSQ_LOG_WARNING, "Warning: %s");
			ચાલુ;
		}
		listener->sock_ગણતરી++;
		listener->socks = mosquitto_realloc(listener->socks, કદ(mosq_sock_t)*(size_t)listener->sock_ગણતરી);
		જો(!listener->socks){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			પાછા MOSQ_ERR_NOMEM;
		}
		listener->socks[listener->sock_ગણતરી-1] = sock;

#ifndef WIN32
		ss_opt = 1;
		/* Unimportant if this fails */
		(શૂન્ય)setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (અચલ શૂન્ય *)&ss_opt, કદ(ss_opt));
#endif
#ifdef IPV6_V6ONLY
		ss_opt = 1;
		(શૂન્ય)setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (અચલ શૂન્ય *)&ss_opt, કદ(ss_opt));
#endif

		જો(net__socket_nonblock(&sock)){
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			પાછા 1;
		}

#જો !defined(WIN32) && !defined(_AIX)
		જો(listener->bind_interface){
			/* It might be possible that an interface does not support all relevant sa_families.
			 * We should successfully find at least one. */
			rc = net__bind_interface(listener, rp);
			જો(rc){
				COMPAT_CLOSE(sock);
				listener->sock_ગણતરી--;
				જો(rc == MOSQ_ERR_NOT_FOUND || rc == MOSQ_ERR_INVAL){
					freeaddrinfo(ainfo);
					પાછા rc;
				}નહીંતર{
					ચાલુ;
				}
			}
			interface_bound = true;
		}
#endif

		જો(bind(sock, rp->ai_addr, rp->ai_addrlen) == -1){
#જો defined(__linux__)
			જો(errno == EACCES){
				log__printf(ખાલી, MOSQ_LOG_ERR, "If you are trying to bind to a privileged port (<1024), try using setcap and do not start the broker as root:");
				log__printf(ખાલી, MOSQ_LOG_ERR, "    sudo setcap 'CAP_NET_BIND_SERVICE=+ep /usr/sbin/mosquitto'");
			}
#endif
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			COMPAT_CLOSE(sock);
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			પાછા 1;
		}

		જો(listen(sock, 100) == -1){
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			mosquitto_FREE(listener->socks);
			પાછા 1;
		}
	}
	freeaddrinfo(ainfo);

#ifndef WIN32
	જો(listener->bind_interface && !interface_bound){
		mosquitto_FREE(listener->socks);
		પાછા 1;
	}
#endif

	પાછા 0;
}


#ifdef WITH_UNIX_SOCKETS


સ્થિર પૂર્ણાંક net__socket_listen_unix(સંરચના mosquitto__listener *listener)
{
	સંરચના sockaddr_un addr;
	mosq_sock_t sock = INVALID_SOCKET;
	પૂર્ણાંક rc;
#ifndef WIN32
	mode_t old_mask;
#endif

	જો(listener->unix_socket_path == ખાલી){
		પાછા MOSQ_ERR_INVAL;
	}
	જો(strlen(listener->unix_socket_path) > કદ(addr.sun_path)-1){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Path to unix socket is too long \"%s\".", listener->unix_socket_path);
		પાછા MOSQ_ERR_INVAL;
	}

#ifdef WIN32
	DeleteFile(listener->unix_socket_path);
#નહીંતર
	unlink(listener->unix_socket_path);
#endif
	log__printf(ખાલી, MOSQ_LOG_INFO, "Opening unix listen socket on path %s.", listener->unix_socket_path);
	memset(&addr, 0, કદ(સંરચના sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, listener->unix_socket_path, કદ(addr.sun_path)-1);

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	જો(sock == INVALID_SOCKET){
		net__print_error(MOSQ_LOG_ERR, "Error creating unix socket: %s");
		પાછા 1;
	}
	listener->sock_ગણતરી++;
	listener->socks = mosquitto_realloc(listener->socks, કદ(mosq_sock_t)*(size_t)listener->sock_ગણતરી);
	જો(!listener->socks){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
		COMPAT_CLOSE(sock);
		પાછા MOSQ_ERR_NOMEM;
	}
	listener->socks[listener->sock_ગણતરી-1] = sock;


#ifndef WIN32
	old_mask = umask(0007);
#endif
	rc = bind(sock, (સંરચના sockaddr *)&addr, કદ(સંરચના sockaddr_un));
#ifndef WIN32
	umask(old_mask);
#endif

	જો(rc == -1){
		net__print_error(MOSQ_LOG_ERR, "Error binding unix socket: %s");
		પાછા 1;
	}

	જો(listen(sock, 10) == -1){
		net__print_error(MOSQ_LOG_ERR, "Error listening to unix socket: %s");
		પાછા 1;
	}

	જો(net__socket_nonblock(&sock)){
		પાછા 1;
	}

	પાછા 0;
}
#endif


/* Creates a socket and listens on port 'port'.
 * Returns 1 on failure
 * Returns 0 on success.
 */
પૂર્ણાંક net__socket_listen(સંરચના mosquitto__listener *listener)
{
	પૂર્ણાંક rc;

	જો(!listener){
		પાછા MOSQ_ERR_INVAL;
	}

#ifdef WITH_UNIX_SOCKETS
	જો(listener->port == 0 && listener->unix_socket_path != ખાલી){
		rc = net__socket_listen_unix(listener);
	}નહીંતર
#endif
	{
		rc = net__socket_listen_tcp(listener);
	}
	જો(rc){
		પાછા rc;
	}

	/* We need to have at least one working socket. */
	જો(listener->sock_ગણતરી > 0){
#ifdef WITH_TLS
		જો(listener->certfile && listener->keyfile){
			જો(net__tls_server_ctx(listener)){
				પાછા 1;
			}

			જો(net__tls_load_verify(listener)){
				પાછા 1;
			}
		}
#  ifdef FINAL_WITH_TLS_PSK
		જો(listener->psk_hint){
			જો(listener->certfile == ખાલી || listener->keyfile == ખાલી){
				જો(net__tls_server_ctx(listener)){
					પાછા 1;
				}
			}
			SSL_CTX_set_psk_server_callback(listener->ssl_ctx, psk_server_callback);
			જો(listener->psk_hint){
				rc = SSL_CTX_use_psk_identity_hint(listener->ssl_ctx, listener->psk_hint);
				જો(rc == 0){
					log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to set TLS PSK hint.");
					net__print_ssl_error(ખાલી, ખાલી);
					પાછા 1;
				}
			}
		}
#  endif /* FINAL_WITH_TLS_PSK */
		જો(tls_ex_index_context == -1){
			tls_ex_index_context = SSL_get_ex_new_index(0, "client context", ખાલી, ખાલી, ખાલી);
		}
		જો(tls_ex_index_listener == -1){
			tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", ખાલી, ખાલી, ખાલી);
		}
#endif /* WITH_TLS */
		પાછા 0;
	}નહીંતર{
		પાછા 1;
	}
}


પૂર્ણાંક net__socket_get_address(mosq_sock_t sock, અક્ષર *buf, size_t len, uint16_t *remote_port)
{
	સંરચના sockaddr_storage addr;
	socklen_t addrlen;

	memset(&addr, 0, કદ(સંરચના sockaddr_storage));
	addrlen = કદ(addr);
	જો(!getpeername(sock, (સંરચના sockaddr *)&addr, &addrlen)){
		જો(addr.ss_family == AF_INET){
			જો(remote_port){
				*remote_port = ntohs(((સંરચના sockaddr_in *)&addr)->sin_port);
			}
			જો(inet_ntop(AF_INET, &((સંરચના sockaddr_in *)&addr)->sin_addr.s_addr, buf, (socklen_t)len)){
				પાછા 0;
			}
		}નહીંતર જો(addr.ss_family == AF_INET6){
			જો(remote_port){
				*remote_port = ntohs(((સંરચના sockaddr_in6 *)&addr)->sin6_port);
			}
			જો(inet_ntop(AF_INET6, &((સંરચના sockaddr_in6 *)&addr)->sin6_addr.s6_addr, buf, (socklen_t)len)){
				પાછા 0;
			}
#ifdef WITH_UNIX_SOCKETS
		}નહીંતર જો(addr.ss_family == AF_UNIX){
			સંરચના sockaddr_un un;
			addrlen = કદ(સંરચના sockaddr_un);
			જો(!getsockname(sock, (સંરચના sockaddr *)&un, &addrlen)){
				snprintf(buf, len, "%s", un.sun_path);
			}નહીંતર{
				snprintf(buf, len, "unix-socket");
			}
			પાછા 0;
#endif
		}
	}
	પાછા 1;
}
