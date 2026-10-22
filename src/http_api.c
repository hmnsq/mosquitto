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

#અન્તર્ભાવ "config.h"

#ifdef WITH_HTTP_API

#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <errno.h>
#અન્તર્ભાવ <limits.h>
#અન્તર્ભાવ <microhttpd.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <sys/stat.h>
#ifndef WIN32
#  include <netdb.h>
#endif

#અન્તર્ભાવ "json_help.h"
#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"
#અન્તર્ભાવ "sys_tree.h"

#ifndef HTTP_API_DIR
#  define HTTP_API_DIR ""
#endif

સંરચના metric {
	int64_t current;
	int64_t next;
	અચલ અક્ષર *topic, *topic_alias;
	bool is_max;
};

time_t broker_uptime(શૂન્ય);
સંરચના MHD_Daemon *mhd = ખાલી;

#ifdef WITH_SYS_વૃક્ષ
બાહ્ય સંરચના metric metrics[mosq_metric_max];
#endif

#define HTTP_RESPONSE_BUFLEN 10000

#ifdef WIN32
#define DIR_SEP '\\'
#define PATH_MAX MAX_PATH
#નહીંતર
#define DIR_SEP '/'
#endif

#ifndef S_ISREG
#define S_ISREG(mode) (((mode)&S_IFMT) == S_IFREG)
#endif


સ્થિર અક્ષર *http__canonical_filename(
		અચલ અક્ષર *url,
		અચલ અક્ષર *http_dir,
		પૂર્ણાંક *error_code)
{
	size_t urllen, slen;
	અક્ષર *filename, *filename_canonical;

	urllen = strlen(url);
	જો(url[urllen-1] == '/'){
		slen = strlen(http_dir) + urllen + strlen("/index.html") + 2;
	}નહીંતર{
		slen = strlen(http_dir) + urllen + 2;
	}
	filename = mosquitto_malloc(slen);
	જો(!filename){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		પાછા ખાલી;
	}
	જો(((અક્ષર *)url)[urllen-1] == '/'){
		snprintf(filename, slen, "%s%c%sindex.html", http_dir, DIR_SEP, url);
	}નહીંતર{
		snprintf(filename, slen, "%s%c%s", http_dir, DIR_SEP, url);
	}


	/* Get canonical path and check it is within our http_dir */
	filename_canonical = mosquitto_calloc(1, PATH_MAX);
	જો(!filename_canonical){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		પાછા ખાલી;
	}
#ifdef WIN32
	અક્ષર *resolved = _fullpath(filename_canonical, filename, PATH_MAX);
	mosquitto_FREE(filename);
	જો(!resolved){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		mosquitto_FREE(filename_canonical);
		પાછા ખાલી;
	}
#નહીંતર
	અક્ષર *resolved = realpath(filename, filename_canonical);
	mosquitto_FREE(filename);
	જો(!resolved){
		જો(errno == EACCES){
			*error_code = MHD_HTTP_FORBIDDEN;
		}નહીંતર જો(errno == EINVAL || errno == EIO || errno == ELOOP){
			*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		}નહીંતર જો(errno == ENAMETOOLONG){
			*error_code = MHD_HTTP_URI_TOO_LONG;
		}નહીંતર જો(errno == ENOENT || errno == ENOTDIR){
			*error_code = MHD_HTTP_NOT_FOUND;
		}
		mosquitto_FREE(filename_canonical);
		પાછા ખાલી;
	}
#endif
	જો(strncmp(http_dir, filename_canonical, strlen(http_dir))){
		/* Requested file isn't within http_dir, deny access because it's not found. */
		mosquitto_FREE(filename_canonical);
		*error_code = MHD_HTTP_NOT_FOUND;
		પાછા ખાલી;
	}

	પાછા filename_canonical;
}

સ્થિર ગણના MHD_Result http_api__send_error_response(સંરચના MHD_Connection *connection, અચલ અક્ષર *error_str, unsigned પૂર્ણાંક error_code)
{
	સંરચના MHD_Response *response = MHD_create_response_from_buffer(strlen(error_str), (શૂન્ય *)error_str, MHD_RESPMEM_MUST_COPY);
	ગણના MHD_Result ret = MHD_queue_response(connection, error_code, response);
	MHD_destroy_response(response);
	પાછા ret;
}

સ્થિર ગણના MHD_Result http_api__send_response_with_headers(સંરચના MHD_Connection *connection, અચલ અક્ષર *buf)
{
	ગણના MHD_Result ret;
	સંરચના MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (શૂન્ય *)buf, MHD_RESPMEM_MUST_COPY);

	ret = MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
	જો(ret != MHD_YES){
		MHD_destroy_response(response);
		પાછા ret;
	}
	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	પાછા ret;
}

