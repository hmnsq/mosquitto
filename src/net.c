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

#అన్తర్భావ "config.h"

#నిర్వచితం_కాకపోతే WIN32
#  include <arpa/inet.h>
#  ifndef _AIX
#    include <ifaddrs.h>
#  endif
#  include <netdb.h>
#  include <netinet/tcp.h>
#  include <strings.h>
#  include <sys/socket.h>
#  include <unistd.h>
#లేకపోతే
#  include <winsock2.h>
#  include <ws2tcpip.h>
#అయితే_ముగింపు

#అన్తర్భావ <errno.h>
#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>
#నిర్వచితమైతే WITH_WRAP
#  include <tcpd.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#  include <netinet/in.h>
#అయితే_ముగింపు

#ఒకవేళ defined(WITH_UNIX_SOCKETS) || defined(WITH_TLS)
#  include "sys/stat.h"
#అయితే_ముగింపు

#నిర్వచితమైతే WITH_UNIX_SOCKETS
#  ifdef WIN32
#    include <afunix.h>
#  లేకపోతే
#    include <sys/un.h>
#  endif
#అయితే_ముగింపు

#నిర్వచితమైతే __QNX__
#  include <net/netbyte.h>
#అయితే_ముగింపు

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto/mqtt_protocol.h"
#అన్తర్భావ "net_mosq.h"
#అన్తర్భావ "util_mosq.h"

#నిర్వచితమైతే WITH_TLS
#  include "tls_mosq.h"
#  include <openssl/err.h>
స్థిర పూర్ణాంకం tls_ex_సూచీ_context = -1;
స్థిర పూర్ణాంకం tls_ex_సూచీ_listener = -1;
#అయితే_ముగింపు

#అన్తర్భావ "sys_tree.h"

/* For EMFILE handling */
స్థిర mosq_sock_t అదనపు_sock = INVALID_SOCKET;


శూన్య net__broker_init(శూన్య)
{
	అదనపు_sock = socket(AF_INET, SOCK_STREAM, 0);
	net__init();
#నిర్వచితమైతే WITH_TLS
	net__init_tls();
#అయితే_ముగింపు
}


శూన్య net__broker_cleanup(శూన్య)
{
	ఒకవేళ(అదనపు_sock != INVALID_SOCKET){
		COMPAT_CLOSE(అదనపు_sock);
		అదనపు_sock = INVALID_SOCKET;
	}
	net__cleanup();
}


స్థిర శూన్య net__print_error(గుర్తులేని పూర్ణాంకం సంవర్గమానం, స్థిరరాశి అక్షరం *format_str)
{
	అక్షరం *buf;

#నిర్వచితమైతే WIN32
	Format_సందేశం(FORMAT_సందేశం_ALLOCATE_బఫరు | FORMAT_సందేశం_FROM_SYSTEM,
			ఖాళీ, WSAGet_చివరి_Error(), LANG_NEUTRAL, (LPTSTR)&buf, 0, ఖాళీ);

	log__printf(ఖాళీ, సంవర్గమానం, format_str, buf);
	LocalFree(buf);
#లేకపోతే
	buf = strerror(errno);
	log__printf(ఖాళీ, సంవర్గమానం, format_str, buf);
#అయితే_ముగింపు
}


