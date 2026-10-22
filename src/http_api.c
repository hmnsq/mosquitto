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

#ifdef WITH_HTTP_API

#شامل <assert.h>
#شامل <errno.h>
#شامل <limits.h>
#شامل <microhttpd.h>
#شامل <string.h>
#شامل <sys/stat.h>
#ifndef WIN32
#  include <netdb.h>
#endif

#شامل "json_help.h"
#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "sys_tree.h"

#ifndef HTTP_API_DIR
#  define HTTP_API_DIR ""
#endif

ساختار metric {
	int64_t current;
	int64_t next;
	ثابت نویسه *topic, *topic_alias;
	bool is_max;
};

time_t broker_uptime(تهی);
ساختار MHD_Daemon *mhd = پوچ;

#ifdef WITH_SYS_درخت
بیرونی ساختار metric metrics[mosq_metric_max];
#endif

#define HTTP_RESPONSE_BUFLEN 10000

#ifdef WIN32
#define DIR_SEP '\\'
#define PATH_MAX MAX_PATH
#وگرنه
#define DIR_SEP '/'
#endif

#ifndef S_ISREG
#define S_ISREG(mode) (((mode)&S_IFMT) == S_IFREG)
#endif


ایستا نویسه *http__canonical_filename(
		ثابت نویسه *url,
		ثابت نویسه *http_dir,
		صحیح *error_code)
{
	size_t urllen, slen;
	نویسه *filename, *filename_canonical;

	urllen = strlen(url);
	اگر(url[urllen-1] == '/'){
		slen = strlen(http_dir) + urllen + strlen("/index.html") + 2;
	}وگرنه{
		slen = strlen(http_dir) + urllen + 2;
	}
	filename = mosquitto_malloc(slen);
	اگر(!filename){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		بازگشت پوچ;
	}
	اگر(((نویسه *)url)[urllen-1] == '/'){
		snprintf(filename, slen, "%s%c%sindex.html", http_dir, DIR_SEP, url);
	}وگرنه{
		snprintf(filename, slen, "%s%c%s", http_dir, DIR_SEP, url);
	}


	/* Get canonical path and check it is within our http_dir */
	filename_canonical = mosquitto_calloc(1, PATH_MAX);
	اگر(!filename_canonical){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		بازگشت پوچ;
	}
#ifdef WIN32
	نویسه *resolved = _fullpath(filename_canonical, filename, PATH_MAX);
	mosquitto_FREE(filename);
	اگر(!resolved){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		mosquitto_FREE(filename_canonical);
		بازگشت پوچ;
	}
#وگرنه
	نویسه *resolved = realpath(filename, filename_canonical);
	mosquitto_FREE(filename);
	اگر(!resolved){
		اگر(errno == EACCES){
			*error_code = MHD_HTTP_FORBIDDEN;
		}وگرنه اگر(errno == EINVAL || errno == EIO || errno == ELOOP){
			*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		}وگرنه اگر(errno == ENAMETOOLONG){
			*error_code = MHD_HTTP_URI_TOO_LONG;
		}وگرنه اگر(errno == ENOENT || errno == ENOTDIR){
			*error_code = MHD_HTTP_NOT_FOUND;
		}
		mosquitto_FREE(filename_canonical);
		بازگشت پوچ;
	}
#endif
	اگر(strncmp(http_dir, filename_canonical, strlen(http_dir))){
		/* Requested file isn't within http_dir, deny access because it's not found. */
		mosquitto_FREE(filename_canonical);
		*error_code = MHD_HTTP_NOT_FOUND;
		بازگشت پوچ;
	}

	بازگشت filename_canonical;
}

ایستا شمارش MHD_Result http_api__send_error_response(ساختار MHD_Connection *connection, ثابت نویسه *error_str, unsigned صحیح error_code)
{
	ساختار MHD_Response *response = MHD_create_response_from_buffer(strlen(error_str), (تهی *)error_str, MHD_RESPMEM_MUST_COPY);
	شمارش MHD_Result ret = MHD_queue_response(connection, error_code, response);
	MHD_destroy_response(response);
	بازگشت ret;
}

ایستا شمارش MHD_Result http_api__send_response_with_headers(ساختار MHD_Connection *connection, ثابت نویسه *buf)
{
	شمارش MHD_Result ret;
	ساختار MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (تهی *)buf, MHD_RESPMEM_MUST_COPY);

	ret = MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
	اگر(ret != MHD_YES){
		MHD_destroy_response(response);
		بازگشت ret;
	}
	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	بازگشت ret;
}

