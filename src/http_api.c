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

#شامل "config.h"

#اگر_تعریف_شدہ WITH_HTTP_API

#شامل <assert.h>
#شامل <errno.h>
#شامل <limits.h>
#شامل <microhttpd.h>
#شامل <string.h>
#شامل <sys/stat.h>
#اگر_غیر_تعریف_شدہ WIN32
#  include <netdb.h>
#اگر_ختم

#شامل "json_help.h"
#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "sys_tree.h"

#اگر_غیر_تعریف_شدہ HTTP_API_DIR
#  define HTTP_API_DIR ""
#اگر_ختم

ساخت metric {
	int64_t current;
	int64_t اگلا;
	ثابت حرف *topic, *topic_alias;
	bool is_max;
};

time_t broker_uptime(خالی);
ساخت MHD_Daemon *mhd = تہی;

#اگر_تعریف_شدہ WITH_SYS_درخت
خارجی ساخت metric metrics[mosq_metric_max];
#اگر_ختم

#تعریف_کرو HTTP_RESPONSE_BUFLEN 10000

#اگر_تعریف_شدہ WIN32
#تعریف_کرو DIR_SEP '\\'
#تعریف_کرو PATH_MAX MAX_راستہ
#ورنہ
#تعریف_کرو DIR_SEP '/'
#اگر_ختم

#اگر_غیر_تعریف_شدہ S_ISREG
#تعریف_کرو S_ISREG(منوال) (((منوال)&S_IFMT) == S_IFREG)
#اگر_ختم


ساکن حرف *http__canonical_filename(
		ثابت حرف *url,
		ثابت حرف *http_dir,
		صحیح *error_رمز)
{
	size_t urllen, slen;
	حرف *filename, *filename_canonical;

	urllen = لڑی_لمبائی(url);
	اگر(url[urllen-1] == '/'){
		slen = لڑی_لمبائی(http_dir) + urllen + لڑی_لمبائی("/index.html") + 2;
	}ورنہ{
		slen = لڑی_لمبائی(http_dir) + urllen + 2;
	}
	filename = mosquitto_malloc(slen);
	اگر(!filename){
		*error_رمز = MHD_HTTP_اندرونی_SERVER_ERROR;
		واپس تہی;
	}
	اگر(((حرف *)url)[urllen-1] == '/'){
		snprintf(filename, slen, "%s%c%sindex.html", http_dir, DIR_SEP, url);
	}ورنہ{
		snprintf(filename, slen, "%s%c%s", http_dir, DIR_SEP, url);
	}


	/* Get canonical path and check it is within our http_dir */
	filename_canonical = mosquitto_calloc(1, PATH_MAX);
	اگر(!filename_canonical){
		*error_رمز = MHD_HTTP_اندرونی_SERVER_ERROR;
		واپس تہی;
	}
#اگر_تعریف_شدہ WIN32
	حرف *resolved = _fullpath(filename_canonical, filename, PATH_MAX);
	mosquitto_FREE(filename);
	اگر(!resolved){
		*error_رمز = MHD_HTTP_اندرونی_SERVER_ERROR;
		mosquitto_FREE(filename_canonical);
		واپس تہی;
	}
#ورنہ
	حرف *resolved = realpath(filename, filename_canonical);
	mosquitto_FREE(filename);
	اگر(!resolved){
		اگر(errno == EACCES){
			*error_رمز = MHD_HTTP_FORBIDDEN;
		}ورنہ اگر(errno == EINVAL || errno == EIO || errno == ELOOP){
			*error_رمز = MHD_HTTP_اندرونی_SERVER_ERROR;
		}ورنہ اگر(errno == ENAMETOOLONG){
			*error_رمز = MHD_HTTP_URI_TOO_LONG;
		}ورنہ اگر(errno == ENOENT || errno == ENOTDIR){
			*error_رمز = MHD_HTTP_NOT_ملا;
		}
		mosquitto_FREE(filename_canonical);
		واپس تہی;
	}
#اگر_ختم
	اگر(لڑی_موازنہ(http_dir, filename_canonical, لڑی_لمبائی(http_dir))){
		/* Requested file isn't within http_dir, deny access because it's not found. */
		mosquitto_FREE(filename_canonical);
		*error_رمز = MHD_HTTP_NOT_ملا;
		واپس تہی;
	}

	واپس filename_canonical;
}

