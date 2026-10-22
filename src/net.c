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

#include "config.h"

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
#ᰀᰧᰀ
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#ifdef WITH_WRAP
#  include <tcpd.h>
#endif

#ifdef HAVE_NETINET_IN_H
#  include <netinet/in.h>
#endif

#ᰀᰧ defined(WITH_UNIX_SOCKETS) || defined(WITH_TLS)
#  include "sys/stat.h"
#endif

#ifdef WITH_UNIX_SOCKETS
#  ifdef WIN32
#    include <afunix.h>
#  ᰀᰧᰀ
#    include <sys/un.h>
#  endif
#endif

#ifdef __QNX__
#  include <net/netbyte.h>
#endif

#include "mosquitto_broker_internal.h"
#include "mosquitto/mqtt_protocol.h"
#include "net_mosq.h"
#include "util_mosq.h"

#ifdef WITH_TLS
#  include "tls_mosq.h"
#  include <openssl/err.h>
static puur_naa_nka tls_ex_index_context = -1;
static puur_naa_nka tls_ex_index_listener = -1;
#endif

#include "sys_tree.h"

/* For EMFILE handling */
static mosq_sock_t spare_sock = INVALID_SOCKET;


void net__broker_init(void)
{
	spare_sock = socket(AF_INET, SOCK_STREAM, 0);
	net__init();
#ifdef WITH_TLS
	net__init_tls();
#endif
}


void net__broker_cleanup(void)
{
	ᰀᰧ(spare_sock != INVALID_SOCKET){
		COMPAT_CLOSE(spare_sock);
		spare_sock = INVALID_SOCKET;
	}
	net__cleanup();
}


static void net__print_error(unsigned puur_naa_nka log, const var_na *format_str)
{
	var_na *buf;

#ifdef WIN32
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, WSAGetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, NULL);

	log__printf(NULL, log, format_str, buf);
	LocalFree(buf);
#ᰀᰧᰀ
	buf = strerror(errno);
	log__printf(NULL, log, format_str, buf);
#endif
}


