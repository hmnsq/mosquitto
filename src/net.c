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
#وإلا
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

#𐤀𐤌 defined(WITH_UNIX_SOCKETS) || defined(WITH_TLS)
#  include "sys/stat.h"
#endif

#ifdef WITH_UNIX_SOCKETS
#  ifdef WIN32
#    include <afunix.h>
#  وإلا
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
	𐤀𐤌(spare_sock != INVALID_SOCKET){
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
#وإلا
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
	𐤀𐤌(new_sock == INVALID_SOCKET){
#ifdef WIN32
		WINDOWS_SET_ERRNO();
		𐤀𐤌(errno == WSAEMFILE){
#وإلا
		𐤀𐤌(errno == EMFILE || errno == ENFILE){
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
			𐤀𐤌(new_sock != INVALID_SOCKET){
				COMPAT_CLOSE(new_sock);
			}
			spare_sock = socket(AF_INET, SOCK_STREAM, 0);
			log__printf(عدم, MOSQ_LOG_WARNING,
					"Unable to accept new connection, system socket count has been exceeded. Try increasing \"ulimit -n\" or equivalent.");
		}
		أرجع عدم;
	}

	metrics__int_inc(mosq_counter_socket_connections, 1);

	𐤀𐤌(net__socket_nonblock(&new_sock)){
		أرجع عدم;
	}

#ifdef WITH_WRAP
	/* Use tcpd / libwrap to determine whether a connection is allowed. */
	request_init(&wrap_req, RQ_FILE, new_sock, RQ_DAEMON, "mosquitto", 0);
	fromhost(&wrap_req);
	𐤀𐤌(!hosts_access(&wrap_req)){
		/* Access is denied */
		𐤀𐤌(db.config->connection_messages == true){
			𐤀𐤌(!net__socket_get_address(new_sock, address, 1024, عدم)){
				log__printf(عدم, MOSQ_LOG_NOTICE, "Client connection from %s denied access by tcpd.", address);
			}
		}
		COMPAT_CLOSE(new_sock);
		أرجع عدم;
	}
#endif

	𐤀𐤌(db.config->set_tcp_nodelay && listensock->listener->port){
		صحيح flag = 1;
		𐤀𐤌(setsockopt(new_sock, IPPROTO_TCP, TCP_NODELAY, (ثابت فراغ *)&flag, حجم(صحيح)) != 0){
			log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Unable to set TCP_NODELAY.");
		}
	}

	new_context = context__init();
	𐤀𐤌(!new_context){
		COMPAT_CLOSE(new_sock);
		أرجع عدم;
	}
	new_context->listener = listensock->listener;
	𐤀𐤌(!new_context->listener){
		context__cleanup(new_context, true);
		أرجع عدم;
	}
	new_context->listener->client_count++;

	𐤀𐤌(new_context->listener->enable_proxy_protocol){
		𐤀𐤌(context__init_sock(new_context, new_sock, false) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			أرجع عدم;
		}
		𐤀𐤌(new_context->listener->enable_proxy_protocol == 2){
			new_context->transport = mosq_t_proxy_v2;
		}وإلا{
			new_context->transport = mosq_t_proxy_v1;
		}
		new_context->proxy.cmd = -1;
	}وإلا{
		𐤀𐤌(context__init_sock(new_context, new_sock, true) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			أرجع عدم;
		}
		بدل(new_context->listener->protocol){
			حالة mp_mqtt:
				new_context->transport = mosq_t_tcp;
				اكسر;
#𐤀𐤌 defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			حالة mp_websockets:
				𐤀𐤌(http__context_init(new_context)){
					context__cleanup(new_context, true);
					أرجع عدم;
				}
				اكسر;
#endif
			افتراضي:
				context__cleanup(new_context, true);
				أرجع عدم;
		}
	}

	𐤀𐤌((new_context->listener->max_connections > 0 && new_context->listener->client_count > new_context->listener->max_connections)
			|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (unsigned صحيح)db.config->global_max_connections)){

		𐤀𐤌(db.config->connection_messages == true){
			log__printf(عدم, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", new_context->address);
		}
		context__cleanup(new_context, true);
		أرجع عدم;
	}

#ifdef WITH_TLS
	/* TLS init */
	𐤀𐤌(new_context->listener->ssl_ctx){
		new_context->ssl = SSL_new(new_context->listener->ssl_ctx);
		𐤀𐤌(!new_context->ssl){
			context__cleanup(new_context, true);
			أرجع عدم;
		}
		𐤀𐤌(!SSL_set_ex_data(new_context->ssl, tls_ex_index_context, new_context)
				|| !SSL_set_ex_data(new_context->ssl, tls_ex_index_listener, new_context->listener)){
			context__cleanup(new_context, true);
			أرجع عدم;
		}
		new_context->want_write = true;
		bio = BIO_new_socket(new_sock, BIO_NOCLOSE);
		SSL_set_bio(new_context->ssl, bio, bio);
		ERR_clear_error();
		SSL_set_accept_state(new_context->ssl);
	}
#endif

	𐤀𐤌(db.config->connection_messages == true
			&& !new_context->listener->enable_proxy_protocol){

		log__printf(عدم, MOSQ_LOG_NOTICE, "New connection from %s:%d on port %d.",
				new_context->address, new_context->remote_port, new_context->listener->port);
	}

	mux__new(new_context);
	keepalive__add(new_context);

	أرجع new_context;
}

#ifdef WITH_TLS


ساكن صحيح client_certificate_verify(صحيح preverify_ok, X509_STORE_CTX *ctx)
{
	/* Preverify should check expiry, revocation. */
	𐤀𐤌(preverify_ok == 0){
		SSL *ssl;
		ssl = X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx());
		𐤀𐤌(ssl){
			بنية mosquitto__listener *listener;
			listener = SSL_get_ex_data(ssl, tls_ex_index_listener);

			𐤀𐤌(listener && listener->disable_client_cert_date_checks){
				صحيح err = X509_STORE_CTX_get_error(ctx);
				𐤀𐤌(err == X509_V_ERR_CERT_NOT_YET_VALID || err == X509_V_ERR_CERT_HAS_EXPIRED){
					preverify_ok = 1;
				}
			}
		}
	}
	أرجع preverify_ok;
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

	𐤀𐤌(!identity){
		أرجع 0;
	}

	context = SSL_get_ex_data(ssl, tls_ex_index_context);
	𐤀𐤌(!context){
		أرجع 0;
	}

	listener = SSL_get_ex_data(ssl, tls_ex_index_listener);
	𐤀𐤌(!listener){
		أرجع 0;
	}

	psk_hint = listener->psk_hint;

	/* The hex to BN conversion results in the length halving, so we can pass
	 * max_psk_len*2 as the max hex key here. */
	psk_key = mosquitto_calloc(1, (size_t)max_psk_len*2 + 1);
	𐤀𐤌(!psk_key){
		أرجع 0;
	}

	𐤀𐤌(mosquitto_psk_key_get(context, psk_hint, identity, psk_key, (صحيح)max_psk_len*2) != MOSQ_ERR_SUCCESS){
		mosquitto_FREE(psk_key);
		أرجع 0;
	}

	len = mosquitto__hex2bin(psk_key, psk, (صحيح)max_psk_len);
	𐤀𐤌(len < 0){
		mosquitto_FREE(psk_key);
		أرجع 0;
	}

	𐤀𐤌(listener->use_identity_as_username){
		𐤀𐤌(mosquitto_validate_utf8(identity, (صحيح)strlen(identity))){
			mosquitto_free(psk_key);
			أرجع 0;
		}
		context->username = mosquitto_strdup(identity);
		𐤀𐤌(!context->username){
			mosquitto_FREE(psk_key);
			أرجع 0;
		}
	}

	mosquitto_FREE(psk_key);
	أرجع (unsigned صحيح)len;
}
#endif

