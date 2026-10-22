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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ <ctype.h>
#അന്തര്ഭാവ <limits.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <errno.h>

#ifdef WIN32
#അല്ലെങ്കിൽ
#  include <dirent.h>
#endif

#ifndef WIN32
#  include <strings.h>
#  include <netdb.h>
#  include <sys/socket.h>
#അല്ലെങ്കിൽ
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#എങ്കിൽ defined(__HAIKU__)
#  include <syslog.h>
#അല്ലെങ്കിലെങ്കിൽ !defined(WIN32) && !defined(__CYGWIN__) && !defined(__QNX__)
#  include <sys/syslog.h>
#endif

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "tls_mosq.h"
#അന്തര്ഭാവ "util_mosq.h"
#അന്തര്ഭാവ "mosquitto/mqtt_protocol.h"


സ്ഥിതം പൂർണ്ണസംഖ്യ scmp_p(സ്ഥിരം ശൂന്യം *p1, സ്ഥിരം ശൂന്യം *p2)
{
	സ്ഥിരം അക്ഷരം *s1 = *(സ്ഥിരം അക്ഷരം **)p1;
	സ്ഥിരം അക്ഷരം *s2 = *(സ്ഥിരം അക്ഷരം **)p2;
	പൂർണ്ണസംഖ്യ result;

	ആകുമ്പോൾ(s1[0] && s2[0]){
		/* Sort by case insensitive part first */
		result = toupper((unsigned അക്ഷരം)s1[0]) - toupper((unsigned അക്ഷരം)s2[0]);
		എങ്കിൽ(result == 0){
			/* Case insensitive part matched, now distinguish between case */
			result = s1[0] - s2[0];
			എങ്കിൽ(result != 0){
				തിരികെ result;
			}
		}അല്ലെങ്കിൽ{
			/* Return case insensitive match fail */
			തിരികെ result;
		}
		s1++;
		s2++;
	}

	തിരികെ s1[0] - s2[0];
}

#ifdef WIN32


പൂർണ്ണസംഖ്യ config__get_dir_files(സ്ഥിരം അക്ഷരം *include_dir, അക്ഷരം ***files, പൂർണ്ണസംഖ്യ *file_എണ്ണം)
{
	size_t len;
	അക്ഷരം **l_files = ശൂന്യ;
	പൂർണ്ണസംഖ്യ l_file_എണ്ണം = 0;
	അക്ഷരം **files_tmp;

	HANDLE fh;
	അക്ഷരം dirpath[MAX_PATH];
	WIN32_FIND_DATA find_data;

	snprintf(dirpath, MAX_PATH, "%s\\*.conf", include_dir);
	fh = FindFirstFile(dirpath, &find_data);
	എങ്കിൽ(fh == INVALID_HANDLE_മൂല്യം){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		തിരികെ 1;
	}

	ചെയ്യുക{
		len = strlen(include_dir)+1+strlen(find_data.cFileName)+1;

		l_file_എണ്ണം++;
		files_tmp = mosquitto_realloc(l_files, l_file_എണ്ണം*വലുപ്പം(അക്ഷരം *));
		എങ്കിൽ(!files_tmp){
			വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<l_file_എണ്ണം-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			തിരികെ MOSQ_ERR_NOMEM;
		}
		l_files = files_tmp;

		l_files[l_file_എണ്ണം-1] = mosquitto_malloc(len+1);
		എങ്കിൽ(!l_files[l_file_എണ്ണം-1]){
			വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<l_file_എണ്ണം-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			തിരികെ MOSQ_ERR_NOMEM;
		}
		snprintf(l_files[l_file_എണ്ണം-1], len, "%s/%s", include_dir, find_data.cFileName);
		l_files[l_file_എണ്ണം-1][len] = '\0';
	}ആകുമ്പോൾ(FindNextFile(fh, &find_data));

	FindClose(fh);

	എങ്കിൽ(l_files){
		qsort(l_files, l_file_എണ്ണം, വലുപ്പം(അക്ഷരം *), scmp_p);
	}
	*files = l_files;
	*file_എണ്ണം = l_file_എണ്ണം;

	തിരികെ 0;
}
#endif


#ifndef WIN32


പൂർണ്ണസംഖ്യ config__get_dir_files(സ്ഥിരം അക്ഷരം *include_dir, അക്ഷരം ***files, പൂർണ്ണസംഖ്യ *file_എണ്ണം)
{
	അക്ഷരം **l_files = ശൂന്യ;
	പൂർണ്ണസംഖ്യ l_file_എണ്ണം = 0;
	അക്ഷരം **files_tmp;
	size_t len;

	DIR *dh;
	ഘടന dirent *de;

	dh = opendir(include_dir);
	എങ്കിൽ(!dh){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		തിരികെ 1;
	}
	ആകുമ്പോൾ((de = readdir(dh)) != ശൂന്യ){
		എങ്കിൽ(strlen(de->d_name) > 5){
			എങ്കിൽ(!strcmp(&de->d_name[strlen(de->d_name)-5], ".conf")){
				len = strlen(include_dir)+1+strlen(de->d_name)+1;

				l_file_എണ്ണം++;
				files_tmp = mosquitto_realloc(l_files, (size_t)l_file_എണ്ണം*വലുപ്പം(അക്ഷരം *));
				എങ്കിൽ(!files_tmp){
					പോകുക error;
				}
				l_files = files_tmp;

				l_files[l_file_എണ്ണം-1] = mosquitto_malloc(len+1);
				എങ്കിൽ(!l_files[l_file_എണ്ണം-1]){
					പോകുക error;
				}
				snprintf(l_files[l_file_എണ്ണം-1], len, "%s/%s", include_dir, de->d_name);
				l_files[l_file_എണ്ണം-1][len] = '\0';
			}
		}
	}
	closedir(dh);

	എങ്കിൽ(l_files){
		qsort(l_files, (size_t)l_file_എണ്ണം, വലുപ്പം(അക്ഷരം *), scmp_p);
	}
	*files = l_files;
	*file_എണ്ണം = l_file_എണ്ണം;

	തിരികെ 0;
error:
	വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<l_file_എണ്ണം-1; i++){
		mosquitto_FREE(l_files[i]);
	}
	mosquitto_FREE(l_files);
	closedir(dh);
	തിരികെ MOSQ_ERR_NOMEM;
}
#endif


