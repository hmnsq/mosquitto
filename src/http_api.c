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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_HTTP_API

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <limits.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <microhttpd.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
#  include <netdb.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "json_help.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sys_tree.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HTTP_API_DIR
#  define HTTP_API_DIR ""
#ਜੇ_ਅੰਤ

ਬਣਤਰ metric {
	int64_t current;
	int64_t ਅਗਲਾ;
	ਅਚਲ ਅੱਖਰ *topic, *topic_alias;
	bool is_max;
};

time_t broker_uptime(ਖਾਲੀ);
ਬਣਤਰ MHD_Daemon *mhd = ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_SYS_ਰੁੱਖ
ਬਾਹਰੀ ਬਣਤਰ metric metrics[mosq_metric_max];
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTP_RESPONSE_BUFLEN 10000

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIR_SEP '\\'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PATH_MAX MAX_ਰਸਤਾ
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIR_SEP '/'
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ S_ISREG
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ S_ISREG(ਬਹੁਲਕ) (((ਬਹੁਲਕ)&S_IFMT) == S_IFREG)
#ਜੇ_ਅੰਤ


ਸਥਿਰ ਅੱਖਰ *http__canonical_filename(
		ਅਚਲ ਅੱਖਰ *url,
		ਅਚਲ ਅੱਖਰ *http_dir,
		ਪੂਰਨ *error_ਕੋਡ)
{
	size_t urllen, slen;
	ਅੱਖਰ *filename, *filename_canonical;

	urllen = ਲੜੀ_ਲੰਬਾਈ(url);
	ਜੇ(url[urllen-1] == '/'){
		slen = ਲੜੀ_ਲੰਬਾਈ(http_dir) + urllen + ਲੜੀ_ਲੰਬਾਈ("/index.html") + 2;
	}ਨਹੀਂਤਾਂ{
		slen = ਲੜੀ_ਲੰਬਾਈ(http_dir) + urllen + 2;
	}
	filename = mosquitto_malloc(slen);
	ਜੇ(!filename){
		*error_ਕੋਡ = MHD_HTTP_ਅੰਦਰੂਨੀ_SERVER_ERROR;
		ਵਾਪਸ ਨਿਰਾ;
	}
	ਜੇ(((ਅੱਖਰ *)url)[urllen-1] == '/'){
		snprintf(filename, slen, "%s%c%sindex.html", http_dir, DIR_SEP, url);
	}ਨਹੀਂਤਾਂ{
		snprintf(filename, slen, "%s%c%s", http_dir, DIR_SEP, url);
	}


	/* Get canonical path and check it is within our http_dir */
	filename_canonical = mosquitto_calloc(1, PATH_MAX);
	ਜੇ(!filename_canonical){
		*error_ਕੋਡ = MHD_HTTP_ਅੰਦਰੂਨੀ_SERVER_ERROR;
		ਵਾਪਸ ਨਿਰਾ;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
	ਅੱਖਰ *resolved = _fullpath(filename_canonical, filename, PATH_MAX);
	mosquitto_FREE(filename);
	ਜੇ(!resolved){
		*error_ਕੋਡ = MHD_HTTP_ਅੰਦਰੂਨੀ_SERVER_ERROR;
		mosquitto_FREE(filename_canonical);
		ਵਾਪਸ ਨਿਰਾ;
	}
#ਨਹੀਂਤਾਂ
	ਅੱਖਰ *resolved = realpath(filename, filename_canonical);
	mosquitto_FREE(filename);
	ਜੇ(!resolved){
		ਜੇ(errno == EACCES){
			*error_ਕੋਡ = MHD_HTTP_FORBIDDEN;
		}ਨਹੀਂਤਾਂ ਜੇ(errno == EINVAL || errno == EIO || errno == ELOOP){
			*error_ਕੋਡ = MHD_HTTP_ਅੰਦਰੂਨੀ_SERVER_ERROR;
		}ਨਹੀਂਤਾਂ ਜੇ(errno == ENAMETOOLONG){
			*error_ਕੋਡ = MHD_HTTP_URI_TOO_LONG;
		}ਨਹੀਂਤਾਂ ਜੇ(errno == ENOENT || errno == ENOTDIR){
			*error_ਕੋਡ = MHD_HTTP_NOT_ਮਿਲਿਆ;
		}
		mosquitto_FREE(filename_canonical);
		ਵਾਪਸ ਨਿਰਾ;
	}
#ਜੇ_ਅੰਤ
	ਜੇ(ਲੜੀ_ਤੁਲਨਾ(http_dir, filename_canonical, ਲੜੀ_ਲੰਬਾਈ(http_dir))){
		/* Requested file isn't within http_dir, deny access because it's not found. */
		mosquitto_FREE(filename_canonical);
		*error_ਕੋਡ = MHD_HTTP_NOT_ਮਿਲਿਆ;
		ਵਾਪਸ ਨਿਰਾ;
	}

	ਵਾਪਸ filename_canonical;
}

ਸਥਿਰ ਗਿਣਤੀ MHD_ਨਤੀਜਾ http_api__send_error_response(ਬਣਤਰ MHD_Connection *connection, ਅਚਲ ਅੱਖਰ *error_str, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ error_ਕੋਡ)
{
	ਬਣਤਰ MHD_Response *response = MHD_create_response_from_ਬਫਰ(ਲੜੀ_ਲੰਬਾਈ(error_str), (ਖਾਲੀ *)error_str, MHD_RESPMEM_MUST_COPY);
	ਗਿਣਤੀ MHD_ਨਤੀਜਾ ret = MHD_ਕਤਾਰ_response(connection, error_ਕੋਡ, response);
	MHD_destroy_response(response);
	ਵਾਪਸ ret;
}

ਸਥਿਰ ਗਿਣਤੀ MHD_ਨਤੀਜਾ http_api_send_response_with_ਸਿਰਲੇਖਾਂ(ਬਣਤਰ MHD_Connection *connection, ਅਚਲ ਅੱਖਰ *buf)
{
	ਗਿਣਤੀ MHD_ਨਤੀਜਾ ret;
	ਬਣਤਰ MHD_Response *response = MHD_create_response_from_ਬਫਰ(ਲੜੀ_ਲੰਬਾਈ(buf), (ਖਾਲੀ *)buf, MHD_RESPMEM_MUST_COPY);

	ret = MHD_ਜੋੜੋ_response_ਸਿਰਲੇਖ(response, "Access-Control-Allow-Origin", "*");
	ਜੇ(ret != MHD_YES){
		MHD_destroy_response(response);
		ਵਾਪਸ ret;
	}
	ret = MHD_ਕਤਾਰ_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	ਵਾਪਸ ret;
}

ਸਥਿਰ ਗਿਣਤੀ MHD_ਨਤੀਜਾ http_api_process_ਸੰਸਕਰਣ(ਬਣਤਰ MHD_Connection *connection)
{
	ਵਾਪਸ http_api_send_response_with_ਸਿਰਲੇਖਾਂ(connection, ਸੰਸਕਰਣ);
}

ਸਥਿਰ ਗਿਣਤੀ MHD_ਨਤੀਜਾ http_api__process_listeners(ਬਣਤਰ MHD_Connection *connection)
{
	ਅੱਖਰ *buf;
	ਗਿਣਤੀ MHD_ਨਤੀਜਾ ret;

	cJSON *j_ਰੁੱਖ = cJSON_CreateObject();
	ਜੇ(!j_ਰੁੱਖ){
		ਵਾਪਸ http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_ਅੰਦਰੂਨੀ_SERVER_ERROR);
	}

	cJSON *j_listeners = c_JSON_ਜੋੜੋ_Array_ਮੰਜ਼ਿਲ_Object(j_ਰੁੱਖ, "listeners");
	ਜੇ(!j_listeners){
		cJSON_Delete(j_ਰੁੱਖ);
		ਵਾਪਸ http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_ਅੰਦਰੂਨੀ_SERVER_ERROR);
	}

	ਲਈ(ਪੂਰਨ i=0; i<db.config->listener_ਗਿਣਤੀ; i++){
		cJSON *j_listener = cJSON_CreateObject();
		ਜੇ(!j_listener){
			cJSON_Delete(j_ਰੁੱਖ);
			ਵਾਪਸ http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_ਅੰਦਰੂਨੀ_SERVER_ERROR);
		}
		c_JSON_ਜੋੜੋ_ਵਸਤੂ_ਮੰਜ਼ਿਲ_Array(j_listeners, j_listener);

		ਬਣਤਰ mosquitto__listener *listener = &db.config->listeners[i];
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_UNIX_SOCKETS
		ਜੇ(listener->unix_socket_ਰਸਤਾ){
			c_JSON_ਜੋੜੋ_String_ਮੰਜ਼ਿਲ_Object(j_listener, "path", listener->unix_socket_ਰਸਤਾ);
		}ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		{
			c_JSON_ਜੋੜੋ_Int_ਮੰਜ਼ਿਲ_Object(j_listener, "port", listener->ਪੋਰਟ);
		}

		ਚੋਣ(listener->protocol){
			ਹਾਲਤ mp_mqtt:
				c_JSON_ਜੋੜੋ_String_ਮੰਜ਼ਿਲ_Object(j_listener, "protocol", "mqtt");
				ਰੋਕੋ;
			ਹਾਲਤ mp_mqttsn:
				c_JSON_ਜੋੜੋ_String_ਮੰਜ਼ਿਲ_Object(j_listener, "protocol", "mqtt-sn");
				ਰੋਕੋ;
			ਹਾਲਤ mp_websockets:
				c_JSON_ਜੋੜੋ_String_ਮੰਜ਼ਿਲ_Object(j_listener, "protocol", "websockets");
				ਰੋਕੋ;
			ਹਾਲਤ mp_http_api:
				c_JSON_ਜੋੜੋ_String_ਮੰਜ਼ਿਲ_Object(j_listener, "protocol", "httpapi");
				ਰੋਕੋ;
		}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
		c_JSON_ਜੋੜੋ_Bool_ਮੰਜ਼ਿਲ_Object(j_listener, "tls", listener->certfile && listener->keyfile);
		c_JSON_ਜੋੜੋ_Bool_ਮੰਜ਼ਿਲ_Object(j_listener, "mtls", listener->require_certificate);