నిర్మాణం mosquitto *net__socket_accept(నిర్మాణం mosquitto__listener_sock *listensock)
{
	mosq_sock_t new_sock = INVALID_SOCKET;
	నిర్మాణం mosquitto *new_context;
#నిర్వచితమైతే WITH_TLS
	BIO *bio;
#అయితే_ముగింపు
#నిర్వచితమైతే WITH_WRAP
	నిర్మాణం request_info wrap_req;
	అక్షరం చిరునామా[1024];
#అయితే_ముగింపు

	new_sock = accept(listensock->sock, ఖాళీ, 0);
	ఒకవేళ(new_sock == INVALID_SOCKET){
#నిర్వచితమైతే WIN32
		WINDOWS_సమితి_ERRNO();
		ఒకవేళ(errno == WSAEMFILE){
#లేకపోతే
		ఒకవేళ(errno == EMFILE || errno == ENFILE){
#అయితే_ముగింపు
			/* Close the spare socket, which means we should be able to accept
			 * this connection. Accept it, then close it immediately and create
			 * a new spare_sock. This prevents the situation of ever properly
			 * running out of sockets.
			 * It would be nice to send a "server not available" connack here,
			 * but there are lots of reasons why this would be tricky (TLS
			 * being the big one). */
			COMPAT_CLOSE(అదనపు_sock);
			new_sock = accept(listensock->sock, ఖాళీ, 0);
			ఒకవేళ(new_sock != INVALID_SOCKET){
				COMPAT_CLOSE(new_sock);
			}
			అదనపు_sock = socket(AF_INET, SOCK_STREAM, 0);
			log__printf(ఖాళీ, MOSQ_LOG_WARNING,
					"Unable to accept new connection, system socket count has been exceeded. Try increasing \"ulimit -n\" or equivalent.");
		}
		తిరిగి ఖాళీ;
	}

	metrics__int_inc(mosq_counter_socket_connections, 1);

	ఒకవేళ(net__socket_nonblock(&new_sock)){
		తిరిగి ఖాళీ;
	}

#నిర్వచితమైతే WITH_WRAP
	/* Use tcpd / libwrap to determine whether a connection is allowed. */
	request_init(&wrap_req, RQ_FILE, new_sock, RQ_DAEMON, "mosquitto", 0);
	fromhost(&wrap_req);
	ఒకవేళ(!hosts_access(&wrap_req)){
		/* Access is denied */
		ఒకవేళ(db.config->connection_messages == true){
			ఒకవేళ(!net_socket_get_చిరునామా(new_sock, చిరునామా, 1024, ఖాళీ)){
				log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client connection from %s denied access by tcpd.", చిరునామా);
			}
		}
		COMPAT_CLOSE(new_sock);
		తిరిగి ఖాళీ;
	}
#అయితే_ముగింపు

	ఒకవేళ(db.config->సమితి_tcp_nodelay && listensock->listener->పోర్టు){
		పూర్ణాంకం flag = 1;
		ఒకవేళ(setsockopt(new_sock, IPPROTO_TCP, TCP_NODELAY, (స్థిరరాశి శూన్య *)&flag, పరిమాణం(పూర్ణాంకం)) != 0){
			log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Unable to set TCP_NODELAY.");
		}
	}

	new_context = context__init();
	ఒకవేళ(!new_context){
		COMPAT_CLOSE(new_sock);
		తిరిగి ఖాళీ;
	}
	new_context->listener = listensock->listener;
	ఒకవేళ(!new_context->listener){
		context__cleanup(new_context, true);
		తిరిగి ఖాళీ;
	}
	new_context->listener->client_లెక్క++;

	ఒకవేళ(new_context->listener->enable_proxy_protocol){
		ఒకవేళ(context__init_sock(new_context, new_sock, false) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			తిరిగి ఖాళీ;
		}
		ఒకవేళ(new_context->listener->enable_proxy_protocol == 2){
			new_context->transport = mosq_t_proxy_v2;
		}లేకపోతే{
			new_context->transport = mosq_t_proxy_v1;
		}
		new_context->proxy.cmd = -1;
	}లేకపోతే{
		ఒకవేళ(context__init_sock(new_context, new_sock, true) != MOSQ_ERR_SUCCESS){
			context__cleanup(new_context, true);
			COMPAT_CLOSE(new_sock);
			తిరిగి ఖాళీ;
		}
		ఎంపిక(new_context->listener->protocol){
			స్థితి mp_mqtt:
				new_context->transport = mosq_t_tcp;
				ఆపు;
#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
			స్థితి mp_websockets:
				ఒకవేళ(http__context_init(new_context)){
					context__cleanup(new_context, true);
					తిరిగి ఖాళీ;
				}
				ఆపు;
#అయితే_ముగింపు
			అప్రమేయ:
				context__cleanup(new_context, true);
				తిరిగి ఖాళీ;
		}
	}

	ఒకవేళ((new_context->listener->max_connections > 0 && new_context->listener->client_లెక్క > new_context->listener->max_connections)
			|| (db.config->global_max_connections > 0 && HASH_CNT(hh_sock, db.contexts_by_sock) > (గుర్తులేని పూర్ణాంకం)db.config->global_max_connections)){

		ఒకవేళ(db.config->connection_messages == true){
			log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Client connection from %s denied: max_connections exceeded.", new_context->చిరునామా);
		}
		context__cleanup(new_context, true);
		తిరిగి ఖాళీ;
	}

#నిర్వచితమైతే WITH_TLS
	/* TLS init */
	ఒకవేళ(new_context->listener->ssl_ctx){
		new_context->ssl = SSL_new(new_context->listener->ssl_ctx);
		ఒకవేళ(!new_context->ssl){
			context__cleanup(new_context, true);
			తిరిగి ఖాళీ;
		}
		ఒకవేళ(!SSL_సమితి_ex_దత్తాంశం(new_context->ssl, tls_ex_సూచీ_context, new_context)
				|| !SSL_సమితి_ex_దత్తాంశం(new_context->ssl, tls_ex_సూచీ_listener, new_context->listener)){
			context__cleanup(new_context, true);
			తిరిగి ఖాళీ;
		}
		new_context->want_write = true;
		bio = BIO_new_socket(new_sock, BIO_NOCLOSE);
		SSL_సమితి_bio(new_context->ssl, bio, bio);
		ERR_clear_error();
		SSL_సమితి_accept_state(new_context->ssl);
	}
#అయితే_ముగింపు

	ఒకవేళ(db.config->connection_messages == true
			&& !new_context->listener->enable_proxy_protocol){

		log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "New connection from %s:%d on port %d.",
				new_context->చిరునామా, new_context->remote_పోర్టు, new_context->listener->పోర్టు);
	}

	mux__new(new_context);
	keepalive_కూడు(new_context);

	తిరిగి new_context;
}

#నిర్వచితమైతే WITH_TLS


స్థిర పూర్ణాంకం client_certificate_verify(పూర్ణాంకం preverify_ok, X509_STORE_CTX *ctx)
{
	/* Preverify should check expiry, revocation. */
	ఒకవేళ(preverify_ok == 0){
		SSL *ssl;
		ssl = X509_STORE_CTX_get_ex_దత్తాంశం(ctx, SSL_get_ex_దత్తాంశం_X509_STORE_CTX_idx());
		ఒకవేళ(ssl){
			నిర్మాణం mosquitto__listener *listener;
			listener = SSL_get_ex_దత్తాంశం(ssl, tls_ex_సూచీ_listener);

			ఒకవేళ(listener && listener->disable_client_cert_date_checks){
				పూర్ణాంకం err = X509_STORE_CTX_get_error(ctx);
				ఒకవేళ(err == X509_V_ERR_CERT_NOT_YET_VALID || err == X509_V_ERR_CERT_HAS_EXPIRED){
					preverify_ok = 1;
				}
			}
		}
	}
	తిరిగి preverify_ok;
}
#అయితే_ముగింపు

#నిర్వచితమైతే FINAL_WITH_TLS_PSK