ساکن شمار MHD_نتیجہ http_api__send_error_response(ساخت MHD_Connection *connection, ثابت حرف *error_str, غیرعلامتی صحیح error_رمز)
{
	ساخت MHD_Response *response = MHD_create_response_from_بفر(لڑی_لمبائی(error_str), (خالی *)error_str, MHD_RESPMEM_MUST_COPY);
	شمار MHD_نتیجہ ret = MHD_قطار_response(connection, error_رمز, response);
	MHD_destroy_response(response);
	واپس ret;
}

ساکن شمار MHD_نتیجہ http_api_send_response_with_سرخیاں(ساخت MHD_Connection *connection, ثابت حرف *buf)
{
	شمار MHD_نتیجہ ret;
	ساخت MHD_Response *response = MHD_create_response_from_بفر(لڑی_لمبائی(buf), (خالی *)buf, MHD_RESPMEM_MUST_COPY);

	ret = MHD_جوڑو_response_سرخی(response, "Access-Control-Allow-Origin", "*");
	اگر(ret != MHD_YES){
		MHD_destroy_response(response);
		واپس ret;
	}
	ret = MHD_قطار_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	واپس ret;
}

ساکن شمار MHD_نتیجہ http_api_process_نسخہ(ساخت MHD_Connection *connection)
{
	واپس http_api_send_response_with_سرخیاں(connection, نسخہ);
}

ساکن شمار MHD_نتیجہ http_api__process_listeners(ساخت MHD_Connection *connection)
{
	حرف *buf;
	شمار MHD_نتیجہ ret;

	cJSON *j_درخت = cJSON_CreateObject();
	اگر(!j_درخت){
		واپس http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_اندرونی_SERVER_ERROR);
	}

	cJSON *j_listeners = c_JSON_جوڑو_Array_مقصد_Object(j_درخت, "listeners");
	اگر(!j_listeners){
		cJSON_Delete(j_درخت);
		واپس http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_اندرونی_SERVER_ERROR);
	}

	برائے(صحیح i=0; i<db.config->listener_گنتی; i++){
		cJSON *j_listener = cJSON_CreateObject();
		اگر(!j_listener){
			cJSON_Delete(j_درخت);
			واپس http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_اندرونی_SERVER_ERROR);
		}
		c_JSON_جوڑو_شے_مقصد_Array(j_listeners, j_listener);

		ساخت mosquitto__listener *listener = &db.config->listeners[i];
#اگر_تعریف_شدہ WITH_UNIX_SOCKETS
		اگر(listener->unix_socket_راستہ){
			c_JSON_جوڑو_String_مقصد_Object(j_listener, "path", listener->unix_socket_راستہ);
		}ورنہ
#اگر_ختم
		{
			c_JSON_جوڑو_Int_مقصد_Object(j_listener, "port", listener->پورٹ);
		}

		انتخاب(listener->protocol){
			حالت mp_mqtt:
				c_JSON_جوڑو_String_مقصد_Object(j_listener, "protocol", "mqtt");
				توڑو;
			حالت mp_mqttsn:
				c_JSON_جوڑو_String_مقصد_Object(j_listener, "protocol", "mqtt-sn");
				توڑو;
			حالت mp_websockets:
				c_JSON_جوڑو_String_مقصد_Object(j_listener, "protocol", "websockets");
				توڑو;
			حالت mp_http_api:
				c_JSON_جوڑو_String_مقصد_Object(j_listener, "protocol", "httpapi");
				توڑو;
		}

#اگر_تعریف_شدہ WITH_TLS
		c_JSON_جوڑو_Bool_مقصد_Object(j_listener, "tls", listener->certfile && listener->keyfile);
		c_JSON_جوڑو_Bool_مقصد_Object(j_listener, "mtls", listener->require_certificate);
#ورنہ
		c_JSON_جوڑو_Bool_مقصد_Object(j_listener, "tls", false);
		c_JSON_جوڑو_Bool_مقصد_Object(j_listener, "mtls", false);
