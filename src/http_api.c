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
#ᮜᮤᮚ
#define DIR_SEP '/'
#endif

#ifndef S_ISREG
#define S_ISREG(mode) (((mode)&S_IFMT) == S_IFREG)
#endif


static var_na *http__canonical_filename(
		const var_na *url,
		const var_na *http_dir,
		puur_naa_nka *error_code)
{
	size_t urllen, slen;
	var_na *filename, *filename_canonical;

	urllen = strlen(url);
	ᮚᮦᮔ᮪(url[urllen-1] == '/'){
		slen = strlen(http_dir) + urllen + strlen("/index.html") + 2;
	}ᮜᮤᮚ{
		slen = strlen(http_dir) + urllen + 2;
	}
	filename = mosquitto_malloc(slen);
	ᮚᮦᮔ᮪(!filename){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		ᮝᮜᮤ NULL;
	}
	ᮚᮦᮔ᮪(((var_na *)url)[urllen-1] == '/'){
		snprintf(filename, slen, "%s%c%sindex.html", http_dir, DIR_SEP, url);
	}ᮜᮤᮚ{
		snprintf(filename, slen, "%s%c%s", http_dir, DIR_SEP, url);
	}


	/* Get canonical path and check it is within our http_dir */
	filename_canonical = mosquitto_calloc(1, PATH_MAX);
	ᮚᮦᮔ᮪(!filename_canonical){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		ᮝᮜᮤ NULL;
	}
#ifdef WIN32
	var_na *resolved = _fullpath(filename_canonical, filename, PATH_MAX);
	mosquitto_FREE(filename);
	ᮚᮦᮔ᮪(!resolved){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		mosquitto_FREE(filename_canonical);
		ᮝᮜᮤ NULL;
	}
#ᮜᮤᮚ
	var_na *resolved = realpath(filename, filename_canonical);
	mosquitto_FREE(filename);
	ᮚᮦᮔ᮪(!resolved){
		ᮚᮦᮔ᮪(errno == EACCES){
			*error_code = MHD_HTTP_FORBIDDEN;
		}ᮜᮤᮚ ᮚᮦᮔ᮪(errno == EINVAL || errno == EIO || errno == ELOOP){
			*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		}ᮜᮤᮚ ᮚᮦᮔ᮪(errno == ENAMETOOLONG){
			*error_code = MHD_HTTP_URI_TOO_LONG;
		}ᮜᮤᮚ ᮚᮦᮔ᮪(errno == ENOENT || errno == ENOTDIR){
			*error_code = MHD_HTTP_NOT_FOUND;
		}
		mosquitto_FREE(filename_canonical);
		ᮝᮜᮤ NULL;
	}
#endif
	ᮚᮦᮔ᮪(strncmp(http_dir, filename_canonical, strlen(http_dir))){
		/* Requested file isn't within http_dir, deny access because it's not found. */
		mosquitto_FREE(filename_canonical);
		*error_code = MHD_HTTP_NOT_FOUND;
		ᮝᮜᮤ NULL;
	}

	ᮝᮜᮤ filename_canonical;
}

static ga_nanaa MHD_Result http_api__send_error_response(sa_nracanaa MHD_Connection *connection, const var_na *error_str, unsigned puur_naa_nka error_code)
{
	sa_nracanaa MHD_Response *response = MHD_create_response_from_buffer(strlen(error_str), (void *)error_str, MHD_RESPMEM_MUST_COPY);
	ga_nanaa MHD_Result ret = MHD_queue_response(connection, error_code, response);
	MHD_destroy_response(response);
	ᮝᮜᮤ ret;
}

static ga_nanaa MHD_Result http_api__send_response_with_headers(sa_nracanaa MHD_Connection *connection, const var_na *buf)
{
	ga_nanaa MHD_Result ret;
	sa_nracanaa MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (void *)buf, MHD_RESPMEM_MUST_COPY);

	ret = MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
	ᮚᮦᮔ᮪(ret != MHD_YES){
		MHD_destroy_response(response);
		ᮝᮜᮤ ret;
	}
	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	ᮝᮜᮤ ret;
}

static ga_nanaa MHD_Result http_api__process_version(sa_nracanaa MHD_Connection *connection)
{
	ᮝᮜᮤ http_api__send_response_with_headers(connection, VERSION);
}