స్థిర గుర్తులేని పూర్ణాంకం psk_server_callback(SSL *ssl, స్థిరరాశి అక్షరం *తటస్థం, గుర్తులేని అక్షరం *psk, గుర్తులేని పూర్ణాంకం max_psk_len)
{
	నిర్మాణం mosquitto *context;
	నిర్మాణం mosquitto__listener *listener;
	అక్షరం *psk_కీలకం = ఖాళీ;
	పూర్ణాంకం len;
	స్థిరరాశి అక్షరం *psk_hint;

	ఒకవేళ(!తటస్థం){
		తిరిగి 0;
	}

	context = SSL_get_ex_దత్తాంశం(ssl, tls_ex_సూచీ_context);
	ఒకవేళ(!context){
		తిరిగి 0;
	}

	listener = SSL_get_ex_దత్తాంశం(ssl, tls_ex_సూచీ_listener);
	ఒకవేళ(!listener){
		తిరిగి 0;
	}

	psk_hint = listener->psk_hint;

	/* The hex to BN conversion results in the length halving, so we can pass
	 * max_psk_len*2 as the max hex key here. */
	psk_కీలకం = mosquitto_calloc(1, (size_t)max_psk_len*2 + 1);
	ఒకవేళ(!psk_కీలకం){
		తిరిగి 0;
	}

	ఒకవేళ(mosquitto_psk_కీలకం_get(context, psk_hint, తటస్థం, psk_కీలకం, (పూర్ణాంకం)max_psk_len*2) != MOSQ_ERR_SUCCESS){
		mosquitto_FREE(psk_కీలకం);
		తిరిగి 0;
	}

	len = mosquitto__hex2bin(psk_కీలకం, psk, (పూర్ణాంకం)max_psk_len);
	ఒకవేళ(len < 0){
		mosquitto_FREE(psk_కీలకం);
		తిరిగి 0;
	}

	ఒకవేళ(listener->use_తటస్థం_as_username){
		ఒకవేళ(mosquitto_validate_utf8(తటస్థం, (పూర్ణాంకం)అక్షరశ్రేణి_పొడవు(తటస్థం))){
			mosquitto_free(psk_కీలకం);
			తిరిగి 0;
		}
		context->username = mosquitto_strdup(తటస్థం);
		ఒకవేళ(!context->username){
			mosquitto_FREE(psk_కీలకం);
			తిరిగి 0;
		}
	}

	mosquitto_FREE(psk_కీలకం);
	తిరిగి (గుర్తులేని పూర్ణాంకం)len;
}
#అయితే_ముగింపు

#నిర్వచితమైతే WITH_TLS


స్థిర శూన్య tls_keylog_callback(స్థిరరాశి SSL *ssl, స్థిరరాశి అక్షరం *పంక్తి)
{
	వాడని(ssl);

	ఒకవేళ(db.tls_keylog){
		FILE *fptr;
		fptr = mosquitto_fopen(db.tls_keylog, "at", true);
		ఒకవేళ(fptr){
#నిర్వచితం_కాకపోతే WIN32
			/* Until mosquitto_fopen enforces file permissions on all files,
			 * enforce it here. We can enforce it here, because it isn't a
			 * change of behaviour. */
			నిర్మాణం stat statbuf;
			ఒకవేళ(fstat(fileno(fptr), &statbuf) < 0
					|| statbuf.st_బహుళకం & (S_IRWXG | S_IRWXO)){

				fclose(fptr);
				తిరిగి;
			}
#అయితే_ముగింపు
			fprintf(fptr, "%s\n", పంక్తి);
			fclose(fptr);
		}లేకపోతే{
#నిర్వచితం_కాకపోతే WIN32
			ఒకవేళ(errno == ELOOP){
				log__printf(ఖాళీ, MOSQ_LOG_INFO, "Error: keylog file must not be a symbolic link");
			}లేకపోతే
#అయితే_ముగింపు
			{
				log__printf(ఖాళీ, MOSQ_LOG_INFO, "Error: Unable to open keylog file: %s", strerror(errno));
			}
		}
	}
}


పూర్ణాంకం net__tls_server_ctx(నిర్మాణం mosquitto__listener *listener)
{
	అక్షరం buf[256];
	పూర్ణాంకం rc;

	ఒకవేళ(tls_ex_సూచీ_context == -1){
		tls_ex_సూచీ_context = SSL_get_ex_new_సూచీ(0, "client context", ఖాళీ, ఖాళీ, ఖాళీ);
	}
	ఒకవేళ(tls_ex_సూచీ_listener == -1){
		tls_ex_సూచీ_listener = SSL_get_ex_new_సూచీ(0, "listener", ఖాళీ, ఖాళీ, ఖాళీ);
	}

	ఒకవేళ(listener->ssl_ctx){
		SSL_CTX_free(listener->ssl_ctx);
		listener->ssl_ctx = ఖాళీ;
	}

	listener->ssl_ctx = SSL_CTX_new(TLS_server_పద్ధతి());

	ఒకవేళ(!listener->ssl_ctx){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to create TLS context.");
		తిరిగి MOSQ_ERR_TLS;
	}

#నిర్వచితమైతే SSL_OP_NO_TLSv1_3
	ఒకవేళ(db.config->చొప్పున_listener_settings){
		ఒకవేళ(listener->security_options->psk_file){
			SSL_CTX_సమితి_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}లేకపోతే{
		ఒకవేళ(db.config->security_options.psk_file){
			SSL_CTX_సమితి_options(listener->ssl_ctx, SSL_OP_NO_TLSv1_3);
		}
	}
#అయితే_ముగింపు

	ఒకవేళ(listener->tls_సంస్కరణ == ఖాళీ){
		SSL_CTX_సమితి_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
#నిర్వచితమైతే SSL_OP_NO_TLSv1_3
	}లేకపోతే ఒకవేళ(!strcmp(listener->tls_సంస్కరణ, "tlsv1.3")){
		SSL_CTX_సమితి_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2);
#అయితే_ముగింపు
	}లేకపోతే ఒకవేళ(!strcmp(listener->tls_సంస్కరణ, "tlsv1.2")){
		SSL_CTX_సమితి_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
	}లేకపోతే ఒకవేళ(!strcmp(listener->tls_సంస్కరణ, "tlsv1.1")){
		SSL_CTX_సమితి_options(listener->ssl_ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1);
		log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: TLS v1.1 is insecure. It must only be used in the case "
				"where you have devices that cannot be upgraded to use a secure version of TLS. It is "
				"recommended to use as secure a set of ciphers as possible and that will restrict it to "
				"TLS v1.1 only, and to use a separate listener using TLS v1.2 and secure ciphers for your "
				"other devices.");
		log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Please be aware that support for TLS v1.1 will go away eventually "
				"and that you should plan now to migrate away from it.");
	}లేకపోతే{
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unsupported tls_version \"%s\".", listener->tls_సంస్కరణ);
		తిరిగి MOSQ_ERR_TLS;
	}