#ifdef WITH_TLS


ساكن فراغ tls_keylog_callback(ثابت SSL *ssl, ثابت حرف *line)
{
	UNUSED(ssl);

	𐤀𐤌(db.tls_keylog){
		FILE *fptr;
		fptr = mosquitto_fopen(db.tls_keylog, "at", true);
		𐤀𐤌(fptr){
#ifndef WIN32
			/* Until mosquitto_fopen enforces file permissions on all files,
			 * enforce it here. We can enforce it here, because it isn't a
			 * change of behaviour. */
			بنية stat statbuf;
			𐤀𐤌(fstat(fileno(fptr), &statbuf) < 0
					|| statbuf.st_mode & (S_IRWXG | S_IRWXO)){

				fclose(fptr);
				أرجع;
			}
#endif
			fprintf(fptr, "%s\n", line);
			fclose(fptr);
		}وإلا{
#ifndef WIN32
			𐤀𐤌(errno == ELOOP){
				log__printf(عدم, MOSQ_LOG_INFO, "Error: keylog file must not be a symbolic link");
			}وإلا
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

	𐤀𐤌(tls_ex_index_context == -1){
		tls_ex_index_context = SSL_get_ex_new_index(0, "client context", عدم, عدم, عدم);
	}
	𐤀𐤌(tls_ex_index_listener == -1){
		tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", عدم, عدم, عدم);
	}

	𐤀𐤌(listener->ssl_ctx){
		SSL_CTX_free(listener->ssl_ctx);
		listener->ssl_ctx = عدم;
	}

	listener->ssl_ctx = SSL_CTX_new(TLS_server_method());

	𐤀𐤌(!listener->ssl_ctx){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to create TLS context.");
		أرجع MOSQ_ERR_TLS;
	}

#ifdef SSL_OP_NO_TLSv1_3
	𐤀𐤌(db.config->per_listener_settings){
		𐤀𐤌(listener->security_options->psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}وإلا{
		𐤀𐤌(db.config->security_options.psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}
#endif

	𐤀𐤌(listener->tls_version == عدم){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
#ifdef SSL_OP_NO_TLSv1_3
	}وإلا 𐤀𐤌(!strcmp(listener->tls_version, "tlsv1.3")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2);
#endif
	}وإلا 𐤀𐤌(!strcmp(listener->tls_version, "tlsv1.2")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
	}وإلا 𐤀𐤌(!strcmp(listener->tls_version, "tlsv1.1")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1);
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: TLS v1.1 is insecure. It must only be used in the case "
				"where you have devices that cannot be upgraded to use a secure version of TLS. It is "
				"recommended to use as secure a set of ciphers as possible and that will restrict it to "
				"TLS v1.1 only, and to use a separate listener using TLS v1.2 and secure ciphers for your "
				"other devices.");
		log__printf(عدم, MOSQ_LOG_WARNING, "Please be aware that support for TLS v1.1 will go away eventually "
				"and that you should plan now to migrate away from it.");
	}وإلا{
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Unsupported tls_version \"%s\".", listener->tls_version);
		أرجع MOSQ_ERR_TLS;
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

	𐤀𐤌(db.tls_keylog){
		log__printf(عدم, MOSQ_LOG_NOTICE, "TLS key logging to '%s' enabled for all listeners.",
				db.tls_keylog);
		log__printf(عدم, MOSQ_LOG_NOTICE, "TLS key logging is for DEBUGGING only.");

		SSL_CTX_set_keylog_callback(listener->ssl_ctx, tls_keylog_callback);
	}

	snprintf(buf, 256, "mosquitto-%d", listener->port);
	SSL_CTX_set_session_id_context(listener->ssl_ctx, (unsigned حرف *)buf, (unsigned صحيح)strlen(buf));

	𐤀𐤌(listener->ciphers){
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, listener->ciphers);
		𐤀𐤌(rc == 0){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			أرجع MOSQ_ERR_TLS;
		}
	}وإلا{
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, "DEFAULT:!aNULL:!eNULL:!LOW:!EXPORT:!SSLv2:@STRENGTH");
		𐤀𐤌(rc == 0){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			أرجع MOSQ_ERR_TLS;
		}
	}
	𐤀𐤌(listener->ciphers_tls13){
		rc = SSL_CTX_set_ciphersuites(listener->ssl_ctx, listener->ciphers_tls13);
		𐤀𐤌(rc == 0){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to set TLS 1.3 ciphersuites. Check cipher_tls13 list \"%s\".", listener->ciphers_tls13);
			أرجع MOSQ_ERR_TLS;
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}
#endif