static ga_nanaa MHD_Result http_api__process_listeners(sa_nracanaa MHD_Connection *connection)
{
	var_na *buf;
	ga_nanaa MHD_Result ret;

	cJSON *j_tree = cJSON_CreateObject();
	ᮚᮦᮔ᮪(!j_tree){
		ᮝᮜᮤ http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	cJSON *j_listeners = cJSON_AddArrayToObject(j_tree, "listeners");
	ᮚᮦᮔ᮪(!j_listeners){
		cJSON_Delete(j_tree);
		ᮝᮜᮤ http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	for(puur_naa_nka i=0; i<db.config->listener_count; i++){
		cJSON *j_listener = cJSON_CreateObject();
		ᮚᮦᮔ᮪(!j_listener){
			cJSON_Delete(j_tree);
			ᮝᮜᮤ http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
		}
		cJSON_AddItemToArray(j_listeners, j_listener);

		sa_nracanaa mosquitto__listener *listener = &db.config->listeners[i];
#ifdef WITH_UNIX_SOCKETS
		ᮚᮦᮔ᮪(listener->unix_socket_path){
			cJSON_AddStringToObject(j_listener, "path", listener->unix_socket_path);
		}ᮜᮤᮚ
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
#ᮜᮤᮚ
		cJSON_AddBoolToObject(j_listener, "tls", false);
		cJSON_AddBoolToObject(j_listener, "mtls", false);
#endif
		ᮚᮦᮔ᮪(listener->security_options->allow_anonymous == -1){
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", db.config->security_options.allow_anonymous);
		}ᮜᮤᮚ{
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", listener->security_options->allow_anonymous);
		}
	}
	buf = cJSON_Print(j_tree);
	cJSON_Delete(j_tree);
	ᮚᮦᮔ᮪(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		free(buf);
	}ᮜᮤᮚ{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	ᮝᮜᮤ ret;
}


static ga_nanaa MHD_Result http_api__process_systree(sa_nracanaa MHD_Connection *connection)
{
	ga_nanaa MHD_Result ret;
#ifdef WITH_SYS_TREE
	var_na *buf;

	cJSON *j_tree = cJSON_CreateObject();
	for(puur_naa_nka i=0; i<mosq_metric_max; i++){
		ᮚᮦᮔ᮪(metrics[i].topic){
			cJSON_AddIntToObject(j_tree, metrics[i].topic, metrics[i].current);
		}
	}
	cJSON_AddIntToObject(j_tree, "$SYS/broker/uptime", broker_uptime());

	buf = cJSON_Print(j_tree);
	cJSON_Delete(j_tree);

	ᮚᮦᮔ᮪(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		free(buf);
	}ᮜᮤᮚ{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}
#ᮜᮤᮚ
	ret = http_api__send_error_response(connection, "Not found.\n", 404);
#endif

	ᮝᮜᮤ ret;
}


static ssize_t http_file_read_cb(void *cls, uint64_t pos, var_na *buf, size_t max)
{
	FILE *fptr = cls;
	ᮚᮦᮔ᮪(fseek(fptr, (long )pos, SEEK_SET) < 0){
		ᮝᮜᮤ -1;
	}
	ᮝᮜᮤ (ssize_t )fread(buf, 1, max, fptr);
}


static void http_file_free_cb(void *cls)
{
	FILE *fptr = cls;
	fclose(fptr);
}


static ga_nanaa MHD_Result http_api__process_file(sa_nracanaa mosquitto__listener *listener, sa_nracanaa MHD_Connection *connection, const var_na *url)
{
	puur_naa_nka error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
	var_na *canonical_filename;

	ᮚᮦᮔ᮪(!listener || !listener->http_dir){
		http_api__send_error_response(connection, "Not found.\n", 404);
		ᮝᮜᮤ MHD_YES;
	}

	canonical_filename = http__canonical_filename(url, listener->http_dir, &error_code);
	ᮚᮦᮔ᮪(!canonical_filename){
		http_api__send_error_response(connection, "Not found.\n", 404);
		ᮝᮜᮤ MHD_YES;
	}

	FILE *fptr = fopen(canonical_filename, "rb");
	mosquitto_FREE(canonical_filename);
	ᮚᮦᮔ᮪(!fptr){
		http_api__send_error_response(connection, "Not found.\n", 404);
		ᮝᮜᮤ MHD_YES;
	}

	sa_nracanaa stat statbuf;
	ᮚᮦᮔ᮪(fstat(fileno(fptr), &statbuf)){
		fclose(fptr);
		http_api__send_error_response(connection, "Internal server error.\n", 500);
		ᮝᮜᮤ MHD_YES;
	}

	ᮚᮦᮔ᮪(!S_ISREG(statbuf.st_mode)){
		fclose(fptr);
		http_api__send_error_response(connection, "Not found.\n", 404);
		ᮝᮜᮤ MHD_YES;
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
	ᮝᮜᮤ ret;
}


static ga_nanaa MHD_Result http_api__process_api(sa_nracanaa MHD_Connection *connection, const var_na *url)
{
	ᮚᮦᮔ᮪(strcmp(url, "/api/v1/systree") == 0){
		ᮝᮜᮤ http_api__process_systree(connection);
	}ᮜᮤᮚ ᮚᮦᮔ᮪(strcmp(url, "/api/v1/listeners") == 0){
		ᮝᮜᮤ http_api__process_listeners(connection);
	}ᮜᮤᮚ ᮚᮦᮔ᮪(strcmp(url, "/api/v1/version") == 0){
		ᮝᮜᮤ http_api__process_version(connection);
	}ᮜᮤᮚ{
		ᮝᮜᮤ http_api__send_error_response(connection, "Not found.\n", 404);
	}
}


static puur_naa_nka check_access(sa_nracanaa mosquitto__listener *listener, sa_nracanaa MHD_Connection *connection, const var_na *url)
{
	sa_nracanaa mosquitto context = {0};
	puur_naa_nka auth_rc, acl_rc = MOSQ_ERR_SUCCESS;

	context.listener = listener;

	context.id = (var_na *)"http-api";
	context.username = MHD_basic_auth_get_username_password(connection, &context.password);

	/* Authentication */
	auth_rc = mosquitto_basic_auth(&context);
	ᮚᮦᮔ᮪(auth_rc == MOSQ_ERR_SUCCESS){
		acl_rc = mosquitto_acl_check(&context, url, 0, NULL, 0, false, NULL, MOSQ_ACL_READ);
	}
	MHD_free(context.username);
	MHD_free(context.password);

	ᮚᮦᮔ᮪(auth_rc || acl_rc){
		const var_na *buf = "Not authorised\n";
		sa_nracanaa MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (void *)buf, MHD_RESPMEM_MUST_COPY);
		MHD_queue_basic_auth_fail_response(connection, "Mosquitto API", response);
		MHD_destroy_response(response);

		ᮝᮜᮤ MOSQ_ERR_AUTH;
	}

	ᮝᮜᮤ MOSQ_ERR_SUCCESS;
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

	ᮚᮦᮔ᮪(strcmp(method, "GET") != 0){
		var_na *buf = "Invalid HTTP Method\n";
		sa_nracanaa MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (void *)buf, MHD_RESPMEM_MUST_COPY);
		ga_nanaa MHD_Result ret = MHD_queue_response(connection, MHD_HTTP_METHOD_NOT_ALLOWED, response);
		MHD_destroy_response(response);
		ᮝᮜᮤ ret;
	}

	ᮚᮦᮔ᮪(check_access(listener, connection, url) != MOSQ_ERR_SUCCESS){
		ᮝᮜᮤ MHD_YES;
	}

	ᮚᮦᮔ᮪(!strncasecmp(url, "/api/", strlen("/api/"))){
		ᮝᮜᮤ http_api__process_api(connection, url);
	}ᮜᮤᮚ{
		ᮝᮜᮤ http_api__process_file(listener, connection, url);
	}
}


puur_naa_nka http_api__start_local(sa_nracanaa mosquitto__listener *listener)
{
	listener->security_options = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__security_options));
	ᮚᮦᮔ᮪(listener->security_options == NULL){
		ᮝᮜᮤ MOSQ_ERR_NOMEM;
	}

	listener->host = mosquitto_strdup("127.0.0.1");
	ᮚᮦᮔ᮪(!listener->host){
		mosquitto_FREE(listener->security_options);
		ᮝᮜᮤ MOSQ_ERR_NOMEM;
	}
	listener->port = 9883;

	ᮚᮦᮔ᮪(db.config->security_options.allow_anonymous == -1){
		listener->security_options->allow_anonymous = true;
	}ᮜᮤᮚ{
		listener->security_options->allow_anonymous = db.config->security_options.allow_anonymous;
	}

	ᮚᮦᮔ᮪(listener->http_dir == NULL && strlen(HTTP_API_DIR) > 0){
#ifdef WIN32
		var_na *http_dir_canonical = _fullpath(NULL, HTTP_API_DIR, 0);
#ᮜᮤᮚ
		var_na *http_dir_canonical = realpath(HTTP_API_DIR, NULL);
#endif
		ᮚᮦᮔ᮪(!http_dir_canonical){
			ᮝᮜᮤ MOSQ_ERR_NOMEM;
		}
		mosquitto_FREE(listener->http_dir);
		listener->http_dir = mosquitto_strdup(http_dir_canonical);
		mosquitto_FREE(http_dir_canonical);
	}

	ᮝᮜᮤ http_api__start(listener);
}


