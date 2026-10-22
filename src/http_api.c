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

#ಅನ್ತರ್ಭಾವ "config.h"

#ifdef WITH_HTTP_API

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ <limits.h>
#ಅನ್ತರ್ಭಾವ <microhttpd.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ifndef WIN32
#  include <netdb.h>
#endif

#ಅನ್ತರ್ಭಾವ "json_help.h"
#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "sys_tree.h"

#ifndef HTTP_API_DIR
#  define HTTP_API_DIR ""
#endif

ಸಂರಚನಾ metric {
	int64_t current;
	int64_t next;
	ಧ್ರುವ ವರ್ಣ *topic, *topic_alias;
	bool is_max;
};

time_t broker_uptime(ಶೂನ್ಯ);
ಸಂರಚನಾ MHD_Daemon *mhd = ರಿಕ್ತ;

#ifdef WITH_SYS_ವೃಕ್ಷ
ಬಾಹ್ಯ ಸಂರಚನಾ metric metrics[mosq_metric_max];
#endif

#define HTTP_RESPONSE_BUFLEN 10000

#ifdef WIN32
#define DIR_SEP '\\'
#define PATH_MAX MAX_PATH
#ಅನ್ಯಥಾ
#define DIR_SEP '/'
#endif

#ifndef S_ISREG
#define S_ISREG(mode) (((mode)&S_IFMT) == S_IFREG)
#endif


