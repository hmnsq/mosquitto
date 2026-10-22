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

#অন্তর্ভাব "config.h"

#ifdef WITH_HTTP_API

#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <errno.h>
#অন্তর্ভাব <limits.h>
#অন্তর্ভাব <microhttpd.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <sys/stat.h>
#ifndef WIN32
#  include <netdb.h>
#endif

#অন্তর্ভাব "json_help.h"
#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto/mqtt_protocol.h"
#অন্তর্ভাব "sys_tree.h"

#ifndef HTTP_API_DIR
#  define HTTP_API_DIR ""
#endif

কাঠামো metric {
	int64_t current;
	int64_t পরবর্তী;
	ধ্রুবক অক্ষর *topic, *topic_alias;
	bool is_max;
};

time_t broker_uptime(শূন্য);
কাঠামো MHD_Daemon *mhd = ফাঁকা;

#ifdef WITH_SYS_বৃক্ষ
বাহ্যিক কাঠামো metric metrics[mosq_metric_max];
#endif

#define HTTP_RESPONSE_BUFLEN 10000

#ifdef WIN32
#define DIR_SEP '\\'
#define PATH_MAX MAX_PATH
#অন্যথা
#define DIR_SEP '/'
#endif

#ifndef S_ISREG
#define S_ISREG(mode) (((mode)&S_IFMT) == S_IFREG)
#endif


স্থির অক্ষর *http__canonical_filename(
		ধ্রুবক অক্ষর *url,
		ধ্রুবক অক্ষর *http_dir,
		পূর্ণসংখ্যা *error_code)
{
	size_t urllen, slen;
	অক্ষর *filename, *filename_canonical;

	urllen = strlen(url);
	যদি(url[urllen-1] == '/'){
		slen = strlen(http_dir) + urllen + strlen("/index.html") + 2;
	}অন্যথা{
		slen = strlen(http_dir) + urllen + 2;
	}
	filename = mosquitto_malloc(slen);
	যদি(!filename){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		ফেরত ফাঁকা;
	}
	যদি(((অক্ষর *)url)[urllen-1] == '/'){
		snprintf(filename, slen, "%s%c%sindex.html", http_dir, DIR_SEP, url);
	}অন্যথা{
		snprintf(filename, slen, "%s%c%s", http_dir, DIR_SEP, url);
	}


	/* Get canonical path and check it is within our http_dir */
	filename_canonical = mosquitto_calloc(1, PATH_MAX);
	যদি(!filename_canonical){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		ফেরত ফাঁকা;
	}
#ifdef WIN32
	অক্ষর *resolved = _fullpath(filename_canonical, filename, PATH_MAX);
	mosquitto_FREE(filename);
	যদি(!resolved){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		mosquitto_FREE(filename_canonical);
		ফেরত ফাঁকা;
	}
#অন্যথা
	অক্ষর *resolved = realpath(filename, filename_canonical);
	mosquitto_FREE(filename);
	যদি(!resolved){
		যদি(errno == EACCES){
			*error_code = MHD_HTTP_FORBIDDEN;
		}অন্যথা যদি(errno == EINVAL || errno == EIO || errno == ELOOP){
			*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		}অন্যথা যদি(errno == ENAMETOOLONG){
			*error_code = MHD_HTTP_URI_TOO_LONG;
		}অন্যথা যদি(errno == ENOENT || errno == ENOTDIR){
			*error_code = MHD_HTTP_NOT_FOUND;
		}
		mosquitto_FREE(filename_canonical);
		ফেরত ফাঁকা;
	}
#endif
	যদি(strncmp(http_dir, filename_canonical, strlen(http_dir))){
		/* Requested file isn't within http_dir, deny access because it's not found. */
		mosquitto_FREE(filename_canonical);
		*error_code = MHD_HTTP_NOT_FOUND;
		ফেরত ফাঁকা;
	}

	ফেরত filename_canonical;
}

