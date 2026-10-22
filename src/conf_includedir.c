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
#否则
#  include <dirent.h>
#endif

#ifndef WIN32
#  include <strings.h>
#  include <netdb.h>
#  include <sys/socket.h>
#否则
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#如果 defined(__HAIKU__)
#  include <syslog.h>
#elif !defined(WIN32) && !defined(__CYGWIN__) && !defined(__QNX__)
#  include <sys/syslog.h>
#endif

#include "mosquitto_broker_internal.h"
#include "tls_mosq.h"
#include "util_mosq.h"
#include "mosquitto/mqtt_protocol.h"


static 整数 scmp_p(const void *p1, const void *p2)
{
	const char *s1 = *(const char **)p1;
	const char *s2 = *(const char **)p2;
	整数 result;

	当(s1[0] && s2[0]){
		/* Sort by case insensitive part first */
		result = toupper((unsigned char)s1[0]) - toupper((unsigned char)s2[0]);
		如果(result == 0){
			/* Case insensitive part matched, now distinguish between case */
			result = s1[0] - s2[0];
			如果(result != 0){
				返回 result;
			}
		}否则{
			/* Return case insensitive match fail */
			返回 result;
		}
		s1++;
		s2++;
	}

	返回 s1[0] - s2[0];
}

#ifdef WIN32


整数 config__get_dir_files(const char *include_dir, char ***files, 整数 *file_count)
{
	size_t len;
	char **l_files = NULL;
	整数 l_file_count = 0;
	char **files_tmp;

	HANDLE fh;
	char dirpath[MAX_PATH];
	WIN32_FIND_DATA find_data;

	snprintf(dirpath, MAX_PATH, "%s\\*.conf", include_dir);
	fh = FindFirstFile(dirpath, &find_data);
	如果(fh == INVALID_HANDLE_VALUE){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		返回 1;
	}

	do{
		len = strlen(include_dir)+1+strlen(find_data.cFileName)+1;

		l_file_count++;
		files_tmp = mosquitto_realloc(l_files, l_file_count*sizeof(char *));
		如果(!files_tmp){
			为(整数 i=0; i<l_file_count-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			返回 MOSQ_ERR_NOMEM;
		}
		l_files = files_tmp;

		l_files[l_file_count-1] = mosquitto_malloc(len+1);
		如果(!l_files[l_file_count-1]){
			为(整数 i=0; i<l_file_count-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			返回 MOSQ_ERR_NOMEM;
		}
		snprintf(l_files[l_file_count-1], len, "%s/%s", include_dir, find_data.cFileName);
		l_files[l_file_count-1][len] = '\0';
	}当(FindNextFile(fh, &find_data));

	FindClose(fh);

	如果(l_files){
		qsort(l_files, l_file_count, sizeof(char *), scmp_p);
	}
	*files = l_files;
	*file_count = l_file_count;

	返回 0;
}
#endif


#ifndef WIN32


整数 config__get_dir_files(const char *include_dir, char ***files, 整数 *file_count)
{
	char **l_files = NULL;
	整数 l_file_count = 0;
	char **files_tmp;
	size_t len;

	DIR *dh;
	struct dirent *de;

	dh = opendir(include_dir);
	如果(!dh){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		返回 1;
	}
	当((de = readdir(dh)) != NULL){
		如果(strlen(de->d_name) > 5){
			如果(!strcmp(&de->d_name[strlen(de->d_name)-5], ".conf")){
				len = strlen(include_dir)+1+strlen(de->d_name)+1;

				l_file_count++;
				files_tmp = mosquitto_realloc(l_files, (size_t)l_file_count*sizeof(char *));
				如果(!files_tmp){
					goto error;
				}
				l_files = files_tmp;

				l_files[l_file_count-1] = mosquitto_malloc(len+1);
				如果(!l_files[l_file_count-1]){
					goto error;
				}
				snprintf(l_files[l_file_count-1], len, "%s/%s", include_dir, de->d_name);
				l_files[l_file_count-1][len] = '\0';
			}
		}
	}
	closedir(dh);

	如果(l_files){
		qsort(l_files, (size_t)l_file_count, sizeof(char *), scmp_p);
	}
	*files = l_files;
	*file_count = l_file_count;

	返回 0;
error:
	为(整数 i=0; i<l_file_count-1; i++){
		mosquitto_FREE(l_files[i]);
	}
	mosquitto_FREE(l_files);
	closedir(dh);
	返回 MOSQ_ERR_NOMEM;
}
#endif


