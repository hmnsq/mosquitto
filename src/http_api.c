/*
Copyright (c) 2025 Roger Light <roger@atchoo.org>

All rights reserved. This program and the accompanying materials
are made available under the terms of the Eclipse Public License 2.0
and Eclipse Distribution License v1.0 which accompany this distribution.

The Eclipse Public License is available at
	 https://www.eclipse.org/legal/epl-2.0/
and the Eclipse Distribution License is available at
	http://www.eclipse.org/org/documents/edl-v10.php.

SPDX-License-Identifier: EPL-2.0 OR EDL-1.0

Contributors:
	 Roger Light - initial implementation and documentation.
*/

#ضمن "config.h"

#جيڪڏهن_وصف_ٿيل WITH_HTTP_API

#ضمن <assert.h>
#ضمن <errno.h>
#ضمن <limits.h>
#ضمن <microhttpd.h>
#ضمن <string.h>
#ضمن <sys/stat.h>
#جيڪڏهن_وصف_نه_ٿيل WIN32
#  include <netdb.h>
#جيڪڏهن_پورو

#ضمن "json_help.h"
#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "sys_tree.h"

#جيڪڏهن_وصف_نه_ٿيل HTTP_API_DIR
#  define HTTP_API_DIR ""
#جيڪڏهن_پورو

بنية metric {
	int64_t current;
	int64_t ايندڙ;
	ثابت حرف *topic, *topic_alias;
	bool is_max;
};

time_t broker_uptime(فراغ);
بنية MHD_Daemon *mhd = عدم;

#جيڪڏهن_وصف_ٿيل WITH_SYS_وڻ
خارجي بنية metric metrics[mosq_metric_max];
#جيڪڏهن_پورو

#وصف_ڪريو HTTP_RESPONSE_BUFLEN 10000

#جيڪڏهن_وصف_ٿيل WIN32
#وصف_ڪريو DIR_SEP '\\'
#وصف_ڪريو PATH_MAX MAX_رستو
#وإلا
#وصف_ڪريو DIR_SEP '/'
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل S_ISREG
#وصف_ڪريو S_ISREG(گهڻائي) (((گهڻائي)&S_IFMT) == S_IFREG)
#جيڪڏهن_پورو


ساكن حرف *http__canonical_filename(
		ثابت حرف *url,
		ثابت حرف *http_dir,
		صحيح *error_ڪوڊ)
{
	size_t urllen, slen;
	حرف *filename, *filename_canonical;

	urllen = لڙي_ڊيگهه(url);
	إذا(url[urllen-1] == '/'){
		slen = لڙي_ڊيگهه(http_dir) + urllen + لڙي_ڊيگهه("/index.html") + 2;
	}وإلا{
		slen = لڙي_ڊيگهه(http_dir) + urllen + 2;
	}
	filename = mosquitto_malloc(slen);
	إذا(!filename){
		*error_ڪوڊ = MHD_HTTP_اندروني_SERVER_ERROR;
		أرجع عدم;
	}
	إذا(((حرف *)url)[urllen-1] == '/'){
		snprintf(filename, slen, "%s%c%sindex.html", http_dir, DIR_SEP, url);
	}وإلا{
		snprintf(filename, slen, "%s%c%s", http_dir, DIR_SEP, url);
	}


	/* Get canonical path and check it is within our http_dir */
	filename_canonical = mosquitto_calloc(1, PATH_MAX);
	إذا(!filename_canonical){
		*error_ڪوڊ = MHD_HTTP_اندروني_SERVER_ERROR;
		أرجع عدم;
	}
#جيڪڏهن_وصف_ٿيل WIN32
	حرف *resolved = _fullpath(filename_canonical, filename, PATH_MAX);
	mosquitto_FREE(filename);
	إذا(!resolved){
		*error_ڪوڊ = MHD_HTTP_اندروني_SERVER_ERROR;
		mosquitto_FREE(filename_canonical);
		أرجع عدم;
	}
#وإلا
	حرف *resolved = realpath(filename, filename_canonical);
	mosquitto_FREE(filename);
	إذا(!resolved){
		إذا(errno == EACCES){
			*error_ڪوڊ = MHD_HTTP_FORBIDDEN;
		}وإلا إذا(errno == EINVAL || errno == EIO || errno == ELOOP){
			*error_ڪوڊ = MHD_HTTP_اندروني_SERVER_ERROR;
		}وإلا إذا(errno == ENAMETOOLONG){
			*error_ڪوڊ = MHD_HTTP_URI_TOO_LONG;
		}وإلا إذا(errno == ENOENT || errno == ENOTDIR){
			*error_ڪوڊ = MHD_HTTP_NOT_مليو;
		}
		mosquitto_FREE(filename_canonical);
		أرجع عدم;
	}
#جيڪڏهن_پورو
	إذا(لڙي_ڀيٽ(http_dir, filename_canonical, لڙي_ڊيگهه(http_dir))){
		/* Requested file isn't within http_dir, deny access because it's not found. */
		mosquitto_FREE(filename_canonical);
		*error_ڪوڊ = MHD_HTTP_NOT_مليو;
		أرجع عدم;
	}

	أرجع filename_canonical;
}

