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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
#  include <arpa/inet.h>
#  ifndef _AIX
#    include <ifaddrs.h>
#  endif
#  include <netdb.h>
#  include <netinet/tcp.h>
#  include <strings.h>
#  include <sys/socket.h>
#  include <unistd.h>
#ਨਹੀਂਤਾਂ
#  include <winsock2.h>
#  include <ws2tcpip.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_WRAP
#  include <tcpd.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#  include <netinet/in.h>
#ਜੇ_ਅੰਤ

#ਜੇ defined(WITH_UNIX_SOCKETS) || defined(WITH_TLS)
#  include "sys/stat.h"
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_UNIX_SOCKETS
#  ifdef WIN32
#    include <afunix.h>
#  ਨਹੀਂਤਾਂ
#    include <sys/un.h>
#  endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __QNX__
#  include <net/netbyte.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "net_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
#  include "tls_mosq.h"
#  include <openssl/err.h>
ਸਥਿਰ ਪੂਰਨ tls_ex_ਸੂਚਕਾਂਕ_context = -1;
ਸਥਿਰ ਪੂਰਨ tls_ex_ਸੂਚਕਾਂਕ_listener = -1;
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "sys_tree.h"

/* For EMFILE handling */
ਸਥਿਰ mosq_sock_t ਵਾਧੂ_sock = INVALID_SOCKET;


ਖਾਲੀ net__broker_init(ਖਾਲੀ)
{
	ਵਾਧੂ_sock = socket(AF_INET, SOCK_STREAM, 0);
	net__init();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
	net__init_tls();
#ਜੇ_ਅੰਤ
}


ਖਾਲੀ net__broker_cleanup(ਖਾਲੀ)
{
	ਜੇ(ਵਾਧੂ_sock != INVALID_SOCKET){
		COMPAT_CLOSE(ਵਾਧੂ_sock);
		ਵਾਧੂ_sock = INVALID_SOCKET;
	}
	net__cleanup();
}


ਸਥਿਰ ਖਾਲੀ net__print_error(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਲਘੂਗਣਕ, ਅਚਲ ਅੱਖਰ *format_str)
{
	ਅੱਖਰ *buf;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
	Format_ਸੁਨੇਹਾ(FORMAT_ਸੁਨੇਹਾ_ALLOCATE_ਬਫਰ | FORMAT_ਸੁਨੇਹਾ_FROM_SYSTEM,
			ਨਿਰਾ, WSAGet_ਆਖਰੀ_Error(), LANG_NEUTRAL, (LPTSTR)&buf, 0, ਨਿਰਾ);

	log__printf(ਨਿਰਾ, ਲਘੂਗਣਕ, format_str, buf);
	LocalFree(buf);
#ਨਹੀਂਤਾਂ
	buf = strerror(errno);
	log__printf(ਨਿਰਾ, ਲਘੂਗਣਕ, format_str, buf);
#ਜੇ_ਅੰਤ
}


