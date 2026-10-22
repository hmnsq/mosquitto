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
#אחרת
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#ضمن <errno.h>
#ضمن <fcntl.h>
#ضمن <stdio.h>
#ضمن <string.h>
#ifdef WITH_WRAP
#  include <tcpd.h>
#endif

#ifdef HAVE_NETINET_IN_H
#  include <netinet/in.h>
#endif

#אם defined(WITH_UNIX_SOCKETS) || defined(WITH_TLS)
#  include "sys/stat.h"
#endif

#ifdef WITH_UNIX_SOCKETS
#  ifdef WIN32
#    include <afunix.h>
#  אחרת
#    include <sys/un.h>
#  endif
#endif

#ifdef __QNX__
#  include <net/netbyte.h>
#endif

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "net_mosq.h"
#ضمن "util_mosq.h"

#ifdef WITH_TLS
#  include "tls_mosq.h"
#  include <openssl/err.h>
ساكن صحيح tls_ex_index_context = -1;
ساكن صحيح tls_ex_index_listener = -1;
#endif

#ضمن "sys_tree.h"

/* For EMFILE handling */
ساكن mosq_sock_t spare_sock = INVALID_SOCKET;


فراغ net__broker_init(فراغ)
{
	spare_sock = socket(AF_INET, SOCK_STREAM, 0);
	net__init();
#ifdef WITH_TLS
	net__init_tls();
#endif
}


فراغ net__broker_cleanup(فراغ)
{
	אם(spare_sock != INVALID_SOCKET){
		COMPAT_CLOSE(spare_sock);
		spare_sock = INVALID_SOCKET;
	}
	net__cleanup();
}


ساكن فراغ net__print_error(unsigned صحيح log, ثابت حرف *format_str)
{
	حرف *buf;

#ifdef WIN32
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			عدم, WSAGetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, عدم);

	log__printf(عدم, log, format_str, buf);
	LocalFree(buf);
#אחרת
	buf = strerror(errno);
	log__printf(عدم, log, format_str, buf);
#endif
}


