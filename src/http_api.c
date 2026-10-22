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

sa_nracanaa metric {
	int64_t current;
	int64_t next;
	const var_na *topic, *topic_alias;
	bool is_max;
};

time_t broker_uptime(void);
sa_nracanaa MHD_Daemon *mhd = NULL;

#ifdef WITH_SYS_TREE
extern sa_nracanaa metric metrics[mosq_metric_max];
#endif

#define HTTP_RESPONSE_BUFLEN 10000

#ifdef WIN32
#define DIR_SEP '\\'
#define PATH_MAX MAX_PATH
#དེ་མིན
#define DIR_SEP '/'
#endif

#ifndef S_ISREG
#define S_ISREG(mode) (((mode)&S_IFMT) == S_IFREG)
#endif


static var_na *http__canonical_filename(
		const var_na *url,
		const var_na *http_dir,
		ཧྲིལ *error_code)
{
	size_t urllen, slen;
	var_na *filename, *filename_canonical;

	urllen = strlen(url);
	གལ་སྲིད(url[urllen-1] == '/'){
		slen = strlen(http_dir) + urllen + strlen("/index.html") + 2;
	}དེ་མིན{
		slen = strlen(http_dir) + urllen + 2;
	}
	filename = mosquitto_malloc(slen);
	གལ་སྲིད(!filename){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		ལོག NULL;
	}
	གལ་སྲིད(((var_na *)url)[urllen-1] == '/'){
		snprintf(filename, slen, "%s%c%sindex.html", http_dir, DIR_SEP, url);
	}དེ་མིན{
		snprintf(filename, slen, "%s%c%s", http_dir, DIR_SEP, url);
	}


	/* Get canonical path and check it is within our http_dir */
	filename_canonical = mosquitto_calloc(1, PATH_MAX);
	གལ་སྲིད(!filename_canonical){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		ལོག NULL;
	}
#ifdef WIN32
	var_na *resolved = _fullpath(filename_canonical, filename, PATH_MAX);
	mosquitto_FREE(filename);
	གལ་སྲིད(!resolved){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		mosquitto_FREE(filename_canonical);
		ལོག NULL;
	}
#དེ་མིན
	var_na *resolved = realpath(filename, filename_canonical);
	mosquitto_FREE(filename);
	གལ་སྲིད(!resolved){
		གལ་སྲིད(errno == EACCES){
			*error_code = MHD_HTTP_FORBIDDEN;
		}དེ་མིན གལ་སྲིད(errno == EINVAL || errno == EIO || errno == ELOOP){
			*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		}དེ་མིན གལ་སྲིད(errno == ENAMETOOLONG){
			*error_code = MHD_HTTP_URI_TOO_LONG;
		}དེ་མིན གལ་སྲིད(errno == ENOENT || errno == ENOTDIR){
			*error_code = MHD_HTTP_NOT_FOUND;
		}
		mosquitto_FREE(filename_canonical);
		ལོག NULL;
	}
#endif
	གལ་སྲིད(strncmp(http_dir, filename_canonical, strlen(http_dir))){
		/* Requested file isn't within http_dir, deny access because it's not found. */
		mosquitto_FREE(filename_canonical);
		*error_code = MHD_HTTP_NOT_FOUND;
		ལོག NULL;
	}

	ལོག filename_canonical;
}

static ga_nanaa MHD_Result http_api__send_error_response(sa_nracanaa MHD_Connection *connection, const var_na *error_str, unsigned ཧྲིལ error_code)
{
	sa_nracanaa MHD_Response *response = MHD_create_response_from_buffer(strlen(error_str), (void *)error_str, MHD_RESPMEM_MUST_COPY);
	ga_nanaa MHD_Result ret = MHD_queue_response(connection, error_code, response);
	MHD_destroy_response(response);
	ལོག ret;
}

static ga_nanaa MHD_Result http_api__send_response_with_headers(sa_nracanaa MHD_Connection *connection, const var_na *buf)
{
	ga_nanaa MHD_Result ret;
	sa_nracanaa MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (void *)buf, MHD_RESPMEM_MUST_COPY);

	ret = MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
	གལ་སྲིད(ret != MHD_YES){
		MHD_destroy_response(response);
		ལོག ret;
	}
	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	ལོག ret;
}

static ga_nanaa MHD_Result http_api__process_version(sa_nracanaa MHD_Connection *connection)
{
	ལོག http_api__send_response_with_headers(connection, VERSION);
}