#ifdef WITH_TLS


ساكن صحيح net__load_crl_file(بنية mosquitto__listener *listener)
{
	X509_STORE *store;
	X509_LOOKUP *lookup;
	صحيح rc;

	store = SSL_CTX_get_cert_store(listener->ssl_ctx);
	𐤀𐤌(!store){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to obtain TLS store.");
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		أرجع MOSQ_ERR_TLS;
	}
	lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
	rc = X509_load_crl_file(lookup, listener->crlfile, X509_FILETYPE_PEM);
	𐤀𐤌(rc < 1){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load certificate revocation file \"%s\". Check crlfile.", listener->crlfile);
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		net__print_ssl_error(عدم, عدم);
		أرجع MOSQ_ERR_TLS;
	}
	X509_STORE_set_flags(store, X509_V_FLAG_CRL_CHECK);

	أرجع MOSQ_ERR_SUCCESS;
}
#endif


صحيح net__load_certificates(بنية mosquitto__listener *listener)
{
#ifdef WITH_TLS
	صحيح rc;

	𐤀𐤌(listener->require_certificate){
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, client_certificate_verify);
	}وإلا{
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_NONE, client_certificate_verify);
	}
	rc = SSL_CTX_use_certificate_chain_file(listener->ssl_ctx, listener->certfile);
	𐤀𐤌(rc != 1){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
		net__print_ssl_error(عدم, عدم);
		أرجع MOSQ_ERR_TLS;
	}
	𐤀𐤌(listener->tls_engine == عدم || listener->tls_keyform == mosq_k_pem){
		rc = SSL_CTX_use_PrivateKey_file(listener->ssl_ctx, listener->keyfile, SSL_FILETYPE_PEM);
		𐤀𐤌(rc != 1){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			net__print_ssl_error(عدم, عدم);
			أرجع MOSQ_ERR_TLS;
		}
	}
	rc = SSL_CTX_check_private_key(listener->ssl_ctx);
	𐤀𐤌(rc != 1){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Server certificate/key are inconsistent.");
		net__print_ssl_error(عدم, عدم);
		أرجع MOSQ_ERR_TLS;
	}
	/* Load CRLs if they exist. */
	𐤀𐤌(listener->crlfile){
		rc = net__load_crl_file(listener);
		𐤀𐤌(rc){
			أرجع rc;
		}
	}
