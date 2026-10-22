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

#അന്തര്ഭാവ "config.h"

#ifdef WITH_HTTP_API

#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <errno.h>
#അന്തര്ഭാവ <limits.h>
#അന്തര്ഭാവ <microhttpd.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <sys/stat.h>
#ifndef WIN32
#  include <netdb.h>
#endif

#അന്തര്ഭാവ "json_help.h"
#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/mqtt_protocol.h"
#അന്തര്ഭാവ "sys_tree.h"

#ifndef HTTP_API_DIR
#  define HTTP_API_DIR ""
#endif

ഘടന metric {
	int64_t current;
	int64_t next;
	സ്ഥിരം അക്ഷരം *topic, *topic_alias;
	bool is_max;
};

time_t broker_uptime(ശൂന്യം);
ഘടന MHD_Daemon *mhd = ശൂന്യ;

#ifdef WITH_SYS_വൃക്ഷം
ബാഹ്യ ഘടന metric metrics[mosq_metric_max];
#endif

#define HTTP_RESPONSE_BUFLEN 10000

#ifdef WIN32
#define DIR_SEP '\\'
#define PATH_MAX MAX_PATH
#അല്ലെങ്കിൽ
#define DIR_SEP '/'
#endif

#ifndef S_ISREG
#define S_ISREG(mode) (((mode)&S_IFMT) == S_IFREG)
#endif


സ്ഥിതം അക്ഷരം *http__canonical_filename(
		സ്ഥിരം അക്ഷരം *url,
		സ്ഥിരം അക്ഷരം *http_dir,
		പൂർണ്ണസംഖ്യ *error_code)
{
	size_t urllen, slen;
	അക്ഷരം *filename, *filename_canonical;

	urllen = strlen(url);
	എങ്കിൽ(url[urllen-1] == '/'){
		slen = strlen(http_dir) + urllen + strlen("/index.html") + 2;
	}അല്ലെങ്കിൽ{
		slen = strlen(http_dir) + urllen + 2;
	}
	filename = mosquitto_malloc(slen);
	എങ്കിൽ(!filename){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		തിരികെ ശൂന്യ;
	}
	എങ്കിൽ(((അക്ഷരം *)url)[urllen-1] == '/'){
		snprintf(filename, slen, "%s%c%sindex.html", http_dir, DIR_SEP, url);
	}അല്ലെങ്കിൽ{
		snprintf(filename, slen, "%s%c%s", http_dir, DIR_SEP, url);
	}


	/* Get canonical path and check it is within our http_dir */
	filename_canonical = mosquitto_calloc(1, PATH_MAX);
	എങ്കിൽ(!filename_canonical){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		തിരികെ ശൂന്യ;
	}
#ifdef WIN32
	അക്ഷരം *resolved = _fullpath(filename_canonical, filename, PATH_MAX);
	mosquitto_FREE(filename);
	എങ്കിൽ(!resolved){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		mosquitto_FREE(filename_canonical);
		തിരികെ ശൂന്യ;
	}
#അല്ലെങ്കിൽ
	അക്ഷരം *resolved = realpath(filename, filename_canonical);
	mosquitto_FREE(filename);
	എങ്കിൽ(!resolved){
		എങ്കിൽ(errno == EACCES){
			*error_code = MHD_HTTP_FORBIDDEN;
		}അല്ലെങ്കിൽ എങ്കിൽ(errno == EINVAL || errno == EIO || errno == ELOOP){
			*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		}അല്ലെങ്കിൽ എങ്കിൽ(errno == ENAMETOOLONG){
			*error_code = MHD_HTTP_URI_TOO_LONG;
		}അല്ലെങ്കിൽ എങ്കിൽ(errno == ENOENT || errno == ENOTDIR){
			*error_code = MHD_HTTP_NOT_FOUND;
		}
		mosquitto_FREE(filename_canonical);
		തിരികെ ശൂന്യ;
	}
#endif
	എങ്കിൽ(strncmp(http_dir, filename_canonical, strlen(http_dir))){
		/* Requested file isn't within http_dir, deny access because it's not found. */
		mosquitto_FREE(filename_canonical);
		*error_code = MHD_HTTP_NOT_FOUND;
		തിരികെ ശൂന്യ;
	}

	തിരികെ filename_canonical;
}

