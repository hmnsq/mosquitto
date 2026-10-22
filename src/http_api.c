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

#అన్తర్భావ "config.h"

#నిర్వచితమైతే WITH_HTTP_API

#అన్తర్భావ <assert.h>
#అన్తర్భావ <errno.h>
#అన్తర్భావ <limits.h>
#అన్తర్భావ <microhttpd.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <sys/stat.h>
#నిర్వచితం_కాకపోతే WIN32
#  include <netdb.h>
#అయితే_ముగింపు

#అన్తర్భావ "json_help.h"
#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto/mqtt_protocol.h"
#అన్తర్భావ "sys_tree.h"

#నిర్వచితం_కాకపోతే HTTP_API_DIR
#  define HTTP_API_DIR ""
#అయితే_ముగింపు

నిర్మాణం metric {
	int64_t current;
	int64_t తదుపరి;
	స్థిరరాశి అక్షరం *topic, *topic_alias;
	bool is_max;
};

time_t broker_uptime(శూన్య);
నిర్మాణం MHD_Daemon *mhd = ఖాళీ;

#నిర్వచితమైతే WITH_SYS_వృక్షం
బాహ్య నిర్మాణం metric metrics[mosq_metric_max];
#అయితే_ముగింపు

#నిర్వచించు HTTP_RESPONSE_BUFLEN 10000

#నిర్వచితమైతే WIN32
#నిర్వచించు DIR_SEP '\\'
#నిర్వచించు PATH_MAX MAX_మార్గం
#లేకపోతే
#నిర్వచించు DIR_SEP '/'
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే S_ISREG
#నిర్వచించు S_ISREG(బహుళకం) (((బహుళకం)&S_IFMT) == S_IFREG)
#అయితే_ముగింపు


స్థిర అక్షరం *http__canonical_filename(
		స్థిరరాశి అక్షరం *url,
		స్థిరరాశి అక్షరం *http_dir,
		పూర్ణాంకం *error_సంకేతం)
{
	size_t urllen, slen;
	అక్షరం *filename, *filename_canonical;

	urllen = అక్షరశ్రేణి_పొడవు(url);
	ఒకవేళ(url[urllen-1] == '/'){
		slen = అక్షరశ్రేణి_పొడవు(http_dir) + urllen + అక్షరశ్రేణి_పొడవు("/index.html") + 2;
	}లేకపోతే{
		slen = అక్షరశ్రేణి_పొడవు(http_dir) + urllen + 2;
	}
	filename = mosquitto_malloc(slen);
	ఒకవేళ(!filename){
		*error_సంకేతం = MHD_HTTP_అంతర్గత_SERVER_ERROR;
		తిరిగి ఖాళీ;
	}
	ఒకవేళ(((అక్షరం *)url)[urllen-1] == '/'){
		snprintf(filename, slen, "%s%c%sindex.html", http_dir, DIR_SEP, url);
	}లేకపోతే{
		snprintf(filename, slen, "%s%c%s", http_dir, DIR_SEP, url);
	}


	/* Get canonical path and check it is within our http_dir */
	filename_canonical = mosquitto_calloc(1, PATH_MAX);
	ఒకవేళ(!filename_canonical){
		*error_సంకేతం = MHD_HTTP_అంతర్గత_SERVER_ERROR;
		తిరిగి ఖాళీ;
	}
#నిర్వచితమైతే WIN32
	అక్షరం *resolved = _fullpath(filename_canonical, filename, PATH_MAX);
	mosquitto_FREE(filename);
	ఒకవేళ(!resolved){
		*error_సంకేతం = MHD_HTTP_అంతర్గత_SERVER_ERROR;
		mosquitto_FREE(filename_canonical);
		తిరిగి ఖాళీ;
	}
#లేకపోతే
	అక్షరం *resolved = realpath(filename, filename_canonical);
	mosquitto_FREE(filename);
	ఒకవేళ(!resolved){
		ఒకవేళ(errno == EACCES){
			*error_సంకేతం = MHD_HTTP_FORBIDDEN;
		}లేకపోతే ఒకవేళ(errno == EINVAL || errno == EIO || errno == ELOOP){
			*error_సంకేతం = MHD_HTTP_అంతర్గత_SERVER_ERROR;
		}లేకపోతే ఒకవేళ(errno == ENAMETOOLONG){
			*error_సంకేతం = MHD_HTTP_URI_TOO_LONG;
		}లేకపోతే ఒకవేళ(errno == ENOENT || errno == ENOTDIR){
			*error_సంకేతం = MHD_HTTP_NOT_దొరికింది;
		}
		mosquitto_FREE(filename_canonical);
		తిరిగి ఖాళీ;
	}
#అయితే_ముగింపు
	ఒకవేళ(అక్షరశ్రేణి_పోలిక(http_dir, filename_canonical, అక్షరశ్రేణి_పొడవు(http_dir))){
		/* Requested file isn't within http_dir, deny access because it's not found. */
		mosquitto_FREE(filename_canonical);
		*error_సంకేతం = MHD_HTTP_NOT_దొరికింది;
		తిరిగి ఖాళీ;
	}

	తిరిగి filename_canonical;
}

