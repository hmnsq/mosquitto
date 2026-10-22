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
#아니면
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

#만약 defined(WITH_UNIX_SOCKETS) || defined(WITH_TLS)
#  include "sys/stat.h"
#endif

#ifdef WITH_UNIX_SOCKETS
#  ifdef WIN32
#    include <afunix.h>
#  아니면
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
static 정수 tls_ex_index_context = -1;
static 정수 tls_ex_index_listener = -1;
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
	만약(spare_sock != INVALID_SOCKET){
		COMPAT_CLOSE(spare_sock);
		spare_sock = INVALID_SOCKET;
	}
	net__cleanup();
}


static void net__print_error(unsigned 정수 log, const char *format_str)
{
	char *buf;

#ifdef WIN32
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, WSAGetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, NULL);

	log__printf(NULL, log, format_str, buf);
	LocalFree(buf);
#아니면
	buf = strerror(errno);
	log__printf(NULL, log, format_str, buf);
#endif
}


struct mosquitto *net__socket_accept(struct mosquitto__listener_sock *listensock)
{
	mosq_sock_t new_sock = INVALID_SOCKET;
	struct mosquitto *new_context;
#ifdef WITH_TLS
	BIO *bio;
#endif
#ifdef WITH_WRAP
	struct request_info wrap_req;
	char address[1024];
#endif

	new_sock = accept(listensock->sock, NULL, 0);
	만약(new_sock == INVALID_SOCKET){
#ifdef WIN32
		WINDOWS_SET_ERRNO();
		만약(errno == WSAEMFILE){
#아니면
		만약(errno == EMFILE || errno == ENFILE){
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
			만약(new_sock != INVALID_SOCKET){
				COMPAT_CLOSE(new_sock);
			}
			spare_sock = socket(AF_INET, SOCK_STREAM, 0);
			log__printf(NULL, MOSQ_LOG_WARNING,
					"Unable to accept new connection, system socket count has been exceeded. Try increasing \"ulimit -n\" or equivalent.");
		}
		반환 NULL;
	}

	metrics__int_inc(mosq_counter_socket_connections, 1);

	만약(net__socket_nonblock(&new_sock)){
		반환 NULL;
	}

#ifdef WITH_WRAP
	/* Use tcpd / libwrap to determine whether a connection is allowed. */
	request_init(&wrap_req, RQ_FILE, new_sock, RQ_DAEMON, "mosquitto", 0);
	fromhost(&wrap_req);
	만약(!hosts_access(&wrap_req)){
		/* Access is denied */
		만약(db.config->connection_messages == true){
			만약(!net__socket_get_address(new_sock, address, 1024, NULL)){
				log__printf(NULL, MOSQ_LOG_NOTICE, "Client connection from %s denied access by tcpd.", address);
			}
		}
		COMPAT_CLOSE(new_sock);
		반환 NULL;
	}
#endif

	만약(db.config->set_tcp_nodelay && listensock->listener->port){
		정수 flag = 1;
		만약(setsockopt(new_sock, IPPROTO_TCP, TCP_NODELAY, (const void *)&flag, sizeof(정수)) != 0){
			log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Unable to set TCP_NODELAY.");
		}
	}

	new_context = context__init();
	만약(!new_context){
		COMPAT_CLOSE(new_sock);
		반환 NULL;
	}
	new_context->listener = listensock->listener;
	만약(!new_context->listener){
		context__cleanup(new_context, true);
		반환 NULL;
	}
	new_context->listener->client_count++;

	만약(new_context->listener->enable_proxy_protocol){
		만약(context__init_sock(new_context, new_sock, false) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			반환 NULL;
		}
		만약(new_context->listener->enable_proxy_protocol == 2){
			new_context->transport = mosq_t_proxy_v2;
		}아니면{
			new_context->transport = mosq_t_proxy_v1;
		}
		new_context->proxy.cmd = -1;
	}아니면{
		만약(context__init_sock(new_context, new_sock, true) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			반환 NULL;
		}
		switch(new_context->listener->protocol){
			case mp_mqtt:
				new_context->transport = mosq_t_tcp;
				break;
#만약 defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			case mp_websockets:
				만약(http__context_init(new_context)){
					context__cleanup(new_context, true);
					반환 NULL;
				}
				break;
#endif
			default:
				context__cleanup(new_context, true);
				반환 NULL;
		}
	}

	만약((new_context->listener->max_connections > 0 && new_context->listener->client_count > new_context->listener->max_connections)
			|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (unsigned 정수)db.config->global_max_connections)){

		만약(db.config->connection_messages == true){
			log__printf(NULL, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", new_context->address);
		}
		context__cleanup(new_context, true);
		반환 NULL;
	}

#ifdef WITH_TLS
	/* TLS init */
	만약(new_context->listener->ssl_ctx){
		new_context->ssl = SSL_new(new_context->listener->ssl_ctx);
		만약(!new_context->ssl){
			context__cleanup(new_context, true);
			반환 NULL;
		}
		만약(!SSL_set_ex_data(new_context->ssl, tls_ex_index_context, new_context)
				|| !SSL_set_ex_data(new_context->ssl, tls_ex_index_listener, new_context->listener)){
			context__cleanup(new_context, true);
			반환 NULL;
		}
		new_context->want_write = true;
		bio = BIO_new_socket(new_sock, BIO_NOCLOSE);
		SSL_set_bio(new_context->ssl, bio, bio);
		ERR_clear_error();
		SSL_set_accept_state(new_context->ssl);
	}
#endif

	만약(db.config->connection_messages == true
			&& !new_context->listener->enable_proxy_protocol){

		log__printf(NULL, MOSQ_LOG_NOTICE, "New connection from %s:%d on port %d.",
				new_context->address, new_context->remote_port, new_context->listener->port);
	}

	mux__new(new_context);
	keepalive__add(new_context);

	반환 new_context;
}

#ifdef WITH_TLS


static 정수 client_certificate_verify(정수 preverify_ok, X509_STORE_CTX *ctx)
{
	/* Preverify should check expiry, revocation. */
	만약(preverify_ok == 0){
		SSL *ssl;
		ssl = X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx());
		만약(ssl){
			struct mosquitto__listener *listener;
			listener = SSL_get_ex_data(ssl, tls_ex_index_listener);

			만약(listener && listener->disable_client_cert_date_checks){
				정수 err = X509_STORE_CTX_get_error(ctx);
				만약(err == X509_V_ERR_CERT_NOT_YET_VALID || err == X509_V_ERR_CERT_HAS_EXPIRED){
					preverify_ok = 1;
				}
			}
		}
	}
	반환 preverify_ok;
}
#endif

