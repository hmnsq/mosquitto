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

#समावेश "config.h"

#अगर_घोषित WITH_HTTP_API

#समावेश <assert.h>
#समावेश <errno.h>
#समावेश <limits.h>
#समावेश <microhttpd.h>
#समावेश <string.h>
#समावेश <sys/stat.h>
#अगर_अघोषित WIN32
#  include <netdb.h>
#पूर्ण_अगर

#समावेश "json_help.h"
#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/mqtt_protocol.h"
#समावेश "sys_tree.h"

#अगर_अघोषित HTTP_API_DIR
#  define HTTP_API_DIR ""
#पूर्ण_अगर

काष्ठा metric {
	int64_t current;
	int64_t अग्र;
	स्थिर अक्षर *topic, *topic_alias;
	bool is_max;
};

समय_प्रकार broker_uptime(व्योम);
काष्ठा MHD_Daemon *mhd = शून्य;

#अगर_घोषित WITH_SYS_वृक्ष
बाह्य काष्ठा metric metrics[mosq_metric_max];
#पूर्ण_अगर

#घोषणा HTTP_RESPONSE_BUFLEN 10000

#अगर_घोषित WIN32
#घोषणा DIR_SEP '\\'
#घोषणा PATH_MAX MAX_PATH
#अन्यथा
#घोषणा DIR_SEP '/'
#पूर्ण_अगर

#अगर_अघोषित S_ISREG
#घोषणा S_ISREG(mode) (((mode)&S_IFMT) == S_IFREG)
#पूर्ण_अगर


अटल अक्षर *http__canonical_filename(
		स्थिर अक्षर *url,
		स्थिर अक्षर *http_dir,
		पूर्णांक *error_code)
{
	माप_प्रकार urllen, slen;
	अक्षर *filename, *filename_canonical;

	urllen = म_माप(url);
	अगर(url[urllen-1] == '/'){
		slen = म_माप(http_dir) + urllen + म_माप("/index.html") + 2;
	}अन्यथा{
		slen = म_माप(http_dir) + urllen + 2;
	}
	filename = mosquitto_malloc(slen);
	अगर(!filename){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		वापस शून्य;
	}
	अगर(((अक्षर *)url)[urllen-1] == '/'){
		snprintf(filename, slen, "%s%c%sindex.html", http_dir, DIR_SEP, url);
	}अन्यथा{
		snprintf(filename, slen, "%s%c%s", http_dir, DIR_SEP, url);
	}


	/* Get canonical path and check it is within our http_dir */
	filename_canonical = mosquitto_calloc(1, PATH_MAX);
	अगर(!filename_canonical){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		वापस शून्य;
	}
#अगर_घोषित WIN32
	अक्षर *resolved = _fullpath(filename_canonical, filename, PATH_MAX);
	mosquitto_FREE(filename);
	अगर(!resolved){
		*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		mosquitto_FREE(filename_canonical);
		वापस शून्य;
	}
#अन्यथा
	अक्षर *resolved = realpath(filename, filename_canonical);
	mosquitto_FREE(filename);
	अगर(!resolved){
		अगर(त्रुटि_सं == EACCES){
			*error_code = MHD_HTTP_FORBIDDEN;
		}अन्यथा अगर(त्रुटि_सं == EINVAL || त्रुटि_सं == EIO || त्रुटि_सं == ELOOP){
			*error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
		}अन्यथा अगर(त्रुटि_सं == ENAMETOOLONG){
			*error_code = MHD_HTTP_URI_TOO_LONG;
		}अन्यथा अगर(त्रुटि_सं == ENOENT || त्रुटि_सं == ENOTDIR){
			*error_code = MHD_HTTP_NOT_FOUND;
		}
		mosquitto_FREE(filename_canonical);
		वापस शून्य;
	}
#पूर्ण_अगर
	अगर(म_भेदन(http_dir, filename_canonical, म_माप(http_dir))){
		/* Requested file isn't within http_dir, deny access because it's not found. */
		mosquitto_FREE(filename_canonical);
		*error_code = MHD_HTTP_NOT_FOUND;
		वापस शून्य;
	}

	वापस filename_canonical;
}

