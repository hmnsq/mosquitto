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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <ctype.h>
#ಅನ್ತರ್ಭಾವ <limits.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <errno.h>

#ifdef WIN32
#ಅನ್ಯಥಾ
#  include <dirent.h>
#endif

#ifndef WIN32
#  include <strings.h>
#  include <netdb.h>
#  include <sys/socket.h>
#ಅನ್ಯಥಾ
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#ಯದಿ defined(__HAIKU__)
#  include <syslog.h>
#ನೋಚೇದ್ಯದಿ !defined(WIN32) && !defined(__CYGWIN__) && !defined(__QNX__)
#  include <sys/syslog.h>
#endif

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "tls_mosq.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ scmp_p(ಧ್ರುವ ಶೂನ್ಯ *p1, ಧ್ರುವ ಶೂನ್ಯ *p2)
{
	ಧ್ರುವ ವರ್ಣ *s1 = *(ಧ್ರುವ ವರ್ಣ **)p1;
	ಧ್ರುವ ವರ್ಣ *s2 = *(ಧ್ರುವ ವರ್ಣ **)p2;
	ಪೂರ್ಣಾಙ್ಕ result;

	ಯಾವತ್(s1[0] && s2[0]){
		/* Sort by case insensitive part first */
		result = toupper((unsigned ವರ್ಣ)s1[0]) - toupper((unsigned ವರ್ಣ)s2[0]);
		ಯದಿ(result == 0){
			/* Case insensitive part matched, now distinguish between case */
			result = s1[0] - s2[0];
			ಯದಿ(result != 0){
				ಪ್ರತ್ಯಾಗಮ result;
			}
		}ಅನ್ಯಥಾ{
			/* Return case insensitive match fail */
			ಪ್ರತ್ಯಾಗಮ result;
		}
		s1++;
		s2++;
	}

	ಪ್ರತ್ಯಾಗಮ s1[0] - s2[0];
}

#ifdef WIN32


ಪೂರ್ಣಾಙ್ಕ config__get_dir_files(ಧ್ರುವ ವರ್ಣ *include_dir, ವರ್ಣ ***files, ಪೂರ್ಣಾಙ್ಕ *file_ಎಣಿಕೆ)
{
	size_t len;
	ವರ್ಣ **l_files = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ l_file_ಎಣಿಕೆ = 0;
	ವರ್ಣ **files_tmp;

	HANDLE fh;
	ವರ್ಣ dirpath[MAX_PATH];
	WIN32_FIND_DATA find_data;

	snprintf(dirpath, MAX_PATH, "%s\\*.conf", include_dir);
	fh = FindFirstFile(dirpath, &find_data);
	ಯದಿ(fh == INVALID_HANDLE_ಮೌಲ್ಯ){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		ಪ್ರತ್ಯಾಗಮ 1;
	}

	ಕುರು{
		len = strlen(include_dir)+1+strlen(find_data.cFileName)+1;

		l_file_ಎಣಿಕೆ++;
		files_tmp = mosquitto_realloc(l_files, l_file_ಎಣಿಕೆ*ಪರಿಮಾಣ(ವರ್ಣ *));
		ಯದಿ(!files_tmp){
			ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<l_file_ಎಣಿಕೆ-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}
		l_files = files_tmp;

		l_files[l_file_ಎಣಿಕೆ-1] = mosquitto_malloc(len+1);
		ಯದಿ(!l_files[l_file_ಎಣಿಕೆ-1]){
			ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<l_file_ಎಣಿಕೆ-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}
		snprintf(l_files[l_file_ಎಣಿಕೆ-1], len, "%s/%s", include_dir, find_data.cFileName);
		l_files[l_file_ಎಣಿಕೆ-1][len] = '\0';
	}ಯಾವತ್(FindNextFile(fh, &find_data));

	FindClose(fh);

	ಯದಿ(l_files){
		qsort(l_files, l_file_ಎಣಿಕೆ, ಪರಿಮಾಣ(ವರ್ಣ *), scmp_p);
	}
	*files = l_files;
	*file_ಎಣಿಕೆ = l_file_ಎಣಿಕೆ;

	ಪ್ರತ್ಯಾಗಮ 0;
}
#endif


#ifndef WIN32


ಪೂರ್ಣಾಙ್ಕ config__get_dir_files(ಧ್ರುವ ವರ್ಣ *include_dir, ವರ್ಣ ***files, ಪೂರ್ಣಾಙ್ಕ *file_ಎಣಿಕೆ)
{
	ವರ್ಣ **l_files = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ l_file_ಎಣಿಕೆ = 0;
	ವರ್ಣ **files_tmp;
	size_t len;

	DIR *dh;
	ಸಂರಚನಾ dirent *de;

	dh = opendir(include_dir);
	ಯದಿ(!dh){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		ಪ್ರತ್ಯಾಗಮ 1;
	}
	ಯಾವತ್((de = readdir(dh)) != ರಿಕ್ತ){
		ಯದಿ(strlen(de->d_name) > 5){
			ಯದಿ(!strcmp(&de->d_name[strlen(de->d_name)-5], ".conf")){
				len = strlen(include_dir)+1+strlen(de->d_name)+1;

				l_file_ಎಣಿಕೆ++;
				files_tmp = mosquitto_realloc(l_files, (size_t)l_file_ಎಣಿಕೆ*ಪರಿಮಾಣ(ವರ್ಣ *));
				ಯದಿ(!files_tmp){
					ಗಚ್ಛ error;
				}
				l_files = files_tmp;

				l_files[l_file_ಎಣಿಕೆ-1] = mosquitto_malloc(len+1);
				ಯದಿ(!l_files[l_file_ಎಣಿಕೆ-1]){
					ಗಚ್ಛ error;
				}
				snprintf(l_files[l_file_ಎಣಿಕೆ-1], len, "%s/%s", include_dir, de->d_name);
				l_files[l_file_ಎಣಿಕೆ-1][len] = '\0';
			}
		}
	}
	closedir(dh);

	ಯದಿ(l_files){
		qsort(l_files, (size_t)l_file_ಎಣಿಕೆ, ಪರಿಮಾಣ(ವರ್ಣ *), scmp_p);
	}
	*files = l_files;
	*file_ಎಣಿಕೆ = l_file_ಎಣಿಕೆ;

	ಪ್ರತ್ಯಾಗಮ 0;
error:
	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<l_file_ಎಣಿಕೆ-1; i++){
		mosquitto_FREE(l_files[i]);
	}
	mosquitto_FREE(l_files);
	closedir(dh);
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
}
#endif