ਬਣਤਰ mosquitto *net__socket_accept(ਬਣਤਰ mosquitto__listener_sock *listensock)
{
	mosq_sock_t new_sock = INVALID_SOCKET;
	ਬਣਤਰ mosquitto *new_context;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
	BIO *bio;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_WRAP
	ਬਣਤਰ request_info wrap_req;
	ਅੱਖਰ ਪਤਾ[1024];
#ਜੇ_ਅੰਤ

	new_sock = accept(listensock->sock, ਨਿਰਾ, 0);
	ਜੇ(new_sock == INVALID_SOCKET){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
		WINDOWS_ਸੈੱਟ_ERRNO();
		ਜੇ(errno == WSAEMFILE){
#ਨਹੀਂਤਾਂ
		ਜੇ(errno == EMFILE || errno == ENFILE){
#ਜੇ_ਅੰਤ
			/* Close the spare socket, which means we should be able to accept
			 * this connection. Accept it, then close it immediately and create
			 * a new spare_sock. This prevents the situation of ever properly
			 * running out of sockets.
			 * It would be nice to send a "server not available" connack here,
			 * but there are lots of reasons why this would be tricky (TLS
			 * being the big one). */
			COMPAT_CLOSE(ਵਾਧੂ_sock);
			new_sock = accept(listensock->sock, ਨਿਰਾ, 0);
			ਜੇ(new_sock != INVALID_SOCKET){
				COMPAT_CLOSE(new_sock);
			}
			ਵਾਧੂ_sock = socket(AF_INET, SOCK_STREAM, 0);
			log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING,
					"Unable to accept new connection, system socket count has been exceeded. Try increasing \"ulimit -n\" or equivalent.");
		}
		ਵਾਪਸ ਨਿਰਾ;
	}

	metrics__int_inc(mosq_counter_socket_connections, 1);

	ਜੇ(net__socket_nonblock(&new_sock)){
		ਵਾਪਸ ਨਿਰਾ;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_WRAP
	/* Use tcpd / libwrap to determine whether a connection is allowed. */
	request_init(&wrap_req, RQ_FILE, new_sock, RQ_DAEMON, "mosquitto", 0);
	fromhost(&wrap_req);
	ਜੇ(!hosts_access(&wrap_req)){
		/* Access is denied */
		ਜੇ(db.config->connection_messages == true){
			ਜੇ(!net_socket_get_ਪਤਾ(new_sock, ਪਤਾ, 1024, ਨਿਰਾ)){
				log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client connection from %s denied access by tcpd.", ਪਤਾ);
			}
		}
		COMPAT_CLOSE(new_sock);
		ਵਾਪਸ ਨਿਰਾ;
	}
#ਜੇ_ਅੰਤ

	ਜੇ(db.config->ਸੈੱਟ_tcp_nodelay && listensock->listener->ਪੋਰਟ){
		ਪੂਰਨ flag = 1;
		ਜੇ(setsockopt(new_sock, IPPROTO_TCP, TCP_NODELAY, (ਅਚਲ ਖਾਲੀ *)&flag, ਆਕਾਰ(ਪੂਰਨ)) != 0){
			log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Unable to set TCP_NODELAY.");
		}
	}

	new_context = context__init();
	ਜੇ(!new_context){
		COMPAT_CLOSE(new_sock);
		ਵਾਪਸ ਨਿਰਾ;
	}
	new_context->listener = listensock->listener;
	ਜੇ(!new_context->listener){
		context__cleanup(new_context, true);
		ਵਾਪਸ ਨਿਰਾ;
	}
	new_context->listener->client_ਗਿਣਤੀ++;

	ਜੇ(new_context->listener->enable_proxy_protocol){
		ਜੇ(context__init_sock(new_context, new_sock, false) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			ਵਾਪਸ ਨਿਰਾ;
		}
		ਜੇ(new_context->listener->enable_proxy_protocol == 2){
			new_context->transport = mosq_t_proxy_v2;
		}ਨਹੀਂਤਾਂ{
			new_context->transport = mosq_t_proxy_v1;
		}
		new_context->proxy.cmd = -1;
	}ਨਹੀਂਤਾਂ{
		ਜੇ(context__init_sock(new_context, new_sock, true) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			ਵਾਪਸ ਨਿਰਾ;
		}
		ਚੋਣ(new_context->listener->protocol){
			ਹਾਲਤ mp_mqtt:
				new_context->transport = mosq_t_tcp;
				ਰੋਕੋ;
#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			ਹਾਲਤ mp_websockets:
				ਜੇ(http__context_init(new_context)){
					context__cleanup(new_context, true);
					ਵਾਪਸ ਨਿਰਾ;
				}
				ਰੋਕੋ;
#ਜੇ_ਅੰਤ
			ਮੂਲ:
				context__cleanup(new_context, true);
				ਵਾਪਸ ਨਿਰਾ;
		}
	}

	ਜੇ((new_context->listener->max_connections > 0 && new_context->listener->client_ਗਿਣਤੀ > new_context->listener->max_connections)
			|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)db.config->global_max_connections)){

		ਜੇ(db.config->connection_messages == true){
			log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", new_context->ਪਤਾ);
		}
		context__cleanup(new_context, true);
		ਵਾਪਸ ਨਿਰਾ;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
	/* TLS init */
	ਜੇ(new_context->listener->ssl_ctx){
		new_context->ssl = SSL_new(new_context->listener->ssl_ctx);
		ਜੇ(!new_context->ssl){
			context__cleanup(new_context, true);
			ਵਾਪਸ ਨਿਰਾ;
		}
		ਜੇ(!SSL_ਸੈੱਟ_ex_ਅੰਕੜੇ(new_context->ssl, tls_ex_ਸੂਚਕਾਂਕ_context, new_context)
				|| !SSL_ਸੈੱਟ_ex_ਅੰਕੜੇ(new_context->ssl, tls_ex_ਸੂਚਕਾਂਕ_listener, new_context->listener)){
			context__cleanup(new_context, true);
			ਵਾਪਸ ਨਿਰਾ;
		}
		new_context->want_write = true;
		bio = BIO_new_socket(new_sock, BIO_NOCLOSE);
		SSL_ਸੈੱਟ_bio(new_context->ssl, bio, bio);
		ERR_clear_error();
		SSL_ਸੈੱਟ_accept_state(new_context->ssl);
	}
#ਜੇ_ਅੰਤ

	ਜੇ(db.config->connection_messages == true
			&& !new_context->listener->enable_proxy_protocol){

		log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "New connection from %s:%d on port %d.",
				new_context->ਪਤਾ, new_context->remote_ਪੋਰਟ, new_context->listener->ਪੋਰਟ);
	}

	mux__new(new_context);
	keepalive_ਜੋੜੋ(new_context);

	ਵਾਪਸ new_context;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS


