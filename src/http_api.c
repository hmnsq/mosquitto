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

#அந்தர்பாவ "config.h"

#ifdef WITH_HTTP_API

#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <errno.h>
#அந்தர்பாவ <limits.h>
#அந்தர்பாவ <microhttpd.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <sys/stat.h>
#ifndef WIN32
#  include <netdb.h>
#endif

#அந்தர்பாவ "json_help.h"
#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto/mqtt_protocol.h"
#அந்தர்பாவ "sys_tree.h"

#ifndef HTTP_API_DIR
#  define HTTP_API_DIR ""
#endif

கட்டமைப்பு metric {
	int64_t current;
	int64_t next;
	மாறிலி எழுத்து *topic, *topic_alias;
	bool is_max;
};

time_t broker_uptime(வெறுமை);
கட்டமைப்பு MHD_Daemon *mhd = வெற்று;

#ifdef WITH_SYS_மரம்
வெளி கட்டமைப்பு metric metrics[mosq_metric_max];
#endif

#define HTTP_RESPONSE_BUFLEN 10000

#ifdef WIN32
#define DIR_SEP '\\'
#define PATH_MAX MAX_PATH
#இல்லையெனில்
#define DIR_SEP '/'
#endif

#ifndef S_ISREG
#define S_ISREG(mode) (((mode)&S_IFMT) == S_IFREG)
#endif


நிலையான எழுத்து *http__canonical_filename(
		மாறிலி எழுத்து *url,
		மாறிலி எழுத்து *http_dir,
		முழு *error_code)
{
	size_t urllen, slen;
	எழுத்து *filename, *filename_canonical;

	urllen = strlen(url);
	எனில்(url[urllen-1] == '/'){
		slen = strlen(http_dir) + urllen + strlen("/index.html") + 2;
	}இல்லையெனில்{
		slen = strlen(http_dir) + urllen + 2;
	}
	filename = mosquitto_malloc(slen);
	எனில்(!filename){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		திரும்பு வெற்று;
	}
	எனில்(((எழுத்து *)url)[urllen-1] == '/'){
		snprintf(filename, slen, "%s%c%sindex.html", http_dir, DIR_SEP, url);
	}இல்லையெனில்{
		snprintf(filename, slen, "%s%c%s", http_dir, DIR_SEP, url);
	}


	/* Get canonical path and check it is within our http_dir */
	filename_canonical = mosquitto_calloc(1, PATH_MAX);
	எனில்(!filename_canonical){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		திரும்பு வெற்று;
	}
#ifdef WIN32
	எழுத்து *resolved = _fullpath(filename_canonical, filename, PATH_MAX);
	mosquitto_FREE(filename);
	எனில்(!resolved){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		mosquitto_FREE(filename_canonical);
		திரும்பு வெற்று;
	}
#இல்லையெனில்
	எழுத்து *resolved = realpath(filename, filename_canonical);
	mosquitto_FREE(filename);
	எனில்(!resolved){
		எனில்(errno == EACCES){
			*error_code = MHD_HTTP_FORBIDDEN;
		}இல்லையெனில் எனில்(errno == EINVAL || errno == EIO || errno == ELOOP){
			*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		}இல்லையெனில் எனில்(errno == ENAMETOOLONG){
			*error_code = MHD_HTTP_URI_TOO_LONG;
		}இல்லையெனில் எனில்(errno == ENOENT || errno == ENOTDIR){
			*error_code = MHD_HTTP_NOT_FOUND;
		}
		mosquitto_FREE(filename_canonical);
		திரும்பு வெற்று;
	}
#endif
	எனில்(strncmp(http_dir, filename_canonical, strlen(http_dir))){
		/* Requested file isn't within http_dir, deny access because it's not found. */
		mosquitto_FREE(filename_canonical);
		*error_code = MHD_HTTP_NOT_FOUND;
		திரும்பு வெற்று;
	}

	திரும்பு filename_canonical;
}

