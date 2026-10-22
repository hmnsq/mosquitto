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

#अन्तर्भाव "config.h"

#ifdef WITH_HTTP_API

#अन्तर्भाव <assert.h>
#अन्तर्भाव <errno.h>
#अन्तर्भाव <limits.h>
#अन्तर्भाव <microhttpd.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <sys/stat.h>
#ifndef WIN32
#  include <netdb.h>
#endif

#अन्तर्भाव "json_help.h"
#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "sys_tree.h"

#ifndef HTTP_API_DIR
#  define HTTP_API_DIR ""
#endif

संरचना metric {
	int64_t current;
	int64_t next;
	ध्रुव वर्ण *topic, *topic_alias;
	bool is_max;
};

time_t broker_uptime(शून्य);
संरचना MHD_Daemon *mhd = रिक्त;

#ifdef WITH_SYS_वृक्षः
बाह्य संरचना metric metrics[mosq_metric_max];
#endif

#define HTTP_RESPONSE_BUFLEN 10000

#ifdef WIN32
#define DIR_SEP '\\'
#define PATH_MAX MAX_PATH
#अञ्ञथा
#define DIR_SEP '/'
#endif

#ifndef S_ISREG
#define S_ISREG(mode) (((mode)&S_IFMT) == S_IFREG)
#endif


स्थिर वर्ण *http__canonical_filename(
		ध्रुव वर्ण *url,
		ध्रुव वर्ण *http_dir,
		पूर्णाङ्क *error_code)
{
	size_t urllen, slen;
	वर्ण *filename, *filename_canonical;

	urllen = strlen(url);
	सचे(url[urllen-1] == '/'){
		slen = strlen(http_dir) + urllen + strlen("/index.html") + 2;
	}अञ्ञथा{
		slen = strlen(http_dir) + urllen + 2;
	}
	filename = mosquitto_malloc(slen);
	सचे(!filename){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		पटिनिवत्त रिक्त;
	}
	सचे(((वर्ण *)url)[urllen-1] == '/'){
		snprintf(filename, slen, "%s%c%sindex.html", http_dir, DIR_SEP, url);
	}अञ्ञथा{
		snprintf(filename, slen, "%s%c%s", http_dir, DIR_SEP, url);
	}


	/* Get canonical path and check it is within our http_dir */
	filename_canonical = mosquitto_calloc(1, PATH_MAX);
	सचे(!filename_canonical){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		पटिनिवत्त रिक्त;
	}
#ifdef WIN32
	वर्ण *resolved = _fullpath(filename_canonical, filename, PATH_MAX);
	mosquitto_FREE(filename);
	सचे(!resolved){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		mosquitto_FREE(filename_canonical);
		पटिनिवत्त रिक्त;
	}
#अञ्ञथा
	वर्ण *resolved = realpath(filename, filename_canonical);
	mosquitto_FREE(filename);
	सचे(!resolved){
		सचे(errno == EACCES){
			*error_code = MHD_HTTP_FORBIDDEN;
		}अञ्ञथा सचे(errno == EINVAL || errno == EIO || errno == ELOOP){
			*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		}अञ्ञथा सचे(errno == ENAMETOOLONG){
			*error_code = MHD_HTTP_URI_TOO_LONG;
		}अञ्ञथा सचे(errno == ENOENT || errno == ENOTDIR){
			*error_code = MHD_HTTP_NOT_FOUND;
		}
		mosquitto_FREE(filename_canonical);
		पटिनिवत्त रिक्त;
	}
#endif
	सचे(strncmp(http_dir, filename_canonical, strlen(http_dir))){
		/* Requested file isn't within http_dir, deny access because it's not found. */
		mosquitto_FREE(filename_canonical);
		*error_code = MHD_HTTP_NOT_FOUND;
		पटिनिवत्त रिक्त;
	}

	पटिनिवत्त filename_canonical;
}

