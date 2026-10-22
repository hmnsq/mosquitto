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

#جے_تعریف_شدہ WITH_HTTP_API

#ضمن <assert.h>
#ضمن <errno.h>
#ضمن <limits.h>
#ضمن <microhttpd.h>
#ضمن <string.h>
#ضمن <sys/stat.h>
#جے_بنا_تعریف WIN32
#  include <netdb.h>
#جے_انت

#ضمن "json_help.h"
#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "sys_tree.h"

#جے_بنا_تعریف HTTP_API_DIR
#  define HTTP_API_DIR ""
#جے_انت

بنية metric {
	int64_t current;
	int64_t اگلا;
	ثابت حرف *topic, *topic_alias;
	bool is_max;
};

time_t broker_uptime(فراغ);
بنية MHD_Daemon *mhd = عدم;

#جے_تعریف_شدہ WITH_SYS_درخت
خارجي بنية metric metrics[mosq_metric_max];
#جے_انت

#تعریف_کرو HTTP_RESPONSE_BUFLEN 10000

#جے_تعریف_شدہ WIN32
#تعریف_کرو DIR_SEP '\\'
#تعریف_کرو PATH_MAX MAX_رستہ
#وإلا
#تعریف_کرو DIR_SEP '/'
#جے_انت

#جے_بنا_تعریف S_ISREG
#تعریف_کرو S_ISREG(منوال) (((منوال)&S_IFMT) == S_IFREG)
#جے_انت


ساكن حرف *http__canonical_filename(
		ثابت حرف *url,
		ثابت حرف *http_dir,
		صحيح *error_کوڈ)
{
	size_t urllen, slen;
	حرف *filename, *filename_canonical;

	urllen = لڑی_لمبائی(url);
	إذا(url[urllen-1] == '/'){
		slen = لڑی_لمبائی(http_dir) + urllen + لڑی_لمبائی("/index.html") + 2;
	}وإلا{
		slen = لڑی_لمبائی(http_dir) + urllen + 2;
	}
	filename = mosquitto_malloc(slen);
	إذا(!filename){
		*error_کوڈ = MHD_HTTP_اندرونی_SERVER_ERROR;
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
		*error_کوڈ = MHD_HTTP_اندرونی_SERVER_ERROR;
		أرجع عدم;
	}
#جے_تعریف_شدہ WIN32
	حرف *resolved = _fullpath(filename_canonical, filename, PATH_MAX);
	mosquitto_FREE(filename);
	إذا(!resolved){
		*error_کوڈ = MHD_HTTP_اندرونی_SERVER_ERROR;
		mosquitto_FREE(filename_canonical);
		أرجع عدم;
	}
#وإلا
	حرف *resolved = realpath(filename, filename_canonical);
	mosquitto_FREE(filename);
	إذا(!resolved){
		إذا(errno == EACCES){
			*error_کوڈ = MHD_HTTP_FORBIDDEN;
		}وإلا إذا(errno == EINVAL || errno == EIO || errno == ELOOP){
			*error_کوڈ = MHD_HTTP_اندرونی_SERVER_ERROR;
		}وإلا إذا(errno == ENAMETOOLONG){
			*error_کوڈ = MHD_HTTP_URI_TOO_LONG;
		}وإلا إذا(errno == ENOENT || errno == ENOTDIR){
			*error_کوڈ = MHD_HTTP_NOT_لبھیا;
		}
		mosquitto_FREE(filename_canonical);
		أرجع عدم;
	}
#جے_انت
	إذا(لڑی_مقابلہ(http_dir, filename_canonical, لڑی_لمبائی(http_dir))){
		/* Requested file isn't within http_dir, deny access because it's not found. */
		mosquitto_FREE(filename_canonical);
		*error_کوڈ = MHD_HTTP_NOT_لبھیا;
		أرجع عدم;
	}

	أرجع filename_canonical;
}

ساكن تعداد MHD_نتیجہ http_api__send_error_response(بنية MHD_Connection *connection, ثابت حرف *error_str, بنانشانی صحيح error_کوڈ)
{
	بنية MHD_Response *response = MHD_create_response_from_بفر(لڑی_لمبائی(error_str), (فراغ *)error_str, MHD_RESPMEM_MUST_COPY);
	تعداد MHD_نتیجہ ret = MHD_قطار_response(connection, error_کوڈ, response);
	MHD_destroy_response(response);
	أرجع ret;
}