بنية mosquitto *net__socket_accept(بنية mosquitto__listener_sock *listensock)
{
	mosq_sock_t new_sock = INVALID_SOCKET;
	بنية mosquitto *new_context;
#ifdef WITH_TLS
	BIO *bio;
#endif
#ifdef WITH_WRAP
	بنية request_info wrap_req;
	حرف address[1024];
#endif

	new_sock = accept(listensock->sock, عدم, 0);
	אם(new_sock == INVALID_SOCKET){
#ifdef WIN32
		WINDOWS_SET_ERRNO();
		אם(errno == WSAEMFILE){
#אחרת
		אם(errno == EMFILE || errno == ENFILE){
#endif
			/* Close the spare socket, which means we should be able to accept
			 * this connection. Accept it, then close it immediately and create
			 * a new spare_sock. This prevents the situation of ever properly
			 * running out of sockets.
			 * It would be nice to send a "server not available" connack here,
			 * but there are lots of reasons why this would be tricky (TLS
			 * being the big one). */
			COMPAT_CLOSE(spare_sock);
			new_sock = accept(listensock->sock, عدم, 0);
			אם(new_sock != INVALID_SOCKET){
				COMPAT_CLOSE(new_sock);
			}
			spare_sock = socket(AF_INET, SOCK_STREAM, 0);
			log__printf(عدم, MOSQ_LOG_WARNING,
					"Unable to accept new connection, system socket count has been exceeded. Try increasing \"ulimit -n\" or equivalent.");
		}
		החזר عدم;
	}

	metrics__int_inc(mosq_counter_socket_connections, 1);

	אם(net__socket_nonblock(&new_sock)){
		החזר عدم;
	}

#ifdef WITH_WRAP
	/* Use tcpd / libwrap to determine whether a connection is allowed. */
	request_init(&wrap_req, RQ_FILE, new_sock, RQ_DAEMON, "mosquitto", 0);
	fromhost(&wrap_req);
	אם(!hosts_access(&wrap_req)){
		/* Access is denied */
		אם(db.config->connection_messages == true){
			אם(!net__socket_get_address(new_sock, address, 1024, عدم)){
				log__printf(عدم, MOSQ_LOG_NOTICE, "Client connection from %s denied access by tcpd.", address);
			}
		}
		COMPAT_CLOSE(new_sock);
		החזר عدم;
	}
#endif

	אם(db.config->set_tcp_nodelay && listensock->listener->port){
		صحيح flag = 1;
		אם(setsockopt(new_sock, IPPROTO_TCP, TCP_NODELAY, (ثابت فراغ *)&flag, حجم(صحيح)) != 0){
			log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Unable to set TCP_NODELAY.");
		}
	}

	new_context = context__init();
	אם(!new_context){
		COMPAT_CLOSE(new_sock);
		החזר عدم;
	}
	new_context->listener = listensock->listener;
	אם(!new_context->listener){
		context__cleanup(new_context, true);
		החזר عدم;
	}
	new_context->listener->client_count++;

	אם(new_context->listener->enable_proxy_protocol){
		אם(context__init_sock(new_context, new_sock, false) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			החזר عدم;
		}
		אם(new_context->listener->enable_proxy_protocol == 2){
			new_context->transport = mosq_t_proxy_v2;
		}אחרת{
			new_context->transport = mosq_t_proxy_v1;
		}
		new_context->proxy.cmd = -1;
	}אחרת{
		אם(context__init_sock(new_context, new_sock, true) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			החזר عدم;
		}
		بدل(new_context->listener->protocol){
			حالة mp_mqtt:
				new_context->transport = mosq_t_tcp;
				اكسر;
#אם defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			حالة mp_websockets:
				אם(http__context_init(new_context)){
					context__cleanup(new_context, true);
					החזר عدم;
				}
				اكسر;
#endif
			افتراضي:
				context__cleanup(new_context, true);
				החזר عدم;
		}
	}

	אם((new_context->listener->max_connections > 0 && new_context->listener->client_count > new_context->listener->max_connections)
			|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (unsigned صحيح)db.config->global_max_connections)){

		אם(db.config->connection_messages == true){
			log__printf(عدم, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", new_context->address);
		}
		context__cleanup(new_context, true);
		החזר عدم;
	}

#ifdef WITH_TLS
	/* TLS init */
	אם(new_context->listener->ssl_ctx){
		new_context->ssl = SSL_new(new_context->listener->ssl_ctx);
		אם(!new_context->ssl){
			context__cleanup(new_context, true);
			החזר عدم;
		}
		אם(!SSL_set_ex_data(new_context->ssl, tls_ex_index_context, new_context)
				|| !SSL_set_ex_data(new_context->ssl, tls_ex_index_listener, new_context->listener)){
			context__cleanup(new_context, true);
			החזר عدم;
		}
		new_context->want_write = true;
		bio = BIO_new_socket(new_sock, BIO_NOCLOSE);
		SSL_set_bio(new_context->ssl, bio, bio);
		ERR_clear_error();
		SSL_set_accept_state(new_context->ssl);
	}
#endif

	אם(db.config->connection_messages == true
			&& !new_context->listener->enable_proxy_protocol){

		log__printf(عدم, MOSQ_LOG_NOTICE, "New connection from %s:%d on port %d.",
				new_context->address, new_context->remote_port, new_context->listener->port);
	}

	mux__new(new_context);
	keepalive__add(new_context);

	החזר new_context;
}

#ifdef WITH_TLS


ساكن صحيح client_certificate_verify(صحيح preverify_ok, X509_STORE_CTX *ctx)
{
	/* Preverify should check expiry, revocation. */
	אם(preverify_ok == 0){
		SSL *ssl;
		ssl = X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx());
		אם(ssl){
			بنية mosquitto__listener *listener;
			listener = SSL_get_ex_data(ssl, tls_ex_index_listener);

			אם(listener && listener->disable_client_cert_date_checks){
				صحيح err = X509_STORE_CTX_get_error(ctx);
				אם(err == X509_V_ERR_CERT_NOT_YET_VALID || err == X509_V_ERR_CERT_HAS_EXPIRED){
					preverify_ok = 1;
				}
			}
		}
	}
	החזר preverify_ok;
}
#endif

#ifdef FINAL_WITH_TLS_PSK


