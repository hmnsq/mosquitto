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

#شامل "config.h"

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
#وگرنه
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#شامل <errno.h>
#شامل <fcntl.h>
#شامل <stdio.h>
#شامل <string.h>
#ifdef WITH_WRAP
#  include <tcpd.h>
#endif

#ifdef HAVE_NETINET_IN_H
#  include <netinet/in.h>
#endif

#اگر defined(WITH_UNIX_SOCKETS) || defined(WITH_TLS)
#  include "sys/stat.h"
#endif

#ifdef WITH_UNIX_SOCKETS
#  ifdef WIN32
#    include <afunix.h>
#  وگرنه
#    include <sys/un.h>
#  endif
#endif

#ifdef __QNX__
#  include <net/netbyte.h>
#endif

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "net_mosq.h"
#شامل "util_mosq.h"

#ifdef WITH_TLS
#  include "tls_mosq.h"
#  include <openssl/err.h>
ایستا صحیح tls_ex_index_context = -1;
ایستا صحیح tls_ex_index_listener = -1;
#endif

#شامل "sys_tree.h"

/* For EMFILE handling */
ایستا mosq_sock_t spare_sock = INVALID_SOCKET;


تهی net__broker_init(تهی)
{
	spare_sock = socket(AF_INET, SOCK_STREAM, 0);
	net__init();
#ifdef WITH_TLS
	net__init_tls();
#endif
}


تهی net__broker_cleanup(تهی)
{
	اگر(spare_sock != INVALID_SOCKET){
		COMPAT_CLOSE(spare_sock);
		spare_sock = INVALID_SOCKET;
	}
	net__cleanup();
}


ایستا تهی net__print_error(unsigned صحیح log, ثابت نویسه *format_str)
{
	نویسه *buf;

#ifdef WIN32
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			پوچ, WSAGetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, پوچ);

	log__printf(پوچ, log, format_str, buf);
	LocalFree(buf);
#وگرنه
	buf = strerror(errno);
	log__printf(پوچ, log, format_str, buf);
#endif
}