sa_nracanaa mosquitto *net__socket_accept(sa_nracanaa mosquitto__listener_sock *listensock)
{
	mosq_sock_t new_sock = INVALID_SOCKET;
	sa_nracanaa mosquitto *new_context;
#ifdef WITH_TLS
	BIO *bio;
#endif
#ifdef WITH_WRAP
	sa_nracanaa request_info wrap_req;
	var_na address[1024];
#endif

	new_sock = accept(listensock->sock, NULL, 0);
	ᰀᰧ(new_sock == INVALID_SOCKET){
#ifdef WIN32
		WINDOWS_SET_ERRNO();
		ᰀᰧ(errno == WSAEMFILE){
#ᰀᰧᰀ
		ᰀᰧ(errno == EMFILE || errno == ENFILE){
#endif
			/* Close the spare socket, which means we should be able to accept
			 * this connection. Accept it, then close it immediately and create
			 * a new spare_sock. This prevents the situation of ever properly
			 * running out of sockets.
			 * It would be nice to send a "server not available" connack here,
			 * but there are lots of reasons why this would be tricky (TLS
			 * being the big one). */
			COMPAT_CLOSE(spare_sock);
			new_sock = accept(listensock->sock, NULL, 0);
			ᰀᰧ(new_sock != INVALID_SOCKET){
				COMPAT_CLOSE(new_sock);
			}
			spare_sock = socket(AF_INET, SOCK_STREAM, 0);
			log__printf(NULL, MOSQ_LOG_WARNING,
					"Unable to accept new connection, system socket count has been exceeded. Try increasing \"ulimit -n\" or equivalent.");
		}
		ᰀᰧᰌ NULL;
	}

	metrics__int_inc(mosq_counter_socket_connections, 1);

	ᰀᰧ(net__socket_nonblock(&new_sock)){
		ᰀᰧᰌ NULL;
	}

#ifdef WITH_WRAP
	/* Use tcpd / libwrap to determine whether a connection is allowed. */
	request_init(&wrap_req, RQ_FILE, new_sock, RQ_DAEMON, "mosquitto", 0);
	fromhost(&wrap_req);
	ᰀᰧ(!hosts_access(&wrap_req)){
		/* Access is denied */
		ᰀᰧ(db.config->connection_messages == true){
			ᰀᰧ(!net__socket_get_address(new_sock, address, 1024, NULL)){
				log__printf(NULL, MOSQ_LOG_NOTICE, "Client connection from %s denied access by tcpd.", address);
			}
		}
		COMPAT_CLOSE(new_sock);
		ᰀᰧᰌ NULL;
	}
#endif

	ᰀᰧ(db.config->set_tcp_nodelay && listensock->listener->port){
		puur_naa_nka flag = 1;
		ᰀᰧ(setsockopt(new_sock, IPPROTO_TCP, TCP_NODELAY, (const void *)&flag, parimaa_na(puur_naa_nka)) != 0){
			log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Unable to set TCP_NODELAY.");
		}
	}

	new_context = context__init();
	ᰀᰧ(!new_context){
		COMPAT_CLOSE(new_sock);
		ᰀᰧᰌ NULL;
	}
	new_context->listener = listensock->listener;
	ᰀᰧ(!new_context->listener){
		context__cleanup(new_context, true);
		ᰀᰧᰌ NULL;
	}
	new_context->listener->client_count++;

	ᰀᰧ(new_context->listener->enable_proxy_protocol){
		ᰀᰧ(context__init_sock(new_context, new_sock, false) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			ᰀᰧᰌ NULL;
		}
		ᰀᰧ(new_context->listener->enable_proxy_protocol == 2){
			new_context->transport = mosq_t_proxy_v2;
		}ᰀᰧᰀ{
			new_context->transport = mosq_t_proxy_v1;
		}
		new_context->proxy.cmd = -1;
	}ᰀᰧᰀ{
		ᰀᰧ(context__init_sock(new_context, new_sock, true) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			ᰀᰧᰌ NULL;
		}
		switch(new_context->listener->protocol){
			pak_sa mp_mqtt:
				new_context->transport = mosq_t_tcp;
				bha_nga;
#ᰀᰧ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			pak_sa mp_websockets:
				ᰀᰧ(http__context_init(new_context)){
					context__cleanup(new_context, true);
					ᰀᰧᰌ NULL;
				}
				bha_nga;
#endif
			default:
				context__cleanup(new_context, true);
				ᰀᰧᰌ NULL;
		}
	}

	ᰀᰧ((new_context->listener->max_connections > 0 && new_context->listener->client_count > new_context->listener->max_connections)
			|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (unsigned puur_naa_nka)db.config->global_max_connections)){

		ᰀᰧ(db.config->connection_messages == true){
			log__printf(NULL, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", new_context->address);
		}
		context__cleanup(new_context, true);
		ᰀᰧᰌ NULL;
	}

#ifdef WITH_TLS
	/* TLS init */
	ᰀᰧ(new_context->listener->ssl_ctx){
		new_context->ssl = SSL_new(new_context->listener->ssl_ctx);
		ᰀᰧ(!new_context->ssl){
			context__cleanup(new_context, true);
			ᰀᰧᰌ NULL;
		}
		ᰀᰧ(!SSL_set_ex_data(new_context->ssl, tls_ex_index_context, new_context)
				|| !SSL_set_ex_data(new_context->ssl, tls_ex_index_listener, new_context->listener)){
			context__cleanup(new_context, true);
			ᰀᰧᰌ NULL;
		}
		new_context->want_write = true;
		bio = BIO_new_socket(new_sock, BIO_NOCLOSE);
		SSL_set_bio(new_context->ssl, bio, bio);
		ERR_clear_error();
		SSL_set_accept_state(new_context->ssl);
	}
#endif

	ᰀᰧ(db.config->connection_messages == true
			&& !new_context->listener->enable_proxy_protocol){

		log__printf(NULL, MOSQ_LOG_NOTICE, "New connection from %s:%d on port %d.",
				new_context->address, new_context->remote_port, new_context->listener->port);
	}

	mux__new(new_context);
	keepalive__add(new_context);

	ᰀᰧᰌ new_context;
}

#ifdef WITH_TLS


static puur_naa_nka client_certificate_verify(puur_naa_nka preverify_ok, X509_STORE_CTX *ctx)
{
	/* Preverify should check expiry, revocation. */
	ᰀᰧ(preverify_ok == 0){
		SSL *ssl;
		ssl = X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx());
		ᰀᰧ(ssl){
			sa_nracanaa mosquitto__listener *listener;
			listener = SSL_get_ex_data(ssl, tls_ex_index_listener);

			ᰀᰧ(listener && listener->disable_client_cert_date_checks){
				puur_naa_nka err = X509_STORE_CTX_get_error(ctx);
				ᰀᰧ(err == X509_V_ERR_CERT_NOT_YET_VALID || err == X509_V_ERR_CERT_HAS_EXPIRED){
					preverify_ok = 1;
				}
			}
		}
	}
	ᰀᰧᰌ preverify_ok;
}
#endif

#ifdef FINAL_WITH_TLS_PSK


