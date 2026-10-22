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
	स्थिरांक अक्षर *topic, *topic_alias;
	bool is_max;
};

समय_प्रकार broker_uptime(शून्य);
संरचना MHD_Daemon *mhd = रिक्त;

#ifdef WITH_SYS_वृक्ष
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


स्थिर अक्षर *http__canonical_filename(
		स्थिरांक अक्षर *url,
		स्थिरांक अक्षर *http_dir,
		पूर्णांक *error_code)
{
	माप_प्रकार urllen, slen;
	अक्षर *filename, *filename_canonical;

	urllen = म_माप(url);
	जर(url[urllen-1] == '/'){
		slen = म_माप(http_dir) + urllen + म_माप("/index.html") + 2;
	}अन्यथा{
		slen = म_माप(http_dir) + urllen + 2;
	}
	filename = mosquitto_malloc(slen);
	जर(!filename){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		परत रिक्त;
	}
	जर(((अक्षर *)url)[urllen-1] == '/'){
		snprintf(filename, slen, "%s%c%sindex.html", http_dir, DIR_SEP, url);
	}अन्यथा{
		snprintf(filename, slen, "%s%c%s", http_dir, DIR_SEP, url);
	}


	/* Get canonical path and check it is within our http_dir */
	filename_canonical = mosquitto_calloc(1, PATH_MAX);
	जर(!filename_canonical){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		परत रिक्त;
	}
#ifdef WIN32
	अक्षर *resolved = _fullpath(filename_canonical, filename, PATH_MAX);
	mosquitto_FREE(filename);
	जर(!resolved){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		mosquitto_FREE(filename_canonical);
		परत रिक्त;
	}
#अन्यथा
	अक्षर *resolved = realpath(filename, filename_canonical);
	mosquitto_FREE(filename);
	जर(!resolved){
		जर(त्रुटि_सं == EACCES){
			*error_code = MHD_HTTP_FORBIDDEN;
		}अन्यथा जर(त्रुटि_सं == EINVAL || त्रुटि_सं == EIO || त्रुटि_सं == ELOOP){
			*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		}अन्यथा जर(त्रुटि_सं == ENAMETOOLONG){
			*error_code = MHD_HTTP_URI_TOO_LONG;
		}अन्यथा जर(त्रुटि_सं == ENOENT || त्रुटि_सं == ENOTDIR){
			*error_code = MHD_HTTP_NOT_FOUND;
		}
		mosquitto_FREE(filename_canonical);
		परत रिक्त;
	}
#endif
	जर(म_भेदन(http_dir, filename_canonical, म_माप(http_dir))){
		/* Requested file isn't within http_dir, deny access because it's not found. */
		mosquitto_FREE(filename_canonical);
		*error_code = MHD_HTTP_NOT_FOUND;
		परत रिक्त;
	}

	परत filename_canonical;
}

स्थिर गणना MHD_Result http_api__send_error_response(संरचना MHD_Connection *connection, स्थिरांक अक्षर *error_str, अचिन्हित पूर्णांक error_code)
{
	संरचना MHD_Response *response = MHD_create_response_from_buffer(म_माप(error_str), (शून्य *)error_str, MHD_RESPMEM_MUST_COPY);
	गणना MHD_Result ret = MHD_queue_response(connection, error_code, response);
	MHD_destroy_response(response);
	परत ret;
}

स्थिर गणना MHD_Result http_api__send_response_with_headers(संरचना MHD_Connection *connection, स्थिरांक अक्षर *buf)
{
	गणना MHD_Result ret;
	संरचना MHD_Response *response = MHD_create_response_from_buffer(म_माप(buf), (शून्य *)buf, MHD_RESPMEM_MUST_COPY);

	ret = MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
	जर(ret != MHD_YES){
		MHD_destroy_response(response);
		परत ret;
	}
	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	परत ret;
}

स्थिर गणना MHD_Result http_api__process_version(संरचना MHD_Connection *connection)
{
	परत http_api__send_response_with_headers(connection, VERSION);
}

