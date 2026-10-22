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

#شامل "config.h"

#شامل <ctype.h>
#شامل <limits.h>
#شامل <stdio.h>
#شامل <stdlib.h>
#شامل <string.h>
#شامل <errno.h>

#اگر_تعریف_شدہ WIN32
#ورنہ
#  include <dirent.h>
#اگر_ختم

#اگر_غیر_تعریف_شدہ WIN32
#  include <strings.h>
#  include <netdb.h>
#  include <sys/socket.h>
#ورنہ
#  include <winsock2.h>
#  include <ws2tcpip.h>
#اگر_ختم

#اگر defined(__HAIKU__)
#  include <syslog.h>
#وگرنہ_اگر !defined(WIN32) && !defined(__CYGWIN__) && !defined(__QNX__)
#  include <sys/syslog.h>
#اگر_ختم

#شامل "mosquitto_broker_internal.h"
#شامل "tls_mosq.h"
#شامل "util_mosq.h"
#شامل "mosquitto/mqtt_protocol.h"


ساکن صحیح scmp_p(ثابت خالی *p1, ثابت خالی *p2)
{
	ثابت حرف *s1 = *(ثابت حرف **)p1;
	ثابت حرف *s2 = *(ثابت حرف **)p2;
	صحیح نتیجہ;

	جب(s1[0] && s2[0]){
		/* Sort by case insensitive part first */
		نتیجہ = بڑا_حرف((غیرعلامتی حرف)s1[0]) - بڑا_حرف((غیرعلامتی حرف)s2[0]);
		اگر(نتیجہ == 0){
			/* Case insensitive part matched, now distinguish between case */
			نتیجہ = s1[0] - s2[0];
			اگر(نتیجہ != 0){
				واپس نتیجہ;
			}
		}ورنہ{
			/* Return case insensitive match fail */
			واپس نتیجہ;
		}
		s1++;
		s2++;
	}

	واپس s1[0] - s2[0];
}

#اگر_تعریف_شدہ WIN32


صحیح config__get_dir_files(ثابت حرف *include_dir, حرف ***files, صحیح *file_گنتی)
{
	size_t len;
	حرف **l_files = تہی;
	صحیح l_file_گنتی = 0;
	حرف **files_tmp;

	HANDLE fh;
	حرف dirpath[MAX_راستہ];
	WIN32_ڈھونڈو_ڈیٹا ڈھونڈو_ڈیٹا;

	snprintf(dirpath, MAX_راستہ, "%s\\*.conf", include_dir);
	fh = ڈھونڈو_پہلا_File(dirpath, &ڈھونڈو_ڈیٹا);
	اگر(fh == INVALID_HANDLE_قدر){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		واپس 1;
	}

	کرو{
		len = لڑی_لمبائی(include_dir)+1+لڑی_لمبائی(ڈھونڈو_ڈیٹا.cFileName)+1;

		l_file_گنتی++;
		files_tmp = mosquitto_realloc(l_files, l_file_گنتی*حجم(حرف *));
		اگر(!files_tmp){
			برائے(صحیح i=0; i<l_file_گنتی-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			ڈھونڈو_Close(fh);
			واپس MOSQ_ERR_NOMEM;
		}
		l_files = files_tmp;

		l_files[l_file_گنتی-1] = mosquitto_malloc(len+1);
		اگر(!l_files[l_file_گنتی-1]){
			برائے(صحیح i=0; i<l_file_گنتی-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			ڈھونڈو_Close(fh);
			واپس MOSQ_ERR_NOMEM;
		}
		snprintf(l_files[l_file_گنتی-1], len, "%s/%s", include_dir, ڈھونڈو_ڈیٹا.cFileName);
		l_files[l_file_گنتی-1][len] = '\0';
	}جب(ڈھونڈو_اگلا_File(fh, &ڈھونڈو_ڈیٹا));

	ڈھونڈو_Close(fh);

	اگر(l_files){
		qsort(l_files, l_file_گنتی, حجم(حرف *), scmp_p);
	}
	*files = l_files;
	*file_گنتی = l_file_گنتی;

	واپس 0;
}
#اگر_ختم


#اگر_غیر_تعریف_شدہ WIN32


صحیح config__get_dir_files(ثابت حرف *include_dir, حرف ***files, صحیح *file_گنتی)
{
	حرف **l_files = تہی;
	صحیح l_file_گنتی = 0;
	حرف **files_tmp;
	size_t len;

	DIR *dh;
	ساخت dirent *de;

	dh = opendir(include_dir);
	اگر(!dh){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		واپس 1;
	}
	جب((de = readdir(dh)) != تہی){
		اگر(لڑی_لمبائی(de->d_name) > 5){
			اگر(!strcmp(&de->d_name[لڑی_لمبائی(de->d_name)-5], ".conf")){
				len = لڑی_لمبائی(include_dir)+1+لڑی_لمبائی(de->d_name)+1;

				l_file_گنتی++;
				files_tmp = mosquitto_realloc(l_files, (size_t)l_file_گنتی*حجم(حرف *));
				اگر(!files_tmp){
					جاؤ error;
				}
				l_files = files_tmp;

				l_files[l_file_گنتی-1] = mosquitto_malloc(len+1);
				اگر(!l_files[l_file_گنتی-1]){
					جاؤ error;
				}
				snprintf(l_files[l_file_گنتی-1], len, "%s/%s", include_dir, de->d_name);
				l_files[l_file_گنتی-1][len] = '\0';
			}
		}
	}
	closedir(dh);

	اگر(l_files){
		qsort(l_files, (size_t)l_file_گنتی, حجم(حرف *), scmp_p);
	}
	*files = l_files;
	*file_گنتی = l_file_گنتی;

	واپس 0;
error:
	برائے(صحیح i=0; i<l_file_گنتی-1; i++){
		mosquitto_FREE(l_files[i]);
	}
	mosquitto_FREE(l_files);
	closedir(dh);
	واپس MOSQ_ERR_NOMEM;
}
#اگر_ختم