ساكن unsigned صحيح psk_server_callback(SSL *ssl, ثابت حرف *identity, unsigned حرف *psk, unsigned صحيح max_psk_len)
{
	بنية mosquitto *context;
	بنية mosquitto__listener *listener;
	حرف *psk_key = عدم;
	صحيح len;
	ثابت حرف *psk_hint;

	אם(!identity){
		החזר 0;
	}

	context = SSL_get_ex_data(ssl, tls_ex_index_context);
	אם(!context){
		החזר 0;
	}

	listener = SSL_get_ex_data(ssl, tls_ex_index_listener);
	אם(!listener){
		החזר 0;
	}

	psk_hint = listener->psk_hint;

	/* The hex to BN conversion results in the length halving, so we can pass
	 * max_psk_len*2 as the max hex key here. */
	psk_key = mosquitto_calloc(1, (size_t)max_psk_len*2 + 1);
	אם(!psk_key){
		החזר 0;
	}

	אם(mosquitto_psk_key_get(context, psk_hint, identity, psk_key, (صحيح)max_psk_len*2) != MOSQ_ERR_SUCCESS){
		mosquitto_FREE(psk_key);
		החזר 0;
	}

	len = mosquitto__hex2bin(psk_key, psk, (صحيح)max_psk_len);
	אם(len < 0){
		mosquitto_FREE(psk_key);
		החזר 0;
	}

	אם(listener->use_identity_as_username){
		אם(mosquitto_validate_utf8(identity, (صحيح)strlen(identity))){
			mosquitto_free(psk_key);
			החזר 0;
		}
		context->username = mosquitto_strdup(identity);
		אם(!context->username){
			mosquitto_FREE(psk_key);
			החזר 0;
		}
	}

	mosquitto_FREE(psk_key);
	החזר (unsigned صحيح)len;
}
#endif

#ifdef WITH_TLS


ساكن فراغ tls_keylog_callback(ثابت SSL *ssl, ثابت حرف *line)
{
	UNUSED(ssl);

	אם(db.tls_keylog){
		FILE *fptr;
		fptr = mosquitto_fopen(db.tls_keylog, "at", true);
		אם(fptr){
#ifndef WIN32
			/* Until mosquitto_fopen enforces file permissions on all files,
			 * enforce it here. We can enforce it here, because it isn't a
			 * change of behaviour. */
			بنية stat statbuf;
			אם(fstat(fileno(fptr), &statbuf) < 0
					|| statbuf.st_mode & (S_IRWXG | S_IRWXO)){

				fclose(fptr);
				החזר;
			}
#endif
			fprintf(fptr, "%s\n", line);
			fclose(fptr);
		}אחרת{
#ifndef WIN32
			אם(errno == ELOOP){
				log__printf(عدم, MOSQ_LOG_INFO, "Error: keylog file must not be a symbolic link");
			}אחרת
#endif
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

	אם(tls_ex_index_context == -1){
		tls_ex_index_context = SSL_get_ex_new_index(0, "client context", عدم, عدم, عدم);
	}
	אם(tls_ex_index_listener == -1){
		tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", عدم, عدم, عدم);
	}

	אם(listener->ssl_ctx){
		SSL_CTX_free(listener->ssl_ctx);
		listener->ssl_ctx = عدم;
	}

	listener->ssl_ctx = SSL_CTX_new(TLS_server_method());

	אם(!listener->ssl_ctx){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to create TLS context.");
		החזר MOSQ_ERR_TLS;
	}

#ifdef SSL_OP_NO_TLSv1_3
	אם(db.config->per_listener_settings){
		אם(listener->security_options->psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}אחרת{
		אם(db.config->security_options.psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}
#endif

	אם(listener->tls_version == عدم){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
#ifdef SSL_OP_NO_TLSv1_3
	}אחרת אם(!strcmp(listener->tls_version, "tlsv1.3")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2);
#endif
	}אחרת אם(!strcmp(listener->tls_version, "tlsv1.2")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
	}אחרת אם(!strcmp(listener->tls_version, "tlsv1.1")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1);
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS v1.1 is insecure. It must only be used in the case "
				"where you have devices that cannot be upgraded to use a secure version of TLS. It is "
				"recommended to use as secure a set of ciphers as possible and that will restrict it to "
				"TLS v1.1 only, and to use a separate listener using TLS v1.2 and secure ciphers for your "
				"other devices.");
		log__printf(عدم, MOSQ_LOG_WARNING, "Please be aware that support for TLS v1.1 will go away eventually "
				"and that you should plan now to migrate away from it.");
	}אחרת{
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Unsupported tls_version \"%s\".", listener->tls_version);
		החזר MOSQ_ERR_TLS;
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

	אם(db.tls_keylog){
		log__printf(عدم, MOSQ_LOG_NOTICE, "TLS key logging to '%s' enabled for all listeners.",
				db.tls_keylog);
		log__printf(عدم, MOSQ_LOG_NOTICE, "TLS key logging is for DEBUGGING only.");

		SSL_CTX_set_keylog_callback(listener->ssl_ctx, tls_keylog_callback);
	}

	snprintf(buf, 256, "mosquitto-%d", listener->port);
	SSL_CTX_set_session_id_context(listener->ssl_ctx, (unsigned حرف *)buf, (unsigned صحيح)strlen(buf));

	אם(listener->ciphers){
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, listener->ciphers);
		אם(rc == 0){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			החזר MOSQ_ERR_TLS;
		}
	}אחרת{
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, "DEFAULT:!aNULL:!eNULL:!LOW:!EXPORT:!SSLv2:@STRENGTH");
		אם(rc == 0){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			החזר MOSQ_ERR_TLS;
		}
	}
	אם(listener->ciphers_tls13){
		rc = SSL_CTX_set_ciphersuites(listener->ssl_ctx, listener->ciphers_tls13);
		אם(rc == 0){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to set TLS 1.3 ciphersuites. Check cipher_tls13 list \"%s\".", listener->ciphers_tls13);
			החזר MOSQ_ERR_TLS;
		}
	}

	החזר MOSQ_ERR_SUCCESS;
}
#endif