ساختار mosquitto *net__socket_accept(ساختار mosquitto__listener_sock *listensock)
{
	mosq_sock_t new_sock = INVALID_SOCKET;
	ساختار mosquitto *new_context;
#ifdef WITH_TLS
	BIO *bio;
#endif
#ifdef WITH_WRAP
	ساختار request_info wrap_req;
	نویسه address[1024];
#endif

	new_sock = accept(listensock->sock, پوچ, 0);
	اگر(new_sock == INVALID_SOCKET){
#ifdef WIN32
		WINDOWS_SET_ERRNO();
		اگر(errno == WSAEMFILE){
#وگرنه
		اگر(errno == EMFILE || errno == ENFILE){
#endif
			/* Close the spare socket, which means we should be able to accept
			 * this connection. Accept it, then close it immediately and create
			 * a new spare_sock. This prevents the situation of ever properly
			 * running out of sockets.
			 * It would be nice to send a "server not available" connack here,
			 * but there are lots of reasons why this would be tricky (TLS
			 * being the big one). */
			COMPAT_CLOSE(spare_sock);
			new_sock = accept(listensock->sock, پوچ, 0);
			اگر(new_sock != INVALID_SOCKET){
				COMPAT_CLOSE(new_sock);
			}
			spare_sock = socket(AF_INET, SOCK_STREAM, 0);
			log__printf(پوچ, MOSQ_LOG_WARNING,
					"Unable to accept new connection, system socket count has been exceeded. Try increasing \"ulimit -n\" or equivalent.");
		}
		بازگشت پوچ;
	}

	metrics__int_inc(mosq_counter_socket_connections, 1);

	اگر(net__socket_nonblock(&new_sock)){
		بازگشت پوچ;
	}

#ifdef WITH_WRAP
	/* Use tcpd / libwrap to determine whether a connection is allowed. */
	request_init(&wrap_req, RQ_FILE, new_sock, RQ_DAEMON, "mosquitto", 0);
	fromhost(&wrap_req);
	اگر(!hosts_access(&wrap_req)){
		/* Access is denied */
		اگر(db.config->connection_messages == true){
			اگر(!net__socket_get_address(new_sock, address, 1024, پوچ)){
				log__printf(پوچ, MOSQ_LOG_NOTICE, "Client connection from %s denied access by tcpd.", address);
			}
		}
		COMPAT_CLOSE(new_sock);
		بازگشت پوچ;
	}
#endif

	اگر(db.config->set_tcp_nodelay && listensock->listener->port){
		صحیح flag = 1;
		اگر(setsockopt(new_sock, IPPROTO_TCP, TCP_NODELAY, (ثابت تهی *)&flag, اندازه(صحیح)) != 0){
			log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Unable to set TCP_NODELAY.");
		}
	}

	new_context = context__init();
	اگر(!new_context){
		COMPAT_CLOSE(new_sock);
		بازگشت پوچ;
	}
	new_context->listener = listensock->listener;
	اگر(!new_context->listener){
		context__cleanup(new_context, true);
		بازگشت پوچ;
	}
	new_context->listener->client_شمار++;

	اگر(new_context->listener->enable_proxy_protocol){
		اگر(context__init_sock(new_context, new_sock, false) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			بازگشت پوچ;
		}
		اگر(new_context->listener->enable_proxy_protocol == 2){
			new_context->transport = mosq_t_proxy_v2;
		}وگرنه{
			new_context->transport = mosq_t_proxy_v1;
		}
		new_context->proxy.cmd = -1;
	}وگرنه{
		اگر(context__init_sock(new_context, new_sock, true) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			بازگشت پوچ;
		}
		انتخاب(new_context->listener->protocol){
			حالت mp_mqtt:
				new_context->transport = mosq_t_tcp;
				بشکن;
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			حالت mp_websockets:
				اگر(http__context_init(new_context)){
					context__cleanup(new_context, true);
					بازگشت پوچ;
				}
				بشکن;
#endif
			پیشفرض:
				context__cleanup(new_context, true);
				بازگشت پوچ;
		}
	}

	اگر((new_context->listener->max_connections > 0 && new_context->listener->client_شمار > new_context->listener->max_connections)
			|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (unsigned صحیح)db.config->global_max_connections)){

		اگر(db.config->connection_messages == true){
			log__printf(پوچ, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", new_context->address);
		}
		context__cleanup(new_context, true);
		بازگشت پوچ;
	}

#ifdef WITH_TLS
	/* TLS init */
	اگر(new_context->listener->ssl_ctx){
		new_context->ssl = SSL_new(new_context->listener->ssl_ctx);
		اگر(!new_context->ssl){
			context__cleanup(new_context, true);
			بازگشت پوچ;
		}
		اگر(!SSL_set_ex_data(new_context->ssl, tls_ex_index_context, new_context)
				|| !SSL_set_ex_data(new_context->ssl, tls_ex_index_listener, new_context->listener)){
			context__cleanup(new_context, true);
			بازگشت پوچ;
		}
		new_context->want_write = true;
		bio = BIO_new_socket(new_sock, BIO_NOCLOSE);
		SSL_set_bio(new_context->ssl, bio, bio);
		ERR_clear_error();
		SSL_set_accept_state(new_context->ssl);
	}
#endif

	اگر(db.config->connection_messages == true
			&& !new_context->listener->enable_proxy_protocol){

		log__printf(پوچ, MOSQ_LOG_NOTICE, "New connection from %s:%d on port %d.",
				new_context->address, new_context->remote_port, new_context->listener->port);
	}

	mux__new(new_context);
	keepalive__add(new_context);

	بازگشت new_context;
}

#ifdef WITH_TLS


ایستا صحیح client_certificate_verify(صحیح preverify_ok, X509_STORE_CTX *ctx)
{
	/* Preverify should check expiry, revocation. */
	اگر(preverify_ok == 0){
		SSL *ssl;
		ssl = X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx());
		اگر(ssl){
			ساختار mosquitto__listener *listener;
			listener = SSL_get_ex_data(ssl, tls_ex_index_listener);

			اگر(listener && listener->disable_client_cert_date_checks){
				صحیح err = X509_STORE_CTX_get_error(ctx);
				اگر(err == X509_V_ERR_CERT_NOT_YET_VALID || err == X509_V_ERR_CERT_HAS_EXPIRED){
					preverify_ok = 1;
				}
			}
		}
	}
	بازگشت preverify_ok;
}
#endif

#ifdef FINAL_WITH_TLS_PSK