స్థిర గణన MHD_ఫలితం http_api__send_error_response(నిర్మాణం MHD_Connection *connection, స్థిరరాశి అక్షరం *error_str, గుర్తులేని పూర్ణాంకం error_సంకేతం)
{
	నిర్మాణం MHD_Response *response = MHD_create_response_from_బఫరు(అక్షరశ్రేణి_పొడవు(error_str), (శూన్య *)error_str, MHD_RESPMEM_MUST_COPY);
	గణన MHD_ఫలితం ret = MHD_వరుస_response(connection, error_సంకేతం, response);
	MHD_destroy_response(response);
	తిరిగి ret;
}

స్థిర గణన MHD_ఫలితం http_api_send_response_with_శీర్షికలు(నిర్మాణం MHD_Connection *connection, స్థిరరాశి అక్షరం *buf)
{
	గణన MHD_ఫలితం ret;
	నిర్మాణం MHD_Response *response = MHD_create_response_from_బఫరు(అక్షరశ్రేణి_పొడవు(buf), (శూన్య *)buf, MHD_RESPMEM_MUST_COPY);

	ret = MHD_కూడు_response_శీర్షిక(response, "Access-Control-Allow-Origin", "*");
	ఒకవేళ(ret != MHD_YES){
		MHD_destroy_response(response);
		తిరిగి ret;
	}
	ret = MHD_వరుస_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	తిరిగి ret;
}

స్థిర గణన MHD_ఫలితం http_api_process_సంస్కరణ(నిర్మాణం MHD_Connection *connection)
{
	తిరిగి http_api_send_response_with_శీర్షికలు(connection, సంస్కరణ);
}

స్థిర గణన MHD_ఫలితం http_api__process_listeners(నిర్మాణం MHD_Connection *connection)
{
	అక్షరం *buf;
	గణన MHD_ఫలితం ret;

	cJSON *j_వృక్షం = cJSON_CreateObject();
	ఒకవేళ(!j_వృక్షం){
		తిరిగి http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_అంతర్గత_SERVER_ERROR);
	}

	cJSON *j_listeners = c_JSON_కూడు_Array_గమ్యం_Object(j_వృక్షం, "listeners");
	ఒకవేళ(!j_listeners){
		cJSON_Delete(j_వృక్షం);
		తిరిగి http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_అంతర్గత_SERVER_ERROR);
	}

	కొరకు(పూర్ణాంకం i=0; i<db.config->listener_లెక్క; i++){
		cJSON *j_listener = cJSON_CreateObject();
		ఒకవేళ(!j_listener){
			cJSON_Delete(j_వృక్షం);
			తిరిగి http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_అంతర్గత_SERVER_ERROR);
		}
		c_JSON_కూడు_అంశం_గమ్యం_Array(j_listeners, j_listener);

		నిర్మాణం mosquitto__listener *listener = &db.config->listeners[i];
#నిర్వచితమైతే WITH_UNIX_SOCKETS
		ఒకవేళ(listener->unix_socket_మార్గం){
			c_JSON_కూడు_String_గమ్యం_Object(j_listener, "path", listener->unix_socket_మార్గం);
		}లేకపోతే