ಸ್ಥಿರ ವರ್ಣ *http__canonical_filename(
		ಧ್ರುವ ವರ್ಣ *url,
		ಧ್ರುವ ವರ್ಣ *http_dir,
		ಪೂರ್ಣಾಙ್ಕ *error_code)
{
	size_t urllen, slen;
	ವರ್ಣ *filename, *filename_canonical;

	urllen = strlen(url);
	ಯದಿ(url[urllen-1] == '/'){
		slen = strlen(http_dir) + urllen + strlen("/index.html") + 2;
	}ಅನ್ಯಥಾ{
		slen = strlen(http_dir) + urllen + 2;
	}
	filename = mosquitto_malloc(slen);
	ಯದಿ(!filename){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	ಯದಿ(((ವರ್ಣ *)url)[urllen-1] == '/'){
		snprintf(filename, slen, "%s%c%sindex.html", http_dir, DIR_SEP, url);
	}ಅನ್ಯಥಾ{
		snprintf(filename, slen, "%s%c%s", http_dir, DIR_SEP, url);
	}


	/* Get canonical path and check it is within our http_dir */
	filename_canonical = mosquitto_calloc(1, PATH_MAX);
	ಯದಿ(!filename_canonical){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
#ifdef WIN32
	ವರ್ಣ *resolved = _fullpath(filename_canonical, filename, PATH_MAX);
	mosquitto_FREE(filename);
	ಯದಿ(!resolved){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		mosquitto_FREE(filename_canonical);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
#ಅನ್ಯಥಾ
	ವರ್ಣ *resolved = realpath(filename, filename_canonical);
	mosquitto_FREE(filename);
	ಯದಿ(!resolved){
		ಯದಿ(errno == EACCES){
			*error_code = MHD_HTTP_FORBIDDEN;
		}ಅನ್ಯಥಾ ಯದಿ(errno == EINVAL || errno == EIO || errno == ELOOP){
			*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		}ಅನ್ಯಥಾ ಯದಿ(errno == ENAMETOOLONG){
			*error_code = MHD_HTTP_URI_TOO_LONG;
		}ಅನ್ಯಥಾ ಯದಿ(errno == ENOENT || errno == ENOTDIR){
			*error_code = MHD_HTTP_NOT_FOUND;
		}
		mosquitto_FREE(filename_canonical);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
#endif
	ಯದಿ(strncmp(http_dir, filename_canonical, strlen(http_dir))){
		/* Requested file isn't within http_dir, deny access because it's not found. */
		mosquitto_FREE(filename_canonical);
		*error_code = MHD_HTTP_NOT_FOUND;
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	ಪ್ರತ್ಯಾಗಮ filename_canonical;
}

ಸ್ಥಿರ ಗಣನಾ MHD_Result http_api__send_error_response(ಸಂರಚನಾ MHD_Connection *connection, ಧ್ರುವ ವರ್ಣ *error_str, unsigned ಪೂರ್ಣಾಙ್ಕ error_code)
{
	ಸಂರಚನಾ MHD_Response *response = MHD_create_response_from_buffer(strlen(error_str), (ಶೂನ್ಯ *)error_str, MHD_RESPMEM_MUST_COPY);
	ಗಣನಾ MHD_Result ret = MHD_queue_response(connection, error_code, response);
	MHD_destroy_response(response);
	ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ ಗಣನಾ MHD_Result http_api__send_response_with_headers(ಸಂರಚನಾ MHD_Connection *connection, ಧ್ರುವ ವರ್ಣ *buf)
{
	ಗಣನಾ MHD_Result ret;
	ಸಂರಚನಾ MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (ಶೂನ್ಯ *)buf, MHD_RESPMEM_MUST_COPY);

	ret = MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
	ಯದಿ(ret != MHD_YES){
		MHD_destroy_response(response);
		ಪ್ರತ್ಯಾಗಮ ret;
	}
	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ ಗಣನಾ MHD_Result http_api__process_version(ಸಂರಚನಾ MHD_Connection *connection)
{
	ಪ್ರತ್ಯಾಗಮ http_api__send_response_with_headers(connection, VERSION);
}

ಸ್ಥಿರ ಗಣನಾ MHD_Result http_api__process_listeners(ಸಂರಚನಾ MHD_Connection *connection)
{
	ವರ್ಣ *buf;
	ಗಣನಾ MHD_Result ret;

	cJSON *j_ವೃಕ್ಷ = cJSON_CreateObject();
	ಯದಿ(!j_ವೃಕ್ಷ){
		ಪ್ರತ್ಯಾಗಮ http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	cJSON *j_listeners = cJSON_AddArrayToObject(j_ವೃಕ್ಷ, "listeners");
	ಯದಿ(!j_listeners){
		cJSON_Delete(j_ವೃಕ್ಷ);
		ಪ್ರತ್ಯಾಗಮ http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
		cJSON *j_listener = cJSON_CreateObject();
		ಯದಿ(!j_listener){
			cJSON_Delete(j_ವೃಕ್ಷ);
			ಪ್ರತ್ಯಾಗಮ http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
		}
		cJSON_AddItemToArray(j_listeners, j_listener);

		ಸಂರಚನಾ mosquitto__listener *listener = &db.config->listeners[i];
#ifdef WITH_UNIX_SOCKETS
		ಯದಿ(listener->unix_socket_path){
			cJSON_AddStringToObject(j_listener, "path", listener->unix_socket_path);
		}ಅನ್ಯಥಾ
#endif
		{
			cJSON_AddIntToObject(j_listener, "port", listener->port);
		}

		ವಿಕಲ್ಪ(listener->protocol){
			ಪಕ್ಷ mp_mqtt:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt");
				ಭಙ್ಗ;
			ಪಕ್ಷ mp_mqttsn:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt-sn");
				ಭಙ್ಗ;
			ಪಕ್ಷ mp_websockets:
				cJSON_AddStringToObject(j_listener, "protocol", "websockets");
				ಭಙ್ಗ;
			ಪಕ್ಷ mp_http_api:
				cJSON_AddStringToObject(j_listener, "protocol", "httpapi");
				ಭಙ್ಗ;
		}

#ifdef WITH_TLS
		cJSON_AddBoolToObject(j_listener, "tls", listener->certfile && listener->keyfile);
		cJSON_AddBoolToObject(j_listener, "mtls", listener->require_certificate);
#ಅನ್ಯಥಾ
		cJSON_AddBoolToObject(j_listener, "tls", false);
		cJSON_AddBoolToObject(j_listener, "mtls", false);
#endif
		ಯದಿ(listener->security_options->allow_anonymous == -1){
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", db.config->security_options.allow_anonymous);
		}ಅನ್ಯಥಾ{
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", listener->security_options->allow_anonymous);
		}
	}
	buf = cJSON_Print(j_ವೃಕ್ಷ);
	cJSON_Delete(j_ವೃಕ್ಷ);
	ಯದಿ(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		ಮುಕ್ತ(buf);
	}ಅನ್ಯಥಾ{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}


ಸ್ಥಿರ ಗಣನಾ MHD_Result http_api__process_systree(ಸಂರಚನಾ MHD_Connection *connection)
{
	ಗಣನಾ MHD_Result ret;
#ifdef WITH_SYS_ವೃಕ್ಷ
	ವರ್ಣ *buf;

	cJSON *j_ವೃಕ್ಷ = cJSON_CreateObject();
	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<mosq_metric_max; i++){
		ಯದಿ(metrics[i].topic){
			cJSON_AddIntToObject(j_ವೃಕ್ಷ, metrics[i].topic, metrics[i].current);
		}
	}
	cJSON_AddIntToObject(j_ವೃಕ್ಷ, "$SYS/broker/uptime", broker_uptime());

	buf = cJSON_Print(j_ವೃಕ್ಷ);
	cJSON_Delete(j_ವೃಕ್ಷ);

	ಯದಿ(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		ಮುಕ್ತ(buf);
	}ಅನ್ಯಥಾ{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}
#ಅನ್ಯಥಾ
	ret = http_api__send_error_response(connection, "Not found.\n", 404);
#endif

	ಪ್ರತ್ಯಾಗಮ ret;
}


ಸ್ಥಿರ ssize_t http_file_read_cb(ಶೂನ್ಯ *cls, uint64_t pos, ವರ್ಣ *buf, size_t max)
{
	FILE *fptr = cls;
	ಯದಿ(fseek(fptr, (long )pos, SEEK_SET) < 0){
		ಪ್ರತ್ಯಾಗಮ -1;
	}
	ಪ್ರತ್ಯಾಗಮ (ssize_t )fread(buf, 1, max, fptr);
}


ಸ್ಥಿರ ಶೂನ್ಯ http_file_free_cb(ಶೂನ್ಯ *cls)
{
	FILE *fptr = cls;
	fclose(fptr);
}


ಸ್ಥಿರ ಗಣನಾ MHD_Result http_api__process_file(ಸಂರಚನಾ mosquitto__listener *listener, ಸಂರಚನಾ MHD_Connection *connection, ಧ್ರುವ ವರ್ಣ *url)
{
	ಪೂರ್ಣಾಙ್ಕ error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
	ವರ್ಣ *canonical_filename;

	ಯದಿ(!listener || !listener->http_dir){
		http_api__send_error_response(connection, "Not found.\n", 404);
		ಪ್ರತ್ಯಾಗಮ MHD_YES;
	}

	canonical_filename = http__canonical_filename(url, listener->http_dir, &error_code);
	ಯದಿ(!canonical_filename){
		http_api__send_error_response(connection, "Not found.\n", 404);
		ಪ್ರತ್ಯಾಗಮ MHD_YES;
	}

	FILE *fptr = fopen(canonical_filename, "rb");
	mosquitto_FREE(canonical_filename);
	ಯದಿ(!fptr){
		http_api__send_error_response(connection, "Not found.\n", 404);
		ಪ್ರತ್ಯಾಗಮ MHD_YES;
	}

	ಸಂರಚನಾ stat statbuf;
	ಯದಿ(fstat(fileno(fptr), &statbuf)){
		fclose(fptr);
		http_api__send_error_response(connection, "Internal server error.\n", 500);
		ಪ್ರತ್ಯಾಗಮ MHD_YES;
	}

	ಯದಿ(!S_ISREG(statbuf.st_mode)){
		fclose(fptr);
		http_api__send_error_response(connection, "Not found.\n", 404);
		ಪ್ರತ್ಯಾಗಮ MHD_YES;
	}
	uint64_t flen = (uint64_t )statbuf.st_size;

	/* Using MHD_create_response_from_fd would be easier here, but is less portable */
	ಸಂರಚನಾ MHD_Response *response = MHD_create_response_from_callback(
			flen,
			65536,
			http_file_read_cb,
			fptr,
			http_file_free_cb);

	ಗಣನಾ MHD_Result ret = MHD_queue_response(connection, 200, response);
	MHD_destroy_response(response);
	ಪ್ರತ್ಯಾಗಮ ret;
}


ಸ್ಥಿರ ಗಣನಾ MHD_Result http_api__process_api(ಸಂರಚನಾ MHD_Connection *connection, ಧ್ರುವ ವರ್ಣ *url)
{
	ಯದಿ(strcmp(url, "/api/v1/systree") == 0){
		ಪ್ರತ್ಯಾಗಮ http_api__process_systree(connection);
	}ಅನ್ಯಥಾ ಯದಿ(strcmp(url, "/api/v1/listeners") == 0){
		ಪ್ರತ್ಯಾಗಮ http_api__process_listeners(connection);
	}ಅನ್ಯಥಾ ಯದಿ(strcmp(url, "/api/v1/version") == 0){
		ಪ್ರತ್ಯಾಗಮ http_api__process_version(connection);
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ http_api__send_error_response(connection, "Not found.\n", 404);
	}
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ check_access(ಸಂರಚನಾ mosquitto__listener *listener, ಸಂರಚನಾ MHD_Connection *connection, ಧ್ರುವ ವರ್ಣ *url)
{
	ಸಂರಚನಾ mosquitto context = {0};
	ಪೂರ್ಣಾಙ್ಕ auth_rc, acl_rc = MOSQ_ERR_SUCCESS;

	context.listener = listener;

	context.id = (ವರ್ಣ *)"http-api";
	context.username = MHD_basic_auth_get_username_password(connection, &context.password);

	/* Authentication */
	auth_rc = mosquitto_basic_auth(&context);
	ಯದಿ(auth_rc == MOSQ_ERR_SUCCESS){
		acl_rc = mosquitto_acl_check(&context, url, 0, ರಿಕ್ತ, 0, false, ರಿಕ್ತ, MOSQ_ACL_READ);
	}
	MHD_free(context.username);
	MHD_free(context.password);

	ಯದಿ(auth_rc || acl_rc){
		ಧ್ರುವ ವರ್ಣ *buf = "Not authorised\n";
		ಸಂರಚನಾ MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (ಶೂನ್ಯ *)buf, MHD_RESPMEM_MUST_COPY);
		MHD_queue_basic_auth_fail_response(connection, "Mosquitto API", response);
		MHD_destroy_response(response);

		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_AUTH;
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}

ಸ್ಥಿರ ಗಣನಾ MHD_Result http_api_handler(ಶೂನ್ಯ *cls, ಸಂರಚನಾ MHD_Connection
		*connection, ಧ್ರುವ ವರ್ಣ *url, ಧ್ರುವ ವರ್ಣ *method, ಧ್ರುವ ವರ್ಣ *version,
		ಧ್ರುವ ವರ್ಣ *upload_data, size_t *upload_data_size, ಶೂನ್ಯ **con_cls)
{
	UNUSED(version);
	UNUSED(upload_data);
	UNUSED(upload_data_size);
	UNUSED(con_cls);

	ಸಂರಚನಾ mosquitto__listener *listener = cls;

	ಯದಿ(strcmp(method, "GET") != 0){
		ವರ್ಣ *buf = "Invalid HTTP Method\n";
		ಸಂರಚನಾ MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (ಶೂನ್ಯ *)buf, MHD_RESPMEM_MUST_COPY);
		ಗಣನಾ MHD_Result ret = MHD_queue_response(connection, MHD_HTTP_METHOD_NOT_ALLOWED, response);
		MHD_destroy_response(response);
		ಪ್ರತ್ಯಾಗಮ ret;
	}

	ಯದಿ(check_access(listener, connection, url) != MOSQ_ERR_SUCCESS){
		ಪ್ರತ್ಯಾಗಮ MHD_YES;
	}

	ಯದಿ(!strncasecmp(url, "/api/", strlen("/api/"))){
		ಪ್ರತ್ಯಾಗಮ http_api__process_api(connection, url);
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ http_api__process_file(listener, connection, url);
	}
}


ಪೂರ್ಣಾಙ್ಕ http_api__start_local(ಸಂರಚನಾ mosquitto__listener *listener)
{
	listener->security_options = mosquitto_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__security_options));
	ಯದಿ(listener->security_options == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}

	listener->host = mosquitto_strdup("127.0.0.1");
	ಯದಿ(!listener->host){
		mosquitto_FREE(listener->security_options);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}
	listener->port = 9883;

	ಯದಿ(db.config->security_options.allow_anonymous == -1){
		listener->security_options->allow_anonymous = true;
	}ಅನ್ಯಥಾ{
		listener->security_options->allow_anonymous = db.config->security_options.allow_anonymous;
	}

	ಯದಿ(listener->http_dir == ರಿಕ್ತ && strlen(HTTP_API_DIR) > 0){
#ifdef WIN32
		ವರ್ಣ *http_dir_canonical = _fullpath(ರಿಕ್ತ, HTTP_API_DIR, 0);
#ಅನ್ಯಥಾ
		ವರ್ಣ *http_dir_canonical = realpath(HTTP_API_DIR, ರಿಕ್ತ);
#endif
		ಯದಿ(!http_dir_canonical){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}
		mosquitto_FREE(listener->http_dir);
		listener->http_dir = mosquitto_strdup(http_dir_canonical);
		mosquitto_FREE(http_dir_canonical);
	}

	ಪ್ರತ್ಯಾಗಮ http_api__start(listener);
}


ಪೂರ್ಣಾಙ್ಕ http_api__start(ಸಂರಚನಾ mosquitto__listener *listener)
{
	unsigned ಪೂರ್ಣಾಙ್ಕ flags = MHD_USE_AUTO_INTERNAL_THREAD;
	ಧ್ರುವ ವರ್ಣ *bind_address;
	uint16_t port = 9883;
	ವರ್ಣ *x509_cert = ರಿಕ್ತ;
	ವರ್ಣ *x509_ಕೀಲಿ = ರಿಕ್ತ;

	ಯದಿ(!listener->security_options){
		listener->security_options = mosquitto_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__security_options));
		ಯದಿ(listener->security_options == ರಿಕ್ತ){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}
	}
	listener->protocol = mp_http_api;

	bind_address = listener->host;
	port = listener->port;

#ifdef WITH_TLS
	ಯದಿ(listener->certfile && listener->keyfile){
		ಯದಿ(mosquitto_read_file(listener->certfile, false, &x509_cert, ರಿಕ್ತ)){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
		ಯದಿ(mosquitto_read_file(listener->keyfile, false, &x509_ಕೀಲಿ, ರಿಕ್ತ)){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			mosquitto_FREE(x509_cert);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
		flags |= MHD_USE_TLS;
	}
#endif

	ಯದಿ(bind_address){
		ವರ್ಣ service[10];
		ಸಂರಚನಾ addrinfo hints;
		ಸಂರಚನಾ addrinfo *ainfo, *rp;

		snprintf(service, ಪರಿಮಾಣ(service), "%d", port);

		memset(&hints, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = AI_PASSIVE;
		hints.ai_socktype = SOCK_STREAM;

		ಪೂರ್ಣಾಙ್ಕ rc = getaddrinfo(bind_address, service, &hints, &ainfo);
		ಯದಿ(rc){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Unable to start http api listener (%s).", strerror(errno));
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_ಕೀಲಿ);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_ERRNO;
		}

		ಪ್ರತಿ(rp = ainfo; rp; rp = rp->ai_next){
			ಯದಿ(rp->ai_family == AF_INET || rp->ai_family == AF_INET6){
				ಭಙ್ಗ;
			}
		}
		ಯದಿ(!rp){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Unable to start http api listener, could not find address.");
			freeaddrinfo(ainfo);
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_ಕೀಲಿ);

			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}

		ಯದಿ(rp->ai_family == AF_INET6){
			flags |= MHD_USE_IPv6;
		}
		ಯದಿ(x509_cert && x509_ಕೀಲಿ){
			listener->mhd = MHD_start_daemon(flags, port, ರಿಕ್ತ, ರಿಕ್ತ, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_ಕೀಲಿ, x509_ಕೀಲಿ,
					MHD_OPTION_END);
		}ಅನ್ಯಥಾ{
			listener->mhd = MHD_start_daemon(flags, port, ರಿಕ್ತ, ರಿಕ್ತ, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_END);
		}
		freeaddrinfo(ainfo);
	}ಅನ್ಯಥಾ{
		ಯದಿ(x509_cert && x509_ಕೀಲಿ){
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_ರಾಶಿ, port, ರಿಕ್ತ, ರಿಕ್ತ, &http_api_handler, listener,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_ಕೀಲಿ, x509_ಕೀಲಿ,
					MHD_OPTION_END);
		}ಅನ್ಯಥಾ{
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_ರಾಶಿ, port, ರಿಕ್ತ, ರಿಕ್ತ, &http_api_handler, listener,
					MHD_OPTION_END);
		}
	}

	mosquitto_FREE(x509_cert);
	mosquitto_FREE(x509_ಕೀಲಿ);

	ಯದಿ(listener->mhd){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Opening http api listen socket on port %d.", port);
		ಯದಿ(listener->http_dir){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Using http_dir %s", listener->http_dir);
		}
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
	}
}


ಶೂನ್ಯ http_api__stop(ಸಂರಚನಾ mosquitto__listener *listener)
{
	MHD_stop_daemon(listener->mhd);
	listener->mhd = ರಿಕ್ತ;
}

#endif