#ifdef WITH_TLS


ساكن صحيح net__load_crl_file(بنية mosquitto__listener *listener)
{
	X509_STORE *store;
	X509_LOOKUP *lookup;
	صحيح rc;

	store = SSL_CTX_get_cert_store(listener->ssl_ctx);
	אם(!store){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to obtain TLS store.");
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		החזר MOSQ_ERR_TLS;
	}
	lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
	rc = X509_load_crl_file(lookup, listener->crlfile, X509_FILETYPE_PEM);
	אם(rc < 1){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load certificate revocation file \"%s\". Check crlfile.", listener->crlfile);
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		net__print_ssl_error(عدم, عدم);
		החזר MOSQ_ERR_TLS;
	}
	X509_STORE_set_flags(store, X509_V_FLAG_CRL_CHECK);

	החזר MOSQ_ERR_SUCCESS;
}
#endif


صحيح net__load_certificates(بنية mosquitto__listener *listener)
{
#ifdef WITH_TLS
	صحيح rc;

	אם(listener->require_certificate){
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, client_certificate_verify);
	}אחרת{
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_NONE, client_certificate_verify);
	}
	rc = SSL_CTX_use_certificate_chain_file(listener->ssl_ctx, listener->certfile);
	אם(rc != 1){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
		net__print_ssl_error(عدم, عدم);
		החזר MOSQ_ERR_TLS;
	}
	אם(listener->tls_engine == عدم || listener->tls_keyform == mosq_k_pem){
		rc = SSL_CTX_use_PrivateKey_file(listener->ssl_ctx, listener->keyfile, SSL_FILETYPE_PEM);
		אם(rc != 1){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			net__print_ssl_error(عدم, عدم);
			החזר MOSQ_ERR_TLS;
		}
	}
	rc = SSL_CTX_check_private_key(listener->ssl_ctx);
	אם(rc != 1){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Server certificate/key are inconsistent.");
		net__print_ssl_error(عدم, عدم);
		החזר MOSQ_ERR_TLS;
	}
	/* Load CRLs if they exist. */
	אם(listener->crlfile){
		rc = net__load_crl_file(listener);
		אם(rc){
			החזר rc;
		}
	}
#אחרת
	UNUSED(listener);
#endif
	החזר MOSQ_ERR_SUCCESS;
}


#אם defined(WITH_TLS) && !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000