સ્થિર ગણના MHD_Result http_api__process_version(સંરચના MHD_Connection *connection)
{
	પાછા http_api__send_response_with_headers(connection, VERSION);
}

સ્થિર ગણના MHD_Result http_api__process_listeners(સંરચના MHD_Connection *connection)
{
	અક્ષર *buf;
	ગણના MHD_Result ret;

	cJSON *j_વૃક્ષ = cJSON_CreateObject();
	જો(!j_વૃક્ષ){
		પાછા http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	cJSON *j_listeners = cJSON_AddArrayToObject(j_વૃક્ષ, "listeners");
	જો(!j_listeners){
		cJSON_Delete(j_વૃક્ષ);
		પાછા http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	માટે(પૂર્ણાંક i=0; i<db.config->listener_ગણતરી; i++){
		cJSON *j_listener = cJSON_CreateObject();
		જો(!j_listener){
			cJSON_Delete(j_વૃક્ષ);
			પાછા http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
		}
		cJSON_AddItemToArray(j_listeners, j_listener);

		સંરચના mosquitto__listener *listener = &db.config->listeners[i];
#ifdef WITH_UNIX_SOCKETS
		જો(listener->unix_socket_path){
			cJSON_AddStringToObject(j_listener, "path", listener->unix_socket_path);
		}નહીંતર
#endif
		{
			cJSON_AddIntToObject(j_listener, "port", listener->port);
		}

		પસંદગી(listener->protocol){
			સ્થિતિ mp_mqtt:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt");
				તોડો;
			સ્થિતિ mp_mqttsn:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt-sn");
				તોડો;
			સ્થિતિ mp_websockets:
				cJSON_AddStringToObject(j_listener, "protocol", "websockets");
				તોડો;
			સ્થિતિ mp_http_api:
				cJSON_AddStringToObject(j_listener, "protocol", "httpapi");
				તોડો;
		}

#ifdef WITH_TLS
		cJSON_AddBoolToObject(j_listener, "tls", listener->certfile && listener->keyfile);
		cJSON_AddBoolToObject(j_listener, "mtls", listener->require_certificate);
#નહીંતર
		cJSON_AddBoolToObject(j_listener, "tls", false);
		cJSON_AddBoolToObject(j_listener, "mtls", false);
#endif
		જો(listener->security_options->allow_anonymous == -1){
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", db.config->security_options.allow_anonymous);
		}નહીંતર{
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", listener->security_options->allow_anonymous);
		}
	}
	buf = cJSON_Print(j_વૃક્ષ);
	cJSON_Delete(j_વૃક્ષ);
	જો(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		મુક્ત(buf);
	}નહીંતર{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	પાછા ret;
}


સ્થિર ગણના MHD_Result http_api__process_systree(સંરચના MHD_Connection *connection)
{
	ગણના MHD_Result ret;
#ifdef WITH_SYS_વૃક્ષ
	અક્ષર *buf;

	cJSON *j_વૃક્ષ = cJSON_CreateObject();
	માટે(પૂર્ણાંક i=0; i<mosq_metric_max; i++){
		જો(metrics[i].topic){
			cJSON_AddIntToObject(j_વૃક્ષ, metrics[i].topic, metrics[i].current);
		}
	}
	cJSON_AddIntToObject(j_વૃક્ષ, "$SYS/broker/uptime", broker_uptime());

	buf = cJSON_Print(j_વૃક્ષ);
	cJSON_Delete(j_વૃક્ષ);

	જો(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		મુક્ત(buf);
	}નહીંતર{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}
#નહીંતર
	ret = http_api__send_error_response(connection, "Not found.\n", 404);
#endif

	પાછા ret;
}


સ્થિર ssize_t http_file_read_cb(શૂન્ય *cls, uint64_t pos, અક્ષર *buf, size_t max)
{
	FILE *fptr = cls;
	જો(fseek(fptr, (long )pos, SEEK_SET) < 0){
		પાછા -1;
	}
	પાછા (ssize_t )fread(buf, 1, max, fptr);
}


સ્થિર શૂન્ય http_file_free_cb(શૂન્ય *cls)
{
	FILE *fptr = cls;
	fclose(fptr);
}


સ્થિર ગણના MHD_Result http_api__process_file(સંરચના mosquitto__listener *listener, સંરચના MHD_Connection *connection, અચલ અક્ષર *url)
{
	પૂર્ણાંક error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
	અક્ષર *canonical_filename;

	જો(!listener || !listener->http_dir){
		http_api__send_error_response(connection, "Not found.\n", 404);
		પાછા MHD_YES;
	}

	canonical_filename = http__canonical_filename(url, listener->http_dir, &error_code);
	જો(!canonical_filename){
		http_api__send_error_response(connection, "Not found.\n", 404);
		પાછા MHD_YES;
	}

	FILE *fptr = fopen(canonical_filename, "rb");
	mosquitto_FREE(canonical_filename);
	જો(!fptr){
		http_api__send_error_response(connection, "Not found.\n", 404);
		પાછા MHD_YES;
	}

	સંરચના stat statbuf;
	જો(fstat(fileno(fptr), &statbuf)){
		fclose(fptr);
		http_api__send_error_response(connection, "Internal server error.\n", 500);
		પાછા MHD_YES;
	}

	જો(!S_ISREG(statbuf.st_mode)){
		fclose(fptr);
		http_api__send_error_response(connection, "Not found.\n", 404);
		પાછા MHD_YES;
	}
	uint64_t flen = (uint64_t )statbuf.st_size;

	/* Using MHD_create_response_from_fd would be easier here, but is less portable */
	સંરચના MHD_Response *response = MHD_create_response_from_callback(
			flen,
			65536,
			http_file_read_cb,
			fptr,
			http_file_free_cb);

	ગણના MHD_Result ret = MHD_queue_response(connection, 200, response);
	MHD_destroy_response(response);
	પાછા ret;
}


સ્થિર ગણના MHD_Result http_api__process_api(સંરચના MHD_Connection *connection, અચલ અક્ષર *url)
{
	જો(strcmp(url, "/api/v1/systree") == 0){
		પાછા http_api__process_systree(connection);
	}નહીંતર જો(strcmp(url, "/api/v1/listeners") == 0){
		પાછા http_api__process_listeners(connection);
	}નહીંતર જો(strcmp(url, "/api/v1/version") == 0){
		પાછા http_api__process_version(connection);
	}નહીંતર{
		પાછા http_api__send_error_response(connection, "Not found.\n", 404);
	}
}


સ્થિર પૂર્ણાંક check_access(સંરચના mosquitto__listener *listener, સંરચના MHD_Connection *connection, અચલ અક્ષર *url)
{
	સંરચના mosquitto context = {0};
	પૂર્ણાંક auth_rc, acl_rc = MOSQ_ERR_SUCCESS;

	context.listener = listener;

	context.id = (અક્ષર *)"http-api";
	context.username = MHD_basic_auth_get_username_password(connection, &context.password);

	/* Authentication */
	auth_rc = mosquitto_basic_auth(&context);
	જો(auth_rc == MOSQ_ERR_SUCCESS){
		acl_rc = mosquitto_acl_check(&context, url, 0, ખાલી, 0, false, ખાલી, MOSQ_ACL_READ);
	}
	MHD_free(context.username);
	MHD_free(context.password);

	જો(auth_rc || acl_rc){
		અચલ અક્ષર *buf = "Not authorised\n";
		સંરચના MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (શૂન્ય *)buf, MHD_RESPMEM_MUST_COPY);
		MHD_queue_basic_auth_fail_response(connection, "Mosquitto API", response);
		MHD_destroy_response(response);

		પાછા MOSQ_ERR_AUTH;
	}

	પાછા MOSQ_ERR_SUCCESS;
}

સ્થિર ગણના MHD_Result http_api_handler(શૂન્ય *cls, સંરચના MHD_Connection
		*connection, અચલ અક્ષર *url, અચલ અક્ષર *method, અચલ અક્ષર *version,
		અચલ અક્ષર *upload_data, size_t *upload_data_size, શૂન્ય **con_cls)
{
	UNUSED(version);
	UNUSED(upload_data);
	UNUSED(upload_data_size);
	UNUSED(con_cls);

	સંરચના mosquitto__listener *listener = cls;

	જો(strcmp(method, "GET") != 0){
		અક્ષર *buf = "Invalid HTTP Method\n";
		સંરચના MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (શૂન્ય *)buf, MHD_RESPMEM_MUST_COPY);
		ગણના MHD_Result ret = MHD_queue_response(connection, MHD_HTTP_METHOD_NOT_ALLOWED, response);
		MHD_destroy_response(response);
		પાછા ret;
	}

	જો(check_access(listener, connection, url) != MOSQ_ERR_SUCCESS){
		પાછા MHD_YES;
	}

	જો(!strncasecmp(url, "/api/", strlen("/api/"))){
		પાછા http_api__process_api(connection, url);
	}નહીંતર{
		પાછા http_api__process_file(listener, connection, url);
	}
}


પૂર્ણાંક http_api__start_local(સંરચના mosquitto__listener *listener)
{
	listener->security_options = mosquitto_calloc(1, કદ(સંરચના mosquitto__security_options));
	જો(listener->security_options == ખાલી){
		પાછા MOSQ_ERR_NOMEM;
	}

	listener->host = mosquitto_strdup("127.0.0.1");
	જો(!listener->host){
		mosquitto_FREE(listener->security_options);
		પાછા MOSQ_ERR_NOMEM;
	}
	listener->port = 9883;

	જો(db.config->security_options.allow_anonymous == -1){
		listener->security_options->allow_anonymous = true;
	}નહીંતર{
		listener->security_options->allow_anonymous = db.config->security_options.allow_anonymous;
	}

	જો(listener->http_dir == ખાલી && strlen(HTTP_API_DIR) > 0){
#ifdef WIN32
		અક્ષર *http_dir_canonical = _fullpath(ખાલી, HTTP_API_DIR, 0);
#નહીંતર
		અક્ષર *http_dir_canonical = realpath(HTTP_API_DIR, ખાલી);
#endif
		જો(!http_dir_canonical){
			પાછા MOSQ_ERR_NOMEM;
		}
		mosquitto_FREE(listener->http_dir);
		listener->http_dir = mosquitto_strdup(http_dir_canonical);
		mosquitto_FREE(http_dir_canonical);
	}

	પાછા http_api__start(listener);
}


પૂર્ણાંક http_api__start(સંરચના mosquitto__listener *listener)
{
	unsigned પૂર્ણાંક flags = MHD_USE_AUTO_INTERNAL_THREAD;
	અચલ અક્ષર *bind_address;
	uint16_t port = 9883;
	અક્ષર *x509_cert = ખાલી;
	અક્ષર *x509_ચાવી = ખાલી;

	જો(!listener->security_options){
		listener->security_options = mosquitto_calloc(1, કદ(સંરચના mosquitto__security_options));
		જો(listener->security_options == ખાલી){
			પાછા MOSQ_ERR_NOMEM;
		}
	}
	listener->protocol = mp_http_api;

	bind_address = listener->host;
	port = listener->port;

#ifdef WITH_TLS
	જો(listener->certfile && listener->keyfile){
		જો(mosquitto_read_file(listener->certfile, false, &x509_cert, ખાલી)){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
			પાછા MOSQ_ERR_INVAL;
		}
		જો(mosquitto_read_file(listener->keyfile, false, &x509_ચાવી, ખાલી)){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			mosquitto_FREE(x509_cert);
			પાછા MOSQ_ERR_INVAL;
		}
		flags |= MHD_USE_TLS;
	}
#endif

	જો(bind_address){
		અક્ષર service[10];
		સંરચના addrinfo hints;
		સંરચના addrinfo *ainfo, *rp;

		snprintf(service, કદ(service), "%d", port);

		memset(&hints, 0, કદ(સંરચના addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = AI_PASSIVE;
		hints.ai_socktype = SOCK_STREAM;

		પૂર્ણાંક rc = getaddrinfo(bind_address, service, &hints, &ainfo);
		જો(rc){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Unable to start http api listener (%s).", strerror(errno));
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_ચાવી);
			પાછા MOSQ_ERR_ERRNO;
		}

		માટે(rp = ainfo; rp; rp = rp->ai_next){
			જો(rp->ai_family == AF_INET || rp->ai_family == AF_INET6){
				તોડો;
			}
		}
		જો(!rp){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Unable to start http api listener, could not find address.");
			freeaddrinfo(ainfo);
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_ચાવી);

			પાછા MOSQ_ERR_INVAL;
		}

		જો(rp->ai_family == AF_INET6){
			flags |= MHD_USE_IPv6;
		}
		જો(x509_cert && x509_ચાવી){
			listener->mhd = MHD_start_daemon(flags, port, ખાલી, ખાલી, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_ચાવી, x509_ચાવી,
					MHD_OPTION_END);
		}નહીંતર{
			listener->mhd = MHD_start_daemon(flags, port, ખાલી, ખાલી, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_END);
		}
		freeaddrinfo(ainfo);
	}નહીંતર{
		જો(x509_cert && x509_ચાવી){
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_ઢગલો, port, ખાલી, ખાલી, &http_api_handler, listener,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_ચાવી, x509_ચાવી,
					MHD_OPTION_END);
		}નહીંતર{
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_ઢગલો, port, ખાલી, ખાલી, &http_api_handler, listener,
					MHD_OPTION_END);
		}
	}

	mosquitto_FREE(x509_cert);
	mosquitto_FREE(x509_ચાવી);

	જો(listener->mhd){
		log__printf(ખાલી, MOSQ_LOG_INFO, "Opening http api listen socket on port %d.", port);
		જો(listener->http_dir){
			log__printf(ખાલી, MOSQ_LOG_INFO, "Using http_dir %s", listener->http_dir);
		}
		પાછા MOSQ_ERR_SUCCESS;
	}નહીંતર{
		પાછા MOSQ_ERR_UNKNOWN;
	}
}


શૂન્ય http_api__stop(સંરચના mosquitto__listener *listener)
{
	MHD_stop_daemon(listener->mhd);
	listener->mhd = ખાલી;
}

#endif