स्थिर गणना MHD_Result http_api__send_error_response(संरचना MHD_Connection *connection, ध्रुव वर्ण *error_str, unsigned पूर्णाङ्क error_code)
{
	संरचना MHD_Response *response = MHD_create_response_from_buffer(strlen(error_str), (शून्य *)error_str, MHD_RESPMEM_MUST_COPY);
	गणना MHD_Result ret = MHD_queue_response(connection, error_code, response);
	MHD_destroy_response(response);
	पटिनिवत्त ret;
}

स्थिर गणना MHD_Result http_api__send_response_with_headers(संरचना MHD_Connection *connection, ध्रुव वर्ण *buf)
{
	गणना MHD_Result ret;
	संरचना MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (शून्य *)buf, MHD_RESPMEM_MUST_COPY);

	ret = MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
	सचे(ret != MHD_YES){
		MHD_destroy_response(response);
		पटिनिवत्त ret;
	}
	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	पटिनिवत्त ret;
}

स्थिर गणना MHD_Result http_api__process_version(संरचना MHD_Connection *connection)
{
	पटिनिवत्त http_api__send_response_with_headers(connection, VERSION);
}

स्थिर गणना MHD_Result http_api__process_listeners(संरचना MHD_Connection *connection)
{
	वर्ण *buf;
	गणना MHD_Result ret;

	cJSON *j_वृक्षः = cJSON_CreateObject();
	सचे(!j_वृक्षः){
		पटिनिवत्त http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	cJSON *j_listeners = cJSON_AddArrayToObject(j_वृक्षः, "listeners");
	सचे(!j_listeners){
		cJSON_Delete(j_वृक्षः);
		पटिनिवत्त http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	अत्थाय(पूर्णाङ्क i=0; i<db.config->listener_गणनम्; i++){
		cJSON *j_listener = cJSON_CreateObject();
		सचे(!j_listener){
			cJSON_Delete(j_वृक्षः);
			पटिनिवत्त http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
		}
		cJSON_AddItemToArray(j_listeners, j_listener);

		संरचना mosquitto__listener *listener = &db.config->listeners[i];
#ifdef WITH_UNIX_SOCKETS
		सचे(listener->unix_socket_path){
			cJSON_AddStringToObject(j_listener, "path", listener->unix_socket_path);
		}अञ्ञथा
#endif
		{
			cJSON_AddIntToObject(j_listener, "port", listener->port);
		}

		विकल्प(listener->protocol){
			पक्ष mp_mqtt:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt");
				भङ्ग;
			पक्ष mp_mqttsn:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt-sn");
				भङ्ग;
			पक्ष mp_websockets:
				cJSON_AddStringToObject(j_listener, "protocol", "websockets");
				भङ्ग;
			पक्ष mp_http_api:
				cJSON_AddStringToObject(j_listener, "protocol", "httpapi");
				भङ्ग;
		}

#ifdef WITH_TLS
		cJSON_AddBoolToObject(j_listener, "tls", listener->certfile && listener->keyfile);
		cJSON_AddBoolToObject(j_listener, "mtls", listener->require_certificate);
#अञ्ञथा
		cJSON_AddBoolToObject(j_listener, "tls", false);
		cJSON_AddBoolToObject(j_listener, "mtls", false);
#endif
		सचे(listener->security_options->allow_anonymous == -1){
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", db.config->security_options.allow_anonymous);
		}अञ्ञथा{
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", listener->security_options->allow_anonymous);
		}
	}
	buf = cJSON_Print(j_वृक्षः);
	cJSON_Delete(j_वृक्षः);
	सचे(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		मुक्त(buf);
	}अञ्ञथा{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	पटिनिवत्त ret;
}


स्थिर गणना MHD_Result http_api__process_systree(संरचना MHD_Connection *connection)
{
	गणना MHD_Result ret;
#ifdef WITH_SYS_वृक्षः
	वर्ण *buf;

	cJSON *j_वृक्षः = cJSON_CreateObject();
	अत्थाय(पूर्णाङ्क i=0; i<mosq_metric_max; i++){
		सचे(metrics[i].topic){
			cJSON_AddIntToObject(j_वृक्षः, metrics[i].topic, metrics[i].current);
		}
	}
	cJSON_AddIntToObject(j_वृक्षः, "$SYS/broker/uptime", broker_uptime());

	buf = cJSON_Print(j_वृक्षः);
	cJSON_Delete(j_वृक्षः);

	सचे(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		मुक्त(buf);
	}अञ्ञथा{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}
#अञ्ञथा
	ret = http_api__send_error_response(connection, "Not found.\n", 404);
#endif

	पटिनिवत्त ret;
}


स्थिर ssize_t http_file_read_cb(शून्य *cls, uint64_t pos, वर्ण *buf, size_t max)
{
	FILE *fptr = cls;
	सचे(fseek(fptr, (long )pos, SEEK_SET) < 0){
		पटिनिवत्त -1;
	}
	पटिनिवत्त (ssize_t )fread(buf, 1, max, fptr);
}


स्थिर शून्य http_file_free_cb(शून्य *cls)
{
	FILE *fptr = cls;
	fclose(fptr);
}


स्थिर गणना MHD_Result http_api__process_file(संरचना mosquitto__listener *listener, संरचना MHD_Connection *connection, ध्रुव वर्ण *url)
{
	पूर्णाङ्क error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
	वर्ण *canonical_filename;

	सचे(!listener || !listener->http_dir){
		http_api__send_error_response(connection, "Not found.\n", 404);
		पटिनिवत्त MHD_YES;
	}

	canonical_filename = http__canonical_filename(url, listener->http_dir, &error_code);
	सचे(!canonical_filename){
		http_api__send_error_response(connection, "Not found.\n", 404);
		पटिनिवत्त MHD_YES;
	}

	FILE *fptr = fopen(canonical_filename, "rb");
	mosquitto_FREE(canonical_filename);
	सचे(!fptr){
		http_api__send_error_response(connection, "Not found.\n", 404);
		पटिनिवत्त MHD_YES;
	}

	संरचना stat statbuf;
	सचे(fstat(fileno(fptr), &statbuf)){
		fclose(fptr);
		http_api__send_error_response(connection, "Internal server error.\n", 500);
		पटिनिवत्त MHD_YES;
	}

	सचे(!S_ISREG(statbuf.st_mode)){
		fclose(fptr);
		http_api__send_error_response(connection, "Not found.\n", 404);
		पटिनिवत्त MHD_YES;
	}
	uint64_t flen = (uint64_t )statbuf.st_size;

	/* Using MHD_create_response_from_fd would be easier here, but is less portable */
	संरचना MHD_Response *response = MHD_create_response_from_callback(
			flen,
			65536,
			http_file_read_cb,
			fptr,
			http_file_free_cb);

	गणना MHD_Result ret = MHD_queue_response(connection, 200, response);
	MHD_destroy_response(response);
	पटिनिवत्त ret;
}


स्थिर गणना MHD_Result http_api__process_api(संरचना MHD_Connection *connection, ध्रुव वर्ण *url)
{
	सचे(strcmp(url, "/api/v1/systree") == 0){
		पटिनिवत्त http_api__process_systree(connection);
	}अञ्ञथा सचे(strcmp(url, "/api/v1/listeners") == 0){
		पटिनिवत्त http_api__process_listeners(connection);
	}अञ्ञथा सचे(strcmp(url, "/api/v1/version") == 0){
		पटिनिवत्त http_api__process_version(connection);
	}अञ्ञथा{
		पटिनिवत्त http_api__send_error_response(connection, "Not found.\n", 404);
	}
}


स्थिर पूर्णाङ्क check_access(संरचना mosquitto__listener *listener, संरचना MHD_Connection *connection, ध्रुव वर्ण *url)
{
	संरचना mosquitto context = {0};
	पूर्णाङ्क auth_rc, acl_rc = MOSQ_ERR_SUCCESS;

	context.listener = listener;

	context.id = (वर्ण *)"http-api";
	context.username = MHD_basic_auth_get_username_password(connection, &context.password);

	/* Authentication */
	auth_rc = mosquitto_basic_auth(&context);
	सचे(auth_rc == MOSQ_ERR_SUCCESS){
		acl_rc = mosquitto_acl_check(&context, url, 0, रिक्त, 0, false, रिक्त, MOSQ_ACL_READ);
	}
	MHD_free(context.username);
	MHD_free(context.password);

	सचे(auth_rc || acl_rc){
		ध्रुव वर्ण *buf = "Not authorised\n";
		संरचना MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (शून्य *)buf, MHD_RESPMEM_MUST_COPY);
		MHD_queue_basic_auth_fail_response(connection, "Mosquitto API", response);
		MHD_destroy_response(response);

		पटिनिवत्त MOSQ_ERR_AUTH;
	}

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}

स्थिर गणना MHD_Result http_api_handler(शून्य *cls, संरचना MHD_Connection
		*connection, ध्रुव वर्ण *url, ध्रुव वर्ण *method, ध्रुव वर्ण *version,
		ध्रुव वर्ण *upload_data, size_t *upload_data_size, शून्य **con_cls)
{
	UNUSED(version);
	UNUSED(upload_data);
	UNUSED(upload_data_size);
	UNUSED(con_cls);

	संरचना mosquitto__listener *listener = cls;

	सचे(strcmp(method, "GET") != 0){
		वर्ण *buf = "Invalid HTTP Method\n";
		संरचना MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), (शून्य *)buf, MHD_RESPMEM_MUST_COPY);
		गणना MHD_Result ret = MHD_queue_response(connection, MHD_HTTP_METHOD_NOT_ALLOWED, response);
		MHD_destroy_response(response);
		पटिनिवत्त ret;
	}

	सचे(check_access(listener, connection, url) != MOSQ_ERR_SUCCESS){
		पटिनिवत्त MHD_YES;
	}

	सचे(!strncasecmp(url, "/api/", strlen("/api/"))){
		पटिनिवत्त http_api__process_api(connection, url);
	}अञ्ञथा{
		पटिनिवत्त http_api__process_file(listener, connection, url);
	}
}