#ifdef FINAL_WITH_TLS_PSK


static unsigned 정수 psk_server_callback(SSL *ssl, const char *identity, unsigned char *psk, unsigned 정수 max_psk_len)
{
	struct mosquitto *context;
	struct mosquitto__listener *listener;
	char *psk_key = NULL;
	정수 len;
	const char *psk_hint;

	만약(!identity){
		반환 0;
	}

	context = SSL_get_ex_data(ssl, tls_ex_index_context);
	만약(!context){
		반환 0;
	}

	listener = SSL_get_ex_data(ssl, tls_ex_index_listener);
	만약(!listener){
		반환 0;
	}

	psk_hint = listener->psk_hint;

	/* The hex to BN conversion results in the length halving, so we can pass
	 * max_psk_len*2 as the max hex key here. */
	psk_key = mosquitto_calloc(1, (size_t)max_psk_len*2 + 1);
	만약(!psk_key){
		반환 0;
	}

	만약(mosquitto_psk_key_get(context, psk_hint, identity, psk_key, (정수)max_psk_len*2) != MOSQ_ERR_SUCCESS){
		mosquitto_FREE(psk_key);
		반환 0;
	}

	len = mosquitto__hex2bin(psk_key, psk, (정수)max_psk_len);
	만약(len < 0){
		mosquitto_FREE(psk_key);
		반환 0;
	}

	만약(listener->use_identity_as_username){
		만약(mosquitto_validate_utf8(identity, (정수)strlen(identity))){
			mosquitto_free(psk_key);
			반환 0;
		}
		context->username = mosquitto_strdup(identity);
		만약(!context->username){
			mosquitto_FREE(psk_key);
			반환 0;
		}
	}

	mosquitto_FREE(psk_key);
	반환 (unsigned 정수)len;
}
#endif