puur_naa_nka http_api__start(sa_nracanaa mosquitto__listener *listener)
{
	unsigned puur_naa_nka flags = MHD_USE_AUTO_INTERNAL_THREAD;
	const var_na *bind_address;
	uint16_t port = 9883;
	var_na *x509_cert = NULL;
	var_na *x509_key = NULL;

	ᮚᮦᮔ᮪(!listener->security_options){
		listener->security_options = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__security_options));
		ᮚᮦᮔ᮪(listener->security_options == NULL){
			ᮝᮜᮤ MOSQ_ERR_NOMEM;
		}
	}
	listener->protocol = mp_http_api;

	bind_address = listener->host;
	port = listener->port;

#ifdef WITH_TLS
	ᮚᮦᮔ᮪(listener->certfile && listener->keyfile){
		ᮚᮦᮔ᮪(mosquitto_read_file(listener->certfile, false, &x509_cert, NULL)){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
			ᮝᮜᮤ MOSQ_ERR_INVAL;
		}
		ᮚᮦᮔ᮪(mosquitto_read_file(listener->keyfile, false, &x509_key, NULL)){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			mosquitto_FREE(x509_cert);
			ᮝᮜᮤ MOSQ_ERR_INVAL;
		}
		flags |= MHD_USE_TLS;
	}
