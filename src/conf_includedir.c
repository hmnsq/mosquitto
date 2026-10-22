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

#که_تعریف_شوی WIN32
#وإلا
#  include <dirent.h>
#د_که_پای

#که_نه_تعریف_شوی WIN32
#  include <strings.h>
#  include <netdb.h>
#  include <sys/socket.h>
#وإلا
#  include <winsock2.h>
#  include <ws2tcpip.h>
#د_که_پای

#إذا defined(__HAIKU__)
#  include <syslog.h>
#وإلاإذا !defined(WIN32) && !defined(__CYGWIN__) && !defined(__QNX__)
#  include <sys/syslog.h>
#د_که_پای

#ضمن "mosquitto_broker_internal.h"
#ضمن "tls_mosq.h"
#ضمن "util_mosq.h"
#ضمن "mosquitto/mqtt_protocol.h"


ساكن صحيح scmp_p(ثابت فراغ *p1, ثابت فراغ *p2)
{
	ثابت حرف *s1 = *(ثابت حرف **)p1;
	ثابت حرف *s2 = *(ثابت حرف **)p2;
	صحيح پایله;

	طالما(s1[0] && s2[0]){
		/* Sort by case insensitive part first */
		پایله = لوی_توری((بینښه حرف)s1[0]) - لوی_توری((بینښه حرف)s2[0]);
		إذا(پایله == 0){
			/* Case insensitive part matched, now distinguish between case */
			پایله = s1[0] - s2[0];
			إذا(پایله != 0){
				أرجع پایله;
			}
		}وإلا{
			/* Return case insensitive match fail */
			أرجع پایله;
		}
		s1++;
		s2++;
	}

	أرجع s1[0] - s2[0];
}

#که_تعریف_شوی WIN32


صحيح config__get_dir_files(ثابت حرف *include_dir, حرف ***files, صحيح *file_شمېر)
{
	size_t len;
	حرف **l_files = عدم;
	صحيح l_file_شمېر = 0;
	حرف **files_tmp;

	HANDLE fh;
	حرف dirpath[MAX_لار];
	WIN32_پیدا_کړه_معلومات پیدا_کړه_معلومات;

	snprintf(dirpath, MAX_لار, "%s\\*.conf", include_dir);
	fh = پیدا_کړه_لومړی_File(dirpath, &پیدا_کړه_معلومات);
	إذا(fh == INVALID_HANDLE_ارزښت){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		أرجع 1;
	}

	افعل{
		len = مزي_اوږدوالی(include_dir)+1+مزي_اوږدوالی(پیدا_کړه_معلومات.cFileName)+1;

		l_file_شمېر++;
		files_tmp = mosquitto_realloc(l_files, l_file_شمېر*حجم(حرف *));
		إذا(!files_tmp){
			لكل(صحيح i=0; i<l_file_شمېر-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			پیدا_کړه_Close(fh);
			أرجع MOSQ_ERR_NOMEM;
		}
		l_files = files_tmp;

		l_files[l_file_شمېر-1] = mosquitto_malloc(len+1);
		إذا(!l_files[l_file_شمېر-1]){
			لكل(صحيح i=0; i<l_file_شمېر-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			پیدا_کړه_Close(fh);
			أرجع MOSQ_ERR_NOMEM;
		}
		snprintf(l_files[l_file_شمېر-1], len, "%s/%s", include_dir, پیدا_کړه_معلومات.cFileName);
		l_files[l_file_شمېر-1][len] = '\0';
	}طالما(پیدا_کړه_بل_File(fh, &پیدا_کړه_معلومات));

	پیدا_کړه_Close(fh);

	إذا(l_files){
		qsort(l_files, l_file_شمېر, حجم(حرف *), scmp_p);
	}
	*files = l_files;
	*file_شمېر = l_file_شمېر;

	أرجع 0;
}
#د_که_پای


#که_نه_تعریف_شوی WIN32


صحيح config__get_dir_files(ثابت حرف *include_dir, حرف ***files, صحيح *file_شمېر)
{
	حرف **l_files = عدم;
	صحيح l_file_شمېر = 0;
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
		إذا(مزي_اوږدوالی(de->d_name) > 5){
			إذا(!strcmp(&de->d_name[مزي_اوږدوالی(de->d_name)-5], ".conf")){
				len = مزي_اوږدوالی(include_dir)+1+مزي_اوږدوالی(de->d_name)+1;

				l_file_شمېر++;
				files_tmp = mosquitto_realloc(l_files, (size_t)l_file_شمېر*حجم(حرف *));
				إذا(!files_tmp){
					اذهب error;
				}
				l_files = files_tmp;

				l_files[l_file_شمېر-1] = mosquitto_malloc(len+1);
				إذا(!l_files[l_file_شمېر-1]){
					اذهب error;
				}
				snprintf(l_files[l_file_شمېر-1], len, "%s/%s", include_dir, de->d_name);
				l_files[l_file_شمېر-1][len] = '\0';
			}
		}
	}
	closedir(dh);

	إذا(l_files){
		qsort(l_files, (size_t)l_file_شمېر, حجم(حرف *), scmp_p);
	}
	*files = l_files;
	*file_شمېر = l_file_شمېر;

	أرجع 0;
error:
	لكل(صحيح i=0; i<l_file_شمېر-1; i++){
		mosquitto_FREE(l_files[i]);
	}
	mosquitto_FREE(l_files);
	closedir(dh);
	أرجع MOSQ_ERR_NOMEM;
}
#د_که_پای