#ifdef WITH_TLS


static void tls_keylog_callback(const SSL *ssl, const char *line)
{
	UNUSED(ssl);

	만약(db.tls_keylog){
		FILE *fptr;
		fptr = mosquitto_fopen(db.tls_keylog, "at", true);
		만약(fptr){
#ifndef WIN32
			/* Until mosquitto_fopen enforces file permissions on all files,
			 * enforce it here. We can enforce it here, because it isn't a
			 * change of behaviour. */
			struct stat statbuf;
			만약(fstat(fileno(fptr), &statbuf) < 0
					|| statbuf.st_mode & (S_IRWXG | S_IRWXO)){

				fclose(fptr);
				반환;
			}
#endif
			fprintf(fptr, "%s\n", line);
			fclose(fptr);
		}아니면{
#ifndef WIN32
			만약(errno == ELOOP){
				log__printf(NULL, MOSQ_LOG_INFO, "Error: keylog file must not be a symbolic link");
			}아니면
#endif
			{
				log__printf(NULL, MOSQ_LOG_INFO, "Error: Unable to open keylog file: %s", strerror(errno));
			}
		}
	}
}


정수 net__tls_server_ctx(struct mosquitto__listener *listener)
{
	char buf[256];
	정수 rc;

	만약(tls_ex_index_context == -1){
		tls_ex_index_context = SSL_get_ex_new_index(0, "client context", NULL, NULL, NULL);
	}
	만약(tls_ex_index_listener == -1){
		tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", NULL, NULL, NULL);
	}

	만약(listener->ssl_ctx){
		SSL_CTX_free(listener->ssl_ctx);
		listener->ssl_ctx = NULL;
	}

	listener->ssl_ctx = SSL_CTX_new(TLS_server_method());

	만약(!listener->ssl_ctx){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to create TLS context.");
		반환 MOSQ_ERR_TLS;
	}

#ifdef SSL_OP_NO_TLSv1_3
	만약(db.config->per_listener_settings){
		만약(listener->security_options->psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}아니면{
		만약(db.config->security_options.psk_file){
			SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}
#endif

	만약(listener->tls_version == NULL){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
#ifdef SSL_OP_NO_TLSv1_3
	}아니면 만약(!strcmp(listener->tls_version, "tlsv1.3")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2);
#endif
	}아니면 만약(!strcmp(listener->tls_version, "tlsv1.2")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
	}아니면 만약(!strcmp(listener->tls_version, "tlsv1.1")){
		SSL_CTX_set_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1);
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: TLS v1.1 is insecure. It must only be used in the case "
				"where you have devices that cannot be upgraded to use a secure version of TLS. It is "
				"recommended to use as secure a set of ciphers as possible and that will restrict it to "
				"TLS v1.1 only, and to use a separate listener using TLS v1.2 and secure ciphers for your "
				"other devices.");
		log__printf(NULL, MOSQ_LOG_WARNING, "Please be aware that support for TLS v1.1 will go away eventually "
				"and that you should plan now to migrate away from it.");
	}아니면{
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unsupported tls_version \"%s\".", listener->tls_version);
		반환 MOSQ_ERR_TLS;
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

	만약(db.tls_keylog){
		log__printf(NULL, MOSQ_LOG_NOTICE, "TLS key logging to '%s' enabled for all listeners.",
				db.tls_keylog);
		log__printf(NULL, MOSQ_LOG_NOTICE, "TLS key logging is for DEBUGGING only.");

		SSL_CTX_set_keylog_callback(listener->ssl_ctx, tls_keylog_callback);
	}

	snprintf(buf, 256, "mosquitto-%d", listener->port);
	SSL_CTX_set_session_id_context(listener->ssl_ctx, (unsigned char *)buf, (unsigned 정수)strlen(buf));

	만약(listener->ciphers){
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, listener->ciphers);
		만약(rc == 0){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			반환 MOSQ_ERR_TLS;
		}
	}아니면{
		rc = SSL_CTX_set_cipher_list(listener->ssl_ctx, "DEFAULT:!aNULL:!eNULL:!LOW:!EXPORT:!SSLv2:@STRENGTH");
		만약(rc == 0){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			반환 MOSQ_ERR_TLS;
		}
	}
	만약(listener->ciphers_tls13){
		rc = SSL_CTX_set_ciphersuites(listener->ssl_ctx, listener->ciphers_tls13);
		만약(rc == 0){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to set TLS 1.3 ciphersuites. Check cipher_tls13 list \"%s\".", listener->ciphers_tls13);
			반환 MOSQ_ERR_TLS;
		}
	}

	반환 MOSQ_ERR_SUCCESS;
}
#endif