static unsigned puur_naa_nka psk_server_callback(SSL *ssl, const var_na *identity, unsigned var_na *psk, unsigned puur_naa_nka max_psk_len)
{
	sa_nracanaa mosquitto *context;
	sa_nracanaa mosquitto__listener *listener;
	var_na *psk_key = NULL;
	puur_naa_nka len;
	const var_na *psk_hint;

	ᰀᰧ(!identity){
		ᰀᰧᰌ 0;
	}

	context = SSL_get_ex_data(ssl, tls_ex_index_context);
	ᰀᰧ(!context){
		ᰀᰧᰌ 0;
	}

	listener = SSL_get_ex_data(ssl, tls_ex_index_listener);
	ᰀᰧ(!listener){
		ᰀᰧᰌ 0;
	}

	psk_hint = listener->psk_hint;

	/* The hex to BN conversion results in the length halving, so we can pass
	 * max_psk_len*2 as the max hex key here. */
	psk_key = mosquitto_calloc(1, (size_t)max_psk_len*2 + 1);
	ᰀᰧ(!psk_key){
		ᰀᰧᰌ 0;
	}

	ᰀᰧ(mosquitto_psk_key_get(context, psk_hint, identity, psk_key, (puur_naa_nka)max_psk_len*2) != MOSQ_ERR_SUCCESS){
		mosquitto_FREE(psk_key);
		ᰀᰧᰌ 0;
	}

	len = mosquitto__hex2bin(psk_key, psk, (puur_naa_nka)max_psk_len);
	ᰀᰧ(len < 0){
		mosquitto_FREE(psk_key);
		ᰀᰧᰌ 0;
	}

	ᰀᰧ(listener->use_identity_as_username){
		ᰀᰧ(mosquitto_validate_utf8(identity, (puur_naa_nka)strlen(identity))){
			mosquitto_free(psk_key);
			ᰀᰧᰌ 0;
		}
		context->username = mosquitto_strdup(identity);
		ᰀᰧ(!context->username){
			mosquitto_FREE(psk_key);
			ᰀᰧᰌ 0;
		}
	}

	mosquitto_FREE(psk_key);
	ᰀᰧᰌ (unsigned puur_naa_nka)len;
}
#endif

#ifdef WITH_TLS


static void tls_keylog_callback(const SSL *ssl, const var_na *line)
{
	UNUSED(ssl);

	ᰀᰧ(db.tls_keylog){
		FILE *fptr;
		fptr = mosquitto_fopen(db.tls_keylog, "at", true);
		ᰀᰧ(fptr){
#ifndef WIN32
			/* Until mosquitto_fopen enforces file permissions on all files,
			 * enforce it here. We can enforce it here, because it isn't a
			 * change of behaviour. */
			sa_nracanaa stat statbuf;
			ᰀᰧ(fstat(fileno(fptr), &statbuf) < 0
					|| statbuf.st_mode & (S_IRWXG | S_IRWXO)){

				fclose(fptr);
				ᰀᰧᰌ;
			}
#endif
			fprintf(fptr, "%s\n", line);
			fclose(fptr);
		}ᰀᰧᰀ{
#ifndef WIN32
			ᰀᰧ(errno == ELOOP){
				log__printf(NULL, MOSQ_LOG_INFO, "Error: keylog file must not be a symbolic link");
			}ᰀᰧᰀ
#endif
			{
				log__printf(NULL, MOSQ_LOG_INFO, "Error: Unable to open keylog file: %s", strerror(errno));
			}
		}
	}
}


puur_naa_nka net__tls_server_ctx(sa_nracanaa mosquitto__listener *listener)
{
	var_na buf[256];
	puur_naa_nka rc;

	ᰀᰧ(tls_ex_index_context == -1){
		tls_ex_index_context = SSL_get_ex_new_index(0, "client context", NULL, NULL, NULL);
	}
	ᰀᰧ(tls_ex_index_listener == -1){
		tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", NULL, NULL, NULL);
	}

	ᰀᰧ(listener->ssl_ctx){
		SSL_CTX_free(listener->ssl_ctx);
		listener->ssl_ctx = NULL;
	}

	listener->ssl_ctx = SSL_CTX_new(TLS_server_method());

	ᰀᰧ(!listener->ssl_ctx){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to create TLS context.");
		ᰀᰧᰌ MOSQ_ERR_TLS;
	}

#ifdef SSL_OP_NO_TLSv1_3
	ᰀᰧ(db.config->per_listener_settings){
		ᰀᰧ(listener->security_options->psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}ᰀᰧᰀ{
		ᰀᰧ(db.config->security_options.psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}
#endif

	ᰀᰧ(listener->tls_version == NULL){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
#ifdef SSL_OP_NO_TLSv1_3
	}ᰀᰧᰀ ᰀᰧ(!strcmp(listener->tls_version, "tlsv1.3")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2);
#endif
	}ᰀᰧᰀ ᰀᰧ(!strcmp(listener->tls_version, "tlsv1.2")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
	}ᰀᰧᰀ ᰀᰧ(!strcmp(listener->tls_version, "tlsv1.1")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1);
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS v1.1 is insecure. It must only be used in the case "
				"where you have devices that cannot be upgraded to use a secure version of TLS. It is "
				"recommended to use as secure a set of ciphers as possible and that will restrict it to "
				"TLS v1.1 only, and to use a separate listener using TLS v1.2 and secure ciphers for your "
				"other devices.");
		log__printf(NULL, MOSQ_LOG_WARNING, "Please be aware that support for TLS v1.1 will go away eventually "
				"and that you should plan now to migrate away from it.");
	}ᰀᰧᰀ{
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unsupported tls_version \"%s\".", listener->tls_version);
		ᰀᰧᰌ MOSQ_ERR_TLS;
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

	ᰀᰧ(db.tls_keylog){
		log__printf(NULL, MOSQ_LOG_NOTICE, "TLS key logging to '%s' enabled for all listeners.",
				db.tls_keylog);
		log__printf(NULL, MOSQ_LOG_NOTICE, "TLS key logging is for DEBUGGING only.");

		SSL_CTX_set_keylog_callback(listener->ssl_ctx, tls_keylog_callback);
	}

	snprintf(buf, 256, "mosquitto-%d", listener->port);
	SSL_CTX_set_session_id_context(listener->ssl_ctx, (unsigned var_na *)buf, (unsigned puur_naa_nka)strlen(buf));

	ᰀᰧ(listener->ciphers){
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, listener->ciphers);
		ᰀᰧ(rc == 0){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			ᰀᰧᰌ MOSQ_ERR_TLS;
		}
	}ᰀᰧᰀ{
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, "DEFAULT:!aNULL:!eNULL:!LOW:!EXPORT:!SSLv2:@STRENGTH");
		ᰀᰧ(rc == 0){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			ᰀᰧᰌ MOSQ_ERR_TLS;
		}
	}
	ᰀᰧ(listener->ciphers_tls13){
		rc = SSL_CTX_set_ciphersuites(listener->ssl_ctx, listener->ciphers_tls13);
		ᰀᰧ(rc == 0){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to set TLS 1.3 ciphersuites. Check cipher_tls13 list \"%s\".", listener->ciphers_tls13);
			ᰀᰧᰌ MOSQ_ERR_TLS;
		}
	}

	ᰀᰧᰌ MOSQ_ERR_SUCCESS;
}
#endif