ایستا unsigned صحیح psk_server_callback(SSL *ssl, ثابت نویسه *identity, unsigned نویسه *psk, unsigned صحیح max_psk_len)
{
	ساختار mosquitto *context;
	ساختار mosquitto__listener *listener;
	نویسه *psk_کلید = پوچ;
	صحیح len;
	ثابت نویسه *psk_hint;

	اگر(!identity){
		بازگشت 0;
	}

	context = SSL_get_ex_data(ssl, tls_ex_index_context);
	اگر(!context){
		بازگشت 0;
	}

	listener = SSL_get_ex_data(ssl, tls_ex_index_listener);
	اگر(!listener){
		بازگشت 0;
	}

	psk_hint = listener->psk_hint;

	/* The hex to BN conversion results in the length halving, so we can pass
	 * max_psk_len*2 as the max hex key here. */
	psk_کلید = mosquitto_calloc(1, (size_t)max_psk_len*2 + 1);
	اگر(!psk_کلید){
		بازگشت 0;
	}

	اگر(mosquitto_psk_کلید_get(context, psk_hint, identity, psk_کلید, (صحیح)max_psk_len*2) != MOSQ_ERR_SUCCESS){
		mosquitto_FREE(psk_کلید);
		بازگشت 0;
	}

	len = mosquitto__hex2bin(psk_کلید, psk, (صحیح)max_psk_len);
	اگر(len < 0){
		mosquitto_FREE(psk_کلید);
		بازگشت 0;
	}

	اگر(listener->use_identity_as_username){
		اگر(mosquitto_validate_utf8(identity, (صحیح)strlen(identity))){
			mosquitto_free(psk_کلید);
			بازگشت 0;
		}
		context->username = mosquitto_strdup(identity);
		اگر(!context->username){
			mosquitto_FREE(psk_کلید);
			بازگشت 0;
		}
	}

	mosquitto_FREE(psk_کلید);
	بازگشت (unsigned صحیح)len;
}
#endif

#ifdef WITH_TLS


ایستا تهی tls_keylog_callback(ثابت SSL *ssl, ثابت نویسه *سطر)
{
	UNUSED(ssl);

	اگر(db.tls_keylog){
		FILE *fptr;
		fptr = mosquitto_fopen(db.tls_keylog, "at", true);
		اگر(fptr){
#ifndef WIN32
			/* Until mosquitto_fopen enforces file permissions on all files,
			 * enforce it here. We can enforce it here, because it isn't a
			 * change of behaviour. */
			ساختار stat statbuf;
			اگر(fstat(fileno(fptr), &statbuf) < 0
					|| statbuf.st_mode & (S_IRWXG | S_IRWXO)){

				fclose(fptr);
				بازگشت;
			}
#endif
			fprintf(fptr, "%s\n", سطر);
			fclose(fptr);
		}وگرنه{
#ifndef WIN32
			اگر(errno == ELOOP){
				log__printf(پوچ, MOSQ_LOG_INFO, "Error: keylog file must not be a symbolic link");
			}وگرنه
#endif
			{
				log__printf(پوچ, MOSQ_LOG_INFO, "Error: Unable to open keylog file: %s", strerror(errno));
			}
		}
	}
}