സ്ഥിതം എണ്ണൽ MHD_Result http_api__send_error_response(ഘടന MHD_Connection *connection, സ്ഥിരം അക്ഷരം *error_str, unsigned പൂർണ്ണസംഖ്യ error_code)
{
	ഘടന MHD_Response *response = MHD_create_response_from_buffer(strlen(error_str), (ശൂന്യം *)error_str, MHD_RESPMEM_MUST_COPY);
	എണ്ണൽ MHD_Result ret = MHD_queue_response(connection, error_code, response);
	MHD_destroy_response(response);
	തിരികെ ret;
}

സ്ഥിതം എണ്ണൽ MHD_Result http_api__send_response_with_headers(ഘടന MHD_Connection *connection, സ്ഥിരം അക്ഷരം *buf)
{
	എണ്ണൽ MHD_Result ret;
	ഘടന MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (ശൂന്യം *)buf, MHD_RESPMEM_MUST_COPY);

	ret = MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
	എങ്കിൽ(ret != MHD_YES){
		MHD_destroy_response(response);
		തിരികെ ret;
	}
	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	തിരികെ ret;
}

സ്ഥിതം എണ്ണൽ MHD_Result http_api__process_version(ഘടന MHD_Connection *connection)
{
	തിരികെ http_api__send_response_with_headers(connection, VERSION);
}

