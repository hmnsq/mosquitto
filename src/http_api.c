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

#ಅನ್ತರ್ಭಾವ "config.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_HTTP_API

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ <limits.h>
#ಅನ್ತರ್ಭಾವ <microhttpd.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
#  include <netdb.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "json_help.h"
#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "sys_tree.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HTTP_API_DIR
#  define HTTP_API_DIR ""
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ metric {
	int64_t current;
	int64_t ಮುಂದಿನ;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic, *topic_alias;
	bool is_max;
};

time_t broker_uptime(ಶೂನ್ಯ);
ರಚನೆ MHD_Daemon *mhd = ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_SYS_ವೃಕ್ಷ
ಬಾಹ್ಯ ರಚನೆ metric metrics[mosq_metric_max];
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು HTTP_RESPONSE_BUFLEN 10000

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
#ವ್ಯಾಖ್ಯಾನಿಸು DIR_SEP '\\'
#ವ್ಯಾಖ್ಯಾನಿಸು PATH_MAX MAX_ಮಾರ್ಗ
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು DIR_SEP '/'
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ S_ISREG
#ವ್ಯಾಖ್ಯಾನಿಸು S_ISREG(ಬಹುಲಕ) (((ಬಹುಲಕ)&S_IFMT) == S_IFREG)
#ಆದರೆ_ಅಂತ್ಯ