#اگر_ختم
		اگر(listener->security_options->allow_anonymous == -1){
			c_JSON_جوڑو_Bool_مقصد_Object(j_listener, "allow_anonymous", db.config->security_options.allow_anonymous);
		}ورنہ{
			c_JSON_جوڑو_Bool_مقصد_Object(j_listener, "allow_anonymous", listener->security_options->allow_anonymous);
		}
	}
	buf = cJSON_Print(j_درخت);
	cJSON_Delete(j_درخت);
	اگر(buf){
		ret = http_api_send_response_with_سرخیاں(connection, buf);
		آزاد(buf);
	}ورنہ{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_اندرونی_SERVER_ERROR);
	}

	واپس ret;
}


ساکن شمار MHD_نتیجہ http_api__process_systree(ساخت MHD_Connection *connection)
{
	شمار MHD_نتیجہ ret;
#اگر_تعریف_شدہ WITH_SYS_درخت
	حرف *buf;

	cJSON *j_درخت = cJSON_CreateObject();
	برائے(صحیح i=0; i<mosq_metric_max; i++){
		اگر(metrics[i].topic){
			c_JSON_جوڑو_Int_مقصد_Object(j_درخت, metrics[i].topic, metrics[i].current);
		}
	}
	c_JSON_جوڑو_Int_مقصد_Object(j_درخت, "$SYS/broker/uptime", broker_uptime());

	buf = cJSON_Print(j_درخت);
	cJSON_Delete(j_درخت);

	اگر(buf){
		ret = http_api_send_response_with_سرخیاں(connection, buf);
		آزاد(buf);
	}ورنہ{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_اندرونی_SERVER_ERROR);
	}
#ورنہ
	ret = http_api__send_error_response(connection, "Not found.\n", 404);
#اگر_ختم

	واپس ret;
}


ساکن ssize_t http_file_read_cb(خالی *cls, uint64_t pos, حرف *buf, size_t max)
{
	FILE *fptr = cls;
	اگر(fseek(fptr, (طویل )pos, SEEK_SET) < 0){
		واپس -1;
	}
	واپس (ssize_t )fread(buf, 1, max, fptr);
}


ساکن خالی http_file_free_cb(خالی *cls)
{
	FILE *fptr = cls;
	fclose(fptr);
}


ساکن شمار MHD_نتیجہ http_api__process_file(ساخت mosquitto__listener *listener, ساخت MHD_Connection *connection, ثابت حرف *url)
{
	صحیح error_رمز = MHD_HTTP_اندرونی_SERVER_ERROR;
	حرف *canonical_filename;

	اگر(!listener || !listener->http_dir){
		http_api__send_error_response(connection, "Not found.\n", 404);
		واپس MHD_YES;
	}

	canonical_filename = http__canonical_filename(url, listener->http_dir, &error_رمز);
	اگر(!canonical_filename){
		http_api__send_error_response(connection, "Not found.\n", 404);
		واپس MHD_YES;
	}

	FILE *fptr = fopen(canonical_filename, "rb");
	mosquitto_FREE(canonical_filename);
	اگر(!fptr){
		http_api__send_error_response(connection, "Not found.\n", 404);
		واپس MHD_YES;
	}

	ساخت stat statbuf;
	اگر(fstat(fileno(fptr), &statbuf)){
		fclose(fptr);
		http_api__send_error_response(connection, "Internal server error.\n", 500);
		واپس MHD_YES;
	}

	اگر(!S_ISREG(statbuf.st_منوال)){
		fclose(fptr);
		http_api__send_error_response(connection, "Not found.\n", 404);
		واپس MHD_YES;
	}
	uint64_t flen = (uint64_t )statbuf.st_سائز;

	/* Using MHD_create_response_from_fd would be easier here, but is less portable */
	ساخت MHD_Response *response = MHD_create_response_from_callback(
			flen,
			65536,
			http_file_read_cb,
			fptr,
			http_file_free_cb);

	شمار MHD_نتیجہ ret = MHD_قطار_response(connection, 200, response);
	MHD_destroy_response(response);
	واپس ret;
}


ساکن شمار MHD_نتیجہ http_api__process_api(ساخت MHD_Connection *connection, ثابت حرف *url)
{
	اگر(strcmp(url, "/api/v1/systree") == 0){
		واپس http_api__process_systree(connection);
	}ورنہ اگر(strcmp(url, "/api/v1/listeners") == 0){
		واپس http_api__process_listeners(connection);
	}ورنہ اگر(strcmp(url, "/api/v1/version") == 0){
		واپس http_api_process_نسخہ(connection);
	}ورنہ{
		واپس http_api__send_error_response(connection, "Not found.\n", 404);
	}
}