#నిర్వచితమైతే SSL_OP_NO_COMPRESSION
	/* Disable compression */
	SSL_CTX_సమితి_options(listener->ssl_ctx, SSL_OP_NO_COMPRESSION);
#అయితే_ముగింపు
#నిర్వచితమైతే SSL_OP_CIPHER_SERVER_PREFERENCE
	/* Server chooses cipher */
	SSL_CTX_సమితి_options(listener->ssl_ctx, SSL_OP_CIPHER_SERVER_PREFERENCE);
#అయితే_ముగింపు

#నిర్వచితమైతే SSL_బహుళకం_RELEASE_BUFFERS
	/* Use even less memory per SSL connection. */
	SSL_CTX_సమితి_బహుళకం(listener->ssl_ctx, SSL_బహుళకం_RELEASE_BUFFERS);
#అయితే_ముగింపు

	SSL_CTX_సమితి_dh_auto(listener->ssl_ctx, 1);

#నిర్వచితమైతే SSL_OP_NO_RENEGOTIATION
	SSL_CTX_సమితి_options(listener->ssl_ctx, SSL_OP_NO_RENEGOTIATION);
#అయితే_ముగింపు

	ఒకవేళ(db.tls_keylog){
		log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "TLS key logging to '%s' enabled for all listeners.",
				db.tls_keylog);
		log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "TLS key logging is for DEBUGGING only.");

		SSL_CTX_సమితి_keylog_callback(listener->ssl_ctx, tls_keylog_callback);
	}

	snprintf(buf, 256, "mosquitto-%d", listener->పోర్టు);
	SSL_CTX_సమితి_session_id_context(listener->ssl_ctx, (గుర్తులేని అక్షరం *)buf, (గుర్తులేని పూర్ణాంకం)అక్షరశ్రేణి_పొడవు(buf));

	ఒకవేళ(listener->ciphers){
		rc = SSL_CTX_సమితి_cipher_list(listener->ssl_ctx, listener->ciphers);
		ఒకవేళ(rc == 0){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			తిరిగి MOSQ_ERR_TLS;
		}
	}లేకపోతే{
		rc = SSL_CTX_సమితి_cipher_list(listener->ssl_ctx, "DEFAULT:!aNULL:!eNULL:!LOW:!EXPORT:!SSLv2:@STRENGTH");
		ఒకవేళ(rc == 0){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to set TLS ciphers. Check cipher list \"%s\".", listener->ciphers);
			తిరిగి MOSQ_ERR_TLS;
		}
	}
	ఒకవేళ(listener->ciphers_tls13){
		rc = SSL_CTX_సమితి_ciphersuites(listener->ssl_ctx, listener->ciphers_tls13);
		ఒకవేళ(rc == 0){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to set TLS 1.3 ciphersuites. Check cipher_tls13 list \"%s\".", listener->ciphers_tls13);
			తిరిగి MOSQ_ERR_TLS;
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}
#అయితే_ముగింపు


#నిర్వచితమైతే WITH_TLS


స్థిర పూర్ణాంకం net_ఎక్కించు_crl_file(నిర్మాణం mosquitto__listener *listener)
{
	X509_STORE *store;
	X509_LOOKUP *lookup;
	పూర్ణాంకం rc;

	store = SSL_CTX_get_cert_store(listener->ssl_ctx);
	ఒకవేళ(!store){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to obtain TLS store.");
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		తిరిగి MOSQ_ERR_TLS;
	}
	lookup = X509_STORE_కూడు_lookup(store, X509_LOOKUP_file());
	rc = X509_ఎక్కించు_crl_file(lookup, listener->crlfile, X509_FILETYPE_PEM);
	ఒకవేళ(rc < 1){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to load certificate revocation file \"%s\". Check crlfile.", listener->crlfile);
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		net__print_ssl_error(ఖాళీ, ఖాళీ);
		తిరిగి MOSQ_ERR_TLS;
	}
	X509_STORE_సమితి_flags(store, X509_V_FLAG_CRL_CHECK);

	తిరిగి MOSQ_ERR_SUCCESS;
}
#అయితే_ముగింపు


పూర్ణాంకం net_ఎక్కించు_certificates(నిర్మాణం mosquitto__listener *listener)
{
#నిర్వచితమైతే WITH_TLS
	పూర్ణాంకం rc;

	ఒకవేళ(listener->require_certificate){
		SSL_CTX_సమితి_verify(listener->ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, client_certificate_verify);
	}లేకపోతే{
		SSL_CTX_సమితి_verify(listener->ssl_ctx, SSL_VERIFY_NONE, client_certificate_verify);
	}
	rc = SSL_CTX_use_certificate_chain_file(listener->ssl_ctx, listener->certfile);
	ఒకవేళ(rc != 1){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
		net__print_ssl_error(ఖాళీ, ఖాళీ);
		తిరిగి MOSQ_ERR_TLS;
	}
	ఒకవేళ(listener->tls_engine == ఖాళీ || listener->tls_keyform == mosq_k_pem){
		rc = SSL_CTX_use_వ్యక్తిగత_కీలకం_file(listener->ssl_ctx, listener->keyfile, SSL_FILETYPE_PEM);
		ఒకవేళ(rc != 1){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			net__print_ssl_error(ఖాళీ, ఖాళీ);
			తిరిగి MOSQ_ERR_TLS;
		}
	}
	rc = SSL_CTX_check_వ్యక్తిగత_కీలకం(listener->ssl_ctx);
	ఒకవేళ(rc != 1){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Server certificate/key are inconsistent.");
		net__print_ssl_error(ఖాళీ, ఖాళీ);
		తిరిగి MOSQ_ERR_TLS;
	}
	/* Load CRLs if they exist. */
	ఒకవేళ(listener->crlfile){
		rc = net_ఎక్కించు_crl_file(listener);
		ఒకవేళ(rc){
			తిరిగి rc;
		}
	}
#లేకపోతే
	వాడని(listener);
#అయితే_ముగింపు
	తిరిగి MOSQ_ERR_SUCCESS;
}