ಸ್ಥಿರ ಅಕ್ಷರ *http__canonical_filename(
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url,
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *http_dir,
		ಪೂರ್ಣಾಂಕ *error_ಸಂಕೇತ)
{
	size_t urllen, slen;
	ಅಕ್ಷರ *filename, *filename_canonical;

	urllen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(url);
	ಆದರೆ(url[urllen-1] == '/'){
		slen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(http_dir) + urllen + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("/index.html") + 2;
	}ಇಲ್ಲದಿದ್ದರೆ{
		slen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(http_dir) + urllen + 2;
	}
	filename = mosquitto_malloc(slen);
	ಆದರೆ(!filename){
		*error_ಸಂಕೇತ = MHD_HTTP_ಆಂತರಿಕ_SERVER_ERROR;
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	ಆದರೆ(((ಅಕ್ಷರ *)url)[urllen-1] == '/'){
		snprintf(filename, slen, "%s%c%sindex.html", http_dir, DIR_SEP, url);
	}ಇಲ್ಲದಿದ್ದರೆ{
		snprintf(filename, slen, "%s%c%s", http_dir, DIR_SEP, url);
	}


	/* Get canonical path and check it is within our http_dir */
	filename_canonical = mosquitto_calloc(1, PATH_MAX);
	ಆದರೆ(!filename_canonical){
		*error_ಸಂಕೇತ = MHD_HTTP_ಆಂತರಿಕ_SERVER_ERROR;
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
	ಅಕ್ಷರ *resolved = _fullpath(filename_canonical, filename, PATH_MAX);
	mosquitto_FREE(filename);
	ಆದರೆ(!resolved){
		*error_ಸಂಕೇತ = MHD_HTTP_ಆಂತರಿಕ_SERVER_ERROR;
		mosquitto_FREE(filename_canonical);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
#ಇಲ್ಲದಿದ್ದರೆ
	ಅಕ್ಷರ *resolved = realpath(filename, filename_canonical);
	mosquitto_FREE(filename);
	ಆದರೆ(!resolved){
		ಆದರೆ(errno == EACCES){
			*error_ಸಂಕೇತ = MHD_HTTP_FORBIDDEN;
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(errno == EINVAL || errno == EIO || errno == ELOOP){
			*error_ಸಂಕೇತ = MHD_HTTP_ಆಂತರಿಕ_SERVER_ERROR;
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(errno == ENAMETOOLONG){
			*error_ಸಂಕೇತ = MHD_HTTP_URI_TOO_LONG;
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(errno == ENOENT || errno == ENOTDIR){
			*error_ಸಂಕೇತ = MHD_HTTP_NOT_ಸಿಕ್ಕಿತು;
		}
		mosquitto_FREE(filename_canonical);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(http_dir, filename_canonical, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(http_dir))){
		/* Requested file isn't within http_dir, deny access because it's not found. */
		mosquitto_FREE(filename_canonical);
		*error_ಸಂಕೇತ = MHD_HTTP_NOT_ಸಿಕ್ಕಿತು;
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	ಹಿಂತಿರುಗಿ filename_canonical;
}

ಸ್ಥಿರ ಎಣಿಕೆ MHD_ಫಲಿತಾಂಶ http_api__send_error_response(ರಚನೆ MHD_Connection *connection, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *error_str, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ error_ಸಂಕೇತ)
{
	ರಚನೆ MHD_Response *response = MHD_create_response_from_ಬಫರ್(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(error_str), (ಶೂನ್ಯ *)error_str, MHD_RESPMEM_MUST_COPY);
	ಎಣಿಕೆ MHD_ಫಲಿತಾಂಶ ret = MHD_ಸರತಿ_response(connection, error_ಸಂಕೇತ, response);
	MHD_destroy_response(response);
	ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ಎಣಿಕೆ MHD_ಫಲಿತಾಂಶ http_api_send_response_with_ಶೀರ್ಷಿಕೆಗಳು(ರಚನೆ MHD_Connection *connection, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf)
{
	ಎಣಿಕೆ MHD_ಫಲಿತಾಂಶ ret;
	ರಚನೆ MHD_Response *response = MHD_create_response_from_ಬಫರ್(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(buf), (ಶೂನ್ಯ *)buf, MHD_RESPMEM_MUST_COPY);

	ret = MHD_ಕೂಡಿಸು_response_ಶೀರ್ಷಿಕೆ(response, "Access-Control-Allow-Origin", "*");
	ಆದರೆ(ret != MHD_YES){
		MHD_destroy_response(response);
		ಹಿಂತಿರುಗಿ ret;
	}
	ret = MHD_ಸರತಿ_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ಎಣಿಕೆ MHD_ಫಲಿತಾಂಶ http_api_process_ಆವೃತ್ತಿ(ರಚನೆ MHD_Connection *connection)
{
	ಹಿಂತಿರುಗಿ http_api_send_response_with_ಶೀರ್ಷಿಕೆಗಳು(connection, ಆವೃತ್ತಿ);
}

ಸ್ಥಿರ ಎಣಿಕೆ MHD_ಫಲಿತಾಂಶ http_api__process_listeners(ರಚನೆ MHD_Connection *connection)
{
	ಅಕ್ಷರ *buf;
	ಎಣಿಕೆ MHD_ಫಲಿತಾಂಶ ret;

	cJSON *j_ವೃಕ್ಷ = cJSON_CreateObject();
	ಆದರೆ(!j_ವೃಕ್ಷ){
		ಹಿಂತಿರುಗಿ http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_ಆಂತರಿಕ_SERVER_ERROR);
	}

	cJSON *j_listeners = c_JSON_ಕೂಡಿಸು_Array_ಗಮ್ಯ_Object(j_ವೃಕ್ಷ, "listeners");
	ಆದರೆ(!j_listeners){
		cJSON_Delete(j_ವೃಕ್ಷ);
		ಹಿಂತಿರುಗಿ http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_ಆಂತರಿಕ_SERVER_ERROR);
	}

	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
		cJSON *j_listener = cJSON_CreateObject();
		ಆದರೆ(!j_listener){
			cJSON_Delete(j_ವೃಕ್ಷ);
			ಹಿಂತಿರುಗಿ http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_ಆಂತರಿಕ_SERVER_ERROR);
		}
		c_JSON_ಕೂಡಿಸು_ಅಂಶ_ಗಮ್ಯ_Array(j_listeners, j_listener);

		ರಚನೆ mosquitto__listener *listener = &db.config->listeners[i];
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_UNIX_SOCKETS
		ಆದರೆ(listener->unix_socket_ಮಾರ್ಗ){
			c_JSON_ಕೂಡಿಸು_String_ಗಮ್ಯ_Object(j_listener, "path", listener->unix_socket_ಮಾರ್ಗ);
		}ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		{
			c_JSON_ಕೂಡಿಸು_Int_ಗಮ್ಯ_Object(j_listener, "port", listener->ಪೋರ್ಟ್);
		}

		ಆಯ್ಕೆ(listener->protocol){
			ಸ್ಥಿತಿ mp_mqtt:
				c_JSON_ಕೂಡಿಸು_String_ಗಮ್ಯ_Object(j_listener, "protocol", "mqtt");
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ mp_mqttsn:
				c_JSON_ಕೂಡಿಸು_String_ಗಮ್ಯ_Object(j_listener, "protocol", "mqtt-sn");
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ mp_websockets:
				c_JSON_ಕೂಡಿಸು_String_ಗಮ್ಯ_Object(j_listener, "protocol", "websockets");
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ mp_http_api:
				c_JSON_ಕೂಡಿಸು_String_ಗಮ್ಯ_Object(j_listener, "protocol", "httpapi");
				ನಿಲ್ಲಿಸು;
		}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
		c_JSON_ಕೂಡಿಸು_Bool_ಗಮ್ಯ_Object(j_listener, "tls", listener->certfile && listener->keyfile);
		c_JSON_ಕೂಡಿಸು_Bool_ಗಮ್ಯ_Object(j_listener, "mtls", listener->require_certificate);
#ಇಲ್ಲದಿದ್ದರೆ
		c_JSON_ಕೂಡಿಸು_Bool_ಗಮ್ಯ_Object(j_listener, "tls", false);
		c_JSON_ಕೂಡಿಸು_Bool_ಗಮ್ಯ_Object(j_listener, "mtls", false);
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ(listener->security_options->allow_anonymous == -1){
			c_JSON_ಕೂಡಿಸು_Bool_ಗಮ್ಯ_Object(j_listener, "allow_anonymous", db.config->security_options.allow_anonymous);
		}ಇಲ್ಲದಿದ್ದರೆ{
			c_JSON_ಕೂಡಿಸು_Bool_ಗಮ್ಯ_Object(j_listener, "allow_anonymous", listener->security_options->allow_anonymous);
		}
	}
	buf = cJSON_Print(j_ವೃಕ್ಷ);
	cJSON_Delete(j_ವೃಕ್ಷ);
	ಆದರೆ(buf){
		ret = http_api_send_response_with_ಶೀರ್ಷಿಕೆಗಳು(connection, buf);
		ಬಿಡುಗಡೆ(buf);
	}ಇಲ್ಲದಿದ್ದರೆ{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_ಆಂತರಿಕ_SERVER_ERROR);
	}

	ಹಿಂತಿರುಗಿ ret;
}


ಸ್ಥಿರ ಎಣಿಕೆ MHD_ಫಲಿತಾಂಶ http_api__process_systree(ರಚನೆ MHD_Connection *connection)
{
	ಎಣಿಕೆ MHD_ಫಲಿತಾಂಶ ret;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_SYS_ವೃಕ್ಷ
	ಅಕ್ಷರ *buf;

	cJSON *j_ವೃಕ್ಷ = cJSON_CreateObject();
	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<mosq_metric_max; i++){
		ಆದರೆ(metrics[i].topic){
			c_JSON_ಕೂಡಿಸು_Int_ಗಮ್ಯ_Object(j_ವೃಕ್ಷ, metrics[i].topic, metrics[i].current);
		}
	}
	c_JSON_ಕೂಡಿಸು_Int_ಗಮ್ಯ_Object(j_ವೃಕ್ಷ, "$SYS/broker/uptime", broker_uptime());

	buf = cJSON_Print(j_ವೃಕ್ಷ);
	cJSON_Delete(j_ವೃಕ್ಷ);

	ಆದರೆ(buf){
		ret = http_api_send_response_with_ಶೀರ್ಷಿಕೆಗಳು(connection, buf);
		ಬಿಡುಗಡೆ(buf);
	}ಇಲ್ಲದಿದ್ದರೆ{
		ret = http_api__send_error_response(connection, "Internal server error.\n", MHD_HTTP_ಆಂತರಿಕ_SERVER_ERROR);
	}
#ಇಲ್ಲದಿದ್ದರೆ
	ret = http_api__send_error_response(connection, "Not found.\n", 404);
#ಆದರೆ_ಅಂತ್ಯ

	ಹಿಂತಿರುಗಿ ret;
}


ಸ್ಥಿರ ssize_t http_file_read_cb(ಶೂನ್ಯ *cls, uint64_t pos, ಅಕ್ಷರ *buf, size_t max)
{
	FILE *fptr = cls;
	ಆದರೆ(fseek(fptr, (ದೀರ್ಘ )pos, SEEK_SET) < 0){
		ಹಿಂತಿರುಗಿ -1;
	}
	ಹಿಂತಿರುಗಿ (ssize_t )fread(buf, 1, max, fptr);
}


ಸ್ಥಿರ ಶೂನ್ಯ http_file_free_cb(ಶೂನ್ಯ *cls)
{
	FILE *fptr = cls;
	fclose(fptr);
}


ಸ್ಥಿರ ಎಣಿಕೆ MHD_ಫಲಿತಾಂಶ http_api__process_file(ರಚನೆ mosquitto__listener *listener, ರಚನೆ MHD_Connection *connection, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url)
{
	ಪೂರ್ಣಾಂಕ error_ಸಂಕೇತ = MHD_HTTP_ಆಂತರಿಕ_SERVER_ERROR;
	ಅಕ್ಷರ *canonical_filename;

	ಆದರೆ(!listener || !listener->http_dir){
		http_api__send_error_response(connection, "Not found.\n", 404);
		ಹಿಂತಿರುಗಿ MHD_YES;
	}

	canonical_filename = http__canonical_filename(url, listener->http_dir, &error_ಸಂಕೇತ);
	ಆದರೆ(!canonical_filename){
		http_api__send_error_response(connection, "Not found.\n", 404);
		ಹಿಂತಿರುಗಿ MHD_YES;
	}

	FILE *fptr = fopen(canonical_filename, "rb");
	mosquitto_FREE(canonical_filename);
	ಆದರೆ(!fptr){
		http_api__send_error_response(connection, "Not found.\n", 404);
		ಹಿಂತಿರುಗಿ MHD_YES;
	}

	ರಚನೆ stat statbuf;
	ಆದರೆ(fstat(fileno(fptr), &statbuf)){
		fclose(fptr);
		http_api__send_error_response(connection, "Internal server error.\n", 500);
		ಹಿಂತಿರುಗಿ MHD_YES;
	}

	ಆದರೆ(!S_ISREG(statbuf.st_ಬಹುಲಕ)){
		fclose(fptr);
		http_api__send_error_response(connection, "Not found.\n", 404);
		ಹಿಂತಿರುಗಿ MHD_YES;
	}
	uint64_t flen = (uint64_t )statbuf.st_ಗಾತ್ರ;

	/* Using MHD_create_response_from_fd would be easier here, but is less portable */
	ರಚನೆ MHD_Response *response = MHD_create_response_from_callback(
			flen,
			65536,
			http_file_read_cb,
			fptr,
			http_file_free_cb);

	ಎಣಿಕೆ MHD_ಫಲಿತಾಂಶ ret = MHD_ಸರತಿ_response(connection, 200, response);
	MHD_destroy_response(response);
	ಹಿಂತಿರುಗಿ ret;
}


ಸ್ಥಿರ ಎಣಿಕೆ MHD_ಫಲಿತಾಂಶ http_api__process_api(ರಚನೆ MHD_Connection *connection, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url)
{
	ಆದರೆ(strcmp(url, "/api/v1/systree") == 0){
		ಹಿಂತಿರುಗಿ http_api__process_systree(connection);
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strcmp(url, "/api/v1/listeners") == 0){
		ಹಿಂತಿರುಗಿ http_api__process_listeners(connection);
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strcmp(url, "/api/v1/version") == 0){
		ಹಿಂತಿರುಗಿ http_api_process_ಆವೃತ್ತಿ(connection);
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ http_api__send_error_response(connection, "Not found.\n", 404);
	}
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ check_access(ರಚನೆ mosquitto__listener *listener, ರಚನೆ MHD_Connection *connection, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url)
{
	ರಚನೆ mosquitto context = {0};
	ಪೂರ್ಣಾಂಕ auth_rc, acl_rc = MOSQ_ERR_SUCCESS;

	context.listener = listener;

	context.id = (ಅಕ್ಷರ *)"http-api";
	context.username = MHD_basic_auth_get_username_password(connection, &context.password);

	/* Authentication */
	auth_rc = mosquitto_basic_auth(&context);
	ಆದರೆ(auth_rc == MOSQ_ERR_SUCCESS){
		acl_rc = mosquitto_acl_check(&context, url, 0, ಖಾಲಿ, 0, false, ಖಾಲಿ, MOSQ_ACL_READ);
	}
	MHD_free(context.username);
	MHD_free(context.password);

	ಆದರೆ(auth_rc || acl_rc){
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf = "Not authorised\n";
		ರಚನೆ MHD_Response *response = MHD_create_response_from_ಬಫರ್(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(buf), (ಶೂನ್ಯ *)buf, MHD_RESPMEM_MUST_COPY);
		MHD_ಸರತಿ_basic_auth_fail_response(connection, "Mosquitto API", response);
		MHD_destroy_response(response);

		ಹಿಂತಿರುಗಿ MOSQ_ERR_AUTH;
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}

ಸ್ಥಿರ ಎಣಿಕೆ MHD_ಫಲಿತಾಂಶ http_api_handler(ಶೂನ್ಯ *cls, ರಚನೆ MHD_Connection
		*connection, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ವಿಧಾನ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಆವೃತ್ತಿ_2,
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *upload_ದತ್ತಾಂಶ, size_t *upload_ದತ್ತಾಂಶ_ಗಾತ್ರ, ಶೂನ್ಯ **con_cls)
{
	ಬಳಸದ(ಆವೃತ್ತಿ_2);
	ಬಳಸದ(upload_ದತ್ತಾಂಶ);
	ಬಳಸದ(upload_ದತ್ತಾಂಶ_ಗಾತ್ರ);
	ಬಳಸದ(con_cls);

	ರಚನೆ mosquitto__listener *listener = cls;

	ಆದರೆ(strcmp(ವಿಧಾನ, "GET") != 0){
		ಅಕ್ಷರ *buf = "Invalid HTTP Method\n";
		ರಚನೆ MHD_Response *response = MHD_create_response_from_ಬಫರ್(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(buf), (ಶೂನ್ಯ *)buf, MHD_RESPMEM_MUST_COPY);
		ಎಣಿಕೆ MHD_ಫಲಿತಾಂಶ ret = MHD_ಸರತಿ_response(connection, MHD_HTTP_ವಿಧಾನ_NOT_ALLOWED, response);
		MHD_destroy_response(response);
		ಹಿಂತಿರುಗಿ ret;
	}

	ಆದರೆ(check_access(listener, connection, url) != MOSQ_ERR_SUCCESS){
		ಹಿಂತಿರುಗಿ MHD_YES;
	}

	ಆದರೆ(!strncasecmp(url, "/api/", ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("/api/"))){
		ಹಿಂತಿರುಗಿ http_api__process_api(connection, url);
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ http_api__process_file(listener, connection, url);
	}
}


ಪೂರ್ಣಾಂಕ http_api_ಆರಂಭ_local(ರಚನೆ mosquitto__listener *listener)
{
	listener->security_options = mosquitto_calloc(1, ಗಾತ್ರ(ರಚನೆ mosquitto__security_options));
	ಆದರೆ(listener->security_options == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}

	listener->host = mosquitto_strdup("127.0.0.1");
	ಆದರೆ(!listener->host){
		mosquitto_FREE(listener->security_options);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}
	listener->ಪೋರ್ಟ್ = 9883;

	ಆದರೆ(db.config->security_options.allow_anonymous == -1){
		listener->security_options->allow_anonymous = true;
	}ಇಲ್ಲದಿದ್ದರೆ{
		listener->security_options->allow_anonymous = db.config->security_options.allow_anonymous;
	}

	ಆದರೆ(listener->http_dir == ಖಾಲಿ && ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(HTTP_API_DIR) > 0){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
		ಅಕ್ಷರ *http_dir_canonical = _fullpath(ಖಾಲಿ, HTTP_API_DIR, 0);
#ಇಲ್ಲದಿದ್ದರೆ
		ಅಕ್ಷರ *http_dir_canonical = realpath(HTTP_API_DIR, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ(!http_dir_canonical){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}
		mosquitto_FREE(listener->http_dir);
		listener->http_dir = mosquitto_strdup(http_dir_canonical);
		mosquitto_FREE(http_dir_canonical);
	}

	ಹಿಂತಿರುಗಿ http_api_ಆರಂಭ(listener);
}


ಪೂರ್ಣಾಂಕ http_api_ಆರಂಭ(ರಚನೆ mosquitto__listener *listener)
{
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags = MHD_USE_AUTO_ಆಂತರಿಕ_THREAD;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *bind_ವಿಳಾಸ;
	uint16_t ಪೋರ್ಟ್ = 9883;
	ಅಕ್ಷರ *x509_cert = ಖಾಲಿ;
	ಅಕ್ಷರ *x509_ಕೀಲಿ = ಖಾಲಿ;

	ಆದರೆ(!listener->security_options){
		listener->security_options = mosquitto_calloc(1, ಗಾತ್ರ(ರಚನೆ mosquitto__security_options));
		ಆದರೆ(listener->security_options == ಖಾಲಿ){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}
	}
	listener->protocol = mp_http_api;

	bind_ವಿಳಾಸ = listener->host;
	ಪೋರ್ಟ್ = listener->ಪೋರ್ಟ್;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
	ಆದರೆ(listener->certfile && listener->keyfile){
		ಆದರೆ(mosquitto_read_file(listener->certfile, false, &x509_cert, ಖಾಲಿ)){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to load server certificate \"%s\". Check certfile.", listener->certfile);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
		ಆದರೆ(mosquitto_read_file(listener->keyfile, false, &x509_ಕೀಲಿ, ಖಾಲಿ)){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to load server key file \"%s\". Check keyfile.", listener->keyfile);
			mosquitto_FREE(x509_cert);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
		flags |= MHD_USE_TLS;
	}
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ(bind_ವಿಳಾಸ){
		ಅಕ್ಷರ service[10];
		ರಚನೆ addrinfo hints;
		ರಚನೆ addrinfo *ainfo, *rp;

		snprintf(service, ಗಾತ್ರ(service), "%d", ಪೋರ್ಟ್);

		memset(&hints, 0, ಗಾತ್ರ(ರಚನೆ addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = AI_PASSIVE;
		hints.ai_socktype = SOCK_STREAM;

		ಪೂರ್ಣಾಂಕ rc = getaddrinfo(bind_ವಿಳಾಸ, service, &hints, &ainfo);
		ಆದರೆ(rc){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Unable to start http api listener (%s).", strerror(errno));
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_ಕೀಲಿ);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_ERRNO;
		}

		ಗಾಗಿ(rp = ainfo; rp; rp = rp->ai_ಮುಂದಿನ){
			ಆದರೆ(rp->ai_family == AF_INET || rp->ai_family == AF_INET6){
				ನಿಲ್ಲಿಸು;
			}
		}
		ಆದರೆ(!rp){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Unable to start http api listener, could not find address.");
			freeaddrinfo(ainfo);
			mosquitto_FREE(x509_cert);
			mosquitto_FREE(x509_ಕೀಲಿ);

			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}

		ಆದರೆ(rp->ai_family == AF_INET6){
			flags |= MHD_USE_IPv6;
		}
		ಆದರೆ(x509_cert && x509_ಕೀಲಿ){
			listener->mhd = MHD_ಆರಂಭ_daemon(flags, ಪೋರ್ಟ್, ಖಾಲಿ, ಖಾಲಿ, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_ಕೀಲಿ, x509_ಕೀಲಿ,
					MHD_OPTION_ಕೊನೆ);
		}ಇಲ್ಲದಿದ್ದರೆ{
			listener->mhd = MHD_ಆರಂಭ_daemon(flags, ಪೋರ್ಟ್, ಖಾಲಿ, ಖಾಲಿ, &http_api_handler, listener,
					MHD_OPTION_SOCK_ADDR, rp->ai_addr,
					MHD_OPTION_ಕೊನೆ);
		}
		freeaddrinfo(ainfo);
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಆದರೆ(x509_cert && x509_ಕೀಲಿ){
			listener->mhd = MHD_ಆರಂಭ_daemon(flags | MHD_USE_DUAL_ರಾಶಿ, ಪೋರ್ಟ್, ಖಾಲಿ, ಖಾಲಿ, &http_api_handler, listener,
					MHD_OPTION_HTTPS_MEM_CERT, x509_cert,
					MHD_OPTION_HTTPS_MEM_ಕೀಲಿ, x509_ಕೀಲಿ,
					MHD_OPTION_ಕೊನೆ);
		}ಇಲ್ಲದಿದ್ದರೆ{
			listener->mhd = MHD_ಆರಂಭ_daemon(flags | MHD_USE_DUAL_ರಾಶಿ, ಪೋರ್ಟ್, ಖಾಲಿ, ಖಾಲಿ, &http_api_handler, listener,
					MHD_OPTION_ಕೊನೆ);
		}
	}

	mosquitto_FREE(x509_cert);
	mosquitto_FREE(x509_ಕೀಲಿ);

	ಆದರೆ(listener->mhd){
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Opening http api listen socket on port %d.", ಪೋರ್ಟ್);
		ಆದರೆ(listener->http_dir){
			log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Using http_dir %s", listener->http_dir);
		}
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ MOSQ_ERR_UNKNOWN;
	}
}


ಶೂನ್ಯ http_api__stop(ರಚನೆ mosquitto__listener *listener)
{
	MHD_stop_daemon(listener->mhd);
	listener->mhd = ಖಾಲಿ;
}

#ಆದರೆ_ಅಂತ್ಯ