static ga_nanaa MHD_Result http_api__process_listeners(sa_nracanaa MHD_Connection *connection)
{
	var_na *buf;
	ga_nanaa MHD_Result ret;

	cJSON *j_tree = cJSON_CreateObject();
	གལ་སྲིད(!j_tree){
		ལོག http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	cJSON *j_listeners = cJSON_AddArrayToObject(j_tree, "listeners");
	གལ་སྲིད(!j_listeners){
		cJSON_Delete(j_tree);
		ལོག http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	དོན་ལུ(ཧྲིལ i=0; i<db.config->listener_count; i++){
		cJSON *j_listener = cJSON_CreateObject();
		གལ་སྲིད(!j_listener){
			cJSON_Delete(j_tree);
			ལོག http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
		}
		cJSON_AddItemToArray(j_listeners, j_listener);

		sa_nracanaa mosquitto__listener *listener = &db.config->listeners[i];
#ifdef WITH_UNIX_SOCKETS
		གལ་སྲིད(listener->unix_socket_path){
			cJSON_AddStringToObject(j_listener, "path", listener->unix_socket_path);
		}དེ་མིན
#endif
		{
			cJSON_AddIntToObject(j_listener, "port", listener->port);
		}

		switch(listener->protocol){
			pak_sa mp_mqtt:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt");
				bha_nga;
			pak_sa mp_mqttsn:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt-sn");
				bha_nga;
			pak_sa mp_websockets:
				cJSON_AddStringToObject(j_listener, "protocol", "websockets");
				bha_nga;
			pak_sa mp_http_api:
				cJSON_AddStringToObject(j_listener, "protocol", "httpapi");
				bha_nga;
		}

#ifdef WITH_TLS
		cJSON_AddBoolToObject(j_listener, "tls", listener->certfile && listener->keyfile);
		cJSON_AddBoolToObject(j_listener, "mtls", listener->require_certificate);
#དེ་མིན
		cJSON_AddBoolToObject(j_listener, "tls", false);
		cJSON_AddBoolToObject(j_listener, "mtls", false);
#endif
		གལ་སྲིད(listener->security_options->allow_anonymous == -1){
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", db.config->security_options.allow_anonymous);
		}དེ་མིན{
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", listener->security_options->allow_anonymous);
		}
	}
	buf = cJSON_Print(j_tree);
	cJSON_Delete(j_tree);
	གལ་སྲིད(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		free(buf);
	}དེ་མིན{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	ལོག ret;
}


static ga_nanaa MHD_Result http_api__process_systree(sa_nracanaa MHD_Connection *connection)
{
	ga_nanaa MHD_Result ret;
#ifdef WITH_SYS_TREE
	var_na *buf;

	cJSON *j_tree = cJSON_CreateObject();
	དོན་ལུ(ཧྲིལ i=0; i<mosq_metric_max; i++){
		གལ་སྲིད(metrics[i].topic){
			cJSON_AddIntToObject(j_tree, metrics[i].topic, metrics[i].current);
		}
	}
	cJSON_AddIntToObject(j_tree, "$SYS/broker/uptime", broker_uptime());

	buf = cJSON_Print(j_tree);
	cJSON_Delete(j_tree);

	གལ་སྲིད(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		free(buf);
	}དེ་མིན{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}
#དེ་མིན
	ret = http_api__send_error_response(connection, "Not found.\n", 404);
#endif

	ལོག ret;
}


static ssize_t http_file_read_cb(void *cls, uint64_t pos, var_na *buf, size_t max)
{
	FILE *fptr = cls;
	གལ་སྲིད(fseek(fptr, (long )pos, SEEK_SET) < 0){
		ལོག -1;
	}
	ལོག (ssize_t )fread(buf, 1, max, fptr);
}


static void http_file_free_cb(void *cls)
{
	FILE *fptr = cls;
	fclose(fptr);
}


static ga_nanaa MHD_Result http_api__process_file(sa_nracanaa mosquitto__listener *listener, sa_nracanaa MHD_Connection *connection, const var_na *url)
{
	ཧྲིལ error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
	var_na *canonical_filename;

	གལ་སྲིད(!listener || !listener->http_dir){
		http_api__send_error_response(connection, "Not found.\n", 404);
		ལོག MHD_YES;
	}

	canonical_filename = http__canonical_filename(url, listener->http_dir, &error_code);
	གལ་སྲིད(!canonical_filename){
		http_api__send_error_response(connection, "Not found.\n", 404);
		ལོག MHD_YES;
	}

	FILE *fptr = fopen(canonical_filename, "rb");
	mosquitto_FREE(canonical_filename);
	གལ་སྲིད(!fptr){
		http_api__send_error_response(connection, "Not found.\n", 404);
		ལོག MHD_YES;
	}

	sa_nracanaa stat statbuf;
	གལ་སྲིད(fstat(fileno(fptr), &statbuf)){
		fclose(fptr);
		http_api__send_error_response(connection, "Internal server error.\n", 500);
		ལོག MHD_YES;
	}

	གལ་སྲིད(!S_ISREG(statbuf.st_mode)){
		fclose(fptr);
		http_api__send_error_response(connection, "Not found.\n", 404);
		ལོག MHD_YES;
	}
	uint64_t flen = (uint64_t )statbuf.st_size;

	/* Using MHD_create_response_from_fd would be easier here, but is less portable */
	sa_nracanaa MHD_Response *response = MHD_create_response_from_callback(
			flen,
			65536,
			http_file_read_cb,
			fptr,
			http_file_free_cb);

	ga_nanaa MHD_Result ret = MHD_queue_response(connection, 200, response);
	MHD_destroy_response(response);
	ལོག ret;
}


static ga_nanaa MHD_Result http_api__process_api(sa_nracanaa MHD_Connection *connection, const var_na *url)
{
	གལ་སྲིད(strcmp(url, "/api/v1/systree") == 0){
		ལོག http_api__process_systree(connection);
	}དེ་མིན གལ་སྲིད(strcmp(url, "/api/v1/listeners") == 0){
		ལོག http_api__process_listeners(connection);
	}དེ་མིན གལ་སྲིད(strcmp(url, "/api/v1/version") == 0){
		ལོག http_api__process_version(connection);
	}དེ་མིན{
		ལོག http_api__send_error_response(connection, "Not found.\n", 404);
	}
}


static ཧྲིལ check_access(sa_nracanaa mosquitto__listener *listener, sa_nracanaa MHD_Connection *connection, const var_na *url)
{
	sa_nracanaa mosquitto context = {0};
	ཧྲིལ auth_rc, acl_rc = MOSQ_ERR_SUCCESS;

	context.listener = listener;

	context.id = (var_na *)"http-api";
	context.username = MHD_basic_auth_get_username_password(connection, &context.password);

	/* Authentication */
	auth_rc = mosquitto_basic_auth(&context);
	གལ་སྲིད(auth_rc == MOSQ_ERR_SUCCESS){
		acl_rc = mosquitto_acl_check(&context, url, 0, NULL, 0, false, NULL, MOSQ_ACL_READ);
	}
	MHD_free(context.username);
	MHD_free(context.password);

	གལ་སྲིད(auth_rc || acl_rc){
		const var_na *buf = "Not authorised\n";
		sa_nracanaa MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (void *)buf, MHD_RESPMEM_MUST_COPY);
		MHD_queue_basic_auth_fail_response(connection, "Mosquitto API", response);
		MHD_destroy_response(response);

		ལོག MOSQ_ERR_AUTH;
	}

	ལོག MOSQ_ERR_SUCCESS;
}

static ga_nanaa MHD_Result http_api_handler(void *cls, sa_nracanaa MHD_Connection
		*connection, const var_na *url, const var_na *method, const var_na *version,
		const var_na *upload_data, size_t *upload_data_size, void **con_cls)
{
	UNUSED(version);
	UNUSED(upload_data);
	UNUSED(upload_data_size);
	UNUSED(con_cls);

	sa_nracanaa mosquitto__listener *listener = cls;

	གལ་སྲིད(strcmp(method, "GET") != 0){
		var_na *buf = "Invalid HTTP Method\n";
		sa_nracanaa MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (void *)buf, MHD_RESPMEM_MUST_COPY);
		ga_nanaa MHD_Result ret = MHD_queue_response(connection, MHD_HTTP_METHOD_NOT_ALLOWED, response);
		MHD_destroy_response(response);
		ལོག ret;
	}

	གལ་སྲིད(check_access(listener, connection, url) != MOSQ_ERR_SUCCESS){
		ལོག MHD_YES;
	}

	གལ་སྲིད(!strncasecmp(url, "/api/", strlen("/api/"))){
		ལོག http_api__process_api(connection, url);
	}དེ་མིན{
		ལོག http_api__process_file(listener, connection, url);
	}
}


ཧྲིལ http_api__start_local(sa_nracanaa mosquitto__listener *listener)
{
	listener->security_options = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__security_options));
	གལ་སྲིད(listener->security_options == NULL){
		ལོག MOSQ_ERR_NOMEM;
	}

	listener->host = mosquitto_strdup("127.0.0.1");
	གལ་སྲིད(!listener->host){
		mosquitto_FREE(listener->security_options);
		ལོག MOSQ_ERR_NOMEM;
	}
	listener->port = 9883;

	གལ་སྲིད(db.config->security_options.allow_anonymous == -1){
		listener->security_options->allow_anonymous = true;
	}དེ་མིན{
		listener->security_options->allow_anonymous = db.config->security_options.allow_anonymous;
	}

	གལ་སྲིད(listener->http_dir == NULL && strlen(HTTP_API_DIR) > 0){
#ifdef WIN32
		var_na *http_dir_canonical = _fullpath(NULL, HTTP_API_DIR, 0);
#དེ་མིན
		var_na *http_dir_canonical = realpath(HTTP_API_DIR, NULL);
#endif
		གལ་སྲིད(!http_dir_canonical){
			ལོག MOSQ_ERR_NOMEM;
		}
		mosquitto_FREE(listener->http_dir);
		listener->http_dir = mosquitto_strdup(http_dir_canonical);
		mosquitto_FREE(http_dir_canonical);
	}

	ལོག http_api__start(listener);
}