ਸਥਿਰ ਪੂਰਨ client_certificate_verify(ਪੂਰਨ preverify_ok, X509_STORE_CTX *ctx)
{
	/* Preverify should check expiry, revocation. */
	ਜੇ(preverify_ok == 0){
		SSL *ssl;
		ssl = X509_STORE_CTX_get_ex_ਅੰਕੜੇ(ctx, SSL_get_ex_ਅੰਕੜੇ_X509_STORE_CTX_idx());
		ਜੇ(ssl){
			ਬਣਤਰ mosquitto__listener *listener;
			listener = SSL_get_ex_ਅੰਕੜੇ(ssl, tls_ex_ਸੂਚਕਾਂਕ_listener);

			ਜੇ(listener && listener->disable_client_cert_date_checks){
				ਪੂਰਨ err = X509_STORE_CTX_get_error(ctx);
				ਜੇ(err == X509_V_ERR_CERT_NOT_YET_VALID || err == X509_V_ERR_CERT_HAS_EXPIRED){
					preverify_ok = 1;
				}
			}
		}
	}
	ਵਾਪਸ preverify_ok;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FINAL_WITH_TLS_PSK


ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ psk_server_callback(SSL *ssl, ਅਚਲ ਅੱਖਰ *ਪਛਾਣ_ਤੱਤ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *psk, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ max_psk_len)
{
	ਬਣਤਰ mosquitto *context;
	ਬਣਤਰ mosquitto__listener *listener;
	ਅੱਖਰ *psk_ਕੁੰਜੀ = ਨਿਰਾ;
	ਪੂਰਨ len;
	ਅਚਲ ਅੱਖਰ *psk_hint;

	ਜੇ(!ਪਛਾਣ_ਤੱਤ){
		ਵਾਪਸ 0;
	}

	context = SSL_get_ex_ਅੰਕੜੇ(ssl, tls_ex_ਸੂਚਕਾਂਕ_context);
	ਜੇ(!context){
		ਵਾਪਸ 0;
	}

	listener = SSL_get_ex_ਅੰਕੜੇ(ssl, tls_ex_ਸੂਚਕਾਂਕ_listener);
	ਜੇ(!listener){
		ਵਾਪਸ 0;
	}

	psk_hint = listener->psk_hint;

	/* The hex to BN conversion results in the length halving, so we can pass
	 * max_psk_len*2 as the max hex key here. */
	psk_ਕੁੰਜੀ = mosquitto_calloc(1, (size_t)max_psk_len*2 + 1);
	ਜੇ(!psk_ਕੁੰਜੀ){
		ਵਾਪਸ 0;
	}

	ਜੇ(mosquitto_psk_ਕੁੰਜੀ_get(context, psk_hint, ਪਛਾਣ_ਤੱਤ, psk_ਕੁੰਜੀ, (ਪੂਰਨ)max_psk_len*2) != MOSQ_ERR_SUCCESS){
		mosquitto_FREE(psk_ਕੁੰਜੀ);
		ਵਾਪਸ 0;
	}

	len = mosquitto__hex2bin(psk_ਕੁੰਜੀ, psk, (ਪੂਰਨ)max_psk_len);
	ਜੇ(len < 0){
		mosquitto_FREE(psk_ਕੁੰਜੀ);
		ਵਾਪਸ 0;
	}

	ਜੇ(listener->use_ਪਛਾਣ_ਤੱਤ_as_username){
		ਜੇ(mosquitto_validate_utf8(ਪਛਾਣ_ਤੱਤ, (ਪੂਰਨ)ਲੜੀ_ਲੰਬਾਈ(ਪਛਾਣ_ਤੱਤ))){
			mosquitto_free(psk_ਕੁੰਜੀ);
			ਵਾਪਸ 0;
		}
		context->username = mosquitto_strdup(ਪਛਾਣ_ਤੱਤ);
		ਜੇ(!context->username){
			mosquitto_FREE(psk_ਕੁੰਜੀ);
			ਵਾਪਸ 0;
		}
	}

	mosquitto_FREE(psk_ਕੁੰਜੀ);
	ਵਾਪਸ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)len;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS


ਸਥਿਰ ਖਾਲੀ tls_keylog_callback(ਅਚਲ SSL *ssl, ਅਚਲ ਅੱਖਰ *ਸਤਰ)
{
	ਅਣਵਰਤਿਆ(ssl);

	ਜੇ(db.tls_keylog){
		FILE *fptr;
		fptr = mosquitto_fopen(db.tls_keylog, "at", true);
		ਜੇ(fptr){
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
			/* Until mosquitto_fopen enforces file permissions on all files,
			 * enforce it here. We can enforce it here, because it isn't a
			 * change of behaviour. */
			ਬਣਤਰ stat statbuf;
			ਜੇ(fstat(fileno(fptr), &statbuf) < 0
					|| statbuf.st_ਬਹੁਲਕ & (S_IRWXG | S_IRWXO)){

				fclose(fptr);
				ਵਾਪਸ;
			}
#ਜੇ_ਅੰਤ
			fprintf(fptr, "%s\n", ਸਤਰ);
			fclose(fptr);
		}ਨਹੀਂਤਾਂ{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
			ਜੇ(errno == ELOOP){
				log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Error: keylog file must not be a symbolic link");
			}ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
			{
				log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Error: Unable to open keylog file: %s", strerror(errno));
			}
		}
	}
}


ਪੂਰਨ net__tls_server_ctx(ਬਣਤਰ mosquitto__listener *listener)
{
	ਅੱਖਰ buf[256];
	ਪੂਰਨ rc;

	ਜੇ(tls_ex_ਸੂਚਕਾਂਕ_context == -1){
		tls_ex_ਸੂਚਕਾਂਕ_context = SSL_get_ex_new_ਸੂਚਕਾਂਕ(0, "client context", ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
	}
	ਜੇ(tls_ex_ਸੂਚਕਾਂਕ_listener == -1){
		tls_ex_ਸੂਚਕਾਂਕ_listener = SSL_get_ex_new_ਸੂਚਕਾਂਕ(0, "listener", ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
	}

	ਜੇ(listener->ssl_ctx){
		SSL_CTX_free(listener->ssl_ctx);
		listener->ssl_ctx = ਨਿਰਾ;
	}

	listener->ssl_ctx = SSL_CTX_new(TLS_server_ਵਿਧੀ());

	ਜੇ(!listener->ssl_ctx){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to create TLS context.");
		ਵਾਪਸ MOSQ_ERR_TLS;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SSL_OP_NO_TLSv1_3
	ਜੇ(db.config->ਪ੍ਰਤੀ_listener_settings){
		ਜੇ(listener->security_options->psk_file){
			SSL_CTX_ਸੈੱਟ_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}ਨਹੀਂਤਾਂ{
		ਜੇ(db.config->security_options.psk_file){
			SSL_CTX_ਸੈੱਟ_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}
#ਜੇ_ਅੰਤ

	ਜੇ(listener->tls_ਸੰਸਕਰਣ == ਨਿਰਾ){
		SSL_CTX_ਸੈੱਟ_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SSL_OP_NO_TLSv1_3
	}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(listener->tls_ਸੰਸਕਰਣ, "tlsv1.3")){
		SSL_CTX_ਸੈੱਟ_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2);
#ਜੇ_ਅੰਤ
	}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(listener->tls_ਸੰਸਕਰਣ, "tlsv1.2")){
		SSL_CTX_ਸੈੱਟ_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
	}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(listener->tls_ਸੰਸਕਰਣ, "tlsv1.1")){
		SSL_CTX_ਸੈੱਟ_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1);
		log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: TLS v1.1 is insecure. It must only be used in the case "
				"where you have devices that cannot be upgraded to use a secure version of TLS. It is "
				"recommended to use as secure a set of ciphers as possible and that will restrict it to "
				"TLS v1.1 only, and to use a separate listener using TLS v1.2 and secure ciphers for your "
				"other devices.");
		log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Please be aware that support for TLS v1.1 will go away eventually "
				"and that you should plan now to migrate away from it.");
	}ਨਹੀਂਤਾਂ{
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unsupported tls_version \"%s\".", listener->tls_ਸੰਸਕਰਣ);
		ਵਾਪਸ MOSQ_ERR_TLS;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SSL_OP_NO_COMPRESSION
	/* Disable compression */
	SSL_CTX_ਸੈੱਟ_options(listener->ssl_ctx, SSL_OP_NO_COMPRESSION);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SSL_OP_CIPHER_SERVER_PREFERENCE
	/* Server chooses cipher */
	SSL_CTX_ਸੈੱਟ_options(listener->ssl_ctx, SSL_OP_CIPHER_SERVER_PREFERENCE);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SSL_ਬਹੁਲਕ_RELEASE_BUFFERS
	/* Use even less memory per SSL connection. */
	SSL_CTX_ਸੈੱਟ_ਬਹੁਲਕ(listener->ssl_ctx, SSL_ਬਹੁਲਕ_RELEASE_BUFFERS);