#ਨਹੀਂਤਾਂ
		c_JSON_ਜੋੜੋ_Bool_ਮੰਜ਼ਿਲ_Object(j_listener, "tls", false);
		c_JSON_ਜੋੜੋ_Bool_ਮੰਜ਼ਿਲ_Object(j_listener, "mtls", false);
#ਜੇ_ਅੰਤ
		ਜੇ(listener->security_options->allow_anonymous == -1){
			c_JSON_ਜੋੜੋ_Bool_ਮੰਜ਼ਿਲ_Object(j_listener, "allow_anonymous", db.config->security_options.allow_anonymous);
		}ਨਹੀਂਤਾਂ{
			c_JSON_ਜੋੜੋ_Bool_ਮੰਜ਼ਿਲ_Object(j_listener, "allow_anonymous", listener->security_options->allow_anonymous);
		}
	}
	buf = cJSON_Print(j_ਰੁੱਖ);
	cJSON_Delete(j_ਰੁੱਖ);
	ਜੇ(buf){
		ret = http_api_send_response_with_ਸਿਰਲੇਖਾਂ(connection, buf);
		ਮੁਕਤ(buf);
	}ਨਹੀਂਤਾਂ{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_ਅੰਦਰੂਨੀ_SERVER_ERROR);
	}

	ਵਾਪਸ ret;
}