സ്ഥിതം എണ്ണൽ MHD_Result http_api__process_listeners(ഘടന MHD_Connection *connection)
{
	അക്ഷരം *buf;
	എണ്ണൽ MHD_Result ret;

	cJSON *j_വൃക്ഷം = cJSON_CreateObject();
	എങ്കിൽ(!j_വൃക്ഷം){
		തിരികെ http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	cJSON *j_listeners = cJSON_AddArrayToObject(j_വൃക്ഷം, "listeners");
	എങ്കിൽ(!j_listeners){
		cJSON_Delete(j_വൃക്ഷം);
		തിരികെ http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<db.config->listener_എണ്ണം; i++){
		cJSON *j_listener = cJSON_CreateObject();
		എങ്കിൽ(!j_listener){
			cJSON_Delete(j_വൃക്ഷം);
			തിരികെ http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
		}
		cJSON_AddItemToArray(j_listeners, j_listener);

		ഘടന mosquitto__listener *listener = &db.config->listeners[i];
#ifdef WITH_UNIX_SOCKETS
		എങ്കിൽ(listener->unix_socket_path){
			cJSON_AddStringToObject(j_listener, "path", listener->unix_socket_path);
		}അല്ലെങ്കിൽ
#endif
		{
			cJSON_AddIntToObject(j_listener, "port", listener->port);
		}

		തിരഞ്ഞെടുപ്പ്(listener->protocol){
			അവസ്ഥ mp_mqtt:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt");
				നിർത്തുക;
			അവസ്ഥ mp_mqttsn:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt-sn");
				നിർത്തുക;
			അവസ്ഥ mp_websockets:
				cJSON_AddStringToObject(j_listener, "protocol", "websockets");
				നിർത്തുക;
			അവസ്ഥ mp_http_api:
				cJSON_AddStringToObject(j_listener, "protocol", "httpapi");
				നിർത്തുക;
		}

#ifdef WITH_TLS
		cJSON_AddBoolToObject(j_listener, "tls", listener->certfile && listener->keyfile);
		cJSON_AddBoolToObject(j_listener, "mtls", listener->require_certificate);
#അല്ലെങ്കിൽ
		cJSON_AddBoolToObject(j_listener, "tls", false);
		cJSON_AddBoolToObject(j_listener, "mtls", false);
#endif
		എങ്കിൽ(listener->security_options->allow_anonymous == -1){
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", db.config->security_options.allow_anonymous);
		}അല്ലെങ്കിൽ{
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", listener->security_options->allow_anonymous);
		}
	}
	buf = cJSON_Print(j_വൃക്ഷം);
	cJSON_Delete(j_വൃക്ഷം);
	എങ്കിൽ(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		സ്വതന്ത്രം(buf);
	}അല്ലെങ്കിൽ{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	തിരികെ ret;
}


സ്ഥിതം എണ്ണൽ MHD_Result http_api__process_systree(ഘടന MHD_Connection *connection)
{
	എണ്ണൽ MHD_Result ret;
#ifdef WITH_SYS_വൃക്ഷം
	അക്ഷരം *buf;

	cJSON *j_വൃക്ഷം = cJSON_CreateObject();
	വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<mosq_metric_max; i++){
		എങ്കിൽ(metrics[i].topic){
			cJSON_AddIntToObject(j_വൃക്ഷം, metrics[i].topic, metrics[i].current);
		}
	}
	cJSON_AddIntToObject(j_വൃക്ഷം, "$SYS/broker/uptime", broker_uptime());

	buf = cJSON_Print(j_വൃക്ഷം);
	cJSON_Delete(j_വൃക്ഷം);

	എങ്കിൽ(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		സ്വതന്ത്രം(buf);
	}അല്ലെങ്കിൽ{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}
#അല്ലെങ്കിൽ
	ret = http_api__send_error_response(connection, "Not found.\n", 404);
#endif

	തിരികെ ret;
}


സ്ഥിതം ssize_t http_file_read_cb(ശൂന്യം *cls, uint64_t pos, അക്ഷരം *buf, size_t max)
{
	FILE *fptr = cls;
	എങ്കിൽ(fseek(fptr, (long )pos, SEEK_SET) < 0){
		തിരികെ -1;
	}
	തിരികെ (ssize_t )fread(buf, 1, max, fptr);
}


സ്ഥിതം ശൂന്യം http_file_free_cb(ശൂന്യം *cls)
{
	FILE *fptr = cls;
	fclose(fptr);
}


സ്ഥിതം എണ്ണൽ MHD_Result http_api__process_file(ഘടന mosquitto__listener *listener, ഘടന MHD_Connection *connection, സ്ഥിരം അക്ഷരം *url)
{
	പൂർണ്ണസംഖ്യ error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
	അക്ഷരം *canonical_filename;

	എങ്കിൽ(!listener || !listener->http_dir){
		http_api__send_error_response(connection, "Not found.\n", 404);
		തിരികെ MHD_YES;
	}

	canonical_filename = http__canonical_filename(url, listener->http_dir, &error_code);
	എങ്കിൽ(!canonical_filename){
		http_api__send_error_response(connection, "Not found.\n", 404);
		തിരികെ MHD_YES;
	}

	FILE *fptr = fopen(canonical_filename, "rb");
	mosquitto_FREE(canonical_filename);
	എങ്കിൽ(!fptr){
		http_api__send_error_response(connection, "Not found.\n", 404);
		തിരികെ MHD_YES;
	}

	ഘടന stat statbuf;
	എങ്കിൽ(fstat(fileno(fptr), &statbuf)){
		fclose(fptr);
		http_api__send_error_response(connection, "Internal server error.\n", 500);
		തിരികെ MHD_YES;
	}

	എങ്കിൽ(!S_ISREG(statbuf.st_mode)){
		fclose(fptr);
		http_api__send_error_response(connection, "Not found.\n", 404);
		തിരികെ MHD_YES;
	}
	uint64_t flen = (uint64_t )statbuf.st_size;

	/* Using MHD_create_response_from_fd would be easier here, but is less portable */
	ഘടന MHD_Response *response = MHD_create_response_from_callback(
			flen,
			65536,
			http_file_read_cb,
			fptr,
			http_file_free_cb);

	എണ്ണൽ MHD_Result ret = MHD_queue_response(connection, 200, response);
	MHD_destroy_response(response);
	തിരികെ ret;
}


സ്ഥിതം എണ്ണൽ MHD_Result http_api__process_api(ഘടന MHD_Connection *connection, സ്ഥിരം അക്ഷരം *url)
{
	എങ്കിൽ(strcmp(url, "/api/v1/systree") == 0){
		തിരികെ http_api__process_systree(connection);
	}അല്ലെങ്കിൽ എങ്കിൽ(strcmp(url, "/api/v1/listeners") == 0){
		തിരികെ http_api__process_listeners(connection);
	}അല്ലെങ്കിൽ എങ്കിൽ(strcmp(url, "/api/v1/version") == 0){
		തിരികെ http_api__process_version(connection);
	}അല്ലെങ്കിൽ{
		തിരികെ http_api__send_error_response(connection, "Not found.\n", 404);
	}
}


സ്ഥിതം പൂർണ്ണസംഖ്യ check_access(ഘടന mosquitto__listener *listener, ഘടന MHD_Connection *connection, സ്ഥിരം അക്ഷരം *url)
{
	ഘടന mosquitto context = {0};
	പൂർണ്ണസംഖ്യ auth_rc, acl_rc = MOSQ_ERR_SUCCESS;

	context.listener = listener;

	context.id = (അക്ഷരം *)"http-api";
	context.username = MHD_basic_auth_get_username_password(connection, &context.password);

	/* Authentication */
	auth_rc = mosquitto_basic_auth(&context);
	എങ്കിൽ(auth_rc == MOSQ_ERR_SUCCESS){
		acl_rc = mosquitto_acl_check(&context, url, 0, ശൂന്യ, 0, false, ശൂന്യ, MOSQ_ACL_READ);
	}
	MHD_free(context.username);
	MHD_free(context.password);

	എങ്കിൽ(auth_rc || acl_rc){
		സ്ഥിരം അക്ഷരം *buf = "Not authorised\n";
		ഘടന MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (ശൂന്യം *)buf, MHD_RESPMEM_MUST_COPY);
		MHD_queue_basic_auth_fail_response(connection, "Mosquitto API", response);
		MHD_destroy_response(response);

		തിരികെ MOSQ_ERR_AUTH;
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}

സ്ഥിതം എണ്ണൽ MHD_Result http_api_handler(ശൂന്യം *cls, ഘടന MHD_Connection
		*connection, സ്ഥിരം അക്ഷരം *url, സ്ഥിരം അക്ഷരം *method, സ്ഥിരം അക്ഷരം *version,
		സ്ഥിരം അക്ഷരം *upload_data, size_t *upload_data_size, ശൂന്യം **con_cls)
{
	UNUSED(version);
	UNUSED(upload_data);
	UNUSED(upload_data_size);
	UNUSED(con_cls);

	ഘടന mosquitto__listener *listener = cls;

	എങ്കിൽ(strcmp(method, "GET") != 0){
		അക്ഷരം *buf = "Invalid HTTP Method\n";
		ഘടന MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (ശൂന്യം *)buf, MHD_RESPMEM_MUST_COPY);
		എണ്ണൽ MHD_Result ret = MHD_queue_response(connection, MHD_HTTP_METHOD_NOT_ALLOWED, response);
		MHD_destroy_response(response);
		തിരികെ ret;
	}

	എങ്കിൽ(check_access(listener, connection, url) != MOSQ_ERR_SUCCESS){
		തിരികെ MHD_YES;
	}

	എങ്കിൽ(!strncasecmp(url, "/api/", strlen("/api/"))){
		തിരികെ http_api__process_api(connection, url);
	}അല്ലെങ്കിൽ{
		തിരികെ http_api__process_file(listener, connection, url);
	}
}