ساكن تعداد MHD_نتیجہ http_api_send_response_with_سرخیاں(بنية MHD_Connection *connection, ثابت حرف *buf)
{
	تعداد MHD_نتیجہ ret;
	بنية MHD_Response *response = MHD_create_response_from_بفر(لڑی_لمبائی(buf), (فراغ *)buf, MHD_RESPMEM_MUST_COPY);

	ret = MHD_جوڑو_response_سرخی(response, "Access-Control-Allow-Origin", "*");
	إذا(ret != MHD_YES){
		MHD_destroy_response(response);
		أرجع ret;
	}
	ret = MHD_قطار_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	أرجع ret;
}

ساكن تعداد MHD_نتیجہ http_api_process_نسخہ(بنية MHD_Connection *connection)
{
	أرجع http_api_send_response_with_سرخیاں(connection, نسخہ);
}

ساكن تعداد MHD_نتیجہ http_api__process_listeners(بنية MHD_Connection *connection)
{
	حرف *buf;
	تعداد MHD_نتیجہ ret;

	cJSON *j_درخت = cJSON_CreateObject();
	إذا(!j_درخت){
		أرجع http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_اندرونی_SERVER_ERROR);
	}

	cJSON *j_listeners = c_JSON_جوڑو_Array_منزل_Object(j_درخت, "listeners");
	إذا(!j_listeners){
		cJSON_Delete(j_درخت);
		أرجع http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_اندرونی_SERVER_ERROR);
	}

	لكل(صحيح i=0; i<db.config->listener_گنتی; i++){
		cJSON *j_listener = cJSON_CreateObject();
		إذا(!j_listener){
			cJSON_Delete(j_درخت);
			أرجع http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_اندرونی_SERVER_ERROR);
		}
		c_JSON_جوڑو_شے_منزل_Array(j_listeners, j_listener);

		بنية mosquitto__listener *listener = &db.config->listeners[i];
#جے_تعریف_شدہ WITH_UNIX_SOCKETS
		إذا(listener->unix_socket_رستہ){
			c_JSON_جوڑو_String_منزل_Object(j_listener, "path", listener->unix_socket_رستہ);
		}وإلا
#جے_انت
		{
			c_JSON_جوڑو_Int_منزل_Object(j_listener, "port", listener->پورٹ);
		}

		بدل(listener->protocol){
			حالة mp_mqtt:
				c_JSON_جوڑو_String_منزل_Object(j_listener, "protocol", "mqtt");
				اكسر;
			حالة mp_mqttsn:
				c_JSON_جوڑو_String_منزل_Object(j_listener, "protocol", "mqtt-sn");
				اكسر;
			حالة mp_websockets:
				c_JSON_جوڑو_String_منزل_Object(j_listener, "protocol", "websockets");
				اكسر;
			حالة mp_http_api:
				c_JSON_جوڑو_String_منزل_Object(j_listener, "protocol", "httpapi");
				اكسر;
		}

#جے_تعریف_شدہ WITH_TLS
		c_JSON_جوڑو_Bool_منزل_Object(j_listener, "tls", listener->certfile && listener->keyfile);
		c_JSON_جوڑو_Bool_منزل_Object(j_listener, "mtls", listener->require_certificate);
#وإلا
		c_JSON_جوڑو_Bool_منزل_Object(j_listener, "tls", false);
		c_JSON_جوڑو_Bool_منزل_Object(j_listener, "mtls", false);
#جے_انت
		إذا(listener->security_options->allow_anonymous == -1){
			c_JSON_جوڑو_Bool_منزل_Object(j_listener, "allow_anonymous", db.config->security_options.allow_anonymous);
		}وإلا{
			c_JSON_جوڑو_Bool_منزل_Object(j_listener, "allow_anonymous", listener->security_options->allow_anonymous);
		}
	}
	buf = cJSON_Print(j_درخت);
	cJSON_Delete(j_درخت);
	إذا(buf){
		ret = http_api_send_response_with_سرخیاں(connection, buf);
		حرر(buf);
	}وإلا{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_اندرونی_SERVER_ERROR);
	}

	أرجع ret;
}


ساكن تعداد MHD_نتیجہ http_api__process_systree(بنية MHD_Connection *connection)
{
	تعداد MHD_نتیجہ ret;
#جے_تعریف_شدہ WITH_SYS_درخت
	حرف *buf;

	cJSON *j_درخت = cJSON_CreateObject();
	لكل(صحيح i=0; i<mosq_metric_max; i++){
		إذا(metrics[i].topic){
			c_JSON_جوڑو_Int_منزل_Object(j_درخت, metrics[i].topic, metrics[i].current);
		}
	}
	c_JSON_جوڑو_Int_منزل_Object(j_درخت, "$SYS/broker/uptime", broker_uptime());

	buf = cJSON_Print(j_درخت);
	cJSON_Delete(j_درخت);

	إذا(buf){
		ret = http_api_send_response_with_سرخیاں(connection, buf);
		حرر(buf);
	}وإلا{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_اندرونی_SERVER_ERROR);
	}