ਸਥਿਰ ਗਿਣਤੀ MHD_ਨਤੀਜਾ http_api__process_systree(ਬਣਤਰ MHD_Connection *connection)
{
	ਗਿਣਤੀ MHD_ਨਤੀਜਾ ret;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_SYS_ਰੁੱਖ
	ਅੱਖਰ *buf;

	cJSON *j_ਰੁੱਖ = cJSON_CreateObject();
	ਲਈ(ਪੂਰਨ i=0; i<mosq_metric_max; i++){
		ਜੇ(metrics[i].topic){
			c_JSON_ਜੋੜੋ_Int_ਮੰਜ਼ਿਲ_Object(j_ਰੁੱਖ, metrics[i].topic, metrics[i].current);
		}
	}
	c_JSON_ਜੋੜੋ_Int_ਮੰਜ਼ਿਲ_Object(j_ਰੁੱਖ, "$SYS/broker/uptime", broker_uptime());

	buf = cJSON_Print(j_ਰੁੱਖ);
	cJSON_Delete(j_ਰੁੱਖ);

	ਜੇ(buf){
		ret = http_api_send_response_with_ਸਿਰਲੇਖਾਂ(connection, buf);
		ਮੁਕਤ(buf);
	}ਨਹੀਂਤਾਂ{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_ਅੰਦਰੂਨੀ_SERVER_ERROR);
	}
