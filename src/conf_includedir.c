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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ <ctype.h>
#અન્તર્ભાવ <limits.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <errno.h>

#ifdef WIN32
#નહીંતર
#  include <dirent.h>
#endif

#ifndef WIN32
#  include <strings.h>
#  include <netdb.h>
#  include <sys/socket.h>
#નહીંતર
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#જો defined(__HAIKU__)
#  include <syslog.h>
#અથવાજો !defined(WIN32) && !defined(__CYGWIN__) && !defined(__QNX__)
#  include <sys/syslog.h>
#endif

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "tls_mosq.h"
#અન્તર્ભાવ "util_mosq.h"
#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"


સ્થિર પૂર્ણાંક scmp_p(અચલ શૂન્ય *p1, અચલ શૂન્ય *p2)
{
	અચલ અક્ષર *s1 = *(અચલ અક્ષર **)p1;
	અચલ અક્ષર *s2 = *(અચલ અક્ષર **)p2;
	પૂર્ણાંક result;

	જ્યારે(s1[0] && s2[0]){
		/* Sort by case insensitive part first */
		result = toupper((unsigned અક્ષર)s1[0]) - toupper((unsigned અક્ષર)s2[0]);
		જો(result == 0){
			/* Case insensitive part matched, now distinguish between case */
			result = s1[0] - s2[0];
			જો(result != 0){
				પાછા result;
			}
		}નહીંતર{
			/* Return case insensitive match fail */
			પાછા result;
		}
		s1++;
		s2++;
	}

	પાછા s1[0] - s2[0];
}

#ifdef WIN32


પૂર્ણાંક config__get_dir_files(અચલ અક્ષર *include_dir, અક્ષર ***files, પૂર્ણાંક *file_ગણતરી)
{
	size_t len;
	અક્ષર **l_files = ખાલી;
	પૂર્ણાંક l_file_ગણતરી = 0;
	અક્ષર **files_tmp;

	HANDLE fh;
	અક્ષર dirpath[MAX_PATH];
	WIN32_FIND_DATA find_data;

	snprintf(dirpath, MAX_PATH, "%s\\*.conf", include_dir);
	fh = FindFirstFile(dirpath, &find_data);
	જો(fh == INVALID_HANDLE_કિંમત){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		પાછા 1;
	}

	કરો{
		len = strlen(include_dir)+1+strlen(find_data.cFileName)+1;

		l_file_ગણતરી++;
		files_tmp = mosquitto_realloc(l_files, l_file_ગણતરી*કદ(અક્ષર *));
		જો(!files_tmp){
			માટે(પૂર્ણાંક i=0; i<l_file_ગણતરી-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			પાછા MOSQ_ERR_NOMEM;
		}
		l_files = files_tmp;

		l_files[l_file_ગણતરી-1] = mosquitto_malloc(len+1);
		જો(!l_files[l_file_ગણતરી-1]){
			માટે(પૂર્ણાંક i=0; i<l_file_ગણતરી-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			પાછા MOSQ_ERR_NOMEM;
		}
		snprintf(l_files[l_file_ગણતરી-1], len, "%s/%s", include_dir, find_data.cFileName);
		l_files[l_file_ગણતરી-1][len] = '\0';
	}જ્યારે(FindNextFile(fh, &find_data));

	FindClose(fh);

	જો(l_files){
		qsort(l_files, l_file_ગણતરી, કદ(અક્ષર *), scmp_p);
	}
	*files = l_files;
	*file_ગણતરી = l_file_ગણતરી;

	પાછા 0;
}
#endif


#ifndef WIN32


પૂર્ણાંક config__get_dir_files(અચલ અક્ષર *include_dir, અક્ષર ***files, પૂર્ણાંક *file_ગણતરી)
{
	અક્ષર **l_files = ખાલી;
	પૂર્ણાંક l_file_ગણતરી = 0;
	અક્ષર **files_tmp;
	size_t len;

	DIR *dh;
	સંરચના dirent *de;

	dh = opendir(include_dir);
	જો(!dh){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		પાછા 1;
	}
	જ્યારે((de = readdir(dh)) != ખાલી){
		જો(strlen(de->d_name) > 5){
			જો(!strcmp(&de->d_name[strlen(de->d_name)-5], ".conf")){
				len = strlen(include_dir)+1+strlen(de->d_name)+1;

				l_file_ગણતરી++;
				files_tmp = mosquitto_realloc(l_files, (size_t)l_file_ગણતરી*કદ(અક્ષર *));
				જો(!files_tmp){
					જાઓ error;
				}
				l_files = files_tmp;

				l_files[l_file_ગણતરી-1] = mosquitto_malloc(len+1);
				જો(!l_files[l_file_ગણતરી-1]){
					જાઓ error;
				}
				snprintf(l_files[l_file_ગણતરી-1], len, "%s/%s", include_dir, de->d_name);
				l_files[l_file_ગણતરી-1][len] = '\0';
			}
		}
	}
	closedir(dh);

	જો(l_files){
		qsort(l_files, (size_t)l_file_ગણતરી, કદ(અક્ષર *), scmp_p);
	}
	*files = l_files;
	*file_ગણતરી = l_file_ગણતરી;

	પાછા 0;
error:
	માટે(પૂર્ણાંક i=0; i<l_file_ગણતરી-1; i++){
		mosquitto_FREE(l_files[i]);
	}
	mosquitto_FREE(l_files);
	closedir(dh);
	પાછા MOSQ_ERR_NOMEM;
}
#endif