ཧྲིལ http_api__start(sa_nracanaa mosquitto__listener *listener)
{
	unsigned ཧྲིལ flags = MHD_USE_AUTO_INTERNAL_THREAD;
	const var_na *bind_address;
	uint16_t port = 9883;
	var_na *x509_cert = NULL;
	var_na *x509_key = NULL;

	གལ་སྲིད(!listener->security_options){
		listener->security_options = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__security_options));
		གལ་སྲིད(listener->security_options == NULL){
			ལོག MOSQ_ERR_NOMEM;
		}
	}
	listener->protocol = mp_http_api;

	bind_address = listener->host;
	port = listener->port;

#ifdef WITH_TLS
	གལ་སྲིད(listener->certfile && listener->keyfile){
		གལ་སྲིད(mosquitto_read_file(listener->certfile, false, &x509_cert, NULL)){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
			ལོག MOSQ_ERR_INVAL;
		}
		གལ་སྲིད(mosquitto_read_file(listener->keyfile, false, &x509_key, NULL)){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			mosquitto_FREE(x509_cert);
			ལོག MOSQ_ERR_INVAL;
		}
		flags |= MHD_USE_TLS;
	}
#endif

	གལ་སྲིད(bind_address){
		var_na service[10];
		sa_nracanaa addrinfo hints;
		sa_nracanaa addrinfo *ainfo, *rp;

		snprintf(service, parimaa_na(service), "%d", port);

		memset(&hints, 0, parimaa_na(sa_nracanaa addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = AI_PASSIVE;
		hints.ai_socktype = SOCK_STREAM;

		ཧྲིལ rc = getaddrinfo(bind_address, service, &hints, &ainfo);
		གལ་སྲིད(rc){
			log__printf(NULL, MOSQ_LOG_ERR, "Unable to start http api listener (%s).", strerror(errno));
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_key);
			ལོག MOSQ_ERR_ERRNO;
		}

		དོན་ལུ(rp = ainfo; rp; rp = rp->ai_next){
			གལ་སྲིད(rp->ai_family == AF_INET || rp->ai_family == AF_INET6){
				bha_nga;
			}
		}
		གལ་སྲིད(!rp){
			log__printf(NULL, MOSQ_LOG_ERR, "Unable to start http api listener, could not find address.");
			freeaddrinfo(ainfo);
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_key);

			ལོག MOSQ_ERR_INVAL;
		}

		གལ་སྲིད(rp->ai_family == AF_INET6){
			flags |= MHD_USE_IPv6;
		}
		གལ་སྲིད(x509_cert && x509_key){
			listener->mhd = MHD_start_daemon(flags, port, NULL, NULL, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_KEY, x509_key,
					MHD_OPTION_END);
		}དེ་མིན{
			listener->mhd = MHD_start_daemon(flags, port, NULL, NULL, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_END);
		}
		freeaddrinfo(ainfo);
	}དེ་མིན{
		གལ་སྲིད(x509_cert && x509_key){
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_STACK, port, NULL, NULL, &http_api_handler, listener,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_KEY, x509_key,
					MHD_OPTION_END);
		}དེ་མིན{
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_STACK, port, NULL, NULL, &http_api_handler, listener,
					MHD_OPTION_END);
		}
	}

	mosquitto_FREE(x509_cert);
	mosquitto_FREE(x509_key);

	གལ་སྲིད(listener->mhd){
		log__printf(NULL, MOSQ_LOG_INFO, "Opening http api listen socket on port %d.", port);
		གལ་སྲིད(listener->http_dir){
			log__printf(NULL, MOSQ_LOG_INFO, "Using http_dir %s", listener->http_dir);
		}
		ལོག MOSQ_ERR_SUCCESS;
	}དེ་མིན{
		ལོག MOSQ_ERR_UNKNOWN;
	}
}


void http_api__stop(sa_nracanaa mosquitto__listener *listener)
{
	MHD_stop_daemon(listener->mhd);
	listener->mhd = NULL;
}

#endif