#ఒకవేళ defined(WITH_TLS) && !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000


స్థిర పూర్ణాంకం net_ఎక్కించు_engine(నిర్మాణం mosquitto__listener *listener)
{
	ENGINE *engine = ఖాళీ;
	UI_పద్ధతి *ui_పద్ధతి;
	EVP_PKEY *pkey;

	ఒకవేళ(!listener->tls_engine){
		తిరిగి MOSQ_ERR_SUCCESS;
	}

	engine = ENGINE_by_id(listener->tls_engine);
	ఒకవేళ(!engine){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error loading %s engine\n", listener->tls_engine);
		net__print_ssl_error(ఖాళీ, ఖాళీ);
		తిరిగి MOSQ_ERR_TLS;
	}
	ఒకవేళ(!ENGINE_init(engine)){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Failed engine initialisation\n");
		net__print_ssl_error(ఖాళీ, ఖాళీ);
		తిరిగి MOSQ_ERR_TLS;
	}
	ENGINE_సమితి_default(engine, ENGINE_పద్ధతి_ALL);

	ఒకవేళ(listener->tls_keyform == mosq_k_engine){
		ui_పద్ధతి = net_get_ui_పద్ధతి();
		ఒకవేళ(listener->tls_engine_kpass_sha1){
			ఒకవేళ(!ENGINE_ctrl_cmd(engine, ENGINE_SECRET_బహుళకం, ENGINE_SECRET_బహుళకం_SHA, ఖాళీ, ఖాళీ, 0)){
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to set engine secret mode sha");
				net__print_ssl_error(ఖాళీ, ఖాళీ);
				తిరిగి MOSQ_ERR_TLS;
			}
			ఒకవేళ(!ENGINE_ctrl_cmd(engine, ENGINE_PIN, 0, listener->tls_engine_kpass_sha1, ఖాళీ, 0)){
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to set engine pin");
				net__print_ssl_error(ఖాళీ, ఖాళీ);
				తిరిగి MOSQ_ERR_TLS;
			}
			ui_పద్ధతి = ఖాళీ;
		}
		pkey = ENGINE_ఎక్కించు_వ్యక్తిగత_కీలకం(engine, listener->keyfile, ui_పద్ధతి, ఖాళీ);
		ఒకవేళ(!pkey){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to load engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(ఖాళీ, ఖాళీ);
			తిరిగి MOSQ_ERR_TLS;
		}
		ఒకవేళ(SSL_CTX_use_వ్యక్తిగత_కీలకం(listener->ssl_ctx, pkey) <= 0){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to use engine private key file \"%s\".", listener->keyfile);
			net__print_ssl_error(ఖాళీ, ఖాళీ);
			తిరిగి MOSQ_ERR_TLS;
		}
	}
	ENGINE_free(engine); /* release the structural reference from ENGINE_by_id() */

	తిరిగి MOSQ_ERR_SUCCESS;
}
#అయితే_ముగింపు


పూర్ణాంకం net_tls_ఎక్కించు_verify(నిర్మాణం mosquitto__listener *listener)
{
#నిర్వచితమైతే WITH_TLS
	పూర్ణాంకం rc;

#  ఒకవేళ OPENSSL_సంస్కరణ_సంఖ్య < 0x30000000L
	ఒకవేళ(listener->cafile || listener->capath){
		rc = SSL_CTX_ఎక్కించు_verify_locations(listener->ssl_ctx, listener->cafile, listener->capath);
		ఒకవేళ(rc == 0){
			ఒకవేళ(listener->cafile && listener->capath){
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\" and capath \"%s\".", listener->cafile, listener->capath);
			}లేకపోతే ఒకవేళ(listener->cafile){
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			}లేకపోతే{
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			}
		}
	}
#  లేకపోతే
	దొంతర_యొక్క(X509_NAME) *ca_పేర్లు = ఖాళీ;
	ఒకవేళ(listener->cafile || listener->capath){
		ca_పేర్లు = sk_X509_NAME_new_null();
	}

	ఒకవేళ(listener->cafile){
		rc = SSL_CTX_ఎక్కించు_verify_file(listener->ssl_ctx, listener->cafile);
		ఒకవేళ(rc == 0){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check cafile \"%s\".", listener->cafile);
			net__print_ssl_error(ఖాళీ, ఖాళీ);
			తిరిగి MOSQ_ERR_TLS;
		}

		దొంతర_యొక్క(X509_NAME) *cert_పేర్లు = SSL_ఎక్కించు_client_CA_file(listener->cafile);
		ఒకవేళ(cert_పేర్లు){
			కొరకు(పూర్ణాంకం i=0; i<sk_X509_NAME_అంకె(cert_పేర్లు); i++){
				sk_X509_NAME_నెట్టు(ca_పేర్లు, X509_NAME_dup(sk_X509_NAME_విలువ(cert_పేర్లు, i)));
			}
			sk_X509_NAME_తీయి_free(cert_పేర్లు, X509_NAME_free);
		}

	}
	ఒకవేళ(listener->capath){
		rc = SSL_CTX_ఎక్కించు_verify_dir(listener->ssl_ctx, listener->capath);
		ఒకవేళ(rc == 0){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to load CA certificates. Check capath \"%s\".", listener->capath);
			net__print_ssl_error(ఖాళీ, ఖాళీ);
			sk_X509_NAME_తీయి_free(ca_పేర్లు, X509_NAME_free);
			తిరిగి MOSQ_ERR_TLS;
		}

		SSL_కూడు_dir_cert_subjects_గమ్యం_దొంతర(ca_పేర్లు, listener->capath);
	}
	ఒకవేళ(ca_పేర్లు){
		SSL_CTX_సమితి_client_CA_list(listener->ssl_ctx, ca_పేర్లు);
	}