صحیح net__tls_server_ctx(ساختار mosquitto__listener *listener)
{
	نویسه buf[256];
	صحیح rc;

	اگر(tls_ex_index_context == -1){
		tls_ex_index_context = SSL_get_ex_new_index(0, "client context", پوچ, پوچ, پوچ);
	}
	اگر(tls_ex_index_listener == -1){
		tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", پوچ, پوچ, پوچ);
	}

	اگر(listener->ssl_ctx){
		SSL_CTX_free(listener->ssl_ctx);
		listener->ssl_ctx = پوچ;
	}

	listener->ssl_ctx = SSL_CTX_new(TLS_server_method());

	اگر(!listener->ssl_ctx){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to create TLS context.");
		بازگشت MOSQ_ERR_TLS;
	}

#ifdef SSL_OP_NO_TLSv1_3
	اگر(db.config->per_listener_settings){
		اگر(listener->security_options->psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}وگرنه{
		اگر(db.config->security_options.psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}
#endif

	اگر(listener->tls_version == پوچ){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
#ifdef SSL_OP_NO_TLSv1_3
	}وگرنه اگر(!strcmp(listener->tls_version, "tlsv1.3")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2);
#endif
	}وگرنه اگر(!strcmp(listener->tls_version, "tlsv1.2")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
	}وگرنه اگر(!strcmp(listener->tls_version, "tlsv1.1")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1);
		log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: TLS v1.1 is insecure. It must only be used in the case "
				"where you have devices that cannot be upgraded to use a secure version of TLS. It is "
				"recommended to use as secure a set of ciphers as possible and that will restrict it to "
				"TLS v1.1 only, and to use a separate listener using TLS v1.2 and secure ciphers for your "
				"other devices.");
		log__printf(پوچ, MOSQ_LOG_WARNING, "Please be aware that support for TLS v1.1 will go away eventually "
				"and that you should plan now to migrate away from it.");
	}وگرنه{
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unsupported tls_version \"%s\".", listener->tls_version);
		بازگشت MOSQ_ERR_TLS;
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

	اگر(db.tls_keylog){
		log__printf(پوچ, MOSQ_LOG_NOTICE, "TLS key logging to '%s' enabled for all listeners.",
				db.tls_keylog);
		log__printf(پوچ, MOSQ_LOG_NOTICE, "TLS key logging is for DEBUGGING only.");

		SSL_CTX_set_keylog_callback(listener->ssl_ctx, tls_keylog_callback);
	}

	snprintf(buf, 256, "mosquitto-%d", listener->port);
	SSL_CTX_set_session_id_context(listener->ssl_ctx, (unsigned نویسه *)buf, (unsigned صحیح)strlen(buf));

	اگر(listener->ciphers){
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, listener->ciphers);
		اگر(rc == 0){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			بازگشت MOSQ_ERR_TLS;
		}
	}وگرنه{
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, "DEFAULT:!aNULL:!eNULL:!LOW:!EXPORT:!SSLv2:@STRENGTH");
		اگر(rc == 0){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			بازگشت MOSQ_ERR_TLS;
		}
	}
	اگر(listener->ciphers_tls13){
		rc = SSL_CTX_set_ciphersuites(listener->ssl_ctx, listener->ciphers_tls13);
		اگر(rc == 0){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to set TLS 1.3 ciphersuites. Check cipher_tls13 list \"%s\".", listener->ciphers_tls13);
			بازگشت MOSQ_ERR_TLS;
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
}
#endif


#ifdef WITH_TLS


ایستا صحیح net__load_crl_file(ساختار mosquitto__listener *listener)
{
	X509_STORE *store;
	X509_LOOKUP *lookup;
	صحیح rc;

	store = SSL_CTX_get_cert_store(listener->ssl_ctx);
	اگر(!store){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to obtain TLS store.");
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		بازگشت MOSQ_ERR_TLS;
	}
	lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
	rc = X509_load_crl_file(lookup, listener->crlfile, X509_FILETYPE_PEM);
	اگر(rc < 1){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to load certificate revocation file \"%s\". Check crlfile.", listener->crlfile);
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		net__print_ssl_error(پوچ, پوچ);
		بازگشت MOSQ_ERR_TLS;
	}
	X509_STORE_set_flags(store, X509_V_FLAG_CRL_CHECK);

	بازگشت MOSQ_ERR_SUCCESS;
}
#endif


صحیح net__load_certificates(ساختار mosquitto__listener *listener)
{
#ifdef WITH_TLS
	صحیح rc;

	اگر(listener->require_certificate){
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, client_certificate_verify);
	}وگرنه{
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_NONE, client_certificate_verify);
	}
	rc = SSL_CTX_use_certificate_chain_file(listener->ssl_ctx, listener->certfile);
	اگر(rc != 1){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
		net__print_ssl_error(پوچ, پوچ);
		بازگشت MOSQ_ERR_TLS;
	}
	اگر(listener->tls_engine == پوچ || listener->tls_keyform == mosq_k_pem){
		rc = SSL_CTX_use_Private_کلید_file(listener->ssl_ctx, listener->keyfile, SSL_FILETYPE_PEM);
		اگر(rc != 1){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			net__print_ssl_error(پوچ, پوچ);
			بازگشت MOSQ_ERR_TLS;
		}
	}
	rc = SSL_CTX_check_private_کلید(listener->ssl_ctx);
	اگر(rc != 1){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Server certificate/key are inconsistent.");
		net__print_ssl_error(پوچ, پوچ);
		بازگشت MOSQ_ERR_TLS;
	}
	/* Load CRLs if they exist. */
	اگر(listener->crlfile){
		rc = net__load_crl_file(listener);
		اگر(rc){
			بازگشت rc;
		}
	}
#وگرنه
	UNUSED(listener);
#endif
	بازگشت MOSQ_ERR_SUCCESS;
}


#اگر defined(WITH_TLS) && !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000


