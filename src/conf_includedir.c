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

#অন্তর্ভাৱ "config.h"

#অন্তর্ভাৱ <ctype.h>
#অন্তর্ভাৱ <limits.h>
#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <stdlib.h>
#অন্তর্ভাৱ <string.h>
#অন্তর্ভাৱ <errno.h>

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
#নোচেদ্যদি !defined(WIN32) && !defined(__CYGWIN__) && !defined(__QNX__)
#  include <sys/syslog.h>
#endif

#অন্তর্ভাৱ "mosquitto_broker_internal.h"
#অন্তর্ভাৱ "tls_mosq.h"
#অন্তর্ভাৱ "util_mosq.h"
#অন্তর্ভাৱ "mosquitto/mqtt_protocol.h"


স্থির পূর্ণাঙ্ক scmp_p(ধ্রুৱ শূন্য *p1, ধ্রুৱ শূন্য *p2)
{
	ধ্রুৱ ৱর্ণ *s1 = *(ধ্রুৱ ৱর্ণ **)p1;
	ধ্রুৱ ৱর্ণ *s2 = *(ধ্রুৱ ৱর্ণ **)p2;
	পূর্ণাঙ্ক result;

	যেতিয়ালৈ(s1[0] && s2[0]){
		/* Sort by case insensitive part first */
		result = toupper((unsigned ৱর্ণ)s1[0]) - toupper((unsigned ৱর্ণ)s2[0]);
		যদি(result == 0){
			/* Case insensitive part matched, now distinguish between case */
			result = s1[0] - s2[0];
			যদি(result != 0){
				ঘূৰাই result;
			}
		}অন্যথা{
			/* Return case insensitive match fail */
			ঘূৰাই result;
		}
		s1++;
		s2++;
	}

	ঘূৰাই s1[0] - s2[0];
}

#ifdef WIN32


পূর্ণাঙ্ক config__get_dir_files(ধ্রুৱ ৱর্ণ *include_dir, ৱর্ণ ***files, পূর্ণাঙ্ক *file_গুনতি)
{
	size_t len;
	ৱর্ণ **l_files = রিক্ত;
	পূর্ণাঙ্ক l_file_গুনতি = 0;
	ৱর্ণ **files_tmp;

	HANDLE fh;
	ৱর্ণ dirpath[MAX_PATH];
	WIN32_FIND_DATA find_data;

	snprintf(dirpath, MAX_PATH, "%s\\*.conf", include_dir);
	fh = FindFirstFile(dirpath, &find_data);
	যদি(fh == INVALID_HANDLE_মান){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		ঘূৰাই 1;
	}

	কৰা{
		len = strlen(include_dir)+1+strlen(find_data.cFileName)+1;

		l_file_গুনতি++;
		files_tmp = mosquitto_realloc(l_files, l_file_গুনতি*পরিমাণ(ৱর্ণ *));
		যদি(!files_tmp){
			বাবে(পূর্ণাঙ্ক i=0; i<l_file_গুনতি-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			ঘূৰাই MOSQ_ERR_NOMEM;
		}
		l_files = files_tmp;

		l_files[l_file_গুনতি-1] = mosquitto_malloc(len+1);
		যদি(!l_files[l_file_গুনতি-1]){
			বাবে(পূর্ণাঙ্ক i=0; i<l_file_গুনতি-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			ঘূৰাই MOSQ_ERR_NOMEM;
		}
		snprintf(l_files[l_file_গুনতি-1], len, "%s/%s", include_dir, find_data.cFileName);
		l_files[l_file_গুনতি-1][len] = '\0';
	}যেতিয়ালৈ(FindNextFile(fh, &find_data));

	FindClose(fh);

	যদি(l_files){
		qsort(l_files, l_file_গুনতি, পরিমাণ(ৱর্ণ *), scmp_p);
	}
	*files = l_files;
	*file_গুনতি = l_file_গুনতি;

	ঘূৰাই 0;
}
#endif


#ifndef WIN32


পূর্ণাঙ্ক config__get_dir_files(ধ্রুৱ ৱর্ণ *include_dir, ৱর্ণ ***files, পূর্ণাঙ্ক *file_গুনতি)
{
	ৱর্ণ **l_files = রিক্ত;
	পূর্ণাঙ্ক l_file_গুনতি = 0;
	ৱর্ণ **files_tmp;
	size_t len;

	DIR *dh;
	সংরচনা dirent *de;

	dh = opendir(include_dir);
	যদি(!dh){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		ঘূৰাই 1;
	}
	যেতিয়ালৈ((de = readdir(dh)) != রিক্ত){
		যদি(strlen(de->d_name) > 5){
			যদি(!strcmp(&de->d_name[strlen(de->d_name)-5], ".conf")){
				len = strlen(include_dir)+1+strlen(de->d_name)+1;

				l_file_গুনতি++;
				files_tmp = mosquitto_realloc(l_files, (size_t)l_file_গুনতি*পরিমাণ(ৱর্ণ *));
				যদি(!files_tmp){
					গচ্ছ error;
				}
				l_files = files_tmp;

				l_files[l_file_গুনতি-1] = mosquitto_malloc(len+1);
				যদি(!l_files[l_file_গুনতি-1]){
					গচ্ছ error;
				}
				snprintf(l_files[l_file_গুনতি-1], len, "%s/%s", include_dir, de->d_name);
				l_files[l_file_গুনতি-1][len] = '\0';
			}
		}
	}
	closedir(dh);

	যদি(l_files){
		qsort(l_files, (size_t)l_file_গুনতি, পরিমাণ(ৱর্ণ *), scmp_p);
	}
	*files = l_files;
	*file_গুনতি = l_file_গুনতি;

	ঘূৰাই 0;
error:
	বাবে(পূর্ণাঙ্ক i=0; i<l_file_গুনতি-1; i++){
		mosquitto_FREE(l_files[i]);
	}
	mosquitto_FREE(l_files);
	closedir(dh);
	ঘূৰাই MOSQ_ERR_NOMEM;
}
#endif