#ifdef WITH_TLS


static 정수 net__load_crl_file(struct mosquitto__listener *listener)
{
	X509_STORE *store;
	X509_LOOKUP *lookup;
	정수 rc;

	store = SSL_CTX_get_cert_store(listener->ssl_ctx);
	만약(!store){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to obtain TLS store.");
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		반환 MOSQ_ERR_TLS;
	}
	lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
	rc = X509_load_crl_file(lookup, listener->crlfile, X509_FILETYPE_PEM);
	만약(rc < 1){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to load certificate revocation file \"%s\". Check crlfile.", listener->crlfile);
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		net__print_ssl_error(NULL, NULL);
		반환 MOSQ_ERR_TLS;
	}
	X509_STORE_set_flags(store, X509_V_FLAG_CRL_CHECK);

	반환 MOSQ_ERR_SUCCESS;
}
#endif


정수 net__load_certificates(struct mosquitto__listener *listener)
{
#ifdef WITH_TLS
	정수 rc;

	만약(listener->require_certificate){
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, client_certificate_verify);
	}아니면{
		SSL_CTX_set_verify(listener->ssl_ctx, SSL_VERIFY_NONE, client_certificate_verify);
	}
	rc = SSL_CTX_use_certificate_chain_file(listener->ssl_ctx, listener->certfile);
	만약(rc != 1){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
		net__print_ssl_error(NULL, NULL);
		반환 MOSQ_ERR_TLS;
	}
	만약(listener->tls_engine == NULL || listener->tls_keyform == mosq_k_pem){
		rc = SSL_CTX_use_PrivateKey_file(listener->ssl_ctx, listener->keyfile, SSL_FILETYPE_PEM);
		만약(rc != 1){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			net__print_ssl_error(NULL, NULL);
			반환 MOSQ_ERR_TLS;
		}
	}
	rc = SSL_CTX_check_private_key(listener->ssl_ctx);
	만약(rc != 1){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Server certificate/key are inconsistent.");
		net__print_ssl_error(NULL, NULL);
		반환 MOSQ_ERR_TLS;
	}
	/* Load CRLs if they exist. */
	만약(listener->crlfile){
		rc = net__load_crl_file(listener);
		만약(rc){
			반환 rc;
		}
	}
#아니면
	UNUSED(listener);
#endif
	반환 MOSQ_ERR_SUCCESS;
}


#만약 defined(WITH_TLS) && !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000