ایستا صحیح net__load_engine(ساختار mosquitto__listener *listener)
{
	ENGINE *engine = پوچ;
	UI_METHOD *ui_method;
	EVP_PKEY *pkey;

	اگر(!listener->tls_engine){
		بازگشت MOSQ_ERR_SUCCESS;
	}

	engine = ENGINE_by_id(listener->tls_engine);
	اگر(!engine){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error loading %s engine\n", listener->tls_engine);
		net__print_ssl_error(پوچ, پوچ);
		بازگشت MOSQ_ERR_TLS;
	}
	اگر(!ENGINE_init(engine)){
		log__printf(پوچ, MOSQ_LOG_ERR, "Failed engine initialisation\n");
		net__print_ssl_error(پوچ, پوچ);
		بازگشت MOSQ_ERR_TLS;
	}
	ENGINE_set_default(engine, ENGINE_METHOD_ALL);

	اگر(listener->tls_keyform == mosq_k_engine){
		ui_method = net__get_ui_method();
		اگر(listener->tls_engine_kpass_sha1){
			اگر(!ENGINE_ctrl_cmd(engine, ENGINE_SECRET_MODE, ENGINE_SECRET_MODE_SHA, پوچ, پوچ, 0)){
				log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to set engine secret mode sha");
				net__print_ssl_error(پوچ, پوچ);
				بازگشت MOSQ_ERR_TLS;
			}
			اگر(!ENGINE_ctrl_cmd(engine, ENGINE_PIN, 0, listener->tls_engine_kpass_sha1, پوچ, 0)){
				log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to set engine pin");
				net__print_ssl_error(پوچ, پوچ);
				بازگشت MOSQ_ERR_TLS;
			}
			ui_method = پوچ;
		}
		pkey = ENGINE_load_private_کلید(engine, listener->keyfile, ui_method, پوچ);
		اگر(!pkey){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to load engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(پوچ, پوچ);
			بازگشت MOSQ_ERR_TLS;
		}
		اگر(SSL_CTX_use_Private_کلید(listener->ssl_ctx, pkey) <= 0){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to use engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(پوچ, پوچ);
			بازگشت MOSQ_ERR_TLS;
		}
	}
	ENGINE_free(engine); /* release the structural reference from ENGINE_by_id() */

	بازگشت MOSQ_ERR_SUCCESS;
}
#endif


صحیح net__tls_load_verify(ساختار mosquitto__listener *listener)
{
#ifdef WITH_TLS
	صحیح rc;

#  اگر OPENSSL_VERSION_عدد < 0x30000000L
	اگر(listener->cafile || listener->capath){
		rc = SSL_CTX_load_verify_locations(listener->ssl_ctx, listener->cafile, listener->capath);
		اگر(rc == 0){
			اگر(listener->cafile && listener->capath){
				log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\" and capath \"%s\".", listener->cafile, listener->capath);
			}وگرنه اگر(listener->cafile){
				log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			}وگرنه{
				log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			}
		}
	}
#  وگرنه
	پشته_OF(X509_NAME) *ca_names = پوچ;
	اگر(listener->cafile || listener->capath){
		ca_names = sk_X509_NAME_new_null();
	}

	اگر(listener->cafile){
		rc = SSL_CTX_load_verify_file(listener->ssl_ctx, listener->cafile);
		اگر(rc == 0){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			net__print_ssl_error(پوچ, پوچ);
			بازگشت MOSQ_ERR_TLS;
		}

		پشته_OF(X509_NAME) *cert_names = SSL_load_client_CA_file(listener->cafile);
		اگر(cert_names){
			برای(صحیح i=0; i<sk_X509_NAME_رقم(cert_names); i++){
				sk_X509_NAME_افزودن(ca_names, X509_NAME_dup(sk_X509_NAME_مقدار(cert_names, i)));
			}
			sk_X509_NAME_برداشت_free(cert_names, X509_NAME_free);
		}

	}
	اگر(listener->capath){
		rc = SSL_CTX_load_verify_dir(listener->ssl_ctx, listener->capath);
		اگر(rc == 0){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			net__print_ssl_error(پوچ, پوچ);
			sk_X509_NAME_برداشت_free(ca_names, X509_NAME_free);
			بازگشت MOSQ_ERR_TLS;
		}

		SSL_add_dir_cert_subjects_to_پشته(ca_names, listener->capath);
	}
	اگر(ca_names){
		SSL_CTX_set_client_CA_list(listener->ssl_ctx, ca_names);
	}
#  endif

#  اگر !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000
	اگر(net__load_engine(listener)){
		بازگشت MOSQ_ERR_TLS;
	}
#  endif
#endif
	بازگشت net__load_certificates(listener);
}


#اگر !defined(WIN32) && !defined(_AIX)