ساكن تعداد MHD_نتيجو http_api__send_error_response(بنية MHD_Connection *connection, ثابت حرف *error_str, بنانشان صحيح error_ڪوڊ)
{
	بنية MHD_Response *response = MHD_create_response_from_بفر(لڙي_ڊيگهه(error_str), (فراغ *)error_str, MHD_RESPMEM_MUST_COPY);
	تعداد MHD_نتيجو ret = MHD_قطار_response(connection, error_ڪوڊ, response);
	MHD_destroy_response(response);
	أرجع ret;
}

ساكن تعداد MHD_نتيجو http_api_send_response_with_سرا(بنية MHD_Connection *connection, ثابت حرف *buf)
{
	تعداد MHD_نتيجو ret;
	بنية MHD_Response *response = MHD_create_response_from_بفر(لڙي_ڊيگهه(buf), (فراغ *)buf, MHD_RESPMEM_MUST_COPY);

	ret = MHD_جوڙيو_response_سرو(response, "Access-Control-Allow-Origin", "*");
	إذا(ret != MHD_YES){
		MHD_destroy_response(response);
		أرجع ret;
	}
	ret = MHD_قطار_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	أرجع ret;
}

ساكن تعداد MHD_نتيجو http_api_process_نسخو(بنية MHD_Connection *connection)
{
	أرجع http_api_send_response_with_سرا(connection, نسخو);
}

ساكن تعداد MHD_نتيجو http_api__process_listeners(بنية MHD_Connection *connection)
{
	حرف *buf;
	تعداد MHD_نتيجو ret;

	cJSON *j_وڻ = cJSON_CreateObject();
	إذا(!j_وڻ){
		أرجع http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_اندروني_SERVER_ERROR);
	}

	cJSON *j_listeners = c_JSON_جوڙيو_Array_منزل_Object(j_وڻ, "listeners");
	إذا(!j_listeners){
		cJSON_Delete(j_وڻ);
		أرجع http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_اندروني_SERVER_ERROR);
	}

	لكل(صحيح i=0; i<db.config->listener_ڳڻپ; i++){
		cJSON *j_listener = cJSON_CreateObject();
		إذا(!j_listener){
			cJSON_Delete(j_وڻ);
			أرجع http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_اندروني_SERVER_ERROR);
		}
		c_JSON_جوڙيو_شي_منزل_Array(j_listeners, j_listener);

		بنية mosquitto__listener *listener = &db.config->listeners[i];
#جيڪڏهن_وصف_ٿيل WITH_UNIX_SOCKETS
		إذا(listener->unix_socket_رستو){
			c_JSON_جوڙيو_String_منزل_Object(j_listener, "path", listener->unix_socket_رستو);
		}وإلا
#جيڪڏهن_پورو
		{
			c_JSON_جوڙيو_Int_منزل_Object(j_listener, "port", listener->پورٽ);
		}

		بدل(listener->protocol){
			حالة mp_mqtt:
				c_JSON_جوڙيو_String_منزل_Object(j_listener, "protocol", "mqtt");
				اكسر;
			حالة mp_mqttsn:
				c_JSON_جوڙيو_String_منزل_Object(j_listener, "protocol", "mqtt-sn");
				اكسر;
			حالة mp_websockets:
				c_JSON_جوڙيو_String_منزل_Object(j_listener, "protocol", "websockets");
				اكسر;
			حالة mp_http_api:
				c_JSON_جوڙيو_String_منزل_Object(j_listener, "protocol", "httpapi");
				اكسر;
		}

#جيڪڏهن_وصف_ٿيل WITH_TLS
		c_JSON_جوڙيو_Bool_منزل_Object(j_listener, "tls", listener->certfile && listener->keyfile);
		c_JSON_جوڙيو_Bool_منزل_Object(j_listener, "mtls", listener->require_certificate);
