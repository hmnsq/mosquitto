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

#ضمن "config.h"

#ضمن <ctype.h>
#ضمن <limits.h>
#ضمن <stdio.h>
#ضمن <stdlib.h>
#ضمن <string.h>
#ضمن <errno.h>

#جيڪڏهن_وصف_ٿيل WIN32
#وإلا
#  include <dirent.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل WIN32
#  include <strings.h>
#  include <netdb.h>
#  include <sys/socket.h>
#وإلا
#  include <winsock2.h>
#  include <ws2tcpip.h>
#جيڪڏهن_پورو

#إذا defined(__HAIKU__)
#  include <syslog.h>
#وإلاإذا !defined(WIN32) && !defined(__CYGWIN__) && !defined(__QNX__)
#  include <sys/syslog.h>
#جيڪڏهن_پورو

#ضمن "mosquitto_broker_internal.h"
#ضمن "tls_mosq.h"
#ضمن "util_mosq.h"
#ضمن "mosquitto/mqtt_protocol.h"


ساكن صحيح scmp_p(ثابت فراغ *p1, ثابت فراغ *p2)
{
	ثابت حرف *s1 = *(ثابت حرف **)p1;
	ثابت حرف *s2 = *(ثابت حرف **)p2;
	صحيح نتيجو;

	طالما(s1[0] && s2[0]){
		/* Sort by case insensitive part first */
		نتيجو = وڏو_اکر((بنانشان حرف)s1[0]) - وڏو_اکر((بنانشان حرف)s2[0]);
		إذا(نتيجو == 0){
			/* Case insensitive part matched, now distinguish between case */
			نتيجو = s1[0] - s2[0];
			إذا(نتيجو != 0){
				أرجع نتيجو;
			}
		}وإلا{
			/* Return case insensitive match fail */
			أرجع نتيجو;
		}
		s1++;
		s2++;
	}

	أرجع s1[0] - s2[0];
}

#جيڪڏهن_وصف_ٿيل WIN32


صحيح config__get_dir_files(ثابت حرف *include_dir, حرف ***files, صحيح *file_ڳڻپ)
{
	size_t len;
	حرف **l_files = عدم;
	صحيح l_file_ڳڻپ = 0;
	حرف **files_tmp;

	HANDLE fh;
	حرف dirpath[MAX_رستو];
	WIN32_ڳوليو_معلومات ڳوليو_معلومات;

	snprintf(dirpath, MAX_رستو, "%s\\*.conf", include_dir);
	fh = ڳوليو_پهريون_File(dirpath, &ڳوليو_معلومات);
	إذا(fh == INVALID_HANDLE_قدر){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		أرجع 1;
	}

	افعل{
		len = لڙي_ڊيگهه(include_dir)+1+لڙي_ڊيگهه(ڳوليو_معلومات.cFileName)+1;

		l_file_ڳڻپ++;
		files_tmp = mosquitto_realloc(l_files, l_file_ڳڻپ*حجم(حرف *));
		إذا(!files_tmp){
			لكل(صحيح i=0; i<l_file_ڳڻپ-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			ڳوليو_Close(fh);
			أرجع MOSQ_ERR_NOMEM;
		}
		l_files = files_tmp;

		l_files[l_file_ڳڻپ-1] = mosquitto_malloc(len+1);
		إذا(!l_files[l_file_ڳڻپ-1]){
			لكل(صحيح i=0; i<l_file_ڳڻپ-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			ڳوليو_Close(fh);
			أرجع MOSQ_ERR_NOMEM;
		}
		snprintf(l_files[l_file_ڳڻپ-1], len, "%s/%s", include_dir, ڳوليو_معلومات.cFileName);
		l_files[l_file_ڳڻپ-1][len] = '\0';
	}طالما(ڳوليو_ايندڙ_File(fh, &ڳوليو_معلومات));

	ڳوليو_Close(fh);

	إذا(l_files){
		qsort(l_files, l_file_ڳڻپ, حجم(حرف *), scmp_p);
	}
	*files = l_files;
	*file_ڳڻپ = l_file_ڳڻپ;

	أرجع 0;
}
#جيڪڏهن_پورو


#جيڪڏهن_وصف_نه_ٿيل WIN32


صحيح config__get_dir_files(ثابت حرف *include_dir, حرف ***files, صحيح *file_ڳڻپ)
{
	حرف **l_files = عدم;
	صحيح l_file_ڳڻپ = 0;
	حرف **files_tmp;
	size_t len;

	DIR *dh;
	بنية dirent *de;

	dh = opendir(include_dir);
	إذا(!dh){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		أرجع 1;
	}
	طالما((de = readdir(dh)) != عدم){
		إذا(لڙي_ڊيگهه(de->d_name) > 5){
			إذا(!strcmp(&de->d_name[لڙي_ڊيگهه(de->d_name)-5], ".conf")){
				len = لڙي_ڊيگهه(include_dir)+1+لڙي_ڊيگهه(de->d_name)+1;

				l_file_ڳڻپ++;
				files_tmp = mosquitto_realloc(l_files, (size_t)l_file_ڳڻپ*حجم(حرف *));
				إذا(!files_tmp){
					اذهب error;
				}
				l_files = files_tmp;

				l_files[l_file_ڳڻپ-1] = mosquitto_malloc(len+1);
				إذا(!l_files[l_file_ڳڻپ-1]){
					اذهب error;
				}
				snprintf(l_files[l_file_ڳڻپ-1], len, "%s/%s", include_dir, de->d_name);
				l_files[l_file_ڳڻپ-1][len] = '\0';
			}
		}
	}
	closedir(dh);

	إذا(l_files){
		qsort(l_files, (size_t)l_file_ڳڻپ, حجم(حرف *), scmp_p);
	}
	*files = l_files;
	*file_ڳڻپ = l_file_ڳڻپ;

	أرجع 0;
error:
	لكل(صحيح i=0; i<l_file_ڳڻپ-1; i++){
		mosquitto_FREE(l_files[i]);
	}
	mosquitto_FREE(l_files);
	closedir(dh);
	أرجع MOSQ_ERR_NOMEM;
}
#جيڪڏهن_پورو


