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

#include "config.h"

#ifdef WITH_HTTP_API

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <microhttpd.h>
#include <string.h>
#include <sys/stat.h>
#ifndef WIN32
#  include <netdb.h>
#endif

#include "json_help.h"
#include "mosquitto_broker_internal.h"
#include "mosquitto/mqtt_protocol.h"
#include "sys_tree.h"

#ifndef HTTP_API_DIR
#  define HTTP_API_DIR ""
#endif

struct metric {
	int64_t current;
	int64_t next;
	const char *topic, *topic_alias;
	bool is_max;
};

time_t broker_uptime(void);
struct MHD_Daemon *mhd = NULL;

#ifdef WITH_SYS_TREE
extern struct metric metrics[mosq_metric_max];
#endif

#define HTTP_RESPONSE_BUFLEN 10000

#ifdef WIN32
#define DIR_SEP '\\'
#define PATH_MAX MAX_PATH
#инако
#define DIR_SEP '/'
#endif

#ifndef S_ISREG
#define S_ISREG(mode) (((mode)&S_IFMT) == S_IFREG)
#endif


static char *http__canonical_filename(
		const char *url,
		const char *http_dir,
		цео *error_code)
{
	size_t urllen, slen;
	char *filename, *filename_canonical;

	urllen = strlen(url);
	ако(url[urllen-1] == '/'){
		slen = strlen(http_dir) + urllen + strlen("/index.html") + 2;
	}инако{
		slen = strlen(http_dir) + urllen + 2;
	}
	filename = mosquitto_malloc(slen);
	ако(!filename){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		врати NULL;
	}
	ако(((char *)url)[urllen-1] == '/'){
		snprintf(filename, slen, "%s%c%sindex.html", http_dir, DIR_SEP, url);
	}инако{
		snprintf(filename, slen, "%s%c%s", http_dir, DIR_SEP, url);
	}


	/* Get canonical path and check it is within our http_dir */
	filename_canonical = mosquitto_calloc(1, PATH_MAX);
	ако(!filename_canonical){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		врати NULL;
	}
#ifdef WIN32
	char *resolved = _fullpath(filename_canonical, filename, PATH_MAX);
	mosquitto_FREE(filename);
	ако(!resolved){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		mosquitto_FREE(filename_canonical);
		врати NULL;
	}
#инако
	char *resolved = realpath(filename, filename_canonical);
	mosquitto_FREE(filename);
	ако(!resolved){
		ако(errno == EACCES){
			*error_code = MHD_HTTP_FORBIDDEN;
		}инако ако(errno == EINVAL || errno == EIO || errno == ELOOP){
			*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		}инако ако(errno == ENAMETOOLONG){
			*error_code = MHD_HTTP_URI_TOO_LONG;
		}инако ако(errno == ENOENT || errno == ENOTDIR){
			*error_code = MHD_HTTP_NOT_FOUND;
		}
		mosquitto_FREE(filename_canonical);
		врати NULL;
	}
#endif
	ако(strncmp(http_dir, filename_canonical, strlen(http_dir))){
		/* Requested file isn't within http_dir, deny access because it's not found. */
		mosquitto_FREE(filename_canonical);
		*error_code = MHD_HTTP_NOT_FOUND;
		врати NULL;
	}

	врати filename_canonical;
}

static enum MHD_Result http_api__send_error_response(struct MHD_Connection *connection, const char *error_str, unsigned цео error_code)
{
	struct MHD_Response *response = MHD_create_response_from_buffer(strlen(error_str), (void *)error_str, MHD_RESPMEM_MUST_COPY);
	enum MHD_Result ret = MHD_queue_response(connection, error_code, response);
	MHD_destroy_response(response);
	врати ret;
}

static enum MHD_Result http_api__send_response_with_headers(struct MHD_Connection *connection, const char *buf)
{
	enum MHD_Result ret;
	struct MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (void *)buf, MHD_RESPMEM_MUST_COPY);

	ret = MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
	ако(ret != MHD_YES){
		MHD_destroy_response(response);
		врати ret;
	}
	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	врати ret;
}

static enum MHD_Result http_api__process_version(struct MHD_Connection *connection)
{
	врати http_api__send_response_with_headers(connection, VERSION);
}