static 정수 net__load_engine(struct mosquitto__listener *listener)
{
	ENGINE *engine = NULL;
	UI_METHOD *ui_method;
	EVP_PKEY *pkey;

	만약(!listener->tls_engine){
		반환 MOSQ_ERR_SUCCESS;
	}

	engine = ENGINE_by_id(listener->tls_engine);
	만약(!engine){
		log__printf(NULL, MOSQ_LOG_ERR, "Error loading %s engine\n", listener->tls_engine);
		net__print_ssl_error(NULL, NULL);
		반환 MOSQ_ERR_TLS;
	}
	만약(!ENGINE_init(engine)){
		log__printf(NULL, MOSQ_LOG_ERR, "Failed engine initialisation\n");
		net__print_ssl_error(NULL, NULL);
		반환 MOSQ_ERR_TLS;
	}
	ENGINE_set_default(engine, ENGINE_METHOD_ALL);

	만약(listener->tls_keyform == mosq_k_engine){
		ui_method = net__get_ui_method();
		만약(listener->tls_engine_kpass_sha1){
			만약(!ENGINE_ctrl_cmd(engine, ENGINE_SECRET_MODE, ENGINE_SECRET_MODE_SHA, NULL, NULL, 0)){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to set engine secret mode sha");
				net__print_ssl_error(NULL, NULL);
				반환 MOSQ_ERR_TLS;
			}
			만약(!ENGINE_ctrl_cmd(engine, ENGINE_PIN, 0, listener->tls_engine_kpass_sha1, NULL, 0)){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to set engine pin");
				net__print_ssl_error(NULL, NULL);
				반환 MOSQ_ERR_TLS;
			}
			ui_method = NULL;
		}
		pkey = ENGINE_load_private_key(engine, listener->keyfile, ui_method, NULL);
		만약(!pkey){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to load engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(NULL, NULL);
			반환 MOSQ_ERR_TLS;
		}
		만약(SSL_CTX_use_PrivateKey(listener->ssl_ctx, pkey) <= 0){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to use engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(NULL, NULL);
			반환 MOSQ_ERR_TLS;
		}
	}
	ENGINE_free(engine); /* release the structural reference from ENGINE_by_id() */

	반환 MOSQ_ERR_SUCCESS;
}
#endif


정수 net__tls_load_verify(struct mosquitto__listener *listener)
{
#ifdef WITH_TLS
	정수 rc;

#  만약 OPENSSL_VERSION_NUMBER < 0x30000000L
	만약(listener->cafile || listener->capath){
		rc = SSL_CTX_load_verify_locations(listener->ssl_ctx, listener->cafile, listener->capath);
		만약(rc == 0){
			만약(listener->cafile && listener->capath){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\" and capath \"%s\".", listener->cafile, listener->capath);
			}아니면 만약(listener->cafile){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			}아니면{
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			}
		}
	}
#  아니면
	STACK_OF(X509_NAME) *ca_names = NULL;
	만약(listener->cafile || listener->capath){
		ca_names = sk_X509_NAME_new_null();
	}

	만약(listener->cafile){
		rc = SSL_CTX_load_verify_file(listener->ssl_ctx, listener->cafile);
		만약(rc == 0){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			net__print_ssl_error(NULL, NULL);
			반환 MOSQ_ERR_TLS;
		}

		STACK_OF(X509_NAME) *cert_names = SSL_load_client_CA_file(listener->cafile);
		만약(cert_names){
			위해(정수 i=0; i<sk_X509_NAME_num(cert_names); i++){
				sk_X509_NAME_push(ca_names, X509_NAME_dup(sk_X509_NAME_value(cert_names, i)));
			}
			sk_X509_NAME_pop_free(cert_names, X509_NAME_free);
		}

	}
	만약(listener->capath){
		rc = SSL_CTX_load_verify_dir(listener->ssl_ctx, listener->capath);
		만약(rc == 0){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			net__print_ssl_error(NULL, NULL);
			sk_X509_NAME_pop_free(ca_names, X509_NAME_free);
			반환 MOSQ_ERR_TLS;
		}

		SSL_add_dir_cert_subjects_to_stack(ca_names, listener->capath);
	}
	만약(ca_names){
		SSL_CTX_set_client_CA_list(listener->ssl_ctx, ca_names);
	}
#  endif

#  만약 !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000
	만약(net__load_engine(listener)){
		반환 MOSQ_ERR_TLS;
	}
#  endif
#endif
	반환 net__load_certificates(listener);
}


#만약 !defined(WIN32) && !defined(_AIX)