#وإلا
		c_JSON_جوڙيو_Bool_منزل_Object(j_listener, "tls", false);
		c_JSON_جوڙيو_Bool_منزل_Object(j_listener, "mtls", false);
#جيڪڏهن_پورو
		إذا(listener->security_options->allow_anonymous == -1){
			c_JSON_جوڙيو_Bool_منزل_Object(j_listener, "allow_anonymous", db.config->security_options.allow_anonymous);
		}وإلا{
			c_JSON_جوڙيو_Bool_منزل_Object(j_listener, "allow_anonymous", listener->security_options->allow_anonymous);
		}
	}
	buf = cJSON_Print(j_وڻ);
	cJSON_Delete(j_وڻ);
	إذا(buf){
		ret = http_api_send_response_with_سرا(connection, buf);
		حرر(buf);
	}وإلا{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_اندروني_SERVER_ERROR);
	}

	أرجع ret;
}


ساكن تعداد MHD_نتيجو http_api__process_systree(بنية MHD_Connection *connection)
{
	تعداد MHD_نتيجو ret;
#جيڪڏهن_وصف_ٿيل WITH_SYS_وڻ
	حرف *buf;

	cJSON *j_وڻ = cJSON_CreateObject();
	لكل(صحيح i=0; i<mosq_metric_max; i++){
		إذا(metrics[i].topic){
			c_JSON_جوڙيو_Int_منزل_Object(j_وڻ, metrics[i].topic, metrics[i].current);
		}
	}
	c_JSON_جوڙيو_Int_منزل_Object(j_وڻ, "$SYS/broker/uptime", broker_uptime());

	buf = cJSON_Print(j_وڻ);
	cJSON_Delete(j_وڻ);

	إذا(buf){
		ret = http_api_send_response_with_سرا(connection, buf);
		حرر(buf);
	}وإلا{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_اندروني_SERVER_ERROR);
	}
#وإلا
	ret = http_api__send_error_response(connection, "Not found.\n", 404);
#جيڪڏهن_پورو

	أرجع ret;
}


ساكن ssize_t http_file_read_cb(فراغ *cls, uint64_t pos, حرف *buf, size_t max)
{
	FILE *fptr = cls;
	إذا(fseek(fptr, (ڊگهو )pos, SEEK_SET) < 0){
		أرجع -1;
	}
	أرجع (ssize_t )fread(buf, 1, max, fptr);
}


ساكن فراغ http_file_free_cb(فراغ *cls)
{
	FILE *fptr = cls;
	fclose(fptr);
}


ساكن تعداد MHD_نتيجو http_api__process_file(بنية mosquitto__listener *listener, بنية MHD_Connection *connection, ثابت حرف *url)
{
	صحيح error_ڪوڊ = MHD_HTTP_اندروني_SERVER_ERROR;
	حرف *canonical_filename;

	إذا(!listener || !listener->http_dir){
		http_api__send_error_response(connection, "Not found.\n", 404);
		أرجع MHD_YES;
	}

	canonical_filename = http__canonical_filename(url, listener->http_dir, &error_ڪوڊ);
	إذا(!canonical_filename){
		http_api__send_error_response(connection, "Not found.\n", 404);
		أرجع MHD_YES;
	}

	FILE *fptr = fopen(canonical_filename, "rb");
	mosquitto_FREE(canonical_filename);
	إذا(!fptr){
		http_api__send_error_response(connection, "Not found.\n", 404);
		أرجع MHD_YES;
	}

	بنية stat statbuf;
	إذا(fstat(fileno(fptr), &statbuf)){
		fclose(fptr);
		http_api__send_error_response(connection, "Internal server error.\n", 500);
		أرجع MHD_YES;
	}

	إذا(!S_ISREG(statbuf.st_گهڻائي)){
		fclose(fptr);
		http_api__send_error_response(connection, "Not found.\n", 404);
		أرجع MHD_YES;
	}
	uint64_t flen = (uint64_t )statbuf.st_سائيز;

	/* Using MHD_create_response_from_fd would be easier here, but is less portable */
	بنية MHD_Response *response = MHD_create_response_from_callback(
			flen,
			65536,
			http_file_read_cb,
			fptr,
			http_file_free_cb);

	تعداد MHD_نتيجو ret = MHD_قطار_response(connection, 200, response);
	MHD_destroy_response(response);
	أرجع ret;
}