static enum MHD_Result http_api__process_listeners(struct MHD_Connection *connection)
{
	char *buf;
	enum MHD_Result ret;

	cJSON *j_tree = cJSON_CreateObject();
	ако(!j_tree){
		врати http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	cJSON *j_listeners = cJSON_AddArrayToObject(j_tree, "listeners");
	ако(!j_listeners){
		cJSON_Delete(j_tree);
		врати http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	за(цео i=0; i<db.config->listener_count; i++){
		cJSON *j_listener = cJSON_CreateObject();
		ако(!j_listener){
			cJSON_Delete(j_tree);
			врати http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
		}
		cJSON_AddItemToArray(j_listeners, j_listener);

		struct mosquitto__listener *listener = &db.config->listeners[i];
#ifdef WITH_UNIX_SOCKETS
		ако(listener->unix_socket_path){
			cJSON_AddStringToObject(j_listener, "path", listener->unix_socket_path);
		}инако
#endif
		{
			cJSON_AddIntToObject(j_listener, "port", listener->port);
		}

		switch(listener->protocol){
			case mp_mqtt:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt");
				break;
			case mp_mqttsn:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt-sn");
				break;
			case mp_websockets:
				cJSON_AddStringToObject(j_listener, "protocol", "websockets");
				break;
			case mp_http_api:
				cJSON_AddStringToObject(j_listener, "protocol", "httpapi");
				break;
		}

#ifdef WITH_TLS
		cJSON_AddBoolToObject(j_listener, "tls", listener->certfile && listener->keyfile);
		cJSON_AddBoolToObject(j_listener, "mtls", listener->require_certificate);
#инако
		cJSON_AddBoolToObject(j_listener, "tls", false);
		cJSON_AddBoolToObject(j_listener, "mtls", false);
#endif
		ако(listener->security_options->allow_anonymous == -1){
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", db.config->security_options.allow_anonymous);
		}инако{
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", listener->security_options->allow_anonymous);
		}
	}
	buf = cJSON_Print(j_tree);
	cJSON_Delete(j_tree);
	ако(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		free(buf);
	}инако{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	врати ret;
}


static enum MHD_Result http_api__process_systree(struct MHD_Connection *connection)
{
	enum MHD_Result ret;
#ifdef WITH_SYS_TREE
	char *buf;

	cJSON *j_tree = cJSON_CreateObject();
	за(цео i=0; i<mosq_metric_max; i++){
		ако(metrics[i].topic){
			cJSON_AddIntToObject(j_tree, metrics[i].topic, metrics[i].current);
		}
	}
	cJSON_AddIntToObject(j_tree, "$SYS/broker/uptime", broker_uptime());

	buf = cJSON_Print(j_tree);
	cJSON_Delete(j_tree);

	ако(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		free(buf);
	}инако{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}
#инако
	ret = http_api__send_error_response(connection, "Not found.\n", 404);
#endif

	врати ret;
}


static ssize_t http_file_read_cb(void *cls, uint64_t pos, char *buf, size_t max)
{
	FILE *fptr = cls;
	ако(fseek(fptr, (long )pos, SEEK_SET) < 0){
		врати -1;
	}
	врати (ssize_t )fread(buf, 1, max, fptr);
}


static void http_file_free_cb(void *cls)
{
	FILE *fptr = cls;
	fclose(fptr);
}


static enum MHD_Result http_api__process_file(struct mosquitto__listener *listener, struct MHD_Connection *connection, const char *url)
{
	цео error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
	char *canonical_filename;

	ако(!listener || !listener->http_dir){
		http_api__send_error_response(connection, "Not found.\n", 404);
		врати MHD_YES;
	}

	canonical_filename = http__canonical_filename(url, listener->http_dir, &error_code);
	ако(!canonical_filename){
		http_api__send_error_response(connection, "Not found.\n", 404);
		врати MHD_YES;
	}

	FILE *fptr = fopen(canonical_filename, "rb");
	mosquitto_FREE(canonical_filename);
	ако(!fptr){
		http_api__send_error_response(connection, "Not found.\n", 404);
		врати MHD_YES;
	}

	struct stat statbuf;
	ако(fstat(fileno(fptr), &statbuf)){
		fclose(fptr);
		http_api__send_error_response(connection, "Internal server error.\n", 500);
		врати MHD_YES;
	}

	ако(!S_ISREG(statbuf.st_mode)){
		fclose(fptr);
		http_api__send_error_response(connection, "Not found.\n", 404);
		врати MHD_YES;
	}
	uint64_t flen = (uint64_t )statbuf.st_size;

	/* Using MHD_create_response_from_fd would be easier here, but is less portable */
	struct MHD_Response *response = MHD_create_response_from_callback(
			flen,
			65536,
			http_file_read_cb,
			fptr,
			http_file_free_cb);

	enum MHD_Result ret = MHD_queue_response(connection, 200, response);
	MHD_destroy_response(response);
	врати ret;
}


static enum MHD_Result http_api__process_api(struct MHD_Connection *connection, const char *url)
{
	ако(strcmp(url, "/api/v1/systree") == 0){
		врати http_api__process_systree(connection);
	}инако ако(strcmp(url, "/api/v1/listeners") == 0){
		врати http_api__process_listeners(connection);
	}инако ако(strcmp(url, "/api/v1/version") == 0){
		врати http_api__process_version(connection);
	}инако{
		врати http_api__send_error_response(connection, "Not found.\n", 404);
	}
}


static цео check_access(struct mosquitto__listener *listener, struct MHD_Connection *connection, const char *url)
{
	struct mosquitto context = {0};
	цео auth_rc, acl_rc = MOSQ_ERR_SUCCESS;

	context.listener = listener;

	context.id = (char *)"http-api";
	context.username = MHD_basic_auth_get_username_password(connection, &context.password);

	/* Authentication */
	auth_rc = mosquitto_basic_auth(&context);
	ако(auth_rc == MOSQ_ERR_SUCCESS){
		acl_rc = mosquitto_acl_check(&context, url, 0, NULL, 0, false, NULL, MOSQ_ACL_READ);
	}
	MHD_free(context.username);
	MHD_free(context.password);

	ако(auth_rc || acl_rc){
		const char *buf = "Not authorised\n";
		struct MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (void *)buf, MHD_RESPMEM_MUST_COPY);
		MHD_queue_basic_auth_fail_response(connection, "Mosquitto API", response);
		MHD_destroy_response(response);

		врати MOSQ_ERR_AUTH;
	}

	врати MOSQ_ERR_SUCCESS;
}

static enum MHD_Result http_api_handler(void *cls, struct MHD_Connection
		*connection, const char *url, const char *method, const char *version,
		const char *upload_data, size_t *upload_data_size, void **con_cls)
{
	UNUSED(version);
	UNUSED(upload_data);
	UNUSED(upload_data_size);
	UNUSED(con_cls);

	struct mosquitto__listener *listener = cls;

	ако(strcmp(method, "GET") != 0){
		char *buf = "Invalid HTTP Method\n";
		struct MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (void *)buf, MHD_RESPMEM_MUST_COPY);
		enum MHD_Result ret = MHD_queue_response(connection, MHD_HTTP_METHOD_NOT_ALLOWED, response);
		MHD_destroy_response(response);
		врати ret;
	}

	ако(check_access(listener, connection, url) != MOSQ_ERR_SUCCESS){
		врати MHD_YES;
	}

	ако(!strncasecmp(url, "/api/", strlen("/api/"))){
		врати http_api__process_api(connection, url);
	}инако{
		врати http_api__process_file(listener, connection, url);
	}
}


цео http_api__start_local(struct mosquitto__listener *listener)
{
	listener->security_options = mosquitto_calloc(1, sizeof(struct mosquitto__security_options));
	ако(listener->security_options == NULL){
		врати MOSQ_ERR_NOMEM;
	}

	listener->host = mosquitto_strdup("127.0.0.1");
	ако(!listener->host){
		mosquitto_FREE(listener->security_options);
		врати MOSQ_ERR_NOMEM;
	}
	listener->port = 9883;

	ако(db.config->security_options.allow_anonymous == -1){
		listener->security_options->allow_anonymous = true;
	}инако{
		listener->security_options->allow_anonymous = db.config->security_options.allow_anonymous;
	}

	ако(listener->http_dir == NULL && strlen(HTTP_API_DIR) > 0){
#ifdef WIN32
		char *http_dir_canonical = _fullpath(NULL, HTTP_API_DIR, 0);
#инако
		char *http_dir_canonical = realpath(HTTP_API_DIR, NULL);
#endif
		ако(!http_dir_canonical){
			врати MOSQ_ERR_NOMEM;
		}
		mosquitto_FREE(listener->http_dir);
		listener->http_dir = mosquitto_strdup(http_dir_canonical);
		mosquitto_FREE(http_dir_canonical);
	}

	врати http_api__start(listener);
}


цео http_api__start(struct mosquitto__listener *listener)
{
	unsigned цео flags = MHD_USE_AUTO_INTERNAL_THREAD;
	const char *bind_address;
	uint16_t port = 9883;
	char *x509_cert = NULL;
	char *x509_key = NULL;

	ако(!listener->security_options){
		listener->security_options = mosquitto_calloc(1, sizeof(struct mosquitto__security_options));
		ако(listener->security_options == NULL){
			врати MOSQ_ERR_NOMEM;
		}
	}
	listener->protocol = mp_http_api;

	bind_address = listener->host;
	port = listener->port;

#ifdef WITH_TLS
	ако(listener->certfile && listener->keyfile){
		ако(mosquitto_read_file(listener->certfile, false, &x509_cert, NULL)){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
			врати MOSQ_ERR_INVAL;
		}
		ако(mosquitto_read_file(listener->keyfile, false, &x509_key, NULL)){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			mosquitto_FREE(x509_cert);
			врати MOSQ_ERR_INVAL;
		}
		flags |= MHD_USE_TLS;
	}
#endif

	ако(bind_address){
		char service[10];
		struct addrinfo hints;
		struct addrinfo *ainfo, *rp;

		snprintf(service, sizeof(service), "%d", port);

		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = AI_PASSIVE;
		hints.ai_socktype = SOCK_STREAM;

		цео rc = getaddrinfo(bind_address, service, &hints, &ainfo);
		ако(rc){
			log__printf(NULL, MOSQ_LOG_ERR, "Unable to start http api listener (%s).", strerror(errno));
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_key);
			врати MOSQ_ERR_ERRNO;
		}

		за(rp = ainfo; rp; rp = rp->ai_next){
			ако(rp->ai_family == AF_INET || rp->ai_family == AF_INET6){
				break;
			}
		}
		ако(!rp){
			log__printf(NULL, MOSQ_LOG_ERR, "Unable to start http api listener, could not find address.");
			freeaddrinfo(ainfo);
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_key);

			врати MOSQ_ERR_INVAL;
		}