#وإلا
	UNUSED(listener);
#endif
	أرجع MOSQ_ERR_SUCCESS;
}


#𐤀𐤌 defined(WITH_TLS) && !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000


ساكن صحيح net__load_engine(بنية mosquitto__listener *listener)
{
	ENGINE *engine = عدم;
	UI_METHOD *ui_method;
	EVP_PKEY *pkey;

	𐤀𐤌(!listener->tls_engine){
		أرجع MOSQ_ERR_SUCCESS;
	}

	engine = ENGINE_by_id(listener->tls_engine);
	𐤀𐤌(!engine){
		log__printf(عدم, MOSQ_LOG_ERR, "Error loading %s engine\n", listener->tls_engine);
		net__print_ssl_error(عدم, عدم);
		أرجع MOSQ_ERR_TLS;
	}
	𐤀𐤌(!ENGINE_init(engine)){
		log__printf(عدم, MOSQ_LOG_ERR, "Failed engine initialisation\n");
		net__print_ssl_error(عدم, عدم);
		أرجع MOSQ_ERR_TLS;
	}
	ENGINE_set_default(engine, ENGINE_METHOD_ALL);

	𐤀𐤌(listener->tls_keyform == mosq_k_engine){
		ui_method = net__get_ui_method();
		𐤀𐤌(listener->tls_engine_kpass_sha1){
			𐤀𐤌(!ENGINE_ctrl_cmd(engine, ENGINE_SECRET_MODE, ENGINE_SECRET_MODE_SHA, عدم, عدم, 0)){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to set engine secret mode sha");
				net__print_ssl_error(عدم, عدم);
				أرجع MOSQ_ERR_TLS;
			}
			𐤀𐤌(!ENGINE_ctrl_cmd(engine, ENGINE_PIN, 0, listener->tls_engine_kpass_sha1, عدم, 0)){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to set engine pin");
				net__print_ssl_error(عدم, عدم);
				أرجع MOSQ_ERR_TLS;
			}
			ui_method = عدم;
		}
		pkey = ENGINE_load_private_key(engine, listener->keyfile, ui_method, عدم);
		𐤀𐤌(!pkey){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(عدم, عدم);
			أرجع MOSQ_ERR_TLS;
		}
		𐤀𐤌(SSL_CTX_use_PrivateKey(listener->ssl_ctx, pkey) <= 0){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to use engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(عدم, عدم);
			أرجع MOSQ_ERR_TLS;
		}
	}
	ENGINE_free(engine); /* release the structural reference from ENGINE_by_id() */

	أرجع MOSQ_ERR_SUCCESS;
}
#endif


