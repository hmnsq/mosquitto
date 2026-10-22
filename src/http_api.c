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
	अचर वर्ण *topic, *topic_alias;
	bool is_max;
};

समय_प्रकार broker_uptime(शून्य);
संरचना MHD_Daemon *mhd = रिक्त;

#ifdef WITH_SYS_रूख
बाह्य संरचना metric metrics[mosq_metric_max];
#endif

#define HTTP_RESPONSE_BUFLEN 10000

#ifdef WIN32
#define DIR_SEP '\\'
#define PATH_MAX MAX_PATH
#अन्यथा
#define DIR_SEP '/'
#endif

#ifndef S_ISREG
#define S_ISREG(mode) (((mode)&S_IFMT) == S_IFREG)
#endif


स्थिर वर्ण *http__canonical_filename(
		अचर वर्ण *url,
		अचर वर्ण *http_dir,
		पूर्णाङ्क *error_code)
{
	माप_प्रकार urllen, slen;
	वर्ण *filename, *filename_canonical;

	urllen = म_माप(url);
	यदि(url[urllen-1] == '/'){
		slen = म_माप(http_dir) + urllen + म_माप("/index.html") + 2;
	}अन्यथा{
		slen = म_माप(http_dir) + urllen + 2;
	}
	filename = mosquitto_malloc(slen);
	यदि(!filename){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		फर्काऊ रिक्त;
	}
	यदि(((वर्ण *)url)[urllen-1] == '/'){
		snprintf(filename, slen, "%s%c%sindex.html", http_dir, DIR_SEP, url);
	}अन्यथा{
		snprintf(filename, slen, "%s%c%s", http_dir, DIR_SEP, url);
	}


	/* Get canonical path and check it is within our http_dir */
	filename_canonical = mosquitto_calloc(1, PATH_MAX);
	यदि(!filename_canonical){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		फर्काऊ रिक्त;
	}
#ifdef WIN32
	वर्ण *resolved = _fullpath(filename_canonical, filename, PATH_MAX);
	mosquitto_FREE(filename);
	यदि(!resolved){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		mosquitto_FREE(filename_canonical);
		फर्काऊ रिक्त;
	}
#अन्यथा
	वर्ण *resolved = realpath(filename, filename_canonical);
	mosquitto_FREE(filename);
	यदि(!resolved){
		यदि(त्रुटि_सं == EACCES){
			*error_code = MHD_HTTP_FORBIDDEN;
		}अन्यथा यदि(त्रुटि_सं == EINVAL || त्रुटि_सं == EIO || त्रुटि_सं == ELOOP){
			*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		}अन्यथा यदि(त्रुटि_सं == ENAMETOOLONG){
			*error_code = MHD_HTTP_URI_TOO_LONG;
		}अन्यथा यदि(त्रुटि_सं == ENOENT || त्रुटि_सं == ENOTDIR){
			*error_code = MHD_HTTP_NOT_FOUND;
		}
		mosquitto_FREE(filename_canonical);
		फर्काऊ रिक्त;
	}
#endif
	यदि(म_भेदन(http_dir, filename_canonical, म_माप(http_dir))){
		/* Requested file isn't within http_dir, deny access because it's not found. */
		mosquitto_FREE(filename_canonical);
		*error_code = MHD_HTTP_NOT_FOUND;
		फर्काऊ रिक्त;
	}

	फर्काऊ filename_canonical;
}

स्थिर गणना MHD_Result http_api__send_error_response(संरचना MHD_Connection *connection, अचर वर्ण *error_str, अचिन्हित पूर्णाङ्क error_code)
{
	संरचना MHD_Response *response = MHD_create_response_from_buffer(म_माप(error_str), (शून्य *)error_str, MHD_RESPMEM_MUST_COPY);
	गणना MHD_Result ret = MHD_queue_response(connection, error_code, response);
	MHD_destroy_response(response);
	फर्काऊ ret;
}

स्थिर गणना MHD_Result http_api__send_response_with_headers(संरचना MHD_Connection *connection, अचर वर्ण *buf)
{
	गणना MHD_Result ret;
	संरचना MHD_Response *response = MHD_create_response_from_buffer(म_माप(buf), (शून्य *)buf, MHD_RESPMEM_MUST_COPY);

	ret = MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
	यदि(ret != MHD_YES){
		MHD_destroy_response(response);
		फर्काऊ ret;
	}
	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	फर्काऊ ret;
}

स्थिर गणना MHD_Result http_api__process_version(संरचना MHD_Connection *connection)
{
	फर्काऊ http_api__send_response_with_headers(connection, VERSION);
}