#وإلا
	ret = http_api__send_error_response(connection, "Not found.\n", 404);
#جے_انت

	أرجع ret;
}


ساكن ssize_t http_file_read_cb(فراغ *cls, uint64_t pos, حرف *buf, size_t max)
{
	FILE *fptr = cls;
	إذا(fseek(fptr, (لما )pos, SEEK_SET) < 0){
		أرجع -1;
	}
	أرجع (ssize_t )fread(buf, 1, max, fptr);
}


ساكن فراغ http_file_free_cb(فراغ *cls)
{
	FILE *fptr = cls;
	fclose(fptr);
}


ساكن تعداد MHD_نتیجہ http_api__process_file(بنية mosquitto__listener *listener, بنية MHD_Connection *connection, ثابت حرف *url)
{
	صحيح error_کوڈ = MHD_HTTP_اندرونی_SERVER_ERROR;
	حرف *canonical_filename;

	إذا(!listener || !listener->http_dir){
		http_api__send_error_response(connection, "Not found.\n", 404);
		أرجع MHD_YES;
	}

	canonical_filename = http__canonical_filename(url, listener->http_dir, &error_کوڈ);
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

	إذا(!S_ISREG(statbuf.st_منوال)){
		fclose(fptr);
		http_api__send_error_response(connection, "Not found.\n", 404);
		أرجع MHD_YES;
	}
	uint64_t flen = (uint64_t )statbuf.st_ناپ;

	/* Using MHD_create_response_from_fd would be easier here, but is less portable */
	بنية MHD_Response *response = MHD_create_response_from_callback(
			flen,
			65536,
			http_file_read_cb,
			fptr,
			http_file_free_cb);

	تعداد MHD_نتیجہ ret = MHD_قطار_response(connection, 200, response);
	MHD_destroy_response(response);
	أرجع ret;
}


ساكن تعداد MHD_نتیجہ http_api__process_api(بنية MHD_Connection *connection, ثابت حرف *url)
{
	إذا(strcmp(url, "/api/v1/systree") == 0){
		أرجع http_api__process_systree(connection);
	}وإلا إذا(strcmp(url, "/api/v1/listeners") == 0){
		أرجع http_api__process_listeners(connection);
	}وإلا إذا(strcmp(url, "/api/v1/version") == 0){
		أرجع http_api_process_نسخہ(connection);
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
		بنية MHD_Response *response = MHD_create_response_from_بفر(لڑی_لمبائی(buf), (فراغ *)buf, MHD_RESPMEM_MUST_COPY);
		MHD_قطار_basic_auth_fail_response(connection, "Mosquitto API", response);
		MHD_destroy_response(response);

		أرجع MOSQ_ERR_AUTH;
	}

	أرجع MOSQ_ERR_SUCCESS;
}

ساكن تعداد MHD_نتیجہ http_api_handler(فراغ *cls, بنية MHD_Connection
		*connection, ثابت حرف *url, ثابت حرف *طریقہ, ثابت حرف *نسخہ_2,
		ثابت حرف *upload_انکڑے, size_t *upload_انکڑے_ناپ, فراغ **con_cls)
{
	بن_ورتیا(نسخہ_2);
	بن_ورتیا(upload_انکڑے);
	بن_ورتیا(upload_انکڑے_ناپ);
	بن_ورتیا(con_cls);

	بنية mosquitto__listener *listener = cls;

	إذا(strcmp(طریقہ, "GET") != 0){
		حرف *buf = "Invalid HTTP Method\n";
		بنية MHD_Response *response = MHD_create_response_from_بفر(لڑی_لمبائی(buf), (فراغ *)buf, MHD_RESPMEM_MUST_COPY);
		تعداد MHD_نتیجہ ret = MHD_قطار_response(connection, MHD_HTTP_طریقہ_NOT_ALLOWED, response);
		MHD_destroy_response(response);
		أرجع ret;
	}

	إذا(check_access(listener, connection, url) != MOSQ_ERR_SUCCESS){
		أرجع MHD_YES;
	}

	إذا(!strncasecmp(url, "/api/", لڑی_لمبائی("/api/"))){
		أرجع http_api__process_api(connection, url);
	}وإلا{
		أرجع http_api__process_file(listener, connection, url);
	}
}


