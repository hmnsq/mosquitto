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

#ifdef WITH_HTTP_API

#ضمن <assert.h>
#ضمن <errno.h>
#ضمن <limits.h>
#ضمن <microhttpd.h>
#ضمن <string.h>
#ضمن <sys/stat.h>
#ifndef WIN32
#  include <netdb.h>
#endif

#ضمن "json_help.h"
#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "sys_tree.h"

#ifndef HTTP_API_DIR
#  define HTTP_API_DIR ""
#endif

بنية metric {
	int64_t current;
	int64_t next;
	ثابت حرف *topic, *topic_alias;
	bool is_max;
};

time_t broker_uptime(فراغ);
بنية MHD_Daemon *mhd = عدم;

#ifdef WITH_SYS_TREE
خارجي بنية metric metrics[mosq_metric_max];
#endif

#define HTTP_RESPONSE_BUFLEN 10000

#ifdef WIN32
#define DIR_SEP '\\'
#define PATH_MAX MAX_PATH
#ܐܚܪܢܐ
#define DIR_SEP '/'
#endif

#ifndef S_ISREG
#define S_ISREG(mode) (((mode)&S_IFMT) == S_IFREG)
#endif


ساكن حرف *http__canonical_filename(
		ثابت حرف *url,
		ثابت حرف *http_dir,
		صحيح *error_code)
{
	size_t urllen, slen;
	حرف *filename, *filename_canonical;

	urllen = strlen(url);
	ܐܢ(url[urllen-1] == '/'){
		slen = strlen(http_dir) + urllen + strlen("/index.html") + 2;
	}ܐܚܪܢܐ{
		slen = strlen(http_dir) + urllen + 2;
	}
	filename = mosquitto_malloc(slen);
	ܐܢ(!filename){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		ܦܢܐ عدم;
	}
	ܐܢ(((حرف *)url)[urllen-1] == '/'){
		snprintf(filename, slen, "%s%c%sindex.html", http_dir, DIR_SEP, url);
	}ܐܚܪܢܐ{
		snprintf(filename, slen, "%s%c%s", http_dir, DIR_SEP, url);
	}


	/* Get canonical path and check it is within our http_dir */
	filename_canonical = mosquitto_calloc(1, PATH_MAX);
	ܐܢ(!filename_canonical){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		ܦܢܐ عدم;
	}
#ifdef WIN32
	حرف *resolved = _fullpath(filename_canonical, filename, PATH_MAX);
	mosquitto_FREE(filename);
	ܐܢ(!resolved){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		mosquitto_FREE(filename_canonical);
		ܦܢܐ عدم;
	}
#ܐܚܪܢܐ
	حرف *resolved = realpath(filename, filename_canonical);
	mosquitto_FREE(filename);
	ܐܢ(!resolved){
		ܐܢ(errno == EACCES){
			*error_code = MHD_HTTP_FORBIDDEN;
		}ܐܚܪܢܐ ܐܢ(errno == EINVAL || errno == EIO || errno == ELOOP){
			*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		}ܐܚܪܢܐ ܐܢ(errno == ENAMETOOLONG){
			*error_code = MHD_HTTP_URI_TOO_LONG;
		}ܐܚܪܢܐ ܐܢ(errno == ENOENT || errno == ENOTDIR){
			*error_code = MHD_HTTP_NOT_FOUND;
		}
		mosquitto_FREE(filename_canonical);
		ܦܢܐ عدم;
	}
#endif
	ܐܢ(strncmp(http_dir, filename_canonical, strlen(http_dir))){
		/* Requested file isn't within http_dir, deny access because it's not found. */
		mosquitto_FREE(filename_canonical);
		*error_code = MHD_HTTP_NOT_FOUND;
		ܦܢܐ عدم;
	}

	ܦܢܐ filename_canonical;
}

ساكن تعداد MHD_Result http_api__send_error_response(بنية MHD_Connection *connection, ثابت حرف *error_str, unsigned صحيح error_code)
{
	بنية MHD_Response *response = MHD_create_response_from_buffer(strlen(error_str), (فراغ *)error_str, MHD_RESPMEM_MUST_COPY);
	تعداد MHD_Result ret = MHD_queue_response(connection, error_code, response);
	MHD_destroy_response(response);
	ܦܢܐ ret;
}

ساكن تعداد MHD_Result http_api__send_response_with_headers(بنية MHD_Connection *connection, ثابت حرف *buf)
{
	تعداد MHD_Result ret;
	بنية MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (فراغ *)buf, MHD_RESPMEM_MUST_COPY);

	ret = MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
	ܐܢ(ret != MHD_YES){
		MHD_destroy_response(response);
		ܦܢܐ ret;
	}
	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	ܦܢܐ ret;
}