#అయితే_ముగింపు
		{
			c_JSON_కూడు_Int_గమ్యం_Object(j_listener, "port", listener->పోర్టు);
		}

		ఎంపిక(listener->protocol){
			స్థితి mp_mqtt:
				c_JSON_కూడు_String_గమ్యం_Object(j_listener, "protocol", "mqtt");
				ఆపు;
			స్థితి mp_mqttsn:
				c_JSON_కూడు_String_గమ్యం_Object(j_listener, "protocol", "mqtt-sn");
				ఆపు;
			స్థితి mp_websockets:
				c_JSON_కూడు_String_గమ్యం_Object(j_listener, "protocol", "websockets");
				ఆపు;
			స్థితి mp_http_api:
				c_JSON_కూడు_String_గమ్యం_Object(j_listener, "protocol", "httpapi");
				ఆపు;
		}

#నిర్వచితమైతే WITH_TLS
		c_JSON_కూడు_Bool_గమ్యం_Object(j_listener, "tls", listener->certfile && listener->keyfile);
		c_JSON_కూడు_Bool_గమ్యం_Object(j_listener, "mtls", listener->require_certificate);
#లేకపోతే
		c_JSON_కూడు_Bool_గమ్యం_Object(j_listener, "tls", false);
		c_JSON_కూడు_Bool_గమ్యం_Object(j_listener, "mtls", false);
#అయితే_ముగింపు
		ఒకవేళ(listener->security_options->allow_anonymous == -1){
			c_JSON_కూడు_Bool_గమ్యం_Object(j_listener, "allow_anonymous", db.config->security_options.allow_anonymous);
		}లేకపోతే{
			c_JSON_కూడు_Bool_గమ్యం_Object(j_listener, "allow_anonymous", listener->security_options->allow_anonymous);
		}
	}
	buf = cJSON_Print(j_వృక్షం);
	cJSON_Delete(j_వృక్షం);
	ఒకవేళ(buf){
		ret = http_api_send_response_with_శీర్షికలు(connection, buf);
		విడుదల(buf);
	}లేకపోతే{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_అంతర్గత_SERVER_ERROR);
	}

	తిరిగి ret;
}


స్థిర గణన MHD_ఫలితం http_api__process_systree(నిర్మాణం MHD_Connection *connection)
{
	గణన MHD_ఫలితం ret;
#నిర్వచితమైతే WITH_SYS_వృక్షం
	అక్షరం *buf;

	cJSON *j_వృక్షం = cJSON_CreateObject();
	కొరకు(పూర్ణాంకం i=0; i<mosq_metric_max; i++){
		ఒకవేళ(metrics[i].topic){
			c_JSON_కూడు_Int_గమ్యం_Object(j_వృక్షం, metrics[i].topic, metrics[i].current);
		}
	}
	c_JSON_కూడు_Int_గమ్యం_Object(j_వృక్షం, "$SYS/broker/uptime", broker_uptime());

	buf = cJSON_Print(j_వృక్షం);
	cJSON_Delete(j_వృక్షం);

	ఒకవేళ(buf){
		ret = http_api_send_response_with_శీర్షికలు(connection, buf);
		విడుదల(buf);
	}లేకపోతే{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_అంతర్గత_SERVER_ERROR);
	}
#లేకపోతే
	ret = http_api__send_error_response(connection, "Not found.\n", 404);
#అయితే_ముగింపు

	తిరిగి ret;
}


స్థిర ssize_t http_file_read_cb(శూన్య *cls, uint64_t pos, అక్షరం *buf, size_t max)
{
	FILE *fptr = cls;
	ఒకవేళ(fseek(fptr, (దీర్ఘ )pos, SEEK_SET) < 0){
		తిరిగి -1;
	}
	తిరిగి (ssize_t )fread(buf, 1, max, fptr);
}


స్థిర శూన్య http_file_free_cb(శూన్య *cls)
{
	FILE *fptr = cls;
	fclose(fptr);
}