#  endif

#  ఒకవేళ !defined(OPENSSL_NO_ENGINE) && OPENSSL_API_LEVEL < 30000
	ఒకవేళ(net_ఎక్కించు_engine(listener)){
		తిరిగి MOSQ_ERR_TLS;
	}
#  endif
#అయితే_ముగింపు
	తిరిగి net_ఎక్కించు_certificates(listener);
}


#ఒకవేళ !defined(WIN32) && !defined(_AIX)


స్థిర పూర్ణాంకం net__bind_interface(నిర్మాణం mosquitto__listener *listener, నిర్మాణం addrinfo *rp)
{
	/*
	 * This binds the listener sock to a network interface.
	 * The use of SO_BINDTODEVICE requires root access, which we don't have, so instead
	 * use getifaddrs to find the interface addresses, and use IP of the
	 * matching interface in the later bind().
	 */
	నిర్మాణం ifaddrs *ifaddr;
	bool have_interface = false;

	ఒకవేళ(getifaddrs(&ifaddr) < 0){
		net__print_error(MOSQ_LOG_ERR, "Error: %s");
		తిరిగి MOSQ_ERR_ERRNO;
	}

	కొరకు(నిర్మాణం ifaddrs *ifa=ifaddr; ifa!=ఖాళీ; ifa=ifa->ifa_తదుపరి){
		ఒకవేళ(ifa->ifa_addr == ఖాళీ){
			కొనసాగు;
		}

		ఒకవేళ(!strcasecmp(listener->bind_interface, ifa->ifa_name)){
			have_interface = true;

			ఒకవేళ(ifa->ifa_addr->sa_family == rp->ai_addr->sa_family){
				ఒకవేళ(rp->ai_addr->sa_family == AF_INET){
					ఒకవేళ(listener->host &&
							memcmp(&((నిర్మాణం sockaddr_in *)rp->ai_addr)->sin_addr,
							&((నిర్మాణం sockaddr_in *)ifa->ifa_addr)->sin_addr,
							పరిమాణం(నిర్మాణం in_addr))){

						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						తిరిగి MOSQ_ERR_INVAL;
					}లేకపోతే{
						జ్ఞాపకం_నకలు(&((నిర్మాణం sockaddr_in *)rp->ai_addr)->sin_addr,
								&((నిర్మాణం sockaddr_in *)ifa->ifa_addr)->sin_addr,
								పరిమాణం(నిర్మాణం in_addr));

						freeifaddrs(ifaddr);
						తిరిగి MOSQ_ERR_SUCCESS;
					}
				}లేకపోతే ఒకవేళ(rp->ai_addr->sa_family == AF_INET6){
					ఒకవేళ(listener->host &&
							memcmp(&((నిర్మాణం sockaddr_in6 *)rp->ai_addr)->sin6_addr,
							&((నిర్మాణం sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
							పరిమాణం(నిర్మాణం in6_addr))){

						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Interface address for %s does not match specified listener address (%s).",
								listener->bind_interface, listener->host);
						తిరిగి MOSQ_ERR_INVAL;
					}లేకపోతే{
						జ్ఞాపకం_నకలు(&((నిర్మాణం sockaddr_in6 *)rp->ai_addr)->sin6_addr,
								&((నిర్మాణం sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
								పరిమాణం(నిర్మాణం in6_addr));

						((నిర్మాణం sockaddr_in6 *)rp->ai_addr)->sin6_scope_id = ((నిర్మాణం sockaddr_in6 *)ifa->ifa_addr)->sin6_scope_id;

						freeifaddrs(ifaddr);
						తిరిగి MOSQ_ERR_SUCCESS;
					}
				}
			}
		}
	}
	freeifaddrs(ifaddr);
	ఒకవేళ(have_interface){
		log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Interface %s does not support %s configuration.",
				listener->bind_interface, rp->ai_addr->sa_family == AF_INET ? "IPv4" : "IPv6");
		తిరిగి MOSQ_ERR_NOT_SUPPORTED;
	}లేకపోతే{
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Interface %s does not exist.", listener->bind_interface);
		తిరిగి MOSQ_ERR_NOT_దొరికింది;
	}
}
#అయితే_ముగింపు


స్థిర పూర్ణాంకం net__socket_listen_tcp(నిర్మాణం mosquitto__listener *listener)
{
	mosq_sock_t sock = INVALID_SOCKET;
	నిర్మాణం addrinfo hints;
	నిర్మాణం addrinfo *ainfo, *rp;
	అక్షరం service[10];
	పూర్ణాంకం rc;
	పూర్ణాంకం ss_opt = 1;
#నిర్వచితం_కాకపోతే WIN32
	bool interface_bound = false;
#అయితే_ముగింపు

	ఒకవేళ(!listener){
		తిరిగి MOSQ_ERR_INVAL;
	}

	snprintf(service, 10, "%d", listener->పోర్టు);
	memset(&hints, 0, పరిమాణం(నిర్మాణం addrinfo));
	ఒకవేళ(listener->socket_domain){
		hints.ai_family = listener->socket_domain;
	}లేకపోతే{
		hints.ai_family = AF_UNSPEC;
	}
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	rc = getaddrinfo(listener->host, service, &hints, &ainfo);
	ఒకవేళ(rc){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error creating listener: %s.", gai_strerror(rc));
		తిరిగి INVALID_SOCKET;
	}

	listener->sock_లెక్క = 0;
	listener->socks = ఖాళీ;

	కొరకు(rp = ainfo; rp; rp = rp->ai_తదుపరి){
		ఒకవేళ(rp->ai_family == AF_INET){
			log__printf(ఖాళీ, MOSQ_LOG_INFO, "Opening ipv4 listen socket on port %d.", ntohs(((నిర్మాణం sockaddr_in *)rp->ai_addr)->sin_port));
		}లేకపోతే ఒకవేళ(rp->ai_family == AF_INET6){
			log__printf(ఖాళీ, MOSQ_LOG_INFO, "Opening ipv6 listen socket on port %d.", ntohs(((నిర్మాణం sockaddr_in6 *)rp->ai_addr)->sin6_port));
		}లేకపోతే{
			కొనసాగు;
		}

		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		ఒకవేళ(sock == INVALID_SOCKET){
			net__print_error(MOSQ_LOG_WARNING, "Warning: %s");
			కొనసాగు;
		}
		listener->sock_లెక్క++;
		listener->socks = mosquitto_realloc(listener->socks, పరిమాణం(mosq_sock_t)*(size_t)listener->sock_లెక్క);
		ఒకవేళ(!listener->socks){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			తిరిగి MOSQ_ERR_NOMEM;
		}
		listener->socks[listener->sock_లెక్క-1] = sock;

#నిర్వచితం_కాకపోతే WIN32
		ss_opt = 1;
		/* Unimportant if this fails */
		(శూన్య)setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (స్థిరరాశి శూన్య *)&ss_opt, పరిమాణం(ss_opt));
#అయితే_ముగింపు
#నిర్వచితమైతే IPV6_V6ONLY
		ss_opt = 1;
		(శూన్య)setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (స్థిరరాశి శూన్య *)&ss_opt, పరిమాణం(ss_opt));
#అయితే_ముగింపు

		ఒకవేళ(net__socket_nonblock(&sock)){
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			తిరిగి 1;
		}

#ఒకవేళ !defined(WIN32) && !defined(_AIX)
		ఒకవేళ(listener->bind_interface){
			/* It might be possible that an interface does not support all relevant sa_families.
			 * We should successfully find at least one. */
			rc = net__bind_interface(listener, rp);
			ఒకవేళ(rc){
				COMPAT_CLOSE(sock);
				listener->sock_లెక్క--;
				ఒకవేళ(rc == MOSQ_ERR_NOT_దొరికింది || rc == MOSQ_ERR_INVAL){
					freeaddrinfo(ainfo);
					తిరిగి rc;
				}లేకపోతే{
					కొనసాగు;
				}
			}
			interface_bound = true;
		}
#అయితే_ముగింపు

		ఒకవేళ(bind(sock, rp->ai_addr, rp->ai_addrlen) == -1){
#ఒకవేళ defined(__linux__)
			ఒకవేళ(errno == EACCES){
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "If you are trying to bind to a privileged port (<1024), try using setcap and do not start the broker as root:");
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "    sudo setcap 'CAP_NET_BIND_SERVICE=+ep /usr/sbin/mosquitto'");
			}
#అయితే_ముగింపు
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			COMPAT_CLOSE(sock);
			freeaddrinfo(ainfo);
			mosquitto_FREE(listener->socks);
			తిరిగి 1;
		}

		ఒకవేళ(listen(sock, 100) == -1){
			net__print_error(MOSQ_LOG_ERR, "Error: %s");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			mosquitto_FREE(listener->socks);
			తిరిగి 1;
		}
	}
	freeaddrinfo(ainfo);

