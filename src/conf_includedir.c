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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <ctype.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <limits.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
#ਨਹੀਂਤਾਂ
#  include <dirent.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
#  include <strings.h>
#  include <netdb.h>
#  include <sys/socket.h>
#ਨਹੀਂਤਾਂ
#  include <winsock2.h>
#  include <ws2tcpip.h>
#ਜੇ_ਅੰਤ

#ਜੇ defined(__HAIKU__)
#  include <syslog.h>
#ਜਾਂਜੇ !defined(WIN32) && !defined(__CYGWIN__) && !defined(__QNX__)
#  include <sys/syslog.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "tls_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"


ਸਥਿਰ ਪੂਰਨ scmp_p(ਅਚਲ ਖਾਲੀ *p1, ਅਚਲ ਖਾਲੀ *p2)
{
	ਅਚਲ ਅੱਖਰ *s1 = *(ਅਚਲ ਅੱਖਰ **)p1;
	ਅਚਲ ਅੱਖਰ *s2 = *(ਅਚਲ ਅੱਖਰ **)p2;
	ਪੂਰਨ ਨਤੀਜਾ;

	ਜਦੋਂ(s1[0] && s2[0]){
		/* Sort by case insensitive part first */
		ਨਤੀਜਾ = ਵੱਡਾ_ਅੱਖਰ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)s1[0]) - ਵੱਡਾ_ਅੱਖਰ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)s2[0]);
		ਜੇ(ਨਤੀਜਾ == 0){
			/* Case insensitive part matched, now distinguish between case */
			ਨਤੀਜਾ = s1[0] - s2[0];
			ਜੇ(ਨਤੀਜਾ != 0){
				ਵਾਪਸ ਨਤੀਜਾ;
			}
		}ਨਹੀਂਤਾਂ{
			/* Return case insensitive match fail */
			ਵਾਪਸ ਨਤੀਜਾ;
		}
		s1++;
		s2++;
	}

	ਵਾਪਸ s1[0] - s2[0];
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32


ਪੂਰਨ config__get_dir_files(ਅਚਲ ਅੱਖਰ *include_dir, ਅੱਖਰ ***files, ਪੂਰਨ *file_ਗਿਣਤੀ)
{
	size_t len;
	ਅੱਖਰ **l_files = ਨਿਰਾ;
	ਪੂਰਨ l_file_ਗਿਣਤੀ = 0;
	ਅੱਖਰ **files_tmp;

	HANDLE fh;
	ਅੱਖਰ dirpath[MAX_ਰਸਤਾ];
	WIN32_ਲੱਭੋ_ਅੰਕੜੇ ਲੱਭੋ_ਅੰਕੜੇ;

	snprintf(dirpath, MAX_ਰਸਤਾ, "%s\\*.conf", include_dir);
	fh = ਲੱਭੋ_ਪਹਿਲਾ_File(dirpath, &ਲੱਭੋ_ਅੰਕੜੇ);
	ਜੇ(fh == INVALID_HANDLE_ਮੁੱਲ){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		ਵਾਪਸ 1;
	}

	ਕਰੋ{
		len = ਲੜੀ_ਲੰਬਾਈ(include_dir)+1+ਲੜੀ_ਲੰਬਾਈ(ਲੱਭੋ_ਅੰਕੜੇ.cFileName)+1;

		l_file_ਗਿਣਤੀ++;
		files_tmp = mosquitto_realloc(l_files, l_file_ਗਿਣਤੀ*ਆਕਾਰ(ਅੱਖਰ *));
		ਜੇ(!files_tmp){
			ਲਈ(ਪੂਰਨ i=0; i<l_file_ਗਿਣਤੀ-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			ਲੱਭੋ_Close(fh);
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}
		l_files = files_tmp;

		l_files[l_file_ਗਿਣਤੀ-1] = mosquitto_malloc(len+1);
		ਜੇ(!l_files[l_file_ਗਿਣਤੀ-1]){
			ਲਈ(ਪੂਰਨ i=0; i<l_file_ਗਿਣਤੀ-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			ਲੱਭੋ_Close(fh);
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}
		snprintf(l_files[l_file_ਗਿਣਤੀ-1], len, "%s/%s", include_dir, ਲੱਭੋ_ਅੰਕੜੇ.cFileName);
		l_files[l_file_ਗਿਣਤੀ-1][len] = '\0';
	}ਜਦੋਂ(ਲੱਭੋ_ਅਗਲਾ_File(fh, &ਲੱਭੋ_ਅੰਕੜੇ));

	ਲੱਭੋ_Close(fh);

	ਜੇ(l_files){
		qsort(l_files, l_file_ਗਿਣਤੀ, ਆਕਾਰ(ਅੱਖਰ *), scmp_p);
	}
	*files = l_files;
	*file_ਗਿਣਤੀ = l_file_ਗਿਣਤੀ;

	ਵਾਪਸ 0;
}
#ਜੇ_ਅੰਤ


#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32


ਪੂਰਨ config__get_dir_files(ਅਚਲ ਅੱਖਰ *include_dir, ਅੱਖਰ ***files, ਪੂਰਨ *file_ਗਿਣਤੀ)
{
	ਅੱਖਰ **l_files = ਨਿਰਾ;
	ਪੂਰਨ l_file_ਗਿਣਤੀ = 0;
	ਅੱਖਰ **files_tmp;
	size_t len;

	DIR *dh;
	ਬਣਤਰ dirent *de;

	dh = opendir(include_dir);
	ਜੇ(!dh){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		ਵਾਪਸ 1;
	}
	ਜਦੋਂ((de = readdir(dh)) != ਨਿਰਾ){
		ਜੇ(ਲੜੀ_ਲੰਬਾਈ(de->d_name) > 5){
			ਜੇ(!strcmp(&de->d_name[ਲੜੀ_ਲੰਬਾਈ(de->d_name)-5], ".conf")){
				len = ਲੜੀ_ਲੰਬਾਈ(include_dir)+1+ਲੜੀ_ਲੰਬਾਈ(de->d_name)+1;

				l_file_ਗਿਣਤੀ++;
				files_tmp = mosquitto_realloc(l_files, (size_t)l_file_ਗਿਣਤੀ*ਆਕਾਰ(ਅੱਖਰ *));
				ਜੇ(!files_tmp){
					ਜਾਓ error;
				}
				l_files = files_tmp;

				l_files[l_file_ਗਿਣਤੀ-1] = mosquitto_malloc(len+1);
				ਜੇ(!l_files[l_file_ਗਿਣਤੀ-1]){
					ਜਾਓ error;
				}
				snprintf(l_files[l_file_ਗਿਣਤੀ-1], len, "%s/%s", include_dir, de->d_name);
				l_files[l_file_ਗਿਣਤੀ-1][len] = '\0';
			}
		}
	}
	closedir(dh);

	ਜੇ(l_files){
		qsort(l_files, (size_t)l_file_ਗਿਣਤੀ, ਆਕਾਰ(ਅੱਖਰ *), scmp_p);
	}
	*files = l_files;
	*file_ਗਿਣਤੀ = l_file_ਗਿਣਤੀ;

	ਵਾਪਸ 0;
error:
	ਲਈ(ਪੂਰਨ i=0; i<l_file_ਗਿਣਤੀ-1; i++){
		mosquitto_FREE(l_files[i]);
	}
	mosquitto_FREE(l_files);
	closedir(dh);
	ਵਾਪਸ MOSQ_ERR_NOMEM;
}
#ਜੇ_ਅੰਤ