#ਜੇ_ਅੰਤ

	SSL_CTX_ਸੈੱਟ_dh_auto(listener->ssl_ctx, 1);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SSL_OP_NO_RENEGOTIATION
	SSL_CTX_ਸੈੱਟ_options(listener->ssl_ctx, SSL_OP_NO_RENEGOTIATION);
#ਜੇ_ਅੰਤ

	ਜੇ(db.tls_keylog){
		log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "TLS key logging to '%s' enabled for all listeners.",
				db.tls_keylog);
		log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "TLS key logging is for DEBUGGING only.");

		SSL_CTX_ਸੈੱਟ_keylog_callback(listener->ssl_ctx, tls_keylog_callback);
	}

	snprintf(buf, 256, "mosquitto-%d", listener->ਪੋਰਟ);
	SSL_CTX_ਸੈੱਟ_session_id_context(listener->ssl_ctx, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)buf, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)ਲੜੀ_ਲੰਬਾਈ(buf));

	ਜੇ(listener->ciphers){
		rc = SSL_CTX_ਸੈੱਟ_cipher_list(listener->ssl_ctx, listener->ciphers);
		ਜੇ(rc == 0){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			ਵਾਪਸ MOSQ_ERR_TLS;
		}
	}ਨਹੀਂਤਾਂ{
		rc = SSL_CTX_ਸੈੱਟ_cipher_list(listener->ssl_ctx, "DEFAULT:!aNULL:!eNULL:!LOW:!EXPORT:!SSLv2:@STRENGTH");
		ਜੇ(rc == 0){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			ਵਾਪਸ MOSQ_ERR_TLS;
		}
	}
	ਜੇ(listener->ciphers_tls13){
		rc = SSL_CTX_ਸੈੱਟ_ciphersuites(listener->ssl_ctx, listener->ciphers_tls13);
		ਜੇ(rc == 0){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to set TLS 1.3 ciphersuites. Check cipher_tls13 list \"%s\".", listener->ciphers_tls13);
			ਵਾਪਸ MOSQ_ERR_TLS;
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}
#ਜੇ_ਅੰਤ


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS


ਸਥਿਰ ਪੂਰਨ net_ਲੱਦੋ_crl_file(ਬਣਤਰ mosquitto__listener *listener)
{
	X509_STORE *store;
	X509_LOOKUP *lookup;
	ਪੂਰਨ rc;

	store = SSL_CTX_get_cert_store(listener->ssl_ctx);
	ਜੇ(!store){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to obtain TLS store.");
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		ਵਾਪਸ MOSQ_ERR_TLS;
	}
	lookup = X509_STORE_ਜੋੜੋ_lookup(store, X509_LOOKUP_file());
	rc = X509_ਲੱਦੋ_crl_file(lookup, listener->crlfile, X509_FILETYPE_PEM);
	ਜੇ(rc < 1){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to load certificate revocation file \"%s\". Check crlfile.", listener->crlfile);
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		net__print_ssl_error(ਨਿਰਾ, ਨਿਰਾ);
		ਵਾਪਸ MOSQ_ERR_TLS;
	}
	X509_STORE_ਸੈੱਟ_flags(store, X509_V_FLAG_CRL_CHECK);

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}
#ਜੇ_ਅੰਤ


ਪੂਰਨ net_ਲੱਦੋ_certificates(ਬਣਤਰ mosquitto__listener *listener)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
	ਪੂਰਨ rc;

	ਜੇ(listener->require_certificate){
		SSL_CTX_ਸੈੱਟ_verify(listener->ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, client_certificate_verify);
	}ਨਹੀਂਤਾਂ{
		SSL_CTX_ਸੈੱਟ_verify(listener->ssl_ctx, SSL_VERIFY_NONE, client_certificate_verify);
	}
	rc = SSL_CTX_use_certificate_chain_file(listener->ssl_ctx, listener->certfile);
	ਜੇ(rc != 1){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
		net__print_ssl_error(ਨਿਰਾ, ਨਿਰਾ);
		ਵਾਪਸ MOSQ_ERR_TLS;
	}
	ਜੇ(listener->tls_engine == ਨਿਰਾ || listener->tls_keyform == mosq_k_pem){
		rc = SSL_CTX_use_ਨਿੱਜੀ_ਕੁੰਜੀ_file(listener->ssl_ctx, listener->keyfile, SSL_FILETYPE_PEM);
		ਜੇ(rc != 1){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			net__print_ssl_error(ਨਿਰਾ, ਨਿਰਾ);
			ਵਾਪਸ MOSQ_ERR_TLS;
		}
	}
	rc = SSL_CTX_check_ਨਿੱਜੀ_ਕੁੰਜੀ(listener->ssl_ctx);
	ਜੇ(rc != 1){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Server certificate/key are inconsistent.");
		net__print_ssl_error(ਨਿਰਾ, ਨਿਰਾ);
		ਵਾਪਸ MOSQ_ERR_TLS;
	}
	/* Load CRLs if they exist. */
	ਜੇ(listener->crlfile){
		rc = net_ਲੱਦੋ_crl_file(listener);
		ਜੇ(rc){
			ਵਾਪਸ rc;
		}
	}