স্থির গণনা MHD_ফলাফল http_api__send_error_response(কাঠামো MHD_Connection *connection, ধ্রুবক অক্ষর *error_str, unsigned পূর্ণসংখ্যা error_code)
{
	কাঠামো MHD_Response *response = MHD_create_response_from_বাফার(strlen(error_str), (শূন্য *)error_str, MHD_RESPMEM_MUST_COPY);
	গণনা MHD_ফলাফল ret = MHD_queue_response(connection, error_code, response);
	MHD_destroy_response(response);
	ফেরত ret;
}

স্থির গণনা MHD_ফলাফল http_api__send_response_with_headers(কাঠামো MHD_Connection *connection, ধ্রুবক অক্ষর *buf)
{
	গণনা MHD_ফলাফল ret;
	কাঠামো MHD_Response *response = MHD_create_response_from_বাফার(strlen(buf), (শূন্য *)buf, MHD_RESPMEM_MUST_COPY);

	ret = MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
	যদি(ret != MHD_YES){
		MHD_destroy_response(response);
		ফেরত ret;
	}
	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	ফেরত ret;
}

স্থির গণনা MHD_ফলাফল http_api__process_version(কাঠামো MHD_Connection *connection)
{
	ফেরত http_api__send_response_with_headers(connection, VERSION);
}