ساكن تعداد MHD_نتيجو http_api__process_api(بنية MHD_Connection *connection, ثابت حرف *url)
{
	إذا(strcmp(url, "/api/v1/systree") == 0){
		أرجع http_api__process_systree(connection);
	}وإلا إذا(strcmp(url, "/api/v1/listeners") == 0){
		أرجع http_api__process_listeners(connection);
	}وإلا إذا(strcmp(url, "/api/v1/version") == 0){
		أرجع http_api_process_نسخو(connection);
	}وإلا{
		أرجع http_api__send_error_response(connection, "Not found.\n", 404);
	}
}


ساكن صحيح check_access(بنية mosquitto__listener *listener, بنية MHD_Connection *connection, ثابت حرف *url)
{
	بنية mosquitto context = {0};
	صحيح auth_rc, acl_rc = MOSQ_ERR_SUCCESS;

	context.listener = listener;

	context.id = (حرف *)"http-api";
	context.username = MHD_basic_auth_get_username_password(connection, &context.password);

	/* Authentication */
	auth_rc = mosquitto_basic_auth(&context);
	إذا(auth_rc == MOSQ_ERR_SUCCESS){
		acl_rc = mosquitto_acl_check(&context, url, 0, عدم, 0, false, عدم, MOSQ_ACL_READ);
	}
	MHD_free(context.username);
	MHD_free(context.password);

	إذا(auth_rc || acl_rc){
		ثابت حرف *buf = "Not authorised\n";
		بنية MHD_Response *response = MHD_create_response_from_بفر(لڙي_ڊيگهه(buf), (فراغ *)buf, MHD_RESPMEM_MUST_COPY);
		MHD_قطار_basic_auth_fail_response(connection, "Mosquitto API", response);
		MHD_destroy_response(response);

		أرجع MOSQ_ERR_AUTH;
	}

	أرجع MOSQ_ERR_SUCCESS;
}

ساكن تعداد MHD_نتيجو http_api_handler(فراغ *cls, بنية MHD_Connection
		*connection, ثابت حرف *url, ثابت حرف *طريقو, ثابت حرف *نسخو_2,
		ثابت حرف *upload_معلومات, size_t *upload_معلومات_سائيز, فراغ **con_cls)
{
	اڻ_استعمال_ٿيل(نسخو_2);
	اڻ_استعمال_ٿيل(upload_معلومات);
	اڻ_استعمال_ٿيل(upload_معلومات_سائيز);
	اڻ_استعمال_ٿيل(con_cls);

	بنية mosquitto__listener *listener = cls;

	إذا(strcmp(طريقو, "GET") != 0){
		حرف *buf = "Invalid HTTP Method\n";
		بنية MHD_Response *response = MHD_create_response_from_بفر(لڙي_ڊيگهه(buf), (فراغ *)buf, MHD_RESPMEM_MUST_COPY);
		تعداد MHD_نتيجو ret = MHD_قطار_response(connection, MHD_HTTP_طريقو_NOT_ALLOWED, response);
		MHD_destroy_response(response);
		أرجع ret;
	}

	إذا(check_access(listener, connection, url) != MOSQ_ERR_SUCCESS){
		أرجع MHD_YES;
	}

	إذا(!strncasecmp(url, "/api/", لڙي_ڊيگهه("/api/"))){
		أرجع http_api__process_api(connection, url);
	}وإلا{
		أرجع http_api__process_file(listener, connection, url);
	}
}


صحيح http_api_شروعات_local(بنية mosquitto__listener *listener)
{
	listener->security_options = mosquitto_calloc(1, حجم(بنية mosquitto__security_options));
	إذا(listener->security_options == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}

	listener->host = mosquitto_strdup("127.0.0.1");
	إذا(!listener->host){
		mosquitto_FREE(listener->security_options);
		أرجع MOSQ_ERR_NOMEM;
	}
	listener->پورٽ = 9883;

	إذا(db.config->security_options.allow_anonymous == -1){
		listener->security_options->allow_anonymous = true;
	}وإلا{
		listener->security_options->allow_anonymous = db.config->security_options.allow_anonymous;
	}

	إذا(listener->http_dir == عدم && لڙي_ڊيگهه(HTTP_API_DIR) > 0){
#جيڪڏهن_وصف_ٿيل WIN32
		حرف *http_dir_canonical = _fullpath(عدم, HTTP_API_DIR, 0);
#وإلا
		حرف *http_dir_canonical = realpath(HTTP_API_DIR, عدم);
#جيڪڏهن_پورو
		إذا(!http_dir_canonical){
			أرجع MOSQ_ERR_NOMEM;
		}
		mosquitto_FREE(listener->http_dir);
		listener->http_dir = mosquitto_strdup(http_dir_canonical);
		mosquitto_FREE(http_dir_canonical);
	}

	أرجع http_api_شروعات(listener);
}