		ако(rp->ai_family == AF_INET6){
			flags |= MHD_USE_IPv6;
		}
		ако(x509_cert && x509_key){
			listener->mhd = MHD_start_daemon(flags, port, NULL, NULL, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_KEY, x509_key,
					MHD_OPTION_END);
		}инако{
			listener->mhd = MHD_start_daemon(flags, port, NULL, NULL, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_END);
		}
		freeaddrinfo(ainfo);
	}инако{
		ако(x509_cert && x509_key){
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_STACK, port, NULL, NULL, &http_api_handler, listener,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_KEY, x509_key,
					MHD_OPTION_END);
		}инако{
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_STACK, port, NULL, NULL, &http_api_handler, listener,
					MHD_OPTION_END);
		}
	}

	mosquitto_FREE(x509_cert);
	mosquitto_FREE(x509_key);

	ако(listener->mhd){
		log__printf(NULL, MOSQ_LOG_INFO, "Opening http api listen socket on port %d.", port);
		ако(listener->http_dir){
			log__printf(NULL, MOSQ_LOG_INFO, "Using http_dir %s", listener->http_dir);
		}
		врати MOSQ_ERR_SUCCESS;
	}инако{
		врати MOSQ_ERR_UNKNOWN;
	}
}


void http_api__stop(struct mosquitto__listener *listener)
{
	MHD_stop_daemon(listener->mhd);
	listener->mhd = NULL;
}

#endif
