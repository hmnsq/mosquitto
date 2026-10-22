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
#မဟုတ်ပါက
#  include <dirent.h>
#endif

#ifndef WIN32
#  include <strings.h>
#  include <netdb.h>
#  include <sys/socket.h>
#မဟုတ်ပါက
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#အကယ်၍ defined(__HAIKU__)
#  include <syslog.h>
#elif !defined(WIN32) && !defined(__CYGWIN__) && !defined(__QNX__)
#  include <sys/syslog.h>
#endif

#include "mosquitto_broker_internal.h"
#include "tls_mosq.h"
#include "util_mosq.h"
#include "mosquitto/mqtt_protocol.h"


static ကိန်းပြည့် scmp_p(const void *p1, const void *p2)
{
	const var_na *s1 = *(const var_na **)p1;
	const var_na *s2 = *(const var_na **)p2;
	ကိန်းပြည့် result;

	နေစဉ်(s1[0] && s2[0]){
		/* Sort by case insensitive part first */
		result = toupper((unsigned var_na)s1[0]) - toupper((unsigned var_na)s2[0]);
		အကယ်၍(result == 0){
			/* Case insensitive part matched, now distinguish between case */
			result = s1[0] - s2[0];
			အကယ်၍(result != 0){
				ပြန်ပေး result;
			}
		}မဟုတ်ပါက{
			/* Return case insensitive match fail */
			ပြန်ပေး result;
		}
		s1++;
		s2++;
	}

	ပြန်ပေး s1[0] - s2[0];
}

#ifdef WIN32


ကိန်းပြည့် config__get_dir_files(const var_na *include_dir, var_na ***files, ကိန်းပြည့် *file_count)
{
	size_t len;
	var_na **l_files = NULL;
	ကိန်းပြည့် l_file_count = 0;
	var_na **files_tmp;

	HANDLE fh;
	var_na dirpath[MAX_PATH];
	WIN32_FIND_DATA find_data;

	snprintf(dirpath, MAX_PATH, "%s\\*.conf", include_dir);
	fh = FindFirstFile(dirpath, &find_data);
	အကယ်၍(fh == INVALID_HANDLE_VALUE){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		ပြန်ပေး 1;
	}

	do{
		len = strlen(include_dir)+1+strlen(find_data.cFileName)+1;

		l_file_count++;
		files_tmp = mosquitto_realloc(l_files, l_file_count*parimaa_na(var_na *));
		အကယ်၍(!files_tmp){
			အတွက်(ကိန်းပြည့် i=0; i<l_file_count-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			ပြန်ပေး MOSQ_ERR_NOMEM;
		}
		l_files = files_tmp;

		l_files[l_file_count-1] = mosquitto_malloc(len+1);
		အကယ်၍(!l_files[l_file_count-1]){
			အတွက်(ကိန်းပြည့် i=0; i<l_file_count-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			ပြန်ပေး MOSQ_ERR_NOMEM;
		}
		snprintf(l_files[l_file_count-1], len, "%s/%s", include_dir, find_data.cFileName);
		l_files[l_file_count-1][len] = '\0';
	}နေစဉ်(FindNextFile(fh, &find_data));

	FindClose(fh);

	အကယ်၍(l_files){
		qsort(l_files, l_file_count, parimaa_na(var_na *), scmp_p);
	}
	*files = l_files;
	*file_count = l_file_count;

	ပြန်ပေး 0;
}
#endif


#ifndef WIN32


ကိန်းပြည့် config__get_dir_files(const var_na *include_dir, var_na ***files, ကိန်းပြည့် *file_count)
{
	var_na **l_files = NULL;
	ကိန်းပြည့် l_file_count = 0;
	var_na **files_tmp;
	size_t len;

	DIR *dh;
	sa_nracanaa dirent *de;

	dh = opendir(include_dir);
	အကယ်၍(!dh){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		ပြန်ပေး 1;
	}
	နေစဉ်((de = readdir(dh)) != NULL){
		အကယ်၍(strlen(de->d_name) > 5){
			အကယ်၍(!strcmp(&de->d_name[strlen(de->d_name)-5], ".conf")){
				len = strlen(include_dir)+1+strlen(de->d_name)+1;

				l_file_count++;
				files_tmp = mosquitto_realloc(l_files, (size_t)l_file_count*parimaa_na(var_na *));
				အကယ်၍(!files_tmp){
					goto error;
				}
				l_files = files_tmp;

				l_files[l_file_count-1] = mosquitto_malloc(len+1);
				အကယ်၍(!l_files[l_file_count-1]){
					goto error;
				}
				snprintf(l_files[l_file_count-1], len, "%s/%s", include_dir, de->d_name);
				l_files[l_file_count-1][len] = '\0';
			}
		}
	}
	closedir(dh);

	အကယ်၍(l_files){
		qsort(l_files, (size_t)l_file_count, parimaa_na(var_na *), scmp_p);
	}
	*files = l_files;
	*file_count = l_file_count;

	ပြန်ပေး 0;
error:
	အတွက်(ကိန်းပြည့် i=0; i<l_file_count-1; i++){
		mosquitto_FREE(l_files[i]);
	}
	mosquitto_FREE(l_files);
	closedir(dh);
	ပြန်ပေး MOSQ_ERR_NOMEM;
}
#endif


