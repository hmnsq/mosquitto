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

#অন্তর্ভাৱ "config.h"

#ifdef WITH_HTTP_API

#অন্তর্ভাৱ <assert.h>
#অন্তর্ভাৱ <errno.h>
#অন্তর্ভাৱ <limits.h>
#অন্তর্ভাৱ <microhttpd.h>
#অন্তর্ভাৱ <string.h>
#অন্তর্ভাৱ <sys/stat.h>
#ifndef WIN32
#  include <netdb.h>
#endif

#অন্তর্ভাৱ "json_help.h"
#অন্তর্ভাৱ "mosquitto_broker_internal.h"
#অন্তর্ভাৱ "mosquitto/mqtt_protocol.h"
#অন্তর্ভাৱ "sys_tree.h"

#ifndef HTTP_API_DIR
#  define HTTP_API_DIR ""
#endif

সংরচনা metric {
	int64_t current;
	int64_t next;
	ধ্রুৱ ৱর্ণ *topic, *topic_alias;
	bool is_max;
};

time_t broker_uptime(শূন্য);
সংরচনা MHD_Daemon *mhd = রিক্ত;

#ifdef WITH_SYS_বৃক্ষ
বাহ্য সংরচনা metric metrics[mosq_metric_max];
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


স্থির ৱর্ণ *http__canonical_filename(
		ধ্রুৱ ৱর্ণ *url,
		ধ্রুৱ ৱর্ণ *http_dir,
		পূর্ণাঙ্ক *error_code)
{
	size_t urllen, slen;
	ৱর্ণ *filename, *filename_canonical;

	urllen = strlen(url);
	যদি(url[urllen-1] == '/'){
		slen = strlen(http_dir) + urllen + strlen("/index.html") + 2;
	}অন্যথা{
		slen = strlen(http_dir) + urllen + 2;
	}
	filename = mosquitto_malloc(slen);
	যদি(!filename){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		ঘূৰাই রিক্ত;
	}
	যদি(((ৱর্ণ *)url)[urllen-1] == '/'){
		snprintf(filename, slen, "%s%c%sindex.html", http_dir, DIR_SEP, url);
	}অন্যথা{
		snprintf(filename, slen, "%s%c%s", http_dir, DIR_SEP, url);
	}


	/* Get canonical path and check it is within our http_dir */
	filename_canonical = mosquitto_calloc(1, PATH_MAX);
	যদি(!filename_canonical){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		ঘূৰাই রিক্ত;
	}
#ifdef WIN32
	ৱর্ণ *resolved = _fullpath(filename_canonical, filename, PATH_MAX);
	mosquitto_FREE(filename);
	যদি(!resolved){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		mosquitto_FREE(filename_canonical);
		ঘূৰাই রিক্ত;
	}
#অন্যথা
	ৱর্ণ *resolved = realpath(filename, filename_canonical);
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
		ঘূৰাই রিক্ত;
	}
#endif
	যদি(strncmp(http_dir, filename_canonical, strlen(http_dir))){
		/* Requested file isn't within http_dir, deny access because it's not found. */
		mosquitto_FREE(filename_canonical);
		*error_code = MHD_HTTP_NOT_FOUND;
		ঘূৰাই রিক্ত;
	}

	ঘূৰাই filename_canonical;
}

স্থির গণনা MHD_Result http_api__send_error_response(সংরচনা MHD_Connection *connection, ধ্রুৱ ৱর্ণ *error_str, unsigned পূর্ণাঙ্ক error_code)
{
	সংরচনা MHD_Response *response = MHD_create_response_from_buffer(strlen(error_str), (শূন্য *)error_str, MHD_RESPMEM_MUST_COPY);
	গণনা MHD_Result ret = MHD_queue_response(connection, error_code, response);
	MHD_destroy_response(response);
	ঘূৰাই ret;
}

স্থির গণনা MHD_Result http_api__send_response_with_headers(সংরচনা MHD_Connection *connection, ধ্রুৱ ৱর্ণ *buf)
{
	গণনা MHD_Result ret;
	সংরচনা MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (শূন্য *)buf, MHD_RESPMEM_MUST_COPY);

	ret = MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
	যদি(ret != MHD_YES){
		MHD_destroy_response(response);
		ঘূৰাই ret;
	}
	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	ঘূৰাই ret;
}

স্থির গণনা MHD_Result http_api__process_version(সংরচনা MHD_Connection *connection)
{
	ঘূৰাই http_api__send_response_with_headers(connection, VERSION);
}