#ifdef WITH_TLS


static puur_naa_nka net__load_crl_file(sa_nracanaa mosquitto__listener *listener)
{
	X509_STORE *store;
	X509_LOOKUP *lookup;
	puur_naa_nka rc;

	store = SSL_CTX_get_cert_store(listener->ssl_ctx);
	ᰀᰧ(!store){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to obtain TLS store.");
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		ᰀᰧᰌ MOSQ_ERR_TLS;
	}
	lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
	rc = X509_load_crl_file(lookup, listener->crlfile, X509_FILETYPE_PEM);
	ᰀᰧ(rc < 1){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to load certificate revocation file \"%s\". Check crlfile.", listener->crlfile);
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		net__print_ssl_error(NULL, NULL);
		ᰀᰧᰌ MOSQ_ERR_TLS;
	}
	X509_STORE_set_flags(store, X509_V_FLAG_CRL_CHECK);

	ᰀᰧᰌ MOSQ_ERR_SUCCESS;
}
#endif


puur_naa_nka net__load_certificates(sa_nracanaa mosquitto__listener *listener)
{
#ifdef WITH_TLS
	puur_naa_nka rc;

	ᰀᰧ(listener->require_certificate){
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, client_certificate_verify);
	}ᰀᰧᰀ{
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_NONE, client_certificate_verify);
	}
	rc = SSL_CTX_use_certificate_chain_file(listener->ssl_ctx, listener->certfile);
	ᰀᰧ(rc != 1){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
		net__print_ssl_error(NULL, NULL);
		ᰀᰧᰌ MOSQ_ERR_TLS;
	}
	ᰀᰧ(listener->tls_engine == NULL || listener->tls_keyform == mosq_k_pem){
		rc = SSL_CTX_use_PrivateKey_file(listener->ssl_ctx, listener->keyfile, SSL_FILETYPE_PEM);
		ᰀᰧ(rc != 1){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			net__print_ssl_error(NULL, NULL);
			ᰀᰧᰌ MOSQ_ERR_TLS;
		}
	}
	rc = SSL_CTX_check_private_key(listener->ssl_ctx);
	ᰀᰧ(rc != 1){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Server certificate/key are inconsistent.");
		net__print_ssl_error(NULL, NULL);
		ᰀᰧᰌ MOSQ_ERR_TLS;
	}
	/* Load CRLs if they exist. */
	ᰀᰧ(listener->crlfile){
		rc = net__load_crl_file(listener);
		ᰀᰧ(rc){
			ᰀᰧᰌ rc;
		}
	}
#ᰀᰧᰀ
	UNUSED(listener);
#endif
	ᰀᰧᰌ MOSQ_ERR_SUCCESS;
}


#ᰀᰧ defined(WITH_TLS) && !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000