static 정수 net__bind_interface(struct mosquitto__listener *listener, struct addrinfo *rp)
{
	/*
	 * This binds the listener sock to a network interface.
	 * The use of SO_BINDTODEVICE requires root access, which we don't have, so instead
	 * use getifaddrs to find the interface addresses, and use IP of the
	 * matching interface in the later bind().
	 */
	struct ifaddrs *ifaddr;
	bool have_interface = false;

	만약(getifaddrs(&ifaddr) < 0){
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		반환 MOSQ_ERR_ERRNO;
	}

	위해(struct ifaddrs *ifa=ifaddr; ifa!=NULL; ifa=ifa->ifa_next){
		만약(ifa->ifa_addr == NULL){
			continue;
		}

		만약(!strcasecmp(listener->bind_interface, ifa->ifa_name)){
			have_interface = true;

			만약(ifa->ifa_addr->sa_family == rp->ai_addr->sa_family){
				만약(rp->ai_addr->sa_family == AF_INET){
					만약(listener->host &&
							memcmp(&((struct sockaddr_in *)rp->ai_addr)->sin_addr,
							&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr,
							sizeof(struct in_addr))){

						log__printf(NULL, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						반환 MOSQ_ERR_INVAL;
					}아니면{
						memcpy(&((struct sockaddr_in *)rp->ai_addr)->sin_addr,
								&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr,
								sizeof(struct in_addr));

						freeifaddrs(ifaddr);
						반환 MOSQ_ERR_SUCCESS;
					}
				}아니면 만약(rp->ai_addr->sa_family == AF_INET6){
					만약(listener->host &&
							memcmp(&((struct sockaddr_in6 *)rp->ai_addr)->sin6_addr,
							&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
							sizeof(struct in6_addr))){

						log__printf(NULL, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						반환 MOSQ_ERR_INVAL;
					}아니면{
						memcpy(&((struct sockaddr_in6 *)rp->ai_addr)->sin6_addr,
								&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
								sizeof(struct in6_addr));

						((struct sockaddr_in6 *)rp->ai_addr)->sin6_scope_id = ((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_scope_id;

						freeifaddrs(ifaddr);
						반환 MOSQ_ERR_SUCCESS;
					}
				}
			}
		}
	}
	freeifaddrs(ifaddr);
	만약(have_interface){
		log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Interface %s does not support %s configuration.",
				listener->bind_interface, rp->ai_addr->sa_family == AF_INET ? "IPv4" : "IPv6");
		반환 MOSQ_ERR_NOT_SUPPORTED;
	}아니면{
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Interface %s does not exist.", listener->bind_interface);
		반환 MOSQ_ERR_NOT_FOUND;
	}
}
#endif


static 정수 net__socket_listen_tcp(struct mosquitto__listener *listener)
{
	mosq_sock_t sock = INVALID_SOCKET;
	struct addrinfo hints;
	struct addrinfo *ainfo, *rp;
	char service[10];
	정수 rc;
	정수 ss_opt = 1;
#ifndef WIN32
	bool interface_bound = false;
#endif

	만약(!listener){
		반환 MOSQ_ERR_INVAL;
	}

	snprintf(service, 10, "%d", listener->port);
	memset(&hints, 0, sizeof(struct addrinfo));
	만약(listener->socket_domain){
		hints.ai_family = listener->socket_domain;
	}아니면{
		hints.ai_family = AF_UNSPEC;
	}
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	rc = getaddrinfo(listener->host, service, &hints, &ainfo);
	만약(rc){
		log__printf(NULL, MOSQ_LOG_ERR, "Error creating listener: %s.", gai_strerror(rc));
		반환 INVALID_SOCKET;
	}

	listener->sock_count = 0;
	listener->socks = NULL;

	위해(rp = ainfo; rp; rp = rp->ai_next){
		만약(rp->ai_family == AF_INET){
			log__printf(NULL, MOSQ_LOG_INFO, "Opening ipv4 listen socket on port %d.", ntohs(((struct sockaddr_in *)rp->ai_addr)->sin_port));
		}아니면 만약(rp->ai_family == AF_INET6){
			log__printf(NULL, MOSQ_LOG_INFO, "Opening ipv6 listen socket on port %d.", ntohs(((struct sockaddr_in6 *)rp->ai_addr)->sin6_port));
		}아니면{
			continue;
		}

		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		만약(sock == INVALID_SOCKET){
			net__print_error(MOSQ_LOG_WARNING, "Warning: %s");
			continue;
		}
		listener->sock_count++;
		listener->socks = mosquitto_realloc(listener->socks, sizeof(mosq_sock_t)*(size_t)listener->sock_count);
		만약(!listener->socks){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			반환 MOSQ_ERR_NOMEM;
		}
		listener->socks[listener->sock_count-1] = sock;

#ifndef WIN32
		ss_opt = 1;
		/* Unimportant if this fails */
		(void)setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&ss_opt, sizeof(ss_opt));
#endif
#ifdef IPV6_V6ONLY
		ss_opt = 1;
		(void)setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (const void *)&ss_opt, sizeof(ss_opt));
#endif

		만약(net__socket_nonblock(&sock)){
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			반환 1;
		}

#만약 !defined(WIN32) && !defined(_AIX)
		만약(listener->bind_interface){
			/* It might be possible that an interface does not support all relevant sa_families.
			 * We should successfully find at least one. */
			rc = net__bind_interface(listener, rp);
			만약(rc){
				COMPAT_CLOSE(sock);
				listener->sock_count--;
				만약(rc == MOSQ_ERR_NOT_FOUND || rc == MOSQ_ERR_INVAL){
					freeaddrinfo(ainfo);
					반환 rc;
				}아니면{
					continue;
				}
			}
			interface_bound = true;
		}
#endif

		만약(bind(sock, rp->ai_addr, rp->ai_addrlen) == -1){
#만약 defined(__linux__)
			만약(errno == EACCES){
				log__printf(NULL, MOSQ_LOG_ERR, "If you are trying to bind to a privileged port (<1024), try using setcap and do not start the broker as root:");
				log__printf(NULL, MOSQ_LOG_ERR, "    sudo setcap 'CAP_NET_BIND_SERVICE=+ep /usr/sbin/mosquitto'");
			}
#endif
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			COMPAT_CLOSE(sock);
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			반환 1;
		}

		만약(listen(sock, 100) == -1){
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			mosquitto_FREE(listener->socks);
			반환 1;
		}
	}
	freeaddrinfo(ainfo);

#ifndef WIN32
	만약(listener->bind_interface && !interface_bound){
		mosquitto_FREE(listener->socks);
		반환 1;
	}
#endif

	반환 0;
}


#ifdef WITH_UNIX_SOCKETS


static 정수 net__socket_listen_unix(struct mosquitto__listener *listener)
{
	struct sockaddr_un addr;
	mosq_sock_t sock = INVALID_SOCKET;
	정수 rc;
#ifndef WIN32
	mode_t old_mask;
#endif

	만약(listener->unix_socket_path == NULL){
		반환 MOSQ_ERR_INVAL;
	}
	만약(strlen(listener->unix_socket_path) > sizeof(addr.sun_path)-1){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Path to unix socket is too long \"%s\".", listener->unix_socket_path);
		반환 MOSQ_ERR_INVAL;
	}

#ifdef WIN32
	DeleteFile(listener->unix_socket_path);
#아니면
	unlink(listener->unix_socket_path);
#endif
	log__printf(NULL, MOSQ_LOG_INFO, "Opening unix listen socket on path %s.", listener->unix_socket_path);
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, listener->unix_socket_path, sizeof(addr.sun_path)-1);

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	만약(sock == INVALID_SOCKET){
		net__print_error(MOSQ_LOG_ERR, "Error creating unix socket: %s");
		반환 1;
	}
	listener->sock_count++;
	listener->socks = mosquitto_realloc(listener->socks, sizeof(mosq_sock_t)*(size_t)listener->sock_count);
	만약(!listener->socks){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		COMPAT_CLOSE(sock);
		반환 MOSQ_ERR_NOMEM;
	}
	listener->socks[listener->sock_count-1] = sock;