#ਨਹੀਂਤਾਂ
	ਅਣਵਰਤਿਆ(listener);
#ਜੇ_ਅੰਤ
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


#ਜੇ defined(WITH_TLS) && !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000


ਸਥਿਰ ਪੂਰਨ net_ਲੱਦੋ_engine(ਬਣਤਰ mosquitto__listener *listener)
{
	ENGINE *engine = ਨਿਰਾ;
	UI_ਵਿਧੀ *ui_ਵਿਧੀ;
	EVP_PKEY *pkey;

	ਜੇ(!listener->tls_engine){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

	engine = ENGINE_by_id(listener->tls_engine);
	ਜੇ(!engine){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error loading %s engine\n", listener->tls_engine);
		net__print_ssl_error(ਨਿਰਾ, ਨਿਰਾ);
		ਵਾਪਸ MOSQ_ERR_TLS;
	}
	ਜੇ(!ENGINE_init(engine)){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Failed engine initialisation\n");
		net__print_ssl_error(ਨਿਰਾ, ਨਿਰਾ);
		ਵਾਪਸ MOSQ_ERR_TLS;
	}
	ENGINE_ਸੈੱਟ_default(engine, ENGINE_ਵਿਧੀ_ALL);

	ਜੇ(listener->tls_keyform == mosq_k_engine){
		ui_ਵਿਧੀ = net_get_ui_ਵਿਧੀ();
		ਜੇ(listener->tls_engine_kpass_sha1){
			ਜੇ(!ENGINE_ctrl_cmd(engine, ENGINE_SECRET_ਬਹੁਲਕ, ENGINE_SECRET_ਬਹੁਲਕ_SHA, ਨਿਰਾ, ਨਿਰਾ, 0)){
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to set engine secret mode sha");
				net__print_ssl_error(ਨਿਰਾ, ਨਿਰਾ);
				ਵਾਪਸ MOSQ_ERR_TLS;
			}
			ਜੇ(!ENGINE_ctrl_cmd(engine, ENGINE_PIN, 0, listener->tls_engine_kpass_sha1, ਨਿਰਾ, 0)){
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to set engine pin");
				net__print_ssl_error(ਨਿਰਾ, ਨਿਰਾ);
				ਵਾਪਸ MOSQ_ERR_TLS;
			}
			ui_ਵਿਧੀ = ਨਿਰਾ;
		}
		pkey = ENGINE_ਲੱਦੋ_ਨਿੱਜੀ_ਕੁੰਜੀ(engine, listener->keyfile, ui_ਵਿਧੀ, ਨਿਰਾ);
		ਜੇ(!pkey){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to load engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(ਨਿਰਾ, ਨਿਰਾ);
			ਵਾਪਸ MOSQ_ERR_TLS;
		}
		ਜੇ(SSL_CTX_use_ਨਿੱਜੀ_ਕੁੰਜੀ(listener->ssl_ctx, pkey) <= 0){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to use engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(ਨਿਰਾ, ਨਿਰਾ);
			ਵਾਪਸ MOSQ_ERR_TLS;
		}
	}
	ENGINE_free(engine); /* release the structural reference from ENGINE_by_id() */

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}
#ਜੇ_ਅੰਤ


ਪੂਰਨ net_tls_ਲੱਦੋ_verify(ਬਣਤਰ mosquitto__listener *listener)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
	ਪੂਰਨ rc;

#  ਜੇ OPENSSL_ਸੰਸਕਰਣ_ਸੰਖਿਆ < 0x30000000L
	ਜੇ(listener->cafile || listener->capath){
		rc = SSL_CTX_ਲੱਦੋ_verify_locations(listener->ssl_ctx, listener->cafile, listener->capath);
		ਜੇ(rc == 0){
			ਜੇ(listener->cafile && listener->capath){
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\" and capath \"%s\".", listener->cafile, listener->capath);
			}ਨਹੀਂਤਾਂ ਜੇ(listener->cafile){
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			}ਨਹੀਂਤਾਂ{
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			}
		}
	}
#  ਨਹੀਂਤਾਂ
	ਢੇਰ_ਦਾ(X509_NAME) *ca_ਨਾਂ = ਨਿਰਾ;
	ਜੇ(listener->cafile || listener->capath){
		ca_ਨਾਂ = sk_X509_NAME_new_null();
	}

	ਜੇ(listener->cafile){
		rc = SSL_CTX_ਲੱਦੋ_verify_file(listener->ssl_ctx, listener->cafile);
		ਜੇ(rc == 0){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			net__print_ssl_error(ਨਿਰਾ, ਨਿਰਾ);
			ਵਾਪਸ MOSQ_ERR_TLS;
		}

		ਢੇਰ_ਦਾ(X509_NAME) *cert_ਨਾਂ = SSL_ਲੱਦੋ_client_CA_file(listener->cafile);
		ਜੇ(cert_ਨਾਂ){
			ਲਈ(ਪੂਰਨ i=0; i<sk_X509_NAME_ਅੰਕ(cert_ਨਾਂ); i++){
				sk_X509_NAME_ਧੱਕੋ(ca_ਨਾਂ, X509_NAME_dup(sk_X509_NAME_ਮੁੱਲ(cert_ਨਾਂ, i)));
			}
			sk_X509_NAME_ਕੱਢੋ_free(cert_ਨਾਂ, X509_NAME_free);
		}

	}
	ਜੇ(listener->capath){
		rc = SSL_CTX_ਲੱਦੋ_verify_dir(listener->ssl_ctx, listener->capath);
		ਜੇ(rc == 0){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			net__print_ssl_error(ਨਿਰਾ, ਨਿਰਾ);
			sk_X509_NAME_ਕੱਢੋ_free(ca_ਨਾਂ, X509_NAME_free);
			ਵਾਪਸ MOSQ_ERR_TLS;
		}

		SSL_ਜੋੜੋ_dir_cert_subjects_ਮੰਜ਼ਿਲ_ਢੇਰ(ca_ਨਾਂ, listener->capath);
	}
	ਜੇ(ca_ਨਾਂ){
		SSL_CTX_ਸੈੱਟ_client_CA_list(listener->ssl_ctx, ca_ਨਾਂ);
	}