स्थिर गणना MHD_Result http_api__process_listeners(संरचना MHD_Connection *connection)
{
	अक्षर *buf;
	गणना MHD_Result ret;

	cJSON *j_वृक्ष = cJSON_CreateObject();
	जर(!j_वृक्ष){
		परत http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	cJSON *j_listeners = cJSON_AddArrayToObject(j_वृक्ष, "listeners");
	जर(!j_listeners){
		cJSON_Delete(j_वृक्ष);
		परत http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	साठी(पूर्णांक i=0; i<db.config->listener_मोजणी; i++){
		cJSON *j_listener = cJSON_CreateObject();
		जर(!j_listener){
			cJSON_Delete(j_वृक्ष);
			परत http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
		}
		cJSON_AddItemToArray(j_listeners, j_listener);

		संरचना mosquitto__listener *listener = &db.config->listeners[i];
#ifdef WITH_UNIX_SOCKETS
		जर(listener->unix_socket_path){
			cJSON_AddStringToObject(j_listener, "path", listener->unix_socket_path);
		}अन्यथा
#endif
		{
			cJSON_AddIntToObject(j_listener, "port", listener->port);
		}

		निवड(listener->protocol){
			स्थिती mp_mqtt:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt");
				थांबा;
			स्थिती mp_mqttsn:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt-sn");
				थांबा;
			स्थिती mp_websockets:
				cJSON_AddStringToObject(j_listener, "protocol", "websockets");
				थांबा;
			स्थिती mp_http_api:
				cJSON_AddStringToObject(j_listener, "protocol", "httpapi");
				थांबा;
		}

#ifdef WITH_TLS
		cJSON_AddBoolToObject(j_listener, "tls", listener->certfile && listener->keyfile);
		cJSON_AddBoolToObject(j_listener, "mtls", listener->require_certificate);
#अन्यथा
		cJSON_AddBoolToObject(j_listener, "tls", false);
		cJSON_AddBoolToObject(j_listener, "mtls", false);
#endif
		जर(listener->security_options->allow_anonymous == -1){
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", db.config->security_options.allow_anonymous);
		}अन्यथा{
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", listener->security_options->allow_anonymous);
		}
	}
	buf = cJSON_Print(j_वृक्ष);
	cJSON_Delete(j_वृक्ष);
	जर(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		मुक्त(buf);
	}अन्यथा{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	परत ret;
}


स्थिर गणना MHD_Result http_api__process_systree(संरचना MHD_Connection *connection)
{
	गणना MHD_Result ret;
#ifdef WITH_SYS_वृक्ष
	अक्षर *buf;

	cJSON *j_वृक्ष = cJSON_CreateObject();
	साठी(पूर्णांक i=0; i<mosq_metric_max; i++){
		जर(metrics[i].topic){
			cJSON_AddIntToObject(j_वृक्ष, metrics[i].topic, metrics[i].current);
		}
	}
	cJSON_AddIntToObject(j_वृक्ष, "$SYS/broker/uptime", broker_uptime());

	buf = cJSON_Print(j_वृक्ष);
	cJSON_Delete(j_वृक्ष);

	जर(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		मुक्त(buf);
	}अन्यथा{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}
#अन्यथा
	ret = http_api__send_error_response(connection, "Not found.\n", 404);
#endif

	परत ret;
}


स्थिर ssize_t http_file_read_cb(शून्य *cls, uint64_t pos, अक्षर *buf, माप_प्रकार max)
{
	खाता *fptr = cls;
	जर(ख_जाओ(fptr, (दीर्घ )pos, शुरू_से) < 0){
		परत -1;
	}
	परत (ssize_t )ख_पढ़ो(buf, 1, max, fptr);
}


स्थिर शून्य http_file_free_cb(शून्य *cls)
{
	खाता *fptr = cls;
	ख_बंद(fptr);
}


स्थिर गणना MHD_Result http_api__process_file(संरचना mosquitto__listener *listener, संरचना MHD_Connection *connection, स्थिरांक अक्षर *url)
{
	पूर्णांक error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
	अक्षर *canonical_filename;

	जर(!listener || !listener->http_dir){
		http_api__send_error_response(connection, "Not found.\n", 404);
		परत MHD_YES;
	}

	canonical_filename = http__canonical_filename(url, listener->http_dir, &error_code);
	जर(!canonical_filename){
		http_api__send_error_response(connection, "Not found.\n", 404);
		परत MHD_YES;
	}

	खाता *fptr = ख_खोलो(canonical_filename, "rb");
	mosquitto_FREE(canonical_filename);
	जर(!fptr){
		http_api__send_error_response(connection, "Not found.\n", 404);
		परत MHD_YES;
	}

	संरचना stat statbuf;
	जर(ख_स्थिति(fileno(fptr), &statbuf)){
		ख_बंद(fptr);
		http_api__send_error_response(connection, "Internal server error.\n", 500);
		परत MHD_YES;
	}

	जर(!S_ISREG(statbuf.st_mode)){
		ख_बंद(fptr);
		http_api__send_error_response(connection, "Not found.\n", 404);
		परत MHD_YES;
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
	परत ret;
}


स्थिर गणना MHD_Result http_api__process_api(संरचना MHD_Connection *connection, स्थिरांक अक्षर *url)
{
	जर(म_भेद(url, "/api/v1/systree") == 0){
		परत http_api__process_systree(connection);
	}अन्यथा जर(म_भेद(url, "/api/v1/listeners") == 0){
		परत http_api__process_listeners(connection);
	}अन्यथा जर(म_भेद(url, "/api/v1/version") == 0){
		परत http_api__process_version(connection);
	}अन्यथा{
		परत http_api__send_error_response(connection, "Not found.\n", 404);
	}
}


स्थिर पूर्णांक check_access(संरचना mosquitto__listener *listener, संरचना MHD_Connection *connection, स्थिरांक अक्षर *url)
{
	संरचना mosquitto context = {0};
	पूर्णांक auth_rc, acl_rc = MOSQ_ERR_SUCCESS;

	context.listener = listener;

	context.id = (अक्षर *)"http-api";
	context.username = MHD_basic_auth_get_username_password(connection, &context.password);

	/* Authentication */
	auth_rc = mosquitto_basic_auth(&context);
	जर(auth_rc == MOSQ_ERR_SUCCESS){
		acl_rc = mosquitto_acl_check(&context, url, 0, रिक्त, 0, false, रिक्त, MOSQ_ACL_READ);
	}
	MHD_free(context.username);
	MHD_free(context.password);

	जर(auth_rc || acl_rc){
		स्थिरांक अक्षर *buf = "Not authorised\n";
		संरचना MHD_Response *response = MHD_create_response_from_buffer(म_माप(buf), (शून्य *)buf, MHD_RESPMEM_MUST_COPY);
		MHD_queue_basic_auth_fail_response(connection, "Mosquitto API", response);
		MHD_destroy_response(response);

		परत MOSQ_ERR_AUTH;
	}

	परत MOSQ_ERR_SUCCESS;
}

स्थिर गणना MHD_Result http_api_handler(शून्य *cls, संरचना MHD_Connection
		*connection, स्थिरांक अक्षर *url, स्थिरांक अक्षर *method, स्थिरांक अक्षर *version,
		स्थिरांक अक्षर *upload_data, माप_प्रकार *upload_data_size, शून्य **con_cls)
{
	UNUSED(version);
	UNUSED(upload_data);
	UNUSED(upload_data_size);
	UNUSED(con_cls);

	संरचना mosquitto__listener *listener = cls;

	जर(म_भेद(method, "GET") != 0){
		अक्षर *buf = "Invalid HTTP Method\n";
		संरचना MHD_Response *response = MHD_create_response_from_buffer(म_माप(buf), (शून्य *)buf, MHD_RESPMEM_MUST_COPY);
		गणना MHD_Result ret = MHD_queue_response(connection, MHD_HTTP_METHOD_NOT_ALLOWED, response);
		MHD_destroy_response(response);
		परत ret;
	}

	जर(check_access(listener, connection, url) != MOSQ_ERR_SUCCESS){
		परत MHD_YES;
	}

	जर(!strncasecmp(url, "/api/", म_माप("/api/"))){
		परत http_api__process_api(connection, url);
	}अन्यथा{
		परत http_api__process_file(listener, connection, url);
	}
}


पूर्णांक http_api__start_local(संरचना mosquitto__listener *listener)
{
	listener->security_options = mosquitto_calloc(1, आकार(संरचना mosquitto__security_options));
	जर(listener->security_options == रिक्त){
		परत MOSQ_ERR_NOMEM;
	}

	listener->host = mosquitto_strdup("127.0.0.1");
	जर(!listener->host){
		mosquitto_FREE(listener->security_options);
		परत MOSQ_ERR_NOMEM;
	}
	listener->port = 9883;

	जर(db.config->security_options.allow_anonymous == -1){
		listener->security_options->allow_anonymous = true;
	}अन्यथा{
		listener->security_options->allow_anonymous = db.config->security_options.allow_anonymous;
	}

	जर(listener->http_dir == रिक्त && म_माप(HTTP_API_DIR) > 0){
#ifdef WIN32
		अक्षर *http_dir_canonical = _fullpath(रिक्त, HTTP_API_DIR, 0);
#अन्यथा
		अक्षर *http_dir_canonical = realpath(HTTP_API_DIR, रिक्त);
#endif
		जर(!http_dir_canonical){
			परत MOSQ_ERR_NOMEM;
		}
		mosquitto_FREE(listener->http_dir);
		listener->http_dir = mosquitto_strdup(http_dir_canonical);
		mosquitto_FREE(http_dir_canonical);
	}

	परत http_api__start(listener);
}


पूर्णांक http_api__start(संरचना mosquitto__listener *listener)
{
	अचिन्हित पूर्णांक flags = MHD_USE_AUTO_INTERNAL_THREAD;
	स्थिरांक अक्षर *bind_address;
	uint16_t port = 9883;
	अक्षर *x509_cert = रिक्त;
	अक्षर *x509_किल्ली = रिक्त;

	जर(!listener->security_options){
		listener->security_options = mosquitto_calloc(1, आकार(संरचना mosquitto__security_options));
		जर(listener->security_options == रिक्त){
			परत MOSQ_ERR_NOMEM;
		}
	}
	listener->protocol = mp_http_api;

	bind_address = listener->host;
	port = listener->port;

#ifdef WITH_TLS
	जर(listener->certfile && listener->keyfile){
		जर(mosquitto_read_file(listener->certfile, false, &x509_cert, रिक्त)){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
			परत MOSQ_ERR_INVAL;
		}
		जर(mosquitto_read_file(listener->keyfile, false, &x509_किल्ली, रिक्त)){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			mosquitto_FREE(x509_cert);
			परत MOSQ_ERR_INVAL;
		}
		flags |= MHD_USE_TLS;
	}
#endif

	जर(bind_address){
		अक्षर service[10];
		संरचना addrinfo hints;
		संरचना addrinfo *ainfo, *rp;

		snprintf(service, आकार(service), "%d", port);

		स_रखो(&hints, 0, आकार(संरचना addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = AI_PASSIVE;
		hints.ai_socktype = SOCK_STREAM;

		पूर्णांक rc = getaddrinfo(bind_address, service, &hints, &ainfo);
		जर(rc){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Unable to start http api listener (%s).", म_त्रुटि(त्रुटि_सं));
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_किल्ली);
			परत MOSQ_ERR_ERRNO;
		}

		साठी(rp = ainfo; rp; rp = rp->ai_next){
			जर(rp->ai_family == AF_INET || rp->ai_family == AF_INET6){
				थांबा;
			}
		}
		जर(!rp){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Unable to start http api listener, could not find address.");
			freeaddrinfo(ainfo);
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_किल्ली);

			परत MOSQ_ERR_INVAL;
		}

		जर(rp->ai_family == AF_INET6){
			flags |= MHD_USE_IPv6;
		}
		जर(x509_cert && x509_किल्ली){
			listener->mhd = MHD_start_daemon(flags, port, रिक्त, रिक्त, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_किल्ली, x509_किल्ली,
					MHD_OPTION_END);
		}अन्यथा{
			listener->mhd = MHD_start_daemon(flags, port, रिक्त, रिक्त, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_END);
		}
		freeaddrinfo(ainfo);
	}अन्यथा{
		जर(x509_cert && x509_किल्ली){
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_चळत, port, रिक्त, रिक्त, &http_api_handler, listener,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_किल्ली, x509_किल्ली,
					MHD_OPTION_END);
		}अन्यथा{
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_चळत, port, रिक्त, रिक्त, &http_api_handler, listener,
					MHD_OPTION_END);
		}
	}

	mosquitto_FREE(x509_cert);
	mosquitto_FREE(x509_किल्ली);

	जर(listener->mhd){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Opening http api listen socket on port %d.", port);
		जर(listener->http_dir){
			log__printf(रिक्त, MOSQ_LOG_INFO, "Using http_dir %s", listener->http_dir);
		}
		परत MOSQ_ERR_SUCCESS;
	}अन्यथा{
		परत MOSQ_ERR_UNKNOWN;
	}
}


शून्य http_api__stop(संरचना mosquitto__listener *listener)
{
	MHD_stop_daemon(listener->mhd);
	listener->mhd = रिक्त;
}

#endif