ساكن صحيح net__load_engine(بنية mosquitto__listener *listener)
{
	ENGINE *engine = عدم;
	UI_METHOD *ui_method;
	EVP_PKEY *pkey;

	אם(!listener->tls_engine){
		החזר MOSQ_ERR_SUCCESS;
	}

	engine = ENGINE_by_id(listener->tls_engine);
	אם(!engine){
		log__printf(عدم, MOSQ_LOG_ERR, "Error loading %s engine\n", listener->tls_engine);
		net__print_ssl_error(عدم, عدم);
		החזר MOSQ_ERR_TLS;
	}
	אם(!ENGINE_init(engine)){
		log__printf(عدم, MOSQ_LOG_ERR, "Failed engine initialisation\n");
		net__print_ssl_error(عدم, عدم);
		החזר MOSQ_ERR_TLS;
	}
	ENGINE_set_default(engine, ENGINE_METHOD_ALL);

	אם(listener->tls_keyform == mosq_k_engine){
		ui_method = net__get_ui_method();
		אם(listener->tls_engine_kpass_sha1){
			אם(!ENGINE_ctrl_cmd(engine, ENGINE_SECRET_MODE, ENGINE_SECRET_MODE_SHA, عدم, عدم, 0)){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to set engine secret mode sha");
				net__print_ssl_error(عدم, عدم);
				החזר MOSQ_ERR_TLS;
			}
			אם(!ENGINE_ctrl_cmd(engine, ENGINE_PIN, 0, listener->tls_engine_kpass_sha1, عدم, 0)){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to set engine pin");
				net__print_ssl_error(عدم, عدم);
				החזר MOSQ_ERR_TLS;
			}
			ui_method = عدم;
		}
		pkey = ENGINE_load_private_key(engine, listener->keyfile, ui_method, عدم);
		אם(!pkey){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(عدم, عدم);
			החזר MOSQ_ERR_TLS;
		}
		אם(SSL_CTX_use_PrivateKey(listener->ssl_ctx, pkey) <= 0){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to use engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(عدم, عدم);
			החזר MOSQ_ERR_TLS;
		}
	}
	ENGINE_free(engine); /* release the structural reference from ENGINE_by_id() */

	החזר MOSQ_ERR_SUCCESS;
}
#endif


صحيح net__tls_load_verify(بنية mosquitto__listener *listener)
{
#ifdef WITH_TLS
	صحيح rc;

#  אם OPENSSL_VERSION_NUMBER < 0x30000000L
	אם(listener->cafile || listener->capath){
		rc = SSL_CTX_load_verify_locations(listener->ssl_ctx, listener->cafile, listener->capath);
		אם(rc == 0){
			אם(listener->cafile && listener->capath){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\" and capath \"%s\".", listener->cafile, listener->capath);
			}אחרת אם(listener->cafile){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			}אחרת{
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			}
		}
	}
#  אחרת
	STACK_OF(X509_NAME) *ca_names = عدم;
	אם(listener->cafile || listener->capath){
		ca_names = sk_X509_NAME_new_null();
	}

	אם(listener->cafile){
		rc = SSL_CTX_load_verify_file(listener->ssl_ctx, listener->cafile);
		אם(rc == 0){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			net__print_ssl_error(عدم, عدم);
			החזר MOSQ_ERR_TLS;
		}

		STACK_OF(X509_NAME) *cert_names = SSL_load_client_CA_file(listener->cafile);
		אם(cert_names){
			עבור(صحيح i=0; i<sk_X509_NAME_num(cert_names); i++){
				sk_X509_NAME_push(ca_names, X509_NAME_dup(sk_X509_NAME_value(cert_names, i)));
			}
			sk_X509_NAME_pop_free(cert_names, X509_NAME_free);
		}

	}
	אם(listener->capath){
		rc = SSL_CTX_load_verify_dir(listener->ssl_ctx, listener->capath);
		אם(rc == 0){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			net__print_ssl_error(عدم, عدم);
			sk_X509_NAME_pop_free(ca_names, X509_NAME_free);
			החזר MOSQ_ERR_TLS;
		}

		SSL_add_dir_cert_subjects_to_stack(ca_names, listener->capath);
	}
	אם(ca_names){
		SSL_CTX_set_client_CA_list(listener->ssl_ctx, ca_names);
	}
#  endif

