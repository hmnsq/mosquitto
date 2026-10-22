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

#اگر_تعریف_شده WIN32
#وإلا
#  include <dirent.h>
#پایان_اگر

#اگر_تعریف_نشده WIN32
#  include <strings.h>
#  include <netdb.h>
#  include <sys/socket.h>
#وإلا
#  include <winsock2.h>
#  include <ws2tcpip.h>
#پایان_اگر

#إذا defined(__HAIKU__)
#  include <syslog.h>
#وإلاإذا !defined(WIN32) && !defined(__CYGWIN__) && !defined(__QNX__)
#  include <sys/syslog.h>
#پایان_اگر

#ضمن "mosquitto_broker_internal.h"
#ضمن "tls_mosq.h"
#ضمن "util_mosq.h"
#ضمن "mosquitto/mqtt_protocol.h"


ساكن صحيح scmp_p(ثابت فراغ *p1, ثابت فراغ *p2)
{
	ثابت حرف *s1 = *(ثابت حرف **)p1;
	ثابت حرف *s2 = *(ثابت حرف **)p2;
	صحيح نتیجه;

	طالما(s1[0] && s2[0]){
		/* Sort by case insensitive part first */
		نتیجه = به_بزرگ((بیعلامت حرف)s1[0]) - به_بزرگ((بیعلامت حرف)s2[0]);
		إذا(نتیجه == 0){
			/* Case insensitive part matched, now distinguish between case */
			نتیجه = s1[0] - s2[0];
			إذا(نتیجه != 0){
				أرجع نتیجه;
			}
		}وإلا{
			/* Return case insensitive match fail */
			أرجع نتیجه;
		}
		s1++;
		s2++;
	}

	أرجع s1[0] - s2[0];
}

#اگر_تعریف_شده WIN32


صحيح config__get_dir_files(ثابت حرف *include_dir, حرف ***files, صحيح *file_شمار)
{
	size_t len;
	حرف **l_files = عدم;
	صحيح l_file_شمار = 0;
	حرف **files_tmp;

	HANDLE fh;
	حرف dirpath[MAX_مسیر];
	WIN32_پیدا_کن_داده پیدا_کن_داده;

	snprintf(dirpath, MAX_مسیر, "%s\\*.conf", include_dir);
	fh = پیدا_کن_اولی_File(dirpath, &پیدا_کن_داده);
	إذا(fh == INVALID_HANDLE_مقدار){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		أرجع 1;
	}

	افعل{
		len = طول_رشته(include_dir)+1+طول_رشته(پیدا_کن_داده.cFileName)+1;

		l_file_شمار++;
		files_tmp = mosquitto_realloc(l_files, l_file_شمار*حجم(حرف *));
		إذا(!files_tmp){
			لكل(صحيح i=0; i<l_file_شمار-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			پیدا_کن_Close(fh);
			أرجع MOSQ_ERR_NOMEM;
		}
		l_files = files_tmp;

		l_files[l_file_شمار-1] = mosquitto_malloc(len+1);
		إذا(!l_files[l_file_شمار-1]){
			لكل(صحيح i=0; i<l_file_شمار-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			پیدا_کن_Close(fh);
			أرجع MOSQ_ERR_NOMEM;
		}
		snprintf(l_files[l_file_شمار-1], len, "%s/%s", include_dir, پیدا_کن_داده.cFileName);
		l_files[l_file_شمار-1][len] = '\0';
	}طالما(پیدا_کن_بعدی_File(fh, &پیدا_کن_داده));

	پیدا_کن_Close(fh);

	إذا(l_files){
		qsort(l_files, l_file_شمار, حجم(حرف *), scmp_p);
	}
	*files = l_files;
	*file_شمار = l_file_شمار;

	أرجع 0;
}
#پایان_اگر


#اگر_تعریف_نشده WIN32


صحيح config__get_dir_files(ثابت حرف *include_dir, حرف ***files, صحيح *file_شمار)
{
	حرف **l_files = عدم;
	صحيح l_file_شمار = 0;
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
		إذا(طول_رشته(de->d_name) > 5){
			إذا(!strcmp(&de->d_name[طول_رشته(de->d_name)-5], ".conf")){
				len = طول_رشته(include_dir)+1+طول_رشته(de->d_name)+1;

				l_file_شمار++;
				files_tmp = mosquitto_realloc(l_files, (size_t)l_file_شمار*حجم(حرف *));
				إذا(!files_tmp){
					اذهب error;
				}
				l_files = files_tmp;

				l_files[l_file_شمار-1] = mosquitto_malloc(len+1);
				إذا(!l_files[l_file_شمار-1]){
					اذهب error;
				}
				snprintf(l_files[l_file_شمار-1], len, "%s/%s", include_dir, de->d_name);
				l_files[l_file_شمار-1][len] = '\0';
			}
		}
	}
	closedir(dh);

	إذا(l_files){
		qsort(l_files, (size_t)l_file_شمار, حجم(حرف *), scmp_p);
	}
	*files = l_files;
	*file_شمار = l_file_شمار;

	أرجع 0;
error:
	لكل(صحيح i=0; i<l_file_شمار-1; i++){
		mosquitto_FREE(l_files[i]);
	}
	mosquitto_FREE(l_files);
	closedir(dh);
	أرجع MOSQ_ERR_NOMEM;
}
#پایان_اگر