अटल क्रमागत MHD_परिणाम http_api__send_error_response(काष्ठा MHD_Connection *connection, स्थिर अक्षर *error_str, अचिन्हित पूर्णांक error_code)
{
	काष्ठा MHD_Response *response = MHD_create_response_from_बफर(म_माप(error_str), (व्योम *)error_str, MHD_RESPMEM_MUST_COPY);
	क्रमागत MHD_परिणाम ret = MHD_queue_response(connection, error_code, response);
	MHD_destroy_response(response);
	वापस ret;
}

अटल क्रमागत MHD_परिणाम http_api__send_response_with_headers(काष्ठा MHD_Connection *connection, स्थिर अक्षर *buf)
{
	क्रमागत MHD_परिणाम ret;
	काष्ठा MHD_Response *response = MHD_create_response_from_बफर(म_माप(buf), (व्योम *)buf, MHD_RESPMEM_MUST_COPY);

	ret = MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
	अगर(ret != MHD_YES){
		MHD_destroy_response(response);
		वापस ret;
	}
	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	वापस ret;
}

अटल क्रमागत MHD_परिणाम http_api__process_version(काष्ठा MHD_Connection *connection)
{
	वापस http_api__send_response_with_headers(connection, VERSION);
}

अटल क्रमागत MHD_परिणाम http_api__process_listeners(काष्ठा MHD_Connection *connection)
{
	अक्षर *buf;
	क्रमागत MHD_परिणाम ret;

	cJSON *j_वृक्ष = cJSON_CreateObject();
	अगर(!j_वृक्ष){
		वापस http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	cJSON *j_listeners = cJSON_AddArrayToObject(j_वृक्ष, "listeners");
	अगर(!j_listeners){
		cJSON_Delete(j_वृक्ष);
		वापस http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	क्रम(पूर्णांक i=0; i<db.config->listener_गिनती; i++){
		cJSON *j_listener = cJSON_CreateObject();
		अगर(!j_listener){
			cJSON_Delete(j_वृक्ष);
			वापस http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
		}
		cJSON_AddItemToArray(j_listeners, j_listener);

		काष्ठा mosquitto__listener *listener = &db.config->listeners[i];
#अगर_घोषित WITH_UNIX_SOCKETS
		अगर(listener->unix_socket_path){
			cJSON_AddStringToObject(j_listener, "path", listener->unix_socket_path);
		}अन्यथा
#पूर्ण_अगर
		{
			cJSON_AddIntToObject(j_listener, "port", listener->port);
		}

		चयन(listener->protocol){
			हाल mp_mqtt:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt");
				अवरोध;
			हाल mp_mqttsn:
				cJSON_AddStringToObject(j_listener, "protocol", "mqtt-sn");
				अवरोध;
			हाल mp_websockets:
				cJSON_AddStringToObject(j_listener, "protocol", "websockets");
				अवरोध;
			हाल mp_http_api:
				cJSON_AddStringToObject(j_listener, "protocol", "httpapi");
				अवरोध;
		}

#अगर_घोषित WITH_TLS
		cJSON_AddBoolToObject(j_listener, "tls", listener->certfile && listener->keyfile);
		cJSON_AddBoolToObject(j_listener, "mtls", listener->require_certificate);
#अन्यथा
		cJSON_AddBoolToObject(j_listener, "tls", false);
		cJSON_AddBoolToObject(j_listener, "mtls", false);
#पूर्ण_अगर
		अगर(listener->security_options->allow_anonymous == -1){
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", db.config->security_options.allow_anonymous);
		}अन्यथा{
			cJSON_AddBoolToObject(j_listener, "allow_anonymous", listener->security_options->allow_anonymous);
		}
	}
	buf = cJSON_Print(j_वृक्ष);
	cJSON_Delete(j_वृक्ष);
	अगर(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		मुक्त(buf);
	}अन्यथा{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}

	वापस ret;
}


अटल क्रमागत MHD_परिणाम http_api__process_systree(काष्ठा MHD_Connection *connection)
{
	क्रमागत MHD_परिणाम ret;
#अगर_घोषित WITH_SYS_वृक्ष
	अक्षर *buf;

	cJSON *j_वृक्ष = cJSON_CreateObject();
	क्रम(पूर्णांक i=0; i<mosq_metric_max; i++){
		अगर(metrics[i].topic){
			cJSON_AddIntToObject(j_वृक्ष, metrics[i].topic, metrics[i].current);
		}
	}
	cJSON_AddIntToObject(j_वृक्ष, "$SYS/broker/uptime", broker_uptime());

	buf = cJSON_Print(j_वृक्ष);
	cJSON_Delete(j_वृक्ष);

	अगर(buf){
		ret = http_api__send_response_with_headers(connection, buf);
		मुक्त(buf);
	}अन्यथा{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_INTERNAL_SERVER_ERROR);
	}
#अन्यथा
	ret = http_api__send_error_response(connection, "Not found.\n", 404);
#पूर्ण_अगर

	वापस ret;
}


अटल ssize_t http_file_read_cb(व्योम *cls, uint64_t pos, अक्षर *buf, माप_प्रकार max)
{
	खाता *fptr = cls;
	अगर(ख_जाओ(fptr, (दीर्घ )pos, शुरू_से) < 0){
		वापस -1;
	}
	वापस (ssize_t )ख_पढ़ो(buf, 1, max, fptr);
}


अटल व्योम http_file_free_cb(व्योम *cls)
{
	खाता *fptr = cls;
	ख_बंद(fptr);
}


अटल क्रमागत MHD_परिणाम http_api__process_file(काष्ठा mosquitto__listener *listener, काष्ठा MHD_Connection *connection, स्थिर अक्षर *url)
{
	पूर्णांक error_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
	अक्षर *canonical_filename;

	अगर(!listener || !listener->http_dir){
		http_api__send_error_response(connection, "Not found.\n", 404);
		वापस MHD_YES;
	}

	canonical_filename = http__canonical_filename(url, listener->http_dir, &error_code);
	अगर(!canonical_filename){
		http_api__send_error_response(connection, "Not found.\n", 404);
		वापस MHD_YES;
	}

	खाता *fptr = ख_खोलो(canonical_filename, "rb");
	mosquitto_FREE(canonical_filename);
	अगर(!fptr){
		http_api__send_error_response(connection, "Not found.\n", 404);
		वापस MHD_YES;
	}

	काष्ठा stat statbuf;
	अगर(ख_स्थिति(fileno(fptr), &statbuf)){
		ख_बंद(fptr);
		http_api__send_error_response(connection, "Internal server error.\n", 500);
		वापस MHD_YES;
	}

	अगर(!S_ISREG(statbuf.st_mode)){
		ख_बंद(fptr);
		http_api__send_error_response(connection, "Not found.\n", 404);
		वापस MHD_YES;
	}
	uint64_t flen = (uint64_t )statbuf.st_आकार;

	/* Using MHD_create_response_from_fd would be easier here, but is less portable */
	काष्ठा MHD_Response *response = MHD_create_response_from_callback(
			flen,
			65536,
			http_file_read_cb,
			fptr,
			http_file_free_cb);

	क्रमागत MHD_परिणाम ret = MHD_queue_response(connection, 200, response);
	MHD_destroy_response(response);
	वापस ret;
}


अटल क्रमागत MHD_परिणाम http_api__process_api(काष्ठा MHD_Connection *connection, स्थिर अक्षर *url)
{
	अगर(म_भेद(url, "/api/v1/systree") == 0){
		वापस http_api__process_systree(connection);
	}अन्यथा अगर(म_भेद(url, "/api/v1/listeners") == 0){
		वापस http_api__process_listeners(connection);
	}अन्यथा अगर(म_भेद(url, "/api/v1/version") == 0){
		वापस http_api__process_version(connection);
	}अन्यथा{
		वापस http_api__send_error_response(connection, "Not found.\n", 404);
	}
}


अटल पूर्णांक check_access(काष्ठा mosquitto__listener *listener, काष्ठा MHD_Connection *connection, स्थिर अक्षर *url)
{
	काष्ठा mosquitto context = {0};
	पूर्णांक auth_rc, acl_rc = MOSQ_ERR_SUCCESS;

	context.listener = listener;

	context.id = (अक्षर *)"http-api";
	context.username = MHD_basic_auth_get_username_password(connection, &context.password);

	/* Authentication */
	auth_rc = mosquitto_basic_auth(&context);
	अगर(auth_rc == MOSQ_ERR_SUCCESS){
		acl_rc = mosquitto_acl_check(&context, url, 0, शून्य, 0, false, शून्य, MOSQ_ACL_READ);
	}
	MHD_free(context.username);
	MHD_free(context.password);

	अगर(auth_rc || acl_rc){
		स्थिर अक्षर *buf = "Not authorised\n";
		काष्ठा MHD_Response *response = MHD_create_response_from_बफर(म_माप(buf), (व्योम *)buf, MHD_RESPMEM_MUST_COPY);
		MHD_queue_basic_auth_fail_response(connection, "Mosquitto API", response);
		MHD_destroy_response(response);

		वापस MOSQ_ERR_AUTH;
	}

	वापस MOSQ_ERR_SUCCESS;
}

अटल क्रमागत MHD_परिणाम http_api_handler(व्योम *cls, काष्ठा MHD_Connection
		*connection, स्थिर अक्षर *url, स्थिर अक्षर *method, स्थिर अक्षर *version,
		स्थिर अक्षर *upload_तथ्य, माप_प्रकार *upload_तथ्य_आकार, व्योम **con_cls)
{
	UNUSED(version);
	UNUSED(upload_तथ्य);
	UNUSED(upload_तथ्य_आकार);
	UNUSED(con_cls);

	काष्ठा mosquitto__listener *listener = cls;

	अगर(म_भेद(method, "GET") != 0){
		अक्षर *buf = "Invalid HTTP Method\n";
		काष्ठा MHD_Response *response = MHD_create_response_from_बफर(म_माप(buf), (व्योम *)buf, MHD_RESPMEM_MUST_COPY);
		क्रमागत MHD_परिणाम ret = MHD_queue_response(connection, MHD_HTTP_METHOD_NOT_ALLOWED, response);
		MHD_destroy_response(response);
		वापस ret;
	}

	अगर(check_access(listener, connection, url) != MOSQ_ERR_SUCCESS){
		वापस MHD_YES;
	}

	अगर(!strncasecmp(url, "/api/", म_माप("/api/"))){
		वापस http_api__process_api(connection, url);
	}अन्यथा{
		वापस http_api__process_file(listener, connection, url);
	}
}


पूर्णांक http_api__start_local(काष्ठा mosquitto__listener *listener)
{
	listener->security_options = mosquitto_calloc(1, माप(काष्ठा mosquitto__security_options));
	अगर(listener->security_options == शून्य){
		वापस MOSQ_ERR_NOMEM;
	}

	listener->host = mosquitto_strdup("127.0.0.1");
	अगर(!listener->host){
		mosquitto_FREE(listener->security_options);
		वापस MOSQ_ERR_NOMEM;
	}
	listener->port = 9883;

	अगर(db.config->security_options.allow_anonymous == -1){
		listener->security_options->allow_anonymous = true;
	}अन्यथा{
		listener->security_options->allow_anonymous = db.config->security_options.allow_anonymous;
	}

	अगर(listener->http_dir == शून्य && म_माप(HTTP_API_DIR) > 0){
#अगर_घोषित WIN32
		अक्षर *http_dir_canonical = _fullpath(शून्य, HTTP_API_DIR, 0);
#अन्यथा
		अक्षर *http_dir_canonical = realpath(HTTP_API_DIR, शून्य);
#पूर्ण_अगर
		अगर(!http_dir_canonical){
			वापस MOSQ_ERR_NOMEM;
		}
		mosquitto_FREE(listener->http_dir);
		listener->http_dir = mosquitto_strdup(http_dir_canonical);
		mosquitto_FREE(http_dir_canonical);
	}

	वापस http_api__start(listener);
}


पूर्णांक http_api__start(काष्ठा mosquitto__listener *listener)
{
	अचिन्हित पूर्णांक flags = MHD_USE_AUTO_INTERNAL_THREAD;
	स्थिर अक्षर *bind_address;
	uint16_t port = 9883;
	अक्षर *x509_cert = शून्य;
	अक्षर *x509_कुंजी = शून्य;

	अगर(!listener->security_options){
		listener->security_options = mosquitto_calloc(1, माप(काष्ठा mosquitto__security_options));
		अगर(listener->security_options == शून्य){
			वापस MOSQ_ERR_NOMEM;
		}
	}
	listener->protocol = mp_http_api;

	bind_address = listener->host;
	port = listener->port;

#अगर_घोषित WITH_TLS
	अगर(listener->certfile && listener->keyfile){
		अगर(mosquitto_read_file(listener->certfile, false, &x509_cert, शून्य)){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
			वापस MOSQ_ERR_INVAL;
		}
		अगर(mosquitto_read_file(listener->keyfile, false, &x509_कुंजी, शून्य)){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			mosquitto_FREE(x509_cert);
			वापस MOSQ_ERR_INVAL;
		}
		flags |= MHD_USE_TLS;
	}
#पूर्ण_अगर

	अगर(bind_address){
		अक्षर service[10];
		काष्ठा addrinfo hints;
		काष्ठा addrinfo *ainfo, *rp;

		snprintf(service, माप(service), "%d", port);

		स_रखो(&hints, 0, माप(काष्ठा addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = AI_PASSIVE;
		hints.ai_socktype = SOCK_STREAM;

		पूर्णांक rc = getaddrinfo(bind_address, service, &hints, &ainfo);
		अगर(rc){
			log__printf(शून्य, MOSQ_LOG_ERR, "Unable to start http api listener (%s).", म_त्रुटि(त्रुटि_सं));
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_कुंजी);
			वापस MOSQ_ERR_ERRNO;
		}

		क्रम(rp = ainfo; rp; rp = rp->ai_अग्र){
			अगर(rp->ai_family == AF_INET || rp->ai_family == AF_INET6){
				अवरोध;
			}
		}
		अगर(!rp){
			log__printf(शून्य, MOSQ_LOG_ERR, "Unable to start http api listener, could not find address.");
			freeaddrinfo(ainfo);
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_कुंजी);

			वापस MOSQ_ERR_INVAL;
		}

		अगर(rp->ai_family == AF_INET6){
			flags |= MHD_USE_IPv6;
		}
		अगर(x509_cert && x509_कुंजी){
			listener->mhd = MHD_start_daemon(flags, port, शून्य, शून्य, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_कुंजी, x509_कुंजी,
					MHD_OPTION_END);
		}अन्यथा{
			listener->mhd = MHD_start_daemon(flags, port, शून्य, शून्य, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_END);
		}
		freeaddrinfo(ainfo);
	}अन्यथा{
		अगर(x509_cert && x509_कुंजी){
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_ढेर, port, शून्य, शून्य, &http_api_handler, listener,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_कुंजी, x509_कुंजी,
					MHD_OPTION_END);
		}अन्यथा{
			listener->mhd = MHD_start_daemon(flags | MHD_USE_DUAL_ढेर, port, शून्य, शून्य, &http_api_handler, listener,
					MHD_OPTION_END);
		}
	}

	mosquitto_FREE(x509_cert);
	mosquitto_FREE(x509_कुंजी);

	अगर(listener->mhd){
		log__printf(शून्य, MOSQ_LOG_INFO, "Opening http api listen socket on port %d.", port);
		अगर(listener->http_dir){
			log__printf(शून्य, MOSQ_LOG_INFO, "Using http_dir %s", listener->http_dir);
		}
		वापस MOSQ_ERR_SUCCESS;
	}अन्यथा{
		वापस MOSQ_ERR_UNKNOWN;
	}
}


व्योम http_api__stop(काष्ठा mosquitto__listener *listener)
{
	MHD_stop_daemon(listener->mhd);
	listener->mhd = शून्य;
}

#पूर्ण_अगर