நிலையான எண்ணிக்கை MHD_Result http_api__send_error_response(கட்டமைப்பு MHD_Connection *connection, மாறிலி எழுத்து *error_str, unsigned முழு error_code)
{
	கட்டமைப்பு MHD_Response *response = MHD_create_response_from_buffer(strlen(error_str), (வெறுமை *)error_str, MHD_RESPMEM_MUST_COPY);
	எண்ணிக்கை MHD_Result ret = MHD_queue_response(connection, error_code, response);
	MHD_destroy_response(response);
	திரும்பு ret;
}

நிலையான எண்ணிக்கை MHD_Result http_api__send_response_with_headers(கட்டமைப்பு MHD_Connection *connection, மாறிலி எழுத்து *buf)
{
	எண்ணிக்கை MHD_Result ret;
	கட்டமைப்பு MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (வெறுமை *)buf, MHD_RESPMEM_MUST_COPY);

	ret = MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
	எனில்(ret != MHD_YES){
		MHD_destroy_response(response);
		திரும்பு ret;
	}
	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	திரும்பு ret;
}

நிலையான எண்ணிக்கை MHD_Result http_api__process_version(கட்டமைப்பு MHD_Connection *connection)
{
	திரும்பு http_api__send_response_with_headers(connection, VERSION);
}