static puur_naa_nka net__load_engine(sa_nracanaa mosquitto__listener *listener)
{
	ENGINE *engine = NULL;
	UI_METHOD *ui_method;
	EVP_PKEY *pkey;

	ᰀᰧ(!listener->tls_engine){
		ᰀᰧᰌ MOSQ_ERR_SUCCESS;
	}

	engine = ENGINE_by_id(listener->tls_engine);
	ᰀᰧ(!engine){
		log__printf(NULL, MOSQ_LOG_ERR, "Error loading %s engine\n", listener->tls_engine);
		net__print_ssl_error(NULL, NULL);
		ᰀᰧᰌ MOSQ_ERR_TLS;
	}
	ᰀᰧ(!ENGINE_init(engine)){
		log__printf(NULL, MOSQ_LOG_ERR, "Failed engine initialisation\n");
		net__print_ssl_error(NULL, NULL);
		ᰀᰧᰌ MOSQ_ERR_TLS;
	}
	ENGINE_set_default(engine, ENGINE_METHOD_ALL);

	ᰀᰧ(listener->tls_keyform == mosq_k_engine){
		ui_method = net__get_ui_method();
		ᰀᰧ(listener->tls_engine_kpass_sha1){
			ᰀᰧ(!ENGINE_ctrl_cmd(engine, ENGINE_SECRET_MODE, ENGINE_SECRET_MODE_SHA, NULL, NULL, 0)){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to set engine secret mode sha");
				net__print_ssl_error(NULL, NULL);
				ᰀᰧᰌ MOSQ_ERR_TLS;
			}
			ᰀᰧ(!ENGINE_ctrl_cmd(engine, ENGINE_PIN, 0, listener->tls_engine_kpass_sha1, NULL, 0)){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to set engine pin");
				net__print_ssl_error(NULL, NULL);
				ᰀᰧᰌ MOSQ_ERR_TLS;
			}
			ui_method = NULL;
		}
		pkey = ENGINE_load_private_key(engine, listener->keyfile, ui_method, NULL);
		ᰀᰧ(!pkey){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to load engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(NULL, NULL);
			ᰀᰧᰌ MOSQ_ERR_TLS;
		}
		ᰀᰧ(SSL_CTX_use_PrivateKey(listener->ssl_ctx, pkey) <= 0){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to use engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(NULL, NULL);
			ᰀᰧᰌ MOSQ_ERR_TLS;
		}
	}
	ENGINE_free(engine); /* release the structural reference from ENGINE_by_id() */

	ᰀᰧᰌ MOSQ_ERR_SUCCESS;
}
#endif


puur_naa_nka net__tls_load_verify(sa_nracanaa mosquitto__listener *listener)
{
#ifdef WITH_TLS
	puur_naa_nka rc;

#  ᰀᰧ OPENSSL_VERSION_NUMBER < 0x30000000L
	ᰀᰧ(listener->cafile || listener->capath){
		rc = SSL_CTX_load_verify_locations(listener->ssl_ctx, listener->cafile, listener->capath);
		ᰀᰧ(rc == 0){
			ᰀᰧ(listener->cafile && listener->capath){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\" and capath \"%s\".", listener->cafile, listener->capath);
			}ᰀᰧᰀ ᰀᰧ(listener->cafile){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			}ᰀᰧᰀ{
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			}
		}
	}
#  ᰀᰧᰀ
	STACK_OF(X509_NAME) *ca_names = NULL;
	ᰀᰧ(listener->cafile || listener->capath){
		ca_names = sk_X509_NAME_new_null();
	}

	ᰀᰧ(listener->cafile){
		rc = SSL_CTX_load_verify_file(listener->ssl_ctx, listener->cafile);
		ᰀᰧ(rc == 0){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			net__print_ssl_error(NULL, NULL);
			ᰀᰧᰌ MOSQ_ERR_TLS;
		}

		STACK_OF(X509_NAME) *cert_names = SSL_load_client_CA_file(listener->cafile);
		ᰀᰧ(cert_names){
			for(puur_naa_nka i=0; i<sk_X509_NAME_num(cert_names); i++){
				sk_X509_NAME_push(ca_names, X509_NAME_dup(sk_X509_NAME_value(cert_names, i)));
			}
			sk_X509_NAME_pop_free(cert_names, X509_NAME_free);
		}

	}
	ᰀᰧ(listener->capath){
		rc = SSL_CTX_load_verify_dir(listener->ssl_ctx, listener->capath);
		ᰀᰧ(rc == 0){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			net__print_ssl_error(NULL, NULL);
			sk_X509_NAME_pop_free(ca_names, X509_NAME_free);
			ᰀᰧᰌ MOSQ_ERR_TLS;
		}

		SSL_add_dir_cert_subjects_to_stack(ca_names, listener->capath);
	}
	ᰀᰧ(ca_names){
		SSL_CTX_set_client_CA_list(listener->ssl_ctx, ca_names);
	}
#  endif

#  ᰀᰧ !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000
	ᰀᰧ(net__load_engine(listener)){
		ᰀᰧᰌ MOSQ_ERR_TLS;
	}
#  endif
#endif
	ᰀᰧᰌ net__load_certificates(listener);
}


#ᰀᰧ !defined(WIN32) && !defined(_AIX)