#ifndef WIN32
	old_mask = umask(0007);
#endif
	rc = bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_un));
#ifndef WIN32
	umask(old_mask);
#endif

	만약(rc == -1){
		net__print_error(MOSQ_LOG_ERR, "Error binding unix socket: %s");
		반환 1;
	}

	만약(listen(sock, 10) == -1){
		net__print_error(MOSQ_LOG_ERR, "Error listening to unix socket: %s");
		반환 1;
	}

	만약(net__socket_nonblock(&sock)){
		반환 1;
	}

	반환 0;
}
#endif


/* Creates a socket and listens on port 'port'.
 * Returns 1 on failure
 * Returns 0 on success.
 */
정수 net__socket_listen(struct mosquitto__listener *listener)
{
	정수 rc;

	만약(!listener){
		반환 MOSQ_ERR_INVAL;
	}

#ifdef WITH_UNIX_SOCKETS
	만약(listener->port == 0 && listener->unix_socket_path != NULL){
		rc = net__socket_listen_unix(listener);
	}아니면
#endif
	{
		rc = net__socket_listen_tcp(listener);
	}
	만약(rc){
		반환 rc;
	}

	/* We need to have at least one working socket. */
	만약(listener->sock_count > 0){
#ifdef WITH_TLS
		만약(listener->certfile && listener->keyfile){
			만약(net__tls_server_ctx(listener)){
				반환 1;
			}

			만약(net__tls_load_verify(listener)){
				반환 1;
			}
		}
#  ifdef FINAL_WITH_TLS_PSK
		만약(listener->psk_hint){
			만약(listener->certfile == NULL || listener->keyfile == NULL){
				만약(net__tls_server_ctx(listener)){
					반환 1;
				}
			}
			SSL_CTX_set_psk_server_callback(listener->ssl_ctx, psk_server_callback);
			만약(listener->psk_hint){
				rc = SSL_CTX_use_psk_identity_hint(listener->ssl_ctx, listener->psk_hint);
				만약(rc == 0){
					log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to set TLS PSK hint.");
					net__print_ssl_error(NULL, NULL);
					반환 1;
				}
			}
		}
#  endif /* FINAL_WITH_TLS_PSK */
		만약(tls_ex_index_context == -1){
			tls_ex_index_context = SSL_get_ex_new_index(0, "client context", NULL, NULL, NULL);
		}
		만약(tls_ex_index_listener == -1){
			tls_ex_index_listener = SSL_get_ex_new_index(0, "listener", NULL, NULL, NULL);
		}
#endif /* WITH_TLS */
		반환 0;
	}아니면{
		반환 1;
	}
}