#ਨਹੀਂਤਾਂ
	ret = http_api__send_error_response(connection, "Not found.\n", 404);
#ਜੇ_ਅੰਤ

	ਵਾਪਸ ret;
}


ਸਥਿਰ ssize_t http_file_read_cb(ਖਾਲੀ *cls, uint64_t pos, ਅੱਖਰ *buf, size_t max)
{
	FILE *fptr = cls;
	ਜੇ(fseek(fptr, (ਲੰਮਾ )pos, SEEK_SET) < 0){
		ਵਾਪਸ -1;
	}
	ਵਾਪਸ (ssize_t )fread(buf, 1, max, fptr);
}


ਸਥਿਰ ਖਾਲੀ http_file_free_cb(ਖਾਲੀ *cls)
{
	FILE *fptr = cls;
	fclose(fptr);
}


ਸਥਿਰ ਗਿਣਤੀ MHD_ਨਤੀਜਾ http_api__process_file(ਬਣਤਰ mosquitto__listener *listener, ਬਣਤਰ MHD_Connection *connection, ਅਚਲ ਅੱਖਰ *url)
{
	ਪੂਰਨ error_ਕੋਡ = MHD_HTTP_ਅੰਦਰੂਨੀ_SERVER_ERROR;
	ਅੱਖਰ *canonical_filename;

	ਜੇ(!listener || !listener->http_dir){
		http_api__send_error_response(connection, "Not found.\n", 404);
		ਵਾਪਸ MHD_YES;
	}

	canonical_filename = http__canonical_filename(url, listener->http_dir, &error_ਕੋਡ);
	ਜੇ(!canonical_filename){
		http_api__send_error_response(connection, "Not found.\n", 404);
		ਵਾਪਸ MHD_YES;
	}

	FILE *fptr = fopen(canonical_filename, "rb");
	mosquitto_FREE(canonical_filename);
	ਜੇ(!fptr){
		http_api__send_error_response(connection, "Not found.\n", 404);
		ਵਾਪਸ MHD_YES;
	}

	ਬਣਤਰ stat statbuf;
	ਜੇ(fstat(fileno(fptr), &statbuf)){
		fclose(fptr);
		http_api__send_error_response(connection, "Internal server error.\n", 500);
		ਵਾਪਸ MHD_YES;
	}

	ਜੇ(!S_ISREG(statbuf.st_ਬਹੁਲਕ)){
		fclose(fptr);
		http_api__send_error_response(connection, "Not found.\n", 404);
		ਵਾਪਸ MHD_YES;
	}
	uint64_t flen = (uint64_t )statbuf.st_ਮਾਪ;

	/* Using MHD_create_response_from_fd would be easier here, but is less portable */
	ਬਣਤਰ MHD_Response *response = MHD_create_response_from_callback(
			flen,
			65536,
			http_file_read_cb,
			fptr,
			http_file_free_cb);

	ਗਿਣਤੀ MHD_ਨਤੀਜਾ ret = MHD_ਕਤਾਰ_response(connection, 200, response);
	MHD_destroy_response(response);
	ਵਾਪਸ ret;
}