ساکن صحیح check_access(ساخت mosquitto__listener *listener, ساخت MHD_Connection *connection, ثابت حرف *url)
{
	ساخت mosquitto context = {0};
	صحیح auth_rc, acl_rc = MOSQ_ERR_SUCCESS;

	context.listener = listener;

	context.id = (حرف *)"http-api";
	context.username = MHD_basic_auth_get_username_password(connection, &context.password);

	/* Authentication */
	auth_rc = mosquitto_basic_auth(&context);
	اگر(auth_rc == MOSQ_ERR_SUCCESS){
		acl_rc = mosquitto_acl_check(&context, url, 0, تہی, 0, false, تہی, MOSQ_ACL_READ);
	}
	MHD_free(context.username);
	MHD_free(context.password);

	اگر(auth_rc || acl_rc){
		ثابت حرف *buf = "Not authorised\n";
		ساخت MHD_Response *response = MHD_create_response_from_بفر(لڑی_لمبائی(buf), (خالی *)buf, MHD_RESPMEM_MUST_COPY);
		MHD_قطار_basic_auth_fail_response(connection, "Mosquitto API", response);
		MHD_destroy_response(response);

		واپس MOSQ_ERR_AUTH;
	}

	واپس MOSQ_ERR_SUCCESS;
}

ساکن شمار MHD_نتیجہ http_api_handler(خالی *cls, ساخت MHD_Connection
		*connection, ثابت حرف *url, ثابت حرف *طریقہ, ثابت حرف *نسخہ_2,
		ثابت حرف *upload_ڈیٹا, size_t *upload_ڈیٹا_سائز, خالی **con_cls)
{
	غیر_مستعمل(نسخہ_2);
	غیر_مستعمل(upload_ڈیٹا);
	غیر_مستعمل(upload_ڈیٹا_سائز);
	غیر_مستعمل(con_cls);

	ساخت mosquitto__listener *listener = cls;

	اگر(strcmp(طریقہ, "GET") != 0){
		حرف *buf = "Invalid HTTP Method\n";
		ساخت MHD_Response *response = MHD_create_response_from_بفر(لڑی_لمبائی(buf), (خالی *)buf, MHD_RESPMEM_MUST_COPY);
		شمار MHD_نتیجہ ret = MHD_قطار_response(connection, MHD_HTTP_طریقہ_NOT_ALLOWED, response);
		MHD_destroy_response(response);
		واپس ret;
	}

	اگر(check_access(listener, connection, url) != MOSQ_ERR_SUCCESS){
		واپس MHD_YES;
	}

	اگر(!strncasecmp(url, "/api/", لڑی_لمبائی("/api/"))){
		واپس http_api__process_api(connection, url);
	}ورنہ{
		واپس http_api__process_file(listener, connection, url);
	}
}


صحیح http_api_آغاز_local(ساخت mosquitto__listener *listener)
{
	listener->security_options = mosquitto_calloc(1, حجم(ساخت mosquitto__security_options));
	اگر(listener->security_options == تہی){
		واپس MOSQ_ERR_NOMEM;
	}

	listener->host = mosquitto_strdup("127.0.0.1");
	اگر(!listener->host){
		mosquitto_FREE(listener->security_options);
		واپس MOSQ_ERR_NOMEM;
	}
	listener->پورٹ = 9883;

	اگر(db.config->security_options.allow_anonymous == -1){
		listener->security_options->allow_anonymous = true;
	}ورنہ{
		listener->security_options->allow_anonymous = db.config->security_options.allow_anonymous;
	}

	اگر(listener->http_dir == تہی && لڑی_لمبائی(HTTP_API_DIR) > 0){
#اگر_تعریف_شدہ WIN32
		حرف *http_dir_canonical = _fullpath(تہی, HTTP_API_DIR, 0);
#ورنہ
		حرف *http_dir_canonical = realpath(HTTP_API_DIR, تہی);
#اگر_ختم
		اگر(!http_dir_canonical){
			واپس MOSQ_ERR_NOMEM;
		}
		mosquitto_FREE(listener->http_dir);
		listener->http_dir = mosquitto_strdup(http_dir_canonical);
		mosquitto_FREE(http_dir_canonical);
	}

	واپس http_api_آغاز(listener);
}


