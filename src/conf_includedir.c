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

#شامل "config.h"

#شامل <ctype.h>
#شامل <limits.h>
#شامل <stdio.h>
#شامل <stdlib.h>
#شامل <string.h>
#شامل <errno.h>

#ifdef WIN32
#وگرنه
#  include <dirent.h>
#endif

#ifndef WIN32
#  include <strings.h>
#  include <netdb.h>
#  include <sys/socket.h>
#وگرنه
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#اگر defined(__HAIKU__)
#  include <syslog.h>
#وگرنهاگر !defined(WIN32) && !defined(__CYGWIN__) && !defined(__QNX__)
#  include <sys/syslog.h>
#endif

#شامل "mosquitto_broker_internal.h"
#شامل "tls_mosq.h"
#شامل "util_mosq.h"
#شامل "mosquitto/mqtt_protocol.h"


ایستا صحیح scmp_p(ثابت تهی *p1, ثابت تهی *p2)
{
	ثابت نویسه *s1 = *(ثابت نویسه **)p1;
	ثابت نویسه *s2 = *(ثابت نویسه **)p2;
	صحیح result;

	وقتی(s1[0] && s2[0]){
		/* Sort by case insensitive part first */
		result = toupper((unsigned نویسه)s1[0]) - toupper((unsigned نویسه)s2[0]);
		اگر(result == 0){
			/* Case insensitive part matched, now distinguish between case */
			result = s1[0] - s2[0];
			اگر(result != 0){
				بازگشت result;
			}
		}وگرنه{
			/* Return case insensitive match fail */
			بازگشت result;
		}
		s1++;
		s2++;
	}

	بازگشت s1[0] - s2[0];
}

#ifdef WIN32


صحیح config__get_dir_files(ثابت نویسه *include_dir, نویسه ***files, صحیح *file_شمار)
{
	size_t len;
	نویسه **l_files = پوچ;
	صحیح l_file_شمار = 0;
	نویسه **files_tmp;

	HANDLE fh;
	نویسه dirpath[MAX_PATH];
	WIN32_FIND_DATA find_data;

	snprintf(dirpath, MAX_PATH, "%s\\*.conf", include_dir);
	fh = FindFirstFile(dirpath, &find_data);
	اگر(fh == INVALID_HANDLE_مقدار){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		بازگشت 1;
	}

	انجامبده{
		len = strlen(include_dir)+1+strlen(find_data.cFileName)+1;

		l_file_شمار++;
		files_tmp = mosquitto_realloc(l_files, l_file_شمار*اندازه(نویسه *));
		اگر(!files_tmp){
			برای(صحیح i=0; i<l_file_شمار-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			بازگشت MOSQ_ERR_NOMEM;
		}
		l_files = files_tmp;

		l_files[l_file_شمار-1] = mosquitto_malloc(len+1);
		اگر(!l_files[l_file_شمار-1]){
			برای(صحیح i=0; i<l_file_شمار-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			بازگشت MOSQ_ERR_NOMEM;
		}
		snprintf(l_files[l_file_شمار-1], len, "%s/%s", include_dir, find_data.cFileName);
		l_files[l_file_شمار-1][len] = '\0';
	}وقتی(FindNextFile(fh, &find_data));

	FindClose(fh);

	اگر(l_files){
		qsort(l_files, l_file_شمار, اندازه(نویسه *), scmp_p);
	}
	*files = l_files;
	*file_شمار = l_file_شمار;

	بازگشت 0;
}
#endif


#ifndef WIN32


صحیح config__get_dir_files(ثابت نویسه *include_dir, نویسه ***files, صحیح *file_شمار)
{
	نویسه **l_files = پوچ;
	صحیح l_file_شمار = 0;
	نویسه **files_tmp;
	size_t len;

	DIR *dh;
	ساختار dirent *de;

	dh = opendir(include_dir);
	اگر(!dh){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		بازگشت 1;
	}
	وقتی((de = readdir(dh)) != پوچ){
		اگر(strlen(de->d_name) > 5){
			اگر(!strcmp(&de->d_name[strlen(de->d_name)-5], ".conf")){
				len = strlen(include_dir)+1+strlen(de->d_name)+1;

				l_file_شمار++;
				files_tmp = mosquitto_realloc(l_files, (size_t)l_file_شمار*اندازه(نویسه *));
				اگر(!files_tmp){
					برو error;
				}
				l_files = files_tmp;

				l_files[l_file_شمار-1] = mosquitto_malloc(len+1);
				اگر(!l_files[l_file_شمار-1]){
					برو error;
				}
				snprintf(l_files[l_file_شمار-1], len, "%s/%s", include_dir, de->d_name);
				l_files[l_file_شمار-1][len] = '\0';
			}
		}
	}
	closedir(dh);

	اگر(l_files){
		qsort(l_files, (size_t)l_file_شمار, اندازه(نویسه *), scmp_p);
	}
	*files = l_files;
	*file_شمار = l_file_شمار;

	بازگشت 0;
error:
	برای(صحیح i=0; i<l_file_شمار-1; i++){
		mosquitto_FREE(l_files[i]);
	}
	mosquitto_FREE(l_files);
	closedir(dh);
	بازگشت MOSQ_ERR_NOMEM;
}
#endif