ਸਥਿਰ ਗਿਣਤੀ MHD_ਨਤੀਜਾ http_api__process_api(ਬਣਤਰ MHD_Connection *connection, ਅਚਲ ਅੱਖਰ *url)
{
	ਜੇ(strcmp(url, "/api/v1/systree") == 0){
		ਵਾਪਸ http_api__process_systree(connection);
	}ਨਹੀਂਤਾਂ ਜੇ(strcmp(url, "/api/v1/listeners") == 0){
		ਵਾਪਸ http_api__process_listeners(connection);
	}ਨਹੀਂਤਾਂ ਜੇ(strcmp(url, "/api/v1/version") == 0){
		ਵਾਪਸ http_api_process_ਸੰਸਕਰਣ(connection);
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ http_api__send_error_response(connection, "Not found.\n", 404);
	}
}


ਸਥਿਰ ਪੂਰਨ check_access(ਬਣਤਰ mosquitto__listener *listener, ਬਣਤਰ MHD_Connection *connection, ਅਚਲ ਅੱਖਰ *url)
{
	ਬਣਤਰ mosquitto context = {0};
	ਪੂਰਨ auth_rc, acl_rc = MOSQ_ERR_SUCCESS;

	context.listener = listener;

	context.id = (ਅੱਖਰ *)"http-api";
	context.username = MHD_basic_auth_get_username_password(connection, &context.password);

	/* Authentication */
	auth_rc = mosquitto_basic_auth(&context);
	ਜੇ(auth_rc == MOSQ_ERR_SUCCESS){
		acl_rc = mosquitto_acl_check(&context, url, 0, ਨਿਰਾ, 0, false, ਨਿਰਾ, MOSQ_ACL_READ);
	}
	MHD_free(context.username);
	MHD_free(context.password);

	ਜੇ(auth_rc || acl_rc){
		ਅਚਲ ਅੱਖਰ *buf = "Not authorised\n";
		ਬਣਤਰ MHD_Response *response = MHD_create_response_from_ਬਫਰ(ਲੜੀ_ਲੰਬਾਈ(buf), (ਖਾਲੀ *)buf, MHD_RESPMEM_MUST_COPY);
		MHD_ਕਤਾਰ_basic_auth_fail_response(connection, "Mosquitto API", response);
		MHD_destroy_response(response);

		ਵਾਪਸ MOSQ_ERR_AUTH;
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}

ਸਥਿਰ ਗਿਣਤੀ MHD_ਨਤੀਜਾ http_api_handler(ਖਾਲੀ *cls, ਬਣਤਰ MHD_Connection
		*connection, ਅਚਲ ਅੱਖਰ *url, ਅਚਲ ਅੱਖਰ *ਵਿਧੀ, ਅਚਲ ਅੱਖਰ *ਸੰਸਕਰਣ_2,
		ਅਚਲ ਅੱਖਰ *upload_ਅੰਕੜੇ, size_t *upload_ਅੰਕੜੇ_ਮਾਪ, ਖਾਲੀ **con_cls)
{
	ਅਣਵਰਤਿਆ(ਸੰਸਕਰਣ_2);
	ਅਣਵਰਤਿਆ(upload_ਅੰਕੜੇ);
	ਅਣਵਰਤਿਆ(upload_ਅੰਕੜੇ_ਮਾਪ);
	ਅਣਵਰਤਿਆ(con_cls);

	ਬਣਤਰ mosquitto__listener *listener = cls;

	ਜੇ(strcmp(ਵਿਧੀ, "GET") != 0){
		ਅੱਖਰ *buf = "Invalid HTTP Method\n";
		ਬਣਤਰ MHD_Response *response = MHD_create_response_from_ਬਫਰ(ਲੜੀ_ਲੰਬਾਈ(buf), (ਖਾਲੀ *)buf, MHD_RESPMEM_MUST_COPY);
		ਗਿਣਤੀ MHD_ਨਤੀਜਾ ret = MHD_ਕਤਾਰ_response(connection, MHD_HTTP_ਵਿਧੀ_NOT_ALLOWED, response);
		MHD_destroy_response(response);
		ਵਾਪਸ ret;
	}

	ਜੇ(check_access(listener, connection, url) != MOSQ_ERR_SUCCESS){
		ਵਾਪਸ MHD_YES;
	}

	ਜੇ(!strncasecmp(url, "/api/", ਲੜੀ_ਲੰਬਾਈ("/api/"))){
		ਵਾਪਸ http_api__process_api(connection, url);
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ http_api__process_file(listener, connection, url);
	}
}


ਪੂਰਨ http_api_ਸ਼ੁਰੂ_local(ਬਣਤਰ mosquitto__listener *listener)
{
	listener->security_options = mosquitto_calloc(1, ਆਕਾਰ(ਬਣਤਰ mosquitto__security_options));
	ਜੇ(listener->security_options == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	listener->host = mosquitto_strdup("127.0.0.1");
	ਜੇ(!listener->host){
		mosquitto_FREE(listener->security_options);
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}
	listener->ਪੋਰਟ = 9883;

	ਜੇ(db.config->security_options.allow_anonymous == -1){
		listener->security_options->allow_anonymous = true;
	}ਨਹੀਂਤਾਂ{
		listener->security_options->allow_anonymous = db.config->security_options.allow_anonymous;
	}

	ਜੇ(listener->http_dir == ਨਿਰਾ && ਲੜੀ_ਲੰਬਾਈ(HTTP_API_DIR) > 0){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
		ਅੱਖਰ *http_dir_canonical = _fullpath(ਨਿਰਾ, HTTP_API_DIR, 0);
#ਨਹੀਂਤਾਂ
		ਅੱਖਰ *http_dir_canonical = realpath(HTTP_API_DIR, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
		ਜੇ(!http_dir_canonical){
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}
		mosquitto_FREE(listener->http_dir);
		listener->http_dir = mosquitto_strdup(http_dir_canonical);
		mosquitto_FREE(http_dir_canonical);
	}

	ਵਾਪਸ http_api_ਸ਼ੁਰੂ(listener);
}


ਪੂਰਨ http_api_ਸ਼ੁਰੂ(ਬਣਤਰ mosquitto__listener *listener)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags = MHD_USE_AUTO_ਅੰਦਰੂਨੀ_THREAD;
	ਅਚਲ ਅੱਖਰ *bind_ਪਤਾ;
	uint16_t ਪੋਰਟ = 9883;
	ਅੱਖਰ *x509_cert = ਨਿਰਾ;
	ਅੱਖਰ *x509_ਕੁੰਜੀ = ਨਿਰਾ;

	ਜੇ(!listener->security_options){
		listener->security_options = mosquitto_calloc(1, ਆਕਾਰ(ਬਣਤਰ mosquitto__security_options));
		ਜੇ(listener->security_options == ਨਿਰਾ){
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}
	}
	listener->protocol = mp_http_api;

	bind_ਪਤਾ = listener->host;
	ਪੋਰਟ = listener->ਪੋਰਟ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
	ਜੇ(listener->certfile && listener->keyfile){
		ਜੇ(mosquitto_read_file(listener->certfile, false, &x509_cert, ਨਿਰਾ)){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
		ਜੇ(mosquitto_read_file(listener->keyfile, false, &x509_ਕੁੰਜੀ, ਨਿਰਾ)){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			mosquitto_FREE(x509_cert);
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
		flags |= MHD_USE_TLS;
	}
#ਜੇ_ਅੰਤ

	ਜੇ(bind_ਪਤਾ){
		ਅੱਖਰ service[10];
		ਬਣਤਰ addrinfo hints;
		ਬਣਤਰ addrinfo *ainfo, *rp;

		snprintf(service, ਆਕਾਰ(service), "%d", ਪੋਰਟ);

		memset(&hints, 0, ਆਕਾਰ(ਬਣਤਰ addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = AI_PASSIVE;
		hints.ai_socktype = SOCK_STREAM;

		ਪੂਰਨ rc = getaddrinfo(bind_ਪਤਾ, service, &hints, &ainfo);
		ਜੇ(rc){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Unable to start http api listener (%s).", strerror(errno));
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_ਕੁੰਜੀ);
			ਵਾਪਸ MOSQ_ERR_ERRNO;
		}

		ਲਈ(rp = ainfo; rp; rp = rp->ai_ਅਗਲਾ){
			ਜੇ(rp->ai_family == AF_INET || rp->ai_family == AF_INET6){
				ਰੋਕੋ;
			}
		}
		ਜੇ(!rp){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Unable to start http api listener, could not find address.");
			freeaddrinfo(ainfo);
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_ਕੁੰਜੀ);

			ਵਾਪਸ MOSQ_ERR_INVAL;
		}

		ਜੇ(rp->ai_family == AF_INET6){
			flags |= MHD_USE_IPv6;
		}
		ਜੇ(x509_cert && x509_ਕੁੰਜੀ){
			listener->mhd = MHD_ਸ਼ੁਰੂ_daemon(flags, ਪੋਰਟ, ਨਿਰਾ, ਨਿਰਾ, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_ਕੁੰਜੀ, x509_ਕੁੰਜੀ,
					MHD_OPTION_ਸਿਰਾ);
		}ਨਹੀਂਤਾਂ{
			listener->mhd = MHD_ਸ਼ੁਰੂ_daemon(flags, ਪੋਰਟ, ਨਿਰਾ, ਨਿਰਾ, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_ਸਿਰਾ);
		}
		freeaddrinfo(ainfo);
	}ਨਹੀਂਤਾਂ{
		ਜੇ(x509_cert && x509_ਕੁੰਜੀ){
			listener->mhd = MHD_ਸ਼ੁਰੂ_daemon(flags | MHD_USE_DUAL_ਢੇਰ, ਪੋਰਟ, ਨਿਰਾ, ਨਿਰਾ, &http_api_handler, listener,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_ਕੁੰਜੀ, x509_ਕੁੰਜੀ,
					MHD_OPTION_ਸਿਰਾ);
		}ਨਹੀਂਤਾਂ{
			listener->mhd = MHD_ਸ਼ੁਰੂ_daemon(flags | MHD_USE_DUAL_ਢੇਰ, ਪੋਰਟ, ਨਿਰਾ, ਨਿਰਾ, &http_api_handler, listener,
					MHD_OPTION_ਸਿਰਾ);
		}
	}

	mosquitto_FREE(x509_cert);
	mosquitto_FREE(x509_ਕੁੰਜੀ);

	ਜੇ(listener->mhd){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Opening http api listen socket on port %d.", ਪੋਰਟ);
		ਜੇ(listener->http_dir){
			log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Using http_dir %s", listener->http_dir);
		}
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ MOSQ_ERR_UNKNOWN;
	}
}


ਖਾਲੀ http_api__stop(ਬਣਤਰ mosquitto__listener *listener)
{
	MHD_stop_daemon(listener->mhd);
	listener->mhd = ਨਿਰਾ;
}

#ਜੇ_ਅੰਤ