स्थिर गणना MHD_Result http_api__process_listeners(संरचना MHD_Connection *connection)
{
	वर्ण *buf;
	गणना MHD_Result ret;

	cJSON *j_रूख = cJSON_CreateObject();
	यदि(!j_रूख){
		फर्काऊ http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	cJSON *j_listeners = cJSON_AddArrayToObject(j_रूख, "listeners");
	यदि(!j_listeners){
		cJSON_Delete(j_रूख);
		फर्काऊ http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	लागि(पूर्णाङ्क i=0; i<db.config->listener_गन्ती; i++){
		cJSON *j_listener = cJSON_CreateObject();
		यदि(!j_listener){
			cJSON_Delete(j_रूख);
			फर्काऊ http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
		}
		cJSON_AddItemToArray(j_listeners, j_listener);

		संरचना mosquitto__listener *listener = &db.config->listeners[i];
#ifdef WITH_UNIX_SOCKETS
		यदि(listener->unix_socket_path){
			cJSON_AddStringToObject(j_listener, "path", listener->unix_socket_path);
		}अन्यथा
#endif
		{
			cJSON_AddIntToObject(j_listener, "port", listener->port);
		}

		विकल्प(listener->protocol){
			पक्ष mp_mqtt:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt");
				रोक;
			पक्ष mp_mqttsn:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt-sn");
				रोक;
			पक्ष mp_websockets:
				cJSON_AddStringToObject(j_listener, "protocol", "websockets");
				रोक;
			पक्ष mp_http_api:
				cJSON_AddStringToObject(j_listener, "protocol", "httpapi");
				रोक;
		}

#ifdef WITH_TLS
		cJSON_AddBoolToObject(j_listener, "tls", listener->certfile && listener->keyfile);
		cJSON_AddBoolToObject(j_listener, "mtls", listener->require_certificate);
#अन्यथा
		cJSON_AddBoolToObject(j_listener, "tls", false);
		cJSON_AddBoolToObject(j_listener, "mtls", false);
#endif
		यदि(listener->security_options->allow_anonymous == -1){
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", db.config->security_options.allow_anonymous);
		}अन्यथा{
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", listener->security_options->allow_anonymous);
		}
	}
	buf = cJSON_Print(j_रूख);
	cJSON_Delete(j_रूख);
	यदि(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		मुक्त(buf);
	}अन्यथा{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	फर्काऊ ret;
}


स्थिर गणना MHD_Result http_api__process_systree(संरचना MHD_Connection *connection)
{
	गणना MHD_Result ret;
#ifdef WITH_SYS_रूख
	वर्ण *buf;

	cJSON *j_रूख = cJSON_CreateObject();
	लागि(पूर्णाङ्क i=0; i<mosq_metric_max; i++){
		यदि(metrics[i].topic){
			cJSON_AddIntToObject(j_रूख, metrics[i].topic, metrics[i].current);
		}
	}
	cJSON_AddIntToObject(j_रूख, "$SYS/broker/uptime", broker_uptime());

	buf = cJSON_Print(j_रूख);
	cJSON_Delete(j_रूख);

	यदि(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		मुक्त(buf);
	}अन्यथा{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}
#अन्यथा
	ret = http_api__send_error_response(connection, "Not found.\n", 404);
#endif

	फर्काऊ ret;
}


स्थिर ssize_t http_file_read_cb(शून्य *cls, uint64_t pos, वर्ण *buf, माप_प्रकार max)
{
	खाता *fptr = cls;
	यदि(ख_जाओ(fptr, (दीर्घ )pos, शुरू_से) < 0){
		फर्काऊ -1;
	}
	फर्काऊ (ssize_t )ख_पढ़ो(buf, 1, max, fptr);
}


स्थिर शून्य http_file_free_cb(शून्य *cls)
{
	खाता *fptr = cls;
	ख_बंद(fptr);
}


स्थिर गणना MHD_Result http_api__process_file(संरचना mosquitto__listener *listener, संरचना MHD_Connection *connection, अचर वर्ण *url)
{
	पूर्णाङ्क error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
	वर्ण *canonical_filename;

	यदि(!listener || !listener->http_dir){
		http_api__send_error_response(connection, "Not found.\n", 404);
		फर्काऊ MHD_YES;
	}

	canonical_filename = http__canonical_filename(url, listener->http_dir, &error_code);
	यदि(!canonical_filename){
		http_api__send_error_response(connection, "Not found.\n", 404);
		फर्काऊ MHD_YES;
	}

	खाता *fptr = ख_खोलो(canonical_filename, "rb");
	mosquitto_FREE(canonical_filename);
	यदि(!fptr){
		http_api__send_error_response(connection, "Not found.\n", 404);
		फर्काऊ MHD_YES;
	}

	संरचना stat statbuf;
	यदि(ख_स्थिति(fileno(fptr), &statbuf)){
		ख_बंद(fptr);
		http_api__send_error_response(connection, "Internal server error.\n", 500);
		फर्काऊ MHD_YES;
	}

	यदि(!S_ISREG(statbuf.st_mode)){
		ख_बंद(fptr);
		http_api__send_error_response(connection, "Not found.\n", 404);
		फर्काऊ MHD_YES;
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
	फर्काऊ ret;
}


स्थिर गणना MHD_Result http_api__process_api(संरचना MHD_Connection *connection, अचर वर्ण *url)
{
	यदि(म_भेद(url, "/api/v1/systree") == 0){
		फर्काऊ http_api__process_systree(connection);
	}अन्यथा यदि(म_भेद(url, "/api/v1/listeners") == 0){
		फर्काऊ http_api__process_listeners(connection);
	}अन्यथा यदि(म_भेद(url, "/api/v1/version") == 0){
		फर्काऊ http_api__process_version(connection);
	}अन्यथा{
		फर्काऊ http_api__send_error_response(connection, "Not found.\n", 404);
	}
}


स्थिर पूर्णाङ्क check_access(संरचना mosquitto__listener *listener, संरचना MHD_Connection *connection, अचर वर्ण *url)
{
	संरचना mosquitto context = {0};
	पूर्णाङ्क auth_rc, acl_rc = MOSQ_ERR_SUCCESS;

	context.listener = listener;

	context.id = (वर्ण *)"http-api";
	context.username = MHD_basic_auth_get_username_password(connection, &context.password);

	/* Authentication */
	auth_rc = mosquitto_basic_auth(&context);
	यदि(auth_rc == MOSQ_ERR_SUCCESS){
		acl_rc = mosquitto_acl_check(&context, url, 0, रिक्त, 0, false, रिक्त, MOSQ_ACL_READ);
	}
	MHD_free(context.username);
	MHD_free(context.password);

	यदि(auth_rc || acl_rc){
		अचर वर्ण *buf = "Not authorised\n";
		संरचना MHD_Response *response = MHD_create_response_from_buffer(म_माप(buf), (शून्य *)buf, MHD_RESPMEM_MUST_COPY);
		MHD_queue_basic_auth_fail_response(connection, "Mosquitto API", response);
		MHD_destroy_response(response);

		फर्काऊ MOSQ_ERR_AUTH;
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
}

स्थिर गणना MHD_Result http_api_handler(शून्य *cls, संरचना MHD_Connection
		*connection, अचर वर्ण *url, अचर वर्ण *method, अचर वर्ण *version,
		अचर वर्ण *upload_data, माप_प्रकार *upload_data_size, शून्य **con_cls)
{
	UNUSED(version);
	UNUSED(upload_data);
	UNUSED(upload_data_size);
	UNUSED(con_cls);

	संरचना mosquitto__listener *listener = cls;

	यदि(म_भेद(method, "GET") != 0){
		वर्ण *buf = "Invalid HTTP Method\n";
		संरचना MHD_Response *response = MHD_create_response_from_buffer(म_माप(buf), (शून्य *)buf, MHD_RESPMEM_MUST_COPY);
		गणना MHD_Result ret = MHD_queue_response(connection, MHD_HTTP_METHOD_NOT_ALLOWED, response);
		MHD_destroy_response(response);
		फर्काऊ ret;
	}

	यदि(check_access(listener, connection, url) != MOSQ_ERR_SUCCESS){
		फर्काऊ MHD_YES;
	}

	यदि(!strncasecmp(url, "/api/", म_माप("/api/"))){
		फर्काऊ http_api__process_api(connection, url);
	}अन्यथा{
		फर्काऊ http_api__process_file(listener, connection, url);
	}
}


पूर्णाङ्क http_api__start_local(संरचना mosquitto__listener *listener)
{
	listener->security_options = mosquitto_calloc(1, परिमाण(संरचना mosquitto__security_options));
	यदि(listener->security_options == रिक्त){
		फर्काऊ MOSQ_ERR_NOMEM;
	}

	listener->host = mosquitto_strdup("127.0.0.1");
	यदि(!listener->host){
		mosquitto_FREE(listener->security_options);
		फर्काऊ MOSQ_ERR_NOMEM;
	}
	listener->port = 9883;

	यदि(db.config->security_options.allow_anonymous == -1){
		listener->security_options->allow_anonymous = true;
	}अन्यथा{
		listener->security_options->allow_anonymous = db.config->security_options.allow_anonymous;
	}

	यदि(listener->http_dir == रिक्त && म_माप(HTTP_API_DIR) > 0){
#ifdef WIN32
		वर्ण *http_dir_canonical = _fullpath(रिक्त, HTTP_API_DIR, 0);
#अन्यथा
		वर्ण *http_dir_canonical = realpath(HTTP_API_DIR, रिक्त);
#endif
		यदि(!http_dir_canonical){
			फर्काऊ MOSQ_ERR_NOMEM;
		}
		mosquitto_FREE(listener->http_dir);
		listener->http_dir = mosquitto_strdup(http_dir_canonical);
		mosquitto_FREE(http_dir_canonical);
	}

	फर्काऊ http_api__start(listener);
}


पूर्णाङ्क http_api__start(संरचना mosquitto__listener *listener)
{
	अचिन्हित पूर्णाङ्क flags = MHD_USE_AUTO_INTERNAL_THREAD;
	अचर वर्ण *bind_address;
	uint16_t port = 9883;
	वर्ण *x509_cert = रिक्त;
	वर्ण *x509_साँचो = रिक्त;

	यदि(!listener->security_options){
		listener->security_options = mosquitto_calloc(1, परिमाण(संरचना mosquitto__security_options));
		यदि(listener->security_options == रिक्त){
			फर्काऊ MOSQ_ERR_NOMEM;
		}
	}
	listener->protocol = mp_http_api;

	bind_address = listener->host;
	port = listener->port;

#ifdef WITH_TLS
	यदि(listener->certfile && listener->keyfile){
		यदि(mosquitto_read_file(listener->certfile, false, &x509_cert, रिक्त)){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
			फर्काऊ MOSQ_ERR_INVAL;
		}
		यदि(mosquitto_read_file(listener->keyfile, false, &x509_साँचो, रिक्त)){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			mosquitto_FREE(x509_cert);
			फर्काऊ MOSQ_ERR_INVAL;
		}
		flags |= MHD_USE_TLS;
	}
#endif

	यदि(bind_address){
		वर्ण service[10];
		संरचना addrinfo hints;
		संरचना addrinfo *ainfo, *rp;

		snprintf(service, परिमाण(service), "%d", port);

		स_रखो(&hints, 0, परिमाण(संरचना addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = AI_PASSIVE;
		hints.ai_socktype = SOCK_STREAM;

		पूर्णाङ्क rc = getaddrinfo(bind_address, service, &hints, &ainfo);
		यदि(rc){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Unable to start http api listener (%s).", म_त्रुटि(त्रुटि_सं));
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_साँचो);
			फर्काऊ MOSQ_ERR_ERRNO;
		}

		लागि(rp = ainfo; rp; rp = rp->ai_next){
			यदि(rp->ai_family == AF_INET || rp->ai_family == AF_INET6){
				रोक;
			}
		}
		यदि(!rp){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Unable to start http api listener, could not find address.");
			freeaddrinfo(ainfo);
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_साँचो);

			फर्काऊ MOSQ_ERR_INVAL;
		}

		यदि(rp->ai_family == AF_INET6){
			flags |= MHD_USE_IPv6;
		}
		यदि(x509_cert && x509_साँचो){
			listener->mhd = MHD_start_daemon(flags, port, रिक्त, रिक्त, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_साँचो, x509_साँचो,
					MHD_OPTION_END);
		}अन्यथा{
			listener->mhd = MHD_start_daemon(flags, port, रिक्त, रिक्त, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_END);
		}
		freeaddrinfo(ainfo);
	}अन्यथा{
		यदि(x509_cert && x509_साँचो){
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_थाक, port, रिक्त, रिक्त, &http_api_handler, listener,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_साँचो, x509_साँचो,
					MHD_OPTION_END);
		}अन्यथा{
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_थाक, port, रिक्त, रिक्त, &http_api_handler, listener,
					MHD_OPTION_END);
		}
	}

	mosquitto_FREE(x509_cert);
	mosquitto_FREE(x509_साँचो);

	यदि(listener->mhd){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Opening http api listen socket on port %d.", port);
		यदि(listener->http_dir){
			log__printf(रिक्त, MOSQ_LOG_INFO, "Using http_dir %s", listener->http_dir);
		}
		फर्काऊ MOSQ_ERR_SUCCESS;
	}अन्यथा{
		फर्काऊ MOSQ_ERR_UNKNOWN;
	}
}


शून्य http_api__stop(संरचना mosquitto__listener *listener)
{
	MHD_stop_daemon(listener->mhd);
	listener->mhd = रिक्त;
}

#endif
