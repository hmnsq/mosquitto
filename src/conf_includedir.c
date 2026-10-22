/*
Copyright (c) 2009-2021 Roger Light <roger@atchoo.org>

All rights reserved. This program and the accompanying materials
are made available under the terms of the Eclipse Public License 2.0
and Eclipse Distribution License v1.0 which accompany this distribution.

The Eclipse Public License is available at
   https://www.eclipse.org/legal/epl-2.0/
and the Eclipse Distribution License is available at
  http://www.eclipse.org/org/documents/edl-v10.php.

SPDX-License-Identifier: EPL-2.0 OR BSD-3-Clause

Contributors:
   Roger Light - initial implementation and documentation.
*/

#include "config.h"

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef WIN32
#아니면
#  include <dirent.h>
#endif

#ifndef WIN32
#  include <strings.h>
#  include <netdb.h>
#  include <sys/socket.h>
#아니면
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#만약 defined(__HAIKU__)
#  include <syslog.h>
#elif !defined(WIN32) && !defined(__CYGWIN__) && !defined(__QNX__)
#  include <sys/syslog.h>
#endif

#include "mosquitto_broker_internal.h"
#include "tls_mosq.h"
#include "util_mosq.h"
#include "mosquitto/mqtt_protocol.h"


static 정수 scmp_p(const void *p1, const void *p2)
{
	const char *s1 = *(const char **)p1;
	const char *s2 = *(const char **)p2;
	정수 result;

	동안(s1[0] && s2[0]){
		/* Sort by case insensitive part first */
		result = toupper((unsigned char)s1[0]) - toupper((unsigned char)s2[0]);
		만약(result == 0){
			/* Case insensitive part matched, now distinguish between case */
			result = s1[0] - s2[0];
			만약(result != 0){
				반환 result;
			}
		}아니면{
			/* Return case insensitive match fail */
			반환 result;
		}
		s1++;
		s2++;
	}

	반환 s1[0] - s2[0];
}

#ifdef WIN32


정수 config__get_dir_files(const char *include_dir, char ***files, 정수 *file_count)
{
	size_t len;
	char **l_files = NULL;
	정수 l_file_count = 0;
	char **files_tmp;

	HANDLE fh;
	char dirpath[MAX_PATH];
	WIN32_FIND_DATA find_data;

	snprintf(dirpath, MAX_PATH, "%s\\*.conf", include_dir);
	fh = FindFirstFile(dirpath, &find_data);
	만약(fh == INVALID_HANDLE_VALUE){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		반환 1;
	}

	do{
		len = strlen(include_dir)+1+strlen(find_data.cFileName)+1;

		l_file_count++;
		files_tmp = mosquitto_realloc(l_files, l_file_count*sizeof(char *));
		만약(!files_tmp){
			위해(정수 i=0; i<l_file_count-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			반환 MOSQ_ERR_NOMEM;
		}
		l_files = files_tmp;

		l_files[l_file_count-1] = mosquitto_malloc(len+1);
		만약(!l_files[l_file_count-1]){
			위해(정수 i=0; i<l_file_count-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			반환 MOSQ_ERR_NOMEM;
		}
		snprintf(l_files[l_file_count-1], len, "%s/%s", include_dir, find_data.cFileName);
		l_files[l_file_count-1][len] = '\0';
	}동안(FindNextFile(fh, &find_data));

	FindClose(fh);

	만약(l_files){
		qsort(l_files, l_file_count, sizeof(char *), scmp_p);
	}
	*files = l_files;
	*file_count = l_file_count;

	반환 0;
}
#endif


#ifndef WIN32


정수 config__get_dir_files(const char *include_dir, char ***files, 정수 *file_count)
{
	char **l_files = NULL;
	정수 l_file_count = 0;
	char **files_tmp;
	size_t len;

	DIR *dh;
	struct dirent *de;

	dh = opendir(include_dir);
	만약(!dh){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		반환 1;
	}
	동안((de = readdir(dh)) != NULL){
		만약(strlen(de->d_name) > 5){
			만약(!strcmp(&de->d_name[strlen(de->d_name)-5], ".conf")){
				len = strlen(include_dir)+1+strlen(de->d_name)+1;

				l_file_count++;
				files_tmp = mosquitto_realloc(l_files, (size_t)l_file_count*sizeof(char *));
				만약(!files_tmp){
					goto error;
				}
				l_files = files_tmp;

				l_files[l_file_count-1] = mosquitto_malloc(len+1);
				만약(!l_files[l_file_count-1]){
					goto error;
				}
				snprintf(l_files[l_file_count-1], len, "%s/%s", include_dir, de->d_name);
				l_files[l_file_count-1][len] = '\0';
			}
		}
	}
	closedir(dh);

	만약(l_files){
		qsort(l_files, (size_t)l_file_count, sizeof(char *), scmp_p);
	}
	*files = l_files;
	*file_count = l_file_count;

	반환 0;
error:
	위해(정수 i=0; i<l_file_count-1; i++){
		mosquitto_FREE(l_files[i]);
	}
	mosquitto_FREE(l_files);
	closedir(dh);
	반환 MOSQ_ERR_NOMEM;
}
#endif