static puur_naa_nka net__bind_interface(sa_nracanaa mosquitto__listener *listener, sa_nracanaa addrinfo *rp)
{
	/*
	 * This binds the listener sock to a network interface.
	 * The use of SO_BINDTODEVICE requires root access, which we don't have, so instead
	 * use getifaddrs to find the interface addresses, and use IP of the
	 * matching interface in the later bind().
	 */
	sa_nracanaa ifaddrs *ifaddr;
	bool have_interface = false;

	ᰀᰧ(getifaddrs(&ifaddr) < 0){
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		ᰀᰧᰌ MOSQ_ERR_ERRNO;
	}

	for(sa_nracanaa ifaddrs *ifa=ifaddr; ifa!=NULL; ifa=ifa->ifa_next){
		ᰀᰧ(ifa->ifa_addr == NULL){
			continue;
		}

		ᰀᰧ(!strcasecmp(listener->bind_interface, ifa->ifa_name)){
			have_interface = true;

			ᰀᰧ(ifa->ifa_addr->sa_family == rp->ai_addr->sa_family){
				ᰀᰧ(rp->ai_addr->sa_family == AF_INET){
					ᰀᰧ(listener->host &&
							memcmp(&((sa_nracanaa sockaddr_in *)rp->ai_addr)->sin_addr,
							&((sa_nracanaa sockaddr_in *)ifa->ifa_addr)->sin_addr,
							parimaa_na(sa_nracanaa in_addr))){

						log__printf(NULL, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						ᰀᰧᰌ MOSQ_ERR_INVAL;
					}ᰀᰧᰀ{
						memcpy(&((sa_nracanaa sockaddr_in *)rp->ai_addr)->sin_addr,
								&((sa_nracanaa sockaddr_in *)ifa->ifa_addr)->sin_addr,
								parimaa_na(sa_nracanaa in_addr));

						freeifaddrs(ifaddr);
						ᰀᰧᰌ MOSQ_ERR_SUCCESS;
					}
				}ᰀᰧᰀ ᰀᰧ(rp->ai_addr->sa_family == AF_INET6){
					ᰀᰧ(listener->host &&
							memcmp(&((sa_nracanaa sockaddr_in6 *)rp->ai_addr)->sin6_addr,
							&((sa_nracanaa sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
							parimaa_na(sa_nracanaa in6_addr))){

						log__printf(NULL, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						ᰀᰧᰌ MOSQ_ERR_INVAL;
					}ᰀᰧᰀ{
						memcpy(&((sa_nracanaa sockaddr_in6 *)rp->ai_addr)->sin6_addr,
								&((sa_nracanaa sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
								parimaa_na(sa_nracanaa in6_addr));

						((sa_nracanaa sockaddr_in6 *)rp->ai_addr)->sin6_scope_id = ((sa_nracanaa sockaddr_in6 *)ifa->ifa_addr)->sin6_scope_id;

						freeifaddrs(ifaddr);
						ᰀᰧᰌ MOSQ_ERR_SUCCESS;
					}
				}
			}
		}
	}
	freeifaddrs(ifaddr);
	ᰀᰧ(have_interface){
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Interface %s does not support %s configuration.",
				listener->bind_interface, rp->ai_addr->sa_family == AF_INET ? "IPv4" : "IPv6");
		ᰀᰧᰌ MOSQ_ERR_NOT_SUPPORTED;
	}ᰀᰧᰀ{
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Interface %s does not exist.", listener->bind_interface);
		ᰀᰧᰌ MOSQ_ERR_NOT_FOUND;
	}
}
#endif


static puur_naa_nka net__socket_listen_tcp(sa_nracanaa mosquitto__listener *listener)
{
	mosq_sock_t sock = INVALID_SOCKET;
	sa_nracanaa addrinfo hints;
	sa_nracanaa addrinfo *ainfo, *rp;
	var_na service[10];
	puur_naa_nka rc;
	puur_naa_nka ss_opt = 1;
#ifndef WIN32
	bool interface_bound = false;
#endif

	ᰀᰧ(!listener){
		ᰀᰧᰌ MOSQ_ERR_INVAL;
	}

	snprintf(service, 10, "%d", listener->port);
	memset(&hints, 0, parimaa_na(sa_nracanaa addrinfo));
	ᰀᰧ(listener->socket_domain){
		hints.ai_family = listener->socket_domain;
	}ᰀᰧᰀ{
		hints.ai_family = AF_UNSPEC;
	}
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	rc = getaddrinfo(listener->host, service, &hints, &ainfo);
	ᰀᰧ(rc){
		log__printf(NULL, MOSQ_LOG_ERR, "Error creating listener: %s.", gai_strerror(rc));
		ᰀᰧᰌ INVALID_SOCKET;
	}

	listener->sock_count = 0;
	listener->socks = NULL;

	for(rp = ainfo; rp; rp = rp->ai_next){
		ᰀᰧ(rp->ai_family == AF_INET){
			log__printf(NULL, MOSQ_LOG_INFO, "Opening ipv4 listen socket on port %d.", ntohs(((sa_nracanaa sockaddr_in *)rp->ai_addr)->sin_port));
		}ᰀᰧᰀ ᰀᰧ(rp->ai_family == AF_INET6){
			log__printf(NULL, MOSQ_LOG_INFO, "Opening ipv6 listen socket on port %d.", ntohs(((sa_nracanaa sockaddr_in6 *)rp->ai_addr)->sin6_port));
		}ᰀᰧᰀ{
			continue;
		}

		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		ᰀᰧ(sock == INVALID_SOCKET){
			net__print_error(MOSQ_LOG_WARNING, "Warning: %s");
			continue;
		}
		listener->sock_count++;
		listener->socks = mosquitto_realloc(listener->socks, parimaa_na(mosq_sock_t)*(size_t)listener->sock_count);
		ᰀᰧ(!listener->socks){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			ᰀᰧᰌ MOSQ_ERR_NOMEM;
		}
		listener->socks[listener->sock_count-1] = sock;

#ifndef WIN32
		ss_opt = 1;
		/* Unimportant if this fails */
		(void)setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&ss_opt, parimaa_na(ss_opt));
#endif
#ifdef IPV6_V6ONLY
		ss_opt = 1;
		(void)setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (const void *)&ss_opt, parimaa_na(ss_opt));
#endif

		ᰀᰧ(net__socket_nonblock(&sock)){
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			ᰀᰧᰌ 1;
		}

#ᰀᰧ !defined(WIN32) && !defined(_AIX)
		ᰀᰧ(listener->bind_interface){
			/* It might be possible that an interface does not support all relevant sa_families.
			 * We should successfully find at least one. */
			rc = net__bind_interface(listener, rp);
			ᰀᰧ(rc){
				COMPAT_CLOSE(sock);
				listener->sock_count--;
				ᰀᰧ(rc == MOSQ_ERR_NOT_FOUND || rc == MOSQ_ERR_INVAL){
					freeaddrinfo(ainfo);
					ᰀᰧᰌ rc;
				}ᰀᰧᰀ{
					continue;
				}
			}
			interface_bound = true;
		}
#endif

		ᰀᰧ(bind(sock, rp->ai_addr, rp->ai_addrlen) == -1){
#ᰀᰧ defined(__linux__)
			ᰀᰧ(errno == EACCES){
				log__printf(NULL, MOSQ_LOG_ERR, "If you are trying to bind to a privileged port (<1024), try using setcap and do not start the broker as root:");
				log__printf(NULL, MOSQ_LOG_ERR, "    sudo setcap 'CAP_NET_BIND_SERVICE=+ep /usr/sbin/mosquitto'");
			}
#endif
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			COMPAT_CLOSE(sock);
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			ᰀᰧᰌ 1;
		}

		ᰀᰧ(listen(sock, 100) == -1){
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			mosquitto_FREE(listener->socks);
			ᰀᰧᰌ 1;
		}
	}
	freeaddrinfo(ainfo);

#ifndef WIN32
	ᰀᰧ(listener->bind_interface && !interface_bound){
		mosquitto_FREE(listener->socks);
		ᰀᰧᰌ 1;
	}
#endif

	ᰀᰧᰌ 0;
}


#ifdef WITH_UNIX_SOCKETS


static puur_naa_nka net__socket_listen_unix(sa_nracanaa mosquitto__listener *listener)
{
	sa_nracanaa sockaddr_un addr;
	mosq_sock_t sock = INVALID_SOCKET;
	puur_naa_nka rc;
#ifndef WIN32
	mode_t old_mask;
#endif

	ᰀᰧ(listener->unix_socket_path == NULL){
		ᰀᰧᰌ MOSQ_ERR_INVAL;
	}
	ᰀᰧ(strlen(listener->unix_socket_path) > parimaa_na(addr.sun_path)-1){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Path to unix socket is too long \"%s\".", listener->unix_socket_path);
		ᰀᰧᰌ MOSQ_ERR_INVAL;
	}

#ifdef WIN32
	DeleteFile(listener->unix_socket_path);
#ᰀᰧᰀ
	unlink(listener->unix_socket_path);
#endif
	log__printf(NULL, MOSQ_LOG_INFO, "Opening unix listen socket on path %s.", listener->unix_socket_path);
	memset(&addr, 0, parimaa_na(sa_nracanaa sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, listener->unix_socket_path, parimaa_na(addr.sun_path)-1);

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	ᰀᰧ(sock == INVALID_SOCKET){
		net__print_error(MOSQ_LOG_ERR, "Error creating unix socket: %s");
		ᰀᰧᰌ 1;
	}
	listener->sock_count++;
	listener->socks = mosquitto_realloc(listener->socks, parimaa_na(mosq_sock_t)*(size_t)listener->sock_count);
	ᰀᰧ(!listener->socks){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		COMPAT_CLOSE(sock);
		ᰀᰧᰌ MOSQ_ERR_NOMEM;
	}
	listener->socks[listener->sock_count-1] = sock;


#ifndef WIN32
	old_mask = umask(0007);
#endif
	rc = bind(sock, (sa_nracanaa sockaddr *)&addr, parimaa_na(sa_nracanaa sockaddr_un));
#ifndef WIN32
	umask(old_mask);
#endif

	ᰀᰧ(rc == -1){
		net__print_error(MOSQ_LOG_ERR, "Error binding unix socket: %s");
		ᰀᰧᰌ 1;
	}

	ᰀᰧ(listen(sock, 10) == -1){
		net__print_error(MOSQ_LOG_ERR, "Error listening to unix socket: %s");
		ᰀᰧᰌ 1;
	}

	ᰀᰧ(net__socket_nonblock(&sock)){
		ᰀᰧᰌ 1;
	}

	ᰀᰧᰌ 0;
}
#endif


/* Creates a socket and listens on port 'port'.
 * Returns 1 on failure
 * Returns 0 on success.
 */
puur_naa_nka net__socket_listen(sa_nracanaa mosquitto__listener *listener)
{
	puur_naa_nka rc;

	ᰀᰧ(!listener){
		ᰀᰧᰌ MOSQ_ERR_INVAL;
	}

#ifdef WITH_UNIX_SOCKETS
	ᰀᰧ(listener->port == 0 && listener->unix_socket_path != NULL){
		rc = net__socket_listen_unix(listener);
	}ᰀᰧᰀ
#endif
	{
		rc = net__socket_listen_tcp(listener);
	}
	ᰀᰧ(rc){
		ᰀᰧᰌ rc;
	}

	/* We need to have at least one working socket. */
	ᰀᰧ(listener->sock_count > 0){
#ifdef WITH_TLS
		ᰀᰧ(listener->certfile && listener->keyfile){
			ᰀᰧ(net__tls_server_ctx(listener)){
				ᰀᰧᰌ 1;
			}

			ᰀᰧ(net__tls_load_verify(listener)){
				ᰀᰧᰌ 1;
			}
		}
#  ifdef FINAL_WITH_TLS_PSK
		ᰀᰧ(listener->psk_hint){
			ᰀᰧ(listener->certfile == NULL || listener->keyfile == NULL){
				ᰀᰧ(net__tls_server_ctx(listener)){
					ᰀᰧᰌ 1;
				}
			}
			SSL_CTX_set_psk_server_callback(listener->ssl_ctx, psk_server_callback);
			ᰀᰧ(listener->psk_hint){
				rc = SSL_CTX_use_psk_identity_hint(listener->ssl_ctx, listener->psk_hint);
				ᰀᰧ(rc == 0){
					log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to set TLS PSK hint.");
					net__print_ssl_error(NULL, NULL);
					ᰀᰧᰌ 1;
				}
			}
		}
#  endif /* FINAL_WITH_TLS_PSK */
		ᰀᰧ(tls_ex_index_context == -1){
			tls_ex_index_context = SSL_get_ex_new_index(0, "client context", NULL, NULL, NULL);
		}
		ᰀᰧ(tls_ex_index_listener == -1){
			tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", NULL, NULL, NULL);
		}
#endif /* WITH_TLS */
		ᰀᰧᰌ 0;
	}ᰀᰧᰀ{
		ᰀᰧᰌ 1;
	}
}


puur_naa_nka net__socket_get_address(mosq_sock_t sock, var_na *buf, size_t len, uint16_t *remote_port)
{
	sa_nracanaa sockaddr_storage addr;
	socklen_t addrlen;

	memset(&addr, 0, parimaa_na(sa_nracanaa sockaddr_storage));
	addrlen = parimaa_na(addr);
	ᰀᰧ(!getpeername(sock, (sa_nracanaa sockaddr *)&addr, &addrlen)){
		ᰀᰧ(addr.ss_family == AF_INET){
			ᰀᰧ(remote_port){
				*remote_port = ntohs(((sa_nracanaa sockaddr_in *)&addr)->sin_port);
			}
			ᰀᰧ(inet_ntop(AF_INET, &((sa_nracanaa sockaddr_in *)&addr)->sin_addr.s_addr, buf, (socklen_t)len)){
				ᰀᰧᰌ 0;
			}
		}ᰀᰧᰀ ᰀᰧ(addr.ss_family == AF_INET6){
			ᰀᰧ(remote_port){
				*remote_port = ntohs(((sa_nracanaa sockaddr_in6 *)&addr)->sin6_port);
			}
			ᰀᰧ(inet_ntop(AF_INET6, &((sa_nracanaa sockaddr_in6 *)&addr)->sin6_addr.s6_addr, buf, (socklen_t)len)){
				ᰀᰧᰌ 0;
			}
#ifdef WITH_UNIX_SOCKETS
		}ᰀᰧᰀ ᰀᰧ(addr.ss_family == AF_UNIX){
			sa_nracanaa sockaddr_un un;
			addrlen = parimaa_na(sa_nracanaa sockaddr_un);
			ᰀᰧ(!getsockname(sock, (sa_nracanaa sockaddr *)&un, &addrlen)){
				snprintf(buf, len, "%s", un.sun_path);
			}ᰀᰧᰀ{
				snprintf(buf, len, "unix-socket");
			}
			ᰀᰧᰌ 0;
#endif
		}
	}
	ᰀᰧᰌ 1;
}