ایستا صحیح net__bind_interface(ساختار mosquitto__listener *listener, ساختار addrinfo *rp)
{
	/*
	 * This binds the listener sock to a network interface.
	 * The use of SO_BINDTODEVICE requires root access, which we don't have, so instead
	 * use getifaddrs to find the interface addresses, and use IP of the
	 * matching interface in the later bind().
	 */
	ساختار ifaddrs *ifaddr;
	bool have_interface = false;

	اگر(getifaddrs(&ifaddr) < 0){
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		بازگشت MOSQ_ERR_ERRNO;
	}

	برای(ساختار ifaddrs *ifa=ifaddr; ifa!=پوچ; ifa=ifa->ifa_next){
		اگر(ifa->ifa_addr == پوچ){
			ادامه;
		}

		اگر(!strcasecmp(listener->bind_interface, ifa->ifa_name)){
			have_interface = true;

			اگر(ifa->ifa_addr->sa_family == rp->ai_addr->sa_family){
				اگر(rp->ai_addr->sa_family == AF_INET){
					اگر(listener->host &&
							memcmp(&((ساختار sockaddr_in *)rp->ai_addr)->sin_addr,
							&((ساختار sockaddr_in *)ifa->ifa_addr)->sin_addr,
							اندازه(ساختار in_addr))){

						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						بازگشت MOSQ_ERR_INVAL;
					}وگرنه{
						memcpy(&((ساختار sockaddr_in *)rp->ai_addr)->sin_addr,
								&((ساختار sockaddr_in *)ifa->ifa_addr)->sin_addr,
								اندازه(ساختار in_addr));

						freeifaddrs(ifaddr);
						بازگشت MOSQ_ERR_SUCCESS;
					}
				}وگرنه اگر(rp->ai_addr->sa_family == AF_INET6){
					اگر(listener->host &&
							memcmp(&((ساختار sockaddr_in6 *)rp->ai_addr)->sin6_addr,
							&((ساختار sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
							اندازه(ساختار in6_addr))){

						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						بازگشت MOSQ_ERR_INVAL;
					}وگرنه{
						memcpy(&((ساختار sockaddr_in6 *)rp->ai_addr)->sin6_addr,
								&((ساختار sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
								اندازه(ساختار in6_addr));

						((ساختار sockaddr_in6 *)rp->ai_addr)->sin6_scope_id = ((ساختار sockaddr_in6 *)ifa->ifa_addr)->sin6_scope_id;

						freeifaddrs(ifaddr);
						بازگشت MOSQ_ERR_SUCCESS;
					}
				}
			}
		}
	}
	freeifaddrs(ifaddr);
	اگر(have_interface){
		log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Interface %s does not support %s configuration.",
				listener->bind_interface, rp->ai_addr->sa_family == AF_INET ? "IPv4" : "IPv6");
		بازگشت MOSQ_ERR_NOT_SUPPORTED;
	}وگرنه{
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Interface %s does not exist.", listener->bind_interface);
		بازگشت MOSQ_ERR_NOT_FOUND;
	}
}
#endif


ایستا صحیح net__socket_listen_tcp(ساختار mosquitto__listener *listener)
{
	mosq_sock_t sock = INVALID_SOCKET;
	ساختار addrinfo hints;
	ساختار addrinfo *ainfo, *rp;
	نویسه service[10];
	صحیح rc;
	صحیح ss_opt = 1;
#ifndef WIN32
	bool interface_bound = false;
#endif

	اگر(!listener){
		بازگشت MOSQ_ERR_INVAL;
	}

	snprintf(service, 10, "%d", listener->port);
	memset(&hints, 0, اندازه(ساختار addrinfo));
	اگر(listener->socket_domain){
		hints.ai_family = listener->socket_domain;
	}وگرنه{
		hints.ai_family = AF_UNSPEC;
	}
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	rc = getaddrinfo(listener->host, service, &hints, &ainfo);
	اگر(rc){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error creating listener: %s.", gai_strerror(rc));
		بازگشت INVALID_SOCKET;
	}

	listener->sock_شمار = 0;
	listener->socks = پوچ;

	برای(rp = ainfo; rp; rp = rp->ai_next){
		اگر(rp->ai_family == AF_INET){
			log__printf(پوچ, MOSQ_LOG_INFO, "Opening ipv4 listen socket on port %d.", ntohs(((ساختار sockaddr_in *)rp->ai_addr)->sin_port));
		}وگرنه اگر(rp->ai_family == AF_INET6){
			log__printf(پوچ, MOSQ_LOG_INFO, "Opening ipv6 listen socket on port %d.", ntohs(((ساختار sockaddr_in6 *)rp->ai_addr)->sin6_port));
		}وگرنه{
			ادامه;
		}

		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		اگر(sock == INVALID_SOCKET){
			net__print_error(MOSQ_LOG_WARNING, "Warning: %s");
			ادامه;
		}
		listener->sock_شمار++;
		listener->socks = mosquitto_realloc(listener->socks, اندازه(mosq_sock_t)*(size_t)listener->sock_شمار);
		اگر(!listener->socks){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			بازگشت MOSQ_ERR_NOMEM;
		}
		listener->socks[listener->sock_شمار-1] = sock;

#ifndef WIN32
		ss_opt = 1;
		/* Unimportant if this fails */
		(تهی)setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (ثابت تهی *)&ss_opt, اندازه(ss_opt));
#endif
#ifdef IPV6_V6ONLY
		ss_opt = 1;
		(تهی)setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (ثابت تهی *)&ss_opt, اندازه(ss_opt));
#endif

		اگر(net__socket_nonblock(&sock)){
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			بازگشت 1;
		}

#اگر !defined(WIN32) && !defined(_AIX)
		اگر(listener->bind_interface){
			/* It might be possible that an interface does not support all relevant sa_families.
			 * We should successfully find at least one. */
			rc = net__bind_interface(listener, rp);
			اگر(rc){
				COMPAT_CLOSE(sock);
				listener->sock_شمار--;
				اگر(rc == MOSQ_ERR_NOT_FOUND || rc == MOSQ_ERR_INVAL){
					freeaddrinfo(ainfo);
					بازگشت rc;
				}وگرنه{
					ادامه;
				}
			}
			interface_bound = true;
		}
#endif

		اگر(bind(sock, rp->ai_addr, rp->ai_addrlen) == -1){
#اگر defined(__linux__)
			اگر(errno == EACCES){
				log__printf(پوچ, MOSQ_LOG_ERR, "If you are trying to bind to a privileged port (<1024), try using setcap and do not start the broker as root:");
				log__printf(پوچ, MOSQ_LOG_ERR, "    sudo setcap 'CAP_NET_BIND_SERVICE=+ep /usr/sbin/mosquitto'");
			}
#endif
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			COMPAT_CLOSE(sock);
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			بازگشت 1;
		}

		اگر(listen(sock, 100) == -1){
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			mosquitto_FREE(listener->socks);
			بازگشت 1;
		}
	}
	freeaddrinfo(ainfo);

#ifndef WIN32
	اگر(listener->bind_interface && !interface_bound){
		mosquitto_FREE(listener->socks);
		بازگشت 1;
	}
#endif

	بازگشت 0;
}


#ifdef WITH_UNIX_SOCKETS


ایستا صحیح net__socket_listen_unix(ساختار mosquitto__listener *listener)
{
	ساختار sockaddr_un addr;
	mosq_sock_t sock = INVALID_SOCKET;
	صحیح rc;
#ifndef WIN32
	mode_t old_mask;
#endif

	اگر(listener->unix_socket_path == پوچ){
		بازگشت MOSQ_ERR_INVAL;
	}
	اگر(strlen(listener->unix_socket_path) > اندازه(addr.sun_path)-1){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Path to unix socket is too long \"%s\".", listener->unix_socket_path);
		بازگشت MOSQ_ERR_INVAL;
	}

#ifdef WIN32
	DeleteFile(listener->unix_socket_path);
#وگرنه
	unlink(listener->unix_socket_path);
#endif
	log__printf(پوچ, MOSQ_LOG_INFO, "Opening unix listen socket on path %s.", listener->unix_socket_path);
	memset(&addr, 0, اندازه(ساختار sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, listener->unix_socket_path, اندازه(addr.sun_path)-1);

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	اگر(sock == INVALID_SOCKET){
		net__print_error(MOSQ_LOG_ERR, "Error creating unix socket: %s");
		بازگشت 1;
	}
	listener->sock_شمار++;
	listener->socks = mosquitto_realloc(listener->socks, اندازه(mosq_sock_t)*(size_t)listener->sock_شمار);
	اگر(!listener->socks){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
		COMPAT_CLOSE(sock);
		بازگشت MOSQ_ERR_NOMEM;
	}
	listener->socks[listener->sock_شمار-1] = sock;


#ifndef WIN32
	old_mask = umask(0007);
#endif
	rc = bind(sock, (ساختار sockaddr *)&addr, اندازه(ساختار sockaddr_un));
#ifndef WIN32
	umask(old_mask);
#endif

	اگر(rc == -1){
		net__print_error(MOSQ_LOG_ERR, "Error binding unix socket: %s");
		بازگشت 1;
	}

	اگر(listen(sock, 10) == -1){
		net__print_error(MOSQ_LOG_ERR, "Error listening to unix socket: %s");
		بازگشت 1;
	}

	اگر(net__socket_nonblock(&sock)){
		بازگشت 1;
	}

	بازگشت 0;
}
#endif


/* Creates a socket and listens on port 'port'.
 * Returns 1 on failure
 * Returns 0 on success.
 */
صحیح net__socket_listen(ساختار mosquitto__listener *listener)
{
	صحیح rc;

	اگر(!listener){
		بازگشت MOSQ_ERR_INVAL;
	}

#ifdef WITH_UNIX_SOCKETS
	اگر(listener->port == 0 && listener->unix_socket_path != پوچ){
		rc = net__socket_listen_unix(listener);
	}وگرنه
#endif
	{
		rc = net__socket_listen_tcp(listener);
	}
	اگر(rc){
		بازگشت rc;
	}

	/* We need to have at least one working socket. */
	اگر(listener->sock_شمار > 0){
#ifdef WITH_TLS
		اگر(listener->certfile && listener->keyfile){
			اگر(net__tls_server_ctx(listener)){
				بازگشت 1;
			}

			اگر(net__tls_load_verify(listener)){
				بازگشت 1;
			}
		}
#  ifdef FINAL_WITH_TLS_PSK
		اگر(listener->psk_hint){
			اگر(listener->certfile == پوچ || listener->keyfile == پوچ){
				اگر(net__tls_server_ctx(listener)){
					بازگشت 1;
				}
			}
			SSL_CTX_set_psk_server_callback(listener->ssl_ctx, psk_server_callback);
			اگر(listener->psk_hint){
				rc = SSL_CTX_use_psk_identity_hint(listener->ssl_ctx, listener->psk_hint);
				اگر(rc == 0){
					log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to set TLS PSK hint.");
					net__print_ssl_error(پوچ, پوچ);
					بازگشت 1;
				}
			}
		}
#  endif /* FINAL_WITH_TLS_PSK */
		اگر(tls_ex_index_context == -1){
			tls_ex_index_context = SSL_get_ex_new_index(0, "client context", پوچ, پوچ, پوچ);
		}
		اگر(tls_ex_index_listener == -1){
			tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", پوچ, پوچ, پوچ);
		}
#endif /* WITH_TLS */
		بازگشت 0;
	}وگرنه{
		بازگشت 1;
	}
}


صحیح net__socket_get_address(mosq_sock_t sock, نویسه *buf, size_t len, uint16_t *remote_port)
{
	ساختار sockaddr_storage addr;
	socklen_t addrlen;

	memset(&addr, 0, اندازه(ساختار sockaddr_storage));
	addrlen = اندازه(addr);
	اگر(!getpeername(sock, (ساختار sockaddr *)&addr, &addrlen)){
		اگر(addr.ss_family == AF_INET){
			اگر(remote_port){
				*remote_port = ntohs(((ساختار sockaddr_in *)&addr)->sin_port);
			}
			اگر(inet_ntop(AF_INET, &((ساختار sockaddr_in *)&addr)->sin_addr.s_addr, buf, (socklen_t)len)){
				بازگشت 0;
			}
		}وگرنه اگر(addr.ss_family == AF_INET6){
			اگر(remote_port){
				*remote_port = ntohs(((ساختار sockaddr_in6 *)&addr)->sin6_port);
			}
			اگر(inet_ntop(AF_INET6, &((ساختار sockaddr_in6 *)&addr)->sin6_addr.s6_addr, buf, (socklen_t)len)){
				بازگشت 0;
			}
#ifdef WITH_UNIX_SOCKETS
		}وگرنه اگر(addr.ss_family == AF_UNIX){
			ساختار sockaddr_un un;
			addrlen = اندازه(ساختار sockaddr_un);
			اگر(!getsockname(sock, (ساختار sockaddr *)&un, &addrlen)){
				snprintf(buf, len, "%s", un.sun_path);
			}وگرنه{
				snprintf(buf, len, "unix-socket");
			}
			بازگشت 0;
#endif
		}
	}
	بازگشت 1;
}