#నిర్వచితం_కాకపోతే WIN32
	ఒకవేళ(listener->bind_interface && !interface_bound){
		mosquitto_FREE(listener->socks);
		తిరిగి 1;
	}
#అయితే_ముగింపు

	తిరిగి 0;
}


#నిర్వచితమైతే WITH_UNIX_SOCKETS


స్థిర పూర్ణాంకం net__socket_listen_unix(నిర్మాణం mosquitto__listener *listener)
{
	నిర్మాణం sockaddr_un addr;
	mosq_sock_t sock = INVALID_SOCKET;
	పూర్ణాంకం rc;
#నిర్వచితం_కాకపోతే WIN32
	mode_t old_ముసుగు;
#అయితే_ముగింపు

	ఒకవేళ(listener->unix_socket_మార్గం == ఖాళీ){
		తిరిగి MOSQ_ERR_INVAL;
	}
	ఒకవేళ(అక్షరశ్రేణి_పొడవు(listener->unix_socket_మార్గం) > పరిమాణం(addr.sun_మార్గం)-1){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Path to unix socket is too long \"%s\".", listener->unix_socket_మార్గం);
		తిరిగి MOSQ_ERR_INVAL;
	}

#నిర్వచితమైతే WIN32
	DeleteFile(listener->unix_socket_మార్గం);
#లేకపోతే
	unlink(listener->unix_socket_మార్గం);