#  endif

#  ਜੇ !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000
	ਜੇ(net_ਲੱਦੋ_engine(listener)){
		ਵਾਪਸ MOSQ_ERR_TLS;
	}
#  endif
#ਜੇ_ਅੰਤ
	ਵਾਪਸ net_ਲੱਦੋ_certificates(listener);
}


#ਜੇ !defined(WIN32) && !defined(_AIX)


ਸਥਿਰ ਪੂਰਨ net__bind_interface(ਬਣਤਰ mosquitto__listener *listener, ਬਣਤਰ addrinfo *rp)
{
	/*
	 * This binds the listener sock to a network interface.
	 * The use of SO_BINDTODEVICE requires root access, which we don't have, so instead
	 * use getifaddrs to find the interface addresses, and use IP of the
	 * matching interface in the later bind().
	 */
	ਬਣਤਰ ifaddrs *ifaddr;
	bool have_interface = false;

	ਜੇ(getifaddrs(&ifaddr) < 0){
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		ਵਾਪਸ MOSQ_ERR_ERRNO;
	}

	ਲਈ(ਬਣਤਰ ifaddrs *ifa=ifaddr; ifa!=ਨਿਰਾ; ifa=ifa->ifa_ਅਗਲਾ){
		ਜੇ(ifa->ifa_addr == ਨਿਰਾ){
			ਜਾਰੀ;
		}

		ਜੇ(!strcasecmp(listener->bind_interface, ifa->ifa_name)){
			have_interface = true;

			ਜੇ(ifa->ifa_addr->sa_family == rp->ai_addr->sa_family){
				ਜੇ(rp->ai_addr->sa_family == AF_INET){
					ਜੇ(listener->host &&
							memcmp(&((ਬਣਤਰ sockaddr_in *)rp->ai_addr)->sin_addr,
							&((ਬਣਤਰ sockaddr_in *)ifa->ifa_addr)->sin_addr,
							ਆਕਾਰ(ਬਣਤਰ in_addr))){

						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}ਨਹੀਂਤਾਂ{
						ਸਮ੍ਰਿਤੀ_ਨਕਲ(&((ਬਣਤਰ sockaddr_in *)rp->ai_addr)->sin_addr,
								&((ਬਣਤਰ sockaddr_in *)ifa->ifa_addr)->sin_addr,
								ਆਕਾਰ(ਬਣਤਰ in_addr));

						freeifaddrs(ifaddr);
						ਵਾਪਸ MOSQ_ERR_SUCCESS;
					}
				}ਨਹੀਂਤਾਂ ਜੇ(rp->ai_addr->sa_family == AF_INET6){
					ਜੇ(listener->host &&
							memcmp(&((ਬਣਤਰ sockaddr_in6 *)rp->ai_addr)->sin6_addr,
							&((ਬਣਤਰ sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
							ਆਕਾਰ(ਬਣਤਰ in6_addr))){

						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						ਵਾਪਸ MOSQ_ERR_INVAL;
					}ਨਹੀਂਤਾਂ{
						ਸਮ੍ਰਿਤੀ_ਨਕਲ(&((ਬਣਤਰ sockaddr_in6 *)rp->ai_addr)->sin6_addr,
								&((ਬਣਤਰ sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
								ਆਕਾਰ(ਬਣਤਰ in6_addr));

						((ਬਣਤਰ sockaddr_in6 *)rp->ai_addr)->sin6_scope_id = ((ਬਣਤਰ sockaddr_in6 *)ifa->ifa_addr)->sin6_scope_id;

						freeifaddrs(ifaddr);
						ਵਾਪਸ MOSQ_ERR_SUCCESS;
					}
				}
			}
		}
	}
	freeifaddrs(ifaddr);
	ਜੇ(have_interface){
		log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Interface %s does not support %s configuration.",
				listener->bind_interface, rp->ai_addr->sa_family == AF_INET ? "IPv4" : "IPv6");
		ਵਾਪਸ MOSQ_ERR_NOT_SUPPORTED;
	}ਨਹੀਂਤਾਂ{
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Interface %s does not exist.", listener->bind_interface);
		ਵਾਪਸ MOSQ_ERR_NOT_ਮਿਲਿਆ;
	}
}
#ਜੇ_ਅੰਤ


ਸਥਿਰ ਪੂਰਨ net__socket_listen_tcp(ਬਣਤਰ mosquitto__listener *listener)
{
	mosq_sock_t sock = INVALID_SOCKET;
	ਬਣਤਰ addrinfo hints;
	ਬਣਤਰ addrinfo *ainfo, *rp;
	ਅੱਖਰ service[10];
	ਪੂਰਨ rc;
	ਪੂਰਨ ss_opt = 1;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
	bool interface_bound = false;
#ਜੇ_ਅੰਤ

	ਜੇ(!listener){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	snprintf(service, 10, "%d", listener->ਪੋਰਟ);
	memset(&hints, 0, ਆਕਾਰ(ਬਣਤਰ addrinfo));
	ਜੇ(listener->socket_domain){
		hints.ai_family = listener->socket_domain;
	}ਨਹੀਂਤਾਂ{
		hints.ai_family = AF_UNSPEC;
	}
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	rc = getaddrinfo(listener->host, service, &hints, &ainfo);
	ਜੇ(rc){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error creating listener: %s.", gai_strerror(rc));
		ਵਾਪਸ INVALID_SOCKET;
	}

	listener->sock_ਗਿਣਤੀ = 0;
	listener->socks = ਨਿਰਾ;

	ਲਈ(rp = ainfo; rp; rp = rp->ai_ਅਗਲਾ){
		ਜੇ(rp->ai_family == AF_INET){
			log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Opening ipv4 listen socket on port %d.", ntohs(((ਬਣਤਰ sockaddr_in *)rp->ai_addr)->sin_port));
		}ਨਹੀਂਤਾਂ ਜੇ(rp->ai_family == AF_INET6){
			log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Opening ipv6 listen socket on port %d.", ntohs(((ਬਣਤਰ sockaddr_in6 *)rp->ai_addr)->sin6_port));
		}ਨਹੀਂਤਾਂ{
			ਜਾਰੀ;
		}

		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		ਜੇ(sock == INVALID_SOCKET){
			net__print_error(MOSQ_LOG_WARNING, "Warning: %s");
			ਜਾਰੀ;
		}
		listener->sock_ਗਿਣਤੀ++;
		listener->socks = mosquitto_realloc(listener->socks, ਆਕਾਰ(mosq_sock_t)*(size_t)listener->sock_ਗਿਣਤੀ);
		ਜੇ(!listener->socks){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}
		listener->socks[listener->sock_ਗਿਣਤੀ-1] = sock;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
		ss_opt = 1;
		/* Unimportant if this fails */
		(ਖਾਲੀ)setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (ਅਚਲ ਖਾਲੀ *)&ss_opt, ਆਕਾਰ(ss_opt));
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ IPV6_V6ONLY
		ss_opt = 1;
		(ਖਾਲੀ)setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (ਅਚਲ ਖਾਲੀ *)&ss_opt, ਆਕਾਰ(ss_opt));
#ਜੇ_ਅੰਤ

		ਜੇ(net__socket_nonblock(&sock)){
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			ਵਾਪਸ 1;
		}

#ਜੇ !defined(WIN32) && !defined(_AIX)
		ਜੇ(listener->bind_interface){
			/* It might be possible that an interface does not support all relevant sa_families.
			 * We should successfully find at least one. */
			rc = net__bind_interface(listener, rp);
			ਜੇ(rc){
				COMPAT_CLOSE(sock);
				listener->sock_ਗਿਣਤੀ--;
				ਜੇ(rc == MOSQ_ERR_NOT_ਮਿਲਿਆ || rc == MOSQ_ERR_INVAL){
					freeaddrinfo(ainfo);
					ਵਾਪਸ rc;
				}ਨਹੀਂਤਾਂ{
					ਜਾਰੀ;
				}
			}
			interface_bound = true;
		}
#ਜੇ_ਅੰਤ

		ਜੇ(bind(sock, rp->ai_addr, rp->ai_addrlen) == -1){
#ਜੇ defined(__linux__)
			ਜੇ(errno == EACCES){
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "If you are trying to bind to a privileged port (<1024), try using setcap and do not start the broker as root:");
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "    sudo setcap 'CAP_NET_BIND_SERVICE=+ep /usr/sbin/mosquitto'");
			}
#ਜੇ_ਅੰਤ
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			COMPAT_CLOSE(sock);
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			ਵਾਪਸ 1;
		}

		ਜੇ(listen(sock, 100) == -1){
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			mosquitto_FREE(listener->socks);
			ਵਾਪਸ 1;
		}
	}
	freeaddrinfo(ainfo);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
	ਜੇ(listener->bind_interface && !interface_bound){
		mosquitto_FREE(listener->socks);
		ਵਾਪਸ 1;
	}
#ਜੇ_ਅੰਤ

	ਵਾਪਸ 0;
}


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_UNIX_SOCKETS