స్థిర గణన MHD_ఫలితం http_api__process_file(నిర్మాణం mosquitto__listener *listener, నిర్మాణం MHD_Connection *connection, స్థిరరాశి అక్షరం *url)
{
	పూర్ణాంకం error_సంకేతం = MHD_HTTP_అంతర్గత_SERVER_ERROR;
	అక్షరం *canonical_filename;

	ఒకవేళ(!listener || !listener->http_dir){
		http_api__send_error_response(connection, "Not found.\n", 404);
		తిరిగి MHD_YES;
	}

	canonical_filename = http__canonical_filename(url, listener->http_dir, &error_సంకేతం);
	ఒకవేళ(!canonical_filename){
		http_api__send_error_response(connection, "Not found.\n", 404);
		తిరిగి MHD_YES;
	}

	FILE *fptr = fopen(canonical_filename, "rb");
	mosquitto_FREE(canonical_filename);
	ఒకవేళ(!fptr){
		http_api__send_error_response(connection, "Not found.\n", 404);
		తిరిగి MHD_YES;
	}

	నిర్మాణం stat statbuf;
	ఒకవేళ(fstat(fileno(fptr), &statbuf)){
		fclose(fptr);
		http_api__send_error_response(connection, "Internal server error.\n", 500);
		తిరిగి MHD_YES;
	}

	ఒకవేళ(!S_ISREG(statbuf.st_బహుళకం)){
		fclose(fptr);
		http_api__send_error_response(connection, "Not found.\n", 404);
		తిరిగి MHD_YES;
	}
	uint64_t flen = (uint64_t )statbuf.st_కొలత;

	/* Using MHD_create_response_from_fd would be easier here, but is less portable */
	నిర్మాణం MHD_Response *response = MHD_create_response_from_callback(
			flen,
			65536,
			http_file_read_cb,
			fptr,
			http_file_free_cb);

	గణన MHD_ఫలితం ret = MHD_వరుస_response(connection, 200, response);
	MHD_destroy_response(response);
	తిరిగి ret;
}


స్థిర గణన MHD_ఫలితం http_api__process_api(నిర్మాణం MHD_Connection *connection, స్థిరరాశి అక్షరం *url)
{
	ఒకవేళ(strcmp(url, "/api/v1/systree") == 0){
		తిరిగి http_api__process_systree(connection);
	}లేకపోతే ఒకవేళ(strcmp(url, "/api/v1/listeners") == 0){
		తిరిగి http_api__process_listeners(connection);
	}లేకపోతే ఒకవేళ(strcmp(url, "/api/v1/version") == 0){
		తిరిగి http_api_process_సంస్కరణ(connection);
	}లేకపోతే{
		తిరిగి http_api__send_error_response(connection, "Not found.\n", 404);
	}
}