ساكن تعداد MHD_Result http_api__process_version(بنية MHD_Connection *connection)
{
	ܦܢܐ http_api__send_response_with_headers(connection, VERSION);
}

ساكن تعداد MHD_Result http_api__process_listeners(بنية MHD_Connection *connection)
{
	حرف *buf;
	تعداد MHD_Result ret;

	cJSON *j_tree = cJSON_CreateObject();
	ܐܢ(!j_tree){
		ܦܢܐ http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	cJSON *j_listeners = cJSON_AddArrayToObject(j_tree, "listeners");
	ܐܢ(!j_listeners){
		cJSON_Delete(j_tree);
		ܦܢܐ http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	لكل(صحيح i=0; i<db.config->listener_count; i++){
		cJSON *j_listener = cJSON_CreateObject();
		ܐܢ(!j_listener){
			cJSON_Delete(j_tree);
			ܦܢܐ http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
		}
		cJSON_AddItemToArray(j_listeners, j_listener);

		بنية mosquitto__listener *listener = &db.config->listeners[i];
#ifdef WITH_UNIX_SOCKETS
		ܐܢ(listener->unix_socket_path){
			cJSON_AddStringToObject(j_listener, "path", listener->unix_socket_path);
		}ܐܚܪܢܐ
#endif
		{
			cJSON_AddIntToObject(j_listener, "port", listener->port);
		}

		بدل(listener->protocol){
			حالة mp_mqtt:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt");
				اكسر;
			حالة mp_mqttsn:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt-sn");
				اكسر;
			حالة mp_websockets:
				cJSON_AddStringToObject(j_listener, "protocol", "websockets");
				اكسر;
			حالة mp_http_api:
				cJSON_AddStringToObject(j_listener, "protocol", "httpapi");
				اكسر;
		}

#ifdef WITH_TLS
		cJSON_AddBoolToObject(j_listener, "tls", listener->certfile && listener->keyfile);
		cJSON_AddBoolToObject(j_listener, "mtls", listener->require_certificate);
#ܐܚܪܢܐ
		cJSON_AddBoolToObject(j_listener, "tls", false);
		cJSON_AddBoolToObject(j_listener, "mtls", false);
#endif
		ܐܢ(listener->security_options->allow_anonymous == -1){
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", db.config->security_options.allow_anonymous);
		}ܐܚܪܢܐ{
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", listener->security_options->allow_anonymous);
		}
	}
	buf = cJSON_Print(j_tree);
	cJSON_Delete(j_tree);
	ܐܢ(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		حرر(buf);
	}ܐܚܪܢܐ{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	ܦܢܐ ret;
}


ساكن تعداد MHD_Result http_api__process_systree(بنية MHD_Connection *connection)
{
	تعداد MHD_Result ret;
#ifdef WITH_SYS_TREE
	حرف *buf;

	cJSON *j_tree = cJSON_CreateObject();
	لكل(صحيح i=0; i<mosq_metric_max; i++){
		ܐܢ(metrics[i].topic){
			cJSON_AddIntToObject(j_tree, metrics[i].topic, metrics[i].current);
		}
	}
	cJSON_AddIntToObject(j_tree, "$SYS/broker/uptime", broker_uptime());

	buf = cJSON_Print(j_tree);
	cJSON_Delete(j_tree);

	ܐܢ(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		حرر(buf);
	}ܐܚܪܢܐ{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}
#ܐܚܪܢܐ
	ret = http_api__send_error_response(connection, "Not found.\n", 404);
#endif

	ܦܢܐ ret;
}


ساكن ssize_t http_file_read_cb(فراغ *cls, uint64_t pos, حرف *buf, size_t max)
{
	FILE *fptr = cls;
	ܐܢ(fseek(fptr, (long )pos, SEEK_SET) < 0){
		ܦܢܐ -1;
	}
	ܦܢܐ (ssize_t )fread(buf, 1, max, fptr);
}


ساكن فراغ http_file_free_cb(فراغ *cls)
{
	FILE *fptr = cls;
	fclose(fptr);
}


ساكن تعداد MHD_Result http_api__process_file(بنية mosquitto__listener *listener, بنية MHD_Connection *connection, ثابت حرف *url)
{
	صحيح error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
	حرف *canonical_filename;

	ܐܢ(!listener || !listener->http_dir){
		http_api__send_error_response(connection, "Not found.\n", 404);
		ܦܢܐ MHD_YES;
	}

	canonical_filename = http__canonical_filename(url, listener->http_dir, &error_code);
	ܐܢ(!canonical_filename){
		http_api__send_error_response(connection, "Not found.\n", 404);
		ܦܢܐ MHD_YES;
	}

	FILE *fptr = fopen(canonical_filename, "rb");
	mosquitto_FREE(canonical_filename);
	ܐܢ(!fptr){
		http_api__send_error_response(connection, "Not found.\n", 404);
		ܦܢܐ MHD_YES;
	}

	بنية stat statbuf;
	ܐܢ(fstat(fileno(fptr), &statbuf)){
		fclose(fptr);
		http_api__send_error_response(connection, "Internal server error.\n", 500);
		ܦܢܐ MHD_YES;
	}

	ܐܢ(!S_ISREG(statbuf.st_mode)){
		fclose(fptr);
		http_api__send_error_response(connection, "Not found.\n", 404);
		ܦܢܐ MHD_YES;
	}
	uint64_t flen = (uint64_t )statbuf.st_size;

	/* Using MHD_create_response_from_fd would be easier here, but is less portable */
	بنية MHD_Response *response = MHD_create_response_from_callback(
			flen,
			65536,
			http_file_read_cb,
			fptr,
			http_file_free_cb);

	تعداد MHD_Result ret = MHD_queue_response(connection, 200, response);
	MHD_destroy_response(response);
	ܦܢܐ ret;
}


ساكن تعداد MHD_Result http_api__process_api(بنية MHD_Connection *connection, ثابت حرف *url)
{
	ܐܢ(strcmp(url, "/api/v1/systree") == 0){
		ܦܢܐ http_api__process_systree(connection);
	}ܐܚܪܢܐ ܐܢ(strcmp(url, "/api/v1/listeners") == 0){
		ܦܢܐ http_api__process_listeners(connection);
	}ܐܚܪܢܐ ܐܢ(strcmp(url, "/api/v1/version") == 0){
		ܦܢܐ http_api__process_version(connection);
	}ܐܚܪܢܐ{
		ܦܢܐ http_api__send_error_response(connection, "Not found.\n", 404);
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
	ܐܢ(auth_rc == MOSQ_ERR_SUCCESS){
		acl_rc = mosquitto_acl_check(&context, url, 0, عدم, 0, false, عدم, MOSQ_ACL_READ);
	}
	MHD_free(context.username);
	MHD_free(context.password);

	ܐܢ(auth_rc || acl_rc){
		ثابت حرف *buf = "Not authorised\n";
		بنية MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (فراغ *)buf, MHD_RESPMEM_MUST_COPY);
		MHD_queue_basic_auth_fail_response(connection, "Mosquitto API", response);
		MHD_destroy_response(response);

		ܦܢܐ MOSQ_ERR_AUTH;
	}

	ܦܢܐ MOSQ_ERR_SUCCESS;
}

ساكن تعداد MHD_Result http_api_handler(فراغ *cls, بنية MHD_Connection
		*connection, ثابت حرف *url, ثابت حرف *method, ثابت حرف *version,
		ثابت حرف *upload_data, size_t *upload_data_size, فراغ **con_cls)
{
	UNUSED(version);
	UNUSED(upload_data);
	UNUSED(upload_data_size);
	UNUSED(con_cls);

	بنية mosquitto__listener *listener = cls;

	ܐܢ(strcmp(method, "GET") != 0){
		حرف *buf = "Invalid HTTP Method\n";
		بنية MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (فراغ *)buf, MHD_RESPMEM_MUST_COPY);
		تعداد MHD_Result ret = MHD_queue_response(connection, MHD_HTTP_METHOD_NOT_ALLOWED, response);
		MHD_destroy_response(response);
		ܦܢܐ ret;
	}

	ܐܢ(check_access(listener, connection, url) != MOSQ_ERR_SUCCESS){
		ܦܢܐ MHD_YES;
	}

	ܐܢ(!strncasecmp(url, "/api/", strlen("/api/"))){
		ܦܢܐ http_api__process_api(connection, url);
	}ܐܚܪܢܐ{
		ܦܢܐ http_api__process_file(listener, connection, url);
	}
}


صحيح http_api__start_local(بنية mosquitto__listener *listener)
{
	listener->security_options = mosquitto_calloc(1, حجم(بنية mosquitto__security_options));
	ܐܢ(listener->security_options == عدم){
		ܦܢܐ MOSQ_ERR_NOMEM;
	}

	listener->host = mosquitto_strdup("127.0.0.1");
	ܐܢ(!listener->host){
		mosquitto_FREE(listener->security_options);
		ܦܢܐ MOSQ_ERR_NOMEM;
	}
	listener->port = 9883;

	ܐܢ(db.config->security_options.allow_anonymous == -1){
		listener->security_options->allow_anonymous = true;
	}ܐܚܪܢܐ{
		listener->security_options->allow_anonymous = db.config->security_options.allow_anonymous;
	}

	ܐܢ(listener->http_dir == عدم && strlen(HTTP_API_DIR) > 0){
#ifdef WIN32
		حرف *http_dir_canonical = _fullpath(عدم, HTTP_API_DIR, 0);
#ܐܚܪܢܐ
		حرف *http_dir_canonical = realpath(HTTP_API_DIR, عدم);
#endif
		ܐܢ(!http_dir_canonical){
			ܦܢܐ MOSQ_ERR_NOMEM;
		}
		mosquitto_FREE(listener->http_dir);
		listener->http_dir = mosquitto_strdup(http_dir_canonical);
		mosquitto_FREE(http_dir_canonical);
	}

	ܦܢܐ http_api__start(listener);
}


صحيح http_api__start(بنية mosquitto__listener *listener)
{
	unsigned صحيح flags = MHD_USE_AUTO_INTERNAL_THREAD;
	ثابت حرف *bind_address;
	uint16_t port = 9883;
	حرف *x509_cert = عدم;
	حرف *x509_key = عدم;

	ܐܢ(!listener->security_options){
		listener->security_options = mosquitto_calloc(1, حجم(بنية mosquitto__security_options));
		ܐܢ(listener->security_options == عدم){
			ܦܢܐ MOSQ_ERR_NOMEM;
		}
	}
	listener->protocol = mp_http_api;

	bind_address = listener->host;
	port = listener->port;

#ifdef WITH_TLS
	ܐܢ(listener->certfile && listener->keyfile){
		ܐܢ(mosquitto_read_file(listener->certfile, false, &x509_cert, عدم)){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
			ܦܢܐ MOSQ_ERR_INVAL;
		}
		ܐܢ(mosquitto_read_file(listener->keyfile, false, &x509_key, عدم)){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			mosquitto_FREE(x509_cert);
			ܦܢܐ MOSQ_ERR_INVAL;
		}
		flags |= MHD_USE_TLS;
	}
#endif

	ܐܢ(bind_address){
		حرف service[10];
		بنية addrinfo hints;
		بنية addrinfo *ainfo, *rp;

		snprintf(service, حجم(service), "%d", port);

		memset(&hints, 0, حجم(بنية addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = AI_PASSIVE;
		hints.ai_socktype = SOCK_STREAM;

		صحيح rc = getaddrinfo(bind_address, service, &hints, &ainfo);
		ܐܢ(rc){
			log__printf(عدم, MOSQ_LOG_ERR, "Unable to start http api listener (%s).", strerror(errno));
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_key);
			ܦܢܐ MOSQ_ERR_ERRNO;
		}

		لكل(rp = ainfo; rp; rp = rp->ai_next){
			ܐܢ(rp->ai_family == AF_INET || rp->ai_family == AF_INET6){
				اكسر;
			}
		}
		ܐܢ(!rp){
			log__printf(عدم, MOSQ_LOG_ERR, "Unable to start http api listener, could not find address.");
			freeaddrinfo(ainfo);
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_key);

			ܦܢܐ MOSQ_ERR_INVAL;
		}

		ܐܢ(rp->ai_family == AF_INET6){
			flags |= MHD_USE_IPv6;
		}
		ܐܢ(x509_cert && x509_key){
			listener->mhd = MHD_start_daemon(flags, port, عدم, عدم, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_KEY, x509_key,
					MHD_OPTION_END);
		}ܐܚܪܢܐ{
			listener->mhd = MHD_start_daemon(flags, port, عدم, عدم, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_END);
		}
		freeaddrinfo(ainfo);
	}ܐܚܪܢܐ{
		ܐܢ(x509_cert && x509_key){
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_STACK, port, عدم, عدم, &http_api_handler, listener,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_KEY, x509_key,
					MHD_OPTION_END);
		}ܐܚܪܢܐ{
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_STACK, port, عدم, عدم, &http_api_handler, listener,
					MHD_OPTION_END);
		}
	}

	mosquitto_FREE(x509_cert);
	mosquitto_FREE(x509_key);

	ܐܢ(listener->mhd){
		log__printf(عدم, MOSQ_LOG_INFO, "Opening http api listen socket on port %d.", port);
		ܐܢ(listener->http_dir){
			log__printf(عدم, MOSQ_LOG_INFO, "Using http_dir %s", listener->http_dir);
		}
		ܦܢܐ MOSQ_ERR_SUCCESS;
	}ܐܚܪܢܐ{
		ܦܢܐ MOSQ_ERR_UNKNOWN;
	}
}


فراغ http_api__stop(بنية mosquitto__listener *listener)
{
	MHD_stop_daemon(listener->mhd);
	listener->mhd = عدم;
}

#endif
