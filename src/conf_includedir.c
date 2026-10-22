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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ <ctype.h>
#அந்தர்பாவ <limits.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <errno.h>

#ifdef WIN32
#இல்லையெனில்
#  include <dirent.h>
#endif

#ifndef WIN32
#  include <strings.h>
#  include <netdb.h>
#  include <sys/socket.h>
#இல்லையெனில்
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#எனில் defined(__HAIKU__)
#  include <syslog.h>
#அல்லதெனில் !defined(WIN32) && !defined(__CYGWIN__) && !defined(__QNX__)
#  include <sys/syslog.h>
#endif

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "tls_mosq.h"
#அந்தர்பாவ "util_mosq.h"
#அந்தர்பாவ "mosquitto/mqtt_protocol.h"


நிலையான முழு scmp_p(மாறிலி வெறுமை *p1, மாறிலி வெறுமை *p2)
{
	மாறிலி எழுத்து *s1 = *(மாறிலி எழுத்து **)p1;
	மாறிலி எழுத்து *s2 = *(மாறிலி எழுத்து **)p2;
	முழு result;

	போது(s1[0] && s2[0]){
		/* Sort by case insensitive part first */
		result = toupper((unsigned எழுத்து)s1[0]) - toupper((unsigned எழுத்து)s2[0]);
		எனில்(result == 0){
			/* Case insensitive part matched, now distinguish between case */
			result = s1[0] - s2[0];
			எனில்(result != 0){
				திரும்பு result;
			}
		}இல்லையெனில்{
			/* Return case insensitive match fail */
			திரும்பு result;
		}
		s1++;
		s2++;
	}

	திரும்பு s1[0] - s2[0];
}

#ifdef WIN32


முழு config__get_dir_files(மாறிலி எழுத்து *include_dir, எழுத்து ***files, முழு *file_மொத்தம்)
{
	size_t len;
	எழுத்து **l_files = வெற்று;
	முழு l_file_மொத்தம் = 0;
	எழுத்து **files_tmp;

	HANDLE fh;
	எழுத்து dirpath[MAX_PATH];
	WIN32_FIND_DATA find_data;

	snprintf(dirpath, MAX_PATH, "%s\\*.conf", include_dir);
	fh = FindFirstFile(dirpath, &find_data);
	எனில்(fh == INVALID_HANDLE_மதிப்பு){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		திரும்பு 1;
	}

	செய்{
		len = strlen(include_dir)+1+strlen(find_data.cFileName)+1;

		l_file_மொத்தம்++;
		files_tmp = mosquitto_realloc(l_files, l_file_மொத்தம்*அளவு(எழுத்து *));
		எனில்(!files_tmp){
			க்கு(முழு i=0; i<l_file_மொத்தம்-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			திரும்பு MOSQ_ERR_NOMEM;
		}
		l_files = files_tmp;

		l_files[l_file_மொத்தம்-1] = mosquitto_malloc(len+1);
		எனில்(!l_files[l_file_மொத்தம்-1]){
			க்கு(முழு i=0; i<l_file_மொத்தம்-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			திரும்பு MOSQ_ERR_NOMEM;
		}
		snprintf(l_files[l_file_மொத்தம்-1], len, "%s/%s", include_dir, find_data.cFileName);
		l_files[l_file_மொத்தம்-1][len] = '\0';
	}போது(FindNextFile(fh, &find_data));

	FindClose(fh);

	எனில்(l_files){
		qsort(l_files, l_file_மொத்தம், அளவு(எழுத்து *), scmp_p);
	}
	*files = l_files;
	*file_மொத்தம் = l_file_மொத்தம்;

	திரும்பு 0;
}
#endif


#ifndef WIN32


முழு config__get_dir_files(மாறிலி எழுத்து *include_dir, எழுத்து ***files, முழு *file_மொத்தம்)
{
	எழுத்து **l_files = வெற்று;
	முழு l_file_மொத்தம் = 0;
	எழுத்து **files_tmp;
	size_t len;

	DIR *dh;
	கட்டமைப்பு dirent *de;

	dh = opendir(include_dir);
	எனில்(!dh){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		திரும்பு 1;
	}
	போது((de = readdir(dh)) != வெற்று){
		எனில்(strlen(de->d_name) > 5){
			எனில்(!strcmp(&de->d_name[strlen(de->d_name)-5], ".conf")){
				len = strlen(include_dir)+1+strlen(de->d_name)+1;

				l_file_மொத்தம்++;
				files_tmp = mosquitto_realloc(l_files, (size_t)l_file_மொத்தம்*அளவு(எழுத்து *));
				எனில்(!files_tmp){
					செல் error;
				}
				l_files = files_tmp;

				l_files[l_file_மொத்தம்-1] = mosquitto_malloc(len+1);
				எனில்(!l_files[l_file_மொத்தம்-1]){
					செல் error;
				}
				snprintf(l_files[l_file_மொத்தம்-1], len, "%s/%s", include_dir, de->d_name);
				l_files[l_file_மொத்தம்-1][len] = '\0';
			}
		}
	}
	closedir(dh);

	எனில்(l_files){
		qsort(l_files, (size_t)l_file_மொத்தம், அளவு(எழுத்து *), scmp_p);
	}
	*files = l_files;
	*file_மொத்தம் = l_file_மொத்தம்;

	திரும்பு 0;
error:
	க்கு(முழு i=0; i<l_file_மொத்தம்-1; i++){
		mosquitto_FREE(l_files[i]);
	}
	mosquitto_FREE(l_files);
	closedir(dh);
	திரும்பு MOSQ_ERR_NOMEM;
}
#endif