স্থির গণনা MHD_Result http_api__process_listeners(সংরচনা MHD_Connection *connection)
{
	ৱর্ণ *buf;
	গণনা MHD_Result ret;

	cJSON *j_বৃক্ষ = cJSON_CreateObject();
	যদি(!j_বৃক্ষ){
		ঘূৰাই http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	cJSON *j_listeners = cJSON_AddArrayToObject(j_বৃক্ষ, "listeners");
	যদি(!j_listeners){
		cJSON_Delete(j_বৃক্ষ);
		ঘূৰাই http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	বাবে(পূর্ণাঙ্ক i=0; i<db.config->listener_গুনতি; i++){
		cJSON *j_listener = cJSON_CreateObject();
		যদি(!j_listener){
			cJSON_Delete(j_বৃক্ষ);
			ঘূৰাই http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
		}
		cJSON_AddItemToArray(j_listeners, j_listener);

		সংরচনা mosquitto__listener *listener = &db.config->listeners[i];
#ifdef WITH_UNIX_SOCKETS
		যদি(listener->unix_socket_path){
			cJSON_AddStringToObject(j_listener, "path", listener->unix_socket_path);
		}অন্যথা
#endif
		{
			cJSON_AddIntToObject(j_listener, "port", listener->port);
		}

		ৱিকল্প(listener->protocol){
			পক্ষ mp_mqtt:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt");
				ভঙ্গ;
			পক্ষ mp_mqttsn:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt-sn");
				ভঙ্গ;
			পক্ষ mp_websockets:
				cJSON_AddStringToObject(j_listener, "protocol", "websockets");
				ভঙ্গ;
			পক্ষ mp_http_api:
				cJSON_AddStringToObject(j_listener, "protocol", "httpapi");
				ভঙ্গ;
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

	ঘূৰাই ret;
}


স্থির গণনা MHD_Result http_api__process_systree(সংরচনা MHD_Connection *connection)
{
	গণনা MHD_Result ret;
#ifdef WITH_SYS_বৃক্ষ
	ৱর্ণ *buf;

	cJSON *j_বৃক্ষ = cJSON_CreateObject();
	বাবে(পূর্ণাঙ্ক i=0; i<mosq_metric_max; i++){
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

	ঘূৰাই ret;
}


স্থির ssize_t http_file_read_cb(শূন্য *cls, uint64_t pos, ৱর্ণ *buf, size_t max)
{
	FILE *fptr = cls;
	যদি(fseek(fptr, (long )pos, SEEK_SET) < 0){
		ঘূৰাই -1;
	}
	ঘূৰাই (ssize_t )fread(buf, 1, max, fptr);
}


স্থির শূন্য http_file_free_cb(শূন্য *cls)
{
	FILE *fptr = cls;
	fclose(fptr);
}


স্থির গণনা MHD_Result http_api__process_file(সংরচনা mosquitto__listener *listener, সংরচনা MHD_Connection *connection, ধ্রুৱ ৱর্ণ *url)
{
	পূর্ণাঙ্ক error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
	ৱর্ণ *canonical_filename;

	যদি(!listener || !listener->http_dir){
		http_api__send_error_response(connection, "Not found.\n", 404);
		ঘূৰাই MHD_YES;
	}

	canonical_filename = http__canonical_filename(url, listener->http_dir, &error_code);
	যদি(!canonical_filename){
		http_api__send_error_response(connection, "Not found.\n", 404);
		ঘূৰাই MHD_YES;
	}

	FILE *fptr = fopen(canonical_filename, "rb");
	mosquitto_FREE(canonical_filename);
	যদি(!fptr){
		http_api__send_error_response(connection, "Not found.\n", 404);
		ঘূৰাই MHD_YES;
	}

	সংরচনা stat statbuf;
	যদি(fstat(fileno(fptr), &statbuf)){
		fclose(fptr);
		http_api__send_error_response(connection, "Internal server error.\n", 500);
		ঘূৰাই MHD_YES;
	}

	যদি(!S_ISREG(statbuf.st_mode)){
		fclose(fptr);
		http_api__send_error_response(connection, "Not found.\n", 404);
		ঘূৰাই MHD_YES;
	}
	uint64_t flen = (uint64_t )statbuf.st_size;

	/* Using MHD_create_response_from_fd would be easier here, but is less portable */
	সংরচনা MHD_Response *response = MHD_create_response_from_callback(
			flen,
			65536,
			http_file_read_cb,
			fptr,
			http_file_free_cb);

	গণনা MHD_Result ret = MHD_queue_response(connection, 200, response);
	MHD_destroy_response(response);
	ঘূৰাই ret;
}


স্থির গণনা MHD_Result http_api__process_api(সংরচনা MHD_Connection *connection, ধ্রুৱ ৱর্ণ *url)
{
	যদি(strcmp(url, "/api/v1/systree") == 0){
		ঘূৰাই http_api__process_systree(connection);
	}অন্যথা যদি(strcmp(url, "/api/v1/listeners") == 0){
		ঘূৰাই http_api__process_listeners(connection);
	}অন্যথা যদি(strcmp(url, "/api/v1/version") == 0){
		ঘূৰাই http_api__process_version(connection);
	}অন্যথা{
		ঘূৰাই http_api__send_error_response(connection, "Not found.\n", 404);
	}
}


স্থির পূর্ণাঙ্ক check_access(সংরচনা mosquitto__listener *listener, সংরচনা MHD_Connection *connection, ধ্রুৱ ৱর্ণ *url)
{
	সংরচনা mosquitto context = {0};
	পূর্ণাঙ্ক auth_rc, acl_rc = MOSQ_ERR_SUCCESS;

	context.listener = listener;

	context.id = (ৱর্ণ *)"http-api";
	context.username = MHD_basic_auth_get_username_password(connection, &context.password);

	/* Authentication */
	auth_rc = mosquitto_basic_auth(&context);
	যদি(auth_rc == MOSQ_ERR_SUCCESS){
		acl_rc = mosquitto_acl_check(&context, url, 0, রিক্ত, 0, false, রিক্ত, MOSQ_ACL_READ);
	}
	MHD_free(context.username);
	MHD_free(context.password);

	যদি(auth_rc || acl_rc){
		ধ্রুৱ ৱর্ণ *buf = "Not authorised\n";
		সংরচনা MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (শূন্য *)buf, MHD_RESPMEM_MUST_COPY);
		MHD_queue_basic_auth_fail_response(connection, "Mosquitto API", response);
		MHD_destroy_response(response);

		ঘূৰাই MOSQ_ERR_AUTH;
	}

	ঘূৰাই MOSQ_ERR_SUCCESS;
}

স্থির গণনা MHD_Result http_api_handler(শূন্য *cls, সংরচনা MHD_Connection
		*connection, ধ্রুৱ ৱর্ণ *url, ধ্রুৱ ৱর্ণ *method, ধ্রুৱ ৱর্ণ *version,
		ধ্রুৱ ৱর্ণ *upload_data, size_t *upload_data_size, শূন্য **con_cls)
{
	UNUSED(version);
	UNUSED(upload_data);
	UNUSED(upload_data_size);
	UNUSED(con_cls);

	সংরচনা mosquitto__listener *listener = cls;

	যদি(strcmp(method, "GET") != 0){
		ৱর্ণ *buf = "Invalid HTTP Method\n";
		সংরচনা MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (শূন্য *)buf, MHD_RESPMEM_MUST_COPY);
		গণনা MHD_Result ret = MHD_queue_response(connection, MHD_HTTP_METHOD_NOT_ALLOWED, response);
		MHD_destroy_response(response);
		ঘূৰাই ret;
	}

	যদি(check_access(listener, connection, url) != MOSQ_ERR_SUCCESS){
		ঘূৰাই MHD_YES;
	}

	যদি(!strncasecmp(url, "/api/", strlen("/api/"))){
		ঘূৰাই http_api__process_api(connection, url);
	}অন্যথা{
		ঘূৰাই http_api__process_file(listener, connection, url);
	}
}


পূর্ণাঙ্ক http_api__start_local(সংরচনা mosquitto__listener *listener)
{
	listener->security_options = mosquitto_calloc(1, পরিমাণ(সংরচনা mosquitto__security_options));
	যদি(listener->security_options == রিক্ত){
		ঘূৰাই MOSQ_ERR_NOMEM;
	}

	listener->host = mosquitto_strdup("127.0.0.1");
	যদি(!listener->host){
		mosquitto_FREE(listener->security_options);
		ঘূৰাই MOSQ_ERR_NOMEM;
	}
	listener->port = 9883;

	যদি(db.config->security_options.allow_anonymous == -1){
		listener->security_options->allow_anonymous = true;
	}অন্যথা{
		listener->security_options->allow_anonymous = db.config->security_options.allow_anonymous;
	}

	যদি(listener->http_dir == রিক্ত && strlen(HTTP_API_DIR) > 0){
#ifdef WIN32
		ৱর্ণ *http_dir_canonical = _fullpath(রিক্ত, HTTP_API_DIR, 0);
#অন্যথা
		ৱর্ণ *http_dir_canonical = realpath(HTTP_API_DIR, রিক্ত);
#endif
		যদি(!http_dir_canonical){
			ঘূৰাই MOSQ_ERR_NOMEM;
		}
		mosquitto_FREE(listener->http_dir);
		listener->http_dir = mosquitto_strdup(http_dir_canonical);
		mosquitto_FREE(http_dir_canonical);
	}

	ঘূৰাই http_api__start(listener);
}


পূর্ণাঙ্ক http_api__start(সংরচনা mosquitto__listener *listener)
{
	unsigned পূর্ণাঙ্ক flags = MHD_USE_AUTO_INTERNAL_THREAD;
	ধ্রুৱ ৱর্ণ *bind_address;
	uint16_t port = 9883;
	ৱর্ণ *x509_cert = রিক্ত;
	ৱর্ণ *x509_চাবি = রিক্ত;

	যদি(!listener->security_options){
		listener->security_options = mosquitto_calloc(1, পরিমাণ(সংরচনা mosquitto__security_options));
		যদি(listener->security_options == রিক্ত){
			ঘূৰাই MOSQ_ERR_NOMEM;
		}
	}
	listener->protocol = mp_http_api;

	bind_address = listener->host;
	port = listener->port;

#ifdef WITH_TLS
	যদি(listener->certfile && listener->keyfile){
		যদি(mosquitto_read_file(listener->certfile, false, &x509_cert, রিক্ত)){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
			ঘূৰাই MOSQ_ERR_INVAL;
		}
		যদি(mosquitto_read_file(listener->keyfile, false, &x509_চাবি, রিক্ত)){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			mosquitto_FREE(x509_cert);
			ঘূৰাই MOSQ_ERR_INVAL;
		}
		flags |= MHD_USE_TLS;
	}
#endif

	যদি(bind_address){
		ৱর্ণ service[10];
		সংরচনা addrinfo hints;
		সংরচনা addrinfo *ainfo, *rp;

		snprintf(service, পরিমাণ(service), "%d", port);

		memset(&hints, 0, পরিমাণ(সংরচনা addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = AI_PASSIVE;
		hints.ai_socktype = SOCK_STREAM;

		পূর্ণাঙ্ক rc = getaddrinfo(bind_address, service, &hints, &ainfo);
		যদি(rc){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Unable to start http api listener (%s).", strerror(errno));
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_চাবি);
			ঘূৰাই MOSQ_ERR_ERRNO;
		}

		বাবে(rp = ainfo; rp; rp = rp->ai_next){
			যদি(rp->ai_family == AF_INET || rp->ai_family == AF_INET6){
				ভঙ্গ;
			}
		}
		যদি(!rp){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Unable to start http api listener, could not find address.");
			freeaddrinfo(ainfo);
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_চাবি);

			ঘূৰাই MOSQ_ERR_INVAL;
		}

		যদি(rp->ai_family == AF_INET6){
			flags |= MHD_USE_IPv6;
		}
		যদি(x509_cert && x509_চাবি){
			listener->mhd = MHD_start_daemon(flags, port, রিক্ত, রিক্ত, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_চাবি, x509_চাবি,
					MHD_OPTION_END);
		}অন্যথা{
			listener->mhd = MHD_start_daemon(flags, port, রিক্ত, রিক্ত, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_END);
		}
		freeaddrinfo(ainfo);
	}অন্যথা{
		যদি(x509_cert && x509_চাবি){
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_স্তূপ, port, রিক্ত, রিক্ত, &http_api_handler, listener,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_চাবি, x509_চাবি,
					MHD_OPTION_END);
		}অন্যথা{
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_স্তূপ, port, রিক্ত, রিক্ত, &http_api_handler, listener,
					MHD_OPTION_END);
		}
	}

	mosquitto_FREE(x509_cert);
	mosquitto_FREE(x509_চাবি);

	যদি(listener->mhd){
		log__printf(রিক্ত, MOSQ_LOG_INFO, "Opening http api listen socket on port %d.", port);
		যদি(listener->http_dir){
			log__printf(রিক্ত, MOSQ_LOG_INFO, "Using http_dir %s", listener->http_dir);
		}
		ঘূৰাই MOSQ_ERR_SUCCESS;
	}অন্যথা{
		ঘূৰাই MOSQ_ERR_UNKNOWN;
	}
}


শূন্য http_api__stop(সংরচনা mosquitto__listener *listener)
{
	MHD_stop_daemon(listener->mhd);
	listener->mhd = রিক্ত;
}

#endif