صحيح net__tls_load_verify(بنية mosquitto__listener *listener)
{
#ifdef WITH_TLS
	صحيح rc;

#  𐤀𐤌 OPENSSL_VERSION_NUMBER < 0x30000000L
	𐤀𐤌(listener->cafile || listener->capath){
		rc = SSL_CTX_load_verify_locations(listener->ssl_ctx, listener->cafile, listener->capath);
		𐤀𐤌(rc == 0){
			𐤀𐤌(listener->cafile && listener->capath){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\" and capath \"%s\".", listener->cafile, listener->capath);
			}وإلا 𐤀𐤌(listener->cafile){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			}وإلا{
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			}
		}
	}
#  وإلا
	STACK_OF(X509_NAME) *ca_names = عدم;
	𐤀𐤌(listener->cafile || listener->capath){
		ca_names = sk_X509_NAME_new_null();
	}

	𐤀𐤌(listener->cafile){
		rc = SSL_CTX_load_verify_file(listener->ssl_ctx, listener->cafile);
		𐤀𐤌(rc == 0){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			net__print_ssl_error(عدم, عدم);
			أرجع MOSQ_ERR_TLS;
		}

		STACK_OF(X509_NAME) *cert_names = SSL_load_client_CA_file(listener->cafile);
		𐤀𐤌(cert_names){
			لكل(صحيح i=0; i<sk_X509_NAME_num(cert_names); i++){
				sk_X509_NAME_push(ca_names, X509_NAME_dup(sk_X509_NAME_value(cert_names, i)));
			}
			sk_X509_NAME_pop_free(cert_names, X509_NAME_free);
		}

	}
	𐤀𐤌(listener->capath){
		rc = SSL_CTX_load_verify_dir(listener->ssl_ctx, listener->capath);
		𐤀𐤌(rc == 0){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			net__print_ssl_error(عدم, عدم);
			sk_X509_NAME_pop_free(ca_names, X509_NAME_free);
			أرجع MOSQ_ERR_TLS;
		}

		SSL_add_dir_cert_subjects_to_stack(ca_names, listener->capath);
	}
	𐤀𐤌(ca_names){
		SSL_CTX_set_client_CA_list(listener->ssl_ctx, ca_names);
	}
#  endif

#  𐤀𐤌 !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000
	𐤀𐤌(net__load_engine(listener)){
		أرجع MOSQ_ERR_TLS;
	}
#  endif
#endif
	أرجع net__load_certificates(listener);
}