நிலையான எண்ணிக்கை MHD_Result http_api__process_listeners(கட்டமைப்பு MHD_Connection *connection)
{
	எழுத்து *buf;
	எண்ணிக்கை MHD_Result ret;

	cJSON *j_மரம் = cJSON_CreateObject();
	எனில்(!j_மரம்){
		திரும்பு http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	cJSON *j_listeners = cJSON_AddArrayToObject(j_மரம், "listeners");
	எனில்(!j_listeners){
		cJSON_Delete(j_மரம்);
		திரும்பு http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	க்கு(முழு i=0; i<db.config->listener_மொத்தம்; i++){
		cJSON *j_listener = cJSON_CreateObject();
		எனில்(!j_listener){
			cJSON_Delete(j_மரம்);
			திரும்பு http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
		}
		cJSON_AddItemToArray(j_listeners, j_listener);

		கட்டமைப்பு mosquitto__listener *listener = &db.config->listeners[i];
#ifdef WITH_UNIX_SOCKETS
		எனில்(listener->unix_socket_path){
			cJSON_AddStringToObject(j_listener, "path", listener->unix_socket_path);
		}இல்லையெனில்
#endif
		{
			cJSON_AddIntToObject(j_listener, "port", listener->port);
		}

		தேர்வு(listener->protocol){
			நிலை mp_mqtt:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt");
				நிறுத்து;
			நிலை mp_mqttsn:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt-sn");
				நிறுத்து;
			நிலை mp_websockets:
				cJSON_AddStringToObject(j_listener, "protocol", "websockets");
				நிறுத்து;
			நிலை mp_http_api:
				cJSON_AddStringToObject(j_listener, "protocol", "httpapi");
				நிறுத்து;
		}

#ifdef WITH_TLS
		cJSON_AddBoolToObject(j_listener, "tls", listener->certfile && listener->keyfile);
		cJSON_AddBoolToObject(j_listener, "mtls", listener->require_certificate);
#இல்லையெனில்
		cJSON_AddBoolToObject(j_listener, "tls", false);
		cJSON_AddBoolToObject(j_listener, "mtls", false);
#endif
		எனில்(listener->security_options->allow_anonymous == -1){
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", db.config->security_options.allow_anonymous);
		}இல்லையெனில்{
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", listener->security_options->allow_anonymous);
		}
	}
	buf = cJSON_Print(j_மரம்);
	cJSON_Delete(j_மரம்);
	எனில்(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		விடுவி(buf);
	}இல்லையெனில்{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	திரும்பு ret;
}


நிலையான எண்ணிக்கை MHD_Result http_api__process_systree(கட்டமைப்பு MHD_Connection *connection)
{
	எண்ணிக்கை MHD_Result ret;
#ifdef WITH_SYS_மரம்
	எழுத்து *buf;

	cJSON *j_மரம் = cJSON_CreateObject();
	க்கு(முழு i=0; i<mosq_metric_max; i++){
		எனில்(metrics[i].topic){
			cJSON_AddIntToObject(j_மரம், metrics[i].topic, metrics[i].current);
		}
	}
	cJSON_AddIntToObject(j_மரம், "$SYS/broker/uptime", broker_uptime());

	buf = cJSON_Print(j_மரம்);
	cJSON_Delete(j_மரம்);

	எனில்(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		விடுவி(buf);
	}இல்லையெனில்{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}
#இல்லையெனில்
	ret = http_api__send_error_response(connection, "Not found.\n", 404);
#endif

	திரும்பு ret;
}


நிலையான ssize_t http_file_read_cb(வெறுமை *cls, uint64_t pos, எழுத்து *buf, size_t max)
{
	FILE *fptr = cls;
	எனில்(fseek(fptr, (long )pos, SEEK_SET) < 0){
		திரும்பு -1;
	}
	திரும்பு (ssize_t )fread(buf, 1, max, fptr);
}


நிலையான வெறுமை http_file_free_cb(வெறுமை *cls)
{
	FILE *fptr = cls;
	fclose(fptr);
}


நிலையான எண்ணிக்கை MHD_Result http_api__process_file(கட்டமைப்பு mosquitto__listener *listener, கட்டமைப்பு MHD_Connection *connection, மாறிலி எழுத்து *url)
{
	முழு error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
	எழுத்து *canonical_filename;

	எனில்(!listener || !listener->http_dir){
		http_api__send_error_response(connection, "Not found.\n", 404);
		திரும்பு MHD_YES;
	}

	canonical_filename = http__canonical_filename(url, listener->http_dir, &error_code);
	எனில்(!canonical_filename){
		http_api__send_error_response(connection, "Not found.\n", 404);
		திரும்பு MHD_YES;
	}

	FILE *fptr = fopen(canonical_filename, "rb");
	mosquitto_FREE(canonical_filename);
	எனில்(!fptr){
		http_api__send_error_response(connection, "Not found.\n", 404);
		திரும்பு MHD_YES;
	}

	கட்டமைப்பு stat statbuf;
	எனில்(fstat(fileno(fptr), &statbuf)){
		fclose(fptr);
		http_api__send_error_response(connection, "Internal server error.\n", 500);
		திரும்பு MHD_YES;
	}

	எனில்(!S_ISREG(statbuf.st_mode)){
		fclose(fptr);
		http_api__send_error_response(connection, "Not found.\n", 404);
		திரும்பு MHD_YES;
	}
	uint64_t flen = (uint64_t )statbuf.st_size;

	/* Using MHD_create_response_from_fd would be easier here, but is less portable */
	கட்டமைப்பு MHD_Response *response = MHD_create_response_from_callback(
			flen,
			65536,
			http_file_read_cb,
			fptr,
			http_file_free_cb);

	எண்ணிக்கை MHD_Result ret = MHD_queue_response(connection, 200, response);
	MHD_destroy_response(response);
	திரும்பு ret;
}


நிலையான எண்ணிக்கை MHD_Result http_api__process_api(கட்டமைப்பு MHD_Connection *connection, மாறிலி எழுத்து *url)
{
	எனில்(strcmp(url, "/api/v1/systree") == 0){
		திரும்பு http_api__process_systree(connection);
	}இல்லையெனில் எனில்(strcmp(url, "/api/v1/listeners") == 0){
		திரும்பு http_api__process_listeners(connection);
	}இல்லையெனில் எனில்(strcmp(url, "/api/v1/version") == 0){
		திரும்பு http_api__process_version(connection);
	}இல்லையெனில்{
		திரும்பு http_api__send_error_response(connection, "Not found.\n", 404);
	}
}


நிலையான முழு check_access(கட்டமைப்பு mosquitto__listener *listener, கட்டமைப்பு MHD_Connection *connection, மாறிலி எழுத்து *url)
{
	கட்டமைப்பு mosquitto context = {0};
	முழு auth_rc, acl_rc = MOSQ_ERR_SUCCESS;

	context.listener = listener;

	context.id = (எழுத்து *)"http-api";
	context.username = MHD_basic_auth_get_username_password(connection, &context.password);

	/* Authentication */
	auth_rc = mosquitto_basic_auth(&context);
	எனில்(auth_rc == MOSQ_ERR_SUCCESS){
		acl_rc = mosquitto_acl_check(&context, url, 0, வெற்று, 0, false, வெற்று, MOSQ_ACL_READ);
	}
	MHD_free(context.username);
	MHD_free(context.password);

	எனில்(auth_rc || acl_rc){
		மாறிலி எழுத்து *buf = "Not authorised\n";
		கட்டமைப்பு MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (வெறுமை *)buf, MHD_RESPMEM_MUST_COPY);
		MHD_queue_basic_auth_fail_response(connection, "Mosquitto API", response);
		MHD_destroy_response(response);

		திரும்பு MOSQ_ERR_AUTH;
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}

நிலையான எண்ணிக்கை MHD_Result http_api_handler(வெறுமை *cls, கட்டமைப்பு MHD_Connection
		*connection, மாறிலி எழுத்து *url, மாறிலி எழுத்து *method, மாறிலி எழுத்து *version,
		மாறிலி எழுத்து *upload_data, size_t *upload_data_size, வெறுமை **con_cls)
{
	UNUSED(version);
	UNUSED(upload_data);
	UNUSED(upload_data_size);
	UNUSED(con_cls);

	கட்டமைப்பு mosquitto__listener *listener = cls;

	எனில்(strcmp(method, "GET") != 0){
		எழுத்து *buf = "Invalid HTTP Method\n";
		கட்டமைப்பு MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (வெறுமை *)buf, MHD_RESPMEM_MUST_COPY);
		எண்ணிக்கை MHD_Result ret = MHD_queue_response(connection, MHD_HTTP_METHOD_NOT_ALLOWED, response);
		MHD_destroy_response(response);
		திரும்பு ret;
	}

	எனில்(check_access(listener, connection, url) != MOSQ_ERR_SUCCESS){
		திரும்பு MHD_YES;
	}

	எனில்(!strncasecmp(url, "/api/", strlen("/api/"))){
		திரும்பு http_api__process_api(connection, url);
	}இல்லையெனில்{
		திரும்பு http_api__process_file(listener, connection, url);
	}
}


முழு http_api__start_local(கட்டமைப்பு mosquitto__listener *listener)
{
	listener->security_options = mosquitto_calloc(1, அளவு(கட்டமைப்பு mosquitto__security_options));
	எனில்(listener->security_options == வெற்று){
		திரும்பு MOSQ_ERR_NOMEM;
	}

	listener->host = mosquitto_strdup("127.0.0.1");
	எனில்(!listener->host){
		mosquitto_FREE(listener->security_options);
		திரும்பு MOSQ_ERR_NOMEM;
	}
	listener->port = 9883;

	எனில்(db.config->security_options.allow_anonymous == -1){
		listener->security_options->allow_anonymous = true;
	}இல்லையெனில்{
		listener->security_options->allow_anonymous = db.config->security_options.allow_anonymous;
	}

	எனில்(listener->http_dir == வெற்று && strlen(HTTP_API_DIR) > 0){
#ifdef WIN32
		எழுத்து *http_dir_canonical = _fullpath(வெற்று, HTTP_API_DIR, 0);
#இல்லையெனில்
		எழுத்து *http_dir_canonical = realpath(HTTP_API_DIR, வெற்று);
#endif
		எனில்(!http_dir_canonical){
			திரும்பு MOSQ_ERR_NOMEM;
		}
		mosquitto_FREE(listener->http_dir);
		listener->http_dir = mosquitto_strdup(http_dir_canonical);
		mosquitto_FREE(http_dir_canonical);
	}

	திரும்பு http_api__start(listener);
}


முழு http_api__start(கட்டமைப்பு mosquitto__listener *listener)
{
	unsigned முழு flags = MHD_USE_AUTO_INTERNAL_THREAD;
	மாறிலி எழுத்து *bind_address;
	uint16_t port = 9883;
	எழுத்து *x509_cert = வெற்று;
	எழுத்து *x509_சாவி = வெற்று;

	எனில்(!listener->security_options){
		listener->security_options = mosquitto_calloc(1, அளவு(கட்டமைப்பு mosquitto__security_options));
		எனில்(listener->security_options == வெற்று){
			திரும்பு MOSQ_ERR_NOMEM;
		}
	}
	listener->protocol = mp_http_api;

	bind_address = listener->host;
	port = listener->port;

#ifdef WITH_TLS
	எனில்(listener->certfile && listener->keyfile){
		எனில்(mosquitto_read_file(listener->certfile, false, &x509_cert, வெற்று)){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
			திரும்பு MOSQ_ERR_INVAL;
		}
		எனில்(mosquitto_read_file(listener->keyfile, false, &x509_சாவி, வெற்று)){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			mosquitto_FREE(x509_cert);
			திரும்பு MOSQ_ERR_INVAL;
		}
		flags |= MHD_USE_TLS;
	}
#endif

	எனில்(bind_address){
		எழுத்து service[10];
		கட்டமைப்பு addrinfo hints;
		கட்டமைப்பு addrinfo *ainfo, *rp;

		snprintf(service, அளவு(service), "%d", port);

		memset(&hints, 0, அளவு(கட்டமைப்பு addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = AI_PASSIVE;
		hints.ai_socktype = SOCK_STREAM;

		முழு rc = getaddrinfo(bind_address, service, &hints, &ainfo);
		எனில்(rc){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Unable to start http api listener (%s).", strerror(errno));
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_சாவி);
			திரும்பு MOSQ_ERR_ERRNO;
		}

		க்கு(rp = ainfo; rp; rp = rp->ai_next){
			எனில்(rp->ai_family == AF_INET || rp->ai_family == AF_INET6){
				நிறுத்து;
			}
		}
		எனில்(!rp){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Unable to start http api listener, could not find address.");
			freeaddrinfo(ainfo);
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_சாவி);

			திரும்பு MOSQ_ERR_INVAL;
		}

		எனில்(rp->ai_family == AF_INET6){
			flags |= MHD_USE_IPv6;
		}
		எனில்(x509_cert && x509_சாவி){
			listener->mhd = MHD_start_daemon(flags, port, வெற்று, வெற்று, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_சாவி, x509_சாவி,
					MHD_OPTION_END);
		}இல்லையெனில்{
			listener->mhd = MHD_start_daemon(flags, port, வெற்று, வெற்று, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_END);
		}
		freeaddrinfo(ainfo);
	}இல்லையெனில்{
		எனில்(x509_cert && x509_சாவி){
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_அடுக்கு, port, வெற்று, வெற்று, &http_api_handler, listener,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_சாவி, x509_சாவி,
					MHD_OPTION_END);
		}இல்லையெனில்{
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_அடுக்கு, port, வெற்று, வெற்று, &http_api_handler, listener,
					MHD_OPTION_END);
		}
	}

	mosquitto_FREE(x509_cert);
	mosquitto_FREE(x509_சாவி);

	எனில்(listener->mhd){
		log__printf(வெற்று, MOSQ_LOG_INFO, "Opening http api listen socket on port %d.", port);
		எனில்(listener->http_dir){
			log__printf(வெற்று, MOSQ_LOG_INFO, "Using http_dir %s", listener->http_dir);
		}
		திரும்பு MOSQ_ERR_SUCCESS;
	}இல்லையெனில்{
		திரும்பு MOSQ_ERR_UNKNOWN;
	}
}


வெறுமை http_api__stop(கட்டமைப்பு mosquitto__listener *listener)
{
	MHD_stop_daemon(listener->mhd);
	listener->mhd = வெற்று;
}

#endif