정수 net__socket_get_address(mosq_sock_t sock, char *buf, size_t len, uint16_t *remote_port)
{
	struct sockaddr_storage addr;
	socklen_t addrlen;

	memset(&addr, 0, sizeof(struct sockaddr_storage));
	addrlen = sizeof(addr);
	만약(!getpeername(sock, (struct sockaddr *)&addr, &addrlen)){
		만약(addr.ss_family == AF_INET){
			만약(remote_port){
				*remote_port = ntohs(((struct sockaddr_in *)&addr)->sin_port);
			}
			만약(inet_ntop(AF_INET, &((struct sockaddr_in *)&addr)->sin_addr.s_addr, buf, (socklen_t)len)){
				반환 0;
			}
		}아니면 만약(addr.ss_family == AF_INET6){
			만약(remote_port){
				*remote_port = ntohs(((struct sockaddr_in6 *)&addr)->sin6_port);
			}
			만약(inet_ntop(AF_INET6, &((struct sockaddr_in6 *)&addr)->sin6_addr.s6_addr, buf, (socklen_t)len)){
				반환 0;
			}
#ifdef WITH_UNIX_SOCKETS
		}아니면 만약(addr.ss_family == AF_UNIX){
			struct sockaddr_un un;
			addrlen = sizeof(struct sockaddr_un);
			만약(!getsockname(sock, (struct sockaddr *)&un, &addrlen)){
				snprintf(buf, len, "%s", un.sun_path);
			}아니면{
				snprintf(buf, len, "unix-socket");
			}
			반환 0;
#endif
		}
	}
	반환 1;
}