പൂർണ്ണസംഖ്യ http_api__start_local(ഘടന mosquitto__listener *listener)
{
	listener->security_options = mosquitto_calloc(1, വലുപ്പം(ഘടന mosquitto__security_options));
	എങ്കിൽ(listener->security_options == ശൂന്യ){
		തിരികെ MOSQ_ERR_NOMEM;
	}

	listener->host = mosquitto_strdup("127.0.0.1");
	എങ്കിൽ(!listener->host){
		mosquitto_FREE(listener->security_options);
		തിരികെ MOSQ_ERR_NOMEM;
	}
	listener->port = 9883;

	എങ്കിൽ(db.config->security_options.allow_anonymous == -1){
		listener->security_options->allow_anonymous = true;
	}അല്ലെങ്കിൽ{
		listener->security_options->allow_anonymous = db.config->security_options.allow_anonymous;
	}

	എങ്കിൽ(listener->http_dir == ശൂന്യ && strlen(HTTP_API_DIR) > 0){
#ifdef WIN32
		അക്ഷരം *http_dir_canonical = _fullpath(ശൂന്യ, HTTP_API_DIR, 0);
#അല്ലെങ്കിൽ
		അക്ഷരം *http_dir_canonical = realpath(HTTP_API_DIR, ശൂന്യ);
#endif
		എങ്കിൽ(!http_dir_canonical){
			തിരികെ MOSQ_ERR_NOMEM;
		}
		mosquitto_FREE(listener->http_dir);
		listener->http_dir = mosquitto_strdup(http_dir_canonical);
		mosquitto_FREE(http_dir_canonical);
	}

	തിരികെ http_api__start(listener);
}