స్థిర పూర్ణాంకం check_access(నిర్మాణం mosquitto__listener *listener, నిర్మాణం MHD_Connection *connection, స్థిరరాశి అక్షరం *url)
{
	నిర్మాణం mosquitto context = {0};
	పూర్ణాంకం auth_rc, acl_rc = MOSQ_ERR_SUCCESS;

	context.listener = listener;

	context.id = (అక్షరం *)"http-api";
	context.username = MHD_basic_auth_get_username_password(connection, &context.password);

	/* Authentication */
	auth_rc = mosquitto_basic_auth(&context);
	ఒకవేళ(auth_rc == MOSQ_ERR_SUCCESS){
		acl_rc = mosquitto_acl_check(&context, url, 0, ఖాళీ, 0, false, ఖాళీ, MOSQ_ACL_READ);
	}
	MHD_free(context.username);
	MHD_free(context.password);

	ఒకవేళ(auth_rc || acl_rc){
		స్థిరరాశి అక్షరం *buf = "Not authorised\n";
		నిర్మాణం MHD_Response *response = MHD_create_response_from_బఫరు(అక్షరశ్రేణి_పొడవు(buf), (శూన్య *)buf, MHD_RESPMEM_MUST_COPY);
		MHD_వరుస_basic_auth_fail_response(connection, "Mosquitto API", response);
		MHD_destroy_response(response);

		తిరిగి MOSQ_ERR_AUTH;
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}

స్థిర గణన MHD_ఫలితం http_api_handler(శూన్య *cls, నిర్మాణం MHD_Connection
		*connection, స్థిరరాశి అక్షరం *url, స్థిరరాశి అక్షరం *పద్ధతి, స్థిరరాశి అక్షరం *సంస్కరణ_2,
		స్థిరరాశి అక్షరం *upload_దత్తాంశం, size_t *upload_దత్తాంశం_కొలత, శూన్య **con_cls)
{
	వాడని(సంస్కరణ_2);
	వాడని(upload_దత్తాంశం);
	వాడని(upload_దత్తాంశం_కొలత);
	వాడని(con_cls);

	నిర్మాణం mosquitto__listener *listener = cls;

	ఒకవేళ(strcmp(పద్ధతి, "GET") != 0){
		అక్షరం *buf = "Invalid HTTP Method\n";
		నిర్మాణం MHD_Response *response = MHD_create_response_from_బఫరు(అక్షరశ్రేణి_పొడవు(buf), (శూన్య *)buf, MHD_RESPMEM_MUST_COPY);
		గణన MHD_ఫలితం ret = MHD_వరుస_response(connection, MHD_HTTP_పద్ధతి_NOT_ALLOWED, response);
		MHD_destroy_response(response);
		తిరిగి ret;
	}

	ఒకవేళ(check_access(listener, connection, url) != MOSQ_ERR_SUCCESS){
		తిరిగి MHD_YES;
	}

	ఒకవేళ(!strncasecmp(url, "/api/", అక్షరశ్రేణి_పొడవు("/api/"))){
		తిరిగి http_api__process_api(connection, url);
	}లేకపోతే{
		తిరిగి http_api__process_file(listener, connection, url);
	}
}


పూర్ణాంకం http_api_ప్రారంభం_local(నిర్మాణం mosquitto__listener *listener)
{
	listener->security_options = mosquitto_calloc(1, పరిమాణం(నిర్మాణం mosquitto__security_options));
	ఒకవేళ(listener->security_options == ఖాళీ){
		తిరిగి MOSQ_ERR_NOMEM;
	}

	listener->host = mosquitto_strdup("127.0.0.1");
	ఒకవేళ(!listener->host){
		mosquitto_FREE(listener->security_options);
		తిరిగి MOSQ_ERR_NOMEM;
	}
	listener->పోర్టు = 9883;

	ఒకవేళ(db.config->security_options.allow_anonymous == -1){
		listener->security_options->allow_anonymous = true;
	}లేకపోతే{
		listener->security_options->allow_anonymous = db.config->security_options.allow_anonymous;
	}

	ఒకవేళ(listener->http_dir == ఖాళీ && అక్షరశ్రేణి_పొడవు(HTTP_API_DIR) > 0){
#నిర్వచితమైతే WIN32
		అక్షరం *http_dir_canonical = _fullpath(ఖాళీ, HTTP_API_DIR, 0);
#లేకపోతే
		అక్షరం *http_dir_canonical = realpath(HTTP_API_DIR, ఖాళీ);
#అయితే_ముగింపు
		ఒకవేళ(!http_dir_canonical){
			తిరిగి MOSQ_ERR_NOMEM;
		}
		mosquitto_FREE(listener->http_dir);
		listener->http_dir = mosquitto_strdup(http_dir_canonical);
		mosquitto_FREE(http_dir_canonical);
	}

	తిరిగి http_api_ప్రారంభం(listener);
}


పూర్ణాంకం http_api_ప్రారంభం(నిర్మాణం mosquitto__listener *listener)
{
	గుర్తులేని పూర్ణాంకం flags = MHD_USE_AUTO_అంతర్గత_THREAD;
	స్థిరరాశి అక్షరం *bind_చిరునామా;
	uint16_t పోర్టు = 9883;
	అక్షరం *x509_cert = ఖాళీ;
	అక్షరం *x509_కీలకం = ఖాళీ;

	ఒకవేళ(!listener->security_options){
		listener->security_options = mosquitto_calloc(1, పరిమాణం(నిర్మాణం mosquitto__security_options));
		ఒకవేళ(listener->security_options == ఖాళీ){
			తిరిగి MOSQ_ERR_NOMEM;
		}
	}
	listener->protocol = mp_http_api;

	bind_చిరునామా = listener->host;
	పోర్టు = listener->పోర్టు;

#నిర్వచితమైతే WITH_TLS
	ఒకవేళ(listener->certfile && listener->keyfile){
		ఒకవేళ(mosquitto_read_file(listener->certfile, false, &x509_cert, ఖాళీ)){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
			తిరిగి MOSQ_ERR_INVAL;
		}
		ఒకవేళ(mosquitto_read_file(listener->keyfile, false, &x509_కీలకం, ఖాళీ)){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			mosquitto_FREE(x509_cert);
			తిరిగి MOSQ_ERR_INVAL;
		}
		flags |= MHD_USE_TLS;
	}
#అయితే_ముగింపు

	ఒకవేళ(bind_చిరునామా){
		అక్షరం service[10];
		నిర్మాణం addrinfo hints;
		నిర్మాణం addrinfo *ainfo, *rp;

		snprintf(service, పరిమాణం(service), "%d", పోర్టు);

		memset(&hints, 0, పరిమాణం(నిర్మాణం addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = AI_PASSIVE;
		hints.ai_socktype = SOCK_STREAM;

		పూర్ణాంకం rc = getaddrinfo(bind_చిరునామా, service, &hints, &ainfo);
		ఒకవేళ(rc){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Unable to start http api listener (%s).", strerror(errno));
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_కీలకం);
			తిరిగి MOSQ_ERR_ERRNO;
		}

		కొరకు(rp = ainfo; rp; rp = rp->ai_తదుపరి){
			ఒకవేళ(rp->ai_family == AF_INET || rp->ai_family == AF_INET6){
				ఆపు;
			}
		}
		ఒకవేళ(!rp){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Unable to start http api listener, could not find address.");
			freeaddrinfo(ainfo);
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_కీలకం);

			తిరిగి MOSQ_ERR_INVAL;
		}

		ఒకవేళ(rp->ai_family == AF_INET6){
			flags |= MHD_USE_IPv6;
		}
		ఒకవేళ(x509_cert && x509_కీలకం){
			listener->mhd = MHD_ప్రారంభం_daemon(flags, పోర్టు, ఖాళీ, ఖాళీ, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_కీలకం, x509_కీలకం,
					MHD_OPTION_చివర);
		}లేకపోతే{
			listener->mhd = MHD_ప్రారంభం_daemon(flags, పోర్టు, ఖాళీ, ఖాళీ, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_చివర);
		}
		freeaddrinfo(ainfo);
	}లేకపోతే{
		ఒకవేళ(x509_cert && x509_కీలకం){
			listener->mhd = MHD_ప్రారంభం_daemon(flags | MHD_USE_DUAL_దొంతర, పోర్టు, ఖాళీ, ఖాళీ, &http_api_handler, listener,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_కీలకం, x509_కీలకం,
					MHD_OPTION_చివర);
		}లేకపోతే{
			listener->mhd = MHD_ప్రారంభం_daemon(flags | MHD_USE_DUAL_దొంతర, పోర్టు, ఖాళీ, ఖాళీ, &http_api_handler, listener,
					MHD_OPTION_చివర);
		}
	}

	mosquitto_FREE(x509_cert);
	mosquitto_FREE(x509_కీలకం);

	ఒకవేళ(listener->mhd){
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Opening http api listen socket on port %d.", పోర్టు);
		ఒకవేళ(listener->http_dir){
			log__printf(ఖాళీ, MOSQ_LOG_INFO, "Using http_dir %s", listener->http_dir);
		}
		తిరిగి MOSQ_ERR_SUCCESS;
	}లేకపోతే{
		తిరిగి MOSQ_ERR_UNKNOWN;
	}
}


శూన్య http_api__stop(నిర్మాణం mosquitto__listener *listener)
{
	MHD_stop_daemon(listener->mhd);
	listener->mhd = ఖాళీ;
}

#అయితే_ముగింపు