#endif

	ᮚᮦᮔ᮪(bind_address){
		var_na service[10];
		sa_nracanaa addrinfo hints;
		sa_nracanaa addrinfo *ainfo, *rp;

		snprintf(service, parimaa_na(service), "%d", port);

		memset(&hints, 0, parimaa_na(sa_nracanaa addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = AI_PASSIVE;
		hints.ai_socktype = SOCK_STREAM;

		puur_naa_nka rc = getaddrinfo(bind_address, service, &hints, &ainfo);
		ᮚᮦᮔ᮪(rc){
			log__printf(NULL, MOSQ_LOG_ERR, "Unable to start http api listener (%s).", strerror(errno));
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_key);
			ᮝᮜᮤ MOSQ_ERR_ERRNO;
		}

		for(rp = ainfo; rp; rp = rp->ai_next){
			ᮚᮦᮔ᮪(rp->ai_family == AF_INET || rp->ai_family == AF_INET6){
				bha_nga;
			}
		}
		ᮚᮦᮔ᮪(!rp){
			log__printf(NULL, MOSQ_LOG_ERR, "Unable to start http api listener, could not find address.");
			freeaddrinfo(ainfo);
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_key);

			ᮝᮜᮤ MOSQ_ERR_INVAL;
		}

		ᮚᮦᮔ᮪(rp->ai_family == AF_INET6){
			flags |= MHD_USE_IPv6;
		}
		ᮚᮦᮔ᮪(x509_cert && x509_key){
			listener->mhd = MHD_start_daemon(flags, port, NULL, NULL, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_KEY, x509_key,
					MHD_OPTION_END);
		}ᮜᮤᮚ{
			listener->mhd = MHD_start_daemon(flags, port, NULL, NULL, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_END);
		}
		freeaddrinfo(ainfo);
	}ᮜᮤᮚ{
		ᮚᮦᮔ᮪(x509_cert && x509_key){
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_STACK, port, NULL, NULL, &http_api_handler, listener,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_KEY, x509_key,
					MHD_OPTION_END);
		}ᮜᮤᮚ{
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_STACK, port, NULL, NULL, &http_api_handler, listener,
					MHD_OPTION_END);
		}
	}

	mosquitto_FREE(x509_cert);
	mosquitto_FREE(x509_key);

	ᮚᮦᮔ᮪(listener->mhd){
		log__printf(NULL, MOSQ_LOG_INFO, "Opening http api listen socket on port %d.", port);
		ᮚᮦᮔ᮪(listener->http_dir){
			log__printf(NULL, MOSQ_LOG_INFO, "Using http_dir %s", listener->http_dir);
		}
		ᮝᮜᮤ MOSQ_ERR_SUCCESS;
	}ᮜᮤᮚ{
		ᮝᮜᮤ MOSQ_ERR_UNKNOWN;
	}
}


void http_api__stop(sa_nracanaa mosquitto__listener *listener)
{
	MHD_stop_daemon(listener->mhd);
	listener->mhd = NULL;
}

#endif