#అయితే_ముగింపు
	log__printf(ఖాళీ, MOSQ_LOG_INFO, "Opening unix listen socket on path %s.", listener->unix_socket_మార్గం);
	memset(&addr, 0, పరిమాణం(నిర్మాణం sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_మార్గం, listener->unix_socket_మార్గం, పరిమాణం(addr.sun_మార్గం)-1);

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	ఒకవేళ(sock == INVALID_SOCKET){
		net__print_error(MOSQ_LOG_ERR, "Error creating unix socket: %s");
		తిరిగి 1;
	}
	listener->sock_లెక్క++;
	listener->socks = mosquitto_realloc(listener->socks, పరిమాణం(mosq_sock_t)*(size_t)listener->sock_లెక్క);
	ఒకవేళ(!listener->socks){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
		COMPAT_CLOSE(sock);
		తిరిగి MOSQ_ERR_NOMEM;
	}
	listener->socks[listener->sock_లెక్క-1] = sock;


#నిర్వచితం_కాకపోతే WIN32
	old_ముసుగు = umask(0007);
#అయితే_ముగింపు
	rc = bind(sock, (నిర్మాణం sockaddr *)&addr, పరిమాణం(నిర్మాణం sockaddr_un));
#నిర్వచితం_కాకపోతే WIN32
	umask(old_ముసుగు);
#అయితే_ముగింపు

	ఒకవేళ(rc == -1){
		net__print_error(MOSQ_LOG_ERR, "Error binding unix socket: %s");
		తిరిగి 1;
	}

	ఒకవేళ(listen(sock, 10) == -1){
		net__print_error(MOSQ_LOG_ERR, "Error listening to unix socket: %s");
		తిరిగి 1;
	}

	ఒకవేళ(net__socket_nonblock(&sock)){
		తిరిగి 1;
	}

	తిరిగి 0;
}
#అయితే_ముగింపు


/* Creates a socket and listens on port 'port'.
 * Returns 1 on failure
 * Returns 0 on success.
 */
పూర్ణాంకం net__socket_listen(నిర్మాణం mosquitto__listener *listener)
{
	పూర్ణాంకం rc;

	ఒకవేళ(!listener){
		తిరిగి MOSQ_ERR_INVAL;
	}

#నిర్వచితమైతే WITH_UNIX_SOCKETS
	ఒకవేళ(listener->పోర్టు == 0 && listener->unix_socket_మార్గం != ఖాళీ){
		rc = net__socket_listen_unix(listener);
	}లేకపోతే
#అయితే_ముగింపు
	{
		rc = net__socket_listen_tcp(listener);
	}
	ఒకవేళ(rc){
		తిరిగి rc;
	}

	/* We need to have at least one working socket. */
	ఒకవేళ(listener->sock_లెక్క > 0){
#నిర్వచితమైతే WITH_TLS
		ఒకవేళ(listener->certfile && listener->keyfile){
			ఒకవేళ(net__tls_server_ctx(listener)){
				తిరిగి 1;
			}

			ఒకవేళ(net_tls_ఎక్కించు_verify(listener)){
				తిరిగి 1;
			}
		}
#  ifdef FINAL_WITH_TLS_PSK
		ఒకవేళ(listener->psk_hint){
			ఒకవేళ(listener->certfile == ఖాళీ || listener->keyfile == ఖాళీ){
				ఒకవేళ(net__tls_server_ctx(listener)){
					తిరిగి 1;
				}
			}
			SSL_CTX_సమితి_psk_server_callback(listener->ssl_ctx, psk_server_callback);
			ఒకవేళ(listener->psk_hint){
				rc = SSL_CTX_use_psk_తటస్థం_hint(listener->ssl_ctx, listener->psk_hint);
				ఒకవేళ(rc == 0){
					log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to set TLS PSK hint.");
					net__print_ssl_error(ఖాళీ, ఖాళీ);
					తిరిగి 1;
				}
			}
		}
#  endif /* FINAL_WITH_TLS_PSK */
		ఒకవేళ(tls_ex_సూచీ_context == -1){
			tls_ex_సూచీ_context = SSL_get_ex_new_సూచీ(0, "client context", ఖాళీ, ఖాళీ, ఖాళీ);
		}
		ఒకవేళ(tls_ex_సూచీ_listener == -1){
			tls_ex_సూచీ_listener = SSL_get_ex_new_సూచీ(0, "listener", ఖాళీ, ఖాళీ, ఖాళీ);
		}
#అయితే_ముగింపు /* WITH_TLS */
		తిరిగి 0;
	}లేకపోతే{
		తిరిగి 1;
	}
}


పూర్ణాంకం net_socket_get_చిరునామా(mosq_sock_t sock, అక్షరం *buf, size_t len, uint16_t *remote_పోర్టు)
{
	నిర్మాణం sockaddr_storage addr;
	socklen_t addrlen;

	memset(&addr, 0, పరిమాణం(నిర్మాణం sockaddr_storage));
	addrlen = పరిమాణం(addr);
	ఒకవేళ(!getpeername(sock, (నిర్మాణం sockaddr *)&addr, &addrlen)){
		ఒకవేళ(addr.ss_family == AF_INET){
			ఒకవేళ(remote_పోర్టు){
				*remote_పోర్టు = ntohs(((నిర్మాణం sockaddr_in *)&addr)->sin_port);
			}
			ఒకవేళ(inet_ntop(AF_INET, &((నిర్మాణం sockaddr_in *)&addr)->sin_addr.s_addr, buf, (socklen_t)len)){
				తిరిగి 0;
			}
		}లేకపోతే ఒకవేళ(addr.ss_family == AF_INET6){
			ఒకవేళ(remote_పోర్టు){
				*remote_పోర్టు = ntohs(((నిర్మాణం sockaddr_in6 *)&addr)->sin6_port);
			}
			ఒకవేళ(inet_ntop(AF_INET6, &((నిర్మాణం sockaddr_in6 *)&addr)->sin6_addr.s6_addr, buf, (socklen_t)len)){
				తిరిగి 0;
			}
#నిర్వచితమైతే WITH_UNIX_SOCKETS
		}లేకపోతే ఒకవేళ(addr.ss_family == AF_UNIX){
			నిర్మాణం sockaddr_un un;
			addrlen = పరిమాణం(నిర్మాణం sockaddr_un);
			ఒకవేళ(!getsockname(sock, (నిర్మాణం sockaddr *)&un, &addrlen)){
				snprintf(buf, len, "%s", un.sun_మార్గం);
			}లేకపోతే{
				snprintf(buf, len, "unix-socket");
			}
			తిరిగి 0;
#అయితే_ముగింపు
		}
	}
	తిరిగి 1;
}