স্থির গণনা MHD_ফলাফল http_api__process_listeners(কাঠামো MHD_Connection *connection)
{
	অক্ষর *buf;
	গণনা MHD_ফলাফল ret;

	cJSON *j_বৃক্ষ = cJSON_CreateObject();
	যদি(!j_বৃক্ষ){
		ফেরত http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	cJSON *j_listeners = cJSON_AddArrayToObject(j_বৃক্ষ, "listeners");
	যদি(!j_listeners){
		cJSON_Delete(j_বৃক্ষ);
		ফেরত http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	জন্য(পূর্ণসংখ্যা i=0; i<db.config->listener_গুনতি; i++){
		cJSON *j_listener = cJSON_CreateObject();
		যদি(!j_listener){
			cJSON_Delete(j_বৃক্ষ);
			ফেরত http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
		}
		cJSON_AddItemToArray(j_listeners, j_listener);

		কাঠামো mosquitto__listener *listener = &db.config->listeners[i];
#ifdef WITH_UNIX_SOCKETS
		যদি(listener->unix_socket_path){
			cJSON_AddStringToObject(j_listener, "path", listener->unix_socket_path);
		}অন্যথা
#endif
		{
			cJSON_AddIntToObject(j_listener, "port", listener->port);
		}

		নির্বাচন(listener->protocol){
			ক্ষেত্র mp_mqtt:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt");
				বিরতি;
			ক্ষেত্র mp_mqttsn:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt-sn");
				বিরতি;
			ক্ষেত্র mp_websockets:
				cJSON_AddStringToObject(j_listener, "protocol", "websockets");
				বিরতি;
			ক্ষেত্র mp_http_api:
				cJSON_AddStringToObject(j_listener, "protocol", "httpapi");
				বিরতি;
		}

#ifdef WITH_TLS
		cJSON_AddBoolToObject(j_listener, "tls", listener->certfile && listener->keyfile);
		cJSON_AddBoolToObject(j_listener, "mtls", listener->require_certificate);
#অন্যথা
		cJSON_AddBoolToObject(j_listener, "tls", false);
		cJSON_AddBoolToObject(j_listener, "mtls", false);
#endif
		যদি(listener->security_options->allow_anonymous == -1){
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", db.config->security_options.allow_anonymous);
		}অন্যথা{
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", listener->security_options->allow_anonymous);
		}
	}
	buf = cJSON_Print(j_বৃক্ষ);
	cJSON_Delete(j_বৃক্ষ);
	যদি(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		মুক্ত(buf);
	}অন্যথা{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	ফেরত ret;
}


স্থির গণনা MHD_ফলাফল http_api__process_systree(কাঠামো MHD_Connection *connection)
{
	গণনা MHD_ফলাফল ret;
#ifdef WITH_SYS_বৃক্ষ
	অক্ষর *buf;

	cJSON *j_বৃক্ষ = cJSON_CreateObject();
	জন্য(পূর্ণসংখ্যা i=0; i<mosq_metric_max; i++){
		যদি(metrics[i].topic){
			cJSON_AddIntToObject(j_বৃক্ষ, metrics[i].topic, metrics[i].current);
		}
	}
	cJSON_AddIntToObject(j_বৃক্ষ, "$SYS/broker/uptime", broker_uptime());

	buf = cJSON_Print(j_বৃক্ষ);
	cJSON_Delete(j_বৃক্ষ);

	যদি(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		মুক্ত(buf);
	}অন্যথা{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}
#অন্যথা
	ret = http_api__send_error_response(connection, "Not found.\n", 404);
#endif

	ফেরত ret;
}


স্থির ssize_t http_file_read_cb(শূন্য *cls, uint64_t pos, অক্ষর *buf, size_t max)
{
	FILE *fptr = cls;
	যদি(fseek(fptr, (long )pos, SEEK_SET) < 0){
		ফেরত -1;
	}
	ফেরত (ssize_t )fread(buf, 1, max, fptr);
}


স্থির শূন্য http_file_free_cb(শূন্য *cls)
{
	FILE *fptr = cls;
	fclose(fptr);
}


স্থির গণনা MHD_ফলাফল http_api__process_file(কাঠামো mosquitto__listener *listener, কাঠামো MHD_Connection *connection, ধ্রুবক অক্ষর *url)
{
	পূর্ণসংখ্যা error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
	অক্ষর *canonical_filename;

	যদি(!listener || !listener->http_dir){
		http_api__send_error_response(connection, "Not found.\n", 404);
		ফেরত MHD_YES;
	}

	canonical_filename = http__canonical_filename(url, listener->http_dir, &error_code);
	যদি(!canonical_filename){
		http_api__send_error_response(connection, "Not found.\n", 404);
		ফেরত MHD_YES;
	}

	FILE *fptr = fopen(canonical_filename, "rb");
	mosquitto_FREE(canonical_filename);
	যদি(!fptr){
		http_api__send_error_response(connection, "Not found.\n", 404);
		ফেরত MHD_YES;
	}

	কাঠামো stat statbuf;
	যদি(fstat(fileno(fptr), &statbuf)){
		fclose(fptr);
		http_api__send_error_response(connection, "Internal server error.\n", 500);
		ফেরত MHD_YES;
	}

	যদি(!S_ISREG(statbuf.st_mode)){
		fclose(fptr);
		http_api__send_error_response(connection, "Not found.\n", 404);
		ফেরত MHD_YES;
	}
	uint64_t flen = (uint64_t )statbuf.st_আকার;

	/* Using MHD_create_response_from_fd would be easier here, but is less portable */
	কাঠামো MHD_Response *response = MHD_create_response_from_callback(
			flen,
			65536,
			http_file_read_cb,
			fptr,
			http_file_free_cb);

	গণনা MHD_ফলাফল ret = MHD_queue_response(connection, 200, response);
	MHD_destroy_response(response);
	ফেরত ret;
}


স্থির গণনা MHD_ফলাফল http_api__process_api(কাঠামো MHD_Connection *connection, ধ্রুবক অক্ষর *url)
{
	যদি(strcmp(url, "/api/v1/systree") == 0){
		ফেরত http_api__process_systree(connection);
	}অন্যথা যদি(strcmp(url, "/api/v1/listeners") == 0){
		ফেরত http_api__process_listeners(connection);
	}অন্যথা যদি(strcmp(url, "/api/v1/version") == 0){
		ফেরত http_api__process_version(connection);
	}অন্যথা{
		ফেরত http_api__send_error_response(connection, "Not found.\n", 404);
	}
}


স্থির পূর্ণসংখ্যা check_access(কাঠামো mosquitto__listener *listener, কাঠামো MHD_Connection *connection, ধ্রুবক অক্ষর *url)
{
	কাঠামো mosquitto context = {0};
	পূর্ণসংখ্যা auth_rc, acl_rc = MOSQ_ERR_SUCCESS;

	context.listener = listener;

	context.id = (অক্ষর *)"http-api";
	context.username = MHD_basic_auth_get_username_password(connection, &context.password);

	/* Authentication */
	auth_rc = mosquitto_basic_auth(&context);
	যদি(auth_rc == MOSQ_ERR_SUCCESS){
		acl_rc = mosquitto_acl_check(&context, url, 0, ফাঁকা, 0, false, ফাঁকা, MOSQ_ACL_READ);
	}
	MHD_free(context.username);
	MHD_free(context.password);

	যদি(auth_rc || acl_rc){
		ধ্রুবক অক্ষর *buf = "Not authorised\n";
		কাঠামো MHD_Response *response = MHD_create_response_from_বাফার(strlen(buf), (শূন্য *)buf, MHD_RESPMEM_MUST_COPY);
		MHD_queue_basic_auth_fail_response(connection, "Mosquitto API", response);
		MHD_destroy_response(response);

		ফেরত MOSQ_ERR_AUTH;
	}

	ফেরত MOSQ_ERR_SUCCESS;
}

স্থির গণনা MHD_ফলাফল http_api_handler(শূন্য *cls, কাঠামো MHD_Connection
		*connection, ধ্রুবক অক্ষর *url, ধ্রুবক অক্ষর *method, ধ্রুবক অক্ষর *version,
		ধ্রুবক অক্ষর *upload_তথ্য, size_t *upload_তথ্য_আকার, শূন্য **con_cls)
{
	UNUSED(version);
	UNUSED(upload_তথ্য);
	UNUSED(upload_তথ্য_আকার);
	UNUSED(con_cls);

	কাঠামো mosquitto__listener *listener = cls;

	যদি(strcmp(method, "GET") != 0){
		অক্ষর *buf = "Invalid HTTP Method\n";
		কাঠামো MHD_Response *response = MHD_create_response_from_বাফার(strlen(buf), (শূন্য *)buf, MHD_RESPMEM_MUST_COPY);
		গণনা MHD_ফলাফল ret = MHD_queue_response(connection, MHD_HTTP_METHOD_NOT_ALLOWED, response);
		MHD_destroy_response(response);
		ফেরত ret;
	}

	যদি(check_access(listener, connection, url) != MOSQ_ERR_SUCCESS){
		ফেরত MHD_YES;
	}

	যদি(!strncasecmp(url, "/api/", strlen("/api/"))){
		ফেরত http_api__process_api(connection, url);
	}অন্যথা{
		ফেরত http_api__process_file(listener, connection, url);
	}
}


পূর্ণসংখ্যা http_api__start_local(কাঠামো mosquitto__listener *listener)
{
	listener->security_options = mosquitto_calloc(1, আকার(কাঠামো mosquitto__security_options));
	যদি(listener->security_options == ফাঁকা){
		ফেরত MOSQ_ERR_NOMEM;
	}

	listener->host = mosquitto_strdup("127.0.0.1");
	যদি(!listener->host){
		mosquitto_FREE(listener->security_options);
		ফেরত MOSQ_ERR_NOMEM;
	}
	listener->port = 9883;

	যদি(db.config->security_options.allow_anonymous == -1){
		listener->security_options->allow_anonymous = true;
	}অন্যথা{
		listener->security_options->allow_anonymous = db.config->security_options.allow_anonymous;
	}

	যদি(listener->http_dir == ফাঁকা && strlen(HTTP_API_DIR) > 0){
#ifdef WIN32
		অক্ষর *http_dir_canonical = _fullpath(ফাঁকা, HTTP_API_DIR, 0);
#অন্যথা
		অক্ষর *http_dir_canonical = realpath(HTTP_API_DIR, ফাঁকা);
#endif
		যদি(!http_dir_canonical){
			ফেরত MOSQ_ERR_NOMEM;
		}
		mosquitto_FREE(listener->http_dir);
		listener->http_dir = mosquitto_strdup(http_dir_canonical);
		mosquitto_FREE(http_dir_canonical);
	}

	ফেরত http_api__start(listener);
}


পূর্ণসংখ্যা http_api__start(কাঠামো mosquitto__listener *listener)
{
	unsigned পূর্ণসংখ্যা flags = MHD_USE_AUTO_INTERNAL_THREAD;
	ধ্রুবক অক্ষর *bind_address;
	uint16_t port = 9883;
	অক্ষর *x509_cert = ফাঁকা;
	অক্ষর *x509_চাবি = ফাঁকা;

	যদি(!listener->security_options){
		listener->security_options = mosquitto_calloc(1, আকার(কাঠামো mosquitto__security_options));
		যদি(listener->security_options == ফাঁকা){
			ফেরত MOSQ_ERR_NOMEM;
		}
	}
	listener->protocol = mp_http_api;

	bind_address = listener->host;
	port = listener->port;

#ifdef WITH_TLS
	যদি(listener->certfile && listener->keyfile){
		যদি(mosquitto_read_file(listener->certfile, false, &x509_cert, ফাঁকা)){
			log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
			ফেরত MOSQ_ERR_INVAL;
		}
		যদি(mosquitto_read_file(listener->keyfile, false, &x509_চাবি, ফাঁকা)){
			log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			mosquitto_FREE(x509_cert);
			ফেরত MOSQ_ERR_INVAL;
		}
		flags |= MHD_USE_TLS;
	}
#endif

	যদি(bind_address){
		অক্ষর service[10];
		কাঠামো addrinfo hints;
		কাঠামো addrinfo *ainfo, *rp;

		snprintf(service, আকার(service), "%d", port);

		memset(&hints, 0, আকার(কাঠামো addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = AI_PASSIVE;
		hints.ai_socktype = SOCK_STREAM;

		পূর্ণসংখ্যা rc = getaddrinfo(bind_address, service, &hints, &ainfo);
		যদি(rc){
			log__printf(ফাঁকা, MOSQ_LOG_ERR, "Unable to start http api listener (%s).", strerror(errno));
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_চাবি);
			ফেরত MOSQ_ERR_ERRNO;
		}

		জন্য(rp = ainfo; rp; rp = rp->ai_পরবর্তী){
			যদি(rp->ai_family == AF_INET || rp->ai_family == AF_INET6){
				বিরতি;
			}
		}
		যদি(!rp){
			log__printf(ফাঁকা, MOSQ_LOG_ERR, "Unable to start http api listener, could not find address.");
			freeaddrinfo(ainfo);
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_চাবি);

			ফেরত MOSQ_ERR_INVAL;
		}

		যদি(rp->ai_family == AF_INET6){
			flags |= MHD_USE_IPv6;
		}
		যদি(x509_cert && x509_চাবি){
			listener->mhd = MHD_start_daemon(flags, port, ফাঁকা, ফাঁকা, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_চাবি, x509_চাবি,
					MHD_OPTION_END);
		}অন্যথা{
			listener->mhd = MHD_start_daemon(flags, port, ফাঁকা, ফাঁকা, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_END);
		}
		freeaddrinfo(ainfo);
	}অন্যথা{
		যদি(x509_cert && x509_চাবি){
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_স্তূপ, port, ফাঁকা, ফাঁকা, &http_api_handler, listener,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_চাবি, x509_চাবি,
					MHD_OPTION_END);
		}অন্যথা{
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_স্তূপ, port, ফাঁকা, ফাঁকা, &http_api_handler, listener,
					MHD_OPTION_END);
		}
	}

	mosquitto_FREE(x509_cert);
	mosquitto_FREE(x509_চাবি);

	যদি(listener->mhd){
		log__printf(ফাঁকা, MOSQ_LOG_INFO, "Opening http api listen socket on port %d.", port);
		যদি(listener->http_dir){
			log__printf(ফাঁকা, MOSQ_LOG_INFO, "Using http_dir %s", listener->http_dir);
		}
		ফেরত MOSQ_ERR_SUCCESS;
	}অন্যথা{
		ফেরত MOSQ_ERR_UNKNOWN;
	}
}


শূন্য http_api__stop(কাঠামো mosquitto__listener *listener)
{
	MHD_stop_daemon(listener->mhd);
	listener->mhd = ফাঁকা;
}

#endif
