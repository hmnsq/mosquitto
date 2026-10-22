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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <ctype.h>
#ଅନ୍ତର୍ଭାଵ <limits.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <errno.h>

#ifdef WIN32
#ଅନ୍ୟଥା
#  include <dirent.h>
#endif

#ifndef WIN32
#  include <strings.h>
#  include <netdb.h>
#  include <sys/socket.h>
#ଅନ୍ୟଥା
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#ଯଦି defined(__HAIKU__)
#  include <syslog.h>
#ନଚେତଯଦି !defined(WIN32) && !defined(__CYGWIN__) && !defined(__QNX__)
#  include <sys/syslog.h>
#endif

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "tls_mosq.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା scmp_p(ଅଚଳ ଶୂନ୍ୟ *p1, ଅଚଳ ଶୂନ୍ୟ *p2)
{
	ଅଚଳ ଅକ୍ଷର *s1 = *(ଅଚଳ ଅକ୍ଷର **)p1;
	ଅଚଳ ଅକ୍ଷର *s2 = *(ଅଚଳ ଅକ୍ଷର **)p2;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା result;

	ଯେତେବେଳେ(s1[0] && s2[0]){
		/* Sort by case insensitive part first */
		result = toupper((unsigned ଅକ୍ଷର)s1[0]) - toupper((unsigned ଅକ୍ଷର)s2[0]);
		ଯଦି(result == 0){
			/* Case insensitive part matched, now distinguish between case */
			result = s1[0] - s2[0];
			ଯଦି(result != 0){
				ଫେରନ୍ତୁ result;
			}
		}ଅନ୍ୟଥା{
			/* Return case insensitive match fail */
			ଫେରନ୍ତୁ result;
		}
		s1++;
		s2++;
	}

	ଫେରନ୍ତୁ s1[0] - s2[0];
}

#ifdef WIN32


ପୂର୍ଣ୍ଣସଂଖ୍ୟା config__get_dir_files(ଅଚଳ ଅକ୍ଷର *include_dir, ଅକ୍ଷର ***files, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *file_ଗଣନା)
{
	size_t len;
	ଅକ୍ଷର **l_files = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା l_file_ଗଣନା = 0;
	ଅକ୍ଷର **files_tmp;

	HANDLE fh;
	ଅକ୍ଷର dirpath[MAX_PATH];
	WIN32_FIND_DATA find_data;

	snprintf(dirpath, MAX_PATH, "%s\\*.conf", include_dir);
	fh = FindFirstFile(dirpath, &find_data);
	ଯଦି(fh == INVALID_HANDLE_ମୂଲ୍ୟ){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		ଫେରନ୍ତୁ 1;
	}

	କର{
		len = strlen(include_dir)+1+strlen(find_data.cFileName)+1;

		l_file_ଗଣନା++;
		files_tmp = mosquitto_realloc(l_files, l_file_ଗଣନା*ଆକାର(ଅକ୍ଷର *));
		ଯଦି(!files_tmp){
			ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<l_file_ଗଣନା-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}
		l_files = files_tmp;

		l_files[l_file_ଗଣନା-1] = mosquitto_malloc(len+1);
		ଯଦି(!l_files[l_file_ଗଣନା-1]){
			ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<l_file_ଗଣନା-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}
		snprintf(l_files[l_file_ଗଣନା-1], len, "%s/%s", include_dir, find_data.cFileName);
		l_files[l_file_ଗଣନା-1][len] = '\0';
	}ଯେତେବେଳେ(FindNextFile(fh, &find_data));

	FindClose(fh);

	ଯଦି(l_files){
		qsort(l_files, l_file_ଗଣନା, ଆକାର(ଅକ୍ଷର *), scmp_p);
	}
	*files = l_files;
	*file_ଗଣନା = l_file_ଗଣନା;

	ଫେରନ୍ତୁ 0;
}
#endif


#ifndef WIN32


ପୂର୍ଣ୍ଣସଂଖ୍ୟା config__get_dir_files(ଅଚଳ ଅକ୍ଷର *include_dir, ଅକ୍ଷର ***files, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *file_ଗଣନା)
{
	ଅକ୍ଷର **l_files = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା l_file_ଗଣନା = 0;
	ଅକ୍ଷର **files_tmp;
	size_t len;

	DIR *dh;
	ସଂରଚନା dirent *de;

	dh = opendir(include_dir);
	ଯଦି(!dh){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		ଫେରନ୍ତୁ 1;
	}
	ଯେତେବେଳେ((de = readdir(dh)) != ଖାଲି){
		ଯଦି(strlen(de->d_name) > 5){
			ଯଦି(!strcmp(&de->d_name[strlen(de->d_name)-5], ".conf")){
				len = strlen(include_dir)+1+strlen(de->d_name)+1;

				l_file_ଗଣନା++;
				files_tmp = mosquitto_realloc(l_files, (size_t)l_file_ଗଣନା*ଆକାର(ଅକ୍ଷର *));
				ଯଦି(!files_tmp){
					ଯାଅ error;
				}
				l_files = files_tmp;

				l_files[l_file_ଗଣନା-1] = mosquitto_malloc(len+1);
				ଯଦି(!l_files[l_file_ଗଣନା-1]){
					ଯାଅ error;
				}
				snprintf(l_files[l_file_ଗଣନା-1], len, "%s/%s", include_dir, de->d_name);
				l_files[l_file_ଗଣନା-1][len] = '\0';
			}
		}
	}
	closedir(dh);

	ଯଦି(l_files){
		qsort(l_files, (size_t)l_file_ଗଣନା, ଆକାର(ଅକ୍ଷର *), scmp_p);
	}
	*files = l_files;
	*file_ଗଣନା = l_file_ଗଣନା;

	ଫେରନ୍ତୁ 0;
error:
	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<l_file_ଗଣନା-1; i++){
		mosquitto_FREE(l_files[i]);
	}
	mosquitto_FREE(l_files);
	closedir(dh);
	ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
}
#endif