पूर्णाङ्क http_api__start_local(संरचना mosquitto__listener *listener)
{
	listener->security_options = mosquitto_calloc(1, परिमाण(संरचना mosquitto__security_options));
	सचे(listener->security_options == रिक्त){
		पटिनिवत्त MOSQ_ERR_NOMEM;
	}

	listener->host = mosquitto_strdup("127.0.0.1");
	सचे(!listener->host){
		mosquitto_FREE(listener->security_options);
		पटिनिवत्त MOSQ_ERR_NOMEM;
	}
	listener->port = 9883;

	सचे(db.config->security_options.allow_anonymous == -1){
		listener->security_options->allow_anonymous = true;
	}अञ्ञथा{
		listener->security_options->allow_anonymous = db.config->security_options.allow_anonymous;
	}

	सचे(listener->http_dir == रिक्त && strlen(HTTP_API_DIR) > 0){
#ifdef WIN32
		वर्ण *http_dir_canonical = _fullpath(रिक्त, HTTP_API_DIR, 0);
#अञ्ञथा
		वर्ण *http_dir_canonical = realpath(HTTP_API_DIR, रिक्त);
#endif
		सचे(!http_dir_canonical){
			पटिनिवत्त MOSQ_ERR_NOMEM;
		}
		mosquitto_FREE(listener->http_dir);
		listener->http_dir = mosquitto_strdup(http_dir_canonical);
		mosquitto_FREE(http_dir_canonical);
	}

	पटिनिवत्त http_api__start(listener);
}


