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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ifdef WITH_HTTP_API

#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <errno.h>
#ଅନ୍ତର୍ଭାଵ <limits.h>
#ଅନ୍ତର୍ଭାଵ <microhttpd.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>
#ifndef WIN32
#  include <netdb.h>
#endif

#ଅନ୍ତର୍ଭାଵ "json_help.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"
#ଅନ୍ତର୍ଭାଵ "sys_tree.h"

#ifndef HTTP_API_DIR
#  define HTTP_API_DIR ""
#endif

ସଂରଚନା metric {
	int64_t current;
	int64_t next;
	ଅଚଳ ଅକ୍ଷର *topic, *topic_alias;
	bool is_max;
};

time_t broker_uptime(ଶୂନ୍ୟ);
ସଂରଚନା MHD_Daemon *mhd = ଖାଲି;

#ifdef WITH_SYS_ବୃକ୍ଷ
ବାହ୍ୟ ସଂରଚନା metric metrics[mosq_metric_max];
#endif

#define HTTP_RESPONSE_BUFLEN 10000

#ifdef WIN32
#define DIR_SEP '\\'
#define PATH_MAX MAX_PATH
#ଅନ୍ୟଥା
#define DIR_SEP '/'
#endif

#ifndef S_ISREG
#define S_ISREG(mode) (((mode)&S_IFMT) == S_IFREG)
#endif


ସ୍ଥିର ଅକ୍ଷର *http__canonical_filename(
		ଅଚଳ ଅକ୍ଷର *url,
		ଅଚଳ ଅକ୍ଷର *http_dir,
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା *error_code)
{
	size_t urllen, slen;
	ଅକ୍ଷର *filename, *filename_canonical;

	urllen = strlen(url);
	ଯଦି(url[urllen-1] == '/'){
		slen = strlen(http_dir) + urllen + strlen("/index.html") + 2;
	}ଅନ୍ୟଥା{
		slen = strlen(http_dir) + urllen + 2;
	}
	filename = mosquitto_malloc(slen);
	ଯଦି(!filename){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		ଫେରନ୍ତୁ ଖାଲି;
	}
	ଯଦି(((ଅକ୍ଷର *)url)[urllen-1] == '/'){
		snprintf(filename, slen, "%s%c%sindex.html", http_dir, DIR_SEP, url);
	}ଅନ୍ୟଥା{
		snprintf(filename, slen, "%s%c%s", http_dir, DIR_SEP, url);
	}


	/* Get canonical path and check it is within our http_dir */
	filename_canonical = mosquitto_calloc(1, PATH_MAX);
	ଯଦି(!filename_canonical){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		ଫେରନ୍ତୁ ଖାଲି;
	}
#ifdef WIN32
	ଅକ୍ଷର *resolved = _fullpath(filename_canonical, filename, PATH_MAX);
	mosquitto_FREE(filename);
	ଯଦି(!resolved){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		mosquitto_FREE(filename_canonical);
		ଫେରନ୍ତୁ ଖାଲି;
	}
#ଅନ୍ୟଥା
	ଅକ୍ଷର *resolved = realpath(filename, filename_canonical);
	mosquitto_FREE(filename);
	ଯଦି(!resolved){
		ଯଦି(errno == EACCES){
			*error_code = MHD_HTTP_FORBIDDEN;
		}ଅନ୍ୟଥା ଯଦି(errno == EINVAL || errno == EIO || errno == ELOOP){
			*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		}ଅନ୍ୟଥା ଯଦି(errno == ENAMETOOLONG){
			*error_code = MHD_HTTP_URI_TOO_LONG;
		}ଅନ୍ୟଥା ଯଦି(errno == ENOENT || errno == ENOTDIR){
			*error_code = MHD_HTTP_NOT_FOUND;
		}
		mosquitto_FREE(filename_canonical);
		ଫେରନ୍ତୁ ଖାଲି;
	}
#endif
	ଯଦି(strncmp(http_dir, filename_canonical, strlen(http_dir))){
		/* Requested file isn't within http_dir, deny access because it's not found. */
		mosquitto_FREE(filename_canonical);
		*error_code = MHD_HTTP_NOT_FOUND;
		ଫେରନ୍ତୁ ଖାଲି;
	}

	ଫେରନ୍ତୁ filename_canonical;
}