#𐤀𐤌 !defined(WIN32) && !defined(_AIX)


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

	𐤀𐤌(getifaddrs(&ifaddr) < 0){
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		أرجع MOSQ_ERR_ERRNO;
	}

	لكل(بنية ifaddrs *ifa=ifaddr; ifa!=عدم; ifa=ifa->ifa_next){
		𐤀𐤌(ifa->ifa_addr == عدم){
			استمر;
		}

		𐤀𐤌(!strcasecmp(listener->bind_interface, ifa->ifa_name)){
			have_interface = true;

			𐤀𐤌(ifa->ifa_addr->sa_family == rp->ai_addr->sa_family){
				𐤀𐤌(rp->ai_addr->sa_family == AF_INET){
					𐤀𐤌(listener->host &&
							memcmp(&((بنية sockaddr_in *)rp->ai_addr)->sin_addr,
							&((بنية sockaddr_in *)ifa->ifa_addr)->sin_addr,
							حجم(بنية in_addr))){

						log__printf(عدم, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						أرجع MOSQ_ERR_INVAL;
					}وإلا{
						memcpy(&((بنية sockaddr_in *)rp->ai_addr)->sin_addr,
								&((بنية sockaddr_in *)ifa->ifa_addr)->sin_addr,
								حجم(بنية in_addr));

						freeifaddrs(ifaddr);
						أرجع MOSQ_ERR_SUCCESS;
					}
				}وإلا 𐤀𐤌(rp->ai_addr->sa_family == AF_INET6){
					𐤀𐤌(listener->host &&
							memcmp(&((بنية sockaddr_in6 *)rp->ai_addr)->sin6_addr,
							&((بنية sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
							حجم(بنية in6_addr))){

						log__printf(عدم, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						أرجع MOSQ_ERR_INVAL;
					}وإلا{
						memcpy(&((بنية sockaddr_in6 *)rp->ai_addr)->sin6_addr,
								&((بنية sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
								حجم(بنية in6_addr));

						((بنية sockaddr_in6 *)rp->ai_addr)->sin6_scope_id = ((بنية sockaddr_in6 *)ifa->ifa_addr)->sin6_scope_id;

						freeifaddrs(ifaddr);
						أرجع MOSQ_ERR_SUCCESS;
					}
				}
			}
		}
	}
	freeifaddrs(ifaddr);
	𐤀𐤌(have_interface){
		log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Interface %s does not support %s configuration.",
				listener->bind_interface, rp->ai_addr->sa_family == AF_INET ? "IPv4" : "IPv6");
		أرجع MOSQ_ERR_NOT_SUPPORTED;
	}وإلا{
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Interface %s does not exist.", listener->bind_interface);
		أرجع MOSQ_ERR_NOT_FOUND;
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

	𐤀𐤌(!listener){
		أرجع MOSQ_ERR_INVAL;
	}

	snprintf(service, 10, "%d", listener->port);
	memset(&hints, 0, حجم(بنية addrinfo));
	𐤀𐤌(listener->socket_domain){
		hints.ai_family = listener->socket_domain;
	}وإلا{
		hints.ai_family = AF_UNSPEC;
	}
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	rc = getaddrinfo(listener->host, service, &hints, &ainfo);
	𐤀𐤌(rc){
		log__printf(عدم, MOSQ_LOG_ERR, "Error creating listener: %s.", gai_strerror(rc));
		أرجع INVALID_SOCKET;
	}

	listener->sock_count = 0;
	listener->socks = عدم;

	لكل(rp = ainfo; rp; rp = rp->ai_next){
		𐤀𐤌(rp->ai_family == AF_INET){
			log__printf(عدم, MOSQ_LOG_INFO, "Opening ipv4 listen socket on port %d.", ntohs(((بنية sockaddr_in *)rp->ai_addr)->sin_port));
		}وإلا 𐤀𐤌(rp->ai_family == AF_INET6){
			log__printf(عدم, MOSQ_LOG_INFO, "Opening ipv6 listen socket on port %d.", ntohs(((بنية sockaddr_in6 *)rp->ai_addr)->sin6_port));
		}وإلا{
			استمر;
		}

		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		𐤀𐤌(sock == INVALID_SOCKET){
			net__print_error(MOSQ_LOG_WARNING, "Warning: %s");
			استمر;
		}
		listener->sock_count++;
		listener->socks = mosquitto_realloc(listener->socks, حجم(mosq_sock_t)*(size_t)listener->sock_count);
		𐤀𐤌(!listener->socks){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			أرجع MOSQ_ERR_NOMEM;
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

		𐤀𐤌(net__socket_nonblock(&sock)){
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			أرجع 1;
		}

#𐤀𐤌 !defined(WIN32) && !defined(_AIX)
		𐤀𐤌(listener->bind_interface){
			/* It might be possible that an interface does not support all relevant sa_families.
			 * We should successfully find at least one. */
			rc = net__bind_interface(listener, rp);
			𐤀𐤌(rc){
				COMPAT_CLOSE(sock);
				listener->sock_count--;
				𐤀𐤌(rc == MOSQ_ERR_NOT_FOUND || rc == MOSQ_ERR_INVAL){
					freeaddrinfo(ainfo);
					أرجع rc;
				}وإلا{
					استمر;
				}
			}
			interface_bound = true;
		}
#endif

		𐤀𐤌(bind(sock, rp->ai_addr, rp->ai_addrlen) == -1){
#𐤀𐤌 defined(__linux__)
			𐤀𐤌(errno == EACCES){
				log__printf(عدم, MOSQ_LOG_ERR, "If you are trying to bind to a privileged port (<1024), try using setcap and do not start the broker as root:");
				log__printf(عدم, MOSQ_LOG_ERR, "    sudo setcap 'CAP_NET_BIND_SERVICE=+ep /usr/sbin/mosquitto'");
			}
#endif
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			COMPAT_CLOSE(sock);
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			أرجع 1;
		}

		𐤀𐤌(listen(sock, 100) == -1){
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			mosquitto_FREE(listener->socks);
			أرجع 1;
		}
	}
	freeaddrinfo(ainfo);

#ifndef WIN32
	𐤀𐤌(listener->bind_interface && !interface_bound){
		mosquitto_FREE(listener->socks);
		أرجع 1;
	}
#endif

	أرجع 0;
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

	𐤀𐤌(listener->unix_socket_path == عدم){
		أرجع MOSQ_ERR_INVAL;
	}
	𐤀𐤌(strlen(listener->unix_socket_path) > حجم(addr.sun_path)-1){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Path to unix socket is too long \"%s\".", listener->unix_socket_path);
		أرجع MOSQ_ERR_INVAL;
	}

#ifdef WIN32
	DeleteFile(listener->unix_socket_path);
#وإلا
	unlink(listener->unix_socket_path);
#endif
	log__printf(عدم, MOSQ_LOG_INFO, "Opening unix listen socket on path %s.", listener->unix_socket_path);
	memset(&addr, 0, حجم(بنية sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, listener->unix_socket_path, حجم(addr.sun_path)-1);

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	𐤀𐤌(sock == INVALID_SOCKET){
		net__print_error(MOSQ_LOG_ERR, "Error creating unix socket: %s");
		أرجع 1;
	}
	listener->sock_count++;
	listener->socks = mosquitto_realloc(listener->socks, حجم(mosq_sock_t)*(size_t)listener->sock_count);
	𐤀𐤌(!listener->socks){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		COMPAT_CLOSE(sock);
		أرجع MOSQ_ERR_NOMEM;
	}
	listener->socks[listener->sock_count-1] = sock;


#ifndef WIN32
	old_mask = umask(0007);
#endif
	rc = bind(sock, (بنية sockaddr *)&addr, حجم(بنية sockaddr_un));
#ifndef WIN32
	umask(old_mask);
#endif

	𐤀𐤌(rc == -1){
		net__print_error(MOSQ_LOG_ERR, "Error binding unix socket: %s");
		أرجع 1;
	}

	𐤀𐤌(listen(sock, 10) == -1){
		net__print_error(MOSQ_LOG_ERR, "Error listening to unix socket: %s");
		أرجع 1;
	}

	𐤀𐤌(net__socket_nonblock(&sock)){
		أرجع 1;
	}

	أرجع 0;
}
#endif


/* Creates a socket and listens on port 'port'.
 * Returns 1 on failure
 * Returns 0 on success.
 */
صحيح net__socket_listen(بنية mosquitto__listener *listener)
{
	صحيح rc;

	𐤀𐤌(!listener){
		أرجع MOSQ_ERR_INVAL;
	}

#ifdef WITH_UNIX_SOCKETS
	𐤀𐤌(listener->port == 0 && listener->unix_socket_path != عدم){
		rc = net__socket_listen_unix(listener);
	}وإلا
#endif
	{
		rc = net__socket_listen_tcp(listener);
	}
	𐤀𐤌(rc){
		أرجع rc;
	}

	/* We need to have at least one working socket. */
	𐤀𐤌(listener->sock_count > 0){
#ifdef WITH_TLS
		𐤀𐤌(listener->certfile && listener->keyfile){
			𐤀𐤌(net__tls_server_ctx(listener)){
				أرجع 1;
			}

			𐤀𐤌(net__tls_load_verify(listener)){
				أرجع 1;
			}
		}
#  ifdef FINAL_WITH_TLS_PSK
		𐤀𐤌(listener->psk_hint){
			𐤀𐤌(listener->certfile == عدم || listener->keyfile == عدم){
				𐤀𐤌(net__tls_server_ctx(listener)){
					أرجع 1;
				}
			}
			SSL_CTX_set_psk_server_callback(listener->ssl_ctx, psk_server_callback);
			𐤀𐤌(listener->psk_hint){
				rc = SSL_CTX_use_psk_identity_hint(listener->ssl_ctx, listener->psk_hint);
				𐤀𐤌(rc == 0){
					log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to set TLS PSK hint.");
					net__print_ssl_error(عدم, عدم);
					أرجع 1;
				}
			}
		}
#  endif /* FINAL_WITH_TLS_PSK */
		𐤀𐤌(tls_ex_index_context == -1){
			tls_ex_index_context = SSL_get_ex_new_index(0, "client context", عدم, عدم, عدم);
		}
		𐤀𐤌(tls_ex_index_listener == -1){
			tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", عدم, عدم, عدم);
		}
#endif /* WITH_TLS */
		أرجع 0;
	}وإلا{
		أرجع 1;
	}
}


صحيح net__socket_get_address(mosq_sock_t sock, حرف *buf, size_t len, uint16_t *remote_port)
{
	بنية sockaddr_storage addr;
	socklen_t addrlen;

	memset(&addr, 0, حجم(بنية sockaddr_storage));
	addrlen = حجم(addr);
	𐤀𐤌(!getpeername(sock, (بنية sockaddr *)&addr, &addrlen)){
		𐤀𐤌(addr.ss_family == AF_INET){
			𐤀𐤌(remote_port){
				*remote_port = ntohs(((بنية sockaddr_in *)&addr)->sin_port);
			}
			𐤀𐤌(inet_ntop(AF_INET, &((بنية sockaddr_in *)&addr)->sin_addr.s_addr, buf, (socklen_t)len)){
				أرجع 0;
			}
		}وإلا 𐤀𐤌(addr.ss_family == AF_INET6){
			𐤀𐤌(remote_port){
				*remote_port = ntohs(((بنية sockaddr_in6 *)&addr)->sin6_port);
			}
			𐤀𐤌(inet_ntop(AF_INET6, &((بنية sockaddr_in6 *)&addr)->sin6_addr.s6_addr, buf, (socklen_t)len)){
				أرجع 0;
			}
#ifdef WITH_UNIX_SOCKETS
		}وإلا 𐤀𐤌(addr.ss_family == AF_UNIX){
			بنية sockaddr_un un;
			addrlen = حجم(بنية sockaddr_un);
			𐤀𐤌(!getsockname(sock, (بنية sockaddr *)&un, &addrlen)){
				snprintf(buf, len, "%s", un.sun_path);
			}وإلا{
				snprintf(buf, len, "unix-socket");
			}
			أرجع 0;
#endif
		}
	}
	أرجع 1;
}