ਸਥਿਰ ਪੂਰਨ net__socket_listen_unix(ਬਣਤਰ mosquitto__listener *listener)
{
	ਬਣਤਰ sockaddr_un addr;
	mosq_sock_t sock = INVALID_SOCKET;
	ਪੂਰਨ rc;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
	mode_t old_ਨਕਾਬ;
#ਜੇ_ਅੰਤ

	ਜੇ(listener->unix_socket_ਰਸਤਾ == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}
	ਜੇ(ਲੜੀ_ਲੰਬਾਈ(listener->unix_socket_ਰਸਤਾ) > ਆਕਾਰ(addr.sun_ਰਸਤਾ)-1){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Path to unix socket is too long \"%s\".", listener->unix_socket_ਰਸਤਾ);
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
	DeleteFile(listener->unix_socket_ਰਸਤਾ);
#ਨਹੀਂਤਾਂ
	unlink(listener->unix_socket_ਰਸਤਾ);
#ਜੇ_ਅੰਤ
	log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Opening unix listen socket on path %s.", listener->unix_socket_ਰਸਤਾ);
	memset(&addr, 0, ਆਕਾਰ(ਬਣਤਰ sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_ਰਸਤਾ, listener->unix_socket_ਰਸਤਾ, ਆਕਾਰ(addr.sun_ਰਸਤਾ)-1);

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	ਜੇ(sock == INVALID_SOCKET){
		net__print_error(MOSQ_LOG_ERR, "Error creating unix socket: %s");
		ਵਾਪਸ 1;
	}
	listener->sock_ਗਿਣਤੀ++;
	listener->socks = mosquitto_realloc(listener->socks, ਆਕਾਰ(mosq_sock_t)*(size_t)listener->sock_ਗਿਣਤੀ);
	ਜੇ(!listener->socks){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
		COMPAT_CLOSE(sock);
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}
	listener->socks[listener->sock_ਗਿਣਤੀ-1] = sock;


#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
	old_ਨਕਾਬ = umask(0007);
#ਜੇ_ਅੰਤ
	rc = bind(sock, (ਬਣਤਰ sockaddr *)&addr, ਆਕਾਰ(ਬਣਤਰ sockaddr_un));
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
	umask(old_ਨਕਾਬ);
#ਜੇ_ਅੰਤ

	ਜੇ(rc == -1){
		net__print_error(MOSQ_LOG_ERR, "Error binding unix socket: %s");
		ਵਾਪਸ 1;
	}

	ਜੇ(listen(sock, 10) == -1){
		net__print_error(MOSQ_LOG_ERR, "Error listening to unix socket: %s");
		ਵਾਪਸ 1;
	}

	ਜੇ(net__socket_nonblock(&sock)){
		ਵਾਪਸ 1;
	}

	ਵਾਪਸ 0;
}
#ਜੇ_ਅੰਤ


/* Creates a socket and listens on port 'port'.
 * Returns 1 on failure
 * Returns 0 on success.
 */
ਪੂਰਨ net__socket_listen(ਬਣਤਰ mosquitto__listener *listener)
{
	ਪੂਰਨ rc;

	ਜੇ(!listener){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_UNIX_SOCKETS
	ਜੇ(listener->ਪੋਰਟ == 0 && listener->unix_socket_ਰਸਤਾ != ਨਿਰਾ){
		rc = net__socket_listen_unix(listener);
	}ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	{
		rc = net__socket_listen_tcp(listener);
	}
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}

	/* We need to have at least one working socket. */
	ਜੇ(listener->sock_ਗਿਣਤੀ > 0){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
		ਜੇ(listener->certfile && listener->keyfile){
			ਜੇ(net__tls_server_ctx(listener)){
				ਵਾਪਸ 1;
			}

			ਜੇ(net_tls_ਲੱਦੋ_verify(listener)){
				ਵਾਪਸ 1;
			}
		}
#  ifdef FINAL_WITH_TLS_PSK
		ਜੇ(listener->psk_hint){
			ਜੇ(listener->certfile == ਨਿਰਾ || listener->keyfile == ਨਿਰਾ){
				ਜੇ(net__tls_server_ctx(listener)){
					ਵਾਪਸ 1;
				}
			}
			SSL_CTX_ਸੈੱਟ_psk_server_callback(listener->ssl_ctx, psk_server_callback);
			ਜੇ(listener->psk_hint){
				rc = SSL_CTX_use_psk_ਪਛਾਣ_ਤੱਤ_hint(listener->ssl_ctx, listener->psk_hint);
				ਜੇ(rc == 0){
					log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to set TLS PSK hint.");
					net__print_ssl_error(ਨਿਰਾ, ਨਿਰਾ);
					ਵਾਪਸ 1;
				}
			}
		}
#  endif /* FINAL_WITH_TLS_PSK */
		ਜੇ(tls_ex_ਸੂਚਕਾਂਕ_context == -1){
			tls_ex_ਸੂਚਕਾਂਕ_context = SSL_get_ex_new_ਸੂਚਕਾਂਕ(0, "client context", ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
		}
		ਜੇ(tls_ex_ਸੂਚਕਾਂਕ_listener == -1){
			tls_ex_ਸੂਚਕਾਂਕ_listener = SSL_get_ex_new_ਸੂਚਕਾਂਕ(0, "listener", ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
		}
#ਜੇ_ਅੰਤ /* WITH_TLS */
		ਵਾਪਸ 0;
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ 1;
	}
}


ਪੂਰਨ net_socket_get_ਪਤਾ(mosq_sock_t sock, ਅੱਖਰ *buf, size_t len, uint16_t *remote_ਪੋਰਟ)
{
	ਬਣਤਰ sockaddr_storage addr;
	socklen_t addrlen;

	memset(&addr, 0, ਆਕਾਰ(ਬਣਤਰ sockaddr_storage));
	addrlen = ਆਕਾਰ(addr);
	ਜੇ(!getpeername(sock, (ਬਣਤਰ sockaddr *)&addr, &addrlen)){
		ਜੇ(addr.ss_family == AF_INET){
			ਜੇ(remote_ਪੋਰਟ){
				*remote_ਪੋਰਟ = ntohs(((ਬਣਤਰ sockaddr_in *)&addr)->sin_port);
			}
			ਜੇ(inet_ntop(AF_INET, &((ਬਣਤਰ sockaddr_in *)&addr)->sin_addr.s_addr, buf, (socklen_t)len)){
				ਵਾਪਸ 0;
			}
		}ਨਹੀਂਤਾਂ ਜੇ(addr.ss_family == AF_INET6){
			ਜੇ(remote_ਪੋਰਟ){
				*remote_ਪੋਰਟ = ntohs(((ਬਣਤਰ sockaddr_in6 *)&addr)->sin6_port);
			}
			ਜੇ(inet_ntop(AF_INET6, &((ਬਣਤਰ sockaddr_in6 *)&addr)->sin6_addr.s6_addr, buf, (socklen_t)len)){
				ਵਾਪਸ 0;
			}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_UNIX_SOCKETS
		}ਨਹੀਂਤਾਂ ਜੇ(addr.ss_family == AF_UNIX){
			ਬਣਤਰ sockaddr_un un;
			addrlen = ਆਕਾਰ(ਬਣਤਰ sockaddr_un);
			ਜੇ(!getsockname(sock, (ਬਣਤਰ sockaddr *)&un, &addrlen)){
				snprintf(buf, len, "%s", un.sun_ਰਸਤਾ);
			}ਨਹੀਂਤਾਂ{
				snprintf(buf, len, "unix-socket");
			}
			ਵਾਪਸ 0;
#ਜੇ_ਅੰਤ
		}
	}
	ਵਾਪਸ 1;
}