صحيح http_api_شروعات(بنية mosquitto__listener *listener)
{
	بنانشان صحيح flags = MHD_USE_AUTO_اندروني_THREAD;
	ثابت حرف *bind_پتو;
	uint16_t پورٽ = 9883;
	حرف *x509_cert = عدم;
	حرف *x509_ڪنجي = عدم;

	إذا(!listener->security_options){
		listener->security_options = mosquitto_calloc(1, حجم(بنية mosquitto__security_options));
		إذا(listener->security_options == عدم){
			أرجع MOSQ_ERR_NOMEM;
		}
	}
	listener->protocol = mp_http_api;

	bind_پتو = listener->host;
	پورٽ = listener->پورٽ;

#جيڪڏهن_وصف_ٿيل WITH_TLS
	إذا(listener->certfile && listener->keyfile){
		إذا(mosquitto_read_file(listener->certfile, false, &x509_cert, عدم)){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
			أرجع MOSQ_ERR_INVAL;
		}
		إذا(mosquitto_read_file(listener->keyfile, false, &x509_ڪنجي, عدم)){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			mosquitto_FREE(x509_cert);
			أرجع MOSQ_ERR_INVAL;
		}
		flags |= MHD_USE_TLS;
	}
#جيڪڏهن_پورو

	إذا(bind_پتو){
		حرف service[10];
		بنية addrinfo hints;
		بنية addrinfo *ainfo, *rp;

		snprintf(service, حجم(service), "%d", پورٽ);

		memset(&hints, 0, حجم(بنية addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = AI_PASSIVE;
		hints.ai_socktype = SOCK_STREAM;

		صحيح rc = getaddrinfo(bind_پتو, service, &hints, &ainfo);
		إذا(rc){
			log__printf(عدم, MOSQ_LOG_ERR, "Unable to start http api listener (%s).", strerror(errno));
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_ڪنجي);
			أرجع MOSQ_ERR_ERRNO;
		}

		لكل(rp = ainfo; rp; rp = rp->ai_ايندڙ){
			إذا(rp->ai_family == AF_INET || rp->ai_family == AF_INET6){
				اكسر;
			}
		}
		إذا(!rp){
			log__printf(عدم, MOSQ_LOG_ERR, "Unable to start http api listener, could not find address.");
			freeaddrinfo(ainfo);
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_ڪنجي);

			أرجع MOSQ_ERR_INVAL;
		}

		إذا(rp->ai_family == AF_INET6){
			flags |= MHD_USE_IPv6;
		}
		إذا(x509_cert && x509_ڪنجي){
			listener->mhd = MHD_شروعات_daemon(flags, پورٽ, عدم, عدم, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_ڪنجي, x509_ڪنجي,
					MHD_OPTION_پڇاڙي);
		}وإلا{
			listener->mhd = MHD_شروعات_daemon(flags, پورٽ, عدم, عدم, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_پڇاڙي);
		}
		freeaddrinfo(ainfo);
	}وإلا{
		إذا(x509_cert && x509_ڪنجي){
			listener->mhd = MHD_شروعات_daemon(flags | MHD_USE_DUAL_ڍير, پورٽ, عدم, عدم, &http_api_handler, listener,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_ڪنجي, x509_ڪنجي,
					MHD_OPTION_پڇاڙي);
		}وإلا{
			listener->mhd = MHD_شروعات_daemon(flags | MHD_USE_DUAL_ڍير, پورٽ, عدم, عدم, &http_api_handler, listener,
					MHD_OPTION_پڇاڙي);
		}
	}

	mosquitto_FREE(x509_cert);
	mosquitto_FREE(x509_ڪنجي);

	إذا(listener->mhd){
		log__printf(عدم, MOSQ_LOG_INFO, "Opening http api listen socket on port %d.", پورٽ);
		إذا(listener->http_dir){
			log__printf(عدم, MOSQ_LOG_INFO, "Using http_dir %s", listener->http_dir);
		}
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا{
		أرجع MOSQ_ERR_UNKNOWN;
	}
}


فراغ http_api__stop(بنية mosquitto__listener *listener)
{
	MHD_stop_daemon(listener->mhd);
	listener->mhd = عدم;
}

#جيڪڏهن_پورو