പൂർണ്ണസംഖ്യ http_api__start(ഘടന mosquitto__listener *listener)
{
	unsigned പൂർണ്ണസംഖ്യ flags = MHD_USE_AUTO_INTERNAL_THREAD;
	സ്ഥിരം അക്ഷരം *bind_address;
	uint16_t port = 9883;
	അക്ഷരം *x509_cert = ശൂന്യ;
	അക്ഷരം *x509_താക്കോൽ = ശൂന്യ;

	എങ്കിൽ(!listener->security_options){
		listener->security_options = mosquitto_calloc(1, വലുപ്പം(ഘടന mosquitto__security_options));
		എങ്കിൽ(listener->security_options == ശൂന്യ){
			തിരികെ MOSQ_ERR_NOMEM;
		}
	}
	listener->protocol = mp_http_api;

	bind_address = listener->host;
	port = listener->port;

#ifdef WITH_TLS
	എങ്കിൽ(listener->certfile && listener->keyfile){
		എങ്കിൽ(mosquitto_read_file(listener->certfile, false, &x509_cert, ശൂന്യ)){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
			തിരികെ MOSQ_ERR_INVAL;
		}
		എങ്കിൽ(mosquitto_read_file(listener->keyfile, false, &x509_താക്കോൽ, ശൂന്യ)){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			mosquitto_FREE(x509_cert);
			തിരികെ MOSQ_ERR_INVAL;
		}
		flags |= MHD_USE_TLS;
	}
#endif

	എങ്കിൽ(bind_address){
		അക്ഷരം service[10];
		ഘടന addrinfo hints;
		ഘടന addrinfo *ainfo, *rp;

		snprintf(service, വലുപ്പം(service), "%d", port);

		memset(&hints, 0, വലുപ്പം(ഘടന addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = AI_PASSIVE;
		hints.ai_socktype = SOCK_STREAM;

		പൂർണ്ണസംഖ്യ rc = getaddrinfo(bind_address, service, &hints, &ainfo);
		എങ്കിൽ(rc){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Unable to start http api listener (%s).", strerror(errno));
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_താക്കോൽ);
			തിരികെ MOSQ_ERR_ERRNO;
		}

		വേണ്ടി(rp = ainfo; rp; rp = rp->ai_next){
			എങ്കിൽ(rp->ai_family == AF_INET || rp->ai_family == AF_INET6){
				നിർത്തുക;
			}
		}
		എങ്കിൽ(!rp){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Unable to start http api listener, could not find address.");
			freeaddrinfo(ainfo);
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_താക്കോൽ);

			തിരികെ MOSQ_ERR_INVAL;
		}

		എങ്കിൽ(rp->ai_family == AF_INET6){
			flags |= MHD_USE_IPv6;
		}
		എങ്കിൽ(x509_cert && x509_താക്കോൽ){
			listener->mhd = MHD_start_daemon(flags, port, ശൂന്യ, ശൂന്യ, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_താക്കോൽ, x509_താക്കോൽ,
					MHD_OPTION_END);
		}അല്ലെങ്കിൽ{
			listener->mhd = MHD_start_daemon(flags, port, ശൂന്യ, ശൂന്യ, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_END);
		}
		freeaddrinfo(ainfo);
	}അല്ലെങ്കിൽ{
		എങ്കിൽ(x509_cert && x509_താക്കോൽ){
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_അട്ടി, port, ശൂന്യ, ശൂന്യ, &http_api_handler, listener,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_താക്കോൽ, x509_താക്കോൽ,
					MHD_OPTION_END);
		}അല്ലെങ്കിൽ{
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_അട്ടി, port, ശൂന്യ, ശൂന്യ, &http_api_handler, listener,
					MHD_OPTION_END);
		}
	}

	mosquitto_FREE(x509_cert);
	mosquitto_FREE(x509_താക്കോൽ);

	എങ്കിൽ(listener->mhd){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Opening http api listen socket on port %d.", port);
		എങ്കിൽ(listener->http_dir){
			log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Using http_dir %s", listener->http_dir);
		}
		തിരികെ MOSQ_ERR_SUCCESS;
	}അല്ലെങ്കിൽ{
		തിരികെ MOSQ_ERR_UNKNOWN;
	}
}


ശൂന്യം http_api__stop(ഘടന mosquitto__listener *listener)
{
	MHD_stop_daemon(listener->mhd);
	listener->mhd = ശൂന്യ;
}

#endif