#  אם !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000
	אם(net__load_engine(listener)){
		החזר MOSQ_ERR_TLS;
	}
#  endif
#endif
	החזר net__load_certificates(listener);
}


#אם !defined(WIN32) && !defined(_AIX)


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

	אם(getifaddrs(&ifaddr) < 0){
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		החזר MOSQ_ERR_ERRNO;
	}

	עבור(بنية ifaddrs *ifa=ifaddr; ifa!=عدم; ifa=ifa->ifa_next){
		אם(ifa->ifa_addr == عدم){
			استمر;
		}

		אם(!strcasecmp(listener->bind_interface, ifa->ifa_name)){
			have_interface = true;

			אם(ifa->ifa_addr->sa_family == rp->ai_addr->sa_family){
				אם(rp->ai_addr->sa_family == AF_INET){
					אם(listener->host &&
							memcmp(&((بنية sockaddr_in *)rp->ai_addr)->sin_addr,
							&((بنية sockaddr_in *)ifa->ifa_addr)->sin_addr,
							حجم(بنية in_addr))){

						log__printf(عدم, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						החזר MOSQ_ERR_INVAL;
					}אחרת{
						memcpy(&((بنية sockaddr_in *)rp->ai_addr)->sin_addr,
								&((بنية sockaddr_in *)ifa->ifa_addr)->sin_addr,
								حجم(بنية in_addr));

						freeifaddrs(ifaddr);
						החזר MOSQ_ERR_SUCCESS;
					}
				}אחרת אם(rp->ai_addr->sa_family == AF_INET6){
					אם(listener->host &&
							memcmp(&((بنية sockaddr_in6 *)rp->ai_addr)->sin6_addr,
							&((بنية sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
							حجم(بنية in6_addr))){

						log__printf(عدم, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						החזר MOSQ_ERR_INVAL;
					}אחרת{
						memcpy(&((بنية sockaddr_in6 *)rp->ai_addr)->sin6_addr,
								&((بنية sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
								حجم(بنية in6_addr));

						((بنية sockaddr_in6 *)rp->ai_addr)->sin6_scope_id = ((بنية sockaddr_in6 *)ifa->ifa_addr)->sin6_scope_id;

						freeifaddrs(ifaddr);
						החזר MOSQ_ERR_SUCCESS;
					}
				}
			}
		}
	}
	freeifaddrs(ifaddr);
	אם(have_interface){
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Interface %s does not support %s configuration.",
				listener->bind_interface, rp->ai_addr->sa_family == AF_INET ? "IPv4" : "IPv6");
		החזר MOSQ_ERR_NOT_SUPPORTED;
	}אחרת{
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Interface %s does not exist.", listener->bind_interface);
		החזר MOSQ_ERR_NOT_FOUND;
	}
}
#endif


ساكن صحيح net__socket_listen_tcp(بنية mosquitto__listener *listener)
{
	mosq_sock_t sock = INVALID_SOCKET;
	بنية addrinfo hints;
	بنية addrinfo *ainfo, *rp;
	حرف service[10];
	صحيح rc;
	صحيح ss_opt = 1;
#ifndef WIN32
	bool interface_bound = false;
#endif

	אם(!listener){
		החזר MOSQ_ERR_INVAL;
	}

	snprintf(service, 10, "%d", listener->port);
	memset(&hints, 0, حجم(بنية addrinfo));
	אם(listener->socket_domain){
		hints.ai_family = listener->socket_domain;
	}אחרת{
		hints.ai_family = AF_UNSPEC;
	}
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	rc = getaddrinfo(listener->host, service, &hints, &ainfo);
	אם(rc){
		log__printf(عدم, MOSQ_LOG_ERR, "Error creating listener: %s.", gai_strerror(rc));
		החזר INVALID_SOCKET;
	}

	listener->sock_count = 0;
	listener->socks = عدم;

	עבור(rp = ainfo; rp; rp = rp->ai_next){
		אם(rp->ai_family == AF_INET){
			log__printf(عدم, MOSQ_LOG_INFO, "Opening ipv4 listen socket on port %d.", ntohs(((بنية sockaddr_in *)rp->ai_addr)->sin_port));
		}אחרת אם(rp->ai_family == AF_INET6){
			log__printf(عدم, MOSQ_LOG_INFO, "Opening ipv6 listen socket on port %d.", ntohs(((بنية sockaddr_in6 *)rp->ai_addr)->sin6_port));
		}אחרת{
			استمر;
		}

		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		אם(sock == INVALID_SOCKET){
			net__print_error(MOSQ_LOG_WARNING, "Warning: %s");
			استمر;
		}
		listener->sock_count++;
		listener->socks = mosquitto_realloc(listener->socks, حجم(mosq_sock_t)*(size_t)listener->sock_count);
		אם(!listener->socks){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			החזר MOSQ_ERR_NOMEM;
		}
		listener->socks[listener->sock_count-1] = sock;

#ifndef WIN32
		ss_opt = 1;
		/* Unimportant if this fails */
		(فراغ)setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (ثابت فراغ *)&ss_opt, حجم(ss_opt));
#endif
#ifdef IPV6_V6ONLY
		ss_opt = 1;
		(فراغ)setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (ثابت فراغ *)&ss_opt, حجم(ss_opt));
#endif

		אם(net__socket_nonblock(&sock)){
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			החזר 1;
		}

#אם !defined(WIN32) && !defined(_AIX)
		אם(listener->bind_interface){
			/* It might be possible that an interface does not support all relevant sa_families.
			 * We should successfully find at least one. */
			rc = net__bind_interface(listener, rp);
			אם(rc){
				COMPAT_CLOSE(sock);
				listener->sock_count--;
				אם(rc == MOSQ_ERR_NOT_FOUND || rc == MOSQ_ERR_INVAL){
					freeaddrinfo(ainfo);
					החזר rc;
				}אחרת{
					استمر;
				}
			}
			interface_bound = true;
		}
#endif

		אם(bind(sock, rp->ai_addr, rp->ai_addrlen) == -1){
#אם defined(__linux__)
			אם(errno == EACCES){
				log__printf(عدم, MOSQ_LOG_ERR, "If you are trying to bind to a privileged port (<1024), try using setcap and do not start the broker as root:");
				log__printf(عدم, MOSQ_LOG_ERR, "    sudo setcap 'CAP_NET_BIND_SERVICE=+ep /usr/sbin/mosquitto'");
			}
#endif
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			COMPAT_CLOSE(sock);
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			החזר 1;
		}

		אם(listen(sock, 100) == -1){
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			mosquitto_FREE(listener->socks);
			החזר 1;
		}
	}
	freeaddrinfo(ainfo);

#ifndef WIN32
	אם(listener->bind_interface && !interface_bound){
		mosquitto_FREE(listener->socks);
		החזר 1;
	}
#endif

	החזר 0;
}


#ifdef WITH_UNIX_SOCKETS


ساكن صحيح net__socket_listen_unix(بنية mosquitto__listener *listener)
{
	بنية sockaddr_un addr;
	mosq_sock_t sock = INVALID_SOCKET;
	صحيح rc;
#ifndef WIN32
	mode_t old_mask;
#endif

	אם(listener->unix_socket_path == عدم){
		החזר MOSQ_ERR_INVAL;
	}
	אם(strlen(listener->unix_socket_path) > حجم(addr.sun_path)-1){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Path to unix socket is too long \"%s\".", listener->unix_socket_path);
		החזר MOSQ_ERR_INVAL;
	}

#ifdef WIN32
	DeleteFile(listener->unix_socket_path);
#אחרת
	unlink(listener->unix_socket_path);
#endif
	log__printf(عدم, MOSQ_LOG_INFO, "Opening unix listen socket on path %s.", listener->unix_socket_path);
	memset(&addr, 0, حجم(بنية sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, listener->unix_socket_path, حجم(addr.sun_path)-1);

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	אם(sock == INVALID_SOCKET){
		net__print_error(MOSQ_LOG_ERR, "Error creating unix socket: %s");
		החזר 1;
	}
	listener->sock_count++;
	listener->socks = mosquitto_realloc(listener->socks, حجم(mosq_sock_t)*(size_t)listener->sock_count);
	אם(!listener->socks){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		COMPAT_CLOSE(sock);
		החזר MOSQ_ERR_NOMEM;
	}
	listener->socks[listener->sock_count-1] = sock;


#ifndef WIN32
	old_mask = umask(0007);
#endif
	rc = bind(sock, (بنية sockaddr *)&addr, حجم(بنية sockaddr_un));
#ifndef WIN32
	umask(old_mask);
#endif

	אם(rc == -1){
		net__print_error(MOSQ_LOG_ERR, "Error binding unix socket: %s");
		החזר 1;
	}

	אם(listen(sock, 10) == -1){
		net__print_error(MOSQ_LOG_ERR, "Error listening to unix socket: %s");
		החזר 1;
	}

	אם(net__socket_nonblock(&sock)){
		החזר 1;
	}

	החזר 0;
}
#endif


/* Creates a socket and listens on port 'port'.
 * Returns 1 on failure
 * Returns 0 on success.
 */
صحيح net__socket_listen(بنية mosquitto__listener *listener)
{
	صحيح rc;

	אם(!listener){
		החזר MOSQ_ERR_INVAL;
	}

#ifdef WITH_UNIX_SOCKETS
	אם(listener->port == 0 && listener->unix_socket_path != عدم){
		rc = net__socket_listen_unix(listener);
	}אחרת
#endif
	{
		rc = net__socket_listen_tcp(listener);
	}
	אם(rc){
		החזר rc;
	}

	/* We need to have at least one working socket. */
	אם(listener->sock_count > 0){
#ifdef WITH_TLS
		אם(listener->certfile && listener->keyfile){
			אם(net__tls_server_ctx(listener)){
				החזר 1;
			}

			אם(net__tls_load_verify(listener)){
				החזר 1;
			}
		}
#  ifdef FINAL_WITH_TLS_PSK
		אם(listener->psk_hint){
			אם(listener->certfile == عدم || listener->keyfile == عدم){
				אם(net__tls_server_ctx(listener)){
					החזר 1;
				}
			}
			SSL_CTX_set_psk_server_callback(listener->ssl_ctx, psk_server_callback);
			אם(listener->psk_hint){
				rc = SSL_CTX_use_psk_identity_hint(listener->ssl_ctx, listener->psk_hint);
				אם(rc == 0){
					log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to set TLS PSK hint.");
					net__print_ssl_error(عدم, عدم);
					החזר 1;
				}
			}
		}
#  endif /* FINAL_WITH_TLS_PSK */
		אם(tls_ex_index_context == -1){
			tls_ex_index_context = SSL_get_ex_new_index(0, "client context", عدم, عدم, عدم);
		}
		אם(tls_ex_index_listener == -1){
			tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", عدم, عدم, عدم);
		}
#endif /* WITH_TLS */
		החזר 0;
	}אחרת{
		החזר 1;
	}
}


صحيح net__socket_get_address(mosq_sock_t sock, حرف *buf, size_t len, uint16_t *remote_port)
{
	بنية sockaddr_storage addr;
	socklen_t addrlen;

	memset(&addr, 0, حجم(بنية sockaddr_storage));
	addrlen = حجم(addr);
	אם(!getpeername(sock, (بنية sockaddr *)&addr, &addrlen)){
		אם(addr.ss_family == AF_INET){
			אם(remote_port){
				*remote_port = ntohs(((بنية sockaddr_in *)&addr)->sin_port);
			}
			אם(inet_ntop(AF_INET, &((بنية sockaddr_in *)&addr)->sin_addr.s_addr, buf, (socklen_t)len)){
				החזר 0;
			}
		}אחרת אם(addr.ss_family == AF_INET6){
			אם(remote_port){
				*remote_port = ntohs(((بنية sockaddr_in6 *)&addr)->sin6_port);
			}
			אם(inet_ntop(AF_INET6, &((بنية sockaddr_in6 *)&addr)->sin6_addr.s6_addr, buf, (socklen_t)len)){
				החזר 0;
			}
#ifdef WITH_UNIX_SOCKETS
		}אחרת אם(addr.ss_family == AF_UNIX){
			بنية sockaddr_un un;
			addrlen = حجم(بنية sockaddr_un);
			אם(!getsockname(sock, (بنية sockaddr *)&un, &addrlen)){
				snprintf(buf, len, "%s", un.sun_path);
			}אחרת{
				snprintf(buf, len, "unix-socket");
			}
			החזר 0;
#endif
		}
	}
	החזר 1;
}