صحیح http_api_آغاز(ساخت mosquitto__listener *listener)
{
	غیرعلامتی صحیح flags = MHD_USE_AUTO_اندرونی_THREAD;
	ثابت حرف *bind_پتہ;
	uint16_t پورٹ = 9883;
	حرف *x509_cert = تہی;
	حرف *x509_کلید = تہی;

	اگر(!listener->security_options){
		listener->security_options = mosquitto_calloc(1, حجم(ساخت mosquitto__security_options));
		اگر(listener->security_options == تہی){
			واپس MOSQ_ERR_NOMEM;
		}
	}
	listener->protocol = mp_http_api;

	bind_پتہ = listener->host;
	پورٹ = listener->پورٹ;

#اگر_تعریف_شدہ WITH_TLS
	اگر(listener->certfile && listener->keyfile){
		اگر(mosquitto_read_file(listener->certfile, false, &x509_cert, تہی)){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
			واپس MOSQ_ERR_INVAL;
		}
		اگر(mosquitto_read_file(listener->keyfile, false, &x509_کلید, تہی)){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			mosquitto_FREE(x509_cert);
			واپس MOSQ_ERR_INVAL;
		}
		flags |= MHD_USE_TLS;
	}
#اگر_ختم

	اگر(bind_پتہ){
		حرف service[10];
		ساخت addrinfo hints;
		ساخت addrinfo *ainfo, *rp;

		snprintf(service, حجم(service), "%d", پورٹ);

		memset(&hints, 0, حجم(ساخت addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = AI_PASSIVE;
		hints.ai_socktype = SOCK_STREAM;

		صحیح rc = getaddrinfo(bind_پتہ, service, &hints, &ainfo);
		اگر(rc){
			log__printf(تہی, MOSQ_LOG_ERR, "Unable to start http api listener (%s).", strerror(errno));
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_کلید);
			واپس MOSQ_ERR_ERRNO;
		}

		برائے(rp = ainfo; rp; rp = rp->ai_اگلا){
			اگر(rp->ai_family == AF_INET || rp->ai_family == AF_INET6){
				توڑو;
			}
		}
		اگر(!rp){
			log__printf(تہی, MOSQ_LOG_ERR, "Unable to start http api listener, could not find address.");
			freeaddrinfo(ainfo);
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_کلید);

			واپس MOSQ_ERR_INVAL;
		}

		اگر(rp->ai_family == AF_INET6){
			flags |= MHD_USE_IPv6;
		}
		اگر(x509_cert && x509_کلید){
			listener->mhd = MHD_آغاز_daemon(flags, پورٹ, تہی, تہی, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_کلید, x509_کلید,
					MHD_OPTION_انتہا);
		}ورنہ{
			listener->mhd = MHD_آغاز_daemon(flags, پورٹ, تہی, تہی, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_انتہا);
		}
		freeaddrinfo(ainfo);
	}ورنہ{
		اگر(x509_cert && x509_کلید){
			listener->mhd = MHD_آغاز_daemon(flags | MHD_USE_DUAL_ڈھیر, پورٹ, تہی, تہی, &http_api_handler, listener,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_کلید, x509_کلید,
					MHD_OPTION_انتہا);
		}ورنہ{
			listener->mhd = MHD_آغاز_daemon(flags | MHD_USE_DUAL_ڈھیر, پورٹ, تہی, تہی, &http_api_handler, listener,
					MHD_OPTION_انتہا);
		}
	}

	mosquitto_FREE(x509_cert);
	mosquitto_FREE(x509_کلید);

	اگر(listener->mhd){
		log__printf(تہی, MOSQ_LOG_INFO, "Opening http api listen socket on port %d.", پورٹ);
		اگر(listener->http_dir){
			log__printf(تہی, MOSQ_LOG_INFO, "Using http_dir %s", listener->http_dir);
		}
		واپس MOSQ_ERR_SUCCESS;
	}ورنہ{
		واپس MOSQ_ERR_UNKNOWN;
	}
}


خالی http_api__stop(ساخت mosquitto__listener *listener)
{
	MHD_stop_daemon(listener->mhd);
	listener->mhd = تہی;
}

#اگر_ختم