पूर्णाङ्क http_api__start(संरचना mosquitto__listener *listener)
{
	unsigned पूर्णाङ्क flags = MHD_USE_AUTO_INTERNAL_THREAD;
	ध्रुव वर्ण *bind_address;
	uint16_t port = 9883;
	वर्ण *x509_cert = रिक्त;
	वर्ण *x509_कुञ्चिका = रिक्त;

	सचे(!listener->security_options){
		listener->security_options = mosquitto_calloc(1, परिमाण(संरचना mosquitto__security_options));
		सचे(listener->security_options == रिक्त){
			पटिनिवत्त MOSQ_ERR_NOMEM;
		}
	}
	listener->protocol = mp_http_api;

	bind_address = listener->host;
	port = listener->port;

#ifdef WITH_TLS
	सचे(listener->certfile && listener->keyfile){
		सचे(mosquitto_read_file(listener->certfile, false, &x509_cert, रिक्त)){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
			पटिनिवत्त MOSQ_ERR_INVAL;
		}
		सचे(mosquitto_read_file(listener->keyfile, false, &x509_कुञ्चिका, रिक्त)){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			mosquitto_FREE(x509_cert);
			पटिनिवत्त MOSQ_ERR_INVAL;
		}
		flags |= MHD_USE_TLS;
	}
#endif

	सचे(bind_address){
		वर्ण service[10];
		संरचना addrinfo hints;
		संरचना addrinfo *ainfo, *rp;

		snprintf(service, परिमाण(service), "%d", port);

		memset(&hints, 0, परिमाण(संरचना addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = AI_PASSIVE;
		hints.ai_socktype = SOCK_STREAM;

		पूर्णाङ्क rc = getaddrinfo(bind_address, service, &hints, &ainfo);
		सचे(rc){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Unable to start http api listener (%s).", strerror(errno));
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_कुञ्चिका);
			पटिनिवत्त MOSQ_ERR_ERRNO;
		}

		अत्थाय(rp = ainfo; rp; rp = rp->ai_next){
			सचे(rp->ai_family == AF_INET || rp->ai_family == AF_INET6){
				भङ्ग;
			}
		}
		सचे(!rp){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Unable to start http api listener, could not find address.");
			freeaddrinfo(ainfo);
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_कुञ्चिका);

			पटिनिवत्त MOSQ_ERR_INVAL;
		}

		सचे(rp->ai_family == AF_INET6){
			flags |= MHD_USE_IPv6;
		}
		सचे(x509_cert && x509_कुञ्चिका){
			listener->mhd = MHD_start_daemon(flags, port, रिक्त, रिक्त, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_कुञ्चिका, x509_कुञ्चिका,
					MHD_OPTION_END);
		}अञ्ञथा{
			listener->mhd = MHD_start_daemon(flags, port, रिक्त, रिक्त, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_END);
		}
		freeaddrinfo(ainfo);
	}अञ्ञथा{
		सचे(x509_cert && x509_कुञ्चिका){
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_राशिः, port, रिक्त, रिक्त, &http_api_handler, listener,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_कुञ्चिका, x509_कुञ्चिका,
					MHD_OPTION_END);
		}अञ्ञथा{
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_राशिः, port, रिक्त, रिक्त, &http_api_handler, listener,
					MHD_OPTION_END);
		}
	}

	mosquitto_FREE(x509_cert);
	mosquitto_FREE(x509_कुञ्चिका);

	सचे(listener->mhd){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Opening http api listen socket on port %d.", port);
		सचे(listener->http_dir){
			log__printf(रिक्त, MOSQ_LOG_INFO, "Using http_dir %s", listener->http_dir);
		}
		पटिनिवत्त MOSQ_ERR_SUCCESS;
	}अञ्ञथा{
		पटिनिवत्त MOSQ_ERR_UNKNOWN;
	}
}


शून्य http_api__stop(संरचना mosquitto__listener *listener)
{
	MHD_stop_daemon(listener->mhd);
	listener->mhd = रिक्त;
}

#endif