ایستا شمارش MHD_Result http_api__process_version(ساختار MHD_Connection *connection)
{
	بازگشت http_api__send_response_with_headers(connection, VERSION);
}

ایستا شمارش MHD_Result http_api__process_listeners(ساختار MHD_Connection *connection)
{
	نویسه *buf;
	شمارش MHD_Result ret;

	cJSON *j_درخت = cJSON_CreateObject();
	اگر(!j_درخت){
		بازگشت http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	cJSON *j_listeners = cJSON_AddArrayToObject(j_درخت, "listeners");
	اگر(!j_listeners){
		cJSON_Delete(j_درخت);
		بازگشت http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	برای(صحیح i=0; i<db.config->listener_شمار; i++){
		cJSON *j_listener = cJSON_CreateObject();
		اگر(!j_listener){
			cJSON_Delete(j_درخت);
			بازگشت http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
		}
		cJSON_AddItemToArray(j_listeners, j_listener);

		ساختار mosquitto__listener *listener = &db.config->listeners[i];
#ifdef WITH_UNIX_SOCKETS
		اگر(listener->unix_socket_path){
			cJSON_AddStringToObject(j_listener, "path", listener->unix_socket_path);
		}وگرنه
#endif
		{
			cJSON_AddIntToObject(j_listener, "port", listener->port);
		}

		انتخاب(listener->protocol){
			حالت mp_mqtt:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt");
				بشکن;
			حالت mp_mqttsn:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt-sn");
				بشکن;
			حالت mp_websockets:
				cJSON_AddStringToObject(j_listener, "protocol", "websockets");
				بشکن;
			حالت mp_http_api:
				cJSON_AddStringToObject(j_listener, "protocol", "httpapi");
				بشکن;
		}

#ifdef WITH_TLS
		cJSON_AddBoolToObject(j_listener, "tls", listener->certfile && listener->keyfile);
		cJSON_AddBoolToObject(j_listener, "mtls", listener->require_certificate);
#وگرنه
		cJSON_AddBoolToObject(j_listener, "tls", false);
		cJSON_AddBoolToObject(j_listener, "mtls", false);
#endif
		اگر(listener->security_options->allow_anonymous == -1){
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", db.config->security_options.allow_anonymous);
		}وگرنه{
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", listener->security_options->allow_anonymous);
		}
	}
	buf = cJSON_Print(j_درخت);
	cJSON_Delete(j_درخت);
	اگر(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		آزاد(buf);
	}وگرنه{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	بازگشت ret;
}


ایستا شمارش MHD_Result http_api__process_systree(ساختار MHD_Connection *connection)
{
	شمارش MHD_Result ret;
#ifdef WITH_SYS_درخت
	نویسه *buf;

	cJSON *j_درخت = cJSON_CreateObject();
	برای(صحیح i=0; i<mosq_metric_max; i++){
		اگر(metrics[i].topic){
			cJSON_AddIntToObject(j_درخت, metrics[i].topic, metrics[i].current);
		}
	}
	cJSON_AddIntToObject(j_درخت, "$SYS/broker/uptime", broker_uptime());

	buf = cJSON_Print(j_درخت);
	cJSON_Delete(j_درخت);

	اگر(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		آزاد(buf);
	}وگرنه{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}
#وگرنه
	ret = http_api__send_error_response(connection, "Not found.\n", 404);
#endif

	بازگشت ret;
}


ایستا ssize_t http_file_read_cb(تهی *cls, uint64_t pos, نویسه *buf, size_t max)
{
	FILE *fptr = cls;
	اگر(fseek(fptr, (long )pos, SEEK_SET) < 0){
		بازگشت -1;
	}
	بازگشت (ssize_t )fread(buf, 1, max, fptr);
}


ایستا تهی http_file_free_cb(تهی *cls)
{
	FILE *fptr = cls;
	fclose(fptr);
}


ایستا شمارش MHD_Result http_api__process_file(ساختار mosquitto__listener *listener, ساختار MHD_Connection *connection, ثابت نویسه *url)
{
	صحیح error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
	نویسه *canonical_filename;

	اگر(!listener || !listener->http_dir){
		http_api__send_error_response(connection, "Not found.\n", 404);
		بازگشت MHD_YES;
	}

	canonical_filename = http__canonical_filename(url, listener->http_dir, &error_code);
	اگر(!canonical_filename){
		http_api__send_error_response(connection, "Not found.\n", 404);
		بازگشت MHD_YES;
	}

	FILE *fptr = fopen(canonical_filename, "rb");
	mosquitto_FREE(canonical_filename);
	اگر(!fptr){
		http_api__send_error_response(connection, "Not found.\n", 404);
		بازگشت MHD_YES;
	}

	ساختار stat statbuf;
	اگر(fstat(fileno(fptr), &statbuf)){
		fclose(fptr);
		http_api__send_error_response(connection, "Internal server error.\n", 500);
		بازگشت MHD_YES;
	}

	اگر(!S_ISREG(statbuf.st_mode)){
		fclose(fptr);
		http_api__send_error_response(connection, "Not found.\n", 404);
		بازگشت MHD_YES;
	}
	uint64_t flen = (uint64_t )statbuf.st_size;

	/* Using MHD_create_response_from_fd would be easier here, but is less portable */
	ساختار MHD_Response *response = MHD_create_response_from_callback(
			flen,
			65536,
			http_file_read_cb,
			fptr,
			http_file_free_cb);

	شمارش MHD_Result ret = MHD_queue_response(connection, 200, response);
	MHD_destroy_response(response);
	بازگشت ret;
}


ایستا شمارش MHD_Result http_api__process_api(ساختار MHD_Connection *connection, ثابت نویسه *url)
{
	اگر(strcmp(url, "/api/v1/systree") == 0){
		بازگشت http_api__process_systree(connection);
	}وگرنه اگر(strcmp(url, "/api/v1/listeners") == 0){
		بازگشت http_api__process_listeners(connection);
	}وگرنه اگر(strcmp(url, "/api/v1/version") == 0){
		بازگشت http_api__process_version(connection);
	}وگرنه{
		بازگشت http_api__send_error_response(connection, "Not found.\n", 404);
	}
}


ایستا صحیح check_access(ساختار mosquitto__listener *listener, ساختار MHD_Connection *connection, ثابت نویسه *url)
{
	ساختار mosquitto context = {0};
	صحیح auth_rc, acl_rc = MOSQ_ERR_SUCCESS;

	context.listener = listener;

	context.id = (نویسه *)"http-api";
	context.username = MHD_basic_auth_get_username_password(connection, &context.password);

	/* Authentication */
	auth_rc = mosquitto_basic_auth(&context);
	اگر(auth_rc == MOSQ_ERR_SUCCESS){
		acl_rc = mosquitto_acl_check(&context, url, 0, پوچ, 0, false, پوچ, MOSQ_ACL_READ);
	}
	MHD_free(context.username);
	MHD_free(context.password);

	اگر(auth_rc || acl_rc){
		ثابت نویسه *buf = "Not authorised\n";
		ساختار MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (تهی *)buf, MHD_RESPMEM_MUST_COPY);
		MHD_queue_basic_auth_fail_response(connection, "Mosquitto API", response);
		MHD_destroy_response(response);

		بازگشت MOSQ_ERR_AUTH;
	}

	بازگشت MOSQ_ERR_SUCCESS;
}

ایستا شمارش MHD_Result http_api_handler(تهی *cls, ساختار MHD_Connection
		*connection, ثابت نویسه *url, ثابت نویسه *method, ثابت نویسه *version,
		ثابت نویسه *upload_data, size_t *upload_data_size, تهی **con_cls)
{
	UNUSED(version);
	UNUSED(upload_data);
	UNUSED(upload_data_size);
	UNUSED(con_cls);

	ساختار mosquitto__listener *listener = cls;

	اگر(strcmp(method, "GET") != 0){
		نویسه *buf = "Invalid HTTP Method\n";
		ساختار MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (تهی *)buf, MHD_RESPMEM_MUST_COPY);
		شمارش MHD_Result ret = MHD_queue_response(connection, MHD_HTTP_METHOD_NOT_ALLOWED, response);
		MHD_destroy_response(response);
		بازگشت ret;
	}

	اگر(check_access(listener, connection, url) != MOSQ_ERR_SUCCESS){
		بازگشت MHD_YES;
	}

	اگر(!strncasecmp(url, "/api/", strlen("/api/"))){
		بازگشت http_api__process_api(connection, url);
	}وگرنه{
		بازگشت http_api__process_file(listener, connection, url);
	}
}


صحیح http_api__start_local(ساختار mosquitto__listener *listener)
{
	listener->security_options = mosquitto_calloc(1, اندازه(ساختار mosquitto__security_options));
	اگر(listener->security_options == پوچ){
		بازگشت MOSQ_ERR_NOMEM;
	}

	listener->host = mosquitto_strdup("127.0.0.1");
	اگر(!listener->host){
		mosquitto_FREE(listener->security_options);
		بازگشت MOSQ_ERR_NOMEM;
	}
	listener->port = 9883;

	اگر(db.config->security_options.allow_anonymous == -1){
		listener->security_options->allow_anonymous = true;
	}وگرنه{
		listener->security_options->allow_anonymous = db.config->security_options.allow_anonymous;
	}

	اگر(listener->http_dir == پوچ && strlen(HTTP_API_DIR) > 0){
#ifdef WIN32
		نویسه *http_dir_canonical = _fullpath(پوچ, HTTP_API_DIR, 0);
#وگرنه
		نویسه *http_dir_canonical = realpath(HTTP_API_DIR, پوچ);
#endif
		اگر(!http_dir_canonical){
			بازگشت MOSQ_ERR_NOMEM;
		}
		mosquitto_FREE(listener->http_dir);
		listener->http_dir = mosquitto_strdup(http_dir_canonical);
		mosquitto_FREE(http_dir_canonical);
	}

	بازگشت http_api__start(listener);
}


صحیح http_api__start(ساختار mosquitto__listener *listener)
{
	unsigned صحیح flags = MHD_USE_AUTO_INTERNAL_THREAD;
	ثابت نویسه *bind_address;
	uint16_t port = 9883;
	نویسه *x509_cert = پوچ;
	نویسه *x509_کلید = پوچ;

	اگر(!listener->security_options){
		listener->security_options = mosquitto_calloc(1, اندازه(ساختار mosquitto__security_options));
		اگر(listener->security_options == پوچ){
			بازگشت MOSQ_ERR_NOMEM;
		}
	}
	listener->protocol = mp_http_api;

	bind_address = listener->host;
	port = listener->port;

#ifdef WITH_TLS
	اگر(listener->certfile && listener->keyfile){
		اگر(mosquitto_read_file(listener->certfile, false, &x509_cert, پوچ)){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
			بازگشت MOSQ_ERR_INVAL;
		}
		اگر(mosquitto_read_file(listener->keyfile, false, &x509_کلید, پوچ)){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			mosquitto_FREE(x509_cert);
			بازگشت MOSQ_ERR_INVAL;
		}
		flags |= MHD_USE_TLS;
	}
#endif

	اگر(bind_address){
		نویسه service[10];
		ساختار addrinfo hints;
		ساختار addrinfo *ainfo, *rp;

		snprintf(service, اندازه(service), "%d", port);

		memset(&hints, 0, اندازه(ساختار addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = AI_PASSIVE;
		hints.ai_socktype = SOCK_STREAM;

		صحیح rc = getaddrinfo(bind_address, service, &hints, &ainfo);
		اگر(rc){
			log__printf(پوچ, MOSQ_LOG_ERR, "Unable to start http api listener (%s).", strerror(errno));
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_کلید);
			بازگشت MOSQ_ERR_ERRNO;
		}

		برای(rp = ainfo; rp; rp = rp->ai_next){
			اگر(rp->ai_family == AF_INET || rp->ai_family == AF_INET6){
				بشکن;
			}
		}
		اگر(!rp){
			log__printf(پوچ, MOSQ_LOG_ERR, "Unable to start http api listener, could not find address.");
			freeaddrinfo(ainfo);
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_کلید);

			بازگشت MOSQ_ERR_INVAL;
		}

		اگر(rp->ai_family == AF_INET6){
			flags |= MHD_USE_IPv6;
		}
		اگر(x509_cert && x509_کلید){
			listener->mhd = MHD_start_daemon(flags, port, پوچ, پوچ, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_کلید, x509_کلید,
					MHD_OPTION_END);
		}وگرنه{
			listener->mhd = MHD_start_daemon(flags, port, پوچ, پوچ, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_END);
		}
		freeaddrinfo(ainfo);
	}وگرنه{
		اگر(x509_cert && x509_کلید){
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_پشته, port, پوچ, پوچ, &http_api_handler, listener,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_کلید, x509_کلید,
					MHD_OPTION_END);
		}وگرنه{
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_پشته, port, پوچ, پوچ, &http_api_handler, listener,
					MHD_OPTION_END);
		}
	}

	mosquitto_FREE(x509_cert);
	mosquitto_FREE(x509_کلید);

	اگر(listener->mhd){
		log__printf(پوچ, MOSQ_LOG_INFO, "Opening http api listen socket on port %d.", port);
		اگر(listener->http_dir){
			log__printf(پوچ, MOSQ_LOG_INFO, "Using http_dir %s", listener->http_dir);
		}
		بازگشت MOSQ_ERR_SUCCESS;
	}وگرنه{
		بازگشت MOSQ_ERR_UNKNOWN;
	}
}


تهی http_api__stop(ساختار mosquitto__listener *listener)
{
	MHD_stop_daemon(listener->mhd);
	listener->mhd = پوچ;
}

#endif