ସ୍ଥିର ଗଣନା MHD_Result http_api__send_error_response(ସଂରଚନା MHD_Connection *connection, ଅଚଳ ଅକ୍ଷର *error_str, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା error_code)
{
	ସଂରଚନା MHD_Response *response = MHD_create_response_from_buffer(strlen(error_str), (ଶୂନ୍ୟ *)error_str, MHD_RESPMEM_MUST_COPY);
	ଗଣନା MHD_Result ret = MHD_queue_response(connection, error_code, response);
	MHD_destroy_response(response);
	ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର ଗଣନା MHD_Result http_api__send_response_with_headers(ସଂରଚନା MHD_Connection *connection, ଅଚଳ ଅକ୍ଷର *buf)
{
	ଗଣନା MHD_Result ret;
	ସଂରଚନା MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (ଶୂନ୍ୟ *)buf, MHD_RESPMEM_MUST_COPY);

	ret = MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
	ଯଦି(ret != MHD_YES){
		MHD_destroy_response(response);
		ଫେରନ୍ତୁ ret;
	}
	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର ଗଣନା MHD_Result http_api__process_version(ସଂରଚନା MHD_Connection *connection)
{
	ଫେରନ୍ତୁ http_api__send_response_with_headers(connection, VERSION);
}

ସ୍ଥିର ଗଣନା MHD_Result http_api__process_listeners(ସଂରଚନା MHD_Connection *connection)
{
	ଅକ୍ଷର *buf;
	ଗଣନା MHD_Result ret;

	cJSON *j_ବୃକ୍ଷ = cJSON_CreateObject();
	ଯଦି(!j_ବୃକ୍ଷ){
		ଫେରନ୍ତୁ http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	cJSON *j_listeners = cJSON_AddArrayToObject(j_ବୃକ୍ଷ, "listeners");
	ଯଦି(!j_listeners){
		cJSON_Delete(j_ବୃକ୍ଷ);
		ଫେରନ୍ତୁ http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<db.config->listener_ଗଣନା; i++){
		cJSON *j_listener = cJSON_CreateObject();
		ଯଦି(!j_listener){
			cJSON_Delete(j_ବୃକ୍ଷ);
			ଫେରନ୍ତୁ http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
		}
		cJSON_AddItemToArray(j_listeners, j_listener);

		ସଂରଚନା mosquitto__listener *listener = &db.config->listeners[i];
#ifdef WITH_UNIX_SOCKETS
		ଯଦି(listener->unix_socket_path){
			cJSON_AddStringToObject(j_listener, "path", listener->unix_socket_path);
		}ଅନ୍ୟଥା
#endif
		{
			cJSON_AddIntToObject(j_listener, "port", listener->port);
		}

		ବାଛ(listener->protocol){
			ସ୍ଥିତି mp_mqtt:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt");
				ଭାଙ୍ଗ;
			ସ୍ଥିତି mp_mqttsn:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt-sn");
				ଭାଙ୍ଗ;
			ସ୍ଥିତି mp_websockets:
				cJSON_AddStringToObject(j_listener, "protocol", "websockets");
				ଭାଙ୍ଗ;
			ସ୍ଥିତି mp_http_api:
				cJSON_AddStringToObject(j_listener, "protocol", "httpapi");
				ଭାଙ୍ଗ;
		}

#ifdef WITH_TLS
		cJSON_AddBoolToObject(j_listener, "tls", listener->certfile && listener->keyfile);
		cJSON_AddBoolToObject(j_listener, "mtls", listener->require_certificate);
#ଅନ୍ୟଥା
		cJSON_AddBoolToObject(j_listener, "tls", false);
		cJSON_AddBoolToObject(j_listener, "mtls", false);
#endif
		ଯଦି(listener->security_options->allow_anonymous == -1){
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", db.config->security_options.allow_anonymous);
		}ଅନ୍ୟଥା{
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", listener->security_options->allow_anonymous);
		}
	}
	buf = cJSON_Print(j_ବୃକ୍ଷ);
	cJSON_Delete(j_ବୃକ୍ଷ);
	ଯଦି(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		ମୁକ୍ତ(buf);
	}ଅନ୍ୟଥା{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	ଫେରନ୍ତୁ ret;
}


ସ୍ଥିର ଗଣନା MHD_Result http_api__process_systree(ସଂରଚନା MHD_Connection *connection)
{
	ଗଣନା MHD_Result ret;
#ifdef WITH_SYS_ବୃକ୍ଷ
	ଅକ୍ଷର *buf;

	cJSON *j_ବୃକ୍ଷ = cJSON_CreateObject();
	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<mosq_metric_max; i++){
		ଯଦି(metrics[i].topic){
			cJSON_AddIntToObject(j_ବୃକ୍ଷ, metrics[i].topic, metrics[i].current);
		}
	}
	cJSON_AddIntToObject(j_ବୃକ୍ଷ, "$SYS/broker/uptime", broker_uptime());

	buf = cJSON_Print(j_ବୃକ୍ଷ);
	cJSON_Delete(j_ବୃକ୍ଷ);

	ଯଦି(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		ମୁକ୍ତ(buf);
	}ଅନ୍ୟଥା{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}
#ଅନ୍ୟଥା
	ret = http_api__send_error_response(connection, "Not found.\n", 404);
#endif

	ଫେରନ୍ତୁ ret;
}


ସ୍ଥିର ssize_t http_file_read_cb(ଶୂନ୍ୟ *cls, uint64_t pos, ଅକ୍ଷର *buf, size_t max)
{
	FILE *fptr = cls;
	ଯଦି(fseek(fptr, (long )pos, SEEK_SET) < 0){
		ଫେରନ୍ତୁ -1;
	}
	ଫେରନ୍ତୁ (ssize_t )fread(buf, 1, max, fptr);
}


ସ୍ଥିର ଶୂନ୍ୟ http_file_free_cb(ଶୂନ୍ୟ *cls)
{
	FILE *fptr = cls;
	fclose(fptr);
}


ସ୍ଥିର ଗଣନା MHD_Result http_api__process_file(ସଂରଚନା mosquitto__listener *listener, ସଂରଚନା MHD_Connection *connection, ଅଚଳ ଅକ୍ଷର *url)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
	ଅକ୍ଷର *canonical_filename;

	ଯଦି(!listener || !listener->http_dir){
		http_api__send_error_response(connection, "Not found.\n", 404);
		ଫେରନ୍ତୁ MHD_YES;
	}

	canonical_filename = http__canonical_filename(url, listener->http_dir, &error_code);
	ଯଦି(!canonical_filename){
		http_api__send_error_response(connection, "Not found.\n", 404);
		ଫେରନ୍ତୁ MHD_YES;
	}

	FILE *fptr = fopen(canonical_filename, "rb");
	mosquitto_FREE(canonical_filename);
	ଯଦି(!fptr){
		http_api__send_error_response(connection, "Not found.\n", 404);
		ଫେରନ୍ତୁ MHD_YES;
	}

	ସଂରଚନା stat statbuf;
	ଯଦି(fstat(fileno(fptr), &statbuf)){
		fclose(fptr);
		http_api__send_error_response(connection, "Internal server error.\n", 500);
		ଫେରନ୍ତୁ MHD_YES;
	}

	ଯଦି(!S_ISREG(statbuf.st_mode)){
		fclose(fptr);
		http_api__send_error_response(connection, "Not found.\n", 404);
		ଫେରନ୍ତୁ MHD_YES;
	}
	uint64_t flen = (uint64_t )statbuf.st_size;

	/* Using MHD_create_response_from_fd would be easier here, but is less portable */
	ସଂରଚନା MHD_Response *response = MHD_create_response_from_callback(
			flen,
			65536,
			http_file_read_cb,
			fptr,
			http_file_free_cb);

	ଗଣନା MHD_Result ret = MHD_queue_response(connection, 200, response);
	MHD_destroy_response(response);
	ଫେରନ୍ତୁ ret;
}


ସ୍ଥିର ଗଣନା MHD_Result http_api__process_api(ସଂରଚନା MHD_Connection *connection, ଅଚଳ ଅକ୍ଷର *url)
{
	ଯଦି(strcmp(url, "/api/v1/systree") == 0){
		ଫେରନ୍ତୁ http_api__process_systree(connection);
	}ଅନ୍ୟଥା ଯଦି(strcmp(url, "/api/v1/listeners") == 0){
		ଫେରନ୍ତୁ http_api__process_listeners(connection);
	}ଅନ୍ୟଥା ଯଦି(strcmp(url, "/api/v1/version") == 0){
		ଫେରନ୍ତୁ http_api__process_version(connection);
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ http_api__send_error_response(connection, "Not found.\n", 404);
	}
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_access(ସଂରଚନା mosquitto__listener *listener, ସଂରଚନା MHD_Connection *connection, ଅଚଳ ଅକ୍ଷର *url)
{
	ସଂରଚନା mosquitto context = {0};
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା auth_rc, acl_rc = MOSQ_ERR_SUCCESS;

	context.listener = listener;

	context.id = (ଅକ୍ଷର *)"http-api";
	context.username = MHD_basic_auth_get_username_password(connection, &context.password);

	/* Authentication */
	auth_rc = mosquitto_basic_auth(&context);
	ଯଦି(auth_rc == MOSQ_ERR_SUCCESS){
		acl_rc = mosquitto_acl_check(&context, url, 0, ଖାଲି, 0, false, ଖାଲି, MOSQ_ACL_READ);
	}
	MHD_free(context.username);
	MHD_free(context.password);

	ଯଦି(auth_rc || acl_rc){
		ଅଚଳ ଅକ୍ଷର *buf = "Not authorised\n";
		ସଂରଚନା MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (ଶୂନ୍ୟ *)buf, MHD_RESPMEM_MUST_COPY);
		MHD_queue_basic_auth_fail_response(connection, "Mosquitto API", response);
		MHD_destroy_response(response);

		ଫେରନ୍ତୁ MOSQ_ERR_AUTH;
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}

ସ୍ଥିର ଗଣନା MHD_Result http_api_handler(ଶୂନ୍ୟ *cls, ସଂରଚନା MHD_Connection
		*connection, ଅଚଳ ଅକ୍ଷର *url, ଅଚଳ ଅକ୍ଷର *method, ଅଚଳ ଅକ୍ଷର *version,
		ଅଚଳ ଅକ୍ଷର *upload_data, size_t *upload_data_size, ଶୂନ୍ୟ **con_cls)
{
	UNUSED(version);
	UNUSED(upload_data);
	UNUSED(upload_data_size);
	UNUSED(con_cls);

	ସଂରଚନା mosquitto__listener *listener = cls;

	ଯଦି(strcmp(method, "GET") != 0){
		ଅକ୍ଷର *buf = "Invalid HTTP Method\n";
		ସଂରଚନା MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (ଶୂନ୍ୟ *)buf, MHD_RESPMEM_MUST_COPY);
		ଗଣନା MHD_Result ret = MHD_queue_response(connection, MHD_HTTP_METHOD_NOT_ALLOWED, response);
		MHD_destroy_response(response);
		ଫେରନ୍ତୁ ret;
	}

	ଯଦି(check_access(listener, connection, url) != MOSQ_ERR_SUCCESS){
		ଫେରନ୍ତୁ MHD_YES;
	}

	ଯଦି(!strncasecmp(url, "/api/", strlen("/api/"))){
		ଫେରନ୍ତୁ http_api__process_api(connection, url);
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ http_api__process_file(listener, connection, url);
	}
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା http_api__start_local(ସଂରଚନା mosquitto__listener *listener)
{
	listener->security_options = mosquitto_calloc(1, ଆକାର(ସଂରଚନା mosquitto__security_options));
	ଯଦି(listener->security_options == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	listener->host = mosquitto_strdup("127.0.0.1");
	ଯଦି(!listener->host){
		mosquitto_FREE(listener->security_options);
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}
	listener->port = 9883;

	ଯଦି(db.config->security_options.allow_anonymous == -1){
		listener->security_options->allow_anonymous = true;
	}ଅନ୍ୟଥା{
		listener->security_options->allow_anonymous = db.config->security_options.allow_anonymous;
	}

	ଯଦି(listener->http_dir == ଖାଲି && strlen(HTTP_API_DIR) > 0){
#ifdef WIN32
		ଅକ୍ଷର *http_dir_canonical = _fullpath(ଖାଲି, HTTP_API_DIR, 0);
#ଅନ୍ୟଥା
		ଅକ୍ଷର *http_dir_canonical = realpath(HTTP_API_DIR, ଖାଲି);
#endif
		ଯଦି(!http_dir_canonical){
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}
		mosquitto_FREE(listener->http_dir);
		listener->http_dir = mosquitto_strdup(http_dir_canonical);
		mosquitto_FREE(http_dir_canonical);
	}

	ଫେରନ୍ତୁ http_api__start(listener);
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା http_api__start(ସଂରଚନା mosquitto__listener *listener)
{
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags = MHD_USE_AUTO_INTERNAL_THREAD;
	ଅଚଳ ଅକ୍ଷର *bind_address;
	uint16_t port = 9883;
	ଅକ୍ଷର *x509_cert = ଖାଲି;
	ଅକ୍ଷର *x509_ଚାବି = ଖାଲି;

	ଯଦି(!listener->security_options){
		listener->security_options = mosquitto_calloc(1, ଆକାର(ସଂରଚନା mosquitto__security_options));
		ଯଦି(listener->security_options == ଖାଲି){
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}
	}
	listener->protocol = mp_http_api;

	bind_address = listener->host;
	port = listener->port;

#ifdef WITH_TLS
	ଯଦି(listener->certfile && listener->keyfile){
		ଯଦି(mosquitto_read_file(listener->certfile, false, &x509_cert, ଖାଲି)){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
		ଯଦି(mosquitto_read_file(listener->keyfile, false, &x509_ଚାବି, ଖାଲି)){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			mosquitto_FREE(x509_cert);
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
		flags |= MHD_USE_TLS;
	}
#endif

	ଯଦି(bind_address){
		ଅକ୍ଷର service[10];
		ସଂରଚନା addrinfo hints;
		ସଂରଚନା addrinfo *ainfo, *rp;

		snprintf(service, ଆକାର(service), "%d", port);

		memset(&hints, 0, ଆକାର(ସଂରଚନା addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = AI_PASSIVE;
		hints.ai_socktype = SOCK_STREAM;

		ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = getaddrinfo(bind_address, service, &hints, &ainfo);
		ଯଦି(rc){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Unable to start http api listener (%s).", strerror(errno));
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_ଚାବି);
			ଫେରନ୍ତୁ MOSQ_ERR_ERRNO;
		}

		ପାଇଁ(rp = ainfo; rp; rp = rp->ai_next){
			ଯଦି(rp->ai_family == AF_INET || rp->ai_family == AF_INET6){
				ଭାଙ୍ଗ;
			}
		}
		ଯଦି(!rp){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Unable to start http api listener, could not find address.");
			freeaddrinfo(ainfo);
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_ଚାବି);

			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}

		ଯଦି(rp->ai_family == AF_INET6){
			flags |= MHD_USE_IPv6;
		}
		ଯଦି(x509_cert && x509_ଚାବି){
			listener->mhd = MHD_start_daemon(flags, port, ଖାଲି, ଖାଲି, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_ଚାବି, x509_ଚାବି,
					MHD_OPTION_END);
		}ଅନ୍ୟଥା{
			listener->mhd = MHD_start_daemon(flags, port, ଖାଲି, ଖାଲି, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_END);
		}
		freeaddrinfo(ainfo);
	}ଅନ୍ୟଥା{
		ଯଦି(x509_cert && x509_ଚାବି){
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_ଗଦା, port, ଖାଲି, ଖାଲି, &http_api_handler, listener,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_ଚାବି, x509_ଚାବି,
					MHD_OPTION_END);
		}ଅନ୍ୟଥା{
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_ଗଦା, port, ଖାଲି, ଖାଲି, &http_api_handler, listener,
					MHD_OPTION_END);
		}
	}

	mosquitto_FREE(x509_cert);
	mosquitto_FREE(x509_ଚାବି);

	ଯଦି(listener->mhd){
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "Opening http api listen socket on port %d.", port);
		ଯଦି(listener->http_dir){
			log__printf(ଖାଲି, MOSQ_LOG_INFO, "Using http_dir %s", listener->http_dir);
		}
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
	}
}


ଶୂନ୍ୟ http_api__stop(ସଂରଚନା mosquitto__listener *listener)
{
	MHD_stop_daemon(listener->mhd);
	listener->mhd = ଖାଲି;
}

#endif