صحيح http_api_شروع_local(بنية mosquitto__listener *listener)
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
	listener->پورٹ = 9883;

	إذا(db.config->security_options.allow_anonymous == -1){
		listener->security_options->allow_anonymous = true;
	}وإلا{
		listener->security_options->allow_anonymous = db.config->security_options.allow_anonymous;
	}

	إذا(listener->http_dir == عدم && لڑی_لمبائی(HTTP_API_DIR) > 0){
#جے_تعریف_شدہ WIN32
		حرف *http_dir_canonical = _fullpath(عدم, HTTP_API_DIR, 0);
#وإلا
		حرف *http_dir_canonical = realpath(HTTP_API_DIR, عدم);
#جے_انت
		إذا(!http_dir_canonical){
			أرجع MOSQ_ERR_NOMEM;
		}
		mosquitto_FREE(listener->http_dir);
		listener->http_dir = mosquitto_strdup(http_dir_canonical);
		mosquitto_FREE(http_dir_canonical);
	}

	أرجع http_api_شروع(listener);
}


صحيح http_api_شروع(بنية mosquitto__listener *listener)
{
	بنانشانی صحيح flags = MHD_USE_AUTO_اندرونی_THREAD;
	ثابت حرف *bind_پتہ;
	uint16_t پورٹ = 9883;
	حرف *x509_cert = عدم;
	حرف *x509_کلید = عدم;

	إذا(!listener->security_options){
		listener->security_options = mosquitto_calloc(1, حجم(بنية mosquitto__security_options));
		إذا(listener->security_options == عدم){
			أرجع MOSQ_ERR_NOMEM;
		}
	}
	listener->protocol = mp_http_api;

	bind_پتہ = listener->host;
	پورٹ = listener->پورٹ;

#جے_تعریف_شدہ WITH_TLS
	إذا(listener->certfile && listener->keyfile){
		إذا(mosquitto_read_file(listener->certfile, false, &x509_cert, عدم)){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
			أرجع MOSQ_ERR_INVAL;
		}
		إذا(mosquitto_read_file(listener->keyfile, false, &x509_کلید, عدم)){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			mosquitto_FREE(x509_cert);
			أرجع MOSQ_ERR_INVAL;
		}
		flags |= MHD_USE_TLS;
	}
#جے_انت

	إذا(bind_پتہ){
		حرف service[10];
		بنية addrinfo hints;
		بنية addrinfo *ainfo, *rp;

		snprintf(service, حجم(service), "%d", پورٹ);

		memset(&hints, 0, حجم(بنية addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = AI_PASSIVE;
		hints.ai_socktype = SOCK_STREAM;

		صحيح rc = getaddrinfo(bind_پتہ, service, &hints, &ainfo);
		إذا(rc){
			log__printf(عدم, MOSQ_LOG_ERR, "Unable to start http api listener (%s).", strerror(errno));
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_کلید);
			أرجع MOSQ_ERR_ERRNO;
		}

		لكل(rp = ainfo; rp; rp = rp->ai_اگلا){
			إذا(rp->ai_family == AF_INET || rp->ai_family == AF_INET6){
				اكسر;
			}
		}
		إذا(!rp){
			log__printf(عدم, MOSQ_LOG_ERR, "Unable to start http api listener, could not find address.");
			freeaddrinfo(ainfo);
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_کلید);

			أرجع MOSQ_ERR_INVAL;
		}

		إذا(rp->ai_family == AF_INET6){
			flags |= MHD_USE_IPv6;
		}
		إذا(x509_cert && x509_کلید){
			listener->mhd = MHD_شروع_daemon(flags, پورٹ, عدم, عدم, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_کلید, x509_کلید,
					MHD_OPTION_سرا);
		}وإلا{
			listener->mhd = MHD_شروع_daemon(flags, پورٹ, عدم, عدم, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_سرا);
		}
		freeaddrinfo(ainfo);
	}وإلا{
		إذا(x509_cert && x509_کلید){
			listener->mhd = MHD_شروع_daemon(flags | MHD_USE_DUAL_ڈھیر, پورٹ, عدم, عدم, &http_api_handler, listener,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_کلید, x509_کلید,
					MHD_OPTION_سرا);
		}وإلا{
			listener->mhd = MHD_شروع_daemon(flags | MHD_USE_DUAL_ڈھیر, پورٹ, عدم, عدم, &http_api_handler, listener,
					MHD_OPTION_سرا);
		}
	}

	mosquitto_FREE(x509_cert);
	mosquitto_FREE(x509_کلید);

	إذا(listener->mhd){
		log__printf(عدم, MOSQ_LOG_INFO, "Opening http api listen socket on port %d.", پورٹ);
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

#جے_انت
