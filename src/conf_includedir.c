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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব <ctype.h>
#অন্তর্ভাব <limits.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <errno.h>

#ifdef WIN32
#অন্যথা
#  include <dirent.h>
#endif

#ifndef WIN32
#  include <strings.h>
#  include <netdb.h>
#  include <sys/socket.h>
#অন্যথা
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#যদি defined(__HAIKU__)
#  include <syslog.h>
#নতুবা_যদি !defined(WIN32) && !defined(__CYGWIN__) && !defined(__QNX__)
#  include <sys/syslog.h>
#endif

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "tls_mosq.h"
#অন্তর্ভাব "util_mosq.h"
#অন্তর্ভাব "mosquitto/mqtt_protocol.h"


স্থির পূর্ণসংখ্যা scmp_p(ধ্রুবক শূন্য *p1, ধ্রুবক শূন্য *p2)
{
	ধ্রুবক অক্ষর *s1 = *(ধ্রুবক অক্ষর **)p1;
	ধ্রুবক অক্ষর *s2 = *(ধ্রুবক অক্ষর **)p2;
	পূর্ণসংখ্যা ফলাফল;

	যখন(s1[0] && s2[0]){
		/* Sort by case insensitive part first */
		ফলাফল = toupper((unsigned অক্ষর)s1[0]) - toupper((unsigned অক্ষর)s2[0]);
		যদি(ফলাফল == 0){
			/* Case insensitive part matched, now distinguish between case */
			ফলাফল = s1[0] - s2[0];
			যদি(ফলাফল != 0){
				ফেরত ফলাফল;
			}
		}অন্যথা{
			/* Return case insensitive match fail */
			ফেরত ফলাফল;
		}
		s1++;
		s2++;
	}

	ফেরত s1[0] - s2[0];
}

#ifdef WIN32


পূর্ণসংখ্যা config__get_dir_files(ধ্রুবক অক্ষর *include_dir, অক্ষর ***files, পূর্ণসংখ্যা *file_গুনতি)
{
	size_t len;
	অক্ষর **l_files = ফাঁকা;
	পূর্ণসংখ্যা l_file_গুনতি = 0;
	অক্ষর **files_tmp;

	HANDLE fh;
	অক্ষর dirpath[MAX_PATH];
	WIN32_FIND_তথ্য find_তথ্য;

	snprintf(dirpath, MAX_PATH, "%s\\*.conf", include_dir);
	fh = FindFirstFile(dirpath, &find_তথ্য);
	যদি(fh == INVALID_HANDLE_মান){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		ফেরত 1;
	}

	করো{
		len = strlen(include_dir)+1+strlen(find_তথ্য.cFileName)+1;

		l_file_গুনতি++;
		files_tmp = mosquitto_realloc(l_files, l_file_গুনতি*আকার(অক্ষর *));
		যদি(!files_tmp){
			জন্য(পূর্ণসংখ্যা i=0; i<l_file_গুনতি-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			ফেরত MOSQ_ERR_NOMEM;
		}
		l_files = files_tmp;

		l_files[l_file_গুনতি-1] = mosquitto_malloc(len+1);
		যদি(!l_files[l_file_গুনতি-1]){
			জন্য(পূর্ণসংখ্যা i=0; i<l_file_গুনতি-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			ফেরত MOSQ_ERR_NOMEM;
		}
		snprintf(l_files[l_file_গুনতি-1], len, "%s/%s", include_dir, find_তথ্য.cFileName);
		l_files[l_file_গুনতি-1][len] = '\0';
	}যখন(Find_পরবর্তী_File(fh, &find_তথ্য));

	FindClose(fh);

	যদি(l_files){
		qsort(l_files, l_file_গুনতি, আকার(অক্ষর *), scmp_p);
	}
	*files = l_files;
	*file_গুনতি = l_file_গুনতি;

	ফেরত 0;
}
#endif


#ifndef WIN32


পূর্ণসংখ্যা config__get_dir_files(ধ্রুবক অক্ষর *include_dir, অক্ষর ***files, পূর্ণসংখ্যা *file_গুনতি)
{
	অক্ষর **l_files = ফাঁকা;
	পূর্ণসংখ্যা l_file_গুনতি = 0;
	অক্ষর **files_tmp;
	size_t len;

	DIR *dh;
	কাঠামো dirent *de;

	dh = opendir(include_dir);
	যদি(!dh){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		ফেরত 1;
	}
	যখন((de = readdir(dh)) != ফাঁকা){
		যদি(strlen(de->d_name) > 5){
			যদি(!strcmp(&de->d_name[strlen(de->d_name)-5], ".conf")){
				len = strlen(include_dir)+1+strlen(de->d_name)+1;

				l_file_গুনতি++;
				files_tmp = mosquitto_realloc(l_files, (size_t)l_file_গুনতি*আকার(অক্ষর *));
				যদি(!files_tmp){
					যাও error;
				}
				l_files = files_tmp;

				l_files[l_file_গুনতি-1] = mosquitto_malloc(len+1);
				যদি(!l_files[l_file_গুনতি-1]){
					যাও error;
				}
				snprintf(l_files[l_file_গুনতি-1], len, "%s/%s", include_dir, de->d_name);
				l_files[l_file_গুনতি-1][len] = '\0';
			}
		}
	}
	closedir(dh);

	যদি(l_files){
		qsort(l_files, (size_t)l_file_গুনতি, আকার(অক্ষর *), scmp_p);
	}
	*files = l_files;
	*file_গুনতি = l_file_গুনতি;

	ফেরত 0;
error:
	জন্য(পূর্ণসংখ্যা i=0; i<l_file_গুনতি-1; i++){
		mosquitto_FREE(l_files[i]);
	}
	mosquitto_FREE(l_files);
	closedir(dh);
	ফেরত MOSQ_ERR_NOMEM;
}
#endif


